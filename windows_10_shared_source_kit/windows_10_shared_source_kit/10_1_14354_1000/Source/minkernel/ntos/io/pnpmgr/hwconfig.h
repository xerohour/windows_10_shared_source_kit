/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    hwconfig.h

Abstract:

    This module implements the hardware resource configuration engine.

Author:

    Santosh Jodh (santoshj) 08-02-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

//
// Forward typedefs.
//
typedef struct _REQ_DESC REQ_DESC, *PREQ_DESC;
typedef struct _REQ_LIST REQ_LIST, *PREQ_LIST;
typedef struct _REQ_ALTERNATIVE REQ_ALTERNATIVE, *PREQ_ALTERNATIVE, **PPREQ_ALTERNATIVE;
typedef struct _IOP_POOL IOP_POOL, *PIOP_POOL;

typedef enum _RESOURCE_HANDLER_TYPE {
    ResourceHandlerNull,
    ResourceTranslator,
    ResourceArbiter,
    ResourceLegacyDeviceDetection
} RESOURCE_HANDLER_TYPE;

#define PI_MAXIMUM_RESOURCE_TYPE_TRACKED 15

//
// Internal Arbiters tracking structures
// Note the first three fields of PI_RESOURCE_ARBITER_ENTRY and PI_RESOURCE_TRANSLATOR_ENTRY
// must be the same.
//

typedef struct _PI_RESOURCE_ARBITER_ENTRY {
    LIST_ENTRY          DeviceArbiterList;         // Link all the arbiters of a PDO.
    UCHAR               ResourceType;
    PARBITER_INTERFACE  ArbiterInterface;
    PDEVICE_NODE        DeviceNode;                // Owning device.
    LIST_ENTRY          ResourceList;              // Links _REQ_DESC::AlternativeTable. The memory for the non-header LIST_ENTRYs are owned by _REQ_DESC.
    LIST_ENTRY          BestResourceList;          // Point-in-time shallow copy of ResourceList
    LIST_ENTRY          BestConfig;                // Link all the arbiters which produces the best logconf. Point-in-time shallow copy of ActiveArbiterList.
    LIST_ENTRY          ActiveArbiterList;         // Link all the arbiters under testing
    UCHAR               State;
    BOOLEAN             ResourcesChanged;
} PI_RESOURCE_ARBITER_ENTRY, *PPI_RESOURCE_ARBITER_ENTRY;

//
// Define PI_RESOURCE_ARBITER_ENTRY state
//

#define PI_ARBITER_HAS_SOMETHING 1
#define PI_ARBITER_TEST_FAILED   2

//
// Internal Translator tracking structures
//

typedef struct _PI_RESOURCE_TRANSLATOR_ENTRY {
    LIST_ENTRY              DeviceTranslatorList;
    UCHAR                   ResourceType;
    PTRANSLATOR_INTERFACE   TranslatorInterface;
    PDEVICE_NODE            DeviceNode;
} PI_RESOURCE_TRANSLATOR_ENTRY, *PPI_RESOURCE_TRANSLATOR_ENTRY;

//
// REQ_LIST is an internal representation of an IO_RESOURCE_REQUIREMENTS_LIST.
//
// This is constructed by IopResourceRequirementsListToReqList()
// from a PNP_RESOURCE_REQUEST.
//

struct _REQ_LIST {
    INTERFACE_TYPE          InterfaceType;
    ULONG                   BusNumber;

    //
    // Owning request
    //

    PPNP_RESOURCE_REQUEST   Request;

    //
    // Pointer to the currently selected alternative in AlternativeTable
    //

    PPREQ_ALTERNATIVE       SelectedAlternative;

    //
    // Pointer to after the last alternative in AlternativeTable that should
    // be tested by the arbiters.
    //
    // IopRearrangeReqList() initializes this to point to the first alternative
    // in AlternativeTable that has a priority worse than LCPRI_LASTSOFTCONFIG.
    // If all alternatives have this property, all priorities are invalid and
    // this is set to NULL.
    //
    // IopSaveRestoreConfiguration() updates this to point to a selected
    // alternative that was part of a configuration (combination of different
    // devices' resource alternatives) that was accepted by the arbiters. That
    // way, only a combination of even better alternatives will be tried next
    // time.
    //

    PPREQ_ALTERNATIVE       BestAlternative;

    //
    // AlternativeTable length
    //

    ULONG                   AlternativeCount;

    //
    // Variable length, sorted list of alternatives from most to least
    // preferable (based on priority).
    //
    // This is sorted by IopRearrangeReqList().
    //

    PREQ_ALTERNATIVE        AlternativeTable[1];
};

//
// REQ_ALTERNATIVE is an internal representation of an IO_RESOURCE_LIST.
//
// Note that its REQ_DESC children are not directly equivalent to
// IO_RESOURCE_DESCRIPTORs - see the comment for REQ_DESC.
//
// For a list of PNP_RESOURCE_REQUESTs that are being arbitrated at the same
// time, a "configuration" is a combination of selected REQ_ALTERNATIVEs,
// where each alternative is from a different PNP_RESOURCE_REQUEST.
//

struct _REQ_ALTERNATIVE {
    ULONG       Priority;               // Priority for this alternative
    ULONG       Position;               // Used for sorting if Priority is identical
    PREQ_LIST   ReqList;                // Pointer to owner
    ULONG       ReqAlternativeIndex;    // Index within the table in the list
    ULONG       DescCount;              // Entry count for DescTable
    PREQ_DESC   DescTable[1];           // Variable length
};

//
// REQ_DESC represents a list of one or more translated IO_RESOURCE_DESCRIPTORs
// that are alternatives to one another (alternative in the sense of
// IO_RESOURCE_ALTERNATIVE).
//

struct _REQ_DESC {
    INTERFACE_TYPE                  InterfaceType;
    ULONG                           BusNumber;
    BOOLEAN                         ArbitrationRequired;
    UCHAR                           Reserved[3];
    PREQ_ALTERNATIVE                ReqAlternative;
    ULONG                           ReqDescIndex;
    PREQ_DESC                       TranslatedReqDesc;
    ARBITER_LIST_ENTRY              AlternativeTable;
    CM_PARTIAL_RESOURCE_DESCRIPTOR  Allocation;
    ARBITER_LIST_ENTRY              BestAlternativeTable; // Point-in-time shallow copy of AlternativeTable
    CM_PARTIAL_RESOURCE_DESCRIPTOR  BestAllocation;     // Point-in-time shallow copy of Allocation
    ULONG                           DevicePrivateCount; // DevicePrivate info
    PIO_RESOURCE_DESCRIPTOR         DevicePrivate;      // per LogConf
    union {
        PPI_RESOURCE_ARBITER_ENTRY      Arbiter;    // In original REQ_DESC
        PPI_RESOURCE_TRANSLATOR_ENTRY   Translator; // In translated REQ_DESC
    } u;
};

//
// Pool
//
struct _IOP_POOL {
    PUCHAR  PoolStart;
    ULONG   PoolSize;
};

#define IS_TRANSLATED_REQ_DESC(r)   (!((r)->ReqAlternative))

//
// Initialize arbiter entry.
//
#define IopInitializeArbiterEntryState(a) {         \
    (a)->ResourcesChanged   = FALSE;                \
    (a)->State              = 0;                    \
    InitializeListHead(&(a)->ActiveArbiterList);    \
    InitializeListHead(&(a)->BestConfig);           \
    InitializeListHead(&(a)->ResourceList);         \
    InitializeListHead(&(a)->BestResourceList);     \
}

//
// Pool management MACROs
//
#define IopInitPool(Pool,Start,Size) {      \
    (Pool)->PoolStart   = (Start);          \
    (Pool)->PoolSize    = (Size);           \
    RtlZeroMemory(Start, Size);             \
}
#define IopAllocPool(M,P,S) {                                       \
    *(M)            = (PVOID)(P)->PoolStart;                        \
    PNP_ASSERT((P)->PoolStart + (S) <= (P)->PoolStart + (P)->PoolSize); \
    (P)->PoolStart  += (S);                                         \
}

#if DBG

typedef struct {
    PDEVICE_NODE                    devnode;
    CM_PARTIAL_RESOURCE_DESCRIPTOR  resource;
} PNPRESDEBUGTRANSLATIONFAILURE;

VOID
IopCheckDataStructuresWorker(
    __in PDEVICE_NODE Device
    );

VOID
IopCheckDataStructures(
    __in PDEVICE_NODE DeviceNode
    );

VOID
PiDumpArbiterListEntry (
    __in PARBITER_LIST_ENTRY  ArbiterListEntry,
    __in BOOLEAN              NewOwner
    );

VOID
PiDumpArbiterList (
    __in PLIST_ENTRY ArbitrationList
    );

#define IopStopOnTimeout()                  (IopUseTimeout)

#define IopRecordTranslationFailure(d,s) {              \
    if (PnpResDebugTranslationFailureCount) {           \
        PnpResDebugTranslationFailureCount--;           \
        PnpResDebugTranslationFailure->devnode = d;     \
        PnpResDebugTranslationFailure->resource = s;    \
        PnpResDebugTranslationFailure++;                \
    }                                                   \
}

#else

#define IopCheckDataStructures(x)
#define IopStopOnTimeout()                  1
#define IopRecordTranslationFailure(d,s)

#define PiDumpArbiterListEntry(a, n)
#define PiDumpArbiterList(l)

#endif // DBG

//
// The minimum time between progress messages in
// PnpFindBestConfigurationWorker(), in milliseconds.
//
// Applicable only if IopStopOnTimeout() == FALSE.
//
#define PnpFindBestConfigurationDebugSpewTimeout 30000

NTSTATUS
IopTestConfiguration (
    __in PPNP_RESOURCE_REQUEST RequestTable,
    __in ULONG RequestTableCount,
    __inout PLIST_ENTRY ArbiterList
    );

NTSTATUS
IopRetestConfiguration (
    __in PPNP_RESOURCE_REQUEST RequestTable,
    __in ULONG RequestTableCount,
    __inout PLIST_ENTRY ArbiterList
    );

NTSTATUS
IopCommitConfiguration (
    __inout  PLIST_ENTRY ArbiterList
    );

ULONG64
IopCountMaximumConfigurations (
    __in_ecount(RequestTableCount)  PPNP_RESOURCE_REQUEST    RequestTable,
    __in                            ULONG                    RequestTableCount
);

VOID
PnpSelectFirstConfiguration (
    __in_ecount(RequestTableCount) PPNP_RESOURCE_REQUEST    RequestTable,
    __in                           ULONG                    RequestTableCount,
    __inout                        PLIST_ENTRY              ActiveArbiterList,
    __in                           ULONG                    Phase
    );

BOOLEAN
IopSelectNextConfiguration (
    __in_ecount(RequestTableCount)       PPNP_RESOURCE_REQUEST    RequestTable,
    __in                                 ULONG                    RequestTableCount,
    __inout                              PLIST_ENTRY              ActiveArbiterList
    );

VOID
IopCleanupSelectedConfiguration (
    __in_ecount(RequestTableCount) PPNP_RESOURCE_REQUEST    RequestTable,
    __in ULONG                    RequestTableCount
    );

ULONG
IopComputeConfigurationPriority (
    __in_ecount(RequestTableCount) PPNP_RESOURCE_REQUEST    RequestTable,
    __in ULONG                    RequestTableCount
    );

VOID
IopSaveRestoreConfiguration (
    __in_ecount(RequestTableCount)      PPNP_RESOURCE_REQUEST   RequestTable,
    __in      ULONG                   RequestTableCount,
    __inout  PLIST_ENTRY             ArbiterList,
    __in      BOOLEAN                 Save
    );

VOID
IopAddRemoveReqDescs (
    __in_ecount(ReqDescCount)      PREQ_DESC   *ReqDescTable,
    __in      ULONG       ReqDescCount,
    __inout_opt PLIST_ENTRY ActiveArbiterList,
    __in      BOOLEAN     Add
    );

NTSTATUS
PnpFindBestConfiguration (
    __in_ecount(RequestTableCount) PPNP_RESOURCE_REQUEST   RequestTable,
    __in      ULONG                   RequestTableCount,
    __inout  PLIST_ENTRY             ActiveArbiterList
    );

NTSTATUS
PnpFindBestConfigurationWorker (
    __in_ecount(RequestTableCount)  PPNP_RESOURCE_REQUEST   RequestTable,
    __in                            ULONG                   RequestTableCount,
    __inout                         PLIST_ENTRY             ActiveArbiterList,
    __in                            ULONG                   Phase
    );

VOID
IopFreeReqAlternative (
    __in PREQ_ALTERNATIVE ReqAlternative
    );

VOID
IopFreeReqList (
    __in PREQ_LIST ReqList
    );

NTSTATUS
IopResourceRequirementsListToReqList(
    __in PPNP_RESOURCE_REQUEST Request,
    __out PVOID *ResReqList
    );

VOID
IopRearrangeReqList (
    __in PREQ_LIST ReqList
    );

int
__cdecl
IopCompareReqAlternativePriority (
    __in const void *arg1,
    __in const void *arg2
    );

NTSTATUS
IopSetupArbiterAndTranslators(
    __in PREQ_DESC ReqDesc
    );

BOOLEAN
IopFindResourceHandlerInfo(
    __in RESOURCE_HANDLER_TYPE    HandlerType,
    __in PDEVICE_NODE             DeviceNode,
    __in UCHAR                    ResourceType,
    __out PVOID                   *HandlerEntry
    );

NTSTATUS
IopCallArbiter(
    __in PPI_RESOURCE_ARBITER_ENTRY ArbiterEntry,
    __in ARBITER_ACTION Command,
    __in_opt PVOID Input1,
    __in_opt PVOID Input2,
    __in_opt PVOID Input3
    );

NTSTATUS
IopTranslateAndAdjustReqDesc(
    __in PREQ_DESC ReqDesc,
    __in PPI_RESOURCE_TRANSLATOR_ENTRY TranslatorEntry,
    __out PREQ_DESC *TranslatedReqDesc
    );

NTSTATUS
PnpLookupArbitersNewResources (
    __in_ecount(RequestTableCount) PPNP_RESOURCE_REQUEST                RequestTable,
    __in ULONG                                RequestTableCount,
    __in PDEVICE_NODE                         DeviceNode,
    __in UCHAR                                ResourceType,
    __out PCM_PARTIAL_RESOURCE_DESCRIPTOR     *AllocateFrom,
    __out PULONG                              AllocateFromCount
    );

NTSTATUS
IopParentToRawTranslation(
    __inout PREQ_DESC ReqDesc
    );

NTSTATUS
IopChildToRootTranslation(
    __in_opt PDEVICE_NODE DeviceNode,
    __in INTERFACE_TYPE InterfaceType,
    __in ULONG BusNumber,
    __in ARBITER_REQUEST_SOURCE ArbiterRequestSource,
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Source,
    __out PCM_PARTIAL_RESOURCE_DESCRIPTOR *Target
    );

