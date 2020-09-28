/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    replace.h

Abstract:

    This module implements the partition unit replace.

Author:

    Santosh Jodh (santoshj) 14-03-2006

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

#define MINIMUM_RESOURCE_EXTENSION 4
#define PNPR_REPLACE_DRIVER_VALUE  L"ReplaceDriver"
#define PNPR_REPLACE_DRIVER_ROOT   L"\\systemroot\\"

#define PROCESSOR_LIST_SIZE(_Count) \
            (FIELD_OFFSET(PNP_REPLACE_PROCESSOR_LIST, ApicIds) + \
             ((_Count) * RTL_FIELD_SIZE(PNP_REPLACE_PROCESSOR_LIST, ApicIds)))

#define MEMORY_LIST_SIZE(_Count) \
            (FIELD_OFFSET(PNP_REPLACE_MEMORY_LIST, Ranges) + \
             ((_Count) * RTL_FIELD_SIZE(PNP_REPLACE_MEMORY_LIST, Ranges)))

#define ACQUIRE_REPLACE_LOCK() KeWaitForSingleObject(&PnpReplaceEvent, Executive, KernelMode, FALSE, NULL)
#define RELEASE_REPLACE_LOCK() KeSetEvent(&PnpReplaceEvent, 0, FALSE)

#define IS_TARGET_PROCESSOR(n) \
    ((AFFINITY_MASK((n)->Number) & PnprContext->Target.Processors->Affinity[(n)->Group]) != 0)

#define PNPR_REPLACE_FAILURE(_x)  \
    (PnprContext->FailureLine = (PnprContext->FailureLine == 0) ? \
                                (__LINE__) : \
                                (PnprContext->FailureLine)); \
    (PnprContext->ExtendedStatus = (PnprContext->ExtendedStatus == NoFailure) ? \
                                   (_x) : \
                                   PnprContext->ExtendedStatus)

typedef struct _PNP_REPLACEABLE_UNIT {
    PDEVICE_OBJECT Pdo;
    ULONG64 UniqueId;
    PPNP_REPLACE_PROCESSOR_LIST Processors;
    PPNP_REPLACE_MEMORY_LIST Memory;
} PNP_REPLACEABLE_UNIT, *PPNP_REPLACEABLE_UNIT;

#define PNPR_SPARE_PROCESSOR        INVALID_PROCESSOR_INDEX

typedef struct _PNP_REPLACE_POWER_CONTEXT {

    ULONG Flags;
    PO_BROADCAST_SYSTEM_STATE Broadcast;
    ULONG ConsoleId;
    WIN32_POWERSTATE_PARAMETERS Win32Parameters;

} PNP_REPLACE_POWER_CONTEXT, *PPNP_REPLACE_POWER_CONTEXT;

typedef struct _PNPR_MEMORY_BITMAP_ENTRY {
    LIST_ENTRY Links;
    PFN_NUMBER StartPfn;
    PFN_NUMBER Pages;
    RTL_BITMAP Bitmap;
} PNPR_MEMORY_BITMAP_ENTRY, *PPNPR_MEMORY_BITMAP_ENTRY;

typedef struct _PNPR_MAPPING_RESERVE {
    PMDL Mdl;
    PVOID Reserve;
    CSHORT MdlFlags;
} PNPR_MAPPING_RESERVE, *PPNPR_MAPPING_RESERVE;

#define PNPR_HAL_BEGIN_RUN 0x1
#define PNPR_DRIVER_BEGIN_RUN 0x2
#define PNPR_SWAP_IN_PROGRESS 0x4
#define PNPR_HARDWARE_MIRRORING 0x8
#define PNPR_HARDWARE_PAGE_COPY 0x10
#define PNPR_HARDWARE_QUIESCE 0x20

typedef struct _PNP_REPLACE_ARGUMENTS {

    PDEVICE_OBJECT TargetPdo;
    PDEVICE_OBJECT SparePdo;
    ULONG Flags;
    NTSTATUS Status;
    KEVENT CompletionEvent;
    WORK_QUEUE_ITEM WorkQueueItem;

} PNP_REPLACE_ARGUMENTS, *PPNP_REPLACE_ARGUMENTS;

typedef struct _PNP_REPLACE_TIMING {

    ULONG64 QuiesceTime;
    ULONG QuiesceEnabled;
    ULONG TargetsOffline; 
    ULONG SparesStarted;
    ULONG ChipsetSwapCalled; 
    ULONG ChipsetSwapComplete; 
    ULONG QuiesceDisabled;
    ULONG DeviceQuiesceStart;
    ULONG DeviceQuiesceEnd;
    ULONG DeviceWakeStart;
    ULONG DeviceWakeEnd;
    ULONG ReplaceEnd;

} PNP_REPLACE_TIMING, *PPNP_REPLACE_TIMING;

//
// Define extended status codes.  These extended codes are published as part of
// the event that is sent when a replace failure occurs.  The values are 
// directly consumed by management applications and must stay in this order.
//

typedef enum {
    NoFailure = 0,
    GeneralFailure,
    UnsupportedBySku,
    PartitionUnitDoesNotExist,
    PartitionUnitsNotCompatible,
    PartitionUnitNotReady,
    PartitionUnitResourceNotSupported,
    QuiesceFailure,
    ChipsetDriverFailure,
    ChipsetDriverNotSupported,
    AllocationFailure
} PNPR_EXTENDED_STATUS;

#define SWAP_PHASE_BEGIN 0
#define SWAP_PHASE_TARGET_REPLACE 1
#define SWAP_PHASE_COPY_MEMORY 2
#define SWAP_PHASE_EXIT_COPY 3
#define SWAP_PHASE_SWAP_PREPARE 4
#define SWAP_PHASE_SWAPPED 5
#define SWAP_PHASE_QUIESCE_ENGINE_OFF 6
#define SWAP_PHASE_RELEASE 7

typedef struct _PNP_REPLACE_CONTEXT {

    //
    // These are the fields used to maintain the information about the target
    // and spare partition units.
    //

    PNP_REPLACEABLE_UNIT Target;
    PNP_REPLACEABLE_UNIT Spare;
    ULONG Flags;

    //
    // This structures will contain the replace parameters passed to the replace
    // driver for the duration of the replace operation.
    //

    PNP_REPLACE_PARAMETERS DriverParameters;

    //
    // These fields will maintain information regarding the reserve mappings
    // used for mirroring physical memory during the replace operation.
    //

    PPNPR_MAPPING_RESERVE TargetReserves;
    PPNPR_MAPPING_RESERVE SpareReserves;

    //
    // This list head refers to a list of memory bitmaps corresponding to the
    // physical pages in the target module.  If a bit is clear, then that page
    // must be mirrored.  If it is set, then it must not be mirrored.
    //

    LIST_ENTRY TargetMemoryMapList;
    KSPIN_LOCK MemoryMapListLock;

    //
    // These are the fields used internally to control the replace operation.
    //

    ULONG ProcessorCount;
    ULONG ControlProcessor;
    volatile LONG EntryBarrier;
    volatile LONG ExitBarrier;
    volatile LONG InterruptDisableBarrier;
    volatile LONG QuiesceDisableBarrier;
    volatile LONG SwapPhase;
    volatile LONG SwapBarrier;
    UCHAR MirrorPhase;

    //
    // These arrays hold per-processor information about the pages modified by
    // a processor during memory copy.
    //

    ULONG_PTR CurrentStackPointer[MAXIMUM_PROC_PER_SYSTEM];
    ULONG_PTR StackLimitPointer[MAXIMUM_PROC_PER_SYSTEM];

    //
    // This structure is used to maintain the state of the device system power
    // state transition.
    //

    PNP_REPLACE_POWER_CONTEXT PowerContext;
    KEVENT BeginQuiesce;
    KEVENT QuiesceComplete;
    KEVENT BeginWake;
    KEVENT WakeComplete;
    NTSTATUS QuiesceStatus;

    //
    // These are used to maintain the context for the driver and hal interfaces.
    //

    PVOID ImageHandle;
    PVOID HalContext;
    PVOID DriverContext;
    PNP_REPLACE_DRIVER_INTERFACE DriverInterface;

    //
    // These are used to maintain timing and logging information.
    //

    ULONG FailureLine;
    PNPR_EXTENDED_STATUS ExtendedStatus;

    PNP_REPLACE_TIMING Timing;

} PNP_REPLACE_CONTEXT, *PPNP_REPLACE_CONTEXT;

typedef struct _PNPR_RESOURCE_CONTEXT {
    BOOLEAN IsTarget;
    PPNP_REPLACEABLE_UNIT Unit;
} PNPR_RESOURCE_CONTEXT, *PPNPR_RESOURCE_CONTEXT;

//
// Define the maximum number of pages that can be mapped at once during
// a memory mirroring operation.
//

#define PNPR_MAXIMUM_PAGE_RUN 0x10

//
// Define function prototypes.
//

VOID
PnpReplacePartitionUnit (
    __in PVOID ReplaceArguments
    );

NTSTATUS
PnprAddDeviceResources (
    __in PDEVICE_NODE DeviceNode,
    __in PPNPR_RESOURCE_CONTEXT ResourceContext
    );

NTSTATUS
PnprAddMemoryResources (
    __in PDEVICE_OBJECT DeviceObject,
    __inout PPNP_REPLACE_MEMORY_LIST *MemoryListOut
    );

NTSTATUS
PnprAddProcessorResources (
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG ProcessorIndex,
    __in ULONG ApicId,
    __inout PPNP_REPLACE_PROCESSOR_LIST *ProcessorListOut
    );

ULONG
PnprQueryReplaceFeatures (
    __in PPNP_REPLACE_DRIVER_INTERFACE DriverInterface,
    __in PPNP_REPLACEABLE_UNIT Spare
    );

NTSTATUS
PnprCollectResources (
    __inout PPNP_REPLACEABLE_UNIT Target,
    __inout PPNP_REPLACEABLE_UNIT Spare
    );

VOID
PnprCompleteWake(
    VOID
    );

NTSTATUS
PnprEndMirroring (
    __in ULONG PassNumber
    );

ULONG
PnprGetMillisecondCounter (
    BOOLEAN Reset
    );

NTSTATUS
PnprGetPluginDriverImagePath (
    __inout PUNICODE_STRING ImagePath
    );

NTSTATUS
PnprIdentifyUnits (
    __in PDEVICE_OBJECT TargetPdo,
    __in PDEVICE_OBJECT SparePdo,
    __out PULONG64 TargetUniqueId,
    __out PULONG64 SpareUniqueId
    );

NTSTATUS
PnprInitiateReplaceOperation (
    VOID
    );

BOOLEAN
PnprIsMemoryDevice (
    __in PDEVICE_OBJECT DeviceObject,
    __in BOOLEAN IsTarget
    );

BOOLEAN
PnprIsProcessorDevice (
    __in PDEVICE_OBJECT DeviceObject,
    __in BOOLEAN IsTarget,
    __out PULONG ProcessorNumber,
    __out PULONG ApicId
    );

BOOLEAN
PnprLegacyDeviceDriversPresent (
    VOID
    );

NTSTATUS
PnprLoadPluginDriver (
    __out PVOID *ImageHandle,
    __out PPNP_REPLACE_DRIVER_INTERFACE DriverInterface
    );

VOID
PnprLockPagesForReplace (
    VOID
    );

VOID
PnprLogFailureEvent (
    __in PDEVICE_OBJECT TargetPdo,
    __in PDEVICE_OBJECT SparePdo,
    __in NTSTATUS Status
    );

VOID
PnprLogStartEvent (
    __in PDEVICE_OBJECT TargetPdo,
    __in PDEVICE_OBJECT SparePdo
    );

VOID
PnprLogSuccessEvent (
    VOID
    );

NTSTATUS
PnprAllocateMappingReserves (
    __out PPNPR_MAPPING_RESERVE *TargetReservesOut,
    __out PPNPR_MAPPING_RESERVE *SpareReservesOut,
    __in ULONG ProcessorCount
    );

NTSTATUS
PnprInitializeMappingReserve (
    __in CSHORT MdlFlags,
    __out PPNPR_MAPPING_RESERVE Reserve
    );

VOID
PnprFreeMappingReserve (
    __inout PPNPR_MAPPING_RESERVE Reserve
    );

NTSTATUS
PnprMapPhysicalPages (
    __in PPNPR_MAPPING_RESERVE Reserve,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __inout PLARGE_INTEGER NumberOfBytes,
    __deref_out PVOID *VirtualAddress
    );

NTSTATUS
PnprMapTargetSparePhysicalPages (
    __in PHYSICAL_ADDRESS TargetPhysicalAddress,
    __in PHYSICAL_ADDRESS SparePhysicalAddress,
    __inout PLARGE_INTEGER NumberOfBytes,
    __deref_out PVOID *TargetAddress,
    __deref_out PVOID *SpareAddress
    );

NTSTATUS
PnprMarkOrMirrorPages (
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in LARGE_INTEGER NumberOfBytes,
    __in BOOLEAN Mirror
    );

NTSTATUS
PnprMirrorMarkedPages (
    VOID
    );

NTSTATUS
PnprMirrorPhysicalMemory (
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in LARGE_INTEGER NumberOfBytes
    );

NTSTATUS
PnprMmAddRange (
    __inout PLIST_ENTRY MemoryMapList,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in LARGE_INTEGER Length
    );

NTSTATUS
PnprMmConstruct (
    __in PPNP_REPLACE_MEMORY_LIST MemoryList,
    __inout PLIST_ENTRY MemoryMapList
    );

VOID
PnprMmFree (
    __inout PLIST_ENTRY MemoryMapList
    );

NTSTATUS
PnprReplaceStart (
    VOID
    );

NTSTATUS
PnprQuiesce (
    VOID
    );

NTSTATUS
PnprQuiesceDevices (
    __out PPNP_REPLACE_POWER_CONTEXT Context
    );

VOID
PnprQuiesceProcessorDpc (
    __in struct _KDPC  *Dpc,
    __in PVOID  DeferredContext,
    __in PVOID  SystemArgument1,
    __in PVOID  SystemArgument2
    );

VOID
PnprQuiesceProcessors (
    VOID
    );

VOID
PnprQuiesceWorker (
    __in PVOID Parameter
    );

NTSTATUS
PnprStartMirroring (
    VOID
    );

NTSTATUS
PnprSwap (
    VOID
    );

NTSTATUS
PnprSwapFinalize (
    VOID
    );

VOID
PnprUnloadPluginDriver (
    __in PVOID ImageHandle,
    __in PPNP_REPLACE_DRIVER_INTERFACE DriverInterface
    );

VOID
PnprUnlockPagesForReplace (
    VOID
    );

NTSTATUS
PnprWakeDevices (
    __inout PPNP_REPLACE_POWER_CONTEXT Context
    );

VOID
PnprWakeProcessors (
    VOID
    );

NTSTATUS
PnprCopyReservedMapping (
    VOID
    );

NTSTATUS
PnprGetPageDestination (
    __in PHYSICAL_ADDRESS SourceAddress,
    __out PPHYSICAL_ADDRESS DestinationAddress
    );

VOID
PnprGetStackLimits (
    __out PULONG_PTR CurrentPointer,
    __out PULONG_PTR LimitPointer
    );

NTSTATUS
PnprRecopyAddress (
    __in PVOID Address,
    __in ULONG Size
    );

NTSTATUS
PnprRecopyMappingReserve (
    __in PPNPR_MAPPING_RESERVE MapReserve
    );

NTSTATUS
PnprRecopyMirrorPages (
    VOID
    );

//
// Define an IA64 RSE flush routine that is available in the kernel but not in
// the headers.
//

#if defined(_IA64_)

VOID
RtlpFlushRSE (
    OUT PULONGLONG BackingStore,
    OUT PULONGLONG RNat
    );

#endif

extern KEVENT PnpReplaceEvent;

