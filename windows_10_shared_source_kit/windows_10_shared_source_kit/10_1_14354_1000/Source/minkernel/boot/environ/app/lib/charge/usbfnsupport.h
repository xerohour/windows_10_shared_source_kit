/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    usbfnsupport.h

Abstract:

    This header and usbfnsupport.c were ported from SimpleWinphoneIO

Environment:

    Boot
    

--*/

#pragma once

#include "chargelib.h"
#include <usb100.h>
#include <windef.h>
#include <UsbFnIo.h>
#include "efiwrappers.h"



#define EFI_USB_MAXIMUM_PACKET_SIZE  (16376) //16K-8

//
// UsbFnIo interface
//
extern EFI_USBFN_IO_PROTOCOL    *EfiUsbProtocol;

//
// Maximum transfer size
//
extern UINTN                    cMaxTransferSize;

//
// USB string descriptors
//
#define USBSERIALNUM_DEFAULT    L"FFFFFFFFFFFF"
#define MANUFACTURER_DEFAULT    L"Microsoft"
#define PRODUCT_DEFAULT         L"Microsoft Simple IO Device"


//
// Internal transfer status
//
typedef enum _TRANSFER_STATUS
{
    TransferStatusNone,
    TransferStatusOngoing,
    TransferStatusComplete,
    TransferStatusAborted
} TRANSFER_STATUS;



#define CONTROL_ENDPT               0               // Control endpoint
#define BULKOUT_ENDPT               1               // Logical bulkout endpoint
#define BULKIN_ENDPT                2               // Logical bulkin endpoint
#define MAX_ENDPT                   CONTROL_ENDPT


#define MAX_MSGBUF_SIZE             (64)
#define MAX_RXBUF_SIZE              (cMaxTransferSize)

#define EFIMSG_ERROR    dprintf
#define EFIMSG_VERBOSE  dprintf
#define EFIMSG_WARN     dprintf
#define EFIMSG_TRACE    dprintf

// Length from starting of USB_CONFIGURATION_DESCRIPTOR_TYPE to end (50)
#define CONFIGDESC_LEN              42              
// Device (class) specific request
#define SET_CONTROL_LINE_STATE      0x22            
// No message ever recvd on EP0
#define EP0_MAX_MSG_RECV_BUFFER     4               



typedef volatile struct
{
    UINT8*           pbBuffer;
    UINT32           cbRequested;
    UINT32           cbCompleted;
    TRANSFER_STATUS  status;
} EPTransfer;




extern UINT64                       gIoReadWriteTimeoutIn100ns;

extern EPTransfer                   cEpTransfers[];
extern EFI_USBFN_MESSAGE_PAYLOAD    *cUsbFnMsgPayload;
extern UINT8                        *cRxBuffer;
extern EFI_USB_BUS_SPEED            cBusSpeed;

#define EfiCopyMemory(_Destination, _Source, _Length) \
            EfiBS->CopyMem(_Destination, _Source, _Length)



#include <pshpack1.h>
typedef struct _USB_DEVICE_REQUEST {
    UCHAR   bmRequestType;
    UCHAR   bRequest;
    USHORT  wValue;
    USHORT  wIndex;
    USHORT  wLength;
} USB_DEVICE_REQUEST, *PUSB_DEVICE_REQUEST;
typedef USB_DEVICE_REQUEST const * PCUSB_DEVICE_REQUEST;
#include <poppack.h>
//
#include <pshpack1.h>

//
// HID constants
//
#define HID_GET_REPORT          0x01
#define HID_GET_IDLE            0x02
#define HID_GET_PROTOCOL        0x03
#define HID_SET_REPORT          0x09
#define HID_SET_IDLE            0x0A
#define HID_SET_PROTOCOL        0x0B
#define HID_DESCRIPTOR_TYPE_REPORT 0x22
#define USB_HID_DESCRIPTOR_TYPE 0x21


typedef struct _EFI_USB_HID_DESCRIPTOR
{
    UCHAR   bLength;
    UCHAR   bDescriptorType;
    USHORT  bcdHID;
    UCHAR   bCountryCode;
    UCHAR   bNumDescriptors;
    UCHAR   bClassDescriptorType;
    USHORT  wDescriptorLength;
} EFI_USB_HID_DESCRIPTOR;

//
// This is to extend the USBFN spec to include HID
//
typedef struct _EFI_USB_HID_CONFIG_INFO
{   
    EFI_USB_CONFIG_DESCRIPTOR ConfigDescriptor;
    EFI_USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
    EFI_USB_HID_DESCRIPTOR HidDescriptor;
    EFI_USB_ENDPOINT_DESCRIPTOR EndpointInterruptIn;
   
} EFI_USB_HID_CONFIG_INFO;



#include <poppack.h>

#include <pshpack1.h>
typedef struct _USB_STRING_RESPONSE
{
    //
    // Reference: USB Spec 2.0 Section 9.6.7
    //
    // String descriptors use UNICODE encodings and might support multiple
    // languages. The UNICODE string descriptor is *not* NULL terminated. The
    // string length is computed by subtracting two from the value of the first
    // byte of the descriptor.
    //
    // Above indicates that maximum size of the string can only be 253 bytes or
    // 126 wide characters.
    //
    USB_STRING_DESCRIPTOR hdr;
    UINT8 reserved[(MAXIMUM_USB_STRING_LENGTH - sizeof(WCHAR) - 2) / sizeof(UCHAR)];
} USB_STRING_RESPONSE;
#include <poppack.h>

EFI_STATUS
EventHandler(
    void
    );

EFI_STATUS
GetChargerEndpointMaxPacketSize(
    void
    );

void
ChargerIssueTransfer(
    UINT8                           EndpointIndex,
    EFI_USBFN_ENDPOINT_DIRECTION    Direction,
    UINT8                           *pData,
    UINT32                          cbData
    );
    
EFI_STATUS
ConfigureEnableEndpoints(
    EFI_USBFN_IO_PROTOCOL *EfiUsbProtocol
    );

EFI_STATUS
AllocBuffers(
    void
    );

EFI_STATUS
FreeBuffers(
    void
    );
