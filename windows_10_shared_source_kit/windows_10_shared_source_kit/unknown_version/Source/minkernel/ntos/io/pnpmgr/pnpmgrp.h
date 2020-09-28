/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    pnpmgrp.h

Abstract:

    This module contains the private (internal) header file for the NT PNP subsystem.

Author:

    Santosh Jodh (santoshj) 12-Jul-2004

--*/

#pragma once

#ifndef FAR
#define FAR
#endif

#define Add2Ptr(P,I) ((PVOID)((PUCHAR)(P) + (I)))
#define PtrOffset(B,O) ((ULONG)((ULONG_PTR)(O) - (ULONG_PTR)(B)))

#define CSTRLEN(s) (RTL_NUMBER_OF(s) - 1)

#define RTL_USE_AVL_TABLES 0
#define _KRPCENV_

#include <ntos.h>
#include <pnppo.h>
#include <zwapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minwindef.h>
#include <winerror.h>
#include <arbiter.h>
#include <wdmguid.h>
#include <iopcmn.h>
#include <verifier.h>
#include <inbv.h>
#include <hdlsblk.h>
#include <hdlsterm.h>
#include <SystemEvents.h>
#include <poclass.h>
#include "cm.h"
#include <ntstrsafe.h>
#include <ntintsafe.h>
#include <dontuse.h>
#include <triage9f.h>
#include <pnprtl.h>
#include <drvdb.h>
#include <devpkey.h>
#include <devpkeyp.h>
#include <devfilter.h>
#include <rpc.h>
#include <rpcndr.h>
#include <devapi.h>
#include <wnfnamesp.h>
#include <smbios.h>
#include <mfgmode.h>
#include <winmeta.h>
#include <telemetry\MicrosoftTelemetry.h>
#include <tlgaggregate.h>

#ifndef _NTSYM_HARDWARE_PTE_SYMBOL_
#include <swdeviceif_s.h>
#include <devqueryif_s.h>
#endif

//
// Upper limit on how many times we will retry a device that has failed because we hit the
// race condition of attempting a load when the image is still loaded but the driver
// object has disappeared from namespace.
//
#define PNP_DRIVER_UNLOAD_RETRY_LIMIT   5

#define GUID_STRING_LEN         39
#define MAX_DEVICE_ID_LEN       200     // size in chars
#define MAX_SERVICE_NAME_LEN    256     // size in chars
#define SERVICE_NAME_LEN_GUESS  (MAX_SERVICE_NAME_LEN / 3) // size in chars
#define MAX_CLASS_NAME_LEN      32      // size in chars
#define MAX_PROFILE_ID_LEN      5       // size in chars

#define INITIAL_HARDWARE_ID_LIST_SIZE  512 // size in bytes
#define INITIAL_DEVICE_LIST_SIZE      4096 // size in bytes
#define INITIAL_LOCATION_INFO_SIZE      64 // size in bytes

//
// Max number of times we will retry getting a list of devices or device interfaces
// where STATUS_BUFFER_TOO_SMALL is returned each attempt (i.e. the size of the
// list keeps on changing).
//
#define MAX_GET_LIST_RETRIES   5

//
// Lengths to pass to IopGetRegistryValue as an initial guess for the size of the data
// we will retrieve (in bytes).
//
#define SERVICE_NAME_SIZE       (16 * sizeof(WCHAR))
#define GUID_STR_SIZE           (GUID_STRING_LEN * sizeof(WCHAR))
#define SERVICE_INSTANCE_SIZE   (50 * sizeof(WCHAR))
#define REGISTRY_PROPERTY_SIZE  0x140
#define DRIVER_TAG_SIZE         0x50

//
// Constants
//
#define MAX_ULONGLONG                           ((ULONGLONG) -1)
#define PNP_POLL_BOOT_PARTITION_INTERVAL        200 //milliseconds
#define PNP_MAX_POLL_BOOT_PARTITION_TIMEOUT     180000 //milliseconds

extern const WCHAR PathPrefixWin32[];
extern const USHORT PathPrefixLength;


//
// Forward typedefs
//
typedef struct _PNP_DEVICE_ACTION_ENTRY PNP_DEVICE_ACTION_ENTRY, *PPNP_DEVICE_ACTION_ENTRY;
typedef struct _RELATION_LIST RELATION_LIST, *PRELATION_LIST;
typedef struct _PNP_DEVICE_EVENT_ENTRY PNP_DEVICE_EVENT_ENTRY, *PPNP_DEVICE_EVENT_ENTRY;
typedef struct _PI_DM_OBJECT PI_DM_OBJECT, *PPI_DM_OBJECT;
typedef enum _DEVICE_RELATION_LEVEL DEVICE_RELATION_LEVEL, *PDEVICE_RELATION_LEVEL;

#include "pooltags.h"
#include "debug.h"
#include "macro.h"
#include "locks.h"
#include "queue.h"
#include "registry.h"
#include "driver.h"
#include "dockintf.h"
#include "fileutil.h"
#include "diag.h"
#include "verifier.h"
#include "rmvpolicy.h"
#include "blockeddrv.h"
#include "devcfg.h"
#include "remove.h"
#include "relations.h"
#include "event.h"
#include "dockdevice.h"
#include "swdevicep.h"
#include "device.h"
#include "async.h"
#include "pagepath.h"
#include "control.h"
#include "lastgood.h"
#include "enum.h"
#include "action.h"
#include "notify.h"
#include "legacy.h"
#include "assign.h"
#include "hwconfig.h"
#include "irp.h"
#include "bootcfg.h"
#include "conflict.h"
#include "rebalance.h"
#include "start.h"
#include "audit.h"
#include "register.h"
#include "memioarb.h"
#include "irqarb.h"
#include "dmaarb.h"
#include "busnumarb.h"
#include "interface.h"
#include "utils.h"
#include "property.h"
#include "pnpinit.h"
#include "replace.h"
#include "depend.h"
#include "pnprtlp.h"
#include "devqueryp.h"
#include "devapidd.h"
#include "cmapi.h"
#include "devcontainer.h"
#include "uevent.h"
#include "auth.h"
#include "datamodel.h"
#include "pendop.h"
#include "reset.h"
#include "tracelog.h"
#include "eventremove.h"

//
// Enter critical section and acquire a lock on the registry.  Both these
// mechanisms are required to prevent deadlock in the case where an APC
// routine calls this routine after the registry resource has been claimed
// in this case it would wait blocking this thread so the registry would
// never be released -> deadlock.  Critical sectioning the registry manipulation
// portion solves this problem
//
#define PiLockPnpRegistry(Exclusive) {  \
    KeEnterCriticalRegion();            \
    if (Exclusive) {                    \
        ExAcquireResourceExclusiveLite( \
            &PnpRegistryDeviceResource, \
            TRUE);                      \
    } else {                            \
        ExAcquireResourceSharedLite(    \
            &PnpRegistryDeviceResource, \
            TRUE);                      \
    }                                   \
}

//
// Unblock write access to Pnp portion of registry.
//
#define PiUnlockPnpRegistry() {                         \
    ExReleaseResourceLite(&PnpRegistryDeviceResource);  \
    KeLeaveCriticalRegion();                            \
}

#define PiIsPnpRegistryLocked(Exclusive)    \
    ((Exclusive) ? ExIsResourceAcquiredExclusiveLite(&PnpRegistryDeviceResource) : \
                    ((ExIsResourceAcquiredSharedLite(&PnpRegistryDeviceResource) > 0) ? TRUE : FALSE))

//
// PNP registry lock.
//
extern ERESOURCE PnpRegistryDeviceResource;

//
// Private Entry Points
//
NTSTATUS
PiGetDeviceRegistryProperty(
    __in      PDEVICE_OBJECT   DeviceObject,
    __in      ULONG            ValueType,
    __in      PWSTR            ValueName,
    __in_opt  PWSTR            KeyName,
    __out_bcount_opt(*BufferLength)  PVOID Buffer,
    __inout   PULONG           BufferLength
    );

NTSTATUS
PiGetDeviceRegProperty(
    __in      PCWSTR           DeviceInstanceId,
    __in_opt  HANDLE           BaseKeyHandle,
    __in      ULONG            ExpectedType,
    __in      CM_REG_PROP      RegProperty,
    __out_bcount_opt(*BufferSize)  PVOID  Buffer,
    __inout   PULONG           BufferSize
    );

NTSTATUS
PiControlMakeUserModeCallersCopy(
    __deref_inout_bcount(Length) PVOID           *Destination,
    __in_bcount(Length) PVOID           Src,
    __in ULONG           Length,
    __in ULONG           Alignment,
    __in KPROCESSOR_MODE CallerMode,
    __in LOGICAL         AllocateDestination
    );

#if DBG

LONG
PiControlExceptionFilter(
    __in  PEXCEPTION_POINTERS ExceptionPointers
    );

#else

#define PiControlExceptionFilter(a)  EXCEPTION_EXECUTE_HANDLER

#endif

//
// Pool tags
//

#define IOP_DNOD_TAG    'donD'
#define IOP_DPWR_TAG    'rwPD'

//
// The DEVICE_NODE is really just some extra stuff that we'd like to keep around
// for each physical device object.
// It is seperated from DEVOBJ_EXTENSION because these fields only apply to
// PDO.
//

//
// Driver Violation Bugcheck Subcodes
//
#define DRIVER_IRQL_VIOLATION                     0x1

//
// PNP Bugcheck Subcodes
//
#define PNP_ERR_DUPLICATE_PDO                       1
#define PNP_ERR_INVALID_PDO                         2
#define PNP_ERR_BOGUS_ID                            3
#define PNP_ERR_PDO_ENUMERATED_AFTER_DELETION       4
#define PNP_ERR_ACTIVE_PDO_FREED                    5

#define PNP_ERR_DEVICE_MISSING_FROM_EJECT_LIST      6
#define PNP_ERR_UNEXPECTED_ADD_RELATION_ERR         7
#define PNP_ERR_BAD_INTERRUPT_DISCONNECT_ATTEMPT    9
#define PNP_ERR_INCORRECT_NOTIFY_CALLBACK_BEHAVIOR  10
#define PNP_ERR_DELETED_PDO_REPORTED_AS_RELATION    11
#define PNP_ERR_BAD_INTERRUPT_REPORTING_ATTEMPT     12
#define PNP_ERR_REQUEST_TO_IDLE_INACTIVE_DEVICE     13
#define PNP_ERR_INVALID_REMOVAL_REQUEST             14

#define MAX_INSTANCE_PATH_LENGTH   (MAX_DEVICE_ID_LEN+1)

//
// Define context structures for Start and Add device services
//

#define NO_MORE_GROUP ((USHORT) -1)

#define QUERY_RESOURCE_LIST                0
#define QUERY_RESOURCE_REQUIREMENTS        1

#define REGISTRY_ALLOC_CONFIG              1
#define REGISTRY_FORCED_CONFIG             2
#define REGISTRY_BOOT_CONFIG               4
#define REGISTRY_OVERRIDE_CONFIGVECTOR     1
#define REGISTRY_BASIC_CONFIGVECTOR        2

//
// Misc
//

#define CmResourceTypeReserved  0xf0

//
// Size of scratch buffer used in this module.
//

#define PNP_SCRATCH_BUFFER_SIZE 512

//
// Define Device Instance Flags (used by IoQueryDeviceConfiguration apis)
//

#define DEVINSTANCE_FLAG_HWPROFILE_DISABLED 0x1
#define DEVINSTANCE_FLAG_PNP_ENUMERATED 0x2

//
// The following definitions are used in IoOpenDeviceInstanceKey
//

#define PLUGPLAY_REGKEY_DEVICE  1
#define PLUGPLAY_REGKEY_DRIVER  2
#define PLUGPLAY_REGKEY_CURRENT_HWPROFILE 4

//
// the below defines are copied from ppmacros.h
//
#define INVALID_FLAGS(ulFlags, ulAllowed) ((ulFlags) & ~(ulAllowed))
#define TEST_FLAGS(t, ulMask, ulBit)      (((t)&(ulMask)) == (ulBit))
#define IS_FLAG_SET(t, ulMask)            TEST_FLAGS(t, ulMask, ulMask)
#define SET_FLAG(Status, Flag)            ((Status) |= (Flag))
#define CLEAR_FLAG(Status, Flag)          ((Status) &= ~(Flag))

//
// Init data
//

extern PCM_RESOURCE_LIST IopInitHalResources;
extern PDEVICE_NODE IopInitHalDeviceNode;
extern PIOP_RESERVED_RESOURCES_RECORD IopInitReservedResourceList;

//
// Regular data
//

//
// IopPnPDriverObject - the madeup driver object for pnp manager
//

extern PDRIVER_OBJECT IopPnPDriverObject;

//
// PiEngineLock - Synchronizes the start/enum and remove engines.
//     (Note that this is a resource as certain acquisition paths are reentrant,
//      specifically those that call PnpIsChainDereferenced.)
//

extern ERESOURCE PiEngineLock;

//
// IopDeviceTreeLock - performs syncronization on the whole device node tree.
//      IopAcquireEnumerationLock acquires this lock shared then optionally
//                                acquires an exclusive lock on a devnode.
//      IopAcquireDeviceTreeLock acquires this lock exclusive
//

extern ERESOURCE IopDeviceTreeLock;

//
// IopSurpriseRemoveListLock - synchronizes access to the surprise remove list.
//

extern ERESOURCE IopSurpriseRemoveListLock;

//
// PiResourceListLock - to synchronize between the IoGetDeviceProperty of
// resource lists and their storage in the devnode
//

extern KGUARDED_MUTEX PiResourceListLock;

//
// PnPInitialized - A flag to indicate if PnP initialization is completed.
//

extern BOOLEAN PnPInitialized;

//
// PnPBootDriverInitialied
//

extern BOOLEAN PnPBootDriversInitialized;

//
// PnPBootDriverLoaded
//

extern BOOLEAN PnPBootDriversLoaded;

//
// IopBootConfigsReserved - Indicates whether we have reserved BOOT configs or not.
//

extern BOOLEAN IopBootConfigsReserved;

//
// PnpDefaultInterfaceTYpe - Use this if the interface type of resource list is unknown.
//

extern INTERFACE_TYPE PnpDefaultInterfaceType;

//
// IopPendingEjects - List of pending eject requests
//
extern LIST_ENTRY  IopPendingEjects;

//
// IopPendingSurpriseRemovals - List of pending surprise removal requests
//
extern LIST_ENTRY   IopPendingSurpriseRemovals;

extern KSEMAPHORE   PpRegistrySemaphore;

//
// PnpDeviceReferenceTableLock - synchronizes access to device reference table.
//

extern KGUARDED_MUTEX PnpDeviceReferenceTableLock;

#define PiInitializeEngineLock() \
    ExInitializeResourceLite(&PiEngineLock)

typedef enum {

    PPL_SIMPLE_READ,
    PPL_TREEOP_ALLOW_READS,
    PPL_TREEOP_ALLOW_READS_NO_WAIT,
    PPL_TREEOP_BLOCK_READS,
    PPL_TREEOP_BLOCK_READS_FROM_ALLOW

} PNP_LOCK_LEVEL;

BOOLEAN
PpDevNodeLockTree(
    __in  PNP_LOCK_LEVEL  LockLevel
    );

VOID
PpDevNodeUnlockTree(
    __in  PNP_LOCK_LEVEL  LockLevel
    );

#if DBG
VOID
PpDevNodeAssertLockLevel(
    __in  PNP_LOCK_LEVEL  LockLevel,
    __in  PCSTR           File,
    __in  ULONG           Line
    );

#define PPDEVNODE_ASSERT_LOCK_HELD(Level) \
    PpDevNodeAssertLockLevel(Level, __FILE__, __LINE__)

#else
#define PPDEVNODE_ASSERT_LOCK_HELD(Level)
#endif


VOID
IopResourceRequirementsChanged(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in BOOLEAN StopRequired
    );

VOID
IopInitializeResourceMap (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
IopWriteResourceList(
    __in HANDLE ResourceMapKey,
    __in PUNICODE_STRING ClassName,
    __in PUNICODE_STRING DriverName,
    __in PUNICODE_STRING DeviceName,
    __in_bcount(ResourceListSize) PCM_RESOURCE_LIST ResourceList,
    __in ULONG ResourceListSize
    );

USHORT
PpInitGetGroupOrderIndex(
    __in_opt HANDLE ServiceHandle
    );

VOID
PpResetProblemDevices(
    __in  PDEVICE_NODE    DeviceNode,
    __in  ULONG           Problem
    );

NTSTATUS
IoReportResourceUsageInternal(
    __in ARBITER_REQUEST_SOURCE AllocationType,
    __in_opt PUNICODE_STRING DriverClassName ,
    __in PDRIVER_OBJECT DriverObject,
    __in_bcount_opt(DriverListSize) PCM_RESOURCE_LIST DriverList ,
    __in ULONG DriverListSize ,
    __in_opt PDEVICE_OBJECT DeviceObject ,
    __in_bcount_opt(DeviceListSize) PCM_RESOURCE_LIST DeviceList ,
    __in ULONG DeviceListSize ,
    __in BOOLEAN OverrideConflict,
    __out PBOOLEAN ConflictDetected
    );

LOGICAL
PnpEarlyLaunchHasCallbacksRegistered (
    VOID
    );

VOID
PnpEarlyLaunchImageNotificationPreProcess (
    __in PVOID Context,
    __in PVOID CallbackType,
    __in PBDCB_IMAGE_INFORMATION ImageInformation,
    __in PBDCB_CLASSIFICATION Classification,
    __in PBDCB_IMAGE_INFORMATION ControlImageInformation
    );

VOID
PnpEarlyLaunchImageNotificationPostProcess (
    __in PVOID Context,
    __in PVOID CallbackType,
    __in PBDCB_IMAGE_INFORMATION ReturnedImageInformation,
    __in PBDCB_CLASSIFICATION Classification,
    __in PBDCB_IMAGE_INFORMATION ControlImageInformation
    );

VOID
PnpEarlyLaunchStatusNotificationPreProcess (
    __in PVOID Context,
    __in PVOID CallbackType,
    __in PBDCB_STATUS_UPDATE_CONTEXT StatusContext,
    __in PVOID Classification,
    __in PBDCB_STATUS_UPDATE_CONTEXT ControlStatusContext
    );

VOID
PnpNotifyEarlyLaunchImageLoad (
    __in PBDCB_IMAGE_INFORMATION ImageInformation,
    __inout PBDCB_CLASSIFICATION Classification
    );

VOID
PnpNotifyEarlyLaunchStatusUpdate (
    __in BDCB_STATUS_UPDATE_TYPE Status
    );

VOID
PnpCloseBootDriverCallbackRegistration (
    VOID
    );

NTSTATUS
PnpLoadBootFilterDriver (
    __in HANDLE ServiceKeyHandle,
    __in PUNICODE_STRING DriverName,
    __in ULONG GroupIndex,
    __out PDRIVER_OBJECT *LoadedFilter
    );

NTSTATUS
PnpSetDeviceInstanceRemovalEvent (
    __in  PDEVICE_NODE DeviceNode
    );

NTSTATUS
PnpSetDeviceInstancePropertyChangeEvent (
    __in  PDEVICE_NODE DeviceNode
    );

NTSTATUS
PnpSetDeviceInstancePropertyChangeEventFromDeviceInstance(
    __in  PUNICODE_STRING DeviceInstance
    );

NTSTATUS
PnpSetDeviceInstanceStartedEvent (
    __in  PDEVICE_NODE DeviceNode
    );

NTSTATUS
PnpSetDeviceInstanceStartedEventFromDeviceInstance(
    __in  PUNICODE_STRING DeviceInstance
    );

LOGICAL
PnpFindAlternateStringData(
    __in_bcount(StringLength) PWCHAR String,
    __in ULONG StringLength,
    __deref_out_bcount(*SubStringLength) PWCHAR *SubStringPtr,
    __out PULONG SubStringLength
    );

NTSTATUS
PpLastGoodDeleteFilesCallback (
    __in PUNICODE_STRING FullPathName,
    __in PUNICODE_STRING FileName,
    __in ULONG FileAttributes,
    __in PVOID Context
    );

//
// IopMaxDeviceNodeLevel - Level number of the DeviceNode deepest in the tree
//
extern ULONG       IopMaxDeviceNodeLevel;

//
// Warm eject externs and function prototypes
//
extern KEVENT IopWarmEjectLock;
extern PDEVICE_OBJECT IopWarmEjectPdo;

NTSTATUS
IopWarmEjectDevice(
    __in PDEVICE_OBJECT      DeviceToEject,
    __in SYSTEM_POWER_STATE  LightestSleepState
    );

//
// PnpSystemDeviceEnumerationComplete - Event which signals the end of
// system device enumeration.
//

extern KEVENT PnpSystemDeviceEnumerationComplete;

//
// PnpFindBestConfigurationTimeout - Maximum resource arbitration timeout. The
// default value can be overriden via the registry.
//

extern ULONG PnpFindBestConfigurationTimeout;

//
// Hardware configuration guid of the system being booted on.
//
extern UNICODE_STRING PnpCurrentHardwareConfigurationGuidString;
extern ULONG PnpCurrentHardwareConfigurationIndex;

#ifdef POOL_TAGGING
#undef ExAllocatePool
#define ExAllocatePool(a,b) ExAllocatePoolWithTag(a,b,'  pP')
#undef ExAllocatePoolWithQuota
#define ExAllocatePoolWithQuota(a,b) ExAllocatePoolWithQuotaTag(a,b,'  pP')
#endif

//
// Characteristics to propagate post-start
//
#define PNP_POST_START_CHARACTERISTICS  FILE_PORTABLE_DEVICE
