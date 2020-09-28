//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: reg.c */

#if DBG
#endif

NTSTATUS
UsbhQueryGlobalHubValue(
    PCWSTR ValueName,
    __in ULONG ValueType,
    __inout_bcount(ValueLength) PVOID ValueData,
    __in ULONG ValueLength,
    __inout_opt PVOID Context,
    __inout_opt PVOID EntryContext
    );

NTSTATUS
UsbhReadPdoRegistryKeys(
    __in PDEVICE_OBJECT HubFdo,
    __in PDEVICE_OBJECT Pdo
    );


NTSTATUS
UsbhRegQueryGlobalKey(
    __in ULONG RelativeTo,
    __in_bcount(CbPath) PWSTR Path,
    __in USHORT CbPath,
    __inout_bcount(CbQueryTable) PRTL_QUERY_REGISTRY_TABLE QueryTable,
    __in_range(>=, sizeof(RTL_QUERY_REGISTRY_TABLE))
    __in ULONG CbQueryTable
    );


NTSTATUS
UsbhQueryGlobalLegacyDeviceValue(
    PCWSTR ValueName,
    __in ULONG ValueType,
    __in_bcount(ValueLength) PVOID ValueData,
    __in ULONG ValueLength,
    __inout_opt PVOID Context,
    __inout_opt PVOID EntryContext
    );


NTSTATUS
UsbhRegQueryGlobalLegacyDeviceKeys(
    __in PDEVICE_OBJECT HubFdo,
    __in PDEVICE_OBJECT Pdo
    );


NTSTATUS
UsbhQueryGlobalDeviceValue(
    PCWSTR ValueName,
    __in ULONG ValueType,
    __in_bcount(ValueLength) PVOID ValueData,
    __in ULONG ValueLength,
    __inout_opt PVOID Context,
    __inout PVOID EntryContext
    );

VOID
UsbhGetRegUsbClassFlags(
    __in PDEVICE_OBJECT HubFdo,
    __in PDEVICE_OBJECT Pdo
    );

PWCHAR
UsbhGetRegUsbDeviceFlags(
    __in PDEVICE_OBJECT HubFdo,
    __in PDEVICE_OBJECT Pdo
    );

VOID
UsbhGetInitRegUsbDeviceFlags(
    __in PDEVICE_OBJECT HubFdo,
    __in PDEVICE_OBJECT Pdo
    );

VOID
UsbhGetRegUsbHubFlags(
    __in PDEVICE_EXTENSION_HUB hubExt
    );

NTSTATUS
UsbhRegCreateUsbflagsKey(
    __in PDRIVER_OBJECT DriverObject
    );

NTSTATUS
UsbhSetPdoRegistryParameter(
    __in PDEVICE_OBJECT Pdo,
    PCWSTR KeyName,
    __in ULONG Type,
    __in_bcount(DataLength) PVOID Data,
    __in ULONG DataLength,
    __in BOOLEAN HardwareBranch
    );

NTSTATUS
UsbhGetPdoRegistryParameter(
    __in PDEVICE_OBJECT Pdo,
    PCWSTR ValueName,
    __in_bcount(DataLength) PVOID Data,
    __in ULONG DataLength,
    __out_opt PULONG Type,
    __out_opt PULONG ActualDataLength,
    __in BOOLEAN HardwareBranch
    );


NTSTATUS
UsbhSetRegSymbolicLinkName(
    __in PDEVICE_OBJECT Pdo,
    __in PUNICODE_STRING SymbolicLinkName
    );

NTSTATUS
UsbhSetHubOvercurrentDetectedKey(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG PortNumber,
    __in PVOID Context
    );

NTSTATUS
UsbhSetPdoSelectiveSuspendedKey(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG PortNumber,
    __in PVOID Context); 

VOID
UsbhSetSqmEnumerationData(
    __in PDEVICE_OBJECT Pdo
    );

NTSTATUS
UsbhRegPnpStart(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT StateContext
    );


NTSTATUS
UsbhRegDriverEntry(
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING UniRegistryPath
    );

NTSTATUS
UsbhSetRegistryWakeOnConnect(
    __in PDEVICE_OBJECT HubFdo,
    __in BOOLEAN Enable
    );

NTSTATUS
UsbhSetHubSelectiveSuspend(
    __in PDEVICE_OBJECT HubFdo,
    __in BOOLEAN Enable
    );

VOID
UsbhUpdateRegHubHardResetCount(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG PortNumber,
    __in PVOID Context
    );

VOID
UsbhUpdateRegSurpriseRemovalCount(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG PortNumber,
    __in PVOID Context
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhUpdateSqmFlags (
    __in PDEVICE_OBJECT Pdo
   );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSetPdoEnumerationFailureCodeKey(
    __in PDEVICE_OBJECT Pdo
   );

VOID
UsbhGetD3Policy(
    __in PDEVICE_OBJECT Pdo
    );
