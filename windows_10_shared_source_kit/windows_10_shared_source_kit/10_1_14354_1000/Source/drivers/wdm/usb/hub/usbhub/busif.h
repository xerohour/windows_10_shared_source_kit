//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: busif.c */


NTSTATUS
UsbhGetBusInterface(
    PDEVICE_OBJECT HubFdo,
    __in_bcount(Size) PVOID BusInterface,
    CONST GUID *InterfaceType,
    USHORT Size,
    USHORT Version
    );


NTSTATUS
UsbhBusIfAddDevice(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhBusIfRemoveDevice(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


NTSTATUS
UsbhQuerySSstate(
    PDEVICE_OBJECT HubFdo,
    PBOOLEAN SelectiveSuspendEnabled
    );

VOID
UsbhBusIfSSReference(
    __inout PVOID Context
    );

VOID
UsbhBusIfSSDereference(
    __inout PVOID Context
    );

NTSTATUS
UsbhBusIf_SuspendChildHub(    
    PDEVICE_OBJECT Pdo
    );

NTSTATUS
UsbhBusIf_ResumeChildHub(
    PDEVICE_OBJECT Pdo
    );

NTSTATUS
UsbhBusifGetSSHub(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );

NTSTATUS
UsbhBusIf_GetContainerIdForPort(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    LPGUID ContainerId
    );

VOID
UsbhBusIf_SetContainerIdForPort(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    LPGUID ContainerId
    );

NTSTATUS
UsbhBusifGetLocationInterface(
    PDEVICE_OBJECT  HubFdo,
    PDEVICE_OBJECT  Pdo,
    PIRP            Irp
    );

VOID
UsbhBusIfLocationReference(
    __inout PVOID Context
    );

VOID
UsbhBusIfLocationDereference(
    __inout PVOID Context
    );

NTSTATUS
UsbhBusIfLocationGetLocationStrings(
    __inout PVOID Context,
    __out PWCHAR *LocationStrings
    );

VOID
UsbhPdoAbortAllDevicePipes(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    );

USHORT
UsbhBusIf_GetDebugPortNumber(
    PDEVICE_OBJECT HubFdo
    );

VOID
UsbhBusIf_SetMsOs20Support(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE DeviceHandle,
    PUSB_DEVICE_DESCRIPTOR DeviceDescriptor,
    UCHAR MsOS20VendorCode
    );

VOID
UsbhBusIf_SetClearRootPortResumeTime(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE DeviceHandle,
    BOOLEAN SetTime,
    UCHAR ResumeTime
    );
