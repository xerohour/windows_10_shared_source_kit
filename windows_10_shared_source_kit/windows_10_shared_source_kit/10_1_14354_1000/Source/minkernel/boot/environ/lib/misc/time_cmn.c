/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    time_cmn.c

Abstract:

    Implementation of firmware independent boot library time-related APIs.

Environment:

    Boot.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "bootlibp.h"

//
// -------------------------------------------------------------------- Globals
//

LARGE_INTEGER BlpTimePerformanceFrequency = { 0, 0 };

//
// ------------------------------------------------------------------ Functions
//

LARGE_INTEGER
BlTimeQueryPerformanceCounter (
    __out_opt PLARGE_INTEGER Frequency
    )

/*++

Routine Description:

    This routine implements a 64-bit increasing fixed-frequency performance
    counter.

Arguments:

    Frequency - Supplies an optional buffer to receive the performance
        frequency.

Return Value:

    Returns a 64-bit performance counter value.

--*/

{

    LARGE_INTEGER ReturnValue;

    if (ARGUMENT_PRESENT(Frequency) != FALSE) {
        Frequency->QuadPart = BlpTimePerformanceFrequency.QuadPart;
    }

    ReturnValue.QuadPart = ReadTimeStampCounter();
    return ReturnValue;
}

VOID
BlTimeStallExecution (
    ULONG Duration
    )

/*++

Routine Description:

    This routine will stall the processsor for specified number of
    microseconds.

Arguments:

    Duration - Supplies the number of microseconds to stall.

Return Value:

    None

--*/

{

    LARGE_INTEGER TickTarget;

    TickTarget.QuadPart = BlTimeQueryPerformanceCounter(NULL).QuadPart +
        ((BlpTimePerformanceFrequency.QuadPart / (1000 * 1000)) * Duration);

    do {
        NOTHING;
    } while (BlTimeQueryPerformanceCounter(NULL).QuadPart <
             TickTarget.QuadPart);

    return;
}

//
// ---------------------------------------------------------- Private Functions
//

NTSTATUS
BlpTimeDestroy (
    VOID
    )

/*++

Routine Description:

    This routine frees any resources used by the boot library time-related
    APIs.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

    return STATUS_SUCCESS;
}

NTSTATUS
BlpTimeInitialize (
    VOID
    )

/*++

Routine Description:

    This routine handles initialization of the boot library time-related
    services.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

    ULONGLONG Frequency;
    NTSTATUS Status;

    Frequency = 0;
    Status = BlGetApplicationOptionInteger(
                 BCDE_LIBRARY_TYPE_PERFORMANCE_FREQUENCY,
                 &Frequency);

    //
    // Fallback to normal performance counter calibration
    // when pre-calculated frequency is not provided.
    //

    if (!NT_SUCCESS(Status) || (Frequency == 0)) {
        Status = BlpTimeCalibratePerformanceCounter();

    } else {
        BlpTimePerformanceFrequency.QuadPart = Frequency;
    }

    return Status;
}

