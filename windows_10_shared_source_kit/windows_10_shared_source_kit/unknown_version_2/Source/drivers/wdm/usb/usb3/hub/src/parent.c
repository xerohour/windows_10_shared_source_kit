/*++
Copyright (c) Microsoft Corporation

Module Name:

    parent.c

Abstract:

    IOCTLs and bus interface calls to the hub's parent device

--*/

#include "pch.h"
#include "parent.tmh"



EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBPARENT_SetHubConfigurationComplete;

VOID
HUBPARENT_SetHubConfigurationComplete(
    __in WDFREQUEST         WdfRequest,
    __in WDFIOTARGET        Target,
    __in PWDF_REQUEST_COMPLETION_PARAMS
                            CompletionParams,
    __in WDFCONTEXT         Context
    )
/*++

Routine Description:

    Completion routine for hub set configuration request.

Returns:

    VOID

--*/
{
    NTSTATUS                status;
    PHUB_FDO_CONTEXT        hubFdoContext;
    USBD_STATUS             urbStatus;
    PURB                    urb;
    PUSBD_INTERFACE_INFORMATION
                            interfaceInformation;

    UNREFERENCED_PARAMETER(Target);

    TRY {

        hubFdoContext = GetHubFdoContext(WdfIoTargetGetDevice(Target));
        urb = (PURB) Context;
        
        urbStatus = urb->UrbSelectConfiguration.Hdr.Status;

        interfaceInformation = &urb->UrbSelectConfiguration.Interface;

        status = CompletionParams->IoStatus.Status;

        if (NT_SUCCESS(status)) {

            //
            // Save the pipe handle for the Interrupt pipe
            //
            hubFdoContext->StatusChangeInterruptRequest.UsbdInterruptPipeHandle =
                interfaceInformation->Pipes[0].PipeHandle;

            //
            // Save the configuration handle for the hub
            //
            hubFdoContext->HubInformation.HubConfigurationHandle =
                urb->UrbSelectConfiguration.ConfigurationHandle;

            //
            // Save the max packet size for Interrupt pipe,
            // that will used to calculate the transfer size
            //                                                   
            hubFdoContext->HubInformation.InterruptPipeMaxPacketSize = 
                interfaceInformation->Pipes[0].MaximumPacketSize;
            
            //
            // For SuperSpeed hubs, the only allowed value for InterruptPipeMaxPacketSize
            // is 2 but for < 3.0 hubs, it could be any value. We are allowing a value of 1 
            // for SuperSpeed hubs for now as a workaround for certain hubs.
            //
            if (hubFdoContext->HubInfoFromParent.DeviceDescriptor.bcdUSB >= 0x300) {

                if (hubFdoContext->HubInformation.InterruptPipeMaxPacketSize > 2) {
                
                    NT_ASSERTMSG("SuperSpeed hub has a value of greater than 2 as its InterruptPipeMaxPacketSize",
                                 FALSE);

                    status = STATUS_INVALID_HW_PROFILE;
                }

                if (hubFdoContext->HubInformation.InterruptPipeMaxPacketSize != 2) {
            
                    DbgTraceTo(hubFdoContext->IfrLog,
                               TL_ERROR,
                               Hub,
                               "HW_COMPLIANCE:InterruptPipeMaxPacketSize does not equal 2 for SS hub's interrupt endpoint - %d",
                               hubFdoContext->HubInformation.InterruptPipeMaxPacketSize);
                }
            }
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Set Hub Configuration Failed! - %!STATUS!, URB status - %x",
                       status,
                       urbStatus);
        }

        WdfObjectDelete(WdfRequest);
        ExFreePool(urb);

        HUBSM_AddHsmEvent(hubFdoContext,
                          NT_SUCCESS(status) ? HsmEventConfigurationSuccess :
                                               HsmEventConfigurationFailure);
    }
}


VOID
HUBPARENT_SetHubConfiguration (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Parses the hub's configuration descriptor and selects the appropriate
    configuration for the hub, and sends a select configuration URB to the parent.

    Based on Windows 7 USB hub driver

    The completion routine for this URB should queue HsmEventConfigurationSuccess
    or HsmEventConfigurationFailure to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PURB                            urb;
    PUSB_INTERFACE_DESCRIPTOR       interfaceDescriptor;
    PUSB_CONFIGURATION_DESCRIPTOR   configDesc;
    WDFREQUEST                      wdfRequest;
    WDFIOTARGET                     wdfIoTarget;
    WDF_OBJECT_ATTRIBUTES           attributes;
    BOOLEAN                         requestSent;
    USBD_INTERFACE_LIST_ENTRY       interfaceList[2];  
    IO_STACK_LOCATION               ioStackLocation;
    
    TRY {

        configDesc = HubFdoContext->ConfigurationDescriptor;
        
        interfaceDescriptor = NULL;
        urb = NULL;
        status = STATUS_SUCCESS;
        wdfRequest = NULL;

        switch (HubFdoContext->HubParentInterface.HubSpeed) {
        case UsbHighSpeed:

            //
            // Try to select the multi-TT interface, if it exists
            //
            interfaceDescriptor =
                HUBDESC_ParseConfigurationDescriptor(configDesc,
                                                     (PVOID)configDesc,
                                                     -1,
                                                     -1,
                                                     USB_DEVICE_CLASS_HUB,
                                                     -1,
                                                     MULTI_TT_HUB_PROTOCOL,
                                                     NULL,
                                                     HubFdoContext->IfrLog);

            if (interfaceDescriptor != NULL) {
                //
                // Parsing for multi-TT interface has already occurred once when
                // the UCX hub info was updated.  Assert if a multi-TT interface
                // was not found on the previous parse.
                //
                NT_ASSERT(HubFdoContext->HubInformation.HubFlags.IsMultiTtHub == 1);
                break;
            }

            NT_ASSERT(HubFdoContext->HubInformation.HubFlags.IsMultiTtHub != 1);

            //
            // Failed to find Multi-TT interface, fall through to
            // single-TT/no-TT interface
            //
        case UsbFullSpeed:
        case UsbSuperSpeed:

            //
            // Protocol may be full speed or single TT. Check for single TT hub.
            //
            interfaceDescriptor =
                HUBDESC_ParseConfigurationDescriptor(configDesc,
                                                     (PVOID)configDesc,
                                                     -1,
                                                     -1,
                                                     USB_DEVICE_CLASS_HUB,
                                                     -1,
                                                     SINGLE_TT_HUB_PROTOCOL,
                                                     NULL,
                                                     HubFdoContext->IfrLog);

            if (interfaceDescriptor != NULL) {
                break;
            }

            //
            // locate full speed protocol
            //
            interfaceDescriptor =
                HUBDESC_ParseConfigurationDescriptor(configDesc,
                                                     (PVOID)configDesc,
                                                     -1,
                                                     -1,
                                                     USB_DEVICE_CLASS_HUB,
                                                     -1,
                                                     FULL_SPEED_HUB_PROTOCOL,
                                                     NULL,
                                                     HubFdoContext->IfrLog);

            if (interfaceDescriptor != NULL) {
                break;
            }

            //
            // COMPAT: Preserve Windows 7 Hub Driver Behavior
            //         Use any interface protocol we can find
            //
            interfaceDescriptor =
                HUBDESC_ParseConfigurationDescriptor(configDesc,
                                                     (PVOID)configDesc,
                                                     -1,
                                                     -1,
                                                     USB_DEVICE_CLASS_HUB,
                                                     -1,
                                                     -1,
                                                     NULL,
                                                     HubFdoContext->IfrLog);
            break;

        default:
            NT_ASSERTMSG("Invalid Hub Type for SELECT_CONFIGURATION", FALSE);
        }

        if (interfaceDescriptor == NULL) {

            //
            // Unable to find an interface for the hub.
            //
            status = STATUS_UNSUCCESSFUL;
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Unable to Find Interface Descriptor for Hub");

            LEAVE;
        }

        if (interfaceDescriptor->bInterfaceClass != USB_DEVICE_CLASS_HUB) {

            //
            // Parsing code must be broken.  It should not return an
            // interface with an incorrect interface class.
            //
            NT_ASSERTMSG("Parsing Error for Hub's Interface Descriptor!", FALSE);

            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        if (interfaceDescriptor->bNumEndpoints == 0) {

            //
            // The Hub Class is required to have the Status Change endpoint.
            // This can be a malfunctioning HUB
            //
            NT_ASSERTMSG("Hub's Interface Descriptor has 0 bNumEndPoints!", FALSE);

            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        interfaceList[0].InterfaceDescriptor = interfaceDescriptor;
        interfaceList[1].InterfaceDescriptor = NULL;
        
        urb = USBD_CreateConfigurationRequestEx(configDesc, &interfaceList[0]);

        if (urb == NULL) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Unable to Create Select Configuration Request");
            LEAVE;
        }

        wdfIoTarget = WdfDeviceGetIoTarget(HubFdoContext->HubFdoDevice);

        
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = HubFdoContext->HubFdoDevice;
        
        status = WdfRequestCreate(&attributes, 
                                  wdfIoTarget,
                                  &wdfRequest);

        if ( !NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfRequestCreate Failed %!STATUS!",
                       status);
            LEAVE;
        }

        RtlZeroMemory(&ioStackLocation, sizeof(ioStackLocation));
        ioStackLocation.MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        ioStackLocation.Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
        ioStackLocation.Parameters.Others.Argument1 = urb;

        WdfRequestWdmFormatUsingStackLocation(wdfRequest, &ioStackLocation);

        WdfRequestSetCompletionRoutine(wdfRequest,
                                       HUBPARENT_SetHubConfigurationComplete,
                                       urb);

        requestSent = WdfRequestSend(wdfRequest, wdfIoTarget, NULL);

        if (requestSent == FALSE) {

            status = WdfRequestGetStatus(wdfRequest);

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Async IOCTL Request Not Sent! %!STATUS!",
                       status);

            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (wdfRequest != NULL) {
                WdfObjectDelete(wdfRequest);
            }

            if (urb) {
                ExFreePool(urb);
            }

            HUBSM_AddHsmEvent(HubFdoContext, HsmEventConfigurationFailure);
        }
    }
}


VOID
HUBPARENT_GetHubSymbolicLinkName (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext,
    __in
        PUNICODE_STRING    HubSymbolicLinkName
    )
/*++

Routine Description:

    Retrieves the hub's symbolic link name string from the parent.

--*/
{
    PHUB_PARENT_INTERFACE   parentInterface;

    TRY {
        parentInterface = &HubFdoContext->HubParentInterface;

        if (parentInterface->GetHubSymbolicLinkName != NULL) {
            parentInterface->GetHubSymbolicLinkName(parentInterface->Context, HubSymbolicLinkName);
        }

    } FINALLY {

    }
}


HSM_EVENT
HUBPARENT_QueryParentIfDeviceWasReset (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Synchronous call to the parent to determine if the hub was reset on
    system resume.

--*/
{
    HSM_EVENT               hsmEvent = HsmEventYes;
    HUB_PARENT_INTERFACE    parentInterface;

    TRY {
        parentInterface = HubFdoContext->HubParentInterface;

        if(parentInterface.WasHubResetOnResume(parentInterface.Context)){
            hsmEvent = HsmEventYes;

            if (parentInterface.WasDeviceProgrammingLostOnResume(parentInterface.Context)) {
                HUBMUX_PropogateDeviceProgrammingLostFlagsToAllDSMs(HubFdoContext);
            }

        } else {
            hsmEvent = HsmEventNo;
        }

    } FINALLY {

    }

    return hsmEvent;
}
EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBPARENT_ResetInterruptPipeComplete;

VOID
HUBPARENT_ResetInterruptPipeComplete(
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

    Completion routine for reset interrupt pipe request.

Returns:

    VOID

--*/
{
    NTSTATUS                    status;
    PHUB_FDO_CONTEXT            hubFdoContext;
    USBD_STATUS                 urbStatus;
    struct _URB_PIPE_REQUEST    *urb;

    UNREFERENCED_PARAMETER(Target);

    TRY {

        status = CompletionParams->IoStatus.Status;

        hubFdoContext = (PHUB_FDO_CONTEXT)Context;

        urb = &hubFdoContext->HubControlRequest.PipeUrb;
        urbStatus = urb->Hdr.Status;

        HUBMISC_RequestReuse(WdfRequest);


    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Reset Interrupt Pipe Request Failed! - %!STATUS!, URB status - %x",
                       status,
                       urbStatus);
            
            //
            // This is not a control transfer but it does involve 
            // a control transfer. We will re-use the control transfer 
            // event instead of inventing a new event just for this 
            // case
            //
            EventWriteUSBHUB3_ETW_EVENT_HUB_CONTROL_TRANSFER_ERROR(
                0,
                hubFdoContext->HubParentInterface.Hub, 
                0,
                urbStatus,
                status);
            
            USB_HW_VERIFIER_HUB_BREAK(hubFdoContext, HubHwVerifierControlTransferFailure);
        }                                                                                 

        HUBSM_AddHsmEvent(hubFdoContext,
                          NT_SUCCESS(status) ? HsmEventResetInterruptPipeSuccess :
                                               HsmEventResetInterruptPipeFailure);
    }
}

VOID
HUBPARENT_ResetInterruptPipe (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:
    
    Sends a URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL request
    down the stack for interrupt point to recover from a transfer
    failure

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    struct _URB_PIPE_REQUEST        *urb;
    IO_STACK_LOCATION               ioStackLocation;
    BOOLEAN                         requestSent;
    WDFREQUEST                      wdfRequest;
    WDFIOTARGET                     wdfIoTarget;

    TRY {

            status = STATUS_SUCCESS;
            urb = &HubFdoContext->HubControlRequest.PipeUrb;

            RtlZeroMemory(urb, sizeof(struct _URB_PIPE_REQUEST));
            urb->Hdr.UsbdDeviceHandle = HubFdoContext->HubParentInterface.Hub;
            urb->Hdr.Length = sizeof(struct _URB_PIPE_REQUEST);
            urb->Hdr.Function = URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL;
            urb->PipeHandle = HubFdoContext->StatusChangeInterruptRequest.UsbdInterruptPipeHandle;

            wdfIoTarget = WdfDeviceGetIoTarget(HubFdoContext->HubFdoDevice);
            wdfRequest = HubFdoContext->HubControlRequest.WdfRequest;

            RtlZeroMemory(&ioStackLocation,  sizeof(ioStackLocation));
            ioStackLocation.MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
            ioStackLocation.Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
            ioStackLocation.Parameters.Others.Argument1 = urb;

            WdfRequestWdmFormatUsingStackLocation(wdfRequest, &ioStackLocation);

            WdfRequestSetCompletionRoutine(wdfRequest,
                                           HUBPARENT_ResetInterruptPipeComplete,
                                           HubFdoContext);

            requestSent = WdfRequestSend(wdfRequest,
                                         wdfIoTarget,
                                         NULL);

            if (requestSent == FALSE) {

                DbgTraceTo(HubFdoContext->IfrLog, TL_ERROR, Hub, "Reset Interrupt Pipe Request Not Sent!");

                status = WdfRequestGetStatus(wdfRequest);
                LEAVE;
            }


    } FINALLY {

        if (!NT_SUCCESS(status)) {
            
            //
            // Notify HSM of failure of to acknowledge change
            //
            HUBSM_AddHsmEvent(HubFdoContext, HsmEventResetInterruptPipeFailure);
        }
    }
}

EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBPARENT_ResetHubComplete;

VOID
HUBPARENT_ResetHubComplete(
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

    Completion routine for hub reset request.

Returns:

    VOID

--*/
{
    NTSTATUS                    status;
    PHUB_FDO_CONTEXT            hubFdoContext;
    HSM_EVENT                   hsmEvent;


    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(Context);

    TRY {

        hubFdoContext = GetHubFdoContext(WdfIoTargetGetDevice(Target));
        
        status = CompletionParams->IoStatus.Status;

        EventWriteUSBHUB3_ETW_EVENT_HUB_RESET_REQUEST_COMPLETE(
            0,
            hubFdoContext->HubParentInterface.Hub,
            status);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Reset Hub failed with status %!STATUS!",
                       status);

            LEAVE;
        }         

        if (hubFdoContext->ResetFlags.ControllerProgrammingLost == 1) {

            HUBMUX_PropogateDeviceProgrammingLostFlagsToAllDSMs(hubFdoContext);
        }


    } FINALLY {

        WdfObjectDelete(WdfRequest);

        if (NT_SUCCESS(status)) {
            hsmEvent = HsmEventResetHubComplete;
        } else if(status == STATUS_NO_SUCH_DEVICE) {
            hsmEvent = HsmEventResetHubFailedOnRemoval;
        } else {
            hsmEvent = HsmEventResetHubFailed;
        }
        HUBSM_AddHsmEvent(hubFdoContext,
                          hsmEvent);
    }
}


VOID
HUBPARENT_ResetHub (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Sends the reset Ioctl to the parent asynchronously.

    The completion routine for the Ioctl should queue ResetHubFailed
    or ResetHubComplete to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                status;
    WDFREQUEST              wdfRequest;
    WDFIOTARGET             wdfIoTarget;
    WDF_OBJECT_ATTRIBUTES   attributes;
    BOOLEAN                 requestSent;
    IO_STACK_LOCATION       ioStackLocation;        


   
    TRY {
        wdfIoTarget = WdfDeviceGetIoTarget(HubFdoContext->HubFdoDevice);
        
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = (HubFdoContext->HubFdoDevice);
        
        EventWriteUSBHUB3_ETW_EVENT_HUB_RESET_REQUEST_START(
            0,
            HubFdoContext->HubParentInterface.Hub);

        status = WdfRequestCreate(&attributes, wdfIoTarget, &wdfRequest);

        if ( !NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfRequestCreate Failed %!STATUS!",
                       status);
            LEAVE;
        }       

        RtlZeroMemory(&HubFdoContext->ResetFlags,
                      sizeof(HubFdoContext->ResetFlags));

        RtlZeroMemory(&ioStackLocation, 
                      sizeof(ioStackLocation));
        ioStackLocation.MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        ioStackLocation.Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_RESET_PORT_ASYNC;
        ioStackLocation.Parameters.Others.Argument1 = &HubFdoContext->ResetFlags;

        WdfRequestWdmFormatUsingStackLocation(wdfRequest, 
                                              &ioStackLocation);

        WdfRequestSetCompletionRoutine(wdfRequest,
                                       HUBPARENT_ResetHubComplete,
                                       NULL);

        requestSent = WdfRequestSend(wdfRequest,
                                     wdfIoTarget,
                                     NULL);

        if (requestSent == FALSE) {
            status = WdfRequestGetStatus(wdfRequest);

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "IOCTL_INTERNAL_USB_RESET_PORT Request Not Sent %!STATUS!",
                       status);

            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (wdfRequest != NULL) {
                WdfObjectDelete(wdfRequest);
            }

            HUBSM_AddHsmEvent(HubFdoContext, HsmEventResetHubFailed);
        }
    }
}


EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBPARENT_GetInfoFromParentUsingParentIoctlComplete;

_Use_decl_annotations_
VOID
HUBPARENT_GetInfoFromParentUsingParentIoctlComplete(
    WDFREQUEST                      WdfRequest,
    WDFIOTARGET                     Target,
    PWDF_REQUEST_COMPLETION_PARAMS  CompletionParams,
    WDFCONTEXT                      Context
    )
/*++

Routine Description:

    Completion routine for hub set configuration request.

Returns:

    VOID

--*/
{
    NTSTATUS                    status;
    PHUB_FDO_CONTEXT            hubFdoContext;
    WDF_IO_TARGET_OPEN_PARAMS   openParams;

    UNREFERENCED_PARAMETER(Context);

    TRY {

        hubFdoContext = GetHubFdoContext(WdfIoTargetGetDevice(Target));

        status = CompletionParams->IoStatus.Status;

        if (NT_SUCCESS(status)) {
            
            //
            // Create an I/O target for the root hub PDO
            //
            WDF_IO_TARGET_OPEN_PARAMS_INIT_EXISTING_DEVICE(&openParams, 
                                                           hubFdoContext->HubInfoFromParent.IoTarget);
    
            status = WdfIoTargetCreate(hubFdoContext->HubFdoDevice,
                                       WDF_NO_OBJECT_ATTRIBUTES,
                                       &hubFdoContext->RootHubIoTarget);
            
            if (!NT_SUCCESS (status)) {
                LEAVE;
            }
            
            status = WdfIoTargetOpen(hubFdoContext->RootHubIoTarget,
                                     &openParams);

            if (!NT_SUCCESS (status)) {
                
                DbgTraceTo(hubFdoContext->IfrLog,
                    TL_ERROR,
                    Hub,
                    "Could not open the IO target: %!STATUS!", 
                    status);
                WdfObjectDelete(hubFdoContext->RootHubIoTarget);

                LEAVE;
            }

            if (hubFdoContext->HubInfoFromParent.HubFlags.DisableLpmForAllDownstreamDevices) {
                INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &hubFdoContext->HubInformation.HubFlags, DisableLpm);
            }

        }

    } FINALLY {

        WdfObjectDelete(WdfRequest);

        HUBSM_AddHsmEvent(hubFdoContext,
                          NT_SUCCESS(status) ? HsmEventIoctlSuccess :
                                               HsmEventIoctlFailure);
    }
}


VOID
HUBPARENT_GetInfoFromParentUsingParentIoctl (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Sends IOCTL_INTERNAL_USB_GET_HUB_INFO to the parent to get IoTarget,
    ConnectorId and 30LtmInfo and store in the hub FDO context.

    If WdfRequestSend fails then HsmEventIoctlFailure should be queued
    directly from this function.

    Completion routine of this function should queue a HsmEventIoctlSuccess
    or HsmEventIoctlFailure event to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                status;
    WDFREQUEST              wdfRequest;
    WDFIOTARGET             wdfIoTarget;
    WDF_OBJECT_ATTRIBUTES   attributes;
    BOOLEAN                 requestSent;
    IO_STACK_LOCATION       ioStackLocation;

    TRY {
        
        wdfIoTarget = WdfDeviceGetIoTarget(HubFdoContext->HubFdoDevice);
        
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = (HubFdoContext->HubFdoDevice);
        
        status = WdfRequestCreate(&attributes, wdfIoTarget, &wdfRequest);

        if ( !NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfRequestCreate Failed %!STATUS!",
                       status);
            LEAVE;
        }

        RtlZeroMemory(&ioStackLocation,  sizeof(ioStackLocation));
        ioStackLocation.MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        ioStackLocation.Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_GET_HUB_INFO;
        ioStackLocation.Parameters.Others.Argument1 = &HubFdoContext->HubInfoFromParent;

        WdfRequestWdmFormatUsingStackLocation(wdfRequest, &ioStackLocation);

        WdfRequestSetCompletionRoutine(wdfRequest,
                                       HUBPARENT_GetInfoFromParentUsingParentIoctlComplete,
                                       NULL);

        requestSent = WdfRequestSend(wdfRequest,
                                     wdfIoTarget,
                                     NULL);

        if (requestSent == FALSE) {
            status = WdfRequestGetStatus(wdfRequest);

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "IOCTL_INTERNAL_USB_GET_HUB_INFO Request Not Sent! %!STATUS!",
                       status);

            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (wdfRequest != NULL) {
                WdfObjectDelete(wdfRequest);
            }

            HUBSM_AddHsmEvent(HubFdoContext, HsmEventIoctlFailure);
        }
    }
}
