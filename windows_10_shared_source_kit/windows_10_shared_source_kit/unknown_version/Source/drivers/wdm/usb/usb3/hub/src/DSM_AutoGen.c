
/*++
Copyright (c) Microsoft Corporation

Module Name:

DSM_AutoGen.c

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/


#include <pch.h>

DSM_EVENT
HUBDSM_AckingD0ForFaultyClientDrivers(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AckingD0ForFaultyClientDrivers is called when the
    state machine enters the AckingD0ForFaultyClientDrivers State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AckingD0ForFaultyClientDrivers


DSM_EVENT
HUBDSM_AckingHubStopAfterSuspendFromStoppedState(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AckingHubStopAfterSuspendFromStoppedState is called when the
    state machine enters the AckingHubStopAfterSuspendFromStoppedState State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AckStopAfterSuspend(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AckingHubStopAfterSuspendFromStoppedState


DSM_EVENT
HUBDSM_AckingPdoPreStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AckingPdoPreStart is called when the
    state machine enters the AckingPdoPreStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompletePdoPreStart(deviceContext);

        HUBPDO_ClearFailIo(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AckingPdoPreStart


DSM_EVENT
HUBDSM_AckingPdoPreStartFromStoppedEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AckingPdoPreStartFromStoppedEnabled is called when the
    state machine enters the AckingPdoPreStartFromStoppedEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_ClearFailIo(deviceContext);

        HUBPDO_CompletePdoPreStart(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AckingPdoPreStartFromStoppedEnabled


DSM_EVENT
HUBDSM_AckingQueryResourcesInConfiguredD0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AckingQueryResourcesInConfiguredD0 is called when the
    state machine enters the AckingQueryResourcesInConfiguredD0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AckingQueryResourcesInConfiguredD0


DSM_EVENT
HUBDSM_AckingStopAfterSuspendOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AckingStopAfterSuspendOnDetach is called when the
    state machine enters the AckingStopAfterSuspendOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AckStopAfterSuspend(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AckingStopAfterSuspendOnDetach


DSM_EVENT
HUBDSM_AcquiringPowerReferenceOnHubOnAttach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AcquiringPowerReferenceOnHubOnAttach is called when the
    state machine enters the AcquiringPowerReferenceOnHubOnAttach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_AcquireWdfPowerReferenceOnHub(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AcquiringPowerReferenceOnHubOnAttach


DSM_EVENT
HUBDSM_AcquiringPowerReferenceOnHubStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AcquiringPowerReferenceOnHubStart is called when the
    state machine enters the AcquiringPowerReferenceOnHubStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AcquiringPowerReferenceOnHubStart


DSM_EVENT
HUBDSM_BugcheckingSystemOnBootDeviceEnumerationFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_BugcheckingSystemOnBootDeviceEnumerationFailure is called when the
    state machine enters the BugcheckingSystemOnBootDeviceEnumerationFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
//
// Temperorily disable the warning for unreachable code
// We need to return an event from this function because
// it is a state machine function
//
#pragma warning(push)
#pragma warning(disable:4702)
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_BugcheckSystemOnBootDeviceEnumerationFailure(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_BugcheckingSystemOnBootDeviceEnumerationFailure


#pragma warning(pop)
DSM_EVENT
HUBDSM_CheckIfDeviceCanBeDisabledOnDriverNotFound(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckIfDeviceCanBeDisabledOnDriverNotFound is called when the
    state machine enters the CheckIfDeviceCanBeDisabledOnDriverNotFound State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfDeviceShouldBeDisabledOnRemove(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckIfDeviceCanBeDisabledOnDriverNotFound


DSM_EVENT
HUBDSM_CheckingForZeroBwPipeOnClientClearStall(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingForZeroBwPipeOnClientClearStall is called when the
    state machine enters the CheckingForZeroBwPipeOnClientClearStall State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_CheckIfTargetPipeIsForZeroBw(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingForZeroBwPipeOnClientClearStall


DSM_EVENT
HUBDSM_CheckingForZeroBwPipeOnClientResetPipe(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingForZeroBwPipeOnClientResetPipe is called when the
    state machine enters the CheckingForZeroBwPipeOnClientResetPipe State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_CheckIfTargetPipeIsForZeroBw(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingForZeroBwPipeOnClientResetPipe


DSM_EVENT
HUBDSM_CheckingForZeroBwPipeOnClientSyncResetPipe(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingForZeroBwPipeOnClientSyncResetPipe is called when the
    state machine enters the CheckingForZeroBwPipeOnClientSyncResetPipe State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_CheckIfTargetPipeIsForZeroBw(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingForZeroBwPipeOnClientSyncResetPipe


DSM_EVENT
HUBDSM_CheckingIfBootDeviceOnD0Exit(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfBootDeviceOnD0Exit is called when the
    state machine enters the CheckingIfBootDeviceOnD0Exit State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_ClearResetOnLastResumeFlag(deviceContext);

        syncEvent = HUBPDO_IsItABootDevice(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfBootDeviceOnD0Exit


DSM_EVENT
HUBDSM_CheckingIfBootDeviceOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfBootDeviceOnDetach is called when the
    state machine enters the CheckingIfBootDeviceOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_IsItABootDevice(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfBootDeviceOnDetach


DSM_EVENT
HUBDSM_CheckingIfBootDeviceOnReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfBootDeviceOnReset is called when the
    state machine enters the CheckingIfBootDeviceOnReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SetDeviceFlagToForceResetOnEnumeration(deviceContext);

        syncEvent = HUBPDO_IsItABootDevice(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfBootDeviceOnReset


DSM_EVENT
HUBDSM_CheckingIfDeviceHasReceivedFirstStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDeviceHasReceivedFirstStart is called when the
    state machine enters the CheckingIfDeviceHasReceivedFirstStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_CheckIfPdoIsStarted(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDeviceHasReceivedFirstStart


DSM_EVENT
HUBDSM_CheckingIfDeviceHasReceivedFirstStartInUncoonfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDeviceHasReceivedFirstStartInUncoonfigured is called when the
    state machine enters the CheckingIfDeviceHasReceivedFirstStartInUncoonfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_CheckIfPdoIsStarted(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDeviceHasReceivedFirstStartInUncoonfigured


DSM_EVENT
HUBDSM_CheckingIfDeviceShouldBeDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDeviceShouldBeDisabled is called when the
    state machine enters the CheckingIfDeviceShouldBeDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfDeviceShouldBeDisabledOnRemove(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDeviceShouldBeDisabled


DSM_EVENT
HUBDSM_CheckingIfItIsIsochronousEndpoint(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfItIsIsochronousEndpoint is called when the
    state machine enters the CheckingIfItIsIsochronousEndpoint State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_CheckIfTargetPipeIsIsochronous(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfItIsIsochronousEndpoint


DSM_EVENT
HUBDSM_CleaningUpAnyExistingConfigurationOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CleaningUpAnyExistingConfigurationOnStart is called when the
    state machine enters the CleaningUpAnyExistingConfigurationOnStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteEndpointsAndFreeResourcesForCurrentConfiguration(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CleaningUpAnyExistingConfigurationOnStart


DSM_EVENT
HUBDSM_ClearingEndpointHaltOnClientClearStall(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ClearingEndpointHaltOnClientClearStall is called when the
    state machine enters the ClearingEndpointHaltOnClientClearStall State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_ClearEndpointHaltUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ClearingEndpointHaltOnClientClearStall


DSM_EVENT
HUBDSM_ClearingEndpointHaltOnClientResetPipe(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ClearingEndpointHaltOnClientResetPipe is called when the
    state machine enters the ClearingEndpointHaltOnClientResetPipe State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_ClearEndpointHaltUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ClearingEndpointHaltOnClientResetPipe


DSM_EVENT
HUBDSM_ClearingFailIoFlagOnSuccessfulRecovery(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ClearingFailIoFlagOnSuccessfulRecovery is called when the
    state machine enters the ClearingFailIoFlagOnSuccessfulRecovery State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_NotifyDeviceReconnected(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ClearingFailIoFlagOnSuccessfulRecovery


DSM_EVENT
HUBDSM_CompletingClientRequestInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingClientRequestInConfigured is called when the
    state machine enters the CompletingClientRequestInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteClientSerialRequestWithStatusSuccess(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingClientRequestInConfigured


DSM_EVENT
HUBDSM_CompletingClientRequestInConfiguredWithLastStatus(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingClientRequestInConfiguredWithLastStatus is called when the
    state machine enters the CompletingClientRequestInConfiguredWithLastStatus State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteClientSerialRequestWithLastStatus(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingClientRequestInConfiguredWithLastStatus


DSM_EVENT
HUBDSM_CompletingClientRequestInConfiguredWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingClientRequestInConfiguredWithSpecialFile is called when the
    state machine enters the CompletingClientRequestInConfiguredWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteClientSerialRequestWithStatusSuccess(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingClientRequestInConfiguredWithSpecialFile


DSM_EVENT
HUBDSM_CompletingClientRequestInUnconfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingClientRequestInUnconfigured is called when the
    state machine enters the CompletingClientRequestInUnconfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteClientSerialRequestWithStatusSuccess(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingClientRequestInUnconfigured


DSM_EVENT
HUBDSM_CompletingClientRequestInUnConfiguredWithLastStatus(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingClientRequestInUnConfiguredWithLastStatus is called when the
    state machine enters the CompletingClientRequestInUnConfiguredWithLastStatus State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteClientSerialRequestWithLastStatus(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingClientRequestInUnConfiguredWithLastStatus


DSM_EVENT
HUBDSM_CompletingClientRequestOnDetachInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingClientRequestOnDetachInConfigured is called when the
    state machine enters the CompletingClientRequestOnDetachInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteClientSerialRequestWithStatusFailed(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingClientRequestOnDetachInConfigured


DSM_EVENT
HUBDSM_CompletingClientRequestOnDetachInDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingClientRequestOnDetachInDisabled is called when the
    state machine enters the CompletingClientRequestOnDetachInDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteClientSerialRequestWithStatusFailed(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingClientRequestOnDetachInDisabled


DSM_EVENT
HUBDSM_CompletingClientRequestOnDetachWithDeviceProgrammed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingClientRequestOnDetachWithDeviceProgrammed is called when the
    state machine enters the CompletingClientRequestOnDetachWithDeviceProgrammed State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteClientSerialRequestWithStatusFailed(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingClientRequestOnDetachWithDeviceProgrammed


DSM_EVENT
HUBDSM_CompletingClientRequestOnFailureInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingClientRequestOnFailureInConfigured is called when the
    state machine enters the CompletingClientRequestOnFailureInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteClientSerialRequestWithLastStatus(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingClientRequestOnFailureInConfigured


DSM_EVENT
HUBDSM_CompletingClientRequestOnFailureInDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingClientRequestOnFailureInDisabled is called when the
    state machine enters the CompletingClientRequestOnFailureInDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteClientSerialRequestWithStatusFailed(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingClientRequestOnFailureInDisabled


DSM_EVENT
HUBDSM_CompletingClientRequestOnFailureInUnconfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingClientRequestOnFailureInUnconfigured is called when the
    state machine enters the CompletingClientRequestOnFailureInUnconfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteClientSerialRequestWithStatusFailed(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingClientRequestOnFailureInUnconfigured


DSM_EVENT
HUBDSM_CompletingClientUnconfigureRequest(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingClientUnconfigureRequest is called when the
    state machine enters the CompletingClientUnconfigureRequest State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteEndpointsAndFreeResourcesForCurrentConfiguration(deviceContext);

        HUBPDO_CompleteClientSerialRequestWithStatusSuccess(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingClientUnconfigureRequest


DSM_EVENT
HUBDSM_CompletingClientUnconfigureRequestWithFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingClientUnconfigureRequestWithFailure is called when the
    state machine enters the CompletingClientUnconfigureRequestWithFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteEndpointsAndFreeResourcesForCurrentConfiguration(deviceContext);

        HUBPDO_CompleteClientSerialRequestWithLastStatus(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingClientUnconfigureRequestWithFailure


DSM_EVENT
HUBDSM_CompletingD0EntryInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingD0EntryInConfigured is called when the
    state machine enters the CompletingD0EntryInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingD0EntryInConfigured


DSM_EVENT
HUBDSM_CompletingD0EntryInConfiguredWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingD0EntryInConfiguredWithSpecialFile is called when the
    state machine enters the CompletingD0EntryInConfiguredWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingD0EntryInConfiguredWithSpecialFile


DSM_EVENT
HUBDSM_CompletingD0EntryInConfiguredWithSpecialFileOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingD0EntryInConfiguredWithSpecialFileOnDetach is called when the
    state machine enters the CompletingD0EntryInConfiguredWithSpecialFileOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingD0EntryInConfiguredWithSpecialFileOnDetach


DSM_EVENT
HUBDSM_CompletingD0EntryInConfiguredWithSpecialFileOnDetachWithDeviceEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingD0EntryInConfiguredWithSpecialFileOnDetachWithDeviceEnabled is called when the
    state machine enters the CompletingD0EntryInConfiguredWithSpecialFileOnDetachWithDeviceEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingD0EntryInConfiguredWithSpecialFileOnDetachWithDeviceEnabled


DSM_EVENT
HUBDSM_CompletingD0EntryInUnconfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingD0EntryInUnconfigured is called when the
    state machine enters the CompletingD0EntryInUnconfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingD0EntryInUnconfigured


DSM_EVENT
HUBDSM_CompletingD0EntryOnDetachWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingD0EntryOnDetachWithSpecialFile is called when the
    state machine enters the CompletingD0EntryOnDetachWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingD0EntryOnDetachWithSpecialFile


DSM_EVENT
HUBDSM_CompletingD0ExitAfterHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingD0ExitAfterHubSuspend is called when the
    state machine enters the CompletingD0ExitAfterHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingD0ExitAfterHubSuspend


DSM_EVENT
HUBDSM_CompletingD0ExitOnSuspendFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingD0ExitOnSuspendFailure is called when the
    state machine enters the CompletingD0ExitOnSuspendFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingD0ExitOnSuspendFailure


DSM_EVENT
HUBDSM_CompletingDeviceCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingDeviceCleanup is called when the
    state machine enters the CompletingDeviceCleanup State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingDeviceCleanup


DSM_EVENT
HUBDSM_CompletingHubStopAfterSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingHubStopAfterSuspend is called when the
    state machine enters the CompletingHubStopAfterSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AckStopAfterSuspend(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingHubStopAfterSuspend


DSM_EVENT
HUBDSM_CompletingHubStopWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingHubStopWithPortOff is called when the
    state machine enters the CompletingHubStopWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingHubStopWithPortOff


DSM_EVENT
HUBDSM_CompletingPDOCleanupForUnknownDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingPDOCleanupForUnknownDevice is called when the
    state machine enters the CompletingPDOCleanupForUnknownDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingPDOCleanupForUnknownDevice


DSM_EVENT
HUBDSM_CompletingPdoExitFinalOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingPdoExitFinalOnDetach is called when the
    state machine enters the CompletingPdoExitFinalOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingPdoExitFinalOnDetach


DSM_EVENT
HUBDSM_CompletingPdoExitFinalOnDetachOnDeviceReportedMissing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingPdoExitFinalOnDetachOnDeviceReportedMissing is called when the
    state machine enters the CompletingPdoExitFinalOnDetachOnDeviceReportedMissing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingPdoExitFinalOnDetachOnDeviceReportedMissing


DSM_EVENT
HUBDSM_CompletingPdoReStartOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingPdoReStartOnDetach is called when the
    state machine enters the CompletingPdoReStartOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompletePdoPreStart(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingPdoReStartOnDetach


DSM_EVENT
HUBDSM_CompletingPdoReStartOnDetachWithDeviceEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingPdoReStartOnDetachWithDeviceEnabled is called when the
    state machine enters the CompletingPdoReStartOnDetachWithDeviceEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompletePdoPreStart(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingPdoReStartOnDetachWithDeviceEnabled


DSM_EVENT
HUBDSM_CompletingPdoReStartOnOperationFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingPdoReStartOnOperationFailure is called when the
    state machine enters the CompletingPdoReStartOnOperationFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompletePdoPreStart(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingPdoReStartOnOperationFailure


DSM_EVENT
HUBDSM_CompletingPnpEventOnDetachInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingPnpEventOnDetachInConfigured is called when the
    state machine enters the CompletingPnpEventOnDetachInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingPnpEventOnDetachInConfigured


DSM_EVENT
HUBDSM_CompletingPnpEventOnDetachInDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingPnpEventOnDetachInDisabled is called when the
    state machine enters the CompletingPnpEventOnDetachInDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingPnpEventOnDetachInDisabled


DSM_EVENT
HUBDSM_CompletingPnpEventOnDetachInUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingPnpEventOnDetachInUnConfigured is called when the
    state machine enters the CompletingPnpEventOnDetachInUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingPnpEventOnDetachInUnConfigured


DSM_EVENT
HUBDSM_CompletingPnpEventOnDetachWhileRenumeratingOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingPnpEventOnDetachWhileRenumeratingOnResume is called when the
    state machine enters the CompletingPnpEventOnDetachWhileRenumeratingOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingPnpEventOnDetachWhileRenumeratingOnResume


DSM_EVENT
HUBDSM_CompletingPnpEventOnFailureInDisabledWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingPnpEventOnFailureInDisabledWithPortOff is called when the
    state machine enters the CompletingPnpEventOnFailureInDisabledWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingPnpEventOnFailureInDisabledWithPortOff


DSM_EVENT
HUBDSM_CompletingPnpEventOnFailureWithEndpointsToBeDeleted(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingPnpEventOnFailureWithEndpointsToBeDeleted is called when the
    state machine enters the CompletingPnpEventOnFailureWithEndpointsToBeDeleted State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingPnpEventOnFailureWithEndpointsToBeDeleted


DSM_EVENT
HUBDSM_ConfiguredInD0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguredInD0 is called when the
    state machine enters the ConfiguredInD0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ConfiguredInD0


DSM_EVENT
HUBDSM_ConfiguredResumingOnHubResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguredResumingOnHubResume is called when the
    state machine enters the ConfiguredResumingOnHubResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestResume);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ConfiguredResumingOnHubResume


DSM_EVENT
HUBDSM_ConfiguredSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguredSuspended is called when the
    state machine enters the ConfiguredSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ConfiguredSuspended


DSM_EVENT
HUBDSM_ConfiguredSuspendedOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguredSuspendedOnHubSuspend is called when the
    state machine enters the ConfiguredSuspendedOnHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ConfiguredSuspendedOnHubSuspend


DSM_EVENT
HUBDSM_ConfiguredSuspendedWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguredSuspendedWithSpecialFile is called when the
    state machine enters the ConfiguredSuspendedWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ConfiguredSuspendedWithSpecialFile


DSM_EVENT
HUBDSM_ConfiguredSuspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguredSuspending is called when the
    state machine enters the ConfiguredSuspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ConfiguredSuspending


DSM_EVENT
HUBDSM_ConfiguredSuspendingOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguredSuspendingOnHubSuspend is called when the
    state machine enters the ConfiguredSuspendingOnHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestSuspend);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ConfiguredSuspendingOnHubSuspend


DSM_EVENT
HUBDSM_ConfiguredSuspendingWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguredSuspendingWithSpecialFile is called when the
    state machine enters the ConfiguredSuspendingWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ConfiguredSuspendingWithSpecialFile


DSM_EVENT
HUBDSM_ConfiguringDeviceFromCachedInformation(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguringDeviceFromCachedInformation is called when the
    state machine enters the ConfiguringDeviceFromCachedInformation State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ConfiguringDeviceFromCachedInformation


DSM_EVENT
HUBDSM_ConfiguringDeviceFromCachedInformationOnHubResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguringDeviceFromCachedInformationOnHubResume is called when the
    state machine enters the ConfiguringDeviceFromCachedInformationOnHubResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ConfiguringDeviceFromCachedInformationOnHubResume


DSM_EVENT
HUBDSM_ConfiguringDeviceFromCachedInformationOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguringDeviceFromCachedInformationOnResume is called when the
    state machine enters the ConfiguringDeviceFromCachedInformationOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ConfiguringDeviceFromCachedInformationOnResume


DSM_EVENT
HUBDSM_ConfiguringDeviceFromCachedInformationOnResumeWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguringDeviceFromCachedInformationOnResumeWithSpecialFile is called when the
    state machine enters the ConfiguringDeviceFromCachedInformationOnResumeWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ConfiguringDeviceFromCachedInformationOnResumeWithSpecialFile


DSM_EVENT
HUBDSM_ConfiguringDeviceFromCachedInformationWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguringDeviceFromCachedInformationWithSpecialFile is called when the
    state machine enters the ConfiguringDeviceFromCachedInformationWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ConfiguringDeviceFromCachedInformationWithSpecialFile


DSM_EVENT
HUBDSM_ConfiguringOnSelectConfiguration(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguringOnSelectConfiguration is called when the
    state machine enters the ConfiguringOnSelectConfiguration State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ConfiguringOnSelectConfiguration


DSM_EVENT
HUBDSM_CyclingPortOnRenumerationOfUnknownDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CyclingPortOnRenumerationOfUnknownDevice is called when the
    state machine enters the CyclingPortOnRenumerationOfUnknownDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestCycle);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CyclingPortOnRenumerationOfUnknownDevice


DSM_EVENT
HUBDSM_D0EntryForUnknownDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_D0EntryForUnknownDevice is called when the
    state machine enters the D0EntryForUnknownDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_D0EntryForUnknownDevice


DSM_EVENT
HUBDSM_DeConfiguringDeviceOnClientRequest(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeConfiguringDeviceOnClientRequest is called when the
    state machine enters the DeConfiguringDeviceOnClientRequest State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_SetDeviceNullConfigurationUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DeConfiguringDeviceOnClientRequest


DSM_EVENT
HUBDSM_DeConfiguringDeviceOnClientRequestFromUnconfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeConfiguringDeviceOnClientRequestFromUnconfigured is called when the
    state machine enters the DeConfiguringDeviceOnClientRequestFromUnconfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_SetDeviceNullConfigurationUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DeConfiguringDeviceOnClientRequestFromUnconfigured


DSM_EVENT
HUBDSM_DeletingDefaultEndpointAndDeviceOnDetachDuringEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingDefaultEndpointAndDeviceOnDetachDuringEnum is called when the
    state machine enters the DeletingDefaultEndpointAndDeviceOnDetachDuringEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingDefaultEndpointAndDeviceOnDetachDuringEnum


DSM_EVENT
HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanup is called when the
    state machine enters the DeletingDeviceAndDefaultEndpointOnCleanup State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteEndpointsAndFreeResourcesForCurrentConfiguration(deviceContext);

        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        HUBMISC_RemoveDeviceInfoFromGlobalChildList(deviceContext);

        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanup


DSM_EVENT
HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanupAfterDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanupAfterDetach is called when the
    state machine enters the DeletingDeviceAndDefaultEndpointOnCleanupAfterDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteEndpointsAndFreeResourcesForCurrentConfiguration(deviceContext);

        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanupAfterDetach


DSM_EVENT
HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanupWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanupWithPortOff is called when the
    state machine enters the DeletingDeviceAndDefaultEndpointOnCleanupWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteEndpointsAndFreeResourcesForCurrentConfiguration(deviceContext);

        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        HUBMISC_RemoveDeviceInfoFromGlobalChildList(deviceContext);

        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanupWithPortOff


DSM_EVENT
HUBDSM_DeletingEndpoinstAndDeviceOnDetachOnCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingEndpoinstAndDeviceOnDetachOnCleanup is called when the
    state machine enters the DeletingEndpoinstAndDeviceOnDetachOnCleanup State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteEndpointsAndFreeResourcesForCurrentConfiguration(deviceContext);

        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        HUBMISC_RemoveDeviceInfoFromGlobalChildList(deviceContext);

        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingEndpoinstAndDeviceOnDetachOnCleanup


DSM_EVENT
HUBDSM_DeletingEndpointsAndDeviceOnOnCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingEndpointsAndDeviceOnOnCleanup is called when the
    state machine enters the DeletingEndpointsAndDeviceOnOnCleanup State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteEndpointsAndFreeResourcesForCurrentConfiguration(deviceContext);

        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        HUBMISC_RemoveDeviceInfoFromGlobalChildList(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingEndpointsAndDeviceOnOnCleanup


DSM_EVENT
HUBDSM_DeporgrammingAllEndpointsOnCleanupFromEnumeratedConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeporgrammingAllEndpointsOnCleanupFromEnumeratedConfigured is called when the
    state machine enters the DeporgrammingAllEndpointsOnCleanupFromEnumeratedConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DeporgrammingAllEndpointsOnCleanupFromEnumeratedConfigured


DSM_EVENT
HUBDSM_DeporgrammingAllEndpointsOnHubStopFromEnumeratedConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeporgrammingAllEndpointsOnHubStopFromEnumeratedConfigured is called when the
    state machine enters the DeporgrammingAllEndpointsOnHubStopFromEnumeratedConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DeporgrammingAllEndpointsOnHubStopFromEnumeratedConfigured


DSM_EVENT
HUBDSM_DeprogrammingAllEndpointsOnHubSuspendFromEnumeratedConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeprogrammingAllEndpointsOnHubSuspendFromEnumeratedConfigured is called when the
    state machine enters the DeprogrammingAllEndpointsOnHubSuspendFromEnumeratedConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DeprogrammingAllEndpointsOnHubSuspendFromEnumeratedConfigured


DSM_EVENT
HUBDSM_DetachedAndDeprogrammedWithResetPending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DetachedAndDeprogrammedWithResetPending is called when the
    state machine enters the DetachedAndDeprogrammedWithResetPending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SetDeviceLostPowerFlag(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DetachedAndDeprogrammedWithResetPending


DSM_EVENT
HUBDSM_DetachedWithResetPending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DetachedWithResetPending is called when the
    state machine enters the DetachedWithResetPending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SetDeviceLostPowerFlag(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DetachedWithResetPending


DSM_EVENT
HUBDSM_DeviceDetachedAndDeporgrammedWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeviceDetachedAndDeporgrammedWithSpecialFile is called when the
    state machine enters the DeviceDetachedAndDeporgrammedWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceDetached);

        HUBPDO_SetDeviceLostPowerFlag(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DeviceDetachedAndDeporgrammedWithSpecialFile


DSM_EVENT
HUBDSM_DeviceDetachedWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeviceDetachedWithSpecialFile is called when the
    state machine enters the DeviceDetachedWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SetDeviceLostPowerFlag(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DeviceDetachedWithSpecialFile


DSM_EVENT
HUBDSM_DisablingBackPortInEnumeratedUnknown(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingBackPortInEnumeratedUnknown is called when the
    state machine enters the DisablingBackPortInEnumeratedUnknown State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingBackPortInEnumeratedUnknown


DSM_EVENT
HUBDSM_DisablingBackPortInWaitingForD0EntryForFailedDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingBackPortInWaitingForD0EntryForFailedDevice is called when the
    state machine enters the DisablingBackPortInWaitingForD0EntryForFailedDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingBackPortInWaitingForD0EntryForFailedDevice


DSM_EVENT
HUBDSM_DisablingDeviceAndDefaultEndpointInControllerOnDetachDuringEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceAndDefaultEndpointInControllerOnDetachDuringEnum is called when the
    state machine enters the DisablingDeviceAndDefaultEndpointInControllerOnDetachDuringEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceAndDefaultEndpointInControllerOnDetachDuringEnum


DSM_EVENT
HUBDSM_DisablingDeviceAndDefaultEndpointOnDetachOnHubStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceAndDefaultEndpointOnDetachOnHubStart is called when the
    state machine enters the DisablingDeviceAndDefaultEndpointOnDetachOnHubStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceAndDefaultEndpointOnDetachOnHubStart


DSM_EVENT
HUBDSM_DisablingDeviceAndDefaultEndpointOnEnumFailureOnHubStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceAndDefaultEndpointOnEnumFailureOnHubStart is called when the
    state machine enters the DisablingDeviceAndDefaultEndpointOnEnumFailureOnHubStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceAndDefaultEndpointOnEnumFailureOnHubStart


DSM_EVENT
HUBDSM_DisablingDeviceAndDefaultEpInControllerOFailureWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceAndDefaultEpInControllerOFailureWithPortOff is called when the
    state machine enters the DisablingDeviceAndDefaultEpInControllerOFailureWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceAndDefaultEpInControllerOFailureWithPortOff


DSM_EVENT
HUBDSM_DisablingDeviceAndDefaultEpInControllerOnCleanupWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnCleanupWithPortOff is called when the
    state machine enters the DisablingDeviceAndDefaultEpInControllerOnCleanupWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceAndDefaultEpInControllerOnCleanupWithPortOff


DSM_EVENT
HUBDSM_DisablingDeviceAndDefaultEpInControllerOnD0ExitOnDetachWithPDOMissing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnD0ExitOnDetachWithPDOMissing is called when the
    state machine enters the DisablingDeviceAndDefaultEpInControllerOnD0ExitOnDetachWithPDOMissing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceAndDefaultEpInControllerOnD0ExitOnDetachWithPDOMissing


DSM_EVENT
HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetach is called when the
    state machine enters the DisablingDeviceAndDefaultEpInControllerOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetach


DSM_EVENT
HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetachOnHubStopWithPDOMissing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetachOnHubStopWithPDOMissing is called when the
    state machine enters the DisablingDeviceAndDefaultEpInControllerOnDetachOnHubStopWithPDOMissing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetachOnHubStopWithPDOMissing


DSM_EVENT
HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetachOnPDOCleaupWithPDOMissing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetachOnPDOCleaupWithPDOMissing is called when the
    state machine enters the DisablingDeviceAndDefaultEpInControllerOnDetachOnPDOCleaupWithPDOMissing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetachOnPDOCleaupWithPDOMissing


DSM_EVENT
HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspend is called when the
    state machine enters the DisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspend


DSM_EVENT
HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspendOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspendOnDetach is called when the
    state machine enters the DisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspendOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspendOnDetach


DSM_EVENT
HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopOnDetach is called when the
    state machine enters the DisablingDeviceAndDefaultEpInControllerOnHubStopOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopOnDetach


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnCleanup is called when the
    state machine enters the DisablingDeviceInControllerOnCleanup State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnCleanup


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnDetachOnCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnDetachOnCleanup is called when the
    state machine enters the DisablingDeviceInControllerOnDetachOnCleanup State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnDetachOnCleanup


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnEnumFailureOnPdoPreStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnEnumFailureOnPdoPreStart is called when the
    state machine enters the DisablingDeviceInControllerOnEnumFailureOnPdoPreStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnEnumFailureOnPdoPreStart


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnEnumFailureOnRestart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnEnumFailureOnRestart is called when the
    state machine enters the DisablingDeviceInControllerOnEnumFailureOnRestart State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnEnumFailureOnRestart


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnHubStopFromStoppedEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnHubStopFromStoppedEnabled is called when the
    state machine enters the DisablingDeviceInControllerOnHubStopFromStoppedEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnHubStopFromStoppedEnabled


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnHubStopInStopped(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnHubStopInStopped is called when the
    state machine enters the DisablingDeviceInControllerOnHubStopInStopped State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnHubStopInStopped


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnHubStopInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnHubStopInSuspended is called when the
    state machine enters the DisablingDeviceInControllerOnHubStopInSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnHubStopInSuspended


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnPDOStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnPDOStop is called when the
    state machine enters the DisablingDeviceInControllerOnPDOStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnPDOStop


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnRenumerationWhileSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnRenumerationWhileSuspended is called when the
    state machine enters the DisablingDeviceInControllerOnRenumerationWhileSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnRenumerationWhileSuspended


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnStartTimeout(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnStartTimeout is called when the
    state machine enters the DisablingDeviceInControllerOnStartTimeout State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnStartTimeout


DSM_EVENT
HUBDSM_DisablingEndpointsInControllerOHubStopAfterSuspendOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsInControllerOHubStopAfterSuspendOnDetach is called when the
    state machine enters the DisablingEndpointsInControllerOHubStopAfterSuspendOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsInControllerOHubStopAfterSuspendOnDetach


DSM_EVENT
HUBDSM_DisablingEndpointsInControllerOHubStopOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsInControllerOHubStopOnDetach is called when the
    state machine enters the DisablingEndpointsInControllerOHubStopOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsInControllerOHubStopOnDetach


DSM_EVENT
HUBDSM_DisablingEndpointsInControllerOnCleanupWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsInControllerOnCleanupWithPortOff is called when the
    state machine enters the DisablingEndpointsInControllerOnCleanupWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsInControllerOnCleanupWithPortOff


DSM_EVENT
HUBDSM_DisablingEndpointsInControllerOnClientRequest(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsInControllerOnClientRequest is called when the
    state machine enters the DisablingEndpointsInControllerOnClientRequest State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsInControllerOnClientRequest


DSM_EVENT
HUBDSM_DisablingEndpointsInControllerOnD0ExitOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsInControllerOnD0ExitOnDetach is called when the
    state machine enters the DisablingEndpointsInControllerOnD0ExitOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsInControllerOnD0ExitOnDetach


DSM_EVENT
HUBDSM_DisablingEndpointsInControllerOnD0ExitOnDetachWithPDOMissing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsInControllerOnD0ExitOnDetachWithPDOMissing is called when the
    state machine enters the DisablingEndpointsInControllerOnD0ExitOnDetachWithPDOMissing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsInControllerOnD0ExitOnDetachWithPDOMissing


DSM_EVENT
HUBDSM_DisablingEndpointsInControllerOnDetachOnHubStopWithPDOMissing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsInControllerOnDetachOnHubStopWithPDOMissing is called when the
    state machine enters the DisablingEndpointsInControllerOnDetachOnHubStopWithPDOMissing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsInControllerOnDetachOnHubStopWithPDOMissing


DSM_EVENT
HUBDSM_DisablingEndpointsInControllerOnDetachOnPDOCleanupWithPDOMissing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsInControllerOnDetachOnPDOCleanupWithPDOMissing is called when the
    state machine enters the DisablingEndpointsInControllerOnDetachOnPDOCleanupWithPDOMissing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsInControllerOnDetachOnPDOCleanupWithPDOMissing


DSM_EVENT
HUBDSM_DisablingEndpointsInControllerOnFailureWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsInControllerOnFailureWithPortOff is called when the
    state machine enters the DisablingEndpointsInControllerOnFailureWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsInControllerOnFailureWithPortOff


DSM_EVENT
HUBDSM_DisablingEndpointsInControllerOnHubStopAfterSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsInControllerOnHubStopAfterSuspend is called when the
    state machine enters the DisablingEndpointsInControllerOnHubStopAfterSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsInControllerOnHubStopAfterSuspend


DSM_EVENT
HUBDSM_DisablingEndpointsInControllerOnRenumerationWhileSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsInControllerOnRenumerationWhileSuspended is called when the
    state machine enters the DisablingEndpointsInControllerOnRenumerationWhileSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsInControllerOnRenumerationWhileSuspended


DSM_EVENT
HUBDSM_DisablingEndpointsInControllerOnStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsInControllerOnStop is called when the
    state machine enters the DisablingEndpointsInControllerOnStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsInControllerOnStop


DSM_EVENT
HUBDSM_DisablingEndpointsOnDetachWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsOnDetachWithSpecialFile is called when the
    state machine enters the DisablingEndpointsOnDetachWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

        HUBPDO_SetDeviceLostPowerFlag(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsOnDetachWithSpecialFile


DSM_EVENT
HUBDSM_DisablingEndpointsOnDetachWithSpecialFileAndResetPending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsOnDetachWithSpecialFileAndResetPending is called when the
    state machine enters the DisablingEndpointsOnDetachWithSpecialFileAndResetPending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

        HUBPDO_NotifyDeviceDisconnected(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsOnDetachWithSpecialFileAndResetPending


DSM_EVENT
HUBDSM_DisablingEndpointsOnFailureWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsOnFailureWithSpecialFile is called when the
    state machine enters the DisablingEndpointsOnFailureWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

        HUBPDO_NotifyDeviceDisconnected(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsOnFailureWithSpecialFile


DSM_EVENT
HUBDSM_DisablingPortOnFailureInUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingPortOnFailureInUnConfigured is called when the
    state machine enters the DisablingPortOnFailureInUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingPortOnFailureInUnConfigured


DSM_EVENT
HUBDSM_DisablingPortOnHubStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingPortOnHubStop is called when the
    state machine enters the DisablingPortOnHubStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingPortOnHubStop


DSM_EVENT
HUBDSM_DisablingPortOnHubStopFromStoppedEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingPortOnHubStopFromStoppedEnabled is called when the
    state machine enters the DisablingPortOnHubStopFromStoppedEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DisablingPortOnHubStopFromStoppedEnabled


DSM_EVENT
HUBDSM_DisablingPortOnPdoCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingPortOnPdoCleanup is called when the
    state machine enters the DisablingPortOnPdoCleanup State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingPortOnPdoCleanup


DSM_EVENT
HUBDSM_DisablingPortOnPDOStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingPortOnPDOStop is called when the
    state machine enters the DisablingPortOnPDOStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingPortOnPDOStop


DSM_EVENT
HUBDSM_DisablingPortOnStartTimeout(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingPortOnStartTimeout is called when the
    state machine enters the DisablingPortOnStartTimeout State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingPortOnStartTimeout


DSM_EVENT
HUBDSM_EnumeratedAsFailedUnknown(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_EnumeratedAsFailedUnknown is called when the
    state machine enters the EnumeratedAsFailedUnknown State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_EnumeratedAsFailedUnknown


DSM_EVENT
HUBDSM_Enumerating(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_Enumerating is called when the
    state machine enters the Enumerating State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_Enumerating


DSM_EVENT
HUBDSM_FlushingPD0D0ExitFinalInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingPD0D0ExitFinalInConfigured is called when the
    state machine enters the FlushingPD0D0ExitFinalInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingPD0D0ExitFinalInConfigured


DSM_EVENT
HUBDSM_FlushingPD0D0ExitFinalInUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingPD0D0ExitFinalInUnConfigured is called when the
    state machine enters the FlushingPD0D0ExitFinalInUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingPD0D0ExitFinalInUnConfigured


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInConfigured is called when the
    state machine enters the FowardingStreamsRequestToUcxInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInConfigured


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInConfiguredSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInConfiguredSuspended is called when the
    state machine enters the FowardingStreamsRequestToUcxInConfiguredSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInConfiguredSuspended


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInConfiguredSuspendedWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInConfiguredSuspendedWithSpecialFile is called when the
    state machine enters the FowardingStreamsRequestToUcxInConfiguredSuspendedWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInConfiguredSuspendedWithSpecialFile


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInDeviceDetachedAndDeporgrammedWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInDeviceDetachedAndDeporgrammedWithSpecialFile is called when the
    state machine enters the FowardingStreamsRequestToUcxInDeviceDetachedAndDeporgrammedWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInDeviceDetachedAndDeporgrammedWithSpecialFile


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInDeviceDetachedWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInDeviceDetachedWithSpecialFile is called when the
    state machine enters the FowardingStreamsRequestToUcxInDeviceDetachedWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInDeviceDetachedWithSpecialFile


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInStopEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInStopEnabled is called when the
    state machine enters the FowardingStreamsRequestToUcxInStopEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInStopEnabled


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInStopped(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInStopped is called when the
    state machine enters the FowardingStreamsRequestToUcxInStopped State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInStopped


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInStoppedWithHubSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInStoppedWithHubSuspended is called when the
    state machine enters the FowardingStreamsRequestToUcxInStoppedWithHubSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInStoppedWithHubSuspended


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInUnconfiguredInD0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInUnconfiguredInD0 is called when the
    state machine enters the FowardingStreamsRequestToUcxInUnconfiguredInD0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInUnconfiguredInD0


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInUnconfiguredSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInUnconfiguredSuspended is called when the
    state machine enters the FowardingStreamsRequestToUcxInUnconfiguredSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInUnconfiguredSuspended


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInWaitingForDetachorResetWithDeviceDeprogrammed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInWaitingForDetachorResetWithDeviceDeprogrammed is called when the
    state machine enters the FowardingStreamsRequestToUcxInWaitingForDetachorResetWithDeviceDeprogrammed State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInWaitingForDetachorResetWithDeviceDeprogrammed


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInWaitingForDetatchorReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInWaitingForDetatchorReset is called when the
    state machine enters the FowardingStreamsRequestToUcxInWaitingForDetatchorReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInWaitingForDetatchorReset


DSM_EVENT
HUBDSM_GettingMatchingPipeHandleOnClientClearStall(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingMatchingPipeHandleOnClientClearStall is called when the
    state machine enters the GettingMatchingPipeHandleOnClientClearStall State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_FindAndStoreMatchingInternalPipeHandle(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_GettingMatchingPipeHandleOnClientClearStall


DSM_EVENT
HUBDSM_GettingMatchingPipeHandleOnClientResetPipe(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingMatchingPipeHandleOnClientResetPipe is called when the
    state machine enters the GettingMatchingPipeHandleOnClientResetPipe State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_FindAndStoreMatchingInternalPipeHandle(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_GettingMatchingPipeHandleOnClientResetPipe


DSM_EVENT
HUBDSM_GettingMatchingPipeHandleOnClientSyncResetPipe(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingMatchingPipeHandleOnClientSyncResetPipe is called when the
    state machine enters the GettingMatchingPipeHandleOnClientSyncResetPipe State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_FindAndStoreMatchingInternalPipeHandle(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_GettingMatchingPipeHandleOnClientSyncResetPipe


DSM_EVENT
HUBDSM_IssuingDeviceDetachedToPSMOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_IssuingDeviceDetachedToPSMOnDetach is called when the
    state machine enters the IssuingDeviceDetachedToPSMOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceDetached);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_IssuingDeviceDetachedToPSMOnDetach


DSM_EVENT
HUBDSM_IssuingPortCycleAndCompletingClientRequestOnEnumFailue(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_IssuingPortCycleAndCompletingClientRequestOnEnumFailue is called when the
    state machine enters the IssuingPortCycleAndCompletingClientRequestOnEnumFailue State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteClientSerialRequestWithStatusFailed(deviceContext);

        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestCycle);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_IssuingPortCycleAndCompletingClientRequestOnEnumFailue


DSM_EVENT
HUBDSM_IssuingPortCycleAndSingalingPnpEventOnEnumFailueFromConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_IssuingPortCycleAndSingalingPnpEventOnEnumFailueFromConfigured is called when the
    state machine enters the IssuingPortCycleAndSingalingPnpEventOnEnumFailueFromConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestCycle);

        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_IssuingPortCycleAndSingalingPnpEventOnEnumFailueFromConfigured


DSM_EVENT
HUBDSM_IssuingPortCycleOnEnumFailureOnHubResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_IssuingPortCycleOnEnumFailureOnHubResume is called when the
    state machine enters the IssuingPortCycleOnEnumFailureOnHubResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestCycle);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_IssuingPortCycleOnEnumFailureOnHubResume


DSM_EVENT
HUBDSM_IssuingPortCycleOnFailueWithEndpointsToBeDeleted(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_IssuingPortCycleOnFailueWithEndpointsToBeDeleted is called when the
    state machine enters the IssuingPortCycleOnFailueWithEndpointsToBeDeleted State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestCycle);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_IssuingPortCycleOnFailueWithEndpointsToBeDeleted


DSM_EVENT
HUBDSM_NotifyingDeviceDisconnectedOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_NotifyingDeviceDisconnectedOnDetach is called when the
    state machine enters the NotifyingDeviceDisconnectedOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_NotifyDeviceDisconnected(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_NotifyingDeviceDisconnectedOnDetach


DSM_EVENT
HUBDSM_NotifyingDeviceDisconnectedOnDetachWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_NotifyingDeviceDisconnectedOnDetachWithSpecialFile is called when the
    state machine enters the NotifyingDeviceDisconnectedOnDetachWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_NotifyDeviceDisconnected(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_NotifyingDeviceDisconnectedOnDetachWithSpecialFile


DSM_EVENT
HUBDSM_PreparingEndpointAndInterfaceListsOnClientReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PreparingEndpointAndInterfaceListsOnClientReset is called when the
    state machine enters the PreparingEndpointAndInterfaceListsOnClientReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_PrepareEndpointAndInterfaceListsForConfiguringDeviceOnReset(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_PreparingEndpointAndInterfaceListsOnClientReset


DSM_EVENT
HUBDSM_PreparingEndpointAndInterfaceListsOnClientResetWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PreparingEndpointAndInterfaceListsOnClientResetWithSpecialFile is called when the
    state machine enters the PreparingEndpointAndInterfaceListsOnClientResetWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_PrepareEndpointAndInterfaceListsForConfiguringDeviceOnReset(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_PreparingEndpointAndInterfaceListsOnClientResetWithSpecialFile


DSM_EVENT
HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterHubResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterHubResume is called when the
    state machine enters the PreparingEndpointAndInterfaceListsOnResetAfterHubResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_PrepareEndpointAndInterfaceListsForConfiguringDeviceOnReset(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterHubResume


DSM_EVENT
HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterResume is called when the
    state machine enters the PreparingEndpointAndInterfaceListsOnResetAfterResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_PrepareEndpointAndInterfaceListsForConfiguringDeviceOnReset(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterResume


DSM_EVENT
HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterResumeWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterResumeWithSpecialFile is called when the
    state machine enters the PreparingEndpointAndInterfaceListsOnResetAfterResumeWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_PrepareEndpointAndInterfaceListsForConfiguringDeviceOnReset(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterResumeWithSpecialFile


DSM_EVENT
HUBDSM_PreparingEndpointAndInterfaceListsOnSelectConfiguration(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PreparingEndpointAndInterfaceListsOnSelectConfiguration is called when the
    state machine enters the PreparingEndpointAndInterfaceListsOnSelectConfiguration State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_PrepareEndpointAndInterfaceListsForConfiguringDeviceOnSelectConfiguration(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_PreparingEndpointAndInterfaceListsOnSelectConfiguration


DSM_EVENT
HUBDSM_PreparingEndpointAndInterfaceListsOnSelectConfigurationFromConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PreparingEndpointAndInterfaceListsOnSelectConfigurationFromConfigured is called when the
    state machine enters the PreparingEndpointAndInterfaceListsOnSelectConfigurationFromConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_PrepareEndpointAndInterfaceListsForConfiguringDeviceOnSelectConfiguration(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_PreparingEndpointAndInterfaceListsOnSelectConfigurationFromConfigured


DSM_EVENT
HUBDSM_PurgingDeviceIoOnDetachInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PurgingDeviceIoOnDetachInConfigured is called when the
    state machine enters the PurgingDeviceIoOnDetachInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_NotifyDeviceDisconnected(deviceContext);

        HUBUCX_PurgeDeviceIoUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_PurgingDeviceIoOnDetachInConfigured


DSM_EVENT
HUBDSM_PurgingDeviceIoOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PurgingDeviceIoOnHubSuspend is called when the
    state machine enters the PurgingDeviceIoOnHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_PurgeDeviceIoForSuspendUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_PurgingDeviceIoOnHubSuspend


DSM_EVENT
HUBDSM_PurgingDeviceIoOnHubSuspendFromSuspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PurgingDeviceIoOnHubSuspendFromSuspending is called when the
    state machine enters the PurgingDeviceIoOnHubSuspendFromSuspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_PurgeDeviceIoForSuspendUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_PurgingDeviceIoOnHubSuspendFromSuspending


DSM_EVENT
HUBDSM_PurgingDeviceIoOnPrepareForHibernate(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PurgingDeviceIoOnPrepareForHibernate is called when the
    state machine enters the PurgingDeviceIoOnPrepareForHibernate State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_PurgeDeviceIoForSuspendUsingUCXIoctl(deviceContext);

        HUBPDO_ClearResetOnLastResumeFlag(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_PurgingDeviceIoOnPrepareForHibernate


DSM_EVENT
HUBDSM_QueryingDescriptorForFdoRequestFromConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueryingDescriptorForFdoRequestFromConfigured is called when the
    state machine enters the QueryingDescriptorForFdoRequestFromConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetDescriptorForFdoRequestUsingPreformattedUrb(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_QueryingDescriptorForFdoRequestFromConfigured


DSM_EVENT
HUBDSM_QueryingDescriptorForFdoRequestFromStoppedEnumerated(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueryingDescriptorForFdoRequestFromStoppedEnumerated is called when the
    state machine enters the QueryingDescriptorForFdoRequestFromStoppedEnumerated State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetDescriptorForFdoRequestUsingPreformattedUrb(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_QueryingDescriptorForFdoRequestFromStoppedEnumerated


DSM_EVENT
HUBDSM_QueryingDescriptorForFdoRequestFromUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueryingDescriptorForFdoRequestFromUnConfigured is called when the
    state machine enters the QueryingDescriptorForFdoRequestFromUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetDescriptorForFdoRequestUsingPreformattedUrb(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_QueryingDescriptorForFdoRequestFromUnConfigured


DSM_EVENT
HUBDSM_QueryingDeviceTextStringFromConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueryingDeviceTextStringFromConfigured is called when the
    state machine enters the QueryingDeviceTextStringFromConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_QueryingDeviceTextStringFromConfigured


DSM_EVENT
HUBDSM_QueryingDeviceTextStringFromStoppedEnumerated(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueryingDeviceTextStringFromStoppedEnumerated is called when the
    state machine enters the QueryingDeviceTextStringFromStoppedEnumerated State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_QueryingDeviceTextStringFromStoppedEnumerated


DSM_EVENT
HUBDSM_QueryingDeviceTextStringFromUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueryingDeviceTextStringFromUnConfigured is called when the
    state machine enters the QueryingDeviceTextStringFromUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_QueryingDeviceTextStringFromUnConfigured


DSM_EVENT
HUBDSM_QueueingAttachFailureToPort(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueueingAttachFailureToPort is called when the
    state machine enters the QueueingAttachFailureToPort State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_SetAttachStatusToFailure(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_QueueingAttachFailureToPort


DSM_EVENT
HUBDSM_QueueingAttachSuccessToPort(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueueingAttachSuccessToPort is called when the
    state machine enters the QueueingAttachSuccessToPort State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_AcquireWdfReferenceOnPort(deviceContext);

        HUBMISC_SetAttachStatusToSuccess(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_QueueingAttachSuccessToPort


DSM_EVENT
HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceInUnconfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceInUnconfigured is called when the
    state machine enters the QueueingDeviceDetachedToPsmForBootDeviceInUnconfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceDetached);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceInUnconfigured


DSM_EVENT
HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceWithResetPending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceWithResetPending is called when the
    state machine enters the QueueingDeviceDetachedToPsmForBootDeviceWithResetPending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceDetached);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceWithResetPending


DSM_EVENT
HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceWithResetPendingAndDeviceDeprogrammed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceWithResetPendingAndDeviceDeprogrammed is called when the
    state machine enters the QueueingDeviceDetachedToPsmForBootDeviceWithResetPendingAndDeviceDeprogrammed State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceDetached);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceWithResetPendingAndDeviceDeprogrammed


DSM_EVENT
HUBDSM_QueueingPrepareForHiberToPSM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueueingPrepareForHiberToPSM is called when the
    state machine enters the QueueingPrepareForHiberToPSM State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDevicePrepareForHibernation);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_QueueingPrepareForHiberToPSM


DSM_EVENT
HUBDSM_ReDisablingPortAfterHubResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReDisablingPortAfterHubResume is called when the
    state machine enters the ReDisablingPortAfterHubResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ReDisablingPortAfterHubResume


DSM_EVENT
HUBDSM_ReEnumeratingOnClientRequestInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReEnumeratingOnClientRequestInConfigured is called when the
    state machine enters the ReEnumeratingOnClientRequestInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ReEnumeratingOnClientRequestInConfigured


DSM_EVENT
HUBDSM_ReEnumeratingOnClientRequestInConfiguredWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReEnumeratingOnClientRequestInConfiguredWithSpecialFile is called when the
    state machine enters the ReEnumeratingOnClientRequestInConfiguredWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ReEnumeratingOnClientRequestInConfiguredWithSpecialFile


DSM_EVENT
HUBDSM_ReEnumeratingOnClientRequestInUnconfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReEnumeratingOnClientRequestInUnconfigured is called when the
    state machine enters the ReEnumeratingOnClientRequestInUnconfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SetDeviceFlagToForceResetOnEnumeration(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ReEnumeratingOnClientRequestInUnconfigured


DSM_EVENT
HUBDSM_ReEnumeratingOnClientRequestOnReattach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReEnumeratingOnClientRequestOnReattach is called when the
    state machine enters the ReEnumeratingOnClientRequestOnReattach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ReEnumeratingOnClientRequestOnReattach


DSM_EVENT
HUBDSM_ReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed is called when the
    state machine enters the ReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed


DSM_EVENT
HUBDSM_ReEnumeratingOnHubResumeInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReEnumeratingOnHubResumeInConfigured is called when the
    state machine enters the ReEnumeratingOnHubResumeInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ReEnumeratingOnHubResumeInConfigured


DSM_EVENT
HUBDSM_ReEnumeratingOnHubResumeInUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReEnumeratingOnHubResumeInUnConfigured is called when the
    state machine enters the ReEnumeratingOnHubResumeInUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ReEnumeratingOnHubResumeInUnConfigured


DSM_EVENT
HUBDSM_ReEnumeratingOnHubStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReEnumeratingOnHubStart is called when the
    state machine enters the ReEnumeratingOnHubStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ReEnumeratingOnHubStart


DSM_EVENT
HUBDSM_ReEnumeratingOnPdoPreStartWhileEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReEnumeratingOnPdoPreStartWhileEnabled is called when the
    state machine enters the ReEnumeratingOnPdoPreStartWhileEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SetDeviceFlagToForceResetOnEnumeration(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ReEnumeratingOnPdoPreStartWhileEnabled


DSM_EVENT
HUBDSM_ReEnumeratingOnPdoStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReEnumeratingOnPdoStart is called when the
    state machine enters the ReEnumeratingOnPdoStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ReEnumeratingOnPdoStart


DSM_EVENT
HUBDSM_ReEnumeratingOnPdoStartWhileEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReEnumeratingOnPdoStartWhileEnabled is called when the
    state machine enters the ReEnumeratingOnPdoStartWhileEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ReEnumeratingOnPdoStartWhileEnabled


DSM_EVENT
HUBDSM_ReEnumeratingOnResumeInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReEnumeratingOnResumeInConfigured is called when the
    state machine enters the ReEnumeratingOnResumeInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ReEnumeratingOnResumeInConfigured


DSM_EVENT
HUBDSM_ReEnumeratingOnResumeInConfiguredWithSpecialFile(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReEnumeratingOnResumeInConfiguredWithSpecialFile is called when the
    state machine enters the ReEnumeratingOnResumeInConfiguredWithSpecialFile State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ReEnumeratingOnResumeInConfiguredWithSpecialFile


DSM_EVENT
HUBDSM_ReEnumeratingOnResumeInUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReEnumeratingOnResumeInUnConfigured is called when the
    state machine enters the ReEnumeratingOnResumeInUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ReEnumeratingOnResumeInUnConfigured


DSM_EVENT
HUBDSM_RegisteringWithHSM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_RegisteringWithHSM is called when the
    state machine enters the RegisteringWithHSM State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMUX_RegisterWithHSM(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_RegisteringWithHSM


DSM_EVENT
HUBDSM_ReleasingDevicePowerReferenceOnStopAfterDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingDevicePowerReferenceOnStopAfterDetach is called when the
    state machine enters the ReleasingDevicePowerReferenceOnStopAfterDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingDevicePowerReferenceOnStopAfterDetach


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnDetachInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnDetachInConfigured is called when the
    state machine enters the ReleasingPowerReferenceOnDetachInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnDetachInConfigured


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnHubOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnHubOnDetach is called when the
    state machine enters the ReleasingPowerReferenceOnHubOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_ReleaseWdfPowerReferenceOnHub(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnHubOnDetach


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnHubOnDetachOnEnumSuccess(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnHubOnDetachOnEnumSuccess is called when the
    state machine enters the ReleasingPowerReferenceOnHubOnDetachOnEnumSuccess State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_ReleaseWdfPowerReferenceOnHub(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnHubOnDetachOnEnumSuccess


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnHubOnEnumerationFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnHubOnEnumerationFailure is called when the
    state machine enters the ReleasingPowerReferenceOnHubOnEnumerationFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_ReleaseWdfPowerReferenceOnHub(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnHubOnEnumerationFailure


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnHubStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnHubStopSuspend is called when the
    state machine enters the ReleasingPowerReferenceOnHubStopSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnHubStopSuspend


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnHubSuspendStopOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnHubSuspendStopOnDetach is called when the
    state machine enters the ReleasingPowerReferenceOnHubSuspendStopOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnHubSuspendStopOnDetach


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnPortFailureInHubStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnPortFailureInHubStopSuspend is called when the
    state machine enters the ReleasingPowerReferenceOnPortFailureInHubStopSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnPortFailureInHubStopSuspend


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnSuspendFailureInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnSuspendFailureInConfigured is called when the
    state machine enters the ReleasingPowerReferenceOnSuspendFailureInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnSuspendFailureInConfigured


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnSuspendFailureInUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnSuspendFailureInUnConfigured is called when the
    state machine enters the ReleasingPowerReferenceOnSuspendFailureInUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnSuspendFailureInUnConfigured


DSM_EVENT
HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanup is called when the
    state machine enters the RemovingDeviceInfoFromGlobalListOnCleanup State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_RemoveDeviceInfoFromGlobalChildList(deviceContext);

        HUBMISC_ReleaseWdfReferenceOnPort(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanup


DSM_EVENT
HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanupOnDetachInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanupOnDetachInConfigured is called when the
    state machine enters the RemovingDeviceInfoFromGlobalListOnCleanupOnDetachInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_RemoveDeviceInfoFromGlobalChildList(deviceContext);

        HUBMISC_ReleaseWdfReferenceOnPort(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanupOnDetachInConfigured


DSM_EVENT
HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanupOnDetachInUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanupOnDetachInUnConfigured is called when the
    state machine enters the RemovingDeviceInfoFromGlobalListOnCleanupOnDetachInUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_RemoveDeviceInfoFromGlobalChildList(deviceContext);

        HUBMISC_ReleaseWdfReferenceOnPort(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanupOnDetachInUnConfigured


DSM_EVENT
HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanUpOnHubStopOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanUpOnHubStopOnDetach is called when the
    state machine enters the RemovingDeviceInfoFromGlobalListOnCleanUpOnHubStopOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_RemoveDeviceInfoFromGlobalChildList(deviceContext);

        HUBMISC_ReleaseWdfReferenceOnPort(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanUpOnHubStopOnDetach


DSM_EVENT
HUBDSM_RemovingDeviceInfoFromGlobalListOnDeviceReportedMissing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_RemovingDeviceInfoFromGlobalListOnDeviceReportedMissing is called when the
    state machine enters the RemovingDeviceInfoFromGlobalListOnDeviceReportedMissing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_RemoveDeviceInfoFromGlobalChildList(deviceContext);

        HUBMISC_ReleaseWdfReferenceOnPort(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_RemovingDeviceInfoFromGlobalListOnDeviceReportedMissing


DSM_EVENT
HUBDSM_RemovingDeviceInfoFromGlobalListOnReportedMissingOnHubStopOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_RemovingDeviceInfoFromGlobalListOnReportedMissingOnHubStopOnDetach is called when the
    state machine enters the RemovingDeviceInfoFromGlobalListOnReportedMissingOnHubStopOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_RemoveDeviceInfoFromGlobalChildList(deviceContext);

        HUBMISC_ReleaseWdfReferenceOnPort(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_RemovingDeviceInfoFromGlobalListOnReportedMissingOnHubStopOnDetach


DSM_EVENT
HUBDSM_ReportingDeviceMissing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReportingDeviceMissing is called when the
    state machine enters the ReportingDeviceMissing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_ReportDeviceAsMissing(deviceContext);

        HUBMISC_MarkDeviceAsDisconnectedInGlobalChildList(deviceContext);

        HUBPDO_NotifyDeviceDisconnected(deviceContext);

        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceDetached);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReportingDeviceMissing


DSM_EVENT
HUBDSM_ReportingDeviceToPnp(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReportingDeviceToPnp is called when the
    state machine enters the ReportingDeviceToPnp State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ReportingDeviceToPnp


DSM_EVENT
HUBDSM_ReportingFailedDeviceAsMissing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReportingFailedDeviceAsMissing is called when the
    state machine enters the ReportingFailedDeviceAsMissing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_ReportDeviceAsMissing(deviceContext);

        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceDetached);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ReportingFailedDeviceAsMissing


DSM_EVENT
HUBDSM_ReportingFailedUnknownDeviceToPnp(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReportingFailedUnknownDeviceToPnp is called when the
    state machine enters the ReportingFailedUnknownDeviceToPnp State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ReportingFailedUnknownDeviceToPnp


DSM_EVENT
HUBDSM_RequestingCyclePortAndCompletingPnpEventOnFailureInUnconfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_RequestingCyclePortAndCompletingPnpEventOnFailureInUnconfigured is called when the
    state machine enters the RequestingCyclePortAndCompletingPnpEventOnFailureInUnconfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestCycle);

        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_RequestingCyclePortAndCompletingPnpEventOnFailureInUnconfigured


DSM_EVENT
HUBDSM_RequestingPortCycleOnErrorFromConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_RequestingPortCycleOnErrorFromConfigured is called when the
    state machine enters the RequestingPortCycleOnErrorFromConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestCycle);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_RequestingPortCycleOnErrorFromConfigured


DSM_EVENT
HUBDSM_ResettingPipeInUCXOnClientReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ResettingPipeInUCXOnClientReset is called when the
    state machine enters the ResettingPipeInUCXOnClientReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ResetEndpointAndTransferStateUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ResettingPipeInUCXOnClientReset


DSM_EVENT
HUBDSM_ResettingPipeInUCXOnClientSyncResetPipe(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ResettingPipeInUCXOnClientSyncResetPipe is called when the
    state machine enters the ResettingPipeInUCXOnClientSyncResetPipe State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ResetEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ResettingPipeInUCXOnClientSyncResetPipe


DSM_EVENT
HUBDSM_ResumingOnHubResumeInStopped(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ResumingOnHubResumeInStopped is called when the
    state machine enters the ResumingOnHubResumeInStopped State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestResume);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ResumingOnHubResumeInStopped


DSM_EVENT
HUBDSM_SettingConfigurationInformationInClientRequest(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingConfigurationInformationInClientRequest is called when the
    state machine enters the SettingConfigurationInformationInClientRequest State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_SetConfigurationInformationInClientRequest(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SettingConfigurationInformationInClientRequest


DSM_EVENT
HUBDSM_SettingDeviceInterface(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingDeviceInterface is called when the
    state machine enters the SettingDeviceInterface State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_SettingDeviceInterface


DSM_EVENT
HUBDSM_SettingExitLatencyAdjustmentOnNoPingResponseError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingExitLatencyAdjustmentOnNoPingResponseError is called when the
    state machine enters the SettingExitLatencyAdjustmentOnNoPingResponseError State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_SetLatencyAdjustmentOnNoPingResponse(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SettingExitLatencyAdjustmentOnNoPingResponseError


DSM_EVENT
HUBDSM_SettingFailIoFlagOnClientReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingFailIoFlagOnClientReset is called when the
    state machine enters the SettingFailIoFlagOnClientReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_NotifyDeviceDisconnected(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SettingFailIoFlagOnClientReset


DSM_EVENT
HUBDSM_SettingForceResetOnRenumFlag(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingForceResetOnRenumFlag is called when the
    state machine enters the SettingForceResetOnRenumFlag State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SetDeviceFlagToForceResetOnEnumeration(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SettingForceResetOnRenumFlag


DSM_EVENT
HUBDSM_SettingInterfaceInformationInClientRequest(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingInterfaceInformationInClientRequest is called when the
    state machine enters the SettingInterfaceInformationInClientRequest State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_SetInterfaceInformationInClientRequest(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SettingInterfaceInformationInClientRequest


DSM_EVENT
HUBDSM_SettingLPMValuesInConfiguredOnSettingChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingLPMValuesInConfiguredOnSettingChange is called when the
    state machine enters the SettingLPMValuesInConfiguredOnSettingChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_SettingLPMValuesInConfiguredOnSettingChange


DSM_EVENT
HUBDSM_SettingLPMValuesInStoppedEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingLPMValuesInStoppedEnabled is called when the
    state machine enters the SettingLPMValuesInStoppedEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_SettingLPMValuesInStoppedEnabled


DSM_EVENT
HUBDSM_SettingLPMValuesInUnConfiguredOnSettingChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingLPMValuesInUnConfiguredOnSettingChange is called when the
    state machine enters the SettingLPMValuesInUnConfiguredOnSettingChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_SettingLPMValuesInUnConfiguredOnSettingChange


DSM_EVENT
HUBDSM_SignalingPnpPowerEventOnDetachDuringRenumOnRestart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SignalingPnpPowerEventOnDetachDuringRenumOnRestart is called when the
    state machine enters the SignalingPnpPowerEventOnDetachDuringRenumOnRestart State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SignalingPnpPowerEventOnDetachDuringRenumOnRestart


DSM_EVENT
HUBDSM_SignalingPnpPowerEventOnStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SignalingPnpPowerEventOnStop is called when the
    state machine enters the SignalingPnpPowerEventOnStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SignalingPnpPowerEventOnStop


DSM_EVENT
HUBDSM_SignalingPnpPowerEventOnStopEnable(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SignalingPnpPowerEventOnStopEnable is called when the
    state machine enters the SignalingPnpPowerEventOnStopEnable State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        HUBPDO_SetDeviceFlagToForceResetOnEnumeration(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SignalingPnpPowerEventOnStopEnable


DSM_EVENT
HUBDSM_SignallingPnpEventOnHubSuspendFromUnconfiguredSuspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SignallingPnpEventOnHubSuspendFromUnconfiguredSuspending is called when the
    state machine enters the SignallingPnpEventOnHubSuspendFromUnconfiguredSuspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SignallingPnpEventOnHubSuspendFromUnconfiguredSuspending


DSM_EVENT
HUBDSM_StartingDeviceIoOnHubResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StartingDeviceIoOnHubResume is called when the
    state machine enters the StartingDeviceIoOnHubResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_StartDeviceIoUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_StartingDeviceIoOnHubResume


DSM_EVENT
HUBDSM_Stopped(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_Stopped is called when the
    state machine enters the Stopped State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_Stopped


DSM_EVENT
HUBDSM_StoppedEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppedEnabled is called when the
    state machine enters the StoppedEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_StoppedEnabled


DSM_EVENT
HUBDSM_StoppedEnumerated(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppedEnumerated is called when the
    state machine enters the StoppedEnumerated State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_StartNoDriverFoundTimer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_StoppedEnumerated


DSM_EVENT
HUBDSM_StoppedEnumeratedConfiguredForFaultyClients(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppedEnumeratedConfiguredForFaultyClients is called when the
    state machine enters the StoppedEnumeratedConfiguredForFaultyClients State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_StoppedEnumeratedConfiguredForFaultyClients


DSM_EVENT
HUBDSM_StoppedEnumeratedWithHubSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppedEnumeratedWithHubSuspended is called when the
    state machine enters the StoppedEnumeratedWithHubSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_StoppedEnumeratedWithHubSuspended


DSM_EVENT
HUBDSM_StoppedWithHubSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppedWithHubSuspended is called when the
    state machine enters the StoppedWithHubSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_StoppedWithHubSuspended


DSM_EVENT
HUBDSM_SuspendingOnHubSuspendInStopped(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SuspendingOnHubSuspendInStopped is called when the
    state machine enters the SuspendingOnHubSuspendInStopped State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestSuspend);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SuspendingOnHubSuspendInStopped


DSM_EVENT
HUBDSM_UnConfiguredInD0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UnConfiguredInD0 is called when the
    state machine enters the UnConfiguredInD0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_UnConfiguredInD0


DSM_EVENT
HUBDSM_UnConfiguredResumingOnHubResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UnConfiguredResumingOnHubResume is called when the
    state machine enters the UnConfiguredResumingOnHubResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestResume);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_UnConfiguredResumingOnHubResume


DSM_EVENT
HUBDSM_UnconfiguredSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UnconfiguredSuspended is called when the
    state machine enters the UnconfiguredSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_UnconfiguredSuspended


DSM_EVENT
HUBDSM_UnConfiguredSuspendedOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UnConfiguredSuspendedOnHubSuspend is called when the
    state machine enters the UnConfiguredSuspendedOnHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_UnConfiguredSuspendedOnHubSuspend


DSM_EVENT
HUBDSM_UnconfiguredSuspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UnconfiguredSuspending is called when the
    state machine enters the UnconfiguredSuspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_ClearResetOnLastResumeFlag(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_UnconfiguredSuspending


DSM_EVENT
HUBDSM_UnConfiguredSuspendingOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UnConfiguredSuspendingOnHubSuspend is called when the
    state machine enters the UnConfiguredSuspendingOnHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestSuspend);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_UnConfiguredSuspendingOnHubSuspend


DSM_EVENT
HUBDSM_UnregisteringWithHSMOnDetachAfterCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UnregisteringWithHSMOnDetachAfterCleanup is called when the
    state machine enters the UnregisteringWithHSMOnDetachAfterCleanup State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_UnregisterWithHSM(deviceContext);

        HUBMISC_ReleaseWdfReferenceOnPort(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_UnregisteringWithHSMOnDetachAfterCleanup


DSM_EVENT
HUBDSM_UnregisteringWithHsmOnDetachOnDeviceReportedMissing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UnregisteringWithHsmOnDetachOnDeviceReportedMissing is called when the
    state machine enters the UnregisteringWithHsmOnDetachOnDeviceReportedMissing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_UnregisterWithHSM(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_UnregisteringWithHsmOnDetachOnDeviceReportedMissing


DSM_EVENT
HUBDSM_UnregsiteringWithHsmOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UnregsiteringWithHsmOnDetach is called when the
    state machine enters the UnregsiteringWithHsmOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_UnregisterWithHSM(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_UnregsiteringWithHsmOnDetach


DSM_EVENT
HUBDSM_UnregsiteringWithHSMOnDetachOnPDOCleaupWithPDOMissing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UnregsiteringWithHSMOnDetachOnPDOCleaupWithPDOMissing is called when the
    state machine enters the UnregsiteringWithHSMOnDetachOnPDOCleaupWithPDOMissing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_UnregisterWithHSM(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_UnregsiteringWithHSMOnDetachOnPDOCleaupWithPDOMissing


DSM_EVENT
HUBDSM_ValidatingSelectConfigUrbFromConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingSelectConfigUrbFromConfigured is called when the
    state machine enters the ValidatingSelectConfigUrbFromConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_ValidateSelectConfigUrb(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingSelectConfigUrbFromConfigured


DSM_EVENT
HUBDSM_ValidatingSelectConfigUrbFromUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingSelectConfigUrbFromUnConfigured is called when the
    state machine enters the ValidatingSelectConfigUrbFromUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_ValidateSelectConfigUrb(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingSelectConfigUrbFromUnConfigured


DSM_EVENT
HUBDSM_ValidatingSelectInterfaceUrbFromConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingSelectInterfaceUrbFromConfigured is called when the
    state machine enters the ValidatingSelectInterfaceUrbFromConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_ValidateSelectInterfaceUrb(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingSelectInterfaceUrbFromConfigured


DSM_EVENT
HUBDSM_WaitingForD0EntryForFailedDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForD0EntryForFailedDevice is called when the
    state machine enters the WaitingForD0EntryForFailedDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForD0EntryForFailedDevice


DSM_EVENT
HUBDSM_WaitingForDetachOrRemove(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDetachOrRemove is called when the
    state machine enters the WaitingForDetachOrRemove State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDetachOrRemove


DSM_EVENT
HUBDSM_WaitingForDetachOrRemoveInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDetachOrRemoveInConfigured is called when the
    state machine enters the WaitingForDetachOrRemoveInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDetachOrRemoveInConfigured


DSM_EVENT
HUBDSM_WaitingForDetachOrRemoveInDisabledWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDetachOrRemoveInDisabledWithPortOff is called when the
    state machine enters the WaitingForDetachOrRemoveInDisabledWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDetachOrRemoveInDisabledWithPortOff


DSM_EVENT
HUBDSM_WaitingForDetachOrRemoveInUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDetachOrRemoveInUnConfigured is called when the
    state machine enters the WaitingForDetachOrRemoveInUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDetachOrRemoveInUnConfigured


DSM_EVENT
HUBDSM_WaitingForDetachOrRemoveWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDetachOrRemoveWithPortOff is called when the
    state machine enters the WaitingForDetachOrRemoveWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDetachOrRemoveWithPortOff


DSM_EVENT
HUBDSM_WaitingForDetachOrReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDetachOrReset is called when the
    state machine enters the WaitingForDetachOrReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDetachOrReset


DSM_EVENT
HUBDSM_WaitingForDetachOrResetWithDeviceDeporgrammed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDetachOrResetWithDeviceDeporgrammed is called when the
    state machine enters the WaitingForDetachOrResetWithDeviceDeporgrammed State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDetachOrResetWithDeviceDeporgrammed


DSM_EVENT
HUBDSM_WaitingForDetachOrToBeDeleted(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDetachOrToBeDeleted is called when the
    state machine enters the WaitingForDetachOrToBeDeleted State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDetachOrToBeDeleted


DSM_EVENT
HUBDSM_WaitingForDetachOrToBeDeletedWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDetachOrToBeDeletedWithPortOff is called when the
    state machine enters the WaitingForDetachOrToBeDeletedWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDetachOrToBeDeletedWithPortOff


DSM_EVENT
HUBDSM_WaitingForDisableCompleteOnHubStopFromStoppedEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDisableCompleteOnHubStopFromStoppedEnabled is called when the
    state machine enters the WaitingForDisableCompleteOnHubStopFromStoppedEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDisableCompleteOnHubStopFromStoppedEnabled


DSM_EVENT
HUBDSM_WaitingForPDOD0ExitOnDetachInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPDOD0ExitOnDetachInConfigured is called when the
    state machine enters the WaitingForPDOD0ExitOnDetachInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_RemoveDeviceInfoFromGlobalChildList(deviceContext);

        HUBMISC_ReleaseWdfReferenceOnPort(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForPDOD0ExitOnDetachInConfigured


DSM_EVENT
HUBDSM_WaitingForPDOD0ExitOnDetachInUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPDOD0ExitOnDetachInUnConfigured is called when the
    state machine enters the WaitingForPDOD0ExitOnDetachInUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_RemoveDeviceInfoFromGlobalChildList(deviceContext);

        HUBMISC_ReleaseWdfReferenceOnPort(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForPDOD0ExitOnDetachInUnConfigured


DSM_EVENT
HUBDSM_WaitingForPDOReportedMissing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPDOReportedMissing is called when the
    state machine enters the WaitingForPDOReportedMissing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPDOReportedMissing


DSM_EVENT
HUBDSM_WaitingForPortAttach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPortAttach is called when the
    state machine enters the WaitingForPortAttach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPortAttach


DSM_EVENT
HUBDSM_WaitingForPortResumeResponseOnStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPortResumeResponseOnStop is called when the
    state machine enters the WaitingForPortResumeResponseOnStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPortResumeResponseOnStop


DSM_EVENT
HUBDSM_WaitingForPortResumeResponseOnSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPortResumeResponseOnSuspend is called when the
    state machine enters the WaitingForPortResumeResponseOnSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPortResumeResponseOnSuspend


DSM_EVENT
HUBDSM_WaitingForRemoveOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForRemoveOnDetach is called when the
    state machine enters the WaitingForRemoveOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForRemoveOnDetach


DSM_EVENT
HUBDSM_WaitingForRemoveOrReprotedMissingOnHubStopOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForRemoveOrReprotedMissingOnHubStopOnDetach is called when the
    state machine enters the WaitingForRemoveOrReprotedMissingOnHubStopOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForRemoveOrReprotedMissingOnHubStopOnDetach


DSM_EVENT
HUBDSM_WaitingForReportedMissingOnDetachInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForReportedMissingOnDetachInConfigured is called when the
    state machine enters the WaitingForReportedMissingOnDetachInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_ReportDeviceAsMissing(deviceContext);

        HUBMISC_MarkDeviceAsDisconnectedInGlobalChildList(deviceContext);

        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceDetached);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForReportedMissingOnDetachInConfigured


DSM_EVENT
HUBDSM_WaitingForReportedMissingOnDetachInUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForReportedMissingOnDetachInUnConfigured is called when the
    state machine enters the WaitingForReportedMissingOnDetachInUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_ReportDeviceAsMissing(deviceContext);

        HUBMISC_MarkDeviceAsDisconnectedInGlobalChildList(deviceContext);

        HUBPDO_NotifyDeviceDisconnected(deviceContext);

        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceDetached);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForReportedMissingOnDetachInUnConfigured


DSM_EVENT
HUBDSM_WaitingToBeDeleted(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingToBeDeleted is called when the
    state machine enters the WaitingToBeDeleted State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingToBeDeleted


DSM_EVENT
HUBDSM_QueryingLanguageSpecificProductIdString(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueryingLanguageSpecificProductIdString is called when the
    state machine enters the QueryingLanguageSpecificProductIdString State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetProductIdStringDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_QueryingLanguageSpecificProductIdString


DSM_EVENT
HUBDSM_ReturningOperationSuccessInQueryingProductId(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInQueryingProductId is called when the
    state machine enters the ReturningOperationSuccessInQueryingProductId State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInQueryingProductId


DSM_EVENT
HUBDSM_SignalingDeviceQueryTextEvent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SignalingDeviceQueryTextEvent is called when the
    state machine enters the SignalingDeviceQueryTextEvent State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalQueryDeviceTextEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SignalingDeviceQueryTextEvent


DSM_EVENT
HUBDSM_ValidatingLanguageSpecificProductIdString(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingLanguageSpecificProductIdString is called when the
    state machine enters the ValidatingLanguageSpecificProductIdString State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheProductIdStringDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingLanguageSpecificProductIdString


DSM_EVENT
HUBDSM_AcquiringAddressZeroOwnershipInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AcquiringAddressZeroOwnershipInEnum is called when the
    state machine enters the AcquiringAddressZeroOwnershipInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_AcquireAddress0OwnershipUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_AcquiringAddressZeroOwnershipInEnum


DSM_EVENT
HUBDSM_AcquiringPowerReferenceOnStartInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AcquiringPowerReferenceOnStartInEnum is called when the
    state machine enters the AcquiringPowerReferenceOnStartInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AcquiringPowerReferenceOnStartInEnum


DSM_EVENT
HUBDSM_CancellingAcquiringAddress0OnDetachInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CancellingAcquiringAddress0OnDetachInEnum is called when the
    state machine enters the CancellingAcquiringAddress0OnDetachInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_CancelUCXIoctl(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CancellingAcquiringAddress0OnDetachInEnum


DSM_EVENT
HUBDSM_CancellingRetryTimerOnDetachInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CancellingRetryTimerOnDetachInEnum is called when the
    state machine enters the CancellingRetryTimerOnDetachInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CancellingRetryTimerOnDetachInEnum


DSM_EVENT
HUBDSM_CancellingRetryTimerOnStopSuspendInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CancellingRetryTimerOnStopSuspendInEnum is called when the
    state machine enters the CancellingRetryTimerOnStopSuspendInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CancellingRetryTimerOnStopSuspendInEnum


DSM_EVENT
HUBDSM_CheckingIfDevicePDChargingPolicySupported(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDevicePDChargingPolicySupported is called when the
    state machine enters the CheckingIfDevicePDChargingPolicySupported State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfDevicePDChargingPolicySupported(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDevicePDChargingPolicySupported


DSM_EVENT
HUBDSM_CheckingIfEnumRetryReachedMaximumInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfEnumRetryReachedMaximumInEnum is called when the
    state machine enters the CheckingIfEnumRetryReachedMaximumInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IncrementEnumRetryCountAndCheckIfReachedMax(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfEnumRetryReachedMaximumInEnum


DSM_EVENT
HUBDSM_CheckingIfEnumRetryReachedMaximumInEnumWithAddr0Ownership(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfEnumRetryReachedMaximumInEnumWithAddr0Ownership is called when the
    state machine enters the CheckingIfEnumRetryReachedMaximumInEnumWithAddr0Ownership State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IncrementEnumRetryCountAndCheckIfReachedMax(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfEnumRetryReachedMaximumInEnumWithAddr0Ownership


DSM_EVENT
HUBDSM_ClearingDeviceProgrammingLostFlagOnEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ClearingDeviceProgrammingLostFlagOnEnum is called when the
    state machine enters the ClearingDeviceProgrammingLostFlagOnEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfDeviceProgrammingWasLost(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ClearingDeviceProgrammingLostFlagOnEnum


DSM_EVENT
HUBDSM_DeletingDefaultEndpointAndDeviceFromUCXOnDetachInEnumeratingAfterAddr(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingDefaultEndpointAndDeviceFromUCXOnDetachInEnumeratingAfterAddr is called when the
    state machine enters the DeletingDefaultEndpointAndDeviceFromUCXOnDetachInEnumeratingAfterAddr State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingDefaultEndpointAndDeviceFromUCXOnDetachInEnumeratingAfterAddr


DSM_EVENT
HUBDSM_DeletingDefaultEndpointAndDeviceFromUCXOnFailureInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingDefaultEndpointAndDeviceFromUCXOnFailureInEnum is called when the
    state machine enters the DeletingDefaultEndpointAndDeviceFromUCXOnFailureInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingDefaultEndpointAndDeviceFromUCXOnFailureInEnum


DSM_EVENT
HUBDSM_DeletingDefaultEndpointAndDeviceOnPostAddressFailureInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingDefaultEndpointAndDeviceOnPostAddressFailureInEnum is called when the
    state machine enters the DeletingDefaultEndpointAndDeviceOnPostAddressFailureInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingDefaultEndpointAndDeviceOnPostAddressFailureInEnum


DSM_EVENT
HUBDSM_DeletingDefaultEndpointAndDeviceOnStopSuspendAfterAddressInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingDefaultEndpointAndDeviceOnStopSuspendAfterAddressInEnum is called when the
    state machine enters the DeletingDefaultEndpointAndDeviceOnStopSuspendAfterAddressInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingDefaultEndpointAndDeviceOnStopSuspendAfterAddressInEnum


DSM_EVENT
HUBDSM_DeletingDeviceAndDefaultEndpointOnDetachWithAddress0OwnershipInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingDeviceAndDefaultEndpointOnDetachWithAddress0OwnershipInEnum is called when the
    state machine enters the DeletingDeviceAndDefaultEndpointOnDetachWithAddress0OwnershipInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingDeviceAndDefaultEndpointOnDetachWithAddress0OwnershipInEnum


DSM_EVENT
HUBDSM_DeletingDeviceAndDefaultEndpointOnStopSuspendWithAddress0OwnershipInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingDeviceAndDefaultEndpointOnStopSuspendWithAddress0OwnershipInEnum is called when the
    state machine enters the DeletingDeviceAndDefaultEndpointOnStopSuspendWithAddress0OwnershipInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingDeviceAndDefaultEndpointOnStopSuspendWithAddress0OwnershipInEnum


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnDetachInEnumeratingAfterAddr(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnDetachInEnumeratingAfterAddr is called when the
    state machine enters the DisablingDeviceInControllerOnDetachInEnumeratingAfterAddr State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnDetachInEnumeratingAfterAddr


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnDetachWithAddress0OwnershipInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnDetachWithAddress0OwnershipInEnum is called when the
    state machine enters the DisablingDeviceInControllerOnDetachWithAddress0OwnershipInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnDetachWithAddress0OwnershipInEnum


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnPostAddressEnumFailureInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnPostAddressEnumFailureInEnum is called when the
    state machine enters the DisablingDeviceInControllerOnPostAddressEnumFailureInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnPostAddressEnumFailureInEnum


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnSetAddressFailureInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnSetAddressFailureInEnum is called when the
    state machine enters the DisablingDeviceInControllerOnSetAddressFailureInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnSetAddressFailureInEnum


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnStopSuspendAfterAddressInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnStopSuspendAfterAddressInEnum is called when the
    state machine enters the DisablingDeviceInControllerOnStopSuspendAfterAddressInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnStopSuspendAfterAddressInEnum


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnStopSuspendWithAddress0OwnershipInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnStopSuspendWithAddress0OwnershipInEnum is called when the
    state machine enters the DisablingDeviceInControllerOnStopSuspendWithAddress0OwnershipInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnStopSuspendWithAddress0OwnershipInEnum


DSM_EVENT
HUBDSM_DisablingOnEnumAfterFailureInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingOnEnumAfterFailureInEnum is called when the
    state machine enters the DisablingOnEnumAfterFailureInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingOnEnumAfterFailureInEnum


DSM_EVENT
HUBDSM_DisablingOnEnumAfterFailureInEnumWithAddress0Ownership(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingOnEnumAfterFailureInEnumWithAddress0Ownership is called when the
    state machine enters the DisablingOnEnumAfterFailureInEnumWithAddress0Ownership State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingOnEnumAfterFailureInEnumWithAddress0Ownership


DSM_EVENT
HUBDSM_DisablingOnStopSuspendInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingOnStopSuspendInEnum is called when the
    state machine enters the DisablingOnStopSuspendInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DisablingOnStopSuspendInEnum


DSM_EVENT
HUBDSM_DisablingOnStopSuspendInEnumWithAddress0Ownership(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingOnStopSuspendInEnumWithAddress0Ownership is called when the
    state machine enters the DisablingOnStopSuspendInEnumWithAddress0Ownership State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DisablingOnStopSuspendInEnumWithAddress0Ownership


DSM_EVENT
HUBDSM_DisablingOnStopSuspendOnIoctlFailureInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingOnStopSuspendOnIoctlFailureInEnum is called when the
    state machine enters the DisablingOnStopSuspendOnIoctlFailureInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingOnStopSuspendOnIoctlFailureInEnum


DSM_EVENT
HUBDSM_EnumeratingAfterAddressInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_EnumeratingAfterAddressInEnum is called when the
    state machine enters the EnumeratingAfterAddressInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_EnumeratingAfterAddressInEnum


DSM_EVENT
HUBDSM_EnumeratingAtAddressZeroInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_EnumeratingAtAddressZeroInEnum is called when the
    state machine enters the EnumeratingAtAddressZeroInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_EnumeratingAtAddressZeroInEnum


DSM_EVENT
HUBDSM_InitializingEnumRetryCountInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_InitializingEnumRetryCountInEnum is called when the
    state machine enters the InitializingEnumRetryCountInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_InitializeEnumRetryCount(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_InitializingEnumRetryCountInEnum


DSM_EVENT
HUBDSM_ReleasingAddressZeroOwnershipInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingAddressZeroOwnershipInEnum is called when the
    state machine enters the ReleasingAddressZeroOwnershipInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ReleaseAddress0OwnershipFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingAddressZeroOwnershipInEnum


DSM_EVENT
HUBDSM_ReleasingAddressZeroOwnershipOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingAddressZeroOwnershipOnDetach is called when the
    state machine enters the ReleasingAddressZeroOwnershipOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ReleaseAddress0OwnershipFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingAddressZeroOwnershipOnDetach


DSM_EVENT
HUBDSM_ReleasingAddressZeroOwnershipOnDetachInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingAddressZeroOwnershipOnDetachInEnum is called when the
    state machine enters the ReleasingAddressZeroOwnershipOnDetachInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ReleaseAddress0OwnershipFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingAddressZeroOwnershipOnDetachInEnum


DSM_EVENT
HUBDSM_ReleasingAddressZeroOwnershipOnEnumFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingAddressZeroOwnershipOnEnumFailure is called when the
    state machine enters the ReleasingAddressZeroOwnershipOnEnumFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ReleaseAddress0OwnershipFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingAddressZeroOwnershipOnEnumFailure


DSM_EVENT
HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspend is called when the
    state machine enters the ReleasingAddressZeroOwnershipOnStopSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ReleaseAddress0OwnershipFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspend


DSM_EVENT
HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendInEnum is called when the
    state machine enters the ReleasingAddressZeroOwnershipOnStopSuspendInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ReleaseAddress0OwnershipFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendInEnum


DSM_EVENT
HUBDSM_ReleasingPowerReferenceInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceInEnum is called when the
    state machine enters the ReleasingPowerReferenceInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceInEnum


DSM_EVENT
HUBDSM_ReturningOperationFailureInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureInEnum is called when the
    state machine enters the ReturningOperationFailureInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureInEnum


DSM_EVENT
HUBDSM_ReturningOperationSuccessInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInEnum is called when the
    state machine enters the ReturningOperationSuccessInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInEnum


DSM_EVENT
HUBDSM_ReturningPortDetachDeviceInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDetachDeviceInEnum is called when the
    state machine enters the ReturningPortDetachDeviceInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDetachDeviceInEnum


DSM_EVENT
HUBDSM_SettingDevicePDChargingPolicy(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingDevicePDChargingPolicy is called when the
    state machine enters the SettingDevicePDChargingPolicy State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_SetDevicePDChargingPolicyUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SettingDevicePDChargingPolicy


DSM_EVENT
HUBDSM_SettingLPMValues(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingLPMValues is called when the
    state machine enters the SettingLPMValues State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_SettingLPMValues


DSM_EVENT
HUBDSM_SettingNonZeroAddressInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingNonZeroAddressInEnum is called when the
    state machine enters the SettingNonZeroAddressInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_SettingNonZeroAddressInEnum


DSM_EVENT
HUBDSM_StartingTimerForEnumRetryInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StartingTimerForEnumRetryInEnum is called when the
    state machine enters the StartingTimerForEnumRetryInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StartEnumRetryTimer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_StartingTimerForEnumRetryInEnum


DSM_EVENT
HUBDSM_StartingTimerForEnumRetryInEnumWithAddr0Ownership(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StartingTimerForEnumRetryInEnumWithAddr0Ownership is called when the
    state machine enters the StartingTimerForEnumRetryInEnumWithAddr0Ownership State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StartEnumRetryTimer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_StartingTimerForEnumRetryInEnumWithAddr0Ownership


DSM_EVENT
HUBDSM_StoppedSuspendedInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppedSuspendedInEnum is called when the
    state machine enters the StoppedSuspendedInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_StoppedSuspendedInEnum


DSM_EVENT
HUBDSM_UpdatingDeviceInformationInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UpdatingDeviceInformationInEnum is called when the
    state machine enters the UpdatingDeviceInformationInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_UpdateDeviceInfoUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_UpdatingDeviceInformationInEnum


DSM_EVENT
HUBDSM_WaitingForDisableCompleteOnStopSuspendAfterFailureInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDisableCompleteOnStopSuspendAfterFailureInEnum is called when the
    state machine enters the WaitingForDisableCompleteOnStopSuspendAfterFailureInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDisableCompleteOnStopSuspendAfterFailureInEnum


DSM_EVENT
HUBDSM_WaitingForDisableCompleteOnStopSuspendInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDisableCompleteOnStopSuspendInEnum is called when the
    state machine enters the WaitingForDisableCompleteOnStopSuspendInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDisableCompleteOnStopSuspendInEnum


DSM_EVENT
HUBDSM_WaitingForDisableCompleteOnStopSuspendInEnumWithAddress0Ownership(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDisableCompleteOnStopSuspendInEnumWithAddress0Ownership is called when the
    state machine enters the WaitingForDisableCompleteOnStopSuspendInEnumWithAddress0Ownership State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDisableCompleteOnStopSuspendInEnumWithAddress0Ownership


DSM_EVENT
HUBDSM_WaitingForTimerToFlushOnDetachInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerToFlushOnDetachInEnum is called when the
    state machine enters the WaitingForTimerToFlushOnDetachInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerToFlushOnDetachInEnum


DSM_EVENT
HUBDSM_WaitingForTimerToFlushOnStopSuspendInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerToFlushOnStopSuspendInEnum is called when the
    state machine enters the WaitingForTimerToFlushOnStopSuspendInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerToFlushOnStopSuspendInEnum


DSM_EVENT
HUBDSM_WaitingForUCXIoctlOnDetachInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForUCXIoctlOnDetachInEnum is called when the
    state machine enters the WaitingForUCXIoctlOnDetachInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForUCXIoctlOnDetachInEnum


DSM_EVENT
HUBDSM_WaitingForUCXIoctlOnStopSuspendInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForUCXIoctlOnStopSuspendInEnum is called when the
    state machine enters the WaitingForUCXIoctlOnStopSuspendInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_CancelUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForUCXIoctlOnStopSuspendInEnum


DSM_EVENT
HUBDSM_CheckingIfFirstEnumTryAfterReset1(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfFirstEnumTryAfterReset1 is called when the
    state machine enters the CheckingIfFirstEnumTryAfterReset1 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfFirstEnumTry(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfFirstEnumTryAfterReset1


DSM_EVENT
HUBDSM_CheckingIfFirstEnumTryAfterReset2(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfFirstEnumTryAfterReset2 is called when the
    state machine enters the CheckingIfFirstEnumTryAfterReset2 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfFirstEnumTry(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfFirstEnumTryAfterReset2


DSM_EVENT
HUBDSM_CheckingIfSecondResetIsNeeded(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfSecondResetIsNeeded is called when the
    state machine enters the CheckingIfSecondResetIsNeeded State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIf20TypeDeviceNeedsSecondReset(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfSecondResetIsNeeded


DSM_EVENT
HUBDSM_CreatingUCXDefaultEndpoint(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CreatingUCXDefaultEndpoint is called when the
    state machine enters the CreatingUCXDefaultEndpoint State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBUCX_CreateDefaultEndpointInUCX(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CreatingUCXDefaultEndpoint


DSM_EVENT
HUBDSM_CreatingUCXDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CreatingUCXDevice is called when the
    state machine enters the CreatingUCXDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBUCX_CreateDeviceInUCX(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CreatingUCXDevice


DSM_EVENT
HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnDetach is called when the
    state machine enters the DeletingUCXDefaultEndpointAndDeviceOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnDetach


DSM_EVENT
HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnStop is called when the
    state machine enters the DeletingUCXDefaultEndpointAndDeviceOnStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnStop


DSM_EVENT
HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnSuspend is called when the
    state machine enters the DeletingUCXDefaultEndpointAndDeviceOnSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnSuspend


DSM_EVENT
HUBDSM_DeletingUCXDefaultEndpointOnOperationFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingUCXDefaultEndpointOnOperationFailure is called when the
    state machine enters the DeletingUCXDefaultEndpointOnOperationFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingUCXDefaultEndpointOnOperationFailure


DSM_EVENT
HUBDSM_DeletingUCXDeviceOnOperationFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingUCXDeviceOnOperationFailure is called when the
    state machine enters the DeletingUCXDeviceOnOperationFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingUCXDeviceOnOperationFailure


DSM_EVENT
HUBDSM_DisablingDeviceInUCXOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInUCXOnDetach is called when the
    state machine enters the DisablingDeviceInUCXOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInUCXOnDetach


DSM_EVENT
HUBDSM_DisablingDeviceInUCXOnOperationFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInUCXOnOperationFailure is called when the
    state machine enters the DisablingDeviceInUCXOnOperationFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInUCXOnOperationFailure


DSM_EVENT
HUBDSM_DisablingDeviceInUCXOnStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInUCXOnStop is called when the
    state machine enters the DisablingDeviceInUCXOnStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInUCXOnStop


DSM_EVENT
HUBDSM_DisablingDeviceInUCXOnSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInUCXOnSuspend is called when the
    state machine enters the DisablingDeviceInUCXOnSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInUCXOnSuspend


DSM_EVENT
HUBDSM_DisablingPortOnHubStopInEnumAtAddr0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingPortOnHubStopInEnumAtAddr0 is called when the
    state machine enters the DisablingPortOnHubStopInEnumAtAddr0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingPortOnHubStopInEnumAtAddr0


DSM_EVENT
HUBDSM_DisablingPortOnHubSuspendInEnumAtAddr0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingPortOnHubSuspendInEnumAtAddr0 is called when the
    state machine enters the DisablingPortOnHubSuspendInEnumAtAddr0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingPortOnHubSuspendInEnumAtAddr0


DSM_EVENT
HUBDSM_EnablingDeviceInUCX(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_EnablingDeviceInUCX is called when the
    state machine enters the EnablingDeviceInUCX State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_EnableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_EnablingDeviceInUCX


DSM_EVENT
HUBDSM_GettingDeviceDescriptorInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingDeviceDescriptorInEnumAtZero is called when the
    state machine enters the GettingDeviceDescriptorInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetInitialDeviceDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingDeviceDescriptorInEnumAtZero


DSM_EVENT
HUBDSM_LoggingReset1TimeoutInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_LoggingReset1TimeoutInEnumAtZero is called when the
    state machine enters the LoggingReset1TimeoutInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_LogResetTimeout(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_LoggingReset1TimeoutInEnumAtZero


DSM_EVENT
HUBDSM_LoggingReset2TimeoutInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_LoggingReset2TimeoutInEnumAtZero is called when the
    state machine enters the LoggingReset2TimeoutInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_LogResetTimeout(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_LoggingReset2TimeoutInEnumAtZero


DSM_EVENT
HUBDSM_NotifyingDeviceResetToUCXAfterReset2(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_NotifyingDeviceResetToUCXAfterReset2 is called when the
    state machine enters the NotifyingDeviceResetToUCXAfterReset2 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_NotifyDeviceResetUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_NotifyingDeviceResetToUCXAfterReset2


DSM_EVENT
HUBDSM_Resetting1(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_Resetting1 is called when the
    state machine enters the Resetting1 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestReset);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_Resetting1


DSM_EVENT
HUBDSM_Resetting2(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_Resetting2 is called when the
    state machine enters the Resetting2 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestReset);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_Resetting2


DSM_EVENT
HUBDSM_ReturningDeviceOperationFailureInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningDeviceOperationFailureInEnumAtZero is called when the
    state machine enters the ReturningDeviceOperationFailureInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningDeviceOperationFailureInEnumAtZero


DSM_EVENT
HUBDSM_ReturningDeviceOperationSuccessInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningDeviceOperationSuccessInEnumAtZero is called when the
    state machine enters the ReturningDeviceOperationSuccessInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningDeviceOperationSuccessInEnumAtZero


DSM_EVENT
HUBDSM_ReturningHubStopInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubStopInEnumAtZero is called when the
    state machine enters the ReturningHubStopInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubStop;

} // HUBDSM_ReturningHubStopInEnumAtZero


DSM_EVENT
HUBDSM_ReturningHubSuspendInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubSuspendInEnumAtZero is called when the
    state machine enters the ReturningHubSuspendInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubSuspend;

} // HUBDSM_ReturningHubSuspendInEnumAtZero


DSM_EVENT
HUBDSM_ReturningPortDetachDeviceInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDetachDeviceInEnumAtZero is called when the
    state machine enters the ReturningPortDetachDeviceInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDetachDeviceInEnumAtZero


DSM_EVENT
HUBDSM_SettingSpeedFlagFor20Devices(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingSpeedFlagFor20Devices is called when the
    state machine enters the SettingSpeedFlagFor20Devices State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_SetSpeedFlagFor20TypeDevice(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SettingSpeedFlagFor20Devices


DSM_EVENT
HUBDSM_StoppingTimerOnHubStopInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppingTimerOnHubStopInEnumAtZero is called when the
    state machine enters the StoppingTimerOnHubStopInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_StoppingTimerOnHubStopInEnumAtZero


DSM_EVENT
HUBDSM_StoppingTimerOnHubSuspendInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppingTimerOnHubSuspendInEnumAtZero is called when the
    state machine enters the StoppingTimerOnHubSuspendInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_StoppingTimerOnHubSuspendInEnumAtZero


DSM_EVENT
HUBDSM_StoppingTimerOnPortDetachInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppingTimerOnPortDetachInEnumAtZero is called when the
    state machine enters the StoppingTimerOnPortDetachInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_StoppingTimerOnPortDetachInEnumAtZero


DSM_EVENT
HUBDSM_UpdatingDefaultEndpointMaxPacketSizeInEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UpdatingDefaultEndpointMaxPacketSizeInEnum is called when the
    state machine enters the UpdatingDefaultEndpointMaxPacketSizeInEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_UpdateDefaultEndpointUsingUCXIoctl(deviceContext);

        HUBMISC_SetDeviceVersionFlag(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_UpdatingDefaultEndpointMaxPacketSizeInEnum


DSM_EVENT
HUBDSM_ValidatingDeviceDescriptorInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingDeviceDescriptorInEnumAtZero is called when the
    state machine enters the ValidatingDeviceDescriptorInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateFirstDeviceDescriptorFor20(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingDeviceDescriptorInEnumAtZero


DSM_EVENT
HUBDSM_WaitingForPortReset1OrDisableOnHubStopInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPortReset1OrDisableOnHubStopInEnumAtZero is called when the
    state machine enters the WaitingForPortReset1OrDisableOnHubStopInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPortReset1OrDisableOnHubStopInEnumAtZero


DSM_EVENT
HUBDSM_WaitingForPortReset1OrDisableOnHubSuspendInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPortReset1OrDisableOnHubSuspendInEnumAtZero is called when the
    state machine enters the WaitingForPortReset1OrDisableOnHubSuspendInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPortReset1OrDisableOnHubSuspendInEnumAtZero


DSM_EVENT
HUBDSM_WaitingForPortReset2OnHubStopInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPortReset2OnHubStopInEnumAtZero is called when the
    state machine enters the WaitingForPortReset2OnHubStopInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPortReset2OnHubStopInEnumAtZero


DSM_EVENT
HUBDSM_WaitingForPortReset2OnHubSuspendInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPortReset2OnHubSuspendInEnumAtZero is called when the
    state machine enters the WaitingForPortReset2OnHubSuspendInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPortReset2OnHubSuspendInEnumAtZero


DSM_EVENT
HUBDSM_WaitingForPostReset1ExtendedTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPostReset1ExtendedTimer is called when the
    state machine enters the WaitingForPostReset1ExtendedTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StartPostResetExtendedTimer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForPostReset1ExtendedTimer


DSM_EVENT
HUBDSM_WaitingForPostReset1Timer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPostReset1Timer is called when the
    state machine enters the WaitingForPostReset1Timer State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StartPostResetTimerFor20(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForPostReset1Timer


DSM_EVENT
HUBDSM_WaitingForPostReset2ExtendedTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPostReset2ExtendedTimer is called when the
    state machine enters the WaitingForPostReset2ExtendedTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StartPostResetExtendedTimer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForPostReset2ExtendedTimer


DSM_EVENT
HUBDSM_WaitingForPostReset2Timer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPostReset2Timer is called when the
    state machine enters the WaitingForPostReset2Timer State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StartPostResetTimerFor20(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForPostReset2Timer


DSM_EVENT
HUBDSM_WaitingForStopSuspendOnReset1InEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForStopSuspendOnReset1InEnumAtZero is called when the
    state machine enters the WaitingForStopSuspendOnReset1InEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForStopSuspendOnReset1InEnumAtZero


DSM_EVENT
HUBDSM_WaitingForStopSuspendOnReset2InEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForStopSuspendOnReset2InEnumAtZero is called when the
    state machine enters the WaitingForStopSuspendOnReset2InEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForStopSuspendOnReset2InEnumAtZero


DSM_EVENT
HUBDSM_WaitingForTimerOnHubStopInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerOnHubStopInEnumAtZero is called when the
    state machine enters the WaitingForTimerOnHubStopInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerOnHubStopInEnumAtZero


DSM_EVENT
HUBDSM_WaitingForTimerOnHubSuspendInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerOnHubSuspendInEnumAtZero is called when the
    state machine enters the WaitingForTimerOnHubSuspendInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerOnHubSuspendInEnumAtZero


DSM_EVENT
HUBDSM_WaitingForTimerOnPortDetachInEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerOnPortDetachInEnumAtZero is called when the
    state machine enters the WaitingForTimerOnPortDetachInEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerOnPortDetachInEnumAtZero


DSM_EVENT
HUBDSM_CreatingUCXDefaultEndpointFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CreatingUCXDefaultEndpointFor30 is called when the
    state machine enters the CreatingUCXDefaultEndpointFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBUCX_CreateDefaultEndpointInUCX(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CreatingUCXDefaultEndpointFor30


DSM_EVENT
HUBDSM_CreatingUCXDevice30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CreatingUCXDevice30 is called when the
    state machine enters the CreatingUCXDevice30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBUCX_CreateDeviceInUCX(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CreatingUCXDevice30


DSM_EVENT
HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30 is called when the
    state machine enters the DeletingUCXDefaultEndpointOnOperationFailureFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30


DSM_EVENT
HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnDetach is called when the
    state machine enters the DeletingUCXDefaultEndpointOnOperationFailureFor30OnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnDetach


DSM_EVENT
HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnHubStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnHubStop is called when the
    state machine enters the DeletingUCXDefaultEndpointOnOperationFailureFor30OnHubStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnHubStop


DSM_EVENT
HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnHubSuspend is called when the
    state machine enters the DeletingUCXDefaultEndpointOnOperationFailureFor30OnHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDefaultEndpointFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnHubSuspend


DSM_EVENT
HUBDSM_DeletingUCXDeviceOnOperationFailureFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingUCXDeviceOnOperationFailureFor30 is called when the
    state machine enters the DeletingUCXDeviceOnOperationFailureFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingUCXDeviceOnOperationFailureFor30


DSM_EVENT
HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnDetach is called when the
    state machine enters the DeletingUCXDeviceOnOperationFailureFor30OnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnDetach


DSM_EVENT
HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnHubStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnHubStop is called when the
    state machine enters the DeletingUCXDeviceOnOperationFailureFor30OnHubStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnHubStop


DSM_EVENT
HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnHubSuspend is called when the
    state machine enters the DeletingUCXDeviceOnOperationFailureFor30OnHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteDeviceFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnHubSuspend


DSM_EVENT
HUBDSM_EnablingDeviceInUCXFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_EnablingDeviceInUCXFor30 is called when the
    state machine enters the EnablingDeviceInUCXFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_EnableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_EnablingDeviceInUCXFor30


DSM_EVENT
HUBDSM_ResettingDeviceInEnumeration(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ResettingDeviceInEnumeration is called when the
    state machine enters the ResettingDeviceInEnumeration State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ResettingDeviceInEnumeration


DSM_EVENT
HUBDSM_ReturningDetachInPreAddressEnumFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningDetachInPreAddressEnumFor30 is called when the
    state machine enters the ReturningDetachInPreAddressEnumFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningDetachInPreAddressEnumFor30


DSM_EVENT
HUBDSM_ReturningDeviceOperationFailureInPreAddressEnumFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningDeviceOperationFailureInPreAddressEnumFor30 is called when the
    state machine enters the ReturningDeviceOperationFailureInPreAddressEnumFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningDeviceOperationFailureInPreAddressEnumFor30


DSM_EVENT
HUBDSM_ReturningDeviceOperationSuccessInPreAddressEnumFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningDeviceOperationSuccessInPreAddressEnumFor30 is called when the
    state machine enters the ReturningDeviceOperationSuccessInPreAddressEnumFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningDeviceOperationSuccessInPreAddressEnumFor30


DSM_EVENT
HUBDSM_ReturningHubStopInPreAddressEnumFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubStopInPreAddressEnumFor30 is called when the
    state machine enters the ReturningHubStopInPreAddressEnumFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubStop;

} // HUBDSM_ReturningHubStopInPreAddressEnumFor30


DSM_EVENT
HUBDSM_ReturningHubSuspendInPreAddressEnumFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubSuspendInPreAddressEnumFor30 is called when the
    state machine enters the ReturningHubSuspendInPreAddressEnumFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubSuspend;

} // HUBDSM_ReturningHubSuspendInPreAddressEnumFor30


DSM_EVENT
HUBDSM_Setting30Speed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_Setting30Speed is called when the
    state machine enters the Setting30Speed State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_SetSpeedFlagFor30TypeDevice(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_Setting30Speed


DSM_EVENT
HUBDSM_CancellingControlTransferOnDetachInEnumAfterAddr(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CancellingControlTransferOnDetachInEnumAfterAddr is called when the
    state machine enters the CancellingControlTransferOnDetachInEnumAfterAddr State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_CancelControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_CancellingControlTransferOnDetachInEnumAfterAddr


DSM_EVENT
HUBDSM_CancellingControlTransferOnHubStopInEnumAfterAddr(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CancellingControlTransferOnHubStopInEnumAfterAddr is called when the
    state machine enters the CancellingControlTransferOnHubStopInEnumAfterAddr State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_CancelControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_CancellingControlTransferOnHubStopInEnumAfterAddr


DSM_EVENT
HUBDSM_CancellingControlTransferOnHubSuspendInEnumAfterAddr(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CancellingControlTransferOnHubSuspendInEnumAfterAddr is called when the
    state machine enters the CancellingControlTransferOnHubSuspendInEnumAfterAddr State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_CancelControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_CancellingControlTransferOnHubSuspendInEnumAfterAddr


DSM_EVENT
HUBDSM_CheckingBytesReturnedInConfigDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingBytesReturnedInConfigDescriptor is called when the
    state machine enters the CheckingBytesReturnedInConfigDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_IsBytesReturnedLessThanTotalLengthForConfigDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingBytesReturnedInConfigDescriptor


DSM_EVENT
HUBDSM_CheckingIfAltEnumCmdCached(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfAltEnumCmdCached is called when the
    state machine enters the CheckingIfAltEnumCmdCached State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsMsOs20AltEnumCmdNeededInEnum(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfAltEnumCmdCached


DSM_EVENT
HUBDSM_CheckingIfSuperSpeedNeedsToBeDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfSuperSpeedNeedsToBeDisabled is called when the
    state machine enters the CheckingIfSuperSpeedNeedsToBeDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfDeviceIsAtSuperSpeedAndNeedsToBeDisabled(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfSuperSpeedNeedsToBeDisabled


DSM_EVENT
HUBDSM_DiasablingSuperSpeed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DiasablingSuperSpeed is called when the
    state machine enters the DiasablingSuperSpeed State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisableSuperSpeed);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DiasablingSuperSpeed


DSM_EVENT
HUBDSM_GettingConfigurationDescriptorWithDefaultLength(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingConfigurationDescriptorWithDefaultLength is called when the
    state machine enters the GettingConfigurationDescriptorWithDefaultLength State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetConfigDescriptorWithDefaultSizeUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingConfigurationDescriptorWithDefaultLength


DSM_EVENT
HUBDSM_GettingConfigurationDescriptorWithReturnedLength(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingConfigurationDescriptorWithReturnedLength is called when the
    state machine enters the GettingConfigurationDescriptorWithReturnedLength State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetConfigDescriptorWithReturnedLengthUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingConfigurationDescriptorWithReturnedLength


DSM_EVENT
HUBDSM_GettingDeviceDescriptorAfterAddressing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingDeviceDescriptorAfterAddressing is called when the
    state machine enters the GettingDeviceDescriptorAfterAddressing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetDeviceDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingDeviceDescriptorAfterAddressing


DSM_EVENT
HUBDSM_GettingRemainingDescriptorsAfterConfigDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingRemainingDescriptorsAfterConfigDescriptor is called when the
    state machine enters the GettingRemainingDescriptorsAfterConfigDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_GettingRemainingDescriptorsAfterConfigDescriptor


DSM_EVENT
HUBDSM_QueryingRegistryValuesForDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueryingRegistryValuesForDevice is called when the
    state machine enters the QueryingRegistryValuesForDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_QueryAndCacheRegistryValuesForDevice(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_QueryingRegistryValuesForDevice


DSM_EVENT
HUBDSM_ReturningHubStopInEnumAfterAddr(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubStopInEnumAfterAddr is called when the
    state machine enters the ReturningHubStopInEnumAfterAddr State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubStop;

} // HUBDSM_ReturningHubStopInEnumAfterAddr


DSM_EVENT
HUBDSM_ReturningHubSuspendInEnumAfterAddr(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubSuspendInEnumAfterAddr is called when the
    state machine enters the ReturningHubSuspendInEnumAfterAddr State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubSuspend;

} // HUBDSM_ReturningHubSuspendInEnumAfterAddr


DSM_EVENT
HUBDSM_ReturningOperationFailureInEnumAfterAddr(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureInEnumAfterAddr is called when the
    state machine enters the ReturningOperationFailureInEnumAfterAddr State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureInEnumAfterAddr


DSM_EVENT
HUBDSM_ReturningOperationSuccessInEnumAfterAddr(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInEnumAfterAddr is called when the
    state machine enters the ReturningOperationSuccessInEnumAfterAddr State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInEnumAfterAddr


DSM_EVENT
HUBDSM_ReturningPortDeviceDetachInEnumAfterAddr(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDeviceDetachInEnumAfterAddr is called when the
    state machine enters the ReturningPortDeviceDetachInEnumAfterAddr State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDeviceDetachInEnumAfterAddr


DSM_EVENT
HUBDSM_SendingAltEnumCmdInEnumAfterAddr(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SendingAltEnumCmdInEnumAfterAddr is called when the
    state machine enters the SendingAltEnumCmdInEnumAfterAddr State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_SendMsOs20AltEnumCommandUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SendingAltEnumCmdInEnumAfterAddr


DSM_EVENT
HUBDSM_SettingVersionFlagInEnumerating(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingVersionFlagInEnumerating is called when the
    state machine enters the SettingVersionFlagInEnumerating State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_SetDeviceVersionFlag(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SettingVersionFlagInEnumerating


DSM_EVENT
HUBDSM_ValidatingConfigurationDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingConfigurationDescriptor is called when the
    state machine enters the ValidatingConfigurationDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheConfigDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingConfigurationDescriptor


DSM_EVENT
HUBDSM_ValidatingDeviceDescriptorAfterAddressing(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingDeviceDescriptorAfterAddressing is called when the
    state machine enters the ValidatingDeviceDescriptorAfterAddressing State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheDeviceDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingDeviceDescriptorAfterAddressing


DSM_EVENT
HUBDSM_CancellingControlTransferOnDetachInRemainingDescriptors(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CancellingControlTransferOnDetachInRemainingDescriptors is called when the
    state machine enters the CancellingControlTransferOnDetachInRemainingDescriptors State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_CancelControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_CancellingControlTransferOnDetachInRemainingDescriptors


DSM_EVENT
HUBDSM_CancellingControlTransferOnHubStopInRemainingDescriptors(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CancellingControlTransferOnHubStopInRemainingDescriptors is called when the
    state machine enters the CancellingControlTransferOnHubStopInRemainingDescriptors State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_CancelControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_CancellingControlTransferOnHubStopInRemainingDescriptors


DSM_EVENT
HUBDSM_CancellingControlTransferOnHubSuspendInRemainingDescriptors(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CancellingControlTransferOnHubSuspendInRemainingDescriptors is called when the
    state machine enters the CancellingControlTransferOnHubSuspendInRemainingDescriptors State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_CancelControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_CancellingControlTransferOnHubSuspendInRemainingDescriptors


DSM_EVENT
HUBDSM_CheckingIfIProductIdStringDescriptorShouldBeQueried(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfIProductIdStringDescriptorShouldBeQueried is called when the
    state machine enters the CheckingIfIProductIdStringDescriptorShouldBeQueried State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfProductIdDescriptorShouldBeQueried(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfIProductIdStringDescriptorShouldBeQueried


DSM_EVENT
HUBDSM_CheckingIfIProductIsZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfIProductIsZero is called when the
    state machine enters the CheckingIfIProductIsZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfIProductIsZero(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfIProductIsZero


DSM_EVENT
HUBDSM_GettingBosDescriptorSet(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingBosDescriptorSet is called when the
    state machine enters the GettingBosDescriptorSet State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_GettingBosDescriptorSet


DSM_EVENT
HUBDSM_GettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch is called when the
    state machine enters the GettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_GettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch


DSM_EVENT
HUBDSM_GettingLanguageIdStringDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingLanguageIdStringDescriptor is called when the
    state machine enters the GettingLanguageIdStringDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetLanguageIdStringDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingLanguageIdStringDescriptor


DSM_EVENT
HUBDSM_GettingMSOSAndSerialNumberDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingMSOSAndSerialNumberDescriptor is called when the
    state machine enters the GettingMSOSAndSerialNumberDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_GettingMSOSAndSerialNumberDescriptor


DSM_EVENT
HUBDSM_GettingProductIdStringDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingProductIdStringDescriptor is called when the
    state machine enters the GettingProductIdStringDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetProductIdStringDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingProductIdStringDescriptor


DSM_EVENT
HUBDSM_ReturningErrorResponseOnLanguageIdQuery(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningErrorResponseOnLanguageIdQuery is called when the
    state machine enters the ReturningErrorResponseOnLanguageIdQuery State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_GetGenericErrorResponseOnDescriptorFailure(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReturningErrorResponseOnLanguageIdQuery


DSM_EVENT
HUBDSM_ReturningErrorResponseOnProductStringQuery(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningErrorResponseOnProductStringQuery is called when the
    state machine enters the ReturningErrorResponseOnProductStringQuery State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_GetGenericErrorResponseOnDescriptorFailure(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReturningErrorResponseOnProductStringQuery


DSM_EVENT
HUBDSM_ReturningHubStopInRemainingDescriptors(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubStopInRemainingDescriptors is called when the
    state machine enters the ReturningHubStopInRemainingDescriptors State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubStop;

} // HUBDSM_ReturningHubStopInRemainingDescriptors


DSM_EVENT
HUBDSM_ReturningHubSuspendInRemainingDescriptors(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubSuspendInRemainingDescriptors is called when the
    state machine enters the ReturningHubSuspendInRemainingDescriptors State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubSuspend;

} // HUBDSM_ReturningHubSuspendInRemainingDescriptors


DSM_EVENT
HUBDSM_ReturningOperationFailureInRemainingDescriptors(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureInRemainingDescriptors is called when the
    state machine enters the ReturningOperationFailureInRemainingDescriptors State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureInRemainingDescriptors


DSM_EVENT
HUBDSM_ReturningOperationSuccessInRemainingDescriptors(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInRemainingDescriptors is called when the
    state machine enters the ReturningOperationSuccessInRemainingDescriptors State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInRemainingDescriptors


DSM_EVENT
HUBDSM_ReturningPortDeviceDetachInRemainingDescriptors(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDeviceDetachInRemainingDescriptors is called when the
    state machine enters the ReturningPortDeviceDetachInRemainingDescriptors State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDeviceDetachInRemainingDescriptors


DSM_EVENT
HUBDSM_ValidatingLanguageIdStringDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingLanguageIdStringDescriptor is called when the
    state machine enters the ValidatingLanguageIdStringDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheLanguageIdStringDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingLanguageIdStringDescriptor


DSM_EVENT
HUBDSM_ValidatingProductIdStringDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingProductIdStringDescriptor is called when the
    state machine enters the ValidatingProductIdStringDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheProductIdStringDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingProductIdStringDescriptor


DSM_EVENT
HUBDSM_CheckingIfDeviceSupportsContainerId(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDeviceSupportsContainerId is called when the
    state machine enters the CheckingIfDeviceSupportsContainerId State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckMSOSDescriptorIfContainerIdIsSupported(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDeviceSupportsContainerId


DSM_EVENT
HUBDSM_CheckingIfIgnoreHWSerNumIsSet(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfIgnoreHWSerNumIsSet is called when the
    state machine enters the CheckingIfIgnoreHWSerNumIsSet State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_QueryCachedRegistryIfIgnoreHWSerNumIsSet(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfIgnoreHWSerNumIsSet


DSM_EVENT
HUBDSM_CheckingIfMSOSContainerIdDescriptorIsSupported(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfMSOSContainerIdDescriptorIsSupported is called when the
    state machine enters the CheckingIfMSOSContainerIdDescriptorIsSupported State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfContainerIdShouldBeQueried(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfMSOSContainerIdDescriptorIsSupported


DSM_EVENT
HUBDSM_CheckingIfMSOSDescriptorShouldBeQueried(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfMSOSDescriptorShouldBeQueried is called when the
    state machine enters the CheckingIfMSOSDescriptorShouldBeQueried State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfMSOSDescriptorShouldBeQueried(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfMSOSDescriptorShouldBeQueried


DSM_EVENT
HUBDSM_CheckingIfMSOSExtendedConfigDescriptorIsSupported(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfMSOSExtendedConfigDescriptorIsSupported is called when the
    state machine enters the CheckingIfMSOSExtendedConfigDescriptorIsSupported State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfMSOSExtendedConfigDescriptorIsSupported(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfMSOSExtendedConfigDescriptorIsSupported


DSM_EVENT
HUBDSM_CheckingIfSerialNumberStringIndexIsZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfSerialNumberStringIndexIsZero is called when the
    state machine enters the CheckingIfSerialNumberStringIndexIsZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfSerialNumberStringIndexIsZero(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfSerialNumberStringIndexIsZero


DSM_EVENT
HUBDSM_GettingMSOSContainerIdDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingMSOSContainerIdDescriptor is called when the
    state machine enters the GettingMSOSContainerIdDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetMSOSContainerIdDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingMSOSContainerIdDescriptor


DSM_EVENT
HUBDSM_GettingMSOSContainerIdHeaderDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingMSOSContainerIdHeaderDescriptor is called when the
    state machine enters the GettingMSOSContainerIdHeaderDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetMSOSContainerIdDescriptorHeaderUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingMSOSContainerIdHeaderDescriptor


DSM_EVENT
HUBDSM_GettingMSOSDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingMSOSDescriptor is called when the
    state machine enters the GettingMSOSDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetMSOSDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingMSOSDescriptor


DSM_EVENT
HUBDSM_GettingMSOSExtendedDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingMSOSExtendedDescriptor is called when the
    state machine enters the GettingMSOSExtendedDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetMSOSExtendedConfigDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingMSOSExtendedDescriptor


DSM_EVENT
HUBDSM_GettingMSOSExtendedDescriptorHeader(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingMSOSExtendedDescriptorHeader is called when the
    state machine enters the GettingMSOSExtendedDescriptorHeader State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetMSOSExtendedConfigDescriptorHeaderUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingMSOSExtendedDescriptorHeader


DSM_EVENT
HUBDSM_GettingSerialNumberStringDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingSerialNumberStringDescriptor is called when the
    state machine enters the GettingSerialNumberStringDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetSerialNumberStringDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingSerialNumberStringDescriptor


DSM_EVENT
HUBDSM_MarkDeviceAsNotSupportingMSOSDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_MarkDeviceAsNotSupportingMSOSDescriptor is called when the
    state machine enters the MarkDeviceAsNotSupportingMSOSDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_MarkDeviceAsNotSupportingMSOSDescriptor(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_MarkDeviceAsNotSupportingMSOSDescriptor


DSM_EVENT
HUBDSM_MarkDeviceAsSupportingMSOSDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_MarkDeviceAsSupportingMSOSDescriptor is called when the
    state machine enters the MarkDeviceAsSupportingMSOSDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StoreDeviceMSOSVendorCodeInRegsitry(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_MarkDeviceAsSupportingMSOSDescriptor


DSM_EVENT
HUBDSM_MarkingDeviceAsNotSupportingContainerId(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_MarkingDeviceAsNotSupportingContainerId is called when the
    state machine enters the MarkingDeviceAsNotSupportingContainerId State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_MarkDeviceAsNotSupportingContainerId(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_MarkingDeviceAsNotSupportingContainerId


DSM_EVENT
HUBDSM_MarkingDeviceAsNotSupportingContainerIdBasedOnMSOSDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_MarkingDeviceAsNotSupportingContainerIdBasedOnMSOSDescriptor is called when the
    state machine enters the MarkingDeviceAsNotSupportingContainerIdBasedOnMSOSDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_MarkDeviceAsNotSupportingContainerId(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_MarkingDeviceAsNotSupportingContainerIdBasedOnMSOSDescriptor


DSM_EVENT
HUBDSM_ReturningErrorResponseOnContainerIdHeaderQuery(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningErrorResponseOnContainerIdHeaderQuery is called when the
    state machine enters the ReturningErrorResponseOnContainerIdHeaderQuery State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_GetGenericErrorResponseOnDescriptorFailure(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReturningErrorResponseOnContainerIdHeaderQuery


DSM_EVENT
HUBDSM_ReturningErrorResponseOnContainerIdQuery(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningErrorResponseOnContainerIdQuery is called when the
    state machine enters the ReturningErrorResponseOnContainerIdQuery State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_GetGenericErrorResponseOnDescriptorFailure(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReturningErrorResponseOnContainerIdQuery


DSM_EVENT
HUBDSM_ReturningErrorResponseOnMSOSExtendedHeaderQuery(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningErrorResponseOnMSOSExtendedHeaderQuery is called when the
    state machine enters the ReturningErrorResponseOnMSOSExtendedHeaderQuery State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_GetGenericErrorResponseOnDescriptorFailure(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReturningErrorResponseOnMSOSExtendedHeaderQuery


DSM_EVENT
HUBDSM_ReturningErrorResponseOnMSOSExtendedQuery(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningErrorResponseOnMSOSExtendedQuery is called when the
    state machine enters the ReturningErrorResponseOnMSOSExtendedQuery State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_GetGenericErrorResponseOnDescriptorFailure(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReturningErrorResponseOnMSOSExtendedQuery


DSM_EVENT
HUBDSM_ReturningErrorResponseOnMSOSQuery(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningErrorResponseOnMSOSQuery is called when the
    state machine enters the ReturningErrorResponseOnMSOSQuery State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventDeviceIgnoreError;

} // HUBDSM_ReturningErrorResponseOnMSOSQuery


DSM_EVENT
HUBDSM_ReturningErrorResponseOnSerialNumberQuery(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningErrorResponseOnSerialNumberQuery is called when the
    state machine enters the ReturningErrorResponseOnSerialNumberQuery State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_GetGenericErrorResponseOnDescriptorFailure(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReturningErrorResponseOnSerialNumberQuery


DSM_EVENT
HUBDSM_ReturningOperationFailureInGettingDescriptorsForGreaterThan1x(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureInGettingDescriptorsForGreaterThan1x is called when the
    state machine enters the ReturningOperationFailureInGettingDescriptorsForGreaterThan1x State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureInGettingDescriptorsForGreaterThan1x


DSM_EVENT
HUBDSM_ReturningOperationSuccessInGettingDescriptorsForGreaterThan1x(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInGettingDescriptorsForGreaterThan1x is called when the
    state machine enters the ReturningOperationSuccessInGettingDescriptorsForGreaterThan1x State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInGettingDescriptorsForGreaterThan1x


DSM_EVENT
HUBDSM_ValidatingMSOSContainerIdDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingMSOSContainerIdDescriptor is called when the
    state machine enters the ValidatingMSOSContainerIdDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheMSOSContainerIdDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingMSOSContainerIdDescriptor


DSM_EVENT
HUBDSM_ValidatingMSOSContainerIdHeaderDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingMSOSContainerIdHeaderDescriptor is called when the
    state machine enters the ValidatingMSOSContainerIdHeaderDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheMSOSContainerIdDescriptorHeader(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingMSOSContainerIdHeaderDescriptor


DSM_EVENT
HUBDSM_ValidatingMSOSDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingMSOSDescriptor is called when the
    state machine enters the ValidatingMSOSDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheMSOSDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingMSOSDescriptor


DSM_EVENT
HUBDSM_ValidatingMSOSExtendedConfigDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingMSOSExtendedConfigDescriptor is called when the
    state machine enters the ValidatingMSOSExtendedConfigDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheMSOSExtendedConfigDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingMSOSExtendedConfigDescriptor


DSM_EVENT
HUBDSM_ValidatingMSOSExtendedConfigDescriptorHeader(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingMSOSExtendedConfigDescriptorHeader is called when the
    state machine enters the ValidatingMSOSExtendedConfigDescriptorHeader State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheMSOSExtendedConfigDescriptorHeader(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingMSOSExtendedConfigDescriptorHeader


DSM_EVENT
HUBDSM_ValidatingSerialNumberStringDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingSerialNumberStringDescriptor is called when the
    state machine enters the ValidatingSerialNumberStringDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheSerialNumberStringDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingSerialNumberStringDescriptor


DSM_EVENT
HUBDSM_CheckingIfIgnoreHWSerNumIsSetFor1xDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfIgnoreHWSerNumIsSetFor1xDevice is called when the
    state machine enters the CheckingIfIgnoreHWSerNumIsSetFor1xDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_QueryCachedRegistryIfIgnoreHWSerNumIsSet(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfIgnoreHWSerNumIsSetFor1xDevice


DSM_EVENT
HUBDSM_CheckingIfSerialNumberStringIndexIsZeroFor1xDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfSerialNumberStringIndexIsZeroFor1xDevice is called when the
    state machine enters the CheckingIfSerialNumberStringIndexIsZeroFor1xDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfSerialNumberStringIndexIsZero(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfSerialNumberStringIndexIsZeroFor1xDevice


DSM_EVENT
HUBDSM_GettingSerialNumberStringDescriptorFor1xDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingSerialNumberStringDescriptorFor1xDevice is called when the
    state machine enters the GettingSerialNumberStringDescriptorFor1xDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetSerialNumberStringDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingSerialNumberStringDescriptorFor1xDevice


DSM_EVENT
HUBDSM_ReturningErrorResponseOnSerialNumberQueryFor1xDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningErrorResponseOnSerialNumberQueryFor1xDevice is called when the
    state machine enters the ReturningErrorResponseOnSerialNumberQueryFor1xDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_GetGenericErrorResponseOnDescriptorFailure(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReturningErrorResponseOnSerialNumberQueryFor1xDevice


DSM_EVENT
HUBDSM_ReturningOperationFailureInGettingSerialNumberFor1x(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureInGettingSerialNumberFor1x is called when the
    state machine enters the ReturningOperationFailureInGettingSerialNumberFor1x State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureInGettingSerialNumberFor1x


DSM_EVENT
HUBDSM_ReturningOperationSuccessInGettingSerialNumberFor1x(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInGettingSerialNumberFor1x is called when the
    state machine enters the ReturningOperationSuccessInGettingSerialNumberFor1x State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInGettingSerialNumberFor1x


DSM_EVENT
HUBDSM_ValidatingSerialNumberStringDescriptorFor1xDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingSerialNumberStringDescriptorFor1xDevice is called when the
    state machine enters the ValidatingSerialNumberStringDescriptorFor1xDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheSerialNumberStringDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingSerialNumberStringDescriptorFor1xDevice


DSM_EVENT
HUBDSM_CheckingIfWaitRequiredAfterSetAddress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfWaitRequiredAfterSetAddress is called when the
    state machine enters the CheckingIfWaitRequiredAfterSetAddress State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfWaitRequiredAfterSetAddress(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfWaitRequiredAfterSetAddress


DSM_EVENT
HUBDSM_ReturningHubStopInSettingAddress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubStopInSettingAddress is called when the
    state machine enters the ReturningHubStopInSettingAddress State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubStop;

} // HUBDSM_ReturningHubStopInSettingAddress


DSM_EVENT
HUBDSM_ReturningHubSuspendInSettingAddress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubSuspendInSettingAddress is called when the
    state machine enters the ReturningHubSuspendInSettingAddress State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubSuspend;

} // HUBDSM_ReturningHubSuspendInSettingAddress


DSM_EVENT
HUBDSM_ReturningOperationFailureInSettingAddress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureInSettingAddress is called when the
    state machine enters the ReturningOperationFailureInSettingAddress State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureInSettingAddress


DSM_EVENT
HUBDSM_ReturningOperationSuccessInSettingAddress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInSettingAddress is called when the
    state machine enters the ReturningOperationSuccessInSettingAddress State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInSettingAddress


DSM_EVENT
HUBDSM_ReturningPortDetachDeviceInSettingAddress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDetachDeviceInSettingAddress is called when the
    state machine enters the ReturningPortDetachDeviceInSettingAddress State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDetachDeviceInSettingAddress


DSM_EVENT
HUBDSM_SettingNonZeroAddress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingNonZeroAddress is called when the
    state machine enters the SettingNonZeroAddress State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_SetDeviceAddressUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SettingNonZeroAddress


DSM_EVENT
HUBDSM_StoppingTimerOnHubStopInSettingAddress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppingTimerOnHubStopInSettingAddress is called when the
    state machine enters the StoppingTimerOnHubStopInSettingAddress State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_StoppingTimerOnHubStopInSettingAddress


DSM_EVENT
HUBDSM_StoppingTimerOnHubSuspendInSettingAddress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppingTimerOnHubSuspendInSettingAddress is called when the
    state machine enters the StoppingTimerOnHubSuspendInSettingAddress State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_StoppingTimerOnHubSuspendInSettingAddress


DSM_EVENT
HUBDSM_StoppingTimerOnPortDetachInSettingAddress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppingTimerOnPortDetachInSettingAddress is called when the
    state machine enters the StoppingTimerOnPortDetachInSettingAddress State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_StoppingTimerOnPortDetachInSettingAddress


DSM_EVENT
HUBDSM_WaitingForTimerAfterSetAddress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerAfterSetAddress is called when the
    state machine enters the WaitingForTimerAfterSetAddress State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StartPostAddressTimer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForTimerAfterSetAddress


DSM_EVENT
HUBDSM_WaitingForTimerOnHubStopInSettingAddress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerOnHubStopInSettingAddress is called when the
    state machine enters the WaitingForTimerOnHubStopInSettingAddress State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerOnHubStopInSettingAddress


DSM_EVENT
HUBDSM_WaitingForTimerOnHubSuspendInSettingAddress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerOnHubSuspendInSettingAddress is called when the
    state machine enters the WaitingForTimerOnHubSuspendInSettingAddress State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerOnHubSuspendInSettingAddress


DSM_EVENT
HUBDSM_WaitingForTimerOnPortDetachInSettingAddress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerOnPortDetachInSettingAddress is called when the
    state machine enters the WaitingForTimerOnPortDetachInSettingAddress State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerOnPortDetachInSettingAddress


DSM_EVENT
HUBDSM_CheckingBytesReturnedInAlternateConfigDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingBytesReturnedInAlternateConfigDescriptor is called when the
    state machine enters the CheckingBytesReturnedInAlternateConfigDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_IsBytesReturnedLessThanTotalLengthForConfigDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingBytesReturnedInAlternateConfigDescriptor


DSM_EVENT
HUBDSM_CheckingIfAltEnumCommandNeeded(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfAltEnumCommandNeeded is called when the
    state machine enters the CheckingIfAltEnumCommandNeeded State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsMsOs20AltEnumCmdNeededAfterGettingBos(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfAltEnumCommandNeeded


DSM_EVENT
HUBDSM_CheckingIfAlternateBOSDescriptorQueryShouldBeSkipped(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfAlternateBOSDescriptorQueryShouldBeSkipped is called when the
    state machine enters the CheckingIfAlternateBOSDescriptorQueryShouldBeSkipped State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfBOSDescriptorQueryShouldBeSkipped(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfAlternateBOSDescriptorQueryShouldBeSkipped


DSM_EVENT
HUBDSM_CheckingIfAlternateModeStringDescriptorShouldBeQueried(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfAlternateModeStringDescriptorShouldBeQueried is called when the
    state machine enters the CheckingIfAlternateModeStringDescriptorShouldBeQueried State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_CheckIfAlternateModeStringDescriptorShouldBeQueried(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfAlternateModeStringDescriptorShouldBeQueried


DSM_EVENT
HUBDSM_CheckingIfBillboardBOSDescriptorIsPresent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfBillboardBOSDescriptorIsPresent is called when the
    state machine enters the CheckingIfBillboardBOSDescriptorIsPresent State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_CheckIfBillboardBOSDescriptorIsPresent(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfBillboardBOSDescriptorIsPresent


DSM_EVENT
HUBDSM_CheckingIfBillboardStringDescriptorShouldBeQueried(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfBillboardStringDescriptorShouldBeQueried is called when the
    state machine enters the CheckingIfBillboardStringDescriptorShouldBeQueried State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_CheckIfBillboardStringDescriptorShouldBeQueried(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfBillboardStringDescriptorShouldBeQueried


DSM_EVENT
HUBDSM_CheckingIfBOSDescriptorQueryShouldBeSkipped(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfBOSDescriptorQueryShouldBeSkipped is called when the
    state machine enters the CheckingIfBOSDescriptorQueryShouldBeSkipped State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfBOSDescriptorQueryShouldBeSkipped(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfBOSDescriptorQueryShouldBeSkipped


DSM_EVENT
HUBDSM_CheckingIfCompleteAlternateBOSDescriptorWasRetrieved(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfCompleteAlternateBOSDescriptorWasRetrieved is called when the
    state machine enters the CheckingIfCompleteAlternateBOSDescriptorWasRetrieved State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_CheckIfCompleteBOSDescriptorHasBeenRetrieved(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfCompleteAlternateBOSDescriptorWasRetrieved


DSM_EVENT
HUBDSM_CheckingIfCompleteBOSDescriptorWasRetrieved(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfCompleteBOSDescriptorWasRetrieved is called when the
    state machine enters the CheckingIfCompleteBOSDescriptorWasRetrieved State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_CheckIfCompleteBOSDescriptorHasBeenRetrieved(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfCompleteBOSDescriptorWasRetrieved


DSM_EVENT
HUBDSM_CheckingIfDualRoleFeaturesSupported(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDualRoleFeaturesSupported is called when the
    state machine enters the CheckingIfDualRoleFeaturesSupported State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfDualRoleSupported(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDualRoleFeaturesSupported


DSM_EVENT
HUBDSM_GetMsOs20DescriptorSet(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GetMsOs20DescriptorSet is called when the
    state machine enters the GetMsOs20DescriptorSet State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetMSOS20DescriptorSetUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GetMsOs20DescriptorSet


DSM_EVENT
HUBDSM_GettingAlternateBOSDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingAlternateBOSDescriptor is called when the
    state machine enters the GettingAlternateBOSDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetBOSDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingAlternateBOSDescriptor


DSM_EVENT
HUBDSM_GettingAlternateBOSDescriptorHeader(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingAlternateBOSDescriptorHeader is called when the
    state machine enters the GettingAlternateBOSDescriptorHeader State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetBOSDescriptorHeaderUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingAlternateBOSDescriptorHeader


DSM_EVENT
HUBDSM_GettingAlternateConfigurationDescriptorWithDefaultLength(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingAlternateConfigurationDescriptorWithDefaultLength is called when the
    state machine enters the GettingAlternateConfigurationDescriptorWithDefaultLength State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetConfigDescriptorWithDefaultSizeUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingAlternateConfigurationDescriptorWithDefaultLength


DSM_EVENT
HUBDSM_GettingAlternateConfigurationDescriptorWithReturnedLength(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingAlternateConfigurationDescriptorWithReturnedLength is called when the
    state machine enters the GettingAlternateConfigurationDescriptorWithReturnedLength State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetConfigDescriptorWithReturnedLengthUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingAlternateConfigurationDescriptorWithReturnedLength


DSM_EVENT
HUBDSM_GettingAlternateDeviceDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingAlternateDeviceDescriptor is called when the
    state machine enters the GettingAlternateDeviceDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetDeviceDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingAlternateDeviceDescriptor


DSM_EVENT
HUBDSM_GettingAlternateModeStringDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingAlternateModeStringDescriptor is called when the
    state machine enters the GettingAlternateModeStringDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetAlternateModeStringDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingAlternateModeStringDescriptor


DSM_EVENT
HUBDSM_GettingBillboardStringDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingBillboardStringDescriptor is called when the
    state machine enters the GettingBillboardStringDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetBillboardStringDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingBillboardStringDescriptor


DSM_EVENT
HUBDSM_GettingBOSDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingBOSDescriptor is called when the
    state machine enters the GettingBOSDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetBOSDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingBOSDescriptor


DSM_EVENT
HUBDSM_GettingBOSDescriptorHeader(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingBOSDescriptorHeader is called when the
    state machine enters the GettingBOSDescriptorHeader State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetBOSDescriptorHeaderUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingBOSDescriptorHeader


DSM_EVENT
HUBDSM_IsMsOs20DescriptorSupported(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_IsMsOs20DescriptorSupported is called when the
    state machine enters the IsMsOs20DescriptorSupported State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfMsOs20DescriptorSupported(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_IsMsOs20DescriptorSupported


DSM_EVENT
HUBDSM_QueryingRegistryValuesForAlternateDeviceEnumeration(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueryingRegistryValuesForAlternateDeviceEnumeration is called when the
    state machine enters the QueryingRegistryValuesForAlternateDeviceEnumeration State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_QueryAndCacheRegistryValuesForDevice(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_QueryingRegistryValuesForAlternateDeviceEnumeration


DSM_EVENT
HUBDSM_ReturningErrorResponseOnBOSDescriptorHeaderQuery(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningErrorResponseOnBOSDescriptorHeaderQuery is called when the
    state machine enters the ReturningErrorResponseOnBOSDescriptorHeaderQuery State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_GetGenericErrorResponseOnDescriptorFailure(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReturningErrorResponseOnBOSDescriptorHeaderQuery


DSM_EVENT
HUBDSM_ReturningErrorResponseOnBOSDescriptorQuery(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningErrorResponseOnBOSDescriptorQuery is called when the
    state machine enters the ReturningErrorResponseOnBOSDescriptorQuery State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_GetGenericErrorResponseOnDescriptorFailure(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReturningErrorResponseOnBOSDescriptorQuery


DSM_EVENT
HUBDSM_ReturningOperationFailureInGettingBOSDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureInGettingBOSDescriptor is called when the
    state machine enters the ReturningOperationFailureInGettingBOSDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureInGettingBOSDescriptor


DSM_EVENT
HUBDSM_ReturningOperationSuccessInGettingBOSDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInGettingBOSDescriptor is called when the
    state machine enters the ReturningOperationSuccessInGettingBOSDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInGettingBOSDescriptor


DSM_EVENT
HUBDSM_SendingMsOs20AlternateEnumerationCommand(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SendingMsOs20AlternateEnumerationCommand is called when the
    state machine enters the SendingMsOs20AlternateEnumerationCommand State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_SendMsOs20AltEnumCommandUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SendingMsOs20AlternateEnumerationCommand


DSM_EVENT
HUBDSM_SendingUsbFeaturesVendorCmd(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SendingUsbFeaturesVendorCmd is called when the
    state machine enters the SendingUsbFeaturesVendorCmd State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_SendUsbFeaturesVendorCmdUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SendingUsbFeaturesVendorCmd


DSM_EVENT
HUBDSM_SettingVersionFlagInAlternateEnumeration(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingVersionFlagInAlternateEnumeration is called when the
    state machine enters the SettingVersionFlagInAlternateEnumeration State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_SetDeviceVersionFlag(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SettingVersionFlagInAlternateEnumeration


DSM_EVENT
HUBDSM_ValidatingAlternateBOSDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingAlternateBOSDescriptor is called when the
    state machine enters the ValidatingAlternateBOSDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheBOSDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingAlternateBOSDescriptor


DSM_EVENT
HUBDSM_ValidatingAlternateBOSDescriptorHeader(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingAlternateBOSDescriptorHeader is called when the
    state machine enters the ValidatingAlternateBOSDescriptorHeader State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheBOSDescriptorHeader(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingAlternateBOSDescriptorHeader


DSM_EVENT
HUBDSM_ValidatingAlternateConfigurationDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingAlternateConfigurationDescriptor is called when the
    state machine enters the ValidatingAlternateConfigurationDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheConfigDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingAlternateConfigurationDescriptor


DSM_EVENT
HUBDSM_ValidatingAlternateDeviceDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingAlternateDeviceDescriptor is called when the
    state machine enters the ValidatingAlternateDeviceDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheDeviceDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingAlternateDeviceDescriptor


DSM_EVENT
HUBDSM_ValidatingAndLoggingAlternateModeStringDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingAndLoggingAlternateModeStringDescriptor is called when the
    state machine enters the ValidatingAndLoggingAlternateModeStringDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheAlternateModeStringDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingAndLoggingAlternateModeStringDescriptor


DSM_EVENT
HUBDSM_ValidatingAndLoggingBillboardStringDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingAndLoggingBillboardStringDescriptor is called when the
    state machine enters the ValidatingAndLoggingBillboardStringDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheBillboardStringDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingAndLoggingBillboardStringDescriptor


DSM_EVENT
HUBDSM_ValidatingBOSDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingBOSDescriptor is called when the
    state machine enters the ValidatingBOSDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheBOSDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingBOSDescriptor


DSM_EVENT
HUBDSM_ValidatingBOSDescriptorHeader(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingBOSDescriptorHeader is called when the
    state machine enters the ValidatingBOSDescriptorHeader State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheBOSDescriptorHeader(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingBOSDescriptorHeader


DSM_EVENT
HUBDSM_ValidatingMsOs20DescriptorSetIfPresent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingMsOs20DescriptorSetIfPresent is called when the
    state machine enters the ValidatingMsOs20DescriptorSetIfPresent State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheMSOS20DescriptorSet(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingMsOs20DescriptorSetIfPresent


DSM_EVENT
HUBDSM_GettingDeviceQualifierDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingDeviceQualifierDescriptor is called when the
    state machine enters the GettingDeviceQualifierDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetDeviceQualifierDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingDeviceQualifierDescriptor


DSM_EVENT
HUBDSM_ReturningErrorResponseOnDeviceQualifierQuery(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningErrorResponseOnDeviceQualifierQuery is called when the
    state machine enters the ReturningErrorResponseOnDeviceQualifierQuery State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_GetGenericErrorResponseOnDescriptorFailure(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReturningErrorResponseOnDeviceQualifierQuery


DSM_EVENT
HUBDSM_ReturningOperationSuccessInCheckingFor20FullSpeed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInCheckingFor20FullSpeed is called when the
    state machine enters the ReturningOperationSuccessInCheckingFor20FullSpeed State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInCheckingFor20FullSpeed


DSM_EVENT
HUBDSM_ValidatingDeviceQualifierDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingDeviceQualifierDescriptor is called when the
    state machine enters the ValidatingDeviceQualifierDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateAndCacheDeviceQualifierDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingDeviceQualifierDescriptor


DSM_EVENT
HUBDSM_AddingDeviceToGlobalChildList(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AddingDeviceToGlobalChildList is called when the
    state machine enters the AddingDeviceToGlobalChildList State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_AddDeviceInfoToTheGlobalChildList(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AddingDeviceToGlobalChildList


DSM_EVENT
HUBDSM_CheckingIfAddDeviceToGlobalListRetryCountExceededLimit(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfAddDeviceToGlobalListRetryCountExceededLimit is called when the
    state machine enters the CheckingIfAddDeviceToGlobalListRetryCountExceededLimit State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IncrementAndCheckIfDuplicateSerialNumberRetryCountExceededLimit(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfAddDeviceToGlobalListRetryCountExceededLimit


DSM_EVENT
HUBDSM_CreatingChildPDOAndReportingToPnp(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CreatingChildPDOAndReportingToPnp is called when the
    state machine enters the CreatingChildPDOAndReportingToPnp State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_CreatePdo(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CreatingChildPDOAndReportingToPnp


DSM_EVENT
HUBDSM_DiscardSerialNumber(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DiscardSerialNumber is called when the
    state machine enters the DiscardSerialNumber State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_DiscardSerialNumber(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DiscardSerialNumber


DSM_EVENT
HUBDSM_InitializingAddDeviceToGlobalListRetryCount(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_InitializingAddDeviceToGlobalListRetryCount is called when the
    state machine enters the InitializingAddDeviceToGlobalListRetryCount State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_InitializeDuplicateSerialNumberRetryCount(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_InitializingAddDeviceToGlobalListRetryCount


DSM_EVENT
HUBDSM_RequestingDeviceCycleInReportingToPnp(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_RequestingDeviceCycleInReportingToPnp is called when the
    state machine enters the RequestingDeviceCycleInReportingToPnp State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestCycle);

        HUBMISC_RemoveDeviceInfoFromGlobalChildList(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_RequestingDeviceCycleInReportingToPnp


DSM_EVENT
HUBDSM_ReturningOperationSuccessInReportingToPnp(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInReportingToPnp is called when the
    state machine enters the ReturningOperationSuccessInReportingToPnp State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInReportingToPnp


DSM_EVENT
HUBDSM_ReturningPortDetachDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDetachDevice is called when the
    state machine enters the ReturningPortDetachDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDetachDevice


DSM_EVENT
HUBDSM_StoppingTimerAndRequestingCycleOnStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppingTimerAndRequestingCycleOnStopSuspend is called when the
    state machine enters the StoppingTimerAndRequestingCycleOnStopSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestCycle);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_StoppingTimerAndRequestingCycleOnStopSuspend


DSM_EVENT
HUBDSM_StoppingTimerOnDetachInReportingToPnp(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppingTimerOnDetachInReportingToPnp is called when the
    state machine enters the StoppingTimerOnDetachInReportingToPnp State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_StoppingTimerOnDetachInReportingToPnp


DSM_EVENT
HUBDSM_WaitingForDetachInReportingToPnp(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDetachInReportingToPnp is called when the
    state machine enters the WaitingForDetachInReportingToPnp State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDetachInReportingToPnp


DSM_EVENT
HUBDSM_WaitingForDetachOrTimerInReportingToPnp(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDetachOrTimerInReportingToPnp is called when the
    state machine enters the WaitingForDetachOrTimerInReportingToPnp State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDetachOrTimerInReportingToPnp


DSM_EVENT
HUBDSM_WaitingForDuplicateDeviceToGoAway(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDuplicateDeviceToGoAway is called when the
    state machine enters the WaitingForDuplicateDeviceToGoAway State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StartDuplicateDeviceTimer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForDuplicateDeviceToGoAway


DSM_EVENT
HUBDSM_WaitingForTimerToFlushOnDetachInReportingToPnp(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerToFlushOnDetachInReportingToPnp is called when the
    state machine enters the WaitingForTimerToFlushOnDetachInReportingToPnp State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerToFlushOnDetachInReportingToPnp


DSM_EVENT
HUBDSM_CreatingUnknownChildPDOAndReportingToPnp(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CreatingUnknownChildPDOAndReportingToPnp is called when the
    state machine enters the CreatingUnknownChildPDOAndReportingToPnp State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_CreateUnknownPdo(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CreatingUnknownChildPDOAndReportingToPnp


DSM_EVENT
HUBDSM_MarkingUnknownDeviceAsFailed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_MarkingUnknownDeviceAsFailed is called when the
    state machine enters the MarkingUnknownDeviceAsFailed State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_ReportDeviceFailure(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_MarkingUnknownDeviceAsFailed


DSM_EVENT
HUBDSM_RequestingDeviceCycleForUnknownDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_RequestingDeviceCycleForUnknownDevice is called when the
    state machine enters the RequestingDeviceCycleForUnknownDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestCycle);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_RequestingDeviceCycleForUnknownDevice


DSM_EVENT
HUBDSM_ReturningOperationSuccessInReportingUnknownDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInReportingUnknownDevice is called when the
    state machine enters the ReturningOperationSuccessInReportingUnknownDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInReportingUnknownDevice


DSM_EVENT
HUBDSM_ReturningPortDetachDeviceInReportingUnknownDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDetachDeviceInReportingUnknownDevice is called when the
    state machine enters the ReturningPortDetachDeviceInReportingUnknownDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDetachDeviceInReportingUnknownDevice


DSM_EVENT
HUBDSM_AckingResumed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AckingResumed is called when the
    state machine enters the AckingResumed State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestResume);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AckingResumed


DSM_EVENT
HUBDSM_AcquiringPowerReferenceInEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AcquiringPowerReferenceInEnabled is called when the
    state machine enters the AcquiringPowerReferenceInEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AcquiringPowerReferenceInEnabled


DSM_EVENT
HUBDSM_AcquiringPowerReferenceInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AcquiringPowerReferenceInSuspended is called when the
    state machine enters the AcquiringPowerReferenceInSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AcquiringPowerReferenceInSuspended


DSM_EVENT
HUBDSM_AcquiringPowerReferenceInSuspendedOnResumeWithReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AcquiringPowerReferenceInSuspendedOnResumeWithReset is called when the
    state machine enters the AcquiringPowerReferenceInSuspendedOnResumeWithReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AcquiringPowerReferenceInSuspendedOnResumeWithReset


DSM_EVENT
HUBDSM_AcquiringPowerReferenceOnHubResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AcquiringPowerReferenceOnHubResume is called when the
    state machine enters the AcquiringPowerReferenceOnHubResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AcquiringPowerReferenceOnHubResume


DSM_EVENT
HUBDSM_AcquiringPowerReferenceOnHubResumeWithReenumRequired(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AcquiringPowerReferenceOnHubResumeWithReenumRequired is called when the
    state machine enters the AcquiringPowerReferenceOnHubResumeWithReenumRequired State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AcquiringPowerReferenceOnHubResumeWithReenumRequired


DSM_EVENT
HUBDSM_AcquiringPowerReferenceOnHubResumeWtihReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AcquiringPowerReferenceOnHubResumeWtihReset is called when the
    state machine enters the AcquiringPowerReferenceOnHubResumeWtihReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AcquiringPowerReferenceOnHubResumeWtihReset


DSM_EVENT
HUBDSM_AcquiringPowerReferenceOnResumeInS0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AcquiringPowerReferenceOnResumeInS0 is called when the
    state machine enters the AcquiringPowerReferenceOnResumeInS0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AcquiringPowerReferenceOnResumeInS0


DSM_EVENT
HUBDSM_CheckingIfDeviceNeedsResetOnResumeInSxOnEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDeviceNeedsResetOnResumeInSxOnEnabled is called when the
    state machine enters the CheckingIfDeviceNeedsResetOnResumeInSxOnEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsResetOnResumeInSxSet(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDeviceNeedsResetOnResumeInSxOnEnabled


DSM_EVENT
HUBDSM_CheckingIfDeviceNeedsResetOnResumeInSxOnSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDeviceNeedsResetOnResumeInSxOnSuspended is called when the
    state machine enters the CheckingIfDeviceNeedsResetOnResumeInSxOnSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsResetOnResumeInSxSet(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDeviceNeedsResetOnResumeInSxOnSuspended


DSM_EVENT
HUBDSM_CompletingUnexpectedD0ExitInHubSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingUnexpectedD0ExitInHubSuspended is called when the
    state machine enters the CompletingUnexpectedD0ExitInHubSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingUnexpectedD0ExitInHubSuspended


DSM_EVENT
HUBDSM_DisabledOrFailedInHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisabledOrFailedInHubSuspend is called when the
    state machine enters the DisabledOrFailedInHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_DisabledOrFailedInHubSuspend


DSM_EVENT
HUBDSM_DisablingPortOnPortEnabledInPendingHubStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingPortOnPortEnabledInPendingHubStop is called when the
    state machine enters the DisablingPortOnPortEnabledInPendingHubStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingPortOnPortEnabledInPendingHubStop


DSM_EVENT
HUBDSM_ReleasingReferenceOnHubSuspendAfterPSMSyncUp(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingReferenceOnHubSuspendAfterPSMSyncUp is called when the
    state machine enters the ReleasingReferenceOnHubSuspendAfterPSMSyncUp State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingReferenceOnHubSuspendAfterPSMSyncUp


DSM_EVENT
HUBDSM_ReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed is called when the
    state machine enters the ReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed


DSM_EVENT
HUBDSM_ReleasingReferenceOnHubSuspendOnDetachAfterPSMSyncUp(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingReferenceOnHubSuspendOnDetachAfterPSMSyncUp is called when the
    state machine enters the ReleasingReferenceOnHubSuspendOnDetachAfterPSMSyncUp State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingReferenceOnHubSuspendOnDetachAfterPSMSyncUp


DSM_EVENT
HUBDSM_ResumingBeforeResetting(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ResumingBeforeResetting is called when the
    state machine enters the ResumingBeforeResetting State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestResume);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ResumingBeforeResetting


DSM_EVENT
HUBDSM_ReturningDeviceRenumerationOnHubResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningDeviceRenumerationOnHubResume is called when the
    state machine enters the ReturningDeviceRenumerationOnHubResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventDeviceRenumerateOnHubResume;

} // HUBDSM_ReturningDeviceRenumerationOnHubResume


DSM_EVENT
HUBDSM_ReturningDeviceResumedOnHubResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningDeviceResumedOnHubResume is called when the
    state machine enters the ReturningDeviceResumedOnHubResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventDeviceResumedOnHubResume;

} // HUBDSM_ReturningDeviceResumedOnHubResume


DSM_EVENT
HUBDSM_ReturningDeviceSuspendedOnHubResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningDeviceSuspendedOnHubResume is called when the
    state machine enters the ReturningDeviceSuspendedOnHubResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventDeviceSuspendedOnHubResume;

} // HUBDSM_ReturningDeviceSuspendedOnHubResume


DSM_EVENT
HUBDSM_ReturningHubStopWithReferenceAcquiredInHubSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubStopWithReferenceAcquiredInHubSuspended is called when the
    state machine enters the ReturningHubStopWithReferenceAcquiredInHubSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubStopWithReferenceAcquired;

} // HUBDSM_ReturningHubStopWithReferenceAcquiredInHubSuspended


DSM_EVENT
HUBDSM_ReturningPortDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDetach is called when the
    state machine enters the ReturningPortDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDetach


DSM_EVENT
HUBDSM_SuspendedInHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SuspendedInHubSuspend is called when the
    state machine enters the SuspendedInHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_SuspendedInHubSuspend


DSM_EVENT
HUBDSM_SuspendingPortOnPortEnabledInPendingHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SuspendingPortOnPortEnabledInPendingHubSuspend is called when the
    state machine enters the SuspendingPortOnPortEnabledInPendingHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestSuspend);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SuspendingPortOnPortEnabledInPendingHubSuspend


DSM_EVENT
HUBDSM_WaitingForHubResumeInEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForHubResumeInEnabled is called when the
    state machine enters the WaitingForHubResumeInEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForHubResumeInEnabled


DSM_EVENT
HUBDSM_WaitingForHubResumeInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForHubResumeInSuspended is called when the
    state machine enters the WaitingForHubResumeInSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForHubResumeInSuspended


DSM_EVENT
HUBDSM_WaitingForHubResumeWithRenumRequired(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForHubResumeWithRenumRequired is called when the
    state machine enters the WaitingForHubResumeWithRenumRequired State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForHubResumeWithRenumRequired


DSM_EVENT
HUBDSM_WaitingForPSMSyncUp(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPSMSyncUp is called when the
    state machine enters the WaitingForPSMSyncUp State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPSMSyncUp


DSM_EVENT
HUBDSM_WaitingForPSMSyncUpOnPendingStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPSMSyncUpOnPendingStop is called when the
    state machine enters the WaitingForPSMSyncUpOnPendingStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPSMSyncUpOnPendingStop


DSM_EVENT
HUBDSM_WaitingForPSMSyncUpOnPendingSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPSMSyncUpOnPendingSuspend is called when the
    state machine enters the WaitingForPSMSyncUpOnPendingSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPSMSyncUpOnPendingSuspend


DSM_EVENT
HUBDSM_WaitingForPSMSyncUpOnResumeWithReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPSMSyncUpOnResumeWithReset is called when the
    state machine enters the WaitingForPSMSyncUpOnResumeWithReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPSMSyncUpOnResumeWithReset


DSM_EVENT
HUBDSM_WaitingForResumeResponseFromPortOnHubStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForResumeResponseFromPortOnHubStop is called when the
    state machine enters the WaitingForResumeResponseFromPortOnHubStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForResumeResponseFromPortOnHubStop


DSM_EVENT
HUBDSM_WaitingForResumeResponseFromPortOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForResumeResponseFromPortOnHubSuspend is called when the
    state machine enters the WaitingForResumeResponseFromPortOnHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForResumeResponseFromPortOnHubSuspend


DSM_EVENT
HUBDSM_AcquiringAddressZeroOwnershipOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AcquiringAddressZeroOwnershipOnRenum is called when the
    state machine enters the AcquiringAddressZeroOwnershipOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_AcquireAddress0OwnershipUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_AcquiringAddressZeroOwnershipOnRenum


DSM_EVENT
HUBDSM_AcquiringPowerReferenceOnStartInReEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AcquiringPowerReferenceOnStartInReEnum is called when the
    state machine enters the AcquiringPowerReferenceOnStartInReEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AcquiringPowerReferenceOnStartInReEnum


DSM_EVENT
HUBDSM_CancellingAcquiringAddress0OnDetachOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CancellingAcquiringAddress0OnDetachOnRenum is called when the
    state machine enters the CancellingAcquiringAddress0OnDetachOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_CancelUCXIoctl(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CancellingAcquiringAddress0OnDetachOnRenum


DSM_EVENT
HUBDSM_CancellingRetryTimerOnDetachOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CancellingRetryTimerOnDetachOnRenum is called when the
    state machine enters the CancellingRetryTimerOnDetachOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CancellingRetryTimerOnDetachOnRenum


DSM_EVENT
HUBDSM_CancellingRetryTimerOnStopSuspendOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CancellingRetryTimerOnStopSuspendOnRenum is called when the
    state machine enters the CancellingRetryTimerOnStopSuspendOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CancellingRetryTimerOnStopSuspendOnRenum


DSM_EVENT
HUBDSM_CheckingIfAltEnumCmdNeededInReenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfAltEnumCmdNeededInReenum is called when the
    state machine enters the CheckingIfAltEnumCmdNeededInReenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsMsOs20AltEnumCmdNeededOnReenum(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfAltEnumCmdNeededInReenum


DSM_EVENT
HUBDSM_CheckingIfDevicePDChargingPolicySupportedInReEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDevicePDChargingPolicySupportedInReEnum is called when the
    state machine enters the CheckingIfDevicePDChargingPolicySupportedInReEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfDevicePDChargingPolicySupported(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDevicePDChargingPolicySupportedInReEnum


DSM_EVENT
HUBDSM_CheckingIfDevicePorgrammingWasLosttInUnconfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDevicePorgrammingWasLosttInUnconfigured is called when the
    state machine enters the CheckingIfDevicePorgrammingWasLosttInUnconfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfDeviceProgrammingWasLost(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDevicePorgrammingWasLosttInUnconfigured


DSM_EVENT
HUBDSM_CheckingIfDeviceProgrammingWasLosttInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDeviceProgrammingWasLosttInConfigured is called when the
    state machine enters the CheckingIfDeviceProgrammingWasLosttInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfDeviceProgrammingWasLost(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDeviceProgrammingWasLosttInConfigured


DSM_EVENT
HUBDSM_CheckingIfEnumRetryReachedMaximumOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfEnumRetryReachedMaximumOnRenum is called when the
    state machine enters the CheckingIfEnumRetryReachedMaximumOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IncrementEnumRetryCountAndCheckIfReachedMax(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfEnumRetryReachedMaximumOnRenum


DSM_EVENT
HUBDSM_CheckingIfEnumRetryReachedMaximumOnRenumWithAddress0Ownership(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfEnumRetryReachedMaximumOnRenumWithAddress0Ownership is called when the
    state machine enters the CheckingIfEnumRetryReachedMaximumOnRenumWithAddress0Ownership State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IncrementEnumRetryCountAndCheckIfReachedMax(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfEnumRetryReachedMaximumOnRenumWithAddress0Ownership


DSM_EVENT
HUBDSM_CheckingIfEnumRetryReachedMaximumWithDeviceDisabledOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfEnumRetryReachedMaximumWithDeviceDisabledOnRenum is called when the
    state machine enters the CheckingIfEnumRetryReachedMaximumWithDeviceDisabledOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IncrementEnumRetryCountAndCheckIfReachedMax(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfEnumRetryReachedMaximumWithDeviceDisabledOnRenum


DSM_EVENT
HUBDSM_CheckingIfItIsBootDeviceOnIdComparisionFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfItIsBootDeviceOnIdComparisionFailure is called when the
    state machine enters the CheckingIfItIsBootDeviceOnIdComparisionFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_IsItABootDevice(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfItIsBootDeviceOnIdComparisionFailure


DSM_EVENT
HUBDSM_CheckingIfRequestConfigDescOnResetIsSet(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfRequestConfigDescOnResetIsSet is called when the
    state machine enters the CheckingIfRequestConfigDescOnResetIsSet State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_QueryCachedRegistryIfRequestConfigDescOnResetIsSet(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfRequestConfigDescOnResetIsSet


DSM_EVENT
HUBDSM_CheckingIfSerialNumberShouldBeCompared(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfSerialNumberShouldBeCompared is called when the
    state machine enters the CheckingIfSerialNumberShouldBeCompared State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfSerialNumberShouldBeQueriedOnRenumeration(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfSerialNumberShouldBeCompared


DSM_EVENT
HUBDSM_ClearingDeviceLostProgammingFlagOnRestart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ClearingDeviceLostProgammingFlagOnRestart is called when the
    state machine enters the ClearingDeviceLostProgammingFlagOnRestart State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_ClearDeviceNeedsReprogrammingFlag(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ClearingDeviceLostProgammingFlagOnRestart


DSM_EVENT
HUBDSM_ClearingDeviceProgrammingLostFlagOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ClearingDeviceProgrammingLostFlagOnRenum is called when the
    state machine enters the ClearingDeviceProgrammingLostFlagOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfDeviceProgrammingWasLost(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ClearingDeviceProgrammingLostFlagOnRenum


DSM_EVENT
HUBDSM_ComparingDeviceOnReEnumeration(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ComparingDeviceOnReEnumeration is called when the
    state machine enters the ComparingDeviceOnReEnumeration State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfSameDeviceIsConnected(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ComparingDeviceOnReEnumeration


DSM_EVENT
HUBDSM_ComparingSerialNumberOnReEnumeration(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ComparingSerialNumberOnReEnumeration is called when the
    state machine enters the ComparingSerialNumberOnReEnumeration State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfSerialNumberIsIdentical(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ComparingSerialNumberOnReEnumeration


DSM_EVENT
HUBDSM_DeconfiguringEndpointsInControllerBeforeRenumerating(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeconfiguringEndpointsInControllerBeforeRenumerating is called when the
    state machine enters the DeconfiguringEndpointsInControllerBeforeRenumerating State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DeconfiguringEndpointsInControllerBeforeRenumerating


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnStopSuspendOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnStopSuspendOnRenum is called when the
    state machine enters the DisablingDeviceInControllerOnStopSuspendOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnStopSuspendOnRenum


DSM_EVENT
HUBDSM_DisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership is called when the
    state machine enters the DisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership


DSM_EVENT
HUBDSM_DisablingOnEnumAfterFailureInReEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingOnEnumAfterFailureInReEnum is called when the
    state machine enters the DisablingOnEnumAfterFailureInReEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingOnEnumAfterFailureInReEnum


DSM_EVENT
HUBDSM_DisablingOnEnumAfterFailureInReEnumWithAddress0Ownership(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingOnEnumAfterFailureInReEnumWithAddress0Ownership is called when the
    state machine enters the DisablingOnEnumAfterFailureInReEnumWithAddress0Ownership State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingOnEnumAfterFailureInReEnumWithAddress0Ownership


DSM_EVENT
HUBDSM_DisablingOnEnumAfterFailureWithDeviceDisabledInReEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingOnEnumAfterFailureWithDeviceDisabledInReEnum is called when the
    state machine enters the DisablingOnEnumAfterFailureWithDeviceDisabledInReEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SetFailIo(deviceContext);

        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingOnEnumAfterFailureWithDeviceDisabledInReEnum


DSM_EVENT
HUBDSM_EnablingDeviceInControllerInRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_EnablingDeviceInControllerInRenum is called when the
    state machine enters the EnablingDeviceInControllerInRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_EnableDeviceAndDefaultEndpointUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_EnablingDeviceInControllerInRenum


DSM_EVENT
HUBDSM_EnumeratingAtAddressZeroOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_EnumeratingAtAddressZeroOnRenum is called when the
    state machine enters the EnumeratingAtAddressZeroOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_EnumeratingAtAddressZeroOnRenum


DSM_EVENT
HUBDSM_GettingConfigDescriptorWhileRenuemrating(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingConfigDescriptorWhileRenuemrating is called when the
    state machine enters the GettingConfigDescriptorWhileRenuemrating State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetConfigDescriptorWithDefaultSizeUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingConfigDescriptorWhileRenuemrating


DSM_EVENT
HUBDSM_GettingDeviceDescriptorOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingDeviceDescriptorOnRenum is called when the
    state machine enters the GettingDeviceDescriptorOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetDeviceDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingDeviceDescriptorOnRenum


DSM_EVENT
HUBDSM_GettingSerialNumberStringDescriptorWhileRenuemrating(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingSerialNumberStringDescriptorWhileRenuemrating is called when the
    state machine enters the GettingSerialNumberStringDescriptorWhileRenuemrating State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetSerialNumberStringDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingSerialNumberStringDescriptorWhileRenuemrating


DSM_EVENT
HUBDSM_InitializingEnumRetryCountInReEnumDuringResetInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_InitializingEnumRetryCountInReEnumDuringResetInConfigured is called when the
    state machine enters the InitializingEnumRetryCountInReEnumDuringResetInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_InitializeEnumRetryCount(deviceContext);

        HUBMISC_LogDeviceReEnumeration(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_InitializingEnumRetryCountInReEnumDuringResetInConfigured


DSM_EVENT
HUBDSM_InitializingEnumRetryCountInReEnumDuringResetInUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_InitializingEnumRetryCountInReEnumDuringResetInUnConfigured is called when the
    state machine enters the InitializingEnumRetryCountInReEnumDuringResetInUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_InitializeEnumRetryCount(deviceContext);

        HUBMISC_LogDeviceReEnumeration(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_InitializingEnumRetryCountInReEnumDuringResetInUnConfigured


DSM_EVENT
HUBDSM_InitializingEnumRetryCountInReEnumDuringRestart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_InitializingEnumRetryCountInReEnumDuringRestart is called when the
    state machine enters the InitializingEnumRetryCountInReEnumDuringRestart State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_InitializeEnumRetryCount(deviceContext);

        HUBMISC_LogDeviceReEnumeration(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_InitializingEnumRetryCountInReEnumDuringRestart


DSM_EVENT
HUBDSM_PurgingDeviceTreeIoOnReEnumerationInConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PurgingDeviceTreeIoOnReEnumerationInConfigured is called when the
    state machine enters the PurgingDeviceTreeIoOnReEnumerationInConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_PurgeDeviceTreeIoUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_PurgingDeviceTreeIoOnReEnumerationInConfigured


DSM_EVENT
HUBDSM_PurgingDeviceTreeIoOnReEnumerationInUnConfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PurgingDeviceTreeIoOnReEnumerationInUnConfigured is called when the
    state machine enters the PurgingDeviceTreeIoOnReEnumerationInUnConfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_PurgeDeviceTreeIoUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_PurgingDeviceTreeIoOnReEnumerationInUnConfigured


DSM_EVENT
HUBDSM_PurgingIoOnEnumAfterFailureInReEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PurgingIoOnEnumAfterFailureInReEnum is called when the
    state machine enters the PurgingIoOnEnumAfterFailureInReEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SetFailIo(deviceContext);

        HUBUCX_PurgeDeviceIoUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_PurgingIoOnEnumAfterFailureInReEnum


DSM_EVENT
HUBDSM_PurgingIoOnEnumAfterFailureInReEnumWithAddress0Ownership(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PurgingIoOnEnumAfterFailureInReEnumWithAddress0Ownership is called when the
    state machine enters the PurgingIoOnEnumAfterFailureInReEnumWithAddress0Ownership State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SetFailIo(deviceContext);

        HUBUCX_PurgeDeviceIoUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_PurgingIoOnEnumAfterFailureInReEnumWithAddress0Ownership


DSM_EVENT
HUBDSM_ReleasingAddressZeroOwnershipOnDetachOnEnumFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingAddressZeroOwnershipOnDetachOnEnumFailure is called when the
    state machine enters the ReleasingAddressZeroOwnershipOnDetachOnEnumFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ReleaseAddress0OwnershipFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingAddressZeroOwnershipOnDetachOnEnumFailure


DSM_EVENT
HUBDSM_ReleasingAddressZeroOwnershipOnDetachWithDeviceEnabledOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingAddressZeroOwnershipOnDetachWithDeviceEnabledOnRenum is called when the
    state machine enters the ReleasingAddressZeroOwnershipOnDetachWithDeviceEnabledOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ReleaseAddress0OwnershipFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingAddressZeroOwnershipOnDetachWithDeviceEnabledOnRenum


DSM_EVENT
HUBDSM_ReleasingAddressZeroOwnershipOnDetachWithTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingAddressZeroOwnershipOnDetachWithTimer is called when the
    state machine enters the ReleasingAddressZeroOwnershipOnDetachWithTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ReleaseAddress0OwnershipFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingAddressZeroOwnershipOnDetachWithTimer


DSM_EVENT
HUBDSM_ReleasingAddressZeroOwnershipOnEnumFailureInRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingAddressZeroOwnershipOnEnumFailureInRenum is called when the
    state machine enters the ReleasingAddressZeroOwnershipOnEnumFailureInRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ReleaseAddress0OwnershipFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingAddressZeroOwnershipOnEnumFailureInRenum


DSM_EVENT
HUBDSM_ReleasingAddressZeroOwnershipOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingAddressZeroOwnershipOnRenum is called when the
    state machine enters the ReleasingAddressZeroOwnershipOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ReleaseAddress0OwnershipFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingAddressZeroOwnershipOnRenum


DSM_EVENT
HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendOnEnumFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendOnEnumFailure is called when the
    state machine enters the ReleasingAddressZeroOwnershipOnStopSuspendOnEnumFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ReleaseAddress0OwnershipFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendOnEnumFailure


DSM_EVENT
HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendOnRenum is called when the
    state machine enters the ReleasingAddressZeroOwnershipOnStopSuspendOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ReleaseAddress0OwnershipFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendOnRenum


DSM_EVENT
HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendWithTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendWithTimer is called when the
    state machine enters the ReleasingAddressZeroOwnershipOnStopSuspendWithTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ReleaseAddress0OwnershipFromUCX(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendWithTimer


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnRenum is called when the
    state machine enters the ReleasingPowerReferenceOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnRenum


DSM_EVENT
HUBDSM_ReturningOperationFailureOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureOnRenum is called when the
    state machine enters the ReturningOperationFailureOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureOnRenum


DSM_EVENT
HUBDSM_ReturningOperationFailureWithDeviceEnabledOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureWithDeviceEnabledOnRenum is called when the
    state machine enters the ReturningOperationFailureWithDeviceEnabledOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailureWithDeviceEnabled;

} // HUBDSM_ReturningOperationFailureWithDeviceEnabledOnRenum


DSM_EVENT
HUBDSM_ReturningOperationSuccessOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessOnRenum is called when the
    state machine enters the ReturningOperationSuccessOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessOnRenum


DSM_EVENT
HUBDSM_ReturningPortDetachDeviceOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDetachDeviceOnRenum is called when the
    state machine enters the ReturningPortDetachDeviceOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDetachDeviceOnRenum


DSM_EVENT
HUBDSM_ReturningPortDetachDeviceWithDeviceEnabledOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDetachDeviceWithDeviceEnabledOnRenum is called when the
    state machine enters the ReturningPortDetachDeviceWithDeviceEnabledOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDeviceWithDeviceEnabled;

} // HUBDSM_ReturningPortDetachDeviceWithDeviceEnabledOnRenum


DSM_EVENT
HUBDSM_SendingMsOs20AltEnumCmdOnReenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SendingMsOs20AltEnumCmdOnReenum is called when the
    state machine enters the SendingMsOs20AltEnumCmdOnReenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_SendMsOs20AltEnumCommandUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SendingMsOs20AltEnumCmdOnReenum


DSM_EVENT
HUBDSM_SettingDevicePDChargingPolicyInReEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingDevicePDChargingPolicyInReEnum is called when the
    state machine enters the SettingDevicePDChargingPolicyInReEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_SetDevicePDChargingPolicyUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SettingDevicePDChargingPolicyInReEnum


DSM_EVENT
HUBDSM_SettingLPMValuesInReEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingLPMValuesInReEnum is called when the
    state machine enters the SettingLPMValuesInReEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_SettingLPMValuesInReEnum


DSM_EVENT
HUBDSM_SettingNonZeroAddressOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingNonZeroAddressOnRenum is called when the
    state machine enters the SettingNonZeroAddressOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_SettingNonZeroAddressOnRenum


DSM_EVENT
HUBDSM_StartingTimerForEnumRetryOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StartingTimerForEnumRetryOnRenum is called when the
    state machine enters the StartingTimerForEnumRetryOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StartEnumRetryTimer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_StartingTimerForEnumRetryOnRenum


DSM_EVENT
HUBDSM_StartingTimerForEnumRetryOnRenumWithAddress0Ownership(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StartingTimerForEnumRetryOnRenumWithAddress0Ownership is called when the
    state machine enters the StartingTimerForEnumRetryOnRenumWithAddress0Ownership State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StartEnumRetryTimer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_StartingTimerForEnumRetryOnRenumWithAddress0Ownership


DSM_EVENT
HUBDSM_StoppedSuspendedOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppedSuspendedOnRenum is called when the
    state machine enters the StoppedSuspendedOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_StoppedSuspendedOnRenum


DSM_EVENT
HUBDSM_WaitingForDetachAfterWrongDeviceDetectedOnBootDevicePort(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDetachAfterWrongDeviceDetectedOnBootDevicePort is called when the
    state machine enters the WaitingForDetachAfterWrongDeviceDetectedOnBootDevicePort State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_NotifyDifferentDeviceAttachedOnBootDevicePort(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForDetachAfterWrongDeviceDetectedOnBootDevicePort


DSM_EVENT
HUBDSM_WaitingForDisableCompleteOnStopSuspendAfterFailureInReEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDisableCompleteOnStopSuspendAfterFailureInReEnum is called when the
    state machine enters the WaitingForDisableCompleteOnStopSuspendAfterFailureInReEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDisableCompleteOnStopSuspendAfterFailureInReEnum


DSM_EVENT
HUBDSM_WaitingForDisableCompleteOnStopSuspendOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDisableCompleteOnStopSuspendOnRenum is called when the
    state machine enters the WaitingForDisableCompleteOnStopSuspendOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForDisableCompleteOnStopSuspendOnRenum


DSM_EVENT
HUBDSM_WaitingForDisableCompleteOnStopSuspendWithDeviceDisabledAfterFailureInReEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDisableCompleteOnStopSuspendWithDeviceDisabledAfterFailureInReEnum is called when the
    state machine enters the WaitingForDisableCompleteOnStopSuspendWithDeviceDisabledAfterFailureInReEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDisableCompleteOnStopSuspendWithDeviceDisabledAfterFailureInReEnum


DSM_EVENT
HUBDSM_WaitingForTimerToFlushOnDetachOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerToFlushOnDetachOnRenum is called when the
    state machine enters the WaitingForTimerToFlushOnDetachOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerToFlushOnDetachOnRenum


DSM_EVENT
HUBDSM_WaitingForTimerToFlushOnStopSuspendOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerToFlushOnStopSuspendOnRenum is called when the
    state machine enters the WaitingForTimerToFlushOnStopSuspendOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerToFlushOnStopSuspendOnRenum


DSM_EVENT
HUBDSM_WaitingForUCXIoctlOnDetachOnRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForUCXIoctlOnDetachOnRenum is called when the
    state machine enters the WaitingForUCXIoctlOnDetachOnRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForUCXIoctlOnDetachOnRenum


DSM_EVENT
HUBDSM_WaitingForUCXIoctlOnStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForUCXIoctlOnStopSuspend is called when the
    state machine enters the WaitingForUCXIoctlOnStopSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_CancelUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForUCXIoctlOnStopSuspend


DSM_EVENT
HUBDSM_CheckingIfAnyAlternateInterfaceLeft(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfAnyAlternateInterfaceLeft is called when the
    state machine enters the CheckingIfAnyAlternateInterfaceLeft State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_CheckIfAlternateInterfaceLeftToBeSet(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfAnyAlternateInterfaceLeft


DSM_EVENT
HUBDSM_ConfiguringDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ConfiguringDevice is called when the
    state machine enters the ConfiguringDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_SetDeviceConfigurationUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ConfiguringDevice


DSM_EVENT
HUBDSM_CreatingNewEndpoints(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CreatingNewEndpoints is called when the
    state machine enters the CreatingNewEndpoints State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBUCX_CreateEndpointsInUCX(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CreatingNewEndpoints


DSM_EVENT
HUBDSM_DeconfiguringEndpointsInControllerForDeviceConfiguration(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeconfiguringEndpointsInControllerForDeviceConfiguration is called when the
    state machine enters the DeconfiguringEndpointsInControllerForDeviceConfiguration State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableEndpointsInPendingDisableListUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DeconfiguringEndpointsInControllerForDeviceConfiguration


DSM_EVENT
HUBDSM_DeletingEndpointsForOldConfiguration(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingEndpointsForOldConfiguration is called when the
    state machine enters the DeletingEndpointsForOldConfiguration State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteEndpointsAndFreeResourcesForOldConfiguration(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingEndpointsForOldConfiguration


DSM_EVENT
HUBDSM_MarkingAnyEndpointsToBeEnableAsDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_MarkingAnyEndpointsToBeEnableAsDisabled is called when the
    state machine enters the MarkingAnyEndpointsToBeEnableAsDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_MarkEndpointsToBeEnabledAsDisabled(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_MarkingAnyEndpointsToBeEnableAsDisabled


DSM_EVENT
HUBDSM_ProgrammingEndpointsAndSettingLPMValuesrForDeviceConfiguration(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ProgrammingEndpointsAndSettingLPMValuesrForDeviceConfiguration is called when the
    state machine enters the ProgrammingEndpointsAndSettingLPMValuesrForDeviceConfiguration State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ProgrammingEndpointsAndSettingLPMValuesrForDeviceConfiguration


DSM_EVENT
HUBDSM_ProgrammingEndpointsInControllerForDeviceConfigurationFor20Devices(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ProgrammingEndpointsInControllerForDeviceConfigurationFor20Devices is called when the
    state machine enters the ProgrammingEndpointsInControllerForDeviceConfigurationFor20Devices State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_EnableDisableEndpointsUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ProgrammingEndpointsInControllerForDeviceConfigurationFor20Devices


DSM_EVENT
HUBDSM_ReturningOperationFailureInReConfiguring(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureInReConfiguring is called when the
    state machine enters the ReturningOperationFailureInReConfiguring State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureInReConfiguring


DSM_EVENT
HUBDSM_ReturningOperationSuccessInReConfiguring(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInReConfiguring is called when the
    state machine enters the ReturningOperationSuccessInReConfiguring State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInReConfiguring


DSM_EVENT
HUBDSM_SettingDeviceInterfaceInConfiguring(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingDeviceInterfaceInConfiguring is called when the
    state machine enters the SettingDeviceInterfaceInConfiguring State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_SetDeviceInterfaceUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SettingDeviceInterfaceInConfiguring


DSM_EVENT
HUBDSM_DisablingPortOnHubSuspendInCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingPortOnHubSuspendInCleanup is called when the
    state machine enters the DisablingPortOnHubSuspendInCleanup State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingPortOnHubSuspendInCleanup


DSM_EVENT
HUBDSM_FlushingHubPowerDownEvents(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingHubPowerDownEvents is called when the
    state machine enters the FlushingHubPowerDownEvents State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingHubPowerDownEvents


DSM_EVENT
HUBDSM_FlushingHubPowerUpEvents(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingHubPowerUpEvents is called when the
    state machine enters the FlushingHubPowerUpEvents State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingHubPowerUpEvents


DSM_EVENT
HUBDSM_FlushingHubStopAfterSuspendEvent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingHubStopAfterSuspendEvent is called when the
    state machine enters the FlushingHubStopAfterSuspendEvent State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AckStopAfterSuspend(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingHubStopAfterSuspendEvent


DSM_EVENT
HUBDSM_FlushingPnpEvents(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingPnpEvents is called when the
    state machine enters the FlushingPnpEvents State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerFailureEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingPnpEvents


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEvents(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEvents is called when the
    state machine enters the FowardingStreamsRequestToUcxInWaitingForDevicePortEvents State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEvents


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnHubSuspendInCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnHubSuspendInCleanup is called when the
    state machine enters the ReleasingPowerReferenceOnHubSuspendInCleanup State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnHubSuspendInCleanup


DSM_EVENT
HUBDSM_ReturningPortDetachDeviceFromIgnoringDevicePort(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDetachDeviceFromIgnoringDevicePort is called when the
    state machine enters the ReturningPortDetachDeviceFromIgnoringDevicePort State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDetachDeviceFromIgnoringDevicePort


DSM_EVENT
HUBDSM_ReturningPortDisabledOnHubSuspendInCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDisabledOnHubSuspendInCleanup is called when the
    state machine enters the ReturningPortDisabledOnHubSuspendInCleanup State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDisabledOnHubSuspend;

} // HUBDSM_ReturningPortDisabledOnHubSuspendInCleanup


DSM_EVENT
HUBDSM_WaitingForDevicePortEvents(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDevicePortEvents is called when the
    state machine enters the WaitingForDevicePortEvents State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDevicePortEvents


DSM_EVENT
HUBDSM_FlushingHubPowerDownEventsWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingHubPowerDownEventsWithPortOff is called when the
    state machine enters the FlushingHubPowerDownEventsWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingHubPowerDownEventsWithPortOff


DSM_EVENT
HUBDSM_FlushingHubPowerUpEventsWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingHubPowerUpEventsWithPortOff is called when the
    state machine enters the FlushingHubPowerUpEventsWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingHubPowerUpEventsWithPortOff


DSM_EVENT
HUBDSM_FlushingHubStopAfterSuspendEventWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingHubStopAfterSuspendEventWithPortOff is called when the
    state machine enters the FlushingHubStopAfterSuspendEventWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AckStopAfterSuspend(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingHubStopAfterSuspendEventWithPortOff


DSM_EVENT
HUBDSM_FlushingPnpEventsWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingPnpEventsWithPortOff is called when the
    state machine enters the FlushingPnpEventsWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerFailureEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingPnpEventsWithPortOff


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEventsWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEventsWithPortOff is called when the
    state machine enters the FowardingStreamsRequestToUcxInWaitingForDevicePortEventsWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEventsWithPortOff


DSM_EVENT
HUBDSM_WaitingForDevicePortEventsWithPortOff(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDevicePortEventsWithPortOff is called when the
    state machine enters the WaitingForDevicePortEventsWithPortOff State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDevicePortEventsWithPortOff


DSM_EVENT
HUBDSM_DisablingPortOnHubSuspendForFailedDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingPortOnHubSuspendForFailedDevice is called when the
    state machine enters the DisablingPortOnHubSuspendForFailedDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingPortOnHubSuspendForFailedDevice


DSM_EVENT
HUBDSM_FailedDeviceHubSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FailedDeviceHubSuspended is called when the
    state machine enters the FailedDeviceHubSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FailedDeviceHubSuspended


DSM_EVENT
HUBDSM_FlushingHubPowerDownEventsForFailedDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingHubPowerDownEventsForFailedDevice is called when the
    state machine enters the FlushingHubPowerDownEventsForFailedDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingHubPowerDownEventsForFailedDevice


DSM_EVENT
HUBDSM_FlushingHubPowerUpEventsForFailedDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingHubPowerUpEventsForFailedDevice is called when the
    state machine enters the FlushingHubPowerUpEventsForFailedDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingHubPowerUpEventsForFailedDevice


DSM_EVENT
HUBDSM_FlushingHubStopAfterSuspendEventForFailedDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingHubStopAfterSuspendEventForFailedDevice is called when the
    state machine enters the FlushingHubStopAfterSuspendEventForFailedDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AckStopAfterSuspend(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingHubStopAfterSuspendEventForFailedDevice


DSM_EVENT
HUBDSM_FlushingPnpEventsForFailedDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingPnpEventsForFailedDevice is called when the
    state machine enters the FlushingPnpEventsForFailedDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingPnpEventsForFailedDevice


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnDetachForFailedDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnDetachForFailedDevice is called when the
    state machine enters the ReleasingPowerReferenceOnDetachForFailedDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnDetachForFailedDevice


DSM_EVENT
HUBDSM_ReturningPortDetachDeviceForFailedDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDetachDeviceForFailedDevice is called when the
    state machine enters the ReturningPortDetachDeviceForFailedDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDetachDeviceForFailedDevice


DSM_EVENT
HUBDSM_WaitingForDevicePortEventsForFailedDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDevicePortEventsForFailedDevice is called when the
    state machine enters the WaitingForDevicePortEventsForFailedDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDevicePortEventsForFailedDevice


DSM_EVENT
HUBDSM_FlushingHubStartResumeEvent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingHubStartResumeEvent is called when the
    state machine enters the FlushingHubStartResumeEvent State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingHubStartResumeEvent


DSM_EVENT
HUBDSM_FlushingHubSuspendEvent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingHubSuspendEvent is called when the
    state machine enters the FlushingHubSuspendEvent State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingHubSuspendEvent


DSM_EVENT
HUBDSM_FlushingPnpEventExceptStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingPnpEventExceptStop is called when the
    state machine enters the FlushingPnpEventExceptStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingPnpEventExceptStop


DSM_EVENT
HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEventsExceptStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEventsExceptStop is called when the
    state machine enters the FowardingStreamsRequestToUcxInWaitingForDevicePortEventsExceptStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_ForwardRequestToUCX(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEventsExceptStop


DSM_EVENT
HUBDSM_WaitingForDevicePortEventsExceptStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDevicePortEventsExceptStop is called when the
    state machine enters the WaitingForDevicePortEventsExceptStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDevicePortEventsExceptStop


DSM_EVENT
HUBDSM_FlushingHubResumeEventForBootDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingHubResumeEventForBootDevice is called when the
    state machine enters the FlushingHubResumeEventForBootDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_AcquireDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingHubResumeEventForBootDevice


DSM_EVENT
HUBDSM_FlushingHubSuspendEventForBootDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingHubSuspendEventForBootDevice is called when the
    state machine enters the FlushingHubSuspendEventForBootDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingHubSuspendEventForBootDevice


DSM_EVENT
HUBDSM_FlushingPnpEventsForBootDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FlushingPnpEventsForBootDevice is called when the
    state machine enters the FlushingPnpEventsForBootDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FlushingPnpEventsForBootDevice


DSM_EVENT
HUBDSM_WaitingForDeviceHubEventsForBootDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForDeviceHubEventsForBootDevice is called when the
    state machine enters the WaitingForDeviceHubEventsForBootDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForDeviceHubEventsForBootDevice


DSM_EVENT
HUBDSM_AckingPortEventInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AckingPortEventInD3Cold is called when the
    state machine enters the AckingPortEventInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SetResetOnLastResumeFlag(deviceContext);

        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestResume);

        HUBPDO_CompleteWaitWake(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AckingPortEventInD3Cold


DSM_EVENT
HUBDSM_AckingPortResumed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AckingPortResumed is called when the
    state machine enters the AckingPortResumed State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestResume);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AckingPortResumed


DSM_EVENT
HUBDSM_CheckingIfDeviceArmedForWakeOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDeviceArmedForWakeOnResume is called when the
    state machine enters the CheckingIfDeviceArmedForWakeOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsDeviceArmedForWake(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDeviceArmedForWakeOnResume


DSM_EVENT
HUBDSM_CheckingIfDeviceNeedsResetOnResumeInS0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDeviceNeedsResetOnResumeInS0 is called when the
    state machine enters the CheckingIfDeviceNeedsResetOnResumeInS0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsResetOnResumeInS0Set(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDeviceNeedsResetOnResumeInS0


DSM_EVENT
HUBDSM_CheckingIfDeviceShouldBeDisarmedForWakeOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDeviceShouldBeDisarmedForWakeOnResume is called when the
    state machine enters the CheckingIfDeviceShouldBeDisarmedForWakeOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfDeviceShouldBeDisarmedForWakeOnResume(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDeviceShouldBeDisarmedForWakeOnResume


DSM_EVENT
HUBDSM_CheckingIfResetOnLastResumeFlagIsSet(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfResetOnLastResumeFlagIsSet is called when the
    state machine enters the CheckingIfResetOnLastResumeFlagIsSet State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_CheckIfResetOnLastResumeFlagIsSet(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfResetOnLastResumeFlagIsSet


DSM_EVENT
HUBDSM_CheckingIfResetOnLastResumeFlagIsSetOnSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfResetOnLastResumeFlagIsSetOnSuspended is called when the
    state machine enters the CheckingIfResetOnLastResumeFlagIsSetOnSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBPDO_CheckIfResetOnLastResumeFlagIsSet(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfResetOnLastResumeFlagIsSetOnSuspended


DSM_EVENT
HUBDSM_CompletingD0EntryOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingD0EntryOnDetach is called when the
    state machine enters the CompletingD0EntryOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingD0EntryOnDetach


DSM_EVENT
HUBDSM_CompletingD0EntryOnPortFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingD0EntryOnPortFailure is called when the
    state machine enters the CompletingD0EntryOnPortFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingD0EntryOnPortFailure


DSM_EVENT
HUBDSM_DisabledAfterD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisabledAfterD3Cold is called when the
    state machine enters the DisabledAfterD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_DisabledAfterD3Cold


DSM_EVENT
HUBDSM_DisablingOnHubSuspendNeedingRenumeration(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingOnHubSuspendNeedingRenumeration is called when the
    state machine enters the DisablingOnHubSuspendNeedingRenumeration State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

        HUBPDO_SetResetOnLastResumeFlag(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingOnHubSuspendNeedingRenumeration


DSM_EVENT
HUBDSM_DisablingPortOnStopFromSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingPortOnStopFromSuspended is called when the
    state machine enters the DisablingPortOnStopFromSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestDisable);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingPortOnStopFromSuspended


DSM_EVENT
HUBDSM_DisarmingDeviceForWakeOnD0Entry(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisarmingDeviceForWakeOnD0Entry is called when the
    state machine enters the DisarmingDeviceForWakeOnD0Entry State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteWaitWake(deviceContext);

        HUBDTX_DisarmDeviceForWakeUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisarmingDeviceForWakeOnD0Entry


DSM_EVENT
HUBDSM_DisarmingDeviceForWakeOnPortResumed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisarmingDeviceForWakeOnPortResumed is called when the
    state machine enters the DisarmingDeviceForWakeOnPortResumed State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_DisarmDeviceForWakeUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisarmingDeviceForWakeOnPortResumed


DSM_EVENT
HUBDSM_PurgingDeviceIoOnHubSuspendWhileWaitingForD0Entry(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PurgingDeviceIoOnHubSuspendWhileWaitingForD0Entry is called when the
    state machine enters the PurgingDeviceIoOnHubSuspendWhileWaitingForD0Entry State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_PurgeDeviceIoForSuspendUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_PurgingDeviceIoOnHubSuspendWhileWaitingForD0Entry


DSM_EVENT
HUBDSM_PurgingDeviceIoOnHubSuspendWhileWaitingForD0EntryOnHwWake(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PurgingDeviceIoOnHubSuspendWhileWaitingForD0EntryOnHwWake is called when the
    state machine enters the PurgingDeviceIoOnHubSuspendWhileWaitingForD0EntryOnHwWake State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_PurgeDeviceIoForSuspendUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_PurgingDeviceIoOnHubSuspendWhileWaitingForD0EntryOnHwWake


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnDetachWhileSuspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnDetachWhileSuspending is called when the
    state machine enters the ReleasingPowerReferenceOnDetachWhileSuspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnDetachWhileSuspending


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnDetachWhileWaitingForHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnDetachWhileWaitingForHubSuspend is called when the
    state machine enters the ReleasingPowerReferenceOnDetachWhileWaitingForHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnDetachWhileWaitingForHubSuspend


DSM_EVENT
HUBDSM_ReleasingPowerReferenceOnPortFailureWhileWaitingForHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReleasingPowerReferenceOnPortFailureWhileWaitingForHubSuspend is called when the
    state machine enters the ReleasingPowerReferenceOnPortFailureWhileWaitingForHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ReleasingPowerReferenceOnPortFailureWhileWaitingForHubSuspend


DSM_EVENT
HUBDSM_ResumingFromSelectiveSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ResumingFromSelectiveSuspend is called when the
    state machine enters the ResumingFromSelectiveSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestResume);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ResumingFromSelectiveSuspend


DSM_EVENT
HUBDSM_ReturningDetachDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningDetachDevice is called when the
    state machine enters the ReturningDetachDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningDetachDevice


DSM_EVENT
HUBDSM_ReturningDeviceResumed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningDeviceResumed is called when the
    state machine enters the ReturningDeviceResumed State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventDeviceResumed;

} // HUBDSM_ReturningDeviceResumed


DSM_EVENT
HUBDSM_ReturningHubStopFromSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubStopFromSuspended is called when the
    state machine enters the ReturningHubStopFromSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubStop;

} // HUBDSM_ReturningHubStopFromSuspended


DSM_EVENT
HUBDSM_ReturningPortFailureOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortFailureOnResume is called when the
    state machine enters the ReturningPortFailureOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortFailed;

} // HUBDSM_ReturningPortFailureOnResume


DSM_EVENT
HUBDSM_ReturningRenumerateDeviceOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningRenumerateDeviceOnResume is called when the
    state machine enters the ReturningRenumerateDeviceOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventDeviceRenumerateOnHubResume;

} // HUBDSM_ReturningRenumerateDeviceOnResume


DSM_EVENT
HUBDSM_SettingResetOnLastResumeDueToD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingResetOnLastResumeDueToD3Cold is called when the
    state machine enters the SettingResetOnLastResumeDueToD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SetResetOnLastResumeFlag(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SettingResetOnLastResumeDueToD3Cold


DSM_EVENT
HUBDSM_SettingResetOnLastResumeFlagForPDO(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingResetOnLastResumeFlagForPDO is called when the
    state machine enters the SettingResetOnLastResumeFlagForPDO State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SetDeviceFlagToForceResetOnEnumeration(deviceContext);

        HUBPDO_SetResetOnLastResumeFlag(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SettingResetOnLastResumeFlagForPDO


DSM_EVENT
HUBDSM_SettingResetOnLastResumeFlagForPDOAfterD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingResetOnLastResumeFlagForPDOAfterD3Cold is called when the
    state machine enters the SettingResetOnLastResumeFlagForPDOAfterD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SetResetOnLastResumeFlag(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SettingResetOnLastResumeFlagForPDOAfterD3Cold


DSM_EVENT
HUBDSM_StartingDeviceIoOnDeviceResumeOnHwWake(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StartingDeviceIoOnDeviceResumeOnHwWake is called when the
    state machine enters the StartingDeviceIoOnDeviceResumeOnHwWake State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_StartDeviceIoUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_StartingDeviceIoOnDeviceResumeOnHwWake


DSM_EVENT
HUBDSM_StartingDeviceIoOnDeviceResumeOnSwWake(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StartingDeviceIoOnDeviceResumeOnSwWake is called when the
    state machine enters the StartingDeviceIoOnDeviceResumeOnSwWake State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_StartDeviceIoUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_StartingDeviceIoOnDeviceResumeOnSwWake


DSM_EVENT
HUBDSM_Suspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_Suspended is called when the
    state machine enters the Suspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_Suspended


DSM_EVENT
HUBDSM_SuspendedWithHubSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SuspendedWithHubSuspended is called when the
    state machine enters the SuspendedWithHubSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SuspendedWithHubSuspended


DSM_EVENT
HUBDSM_SuspendedWithHubSuspendedInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SuspendedWithHubSuspendedInD3Cold is called when the
    state machine enters the SuspendedWithHubSuspendedInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMUX_ReleaseDevicePowerReference(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SuspendedWithHubSuspendedInD3Cold


DSM_EVENT
HUBDSM_SuspendingBackUnarmedDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SuspendingBackUnarmedDevice is called when the
    state machine enters the SuspendingBackUnarmedDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestSuspend);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SuspendingBackUnarmedDevice


DSM_EVENT
HUBDSM_SuspendingWakeArmedDeviceOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SuspendingWakeArmedDeviceOnHubSuspend is called when the
    state machine enters the SuspendingWakeArmedDeviceOnHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestSuspend);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SuspendingWakeArmedDeviceOnHubSuspend


DSM_EVENT
HUBDSM_WaitingForD0EntryOnHwWake(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForD0EntryOnHwWake is called when the
    state machine enters the WaitingForD0EntryOnHwWake State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteWaitWake(deviceContext);

        HUBPDO_SetSystemWakeSource(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForD0EntryOnHwWake


DSM_EVENT
HUBDSM_WaitingForD0EntryOnPendingRenumerate(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForD0EntryOnPendingRenumerate is called when the
    state machine enters the WaitingForD0EntryOnPendingRenumerate State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForD0EntryOnPendingRenumerate


DSM_EVENT
HUBDSM_WaitingForSuspendCompleteOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForSuspendCompleteOnHubSuspend is called when the
    state machine enters the WaitingForSuspendCompleteOnHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForSuspendCompleteOnHubSuspend


DSM_EVENT
HUBDSM_AbortingDeviceIoInSuspendingBeforeArmingForWakeFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AbortingDeviceIoInSuspendingBeforeArmingForWakeFailure is called when the
    state machine enters the AbortingDeviceIoInSuspendingBeforeArmingForWakeFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_AbortDeviceIoUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_AbortingDeviceIoInSuspendingBeforeArmingForWakeFailure


DSM_EVENT
HUBDSM_ArmingDeviceForWake(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ArmingDeviceForWake is called when the
    state machine enters the ArmingDeviceForWake State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_ArmDeviceForWakeUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ArmingDeviceForWake


DSM_EVENT
HUBDSM_CheckingIfDeviceArmedForWakeOnSuspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDeviceArmedForWakeOnSuspending is called when the
    state machine enters the CheckingIfDeviceArmedForWakeOnSuspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_ShouldDeviceBeArmedForWake(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDeviceArmedForWakeOnSuspending


DSM_EVENT
HUBDSM_CompletingWaitWakeOnDetachDuringSuspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingWaitWakeOnDetachDuringSuspending is called when the
    state machine enters the CompletingWaitWakeOnDetachDuringSuspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteWaitWake(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingWaitWakeOnDetachDuringSuspending


DSM_EVENT
HUBDSM_CompletingWaitWakeOnSuspendFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CompletingWaitWakeOnSuspendFailure is called when the
    state machine enters the CompletingWaitWakeOnSuspendFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_CompleteWaitWake(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CompletingWaitWakeOnSuspendFailure


DSM_EVENT
HUBDSM_PurgingDeviceIoInSuspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PurgingDeviceIoInSuspending is called when the
    state machine enters the PurgingDeviceIoInSuspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_PurgeDeviceIoForSuspendUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_PurgingDeviceIoInSuspending


DSM_EVENT
HUBDSM_PurgingDeviceIoInSuspendingAfterArmingForWake(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PurgingDeviceIoInSuspendingAfterArmingForWake is called when the
    state machine enters the PurgingDeviceIoInSuspendingAfterArmingForWake State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_PurgeDeviceIoForSuspendUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_PurgingDeviceIoInSuspendingAfterArmingForWake


DSM_EVENT
HUBDSM_PurgingDeviceIoInSuspendingAfterArmingForWakeFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PurgingDeviceIoInSuspendingAfterArmingForWakeFailure is called when the
    state machine enters the PurgingDeviceIoInSuspendingAfterArmingForWakeFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_PurgeDeviceIoForSuspendUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_PurgingDeviceIoInSuspendingAfterArmingForWakeFailure


DSM_EVENT
HUBDSM_ReturningOperationFailureInSuspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureInSuspending is called when the
    state machine enters the ReturningOperationFailureInSuspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureInSuspending


DSM_EVENT
HUBDSM_ReturningOperationSuccessInSuspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInSuspending is called when the
    state machine enters the ReturningOperationSuccessInSuspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInSuspending


DSM_EVENT
HUBDSM_ReturningPortDetachDeviceInSuspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDetachDeviceInSuspending is called when the
    state machine enters the ReturningPortDetachDeviceInSuspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDetachDeviceInSuspending


DSM_EVENT
HUBDSM_ReturningUnexpectedHubSuspendInSuspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningUnexpectedHubSuspendInSuspending is called when the
    state machine enters the ReturningUnexpectedHubSuspendInSuspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubSuspend;

} // HUBDSM_ReturningUnexpectedHubSuspendInSuspending


DSM_EVENT
HUBDSM_Suspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_Suspending is called when the
    state machine enters the Suspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestSuspend);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_Suspending


DSM_EVENT
HUBDSM_SuspendingPortOnFailureInSuspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SuspendingPortOnFailureInSuspending is called when the
    state machine enters the SuspendingPortOnFailureInSuspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestSuspend);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SuspendingPortOnFailureInSuspending


DSM_EVENT
HUBDSM_SuspendingWithArmedForWake(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SuspendingWithArmedForWake is called when the
    state machine enters the SuspendingWithArmedForWake State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestSuspend);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SuspendingWithArmedForWake


DSM_EVENT
HUBDSM_WaitingForPortResponseOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPortResponseOnHubSuspend is called when the
    state machine enters the WaitingForPortResponseOnHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPortResponseOnHubSuspend


DSM_EVENT
HUBDSM_CreatingNewEndpointsInSettingInterface(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CreatingNewEndpointsInSettingInterface is called when the
    state machine enters the CreatingNewEndpointsInSettingInterface State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBUCX_CreateEndpointsInUCX(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CreatingNewEndpointsInSettingInterface


DSM_EVENT
HUBDSM_DeletingEndpointsForNewInterfaceOnFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingEndpointsForNewInterfaceOnFailure is called when the
    state machine enters the DeletingEndpointsForNewInterfaceOnFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteEndpointsAndFreeResourcesForNewInterface(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingEndpointsForNewInterfaceOnFailure


DSM_EVENT
HUBDSM_DeletingEndpointsForOldInterface(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingEndpointsForOldInterface is called when the
    state machine enters the DeletingEndpointsForOldInterface State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteEndpointsAndFreeResourcesForOldInterface(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingEndpointsForOldInterface


DSM_EVENT
HUBDSM_DeletingEndpointsForOldInterfaceOnFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeletingEndpointsForOldInterfaceOnFailure is called when the
    state machine enters the DeletingEndpointsForOldInterfaceOnFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DeleteEndpointsAndFreeResourcesForOldInterface(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeletingEndpointsForOldInterfaceOnFailure


DSM_EVENT
HUBDSM_DisablingEndpointsForTheCurrentInterface(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsForTheCurrentInterface is called when the
    state machine enters the DisablingEndpointsForTheCurrentInterface State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableEndpointsInPendingDisableListUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsForTheCurrentInterface


DSM_EVENT
HUBDSM_DisablingEndpointsForTheNewInterfaceOnFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsForTheNewInterfaceOnFailure is called when the
    state machine enters the DisablingEndpointsForTheNewInterfaceOnFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_DisableAllEndpointsForNewInterfaceUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsForTheNewInterfaceOnFailure


DSM_EVENT
HUBDSM_PreparingEndpointAndInterfaceListsOnSelectInterface(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_PreparingEndpointAndInterfaceListsOnSelectInterface is called when the
    state machine enters the PreparingEndpointAndInterfaceListsOnSelectInterface State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_PrepareEndpointAndInterfaceListsForConfiguringDeviceOnSelectInterface(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_PreparingEndpointAndInterfaceListsOnSelectInterface


DSM_EVENT
HUBDSM_ProgrammingEndpointsAndSettingLPMValuesInSettingInterface(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ProgrammingEndpointsAndSettingLPMValuesInSettingInterface is called when the
    state machine enters the ProgrammingEndpointsAndSettingLPMValuesInSettingInterface State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_ProgrammingEndpointsAndSettingLPMValuesInSettingInterface


DSM_EVENT
HUBDSM_ProgrammingEndpointsInControllerInSettingInterfaceFor20Devices(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ProgrammingEndpointsInControllerInSettingInterfaceFor20Devices is called when the
    state machine enters the ProgrammingEndpointsInControllerInSettingInterfaceFor20Devices State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_EnableDisableEndpointsUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ProgrammingEndpointsInControllerInSettingInterfaceFor20Devices


DSM_EVENT
HUBDSM_ReturningOperationFailureInSettingInterface(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureInSettingInterface is called when the
    state machine enters the ReturningOperationFailureInSettingInterface State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureInSettingInterface


DSM_EVENT
HUBDSM_ReturningOperationSuccessInSettingInterface(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInSettingInterface is called when the
    state machine enters the ReturningOperationSuccessInSettingInterface State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInSettingInterface


DSM_EVENT
HUBDSM_SettingDeviceInterfaceInSettingInterface(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingDeviceInterfaceInSettingInterface is called when the
    state machine enters the SettingDeviceInterfaceInSettingInterface State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_SetDeviceInterfaceUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SettingDeviceInterfaceInSettingInterface


DSM_EVENT
HUBDSM_SettingDeviceInterfaceInSettingInterfaceOnFailureForCompat(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingDeviceInterfaceInSettingInterfaceOnFailureForCompat is called when the
    state machine enters the SettingDeviceInterfaceInSettingInterfaceOnFailureForCompat State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_SetDeviceInterfaceUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SettingDeviceInterfaceInSettingInterfaceOnFailureForCompat


DSM_EVENT
HUBDSM_CheckingIfDeviceSpeedChanged(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfDeviceSpeedChanged is called when the
    state machine enters the CheckingIfDeviceSpeedChanged State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfDeviceSpeedChanged(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfDeviceSpeedChanged


DSM_EVENT
HUBDSM_CheckingIfFirstEnumTryInRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfFirstEnumTryInRenum is called when the
    state machine enters the CheckingIfFirstEnumTryInRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfFirstEnumTry(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfFirstEnumTryInRenum


DSM_EVENT
HUBDSM_GettingDeviceDescriptorInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingDeviceDescriptorInReEnumAtZero is called when the
    state machine enters the GettingDeviceDescriptorInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetInitialDeviceDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingDeviceDescriptorInReEnumAtZero


DSM_EVENT
HUBDSM_LoggingReset1TimeoutInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_LoggingReset1TimeoutInReEnumAtZero is called when the
    state machine enters the LoggingReset1TimeoutInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_LogResetTimeout(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_LoggingReset1TimeoutInReEnumAtZero


DSM_EVENT
HUBDSM_LoggingReset2TimeoutInReEnum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_LoggingReset2TimeoutInReEnum is called when the
    state machine enters the LoggingReset2TimeoutInReEnum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_LogResetTimeout(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_LoggingReset2TimeoutInReEnum


DSM_EVENT
HUBDSM_NotifyingDeviceResetToUCXAfterReset1InRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_NotifyingDeviceResetToUCXAfterReset1InRenum is called when the
    state machine enters the NotifyingDeviceResetToUCXAfterReset1InRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_NotifyDeviceResetUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_NotifyingDeviceResetToUCXAfterReset1InRenum


DSM_EVENT
HUBDSM_NotifyingDeviceResetToUCXAfterReset2InRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_NotifyingDeviceResetToUCXAfterReset2InRenum is called when the
    state machine enters the NotifyingDeviceResetToUCXAfterReset2InRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_NotifyDeviceResetUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_NotifyingDeviceResetToUCXAfterReset2InRenum


DSM_EVENT
HUBDSM_Resetting1InRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_Resetting1InRenum is called when the
    state machine enters the Resetting1InRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestReset);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_Resetting1InRenum


DSM_EVENT
HUBDSM_Resetting2InRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_Resetting2InRenum is called when the
    state machine enters the Resetting2InRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestReset);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_Resetting2InRenum


DSM_EVENT
HUBDSM_ReturningDeviceOperationFailureInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningDeviceOperationFailureInReEnumAtZero is called when the
    state machine enters the ReturningDeviceOperationFailureInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningDeviceOperationFailureInReEnumAtZero


DSM_EVENT
HUBDSM_ReturningDeviceOperationSuccessInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningDeviceOperationSuccessInReEnumAtZero is called when the
    state machine enters the ReturningDeviceOperationSuccessInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningDeviceOperationSuccessInReEnumAtZero


DSM_EVENT
HUBDSM_ReturningHubStopInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubStopInReEnumAtZero is called when the
    state machine enters the ReturningHubStopInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubStop;

} // HUBDSM_ReturningHubStopInReEnumAtZero


DSM_EVENT
HUBDSM_ReturningHubSuspendInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubSuspendInReEnumAtZero is called when the
    state machine enters the ReturningHubSuspendInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubSuspend;

} // HUBDSM_ReturningHubSuspendInReEnumAtZero


DSM_EVENT
HUBDSM_ReturningPortDetachDeviceInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDetachDeviceInReEnumAtZero is called when the
    state machine enters the ReturningPortDetachDeviceInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDetachDeviceInReEnumAtZero


DSM_EVENT
HUBDSM_StoppingTimerOnHubStopInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppingTimerOnHubStopInReEnumAtZero is called when the
    state machine enters the StoppingTimerOnHubStopInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_StoppingTimerOnHubStopInReEnumAtZero


DSM_EVENT
HUBDSM_StoppingTimerOnHubSuspendInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppingTimerOnHubSuspendInReEnumAtZero is called when the
    state machine enters the StoppingTimerOnHubSuspendInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_StoppingTimerOnHubSuspendInReEnumAtZero


DSM_EVENT
HUBDSM_StoppingTimerOnPortDetachInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppingTimerOnPortDetachInReEnumAtZero is called when the
    state machine enters the StoppingTimerOnPortDetachInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_StoppingTimerOnPortDetachInReEnumAtZero


DSM_EVENT
HUBDSM_ValidatingDeviceDescriptorInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingDeviceDescriptorInReEnumAtZero is called when the
    state machine enters the ValidatingDeviceDescriptorInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateFirstDeviceDescriptorFor20(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingDeviceDescriptorInReEnumAtZero


DSM_EVENT
HUBDSM_WaitingForPortResetOnHubStopInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPortResetOnHubStopInReEnumAtZero is called when the
    state machine enters the WaitingForPortResetOnHubStopInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPortResetOnHubStopInReEnumAtZero


DSM_EVENT
HUBDSM_WaitingForPortResetOnHubSuspendInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPortResetOnHubSuspendInReEnumAtZero is called when the
    state machine enters the WaitingForPortResetOnHubSuspendInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPortResetOnHubSuspendInReEnumAtZero


DSM_EVENT
HUBDSM_WaitingForPostReset1TimerInRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPostReset1TimerInRenum is called when the
    state machine enters the WaitingForPostReset1TimerInRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StartPostResetTimerFor20(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForPostReset1TimerInRenum


DSM_EVENT
HUBDSM_WaitingForPostReset2ExtendedTimerInRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPostReset2ExtendedTimerInRenum is called when the
    state machine enters the WaitingForPostReset2ExtendedTimerInRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StartPostResetExtendedTimer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForPostReset2ExtendedTimerInRenum


DSM_EVENT
HUBDSM_WaitingForPostReset2TimerInRenum(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPostReset2TimerInRenum is called when the
    state machine enters the WaitingForPostReset2TimerInRenum State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StartPostResetTimerFor20(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_WaitingForPostReset2TimerInRenum


DSM_EVENT
HUBDSM_WaitingForStopSuspendOnReset1InReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForStopSuspendOnReset1InReEnumAtZero is called when the
    state machine enters the WaitingForStopSuspendOnReset1InReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForStopSuspendOnReset1InReEnumAtZero


DSM_EVENT
HUBDSM_WaitingForTimerOnHubStopInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerOnHubStopInReEnumAtZero is called when the
    state machine enters the WaitingForTimerOnHubStopInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerOnHubStopInReEnumAtZero


DSM_EVENT
HUBDSM_WaitingForTimerOnHubSuspendInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerOnHubSuspendInReEnumAtZero is called when the
    state machine enters the WaitingForTimerOnHubSuspendInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerOnHubSuspendInReEnumAtZero


DSM_EVENT
HUBDSM_WaitingForTimerOnPortDetachInReEnumAtZero(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerOnPortDetachInReEnumAtZero is called when the
    state machine enters the WaitingForTimerOnPortDetachInReEnumAtZero State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerOnPortDetachInReEnumAtZero


DSM_EVENT
HUBDSM_CheckingIfFirstReEnumTryFor30Device(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfFirstReEnumTryFor30Device is called when the
    state machine enters the CheckingIfFirstReEnumTryFor30Device State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfFirstEnumTry(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfFirstReEnumTryFor30Device


DSM_EVENT
HUBDSM_DeterminingPortResetTypeRequired(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DeterminingPortResetTypeRequired is called when the
    state machine enters the DeterminingPortResetTypeRequired State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_DetermineDeviceResetTypeRequiredAndClearResetFlags(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DeterminingPortResetTypeRequired


DSM_EVENT
HUBDSM_LoggingResetTimeoutInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_LoggingResetTimeoutInResettingFor30 is called when the
    state machine enters the LoggingResetTimeoutInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_LogResetTimeout(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_LoggingResetTimeoutInResettingFor30


DSM_EVENT
HUBDSM_NotifyingDeviceResetToUCXInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_NotifyingDeviceResetToUCXInResettingFor30 is called when the
    state machine enters the NotifyingDeviceResetToUCXInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_NotifyDeviceResetUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_NotifyingDeviceResetToUCXInResettingFor30


DSM_EVENT
HUBDSM_QueueingHotPortResetFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueueingHotPortResetFor30 is called when the
    state machine enters the QueueingHotPortResetFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestReset);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_QueueingHotPortResetFor30


DSM_EVENT
HUBDSM_QueueingWarmPortResetFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueueingWarmPortResetFor30 is called when the
    state machine enters the QueueingWarmPortResetFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceRequestWarmReset);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_QueueingWarmPortResetFor30


DSM_EVENT
HUBDSM_ReturningDeviceOperationFailureInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningDeviceOperationFailureInResettingFor30 is called when the
    state machine enters the ReturningDeviceOperationFailureInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningDeviceOperationFailureInResettingFor30


DSM_EVENT
HUBDSM_ReturningDeviceOperationSuccessInInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningDeviceOperationSuccessInInResettingFor30 is called when the
    state machine enters the ReturningDeviceOperationSuccessInInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningDeviceOperationSuccessInInResettingFor30


DSM_EVENT
HUBDSM_ReturningHubStopInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubStopInResettingFor30 is called when the
    state machine enters the ReturningHubStopInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubStop;

} // HUBDSM_ReturningHubStopInResettingFor30


DSM_EVENT
HUBDSM_ReturningHubSuspendInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningHubSuspendInResettingFor30 is called when the
    state machine enters the ReturningHubSuspendInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventHubSuspend;

} // HUBDSM_ReturningHubSuspendInResettingFor30


DSM_EVENT
HUBDSM_ReturningPortDetachDeviceInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDetachDeviceInResettingFor30 is called when the
    state machine enters the ReturningPortDetachDeviceInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDetachDeviceInResettingFor30


DSM_EVENT
HUBDSM_StartingPostResetTimerFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StartingPostResetTimerFor30 is called when the
    state machine enters the StartingPostResetTimerFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_StartPostResetTimerFor30(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_StartingPostResetTimerFor30


DSM_EVENT
HUBDSM_StoppingTimerOnHubStopInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppingTimerOnHubStopInResettingFor30 is called when the
    state machine enters the StoppingTimerOnHubStopInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_StoppingTimerOnHubStopInResettingFor30


DSM_EVENT
HUBDSM_StoppingTimerOnHubSuspendInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppingTimerOnHubSuspendInResettingFor30 is called when the
    state machine enters the StoppingTimerOnHubSuspendInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_StoppingTimerOnHubSuspendInResettingFor30


DSM_EVENT
HUBDSM_StoppingTimerOnPortDetachInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_StoppingTimerOnPortDetachInResettingFor30 is called when the
    state machine enters the StoppingTimerOnPortDetachInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopTimer(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_StoppingTimerOnPortDetachInResettingFor30


DSM_EVENT
HUBDSM_WaitingForPortResetCompleteFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPortResetCompleteFor30 is called when the
    state machine enters the WaitingForPortResetCompleteFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPortResetCompleteFor30


DSM_EVENT
HUBDSM_WaitingForPortResetOnHubStopInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPortResetOnHubStopInResettingFor30 is called when the
    state machine enters the WaitingForPortResetOnHubStopInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPortResetOnHubStopInResettingFor30


DSM_EVENT
HUBDSM_WaitingForPortResetOnHubSuspendInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForPortResetOnHubSuspendInResettingFor30 is called when the
    state machine enters the WaitingForPortResetOnHubSuspendInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForPortResetOnHubSuspendInResettingFor30


DSM_EVENT
HUBDSM_WaitingForStopSuspendOnReset1InResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForStopSuspendOnReset1InResettingFor30 is called when the
    state machine enters the WaitingForStopSuspendOnReset1InResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForStopSuspendOnReset1InResettingFor30


DSM_EVENT
HUBDSM_WaitingForTimerOnHubStopInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerOnHubStopInResettingFor30 is called when the
    state machine enters the WaitingForTimerOnHubStopInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerOnHubStopInResettingFor30


DSM_EVENT
HUBDSM_WaitingForTimerOnHubSuspendInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerOnHubSuspendInResettingFor30 is called when the
    state machine enters the WaitingForTimerOnHubSuspendInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerOnHubSuspendInResettingFor30


DSM_EVENT
HUBDSM_WaitingForTimerOnPortDetachInResettingFor30(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForTimerOnPortDetachInResettingFor30 is called when the
    state machine enters the WaitingForTimerOnPortDetachInResettingFor30 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForTimerOnPortDetachInResettingFor30


DSM_EVENT
HUBDSM_AllocatingBufferForMsOsExtendedPropertyDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_AllocatingBufferForMsOsExtendedPropertyDescriptor is called when the
    state machine enters the AllocatingBufferForMsOsExtendedPropertyDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_AllocateBufferForMSOSExtendedPropertyDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_AllocatingBufferForMsOsExtendedPropertyDescriptor


DSM_EVENT
HUBDSM_CheckingIfMsOs20RegistryValuesShouldBeInstalled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfMsOs20RegistryValuesShouldBeInstalled is called when the
    state machine enters the CheckingIfMsOs20RegistryValuesShouldBeInstalled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfMsOs20RegistryValuesShouldBeInstalled(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfMsOs20RegistryValuesShouldBeInstalled


DSM_EVENT
HUBDSM_CheckingIfMSOSExtendedPropertyDescriptorShouldBeQueried(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfMSOSExtendedPropertyDescriptorShouldBeQueried is called when the
    state machine enters the CheckingIfMSOSExtendedPropertyDescriptorShouldBeQueried State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfMSOSExtendedPropertyDescriptorShouldBeQueried(deviceContext);

        HUBPDO_SetInstallMSOSExtEventProcessed(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfMSOSExtendedPropertyDescriptorShouldBeQueried


DSM_EVENT
HUBDSM_FreeingBufferAllocatedForMSOSExtendedProperty(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_FreeingBufferAllocatedForMSOSExtendedProperty is called when the
    state machine enters the FreeingBufferAllocatedForMSOSExtendedProperty State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_FreeBufferForMSOSExtendedPropertyDescriptor(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_FreeingBufferAllocatedForMSOSExtendedProperty


DSM_EVENT
HUBDSM_GettingMSOSExtendedPropertyDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingMSOSExtendedPropertyDescriptor is called when the
    state machine enters the GettingMSOSExtendedPropertyDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetMSOSExtendedPropertyDescriptorUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingMSOSExtendedPropertyDescriptor


DSM_EVENT
HUBDSM_GettingMSOSExtendedPropertyDescriptorHeader(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingMSOSExtendedPropertyDescriptorHeader is called when the
    state machine enters the GettingMSOSExtendedPropertyDescriptorHeader State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetMSOSExtendedPropertyDescriptorHeaderUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingMSOSExtendedPropertyDescriptorHeader


DSM_EVENT
HUBDSM_InstallingMsOs20RegistryValues(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_InstallingMsOs20RegistryValues is called when the
    state machine enters the InstallingMsOs20RegistryValues State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_InstallMsOs20RegistryProperties(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_InstallingMsOs20RegistryValues


DSM_EVENT
HUBDSM_SettingExtPropDescSemaphoreForMsOs2(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingExtPropDescSemaphoreForMsOs2 is called when the
    state machine enters the SettingExtPropDescSemaphoreForMsOs2 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_SetExtPropDescSemaphoreInRegistry(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SettingExtPropDescSemaphoreForMsOs2


DSM_EVENT
HUBDSM_SettingExtPropDescSemaphoreRegistryFlag(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingExtPropDescSemaphoreRegistryFlag is called when the
    state machine enters the SettingExtPropDescSemaphoreRegistryFlag State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_SetExtPropDescSemaphoreInRegistry(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SettingExtPropDescSemaphoreRegistryFlag


DSM_EVENT
HUBDSM_SignalingPnpEventForMSOSExtInstall(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SignalingPnpEventForMSOSExtInstall is called when the
    state machine enters the SignalingPnpEventForMSOSExtInstall State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBPDO_SignalPnpPowerEvent(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_SignalingPnpEventForMSOSExtInstall


DSM_EVENT
HUBDSM_ValidatingMSOSExtendedPropertyDescriptor(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingMSOSExtendedPropertyDescriptor is called when the
    state machine enters the ValidatingMSOSExtendedPropertyDescriptor State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_ValidateAndCacheMSOSExtendedPropertyDescriptor(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingMSOSExtendedPropertyDescriptor


DSM_EVENT
HUBDSM_ValidatingMSOSExtendedPropertyDescriptorHeader(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingMSOSExtendedPropertyDescriptorHeader is called when the
    state machine enters the ValidatingMSOSExtendedPropertyDescriptorHeader State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_ValidateAndCacheMSOSExtendedPropertyDescriptorHeader(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingMSOSExtendedPropertyDescriptorHeader


DSM_EVENT
HUBDSM_WaitingForInstallMSOSExt(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingForInstallMSOSExt is called when the
    state machine enters the WaitingForInstallMSOSExt State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingForInstallMSOSExt


DSM_EVENT
HUBDSM_WritingCustomPropertiesInRegistry(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WritingCustomPropertiesInRegistry is called when the
    state machine enters the WritingCustomPropertiesInRegistry State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_ParseAndWriteCustomPropertiesInRegistry(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_WritingCustomPropertiesInRegistry


DSM_EVENT
HUBDSM_CheckingIfLTMShouldBeEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfLTMShouldBeEnabled is called when the
    state machine enters the CheckingIfLTMShouldBeEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfLTMShouldBeEnabled(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfLTMShouldBeEnabled


DSM_EVENT
HUBDSM_CheckingIfSetIsochDelayShouldBeSkipped(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfSetIsochDelayShouldBeSkipped is called when the
    state machine enters the CheckingIfSetIsochDelayShouldBeSkipped State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfSetIsochDelayShouldBeSkipped(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfSetIsochDelayShouldBeSkipped


DSM_EVENT
HUBDSM_CheckingIfSetSelShouldBeSkipped(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfSetSelShouldBeSkipped is called when the
    state machine enters the CheckingIfSetSelShouldBeSkipped State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfSetSelShouldBeSkipped(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfSetSelShouldBeSkipped


DSM_EVENT
HUBDSM_EnablingLTM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_EnablingLTM is called when the
    state machine enters the EnablingLTM State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_EnableLTMUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_EnablingLTM


DSM_EVENT
HUBDSM_ReturningOperationFailureInSettingLPMValues(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureInSettingLPMValues is called when the
    state machine enters the ReturningOperationFailureInSettingLPMValues State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureInSettingLPMValues


DSM_EVENT
HUBDSM_ReturningOperationSuccessInSettingLPMValues(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInSettingLPMValues is called when the
    state machine enters the ReturningOperationSuccessInSettingLPMValues State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInSettingLPMValues


DSM_EVENT
HUBDSM_SettingSEL(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SettingSEL is called when the
    state machine enters the SettingSEL State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_SetSelUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SettingSEL


DSM_EVENT
HUBDSM_SetttingIsochDelay(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_SetttingIsochDelay is called when the
    state machine enters the SetttingIsochDelay State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_SetIsochDelayUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_SetttingIsochDelay


DSM_EVENT
HUBDSM_CheckingIfEnableU1NeedsToBeUpdated(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfEnableU1NeedsToBeUpdated is called when the
    state machine enters the CheckingIfEnableU1NeedsToBeUpdated State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfEnableU1NeedsToBeUpdated(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfEnableU1NeedsToBeUpdated


DSM_EVENT
HUBDSM_CheckingIfEnableU2NeedsToBeUpdated(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfEnableU2NeedsToBeUpdated is called when the
    state machine enters the CheckingIfEnableU2NeedsToBeUpdated State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfEnableU2NeedsToBeUpdated(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfEnableU2NeedsToBeUpdated


DSM_EVENT
HUBDSM_CheckingIfEndpointsNeedToBeConfiguredAfterDecreasingExitLatency(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfEndpointsNeedToBeConfiguredAfterDecreasingExitLatency is called when the
    state machine enters the CheckingIfEndpointsNeedToBeConfiguredAfterDecreasingExitLatency State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBUCX_CheckIfEndpointsNeedToBeDisabledOrEnabled(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfEndpointsNeedToBeConfiguredAfterDecreasingExitLatency


DSM_EVENT
HUBDSM_CheckingIfEndpointsNeedToBeProgrammedAfterIncreasingLatency(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfEndpointsNeedToBeProgrammedAfterIncreasingLatency is called when the
    state machine enters the CheckingIfEndpointsNeedToBeProgrammedAfterIncreasingLatency State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBUCX_CheckIfEndpointsNeedToBeDisabledOrEnabled(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfEndpointsNeedToBeProgrammedAfterIncreasingLatency


DSM_EVENT
HUBDSM_CheckingIfEndpointsToBeDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfEndpointsToBeDisabled is called when the
    state machine enters the CheckingIfEndpointsToBeDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBUCX_CheckIfEndpointsNeedToBeDisabledOnConfigFailure(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfEndpointsToBeDisabled


DSM_EVENT
HUBDSM_CheckingIfEndpointsToBeDisabledOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfEndpointsToBeDisabledOnDetach is called when the
    state machine enters the CheckingIfEndpointsToBeDisabledOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBUCX_CheckIfEndpointsNeedToBeDisabledOnConfigFailure(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfEndpointsToBeDisabledOnDetach


DSM_EVENT
HUBDSM_CheckingIfExitLatencyNeedsToBeDecreased(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfExitLatencyNeedsToBeDecreased is called when the
    state machine enters the CheckingIfExitLatencyNeedsToBeDecreased State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfMaxExitLatencyNeedsToBeDecreased(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfExitLatencyNeedsToBeDecreased


DSM_EVENT
HUBDSM_CheckingIfExitLatencyNeedsToBeIncreased(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfExitLatencyNeedsToBeIncreased is called when the
    state machine enters the CheckingIfExitLatencyNeedsToBeIncreased State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfMaxExitLatencyNeedsToBeIncreased(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfExitLatencyNeedsToBeIncreased


DSM_EVENT
HUBDSM_CheckingIfU1TimeoutNeedsToBeChanged(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfU1TimeoutNeedsToBeChanged is called when the
    state machine enters the CheckingIfU1TimeoutNeedsToBeChanged State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfU1TimeoutNeedsToBeUpdated(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfU1TimeoutNeedsToBeChanged


DSM_EVENT
HUBDSM_CheckingIfU2TimeoutNeedsToBeChanged(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIfU2TimeoutNeedsToBeChanged is called when the
    state machine enters the CheckingIfU2TimeoutNeedsToBeChanged State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfU2TimeoutNeedsToBeUpdated(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIfU2TimeoutNeedsToBeChanged


DSM_EVENT
HUBDSM_ComputingU1U2TimeoutsAndExitLatency(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ComputingU1U2TimeoutsAndExitLatency is called when the
    state machine enters the ComputingU1U2TimeoutsAndExitLatency State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_ComputeU1Timeout(deviceContext);

        HUBMISC_ComputeU2Timeout(deviceContext);

        HUBMISC_ComputeMaximumExitLatency(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ComputingU1U2TimeoutsAndExitLatency


DSM_EVENT
HUBDSM_DisablingEndpointsOnConfigFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsOnConfigFailure is called when the
    state machine enters the DisablingEndpointsOnConfigFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_EnableDisableEndpointsUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsOnConfigFailure


DSM_EVENT
HUBDSM_DisablingEndpointsOnConfigFailureOnDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingEndpointsOnConfigFailureOnDetach is called when the
    state machine enters the DisablingEndpointsOnConfigFailureOnDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_EnableDisableEndpointsUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingEndpointsOnConfigFailureOnDetach


DSM_EVENT
HUBDSM_DisablingU1(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingU1 is called when the
    state machine enters the DisablingU1 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_DisableU1UsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingU1


DSM_EVENT
HUBDSM_DisablingU1U2OnExitLatencyTooLargeError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingU1U2OnExitLatencyTooLargeError is called when the
    state machine enters the DisablingU1U2OnExitLatencyTooLargeError State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_DisableU1U2OnExitLatencyTooLargeError(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_DisablingU1U2OnExitLatencyTooLargeError


DSM_EVENT
HUBDSM_DisablingU2(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_DisablingU2 is called when the
    state machine enters the DisablingU2 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_DisableU2UsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_DisablingU2


DSM_EVENT
HUBDSM_EnablingU1(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_EnablingU1 is called when the
    state machine enters the EnablingU1 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_EnableU1UsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_EnablingU1


DSM_EVENT
HUBDSM_EnablingU2(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_EnablingU2 is called when the
    state machine enters the EnablingU2 State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_EnableU2UsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_EnablingU2


DSM_EVENT
HUBDSM_InitializingU1U2Flags(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_InitializingU1U2Flags is called when the
    state machine enters the InitializingU1U2Flags State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_InitializeU1U2Flags(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_InitializingU1U2Flags


DSM_EVENT
HUBDSM_ProgrammingEndpointsInControllerAfterDecreasingExitLatency(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ProgrammingEndpointsInControllerAfterDecreasingExitLatency is called when the
    state machine enters the ProgrammingEndpointsInControllerAfterDecreasingExitLatency State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_EnableDisableEndpointsUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ProgrammingEndpointsInControllerAfterDecreasingExitLatency


DSM_EVENT
HUBDSM_ProgrammingEndpointsInControllerAfterIncreasingExitLatency(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ProgrammingEndpointsInControllerAfterIncreasingExitLatency is called when the
    state machine enters the ProgrammingEndpointsInControllerAfterIncreasingExitLatency State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_EnableDisableEndpointsUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_ProgrammingEndpointsInControllerAfterIncreasingExitLatency


DSM_EVENT
HUBDSM_QueuingSetU1TimeoutToPsm(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueuingSetU1TimeoutToPsm is called when the
    state machine enters the QueuingSetU1TimeoutToPsm State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceSetU1Timeout);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_QueuingSetU1TimeoutToPsm


DSM_EVENT
HUBDSM_QueuingSetU2TimeoutToPsm(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueuingSetU2TimeoutToPsm is called when the
    state machine enters the QueuingSetU2TimeoutToPsm State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceSetU2Timeout);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_QueuingSetU2TimeoutToPsm


DSM_EVENT
HUBDSM_ReturningOperationFailureInUpdatingLPMValues(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureInUpdatingLPMValues is called when the
    state machine enters the ReturningOperationFailureInUpdatingLPMValues State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureInUpdatingLPMValues


DSM_EVENT
HUBDSM_ReturningOperationSuccessInUpdatingLPMValues(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInUpdatingLPMValues is called when the
    state machine enters the ReturningOperationSuccessInUpdatingLPMValues State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInUpdatingLPMValues


DSM_EVENT
HUBDSM_ReturningPortDetachDeviceInUpdatingLPMValues(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningPortDetachDeviceInUpdatingLPMValues is called when the
    state machine enters the ReturningPortDetachDeviceInUpdatingLPMValues State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventPortDetachDevice;

} // HUBDSM_ReturningPortDetachDeviceInUpdatingLPMValues


DSM_EVENT
HUBDSM_UpdatingDeviceExitLatencyInTheControllerAfterDecreasingExitLatency(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UpdatingDeviceExitLatencyInTheControllerAfterDecreasingExitLatency is called when the
    state machine enters the UpdatingDeviceExitLatencyInTheControllerAfterDecreasingExitLatency State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_UpdateDeviceExitLatencyUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_UpdatingDeviceExitLatencyInTheControllerAfterDecreasingExitLatency


DSM_EVENT
HUBDSM_UpdatingDeviceExitLatencyInTheControllerAfterIncreasingExitLatency(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UpdatingDeviceExitLatencyInTheControllerAfterIncreasingExitLatency is called when the
    state machine enters the UpdatingDeviceExitLatencyInTheControllerAfterIncreasingExitLatency State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBUCX_UpdateDeviceExitLatencyUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_UpdatingDeviceExitLatencyInTheControllerAfterIncreasingExitLatency


DSM_EVENT
HUBDSM_UpdatingDeviceStatusToU1Disabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UpdatingDeviceStatusToU1Disabled is called when the
    state machine enters the UpdatingDeviceStatusToU1Disabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_UpdateDeviceU1StatusToDisabled(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_UpdatingDeviceStatusToU1Disabled


DSM_EVENT
HUBDSM_UpdatingDeviceStatusToU1Enabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UpdatingDeviceStatusToU1Enabled is called when the
    state machine enters the UpdatingDeviceStatusToU1Enabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_UpdateDeviceU1StatusToEnabled(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_UpdatingDeviceStatusToU1Enabled


DSM_EVENT
HUBDSM_UpdatingDeviceStatusToU2Disabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UpdatingDeviceStatusToU2Disabled is called when the
    state machine enters the UpdatingDeviceStatusToU2Disabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_UpdateDeviceU2StatusToDisabled(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_UpdatingDeviceStatusToU2Disabled


DSM_EVENT
HUBDSM_UpdatingDeviceStatusToU2Enabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_UpdatingDeviceStatusToU2Enabled is called when the
    state machine enters the UpdatingDeviceStatusToU2Enabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBMISC_UpdateDeviceU2StatusToEnabled(deviceContext);

        syncEvent = DsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_UpdatingDeviceStatusToU2Enabled


DSM_EVENT
HUBDSM_ComputingLPMTimeoutValuesInUnconfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ComputingLPMTimeoutValuesInUnconfigured is called when the
    state machine enters the ComputingLPMTimeoutValuesInUnconfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfU2ShouldBeSetForEnumeratedDevice(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ComputingLPMTimeoutValuesInUnconfigured


DSM_EVENT
HUBDSM_QueuingSetU2TimeoutToPsmForEnumeratedDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_QueuingSetU2TimeoutToPsmForEnumeratedDevice is called when the
    state machine enters the QueuingSetU2TimeoutToPsmForEnumeratedDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                        PsmEventDeviceSetU2Timeout);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_QueuingSetU2TimeoutToPsmForEnumeratedDevice


DSM_EVENT
HUBDSM_ReturningOperationFailureInUpdatingLPMValuesInUnconfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureInUpdatingLPMValuesInUnconfigured is called when the
    state machine enters the ReturningOperationFailureInUpdatingLPMValuesInUnconfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureInUpdatingLPMValuesInUnconfigured


DSM_EVENT
HUBDSM_ReturningOperationSuccessInUpdatingLPMValuesInUnconfigured(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInUpdatingLPMValuesInUnconfigured is called when the
    state machine enters the ReturningOperationSuccessInUpdatingLPMValuesInUnconfigured State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInUpdatingLPMValuesInUnconfigured


DSM_EVENT
HUBDSM_GettingRemoteWakeCapability(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_GettingRemoteWakeCapability is called when the
    state machine enters the GettingRemoteWakeCapability State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        HUBDTX_GetRemoteWakeCapabilityUsingControlTransfer(deviceContext);

    } FINALLY {

    }

    return DsmEventNull;

} // HUBDSM_GettingRemoteWakeCapability


DSM_EVENT
HUBDSM_ReturningOperationFailureInGettingRemoteWakeCap(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationFailureInGettingRemoteWakeCap is called when the
    state machine enters the ReturningOperationFailureInGettingRemoteWakeCap State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationFailure;

} // HUBDSM_ReturningOperationFailureInGettingRemoteWakeCap


DSM_EVENT
HUBDSM_ReturningOperationSuccessInGettingRemoteWakeCap(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInGettingRemoteWakeCap is called when the
    state machine enters the ReturningOperationSuccessInGettingRemoteWakeCap State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInGettingRemoteWakeCap


DSM_EVENT
HUBDSM_ValidatingInterfaceStatusForRemoteWake(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ValidatingInterfaceStatusForRemoteWake is called when the
    state machine enters the ValidatingInterfaceStatusForRemoteWake State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBDTX_ValidateInterfaceStatusForRemoteWakeCapability(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_ValidatingInterfaceStatusForRemoteWake


DSM_EVENT
HUBDSM_CheckingIf20LPMShouldBeEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_CheckingIf20LPMShouldBeEnabled is called when the
    state machine enters the CheckingIf20LPMShouldBeEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    PDEVICE_CONTEXT                 deviceContext;
    DSM_EVENT                       syncEvent;
    
    TRY {

        deviceContext = SmContext->ParentContext;
        syncEvent = HUBUCX_CheckIf20LPMIsEnabledAndUpdateUsingUCXIoctl(deviceContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBDSM_CheckingIf20LPMShouldBeEnabled


DSM_EVENT
HUBDSM_ReturningOperationSuccessInSetting20LPMValues(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_ReturningOperationSuccessInSetting20LPMValues is called when the
    state machine enters the ReturningOperationSuccessInSetting20LPMValues State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return DsmEventOperationSuccess;

} // HUBDSM_ReturningOperationSuccessInSetting20LPMValues


DSM_EVENT
HUBDSM_WaitingFor20LpmUpdateIoctlToComplete(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBDSM_WaitingFor20LpmUpdateIoctlToComplete is called when the
    state machine enters the WaitingFor20LpmUpdateIoctlToComplete State

Arguments:

    SmContext - State Machine Context


Return Value:

    DSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return DsmEventNull;

} // HUBDSM_WaitingFor20LpmUpdateIoctlToComplete





