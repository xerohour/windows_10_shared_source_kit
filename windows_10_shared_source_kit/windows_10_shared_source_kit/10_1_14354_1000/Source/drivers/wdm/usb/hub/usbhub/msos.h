//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: msos.c */

#ifdef HUB_WPP
#endif

NTSTATUS
UsbhGetMSOS_Descriptor(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    );


VOID
UsbhInstallMsOsExtendedProperties(
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo
    );

PMS_EXT_CONFIG_DESC
UsbhGetMsOsExtendedConfigDesc(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    );

NTSTATUS
UsbhGetMsOsContainerID(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    );

BOOLEAN
UsbhGetNextMsOs20Descriptor(
    __in PUCHAR EndOfDescriptorSet,
    __inout PMSOS20_COMMON_DESCRIPTOR *CurrentDescriptor
    );

VOID
UsbhInstallMsOs20RegistryProperties(
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo
    );


NTSTATUS
UsbhGetMsOs20DescriptorSet(
    PDEVICE_OBJECT  HubFdo,
    PDEVICE_OBJECT  Pdo,
    PMSOS20_DESCRIPTOR_SET_INFO DescriptoInfo
    );

NTSTATUS
UsbhSendMsOs20AltEnumCommand(
    PDEVICE_OBJECT  HubFdo,
    PDEVICE_OBJECT  Pdo,
    PMSOS20_DESCRIPTOR_SET_INFO DescriptorInfo
    );
