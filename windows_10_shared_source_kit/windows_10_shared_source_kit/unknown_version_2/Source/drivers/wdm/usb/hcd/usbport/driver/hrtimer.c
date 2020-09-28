//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*++
Module Name:

    hrtimer.c

Abstract:

    High resolution timers

Environment:

    kernel mode only

Notes:

Revision History:

    01-04-2011 : created

--*/

#include <ntosp.h>

#include "extimer.h"

#pragma warning(disable : 4055)   // cast from data to function pointer

EXT_CALLBACK USBPORT_HighResCallbackWrapper;
PFN_EX_ALLOCATE_TIMER g_ExAllocateTimer;
PFN_EX_DELETE_TIMER g_ExDeleteTimer;
PFN_EX_SET_TIMER g_ExSetTimer;
PFN_EX_CANCEL_TIMER g_ExCancelTimer;


BOOLEAN HighResolutionTimerEnabled = FALSE;

VOID
USBPORT_InitializeHighResTimer(
    )
/*++

Routine Description:

    Initializes the high resolution timer by importing the enhanced timer function addresses.  This is done in case
    we ever need to ship on a down-level OS.

*/
{
    UNICODE_STRING functionName;

    RtlInitUnicodeString(&functionName, L"ExAllocateTimer");
    g_ExAllocateTimer = (PFN_EX_ALLOCATE_TIMER)MmGetSystemRoutineAddress(&functionName);

    if (g_ExAllocateTimer == NULL) {
        return;
    }

    RtlInitUnicodeString(&functionName, L"ExDeleteTimer");
    g_ExDeleteTimer  = (PFN_EX_DELETE_TIMER)MmGetSystemRoutineAddress(&functionName);

    if (g_ExDeleteTimer == NULL) {
        return;
    }

    RtlInitUnicodeString(&functionName, L"ExSetTimer");
    g_ExSetTimer  = (PFN_EX_SET_TIMER)MmGetSystemRoutineAddress(&functionName);

    if (g_ExSetTimer == NULL) {
        return;
    }

    RtlInitUnicodeString(&functionName, L"ExCancelTimer");
    g_ExCancelTimer  = (PFN_EX_CANCEL_TIMER)MmGetSystemRoutineAddress(&functionName);

    if (g_ExCancelTimer == NULL) {
        return;
    }

    HighResolutionTimerEnabled = TRUE;
}


PVOID
USBPORT_AllocateHighResTimer(
    _In_ PEXT_CALLBACK Callback,
    _In_opt_ PVOID Context
    )
/*++

Routine Description:

    Allocates a high resolution timer

Arguments:

    DeferredRoutine - Callback routine to be called when the timer expires

    Context - context value to be passed to the callback routine

Returns:

    Pointer to the high resolution timer cast as PVOID if successfully allocated, NULL otherwise.

*/
{
    if (HighResolutionTimerEnabled == FALSE) {
        return NULL;
    }

    return (PVOID)g_ExAllocateTimer(Callback,
                                    Context,
                                    EX_TIMER_HIGH_RESOLUTION);
}


VOID
USBPORT_SetHighResTimer(
    _In_ PVOID HighResTimer,
    _In_ LONG Milliseconds
    )
/*++

Routine Description:

    Starts a high resolution timer

Arguments:

    HighResTimer - High resolution timer to be started

    Milliseconds - Number of milliseconds for the timer to expire
*/
{
    BOOLEAN cancelled;

    NT_ASSERT(HighResolutionTimerEnabled == TRUE);

    cancelled = g_ExSetTimer((PEX_TIMER)HighResTimer, Milliseconds * (LONGLONG)-10000, 0, NULL);

    NT_ASSERT(cancelled == FALSE);
}


VOID
USBPORT_FreeHighResTimer(
    _In_ PVOID HighResTimer,
    _In_ BOOLEAN Synchronous
    )
/*++

Routine Description:

    Frees a high resolution timer

Arguments:

    HighResTimer - timer to be freed

    Synchronous - Indicates whether the call is synchronous or not

*/
{
    NT_ASSERT(HighResolutionTimerEnabled == TRUE);

    g_ExDeleteTimer((PEX_TIMER)HighResTimer, TRUE, Synchronous, NULL);

}


BOOLEAN
USBPORT_CancelHighResTimer(
    _In_ PVOID HighResTimer
    )
/*++

Routine Description:

    Cancels a high resolution timer

Arguments:

    HighResTimer - timer to be cancelled

Returns:

    TRUE if the timer was successfully cancelled, FALSE otherwise.

*/
{
    NT_ASSERT(HighResolutionTimerEnabled == TRUE);

    return (g_ExCancelTimer((PEX_TIMER)HighResTimer, NULL));
}


_Function_class_(KDEFERRED_ROUTINE)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_min_(DISPATCH_LEVEL)
_IRQL_requires_(DISPATCH_LEVEL)
_IRQL_requires_same_
VOID
USBPORT_HighResTimerCompletion (
    __inout PEX_TIMER Timer,
    __inout  PKEVENT Event
    )

/*++

Routine Description:

    Completion routine for high resolution timer

Arguments:

    Dpc -

    Event - Event to signal completion of the wait

--*/

{
    UNREFERENCED_PARAMETER(Timer);

    KeSetEvent(Event, IO_NO_INCREMENT, FALSE);
}



BOOLEAN
USBPORT_HighResWait(
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
    PVOID timer;

    if (HighResolutionTimerEnabled == FALSE) {
        return FALSE;
    }

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    timer = USBPORT_AllocateHighResTimer(USBPORT_HighResTimerCompletion, &event);

    if (timer == NULL) {
        return FALSE;
    }

    USBPORT_SetHighResTimer(timer, Milliseconds);

    KeWaitForSingleObject(&event,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);

    USBPORT_FreeHighResTimer(timer, TRUE);

    return TRUE;
}
