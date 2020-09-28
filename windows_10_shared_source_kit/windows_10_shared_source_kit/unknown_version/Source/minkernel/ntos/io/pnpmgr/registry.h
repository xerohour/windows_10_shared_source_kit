/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    registry.h

Abstract:

    This module contains registry functions for the PNP subsystem.

Author:

    Shie-Lin Tzong (shielint) 10/03/1995


Revision History:


--*/

#pragma once

#undef TEXT
#define TEXT(quote) L##quote

#define _IN_KERNEL_
#include <regstr.h>
#include <pnpmgr.h>

//
// kernel mode specific definitions
//
#define REGSTR_VALUE_HARDWAREID                      REGSTR_VAL_HARDWAREID
#define REGSTR_VALUE_COMPATIBLEIDS                   REGSTR_VAL_COMPATIBLEIDS
#define REGSTR_VALUE_CLASSGUID                       REGSTR_VAL_CLASSGUID
#define REGSTR_VALUE_COUNT                           REGSTR_VAL_COUNT
#define REGSTR_KEY_INSTANCE_KEY_FORMAT               TEXT("%04u")
#define REGSTR_VALUE_STANDARD_ULONG_FORMAT           TEXT("%u")
#define REGSTR_VALUE_GROUP                           TEXT("Group")
#define REGSTR_VALUE_NEXT_INSTANCE                   TEXT("NextInstance")
#define REGSTR_VALUE_CSCONFIG_FLAGS                  REGSTR_VAL_CSCONFIGFLAGS
#define REGSTR_VALUE_ROOT_CONTAINERID                TEXT("{00000000-0000-0000-FFFF-FFFFFFFFFFFF}")
#define REGSTR_VALUE_ALLOC_CONFIG                    REGSTR_VAL_ALLOCCONFIG
#define REGSTR_VALUE_FORCED_CONFIG                   REGSTR_VAL_FORCEDCONFIG
#define REGSTR_VALUE_BOOT_CONFIG                     REGSTR_VAL_BOOTCONFIG
#define REGSTR_VALUE_FILTERED_CONFIG_VECTOR          REGSTR_VAL_FILTEREDCONFIGVECTOR
#define REGSTR_VALUE_OVERRIDE_CONFIG_VECTOR          REGSTR_VAL_OVERRIDECONFIGVECTOR
#define REGSTR_VALUE_BASIC_CONFIG_VECTOR             REGSTR_VAL_BASICCONFIGVECTOR
#define REGSTR_VALUE_DEVICE_REPORTED                 REGSTR_VAL_DEVICE_REPORTED
#define REGSTR_VALUE_NO_RESOURCE_AT_INIT             TEXT("NoResourceAtInitTime")
#define PNPMGR_STR_PNP_MANAGER                       TEXT("PnP Manager")
#define PNPMGR_STR_PNP_DRIVER                        TEXT("\\Driver\\PnpManager")    // Must be the same
#define REGSTR_KEY_PNP_DRIVER                        TEXT("PnpManager")              // Must be the same
#define REGSTR_PATH_CONTROL_PNP                      TEXT("Control\\Pnp")
#define REGSTR_PATH_CONTROL_PENDING_SERVICES         TEXT("Control\\PendingDriverOperations\\Services")
#define REGSTR_PATH_CONTROL_PENDING_OS_EXTENSIONS    TEXT("Control\\PendingDriverOperations\\OsExtensionDatabase")
#define REGSTR_PATH_CONTROL_FIRMWARE_RESOURCES       TEXT("Control\\FirmwareResources")
#define REGSTR_PATH_CONTROL_OS_EXTENSIONS            TEXT("Control\\OsExtensionDatabase")
#define REGSTR_VALUE_DEPEND_ON_FIRMWARE              TEXT("DependOnFirmware")
#define REGSTR_KEY_EVENT_LOG                         TEXT("EventLog")
#define REGSTR_VALUE_PHASE                           TEXT("Phase")
#define REGSTR_VALUE_LAST_ATTEMPT_STATUS             TEXT("LastAttemptStatus")
#define REGSTR_VAL_REFERENCECOUNT                    TEXT("ReferenceCount")
#define REGSTR_VALUE_ASYNC                           TEXT("AsynchronousOptions")
#define REGSTR_VALUE_PNP_FLAGS                       TEXT("PnpFlags")
#define REGSTR_VALUE_DISABLEREBALANCE                TEXT("DisableRebalance")
#define REGSTR_VALUE_PNP_BOOT_OPTIONS                TEXT("BootOptions")
#define REGSTR_VALUE_DISABLE_LKG                     TEXT("DisableLKG")
#define REGSTR_VALUE_FIND_BEST_CONFIGURATION_TIMEOUT TEXT("FindBestConfigurationTimeout")
#define REGSTR_VALUE_DEVICE_RESET_RETRY_INTERVAL     TEXT("DeviceResetRetryInterval")
#define REGSTR_VALUE_DEVICE_RESET_MAXIMUM_RETRIES    TEXT("DeviceResetMaximumRetries")

//
// System hive thresholds.
//

#define SYSTEM_HIVE_LOW     80
#define SYSTEM_HIVE_HIGH    90

//
// Define callback routine for PipApplyFunctionToServiceInstances.
//
typedef LOGICAL (*PIOP_SUBKEY_CALLBACK_ROUTINE) (
    __in     HANDLE,
    __in     PUNICODE_STRING,
    __inout PVOID
    );

typedef struct _SYSTEM_HIVE_LIMITS {
    ULONG Low;
    ULONG High;
} SYSTEM_HIVE_LIMITS, *PSYSTEM_HIVE_LIMITS;

VOID
PpSystemHiveLimitCallback(
    PSYSTEM_HIVE_LIMITS HiveLimits,
    ULONG Level
    );

NTSTATUS
PipOpenServiceEnumKeys(
    __in PUNICODE_STRING ServiceKeyName,
    __in ACCESS_MASK DesiredAccess,
    __out_opt PHANDLE ServiceHandle,
    __out_opt PHANDLE ServiceEnumHandle,
    __in BOOLEAN CreateEnum
    );

NTSTATUS
IopOpenCurrentHwProfileDeviceInstanceKey(
    __out PHANDLE Handle,
    __in  PUNICODE_STRING ServiceKeyName,
    __in  ULONG Instance,
    __in  ACCESS_MASK DesiredAccess,
    __in  BOOLEAN Create
    );

NTSTATUS
PipApplyFunctionToServiceInstances(
    __in_opt HANDLE ServiceKeyHandle,
    __in_opt PUNICODE_STRING ServiceKeyName,
    __in ACCESS_MASK DesiredAccess,
    __in BOOLEAN IgnoreNonCriticalErrors,
    __in PIOP_SUBKEY_CALLBACK_ROUTINE DevInstCallbackRoutine,
    __inout PVOID Context,
    __out_opt PULONG ServiceInstanceOrdinal
    );

NTSTATUS
PipServiceInstanceToDeviceInstance(
    __in_opt HANDLE ServiceKeyHandle,
    __in_opt PUNICODE_STRING ServiceKeyName,
    __in  ULONG ServiceInstanceOrdinal,
    __out_opt PUNICODE_STRING DeviceInstanceRegistryPath,
    __out_opt PHANDLE DeviceInstanceHandle,
    __in  ACCESS_MASK DesiredAccess
    );

NTSTATUS
PnpSetRegistryResourceList (
    __in HANDLE Key,
    __in PWSTR Name,
    __in_opt PCM_RESOURCE_LIST ResourceList
    );

NTSTATUS
PnpSetRegistryRequirementsList (
    __in HANDLE Key,
    __in PWSTR Name,
    __in_opt PIO_RESOURCE_REQUIREMENTS_LIST ResourceRequirements
    );

NTSTATUS
PnpSetRegistryDword (
    __in HANDLE Key,
    __in PWSTR Name,
    __in ULONG Value
    );

