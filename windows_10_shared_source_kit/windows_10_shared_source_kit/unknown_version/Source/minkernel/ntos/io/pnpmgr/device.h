/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    device.h

Abstract:

    This module contains the private (internal) header file for a PNP device
    representation.

Author:

    Santosh Jodh (santoshj) 12-Jul-2004


Revision History:

--*/

#pragma once

#include "pnppo.h"

//
// DNF_MAKEUP - this devnode's device is created and owned by PnP manager
//

#define DNF_MADEUP                                  0x00000001

//
// DNF_DUPLICATE - this devnode's device is a duplicate of another enumerate PDO
//

#define DNF_DUPLICATE                               0x00000002

//
// DNF_HAL_NODE - a flag to indicate which device node is the root node created by
// the hal
//

#define DNF_HAL_NODE                                0x00000004

//
// DNF_REENUMERATE - needs to be reenumerated
//

#define DNF_REENUMERATE                             0x00000008

//
// DNF_ENUMERATED - used to track enumeration in IopEnumerateDevice()
//

#define DNF_ENUMERATED                              0x00000010

//
// Singal that we need to send driver query id irps
//

#define DNF_IDS_QUERIED                             0x00000020

//
// DNF_HAS_BOOT_CONFIG - the device has resource assigned by BIOS.  It is considered
//    pseudo-started and need to participate in rebalance.
//

#define DNF_HAS_BOOT_CONFIG                         0x00000040

//
// DNF_BOOT_CONFIG_RESERVED - Indicates the BOOT resources of the device are reserved.
//

#define DNF_BOOT_CONFIG_RESERVED                    0x00000080

//
// DNF_NO_RESOURCE_REQUIRED - this devnode's device does not require resource.
//

#define DNF_NO_RESOURCE_REQUIRED                    0x00000100

//
// DNF_RESOURCE_REQUIREMENTS_NEED_FILTERED - to distinguished the
//      DeviceNode->ResourceRequirements is a filtered list or not.
//

#define DNF_RESOURCE_REQUIREMENTS_NEED_FILTERED     0x00000200

//
// DNF_RESOURCE_REQUIREMENTS_CHANGED - Indicates the device's resource
//      requirements list has been changed.
//

#define DNF_RESOURCE_REQUIREMENTS_CHANGED           0x00000400

//
// DNF_NON_STOPPED_REBALANC - indicates the device can be restarted with new
//      resources without being stopped.
//

#define DNF_NON_STOPPED_REBALANCE                   0x00000800

//
// The device's controlling driver is a legacy driver
//

#define DNF_LEGACY_DRIVER                           0x00001000

//
// This corresponds to the user-mode CM_PROB_WILL_BE_REMOVED problem value and
// the DN_WILL_BE_REMOVED status flag.
//

#define DNF_HAS_PROBLEM                             0x00002000

//
// DNF_HAS_PRIVATE_PROBLEM - indicates this device reported PNP_DEVICE_FAILED
//  to a IRP_MN_QUERY_PNP_DEVICE_STATE without also reporting
//  PNP_DEVICE_RESOURCE_REQUIREMENTS_CHANGED.
//

#define DNF_HAS_PRIVATE_PROBLEM                     0x00004000

//
// DNF_HARDWARE_VERIFICATION is set on device nodes that have hardware
// verification (probably via WHQL applet).
//

#define DNF_HARDWARE_VERIFICATION                   0x00008000

//
// DNF_DEVICE_GONE is set when a pdo is no longer returned in a query bus
// relations.  It will then be processed as a surprise remove if started.
// This flag is used to better detect when a device is resurrected, and when
// processing surprise remove, to determine if the devnode should be removed
// from the tree.
//

#define DNF_DEVICE_GONE                             0x00010000

//
// DNF_LEGACY_RESOURCE_DEVICENODE is set for device nodes created for legacy
// resource allocation.
//

#define DNF_LEGACY_RESOURCE_DEVICENODE              0x00020000

//
// DNF_NEEDS_REBALANCE is set for device nodes that trigger rebalance.
//

#define DNF_NEEDS_REBALANCE                         0x00040000

//
// DNF_LOCKED_FOR_EJECT is set on device nodes that are being ejected or are
// related to a device being ejected.
//

#define DNF_LOCKED_FOR_EJECT                        0x00080000

//
// DNF_DRIVER_BLOCKED is set on device nodes that use one or more drivers that
// have been blocked from loading.
//

#define DNF_DRIVER_BLOCKED                          0x00100000

//
// DNF_CHILD_WITH_INVALID_ID is set on device nodes that has one or more children
// that have invalid id(s).
//

#define DNF_CHILD_WITH_INVALID_ID                   0x00200000

//
// DNF_ASYNC_START_NOT_SUPPORTED is set on device nodes that do not support async starts.
//

#define DNF_ASYNC_START_NOT_SUPPORTED               0x00400000

//
// DNF_ASYNC_ENUMERATION_NOT_SUPPORTED is set on device nodes that do not support async
// enumeration.
//

#define DNF_ASYNC_ENUMERATION_NOT_SUPPORTED         0x00800000

//
// DNF_LOCKED_FOR_REBALANCE is set on a device during rebalance after
// acquiring the DeviceLock.
//

#define DNF_LOCKED_FOR_REBALANCE                    0x01000000

//
// DNF_UNINSTALLED is set on the target device during a query remove/remove.
//

#define DNF_UNINSTALLED                             0x02000000

//
// DNF_NO_LOWER_DEVICE_FILTERS is set on a device when no lower device filters
// entry has been found in the registry.
//

#define DNF_NO_LOWER_DEVICE_FILTERS                 0x04000000

//
// DNF_NO_LOWER_CLASS_FILTERS is set on a device when no lower class filters
// entry has been found in the registry.
//

#define DNF_NO_LOWER_CLASS_FILTERS                  0x08000000

//
// DNF_NO_SERVICE is set on a device when no service entry is found in the
// registry.
//

#define DNF_NO_SERVICE                              0x10000000

//
// DNF_NO_UPPER_DEVICE_FILTERS is set on a device when no upper device filters
// entry has been found in the registry.
//

#define DNF_NO_UPPER_DEVICE_FILTERS                 0x20000000

//
// DNF_NO_UPPER_CLASS_FILTERS is set on a device when no upper class filters
// entry has been found in the registry.
//

#define DNF_NO_UPPER_CLASS_FILTERS                  0x40000000

//
// DNF_WAITING_FOR_FDO is set on a reported device such that enumeration is
// held off until the driver attaches its FDO.
//

#define DNF_WAITING_FOR_FDO                         0x80000000

//
// DRIVER_FLAGS_MASK supplies a mask of all of query results for drivers.
//

#define DRIVER_FLAGS_MASK DNF_NO_LOWER_DEVICE_FILTERS | \
                          DNF_NO_LOWER_CLASS_FILTERS | \
                          DNF_NO_SERVICE | \
                          DNF_NO_UPPER_DEVICE_FILTERS | \
                          DNF_NO_UPPER_CLASS_FILTERS
//
// This corresponds to the user-mode the DN_WILL_BE_REMOVED status flag.
//

#define DNUF_WILL_BE_REMOVED                        0x00000001

//
// This corresponds to the user-mode DN_NO_SHOW_IN_DM status flag.
//

#define DNUF_DONT_SHOW_IN_UI                        0x00000002

//
// This flag is set when user-mode lets us know that a reboot is required
// for this device.
//

#define DNUF_NEED_RESTART                           0x00000004

//
// This flag is set to let the user-mode know when a device can be disabled
// it is still possible for this to be TRUE, yet disable to fail, as it's
// a polled flag (see also PNP_DEVICE_NOT_DISABLEABLE)
//

#define DNUF_NOT_DISABLEABLE                        0x00000008

//
// Flags used during shutdown when the IO Verifier is trying to remove all
// PNP devices.
//
// DNUF_SHUTDOWN_QUERIED is set when we issue the QueryRemove to a devnode.
//
// DNUF_SHUTDOWN_SUBTREE_DONE is set once we've issued the QueryRemove to all
// a Devnodes descendants.
//
#define DNUF_SHUTDOWN_QUERIED                       0x00000010
#define DNUF_SHUTDOWN_SUBTREE_DONE                  0x00000020

//
// This corresponds to the user-mode DN_DEVICE_DISCONNECTED status flag.
//

#define DNUF_DEVICE_DISCONNECTED                    0x00000040

//
// NUMA node information is uninitialized.
//

#define PNP_NUMA_NODE_UNKNOWN   ((ULONG)-1)

//
// NUMA node information is not available.
//

#define PNP_NUMA_NODE_NONE      ((ULONG)-2)

//
// This flag indicates that the device node has been queried for overrides.
//
#define DNOF_OVERRIDES_QUERIED                      0x00000001

//
// This flag is set on device nodes that have location overrides.
//
#define DNOF_HAS_LOCATION_OVERRIDES                 0x00000002

//
// This flag is set on device nodes that have location overrides for their
// child device nodes.
//
#define DNOF_HAS_CHILD_LOCATION_OVERRIDES           0x00000004

//
// DNSF_CONFIGURED_AWAITING_START is set on devices that have just been through
// device configuration and are waiting to be started for the first time.
//
#define DNSF_CONFIGURED_AWAITING_START              0x00000001


//
// The power flags are set when the device is activated prior to the
// corresponding PNP transition via PoFxActivateDevice.  The flag is cleared
// when the device is marked idle after the transition has completed.
//

#define DNSF_POFX_QUERY_REMOVE                      0x00000004
#define DNSF_POFX_SURPRISE_REMOVE                   0x00000008
#define DNSF_POFX_REMOVE                            0x00000010
#define DNSF_POFX_QUERY_STOP                        0x00000020
#define DNSF_POFX_REQUIREMENTS_CHANGED              0x00000040
#define DNSF_POFX_BUS_RELATIONS                     0x00000080
#define DNSF_POFX_QUERY_DEVICE_STATE                0x00000100
#define DNSF_POFX_START_ENUM                        0x00000200
#define DNSF_POFX_FLAGS \
    (DNSF_POFX_QUERY_REMOVE | \
     DNSF_POFX_SURPRISE_REMOVE | \
     DNSF_POFX_REMOVE | \
     DNSF_POFX_QUERY_STOP | \
     DNSF_POFX_REQUIREMENTS_CHANGED | \
     DNSF_POFX_BUS_RELATIONS | \
     DNSF_POFX_QUERY_DEVICE_STATE | \
     DNSF_POFX_START_ENUM)

//
// Indicate that this device arrived during boot and may need to be restarted
// in order to pick up the best matching driver.
//

#define DNSF_ARRIVED_DURING_BOOT                    0x00001000

//
// Tag for PreviousParent field of DEVICE_NODE.
// This tag will be set if the parent has DeletedChildren field
// incremented for the child device. The tag is removed when
// the count is decremented.
//

#define PNP_DN_PREVIOUS_PARENT_REFERENCED 0x1

//
// Macro for getting the previous parent without any tags.
//

#define PNP_DN_PREVIOUS_PARENT(N) \
    (PDEVICE_NODE) ((ULONG_PTR) (N->PreviousParent) & \
                    ~PNP_DN_PREVIOUS_PARENT_REFERENCED)

TRIAGE_SIZE_CHECK(DEVICE_NODE);
TRIAGE_FIELD_CHECK(DEVICE_NODE, Sibling);
TRIAGE_FIELD_CHECK(DEVICE_NODE, Child);
TRIAGE_FIELD_CHECK(DEVICE_NODE, Parent);
TRIAGE_FIELD_CHECK(DEVICE_NODE, LastChild);
TRIAGE_FIELD_CHECK(DEVICE_NODE, PhysicalDeviceObject);
TRIAGE_FIELD_CHECK(DEVICE_NODE, InstancePath);
TRIAGE_FIELD_CHECK(DEVICE_NODE, ServiceName);
TRIAGE_FIELD_CHECK(DEVICE_NODE, PendingIrp);
TRIAGE_FIELD_CHECK(DEVICE_NODE, FxDevice);

typedef NTSTATUS (*PENUM_CALLBACK)(
    __in PDEVICE_NODE DeviceNode,
    __in PVOID Context
    );

//
// IopNumberDeviceNodes - Number of device nodes in the system.
//
__volatile extern ULONG IopNumberDeviceNodes;


//
// A device Object is a PDO iff it has a non NULL device node (aka set by
// plug and play during a query device relations.
//
#define IS_PDO(d) \
    (((d) != NULL) && \
    (NULL != (d)->DeviceObjectExtension->DeviceNode) && \
    (!(((PDEVICE_NODE)(d)->DeviceObjectExtension->DeviceNode)->Flags & DNF_LEGACY_RESOURCE_DEVICENODE)))

#define ASSERT_PDO(d) \
    do { \
        if (!IS_PDO(d))  { \
            PNP_SAVE_DEVICEOBJECT_TO_TRIAGE_DUMP(d);                                         \
            KeBugCheckEx(PNP_DETECTED_FATAL_ERROR, PNP_ERR_INVALID_PDO, (ULONG_PTR)d, 0, 0); \
        } \
    } \
    while (0)

//
// IopReleaseBootResources can only be called for non ROOT enumerated devices
//
#define IopReleaseBootResources(DeviceNode) {                       \
    PNP_ASSERT(((DeviceNode)->Flags & DNF_MADEUP) == 0);            \
    PnpReleaseResourcesInternal(DeviceNode);                        \
    PipClearDevNodeFlags(DeviceNode, DNF_HAS_BOOT_CONFIG | DNF_BOOT_CONFIG_RESERVED); \
    if ((DeviceNode)->BootResources) {                              \
        ExFreePool((DeviceNode)->BootResources);                    \
        (DeviceNode)->BootResources = NULL;                         \
    }                                                               \
}

#if DBG

#define PP_DEVNODESTATE_NAME(s) ((s >= DeviceNodeUnspecified && s <= MaxDeviceNodeState)? PpStateToNameTable[(s) - DeviceNodeUnspecified] : PpStateToNameTable[0])

extern const char *PpStateToNameTable[];

#else

#define PP_DEVNODESTATE_NAME(s)

#endif

//
// Save failure status info.
//

#if DBG

#define PNP_SAVE_FAILURE_INFO(DeviceNode, Status) \
    (DeviceNode)->FailureStatus = (Status)

#else

#define PNP_SAVE_FAILURE_INFO(DeviceNode, Status)

#endif

typedef struct _ENUM_CONTEXT{
    PENUM_CALLBACK CallersCallback;
    PVOID CallersContext;
} ENUM_CONTEXT, *PENUM_CONTEXT;

NTSTATUS
PipForAllDeviceNodesCallback(
    __in PDEVICE_NODE DeviceNode,
    __in PVOID Context
    );

LOGICAL
PipAreDriversLoadedWorker(
    __in PNP_DEVNODE_STATE    CurrentNodeState,
    __in PNP_DEVNODE_STATE    PreviousNodeState
    );

VOID
IopReleaseResources (
    __in PDEVICE_NODE DeviceNode
    );

NTSTATUS
IopWriteAllocatedResourcesToRegistry (
    __in PDEVICE_NODE DeviceNode,
    __in_bcount_opt(Length) PCM_RESOURCE_LIST ResourceList,
    __in_opt ULONG Length
    );

NTSTATUS
IopReleaseDeviceResources(
    __in PDEVICE_NODE DeviceNode,
    __in LOGICAL ReserveResources
    );

VOID
IopUncacheInterfaceInformation(
    __in PDEVICE_OBJECT DeviceObject,
    __in LOGICAL        AcquireLock
    );

LOGICAL
PipAreDriversLoaded(
    __in PDEVICE_NODE DeviceNode
    );

LOGICAL
PipIsDevNodeDNStarted(
    __in PDEVICE_NODE DeviceNode
    );

LOGICAL
PipIsDevNodeRootEnumerated(
    __in PDEVICE_NODE DeviceNode
    );

VOID
PipClearDevNodeProblem(
    __in PDEVICE_NODE DeviceNode
    );

VOID
PipSetDevNodeProblem(
    __in PDEVICE_NODE DeviceNode,
    __in ULONG        Problem,
    __in NTSTATUS     ProblemStatus
    );

#define PipIsRequestPending(devnode)   FALSE

#define PipDoesDevNodeHaveResources(devnode)                        \
        ((devnode)->ResourceList != NULL || (devnode)->BootResources != NULL || \
        ((devnode)->Flags & DNF_HAS_BOOT_CONFIG) != 0)


#define PipDoesDevNodeHaveProblem(devnode)                          \
        ((devnode)->Flags & (DNF_HAS_PROBLEM | DNF_HAS_PRIVATE_PROBLEM))

#define PipIsDevNodeProblem(devnode, problem)                       \
        (((devnode)->Flags & DNF_HAS_PROBLEM) && (devnode)->Problem == (problem))

#define PipIsDevNodeDeleted(d)                                      \
    ((d)->State == DeviceNodeDeletePendingCloses ||(d)->State == DeviceNodeDeleted)

#define IopDeviceNodeFlagsToCapabilities(DeviceNode) \
     ((PDEVICE_CAPABILITIES) (((PUCHAR) (&(DeviceNode)->CapabilityFlags)) - \
                              FIELD_OFFSET(DEVICE_CAPABILITIES, Version) - \
                              FIELD_SIZE(DEVICE_CAPABILITIES, Version)))

VOID
PipSetDevNodeState(
    __in  PDEVICE_NODE        DeviceNode,
    __in  PNP_DEVNODE_STATE   State,
    __out_opt PNP_DEVNODE_STATE  *OldState
    );

VOID
PipRestoreDevNodeState(
    __in PDEVICE_NODE DeviceNode
    );

VOID PipSetDevNodeFlags(
    __in PDEVICE_NODE DeviceNode,
    __in ULONG Flags
    );

VOID PipClearDevNodeFlags(
    __in PDEVICE_NODE DeviceNode,
    __in ULONG Flags
    );

VOID PipSetDevNodeUserFlags(
    __in PDEVICE_NODE DeviceNode,
    __in ULONG Flags
    );

VOID PipClearDevNodeUserFlags(
    __in PDEVICE_NODE DeviceNode,
    __in ULONG Flags
    );

LOGICAL
PipIsProblemReadonly(
    __in PDEVICE_NODE DeviceNode,
    __in ULONG Problem
    );

VOID
PpDevNodeInsertIntoTree(
    __in PDEVICE_NODE     ParentNode,
    __in PDEVICE_NODE     DeviceNode
    );

BOOLEAN
PpDevNodeRemoveFromTree(
    __in PDEVICE_NODE     DeviceNode
    );

NTSTATUS
PipAllocateDeviceNode(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __out PDEVICE_NODE *DeviceNode
    );

NTSTATUS
PipForAllDeviceNodes(
    __in PENUM_CALLBACK Callback,
    __in_opt PVOID Context
    );

NTSTATUS
PipForDeviceNodeSubtree(
    __in PDEVICE_NODE     DeviceNode,
    __in PENUM_CALLBACK   Callback,
    __in_opt PVOID            Context
    );

NTSTATUS
PipForAllChildDeviceNodes(
    __in PDEVICE_NODE Parent,
    __in PENUM_CALLBACK Callback,
    __in PVOID Context
    );

NTSTATUS
PnpAllocateDeviceInstancePath (
    __in PDEVICE_NODE   DeviceNode,
    __in ULONG          Size
    );

VOID
PnpFreeDeviceInstancePath (
    __in PDEVICE_NODE   DeviceNode
    );

VOID
PnpCopyDeviceInstancePath (
    __in PDEVICE_NODE       DeviceNode,
    __in PUNICODE_STRING    InstancePath
    );

//
// This macro returns true if the device has information about the closest NUMA node.
//

#define PNP_DEVICE_HAS_NUMA_NODE_INFO(d)    \
    ((d)->NumaNodeIndex != PNP_NUMA_NODE_UNKNOWN && \
        (d)->NumaNodeIndex != PNP_NUMA_NODE_NONE)


KINLINE
LOGICAL
PnpSetDeviceAffinityThread (
    __in PDEVICE_OBJECT Device,
    __out PGROUP_AFFINITY PreviousAffinity
    )
{

    PDEVICE_NODE DeviceNode;
    GROUP_AFFINITY Affinity;

    PNP_ASSERT(Device);

    DeviceNode = PP_DO_TO_DN(Device);
    if (DeviceNode && PNP_DEVICE_HAS_NUMA_NODE_INFO(DeviceNode)) {
        if (DeviceNode->NumaNodeIndex < KeNumberNodes) {
            KeQueryNodeActiveAffinity((USHORT)DeviceNode->NumaNodeIndex,
                                      &Affinity,
                                      NULL);

            KeAndGroupAffinityEx((PKAFFINITY_EX)&KeActiveProcessors,
                                 &Affinity,
                                 &Affinity);

            if (Affinity.Mask != 0) {
                KeSetSystemGroupAffinityThread(&Affinity, PreviousAffinity);
                return TRUE;
            }
        }
    }

    return FALSE;
}

KINLINE
ULONG
PnpGetDeviceCount (
    VOID
    )
{
    return IopNumberDeviceNodes;
}

KINLINE
PUNICODE_STRING
PnpGetDeviceInstancePath (
    __in PDEVICE_NODE DeviceNode
    )
{
    return &DeviceNode->InstancePath;
}

KINLINE
LOGICAL
PnpIsDeviceInstancePathEmpty (
    __in PDEVICE_NODE DeviceNode
    )
{
    return (PnpGetDeviceInstancePath(DeviceNode)->Buffer == NULL)? TRUE : FALSE;
}

LOGICAL
PipIsDevNodeEffectivelyRemoved(
    _In_ PDEVICE_NODE DeviceNode
    );


