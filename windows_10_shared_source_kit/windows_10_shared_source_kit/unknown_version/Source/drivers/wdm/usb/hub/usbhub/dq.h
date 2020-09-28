//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: dq.c */


BOOLEAN
UsbhValidateConfigurationDescriptor(
    PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    ULONG LengthInBytes,
    USBD_STATUS *UsbdStatus,
    BOOLEAN ExtendedValidation
    );


BOOLEAN
UsbhValidateStringDescriptor(
    PDEVICE_OBJECT HubFdo,
    PUSB_STRING_DESCRIPTOR StringDescriptor,
    ULONG BufferLength,
    USBD_STATUS *UsbdStatus
    );


BOOLEAN
UsbhValidateSerialNumberString(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString,
    USBD_STATUS *ReturnUsbdStatus,
    BOOLEAN *FailEnumeration
    );

#if 0
#endif

BOOLEAN
UsbhValidateDeviceDescriptor(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_DESCRIPTOR DeviceDescriptor,
    ULONG BufferLength,
    USBD_STATUS *UsbdStatus,
    PBOOLEAN IsBillboard
    );

VOID
UsbhCheckDeviceErrata(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    );

VOID
UsbhCheck4GlobalErrata(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    );

VOID
UsbhCheckHubErrata(
    PDEVICE_EXTENSION_HUB hubExt
    );

BOOLEAN
UsbhValidateMsOs20DescriptorSet(
    __in PDEVICE_OBJECT HubFdo,
    __in PDEVICE_OBJECT Pdo,
    __in PMSOS20_DESCRIPTOR_SET_INFO DescriptoSetInfo
    );

BOOLEAN
UsbhValidateBOSDescriptorSet (
    __in PDEVICE_OBJECT HubFdo,
    __in PUSB_BOS_DESCRIPTOR BOSDescriptor,
    __in ULONG BufferLength,
    __out_opt PBOS_DESC_SET_INFO BOSInfo
    );

VOID
UsbhQueryKseGlobalFlags(
    );
