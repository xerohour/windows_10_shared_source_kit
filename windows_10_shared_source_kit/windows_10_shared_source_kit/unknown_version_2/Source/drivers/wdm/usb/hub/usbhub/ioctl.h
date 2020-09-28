//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: ioctl.c */


NTSTATUS
UsbhAcquireApiLock(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG Tag,
    __inout PBOOLEAN 
    );


VOID
UsbhReleaseApiLock(
    PDEVICE_OBJECT HubFdo,
    ULONG Tag
    );


NTSTATUS
UsbhIoctlCyclePort(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );


NTSTATUS
UsbhIoctlGetHubCaps(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );


NTSTATUS
UsbhIoctlResetStuckHub(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );

NTSTATUS
UsbhIoctlGetHubCapsEx(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );


NTSTATUS
UsbhIoctlGetHubInformationEx(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );


NTSTATUS
UsbhIoctlGetPortConnectorProperties(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );


NTSTATUS
UsbhIoctlGetDescriptorForPDO(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PUSHORT DataBufferLength,
    PUCHAR DataBuffer,
    BOOLEAN ApiLockAcquired,
    BOOLEAN UseCachedDescriptor
    );


NTSTATUS
UsbhIoctlGetDescriptorFromNodeConnection(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );


NTSTATUS
UsbhIoctlGetNodeConnectionName(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );


NTSTATUS
UsbhIoctlGetNodeConnectionAttributes(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );


PUSB_DEVICE_INFORMATION_0
UsbhGetDeviceInformationEx(
    PDEVICE_OBJECT HubFdo,
    NTSTATUS *NtStatus,
    PUSB_DEVICE_HANDLE DeviceHandle
    );


NTSTATUS
UsbhGetNodeConnectionInfoForPdo(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo,
    PUSB_NODE_CONNECTION_INFORMATION NciBuffer,
    PULONG OutBufferLength,
    BOOLEAN ExApi
    );


NTSTATUS
UsbhIoctlGetNodeConnectionInfoExApi(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack,
    BOOLEAN ExApi
    );


NTSTATUS
UsbhIoctlGetNodeConnectionInfo(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );


NTSTATUS
UsbhIoctlGetNodeConnectionInfoEx(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );


NTSTATUS
UsbhIoctlGetNodeConnectionInfoExV2(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );


NTSTATUS
UsbhIoctlValidateParameters(
    PDEVICE_OBJECT HubFdo,
    ULONG UsbIoCode,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack,
    ULONG MinInputSize,
    ULONG MinOutputSize
    );


NTSTATUS
UsbhIoctlGetNodeConnectionDriverKeyName(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );


NTSTATUS
UsbhIoctlGetNodeInformation(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );

VOID
UsbhIoctlTraceOutput(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    NTSTATUS NtStatus
    );


NTSTATUS
UsbhInternalIoctlGlobalSuspend(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );


NTSTATUS
UsbhInternalIoctlPropagateResume(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PIO_STACK_LOCATION IoStack
    );


NTSTATUS
UsbhSendSynchronousUsbIoctlRequest(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    ULONG Ioctl
    );

BOOLEAN
UsbhVerifyCallerIsAdmin(
    );
