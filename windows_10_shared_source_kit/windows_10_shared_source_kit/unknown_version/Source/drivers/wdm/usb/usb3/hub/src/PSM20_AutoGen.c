
/*++
Copyright (c) Microsoft Corporation

Module Name:

PSM20_AutoGen.c

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/


#include <pch.h>

PSM_EVENT
HUBPSM20_AcquiringInterruptReferenceOnHSMInDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AcquiringInterruptReferenceOnHSMInDisabled is called when the
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

} // HUBPSM20_AcquiringInterruptReferenceOnHSMInDisabled


PSM_EVENT
HUBPSM20_AcquiringInterruptReferenceOnHSMInEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AcquiringInterruptReferenceOnHSMInEnabled is called when the
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

} // HUBPSM20_AcquiringInterruptReferenceOnHSMInEnabled


PSM_EVENT
HUBPSM20_AcquiringInterruptReferenceOnResuming(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AcquiringInterruptReferenceOnResuming is called when the
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

} // HUBPSM20_AcquiringInterruptReferenceOnResuming


PSM_EVENT
HUBPSM20_AcquiringPowerReferenceFromPoweredOffDisabledInS0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AcquiringPowerReferenceFromPoweredOffDisabledInS0 is called when the
    state machine enters the AcquiringPowerReferenceFromPoweredOffDisabledInS0 State

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

} // HUBPSM20_AcquiringPowerReferenceFromPoweredOffDisabledInS0


PSM_EVENT
HUBPSM20_AcquiringPowerReferenceFromPoweredOffDisconnectedInS0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AcquiringPowerReferenceFromPoweredOffDisconnectedInS0 is called when the
    state machine enters the AcquiringPowerReferenceFromPoweredOffDisconnectedInS0 State

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

} // HUBPSM20_AcquiringPowerReferenceFromPoweredOffDisconnectedInS0


PSM_EVENT
HUBPSM20_AcquiringPowerReferenceFromPoweredOffSuspendedInS0(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AcquiringPowerReferenceFromPoweredOffSuspendedInS0 is called when the
    state machine enters the AcquiringPowerReferenceFromPoweredOffSuspendedInS0 State

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

} // HUBPSM20_AcquiringPowerReferenceFromPoweredOffSuspendedInS0


PSM_EVENT
HUBPSM20_CancellingDisableOnHubStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CancellingDisableOnHubStopSuspend is called when the
    state machine enters the CancellingDisableOnHubStopSuspend State

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

} // HUBPSM20_CancellingDisableOnHubStopSuspend


PSM_EVENT
HUBPSM20_CancellingDisablingPortOnPortCycleOnHubStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CancellingDisablingPortOnPortCycleOnHubStop is called when the
    state machine enters the CancellingDisablingPortOnPortCycleOnHubStop State

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

} // HUBPSM20_CancellingDisablingPortOnPortCycleOnHubStop


PSM_EVENT
HUBPSM20_CancellingDisablingPortOnPortCycleWithTimerOnHubStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CancellingDisablingPortOnPortCycleWithTimerOnHubStop is called when the
    state machine enters the CancellingDisablingPortOnPortCycleWithTimerOnHubStop State

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

} // HUBPSM20_CancellingDisablingPortOnPortCycleWithTimerOnHubStop


PSM_EVENT
HUBPSM20_CancellingQueryPortStatus(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CancellingQueryPortStatus is called when the
    state machine enters the CancellingQueryPortStatus State

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

} // HUBPSM20_CancellingQueryPortStatus


PSM_EVENT
HUBPSM20_CancellingResetOnSurpriseRemove(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CancellingResetOnSurpriseRemove is called when the
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

} // HUBPSM20_CancellingResetOnSurpriseRemove


PSM_EVENT
HUBPSM20_CancellingSetPortPower(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CancellingSetPortPower is called when the
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

} // HUBPSM20_CancellingSetPortPower


PSM_EVENT
HUBPSM20_CheckingIfConnectBitOne(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckingIfConnectBitOne is called when the
    state machine enters the CheckingIfConnectBitOne State

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

} // HUBPSM20_CheckingIfConnectBitOne


PSM_EVENT
HUBPSM20_CheckingIfOvercurrentBitOne(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckingIfOvercurrentBitOne is called when the
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

} // HUBPSM20_CheckingIfOvercurrentBitOne


PSM_EVENT
HUBPSM20_CheckingIfPersistentOvercurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckingIfPersistentOvercurrent is called when the
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

} // HUBPSM20_CheckingIfPersistentOvercurrent


PSM_EVENT
HUBPSM20_ConnectedDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ConnectedDisabled is called when the
    state machine enters the ConnectedDisabled State

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

} // HUBPSM20_ConnectedDisabled


PSM_EVENT
HUBPSM20_ConnectedEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ConnectedEnabled is called when the
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

} // HUBPSM20_ConnectedEnabled


PSM_EVENT
HUBPSM20_CreatingDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CreatingDevice is called when the
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

} // HUBPSM20_CreatingDevice


PSM_EVENT
HUBPSM20_DetachingDeviceFromPortOnCycleOnHubStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DetachingDeviceFromPortOnCycleOnHubStop is called when the
    state machine enters the DetachingDeviceFromPortOnCycleOnHubStop State

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

} // HUBPSM20_DetachingDeviceFromPortOnCycleOnHubStop


PSM_EVENT
HUBPSM20_DetachingDeviceFromPortOnDisconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DetachingDeviceFromPortOnDisconnect is called when the
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

} // HUBPSM20_DetachingDeviceFromPortOnDisconnect


PSM_EVENT
HUBPSM20_DetachingDeviceFromPortOnDisconnectWithTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DetachingDeviceFromPortOnDisconnectWithTimer is called when the
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
        HUBMUX_ReleaseInterruptReference(portContext);

        HUBMISC_DetachDevice(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_DetachingDeviceFromPortOnDisconnectWithTimer


PSM_EVENT
HUBPSM20_DetachingDeviceFromPortOnOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DetachingDeviceFromPortOnOverCurrent is called when the
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

} // HUBPSM20_DetachingDeviceFromPortOnOverCurrent


PSM_EVENT
HUBPSM20_DetachingDeviceFromPortOnOverCurrentClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DetachingDeviceFromPortOnOverCurrentClear is called when the
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

} // HUBPSM20_DetachingDeviceFromPortOnOverCurrentClear


PSM_EVENT
HUBPSM20_DetachingDeviceFromPortOnOverCurrentClearWithTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DetachingDeviceFromPortOnOverCurrentClearWithTimer is called when the
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

} // HUBPSM20_DetachingDeviceFromPortOnOverCurrentClearWithTimer


PSM_EVENT
HUBPSM20_DetachingDeviceFromPortOnOverCurrentWithTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DetachingDeviceFromPortOnOverCurrentWithTimer is called when the
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
        HUBMUX_ReleaseInterruptReference(portContext);

        HUBMISC_DetachDevice(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_DetachingDeviceFromPortOnOverCurrentWithTimer


PSM_EVENT
HUBPSM20_DetachingDeviceFromPortOnPortCycle(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DetachingDeviceFromPortOnPortCycle is called when the
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

} // HUBPSM20_DetachingDeviceFromPortOnPortCycle


PSM_EVENT
HUBPSM20_DetachingDeviceOnCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DetachingDeviceOnCleanup is called when the
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

} // HUBPSM20_DetachingDeviceOnCleanup


PSM_EVENT
HUBPSM20_DetachingDeviceOnDisableAndCycle(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DetachingDeviceOnDisableAndCycle is called when the
    state machine enters the DetachingDeviceOnDisableAndCycle State

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

} // HUBPSM20_DetachingDeviceOnDisableAndCycle


PSM_EVENT
HUBPSM20_DetachingDeviceOnHubReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DetachingDeviceOnHubReset is called when the
    state machine enters the DetachingDeviceOnHubReset State

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

} // HUBPSM20_DetachingDeviceOnHubReset


PSM_EVENT
HUBPSM20_DetachingDeviceOnPortCycleWithTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DetachingDeviceOnPortCycleWithTimer is called when the
    state machine enters the DetachingDeviceOnPortCycleWithTimer State

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

        HUBMISC_DetachDevice(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_DetachingDeviceOnPortCycleWithTimer


PSM_EVENT
HUBPSM20_DetachingDeviceOnPortDisableAndCycleWithTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DetachingDeviceOnPortDisableAndCycleWithTimer is called when the
    state machine enters the DetachingDeviceOnPortDisableAndCycleWithTimer State

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

        HUBMISC_DetachDevice(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_DetachingDeviceOnPortDisableAndCycleWithTimer


PSM_EVENT
HUBPSM20_DisablingOnHubSuspendWithTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DisablingOnHubSuspendWithTimer is called when the
    state machine enters the DisablingOnHubSuspendWithTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_DisablePortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_DisablingOnHubSuspendWithTimer


PSM_EVENT
HUBPSM20_DisablingPortBeforeConnecting(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DisablingPortBeforeConnecting is called when the
    state machine enters the DisablingPortBeforeConnecting State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_DisablePortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_DisablingPortBeforeConnecting


PSM_EVENT
HUBPSM20_DisablingPortOnCycle(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DisablingPortOnCycle is called when the
    state machine enters the DisablingPortOnCycle State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_DisablePortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_DisablingPortOnCycle


PSM_EVENT
HUBPSM20_DisablingPortOnDeviceRequest(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DisablingPortOnDeviceRequest is called when the
    state machine enters the DisablingPortOnDeviceRequest State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_DisablePortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_DisablingPortOnDeviceRequest


PSM_EVENT
HUBPSM20_DisablingPortOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DisablingPortOnHubSuspend is called when the
    state machine enters the DisablingPortOnHubSuspend State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_DisablePortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_DisablingPortOnHubSuspend


PSM_EVENT
HUBPSM20_DisablingPortOnPortCycleWithTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DisablingPortOnPortCycleWithTimer is called when the
    state machine enters the DisablingPortOnPortCycleWithTimer State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_DisablePortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_DisablingPortOnPortCycleWithTimer


PSM_EVENT
HUBPSM20_DisablingPortOnTimeOut(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_DisablingPortOnTimeOut is called when the
    state machine enters the DisablingPortOnTimeOut State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBHTX_DisablePortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_DisablingPortOnTimeOut


PSM_EVENT
HUBPSM20_Disconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_Disconnected is called when the
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

} // HUBPSM20_Disconnected


PSM_EVENT
HUBPSM20_ErrorOnAcquringReferenceOnHubResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnAcquringReferenceOnHubResume is called when the
    state machine enters the ErrorOnAcquringReferenceOnHubResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortIgnoreError;

} // HUBPSM20_ErrorOnAcquringReferenceOnHubResume


PSM_EVENT
HUBPSM20_ErrorOnConnectedDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnConnectedDisabled is called when the
    state machine enters the ErrorOnConnectedDisabled State

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
        syncEvent = HUBHTX_GetGenericErrorResponseEventForInvalidPortStatus(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ErrorOnConnectedDisabled


PSM_EVENT
HUBPSM20_ErrorOnConnectedEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnConnectedEnabled is called when the
    state machine enters the ErrorOnConnectedEnabled State

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
        syncEvent = HUBHTX_GetGenericErrorResponseEventForInvalidPortStatus(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ErrorOnConnectedEnabled


PSM_EVENT
HUBPSM20_ErrorOnDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnDisconnected is called when the
    state machine enters the ErrorOnDisconnected State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortIgnoreError;

} // HUBPSM20_ErrorOnDisconnected


PSM_EVENT
HUBPSM20_ErrorOnIssuingResetComplete(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnIssuingResetComplete is called when the
    state machine enters the ErrorOnIssuingResetComplete State

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
        syncEvent = HUBHTX_GetGenericErrorResponseEventForInvalidPortStatus(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ErrorOnIssuingResetComplete


PSM_EVENT
HUBPSM20_ErrorOnResettingEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnResettingEnabled is called when the
    state machine enters the ErrorOnResettingEnabled State

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
        syncEvent = HUBHTX_GetErrorResponseEventForInvalidPortStatusInPendingReset(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ErrorOnResettingEnabled


PSM_EVENT
HUBPSM20_ErrorOnResettingPort(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnResettingPort is called when the
    state machine enters the ErrorOnResettingPort State

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
        syncEvent = HUBHTX_GetErrorResponseEventForInvalidPortStatusInPendingReset(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ErrorOnResettingPort


PSM_EVENT
HUBPSM20_ErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled is called when the
    state machine enters the ErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortIgnoreError;

} // HUBPSM20_ErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled


PSM_EVENT
HUBPSM20_ErrorOnStoppingTimerOnDisconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnStoppingTimerOnDisconnect is called when the
    state machine enters the ErrorOnStoppingTimerOnDisconnect State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortIgnoreError;

} // HUBPSM20_ErrorOnStoppingTimerOnDisconnect


PSM_EVENT
HUBPSM20_ErrorOnStoppingTimerOnOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnStoppingTimerOnOverCurrent is called when the
    state machine enters the ErrorOnStoppingTimerOnOverCurrent State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortIgnoreError;

} // HUBPSM20_ErrorOnStoppingTimerOnOverCurrent


PSM_EVENT
HUBPSM20_ErrorOnStoppingTimerOnOverCurrentClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnStoppingTimerOnOverCurrentClear is called when the
    state machine enters the ErrorOnStoppingTimerOnOverCurrentClear State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortIgnoreError;

} // HUBPSM20_ErrorOnStoppingTimerOnOverCurrentClear


PSM_EVENT
HUBPSM20_ErrorOnStoppingTimerOnPortCycle(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnStoppingTimerOnPortCycle is called when the
    state machine enters the ErrorOnStoppingTimerOnPortCycle State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortIgnoreError;

} // HUBPSM20_ErrorOnStoppingTimerOnPortCycle


PSM_EVENT
HUBPSM20_ErrorOnSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnSuspended is called when the
    state machine enters the ErrorOnSuspended State

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
        syncEvent = HUBHTX_GetGenericErrorResponseEventForInvalidPortStatus(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ErrorOnSuspended


PSM_EVENT
HUBPSM20_ErrorOnWaitingForCompanionPort(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnWaitingForCompanionPort is called when the
    state machine enters the ErrorOnWaitingForCompanionPort State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortIgnoreError;

} // HUBPSM20_ErrorOnWaitingForCompanionPort


PSM_EVENT
HUBPSM20_ErrorOnWaitingForDebounce(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnWaitingForDebounce is called when the
    state machine enters the ErrorOnWaitingForDebounce State

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
        syncEvent = HUBHTX_GetGenericErrorResponseEventForInvalidPortStatus(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ErrorOnWaitingForDebounce


PSM_EVENT
HUBPSM20_ErrorOnWaitingForDSMResumeResponse(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnWaitingForDSMResumeResponse is called when the
    state machine enters the ErrorOnWaitingForDSMResumeResponse State

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
        syncEvent = HUBHTX_GetGenericErrorResponseEventForInvalidPortStatus(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ErrorOnWaitingForDSMResumeResponse


PSM_EVENT
HUBPSM20_ErrorOnWaitingForOldDeviceToCleanup(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnWaitingForOldDeviceToCleanup is called when the
    state machine enters the ErrorOnWaitingForOldDeviceToCleanup State

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
        syncEvent = HUBHTX_GetGenericErrorResponseEventForInvalidPortStatus(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ErrorOnWaitingForOldDeviceToCleanup


PSM_EVENT
HUBPSM20_ErrorOnWaitingForOverCurrentClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnWaitingForOverCurrentClear is called when the
    state machine enters the ErrorOnWaitingForOverCurrentClear State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortIgnoreError;

} // HUBPSM20_ErrorOnWaitingForOverCurrentClear


PSM_EVENT
HUBPSM20_ErrorOnWaitingForResumeComplete(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnWaitingForResumeComplete is called when the
    state machine enters the ErrorOnWaitingForResumeComplete State

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
        syncEvent = HUBHTX_GetGenericErrorResponseEventForInvalidPortStatus(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ErrorOnWaitingForResumeComplete


PSM_EVENT
HUBPSM20_ErrorOnWaitingForResumeRecoveryTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnWaitingForResumeRecoveryTimer is called when the
    state machine enters the ErrorOnWaitingForResumeRecoveryTimer State

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
        syncEvent = HUBHTX_GetGenericErrorResponseEventForInvalidPortStatus(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ErrorOnWaitingForResumeRecoveryTimer


PSM_EVENT
HUBPSM20_ErrorOnWaitingForResumeTimerToFlush(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ErrorOnWaitingForResumeTimerToFlush is called when the
    state machine enters the ErrorOnWaitingForResumeTimerToFlush State

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
        syncEvent = HUBHTX_GetGenericErrorResponseEventForInvalidPortStatus(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ErrorOnWaitingForResumeTimerToFlush


PSM_EVENT
HUBPSM20_FlushingStaleDeviceEvents(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_FlushingStaleDeviceEvents is called when the
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

} // HUBPSM20_FlushingStaleDeviceEvents


PSM_EVENT
HUBPSM20_FlushingUserResetForOverCurrentOnHubStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_FlushingUserResetForOverCurrentOnHubStopSuspend is called when the
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

} // HUBPSM20_FlushingUserResetForOverCurrentOnHubStopSuspend


PSM_EVENT
HUBPSM20_FlushingUserResetForOverCurrentOnOverCurrentClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_FlushingUserResetForOverCurrentOnOverCurrentClear is called when the
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

} // HUBPSM20_FlushingUserResetForOverCurrentOnOverCurrentClear


PSM_EVENT
HUBPSM20_InitiatingResetPort(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_InitiatingResetPort is called when the
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

} // HUBPSM20_InitiatingResetPort


PSM_EVENT
HUBPSM20_InitiatingResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_InitiatingResume is called when the
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
        HUBHTX_ResumePortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_InitiatingResume


PSM_EVENT
HUBPSM20_IsItBootDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IsItBootDevice is called when the
    state machine enters the IsItBootDevice State

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

} // HUBPSM20_IsItBootDevice


PSM_EVENT
HUBPSM20_IsOldDevicePresent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IsOldDevicePresent is called when the
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

} // HUBPSM20_IsOldDevicePresent


PSM_EVENT
HUBPSM20_IssuingAttachDeviceToPort(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingAttachDeviceToPort is called when the
    state machine enters the IssuingAttachDeviceToPort State

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
        HUBMISC_SetPortFlagFor20TypeDevice(portContext);

        syncEvent = HUBMISC_AttachDevice(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_IssuingAttachDeviceToPort


PSM_EVENT
HUBPSM20_IssuingDetachDeviceOnCycleFromPoweredOffDisabledOrSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingDetachDeviceOnCycleFromPoweredOffDisabledOrSuspended is called when the
    state machine enters the IssuingDetachDeviceOnCycleFromPoweredOffDisabledOrSuspended State

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

} // HUBPSM20_IssuingDetachDeviceOnCycleFromPoweredOffDisabledOrSuspended


PSM_EVENT
HUBPSM20_IssuingDisabledToDSMFromDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingDisabledToDSMFromDisabled is called when the
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

        HUBMISC_UpdateCurrentPortStatusAsDisabled(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_IssuingDisabledToDSMFromDisabled


PSM_EVENT
HUBPSM20_IssuingHubReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingHubReset is called when the
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

} // HUBPSM20_IssuingHubReset


PSM_EVENT
HUBPSM20_IssuingHubResetFromDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingHubResetFromDisconnected is called when the
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

} // HUBPSM20_IssuingHubResetFromDisconnected


PSM_EVENT
HUBPSM20_IssuingHubResetOnDisableFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingHubResetOnDisableFailure is called when the
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

} // HUBPSM20_IssuingHubResetOnDisableFailure


PSM_EVENT
HUBPSM20_IssuingHubResetWhenWaitingForDSMResumeResponse(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingHubResetWhenWaitingForDSMResumeResponse is called when the
    state machine enters the IssuingHubResetWhenWaitingForDSMResumeResponse State

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

} // HUBPSM20_IssuingHubResetWhenWaitingForDSMResumeResponse


PSM_EVENT
HUBPSM20_IssuingHubResetWhileHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingHubResetWhileHubSuspend is called when the
    state machine enters the IssuingHubResetWhileHubSuspend State

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

} // HUBPSM20_IssuingHubResetWhileHubSuspend


PSM_EVENT
HUBPSM20_IssuingHubResetWithPendingTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingHubResetWithPendingTimer is called when the
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

} // HUBPSM20_IssuingHubResetWithPendingTimer


PSM_EVENT
HUBPSM20_IssuingHubResetWithTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingHubResetWithTimer is called when the
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

} // HUBPSM20_IssuingHubResetWithTimer


PSM_EVENT
HUBPSM20_IssuingHubResetWithTimerOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingHubResetWithTimerOnHubSuspend is called when the
    state machine enters the IssuingHubResetWithTimerOnHubSuspend State

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

} // HUBPSM20_IssuingHubResetWithTimerOnHubSuspend


PSM_EVENT
HUBPSM20_IssuingHubResetWithTimerWhileHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingHubResetWithTimerWhileHubSuspend is called when the
    state machine enters the IssuingHubResetWithTimerWhileHubSuspend State

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

} // HUBPSM20_IssuingHubResetWithTimerWhileHubSuspend


PSM_EVENT
HUBPSM20_IssuingPortDisabledToDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingPortDisabledToDevice is called when the
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

        HUBMISC_UpdateCurrentPortStatusAsDisabled(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_IssuingPortDisabledToDevice


PSM_EVENT
HUBPSM20_IssuingPortDisableFailedToDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingPortDisableFailedToDevice is called when the
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

} // HUBPSM20_IssuingPortDisableFailedToDevice


PSM_EVENT
HUBPSM20_IssuingReAttachDeviceToBootDevice(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingReAttachDeviceToBootDevice is called when the
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

} // HUBPSM20_IssuingReAttachDeviceToBootDevice


PSM_EVENT
HUBPSM20_IssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled is called when the
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

} // HUBPSM20_IssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled


PSM_EVENT
HUBPSM20_IssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled is called when the
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

} // HUBPSM20_IssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled


PSM_EVENT
HUBPSM20_IssuingResumedToDeviceSM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingResumedToDeviceSM is called when the
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

        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_IssuingResumedToDeviceSM


PSM_EVENT
HUBPSM20_IssuingSuspendedToDeviceSM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingSuspendedToDeviceSM is called when the
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

        HUBMISC_UpdateCurrentPortStatusAsSuspended(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_IssuingSuspendedToDeviceSM


PSM_EVENT
HUBPSM20_NotifyingUserAboutPersistentOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_NotifyingUserAboutPersistentOverCurrent is called when the
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

} // HUBPSM20_NotifyingUserAboutPersistentOverCurrent


PSM_EVENT
HUBPSM20_PoweredOffDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_PoweredOffDisabled is called when the
    state machine enters the PoweredOffDisabled State

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

} // HUBPSM20_PoweredOffDisabled


PSM_EVENT
HUBPSM20_PoweredOffDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_PoweredOffDisconnected is called when the
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

} // HUBPSM20_PoweredOffDisconnected


PSM_EVENT
HUBPSM20_PoweredOffSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_PoweredOffSuspended is called when the
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

} // HUBPSM20_PoweredOffSuspended


PSM_EVENT
HUBPSM20_QueryingPortStatusOnOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueryingPortStatusOnOverCurrent is called when the
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

} // HUBPSM20_QueryingPortStatusOnOverCurrent


PSM_EVENT
HUBPSM20_QueueingPortFailureToDSMOnSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingPortFailureToDSMOnSuspend is called when the
    state machine enters the QueueingPortFailureToDSMOnSuspend State

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

} // HUBPSM20_QueueingPortFailureToDSMOnSuspend


PSM_EVENT
HUBPSM20_QueueingPortResumeFailedDueToPendingSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingPortResumeFailedDueToPendingSuspend is called when the
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

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_QueueingPortResumeFailedDueToPendingSuspend


PSM_EVENT
HUBPSM20_QueueingPortResumeTimedOut(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingPortResumeTimedOut is called when the
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
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortResumeTimedOut);

        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_QueueingPortResumeTimedOut


PSM_EVENT
HUBPSM20_QueueingStateDisabledOnHubResumeFromDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingStateDisabledOnHubResumeFromDisabled is called when the
    state machine enters the QueueingStateDisabledOnHubResumeFromDisabled State

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

} // HUBPSM20_QueueingStateDisabledOnHubResumeFromDisabled


PSM_EVENT
HUBPSM20_QueueingStateDisabledToDeviceSM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingStateDisabledToDeviceSM is called when the
    state machine enters the QueueingStateDisabledToDeviceSM State

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

} // HUBPSM20_QueueingStateDisabledToDeviceSM


PSM_EVENT
HUBPSM20_QueueingStateDisabledToDeviceSMOnResetHub(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingStateDisabledToDeviceSMOnResetHub is called when the
    state machine enters the QueueingStateDisabledToDeviceSMOnResetHub State

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

} // HUBPSM20_QueueingStateDisabledToDeviceSMOnResetHub


PSM_EVENT
HUBPSM20_QueueingStateEnabledToDeviceSM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingStateEnabledToDeviceSM is called when the
    state machine enters the QueueingStateEnabledToDeviceSM State

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

} // HUBPSM20_QueueingStateEnabledToDeviceSM


PSM_EVENT
HUBPSM20_QueueingStateSuspendedToDeviceSM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingStateSuspendedToDeviceSM is called when the
    state machine enters the QueueingStateSuspendedToDeviceSM State

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

} // HUBPSM20_QueueingStateSuspendedToDeviceSM


PSM_EVENT
HUBPSM20_QueueingStateSuspendedToDeviceSMOnIgnoreError(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingStateSuspendedToDeviceSMOnIgnoreError is called when the
    state machine enters the QueueingStateSuspendedToDeviceSMOnIgnoreError State

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

} // HUBPSM20_QueueingStateSuspendedToDeviceSMOnIgnoreError


PSM_EVENT
HUBPSM20_ReleasingInterruptReferenceAndIssuingPortResetTimedOutToDSM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReleasingInterruptReferenceAndIssuingPortResetTimedOutToDSM is called when the
    state machine enters the ReleasingInterruptReferenceAndIssuingPortResetTimedOutToDSM State

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

        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ReleasingInterruptReferenceAndIssuingPortResetTimedOutToDSM


PSM_EVENT
HUBPSM20_ReleasingInterruptReferenceAndStoppingTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReleasingInterruptReferenceAndStoppingTimer is called when the
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

} // HUBPSM20_ReleasingInterruptReferenceAndStoppingTimer


PSM_EVENT
HUBPSM20_ReleasingInterruptReferenceOnResetTransferFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReleasingInterruptReferenceOnResetTransferFailure is called when the
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
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortFailed);

        HUBMUX_ReleaseInterruptReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ReleasingInterruptReferenceOnResetTransferFailure


PSM_EVENT
HUBPSM20_ReleasingInterruptReferenceOnSuccessfulReset(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReleasingInterruptReferenceOnSuccessfulReset is called when the
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

} // HUBPSM20_ReleasingInterruptReferenceOnSuccessfulReset


PSM_EVENT
HUBPSM20_ReleasingPowerReferenceonPoweredOffDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReleasingPowerReferenceonPoweredOffDisabled is called when the
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

} // HUBPSM20_ReleasingPowerReferenceonPoweredOffDisabled


PSM_EVENT
HUBPSM20_ReleasingPowerReferenceonPoweredOffDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReleasingPowerReferenceonPoweredOffDisconnected is called when the
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

} // HUBPSM20_ReleasingPowerReferenceonPoweredOffDisconnected


PSM_EVENT
HUBPSM20_ReleasingResetReferenceOnDeviceRemoval(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReleasingResetReferenceOnDeviceRemoval is called when the
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
        HUBMUX_ReleaseResetReference(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ReleasingResetReferenceOnDeviceRemoval


PSM_EVENT
HUBPSM20_ResettingEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ResettingEnabled is called when the
    state machine enters the ResettingEnabled State

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

} // HUBPSM20_ResettingEnabled


PSM_EVENT
HUBPSM20_SettingPortPowerOnOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_SettingPortPowerOnOverCurrent is called when the
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

} // HUBPSM20_SettingPortPowerOnOverCurrent


PSM_EVENT
HUBPSM20_StartingAndAcquiringReferenceOnHubResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StartingAndAcquiringReferenceOnHubResume is called when the
    state machine enters the StartingAndAcquiringReferenceOnHubResume State

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

} // HUBPSM20_StartingAndAcquiringReferenceOnHubResume


PSM_EVENT
HUBPSM20_StartingAndAcquiringReferenceOnHubResumeFromDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StartingAndAcquiringReferenceOnHubResumeFromDisabled is called when the
    state machine enters the StartingAndAcquiringReferenceOnHubResumeFromDisabled State

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

} // HUBPSM20_StartingAndAcquiringReferenceOnHubResumeFromDisabled


PSM_EVENT
HUBPSM20_StartingAndAcquiringReferenceOnHubStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StartingAndAcquiringReferenceOnHubStart is called when the
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

} // HUBPSM20_StartingAndAcquiringReferenceOnHubStart


PSM_EVENT
HUBPSM20_StartingAndAcquiringReferenceOnHubStartFromDisabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StartingAndAcquiringReferenceOnHubStartFromDisabled is called when the
    state machine enters the StartingAndAcquiringReferenceOnHubStartFromDisabled State

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

} // HUBPSM20_StartingAndAcquiringReferenceOnHubStartFromDisabled


PSM_EVENT
HUBPSM20_StartingAndAcquiringReferenceOnWarmResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StartingAndAcquiringReferenceOnWarmResume is called when the
    state machine enters the StartingAndAcquiringReferenceOnWarmResume State

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

} // HUBPSM20_StartingAndAcquiringReferenceOnWarmResume


PSM_EVENT
HUBPSM20_StartingOverCurrentTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StartingOverCurrentTimer is called when the
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

} // HUBPSM20_StartingOverCurrentTimer


PSM_EVENT
HUBPSM20_StartingResetTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StartingResetTimer is called when the
    state machine enters the StartingResetTimer State

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
        HUBMISC_StartResetCompletionTimer(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_StartingResetTimer


PSM_EVENT
HUBPSM20_StartingResumeRecoveryTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StartingResumeRecoveryTimer is called when the
    state machine enters the StartingResumeRecoveryTimer State

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
        HUBMISC_StartResumeRecoveryTimer(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_StartingResumeRecoveryTimer


PSM_EVENT
HUBPSM20_StartingResumeTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StartingResumeTimer is called when the
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

} // HUBPSM20_StartingResumeTimer


PSM_EVENT
HUBPSM20_StoppingResumeTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingResumeTimer is called when the
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

} // HUBPSM20_StoppingResumeTimer


PSM_EVENT
HUBPSM20_StoppingTimerAndQueueingHubResetInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingTimerAndQueueingHubResetInDisconnected is called when the
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

} // HUBPSM20_StoppingTimerAndQueueingHubResetInDisconnected


PSM_EVENT
HUBPSM20_StoppingTimerOnDisconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingTimerOnDisconnect is called when the
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

} // HUBPSM20_StoppingTimerOnDisconnect


PSM_EVENT
HUBPSM20_StoppingTimerOnHubStopSuspendInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingTimerOnHubStopSuspendInDisconnected is called when the
    state machine enters the StoppingTimerOnHubStopSuspendInDisconnected State

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

} // HUBPSM20_StoppingTimerOnHubStopSuspendInDisconnected


PSM_EVENT
HUBPSM20_StoppingTimerOnOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingTimerOnOverCurrent is called when the
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

} // HUBPSM20_StoppingTimerOnOverCurrent


PSM_EVENT
HUBPSM20_StoppingTimerOnOverCurrentClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingTimerOnOverCurrentClear is called when the
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

} // HUBPSM20_StoppingTimerOnOverCurrentClear


PSM_EVENT
HUBPSM20_StoppingTimerOnPortCycle(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingTimerOnPortCycle is called when the
    state machine enters the StoppingTimerOnPortCycle State

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

} // HUBPSM20_StoppingTimerOnPortCycle


PSM_EVENT
HUBPSM20_StoppingTimerOnResetComplete(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingTimerOnResetComplete is called when the
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

} // HUBPSM20_StoppingTimerOnResetComplete


PSM_EVENT
HUBPSM20_Suspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_Suspended is called when the
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

} // HUBPSM20_Suspended


PSM_EVENT
HUBPSM20_Suspending(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_Suspending is called when the
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
        HUBHTX_SuspendPortUsingControlTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_Suspending


PSM_EVENT
HUBPSM20_WaitingForDebounce(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDebounce is called when the
    state machine enters the WaitingForDebounce State

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

} // HUBPSM20_WaitingForDebounce


PSM_EVENT
HUBPSM20_WaitingForDisableCompleteOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDisableCompleteOnHubSuspend is called when the
    state machine enters the WaitingForDisableCompleteOnHubSuspend State

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

} // HUBPSM20_WaitingForDisableCompleteOnHubSuspend


PSM_EVENT
HUBPSM20_WaitingForDisableCompleteWithTimerOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDisableCompleteWithTimerOnHubSuspend is called when the
    state machine enters the WaitingForDisableCompleteWithTimerOnHubSuspend State

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

} // HUBPSM20_WaitingForDisableCompleteWithTimerOnHubSuspend


PSM_EVENT
HUBPSM20_WaitingForDSMResumeResponse(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDSMResumeResponse is called when the
    state machine enters the WaitingForDSMResumeResponse State

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

} // HUBPSM20_WaitingForDSMResumeResponse


PSM_EVENT
HUBPSM20_WaitingForDSMResumeResponseOnHubStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDSMResumeResponseOnHubStop is called when the
    state machine enters the WaitingForDSMResumeResponseOnHubStop State

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

} // HUBPSM20_WaitingForDSMResumeResponseOnHubStop


PSM_EVENT
HUBPSM20_WaitingForHubStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForHubStopSuspend is called when the
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

} // HUBPSM20_WaitingForHubStopSuspend


PSM_EVENT
HUBPSM20_WaitingForHubStopSuspendInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForHubStopSuspendInDisconnected is called when the
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

} // HUBPSM20_WaitingForHubStopSuspendInDisconnected


PSM_EVENT
HUBPSM20_WaitingForHubStopSuspendOnAttachFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForHubStopSuspendOnAttachFailure is called when the
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

} // HUBPSM20_WaitingForHubStopSuspendOnAttachFailure


PSM_EVENT
HUBPSM20_WaitingForHubStopSuspendOrTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForHubStopSuspendOrTimer is called when the
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

} // HUBPSM20_WaitingForHubStopSuspendOrTimer


PSM_EVENT
HUBPSM20_WaitingForOldDeviceToDetach(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForOldDeviceToDetach is called when the
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

} // HUBPSM20_WaitingForOldDeviceToDetach


PSM_EVENT
HUBPSM20_WaitingForOverCurrentClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForOverCurrentClear is called when the
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

} // HUBPSM20_WaitingForOverCurrentClear


PSM_EVENT
HUBPSM20_WaitingForResetComplete(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForResetComplete is called when the
    state machine enters the WaitingForResetComplete State

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

} // HUBPSM20_WaitingForResetComplete


PSM_EVENT
HUBPSM20_WaitingForResetTimerToFlush(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForResetTimerToFlush is called when the
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

} // HUBPSM20_WaitingForResetTimerToFlush


PSM_EVENT
HUBPSM20_WaitingForResumeComplete(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForResumeComplete is called when the
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

} // HUBPSM20_WaitingForResumeComplete


PSM_EVENT
HUBPSM20_WaitingForResumeRecoveryTimer(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForResumeRecoveryTimer is called when the
    state machine enters the WaitingForResumeRecoveryTimer State

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

} // HUBPSM20_WaitingForResumeRecoveryTimer


PSM_EVENT
HUBPSM20_WaitingForResumeTimerToFlush(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForResumeTimerToFlush is called when the
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

} // HUBPSM20_WaitingForResumeTimerToFlush


PSM_EVENT
HUBPSM20_WaitingForTimerOnResetInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForTimerOnResetInDisconnected is called when the
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

} // HUBPSM20_WaitingForTimerOnResetInDisconnected


PSM_EVENT
HUBPSM20_WaitingForTimerToFlushOnDisconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForTimerToFlushOnDisconnect is called when the
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

} // HUBPSM20_WaitingForTimerToFlushOnDisconnect


PSM_EVENT
HUBPSM20_WaitingForTimerToFlushOnHubStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForTimerToFlushOnHubStopSuspend is called when the
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

} // HUBPSM20_WaitingForTimerToFlushOnHubStopSuspend


PSM_EVENT
HUBPSM20_WaitingForTimerToFlushOnHubStopSuspendInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForTimerToFlushOnHubStopSuspendInDisconnected is called when the
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

} // HUBPSM20_WaitingForTimerToFlushOnHubStopSuspendInDisconnected


PSM_EVENT
HUBPSM20_WaitingForTimerToFlushOnOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForTimerToFlushOnOverCurrent is called when the
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

} // HUBPSM20_WaitingForTimerToFlushOnOverCurrent


PSM_EVENT
HUBPSM20_WaitingForTimerToFlushOnOverCurrentClear(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForTimerToFlushOnOverCurrentClear is called when the
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

} // HUBPSM20_WaitingForTimerToFlushOnOverCurrentClear


PSM_EVENT
HUBPSM20_WaitingForTimerToFlushOnPortCycle(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForTimerToFlushOnPortCycle is called when the
    state machine enters the WaitingForTimerToFlushOnPortCycle State

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

} // HUBPSM20_WaitingForTimerToFlushOnPortCycle


PSM_EVENT
HUBPSM20_WaitingForUserResetOnOverCurrent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForUserResetOnOverCurrent is called when the
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

} // HUBPSM20_WaitingForUserResetOnOverCurrent


PSM_EVENT
HUBPSM20_WaitingToBeDeleted(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingToBeDeleted is called when the
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

} // HUBPSM20_WaitingToBeDeleted


PSM_EVENT
HUBPSM20_AckingPortChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AckingPortChange is called when the
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

} // HUBPSM20_AckingPortChange


PSM_EVENT
HUBPSM20_CheckIfThereIsAPortChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckIfThereIsAPortChange is called when the
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
        syncEvent = HUBHTX_CheckAndSelectIfAny20PortChangeSet(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_CheckIfThereIsAPortChange


PSM_EVENT
HUBPSM20_CheckIfThereIsAPortChangeOnPortStatus(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckIfThereIsAPortChangeOnPortStatus is called when the
    state machine enters the CheckIfThereIsAPortChangeOnPortStatus State

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
        syncEvent = HUBHTX_CheckAndSelectIfAny20PortChangeSet(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_CheckIfThereIsAPortChangeOnPortStatus


PSM_EVENT
HUBPSM20_EnablingInterruptsAndGettingPortEvent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_EnablingInterruptsAndGettingPortEvent is called when the
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
        syncEvent = HUBHTX_Get20PortChangeEvent(portContext);

        HUBMUX_ReEnableInterruptTransfer(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_EnablingInterruptsAndGettingPortEvent


PSM_EVENT
HUBPSM20_GettingPortStatus(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_GettingPortStatus is called when the
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

} // HUBPSM20_GettingPortStatus


PSM_EVENT
HUBPSM20_InitializingCumulativePortChangeBits(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_InitializingCumulativePortChangeBits is called when the
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

} // HUBPSM20_InitializingCumulativePortChangeBits


PSM_EVENT
HUBPSM20_IssuingHubResetOnControlTransferFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingHubResetOnControlTransferFailure is called when the
    state machine enters the IssuingHubResetOnControlTransferFailure State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReEnableInterruptTransfer(portContext);

        HUBMUX_ResetHub(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_IssuingHubResetOnControlTransferFailure


PSM_EVENT
HUBPSM20_QueueingEnableInterruptTransferOnPortChange(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingEnableInterruptTransferOnPortChange is called when the
    state machine enters the QueueingEnableInterruptTransferOnPortChange State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReEnableInterruptTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_QueueingEnableInterruptTransferOnPortChange


PSM_EVENT
HUBPSM20_WaitingForPortChangeEvent(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForPortChangeEvent is called when the
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

} // HUBPSM20_WaitingForPortChangeEvent


PSM_EVENT
HUBPSM20_AcquiringPortReferenceOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AcquiringPortReferenceOnStart is called when the
    state machine enters the AcquiringPortReferenceOnStart State

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

} // HUBPSM20_AcquiringPortReferenceOnStart


PSM_EVENT
HUBPSM20_GettingPortLostChangesOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_GettingPortLostChangesOnStart is called when the
    state machine enters the GettingPortLostChangesOnStart State

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
        syncEvent = HUBHTX_GetPortLostChangeEvent(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_GettingPortLostChangesOnStart


PSM_EVENT
HUBPSM20_GettingPortStatusOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_GettingPortStatusOnStart is called when the
    state machine enters the GettingPortStatusOnStart State

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

} // HUBPSM20_GettingPortStatusOnStart


PSM_EVENT
HUBPSM20_IssuingHubResetOnControlTransferFailureOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingHubResetOnControlTransferFailureOnStart is called when the
    state machine enters the IssuingHubResetOnControlTransferFailureOnStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_AcquirePortPowerReference(portContext);

        HUBMUX_ResetHub(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_IssuingHubResetOnControlTransferFailureOnStart


PSM_EVENT
HUBPSM20_PoweringOnPortOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_PoweringOnPortOnStart is called when the
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

} // HUBPSM20_PoweringOnPortOnStart


PSM_EVENT
HUBPSM20_QueueingEnableInterruptTransferOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingEnableInterruptTransferOnStart is called when the
    state machine enters the QueueingEnableInterruptTransferOnStart State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReEnableInterruptTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_QueueingEnableInterruptTransferOnStart


PSM_EVENT
HUBPSM20_WaitingForPortPowerOnTimerOnStart(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForPortPowerOnTimerOnStart is called when the
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

} // HUBPSM20_WaitingForPortPowerOnTimerOnStart


PSM_EVENT
HUBPSM20_AckingConnectChangeOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AckingConnectChangeOnResume is called when the
    state machine enters the AckingConnectChangeOnResume State

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

} // HUBPSM20_AckingConnectChangeOnResume


PSM_EVENT
HUBPSM20_AckingPortChangeWhileWaitingForReconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AckingPortChangeWhileWaitingForReconnect is called when the
    state machine enters the AckingPortChangeWhileWaitingForReconnect State

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

} // HUBPSM20_AckingPortChangeWhileWaitingForReconnect


PSM_EVENT
HUBPSM20_AcquiringPortReferenceOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AcquiringPortReferenceOnResume is called when the
    state machine enters the AcquiringPortReferenceOnResume State

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

} // HUBPSM20_AcquiringPortReferenceOnResume


PSM_EVENT
HUBPSM20_AcquiringPortReferenceOnResumeInFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AcquiringPortReferenceOnResumeInFailure is called when the
    state machine enters the AcquiringPortReferenceOnResumeInFailure State

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

} // HUBPSM20_AcquiringPortReferenceOnResumeInFailure


PSM_EVENT
HUBPSM20_CheckIfThereIsAPortChangeOnPortStatusWhileWaitingForReconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckIfThereIsAPortChangeOnPortStatusWhileWaitingForReconnect is called when the
    state machine enters the CheckIfThereIsAPortChangeOnPortStatusWhileWaitingForReconnect State

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
        syncEvent = HUBHTX_CheckAndSelectIfAny20PortChangeSet(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_CheckIfThereIsAPortChangeOnPortStatusWhileWaitingForReconnect


PSM_EVENT
HUBPSM20_CheckingIfConnectBitChangeSetOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckingIfConnectBitChangeSetOnResume is called when the
    state machine enters the CheckingIfConnectBitChangeSetOnResume State

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
        syncEvent = HUBMISC_CheckAndSelectIfConnectChangeSetOnResume(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_CheckingIfConnectBitChangeSetOnResume


PSM_EVENT
HUBPSM20_CheckingIfConnectBitIsOneOnChangeWhileWaitingForReconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckingIfConnectBitIsOneOnChangeWhileWaitingForReconnect is called when the
    state machine enters the CheckingIfConnectBitIsOneOnChangeWhileWaitingForReconnect State

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

} // HUBPSM20_CheckingIfConnectBitIsOneOnChangeWhileWaitingForReconnect


PSM_EVENT
HUBPSM20_CheckingIfDeviceDisconnectedOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckingIfDeviceDisconnectedOnResume is called when the
    state machine enters the CheckingIfDeviceDisconnectedOnResume State

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

} // HUBPSM20_CheckingIfDeviceDisconnectedOnResume


PSM_EVENT
HUBPSM20_CheckingIfDeviceDisconnectedOnResumeAfterWaiting(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckingIfDeviceDisconnectedOnResumeAfterWaiting is called when the
    state machine enters the CheckingIfDeviceDisconnectedOnResumeAfterWaiting State

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

} // HUBPSM20_CheckingIfDeviceDisconnectedOnResumeAfterWaiting


PSM_EVENT
HUBPSM20_EnablingInterruptsWaitingForReconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_EnablingInterruptsWaitingForReconnect is called when the
    state machine enters the EnablingInterruptsWaitingForReconnect State

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

} // HUBPSM20_EnablingInterruptsWaitingForReconnect


PSM_EVENT
HUBPSM20_GettingPortLostChangesOnStartOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_GettingPortLostChangesOnStartOnResume is called when the
    state machine enters the GettingPortLostChangesOnStartOnResume State

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
        syncEvent = HUBHTX_GetPortLostChangeEvent(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_GettingPortLostChangesOnStartOnResume


PSM_EVENT
HUBPSM20_GettingPortStatusOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_GettingPortStatusOnResume is called when the
    state machine enters the GettingPortStatusOnResume State

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

} // HUBPSM20_GettingPortStatusOnResume


PSM_EVENT
HUBPSM20_GettingPortStatusOnResumeAfterWaiting(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_GettingPortStatusOnResumeAfterWaiting is called when the
    state machine enters the GettingPortStatusOnResumeAfterWaiting State

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

} // HUBPSM20_GettingPortStatusOnResumeAfterWaiting


PSM_EVENT
HUBPSM20_GettingPortStatusWhileWaitingForReconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_GettingPortStatusWhileWaitingForReconnect is called when the
    state machine enters the GettingPortStatusWhileWaitingForReconnect State

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

} // HUBPSM20_GettingPortStatusWhileWaitingForReconnect


PSM_EVENT
HUBPSM20_InitializingCumulativePortChangeBitsWhileWaitingForReconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_InitializingCumulativePortChangeBitsWhileWaitingForReconnect is called when the
    state machine enters the InitializingCumulativePortChangeBitsWhileWaitingForReconnect State

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

} // HUBPSM20_InitializingCumulativePortChangeBitsWhileWaitingForReconnect


PSM_EVENT
HUBPSM20_IssuingHubResetOnControlTransferFailureOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingHubResetOnControlTransferFailureOnResume is called when the
    state machine enters the IssuingHubResetOnControlTransferFailureOnResume State

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

} // HUBPSM20_IssuingHubResetOnControlTransferFailureOnResume


PSM_EVENT
HUBPSM20_PoweringOnPortOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_PoweringOnPortOnResume is called when the
    state machine enters the PoweringOnPortOnResume State

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

} // HUBPSM20_PoweringOnPortOnResume


PSM_EVENT
HUBPSM20_QueueingEnableInterruptTransferOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingEnableInterruptTransferOnResume is called when the
    state machine enters the QueueingEnableInterruptTransferOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReEnableInterruptTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_QueueingEnableInterruptTransferOnResume


PSM_EVENT
HUBPSM20_QueueingStateDisabledOnFailure(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingStateDisabledOnFailure is called when the
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
        portContext->DeviceContext->DsmContext.AddEvent(portContext->DeviceContext,
                                                        DsmEventPortStateDisabled);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_QueueingStateDisabledOnFailure


PSM_EVENT
HUBPSM20_ResettingHubOnFailureWhileWaitingForReconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ResettingHubOnFailureWhileWaitingForReconnect is called when the
    state machine enters the ResettingHubOnFailureWhileWaitingForReconnect State

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

        HUBMUX_ResetHub(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_ResettingHubOnFailureWhileWaitingForReconnect


PSM_EVENT
HUBPSM20_ReturningHubStopOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReturningHubStopOnResume is called when the
    state machine enters the ReturningHubStopOnResume State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventHubStop;

} // HUBPSM20_ReturningHubStopOnResume


PSM_EVENT
HUBPSM20_StartingTimerForAllowingReconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StartingTimerForAllowingReconnect is called when the
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

} // HUBPSM20_StartingTimerForAllowingReconnect


PSM_EVENT
HUBPSM20_StoppingReconnectTimerOnDeviceConnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingReconnectTimerOnDeviceConnect is called when the
    state machine enters the StoppingReconnectTimerOnDeviceConnect State

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

} // HUBPSM20_StoppingReconnectTimerOnDeviceConnect


PSM_EVENT
HUBPSM20_StoppingReconnectTimerOnHubStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingReconnectTimerOnHubStop is called when the
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

} // HUBPSM20_StoppingReconnectTimerOnHubStop


PSM_EVENT
HUBPSM20_WaitingForDeviceToReconnect(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDeviceToReconnect is called when the
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

} // HUBPSM20_WaitingForDeviceToReconnect


PSM_EVENT
HUBPSM20_WaitingForPortPowerOnTimerOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForPortPowerOnTimerOnResume is called when the
    state machine enters the WaitingForPortPowerOnTimerOnResume State

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

} // HUBPSM20_WaitingForPortPowerOnTimerOnResume


PSM_EVENT
HUBPSM20_WaitingForReconnectTimerToFlushOnHubStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForReconnectTimerToFlushOnHubStop is called when the
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

} // HUBPSM20_WaitingForReconnectTimerToFlushOnHubStop


PSM_EVENT
HUBPSM20_AckingPortChangeInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AckingPortChangeInSuspended is called when the
    state machine enters the AckingPortChangeInSuspended State

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

} // HUBPSM20_AckingPortChangeInSuspended


PSM_EVENT
HUBPSM20_AcquiringPowerReferenceOnHubS0IdleInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AcquiringPowerReferenceOnHubS0IdleInD3Cold is called when the
    state machine enters the AcquiringPowerReferenceOnHubS0IdleInD3Cold State

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

} // HUBPSM20_AcquiringPowerReferenceOnHubS0IdleInD3Cold


PSM_EVENT
HUBPSM20_CheckIfThereIsAPortChangeAfterInitialGetPortStatusOnSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckIfThereIsAPortChangeAfterInitialGetPortStatusOnSuspended is called when the
    state machine enters the CheckIfThereIsAPortChangeAfterInitialGetPortStatusOnSuspended State

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
        syncEvent = HUBHTX_CheckAndSelectIfAny20PortChangeSet(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_CheckIfThereIsAPortChangeAfterInitialGetPortStatusOnSuspended


PSM_EVENT
HUBPSM20_CheckIfThereIsAPortChangeOnSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckIfThereIsAPortChangeOnSuspended is called when the
    state machine enters the CheckIfThereIsAPortChangeOnSuspended State

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
        syncEvent = HUBHTX_CheckAndSelectIfAny20PortChangeSet(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_CheckIfThereIsAPortChangeOnSuspended


PSM_EVENT
HUBPSM20_CheckingIfDeviceIsConnectedAfterHubResumeInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckingIfDeviceIsConnectedAfterHubResumeInD3Cold is called when the
    state machine enters the CheckingIfDeviceIsConnectedAfterHubResumeInD3Cold State

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

} // HUBPSM20_CheckingIfDeviceIsConnectedAfterHubResumeInD3Cold


PSM_EVENT
HUBPSM20_CheckingIfDeviceIsConnectedOnConnectChangeForD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckingIfDeviceIsConnectedOnConnectChangeForD3Cold is called when the
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
        syncEvent = HUBMISC_IsConnectBitOne(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_CheckingIfDeviceIsConnectedOnConnectChangeForD3Cold


PSM_EVENT
HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangeInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangeInD3Cold is called when the
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
        syncEvent = HUBMISC_IsConnectBitOne(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangeInD3Cold


PSM_EVENT
HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangeInD3ColdOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangeInD3ColdOnResume is called when the
    state machine enters the CheckingIfDeviceIsConnectedOnPortChangeInD3ColdOnResume State

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

} // HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangeInD3ColdOnResume


PSM_EVENT
HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume is called when the
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
        syncEvent = HUBMISC_IsConnectBitOne(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_CheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume


PSM_EVENT
HUBPSM20_CheckingIsD3ColdIsEnabled(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckingIsD3ColdIsEnabled is called when the
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

} // HUBPSM20_CheckingIsD3ColdIsEnabled


PSM_EVENT
HUBPSM20_GettingPortChangeEventInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_GettingPortChangeEventInSuspended is called when the
    state machine enters the GettingPortChangeEventInSuspended State

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
        syncEvent = HUBHTX_Get20PortChangeEvent(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_GettingPortChangeEventInSuspended


PSM_EVENT
HUBPSM20_GettingPortChangeOnResumeFromD3ColdSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_GettingPortChangeOnResumeFromD3ColdSuspended is called when the
    state machine enters the GettingPortChangeOnResumeFromD3ColdSuspended State

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

} // HUBPSM20_GettingPortChangeOnResumeFromD3ColdSuspended


PSM_EVENT
HUBPSM20_GettingPortStatusInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_GettingPortStatusInSuspended is called when the
    state machine enters the GettingPortStatusInSuspended State

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

} // HUBPSM20_GettingPortStatusInSuspended


PSM_EVENT
HUBPSM20_InitializingCumulativePortChangeBitsInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_InitializingCumulativePortChangeBitsInSuspended is called when the
    state machine enters the InitializingCumulativePortChangeBitsInSuspended State

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

} // HUBPSM20_InitializingCumulativePortChangeBitsInSuspended


PSM_EVENT
HUBPSM20_IssuingHubResetOnControlTransferFailureInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_IssuingHubResetOnControlTransferFailureInSuspended is called when the
    state machine enters the IssuingHubResetOnControlTransferFailureInSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReEnableInterruptTransfer(portContext);

        HUBMUX_ResetHub(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_IssuingHubResetOnControlTransferFailureInSuspended


PSM_EVENT
HUBPSM20_PoweredOffOnHubSuspendFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_PoweredOffOnHubSuspendFromD3Cold is called when the
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

} // HUBPSM20_PoweredOffOnHubSuspendFromD3Cold


PSM_EVENT
HUBPSM20_PoweringOnResumeFromD3ColdSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_PoweringOnResumeFromD3ColdSuspended is called when the
    state machine enters the PoweringOnResumeFromD3ColdSuspended State

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

} // HUBPSM20_PoweringOnResumeFromD3ColdSuspended


PSM_EVENT
HUBPSM20_QueueingDisabledToDSMOnReconnectFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingDisabledToDSMOnReconnectFromD3Cold is called when the
    state machine enters the QueueingDisabledToDSMOnReconnectFromD3Cold State

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

} // HUBPSM20_QueueingDisabledToDSMOnReconnectFromD3Cold


PSM_EVENT
HUBPSM20_QueueingEnableInterruptTransferOnPortChangeForSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingEnableInterruptTransferOnPortChangeForSuspended is called when the
    state machine enters the QueueingEnableInterruptTransferOnPortChangeForSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMUX_ReEnableInterruptTransfer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_QueueingEnableInterruptTransferOnPortChangeForSuspended


PSM_EVENT
HUBPSM20_QueueingPortStateDisabledOnHubResumeInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingPortStateDisabledOnHubResumeInD3Cold is called when the
    state machine enters the QueueingPortStateDisabledOnHubResumeInD3Cold State

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

} // HUBPSM20_QueueingPortStateDisabledOnHubResumeInD3Cold


PSM_EVENT
HUBPSM20_QueueingResumeToDSMFromSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingResumeToDSMFromSuspended is called when the
    state machine enters the QueueingResumeToDSMFromSuspended State

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

        HUBMUX_ReEnableInterruptTransfer(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_QueueingResumeToDSMFromSuspended


PSM_EVENT
HUBPSM20_ReEnablingInterruptsOnConnectChangeInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReEnablingInterruptsOnConnectChangeInSuspended is called when the
    state machine enters the ReEnablingInterruptsOnConnectChangeInSuspended State

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

} // HUBPSM20_ReEnablingInterruptsOnConnectChangeInSuspended


PSM_EVENT
HUBPSM20_ReEnablingInterruptsOnErrorInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReEnablingInterruptsOnErrorInSuspended is called when the
    state machine enters the ReEnablingInterruptsOnErrorInSuspended State

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

} // HUBPSM20_ReEnablingInterruptsOnErrorInSuspended


PSM_EVENT
HUBPSM20_ReEnablingInterruptsOnOverCurrentClearedInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReEnablingInterruptsOnOverCurrentClearedInSuspended is called when the
    state machine enters the ReEnablingInterruptsOnOverCurrentClearedInSuspended State

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

} // HUBPSM20_ReEnablingInterruptsOnOverCurrentClearedInSuspended


PSM_EVENT
HUBPSM20_ReEnablingInterruptsOnOverCurrentInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReEnablingInterruptsOnOverCurrentInSuspended is called when the
    state machine enters the ReEnablingInterruptsOnOverCurrentInSuspended State

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

} // HUBPSM20_ReEnablingInterruptsOnOverCurrentInSuspended


PSM_EVENT
HUBPSM20_ReleasingPowerReferenceOnHubS0IdleInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReleasingPowerReferenceOnHubS0IdleInD3Cold is called when the
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

} // HUBPSM20_ReleasingPowerReferenceOnHubS0IdleInD3Cold


PSM_EVENT
HUBPSM20_ReturningHubStopFromSuspendedInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReturningHubStopFromSuspendedInD3Cold is called when the
    state machine enters the ReturningHubStopFromSuspendedInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventHubStop;

} // HUBPSM20_ReturningHubStopFromSuspendedInD3Cold


PSM_EVENT
HUBPSM20_ReturningHubSuspendFromSuspendedInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReturningHubSuspendFromSuspendedInD3Cold is called when the
    state machine enters the ReturningHubSuspendFromSuspendedInD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventHubSuspend;

} // HUBPSM20_ReturningHubSuspendFromSuspendedInD3Cold


PSM_EVENT
HUBPSM20_ReturningPortConnectChangeFromSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReturningPortConnectChangeFromSuspended is called when the
    state machine enters the ReturningPortConnectChangeFromSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortConnectChange;

} // HUBPSM20_ReturningPortConnectChangeFromSuspended


PSM_EVENT
HUBPSM20_ReturningPortErrorFromSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReturningPortErrorFromSuspended is called when the
    state machine enters the ReturningPortErrorFromSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortError;

} // HUBPSM20_ReturningPortErrorFromSuspended


PSM_EVENT
HUBPSM20_ReturningPortOverCurrentClearedFromSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReturningPortOverCurrentClearedFromSuspended is called when the
    state machine enters the ReturningPortOverCurrentClearedFromSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortOverCurrentCleared;

} // HUBPSM20_ReturningPortOverCurrentClearedFromSuspended


PSM_EVENT
HUBPSM20_ReturningPortOverCurrentFromSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReturningPortOverCurrentFromSuspended is called when the
    state machine enters the ReturningPortOverCurrentFromSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortOverCurrent;

} // HUBPSM20_ReturningPortOverCurrentFromSuspended


PSM_EVENT
HUBPSM20_ReturningPortResumedFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReturningPortResumedFromD3Cold is called when the
    state machine enters the ReturningPortResumedFromD3Cold State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortDisabled;

} // HUBPSM20_ReturningPortResumedFromD3Cold


PSM_EVENT
HUBPSM20_ReturningPortResumedFromSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_ReturningPortResumedFromSuspended is called when the
    state machine enters the ReturningPortResumedFromSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    UNREFERENCED_PARAMETER(SmContext);

    return PsmEventPortResumed;

} // HUBPSM20_ReturningPortResumedFromSuspended


PSM_EVENT
HUBPSM20_StartingDebounceTimerOnResumeFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StartingDebounceTimerOnResumeFromD3Cold is called when the
    state machine enters the StartingDebounceTimerOnResumeFromD3Cold State

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
        HUBMISC_StartDebounceTimer(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_StartingDebounceTimerOnResumeFromD3Cold


PSM_EVENT
HUBPSM20_StartingDebounceTimerOnResumeFromD3ColdOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StartingDebounceTimerOnResumeFromD3ColdOnResume is called when the
    state machine enters the StartingDebounceTimerOnResumeFromD3ColdOnResume State

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
        HUBMISC_StartDebounceTimer(portContext);

        syncEvent = PsmEventOperationSuccess;

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_StartingDebounceTimerOnResumeFromD3ColdOnResume


PSM_EVENT
HUBPSM20_StartingTimerForAllowingReconnectOnResumingFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StartingTimerForAllowingReconnectOnResumingFromD3Cold is called when the
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

} // HUBPSM20_StartingTimerForAllowingReconnectOnResumingFromD3Cold


PSM_EVENT
HUBPSM20_StoppingDebounceTimerInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingDebounceTimerInD3Cold is called when the
    state machine enters the StoppingDebounceTimerInD3Cold State

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

} // HUBPSM20_StoppingDebounceTimerInD3Cold


PSM_EVENT
HUBPSM20_StoppingDebounceTimerInD3ColdOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingDebounceTimerInD3ColdOnResume is called when the
    state machine enters the StoppingDebounceTimerInD3ColdOnResume State

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

} // HUBPSM20_StoppingDebounceTimerInD3ColdOnResume


PSM_EVENT
HUBPSM20_StoppingReconnectTimerOnDeviceReAttachAfterD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingReconnectTimerOnDeviceReAttachAfterD3Cold is called when the
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

} // HUBPSM20_StoppingReconnectTimerOnDeviceReAttachAfterD3Cold


PSM_EVENT
HUBPSM20_StoppingReconnectTimerOnHubStopInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingReconnectTimerOnHubStopInD3Cold is called when the
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

} // HUBPSM20_StoppingReconnectTimerOnHubStopInD3Cold


PSM_EVENT
HUBPSM20_StoppingReconnectTimerOnOvercurrentClearedInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingReconnectTimerOnOvercurrentClearedInD3Cold is called when the
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

} // HUBPSM20_StoppingReconnectTimerOnOvercurrentClearedInD3Cold


PSM_EVENT
HUBPSM20_StoppingReconnectTimerOnOvercurrentInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_StoppingReconnectTimerOnOvercurrentInD3Cold is called when the
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

} // HUBPSM20_StoppingReconnectTimerOnOvercurrentInD3Cold


PSM_EVENT
HUBPSM20_WaitingForDebounceTimerOnReconnectInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDebounceTimerOnReconnectInD3Cold is called when the
    state machine enters the WaitingForDebounceTimerOnReconnectInD3Cold State

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

} // HUBPSM20_WaitingForDebounceTimerOnReconnectInD3Cold


PSM_EVENT
HUBPSM20_WaitingForDebounceTimerOnReconnectInD3ColdOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDebounceTimerOnReconnectInD3ColdOnResume is called when the
    state machine enters the WaitingForDebounceTimerOnReconnectInD3ColdOnResume State

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

} // HUBPSM20_WaitingForDebounceTimerOnReconnectInD3ColdOnResume


PSM_EVENT
HUBPSM20_WaitingForDebounceTimerToFlushOnHubStop(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDebounceTimerToFlushOnHubStop is called when the
    state machine enters the WaitingForDebounceTimerToFlushOnHubStop State

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

} // HUBPSM20_WaitingForDebounceTimerToFlushOnHubStop


PSM_EVENT
HUBPSM20_WaitingForDebounceTimerToFlushOnHubStopOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDebounceTimerToFlushOnHubStopOnResume is called when the
    state machine enters the WaitingForDebounceTimerToFlushOnHubStopOnResume State

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

} // HUBPSM20_WaitingForDebounceTimerToFlushOnHubStopOnResume


PSM_EVENT
HUBPSM20_WaitingForDebounceTimerToFlushOnHubSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDebounceTimerToFlushOnHubSuspend is called when the
    state machine enters the WaitingForDebounceTimerToFlushOnHubSuspend State

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

} // HUBPSM20_WaitingForDebounceTimerToFlushOnHubSuspend


PSM_EVENT
HUBPSM20_WaitingForDebounceTimerToFlushOnPortChangeInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDebounceTimerToFlushOnPortChangeInD3Cold is called when the
    state machine enters the WaitingForDebounceTimerToFlushOnPortChangeInD3Cold State

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

} // HUBPSM20_WaitingForDebounceTimerToFlushOnPortChangeInD3Cold


PSM_EVENT
HUBPSM20_WaitingForDebounceTimerToFlushOnPortChangeInD3ColdOnResume(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDebounceTimerToFlushOnPortChangeInD3ColdOnResume is called when the
    state machine enters the WaitingForDebounceTimerToFlushOnPortChangeInD3ColdOnResume State

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

} // HUBPSM20_WaitingForDebounceTimerToFlushOnPortChangeInD3ColdOnResume


PSM_EVENT
HUBPSM20_WaitingForDeviceRequestResumeOnD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDeviceRequestResumeOnD3Cold is called when the
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

} // HUBPSM20_WaitingForDeviceRequestResumeOnD3Cold


PSM_EVENT
HUBPSM20_WaitingForDeviceToReconnectOnResumeFromD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDeviceToReconnectOnResumeFromD3Cold is called when the
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

} // HUBPSM20_WaitingForDeviceToReconnectOnResumeFromD3Cold


PSM_EVENT
HUBPSM20_WaitingForPortChangeEventInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForPortChangeEventInD3Cold is called when the
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

} // HUBPSM20_WaitingForPortChangeEventInD3Cold


PSM_EVENT
HUBPSM20_WaitingForPortChangeEventInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForPortChangeEventInSuspended is called when the
    state machine enters the WaitingForPortChangeEventInSuspended State

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

} // HUBPSM20_WaitingForPortChangeEventInSuspended


PSM_EVENT
HUBPSM20_WaitingForReconnectTimerToFlushOnHubStopInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForReconnectTimerToFlushOnHubStopInD3Cold is called when the
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

} // HUBPSM20_WaitingForReconnectTimerToFlushOnHubStopInD3Cold


PSM_EVENT
HUBPSM20_WaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold is called when the
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

} // HUBPSM20_WaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold


PSM_EVENT
HUBPSM20_WaitingForReconnectTimerToFlushOnOvercurrentInD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForReconnectTimerToFlushOnOvercurrentInD3Cold is called when the
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

} // HUBPSM20_WaitingForReconnectTimerToFlushOnOvercurrentInD3Cold


PSM_EVENT
HUBPSM20_WaitingForReconnectTimerToFlushOnReattachAfterD3Cold(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForReconnectTimerToFlushOnReattachAfterD3Cold is called when the
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

} // HUBPSM20_WaitingForReconnectTimerToFlushOnReattachAfterD3Cold


PSM_EVENT
HUBPSM20_WaitingForResumeRecoveryTimerOnResumeInSuspended(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForResumeRecoveryTimerOnResumeInSuspended is called when the
    state machine enters the WaitingForResumeRecoveryTimerOnResumeInSuspended State

Arguments:

    SmContext - State Machine Context


Return Value:

    PSM_EVENT

--*/
{

    PPORT_CONTEXT                   portContext;
    
    TRY {

        portContext = SmContext->ParentContext;
        HUBMISC_StartResumeRecoveryTimer(portContext);

    } FINALLY {

    }

    return PsmEventNull;

} // HUBPSM20_WaitingForResumeRecoveryTimerOnResumeInSuspended


PSM_EVENT
HUBPSM20_AckingPortChangeInInWaitingForStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AckingPortChangeInInWaitingForStopSuspend is called when the
    state machine enters the AckingPortChangeInInWaitingForStopSuspend State

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

} // HUBPSM20_AckingPortChangeInInWaitingForStopSuspend


PSM_EVENT
HUBPSM20_CheckIfThereIsAPortChangeInInWaitingForStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckIfThereIsAPortChangeInInWaitingForStopSuspend is called when the
    state machine enters the CheckIfThereIsAPortChangeInInWaitingForStopSuspend State

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
        syncEvent = HUBHTX_CheckAndSelectIfAny20PortChangeSet(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_CheckIfThereIsAPortChangeInInWaitingForStopSuspend


PSM_EVENT
HUBPSM20_GettingPortStatusInWaitingForStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_GettingPortStatusInWaitingForStopSuspend is called when the
    state machine enters the GettingPortStatusInWaitingForStopSuspend State

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

} // HUBPSM20_GettingPortStatusInWaitingForStopSuspend


PSM_EVENT
HUBPSM20_QueueingEnableInterruptTransferInWaitingForStopSuspend(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingEnableInterruptTransferInWaitingForStopSuspend is called when the
    state machine enters the QueueingEnableInterruptTransferInWaitingForStopSuspend State

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

} // HUBPSM20_QueueingEnableInterruptTransferInWaitingForStopSuspend


PSM_EVENT
HUBPSM20_QueueingPortEventFailureToDSM(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingPortEventFailureToDSM is called when the
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

} // HUBPSM20_QueueingPortEventFailureToDSM


PSM_EVENT
HUBPSM20_WaitingForDevicePortEvents(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDevicePortEvents is called when the
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

} // HUBPSM20_WaitingForDevicePortEvents


PSM_EVENT
HUBPSM20_AckingPortChangeInInWaitingForStopSuspendInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_AckingPortChangeInInWaitingForStopSuspendInDisconnected is called when the
    state machine enters the AckingPortChangeInInWaitingForStopSuspendInDisconnected State

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

} // HUBPSM20_AckingPortChangeInInWaitingForStopSuspendInDisconnected


PSM_EVENT
HUBPSM20_CheckIfThereIsAPortChangeInInWaitingForStopSuspendInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_CheckIfThereIsAPortChangeInInWaitingForStopSuspendInDisconnected is called when the
    state machine enters the CheckIfThereIsAPortChangeInInWaitingForStopSuspendInDisconnected State

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
        syncEvent = HUBHTX_CheckAndSelectIfAny20PortChangeSet(portContext);

    } FINALLY {

    }

    return syncEvent;

} // HUBPSM20_CheckIfThereIsAPortChangeInInWaitingForStopSuspendInDisconnected


PSM_EVENT
HUBPSM20_GettingPortStatusInWaitingForStopSuspendInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_GettingPortStatusInWaitingForStopSuspendInDisconnected is called when the
    state machine enters the GettingPortStatusInWaitingForStopSuspendInDisconnected State

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

} // HUBPSM20_GettingPortStatusInWaitingForStopSuspendInDisconnected


PSM_EVENT
HUBPSM20_QueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_QueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected is called when the
    state machine enters the QueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected State

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

} // HUBPSM20_QueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected


PSM_EVENT
HUBPSM20_WaitingForDevicePortEventsInDisconnected(
    PSM_CONTEXT               SmContext
    )
/*++
Routine Description:

    HUBPSM20_WaitingForDevicePortEventsInDisconnected is called when the
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

} // HUBPSM20_WaitingForDevicePortEventsInDisconnected





