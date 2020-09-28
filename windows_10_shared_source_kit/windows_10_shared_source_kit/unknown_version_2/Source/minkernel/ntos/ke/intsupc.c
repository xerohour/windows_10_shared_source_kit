/*++

Copyright (c) 2003  Microsoft Corporation

Module Name:

    intsupc.c

Abstract:

    This module implements the kernel interrupt object. Functions are provided
    to initialize, connect, and disconnect interrupt objects.

    ARM does not natively support vectored interrupts.  A single
    exception occurs that must be further directed by querying the HAL
    for vector information.  As a result the IDT managed here is
    entirely a software construct consisting of an array of pointers
    to KINTERRUPT structures.

Environment:

    Kernel mode only.

--*/

#include "ki.h"
#include "ntrtlp.h"

VOID
KiResetException (
    VOID
    );

VOID
KiPlayInterrupt (
    __in PKTRAP_FRAME TrapFrame,
    __in ULONG Vector,
    __in KIRQL Irql,
    __in KIRQL PreviousIrql
    );

//
// ------------------------------------------------------------------ Globals
//

//
// ------------------------------------------------------------------ Functions
//

#if defined(_CAPKERN)

ULONGLONG
CAPTimeStamp (
    VOID
    );

VOID
CAPLogInterrupt (
    IN ULONGLONG TimeStamp,
    IN PVOID ServiceRoutine
    );

#endif


VOID
KiInitializeIdt (
    __in ULONG Number,
    __in PKPCR Pcr
    )

/*++

Routine Description:

    This function initializes the interrupt vector table which is a software
    construct on ARM processors rather than a hardware defined structure.

Arguments:

    None.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Number);
    UNREFERENCED_PARAMETER(Pcr);
    return;
}

KINLINE
BOOLEAN
KiCheckIdtExt (
    __in ULONG IdtIndex
    )

/*++

Routine Description:

    This function allocates an extended table of vectors if necessary.

Arguments:

    Pcr - Supplies a pointer to the Pcr.

    IdtIndex - Supplies the Vector Index.

Return Value:

    True if successful or allocation not needed, false otherwise.

Notes:

    This code must be called from passive level with the thread
    running on the target processor.

--*/

{

    PVOID *IdtExt;
    PVOID *IdtExtPrev;
    ULONG PackedBitsMask;
    PKPCR Pcr;
    ULONG Size;

    //
    // If all the bits between the lowest and highest four are zero, this vector
    // is stored in the preallocated Idt table.
    //
    // PackedBitsMask is a mask of the bits in the vector above the lowest four
    // and below the top four (the IRQL).
    //

    PackedBitsMask = ((1 << (MAXIMUM_IDTVECTOR_BITS - 4))- 0x10);
    if ((IdtIndex & PackedBitsMask) == 0) {
        return TRUE;
    }

    Pcr = KeGetPcr();

    if (Pcr->IdtExt != NULL) {
        return TRUE;
    }

    Size = NUMBER_DEVICEVECTOR * sizeof(PVOID);
    IdtExt = ExAllocatePoolWithTag(NonPagedPoolNx, Size, '  eK');
    RtlZeroMemory(IdtExt, Size);
    if (IdtExt == NULL) {
        return FALSE;
    }

    //
    // This code is running at passive level and could be preempted.
    // Ensure only one caller sets the IdtExt pointer.
    //

    IdtExtPrev = InterlockedCompareExchangePointer(
        (PVOID)&Pcr->IdtExt,
        IdtExt,
        NULL);

    if (IdtExtPrev != NULL) {
        ExFreePoolWithTag(IdtExt, '  eK');
    }

    return TRUE;

}

KINLINE
PVOID *
KiGetIdtEntry (
    __in PKPCR Pcr,
    __in ULONG IdtIndex
    )

/*++

Routine Description:

    This function returns a pointer to an entry in the Idt table. For vectors
    beyond the first 16 with a priority band it will return an entry from an
    extended table of vectors.

Arguments:

    Pcr - Supplies a pointer to the Pcr.

    IdtIndex - Supplies the Vector to be returned.

Return Value:

    Pointer to the requested Idt entry location, or NULL if the table entry
    was never allocated.

--*/

{

    //
    // Vectors are defined such that the top four bits contain the IRQL
    // priority value. The lower bits represent an index to an interrupt within
    // that IRQL band. The the first sixteen vectors within each
    // band are stored in a preallocated Idt table in the Pcr. This includes
    // all predefined vectors, such as the timer and IPI vectors.
    //
    // If a vector beyond the first sixteen is used, the code lazily allocates
    // an additional extended vector table to hold additional vectors.
    //

    ULONG PackedBitsMask;
    ULONG PackedBitsShift;
    ULONG PackedIndex;

    //
    // If all the bits between the lowest and highest four are zero, this vector
    // is stored in the preallocated Idt table.
    //
    // PackedBitsMask is a mask of the bits in the vector above the lowest four
    // and below the top four (the IRQL).
    //

    PackedBitsMask = ((1 << (MAXIMUM_IDTVECTOR_BITS - 4))- 0x10);
    if ((IdtIndex & PackedBitsMask) == 0) {

        //
        // Throw away the intermediate bits, which are all zero, to generate
        // an 8-bit index where the top nibble is the IRQL and the bottom
        // nibble is one of 16 vectors associated with that IRQL.
        //
        // PackedBitsShift is the number of bits which the vector must be
        // shifted to move the IRQL value so the entire vector fits within
        // a single byte.
        //

        PackedBitsShift = (MAXIMUM_IDTVECTOR_BITS - 8);
        PackedIndex = ((IdtIndex >> PackedBitsShift) & 0xF0) | (IdtIndex & 0x0F);
        return &Pcr->Idt[PackedIndex];
    }

    //
    // This vector doesn't fit in the standard Idt. It must be a device vector.
    //

    NT_ASSERT(IdtIndex >= MINIMUM_DEVICEVECTOR);
    NT_ASSERT(IdtIndex <= MAXIMUM_DEVICEVECTOR);
    NT_ASSERT(Pcr->IdtExt != NULL);

    IdtIndex = IdtIndex - MINIMUM_DEVICEVECTOR;
    if (IdtIndex >= NUMBER_DEVICEVECTOR) {
        return NULL;
    }

    if (Pcr->IdtExt == NULL) {
        return NULL;
    }

    return &Pcr->IdtExt[IdtIndex];
}

KINLINE
PVOID
KiGetIdtEntryValue (
    __in PKPCR Pcr,
    __in ULONG IdtIndex
    )

/*++

Routine Description:

    This function returns an entry in the Idt table. For vectors
    beyond the first 16 with a priority band it will return an entry from an
    extended table of vectors.

Arguments:

    Pcr - Supplies a pointer to the Pcr.

    IdtIndex - Supplies the Vector to be returned.

Return Value:

    Value in the requested Idt entry location, or NULL if not present.

--*/

{
    PVOID *IdtEntry;

    IdtEntry = KiGetIdtEntry(Pcr, IdtIndex);
    return (IdtEntry != NULL) ? *IdtEntry : NULL;
}


VOID
KiInitializeInterrupt (
    __out PKINTERRUPT Interrupt,
    __out_opt PKSPIN_LOCK SpinLock,
    __in BOOLEAN FloatingSave
    )

/*++

Routine Description:

    This function performs the ARM architecture specific initization of a
    kernel interrupt object.

Arguments:

    Interrupt - Supplies a pointer to a control object of type interrupt.

    SpinLock - Supplies a pointer to an executive spin lock.  There are two
        distinguished values recognized for SpinLock:

        NO_INTERRUPT_SPINLOCK - The kernel does not manage a spinlock
            associated with this interrupt.

        NO_END_OF_INTERRUPT - The interrupt represents a spurious interrupt
            vector, which is handled with a special, non-EOI interrupt
            routine.

    FloatingSave - Supplies a boolean value that determines whether the
        floating point registers are to be saved before calling the service
        routine function. N.B. This argument is ignored.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(SpinLock);
    UNREFERENCED_PARAMETER(FloatingSave);

    InitializeListHead(&Interrupt->InterruptListEntry);

    return;
}

NTSTATUS
KiConnectInterruptInternal (
    __inout PKINTERRUPT Interrupt,
    __in BOOLEAN HalInterrupt
    )

/*++

Routine Description:

    This function connects an interrupt object to the interrupt vector
    specified by the interrupt object.

Arguments:

    Interrupt - Supplies a pointer to a control object of type interrupt.

    HalInterrupt - Supplies an indication that the interrupt connect
        originates in the HAL.  In that case, synchronization of the
        IDT is provided by the HAL.  In addition, the processor number
        is not validated as the connect may originate on an embryonic
        processor.

Return Value:

    STATUS_SUCCESS - The interrupt vector was connected, and had not
        been before.

    STATUS_INTERRUPT_VECTOR_ALREADY_CONNECTED - The interrupt vector was already
        connected, and this interrupt object has been added to the chain.

    STATUS_INVALID_PARAMETER_1 - Some aspect of the interrupt object made it
        impossible to connect the interrupt.

--*/

{

    BOOLEAN AlreadyConnected;
    BOOLEAN Connected;
    GROUP_AFFINITY GroupAffinity;
    PVOID *IdtEntry;
    ULONG IdtIndex;
    PKINTERRUPT Interruptx;
    KIRQL Irql;
    ULONG Number;
    KIRQL OldIrql;
    PKPRCB Prcb;
    GROUP_AFFINITY PreviousAffinity;
    ULONG Vector;


    //
    // If the interrupt object is already connected, the interrupt vector
    // number is invalid, an attempt is being made to connect to a vector
    // that cannot be connected, the interrupt request level is invalid, or
    // the processor number is invalid, then do not connect the interrupt
    // object. Otherwise, connect the interrupt object to the specified
    // vector and establish the proper interrupt dispatcher.
    //

    AlreadyConnected = FALSE;
    Connected = FALSE;
    Irql = Interrupt->Irql;
    Number = Interrupt->Number;
    Vector = Interrupt->Vector;
    IdtIndex = Vector;
    OldIrql = HIGH_LEVEL;

    if (((IdtIndex > MAXIMUM_PRIMARY_VECTOR) ||
        (Irql > HIGH_LEVEL) ||
        ((Number >= KeNumberProcessors) && (HalInterrupt == FALSE)) ||
        ((Interrupt->SynchronizeIrql < Irql) &&
         (Interrupt->SynchronizeIrql != PASSIVE_LEVEL))) == FALSE) {

        //
        // Get a pointer to the IdtEntry prior to acquiring the spinlock in case
        // KiAllocIdtEntry needs to do memory allocation.
        //
        // N.B. An implicit assumption of this code is that Hal Interrupts
        //      lie within the first 16 vectors within a priority band, so they
        //      will not use the Idt Extension table, which is dynamically
        //      allocated as needed by KiCheckIdtExt().
        //

        if (HalInterrupt != TRUE) {

            //
            // Prior versions of this code called KiAcquireInterruptConnectLock,
            // which would affinitize to the target processor and then raise
            // IRQL to HIGH_LEVEL. However, the code in KiCheckIdtExt needs
            // to run at passive on the target, so that code has been inlined
            // here.
            //

            NT_ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

            //
            //
            // Set system affinity to the specified processor.
            //
            // N.B. Since all accesses to a particular interrupt object come from a
            //      single processor, no actual lock is required.
            //

            Prcb = KiProcessorBlock[Number];
            RtlZeroMemory(&GroupAffinity, sizeof(GROUP_AFFINITY));
            GroupAffinity.Group = Prcb->Group;
            GroupAffinity.Mask = Prcb->GroupSetMember;
            KeSetSystemGroupAffinityThread(&GroupAffinity, &PreviousAffinity);

            //
            // Now running at passive on the target processor.
            // Verify that the extended Idt table is available if needed.
            //

            KiCheckIdtExt(IdtIndex);

            //
            // Raise to high level.
            //

            KiAcquireInterruptConnectLockOnCurrentProcessor(&OldIrql);
        }

        //
        // If the specified interrupt vector is not connected, then
        // connect the interrupt vector to the interrupt object dispatch
        // code, establish the dispatcher address, and set the new
        // interrupt mode and enable masks. Otherwise, if the interrupt is
        // already chained, then add the new interrupt object at the end
        // of the chain. If the interrupt vector is not chained, then
        // start a chain with the previous interrupt object at the front
        // of the chain. The interrupt mode of all interrupt objects in
        // a chain must be the same.
        //

        if (Interrupt->Connected == FALSE) {

            IdtEntry = KiGetIdtEntry(KeGetPcr(), IdtIndex);
            if (IdtEntry != NULL) {
                if (*IdtEntry == NULL) {

                    Connected = TRUE;
                    Interrupt->Connected = TRUE;
                    *IdtEntry = Interrupt;

                } else {

                    //
                    // The interrupt vector is connected. Make sure the interrupt
                    // mode matchs and that both interrupt objects allow sharing
                    // of the interrupt vector.
                    //

                    AlreadyConnected = TRUE;
                    Interruptx = *IdtEntry;

                    if ((Interrupt->Mode == Interruptx->Mode) &&
                        (Interrupt->ShareVector != FALSE) &&
                        (Interruptx->ShareVector != FALSE)) {

                        Connected = TRUE;
                        Interrupt->Connected = TRUE;

                        //
                        // The interrupt list is ordered on the IRQLs (i.e. all
                        // non-passive interrupt objects appear before
                        // passive-level interrupt objects). Insert the interrupt
                        // object to the list at the correct location.
                        //

                        KiInsertInterruptObjectOrdered(Interruptx, Interrupt);

                        //
                        // If the head of the interrupt list points to a
                        // passive-level interrupt object, then switch the list
                        // order to make non-passive interrupt objects appear
                        // first in the list.
                        //

                        if ((Interruptx->SynchronizeIrql == PASSIVE_LEVEL)  &&
                            (Interrupt->SynchronizeIrql != PASSIVE_LEVEL)) {

                            *IdtEntry = Interrupt;
                        }
                    }
                }
            }
        }

        if (HalInterrupt != TRUE) {

            //
            // Release the interrupt connect lock, lower IRQL to its previous
            // value, and set the system affinity back to the original value.
            //

            KiReleaseInterruptConnectLock(OldIrql, &PreviousAffinity);
        }
    }

    //
    // Return whether interrupt was connected to the specified vector.
    //

    if (Connected == FALSE) {
        return STATUS_INVALID_PARAMETER_1;

    } else if (AlreadyConnected != FALSE) {
        return STATUS_INTERRUPT_VECTOR_ALREADY_CONNECTED;

    } else {
        return STATUS_SUCCESS;
    }
}

NTSTATUS
KiConnectInterrupt (
    __inout PKINTERRUPT Interrupt
    )

/*++

Routine Description:

    This function connects an interrupt object to the interrupt vector
    specified by the interrupt object.

Arguments:

    Interrupt - Supplies a pointer to a control object of type interrupt.

Return Value:

    STATUS_SUCCESS - The interrupt vector was connected, and had not
        been before.

    STATUS_INTERRUPT_VECTOR_ALREADY_CONNECTED - The interrupt vector was already
        connected, and this interrupt object has been added to the chain.

    STATUS_INVALID_PARAMETER_1 - Some aspect of the interrupt object made it
        impossible to connect the interrupt.

--*/

{

    return KiConnectInterruptInternal(Interrupt, FALSE);
}

NTSTATUS
KiConnectHalInterrupt (
    __inout PVOID Interrupt
    )

/*++

Routine Description:

    This function connects an interrupt object to the interrupt vector
    specified by the interrupt object.  It used by the HAL to connect
    internal interrupts early in boot.  Synchronization is provided by the
    HAL.

Arguments:

    Interrupt - Supplies a pointer to a control object of type interrupt.

Return Value:

    STATUS_SUCCESS - The interrupt vector was connected, and had not
        been before.

    STATUS_INTERRUPT_VECTOR_ALREADY_CONNECTED - The interrupt vector was already
        connected, and this interrupt object has been added to the chain.

    STATUS_INVALID_PARAMETER_1 - Some aspect of the interrupt object made it
        impossible to connect the interrupt.

--*/

{

    return KiConnectInterruptInternal((PKINTERRUPT)Interrupt, TRUE);
}

NTSTATUS
KiDisconnectInterruptInternal (
    __inout PKINTERRUPT Interrupt,
    __in PINTERRUPT_CONNECTION_DATA ConnectionData
    )

/*++

Routine Description:

    This function disconnects an interrupt object from the interrupt vector
    specified by the interrupt object.

    N.B. Assumes the caller has acquired the HIGH_LEVEL connect lock before
         invocation.

Arguments:

    Interrupt - Supplies a pointer to a control object of type interrupt.

    ConnectionData - Supplies a pointer to information describing how the
        interrupt is to be disconnected by the platform.  This is used to allow
        the HAL to prevent the relevant device from interrupting on the
        specified vector.

Return Value:

    STATUS_SUCCESS - The interrupt was disconnected, and there are
        no more interrupt objects in the chain, meaning that the
        interrupt vector is now unconnected.

    STATUS_INTERRUPT_STILL_CONNECTED - This interrupt object was
        disconnected, though there are still other object(s) in the
        chain, meaning that the vector is still connected.

    STATUS_INVALID_PARAMETER_1 - The specified interrupt object was
        invalid, or not connected.

--*/

{

    BOOLEAN Connected;
    PVOID *IdtEntry;
    PKINTERRUPT Interruptx;
    NTSTATUS Status;

    NT_ASSERT(KeGetCurrentIrql() == HIGH_LEVEL);

    Status = STATUS_INVALID_PARAMETER_1;

    //
    // If the interrupt object is connected, then disconnect it from the
    // specified vector.
    //

    Connected = Interrupt->Connected;
    if (Connected != FALSE) {
        IdtEntry = KiGetIdtEntry(KeGetPcr(), Interrupt->Vector);
        if (IdtEntry != NULL) {

            //
            // If there is only one entry connected to the specified interrupt
            // vector, then disconnect it by setting the IDT entry to NULL.
            // Otherwise remove the interrupt object from the interrupt chain.
            //

            Interruptx = *IdtEntry;
            NT_ASSERT(Interruptx != NULL);
            if (IsListEmpty(&Interruptx->InterruptListEntry) != TRUE) {

                //
                // If the interrupt is being removed from head, then update next
                // interrupt object to be head.
                //

                if (Interrupt == Interruptx) {
                    Interruptx = CONTAINING_RECORD(Interruptx->InterruptListEntry.Flink,
                                                   KINTERRUPT,
                                                   InterruptListEntry);

                    *IdtEntry = Interruptx;
                }

                RemoveEntryList(&Interrupt->InterruptListEntry);
                Status = STATUS_INTERRUPT_STILL_CONNECTED;

            } else {

                //
                // The interrupt object is not connected to the chained interrupt
                // dispatcher.  Prevent the platform from asserting the interrupt,
                // and then set the IDT entry to point to the unexpected handler.
                //

                HalDisableInterrupt(ConnectionData);
                *IdtEntry = NULL;
                Status = STATUS_SUCCESS;
            }
        }

        Interrupt->Connected = FALSE;
    }

    //
    // Return status describing whether the interrupt was disconnected from
    // the specified vector.
    //

    return Status;
}

NTSTATUS
KiMaskInterruptInternal (
    __in ULONG Vector,
    __in ULONG Gsiv
    )

/*++

Routine Description:

    This function checks the IDT chain linked to a specific vector to determine
    if all of the entries have been masked, in which case the line can be
    masked.

    N.B. Assumes the caller has acquired the HIGH_LEVEL connect lock before
         invocation.

Arguments:

    Vector - Supplies the vector on the current processor to check.

    Gsiv - Supplies the GSIV identifying the vector in system space.

Return Value:

    STATUS_SUCCESS - There are no more active interrupt objects in the chain,
        so the line was masked.
    STATUS_INTERRUPT_STILL_CONNECTED - There are still active interrupt objects
        in the chain so no action was taken.
    STATUS_ALREADY_DISCONNECTED - There are no interrupts attached to the line
        specified by vector on this processor.

--*/

{

    PKINTERRUPT Interrupt;
    PKINTERRUPT Interruptx;
    PLIST_ENTRY NextInterrupt;
    KINTERRUPT_STATE State;
    NTSTATUS Status;

    NT_ASSERT(KeGetCurrentIrql() == HIGH_LEVEL);
    NT_ASSERT(Vector <= MAXIMUM_PRIMARY_VECTOR);

    Interrupt = KiGetIdtEntryValue(KeGetPcr(), Vector);
    if (Interrupt == NULL) {
        Status = STATUS_ALREADY_DISCONNECTED;

    } else {

        if (IsListEmpty(&Interrupt->InterruptListEntry) == FALSE) {

            //
            // Walk the list to determine if all of the entries are currently
            // masked.
            //

            Status = STATUS_SUCCESS;
            NextInterrupt = &Interrupt->InterruptListEntry;
            do {
                Interruptx = CONTAINING_RECORD(NextInterrupt,
                                               KINTERRUPT,
                                               InterruptListEntry);

                State = *(PKINTERRUPT_STATE)&Interruptx->InternalState;
                if (State.Disabled == FALSE) {
                    Status = STATUS_INTERRUPT_STILL_CONNECTED;
                    break;
                }

                NextInterrupt = Interruptx->InterruptListEntry.Flink;
            } while (NextInterrupt != &Interrupt->InterruptListEntry);

        } else {

            State = *(PKINTERRUPT_STATE)&Interrupt->InternalState;
            if (State.Disabled == FALSE) {
                Status = STATUS_INTERRUPT_STILL_CONNECTED;

            } else {

                Status = STATUS_SUCCESS;
            }
        }
    }

    if (Status == STATUS_SUCCESS) {
        Status = HalMaskInterrupt(Gsiv, HAL_MASK_UNMASK_FLAGS_NONE);
    }

    return Status;
}

NTSTATUS
KiUnmaskInterruptInternal (
    __in ULONG Vector,
    __in ULONG Gsiv
    )

/*++

Routine Description:

    This function unmasks the line for Gsiv.

Arguments:

    Vector - Supplies the vector for the first processor in the interrupt list.

    Gsiv - Supplies the GSIV identifying the vector in system space.

Return Value:

    NTSTATUS value.

--*/

{

    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Vector);

    Status = HalUnmaskInterrupt(Gsiv, HAL_MASK_UNMASK_FLAGS_NONE);

    NT_ASSERT(NT_SUCCESS(Status));

    return Status;
}

extern
KINLINE
VOID
KiRequestSoftwareInterrupt (
    __in_opt PKPRCB Prcb,
    __in KIRQL RequestIrql
    )

/*++

Routine Description:

    This function requests a software interrupt at the specified IRQL level.

Arguments:

    Prcb - Supplies an optional pointer to the current PRCB.

    RequestIrql - Supplies the request IRQL value.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Prcb);

    HalRequestSoftwareInterrupt(RequestIrql);
    return;
}

LONG
KiInterruptExceptionFilter (
    __in PEXCEPTION_POINTERS ExceptionPointer,
    __out PULONG BugCheckCode
    )

/*++

Routine Description:

    This routine serves as a exception filter and has the special job of
    determining the right bugcheck code if an unhandled exception has occurred
    in an interrupt handler.

Arguments:

    ExceptionPointer - A pointer to the exception record that contains
                       the real Io Status.

    BugCheckCode  - A pointer to an value that will used in bugcheck.

Return Value:

    EXCEPTION_EXECUTE_HANDLER

--*/

{

    if (ExceptionPointer->ExceptionRecord->ExceptionFlags & EXCEPTION_UNWIND) {
        *BugCheckCode = INTERRUPT_UNWIND_ATTEMPTED;

    } else {
        *BugCheckCode = INTERRUPT_EXCEPTION_NOT_HANDLED;
    }

    KeBugCheckEx(*BugCheckCode,
                 (ULONG_PTR)ExceptionPointer->ExceptionRecord,
                 (ULONG_PTR)ExceptionPointer->ContextRecord,
                 0,
                 0);
}

VOID
KxSwitchStackAndPlayInterrupt (
    __in PKTRAP_FRAME TrapFrame,
    __in ULONG Vector,
    __in KIRQL Irql,
    __in KIRQL PreviousIrql,
    __in PVOID IsrStack
    );

KINLINE
VOID
KiSwitchStackAndPlayInterrupt (
    __in PKTRAP_FRAME TrapFrame,
    __in ULONG Vector,
    __in KIRQL Irql,
    __in KIRQL PreviousIrql,
    __in ULONG OpaqueToken
    )

/*++

Routine Description:

    This routine wraps the call KiPlayInterrupt. It tests if already running
    on the ISR stack and, if not, calls the assembly helper routine to perform
    the switch first.

Arguments:

    TrapFrame - Supplies a pointer to the trap frame of the interrupt
        exception.

    Vector - Supplies the vector to execute.

    Irql - Supplies the IRQL of this interrupt.

    PreviousIrql - Supplies the IRQL before the interrupt.

    OpaqueToken - Supplies the opaque token returned when the interrupt
        happened.

Return Value:

    None.

--*/

{

    ULONG_PTR CurrentStack;
    ULONG_PTR IsrStackBase;
    ULONG_PTR IsrStackLimit;
    PKPRCB Prcb;

    CurrentStack = KeGetCurrentStackPointer();
    IsrStackBase = KeGetIsrStackBase();
    IsrStackLimit = IsrStackBase - ISR_STACK_SIZE;
    if (((IsrStackLimit <= CurrentStack) && (CurrentStack < IsrStackBase)) ||
        (KeIsBugCheckActive(NULL) != FALSE)) {

        //
        // Already running in the ISR stack or bugchecking. Remain on the
        // current stack.
        //

        KiPlayInterrupt(TrapFrame,
                        Vector,
                        Irql,
                        PreviousIrql);

    } else {

        //
        // Switch to the ISR stack and then call the service routine.
        //

        Prcb = KeGetCurrentPrcb();
        KxSwitchStackAndPlayInterrupt(TrapFrame,
                                      Vector,
                                      Irql,
                                      PreviousIrql,
                                      Prcb->IsrStack);

        NT_ASSERT(Prcb == KeGetCurrentPrcb());
    }

    HalEndSystemInterrupt(Vector, PreviousIrql, OpaqueToken, TrapFrame);
}

UCHAR
KiRunIsr (
    __in PKINTERRUPT Interrupt,
    __in PKTRAP_FRAME TrapFrame,
    __in KIRQL InterruptIrql,
    __in ULONG Vector
    )

/*++

Routine Description:

    This routine runs the ISR associated with a given interrupt.

Arguments:

    Interrupt - Supplies a pointer to the KINTERRUPT structure representing
        this interrupt.

    TrapFrame - Supplies a pointer to the trap frame of the interrupt
        exception.

    InterruptIrql - Supplies the IRQL associated with this interrupt

    Vector - Supplies the vector of the interrupt (used for logging).

Return Value:

    Returns the value returned by KiCallInterruptServiceRoutine.

--*/

{

    BOOLEAN AcquireLock;
    UCHAR IsrResult;
    BOOLEAN LoggingEnabled;
    ETW_KERNEL_TRACE_TIMESTAMP TimeStamp;

#if defined(_CAPKERN)

    ULONGLONG StartTimeStamp;

#endif

    AcquireLock = TRUE;
    LoggingEnabled = FALSE;
    Interrupt->TrapFrame = TrapFrame;

    if (InterruptIrql != Interrupt->SynchronizeIrql) {
        KeRaiseIrql(Interrupt->SynchronizeIrql, &InterruptIrql);
    }

    if ((Interrupt->ActualLock == (PKSPIN_LOCK)INTERRUPT_PERFORMANCE) ||
        (Interrupt->ActualLock == (PKSPIN_LOCK)NO_INTERRUPT_SPINLOCK)) {

        AcquireLock = FALSE;

    }

    if (Interrupt->ActualLock != (PKSPIN_LOCK)INTERRUPT_PERFORMANCE) {
        LoggingEnabled = PERFINFO_IS_GROUP_ON(PERF_INTERRUPT);
    }

#if defined(_CAPKERN)

    StartTimeStamp = CAPTimeStamp();

#endif

    if (LoggingEnabled != FALSE) {
        EtwGetKernelTraceTimestamp(&TimeStamp, PERF_INTERRUPT);

        IsrResult = KiCallInterruptServiceRoutine(Interrupt, AcquireLock);

        PerfInfoLogInterrupt(Interrupt,
                             (Vector << 8) | IsrResult,
                             &TimeStamp);

    } else {
        IsrResult = KiCallInterruptServiceRoutine(Interrupt, AcquireLock);
    }

#if defined(_CAPKERN)

    CAPLogInterrupt( StartTimeStamp, Interrupt->ServiceRoutine );

#endif

    if (InterruptIrql != Interrupt->SynchronizeIrql) {
        KeLowerIrql(InterruptIrql);
    }

    return IsrResult;
}

VOID
KiProcessInterrupt (
    __in PKTRAP_FRAME TrapFrame
    )

/*++

Routine Description:

    This routine processes an interrupt exception and dispatches any
    pending interrupts.  It invokes the HAL to determine the pending
    interrupt, manage the IRQLs and later EOI the interrupt.

Arguments:

    TrapFrame - Supplies a pointer to the trap frame of the interrupt
        exception.

Return Value:

    None.

--*/

{

    BOOLEAN CollectEntropy;
    ULONG EntropyCount;
    ULONG EntropyIndex;
    PKENTROPY_TIMING_STATE EntropyState;
    PULONG EntropyTarget;
    KIRQL InterruptIrql;
    ULONG OpaqueToken;
    PKPRCB Prcb;
    KIRQL PreviousIrql;
    ULONG64 TimeStamp;
    BOOLEAN Valid;
    ULONG Vector;

#if KENTROPY_TIMING_ANALYSIS

    PULONG RawDataBuffer;

#endif

    //
    // N.B. ARM's Load/Store Exclusive feature replaces the need for
    //      SLIST sequence numbers and procedure rewind on generic traps and
    //      interrupts, so there is no KiCheckForSListAddress here.
    //      Rewinds for #AV while accessing First->Next are still required and
    //      are performed by KiOpPreprocessAccessViolation.
    //
    //
    // Update interrupt count (atomicity provided by interrupts being disabled)
    //

    Prcb = KeGetCurrentPrcb();
    Prcb->InterruptCount += 1;

    //
    // End thread accounting if this is not a nested interrupt.
    //

    CollectEntropy = FALSE;
    if (Prcb->NestingLevel == 0) {
        KiEndThreadCycleAccumulation(Prcb, Prcb->CurrentThread, &TimeStamp);
        CollectEntropy = TRUE;

    } else {
        Prcb->NestingLevel += 1;
        TimeStamp = 0;
    }

    //
    // The HAL is called with interrupts disabled and returns with them
    // enabled if the interrupt is accepted, or disabled if the interrupt
    // is spurious or deferred.
    //

    Valid = HalBeginSystemInterruptUnspecified(&Vector,
                                               &InterruptIrql,
                                               &PreviousIrql,
                                               &OpaqueToken);

    if (Valid == FALSE) {

        NT_ASSERT(KeAreInterruptsEnabled() == FALSE);

        //
        // Either the interrupt was spurious, or it was at an IRQL that is lower
        // than the current IRQL and therefore has been queued for later. If
        // returning to a thread, start thread accounting again.
        //

        if (Prcb->NestingLevel == 1) {
            KiStartThreadCycleAccumulation(Prcb, Prcb->CurrentThread, FALSE);

        } else {
            Prcb->NestingLevel -= 1;
        }

        return;
    }

    //
    // Collect entropy data from the interrupt timestamp.
    //
    // N.B. Entropy is only collected for interrupts which are actually serviced
    //      when they arrive.  If an interrupt is deferred due to lazy IRQL, no
    //      entropy is collected.
    //

    if ((CollectEntropy != FALSE) && (InterruptIrql < HIGH_LEVEL)) {
        EntropyState = &Prcb->EntropyTimingState;
        EntropyCount = EntropyState->EntropyCount;

        //
        // Compute the index at which to store the current interrupt's entropy.
        // The entropy timing code mixes 32 adjacent interrupts into a single
        // double word.
        //

        EntropyIndex = (EntropyCount & KENTROPY_TIMING_BUFFER_MASK);
        EntropyIndex /= 32;
        EntropyTarget = &EntropyState->Buffer[EntropyIndex];
        *EntropyTarget = RotateRight32(*EntropyTarget, 5) ^ ((ULONG)TimeStamp);

#if KENTROPY_TIMING_ANALYSIS

        RawDataBuffer = EntropyState->RawDataBuffer;
        KeMemoryBarrierWithoutFence();
        if (RawDataBuffer != NULL) {
            RawDataBuffer[EntropyCount % KENTROPY_TIMING_DATA_COUNT] = (ULONG)TimeStamp;
        }

#endif

        //
        // Store the updated entropy count and trigger entropy delivery if
        // sufficient interrupts have occurred.
        //

        EntropyCount += 1;
        EntropyState->EntropyCount = EntropyCount;
        if ((EntropyCount & (KENTROPY_TIMING_INTERRUPTS_PER_BUFFER - 1)) == 0) {
            KiEntropyQueueDpc(Prcb);
        }
    }

    KiSwitchStackAndPlayInterrupt(TrapFrame,
                                  Vector,
                                  InterruptIrql,
                                  PreviousIrql,
                                  OpaqueToken);

    return;
}

VOID
KiReplayInterrupt (
    __in ULONG Vector,
    __in KIRQL Irql,
    __in KIRQL PreviousIrql,
    __in ULONG OpaqueToken
    )

/*++

Routine Description:

    This routine replays an interrupt on behalf of the HAL when lazy IRQL is
    flushing its queue of pended interrupts.

Arguments:

    Vector - Supplies the vector to execute.

    Irql - Supplies the IRQL of this interrupt.

    PreviousIrql - Supplies the IRQL before the interrupt.

    OpaqueToken - Supplies the opaque token returned when the interrupt
        happened.

Return Value:

    None.

--*/

{

    KTRAP_FRAME FakeTrapFrame;
    PKPRCB Prcb;

    //
    // End thread accounting if this isn't a nested interrupt.
    //

    Prcb = KeGetCurrentPrcb();
    if (Prcb->NestingLevel == 0) {
        KiEndThreadCycleAccumulation(Prcb, Prcb->CurrentThread, NULL);

    } else {
        Prcb->NestingLevel += 1;
    }

#if DBG

    RtlFillMemory(&FakeTrapFrame, sizeof(KTRAP_FRAME), 0xdd);

#endif

    //
    // Only fill out the flags register, exception active (set to interrupt)
    // debug registers valid (no), and VFP state pointer (NULL).
    //

    FakeTrapFrame.Cpsr = _ReadStatusReg(0);
    FakeTrapFrame.ExceptionActive = KEXCEPTION_ACTIVE_INTERRUPT_FRAME;
    FakeTrapFrame.DebugRegistersValid = 0;
    FakeTrapFrame.VfpState = NULL;

    //
    // Also update the frame chain to include this fake trap frame, and
    // set up the sentinel in the reserved field.
    //

    FakeTrapFrame.Reserved = (ULONG_PTR) KiResetException & ~1;
    FakeTrapFrame.R11 = RtlpGetFrameChain();
    FakeTrapFrame.Pc = (ULONG_PTR) _ReturnAddress();
    RtlpSetFrameChain((ULONG_PTR) &FakeTrapFrame.R11);

    //
    // The arguments to play interrupt are cleverly buried in the trap frame
    // itself.
    //

    KiSwitchStackAndPlayInterrupt(&FakeTrapFrame,
                                  Vector,
                                  Irql,
                                  PreviousIrql,
                                  OpaqueToken);

    //
    // Clear the sentinel value to eliminate accidental false positives in the
    // future on the same stack.
    //

    FakeTrapFrame.Reserved = 0;
    return;
}

VOID
KiPlayInterrupt (
    __in PKTRAP_FRAME TrapFrame,
    __in ULONG Vector,
    __in KIRQL Irql,
    __in KIRQL PreviousIrql
    )

/*++

Routine Description:

    This routine runs any ISRs associated with taking the given interrupt. This
    action may have been deferred by lazy IRQL some amount of time after the
    interrupt actually happened if the processor was at a higher IRQL at the
    time.

Arguments:

    TrapFrame - Supplies a pointer to the trap frame of the interrupt
        exception.

    Vector - Supplies the vector to execute.

    Irql - Supplies the IRQL of this interrupt.

    PreviousIrql - Supplies the IRQL before the interrupt.

    OpaqueToken - Supplies the opaque token returned when the interrupt
        happened.

Return Value:

    None.

--*/

{

    BOOLEAN ActiveISR;
    BOOLEAN Claimed;
    ULONG Code;
    BOOLEAN Disabled;
    PVOID IdtEntry;
    UCHAR IsrResult;
    PKINTERRUPT Interrupt;
    PKINTERRUPT Interruptx;
    BOOLEAN OnlyEntry;
    PKPCR Pcr;
    PKPRCB Prcb;
    KINTERRUPT_STATE State;

    Claimed = FALSE;
    Code = 0;
    Pcr = KeGetPcr();
    Prcb = &(Pcr->Prcb);

    //
    // N.B. There are no APC or DPC interrupts on ARM.  APCs and DPCs
    //      are signalled to another processor via an IPI and then
    //      queued for a subsequent IRQL lower.
    //

    NT_ASSERT(Irql > DISPATCH_LEVEL);

    //
    // Enable interrupts. Don't be fooled, interrupts may already be enabled,
    // there are two paths to calling this function. The first is that an
    // interrupt was taken and was of sufficient IRQL to run. In that case the
    // HAL has already enabled interrupts. The other is that an interrupt is
    // being replayed during a lower IRQL operation. In that case the HAL calls
    // the kernel to replay the interrupt, and interrupts are disabled at the
    // processor. The reason they aren't enabled before this point is that
    // unlike x86, the HAL's begin system interrupt routine does not run on ARM
    // (because it would touch the controller, which has already acknowledged
    // this interrupt).
    //

    _enable();
    ActiveISR = FALSE;
    TrapFrame->PreviousIrql = PreviousIrql;
    __try {
        IdtEntry = KiGetIdtEntryValue(Pcr, Vector);
        if (IdtEntry != NULL) {
            Interrupt = IdtEntry;
            Interruptx = Interrupt;
            OnlyEntry = IsListEmpty(&Interrupt->InterruptListEntry);
            Disabled = FALSE;
            if (Interrupt->Mode == LevelSensitive) {

                //
                // Walk interrupt chain until an ISR claims the interrupt
                //

                for (;;) {
                    if (OnlyEntry == FALSE) {
                        State = *(PKINTERRUPT_STATE)&Interrupt->InternalState;
                        Disabled = (State.Disabled != FALSE);
                    }

                    if (Disabled == FALSE) {
                        ActiveISR = TRUE;
                        if (Interrupt->SynchronizeIrql == PASSIVE_LEVEL) {
                            KiDispatchPassiveInterrupts(Interrupt);
                            break;
                        }

                        IsrResult = KiRunIsr(Interrupt,
                                             TrapFrame,
                                             Irql,
                                             Vector);

                        if (IsrResult == KI_INTERRUPT_SERVICE_ROUTINE_CLAIMED) {
                            break;
                        }
                    }

                    Interrupt =
                        CONTAINING_RECORD(Interrupt->InterruptListEntry.Flink,
                                          KINTERRUPT,
                                          InterruptListEntry);

                    if (Interrupt == Interruptx) {
                        break;
                    }
                }

            } else {

                //
                // Keep walking the interrupt chain until no ISR claims
                // the interrupt.
                //

                for (;;) {
                    if (OnlyEntry == FALSE) {
                        State = *(PKINTERRUPT_STATE)&Interrupt->InternalState;
                        Disabled = (State.Disabled != FALSE);
                    }

                    if (Disabled == FALSE) {
                        ActiveISR = TRUE;
                        if (Interrupt->SynchronizeIrql == PASSIVE_LEVEL) {
                            KiDispatchPassiveInterrupts(Interrupt);
                            break;
                        }

                        IsrResult = KiRunIsr(Interrupt,
                                             TrapFrame,
                                             Irql,
                                             Vector);

                        if (IsrResult == KI_INTERRUPT_SERVICE_ROUTINE_CLAIMED) {
                            Claimed = TRUE;
                        }
                    }

                    Interrupt =
                         CONTAINING_RECORD(Interrupt->InterruptListEntry.Flink,
                                           KINTERRUPT,
                                           InterruptListEntry);

                    //
                    // If the next interrupt is back at the beginning of the
                    // list, loop if any ISR claimed the interrupt. If this
                    // interrupt is not shared, this action is not needed.
                    //

                    if (Interrupt == Interruptx) {
                        if ((Claimed != FALSE) && (OnlyEntry == FALSE)) {
                            Claimed = FALSE;

                        } else {
                            break;
                        }
                    }
                }
            }

        }

        if (ActiveISR == FALSE) {

            //
            // Unexpected interrupt. Log this, and if appropriate, bugcheck.
            //

            PerfInfoLogUnexpectedInterrupt(Vector);
            if (KiBugCheckUnexpectedInterrupts != 0) {
                KeBugCheckEx(TRAP_CAUSE_UNKNOWN, 1, Vector, 0, 0);
            }
        }

    } except(KiInterruptExceptionFilter(GetExceptionInformation(), &Code)) {
        NOTHING;
    }

    //
    // If returning to a thread, start thread accounting again.
    //

    _disable();
    if (Prcb->NestingLevel == 1) {
        KiStartThreadCycleAccumulation(Prcb, Prcb->CurrentThread, FALSE);

    } else {
        Prcb->NestingLevel -= 1;
    }

    return;
}

BOOLEAN
KeSynchronizeExecution (
    IN PKINTERRUPT Interrupt,
    IN PKSYNCHRONIZE_ROUTINE SynchronizeRoutine,
    IN PVOID SynchronizeContext
    )

/*++

Routine Description:

   This function synchronizes the execution of the specified routine with the
   execution of the service routine associated with the specified interrupt
   object.

Arguments:

   Interrupt - Supplies a pointer to a control object of type interrupt.

   SynchronizeRoutine - Supplies a pointer to a function whose execution
      is to be synchronized with the execution of the service routine
      associated with the specified interrupt object.

   SynchronizeContext - Supplies a pointer to an arbitrary data structure
      which is to be passed to the function specified by the
      SynchronizeRoutine parameter.

Return Value:

   The value returned by the SynchronizeRoutine function is returned as the
   function value.

--*/

{

    KIRQL PreviousIrql;
    BOOLEAN Result;

    if (Interrupt->SynchronizeIrql > PASSIVE_LEVEL) {
        KeRaiseIrql(Interrupt->SynchronizeIrql, &PreviousIrql);
        KeAcquireSpinLockAtDpcLevel(Interrupt->ActualLock);
        Result = SynchronizeRoutine(SynchronizeContext);
        KeReleaseSpinLockFromDpcLevel(Interrupt->ActualLock);
        KeLowerIrql(PreviousIrql);

    } else {
        Result = KiSynchronizePassiveInterruptExecution(Interrupt,
                                                        SynchronizeRoutine,
                                                        SynchronizeContext);
    }

    return Result;
}

PKINTERRUPT
KiGetInterruptObjectFromVector (
    __in ULONG Vector
    )

/*++

Routine Description:

    This routine returns the head of the interrupt chain for the given vector.

    N.B. This routine assumes the caller holds the HIGH_LEVEL connect lock.

Arguments:

    Vector - Supplies the vector for which the interrupt list should be
        returned.

Return Value:

    Pointer to the first interrupt object in the interrupt chain. NULL if the
    chain is empty.

--*/

{

    PVOID IdtEntry;
    ULONG IdtIndex;
    PKINTERRUPT Interrupt;
    PKPCR Pcr;

    NT_ASSERT(KeGetCurrentIrql() == HIGH_LEVEL);

    Interrupt = NULL;
    Pcr = KeGetPcr();
    IdtIndex = Vector;
    if (IdtIndex <= MAXIMUM_PRIMARY_VECTOR) {
        IdtEntry = KiGetIdtEntryValue(Pcr, IdtIndex);
        if (IdtEntry != NULL) {
            Interrupt = IdtEntry;
        }
    }

    return Interrupt;
}


