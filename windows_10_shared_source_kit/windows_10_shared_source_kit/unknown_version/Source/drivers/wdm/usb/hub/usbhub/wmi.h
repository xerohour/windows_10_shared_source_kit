//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: wmi.c */


NTSTATUS
UsbhWmiDriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING UniRegistryPath
    );


VOID
UsbhWmiUnload(
    PDRIVER_OBJECT DriverObject
    );


NTSTATUS
UsbhWmiPnpStart(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhWmiPnpStop(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


NTSTATUS
UsbhSetWmiDataBlock(
    IN PDEVICE_OBJECT HubFdo,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG InstanceIndex,
    IN ULONG BufferSize,
    IN PUCHAR Buffer
    );


NTSTATUS
UsbhQueryWmiDataBlock(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    ULONG GuidIndex,
    ULONG InstanceIndex,
    ULONG InstanceCount,
    PULONG InstanceLengthArray,
    ULONG OutBufferSize,
    PUCHAR Buffer
    );


NTSTATUS
UsbhPdoQueryWmiDataBlock(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    ULONG GuidIndex,
    ULONG InstanceIndex,
    ULONG InstanceCount,
    PULONG InstanceLengthArray,
    ULONG OutBufferSize,
    PUCHAR Buffer
    );


NTSTATUS
UsbhExecuteWmiMethod(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    ULONG GuidIndex,
    ULONG InstanceIndex,
    ULONG MethodId,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PUCHAR Buffer
    );


NTSTATUS
UsbhQueryWmiRegInfo(
    PDEVICE_OBJECT HubFdo,
    PULONG RegFlags,
    PUNICODE_STRING InstanceName,
    PUNICODE_STRING *RegistryPath,
    PUNICODE_STRING MofResourceName,
    PDEVICE_OBJECT *Pdo
    );


NTSTATUS
UsbhPdoQueryWmiRegInfo(
    PDEVICE_OBJECT DeviceObject,
    PULONG RegFlags,
    PUNICODE_STRING InstanceName,
    PUNICODE_STRING *RegistryPath,
    PUNICODE_STRING MofResourceName,
    PDEVICE_OBJECT *Pdo
    );


NTSTATUS
UsbhWmiEnumerationFailure(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    );


NTSTATUS
UsbhWmiInsufficentBandwidth(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    );


NTSTATUS
UsbhWmiDoNothing(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    );


NTSTATUS
UsbhAcquireHubName(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    );


NTSTATUS
UsbhAcquireControllerName(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    );


NTSTATUS
UsbhWmiInsufficentPower(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    );


NTSTATUS
UsbhAcquireBusInfo(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    );


NTSTATUS
UsbhWmiResetOvercurrent(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    );


PUSB_CONNECTION_NOTIFICATION
UsbhBuildWmiConnectionNotification(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber
    );


VOID
UsbhPdoRegisterWmi(
    PDEVICE_OBJECT Pdo
    );

NTSTATUS
UsbhBuildTopologyAddress(
    PDEVICE_OBJECT HubFdo,
    PUSB_TOPOLOGY_ADDRESS Address,
    ULONG PortNumber
    );

