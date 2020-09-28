/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    depend.h

Abstract:

    This module contains the private (internal) header file for PNP
        dependency relations.

Author:

    Paresh Maisuria (pareshm) 9-Nov-2006


Revision History:

--*/

#pragma once

#define PNP_DO_TO_DEPENDENCY_NODE(D) \
    ((PPNP_DEPENDENCY_NODE)((D)? (D)->DeviceObjectExtension->DependencyNode : NULL))

//
// Flags for PNP_DEPENDENCY_NODE.
//

#define PNP_DEPENDENCY_NODE_VISITED    0x1
#define PNP_DEPENDENCY_NODE_PROCESSING 0x2

typedef struct _PNP_DEPENDENCY_NODE {
    LIST_ENTRY ListEntry;
    _Write_guarded_by_(PiDependencyEdgeWriteLock) LIST_ENTRY Providers;
    _Write_guarded_by_(PiDependencyEdgeWriteLock) LIST_ENTRY Dependents;
    PDEVICE_OBJECT DeviceObject;
    LIST_ENTRY BindingIds;
    LIST_ENTRY QueueListEntry;
    ULONG ReferenceCount;
    ULONG Flags;
} PNP_DEPENDENCY_NODE, *PPNP_DEPENDENCY_NODE;

typedef enum _PNP_BINDING_RESOLUTION_TYPE {
    ResolvedWithPdo,
    ResolvedWithLabel
} PNP_BINDING_RESOLUTION_TYPE, *PPNP_BINDING_RESOLUTION_TYPE;

typedef enum {
    ProviderList,
    DependentList
} PNP_DEPENDENCY_EDGE_LIST_TYPE, *PPNP_DEPENDENCY_EDGE_LIST_TYPE;

typedef struct _PNP_BINDING_ID {
    LIST_ENTRY ListEntry;
    UNICODE_STRING Label;
} PNP_BINDING_ID, *PPNP_BINDING_ID;

typedef struct _PNP_BINDING_RESOLUTION {
    PNP_BINDING_RESOLUTION_TYPE Type;
    union {
        PDEVICE_OBJECT Pdo;
        PCUNICODE_STRING Label;
    } u;
} PNP_BINDING_RESOLUTION, *PPNP_BINDING_RESOLUTION;

typedef struct _PNP_BINDING_REQUEST {
    PNP_BINDING_RESOLUTION Provider;
    ULONG DependencyTypes;
} PNP_BINDING_REQUEST, *PPNP_BINDING_REQUEST;

typedef struct _PNP_BINDING_REQUEST_ENTRY {
    LIST_ENTRY ListEntry;
    PNP_BINDING_REQUEST Request;
} PNP_BINDING_REQUEST_ENTRY, *PPNP_BINDING_REQUEST_ENTRY;

typedef struct _PNP_DEPENDENCY_EDGE {

    //
    // Used by the dependent node.
    //

    _Write_guarded_by_(PiDependencyEdgeWriteLock) LIST_ENTRY DependentListEntry;

    //
    // Used by the provider node.
    //

    _Write_guarded_by_(PiDependencyEdgeWriteLock) LIST_ENTRY ProviderListEntry;
    _Write_guarded_by_(PiDependencyEdgeWriteLock) PPNP_DEPENDENCY_NODE Provider;
    _Write_guarded_by_(PiDependencyEdgeWriteLock) PPNP_DEPENDENCY_NODE Dependent;
    _Write_guarded_by_(PiDependencyEdgeWriteLock) ULONG DependencyTypes;

    //
    // List of PNP_BINDING_REQUEST_ENTRY to keep track of the original request
    // to establish a dependency. This is used to "undo" the binding operation
    // when the provider device is removed.
    //

    LIST_ENTRY Requests;
    BOOLEAN DeletePending;
} PNP_DEPENDENCY_EDGE, *PPNP_DEPENDENCY_EDGE;

//
// PiDependencyRelationsLock - Serializes all requests to set or modify
// dependency relations between devices.
// Acquire with PnpAcquireDependencyRelationsLock and release with
// PnpReleaseDependencyRelationsLock.
//

ERESOURCE PiDependencyRelationsLock;

//
// PiDependencyEdgeWriteLock is required to be held when the graph is being modified.
// For example: adding, modifying and removing edges from dependency nodes.
//

KSPIN_LOCK PiDependencyEdgeWriteLock;

NTSTATUS
PiDeviceDependencyInit(
    VOID
    );

BOOLEAN
PnpCheckForActiveDependencies (
    __in PDEVICE_NODE DeviceNode,
    __in ULONG DependencyTypes
    );

_Acquires_lock_(IopDeviceTreeLock)
_When_(ExclusiveLock != FALSE, _Acquires_exclusive_lock_(PiDependencyRelationsLock))
_When_(ExclusiveLock == FALSE, _Acquires_shared_lock_(PiDependencyRelationsLock))
VOID
PnpAcquireDependencyRelationsLock(
    __in BOOLEAN ExclusiveLock
    );

_Releases_lock_(PiDependencyRelationsLock)
_Releases_lock_(IopDeviceTreeLock)
VOID
PnpReleaseDependencyRelationsLock(
    );

VOID
PnpResetDependencyRelations (
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG Flags
    );

VOID
PnpNewDeviceNodeDependencyCheck (
    __in PDEVICE_NODE DeviceNode
    );

VOID
PnpStartedDeviceNodeDependencyCheck (
    __in PDEVICE_NODE DeviceNode
    );

NTSTATUS
PnpSurpriseRemovedDeviceNodeDependencyCheck (
    __in PDEVICE_NODE DeviceNode
    );

_When_(ExclusiveLock != FALSE, _Requires_exclusive_lock_held_(PiDependencyRelationsLock))
_When_(ExclusiveLock == FALSE, _Requires_lock_held_(PiDependencyRelationsLock))
VOID
PiAssertDependencyRelationsLockHeld (
    __in BOOLEAN ExclusiveLock
    );

_Requires_lock_held_(PiDependencyRelationsLock)
PLIST_ENTRY
PiGetDependentList (
    __in PDEVICE_OBJECT DeviceObject
    );

_Requires_lock_held_(PiDependencyRelationsLock)
PLIST_ENTRY
PiGetProviderList (
    __in PDEVICE_OBJECT DeviceObject
    );

_Requires_lock_held_(PiDependencyRelationsLock)
VOID
PiEnumerateDependentListEntry (
    __in PLIST_ENTRY ListEntry,
    __out PDEVICE_OBJECT *Dependent,
    __out_opt PULONG DependencyTypes
    );

_Requires_lock_held_(PiDependencyRelationsLock)
VOID
PiEnumerateProviderListEntry (
    __in PLIST_ENTRY ListEntry,
    __out PDEVICE_OBJECT *Provider,
    __out_opt PULONG DependencyTypes
    );

PPNP_DEPENDENCY_EDGE
PiListEntryToDependencyEdge (
    __in PLIST_ENTRY ListEntry,
    __in PNP_DEPENDENCY_EDGE_LIST_TYPE Mode
    );

_Requires_lock_not_held_(PiDependencyRelationsLock)
NTSTATUS
PnpGetDeviceDependencyList(
    __in PDEVICE_NODE DeviceNode,
    __in PNP_DEPENDENCY_EDGE_LIST_TYPE Mode,
    __out_ecount(DependencyListLength) PZZWSTR DependencyList,
    __in ULONG DependencyListLength,
    __out_opt PULONG RequiredLength
    );
