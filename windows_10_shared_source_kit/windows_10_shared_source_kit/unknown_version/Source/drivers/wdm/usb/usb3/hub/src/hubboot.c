/*++
Copyright (c) Microsoft Corporation

Module Name:

    hubboot.c

Abstract:

    This file contains functions that are used by rest
    of the hub driver to interact with the kernel to
    notify about presence or absence od boot devices 
    for the purpose of dealing with suprise removal
    of boot device

Author:

Environment:

    Kernel mode only.

--*/

#include <ntosp.h>
#include <wdf.h>
#include <initguid.h>
#include <devpkey.h>

#include "project.h"
#include "tracing.h"

#include "hubboot.tmh"

typedef struct _DEVICE_CONTEXT     DEVICE_CONTEXT, *PDEVICE_CONTEXT;


typedef
VOID
(*PFN_EX_NOTIFY_BOOT_DEVICE_REMOVAL) (
    __in
        EX_BOOT_DEVICE_HANDLE Handle
    );  


PFN_EX_NOTIFY_BOOT_DEVICE_REMOVAL  g_ExNotifyBootDeviceRemoval;

typedef
NTSTATUS
(*PFN_EX_REGISTER_BOOT_DEVICE) (
    __in
        PEX_BOOT_DEVICE_REGISTRATION Registration,
    __out
        EX_BOOT_DEVICE_HANDLE *Handle
    );  

PFN_EX_REGISTER_BOOT_DEVICE    g_ExRegisterBootDevice;


__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBMISC_IsBootDeviceReady(
    __in 
        PDEVICE_CONTEXT     DeviceContext
    );

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBBOOT_IsBootDeviceReady(
    __in 
        PDEVICE_OBJECT      Pdo,
    __in_opt 
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    This callback is invoked by the kernel to determine
    if the boot device is back. This wrapper function is
    needed to avoid header file conflicts
    
Arguments:

Return Value:

    BOOLEAN

--*/
{
    UNREFERENCED_PARAMETER(Pdo);
    
    return  HUBMISC_IsBootDeviceReady(DeviceContext);
}


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBBOOT_NotifyBootDeviceRemoval(
    __in 
        PVOID                   BootDeviceHandle
    )
/*++

Routine Description:

    This routine is invoked whenever a boot device goes
    missing. This wrapper function is needed to avoid 
    header file conflicts 

Arguments:

Return Value:

    VOID

--*/
{

        if (g_ExNotifyBootDeviceRemoval != NULL) {

            g_ExNotifyBootDeviceRemoval((EX_BOOT_DEVICE_HANDLE)BootDeviceHandle);
        }

}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBBOOT_RegisterBootDevice (
    __in
        PDEVICE_CONTEXT                 DeviceContext,
    __out
        PVOID *                         BootDeviceHandle
    )
/*++

Routine Description:

    This routine is invoked whenever we get a usage notification 
    indicating that there is a boot device present. 

Arguments:

Return Value:

    VOID

--*/
{
    NTSTATUS                    status;
    EX_BOOT_DEVICE_REGISTRATION registration;
    UNICODE_STRING              functionName;

    //
    // Import kernel functions that are only supported in Windows 8 and later.
    //

    RtlInitUnicodeString(&functionName, L"ExNotifyBootDeviceRemoval");
    g_ExNotifyBootDeviceRemoval = (PFN_EX_NOTIFY_BOOT_DEVICE_REMOVAL)(ULONG_PTR)MmGetSystemRoutineAddress(&functionName);

    RtlInitUnicodeString(&functionName, L"ExRegisterBootDevice");
    g_ExRegisterBootDevice = (PFN_EX_REGISTER_BOOT_DEVICE)(ULONG_PTR)MmGetSystemRoutineAddress(&functionName);

    status = STATUS_NOT_SUPPORTED;

    TRY {

        if (g_ExRegisterBootDevice == NULL) {

            LEAVE;
        }
        
        RtlZeroMemory(&registration, sizeof(registration));
        registration.Version = EX_BOOT_DEVICE_REGISTRATION_VERSION;
        registration.DriverObject = WdfDriverWdmGetDriverObject(WdfGetDriver());
        registration.DeviceObject = NULL;
        registration.IsBootDeviceReady = HUBBOOT_IsBootDeviceReady;
        registration.Context = DeviceContext;

        status = g_ExRegisterBootDevice(&registration, 
                                      (EX_BOOT_DEVICE_HANDLE * )BootDeviceHandle);

        if (!NT_SUCCESS(status)) {                                                            

            LEAVE;
        }

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(PASSIVE_LEVEL)
BOOLEAN
HUBBOOT_IsBootDeviceExternal (
    __in
        WDFDEVICE               Device
    )
/*++

Routine Description:

    This routine is invoked whenever we get a usage notification 
    indicating that there is a boot or paging device present.
    This routine determines if the device is internal or external
    to the system

Arguments:

Return Value:

    VOID

--*/
{
    DEVPROP_BOOLEAN     isInLocalMachineContainer;
    ULONG               size;                     
    NTSTATUS            nts;         
    DEVPROPTYPE         type;                     
    PDEVICE_OBJECT      pdo;
    BOOLEAN             isDeviceExternal;
     
    size = 0;
    type = DEVPROP_TYPE_EMPTY;
    isDeviceExternal = TRUE;
    
    pdo = WdfDeviceWdmGetPhysicalDevice(Device);
    //
    // Retrieve the device removable property.
    //
    nts = IoGetDevicePropertyData(pdo,
                                  &DEVPKEY_Device_InLocalMachineContainer,
                                  0,
                                  0,
                                  sizeof(isInLocalMachineContainer),
                                  &isInLocalMachineContainer,
                                  &size,
                                  &type);
    
    if (NT_SUCCESS(nts)) {
        isDeviceExternal = (isInLocalMachineContainer == 0);
    }

 
    return isDeviceExternal;
}