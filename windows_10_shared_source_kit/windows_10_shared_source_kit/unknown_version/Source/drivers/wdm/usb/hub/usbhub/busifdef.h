/*************************************************************************************

Module: busifdef.h

Name: Bus interfaces hub and usbd 

Copyright (c) Microsoft Corporation. All rights reserved

Abstract: 

    Functions exported by the USB port driver. (USBPORT) USBHUB interface is used exclusively by the the usbhub driver
    for enumeration of the bus.  USBD interface is used by function drivers including the usbhub driver. Documentation
    in this file also serves as functional spec for this API.
   

BusContext Parameter - This value is opaque to the caller, USBHUB.SYS, it only
has meaning to the driver that implements the interface e.g. USBPORT.SYS.
The value is returned in the structure passed to
IRP_MJ_PNP.IRP_MN_QUERY_INTERFACE and is passed in as a parameter to the
services.

USBD bus interface function (V3)
    InterfaceReference
    InterfaceDereference
    GetUSBDIVersion
    QueryBusTime
    SubmitIsoOutUrb
    QueryBusInformation
    IsDeviceHighSpeed
    EnumLogEntry

USBHUB bus Interface(V6)    
    InterfaceReference
    InterfaceDereference
    CreateUsbDeviceEx
    InitializeUsbDeviceEx
    GetUsbDescriptors
    RemoveUsbDeviceEx
    RestoreUsbDevice
    GetPortHackFlags
    QueryDeviceInformation
    GetControllerInformation
    ControllerSelectiveSuspend
    GetExtendedHubInformation
    GetRootHubSymbolicName
    GetDeviceBusContext
    Initialize20Hub
    RootHubInitNotification
    FlushTransfers
    SetDeviceHandleData
    HubIsRoot
    AcquireBusSemaphore
    ReleaseBusSemaphore
    CalculatePipeBandwidth
    SetBusSystemWakeMode
    SetDeviceFlag
    HubTestPoint
    GetDevicePerformanceInfo
    WaitAsyncPowerUp
    GetDeviceAddress
    RefDeviceHandle
    DerefDeviceHandle
    SetDeviceHandleIdleReadyState
    
***************************************************************************************/


#ifndef   __BUSIFDEF_H__
#define   __BUSIFDEF_H__

/*
Routine:GetRootHubSymbolicName

NTSTATUS GetRootHubSymbolicName(
    IN PVOID BusContext,
    IN OUT PVOID HubInformationBuffer,
    IN ULONG HubInformationBufferLength,
    OUT PULONG HubNameActualLength
    );

Routine Description:

This API is obsolete in Longhorn, it returns the same information as IOCTL_INTERNAL_GET_HUB_NAME but this API
only works for the root.  The hub driver uses IOCTL_INTERNAL_GET_HUB_NAME instead.

*/

/*

Routine:GetExtendedHubInformation

NTSTATUS GetExtendedHubInformation(
    IN PVOID BusContext,
    IN PDEVICE_OBJECT HubPhysicalDeviceObject,
    IN OUT PVOID HubInformationBuffer,
    IN ULONG HubInformationBufferLength,
    IN OUT PULONG LengthOfDataReturned
    );

Routine Description:

In order to simplify the implementation of usbport.sys this API is no longer supported or used in version 6 and beyond. 
The usbhub.sys driver accesses this information internally.


*/

/*
Routine:GetDevicePerformanceInfo

NTSTATUS GetDevicePerformanceInfo(
    IN PVOID BusContext,
    IN PUSB_DEVICE_HANDLE DeviceHandle,
    IN OUT PVOID DeviceInformationBuffer,
    IN ULONG DeviceInformationBufferLength,
    IN OUT PULONG LengthOfDataReturned,
    );

Routine Description:

Service exported for use by the hub driver.  This api returns various
information about the performance of USB devices attached to the system.

IRQL = PASSIVE_LEVEL

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - Handle for the device you want information about. This is the
handle returned by CreateUsbDevice or returned in IOCTL api
IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE.

DeviceInformationBuffer - buffer for returning device information.  The type
of data returned is defined by the value passed in the InformationLevel field
of the buffer passed in.
Levels Supported:
_ _USB_DEVICE_PERFORMANCE_INFO_0, level = 0

DeviceInformationBufferLength - length of DeviceInformationBuffer in bytes

LengthOfDataReturned - length of data copied into DeviceInformationBuffer.
*/

/*
Routine:HubTestPoint

VOID HubTestPoint(
    PVOID BusContext,
    PVOID DeviceHandle,
    ULONG Opcode,
    PVOID TestData
    )

Routine Description:

This service is exported for use by the hub driver. It is used by internal
system test tools.

IRQL = PASSIVE

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - Handle for the device you want information about. This is the
handle returned by CreateUsbDevice or returned in IOCTL api
IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE.

Opcode - defines internal test.
TestData - opaque structure specific to opcode.
*/

/*
Routine:SetDeviceFlag

VOID SetDeviceFlag(
    PVOID BusContext,
    GUID *DeviceFlagGuid,
    PVOID ValueData,
    ULONG ValueLength
    );

Routine Description:

Service exported for use by the hub driver.

This routine is used by the hub driver to notify the usbport about any device
specific flags for this device.

Device flags are defined for specific vid/pid/rev from the device descriptor.

The hub driver calls this routine for each flag indicated.

IRQL = ANY

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceFlagGuid - Guid that identifies a particular device flag

ValueData - ptr to binary data
ValueLength - length of binary data
*/


/*
Routine:AcquireBusSemaphore

VOID AcquireBusSemaphore(
    PVOID BusContext
    );

Routine Description:

Service exported for use by the hub driver

Acquires or releases the global bus semaphore.  This semaphore is used by the
hub to serialize parts of enumeration.  The semaphore is held by the hub as
long as a device is at address zero (0).

IRQL = PASSIVE

*/


/*
Routine:ReleaseBusSemaphore

VOID ReleaseBusSemaphore(
    PVOID BusContext
    );

Routine Description:

Service exported for use by the hub driver

see AcquireBusSemaphore

IRQL = PASSIVE

*/


/*
Routine:SetBusSystemWakeMode

NTSTATUS SetBusSystemWakeMode(
    PVOID BusContext,
    ULONG ArmMode
    );

Routine Description:

Service exported for use by the hub driver

This routine will enable resume signaling on the USB to wake the system.  The
hub driver calls this function if one or more USB devices are 'enabled' to wake
the system.

IRQL = ANY

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

ArmForWake - 1 if we want to arm, 0 to disarm

*/


/*
Routine:InitializeUsbDeviceEx

NTSTATUS InitializeUsbDeviceEx(
    IN PVOID BusContext,
    IN OUT PUSB_DEVICE_HANDLE DeviceHandle
    IN PUSB_ID_ERROR_INFORMATION IdErrInfo
    );

Routine Description:

Service exported for use by the hub driver

This function finalizes the initialization of the DeviceHandle returned from
CreateUsbDevice and assigns the device a USB address.

IRQL = PASSIVE_LEVEL

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle -The handle of the new device on the hub returned from
CreateUsbDevice.  The value returned here is passed in the DeviceHandle field
of all URBs passed to the USB port driver and references this device.


For InitializeUsbDeviceEx Parameters and behavior are the same as
InitializeUsbDevice with one additional parameter that returns extended error
information. This information can be used to diagnose the precise cause of
the enumeration failure.

<pre>
typedef struct _USB_ID_ERROR_INFORMATION {

IN     ULONG Version;
OUT    USBPORT_INITDEV_ERROR PathError;
OUT    NTSTATUS NtStatus
OUT    UCHAR XtraInfo[64];

} USB_ID_ERROR_INFORMATION;

</pre>

Version - The callers expected version of this structure for backward compatibility.
This parameter is filled in by the caller.

PathError - A unique identifier indicated the code path where the error occurred.

NtStatus - NTSTATUS code of the Failure.

XtraInfo - Up to 64 bytes of additional data, this data is specific to the PathError it
may contain such things as the descriptor data returned by the device etc.

*/

__inline
NTSTATUS
Usbh_HubInitializeDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_ERROR_INFORMATION IdErrorInfo
    )
/*++

Routine Description:

   This function initializes a device (DeviceHandle) created by a call to
   UsbhCreateDevice.

   This function assigns the address to the USB device

Arguments:

    DeviceData - the 'device handle' for this device

Return Value:

    NT status code.

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_HUB_BUS_INTERFACE bus;
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = PdoExt(Pdo);
    hubExt = FdoExt(HubFdo);
    bus = &hubExt->HubBusInterface;

    if (!bus->InitializeUsbDevice) {
        UsbhAssertMsg(HubFdo, "InitializeUsbDevice function should not be NULL", FALSE);
        return STATUS_NOT_IMPLEMENTED;
    } else {
        nts = bus->InitializeUsbDevice(bus->BusContext,
                                       pdoExt->DeviceHandle,
                                       IdErrorInfo);
    }

    return nts;
}



VOID
__inline
Usbh_MiniDumpInterfaceDereference(
    PDEVICE_OBJECT HubFdo
    )
{
    PUSB_BUS_INTERFACE_HUB_MINIDUMP bus;

    bus = &FdoExt(HubFdo)->HubMiniDumpInterface;
    if (!bus->InterfaceDereference) {
        UsbhAssert(HubFdo, bus->InterfaceDereference!=NULL);
    } else {
        bus->InterfaceDereference(bus->BusContext);
    }
}

VOID
__inline
Usbh_MiniDumpInterfaceReference(
    PDEVICE_OBJECT HubFdo
    )
{
    PUSB_BUS_INTERFACE_HUB_MINIDUMP bus;

    bus = &FdoExt(HubFdo)->HubMiniDumpInterface;
    if (!bus->InterfaceReference) {
        UsbhAssert(HubFdo, bus->InterfaceReference!=NULL);
    } else {
        bus->InterfaceReference(bus->BusContext);
    }
}



VOID
__inline
Usbh_HubInterfaceDereference(
    PDEVICE_OBJECT HubFdo
    )
{
    PUSB_HUB_BUS_INTERFACE bus;

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->InterfaceDereference) {
        UsbhAssert(HubFdo, bus->InterfaceDereference!=NULL);
    } else {
        bus->InterfaceDereference(bus->BusContext);
    }
}

VOID
__inline
Usbh_HubInterfaceReference(
    PDEVICE_OBJECT HubFdo
    )
{
    PUSB_HUB_BUS_INTERFACE bus;

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->InterfaceReference) {
        UsbhAssert(HubFdo, bus->InterfaceReference!=NULL);
    } else {
        bus->InterfaceReference(bus->BusContext);
    }
}


__inline
Usbh_SsInterfaceDereference(
    PDEVICE_OBJECT HubFdo
    )
{
    PUSB_BUS_INTERFACE_HUB_SELECTIVE_SUSPEND bus;

    bus = &FdoExt(HubFdo)->HubSSInterface;
    if (!bus->InterfaceDereference) {
        UsbhAssert(HubFdo, bus->InterfaceDereference!=NULL);
    } else {
        bus->InterfaceDereference(bus->BusContext);
    }
}

/*
Routine:RootHubInitNotification


NTSTATUS RootHubInitNotification(
    IN PVOID BusContext,
    IN PVOID CallbackContext,
    IN PRH_INIT_CALLBACK CallbackFunction
    );

Routine Description:

Service exported for use by the hub driver

Allows the hub driver (when it is loaded for the root hub) to synchronize
enumeration with multiple USB 2.0 companion controllers.

The port driver calls the notification function when it is safe to access the
port status registers (i.e. enumerate devices).  The hub driver must complete
PNP.START_DEVICE but not access the port registers until the callback is
invoked.

The purpose of this function is to delay enumeration on USB companion controllers
until the associated USB 2.0 controller starts.  This minimizes the number of
times the device will move from one bus to the other during boot.

IRQL = ANY

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

CallBackContext - A hub driver specific context value to be passed to the
notification callback function.

CallbackFunction - Function to call when it is OK to enumerate devices.  The
callback is called at raised IRQL (DISPATCH_LEVEL).

<pre>
typedef VOID
    (__stdcall *PRH_INIT_CALLBACK) (
        IN PVOID
        );
</pre>
*/

NTSTATUS
__inline
Usbh_HubRootHubInitNotification(
    PDEVICE_OBJECT HubFdo,
    PVOID Context,
    PRH_INIT_CALLBACK RootHubInitCallback
    )
{
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->RootHubInitNotification) {
        nts =  STATUS_NOT_IMPLEMENTED;
        UsbhAssert(HubFdo, bus->RootHubInitNotification!=NULL);
    } else {
        nts = bus->RootHubInitNotification(bus->BusContext, Context, RootHubInitCallback);
    }

    return nts;
}


/*
Routine:GetUsbDescriptors

NTSTATUS GetUsbDescriptors(
    IN PVOID BusContext,
    IN OUT PUSB_DEVICE_HANDLE DeviceHandle,
    IN OUT PUCHAR DeviceDescriptorBuffer,
    IN OUT PULONG DeviceDescriptorBufferLength,
    IN OUT PUCHAR ConfigDescriptorBuffer,
    IN OUT PULONG ConfigDescriptorBufferLength,
    );

Routine Description:

Service exported for use by the hub driver

Called by the hub driver to retrieve the device and configuration descriptors
for a device.  This service may return cached copies of these descriptors
decreasing the amount of traffic to the device.

IRQL = PASSIVE_LEVEL

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - Handle for the device you want information about. This is the
handle returned by CreateUsbDevice or returned in IOCTL api
IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE.

DeviceDescriptorBuffer - caller allocated buffer to return the device
descriptor in.
DeviceDescriptorBufferLength - pointer, input is the size of the
DeviceDescriptorBuffer in bytes. Output is the number of bytes copied to the
buffer.

ConfigDescriptorBuffer - caller allocated buffer to return the configuration
descriptor in.
ConfigDescriptorBufferLength - pointer, input is the size of the
ConfigDescriptorBuffer in bytes. Output is the number of bytes copied to the
buffer.

*/

NTSTATUS
__inline
Usbh_HubGetUsbDescriptors(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE DeviceHandle,
    PUCHAR DeviceDescriptorBuffer,
    PULONG DeviceDescriptorBufferLength,
    PUCHAR ConfigDescriptorBuffer,
    PULONG ConfigDescriptorBufferLength
    )
{
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->GetUsbDescriptors) {
        nts =  STATUS_NOT_IMPLEMENTED;
        UsbhAssert(HubFdo, bus->GetUsbDescriptors!=NULL);
    } else {
        nts = bus->GetUsbDescriptors(bus->BusContext,
                                     DeviceHandle,
                                     DeviceDescriptorBuffer,
                                     DeviceDescriptorBufferLength,
                                     ConfigDescriptorBuffer,
                                     ConfigDescriptorBufferLength);
    }

    return nts;
}

NTSTATUS
__inline
Usbh_HubRestoreDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_DEVICE_HANDLE OldDeviceHandle,
    PUSB_DEVICE_HANDLE NewDeviceHandle
    )
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;

    pdoExt = PdoExt(Pdo);

    if (pdoExt->DeviceFlags.RequestConfigDescOnReset) {

        ULONG deviceDescriptorLength;
        ULONG configDescriptorLength;

        struct _DEVICE_CONFIG_DESC {
            USB_DEVICE_DESCRIPTOR DeviceDescriptor;
            USB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor;
        } *deviceConfigDesc;

        UsbhAllocatePool_Z(deviceConfigDesc, NonPagedPool,
                           sizeof(struct _DEVICE_CONFIG_DESC));

        if (deviceConfigDesc != NULL) {

            deviceDescriptorLength = sizeof(deviceConfigDesc->DeviceDescriptor);

            // For at least one device (the Gyration devices) the Get
            // Configuration Descriptor request must be either the size
            // of USB_CONFIGURATION_DESCRIPTOR (0x0009) or the exact
            // size of the complete Configuration Descriptor (including
            // Interface and Endpoint Descriptors).  A wLength value of
            // 0x00FF will *not* return the device to normal operating
            // mode.
            //
            configDescriptorLength = sizeof(deviceConfigDesc->ConfigurationDescriptor);

            Usbh_HubGetUsbDescriptors(HubFdo,
                                      NewDeviceHandle,
                                      (PUCHAR)&deviceConfigDesc->DeviceDescriptor,
                                      &deviceDescriptorLength,
                                      (PUCHAR)&deviceConfigDesc->ConfigurationDescriptor,
                                      &configDescriptorLength);

            UsbhFreePool(deviceConfigDesc);
        }
    }

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->RestoreUsbDevice) {
        nts =  STATUS_NOT_IMPLEMENTED;
        UsbhAssert(HubFdo, bus->RestoreUsbDevice!=NULL);
    } else {
        nts = bus->RestoreUsbDevice(bus->BusContext, OldDeviceHandle, NewDeviceHandle);
    }

    return nts;
}


VOID
__inline
Usbh_HubRemoveUsbDevice(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE DeviceHandle,
    ULONG Flags
    )
{
    PUSB_HUB_BUS_INTERFACE bus;

    UsbhAssert(HubFdo, DeviceHandle != NULL);
    UsbhAssert(HubFdo, DeviceHandle != UsbhInvalidDeviceHandle);

//    if (DeviceHandle == UsbhInvalidDeviceHandle || DeviceHandle == NULL) {
//        return;
//    }

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->RemoveUsbDevice) {
        UsbhAssert(HubFdo, bus->RemoveUsbDevice!=NULL);
    } else {
        bus->RemoveUsbDevice(bus->BusContext, DeviceHandle, Flags);
    }

    return;
}

/*
Routine:GetControllerInformation

NTSTATUS GetControllerInformation(
    IN PVOID BusContext,
    IN OUT PVOID ControllerInformationBuffer,
    IN ULONG ControllerInformationBufferLength,
    IN OUT PULONG LengthOfDataReturned
    );

Routine Description:

Service exported for use by the hub driver.  This api returns
various information about the USB devices attached to the system

IRQL = PASSIVE_LEVEL

Arguments:

BusHandle - Handle to the bus we need to create the device on
            this is returned to the hub driver when it requests
            the interface.

ControllerInformationBuffer (input) - info level requested must be at least the
size of the structure below.

<pre>
typedef struct _USB_CONTROLLER_INFORMATION {
    ULONG InformationLevel;
    ULONG ActualLength;
} USB_CONTROLLER_INFORMATION, *PUSB_CONTROLLER_INFORMATION;
</pre>

ControllerInformationBuffer (output) - info level requested.

ControllerInformationBufferLength -  length of callers buffer.

LengthOfDataReturned -  filled in with the number of bytes used in the callers
buffer.

ControllerInformationBuffer Parameters:

InformationLevel - input information level requested.
ActualLength - actual length needed to return the level requested.

Level 0:

SelectiveSuspendEnabled - true if selective suspend is enabled for this
controller.
IsHighSpeedController - true if this is a USB 2.0 High speed Host controller.

Level 1:

HcBusNumber,HcBusDevice,HcBusFunction - These fields identify the pci bus,
device and function number of the USB host controller the hub is attached.

*/

NTSTATUS
__inline
Usbh_HubGetControllerInformation(
    PDEVICE_OBJECT HubFdo,
    PVOID Buffer,
    ULONG BufferLength,
    PULONG OutputLength
    )
{
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->GetControllerInformation) {
        nts =  STATUS_NOT_IMPLEMENTED;
        UsbhAssert(HubFdo, bus->GetControllerInformation!=NULL);
    } else {
        nts = bus->GetControllerInformation(bus->BusContext, Buffer, BufferLength,
                OutputLength);
    }

    return nts;
}


typedef enum _SS_CONTROL {
    DisableSelectiveSuspend = 0,
    EnableSelectiveSuspend
} SS_CONTROL;

/*
Routine:ControllerSelectiveSuspend

NTSTATUS ControllerSelectiveSuspend(
    IN PVOID BusContext,
    IN BOOLEAN Enable
    );

Routine Description:

Service exported for use by the hub driver.  This api enables or
disables a selective suspend for the controller

IRQL = PASSIVE_LEVEL

Arguments:

BusHandle - Handle to the bus we need to create the device on
            this is returned to the hub driver when it requests
            the interface.

Enable  - TRUE enables selective suspend, false disables it.

*/

NTSTATUS
__inline
Usbh_HubControllerSelectiveSuspend(
    PDEVICE_OBJECT HubFdo,
    SS_CONTROL SScontrolFlag
    )
{
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->ControllerSelectiveSuspend) {
        nts =  STATUS_NOT_IMPLEMENTED;
        UsbhAssert(HubFdo, bus->ControllerSelectiveSuspend!=NULL);
    } else {
        nts = bus->ControllerSelectiveSuspend(bus->BusContext, (BOOLEAN)SScontrolFlag);
    }

    return nts;
}

/*
Routine:HubIsRoot

BOOLEAN HubIsRoot(
    PVOID BusContext,
    PVOID PhysicalDeviceObject
    );

Routine Description:

Service exported for use by the hub driver

This routine returns true if the PhysicalDeviceObject passed in is for the
root hub on the controller.

IRQL = ANY
*/

NTSTATUS
__inline
Usbh_HubIsRoot(
    PDEVICE_OBJECT HubFdo,
    PBOOLEAN IsRoot
    )
{
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;

    *IsRoot = FALSE;

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->HubIsRoot) {
        nts =  STATUS_NOT_IMPLEMENTED;
        UsbhAssert(HubFdo, bus->HubIsRoot!=NULL);
    } else {
        *IsRoot = bus->HubIsRoot(bus->BusContext, FdoExt(HubFdo)->PhysicalDeviceObject);
        nts = STATUS_SUCCESS;
    }

    return nts;
}

/*
Routine:QueryDeviceInformation

NTSTATUS QueryDeviceInformation(
    IN PVOID BusContext,
    IN PUSB_DEVICE_HANDLE DeviceHandle,
    IN OUT PVOID DeviceInformationBuffer,
    IN ULONG DeviceInformationBufferLength,
    IN OUT PULONG LengthOfDataReturned,
    );

Routine Description:

Service exported for use by the hub driver.  This api returns various
information about a USB devices attached to the system.

IRQL = PASSIVE_LEVEL

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - Handle for the device you want information about. This is the
handle returned by CreateUsbDevice or returned in IOCTL api
IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE.

DeviceInformationBuffer - buffer for returning device information.  The type
of data returned is defined by the value passed in the InformationLevel field
of the buffer passed in.
Levels Supported:
_ USB_DEVICE_INFORMATION_0, level = 0

DeviceInformationBufferLength - length of DeviceInformationBuffer in bytes

LengthOfDataReturned - length of data copied into DeviceInformationBuffer.
*/

NTSTATUS
__inline
Usbh_HubQueryDeviceInformation(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE DeviceHandle,
    PVOID DeviceInformationBuffer,
    ULONG DeviceInformationBufferLength,
    PULONG LengthOfDataReturned
    )
{
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->QueryDeviceInformation) {
        nts =  STATUS_NOT_IMPLEMENTED;
        UsbhAssert(HubFdo, bus->QueryDeviceInformation!=NULL);
    } else {
        nts = bus->QueryDeviceInformation(bus->BusContext,
                                          DeviceHandle,
                                          DeviceInformationBuffer,
                                          DeviceInformationBufferLength,
                                          LengthOfDataReturned);
    }

    return nts;
}

/*
Routine:Initialize20Hub

NTSTATUS Initialize20Hub(
    IN PVOID BusContext,
    IN PUSB_DEVICE_HANDLE HubDeviceHandle,
    IN ULONG TtCount
    );

Routine Description:

Initialize internal USBPORT structures for a USB 2.0 hub, called during the hub start process.  This routine
sets up the budgeter code for the TTs.

IRQL = ANY

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the value returned in the call to 
QUERY_INTERFACE and is opaque to the caller.

HubDeviceHandle - device handle for this hub, refernced by the hub.

TtCount - Transaction translator count, this is either 1 or equal to the number of downstream ports depending on 
what the hub reported in its configuration descriptor.

Returns:

STATUS_SUCCESS if the TTs can be initialized and linked to the hub.
    
*/
NTSTATUS
__inline
Usbh_HubInitialize20Hub(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE HubDeviceHandle,
    ULONG NumberOfTTs
    )
{
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    if (NumberOfTTs != 1) {
        // we only support one tt/port multi tt hubs
        UsbhAssert(HubFdo, NumberOfTTs == N_PORTS(HubFdo));
    }

    bus = &FdoExt(HubFdo)->HubBusInterface;

    if (hubExt->ErrataFlags.ResetTTOnCancel) {
        if (bus->SetDeviceErrataFlag) {
            bus->SetDeviceErrataFlag(bus->BusContext,
                               HubDeviceHandle,
                               ERRATA_FLAG_RESET_TT_ON_CANCEL);
        }
    }

    if (hubExt->ErrataFlags.NoClearTTBufferOnCancel) {
        if (bus->SetDeviceErrataFlag) {
            bus->SetDeviceErrataFlag(bus->BusContext,
                               HubDeviceHandle,
                               ERRATA_FLAG_NO_CLEAR_TT_BUFFER_ON_CANCEL);
        }
    }

    if (!bus->Initialize20Hub) {
        nts =  STATUS_NOT_IMPLEMENTED;
        UsbhAssert(HubFdo, bus->Initialize20Hub!=NULL);
    } else {
        nts = bus->Initialize20Hub(bus->BusContext,
                                   HubDeviceHandle,
                                   NumberOfTTs);
    }

    return nts;
}

/*
Routine:FlushTransfers

VOID FlushTransfers(
    PVOID BusContext,
    PVOID DeviceHandle
    );

Routine Description:

Service exported for use by the hub driver

This routine flushes any outstanding transfers associated with a bus and
device handle.  The service is used in conjunction with IRP_MN_REMOVE_DEVICE
for a device PDO to ensure that all outstanding transfers have been completed
when a device is removed.

IRQL = ANY

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - Opaque structure returned from CreateUsbDevice.


*/

VOID
__inline
Usbh_HubFlushTransfers(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE DeviceHandle
    )
{
    PUSB_HUB_BUS_INTERFACE bus;

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->FlushTransfers) {
        UsbhAssert(HubFdo, bus->FlushTransfers!=NULL);
    } else {
        bus->FlushTransfers(bus->BusContext,
                            DeviceHandle);
    }

    return;
}

/*
Routine:GetDeviceAddress

NTSTATUS GetDeviceAddress(
    IN PVOID BusContext,
    IN PUSB_DEVICE_HANDLE DeviceHandle,
    IN OUT PUSHORT DeviceAddress
    );

Routine Description:

Service exported for use by the hub driver.  This API returns the currently
assigned usb device address for use with TT APIs that require the device
addresssed in the setup packet.

IRQL = PASSIVE_LEVEL

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the value returned in the call to 
QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - device handle returned in a previous call to CreateDevice

DeviceAddress - pointer to ushort filled in with the currently assigned usb address.

*/
NTSTATUS
__inline
Usbh_HubGetDeviceAddress(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE DeviceHandle,
    PUSHORT DeviceAddress
    )
{
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->GetDeviceAddress) {
        nts =  STATUS_NOT_IMPLEMENTED;
        UsbhAssert(HubFdo, bus->GetDeviceAddress!=NULL);
    } else {
        nts = bus->GetDeviceAddress(bus->BusContext,
                                     DeviceHandle,
                                     DeviceAddress);
    }

    return nts;
}

/*
Routine:RefDeviceHandle

NTSTATUS RefDeviceHandle(
    IN PVOID BusContext,
    IN OUT PUSB_DEVICE_HANDLE DeviceHandle,
    IN PVOID Object,
    IN ULONG Tag
    );

Routine Description:

This function references a device handle created with CreateUsbDevice with the object passed in. The hub driver uses  
this function to add a reference to any device handles it returns in IOCTL Apis.  This prevents the device handle from
being deleted by usbport while another driver has it.  Device handles are generally not used by function drivers but 
they may be used by other instances of the hub driver or other services that interface with the USBPORT driver.

IRQL = ANY

Arguments:

BusContext - Handle to the bus the device is on. This is the value returned in the call to QUERY_INTERFACE and is 
opaque to the caller.

DeviceHandle - This is the handle returned by CreateUsbDevice api or an IOCTL.

Object - this is a pointer to a context object for this reference -- it must be unique in combination with the tag.

Tag (optional) - Tag assocaited with reference call.

Returns:
    STATUS_SUCCESS if the handle can be refernced.  This api typically fails if the handle is being removed.

*/
NTSTATUS
__inline
Usbh_HubRefDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE DeviceHandle,
    PVOID Object,
    ULONG Tag
    )
{
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->RefDeviceHandle) {
        nts =  STATUS_NOT_IMPLEMENTED;
        UsbhAssert(HubFdo, bus->RefDeviceHandle!=NULL);
    } else {
        nts = bus->RefDeviceHandle(bus->BusContext,
                                   DeviceHandle,
                                   Object,
                                   Tag);
    }

    return nts;
}

/*
Routine:DerefDeviceHandle

VOID DerefDeviceHandle(
    IN PVOID BusContext,
    IN OUT PUSB_DEVICE_HANDLE DeviceHandle,
    IN PVOID Object,
    IN ULONG Tag
    );

Routine Description:

This function releases a device handle refrence previously acquired with RefDeviceHandle.

IRQL = ANY

Arguments:

BusContext - Handle to the bus the device is on. This is the value returned in the call to QUERY_INTERFACE and is 
opaque to the caller.

DeviceHandle - This is the handle returned by CreateUsbDevice api.

Object - this is a pointer to a context object for this reference -- it must be
    unique.

Tag (optional) - Tag assocaited with original RefDeviceHandle call.

*/
VOID
__inline
Usbh_HubDerefDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE DeviceHandle,
    PVOID Object,
    ULONG Tag
    )
{
    PUSB_HUB_BUS_INTERFACE bus;

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->DerefDeviceHandle) {
        UsbhAssert(HubFdo, bus->DerefDeviceHandle!=NULL);
    } else {
        bus->DerefDeviceHandle(bus->BusContext,
                                   DeviceHandle,
                                   Object,
                                   Tag);
    }

    return;
}

/*
Routine:WaitAsyncPowerUp

NTSTATUS WaitAsyncPowerUp(
    IN PVOID BusContext
    );

Routine Description:

Service exported for use by the hub driver.  This api is used for host
controller synchronization on power up in S1-S5.

IRQL = PASSIVE_LEVEL

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the value returned in the call to 
QUERY_INTERFACE and is opaque to the caller.
*/

NTSTATUS
__inline
Usbh_HubWaitAsyncPowerUp(
    PDEVICE_OBJECT HubFdo
    )
{
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;

    if (FdoExt(HubFdo)->FdoFlags.HubBusIfReferenced == 0) {
        return NT_DEVICE_DISCONNECTED_STATUS;
    }

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->WaitAsyncPowerUp) {
        nts =  STATUS_NOT_IMPLEMENTED;
        UsbhAssert(HubFdo, bus->WaitAsyncPowerUp!=NULL);
    } else {
        nts = bus->WaitAsyncPowerUp(bus->BusContext);
    }

    return nts;
}

/*

Routine:GetDeviceBusContext

PVOID GetDeviceBusContext(
    IN PVOID BusContext,
    IN PVOID DeviceHandle
    );

Routine Description:

Service exported for use by the hub driver.  This api returns various
information about a USB devices attached to the system.
This API returns the device relative buscontext for the USBD bus interface.
This is the opaque value associated with the USBD bus interface for a
particular device handle.  This function does the exact same thing as calling
QueryBusInterface for the USBD bus interface and passing the devices handle
in InterfaceSpecific data.

IRQL = PASSIVE_LEVEL

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - Handle for the device you want information about. This is the
handle returned by CreateUsbDevice or returned in IOCTL api
IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE.

*/

NTSTATUS
__inline
Usbh_HubGetDeviceBusContext(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE DeviceHandle,
    PVOID *DeviceBusContext
    )
{
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->GetDeviceBusContext) {
        nts =  STATUS_NOT_IMPLEMENTED;
        UsbhAssert(HubFdo, bus->GetDeviceBusContext!=NULL);
    } else {
        *DeviceBusContext = bus->GetDeviceBusContext(bus->BusContext,
                                                     DeviceHandle);
        nts = STATUS_SUCCESS;
    }

    return nts;
}

/*
Routine:CalculatePipeBandwidth

ULONG CalculatePipeBandwidth(
    PVOID BusContext,
    PVOID DeviceHandle,
    PUSBD_PIPE_INFORMATION PipeInformation,
    USB_DEVICE_SPEED Speed
    );

Routine Description:

Service exported for use by the hub driver

This routine is used to calculate bandwidth consumed by a pipe with the given
characteristics set in the PipeInformation structure.  The bandwidth value
returned is in bits/second.


IRQL = ANY

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

PipeInformation - pointer to USBD_PIPE_INFORMATION structure filled in with
the attributes of a pipe.

Speed - speed of the device high, full or low

Returns:

The bandwidth consumed in bits/second.
*/    

ULONG
__inline
Usbh_HubCaculatePipeBandwidth(
    PDEVICE_OBJECT HubFdo,
    PUSBD_PIPE_INFORMATION PipeInformation,
    USB_DEVICE_SPEED DeviceSpeed
    )
{
    PUSB_HUB_BUS_INTERFACE bus;
    ULONG bw = 0;

    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->CaculatePipeBandwidth) {
        UsbhAssert(HubFdo, bus->CaculatePipeBandwidth!=NULL);
    } else {
        bw = bus->CaculatePipeBandwidth(bus->BusContext,
                                        PipeInformation,
                                        DeviceSpeed);
    }

    return bw;
}

/*
Routine:SetDeviceHandleData

VOID SetDeviceHandleData(
    IN PVOID BusContext,
    IN PVOID DeviceHandle,
    IN PDEVICE_OBJECT UsbDevicePdo
    );

Routine Description:

Associates a particular PDO with a device handle for use in debugging.  The PDO should not be referenced beyond 
this call. Any information must be extracted before returning from this function.  The address of the PDO may be 
stored for debugging but is not guaranteed to be valid.

IRQL = PASSIVE

Arguments:

BusContext - Handle to the bus the device is on. This is the value returned in the call to QUERY_INTERFACE and is 
opaque to the caller.

DeviceHandle - Handle for the device you want to set information about. This is the handle returned by 
CreateUsbDevice or returned in IOCTL api IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE.

UsbDevicePdo - device object (PDO) created by the hub driver.


*/
NTSTATUS
__inline
Usbh_HubSetDeviceHandleData(
    PDEVICE_OBJECT HubFdo,
    PVOID DeviceHandle,
    PDEVICE_OBJECT UsbDevicePdo
    )
{
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;
    
    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->SetDeviceHandleData) {
        UsbhAssert(HubFdo, bus->SetDeviceHandleData!=NULL);
        nts =  STATUS_NOT_IMPLEMENTED;
    } else {
        bus->SetDeviceHandleData(bus->BusContext,
                                 DeviceHandle,
                                 UsbDevicePdo);
        nts = STATUS_SUCCESS;
    }

    return nts;
}

/*
Routine:SetDeviceHandleIdleReadyState

ULONG SetDeviceHandleIdleReadyState(
    IN PVOID BusContext,
    IN PVOID DeviceHandle,
    IN ULONG IdleReadyFlag
    );

Routine Description:

Sets the value of the 'IdleReady' flag for a device handle.  A non-zero value will set the device handle to IDLE_OK 
modeindicating that the port the device is connected to may be suspended at any time.  The USBPORT driver may attempt 
to globally suspend the bus when all device handles indicate a IDLE_OK status.

IRQL = ANY

Arguments:

BusContext - Handle to the bus we need the device is on. This is the value returned in the call to QUERY_INTERFACE 
and is opaque to the caller.

DeviceHandle - Handle for the device you want information about. This is the handle returned by CreateUsbDevice or 
returned in IOCTL api IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE.

IdleReadyFlag - value for the IdleReady flag USB_IDLE_READY, USB_IDLE_NOT_READY.

Returns:

The previous Value of the IdleReady Flag.

*/
NTSTATUS
__inline
Usbh_HubSetDeviceHandleIdleReadyState(
    PDEVICE_OBJECT HubFdo,
    PVOID DeviceHandle,
    ULONG NewValue,
    PULONG OldValue
    )
{
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;
    
    bus = &FdoExt(HubFdo)->HubBusInterface;
    if (!bus->SetDeviceHandleIdleReadyState) {
        UsbhAssert(HubFdo, bus->SetDeviceHandleIdleReadyState!=NULL);
        nts =  STATUS_NOT_IMPLEMENTED;
    } else {
        *OldValue = bus->SetDeviceHandleIdleReadyState(bus->BusContext,
                                                       DeviceHandle,
                                                       NewValue);
        nts = STATUS_SUCCESS;
    }

    return nts;
}

// USB Host Controller Types returned by QueryControllerType
#define USB_HC_TYPE_UHCI 0x00
#define USB_HC_TYPE_OHCI 0x10
#define USB_HC_TYPE_EHCI 0x20

VOID
__inline
Usbh_UsbdQueryContollerType(
    PDEVICE_OBJECT HubFdo
    )
{
    PUSB_BUS_INTERFACE bus;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    bus = &hubExt->UsbdBusInterface;

    if (hubExt->UsbdBusifVersion != USB_BUSIF_USBDI_VERSION_3) {
        return;
    }        
    
    if (!bus->QueryControllerType) {
        return;
    } else {
        bus->QueryControllerType(bus->BusContext,
                                 &hubExt->HcHcdiOptionFlags,
                                 &hubExt->HcPciVendorId,
                                 &hubExt->HcPciDeviceId,
                                 &hubExt->HcPciClass,
                                 &hubExt->HcPciSubClass,
                                 &hubExt->HcPciRevisionId,
                                 &hubExt->HcPciProgIf);
    }

    return;
}


ULONG
__inline
Usbh_UsbdReadFrameCounter(
    PDEVICE_OBJECT HubFdo
    )
{
    PUSB_BUS_INTERFACE bus;
    ULONG frame;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);
    
    bus = &hubExt->UsbdBusInterface;

    if (!bus->QueryBusTime) {
        return 0;
    } else {
        bus->QueryBusTime(bus->BusContext,
                          &frame);
    }

    return frame;
}


VOID
__inline
Usbh_UsbdInterfaceDereference(
    PDEVICE_OBJECT HubFdo,
    PVOID BusContext
    )
{
    PUSB_BUS_INTERFACE bus;

    bus = &FdoExt(HubFdo)->UsbdBusInterface;
    if (!bus->InterfaceDereference) {
        UsbhAssert(HubFdo, bus->InterfaceDereference!=NULL);
    } else {
        bus->InterfaceDereference(BusContext);
    }
}

VOID
__inline
Usbh_UsbdInterfaceReference(
    PDEVICE_OBJECT HubFdo,
    PVOID BusContext
    )
{
    PUSB_BUS_INTERFACE bus;

    bus = &FdoExt(HubFdo)->UsbdBusInterface;
    if (!bus->InterfaceReference) {
        UsbhAssert(HubFdo, bus->InterfaceReference!=NULL);
    } else {
        bus->InterfaceReference(BusContext);
    }
}

VOID
__inline
Usbh_ForwardProgressInterfaceDereference(
    PDEVICE_OBJECT HubFdo
    )
{
    PUSB_BUS_INTERFACE_HUB_FORWARD_PROGRESS bus;

    bus = &FdoExt(HubFdo)->HubFPInterface;
    if (!bus->InterfaceDereference) {
        UsbhAssert(HubFdo, bus->InterfaceDereference!=NULL);
    } else {
        bus->InterfaceDereference(bus->BusContext);
    }
}

VOID
__inline
Usbh_ForwardProgressInterfaceReference(
    PDEVICE_OBJECT HubFdo
    )
{
    PUSB_BUS_INTERFACE_HUB_FORWARD_PROGRESS bus;

    bus = &FdoExt(HubFdo)->HubFPInterface;
    if (!bus->InterfaceReference) {
        UsbhAssert(HubFdo, bus->InterfaceReference!=NULL);
    } else {
        bus->InterfaceReference(bus->BusContext);
    }
}


NTSTATUS
__inline
Usbh_HubDeferIrpProcessing(
    __in PDEVICE_OBJECT HubFdo,
    __in PDEVICE_OBJECT DeviceObject,
    __in PUSB_IRP_WORKER_ROUTINE IrpWorkerRoutine,
    __in PIRP Irp
    )
{
    PUSB_BUS_INTERFACE_HUB_FORWARD_PROGRESS bus;
    NTSTATUS nts;

    bus = &FdoExt(HubFdo)->HubFPInterface;
    if (!bus->DeferIrpProcessing) {
        nts =  STATUS_NOT_IMPLEMENTED;
        UsbhAssert(HubFdo, bus->DeferIrpProcessing!=NULL);
    } else {
        bus->DeferIrpProcessing(DeviceObject,
                                IrpWorkerRoutine,
                                Irp);
        nts = STATUS_SUCCESS;
    }

    return nts;
}

USB_WORKITEM_HANDLE
__inline
Usbh_HubAllocateWorkItem(
    __in PDEVICE_OBJECT HubFdo,
    __in PDEVICE_OBJECT DeviceObject,
    __in BOOLEAN ForwardProgress
    )
{
    PUSB_BUS_INTERFACE_HUB_FORWARD_PROGRESS bus;

    bus = &FdoExt(HubFdo)->HubFPInterface;
    if (!bus->AllocateWorkItem) {
        UsbhAssert(HubFdo, bus->QueueWorkItem!=NULL);
        return NULL;
    } else {
        return bus->AllocateWorkItem(DeviceObject, ForwardProgress);
    }
}

VOID
__inline
Usbh_HubFreeWorkItem(
    __in PDEVICE_OBJECT HubFdo,
    __in USB_WORKITEM_HANDLE IoWorkItem
    )
{
    PUSB_BUS_INTERFACE_HUB_FORWARD_PROGRESS bus;

    bus = &FdoExt(HubFdo)->HubFPInterface;
    if (!bus->FreeWorkItem) {
        UsbhAssert(HubFdo, bus->QueueWorkItem!=NULL);
    } else {
        bus->FreeWorkItem(IoWorkItem);
    }
}

NTSTATUS
__inline
Usbh_HubQueueWorkItem(
    __in PDEVICE_OBJECT HubFdo,
    __in PDEVICE_OBJECT DeviceObject,
    __in USB_WORKITEM_HANDLE IoWorkItem,
    __in PIO_WORKITEM_ROUTINE_EX WorkerRoutine,
    __in WORK_QUEUE_TYPE QueueType,
    __in_opt PVOID Context,
    __in BOOLEAN ForwardProgress
    )
{
    PUSB_BUS_INTERFACE_HUB_FORWARD_PROGRESS bus;
    NTSTATUS nts;

    bus = &FdoExt(HubFdo)->HubFPInterface;
    if (!bus->QueueWorkItem) {
        nts =  STATUS_NOT_IMPLEMENTED;
        UsbhAssert(HubFdo, bus->QueueWorkItem!=NULL);
    } else {
        bus->QueueWorkItem(DeviceObject,
                           IoWorkItem,
                           WorkerRoutine,
                           QueueType,
                           Context,
                           ForwardProgress);
        nts = STATUS_SUCCESS;
    }

    return nts;
}
#endif //__BUSIFDEF_H__
