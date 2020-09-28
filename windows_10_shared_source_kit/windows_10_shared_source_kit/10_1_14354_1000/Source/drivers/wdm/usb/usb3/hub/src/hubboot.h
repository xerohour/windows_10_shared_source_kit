/*++
Copyright (c) Microsoft Corporation

Module Name:

    hubboot.c

Abstract:

    This file contains functions that are used by reset
    of the hub driver to interact with the kernel to
    notify about presence or absence of boot devices 
    for the purpose of dealing with suprise removal
    of boot device

Author:

Environment:

    Kernel mode only.

--*/

#pragma once

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBBOOT_RegisterBootDevice (
    __in
        PDEVICE_CONTEXT                 DeviceContext,
    __out
        PVOID *                         BootDeviceHandle
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBBOOT_NotifyBootDeviceRemoval (
    __in 
        PVOID                   BootDeviceHandle
    );


__drv_maxIRQL(PASSIVE_LEVEL)
BOOLEAN
HUBBOOT_IsBootDeviceExternal (
    __in
        WDFDEVICE               Device
    );
