//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*++
Module Name:

    runtime power.c

Abstract:

    the runtime power code

Environment:

    kernel mode only

Notes:

Revision History:

    08-17-2010 : created (gaurav.nolkha@intel.com)

--*/

#include "common.h"
#include "wdm.h"

#ifdef USBPORT_WPP
#include "rtpower.tmh"
#endif
BOOLEAN USBPORT_EnableIdleD3 = TRUE;

PO_FX_COMPONENT_IDLE_STATE_CALLBACK USBPORT_RuntimePMStateCallback;

VOID
USBPORT_RuntimePMStateCallback (
    PVOID Context,
    ULONG Component,
    ULONG State
    )
/*++

Routine Description:

    Implements the callback routine for FState transitions
    We get called from Runtime PM, when the state change happens (F0 <-> F1)
    because of calling PoRuntimeActive or PoRuntimeIdle

Arguments:

    Context: void pointer that was passed during registration
    Component: the index of the component for which FState changed, 
               for us its just one component
    State: new FState

Return Value:

    TRUE

--*/
{
    PDEVICE_OBJECT FdoDeviceObject;
    PDEVICE_EXTENSION devExt;

    FdoDeviceObject = (PDEVICE_OBJECT) Context;
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);


    USBPORT_KdPrint((1, "Entering %s PdoExt %p\n", __FUNCTION__, devExt));

    USBPORT_ASSERT(Component == EHCI_COMP_IDX);    
    USBPORT_ASSERT(State == 0);
    USBPORT_ASSERT(devExt->Fdo.RuntimePMContext.RuntimePMSuspended == FALSE);

    //
    // Even though we don't support F state other than F0, we should notify 
    // the power framework incase it decides to tell us that we are in F0
    // 
    
    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtFx', State, 0, 0);
    PoFxCompleteIdleState(devExt->Fdo.RuntimePMContext.Handle,Component);

}


PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK USBPORT_RuntimePMComponentCallback;
VOID
USBPORT_RuntimePMComponentActiveCallback (
    PVOID Context,
    ULONG Component
    )
/*++

Routine Description:

    Implements the callback routine for Component callback
    Gets called from Runtime PM, when the state of component 
    changes from idle to active
    

Arguments:

    Context: void pointer that was passed during registration
    Component: the index of the component for which FState changed, 
               for us its just one component
    
Return Value:

    VOID

--*/
{
    PDEVICE_OBJECT FdoDeviceObject;
    PDEVICE_EXTENSION devExt;
    

    FdoDeviceObject = (PDEVICE_OBJECT) Context;
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);
    
    USBPORTRTPM_EtwEvent(FdoDeviceObject,
                         RunTimePmEventActiveCallbackInvoked);
    
    USBPORT_KdPrint((1, "Entering %s PdoExt %p\n", __FUNCTION__, devExt));
   
    USBPORT_ASSERT(Component == EHCI_COMP_IDX);
    USBPORT_ASSERT(devExt->Fdo.RuntimePMContext.RuntimePMSuspended == FALSE);

    USBPORT_ASSERT(devExt->Fdo.RuntimePMContext.RuntimePMState != RTPM_Active);
    devExt->Fdo.RuntimePMContext.RuntimePMState = RTPM_Active;
    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtAc', 0, 0, 0);
    
}

PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK USBPORT_RuntimePMComponentIdleCallback;

VOID
USBPORT_RuntimePMComponentIdleCallback (
    PVOID Context,
    ULONG Component
    )

/*++

Routine Description:

    Implements the callback routine for Component callback
    Gets called from Runtime PM, when the state of component 
    changes from active to idle
    

Arguments:

    Context: void pointer that was passed during registration
    Component: the index of the component for which FState changed, 
               for us its just one component
    
Return Value:

    VOID

--*/
{

    PDEVICE_OBJECT FdoDeviceObject;
    PDEVICE_EXTENSION devExt;
    

    FdoDeviceObject = (PDEVICE_OBJECT) Context;
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);
    
    USBPORTRTPM_EtwEvent(FdoDeviceObject,
                         RunTimePmEventIdleCallbackInvoked);
    
    USBPORT_KdPrint((1, "Entering %s PdoExt %p\n", __FUNCTION__, devExt));
   
    USBPORT_ASSERT(Component == EHCI_COMP_IDX);
    USBPORT_ASSERT(devExt->Fdo.RuntimePMContext.RuntimePMSuspended == FALSE);

    USBPORT_ASSERT(devExt->Fdo.RuntimePMContext.RuntimePMState != RTPM_Idle);
    devExt->Fdo.RuntimePMContext.RuntimePMState = RTPM_Idle;

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtId', 0, 0, 0);
    PoFxCompleteIdleCondition(devExt->Fdo.RuntimePMContext.Handle,
                              Component);

}


PO_FX_DEVICE_POWER_REQUIRED_CALLBACK USBPORT_RuntimePMDevicePowerRequiredCallback;

VOID
USBPORT_RuntimePMDevicePowerRequiredCallback (
    PVOID Context
    )

/*++

Routine Description:

    This is stub for the device power callback.

Arguments:

    None referenced.

Return Value:

    None.

--*/

{

    PDEVICE_OBJECT FdoDeviceObject;
    PDEVICE_EXTENSION devExt;

    FdoDeviceObject = (PDEVICE_OBJECT) Context;
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);
    
    USBPORTRTPM_EtwEvent(FdoDeviceObject,
                         RunTimePmEventPowerRequiredCallbackInvoked);

    USBPORT_KdPrint((1, "Entering %s PdoExt %p\n", __FUNCTION__, devExt));
    USBPORT_ASSERT(devExt->Fdo.RuntimePMContext.RuntimePMSuspended == FALSE);
    NT_ASSERT(devExt->Fdo.RuntimePMContext.RuntimePMState != RTPM_Active);

    //
    // We need to wake up, if we are asleep
    // 
    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtPr', 0, 0, 0);
    USBPORTRTPM_AddEvent(&devExt->Fdo.RuntimePMContext.RtpmSmContext,
                         RtpmEventPowerRequired);
        
    return;
}


PO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK USBPORT_RuntimePMDevicePowerRequiredCallback;

VOID
USBPORT_RuntimePMDevicePowerNotRequiredCallback (
    PVOID Context
    )

/*++

Routine Description:

    This is stub for the device power callback.

Arguments:

    None referenced.

Return Value:

    None.

--*/

{

    PDEVICE_OBJECT FdoDeviceObject;
    PDEVICE_EXTENSION devExt;

    FdoDeviceObject = (PDEVICE_OBJECT) Context;
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);
    
    USBPORTRTPM_EtwEvent(FdoDeviceObject,
                         RunTimePmEventPowerNotRequiredCallbackInvoked);
    USBPORT_KdPrint((1, "Entering %s PdoExt %p\n", __FUNCTION__, devExt));
    USBPORT_ASSERT(devExt->Fdo.RuntimePMContext.RuntimePMSuspended == FALSE);
    NT_ASSERT(devExt->Fdo.RuntimePMContext.RuntimePMState != RTPM_Active);

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtPn', 0, 0, 0);
    //
    // We can go ahead and go to sleep now.
    // 
    USBPORTRTPM_AddEvent(&devExt->Fdo.RuntimePMContext.RtpmSmContext,
                         RtpmEventPowerNotRequired);

    PoFxCompleteDevicePowerNotRequired(devExt->Fdo.RuntimePMContext.Handle);        

    return;
}


NTSTATUS
USBPORT_RuntimePMContextInit(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Allocates the data structures needed for RuntimePM

Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    NTSTATUS

--*/
{

    PDEVICE_EXTENSION   devExt;
    NTSTATUS            status;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    
    USBPORT_KdPrint((1, "Entering %s PdoExt %p\n", __FUNCTION__, devExt));

    RtlZeroMemory(&devExt->Fdo.RuntimePMContext,
                  sizeof(devExt->Fdo.RuntimePMContext));

    KeInitializeEvent(&devExt->Fdo.RuntimePMContext.ComponentCallbackActiveEvent,
                      NotificationEvent,
                      USBD_EVENT_STATE_NOT_SIGNALED);

    status = USBPORTRTPM_InitializeRtpmSm(FdoDeviceObject);                                                 
  
    return status;
}

VOID
USBPORT_RuntimePMContextFree(
    PDEVICE_OBJECT  FdoDeviceObject
    )
/*++
Routine Description:

    Frees the data structures needed for RuntimePM

Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    VOID

*/
{

    PDEVICE_EXTENSION   devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    if (devExt->Fdo.RuntimePMContext.Handle != NULL) {
        PoFxUnregisterDevice(devExt->Fdo.RuntimePMContext.Handle);
    }
    USBPORTRTPM_FreeRtpmSm(FdoDeviceObject);

}


NTSTATUS
USBPORT_RuntimePMRegister(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Registers with the power framework.  If this driver is loaded for
    a device which does not support wake in S0, then this function will 
    also take an additional power reference to prevent reaching the idle
    state.

    Preconditions:
    + The device is in D0.
    + The device in the Running condition.
    + Each component is in F0.
    + Each component is in the Idle condition. This implies that the component 
    hardware is not required at this time and can immediately transition 
    into a non-F0 idle state.

Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    NTSTATUS

--*/
{

    NTSTATUS status;
    PDEVICE_EXTENSION devExt;
    PO_FX_DEVICE poFxDevice;
    PO_FX_COMPONENT_IDLE_STATE idleState;
    BOOLEAN enableIdle = FALSE;
    PD3COLD_SUPPORT_INTERFACE   d3ColdInterface;
    DEVICE_WAKE_DEPTH deepestWakeableDstate;
    
    RtlZeroMemory(&poFxDevice, sizeof(poFxDevice));
    RtlZeroMemory(&idleState, sizeof(idleState));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);    

    //
    // We can only register once
    //
    if (devExt->Fdo.RuntimePMContext.Handle != NULL) {
        return STATUS_SUCCESS;
    }

    //
    // First lets figure out if this controller supports wake from Dx while in S0
    // 

    devExt->Fdo.RuntimePMContext.S0WakeableDState = DeviceWakeDepthD0;

    d3ColdInterface = &devExt->Fdo.D3ColdInterface;
    status = USBPORT_QueryD3ColdInterface(FdoDeviceObject);

    if (NT_SUCCESS(status) && (d3ColdInterface->GetIdleWakeInfo != NULL)) {
        status = d3ColdInterface->GetIdleWakeInfo(d3ColdInterface->Context,
                                                  PowerSystemWorking,
                                                  &deepestWakeableDstate);
        if (NT_SUCCESS(status)) {
            devExt->Fdo.RuntimePMContext.S0WakeableDState = deepestWakeableDstate;
        }
    }

    //
    // Ignore failures to get wake capabilities.
    // 
    status = STATUS_SUCCESS;

    //
    // Only enable Dx idle under the following conditions
    //      There are no companion controllers
    //      ACPI explicitly indicated support
    // 
    if (devExt->Fdo.Flags.IsCompanionController) {

        devExt->Fdo.RuntimePMContext.SetupStatus = RunTimePmSetupStatusFailedDueToCompanionController;

    } else if (MapWakeDepthToDstate(devExt->Fdo.RuntimePMContext.S0WakeableDState) <= PowerDeviceD0) {
        
        devExt->Fdo.RuntimePMContext.SetupStatus = RunTimePmSetupStatusFailedDueToNoS0WakeSupport;

    } else {
        
        enableIdle = USBPORT_EnableIdleD3;

    }
    
    poFxDevice.Version = PO_FX_VERSION_V1;   
    
    //
    // Specify callbacks and context
    //
    poFxDevice.ComponentIdleStateCallback = 
                    USBPORT_RuntimePMStateCallback;
    poFxDevice.ComponentActiveConditionCallback = 
                    USBPORT_RuntimePMComponentActiveCallback;
    poFxDevice.ComponentIdleConditionCallback = 
                    USBPORT_RuntimePMComponentIdleCallback;
    poFxDevice.DevicePowerRequiredCallback = 
                    USBPORT_RuntimePMDevicePowerRequiredCallback;
    poFxDevice.DevicePowerNotRequiredCallback =
                    USBPORT_RuntimePMDevicePowerNotRequiredCallback;
    poFxDevice.DeviceContext = FdoDeviceObject;

    //
    // We register as a single component device
    //
    poFxDevice.ComponentCount = 1;

    //
    // We only support F0
    //
    poFxDevice.Components[0].IdleStateCount = 1;
    
    //
    // Transition latency should be 0 for F0
    //
    idleState.TransitionLatency = 0;

    //
    // Residency requirement should be 0 for F0
    //
    idleState.ResidencyRequirement = 0;

    //
    // We don't know the power draw of this specific implementation. The
    // value doesn't matter for F0.
    //
    idleState.NominalPower = PO_FX_UNKNOWN_POWER;

    //
    // Specify the state information for F0
    //
    poFxDevice.Components[0].IdleStates = &idleState;
    

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtRg', 0, 0, 0);
    status = PoFxRegisterDevice(devExt->Fdo.PhysicalDeviceObject,
                                &poFxDevice,
                                &devExt->Fdo.RuntimePMContext.Handle);
        

    if (NT_SUCCESS(status)) {

        //
        // Need to ensure that when PoFxStartDevicePowerManagement is called, 
        // the controller is already in the active state.  When calling PoFxActivateComponent
        // prior to the call to PoFxStartDevicePowerManagement, the component callback will 
        // not be invoked, so we will set our state manually here.  
        //        
        devExt->Fdo.RuntimePMContext.RuntimePMState = RTPM_Active;
        USBPORT_RequestRuntimePMTransitionToActive(FdoDeviceObject);

        PoFxStartDevicePowerManagement(devExt->Fdo.RuntimePMContext.Handle);
                

        //
        // If we are on a system that should NOT be powered down in S0, we need to take 
        // and extra power reference here
        //
        if (!enableIdle) {

            LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtEr', 0, 0, 0);
            //
            // We don't want impact the rest of our state, and we know that we are already active
            // due to the previous call, so we can call PoFx directly here.
            //
            PoFxActivateComponent(devExt->Fdo.RuntimePMContext.Handle, 0, 0);
            
        } else {
            devExt->Fdo.RuntimePMContext.SetupStatus = RunTimePmSetupStatusSuccessful;
        }
    } else {
        devExt->Fdo.RuntimePMContext.SetupStatus = RunTimePmSetupStatusFailedDueToFailedPoRegistration;
    }

    return status;
}

BOOLEAN
USBPORT_IsRuntimePMEnabled(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Checks if RunTime power management is enabled on this system.

Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    BOOELAN
--*/
{
    PDEVICE_EXTENSION devExt;
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    return (devExt->Fdo.RuntimePMContext.SetupStatus == RunTimePmSetupStatusSuccessful);
}

VOID
USBPORT_RuntimePMStateActiveAssert(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Checks if the RTPM state of the controller is Active. Breaks into the debugger if it is not.
    
Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    BOOELAN
--*/
{
    PDEVICE_EXTENSION devExt;
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    if (devExt->Fdo.RuntimePMContext.RuntimePMState != RTPM_Active) {
        DbgBreakPoint();
    }
}


VOID
USBPORT_RequestRuntimePMTransitionToActive(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Transitions the lone component to Active state

Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    VOID

--*/
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtA+', 0, 0, 0);
    PoFxActivateComponent(devExt->Fdo.RuntimePMContext.Handle, 
                          0, 
                          PO_FX_FLAG_BLOCKING);
    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtA-', 0, 0, 0);
    USBPORT_ASSERT(devExt->Fdo.RuntimePMContext.RuntimePMState == RTPM_Active);
        
}

VOID
USBPORT_RequestRuntimePMTransitionToIdle(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Transitions the lone component to Idle state

Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    NTSTATUS

--*/
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_KdPrint((1, "Entering %s PdoExt %p\n", __FUNCTION__, devExt));

    USBPORT_ASSERT(devExt->Fdo.RuntimePMContext.RuntimePMState != RTPM_Idle);
    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtI', 0, 0, 0);
    PoFxIdleComponent(devExt->Fdo.RuntimePMContext.Handle, 0, 0);

    // 
    // We are not waiting for this to complete. 
    // So the RuntimePMState may not be Idle yet.
    //

}

NTSTATUS
USBPORT_RequestRuntimePMExit(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    Temporary funciton that will likely be removed when unregistration is no longer needed

Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    NTSTATUS

--*/
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);    
    ASSERT_FDOEXT(devExt);

    PoFxUnregisterDevice(devExt->Fdo.RuntimePMContext.Handle);
    devExt->Fdo.RuntimePMContext.Handle = NULL;
    
    // Default Values: Just for completion
    devExt->Fdo.RuntimePMContext.RuntimePMState = RTPM_Uninitialized;

    return STATUS_SUCCESS;
}

        
VOID
USBPORTRTPM_AcknowledgePower(
    PDEVICE_OBJECT      FdoDeviceObject
    )
/*++

Routine Description:

    Notifies the power framework that the controller is in D0 and will not leave
    D0, thus completing the PowerRequired operation.

Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    VOID

--*/
{
        
    PDEVICE_EXTENSION devExt;


    GET_DEVICE_EXT(devExt, FdoDeviceObject);    
    ASSERT_FDOEXT(devExt);

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtAp', 0, 0, 0);
    PoFxReportDevicePoweredOn(devExt->Fdo.RuntimePMContext.Handle);


}



RTPM_EVENT
USBPORTRTPM_ArmForWakeFromS0(
    PDEVICE_OBJECT      FdoDeviceObject
    )
/*++

Routine Description:

    Synchronizes with any already pending WaitWake irps and
    Requests a new WaitWake irp for the controller

Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    RtpmEventOperationFailure or RtpmEventOperationSuccess
       
--*/
{
    NTSTATUS ntStatus;

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtWa', 0, 0, 0);
    ntStatus = USBPORT_ArmHcForWake(FdoDeviceObject,TRUE);

    if (!NT_SUCCESS(ntStatus)) {
        return RtpmEventOperationFailure;
    }
   
    return RtpmEventOperationSuccess;
}

VOID
USBPORTRTPM_CompleteHubWWIrp(
    PDEVICE_OBJECT      FdoDeviceObject
    )
/*++

Routine Description

    Causes the root hub's pending WaitWake irp (if there is one) to 
    complete.  Also invalidates the state of the virtual interrupt endpoint.

Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    VOID
            
--*/
{
    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtHW', 0, 0, 0);
    USBPORT_Ev_Rh_IntrEp_Invalidate(FdoDeviceObject);
}

       
VOID
USBPORTRTPM_DisarmForWakeFromS0(
    PDEVICE_OBJECT      FdoDeviceObject
    )
/*++

Routine Description

    Cancels any pending WaitWake irp.

Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    VOID

--*/
{
    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtWd', 0, 0, 0);
    USBPORT_DisarmHcForWake(FdoDeviceObject);
    
}

RTPM_EVENT
USBPORTRTPM_EnableInterrupts(
    PDEVICE_OBJECT      FdoDeviceObject
    )
/*++

Routine Description

    Re enables interrupts on the controller

Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    RtpmEventOperationSuccess
--*/
{

    PDEVICE_EXTENSION   devExt;
    KIRQL               irql;
	
    GET_DEVICE_EXT(devExt, FdoDeviceObject); 
    ASSERT_FDOEXT(devExt);

    USBPORT_ACQUIRE_DM_LOCK(devExt, irql);
    devExt->Fdo.DmTimerFlags.DecodesOff = 0;
    USBPORT_RELEASE_DM_LOCK(devExt, irql);
	
    //
    // Now that the controller is powered back up, the 
    // deadman timer can be restarted
    // 
    USBPORT_ResumeDM_Timer(FdoDeviceObject);

    //
    // If an interrupt is connected, then report the interrupt as active
    // 
    if (devExt->Fdo.Flags.IrqConnected) {
        IO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS	parameters;

        RtlZeroMemory(&parameters,
                      sizeof(IO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS));

        parameters.Version = CONNECT_FULLY_SPECIFIED;
        parameters.ConnectionContext.InterruptObject = devExt->Fdo.InterruptObject;
        IoReportInterruptActive(&parameters);
    }			

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtEI', 0, 0, 0);
	MP_EnableInterrupts(FdoDeviceObject);


	
    //MPf_FlushInterrupts(devExt);
    return RtpmEventOperationSuccess;
}

VOID
USBPORTRTPM_IndicateCriticalPowerFailure(
    PDEVICE_OBJECT      FdoDeviceObject
    )
/*++

Routine Description:

    Called when there is a critical failure detected by the 
    RuntimePM state machine.  This should cause pnp to send 
    remove irps.

Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    VOID
--*/
{

    //
    // Treat this as though it were a surprise removal
    // of the host controller to kick off the cleanup process.
    //
    USBPORT_InvalidateController(FdoDeviceObject,
                                 UsbMpControllerRemoved);
}

REQUEST_POWER_COMPLETE USBPORTRTPM_PoRequestCompletionDx;

VOID
USBPORTRTPM_PoRequestCompletionDx(
    PDEVICE_OBJECT FdoDeviceObject,
    UCHAR MinorFunction,
    POWER_STATE PowerState,
    PVOID Context,
    PIO_STATUS_BLOCK IoStatus
    )
/*++

Routine Description:

    Called when the Dx Device Power State Irp we requested is completed 
    while in S0.
    

Arguments:

    DeviceObject - Pointer to the device object for the class device.

    DevicePowerState - The Dx that we are in/tagetted.

    Context - Driver defined context.

    IoStatus - The status of the IRP.

Return Value:

    None.

--*/
{

    PDEVICE_EXTENSION   devExt;

    UNREFERENCED_PARAMETER(MinorFunction);
    UNREFERENCED_PARAMETER(Context);

    GET_DEVICE_EXT(devExt, FdoDeviceObject);    
    ASSERT_FDOEXT(devExt);
    
    if (PowerState.DeviceState == PowerDeviceD3) {
        USBPORTRTPM_EtwEvent(FdoDeviceObject,
                             RunTimePmEventEnteredD3);
    } else {
        USBPORTRTPM_EtwEvent(FdoDeviceObject,
                             RunTimePmEventEnteredD2);
    }

    devExt->Fdo.RuntimePMContext.CurrentDevicePowerState = PowerState.DeviceState;
    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtX-', IoStatus->Status, 0, 0);
    if (!NT_SUCCESS(IoStatus->Status)) {
        USBPORTRTPM_AddEvent(&devExt->Fdo.RuntimePMContext.RtpmSmContext,
                             RtpmEventDxCompleteFailure);
    } else {
        USBPORTRTPM_AddEvent(&devExt->Fdo.RuntimePMContext.RtpmSmContext,
                             RtpmEventDxCompleteSuccess);
    }

}

REQUEST_POWER_COMPLETE USBPORTRTPM_PoRequestCompletionD0;

VOID
USBPORTRTPM_PoRequestCompletionD0(
    PDEVICE_OBJECT FdoDeviceObject,
    UCHAR MinorFunction,
    POWER_STATE PowerState,
    PVOID Context,
    PIO_STATUS_BLOCK IoStatus
    )
/*++

Routine Description:

    Called when the D0 Device Power State Irp we requested is completed while
    in S0.
    
Arguments:

    DeviceObject - Pointer to the device object for the class device.

    DevicePowerState - The Dx that we are in/tagetted.

    Context - Driver defined context.

    IoStatus - The status of the IRP.

Return Value:

    None.

--*/
{

    PDEVICE_EXTENSION   devExt;

    UNREFERENCED_PARAMETER(MinorFunction);
    UNREFERENCED_PARAMETER(Context);

    GET_DEVICE_EXT(devExt, FdoDeviceObject);  
    ASSERT_FDOEXT(devExt);
    
    USBPORTRTPM_EtwEvent(FdoDeviceObject,
                         RunTimePmEventEnteredD0);

    devExt->Fdo.RuntimePMContext.CurrentDevicePowerState = PowerState.DeviceState;
    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rt0-', IoStatus->Status, 0, 0);
    if (!NT_SUCCESS(IoStatus->Status)) {
        USBPORTRTPM_AddEvent(&devExt->Fdo.RuntimePMContext.RtpmSmContext,
                             RtpmEventD0CompleteFailure);
    } else {
        USBPORTRTPM_AddEvent(&devExt->Fdo.RuntimePMContext.RtpmSmContext,
                             RtpmEventD0CompleteSuccess);
    }
}

RTPM_EVENT
USBPORTRTPM_RequestD0(
    PDEVICE_OBJECT      FdoDeviceObject
    )
/*++

Routine Description:

    Called by the Runtime PM state machine to request a D0 irp.
    
Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    RtpmEventOperationFailure or RtpmEventOperationSuccess

--*/
{

    PDEVICE_EXTENSION   devExt;
    NTSTATUS            ntStatus;
    POWER_STATE         powerState;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);    
    ASSERT_FDOEXT(devExt);

    powerState.DeviceState = PowerDeviceD0;

    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rt0+', 0, 0, 0);
    ntStatus = PoRequestPowerIrp(FdoDeviceObject,
                                 IRP_MN_SET_POWER,
                                 powerState,
                                 USBPORTRTPM_PoRequestCompletionD0,
                                 FdoDeviceObject,    // Context
                                 NULL);


    if (!NT_SUCCESS(ntStatus)) {
        return RtpmEventOperationFailure;
    }

    return RtpmEventOperationSuccess;
}

RTPM_EVENT
USBPORTRTPM_RequestDx(
    PDEVICE_OBJECT      FdoDeviceObject
    )
/*++

Routine Description:

    Called by the Runtime PM state machine to request a Dx irp.
    
Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    RtpmEventOperationFailure or RtpmEventOperationSuccess

--*/
{

    PDEVICE_EXTENSION   devExt;
    NTSTATUS            ntStatus;
    POWER_STATE         powerState;
    
    GET_DEVICE_EXT(devExt, FdoDeviceObject);    
    ASSERT_FDOEXT(devExt);

    powerState.DeviceState = MapWakeDepthToDstate(devExt->Fdo.RuntimePMContext.S0WakeableDState);
    LOGENTRY(FdoDeviceObject, LOG_POWER, 'rtX+', powerState.DeviceState, 0, 0);
    ntStatus = PoRequestPowerIrp(FdoDeviceObject,
                                 IRP_MN_SET_POWER,
                                 powerState,
                                 USBPORTRTPM_PoRequestCompletionDx,
                                 FdoDeviceObject,    // Context
                                 NULL);

    if (!NT_SUCCESS(ntStatus)) {
        return RtpmEventOperationFailure;
    }

    return RtpmEventOperationSuccess;
}

VOID
USBPORTRTPM_EnableTimerWork(
    PDEVICE_OBJECT      FdoDeviceObject
    )
/*++

Routine Description:

    Re enables the periodic timer DPC's access to the controller
    
Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    VOID

--*/
{
    PDEVICE_EXTENSION   devExt;
    KIRQL               irql;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);    
    ASSERT_FDOEXT(devExt);

    // tell the DM timer to resume polling the controller
    USBPORT_ACQUIRE_DM_LOCK(devExt, irql);
    devExt->Fdo.DmTimerFlags.SkipTimerWork = 0;
    USBPORT_RELEASE_DM_LOCK(devExt, irql);
}


VOID
USBPORTRTPM_DisableTimerWork(
    PDEVICE_OBJECT      FdoDeviceObject
    )
/*++

Routine Description:

    Disables the periodic timer DPCs access to the controller
    
Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    VOID

--*/
{

    PDEVICE_EXTENSION   devExt;
    KIRQL               irql;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);    
    ASSERT_FDOEXT(devExt);

    // tell the DM timer to stop polling the controller
    USBPORT_ACQUIRE_DM_LOCK(devExt, irql);
    devExt->Fdo.DmTimerFlags.SkipTimerWork = 1;
    USBPORT_RELEASE_DM_LOCK(devExt, irql);
}

VOID
USBPORTRTPM_EtwEvent(
    PDEVICE_OBJECT      FdoDeviceObject,
    RUNTIME_PM_EVENT    Event
    )
/*++

Routine Description:

    Disables the periodic timer DPCs access to the controller
    
Arguments:

    FdoDeviceObject: Host controller FDO

Return Value:

    VOID

--*/
{     

    USBPORT_EtwWriteHcData1(FdoDeviceObject,
                            &USBPORT_ETW_EVENT_RTPM_TRANSITION,
                            NULL,

                            &Event,
                            sizeof(Event));
}


