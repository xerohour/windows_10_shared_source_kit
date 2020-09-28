/*++
Copyright (c) Microsoft Corporation

Module Name:

    hubxfer.c

Abstract:

    Control and interrupt transfers for the hub.

--*/

#include "pch.h"
#include "hubxfer.tmh"


EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBHTX_HubControlTransferComplete;
EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBHTX_PortControlTransferComplete;
EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBHTX_SendInterruptTransferComplete;
EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBHTX_BootDevicePortStatusControlTransferComplete;
EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBHTX_ClearTtBufferControlTransferComplete;

const CHAR ClearTtBufferTag[]              = "ClearTTBuffer Tag";


VOID
HUBHTX_HubControlTransferComplete(
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

    Completion routine for hub control requests.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PHUB_FDO_CONTEXT                hubFdoContext;
    USBD_STATUS                     urbStatus;
    struct _URB_CONTROL_TRANSFER_EX *urb;

    UNREFERENCED_PARAMETER(Target);

    TRY {

        status = CompletionParams->IoStatus.Status;

        hubFdoContext = (PHUB_FDO_CONTEXT)Context;

        urb = &hubFdoContext->HubControlRequest.Urb;
        urbStatus = urb->Hdr.Status;

        HUBMISC_RequestReuse(WdfRequest);

    } FINALLY {
        
        if (!NT_SUCCESS(status)) {
            
            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "HUB_CONTROL_TRANSFER_ERROR: Hub Control Transfer Completed with Status %!STATUS!, URB Status %x",
                       status,
                       urbStatus);

            EventWriteUSBHUB3_ETW_EVENT_HUB_CONTROL_TRANSFER_ERROR(
                0,
                hubFdoContext->HubParentInterface.Hub, 
                0,
                urbStatus,
                status);
            
            USB_HW_VERIFIER_HUB_BREAK(hubFdoContext, HubHwVerifierControlTransferFailure);

            hubFdoContext->HubFailureMsgId = HUB_FAILURE_CONTROL_TRANSFER_FAILED;

        }

        //
        // Notify HSM of completion status of the control transfer
        //
        HUBSM_AddHsmEvent(hubFdoContext,
                          NT_SUCCESS(status) ? HsmEventTransferSuccess : HsmEventTransferFailure);
    }
}


VOID
HUBHTX_CancelHubControlTransfer (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Cancels the current control transfer for the hub

Return Value:

    VOID

--*/
{
    BOOLEAN             cancelled;

    TRY {

        cancelled = WdfRequestCancelSentRequest(HubFdoContext->HubControlRequest.WdfRequest);

        if (cancelled == FALSE) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Unable to Cancel Control Request");
        }

    } FINALLY {

    }
}


VOID
HUBHTX_AckHubChangeUsingControlTransfer (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Acknowledges the current hub status change. Completion routine of this
    function should queue a HsmEventTransferSuccess or HsmEventTransferFailure
    event to HSM.

    Determines the appropriate feature selector that should be cleared, based
    on the current hub change event stored in the hub FDO context.


Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {


        //
        // Format the setup packet for Clear Hub Feature
        // USB 3.0 Spec, 10.14.2.1
        // USB 2.0 Spec, 11.24.2.1
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)HubFdoContext->HubControlRequest.Urb.SetupPacket;

        setupPacket->bRequest = USB_REQUEST_CLEAR_FEATURE;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->bmRequestType.Type = BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->wValue.W = HubFdoContext->CurrentFeatureSelector;
        setupPacket->wIndex.W = 0;
        setupPacket->wLength = 0;

        DbgTraceTo(HubFdoContext->IfrLog,
                   TL_INFO,
                   Hub,
                   "Ack Hub Change %!HUBFEATURESELECTOR!",
                   setupPacket->wValue.W);

        status = HUBMISC_ControlTransfer(HubFdoContext,
                                         HubFdoContext->HubParentInterface.Hub,
                                         (PVOID)HubFdoContext,
                                         &HubFdoContext->HubControlRequest,
                                         HUBHTX_HubControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         HubFdoContext->HsmContext.NeedsForwardProgress);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Clear Feature Request for Hub Status Change Failed %!STATUS!",
                       status);
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            //
            // Notify HSM of failure of to acknowledge change
            //
            HUBSM_AddHsmEvent(HubFdoContext, HsmEventTransferFailure);
        }
    }
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
HUBHTX_GetDescriptor (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext,
    __out_bcount(DescriptorBufferLength)
        PVOID              DescriptorBuffer,
    __in
        size_t             DescriptorBufferLength,
    __in
        UCHAR              DescriptorType,
    __in
        UCHAR              DescriptorIndex,
    __in
        BOOLEAN            ClassDescriptor
    )
/*++

Routine Description:


    Submits a request to get a descriptor from the hub. Completion routine of this
    function should queue a HsmEventTransferSuccess or HsmEventTransferFailure
    event to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        //
        // Format the setup packet for Get Descriptor
        // USB 3.0 Spec, 9.4.3
        // USB 2.0 Spec, 9.4.3
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)HubFdoContext->HubControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->bmRequestType.Type = (ClassDescriptor ? BMREQUEST_CLASS : BMREQUEST_STANDARD);
        setupPacket->bmRequestType.Dir = BMREQUEST_DEVICE_TO_HOST;

        setupPacket->bRequest = USB_REQUEST_GET_DESCRIPTOR;
        setupPacket->wValue.HiByte = DescriptorType;
        setupPacket->wValue.LowByte = DescriptorIndex;
        setupPacket->wIndex.W = 0;
        setupPacket->wLength = (USHORT)DescriptorBufferLength;

        if (ClassDescriptor == 0) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_INFO,
                       Hub,
                       "Get Descriptor %!USBDESCRIPTOR! - index %d",
                       setupPacket->wValue.HiByte,
                       setupPacket->wValue.LowByte);
        } else {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_INFO,
                       Hub,
                       "Get Descriptor %!HUBDESCRIPTOR!",
                       setupPacket->wValue.HiByte);
        }

        status = HUBMISC_ControlTransfer(HubFdoContext,
                                         HubFdoContext->HubParentInterface.Hub,
                                         (PVOID)HubFdoContext,
                                         &HubFdoContext->HubControlRequest,
                                         HUBHTX_HubControlTransferComplete,
                                         DescriptorBuffer,
                                         DescriptorBufferLength,
                                         TRUE,
                                         HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

    }

    return status;
}


VOID
HUBHTX_GetHubDescriptorUsingControlTransfer (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:


    Gets the hub descriptor. Completion routine of this function should queue
    a HsmEventTransferSuccess or HsmEventTransferFailure event to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                status;

    TRY {

        switch (HubFdoContext->HubParentInterface.HubSpeed) {
        //
        // Both 1.1 and 2.0 hubs use the same hub descriptor format
        // USB 2.0 Spec, 11.23.2.1
        //
        case UsbFullSpeed:
        case UsbHighSpeed:
            status = HUBHTX_GetDescriptor(HubFdoContext,
                                          &HubFdoContext->HubDescriptor.Hub20Descriptor,
                                          sizeof(struct _USB_HUB_DESCRIPTOR),
                                          USB_20_HUB_DESCRIPTOR_TYPE,
                                          0,
                                          TRUE);
            break;

        //
        // USB 3.0 hub descriptor format
        // USB 3.0 Spec, 10.13.2.1
        //
        case UsbSuperSpeed:
            status = HUBHTX_GetDescriptor(HubFdoContext,
                                          &HubFdoContext->HubDescriptor.Hub30Descriptor,
                                          sizeof(struct _USB_30_HUB_DESCRIPTOR),
                                          USB_30_HUB_DESCRIPTOR_TYPE,
                                          0,
                                          TRUE);
            break;

        //
        // No hub descriptor for the root hub, or unknown hub type
        //
        default:
            status = STATUS_UNSUCCESSFUL;

            NT_ASSERTMSG("Get Hub Descriptor for invalid hub type", FALSE);
            break;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Get Hub Descriptor Request Failed %!STATUS!",
                       status);

            //
            // Notify the HSM of of the failure to retrieve the hub descriptor
            //
            HUBSM_AddHsmEvent(HubFdoContext, HsmEventTransferFailure);
        }
    }
}


VOID
HUBHTX_GetHubConfigDescriptorWithDefaultSizeUsingControlTransfer (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the hub for the CONFIGURATION
    descriptor with the size set to 255.  A pointer/handle to the descriptor
    is cached in the HSM context.

    If WdfRequestSend fails then HsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a HsmEventTransferSuccess or HsmEventTransferFailure event to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS        status;

    TRY {

        //
        // Free any previous buffer allocation for the configuration descriptor
        //
        if (HubFdoContext->ConfigurationDescriptor != NULL) {
            ExFreePoolWithTag(HubFdoContext->ConfigurationDescriptor,
                              USBHUB3_HUB_TAG);
            HubFdoContext->ConfigurationDescriptor = NULL;
        }        
        
        //
        // COMPAT: Windows 7 hub driver always submits an initial 255 byte 
        //         request for the configuration descriptor.  If the descriptor
        //         is larger than 255 bytes, a larger buffer will be allocated
        //         and the request re-submitted.
        //
        HubFdoContext->ConfigurationDescriptor =
            ExAllocatePoolWithTag(NonPagedPool,
                                  MAXIMUM_CONFIGURATION_DESCRIPTOR_SIZE,
                                  USBHUB3_HUB_TAG);

        if (HubFdoContext->ConfigurationDescriptor == NULL) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Unable to allocate space for configuration descriptor");

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }
        
        status = HUBHTX_GetDescriptor(HubFdoContext,
                                      HubFdoContext->ConfigurationDescriptor,
                                      MAXIMUM_CONFIGURATION_DESCRIPTOR_SIZE,
                                      USB_CONFIGURATION_DESCRIPTOR_TYPE,
                                      0,
                                      FALSE);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Get USB_CONFIGURATION_DESCRIPTOR Request Failed for Hub %!STATUS!",
                       status);

            if (HubFdoContext->ConfigurationDescriptor != NULL) {
                
                ExFreePoolWithTag(HubFdoContext->ConfigurationDescriptor, USBHUB3_HUB_TAG);
                HubFdoContext->ConfigurationDescriptor = NULL;
            }

            //
            // Notify the HSM of of the failure to retrieve the config descriptor
            //
            HUBSM_AddHsmEvent(HubFdoContext, HsmEventTransferFailure);
        }
    }
}


VOID
HUBHTX_GetHubConfigDescriptorWithReturnedLengthUsingControlTransfer (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the hub for the CONFIGURATION
    descriptor with the size set to the value returned from the last
    call to HUBHTX_GetHubConfigDescriptorWithDefaultSizeUsingControlTransfer.
    A pointer/handle to the descriptor is cached in the HSM context.

    If WdfRequestSend fails then HsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a HsmEventTransferSuccess or HsmEventTransferFailure event to HSM.

Return Value:

    VOID

--*/
{
    PUSB_CONFIGURATION_DESCRIPTOR   oldConfigurationDescriptorBuffer;
    NTSTATUS                        status;

    TRY {
    
        NT_ASSERT(HubFdoContext->ConfigurationDescriptor != NULL);
        
        oldConfigurationDescriptorBuffer = HubFdoContext->ConfigurationDescriptor;

        HubFdoContext->ConfigurationDescriptor =
            ExAllocatePoolWithTag(NonPagedPool,
                                  oldConfigurationDescriptorBuffer->wTotalLength,
                                  USBHUB3_HUB_TAG);

        if (HubFdoContext->ConfigurationDescriptor == NULL) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Unable to allocate space for configuration descriptor");

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }
        
        status = HUBHTX_GetDescriptor(HubFdoContext,
                                      HubFdoContext->ConfigurationDescriptor,
                                      oldConfigurationDescriptorBuffer->wTotalLength,
                                      USB_CONFIGURATION_DESCRIPTOR_TYPE,
                                      0,
                                      FALSE);
    } FINALLY {

        if (oldConfigurationDescriptorBuffer != NULL) {
            ExFreePoolWithTag(oldConfigurationDescriptorBuffer, USBHUB3_HUB_TAG);
        }

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "USB_CONFIGURATION_DESCRIPTOR_TYPE Request Failed for Hub %!STATUS!",
                       status);

            if (HubFdoContext->ConfigurationDescriptor != NULL) {
                ExFreePoolWithTag(HubFdoContext->ConfigurationDescriptor, USBHUB3_HUB_TAG);
                HubFdoContext->ConfigurationDescriptor = NULL;
            }

            //
            // Notify the HSM of of the failure to retrieve the config descriptor
            //
            HUBSM_AddHsmEvent(HubFdoContext, HsmEventTransferFailure);
        }
    }
}


HSM_EVENT
HUBHTX_IsBytesReturnedLessThanTotalLengthForHubConfigDescriptor (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Determines if the bytes returned from the last request to
    get confiugration descriptor is less than the value
    returned in wTotalLength

Returns:

    HsmEventYes
    HsmEventNo

--*/
{
    HSM_EVENT   hsmEvent;

    TRY {

        hsmEvent = (HubFdoContext->ConfigurationDescriptor->wTotalLength > 
                    MAXIMUM_CONFIGURATION_DESCRIPTOR_SIZE) ?
                   HsmEventYes :
                   HsmEventNo;

        // Store the wTotalLength here, to validate the new wTotalLength against
        HubFdoContext->OldwTotalLength = HubFdoContext->ConfigurationDescriptor->wTotalLength;

    } FINALLY {

    }

    return hsmEvent;
}


HSM_EVENT
HUBHTX_ValidateAndCacheHubConfigDescriptor (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Performs validation of the hub configuration descriptor.

Returns:

    HsmEventOperationSuccess
    HsmEventOperationFailure

--*/
{
    PUSB_CONFIGURATION_DESCRIPTOR   oldConfigurationDescriptorBuffer;
    NTSTATUS                        status;
    VALIDATION_DEVICE_INFO          devInfo;
    
    
    TRY {
        
        oldConfigurationDescriptorBuffer = NULL;

        RtlZeroMemory(&devInfo,
                      sizeof(VALIDATION_DEVICE_INFO));
        devInfo.USBVersion = HubFdoContext->HubInfoFromParent.DeviceDescriptor.bcdUSB;
        devInfo.USBSpeed = HubFdoContext->HubParentInterface.HubSpeed;
        devInfo.LogErrorFunction = HUBMISC_LogDescriptorValidationErrorForHub;
        devInfo.LogWarningFunction = HUBMISC_LogDescriptorValidationWarningForHub;
        devInfo.LogFunctionContext = HubFdoContext;

        // If the config descriptor is smaller than the default allocated
        // length, then allocate a smaller buffer to reduce memory footprint.
        if (HubFdoContext->ConfigurationDescriptor->wTotalLength < 
            MAXIMUM_CONFIGURATION_DESCRIPTOR_SIZE) {

            oldConfigurationDescriptorBuffer = HubFdoContext->ConfigurationDescriptor;

            HubFdoContext->ConfigurationDescriptor =
                ExAllocatePoolWithTag(NonPagedPool,
                                      oldConfigurationDescriptorBuffer->wTotalLength,
                                      USBHUB3_HUB_TAG);

            if (HubFdoContext->ConfigurationDescriptor == NULL) {
                DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Unable to allocate space for configuration descriptor");

                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            RtlCopyMemory(HubFdoContext->ConfigurationDescriptor,
                          oldConfigurationDescriptorBuffer,
                          oldConfigurationDescriptorBuffer->wTotalLength);
        }

        if (FALSE == HUBDESC_ValidateConfigurationDescriptorSet(HubFdoContext->ConfigurationDescriptor,
                                                                HubFdoContext->ConfigurationDescriptor->wTotalLength,
                                                                &devInfo,
                                                                HubFdoContext->IfrLog,
                                                                NULL)) {
                     
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }
        
        // 
        // Verify that the new wTotalLength returned is same as the one returned
        // in the header(or in the first call). We do this after calling the 
        // validation function, so that other failures gets logged before bailing
        // out due to this check
        //
        if (HubFdoContext->ConfigurationDescriptor->wTotalLength != 
                HubFdoContext->OldwTotalLength) {

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to value of ConfigDesc->wTotalLength (0x%x) exceeding the buffer length (0x%x).",
                       HubFdoContext->ConfigurationDescriptor->wTotalLength,
                       HubFdoContext->OldwTotalLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&devInfo, DescriptorValidationErrorConfigWTotalLengthTooLarge);
            status = STATUS_UNSUCCESSFUL;
            LEAVE;

        }

        status = STATUS_SUCCESS;

    } FINALLY {
        
        if (oldConfigurationDescriptorBuffer != NULL) {
            ExFreePoolWithTag(oldConfigurationDescriptorBuffer, USBHUB3_HUB_TAG);
        }

        if (!NT_SUCCESS(status)) {

            if (HubFdoContext->ConfigurationDescriptor != NULL) {
                ExFreePoolWithTag(HubFdoContext->ConfigurationDescriptor, USBHUB3_HUB_TAG);
                HubFdoContext->ConfigurationDescriptor = NULL;
            }
        }
    }

    return (NT_SUCCESS(status) ? HsmEventOperationSuccess : HsmEventOperationFailure);
}


VOID
HUBHTX_GetHubStandardStatusUsingControlTransfer (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Gets hub's device status using standard USB_REQUEST_GET_STATUS. Completion
    routine of this function should queue a HsmEventTransferSuccess or HsmEventTransferFailure
    event to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)HubFdoContext->HubControlRequest.Urb.SetupPacket;

        //
        // Format the setup packet for Get Hub Status
        // USB 3.0 Spec, 10.14.2.4
        // USB 2.0 Spec, 11.24.2.6
        //
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->bmRequestType.Type =  BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_DEVICE_TO_HOST;

        setupPacket->bRequest = USB_REQUEST_GET_STATUS;
        setupPacket->wValue.W = 0;
        setupPacket->wIndex.W = 0;
        setupPacket->wLength = sizeof(HubFdoContext->HubDeviceStatus);

        DbgTraceTo(HubFdoContext->IfrLog,
                   TL_INFO,
                   Hub,
                   "Get Hub Standard Status");

        status = HUBMISC_ControlTransfer(HubFdoContext,
                                         HubFdoContext->HubParentInterface.Hub,
                                         (PVOID)HubFdoContext,
                                         &HubFdoContext->HubControlRequest,
                                         HUBHTX_HubControlTransferComplete,
                                         &HubFdoContext->HubDeviceStatus,
                                         sizeof(HubFdoContext->HubDeviceStatus),
                                         FALSE,
                                         HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Get Hub Standard Status Request Failed %!STATUS!", status);

            HUBSM_AddHsmEvent(HubFdoContext, HsmEventTransferFailure);
        }

    }
}

VOID
HUBHTX_GetHubStatusUsingControlTransfer (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Gets hub's port 0 status using class-specific USB_REQUEST_GET_STATUS. Completion
    routine of this function should queue a HsmEventTransferSuccess or HsmEventTransferFailure
    event to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)HubFdoContext->HubControlRequest.Urb.SetupPacket;

        //
        // Format the setup packet for Get Hub Status
        // USB 3.0 Spec, 10.14.2.4
        // USB 2.0 Spec, 11.24.2.6
        //
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_DEVICE_TO_HOST;

        setupPacket->bRequest = USB_REQUEST_GET_STATUS;
        setupPacket->wValue.W = 0;
        setupPacket->wIndex.W = 0;
        setupPacket->wLength = 4;

        HubFdoContext->OldHubStatusAndChange = HubFdoContext->HubStatusAndChange;

        DbgTraceTo(HubFdoContext->IfrLog,
                   TL_INFO,
                   Hub,
                   "Get Hub Status");

        status = HUBMISC_ControlTransfer(HubFdoContext,
                                         HubFdoContext->HubParentInterface.Hub,
                                         (PVOID)HubFdoContext,
                                         &HubFdoContext->HubControlRequest,
                                         HUBHTX_HubControlTransferComplete,
                                         &HubFdoContext->HubStatusAndChange,
                                         sizeof(HubFdoContext->HubStatusAndChange),
                                         FALSE,
                                         HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Get Hub Status Request Failed %!STATUS!", status);

            HUBSM_AddHsmEvent(HubFdoContext, HsmEventTransferFailure);
        }
    }
}

VOID
HUBHTX_SetHubDepthUsingControlTransfer (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Set Hub Depth by sending a control transfer to the hub
    
Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        NT_ASSERT(HubFdoContext->HubParentInterface.HubDepth != 0);

        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)HubFdoContext->HubControlRequest.Urb.SetupPacket;

        //
        // Format the setup packet for Get Hub Status
        // USB 3.0 Spec, 10.14.2.4
        // USB 2.0 Spec, 11.24.2.6
        //
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_HUB_DEPTH;

        //
        // HubDepth expected in this request is 0 based, while we
        // get 1 based depth from the parent
        //
        setupPacket->wValue.W = HubFdoContext->HubParentInterface.HubDepth  - 1;
        setupPacket->wIndex.W = 0;
        setupPacket->wLength = 0;

        DbgTraceTo(HubFdoContext->IfrLog,
                   TL_INFO,
                   Hub,
                   "Set Hub Depth %d",
                   setupPacket->wValue.W);

        status = HUBMISC_ControlTransfer(HubFdoContext,
                                         HubFdoContext->HubParentInterface.Hub,
                                         (PVOID)HubFdoContext,
                                         &HubFdoContext->HubControlRequest,
                                         HUBHTX_HubControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Set Hub Depth Request Failed %!STATUS!",
                       status);

            HUBSM_AddHsmEvent(HubFdoContext, HsmEventTransferFailure);
        }
    }
}

VOID
HUBHTX_SendInterruptTransfer (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Sends interrupt transfer for the port status change.

    The completion routine for the transfer should queue
    HsmEventInterruptTransferSuccess or
    HsmEventInterruptTransferFailure to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                                status;
    WDFREQUEST                              wdfRequest;
    IO_STACK_LOCATION                       ioStackLocation;
    BOOLEAN                                 requestSent;
    struct _URB_BULK_OR_INTERRUPT_TRANSFER  *urb;

    TRY {

        status = STATUS_SUCCESS;
        
        wdfRequest = HubFdoContext->StatusChangeInterruptRequest.WdfRequest;

        HUBMISC_RequestReuse(wdfRequest);

        urb = &HubFdoContext->StatusChangeInterruptRequest.Urb;

        RtlZeroMemory(urb,
                      sizeof(*urb));

        urb->Hdr.UsbdDeviceHandle = HubFdoContext->HubParentInterface.Hub;

        //
        // set up the interrupt transfer
        //
        urb->Hdr.Length = sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);
        urb->Hdr.Function = URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER;
        urb->TransferFlags = USBD_SHORT_TRANSFER_OK;

        if (HubFdoContext->HsmContext.NeedsForwardProgress) {
            urb->TransferFlags |= USBD_FORWARD_PROGRESS_TRANSFER;
        }
        //
        // We have already made sure that the StatusChangeBitMap buffer is 
        // not less than InterruptPipeMaxPacketSize. So it is safe to use
        // this buffer for all hubs.
        //
        urb->TransferBuffer = HubFdoContext->StatusChangeInterruptRequest.StatusChangeBitMap;
        //
        // Transfer size for the root hub will be calculated based on the 
        // number of ports. For non root hubs, it has to be equal to the
        // Max Packet Size
        //
        if (HubFdoContext->HubParentInterface.HubDepth == 0) {
            urb->TransferBufferLength =
                HubFdoContext->StatusChangeInterruptRequest.MaxStatusChangeBitMapSize;
        } else {   
            urb->TransferBufferLength =
                HubFdoContext->HubInformation.InterruptPipeMaxPacketSize;
        }
        urb->TransferBufferMDL = NULL;
        urb->PipeHandle = HubFdoContext->StatusChangeInterruptRequest.UsbdInterruptPipeHandle;
        
        if(HubFdoContext->StatusChangeInterruptRequest.NoChangesInLastInterrupt) {
            //
            // The last interrupt we got had no changes in it. This flag will
            // only be used for root hub by UCX to determine whether to send
            // the transfer to HCD or pend it
            //
            urb->Hdr.UsbdFlags |= USBDFLAG_ROOTHUB_PEND_INTERRUPT_TX;
            HubFdoContext->StatusChangeInterruptRequest.NoChangesInLastInterrupt = FALSE;
        }                                    

        RtlZeroMemory(&ioStackLocation,  sizeof(ioStackLocation));
        ioStackLocation.MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        ioStackLocation.Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
        ioStackLocation.Parameters.Others.Argument1 = urb;

        WdfRequestWdmFormatUsingStackLocation(wdfRequest, &ioStackLocation);

        WdfRequestSetCompletionRoutine(wdfRequest,
                                       HUBHTX_SendInterruptTransferComplete,
                                       HubFdoContext);

        requestSent = WdfRequestSend(wdfRequest,
                                     WdfDeviceGetIoTarget(HubFdoContext->HubFdoDevice),
                                     NULL);

        if (requestSent == FALSE) {

            status = WdfRequestGetStatus(wdfRequest);

            HUBMISC_RequestReuse(wdfRequest);

            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            HUBSM_AddHsmEvent(HubFdoContext, HsmEventInterruptTransferFailure);
        }
    }
}


VOID
HUBHTX_SendInterruptTransferComplete(
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

    Completion routine for interrupt transfer requests.

Returns:

    VOID

--*/
{
    NTSTATUS                                status;
    PHUB_FDO_CONTEXT                        hubFdoContext;
    USBD_STATUS                             urbStatus;
    struct _URB_BULK_OR_INTERRUPT_TRANSFER  *urb;

    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(WdfRequest);

    TRY {

        status = CompletionParams->IoStatus.Status;

        hubFdoContext = (PHUB_FDO_CONTEXT) Context;

        urb = &hubFdoContext->StatusChangeInterruptRequest.Urb;
        urbStatus = urb->Hdr.Status;
        NT_ASSERT(urb->TransferBufferLength <= hubFdoContext->StatusChangeInterruptRequest.MaxStatusChangeBitMapSize);
        hubFdoContext->StatusChangeInterruptRequest.CurrentStatusChangeBitMapSize = (USHORT)urb->TransferBufferLength;
        
        //
        // We initialize the RTL bitmap manually, as RtlIntializeBitMap
        // is not callable at dispatch level (despite the documentation).  
        //
        HUBMISC_InitializeBitmap(&hubFdoContext->StatusChangeInterruptRequest.StatusChangeBitMapHeader,
                                 hubFdoContext->StatusChangeInterruptRequest.StatusChangeBitMap,
                                 hubFdoContext->StatusChangeInterruptRequest.CurrentStatusChangeBitMapSize * 8);



    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Status Change Interrupt Completion %!STATUS!, URB status %x",
                       status,
                       urbStatus);
            
            if (status != STATUS_CANCELLED) {
                DbgTraceTo(hubFdoContext->IfrLog,
                           TL_ERROR,
                           Hub,
                           "ERROR: HubHwVerifierInterruptTransferFailure Status Change Interrupt Completion %!STATUS!, URB status %x",
                           status,
                           urbStatus);

                USB_HW_VERIFIER_HUB_BREAK(hubFdoContext, HubHwVerifierInterruptTransferFailure);

                hubFdoContext->HubFailureMsgId = HUB_FAILURE_INTERRUPT_TRANSFER_FAILED;

                EventWriteUSBHUB3_ETW_EVENT_PORT_INTERRUPT_TRANSFER_ERROR(
                    &hubFdoContext->CurrentActivityId,
                    hubFdoContext->HubParentInterface.Hub,
                    0,
                    urbStatus,
                    status);

            }
        }

        //
        // Notify HSM of completion status of the interrupt transfer
        //
        HUBSM_AddHsmEvent( hubFdoContext,
                           NT_SUCCESS(status) ? HsmEventInterruptTransferSuccess
                                              : HsmEventInterruptTransferFailure );
    }
}


VOID
HUBHTX_CancelInterruptTransfer (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Cancels the current control transfer

Return Value:

    VOID

--*/
{
    BOOLEAN                 cancelled;

    TRY {

        cancelled =
            WdfRequestCancelSentRequest(HubFdoContext->StatusChangeInterruptRequest.WdfRequest);

        if (!cancelled) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Unable to Cancel Interrupt Request");
        }

    } FINALLY {

    }
}


HSM_EVENT
HUBHTX_IsHubChangeBitSet (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Determines if for the latest port status change notification the change
    bit for the hub(0) is set.  This function will also check for status changes
    for invalid port numbers.

Return Value:

    HsmEventYes
    HsmEventNo
    HsmEventHubError

--*/
{
    HSM_EVENT   hsmEvent;
    ULONG       portNumber;

    TRY {

        if (RtlCheckBit(&HubFdoContext->StatusChangeInterruptRequest.StatusChangeBitMapHeader,
                        HUB_CHANGE_BIT)) {

            hsmEvent = HsmEventYes;
        } else {

            hsmEvent = HsmEventNo;
        }

        //
        // Check that we have valid port contexts for all change bits that are set.
        //
        for (portNumber = 1; 
             portNumber < (ULONG)(HubFdoContext->StatusChangeInterruptRequest.CurrentStatusChangeBitMapSize * 8); 
             portNumber++) {

            if (RtlCheckBit(&HubFdoContext->StatusChangeInterruptRequest.StatusChangeBitMapHeader,
                            portNumber)) {
                
                if (HUBFDO_FindPortContextFromNumber(HubFdoContext, portNumber) == NULL) {

                    DbgTraceTo(HubFdoContext->IfrLog,
                               TL_ERROR,
                               Hub,
                               "Port status change for invalid port number %d",
                               portNumber);

                    hsmEvent = HsmEventHubError;
                    LEAVE;
                }
            }
        }

    } FINALLY {

    }

    return hsmEvent;
}


HSM_EVENT
HUBHTX_IsOverCurrentBitCleared (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Checks the cached value of hub status and returns true if Overcurrent bit
    is cleared and false if not.

--*/
{
    HSM_EVENT           hsmEvent;

    TRY {

        if (HubFdoContext->HubStatusAndChange.HubStatus.OverCurrent == 0) {

            hsmEvent = HsmEventYes;
        } else {

            hsmEvent = HsmEventNo;
        }

    } FINALLY {

    }

    return hsmEvent;
}


HSM_EVENT
HUBHTX_GetHubChangeEvent (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    This function returns the CurrentHubChangeEvent stored
    in the Hub Fdo Context

Return Value:

    HsmEventHubOverCurrent
    HsmEventHubOverCurrentCleared
    HsmEventLocalPowerGood
    HsmEventLocalPowerLost
    HsmEventError

--*/
{

    TRY {

        NT_ASSERTMSG("Invalid Port Change Event",
                     (HubFdoContext->CurrentHubChangeEvent == HsmEventHubOverCurrent) ||
                     (HubFdoContext->CurrentHubChangeEvent == HsmEventHubOverCurrentCleared) ||
                     (HubFdoContext->CurrentHubChangeEvent == HsmEventHubLocalPowerGood) ||
                     (HubFdoContext->CurrentHubChangeEvent == HsmEventHubLocalPowerLost) ||
                     (HubFdoContext->CurrentHubChangeEvent == HsmEventHubError));

    } FINALLY {

    }

    return HubFdoContext->CurrentHubChangeEvent;
}


HSM_EVENT
HUBHTX_ValidateAndParseHubDescriptor (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Parses and validates the hub descriptor.

Return Value:

    HsmEventOperationSuccess
    HsmEventOperationFailure

--*/
{
    HSM_EVENT                hsmEvent;
    PUSB20_HUB_CHAR_BITS     usb20Characteristics;
    PUSB30_HUB_CHAR_BITS     usb30Characteristics;
    VALIDATION_DEVICE_INFO   devInfo;

    TRY {

        RtlZeroMemory(&devInfo,
                      sizeof(VALIDATION_DEVICE_INFO));
        devInfo.USBVersion = HubFdoContext->HubInfoFromParent.DeviceDescriptor.bcdUSB;
        devInfo.USBSpeed = HubFdoContext->HubParentInterface.HubSpeed;
        devInfo.LogErrorFunction = HUBMISC_LogDescriptorValidationErrorForHub;
        devInfo.LogWarningFunction = HUBMISC_LogDescriptorValidationWarningForHub;
        devInfo.LogFunctionContext = HubFdoContext;
        
        switch (HubFdoContext->HubParentInterface.HubSpeed) {
        case UsbFullSpeed:
        case UsbHighSpeed:
            
            if (HUBDESC_Validate20HubDescriptor(&HubFdoContext->HubDescriptor.Hub20Descriptor,
                                                sizeof(struct _USB_HUB_DESCRIPTOR),
                                                &devInfo,
                                                HubFdoContext->IfrLog)) {                
                hsmEvent = HsmEventOperationSuccess;
            } else {
                hsmEvent = HsmEventOperationFailure;
                break;
            }

            //
            // Extract relevant information from the hub descriptor
            //
            HubFdoContext->HubInformation.NumberOfPorts =
                HubFdoContext->HubDescriptor.Hub20Descriptor.bNumberOfPorts;

            HubFdoContext->HubInformation.NumberOf20Ports =
                HubFdoContext->HubDescriptor.Hub20Descriptor.bNumberOfPorts;

            usb20Characteristics = (PUSB20_HUB_CHAR_BITS)
                &HubFdoContext->HubDescriptor.Hub20Descriptor.wHubCharacteristics;

            if (usb20Characteristics->PortPowerSwitched == 1) {
                INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &HubFdoContext->HubInformation.HubFlags, PerPortPower);
            }

            if (usb20Characteristics->PerPortOvercurrent == 1) {
                INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &HubFdoContext->HubInformation.HubFlags, PortPortOverCurrent);
            }
            
            break;

        case UsbSuperSpeed:

            if (HUBDESC_Validate30HubDescriptor(&HubFdoContext->HubDescriptor.Hub30Descriptor,
                                                sizeof(struct _USB_30_HUB_DESCRIPTOR),
                                                &devInfo,
                                                HubFdoContext->IfrLog)) {                
                hsmEvent = HsmEventOperationSuccess;
            } else {
                hsmEvent = HsmEventOperationFailure;
                break;
            }
            //
            // Extract relevant information from the hub descriptor
            //
            HubFdoContext->HubInformation.NumberOfPorts =
                HubFdoContext->HubDescriptor.Hub30Descriptor.bNumberOfPorts;

            HubFdoContext->HubInformation.NumberOf30Ports =
                HubFdoContext->HubDescriptor.Hub30Descriptor.bNumberOfPorts;

            usb30Characteristics = (PUSB30_HUB_CHAR_BITS) 
                &HubFdoContext->HubDescriptor.Hub30Descriptor.wHubCharacteristics;

            if (usb30Characteristics->PortPowerSwitched == 1) {
                INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &HubFdoContext->HubInformation.HubFlags, PerPortPower);
            }

            if ((usb30Characteristics->OvercurrentProectionMode == 0x2) ||
                (usb30Characteristics->OvercurrentProectionMode == 0x3)) {

                INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &HubFdoContext->HubInformation.HubFlags, NoOverCurrentProtection);

            } else if (usb30Characteristics->OvercurrentProectionMode == 0x1) {

                INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &HubFdoContext->HubInformation.HubFlags, PortPortOverCurrent);

            }

            break;

        //
        // No hub descriptor for the root hub, or unknown hub type
        //
        default:

            NT_ASSERTMSG("GetHubDescriptor called for invalid hub type", FALSE);

            hsmEvent = HsmEventOperationFailure;
            break;
        }

    } FINALLY {

        if (hsmEvent == HsmEventOperationFailure) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Hub Descriptor Validation Failed!");
        }
    }

    return hsmEvent;
}


HSM_EVENT
HUBHTX_ValidateAndParseDevicePowerStatus (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Parses and validates the hub status returned by GetStatusUsingControl
    Transfer and stores the power status of the hub in the hub FDO context.

    Based on UsbhGetMaxPowerPerPort() in the Windows 7 hub driver.

Return Value:

    VOID

--*/
{
    USB_DEVICE_STATUS               hubDeviceStatus;

    TRY {

        hubDeviceStatus = HubFdoContext->HubDeviceStatus;

        if (hubDeviceStatus.SelfPowered == 1) {
            HubFdoContext->HubInformation.MaxPortPower = MaxPortPower500Milliamps;
        } else {
            HubFdoContext->HubInformation.MaxPortPower = MaxPortPower100Milliamps;
        }

    } FINALLY {

    }

    return HsmEventOperationSuccess;
}


HSM_EVENT
HUBHTX_GetErrorResponseEventOnBadStatusChange (
    __in
        PHUB_FDO_CONTEXT       HubFdoContext
    )
/*++

Routine Description:

Return Value:

    VOID

--*/
{
    UNREFERENCED_PARAMETER(HubFdoContext);

    TRY {

    } FINALLY {

    }

    return HsmEventHubFatalError;
}


HSM_EVENT
HUBHTX_GetHubLostChangeEvent (
    __in
        PHUB_FDO_CONTEXT       HubFdoContext
    )
/*++

Routine Description:

    Lookes at the cached port 0 status for the hub and returns
    a hub change event for any status change for which the
    corresponding change bit is not set.

Return Value:

    HsmEventHubOverCurrent
    HsmEventHubOverCurrentCleared
    HsmEventOperationSuccess
    HsmEventLocalPowerFood
    HsmEventLocalPowerLost

--*/
{
    HSM_EVENT           hsmEvent;
    USB_HUB_CHANGE      hubChange;
    USB_HUB_STATUS      hubStatus, oldHubStatus;

    TRY {

        hubStatus = HubFdoContext->HubStatusAndChange.HubStatus;
        hubChange = HubFdoContext->HubStatusAndChange.HubChange;
        oldHubStatus = HubFdoContext->HubStatusAndChange.HubStatus;

        if (hubChange.OverCurrentChange == 0) {

            if ((hubStatus.OverCurrent == 1) &&
                (oldHubStatus.OverCurrent == 0)) {
                
                HUBREG_UpdateSqmHubOvercurrentDetected(HubFdoContext);
                    
                hsmEvent = HsmEventHubOverCurrent;
                LEAVE;

            } else if ((hubStatus.OverCurrent == 0) &&
                       (oldHubStatus.OverCurrent == 1)) {

                hsmEvent = HsmEventHubOverCurrentCleared;
                LEAVE;
            }

        } else if (hubChange.LocalPowerChange == 0) {

            if ((hubStatus.LocalPowerLost == 1) &&
                (oldHubStatus.LocalPowerLost == 0)) {

                hsmEvent = HsmEventHubLocalPowerLost;
                LEAVE;

            } else if ((hubStatus.LocalPowerLost == 0) &&
                       (oldHubStatus.LocalPowerLost == 1)) {

                hsmEvent = HsmEventHubLocalPowerGood;
                LEAVE;
            }
        }

        hsmEvent = HsmEventOperationSuccess;

    } FINALLY {

    }

    return hsmEvent;
}


VOID
HUBHTX_PortControlTransferComplete(
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

    Completion routine for hub control requests.

Returns:

    NTSTATUS

--*/
{
    NTSTATUS                            status;
    PPORT_CONTEXT                       portContext;
    USBD_STATUS                         urbStatus;
    struct _URB_CONTROL_TRANSFER_EX     *urb;
    USB_PORT_STATUS_AND_CHANGE          portStatus;
    PUSB_DEFAULT_PIPE_SETUP_PACKET      setupPacket;
    NTSTATUS                            stopIdleStatus;


    UNREFERENCED_PARAMETER(Target);

    TRY {

        portContext = (PPORT_CONTEXT) Context;

        status = CompletionParams->IoStatus.Status;

        urb = &portContext->PortControlRequest.Urb;
        urbStatus = urb->Hdr.Status;

        if (!NT_SUCCESS(status)) {
    
            DbgTraceTo(portContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Port Control Transfer Completed with Error %!STATUS!, URB Status %x",
                       status,
                       urbStatus);

            USB_HW_VERIFIER_HUB_BREAK(portContext->HubFdoContext, HubHwVerifierControlTransferFailure);

            portContext->HubFdoContext->HubFailureMsgId = HUB_FAILURE_CONTROL_TRANSFER_FAILED;

            EventWriteUSBHUB3_ETW_EVENT_PORT_CONTROL_TRANSFER_ERROR(
                0,
                portContext->HubFdoContext->HubParentInterface.Hub, 
                portContext->PortInformation.PortNumber,
                urbStatus,
                status);
            
            LEAVE;
        }

        setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)urb->SetupPacket;

        if (setupPacket->bRequest == USB_REQUEST_GET_STATUS){

            //
            // Consider the case where controller was asleep and it wakes up to
            // handle a device resuming. WHenver controller wakes up, it also 
            // wakes up the root hub right away. Now,the root hub will not see any 
            // change until the resume is acknowledged by the controller driver. As a 
            // result, Root hub might start idling down. That considerably increases 
            // the probability of hub idle down process overlapping with  a 2.0 port 
            // resume process. This overlap can lead to 2.0 port being suspended right 
            // after it completes resuming, which can confuse some legacy devices. 
            // To avoid this problem, XHCI returns this special pending URB status 
            // whenever this reusme process is pending. When the hub sees this status, 
            // it acquires a power reference and  will only release it when it sees a 
            // new get port status with some other status code.
            //                                  
            if (urbStatus == USBD_STATUS_PORT_OPERATION_PENDING) {

                if (portContext->PortFlags.PendingPowerReferenceOnHub == 0) {

                    stopIdleStatus = WdfDeviceStopIdle(portContext->HubFdoContext->HubFdoDevice, FALSE);
                    if (NT_SUCCESS(stopIdleStatus)) {

                        INTERLOCKED_SET_BIT_FIELD(USB_PORT_FLAGS, &portContext->PortFlags, PendingPowerReferenceOnHub);
                    }
                }
            } else if (portContext->PortFlags.PendingPowerReferenceOnHub == 1) {
                
                WdfDeviceResumeIdle(portContext->HubFdoContext->HubFdoDevice);
                INTERLOCKED_CLEAR_BIT_FIELD(USB_PORT_FLAGS, &portContext->PortFlags, PendingPowerReferenceOnHub);

            }
            
            portStatus.AsUlong32 = *((PULONG)urb->TransferBuffer);
            
            if (portContext->PsmContext.StartIndex == StartIndexPsm30) {

                EventWriteUSBHUB3_ETW_EVENT_30_PORT_STATUS(
                    0,
                    portContext->HubFdoContext->HubParentInterface.Hub,
                    portContext->PortInformation.PortNumber,
                    portContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.AsUshort16,
                    portContext->CurrentExtPortStatusChange.PortStatusChange.PortChange.AsUshort16);

                DbgTraceTo(portContext->IfrLog,
                           TL_INFO,
                           Port,
                           "PortStatus: Connect=%c LinkState=%!LINKSTATE! Enable=%c Reset=%c Power=%c OverCurrent=%c",
                           (UCHAR)portStatus.PortStatus.Usb30PortStatus.CurrentConnectStatus + '0',
                           portStatus.PortStatus.Usb30PortStatus.PortLinkState,
                           (UCHAR)portStatus.PortStatus.Usb30PortStatus.PortEnabledDisabled + '0',
                           (UCHAR)portStatus.PortStatus.Usb30PortStatus.Reset + '0',
                           (UCHAR)portStatus.PortStatus.Usb30PortStatus.PortPower + '0',
                           (UCHAR)portStatus.PortStatus.Usb30PortStatus.OverCurrent + '0'); 

                DbgTraceTo(portContext->IfrLog,
                           TL_INFO,
                           Port,
                           "PortStatusChange: Connect=%c LinkState=%c Reset=%c BHReset=%c OverCurrent=%c ConfigError=%c",
                           (UCHAR)portStatus.PortChange.Usb30PortChange.ConnectStatusChange + '0',
                           (UCHAR)portStatus.PortChange.Usb30PortChange.PortLinkStateChange + '0',
                           (UCHAR)portStatus.PortChange.Usb30PortChange.ResetChange + '0',
                           (UCHAR)portStatus.PortChange.Usb30PortChange.BHResetChange + '0',
                           (UCHAR)portStatus.PortChange.Usb30PortChange.OverCurrentIndicatorChange + '0',
                           (UCHAR)portStatus.PortChange.Usb30PortChange.PortConfigErrorChange + '0'); 
            } else {
                
                EventWriteUSBHUB3_ETW_EVENT_20_PORT_STATUS(
                    0,
                    portContext->HubFdoContext->HubParentInterface.Hub,
                    portContext->PortInformation.PortNumber,
                    portContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.AsUshort16,
                    portContext->CurrentExtPortStatusChange.PortStatusChange.PortChange.AsUshort16);
        
                DbgTraceTo(portContext->IfrLog,
                           TL_INFO,
                           Port,
                           "PortStatus: Connect=%c HighSpeed=%c LowSpeed=%c Enable=%c Reset=%c Suspend=%c Power=%c OverCurrent=%c",
                           (UCHAR)portStatus.PortStatus.Usb20PortStatus.CurrentConnectStatus + '0',
                           (UCHAR)portStatus.PortStatus.Usb20PortStatus.HighSpeedDeviceAttached + '0',
                           (UCHAR)portStatus.PortStatus.Usb20PortStatus.LowSpeedDeviceAttached + '0',
                           (UCHAR)portStatus.PortStatus.Usb20PortStatus.PortEnabledDisabled + '0',
                           (UCHAR)portStatus.PortStatus.Usb20PortStatus.Reset + '0',
                           (UCHAR)portStatus.PortStatus.Usb20PortStatus.Suspend + '0',
                           (UCHAR)portStatus.PortStatus.Usb20PortStatus.PortPower + '0',
                           (UCHAR)portStatus.PortStatus.Usb20PortStatus.OverCurrent + '0'); 

                DbgTraceTo(portContext->IfrLog,
                           TL_INFO,
                           Port,
                           "PortStatusChange: Connect=%c Enable=%c Reset=%c Suspend=%c OverCurrent=%c",
                           (UCHAR)portStatus.PortChange.Usb20PortChange.ConnectStatusChange + '0',
                           (UCHAR)portStatus.PortChange.Usb20PortChange.PortEnableDisableChange + '0',
                           (UCHAR)portStatus.PortChange.Usb20PortChange.ResetChange + '0',
                           (UCHAR)portStatus.PortChange.Usb20PortChange.SuspendChange + '0',
                           (UCHAR)portStatus.PortChange.Usb20PortChange.OverCurrentIndicatorChange + '0'); 
            }
        }
        
        if (setupPacket->bRequest == USB_REQUEST_SET_FEATURE) {
            //
            // For suspend requests, we don't get a notification for when the operation
            // completes. So we just assume that they succeeded and update our cached
            // port status
            //

            if (setupPacket->wValue.W == PORT_SUSPEND) {
                portContext->PreviousPortStatus.Usb20PortStatus.Suspend = 1;
                portContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb20PortStatus.Suspend = 1;
            }
            if ((setupPacket->wValue.W == PORT_LINK_STATE) &&
                (setupPacket->wIndex.HiByte == PORT_LINK_STATE_U3)) {
                portContext->PreviousPortStatus.Usb30PortStatus.PortLinkState = PORT_LINK_STATE_U3;
                portContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb30PortStatus.PortLinkState = PORT_LINK_STATE_U3;
            }
            if ((setupPacket->wValue.W == PORT_LINK_STATE) &&
                (setupPacket->wIndex.HiByte == PORT_LINK_STATE_DISABLED)) {
                portContext->PreviousPortStatus.Usb30PortStatus.PortLinkState = PORT_LINK_STATE_DISABLED;
                portContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb30PortStatus.PortLinkState = PORT_LINK_STATE_DISABLED;
            }

        }



    } FINALLY {

        HUBMISC_RequestReuse(WdfRequest);
        
        //
        // Notify PSM of completion status of the control transfer
        //
        portContext->PsmContext.AddEvent(portContext,
                                         NT_SUCCESS(status) ? PsmEventTransferSuccess :
                                                              PsmEventTransferFailure);
    }
}


VOID
HUBHTX_CancelControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Cancels the current control transfer for the port

Return Value:

    VOID

--*/
{
    BOOLEAN             cancelled;

    TRY {

        cancelled = WdfRequestCancelSentRequest(PortContext->PortControlRequest.WdfRequest);

        if (!cancelled) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Unable to Cancel Control Request");
        }

    } FINALLY {

    }
}

VOID
HUBHTX_GetPortStatusUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Gets port status by sending a control transfer. Completion routine of this
    function should queue a TransferSuccess or TransferFailure event to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    USHORT                          length;

    TRY {

        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        //
        // Format the setup packet for Get Port Status
        // USB 3.0 Spec, 10.14.2.6
        // USB 2.0 Spec, 11.24.2.7
        //
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_DEVICE_TO_HOST;

        setupPacket->bRequest = USB_REQUEST_GET_STATUS;
        setupPacket->wIndex.W = PortContext->PortInformation.PortNumber;
        if (PortContext->PortInformation.PortFlags.PortFlagEnhancedSuperSpeed) {
            setupPacket->wValue.W = USB_STATUS_EXT_PORT_STATUS;
            length = sizeof(PortContext->CurrentExtPortStatusChange);
        } else {
            setupPacket->wValue.W = USB_STATUS_PORT_STATUS;
            length = sizeof(PortContext->CurrentExtPortStatusChange.PortStatusChange);
        }
        setupPacket->wLength = length;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Get Port %d Status",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         &PortContext->CurrentExtPortStatusChange,
                                         length,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {
       
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Get Port Status request failed - %!STATUS!", 
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}


VOID
HUBHTX_AckConnectChangeUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Acknowledges the current port connect change. Completion routine of this
    function should queue a TransferSuccess or TransferFailure event to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        //
        // Format the setup packet for Clear Port Feature
        // USB 3.0 Spec, 10.14.2.2
        // USB 2.0 Spec, 11.24.2.2
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bRequest = USB_REQUEST_CLEAR_FEATURE;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->wValue.W = C_PORT_CONNECTION;
        setupPacket->wIndex.W = PortContext->PortInformation.PortNumber;
        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Ack Port %d Change - C_PORT_CONNECTION",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);
    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Acknowledge Connect Change submitted - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}


VOID
HUBHTX_AckPortChangeUsingControlTransfer (
    __in PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Acknowledges the current port status change stored in CurrentFeatureSelector
    field in the PortContex.. Completion routine of this function should queue a 
    TransferSuccess or TransferFailure event to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {                                     

        NT_ASSERT(PortContext->CurrentFeatureSelector != PsmEventNull);
        
        //
        // Format the setup packet for Clear Port Feature
        // USB 3.0 Spec, 10.14.2.2
        // USB 2.0 Spec, 11.24.2.2
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bRequest = USB_REQUEST_CLEAR_FEATURE;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->wValue.W = PortContext->CurrentFeatureSelector;
        setupPacket->wIndex.W = PortContext->PortInformation.PortNumber;
        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Ack Port %d Change - %!PORTFEATURESELECTOR!",
                   PortContext->PortInformation.PortNumber,
                   setupPacket->wValue.W);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);

        PortContext->CurrentFeatureSelector = PsmEventNull;

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Acknowledge Port Change failed - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}


VOID
HUBHTX_ResetPortUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Resets the port using control transfer. The completion routine should
    queue a TransferSuccess or TransferFailure to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        PortContext->CurrentPortEvent = PsmEventPortNoChange;

        //
        // Format the setup packet for Clear Port Feature
        // USB 3.0 Spec, 10.14.2.2
        // USB 2.0 Spec, 11.24.2.2
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wValue.W = PORT_RESET;
        setupPacket->wIndex.W = PortContext->PortInformation.PortNumber;
        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Reset Port %d",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);
    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Reset Port request failed - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}

VOID
HUBHTX_WarmResetPortUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Resets the port using control transfer. The completion routine should
    queue a TransferSuccess or TransferFailure to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        PortContext->CurrentPortEvent = PsmEventPortNoChange;

        //
        // Format the setup packet for Clear Port Feature
        // USB 3.0 Spec, 10.14.2.2
        // USB 2.0 Spec, 11.24.2.2
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wValue.W = BH_PORT_RESET;
        setupPacket->wIndex.W = PortContext->PortInformation.PortNumber;
        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Warm Reset Port %d",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);
    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Warm Reset Port request failed - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}

VOID
HUBHTX_DisablePortUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Disable the port using control transfer. The completion routine should
    queue a TransferSuccess or TransferFailure to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        //
        // Format the setup packet for Clear Port Feature
        // USB 3.0 Spec, 10.14.2.2
        // USB 2.0 Spec, 11.24.2.2
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_CLEAR_FEATURE;
        setupPacket->wValue.W = PORT_ENABLE;
        setupPacket->wIndex.W = PortContext->PortInformation.PortNumber;
        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Disable Port %d",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Disable Port request failed - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}

VOID
HUBHTX_DisableRemoteWakeOnPortUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Disable the remote wake on port using control transfer. The completion 
    routine should queue a TransferSuccess or TransferFailure to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                            status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET      setupPacket;
    USB_HUB_30_PORT_REMOTE_WAKE_MASK    remoteWakeMask;

    TRY {

        remoteWakeMask.AsUchar8 = 0;
        
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wValue.W = PORT_REMOTE_WAKE_MASK;
        setupPacket->wIndex.HiByte = remoteWakeMask.AsUchar8; 
        setupPacket->wIndex.LowByte = (UCHAR)PortContext->PortInformation.PortNumber;
        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Disable Remote Wake On Port %d",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Disable Remote Wake On Port request failed - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}

VOID
HUBHTX_EnableRemoteWakeOnPortUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Enable the remote wake on port using control transfer. The completion 
    routine should queue a TransferSuccess or TransferFailure to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                            status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET      setupPacket;
    USB_HUB_30_PORT_REMOTE_WAKE_MASK    remoteWakeMask;

    TRY {

        remoteWakeMask.AsUchar8 = 0;
        remoteWakeMask.ConnectRemoteWakeEnable = 1;
        remoteWakeMask.DisconnectRemoteWakeEnable = 1;
        remoteWakeMask.OverCurrentRemoteWakeEnable = 1;
        
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wValue.W = PORT_REMOTE_WAKE_MASK;
        setupPacket->wIndex.HiByte = remoteWakeMask.AsUchar8; 
        setupPacket->wIndex.LowByte = (UCHAR)PortContext->PortInformation.PortNumber;
        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Enable Remote Wake On Port %d",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Enable Remote Wake On Port request failed - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}


VOID
HUBHTX_ResumePortUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Send control transfer to resume the port. Completion routine should
    queue a TransferSuccess or a TransferFailure.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        //
        // Format the setup packet for Clear Port Feature
        // USB 3.0 Spec, 10.14.2.2
        // USB 2.0 Spec, 11.24.2.2
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_CLEAR_FEATURE;
        setupPacket->wValue.W = PORT_SUSPEND;
        setupPacket->wIndex.W = PortContext->PortInformation.PortNumber;
        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Resume 2.0 Port %d",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);
    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Resume 2.0 Port request failed - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}


VOID
HUBHTX_SettingPortPowerUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Power on the port by sending control transfer. Completion routine should
    queue a TransferSuccess or a TransferFailure.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        //
        // If the port had an overecurrent condition, now is a good time
        // to reset the ConnectionStatus for the user mode tools as we 
        // are going to try to power on the port. Note that the port
        // might be powered on in other cases like resuming from hibernate
        // where a device is already connected. The status should not
        // be changed in that case.
        //
        if (PortContext->ConnectionStatus == DeviceCausedOvercurrent) {
            PortContext->ConnectionStatus = NoDeviceConnected;
        }

        //
        // Format the setup packet for Set Port Feature
        // USB 3.0 Spec, 10.14.2.10
        // USB 2.0 Spec, 11.24.2.13
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wValue.W = PORT_POWER;
        setupPacket->wIndex.W = PortContext->PortInformation.PortNumber;
        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Set Port %d Power",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Set Port Power request failed - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}

VOID
HUBHTX_SetU1TimeoutUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Set the U1 timeout on the downstream port as directed 
    by the DSM
    
Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        //
        // Format the setup packet for Set Port Feature
        // USB 3.0 Spec, 10.14.2.10
        // USB 2.0 Spec, 11.24.2.13
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wValue.W = PORT_U1_TIMEOUT;
        setupPacket->wIndex.HiByte = PortContext->DeviceContext->U1Timeout;
        setupPacket->wIndex.LowByte = (UCHAR)PortContext->PortInformation.PortNumber;

        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Set Port %d U1 Timeout - timeout 0x%x",
                   PortContext->PortInformation.PortNumber,
                   PortContext->DeviceContext->U1Timeout);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Set U1 timeout request error - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}

VOID
HUBHTX_SetU2TimeoutUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Set the U2 timeout on the downstream port as directed 
    by the DSM

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        //
        // Format the setup packet for Set Port Feature
        // USB 3.0 Spec, 10.14.2.10
        // USB 2.0 Spec, 11.24.2.13
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wValue.W = PORT_U2_TIMEOUT;
        setupPacket->wIndex.HiByte = PortContext->DeviceContext->U2Timeout;
        setupPacket->wIndex.LowByte = (UCHAR)PortContext->PortInformation.PortNumber;

        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Set Port %d U2 Timeout - timeout 0x%x",
                   PortContext->PortInformation.PortNumber,
                   PortContext->DeviceContext->U2Timeout);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Set U2 timeout request error - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}

VOID
HUBHTX_SetPortRemoteWakeMaskUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Power on the port by sending control transfer. Completion routine should
    queue a TransferSuccess or a TransferFailure.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        //
        // Format the setup packet for Set Port Feature
        // USB 3.0 Spec, 10.14.2.10
        // USB 2.0 Spec, 11.24.2.13
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wValue.W = PORT_REMOTE_WAKE_MASK;
        setupPacket->wIndex.LowByte = (UCHAR)PortContext->PortInformation.PortNumber;
        setupPacket->wIndex.HiByte = 0;

        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Set Port %d Remote Wake Mask",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Set Port Remote Wake Mask request failed - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}

VOID
HUBHTX_SuspendPortUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Suspend the port by sending a control transfer. Completion routine should
    queue a TransferSuccess or a TransferFailure.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        //
        // Format the setup packet for Set Port Feature
        // USB 3.0 Spec, 10.14.2.10
        // USB 2.0 Spec, 11.24.2.13
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wValue.W = PORT_SUSPEND;
        setupPacket->wIndex.W = PortContext->PortInformation.PortNumber;
        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Suspend 2.0 Port %d",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Suspend 2.0 Port request failed - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}

VOID
HUBHTX_Suspend30PortUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Suspend the port by sending a link state change control transfer. 
    Completion routine should queue a TransferSuccess or a TransferFailure.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {
        
        //
        // Format the setup packet for Set Port Feature
        // USB 3.0 Spec, 10.14.2.10
        // USB 2.0 Spec, 11.24.2.13
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wValue.W = PORT_LINK_STATE;
        setupPacket->wIndex.LowByte = (UCHAR)PortContext->PortInformation.PortNumber;
        //
        // HiByte indicates the link state. For suspend, 
        // we need to put the link into U3
        //
        setupPacket->wIndex.HiByte = 3;
        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Suspend 3.0 Port %d",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "U3 request failed - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}

VOID
HUBHTX_Resume30PortUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Resumes the port by sending a link state changecontrol transfer. 
    Completion routine should queue a TransferSuccess or a TransferFailure.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        //
        // Format the setup packet for Set Port Feature
        // USB 3.0 Spec, 10.14.2.10
        // USB 2.0 Spec, 11.24.2.13
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wValue.W = PORT_LINK_STATE;
        setupPacket->wIndex.LowByte = (UCHAR)PortContext->PortInformation.PortNumber;
        //
        // HiByte indicates the link state. For resuming, we need to put the
        // link into U0
        //
        setupPacket->wIndex.HiByte = 0;
        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Resume 3.0 Port %d",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Resume 3.0 port request failed - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}
PSM_EVENT
HUBHTX_Get20PortChangeEvent (
    __in PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    This functions looks at the 3 things that are stored in the port context:
    current port status bits, cumulative port status change bits and the
    previous port status bits. Based on these bits, it figures out if there
    is an effective change in the port status and generated a port event 
    for that change.

    If there is no such change, then it returns PsmEventPortNoChange

    If the function determines that these bits are just plain wrong,
    like if the power bit is off , then it will return PsmEventPortError.

Return Value:

    PsmEventPortResumed
    PsmEventPortResetComplete
    PsmEventPortOverCurrent
    PsmEventPortOverCurrentClear
    PsmEventPortConnectChange
    PsmEventPortError
    PsmEventPortNoChange

--*/
{
    PSM_EVENT               portEvent;
    USB_20_PORT_STATUS      currentPortStatus, previousPortStatus;
    USB_20_PORT_CHANGE      currentPortChange;
    PDEVICE_CONTEXT         deviceContext;
    USHORT                  idVendor, idProduct, bcdDevice;


    TRY {

        //
        // If there was a power reference acquired due to a initial pending
        // change, it should be released now as we are processing the change
        // now
        //
        if (PortContext->PowerRefAcquiredForInitialConnectChange) {
            WdfDeviceResumeIdle(PortContext->HubFdoContext->HubFdoDevice);
            PortContext->PowerRefAcquiredForInitialConnectChange = FALSE;
        }
        
        currentPortStatus = PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb20PortStatus;
        currentPortChange = PortContext->CumulativePortChange.Usb20PortChange;
        previousPortStatus = PortContext->PreviousPortStatus.Usb20PortStatus;
        PortContext->PreviousPortStatus.Usb20PortStatus = currentPortStatus;
        deviceContext = PortContext->DeviceContext;
        idVendor = (deviceContext != NULL) ? deviceContext->DeviceDescriptor.idVendor : 0;
        idProduct = (deviceContext != NULL) ? deviceContext->DeviceDescriptor.idProduct : 0;
        bcdDevice = (deviceContext != NULL) ? deviceContext->DeviceDescriptor.bcdDevice : 0;

        portEvent = PsmEventPortError;

        if (currentPortStatus.OverCurrent == 1) {
               
            HUBREG_UpdateSqmHubOvercurrentDetected(PortContext->HubFdoContext);
            
            portEvent = PsmEventPortOverCurrent;
            PortContext->ConnectionStatus = DeviceCausedOvercurrent;
            USB_HW_VERIFIER_PORT_BREAK(PortContext, HubHwVerifierPortOverCurrent);

            EventWriteUSBHUB3_ETW_EVENT_PORT_STATUS_ERROR_V1(
                &PortContext->HubFdoContext->CurrentActivityId,
                PortContext->PortInformation.PortNumber,
                previousPortStatus.AsUshort16,
                currentPortStatus.AsUshort16,
                currentPortChange.AsUshort16,
                portEvent,
                idVendor,
                idProduct,
                bcdDevice);

            LEAVE;
        }
        if (currentPortChange.OverCurrentIndicatorChange == 1)  {

            HUBREG_UpdateSqmHubOvercurrentDetected(PortContext->HubFdoContext);

            //
            // It is worth noting that OverCurrentClear event is not
            // being returned by this function in this scenario. At 
            // one point, this event was being returned and the state
            // machine would treat this event as an indication that the
            // over-current condition has been cleared and would proceed
            // to do an auto-recovery. However, it turns out that this
            // usage of this event is not correct. Once the over-current 
            // condition occurs and the hub has cut off power to the port, 
            // the over-curent gets cleared even if the device causing the
            // over-current is still connected. If the driver keeps retrying
            // to auto-recover, it can end up heating the faulty device
            // leading to bad things. Therefore, we decided to completely
            // get rid of this event and instead base the recovery action
            // on a time based heuristics. TODO: Remove the overcurrent clear
            // event from the state machines as well
            // Note that if we previously did not report an overcurrent, we
            // still need to let the state machine know about the overcurrent
            //
            if (previousPortStatus.OverCurrent == 0) {
                PortContext->ConnectionStatus = DeviceCausedOvercurrent;
                portEvent = PsmEventPortOverCurrent;
            }
            USB_HW_VERIFIER_PORT_BREAK(PortContext, HubHwVerifierPortOverCurrent);

            EventWriteUSBHUB3_ETW_EVENT_PORT_STATUS_ERROR_V1(
                &PortContext->HubFdoContext->CurrentActivityId,
                PortContext->PortInformation.PortNumber,
                previousPortStatus.AsUshort16,
                currentPortStatus.AsUshort16,
                currentPortChange.AsUshort16,
                portEvent,
                idVendor,
                idProduct,
                bcdDevice);

            LEAVE;
        }
        
        if (currentPortStatus.PortPower == 0) {
            
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port, 
                       "Returning PortError because port power is 0 and overcurrent is not indicated");
            //
            // Over Current Change bit is not 1 and Over Current is not 
            // indicated either. PortPower should not really be 0 now.
            //
            portEvent = PsmEventPortError;
            LEAVE;
        }
        
        if ((currentPortChange.ConnectStatusChange == 1) ||
            (currentPortStatus.CurrentConnectStatus != previousPortStatus.CurrentConnectStatus)) {
            portEvent = PsmEventPortConnectChange;
            LEAVE;
        }    
        
        
        if (currentPortStatus.Reset == 1) {

            DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR, 
                           Port, 
                           "Returning PortError because Reset bit is on");   
            portEvent = PsmEventPortError;
            LEAVE;
        }
        
        if (currentPortStatus.CurrentConnectStatus == 0) {
            

            if (currentPortStatus.PortEnabledDisabled == 1) {
                //
                // If connect bit is 0, Enabled should be 0 too
                //
                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port, 
                           "Returning PortError because Connect bit is 0 but Enable bit is 1");
                portEvent = PsmEventPortError;
                LEAVE;

            }
            
            if (currentPortStatus.Suspend == 1) {
                //
                // If connect bit is 0, suspend should be 0 too
                //
                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port, 
                           "Returning PortError because Connect bit is 0 but Suspend bit is 1");

                portEvent = PsmEventPortError;
                LEAVE;
            }

            //
            // Device is not connected and no errors. So no change
            //
            LEAVE;

        }

        if (currentPortChange.ResetChange == 1) {

            if (currentPortStatus.PortEnabledDisabled == 0) {
                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port, 
                           "Returning PortError because Enable bit is off after reset");

                portEvent = PsmEventPortError;
                LEAVE;                                       
            }
            
            if (currentPortStatus.Suspend == 1) {
                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port, 
                           "Returning PortError because Suspend bit is 1 after reset");

                portEvent = PsmEventPortError;
                LEAVE;
            }

            portEvent = PsmEventPortResetComplete;
            LEAVE;
        }

        if (currentPortChange.SuspendChange == 1) {

            if (currentPortStatus.PortEnabledDisabled == 0) {
                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port, 
                           "Returning PortError because Enable bit is off after resume");

                portEvent = PsmEventPortError;
                LEAVE;                                       
            }
            
            if (currentPortStatus.Suspend == 1) {
                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port, 
                           "Returning PortError because Suspend bit is 1 after resume");

                portEvent = PsmEventPortError;
                LEAVE;
            }

            portEvent = PsmEventPortResumed;
            LEAVE;
        }

        portEvent = PsmEventPortError;

    } FINALLY {        

        PortContext->CurrentPortEvent = portEvent;
    
        if ((portEvent == PsmEventPortConnectChange) &&
            (currentPortStatus.CurrentConnectStatus == 0)) {

            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port, 
                       "HUBHTX_Get20PortChangeEvent: PORT_STATUS_ERROR / HubHwVerifierPortDeviceDisconnected event: PortConnectChange & CCS=0; portNum %d, prevPS 0x%04x, curPS 0x%04x, curPC 0x%04x", 
                       PortContext->PortInformation.PortNumber,
                       previousPortStatus.AsUshort16,
                       currentPortStatus.AsUshort16,
                       currentPortChange.AsUshort16);

            USB_HW_VERIFIER_PORT_BREAK(PortContext, HubHwVerifierPortDeviceDisconnected);

            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port, 
                       "HUBHTX_Get20PortChangeEvent: Called HubHwVerifierPortDeviceDisconnected");

            EventWriteUSBHUB3_ETW_EVENT_PORT_STATUS_ERROR_V1(
                &PortContext->HubFdoContext->CurrentActivityId,
                PortContext->PortInformation.PortNumber,
                previousPortStatus.AsUshort16,
                currentPortStatus.AsUshort16,
                currentPortChange.AsUshort16,
                portEvent,
                idVendor,
                idProduct,
                bcdDevice);

        }
    }

    return portEvent;
}


PSM_EVENT
HUBHTX_CheckAndSelectIfAny20PortChangeSet (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:
    This function looks at the last port status change
    bits and checks if there is a change left that needs
    to be acknowlwdged, If it finds such a change, then
    it chooses the right feature selector, clears that bit
    from the port status change and transfers it to the
    cumulative change bits

Return Value:        

    PsmEventYes
    PsmEventNo
    PsmEventPortError
    
--*/
{
    PSM_EVENT                       retEvent;
    PUSB_20_PORT_CHANGE             currentPortChange;
    PUSB_20_PORT_CHANGE             cumulativePortChange;
    USB_PORT_FEATURE_SELECTOR       featureSelector;


    TRY {
        currentPortChange = &PortContext->CurrentExtPortStatusChange.PortStatusChange.PortChange.Usb20PortChange;
        cumulativePortChange = &PortContext->CumulativePortChange.Usb20PortChange;
        //
        // Ignore the change bits that are already set in the cumulative set
        // Otherwise a faulty hub could force us to keep clearing the same change
        // bit over and over again in a loop
        //
        currentPortChange->AsUshort16 &= (~(cumulativePortChange->AsUshort16));

        featureSelector  = 0;

        if (currentPortChange->ConnectStatusChange == 1) {

            featureSelector = C_PORT_CONNECTION;
            currentPortChange->ConnectStatusChange = 0;
            cumulativePortChange->ConnectStatusChange = 1;

        } else if (currentPortChange->OverCurrentIndicatorChange == 1) {

            featureSelector = C_PORT_OVER_CURRENT;
            currentPortChange->OverCurrentIndicatorChange = 0;
            cumulativePortChange->OverCurrentIndicatorChange = 1;
            PortContext->ConnectionStatus = DeviceCausedOvercurrent;

        } else if (currentPortChange->ResetChange == 1) {

            featureSelector = C_PORT_RESET;
            currentPortChange->ResetChange = 0;
            cumulativePortChange->ResetChange = 1;

        } else if (currentPortChange->SuspendChange == 1) {
            
            featureSelector = C_PORT_SUSPEND;
            currentPortChange->SuspendChange = 0;
            cumulativePortChange->SuspendChange = 1;

        } else if (currentPortChange->PortEnableDisableChange == 1) {

            featureSelector = C_PORT_ENABLE;
            currentPortChange->PortEnableDisableChange = 0;
            cumulativePortChange->PortEnableDisableChange = 1;

        } 

        if (featureSelector != 0) {

            PortContext->CurrentFeatureSelector =  featureSelector;
            retEvent = PsmEventYes;

        } else {

            if (currentPortChange->AsUshort16 != 0 ) {
                //
                // One of the port change bits is set
                // but we didn't find a valid port change
                // event. We will return error
                //
                retEvent = PsmEventPortError;

                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port, 
                           "Returning PsmEventPortError in HUBHTX_CheckAndSelectIfAny20PortChangeSet: One of the port change bits is set but we didn't find a valid port change event: currentPortChange = 0x%x",
                           currentPortChange->AsUshort16);
                
                USB_HW_VERIFIER_HUB_BREAK(PortContext->HubFdoContext, HubHwVerifierInvalidPortStatus);
                
            } else {

                retEvent = PsmEventNo;
            }

        }

    } FINALLY {

    }

    return retEvent;
}
PSM_EVENT
HUBHTX_CheckAndSelectIfAny30PortChangeSet (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:
    This function looks at the last port status change
    bits and checks if there is a change left that needs
    to be acknowlwdged, If it finds such a change, then
    it chooses the right feature selector, clears that bit
    from the port status change and transfers it to the
    cumulative change bits

Return Value:        

    PsmEventYes
    PsmEventNo
    PsmEventPortError
    
--*/
{
    PSM_EVENT                       retEvent;
    PUSB_30_PORT_CHANGE             currentPortChange;
    PUSB_30_PORT_CHANGE             cumulativePortChange;
    USB_PORT_FEATURE_SELECTOR       featureSelector;


    TRY {
        currentPortChange = &PortContext->CurrentExtPortStatusChange.PortStatusChange.PortChange.Usb30PortChange;
        cumulativePortChange = &PortContext->CumulativePortChange.Usb30PortChange;
        //
        // Ignore the change bits that are already set in the cumulative set
        // Otherwise a faulty hub could force us to keep clearing the same change
        // bit over and over again in a loop
        //
        currentPortChange->AsUshort16 &= (~(cumulativePortChange->AsUshort16));

        featureSelector  = 0;

        if (currentPortChange->ConnectStatusChange == 1) {

            featureSelector = C_PORT_CONNECTION;
            currentPortChange->ConnectStatusChange = 0;
            cumulativePortChange->ConnectStatusChange = 1;

        } else if (currentPortChange->OverCurrentIndicatorChange == 1) {

            featureSelector = C_PORT_OVER_CURRENT;
            currentPortChange->OverCurrentIndicatorChange = 0;
            cumulativePortChange->OverCurrentIndicatorChange = 1;
            PortContext->ConnectionStatus = DeviceCausedOvercurrent;

        } else if (currentPortChange->ResetChange == 1) {

            featureSelector = C_PORT_RESET;
            currentPortChange->ResetChange = 0;
            cumulativePortChange->ResetChange = 1;

        } else if (currentPortChange->PortLinkStateChange == 1) {
            
            featureSelector = C_PORT_LINK_STATE;
            currentPortChange->PortLinkStateChange = 0;
            cumulativePortChange->PortLinkStateChange = 1;

        } else if (currentPortChange->BHResetChange == 1) {

            featureSelector = C_BH_PORT_RESET;
            currentPortChange->BHResetChange = 0;
            cumulativePortChange->BHResetChange = 1;

        } else if (currentPortChange->PortConfigErrorChange == 1) {

            featureSelector = C_PORT_CONFIG_ERROR;
            currentPortChange->PortConfigErrorChange = 0;
            cumulativePortChange->PortConfigErrorChange = 1;
        }

        if (featureSelector != 0) {

            PortContext->CurrentFeatureSelector =  featureSelector;
            retEvent = PsmEventYes;

        } else {

            if (currentPortChange->AsUshort16 != 0 ) {
                //
                // One of the port change bits is set
                // but we didn't find a valid port change
                // event. We will return error
                //
                retEvent = PsmEventPortError;

                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port, 
                           "Returning PsmEventPortError in HUBHTX_CheckAndSelectIfAny30PortChangeSet: One of the port change bits is set but we didn't find a valid port change event: currentPortChange = 0x%x",
                           currentPortChange->AsUshort16);
                
                USB_HW_VERIFIER_HUB_BREAK(PortContext->HubFdoContext, HubHwVerifierInvalidPortStatus);
                
            } else {

                retEvent = PsmEventNo;
            }

        }

    } FINALLY {

    }

    return retEvent;
}

HSM_EVENT
HUBHTX_CheckAndSelectIfAnyHubChangeSet (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    This function looks at the hub change bits for  to figure 
    out if a change bit is set. If a change bit is set and 
    the status bits look consistent, then it sets the 
    CurrentHubEvent in the HubFdoContext and returns HsmEventYes.
    If no change bit is set, then it returns HsmEventNo.
    If a change bit is set but the status bits look invalid,
    then it returns HsmEventhubError. To determine the validity
    this functions looks at the 3 things that are stored in the 
    HubFdoContext: current hub status bits, current hub status 
    change bits and the previous hub status bits. 

Return Value:        

    HsmEventYes
    HsmEventNo
    HsmEventHubError
    
--*/
{
    HSM_EVENT                       hsmEvent;
    HSM_EVENT                       retEvent;
    USB_HUB_CHANGE                  hubChange;
    USB_HUB_STATUS                  hubStatus;


    TRY {
        hubStatus = HubFdoContext->HubStatusAndChange.HubStatus;
        hubChange = HubFdoContext->HubStatusAndChange.HubChange;

        hsmEvent = HsmEventNull;

        if (hubChange.OverCurrentChange == 1) {

            HUBREG_UpdateSqmHubOvercurrentDetected(HubFdoContext);

            if (hubStatus.OverCurrent == 1) {

                hsmEvent = HsmEventHubOverCurrent;
            
            } else {
                hsmEvent = HsmEventHubOverCurrentCleared;
            }
            HubFdoContext->CurrentFeatureSelector = C_HUB_OVER_CURRENT;

        } else if (hubChange.LocalPowerChange == 1) {

            if (hubStatus.LocalPowerLost == 1) {
                hsmEvent = HsmEventHubLocalPowerLost;
            } else {
                hsmEvent = HsmEventHubLocalPowerGood;
            }
            HubFdoContext->CurrentFeatureSelector = C_HUB_LOCAL_POWER;

        } 

        //
        // It is possible that there is a change bit 
        // set but changeEvent is Null, In such case, 
        // we will return PortError
        // 
        //
        if (hsmEvent != HsmEventNull) {

            HubFdoContext->CurrentHubChangeEvent = hsmEvent;    
            retEvent = HsmEventYes;

        } else {

            if (hubChange.AsUshort16 != 0 ) {
                //
                // One of the change bits is set but we 
                // didn't find a valid hub change event. 
                // We will return error
                //
                retEvent = HsmEventHubError;

                DbgTraceTo(HubFdoContext->IfrLog,
                           TL_ERROR,
                           Hub, 
                           "Returning HsmEventHubError in HUBHTX_CheckAndSelectIfAnyHubChangeSet: One of the change bits is set but we didn't find a valid hub change event: hubChange = 0x%x",
                           hubChange.AsUshort16);

                USB_HW_VERIFIER_HUB_BREAK(HubFdoContext, HubHwVerifierInvalidPortStatus);
                
            } else {

                retEvent = HsmEventNo;
            }

        }


    } FINALLY {

    }

    return retEvent;
}

PSM_EVENT
HUBHTX_GetPortLostChangeEvent (
    __in PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    This functions looks at the 3 things that are stored in the port context:
    current port status bits, current port status change bits and the
    previous port status bits. Based on these bits, it figures out if there
    is a change in the port status for which the corresponding change bit is
    not on. If there is such a change, then it returns that change.

    If there is no such change, then it returns PsmEventOperationSuccess.


Return Value:

    PsmEventPortResumed
    PsmEventPortDisabled
    PsmEventPortResetComplete
    PsmEventPortOverCurrent
    PsmEventPortOverCurrentClear
    PsmEventPortConnectChange
    PsmEventPortError
    PsmEventPortOperationSuccess

--*/
{
    PSM_EVENT               portEvent;
    USB_20_PORT_STATUS      currentPortStatus, previousPortStatus;
    USB_20_PORT_CHANGE      currentPortChange;
    PDEVICE_CONTEXT         deviceContext;
    USHORT                  idVendor, idProduct, bcdDevice;
    NTSTATUS                status;


    TRY {

        currentPortStatus = PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb20PortStatus;
        currentPortChange = PortContext->CurrentExtPortStatusChange.PortStatusChange.PortChange.Usb20PortChange;
        previousPortStatus = PortContext->PreviousPortStatus.Usb20PortStatus;
        PortContext->PreviousPortStatus.Usb20PortStatus = currentPortStatus;
        deviceContext = PortContext->DeviceContext;
        idVendor = (deviceContext != NULL) ? deviceContext->DeviceDescriptor.idVendor : 0;
        idProduct = (deviceContext != NULL) ? deviceContext->DeviceDescriptor.idProduct : 0;
        bcdDevice = (deviceContext != NULL) ? deviceContext->DeviceDescriptor.bcdDevice : 0;

        portEvent = PsmEventOperationSuccess;

        if (currentPortStatus.CurrentConnectStatus != previousPortStatus.CurrentConnectStatus) {

            if (currentPortChange.ConnectStatusChange == 0) {

                portEvent = PsmEventPortConnectChange;
            } else {
                //
                // The design for the 2.0 ports is that if there is a connect change pending
                // during hub start, the change is not processed right away but simply wait for 
                // the hub to complete the interrupt transfer, which is the regular path for 
                // any changes that happen on the hub anyways. This allows the PSM to not duplicate 
                // the logic at two places. This works in general as long as the Interrupt transfer
                // is generated before the hub idles out. If the hub idles out before the
                // transfer is generated (which can happen in cases of slow wireless transports
                // for MA-USB), it was discovered that a number of hubs don't deal with it 
                // correctly. Rather than resuming right away and generating the interrupt
                // transfer, they simply lose the change. To account for such hubs, we tried
                // changing the design such that these changes are processed right away. 
                // However, that leads to a different compat issue as some hubs are not
                // accustomed to doing device enumeration right away after resume. Therefore
                // the only solution possible is to let the interrupt tranfer occur before
                // processing the change but let the hub not idle out in the meantime. 
                // In order to do this, we will acquire a power reference in such cases and
                // release them once the change is processed via the interrupt mechanism.
                // Note that ideally this logic should be in the state machines. However,
                // that would require significant design change, which is not worth the risk.
                // Therefore we are making this change outside the state machine.
                //
                status = WdfDeviceStopIdle(PortContext->HubFdoContext->HubFdoDevice, FALSE);
                if (NT_SUCCESS(status)) {
                    PortContext->PowerRefAcquiredForInitialConnectChange = TRUE;
                }
            }   
        } else if ((currentPortStatus.OverCurrent == 1) &&
                   (previousPortStatus.OverCurrent == 0) &&
                   (currentPortChange.OverCurrentIndicatorChange == 0)) {

            HUBREG_UpdateSqmHubOvercurrentDetected(PortContext->HubFdoContext);
                       
            PortContext->ConnectionStatus = DeviceCausedOvercurrent;
            portEvent = PsmEventPortOverCurrent;
            USB_HW_VERIFIER_PORT_BREAK(PortContext, HubHwVerifierPortOverCurrent);

            EventWriteUSBHUB3_ETW_EVENT_PORT_STATUS_ERROR_V1(
                &PortContext->HubFdoContext->CurrentActivityId,
                PortContext->PortInformation.PortNumber,
                previousPortStatus.AsUshort16,
                currentPortStatus.AsUshort16,
                currentPortChange.AsUshort16,
                portEvent,
                idVendor,
                idProduct,
                bcdDevice);

        } else if ((currentPortStatus.OverCurrent == 0) &&
                   (previousPortStatus.OverCurrent == 1) &&
                   (currentPortChange.OverCurrentIndicatorChange == 0)) {
            
            HUBREG_UpdateSqmHubOvercurrentDetected(PortContext->HubFdoContext);

            PortContext->ConnectionStatus = NoDeviceConnected;
            portEvent = PsmEventPortOverCurrentCleared;
            USB_HW_VERIFIER_PORT_BREAK(PortContext, HubHwVerifierPortOverCurrent);

            EventWriteUSBHUB3_ETW_EVENT_PORT_STATUS_ERROR_V1(
                &PortContext->HubFdoContext->CurrentActivityId,
                PortContext->PortInformation.PortNumber,
                previousPortStatus.AsUshort16,
                currentPortStatus.AsUshort16,
                currentPortChange.AsUshort16,
                portEvent,
                idVendor,
                idProduct,
                bcdDevice);

        } else if ((currentPortStatus.PortEnabledDisabled == 0) &&
                   (previousPortStatus.PortEnabledDisabled == 1) &&
                   (currentPortChange.PortEnableDisableChange == 0)) {

            portEvent = PsmEventPortDisabled;

        } else if ((currentPortStatus.Suspend == 0) &&
                   (previousPortStatus.Suspend == 1) &&
                   (currentPortChange.SuspendChange == 0)) {

            portEvent = PsmEventPortResumed;
        }

    } FINALLY {

        PortContext->CurrentPortEvent = portEvent;

        if ((portEvent == PsmEventPortConnectChange) &&
            (currentPortStatus.CurrentConnectStatus == 0)) {

            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port, 
                       "HUBHTX_GetPortLostChangeEvent: PORT_STATUS_ERROR / HubHwVerifierPortDeviceDisconnected event: PortConnectChange & CCS=0; portNum %d, prevPS 0x%04x, curPS 0x%04x, curPC 0x%04x", 
                       PortContext->PortInformation.PortNumber,
                       previousPortStatus.AsUshort16,
                       currentPortStatus.AsUshort16,
                       currentPortChange.AsUshort16);

            USB_HW_VERIFIER_PORT_BREAK(PortContext, HubHwVerifierPortDeviceDisconnected);

            EventWriteUSBHUB3_ETW_EVENT_PORT_STATUS_ERROR_V1(
                &PortContext->HubFdoContext->CurrentActivityId,
                PortContext->PortInformation.PortNumber,
                previousPortStatus.AsUshort16,
                currentPortStatus.AsUshort16,
                currentPortChange.AsUshort16,
                portEvent,
                idVendor,
                idProduct,
                bcdDevice);

        }

    }

    return portEvent;
}

VOID
HUBHTX_ClearCurrentPortStatus (
    __in PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    This functions clears previous port changes

Return Value:

    VOID

--*/
{

    RtlZeroMemory(&PortContext->PreviousPortStatus,
                  sizeof(PortContext->PreviousPortStatus));

}




PSM_EVENT
HUBHTX_Get30PortChangeEvent (
    __in PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    This functions looks at the 3 things that are stored in the port context:
    current port status bits, cumulative port status change bits and the
    previous port status bits. Based on these bits, it figures out if there
    is an effective change in the port status and generated a port event 
    for that change.

    If there is no such change, then it returns PsmEventPortNoChange

    If the function determines that these bits are just plain wrong,
    like if the power bit is off , then it will return PsmEventPortResetHubOnError.

Return Value:

    PsmEventPortResumed
    PsmEventPortResetComplete
    PsmEventPortOverCurrent
    PsmEventPortOverCurrentClear
    PsmEventPortConnectChange
    PsmEventPortError
    PsmEventPortNoChange
    PsmEventPortResetHubOnError
    PsmEventPortLinkStateError
    PsmEventPortResetInProgress

--*/
{
    PSM_EVENT               portEvent;
    USB_30_PORT_STATUS      currentPortStatus, previousPortStatus;
    USB_30_PORT_CHANGE      currentPortChange;
    PDEVICE_CONTEXT         deviceContext;
    USHORT                  idVendor, idProduct, bcdDevice;


    TRY {

        //
        // If there is a reference acquired due to an initial pending change,
        // release the reference now as we are processing that change now
        // 
        if (PortContext->PowerRefAcquiredForInitialConnectChange) {
            WdfDeviceResumeIdle(PortContext->HubFdoContext->HubFdoDevice);
            PortContext->PowerRefAcquiredForInitialConnectChange = FALSE;
        }

        currentPortStatus = PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb30PortStatus;
        currentPortChange = PortContext->CumulativePortChange.Usb30PortChange;
        previousPortStatus = PortContext->PreviousPortStatus.Usb30PortStatus;
        PortContext->PreviousPortStatus.Usb30PortStatus = currentPortStatus;
        portEvent = PsmEventPortNoChange;
        deviceContext = PortContext->DeviceContext;
        idVendor = (deviceContext != NULL) ? deviceContext->DeviceDescriptor.idVendor : 0;
        idProduct = (deviceContext != NULL) ? deviceContext->DeviceDescriptor.idProduct : 0;
        bcdDevice = (deviceContext != NULL) ? deviceContext->DeviceDescriptor.bcdDevice : 0;

        //
        // The following parsing is based on the Section 10.3.1, 7.5 and 10.4.2.6
        // of the USB 30 Spec. We want to be strict but at the same time account
        // for reasonable error conditions. If we see something that just does not
        // make sense under any conditions, then we return PsmEventPortResetHubOnError
        //                     

        if (currentPortStatus.OverCurrent == 1) {
            
            HUBREG_UpdateSqmHubOvercurrentDetected(PortContext->HubFdoContext);
            
            portEvent = PsmEventPortOverCurrent;
            PortContext->ConnectionStatus = DeviceCausedOvercurrent;
            USB_HW_VERIFIER_PORT_BREAK(PortContext, HubHwVerifierPortOverCurrent);

            EventWriteUSBHUB3_ETW_EVENT_PORT_STATUS_ERROR_V1(
                &PortContext->HubFdoContext->CurrentActivityId,
                PortContext->PortInformation.PortNumber,
                previousPortStatus.AsUshort16,
                currentPortStatus.AsUshort16,
                currentPortChange.AsUshort16,
                portEvent,
                idVendor,
                idProduct,
                bcdDevice);

            LEAVE;
        }
        if (currentPortChange.OverCurrentIndicatorChange == 1)  {

            HUBREG_UpdateSqmHubOvercurrentDetected(PortContext->HubFdoContext);

            //
            // It is worth noting that OverCurrentClear event is not
            // being returned by this function in this scenario. At 
            // one point, this event was being returned and the state
            // machine would treat this event as an indication that the
            // over-current condition has been cleared and would proceed
            // to do an auto-recovery. However, it turns out that this
            // usage of this event is not correct. Once the over-current 
            // condition occurs and the hub has cut off power to the port, 
            // the over-curent gets cleared even if the device causing the
            // over-current is still connected. If the driver keeps retrying
            // to auto-recover, it can end up heating the faulty device
            // leading to bad things. Therefore, we decided to completely
            // get rid of this event and instead base the recovery action
            // on a time based heuristics. TODO: Remove the overcurrent clear
            // event from the state machines as well
            // Note that if we previously did not report an overcurrent, we
            // still need to let the state machine know about the overcurrent
            //
            //
            if (previousPortStatus.OverCurrent == 0) {
                PortContext->ConnectionStatus = DeviceCausedOvercurrent;
                portEvent = PsmEventPortOverCurrent;
            }
            
            USB_HW_VERIFIER_PORT_BREAK(PortContext, HubHwVerifierPortOverCurrent);

            EventWriteUSBHUB3_ETW_EVENT_PORT_STATUS_ERROR_V1(
                &PortContext->HubFdoContext->CurrentActivityId,
                PortContext->PortInformation.PortNumber,
                previousPortStatus.AsUshort16,
                currentPortStatus.AsUshort16,
                currentPortChange.AsUshort16,
                portEvent,
                idVendor,
                idProduct,
                bcdDevice);

            LEAVE;
        }
        if (currentPortStatus.PortPower == 0) {
            
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port, 
                       "HW_COMPLIANCE:Initiating Hub Reset because port power is 0 and overcurrent is not indicated");
            //
            // Over Current Change bit is not 1 and Over Current is not 
            // indicated either. PortPower should not really be 0 now.
            //
            portEvent = PsmEventPortResetHubOnError;
            LEAVE;
        }
        
        
        if (currentPortStatus.Reset == 1) {  
            //
            // This is an interesting one. We caught the port in DSPORT.Resetting 
            // state i.e. when it was in the middle of a reset operation. Normally 
            // this should never happen as  we don't expect to get a change until 
            // after reset is complete. But it could be that we are just starting 
            // up and a propogated reset was in progress. So we will let the state 
            // machine decide if this is a valid event or not. Port Link State is
            // undefined in this state but we do expect the port enable to be 0
            //
            if (currentPortStatus.PortEnabledDisabled == 1) {
                if (PortContext->HubFdoContext->HubInformation.HubFlags.IgnoreEnabledInReset == 1) {

                    DbgTraceTo(PortContext->IfrLog,
                               TL_ERROR, 
                               Port, 
                               "HW_COMPLIANCE:Enable bit is 1 and Reset bit is 1. Ignoring error on port as the hub flag is set");

                } else {
                    DbgTraceTo(PortContext->IfrLog,
                               TL_ERROR,
                               Port, 
                               "HW_COMPLIANCE:Initiating Hub Reset because Enable bit is on when port is resetting");

                    portEvent = PsmEventPortResetHubOnError;
                    LEAVE;

                }
            }

            if (currentPortStatus.CurrentConnectStatus == 1) {
                portEvent = PsmEventPortResetInProgress;
                LEAVE;
            }

            if ((currentPortChange.ConnectStatusChange == 1) ||
                (currentPortStatus.CurrentConnectStatus != previousPortStatus.CurrentConnectStatus)) {

                //
                // This is valid in the case where a device got disconnected and
                // then at the same time, a reset was being propagated from
                // upstream.
                //
                portEvent = PsmEventPortConnectChange;
                LEAVE;
            }
            
            if ((currentPortChange.ConnectStatusChange == 1) ||
                (currentPortStatus.CurrentConnectStatus != previousPortStatus.CurrentConnectStatus)) {

                //
                // This is valid in the case where a device got disconnected and
                // then at the same time, a reset was being propagated from
                // upstream.
                //
                portEvent = PsmEventPortConnectChange;
                LEAVE;
            }
            
            //
            // Combination of Connect 0 and Reset bit 1 can be observed
            // during the initial phase of device connection. So we will
            // just ignore this state as we will get a final connect
            // change when the connection process completes.Also, there
            // are some hubs that have a problem where if reset is issued
            // in disconnected state, the reset bit gets stuck. In any
            // case, we shall ignore this condition as we can't say for
            // sure whether it is a bug or not
            //
            LEAVE;


        }
        
        if ((currentPortStatus.CurrentConnectStatus == 0) &&
            (currentPortStatus.PortLinkState == PORT_LINK_STATE_RX_DETECT) &&
            (previousPortStatus.PortPower == 1) &&
            (previousPortStatus.PortLinkState != PORT_LINK_STATE_RX_DETECT)) {

            //
            // This one is pretty interesting. When a 30 device is disconnecting,
            // the link first goes to Inactive and then to RxDetect. If we 
            // happen to catch it in Inactive state, then we would think that
            // a device is connected but in error state. Therefore if the 
            // link subsequently transitions to Rx.Detect, then we need to
            // correct our earlier interpretation by generating a connect
            // change
            //
            portEvent = PsmEventPortConnectChange;
            LEAVE;
        }
        
        if ((currentPortStatus.CurrentConnectStatus == 0) &&
            (currentPortStatus.PortLinkState == PORT_LINK_STATE_POLLING) &&
            (previousPortStatus.PortPower == 1) &&
            (previousPortStatus.PortLinkState != PORT_LINK_STATE_POLLING)) {

            //
            // Same as the above case. However for certain controllers the link 
            // ends up in Polling instead of Rx.Detect. It is theoritically 
            // possible that this is now a new device instance and we happen to
            // catch it in the initial polling state. However, most likely it
            // means that the link just ended up in Polling instead of Rx.Detect
            // after a disconnect. We have seen this issue on systems that have
            // something called a re-driver in them.
            //
            
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port, 
                       "HW_COMPLIANCE:Port is found to be in polling state after a disconnect (CCS=0,PP=1)");

            portEvent = PsmEventPortConnectChange;
            LEAVE;
        }

        if (currentPortStatus.PortLinkState == PORT_LINK_STATE_TEST_MODE) {
            
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port, 
                       "HW_COMPLIANCE:Initiating Hub Reset because port link state is 0x%x", 
                       currentPortStatus.PortLinkState);

            //
            // If the port power is on, then we never expect the link to be
            // in these states. It isn't possible to recover from these
            // states by a port reset
            //
            portEvent = PsmEventPortResetHubOnError;
            LEAVE;
        }

        //
        // If it is in disabled, we should have done that. If it goes
        // on its own, then it is an error
        //
        if ((currentPortStatus.PortLinkState == PORT_LINK_STATE_DISABLED) &&
            (previousPortStatus.PortLinkState != PORT_LINK_STATE_DISABLED)) {
            
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port, 
                       "HW_COMPLIANCE:Initiating Hub Reset because port link state is 0x%x", 
                       currentPortStatus.PortLinkState);

            //
            // If the port power is on, then we never expect the link to be
            // in these states. It isn't possible to recover from these
            // states by a port reset
            //
            portEvent = PsmEventPortResetHubOnError;
            LEAVE;
        }

        if (currentPortStatus.PortLinkState == PORT_LINK_STATE_COMPLIANCE_MODE) {

            //
            // COMPLIANCE_MODE can be valid if this is a root port and we 
            // are resuming from low power system state
            //
            if ((WdfDeviceGetSystemPowerAction(WdfObjectContextGetObject(PortContext->HubFdoContext)) == PowerActionNone) ||
                (PortContext->HubFdoContext->HubParentInterface.HubDepth > 0)) {

                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port, 
                           "HW_COMPLIANCE:Initiating port Reset because port link state is 0x%x with Connect bit 0x%x",
                           currentPortStatus.PortLinkState,
                           currentPortStatus.CurrentConnectStatus);
                USB_HW_VERIFIER_PORT_BREAK(PortContext, HubHwVerifierPortLinkStateCompliance);

                EventWriteUSBHUB3_ETW_EVENT_PORT_STATUS_ERROR_V1(
                    &PortContext->HubFdoContext->CurrentActivityId,
                    PortContext->PortInformation.PortNumber,
                    previousPortStatus.AsUshort16,
                    currentPortStatus.AsUshort16,
                    currentPortChange.AsUshort16,
                    portEvent,
                    idVendor,
                    idProduct,
                    bcdDevice);

            } else {

                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port, 
                           "Initiating port Reset because port link state is 0x%x with Connect bit 0x%x",
                           currentPortStatus.PortLinkState,
                           currentPortStatus.CurrentConnectStatus);
            }
            

            //
            // These are the states which indicate that there is a device
            // downstream but it failed one of the early stages of SuperSpeed
            // connection process. Hopefully we can get the device to show
            // up by resetting the port
            //

            portEvent = PsmEventPortLinkStateError;
            LEAVE;
        }

        if (currentPortStatus.PortLinkState == PORT_LINK_STATE_INACTIVE) {
            
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port, 
                       "HW_COMPLIANCE:Initiating port Reset because port link state is 0x%x (PORT_LINK_STATE_INACTIVE) with Connect bit 0x%x",
                       currentPortStatus.PortLinkState,
                       currentPortStatus.CurrentConnectStatus);

            USB_HW_VERIFIER_PORT_BREAK(PortContext, HubHwVerifierPortLinkStateSSInactive);

            EventWriteUSBHUB3_ETW_EVENT_PORT_STATUS_ERROR_V1(
                &PortContext->HubFdoContext->CurrentActivityId,
                PortContext->PortInformation.PortNumber,
                previousPortStatus.AsUshort16,
                currentPortStatus.AsUshort16,
                currentPortChange.AsUshort16,
                portEvent,
                idVendor,
                idProduct,
                bcdDevice);

            //
            // If the CEC is set, it indicates that there is a new transition to
            // SS.Inactive after the link went to U0. Since this could be a new 
            // device, it should be treated as a new instance and therefore we need 
            // to cycle port. This logic particularly impacts the debug scenario where
            // the link might go to SS.Inactive while the debug is being set up and
            // the target system has not been rebooted yet
            //
            if (currentPortChange.PortConfigErrorChange == 1) {

                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port, 
                           "HW_COMPLIANCE:Reporting a connect change event because the Config Error Change bit is set");

                portEvent = PsmEventPortConnectChange;
                LEAVE; 
            }

            //
            // If we have already reported the link state error in the past,
            // then we don't need to report it again. This check is only 
            // aimed at working around a hardware issue where the controller
            // keeps generating a new change for the link in error state.
            //
            if (PortContext->CurrentPortEvent == PsmEventPortLinkStateError) {
                DbgTraceTo(PortContext->IfrLog,
                           TL_WARNING,
                           Port, 
                           "Ignoring port link state error(with connect bit 0x%x because it has already been reported",
                           currentPortStatus.CurrentConnectStatus);
                LEAVE;
            }

            if (currentPortStatus.PortEnabledDisabled == 1) {
                if (PortContext->HubFdoContext->HubInformation.HubFlags.IgnoreEnabledInSSInactive) {
                    DbgTraceTo(PortContext->IfrLog,
                               TL_ERROR,
                               Port, 
                               "HW_COMPLIANCE:Enable bit is 1 while link is in SS.Inactive. Ignoring this error because hack flag is set");
                } else {
                
                    DbgTraceTo(PortContext->IfrLog,
                               TL_ERROR,
                               Port, 
                               "HW_COMPLIANCE:Initiating Hub Reset because Enable bit is on when link is in SS.Inactive");

                    portEvent = PsmEventPortResetHubOnError;
                    LEAVE; 
                }
            }
            //
            // These are the states which indicate that there is a device
            // downstream but it failed one of the early stages of SuperSpeed
            // connection process. Hopefully we can get the device to show
            // up by resetting the port
            //
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port, 
                       "HW_COMPLIANCE:Initiating port Reset because port link state is 0x%x with Connect bit 0x%x",
                       currentPortStatus.PortLinkState,
                       currentPortStatus.CurrentConnectStatus);

            portEvent = PsmEventPortLinkStateError;
            LEAVE;
        }

        if ((currentPortChange.ConnectStatusChange == 1) ||
            (currentPortStatus.CurrentConnectStatus != previousPortStatus.CurrentConnectStatus)) {
            portEvent = PsmEventPortConnectChange;
            LEAVE;
        }

        if (currentPortStatus.CurrentConnectStatus == 0) {
            
            if (currentPortStatus.PortEnabledDisabled == 1) {
                //
                // If connect bit is 0, Enabled should be 0 too
                //
                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port, 
                           "HW_COMPLIANCE:Initiating Hub Reset because connect bit is 0 but Enable bit is 1");

                portEvent = PsmEventPortResetHubOnError;
                LEAVE;
            }
            
            if ((currentPortStatus.PortLinkState >= PORT_LINK_STATE_U0) &&
                (currentPortStatus.PortLinkState <= PORT_LINK_STATE_U3)) {
                
                //
                // If the connect bit is 0, link has no business being in
                // a U state
                //
                if (PortContext->HubFdoContext->HubInformation.HubFlags.IgnoreU0InDisconnected == 1) {

                    DbgTraceTo(PortContext->IfrLog,
                               TL_ERROR, 
                               Port, 
                               "HW_COMPLIANCE:Connect bit is 0 and Port link state is 0x%x. Ignoring error on port as the hub flag is set",
                               currentPortStatus.PortLinkState);
                } else {
                    DbgTraceTo(PortContext->IfrLog,
                               TL_ERROR,
                               Port, 
                               "HW_COMPLIANCE:Initiating Hub Reset because connect bit is 0 and port link state is 0x%x",
                               currentPortStatus.PortLinkState);

                    portEvent = PsmEventPortResetHubOnError;
                    LEAVE;
                }
            }
            
            LEAVE;
        }

        //
        // We have seen instances where the link is incorrectly in Rx detect state even 
        // when there is a device connected to it. Specifically, this has been seen with
        // TI controller in the debugging scenario. It was observed that port reset
        // works better than hub/controller reset.
        //
        if (currentPortStatus.PortLinkState == PORT_LINK_STATE_RX_DETECT) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port, 
                       "HW_COMPLIANCE:Initiating port Reset because port link state is RX_DETECT even though connect is set to 1");
            
            portEvent = PsmEventPortLinkStateError;
            LEAVE;
        }
        
        //
        // If we have reached this point then the only state that we could be in
        // is DSPORT.Enabled state
        //                        
        if (currentPortStatus.PortEnabledDisabled == 0) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port, 
                       "HW_COMPLIANCE:Initiating Hub Reset because Enable bit is off when port is connected and not resetting");

            portEvent = PsmEventPortResetHubOnError;
            LEAVE;                                       
        }

        if ((currentPortStatus.PortLinkState > PORT_LINK_STATE_U3) &&
            (currentPortStatus.PortLinkState != PORT_LINK_STATE_RECOVERY)) {

            //
            // Only U0-U3 and Recovery are the valid link states in this state
            //
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port, 
                       "HW_COMPLIANCE:Initiating Hub Reset because device is connected enabled but the link state is 0x%x", 
                       currentPortStatus.PortLinkState);

            portEvent = PsmEventPortResetHubOnError;
            LEAVE;                                       
        }
        
        if (currentPortChange.ResetChange == 1) {
            if (currentPortStatus.PortLinkState == PORT_LINK_STATE_U3) {

                //
                // Reset completed with link in U3? This is not good.
                //
                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port, 
                           "HW_COMPLIANCE:Initiating Hub Reset because Reset completed but the link is still in U3");

                portEvent = PsmEventPortResetHubOnError;
                LEAVE;
            }

            portEvent = PsmEventPortResetComplete;
            LEAVE;
        }

        if ((previousPortStatus.PortLinkState == PORT_LINK_STATE_U3) &&
            (currentPortStatus.PortLinkState != PORT_LINK_STATE_U3)) {
            portEvent = PsmEventPortResumed;
            LEAVE;
        }

    } FINALLY {

        if(portEvent == PsmEventPortLinkStateError) {
            (PortContext->TotalNumResets)++;
            if((PortContext->TotalNumResets % 100) == 0) {
                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port, 
                           "Reset Watchdog: Port 0x%p LinkStateError reset count is 0d %d.",
                           PortContext,
                           PortContext->TotalNumResets);
                
                USB_HW_VERIFIER_PORT_BREAK(PortContext, HubHwVerifierPortLinkStateErrorResetWatchdog);
            }
        }

        if (portEvent == PsmEventPortResetHubOnError) {
            
            USB_HW_VERIFIER_PORT_BREAK(PortContext, HubHwVerifierInvalidPortStatus);
            
            PortContext->HubFdoContext->HubFailureMsgId = HUB_FAILURE_INVALID_PORT_STATUS;
            
            EventWriteUSBHUB3_ETW_EVENT_HUB_RESET_DUE_TO_PORT_ERROR(
                0,
                PortContext->HubFdoContext->HubParentInterface.Hub, 
                PortContext->PortInformation.PortNumber);
            
            //
            // We will store the current port status value for SQM
            //
            PortContext->HubFdoContext->HubInformation.LastHubResetPortStatus.PortChange.Usb30PortChange.AsUshort16 = currentPortChange.AsUshort16;
            PortContext->HubFdoContext->HubInformation.LastHubResetPortStatus.PortStatus.Usb30PortStatus.AsUshort16 = currentPortStatus.AsUshort16;

        } else if ((portEvent == PsmEventPortConnectChange) &&
                   (currentPortStatus.CurrentConnectStatus == 0)) {

            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port, 
                       "HUBHTX_Get30PortChangeEvent: PORT_STATUS_ERROR / HubHwVerifierPortDeviceDisconnected event: PortConnectChange & CCS=0; portNum %d, prevPS 0x%04x, curPS 0x%04x, curPC 0x%04x", 
                       PortContext->PortInformation.PortNumber,
                       previousPortStatus.AsUshort16,
                       currentPortStatus.AsUshort16,
                       currentPortChange.AsUshort16);

            USB_HW_VERIFIER_PORT_BREAK(PortContext, HubHwVerifierPortDeviceDisconnected);

            EventWriteUSBHUB3_ETW_EVENT_PORT_STATUS_ERROR_V1(
                0,
                PortContext->PortInformation.PortNumber,
                previousPortStatus.AsUshort16,
                currentPortStatus.AsUshort16,
                currentPortChange.AsUshort16,
                portEvent,
                idVendor,
                idProduct,
                bcdDevice);

        }

        if (portEvent != PsmEventPortNoChange) {
            PortContext->CurrentPortEvent = portEvent;
        }
    }

    return portEvent;
}


VOID
HUBHTX_FixPortStateOnStartResume (
    __in PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    This function fixes port state structure for known hardware issues on hub
    start and resume from Sx.

Return Value:                

--*/
{
    USB_30_PORT_STATUS      currentPortStatus;
    
    TRY {

        currentPortStatus = PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb30PortStatus;

        if (PortContext->HubFdoContext->HubInformation.HubFlags.ConvertPollingToComplianceOnStart == 1) {
            if (currentPortStatus.PortLinkState == PORT_LINK_STATE_POLLING) {
                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR, 
                           Port, 
                           "HW_COMPLIANCE:Converting link state from Polling to Compliance as per the hardware flag");
                currentPortStatus.PortLinkState = PORT_LINK_STATE_COMPLIANCE_MODE;
            }
        }
        
    }

}

PSM_EVENT
HUBHTX_GetErrorResponseEventForInvalidPortStatusInPendingReset (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Looks at the port status and port status change bits and returns the
    appropriate error response. This function tries to match the behavior
    of the current USB stack for compatability. Therefore some of these
    actions might not seem logical here but they are for compat.

Return Value:

    Returns PortIgnoreError , PortResetHubOnError, PortCycleOnError or
    PortDisableCycleOnError.

--*/
{
    PSM_EVENT                       retEvent;
    USB_20_PORT_STATUS              portStatus;
    USB_20_PORT_CHANGE              currentPortChange;


    TRY {
        portStatus = PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb20PortStatus;
        currentPortChange = PortContext->CumulativePortChange.Usb20PortChange;


        retEvent = PsmEventPortIgnoreError;
        
        if (currentPortChange.ConnectStatusChange == 1) {

            if (portStatus.CurrentConnectStatus == 0) {
                retEvent = PsmEventPortCycleOnError; 
            }

            LEAVE;
        }

        if (currentPortChange.ResetChange == 1) {

            if (portStatus.CurrentConnectStatus == 0) {

                if (portStatus.PortEnabledDisabled == 0) {
                    
                    retEvent = PsmEventPortResetHubOnError;

                } else {

                    retEvent = PsmEventPortCycleOnError; 
                }

                LEAVE;

            }

            if (portStatus.Suspend == 1) {

                retEvent = PsmEventPortCycleOnError;
                LEAVE;
            
            }

            if (portStatus.OverCurrent == 1) {

                HUBREG_UpdateSqmHubOvercurrentDetected(PortContext->HubFdoContext);

                retEvent = PsmEventPortOverCurrent;
                PortContext->ConnectionStatus = DeviceCausedOvercurrent;
                USB_HW_VERIFIER_PORT_BREAK(PortContext, HubHwVerifierPortOverCurrent);

                LEAVE;

            }

        }

        if (currentPortChange.PortEnableDisableChange == 1) {

            if (portStatus.PortEnabledDisabled == 1) {

                retEvent = PsmEventPortEnabledWhileResetError;
            }

            LEAVE;
        }


    } FINALLY {
        DbgTraceTo(PortContext->IfrLog,
                   TL_ERROR,
                   Port, 
                   "HUBHTX_GetErrorResponseEventForInvalidPortStatusInPendingReset: PortConnectChange & CCS=0; portNum %d, PS 0x%08x, curPC 0x%08x", 
                   PortContext->PortInformation.PortNumber,
                   portStatus.AsUshort16,
                   currentPortChange.AsUshort16);
    }
    return retEvent;
}

PSM_EVENT
HUBHTX_GetGenericErrorResponseEventForInvalidPortStatus (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Looks at the port status and port status change bits and returns the
    appropriate error response. This function tries to match the behavior
    of the current USB stack for compatability. Therefore some of these
    actions might not seem logical here but they are for compat.

Return Value:

    Returns PortIgnoreError , PortResetHubOnError, PortCycleOnError or
    PortDisableCycleOnError.

--*/
{
    PSM_EVENT                       retEvent;
    USB_20_PORT_STATUS              portStatus;
    USB_20_PORT_CHANGE              currentPortChange;

    TRY {

        portStatus = PortContext->CurrentExtPortStatusChange.PortStatusChange.PortStatus.Usb20PortStatus;
        currentPortChange = PortContext->CumulativePortChange.Usb20PortChange;


        retEvent = PsmEventPortIgnoreError;
        
        if (currentPortChange.ResetChange == 1) {

            if (portStatus.PortEnabledDisabled == 1) {

                retEvent = PsmEventPortCycleOnError; 

                LEAVE;
            }

        }              


    } FINALLY {

    }
    return retEvent;
}

VOID
HUBHTX_GetRequestedPortStatusUsingControlTransfer (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Gets hub's port status as requested by the client driver by using 
    class-specific USB_REQUEST_GET_STATUS. Completion routine of this 
    function should queue a HsmEventTransferSuccess or HsmEventTransferFailure
    event to HSM.

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    PPORT_CONTEXT                   portContext;
    USHORT                          length;

    TRY {

        portContext = HubFdoContext->GetPortStatusTargetPortContext;

        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)HubFdoContext->HubControlRequest.Urb.SetupPacket;

        //
        // Format the setup packet for Get Hub Status
        // USB 3.0 Spec, 10.14.2.4
        // USB 2.0 Spec, 11.24.2.6
        //
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_DEVICE_TO_HOST;

        setupPacket->bRequest = USB_REQUEST_GET_STATUS;
        setupPacket->wIndex.W = portContext->PortInformation.PortNumber;
        if (portContext->PortInformation.PortFlags.PortFlagEnhancedSuperSpeed) {
            setupPacket->wValue.W = USB_STATUS_EXT_PORT_STATUS;
            length = sizeof(portContext->CurrentExtPortStatusChange);
        }
        else {
            setupPacket->wValue.W = USB_STATUS_PORT_STATUS;
            length = sizeof(portContext->CurrentExtPortStatusChange.PortStatusChange);
        }
        setupPacket->wLength = length; 

        DbgTraceTo(HubFdoContext->IfrLog,
                   TL_INFO,
                   Hub,
                   "Get Requested Port %d Status",
                   portContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(HubFdoContext,
                                         HubFdoContext->HubParentInterface.Hub,
                                         (PVOID)HubFdoContext,
                                         &HubFdoContext->HubControlRequest,
                                         HUBHTX_HubControlTransferComplete,
                                         &portContext->CurrentExtPortStatusChange,
                                         length,
                                         FALSE,
                                         HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Get Requested Port Status Request Failed %!STATUS!", status);

            HUBSM_AddHsmEvent(HubFdoContext, HsmEventTransferFailure);
        }
    }
}

VOID
HUBHTX_CompleteGetPortStatusWithPortStatusResult (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Gets hub's port 0 status using class-specific USB_REQUEST_GET_STATUS. Completion
    routine of this function should queue a HsmEventTransferSuccess or HsmEventTransferFailure
    event to HSM.

Return Value:

    VOID

--*/
{
    PUSB_PORT_STATUS_INFORMATION        userPortStatus;
    WDFMEMORY                           userPortStatusMemory;
    PULONG                              clientPortStatus;
    WDF_REQUEST_PARAMETERS              params;
    USB_PORT_STATUS_AND_CHANGE          portState;
    PPORT_CONTEXT                       portContext;
    NTSTATUS                            status;
    PIRP                                irp;
    PIO_STACK_LOCATION                  irpStack;
    ULONG                               ioControlCode;


    TRY {

        portContext = HubFdoContext->GetPortStatusTargetPortContext;
        portState = portContext->CurrentExtPortStatusChange.PortStatusChange;
        irp = WdfRequestWdmGetIrp(HubFdoContext->PdoGetPortStatusRequest);
        irpStack = IoGetCurrentIrpStackLocation(irp);
        ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

 
        //
        // The request for get port status could have either originated
        // from a client driver of a PDO or it could be from the user mode
        // They need to be handled differently
        //
        if (ioControlCode == IOCTL_USB_GET_PORT_STATUS) {

            status = WdfRequestRetrieveOutputMemory(HubFdoContext->PdoGetPortStatusRequest, &userPortStatusMemory);
            if (!NT_SUCCESS(status)) {
  
                DbgTraceTo(HubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to retrieve Output Buffer %!STATUS!", status);
                LEAVE;
            }
            userPortStatus = 
               (PUSB_PORT_STATUS_INFORMATION)WdfMemoryGetBuffer(userPortStatusMemory, NULL);

            userPortStatus->PortStatus.AsUshort16 = portState.PortStatus.AsUshort16;

            WdfRequestSetInformation(HubFdoContext->PdoGetPortStatusRequest, 
                                     sizeof(USB_PORT_STATUS_INFORMATION));
        
        } else if (ioControlCode == IOCTL_INTERNAL_USB_GET_PORT_STATUS) {
            PHUB_PDO_CONTEXT    pdoContext;

            WDF_REQUEST_PARAMETERS_INIT(&params);
            WdfRequestGetParameters(HubFdoContext->PdoGetPortStatusRequest,
                                   &params);

            status = STATUS_SUCCESS;
            clientPortStatus = params.Parameters.Others.Arg1;
            pdoContext = params.Parameters.Others.Arg2;

            //
            // Note, the CurrentConnectStatus and the PortEnabledDisabled bits 
            // are in the same position for 2.0 and 3.0 ports
            //        
            if (portState.PortStatus.Usb20PortStatus.CurrentConnectStatus) {
                *clientPortStatus |= USBD_PORT_CONNECTED;
            } else {
                status = STATUS_NO_SUCH_DEVICE;
            }

            if (portState.PortStatus.Usb20PortStatus.PortEnabledDisabled) {
                if (pdoContext->PdoFlags.ReportPortAsDisabled == 1) {
                    //
                    // If we unexpectedly lost power, then we should clear the USBD_PORT_ENABLED
                    // bit in order to encourage a reset (to maintain compat with Win7 stack).
                    //
                    INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &pdoContext->PdoFlags, ReportPortAsDisabled);
                } else {
                    *clientPortStatus |= USBD_PORT_ENABLED;
                }
            }

            if (portState.PortChange.Usb20PortChange.ConnectStatusChange) {
                status = STATUS_NO_SUCH_DEVICE;
            }
        }  else {
            
            NT_ASSERTMSG("Request with unknown IoControlCode received",
                         FALSE);
            
            status = STATUS_ADAPTER_HARDWARE_ERROR;
        }


    } FINALLY {

        WdfRequestComplete(HubFdoContext->PdoGetPortStatusRequest,
                           status);

    }

}

VOID
HUBHTX_CompleteGetPortStatusWithFailedStatus (
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        NTSTATUS            Status
    )
/*++

Routine Description:
    
    Fail the Pending Get Port status request with the 
    given failure code after zeroing out the port status
    bits for compatability with Win7 stack

Return Value:

    VOID

--*/
{
    WDF_REQUEST_PARAMETERS              params;
    PIRP                                irp;
    PIO_STACK_LOCATION                  irpStack;
    ULONG                               ioControlCode;


    TRY {

        irp = WdfRequestWdmGetIrp(HubFdoContext->PdoGetPortStatusRequest);
        irpStack = IoGetCurrentIrpStackLocation(irp);
        ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;
 
        //
        // The request for get port status could have either originated
        // from a client driver of a PDO or it could be from the user mode
        // If it is from the PDO, then zero out the port status bits for
        // compatability with Win7 stack behavior
        //
       
        if (ioControlCode == IOCTL_INTERNAL_USB_GET_PORT_STATUS) {
            
            WDF_REQUEST_PARAMETERS_INIT(&params);
            WdfRequestGetParameters(HubFdoContext->PdoGetPortStatusRequest,
                                   &params);

            * (PULONG)params.Parameters.Others.Arg1 = 0;
        
        } 

    } FINALLY {

        WdfRequestComplete(HubFdoContext->PdoGetPortStatusRequest,
                           Status);

    }

}

VOID
HUBHTX_CompleteGetPortStatusWithNoSuchDevice (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Gets hub's port 0 status using class-specific USB_REQUEST_GET_STATUS. Completion
    routine of this function should queue a HsmEventTransferSuccess or HsmEventTransferFailure
    event to HSM.

Return Value:

    VOID

--*/
{

    TRY {

        HUBHTX_CompleteGetPortStatusWithFailedStatus(HubFdoContext,
                                                     STATUS_NO_SUCH_DEVICE);         

    } FINALLY {


    }

}

VOID
HUBHTX_BootDevicePortStatusControlTransferComplete(
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

    Completion routine for control request to get 
    port status for a boot device. It simply signals
    the event that the  HUBHTX_GetPortStatusForBootDeviceAndCheckIfConnected
    is waiting on

Returns:

    VOID

--*/
{
    UNREFERENCED_PARAMETER(WdfRequest);
    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(CompletionParams);

    TRY {

        KeSetEvent((PKEVENT)Context,
                   IO_NO_INCREMENT,
                   FALSE);


    } FINALLY {

    }
}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBHTX_GetPortStatusForBootDevice (
    __in
        PDEVICE_CONTEXT                 DeviceContext,
    __out
        PUSB_PORT_STATUS_AND_CHANGE     PortStatusChange
    )
/*++

Routine Description:

    Gets port status for a boot device by sending a control 
    transfer using a pre-allocated wdf request.
    
Return Value:
    
    Returns true if the query succeeds and port comes up as
    connected. Otherwise returns false         

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    KEVENT                          completionEvent;


    TRY {

        RtlZeroMemory(PortStatusChange, sizeof(*PortStatusChange));

        HUBMISC_RequestReuse(DeviceContext->ControlRequestForBootDevice.WdfRequest);

        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)DeviceContext->ControlRequestForBootDevice.Urb.SetupPacket;

        //
        // Format the setup packet for Get Port Status
        // USB 3.0 Spec, 10.14.2.6
        // USB 2.0 Spec, 11.24.2.7
        //
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_DEVICE_TO_HOST;

        setupPacket->bRequest = USB_REQUEST_GET_STATUS;
        setupPacket->wValue.W = USB_STATUS_PORT_STATUS;
        setupPacket->wIndex.W = DeviceContext->PortContext->PortInformation.PortNumber;
        setupPacket->wLength = sizeof(*PortStatusChange);

        KeInitializeEvent(&completionEvent,
                  NotificationEvent,
                  FALSE);

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->HubFdoContext->HubParentInterface.Hub,
                                         &completionEvent,
                                         &DeviceContext->ControlRequestForBootDevice,
                                         HUBHTX_BootDevicePortStatusControlTransferComplete,
                                         PortStatusChange,
                                         sizeof(*PortStatusChange),
                                         FALSE,
                                         TRUE);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Get Port Status request for boot device failed - %!STATUS!", 
                       status);

            LEAVE;
        }
        
        HUBMISC_WaitForSignal(&completionEvent,
                              "Get Port Status For Boot Device",
                              WdfObjectContextGetObject(DeviceContext));

        status = WdfRequestGetStatus(DeviceContext->ControlRequestForBootDevice.WdfRequest);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Get Port Status request for boot device failed - %!STATUS!", 
                       status);

            LEAVE;
        }

    } FINALLY {
       
    }

    return status;
}


NTSTATUS
HUBHTX_GetClearTtBufferInfoForEndpoint(
    __in
        PDEVICE_CONTEXT                 DeviceContext,
    __in
        UCXENDPOINT                     UcxEndpoint,
    __in
        USHORT                          EndpointNumber,
    __out
        PUSB_HUB_CLEAR_TT_BUFFER_VALUE  ClearTtBufferValue
    )
/*++

Routine Description:

    Determines if an endpoint is a valid target for a CLEAR_TT_BUFFER command, 
    and if so, builds the info for the setup packet's wValue field.

Returns:

    NTSTATUS

--*/
{
    NTSTATUS                status;
    PINTERFACE_HANDLE       interfaceHandle;
    ULONG                   i;
    PDEVICE_CONFIGURATION   deviceConfiguration;

    TRY {

        status = STATUS_UNSUCCESSFUL;
        ClearTtBufferValue->AsUShort = 0;

        //
        // TTs are only used for low/full speed devices
        //
        if ((DeviceContext->UsbDeviceInfo.DeviceSpeed != UsbLowSpeed) &&
            (DeviceContext->UsbDeviceInfo.DeviceSpeed != UsbFullSpeed)) {
                LEAVE;
        }

        if (UcxEndpoint == DeviceContext->DefaultEndpoint) {
            status = STATUS_SUCCESS;

            ClearTtBufferValue->EndpointType = USB_ENDPOINT_TYPE_CONTROL;
            ClearTtBufferValue->DeviceAddress = LOWORD(DeviceContext->DeviceAddress);
            ClearTtBufferValue->EndpointNumber = EndpointNumber & 0x0F;
            LEAVE;
        }

        deviceConfiguration = DeviceContext->CurrentDeviceConfiguration;

        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &deviceConfiguration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            //
            // Walk the endpoint handles for this interface look for a matching UCX endpoint
            //
            for (i=0; i<interfaceHandle->NumberOfEndpoints; i++) {

                if (interfaceHandle->PipeHandle[i].UcxEndpoint == UcxEndpoint) {
                    PPIPE_HANDLE    pipeHandle;

                    pipeHandle = &interfaceHandle->PipeHandle[i];

                    //
                    // Skip periodic endpoints
                    //
                    switch (pipeHandle->EndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) {
                    case USB_ENDPOINT_TYPE_ISOCHRONOUS:
                    case USB_ENDPOINT_TYPE_INTERRUPT:
                        LEAVE;

                    case USB_ENDPOINT_TYPE_BULK:
                        ClearTtBufferValue->EndpointType = USB_ENDPOINT_TYPE_BULK;
                        break;

                    case USB_ENDPOINT_TYPE_CONTROL:
                        ClearTtBufferValue->EndpointType = USB_ENDPOINT_TYPE_CONTROL;
                        break;

                    default:
                        NT_ASSERTMSG("Invalid Endpoint Type for Clear TT Buffer", FALSE);
                        LEAVE;
                    }

                    ClearTtBufferValue->DeviceAddress = LOWORD(DeviceContext->DeviceAddress);
                    ClearTtBufferValue->EndpointNumber = EndpointNumber & 0x0F;

                    if (USB_ENDPOINT_DIRECTION_IN(EndpointNumber)) {
                        ClearTtBufferValue->Direction_1forIN = 1;
                    }

                    status = STATUS_SUCCESS;
                    LEAVE;
                }
            }
        }
    } FINALLY {

    }

    return status;
}

VOID
HUBHTX_ClearTtBufferControlTransferComplete(
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

    Completion routine for the clear TT buffer or reset TT control request.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PCLEAR_TT_BUFFER_CONTEXT        clearTtBufferContext;
    USBD_STATUS                     urbStatus;
    struct _URB_CONTROL_TRANSFER_EX *urb;

    UNREFERENCED_PARAMETER(Target);

    TRY {
        status = CompletionParams->IoStatus.Status;

        clearTtBufferContext = (PCLEAR_TT_BUFFER_CONTEXT)Context;

        urb = &clearTtBufferContext->ControlRequest.Urb;
        urbStatus = urb->Hdr.Status;

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(clearTtBufferContext->HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Clear TT Buffer or Reset TT Control Transfer Completed with Error %!STATUS!, URB Status %x",
                       status,
                       urbStatus);

            EventWriteUSBHUB3_ETW_EVENT_HUB_CONTROL_TRANSFER_ERROR(
                0,
                clearTtBufferContext->HubFdoContext->HubParentInterface.Hub, 
                0,
                urbStatus,
                status);
        }
        
        //
        // Inform UCX about completion 
        //
        clearTtBufferContext->HubFdoContext->HubControllerStackInterface.ClearTTBufferComplete(
                clearTtBufferContext->HubFdoContext->HubControllerStackInterface.Hub,
                clearTtBufferContext->UcxEndpoint);

        WdfObjectDelete(WdfRequest);
        ExFreePoolWithTag(clearTtBufferContext, USBHUB3_HUB_TAG);

    }
}


VOID
HUBHTX_ClearTTBuffer(
    __in
        USB_HUB_CONTEXT    HubContext,
    __in
        USB_DEVICE_CONTEXT UsbDeviceContext,
    __in
        UCXENDPOINT        UcxEndpoint,
    __in
        ULONG              EndpointNumber,
    __in
        ULONG              TTHubPortNumber
    )
/*++

Routine Description:

    Hub/Controller bus interface callback used to execute a CLEAR_TT_BUFFER command
    when a transfer is cancelled.  

Returns:

    NTSTATUS

--*/
{
    PHUB_FDO_CONTEXT                hubFdoContext;
    PDEVICE_CONTEXT                 deviceContext;
    NTSTATUS                        status;
    WDF_OBJECT_ATTRIBUTES           attributes;
    WDFREQUEST                      wdfRequest;
    PURB                            urb;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    USB_HUB_CLEAR_TT_BUFFER_VALUE   clearTtBufferValue;
    PCLEAR_TT_BUFFER_CONTEXT        clearTtBufferContext;
    PCONTROL_REQUEST                controlRequest;
    BOOLEAN                         resetTt;

    TRY {

        urb = NULL;
        wdfRequest = NULL;
        clearTtBufferContext = NULL;
        resetTt = FALSE;

        hubFdoContext = (PHUB_FDO_CONTEXT)HubContext;
        deviceContext = (PDEVICE_CONTEXT)UsbDeviceContext; 
        
        //
        // Verify we are on a TT hub
        //
        if (hubFdoContext->HubInformation.HubFlags.IsTtHub != 1) {
            NT_ASSERTMSG("Clear TT Buffer for non-TT hub", FALSE);
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        if (deviceContext == NULL) {
            NT_ASSERTMSG("Clear TT Buffer for NULL device context", FALSE);
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }
        
        //
        // Check device flags for skipping Clear TT buffer command
        //
        if (hubFdoContext->HubInformation.HubFlags.NoClearTTBufferOnCancel == 1) {

            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        //
        // Check device flags to use Reset TT command rather than Clear TT buffer command
        //
        if (hubFdoContext->HubInformation.HubFlags.ResetTTOnCancel == 1) {
            resetTt = TRUE;
        }

        WdfObjectReferenceWithTag(WdfObjectContextGetObject(deviceContext), 
                                  (PVOID)ClearTtBufferTag);

        status = HUBHTX_GetClearTtBufferInfoForEndpoint(deviceContext,
                                                        UcxEndpoint,
                                                        LOWORD(EndpointNumber),
                                                        &clearTtBufferValue);

        WdfObjectDereferenceWithTag(WdfObjectContextGetObject(deviceContext), 
                                    (PVOID)ClearTtBufferTag);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = hubFdoContext->HubFdoDevice;

        //
        // Allocate the Clear/Reset TT Buffer request
        //
        status = WdfRequestCreate(&attributes,
                                  WdfDeviceGetIoTarget(hubFdoContext->HubFdoDevice),
                                  &wdfRequest);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfRequestCreate for Clear/Reset TT Buffer Failed %!STATUS!",
                       status);
            LEAVE;
        }

        //
        // Allocate the context structure for Clear TT Buffer
        //
        clearTtBufferContext = 
            (PCLEAR_TT_BUFFER_CONTEXT)ExAllocatePoolWithTag(NonPagedPool,
                                                            sizeof(CLEAR_TT_BUFFER_CONTEXT), 
                                                            USBHUB3_HUB_TAG);

        if (clearTtBufferContext == NULL) {
            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "CLEAR_TT_BUFFER_CONTEXT Allocation Failed %!STATUS!",
                       status);
            LEAVE;
        }

        RtlZeroMemory(clearTtBufferContext, sizeof(CLEAR_TT_BUFFER_CONTEXT));

        clearTtBufferContext->UcxEndpoint = UcxEndpoint;
        clearTtBufferContext->HubFdoContext = hubFdoContext;

        controlRequest = &clearTtBufferContext->ControlRequest;
        controlRequest->WdfRequest = wdfRequest;

        //
        // Format the setup packet for Clear TT Buffer or Reset TT
        // USB 2.0 Spec, 11.24.2.3 Clear TT Buffer
        // USB 2.0 Spec, 11.24.2.9 Reset TT 
        //
        setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)controlRequest->Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        if (resetTt == FALSE) {
            setupPacket->bRequest = USB_REQUEST_CLEAR_TT_BUFFER;
            setupPacket->wValue.W = clearTtBufferValue.AsUShort;
        } else {
            setupPacket->bRequest = USB_REQUEST_RESET_TT;
            setupPacket->wValue.W = 0;
        }
        
        if (hubFdoContext->HubInformation.HubFlags.IsMultiTtHub == 1) {
            setupPacket->wIndex.W = LOWORD(TTHubPortNumber);
        } else {
            setupPacket->wIndex.W = 1;
        }
        
        setupPacket->wLength = 0;

        status = HUBMISC_ControlTransfer(hubFdoContext,
                                         hubFdoContext->HubParentInterface.Hub,
                                         clearTtBufferContext,
                                         controlRequest,
                                         HUBHTX_ClearTtBufferControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         hubFdoContext->HsmContext.NeedsForwardProgress);
    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (wdfRequest != NULL) {
                WdfObjectDelete(wdfRequest);
            }

            if (clearTtBufferContext != NULL) {
                ExFreePoolWithTag(clearTtBufferContext, USBHUB3_HUB_TAG);
            }

            //
            // Inform UCX about completion 
            //
            hubFdoContext->HubControllerStackInterface.ClearTTBufferComplete(
                hubFdoContext->HubControllerStackInterface.Hub,
                UcxEndpoint);
        }
    }
}

PSM_EVENT
HUBHTX_CheckIfPowerOnStartHackIsPresent (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:
    
    There are certains hubs that require 3.0 ports to be powered on
    by the software even though the spec requires them to be powered
    on by default. This function returns whether the hack flag is set
    for this port

Return Value:

    VOID

--*/
{
    PSM_EVENT                       retEvent;


    TRY {

        if (PortContext->HubFdoContext->HubInformation.HubFlags.PowerOnPortsOnStart == 1) {
            
            retEvent =  PsmEventYes;

        } else {

            retEvent = PsmEventNo;

        }

    } FINALLY {

    }

    return retEvent;
}

PSM_EVENT
HUBHTX_CheckIfDisableSuperSpeedHackFlagIsSet (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:
    
    There are certains hubs that we just don't want to
    support as they are too buggy. We will completely
    dsiable SuperSpeed for them
    
Return Value:

    VOID

--*/
{
    PSM_EVENT                       retEvent;


    TRY {

        if (PortContext->HubFdoContext->HubInformation.HubFlags.DisableSuperSpeed == 1) {
            
            retEvent =  PsmEventYes;

        } else {

            retEvent = PsmEventNo;

        }

    } FINALLY {

    }

    return retEvent;
}

VOID
HUBHTX_SetLinkStateToSSDisabledUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Disable the SS port by putting the link in SS.Disabled state

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        //
        // Format the setup packet for Set Port Feature
        // USB 3.0 Spec, 10.14.2.10
        // USB 2.0 Spec, 11.24.2.13
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wValue.W = PORT_LINK_STATE;
        setupPacket->wIndex.LowByte = (UCHAR)PortContext->PortInformation.PortNumber;
        //
        // HiByte indicates the link state. We need to put 
        // the link into SS.Dsiabled
        //
        setupPacket->wIndex.HiByte = PORT_LINK_STATE_DISABLED;
        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Set Link State of 3.0 Port %d to Disabled",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Set Link State to Disabled request failed - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}

VOID
HUBHTX_SetLinkStateToRxDetectUsingControlTransfer (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Enable the SS port by putting the link in Rx.Detect state

Return Value:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        //
        // Format the setup packet for Set Port Feature
        // USB 3.0 Spec, 10.14.2.10
        // USB 2.0 Spec, 11.24.2.13
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)PortContext->PortControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
        setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wValue.W = PORT_LINK_STATE;
        setupPacket->wIndex.LowByte = (UCHAR)PortContext->PortInformation.PortNumber;
        //
        // HiByte indicates the link state. We need to put 
        // the link into Rx.Detect
        //
        setupPacket->wIndex.HiByte = PORT_LINK_STATE_RX_DETECT;
        setupPacket->wLength = 0;

        DbgTraceTo(PortContext->IfrLog,
                   TL_INFO,
                   Port,
                   "Set Link State of 3.0 Port %d to Rx.Detect",
                   PortContext->PortInformation.PortNumber);

        status = HUBMISC_ControlTransfer(PortContext->HubFdoContext,
                                         PortContext->HubFdoContext->HubParentInterface.Hub,
                                         PortContext,
                                         &PortContext->PortControlRequest,
                                         HUBHTX_PortControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         PortContext->HubFdoContext->HsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Set Link State to Rx.Detect request failed - %!STATUS!",
                       status);

            PortContext->PsmContext.AddEvent(PortContext, PsmEventTransferFailure);
        }
    }
}

PSM_EVENT
HUBHTX_IncrementAndCheckIfOverCurrentCountExceededMax (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    This routine determines if the overcurrent condition
    on the port is a persistent one or a transient one.
    If there are more than a particular number of ovecurrent
    events within a particular amount of time, it is
    considered to be a persistent overcurrent for which
    it is not feasible to do auto recovery.

Return Value:

    PsmEventYes
    PsmEventNo

--*/
{
    LARGE_INTEGER       currentTime;
    LARGE_INTEGER       delta;
    HSM_EVENT           psmEvent;


    TRY {

        //
        // Start with assuming that we haven't exceeded the limit
        //
        psmEvent = PsmEventNo;

        KeQuerySystemTime(&currentTime);

        NT_ASSERT(PortContext->LastOvercurrentTimeStamp.QuadPart <= currentTime.QuadPart);

        delta.QuadPart = currentTime.QuadPart - PortContext->LastOvercurrentTimeStamp.QuadPart;
        PortContext->LastOvercurrentTimeStamp.QuadPart = currentTime.QuadPart;

        if (delta.QuadPart > WDF_ABS_TIMEOUT_IN_SEC(MINIMUM_TIME_BETWEEN_INDEPENDENT_OVERCURRENTS_IN_SEC)) {
            //
            // Sufficient time has elapsed since the last overcurrent
            // to assume that this is a new overcurrent as opposed to
            // a persistent overcurrent condition. 
            //
            PortContext->OverCurrentCount = 0;
        }                                          

        PortContext->OverCurrentCount++;

        if (PortContext->OverCurrentCount > MAXIMUM_OVERCURRENT_AUTO_RECOVER_COUNT) {
            psmEvent = PsmEventYes;
        }

    } FINALLY {

    }

    return psmEvent;
}

