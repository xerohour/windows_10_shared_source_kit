/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efiusbfndescriptor.c

Abstract:

    This file contains the HID descriptor that the UEFI Battery Charging 
    Application uses to communicate with a host. Based on the Simple Winphone
    IO device descriptors
    
Environment:

    Boot

--*/
#include "chargelib.h"
#include "usbfnsupport.h"

// 9 + 9 + 9 + 7

#define CONFIG_DESCRIPTOR_SIZE  sizeof(EFI_USB_CONFIG_DESCRIPTOR) + \
                                sizeof(EFI_USB_INTERFACE_DESCRIPTOR) + \
                                sizeof(EFI_USB_HID_DESCRIPTOR) + \
                                sizeof(EFI_USB_ENDPOINT_DESCRIPTOR)


//-----------------------------------------------------------------------------
// Preparing the USB Device Info bottom up
//-----------------------------------------------------------------------------


/////////////////////////////////////////////
// HID Report Descriptor                   //
/////////////////////////////////////////////

UCHAR EfiUsbHidReportDescriptor[] = 
{ 
    0x06, 0x00, 0xFF,   // USAGE_PAGE (vendor defined)
    0x09, 0x01,         // USAGE (vendor usage 1)
    0xA1, 0x01,         // COLLECTION (application)
    0x09, 0x01,         // USAGE (vendor usage 1)
    0x15, 0x00,         // LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x00,   // LOGICAL_MAXIMUM (255)
    0x95, 0x01,         // REPORT_COUNT (1)
    0x75, 0x08,         // REPORT_SIZE (8)
    0x81, 0x02,         // INPUT (Data, Var, Abs)
    0xC0                // END_COLLECTION
};


//////////////////////////////////////////////////
// HID Descriptor                               //
//////////////////////////////////////////////////
EFI_USB_HID_DESCRIPTOR EfiUsbHidDescriptor = 
{
    // bLength of the HID descriptor
    0x09,
    //  bDescriptorType  name specifying type of HID descriptor
    0x21,        
    //  2 bytes bcdHID identifies the HID class specification release
    0x0110,                              
    // bCountryCode
    0,                                  
    // bNumDescriptors number of class descriptors 
    //(at least one for Report Descriptor)
    1,                                  
    // bDescriptorType  Constant name for class descriptor
    0x22,                   
    // Total size of the report descriptor DescriptorLength
    (USHORT)sizeof(EfiUsbHidReportDescriptor),            
};



/////////////////////////////////////////////
// Standard Device Descriptor (One Device) //
/////////////////////////////////////////////

EFI_USB_DEVICE_DESCRIPTOR EfiUsbDeviceDescriptor = 
{

    18,                         //  bLength = 18 bytes.
    USB_DEVICE_DESCRIPTOR_TYPE, //  bDescriptorType = DEVICE
    //
    // USB version in Binary-Coded Decimal (i.e., 2.10 is 210H) with which the
    // device and its descriptors are compliant.
    //
    0x0200,                     //  bcdUSB          = 2.10
    0x0,                        //  bDeviceClass    = Communication Device Class
    0x0,                        //  bDeviceSubClass = Unused at this time.
    0x0,                        //  bDeviceProtocol = Unused at this time.
    0,                          //  bMaxPacketSize0 = EP0 buffer size.
                                //              Read from USBFnIo, 0 by default.
    0x045E,                     //  idVendor        = Microsoft Vendor ID.
    0x062A,                     //  idProduct
    0x0000,                     //  bcdDevice
    0x01,                       //  iManufacturer   = OEM should fill this..
    0x02,                       //  iProduct        = OEM should fill this..
    0x00,                       //  iSerialNumber   = OEM should fill this..
    0x01,                       //  bNumConfigs     = 1
};




///////////////////////////////////////////////////////////
// Standard Configuration Descriptor (One Configuration) //
///////////////////////////////////////////////////////////

EFI_USB_CONFIG_DESCRIPTOR EfiUsbConfigDescriptor = 
{
    9,                                              // bLength
    USB_CONFIGURATION_DESCRIPTOR_TYPE,
    CONFIG_DESCRIPTOR_SIZE,                         // wTotalLength
    1,                                              // bNumInterfaces
    1,                                              // bConfigurationValue
    0,                                              // iConfiguration
    USB_CONFIG_RESERVED | USB_CONFIG_SELF_POWERED,  // bmAttributes
    0x32,                                           // MaxPower = 100mA
};


///////////////////////////////////////////////////
// Standard Interface Descriptor (One Interface) //
///////////////////////////////////////////////////


EFI_USB_INTERFACE_DESCRIPTOR EfiUsbInterfaceDescriptor =
{

    9,                  // bLength
    USB_INTERFACE_DESCRIPTOR_TYPE,
    0,                  // bInterfaceNumber
    0,                  // bAlternateSetting
    0,                  // bNumEndpoints (number endpoints used, excluding EP0)
    0x03,               // bInterfaceClass (HID)
    0x00,               // bInterfaceSubClass
    0x00,               // bInterfaceProtocol
    0,                  // ilInterface  (Index of this interface string desc.)

};


//////////////////////////////////////////////////
// HID Interrupt Endpoint
//////////////////////////////////////////////////

EFI_USB_ENDPOINT_DESCRIPTOR EfiUsbEndpointHidInterrupt =
{

    sizeof(EFI_USB_ENDPOINT_DESCRIPTOR),    // bLength
    USB_ENDPOINT_DESCRIPTOR_TYPE,
    0x82,                                   // bEndpointAddress (EP 2, IN)
    USB_ENDPOINT_TYPE_INTERRUPT,            // bmAttributes  (0010 = Bulk)
    0x40,                                   // Read from USBFnIo.
    0x0A,                                   // bInterval (ignored for Bulk)
};


///////////////////////////////////////////////////
// Endpoint Descriptor Table                     //
///////////////////////////////////////////////////
EFI_USB_ENDPOINT_DESCRIPTOR* EfiUsbEndpointDescriptorTable[] =
{
    &EfiUsbEndpointHidInterrupt,
};



///////////////////////////////////////////////////
// Interface Ino Table                           //
///////////////////////////////////////////////////
EFI_USB_INTERFACE_INFO EfiUsbInterfaceInfo =
{
    &EfiUsbInterfaceDescriptor,
    (EFI_USB_ENDPOINT_DESCRIPTOR**) &EfiUsbEndpointDescriptorTable

};



///////////////////////////////////////////////////
// Configuration Information                     //
///////////////////////////////////////////////////
EFI_USB_CONFIG_INFO EfiUsbConfigInfo =
{
    (EFI_USB_CONFIG_DESCRIPTOR*) &EfiUsbConfigDescriptor,
    (EFI_USB_INTERFACE_INFO **) &EfiUsbInterfaceInfo
};


///////////////////////////////////////////////////
// Finally, the device information               //
///////////////////////////////////////////////////
EFI_USB_DEVICE_INFO EfiUsbDeviceInfo =
{
    (EFI_USB_DEVICE_DESCRIPTOR *) &EfiUsbDeviceDescriptor, 
    (EFI_USB_CONFIG_INFO **) &EfiUsbConfigInfo

};


// USB device descriptor
//
UCHAR gUsbWinphoneChargerConfigurationDescriptor[] =
{

/////////////////////////////////////////////
// Standard Device Descriptor (One Device) //
/////////////////////////////////////////////

/* 0  */    18,                         //  bLength = 18 bytes.
/* 1  */    USB_DEVICE_DESCRIPTOR_TYPE, //  bDescriptorType = DEVICE
//
// USB version in Binary-Coded Decimal (i.e., 2.00 is 200H) with which the
// device and its descriptors are compliant.
//
#ifdef  USB_VERSION_110
/* 2  */    0x10, 0x01,                 //  bcdUSB          = 1.10
#else   USB_VERSION_110
/* 2  */    0x00, 0x02,                 //  bcdUSB          = 2.00
#endif  USB_VERSION_110
/* 4  */    0xff,                       //  bDeviceClass    = Communication Device Class
/* 5  */    0xff,                       //  bDeviceSubClass = Unused at this time.
/* 6  */    0xff,                       //  bDeviceProtocol = Unused at this time.
/* 7  */    0,                          //  bMaxPacketSize0 = EP0 buffer size. read from USBFnIo, 0 by default.
/* 8  */    0x5E, 0x04,                 //  idVendor        = Microsoft Vendor ID.
#ifdef  WINCE_SYNC_DEVICE
/* 10 */    0xCE, 0x00,                 //  idProduct
#else   WINCE_SYNC_DEVICE
/* 10 */    0x2A, 0x06,                 //  idProduct
#endif  WINCE_SYNC_DEVICE
/* 12 */    0, 0,                       //  bcdDevice
/* 14 */    0x01,                       //  iManufacturer   = OEM should fill this..
/* 15 */    0x02,                       //  iProduct        = OEM should fill this..
/* 16 */    0x03,                       //  iSerialNumber   = OEM should fill this..
/* 17 */    0x01,                       //  bNumConfigs     = 1

///////////////////////////////////////////////////////////
// Standard Configuration Descriptor (One Configuration) //
///////////////////////////////////////////////////////////

/* 18 */    9,                  // bLength
/* 19 */    USB_CONFIGURATION_DESCRIPTOR_TYPE,
/* 20 */    32, 0,              // wTotalLength
/* 22 */    1,                  // bNumInterfaces
/* 23 */    1,                  // bConfigurationValue
/* 24 */    0,                  // iConfiguration
/* 25 */    0xC0,               // bmAttributes    = Self-Powered & Bus-Powered
/* 26 */    0x32,               // MaxPower        = 100mA

///////////////////////////////////////////////////
// Standard Interface Descriptor (One Interface) //
///////////////////////////////////////////////////

/* 27 */    9,                  // bLength
/* 28 */    USB_INTERFACE_DESCRIPTOR_TYPE,
/* 29 */    0,                  // bInterfaceNumber
/* 30 */    0,                  // bAlternateSetting
/* 31 */    2,                  // bNumEndpoints (number endpoints used, excluding EP0)
/* 32 */    0xff,               // bInterfaceClass
/* 33 */    0xff,               // bInterfaceSubClass
/* 34 */    0xff,               // bInterfaceProtocol
/* 35 */    0,                  // ilInterface  (Index of this interface string desc.)

///////////////////////////////////////////////////
// Standard Endpoint Descriptor (EP1 - BULK OUT) //
///////////////////////////////////////////////////

/* 36 */    7,                  // bLength
/* 37 */    USB_ENDPOINT_DESCRIPTOR_TYPE,
/* 38 */    0x01,               // bEndpointAddress (EP 1, OUT)
/* 39 */    2,                  // bmAttributes  (0010 = Bulk)
/* 40 */    0, 0,               // wMaxPacketSize, read from USBFnIo, 0 by default.
/* 42 */    0,                  // bInterval (ignored for Bulk)

//////////////////////////////////////////////////
// Standard Endpoint Descriptor (EP2 - BULK IN) //
//////////////////////////////////////////////////

/* 43 */    7,                  // bLength
/* 44 */    USB_ENDPOINT_DESCRIPTOR_TYPE,
/* 45 */    0x82,               // bEndpointAddress (EP 2, IN)
/* 46 */    2,                  // bmAttributes  (0010 = Bulk)
/* 47 */    0, 0,               // wMaxPacketSize, read from USBFnIo, 0 by default.
/* 49 */    0                   // bInterval (ignored for Bulk)
};

//
// USB device configuration descriptors.
//
static EFI_USB_ENDPOINT_DESCRIPTOR* gBulkInEndpointDescriptorPtr =
    (EFI_USB_ENDPOINT_DESCRIPTOR*) &gUsbWinphoneChargerConfigurationDescriptor[43];

static EFI_USB_ENDPOINT_DESCRIPTOR* gBulkOutEndpointDescriptorPtr =
    (EFI_USB_ENDPOINT_DESCRIPTOR*) &gUsbWinphoneChargerConfigurationDescriptor[36];

static EFI_USB_ENDPOINT_DESCRIPTOR* gUsbChargerEndpointDescriptorTable[] =
{   (EFI_USB_ENDPOINT_DESCRIPTOR*) &gUsbWinphoneChargerConfigurationDescriptor[36],
    (EFI_USB_ENDPOINT_DESCRIPTOR*) &gUsbWinphoneChargerConfigurationDescriptor[43]
};

static EFI_USB_INTERFACE_INFO gUsbChargerInterfaceInfo =
{
    (EFI_USB_INTERFACE_DESCRIPTOR*) &gUsbWinphoneChargerConfigurationDescriptor[27],
    (EFI_USB_ENDPOINT_DESCRIPTOR**) &gUsbChargerEndpointDescriptorTable[0]
};

static EFI_USB_INTERFACE_INFO* gUsbChargerInterfaceInfoTable[] =
{
    &gUsbChargerInterfaceInfo
};

static EFI_USB_CONFIG_INFO gUsbChargerConfigInfo =
{
    (EFI_USB_CONFIG_DESCRIPTOR *) &gUsbWinphoneChargerConfigurationDescriptor[18],
    (EFI_USB_INTERFACE_INFO**) &gUsbChargerInterfaceInfoTable[0]
};

static EFI_USB_CONFIG_INFO* gUsbChargerConfigInfoTable[] =
{
    &gUsbChargerConfigInfo
};

EFI_USB_DEVICE_INFO gUsbChargerDeviceInfo =
{
    (EFI_USB_DEVICE_DESCRIPTOR* ) &gUsbWinphoneChargerConfigurationDescriptor[0],
    (EFI_USB_CONFIG_INFO**) &gUsbChargerConfigInfoTable[0]
};








