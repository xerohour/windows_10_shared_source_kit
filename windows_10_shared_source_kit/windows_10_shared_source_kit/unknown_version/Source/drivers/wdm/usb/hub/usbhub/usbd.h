//
//    Copyright (C) Microsoft.  All rights reserved.
//


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
    );

PURB
UsbhCreateConfigurationRequestEx(
    __in PDEVICE_OBJECT HubFdo,
    __in_bcount(CbCfgDescriptor) PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    __in USHORT CbCfgDescriptor,
    __in_bcount(CbInterfaceDescriptor) PUSB_INTERFACE_DESCRIPTOR InterfaceDescriptor,
    __in ULONG CbInterfaceDescriptor
    );

