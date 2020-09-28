//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: devioctl.c */

#ifdef HUB_WPP
#endif

NTSTATUS
UsbhFdoReturnHubCount(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoReturnRootHubPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoReturnControllerName(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoReturnDeviceBusInfo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoReturnPortStatus(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoResetPdoPort(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoCyclePdoPort(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoEnablePdoPort(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoReturnHubName(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoReturnParentHubInfo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoSubmitPdoIdleNotification(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoRecordFailure(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoReturnTopologyAddress_Complete(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    PVOID Context
    );

NTSTATUS
UsbhFdoPdoIoctlIdleReady(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );

NTSTATUS
UsbhFdoReturnDeviceConfigInfo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );

NTSTATUS
UsbhFdoReturnTopologyAddress(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );

NTSTATUS
UsbhFdoHandleGetDumpDataIoctl(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );

NTSTATUS
UsbhFdoHandleFreeDumpDataIoctl(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );

NTSTATUS
UsbhFdoHandleNotifyForwardProgress(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );

