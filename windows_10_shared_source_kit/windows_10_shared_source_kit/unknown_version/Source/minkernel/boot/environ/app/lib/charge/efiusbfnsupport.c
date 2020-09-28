//
// Copyright (C) Microsoft. All rights reserved.
//

#include "usbfnsupport.h"



UINT64                      gIoReadWriteTimeoutIn100ns;     // Read/Write timeout value in 100ns
BOOLEAN                     cConnectionEstablished;         // Indicates connection to host

EFI_USBFN_MESSAGE_PAYLOAD   *cUsbFnMsgPayload;
UINT8                       *cRxBuffer;
static USB_STRING_RESPONSE  gUsbStringResponse;
UINT8                       *gEp0MsgBuffer;

EPTransfer                  cEpTransfers[MAX_ENDPT+1];
static UINT32               gEpMaxPacketSize[MAX_ENDPT+1];

static BOOLEAN              gSetupCanceled = FALSE;
EFI_USB_BUS_SPEED           cBusSpeed = UsbBusSpeedUnknown;




//
// USB device configuration descriptors.
//
extern USB_DEVICE_QUALIFIER_DESCRIPTOR  gUsbDeviceQualifierDescriptor;
extern EFI_USB_DEVICE_INFO          EfiUsbDeviceInfo;
extern EFI_USB_DEVICE_INFO gUsbChargerDeviceInfo;    


extern EFI_USB_ENDPOINT_DESCRIPTOR EfiUsbEndpointHidInterrupt;
extern EFI_USB_HID_DESCRIPTOR EfiUsbHidDescriptor;
extern EFI_USB_CONFIG_DESCRIPTOR EfiUsbConfigDescriptor;
extern EFI_USB_INTERFACE_DESCRIPTOR EfiUsbInterfaceDescriptor;
extern EFI_USB_DEVICE_DESCRIPTOR EfiUsbDeviceDescriptor;

extern UCHAR EfiUsbHidReportDescriptor[];
EFI_USB_HID_CONFIG_INFO EfiUsbHidConfigInfo;

INT16 EfiUsbHidConfigInfoLength;


void
ChargerIssueTransfer(
    UINT8                           EndpointIndex,
    EFI_USBFN_ENDPOINT_DIRECTION    Direction,
    UINT8                           *pData,
    UINT32                          cbData
    )
/*++

Description:

    Issues a transfer request to the function driver and returns immediately.

Arguments:

    EndpointIndex - Indicates the endpoint on which TX or RX transfer needs to take place.
    Direction     - Direction of the endpoint.
    pData         - Pointer to the data buffer.
    cbData        - The size of the buffer pointed by pData in bytes.

Return Value:

    EFI_SUCCESS           The function returned successfully.
    EFI_INVALID_PARAMETER A parameter was incorrect.
    EFI_DEVICE_ERROR      The physical device reported an error.
    EFI_NOT_READY         The physical device is busy or not ready to process this request.

--*/
{
    //
    // Get transfer struct on EndpointIndex
    //
    EPTransfer* pTransfer = &cEpTransfers[EndpointIndex];
    EFI_STATUS Status = EFI_SUCCESS;
    UINT32 cbTransferSize = 0;

    

    //
    // If ep0 and transfer is cancelled return
    //
    if ((0 == EndpointIndex) && gSetupCanceled)
    {
        
        return;
    }

    //
    // Rest transfer details on this endpoint
    //
    pTransfer->pbBuffer     = pData;
    pTransfer->cbRequested  = cbTransferSize = cbData;
    pTransfer->status       = TransferStatusOngoing;
    pTransfer->cbCompleted  = 0;

    Status = EfiUsbFnTransfer(EfiUsbProtocol, EndpointIndex,
                                    Direction, (UINTN *)&cbTransferSize,
                                    (void *)pTransfer->pbBuffer);

    if (EFI_ERROR(Status))
    {
        pTransfer->status = TransferStatusAborted;
    }

    
    return;
}



static
void
OnTransferStatusChange(
    EFI_USBFN_TRANSFER_RESULT *pUtr
    )
/*++

Description:

    Handles transfer status changes as received from the function
    driver.

Arguments:

    pUtr -  Pointer a EFI_USBFN_TRANSFER_RESULT structure containing transfer
            status information

Return Value:

    void

--*/
{
    EPTransfer* pTransfer = &cEpTransfers[pUtr->EndpointIndex];

    
    /*
    EFIMSG_VERBOSE(L"[EFI_USBFN_TRANSFER_RESULT] BytesTransferred=%d, TransferStatus=0x%x, EndpointIndex=%d, Direction=0x%x, Buffer=0x%x [%d %d %d %d] \r\n",
        pUtr->BytesTransferred, pUtr->TransferStatus, pUtr->EndpointIndex, pUtr->Direction, pUtr->Buffer,
        *((unsigned char*)pUtr->Buffer), *((unsigned char*)pUtr->Buffer + 1),
        *((unsigned char*)pUtr->Buffer + 2), *((unsigned char*)pUtr->Buffer + 3));
    */
    pTransfer->cbCompleted = (UINT32) pUtr->BytesTransferred;

    switch (pUtr->TransferStatus)
    {
    case UsbTransferStatusComplete:
        //
        // Hanging read request always remains issued on OUT (RX) endpoint,
        // effectively making the size test irrelevant for RX.
        //
        if (BULKOUT_ENDPT == pUtr->EndpointIndex)
        {
            pTransfer->status = TransferStatusComplete;
        }
        else
        {
            if (pTransfer->cbCompleted >= pTransfer->cbRequested)
            {
                pTransfer->status = TransferStatusComplete;
            }
        }

        if (pTransfer->pbBuffer != pUtr->Buffer)
        {
            EFIMSG_WARN(L"charge: Mismatched transfer buffer address on "
                L"endpoint 0x%x\r\n", pUtr->EndpointIndex);
        }
        break;

    case UsbTransferStatusAborted:
        pTransfer->status = TransferStatusAborted;
        break;

    case UsbTransferStatusActive:
        //
        // Transfer in progress, no need to further process the timeout
        //
        pTransfer->status = TransferStatusOngoing;
        break;

    case UsbTransferStatusUnknown:
        EFIMSG_ERROR(L"charge: ERROR: Unknown transfer status (0x%x) received "
            L"on endpoint#%d.\r\n", pUtr->TransferStatus, pUtr->EndpointIndex);
        break;

    default:
        EFIMSG_ERROR(L"charge: ERROR: Unsupported transfer status (0x%x) "
            L"received on endpoint#%d.\r\n", pUtr->TransferStatus,
            pUtr->EndpointIndex);
        break;
    }

    
    return;
}



static
EFI_STATUS
AbortTransfers(
    BOOLEAN AbortEp0,
    BOOLEAN AbortEpx
    )
/*++

Description:

    Aborts transfer on all endpoints.

Arguments:

    AbortEp0 -  Aborts transfers on CONTROL_ENDPT
    AbortEpx -  Aborts transfers on BULKIN_ENDPT and BULKOUT_ENDPT

Return Value:

    EFI_SUCCESS           The function returned successfully.
    EFI_INVALID_PARAMETER A parameter was incorrect.
    EFI_DEVICE_ERROR      The physical device reported an error.
    EFI_NOT_READY         The physical device is busy or not ready to process this request.

--*/
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    EFI_STATUS Status1 = EFI_UNSUPPORTED;

    

    if (AbortEp0)
    {
        //EFIMSG_VERBOSE(L"AbortTransfer(CONTROL_ENDPT, EfiUsbEndpointDirectionHostIn)...\r\n");
        Status = EfiUsbFnAbortTransfer(EfiUsbProtocol, CONTROL_ENDPT, EfiUsbEndpointDirectionHostIn);
        if (EFI_ERROR(Status))
        {
            EFIMSG_WARN(L"charge: AbortTransfer(CONTROL_ENDPT, "
                L"EfiUsbEndpointDirectionHostIn) returned failure (0x%x)\r\n",
                Status);
        }

        //EFIMSG_VERBOSE(L"AbortTransfer(CONTROL_ENDPT, EfiUsbEndpointDirectionHostOut)...\r\n");
        Status1 = EfiUsbFnAbortTransfer(EfiUsbProtocol, CONTROL_ENDPT, EfiUsbEndpointDirectionHostOut);
        if (EFI_ERROR(Status1))
        {
            EFIMSG_WARN(L"charge: AbortTransfer(CONTROL_ENDPT, "
                L"EfiUsbEndpointDirectionHostOut) returned failure (0x%x)\r\n",
                Status1);
        }

        if (EFI_ERROR(Status) && EFI_ERROR(Status1))
        {
            EFIMSG_ERROR(L"charge: AbortTransfer(CONTROL_ENDPT) encountered "
                L"irrecoverable failure\r\n");
            goto Error;
        }
    }

    if (AbortEpx)
    {
        //EFIMSG_VERBOSE(L"AbortTransfer(BULKIN_ENDPT, EfiUsbEndpointDirectionHostIn)...\r\n");
        Status = EfiUsbFnAbortTransfer(EfiUsbProtocol, BULKIN_ENDPT, EfiUsbEndpointDirectionHostIn);
        if (EFI_ERROR(Status))
        {
            EFIMSG_WARN(L"charge: AbortTransfer(BULKIN_ENDPT, "
                L"EfiUsbEndpointDirectionHostIn) returned failure (0x%x)\r\n",
                Status);
            goto Error;
        }

        //EFIMSG_VERBOSE(L"AbortTransfer(BULKOUT_ENDPT, EfiUsbEndpointDirectionHostOut)...\r\n");
        Status = EfiUsbFnAbortTransfer(EfiUsbProtocol, BULKOUT_ENDPT, EfiUsbEndpointDirectionHostOut);
        if (EFI_ERROR(Status))
        {
            EFIMSG_WARN(L"charge: AbortTransfer(BULKOUT_ENDPT, "
                L"EfiUsbEndpointDirectionHostOut) returned failure (0x%x)\r\n",
                Status);
            goto Error;
        }
    }

    Status = EFI_SUCCESS;

Error:
    
    return Status;
}


static
EFI_STATUS
SendEpAnyRecvEp0Data(
    UINT8                           EndpointIndex,
    EFI_USBFN_ENDPOINT_DIRECTION    Direction,
    UINT8*                          pData,
    UINT32                          cbData
    )
/*++

Description:

    Blocking call that starts a TX or RX transaction.

Arguments:

    EndpointIndex - Indicates the endpoint on which TX or RX transfer needs to take place.
    Direction     - Direction of the endpoint.
    pData         - Pointer to the data buffer.
    cbData        - The size of the buffer pointed by pData in bytes.

Return Value:

    EFI_SUCCESS           The function returned successfully.
    EFI_INVALID_PARAMETER A parameter was incorrect.
    EFI_DEVICE_ERROR      The physical device reported an error.
    EFI_NOT_READY         The physical device is busy or not ready to process
                          this request.
    EFI_TIMEOUT           The data read or write was interrupted due to a
                          timeout.

--*/
{
    EPTransfer* pTransfer = &cEpTransfers[EndpointIndex];
    EFI_EVENT   ReadWriteTimeoutEvent = 0;
    EFI_STATUS  Status;

    
    gIoReadWriteTimeoutIn100ns = USB_ENUMERATION_TIMEOUT * 10;

    ChargerIssueTransfer(EndpointIndex, Direction, pData, cbData);

    EventHandler();

    Status = EfiCreateEvent(EVT_TIMER, TPL_NOTIFY, 0, 0, &ReadWriteTimeoutEvent);
    if (EFI_ERROR(Status))
    {
        goto Error;
    }

    EventHandler();

    Status = EfiSetTimer(ReadWriteTimeoutEvent, TimerRelative, gIoReadWriteTimeoutIn100ns);
    if (EFI_ERROR(Status))
    {
        goto Error;
    }

    EventHandler();

    while (TransferStatusOngoing == pTransfer->status)
    {
        EventHandler();

        Status = EfiCheckEvent(ReadWriteTimeoutEvent);

        if (EFI_SUCCESS == Status)
        {
            EFIMSG_ERROR(L"charge: ERROR: Ep#%d Dir=%d timed out tx/rx data.\r\n",
                EndpointIndex, Direction);
            EfiUsbFnAbortTransfer(EfiUsbProtocol, EndpointIndex, Direction);
            Status = EFI_TIMEOUT;
            goto Error;
        }
    }

    if (TransferStatusComplete == pTransfer->status)
    {
        Status = EFI_SUCCESS;
    }
    else
    {
        Status = EFI_NOT_READY;
    }

Error:
    if (BOOLIFY(ReadWriteTimeoutEvent))
    {
        EfiCloseEvent(ReadWriteTimeoutEvent);
    }
    
    return Status;
}



static
EFI_STATUS
SendControlStatusHandshake(
    void
    )
/*++

Description:

    Sends an empty packet to ACK the transfer as part of the control status
    handshake.

Arguments:

    void

Return Value:

    EFI_SUCCESS           The function returned successfully.
    EFI_INVALID_PARAMETER A parameter was incorrect.
    EFI_DEVICE_ERROR      The physical device reported an error.
    EFI_NOT_READY         The physical device is busy or not ready to process
                          this request.
    EFI_TIMEOUT           The data read or write was interrupted due to a
                          timeout.

--*/
{
    return SendEpAnyRecvEp0Data(CONTROL_ENDPT,
                EfiUsbEndpointDirectionDeviceTx,
                gEp0MsgBuffer,
                0);
}



static
EFI_STATUS
RecvControlStatusHandshake(
    void
    )
/*++

Description:

    Receive an empty packet as part of the control status handshake.

Arguments:

    void

Return Value:

    EFI_SUCCESS           The function returned successfully.
    EFI_INVALID_PARAMETER A parameter was incorrect.
    EFI_DEVICE_ERROR      The physical device reported an error.
    EFI_NOT_READY         The physical device is busy or not ready to process
                          this request.
    EFI_TIMEOUT           The data read or write was interrupted due to a
                          timeout.

--*/
{
    return SendEpAnyRecvEp0Data(CONTROL_ENDPT,
                EfiUsbEndpointDirectionDeviceRx,
                gEp0MsgBuffer,
                0);
}



static
BOOLEAN
SendDescriptor(
    USB_DEVICE_REQUEST* pUdr
    )
/*++

Description:

    Responds to a GET_DESCRIPTOR setup request from the host.

Arguments:

    pUdr - Pointer to the device setup request

Return Value:

    If a descriptor was transmitted to the host, the success or failure code as
    returned from SendEpAnyRecvEp0Data() function, TRUE in all other cases.

--*/
{
    UCHAR*      pucData = NULL;
    USHORT      wLength = 0;
    USHORT      wType = pUdr->wValue;
    BOOLEAN     fReturnDescriptor = TRUE;
    UINTN       DeviceInfoLen;
    EFI_STATUS  Status = EFI_SUCCESS;
    BOOLEAN     RetVal;

 
    
    switch (HIBYTE(wType)) // bReportType
    {
    
    //
    // HID descriptor
    //
    case HID_DESCRIPTOR_TYPE_REPORT:
        
        //
        // check the bReportId... it should be 0 for the report descriptor
        //
        if( 0 == LOBYTE(wType))
        {
            dprintf(L"charge: Sending the HID Report Descriptor\r\n");
            pucData = (UCHAR *)&EfiUsbHidReportDescriptor;
            wLength = 21;
            cConnectionEstablished = TRUE;
        }
        break;
    
    case USB_DEVICE_DESCRIPTOR_TYPE:
        pucData = (UCHAR *)&EfiUsbDeviceDescriptor;
        wLength = EfiUsbDeviceDescriptor.Length;
        break;

    case USB_CONFIGURATION_DESCRIPTOR_TYPE:
        pucData = (UCHAR *)&EfiUsbHidConfigInfo;
        wLength = min(EfiUsbHidConfigInfoLength, pUdr->wLength);
        break;
        
    case USB_STRING_DESCRIPTOR_TYPE:
        switch (LOBYTE(wType)) // bReportID
        {
            case 0x00:
                gUsbStringResponse.hdr.bLength = 0x04;
                gUsbStringResponse.hdr.bString[0] = 0x0409;
                break;

            case 0x01:
            {
                DeviceInfoLen = MAXIMUM_USB_STRING_LENGTH;
                Status = EfiUsbFnGetDeviceInfo(
                                                EfiUsbProtocol,
                                                EfiUsbDeviceInfoManufacturerName,
                                                &DeviceInfoLen,
                                                gUsbStringResponse.hdr.bString);
                if (EFI_SUCCESS == Status)
                {
                    gUsbStringResponse.hdr.bLength = (UCHAR)(DeviceInfoLen + 2);
                }
                else
                {
                    EFIMSG_TRACE(L"charge: Using default manufacturer string.\r\n");
                    EfiCopyMemory(
                        gUsbStringResponse.hdr.bString,
                        MANUFACTURER_DEFAULT,
                        sizeof(MANUFACTURER_DEFAULT));
                    gUsbStringResponse.hdr.bLength = sizeof(MANUFACTURER_DEFAULT) + 2;
                }
            }
            break;

            case 0x02:
            {
                DeviceInfoLen = MAXIMUM_USB_STRING_LENGTH;
                Status = EfiUsbFnGetDeviceInfo(
                                                EfiUsbProtocol,
                                                EfiUsbDeviceInfoProductName,
                                                &DeviceInfoLen,
                                                gUsbStringResponse.hdr.bString);
                if (EFI_SUCCESS == Status)
                {
                    gUsbStringResponse.hdr.bLength = (UCHAR)(DeviceInfoLen + 2);
                }
                else
                {
                    EFIMSG_TRACE(L"charge: Using default product string.\r\n");
                    EfiCopyMemory(
                        gUsbStringResponse.hdr.bString,
                        PRODUCT_DEFAULT,
                        sizeof(PRODUCT_DEFAULT));
                    gUsbStringResponse.hdr.bLength = sizeof(PRODUCT_DEFAULT) + 2;
                }
            }
            break;

            case 0x03:
            {
                DeviceInfoLen = MAXIMUM_USB_STRING_LENGTH;
                Status = EfiUsbFnGetDeviceInfo(
                                                EfiUsbProtocol,
                                                EfiUsbDeviceInfoSerialNumber,
                                                &DeviceInfoLen,
                                                gUsbStringResponse.hdr.bString);
                if (EFI_SUCCESS == Status)
                {
                    gUsbStringResponse.hdr.bLength = (UCHAR)(DeviceInfoLen + 2);
                }
                else
                {
                    EFIMSG_TRACE(L"charge: Using default serial number.\r\n");
                    EfiCopyMemory(
                        gUsbStringResponse.hdr.bString,
                        USBSERIALNUM_DEFAULT,
                        sizeof(USBSERIALNUM_DEFAULT));
                    gUsbStringResponse.hdr.bLength = sizeof(USBSERIALNUM_DEFAULT) + 2;
                }
            }
            break;

            case 0xEE:
                //
                // Windows is querying if device supports Microsoft-Defined USB descriptors. More information here:
                //
                // http://msdn.microsoft.com/en-us/library/windows/hardware/ff537430(v=vs.85).aspx
                //
                // From the website:
                //
                // ****************************************************************************************************
                // Why Does Windows Issue a String Descriptor Request to Index 0xEE?
                //
                // Devices that support Microsoft OS Descriptors must store a special USB string descriptor in firmware
                // at the fixed string index of 0xEE. This string descriptor is called a Microsoft OS String Descriptor.
                //
                // When a new device is attached to a computer for the first time, an operating system that supports 
                // Microsoft OS Descriptors will request the string descriptor that is at index 0xEE. The Microsoft OS 
                // String Descriptor contains an embedded signature field that the operating system uses to differentiate 
                // it from other strings that might be at index 0xEE. The presence of a string descriptor that contains 
                // the proper signature field at index 0xEE indicates to the operating system that the device supports 
                // Microsoft OS Descriptors. The Microsoft OS String Descriptor also provides the operating system with 
                // version information.
                //
                // The operating system queries for the string descriptor at index 0xEE during device enumeration--before 
                // the driver for the device has loaded--which might cause some devices to malfunction. Such devices are 
                // not supported by versions of the Windows operating system that support Microsoft OS Descriptors.
                //
                // If a device does not contain a valid string descriptor at index 0xEE, it must respond with a stall 
                // packet (in other words, a packet that contains a packet identifier of type STALL), which is described 
                // in the "Request Errors" section of the Universal Serial BusSpecification. If the device does not 
                // respond with a stall packet, the system will issue a single-ended zero reset packet to the device, to 
                // help it recover from its stalled state (Windows XP only).
                // ****************************************************************************************************
                //
                // Not stalling control endpoint does not result in a single-ended zero reset packet to the device.
                // Stalling endpoint generates the same results. Leaving this as a noop because Windows assumes that
                // Microsoft-Defined USB descriptors are not supported by default.
                //
                EfiUsbFnSetEndpointStallState(EfiUsbProtocol, CONTROL_ENDPT, EfiUsbEndpointDirectionDeviceTx, TRUE);
                EfiUsbFnSetEndpointStallState(EfiUsbProtocol, CONTROL_ENDPT, EfiUsbEndpointDirectionDeviceRx, TRUE);

                EFIMSG_VERBOSE(L"charge: String Descriptor Request to Index "
                    L"0xEE received\r\n");
                fReturnDescriptor = FALSE;
                break;

            default:
                EFIMSG_ERROR(L"charge: ERROR:*** Unknown STRING index %d\r\n",
                    (UCHAR)wType);
                fReturnDescriptor = FALSE;
                break;
        }

        if (TRUE == fReturnDescriptor)
        {
            gUsbStringResponse.hdr.bDescriptorType = USB_STRING_DESCRIPTOR_TYPE;
            pucData = (UCHAR *)&gUsbStringResponse;
            wLength = gUsbStringResponse.hdr.bLength;
        }
        // else stall?
        break;

    case USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE:
    {
        EFI_USB_DEVICE_DESCRIPTOR* pDevDesc = (EFI_USB_DEVICE_DESCRIPTOR*)&EfiUsbDeviceDescriptor;
        EFIMSG_TRACE(L"charge: Setup request "
            L"[USB_REQUEST_GET_DESCRIPTOR: USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE] Len:%d\r\n",
            pUdr->wLength);
        gUsbDeviceQualifierDescriptor.bLength = sizeof(USB_DEVICE_QUALIFIER_DESCRIPTOR);
        gUsbDeviceQualifierDescriptor.bDescriptorType = USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE;
        gUsbDeviceQualifierDescriptor.bcdUSB = pDevDesc->BcdUSB;
        gUsbDeviceQualifierDescriptor.bDeviceClass = pDevDesc->DeviceClass;
        gUsbDeviceQualifierDescriptor.bDeviceSubClass = pDevDesc->DeviceSubClass;
        gUsbDeviceQualifierDescriptor.bDeviceProtocol = pDevDesc->DeviceProtocol;
        gUsbDeviceQualifierDescriptor.bMaxPacketSize0 = pDevDesc->MaxPacketSize0;
        gUsbDeviceQualifierDescriptor.bNumConfigurations = pDevDesc->NumConfigurations;
        gUsbDeviceQualifierDescriptor.bReserved = 0;
        pucData = (UCHAR*)&gUsbDeviceQualifierDescriptor;
        wLength = gUsbDeviceQualifierDescriptor.bLength;
        break;
    }

    default:
        EFIMSG_ERROR(L"charge: ERROR:*** Unknown GET_DESCRIPTOR request:0x%x\r\n",
            (wType>>8)&0xFF);
        fReturnDescriptor = FALSE;
        break;
    }

    if (pucData == NULL)
    {
        fReturnDescriptor = FALSE;
    }

    //
    // Status stage should still happen
    //
    RetVal = fReturnDescriptor ?
        (SendEpAnyRecvEp0Data(CONTROL_ENDPT, EfiUsbEndpointDirectionDeviceTx, pucData, wLength) == EFI_SUCCESS) : TRUE;
    return RetVal;
}



static
EFI_USB_ENDPOINT_TYPE
GetEndpointType(
    UINT8    EndpointIndex
    )
/*++

Description:

    Returns the endpoint type, given the endpoint index.

Arguments:

    EndpointIndex - Indicates the endpoint

Return Value:

    If a descriptor was transmitted to the host, the success or failure code as
    returned from SendEpAnyRecvEp0Data() function, TRUE in all other cases.

--*/
{
    switch(EndpointIndex)
    {
    case BULKIN_ENDPT:
    case BULKOUT_ENDPT:
        dprintf(L"charge: This should not happend \r\n");
        return UsbEndpointBulk;

    case CONTROL_ENDPT:
        return UsbEndpointControl;

    default:
        NT_ASSERT(FALSE);
    }

    EFIMSG_ERROR(L"charge: ERROR: Nonexistent endpoint 0x%x specified, "
        L"assuming UsbEndpointBulk.\r\n", EndpointIndex);
    return UsbEndpointBulk;
}



static
EFI_USBFN_ENDPOINT_DIRECTION
GetEndpointDirection(
    UINT8    EndpointIndex
    )
/*++

Description:

    Returns the endpoint direction, given the endpoint index.

Arguments:

    EndpointIndex - Indicates the endpoint

Return Value:

    If a descriptor was transmitted to the host, the success or failure code as
    returned from SendEpAnyRecvEp0Data() function, TRUE in all other cases.

--*/
{
    switch(EndpointIndex)
    {
    case BULKIN_ENDPT:
        return EfiUsbEndpointDirectionDeviceTx;

    case BULKOUT_ENDPT:
        return EfiUsbEndpointDirectionDeviceRx;

    case CONTROL_ENDPT:
        EFIMSG_ERROR(L"charge: ERROR: Control endpoint 0x%x is bidirectional.\r\n",
            EndpointIndex);
        NT_ASSERT(EndpointIndex != CONTROL_ENDPT);
        break;

    default:
        NT_ASSERT(FALSE);
    }

    EFIMSG_ERROR(L"charge: ERROR: Nonexistent endpoint 0x%x specified, "
        L"assuming EfiUsbEndpointDirectionDeviceRx.\r\n", EndpointIndex);
    return EfiUsbEndpointDirectionDeviceRx;
}



static
BOOLEAN
ProcessGetStatus(
    USB_DEVICE_REQUEST* pUdr
    )
/*++

Description:

    Responds to a USB_REQUEST_GET_STATUS setup request from the host.

Arguments:

    pUdr - Pointer to the device setup request

Return Value:

    TRUE if successfully tramsitted the requested descriptor to the host,
    FALSE otherwise.

--*/
{
    UCHAR       pucData[2] = {0};
    EFI_STATUS  Status = EFI_SUCCESS;
    BOOLEAN     RetVal;

    

    if(pUdr->bmRequestType == USB_ENDPOINT_DIR_IN)
    {
        //
        // Device selfpowered
        //
        if((EfiUsbHidConfigInfo.ConfigDescriptor.Attributes & USB_CONFIG_SELF_POWERED) != 0)
        {
            pucData[0] |= USB_GETSTATUS_SELF_POWERED;
        }

        //
        // Device configured as remote wakeup resource
        //
        if((EfiUsbHidConfigInfo.ConfigDescriptor.Attributes & USB_CONFIG_REMOTE_WAKEUP) != 0)
        {
            pucData[0] |= USB_GETSTATUS_REMOTE_WAKEUP_ENABLED;
        }
    }

    if (pUdr->bmRequestType == (USB_ENDPOINT_DIR_IN | USB_REQ_TYPE_STANDARD | USB_TARGET_ENDPOINT))
    {
        INT8 epNum = pUdr->wIndex & 0xF;     // Get the low four bits which are the endpoint number

        if (BULKOUT_ENDPT == epNum || BULKIN_ENDPT == epNum)
        {
            EFI_USBFN_ENDPOINT_DIRECTION epDir = GetEndpointDirection(epNum);
            BOOLEAN                      fEndpointHalted = FALSE;

            if (EFI_SUCCESS == (Status = EfiUsbFnGetEndpointStallState(EfiUsbProtocol, epNum, epDir, &fEndpointHalted)))
            {
                if (fEndpointHalted)
                {
                    //EFIMSG_TRACE(L"USB_REQUEST_GET_STATUS for endpoint %x is returning HALTED%x\r\n", epNum);
                    //
                    // Add HALT bit
                    //
                    pucData[0] |= 1;
                }
            }
            else
            {
                //EFIMSG_ERROR(L"ERROR: GetEndpointStallState() returned failure (0x%x) for endpoint %x", Status, epNum);
                return FALSE;
            }
        }
        else
        {
           // EFIMSG_ERROR(L"ERROR: Receivd invalid endpoint %x\r\n", epNum);
            return FALSE;
        }
    }

    RetVal = (SendEpAnyRecvEp0Data(CONTROL_ENDPT, EfiUsbEndpointDirectionDeviceTx, pucData, sizeof(pucData)) == EFI_SUCCESS);
    
    return RetVal;
}



static
BOOLEAN
ProcessSetFeature(
    USB_DEVICE_REQUEST* pUdr
    )
/*++

Description:

    Responds to a USB_REQUEST_SET_FEATURE setup request from the host.

Arguments:

    pUdr - Pointer to the device setup request

Return Value:

    TRUE in case of success, FALSE otherwise.

--*/
{
    

    if((pUdr->bmRequestType ==
    (USB_ENDPOINT_DIR_IN | USB_REQ_TYPE_STANDARD | USB_TARGET_ENDPOINT )) &&
    (pUdr->wValue == USB_FEATURE_ENDPOINT_STALL))
    {
        INT8 epNum = pUdr->wIndex & 0xF;    // Get the low four bits which are the endpoint number

        EFIMSG_TRACE(L"charge: USB_REQUEST_SET_FEATURE ENDPOINT_STALL "
            L"RequestType==0x%x wValue==0x%x bEndpoint=0x%x\r\n",
            pUdr->bmRequestType, pUdr->wValue, epNum);

        if( epNum == BULKOUT_ENDPT || epNum == BULKIN_ENDPT )
        {
            //
            // Stall all bulk endpoints
            //
            EFI_USBFN_ENDPOINT_DIRECTION epDir = GetEndpointDirection(epNum);
            EfiUsbFnSetEndpointStallState(EfiUsbProtocol, epNum, epDir, TRUE);
        }
    }
    else
    {
        EFIMSG_WARN(L"charge: Unhandled USB_REQUEST_SET_FEATURE  "
            L"RequestType==0x%x wValue==0x%x\r\n", pUdr->bmRequestType,
            pUdr->wValue);
    }

    
    return TRUE;

}



static
BOOLEAN
ProcessClearFeature(
    USB_DEVICE_REQUEST* pUdr
    )
/*++

Description:

    Responds to a USB_REQUEST_CLEAR_FEATURE setup request from the host.

Arguments:

    pUdr - Pointer to the device setup request

Return Value:

    TRUE in case of success, FALSE otherwise.

--*/
{
    

    if ((pUdr->bmRequestType == (USB_REQ_TYPE_STANDARD | USB_TARGET_ENDPOINT )) && (pUdr->wValue == USB_FEATURE_ENDPOINT_STALL))
    {
        INT8 epNum = pUdr->wIndex & 0xF;    // Get the low four bits which are the endpoint number

        if (epNum == BULKOUT_ENDPT || epNum == BULKIN_ENDPT)
        {
            dprintf(L"charge: This should also never happen \r\n");
        }
        else
        {
            EFIMSG_WARN(L"charge: Stall state of endpoint 0x%x cannot be "
                L"altered.\r\n");
        }
    }
    else
    {
        EFIMSG_WARN(L"charge: Unhandled USB_REQUEST_CLEAR_FEATURE  "
            L"RequestType==0x%x wValue==0x%x\r\n", pUdr->bmRequestType,
            pUdr->wValue);
    }

    
    return TRUE;
}



static
void
ProcessSetupPacket(
    USB_DEVICE_REQUEST* pUdr
    )
/*++

Description:

    Handles various setup packets received from the host.

Arguments:

    pUdr - Pointer to the device setup request

Return Value:

    void

--*/
{
    BOOLEAN fSendRecvStatusACK = TRUE;
    INT8 epNum;
   

    //
    // class/vendor request
    //
    if (pUdr->bmRequestType & (USB_REQ_TYPE_CLASS | USB_REQ_TYPE_VENDOR))
    {
        switch (pUdr->bRequest)
        {

        case 0xA: // stall the HID
            epNum = pUdr->wIndex & 0xF;  
            EfiUsbFnSetEndpointStallState(EfiUsbProtocol, CONTROL_ENDPT, EfiUsbEndpointDirectionDeviceTx, TRUE);
            EfiUsbFnSetEndpointStallState(EfiUsbProtocol, CONTROL_ENDPT, EfiUsbEndpointDirectionDeviceRx, TRUE);
            break;
        case 0x21:
            epNum = pUdr->wIndex & 0xF;  
            EfiUsbFnSetEndpointStallState(EfiUsbProtocol, CONTROL_ENDPT, EfiUsbEndpointDirectionDeviceTx, TRUE);
            EfiUsbFnSetEndpointStallState(EfiUsbProtocol, CONTROL_ENDPT, EfiUsbEndpointDirectionDeviceRx, TRUE);        
            break;
        default:
            EFIMSG_ERROR(L"charge: ERROR: Unrecognized Setup request. "
                L"bmRequestType=0x%x. bRequest=0x%x\r\n", pUdr->bmRequestType,
                pUdr->bRequest);
            break;
        }
    }
    
    //
    // a standard request 
    //
    else
    {
        switch(pUdr->bRequest)
        {
        case SET_CONTROL_LINE_STATE:
            EFIMSG_TRACE(L"charge: SetupR request [SET_CONTROL_LINE_STATE] %d\r\n",
                pUdr->wValue);
            cConnectionEstablished = BOOLIFY(pUdr->wValue);
            break;
        case USB_REQUEST_SET_CONFIGURATION:
            //
            // SET_ADDRESS is handled by UsbFnIo's event handler immediately
            //
            //epNum = pUdr->wIndex & 0xF;  
            cConnectionEstablished = TRUE;
            EFIMSG_TRACE(L"charge: USB_REQUEST_SET_CONFIGURATION\r\n");
            break;            
        case 0xA: // stall the HID
            EFIMSG_TRACE(L"\r\n charge: Stalling 0xA \r\n");
            epNum = pUdr->wIndex & 0xF;  
            EfiUsbFnSetEndpointStallState(EfiUsbProtocol, epNum, EfiUsbEndpointDirectionDeviceTx, TRUE);
            cConnectionEstablished = BOOLIFY(pUdr->wValue);
            break;
            
        case USB_REQUEST_GET_STATUS:
            //EFIMSG_TRACE(L"USB_REQUEST_GET_STATUS\r\n");
            if ((pUdr->bmRequestType == (USB_ENDPOINT_DIR_IN | USB_REQ_TYPE_STANDARD | USB_TARGET_DEVICE))    ||
                (pUdr->bmRequestType == (USB_ENDPOINT_DIR_IN | USB_REQ_TYPE_STANDARD | USB_TARGET_INTERFACE)) ||
                (pUdr->bmRequestType == (USB_ENDPOINT_DIR_IN | USB_REQ_TYPE_STANDARD | USB_TARGET_ENDPOINT)))
            {
                fSendRecvStatusACK = ProcessGetStatus(pUdr);
            }
            else
            {
                EFIMSG_WARN(L"charge: RequestType==0x%d, Unrecognzied or "
                    L"unsupported request\r\n", pUdr->bmRequestType);
            }
            break;

        case USB_REQUEST_CLEAR_FEATURE:
            EFIMSG_TRACE(L"charge: USB_REQUEST_CLEAR_FEATURE\r\n");
            fSendRecvStatusACK = ProcessClearFeature(pUdr);
            break;

        case USB_REQUEST_SET_FEATURE:
            EFIMSG_TRACE(L"charge: USB_REQUEST_SET_FEATURE\r\n");
            fSendRecvStatusACK = ProcessSetFeature(pUdr);
            break;

        case USB_REQUEST_SET_ADDRESS:
            //
            // SET_ADDRESS is handled by UsbFnIo's event handler immediately
            //
            EFIMSG_TRACE(L"charge: USB_REQUEST_SET_ADDRESS\r\n");
            break;

        // The HID descriptor will be sent from here
        case USB_REQUEST_GET_DESCRIPTOR:
            //EFIMSG_TRACE(L"USB_REQUEST_GET_DESCRIPTOR\r\n");
            fSendRecvStatusACK = SendDescriptor(pUdr);
            break;

        case USB_REQUEST_SET_DESCRIPTOR:
            //EFIMSG_TRACE(L"USB_REQUEST_SET_DESCRIPTOR\r\n");
            break;

        case USB_REQUEST_GET_CONFIGURATION:
            //EFIMSG_TRACE(L"USB_REQUEST_GET_CONFIGURATION\r\n");
            break;



        //case USB_REQUEST_GET_INTERFACE:
            //EFIMSG_TRACE(L"USB_REQUEST_GET_INTERFACE\r\n");
        //    break;

        case USB_REQUEST_SET_INTERFACE:
            EFIMSG_TRACE(L"charge: USB_REQUEST_SET_INTERFACE\r\n");
            break;

        case USB_REQUEST_SYNC_FRAME:
            //EFIMSG_TRACE(L"USB_REQUEST_SYNC_FRAME\r\n");
            break;

        default:
            EFIMSG_WARN(L"charge: *** Unknown request 0x%x ****\r\n",
                pUdr->bRequest);
            break;
        }
    }   // end of else for standard request

    if (fSendRecvStatusACK)
    {
        if (0 == (USB_ENDPOINT_DIR_IN & pUdr->bmRequestType))
        {
            //
            // Host -> Device request
            //
            SendControlStatusHandshake();
        }
        else
        {
            //
            // Device -> Host request
            //
            RecvControlStatusHandshake();
        }
    }

}

EFI_STATUS
ConfigureEnableEndpoints(
    EFI_USBFN_IO_PROTOCOL *EfiUsbProtocol
    )
{
    
    EfiUsbHidConfigInfo.ConfigDescriptor =  EfiUsbConfigDescriptor;
    EfiUsbHidConfigInfo.InterfaceDescriptor = EfiUsbInterfaceDescriptor;
    EfiUsbHidConfigInfo.HidDescriptor = EfiUsbHidDescriptor;
    EfiUsbHidConfigInfo.EndpointInterruptIn = EfiUsbEndpointHidInterrupt;
    
    
    EfiUsbHidConfigInfoLength = EfiUsbHidConfigInfo.ConfigDescriptor.Length
                      + EfiUsbHidConfigInfo.InterfaceDescriptor.Length
                      + EfiUsbHidConfigInfo.HidDescriptor.bLength;

    return EfiUsbFnConfigureEnableEndpoints(EfiUsbProtocol, &gUsbChargerDeviceInfo);

    
}




EFI_STATUS
GetChargerEndpointMaxPacketSize(
    void
    )
/*++

Description:

    Retreieve and store maximum packet sizes for all endpoints.

Arguments:

    void

Return Value:

    EFI_STATUS is successful, a standard EFI error code in case of failure.

--*/
{
    EFI_STATUS  Status = EFI_SUCCESS;
    INT8        EndpointIndex;
    UINT16      MaxPacketSize;

    

     for (EndpointIndex=0; EndpointIndex <= MAX_ENDPT; EndpointIndex++)
     {
         Status = EfiUsbFnGetEndpointMaxPacketSize(
                                        EfiUsbProtocol,
                                        GetEndpointType(EndpointIndex),
                                        cBusSpeed,
                                        &MaxPacketSize);
         if (EFI_ERROR(Status))
         {
             //EFIMSG_ERROR(L"ERROR: Failed to retrieve maxpacketsize for endpoint #%d\r\n", EndpointIndex);
             goto Error;
         }

         gEpMaxPacketSize[EndpointIndex] = MaxPacketSize;
     }

    EfiUsbDeviceInfo.DeviceDescriptor->MaxPacketSize0 =  (UINT8)gEpMaxPacketSize[CONTROL_ENDPT];
   

Error:
    
    return Status;
}



EFI_STATUS
EventHandler(
    void
    )
/*++
    
Description:

    This function must be called periodically to receive updates on USB bus
    states, status changes on endpoints and moreover to aid processing of the
    function driver.

Arguments:

    void

Return Value:

    EFI_SUCCESS           The function returned successfully.
    EFI_INVALID_PARAMETER A parameter was incorrect.
    EFI_DEVICE_ERROR      The physical device reported an error.
    EFI_NOT_READY         The physical device is busy or not ready to process this request.
    EFI_TIMEOUT           The data read was interrupted due to a timeout.

--*/
{
    static USB_DEVICE_REQUEST   pendingUdr;
    static BOOLEAN              fProcessingSetupRequest = FALSE;
    EFI_STATUS                  Status = EFI_SUCCESS;
    EFI_USBFN_MESSAGE           msg = EfiUsbMsgNone;
    UINTN                       MsgPayloadSize = sizeof(EFI_USBFN_MESSAGE_PAYLOAD);
    
    Status = EfiUsbFnEventHandler(EfiUsbProtocol, &msg, &MsgPayloadSize, cUsbFnMsgPayload);

    if (EFI_ERROR(Status))
    {
        goto Error;
    }
    
    switch (msg)
    {
        case EfiUsbMsgEndpointStatusChangedRx:
        case EfiUsbMsgEndpointStatusChangedTx:
        {
            //dprintf(L" status change \r\n");
            OnTransferStatusChange(&(cUsbFnMsgPayload->utr));
            break;
        }

        case EfiUsbMsgSetupPacket:
        {
            USB_DEVICE_REQUEST udr = *((USB_DEVICE_REQUEST*)cUsbFnMsgPayload);
            //
            // If a transfer is already in progress on EP0, mark this setup packet
            // pending and return to unwind stack.
            //
            if (fProcessingSetupRequest)
            {
                dprintf(L"charge: Still processing the setup \r\n");
                gSetupCanceled = TRUE;
                pendingUdr = udr;
    
                //
                // Stop any transfers on EP0
                //
                if (TransferStatusOngoing == cEpTransfers[0].status)
                {
                    EfiUsbFnAbortTransfer(EfiUsbProtocol, CONTROL_ENDPT, EfiUsbEndpointDirectionDeviceRx);
                    EfiUsbFnAbortTransfer(EfiUsbProtocol, CONTROL_ENDPT, EfiUsbEndpointDirectionDeviceTx);
                }
    
                goto Error;
            }
            else
            {
                gSetupCanceled = FALSE;
            }
    
            fProcessingSetupRequest = TRUE;
            ProcessSetupPacket(&udr);
            fProcessingSetupRequest = FALSE;
    
            //
            // While a setup packet was being processed another might have come in.
            // UDR's processing was cancelled and the pending udr was stored in
            // pendingUdr.
            //
            while (gSetupCanceled)
            {
                gSetupCanceled = FALSE;
                fProcessingSetupRequest = TRUE;
                ProcessSetupPacket(&pendingUdr);
                fProcessingSetupRequest = FALSE;
            }
            break;
        }
    
        case EfiUsbMsgBusEventSpeed:
        {
            //
            // Determine bus speed
            //
            cBusSpeed = cUsbFnMsgPayload->ubs;
    
            break;
        }
    
        case EfiUsbMsgBusEventReset:
        case EfiUsbMsgBusEventDetach:
        case EfiUsbMsgBusEventSuspend:
        {
            cConnectionEstablished = FALSE;
            dprintf(L"charge: >>>>>>>>>>>>>>>>>>>>>>>>>>> disconnect \r\n");
            Status = AbortTransfers(TRUE, FALSE);
            if (EFI_ERROR(Status))
            {
                goto Error;
            }
    
            //
            // Retrieve and store maximum packet sizes for all endpoints
            //
            Status = GetChargerEndpointMaxPacketSize();
            if (EFI_ERROR(Status))
            {
                goto Error;
            }
            break;
        }
    
        default:
            break;
        }
    
    Error:
        
        return Status;
}



EFI_STATUS
AllocBuffers(
    void
    )
/*++

Description:

    Allocates various bufferes necessary to call into a USBFnIO driver.

Arguments:

    void

Return Value:

    Standard EFI status code.

--*/
{
    EFI_STATUS  Status = EFI_SUCCESS;

    if (!BOOLIFY(EfiUsbProtocol))
    {
        EFIMSG_ERROR(L"charge: EFI_USBFN_IO_PROTOCOL interface pointer is "
            L"invalid.\r\n");
        Status = EFI_NOT_FOUND;
        goto Error;
    }

    
    cUsbFnMsgPayload = BlMmAllocateHeap(sizeof(EFI_USBFN_MESSAGE_PAYLOAD));
    if(cUsbFnMsgPayload == NULL)
    {
        EFIMSG_ERROR(L"charge: Failed to allocate memory for the UsbFnLayload");
        goto Error;
    }
    
    Status = EfiUsbFnAllocateTransferBuffer(EfiUsbProtocol, EP0_MAX_MSG_RECV_BUFFER, &gEp0MsgBuffer);
    if (EFI_ERROR(Status))
    {
        EFIMSG_ERROR(L"charge: Failed to allocate transfer buffer for EP0.\r\n");
        goto Error;
    }

    EFIMSG_VERBOSE(L"charge: Ep0MsgBuffer=0x%x\r\n", gEp0MsgBuffer);

    Status = EfiUsbFnAllocateTransferBuffer(EfiUsbProtocol, MAX_RXBUF_SIZE, &cRxBuffer);
    if (EFI_ERROR(Status))
    {
        EFIMSG_ERROR(L"charge: Failed to allocate memory for hanging RX "
            L"buffer.\r\n");
        goto Error;
    }

    EFIMSG_VERBOSE(L"charge: RxBuffer=0x%x\r\n", cRxBuffer);

Error:
    return Status;
}



EFI_STATUS
FreeBuffers(
    void
    )
/*++

Description:

    Frees bufferes allocated in AllocBuffers().

Arguments:

    void

Return Value:

    Standard EFI status code.

--*/
{
    EFI_STATUS  TempStatus = EFI_SUCCESS;
    EFI_STATUS  Status = EFI_SUCCESS;

    if (!BOOLIFY(EfiUsbProtocol))
    {
        EFIMSG_ERROR(L"charge: EFI_USBFN_IO_PROTOCOL interface pointer is "
            L"invalid.\r\n");
        Status = EFI_NOT_FOUND;
    }

    if (cUsbFnMsgPayload)
    {
        TempStatus = BlMmFreeHeap(cUsbFnMsgPayload);

        if (EFI_ERROR(TempStatus))
        {
            EFIMSG_ERROR(L"charge: Failed to return pool memory for message "
                L"payload.\r\n");
            Status = TempStatus;
        }
    }

    cUsbFnMsgPayload = 0;

    if (gEp0MsgBuffer && EfiUsbProtocol)
    {
        TempStatus = EfiUsbFnFreeTransferBuffer(EfiUsbProtocol, gEp0MsgBuffer);

        if (EFI_ERROR(TempStatus))
        {
            EFIMSG_ERROR(L"charge: Failed to deallocate transfer buffer for "
                L"EP0.\r\n");
            Status = TempStatus;
        }
    }

    gEp0MsgBuffer = 0;

    if (cRxBuffer && EfiUsbProtocol)
    {
        TempStatus = EfiUsbFnFreeTransferBuffer(EfiUsbProtocol, cRxBuffer);

        if (EFI_ERROR(TempStatus))
        {
            EFIMSG_ERROR(L"charge: Failed to deallocate memory for hanging RX "
                L"buffer.\r\n");
            Status = TempStatus;
        }
    }

    cRxBuffer = 0;

    return Status;
}
