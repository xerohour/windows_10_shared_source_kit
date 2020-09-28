
/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    usbd.c

     1-24-2004

Abstract:


    Functions extracted from usbd.sys to eliminate the dependency on this binary.


Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/

#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "usbd.tmh"
#endif

CCHAR usbfile_usbd_c[] = "usbd.c";

#define USBFILE usbfile_usbd_c

//
// Macros used for select interface and select configuration requests
//

PUSB_COMMON_DESCRIPTOR
UsbhParseDescriptors(
    PDEVICE_OBJECT HubFdo,
    PVOID DescriptorBuffer,
    ULONG TotalLength,
    PVOID StartPosition,
    LONG DescriptorType
    )
/*++

Routine Description:

    Parses a group of standard USB configuration descriptors (returned
    from a device) for a specific descriptor type.

Arguments:

    DescriptorBuffer - pointer to a block of contiguous USB desscriptors.
    TotalLength - size in bytes of the Descriptor buffer
    StartPosition - starting position in the buffer to begin parsing,
                    this must point to the begining of a USB descriptor.
    DescriptorType - USB descritor type to locate.


Return Value:

    pointer to a usb descriptor with a DescriptorType field matching the
            input parameter or NULL if not found.

--*/
{
    PUCHAR pch = (PUCHAR) StartPosition, end;
    PUSB_COMMON_DESCRIPTOR usbDescriptor, foundUsbDescriptor = NULL;

    end = ((PUCHAR) (DescriptorBuffer)) + TotalLength;

    while (pch < end) {
        // see if we are pointing at an interface
        // if not skip over the other junk
        usbDescriptor = (PUSB_COMMON_DESCRIPTOR) pch;

        //
        // Ensure the descriptor header is within bounds
        //
        if (end - pch < sizeof(USB_COMMON_DESCRIPTOR)){
            break;
        }

        if (usbDescriptor->bDescriptorType ==
            DescriptorType) {
            foundUsbDescriptor = usbDescriptor;
            break;
        }

        // note we still stop in debug because the
        // device passed us bad data, the following
        // test will prevent us from hanging
        if (usbDescriptor->bLength == 0) {

            HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_DescriptorParseError,
                        0xFFFFFFFF, 0xFFFFFFFF, DescriptorBuffer, TotalLength);
            break;
        }

        if (pch > (PUCHAR) (pch + usbDescriptor->bLength)) {
            // rollover
            break;
        }

        pch += usbDescriptor->bLength;
    }

    return foundUsbDescriptor;
}


PUSB_INTERFACE_DESCRIPTOR
UsbhParseConfigurationDescriptorEx(
    PDEVICE_OBJECT HubFdo,
    PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    PVOID StartPosition,
    LONG InterfaceNumber,
    LONG AlternateSetting,
    LONG InterfaceClass,
    LONG InterfaceSubClass,
    LONG InterfaceProtocol
    )
/*++

Routine Description:

    Parses a standard USB configuration descriptor (returned from a device)
    for a specific interface, alternate setting class subclass or protocol
    codes.

Arguments:

    ConfigurationDescriptor - Pointer to a configuartion descriptor (input) buffer -- including
            all the interface and endpoint data.

    INPUT PARAMETERS:
    StartPosition - Starting postition in the buffer to begin searching.
    InterfaceNumber - Interface number to locate, -1 means is wildcard.
    AlternateSetting - Alternate setting to locate, -1 means is wildcard.
    InterfaceClass - Interface calss code to locate, -1 means is wildcard.
    InterfaceSubClass - Interface subclass code to locate, -1 means is wildcard.
    InterfaceProtocol - Interface protocol to locate in buffer, -1 means is wildcard.

Return Value:

    Pointer to the interface descriptor that matches the input parameters with the input buffer.
    NULL is returned if no descriptor matches the input parameters.

--*/
{
    PUSB_INTERFACE_DESCRIPTOR foundInterfaceDescriptor = NULL;
    PUSB_INTERFACE_DESCRIPTOR interfaceDescriptor;
    PUCHAR end;

    UsbhAssert(HubFdo, ConfigurationDescriptor->bDescriptorType
        == USB_CONFIGURATION_DESCRIPTOR_TYPE);

    end = (PUCHAR) ConfigurationDescriptor + ConfigurationDescriptor->wTotalLength;

    //
    // we walk the table of descriptors looking for an
    // interface descriptor with parameters matching those
    // passed in.
    //

    do {
        //
        // Search for descriptor type 'interface'
        //

        interfaceDescriptor = (PUSB_INTERFACE_DESCRIPTOR)
            UsbhParseDescriptors(HubFdo,
                                  ConfigurationDescriptor,
                                  ConfigurationDescriptor->wTotalLength,
                                  StartPosition,
                                  USB_INTERFACE_DESCRIPTOR_TYPE);

        //
        // do we have a match?
        //
        if (interfaceDescriptor != NULL) {

            foundInterfaceDescriptor =
                interfaceDescriptor;

            if (InterfaceNumber != -1 &&
                (!IsFieldInBounds(interfaceDescriptor, end, USB_INTERFACE_DESCRIPTOR, bInterfaceNumber) ||
                 interfaceDescriptor->bInterfaceNumber != InterfaceNumber)) {
                foundInterfaceDescriptor = NULL;
            }

            if (AlternateSetting != -1 &&
                (!IsFieldInBounds(interfaceDescriptor, end, USB_INTERFACE_DESCRIPTOR, bAlternateSetting) ||
                 interfaceDescriptor->bAlternateSetting != AlternateSetting)) {
                foundInterfaceDescriptor = NULL;
            }

            if (InterfaceClass != -1 &&
                (!IsFieldInBounds(interfaceDescriptor, end, USB_INTERFACE_DESCRIPTOR, bInterfaceClass) ||
                 interfaceDescriptor->bInterfaceClass != InterfaceClass)) {
                foundInterfaceDescriptor = NULL;
            }

            if (InterfaceSubClass != -1 &&
                (!IsFieldInBounds(interfaceDescriptor, end, USB_INTERFACE_DESCRIPTOR, bInterfaceSubClass) ||
                 interfaceDescriptor->bInterfaceSubClass != InterfaceSubClass)) {
                foundInterfaceDescriptor = NULL;
            }

            if (InterfaceProtocol != -1 &&
                (!IsFieldInBounds(interfaceDescriptor, end, USB_INTERFACE_DESCRIPTOR, bInterfaceProtocol) ||
                 interfaceDescriptor->bInterfaceProtocol != InterfaceProtocol)) {
                foundInterfaceDescriptor = NULL;
            }

            StartPosition =
                ((PUCHAR)interfaceDescriptor) + interfaceDescriptor->bLength;
        }

        if (foundInterfaceDescriptor) {
            break;
        }
        //
        // An Interface Descriptor of bLength = 0 will result in an Infinite loop 
        // since we'll keep on passing same StartPosition. Bail out if bLength is 0.
        //
        if (interfaceDescriptor != NULL &&
            interfaceDescriptor->bLength == 0) {
            interfaceDescriptor = NULL;
        }
    } while (interfaceDescriptor != NULL);


    return (foundInterfaceDescriptor);
}


PURB
UsbhCreateConfigurationRequestEx(
    __in PDEVICE_OBJECT HubFdo,
    __in_bcount(CbCfgDescriptor) PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    __in USHORT CbCfgDescriptor,
    __in_bcount(CbInterfaceDescriptor) PUSB_INTERFACE_DESCRIPTOR InterfaceDescriptor,
    __in ULONG CbInterfaceDescriptor
    )
/*++

Routine Description:

    Builds a URB configuration request from a from the interface descriptor.

    hubs have only one interface (with multiple alt settings)

Arguments:

    ConfigurationDescriptor - pointer to the config descriptor to base the request.

    InterfaceList - ptr to an array of interface/InterfaceInformation structs for
        each interface we wish to select in the configurstion.

Return Value:

    Pointer to initailized select_configuration urb.

--*/
{
    PURB urb = NULL;
    USHORT siz;

    //
    // Our mission here is to construct a URB of the proper size and format for a select_configuration
    // request.
    //
    // This function uses the configurstion descritor as a reference and builds a URB with interface_information
    // structures for each interface requested in the interface list passed in
    //
    // NOTE: the config descriptor may contain interfaces that the caller does not specify in the list -- in 
    // this case the other interfaces will be ignored.
    //


    // includes enough for 1 interface (struct _USBD_INTERFACE_INFORMATION) and 1 pipe 
    // (struct _USBD_PIPE_INFORMATION)
    siz = sizeof(struct _URB_SELECT_CONFIGURATION);

    if (InterfaceDescriptor->bNumEndpoints > 1) {
        siz += sizeof(struct _USBD_PIPE_INFORMATION)*(InterfaceDescriptor->bNumEndpoints-1);
    }            

    UsbhAllocatePool_Z(urb, NonPagedPool, siz);

    if (urb) {

        PUSBD_INTERFACE_INFORMATION iface;
        LONG j;
         
        //
        // now all we have to do is initialize the urb
        //

        RtlZeroMemory(urb, siz);

        iface = &urb->UrbSelectConfiguration.Interface;

        iface->InterfaceNumber =
            InterfaceDescriptor->bInterfaceNumber;

        iface->AlternateSetting =
            InterfaceDescriptor->bAlternateSetting;

        iface->NumberOfPipes =
            InterfaceDescriptor->bNumEndpoints;

        for (j=0; j<InterfaceDescriptor->bNumEndpoints; j++) {
            iface->Pipes[j].MaximumTransferSize =
                USBD_DEFAULT_MAXIMUM_TRANSFER_SIZE;
            iface->Pipes[j].PipeFlags = 0;
        }

        iface->Length =
            (USHORT) GET_USBD_INTERFACE_SIZE(InterfaceDescriptor->bNumEndpoints);

        UsbhAssert(HubFdo, (((PUCHAR) iface) + iface->Length <= ((PUCHAR) urb) + siz));

        iface = (PUSBD_INTERFACE_INFORMATION) ((PUCHAR) iface + iface->Length);

        urb->UrbHeader.Length = siz;
        urb->UrbHeader.Function = URB_FUNCTION_SELECT_CONFIGURATION;
        urb->UrbSelectConfiguration.ConfigurationDescriptor = ConfigurationDescriptor;
    }

//#if DBG
//    interfaceList = InterfaceList;
//
//   while (interfaceList->InterfaceDescriptor) {
//        USBD_KdPrint(3, ("'InterfaceList, Interface = %x\n",
//            interfaceList->Interface));/
//        USBD_KdPrint(3, ("'InterfaceList, InterfaceDescriptor = %x\n",
//            interfaceList->InterfaceDescriptor));
//        interfaceList++;
//    }
//
//    USBD_KdPrint(3, ("'urb = %x\n", urb));
//#endif

    return urb;
}

