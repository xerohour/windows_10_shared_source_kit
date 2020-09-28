/*++

Copyright (c) 2010  Microsoft Corporation

Module Name:

    USBDUMP.c

Abstract:

    USB dump driver support

Environment:

    kernel mode only

Notes:

Revision History:

    7-19-10 

--*/

#include <wdm.h>
#include "usb.h"
#include "usbboot.h"

#define USBDUMP_DESCRIPTOR_MAKE_TYPE_AND_INDEX(d, i) ((USHORT)((USHORT)d<<8 | i))

#define USBDUMP_INIT_SETUP_PACKET(s, brequest, \
    direction, recipient, typ, wvalue, windex, wlength) \
    {\
    (s).bRequest = (brequest);\
    (s).bmRequestType.Dir = (direction);\
    (s).bmRequestType.Type = (typ);\
    (s).bmRequestType.Recipient = (recipient);\
    (s).bmRequestType.Reserved = 0;\
    (s).wValue.W = (wvalue);\
    (s).wIndex.W = (windex);\
    (s).wLength = (wlength);\
    }
NTSTATUS
EHCIDumpControlTransfer(
    __inout PVOID ControllerContext,
    __in_xcount(8) PUCHAR SetupPacket,
    __inout PHYSICAL_ADDRESS Buffer,
    __inout ULONG BufferLength
    );


NTSTATUS
__stdcall
UsbDumpResetDevice(
    __inout PEHCI_CRASHDUMP_CONTEXT ControllerContext,
    __inout PUSB_CONTROL_TRANSFER_FUNCTION ControlTransfer,
    __inout PULONG DeviceAddress
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    PHYSICAL_ADDRESS            physcratch;
    PUCHAR                      scratch;

    //
    // Need to get the device descriptor from storage device
    //
    // Setup packet for get device descriptor
    //    
    USBDUMP_INIT_SETUP_PACKET(setupPacket,
                               USB_REQUEST_GET_DESCRIPTOR,   // bRequest
                               BMREQUEST_DEVICE_TO_HOST,     // Dir
                               BMREQUEST_TO_DEVICE,          // Recipient
                               BMREQUEST_STANDARD,           // Type
                               USBDUMP_DESCRIPTOR_MAKE_TYPE_AND_INDEX(USB_DEVICE_DESCRIPTOR_TYPE, 0), //  wValue
                               0,                            // wIndex
                               USB_DEFAULT_MAX_PACKET);      // wLength

    scratch = (PUCHAR)ControllerContext->CommonBufferPool + DUMP_BUFFER_HEADER_SIZE;
    physcratch.QuadPart = ControllerContext->CommonBufferPA.QuadPart;
    physcratch.QuadPart += DUMP_BUFFER_HEADER_SIZE;
    RtlZeroMemory(scratch, sizeof(USB_DEVICE_DESCRIPTOR));

    //
    // This stall was determined experimentally and required for 32 bit NVidia controllers
    //
    KeStallExecutionProcessor(5000);
    ntStatus = ControlTransfer(ControllerContext,
                               (PUCHAR)&setupPacket,
                               physcratch,
                               USB_DEFAULT_MAX_PACKET);
    
    if (!NT_SUCCESS(ntStatus)) {
    
        goto EHCIResetDeviceEnd;
    }

    //
    // Need to do a set_address on the storage address using the same initial 
    // device address
    //
    //
    // Setup packet for setting device address
    //    
    USBDUMP_INIT_SETUP_PACKET(setupPacket,
                               USB_REQUEST_SET_ADDRESS,    // bRequest
                               BMREQUEST_HOST_TO_DEVICE,   // Dir
                               BMREQUEST_TO_DEVICE,        // Recipient
                               BMREQUEST_STANDARD,         // Type
                               (USHORT)ControllerContext->DeviceAddress, // wValue
                               0,                          // wIndex
                               0);                         // wLength

    physcratch.QuadPart = 0;

    //
    // This stall was determined experimentally and required for 32 bit NVidia controllers
    //
    KeStallExecutionProcessor(5000);
    ntStatus = ControlTransfer(ControllerContext,
                               (PUCHAR)&setupPacket,
                               physcratch,
                               0);

    if (!NT_SUCCESS(ntStatus)) {
    
        goto EHCIResetDeviceEnd;
    }

    //
    // Update storage device's control ep's device address 
    //
    *DeviceAddress &= 0xFFFFFF80;
    *DeviceAddress |= (UCHAR)ControllerContext->DeviceAddress;

    //
    // Need to get the device descriptor from storage device
    //
    // Setup packet for get device descriptor
    //    
    USBDUMP_INIT_SETUP_PACKET(setupPacket,
                               USB_REQUEST_GET_DESCRIPTOR,   // bRequest
                               BMREQUEST_DEVICE_TO_HOST,     // Dir
                               BMREQUEST_TO_DEVICE,          // Recipient
                               BMREQUEST_STANDARD,           // Type
                               USBDUMP_DESCRIPTOR_MAKE_TYPE_AND_INDEX(USB_DEVICE_DESCRIPTOR_TYPE, 0), //  wValue
                               0,                            // wIndex
                               USB_DEFAULT_MAX_PACKET);      // wLength

    scratch = (PUCHAR)ControllerContext->CommonBufferPool + DUMP_BUFFER_HEADER_SIZE;
    physcratch.QuadPart = ControllerContext->CommonBufferPA.QuadPart;
    physcratch.QuadPart += DUMP_BUFFER_HEADER_SIZE;
    RtlZeroMemory(scratch, sizeof(USB_DEVICE_DESCRIPTOR));

    //
    // This stall was determined experimentally and required for 32 bit NVidia controllers
    //
    KeStallExecutionProcessor(5000);
    ntStatus = ControlTransfer(ControllerContext,
                               (PUCHAR)&setupPacket,
                               physcratch,
                               USB_DEFAULT_MAX_PACKET);

    if (!NT_SUCCESS(ntStatus)) {
    
        goto EHCIResetDeviceEnd;
    }

    //
    // Need to select the initial configuration 
    //
    USBDUMP_INIT_SETUP_PACKET(setupPacket,
                               USB_REQUEST_SET_CONFIGURATION,       // bRequest
                               BMREQUEST_HOST_TO_DEVICE,            // Dir
                               BMREQUEST_TO_DEVICE,                 // Recipient
                               BMREQUEST_STANDARD,                  // Type
                               ControllerContext->ConfigurationId,  // wValue, storage device initial config num
                               0,                                   // wIndex
                               0);                                  // wLength

    physcratch.QuadPart = 0;

    //
    // This stall was determined experimentally and required for 32 bit NVidia controllers
    //
    KeStallExecutionProcessor(5000);
    ntStatus = ControlTransfer(ControllerContext,
                               (PUCHAR)&setupPacket,
                               physcratch,
                               0);

    if (!NT_SUCCESS(ntStatus)) {
    
        goto EHCIResetDeviceEnd;
    }


    //
    // Select Alternate Setting for UASP.
    //
    if (ControllerContext->AlternateSettingId != 0) {

        USBDUMP_INIT_SETUP_PACKET(setupPacket,
                                  USB_REQUEST_SET_INTERFACE,            // bRequest
                                  BMREQUEST_HOST_TO_DEVICE,             // Dir
                                  BMREQUEST_TO_INTERFACE,               // Recipient
                                  BMREQUEST_STANDARD,                   // Type
                                  ControllerContext->AlternateSettingId,// wValue
                                  ControllerContext->InterfaceId,       // wIndex
                                  0);                                   // wLength
        
        physcratch.QuadPart = 0;
        
        //
        // This stall was determined experimentally and required for 32 bit NVidia controllers
        //
        KeStallExecutionProcessor(5000);

        ntStatus = ControlTransfer(ControllerContext,
                                   (PUCHAR)&setupPacket,
                                   physcratch,
                                   0);
        
        if (!NT_SUCCESS(ntStatus)) {
        
            goto EHCIResetDeviceEnd;
        }
    }

EHCIResetDeviceEnd:


    return ntStatus;
}

