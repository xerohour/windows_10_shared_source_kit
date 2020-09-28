//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: ex.c */


__drv_aliasesMem
__drv_allocatesMem(Mem)
VOID
UsbhException(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    HUB_EXCEPTION_CLASS Class,
    PVOID ExceptionData,
    ULONG ExceptionDataLength,
    NTSTATUS NtStatus,
    USBD_STATUS UsbdStatus,
    PCSTR SzFile,
    ULONG Line,
    BOOLEAN HardwareReset
    );


VOID
UsbhExceptionTrace(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD HubExr,
    PCHUB_EXCEPTION_DISPATCH HubExDispatch
    );

PCHUB_EXCEPTION_DISPATCH
UsbhGetExceptionDispatch(
    PDEVICE_OBJECT HubFdo,
    HUB_EXCEPTION_CLASS Class
    );

VOID
UsbhExceptionWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID HubExceptionRecord
    );


VOID
UsbhDeviceOvercurrent(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    );


VOID
UsbhEnumFailurePopup(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    );


VOID
UsbhNotEnoughPowerPopup(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    );


VOID
UsbhLegacyDevicePopup(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    );


VOID
UsbhNotEnoughBandwidth_Popup(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    );


PHUB_EXCEPTION_RECORD
UsbhLogException(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    );


VOID
UsbhExAddDeviceInit(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhExRemove(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhWriteExceptionToIoErrorLog(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD HubExr
    );

VOID
UsbhQueueOvercurrentReset(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    );

VOID
UsbhQueueDriverOvercurrent(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    );

VOID
UsbhNestedHubsPopup(
    PDEVICE_OBJECT HubFdo,
    PHUB_EXCEPTION_RECORD Exr
    );

