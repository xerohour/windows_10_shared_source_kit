/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    archapi.c

Abstract:

    This module implements top level architecture API for the Boot
    Environment Library.

    This implementation is ARM specific.

Environment:

    Boot.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include <bootlibp.h>
#include "context.h"

//
// ---------------------------------------------------------------- Definitions
//

#define ARM_CYCLE_COUNTER_SIGNATURE (0x56430000)

//
// -------------------------------------------------------------------- Globals
//


#if defined(BOOTENV_PERF_POLLING)

#define ARCHP_DEFAULT_VALIDATE_COUNT 10
ULONG ArchpInterfaceCount = 0;
ULONG ArchpPollingCount = ARCHP_DEFAULT_VALIDATE_COUNT;

#endif

ULONG ArchpCacheLineSize = 0;

//
// ------------------------------------------------------------------ Functions
//

VOID
BlpArchEnableInterrupts (
    VOID
    )

/*++

Routine Description:

    This routines enables interrupts on an ARM platform.

Arguments:

    None.

Return Value:

    None.

--*/

{
    _enable();
    return;
}

VOID
BlpArchDisableInterrupts (
    VOID
    )

/*++

Routine Description:

    This routines disables interrupts on x64 platform.

Arguments:

    None.

Return Value:

    None.

--*/

{
    _disable();
    return;
}

BOOLEAN
BlArchQueryIoPortAccessSupported (
    VOID
    )

/*++

Routine Description:

    This function determines whether IO port access is supported on the
    current platform.

Arguments:

    None.

Returns:

    A flag indicating whether IO port access is supported.

--*/

{
    return FALSE;
}

FORCEINLINE
VOID
BlpArchCleanCurrentDcacheRangeV7 (
    __in ULONG_PTR Current,
    __in ULONG_PTR End
    )

/*++

Routine Description:

    This function writes back the architecturally described caches to
    main memory across a range of virtual addresses.

Arguments:

    BaseAddress - Supplies a pointer to the base of the range that is flushed.

    Length - Supplies the length of the range that is flushed if the base
        address is specified.

Return Value:

    None.

--*/

{

    //
    // Iterate through the range at cache line intervals cleaning to
    // the Point of Coherency.
    //

    Current = (ULONG_PTR)Current & ~(ArchpCacheLineSize - 1);
    while (Current < End) {
        _MoveToCoprocessor(Current, CP15_DCCMVAC);
        Current += ArchpCacheLineSize;
    }

    return;
}

VOID
BlpArchSweepIcacheRangeV7 (
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

    ULONG_PTR Start;

    Start = (ULONG_PTR)BaseAddress;
    BlpArchCleanCurrentDcacheRangeV7(Start, Start + Length);
    _DataSynchronizationBarrier();

    //
    // On a VIPT ICache, the icache flushes do not have the effect of
    // flushing all of the aliases of the target icache line. Flush
    // the entire icache to be safe.
    //

    _MoveToCoprocessor(0, CP15_ICIALLU);
    _InvalidateBTAC();
    _DataSynchronizationBarrier();
    _InstructionSynchronizationBarrier();
    return;
}

VOID
BlArchSweepIcacheRange (
    __in PVOID BaseAddress,
    __in SIZE_T Length
    )

/*++

Routine Description:

    This routine carries out the actions necessary to make the
    instruction cache coherent with the data cache across the given
    address range.  The instruction cache is not coherent with the
    data cache on ARM.  This routine needs ensure that future
    instruction fetches in this address range will observe the latest
    values stored into the same range through the data cache.

Arguments:

    BaseAddress - Supplies the address of the first byte within the range that
        should be made coherent.

    Length - Supplies the length in bytes of the range that should be made
        coherent.

Return Value:

    None.

--*/

{

    NT_ASSERT(READ_ARM_FEATURE (CP15_MMFR3, MMFR3_L1HIER_CACHE));

    BlpArchSweepIcacheRangeV7(BaseAddress, Length);
    return;
}

VOID
BlArchFlushRectangle (
    __in PVOID Origin,
    __in ULONG Width,
    __in ULONG Height,
    __in ULONG Stride
    )

/*++

Routine Description:

    This routine flushes writes to the memory rectangle.

Arguments:

    Origin - The starting VA of the rectangle.

    Width - The byte width of the rectangle.

    Heigth - The number of rectangle lines.

    Stride - The byte offset between the start of the rectangle lines.

Return Value:

    None.

--*/

{

    ULONG_PTR LineStart;

    for (LineStart = (ULONG_PTR)Origin;
         Height > 0;
         LineStart += Stride, Height -= 1) {

        BlpArchCleanCurrentDcacheRangeV7(LineStart, LineStart + Width);
    }

    return;
}

ULONGLONG
BlArchGetPerformanceCounter (
    VOID
    )

/*++

Routine Description:

    This routines returns a value of a monotomically incrementing
    counter.  The ARM cycle counter is used to implement this functionality.

    N.B. The performance monitor implementation calls this routine.  Therefore,
         it can not call BlpInterfaceEnter/BlpInterfaceExit.

Arguments:

    None.

Returns:

    The current value of the performance counter.

--*/

{

    ULONG CurrentCount;
    ULARGE_INTEGER PerformanceCounter;

    //
    // Bail out if the performance counter has not yet been initialized.
    //

    _InstructionSynchronizationBarrier();
    PerformanceCounter.HighPart = _MoveFromCoprocessor(CP15_TPIDRURW);
    if ((PerformanceCounter.HighPart & 0xFFFF0000) !=
        ARM_CYCLE_COUNTER_SIGNATURE) {

        return 0;
    }

    //
    // Mask off the signature.
    //

    PerformanceCounter.HighPart &= 0x0000FFFF;

    //
    // The ARM cycle counter is only 32 bits and will roll over fairly
    // often.  To reduce the impact of this rollover, the code
    // remembers the last returned count, and will increment the upper
    // 32-bit value as needed to maintain an extra 16 bits so the resulting
    // performance counter is reliable across the entire pre-OS execution.
    //

    CurrentCount = _MoveFromCoprocessor(CP15_PMCCNTR);
    PerformanceCounter.LowPart = _MoveFromCoprocessor(CP15_TPIDRURO);
    _InstructionSynchronizationBarrier();

    //
    // Detect rollover.  If detected, increment the high part and write the
    // new value back to the co-processor.
    //

    if (CurrentCount < PerformanceCounter.LowPart) {
        PerformanceCounter.HighPart += 1;
        _MoveToCoprocessor((ARM_CYCLE_COUNTER_SIGNATURE |
                            PerformanceCounter.HighPart),
                           CP15_TPIDRURW);
    }

    PerformanceCounter.LowPart = CurrentCount;

    //
    // Always write the last raw read of the cycle counter back to the
    // co-processor.
    //

    _MoveToCoprocessor(PerformanceCounter.LowPart, CP15_TPIDRURO);

    //
    // Scale up the values by a factor of 64 so the performance counter tracks
    // cycles.  Ultimately, the OS will run the cycle counter at full speed and
    // this keeps the units identical to ease boot performance analysis.
    //

    PerformanceCounter.QuadPart *= 64;
    return PerformanceCounter.QuadPart;
}

#if defined(BOOTENV_PERF_POLLING)

VOID
BlpArchInterfaceEnter (
    __in INTERFACE_NAME Name
    )

/*++

Routine Description:

    Performs periodic activities if needed to maintain sanity of the
    architectural performance counter.

    This routine is called on the entrance to a Boot Environment Library API or
    API class.

Arguments:

    Name - Supplies the API or API class which is starting.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Name);

    ArchpInterfaceCount += 1;
    if (ArchpInterfaceCount >= ArchpPollingCount) {
        BlArchGetPerformanceCounter();
        ArchpPollingCount = 0;
    }

    return;
}

#endif

VOID
ArchEnablePerformanceCounter (
    VOID
    )

/*++

Routine Description:

    Initializes the ARM Cycle Counter and start it running.

Arguments:

    None.

Returns:

    None.

--*/

{

    ULONG Value;

    //
    // Set the following bits to enable performance counters in general and
    // configure the cycle counter:
    //    Enable - Turns all performance counters on.
    //    Reset - Resets the cycle counter to 0, but only in the event where the
    //            counter is not already enabled and set to divide by 64.  This
    //            code assumes that the firmware starts the cycle counter at the
    //            reduced rate as early as possible for boot performance
    //            tracing.
    //    Divide - Increment every 64 cycles instead of every cycle. 32 bits is
    //        a bit fast for every cycle.
    //    Export - Export this counter to external sources.
    //

    Value = _MoveFromCoprocessor(CP15_PMCR);
    Value |= CP15_PMCR_X;
    if ((Value & (CP15_PMCR_CLKCNT_DIV | CP15_PMCR_ENABLE)) !=
        (CP15_PMCR_CLKCNT_DIV | CP15_PMCR_ENABLE)) {

        Value |= (CP15_PMCR_CLKCNT_DIV |
                  CP15_PMCR_CLKCNT_RST |
                  CP15_PMCR_ENABLE);
    }

    //
    // Clear the bit that disables the cycle counter in prohibited regions. If
    // the machine is taking heavy SMI-type activity, this is the only chance
    // to detect it.
    //

    Value &= ~CP15_PMCR_DP;
    _MoveToCoprocessor(Value, CP15_PMCR);
    _InstructionSynchronizationBarrier();

    //
    // The user RW co-processor register (used to store the TEB once the OS is
    // up and running) is utilized in the boot environment to store the upper
    // 32bits of the cycle counter.  This allows the construction of a 48bit
    // cycle counter that can persist across boot applications into early OS
    // initialization without the need for storing or tracking it's location in
    // memory.
    //
    // The user RO co-processor register (used to store the current thread once
    // the OS is up and running) is utilized to store the last 32bit value read
    // from the cycle counter which enables overflow detection without the need
    // for storing or tracking it's location in memory.
    //
    // In order to determine whether these registers have been previously
    // initialized, use the top 16 bits of this RW register to store a
    // signature.  If the signature is found, this code assumes the bottom 16
    // bits of the RW register properly represent bits 32-47 of the virtual
    // cycle counter and that the RO register represents the last cycle counter
    // queried by this library.
    //
    //

    Value = _MoveFromCoprocessor(CP15_TPIDRURW);
    if ((Value & 0xFFFF0000) != ARM_CYCLE_COUNTER_SIGNATURE) {
        _MoveToCoprocessor(ARM_CYCLE_COUNTER_SIGNATURE, CP15_TPIDRURW);
        _MoveToCoprocessor(0, CP15_TPIDRURO);
    }

    //
    // Disable overflow interrupt
    //

    _MoveToCoprocessor(CP15_PMCNTEN_PMCCNT, CP15_PMINTENCLR);
    _InstructionSynchronizationBarrier();

    //
    // Enable the cycle counter.
    //

    _MoveToCoprocessor(CP15_PMCNTEN_PMCCNT, CP15_PMCNTENSET);
    _InstructionSynchronizationBarrier();

    //
    // Disable user access to the performance counter registers
    //

    _MoveToCoprocessor(0, CP15_PMUSERENR);
    _InstructionSynchronizationBarrier();
    return;
}

VOID
ArchEnableFloatingPoint (
    VOID
    )

/*++

Routine Description:

    This routine enables floating point on the current processor.

Arguments:

    None.

Returns:

    None.

--*/

{

    ULONG Value;

    Value = _MoveFromCoprocessor(CP15_CPACR);

    //
    // Enable access to the floating point coprocessor
    //
    // Also set the CP10 and CP11 access bits to allow both user and kernel
    // free access.
    //

    Value &= ~(CP15_CPACR_D32DIS | CP15_CPACR_ASEDIS);
    Value |= CP15_CPACR_VFP_MASK;
    _MoveToCoprocessor(Value, CP15_CPACR);
    _InstructionSynchronizationBarrier();

    //
    // Enable all FP support
    //

    _MoveToCoprocessor(CPVFP_FPEXC_EN |
                       CPVFP_FPEXC_EX |
                       CPVFP_FPEXC_FP2V, CPVFP_FPEXC);
    _InstructionSynchronizationBarrier();

    return;
}

NTSTATUS
BlpArchInitialize (
    __in ULONG Phase
    )

/*++

Routine Description:

    Performs all ARM specific intitialization required for the Boot
    Environment Library.

    During phase 0, initialization must be done in the absence of library
    MM support with the goal of performing any necessary steps required
    for MM initializaton.

    Phase 1 occurs after MM has been initialized and can use memory services.

Arguments:

    Phase - Supplies phase 0 or phase 1 values.

Returns:

    STATUS_SUCCESS always.

--*/

{

    ARM_CACHE_TYPE CacheType;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Phase);

    BlpInterfaceEnter(InterfaceArch);

    if (Phase == 0) {

        //
        // Determine minimum cache line size.
        //

        CacheType.Ulong = _MoveFromCoprocessor(CP15_CTR);
        ArchpCacheLineSize = 1 << (CacheType.DMinLine + 2);

        //
        // Initialize execution context state for the ARM architecture.
        //

        Status = ArchInitializeContexts();
        if (!NT_SUCCESS(Status)) {
            BlpInterfaceExit(InterfaceArch);
            return Status;
        }

        //
        // Enable performance counter and floating point on the BSP.
        //

        ArchEnablePerformanceCounter();
        ArchEnableFloatingPoint();
    }

    BlpInterfaceExit(InterfaceArch);
    return STATUS_SUCCESS;
}

NTSTATUS
BlpArchDestroy (
    VOID
    )

/*++

Routine Description:

    Performs all ARM specific cleanup required for the Boot Environment
    Library.

Arguments:

    None.

Returns:

    STATUS_SUCCESS always.

--*/

{

    BlpInterfaceEnter(InterfaceArch);
    ArchDestroyContexts();
    BlpInterfaceExit(InterfaceArch);
    return STATUS_SUCCESS;
}

NTSTATUS
BlArchIsVideoBiosSupported (
    __out PBOOLEAN VideoBiosSupported
    )

/*++

Routine Description:

    This routine determines if this platform supports a legacy video bios.

Arguments:

    VideoBiosSupported - Supplies a pointer to a boolean that returns whether or
        not legacy video bios is supported on this system.

Return Value:

    STATUS_SUCCESS when successful.
    Other error codes possible depending on implementation.

--*/

{
    *VideoBiosSupported = FALSE;
    return STATUS_SUCCESS;
}

ULONGLONG
ReadTimeStampCounter (
    VOID
    )

/*++

Routine Description:

    Returns a value of a monotomically incrementing counter.  On ARM, the
    performance counter is used for this feature.

    N.B. The performance monitor implementation calls this routine.  Therefore,
         it can not call BlpInterfaceEnter/BlpInterfaceExit.

Arguments:

    None.

Returns:

    The current value of the performance counter.

--*/

{
    return BlArchGetPerformanceCounter();
}

VOID
RtlRaiseException (
    IN PEXCEPTION_RECORD ExceptionRecord
    )

/*++

Routine Description:

    Provides a stub version of this RTL API which is referenced by a CRT
    function.

Arguments:

    ExceptionRecord - Provides a pointer to the exception record.

Returns:

    None.

--*/

{
    UNREFERENCED_PARAMETER(ExceptionRecord);
    ASSERT(FALSE);
}


