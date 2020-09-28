//
//    Copyright (C) Microsoft.  All rights reserved.
//


//
// Function Prototypes
//

#define UXD_EVENT_DISABLE       0x01
#define UXD_EVENT_DISCONNECT    0x02
#define UXD_EVENT_SHUTDOWN      0x03

NTSTATUS
UsbhDeleteUxdSettings(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    ULONG UdxEvent
    );

NTSTATUS
UsbhUpdateUxdSettings(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    BOOLEAN UpdateHardwareId,
    BOOLEAN *UpdateRequired
    );

NTSTATUS
UsbhBuildUxdPnpId(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    );

NTSTATUS
UsbhQueryParentHubConfig(
    PDEVICE_OBJECT HubFdo
    );

NTSTATUS
UsbhCopyPnpString(
    __in PUSB_ID_STRING Output,
    __out PUSB_ID_STRING Input
    );

NTSTATUS
UsbhAcquirePdoUxdLock(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    );

NTSTATUS
UsbhReleasePdoUxdLock(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    );

NTSTATUS
UsbhGetGlobalUxdSettings(
    PDEVICE_OBJECT HubFdo,
    PHUB_GLOBAL_UXD_SETTINGS HubUxdSettings
    );

NTSTATUS
UsbhUxdShutdown(
    PDEVICE_OBJECT HubFdo
    );
