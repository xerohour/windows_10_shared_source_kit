/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    pnpinit.h

Abstract:

    This module contains the private (internal) header file for PNP
    initialization.

Author:

    Santosh Jodh (santoshj) 1-Feb-2006


Revision History:

--*/

#pragma once

#define ALLOW_WORLD_READ_OF_ENUM        1

#ifndef MAX_ULONG
#define MAX_ULONG ((ULONG)-1)
#endif

//
// This returns true iff PNP is shutdown.
//

#define PnpIsShutDown() (KeReadStateEvent(&PnpShutdownEvent) != 0)

//
// Define the type for driver group name entries in the group list so that
// load order dependencies can be tracked.
//

typedef struct _TREE_ENTRY {
    struct _TREE_ENTRY *Left;
    struct _TREE_ENTRY *Right;
    struct _TREE_ENTRY *Sibling;
    ULONG DriversThisType;
    ULONG DriversLoaded;
    UNICODE_STRING GroupName;
} TREE_ENTRY, *PTREE_ENTRY;

typedef struct _DRIVER_INFORMATION {
    LIST_ENTRY              Link;
    PDRIVER_OBJECT          DriverObject;
    PBOOT_DRIVER_LIST_ENTRY DataTableEntry;
    HANDLE                  ServiceHandle;
    NTSTATUS                Status;
    USHORT                  TagPosition;
    BOOLEAN                 Failed;
    BOOLEAN                 Processed;
} DRIVER_INFORMATION, *PDRIVER_INFORMATION;

typedef enum _CORE_DRIVER_GROUP_TYPE {
    CoreDriverGroup,
    CoreExtensionsDriverGroup,
    TpmCoreDriverGroup,
    CoreDriverGroupTypeMaximum
} CORE_DRIVER_GROUP_TYPE, *PCORE_DRIVER_GROUP_TYPE;

typedef enum _DRIVER_DEPENDENT_DLL_TYPE {
    CoreDriverDependentDLL,
    CoreExtensionDependentDLL,
    NormalDriverDependentDLL,
    DependentDLLTypeMaximum
} DRIVER_DEPENDENT_DLL_TYPE, *PDRIVER_DEPENDENT_DLL_TYPE;

PTREE_ENTRY
PipCreateEntry(
    __in PUNICODE_STRING GroupName
    );

VOID
PipFreeGroupTree(
    __in PTREE_ENTRY TreeEntry
    );

USHORT
PipGetDriverTagPriority(
    __in HANDLE Servicehandle
    );

NTSTATUS
PipAddDevicesToBootDriver(
   __in PDRIVER_OBJECT DriverObject
   );

LOGICAL
PipAddDevicesToBootDriverWorker(
    __in HANDLE DeviceInstanceHandle,
    __in PUNICODE_STRING DeviceInstancePath,
    __inout PVOID Context
    );

LOGICAL
PipCheckDependencies(
    __in HANDLE KeyHandle
    );

VOID
PipInsertDriverList(
    __in PLIST_ENTRY ListHead,
    __in PDRIVER_INFORMATION DriverInfo
    );

PTREE_ENTRY
PipLookupGroupName(
    __in PUNICODE_STRING GroupName,
    __in LOGICAL Insert
    );

LOGICAL
PipWaitForBootDevicesDeleted(
    __in VOID
    );

BOOLEAN
PnpInitPhase0(
    VOID
    );

BOOLEAN
PnpInitPhase1(
    VOID
    );

NTSTATUS
RawInitialize(
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
    );

NTSTATUS
PiInitCacheGroupInformation(
    VOID
    );

VOID
PiInitReleaseCachedGroupInformation(
    VOID
    );

BOOLEAN
PnpDoPolicyCheck (
    __in BDCB_CLASSIFICATION Classification,
    __in BOOLEAN HonorPolicy
    );

NTSTATUS
PnpInitializeBootStartDriver (
    __in PUNICODE_STRING DriverName,
    __in PUNICODE_STRING RegistryPath,
    __in PDRIVER_INITIALIZE EntryPoint,
    __in_opt PBLDR_DATA_TABLE_ENTRY LoaderEntry,
    __in PLIST_ENTRY LoaderModuleListHead,
    __in LOGICAL IsFilter,
    __in LOGICAL InvokeNotifications,
    __in BOOLEAN HonorEarlyLaunchPolicy,
    __out PDRIVER_OBJECT *DriverObject
    );

KINLINE
LOGICAL
PnpWaitForBootDevicesDeleted (
    VOID
    )
{
    NTSTATUS status;

    status = PnpWaitForEmptyDeviceEventQueue();
    if (NT_SUCCESS(status)) {
        return TRUE;
    }

    return FALSE;
}

NTSTATUS
PiInitFirmwareResources(
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
PipInitDeviceOverrideCache(
    VOID
    );

typedef
BOOLEAN
(*PDEVICE_OVERRIDE_MATCH_CALLBACK)(
    __in     HANDLE DeviceOverrideEntryHandle,
    __in_opt PVOID  Context
    );

NTSTATUS
PipFindDeviceOverrideEntry(
    __in PCZZWSTR HardwareIds,
    __in_opt  PDEVICE_OVERRIDE_MATCH_CALLBACK DeviceOverrideMatchCallback,
    __in_opt  PVOID   DeviceOverrideMatchContext,
    __out_opt PHANDLE DeviceOverrideEntryHandle
    );

NTSTATUS
PiQueryRemovableDeviceOverride(
    __in     PDEVICE_NODE DeviceNode,
    __in_opt PCWSTR       HardwareIds,
    __in_opt PCWSTR       CompatibleIds,
    __out    PBOOLEAN     Removable
    );

BOOLEAN
PipCallbackHasDeviceOverrides(
    __in     HANDLE DeviceOverrideEntryHandle,
    __in_opt PVOID  Context
    );

NTSTATUS
PipHardwareConfigInit(
    __in LPGUID HardwareConfigGuid
    );

BOOLEAN
PipHardwareConfigExists(
    __in HANDLE hHardwareConfigRoot,
    __in ULONG  HardwareConfigIndex
    );

NTSTATUS
PipHardwareConfigActivateService(
    __in PCWSTR ServiceName
    );

NTSTATUS
PipHardwareConfigClearStartOverrides(
    __in ULONG HardwareConfigIndex
    );

NTSTATUS
PipHardwareConfigOpenKey(
    __in_opt LPGUID HardwareConfigGuid,
    __in ACCESS_MASK DesiredAccess,
    __out PHANDLE HardwareConfigKey
    );

NTSTATUS
PipHardwareConfigGetIndex(
    __in_opt LPGUID HardwareConfigGuid,
    __out PULONG HardwareConfigIndex
    );

NTSTATUS
PipHardwareConfigGetLastUseTime(
    __in_opt LPGUID HardwareConfigGuid,
    __out PLARGE_INTEGER LastUseTime
    );

__callback
__checkReturn
PNPCTX_REG_ENUM_KEY_ACTION
PipHardwareConfigClearStartOverrideCallback(
    __in HPNPCTX hPnpCtx,
    __in HANDLE ParentKey,
    __in PWSTR KeyName,
    __in_opt PVOID Context
    );

NTSTATUS
PipHardwareConfigTriggerRespecialize(
    __in_opt HANDLE SysprepHandle
    );

VOID
PipCheckSystemFirmwareUpdated(
   __out PBOOLEAN SystemFirmwareUpdated
   );

NTSTATUS
PipInitComputerIds(
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
PipCreateComputerId(
    __in HANDLE ComputerIdsKeyHandle,
    __in_ecount(StringCount) PUNICODE_STRING *StringList,
    __in ULONG StringCount,
    __out LPGUID ComputerId
    );

BOOLEAN
PipCheckComputerSupported(
    __in PCWSTR ComputerId
    );

PSMBIOS_STRUCT_HEADER
PipSmBiosFindStruct(
    __in UCHAR Type,
    __in UCHAR SubType,
    __in_ecount(SmBiosDataLength) PUCHAR SmBiosData,
    __in ULONG SmBiosDataLength
    );

NTSTATUS
PipSmBiosGetString(
    __in PSMBIOS_STRUCT_HEADER StructHeader,
    __in UCHAR StringNumber,
    __in_ecount(SmBiosDataLength) PUCHAR SmBiosData,
    __in ULONG SmBiosDataLength,
    __out PUNICODE_STRING UnicodeString
    );

NTSTATUS
PipMigratePnpState(
    VOID
    );

__callback
_Must_inspect_result_
PNPCTX_REG_ENUM_KEY_ACTION
PipMigrateServiceCallback(
    __in HPNPCTX hPnpCtx,
    __in HANDLE ServicesKeyHandle,
    __in PWSTR ServiceName,
    __in_opt PVOID Context
    );

__callback
_Must_inspect_result_
BOOLEAN
PipMigrateResetDeviceCallback(
    __in HPNPCTX hPnpCtx,
    __in PCWSTR DeviceInstanceId,
    __in CM_OBJECT_TYPE CmObjectType,
    __in_opt PVOID Context
    );

VOID
PipInitializeCoreDriversAndElam (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
PipInitializeCoreDriversByGroup (
    __in CORE_DRIVER_GROUP_TYPE GroupType,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
PipInitializeDriverDependentDLLs (
    __in DRIVER_DEPENDENT_DLL_TYPE DependentDLLType,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
PipInitializeEarlyLaunchDrivers (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
PipUnloadEarlyLaunchDrivers (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

//
// This event is signalled when PNP subsystem is informed of system shutdown.
//

extern KEVENT PnpShutdownEvent;

//
// Boot mode is active until file system and registry have been initialized.
//

extern BOOLEAN PnpBootMode;

//
// System setup is in progress.
//

extern BOOLEAN PnpSetupInProgress;
extern BOOLEAN PnpSetupPhaseInProgress;
extern BOOLEAN PnpSetupTypeInProgress;
extern BOOLEAN PnpSetupOOBEInProgress;
extern BOOLEAN PnpSetupUpgradeInProgress;
extern BOOLEAN PnpSetupRollbackActiveInProgress;
