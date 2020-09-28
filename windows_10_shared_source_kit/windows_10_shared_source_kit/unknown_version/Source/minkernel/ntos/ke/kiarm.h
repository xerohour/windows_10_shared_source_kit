/*++

Copyright (c) 2003  Microsoft Corporation

Module Name:

    kiarm.h

Abstract:

    This module contains the private (internal) platform specific header file
    for the kernel.

--*/

#if !defined(_KIARM_)
#define _KIARM_

#if 0
#define KePrint(fmt, ...)						\
    do {								\
	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, fmt, ##__VA_ARGS__); \
    } while (0)
#else
#define KePrint(fmt, ...)
#endif

#define ROUNDUP128(x) (((x) + 127) & ~127)

//
// Architecture-specific characteristics.
//

#define KI_AGGRESSIVE_REBALANCE                     1
#define KI_HYPER_THREADING_SUPPORTED                0
#define KI_USES_IDT_REGISTER                        0
#define KI_USES_GDT_REGISTER                        0
#define KI_PROCESSORS_HAVE_AFFINITY                 1
#define KI_PROCESSORS_HAVE_APIC_MASKS               0
#define KI_PROCESSORS_HAVE_IDTS                     0
#define KI_PRCB_HAS_PROCESSOR_PROFILE_CONTROL_AREA  0
#define KI_PRCB_HAS_PROFILE_EVENT_INDEX_ADDRESS     0
#define KI_USING_UNWIND_HISTORY_TABLE               1
#define KI_ACQUIRE_SYSTEM_PROCESS_ASID              1
#define KI_USES_NMI                                 0
#define KI_SUPPORTS_IPI_NOP                         1
#define KI_PRCB_HAS_REQUEST_SUMMARY                 1
#define KI_SUPPPORTS_IPI_FROZEN                     1
#define KI_SUPPPORTS_IPI_FREEZE_SYNCH               0
#define KI_USES_BOUND                               0

//
// CP14 access
//

//
// Debug regs and flags
//
#define DBGDSCR_HLT_EN_BIT 0x00004000
#define DBGDSCR_MON_EN_BIT 0x00008000

#define DIDR_WPT_MASK      (0xf0000000)
#define DIDR_WPT_SHIFT     (28)
#define DIDR_BKPT_MASK     (0x0f000000)
#define DIDR_BKPT_SHIFT    (24)

#define SS_BCR_DEFAULT_BITS   (0x000001e0)

#define DBG_VCR_MASK       (~((1 << 28) | (1 << 27) |\
			      (1 << 12) | (1 << 11) | 0xdf))

extern BOOLEAN KiTbSendIpiForBroadcastFlushes;
extern ULONG_PTR KiTbIpiTypeToSendForBroadcast;
extern BOOLEAN KiCP14DebugEnabled;
extern ULONG KiZeroedDebugRegs[];

//
// Define IPI request summary structure.
//
// N.B. The numeric order of the request definitions is important.
//

#define IPI_FLUSH_PROCESS 1
#define IPI_FLUSH_SINGLE 2
#define IPI_FLUSH_ALL 3
#define IPI_FLUSH_MULTIPLE 4
#define IPI_FLUSH_MULTIPLE_RANGE 5
#define IPI_PACKET_READY 6
#define IPI_INVALIDATE_ALL 7
#define IPI_NOP 8

#define IPI_REQUEST_SUMMARY 0xf

typedef struct _REQUEST_SUMMARY {
    union {
        struct {
            ULONG IpiRequest : 4;
            ULONG Count : 8;
            ULONG Parameter : 20;
        };

        LONG Summary;
    };

} REQUEST_SUMMARY, *PREQUEST_SUMMARY;

//
// Define get current ready summary
//

KINLINE
ULONG
KiGetCurrentReadySummary (
    VOID
    )

/*++

Routine Description:

    This function retrieves an indication of whether there are any ready threads
    that can be scheduled presently on the current processor.

Arguments:

    None.

Return Value:

    A nonzero value of any threads can be scheduled. Zero otherwise.

--*/

{

    ULONG ReadySummary;
    PKPRCB Prcb;

    Prcb = KeGetCurrentPrcb();
    ReadySummary = ReadULongNoFence(&Prcb->ReadySummary);
    if (ReadySummary == 0) {
        return Prcb->SharedReadyQueue->ReadySummary;
    }

    return 0;
}

#define KxInitializeProcessorState(State, \
                                   LoaderBlock, \
                                   PerProcessorAllocation, \
                                   DynamicProcessor, \
                                   AllocationSize, \
                                   NodeNumber, \
                                   ProcessorIndex, \
                                   MaxProcCount, \
                                   GdtOffset, \
                                   IdtOffset, \
                                   Stack, \
                                   DpcStack, \
                                   IsrStack) \
    KiInitializeProcessorState(State, \
                               LoaderBlock, \
                               PerProcessorAllocation, \
                               DynamicProcessor, \
                               AllocationSize, \
                               NodeNumber, \
                               ProcessorIndex, \
                               MaxProcCount, \
                               Stack, \
                               DpcStack, \
                               IsrStack)

PKPRCB
KiInitializeProcessorState (
    __out PKPROCESSOR_STATE State,
    __out PLOADER_PARAMETER_BLOCK LoaderBlock,
    __out PUCHAR PerProcessorAllocation,
    __in LOGICAL DynamicProcessor,
    __in ULONG AllocationSize,
    __in ULONG NodeNumber,
    __in ULONG ProcessorIndex,
    __in ULONG MaxProcCount,
    __out PVOID Stack,
    __out PVOID DpcStack,
    __out PUCHAR IsrStack
    );

VOID
KiAdjustProcessorAffinity (
    VOID
    );

typedef
VOID
(*PKLOCAL_IPI_ROUTINE) (
    PVOID
    );

VOID
KiIpiSendRequestEx (
    __in PKPRCB Prcb,
    __in IPI_TYPE IpiType,
    __in_opt PKAFFINITY_EX TargetSet,
    __in ULONG_PTR Parameter,
    __in ULONG_PTR Count,
    __in ULONG_PTR RequestType,
    __in_opt PKLOCAL_IPI_ROUTINE LocalRoutine,
    __in_opt PVOID LocalRoutineParameter
    );

KINLINE
VOID
KiIpiWaitForRequestBarrier (
    __in PKPRCB Prcb
    )

/*++

Routine Description:

    This function waits until the specified barrier is set to zero.

Arguments:

    Barrier - Supplies a pointer to a request barrier.

Return Value:

    None.

--*/

{

    ULONG SpinCount;

    NT_ASSERT(Prcb == KeGetCurrentPrcb());

    //
    // Wait until the request barrier is zero before proceeding.
    //

    SpinCount = 0;
    while (ReadAcquire(&Prcb->PacketBarrier) != 0) {
        KeYieldProcessorEx(&SpinCount);
    }

    return;
}

BOOLEAN
KiIpiProcessRequests (
    __in PKTRAP_FRAME TrapFrame,
    __in PKEXCEPTION_FRAME ExceptionFrame
    );

//
// OS Initialization
//

VOID
KiInitializeIdt(
    __in ULONG Number,
    __in PKPCR Pcr
    );

VOID
KiInitializeKernel (
    __in PKPROCESS Process,
    __in PKTHREAD Thread,
    __in PVOID IdleStack,
    __in PKPRCB Prcb,
    __in ULONG Number,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

ULONG
KiComputeProcessorDataSize (
    __in ULONG MaxProcCount
    );

extern ULONG KiTbrConfiguration;


extern BOOLEAN KiResumeForReboot;

ULONG
KiInitializeVfp(
    VOID
    );

VOID
KiInitializeCP14DebugState (
    VOID
    );

VOID
KiReadBankedRegisters (
    __in ULONG ProcessorMode,
    __out PULONG Sp,
    __out PULONG Lr,
    __out PULONG Spsr
    );

VOID
KiWriteBankedRegisters (
    __in ULONG ProcessorMode,
    __in PULONG Sp,
    __in PULONG Lr,
    __in PULONG Spsr
    );

VOID
KiCacheInitialize (
    VOID
    );

VOID
KiSetCacheInformation (
    VOID
    );

VOID
KiCacheAdjustFlushPromoteThresholds (
    VOID
    );

VOID
KiWaitForReboot (
    VOID
    );

KINLINE
VOID
KiSetupForInstrumentationReturn (
    _Inout_ PKTRAP_FRAME TrapFrame
    )

/*++

Routine Description:

    This function determines whether instrumentation callbacks are enabled for
    the current thread's process.  If so, the trap frame is modified to return
    to the process's instrumentation callback address, with the original return
    address placed in caller's R12.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame.

Return Value:

    None.

--*/

{
    PVOID Callback;
    PKTHREAD Thread;

    NT_ASSERT(KeAreAllApcsDisabled() != FALSE);

    Thread = KeGetCurrentThread();
    Callback = Thread->ApcState.Process->InstrumentationCallback;
    if (Callback != NULL) {

        //
        // We cannot safely jump to an instrumentation point if the IT flags are non-zero
        //

        if ((TrapFrame->Cpsr & CPSRM_MASK) == CPSRM_USER &&
            (TrapFrame->Cpsr & CPSR_IT_MASK) == 0) {

            TrapFrame->R12 = TrapFrame->Pc;
            TrapFrame->Pc = (ULONG_PTR)Callback;
        }
    }

    return;
}

VOID
KiFlushProcessTb (
    VOID
    );

VOID
KiFlushSingleTb(
    __in PVOID Virtual
    );

VOID
KiTbInitialize (
    __in ULONG Number,
    __in PKPROCESS Process
    );

VOID
KiAsidAcquire (
    __in PKPROCESS Process
    );

VOID
KiAsidInitialize (
    __in ULONG Number,
    __in PKPROCESS Process
    );

//
// Scheduling support
//

VOID
KiThreadAccumulateProcessorCycles (
    PKPRCB Prcb,
    PKTHREAD Thread
    );

VOID
KiAccumulateProcessorCycles (
    VOID
    );

//
// Define thread startup routine prototypes.
//

VOID
KiStartSystemThread (
    VOID
    );

VOID
KiStartUserThread (
    VOID
    );

VOID
KiStartUserThreadReturn (
    VOID
    );

VOID
KiDispatchInterruptContinue (
    VOID
    );

BOOLEAN
KiPreprocessFault (
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PKEXCEPTION_FRAME ExceptionFrame,
    IN PKTRAP_FRAME TrapFrame,
    IN PCONTEXT ContextRecord,
    IN KPROCESSOR_MODE PreviousMode
    );

//
// Debug routines
//

VOID
KiSetHwDebugRegs (
    __in ULONG MaxBreakpoints,
    __inout PULONG Bvr,
    __inout PULONG Bcr,
    __in ULONG MaxWatchpoints,
    __inout PULONG Wvr,
    __inout PULONG Wcr
    );

VOID
KiReadHwDebugRegs(
    __inout PULONG Bvr,
    __inout PULONG Bcr,
    __inout PULONG Wvr,
    __inout PULONG Wcr
    );

VOID
KiWriteHwDebugRegs(
    __in PULONG Bvr,
    __in PULONG Bcr,
    __in PULONG Wvr,
    __in PULONG Wcr
    );

BOOLEAN
KiIsArmedForDebug (
    __in PULONG Bcr,
    __in PULONG Wcr
    );

VOID
KiUpdateDbgdscr (
    __in BOOLEAN enable
    );

VOID
KiSetDebugActive (
    __inout PKTHREAD Thread,
    __in UCHAR DebugActive,
    __in UCHAR DebugActiveMask
    );

//
// Clock support routines
//

#define KiWriteHighLowHigh(Destination, Source) \
    WriteNoFence(&(Destination).High2Time, ((PLARGE_INTEGER)(Source))->HighPart); \
    WriteULongRelease(&(Destination).LowPart, ((PLARGE_INTEGER)(Source))->LowPart); \
    WriteRelease(&(Destination).High1Time, ((PLARGE_INTEGER)(Source))->HighPart) \

#define KiWriteInterruptTime(NewTime) \
{ \
    KiWriteHighLowHigh(SharedUserData->InterruptTime, NewTime); \
}

#define KiWriteSystemTime(NewTime) \
{ \
    KiWriteHighLowHigh(SharedUserData->SystemTime, NewTime); \
}

#define KiWriteTickCount(NewTick) \
{ \
    KiWriteHighLowHigh(KeTickCount, NewTick); \
    KiWriteHighLowHigh(SharedUserData->TickCount, NewTick); \
}

//
// Define query interrupt time, system time, and tick count macros.
//
// N.B. System time and low tick count queries are in common platform code.
//

#define KiQueryInterruptTime(CurrentTime) \
    KiReadHighLowHigh(CurrentTime, &SharedUserData->InterruptTime)

#define KiQueryTickCount(CurrentCount) \
    KiReadHighLowHigh(CurrentCount, &KeTickCount)

//
// Spinlock support.
//

#if !DBG

#define TRY_ACQUIRE_SPINLOCK(SpinLock) \
    (InterlockedExchangeAcquire((LONG *)SpinLock, 1) == 0)

#endif

//
// Write to the spinlock with a release fence.
//

#define RELEASE_SPINLOCK_OP(SpinLock) \
    WriteULongPtrRelease(SpinLock, 0)

//
// Stack expansion structure support.
//

#define KiTrapFrameStackPointer(TrapFrame) \
    (TrapFrame->Sp)

#define KiTrapFrameReturnAddress(TrapFrame) \
    (TrapFrame->Pc)

#define KiCalloutFrameStackPointer(CalloutFrame) \
    (CalloutFrame->MachineFrame.Sp)

#define KiCalloutFrameReturnAddress(CalloutFrame) \
    (CalloutFrame->MachineFrame.Pc)


#endif // !defined(_KIARM_)

