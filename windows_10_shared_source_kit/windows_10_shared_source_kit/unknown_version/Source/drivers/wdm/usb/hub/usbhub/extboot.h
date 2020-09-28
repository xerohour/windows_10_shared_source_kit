/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    extboot.h

     3-30-2011

Abstract:

    Provides external boot support routines

Module:

Author:

    pingxie

Environment:

    Kernel mode

Revision History:

--*/

typedef
BOOLEAN
(*USBH_CHECK_BOOT_DEVICE_READY)(
    __in struct _DEVICE_OBJECT *Pdo);

NTSTATUS
UsbhRegisterBootDeviceNotification(
    __in struct _DEVICE_OBJECT *Pdo,
    __in USBH_CHECK_BOOT_DEVICE_READY HandleBootDeviceRemoval,
    __out PVOID *ProviderHandle);
    

BOOLEAN
UsbhNotifyBootDeviceRemoval(
    __in PVOID ProviderHandle);


NTSTATUS
UsbhCheckPortableOperatingSystem(
    __in PBOOLEAN PortableOperatingSystem);

