/*++

Copyright (c) 2000-2011  Microsoft Corporation

Module Name:

    flushtb.c

Abstract:

    This module implements machine dependent functions to flush the TB
    for an ARM system.

    The ARM processor family supports global and local (non-global) TB
    entries.  Non-global TB entries are tagged with the ASID active
    when the TB entry was filled.  Each running process is assigned a
    unique ASID.

        - Most kernel, driver (non-session), and other global
          data/code are marked via the page tables as 'global'.

        - Process-specific ranges are marked non-global.

        - Session-specific ranges are marked non-global.  This
          requires special handling to ensure that session TB flushes
          affect all processes attached to the session.  This is
          accomplished by using TB ops that ignore ASID tagging.

    The ARMv7 MP extensions added broadcast TB operations to the
    architecture.  This code assumes that the known processors are all
    within the inner sharable domain and thus will receive those
    broadcasts.

    MM distinguishes between shooting down a TB for a process and all
    processors and the supporting kernel APIs reflect this.  On
    architectures without ASIDs and broadcast TB operations, this
    allows for reducing the IPI span by targetting only the processors
    running the targeted process.  This code uses broadcast operations
    to achieve the same end as ASID tagged data may be present on any
    core on which the process has historically run.

Environment:

    Kernel mode only.

--*/

#include "ki.h"

#if TB_DBG

#define LOG_TB_OP(_Arg, _Op) KiTbLog(_Arg, _Op)

#else

#define LOG_TB_OP(_Arg, _Op)

#endif

#define TB_LOG_SIZE 0x8000

//
// Variables to support circular TB operation log
//

#pragma bss_seg("CACHEALIGNZ")

DECLSPEC_CACHEALIGN LONG KiTbIndex;

#pragma bss_seg()

struct {
    ULONG Cpu;
    ULONG PteIndex;
    char *Op;
    ULONG Argument;
} KiTbOps[TB_LOG_SIZE];

#pragma bss_seg("ALMOSTROZ")

//
// Ths variable controls whether TLBMVAIS use is disabled in favor of
// TLBMVAAIS.
//

BOOLEAN KiTbDisableSingleByAsidBroadcast;

//
// Ths variable controls whether TLBIASIDIS use is disabled in favor of
// TLBIALLIS.
//

BOOLEAN KiTbDisableProcessByAsidBroadcast;

#pragma bss_seg()

#if DBG

#define VERIFY_ASID_OWNERSHIP() KiAsidVerify()

VOID
KiAsidVerify (
    VOID
    );

#else

#define VERIFY_ASID_OWNERSHIP()

#endif

#pragma alloc_text(PAGELK, KiTbInitialize)

VOID
KiTbLog (
    __in ULONG Argument,
    __in char *Operation
    )

/*++

Routine Description:

    This routine logs a TB operations to a circular buffer.

Arguments:

    Argument - Supplies the argument to the TB operation.  The argument
        meaning is TB operation specific.

    Operation - Supplies a constant string pointer describing the TB operation.

Return Value:

    None.

--*/

{

    ULONG Index;

    Index = InterlockedIncrement(&KiTbIndex);
    Index &= (TB_LOG_SIZE - 1);
    KiTbOps[Index].Cpu = KeGetCurrentProcessorIndex();
    // KiTbOps[Index].PteIndex = MiPteIndex;
    KiTbOps[Index].Op = Operation;
    KiTbOps[Index].Argument = Argument;
    return;
}

KINLINE
VOID
KiTbBarrier (
    VOID
    )

/*++

Routine Description:

    This routine issues the barrier sequence to make a TB operation effective.

Arguments:

    None.

Return Value:

    None.

--*/

{
    //
    // A DSB is needed here to ensure the completion of the TB invalidate
    // operation as per the ARM ARM manual section B3.10.5.
    //
    // And an ISB is needed to ensure instruction prefetch and the data
    // table walker see the change.
    //

    _SyncPte ();

    //
    // Note we are ok even if a context swap happens anywhere in the
    // sequence of PTE editing, TB flushing and barrier issue.  This
    // is because all interrupts (including rescheduling ones) generate
    // an implicit ISB.  And the ARM swap context code issues an
    // explicit DSB.
    //

    LOG_TB_OP(0, "BPIALL;DSB;ISB");
    return;
}

KINLINE
ULONG
KiTbSnapCurrentAsid (
    VOID
   )

/*++

Routine Description:

    This routine fetches the current ASID for use in subsequent TLB
    management.  The ASID may be stale by the time it is used if an
    ASID reset sequence has taken place.  In that scenario, the TLB
    has already been invalidated and the operations targeted at the
    wrong ASID will do no harm.

Arguments:

    None.

Return Value:

    None.

--*/

{

    ULONG Asid;

    Asid = _MoveFromCoprocessor(CP15_CONTEXTIDR) & CP15_CONTEXTIDR_ASID_MASK;
    VERIFY_ASID_OWNERSHIP();
    return Asid;
}

KINLINE
VOID
KiTbFlushCurrent (
    VOID
    )

/*++

Routine Description:

    This routine flushes the TLB on the current processor.

Arguments:

    None.

Return Value:

    None.

--*/

{
    _MoveToCoprocessor(0, CP15_TLBIALL);
    LOG_TB_OP(0, "TLBIALL");
    return;
}

KINLINE
VOID
KiTbFlushBroadcast (
    VOID
    )

/*++

Routine Description:

    This routine flushes the TLB on all processors within the inner
    sharable domain.

Arguments:

    None.

Return Value:

    None.

--*/

{

    _MoveToCoprocessor(0, CP15_TLBIALLIS);
    LOG_TB_OP(0, "TLBIALLIS");
    return;
}

KINLINE
VOID
KiTbFlushProcessByAsidCurrent (
    ULONG Asid
    )

/*++

Routine Description:

    This routine flushes the TLB on the current processor of TB
    entries tagged with the specified ASID.

Arguments:

    Asid - Supplies the ASID identifier of the entries targeted.

Return Value:

    None.

--*/

{

    NT_ASSERT(Asid != TRANSITION_ASID);

    _MoveToCoprocessor(Asid, CP15_TLBIASID);
    LOG_TB_OP(Asid, "TLBIASID");
    return;
}

KINLINE
VOID
KiTbFlushProcessByAsidBroadcast (
    __in ULONG Asid
    )

/*++

Routine Description:

    This routine flushes the TLB on all processors in the innser
    sharable domain of TB entries tagged with the
    specified ASID.

Arguments:

    Asid - Supplies the ASID identifier of the entries targeted.

Return Value:

    None.

--*/

{

    NT_ASSERT(Asid != TRANSITION_ASID);

    _MoveToCoprocessor(Asid, CP15_TLBIASIDIS);
    LOG_TB_OP(Asid, "TLBIASIDIDIS");
    return;
}

#define PDES_PER_PAGE_TABLE     4

#define VA_IS_PAGE_TABLE(VA)   (((VA) >= PTE_BASE) && ((VA) <= PTE_TOP))

KINLINE
VOID
KiTbFlushSingleByAsidCurrent (
    __in PVOID Virtual,
    __in ULONG Asid
    )

/*++

Routine Description:

    This routine flushes a single TLB entry by VA in the specified
    address space on the current processor.  This applies to global TB
    entries as well as non-global entries tagged with the targeted
    ASID.

Arguments:

    Virtual - Supplies the virtual address of the TB to be flushed.

    Asid - Supplies the ASID identifier of the entry targeted.

Return Value:

    None.

--*/

{

    ULONG Count;
    ULONG LeafMva;
    ULONG Mva;

    NT_ASSERT(Asid < TRANSITION_ASID);

    Mva = ((ULONG) PAGE_ALIGN(Virtual) | Asid);

    //
    // Flush the requested virtual address.
    //

    _MoveToCoprocessor(Mva, CP15_TLBIMVA);

    if (VA_IS_PAGE_TABLE (Mva)) {

        //
        // ARM maintains a *virtually tagged* page table cache in hardware
        // and since we are changing a page table translation (as opposed
        // to a leaf translation) we must inform it (by flushing a leaf
        // translation from each of the 4 1K subPDEs) to get rid of any
        // opportunistic entries it may be caching.
        //

        LeafMva = (ULONG) MmGetBaseLeafVa ((PVOID)Mva);

        LeafMva |= Asid;

        for (Count = 0; Count < PDES_PER_PAGE_TABLE; Count += 1) {

            _MoveToCoprocessor(LeafMva, CP15_TLBIMVA);

            LeafMva += ((1 << PDI_SHIFT) / PDES_PER_PAGE_TABLE);
        }
    }

    LOG_TB_OP(Mva, "TLBIMVA");
    return;
}

KINLINE
VOID
KiTbFlushSingleByAsidBroadcast (
    __in PVOID Virtual,
    __in ULONG Asid
    )

/*++

Routine Description:

    This routine flushes a single TLB entry by VA in the specified
    address space on all processors in the inner-sharable domain.
    This applies to global TB entries as well as non-global entries
    tagged with the targeted ASID.

Arguments:

    Virtual - Supplies the virtual address of the TB to be flushed.

    Asid - Supplies the ASID identifier of the entry targeted.

Return Value:

    None.

--*/

{

    ULONG Count;
    ULONG LeafMva;
    ULONG Mva;

    NT_ASSERT(Asid < TRANSITION_ASID);

    Mva = ((ULONG) PAGE_ALIGN(Virtual) | Asid);

    //
    // Flush the requested virtual address.
    //

    _MoveToCoprocessor(Mva, CP15_TLBIMVAIS);

    if (VA_IS_PAGE_TABLE (Mva)) {

        //
        // ARM maintains a *virtually tagged* page table cache in hardware
        // and since we are changing a page table translation (as opposed
        // to a leaf translation) we must inform it (by flushing a leaf
        // translation from each of the 4 1K subPDEs) to get rid of any
        // opportunistic entries it may be caching.
        //

        LeafMva = (ULONG) MmGetBaseLeafVa ((PVOID)Mva);

        LeafMva |= Asid;

        for (Count = 0; Count < PDES_PER_PAGE_TABLE; Count += 1) {

            _MoveToCoprocessor(LeafMva, CP15_TLBIMVAIS);

            LeafMva += ((1 << PDI_SHIFT) / PDES_PER_PAGE_TABLE);
        }
    }

    LOG_TB_OP(Mva, "TLBIMVAIS");
    return;
}

KINLINE
VOID
KiTbFlushSingleAllAsidCurrent (
    __in PVOID Virtual
    )

/*++

Routine Description:

    This routine flushes a single TLB entry by VA regardless of ASID
    on the current processsor.

Arguments:

    Virtual - Supplies the virtual address of the TB to be flushed.

Return Value:

    None.

--*/

{

    ULONG Count;
    ULONG LeafMva;
    ULONG Mva;

    Mva = (ULONG) PAGE_ALIGN(Virtual);

    //
    // Flush the requested virtual address.
    //

    _MoveToCoprocessor(Mva, CP15_TLBIMVAA);

    if (VA_IS_PAGE_TABLE (Mva)) {

        //
        // ARM maintains a *virtually tagged* page table cache in hardware
        // and since we are changing a page table translation (as opposed
        // to a leaf translation) we must inform it (by flushing a leaf
        // translation from each of the 4 1K subPDEs) to get rid of any
        // opportunistic entries it may be caching.
        //

        LeafMva = (ULONG) MmGetBaseLeafVa ((PVOID)Mva);

        for (Count = 0; Count < PDES_PER_PAGE_TABLE; Count += 1) {

            _MoveToCoprocessor(LeafMva, CP15_TLBIMVAA);

            LeafMva += ((1 << PDI_SHIFT) / PDES_PER_PAGE_TABLE);
        }
    }

    LOG_TB_OP((ULONG)(ULONG_PTR)Virtual, "TLBIMVAA");
    return;
}

KINLINE
VOID
KiTbFlushSingleAllAsidBroadcast (
    __in PVOID Virtual
    )

/*++

Routine Description:

    This routine flushes a single TLB entry by VA regardless of ASID
    on processors in the inner-sharable domain

Arguments:

    Virtual - Supplies the virtual address of the TB to be flushed.

Return Value:

    None.

--*/

{

    ULONG Count;
    ULONG LeafMva;
    ULONG Mva;

    Mva = (ULONG) PAGE_ALIGN(Virtual);

    //
    // Flush the requested virtual address.
    //

    _MoveToCoprocessor(Mva, CP15_TLBIMVAAIS);

    if (VA_IS_PAGE_TABLE (Mva)) {

        //
        // ARM maintains a *virtually tagged* page table cache in hardware
        // and since we are changing a page table translation (as opposed
        // to a leaf translation) we must inform it (by flushing a leaf
        // translation from each of the 4 1K subPDEs) to get rid of any
        // opportunistic entries it may be caching.
        //

        LeafMva = (ULONG) MmGetBaseLeafVa ((PVOID)Mva);

        for (Count = 0; Count < PDES_PER_PAGE_TABLE; Count += 1) {

            _MoveToCoprocessor(LeafMva, CP15_TLBIMVAAIS);

            LeafMva += ((1 << PDI_SHIFT) / PDES_PER_PAGE_TABLE);
        }
    }

    LOG_TB_OP((ULONG)(ULONG_PTR)Virtual, "TLBIMVAAIS");
    return;
}

KINLINE
VOID
KiTbFlushRangeByAsidBroadcast (
    __in KTB_FLUSH_VA Virtual,
    __in ULONG TargetAsid
    )

/*++

Routine Description:

    This routine flushes a range of TLB entries by VA in the specified
    address space on all processors in the inner-shareable domain.
    This applies to global TB entries as well as non-global entries
    tagged with the targeted ASID.

Arguments:

    Virtual - Supplies a virtual address range.  See KeFlushMultipleRangeTb()
        for a description of the range format.

    TargetAsid - Supplies the ASID identifier of the entries targeted.

Return Value:

    None.

--*/

{

    ULONG_PTR Count;
    ULONG_PTR PageSize;
    ULONG_PTR Va;

    KiTbFlushSingleByAsidBroadcast(Virtual.u1.Va, TargetAsid);
    Va = Virtual.u1.VaLong;

    Count = Virtual.u1.NumberOfEntries;
    if (Virtual.u1.PageSize == 0) {
        PageSize = PAGE_SIZE;
    } else {
        PageSize = LARGE_PAGE_SIZE;
    }

    while (Count != 0) {
        Va += PageSize;
        KiTbFlushSingleByAsidBroadcast((PVOID)Va, TargetAsid);
        Count -= 1;
    }

    return;
}

KINLINE
VOID
KiTbFlushRangeAllAsidCurrent (
    __in KTB_FLUSH_VA Virtual
    )

/*++

Routine Description:

    This routine flushes a range of TLB entry by VA regardless of ASID
    on the current processsor.

Arguments:

    Virtual - Supplies a virtual address range.  See KeFlushMultipleRangeTb()
        for a description of the range format.

Return Value:

    None.

--*/

{

    ULONG_PTR Count;
    ULONG_PTR PageSize;
    ULONG_PTR Va;

    KiTbFlushSingleAllAsidCurrent(Virtual.u1.Va);
    Va = Virtual.u1.VaLong;

    Count = Virtual.u1.NumberOfEntries;
    if (Virtual.u1.PageSize == 0) {
        PageSize = PAGE_SIZE;
    } else {
        PageSize = LARGE_PAGE_SIZE;
    }

    while (Count != 0) {
        Va += PageSize;
        KiTbFlushSingleAllAsidCurrent((PVOID)Va);
        Count -= 1;
    }

    return;
}

KINLINE
VOID
KiTbFlushRangeAllAsidBroadcast (
    __in KTB_FLUSH_VA Virtual
    )

/*++

Routine Description:

    This routine flushes a range of TLB entry by VA regardless of ASID
    on all processors in the innser-shareable domain.

Arguments:

    Virtual - Supplies a virtual address range.  See KeFlushMultipleRangeTb()
        for a description of the range format.

Return Value:

    None.

--*/

{

    ULONG_PTR Count;
    ULONG_PTR PageSize;
    ULONG_PTR Va;

    KiTbFlushSingleAllAsidBroadcast(Virtual.u1.Va);
    Va = Virtual.u1.VaLong;

    Count = Virtual.u1.NumberOfEntries;
    if (Virtual.u1.PageSize == 0) {
        PageSize = PAGE_SIZE;
    } else {
        PageSize = LARGE_PAGE_SIZE;
    }

    while (Count != 0) {
        Va += PageSize;
        KiTbFlushSingleAllAsidBroadcast((PVOID)Va);
        Count -= 1;
    }

    return;
}

KINLINE
VOID
KiTbFlushMultipleByAsidBroadcast (
    __in ULONG Number,
    __in_ecount(Number) KTB_FLUSH_VA Virtual[],
    __in ULONG TargetAsid
   )

/*++

Routine Description:

    This routine flushes multiple TB entries by VA in the specified
    address space on all processors in the inner-shareable domain.
    This applies to global TB entries as well as non-global entries
    tagged with the targeted ASID.

Arguments:

    Number - Supplies the number of TB entries to flush.

    Virtual - Supplies a pointer to an array of virtual addresses that
        are within the pages whose translation buffer entries are to be
        flushed.

    Range - Indicates whether Virtual should be interpreted as a flat
        address array or as a list of ranges.  This parameter is evaluated
        at compile time.

    TargetAsid - Supplies the ASID identifier of the entries targeted.

Return Value:

    None.

--*/

{

    PKTB_FLUSH_VA End;

    End = Virtual + Number;
    do {
        KiTbFlushRangeByAsidBroadcast(*Virtual, TargetAsid);
        Virtual += 1;
    } while (Virtual < End);

    return;
}

KINLINE
VOID
KiTbFlushMultipleAllAsidBroadcast (
    __in ULONG Number,
    __in_ecount(Number) KTB_FLUSH_VA Virtual[]
   )

/*++

Routine Description:

    This routine flushes multiple TB entries by VA regardless of ASID
    on all processors in the inner-shareable domain.

Arguments:

    Number - Supplies the number of TB entries to flush.

    Virtual - Supplies a pointer to an array of virtual addresses that
        are within the pages whose translation buffer entries are to be
        flushed.

    Range - Indicates whether Virtual should be interpreted as a flat
        address array or as a list of ranges.  This parameter is evaluated
        at compile time.

Return Value:

    None.

--*/

{

    PKTB_FLUSH_VA End;

    End = Virtual + Number;
    do {
        KiTbFlushRangeAllAsidBroadcast(*Virtual);
        Virtual += 1;
    } while (Virtual < End);

    return;
}

KINLINE
VOID
KiTbFlushProcessBroadcast (
    VOID
    )

/*++

Routine Description:

    This routine flushes the TLB entries associated with the current process
    on the current processor or all processors.

Arguments:

    None.

Return Value:

    None.

--*/

{

    BOOLEAN Enabled;
    ULONG TargetAsid;

    //
    // A DSB is needed here to ensure the hardware page walker cannot see
    // (ie, speculate back in) the old PTE value(s) after the actual TLB flush.
    //

    _DataSynchronizationBarrier();
    VERIFY_ASID_OWNERSHIP();
    if (KiTbDisableProcessByAsidBroadcast != FALSE) {
        KiTbFlushBroadcast();
    } else {
        Enabled = KeDisableInterrupts();
        TargetAsid = KiTbSnapCurrentAsid();
        KiTbFlushProcessByAsidBroadcast(TargetAsid);
        if (Enabled != FALSE) {
            _enable();
        }
    }

    KiTbBarrier();
    return;
}

extern
KINLINE
VOID
KiTbSendIpiIfNecessary (
    __in KTBFLUSH_TARGET ProcessorSet
    )

/*++

Routine Description:

    This function sends dummy IPIs to all affected processors
    for a flush operation. It is only necessary for working around
    errata on the Cortex-A15.

Arguments:

    ProcessorSet - Supplies a value that determines which
        translation buffers are to be flushed.

Return Value:

    None.

--*/

{

    KAFFINITY_EX Affinity;
    KAFFINITY_EX DeepIdleProcessors;
    KIRQL OldIrql;
    PKPRCB Prcb;
    PKPROCESS Process;

    //
    // Skip if it isn't required.
    //

    if (KiTbSendIpiForBroadcastFlushes == FALSE) {
        return;
    }

    //
    // Perform a dummy TLBIMVAIS, needed for the A15 errata workaround
    //

    _MoveToCoprocessor(0, CP15_TLBIMVAIS);
    _DataSynchronizationBarrier();

    //
    // Compute the target set of processors, disable context switching,
    // and send the flush entire parameters to the target processors,
    // if any, for execution.
    //

    OldIrql = KeRaiseIrqlToSynchLevel();

    Prcb = KeGetCurrentPrcb();

    if (ProcessorSet == FlushSubsetProcessors) {
        Process = Prcb->CurrentThread->ApcState.Process;
        KeCopyAffinityEx(&Affinity, (PKAFFINITY_EX)&Process->ActiveProcessors);
    } else {
        KeCopyAffinityEx(&Affinity, (PKAFFINITY_EX)&KeActiveProcessors);
    }

    //
    // Avoid waking idle processors by removing them from the
    // target affinity mask, along with the current processor.
    //
    // N.B. PoCopyDeepIdleMask relies on KeNodeBlock being initialized
    //

    if (KeNodeBlock[0] != NULL) {
        PoCopyDeepIdleMask(&DeepIdleProcessors);
        if (KeSubtractAffinityEx(&Affinity, &DeepIdleProcessors, &Affinity) == FALSE) {
            return;
        }
    }
    KeRemoveProcessorAffinityEx(&Affinity, Prcb->Number);

    //
    // Send request to target processors. This is just a NOP IPI, but it
    // is sufficient to ensure that a CLREX and a DSB are executed on the
    // targets.
    //

    KiIpiSendRequestEx(Prcb,
                       IpiAffinity,
                       &Affinity,
                       0,
                       0,
                       KiTbIpiTypeToSendForBroadcast,
                       NULL,
                       NULL);

    //
    // Lower IRQL to its previous value.
    //

    KeLowerIrql(OldIrql);
    return;
}

//
// N.B. Routines beyond this point are exported outside of this file.
//      This is not otherwise obvious due to inconsistent naming
//      conventions.
//

extern
VOID
KiTbFlushAsid (
    ULONG TargetAsid
    )

/*++

Routine Description:

    This routine flushes the TLB entries associated with the current process
    on the current processor or all processors.

Arguments:

    None.

Return Value:

    None.

--*/

{

    //
    // A DSB is needed here to ensure the hardware page walker cannot see
    // (ie, speculate back in) the old PTE value(s) after the actual TLB flush.
    //

    _DataSynchronizationBarrier();

    //
    // N.B. This code is running on the ASID of the old thread.
    //

    if (KiTbDisableProcessByAsidBroadcast != FALSE) {
        KiTbFlushBroadcast();
    } else {
        KiTbFlushProcessByAsidBroadcast(TargetAsid);
    }

    KiTbBarrier();
    KiTbSendIpiIfNecessary(FlushAllProcessors);
    return;
}

extern
KINLINE
VOID
KxFlushMultipleTb (
    __in KTBFLUSH_TYPE FlushType,
    __in ULONG Number,
    __in_ecount(Number) KTB_FLUSH_VA Virtual[],
    __in KTBFLUSH_TARGET ProcessorSet
    )

/*++

Routine Description:

    This function flushes multiple entries from the translation buffer
    on all processors that are currently running threads which are
    children of the current process or flushes multiple entries from
    the translation buffer on all processors in the host configuration.

Arguments:

    FlushType - Address space being flushed.

    Number - Supplies the number of TB entries to flush.

    Virtual - Supplies a pointer to an array of virtual addresses that
        are within the pages whose translation buffer entries are to be
        flushed.

    ProcessorSet - Supplies which processors'
        translation buffers are to be flushed.

Return Value:

    None.

--*/

{

    BOOLEAN Enabled;
    ULONG TargetAsid;

    UNREFERENCED_PARAMETER(ProcessorSet);

    NT_ASSERT(((FlushType != FlushUser) && (ProcessorSet != FlushSubsetProcessors)) ||
              ((FlushType == FlushUser) && (ProcessorSet == FlushSubsetProcessors)));

    //
    // A DSB is needed here to ensure the hardware page walker cannot see
    // (ie, speculate back in) the old PTE value(s) after the actual TLB flush.
    //

    _DataSynchronizationBarrier();

    //
    // Perform appropriate flushes depending on the flush type.
    //

    VERIFY_ASID_OWNERSHIP();
    if (FlushType == FlushUser) {

        //
        // Flush the specified mappings in this process.  These
        // mappings will be tagged with this process's ASID.
        //

        if (KiTbDisableSingleByAsidBroadcast != FALSE) {
            KiTbFlushMultipleAllAsidBroadcast(Number, Virtual);

        } else {
            Enabled = KeDisableInterrupts();
            TargetAsid = KiTbSnapCurrentAsid();
            KiTbFlushMultipleByAsidBroadcast(Number, Virtual, TargetAsid);
            if (Enabled != FALSE) {
                _enable();
            }
        }
    } else if (FlushType == FlushKernel) {

        //
        // Flush the specified kernel mappings across all processors.
        //

        KiTbFlushMultipleAllAsidBroadcast(Number, Virtual);
    } else if (FlushType == FlushSession) {

        //
        // Flush the specified session mappings across all processors.
        // The targetted mappings may have been tagged with multiple
        // ASIDs.
        //

        KiTbFlushMultipleAllAsidBroadcast(Number, Virtual);
    } else {

        NT_ASSERT((FlushType == FlushUser) ||
                  (FlushType == FlushKernel) ||
                  (FlushType == FlushSession));
    }

    KiTbBarrier();
    KiTbSendIpiIfNecessary(ProcessorSet);

    return;
}

extern
KINLINE
VOID
KiFlushRangeTb (
    __in KTB_FLUSH_VA Virtual
    )

/*++

Routine Description:

    This function performs a local flush of the VA described by a single range.

Arguments:

    Virtual - Supplies a virtual address range.  See KeFlushMultipleRangeTb()
        for a description of the range format.

Return Value:

    None.

--*/

{

    //
    // A DSB is needed here to ensure the hardware page walker cannot see
    // (ie, speculate back in) the old PTE value(s) after the actual TLB flush.
    //

    _DataSynchronizationBarrier();
    VERIFY_ASID_OWNERSHIP();

    //
    // If the VA refers to session space, it may have been
    // tagged with multiple ASIDs.
    //

    KiTbFlushRangeAllAsidCurrent(Virtual);
    KiTbBarrier();
    return;
}

extern
KINLINE
VOID
KiFlushSingleTb (
    __in PVOID Virtual
    )

/*++

Routine Description:

    This routine flushes a single TLB entry by VA on the current processor.

Arguments:

    None.

Return Value:

    None.

--*/

{

    //
    // A DSB is needed here to ensure the hardware page walker cannot see
    // (ie, speculate back in) the old PTE value(s) after the actual TLB flush.
    //

    _DataSynchronizationBarrier();
    VERIFY_ASID_OWNERSHIP();

    //
    // No information is provided as to the target address space.
    // Lacking this information session space must be assumed.  If the
    // VA refers to session space, it may have been tagged with
    // multiple ASIDs.
    //

    KiTbFlushSingleAllAsidCurrent(Virtual);

    KiTbBarrier();
}

extern
KINLINE
VOID
KxFlushSingleTb (
    __in PVOID Virtual,
    __in KTBFLUSH_TARGET ProcessorSet
    )

/*++

Routine Description:

    This function flushes a single entry from the translation buffer
    on all processors that are currently running threads which are
    children of the current process or flushes a single entry from
    the translation buffer on all processors in the host configuration.

    N.B. The specified translation entry on all processors in the host
         configuration is always flushed since PowerPC TB is tagged by
         VSID and translations are held across context switch boundaries.

Arguments:

    Virtual - Supplies a virtual address that is within the page whose
        translation buffer entry is to be flushed.

    ProcessorSet - Supplies which processors'
        translation buffers are to be flushed.

Return Value:

    None

--*/

{

    BOOLEAN Enabled;
    ULONG TargetAsid;

    //
    // A DSB is needed here to ensure the hardware page walker cannot see
    // (ie, speculate back in) the old PTE value(s) after the actual TLB flush.
    //

    _DataSynchronizationBarrier();
    VERIFY_ASID_OWNERSHIP();
    if (ProcessorSet != FlushSubsetProcessors) {

        //
        // The VA refers to kernel or session space.  If the VA
        // refers to session space, it may have been tagged with
        // multiple ASIDs.
        //

        KiTbFlushSingleAllAsidBroadcast(Virtual);

    } else {

        if (KiTbDisableSingleByAsidBroadcast != FALSE) {
            KiTbFlushSingleAllAsidBroadcast(Virtual);

        } else {
            Enabled = KeDisableInterrupts();
            TargetAsid = KiTbSnapCurrentAsid();
            KiTbFlushSingleByAsidBroadcast(Virtual, TargetAsid);
            if (Enabled != FALSE) {
                _enable();
            }
        }
    }

    KiTbBarrier();
    KiTbSendIpiIfNecessary(ProcessorSet);
    return;
}

extern
KINLINE
VOID
KiFlushProcessTb (
    VOID
    )

/*++

Routine Description:

    This routine flushes the non-global TLB entries in the current
    address space i.e those that are local to the current process.
    The intention is that this addresses both user space and session space.

Arguments:

    None.

Return Value:

    None.

--*/

{
    VERIFY_ASID_OWNERSHIP();

    //
    // On ARM, session space will be tagged with the per-process
    // ASIDs, so only a full flush will invalidate a session across
    // all attached processes.
    //

    KeFlushCurrentTb();
    return;
}

extern
VOID
KxFlushEntireTb (
    __in KTBFLUSH_TARGET ProcessorSet
    )

/*++

Routine Description:

    This function flushes the entire translation buffer (TB) on all
    processors in the host configuration.

Arguments:

    ProcessorSet - Supplies which processors'
        translation buffers are to be flushed.


Return Value:

    None.

--*/

{

    KIRQL OldIrql;

    OldIrql = KeRaiseIrqlToSynchLevel();

    VERIFY_ASID_OWNERSHIP();

    //
    // Set time stamp busy if possible otherwise another processor did the
    // flush for us.
    //

    if (KiSetTbFlushTimeStampBusy()) {

        //
        // A DSB is needed here to ensure the hardware page walker cannot see
        // (ie, speculate back in) the old PTE value(s) after the actual TLB flush.
        //

        _DataSynchronizationBarrier();

        KiTbFlushBroadcast();
        KiTbBarrier();
        KiTbSendIpiIfNecessary(ProcessorSet);
        KiClearTbFlushTimeStampBusy();
    }

    KeLowerIrql(OldIrql);
    return;
}

extern
KINLINE
VOID
KiFlushTb (
    __in KTBFLUSH_TYPE FlushType,
    __in KTBFLUSH_TARGET ProcessorSet
    )

/*++

Routine Description:

    This function flushes the specified portion of the translation buffer
    on all processors that are currently running threads which are children of
    the current process, or flushes the specified portion of the translation
    buffer on all processors.

Arguments:

    FlushType - Supplies the type of flush to perform.

    ProcessorSet - Supplies which processors'
        translation buffers are to be flushed.

Return Value:

    None.

--*/

{

    NT_ASSERT(((FlushType != FlushUser) && (ProcessorSet != FlushSubsetProcessors)) ||
              ((FlushType == FlushUser) && (ProcessorSet == FlushSubsetProcessors)));

    VERIFY_ASID_OWNERSHIP();
    switch (FlushType) {

        case FlushUser:
            KiTbFlushProcessBroadcast();
            KiTbSendIpiIfNecessary(FlushSubsetProcessors);
            break;
        case FlushSession:

            //
            // Session space tb entries may be valid in any address
            // space that is attached to that session.  Because
            // session PTEs are marked 'local' (non-global), the TBs
            // of all processors that have run processes attached to
            // the session must be flushed.
            //
            // As a simplification, we just flush all the TB entries
            // across all th processor/ASIDs.
            //

        case FlushKernel:
        case FlushEntire:
            KxFlushEntireTb(ProcessorSet);
            break;

        default:
            __assume(0);
    }

    return;
}

extern
KINLINE
VOID
KeFlushCurrentTb (
    VOID
    )

/*++

Routine Description:

    This routine flushes the TLB on the current processor.

Arguments:

    None.

Return Value:

    None.

--*/

{
    //
    // A DSB is needed here to ensure the hardware page walker cannot see
    // (ie, speculate back in) the old PTE value(s) after the actual TLB flush.
    //

    _DataSynchronizationBarrier();

    KiTbFlushCurrent();
    KiTbBarrier();
    return;
}

VOID
KiTbInitialize (
    __in ULONG Number,
    __in PKPROCESS Process
    )
/*++

 Routine Description:

    Initialize TLB and region (ASID) support

 Arguments:

    Number - Supplies a processor index

    Process - Supplies a KPROCESS pointer

 Environment:

    Kernel mode.

--*/
{

    ARM_IDCODE Id;

    if (Number == 0) {

        //
        // Also check based on processor ID.
        //

        Id.Ulong = _MoveFromCoprocessor (CP15_MIDR);
        if (Id.Implementer == 'Q') {

            if (Id.Model == 0x02D) {

                //
                // Qualcomm Scorpion has a bug where the TLBIMVAIS may
                // not work correctly.
                //

                KiTbDisableSingleByAsidBroadcast = TRUE;
            }

            if (((Id.Model & 0xFC0) == 0x40) && (Id.Revision == 0)) {

                //
                // Qualcomm Krait Errata #3 - TLBIASIDIS and
                // TLMBIVMAIS don't work properly on r0px
                //

                KiTbDisableSingleByAsidBroadcast = TRUE;
                KiTbDisableProcessByAsidBroadcast = TRUE;
            }
        }

        if ((Id.Model == 0x0C09) &&
            (Id.Implementer == 'A') &&
            (Id.Revision < 2)) {

            //
            // Cortex-A9 Errata #720789 - TLBIASIDIS and TLMBIVMAIS don't
            // work properly on some revisions.
            //

            KiTbDisableSingleByAsidBroadcast = TRUE;
            KiTbDisableProcessByAsidBroadcast = TRUE;
        }

        if ((Id.Model == 0x0C0F) &&
            (Id.Implementer == 'A') &&
            (Id.Revision < 4)) {

            //
            // Cortex-A15 Errata #798181 - Moving a virtual page that is being
            // accessed by an active process can lead to unexpected behavior.
            //

            KiTbIpiTypeToSendForBroadcast = IPI_NOP;
            KiTbSendIpiForBroadcastFlushes = TRUE;
        }
    }

    KiAsidInitialize(Number, Process);
}

