/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    relations.h

Abstract:

    This file declares the routines and data structures used to manipulate
    relations list.  Relation lists are used by Plug and Play during the
    processing of device removal and ejection.

Author:

    Robert Nelson (robertn) Apr, 1998.

Revision History:

--*/

//
// An IRPLOCK allows for safe cancellation. The idea is to protect the IRP
// while the canceller is calling IoCancelIrp. This is done by wrapping the
// call in InterlockedExchange(s). The roles are as follows:
//
// Initiator/completion: Cancelable --> IoCallDriver() --> Completed
// Canceller: CancelStarted --> IoCancelIrp() --> CancelCompleted
//
// No cancellation:
//   Cancelable-->Completed
//
// Cancellation, IoCancelIrp returns before completion:
//   Cancelable --> CancelStarted --> CancelCompleted --> Completed
//
// Canceled after completion:
//   Cancelable --> Completed -> CancelStarted
//
// Cancellation, IRP completed during call to IoCancelIrp():
//   Cancelable --> CancelStarted -> Completed --> CancelCompleted
//
//  The transition from CancelStarted to Completed tells the completer to block
//  postprocessing (IRP ownership is transfered to the canceller). Similarly,
//  the canceler learns it owns IRP postprocessing (free, completion, etc)
//  during a Completed->CancelCompleted transition.
//
typedef enum {
    IRPLOCK_CANCELABLE,
    IRPLOCK_CANCEL_STARTED,
    IRPLOCK_CANCEL_COMPLETE,
    IRPLOCK_COMPLETED
} IRPLOCK;

typedef enum _DEVICE_RELATION_LEVEL {

    //
    // Related through RemovalRelations or EjectionRelations.
    //

    RELATION_LEVEL_REMOVE_EJECT,

    //
    // A dependent device.
    //

    RELATION_LEVEL_DEPENDENT,

    //
    // A child device.
    //

    RELATION_LEVEL_DIRECT_DESCENDANT
} DEVICE_RELATION_LEVEL, *PDEVICE_RELATION_LEVEL;

typedef enum _DEVICE_TAGGED_FOR_REMOVAL {
    DEVICE_UNTAGGED,
    DEVICE_TAGGED
} DEVICE_TAGGED_FOR_REMOVAL, *PDEVICE_TAGGED_FOR_REMOVAL;

//
// Forward declaration.
//

typedef struct _DEVICE_OBJECT_LIST DEVICE_OBJECT_LIST, *PDEVICE_OBJECT_LIST;

typedef enum _RELATION_LIST_ENUMERATION_MODE {

    //
    // Enumerate a list in no particular order. The list does not need to be
    // sorted before enumeration since order is ignored.
    //

    ENUMERATE_ANY_ORDER,

    //
    // Enumerate a sorted list from beginning to end. In other words, get
    // devices at the bottom of the spanning tree first (devices that do not
    // have children or dependents). The list must be sorted.
    //

    ENUMERATE_FORWARD,

    //
    // Enumerate a sorted list from the end to beginning. In other words, get
    // devices at the of the spanning tree first (devices with no parent
    // or providers). The list must be sorted.
    //

    ENUMERATE_BACKWARD,

    ENUMERATE_INVALID
} RELATION_LIST_ENUMERATION_MODE, *PRELATION_LIST_ENUMERATION_MODE;

//
// RELATION_LIST is a collection of device objects and some properties
// associated with device removal.
//
// The internal data structure, DEVICE_OBJECT_LIST, may be replaced by another
// instance; therefore, do not keep directly reference the pointer to
// manipulate the relation list; use Iop* API listed in this header file.
//

struct _RELATION_LIST {

    //
    // Internal data structure, not to be accessed directly.
    // This list can be freed and switched with a new list for some
    // operations, so do not store pointers to this internal structure.
    //

    PDEVICE_OBJECT_LIST DeviceObjectList;

    //
    // Sorted is initially FALSE. When the list is sorted, it is set to be
    // TRUE. When the list is modified in a way that the order is disrupted,
    // it is set back to FALSE. Note that removing elements from the list does
    // not change the partial ordering of elements.
    //

    BOOLEAN Sorted;
};

//
// A PENDING_RELATIONS_LIST_ENTRY is used to track relation lists for operations
// which may pend.  This includes removal when open handles exist and device
// ejection.
//
// The Link field is used to link the PENDING_RELATIONS_LIST_ENTRYs together.
//
// The DeviceObject field is the DEVICE_OBJECT to which the operation was
// originally targetted.  It will also exist as a member of the relations list.
//
// The RelationsList is a list of BusRelations, RemovalRelations, (and
// EjectionRelations in the case of eject) which are related to DeviceObject and
// its relations.
//
// The EjectIrp is pointer to the Eject IRP which has been sent to the PDO.  If
// this is a pending surprise removal then EjectIrp is not used.
//
struct _PENDING_RELATIONS_LIST_ENTRY {
    LIST_ENTRY              Link;
    WORK_QUEUE_ITEM         WorkItem;
    PPNP_DEVICE_EVENT_ENTRY DeviceEvent;
    PDEVICE_OBJECT          DeviceObject;
    PRELATION_LIST          RelationsList;
    PIRP                    EjectIrp;
    __volatile IRPLOCK      Lock;
    ULONG                   Problem;
    BOOLEAN                 ProfileChangingEject;
    BOOLEAN                 DisplaySafeRemovalDialog;
    SYSTEM_POWER_STATE      LightestSleepState;
    PDOCK_INTERFACE         DockInterface;
    BOOLEAN                 DequeuePending;
    PNP_DEVICE_DELETE_TYPE  DeleteType;
};

typedef struct _RELATION_LIST_MARKER {
    RELATION_LIST_ENUMERATION_MODE Mode;
    ULONG Counter;
} RELATION_LIST_MARKER, *PRELATION_LIST_MARKER;

//
// Functions exported to other kernel modules.
//
NTSTATUS
IopAddRelationToList(
    __in PRELATION_LIST List,
    __in PDEVICE_OBJECT DeviceObject,
    __in DEVICE_RELATION_LEVEL RelationLevel,
    __in DEVICE_TAGGED_FOR_REMOVAL Tagged
    );

PRELATION_LIST
IopAllocateRelationList(
    __in PNP_DEVICE_DELETE_TYPE OperationCode
    );

VOID
IopReverseEnumerationOrder(
    __in PRELATION_LIST List,
    __inout PRELATION_LIST_MARKER Marker
    );

VOID
IopEnumerationMarkerAdvance(
    __inout PRELATION_LIST_MARKER Marker,
    __in LONG Advance
    );

VOID
IopInitEnumerationMarker(
    __in RELATION_LIST_ENUMERATION_MODE Mode,
    __out PRELATION_LIST_MARKER Marker
    );

BOOLEAN
IopEnumerateRelations(
    __in PRELATION_LIST List,
    __inout PRELATION_LIST_MARKER Marker,
    __out PDEVICE_OBJECT *DeviceObject,
    __out_opt PDEVICE_RELATION_LEVEL RelationLevel,
    __out_opt PDEVICE_TAGGED_FOR_REMOVAL Tagged
    );

VOID
IopFreeRelationList(
    __in PRELATION_LIST List
    );

ULONG
IopGetRelationsCount(
    __in PRELATION_LIST List
    );

ULONG
IopGetRelationsTaggedCount(
    __in PRELATION_LIST List
    );

BOOLEAN
IopIsRelationInList(
    __in PRELATION_LIST List,
    __in PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IopMergeRelationLists(
    __inout PRELATION_LIST TargetList,
    __in PRELATION_LIST SourceList,
    __in BOOLEAN MergeTags
    );

NTSTATUS
IopRemoveDeviceRelationsFromList(
    __in PRELATION_LIST List
    );

NTSTATUS
IopRemoveRelationFromList(
    __in PRELATION_LIST List,
    __in PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IopRemoveCurrentRelationFromList(
    __in PRELATION_LIST List,
    __in PDEVICE_OBJECT DeviceObject,
    __in PRELATION_LIST_MARKER Marker
    );

VOID
IopSetAllRelationsTags(
    __in PRELATION_LIST List,
    __in BOOLEAN Tagged
    );

NTSTATUS
IopSetRelationsTag(
    __in PRELATION_LIST List,
    __in PDEVICE_OBJECT DeviceObject,
    __in DEVICE_TAGGED_FOR_REMOVAL Tagged
    );

BOOLEAN
IopIsDescendantNode (
    __in PRELATION_LIST List,
    __in PDEVICE_OBJECT DeviceObject
    );

_Requires_lock_held_(PiDependencyRelationsLock)
BOOLEAN
IopCheckIfMergeRequired (
    __in PRELATION_LIST PendingList,
    __in PRELATION_LIST NewList
    );

NTSTATUS
IopRelationListElementAt (
    __in PRELATION_LIST List,
    __in ULONG Index,
    __out PDEVICE_OBJECT *DeviceObject,
    __out_opt PDEVICE_RELATION_LEVEL RelationLevel,
    __out_opt PDEVICE_TAGGED_FOR_REMOVAL Tagged
    );

NTSTATUS
IopSortRelationListForRemove (
    __in PRELATION_LIST RelationList
    );

NTSTATUS
PiQueryPowerRelations (
    __in PDEVICE_NODE DevNode,
    __in BOOLEAN InternalRelations
    );

NTSTATUS
PiQueryPowerDependencyRelations(
    __in PDEVICE_NODE DevNode
    );

NTSTATUS
PnpQueryDeviceRelations (
    __in PDEVICE_OBJECT             Device,
    __in DEVICE_RELATION_TYPE       RelationType,
    __in_opt PIO_COMPLETION_ROUTINE CompletionRoutine,
    __in_opt PVOID                  Context
    );
