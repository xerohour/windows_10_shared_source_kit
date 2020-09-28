//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: idstring.c */

PWCHAR
UsbhMakeId(
    __in ULONG IdType,
    const WCHAR *IdFormatString,
    __in_bcount_opt(*AppendToStringLength) PWCHAR AppendToString,
    __in PULONG AppendToStringLength,
    __in USHORT NullCount,
    __in USHORT Digits,
    __in USHORT HexId,
    PUCHAR MsCompatId
    );


VOID
UsbhFreeID(
    PUSB_ID_STRING IdString
    );


NTSTATUS
UsbhBuildDeviceID(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    );


NTSTATUS
UsbhBuildHardwareID(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    );

NTSTATUS
UsbhGetLocationIdString(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    LANGID LanguageId,
    PUSB_ID_STRING IdString
    );

NTSTATUS
UsbhBuildDeviceCompatibleID(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    );


NTSTATUS
UsbhBuildClassCompatibleID(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    );


NTSTATUS
UsbhBuildCompatibleID(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    );

NTSTATUS
UsbhBuildContainerID(
    __in PDEVICE_OBJECT HubFdo,
    __in PDEVICE_OBJECT Pdo
    );

NTSTATUS
UsbhGetSerialNumber(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    );


BOOLEAN
UsbhLanguageSupported(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    LANGID LanguageId
    );


NTSTATUS
UsbhGetProductIdString(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    LANGID LanguageId,
    PUSB_ID_STRING IdString
    );


NTSTATUS
UsbhGetLanguageIdString(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    );


VOID
UsbhBuildUnknownIds(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    );