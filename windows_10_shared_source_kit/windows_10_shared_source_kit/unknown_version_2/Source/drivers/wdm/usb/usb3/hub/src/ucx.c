/*++
Copyright (c) Microsoft Corporation

Module Name:

    hubucx.c

Abstract:

    IOCTLs and bus interface calls to UCX

Author:

Environment:

    Kernel mode only.

--*/

#include "pch.h"
#include "ucx.tmh"


FORCEINLINE 
VOID
HUBUCX_UpdateConfigurationEndpointStates (
    __in
        PDEVICE_CONFIGURATION           Configuration,
    __in
        UCXENDPOINT_STATE               OldEndpointState,
    __in
        UCXENDPOINT_STATE               NewEndpointState
    )
/*++

Routine Description:

    Updates any endpoint in the configuration with a specified state

Returns:

    VOID

--*/
{
    PINTERFACE_HANDLE       interfaceHandle;
    ULONG                   i;

    TRY {

        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &Configuration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            //
            // Walk the endpoint handles for this interface and update their state
            //
            for (i=0; i<interfaceHandle->NumberOfEndpoints; i++) {

                if (interfaceHandle->PipeHandle[i].UcxEndpointState == OldEndpointState) {

                    interfaceHandle->PipeHandle[i].UcxEndpointState = NewEndpointState;
                
                }

            }
        }
    } FINALLY {

    }
}


EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBUCX_UCXIoctlComplete;

VOID
HUBUCX_UCXIoctlComplete(
    __in
        WDFREQUEST                      WdfRequest,
    __in
        WDFIOTARGET                     Target,
    __in
        PWDF_REQUEST_COMPLETION_PARAMS  CompletionParams,
    __in
        WDFCONTEXT                      Context
    )
/*++

Routine Description:

    Generic completion routine for DSM UCX IOCTLs.  Queues
    DsmEventUCXIoctlFailure or DsmEventUCXIoctlSuccess event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                    status;
    PDEVICE_CONTEXT             deviceContext;
    PENDPOINTS_CONFIGURE        endpointsConfigure;
    PUSBDEVICE_ENABLE           deviceEnable;
    PUSBDEVICE_UPDATE           deviceUpdate;
    DSM_EVENT                   dsmEvent;   
    PHUB_GLOBALS                hubGlobals;


    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(WdfRequest);

    TRY {

        status = CompletionParams->IoStatus.Status;

        deviceContext = (PDEVICE_CONTEXT) Context;


        if (!NT_SUCCESS(status)) {
            DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "%!UCX_IOCTL! failed with %!STATUS!",
                       deviceContext->UcxIoctl,
                       status);
            //
            // Address 0 ownership failure is not a hardware
            // error. Set Addres and reset endpoint could fail 
            // if the device is disconnected
            //
            if ((deviceContext->UcxIoctl != IOCTL_INTERNAL_USB_ADDRESS0_OWNERSHIP_ACQUIRE) &&
                (deviceContext->UcxIoctl != IOCTL_INTERNAL_USB_USBDEVICE_ADDRESS) &&
                (deviceContext->UcxIoctl != IOCTL_INTERNAL_USB_ENDPOINT_RESET)) {
            

                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "ERROR: HubHwVerifierControllerOperationFailure %!UCX_IOCTL! failed with %!STATUS!",
                           deviceContext->UcxIoctl,
                           status);

                USB_HW_VERIFIER_PORT_BREAK(deviceContext->PortContext, HubHwVerifierControllerOperationFailure);

                EventWriteUSBHUB3_ETW_EVENT_UCX_IOCTL_FAILURE(
                    &deviceContext->DsmContext.CurrentActivityId,
                    deviceContext->UsbDevice,
                    deviceContext->UcxIoctl,
                    status);

            }
        }

        switch (deviceContext->UcxIoctl) {
        case IOCTL_INTERNAL_USB_USBDEVICE_ENABLE:
            
            deviceEnable =
                (PUSBDEVICE_ENABLE) WdfMemoryGetBuffer(deviceContext->UcxIoctlRequestMemory,
                                                          NULL);
            
            if ((deviceEnable->FailureFlags.InsufficientHardwareResourcesForDefaultEndpoint == 1) ||
                (deviceEnable->FailureFlags.InsufficientHardwareResourcesForDevice == 1)) {
                
                deviceContext->LastOperationUsbdStatus = USBD_STATUS_NO_BANDWIDTH;
                status = STATUS_UNSUCCESSFUL;

                //
                // We will start the timer so that if we don't see a successful
                // enable device during that time, we will send the user
                // notification about lack of bandwidth
                //
                WdfTimerStart(deviceContext->OutOfBandwidthTimer,
                              WDF_REL_TIMEOUT_IN_MS(DEVICE_OUT_OF_BANDWIDTH_NOTIFICATION_TIMEOUT));
            
            } else if (NT_SUCCESS(status)) {
                //
                // A enable device is being completed successfully. That
                // indicates that the device was able to recover from
                // any previous failure for lack of resources. We will cancel
                // the timer. We don't care about the return value because it
                // is quite possible that there wasn;t any timer in progress.
                //
                WdfTimerStop(deviceContext->OutOfBandwidthTimer,
                             FALSE);
            } else if (status == STATUS_UNSUCCESSFUL) {
                //
                // COMPAT: Map to a WIN7 compatible status/urbstatus
                //
                status = STATUS_INSUFFICIENT_RESOURCES;
            }
            break;

        case IOCTL_INTERNAL_USB_ENDPOINTS_CONFIGURE:

            endpointsConfigure =
                (PENDPOINTS_CONFIGURE) WdfMemoryGetBuffer(deviceContext->UcxIoctlRequestMemory,
                                                          NULL);
            
            
            //
            // Update the endpoint states 
            //
            if (deviceContext->CurrentDeviceConfiguration != NULL) {
        
                //
                // Any endpoints pending disable are now disabled
                //
                HUBUCX_UpdateConfigurationEndpointStates(
                        deviceContext->CurrentDeviceConfiguration,
                        UcxEndpointStatePendingDisable,
                        UcxEndpointStateDisabled);
            }


            if (deviceContext->OldDeviceConfiguration != NULL) {

                //
                // Endpoints in the old configuration are now disabled
                //
                HUBUCX_UpdateConfigurationEndpointStates(
                        deviceContext->OldDeviceConfiguration,
                        UcxEndpointStatePendingDisable,
                        UcxEndpointStateDisabled);
            }
            
            if (deviceContext->OldInterface != NULL) {
                PINTERFACE_HANDLE       removedInterface;
                ULONG                   i;

                removedInterface = deviceContext->OldInterface;

                //
                // Walk the endpoint handles for the removed interface and mark them as disabled
                //
                for (i=0; i<removedInterface->NumberOfEndpoints; i++) {

                    NT_ASSERT((removedInterface->PipeHandle[i].UcxEndpointState == UcxEndpointStatePendingDisable) ||
                              (removedInterface->PipeHandle[i].UcxEndpointState == UcxEndpointStateDisabled));

                    removedInterface->PipeHandle[i].UcxEndpointState = UcxEndpointStateDisabled;
                }
            }

            if (endpointsConfigure->ExitLatencyDelta > 0) {
                
                if (endpointsConfigure->FailureFlags.MaxExitLatencyTooLarge == 1) {
                    
                    if (deviceContext->EffectiveExitLatency > endpointsConfigure->ExitLatencyDelta) {
                        
                        deviceContext->MaxExitLatencyBasedOnEld = deviceContext->EffectiveExitLatency -
                            endpointsConfigure->ExitLatencyDelta;
                        DbgTraceTo(deviceContext->PortContext->IfrLog,
                                   TL_INFO,
                                   Device,
                                   "New max exit latency calculated based on ELD %d is %d",
                                   endpointsConfigure->ExitLatencyDelta,
                                   deviceContext->MaxExitLatencyBasedOnEld);
                    } else {
                        DbgTraceTo(deviceContext->PortContext->IfrLog,
                                   TL_ERROR,
                                   Device,
                                   "ELD %d is greater than the current exit latency: %d",
                                   endpointsConfigure->ExitLatencyDelta,
                                   deviceContext->EffectiveExitLatency);
                        NT_ASSERTMSG("Exit latency Delta exceeds the current max exit latency",
                                     FALSE); 
                    }
                    deviceContext->MaxExitLatencyBasedOnEld = 0;
                
                } else if (NT_SUCCESS(status)) {
                    
                    deviceContext->MaxExitLatencyBasedOnEld = deviceContext->EffectiveExitLatency +
                        endpointsConfigure->ExitLatencyDelta;
                    DbgTraceTo(deviceContext->PortContext->IfrLog,
                               TL_INFO,
                               Device,
                               "New max exit latency calculated based on ELD %d is %d",
                               endpointsConfigure->ExitLatencyDelta,
                               deviceContext->MaxExitLatencyBasedOnEld);
                } else {
                    deviceContext->MaxExitLatencyBasedOnEld = 0;
                }

            }
            

            if ((endpointsConfigure->FailureFlags.InsufficientBandwidth == 1) ||
                (endpointsConfigure->FailureFlags.InsufficientHardwareResourcesForEndpoints == 1) ||
                (endpointsConfigure->FailureFlags.MaxExitLatencyTooLarge == 1)) {
                
                deviceContext->LastOperationUsbdStatus = USBD_STATUS_NO_BANDWIDTH;
                status = STATUS_UNSUCCESSFUL;
                
                if (deviceContext->EffectiveExitLatency > 0) {
                    //
                    // If we have a non zero max exit latency, it is possible that a legacy
                    // controller might fail the configure endpoint with resource error rather
                    // than the max exit latency error. We will retry after disabling u1/u2
                    // During the retry, we only need the endpoints that need to be enabled.
                    // Add the disabled endpoints to the unchanged lists
                    //
                    RtlCopyMemory(&deviceContext->EndpointsToRemainUnchanged[deviceContext->EndpointsToRemainUnchangedCount],
                                  deviceContext->EndpointsToBeDisabled,
                                  deviceContext->EndpointsToBeDisabledCount * sizeof(UCXENDPOINT));

                    deviceContext->EndpointsToRemainUnchangedCount += deviceContext->EndpointsToBeDisabledCount;
                    deviceContext->EndpointsToBeDisabledCount = 0;
                    dsmEvent = DsmEventUCXIoctlFailureDueToExitLatencyTooLarge;
                    deviceContext->LastOperationNtStatus = HUBPDO_GetUSBDErrorFromNTStatus(status);
                    
                    LEAVE;
                } 

                //
                // We will start the timer so that if we don't see a successful
                // configure request during that time, we will send the user
                // notification about lack of bandwidth
                //
                WdfTimerStart(deviceContext->OutOfBandwidthTimer,
                              WDF_REL_TIMEOUT_IN_MS(DEVICE_OUT_OF_BANDWIDTH_NOTIFICATION_TIMEOUT));
            }
            if (deviceContext->DeviceStateFlags.FailAlternateSetting == 1) {

                INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &deviceContext->DeviceStateFlags, FailAlternateSetting);

                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_INFO,
                           Device,
                           "Failing select interface/configuration due to Alternate Setting filter match");
                                   
                
                deviceContext->LastOperationUsbdStatus = USBD_STATUS_NO_BANDWIDTH;
                status = STATUS_UNSUCCESSFUL;

                //
                // We will start the timer so that if we don't see a successful
                // configure request during that time, we will send the user
                // notification about lack of bandwidth
                //
                WdfTimerStart(deviceContext->OutOfBandwidthTimer,
                              WDF_REL_TIMEOUT_IN_MS(DEVICE_OUT_OF_BANDWIDTH_NOTIFICATION_TIMEOUT));
            
            } else if ((deviceContext->EndpointsToBeEnabledCount != 0) &&
                       (NT_SUCCESS(status))) {
                //
                // A configure request is being completed successfully. That
                // indicates that the client driver is able to recover from
                // any previous failure for lack of bandwidth. We will cancel
                // the timer. We don't care about the return value because it
                // is quite possible that there wasn;t any timer in progress.
                //
                WdfTimerStop(deviceContext->OutOfBandwidthTimer,
                             FALSE);
            } else if (status == STATUS_UNSUCCESSFUL) {
                //
                // COMPAT: Map to a WIN7 compatible status/urbstatus
                //
                status = STATUS_INSUFFICIENT_RESOURCES;
            }
            
            //
            // Update the endpoint states 
            //
            if (deviceContext->CurrentDeviceConfiguration != NULL) {
        
                //
                // If IOCTL failed, any endpoints pending enable are now disabled
                // Otherwise they are now enabled
                //
                HUBUCX_UpdateConfigurationEndpointStates(
                        deviceContext->CurrentDeviceConfiguration,
                        UcxEndpointStatePendingEnable,
                        NT_SUCCESS(status) ? UcxEndpointStateEnabled : UcxEndpointStateDisabled);
            }


            
            deviceContext->EndpointsToBeDisabledCount = 0;
            deviceContext->EndpointsToBeEnabledCount = 0;
            deviceContext->EndpointsToRemainUnchangedCount = 0;
            
            break;

        case IOCTL_INTERNAL_USB_USBDEVICE_PURGEIO:
        case IOCTL_INTERNAL_USB_USBDEVICE_STARTIO:

            break;
        
        case IOCTL_INTERNAL_USB_USBDEVICE_ADDRESS:
            
            hubGlobals = GetHubGlobals(WdfGetDriver());

            if (NT_SUCCESS(status)) {
                PUSBDEVICE_ADDRESS      deviceAddress;

                deviceAddress =
                    (PUSBDEVICE_ADDRESS)WdfMemoryGetBuffer(deviceContext->UcxIoctlRequestMemory,
                                                           NULL);

                deviceContext->DeviceAddress = deviceAddress->Address;

                if (deviceContext->DeviceStateFlags.LastSetAddressFailed == 1) {
                    //
                    // If the device failed a set address before, then 
                    // clear that flag and make sure that we don't apply
                    // the SuperSpeed debounce hack due to this device
                    //
                    DbgTraceTo(deviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "Set Address succeded for a device that had failed earlier");
                    INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &deviceContext->DeviceStateFlags, LastSetAddressFailed);
                    InterlockedDecrement(&hubGlobals->AllowDebounceTimeForSuperSpeedDevices);
                }

            } else {
                deviceContext->EnumMsgId = USBENUM_SET_ADDRESS_FAILURE;

                EventWriteUSBHUB3_ETW_EVENT_SET_ADDRESS_FAILURE(
                    &deviceContext->DsmContext.CurrentActivityId,
                    deviceContext->UsbDevice, 
                    status);

                //
                // We have seen that some SuperSpeed devices need
                // some recovery time after the initial connect and 
                // if we don't give them that, then they fail set
                // address. Since we don't know the VID PID of the 
                // device, from now on, we will start giving this time
                // to all SuperSpeed devices so that if the user plugs
                // out and plugs in this device on any port, it will
                // succeed. 
                //
                
                if ((deviceContext->DsmContext.SubSmFlags & DsmSubSmFlagPort30) &&
                    (deviceContext->DeviceStateFlags.LastSetAddressFailed == 0)) {
                    DbgTraceTo(deviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "Set Address failed, we are going to start giving extra time to SuperSpeed devices on connect");
                    INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &deviceContext->DeviceStateFlags, LastSetAddressFailed);
                    InterlockedIncrement(&hubGlobals->AllowDebounceTimeForSuperSpeedDevices);
                }
                
            }
            break;
        
        case IOCTL_INTERNAL_USB_USBDEVICE_RESET:

            if (deviceContext->CurrentDeviceConfiguration != NULL) {
                
                HUBUCX_UpdateConfigurationEndpointStates(
                        deviceContext->CurrentDeviceConfiguration,
                        UcxEndpointStatePendingDisable,
                        UcxEndpointStateDisabled);

            }
            break;

        case IOCTL_INTERNAL_USB_USBDEVICE_UPDATE:
            if (NT_SUCCESS(status)) {
                deviceContext->EffectiveExitLatency = deviceContext->TargetEffectiveExitLatency;
            } else {
                deviceUpdate =
                    (PUSBDEVICE_UPDATE) WdfMemoryGetBuffer(deviceContext->UcxIoctlRequestMemory,
                                                           NULL);
                if (deviceUpdate->FailureFlags.MaxExitLatencyTooLarge == 1) {
                    dsmEvent = DsmEventUCXIoctlFailureDueToExitLatencyTooLarge;
                    LEAVE;
                }

                
            }
            break;

        }

        if (!NT_SUCCESS(status)) {
            
            deviceContext->LastOperationNtStatus = status;
            
            if (deviceContext->LastOperationUsbdStatus == 0) {
                deviceContext->LastOperationUsbdStatus = HUBPDO_GetUSBDErrorFromNTStatus(status);
            }

        }

        dsmEvent = NT_SUCCESS(status) ? DsmEventUCXIoctlSuccess : DsmEventUCXIoctlFailure;

    } FINALLY {

        
        HUBSM_AddDsmEvent(deviceContext,
                          dsmEvent);
    }
}

NTSTATUS
HUBUCX_SubmitUcxIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext,
    __in
        ULONG                  IoctlCode
    )
/*++

Routine Description:

    Routine for submitting DSM UCX IOCTLs.  Queues DsmEventUCXIoctlFailure
    if IOCTL fails to send.

Arguments:

    IoctlCode - UCX IOCTL to be sent.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    BOOLEAN                         requestSent;
    WDFIOTARGET                     ioTarget;

    TRY {

        ioTarget = DeviceContext->HubFdoContext->RootHubIoTarget;    
        
        HUBMISC_RequestReuse(DeviceContext->UcxRequest);
        
        DeviceContext->UcxIoctl = IoctlCode;
        
        status =
            WdfIoTargetFormatRequestForInternalIoctlOthers(ioTarget,
                                                           DeviceContext->UcxRequest,
                                                           IoctlCode,
                                                           DeviceContext->UcxIoctlRequestMemory,
                                                           NULL,
                                                           NULL,
                                                           NULL,
                                                           NULL,
                                                           NULL);

        if (!NT_SUCCESS(status)) {
            DeviceContext->LastOperationNtStatus = status;
            DeviceContext->LastOperationUsbdStatus = HUBPDO_GetUSBDErrorFromNTStatus(status);
            LEAVE;
        }

        WdfRequestSetCompletionRoutine(DeviceContext->UcxRequest,
                                       HUBUCX_UCXIoctlComplete,
                                       DeviceContext);


        requestSent = WdfRequestSend(DeviceContext->UcxRequest,
                                     ioTarget,
                                     NULL);

        if ( requestSent == FALSE ) {

            status = WdfRequestGetStatus (DeviceContext->UcxRequest);

            DeviceContext->LastOperationNtStatus = status;
            DeviceContext->LastOperationUsbdStatus = HUBPDO_GetUSBDErrorFromNTStatus(status);

            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            HUBSM_AddDsmEvent(DeviceContext, DsmEventUCXIoctlFailure);
        }
    }

    return status;
}


VOID
HUBUCX_CancelUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Cancels the current DSM UCX IOCTL request.

Returns:

    VOID

--*/
{
    BOOLEAN             cancelled;

    TRY {

        cancelled = WdfRequestCancelSentRequest(DeviceContext->UcxRequest);

        if (!cancelled) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to Cancel UCX Request");
        }

    } FINALLY {

    }
}


VOID
HUBUCX_AcquireAddress0OwnershipUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends IOCTL_INTERNAL_USB_ADDRESS0_OWNERSHIP_ACQUIRE to the parent (UCX) to
    acquire ownership of the address 0 communications on the bus.  If
    WdfRequestSend fails then DsmEventUCXIoctlFailure should be queued directly
    from this function.

    Completion routine of this function should queue a DsmEventUCXIoctlFailure
    or DsmEventUCXIoctlSuccess event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PADDRESS0_OWNERSHIP_ACQUIRE     address0OwnershipAcquire;

    TRY {

        address0OwnershipAcquire =
            (PADDRESS0_OWNERSHIP_ACQUIRE) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory,
                                                             NULL);

        RtlZeroMemory(address0OwnershipAcquire, sizeof(*address0OwnershipAcquire));

        address0OwnershipAcquire->Size = sizeof(*address0OwnershipAcquire);
        address0OwnershipAcquire->Hub =
            DeviceContext->PortContext->HubFdoContext->HubParentInterface.Hub;
        address0OwnershipAcquire->UsbDevice = DeviceContext->UsbDevice;

        status = HUBUCX_SubmitUcxIoctl(DeviceContext,
                                       IOCTL_INTERNAL_USB_ADDRESS0_OWNERSHIP_ACQUIRE);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to submit UCX IOCTL_INTERNAL_USB_ADDRESS0_OWNERSHIP_ACQUIRE %!STATUS!",
                       status);
        }

    } FINALLY {

    }
}


VOID
HUBUCX_NotifyDeviceResetUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:


Returns:


--*/
{
    NTSTATUS                status;
    PUSBDEVICE_RESET        deviceReset;
    PINTERFACE_HANDLE       interfaceHandle;
    PDEVICE_CONFIGURATION   configuration;
    UCXENDPOINT             *endpointsToDisable;
    ULONG                   i;
    ULONG                   endpointCount;

    TRY {

        //
        // Walk the list of interfaces for this configuration and
        // find all the currently enabled endpoints that need to be disabled.
        //
        endpointCount = 0;
        endpointsToDisable = NULL;

        status = STATUS_SUCCESS;

        configuration = DeviceContext->CurrentDeviceConfiguration;

        //
        // Notify reset can be called mulitple times. If it is the 
        // first time it is being called when the device was in configured
        // state, the existing endpoints will automatically be de-configured
        // We need to move them to the deleted list , in case the enumeration
        // fails and we end up in the clean-up path
        //

        if ((configuration != NULL) && 
            (DeviceContext->UcxEndpointArrayCount > 0)) {

            endpointsToDisable = DeviceContext->EndpointsToBeDisabled;

            FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                            &configuration->InterfaceList,
                            InterfaceLink,
                            interfaceHandle) {

                //
                // Walk the endpoint handles for this interface
                //
                for (i = 0; i < interfaceHandle->NumberOfEndpoints; i++) {
                    PPIPE_HANDLE    pipeHandle;

                    pipeHandle = &interfaceHandle->PipeHandle[i];

                    if (pipeHandle->UcxEndpointState == UcxEndpointStateEnabled) {

                        if (endpointsToDisable == NULL) {
                            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                                       TL_ERROR, 
                                       Device,
                                       "EndpointsToDisable array is NULL!");

                            status = STATUS_INVALID_PARAMETER;
                            LEAVE;
                        }
                        
                        pipeHandle->UcxEndpointState = UcxEndpointStatePendingDisable;

                        endpointsToDisable[endpointCount] = pipeHandle->UcxEndpoint;
                        endpointCount++;
                    }
                }
            }
        }

        deviceReset = (PUSBDEVICE_RESET) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory,
                                                            NULL);

        RtlZeroMemory (deviceReset, sizeof(*deviceReset));

        deviceReset->Size = sizeof(*deviceReset);
        deviceReset->Hub = DeviceContext->PortContext->HubFdoContext->HubParentInterface.Hub;
        deviceReset->UsbDevice = DeviceContext->UsbDevice;
        deviceReset->DefaultEndpoint = DeviceContext->DefaultEndpoint;
        deviceReset->EndpointsToDisable = endpointsToDisable;
        deviceReset->EndpointsToDisableCount = endpointCount;

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_USBDEVICE_RESET);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to submit UCX IOCTL_INTERNAL_USB_USBDEVICE_RESET %!STATUS!",
                       status);

            //
            // Error in submitting IOCTL_INTERNAL_USB_USBDEVICE_RESET.  Mark all endpoints pending disable as enabled
            //
            if (configuration != NULL) {

                HUBUCX_UpdateConfigurationEndpointStates(
                        configuration,
                        UcxEndpointStatePendingDisable,
                        UcxEndpointStateEnabled);
            }
        }
    }
}


VOID
HUBUCX_EnableDisableEndpointsUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Configures new endpoints and Deconfigures the old endpoints for a device
    using IOCTL_INTERNAL_USB_ENDPOINTS_CONFIGURE.  The list of endpoints to be
    configured and de-configured will be located at a particular place in
    device context. This list is created by one of the
    HUBDSM_PrepareEndpointAndInterfaceListsFor* helper functions.

    If WdfRequestSend fails then DsmEventUCXIoctlFailure should be queued
    directly from this function. Completion routine of this function should queue
    a DsmEventUCXIoctlFailure or DsmEventUCXIoctlSuccess event to DSM.

    Completion routine of this function should also store the NTSTATUS at a
    particular place in the device context.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PENDPOINTS_CONFIGURE            endpointsConfigure;
    PUSB_CONFIGURATION_DESCRIPTOR   configurationDescriptor;

    TRY {

        if (DeviceContext->EndpointsToBeDisabledCount == 0) {
            if (DeviceContext->EndpointsToBeEnabledCount == 0) {
            
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_INFO,
                           Device,
                           "No endpoints to configure/deconfigure");

                //
                // COMPAT: Selecting a zero bandwidth timer will stop the out-of-bandwidth
                // timer popup.
                //
                WdfTimerStop(DeviceContext->OutOfBandwidthTimer,
                             FALSE);

                HUBSM_AddDsmEvent(DeviceContext, DsmEventUCXIoctlSuccess );

                LEAVE;

            } else if (DeviceContext->DeviceStateFlags.FailAlternateSetting == 1) {

                //
                // Mark endpoints as disabled if they are not being enabled due 
                // to the alternate setting filter
                //
                HUBUCX_UpdateConfigurationEndpointStates(
                    DeviceContext->CurrentDeviceConfiguration,
                    UcxEndpointStatePendingEnable,
                    UcxEndpointStateDisabled);

                DeviceContext->LastOperationUsbdStatus = USBD_STATUS_NO_BANDWIDTH;
                DeviceContext->LastOperationNtStatus  = STATUS_UNSUCCESSFUL;

                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_INFO,
                           Device,
                           "No endpoints to configure/deconfigure due to alternate setting filter");

                HUBSM_AddDsmEvent(DeviceContext, DsmEventUCXIoctlFailure );

                LEAVE;
            }
        }

        NT_ASSERT(DeviceContext->UcxEndpointArrayCount != 0);

        endpointsConfigure =
            (PENDPOINTS_CONFIGURE) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory,
                                                      NULL);

        RtlZeroMemory(endpointsConfigure, sizeof(*endpointsConfigure));

        NT_ASSERT(DeviceContext->CurrentDeviceConfiguration != NULL);
        NT_ASSERT(DeviceContext->UsbDevice != NULL);

        endpointsConfigure->Size = sizeof(*endpointsConfigure);
        endpointsConfigure->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        endpointsConfigure->UsbDevice = DeviceContext->UsbDevice;

        //
        // Skip enabling endpoints if we are failing the SELECT_INTERFACE/SELECT_CONFIGURATION due to a
        // match in the alternate setting filter.
        //
        if (DeviceContext->DeviceStateFlags.FailAlternateSetting == 0) {
            endpointsConfigure->EndpointsToEnableCount = DeviceContext->EndpointsToBeEnabledCount;
            endpointsConfigure->EndpointsToEnable = DeviceContext->EndpointsToBeEnabled;
        } 

        endpointsConfigure->EndpointsToDisableCount = DeviceContext->EndpointsToBeDisabledCount;
        endpointsConfigure->EndpointsToDisable = DeviceContext->EndpointsToBeDisabled;

        endpointsConfigure->EndpointsEnabledAndUnchangedCount = DeviceContext->EndpointsToRemainUnchangedCount;
        endpointsConfigure->EndpointsEnabledAndUnchanged = DeviceContext->EndpointsToRemainUnchanged;

        configurationDescriptor = (PUSB_CONFIGURATION_DESCRIPTOR)&DeviceContext->CurrentDeviceConfiguration->ConfigurationDescriptor;

        if (configurationDescriptor != NULL) {

            endpointsConfigure->ConfigurationValue = configurationDescriptor->bConfigurationValue;

            if (DeviceContext->NextInterfaceToSet != NULL) {
                endpointsConfigure->InterfaceNumber = DeviceContext->NewInterface->InterfaceDescriptor->bInterfaceNumber;
                endpointsConfigure->AlternateSetting = DeviceContext->NewInterface->InterfaceDescriptor->bAlternateSetting;
            }
        }

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_ENDPOINTS_CONFIGURE);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to submit UCX IOCTL_INTERNAL_USB_ENDPOINTS_CONFIGURE %!STATUS!",
                       status);
        }

    } FINALLY {

    }
}


VOID
HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Disables the USB device and its default endpoint via UCX's
    IOCTL_INTERNAL_USB_USBDEVICE_DISABLE.

    Completion routine of this function should queue a DsmEventUCXIoctlSuccess
    event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSBDEVICE_DISABLE              deviceDisable;

    TRY {

        deviceDisable =
            (PUSBDEVICE_DISABLE) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory, NULL);

        RtlZeroMemory(deviceDisable, sizeof(*deviceDisable));

        deviceDisable->Size = sizeof(*deviceDisable);
        deviceDisable->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        deviceDisable->UsbDevice = DeviceContext->UsbDevice;
        deviceDisable->DefaultEndpoint = DeviceContext->DefaultEndpoint;

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_USBDEVICE_DISABLE);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to submit UCX IOCTL_INTERNAL_USB_USBDEVICE_DISABLE %!STATUS!",
                       status);
        }

    } FINALLY {

    }
}


VOID
HUBUCX_EnableDeviceAndDefaultEndpointUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Enables the USB device and its default endpoint via UCX's
    IOCTL_INTERNAL_USB_USBDEVICE_ENABLE.

    Completion routine of this function should queue a DsmEventUCXIoctlSuccess
    or DsmEventUCXIoctlFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSBDEVICE_ENABLE               deviceEnable;

    TRY {

        deviceEnable =
            (PUSBDEVICE_ENABLE) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory, NULL);

        RtlZeroMemory(deviceEnable, sizeof(*deviceEnable));

        deviceEnable->Size = sizeof(*deviceEnable);
        deviceEnable->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        deviceEnable->UsbDevice = DeviceContext->UsbDevice;
        deviceEnable->DefaultEndpoint = DeviceContext->DefaultEndpoint;

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_USBDEVICE_ENABLE);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to submit UCX IOCTL_INTERNAL_USB_USBDEVICE_ENABLE %!STATUS!",
                       status);
        }

    } FINALLY {

    }
}


VOID
HUBUCX_SetDeviceAddressUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends IOCTL_INTERNAL_USB_USBDEVICE_ADDRESS to the parent (UCX) to set the
    USB device address for the device.

    If WdfRequestSend fails then DsmEventUCXIoctlFailure should be queued
    directly from this function.  Completion routine of this function should
    queue a DsmEventUCXIoctlFailure or DsmEventUCXIoctlSuccess event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSBDEVICE_ADDRESS              deviceAddress;

    TRY {

        deviceAddress =
            (PUSBDEVICE_ADDRESS) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory, NULL);

        RtlZeroMemory(deviceAddress, sizeof(*deviceAddress));

        deviceAddress->Size = sizeof(*deviceAddress);
        deviceAddress->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        deviceAddress->UsbDevice = DeviceContext->UsbDevice;
        deviceAddress->Address = 0;

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_USBDEVICE_ADDRESS);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to submit UCX IOCTL_INTERNAL_USB_USBDEVICE_ADDRESS %!STATUS!",
                       status);
        }

    } FINALLY {

    }
}


VOID
HUBUCX_UpdateDefaultEndpointUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends IOCTL_INTERNAL_USB_DEFAULT_ENDPOINT_UPDATE to the parent (UCX) to
    update the endpoint's MaxPacketSize.

    If WdfRequestSend fails then DsmEventUCXIoctlFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventUCXIoctlFailure or DsmEventUCXIoctlSuccess event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PDEFAULT_ENDPOINT_UPDATE        defaultEndpointUpdate;

    TRY {

        defaultEndpointUpdate =
            (PDEFAULT_ENDPOINT_UPDATE) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory,
                                                          NULL);

        RtlZeroMemory(defaultEndpointUpdate, sizeof(*defaultEndpointUpdate));

        defaultEndpointUpdate->Size = sizeof(*defaultEndpointUpdate);
        defaultEndpointUpdate->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        defaultEndpointUpdate->UsbDevice = DeviceContext->UsbDevice;
        defaultEndpointUpdate->DefaultEndpoint = DeviceContext->DefaultEndpoint;
        defaultEndpointUpdate->MaxPacketSize = DeviceContext->MaxPacketSize;

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_DEFAULT_ENDPOINT_UPDATE);
        
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to submit UCX IOCTL_INTERNAL_USB_DEFAULT_ENDPOINT_UPDATE %!STATUS!",
                       status);
        }

    } FINALLY {

    }
}

BOOLEAN
HUBUCX_ComputeUsb20HardwareLpmParameters (
    __in
        PDEVICE_CONTEXT         DeviceContext,
    __inout
        PUSBDEVICE_UPDATE       DeviceUpdate
    )
/*++

Routine Description:
    
    Compute the 20 hardware LPM paramters for the device

Returns:

    Whether 20 LPM is enabled for the device

--*/
{
    PHUB_GLOBALS                    hubGlobals;
    BOOLEAN                         lpmEnabled;

    TRY {

        hubGlobals = GetHubGlobals(WdfGetDriver());

        //
        // If either host or device doesn't support 20 LPM Or if the 
        // global policy doesn't allow it Or if is a hub Or there is 
        // a hack flag diabling it for the device, then quit
        //
        if (DeviceContext->DeviceFlags.DeviceIsHub == 1) {
            DeviceContext->Usb20LpmStatus = Usb20LpmDisabledSinceDeviceIsHub;
            lpmEnabled = FALSE;
            LEAVE;
        }

        if (DeviceContext->DeviceHackFlags.DisableUsb20HardwareLpm == 1) {
            DeviceContext->Usb20LpmStatus = Usb20LpmDisabledDueToDeviceHackFlag;
            lpmEnabled = FALSE;
            LEAVE;
        }

        if (DeviceContext->HubFdoContext->HubInformation.HubFlags.DisableUsb20HardwareLpm) {
            DeviceContext->Usb20LpmStatus = Usb20LpmDisabledDueToHubHackFlag;
            lpmEnabled = FALSE;
            LEAVE;
        }

        if (hubGlobals->HubGlobalFlags.EnableUsb20HardwareLpm == 0) {
            DeviceContext->Usb20LpmStatus = Usb20LpmDisabledDueToHubGlobalFlag;
            lpmEnabled = FALSE;
            LEAVE;
        }

        if (DeviceContext->Lpm20Attributes.LPMCapable == 0) {
            DeviceContext->Usb20LpmStatus = Usb20LpmNotSupportedByDevice;
            lpmEnabled = FALSE;
            LEAVE;
        }

        if (DeviceContext->Lpm20Attributes.BESLAndAlternateHIRDSupported == 0) {
            //
            // It seems that though certain devices report that they are 2.0 LPMCapable
            // they run into issues if 2.0 LPM is really enabled. The reason for that is that most
            // the first generation XHCI controllers did not support 2.0 LPM. Thus there were no (few) 
            // XHCI controllers to test the 2.0 LPM functionality of the devices. 
            // Thus we do not want to enable 2.0 LPM on first generation USB 2.0 LPM capable devices. 
            // We leverage the BESLAndAlternateHIRDSupported bit of the BOS descriptor to determine 
            // if it is an older USB 2.0 device. 
            //
            DeviceContext->Usb20LpmStatus = Usb20LpmDisabledSinceBESLandAltHIRDNotSupported;
            lpmEnabled = FALSE;
            LEAVE;
        }

        if (DeviceContext->PortContext->PortInformation.PortFlags.PortFlagUsb20HardwareLpmSupported == 0) {
            DeviceContext->Usb20LpmStatus = Usb20LpmNotSupportedOnThisPort;
            lpmEnabled = FALSE;
            LEAVE;
        }

        DeviceContext->Usb20LpmStatus = Usb20LpmEnabled;

        lpmEnabled = TRUE;

        //
        // We only want hardware LPM enabled for root ports. RootHubInfo should 
        // only get populated for root hubs. 
        //
        NT_ASSERT(DeviceContext->HubFdoContext->HubParentInterface.HubDepth == 0);

        DeviceUpdate->Flags.Update20HardwareLpmParameters = 1;
        DeviceUpdate->Usb20HardwareLpmParameters.HardwareLpmEnable = 1;
        DeviceUpdate->Usb20HardwareLpmParameters.L1Timeout = hubGlobals->Usb20HardwareLpmTimeout;
        
        //
        // We assume that all devices support wake from L1, unless
        // we have a hack flag explicitly disabling it
        //
        if (DeviceContext->DeviceHackFlags.DisableRemoteWakeForUsb20HardwareLpm == 0) {
            DeviceUpdate->Usb20HardwareLpmParameters.RemoteWakeEnable = 1;
        }
        DeviceUpdate->Usb20HardwareLpmParameters.BestEffortServiceLatency = DEFAULT_20_HARDWARE_LPM_BASELINE_BESL;
        DeviceUpdate->Usb20HardwareLpmParameters.BestEffortServiceLatencyDeep = 0;
        DeviceUpdate->Usb20HardwareLpmParameters.HostInitiatedResumeDurationMode = 0;

        if ((DeviceContext->Lpm20Attributes.BESLAndAlternateHIRDSupported == 0) ||
            (DeviceContext->PortContext->PortInformation.PortFlags.PortFlagUsb20HardwareLpmBeslCapabilitySupported == 0)) {
            //
            // At least one of the device or the host does not understand the BESL errata
            // In this case, we will just go with the default safe values 
            //   
            LEAVE;
        }                                          
        
        if (DeviceContext->Lpm20Attributes.BaselineBESLValid == 1) {
            //
            // If device provides a value , choose that over the default
            //
            DeviceUpdate->Usb20HardwareLpmParameters.BestEffortServiceLatency = DeviceContext->Lpm20Attributes.BaselineBESL;
        }

        if (DeviceContext->Lpm20Attributes.DeepBESLValid == 1) {
            //
            // Only if the device provides Deep BESL , we will enable HIRDM
            //
            DeviceUpdate->Usb20HardwareLpmParameters.BestEffortServiceLatencyDeep = DeviceContext->Lpm20Attributes.DeepBESL;
            DeviceUpdate->Usb20HardwareLpmParameters.HostInitiatedResumeDurationMode = 1;  
        }                                                                       

    } FINALLY {

        if (DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagSpeedSuper) {
            //
            // Note: 
            // In this case this routine might still return TRUE, however in reality when the 
            // request goes down the the USB controller, the 20 LPM will not infact be enabled.
            //
            DeviceContext->Usb20LpmStatus = Usb20LpmDiabledSinceDeviceOperatingInSuperspeedModeOrHigher;
        }

    }

    return lpmEnabled;
}


VOID
HUBUCX_UpdateDeviceInfoUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends IOCTL_INTERNAL_USB_USBDEVICE_UPDATE to the root hub PDO (UCX) to
    update information about the USB device.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSBDEVICE_UPDATE               deviceUpdate;

    TRY {

        deviceUpdate = 
            (PUSBDEVICE_UPDATE) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory, NULL);

        RtlZeroMemory(deviceUpdate, sizeof(*deviceUpdate));
        //
        // We are going to update everything 
        //
        deviceUpdate->Flags.UpdateIsHub = 1;
        deviceUpdate->Flags.UpdateBosDescriptor = 1;
        deviceUpdate->Flags.UpdateDeviceDescriptor = 1;
        deviceUpdate->Flags.UpdateMaxExitLatency = 1;

        if (DeviceContext->DeviceHackFlags.AllowInvalidPipeHandles == 1) {
            deviceUpdate->Flags.UpdateAllowIoOnInvalidPipeHandles = 1;
        }

        deviceUpdate->Size = sizeof(*deviceUpdate);
        deviceUpdate->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        deviceUpdate->UsbDevice = DeviceContext->UsbDevice;
        deviceUpdate->DeviceDescriptor = &DeviceContext->DeviceDescriptor;
        deviceUpdate->BosDescriptor = DeviceContext->BOSDescriptor;

        if (HUBMISC_CheckIfU2ShouldBeSetForEnumeratedDevice(DeviceContext) == DsmEventYes) {  
            deviceUpdate->MaxExitLatency = DeviceContext->HostInitiatedU2ExitLatency;
        } else {
            deviceUpdate->MaxExitLatency = 0;        
        }
        
        if (DeviceContext->DeviceFlags.DeviceIsHub == 1) {
            deviceUpdate->IsHub = TRUE;
        }
        
        HUBUCX_ComputeUsb20HardwareLpmParameters(DeviceContext,
                                                 deviceUpdate);

        if ((DeviceContext->MsOs20Info.Flags.MinResumeTime == 1) &&
            (DeviceContext->DeviceFlags.DeviceNotRemovable == 1) &&
            (DeviceContext->DeviceFlags.DeviceIsHub == 0) &&
            (DeviceContext->PortContext->PortInformation.PortProtocol == PortProtocolUsb20)) {
            
            deviceUpdate->Flags.UpdateRootPortResumeTime = 1;

            if (DeviceContext->HubFdoContext->HubParentInterface.HubDepth == 0) {
                //
                // If root attached, use the resume time from the MS OS 2.0 descriptor
                //
                deviceUpdate->RootPortResumeTime = 
                    DeviceContext->MsOs20Info.MinResumeTimeDescriptor->bResumeSignalingTime;
            } else {
                //
                // If not root attached, use the standard 20ms resume time for the hub
                // connected to the root port.
                //
                deviceUpdate->RootPortResumeTime = PORT_RESUME_TIME;
            }
        }

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_USBDEVICE_UPDATE);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to submit UCX IOCTL_INTERNAL_USB_USBDEVICE_UPDATE %!STATUS!",
                       status);

            LEAVE;
        }

    } FINALLY {

    }
}

DSM_EVENT
HUBUCX_CheckIf20LPMIsEnabledAndUpdateUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if 20 LPM is enabled for the device. If yes, 
    Sends IOCTL_INTERNAL_USB_USBDEVICE_UPDATE to the root 
    hub PDO (UCX) to update 20 LPM latency parameters.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSBDEVICE_UPDATE               deviceUpdate;
    DSM_EVENT                       dsmEvent;

    TRY {

        deviceUpdate = 
            (PUSBDEVICE_UPDATE) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory, NULL);

        RtlZeroMemory(deviceUpdate, sizeof(*deviceUpdate));
        
        if (HUBUCX_ComputeUsb20HardwareLpmParameters(DeviceContext,
                                                     deviceUpdate) == FALSE) {
            dsmEvent = DsmEventNo;
            LEAVE;
        }

        dsmEvent = DsmEventYes;
        deviceUpdate->Size = sizeof(*deviceUpdate);
        deviceUpdate->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        deviceUpdate->UsbDevice = DeviceContext->UsbDevice;
        
        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_USBDEVICE_UPDATE);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to submit UCX IOCTL_INTERNAL_USB_USBDEVICE_UPDATE %!STATUS!",
                       status);

            LEAVE;
        }

    } FINALLY {

    }

    return dsmEvent;
}


VOID
HUBUCX_UpdateDeviceExitLatencyUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends IOCTL_INTERNAL_USB_USBDEVICE_UPDATE to the root hub PDO (UCX) to
    update information about the USB device.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSBDEVICE_UPDATE               deviceUpdate;

    TRY {

        if ((DeviceContext->MaxExitLatencyBasedOnEld > 0) &&
            (DeviceContext->TargetEffectiveExitLatency > DeviceContext->MaxExitLatencyBasedOnEld)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failing the max exit latency update(%d) based on MaxExitLatencyBasedOnEld (%d)",
                       DeviceContext->TargetEffectiveExitLatency,
                       DeviceContext->MaxExitLatencyBasedOnEld);
            HUBSM_AddDsmEvent(DeviceContext, DsmEventUCXIoctlFailureDueToExitLatencyTooLarge);
            LEAVE;
        }
        deviceUpdate = 
            (PUSBDEVICE_UPDATE) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory, NULL);

        RtlZeroMemory(deviceUpdate, sizeof(*deviceUpdate));
        //
        // We are going to update the exit latency only 
        //
        deviceUpdate->Flags.UpdateMaxExitLatency = 1;

        deviceUpdate->Size = sizeof(*deviceUpdate);
        deviceUpdate->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        deviceUpdate->UsbDevice = DeviceContext->UsbDevice;

        deviceUpdate->MaxExitLatency = DeviceContext->TargetEffectiveExitLatency;

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_USBDEVICE_UPDATE);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failed to submit UCX IOCTL_INTERNAL_USB_USBDEVICE_UPDATE %!STATUS!",
                       status);

            LEAVE;
        }

    } FINALLY {

    }
}


DSM_EVENT
HUBUCX_CreateDefaultEndpointInUCX (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Creates the default endpoint for a device via UCX's
    HUB_CONTROLLERSTACK_INTERFACE DefaultEndpointCreate function.  On
    successful completion caches the endpoint handle in the DSM context.

    NOTE: UCX docs indicate DefaultEndpointCreate must be called at PASSIVE IRQL.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    NTSTATUS                        status;
    PHUB_FDO_CONTEXT                hubFdoContext;
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;

    TRY {


        hubFdoContext = DeviceContext->HubFdoContext;
        hubControllerStackInterface = &hubFdoContext->HubControllerStackInterface;

        NT_ASSERT(hubControllerStackInterface->DefaultEndpointCreate != NULL);

        status =
            hubControllerStackInterface->DefaultEndpointCreate(
                                            hubFdoContext->HubParentInterface.Hub,
                                            DeviceContext->UsbDevice,
                                            DeviceContext->MaxPacketSize,
                                            &DeviceContext->DefaultEndpoint);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "UCX DefaultEndpointCreate failed %!STATUS!",
                       status);  
            
            DeviceContext->LastOperationUsbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;
            DeviceContext->LastOperationNtStatus = USBD_STATUS_To_NTSTATUS(USBD_STATUS_INSUFFICIENT_RESOURCES);  
        }

    } FINALLY {

    }

    return (NT_SUCCESS(status) ? DsmEventOperationSuccess : DsmEventOperationFailure);
}


DSM_EVENT
HUBUCX_CreateDeviceInUCX (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Creates the device in UCX via UCX's HUB_CONTROLLERSTACK_INTERFACE
    UsbDeviceCreate function.  On successful completion caches the returned
    USB device handle in the DSM context.

    NOTE: UCX docs indicate UsbDeviceCreate must be called at PASSIVE IRQL.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    NTSTATUS                        status;
    PHUB_FDO_CONTEXT                hubFdoContext;
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;
    PPORT_CONTEXT                   portContext;

    TRY {

        hubFdoContext = DeviceContext->HubFdoContext;
        hubControllerStackInterface = &hubFdoContext->HubControllerStackInterface;
        portContext = DeviceContext->PortContext;

        NT_ASSERT(hubControllerStackInterface->UsbDeviceCreate != NULL);    

        DeviceContext->UsbDeviceInfo.Size = sizeof(DeviceContext->UsbDeviceInfo);


        //
        // Examine the SubSm Flags to determine if this is a low, full, or high-speed device
        //
        if (DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagSpeedSuper) {

            DeviceContext->UsbDeviceInfo.DeviceSpeed = UsbSuperSpeed;
            DeviceContext->MaxPacketSize = 512;
        
        } else if (DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagSpeedHigh) {

            DeviceContext->UsbDeviceInfo.DeviceSpeed = UsbHighSpeed;
            DeviceContext->MaxPacketSize = 64;

        } else if (DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagSpeedLow) {

            DeviceContext->UsbDeviceInfo.DeviceSpeed = UsbLowSpeed;
            DeviceContext->MaxPacketSize = 8;

        } else {

            NT_ASSERT(DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagSpeedFull);

            DeviceContext->UsbDeviceInfo.DeviceSpeed = UsbFullSpeed;
            DeviceContext->MaxPacketSize = 64;

        }

        DeviceContext->UsbDeviceInfo.PortNumber = portContext->PortInformation.PortNumber;
        DeviceContext->UsbDeviceInfo.Context = (USB_DEVICE_CONTEXT)DeviceContext;

        status =
            hubControllerStackInterface->UsbDeviceCreate(hubFdoContext->HubParentInterface.Hub,
                                                         &DeviceContext->UsbDeviceInfo,
                                                         &DeviceContext->UsbDevice);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "UCX UsbDeviceCreate failed %!STATUS!",
                       status);
            
            DeviceContext->LastOperationUsbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;
            DeviceContext->LastOperationNtStatus = USBD_STATUS_To_NTSTATUS(USBD_STATUS_INSUFFICIENT_RESOURCES);
        
        } else{
            DeviceContext->DsmContext.ParentHandle = DeviceContext->UsbDevice; 
        }

    } FINALLY {

    }

    return (NT_SUCCESS(status) ? DsmEventOperationSuccess : DsmEventOperationFailure);
}


DSM_EVENT
HUBUCX_CreateEndpointsInUCX (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Creates all endpoints for the device's selected configuration via UCX's
    HUB_CONTROLLERSTACK_INTERFACE EndpointCreate function.  On successful
    completion caches all endpoint handles in the DSM context.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    NTSTATUS                        status;
    PHUB_FDO_CONTEXT                hubFdoContext;
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;
    PINTERFACE_HANDLE               interfaceHandle;
    PDEVICE_CONFIGURATION           configuration;
    UCXENDPOINT                     *endpointsToBeEnabled;
    UCXENDPOINT                     *endpointsToRemainUnchanged;
    ULONG                           i;


    TRY {

        status = STATUS_SUCCESS;
        configuration = NULL;

        hubFdoContext = DeviceContext->HubFdoContext;
        hubControllerStackInterface = &hubFdoContext->HubControllerStackInterface;

        if (DeviceContext->UcxEndpointArrayCount == 0) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "No endpoints to create");

            LEAVE;
        }

        NT_ASSERT(hubControllerStackInterface->EndpointCreate != NULL);

        configuration = DeviceContext->CurrentDeviceConfiguration;

        endpointsToBeEnabled = DeviceContext->EndpointsToBeEnabled;
        endpointsToRemainUnchanged = DeviceContext->EndpointsToRemainUnchanged;

        //
        // Walk the list of interfaces for this configuration and create
        // UCX endpoint handles for any endpoints without them.
        //
        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &configuration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            //
            // Walk the endpoint handles for this interface
            //
            for (i = 0; i < interfaceHandle->NumberOfEndpoints; i++) {
                PPIPE_HANDLE    pipeHandle;

                pipeHandle = &interfaceHandle->PipeHandle[i];

                if (pipeHandle->UcxEndpointState == UcxEndpointStateNotCreated) {

                    pipeHandle->UcxEndpointState = UcxEndpointStatePendingCreate;

                    status =
                        hubControllerStackInterface->EndpointCreate(
                                                        hubFdoContext->HubParentInterface.Hub,
                                                        DeviceContext->UsbDevice,
                                                        pipeHandle->EndpointDescriptor,
                                                        pipeHandle->EndpointDescriptorBufferLength,
                                                        pipeHandle->SSEndpointCompanionDescriptor,
                                                        &pipeHandle->UcxEndpoint);

                    if (!NT_SUCCESS(status)) {
                        DbgTraceTo(DeviceContext->PortContext->IfrLog,
                                   TL_ERROR,
                                   Device,
                                   "Unable to create UCX Endpoint %!STATUS!",
                                   status);
                        
                        pipeHandle->UcxEndpointState = UcxEndpointStateNotCreated;
                        LEAVE;
                    }

                    
                    if (pipeHandle->EndpointFlags.ZeroBandwidth == 1) {
                        //
                        // Don't enable zero bandwidth endpoints
                        //
                        pipeHandle->UcxEndpointState = UcxEndpointStateDisabled;
                        
                        endpointsToRemainUnchanged[DeviceContext->EndpointsToRemainUnchangedCount++] =
                            pipeHandle->UcxEndpoint;

                    } else {                       
                        
                        endpointsToBeEnabled[DeviceContext->EndpointsToBeEnabledCount++] =
                            pipeHandle->UcxEndpoint;

                    }
                }
            }
        }

    } FINALLY {

        if (configuration != NULL) {
            FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                            &configuration->InterfaceList,
                            InterfaceLink,
                            interfaceHandle) {

                //
                // Walk the endpoint handles for this interface
                //
                for (i = 0; i < interfaceHandle->NumberOfEndpoints; i++) {
                    PPIPE_HANDLE    pipeHandle;

                    pipeHandle = &interfaceHandle->PipeHandle[i];
                        
                    if (pipeHandle->UcxEndpointState == UcxEndpointStatePendingCreate) {

                        if (!NT_SUCCESS(status)) {
                                
                            //
                            // If we failed to create all of the endpoints, we must delete the
                            // ones we did create and reset their state to NotCreated
                            //
                            pipeHandle->UcxEndpointState = UcxEndpointStateNotCreated;
                            
                            hubControllerStackInterface->EndpointDelete(
                                                            hubFdoContext->HubParentInterface.Hub,
                                                            DeviceContext->UsbDevice,
                                                            pipeHandle->UcxEndpoint);

                        } else {
                            //
                            // Endpoint is now created and pending enable
                            //
                            pipeHandle->UcxEndpointState = UcxEndpointStatePendingEnable;
                        }
                    }
                }
            }
        }

        if (!NT_SUCCESS(status)) {
            
            DeviceContext->EndpointsToBeEnabledCount = 0;
            DeviceContext->EndpointsToRemainUnchangedCount = 0;
            
            DeviceContext->LastOperationUsbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;
            DeviceContext->LastOperationNtStatus = USBD_STATUS_To_NTSTATUS(USBD_STATUS_INSUFFICIENT_RESOURCES);

        }
    }

    return (NT_SUCCESS(status) ? DsmEventOperationSuccess : DsmEventOperationFailure);
}


VOID
HUBUCX_ReleaseAddress0OwnershipFromUCX (
    __in PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Releases ownership of the address 0 communication channel via the UCX's
    HUB_CONTROLLERSTACK_INTERFACE Address0OwnershipRelease function.

Returns:

    DsmEventOperationSuccess

--*/
{
    PHUB_FDO_CONTEXT                hubFdoContext;
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;

    TRY {

        hubFdoContext = DeviceContext->HubFdoContext;
        hubControllerStackInterface = &hubFdoContext->HubControllerStackInterface;

        NT_ASSERT(hubControllerStackInterface->Address0OwnershipRelease != NULL);

        hubControllerStackInterface->Address0OwnershipRelease(
                                        hubFdoContext->HubParentInterface.Hub,
                                        DeviceContext->UsbDevice);
    } FINALLY {

    }
}


EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBUCX_GetRootHubInfoUsingUCXIoctlComplete;

VOID
HUBUCX_GetRootHubInfoUsingUCXIoctlComplete(
    __in
        WDFREQUEST                      WdfRequest,
    __in
        WDFIOTARGET                     Target,
    __in
        PWDF_REQUEST_COMPLETION_PARAMS  CompletionParams,
    __in
        WDFCONTEXT                      Context
    )
/*++

Routine Description:

    Completion routine for IOCTL_INTERNAL_USB_GET_ROOTHUB_INFO.  Queues
    a HsmEventIoctlSuccess or HsmEventIoctlFailure event to HSM.

Returns:

    VOID

--*/
{
    NTSTATUS                status;
    PHUB_FDO_CONTEXT        hubFdoContext;

    UNREFERENCED_PARAMETER(Target);

    TRY {

        hubFdoContext = (PHUB_FDO_CONTEXT) Context;

        status = CompletionParams->IoStatus.Status;

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "IOCTL_INTERNAL_USB_GET_ROOTHUB_INFO failed %!STATUS!",
                       status);

            LEAVE;
        }

        hubFdoContext->HubInformation.NumberOf20Ports =
            hubFdoContext->HubInformation.RootHubInfo.NumberOf20Ports;

		hubFdoContext->HubInformation.NumberOf30Ports =
            hubFdoContext->HubInformation.RootHubInfo.NumberOf30Ports;

        hubFdoContext->StatusChangeInterruptRequest.UsbdInterruptPipeHandle =
            hubFdoContext->HubInformation.RootHubInfo.UsbdInterruptPipeHandle;

        hubFdoContext->HubInfoFromParent.U1ExitLatency = 
            hubFdoContext->HubInformation.RootHubInfo.MaxU1ExitLatency;

        hubFdoContext->HubInfoFromParent.U2ExitLatency = 
            hubFdoContext->HubInformation.RootHubInfo.MaxU2ExitLatency;

    } FINALLY {

        //
        // Notify HSM of completion status of the IOCTL
        //
        HUBSM_AddHsmEvent(hubFdoContext,
                          NT_SUCCESS(status) ? HsmEventIoctlSuccess : HsmEventIoctlFailure);

        WdfObjectDelete(WdfRequest);
    }
}


VOID
HUBUCX_GetRootHubInfoUsingUCXIoctl (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Sends IOCTL_INTERNAL_USB_GET_ROOTHUB_INFO to the parent (UCX) to
    get Interrupt Endpoint and information about root hub ports.

    If WdfRequestSend fails then HsmEventIoctlFailure should be queued
    directly from this function.

    Completion routine of this function should queue a HsmEventIoctlSuccess
    or HsmEventIoctlFailure event to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    WDF_OBJECT_ATTRIBUTES           attributes;
    WDFREQUEST                      request;
    IO_STACK_LOCATION               ioStackLocation;
    BOOLEAN                         requestSent;
    WDFIOTARGET                     ioTarget;

    TRY {

        ioTarget = WdfDeviceGetIoTarget(HubFdoContext->HubFdoDevice);

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = HubFdoContext->HubFdoDevice;

        status = WdfRequestCreate(&attributes,
                                  ioTarget,
                                  &request);

        if ( !NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfRequestCreate Failed %!STATUS!",
                       status);

            LEAVE;
        }

        HubFdoContext->HubInformation.RootHubInfo.Size =
            sizeof(HubFdoContext->HubInformation.RootHubInfo);

        RtlZeroMemory(&ioStackLocation,  sizeof(ioStackLocation));
        ioStackLocation.MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        ioStackLocation.Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_ROOTHUB_GET_INFO;
        ioStackLocation.Parameters.Others.Argument1 = &HubFdoContext->HubInformation.RootHubInfo;

        WdfRequestWdmFormatUsingStackLocation(request, &ioStackLocation);
        
        WdfRequestSetCompletionRoutine(request,
                                       HUBUCX_GetRootHubInfoUsingUCXIoctlComplete,
                                       HubFdoContext);

        requestSent = WdfRequestSend(request, ioTarget, NULL);

        if (requestSent == FALSE) {

            status = WdfRequestGetStatus(request);

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Failed to submit IOCTL_INTERNAL_USB_ROOTHUB_GET_INFO! %!STATUS!",
                       status);

            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (request != NULL) {
                WdfObjectDelete(request);
            }

            HUBSM_AddHsmEvent(HubFdoContext, HsmEventIoctlFailure);
        }
    }
}


EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBUCX_GetRootHub20PortsInfoUsingUCXIoctlComplete;

VOID
HUBUCX_GetRootHub20PortsInfoUsingUCXIoctlComplete(
    __in WDFREQUEST                         WdfRequest,
    __in WDFIOTARGET                        Target,
    __in PWDF_REQUEST_COMPLETION_PARAMS     CompletionParams,
    __in WDFCONTEXT                         Context
    )
/*++

Routine Description:

    Completion routine for IOC TL_INTERNAL_USB_GET_ROOTHUB_20PORTS_INFO.  Queues
    HsmEventIoctlSuccess or HsmEventIoctlFailure event to HSM.

Returns:

    VOID

--*/
{
    NTSTATUS                status;
    PHUB_FDO_CONTEXT        hubFdoContext;

    UNREFERENCED_PARAMETER(Target);

    TRY {

        hubFdoContext = (PHUB_FDO_CONTEXT)Context;

        status = CompletionParams->IoStatus.Status;

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "IOCTL_INTERNAL_USB_GET_ROOTHUB_20PORTS_INFO failed! %!STATUS!",
                       status);
        }        
        
        //
        // Notify HSM of completion status of the IOCTL
        //
        HUBSM_AddHsmEvent(hubFdoContext,
                          NT_SUCCESS(status) ? HsmEventIoctlSuccess : HsmEventIoctlFailure);

        WdfObjectDelete(WdfRequest);

    } FINALLY {

    }
}


VOID
HUBUCX_GetRootHub20PortsInfoUsingUCXIoctl (
    __in
    PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Sends IOCTL_INTERNAL_USB_GET_ROOTHUB_20PORTS_INFO to the parent (UCX)
    to get information about 2.0 root hub ports, and store in the hub FDO
    context.

    If WdfRequestSend fails then HsmEventIoctlFailure should be queued
    directly from this function.

    Completion routine of this function should queue a HsmEventIoctlSuccess
    or HsmEventIoctlFailure event to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    WDF_OBJECT_ATTRIBUTES           attributes;
    WDFREQUEST                      request;
    BOOLEAN                         requestSent;
    size_t                          bufferSize;
    WDFMEMORY                       rootHub20PortsInfoMemory;
    WDFMEMORY                       rootHub20PortInfoPointerArrayMemory;
    PROOTHUB_20PORTS_INFO           rootHub20PortsInfo;
    PROOTHUB_20PORT_INFO            *rootHub20PortInfoPointerArray;
    PROOTHUB_20PORT_INFO            rootHub20PortInfoArray;
    ULONG                           i;
    WDFIOTARGET                     ioTarget;

    TRY {

        rootHub20PortInfoPointerArrayMemory = NULL;
        request = NULL;

        //
        // Do not do anything if 2.0 ports are unavailable
        //
        if (HubFdoContext->HubInformation.RootHubInfo.NumberOf20Ports == 0) {
            HubFdoContext->HubInformation.RootHub20PortInfoPointerArray = NULL;
            status = STATUS_SUCCESS;
            HUBSM_AddHsmEvent(HubFdoContext, HsmEventIoctlSuccess);
            LEAVE;
        }

        ioTarget = WdfDeviceGetIoTarget(HubFdoContext->HubFdoDevice);

        //
        // Allocate the request for the UCX IOCTL
        //
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = HubFdoContext->HubFdoDevice;

        status = WdfRequestCreate(&attributes, ioTarget, &request);

        if (!NT_SUCCESS(status)) {
            request = NULL;

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfRequestCreate Failed %!STATUS!",
                       status);

            LEAVE;
        }

        //
        // Allocate one block of memory for both the array of PROOTHUB_20PORT_INFO pointers
        // and the array of ROOTHUB_20_PORT_INFO structures
        //
        bufferSize = (sizeof(ROOTHUB_20PORT_INFO) + sizeof(PROOTHUB_20PORT_INFO)) *
                       HubFdoContext->HubInformation.RootHubInfo.NumberOf20Ports;

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = HubFdoContext->HubFdoDevice;

        status =
            WdfMemoryCreate(&attributes,
                            NonPagedPool,
                            USBHUB3_PORT_TAG,
                            bufferSize,
                            &rootHub20PortInfoPointerArrayMemory,
                            (PVOID)&HubFdoContext->HubInformation.RootHub20PortInfoPointerArray);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfMemoryCreate Failed for ROOTHUB_20PORTS_INFO Array %!STATUS!",
                       status);
            LEAVE;
        }

        //
        // Allocate the ROOTHUB_20PORTS_INFO structure that is passed to UCX as a parameter
        //
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = request;

        status = WdfMemoryCreate(&attributes,
                                 NonPagedPool,
                                 USBHUB3_PORT_TAG,
                                 sizeof(ROOTHUB_20PORTS_INFO),
                                 &rootHub20PortsInfoMemory,
                                 (PVOID) &rootHub20PortsInfo);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfMemoryCreate Failed for ROOTHUB_20PORT_INFO Structure %!STATUS!",
                       status);

            LEAVE;
        }

        rootHub20PortInfoPointerArray =
            HubFdoContext->HubInformation.RootHub20PortInfoPointerArray;

        //
        // Initialize ROOTHUB_20PORTS_INFO structure
        //
        rootHub20PortsInfo->Size = sizeof(ROOTHUB_20PORTS_INFO);
        rootHub20PortsInfo->NumberOfPorts =
            HubFdoContext->HubInformation.RootHubInfo.NumberOf20Ports;
        rootHub20PortsInfo->PortInfoSize = sizeof(ROOTHUB_20PORT_INFO);
        rootHub20PortsInfo->PortInfoArray = rootHub20PortInfoPointerArray;

        //
        // rootHub20PortInfoPointerArray is an array of ROOTHUB_20PORT_INFO pointers, followed
        // by the array of ROOTHUB_20PORT_INFO structures that the array of pointers are supposed
        // to point to.  Compute the offset to beginning of ROOTHUB_20PORT_INFO structures
        //
        rootHub20PortInfoArray =
            (PROOTHUB_20PORT_INFO)((PCHAR)rootHub20PortInfoPointerArray +
                                   (sizeof(PROOTHUB_20PORT_INFO) *
                                    rootHub20PortsInfo->NumberOfPorts));

        //
        // Initialize the array of ROOTHUB_20PORT_INFO structures and the array of pointers
        //
        for (i = 0; i < rootHub20PortsInfo->NumberOfPorts; i++) {

            *rootHub20PortInfoPointerArray = rootHub20PortInfoArray;
            rootHub20PortInfoPointerArray++;
            rootHub20PortInfoArray->MinorRevision = 0;
            rootHub20PortInfoArray->HubDepth = 0;
            rootHub20PortInfoArray++;
        }

        status = WdfIoTargetFormatRequestForInternalIoctlOthers(
                            ioTarget,
                            request,
                            IOCTL_INTERNAL_USB_ROOTHUB_GET_20PORT_INFO,
                            rootHub20PortsInfoMemory,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Formatting Target Request for IOCTL_INTERNAL_USB_GET_ROOTHUB_20PORTS_INFO Failed %!STATUS!",
                       status);
            LEAVE;
        }

        WdfRequestSetCompletionRoutine(request,
                                       HUBUCX_GetRootHub20PortsInfoUsingUCXIoctlComplete,
                                       HubFdoContext);


        requestSent = WdfRequestSend(request, ioTarget, NULL);

        if (requestSent == FALSE) {

            status = WdfRequestGetStatus(request);

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Failed to submit IOCTL_INTERNAL_USB_GET_ROOTHUB_20PORTS_INFO %!STATUS!",
                       status);

            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (request != NULL) {
                WdfObjectDelete(request);
            }

            if (rootHub20PortInfoPointerArrayMemory != NULL) {
                WdfObjectDelete(rootHub20PortInfoPointerArrayMemory);

                HubFdoContext->HubInformation.RootHub20PortInfoPointerArray = NULL;
            }

            HUBSM_AddHsmEvent(HubFdoContext, HsmEventIoctlFailure);
        }
    }
}



EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBUCX_GetRootHub30PortsInfoUsingUCXIoctlComplete;

VOID
HUBUCX_GetRootHub30PortsInfoUsingUCXIoctlComplete(
    __in
        WDFREQUEST                      WdfRequest,
    __in
        WDFIOTARGET                     Target,
    __in
        PWDF_REQUEST_COMPLETION_PARAMS  CompletionParams,
    __in
        WDFCONTEXT                      Context
    )
/*++

Routine Description:

    Completion routine for IOCTL_INTERNAL_USB_GET_ROOTHUB_30PORTS_INFO.  Queues
    HsmEventIoctlSuccess or HsmEventIoctlFailure event to HSM.

Returns:

    VOID

--*/
{
    NTSTATUS                status;
    PHUB_FDO_CONTEXT        hubFdoContext;

    UNREFERENCED_PARAMETER(Target);


    TRY {

        hubFdoContext = (PHUB_FDO_CONTEXT) Context;

        status = CompletionParams->IoStatus.Status;

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "IOCTL_INTERNAL_USB_GET_ROOTHUB_30PORTS_INFO failed! %!STATUS!",
                       status);
        }

        //
        // Notify HSM of completion status of the IOCTL
        //
        HUBSM_AddHsmEvent(hubFdoContext,
                          NT_SUCCESS(status) ? HsmEventIoctlSuccess : HsmEventIoctlFailure);

        WdfObjectDelete(WdfRequest);

    } FINALLY {

    }
}


VOID
HUBUCX_GetRootHub30PortsInfoUsingUCXIoctl (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Sends IOCTL_INTERNAL_USB_GET_ROOTHUB_30PORTS_INFO to the parent (UCX)
    to get information about 3.0 root hub ports, and store in the hub FDO
    context.

    If WdfRequestSend fails then HsmEventIoctlFailure should be queued
    directly from this function.

    Completion routine of this function should queue a HsmEventIoctlSuccess
    or HsmEventIoctlFailure event to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    WDF_OBJECT_ATTRIBUTES           attributes;
    WDFREQUEST                      request;
    BOOLEAN                         requestSent;
    size_t                          bufferSize;
    WDFMEMORY                       rootHub30PortsInfoMemory;
    WDFMEMORY                       rootHub30PortInfoPointerArrayMemory;
    PROOTHUB_30PORTS_INFO           rootHub30PortsInfo;
    PROOTHUB_30PORT_INFO_EX         *rootHub30PortInfoPointerArray;
    PROOTHUB_30PORT_INFO_EX         rootHub30PortInfoArray;
    PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED  speeds;
    ULONG                           i;
    WDFIOTARGET                     ioTarget;


    TRY {

        rootHub30PortInfoPointerArrayMemory = NULL;
        request = NULL;

        //
        // Do not do anything if 3.0 ports are unavailable
        //
        if (HubFdoContext->HubInformation.RootHubInfo.NumberOf30Ports == 0) {
            HubFdoContext->HubInformation.RootHub30PortInfoPointerArray = NULL;
            status = STATUS_SUCCESS;
            HUBSM_AddHsmEvent(HubFdoContext, HsmEventIoctlSuccess);
            LEAVE;
        }

        ioTarget = WdfDeviceGetIoTarget(HubFdoContext->HubFdoDevice);

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = HubFdoContext->HubFdoDevice;

        status = WdfRequestCreate(&attributes, ioTarget, &request);

        if ( !NT_SUCCESS(status)) {
            request = NULL;

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfRequestCreate Failed %!STATUS!",
                       status);

            LEAVE;
        }

        //
        // Allocate one block of memory for the array of PROOTHUB_30PORT_INFO_EX pointers,
        // the array of ROOTHUB_30_PORT_INFO structures, and the array of Speed IDs
        //
        bufferSize = (sizeof(ROOTHUB_30PORT_INFO_EX) + sizeof(PROOTHUB_30PORT_INFO_EX) +
                      MAX_SPEEDS_COUNT * sizeof(USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED)) *
                     HubFdoContext->HubInformation.RootHubInfo.NumberOf30Ports;

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = HubFdoContext->HubFdoDevice;

        status =
            WdfMemoryCreate(&attributes,
                            NonPagedPool,
                            USBHUB3_PORT_TAG,
                            bufferSize,
                            &rootHub30PortInfoPointerArrayMemory,
                            (PVOID)&HubFdoContext->HubInformation.RootHub30PortInfoPointerArray);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfMemoryCreate Failed for ROOTHUB_30PORTS_INFO Array %!STATUS!",
                       status);
            LEAVE;
        }

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = request;

        status = WdfMemoryCreate (&attributes,
                                  NonPagedPool,
                                  USBHUB3_PORT_TAG,
                                  sizeof(ROOTHUB_30PORTS_INFO),
                                  &rootHub30PortsInfoMemory,
                                  (PVOID)&rootHub30PortsInfo);

        if (!NT_SUCCESS(status)) {
            rootHub30PortsInfoMemory = NULL;

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfMemoryCreate Failed for ROOTHUB_30PORT_INFO_EX Structure %!STATUS!",
                       status);

            LEAVE;
        }

        rootHub30PortInfoPointerArray =
            HubFdoContext->HubInformation.RootHub30PortInfoPointerArray;

        //
        // Initialize ROOTHUB_30PORTS_INFO structure
        //
        rootHub30PortsInfo->Size = sizeof(ROOTHUB_30PORTS_INFO);
        rootHub30PortsInfo->NumberOfPorts =
            HubFdoContext->HubInformation.RootHubInfo.NumberOf30Ports;
        rootHub30PortsInfo->PortInfoSize = sizeof(ROOTHUB_30PORT_INFO_EX);
        rootHub30PortsInfo->PortInfoArray = (PROOTHUB_30PORT_INFO *)rootHub30PortInfoPointerArray;

        //
        // Compute offset to beginning of ROOTHUB_30PORT_INFO_EX structures
        //
        rootHub30PortInfoArray =
            (PROOTHUB_30PORT_INFO_EX)((PCHAR)rootHub30PortInfoPointerArray +
                                      (sizeof(PROOTHUB_30PORT_INFO_EX) *
                                      rootHub30PortsInfo->NumberOfPorts));

        //
        // Compute offset to beginning of USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED structures
        //
        speeds =
            (PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED)((PCHAR)rootHub30PortInfoArray +
                                                          (sizeof(ROOTHUB_30PORT_INFO_EX) *
                                                           rootHub30PortsInfo->NumberOfPorts));

        //
        // Initialize the array of ROOTHUB_30PORT_INFO_EX structures
        //
        for (i = 0; i < rootHub30PortsInfo->NumberOfPorts; i++) {

            *rootHub30PortInfoPointerArray = rootHub30PortInfoArray;
            rootHub30PortInfoPointerArray++;
            rootHub30PortInfoArray->Speeds = speeds;
            rootHub30PortInfoArray->MinorRevision = 0;
            rootHub30PortInfoArray->HubDepth = 0;
            rootHub30PortInfoArray->SpeedsCount = 0;
            rootHub30PortInfoArray->MaxSpeedsCount = MAX_SPEEDS_COUNT;
            rootHub30PortInfoArray++;
            speeds += MAX_SPEEDS_COUNT;
        }

        status = WdfIoTargetFormatRequestForInternalIoctlOthers(
                            ioTarget,
                            request,
                            IOCTL_INTERNAL_USB_ROOTHUB_GET_30PORT_INFO,
                            rootHub30PortsInfoMemory,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            NULL);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Formatting Target Request for IOCTL_INTERNAL_USB_GET_ROOTHUB_30PORTS_INFO Failed %!STATUS!",
                       status);
            LEAVE;
        }

        WdfRequestSetCompletionRoutine(request,
                                       HUBUCX_GetRootHub30PortsInfoUsingUCXIoctlComplete,
                                       HubFdoContext);

        requestSent = WdfRequestSend(request, ioTarget, NULL);

        if ( requestSent == FALSE ) {

            status = WdfRequestGetStatus(request);

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Failed to send IOCTL_INTERNAL_USB_GET_ROOTHUB_30PORTS_INFO %!STATUS!",
                       status);

            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (request != NULL) {
                WdfObjectDelete(request);
            }

            if (rootHub30PortInfoPointerArrayMemory != NULL) {
                WdfObjectDelete(rootHub30PortInfoPointerArrayMemory);

                HubFdoContext->HubInformation.RootHub30PortInfoPointerArray = NULL;
            }

            HUBSM_AddHsmEvent(HubFdoContext, HsmEventIoctlFailure);
        }
    }
}


VOID
HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Deconfigures all endpoints cached in the DSM context via
    IOCTL_INTERNAL_USB_ENDPOINTS_CONFIGURE.

    Completion routine of this function should queue a
    DsmEventUCXIoctlSuccess event to DSM.

Returns:

    VOID

--*/
{
    BOOLEAN                 ioctlSubmitted;
    PINTERFACE_HANDLE       interfaceHandle;
    PDEVICE_CONFIGURATION   configuration;

    TRY {
        ioctlSubmitted = FALSE;

        configuration = DeviceContext->CurrentDeviceConfiguration;

        if (configuration == NULL) {
            LEAVE;
        }

        if (DeviceContext->UcxEndpointArrayCount == 0) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "No endpoints to deconfigure");

            LEAVE;
        }

        DeviceContext->EndpointsToBeEnabledCount = 0;
        DeviceContext->EndpointsToRemainUnchangedCount = 0;
        DeviceContext->EndpointsToBeDisabledCount = 0;

        //
        // Walk the list of interfaces for this configuration and add all its endpoints to
        // disabled UCX endpoint array
        //
        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &configuration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {
            HUBMISC_AddEnabledEndpointsToPendingDisableList(DeviceContext, interfaceHandle);
        }

        if (DeviceContext->EndpointsToBeDisabledCount != 0) {
            HUBUCX_EnableDisableEndpointsUsingUCXIoctl(DeviceContext);
            ioctlSubmitted = TRUE;
        }

    } FINALLY {

        //
        // if we have no endpoints to disable, indicate IOCTL success to DSM here
        //
        if (ioctlSubmitted == FALSE) {
            HUBSM_AddDsmEvent(DeviceContext, DsmEventUCXIoctlSuccess);
        }
    }
}

VOID
HUBUCX_DisableAllEndpointsForNewInterfaceUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Deconfigures all endpoints belonging to the new
    Interface that the client driver was trying to
    switch to. Completion routine of this function 
    should queue a DsmEventUCXIoctlSuccess event to 
    DSM.

Returns:

    VOID

--*/
{
    BOOLEAN                 ioctlSubmitted;
    PINTERFACE_HANDLE       interfaceHandle;

    TRY {
        ioctlSubmitted = FALSE;

        interfaceHandle = DeviceContext->NewInterface;

        if (interfaceHandle == NULL) {
            LEAVE;
        }

        if (DeviceContext->UcxEndpointArrayCount == 0) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "No endpoints to deconfigure");

            LEAVE;
        }

        DeviceContext->EndpointsToBeEnabledCount = 0;
        DeviceContext->EndpointsToRemainUnchangedCount = 0;
        DeviceContext->EndpointsToBeDisabledCount = 0;

        HUBMISC_AddEnabledEndpointsToPendingDisableList(DeviceContext, interfaceHandle);

        if (DeviceContext->EndpointsToBeDisabledCount != 0) {
            HUBUCX_EnableDisableEndpointsUsingUCXIoctl(DeviceContext);
            ioctlSubmitted = TRUE;
        }

    } FINALLY {

        //
        // if we have no endpoints to disable, indicate IOCTL success to DSM here
        //
        if (ioctlSubmitted == FALSE) {
            HUBSM_AddDsmEvent(DeviceContext, DsmEventUCXIoctlSuccess);
        }
    }
}


VOID
HUBUCX_DeleteDeviceFromUCX (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Deletes device from UCX using UsbDeviceDelete

Returns:

    VOID

--*/
{
    PHUB_FDO_CONTEXT                hubFdoContext;
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;

    TRY {

        hubFdoContext = DeviceContext->HubFdoContext;
        hubControllerStackInterface = &hubFdoContext->HubControllerStackInterface;

        NT_ASSERT(hubControllerStackInterface->UsbDeviceDelete != NULL);

        hubControllerStackInterface->UsbDeviceDelete(hubFdoContext->HubParentInterface.Hub,
                                                     DeviceContext->UsbDevice);

    } FINALLY {

    }
}


VOID
HUBUCX_DeleteEndpointsAndFreeResourcesForInterface (
    __in
        PDEVICE_CONTEXT        DeviceContext,
    __in
        PINTERFACE_HANDLE      Interface
    )
/*++

Routine Description:

    Deletes all endpoints and release WDF memory objects
    for the given interface
    
Returns:

    VOID

--*/
{
    PHUB_FDO_CONTEXT                hubFdoContext;
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;
    ULONG                           i;

    TRY {

        hubFdoContext = DeviceContext->HubFdoContext;
        hubControllerStackInterface = &hubFdoContext->HubControllerStackInterface;

        NT_ASSERT(hubControllerStackInterface->EndpointDelete != NULL);
        
        //
        // Walk the endpoint handles for the interface and delete them
        //
        for (i=0; i<Interface->NumberOfEndpoints; i++) {

            if (Interface->PipeHandle[i].UcxEndpointState == UcxEndpointStateDisabled) {

                hubControllerStackInterface->EndpointDelete(hubFdoContext->HubParentInterface.Hub,
                                                            DeviceContext->UsbDevice,
                                                            Interface->PipeHandle[i].UcxEndpoint);
                
                Interface->PipeHandle[i].UcxEndpointState = UcxEndpointStateDeleted;

            } else {
                NT_ASSERT(Interface->PipeHandle[i].UcxEndpointState == UcxEndpointStateNotCreated);
            }

        }

        //
        // Free the interface handle
        //
        WdfObjectDelete(Interface->MemoryObject);

    } FINALLY {

    }
}

VOID
HUBUCX_DeleteEndpointsAndFreeResourcesForOldInterface (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Deletes all endpoints and release WDF memory objects
    for the old interface that is being replaced as part
    of set interface request from the client

Returns:

    VOID

--*/
{
    
    if (DeviceContext->OldInterface != NULL) {
        HUBUCX_DeleteEndpointsAndFreeResourcesForInterface(DeviceContext,
                                                           DeviceContext->OldInterface);
        DeviceContext->OldInterface = NULL;
    }
}

VOID
HUBUCX_DeleteEndpointsAndFreeResourcesForNewInterface (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Deletes all endpoints and release WDF memory objects
    for the new interface that is being chosen as part
    of set interface request from the client. This will
    only be called in a failure path

Returns:

    VOID

--*/
{
    PINTERFACE_HANDLE           newInterface;

    newInterface = DeviceContext->NewInterface;

    if (newInterface != NULL) {
        //
        // Remove the interface from the current configuration
        //
        RemoveEntryList(&newInterface->InterfaceLink);   

        //
        // Update the number of endpoints for the current configuration
        //
        DeviceContext->CurrentDeviceConfiguration->NumberOfEndpoints -= 
                newInterface->NumberOfEndpoints;

        HUBUCX_DeleteEndpointsAndFreeResourcesForInterface(DeviceContext,
                                                           newInterface);
        DeviceContext->NewInterface = NULL;
    }
}

VOID
HUBUCX_DeleteEndpointsAndFreeResourcesForConfiguration (
    __in
        PDEVICE_CONTEXT             DeviceContext,
    __in
        PDEVICE_CONFIGURATION       Configuration
    )
/*++

Routine Description:

    Deletes all endpoints and release WDF memory objects
    for the given configuration

Returns:

    VOID

--*/
{
    PHUB_FDO_CONTEXT                hubFdoContext;
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;
    PINTERFACE_HANDLE               currentInterfaceHandle;
    PINTERFACE_HANDLE               nextInterfaceHandle;

    TRY {

        hubFdoContext = DeviceContext->HubFdoContext;
        hubControllerStackInterface = &hubFdoContext->HubControllerStackInterface;

        NT_ASSERT(hubControllerStackInterface->EndpointDelete != NULL);

        //
        // Delete endpoints and interfaces
        //
        FOR_ALL_IN_LIST_SAFE(INTERFACE_HANDLE, 
                             &Configuration->InterfaceList,
                             InterfaceLink,
                             currentInterfaceHandle,
                             nextInterfaceHandle) {
        
            RemoveEntryList(&currentInterfaceHandle->InterfaceLink);

            HUBUCX_DeleteEndpointsAndFreeResourcesForInterface(DeviceContext, currentInterfaceHandle);
        }

        //
        // Delete the configuration
        //
        WdfObjectDelete(Configuration->WdfMemory);

    } FINALLY {

    }
}

VOID
HUBUCX_DeleteEndpointsAndFreeResourcesForOldConfiguration (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Deletes all endpoints and release WDF memory objects
    for the old configuration that is being replaced as part
    of select configuration request from the client

Returns:

    VOID

--*/
{
    if (DeviceContext->OldDeviceConfiguration) {
        HUBUCX_DeleteEndpointsAndFreeResourcesForConfiguration(DeviceContext,
                                                               DeviceContext->OldDeviceConfiguration);
        DeviceContext->OldDeviceConfiguration = NULL;
    }
}

VOID
HUBUCX_DeleteEndpointsAndFreeResourcesForCurrentConfiguration (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Deletes all endpoints and release WDF memory objects
    for the current device configuration. This will
    called in the cleanup path or when the device is 
    deconfigured as part of client choosing null configuration

Returns:

    VOID

--*/
{
    if (DeviceContext->CurrentDeviceConfiguration) {
        HUBUCX_DeleteEndpointsAndFreeResourcesForConfiguration(DeviceContext,
                                                               DeviceContext->CurrentDeviceConfiguration);
        DeviceContext->CurrentDeviceConfiguration = NULL;
    }
}


VOID
HUBUCX_DeleteDefaultEndpointFromUCX (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Deletes Default Endpoint from UCX

Returns:

    VOID

--*/
{
    PHUB_FDO_CONTEXT                hubFdoContext;
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;

    TRY {

        hubFdoContext = DeviceContext->HubFdoContext;
        hubControllerStackInterface = &hubFdoContext->HubControllerStackInterface;

        NT_ASSERT(hubControllerStackInterface->EndpointDelete != NULL);

        hubControllerStackInterface->EndpointDelete(hubFdoContext->HubParentInterface.Hub,
                                                    DeviceContext->UsbDevice,
                                                    DeviceContext->DefaultEndpoint);

        DeviceContext->DefaultEndpoint = NULL;

    } FINALLY {

    }
}


VOID
HUBUCX_ResetEndpointUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    In response to a client URB, this function is called to reset the endpoint.
    This function sends the IOCTL_INTERNAL_USB_ENDPOINT_RESET ioctl with
    FlagEndpointResetPreserveTransferState as 1. If WdfRequestSend fails then
    DsmEventUCXIoctlFailure should be queued directly from this function.

    Completion routine of this function should queue a DsmEventUCXIoctlFailure
    or DsmEventUCXIoctlSuccess event to DSM.

    Completion routine of this function should also store the NTSTATUS at a
    particular place in the device context.

Returns:


--*/
{
    NTSTATUS                status;
    WDFREQUEST              request;
    WDF_REQUEST_PARAMETERS  parameters;
    PENDPOINT_RESET         endpointReset;
    PPIPE_HANDLE            pipeHandle;

    TRY {

        request = DeviceContext->CurrentClientRequest;

        WDF_REQUEST_PARAMETERS_INIT(&parameters);
        WdfRequestGetParameters(request, &parameters);

        endpointReset = (PENDPOINT_RESET) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory,
                                                             NULL);

        RtlZeroMemory(endpointReset, sizeof(*endpointReset));

        pipeHandle = DeviceContext->CurrentTargetPipe;

        if (pipeHandle == NULL) {
            DeviceContext->LastOperationUsbdStatus = USBD_STATUS_INVALID_PIPE_HANDLE;
            DeviceContext->LastOperationNtStatus =  USBD_STATUS_To_NTSTATUS(USBD_STATUS_INVALID_PIPE_HANDLE);

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Invalid Pipe Handle!");

            HUBSM_AddDsmEvent(DeviceContext, DsmEventUCXIoctlFailure);
            LEAVE;
        }
        
        endpointReset->Size = sizeof(*endpointReset);
        endpointReset->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        endpointReset->UsbDevice = DeviceContext->UsbDevice;
        endpointReset->Endpoint = pipeHandle->UcxEndpoint;
        endpointReset->Flags = FlagEndpointResetPreserveTransferState;

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_ENDPOINT_RESET);

        //
        // Using _IOCTL_INTERNAL_USB_ENDPOINT_RESET to make sure the compiler 
        // doesn't optimize away unused enum types from the symbol file.
        // 
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "%!UCX_IOCTL! failed! %!STATUS!",
                       _IOCTL_INTERNAL_USB_ENDPOINT_RESET,
                       status);
        }
    
    } FINALLY {

    }
}


VOID
HUBUCX_ResetEndpointAndTransferStateUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    In response to a client URB, this function is called to reset the endpoint.
    This function sends the IOCTL_INTERNAL_USB_ENDPOINT_RESET ioctl with
    FlagEndpointResetPreserveTransferState as 0. If WdfRequestSend fails then
    DsmEventUCXIoctlFailure should be queued directly from this function.
    Completion routine of this function should queue a DsmEventUCXIoctlFailure
    or DsmEventUCXIoctlSuccess event to DSM. Completion routine of this
    function should also store the NTSTATUS at a particular place in the
    device context.

Returns:

    VOID

--*/
{
    NTSTATUS                status;
    WDFREQUEST              request;
    WDF_REQUEST_PARAMETERS  parameters;
    PENDPOINT_RESET         endpointReset;
    PPIPE_HANDLE            pipeHandle;

    TRY {

        request = DeviceContext->CurrentClientRequest;

        WDF_REQUEST_PARAMETERS_INIT(&parameters);
        WdfRequestGetParameters(request, &parameters);

        endpointReset = (PENDPOINT_RESET) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory,
                                                             NULL);

        RtlZeroMemory(endpointReset, sizeof(*endpointReset));

        pipeHandle = DeviceContext->CurrentTargetPipe;

        if (pipeHandle == NULL) {
            DeviceContext->LastOperationUsbdStatus = USBD_STATUS_INVALID_PIPE_HANDLE;
            DeviceContext->LastOperationNtStatus =  USBD_STATUS_To_NTSTATUS(USBD_STATUS_INVALID_PIPE_HANDLE);

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Invalid Pipe Handle!");

            HUBSM_AddDsmEvent(DeviceContext, DsmEventUCXIoctlFailure);
            LEAVE;
        }

        endpointReset->Size = sizeof(*endpointReset);
        endpointReset->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        endpointReset->UsbDevice = DeviceContext->UsbDevice;
        endpointReset->Endpoint = pipeHandle->UcxEndpoint;
        endpointReset->Flags = 0;

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_ENDPOINT_RESET);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "IOCTL_INTERNAL_USB_ENDPOINT_RESET failed! %!STATUS!",
                       status);
        }

    } FINALLY {

    }
}


EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBUCX_UpdateHubInformationUsingUCXIoctlComplete;

VOID
HUBUCX_UpdateHubInformationUsingUCXIoctlComplete(
    __in
        WDFREQUEST                      WdfRequest,
    __in
        WDFIOTARGET                     Target,
    __in
        PWDF_REQUEST_COMPLETION_PARAMS  CompletionParams,
    __in
        WDFCONTEXT                      Context
    )
/*++

Routine Description:

    Completion routine for IOCTL_INTERNAL_USB_USBDEVICE_HUB_INFO.  Queues
    a HsmEventIoctlSuccess or HsmEventIoctlFailure event to HSM.

Returns:

    VOID

--*/
{
    NTSTATUS                status;
    PHUB_FDO_CONTEXT        hubFdoContext;

    UNREFERENCED_PARAMETER(Target);

    TRY {

        hubFdoContext = (PHUB_FDO_CONTEXT) Context;

        status = CompletionParams->IoStatus.Status;

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "IOCTL_INTERNAL_USB_USBDEVICE_HUB_INFO Completion %!STATUS!",
                       status);
        }

    } FINALLY {

        //
        // Notify HSM of completion status of the IOCTL
        //
        HUBSM_AddHsmEvent(hubFdoContext,
                          NT_SUCCESS(status) ? HsmEventIoctlSuccess : HsmEventIoctlFailure);

        WdfObjectDelete(WdfRequest);
    }
}


VOID
HUBUCX_GetControllerInfo (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Calls UCX interface to get the controller bus information.

Return Value:

    VOID

--*/
{
    HubFdoContext->HubControllerStackInterface.ControllerGetInfo(
        HubFdoContext->HubParentInterface.Hub,
        &HubFdoContext->ControllerInfo);
}


VOID
HUBUCX_UpdateHubInformationUsingUCXIoctl (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Sends IOCTL_INTERNAL_USB_USBDEVICE_HUB_INFO to the root hub PDO (UCX) to
    update information about an external hub as defined in the hub descriptor.

    If WdfRequestSend fails then HsmEventIoctlFailure should be queued
    directly from this function.

    Completion routine of this function should queue a HsmEventIoctlSuccess
    or HsmEventIoctlFailure event to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    WDF_OBJECT_ATTRIBUTES           attributes;
    WDFREQUEST                      request;
    BOOLEAN                         requestSent;
    WDFIOTARGET                     ioTarget;
    WDFMEMORY                       hubInfoMemory;
    PUSBDEVICE_HUB_INFO             hubInfo;
    PUSB20_HUB_CHAR_BITS            usb20Characteristics;
    PUSB_INTERFACE_DESCRIPTOR       interfaceDescriptor;

    TRY {

        status = STATUS_SUCCESS;

        request = NULL;

        ioTarget = HubFdoContext->RootHubIoTarget;

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = HubFdoContext->HubFdoDevice;

        status = WdfRequestCreate(&attributes,
                                  ioTarget,
                                  &request);

        if ( !NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfRequestCreate Failed %!STATUS!",
                       status);
            LEAVE;
        }

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = request;

        status = WdfMemoryCreate(&attributes,
                                 NonPagedPool,
                                 USBHUB3_HUB_TAG,
                                 sizeof(*hubInfo),
                                 &hubInfoMemory,
                                 (PVOID)&hubInfo);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfMemoryCreate Failed %!STATUS!",
                       status);
            LEAVE;
        }

        RtlZeroMemory(hubInfo, sizeof(*hubInfo));

        hubInfo->Size = sizeof(*hubInfo);
        hubInfo->Hub = HubFdoContext->HubParentInterface.Hub;
        hubInfo->UsbDevice = HubFdoContext->HubParentInterface.Hub;
        hubInfo->NumberOfPorts = HubFdoContext->HubInformation.NumberOfPorts;

        //
        // For high-speed hubs determine TT type 
        //
        if (HubFdoContext->HubParentInterface.HubSpeed == UsbHighSpeed) {
            
            INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &HubFdoContext->HubInformation.HubFlags, IsTtHub);
            
            hubInfo->NumberOfTTs = 1;

            //
            // Check if hub supports a multi-TT interface
            //
            interfaceDescriptor =
                HUBDESC_ParseConfigurationDescriptor(HubFdoContext->ConfigurationDescriptor,
                                                     (PVOID)HubFdoContext->ConfigurationDescriptor,
                                                     -1,
                                                     -1,
                                                     USB_DEVICE_CLASS_HUB,
                                                     -1,
                                                     MULTI_TT_HUB_PROTOCOL,
                                                     NULL,
                                                     HubFdoContext->IfrLog);

            if (interfaceDescriptor != NULL) {
                //
                // Found multi-TT interface
                //
                INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &HubFdoContext->HubInformation.HubFlags, IsMultiTtHub);
                hubInfo->NumberOfTTs = HubFdoContext->HubInformation.NumberOfPorts;
            }
            
            usb20Characteristics = (PUSB20_HUB_CHAR_BITS)
                &HubFdoContext->HubDescriptor.Hub20Descriptor.wHubCharacteristics;

            hubInfo->TTThinkTime = usb20Characteristics->TT_ThinkTime;
        }

        status =
            WdfIoTargetFormatRequestForInternalIoctlOthers(ioTarget,
                                                           request,
                                                           IOCTL_INTERNAL_USB_USBDEVICE_HUB_INFO,
                                                           hubInfoMemory,
                                                           NULL,
                                                           NULL,
                                                           NULL,
                                                           NULL,
                                                           NULL);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Formatting Request for IOCTL_INTERNAL_USB_USBDEVICE_HUB_INFO Failed %!STATUS!",
                       status);
            LEAVE;
        }

        WdfRequestSetCompletionRoutine(request,
                                       HUBUCX_UpdateHubInformationUsingUCXIoctlComplete,
                                       HubFdoContext);

        requestSent = WdfRequestSend(request, ioTarget, NULL);

        if (requestSent == FALSE) {

            status = WdfRequestGetStatus(request);

            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "IOCTL_INTERNAL_USB_USBDEVICE_HUB_INFO failed! %!STATUS!", status);

            if (request != NULL) {
                WdfObjectDelete(request);
            }

            HUBSM_AddHsmEvent(HubFdoContext, HsmEventIoctlFailure);
        }

    }
}


NTSTATUS
HUBUCX_GetControllerName (
    __in
        PHUB_FDO_CONTEXT    FdoContext,
    __in
        PUSB_HUB_NAME       HubNameBuffer,
    __in
        ULONG               HubNameBufferLength
    )
/*++

Routine Description:

    Fetches the controller symbolic name through the USBD bus interface

    This function returns the controllers Unicode symbolic device name.

Parameters:


    A pointer to a structure to receive the symbolic name.  ActualLength is set
    to the size of the controller name even if there is not enough room to return
    the whole name.

    typedef struct _USB_HUB_NAME {
        ULONG ActualLength;
        WCHAR HubName[1];
    } USB_HUB_NAME, *PUSB_HUB_NAME;

    ioStackLocation->Parameters.Others.Argument2
    The size of the buffer passed in Argument1 in bytes

IRQL: Any

Return Value:

    NTSTATUS

--*/
{
    PUSB_BUS_INFORMATION_LEVEL_1    level_1;
    ULONG                           lenToCopy;
    ULONG                           level_1_siz;
    ULONG                           actualLength;
    NTSTATUS                        status;

    TRY {

        level_1_siz = sizeof(USB_BUS_INFORMATION_LEVEL_1);
        level_1 = ExAllocatePoolWithTag(PagedPool, level_1_siz, USBHUB3_HUB_TAG);

        if (level_1 == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        status =
            FdoContext->UsbdBusInterface.QueryBusInformation(FdoContext->UsbdBusInterface.BusContext,
                                                             1,
                                                             level_1,
                                                             &level_1_siz,
                                                             &actualLength);

        if (status == STATUS_BUFFER_TOO_SMALL) {

            //
            // Ask for the rest
            //
            level_1_siz = actualLength;
            ExFreePoolWithTag(level_1, USBHUB3_HUB_TAG);

            level_1 = NULL;
            level_1 = ExAllocatePoolWithTag(PagedPool, level_1_siz, USBHUB3_HUB_TAG);

            if (level_1 == NULL) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            status =
                FdoContext->UsbdBusInterface.QueryBusInformation(FdoContext->UsbdBusInterface.BusContext,
                                                                 1,
                                                                 level_1,
                                                                 &level_1_siz,
                                                                 &actualLength);
        }

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // Copied from Win7 stack
        // Not sure if BufferLength includes size of the ActualLength
        // field, we will assume it does
        //
        if (HubNameBufferLength < sizeof(HubNameBuffer->ActualLength)) {
            status = STATUS_BUFFER_TOO_SMALL;
            LEAVE;
        }

        HubNameBuffer->ActualLength = level_1->ControllerNameLength;

        if ((HubNameBufferLength - sizeof(HubNameBuffer->ActualLength))
            >= level_1->ControllerNameLength) {
            lenToCopy = level_1->ControllerNameLength;
        } else {
            lenToCopy = HubNameBufferLength - sizeof(HubNameBuffer->ActualLength);
        }
        RtlCopyMemory(&HubNameBuffer->HubName[0],
                      &level_1->ControllerNameUnicodeString[0],
                      lenToCopy);

    } FINALLY {

        if (level_1) {
            ExFreePoolWithTag(level_1, USBHUB3_HUB_TAG);
        }
    }

    return status;
}


NTSTATUS
HUBUCX_GetDeviceBusInfo(
    __in
        PHUB_FDO_CONTEXT        FdoContext,
    __in
        UCXUSBDEVICE            DeviceHandle,
    __in
        PUSB_BUS_NOTIFICATION   UserBuffer,
    __in
        ULONG                   UserBufferLength
    )
/*++

Routine Description:

  Fetches the PDO relative bus information through the USB bus interface

    typedef struct _USB_BUS_NOTIFICATION {
        USB_NOTIFICATION_TYPE NotificationType;
        ULONG TotalBandwidth;
        ULONG ConsumedBandwidth;
        ULONG ControllerNameLength;
    } USB_BUS_NOTIFICATION, *PUSB_BUS_NOTIFICATION;


Return Value:

    NTSTATUS

--*/
{
    PUSB_BUS_INFORMATION_LEVEL_1    level_1;
    ULONG                           level_1_siz;
    ULONG                           actualLength;
    NTSTATUS                        status;

    TRY {

        level_1 = NULL;

        if (UserBufferLength < sizeof(struct _USB_BUS_NOTIFICATION)) {
            status = STATUS_BUFFER_TOO_SMALL;
            LEAVE;

        }

        level_1_siz = sizeof(USB_BUS_INFORMATION_LEVEL_1);
        level_1 = ExAllocatePoolWithTag(PagedPool,
                                        level_1_siz,
                                        USBHUB3_HUB_TAG);

        if (level_1 == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        status = FdoContext->UsbdBusInterface.QueryBusInformation(DeviceHandle,
                                                                  1,
                                                                  level_1,
                                                                  &level_1_siz,
                                                                  &actualLength);

        if (status == STATUS_BUFFER_TOO_SMALL) {
            //
            // Ask for the rest
            //
            level_1_siz = actualLength;
            ExFreePoolWithTag(level_1, USBHUB3_HUB_TAG);
            level_1 = NULL;
            level_1 = ExAllocatePoolWithTag(PagedPool,
                                            level_1_siz,
                                            USBHUB3_HUB_TAG);
            if (level_1 == NULL) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            status = FdoContext->UsbdBusInterface.QueryBusInformation(DeviceHandle,
                                                                      1,
                                                                      level_1,
                                                                      &level_1_siz,
                                                                      &actualLength);

        }

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        UserBuffer->TotalBandwidth = level_1->TotalBandwidth;
        UserBuffer->ConsumedBandwidth = level_1->ConsumedBandwidth;

        //
        // Length of the UNICODE symbolic name (in bytes) for the controller
        // that this device is attached to.
        // not including NULL
        //
        UserBuffer->ControllerNameLength = level_1->ControllerNameLength;

    } FINALLY {

        if (level_1) {
            ExFreePoolWithTag(level_1, USBHUB3_HUB_TAG);
        }
    }

    return status;
}


VOID
HUBUCX_DisableEndpointsInPendingDisableListUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Deconfigures in UCX the endpoints for an existing interface that is 
    being replaced by a SET_INTERFACE command.  

Returns:

    VOID

--*/
{

    TRY {


        DeviceContext->EndpointsToRemainUnchangedCount = 0;
        
        if (DeviceContext->EndpointsToBeDisabledCount != 0) {

            DeviceContext->EndpointsToBeEnabledCount = 0;

            HUBUCX_EnableDisableEndpointsUsingUCXIoctl(DeviceContext);
        
        } else {
            
            HUBSM_AddDsmEvent(DeviceContext, DsmEventUCXIoctlSuccess);
        }

    } FINALLY {

    }
}

VOID
HUBUCX_MarkEndpointsToBeEnabledAsDisabled (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Deconfigures in UCX the endpoints for an existing interface that is 
    being replaced by a SET_INTERFACE command.  

Returns:

    VOID

--*/
{

    TRY {

        //
        // For a device reset request, any endpoints that were pending enable 
        // are disabled at this point.
        //
        HUBUCX_UpdateConfigurationEndpointStates(
                DeviceContext->CurrentDeviceConfiguration,
                UcxEndpointStatePendingEnable,
                UcxEndpointStateDisabled);


    } FINALLY {

    }
}

VOID
HUBUCX_AbortDeviceIoUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Purges the Io for all endpoints for the USB device 
    using IOCTL_INTERNAL_USB_USBDEVICE_ABORTIO.

    Completion routine of this function should queue a DsmEventUCXIoctlSuccess
    event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSBDEVICE_ABORTIO              deviceAbortIo;

    TRY {

        deviceAbortIo =
            (PUSBDEVICE_ABORTIO) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory, NULL);

        RtlZeroMemory(deviceAbortIo, sizeof(*deviceAbortIo));

        deviceAbortIo->Size = sizeof(*deviceAbortIo);
        deviceAbortIo->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        deviceAbortIo->UsbDevice = DeviceContext->UsbDevice;

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_USBDEVICE_ABORTIO);

    } FINALLY {
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "IOCTL_INTERNAL_USB_USBDEVICE_ABORTIO failed! %!STATUS!",
                       status);
        }
    }
}

VOID
HUBUCX_PurgeDeviceIoUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Purges the Io for all endpoints for the USB device 
    using IOCTL_INTERNAL_USB_USBDEVICE_PURGEIO.

    Completion routine of this function should queue a DsmEventUCXIoctlSuccess
    event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSBDEVICE_PURGEIO              devicePurgeIo;

    TRY {

        devicePurgeIo =
            (PUSBDEVICE_PURGEIO) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory, NULL);

        RtlZeroMemory(devicePurgeIo, sizeof(*devicePurgeIo));

        devicePurgeIo->Size = sizeof(*devicePurgeIo);
        devicePurgeIo->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        devicePurgeIo->UsbDevice = DeviceContext->UsbDevice;
        devicePurgeIo->OnSuspend = FALSE;

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_USBDEVICE_PURGEIO);

    } FINALLY {
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "IOCTL_INTERNAL_USB_USBDEVICE_PURGEIO failed! %!STATUS!",
                       status);
        }
    }
}

VOID
HUBUCX_PurgeDeviceIoForSuspendUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Purges the Io for all endpoints for the USB device 
    using IOCTL_INTERNAL_USB_USBDEVICE_PURGEIO.

    Completion routine of this function should queue a DsmEventUCXIoctlSuccess
    event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSBDEVICE_PURGEIO              devicePurgeIo;

    TRY {

        devicePurgeIo =
            (PUSBDEVICE_PURGEIO) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory, NULL);

        RtlZeroMemory(devicePurgeIo, sizeof(*devicePurgeIo));

        devicePurgeIo->Size = sizeof(*devicePurgeIo);
        devicePurgeIo->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        devicePurgeIo->UsbDevice = DeviceContext->UsbDevice;
        devicePurgeIo->OnSuspend = TRUE;

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_USBDEVICE_PURGEIO);

    } FINALLY {
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "IOCTL_INTERNAL_USB_USBDEVICE_PURGEIO failed! %!STATUS!",
                       status);
        }
    }
}

VOID
HUBUCX_PurgeDeviceTreeIoUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Purges the Io for all endpoints for the USB device 
    using IOCTL_INTERNAL_USB_USBDEVICE_TREE_PURGEIO.If 
    it is a hub, it will recursively purge IO for child
    devices too

    Completion routine of this function should queue a DsmEventUCXIoctlSuccess
    event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSBDEVICE_PURGEIO              devicePurgeIo;

    TRY {

        devicePurgeIo =
            (PUSBDEVICE_PURGEIO) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory, NULL);

        RtlZeroMemory(devicePurgeIo, sizeof(*devicePurgeIo));

        devicePurgeIo->Size = sizeof(*devicePurgeIo);
        devicePurgeIo->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        devicePurgeIo->UsbDevice = DeviceContext->UsbDevice;

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_USBDEVICE_TREE_PURGEIO);

    } FINALLY {
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "IOCTL_INTERNAL_USB_USBDEVICE_TREE_PURGEIO failed! %!STATUS!",
                       status);
        }
    }
}


VOID
HUBUCX_StartDeviceIoUsingUCXIoctl (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Starts the Io for all endpoints for the USB device 
    using IOCTL_INTERNAL_USB_USBDEVICE_STARTIO.

    Completion routine of this function should queue a DsmEventUCXIoctlSuccess
    event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSBDEVICE_STARTIO              deviceStartIo;

    TRY {

        deviceStartIo =
            (PUSBDEVICE_STARTIO) WdfMemoryGetBuffer(DeviceContext->UcxIoctlRequestMemory, NULL);

        RtlZeroMemory(deviceStartIo, sizeof(*deviceStartIo));

        deviceStartIo->Size = sizeof(*deviceStartIo);
        deviceStartIo->Hub = DeviceContext->HubFdoContext->HubParentInterface.Hub;
        deviceStartIo->UsbDevice = DeviceContext->UsbDevice;

        status = HUBUCX_SubmitUcxIoctl(DeviceContext, IOCTL_INTERNAL_USB_USBDEVICE_STARTIO);

    } FINALLY {
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "UCX IOCTL_INTERNAL_USB_USBDEVICE_STARTIO failed %!STATUS!",
                       status);
        }
    }
}

EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBUCX_ClientRequestCompleteFromUCX;

VOID
HUBUCX_ClientRequestCompleteFromUCX(
    __in
        WDFREQUEST                      WdfRequest,
    __in
        WDFIOTARGET                     Target,
    __in
        PWDF_REQUEST_COMPLETION_PARAMS  CompletionParams,
    __in
        WDFCONTEXT                      Context
    )
/*++

Routine Description:

    A completion routine for completing a Client request that was forwarded to UCX. 
    Queues a DsmEventClientRequestCompleteFromUCX event to DSM.

Returns:

    VOID

--*/
{
    PDEVICE_CONTEXT             deviceContext;

    UNREFERENCED_PARAMETER(Target);

    TRY {

        deviceContext = (PDEVICE_CONTEXT) Context;

        WdfRequestComplete(WdfRequest, CompletionParams->IoStatus.Status);
        
        HUBSM_AddDsmEvent(deviceContext,
                          DsmEventUCXClientRequestComplete);

    } FINALLY {

    }
}

VOID
HUBUCX_ForwardRequestToUCX (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Forwards a request to UCX.
    
Returns:

    VOID

--*/
{
    WDFREQUEST                      request;
    WDFIOTARGET                     ioTarget;
    BOOLEAN                         requestSent;
    NTSTATUS                        status;

    TRY {

        request = DeviceContext->CurrentClientRequest;
        ioTarget = DeviceContext->HubFdoContext->RootHubIoTarget;    

        WdfRequestFormatRequestUsingCurrentType(request);

        WdfRequestSetCompletionRoutine(request,
                                       HUBUCX_ClientRequestCompleteFromUCX,
                                       DeviceContext);

        requestSent = WdfRequestSend(request,
                                     ioTarget,
                                     NULL);

        if (requestSent == FALSE) {
            status = WdfRequestGetStatus(request);
            WdfRequestComplete(request, status);
            HUBSM_AddDsmEvent(DeviceContext, DsmEventUCXClientRequestComplete);
        }

    } FINALLY {

    }
}

VOID
HUBUCX_DisconnectDeviceInUCX (
    __in
        PPORT_CONTEXT           PortContext
    )
/*++

Routine Description:

    Notifies UCX that the device has been disconnected. So that UCX
    will start failing all IO on this device. If the device is a hub,
    then UCX will start failing all IO on all of its children as well

Returns:


--*/
{
    PHUB_FDO_CONTEXT                hubFdoContext;
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;

    TRY {

        hubFdoContext = PortContext->HubFdoContext;
        hubControllerStackInterface = &hubFdoContext->HubControllerStackInterface;
        
        NT_ASSERT(hubControllerStackInterface->UsbDeviceDisconnect != NULL);    


        hubControllerStackInterface->UsbDeviceDisconnect(hubFdoContext->HubParentInterface.Hub,
                                                         PortContext->PortInformation.PortNumber);

    } FINALLY {

    }

}

BOOLEAN
HUBUCX_CheckIfHubIsDisconnected(
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:
    
    Checks if the hub or one of its parents have been
    reported disconnected to UCX by hub
    
Returns:

--*/
{
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;

    hubControllerStackInterface = &HubFdoContext->HubControllerStackInterface;  
    
    NT_ASSERT(hubControllerStackInterface->IsDeviceDisconnected != NULL);               
    
    return hubControllerStackInterface->IsDeviceDisconnected(HubFdoContext->HubParentInterface.Hub);
}

NTSTATUS
HUBUCX_NotifyForwardProgress(
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:
    
    Tells UCX that we need to be enabled for forward progress
    
Returns:

--*/
{

    NTSTATUS                        status;
    WDFIOTARGET                     ioTarget;
    BYTE                            fpInfoBuffer[SIZE_OF_USB_FORWARD_PROGRESS_INFO(1)];
    PUSB_FORWARD_PROGRESS_INFO      fpInfo;
    WDF_MEMORY_DESCRIPTOR           memDescriptor;

    TRY {

        status = STATUS_SUCCESS;        
        ioTarget = HubFdoContext->RootHubIoTarget;
        fpInfo = (PUSB_FORWARD_PROGRESS_INFO)fpInfoBuffer;

        RtlZeroMemory(fpInfo, sizeof(fpInfoBuffer));

        fpInfo->Header.Version = USB_FORWARD_PROGRESS_INFO_VERSION_1;
        fpInfo->Header.Size = sizeof(fpInfoBuffer);
        fpInfo->ControlPipeMaxForwardProgressTransferSize = 4096; 
        fpInfo->NumberOfPipes = 1;
        fpInfo->DeviceHandle = HubFdoContext->HubParentInterface.Hub;

        if (HubFdoContext->HubParentInterface.HubDepth == 0) {
            fpInfo->Pipes[0].MaxForwardProgressTransferSize = 
                HubFdoContext->StatusChangeInterruptRequest.MaxStatusChangeBitMapSize;
        } else {   
            fpInfo->Pipes[0].MaxForwardProgressTransferSize = 
                HubFdoContext->HubInformation.InterruptPipeMaxPacketSize;
        }
        fpInfo->Pipes[0].PipeHandle = 
            HubFdoContext->StatusChangeInterruptRequest.UsbdInterruptPipeHandle;

        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memDescriptor,
                                          (PVOID)fpInfo,
                                          sizeof(fpInfoBuffer));
        status =
            WdfIoTargetSendInternalIoctlOthersSynchronously(ioTarget,
                                                            NULL,
                                                            IOCTL_INTERNAL_USB_NOTIFY_FORWARD_PROGRESS,
                                                            &memDescriptor,
                                                            NULL,
                                                            NULL,
                                                            NULL,
                                                            NULL);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "IOCTL_INTERNAL_USB_NOTIFY_FORWARD_PROGRESS Failed %!STATUS!",
                       status);
            LEAVE;
        }


    } FINALLY {
        
    }

    return status;
}

DSM_EVENT
HUBUCX_CheckIfEndpointsNeedToBeDisabledOrEnabled (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if there are endpoints waiting to get enabled
    or disabled

Returns:

    DsmEventYes, DsmEventNo

--*/
{
    DSM_EVENT                       dsmEvent;

    TRY {

        if ((DeviceContext->EndpointsToBeDisabledCount > 0 ) ||
            (DeviceContext->EndpointsToBeEnabledCount > 0)) {
            dsmEvent = DsmEventYes;
        } else {
            dsmEvent = DsmEventNo;
        }

    } FINALLY {

    }

    return dsmEvent;
}

DSM_EVENT
HUBUCX_CheckIfEndpointsNeedToBeDisabledOnConfigFailure (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Checks if there are endpoints waiting to get enabled
    or disabled on failure in the configure endpoints
    path. We will simply change the state of the endpoints
    that were waiting to be enabled but we still need to
    send a command to the controller if there are any
    endpoints that are waiting to be disabled

Returns:

    DsmEventYes, DsmEventNo

--*/
{
    DSM_EVENT                       dsmEvent;

    TRY {
        
        if ((DeviceContext->EndpointsToBeEnabledCount > 0 ) &&
            (DeviceContext->CurrentDeviceConfiguration != NULL)) {

            //
            // Pending enable should now be considered disabled
            //
            HUBUCX_UpdateConfigurationEndpointStates(
                    DeviceContext->CurrentDeviceConfiguration,
                    UcxEndpointStatePendingEnable,
                    UcxEndpointStateDisabled);

            RtlCopyMemory(&DeviceContext->EndpointsToRemainUnchanged[DeviceContext->EndpointsToRemainUnchangedCount],
                          DeviceContext->EndpointsToBeEnabled,
                          DeviceContext->EndpointsToBeEnabledCount * sizeof(UCXENDPOINT));

            DeviceContext->EndpointsToRemainUnchangedCount += DeviceContext->EndpointsToBeEnabledCount;
            DeviceContext->EndpointsToBeEnabledCount = 0;

        }

        if (DeviceContext->EndpointsToBeDisabledCount > 0 ) {
            dsmEvent = DsmEventYes;
        } else {
            dsmEvent = DsmEventNo;
        }           

    } FINALLY {

    }

    return dsmEvent;
}


