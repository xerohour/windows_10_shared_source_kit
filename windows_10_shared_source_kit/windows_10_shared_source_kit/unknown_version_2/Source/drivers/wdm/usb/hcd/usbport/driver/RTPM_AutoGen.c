
/*++
Copyright (c) Microsoft Corporation

Module Name:

RTPM_AutoGen.c

Abstract:

     This header file contains State Machines for USBPORT
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/


#include "common.h"

RTPM_EVENT
USBPORTRTPM_AcknowledgingPowerInD0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_AcknowledgingPowerInD0 is called when the
    state machine enters the AcknowledgingPowerInD0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    PDEVICE_OBJECT                  fdoDeviceObject;
    RTPM_EVENT                      syncEvent;
    
    TRY {

        fdoDeviceObject = SmContext->FdoDeviceObject;
        USBPORTRTPM_AcknowledgePower(fdoDeviceObject);

        syncEvent = RtpmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // USBPORTRTPM_AcknowledgingPowerInD0


RTPM_EVENT
USBPORTRTPM_AcknowledgingPowerOnPowerFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_AcknowledgingPowerOnPowerFailure is called when the
    state machine enters the AcknowledgingPowerOnPowerFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    PDEVICE_OBJECT                  fdoDeviceObject;
    RTPM_EVENT                      syncEvent;
    
    TRY {

        fdoDeviceObject = SmContext->FdoDeviceObject;
        USBPORTRTPM_AcknowledgePower(fdoDeviceObject);

        syncEvent = RtpmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // USBPORTRTPM_AcknowledgingPowerOnPowerFailure


RTPM_EVENT
USBPORTRTPM_ArmingForWakeFromS0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_ArmingForWakeFromS0 is called when the
    state machine enters the ArmingForWakeFromS0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    PDEVICE_OBJECT                  fdoDeviceObject;
    RTPM_EVENT                      syncEvent;
    
    TRY {

        fdoDeviceObject = SmContext->FdoDeviceObject;
        syncEvent = USBPORTRTPM_ArmForWakeFromS0(fdoDeviceObject);

    } FINALLY {

    }

    return syncEvent;

} // USBPORTRTPM_ArmingForWakeFromS0


RTPM_EVENT
USBPORTRTPM_CompletingHubWaitWakeOnRequired(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_CompletingHubWaitWakeOnRequired is called when the
    state machine enters the CompletingHubWaitWakeOnRequired State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    PDEVICE_OBJECT                  fdoDeviceObject;
    RTPM_EVENT                      syncEvent;
    
    TRY {

        fdoDeviceObject = SmContext->FdoDeviceObject;
        USBPORTRTPM_CompleteHubWWIrp(fdoDeviceObject);

        syncEvent = RtpmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // USBPORTRTPM_CompletingHubWaitWakeOnRequired


RTPM_EVENT
USBPORTRTPM_CriticalPowerFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_CriticalPowerFailure is called when the
    state machine enters the CriticalPowerFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.
    //
    return RtpmEventNull;

} // USBPORTRTPM_CriticalPowerFailure


RTPM_EVENT
USBPORTRTPM_DisarmForWakeOnDxFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_DisarmForWakeOnDxFailure is called when the
    state machine enters the DisarmForWakeOnDxFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    PDEVICE_OBJECT                  fdoDeviceObject;
    
    TRY {

        fdoDeviceObject = SmContext->FdoDeviceObject;
        USBPORTRTPM_DisarmForWakeFromS0(fdoDeviceObject);

    } FINALLY {

    }

    return RtpmEventNull;

} // USBPORTRTPM_DisarmForWakeOnDxFailure


RTPM_EVENT
USBPORTRTPM_DisarmingForWakeOnPowerRequired(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_DisarmingForWakeOnPowerRequired is called when the
    state machine enters the DisarmingForWakeOnPowerRequired State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    PDEVICE_OBJECT                  fdoDeviceObject;
    
    TRY {

        fdoDeviceObject = SmContext->FdoDeviceObject;
        USBPORTRTPM_DisarmForWakeFromS0(fdoDeviceObject);

    } FINALLY {

    }

    return RtpmEventNull;

} // USBPORTRTPM_DisarmingForWakeOnPowerRequired


RTPM_EVENT
USBPORTRTPM_DxIdle(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_DxIdle is called when the
    state machine enters the DxIdle State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.
    //
    return RtpmEventNull;

} // USBPORTRTPM_DxIdle


RTPM_EVENT
USBPORTRTPM_InD0Idle(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_InD0Idle is called when the
    state machine enters the InD0Idle State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    PDEVICE_OBJECT                  fdoDeviceObject;
    
    TRY {

        fdoDeviceObject = SmContext->FdoDeviceObject;
        USBPORTRTPM_CompleteHubWWIrp(fdoDeviceObject);

        USBPORTRTPM_EnableTimerWork(fdoDeviceObject);

    } FINALLY {

    }

    return RtpmEventNull;

} // USBPORTRTPM_InD0Idle


RTPM_EVENT
USBPORTRTPM_InD0NotIdle(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_InD0NotIdle is called when the
    state machine enters the InD0NotIdle State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.
    //
    return RtpmEventNull;

} // USBPORTRTPM_InD0NotIdle


RTPM_EVENT
USBPORTRTPM_IndicatingCriticalPowerFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_IndicatingCriticalPowerFailure is called when the
    state machine enters the IndicatingCriticalPowerFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    PDEVICE_OBJECT                  fdoDeviceObject;
    RTPM_EVENT                      syncEvent;
    
    TRY {

        fdoDeviceObject = SmContext->FdoDeviceObject;
        USBPORTRTPM_IndicateCriticalPowerFailure(fdoDeviceObject);

        syncEvent = RtpmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // USBPORTRTPM_IndicatingCriticalPowerFailure


RTPM_EVENT
USBPORTRTPM_ReInitializingController(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_ReInitializingController is called when the
    state machine enters the ReInitializingController State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    PDEVICE_OBJECT                  fdoDeviceObject;
    RTPM_EVENT                      syncEvent;
    
    TRY {

        fdoDeviceObject = SmContext->FdoDeviceObject;
        syncEvent = USBPORTRTPM_EnableInterrupts(fdoDeviceObject);

        USBPORTRTPM_EnableTimerWork(fdoDeviceObject);

    } FINALLY {

    }

    return syncEvent;

} // USBPORTRTPM_ReInitializingController


RTPM_EVENT
USBPORTRTPM_ReInitializingControllerOnWake(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_ReInitializingControllerOnWake is called when the
    state machine enters the ReInitializingControllerOnWake State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    PDEVICE_OBJECT                  fdoDeviceObject;
    RTPM_EVENT                      syncEvent;
    
    TRY {

        fdoDeviceObject = SmContext->FdoDeviceObject;
        syncEvent = USBPORTRTPM_EnableInterrupts(fdoDeviceObject);

    } FINALLY {

    }

    return syncEvent;

} // USBPORTRTPM_ReInitializingControllerOnWake


RTPM_EVENT
USBPORTRTPM_RequestingD0OnWake(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_RequestingD0OnWake is called when the
    state machine enters the RequestingD0OnWake State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    PDEVICE_OBJECT                  fdoDeviceObject;
    RTPM_EVENT                      syncEvent;
    
    TRY {

        fdoDeviceObject = SmContext->FdoDeviceObject;
        syncEvent = USBPORTRTPM_RequestD0(fdoDeviceObject);

    } FINALLY {

    }

    return syncEvent;

} // USBPORTRTPM_RequestingD0OnWake


RTPM_EVENT
USBPORTRTPM_RequestingD0Required(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_RequestingD0Required is called when the
    state machine enters the RequestingD0Required State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    PDEVICE_OBJECT                  fdoDeviceObject;
    RTPM_EVENT                      syncEvent;
    
    TRY {

        fdoDeviceObject = SmContext->FdoDeviceObject;
        syncEvent = USBPORTRTPM_RequestD0(fdoDeviceObject);

    } FINALLY {

    }

    return syncEvent;

} // USBPORTRTPM_RequestingD0Required


RTPM_EVENT
USBPORTRTPM_RequestingDx(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_RequestingDx is called when the
    state machine enters the RequestingDx State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    PDEVICE_OBJECT                  fdoDeviceObject;
    RTPM_EVENT                      syncEvent;
    
    TRY {

        fdoDeviceObject = SmContext->FdoDeviceObject;
        USBPORTRTPM_DisableTimerWork(fdoDeviceObject);

        syncEvent = USBPORTRTPM_RequestDx(fdoDeviceObject);

    } FINALLY {

    }

    return syncEvent;

} // USBPORTRTPM_RequestingDx


RTPM_EVENT
USBPORTRTPM_WaitingForD0CompletionRequired(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_WaitingForD0CompletionRequired is called when the
    state machine enters the WaitingForD0CompletionRequired State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.
    //
    return RtpmEventNull;

} // USBPORTRTPM_WaitingForD0CompletionRequired


RTPM_EVENT
USBPORTRTPM_WaitingForD0CompletionWake(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_WaitingForD0CompletionWake is called when the
    state machine enters the WaitingForD0CompletionWake State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.
    //
    return RtpmEventNull;

} // USBPORTRTPM_WaitingForD0CompletionWake


RTPM_EVENT
USBPORTRTPM_WaitingForDxCompletion(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_WaitingForDxCompletion is called when the
    state machine enters the WaitingForDxCompletion State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.
    //
    return RtpmEventNull;

} // USBPORTRTPM_WaitingForDxCompletion


RTPM_EVENT
USBPORTRTPM_WaitingForDxCompletionOnWake(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    USBPORTRTPM_WaitingForDxCompletionOnWake is called when the
    state machine enters the WaitingForDxCompletionOnWake State

Arguments:

    SmContext - State Machine Context


Return Value:

    RTPM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.
    //
    return RtpmEventNull;

} // USBPORTRTPM_WaitingForDxCompletionOnWake





