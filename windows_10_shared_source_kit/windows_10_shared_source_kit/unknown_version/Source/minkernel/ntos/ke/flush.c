/*++

Copyright (c) 2000 - 2006  Microsoft Corporation

Module Name:

    flush.c

Abstract:

    This module implements ARM machine dependent kernel functions to
    flush the caches on all processors.

Environment:

    Kernel mode only.

--*/

#include "ki.h"

//
// Windows terminology:
//
//   The kernel uses ambiguous terms to refer to cache operations.  In
//   particular, it uses 'Invalidate' when the intention is to 'clean
//   and invalidate'.  To avoid confusion, this code uses
//   'Void' to refer to hardware invalidation of cache data.
//
// ARM terminology:
//
//   Point of Coherency (PoC) - the point at which the imposition of
//      further cache is transparent for instruction, data, PTE walks
//      to that address by any processor in the system.
//
//   Point of Unification (PoU) - the point at which the instruction,
//      data, and PTE walks have merged in a uniprocessor system.
//
//    Level of Coherency (LoC) - First level that does not need to be
//      cleaned/invalidated when clean/invalidating to the point of
//      coherency
//
//    Level of Unification (LoU) - First level that does not need to
//      be cleaned/invalidated when clean/invalidating to the point of
//      unification
//

VOID
KiExternalCacheFlushQuiesce (
    __in struct _KDPC *Dpc,
    __in_opt PVOID DeferredContext,
    __in_opt PVOID SystemArgument1,
    __in_opt PVOID SystemArgument2
    );

#pragma data_seg("ALMOSTRO")

//
// Cache type as retrieved from the BSP processor
//

ARM_CACHE_TYPE KiCacheType = {0};

//
// Minimum instruction and data cache line
//

ULONG KiMinimumIcacheLine = 0;
ULONG KiMinimumDcacheLine = 0;

//
// A value indicating the threshold after which a flush by VA should
// be promoted to a full flush operation.
//

ULONG KiCacheFlushReadPromoteThreshold = ULONG_MAX;

//
// A value indicating the threshold after which a flush by VA should
// be promoted to a full flush operation.
//

ULONG KiCacheFlushWritePromoteThreshold = ULONG_MAX;

//
// A value indicating the threshold after which ICache management by
// be promoted to a full flush operation.
//
//

ULONG KiICacheFlushPromoteThreshold = ULONG_MAX;

//
// A flag forcing the promotion of KeFlushIoBuffers to full cache flushes.
//

BOOLEAN KiCacheFlushForcePromote = FALSE;

//
// A flag indicating the presence of an initialized external cache
//

BOOLEAN KiExternalCachePresent = FALSE;

//
// Threshold for external cache flushing quiescing
//

ULONG KiExternalCacheQuiesceThreshold = 32*1024;

//
// Indicates whether quiesces should be performed at high importance
//

ULONG KiExternalCacheQuiesceHighImportanceEnable = 0;

//
// Structure describing the external cache if present
//

CACHE_DESCRIPTOR KiExternalCacheDescriptor = {0};

//
// A flag indicating whether all processors need to synchronously
// block on a full external cache flush.
//

BOOLEAN KiExternalCacheGlobalSyncRequired = FALSE;

#pragma data_seg()

//
// Count of flush promotions for cause.
//

ULONG KiCacheFlushReadPromoteCount;

//
// Count of flush promotions for cause.
//

ULONG KiCacheFlushWritePromoteCount;

//
// A value indicating whether there are external cache flushes
// outstanding and thus a quiescent state is reqiured.
//

LONG KiExternalCacheFlushOutstanding;

//
// Per processor DPCs for quiescing cores.
//

KDPC KiExternalCacheQuiesceDpc[MAXIMUM_PROC_PER_SYSTEM];

#pragma alloc_text(PAGELK, KiCacheInitialize)

ULONG
KiFlushRangeFilter (
    __in PEXCEPTION_POINTERS ExceptionPointers,
    __inout PVOID *BaseAddress,
    __inout PSIZE_T Length,
    __inout PLOGICAL Retry
    )

/*++

Routine Description:

    This is the exception handler used by routines flushing cache
    lines by VA to protect against bad virtual addresses.  If an
    access violation occurs, this routine causes the cache routine to
    restart the sweep at the page following the failing page.

Arguments:

    ExceptionPointers - Supplies exception information.

    BaseAddress - Supplies a pointer to address the base of the region
        being flushed.  If the failing address is not in the last page
        of the region, this routine updates BaseAddress to point to
        the next page of the region.

    Length - Supplies a pointer the length of the region being
        flushed.  If the failing address is not in the last page of
        the region, this routine updates Length to reflect restarting
        the flush at the next page of the region.

    Retry - Supplies a pointer to a LOGICAL that the caller has
        initialized to FALSE.  This routine sets this LOGICAL to TRUE
        if an access violation occurs in a page before the last page
        of the flush region.

Return Value:

    EXCEPTION_EXECUTE_HANDLER.

--*/

{

    PEXCEPTION_RECORD ExceptionRecord;
    ULONG_PTR BadVa;
    ULONG_PTR NextVa;
    ULONG_PTR EndVa;

    ExceptionRecord = ExceptionPointers->ExceptionRecord;

    //
    // If the exception was an access violation, skip the current page of the
    // region and move to the next page.
    //

    if (ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION) {

        //
        // Get the failing address, calculate the base address of the
        // next page, and calculate the address at the end of the
        // region.
        //

        BadVa = ExceptionRecord->ExceptionInformation[1];
        NextVa = ROUND_TO_PAGES (BadVa + 1);
        EndVa = *(PULONG_PTR) BaseAddress + *Length;

        //
        // If the next page didn't wrap, and the next page is below
        // the end of the region, update Length and BaseAddress
        // appropriately and set Retry to TRUE to indicate to
        // NtFlushInstructionCache that it should call
        // KeSweepIcacheRange again.
        //

        if ((NextVa > BadVa) && (NextVa < EndVa)) {
            *Length = (ULONG) (EndVa - NextVa);
            *BaseAddress = (PVOID) NextVa;
            *Retry = TRUE;
        }
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

KINLINE
VOID
KiCleanDcacheRangePoC (
    __in PCHAR Current,
    __in PCHAR End
    )

/*++

Routine Description:

    This routine is invoked to clean the cache lines within a
    specified VA range back to the Point of Coherency.

Arguments:

    Current - Supplies a start to the VA range

    End - Supplies the end to the VA range (not included in range)

Return Value:

    None.

--*/

{

    ULONG LineSize;

    LineSize = KiMinimumDcacheLine;
    Current = (PCHAR)((ULONG_PTR)Current & ~(LineSize - 1));
    while (Current < End) {
        _MoveToCoprocessor((ULONG)Current, CP15_DCCMVAC);
        Current += LineSize;
    }

    return;
}

KINLINE
VOID
KiCleanDcacheRangePoU (
    __in PCHAR Current,
    __in PCHAR End
    )

/*++

Routine Description:

    This routine is invoked to clean the cache lines within a
    specified VA range back to the Point of Unification.

Arguments:

    Current - Supplies a start to the VA range

    End - Supplies the end to the VA range (not included in range)

Return Value:

    None.

--*/

{

    ULONG LineSize;

    LineSize = KiMinimumDcacheLine;
    Current = (PCHAR)((ULONG_PTR)Current & ~(LineSize - 1));
    while (Current < End) {
        _MoveToCoprocessor((ULONG)Current, CP15_DCCMVAU);
        Current += LineSize;
    }

    return;
}

KINLINE
VOID
KiCleanInvalidateDcacheRangePoC (
    __in PCHAR Current,
    __in PCHAR End
    )

/*++

Routine Description:

    This routine is invoked to clean and invalidate cache lines within
    a specified VA range back to the Point of Coherency.

Arguments:

    Current - Supplies a start to the VA range

    End - Supplies the end to the VA range (not included in range)

Return Value:

    None.

--*/

{

    ULONG LineSize;

    LineSize = KiMinimumDcacheLine;
    Current = (PCHAR)((ULONG_PTR)Current & ~(LineSize - 1));
    while (Current < End) {
        _MoveToCoprocessor((ULONG)Current, CP15_DCCIMVAC);
        Current += LineSize;
    }

    return;
}

KINLINE
VOID
KiVoidDcacheRangePoC (
    __in PCHAR Current,
    __in PCHAR End
    )

/*++

Routine Description:

    This routine is invoked to invalidate cache lines within thea
    specified VA range through to the Point of Coherency.

Arguments:

    Current - Supplies a start to the VA range

    End - Supplies the end to the VA range (not included in range)

Return Value:

    None.

--*/

{

    ULONG LineSize;

    LineSize = KiMinimumDcacheLine;
    Current = (PCHAR)((ULONG_PTR)Current & ~(LineSize - 1));
    while (Current < End) {
        _MoveToCoprocessor((ULONG)Current, CP15_DCIMVAC);
        Current += LineSize;
    }

    return;
}

DECLSPEC_NOINLINE
VOID
KiVoidBroadcastIcacheRangePoU (
    __in PCHAR Current,
    __in PCHAR End
    )

/*++

Routine Description:

    This routine is invoked to invalidate the local instruction cache
    lines within a specified VA range to Point of Unification.

Arguments:

    Current - Supplies a start to the VA range

    End - Supplies the end to the VA range (not included in range)

Return Value:

    None.

--*/

{

    ULONG LineSize;

    LineSize = KiMinimumIcacheLine;
    Current = (PCHAR)((ULONG_PTR)Current & ~(LineSize - 1));
    while (Current < End) {
        _MoveToCoprocessor((ULONG)Current, CP15_ICIMVAU);
        Current += LineSize;
    }

    return;
}

VOID
KiVoidIcacheRangePoUWithCatch (
    __in PCHAR BaseAddress,
    __in ULONG Length
    )

/*++

Routine Description:

    This routine is invoked to invalidate the instruction cache
    lines within a specified VA range to Point of Unification.

Arguments:

    Current - Supplies a start to the VA range

    End - Supplies the end to the VA range

Return Value:

    None.

--*/

{

    PCHAR End;
    PCHAR RangeBase;
    ULONG RangeLength;
    LOGICAL Retry;

    RangeBase = BaseAddress;
    RangeLength = Length;

    do {
        Retry = FALSE;
        End = RangeBase + RangeLength;

        __try {
            KiVoidBroadcastIcacheRangePoU(ReadPointerNoFence(&RangeBase), End);

        } __except (KiFlushRangeFilter(GetExceptionInformation(),
                                       &RangeBase,
                                       &RangeLength,
                                       &Retry)) {

            NOTHING;
        }

    } while (Retry != FALSE);

    return;
}

KINLINE
VOID
KiVoidLocalIcachePoU (
    VOID
    )

/*++

Routine Description:

    This routine is invoked to invalidate the local instruction cache
    to Point of Unification.

Arguments:

    None.

Return Value:

    None.

--*/

{

    _MoveToCoprocessor(0, CP15_ICIALLU);
    _InvalidateBTAC();
    _DataSynchronizationBarrier();
    _InstructionSynchronizationBarrier();

    return;
}

KINLINE
VOID
KiVoidBroadcastIcachePoU (
    VOID
    )

/*++

Routine Description:

    This routine is invoked to invalidate the instruction cache on all
    processors that are in the inner shareable domain to the Point of Unification.

Arguments:

    None.

Return Value:

    None.

--*/

{

    _MoveToCoprocessor(0, CP15_ICIALLUIS);
    _InvalidateBTAC();
    _DataSynchronizationBarrier();
    _InstructionSynchronizationBarrier();

    return;
}

VOID
KiVoidIcacheRangePoU (
    __in PCHAR BaseAddress,
    __in ULONG Length
    )

/*++

Routine Description:

    This routine is invoked to invalidate the instruction cache lines
    within a specified VA range to Point of Unification.  Depending on
    the underlying instruction cache type, this routine may promote the
    operation to a full ICache invalidate.

Arguments:

    Current - Supplies a start to the VA range

    End - Supplies the end to the VA range

Return Value:

    None.

--*/

{

    if (KiCacheType.IPolicy == ARM_L1_ICACHE_PIPT) {

        //
        // On a PIPT ICache, the icache flushes should have the effect of
        // flushing all of the aliases of the target icache line.
        //

        KiVoidIcacheRangePoUWithCatch(BaseAddress, Length);
        _InvalidateBTAC();
        _DataSynchronizationBarrier();
        _InstructionSynchronizationBarrier();

    } else {

        //
        // On a VIPT ICache, only a full icache flush has the effect of
        // flushing all of the aliases of the target icache line.
        //

        KiVoidBroadcastIcachePoU();
    }

    return;
}

KINLINE
VOID
KiSweepDcacheBySetWay (
    _In_ ULONG FlushLevel,
    _In_ BOOLEAN Invalidate
    )

/*++

Routine Description:

    This function performs the target cache operation on the current
    Dcache until the specified cache level.

Arguments:

    FlushLevel - XXX Flush level definition

    Invalidate - Specifies whether the cache should be invalidated
                 after being flushed

Return Value:

    None.

Note:

    This routine can be called at high IRQL.

--*/

{

    ARM_CACHE_INFO CacheInfo;
    ARM_CACHE_INFO_SELECTOR CacheSelector;
    ULONG Level;
    ULONG OpValue;
    ULONG Set;
    ULONG SetShift;
    ULONG Value;
    ULONG Way;
    ULONG WayShift;

    Level = 0;
    while (Level < FlushLevel) {
        CacheSelector.Ulong = 0;
        CacheSelector.Level = Level;
        CacheSelector.Select = ARM_CACHE_SELECT_DATA_OR_UNIFIED;

        _MoveToCoprocessor(CacheSelector.Ulong, CP15_CSSELR);
        _InstructionSynchronizationBarrier();

        CacheInfo.Ulong = _MoveFromCoprocessor(CP15_CCSIDR);

        Way = CacheInfo.Associativity + 1;

        //
        // Cache operation register format
        //
        // 31 ----- 32-A 31-A ------ L+S L+S-1 --- L  L-1 ---- 4 3 Level 1 SBZ
        //     WAY             SBZ             SET        SBZ      Level
        // where A = log2n # of ways
        //       L = log2n line size
        //       S = log2n # of sets

        Value = Way;
        WayShift = 0;
        while (Value >>= 1) {
            WayShift += 1;
        }

        if (Way & (Way - 1)) {
            WayShift += 1;
        }

        WayShift = 32 - WayShift;
        SetShift = CacheInfo.LineSize + 4;

        do {

            Way -= 1;
            Set = CacheInfo.NumberOfSets + 1;

            do {
                Set -= 1;

                OpValue = (Level << 1)       |
                          (Way << WayShift)  |
                          (Set << SetShift);

                if (Invalidate) {

                    _MoveToCoprocessor(OpValue, CP15_DCCISW);
                } else {

                    _MoveToCoprocessor(OpValue, CP15_DCCSW);
                }

            } while (Set != 0);
        } while (Way != 0);

        Level += 1;
    }

    return;
}

VOID
KiCleanCacheRangePoC (
    __in PCHAR BaseAddress,
    __in ULONG Length
    )

/*++

Routine Description:

    This function writes back the caches to main memory across a range
    of virtual addresses to Point of Coherency.

Arguments:

    BaseAddress - Supplies a pointer to the base of the range that is flushed.

    Length - Supplies the length of the range that is flushed if the base
        address is specified.

Return Value:

    None.

--*/

{

    KiCleanDcacheRangePoC(BaseAddress, BaseAddress + Length);
    _DataSynchronizationBarrier();

    return;
}


VOID
KiInvalidateCurrentCache (
    VOID
    )

/*++

Routine Description:

    This function writes back and invalidates the caches
    on the current processor

Arguments:

    None.

Return Value:

    None.

--*/

{

    ARM_CACHE_LEVEL_INFO LevelInfo;
    ULONG LevelOfCoherency;

    NT_ASSERT(KeGetCurrentIrql() >= DISPATCH_LEVEL);

    //
    // N.B. This information is not cached to enable big/little scenarios
    //

    LevelInfo.Ulong = _MoveFromCoprocessor(CP15_CLIDR);
    LevelOfCoherency = LevelInfo.LoC;

    //
    // Clean and invalidate data cache to Point of Coherency.
    //

    KiSweepDcacheBySetWay(LevelOfCoherency, TRUE);
    _DataSynchronizationBarrier();

    //
    // Dump instruction cache
    //

    KiVoidLocalIcachePoU();

    return;
}

VOID
KiInvalidateCacheRange (
    __in PCHAR BaseAddress,
    __in ULONG Length
    )

/*++

Routine Description:

    This function writes back and invalidates a cache of virtual addresses
    on the current processor

Arguments:

    BaseAddress - Supplies a pointer to the base of the range that is flushed.

    Length - Supplies the length of the range that is flushed if the base
        address is specified.

Return Value:

    None.

--*/

{

    //
    // ARM instruction caches do not snoop the data cache.  Therefore,
    // the data cache must first be flushed back to the to the
    // implicit point of unification before invalidating the
    // instruction cache
    //

    KiCleanInvalidateDcacheRangePoC(BaseAddress, BaseAddress + Length);
    _DataSynchronizationBarrier();

    //
    // Ensure the Icache is consistent with the Dcache contents.
    //

    KiVoidIcacheRangePoU(BaseAddress, Length);
    _InvalidateBTAC();
    _DataSynchronizationBarrier();
    _InstructionSynchronizationBarrier();

    return;
}

VOID
KiVoidCacheRange (
    __in PCHAR BaseAddress,
    __in ULONG Length
    )

/*++

Routine Description:

    This function invalidates a cache of virtual addresses
    on the current processor

Arguments:

    BaseAddress - Supplies a pointer to the base of the range that is flushed.

    Length - Supplies the length of the range that is flushed if the base
        address is specified.

Return Value:

    None.

--*/

{

    //
    // ARM instruction caches do not snoop the data cache.  Therefore,
    // the instruction cache must be invalidated after the dcache operations
    // are complete.
    //

    KiVoidDcacheRangePoC(BaseAddress, BaseAddress + Length);
    _DataSynchronizationBarrier();

    //
    // Ensure the Icache is consistent with the Dcache contents.
    //

    KiVoidIcacheRangePoU(BaseAddress, Length);
    _InvalidateBTAC();
    _DataSynchronizationBarrier();
    _InstructionSynchronizationBarrier();

    return;
}

VOID
KiSweepCurrentIcache (
    VOID
    )

/*++

Routine Description:

    This function makes the instruction cache coherent with the data
    cache on the current logical processor.

Arguments:

    None.

Return Value:

    None.

Note:

    This routine can be called at high IRQL.

--*/

{

    ARM_IDCODE Id;
    ARM_CACHE_LEVEL_INFO LevelInfo;
    ULONG LevelOfUnification;

    NT_ASSERT(KeGetEffectiveIrql() >= DISPATCH_LEVEL);

    //
    // N.B. This information is not cached to enable big/little scenarios
    //

    LevelInfo.Ulong = _MoveFromCoprocessor(CP15_CLIDR);
    Id.Ulong = _MoveFromCoprocessor (CP15_MIDR);
    LevelOfUnification = LevelInfo.LoUIS;
    if ((Id.Model == 0xC09) &&
        (Id.Revision < 1) &&
        (Id.Implementer == 'A')) {

        //
        // Cortex-A9 Errata #643719 - LoUIS field in CLIDR register is
        // incorrect.
        //

        LevelOfUnification = LevelInfo.LoU;
    }

    //
    // Clean data caches to Point of Unification.
    //

    KiSweepDcacheBySetWay(LevelOfUnification, FALSE);
    _DataSynchronizationBarrier();

    //
    // Void instruction cache to pick up changes from data cache.
    //

    KiVoidLocalIcachePoU();

    return;
}


VOID
KiCleanCurrentCache (
    VOID
    )

/*++

Routine Description:

    This function cleans the architectural caches to the PoC on the current
    logical processor.

Arguments:

    None.

Return Value:

    None.

Note:

    This routine can be called at high IRQL.

--*/

{

    ARM_CACHE_LEVEL_INFO LevelInfo;
    ULONG LevelOfCoherency;

    NT_ASSERT(KeGetEffectiveIrql() >= DISPATCH_LEVEL);

    //
    // N.B. This information is not cached to enable big/little scenarios
    //

    LevelInfo.Ulong = _MoveFromCoprocessor(CP15_CLIDR);
    LevelOfCoherency = LevelInfo.LoC;

    //
    // Clean data caches to Point of Coherency
    //

    KiSweepDcacheBySetWay(LevelOfCoherency, FALSE);
    _DataSynchronizationBarrier();
    return;
}

VOID
KiSweepIcacheRange (
    __in PCHAR BaseAddress,
    __in ULONG Length
    )

/*++

Routine Description:

    This function is used to make a range of virtual addresses from
    the instruction cache consistent with the data cache on the
    current processor

Arguments:

    BaseAddress - Supplies a pointer to the base of the range that is flushed.

    Length - Supplies the length of the range that is flushed if the base
        address is specified.

Return Value:

    None.

--*/

{

    //
    // ARM instruction caches do not snoop the data cache.  Therefore,
    // the data cache must first be flushed back to the to the
    // implicit point of unification before invalidating the
    // instruction cache
    //

    KiCleanDcacheRangePoU(BaseAddress, BaseAddress + Length);
    _DataSynchronizationBarrier();

    //
    // Ensure the Icache is consistent with the Dcache contents.
    //

    KiVoidIcacheRangePoU(BaseAddress, Length);

    return;
}

VOID
KiInvalidateAllCachesTarget (
    IN PULONG SignalDone,
    IN PVOID Parameter1,
    IN PVOID Parameter2,
    IN PVOID Parameter3
    )

/*++

Routine Description:

    This is the target function for sweeping both data and instruction
    caches on target processors.

Arguments:

    SignalDone Supplies a pointer to a variable that is cleared when the
        requested operation has been performed.

    Parameter1 - Supplies a pointer to a barrier used to signal the completion
        of the flush.

    Parameter2 - Parameter3 - Not used.

Return Value:

    None.

--*/

{

    LONG CurrentFlushTimeStamp;
    BOOLEAN Enabled;

    UNREFERENCED_PARAMETER (SignalDone);
    UNREFERENCED_PARAMETER (Parameter2);
    UNREFERENCED_PARAMETER (Parameter3);

    CurrentFlushTimeStamp = ReadAcquire(&KiCacheFlushTimeStamp);
    KiInvalidateCurrentCache();

    //
    // If global flush synchronization is enabled, then Wait for the leader to
    // finish flushing the external cache (with interrupts disabled) before
    // leaving the cache flush routine.
    //

    Enabled = KeDisableInterrupts();
    InterlockedDecrement((volatile LONG *)Parameter1);
    if (KiExternalCacheGlobalSyncRequired != FALSE) {
        while (CurrentFlushTimeStamp == ReadNoFence(&KiCacheFlushTimeStamp)) {
            YieldProcessor();
        }

        KeMemoryBarrier();
    }

    if (Enabled) {
        _enable();
    }

    return;
}

BOOLEAN
KeInvalidateAllCaches (
    VOID
    )

/*++

Routine Description:

    This function writes back and invalidates the cache on all
    processors in the host configuration.

Arguments:

    None.

Return Value:

    TRUE is returned as the function value.

--*/

{

    BOOLEAN Enabled;
    volatile LONG FlushBarrier;
    KIRQL OldIrql;
    ULONG SpinCount;

    //
    // N.B. Do not modify this routine to support running at HIGH_LEVEL to
    //      support crashdump/hibernate until the A9 errata managed by
    //      KiExternalCacheGlobalSyncRequired is no longer possible.
    //
    //      A version that flushes and invalidates the current cache
    //      and then flushes the external cache while the other
    //      processors are generating other write traffic or have
    //      dirty data will encounter the same errata and corrupt memory.
    //

    NT_ASSERT(KeGetCurrentIrql() <= SYNCH_LEVEL);

    //
    // If this isn't a crashdump or hibernate scenario, then we can
    // IPI the other processors.
    //

    OldIrql = KeRaiseIrqlToSynchLevel();

    //
    // Set time stamp busy if possible otherwise another processor did the
    // flush for us.
    //

    if (KiSetCacheFlushTimeStampBusy()) {
        if (KiExternalCacheGlobalSyncRequired != FALSE) {
            KeAcquireSpinLockAtDpcLevel(&KiReverseStallIpiLock);
        }

        //
        // Compute the set of target processors and send the sweep parameters
        // to the target processors, if any, for execution.
        //

        WriteRaw(&FlushBarrier,
                 KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS) - 1);

        if (ReadRaw(&FlushBarrier) != 0) {
            KiIpiSendPacket(IpiAllButSelf,
                            NULL,
                            KiInvalidateAllCachesTarget,
                            (PVOID)&FlushBarrier,
                            NULL,
                            NULL);
        }

        KiInvalidateCurrentCache();

        //
        // Wait until all target processors have finished sweeping their
        // data caches.
        //

        SpinCount = 0;
        while (ReadNoFence(&FlushBarrier) != 0) {
            KeYieldProcessorEx(&SpinCount);
        }

        KeMemoryBarrier();

        //
        // Flush and invalidate this all the way out in case this is a
        // cache attribute change.
        //

        Enabled = KeDisableInterrupts();
        HalFlushExternalCache(TRUE);
        if (Enabled) {
            _enable();
        }

        //
        // Clear cache time stamp busy and lower IRQL to its previous value.
        //
        // N.B. The target processors will reverse stall until the cache flush
        //      timestamp advances.
        //

        KiClearCacheFlushTimeStampBusy();
        if (KiExternalCacheGlobalSyncRequired != FALSE) {
            KeReleaseSpinLockFromDpcLevel(&KiReverseStallIpiLock);
        }
    }

    KeLowerIrql(OldIrql);
    return TRUE;
}

VOID
KiCleanAllCachesTarget (
    __in PULONG SignalDone,
    __in PVOID Parameter1,
    __in PVOID Parameter2,
    __in PVOID Parameter3
    )

/*++

Routine Description:

    This is the target function for cleaning caches on target processors.

Arguments:

    SignalDone Supplies a pointer to a variable that is cleared when the
        requested operation has been performed.

    Parameter1 - Supplies a pointer to a barrier used to signal the completion
        of the flush.

    Parameter2 - Parameter3 - Not used.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER (SignalDone);
    UNREFERENCED_PARAMETER (Parameter2);
    UNREFERENCED_PARAMETER (Parameter3);

    KiCleanCurrentCache();
    InterlockedDecrement((volatile LONG *)Parameter1);
    return;
}

VOID
KiCleanAllCaches (
    VOID
    )

/*++

Routine Description:

    This function cleans the cache on all processors in the host
    configuration.

Arguments:

    None.

Return Value:

    None.

--*/

{

    volatile LONG FlushBarrier;
    KIRQL OldIrql;
    ULONG SpinCount;

    //
    // N.B. This code path is not capable of being run at HIGH_LEVEL and
    //      must not be invoked from the crashdump/hibernate path.

    NT_ASSERT(KeGetCurrentIrql() <= SYNCH_LEVEL);

    OldIrql = KeRaiseIrqlToSynchLevel();

    //
    // Compute the set of target processors and send the clean parameters
    // to the target processors, if any, for execution.
    //

    WriteRaw(&FlushBarrier,
             KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS) - 1);

    if (ReadRaw(&FlushBarrier) != 0) {
        KiIpiSendPacket(IpiAllButSelf,
                        NULL,
                        KiCleanAllCachesTarget,
                        (PVOID)&FlushBarrier,
                        NULL,
                        NULL);
    }

    //
    // Wait until all target processors have finished sweeping their
    // data caches.
    //

    SpinCount = 0;
    while (ReadNoFence(&FlushBarrier) != 0) {
        KeYieldProcessorEx(&SpinCount);
    }

    //
    // It isn't safe to drop the interrupt level below dispatch since the cache
    // flush must occur on the current processor.  However, interrupts may be
    // safely permitted to nest on top of the cache operation.
    //

    if (OldIrql >= DISPATCH_LEVEL) {
        KeLowerIrql(OldIrql);

    } else {
        KeLowerIrql(DISPATCH_LEVEL);
    }

    //
    // Clean the architectural cache
    //

    KiCleanCurrentCache();
    KeLowerIrql(OldIrql);

    //
    // Clean the external cache if present
    //

    HalFlushExternalCache(FALSE);
    return;
}

extern
KINLINE
VOID
KeSweepLocalCaches (
    VOID
    )

/*++

Routine Description:

    This routine flushes and invalidates the caches on the currently
    running processor.

Arguments:

    None.

Return Value:

    None.

--*/

{

    NT_ASSERT(KeGetEffectiveIrql() >= DISPATCH_LEVEL);

    KiInvalidateCurrentCache();

    return;
}

VOID
KiCleanRange (
    __in PVOID BaseAddress,
    __in ULONG Length
    )

/*++

Routine Description:

    This function flushes the specified range of addresses from data
    cache on all processors in the system.

Arguments:

    Base - Supplies a pointer to the start address of the range of adddreses
        to flush.

    Length - Supplies the length of the range of addresses to flush in bytes.

Return Value:

    None.

--*/

{

    //
    // Broadcast operations to clean cache range on all processors.
    //

    KiCleanCacheRangePoC((PCHAR)BaseAddress, (ULONG)Length);
    return;
}

VOID
KiVoidRangeDCaches (
    __in PVOID BaseAddress,
    __in ULONG Length
    )

/*++

Routine Description:

    This function invalidates the specified range of addresses from
    all data caches in the system.

Arguments:

    Base - Supplies a pointer to the start address of the range of adddreses
        to flush.

    Length - Supplies the length of the range of addresses to flush in bytes.

Return Value:

    None.

--*/

{

    //
    // Invalidate the DCache to all levels of the cache hierarchy.
    //

    KiVoidDcacheRangePoC(BaseAddress, ((PCHAR)BaseAddress) + Length);
    _DataSynchronizationBarrier();
    return;
}

VOID
KiVoidRangeAllCaches (
    __in PVOID BaseAddress,
    __in ULONG Length
    )

/*++

Routine Description:

    This function flushes the specified range of addresses from all caches
    in the system.

Arguments:

    Base - Supplies a pointer to the start address of the range of adddreses
        to flush.

    Length - Supplies the length of the range of addresses to flush in bytes.

Return Value:

    None.

--*/

{

    //
    // Broadcast operations to invalidate cache range on all processors.
    //

    KiVoidCacheRange((PCHAR)BaseAddress, (ULONG)Length);
    return;
}

VOID
KeInvalidateRangeAllCaches (
    __in PVOID BaseAddress,
    __in ULONG Length
    )

/*++

Routine Description:

    This function flushes the specified range of addresses from all caches
    in the system.

Arguments:

    Base - Supplies a pointer to the start address of the range of adddreses
        to flush.

    Length - Supplies the length of the range of addresses to flush in bytes.

Return Value:

    None.

--*/

{

    if ((KeFeatureBits & KF_PROMOTE_ATTRIBUTE_FLUSH) != 0) {
        KeInvalidateAllCaches ();
        return;
    }

    //
    // Broadcast operations to clean and invalidate cache range on all
    // processors.
    //

    KiInvalidateCacheRange((PCHAR)BaseAddress, (ULONG)Length);

    //
    // Flush and invalidate the external cache if present.
    //
    // N.B. This is sub-optimal, but necessary for correctness. If the
    //      physical address were provided, a more targeted flush and
    //      invalidate could be performed.
    //

    HalFlushExternalCache(TRUE);
    return;
}

VOID
KeInvalidatePage (
    __in PVOID VirtualAddress,
    __in PHYSICAL_ADDRESS PhysicalAddress
    )

/*++

Routine Description:

    This function flushes the specified page from all caches in the system.

Arguments:

    VirtualAddress - Supplies the virtual address for the page.

    PhysicalAddress - Supplies the physical address for the page.

Return Value:

    None.

--*/

{

    NT_ASSERT(((ULONG_PTR)VirtualAddress & (PAGE_SIZE-1)) == 0);
    NT_ASSERT((PhysicalAddress.QuadPart & (PAGE_SIZE-1)) == 0);

    if ((KeFeatureBits & KF_PROMOTE_ATTRIBUTE_FLUSH) != 0) {
        KeInvalidateAllCaches ();
        return;
    }

    //
    // Broadcast operations to clean and invalidate cache range on all
    // processors.
    //

    KiInvalidateCacheRange((PCHAR)VirtualAddress, (ULONG)PAGE_SIZE);

    //
    // Flush this page from the external cache
    //

    HalFlushAndInvalidatePageExternalCache(PhysicalAddress);
    return;
}

VOID
KeSweepCurrentIcache (
    VOID
    )

/*++

Routine Description:

    This function makes the instruction cache coherent with the data
    cache.  It is generally called by the debugger.

Arguments:

    None.

Return Value:

    None.

Note:

    This routine only works correctly if it is called on the same processor
    that made the modification to the instruction memeory

    This routine can be called at high IRQL.

--*/

{

    KiSweepCurrentIcache();
    return;
}

VOID
KiSweepIcacheTarget (
    IN PULONG SignalDone,
    IN PVOID Parameter1,
    IN PVOID Parameter2,
    IN PVOID Parameter3
    )

/*++

Routine Description:

    This is the target function for sweeping the instruction cache on
    target processors.

Arguments:

    SignalDone Supplies a pointer to a variable that is cleared when the
        requested operation has been performed.

    Parameter1 - Parameter3 - Not used.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER (Parameter1);
    UNREFERENCED_PARAMETER (Parameter2);
    UNREFERENCED_PARAMETER (Parameter3);

    //
    // Sweep the instruction cache on the current processor and clear
    // the sweep instruction cache packet address to signal the source
    // to continue.
    //

    KiSweepCurrentIcache();

    KiIpiSignalPacketDone(SignalDone);

    return;
}

VOID
KeSweepIcache (
    __in BOOLEAN AllProcessors
    )

/*++

Routine Description:

    This function writes back and invalidates the data cache on all
    processors in the host configuration.

Arguments:

    None.

--*/
{

    KIRQL OldIrql;
    ULONG TargetCount;

    UNREFERENCED_PARAMETER(AllProcessors);

    //
    // N.B. This code path is not capable of being run at HIGH_LEVEL and
    //      must not be invoked from the crashdump/hibernate path.

    NT_ASSERT(KeGetCurrentIrql() <= SYNCH_LEVEL);

    OldIrql = KeRaiseIrqlToSynchLevel();

    //
    // Compute the set of target processors and send the sweep parameters
    // to the target processors, if any, for execution.
    //

    TargetCount = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS) - 1;
    if (TargetCount != 0) {
        KiIpiSendPacket(IpiAllButSelf,
                        NULL,
                        KiSweepIcacheTarget,
                        NULL,
                        NULL,
                        NULL);
    }

    KiSweepCurrentIcache();

    //
    // Wait until all target processors have finished sweeping their
    // instruction caches.
    //

    if (TargetCount != 0) {
        KiIpiStallOnPacketTargets(FALSE);
    }

    KeLowerIrql(OldIrql);
    return;
}

VOID
KeSweepIcacheRange (
    __in BOOLEAN AllProcessors,
    __in PVOID BaseAddress,
    __in SIZE_T Length
    )

/*++

Routine Description:

    This function is used to flush a range of virtual addresses from the
    primary instruction cache on all processors in the host configuration.

Arguments:

    AllProcessors -  Not used

    BaseAddress - Supplies a pointer to the base of the range that is flushed.

    Length - Supplies the length of the range that is flushed if the base
        address is specified.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(AllProcessors);

    //
    // If the length of the request is greater than a computed
    // threshold, promote to a full icache flush which can be faster.
    //

    if (Length > KiICacheFlushPromoteThreshold) {
        KeSweepIcache(TRUE);

    } else {
        KiSweepIcacheRange(BaseAddress, Length);
    }

    return;
}

VOID
KeFlushIoBuffers (
    __in PMDL Mdl,
    __in BOOLEAN ReadOperation,
    __in BOOLEAN DmaOperation
    )

/*++

Routine Description:

   This function flushes the I/O buffer specified by the memory descriptor
   list from the data cache on the processor which executes.

   The external documentation for this API says it can only be called
   at DISPATCH_LEVEL or lower.  In actuality the hibernate/crashdump
   stacks depend on this function working at HIGH_LEVEL while the
   other processors are coralled somewhere.  This requirement extends
   to all downstream functionality as well.

   WIN9MQ: Code has been worked over too many times to meet new
           requirements.  A refactoring of this code would divide the
           code into portions and/or subfunctions for decision making,
           MDL flush, full flush, and maybe crash dump.  Also
           consideration should be given to breaking out the breaking
           out external cache path (A9) entirely as that will
           eventually be EOLed and is 32 bit specific whereas the rest
           of this code could be shared with ARM64.

Arguments:

   Mdl - Supplies a pointer to a memory descriptor list that describes the
       I/O buffer location.

   ReadOperation - Supplies a boolean value that determines whether the I/O
       operation is a read into memory.

   DmaOperation - Supplies a boolean value that deternines whether the I/O
       operation is a DMA operation.

Return Value:

   None.

--*/

{

    PCHAR BaseAddress;
    KIRQL CurrentIrql;
    KAFFINITY_EX DeepIdleMask;
    KAFFINITY_ENUMERATION_CONTEXT EnumerationContext;
    BOOLEAN ExternalCachePresent;
    ULONG Index;
    BOOLEAN IrqlRaised;
    ULONG LargestCacheLine;
    ULONG Length;
    BOOLEAN LoggingEnabled;
    KIRQL OldIrql;
    BOOLEAN QuiesceFlush;
    ETW_KERNEL_TRACE_TIMESTAMP StartTimeStamp;
    KAFFINITY_EX TargetMask;
    BOOLEAN UseFullCacheFlush;

    CurrentIrql = KeGetCurrentIrql();

    NT_ASSERT((CurrentIrql <= DISPATCH_LEVEL) ||
              (CurrentIrql == HIGH_LEVEL));

    ExternalCachePresent = KiExternalCachePresent;
    IrqlRaised = FALSE;
    LoggingEnabled = PERFINFO_IS_GROUP_ON(PERF_CACHE_FLUSH);
    OldIrql = PASSIVE_LEVEL;
    UseFullCacheFlush = TRUE;
    if ((DmaOperation == FALSE) && (ReadOperation == FALSE)) {

        //
        // No cache management required for PIO write scenario
        //

        return;
    }

    //
    // If we can get a system address, flush the buffer by address.
    //

    BaseAddress = MmGetSystemAddressForMdlSafe(Mdl,
                                               HighPagePriority | MdlMappingNoExecute);
    if (BaseAddress != NULL) {
        UseFullCacheFlush = FALSE;
    }

    //
    // if the request is as large as the threshold (determined by the
    // largest cache size), promote to a full flush with the expectation
    // that this will be cheaper.
    //

    Length = MmGetMdlByteCount(Mdl);
    if (ReadOperation != FALSE) {
        if (Length > KiCacheFlushReadPromoteThreshold) {
            UseFullCacheFlush = TRUE;
            KiCacheFlushReadPromoteCount += 1;
        }
    } else {
        if (Length > KiCacheFlushWritePromoteThreshold) {
            UseFullCacheFlush = TRUE;
            KiCacheFlushWritePromoteCount += 1;
        }
    }

    if (KiCacheFlushForcePromote != FALSE) {
        UseFullCacheFlush = TRUE;
    }

    //
    // We can't use full cache flushes in the hibernate/crash path and we're
    // guaranteed to receive a mapped MDL.
    //

    if ((UseFullCacheFlush != FALSE) &&
        (CurrentIrql == HIGH_LEVEL)) {

        //
        // If we have a base address, then de-promote.  Otherwise, we
        // have no way to make forward progress safely.
        //

        if (BaseAddress != NULL) {
            UseFullCacheFlush = FALSE;
        } else {
            KeBugCheckEx(DATA_COHERENCY_EXCEPTION,
                         0x310,
                         0xBAD,
                         0,
                         0);
        }
    }

    if (LoggingEnabled != FALSE) {
        EtwGetKernelTraceTimestamp(&StartTimeStamp, PERF_CACHE_FLUSH);
    }

    if (UseFullCacheFlush != FALSE) {
        if (DmaOperation == TRUE) {
            if (ReadOperation != FALSE) {

                //
                // Flush and invalidate the entire cache hierarchy.
                //

                KeInvalidateAllCaches();

            } else {

                //
                // Clean the entire cache hierarchy.
                //

                KiCleanAllCaches();
            }
        } else {

            NT_ASSERT(ReadOperation != FALSE);

            //
            // Driver is reading information from the device to system memory
            //

        }
    } else {
        if (DmaOperation == TRUE) {

            QuiesceFlush = ((KiExternalCachePresent != FALSE) &&
                            (Length >= KiExternalCacheQuiesceThreshold) &&
                            (CurrentIrql <= DISPATCH_LEVEL));

            if (QuiesceFlush != FALSE) {
                if (CurrentIrql < DISPATCH_LEVEL) {
                    OldIrql = KeRaiseIrqlToDpcLevel();
                    IrqlRaised = TRUE;
                }

                //
                // Determine the mask of processors which are not in
                // deep-idle.
                //

                PoCopyDeepIdleMask(&DeepIdleMask);
                KeComplementAffinityEx(&TargetMask, &DeepIdleMask);
                KeAndAffinityEx(&TargetMask,
                                (PKAFFINITY_EX)&KeActiveProcessors,
                                &TargetMask);
                KeRemoveProcessorAffinityEx(&TargetMask,
                                            KeGetCurrentProcessorIndex());

                //
                // Send high importance DPC to each non-idle core
                // asking it to quiesce for the duration of this
                // flush.
                //

                InterlockedIncrementNoFence(&KiExternalCacheFlushOutstanding);
                FOR_EACH_PROCESSOR(&EnumerationContext, &TargetMask, &Index) {
                    KeInsertQueueDpc(&KiExternalCacheQuiesceDpc[Index],
                                     NULL,
                                     NULL);
                }
            }

            if (ReadOperation != FALSE) {

                LargestCacheLine = KeLargestCacheLine;

                //
                // Because there is no bus-snooping on ARM we cannot dirty any
                // cache-line containing memory that the device will be writing.
                // This means that we must enforce strict cache alignment on the
                // memory being used for DMA in order to keep from sharing a
                // cache line with some other data that could potentially
                // cause old values to be flushed back to the memory designated
                // for DMA.
                //

                NT_ASSERT(((ULONG) BaseAddress & (LargestCacheLine - 1)) == 0);
                if ((ULONG) BaseAddress & (LargestCacheLine - 1)) {
                    KeBugCheckEx(DRIVER_VERIFIER_DMA_VIOLATION,
                                 0x1F,
                                 0x04,
                                 (ULONG_PTR)BaseAddress,
                                 (ULONG_PTR)Mdl);
                }

                NT_ASSERT((Length & (LargestCacheLine - 1)) == 0);
                if (Length & (LargestCacheLine - 1)) {
                    KeBugCheckEx(DRIVER_VERIFIER_DMA_VIOLATION,
                                 0x1F,
                                 0x05,
                                 (ULONG_PTR)Length,
                                 (ULONG_PTR)Mdl);
                }

                //
                // Driver is reading information from the device to
                // system memory.  This code ensures that attempts to
                // read the system memory return the new contents
                // rather than the old.
                //
                // N.B. The external cache must be invalidated first
                //      to avoid a race where the architectural cache
                //      speculatively fetches and retains stale data
                //      from the external cache.
                //

                KiVoidRangeDCaches(BaseAddress, Length);
                if (ExternalCachePresent != FALSE) {
                    HalFlushIoBuffersExternalCache(Mdl, TRUE);
                    KiVoidRangeDCaches(BaseAddress, Length);
                }
            } else {

                //
                // Driver is writing information from the system
                // memory to the device.  THis code needs to ensure
                // that the operation reflects any writes to the
                // system memory that are still in the cache.  This is
                // accomplished by cleaning the data cache to the
                // point of coherency.
                //
                // N.B. This wouldn't be necessary if the memory wasn't
                //      configured as write-back
                //

                KiCleanRange(BaseAddress, Length);
                if (ExternalCachePresent != FALSE) {
                    HalFlushIoBuffersExternalCache(Mdl, FALSE);
                }
            }

            if (QuiesceFlush != FALSE) {
                InterlockedDecrementNoFence(&KiExternalCacheFlushOutstanding);
                if (IrqlRaised != FALSE) {
                    KeLowerIrql(OldIrql);
                }
            }
        } else {

            NT_ASSERT(ReadOperation != FALSE);

            //
            // Driver is reading information from the device to system memory
            //
        }
    }

    if (LoggingEnabled != FALSE) {
        EtwTraceCpuCacheFlush(&StartTimeStamp,
                              BaseAddress,
                              MmGetMdlByteCount(Mdl),
                              (ReadOperation == FALSE),
                              UseFullCacheFlush,
                              FALSE);
    }

    return;
}

VOID
KiFlushRectangle (
    __in PCHAR Current,
    __in PCHAR End,
    __in ULONG Width,
    __in ULONG Stride,
    __in BOOLEAN Invalidate
    )

/*++

Routine Description:

   This is an internal helper routine to flush/invalidate a
   rectangular in the architectural caches.

   In case of non-architectural caches the routine uses the PAs of the buffers
   in the Mdl. Since page faults cannot be handled at DISPATCH_LEVEL,
   this routine needs to be called at PASSIVE_LEVEL IRQL.

Arguments:

   Current - Supplies a pointer to the start of the rectangle.

   End - Supplies a pointer to the end of the rectangle.

   Width - Supplies the width of the rectangle in bytes.

   Stride - Supplies a stride in bytes indicating the distance between
       consecutive rows

   Invalidate - Supplies a boolean value that determines whether the cache lines
       in the rectangle shoudl be invalidated.

Return Value:

    None.

--*/

{

    if (Invalidate != FALSE) {
        while (Current < End) {
            KiVoidDcacheRangePoC(Current, Current + Width);
            Current += Stride;
        }
    } else {
        while (Current < End) {
            KiCleanDcacheRangePoC(Current, Current + Width);
            Current += Stride;
        }
    }

    _DataSynchronizationBarrier();
    return;
}

VOID
KeFlushIoRectangle (
    __in PMDL Mdl,
    __in PVOID StartVa,
    __in ULONG Width,
    __in ULONG Height,
    __in ULONG Stride,
    __in BOOLEAN ReadOperation
    )

/*++

Routine Description:

   This routine is intended to implement memory coherency for
   rectangular buffers. Based on the direction of operation either a
   flush or a void is performed. The caller should ensure that the
   parameters Start, Width and Stride are cache aligned in case of
   invalidation.

   An MDL is required so as to ensure that a fixed VA->PA relationship is
   maintained during this operation.

   N.B. This API can easily corrupt memory if given incorrect input.
        This risk is countered as it is in KeFlushIoBuffers by
        handling errors via bugCheck.

Arguments:

   Mdl - Supplies a pointer to the MDL describing the memory the rectangle
       resides in.

   StartVa - Supplies the virtual address of the start of the rectangle.
       The address must be within the range described by the Mdl.

   Width - Supplies the width of the rectangle in bytes.

   Height - Supplies a count of the number of rows in the rectangle.

   Stride - Supplies a stride in bytes indicating the distance between
       the start of consecutive rows.

   ReadOperation - Supplies a boolean value that determines whether the I/O
       operation is a read into memory.

Return Value:

    None.

--*/

{

    PCHAR EndSystemVa;
    ULONG LargestCacheLine;
    ULONG Length;
    BOOLEAN LoggingEnabled;
    PCHAR MdlEnd;
    PCHAR MdlStart;
    ULONG RectangleSpan;
    ULONG_PTR StartOffset;
    PCHAR StartSystemVa;
    ETW_KERNEL_TRACE_TIMESTAMP StartTimeStamp;
    PCHAR SystemAddress;
    BOOLEAN UseFullCacheFlush;
    ULONG_PTR VaOffset;

    NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    UseFullCacheFlush = TRUE;
    LargestCacheLine = KeLargestCacheLine;
    LoggingEnabled = PERFINFO_IS_GROUP_ON(PERF_CACHE_FLUSH);
    if ((Height == 0) || (Width == 0)) {
        return;
    }

    if (Mdl == NULL) {
        KeBugCheckEx(DATA_COHERENCY_EXCEPTION,
                     0x1003,
                     0xBAD,
                     1,
                     0);
    }

    //
    // Strides smaller than the width or not a multiple of the largest cache
    // line size are not supported.
    //

    if ((Stride < Width) || (Stride & (LargestCacheLine - 1))) {
        KeBugCheckEx(DATA_COHERENCY_EXCEPTION,
                     0x1004,
                     Height,
                     Width,
                     Stride);
    }

    //
    // Invalidating rectangles requires strict alignment to avoid data
    // corruption.
    //

    if (ReadOperation != FALSE) {
        if (((ULONG_PTR)StartVa & (LargestCacheLine - 1)) ||
            ((ULONG_PTR)Width & (LargestCacheLine - 1)) ||
            ((ULONG_PTR)Stride & (LargestCacheLine - 1))) {

            KeBugCheckEx(DATA_COHERENCY_EXCEPTION,
                         0x1002,
                         (ULONG_PTR)StartVa,
                         (ULONG_PTR)Width,
                         (ULONG_PTR)Stride);
        }
    }

    //
    // Validate the rectangle span against MDL virtual address space
    // range.
    //

    MdlStart = MmGetMdlVirtualAddress(Mdl);
    MdlEnd = MdlStart + MmGetMdlByteCount(Mdl);
    RectangleSpan = ((Height-1) * Stride) + Width;
    if (((ULONG_PTR)StartVa < ((ULONG_PTR)MdlStart)) ||
        (((ULONG_PTR)StartVa + RectangleSpan) > (ULONG_PTR)MdlEnd)) {

        KeBugCheckEx(DATA_COHERENCY_EXCEPTION,
                     0x1000,
                     (ULONG_PTR)StartVa,
                     (ULONG_PTR)MdlStart,
                     (ULONG_PTR)MdlEnd);
    }

    //
    // If we can get a system address, flush the buffer by address.
    //

    SystemAddress = MmGetSystemAddressForMdlSafe(Mdl,
                                                 HighPagePriority |
                                                 MdlMappingNoExecute);

    if (SystemAddress != NULL) {
        UseFullCacheFlush = FALSE;
    }

    //
    // If the request is as large as the threshold (determined by the
    // largest cache size), promote to a full flush with the expectation
    // that this will be cheaper.
    //

    Length = Height * Width;
    if (ReadOperation != FALSE) {
        if (Length > KiCacheFlushReadPromoteThreshold) {
            UseFullCacheFlush = TRUE;
            KiCacheFlushReadPromoteCount += 1;
        }
    } else {
        if (Length > KiCacheFlushWritePromoteThreshold) {
            UseFullCacheFlush = TRUE;
            KiCacheFlushWritePromoteCount += 1;
        }
    }

    if (KiCacheFlushForcePromote != FALSE) {
        UseFullCacheFlush = TRUE;
    }

    if (LoggingEnabled != FALSE) {
        EtwGetKernelTraceTimestamp(&StartTimeStamp, PERF_CACHE_FLUSH);
    }

    if (UseFullCacheFlush != FALSE) {
        if (ReadOperation != FALSE) {

            //
            // Clean and invalidate the entire cache hierarchy.
            //

            KeInvalidateAllCaches();

        } else {

            //
            // Clean the entire cache hierarchy.
            //

            KiCleanAllCaches();
        }
    } else {

        //
        // Determine the virtual address range of the rectangle in the
        // system address space
        //

        VaOffset = (ULONG_PTR)StartVa - (ULONG_PTR)MmGetMdlVirtualAddress(Mdl);
        StartSystemVa = SystemAddress + VaOffset;
        EndSystemVa = StartSystemVa + RectangleSpan;

        //
        // Determine the physical offset of the rectangle from the
        // start of the MDL physical range.  This is later used to
        // determine which PFN entry and offset within that page to
        // begin the cache operation
        //

        StartOffset = (ULONG_PTR)StartVa - (ULONG_PTR)MmGetMdlBaseVa(Mdl);

        //
        // Perform cache operations via system address space and
        // external cache operations if necessary.
        //

        if (ReadOperation != FALSE) {

            //
            // N.B. The external cache must be invalidated first to avoid
            //      a race where the architectural cache speculatively fetches
            //      and retains stale data from the external cache.
            //

            KiFlushRectangle(StartSystemVa, EndSystemVa, Width, Stride, TRUE);
            if (KiExternalCachePresent != FALSE) {
                HalFlushIoRectangleExternalCache(Mdl,
                                                 StartOffset,
                                                 Width,
                                                 Height,
                                                 Stride,
                                                 TRUE);
                KiFlushRectangle(StartSystemVa, EndSystemVa, Width, Stride, TRUE);
            }

        } else {
            KiFlushRectangle(StartSystemVa, EndSystemVa, Width, Stride, FALSE);
            if (KiExternalCachePresent != FALSE) {
                HalFlushIoRectangleExternalCache(Mdl,
                                                 StartOffset,
                                                 Width,
                                                 Height,
                                                 Stride,
                                                 FALSE);
            }
        }
    }

    if (LoggingEnabled != FALSE) {
        EtwTraceCpuCacheFlush(&StartTimeStamp,
                              SystemAddress,
                              Height * Width,
                              (ReadOperation == FALSE),
                              UseFullCacheFlush,
                              TRUE);
    }

    return;
}

VOID
KiExternalCacheAddDescriptor (
    _Inout_ PKPRCB Prcb
    )

/*++

Routine Description:

   This function flushes the I/O buffer specified by the memory descriptor
   list from the data cache on the processor which executes.

Arugements:

   Prcb - Supplies a pointer to a PRCB to which the external cache descriptor
       will be added.
Return Value:

   None.

--*/

{

    if (KiExternalCachePresent != FALSE) {
        if (sizeof(Prcb->Cache)/sizeof(Prcb->Cache[0]) > Prcb->CacheCount) {
            Prcb->Cache[Prcb->CacheCount] = KiExternalCacheDescriptor;
            Prcb->CacheCount += 1;
        }

        //
        // Refresh computed cache properties.
        //

        KiSetCacheInformation();
    }

    return;
}

VOID
KiCacheInitialize (
    VOID
    )

/*++

Routine Description:

    This function decodes processor cache information on ARMv7.

    The information is used to choose cache flush routines as well as
    set cache information in the PCR and PRCB.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PCACHE_DESCRIPTOR Cache;
    ARM_CACHE_INFO CacheInfo;
    ARM_CACHE_INFO_SELECTOR CacheSelector;
    ARM_CACHE_TYPE CacheType;
    ULONG Index;
    ULONG LargestCacheLine;
    ULONG LargestCacheSize;
    BOOLEAN LastLevel;
    ULONG Level;
    ARM_CACHE_LEVEL_INFO LevelInfo;
    PKPRCB Prcb;
    ULONG SmallestCacheSize;

    Prcb = KeGetCurrentPrcb();

    //
    // Retrieve per processor cache information.  This information is
    // gathered to enable applications to make cache usage
    // optimizations.
    //

    Prcb->CacheCount = 0;
    Cache = &Prcb->Cache[0];
    Level = 1;
    Index = 0;
    LastLevel = FALSE;
    LargestCacheLine = 0;
    LargestCacheSize = 0;
    SmallestCacheSize = ULONG_MAX;

    //
    // Use ARMv7 mechanism to enumerate architectural caches.
    //

    CacheType.Ulong = _MoveFromCoprocessor(CP15_CTR);
    LevelInfo.Ulong = _MoveFromCoprocessor(CP15_CLIDR);
    for (;;) {

        CacheSelector.Ulong = 0;
        CacheSelector.Level = Level - 1;

        //
        // Determine cache level type.  If the cache type is Harvard
        // i.e. I and D caches are present then two iterations through
        // the loop will be required to describe this level.
        //

        switch (LevelInfo.Level1) {
        case ARM_CACHE_HARVARD:
        case ARM_CACHE_INSTRUCTION:
            Cache->Type = CacheInstruction;
            CacheSelector.Select = ARM_CACHE_SELECT_INSTRUCTION;
            break;

        case ARM_CACHE_DATA:
            Cache->Type = CacheData;
            CacheSelector.Select = ARM_CACHE_SELECT_DATA_OR_UNIFIED;
            break;

        case ARM_CACHE_UNIFIED:
            Cache->Type = CacheUnified;
            CacheSelector.Select = ARM_CACHE_SELECT_DATA_OR_UNIFIED;
            break;

        case ARM_CACHE_NONE:
        default:
            LastLevel = TRUE;
        }

        if (LastLevel != FALSE) {
            break;
        }

        //
        // Select cache data.  The pipeline must be flushed to
        // retrieve consistent data.
        //

        _MoveToCoprocessor(CacheSelector.Ulong, CP15_CSSELR);
        _InstructionSynchronizationBarrier();

        //
        // Get cache information for the selected cache level
        //

        CacheInfo.Ulong = _MoveFromCoprocessor(CP15_CCSIDR);
        Cache->Level = (UCHAR) Level;
        Cache->Associativity = (UCHAR) CacheInfo.Associativity + 1;
        Cache->LineSize = (USHORT) (1 << (CacheInfo.LineSize + 4));
        Cache->Size  = Cache->Associativity * Cache->LineSize *
            (CacheInfo.NumberOfSets + 1);

        Prcb->CacheCount += 1;
        if (LevelInfo.Level1 != ARM_CACHE_HARVARD) {
            LevelInfo.Ulong >>= 3;
            Level += 1;

        } else {
            LevelInfo.Level1 = ARM_CACHE_DATA;
        }

        //
        // Track largest cache line and cache level size.
        //

        LargestCacheLine = max(LargestCacheLine, Cache->LineSize);
        LargestCacheSize = max(LargestCacheSize, Cache->Size);
        SmallestCacheSize = min(SmallestCacheSize, Cache->Size);

        //
        // Move on to next level.
        //

        Index += 1;
        Cache += 1;
        if (Index == RTL_NUMBER_OF(Prcb->Cache)) {
            break;
        }

        //
        // Test if there are additional levels to process.
        //

        if (Level > 8) {
            break;
        }
    }

    if (Prcb->Number == 0) {

        //
        // This code runs on ARMv7 platforms with PIPT data caches and
        // PIPT or VIPT instruction caches.  Harvard style caches are
        // assumed.
        //

        if ((CacheType.IPolicy != ARM_L1_ICACHE_PIPT) &&
            (CacheType.IPolicy != ARM_L1_ICACHE_VIPT)) {

            KeBugCheckEx(UNSUPPORTED_PROCESSOR, 8, CacheType.Ulong, 0, 0);
        }

        //
        // Record the cache type of the first processor
        //

        KiCacheType.Ulong = CacheType.Ulong;

        //
        // Determine minimum and maximum cache line sizes, cache flush
        // threshold, and largest cache line.
        //

        KiMinimumIcacheLine = 1 << (CacheType.IMinLine + 2);
        KiMinimumDcacheLine = 1 << (CacheType.DMinLine + 2);
        if (LargestCacheLine != 0) {
            KeLargestCacheLine = LargestCacheLine;
        }

        //
        // Determine ICache flush promote threshold.  This threshold
        // logic reflects measured data and modeling.
        //

        SmallestCacheSize = max(SmallestCacheSize, 16*1024);
        KiICacheFlushPromoteThreshold = SmallestCacheSize / 2;

    } else {

        //
        // If the instruction cache policies differ, then ensure the overall
        // system policy is VIPT which uses full flushes and is therefore safe
        // to use even on processors with PIPT instruction caches.
        //

        if (KiCacheType.IPolicy != CacheType.IPolicy) {
            KiCacheType.IPolicy = ARM_L1_ICACHE_VIPT;
            CacheType.IPolicy = ARM_L1_ICACHE_VIPT;
        }

        if (KiCacheType.Ulong != CacheType.Ulong) {

            //
            // All processors must have the same cache instruction and data
            // cache minimum line sizes.
            //

            KeBugCheckEx(MULTIPROCESSOR_CONFIGURATION_NOT_SUPPORTED,
                         KiCacheType.Ulong,
                         CacheType.Ulong,
                         0,
                         0);

        }

        //
        // Add external cache descriptor
        //

        KiExternalCacheAddDescriptor(Prcb);
    }

    return;
}

VOID
KiCacheFlushTrial (
    _In_ PMDL Mdl,
    PVOID Buffer,
    _In_ BOOLEAN ReadOperation,
    _In_ ULONG DirtySize,
    _In_ ULONG RefillSize,
    _Out_ PULONGLONG Cycles
    )

/*++

Routine Description:

   This function does a trial invocation of KeFlushIoBuffers() in the
   specified direction and captures its cost in processor.  The trial
   dirties the buffer before the flush and refills it after void operations.

Arguments:

   Mdl - Supplies a pointer to a memory descriptor list that describes the
       buffer.

   Buffer - Supplies a pointer to a buffer to be used for the trial.  The same
       buffer is described by the MDL.

   ReadOperation - Supplies a boolean value that determines whether the I/O
       operation is a read into memory.

   DirtySize - Supplies a ULONG indicating how much of the bufffer
       this routine should dirty

   RefillSize - Supplies a ULONG indicating how much of the bufffer
       this routine should refill

   Cycles - Supplies a pointer to a ULONGLONG in which the processor cycles
       consumed by the flush operation are stored.

Return Value:

   None.

--*/

{

    ULONGLONG After;
    ULONGLONG Before;
    PUCHAR Current;
    PUCHAR End;
    ULONG Stride;

    Stride = KeLargestCacheLine;
    Current = Buffer;
    End = Current + RefillSize;

    //
    // Clean the buffer to avoid contaminating results due to already
    // dirty cache lines.
    //

    KeFlushIoBuffers(Mdl, FALSE, TRUE);

    //
    // Dirty the buffer to make the measurement more realistic.
    //

    RtlFillMemory(Current, DirtySize, 0x01);
    KeMemoryBarrier();

    //
    // Measure the flush cost in cycles.  For read operations, capture
    // the cost to refill a portion of the buffer as well.
    //

    Before = __rdpmccntr64();
    KeFlushIoBuffers(Mdl, ReadOperation, TRUE);
    if (ReadOperation != FALSE) {

        //
        // Capture the cost of cache refill by forcing the read of
        // each cache line.
        //

        while (Current < End) {
            ReadUCharNoFence(Current);
            Current += Stride;
        }
    }

    After = __rdpmccntr64();
    *Cycles = After - Before;

    return;
}

VOID
KiCacheInterpolateFlushThreshold (
    _In_ PMDL SourceMdl,
    _Inout_ PMDL PartialMdl,
    _In_ PVOID Buffer,
    _In_ ULONG BufferSize,
    _In_ BOOLEAN ReadOperation,
    _Out_ PULONG Threshold
    )

/*++

Routine Description:

   This function does binary search to determine the threshold at
   which a promoted invocation of KeFlushIoBuffers is more efficient
   than 'flush by VA'.

Arguments:

   SourceMdl - Supplies a pointer to a memory descriptor list describing the
       buffer passed in.

   PartialMdl - Supplies a pointer to a memory descriptor list large enough to
       describe the buffer.

   Buffer - Suppleis a pointer to a buffer of BufferSize

   BufferSize - Supplies a ULONG indicating the size of the buffer described
       by the SourceMDL and Buffer.

   ReadOperation - Supplies a boolean value that determines whether
       the I/O operation to be thresholded is a read into memory.

   Threshold - Supplies a pointer to a ULONG in which the computed threshold
       after which flush promotion is more efficient is stored.

Return Value:

   None.

--*/

{

    ULONGLONG Cycles;
    ULONG HighSize;
    ULONG Index;
    ULONG LastPromoteDirtySize;
    ULONG LowSize;
    ULONG MinimumPromoteSize;
    ULONG PromoteDirtySize;
    ULONGLONG PromoteCycles;
    ULONG SecondLevelCacheSize;
    ULONG TrialSize;

    SecondLevelCacheSize = KeGetPcr()->SecondLevelCacheSize;

    NT_ASSERT(SecondLevelCacheSize < BufferSize);

    //
    // Bound the interpolation by the size of the L1 cache and the
    // buffer size.  The starting point is the middle of the buffer.
    //

    LowSize = KeGetCurrentPrcb()->Cache[0].Size;
    HighSize = BufferSize;
    LastPromoteDirtySize = 0;
    MinimumPromoteSize = ULONG_MAX;
    TrialSize = BufferSize / 2;
    PromoteCycles = 0;

    for (Index = 0; Index < 5; Index += 1)  {

        //
        // Create a partial MDL describing only the trial portion of
        // buffer.
        //

        IoBuildPartialMdl(SourceMdl, PartialMdl, Buffer, TrialSize);

        //
        // Flush without promotion.
        //

        NT_ASSERT(KiCacheFlushForcePromote == FALSE);

        KiCacheFlushTrial(PartialMdl,
                          Buffer,
                          ReadOperation,
                          min(SecondLevelCacheSize, TrialSize),
                          min(SecondLevelCacheSize, TrialSize),
                          &Cycles);

        //
        // Flush with promotion.
        //

        PromoteDirtySize = max(SecondLevelCacheSize * 3 / 4, TrialSize);

        //
        // Avoid re-running the promotion trial iff the dirty size is
        // the same as the previous round.
        //

        if (PromoteDirtySize != LastPromoteDirtySize) {
            KiCacheFlushForcePromote = TRUE;
            LastPromoteDirtySize = PromoteDirtySize;
            KiCacheFlushTrial(PartialMdl,
                              Buffer,
                              ReadOperation,
                              PromoteDirtySize,
                              SecondLevelCacheSize,
                              &PromoteCycles);

            KiCacheFlushForcePromote = FALSE;
        }

        //
        // Binary search/interpolate based on cost to 'flush by VA'
        // vs. a promoted flush.
        //

        if (Cycles < PromoteCycles) {

            //
            // Since 'by VA' was faster than 'promotion' at this trial
            // size, look to the high side for promotion opportunity.
            //

            LowSize = TrialSize;

        } else {

            //
            // Since 'by VA' was slower than 'promotion' at this trial
            // size, look to the low side to see if promotion should
            // occur at a lower threshold.
            //

            MinimumPromoteSize = min(MinimumPromoteSize, TrialSize);
            HighSize = TrialSize;
        }

        TrialSize = (LowSize + HighSize) / 2;
    }

    *Threshold = MinimumPromoteSize;
    return;
}

VOID
KiCacheAdjustFlushPromoteThresholds (
    VOID
    )

/*++

Routine Description:

    This function gathers cache flush promote threshold information
    for read and write applications of KeFlushIoBuffers() using a
    temporary buffer and MDLs.

Arguments:

    None.

Return Value:

    None.

--*/

{

    ULONG BufferSize;
    ARM_IDCODE Id;
    PMDL Mdl;
    SIZE_T MdlSize;
    PMDL PartialMdl;
    ULONG SecondLevelCacheSize;
    PVOID TestBuffer;

    NT_ASSERT(KiCacheFlushForcePromote == FALSE);

    Mdl = NULL;
    PartialMdl = NULL;
    SecondLevelCacheSize = KeGetPcr()->SecondLevelCacheSize;
    TestBuffer = NULL;

    //
    // Limit promotion to A9 platforms due to worries about cache line
    // migration allowing a line to avoid management.  Further, other cores show
    // minimal benefit.
    //

    Id.Ulong = _MoveFromCoprocessor (CP15_MIDR);
    if (((Id.Model == 0xC09) && (Id.Implementer == 'A')) == FALSE) {
        goto out;
    }

    //
    // If no L2 cache is present, avoid recomputing the thresholds.
    //

    if (SecondLevelCacheSize == 0) {
        goto out;
    }

    //
    // Allocate a scratch buffer to use for the measurements.
    //

    BufferSize = SecondLevelCacheSize * 2;
    TestBuffer = ExAllocatePoolWithTag(NonPagedPoolNx,
                                       BufferSize,
                                       'ARKI');

    if (TestBuffer == NULL) {
        goto out;
    }

    MdlSize = MmSizeOfMdl(TestBuffer, BufferSize);

    //
    // Allocate an MDL to describe the buffer and initialize it to
    // reflected the allocated buffer. Allocate an identically sized
    // MDL to use for partial MDLs
    //

    Mdl = (PMDL) ExAllocatePoolWithTag(NonPagedPoolNx,
                                       MdlSize,
                                       'cdmM');

    if (Mdl == NULL) {
        goto out;
    }

    PartialMdl = (PMDL) ExAllocatePoolWithTag(NonPagedPoolNx,
                                              MdlSize,
                                              'clmM');

    if (PartialMdl == NULL) {
        goto out;
    }

    MmInitializeMdl(Mdl, TestBuffer, BufferSize);
    MmBuildMdlForNonPagedPool(Mdl);
    MmInitializeMdl(PartialMdl, TestBuffer, BufferSize);

    //
    // Interpolate flush thresholds
    //

    KiCacheInterpolateFlushThreshold(Mdl,
                                     PartialMdl,
                                     TestBuffer,
                                     BufferSize,
                                     TRUE,
                                     &KiCacheFlushReadPromoteThreshold);

    KiCacheInterpolateFlushThreshold(Mdl,
                                     PartialMdl,
                                     TestBuffer,
                                     BufferSize,
                                     FALSE,
                                     &KiCacheFlushWritePromoteThreshold);
out:

    if (PartialMdl != NULL) {
        ExFreePoolWithTag(PartialMdl, 'clmM');
    }

    if (Mdl != NULL) {
        ExFreePoolWithTag(Mdl, 'clmM');
    }

    if (TestBuffer != NULL) {
        ExFreePoolWithTag(TestBuffer, 'ARKI');
    }

    NT_ASSERT(KiCacheFlushForcePromote == FALSE);

    return;
}

VOID
KeExternalCacheInitialize (
    VOID
    )

/*++

Routine Description:

    This function intializes support for the extneral cache if present.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PKDPC Dpc;
    ULONG Index;
    HAL_EXTERNAL_CACHE_INFO Info;
    ULONG Length;
    PKPRCB Prcb;
    NTSTATUS Status;

    Dpc = NULL;
    Prcb = KeGetCurrentPrcb();
    Status = HalQuerySystemInformation(HalExternalCacheInformation,
                                       sizeof(HAL_EXTERNAL_CACHE_INFO),
                                       &Info,
                                       &Length);

    if (NT_SUCCESS(Status) != FALSE) {
        KiExternalCacheGlobalSyncRequired = Info.GlobalSyncRequired;
        KiExternalCacheDescriptor = Info.CacheDescriptor;

        //
        // Setup mechanism
        //

        for (Index = 0; Index < MAXIMUM_PROC_PER_SYSTEM; Index += 1) {
            Dpc = &KiExternalCacheQuiesceDpc[Index];
            KeInitializeDpc(Dpc, KiExternalCacheFlushQuiesce, NULL);
            KeSetTargetProcessorIndexDpc(Dpc, Index);
            if (KiExternalCacheQuiesceHighImportanceEnable != 0) {
                KeSetImportanceDpc(Dpc, HighImportance);
            } else {
                KeSetImportanceDpc(Dpc, MediumHighImportance);
            }
        }

        KiExternalCachePresent = TRUE;
        SharedUserData->ProcessorFeatures[PF_ARM_EXTERNAL_CACHE_AVAILABLE] = TRUE;

        //
        // Add external cache descriptor to P0.  The same descriptor will
        // be added to subsequently started processors
        //

        NT_ASSERT(Prcb->Number == 0);

        KiExternalCacheAddDescriptor(Prcb);
    }

    return;
}

VOID
KeReportCacheIncoherentDevice (
    VOID
    )

/*++

Routine Description:

    This function allows ACPI to report that a cache incoherent device has been
    enumerated by the system.

Arguments:

    None.

Return Value:

    None.

--*/

{

    //
    // ARM is always assumed to be not fully coherent.
    //

    return;
}

BOOLEAN
KeSystemFullyCacheCoherent (
    VOID
    )

/*++

Routine Description:

    This function allows the caller to check if the system is fully coherent;
    i.e., no cache incoherent device has been enumerated yet by the system.

Arguments:

    None.

Return Value:

    Returns TRUE if the system is fully cache coherent; FALSE otherwise.

--*/

{

    //
    // ARM is never fully coherent.
    //

    return FALSE;
}
