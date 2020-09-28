/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    time.c

Abstract:

    Implementation of timer I/O routines for an EFI firmware boot
    environment.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "bootlibp.h"
#include <hvilib.h>

//
// ---------------------------------------------------------------- Definitions
//

#define SECONDS_PER_HOUR (60 * 60)
#define TSC_FREQUENCY_RETRIES 10

//
// -------------------------------------------------------------------- Globals
//

//
// The following data is used to track elapsed seconds in
// BlTimeGetRelativeTime.
//

UINT8 LastHour = 0;
ULONG BaseSeconds = 0;
ULONG BlpGetRelativeTimeErrorCount = 0;

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
BlGetTime (
    __out PTIME_FIELDS TimeFields,
    __out_opt PBOOLEAN UtcTime
    )

/*++

Routine Description:

    This routine returns the current system time.

Arguments:

    TimeFields - Supplies a buffer for the current time.

    UtcTime - Supplies a pointer that will receive TRUE if the returned time
        is in UTC format. It will be set to FALSE otherwise.

Return Value:

    STATUS_SUCCESS, or an appropriate error status.

--*/

{

    EFI_TIME EfiTime;
    LARGE_INTEGER FirmwareTime;
    NTSTATUS Status;
    BOOLEAN Success;
    LARGE_INTEGER TimeZoneBias;
    BOOLEAN TimeZoneBiasApplied;
    LONG TimeZone;

    TimeZoneBiasApplied = FALSE;
    RtlZeroMemory(&EfiTime, sizeof(EFI_TIME));
    Status = EfiGetTime(&EfiTime);
    if (!NT_SUCCESS(Status)) {
        goto GetTimeEnd;
    }

    TimeFields->Year = EfiTime.Year;
    TimeFields->Month = EfiTime.Month;
    TimeFields->Day = EfiTime.Day;
    TimeFields->Hour = EfiTime.Hour;
    TimeFields->Minute = EfiTime.Minute;
    TimeFields->Second = EfiTime.Second;
    TimeFields->Milliseconds = (CSHORT)(EfiTime.Nanosecond / 1000000);
    TimeFields->Weekday = 7; // Not supported (set out of range).

    //
    // Validate that time time returned is reasonable. Assert otherwise.
    //

    Success = RtlTimeFieldsToTime(TimeFields, &FirmwareTime);
    if (Success == FALSE) {

        ASSERT(!"EFI time is malformed.");

        Status = STATUS_UNSUCCESSFUL;
        goto GetTimeEnd;
    }

    //
    // Valid time zones are -1440 to 1440. If time zone is outside of
    // this then treat time in Local Time.
    //

    TimeZone = EfiTime.TimeZone;
    if ((TimeZone >= -1440 ) && (TimeZone <= 1440)) {
        TimeZoneBias.QuadPart = Int32x32To64(TimeZone * 60, 10000000);

        //
        //  UTC Time is UEFI Time + TimeZone (in nano sec.).
        //

        FirmwareTime.QuadPart += TimeZoneBias.QuadPart;
        RtlTimeToTimeFields(&FirmwareTime, TimeFields);
        TimeZoneBiasApplied = TRUE;
    }

GetTimeEnd:
    if (ARGUMENT_PRESENT(UtcTime) != FALSE) {
        *UtcTime = TimeZoneBiasApplied;
    }

    return Status;
}

ULONG
BlTimeGetRelativeTime (
    VOID
    )

/*++

Routine Description:

    Returns the time in seconds since some arbitrary starting point.

    N.B. In order for this routine to be accurate it must be called at
         least once per hour.

Arguments:

    None

Return Value:

    Time in seconds since some arbitrary starting point.

--*/

{

    return BlTimeGetRelativeTimeEx(TIME_FLAGS_DEFAULT);
}

ULONG
BlTimeGetRelativeTimeEx (
    __in ULONG Flags
    )

/*++

Routine Description:

    Returns the time since some arbitrary starting point. The time unit
    is specified through Flags. By default, it is returned in seconds.

Arguments:

    Flags - Supplies the desired time unit, including:

        TIME_FLAGS_DEFAULT: Indicates that the time returned is in default
            units, viz. seconds.

        TIME_FLAGS_MILLISECONDS: Indicates that the time returned is in
            milliseconds.

Return Value:

    Time since some arbitrary starting point.

--*/

{

    EFI_TIME EfiTime;
    ULONG RelativeTime;
    ULONG Seconds;
    NTSTATUS Status;

    //
    // Get the current time from the firmware. If this fails, return a
    // monotonically increasing value. Otherwise, calculate the relative time
    // from the base time.
    //

    Status = EfiGetTime(&EfiTime);
    if (!NT_SUCCESS(Status)) {
        BlpGetRelativeTimeErrorCount += 1;
        return BlpGetRelativeTimeErrorCount;
    }

    //
    // Compute a seconds count from the time fields.
    //

    Seconds = (EfiTime.Minute * 60) + EfiTime.Second;

    //
    // If the hour has changed, bump our base seconds count.
    //

    if (EfiTime.Hour != LastHour) {
        BaseSeconds += SECONDS_PER_HOUR;
    }

    LastHour = EfiTime.Hour;
    if (CHECK_FLAG(Flags, TIME_FLAGS_MILLISECONDS)) {
        RelativeTime = (((Seconds + BaseSeconds) * 1000) +
            (EfiTime.Nanosecond / 1000000));

    } else {
        RelativeTime = Seconds + BaseSeconds;
    }

    return RelativeTime;
}

ULONG
BlTimeGetTickCount (
    VOID
    )

/*++

Routine Description:

    This routine implements the standard 18.2Hz RTC up-counter.

Arguements:

    None

Return Value:

    32-bit tick count.

--*/

{

    ULONG64 Ticks;
    TIME_FIELDS Time;
    NTSTATUS Status;

    //
    // Convert the EFI bios time into a daily tick count.
    //

    Ticks = 0;
    Status = BlGetTime(&Time, NULL);
    if (NT_SUCCESS(Status)) {
        Ticks = Time.Milliseconds;
        Ticks += Time.Second * 1000;
        Ticks += Time.Minute * 60 * 1000;
        Ticks += Time.Hour * 60 * 60 * 1000;
        Ticks += Time.Day * 24 * 60 * 60 * 1000;
        Ticks *= 182;
        Ticks /= 10000;
    }

    return (ULONG)Ticks;
}

//
// ---------------------------------------------------------- Private Functions
//

NTSTATUS
BlpTimeMeasureTscFrequency (
    VOID
    )

/*++

Routine Description:

    This routine estimates TSC frequency.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS.

--*/

{

    ULONG64 CountCalibrate;
    ULONG64 CountDelta;
    ULONG64 CountDelta1Ms;
    ULONG64 CountEnd;
    ULONG64 CountStart;
    ULONG Retries;

    //
    // N.B. Longer stall time is only needed on ARM
    // not on ARM64 since ccounter runs in /64 on ARM
    // but with its full resolution on ARM64, hence the
    // shorter calibration time on ARM64.
    //

#if defined(_ARM_)

    const ULONG StallTimeInMs = 100;

#else

    const ULONG StallTimeInMs = 8;

#endif

    //
    // Use (EFI) stall execution interface to determine the Time Stamp Counter
    // (TSC) frequency. When running underneath any hypervisor, attempt to avoid
    // a case where CountDelta1Ms is larger than CountDelta due to the fact that
    // hypervisor might deschedule CPU between 1ms stall and the longer stall
    //
    // N.B. The TSC is used to implement the boot library performance counter.
    //

    if (HviIsAnyHypervisorPresent() != FALSE) {
        Retries = TSC_FREQUENCY_RETRIES;

    } else {
        Retries = 1;
    }

    do {
        EfiStall(1); // Align EFI stall to its tick boundary.

        //
        // Now stall to calibrate the TSC frequency.
        // Capture two sets of timings: the time for a 1ms stall, and the time
        // for a StallTimeInMs + 1ms stall. Use the 1ms stall to calibrate the
        // call overhead by subtracting that count from the actual count, and
        // adjust the stall time correspondingly.
        //

        CountCalibrate = ReadTimeStampCounter();
        EfiStall(1000);
        CountStart = ReadTimeStampCounter();
        EfiStall((StallTimeInMs+1) * 1000);
        CountEnd = ReadTimeStampCounter();
        CountDelta1Ms = CountStart - CountCalibrate;
        CountDelta = CountEnd - CountStart;
        Retries -= 1;
    } while ((CountDelta1Ms > CountDelta) && (Retries > 0));

    //
    // On ARM, fail if EfiStall stalled longer for 1ms than it did for 101ms.
    //

#if defined(_ARM_)

    if (CountDelta < CountDelta1Ms) {
        return STATUS_TIMER_RESOLUTION_NOT_SET;
    }

#endif

    CountDelta -= CountDelta1Ms;

    //
    // Convert the delta count to Hertz using this formula.
    // Do the division last to avoid truncating any significant bits.
    //
    // m Cycles   1000 ms    1 delta
    // -------- * ------- * --------- = cycles/sec = Hertz
    // 1 delta     1 sec     n msec
    //

    CountDelta *= 1000;
    CountDelta /= StallTimeInMs;
    BlpTimePerformanceFrequency.QuadPart = CountDelta;
    return STATUS_SUCCESS;
}

NTSTATUS
BlpTimeCalibratePerformanceCounter (
    VOID
    )

/*++

Routine Description:

    This routine determines the performance counter frequency.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS.

--*/

{

    HV_HYPERVISOR_FEATURES HypervisorFeatures;
    NTSTATUS Status = STATUS_SUCCESS;

    HviGetHypervisorFeatures(&HypervisorFeatures);
    if (HypervisorFeatures.FrequencyRegsAvailable != FALSE) {

#if (defined(_X86_) || defined(_AMD64_))

        BlpTimePerformanceFrequency.QuadPart = ReadMSR(HV_X64_MSR_TSC_FREQUENCY);

#elif defined(_ARM_)

        //
        // No ARM Hypervisor yet. 
        // Revisit this code when ARM hypervisor is implemented
        //

        NT_ASSERT(FALSE);

#endif

    } else {
        Status = BlpTimeMeasureTscFrequency();
    }

    return Status;
}

