//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: busfunc.c */


ULONG
UsbhPortIgnoreChange(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhReset1DropDeviceTimeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhPortDisconnect(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhReset2TimeoutBadEnable(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhRequestPortSuspend(
    PDEVICE_OBJECT HubFdo,
    USB_HUB_PORT_EVENT Event,
    UCHAR PortStatusBits,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhPortConnect(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhReset1Timeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


VOID
UsbhDropDevice(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhReset1DebounceError(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhReset1Debounce(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhReset1DropDevice(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhPortWaitForResumeTimeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhHardErrorInvalidData(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhHardErrorReset1BadEnable(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhReset1TimeoutBadEnable(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhReset2DropDeviceTimeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhReset2Timeout(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhReset1Complete(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhReset2DropDevice(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhReset1EnIgnore(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhReset2Complete(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhReset1CompleteNotEnabled(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhPortCycle(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhPortRecycle(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhPortResumeComplete(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhPortDisable(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhPortOvercurrent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhDriverResetPort(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhFailDriverResetPort(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhHandleSuspend(
    PDEVICE_OBJECT HubFdo,
    USB_HUB_PORT_EVENT Event,
    UCHAR PortStatusBits,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhReset2CycleDevice(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhReset2ErrorWaitForTimeoutDevice(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhCancelEnumeration(
    PDEVICE_OBJECT HubFdo,
    ULONG State,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhHandleOvercurrent(
    PDEVICE_OBJECT HubFdo,
    ULONG State,
    UCHAR PortStatusBits,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );


ULONG
UsbhResumeSuspendedPort(
    PDEVICE_OBJECT HubFdo,
    USB_HUB_PORT_EVENT Event,
    UCHAR PortStatusBits,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhWcCancelEnumeration(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

VOID
UsbhBusUnlatchPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHUB_PORT_DATA PortData,
    PVOID Object,
    PHGSL Glock
    );

USB_CONNECTION_STATUS
UsbhSetEnumerationFailed(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo,
    HUB_EXCEPTION_CLASS ExClass,
    PENUM_FAIL_DATA EnumFailData,
    NTSTATUS NtStatus,
    USBD_STATUS UsbdStatus,
    PCSTR SzFile,
    ULONG Line
    );

ULONG
UsbhReset1CompleteOvercurrent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhPortResumeFailed(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhChangeTrap(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );