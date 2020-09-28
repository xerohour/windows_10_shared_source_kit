/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    event.h

Abstract:

    This module contains the private (internal) header file for PNP events.

Author:

    Santosh Jodh (santoshj) 12-Jul-2004


Revision History:

--*/

#pragma once

#define TDF_DEVICEEJECTABLE         0x00000001
#define TDF_NO_RESTART              0x00000002
#define TDF_KERNEL_INITIATED        0x00000004
#define TDF_NOTIFY_VETO             0x00000008

// 
// This macro returns the address of a field given the type of the structure, pointer to
// the base of the structure and the name of the field whose address is to be computed.
// This returns the same value as &Base->Member
//

#define PNP_FIELD_ADDRESS(Type, Base, Member) ((PUCHAR)Base + FIELD_OFFSET(Type, Member))

//
// This macro is used like a C for loop and iterates over all the entries in the list.
//
// _Type - Name of the data type for each element in the list
// _Link - Name of the field of type LIST_ENTRY used to link the elements.
// _Head - Pointer to the head of the lis, FALSE otherwise..
// _Current - Points to the iterated element.
//
// Example:     PNP_FOR_EACH_IN_LIST(SYSTEM_FIRMWARE_TABLE_HANDLER_NODE,
//                                  FirmwareTableProviderList,
//                                  &ExpFirmwareTableProviderListHead,
//                                  HandlerListCurrent) {
//
//                  /* Do something with Current */
//
//              }
//

#define PNP_FOR_EACH_IN_LIST(_Type, _Link, _Head, _Current)                                             \
    for((_Current) = CONTAINING_RECORD((_Head)->Flink, _Type, _Link);                                   \
       (_Head) != (PLIST_ENTRY)PNP_FIELD_ADDRESS(_Type, _Current, _Link);                               \
       (_Current) = CONTAINING_RECORD(((PLIST_ENTRY)PNP_FIELD_ADDRESS(_Type, _Current, _Link))->Flink,  \
                                     _Type,                                                          \
                                     _Link)                                                          \
       )

//
// PNP_DEVICE_EVENT_ENTRY
//
// One of these structures is allocated for each dynamic device event and is removed after
// the event has been posted to all waiting recipients.
//

typedef struct _PNP_DEVICE_EVENT_ENTRY {
    LIST_ENTRY                          ListEntry;
    ULONG                               Argument;
    NTSTATUS                            ArgumentStatus;
    PKEVENT                             CallerEvent;
    PDEVICE_CHANGE_COMPLETE_CALLBACK    Callback;
    PVOID                               Context;
    PPNP_VETO_TYPE                      VetoType;
    PUNICODE_STRING                     VetoName;
    ULONG                               RefCount;
    ULONG                               Lock;
    UCHAR                               Cancel;
    PPNP_DEVICE_EVENT_ENTRY             Parent;
    GUID                                ActivityId;
    PLUGPLAY_EVENT_BLOCK                Data; //this must be the last member in
                                              //the structure due to it
                                              //containing an ANYSIZE array

} PNP_DEVICE_EVENT_ENTRY, *PPNP_DEVICE_EVENT_ENTRY;

//
// PNP_DEVICE_EVENT_LIST
//
//  This is the head of the master device event list for both UserMode and KernelMode.
//

typedef struct _PNP_DEVICE_EVENT_LIST {
    NTSTATUS        Status;
    KMUTEX          EventQueueMutex;
    KGUARDED_MUTEX  Lock;
    LIST_ENTRY      List;

} PNP_DEVICE_EVENT_LIST, *PPNP_DEVICE_EVENT_LIST;

// 
// PNP_VETO_DEVICE_OBJECT_INFO
// 
// This structure is used to describe a device object
// on which an open handle exists which is preventing
// the removal of another device.
// 

typedef struct _PNP_VETO_DEVICE_OBJECT_INFO {
    PDEVICE_OBJECT DeviceObject;
    LIST_ENTRY ListEntry;
} PNP_VETO_DEVICE_OBJECT_INFO, *PPNP_VETO_DEVICE_OBJECT_INFO;

// 
// PNP_VETO_PROCESS_INFO
// 
// This structure is used to describe a process vetoing
// the removal of a device.  A linked list containing
// nodes of this type represent the list of processes
// vetoing the removal of that device.
//

typedef struct _PNP_VETO_PROCESS_INFO {
    LIST_ENTRY DevObjListHead;
    ULONG Pid;
    LIST_ENTRY ListEntry;
} PNP_VETO_PROCESS_INFO, *PPNP_VETO_PROCESS_INFO;

//
// PNP_USER_BLOCK
//
//  The caller block contains info describing the caller of
//  NtGetPlugPlayEvent. There's only one caller block.
//

typedef struct _PNP_USER_BLOCK {
    NTSTATUS                Status;
    ULONG                   Result;
    PPNP_VETO_TYPE          VetoType;
    PUNICODE_STRING         VetoName;
    ERESOURCE               Lock;
    KEVENT                  Registered;
    KEVENT                  NotifyUserEvent;
    KEVENT                  UserResultEvent;
    PVOID                   PoolBuffer;
    ULONG                   PoolUsed;
    ULONG                   PoolSize;
    BOOLEAN                 Deferred;

} PNP_USER_BLOCK, *PPNP_USER_BLOCK;

typedef struct _ENUM_HANDLES_CONTEXT {
    PDEVICE_OBJECT  PhysicalDeviceObject;
    PUNICODE_STRING TargetDeviceInstance;
    LIST_ENTRY      VetoProcessList;
    ULONG           HandleCount;
    BOOLEAN         DumpHandles;
    BOOLEAN         EnableLogging;

} ENUM_HANDLES_CONTEXT, *PENUM_HANDLES_CONTEXT;

VOID
PnpAddVetoInformation (
    __in PDEVICE_OBJECT DeviceObject,
    __in PLIST_ENTRY VetoProcessListHead,
    __in ULONG Pid
    );

//
// Function to complete an event asynchronously.
//

PVOID
PnpAllocateCriticalMemory (
    __in  PNP_DEVICE_DELETE_TYPE DeleteType,
    __in  POOL_TYPE PoolType,
    __in  SIZE_T Size,
    __in  ULONG Tag
    );

VOID
PnpBuildUnsafeRemovalDeviceList (
    __in  PPNP_DEVICE_EVENT_ENTRY     OriginalDeviceEvent,
    __in  PRELATION_LIST              RelationsList,
    __out PWSTR                       *UnsafeRemovalList
    );

VOID
PnpCollectOpenHandles (
    __in_ecount(ArrayCount) PDEVICE_OBJECT *DeviceObjectArray,
    __in                    ULONG ArrayCount,
    __in                    PENUM_HANDLES_CONTEXT EnumContext
    );

LOGICAL
PnpCollectOpenHandlesCallBack (
    __in    PDEVICE_OBJECT  DeviceObject,
    __in    PEPROCESS       Process,
    __in    PFILE_OBJECT    FileObject,
    __in    HANDLE          HandleId,
    __in    PVOID           Context
    );

BOOLEAN
PnpCompareGuid (
    __in CONST GUID *Guid1,
    __in CONST GUID *Guid2
    );

VOID
PnpCompleteDeviceEvent (
    __inout PPNP_DEVICE_EVENT_ENTRY  DeviceEvent,
    __in    NTSTATUS                 FinalStatus
    );

PPNP_DEVICE_EVENT_ENTRY
PnpCreateDeviceEventEntry (
    __in ULONG Size
    );

#define PnpDeleteDeviceEventEntry(e) \
    if (InterlockedDecrement((PLONG)&(e)->RefCount) == 0) { \
        ExFreePoolWithTag((e), PNP_POOLTAG_DEVICE_EVENT_ENTRY); \
    }

VOID
PnpDeviceEventWorker (
    __in PVOID Context
    );

VOID
PnpDumpDeviceEvent (
    __in PPLUGPLAY_EVENT_BLOCK EventBlock
    );

VOID
PnpDumpMultiSz (
    __in __nullnullterminated PWCHAR MultiSz
    );

VOID
PnpFinalizeVetoedRemove (
    __in        PPNP_DEVICE_EVENT_ENTRY VetoedDeviceEvent,
    __in        PNP_VETO_TYPE VetoType,
    __in_opt    PUNICODE_STRING VetoName
    );

VOID
PnpFreeVetoInformation (
    __in PLIST_ENTRY VetoProcessListHead
    );

NTSTATUS
PnpInitializeDeviceEvents (
    VOID
    );

VOID
PnpInitializeTargetDeviceRemoveEvent (
    __out_bcount(DeviceEventSize)   PPNP_DEVICE_EVENT_ENTRY DeviceEvent,
    __in                            ULONG DeviceEventSize,
    __in                            PDEVICE_OBJECT DeviceObject,
    __in                            LOGICAL KernelInitiated,
    __in                            LOGICAL NoRestart,
    __in                            LOGICAL DoEject,
    __in                            LOGICAL NotifyVeto,
    __in                            ULONG Problem,
    __in                            NTSTATUS ProblemStatus,
    __in_opt                        PKEVENT SyncEvent,
    __out_opt                       PULONG Result,
    __out_opt                       PPNP_VETO_TYPE VetoType,
    __out_opt                       PUNICODE_STRING VetoName
    );

NTSTATUS
PnpInsertEventInQueue (
    __in PPNP_DEVICE_EVENT_ENTRY DeviceEvent
    );

VOID
PnpLogVetoInformation (
    __in PUNICODE_STRING DeviceInstance,
    __in PLIST_ENTRY VetoProcessListHead
);

VOID
PnpLookupGuid (
    __in                            CONST GUID *Guid,
    __inout_bcount(StringLength)    PCHAR String,
    __in                            ULONG StringLength
    );

NTSTATUS
PnpNotifyUserModeDeviceRemoval (
    __in        PPNP_DEVICE_EVENT_ENTRY TemplateDeviceEvent,
    __in        PWSTR                   RelatedDeviceInstances,
    __in        CONST GUID *EventGuid,
    __out_opt   PPNP_VETO_TYPE VetoType,
    __out_opt   PUNICODE_STRING VetoName
    );

NTSTATUS
PnpSetBlockedDriverEvent (
    __in GUID CONST *BlockedDriverGuid
    );

NTSTATUS
PnpSetCustomTargetEvent (
    __in        PDEVICE_OBJECT DeviceObject,
    __in_opt    PKEVENT SyncEvent,
    __out_opt   PULONG Result,
    __in_opt    PDEVICE_CHANGE_COMPLETE_CALLBACK Callback,
    __in_opt    PVOID Context,
    __in        PTARGET_DEVICE_CUSTOM_NOTIFICATION NotificationStructure
    );

NTSTATUS
PnpSetDeviceClassChange (
    __in CONST GUID *EventGuid,
    __in CONST GUID *ClassGuid,
    __in PUNICODE_STRING SymbolicLinkName
    );

NTSTATUS
PnpSetDeviceRemovalSafe(
    __in        PDEVICE_OBJECT DeviceObject,
    __in_opt    PKEVENT SyncEvent,
    __out_opt   PULONG Result
    );

NTSTATUS
PnpSetHwProfileChangeEvent (
    __in        CONST GUID *EventGuid,
    __in_opt    PKEVENT CompletionEvent,
    __out_opt   PNTSTATUS CompletionStatus,
    __out_opt   PPNP_VETO_TYPE VetoType,
    __out_opt   PUNICODE_STRING VetoName
    );

NTSTATUS
PnpSetInvalidIDEvent(
    __in PUNICODE_STRING ParentInstance
    );

VOID
PnpSetPlugPlayEvent(
    __in CONST GUID *EventGuid,
    __in PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
PnpSetPowerVetoEvent (
    __in        POWER_ACTION    VetoedPowerOperation,
    __in_opt    PKEVENT         CompletionEvent,
    __out_opt   PNTSTATUS       CompletionStatus,
    __in        PDEVICE_OBJECT  DeviceObject,
    __in        PNP_VETO_TYPE   VetoType,
    __in_opt    PUNICODE_STRING VetoName
    );

NTSTATUS
PnpSetTargetDeviceRemove (
    __in        PDEVICE_OBJECT DeviceObject,
    __in        LOGICAL KernelInitiated,
    __in        LOGICAL NoRestart,
    __in        LOGICAL DoEject,
    __in        LOGICAL NotifyVeto,
    __in        ULONG Problem,
    __in        NTSTATUS ProblemStatus,
    __in_opt    PKEVENT SyncEvent,
    __in_opt    PDEVICE_CHANGE_COMPLETE_CALLBACK Callback,
    __in_opt    PVOID Context,
    __out_opt   PULONG Result,
    __out_opt   PPNP_VETO_TYPE VetoType,
    __out_opt   PUNICODE_STRING VetoName,
    __out_opt   PPNP_DEVICE_EVENT_ENTRY *DeviceEvent
    );

NTSTATUS
PnpSynchronizeDeviceEventQueue (
    VOID
    );

NTSTATUS
PnpProcessQueryRemoveAndEject (
    __inout PPNP_DEVICE_EVENT_ENTRY *DeviceEvent
    );

NTSTATUS
PnpProcessTargetDeviceEvent (
    __inout PPNP_DEVICE_EVENT_ENTRY *DeviceEvent
    );

NTSTATUS
PnpProcessCustomDeviceEvent (
    __inout PPNP_DEVICE_EVENT_ENTRY *DeviceEvent
    );

LOGICAL
PnpRemoveEventFromQueue (
    __inout PPNP_DEVICE_EVENT_ENTRY DeviceEvent
    );

NTSTATUS
PnpResizeTargetDeviceBlock(
    __inout PPNP_DEVICE_EVENT_ENTRY *DeviceEvent,
    __in PNP_DEVICE_DELETE_TYPE DeleteType
    );

NTSTATUS
PnpCompileDeviceInstancePaths(
    __in PNP_DEVICE_DELETE_TYPE DeleteType,
    __in PRELATION_LIST RelationsList,
    __in BOOLEAN ExcludeDeviceRelations,
    __in BOOLEAN ExcludeHungDevices,
    __out PWSTR *RelatedDeviceInstances
    );;

NTSTATUS
PnpWaitForEmptyDeviceEventQueue (
    VOID
    );

extern KGUARDED_MUTEX PnpNotificationInProgressLock;
extern BOOLEAN PnpNotificationInProgress;
