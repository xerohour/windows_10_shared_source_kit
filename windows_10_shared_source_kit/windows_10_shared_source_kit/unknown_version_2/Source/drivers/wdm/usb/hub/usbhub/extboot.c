    
/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    extboot.c

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


#include "ntosp.h"
#include "extboot.h"

BOOLEAN
ExIsBootDeviceReady(
    __in PDEVICE_OBJECT Pdo,
    __in_opt PVOID Context
    )
{
    USBH_CHECK_BOOT_DEVICE_READY callback;
    
    callback = (USBH_CHECK_BOOT_DEVICE_READY)(ULONG_PTR)Context;

    return (*callback)(Pdo);
}


NTSTATUS
UsbhRegisterBootDeviceNotification(
    __in struct _DEVICE_OBJECT *Pdo,
    __in USBH_CHECK_BOOT_DEVICE_READY CheckBootDeviceReady,
    __out PVOID *Handle) 
{
    EX_BOOT_DEVICE_REGISTRATION registration;
    NTSTATUS status;
    
    RtlZeroMemory(&registration, sizeof(registration));
    registration.Version = EX_BOOT_DEVICE_REGISTRATION_VERSION;
    registration.DriverObject = Pdo->DriverObject;
    registration.DeviceObject = Pdo;
    registration.IsBootDeviceReady = ExIsBootDeviceReady;
    registration.Context = (PVOID)(ULONG_PTR)CheckBootDeviceReady;

    status = ExRegisterBootDevice(&registration, (EX_BOOT_DEVICE_HANDLE*)Handle);
    if (!NT_SUCCESS(status)) {
        if (KdRefreshDebuggerNotPresent() == FALSE) {
        
            DbgPrint("ExRegisterBootDevice failed with 0x%x\n",status);
        
            DbgBreakPoint();
        }
    }
    return status;
}


BOOLEAN
UsbhNotifyBootDeviceRemoval(
    __in PVOID Handle)
{
    return ExNotifyBootDeviceRemoval((EX_BOOT_DEVICE_HANDLE)Handle);
}


NTSTATUS
UsbhCheckPortableOperatingSystem(
    __in PBOOLEAN PortableOperatingSystem)
{
    return RtlCheckPortableOperatingSystem(PortableOperatingSystem);
}

