/*
Module: HRTIMER.C


name:USBHUB High Resolution Timer

    Copyright (c) Microsoft Corporation. All rights reserved

created:

    01-02-2013

Abstract:

    Code for using high resolution timers

*/

#include <ntosp.h>

#include "extimer.h"

#pragma warning(disable : 4055)   // cast from data to function pointer

EXT_CALLBACK UsbhHighResTimerCompletion;

PFN_EX_ALLOCATE_TIMER g_ExAllocateTimer;
PFN_EX_DELETE_TIMER g_ExDeleteTimer;
PFN_EX_SET_TIMER g_ExSetTimer;
PFN_EX_CANCEL_TIMER g_ExCancelTimer;


BOOLEAN
UsbhInitializeHighResTimer(
    )
/*++

Routine Description:

    Initializes the high resolution timer by importing the EX_TIMER function addresses.  This is done in case
    we ever need to ship on a down-level OS.

Returns:

    TRUE if high resolution timer is available, FALSE otherwise.

*/
{
    UNICODE_STRING functionName;

    RtlInitUnicodeString(&functionName, L"ExAllocateTimer");
    g_ExAllocateTimer = (PFN_EX_ALLOCATE_TIMER)MmGetSystemRoutineAddress(&functionName);

    if (g_ExAllocateTimer == NULL) {
        return FALSE;
    }

    RtlInitUnicodeString(&functionName, L"ExDeleteTimer");
    g_ExDeleteTimer  = (PFN_EX_DELETE_TIMER)MmGetSystemRoutineAddress(&functionName);

    if (g_ExDeleteTimer == NULL) {
        return FALSE;
    }

    RtlInitUnicodeString(&functionName, L"ExSetTimer");
    g_ExSetTimer  = (PFN_EX_SET_TIMER)MmGetSystemRoutineAddress(&functionName);

    if (g_ExSetTimer == NULL) {
        return FALSE;
    }

    RtlInitUnicodeString(&functionName, L"ExCancelTimer");
    g_ExCancelTimer  = (PFN_EX_CANCEL_TIMER)MmGetSystemRoutineAddress(&functionName);

    if (g_ExCancelTimer == NULL) {
        return FALSE;
    }

    return TRUE;
}

VOID
UsbhHighResTimerCompletion(
    PEX_TIMER Timer,
    PVOID Context
    )
/*++

Routine Description:

    Completion routine for high resolution timer

Arguments:

    Dpc -

    Event - Event to signal completion of the wait

--*/

{
    PKEVENT event;

    UNREFERENCED_PARAMETER(Timer);

    event = (PKEVENT)Context;

    __analysis_assume(Context != NULL);

    KeSetEvent(event, IO_NO_INCREMENT, FALSE);
}



BOOLEAN
UsbhHighResWait(
    ULONG Milliseconds
    )
/*++

Routine Description:

    Waits for the specified number of milliseconds using the high resolution timer

Arguments:

    Milliseconds - Number of milliseconds to wait.

Returns:

    TRUE if the wait was successful, FALSE otherwise.

    */
{
    KEVENT event;
    PEX_TIMER timer;
    LONGLONG dueTime;
    BOOLEAN cancelled;

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    timer = g_ExAllocateTimer(UsbhHighResTimerCompletion,
                              &event,
                              EX_TIMER_HIGH_RESOLUTION);

    if (timer == NULL) {
        return FALSE;
    }

    dueTime = Milliseconds * (LONGLONG)-10000;

    cancelled = g_ExSetTimer(timer, dueTime, 0, NULL);

    NT_ASSERT(cancelled == FALSE);

    KeWaitForSingleObject(&event,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);

    g_ExDeleteTimer(timer, TRUE, TRUE, NULL);

    return TRUE;
}

