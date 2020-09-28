/*++

Copyright (c) 2003-2009  Microsoft Corporation

Module Name:

    region.c

Abstract:

    This module implements the ASID management code.

Environment:

    Kernel mode only.

--*/

#include "ki.h"

#define ASID_COUNT 256

//
// This structure records both the ownership and use count of an ASID.
// There are two significant states:
//
// Owner is 0 -- This indicates the ASID is free.
//
// Owner is not 0 - This indicates the ASID is owned by a process. The owner
//                  value is derived from the hardware page directory which is
//                  unique per process.
//
// The RunningThreads field is a representation of how many threads
// are running using a given ASID.  This number may temporarily exceed
// the thread count due to optimistic incrementing in the ASID acquire
// logic.
//

C_ASSERT(KMAXIMUM_PROCESSORS < (1 << 14));

//
// Bits 31-14 : Owner -- Upper bits of the  physical address of the aligned
//                       hardware page directory
//
// Bits 0-13 : Running Threads -- the number of threads currently
//                                running with this ASID.
//

typedef ULONG KI_ASID_OWNER;

//
// Array of structures that track ASID ownership.  The elements in the
// array are aligned to spread the table across the cache lines.
//

typedef DECLSPEC_ALIGN(SYSTEM_CACHE_ALIGNMENT_SIZE) struct _KI_ASID_OWNER_ALIGNED {
    KI_ASID_OWNER Owner;
} KI_ASID_OWNER_ALIGNED;

KI_ASID_OWNER_ALIGNED KiAsidTable[ASID_COUNT];

//
// This value is chosen to represent a free ASID because the hardware
// page directory that is unique to each process and used to compute
// the ASID owner cannot be at physical page 0.
//

#define FREE_ASID 0

//
// Macros to compute the owner mask
//

#define KI_ASID_PAGEDIR_TO_OWNERMASK(_PageDirectory) \
    (_PageDirectory & ~(ARM_PAGE_DIRECTORY_LENGTH - 1))

#define KI_ASID_OWNER_TO_OWNERMASK(_AsidOwner) \
    KI_ASID_PAGEDIR_TO_OWNERMASK(_AsidOwner)

//
// Macro to extract running thread count from ASID owner value.
//

#define KI_ASID_RUNNING_THREADS(_AsidOwner) \
    (_AsidOwner & (ARM_PAGE_DIRECTORY_LENGTH - 1))

#if DBG

#define MAXIMUM_ASID (KeNumberProcessors * 2 + 1)

#else

#define MAXIMUM_ASID (INVALID_ASID - 1)

#endif

//
// This spinlock is used to synchronize allocation of new ASIDs only.
//

KSPIN_LOCK KiAsidLock;

//
// This value represents the next ASID to be evaluated in the allocation path.
//

ULONG KiAsidNext;

#pragma data_seg("ALMOSTRO")

BOOLEAN KiAsidA9Errata = FALSE;

#pragma data_seg()

VOID
KiTbFlushAsid (
    ULONG TargetAsid
    );

#pragma alloc_text(PAGELK, KiTbInitialize)

VOID
KiAsidVerify (
    VOID
    )
{

    ULONG Asid;
    BOOLEAN Enabled;
    ULONG PageDirectory;
    ULONG OwnerMask;

    Enabled = KeDisableInterrupts();
    Asid = _MoveFromCoprocessor(CP15_CONTEXTIDR) & CP15_CONTEXTIDR_ASID_MASK;
    PageDirectory = (ULONG) _MoveFromCoprocessor (CP15_TTBR0);
    OwnerMask = KI_ASID_PAGEDIR_TO_OWNERMASK(PageDirectory);

    NT_ASSERT(Asid < ASID_COUNT);
    NT_ASSERT(Asid != TRANSITION_ASID);
    NT_ASSERT(KI_ASID_OWNER_TO_OWNERMASK(KiAsidTable[Asid].Owner) == OwnerMask);
    NT_ASSERT(KI_ASID_RUNNING_THREADS(KiAsidTable[Asid].Owner) > 0);

    if (Enabled != FALSE) {
        _enable();
    }

    return;
}

VOID
KiAsidAlloc (
    __in PKPROCESS NewProcess
    )
{

    ULONG Asid;
    KI_ASID_OWNER OldOwner;
    ULONG OwnerMask;
    ULONG PageDirectory;
    KI_ASID_OWNER NewOwner;

    PageDirectory = (ULONG_PTR) NewProcess->PageDirectory;
    OwnerMask = KI_ASID_PAGEDIR_TO_OWNERMASK(PageDirectory);
    KeAcquireSpinLockAtDpcLevel(&KiAsidLock);

    //
    // An ASID may have been assigned to this process in the interim.
    //

    NT_ASSERT(NewProcess->Asid < ASID_COUNT);

    if (KI_ASID_OWNER_TO_OWNERMASK(KiAsidTable[NewProcess->Asid].Owner) == OwnerMask) {
        KeReleaseSpinLockFromDpcLevel(&KiAsidLock);
        return;
    }

    //
    // Loop loking for a an ASID with no running threads or a free
    // ASID.  In parallel other threads may be acquiring/releasing
    // these ASIDs.
    //

    while (TRUE) {

        //
        // Identify the next ASID to try.  Note that this will produce
        // the optimal behavior for the first 255 processes.
        //

        Asid = KiAsidNext;
        if (Asid == MAXIMUM_ASID) {
            KiAsidNext = 0;
        } else {
            KiAsidNext += 1;
        }

        //
        // Replace ASID owner with the FREE_ASID value.  If it fails,
        // move on the next ASID.
        //

        OldOwner = KiAsidTable[Asid].Owner;
        if (KI_ASID_RUNNING_THREADS(OldOwner) == 0) {
            NewOwner = FREE_ASID;
            NewOwner =
                InterlockedCompareExchangeNoFence((LONG *)&KiAsidTable[Asid].Owner,
                                                  NewOwner,
                                                  OldOwner);

            if (NewOwner == OldOwner) {
                break;
            }
        }
    }

    //
    // The ASID has been marked free but no one can allocate it while
    // this code is within the spinlock.  However a thread from a
    // process with a stale ASID may acquire/release the ASID at any
    // time.
    //

    NT_ASSERT(Asid < ASID_COUNT);
    NT_ASSERT(Asid != TRANSITION_ASID);

    //
    // We reserved an ASID but it may have mappings from the previous
    // process that need to be cleared.
    //

    KiTbFlushAsid(Asid);

    //
    // Set ASID owner to reflect the new process/thread.  The
    // reference count may be be artificially inflated by spurious
    // acquires.
    //

    while (TRUE) {

        OldOwner = KiAsidTable[Asid].Owner;
        NewOwner = OwnerMask | KI_ASID_RUNNING_THREADS(OldOwner);
        NewOwner =
            InterlockedCompareExchangeNoFence((LONG *)&KiAsidTable[Asid].Owner,
                                              NewOwner,
                                              OldOwner);

        if (NewOwner == OldOwner) {
            break;
        }
    }

    //
    // Update the process data structure to reflect the ASID
    // allocation.
    //

    NewProcess->Asid = Asid;

    KeReleaseSpinLockFromDpcLevel(&KiAsidLock);
    return;
}

KINLINE
VOID
KiAsidAcquire (
    __in PKPROCESS NewProcess
    )
{

    ULONG Asid;
    ULONG Context;
    KI_ASID_OWNER NewOwner;
    ULONG OwnerMask;
    ULONG PageDirectory;

    PageDirectory = (ULONG_PTR) NewProcess->PageDirectory;
    OwnerMask = KI_ASID_PAGEDIR_TO_OWNERMASK(PageDirectory);

    NT_ASSERT(OwnerMask != 0);

    while (TRUE) {

        //
        // In this fast path, we assume that the new process's ASID is
        // valid.  This should be the case the vast majority of the
        // time. The intent is to avoid the additional loads
        // associated with the setup for InterlockedCompareExchange.
        //

        Asid = NewProcess->Asid;

        NT_ASSERT(Asid < ASID_COUNT);

        //
        // Acquire the ASID that was assigned to this process
        //

        NewOwner = InterlockedIncrementNoFence((LONG *)&KiAsidTable[Asid].Owner);

        //
        // Confirm that we own this ASID, and break out to set the
        // TTB/ASID.
        //

        if (KI_ASID_OWNER_TO_OWNERMASK(NewOwner) == OwnerMask) {

            NT_ASSERT(Asid != INVALID_ASID);

            break;
        }

        //
        // We acquired the wrong ASID because this process has or had
        // stale information. Release the ASID we erroneously
        // acquired.
        //

        NT_ASSERT(KI_ASID_RUNNING_THREADS(KiAsidTable[Asid].Owner) > 0);
        InterlockedDecrementNoFence((LONG *)&KiAsidTable[Asid].Owner);

        //
        // Attempt to Allocate a new ASID.  Either this thread will
        // allocate the ASID or find that another thread has allocated
        // it in the interim.
        //
        // N.B. Note that allocating the ASID does not take a reference to it.
        //

         KiAsidAlloc(NewProcess);
    }

    Context = Asid;

#if DBG

    Context |= ((ULONG_PTR) NewProcess & CP15_CONTEXTIDR_CONTEXT_MASK);

#endif

    //
    // Switch to the new ASID.
    //
    // _ARM_WORKITEM_ not clear this meets the ASID change requirements
    //

    if (KiAsidA9Errata != FALSE) {
        _DataSynchronizationBarrier();
        _MoveToCoprocessor(TRANSITION_ASID, CP15_CONTEXTIDR);
        _InstructionSynchronizationBarrier();
        _MoveToCoprocessor(PageDirectory, CP15_TTBR0);
        _InstructionSynchronizationBarrier();
        _DataSynchronizationBarrier();
    } else {
        _MoveToCoprocessor(TRANSITION_ASID, CP15_CONTEXTIDR);
        _InstructionSynchronizationBarrier();
        _MoveToCoprocessor(PageDirectory, CP15_TTBR0);
        _InstructionSynchronizationBarrier();
    }

    _MoveToCoprocessor(Context, CP15_CONTEXTIDR);
    _InstructionSynchronizationBarrier();
    return;
}

VOID
KiAsidRelease (
     __in PKPROCESS OldProcess
    )
{

    ULONG Asid;

#if DBG

    ULONG OwnerMask;

#endif

    //
    // This process must have a valid ASID since ASIDs are only stolen
    // from processes that are not scheduled.
    //

    Asid = OldProcess->Asid;

    NT_ASSERT(Asid < ASID_COUNT);

#if DBG

    OwnerMask = KI_ASID_PAGEDIR_TO_OWNERMASK((ULONG_PTR)OldProcess->PageDirectory);

    NT_ASSERT(KI_ASID_OWNER_TO_OWNERMASK(KiAsidTable[Asid].Owner) == OwnerMask);
    NT_ASSERT(KI_ASID_RUNNING_THREADS(KiAsidTable[Asid].Owner) > 0);

#endif

    InterlockedDecrementNoFence((LONG *)&KiAsidTable[Asid].Owner);
    return;
}

VOID
KxSwapProcess (
    __in PKPROCESS NewProcess,
    __in PKPROCESS OldProcess
    )

/*++

Routine Description:

    This function switches the current address space to the specified process
    by selecting the region (ASID) and page directory of that process.

    The region associated with the new process may be invalid because
    the process is new or if the regoin space is stale because the
    region space wrapped since this process last executed on any
    processor.  In that scenario, a new region must be assigned to this
    process before continuing.

    This function performs the machine specific operations associated with
    swapping from one process to another.

    N.B. This code is executed at DPC level.

Arguments:

    NewProcess - Supplies a pointer to the new process object.

    OldProcess - Supplies a pointer to the old process object.

Return Value:

    None.

--*/

{

    NT_ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    KiAsidAcquire(NewProcess);
    KiAsidRelease(OldProcess);
    return;
}

VOID
KiAsidInitialize (
    __in ULONG Number,
    __in PKPROCESS Process
    )
/*++

Routine Description:

    Initialize region (ASID) support.

Arguments:

    Number - Supplies a processor index

    Process - Supplies a KPROCESS pointer

Return Value:

    None.

--*/

{

    ULONG HardwarePageDirectory;
    ARM_IDCODE Id;
    ULONG Index;

    if (Number == 0) {

        //
        // Detect Cortex A9 errata #754322
        // "Possible faulty MMU translations following an ASID switch"
        //

        Id.Ulong = _MoveFromCoprocessor(CP15_MIDR);
        if ((Id.Model == 0x0C09) &&
            (Id.Implementer == 'A') &&
            (Id.Revision >= 2)) {

            KiAsidA9Errata = TRUE;
        }

        KeInitializeSpinLock(&KiAsidLock);
        for (Index = 0; Index < ASID_COUNT; Index += 1) {
            KiAsidTable[Index].Owner = FREE_ASID;
        }
        //
        // Initialize the process structure so that the invariants are
        // maintained even for the system process.
        //
        // N.B. Should be able to remove similar code from MM but it
        //      iss ambiguous whether the two code paths are always
        //      referring to the same process (system, idle).
        //

        Process->Asid = INVALID_ASID;
        HardwarePageDirectory = (ULONG) _MoveFromCoprocessor (CP15_TTBR0);
        HardwarePageDirectory &= CP15_TTBRx_PD_MASK;
        HardwarePageDirectory |= KiTbrConfiguration;
        Process->PageDirectory = (PVOID) HardwarePageDirectory;
    }

    //
    // Attach the current thread to the idle process ASID so that the accounting
    // is correct when this thread subsequently attaches to the system process.
    //

    NT_ASSERT((Process == &PsIdleProcess->Pcb) || (PsIdleProcess == NULL));

    KiAsidAcquire(Process);
    KeFlushCurrentTb();

    //
    // If this mechananism is working properly, the system process
    // should receive ASID 0.
    //

    NT_ASSERT(Process->Asid == 0);
    return;
}

