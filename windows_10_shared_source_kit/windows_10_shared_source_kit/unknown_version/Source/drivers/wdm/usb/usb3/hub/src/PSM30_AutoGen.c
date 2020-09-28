
/*++
Copyright (c) Microsoft Corporation

Module Name:

PSM30_AutoGen.c

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/


#include <pch.h>

PSM_EVENT
HUBPSM30_AcquiringInterruptReferenceOnHSMInDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AcquiringInterruptReferenceOnHSMInDisabled is called when the
    state machine enters the AcquiringInterruptReferenceOnHSMInDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMUX_AcquireInterruptReference(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_AcquiringInterruptReferenceOnHSMInDisabled


PSM_EVENT
HUBPSM30_AcquiringInterruptReferenceOnHSMInEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AcquiringInterruptReferenceOnHSMInEnabled is called when the
    state machine enters the AcquiringInterruptReferenceOnHSMInEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMUX_AcquireInterruptReference(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_AcquiringInterruptReferenceOnHSMInEnabled


PSM_EVENT
HUBPSM30_AcquiringInterruptReferenceOnHSMInEnabledOnWarmReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AcquiringInterruptReferenceOnHSMInEnabledOnWarmReset is called when the
    state machine enters the AcquiringInterruptReferenceOnHSMInEnabledOnWarmReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMUX_AcquireInterruptReference(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_AcquiringInterruptReferenceOnHSMInEnabledOnWarmReset


PSM_EVENT
HUBPSM30_AcquiringInterruptReferenceOnHSMOnResetRequest(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AcquiringInterruptReferenceOnHSMOnResetRequest is called when the
    state machine enters the AcquiringInterruptReferenceOnHSMOnResetRequest State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMUX_AcquireInterruptReference(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_AcquiringInterruptReferenceOnHSMOnResetRequest


PSM_EVENT
HUBPSM30_AcquiringInterruptReferenceOnResuming(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AcquiringInterruptReferenceOnResuming is called when the
    state machine enters the AcquiringInterruptReferenceOnResuming State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMUX_AcquireInterruptReference(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_AcquiringInterruptReferenceOnResuming


PSM_EVENT
HUBPSM30_AcquiringPowerReferenceAfterDisablingSuperSpeed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AcquiringPowerReferenceAfterDisablingSuperSpeed is called when the
    state machine enters the AcquiringPowerReferenceAfterDisablingSuperSpeed State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_AcquirePortPowerReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_AcquiringPowerReferenceAfterDisablingSuperSpeed


PSM_EVENT
HUBPSM30_AcquiringPowerReferenceonPoweredOffConnectedErrorInS0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AcquiringPowerReferenceonPoweredOffConnectedErrorInS0 is called when the
    state machine enters the AcquiringPowerReferenceonPoweredOffConnectedErrorInS0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_AcquirePortPowerReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_AcquiringPowerReferenceonPoweredOffConnectedErrorInS0


PSM_EVENT
HUBPSM30_AcquiringPowerReferenceonPoweredOffDisconnectedInS0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AcquiringPowerReferenceonPoweredOffDisconnectedInS0 is called when the
    state machine enters the AcquiringPowerReferenceonPoweredOffDisconnectedInS0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_AcquirePortPowerReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_AcquiringPowerReferenceonPoweredOffDisconnectedInS0


PSM_EVENT
HUBPSM30_AcquiringPowerReferenceonPoweredOffSuspendedInS0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AcquiringPowerReferenceonPoweredOffSuspendedInS0 is called when the
    state machine enters the AcquiringPowerReferenceonPoweredOffSuspendedInS0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_AcquirePortPowerReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_AcquiringPowerReferenceonPoweredOffSuspendedInS0


PSM_EVENT
HUBPSM30_AcquiringReferenceInConnectedWithResetNeeded(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AcquiringReferenceInConnectedWithResetNeeded is called when the
    state machine enters the AcquiringReferenceInConnectedWithResetNeeded State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_AcquirePortPowerReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_AcquiringReferenceInConnectedWithResetNeeded


PSM_EVENT
HUBPSM30_CancellingResetOnSurpriseRemove(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CancellingResetOnSurpriseRemove is called when the
    state machine enters the CancellingResetOnSurpriseRemove State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReleaseInterruptReference(portContext);

        HUBHTX_CancelControlTransfer(portContext);

        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortFailed);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_CancellingResetOnSurpriseRemove


PSM_EVENT
HUBPSM30_CancellingSetPortPower(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CancellingSetPortPower is called when the
    state machine enters the CancellingSetPortPower State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_CancelControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_CancellingSetPortPower


PSM_EVENT
HUBPSM30_CheckingIfADeviceIsPresent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfADeviceIsPresent is called when the
    state machine enters the CheckingIfADeviceIsPresent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_Is30DevicePresent(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfADeviceIsPresent


PSM_EVENT
HUBPSM30_CheckingIfDebounceIsNeeded(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfDebounceIsNeeded is called when the
    state machine enters the CheckingIfDebounceIsNeeded State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfDebounceTimeIsNeeded(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfDebounceIsNeeded


PSM_EVENT
HUBPSM30_CheckingIfLinkStateIsU0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfLinkStateIsU0 is called when the
    state machine enters the CheckingIfLinkStateIsU0 State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsLinkStateU0(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfLinkStateIsU0


PSM_EVENT
HUBPSM30_CheckingIfOvercurrentBitOne(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfOvercurrentBitOne is called when the
    state machine enters the CheckingIfOvercurrentBitOne State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsOvercurrentBitOne(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfOvercurrentBitOne


PSM_EVENT
HUBPSM30_CheckingIfPersistentOvercurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfPersistentOvercurrent is called when the
    state machine enters the CheckingIfPersistentOvercurrent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBHTX_IncrementAndCheckIfOverCurrentCountExceededMax(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfPersistentOvercurrent


PSM_EVENT
HUBPSM30_CheckingIfSuperSpeedShouldBeDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfSuperSpeedShouldBeDisabled is called when the
    state machine enters the CheckingIfSuperSpeedShouldBeDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBHTX_CheckIfDisableSuperSpeedHackFlagIsSet(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfSuperSpeedShouldBeDisabled


PSM_EVENT
HUBPSM30_CheckingIsD3ColdIsEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIsD3ColdIsEnabled is called when the
    state machine enters the CheckingIsD3ColdIsEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsD3ColdSupported(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIsD3ColdIsEnabled


PSM_EVENT
HUBPSM30_ConnectedEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ConnectedEnabled is called when the
    state machine enters the ConnectedEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_ConnectedEnabled


PSM_EVENT
HUBPSM30_ConnectedError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ConnectedError is called when the
    state machine enters the ConnectedError State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_ConnectedError


PSM_EVENT
HUBPSM30_CreatingDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CreatingDevice is called when the
    state machine enters the CreatingDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CreateNewDSM(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CreatingDevice


PSM_EVENT
HUBPSM30_CreatingDeviceInConnectedWithResetInProgress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CreatingDeviceInConnectedWithResetInProgress is called when the
    state machine enters the CreatingDeviceInConnectedWithResetInProgress State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CreateNewDSM(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CreatingDeviceInConnectedWithResetInProgress


PSM_EVENT
HUBPSM30_CreatingDeviceInConnectedWithResetNeeded(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CreatingDeviceInConnectedWithResetNeeded is called when the
    state machine enters the CreatingDeviceInConnectedWithResetNeeded State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CreateNewDSM(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CreatingDeviceInConnectedWithResetNeeded


PSM_EVENT
HUBPSM30_DetachingDeviceFromPortOnDisconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DetachingDeviceFromPortOnDisconnect is called when the
    state machine enters the DetachingDeviceFromPortOnDisconnect State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_DetachDevice(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_DetachingDeviceFromPortOnDisconnect


PSM_EVENT
HUBPSM30_DetachingDeviceFromPortOnDisconnectOnTimedOut(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DetachingDeviceFromPortOnDisconnectOnTimedOut is called when the
    state machine enters the DetachingDeviceFromPortOnDisconnectOnTimedOut State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_DetachDevice(portContext);

        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_DetachingDeviceFromPortOnDisconnectOnTimedOut


PSM_EVENT
HUBPSM30_DetachingDeviceFromPortOnDisconnectWithTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DetachingDeviceFromPortOnDisconnectWithTimer is called when the
    state machine enters the DetachingDeviceFromPortOnDisconnectWithTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_DetachDevice(portContext);

        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_DetachingDeviceFromPortOnDisconnectWithTimer


PSM_EVENT
HUBPSM30_DetachingDeviceFromPortOnOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DetachingDeviceFromPortOnOverCurrent is called when the
    state machine enters the DetachingDeviceFromPortOnOverCurrent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_DetachDevice(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_DetachingDeviceFromPortOnOverCurrent


PSM_EVENT
HUBPSM30_DetachingDeviceFromPortOnOverCurrentClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DetachingDeviceFromPortOnOverCurrentClear is called when the
    state machine enters the DetachingDeviceFromPortOnOverCurrentClear State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_DetachDevice(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_DetachingDeviceFromPortOnOverCurrentClear


PSM_EVENT
HUBPSM30_DetachingDeviceFromPortOnOverCurrentClearOnTimedOut(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DetachingDeviceFromPortOnOverCurrentClearOnTimedOut is called when the
    state machine enters the DetachingDeviceFromPortOnOverCurrentClearOnTimedOut State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_DetachDevice(portContext);

        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_DetachingDeviceFromPortOnOverCurrentClearOnTimedOut


PSM_EVENT
HUBPSM30_DetachingDeviceFromPortOnOverCurrentClearWithTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DetachingDeviceFromPortOnOverCurrentClearWithTimer is called when the
    state machine enters the DetachingDeviceFromPortOnOverCurrentClearWithTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_DetachDevice(portContext);

        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_DetachingDeviceFromPortOnOverCurrentClearWithTimer


PSM_EVENT
HUBPSM30_DetachingDeviceFromPortOnOverCurrentOnTimedOut(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DetachingDeviceFromPortOnOverCurrentOnTimedOut is called when the
    state machine enters the DetachingDeviceFromPortOnOverCurrentOnTimedOut State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_DetachDevice(portContext);

        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_DetachingDeviceFromPortOnOverCurrentOnTimedOut


PSM_EVENT
HUBPSM30_DetachingDeviceFromPortOnOverCurrentWithTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DetachingDeviceFromPortOnOverCurrentWithTimer is called when the
    state machine enters the DetachingDeviceFromPortOnOverCurrentWithTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_DetachDevice(portContext);

        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_DetachingDeviceFromPortOnOverCurrentWithTimer


PSM_EVENT
HUBPSM30_DetachingDeviceFromPortOnPortCycle(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DetachingDeviceFromPortOnPortCycle is called when the
    state machine enters the DetachingDeviceFromPortOnPortCycle State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_DetachDevice(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_DetachingDeviceFromPortOnPortCycle


PSM_EVENT
HUBPSM30_DetachingDeviceOnCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DetachingDeviceOnCleanup is called when the
    state machine enters the DetachingDeviceOnCleanup State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_DetachDevice(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_DetachingDeviceOnCleanup


PSM_EVENT
HUBPSM30_DisablingSuperSpeedOnPortForUnsupportedHubs(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DisablingSuperSpeedOnPortForUnsupportedHubs is called when the
    state machine enters the DisablingSuperSpeedOnPortForUnsupportedHubs State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_SetLinkStateToSSDisabledUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_DisablingSuperSpeedOnPortForUnsupportedHubs


PSM_EVENT
HUBPSM30_DisablingSuperSpeedOnPortOnDeviceRequest(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DisablingSuperSpeedOnPortOnDeviceRequest is called when the
    state machine enters the DisablingSuperSpeedOnPortOnDeviceRequest State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_DetachDevice(portContext);

        HUBHTX_SetLinkStateToSSDisabledUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_DisablingSuperSpeedOnPortOnDeviceRequest


PSM_EVENT
HUBPSM30_Disconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_Disconnected is called when the
    state machine enters the Disconnected State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_NotifyDeviceRemovalIfDeviceSupportsReattach(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_Disconnected


PSM_EVENT
HUBPSM30_DisconnectedSuperSpeedDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DisconnectedSuperSpeedDisabled is called when the
    state machine enters the DisconnectedSuperSpeedDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_DisconnectedSuperSpeedDisabled


PSM_EVENT
HUBPSM30_DoesDeviceSupportsReAttach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DoesDeviceSupportsReAttach is called when the
    state machine enters the DoesDeviceSupportsReAttach State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfConnectedDeviceSupportsReAttach(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_DoesDeviceSupportsReAttach


PSM_EVENT
HUBPSM30_DoesDeviceSupportsReAttachWithResetInProgress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DoesDeviceSupportsReAttachWithResetInProgress is called when the
    state machine enters the DoesDeviceSupportsReAttachWithResetInProgress State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfConnectedDeviceSupportsReAttach(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_DoesDeviceSupportsReAttachWithResetInProgress


PSM_EVENT
HUBPSM30_DoesDeviceSupportsReAttachWithResetNeeded(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DoesDeviceSupportsReAttachWithResetNeeded is called when the
    state machine enters the DoesDeviceSupportsReAttachWithResetNeeded State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfConnectedDeviceSupportsReAttach(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_DoesDeviceSupportsReAttachWithResetNeeded


PSM_EVENT
HUBPSM30_EnablingSuperSpeedOnStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_EnablingSuperSpeedOnStopSuspend is called when the
    state machine enters the EnablingSuperSpeedOnStopSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_SetLinkStateToRxDetectUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_EnablingSuperSpeedOnStopSuspend


PSM_EVENT
HUBPSM30_EnablingSuperSpeedOnTimerExpiration(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_EnablingSuperSpeedOnTimerExpiration is called when the
    state machine enters the EnablingSuperSpeedOnTimerExpiration State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_SetLinkStateToRxDetectUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_EnablingSuperSpeedOnTimerExpiration


PSM_EVENT
HUBPSM30_FlushingStaleDeviceEvents(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_FlushingStaleDeviceEvents is called when the
    state machine enters the FlushingStaleDeviceEvents State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventOperationSuccess;

} // HUBPSM30_FlushingStaleDeviceEvents


PSM_EVENT
HUBPSM30_FlushingStaleDeviceEventsWithResetInProgress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_FlushingStaleDeviceEventsWithResetInProgress is called when the
    state machine enters the FlushingStaleDeviceEventsWithResetInProgress State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventOperationSuccess;

} // HUBPSM30_FlushingStaleDeviceEventsWithResetInProgress


PSM_EVENT
HUBPSM30_FlushingStaleDeviceEventsWithResetNeeded(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_FlushingStaleDeviceEventsWithResetNeeded is called when the
    state machine enters the FlushingStaleDeviceEventsWithResetNeeded State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventOperationSuccess;

} // HUBPSM30_FlushingStaleDeviceEventsWithResetNeeded


PSM_EVENT
HUBPSM30_FlushingUserResetForOverCurrentOnHubStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_FlushingUserResetForOverCurrentOnHubStopSuspend is called when the
    state machine enters the FlushingUserResetForOverCurrentOnHubStopSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_FlushUserInitiatedResetOnOverCurrent(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_FlushingUserResetForOverCurrentOnHubStopSuspend


PSM_EVENT
HUBPSM30_FlushingUserResetForOverCurrentOnOverCurrentClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_FlushingUserResetForOverCurrentOnOverCurrentClear is called when the
    state machine enters the FlushingUserResetForOverCurrentOnOverCurrentClear State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_FlushUserInitiatedResetOnOverCurrent(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_FlushingUserResetForOverCurrentOnOverCurrentClear


PSM_EVENT
HUBPSM30_GettingPortChangeOnStartResumeFromConnectedError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortChangeOnStartResumeFromConnectedError is called when the
    state machine enters the GettingPortChangeOnStartResumeFromConnectedError State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_GettingPortChangeOnStartResumeFromConnectedError


PSM_EVENT
HUBPSM30_GettingPortStatusOnResetTimeout(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatusOnResetTimeout is called when the
    state machine enters the GettingPortStatusOnResetTimeout State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_GettingPortStatusOnResetTimeout


PSM_EVENT
HUBPSM30_GettingPortStatusOnResumeRequest(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatusOnResumeRequest is called when the
    state machine enters the GettingPortStatusOnResumeRequest State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_GetPortStatusUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_GettingPortStatusOnResumeRequest


PSM_EVENT
HUBPSM30_GettingPortStatusOnResumeTimeout(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatusOnResumeTimeout is called when the
    state machine enters the GettingPortStatusOnResumeTimeout State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_GetPortStatusUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_GettingPortStatusOnResumeTimeout


PSM_EVENT
HUBPSM30_InitiatingResetPort(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_InitiatingResetPort is called when the
    state machine enters the InitiatingResetPort State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_ResetPortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_InitiatingResetPort


PSM_EVENT
HUBPSM30_InitiatingResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_InitiatingResume is called when the
    state machine enters the InitiatingResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_Resume30PortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_InitiatingResume


PSM_EVENT
HUBPSM30_InitiatingWarmResetPort(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_InitiatingWarmResetPort is called when the
    state machine enters the InitiatingWarmResetPort State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_WarmResetPortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_InitiatingWarmResetPort


PSM_EVENT
HUBPSM30_IsDeviceInU0OnResumeRequest(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IsDeviceInU0OnResumeRequest is called when the
    state machine enters the IsDeviceInU0OnResumeRequest State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsLinkStateU0(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IsDeviceInU0OnResumeRequest


PSM_EVENT
HUBPSM30_IsDeviceInU0OnResumeTimeout(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IsDeviceInU0OnResumeTimeout is called when the
    state machine enters the IsDeviceInU0OnResumeTimeout State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsLinkStateU0(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IsDeviceInU0OnResumeTimeout


PSM_EVENT
HUBPSM30_IsOldDevicePresent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IsOldDevicePresent is called when the
    state machine enters the IsOldDevicePresent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfOldDeviceIsStillAttached(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IsOldDevicePresent


PSM_EVENT
HUBPSM30_IsOldDevicePresentInConnectedWithResetNeeded(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IsOldDevicePresentInConnectedWithResetNeeded is called when the
    state machine enters the IsOldDevicePresentInConnectedWithResetNeeded State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfOldDeviceIsStillAttached(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IsOldDevicePresentInConnectedWithResetNeeded


PSM_EVENT
HUBPSM30_IsOldDevicePresentWithResetInProgress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IsOldDevicePresentWithResetInProgress is called when the
    state machine enters the IsOldDevicePresentWithResetInProgress State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfOldDeviceIsStillAttached(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IsOldDevicePresentWithResetInProgress


PSM_EVENT
HUBPSM30_IssuingAttachDeviceToDsm(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingAttachDeviceToDsm is called when the
    state machine enters the IssuingAttachDeviceToDsm State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_SetPortAndSpeedFlagsFor30Device(portContext);

        syncEvent = HUBMISC_AttachDevice(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingAttachDeviceToDsm


PSM_EVENT
HUBPSM30_IssuingAttachDeviceWithResetToDsm(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingAttachDeviceWithResetToDsm is called when the
    state machine enters the IssuingAttachDeviceWithResetToDsm State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_SetDeviceFlagToForceResetOnEnumeration(portContext);

        HUBMISC_SetPortAndSpeedFlagsFor30Device(portContext);

        syncEvent = HUBMISC_AttachDevice(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingAttachDeviceWithResetToDsm


PSM_EVENT
HUBPSM30_IssuingAttachDeviceWithResetToDsmWithResetInProgress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingAttachDeviceWithResetToDsmWithResetInProgress is called when the
    state machine enters the IssuingAttachDeviceWithResetToDsmWithResetInProgress State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_SetDeviceFlagToForceResetOnEnumeration(portContext);

        HUBMISC_SetPortAndSpeedFlagsFor30Device(portContext);

        syncEvent = HUBMISC_AttachDevice(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingAttachDeviceWithResetToDsmWithResetInProgress


PSM_EVENT
HUBPSM30_IssuingDetachDeviceOnCycleFromPoweredOffDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingDetachDeviceOnCycleFromPoweredOffDisabled is called when the
    state machine enters the IssuingDetachDeviceOnCycleFromPoweredOffDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_DetachDevice(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingDetachDeviceOnCycleFromPoweredOffDisabled


PSM_EVENT
HUBPSM30_IssuingDisabledToDSMFromDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingDisabledToDSMFromDisabled is called when the
    state machine enters the IssuingDisabledToDSMFromDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortDisabled);

        HUBMISC_SetDeviceFlagToForceResetOnEnumeration(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingDisabledToDSMFromDisabled


PSM_EVENT
HUBPSM30_IssuingHubReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingHubReset is called when the
    state machine enters the IssuingHubReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ResetHub(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_IssuingHubReset


PSM_EVENT
HUBPSM30_IssuingHubResetFromDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingHubResetFromDisconnected is called when the
    state machine enters the IssuingHubResetFromDisconnected State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ResetHub(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_IssuingHubResetFromDisconnected


PSM_EVENT
HUBPSM30_IssuingHubResetOnDisableFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingHubResetOnDisableFailure is called when the
    state machine enters the IssuingHubResetOnDisableFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ResetHub(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingHubResetOnDisableFailure


PSM_EVENT
HUBPSM30_IssuingHubResetOnTimedOut(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingHubResetOnTimedOut is called when the
    state machine enters the IssuingHubResetOnTimedOut State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortFailed);

        HUBMUX_ResetHub(portContext);

        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingHubResetOnTimedOut


PSM_EVENT
HUBPSM30_IssuingHubResetWithPendingTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingHubResetWithPendingTimer is called when the
    state machine enters the IssuingHubResetWithPendingTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ResetHub(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingHubResetWithPendingTimer


PSM_EVENT
HUBPSM30_IssuingHubResetWithTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingHubResetWithTimer is called when the
    state machine enters the IssuingHubResetWithTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ResetHub(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingHubResetWithTimer


PSM_EVENT
HUBPSM30_IssuingPortDisabledToDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingPortDisabledToDevice is called when the
    state machine enters the IssuingPortDisabledToDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortDisabled);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingPortDisabledToDevice


PSM_EVENT
HUBPSM30_IssuingPortDisableFailedToDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingPortDisableFailedToDevice is called when the
    state machine enters the IssuingPortDisableFailedToDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortFailed);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingPortDisableFailedToDevice


PSM_EVENT
HUBPSM30_IssuingPortFailedToDSMOnLinkStateError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingPortFailedToDSMOnLinkStateError is called when the
    state machine enters the IssuingPortFailedToDSMOnLinkStateError State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortFailed);

        HUBMISC_LogPortLinkStateError(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingPortFailedToDSMOnLinkStateError


PSM_EVENT
HUBPSM30_IssuingPortResetTimedOutToDSM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingPortResetTimedOutToDSM is called when the
    state machine enters the IssuingPortResetTimedOutToDSM State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortResetTimedOut);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingPortResetTimedOutToDSM


PSM_EVENT
HUBPSM30_IssuingReAttachDeviceToBootDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingReAttachDeviceToBootDevice is called when the
    state machine enters the IssuingReAttachDeviceToBootDevice State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_ReAttachDevice(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingReAttachDeviceToBootDevice


PSM_EVENT
HUBPSM30_IssuingReAttachDeviceToBootDeviceWithResetInProgress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingReAttachDeviceToBootDeviceWithResetInProgress is called when the
    state machine enters the IssuingReAttachDeviceToBootDeviceWithResetInProgress State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_ReAttachDevice(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingReAttachDeviceToBootDeviceWithResetInProgress


PSM_EVENT
HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled is called when the
    state machine enters the IssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortResetFailedDueToPendingSuspend);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled


PSM_EVENT
HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled is called when the
    state machine enters the IssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortResetFailedDueToPendingSuspend);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled


PSM_EVENT
HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMWhileResetInProgress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMWhileResetInProgress is called when the
    state machine enters the IssuingResetFailedDuetoPendingHubSuspendToDSMWhileResetInProgress State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortResetFailedDueToPendingSuspend);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMWhileResetInProgress


PSM_EVENT
HUBPSM30_IssuingResumedToDeviceSM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingResumedToDeviceSM is called when the
    state machine enters the IssuingResumedToDeviceSM State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortResumed);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingResumedToDeviceSM


PSM_EVENT
HUBPSM30_IssuingSuspendedToDeviceSM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingSuspendedToDeviceSM is called when the
    state machine enters the IssuingSuspendedToDeviceSM State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortSuspended);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingSuspendedToDeviceSM


PSM_EVENT
HUBPSM30_NotifyingUserAboutPersistentOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_NotifyingUserAboutPersistentOverCurrent is called when the
    state machine enters the NotifyingUserAboutPersistentOverCurrent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_GenerateUserNotificationForPersistentOverCurrent(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_NotifyingUserAboutPersistentOverCurrent


PSM_EVENT
HUBPSM30_PoweredOffConnectedError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_PoweredOffConnectedError is called when the
    state machine enters the PoweredOffConnectedError State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_PoweredOffConnectedError


PSM_EVENT
HUBPSM30_PoweredOffConnectedWithResetNeededAndDeviceDetached(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_PoweredOffConnectedWithResetNeededAndDeviceDetached is called when the
    state machine enters the PoweredOffConnectedWithResetNeededAndDeviceDetached State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_PoweredOffConnectedWithResetNeededAndDeviceDetached


PSM_EVENT
HUBPSM30_PoweredOffDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_PoweredOffDisconnected is called when the
    state machine enters the PoweredOffDisconnected State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_ClearCurrentPortStatus(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_PoweredOffDisconnected


PSM_EVENT
HUBPSM30_PoweredOffSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_PoweredOffSuspended is called when the
    state machine enters the PoweredOffSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReleasePortPowerReference(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_PoweredOffSuspended


PSM_EVENT
HUBPSM30_QueryingPortStatusOnOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueryingPortStatusOnOverCurrent is called when the
    state machine enters the QueryingPortStatusOnOverCurrent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_GetPortStatusUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_QueryingPortStatusOnOverCurrent


PSM_EVENT
HUBPSM30_QueueingPortFailureToDSMOnTransferFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueingPortFailureToDSMOnTransferFailure is called when the
    state machine enters the QueueingPortFailureToDSMOnTransferFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortFailed);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueingPortFailureToDSMOnTransferFailure


PSM_EVENT
HUBPSM30_QueueingPortFailureToDSMOnTransferFailureForLpmTimeout(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueingPortFailureToDSMOnTransferFailureForLpmTimeout is called when the
    state machine enters the QueueingPortFailureToDSMOnTransferFailureForLpmTimeout State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortFailed);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueingPortFailureToDSMOnTransferFailureForLpmTimeout


PSM_EVENT
HUBPSM30_QueueingPortResumeFailedDueToPendingSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueingPortResumeFailedDueToPendingSuspend is called when the
    state machine enters the QueueingPortResumeFailedDueToPendingSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortResumeFailedDueToPendingSuspend);

        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueingPortResumeFailedDueToPendingSuspend


PSM_EVENT
HUBPSM30_QueueingPortResumeTimedOut(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueingPortResumeTimedOut is called when the
    state machine enters the QueueingPortResumeTimedOut State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReleaseInterruptReference(portContext);

        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortResumeTimedOut);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueingPortResumeTimedOut


PSM_EVENT
HUBPSM30_QueueingPortTimeoutUpdatedToDSM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueingPortTimeoutUpdatedToDSM is called when the
    state machine enters the QueueingPortTimeoutUpdatedToDSM State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortTimeoutUpdated);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueingPortTimeoutUpdatedToDSM


PSM_EVENT
HUBPSM30_QueueStateDisabledToDSMOnStartFromConnectedError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueStateDisabledToDSMOnStartFromConnectedError is called when the
    state machine enters the QueueStateDisabledToDSMOnStartFromConnectedError State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortStateDisabled);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueStateDisabledToDSMOnStartFromConnectedError


PSM_EVENT
HUBPSM30_ReleasingInterruptReferenceAfterResetFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReleasingInterruptReferenceAfterResetFailure is called when the
    state machine enters the ReleasingInterruptReferenceAfterResetFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ReleasingInterruptReferenceAfterResetFailure


PSM_EVENT
HUBPSM30_ReleasingInterruptReferenceAndStoppingTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReleasingInterruptReferenceAndStoppingTimer is called when the
    state machine enters the ReleasingInterruptReferenceAndStoppingTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopPortTimer(portContext);

        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortFailed);

        HUBMUX_ReleaseInterruptReference(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ReleasingInterruptReferenceAndStoppingTimer


PSM_EVENT
HUBPSM30_ReleasingInterruptReferenceOnResetTransferFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReleasingInterruptReferenceOnResetTransferFailure is called when the
    state machine enters the ReleasingInterruptReferenceOnResetTransferFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReleaseInterruptReference(portContext);

        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortFailed);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ReleasingInterruptReferenceOnResetTransferFailure


PSM_EVENT
HUBPSM30_ReleasingInterruptReferenceOnResumed(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReleasingInterruptReferenceOnResumed is called when the
    state machine enters the ReleasingInterruptReferenceOnResumed State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ReleasingInterruptReferenceOnResumed


PSM_EVENT
HUBPSM30_ReleasingInterruptReferenceOnSuccessfulReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReleasingInterruptReferenceOnSuccessfulReset is called when the
    state machine enters the ReleasingInterruptReferenceOnSuccessfulReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortResetComplete);

        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ReleasingInterruptReferenceOnSuccessfulReset


PSM_EVENT
HUBPSM30_ReleasingInterruptReferenceOnTransferFailureInResuming(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReleasingInterruptReferenceOnTransferFailureInResuming is called when the
    state machine enters the ReleasingInterruptReferenceOnTransferFailureInResuming State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ReleasingInterruptReferenceOnTransferFailureInResuming


PSM_EVENT
HUBPSM30_ReleasingPortPowerReferenceInSuperSpeedDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReleasingPortPowerReferenceInSuperSpeedDisabled is called when the
    state machine enters the ReleasingPortPowerReferenceInSuperSpeedDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReleasePortPowerReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ReleasingPortPowerReferenceInSuperSpeedDisabled


PSM_EVENT
HUBPSM30_ReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded is called when the
    state machine enters the ReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReleasePortPowerReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded


PSM_EVENT
HUBPSM30_ReleasingPowerReferenceonPoweredOffDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReleasingPowerReferenceonPoweredOffDisabled is called when the
    state machine enters the ReleasingPowerReferenceonPoweredOffDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReleasePortPowerReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ReleasingPowerReferenceonPoweredOffDisabled


PSM_EVENT
HUBPSM30_ReleasingPowerReferenceonPoweredOffDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReleasingPowerReferenceonPoweredOffDisconnected is called when the
    state machine enters the ReleasingPowerReferenceonPoweredOffDisconnected State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReleasePortPowerReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ReleasingPowerReferenceonPoweredOffDisconnected


PSM_EVENT
HUBPSM30_ReleasingResetReferenceInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReleasingResetReferenceInDisconnected is called when the
    state machine enters the ReleasingResetReferenceInDisconnected State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReleaseResetReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ReleasingResetReferenceInDisconnected


PSM_EVENT
HUBPSM30_ReleasingResetReferenceInSuperSpeedDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReleasingResetReferenceInSuperSpeedDisabled is called when the
    state machine enters the ReleasingResetReferenceInSuperSpeedDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReleaseResetReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ReleasingResetReferenceInSuperSpeedDisabled


PSM_EVENT
HUBPSM30_ReleasingResetReferenceOnDeviceRemoval(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReleasingResetReferenceOnDeviceRemoval is called when the
    state machine enters the ReleasingResetReferenceOnDeviceRemoval State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_DetachDevice(portContext);

        HUBMUX_ReleaseResetReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ReleasingResetReferenceOnDeviceRemoval


PSM_EVENT
HUBPSM30_ResettingHubAndFlushingUserInitiatedResetOnOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ResettingHubAndFlushingUserInitiatedResetOnOverCurrent is called when the
    state machine enters the ResettingHubAndFlushingUserInitiatedResetOnOverCurrent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_FlushUserInitiatedResetOnOverCurrent(portContext);

        HUBMUX_ResetHub(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ResettingHubAndFlushingUserInitiatedResetOnOverCurrent


PSM_EVENT
HUBPSM30_SettingPortPowerOnOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_SettingPortPowerOnOverCurrent is called when the
    state machine enters the SettingPortPowerOnOverCurrent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_SettingPortPowerUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_SettingPortPowerOnOverCurrent


PSM_EVENT
HUBPSM30_SettingRemotWakeMask(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_SettingRemotWakeMask is called when the
    state machine enters the SettingRemotWakeMask State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_SettingRemotWakeMask


PSM_EVENT
HUBPSM30_SettingRemotWakeMaskInDisconected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_SettingRemotWakeMaskInDisconected is called when the
    state machine enters the SettingRemotWakeMaskInDisconected State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_SettingRemotWakeMaskInDisconected


PSM_EVENT
HUBPSM30_SettingU1TImeOut(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_SettingU1TImeOut is called when the
    state machine enters the SettingU1TImeOut State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_SetU1TimeoutUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_SettingU1TImeOut


PSM_EVENT
HUBPSM30_SettingU2TImeOut(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_SettingU2TImeOut is called when the
    state machine enters the SettingU2TImeOut State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_SetU2TimeoutUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_SettingU2TImeOut


PSM_EVENT
HUBPSM30_StartingAndAcquiringReferenceOnHubStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StartingAndAcquiringReferenceOnHubStart is called when the
    state machine enters the StartingAndAcquiringReferenceOnHubStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_StartingAndAcquiringReferenceOnHubStart


PSM_EVENT
HUBPSM30_StartingAndAcquiringReferenceOnSystemResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StartingAndAcquiringReferenceOnSystemResume is called when the
    state machine enters the StartingAndAcquiringReferenceOnSystemResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_StartingAndAcquiringReferenceOnSystemResume


PSM_EVENT
HUBPSM30_StartingDisableSuperSpeedTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StartingDisableSuperSpeedTimer is called when the
    state machine enters the StartingDisableSuperSpeedTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_StartSuperSpeedDisableTimer(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StartingDisableSuperSpeedTimer


PSM_EVENT
HUBPSM30_StartingOverCurrentTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StartingOverCurrentTimer is called when the
    state machine enters the StartingOverCurrentTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_StartOverCurrentTimer(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StartingOverCurrentTimer


PSM_EVENT
HUBPSM30_StartingResumeTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StartingResumeTimer is called when the
    state machine enters the StartingResumeTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_StartResumeCompletionTimer(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StartingResumeTimer


PSM_EVENT
HUBPSM30_StoppingDisableSuperSpeedTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StoppingDisableSuperSpeedTimer is called when the
    state machine enters the StoppingDisableSuperSpeedTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopPortTimer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StoppingDisableSuperSpeedTimer


PSM_EVENT
HUBPSM30_StoppingResumeTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StoppingResumeTimer is called when the
    state machine enters the StoppingResumeTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopPortTimer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StoppingResumeTimer


PSM_EVENT
HUBPSM30_StoppingTimerAndQueueingHubResetInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StoppingTimerAndQueueingHubResetInDisconnected is called when the
    state machine enters the StoppingTimerAndQueueingHubResetInDisconnected State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ResetHub(portContext);

        syncEvent = HUBMISC_StopPortTimer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StoppingTimerAndQueueingHubResetInDisconnected


PSM_EVENT
HUBPSM30_StoppingTimerOnDisconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StoppingTimerOnDisconnect is called when the
    state machine enters the StoppingTimerOnDisconnect State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopPortTimer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StoppingTimerOnDisconnect


PSM_EVENT
HUBPSM30_StoppingTimerOnOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StoppingTimerOnOverCurrent is called when the
    state machine enters the StoppingTimerOnOverCurrent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopPortTimer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StoppingTimerOnOverCurrent


PSM_EVENT
HUBPSM30_StoppingTimerOnOverCurrentClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StoppingTimerOnOverCurrentClear is called when the
    state machine enters the StoppingTimerOnOverCurrentClear State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopPortTimer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StoppingTimerOnOverCurrentClear


PSM_EVENT
HUBPSM30_StoppingTimerOnResetComplete(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StoppingTimerOnResetComplete is called when the
    state machine enters the StoppingTimerOnResetComplete State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopPortTimer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StoppingTimerOnResetComplete


PSM_EVENT
HUBPSM30_SuperSpeedDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_SuperSpeedDisabled is called when the
    state machine enters the SuperSpeedDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_SuperSpeedDisabled


PSM_EVENT
HUBPSM30_SuperSpeedDisabledStopped(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_SuperSpeedDisabledStopped is called when the
    state machine enters the SuperSpeedDisabledStopped State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_SuperSpeedDisabledStopped


PSM_EVENT
HUBPSM30_Suspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_Suspended is called when the
    state machine enters the Suspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_Suspended


PSM_EVENT
HUBPSM30_SuspendedDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_SuspendedDisabled is called when the
    state machine enters the SuspendedDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_SuspendedDisabled


PSM_EVENT
HUBPSM30_SuspendedInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_SuspendedInD3Cold is called when the
    state machine enters the SuspendedInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_SuspendedInD3Cold


PSM_EVENT
HUBPSM30_Suspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_Suspending is called when the
    state machine enters the Suspending State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_Suspend30PortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_Suspending


PSM_EVENT
HUBPSM30_SuspendingPortOnDisableRequest(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_SuspendingPortOnDisableRequest is called when the
    state machine enters the SuspendingPortOnDisableRequest State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_Suspend30PortUsingControlTransfer(portContext);

        HUBMISC_SetDeviceFlagToForceResetOnEnumeration(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_SuspendingPortOnDisableRequest


PSM_EVENT
HUBPSM30_SuspendingPortOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_SuspendingPortOnHubSuspend is called when the
    state machine enters the SuspendingPortOnHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_Suspend30PortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_SuspendingPortOnHubSuspend


PSM_EVENT
HUBPSM30_WaitingForDebounceTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForDebounceTimer is called when the
    state machine enters the WaitingForDebounceTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_StartDebounceTimer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_WaitingForDebounceTimer


PSM_EVENT
HUBPSM30_WaitingForHubStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForHubStopSuspend is called when the
    state machine enters the WaitingForHubStopSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForHubStopSuspend


PSM_EVENT
HUBPSM30_WaitingForHubStopSuspendInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForHubStopSuspendInDisconnected is called when the
    state machine enters the WaitingForHubStopSuspendInDisconnected State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForHubStopSuspendInDisconnected


PSM_EVENT
HUBPSM30_WaitingForHubStopSuspendOnAttachFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForHubStopSuspendOnAttachFailure is called when the
    state machine enters the WaitingForHubStopSuspendOnAttachFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForHubStopSuspendOnAttachFailure


PSM_EVENT
HUBPSM30_WaitingForHubStopSuspendOnAttachWithResetFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForHubStopSuspendOnAttachWithResetFailure is called when the
    state machine enters the WaitingForHubStopSuspendOnAttachWithResetFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForHubStopSuspendOnAttachWithResetFailure


PSM_EVENT
HUBPSM30_WaitingForHubStopSuspendOrTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForHubStopSuspendOrTimer is called when the
    state machine enters the WaitingForHubStopSuspendOrTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForHubStopSuspendOrTimer


PSM_EVENT
HUBPSM30_WaitingForOldDeviceToDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForOldDeviceToDetach is called when the
    state machine enters the WaitingForOldDeviceToDetach State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForOldDeviceToDetach


PSM_EVENT
HUBPSM30_WaitingForOldDeviceToDetachInConnectedWithResetNeeded(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForOldDeviceToDetachInConnectedWithResetNeeded is called when the
    state machine enters the WaitingForOldDeviceToDetachInConnectedWithResetNeeded State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForOldDeviceToDetachInConnectedWithResetNeeded


PSM_EVENT
HUBPSM30_WaitingForOldDeviceToDetachWithResetInProgress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForOldDeviceToDetachWithResetInProgress is called when the
    state machine enters the WaitingForOldDeviceToDetachWithResetInProgress State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForOldDeviceToDetachWithResetInProgress


PSM_EVENT
HUBPSM30_WaitingForOverCurrentClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForOverCurrentClear is called when the
    state machine enters the WaitingForOverCurrentClear State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForOverCurrentClear


PSM_EVENT
HUBPSM30_WaitingForResetComplete(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForResetComplete is called when the
    state machine enters the WaitingForResetComplete State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_StartResetCompletionTimer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_WaitingForResetComplete


PSM_EVENT
HUBPSM30_WaitingForResetRequestFromDSMWithResetInPorgress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForResetRequestFromDSMWithResetInPorgress is called when the
    state machine enters the WaitingForResetRequestFromDSMWithResetInPorgress State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForResetRequestFromDSMWithResetInPorgress


PSM_EVENT
HUBPSM30_WaitingForResetTimerToFlush(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForResetTimerToFlush is called when the
    state machine enters the WaitingForResetTimerToFlush State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForResetTimerToFlush


PSM_EVENT
HUBPSM30_WaitingForResumeComplete(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForResumeComplete is called when the
    state machine enters the WaitingForResumeComplete State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForResumeComplete


PSM_EVENT
HUBPSM30_WaitingForResumeTimerToFlush(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForResumeTimerToFlush is called when the
    state machine enters the WaitingForResumeTimerToFlush State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForResumeTimerToFlush


PSM_EVENT
HUBPSM30_WaitingForSuperSpeedDisableTimerToFlush(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForSuperSpeedDisableTimerToFlush is called when the
    state machine enters the WaitingForSuperSpeedDisableTimerToFlush State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForSuperSpeedDisableTimerToFlush


PSM_EVENT
HUBPSM30_WaitingForTimerOnResetInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForTimerOnResetInDisconnected is called when the
    state machine enters the WaitingForTimerOnResetInDisconnected State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForTimerOnResetInDisconnected


PSM_EVENT
HUBPSM30_WaitingForTimerToFlushOnDisconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForTimerToFlushOnDisconnect is called when the
    state machine enters the WaitingForTimerToFlushOnDisconnect State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForTimerToFlushOnDisconnect


PSM_EVENT
HUBPSM30_WaitingForTimerToFlushOnHubStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForTimerToFlushOnHubStopSuspend is called when the
    state machine enters the WaitingForTimerToFlushOnHubStopSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForTimerToFlushOnHubStopSuspend


PSM_EVENT
HUBPSM30_WaitingForTimerToFlushOnHubStopSuspendInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForTimerToFlushOnHubStopSuspendInDisconnected is called when the
    state machine enters the WaitingForTimerToFlushOnHubStopSuspendInDisconnected State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForTimerToFlushOnHubStopSuspendInDisconnected


PSM_EVENT
HUBPSM30_WaitingForTimerToFlushOnOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForTimerToFlushOnOverCurrent is called when the
    state machine enters the WaitingForTimerToFlushOnOverCurrent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForTimerToFlushOnOverCurrent


PSM_EVENT
HUBPSM30_WaitingForTimerToFlushOnOverCurrentClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForTimerToFlushOnOverCurrentClear is called when the
    state machine enters the WaitingForTimerToFlushOnOverCurrentClear State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForTimerToFlushOnOverCurrentClear


PSM_EVENT
HUBPSM30_WaitingForUserResetOnOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForUserResetOnOverCurrent is called when the
    state machine enters the WaitingForUserResetOnOverCurrent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForUserResetOnOverCurrent


PSM_EVENT
HUBPSM30_WaitingToBeDeleted(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingToBeDeleted is called when the
    state machine enters the WaitingToBeDeleted State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingToBeDeleted


PSM_EVENT
HUBPSM30_AckingPortChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AckingPortChange is called when the
    state machine enters the AckingPortChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_AckPortChangeUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_AckingPortChange


PSM_EVENT
HUBPSM30_CheckIfThereIsAPortChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckIfThereIsAPortChange is called when the
    state machine enters the CheckIfThereIsAPortChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBHTX_CheckAndSelectIfAny30PortChangeSet(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckIfThereIsAPortChange


PSM_EVENT
HUBPSM30_CheckIfThereIsAPortChangeOnGetPortStatus(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckIfThereIsAPortChangeOnGetPortStatus is called when the
    state machine enters the CheckIfThereIsAPortChangeOnGetPortStatus State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBHTX_CheckAndSelectIfAny30PortChangeSet(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckIfThereIsAPortChangeOnGetPortStatus


PSM_EVENT
HUBPSM30_EnablingInterruptsAndGettingPortEvent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_EnablingInterruptsAndGettingPortEvent is called when the
    state machine enters the EnablingInterruptsAndGettingPortEvent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReEnableInterruptTransfer(portContext);

        syncEvent = HUBHTX_Get30PortChangeEvent(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_EnablingInterruptsAndGettingPortEvent


PSM_EVENT
HUBPSM30_EnablingInterruptsOnFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_EnablingInterruptsOnFailure is called when the
    state machine enters the EnablingInterruptsOnFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReEnableInterruptTransfer(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_EnablingInterruptsOnFailure


PSM_EVENT
HUBPSM30_GettingPortStatus(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatus is called when the
    state machine enters the GettingPortStatus State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_GetPortStatusUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_GettingPortStatus


PSM_EVENT
HUBPSM30_InitializingCumulativePortChangeBits(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_InitializingCumulativePortChangeBits is called when the
    state machine enters the InitializingCumulativePortChangeBits State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_InitializePortChangeBits(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_InitializingCumulativePortChangeBits


PSM_EVENT
HUBPSM30_ReturningPortResetHubOnError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortResetHubOnError is called when the
    state machine enters the ReturningPortResetHubOnError State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortResetHubOnError;

} // HUBPSM30_ReturningPortResetHubOnError


PSM_EVENT
HUBPSM30_AckingPortChangeInNoChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AckingPortChangeInNoChange is called when the
    state machine enters the AckingPortChangeInNoChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_AckPortChangeUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_AckingPortChangeInNoChange


PSM_EVENT
HUBPSM30_CheckIfThereIsAPortChangeInNoChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckIfThereIsAPortChangeInNoChange is called when the
    state machine enters the CheckIfThereIsAPortChangeInNoChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBHTX_CheckAndSelectIfAny30PortChangeSet(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckIfThereIsAPortChangeInNoChange


PSM_EVENT
HUBPSM30_CheckIfThereIsAPortChangeOnGetPortStatusInNoChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckIfThereIsAPortChangeOnGetPortStatusInNoChange is called when the
    state machine enters the CheckIfThereIsAPortChangeOnGetPortStatusInNoChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBHTX_CheckAndSelectIfAny30PortChangeSet(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckIfThereIsAPortChangeOnGetPortStatusInNoChange


PSM_EVENT
HUBPSM30_EnablingInterruptsAndGettingPortEventInNoChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_EnablingInterruptsAndGettingPortEventInNoChange is called when the
    state machine enters the EnablingInterruptsAndGettingPortEventInNoChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBHTX_Get30PortChangeEvent(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_EnablingInterruptsAndGettingPortEventInNoChange


PSM_EVENT
HUBPSM30_GettingPortStatusInNoChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatusInNoChange is called when the
    state machine enters the GettingPortStatusInNoChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_GetPortStatusUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_GettingPortStatusInNoChange


PSM_EVENT
HUBPSM30_InitializingCumulativePortChangeBitsInNoChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_InitializingCumulativePortChangeBitsInNoChange is called when the
    state machine enters the InitializingCumulativePortChangeBitsInNoChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_InitializePortChangeBits(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_InitializingCumulativePortChangeBitsInNoChange


PSM_EVENT
HUBPSM30_ReturningPortResetHubOnErrorInNoChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortResetHubOnErrorInNoChange is called when the
    state machine enters the ReturningPortResetHubOnErrorInNoChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortResetHubOnError;

} // HUBPSM30_ReturningPortResetHubOnErrorInNoChange


PSM_EVENT
HUBPSM30_AckingPortChangeOnStartResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AckingPortChangeOnStartResume is called when the
    state machine enters the AckingPortChangeOnStartResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_AckPortChangeUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_AckingPortChangeOnStartResume


PSM_EVENT
HUBPSM30_AcquiringPortReferenceAndGettingPortEvent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AcquiringPortReferenceAndGettingPortEvent is called when the
    state machine enters the AcquiringPortReferenceAndGettingPortEvent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_AcquirePortPowerReference(portContext);

        HUBHTX_FixPortStateOnStartResume(portContext);

        syncEvent = HUBHTX_Get30PortChangeEvent(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_AcquiringPortReferenceAndGettingPortEvent


PSM_EVENT
HUBPSM30_AcquiringPortReferenceOnFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AcquiringPortReferenceOnFailure is called when the
    state machine enters the AcquiringPortReferenceOnFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_AcquirePortPowerReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_AcquiringPortReferenceOnFailure


PSM_EVENT
HUBPSM30_CheckIfThereIsAPortChangeOnStartResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckIfThereIsAPortChangeOnStartResume is called when the
    state machine enters the CheckIfThereIsAPortChangeOnStartResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBHTX_CheckAndSelectIfAny30PortChangeSet(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckIfThereIsAPortChangeOnStartResume


PSM_EVENT
HUBPSM30_CheckIfThereIsAPortChangeOnStartResumeOnGetPortStatus(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckIfThereIsAPortChangeOnStartResumeOnGetPortStatus is called when the
    state machine enters the CheckIfThereIsAPortChangeOnStartResumeOnGetPortStatus State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBHTX_CheckAndSelectIfAny30PortChangeSet(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckIfThereIsAPortChangeOnStartResumeOnGetPortStatus


PSM_EVENT
HUBPSM30_GettingPortStatusOnStartResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatusOnStartResume is called when the
    state machine enters the GettingPortStatusOnStartResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_GetPortStatusUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_GettingPortStatusOnStartResume


PSM_EVENT
HUBPSM30_InitializingCumulativePortChangeBitsOnStartResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_InitializingCumulativePortChangeBitsOnStartResume is called when the
    state machine enters the InitializingCumulativePortChangeBitsOnStartResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_InitializePortChangeBits(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_InitializingCumulativePortChangeBitsOnStartResume


PSM_EVENT
HUBPSM30_ReturningPortResetHubOnErrorOnStartResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortResetHubOnErrorOnStartResume is called when the
    state machine enters the ReturningPortResetHubOnErrorOnStartResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortResetHubOnError;

} // HUBPSM30_ReturningPortResetHubOnErrorOnStartResume


PSM_EVENT
HUBPSM30_GettingPortChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortChange is called when the
    state machine enters the GettingPortChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_GettingPortChange


PSM_EVENT
HUBPSM30_ReturningConnectChangeOnLinkError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningConnectChangeOnLinkError is called when the
    state machine enters the ReturningConnectChangeOnLinkError State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortConnectChange;

} // HUBPSM30_ReturningConnectChangeOnLinkError


PSM_EVENT
HUBPSM30_ReturningPortResetHubOnErrorInPortChangeHandler(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortResetHubOnErrorInPortChangeHandler is called when the
    state machine enters the ReturningPortResetHubOnErrorInPortChangeHandler State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortResetHubOnError;

} // HUBPSM30_ReturningPortResetHubOnErrorInPortChangeHandler


PSM_EVENT
HUBPSM30_WaitingForPortChangeEvent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForPortChangeEvent is called when the
    state machine enters the WaitingForPortChangeEvent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForPortChangeEvent


PSM_EVENT
HUBPSM30_AcquiringPortReferenceOnTransferFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AcquiringPortReferenceOnTransferFailure is called when the
    state machine enters the AcquiringPortReferenceOnTransferFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_AcquirePortPowerReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_AcquiringPortReferenceOnTransferFailure


PSM_EVENT
HUBPSM30_CheckingIfPortIsPoweredOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfPortIsPoweredOnStart is called when the
    state machine enters the CheckingIfPortIsPoweredOnStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsPortPoweredOn(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfPortIsPoweredOnStart


PSM_EVENT
HUBPSM30_CheckingIfPowerOnStartHackIsPresent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfPowerOnStartHackIsPresent is called when the
    state machine enters the CheckingIfPowerOnStartHackIsPresent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBHTX_CheckIfPowerOnStartHackIsPresent(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfPowerOnStartHackIsPresent


PSM_EVENT
HUBPSM30_GettingPortStatusOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatusOnStart is called when the
    state machine enters the GettingPortStatusOnStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_GettingPortStatusOnStart


PSM_EVENT
HUBPSM30_GettingPortStatusOnStartBeforePoweringOn(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatusOnStartBeforePoweringOn is called when the
    state machine enters the GettingPortStatusOnStartBeforePoweringOn State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_GetPortStatusUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_GettingPortStatusOnStartBeforePoweringOn


PSM_EVENT
HUBPSM30_PoweringOnPortOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_PoweringOnPortOnStart is called when the
    state machine enters the PoweringOnPortOnStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_SettingPortPowerUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_PoweringOnPortOnStart


PSM_EVENT
HUBPSM30_ReturningPortConnectChangeOnLinkErrorOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortConnectChangeOnLinkErrorOnStart is called when the
    state machine enters the ReturningPortConnectChangeOnLinkErrorOnStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortConnectChange;

} // HUBPSM30_ReturningPortConnectChangeOnLinkErrorOnStart


PSM_EVENT
HUBPSM30_ReturningPortResetHubOnErrorOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortResetHubOnErrorOnStart is called when the
    state machine enters the ReturningPortResetHubOnErrorOnStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortResetHubOnError;

} // HUBPSM30_ReturningPortResetHubOnErrorOnStart


PSM_EVENT
HUBPSM30_WaitingForPortPowerOnTimerOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForPortPowerOnTimerOnStart is called when the
    state machine enters the WaitingForPortPowerOnTimerOnStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_StartPortPowerOnTimer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_WaitingForPortPowerOnTimerOnStart


PSM_EVENT
HUBPSM30_CheckingIfDeviceIsStillConnectedOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfDeviceIsStillConnectedOnResume is called when the
    state machine enters the CheckingIfDeviceIsStillConnectedOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsConnectBitOne(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfDeviceIsStillConnectedOnResume


PSM_EVENT
HUBPSM30_GettingPortStatusOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatusOnResume is called when the
    state machine enters the GettingPortStatusOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_GettingPortStatusOnResume


PSM_EVENT
HUBPSM30_GettingPortStatusOnResumeAfterWaiting(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatusOnResumeAfterWaiting is called when the
    state machine enters the GettingPortStatusOnResumeAfterWaiting State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_GettingPortStatusOnResumeAfterWaiting


PSM_EVENT
HUBPSM30_QueueingStateDisabledOnConnectChangeOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueingStateDisabledOnConnectChangeOnResume is called when the
    state machine enters the QueueingStateDisabledOnConnectChangeOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortStateEnabledOnReconnect);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueingStateDisabledOnConnectChangeOnResume


PSM_EVENT
HUBPSM30_QueueingStateDisabledOnFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueingStateDisabledOnFailure is called when the
    state machine enters the QueueingStateDisabledOnFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_SetDeviceFlagToForceResetOnEnumeration(portContext);

        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortStateDisabled);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueingStateDisabledOnFailure


PSM_EVENT
HUBPSM30_QueueingStateDisabledOnResetInProgress(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueingStateDisabledOnResetInProgress is called when the
    state machine enters the QueueingStateDisabledOnResetInProgress State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortStateDisabled);

        HUBMISC_SetDeviceFlagToForceResetOnEnumeration(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueingStateDisabledOnResetInProgress


PSM_EVENT
HUBPSM30_QueueingStateEnabledOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueingStateEnabledOnResume is called when the
    state machine enters the QueueingStateEnabledOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortStateEnabled);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueingStateEnabledOnResume


PSM_EVENT
HUBPSM30_QueueingStateSuspendedOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueingStateSuspendedOnResume is called when the
    state machine enters the QueueingStateSuspendedOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortStateSuspended);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueingStateSuspendedOnResume


PSM_EVENT
HUBPSM30_ReturningHubStopOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningHubStopOnResume is called when the
    state machine enters the ReturningHubStopOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventHubStop;

} // HUBPSM30_ReturningHubStopOnResume


PSM_EVENT
HUBPSM30_ReturningPortConnectChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortConnectChange is called when the
    state machine enters the ReturningPortConnectChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortConnectChange;

} // HUBPSM30_ReturningPortConnectChange


PSM_EVENT
HUBPSM30_ReturningPortNeedsResetOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortNeedsResetOnResume is called when the
    state machine enters the ReturningPortNeedsResetOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortNeedsReset;

} // HUBPSM30_ReturningPortNeedsResetOnResume


PSM_EVENT
HUBPSM30_ReturningPortOperationSuccessOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortOperationSuccessOnResume is called when the
    state machine enters the ReturningPortOperationSuccessOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventOperationSuccess;

} // HUBPSM30_ReturningPortOperationSuccessOnResume


PSM_EVENT
HUBPSM30_ReturningPortResetHubOnErrorOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortResetHubOnErrorOnResume is called when the
    state machine enters the ReturningPortResetHubOnErrorOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortResetHubOnError;

} // HUBPSM30_ReturningPortResetHubOnErrorOnResume


PSM_EVENT
HUBPSM30_ReturningPortResetInProgressOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortResetInProgressOnResume is called when the
    state machine enters the ReturningPortResetInProgressOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortResetInProgress;

} // HUBPSM30_ReturningPortResetInProgressOnResume


PSM_EVENT
HUBPSM30_ReturningPortResumedOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortResumedOnResume is called when the
    state machine enters the ReturningPortResumedOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortResumed;

} // HUBPSM30_ReturningPortResumedOnResume


PSM_EVENT
HUBPSM30_StartingTimerForAllowingReconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StartingTimerForAllowingReconnect is called when the
    state machine enters the StartingTimerForAllowingReconnect State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_StartReconnectOnResumeTimer(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StartingTimerForAllowingReconnect


PSM_EVENT
HUBPSM30_StoppingReconnectTimerOnHubStatusChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StoppingReconnectTimerOnHubStatusChange is called when the
    state machine enters the StoppingReconnectTimerOnHubStatusChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopPortTimer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StoppingReconnectTimerOnHubStatusChange


PSM_EVENT
HUBPSM30_StoppingReconnectTimerOnHubStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StoppingReconnectTimerOnHubStop is called when the
    state machine enters the StoppingReconnectTimerOnHubStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopPortTimer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StoppingReconnectTimerOnHubStop


PSM_EVENT
HUBPSM30_WaitingForDeviceToReconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForDeviceToReconnect is called when the
    state machine enters the WaitingForDeviceToReconnect State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForDeviceToReconnect


PSM_EVENT
HUBPSM30_WaitingForReconnectTimerToFlushOnHubStatusChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForReconnectTimerToFlushOnHubStatusChange is called when the
    state machine enters the WaitingForReconnectTimerToFlushOnHubStatusChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForReconnectTimerToFlushOnHubStatusChange


PSM_EVENT
HUBPSM30_WaitingForReconnectTimerToFlushOnHubStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForReconnectTimerToFlushOnHubStop is called when the
    state machine enters the WaitingForReconnectTimerToFlushOnHubStop State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForReconnectTimerToFlushOnHubStop


PSM_EVENT
HUBPSM30_GettingPortStatusInResuming(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatusInResuming is called when the
    state machine enters the GettingPortStatusInResuming State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_GettingPortStatusInResuming


PSM_EVENT
HUBPSM30_ReturningConnectChangeOnLinkErrorInResuming(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningConnectChangeOnLinkErrorInResuming is called when the
    state machine enters the ReturningConnectChangeOnLinkErrorInResuming State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortConnectChange;

} // HUBPSM30_ReturningConnectChangeOnLinkErrorInResuming


PSM_EVENT
HUBPSM30_ReturningPortResetHubOnErrorInResuming(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortResetHubOnErrorInResuming is called when the
    state machine enters the ReturningPortResetHubOnErrorInResuming State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortResetHubOnError;

} // HUBPSM30_ReturningPortResetHubOnErrorInResuming


PSM_EVENT
HUBPSM30_WaitingForPortChangeEventInResuming(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForPortChangeEventInResuming is called when the
    state machine enters the WaitingForPortChangeEventInResuming State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForPortChangeEventInResuming


PSM_EVENT
HUBPSM30_GettingPortStatusInWaitingForStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatusInWaitingForStopSuspend is called when the
    state machine enters the GettingPortStatusInWaitingForStopSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_GettingPortStatusInWaitingForStopSuspend


PSM_EVENT
HUBPSM30_QueueingPortEventFailureToDSM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueingPortEventFailureToDSM is called when the
    state machine enters the QueueingPortEventFailureToDSM State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortFailed);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueingPortEventFailureToDSM


PSM_EVENT
HUBPSM30_WaitingForDevicePortEvents(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForDevicePortEvents is called when the
    state machine enters the WaitingForDevicePortEvents State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForDevicePortEvents


PSM_EVENT
HUBPSM30_GettingPortStatusInWaitingForStopSuspendInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatusInWaitingForStopSuspendInDisconnected is called when the
    state machine enters the GettingPortStatusInWaitingForStopSuspendInDisconnected State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_GettingPortStatusInWaitingForStopSuspendInDisconnected


PSM_EVENT
HUBPSM30_WaitingForDevicePortEventsInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForDevicePortEventsInDisconnected is called when the
    state machine enters the WaitingForDevicePortEventsInDisconnected State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForDevicePortEventsInDisconnected


PSM_EVENT
HUBPSM30_CheckingIfLinkIsInU0AfterResetPolling(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfLinkIsInU0AfterResetPolling is called when the
    state machine enters the CheckingIfLinkIsInU0AfterResetPolling State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_IsLinkStateU0(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfLinkIsInU0AfterResetPolling


PSM_EVENT
HUBPSM30_CheckingIfResetCompletionRequiresPolling(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfResetCompletionRequiresPolling is called when the
    state machine enters the CheckingIfResetCompletionRequiresPolling State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfResetCompletionRequiresPolling(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfResetCompletionRequiresPolling


PSM_EVENT
HUBPSM30_GettingPortStatusInReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatusInReset is called when the
    state machine enters the GettingPortStatusInReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_GettingPortStatusInReset


PSM_EVENT
HUBPSM30_GettingPortStatusInResetAfterPollingInterval(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_GettingPortStatusInResetAfterPollingInterval is called when the
    state machine enters the GettingPortStatusInResetAfterPollingInterval State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_GettingPortStatusInResetAfterPollingInterval


PSM_EVENT
HUBPSM30_ReturningPortResetCompleted(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortResetCompleted is called when the
    state machine enters the ReturningPortResetCompleted State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortResetComplete;

} // HUBPSM30_ReturningPortResetCompleted


PSM_EVENT
HUBPSM30_ReturningPortResetHubOnErrorInReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortResetHubOnErrorInReset is called when the
    state machine enters the ReturningPortResetHubOnErrorInReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortResetHubOnError;

} // HUBPSM30_ReturningPortResetHubOnErrorInReset


PSM_EVENT
HUBPSM30_WaitingForPollingIntervalForResetCompletion(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForPollingIntervalForResetCompletion is called when the
    state machine enters the WaitingForPollingIntervalForResetCompletion State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_StartTimerForResetCompletionPolling(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_WaitingForPollingIntervalForResetCompletion


PSM_EVENT
HUBPSM30_WaitingForPortChangeEventInReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForPortChangeEventInReset is called when the
    state machine enters the WaitingForPortChangeEventInReset State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForPortChangeEventInReset


PSM_EVENT
HUBPSM30_CheckingIfBootDeviceIsConnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfBootDeviceIsConnected is called when the
    state machine enters the CheckingIfBootDeviceIsConnected State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfConnectedDeviceSupportsReAttach(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfBootDeviceIsConnected


PSM_EVENT
HUBPSM30_CheckingIfParentHubIsArmedForWake(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfParentHubIsArmedForWake is called when the
    state machine enters the CheckingIfParentHubIsArmedForWake State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_CheckIfHubIsArmedForWake(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfParentHubIsArmedForWake


PSM_EVENT
HUBPSM30_DisablingRemoteWakeOnPort(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_DisablingRemoteWakeOnPort is called when the
    state machine enters the DisablingRemoteWakeOnPort State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_DisableRemoteWakeOnPortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_DisablingRemoteWakeOnPort


PSM_EVENT
HUBPSM30_EnablingRemoteWakeOnPort(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_EnablingRemoteWakeOnPort is called when the
    state machine enters the EnablingRemoteWakeOnPort State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_EnableRemoteWakeOnPortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM30_EnablingRemoteWakeOnPort


PSM_EVENT
HUBPSM30_IssuingHubResetOnRemoteWakeMaskFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_IssuingHubResetOnRemoteWakeMaskFailure is called when the
    state machine enters the IssuingHubResetOnRemoteWakeMaskFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ResetHub(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_IssuingHubResetOnRemoteWakeMaskFailure


PSM_EVENT
HUBPSM30_ReturningOperationSucessFromArmingForWake(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningOperationSucessFromArmingForWake is called when the
    state machine enters the ReturningOperationSucessFromArmingForWake State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventOperationSuccess;

} // HUBPSM30_ReturningOperationSucessFromArmingForWake


PSM_EVENT
HUBPSM30_AcquiringPowerReferenceOnHubResumeInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_AcquiringPowerReferenceOnHubResumeInD3Cold is called when the
    state machine enters the AcquiringPowerReferenceOnHubResumeInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_AcquirePortPowerReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_AcquiringPowerReferenceOnHubResumeInD3Cold


PSM_EVENT
HUBPSM30_CheckingIfDeviceIsConnectedOnConnectChangeForD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfDeviceIsConnectedOnConnectChangeForD3Cold is called when the
    state machine enters the CheckingIfDeviceIsConnectedOnConnectChangeForD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_Is30DevicePresent(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfDeviceIsConnectedOnConnectChangeForD3Cold


PSM_EVENT
HUBPSM30_CheckingIfDeviceIsConnectedOnPortChangeInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfDeviceIsConnectedOnPortChangeInD3Cold is called when the
    state machine enters the CheckingIfDeviceIsConnectedOnPortChangeInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_Is30DevicePresent(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfDeviceIsConnectedOnPortChangeInD3Cold


PSM_EVENT
HUBPSM30_CheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_CheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume is called when the
    state machine enters the CheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_Is30DevicePresent(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_CheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume


PSM_EVENT
HUBPSM30_PoweredOffOnHubSuspendFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_PoweredOffOnHubSuspendFromD3Cold is called when the
    state machine enters the PoweredOffOnHubSuspendFromD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_PoweredOffOnHubSuspendFromD3Cold


PSM_EVENT
HUBPSM30_QueueingEnabledToDSMOnReconnectAfterResumeRequestFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueingEnabledToDSMOnReconnectAfterResumeRequestFromD3Cold is called when the
    state machine enters the QueueingEnabledToDSMOnReconnectAfterResumeRequestFromD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortStateEnabledOnReconnect);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueingEnabledToDSMOnReconnectAfterResumeRequestFromD3Cold


PSM_EVENT
HUBPSM30_QueueingEnabledToDSMOnReconnectFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueingEnabledToDSMOnReconnectFromD3Cold is called when the
    state machine enters the QueueingEnabledToDSMOnReconnectFromD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortStateEnabledOnReconnect);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueingEnabledToDSMOnReconnectFromD3Cold


PSM_EVENT
HUBPSM30_QueueingStateSuspendedOnHubResumeInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_QueueingStateSuspendedOnHubResumeInD3Cold is called when the
    state machine enters the QueueingStateSuspendedOnHubResumeInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortStateSuspended);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_QueueingStateSuspendedOnHubResumeInD3Cold


PSM_EVENT
HUBPSM30_ReleasingPowerReferenceOnHubS0IdleInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReleasingPowerReferenceOnHubS0IdleInD3Cold is called when the
    state machine enters the ReleasingPowerReferenceOnHubS0IdleInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReleasePortPowerReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_ReleasingPowerReferenceOnHubS0IdleInD3Cold


PSM_EVENT
HUBPSM30_ReturningConnectChangeFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningConnectChangeFromD3Cold is called when the
    state machine enters the ReturningConnectChangeFromD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortConnectChange;

} // HUBPSM30_ReturningConnectChangeFromD3Cold


PSM_EVENT
HUBPSM30_ReturningHubStopFromSuspendedInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningHubStopFromSuspendedInD3Cold is called when the
    state machine enters the ReturningHubStopFromSuspendedInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventHubStop;

} // HUBPSM30_ReturningHubStopFromSuspendedInD3Cold


PSM_EVENT
HUBPSM30_ReturningOverCurrentClearedFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningOverCurrentClearedFromD3Cold is called when the
    state machine enters the ReturningOverCurrentClearedFromD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortOverCurrentCleared;

} // HUBPSM30_ReturningOverCurrentClearedFromD3Cold


PSM_EVENT
HUBPSM30_ReturningOverCurrentFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningOverCurrentFromD3Cold is called when the
    state machine enters the ReturningOverCurrentFromD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortOverCurrent;

} // HUBPSM30_ReturningOverCurrentFromD3Cold


PSM_EVENT
HUBPSM30_ReturningPortResetHubOnErrorFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortResetHubOnErrorFromD3Cold is called when the
    state machine enters the ReturningPortResetHubOnErrorFromD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortResetHubOnError;

} // HUBPSM30_ReturningPortResetHubOnErrorFromD3Cold


PSM_EVENT
HUBPSM30_ReturningPortResumedFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_ReturningPortResumedFromD3Cold is called when the
    state machine enters the ReturningPortResumedFromD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortResumed;

} // HUBPSM30_ReturningPortResumedFromD3Cold


PSM_EVENT
HUBPSM30_StartingTimerForAllowingReconnectOnResumingFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StartingTimerForAllowingReconnectOnResumingFromD3Cold is called when the
    state machine enters the StartingTimerForAllowingReconnectOnResumingFromD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_StartD3ColdReconnectTimer(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StartingTimerForAllowingReconnectOnResumingFromD3Cold


PSM_EVENT
HUBPSM30_StoppingReconnectTimerOnDeviceReAttachAfterD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StoppingReconnectTimerOnDeviceReAttachAfterD3Cold is called when the
    state machine enters the StoppingReconnectTimerOnDeviceReAttachAfterD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopPortTimer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StoppingReconnectTimerOnDeviceReAttachAfterD3Cold


PSM_EVENT
HUBPSM30_StoppingReconnectTimerOnHubStopInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StoppingReconnectTimerOnHubStopInD3Cold is called when the
    state machine enters the StoppingReconnectTimerOnHubStopInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopPortTimer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StoppingReconnectTimerOnHubStopInD3Cold


PSM_EVENT
HUBPSM30_StoppingReconnectTimerOnOvercurrentClearedInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StoppingReconnectTimerOnOvercurrentClearedInD3Cold is called when the
    state machine enters the StoppingReconnectTimerOnOvercurrentClearedInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopPortTimer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StoppingReconnectTimerOnOvercurrentClearedInD3Cold


PSM_EVENT
HUBPSM30_StoppingReconnectTimerOnOvercurrentInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StoppingReconnectTimerOnOvercurrentInD3Cold is called when the
    state machine enters the StoppingReconnectTimerOnOvercurrentInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopPortTimer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StoppingReconnectTimerOnOvercurrentInD3Cold


PSM_EVENT
HUBPSM30_StoppingReconnectTimerOnResetHubErrorInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_StoppingReconnectTimerOnResetHubErrorInD3Cold is called when the
    state machine enters the StoppingReconnectTimerOnResetHubErrorInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    PSM_EVENT                       syncEvent;
    
    TRY {

        portContext = SmContext->ParentContext;
        syncEvent = HUBMISC_StopPortTimer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM30_StoppingReconnectTimerOnResetHubErrorInD3Cold


PSM_EVENT
HUBPSM30_WaitingForDeviceRequestResumeOnD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForDeviceRequestResumeOnD3Cold is called when the
    state machine enters the WaitingForDeviceRequestResumeOnD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForDeviceRequestResumeOnD3Cold


PSM_EVENT
HUBPSM30_WaitingForDeviceToReconnectOnResumeFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForDeviceToReconnectOnResumeFromD3Cold is called when the
    state machine enters the WaitingForDeviceToReconnectOnResumeFromD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForDeviceToReconnectOnResumeFromD3Cold


PSM_EVENT
HUBPSM30_WaitingForPortChangeEventInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForPortChangeEventInD3Cold is called when the
    state machine enters the WaitingForPortChangeEventInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForPortChangeEventInD3Cold


PSM_EVENT
HUBPSM30_WaitingForReconnectTimerToFlushOnHubStopInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForReconnectTimerToFlushOnHubStopInD3Cold is called when the
    state machine enters the WaitingForReconnectTimerToFlushOnHubStopInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForReconnectTimerToFlushOnHubStopInD3Cold


PSM_EVENT
HUBPSM30_WaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold is called when the
    state machine enters the WaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold


PSM_EVENT
HUBPSM30_WaitingForReconnectTimerToFlushOnOvercurrentInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForReconnectTimerToFlushOnOvercurrentInD3Cold is called when the
    state machine enters the WaitingForReconnectTimerToFlushOnOvercurrentInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForReconnectTimerToFlushOnOvercurrentInD3Cold


PSM_EVENT
HUBPSM30_WaitingForReconnectTimerToFlushOnReattachAfterD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForReconnectTimerToFlushOnReattachAfterD3Cold is called when the
    state machine enters the WaitingForReconnectTimerToFlushOnReattachAfterD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForReconnectTimerToFlushOnReattachAfterD3Cold


PSM_EVENT
HUBPSM30_WaitingForReconnectTimerToFlushOnResetHubErrorInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM30_WaitingForReconnectTimerToFlushOnResetHubErrorInD3Cold is called when the
    state machine enters the WaitingForReconnectTimerToFlushOnResetHubErrorInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    //
    // This State Entry function does not do anything.

    //
    return PsmEventNull;

} // HUBPSM30_WaitingForReconnectTimerToFlushOnResetHubErrorInD3Cold





