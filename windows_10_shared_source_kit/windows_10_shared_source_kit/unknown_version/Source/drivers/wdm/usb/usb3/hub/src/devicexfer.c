/*++
Copyright (c) Microsoft Corporation

Module Name:

    devicexfer.c

Abstract:

    Control transfers for attached USB device and support routines.

Author:

Environment:

    Kernel mode only.

--*/

#include "pch.h"
#include "devicexfer.tmh"

extern CHAR UserModeFdoRequest[];

PPIPE_HANDLE
HUBDTX_GetMatchingInternalPipeHandle (
    __in
        PDEVICE_CONFIGURATION       DeviceConfiguration,
    __in 
        USBD_PIPE_HANDLE            UsbdPipeHandle
    )
/*++

Routine Description:

    Finds a matching PIPE_HANDLE in the current device configuration for a
    given UCXENDPOINT handle.

Returns:

    Matching PPIPE_HANDLE, or NULL if not found.

--*/
{
    PINTERFACE_HANDLE   interfaceHandle;
    PPIPE_HANDLE        pipeHandle;
    ULONG               i;

    TRY {

        pipeHandle = NULL;

        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &DeviceConfiguration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            //
            // Walk the endpoint handles for this interface and look for a matching pipe handle
            //
            for (i = 0; i < interfaceHandle->NumberOfEndpoints; i++) {

                if (interfaceHandle->PipeHandle[i].UsbdPipeHandle == UsbdPipeHandle) {

                    pipeHandle = &interfaceHandle->PipeHandle[i];
                    LEAVE;
                }
            }
        }

    } FINALLY {

    }

    return pipeHandle;
}
                       

VOID
HUBDTX_ControlTransferComplete(
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

    Completion routine for hub control transfer requests.

Returns:

    VOID

--*/
{
    NTSTATUS                            status;
    PDEVICE_CONTEXT                     deviceContext;
    USBD_STATUS                         urbStatus;
    DSM_EVENT                           dsmEvent;
    struct _URB_CONTROL_TRANSFER_EX     *urb;
    PBILLBOARD_INFO                     billboardInfo;

    UNREFERENCED_PARAMETER(Target);

    TRY {

        status = CompletionParams->IoStatus.Status;

        deviceContext = (PDEVICE_CONTEXT) Context;

        urb = &deviceContext->ControlRequest.Urb;
        urbStatus = urb->Hdr.Status;
        deviceContext->ControlRequest.NumberOfBytes = urb->TransferBufferLength;

        deviceContext->ControlRequest.UsbdFlags = 0;

        billboardInfo = deviceContext->BillboardInfo;

        if (!NT_SUCCESS(status)) {
            PUSB_DEFAULT_PIPE_SETUP_PACKET      setupPacket;
            
            //
            // We will start with the generic failure event
            // For particular transfers, we might override
            // it with a more specific failure
            //
            dsmEvent = DsmEventTransferFailure;
            
            setupPacket = 
                (PUSB_DEFAULT_PIPE_SETUP_PACKET) deviceContext->ControlRequest.Urb.SetupPacket;

            //
            // Store the last failed status. This might be used by the 
            // CompleteWithLastStatus to fail the client request with 
            // the correct failure status
            //
            //
            // COMPAT: Failure on SET_CONFIGURATION for a non-NULL configuration requires a URB status
            //         of USBD_STATUS_SET_CONFIG_FAILED.
            //
            if ((setupPacket->bRequest == USB_REQUEST_SET_CONFIGURATION) &&
                (setupPacket->wValue.W != 0)) {

                deviceContext->LastOperationNtStatus = STATUS_UNSUCCESSFUL;
                deviceContext->LastOperationUsbdStatus = USBD_STATUS_SET_CONFIG_FAILED;

            } else {
                deviceContext->LastOperationNtStatus = status;
                deviceContext->LastOperationUsbdStatus = urbStatus;
            }
            
            //
            // Record the specific transfer that failed so a problem description string
            // can be provided for Device Manager to display
            //
            if ((setupPacket->bRequest == USB_REQUEST_SET_SEL) &&
                (setupPacket->bmRequestType.Type == BMREQUEST_STANDARD)) {

                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Device control transfer for Set_Sel failed with error %!STATUS!, URB Status %x",
                           status,
                           urbStatus);
                USB_HW_VERIFIER_DEVICE_BREAK(deviceContext, DeviceHwVerifierSetSelFailure);

                deviceContext->EnumMsgId = USBENUM_SET_SEL_FAILURE;

                EventWriteUSBHUB3_ETW_EVENT_SET_SEL_FAILURE(
                    &deviceContext->DsmContext.CurrentActivityId,
                    deviceContext->UsbDevice, 
                    0,
                    urbStatus,
                    status);

                if (urbStatus == USBD_STATUS_STALL_PID) {
                    dsmEvent = DsmEventTransferStall;
                }
            
            } else if ((setupPacket->bRequest == USB_REQUEST_SET_FEATURE) &&
                       (setupPacket->wValue.W == USB_FEATURE_U1_ENABLE)) {
                
                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Device control transfer for Set U1 Enable failed with error %!STATUS!, URB Status %x",
                           status,
                           urbStatus);
                USB_HW_VERIFIER_DEVICE_BREAK(deviceContext, DeviceHwVerifierSetU1EnableFailure);

                EventWriteUSBHUB3_ETW_EVENT_SET_U1_ENABLE_FAILURE(
                    &deviceContext->DsmContext.CurrentActivityId,
                    deviceContext->UsbDevice, 
                    0,
                    urbStatus,
                    status);

            } else if ((setupPacket->bRequest == USB_REQUEST_SET_FEATURE) &&
                       (setupPacket->wValue.W == USB_FEATURE_U2_ENABLE)) {
                
                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Device control transfer for Set U2 Enable failed with error %!STATUS!, URB Status %x",
                           status,
                           urbStatus);
                USB_HW_VERIFIER_DEVICE_BREAK(deviceContext, DeviceHwVerifierSetU2EnableFailure);

                EventWriteUSBHUB3_ETW_EVENT_SET_U2_ENABLE_FAILURE(
                    &deviceContext->DsmContext.CurrentActivityId,
                    deviceContext->UsbDevice, 
                    0,
                    urbStatus,
                    status);

            } else if ((setupPacket->bRequest == USB_REQUEST_GET_DESCRIPTOR) &&
                       (setupPacket->bmRequestType.Type == BMREQUEST_STANDARD)) {
           
                //
                // It is ok for device to not support MS OS string descriptor or
                // the device qualifier descriptor
                //
                if (((setupPacket->wValue.HiByte == USB_STRING_DESCRIPTOR_TYPE) &&
                    (setupPacket->wValue.LowByte == OS_STRING_DESCRIPTOR_INDEX)) ||
                    (setupPacket->wValue.HiByte == USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE)) {
                } else {
                    USB_HW_VERIFIER_DEVICE_BREAK(deviceContext, DeviceHwVerifierControlTransferFailure);
                }
                
                switch (setupPacket->wValue.HiByte) {
                case USB_DEVICE_DESCRIPTOR_TYPE:
                    deviceContext->EnumMsgId = USBENUM_DEVICE_DESCRIPTOR_FAILURE;

                    DbgTraceTo(deviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE: Device control transfer for getting Device descriptor failed with error %!STATUS!, URB Status %x",
                               status,
                               urbStatus);

                    EventWriteUSBHUB3_ETW_EVENT_DEVICE_DESCRIPTOR_FAILURE(
                        &deviceContext->DsmContext.CurrentActivityId,
                        deviceContext->UsbDevice, 
                        0,
                        urbStatus,
                        status);

                    break;

                case USB_CONFIGURATION_DESCRIPTOR_TYPE:
                    deviceContext->EnumMsgId = USBENUM_CONFIG_DESCRIPTOR_FAILURE;

                    DbgTraceTo(deviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE: Device control transfer for getting Configuration descriptor failed with error %!STATUS!, URB Status %x",
                               status,
                               urbStatus);

                    EventWriteUSBHUB3_ETW_EVENT_CONFIG_DESCRIPTOR_FAILURE(
                        &deviceContext->DsmContext.CurrentActivityId,
                        deviceContext->UsbDevice, 
                        0,
                        urbStatus,
                        status);

                    break;

                case USB_BOS_DESCRIPTOR_TYPE:
                    deviceContext->EnumMsgId = USBENUM_BOS_DESCRIPTOR_FAILURE;

                    DbgTraceTo(deviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE: Device control transfer for getting BOS descriptor failed with error %!STATUS!, URB Status %x",
                               status,
                               urbStatus);

                    EventWriteUSBHUB3_ETW_EVENT_BOS_DESCRIPTOR_FAILURE(
                        &deviceContext->DsmContext.CurrentActivityId,
                        deviceContext->UsbDevice, 
                        0,
                        urbStatus,
                        status);

                    break;

                case USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE:
                    deviceContext->EnumMsgId = USBENUM_DEVICE_QUALIFIER_DESCRIPTOR_FAILURE;
                    DbgTraceTo(deviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE: Device control transfer for getting Device Qualifier descriptor failed with error %!STATUS!, URB Status %x",
                               status,
                               urbStatus);


                    EventWriteUSBHUB3_ETW_EVENT_DEVICE_QUALIFIER_DESCRIPTOR_FAILURE(
                        &deviceContext->DsmContext.CurrentActivityId,
                        deviceContext->UsbDevice, 
                        0,
                        urbStatus,
                        status);

                    break;

                case USB_STRING_DESCRIPTOR_TYPE:
                    if ((deviceContext->DeviceDescriptor.iSerialNumber != 0) &&
                        (deviceContext->DeviceDescriptor.iSerialNumber == 
                         setupPacket->wValue.LowByte)) {
                        
                        deviceContext->EnumMsgId = USBENUM_SERIAL_NUMBER_STRING_FAILURE;
                        DbgTraceTo(deviceContext->PortContext->IfrLog,
                                   TL_ERROR,
                                   Device,
                                   "HW_COMPLIANCE: Device control transfer for getting Serial Number String descriptor failed with error %!STATUS!, URB Status %x",
                                   status,
                                   urbStatus);

                        EventWriteUSBHUB3_ETW_EVENT_SERIAL_NUMBER_STRING_FAILURE(
                            &deviceContext->DsmContext.CurrentActivityId,
                            deviceContext->UsbDevice, 
                            0,
                            urbStatus,
                            status);

                    } else if (setupPacket->wValue.LowByte == 0) {
                    
                        deviceContext->EnumMsgId = USBENUM_LANGUAGE_ID_STRING_FAILURE;
                        DbgTraceTo(deviceContext->PortContext->IfrLog,
                                   TL_ERROR,
                                   Device,
                                   "HW_COMPLIANCE: Device control transfer for getting String descriptor failed with error %!STATUS!, URB Status %x",
                                   status,
                                   urbStatus);

                    
                        EventWriteUSBHUB3_ETW_EVENT_LANGUAGE_ID_STRING_FAILURE(
                            &deviceContext->DsmContext.CurrentActivityId,
                            deviceContext->UsbDevice, 
                            0,
                            urbStatus,
                            status);

                    } else if ((deviceContext->DeviceDescriptor.iProduct != 0) && 
                               (deviceContext->DeviceDescriptor.iProduct == 
                                setupPacket->wValue.LowByte)) {

                        deviceContext->EnumMsgId = USBENUM_PRODUCT_ID_STRING_FAILURE;
                        DbgTraceTo(deviceContext->PortContext->IfrLog,
                                   TL_ERROR,
                                   Device,
                                   "HW_COMPLIANCE: Device control transfer for getting Product Id String descriptor failed with error %!STATUS!, URB Status %x",
                                   status,
                                   urbStatus);

                        EventWriteUSBHUB3_ETW_EVENT_PRODUCT_ID_STRING_FAILURE(
                            &deviceContext->DsmContext.CurrentActivityId,
                            deviceContext->UsbDevice, 
                            0,
                            urbStatus,
                            status);

                    } else if ((billboardInfo != NULL) &&
                               (billboardInfo->BillboardDescriptor->iAddtionalInfoURL != 0) && 
                               (billboardInfo->BillboardDescriptor->iAddtionalInfoURL == 
                                setupPacket->wValue.LowByte)) {

                        deviceContext->EnumMsgId = USBENUM_PRODUCT_ID_STRING_FAILURE;
                        DbgTraceTo(deviceContext->PortContext->IfrLog,
                                   TL_ERROR,
                                   Device,
                                   "HW_COMPLIANCE: Device control transfer for getting Billboard AdditionalInfoUrl String descriptor failed with error %!STATUS!, URB Status %x",
                                   status,
                                   urbStatus);

                        EventWriteUSBHUB3_ETW_EVENT_BILLBOARD_STRING_FAILURE(
                            &deviceContext->DsmContext.CurrentActivityId,
                            deviceContext->UsbDevice, 
                            0,
                            urbStatus,
                            status);

                    } else if ((billboardInfo != NULL) &&
                               (billboardInfo->CurrentAlternateMode != INVALID_ALTERNATE_MODE) &&
                               (billboardInfo->BillboardDescriptor->AlternateMode[billboardInfo->CurrentAlternateMode].iAlternateModeSetting != 0) && 
                               (billboardInfo->BillboardDescriptor->AlternateMode[billboardInfo->CurrentAlternateMode].iAlternateModeSetting == 
                                setupPacket->wValue.LowByte)) {

                        deviceContext->EnumMsgId = USBENUM_PRODUCT_ID_STRING_FAILURE;
                        DbgTraceTo(deviceContext->PortContext->IfrLog,
                                   TL_ERROR,
                                   Device,
                                   "HW_COMPLIANCE: Device control transfer for getting Alternate mode String descriptor failed with error %!STATUS!, URB Status %x",
                                   status,
                                   urbStatus);

                        EventWriteUSBHUB3_ETW_EVENT_ALTERNATE_MODE_STRING_FAILURE(
                            &deviceContext->DsmContext.CurrentActivityId,
                            deviceContext->UsbDevice, 
                            0,
                            urbStatus,
                            status);

                    }
                    break;

                default:
                    DbgTraceTo(deviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE: Device control transfer for getting descriptor (default case) failed with error %!STATUS!, URB Status %x",
                               status,
                               urbStatus);
                    
                    break;
                }           
            } else if ((setupPacket->bmRequestType.Type == BMREQUEST_VENDOR) &&
                       (setupPacket->bRequest == deviceContext->MsOsVendorCode)) {
                
                switch (setupPacket->wIndex.W) {
                case MS_EXT_CONFIG_DESCRIPTOR_INDEX: 
                    deviceContext->EnumMsgId = USBENUM_MS_EXT_CONFIG_DESCRIPTOR_FAILURE;
                    
                    DbgTraceTo(deviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE: Device control transfer for getting MS OS Extended Config descriptor failed with error %!STATUS!, URB Status %x",
                               status,
                               urbStatus);
                    EventWriteUSBHUB3_ETW_EVENT_MS_EXT_CONFIG_DESCRIPTOR_FAILURE(
                        &deviceContext->DsmContext.CurrentActivityId,
                        deviceContext->UsbDevice, 
                        0,
                        urbStatus,
                        status);
                    
                    break;

                case MS_CONTAINER_ID_DESCRIPTOR_INDEX:
                    deviceContext->EnumMsgId = USBENUM_MS_CONTAINER_ID_DESCRIPTOR_FAILURE;

                    DbgTraceTo(deviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE: Device control transfer for getting MS OS Container Id descriptor failed with error %!STATUS!, URB Status %x",
                               status,
                               urbStatus);
                    EventWriteUSBHUB3_ETW_EVENT_MS_CONTAINER_ID_DESCRIPTOR_FAILURE(
                        &deviceContext->DsmContext.CurrentActivityId,
                        deviceContext->UsbDevice, 
                        0,
                        urbStatus,
                        status);

                    break;

                case MSOS20_DESCRIPTOR_INDEX:
                    deviceContext->EnumMsgId = USBENUM_MSOS20_DESCRIPTOR_SET_FAILURE;

                    DbgTraceTo(deviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE: Device control transfer for getting MS OS 2.0 Descriptor Set failed with error %!STATUS!, URB Status %x",
                               status,
                               urbStatus);
                    
                    EventWriteUSBHUB3_ETW_EVENT_MSOS20_DESCRIPTOR_FAILURE(
                        &deviceContext->DsmContext.CurrentActivityId,
                        deviceContext->UsbDevice, 
                        0,
                        urbStatus,
                        status);

                    break;

                case MSOS20_SET_ALT_ENUMERATION:
                    deviceContext->EnumMsgId = USBENUM_MSOS20_SET_ALT_ENUMERATION_FAILURE;

                    DbgTraceTo(deviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE: Device control transfer for setting MS OS 2.0 Alternate Enumeration failed with error %!STATUS!, URB Status %x",
                               status,
                               urbStatus);
                    
                    EventWriteUSBHUB3_ETW_EVENT_MSOS20_SET_ALT_ENUMERATION_FAILURE(
                        &deviceContext->DsmContext.CurrentActivityId,
                        deviceContext->UsbDevice, 
                        0,
                        urbStatus,
                        status);

                    break;

                default:
                    DbgTraceTo(deviceContext->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "HW_COMPLIANCE: Device control transfer for getting MS OS descriptor (default case) failed with error %!STATUS!, URB Status %x",
                               status,
                               urbStatus);

                    break;
                }
            } else if ((setupPacket->bRequest == USB_REQUEST_ISOCH_DELAY) &&
                       (setupPacket->bmRequestType.Type == BMREQUEST_STANDARD)) {

                DbgTraceTo(deviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Device control transfer for Set Isoch Delay with error %!STATUS!, URB Status %x",
                           status,
                           urbStatus);
                USB_HW_VERIFIER_DEVICE_BREAK(deviceContext, DeviceHwVerifierSetIsochDelayFailure);

                EventWriteUSBHUB3_ETW_EVENT_SET_ISOCH_DELAY_FAILURE(
                    &deviceContext->DsmContext.CurrentActivityId,
                    deviceContext->UsbDevice,
                    deviceContext->PortContext->PortInformation.PortNumber,
                    urbStatus,
                    status);

            } else {
                //
                // It is acceptable for transfer that disarms the device for
                // wake, to fail. For everything else, break in the hardware
                // verifier
                //
                if ((setupPacket->bmRequestType.Type == BMREQUEST_STANDARD) &&
                    (setupPacket->bRequest == USB_REQUEST_CLEAR_FEATURE) &&
                    (setupPacket->wValue.W == USB_FEATURE_REMOTE_WAKEUP)) {

                } else if ((setupPacket->bmRequestType.Type == BMREQUEST_STANDARD) &&
                           (setupPacket->bRequest == USB_REQUEST_SET_FEATURE) &&
                           (setupPacket->wValue.W == FUNCTION_SUSPEND) &&
                           (setupPacket->wIndex.W == 0)) {

                } else {
                    
                    DbgTraceTo(deviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Device control transfer failed with error %!STATUS!, URB Status 0x%x, bmRequestType 0x%x, bRequest 0x%x",
                       status,
                       urbStatus,
                       setupPacket->bmRequestType.Type,
                       setupPacket->bRequest
                       );

                    USB_HW_VERIFIER_DEVICE_BREAK(deviceContext, DeviceHwVerifierControlTransferFailure);

                    EventWriteUSBHUB3_ETW_EVENT_DEVICE_CONTROL_TRANSFER_ERROR(
                        &deviceContext->DsmContext.CurrentActivityId,
                        deviceContext->UsbDevice,
                        deviceContext->PortContext->PortInformation.PortNumber,
                        urbStatus,
                        status);
                }

            }
        } else {
            dsmEvent = DsmEventTransferSuccess;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            EventWriteUSBHUB3_ETW_EVENT_DEVICE_CONTROL_TRANSFER_ERROR(
                0,
                deviceContext->UsbDevice, 
                0,
                urbStatus,
                status);

        }
        
        HUBMISC_RequestReuse(WdfRequest);
        
        //
        // Notify DSM of completion status of the control transfer
        //
        HUBSM_AddDsmEvent(deviceContext,
                          dsmEvent);

    }

}

NTSTATUS
HUBDTX_GetMsOsFeatureDescriptor(
    __in
        PDEVICE_CONTEXT    DeviceContext,
    __in
        UCHAR              Recipient,
    __in
        UCHAR              InterfaceNumber,
    __in
        USHORT             Index,
    __out_bcount(DescriptorBufferLength)
        PVOID              DescriptorBuffer,
    __in
        size_t             DescriptorBufferLength
    )
/*++

Routine Description:

    Submits a request to retrieve an MS OS Feature Descriptor

Returns:

    NTSTATUS

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;

        //
        // Format the setup packet for Get MS OS Feature Descriptor
        //   whitepaper - "Microsoft OS Descriptors Overview"
        //      section - "How To Retrieve an OS Feature Descriptor"
        //
        setupPacket->bmRequestType.Recipient = Recipient;
        setupPacket->bmRequestType.Type = BMREQUEST_VENDOR;
        setupPacket->bmRequestType.Dir = BMREQUEST_DEVICE_TO_HOST;
        
        
        setupPacket->bmRequestType.Reserved = 0;

        setupPacket->bRequest = DeviceContext->MsOsVendorCode;
        setupPacket->wValue.HiByte = InterfaceNumber;
        setupPacket->wValue.LowByte = 0;
        setupPacket->wIndex.W = Index;
        setupPacket->wLength = (USHORT)DescriptorBufferLength;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         DescriptorBuffer,
                                         DescriptorBufferLength,
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

    }

    return status;
}


NTSTATUS
HUBDTX_GetDescriptor (
    __in
        PDEVICE_CONTEXT    DeviceContext,
    __out_bcount(DescriptorBufferLength)
        PVOID              DescriptorBuffer,
    __in
        size_t             DescriptorBufferLength,
    __in
        UCHAR              DescriptorType,
    __in
        UCHAR              DescriptorIndex,
    __in
        USHORT             LanguageId,
    __in
        BOOLEAN            ClassDescriptor
    )
/*++

Routine Description:


    Submits a request to get a descriptor from the hub. Completion routine of this
    function should queue a HsmEventTransferSuccess or HsmEventTransferFailure
    event to HSM.

Return Value:

    NTSTATUS

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
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->bmRequestType.Type = (ClassDescriptor ? BMREQUEST_CLASS : BMREQUEST_STANDARD);
        setupPacket->bmRequestType.Dir = BMREQUEST_DEVICE_TO_HOST;

        setupPacket->bRequest = USB_REQUEST_GET_DESCRIPTOR;
        setupPacket->wValue.HiByte = DescriptorType;
        setupPacket->wValue.LowByte = DescriptorIndex;
        setupPacket->wIndex.W = LanguageId;
        setupPacket->wLength = (USHORT)DescriptorBufferLength;



        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         DescriptorBuffer,
                                         DescriptorBufferLength,
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

    }

    return status;
}


DSM_EVENT
HUBDTX_CheckIfAlternateInterfaceLeftToBeSet (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    While configuring the device, DSM will keep track of the number of alternate interface
    setting control transfers that need to be sent to the device. This routine checks if
    all the settings have been taken care of.

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT               dsmEvent;
    PDEVICE_CONFIGURATION   configuration;
    PINTERFACE_HANDLE       interfaceHandle;

    TRY {

        dsmEvent = DsmEventNo;

        configuration = DeviceContext->CurrentDeviceConfiguration;

        FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                        &configuration->InterfaceList,
                        InterfaceLink,
                        interfaceHandle) {

            if (interfaceHandle->InterfaceFlags.NeedsSetInterface == 1) {

                DeviceContext->NextInterfaceToSet = interfaceHandle;

                dsmEvent = DsmEventYes;
                LEAVE;
            }
        }

    } FINALLY {

    }

    return dsmEvent;
}

VOID
HUBDTX_GetRemoteWakeCapabilityUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_STATUS for the first interface to the device using a control transfer.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued directly
    from this function. Completion routine of this function should queue a
    DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {
        //
        // Format the setup packet for Set Feature
        // USB 3.0 Spec, 9.4.9
        // USB 2.0 Spec, 9.4.9
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;
        
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_DEVICE_TO_HOST;
        setupPacket->bRequest = USB_REQUEST_GET_STATUS;
        setupPacket->wLength = sizeof(USB_INTERFACE_STATUS);
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_INTERFACE;
        setupPacket->wValue.W = 0;
        //
        // wIndex has the interface number
        //
        setupPacket->wIndex.W = 0;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         DeviceContext->ScratchBuffer,
                                         sizeof(USB_INTERFACE_STATUS),
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to send request to get interface status %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        } 
    }    

}

DSM_EVENT
HUBDTX_ValidateInterfaceStatusForRemoteWakeCapability (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of get interface status and
    store whether the device supports remote wake or 
    not

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    PUSB_INTERFACE_STATUS   interfaceStatus;
    DSM_EVENT               dsmEvent;

    TRY {

        dsmEvent = DsmEventOperationSuccess;

        interfaceStatus = (PUSB_INTERFACE_STATUS) DeviceContext->ScratchBuffer;

        if (DeviceContext->ControlRequest.NumberOfBytes != sizeof(USB_INTERFACE_STATUS)) {
            
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Number of bytes returned for get interface status is invalid: %d",
                       DeviceContext->ControlRequest.NumberOfBytes);

            dsmEvent = DsmEventOperationFailure;
            LEAVE;

        }

        //
        // Current value of UsbWakeupSupport is based on the bmAttributes in the
        // Configuration descriptor. USB 30 Spec is not too clear about how this
        // field is used for SS devices. For now, we are interpreting the spec
        // that if any of the functions support remote wake, then the remote wake
        // bit should be set. So if the remote wake bit is not set, then we never
        // expect the interface status to say that it supports remote wake. So let
        // us put some checks in place to catch inconsistencies in reporting
        //
        if ((DeviceContext->DeviceFlags.UsbWakeupSupport == 0) &&
            (interfaceStatus->RemoteWakeupCapable == 1)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "HW_COMPLIANCE:Device 0x%p supports remote wake in Interface 0 but does not report remote wake in the Configuration Descriptor",
                       DeviceContext);

            USB_HW_VERIFIER_DEVICE_BREAK(DeviceContext, DeviceHwVerifierInterfaceWakeCapabilityMismatch);

            EventWriteUSBHUB3_ETW_EVENT_INTERFACE_WAKE_CAPABILITY_MISMATCH(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice,
                DeviceContext->DeviceFlags.DeviceIsComposite,
                DeviceContext->DeviceFlags.UsbWakeupSupport,
                interfaceStatus->RemoteWakeupCapable);

            LEAVE;
        }
        
        //
        // For single function devices, the reverse is also not valid
        //
        if ((DeviceContext->DeviceFlags.DeviceIsComposite == 0) &&
            (DeviceContext->DeviceFlags.UsbWakeupSupport == 1) &&
            (interfaceStatus->RemoteWakeupCapable == 0)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "HW_COMPLIANCE:Device 0x%p does not support remote wake in Interface 0 but reports remote wake in the Configuration Descriptor",
                       DeviceContext);

            USB_HW_VERIFIER_DEVICE_BREAK(DeviceContext, DeviceHwVerifierInterfaceWakeCapabilityMismatch);

            EventWriteUSBHUB3_ETW_EVENT_INTERFACE_WAKE_CAPABILITY_MISMATCH(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice,
                DeviceContext->DeviceFlags.DeviceIsComposite,
                DeviceContext->DeviceFlags.UsbWakeupSupport,
                interfaceStatus->RemoteWakeupCapable);
        }
        

    } FINALLY {

    }

    return dsmEvent;
}

VOID
HUBDTX_ArmDeviceForWakeUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a SET_FEATURE for REMOTE_WAKEUP to the device using a control transfer.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued directly
    from this function. Completion routine of this function should queue a
    DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    FUNCTION_SUSPEND_OPTIONS        suspendOptions;

    TRY {
        INTERLOCKED_SET_BIT_FIELD(PDO_FLAGS, &DeviceContext->PdoContext->PdoFlags, ArmedForWake);

        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;
        
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;
        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wLength = 0;
        //
        // Format the setup packet for Set Feature
        // USB 3.0 Spec, 9.4.9
        // USB 2.0 Spec, 9.4.9
        //
        if (DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagSpeedSuper) {
            
            suspendOptions.AsUchar = 0;
            suspendOptions.RemoteWake = 1;
            //
            // Note that it is not strictly necessary to send the function
            // to suspend because we are anyways going to send the port
            // to U3 but it does not hurt to do it explictily
            //
            suspendOptions.Suspend = 1;

            setupPacket->bmRequestType.Recipient = BMREQUEST_TO_INTERFACE;
            setupPacket->wValue.W = FUNCTION_SUSPEND;
            //
            // Lower byte is the interface number
            //
            setupPacket->wIndex.LowByte = 0;
            setupPacket->wIndex.HiByte = suspendOptions.AsUchar;

        } else {
            
            setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
            setupPacket->wValue.W = USB_FEATURE_REMOTE_WAKEUP;
            setupPacket->wIndex.W = 0;
        }   

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to send request to enable wake on device %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        } 
    }    

}

VOID
HUBDTX_DisarmDeviceForWakeUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a CLEAR_FEATURE for REMOTE_WAKEUP to the device using a control transfer.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued directly
    from this function. Completion routine of this function should queue a
    DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    FUNCTION_SUSPEND_OPTIONS        suspendOptions;

    TRY {
        INTERLOCKED_CLEAR_BIT_FIELD(PDO_FLAGS, &DeviceContext->PdoContext->PdoFlags, ArmedForWake);

        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;
        
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;
        setupPacket->wLength = 0;
        //
        // Format the setup packet for Set Feature
        // USB 3.0 Spec, 9.4.9
        // USB 2.0 Spec, 9.4.9
        //
        if (DeviceContext->DsmContext.SubSmFlags & DsmSubSmFlagSpeedSuper) {
            
            setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
            suspendOptions.AsUchar = 0;
            setupPacket->bmRequestType.Recipient = BMREQUEST_TO_INTERFACE;
            setupPacket->wValue.W = FUNCTION_SUSPEND;
            //
            // Lower byte is the interface number
            //
            setupPacket->wIndex.LowByte = 0;
            setupPacket->wIndex.HiByte = suspendOptions.AsUchar;

        } else {
            setupPacket->bRequest = USB_REQUEST_CLEAR_FEATURE;
            setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
            setupPacket->wValue.W = USB_FEATURE_REMOTE_WAKEUP;
            setupPacket->wIndex.W = 0;
        }   

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to send request to disable wake on device %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }

    }    

}

VOID
HUBDTX_CancelControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Cancels the current control transfer request for the DSM.

Returns:

    VOID

--*/
{
    BOOLEAN             cancelled;

    TRY {

        cancelled = WdfRequestCancelSentRequest(DeviceContext->ControlRequest.WdfRequest);

        if (cancelled == FALSE) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Unable to Cancel Hub Control Request");
        }

    } FINALLY {

    }
}



VOID
HUBDTX_GetConfigDescriptorWithDefaultSizeUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the CONFIGURATION
    descriptor with the size set to 255.  A pointer/handle to the descriptor
    is cached in the DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS        status;

    TRY {

        //
        // Free any previous configuration descriptor buffer
        //
        HUBMISC_FreeBufferIfNotNull(DeviceContext->ConfigurationDescriptor, USBHUB3_DEVICE_TAG);
        DeviceContext->ConfigurationDescriptor = NULL;
        DeviceContext->CompatIdInterfaceDescriptor = NULL;
        
        status = HUBDTX_GetDescriptor(DeviceContext,
                                      DeviceContext->ScratchBuffer,
                                      MAXIMUM_CONFIGURATION_DESCRIPTOR_SIZE,
                                      USB_CONFIGURATION_DESCRIPTOR_TYPE,
                                      0,
                                      0,
                                      FALSE);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "USB_CONFIGURATION_DESCRIPTOR_TYPE Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_GetConfigDescriptorWithReturnedLengthUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the CONFIGURATION
    descriptor with the size set to the value returned from the last
    call to HUBDTX_GetConfigDescriptorWithDefaultSizeUsingControlTransfer.
    A pointer/handle to the descriptor is cached in the DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    PUSB_CONFIGURATION_DESCRIPTOR   configurationDescriptor;
    NTSTATUS                        status;

    TRY {

        configurationDescriptor =
            (PUSB_CONFIGURATION_DESCRIPTOR) DeviceContext->ScratchBuffer;

        //
        // Allocate a buffer big enough for the complete configuration descriptor set
        //
        NT_ASSERT(DeviceContext->ConfigurationDescriptor == NULL);
        DeviceContext->ConfigurationDescriptor =
            ExAllocatePoolWithTag(NonPagedPool,
                                  configurationDescriptor->wTotalLength,
                                  USBHUB3_DEVICE_TAG);

        if (DeviceContext->ConfigurationDescriptor == NULL) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to allocate space for configuration descriptor");

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        status = HUBDTX_GetDescriptor(DeviceContext,
                                      DeviceContext->ConfigurationDescriptor,
                                      configurationDescriptor->wTotalLength,
                                      USB_CONFIGURATION_DESCRIPTOR_TYPE,
                                      0,
                                      0,
                                      FALSE);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "USB_CONFIGURATION_DESCRIPTOR_TYPE Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_GetDeviceDescriptorUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the DEVICE descriptor.
    A pointer/handle to the descriptor is cached in the DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.


Returns:

    VOID

--*/
{
    NTSTATUS        status;

    TRY {

        status = HUBDTX_GetDescriptor(DeviceContext,
                                      DeviceContext->ScratchBuffer,
                                      sizeof(USB_DEVICE_DESCRIPTOR),
                                      USB_DEVICE_DESCRIPTOR_TYPE,
                                      0,
                                      0,
                                      FALSE);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "USB_DEVICE_DESCRIPTOR_TYPE Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}

VOID
HUBDTX_GetInitialDeviceDescriptorUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the DEVICE descriptor.
    A pointer/handle to the descriptor is cached in the DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

    COMPAT: This control request must specify a transfer size of 0x40 bytes
            to avoid breaking devices that behave badly when the transfer size
            is smaller.

Returns:

    VOID

--*/
{
    NTSTATUS        status;

    TRY {

        status = HUBDTX_GetDescriptor(DeviceContext,
                                      DeviceContext->ScratchBuffer,
                                      0x40,
                                      USB_DEVICE_DESCRIPTOR_TYPE,
                                      0,
                                      0,
                                      FALSE);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "USB_DEVICE_DESCRIPTOR_TYPE (Initial) Request returned %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_GetDeviceQualifierDescriptorUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the DEVICE QUALIFIER
    descriptor.  A pointer/handle to the descriptor is cached in the DSM
    context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS            status;

    TRY {

        status = HUBDTX_GetDescriptor(DeviceContext,
                                      &DeviceContext->DeviceQualifierDescriptor,
                                      sizeof(USB_DEVICE_QUALIFIER_DESCRIPTOR),
                                      USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE,
                                      0,
                                      0,
                                      FALSE);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_GetLanguageIdStringDescriptorUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the STRING descriptor at
    index 0.  A pointer/handle to the descriptor is cached in the DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS        status;

    TRY {

        HUBMISC_FreeBufferIfNotNull(DeviceContext->LanguageIdDescriptor, USBHUB3_DEVICE_TAG);
        DeviceContext->LanguageIdDescriptor = NULL;

        status = HUBDTX_GetDescriptor(DeviceContext,
                                      DeviceContext->ScratchBuffer,
                                      MAXIMUM_USB_STRING_LENGTH,
                                      USB_STRING_DESCRIPTOR_TYPE,
                                      0,
                                      0,
                                      FALSE);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Language ID USB_STRING_DESCRIPTOR_TYPE Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the language ID string descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}

VOID
HUBDTX_GetBillboardStringDescriptorUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the the Billboard
    AdditionalInfoUrl string.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS        status;

    TRY {

        status = HUBDTX_GetDescriptor(DeviceContext,
                                      DeviceContext->ScratchBuffer,
                                      MAXIMUM_USB_STRING_LENGTH,
                                      USB_STRING_DESCRIPTOR_TYPE,
                                      DeviceContext->BillboardInfo->BillboardDescriptor->iAddtionalInfoURL,
                                      0,
                                      FALSE);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Billboard USB_STRING_DESCRIPTOR_TYPE Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the language ID string descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}

VOID
HUBDTX_GetAlternateModeStringDescriptorUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the STRING descriptor 
    for the current alternate mode.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS            status;
    PBILLBOARD_INFO     billboardInfo;

    billboardInfo = DeviceContext->BillboardInfo;

    TRY {

        status = HUBDTX_GetDescriptor(DeviceContext,
                                      DeviceContext->ScratchBuffer,
                                      MAXIMUM_USB_STRING_LENGTH,
                                      USB_STRING_DESCRIPTOR_TYPE,
                                      billboardInfo->BillboardDescriptor->AlternateMode[
                                      billboardInfo->CurrentAlternateMode].iAlternateModeSetting,
                                      0,
                                      FALSE);
    } FINALLY {

        if (!NT_SUCCESS(status)) {

            billboardInfo->CurrentAlternateMode = INVALID_ALTERNATE_MODE;

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Alternate Mode USB_STRING_DESCRIPTOR_TYPE Request Submitted %!STATUS!",
                       status);
            //
            // Notify the DSM of of the failure to retrieve the alternate mode string descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_GetMSOSContainerIdDescriptorUsingControlTransfer (
__in
PDEVICE_CONTEXT        DeviceContext
)
/*++

Routine Description:

Sends a GET_DESCRIPTOR request to the device for the vendor-defined MS OS
Container ID descriptor.  On successful completion the container ID is
stored in the DSM context.

If WdfRequestSend fails then DsmEventTransferFailure should be queued
directly from this function. Completion routine of this function should
queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

VOID

--*/
{
NTSTATUS        status;

TRY {

status = HUBDTX_GetMsOsFeatureDescriptor(DeviceContext,
                                                         BMREQUEST_TO_DEVICE,
                                                 0,
                                                 MS_CONTAINER_ID_DESCRIPTOR_INDEX,
                                                 DeviceContext->ScratchBuffer,
                                                 sizeof(MS_CONTAINER_ID_DESC));

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "MS OS Container ID Descriptor Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the descriptor header
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_GetMSOS20DescriptorSetUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the vendor-defined MS OS 2.0
    descriptor set.  On successful completion the descriptor set is stored in the 
    DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;

    TRY {

        HUBMISC_FreeBufferIfNotNull(DeviceContext->MsOs20Info.DescriptorSet, USBHUB3_DEVICE_TAG);

        DeviceContext->MsOs20Info.DescriptorSet =
            ExAllocatePoolWithTag(NonPagedPool,
                                  DeviceContext->MsOs20Info.DescriptorSetInfo.wLength,
                                  USBHUB3_DEVICE_TAG);

        if (DeviceContext->MsOs20Info.DescriptorSet == NULL) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to allocate space for MS OS 2.0 descriptor set");

            status = STATUS_INSUFFICIENT_RESOURCES;

            LEAVE;
        }

        DeviceContext->MsOsVendorCode = DeviceContext->MsOs20Info.DescriptorSetInfo.bMS_VendorCode;

        DeviceContext->ControlRequest.UsbdFlags = USBDFLAG_ALLOW_LARGE_TRANSFER;

        status = HUBDTX_GetMsOsFeatureDescriptor(DeviceContext,
                                                 BMREQUEST_TO_DEVICE,
                                                 0,
                                                 MSOS20_DESCRIPTOR_INDEX,
                                                 DeviceContext->MsOs20Info.DescriptorSet,
                                                 DeviceContext->MsOs20Info.DescriptorSetInfo.wLength);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DeviceContext->ControlRequest.UsbdFlags = 0;
            
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "MS OS 2.0 Descriptor Set Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the descriptor set
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_SendMsOs20AltEnumCommandUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a Alternate Enumeration command the device so it can return non-default
    USB descriptors. On successful completion the descriptor set is stored in the 
    DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        NT_ASSERT(DeviceContext->MsOs20Info.Flags.AlternateEnumeration == 1);
        NT_ASSERT(DeviceContext->MsOs20Info.AltEnumFlags.AltEnumCmdSent == 0);
        NT_ASSERT(DeviceContext->MsOs20Info.DescriptorSetInfo.bAltEnumCmd != 0);
        
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->bmRequestType.Type = BMREQUEST_VENDOR;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;
        setupPacket->bmRequestType.Reserved = 0;
        setupPacket->bRequest = DeviceContext->MsOsVendorCode;
        setupPacket->wValue.HiByte = DeviceContext->MsOs20Info.DescriptorSetInfo.bAltEnumCmd;
        setupPacket->wValue.LowByte = 0;
        setupPacket->wIndex.W = MSOS20_SET_ALT_ENUMERATION;
        setupPacket->wLength = 0;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "MS OS 2.0 Set Alternate Enumeration Command Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the descriptor set
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        } else {
            INTERLOCKED_SET_BIT_FIELD(MSOS20_ALT_ENUM_FLAGS, &DeviceContext->MsOs20Info.AltEnumFlags, AltEnumCmdSent);
        }
    }
}



VOID
HUBDTX_GetMSOSDescriptorUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the STRING descriptor at
    index 0xEE.  On successful completion a pointer/handle to the MS OS
    descriptor is cached in the DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS        status;

    TRY {

        status = HUBDTX_GetDescriptor(DeviceContext,
                                      &DeviceContext->MsOsDescriptor,
                                      sizeof(OS_STRING),
                                      USB_STRING_DESCRIPTOR_TYPE,
                                      OS_STRING_DESCRIPTOR_INDEX,
                                      0,
                                      FALSE);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "MSOS Descriptor Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_GetBOSDescriptorUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the BOS Descriptor.
    
    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS            status;
    PUSB_BOS_DESCRIPTOR bosDescriptor;

    TRY {

        bosDescriptor =
            (PUSB_BOS_DESCRIPTOR) DeviceContext->ScratchBuffer;

        //
        // Allocate a buffer big enough for the complete BOS descriptor set
        //
        NT_ASSERT(DeviceContext->BOSDescriptor == NULL);
        DeviceContext->BOSDescriptor =
            ExAllocatePoolWithTag(NonPagedPool,
                                  bosDescriptor->wTotalLength,
                                  USBHUB3_DEVICE_TAG);

        if (DeviceContext->BOSDescriptor == NULL) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to allocate space for BOS descriptor");

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        status = HUBDTX_GetDescriptor(DeviceContext,
                                      DeviceContext->BOSDescriptor,
                                      bosDescriptor->wTotalLength,
                                      USB_BOS_DESCRIPTOR_TYPE,
                                      0,
                                      0,
                                      FALSE);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "USB_BOS_DESCRIPTOR_TYPE Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }

    }

}


VOID

HUBDTX_SendUsbFeaturesVendorCmdUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends the USB dual role features vendor command to the device.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        NT_ASSERT(DeviceContext->MsOs20Info.DescriptorSetInfo.bAltEnumCmd != 0);
        
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->bmRequestType.Type = BMREQUEST_VENDOR;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;
        setupPacket->bmRequestType.Reserved = 0;
        setupPacket->bRequest = DeviceContext->DualRoleVendorCommand;
        setupPacket->wValue.W = LOWORD(UsbDualRoleFeaturesGetUlong(&DeviceContext->UsbDualRoleLocalFeatures));
        setupPacket->wIndex.W = HIWORD(UsbDualRoleFeaturesGetUlong(&DeviceContext->UsbDualRoleLocalFeatures));
        setupPacket->wLength = 0;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
           
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Dual Role USB Features Vendor Command failed submission %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the descriptor set
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        } else {
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, DualRoleVendorCmdSent);
        }
    }
}


DSM_EVENT
HUBDTX_ValidateAndCacheBOSDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the device BOS descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    DSM_EVENT               dsmEvent;
    PUSB_BOS_DESCRIPTOR     bosDescriptor;
    VALIDATION_DEVICE_INFO  devInfo;
    BOS_DESC_SET_INFO       bosInfo;
    BOOLEAN                 msOs20DescriptorSetInfoValid;
    ULONG                   sublinkSpeedAttrCount;
    BOOLEAN                 ltmEnable;
    
    TRY {
        
        msOs20DescriptorSetInfoValid = FALSE;
        ltmEnable = FALSE;

        if (DeviceContext->BOSDescriptor == NULL) {

            bosDescriptor = (PUSB_BOS_DESCRIPTOR) DeviceContext->ScratchBuffer;

        } else {

            bosDescriptor = DeviceContext->BOSDescriptor;

        }

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        //
        // See if we already processed the MS OS 2.0 descriptor set due
        // to the device supporting alternate enumeration
        //
        if (DeviceContext->MsOs20Info.Flags.DescriptorSetInfo == 1) {
            msOs20DescriptorSetInfoValid = TRUE;
        }

        devInfo.USBVersion = DeviceContext->DeviceDescriptor.bcdUSB;
        devInfo.MaxPortPower = DeviceContext->HubFdoContext->HubInformation.MaxPortPower;
        
        if (FALSE == HUBDESC_ValidateBOSDescriptorSet(bosDescriptor,
                                                      DeviceContext->ControlRequest.NumberOfBytes,
                                                      &devInfo,
                                                      &bosInfo,
                                                      &DeviceContext->U1ExitLatency,
                                                      &DeviceContext->U2ExitLatency,
                                                      &ltmEnable,
                                                      &DeviceContext->Lpm20Attributes,
                                                      &DeviceContext->DeviceFlags,
                                                      DeviceContext->PortContext->IfrLog)) {
            
            if (DeviceContext->DeviceHackFlags.IgnoreBOSDescriptorValidationFailure == 0) {
                DeviceContext->EnumMsgId = USBENUM_BAD_BOS_DESCRIPTOR;
                dsmEvent = DsmEventOperationFailure;
                LEAVE;
            } else {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_INFO,
                           Device,
                           "Ignoring BOS descriptor validation failure");
            }
        } 

        if (ltmEnable) {

            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, LtmCapable);
        }


        //
        // Save off the Usb31Capability's sublink speed attributes if necessary
        // 
        if ((bosInfo.USB31CapabilityDescriptor != NULL) && (DeviceContext->SublinkSpeedAttr == NULL)) {
            sublinkSpeedAttrCount = bosInfo.USB31CapabilityDescriptor->bmAttributes.SublinkSpeedAttrCount + 1;
            DeviceContext->SublinkSpeedAttr =
                ExAllocatePoolWithTag(NonPagedPool,
                                      sublinkSpeedAttrCount * sizeof(USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED),
                                      USBHUB3_DEVICE_TAG);

            if (DeviceContext->SublinkSpeedAttr == NULL) {

                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                    TL_ERROR,
                    Device,
                    "Unable to allocate space for sublink speed attributes");

                dsmEvent = DsmEventOperationFailure;
                LEAVE;
            }

            RtlCopyMemory(DeviceContext->SublinkSpeedAttr,
                          bosInfo.USB31CapabilityDescriptor->bmSublinkSpeedAttr,
                          sublinkSpeedAttrCount * sizeof(USB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED));

            DeviceContext->SublinkSpeedAttrCount = sublinkSpeedAttrCount;
        }

        //
        // Save off the container ID if necessary
        // 
        if (bosInfo.ContainerIDDescriptor != NULL) {
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, BOSContainerIdValid);
            //
            // If the OS Descriptor container ID was provided, prefer that
            // 
            if (DeviceContext->DeviceFlags.ContainerIdValid == 0) {
            
                RtlCopyMemory(&DeviceContext->ContainerId,
                              bosInfo.ContainerIDDescriptor->ContainerID,
                              sizeof(GUID));
            }

        }

        //
        // Save off the Billboard descriptor 
        // 
        if (bosInfo.BillboardDescriptor != NULL) {


            HUBDTX_CacheBillboardInfo(DeviceContext,
                                      bosInfo.BillboardDescriptor);

        }
        
        if (bosInfo.UsbDualRoleFeatures != NULL) {
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, DeviceIsDualRole);
            DeviceContext->DualRoleVendorCommand = bosInfo.UsbDualRoleFeatures->VendorCmdCode;
            UsbDualRoleFeaturesQueryLocalMachine(&DeviceContext->UsbDualRoleLocalFeatures);
            UsbDualRoleFeaturesSetUlong(&DeviceContext->UsbDualRolePartnerFeatures, bosInfo.UsbDualRoleFeatures->USBFeatures);
            
            //
            // Publish the USB dual-role capabilities of the device being enumerated
            //
            HUBWNF_PublishUsbPartnerDualRoleFeatures(DeviceContext->PortContext->ConnectorId.UcmConnectorId,
                                                     TRUE,
                                                     bosInfo.UsbDualRoleFeatures->USBFeatures);
        }

        //
        // If this is an alternate BOS descriptor then we already have a valid MS OS 2.0 Descriptor
        // Set Information structure and optionally the MS OS 2.0 Descriptor Set.  Don't try to 
        // get it again.
        //
        if ((msOs20DescriptorSetInfoValid == FALSE) &&
            (bosInfo.SelectedDescriptorSetInfo != NULL)) {

            RtlCopyMemory(&DeviceContext->MsOs20Info.DescriptorSetInfo,
                          bosInfo.SelectedDescriptorSetInfo,
                          sizeof(MSOS20_DESCRIPTOR_SET_INFO));

            DeviceContext->MsOs20Info.Flags.DescriptorSetInfo = 1;

            if ((DeviceContext->MsOs20Info.DescriptorSetInfo.bAltEnumCmd != 0) &&
                (DeviceContext->MsOs20Info.AltEnumFlags.AltEnumCmdCached == 0)) {
                
                DeviceContext->MsOs20Info.AltEnumCmd = 
                    DeviceContext->MsOs20Info.DescriptorSetInfo.bAltEnumCmd;

                DeviceContext->MsOsVendorCode =
                    DeviceContext->MsOs20Info.DescriptorSetInfo.bMS_VendorCode;

                DeviceContext->MsOs20Info.Flags.AlternateEnumeration = 1;
            }
        }

        if (DeviceContext->BOSDescriptor == NULL) {
            //
            // Allocate a buffer big enough for the BOS descriptor
            //
            DeviceContext->BOSDescriptor =
                ExAllocatePoolWithTag(NonPagedPool,
                                      bosDescriptor->wTotalLength,
                                      USBHUB3_DEVICE_TAG);

            if (DeviceContext->BOSDescriptor == NULL) {

                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Unable to allocate space for BOS descriptor");

                dsmEvent = DsmEventOperationFailure;
                LEAVE;
            }

            RtlCopyMemory(DeviceContext->BOSDescriptor,
                          bosDescriptor,
                          bosDescriptor->wTotalLength);
        }
        
        if (DeviceContext->DeviceFlags.DeviceIsSuperSpeedCapable == 1) {
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_SQM_PROPERTIES, &DeviceContext->DeviceSqmFlags, IsSuperSpeedCapable);
        }        
        
        dsmEvent = DsmEventOperationSuccess;

        INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_SQM_PROPERTIES, &DeviceContext->DeviceSqmFlags, ValidBOSDescriptor);

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_BOS_DESCRIPTOR(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }
    }

    return dsmEvent;
}


DSM_EVENT
HUBDTX_ValidateAndCacheMSOS20DescriptorSet(
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Validates that the MS OS 2.0 Descriptor Set returned by the device is well-formed.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    VALIDATION_DEVICE_INFO          devInfo;
    BOOLEAN                         result;
    PMSOS20_INFO                    msOs20Info;

    TRY {

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);
        
        msOs20Info = &DeviceContext->MsOs20Info;

        msOs20Info->BytesReturned = DeviceContext->ControlRequest.NumberOfBytes;
        msOs20Info->IfrLog = DeviceContext->PortContext->IfrLog;

        result = HUBDESC_ValidateMsOs20DescriptorSet(&devInfo,
                                                     msOs20Info);

        if (result == FALSE) {
            LEAVE;
        }

        if (msOs20Info->Flags.CcgpDevice == 1) {
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, DeviceIsComposite);
        }

    } FINALLY {
        if (result == FALSE) {
            
            DeviceContext->EnumMsgId = USBENUM_INVALID_MSOS20_DESCRIPTOR_SET;
           
            EventWriteUSBHUB3_ETW_EVENT_INVALID_MSOS20_DESCRIPTOR_SET(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }
    }

    return (result == TRUE ? DsmEventOperationSuccess : DsmEventOperationFailure);
}


DSM_EVENT
HUBDTX_CheckIfCompleteBOSDescriptorHasBeenRetrieved (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Check if all of the BOS descriptor has been retrieved
    on the first try

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT               dsmEvent;
    PUSB_BOS_DESCRIPTOR     bosDescriptor;
    
    TRY {

        bosDescriptor = (PUSB_BOS_DESCRIPTOR) DeviceContext->ScratchBuffer;

        if (DeviceContext->ControlRequest.NumberOfBytes < bosDescriptor->wTotalLength) {
            dsmEvent = DsmEventNo;
        } else {
            dsmEvent = DsmEventYes;
        }
        
    } FINALLY {

    }

    return dsmEvent;
}


DSM_EVENT
HUBDTX_ValidateAndCacheBOSDescriptorHeader (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the device BOS descriptor header.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    DSM_EVENT               dsmEvent;
    PUSB_BOS_DESCRIPTOR     bosDescriptor;
    VALIDATION_DEVICE_INFO  devInfo;
           

    TRY {

        dsmEvent = DsmEventOperationSuccess;

        bosDescriptor = (PUSB_BOS_DESCRIPTOR) DeviceContext->ScratchBuffer;

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        if (DeviceContext->ControlRequest.NumberOfBytes < sizeof(USB_BOS_DESCRIPTOR)) {

            DeviceContext->EnumMsgId = USBENUM_BAD_BOS_DESCRIPTOR;

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Number of bytes returned for BOS Descriptor (%d) is less than the expected size of a BOS descriptor (%d).  Aborting further validation",
                       DeviceContext->ControlRequest.NumberOfBytes,
                       sizeof(USB_BOS_DESCRIPTOR));

            dsmEvent = DsmEventOperationFailure;
            LEAVE;

        }

        if (FALSE == HUBDESC_ValidateBOSDescriptor(bosDescriptor,
                                                   DeviceContext->ControlRequest.NumberOfBytes,
                                                   &devInfo,
                                                   DeviceContext->PortContext->IfrLog)) {

            DeviceContext->EnumMsgId = USBENUM_BAD_BOS_DESCRIPTOR;
            dsmEvent = DsmEventOperationFailure;
        }
        
    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_BOS_DESCRIPTOR_HEADER(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }
    }

    return dsmEvent;
}



VOID
HUBDTX_GetBOSDescriptorHeaderUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the BOS Descriptor
    header to determine the length of the BOS Descriptor.
    
    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{

    NTSTATUS        status;

    TRY {

        HUBMISC_FreeBufferIfNotNull(DeviceContext->BOSDescriptor, USBHUB3_DEVICE_TAG);
        
        if (DeviceContext->BillboardInfo) {
            HUBMISC_FreeBufferIfNotNull(DeviceContext->BillboardInfo->BillboardDescriptor, USBHUB3_DEVICE_TAG);
        }

        HUBMISC_FreeBufferIfNotNull(DeviceContext->BillboardInfo, USBHUB3_DEVICE_TAG);

        DeviceContext->BOSDescriptor = NULL;
        DeviceContext->BillboardInfo = NULL;

        status = HUBDTX_GetDescriptor(DeviceContext,
                                      DeviceContext->ScratchBuffer,
                                      MAXIMUM_BOS_DESCRIPTOR_SIZE,
                                      USB_BOS_DESCRIPTOR_TYPE,
                                      0,
                                      0,
                                      FALSE);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "USB_BOS_DESCRIPTOR_TYPE Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }

    }

}

VOID
HUBDTX_GetMSOSExtendedConfigDescriptorHeaderUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the header of the
    vendor-defined MS OS Extended Configuration descriptor.  On successful
    completion a pointer/handle for the header  is stored in the DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS        status;

    TRY {

        HUBMISC_FreeBufferIfNotNull(DeviceContext->MsExtConfigDesc, USBHUB3_DEVICE_TAG);
        DeviceContext->MsExtConfigDesc = NULL;

        status = HUBDTX_GetMsOsFeatureDescriptor(DeviceContext,
                                                 BMREQUEST_TO_DEVICE,
                                                 0,
                                                 MS_EXT_CONFIG_DESCRIPTOR_INDEX,
                                                 DeviceContext->ScratchBuffer,
                                                 sizeof(MS_EXT_CONFIG_DESC_HEADER));

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "MSOS Extend Configuration Descriptor Header Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_GetMSOSExtendedConfigDescriptorUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the complete
    vendor-defined MS OS Extended Configuration descriptor.  On successful
    completion a pointer/handle for the descriptor is stored in the DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                    status;
    PMS_EXT_CONFIG_DESC_HEADER  msExtConfigDescHeader;

    TRY {

        //
        // The scratch buffer should still contain the MS OS Configuration Descriptor Header
        //
        msExtConfigDescHeader = (PMS_EXT_CONFIG_DESC_HEADER) DeviceContext->ScratchBuffer;

        //
        // Allocate the full MS OS extended config descriptor
        //
        NT_ASSERT(DeviceContext->MsExtConfigDesc == NULL);
        DeviceContext->MsExtConfigDesc =
            ExAllocatePoolWithTag(NonPagedPool,
                                  msExtConfigDescHeader->dwLength,
                                  USBHUB3_DEVICE_TAG);

        if (DeviceContext->MsExtConfigDesc == NULL) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to allocate memory for MS OS Extended Configuration Descriptor");

            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        status = HUBDTX_GetMsOsFeatureDescriptor(DeviceContext,
                                                 BMREQUEST_TO_DEVICE,
                                                 0,
                                                 MS_EXT_CONFIG_DESCRIPTOR_INDEX,
                                                 DeviceContext->MsExtConfigDesc,
                                                 msExtConfigDescHeader->dwLength);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "MS OS Extend Configuration Descriptor Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_GetProductIdStringDescriptorUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    If the cached USB Device Descriptor's iProduct field is non zero, and the
    device does not have the "IgnoreHWSerNum" registry flag set, sends a
    GET_DESCRIPTOR request for a STRING descriptor using the string index
    specified in the iProduct field and the English language ID of 0x409.
    On successful completion a pointer/handle for the returned product ID
    string will be stored in the DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS            status;
    UCHAR               iProduct;

    TRY {

        status = STATUS_NOT_SUPPORTED;

        iProduct = DeviceContext->DeviceDescriptor.iProduct;

        if ((iProduct == 0) &&
            DeviceContext->DeviceHackFlags.DisableSerialNumber) {
            LEAVE;
        }

        HUBMISC_FreeBufferIfNotNull(DeviceContext->ProductIdStringDescriptor, USBHUB3_DEVICE_TAG);
        DeviceContext->ProductIdStringDescriptor = NULL;

        status = HUBDTX_GetDescriptor(DeviceContext,
                                      DeviceContext->ScratchBuffer,
                                      MAXIMUM_USB_STRING_LENGTH,
                                      USB_STRING_DESCRIPTOR_TYPE,
                                      iProduct,
                                      DeviceContext->CurrentLanguageId,
                                      FALSE);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Product USB_STRING_DESCRIPTOR_TYPE Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_GetSerialNumberStringDescriptorUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    If the USB Device Descriptor reports a serial number string index, sends
    GET_DESCRIPTOR for a STRING descriptor using the American English language
    ID (0x409) and the serial number string index.  On successful completion
    a pointer/handle for the returned serial number string will be stored in
    the DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS            status;
    UCHAR               iSerialNumber;

    TRY {

        status = STATUS_NOT_SUPPORTED;

        iSerialNumber = DeviceContext->DeviceDescriptor.iSerialNumber;

        if (iSerialNumber == 0) {
            LEAVE;
        }


        status = HUBDTX_GetDescriptor(DeviceContext,
                                      DeviceContext->ScratchBuffer,
                                      MAXIMUM_USB_STRING_LENGTH,
                                      USB_STRING_DESCRIPTOR_TYPE,
                                      iSerialNumber,
                                      AMERICAN_ENGLISH,
                                      FALSE);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Serial Number USB_STRING_DESCRIPTOR_TYPE Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


DSM_EVENT
HUBDTX_IsBytesReturnedLessThanTotalLengthForConfigDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Determines if the bytes returned from the last request to
    get confiugration descriptor is less than the value
    returned in wTotalLength

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT                       dsmEvent;
    PUSB_CONFIGURATION_DESCRIPTOR   configurationDescriptor;

    TRY {

        dsmEvent = DsmEventNo;

        configurationDescriptor = (PUSB_CONFIGURATION_DESCRIPTOR) DeviceContext->ScratchBuffer;

        //
        // Sanity check to insure we are looking at a configuration descriptor
        //
        if (configurationDescriptor->bDescriptorType != USB_CONFIGURATION_DESCRIPTOR_TYPE) {
            LEAVE;
        }
        
        //
        // See if the configuration descriptor set is larger than the 256 byte scratch buffer
        //
        if (DeviceContext->ControlRequest.NumberOfBytes < configurationDescriptor->wTotalLength) {

            dsmEvent = DsmEventYes;
        }

    } FINALLY {

    }

    return dsmEvent;
}


VOID
HUBDTX_SetDeviceConfigurationUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a SET_CONFIGURATION for the configuration request cached in the DSM
    context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    PUSB_CONFIGURATION_DESCRIPTOR   configurationDescriptor;

    TRY {

        //
        // Format the setup packet for Set Configuration
        // USB 3.0 Spec, 9.4.7
        // USB 2.0 Spec, 9.4.7
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        configurationDescriptor =
            (PUSB_CONFIGURATION_DESCRIPTOR)
            &DeviceContext->CurrentDeviceConfiguration->ConfigurationDescriptor;

        setupPacket->bRequest = USB_REQUEST_SET_CONFIGURATION;
        setupPacket->wValue.W = configurationDescriptor->bConfigurationValue;
        setupPacket->wIndex.W = 0;
        setupPacket->wLength = 0;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "USB_REQUEST_SET_CONFIGURATION Request Submitted %!STATUS!",
                       status);

            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_SetDeviceInterfaceUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a SET_INTERFACE for the interface number cached in the DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    PUSB_INTERFACE_DESCRIPTOR       interfaceDescriptor;

    TRY {

        status = STATUS_SUCCESS;

        if (DeviceContext->NextInterfaceToSet == NULL) {
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferSuccess);
            LEAVE;
        }

        //
        // Format the setup packet for Set Interface
        // USB 3.0 Spec, 9.4.10
        // USB 2.0 Spec, 9.4.10
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_INTERFACE;
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        interfaceDescriptor =
            (PUSB_INTERFACE_DESCRIPTOR) DeviceContext->NextInterfaceToSet->InterfaceDescriptor;

        setupPacket->bRequest = USB_REQUEST_SET_INTERFACE;
        setupPacket->wValue.W = interfaceDescriptor->bAlternateSetting;
        setupPacket->wIndex.W = interfaceDescriptor->bInterfaceNumber;
        setupPacket->wLength = 0;

        INTERLOCKED_CLEAR_BIT_FIELD(INTERFACE_FLAGS, &DeviceContext->NextInterfaceToSet->InterfaceFlags, NeedsSetInterface);
        DeviceContext->NextInterfaceToSet = NULL;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "USB_REQUEST_SET_INTERFACE Request Submitted %!STATUS!",
                       status);

            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_SetDeviceNullConfigurationUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a SELECT_CONFIGURATION request to the device with wValue=wIndex=wLength=0.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued directly
    from this function. Completion routine of this function should queue a
    DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {

        //
        // Format the setup packet for Set Configuration
        // USB 3.0 Spec, 9.4.7
        // USB 2.0 Spec, 9.4.7
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_SET_CONFIGURATION;
        setupPacket->wValue.W = 0;
        setupPacket->wIndex.W = 0;
        setupPacket->wLength = 0;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "USB_REQUEST_SET_CONFIGURATION For NULL Configuration Submitted %!STATUS!",
                       status);

            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


DSM_EVENT
HUBDTX_ValidateAndCacheConfigDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the configuration descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    DSM_EVENT                       dsmEvent;
    PUSB_CONFIGURATION_DESCRIPTOR   oldConfigurationDescriptor;
    PUSB_CONFIGURATION_DESCRIPTOR   configurationDescriptor;
    PUSB_DEVICE_DESCRIPTOR          deviceDescriptor;
    VALIDATION_DEVICE_INFO          devInfo;
    BOOLEAN                         supportsStreams;

    TRY {

        dsmEvent = DsmEventOperationSuccess;
        supportsStreams = FALSE;

        if (DeviceContext->ConfigurationDescriptor == NULL) {

            configurationDescriptor = (PUSB_CONFIGURATION_DESCRIPTOR) DeviceContext->ScratchBuffer;

        } else {

            configurationDescriptor = DeviceContext->ConfigurationDescriptor;

        }

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        if (FALSE == HUBDESC_ValidateConfigurationDescriptorSet(configurationDescriptor,
                                                                DeviceContext->ControlRequest.NumberOfBytes,
                                                                &devInfo,
                                                                DeviceContext->PortContext->IfrLog,
                                                                &supportsStreams)) {
            
             DeviceContext->EnumMsgId = USBENUM_BAD_CONFIG_DESCRIPTOR;

             DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE:Parsing of device's configuration descriptor set failed");

            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }

        // 
        // Verify that the new wTotalLength returned is same as the one returned
        // in the header(or in the first call). We do this after calling the 
        // validation function, so that other failures gets logged before bailing
        // out due to this check
        //
        oldConfigurationDescriptor = (PUSB_CONFIGURATION_DESCRIPTOR) DeviceContext->ScratchBuffer;
        if (configurationDescriptor->wTotalLength !=
                    oldConfigurationDescriptor->wTotalLength) {
                DeviceContext->EnumMsgId = USBENUM_BAD_CONFIG_DESCRIPTOR;

                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                        TL_ERROR,
                        Device,
                        "HW_COMPLIANCE:Configuration Descriptor Validation Failed due to value of ConfigDesc->wTotalLength (0x%x) exceeding the buffer length (0x%x).",
                        configurationDescriptor->wTotalLength,
                        oldConfigurationDescriptor->wTotalLength);
            LOG_DESCRIPTOR_VALIDATION_ERROR(&devInfo, 
                                            DescriptorValidationErrorConfigWTotalLengthTooLarge);

            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }

        INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ConfigDescIsValid);

        if (supportsStreams) {
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_SQM_PROPERTIES, &DeviceContext->DeviceSqmFlags, SupportsStreams);
        }

        deviceDescriptor = &DeviceContext->DeviceDescriptor;

        if (deviceDescriptor->bNumConfigurations > 1) {
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_SQM_PROPERTIES, &DeviceContext->DeviceSqmFlags, SupportsMultipleConfigurations);
        }
        
        //
        // Determine if we have a composite device
        //
        if ((deviceDescriptor->bNumConfigurations == 1) &&
            (configurationDescriptor->bNumInterfaces > 1) &&
            ((deviceDescriptor->bDeviceClass == 0) ||
             ((deviceDescriptor->bDeviceClass == 0xEF)  &&
              (deviceDescriptor->bDeviceSubClass == 0x02) &&
              (deviceDescriptor->bDeviceProtocol == 0x01)))) {

            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, DeviceIsComposite);
        }

        if (DeviceContext->ConfigurationDescriptor == NULL) {

            //
            // Allocate a buffer big enough for the configuration descriptor
            //
            DeviceContext->ConfigurationDescriptor =
                ExAllocatePoolWithTag(NonPagedPool,
                                      configurationDescriptor->wTotalLength,
                                      USBHUB3_DEVICE_TAG);

            if (DeviceContext->ConfigurationDescriptor == NULL) {

                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to allocate space for configuration descriptor");

                dsmEvent = DsmEventOperationFailure;
                LEAVE;
            }

            RtlCopyMemory(DeviceContext->ConfigurationDescriptor,
                          configurationDescriptor,
                          configurationDescriptor->wTotalLength);
        
            configurationDescriptor = DeviceContext->ConfigurationDescriptor;
        }

        //
        // Find the first interface in the configuration descriptor.
        // This will be used to generate the PnP IDs.
        //
        DeviceContext->CompatIdInterfaceDescriptor =
            HUBDESC_ParseConfigurationDescriptor(configurationDescriptor,
                                                 (PVOID) configurationDescriptor,
                                                 -1,
                                                 -1,
                                                 -1,
                                                 -1,
                                                 -1,
                                                 NULL,
                                                 DeviceContext->PortContext->IfrLog);


        if (DeviceContext->CompatIdInterfaceDescriptor != NULL) {
            
            //
            // Determine if device is a hub
            //
            if (DeviceContext->CompatIdInterfaceDescriptor->bInterfaceClass == 
                USB_DEVICE_CLASS_HUB) {
                
                INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, DeviceIsHub);
                INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, DecorateSerialNumber);

            } else if ((DeviceContext->DeviceFlags.DeviceIsComposite == 0) &&
                       (DeviceContext->DeviceDescriptor.bcdUSB > 0x0200) &&
                       (DeviceContext->DeviceHackFlags.DisableUASP == 0) && 
                       (((DeviceContext->UsbDeviceInfo.DeviceSpeed == UsbSuperSpeed) &&
                         (DeviceContext->HubFdoContext->ControllerFlags.SuperSpeedStreamsSupported == 1)) ||
                        ((DeviceContext->UsbDeviceInfo.DeviceSpeed == UsbHighSpeed)))) {

                //
                // The UASP spec recommends that devices implement BOT as the "default"
                // interface descriptor so that they can achieve interoperability with the vast
                // majority of hosts out there.  Then, the spec recommends specifying an alternate
                // setting that contain the UASP class codes.  
                // If such an alternate setting exists, we should enumerate the PDO using Compatible IDs built
                // from the alternate setting interface descriptor instead of the default one.  
                //
                // In order to mitigate the risk of changing the enumeration behavior
                // with devices that may just happen to have such an alternate setting, but don't intend to 
                // implement UASP, we will only support this feature on devices reporting a bcdUSB version
                // greater that 2.0.
                //
                // We do NOT want to use UAS in cases where the device is running at SuperSpeed on a
                // controller that does not support streams, or if the device is running at full-speed or low-speed.
                // 
                PUSB_INTERFACE_DESCRIPTOR uaspDescriptor;
                uaspDescriptor = 
                    HUBDESC_ParseConfigurationDescriptor(configurationDescriptor,
                                                         DeviceContext->CompatIdInterfaceDescriptor,
                                                         DeviceContext->CompatIdInterfaceDescriptor->bInterfaceNumber,
                                                         -1,
                                                         USB_DEVICE_CLASS_STORAGE,
                                                         USB_STORAGE_SUBCLASS_SCSI,
                                                         USB_STORAGE_PROTOCOL_UAS,
                                                         NULL,
                                                         DeviceContext->PortContext->IfrLog);

                if (uaspDescriptor != NULL) {
                    INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, DecorateSerialNumber);
                    DeviceContext->CompatIdInterfaceDescriptor = uaspDescriptor;
                }

            }

        }

        //
        // We don't support composite hubs
        //
        if((DeviceContext->DeviceFlags.DeviceIsHub == 1) && 
            (DeviceContext->DeviceFlags.DeviceIsComposite == 1)) {
            
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Composite hubs are not supported.");
            NT_ASSERTMSG("Composite hubs are not supported", FALSE);
        }

        if (configurationDescriptor->bmAttributes & USB_CONFIG_REMOTE_WAKEUP) {

            //
            // Set if the device can wake-up the USB
            //
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, UsbWakeupSupport);

        }

        //
        // Fail enumeration if device is marked as being incompatible with Windows.
        // This is done after successfully retrieving the device and configuration
        // descriptors so we can enumerate with the correct device and compat IDs.
        // This allows the device to be identifiable in Device Manager.  It is also
        // done at this point so DSM knows the device is bad, rather than waiting until
        // the PDO is created.  This allows DSM to handle the device appropriately.
        //
        if (DeviceContext->DeviceHackFlags.IncompatibleWithWindows == 1) {
            DeviceContext->EnumMsgId = USBENUM_INCOMPATIBLE_WITH_WINDOWS;
            
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Failing enumeration due to IncompatibleWithWindows flag being set");

            dsmEvent = DsmEventOperationFailure;
        }

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_CONFIGURATION_DESCRIPTOR(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }
    }

    return dsmEvent;
}


DSM_EVENT
HUBDTX_ValidateAndCacheDeviceDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the device descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    PUSB_DEVICE_DESCRIPTOR  deviceDescriptor;
    DSM_EVENT               dsmEvent;
    VALIDATION_DEVICE_INFO  devInfo;
    BOOLEAN                 isBillboard;

    TRY {

        dsmEvent = DsmEventOperationSuccess;
        isBillboard = FALSE;

        deviceDescriptor = (PUSB_DEVICE_DESCRIPTOR) DeviceContext->ScratchBuffer;

        if (DeviceContext->ControlRequest.NumberOfBytes != sizeof(USB_DEVICE_DESCRIPTOR)) {
            
            if (DeviceContext->ControlRequest.NumberOfBytes != 0) {
                DeviceContext->EnumMsgId = USBENUM_BAD_DEVICE_DESCRIPTOR;

                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Device Descriptor request returned incorrect number of bytes (%d returned, %d expected)",
                           DeviceContext->ControlRequest.NumberOfBytes,
                           sizeof(USB_DEVICE_DESCRIPTOR));   
            }
            else {
                DeviceContext->EnumMsgId = USBENUM_DEVICE_DESCRIPTOR_FAILURE;
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "HW_COMPLIANCE: Enumeration Failure: Device Descriptor request returned zero bytes.");
            }   

            dsmEvent = DsmEventOperationFailure;
            LEAVE;

        }
        
        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        if (FALSE == HUBDESC_ValidateDeviceDescriptor(deviceDescriptor,
                                                      DeviceContext->ControlRequest.NumberOfBytes,
                                                      &devInfo,
                                                      &isBillboard,
                                                      DeviceContext->PortContext->IfrLog)) {

            DeviceContext->EnumMsgId = USBENUM_BAD_DEVICE_DESCRIPTOR;
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Device Descriptor validation failed. (%d bytes)",
                       DeviceContext->ControlRequest.NumberOfBytes);

            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }

        //
        // Device descriptor looks OK.  Cache in the device context.
        //
        RtlCopyMemory(&DeviceContext->DeviceDescriptor,
                      deviceDescriptor,
                      sizeof(DeviceContext->DeviceDescriptor));

        //
        // Billboard class is treated as a special case for validation 
        // also for generating compat id based on device descriptor
        //
        if (isBillboard) {
            
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, DeviceIsBillboard);
        }
        

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_DEVICE_DESCRIPTOR(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }
    }

    return dsmEvent;
}

DSM_EVENT
HUBDTX_ValidateFirstDeviceDescriptorFor20  (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the device descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    PUSB_DEVICE_DESCRIPTOR  deviceDescriptor;
    DSM_EVENT               dsmEvent;
    VALIDATION_DEVICE_INFO  devInfo;

    TRY {

        dsmEvent = DsmEventOperationSuccess;

        deviceDescriptor = (PUSB_DEVICE_DESCRIPTOR) DeviceContext->ScratchBuffer;

        //
        // For maintaining comapt with WIN7 stack, we need to pass
        // this validation as long as the number of bytes returned 
        // is 8
        //

        if (DeviceContext->ControlRequest.NumberOfBytes < 8) {
            
            if (DeviceContext->ControlRequest.NumberOfBytes != 0) {
                DeviceContext->EnumMsgId = USBENUM_BAD_DEVICE_DESCRIPTOR;
            }
            else {
                DeviceContext->EnumMsgId = USBENUM_DEVICE_DESCRIPTOR_FAILURE;
            }

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Number of bytes returned for first device descriptor is invalid: %d",
                       DeviceContext->ControlRequest.NumberOfBytes);

            dsmEvent = DsmEventOperationFailure;
            LEAVE;

        }
        
        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        if (FALSE == HUBDESC_ValidateDeviceDescriptor(deviceDescriptor,
                                                      DeviceContext->ControlRequest.NumberOfBytes,
                                                      &devInfo,
                                                      NULL,
                                                      DeviceContext->PortContext->IfrLog)) {

            DeviceContext->EnumMsgId = USBENUM_BAD_DEVICE_DESCRIPTOR;

            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }
        
        //
        // We will need to update the max packet size
        //
        DeviceContext->MaxPacketSize = deviceDescriptor->bMaxPacketSize0;

        //
        // Copy only 8 bytes to the device descriptor as some legacy devices might
        // only return 8 bytes. It is important not to overwrite the VID PID of the 
        // device; that should only come from the second device descriptor as that 
        // is more reliable for some legacy devices. Because otherwise  when we compare 
        // the VID PID of the device after a re-enumeration, we would end 
        // up comparing the VID PIDs that we got during the re-enumeration itself
        //
        RtlCopyMemory(&DeviceContext->DeviceDescriptor,
                      deviceDescriptor,
                      8);   
        
        //
        // We will read the hack flags after we have obtained the second device 
        // descriptor. However there is one hack flag that we need to apply before
        // that happens, which is the DisableFastEnumeration as it will decide
        // whether we do the second reset or not. Ideally, we should not be hard
        // coding VID PID for this hack in the code. However, if we have to read
        // the KSE flags, we will have to go to PASSIVE level and we want to avoid
        // such a change because this is being done at a very last stage in the 
        // product cycle of Win8.1. In Win.Next, we should try to see if we can
        // remove the hard coded value.
        //
        











        

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_DEVICE_DESCRIPTOR(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }

    }

    return dsmEvent;
}


DSM_EVENT
HUBDTX_ValidateAndCacheDeviceQualifierDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the device qualifier descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    DSM_EVENT               dsmEvent;
    VALIDATION_DEVICE_INFO  devInfo;

    TRY {

        dsmEvent = DsmEventOperationSuccess;

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);


        if (DeviceContext->ControlRequest.NumberOfBytes < sizeof(USB_DEVICE_QUALIFIER_DESCRIPTOR)) {

            DeviceContext->EnumMsgId = USBENUM_BAD_DEVICE_QUALIFIER_DESCRIPTOR;

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Number of bytes returned for device qualifier descriptor is invalid: %d",
                       DeviceContext->ControlRequest.NumberOfBytes);

            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }

        //
        // The existence of the device qualifier descriptor implies the device is capable of operating at
        // high speed.
        //
        INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, DeviceIsHighSpeedCapable);

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_DEVICE_QUALIFIER_DESCRIPTOR(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }
    }

    return dsmEvent;
}



DSM_EVENT
HUBDTX_ValidateAndCacheLanguageIdStringDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the language ID string descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    DSM_EVENT               dsmEvent;
    PUSB_STRING_DESCRIPTOR  languageIdDescriptor;
    ULONG                   stringLength;
    VALIDATION_DEVICE_INFO  devInfo;


    TRY {

        dsmEvent = DsmEventOperationSuccess;

        languageIdDescriptor = (PUSB_STRING_DESCRIPTOR) DeviceContext->ScratchBuffer;

        stringLength = DeviceContext->ControlRequest.NumberOfBytes;

        NT_ASSERT(stringLength <= MAXIMUM_USB_STRING_LENGTH);

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        if (FALSE == HUBDESC_ValidateStringDescriptor(languageIdDescriptor,
                                                      stringLength,
                                                      DeviceContext->PortContext->IfrLog,
                                                      &devInfo)) {            
            
            DeviceContext->EnumMsgId = USBENUM_BAD_LANGUAGE_ID_STRING_DESCRIPTOR;
            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }


        NT_ASSERT(DeviceContext->LanguageIdDescriptor == NULL);

        stringLength = languageIdDescriptor->bLength;

        //
        // Language ID string descriptor looks OK.  Allocate a buffer for it and cache it.
        //
        DeviceContext->LanguageIdDescriptor = ExAllocatePoolWithTag (NonPagedPool,
                                                                     stringLength,
                                                                     USBHUB3_DEVICE_TAG); 

        if (DeviceContext->LanguageIdDescriptor == NULL) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to allocate space for language ID descriptor");

            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }

        RtlCopyMemory(DeviceContext->LanguageIdDescriptor,
                      languageIdDescriptor,
                      stringLength);

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_LANGUAGE_ID_DESCRIPTOR(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }

    }

    return dsmEvent;
}


DSM_EVENT
HUBDTX_ValidateAndCacheMSOSContainerIdDescriptorHeader (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the MS OS container ID descriptor header.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    DSM_EVENT                       dsmEvent;
    PMS_CONTAINER_ID_DESC_HEADER    msContainerIdDescHeader;
    VALIDATION_DEVICE_INFO          devInfo;

    TRY {

        dsmEvent = DsmEventOperationSuccess;

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        msContainerIdDescHeader = (PMS_CONTAINER_ID_DESC_HEADER) DeviceContext->ScratchBuffer;

        if (FALSE == HUBDESC_ValidateMSOSContainerIDDescriptorHeader(msContainerIdDescHeader,
                                                                     DeviceContext->ControlRequest.NumberOfBytes,
                                                                     &devInfo,
                                                                     DeviceContext->PortContext->IfrLog)) {
            
            DeviceContext->EnumMsgId = USBENUM_BAD_MS_CONTAINER_ID_DESCRIPTOR;
            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }

        //
        // MS OS Container ID Descriptor Header is valid.  No need to
        // cache it, as we will later read in the full descriptor.
        //

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_MSOS_CONTAINER_ID_DESCRIPTOR_HEADER(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }
    }

    return dsmEvent;
}


VOID
HUBDTX_ClearEndpointHaltUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a CLEAR_FEATURE request to clear the device's ENDPOINT_HALT feature.
    If WdfRequestSend fails then DsmEventTransferFailure should be queued directly
    from this function.

    Completion routine of this function should queue a DsmEventTransferSuccess
    or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    WDFREQUEST                      request;
    WDF_REQUEST_PARAMETERS          parameters;
    PPIPE_HANDLE                    pipeHandle;
    UCHAR                           endpointAddress;

    TRY {

        request = DeviceContext->CurrentClientRequest;

        WDF_REQUEST_PARAMETERS_INIT(&parameters);
        WdfRequestGetParameters(request, &parameters );

        pipeHandle = DeviceContext->CurrentTargetPipe;

        if (pipeHandle == NULL) {
            DeviceContext->LastOperationUsbdStatus = USBD_STATUS_INVALID_PIPE_HANDLE;
            DeviceContext->LastOperationNtStatus =  USBD_STATUS_To_NTSTATUS(USBD_STATUS_INVALID_PIPE_HANDLE);
            status = DeviceContext->LastOperationNtStatus;
            LEAVE;
        }

        endpointAddress = pipeHandle->EndpointDescriptor->bEndpointAddress;

        //
        // Format the setup packet for Clear Feature
        // USB 3.0 Spec, 9.4.1
        // USB 2.0 Spec, 9.4.1
        //
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_ENDPOINT;
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_CLEAR_FEATURE;
        setupPacket->wValue.W = ENDPOINT_HALT;
        setupPacket->wIndex.W = endpointAddress;
        setupPacket->wLength = 0;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         FALSE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
        
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Clear Stall Request Submitted %!STATUS!",
                       status);

            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_GetMSOSContainerIdDescriptorHeaderUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the header of the vendor-
    defined MS OS Container ID descriptor.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function.

    Completion routine of this function should queue a DsmEventTransferSuccess
    or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS            status;

    TRY {

        status = HUBDTX_GetMsOsFeatureDescriptor( DeviceContext,
                                                  BMREQUEST_TO_DEVICE,
                                                  0,
                                                  MS_CONTAINER_ID_DESCRIPTOR_INDEX,
                                                  DeviceContext->ScratchBuffer,
                                                  sizeof(MS_CONTAINER_ID_DESC_HEADER) );

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "MSOS Container ID Descriptor Header Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent( DeviceContext,
                               DsmEventTransferFailure);
        }
    }
}


DSM_EVENT
HUBDTX_ValidateAndCacheMSOSContainerIdDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the MS OS container ID descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    DSM_EVENT               dsmEvent;
    PMS_CONTAINER_ID_DESC   msContainerIdDesc;
    VALIDATION_DEVICE_INFO  devInfo;

    TRY {

        dsmEvent = DsmEventOperationSuccess;

        msContainerIdDesc = (PMS_CONTAINER_ID_DESC) DeviceContext->ScratchBuffer;

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        if (FALSE == HUBDESC_ValidateMSOSContainerIDDescriptor(msContainerIdDesc,
                                                               DeviceContext->ControlRequest.NumberOfBytes,
                                                               &devInfo,
                                                               DeviceContext->PortContext->IfrLog)) {
            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }

        //
        // Container ID looks valid.  Cache it.
        //
        RtlCopyMemory(&DeviceContext->ContainerId,
                      &msContainerIdDesc->Data.bContainerID,
                      sizeof(GUID));

        INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, ContainerIdValid);

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_MSOS_CONTAINER_ID_DESCRIPTOR(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }
}

    return dsmEvent;
}


DSM_EVENT
HUBDTX_ValidateAndCacheMSOSDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the MS OS descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    DSM_EVENT               dsmEvent;
    VALIDATION_DEVICE_INFO  devInfo;


    TRY {

        if (DeviceContext->ControlRequest.NumberOfBytes != sizeof(OS_STRING)) {
            
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Number of bytes returned for MSOS descriptor is invalid: %d",
                       DeviceContext->ControlRequest.NumberOfBytes);

            dsmEvent = DsmEventOperationFailure;
            LEAVE;

        }

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        if (FALSE == HUBDESC_ValidateMSOSDescriptor(&DeviceContext->MsOsDescriptor,
                                                    sizeof(OS_STRING),
                                                    &devInfo)) {
            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }

        DeviceContext->MsOsVendorCode = DeviceContext->MsOsDescriptor.bVendorCode;
                                     
        dsmEvent = DsmEventOperationSuccess;

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_MSOS_DESCRIPTOR(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }
    }

    return dsmEvent;
}


DSM_EVENT
HUBDTX_ValidateAndCacheMSOSExtendedConfigDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the MS OS Extended Configuration descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    DSM_EVENT                       dsmEvent;
    PMS_EXT_CONFIG_DESC_HEADER      msExtConfigDescHeader;
    VALIDATION_DEVICE_INFO          devInfo;


    TRY {

        dsmEvent = DsmEventOperationSuccess;

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        //
        // The scratch buffer should still contain the MS OS Configuration Descriptor Header
        //
        msExtConfigDescHeader = (PMS_EXT_CONFIG_DESC_HEADER) DeviceContext->ScratchBuffer;

        if (DeviceContext->ControlRequest.NumberOfBytes != msExtConfigDescHeader->dwLength) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Number of bytes returned for MS OS Extended Configuration Descriptor is invalid: %d",
                       DeviceContext->ControlRequest.NumberOfBytes );

            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }

        if (FALSE == HUBDESC_ValidateMSOSExtendedConfigDescriptor(DeviceContext->MsExtConfigDesc,
                                                                  DeviceContext->ControlRequest.NumberOfBytes,
                                                                  &devInfo,
                                                                  DeviceContext->PortContext->IfrLog)) {
            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_MSOS_EXTENDED_CONFIGURATION_DESCRIPTOR(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }

        if ((dsmEvent == DsmEventOperationFailure) && (DeviceContext->MsExtConfigDesc != NULL)) {

                ExFreePoolWithTag(DeviceContext->MsExtConfigDesc, USBHUB3_DEVICE_TAG);
                DeviceContext->MsExtConfigDesc = NULL;
        }
    }

    return dsmEvent;
}


DSM_EVENT
HUBDTX_ValidateAndCacheMSOSExtendedConfigDescriptorHeader (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the MSOS Extended Configuration descriptor header descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    PMS_EXT_CONFIG_DESC_HEADER  msExtConfigDescHeader;
    DSM_EVENT                   dsmEvent;
    VALIDATION_DEVICE_INFO      devInfo;


    TRY {

        dsmEvent = DsmEventOperationSuccess;

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        msExtConfigDescHeader = (PMS_EXT_CONFIG_DESC_HEADER) DeviceContext->ScratchBuffer;


        if (FALSE == HUBDESC_ValidateMSOSExtendedConfigDescriptorHeader(msExtConfigDescHeader,
                                                                        DeviceContext->ControlRequest.NumberOfBytes,
                                                                        &devInfo,
                                                                        DeviceContext->PortContext->IfrLog)) {

            DeviceContext->EnumMsgId = USBENUM_BAD_MS_EXT_CONFIG_DESCRIPTOR;
            dsmEvent = DsmEventOperationFailure;
            LEAVE;

        }

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_MSOS_EXTENDED_CONFIGURATION_DESCRIPTOR_HEADER(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }
    }

    return dsmEvent;
}



DSM_EVENT
HUBDTX_ValidateAndCacheProductIdStringDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the product ID string descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{
    DSM_EVENT               dsmEvent;
    BOOLEAN                 result;
    PUSB_STRING_DESCRIPTOR  productIdStringDescriptor;
    ULONG                   stringLength;
    VALIDATION_DEVICE_INFO  devInfo;


    TRY {

        dsmEvent = DsmEventOperationSuccess;

        productIdStringDescriptor = (PUSB_STRING_DESCRIPTOR) DeviceContext->ScratchBuffer;

        stringLength = DeviceContext->ControlRequest.NumberOfBytes;

        NT_ASSERT(stringLength <= MAXIMUM_USB_STRING_LENGTH);

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        result = HUBDESC_ValidateStringDescriptor(productIdStringDescriptor,
                                                  stringLength,
                                                  DeviceContext->PortContext->IfrLog,
                                                  &devInfo);

        if (result == FALSE) {
            DeviceContext->EnumMsgId = USBENUM_BAD_PRODUCT_DESC_STRING;
            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }

        NT_ASSERT(DeviceContext->ProductIdStringDescriptor == NULL);
        stringLength = productIdStringDescriptor->bLength;
        
        //
        // Product ID string descriptor looks OK.  Allocate a buffer for it and cache it.
        //
        DeviceContext->ProductIdStringDescriptor =
            ExAllocatePoolWithTag (NonPagedPool,
                                   stringLength,
                                   USBHUB3_DEVICE_TAG); 

        if (DeviceContext->ProductIdStringDescriptor == NULL) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to allocate space for Product ID string descriptor");

            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }

        RtlCopyMemory(DeviceContext->ProductIdStringDescriptor,
                      productIdStringDescriptor,
                      stringLength);

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_PRODUCT_ID_STRING_DESCRIPTOR(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }
    }

    return dsmEvent;
}


DSM_EVENT
HUBDTX_ValidateAndCacheSerialNumberStringDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the serial number string descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{

    PUSB_STRING_DESCRIPTOR  stringDescriptor;
    ULONG                   stringLength;
    ULONG                   bufferLength;
    PUCHAR                  stringOffset;
    BOOLEAN                 result;
    DSM_EVENT               dsmEvent;
    VALIDATION_DEVICE_INFO  devInfo;

    TRY {

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        stringDescriptor = (PUSB_STRING_DESCRIPTOR) DeviceContext->ScratchBuffer;

        stringLength = DeviceContext->ControlRequest.NumberOfBytes;
        bufferLength = stringLength;

        NT_ASSERT(stringLength <= MAXIMUM_USB_STRING_LENGTH);

        result = HUBDESC_ValidateSerialNumberStringDescriptor(stringDescriptor,
                                                              &stringLength,
                                                              &devInfo,
                                                              DeviceContext->PortContext->IfrLog);
      
        if (result == FALSE) {
            DeviceContext->EnumMsgId = USBENUM_BAD_SERIAL_NUMBER;
            LEAVE;
        }

        HUBMISC_FreeBufferIfNotNull(DeviceContext->SerialNumberId.Buffer, USBHUB3_DEVICE_TAG);

        if (DeviceContext->DeviceStateFlags.DecorateSerialNumber == 1) {
            //
            // If there is a hub with a serial number, we need to decorate it, otherwise the
            // instance ID will be the same across the different driver stacks, resulting in PnP using the same
            // devnode to decide which driver to load.  This would result in the wrong hub driver being loaded.
            // Also, due to 3.0 hubs setting the same serial number for the 2.0 and 3.0 parts, we 
            // need to decorate those differently as well.  Also, for superspeed UAS devices on 
            // controllers without streams support, we need to decorate those serial numbers as well
            // 
            bufferLength += USB_SERIAL_NUMBER_DECORATION_SIZE;

        }

        DeviceContext->SerialNumberId.Buffer =
            ExAllocatePoolWithTag(NonPagedPool,
                                  bufferLength,
                                  USBHUB3_DEVICE_TAG);

        if (DeviceContext->SerialNumberId.Buffer == NULL) {
            result = FALSE;
            LEAVE;
        }

        DeviceContext->SerialNumberId.LengthInBytes = bufferLength;

        RtlZeroMemory(DeviceContext->SerialNumberId.Buffer, bufferLength);
        stringOffset = (PUCHAR)DeviceContext->SerialNumberId.Buffer;

        //
        // Copy the serial number over
        //
        if (DeviceContext->DeviceFlags.DeviceIsHub == 1) {

            if (DeviceContext->DeviceDescriptor.bcdUSB >= 0x0300) {
                RtlCopyMemory(stringOffset,
                              USB_30HUB_SERIAL_NUMBER_DECORATION,
                              USB_SERIAL_NUMBER_DECORATION_SIZE);
                stringOffset += USB_SERIAL_NUMBER_DECORATION_SIZE;
            } else {
                RtlCopyMemory(stringOffset,
                              USB_20HUB_SERIAL_NUMBER_DECORATION,
                              USB_SERIAL_NUMBER_DECORATION_SIZE);
                stringOffset += USB_SERIAL_NUMBER_DECORATION_SIZE;
            }

        } else if (DeviceContext->DeviceStateFlags.DecorateSerialNumber == 1) {
            RtlCopyMemory(stringOffset,
                          USB_SERIAL_NUMBER_DECORATION,
                          USB_SERIAL_NUMBER_DECORATION_SIZE);
            stringOffset += USB_SERIAL_NUMBER_DECORATION_SIZE;
        }

        RtlCopyMemory(stringOffset,
                      stringDescriptor->bString,
                      stringLength - sizeof(WCHAR));

        INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, SerialNumber);

    } FINALLY {

        if (result) {
            dsmEvent = DsmEventOperationSuccess;
        } else {
            dsmEvent = DsmEventOperationFailure;

            EventWriteUSBHUB3_ETW_EVENT_INVALID_SERIAL_NUMBER_STRING_DESCRIPTOR(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }



    }

    return dsmEvent;
}

VOID
HUBDTX_LogBillboardEvent (
    __in
        PDEVICE_CONTEXT         DeviceContext,
    __in_opt
        PUSB_STRING_DESCRIPTOR  BillboardString 
    )
/*++

Routine Description:

    Logs information for the billboard descriptor including
    the optional string  for AdditionalInfoUrl

Returns:


--*/
{

    PUSB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR         billboardDescriptor;
    WCHAR                                               nullTerminatedString[128];
    UCHAR                                               stringLength;



    billboardDescriptor = DeviceContext->BillboardInfo->BillboardDescriptor;

    if (BillboardString != NULL) {
        
        stringLength =  BillboardString->bLength - sizeof (USB_COMMON_DESCRIPTOR);
        
        RtlZeroMemory(nullTerminatedString,
                      stringLength + sizeof(UNICODE_NULL));

        RtlCopyMemory(nullTerminatedString, 
                      BillboardString->bString, 
                      stringLength);

    }


    DbgTraceTo(DeviceContext->PortContext->IfrLog, TL_INFO, Device,
               "Billboard Detected. bNumberOfAlternateModes = 0x%x, bPreferredAlternateMode = 0x%x",
               billboardDescriptor->bNumberOfAlternateModes,
               billboardDescriptor->bPreferredAlternateMode);
    
    EventWriteUSBHUB3_ETW_EVENT_BILLBOARD(
        &DeviceContext->DsmContext.CurrentActivityId,
        DeviceContext->UsbDevice, 
        billboardDescriptor->bNumberOfAlternateModes,
        billboardDescriptor->bPreferredAlternateMode,
        billboardDescriptor->VconnPower.AsUshort,
        (BillboardString == NULL) ? NULL : nullTerminatedString); 

}

DSM_EVENT
HUBDTX_ValidateAndCacheBillboardStringDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the Billboard AdditionalInfoUrl 
    string descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{

    DSM_EVENT               dsmEvent;
    BOOLEAN                 result;
    PUSB_STRING_DESCRIPTOR  billboardStringDescriptor;
    ULONG                   stringLength;
    VALIDATION_DEVICE_INFO  devInfo;


    TRY {

        dsmEvent = DsmEventOperationSuccess;

        billboardStringDescriptor = (PUSB_STRING_DESCRIPTOR) DeviceContext->ScratchBuffer;

        stringLength = DeviceContext->ControlRequest.NumberOfBytes;

        NT_ASSERT(stringLength <= MAXIMUM_USB_STRING_LENGTH);

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        result = HUBDESC_ValidateStringDescriptor(billboardStringDescriptor,
                                                  stringLength,
                                                  DeviceContext->PortContext->IfrLog,
                                                  &devInfo);

        if (result == FALSE) {
            DeviceContext->EnumMsgId = USBENUM_BAD_BILLBOARD_ADDITIONAL_INFO_URL_STRING;
            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }    

        HUBDTX_LogBillboardEvent(DeviceContext,
                                 billboardStringDescriptor);

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_BILLBOARD_STRING_DESCRIPTOR(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }
    }

    return dsmEvent;
}

VOID
HUBDTX_LogAlternateMode (
    __in
        PDEVICE_CONTEXT         DeviceContext,
    __in_opt
        PUSB_STRING_DESCRIPTOR  AlternateModeString

    )
/*++

Routine Description:

    Logs information for the current alterante mode including
    the optional string descriptor

Returns:


--*/
{
    PBILLBOARD_INFO     billboardInfo;
    PUSB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR
                        billboardDescriptor;
    UCHAR               currentMode;
    UCHAR               configured;
    WCHAR               nullTerminatedString[128];
    UCHAR               stringLength;
    

    TRY {
        billboardInfo = DeviceContext->BillboardInfo;

        billboardDescriptor = billboardInfo->BillboardDescriptor;
        currentMode = billboardInfo->CurrentAlternateMode;
        configured = billboardDescriptor->bmConfigured[currentMode / 4];
        configured = (configured >> ((currentMode % 4) * 2)) & 0x3;                 

        if (AlternateModeString != NULL) {
            
            stringLength = AlternateModeString->bLength - sizeof(USB_COMMON_DESCRIPTOR);

            RtlZeroMemory(nullTerminatedString,
                          stringLength + sizeof(UNICODE_NULL));

            RtlCopyMemory(nullTerminatedString, 
                          AlternateModeString->bString, 
                          stringLength);

        }

        if (configured == 0x3) {

            billboardInfo->AlternateModeSuccess = TRUE;

            DbgTraceTo(DeviceContext->PortContext->IfrLog, TL_INFO, Device,
                       "Alternate mode configured successfully. wSVID = 0x%x, bAlternateMode = 0x%x",
                       billboardDescriptor->AlternateMode[currentMode].wSVID,
                       billboardDescriptor->AlternateMode[currentMode].bAlternateMode);

            EventWriteUSBHUB3_ETW_EVENT_ALTERNATE_MODE_ENTERED(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice, 
                billboardDescriptor->AlternateMode[currentMode].wSVID,
                billboardDescriptor->AlternateMode[currentMode].bAlternateMode,
                (AlternateModeString == NULL) ? NULL : nullTerminatedString,
                configured);

        } else {

            billboardInfo->AlternateModeError = TRUE;

            DbgTraceTo(DeviceContext->PortContext->IfrLog, TL_INFO, Device,
                       "Alternate mode failed. wSVID = 0x%x, bAlternateMode = 0x%x, Configured = 0x%x",
                       billboardDescriptor->AlternateMode[currentMode].wSVID,
                       billboardDescriptor->AlternateMode[currentMode].bAlternateMode,
                       configured);
            
            EventWriteUSBHUB3_ETW_EVENT_ALTERNATE_MODE_FAILED(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice, 
                billboardDescriptor->AlternateMode[currentMode].wSVID,
                billboardDescriptor->AlternateMode[currentMode].bAlternateMode,
                (AlternateModeString == NULL) ? NULL : nullTerminatedString,
                configured);   
        }

            

    } FINALLY {

    }

}   

DSM_EVENT
HUBDTX_ValidateAndCacheAlternateModeStringDescriptor (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Performs validation of the alternate mode string descriptor.

Returns:

    DsmEventOperationSuccess
    DsmEventOperationFailure

--*/
{

    DSM_EVENT               dsmEvent;
    BOOLEAN                 result;
    PUSB_STRING_DESCRIPTOR  alternateModeStringDescriptor;
    ULONG                   stringLength;
    VALIDATION_DEVICE_INFO  devInfo;


    TRY {

        dsmEvent = DsmEventOperationSuccess;

        alternateModeStringDescriptor = (PUSB_STRING_DESCRIPTOR) DeviceContext->ScratchBuffer;

        stringLength = DeviceContext->ControlRequest.NumberOfBytes;

        NT_ASSERT(stringLength <= MAXIMUM_USB_STRING_LENGTH);

        HUBDESC_InitializeDeviceInfo(DeviceContext,
                                     &devInfo);

        result = HUBDESC_ValidateStringDescriptor(alternateModeStringDescriptor,
                                                  stringLength,
                                                  DeviceContext->PortContext->IfrLog,
                                                  &devInfo);

        if (result == FALSE) {
            DeviceContext->EnumMsgId = USBENUM_BAD_ALTERNATE_MODE_DESC_STRING;
            dsmEvent = DsmEventOperationFailure;
            LEAVE;
        }

        stringLength = alternateModeStringDescriptor->bLength;
        
        HUBDTX_LogAlternateMode(DeviceContext,
                                alternateModeStringDescriptor);

    } FINALLY {

        if (dsmEvent == DsmEventOperationFailure) {
            EventWriteUSBHUB3_ETW_EVENT_INVALID_ALTERNATE_MODE_STRING_DESCRIPTOR(
                &DeviceContext->DsmContext.CurrentActivityId,
                DeviceContext->UsbDevice);
        }

        DeviceContext->BillboardInfo->CurrentAlternateMode++;      

    }

    return dsmEvent;
}

VOID
HUBDTX_CacheBillboardInfo(
    __in
        PDEVICE_CONTEXT             DeviceContext,
    __in
        PUSB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR             
                                    BillboardDescriptor
    )
/*++

Routine Description:
    
    Allocates and initializes the billBoard information
    and also caches the billBoard descriptor.

Arguments:

Returns:

--*/
{
  
    PBILLBOARD_INFO     billboardInfo;

    TRY { 

        billboardInfo = ExAllocatePoolWithTag(NonPagedPool,
                                              sizeof(BILLBOARD_INFO),
                                              USBHUB3_HUB_TAG);   
        if (billboardInfo == NULL) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog, TL_ERROR, Device,
                       "Memory could not be allocated for caching billboard descriptor");
            LEAVE;
        }
        
        RtlZeroMemory(billboardInfo,
                      sizeof(BILLBOARD_INFO));

        billboardInfo->CurrentAlternateMode = INVALID_ALTERNATE_MODE;
        billboardInfo->BillboardDescriptor = ExAllocatePoolWithTag(NonPagedPool,
                                                                    BillboardDescriptor->bLength,
                                                                    USBHUB3_HUB_TAG);

        if (billboardInfo->BillboardDescriptor == NULL) {
            //
            // It is not fatal to not being able to cache the Billboard descriptor.
            // Ignore the error
            //      
            DbgTraceTo(DeviceContext->PortContext->IfrLog, TL_ERROR, Device,
                       "Memory could not be allocated for caching billboard descriptor");

            ExFreePoolWithTag(billboardInfo,
                              USBHUB3_HUB_TAG);
            billboardInfo = NULL;
            LEAVE;
        }
        
        RtlCopyMemory(billboardInfo->BillboardDescriptor,
                      BillboardDescriptor,
                      BillboardDescriptor->bLength);


    } FINALLY {
        DeviceContext->BillboardInfo = billboardInfo;
    }

}

DSM_EVENT
HUBDTX_CheckIfBillboardBOSDescriptorIsPresent (
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    Checks if the device BOS descriptor set contains a Billboard
    capability descriptor.

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT retEvent;

    TRY {

        if (DeviceContext->BillboardInfo == NULL) {
            retEvent = DsmEventNo;
        } else {
            retEvent = DsmEventYes;
        }

    } FINALLY {

    }

    return retEvent;
}

DSM_EVENT
HUBDTX_CheckIfBillboardStringDescriptorShouldBeQueried (
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    Checks if Billboard descriptor has the string descriptor
    for AdditionalInfoUrl

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT           retEvent;

    TRY {

        NT_ASSERT(DeviceContext->BillboardInfo != NULL);

        if (DeviceContext->BillboardInfo->BillboardDescriptor->iAddtionalInfoURL != 0) {

            retEvent = DsmEventYes;

        } else {

            retEvent = DsmEventNo;
            HUBDTX_LogBillboardEvent(DeviceContext,
                                     NULL);
        }

    } FINALLY {

    }

    return retEvent;
}

DSM_EVENT
HUBDTX_CheckIfAlternateModeStringDescriptorShouldBeQueried (
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    Checks if there are any alternate modes remaining which have
    have the string descriptor. It it finds such an alternate mode,
    it returns DsmEventYes. If it reaches the last alternate mode
    without finding one, it fires the WNF event for the UX and 
    returns DsmEventNo

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT           retEvent;
    PBILLBOARD_INFO     billboardInfo;
    PUSB_DEVICE_CAPABILITY_BILLBOARD_DESCRIPTOR
                        billboardDescriptor;


    TRY {
        billboardInfo = DeviceContext->BillboardInfo;
        NT_ASSERT(billboardInfo != NULL);

        billboardDescriptor = billboardInfo->BillboardDescriptor;

        if (billboardInfo->CurrentAlternateMode == INVALID_ALTERNATE_MODE) {
            billboardInfo->CurrentAlternateMode = 0;
        }

        while((billboardInfo->CurrentAlternateMode < billboardDescriptor->bNumberOfAlternateModes) &&
               (billboardDescriptor->AlternateMode[billboardInfo->CurrentAlternateMode].iAlternateModeSetting == 0)) {
            
            HUBDTX_LogAlternateMode(DeviceContext,
                                    NULL);
            billboardInfo->CurrentAlternateMode++;

        }

        if (billboardInfo->CurrentAlternateMode == billboardDescriptor->bNumberOfAlternateModes) {
            
            billboardInfo->CurrentAlternateMode = INVALID_ALTERNATE_MODE;
            retEvent = DsmEventNo;
            LEAVE;
        }

        retEvent = DsmEventYes;

    } FINALLY {

    }

    return retEvent;
}

DSM_EVENT
HUBMISC_CheckIfBOSDescriptorQueryShouldBeSkipped (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Detemines if the BOS descriptor query should be skipped for this device

Returns:

    DsmEventYes
    DsmEventNo

--*/
{
    DSM_EVENT   responseEvent;

    TRY {
        if ((DeviceContext->DeviceHackFlags.SkipBOSDescriptorQuery == 1) || 
            (DeviceContext->DeviceDescriptor.bcdUSB <= 0x200)) {

            responseEvent = DsmEventYes;
            
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_INFO,
                       Device,
                       "BOS descriptor query will be skipped for device 0x%p",
                       DeviceContext);

        } else {
            responseEvent = DsmEventNo;
        }

    } FINALLY {

    }

    return responseEvent;
}


DSM_EVENT
HUBMISC_GetGenericErrorResponseOnDescriptorFailure (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Determines the generic response for descriptor  failures during enumeration.
    For 1.1 and 2.0 devices the failure is ignored.  For 2.01 and later devices the
    enumeration is failed.

Returns:

    DsmEventDeviceIgnoreError
    DsmEventDeviceFailEnumeration

--*/
{
    DSM_EVENT   responseEvent;

    TRY {
        
        if (DeviceContext->DeviceDescriptor.bcdUSB <= 0x200) {

            responseEvent = DsmEventDeviceIgnoreError;
            
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Ignoring descriptor failure for device 0x%p",
                       DeviceContext);           
        } else {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HW_COMPLIANCE: Failing enumeration for device 0x%p due to descriptor failure",
                       DeviceContext);

            responseEvent = DsmEventDeviceFailEnumeration; 

        }

    } FINALLY {

    }

    return responseEvent;
}


VOID
HUBDTX_GetMSOSExtendedPropertyDescriptorUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the complete
    MS OS Extended Property descriptor.  On successful completion
    a pointer/handle for the descriptor is stored in the DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                    status;
    PMS_EXT_PROP_DESC_HEADER    msExtPropertyDescHeader;           

    TRY {

        msExtPropertyDescHeader = (PMS_EXT_PROP_DESC_HEADER)DeviceContext->ScratchBuffer;

        status = HUBDTX_GetMsOsFeatureDescriptor(DeviceContext,
                                                 BMREQUEST_TO_INTERFACE,
                                                 0,
                                                 MS_EXT_PROP_DESCRIPTOR_INDEX,
                                                 DeviceContext->MsExtPropertyDesc,
                                                 msExtPropertyDescHeader->dwLength);

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "MS OS Extended Property Descriptor Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}


VOID
HUBDTX_GetMSOSExtendedPropertyDescriptorHeaderUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a GET_DESCRIPTOR request to the device for the header of the
    vendor-defined MS OS Extended Property descriptor.  On successful
    completion a pointer/handle for the header is stored in the DSM context.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued
    directly from this function. Completion routine of this function should
    queue a DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS        status;

    TRY {

        status = HUBDTX_GetMsOsFeatureDescriptor(DeviceContext,
                                                 BMREQUEST_TO_INTERFACE,
                                                 0,
                                                 MS_EXT_PROP_DESCRIPTOR_INDEX,
                                                 DeviceContext->ScratchBuffer,
                                                 sizeof(MS_EXT_PROP_DESC_HEADER));

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "MSOS Extended Property Descriptor Header Request Submitted %!STATUS!",
                       status);

            //
            // Notify the DSM of of the failure to retrieve the device descriptor
            //
            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }
}

VOID
HUBDTX_DisableU1UsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a CLEAR_FEATURE for U1 to the device using a control transfer.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued directly
    from this function. Completion routine of this function should queue a
    DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;
        
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;
        setupPacket->bRequest = USB_REQUEST_CLEAR_FEATURE;
        setupPacket->wLength = 0;
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->wValue.W = USB_FEATURE_U1_ENABLE;
        setupPacket->wIndex.W = 0;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to send request to disable U1 on device %!STATUS!",
                       status);

            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        } 
    }    

}

VOID
HUBDTX_EnableU1UsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a SET_FEATURE for U1 to the device using a control transfer.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued directly
    from this function. Completion routine of this function should queue a
    DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;
        
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;
        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wLength = 0;
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->wValue.W = USB_FEATURE_U1_ENABLE;
        setupPacket->wIndex.W = 0;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to send request to enable U1 on device %!STATUS!",
                       status);

            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        } 
    }    

}

VOID
HUBDTX_EnableLTMUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a SET_FEATURE for LTM_ENABLE to the device using a control transfer.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued directly
    from this function. Completion routine of this function should queue a
    DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;
        
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;
        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wLength = 0;
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->wValue.W = USB_FEATURE_LTM_ENABLE;
        setupPacket->wIndex.W = 0;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to send request to enable LTM on device %!STATUS!",
                       status);

            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        } 
    }    

}

VOID
HUBDTX_DisableU2UsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a CLEAR_FEATURE for U2 to the device using a control transfer.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued directly
    from this function. Completion routine of this function should queue a
    DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;
        
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;
        setupPacket->bRequest = USB_REQUEST_CLEAR_FEATURE;
        setupPacket->wLength = 0;
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->wValue.W = USB_FEATURE_U2_ENABLE;
        setupPacket->wIndex.W = 0;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to send request to disable U2 on device %!STATUS!",
                       status);

            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        } 
    }    

}

VOID
HUBDTX_EnableU2UsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a SET_FEATURE for U2 to the device using a control transfer.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued directly
    from this function. Completion routine of this function should queue a
    DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;

    TRY {
        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;
        
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;
        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wLength = 0;
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->wValue.W = USB_FEATURE_U2_ENABLE;
        setupPacket->wIndex.W = 0;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to send request to enable U2 on device %!STATUS!",
                       status);

            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        } 
    }    

}

VOID
HUBDTX_SetSelUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Computes various LPM latencies for the device and sends 
    a SET_SEL to the device using a control transfer.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued directly
    from this function. Completion routine of this function should queue a
    DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    PHUB_INFO_FROM_PARENT           hubInfoFromParent;
    PUSB_SEL_INFO                   selInfo;
    USHORT                          linkLatency;
    USHORT                          pathExitLatencyForSlowestLink;
    USHORT                          timeForErdyAndResponse;
    UCHAR                           hubDepth;

    TRY {
        
        hubInfoFromParent = &DeviceContext->HubFdoContext->HubInfoFromParent;
        hubDepth = DeviceContext->HubFdoContext->HubParentInterface.HubDepth;

        selInfo = &DeviceContext->UsbSelInfo;

        //
        // Calculate U1/U2 PEL - time to transition all links in the path to U0 when the 
        // transition is initiated by the device
        //
        // Calculation of PELs is based on the example given in USB30 Spec, C.2.2.2
        //
        
        //
        // First calculate the latency for the link between this device and the
        // parent
        //
        linkLatency = max(DeviceContext->U1ExitLatency,
                          hubInfoFromParent->U1ExitLatency);

        //
        // We get the information from the parent about the link that has been
        // considered a bottleneck till now. We need to figure out if the current
        // link is the new bottleneck. For that, we first calculate the latency
        // for the slowest link based on the depth of the current deivce (i.e. all
        // the hubs in between will cause additional delay for the current slowest
        // link
        //
        NT_ASSERT(hubDepth >= hubInfoFromParent->DepthOfSlowestLinkForU1);
        
        pathExitLatencyForSlowestLink = hubInfoFromParent->ExitLatencyOfSlowestLinkForU1
            + ((hubDepth - hubInfoFromParent->DepthOfSlowestLinkForU1)
            * HUB_PORT_2_PORT_EXIT_LATENCY_IN_NANOSECONDS + 500)/1000;         
        //
        // Now we compare the total exit latency for the slowest link with the
        // the latency for the current link 
        //
        if (linkLatency > pathExitLatencyForSlowestLink) {
            
            selInfo->U1PEL = (UCHAR)linkLatency;
            INTERLOCKED_SET_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, SlowestLinkForU1Exit);

        } else {
            
            selInfo->U1PEL = (UCHAR)pathExitLatencyForSlowestLink;
        }
        
        //
        // Host Initiaited exit latency is not needed for the Sel transfer
        // but it is a good time to calculate and store this latency. 
        // Latencies are in micro seconds except bHubHdrDecLat which is in
        // one-tenths of microseconds
        //
        DeviceContext->HostInitiatedU1ExitLatency = hubInfoFromParent->HostInitiatedU1ExitLatency
            + linkLatency + (DeviceContext->HubFdoContext->HubDescriptor.Hub30Descriptor.bHubHdrDecLat + 5)/10;

        //
        // Now repeat calculating PEL for U2
        //
        linkLatency = max(DeviceContext->U2ExitLatency,
                          hubInfoFromParent->U2ExitLatency);

        NT_ASSERT(hubDepth >= hubInfoFromParent->DepthOfSlowestLinkForU2); 

        pathExitLatencyForSlowestLink = hubInfoFromParent->ExitLatencyOfSlowestLinkForU2
            + ((hubDepth - hubInfoFromParent->DepthOfSlowestLinkForU2)
            * HUB_PORT_2_PORT_EXIT_LATENCY_IN_NANOSECONDS + 500)/1000;         
        
        if (linkLatency > pathExitLatencyForSlowestLink) {
            
            selInfo->U2PEL = linkLatency;
            INTERLOCKED_SET_BIT_FIELD(DEVICE_LPM_STATE_FLAGS, &DeviceContext->LPMStateFlags, SlowestLinkForU2Exit);

        } else {
            
            selInfo->U2PEL = pathExitLatencyForSlowestLink;
        }
        
        DeviceContext->HostInitiatedU2ExitLatency = hubInfoFromParent->HostInitiatedU2ExitLatency
            + linkLatency + (DeviceContext->HubFdoContext->HubDescriptor.Hub30Descriptor.bHubHdrDecLat + 5)/10;

        //
        // Calculate U1/U2SEL = t1 + t2 + t3 + t4
        // t1 = PEL
        // t2 = time for the ERDY to traverse the interconnect hierarchy 
        //      from the device to the host
        // t3 = time for the host to consume the ERDY and transmit a response 
        //      to that request
        // t4 = time for the response to traverse the interconnect hierarchy 
        //      from the host to the device
        //
        // t2 + t4 is calculated based on the example given in USB30 Spec, C.1.5.1
        //
        if (hubDepth == 0) {
            //
            // Time it takes for ERDY to move up and down the only link
            //
            timeForErdyAndResponse = DeviceContext->RxTpDelay + DeviceContext->TxTpDelay;
        } else {
            timeForErdyAndResponse = (FORWARDING_DELAY_DUE_TO_ONE_MAXIMUM_PACKET_SIZE_IN_NANOSECONDS 
                + (hubDepth - 1) * T_HUB_DELAY_IN_NANOSECONDS
                + hubDepth * T_HUB_DELAY_IN_NANOSECONDS
                //
                // The example in the spec is missing the following time 
                // that it takes to move the bits for the ERDY up and down 
                // each link. 
                //
                + (hubDepth + 1) * (DeviceContext->RxTpDelay + DeviceContext->TxTpDelay)
                + 500)/1000; 
        }

        selInfo->U1SEL = (UCHAR)(selInfo->U1PEL + timeForErdyAndResponse + HOST_RESPONSE_TIME_IN_MICROSECONDS);

        selInfo->U2SEL = selInfo->U2PEL + timeForErdyAndResponse + (USHORT)HOST_RESPONSE_TIME_IN_MICROSECONDS;

        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;
        
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;
        setupPacket->bRequest = USB_REQUEST_SET_SEL;
        setupPacket->wLength = sizeof(USB_SEL_INFO);
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->wValue.W = 0;
        setupPacket->wIndex.W = 0;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         selInfo,
                                         sizeof(USB_SEL_INFO),
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to send request to set SEL on device %!STATUS!",
                       status);

            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        } 
    }    

}


VOID
HUBDTX_SetDevicePDChargingPolicyUsingControlTransfer (
    __in
    PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a SET_FEATURE for specifying charging policy for a
    device using a control transfer.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued directly
    from this function. Completion routine of this function should queue a
    DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;


    TRY {

        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)DeviceContext->ControlRequest.Urb.SetupPacket;

        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;
        setupPacket->bRequest = USB_REQUEST_SET_FEATURE;
        setupPacket->wLength = 0;
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->wValue.W = USB_FEATURE_CHARGING_POLICY;

        if (DeviceContext->HubFdoContext->HubInformation.MaxPortPower == MaxPortPower100Milliamps) {
            setupPacket->wIndex.W = USB_CHARGING_POLICY_ICCLPF;
        } else {
            setupPacket->wIndex.W = USB_CHARGING_POLICY_ICCHPF;
        }

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID)DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to send request to set charging policy on device %!STATUS!",
                       status);

            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        }
    }

}


VOID
HUBDTX_SetIsochDelayUsingControlTransfer (
    __in
        PDEVICE_CONTEXT        DeviceContext
    )
/*++

Routine Description:

    Sends a SET_FEATURE for setting the ISOCH delay to the 
    device using a control transfer.

    If WdfRequestSend fails then DsmEventTransferFailure should be queued directly
    from this function. Completion routine of this function should queue a
    DsmEventTransferSuccess or DsmEventTransferFailure event to DSM.

Returns:

    VOID

--*/
{
    NTSTATUS                        status;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    USHORT                          isochDelay;


    TRY {

        //
        // The request informs the device of the delay from the time a host transmits a packet
        // to the time it is received by the device.
        //
        isochDelay = DeviceContext->HubFdoContext->HubInfoFromParent.TotalTPPropogationDelay 
                     + DeviceContext->HubFdoContext->HubDescriptor.Hub30Descriptor.wHubDelay 
                     + DeviceContext->TxTpDelay;

        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) DeviceContext->ControlRequest.Urb.SetupPacket;
        
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;
        setupPacket->bRequest = USB_REQUEST_ISOCH_DELAY;
        setupPacket->wLength = 0;
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->wValue.W = isochDelay;
        setupPacket->wIndex.W = 0;

        status = HUBMISC_ControlTransfer(DeviceContext->HubFdoContext,
                                         DeviceContext->UsbDevice,
                                         (PVOID) DeviceContext,
                                         &DeviceContext->ControlRequest,
                                         HUBDTX_ControlTransferComplete,
                                         NULL,
                                         0,
                                         TRUE,
                                         DeviceContext->DsmContext.NeedsForwardProgress);

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "Unable to send request to set isoch delay on device %!STATUS!",
                       status);

            HUBSM_AddDsmEvent(DeviceContext, DsmEventTransferFailure);
        } 
    }    

} 


VOID
HUBDTX_GetDeviceDescriptorComplete(
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
    
    Completion routine for the Control Transfer that 
    was sent by DSM on behalf of the FDO for a request
    from the user mode to fetch a descriptor from the 
    device. It signals the event that the IOCTL dispatch
    routine is waiting on and also queues TransferSuccess
    or TransferFailure to DSM

Returns:

    VOID

--*/
{
    NTSTATUS                            status;
    PDEVICE_CONTEXT                     deviceContext;
    PUSER_MODE_IOCTL_QUEUE_CONTEXT      queueContext;


    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(WdfRequest);

    TRY {

        status = CompletionParams->IoStatus.Status; 
        
        deviceContext = (PDEVICE_CONTEXT) Context;  
        
        queueContext = GetUserModeIoctlContext(WdfRequestGetIoQueue(WdfRequest));

        //
        // Notify DSM of completion status of the control transfer
        //
        HUBSM_AddDsmEvent(deviceContext,
                          NT_SUCCESS(status) ? DsmEventTransferSuccess : DsmEventTransferFailure);

        HUBFDO_CompleteGetDescriptorRequest(deviceContext->HubFdoContext,
                                            deviceContext->PortContext->PortInformation.PortNumber,
                                            WdfRequest,
                                            WdfRequestGetStatus(WdfRequest),
                                            queueContext->ControlRequest.Urb.TransferBufferLength);
        
        //
        // Release the WDF reference that was acquired on behalf
        // of the user mode request from the Ioctl handler
        //
        WdfObjectDereferenceWithTag(WdfObjectContextGetObject(deviceContext),
                                    (PVOID) UserModeFdoRequest);

    } FINALLY {

    }
}

VOID
HUBDTX_GetDescriptorForFdoRequestUsingPreformattedUrb(
    PDEVICE_CONTEXT             DeviceContext
    )
/*++

Routine Description:

    Asynchronously sends the transfer that was prepared by
    the FDO IOCTL dispatch handler in response to a request
    received from user mode for fetching a device descriptor.

Arguments:


Returns:

--*/
{
    BOOLEAN     requestSent;
    
    TRY {
        
        WdfRequestSetCompletionRoutine(DeviceContext->PreformattedGetDescriptorRequest,
                                       HUBDTX_GetDeviceDescriptorComplete,
                                       DeviceContext);
        
        requestSent = WdfRequestSend(DeviceContext->PreformattedGetDescriptorRequest,
                                     DeviceContext->HubFdoContext->RootHubIoTarget,
                                     NULL);

        if (requestSent == FALSE) {

            DbgTraceTo(DeviceContext->PortContext->IfrLog, TL_ERROR, 
                       Device, 
                       "HUBDTX_GetDescriptorForFdoRequestUsingPreformattedUrb Request Not Sent!");
            
            HUBSM_AddDsmEvent(DeviceContext,
                              DsmEventTransferFailure);

            HUBFDO_CompleteGetDescriptorRequest(DeviceContext->HubFdoContext,
                                                DeviceContext->PortContext->PortInformation.PortNumber,
                                                DeviceContext->PreformattedGetDescriptorRequest,
                                                STATUS_UNSUCCESSFUL,
                                                0);
            
            //
            // Release the WDF reference that was acquired on behalf
            // of the user mode request from the Ioctl handler
            //
            WdfObjectDereferenceWithTag(WdfObjectContextGetObject(DeviceContext),
                                        (PVOID) UserModeFdoRequest);

        }

    } FINALLY {
    }

}



