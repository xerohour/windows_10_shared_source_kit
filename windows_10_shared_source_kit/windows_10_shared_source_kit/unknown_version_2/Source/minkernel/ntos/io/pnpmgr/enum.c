/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    enum.c

Abstract:

    This module contains routines to perform device enumeration

Author:

    Shie-Lin Tzong (shielint) Sept. 5, 1996.

--*/

#include "pnpmgrp.h"
#pragma hdrstop

#pragma warning(disable:4054)   // function pointer to data pointer cast

#ifdef POOL_TAGGING
#undef ExAllocatePool
#define ExAllocatePool(a,b) ExAllocatePoolWithTag(a,b,'nepP')
#endif

#define FAULT_INJECT_SETPROPERTIES  DBG

#define MAX_REENUMERATION_ATTEMPTS  32

typedef struct _DRIVER_LIST_ENTRY DRIVER_LIST_ENTRY, *PDRIVER_LIST_ENTRY;

struct _DRIVER_LIST_ENTRY {
    PDRIVER_OBJECT DriverObject;
    PDRIVER_LIST_ENTRY NextEntry;
};

typedef enum _ADD_DRIVER_STAGE {
    LowerDeviceFilters = 0,
    LowerClassFilters,
    DeviceService,
    UpperDeviceFilters,
    UpperClassFilters,
    MaximumAddStage
} ADD_DRIVER_STAGE;

#define VerifierTypeFromServiceType(service) \
    (VF_DEVOBJ_TYPE) (service + 2)

#define MakeRegistryPath(path, subkey) (path##L"\\"##subkey)

typedef struct {

    PDEVICE_NODE DeviceNode;
    PADD_CONTEXT AddContext;
    PDRIVER_LIST_ENTRY DriverLists[MaximumAddStage];

} QUERY_CONTEXT, *PQUERY_CONTEXT;

typedef struct {
    ULONG   Problem;
    LOGICAL Restart;
    ULONG   Limit;
} CLEAR_PROBLEM_CONTEXT, *PCLEAR_PROBLEM_CONTEXT;

//
// Hash routine from CNTFS (see cntfs\prefxsup.c)
// (used here in the construction of unique ids)
//

#define HASH_UNICODE_STRING( _pustr, _phash ) {                             \
    PWCHAR _p = (_pustr)->Buffer;                                           \
    PWCHAR _ep = _p + ((_pustr)->Length/sizeof(WCHAR));                     \
    ULONG _chHolder =0;                                                     \
                                                                            \
    while( _p < _ep ) {                                                     \
        _chHolder = 37 * _chHolder + (unsigned int) (*_p++);                \
    }                                                                       \
                                                                            \
    *(_phash) = abs(314159269 * _chHolder) % 1000000007;                    \
}

// Parent prefixes are of the form %x&%x&%x
#define MAX_PARENT_PREFIX (8 + 8 + 8 + 2)

#if DBG

#define ASSERT_INITED(x) \
        if (((x)->Flags & DO_DEVICE_INITIALIZING) != 0)    \
            DbgPrint("DO_DEVICE_INITIALIZING flag not cleared on DO %#08lx\n", \
                    x);
#else

#define ASSERT_INITED(x) /* nothing */

#endif

#if DBG
VOID
PnpAssertDevnodesInConsistentState(
    VOID
    );
#else
#define PnpAssertDevnodesInConsistentState()
#endif

NTSTATUS
PipCallDriverAddDevice(
    __in PDEVICE_NODE DeviceNode,
    __in PADD_CONTEXT AddContext
    );

NTSTATUS
PipCallDriverAddDeviceQueryRoutine(
    __in ULONG ValueType,
    __in_bcount(ValueLength) PWCHAR ValueData,
    __in ULONG ValueLength,
    __in PQUERY_CONTEXT Context,
    __in ULONG ServiceType
    );

NTSTATUS
PipChangeDeviceObjectFromRegistryProperties(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in HANDLE DeviceInstanceKey,
    __in PCWSTR ClassGuid,
    __in HANDLE InstallerClassKey,
    __in BOOLEAN UsePdoCharacteristics
    );

NTSTATUS
PipEnumerateDevice(
    __in PDEVICE_NODE   DeviceNode,
    __in LOGICAL        Synchronous,
    __in LOGICAL        OperationsPending
    );

BOOLEAN
PipGetRegistryDwordWithFallback(
    __in    CM_REG_PROP RegistryProperty,
    __in    PCWSTR DeviceInstanceId,
    __in    HANDLE DeviceInstanceKey,
    __in    PCWSTR ClassGuid,
    __in    HANDLE InstallerClassKey,
    __inout PULONG Value
    );

PSECURITY_DESCRIPTOR
PipGetRegistrySecurityWithFallback(
    __in    CM_REG_PROP RegistryProperty,
    __in    PCWSTR DeviceInstanceId,
    __in    HANDLE DeviceInstanceKey,
    __in_opt PCWSTR ClassGuid,
    __in_opt HANDLE InstallerClassKey
    );

VOID
PnpLogDuplicateDevice (
    __in PWSTR BusId,
    __in PWSTR InstanceId
    );

NTSTATUS
PiProcessNewDeviceNode(
    __inout PDEVICE_NODE DeviceNode
    );

NTSTATUS
PipGenerateContainerID(
    __in        PDEVICE_NODE DeviceNode,
    __in_opt    HANDLE       DeviceInstanceKey,
    __in        BOOLEAN      Removable,
    __in_opt    PCWSTR       QueriedContainerID,
    __deref_out PWSTR        *ContainerID
    );

NTSTATUS
PiResetProblemDevicesWorker(
    __in  PDEVICE_NODE            DeviceNode,
    __in  PCLEAR_PROBLEM_CONTEXT  Context
    );

VOID
PiMarkDeviceTreeForReenumeration(
    __in  PDEVICE_NODE DeviceNode,
    __in  BOOLEAN Subtree
    );

NTSTATUS
PiMarkDeviceTreeForReenumerationWorker(
    __in  PDEVICE_NODE    DeviceNode,
    __in_opt PVOID           Context
    );

BOOLEAN
PiCollapseEnumRequests(
    PLIST_ENTRY ListHead
    );

NTSTATUS
PipEnumerateCompleted(
    __in PDEVICE_NODE DeviceNode
    );

VOID
PipProcessEnumeratedChildDevice(
    __in PDEVICE_NODE DeviceNode,
    __in PDEVICE_OBJECT childDeviceObject
    );

NTSTATUS
PiBuildDeviceNodeInstancePath(
    __in PDEVICE_NODE DeviceNode,
    __in PWSTR BusID,
    __in PWSTR DeviceID,
    __in PWSTR InstanceID
    );

NTSTATUS
PiCreateDeviceInstanceKey(
    __in PDEVICE_NODE DeviceNode,
    __out PHANDLE InstanceHandle,
    __out PULONG Disposition
    );

NTSTATUS
PiQueryAndAllocateBootResources(
    __in PDEVICE_NODE DeviceNode
    );

NTSTATUS
PiQueryResourceRequirements(
    __in PDEVICE_NODE DeviceNode
    );

NTSTATUS
PnpCallDriverQueryServiceHelper(
    __deref_in_bcount(*propertyBufferSize) PUCHAR * propertyBuffer,
    __in PULONG propertyBufferSize,
    __in CM_REG_PROP property,
    __in PWSTR objectName,
    __in HANDLE objectKey,
    __in BOOLEAN isClassProperty,
    __in ADD_DRIVER_STAGE serviceType,
    __in PQUERY_CONTEXT queryContext
    );

NTSTATUS
PnpGetServiceStartType(
    __in PCWSTR ServiceName,
    __in HANDLE ServiceKey,
    __out PULONG LoadType
    );

BOOLEAN
PnpCheckPossibleBootStartDriver(
    __in HANDLE ServiceKey
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PipCallDriverAddDevice)
#pragma alloc_text(PAGE, PipCallDriverAddDeviceQueryRoutine)
#pragma alloc_text(PAGE, PipChangeDeviceObjectFromRegistryProperties)
#pragma alloc_text(PAGE, PipEnumerateDevice)
#pragma alloc_text(PAGE, PipGetRegistryDwordWithFallback)
#pragma alloc_text(PAGE, PipGetRegistrySecurityWithFallback)
#pragma alloc_text(PAGE, PipMakeGloballyUniqueId)
#pragma alloc_text(PAGE, PipProcessDevNodeTree)
#pragma alloc_text(PAGE, PipGenerateContainerID)
#pragma alloc_text(PAGE, PiProcessNewDeviceNode)
#pragma alloc_text(PAGE, PiProcessHaltDevice)
#pragma alloc_text(PAGE, PpResetProblemDevices)
#pragma alloc_text(PAGE, PiResetProblemDevicesWorker)
#pragma alloc_text(PAGE, PiMarkDeviceTreeForReenumeration)
#pragma alloc_text(PAGE, PiMarkDeviceTreeForReenumerationWorker)
#pragma alloc_text(PAGE, PiProcessAddBootDevices)
#pragma alloc_text(PAGE, PiProcessClearDeviceProblem)
#pragma alloc_text(PAGE, PiProcessRequeryDeviceState)
#pragma alloc_text(PAGE, PiRestartDevice)
#pragma alloc_text(PAGE, PiProcessResourceRequirementsChanged)
#pragma alloc_text(PAGE, PiProcessReenumeration)
#pragma alloc_text(PAGE, PiProcessSetDeviceProblem)
#pragma alloc_text(PAGE, PiProcessStartSystemDevices)
#pragma alloc_text(PAGE, PiConfigureDevice)
#pragma alloc_text(PAGE, PipEnumerateCompleted)
#pragma alloc_text(PAGE, PipProcessEnumeratedChildDevice)
#pragma alloc_text(PAGE, PiBuildDeviceNodeInstancePath)
#pragma alloc_text(PAGE, PiCreateDeviceInstanceKey)
#pragma alloc_text(PAGE, PiQueryAndAllocateBootResources)
#pragma alloc_text(PAGE, PiQueryResourceRequirements)
#pragma alloc_text(PAGE, PnpCallAddDevice)
#pragma alloc_text(PAGE, PnpLogDuplicateDevice)
#pragma alloc_text(PAGE, PnpCallDriverQueryServiceHelper)
#pragma alloc_text(PAGE, PnpGetServiceStartType)
#pragma alloc_text(PAGE, PnpCheckPossibleBootStartDriver)

//#pragma alloc_text(NONPAGE, PiCollapseEnumRequests)
//#pragma alloc_text(NONPAGE, PpMarkDeviceStackExtensionFlag)

#if DBG

#pragma alloc_text(PAGE, PnpAssertDevnodesInConsistentState)

#endif

#endif


//
// Cached HAL routine to determine the NUMA node from proximity ID and vice
// versa.
//

PHALNUMAQUERYPROXIMITYNODE PnpQueryProximityNode = NULL;
PHALNUMAQUERYPROXIMITYID PnpQueryProximityId = NULL;

//
// Internal constant strings
//

#define DEVICE_PREFIX_STRING                TEXT("\\Device\\")
#define DOSDEVICES_PREFIX_STRING            TEXT("\\DosDevices\\")

NTSTATUS
PipEnumerateDevice (
    __in PDEVICE_NODE   DeviceNode,
    __in LOGICAL        Synchronous,
    __in LOGICAL        OperationsPending
    )

/*++

Routine Description:

    This function assumes that the specified physical device object is
    a bus and will enumerate all of the children PDOs on the bus.

Arguments:

    DeviceObject - Supplies a pointer to the physical device object to be
                   enumerated.

    StartContext - supplies a pointer to the START_CONTEXT to control how to
                   add/start new devices.

Return Value:

    NTSTATUS code.

--*/

{
    NTSTATUS status;
    PNP_DEVNODE_STATE completionState;
    PPNP_DEVICE_COMPLETION_REQUEST completionRequest;

    PAGED_CODE();

    completionState = DeviceNodeEnumerateCompletion;

    if (Synchronous == FALSE &&
        (PnpAsyncOptions & PNP_ASYNC_ENUM_ENABLED) &&
        !(DeviceNode->Flags & DNF_ASYNC_ENUMERATION_NOT_SUPPORTED) &&
        !InitSafeBootMode) {

        //
        // We should attempt asynchronous enumeration.
        //
        IopDbgPrint((IOP_ENUMERATION_TRACE_LEVEL,
                     "Enumerating %wZ asynchronously\n",
                     PnpGetDeviceInstancePath(DeviceNode)));

        completionRequest = PnpDeviceCompletionRequestCreate(DeviceNode,
                                                             completionState,
                                                             NULL);

        if (!completionRequest) {

            //
            // Enumerate the bus synchronously if we could not allocate the completion
            // request.
            //
            goto sync;
        }

        PipSetDevNodeState(DeviceNode, DeviceNodeEnumeratePending, NULL);

        DeviceNode->CompletionStatus = STATUS_PENDING;

        PnpDeviceCompletionQueueAddDispatchedRequest(&PnpDeviceCompletionQueue,
                                                     &completionRequest->ListEntry);

        PnpDiagnosticTraceObject(&KMPnPEvt_DeviceEnum_Start,
                                 PnpGetDeviceInstancePath(DeviceNode));

        //
        // Clear the flag before the query so we dont lose another enum request before current
        // one completes.
        //
        PipClearDevNodeFlags(DeviceNode, DNF_REENUMERATE);

        //
        // Activate the device before sending the enumeration irp.  Undo of the
        // activation will be done when the request is completed
        // asynchronously.
        //

        PNP_ASSERT((DeviceNode->StateFlags & DNSF_POFX_BUS_RELATIONS) == 0);

        PoFxActivateDevice(DeviceNode->PhysicalDeviceObject);
        DeviceNode->StateFlags |= DNSF_POFX_BUS_RELATIONS;
        status = PnpQueryDeviceRelations(DeviceNode->PhysicalDeviceObject,
                                         BusRelations,
                                         PnpDeviceCompletionRoutine,
                                         (PVOID)completionRequest);
        if (status == STATUS_PENDING) {
            PnpDiagnosticTraceObject(&KMPnPEvt_DeviceEnum_Pend,
                                     PnpGetDeviceInstancePath(DeviceNode));

        } else {

            //
            // If there was a failure to complete the query IRP then just signal
            // the completion of the request now.
            //

            if (completionRequest->Status != status) {
                completionRequest->Status = status;
                PnpDiagnosticTraceObject(&KMPnPEvt_DeviceEnum_Stop,
                                         PnpGetDeviceInstancePath(DeviceNode));

                PnpDeviceCompletionQueueDispatchedEntryCompleted(
                                                &PnpDeviceCompletionQueue,
                                                &completionRequest->ListEntry);

            }

            PnpDeviceCompletionQueueRemoveCompletedRequest(
                                                &PnpDeviceCompletionQueue,
                                                &completionRequest->ListEntry);

            status = PnpDeviceCompletionProcessCompletedRequest(
                                                            completionRequest);

            IopDbgPrint((IOP_ENUMERATION_TRACE_LEVEL,
                         "Enumerated %wZ without pending with status 0x%8.8X\n",
                         PnpGetDeviceInstancePath(DeviceNode),
                         status));


            //
            // Completion status will be set in the devnode.
            //

            status = STATUS_SUCCESS;
        }

        goto exit;
    }

sync:

    //
    // If this device does not support async enum, make sure we dont send the start IRP
    // while some other PNP IRP is pended in the system.
    //
    if (OperationsPending && (DeviceNode->Flags & DNF_ASYNC_ENUMERATION_NOT_SUPPORTED)) {

        status = STATUS_RETRY;
        goto exit;
    }

    //
    // Synchronous enumeration.
    //
    PnpDiagnosticTraceObject(&KMPnPEvt_DeviceEnum_Start,
                             PnpGetDeviceInstancePath(DeviceNode));

    IopDbgPrint((IOP_ENUMERATION_TRACE_LEVEL,
                 "Enumerating %wZ synchronously\n",
                 PnpGetDeviceInstancePath(DeviceNode)));

    //
    // Clear the flag before the query so we dont lose another enum request before current
    // one completes.
    //
    PipClearDevNodeFlags(DeviceNode, DNF_REENUMERATE);

    //
    // Ensure the device is activated prior to sending the enumeration irp.
    //

    PNP_ASSERT((DeviceNode->StateFlags & DNSF_POFX_BUS_RELATIONS) == 0);

    PoFxActivateDevice(DeviceNode->PhysicalDeviceObject);
    DeviceNode->StateFlags |= DNSF_POFX_BUS_RELATIONS;
    status = PnpQueryDeviceRelations(DeviceNode->PhysicalDeviceObject,
                                     BusRelations,
                                     NULL,
                                     (PVOID)&DeviceNode->OverUsed1.PendingDeviceRelations);

    PoFxIdleDevice(DeviceNode->PhysicalDeviceObject);
    DeviceNode->StateFlags &= ~DNSF_POFX_BUS_RELATIONS;
    DeviceNode->CompletionStatus = status;

    PipSetDevNodeState(DeviceNode, completionState, NULL);

    IopDbgPrint((IOP_ENUMERATION_TRACE_LEVEL,
                 "Enumerated %wZ with status 0x%8.8X <=\n",
                 PnpGetDeviceInstancePath(DeviceNode),
                 status));

    PnpDiagnosticTraceObject(&KMPnPEvt_DeviceEnum_Stop,
                             PnpGetDeviceInstancePath(DeviceNode));

    status = STATUS_SUCCESS;

exit:

    return status;
}

NTSTATUS
PipEnumerateCompleted(
    __in PDEVICE_NODE DeviceNode
    )
{
    PDEVICE_NODE    childDeviceNode, nextChildDeviceNode;
    PDEVICE_OBJECT  childDeviceObject;
    BOOLEAN         childRemoved;
    NTSTATUS        status;
    ULONG           i;
    PLIST_ENTRY     swChildrenListHead;
    PLIST_ENTRY     swChildListEntry;
    HPNPOPCTX       hPnpOpCtx = NULL;

    PAGED_CODE();

    //
    // Walk all the child device nodes and mark them as not enumerated
    // in order for them to be removed if they are not re-reported
    // by the bus.
    //
    // Note that if a stack returns NULL PendingDeviceRelations
    // this can indicate that the stack is not a bus, or that it is
    // a bus but that it failed to allocate the DEVICE_RELATIONS structure.
    // In the out of resources case we should not surprise remove the children.
    //

    if (DeviceNode->OverUsed1.PendingDeviceRelations != NULL) {

        //
        // The stack reported device relations.
        // Mark all the children as not enumerated.
        //

        childDeviceNode = DeviceNode->Child;
        while (childDeviceNode) {
            PipClearDevNodeFlags(childDeviceNode, DNF_ENUMERATED);
            childDeviceNode = childDeviceNode->Sibling;
        }

    } else {

        //
        // The stack did not report device relations.
        // Only mark SwDevice children as not enumerated.
        //

        childDeviceNode = DeviceNode->Child;
        while (childDeviceNode) {

            if (childDeviceNode->PhysicalDeviceObject->DriverObject == PiSwDeviceDriverObject) {

                PipClearDevNodeFlags(childDeviceNode, DNF_ENUMERATED);
            }

            childDeviceNode = childDeviceNode->Sibling;
        }
    }

    //
    // Process real children
    //

    if (DeviceNode->OverUsed1.PendingDeviceRelations != NULL) {

        //
        // Check all the PDOs returned see if any new one or any one disappeared.
        //

        for (i = 0; i < DeviceNode->OverUsed1.PendingDeviceRelations->Count; i++) {

            childDeviceObject = DeviceNode->OverUsed1.PendingDeviceRelations->Objects[i];

            ASSERT_INITED(childDeviceObject);

            PipProcessEnumeratedChildDevice(DeviceNode,
                                            childDeviceObject);
        }

        ExFreePool(DeviceNode->OverUsed1.PendingDeviceRelations);
        DeviceNode->OverUsed1.PendingDeviceRelations = NULL;
    }

    //
    // Process software device children
    //

    PiSwLock();

    swChildrenListHead = PiSwFindChildren(PnpGetDeviceInstancePath(DeviceNode));

    if (swChildrenListHead) {

        swChildListEntry = swChildrenListHead->Flink;

        while (swChildListEntry != swChildrenListHead) {

            childDeviceObject = PiSwGetChildPdo(DeviceNode->PhysicalDeviceObject,
                                                swChildListEntry);

            if (childDeviceObject) {

                ASSERT_INITED(childDeviceObject);

                PipProcessEnumeratedChildDevice(DeviceNode,
                                                childDeviceObject);
            }

            swChildListEntry = swChildListEntry->Flink;
        }
    }

    PiSwUnlock();

    //
    // If we get here, the enumeration was successful.  Process any missing
    // devnodes.
    //

    childRemoved = FALSE;

    for (childDeviceNode = DeviceNode->Child;
         childDeviceNode != NULL;
         childDeviceNode = nextChildDeviceNode) {

        //
        // First, we need to remember the 'next child' because the 'child' will be
        // removed and we won't be able to find the 'next child.'
        //

        nextChildDeviceNode = childDeviceNode->Sibling;

        if (!(childDeviceNode->Flags & DNF_ENUMERATED)) {

            if (!(childDeviceNode->Flags & DNF_DEVICE_GONE)) {

                PipSetDevNodeFlags(childDeviceNode, DNF_DEVICE_GONE);

                PnpRequestDeviceRemoval(childDeviceNode,
                                        TRUE,
                                        CM_PROB_DEVICE_NOT_THERE,
                                        STATUS_SUCCESS);

                childRemoved = TRUE;
            }
        }
    }

    PNP_ASSERT(DeviceNode->State == DeviceNodeEnumerateCompletion);
    PipSetDevNodeState(DeviceNode, DeviceNodeStarted, NULL);

    //
    // The root enumerator gets confused if we reenumerate it before we process
    // newly reported PDOs.  Since it can't possibly create the scenario we are
    // trying to fix, we won't bother waiting for the removes to complete before
    // processing the new devnodes.
    //

    if (childRemoved && DeviceNode != IopRootDeviceNode) {

        status = STATUS_PNP_RESTART_ENUMERATION;

    } else {

        status = STATUS_SUCCESS;
    }

    //
    // Raise PnpRtl events for the updated NtPlugPlay properties.
    //
    // If creating the operation fails, just continue and raise the events anyway.
    // We can't fail the operation at this point;
    //

    PiPnpRtlBeginOperation(&hPnpOpCtx);

    _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                 PnpGetDeviceInstancePath(IopRootDeviceNode)->Buffer,
                                                 NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_CHILDREN);

    _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                 PnpGetDeviceInstancePath(IopRootDeviceNode)->Buffer,
                                                 NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_BUS_RELATIONS);

    if (hPnpOpCtx) {

        PiPnpRtlEndOperation(hPnpOpCtx);
    }

    return status;
}

VOID
PipProcessEnumeratedChildDevice(
    __in PDEVICE_NODE DeviceNode,
    __in PDEVICE_OBJECT ChildDeviceObject
    )
{
    NTSTATUS        allocationStatus;
    PDEVICE_NODE    childDeviceNode;

    PAGED_CODE();

    if (ChildDeviceObject->DeviceObjectExtension->ExtensionFlags & DOE_DELETE_PENDING) {

        PNP_SAVE_DEVICEOBJECT_TO_TRIAGE_DUMP(ChildDeviceObject);
        KeBugCheckEx( PNP_DETECTED_FATAL_ERROR,
                      PNP_ERR_PDO_ENUMERATED_AFTER_DELETION,
                      (ULONG_PTR)ChildDeviceObject,
                      0,
                      0);
    }

    //
    // We've found another physical device, see if there is
    // already a devnode for it.
    //

    childDeviceNode = (PDEVICE_NODE)ChildDeviceObject->DeviceObjectExtension->DeviceNode;
    if (childDeviceNode == NULL) {

        //
        // Device node doesn't exist, create one.
        //

        allocationStatus = PipAllocateDeviceNode(
            ChildDeviceObject,
            &childDeviceNode);

        if (childDeviceNode) {

            //
            // We've found or created a devnode for the PDO that the
            // bus driver just enumerated.
            //
            PipSetDevNodeFlags(childDeviceNode, DNF_ENUMERATED);

            //
            // Mark the device object a bus enumerated device
            //
            ChildDeviceObject->Flags |= DO_BUS_ENUMERATED_DEVICE;

            //
            // Put this new device node at the head of the parent's list
            // of children.
            //
            PpDevNodeInsertIntoTree(DeviceNode, childDeviceNode);
            if (allocationStatus == STATUS_SYSTEM_HIVE_TOO_LARGE) {
                PNP_SAVE_FAILURE_INFO(childDeviceNode,
                                      allocationStatus);
                PipSetDevNodeProblem(childDeviceNode,
                                     CM_PROB_REGISTRY_TOO_LARGE,
                                     STATUS_SYSTEM_HIVE_TOO_LARGE);
            }

        } else {

            //
            // Had a problem creating a devnode.  Pretend we've never
            // seen it.
            //
            IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                         "PipEnumerateDevice: Failed to allocate device node\n"));

            ObDereferenceObject(ChildDeviceObject);
        }
    } else {

        //
        // The device is already enumerated.  Remark it and release the
        // device object reference.
        //
        PipSetDevNodeFlags(childDeviceNode, DNF_ENUMERATED);

        if (childDeviceNode->DockInfo.DockStatus == DOCK_EJECTIRP_COMPLETED) {

            //
            // A dock that was listed as departing in an eject relation
            // didn't actually leave. Remove it from the profile transition
            // list...
            //
            PpProfileCancelTransitioningDock(childDeviceNode, DOCK_DEPARTING);
        }

        PNP_ASSERT(!(childDeviceNode->Flags & DNF_DEVICE_GONE));

        ObDereferenceObject(ChildDeviceObject);
    }
}

VOID
PpMarkDeviceStackExtensionFlag(
    __in PDEVICE_OBJECT   DeviceObject,
    __in ULONG            ExtensionFlag,
    __in BOOLEAN          Set
    )

/*++

Routine Description:

    This function marks the entire device stack with the given ExtensionFlag.

Arguments:

    DeviceObject - PDO for the device stack.

Return Value:

    None.

--*/

{
    PDEVICE_OBJECT attachedDevice;
    KIRQL irql;

    irql = KeAcquireQueuedSpinLock( LockQueueIoDatabaseLock );

    for (attachedDevice = DeviceObject;
         attachedDevice != NULL;
         attachedDevice = attachedDevice->AttachedDevice) {

        if (Set) {

            attachedDevice->DeviceObjectExtension->ExtensionFlags |= ExtensionFlag;
        } else {

            attachedDevice->DeviceObjectExtension->ExtensionFlags &= ~ExtensionFlag;
        }
    }

    KeReleaseQueuedSpinLock(LockQueueIoDatabaseLock, irql);
}

NTSTATUS
PiBuildDeviceNodeInstancePath(
    __in PDEVICE_NODE DeviceNode,
    __in PWSTR BusID,
    __in PWSTR DeviceID,
    __in PWSTR InstanceID
    )

/*++

Routine Description:

    This function builds the instance path (BusID\DeviceID\InstanceID). If
    successful, it will free the storage for any existing instance path and
    replace with the new one.

Arguments:

    DeviceNode - DeviceNode for which the instance path will be built.

    BusID - Bus ID.

    DeviceID - Device ID.

    InstanceID - Instance ID.

Return Value:

    NTSTATUS.

--*/

{
    PWCHAR buffer = NULL;
    ULONG length;
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();

    if (BusID == NULL || DeviceID == NULL || InstanceID == NULL) {
        PNP_ASSERT( PipIsDevNodeProblem(DeviceNode, CM_PROB_INVALID_DATA) ||
              PipIsDevNodeProblem(DeviceNode, CM_PROB_OUT_OF_MEMORY) ||
              PipIsDevNodeProblem(DeviceNode, CM_PROB_REGISTRY));

        status = STATUS_UNSUCCESSFUL;
        goto exit;
    }

    if (!PnpIsDeviceInstancePathEmpty(DeviceNode) &&
        !PipIsDevNodeProblem(DeviceNode, CM_PROB_DUPLICATE_DEVICE)) {

        PnpCleanupDeviceRegistryValues(PnpGetDeviceInstancePath(DeviceNode));
    }

    length = (ULONG)((wcslen(BusID) + wcslen(DeviceID) + wcslen(InstanceID) + 2) * sizeof(WCHAR) + sizeof(UNICODE_NULL));

    buffer = ExAllocatePoolWithTag(NonPagedPoolNx,
                                   length,
                                   PNP_POOLTAG_INSTANCE_PATH);
    if (!buffer) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    //
    // Construct the instance path as <BUS>\<DEVICE>\<INSTANCE>. This should
    // always be NULL terminated since we have precomputed the length that we
    // pass into this counted routine.
    //

    status = RtlStringCbPrintfW(buffer,
                                length,
                                L"%s\\%s\\%s",
                                BusID,
                                DeviceID,
                                InstanceID);

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // If the device node does not have an instance path or the new instance
    // path is different from the old one then assign the new buffer to the
    // instance path. Note that if we free the old path and assign the new path
    // there is a possible race with code that gets the instance path
    // from the PDO or DEVICE_NODE.  This race has existed since at least
    // Vista.  This race is in practice only a problem if a bus incorrectly
    // reports duplicate PDO for the same instance ID or a instance ID for an
    // existing device changes without a final remove.  The latter has not been
    // observed.
    //

    if (PnpIsDeviceInstancePathEmpty(DeviceNode) ||
        (_wcsicmp(PnpGetDeviceInstancePath(DeviceNode)->Buffer, buffer) != 0)) {

        PnpFreeDeviceInstancePath(DeviceNode);

        RtlInitUnicodeString(PnpGetDeviceInstancePath(DeviceNode), buffer);

        buffer = NULL;
    }

exit:

    if (buffer) {
        ExFreePoolWithTag(buffer, PNP_POOLTAG_INSTANCE_PATH);
    }

    return status;
}

NTSTATUS
PiCreateDeviceInstanceKey(
    __in PDEVICE_NODE DeviceNode,
    __out PHANDLE InstanceKey,
    __out PULONG Disposition
    )

/*++

Routine Description:

    This function will create the device instance key.

Arguments:

    DeviceNode - DeviceNode for which the instance path will be built.

    InstanceKey - Will recieve the instance key handle.

    Disposition - Will recieve the disposition whether the key existed or was newly created.

Return Value:

    NTSTATUS.

--*/

{
    BOOLEAN created;
    NTSTATUS status;

    PAGED_CODE();

    *InstanceKey = NULL;
    *Disposition = 0;

    created = FALSE;

    PiLockPnpRegistry(FALSE);

    status = _CmCreateDevice(PiPnpRtlCtx,
                             PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                             KEY_ALL_ACCESS,
                             InstanceKey,
                             &created,
                             0);

    if (!NT_SUCCESS(status)) {
        IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                     "PpCreateDeviceInstanceKey: Unable to create %wZ (status 0x%x)\n",
                     PnpGetDeviceInstancePath(DeviceNode),
                     status));

        goto exit;
    }

    PNP_ASSERT(*InstanceKey != NULL);

    if (created) {
        *Disposition = REG_CREATED_NEW_KEY;
    } else {
        *Disposition = REG_OPENED_EXISTING_KEY;
    }

  exit:

    PiUnlockPnpRegistry();

    return status;
}

NTSTATUS
PiQueryAndAllocateBootResources(
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This function will query the BOOT resources for the device and reserve them
    from the arbiter.

Arguments:

    DeviceNode - DeviceNode for which the BOOT resources need to be queried.

Return Value:

    NTSTATUS.

--*/

{
    ULONG cmLength;
    PCM_RESOURCE_LIST cmResource;
    BOOLEAN createKey;
    HANDLE logConfKey;
    NTSTATUS status;
    NTSTATUS tempStatus;
    UNICODE_STRING unicodeString;

    PAGED_CODE();

    cmLength = 0;
    cmResource = NULL;
    createKey = FALSE;
    logConfKey = NULL;
    status = STATUS_SUCCESS;

    if (DeviceNode->BootResources == NULL) {

        status = IopQueryDeviceResources(
                    DeviceNode->PhysicalDeviceObject,
                    QUERY_RESOURCE_LIST,
                    &cmResource,
                    &cmLength);
        if (!NT_SUCCESS(status)) {

            PNP_ASSERT(cmResource == NULL && cmLength == 0);
            cmResource = NULL;
            cmLength = 0;
        }
    } else {

        IopDbgPrint((   IOP_ENUMERATION_INFO_LEVEL,
                        "PNPENUM: %wZ already has BOOT config in PiQueryAndAllocateBootResources!\n",
                        PnpGetDeviceInstancePath(DeviceNode)));
    }

    //
    // Write boot resources to registry
    //

    if ((PipIsDevNodeProblem(DeviceNode, CM_PROB_INVALID_DATA) == FALSE) &&
        (PipIsDevNodeProblem(DeviceNode, CM_PROB_OUT_OF_MEMORY) == FALSE) &&
        (PipIsDevNodeProblem(DeviceNode, CM_PROB_REGISTRY) == FALSE)) {

        //
        // Only create the LogConf key if a resource will be set.  If a resource
        // is not going to be set and the LogConf key doesn't exist, then there
        // is no resource there to delete.
        //

        if (cmResource != NULL) {
            createKey = TRUE;
        }

        tempStatus = _CmOpenDeviceRegKey(
                     PiPnpRtlCtx,
                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                     CM_KEY_TYPE_DEVICE_LOGCONF,
                     0,
                     KEY_ALL_ACCESS,
                     createKey,
                     &logConfKey,
                     NULL);

        if (!NT_SUCCESS(tempStatus)) {
            logConfKey = NULL;
        }

        if ((logConfKey != NULL) && (DeviceNode->BootResources == NULL)) {

            PiWstrToUnicodeString(&unicodeString, REGSTR_VAL_BOOTCONFIG);

            PiLockPnpRegistry(FALSE);

            if (cmResource) {

                ZwSetValueKey(
                    logConfKey,
                    &unicodeString,
                    TITLE_INDEX_VALUE,
                    REG_RESOURCE_LIST,
                    cmResource,
                    cmLength);
            } else {

                ZwDeleteValueKey(logConfKey, &unicodeString);
            }

            PiUnlockPnpRegistry();

            if (cmResource) {
                //
                // This device consumes BOOT resources.  Reserve its boot resources
                //
                status = (*IopAllocateBootResourcesRoutine)(
                    ArbiterRequestPnpEnumerated,
                    DeviceNode->PhysicalDeviceObject,
                    cmResource);
                if (NT_SUCCESS(status)) {

                    PipSetDevNodeFlags(DeviceNode, DNF_HAS_BOOT_CONFIG);
                }
            }
        }
    }

    if (cmResource) {

        ExFreePool(cmResource);
    }

    if (logConfKey != NULL) {
        ZwClose(logConfKey);
    }

    return status;
}

NTSTATUS
PiQueryResourceRequirements(
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This function will query the resource requirements for the device.

Arguments:

    DeviceNode - DeviceNode for which the resource requirements need to be queried.

Return Value:

    NTSTATUS.

--*/

{
    BOOLEAN createKey;
    ULONG ioLength;
    PIO_RESOURCE_REQUIREMENTS_LIST ioResource;
    HANDLE logConfKey;
    NTSTATUS status;
    NTSTATUS tempStatus;
    UNICODE_STRING unicodeString;

    PAGED_CODE();

    createKey = FALSE;
    logConfKey = NULL;

    //
    // Query the device's basic config vector.
    //

    status = PpIrpQueryResourceRequirements(
                DeviceNode->PhysicalDeviceObject,
                &ioResource);
    if (!NT_SUCCESS(status)) {

        PNP_ASSERT(ioResource == NULL);
        ioResource = NULL;
    }
    if (ioResource) {

        ioLength = ioResource->ListSize;
    } else {

        ioLength = 0;
    }

    //
    // Write resource requirements to registry
    //

    if ((PipIsDevNodeProblem(DeviceNode, CM_PROB_INVALID_DATA) == FALSE) &&
        (PipIsDevNodeProblem(DeviceNode, CM_PROB_OUT_OF_MEMORY) == FALSE) &&
        (PipIsDevNodeProblem(DeviceNode, CM_PROB_REGISTRY) == FALSE)) {

        //
        // Only create the LogConf key if a resource will be set.  If a resource
        // is not going to be set and the LogConf key doesn't exist, then there
        // is no resource there to delete.
        //

        if (ioResource != NULL) {
            createKey = TRUE;
        }

        tempStatus = _CmOpenDeviceRegKey(
                     PiPnpRtlCtx,
                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                     CM_KEY_TYPE_DEVICE_LOGCONF,
                     0,
                     KEY_ALL_ACCESS,
                     createKey,
                     &logConfKey,
                     NULL);

        if (!NT_SUCCESS(tempStatus)) {
            logConfKey = NULL;
        }

        if (logConfKey != NULL) {

            PiWstrToUnicodeString(&unicodeString, REGSTR_VALUE_BASIC_CONFIG_VECTOR);

            PiLockPnpRegistry(FALSE);

            if (ioResource) {

                ZwSetValueKey(
                    logConfKey,
                    &unicodeString,
                    TITLE_INDEX_VALUE,
                    REG_RESOURCE_REQUIREMENTS_LIST,
                    ioResource,
                    ioLength);
                IopAcquireResourceListLock(&PiResourceListLock);
                DeviceNode->ResourceRequirements = ioResource;
                IopReleaseResourceListLock(&PiResourceListLock);
                PipSetDevNodeFlags(DeviceNode, DNF_RESOURCE_REQUIREMENTS_NEED_FILTERED);
                ioResource = NULL;
            } else {

                ZwDeleteValueKey(logConfKey, &unicodeString);
            }
            PiUnlockPnpRegistry();
        }
    }

    if (ioResource) {

        ExFreePool(ioResource);
    }

    if (logConfKey != NULL) {
        ZwClose(logConfKey);
    }

    return status;
}

NTSTATUS
PiProcessNewDeviceNode(
    __inout PDEVICE_NODE DeviceNode
    )
/*++

Routine Description:

    This function will process a new device.

Arguments:

    DeviceNode - New DeviceNode.

Return Value:

    NTSTATUS.

--*/
{
    LOGICAL alternateStringAvailable;
    PWCHAR baseContainerId;
    PWCHAR busID;
    DEVICE_CAPABILITIES capabilities;
    BOOLEAN checkDeviceNeedsUpdate;
    PWCHAR compatibleIDs;
    ULONG compatibleIDsSize;
    ULONG configFlags;
    ULONG configFlagsUpdate;
    BOOLEAN configureDevice;
    PWCHAR description;
    ULONG descriptionLength;
    PWCHAR deviceID;
    PDEVICE_OBJECT deviceObject;
    ULONG disposition;
    PDEVICE_NODE dupeDeviceNode;
    PDEVICE_OBJECT dupeDeviceObject;
    LARGE_INTEGER enumTime;
    NTSTATUS finalStatus;
    BOOLEAN globallyUnique;
    GUID guidBaseContainerId;
    HPNPOPCTX hPnpOpCtx = NULL;
    PWCHAR hwIDs;
    ULONG hwIDsSize;
    BOOLEAN idsChanged;
    PWCHAR instanceID;
    ULONG instanceIDLength;
    BOOLEAN instanceIdMadeUnique;
    HANDLE instanceKey = NULL;
    PWCHAR location;
    PWCHAR newBaseContainerId;
    ULONG problem;
    ULONG propertySize;
    DEVPROPTYPE propertyType;
    ULONG propType;
    BOOLEAN removable;
    ULONG setPropertyFlags;
    NTSTATUS status;
    PWCHAR tempString;
    ULONG tempStringLength;
    UNICODE_STRING unicodeString;
    PWCHAR uniqueInstanceID;

    PAGED_CODE();

    //
    // Trace an event indicating a new device is being processed.  This can help
    // in diagnosing performance issues.
    //
    EventWriteKMPnPEvt_ProcessNewDevice_Start(NULL, DeviceNode);

    KeQuerySystemTime(&enumTime);

    //
    // This function needs to use pnprtl, so make sure the context has been
    // initialized already.
    //

    PNP_ASSERT(PiPnpRtlCtx != NULL);

    checkDeviceNeedsUpdate = FALSE;
    configFlags = 0;
    configFlagsUpdate = 0;
    configureDevice = FALSE;
    disposition = 0;
    finalStatus = STATUS_SUCCESS;
    idsChanged = FALSE;
    instanceIdMadeUnique = FALSE;
    instanceKey = NULL;
    setPropertyFlags = 0;

    deviceObject = DeviceNode->PhysicalDeviceObject;

    PiPnpRtlBeginOperation(&hPnpOpCtx);

    //
    // If the system got here and this devnode is marked as uninstalled, then
    // the current enumeration pass expects that uninstalled devices reappear.
    // Clear DNF_UNINSTALLED such that the device node is allowed to start.
    //

    PipClearDevNodeFlags(DeviceNode, DNF_UNINSTALLED);
    status = PnpQueryDeviceID(DeviceNode, &busID, &deviceID);
    if (!NT_SUCCESS(status)) {

        if (status == STATUS_PNP_INVALID_ID) {

            finalStatus = STATUS_UNSUCCESSFUL;
        } else {

            finalStatus = status;
        }
    }

    //
    // Query the device's capabilities.
    //

    status = PpIrpQueryCapabilities(deviceObject, &capabilities);

    //
    // Process the capabilities before saving them.
    //

    PipClearDevNodeUserFlags(DeviceNode, DNUF_DONT_SHOW_IN_UI);
    globallyUnique = FALSE;
    if (NT_SUCCESS(status)) {

        if (capabilities.NoDisplayInUI) {

            PipSetDevNodeUserFlags(DeviceNode, DNUF_DONT_SHOW_IN_UI);
        }
        if (capabilities.UniqueID) {

            globallyUnique = TRUE;
        }
    }
    PpProfileProcessDockDeviceCapability(DeviceNode, &capabilities);

    //
    // Query the new devnode's description and location.
    //

    PnpQueryDeviceDescription(DeviceNode, &description);

    PnpQueryDeviceLocationInformation(DeviceNode, &location);

    //
    // Query the instance ID for the new devnode.
    //

    status = PnpQueryInstanceID(DeviceNode, &instanceID, &instanceIDLength);
    if (!globallyUnique) {

        if ((!PipIsDevNodeProblem(DeviceNode, CM_PROB_INVALID_DATA)) &&
            (DeviceNode->Parent != IopRootDeviceNode)) {

            uniqueInstanceID = NULL;

            status = PipMakeGloballyUniqueId(DeviceNode->Parent->PhysicalDeviceObject,
                                             instanceID,
                                             &uniqueInstanceID);

            instanceIdMadeUnique = TRUE;

            if (instanceID != NULL) {

                ExFreePool(instanceID);
            }
            instanceID = uniqueInstanceID;
            if (instanceID) {

                instanceIDLength = ((ULONG)wcslen(instanceID) + 1) * sizeof(WCHAR);
            } else {

                instanceIDLength = 0;
                PNP_ASSERT(!NT_SUCCESS(status));
            }

        }
    } else if (status == STATUS_NOT_SUPPORTED) {

        PNP_SAVE_FAILURE_INFO(DeviceNode, STATUS_NOT_SUPPORTED);
        PipSetDevNodeProblem(DeviceNode, CM_PROB_INVALID_DATA, STATUS_NOT_SUPPORTED);
        PipSetDevNodeFlags(DeviceNode->Parent, DNF_CHILD_WITH_INVALID_ID);
        PnpSetInvalidIDEvent(PnpGetDeviceInstancePath(DeviceNode->Parent));

        IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                     "PnpQueryID: Bogus ID returned by %wZ\n",
                     &DeviceNode->Parent->ServiceName));
        PNP_ASSERT(status != STATUS_NOT_SUPPORTED || !globallyUnique);
    }

RetryDuplicateId:

    if (!NT_SUCCESS(status)) {

        finalStatus = status;
        if (!PipIsDevNodeProblem(DeviceNode, CM_PROB_INVALID_DATA)) {
            PNP_SAVE_FAILURE_INFO(DeviceNode, status);
            if (status == STATUS_INSUFFICIENT_RESOURCES) {
                PipSetDevNodeProblem(DeviceNode,
                                     CM_PROB_OUT_OF_MEMORY,
                                     STATUS_INSUFFICIENT_RESOURCES);

            } else {

                //
                // Perhaps some other problem code?
                //

                PipSetDevNodeProblem(DeviceNode, CM_PROB_REGISTRY, status);
            }
        }
    }

    //
    // Build the device instance path and create the instance key.
    //

    status = PiBuildDeviceNodeInstancePath(DeviceNode, busID, deviceID, instanceID);
    if (NT_SUCCESS(status)) {
        //
        // Trace an event indicating which device is being processed.  This can help
        // in diagnosing performance issues.  Note that if this is a duplicate id,
        // this event could get logged again on the retry for duplicate ids.
        //
        EventWriteKMPnPEvt_ProcessNewDevice_InstancePath(NULL,
                                                         DeviceNode,
                                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer);

        status = PiCreateDeviceInstanceKey(DeviceNode, &instanceKey, &disposition);

        if (!NT_SUCCESS(status)) {
            PNP_SAVE_FAILURE_INFO(DeviceNode, status);
            PipSetDevNodeProblem(DeviceNode, CM_PROB_REGISTRY, status);
        } else {
            //
            // In cases where the device is being created for the first time, it
            // will have no properties and all property sets will be setting
            // things for the first time.  Use the write through flag to turn
            // off get before set behavior of the datamodel, since the gets in
            // this case will be wasted.
            //
            setPropertyFlags = (disposition == REG_CREATED_NEW_KEY) ?
                                   PIPNPRTL_FLAG_WRITE_THROUGH : 0;
        }
    }

    if (!NT_SUCCESS(status)) {
        finalStatus = status;
    }

    //
    // Mark the devnode as initialized.
    //

    PpMarkDeviceStackExtensionFlag(deviceObject, DOE_START_PENDING, TRUE);


    PipSetDevNodeState(DeviceNode, DeviceNodeInitialized, NULL);

    if (!PipIsDevNodeProblem(DeviceNode, CM_PROB_INVALID_DATA) &&
        !PipIsDevNodeProblem(DeviceNode, CM_PROB_OUT_OF_MEMORY) &&
        !PipIsDevNodeProblem(DeviceNode, CM_PROB_REGISTRY)) {

        //
        // Check if we are encountering this device for the very first time.
        //

        if (disposition == REG_CREATED_NEW_KEY) {

            //
            // Save the description only for new devices so we dont clobber
            // the inf written description for already installed devices.
            //

            PiLockPnpRegistry(FALSE);

            if ((instanceKey != NULL) && (description != NULL)) {
                _CmSetDeviceRegProp(PiPnpRtlCtx,
                                    PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                    instanceKey,
                                    CM_REG_PROP_DEVICEDESC,
                                    REG_SZ,
                                    (PUCHAR)description,
                                    (ULONG)((wcslen(description)+1) * sizeof(WCHAR)),
                                    setPropertyFlags);
            }

            PiUnlockPnpRegistry();

        } else {

            //
            // Check if there is another device with the same name.
            //

            dupeDeviceObject = PnpDeviceObjectFromDeviceInstance(PnpGetDeviceInstancePath(DeviceNode));
            if (dupeDeviceObject) {

                if (dupeDeviceObject != deviceObject) {

                    //
                    // The devices currently have the same instance path.  This will usually occur
                    // if the devices are identical and also have the SAME serial number. In this
                    // case the following will occur:
                    //
                    // 1. Retry to register an instance of the device by attempting to create a unique
                    // device instance path.  If this succeeds and no device instance with the new
                    // path exists in the instance table, the device's problem code will be cleared.
                    //
                    // 2. If after retrying to create a unique device instance path, a duplicate is
                    // still found in the instance table, the system will bugcheck.
                    //
                    // Note that the device instance path is simply a concatenation of the following:
                    // <BUS ID>\<DEVICE ID>\<INSTANCE ID>.  The instance ID is usually the serial
                    // number, which should be unique.
                    //
                    // In this case where the serial number is not unique, we modify the instance id
                    // by prepending it with the unique parent id.
                    //
                    // Thus, it is possible to have any number of devices with the same serial number,
                    // as long as each has a different parent/bus.  If two duplicate devices are found
                    // with the same parent, both devices will have the same modified instance id
                    // and thus the system will bugcheck.
                    //
                    // The only case where two devices with the same serial number can enumerate
                    // successfully on the same parent is when one of the devices is the very first
                    // such device found on the system.  Here the first device will have an
                    // instance id equal to the serial number and the second device will get the
                    // modified instance id.
                    //

                    if (globallyUnique) {

                        //
                        // Since this is a duplicate device, the instance key opened
                        // above won't be used.  Close it so a handle doesn't get
                        // leaked.
                        //

                        PNP_ASSERT(instanceKey != NULL);
                        ZwClose(instanceKey);
                        instanceKey = NULL;

                        globallyUnique = FALSE;
                        capabilities.UniqueID = FALSE;
                        PipSetDevNodeProblem(DeviceNode, CM_PROB_DUPLICATE_DEVICE, STATUS_SUCCESS);
                        PnpLogDuplicateDevice(busID, instanceID);

                        dupeDeviceNode = dupeDeviceObject->DeviceObjectExtension->DeviceNode;
                        PNP_ASSERT(dupeDeviceNode);

                        if (dupeDeviceNode->Parent == DeviceNode->Parent) {

                            //
                            // Definite driver mistake. If the verifier is enabled
                            // we will fail the driver. Otherwise, we will attempt
                            // to uniquify the second device to keep the system
                            // alive.
                            //

                            PpvUtilFailDriver(
                                PPVERROR_DUPLICATE_PDO_ENUMERATED,
                                (PVOID)deviceObject->DriverObject->MajorFunction[IRP_MJ_PNP],
                                deviceObject,
                                (PVOID)dupeDeviceObject);
                        }

                        ObDereferenceObject(dupeDeviceObject);

                        status = PipMakeGloballyUniqueId(DeviceNode->Parent->PhysicalDeviceObject,
                                                         instanceID,
                                                         &uniqueInstanceID);

                        instanceIdMadeUnique = TRUE;

                        if (instanceID != NULL) {

                            ExFreePool(instanceID);
                        }
                        instanceID = uniqueInstanceID;
                        if (instanceID) {

                            instanceIDLength = ((ULONG)wcslen(instanceID) + 1) * sizeof(WCHAR);
                        } else {

                            instanceIDLength = 0;
                            PNP_ASSERT(!NT_SUCCESS(status));
                        }

                        //
                        // Cleanup and retry.
                        //

                        goto RetryDuplicateId;
                    }

                    //
                    // No need to clean up the ref as we're going to crash the
                    // system.
                    //
                    //ObDereferenceObject(dupCheckDeviceObject);

                    PpvUtilFailDriver(
                        PPVERROR_DUPLICATE_PDO_ENUMERATED,
                        (PVOID) deviceObject->DriverObject->MajorFunction[IRP_MJ_PNP],
                        deviceObject,
                        (PVOID)dupeDeviceObject);

                    PNP_SAVE_DEVICEOBJECT_TO_TRIAGE_DUMP(deviceObject);
                    PNP_SAVE_DEVICEOBJECT_TO_TRIAGE_DUMP(dupeDeviceObject);
                    KeBugCheckEx(
                        PNP_DETECTED_FATAL_ERROR,
                        PNP_ERR_DUPLICATE_PDO,
                        (ULONG_PTR)deviceObject,
                        (ULONG_PTR)dupeDeviceObject,
                        0);
                }
                ObDereferenceObject(dupeDeviceObject);
            }
        }

        //
        // Remove CM_PROB_DUPLICATE_DEVICE if it was set since if the system got
        // here we should be guaranteed a unique instance id.  Otherwise we
        // should have bugchecked.
        //

        if (PipIsDevNodeProblem(DeviceNode, CM_PROB_DUPLICATE_DEVICE)) {
            PipClearDevNodeProblem(DeviceNode);
        }
    }

    if (instanceKey != NULL) {
        //
        // Save the last known parent.
        //

        _PnpSetObjectProperty(PiPnpRtlCtx,
                              PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                              PNP_OBJECT_TYPE_DEVICE,
                              instanceKey,
                              NULL,
                              &DEVPKEY_Device_LastKnownParent,
                              DEVPROP_TYPE_STRING,
                              (PUCHAR) PnpGetDeviceInstancePath(DeviceNode->Parent)->Buffer,
                              PnpGetDeviceInstancePath(DeviceNode->Parent)->Length + sizeof(WCHAR),
                              setPropertyFlags);

        //
        // Set last enumerated date property, clear the last removed date.
        //

        _PnpSetObjectProperty(PiPnpRtlCtx,
                              PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                              PNP_OBJECT_TYPE_DEVICE,
                              instanceKey,
                              NULL,
                              &DEVPKEY_Device_LastArrivalDate,
                              DEVPROP_TYPE_FILETIME,
                              (PUCHAR)&enumTime,
                              sizeof(LARGE_INTEGER),
                              setPropertyFlags);

        _PnpSetObjectProperty(PiPnpRtlCtx,
                              PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                              PNP_OBJECT_TYPE_DEVICE,
                              instanceKey,
                              NULL,
                              &DEVPKEY_Device_LastRemovalDate,
                              DEVPROP_TYPE_EMPTY,
                              NULL,
                              0,
                              setPropertyFlags);

        //
        // Set a property indicating what hardware configuration this device was
        // last used on.
        //
        _PnpSetObjectProperty(PiPnpRtlCtx,
                              PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                              PNP_OBJECT_TYPE_DEVICE,
                              instanceKey,
                              NULL,
                              &DEVPKEY_Device_HardwareConfigurationIndex,
                              DEVPROP_TYPE_UINT32,
                              (PUCHAR)&PnpCurrentHardwareConfigurationIndex,
                              sizeof(PnpCurrentHardwareConfigurationIndex),
                              setPropertyFlags);
    }

    //
    // Process Capabilities and ConfigFlags.
    //

    if ((capabilities.HardwareDisabled) &&
        (!PipIsDevNodeProblem(DeviceNode, CM_PROB_NOT_CONFIGURED)) &&
        (!PipIsDevNodeProblem(DeviceNode, CM_PROB_NEED_RESTART))) {

        PnpDisableDevice(DeviceNode, CM_PROB_HARDWARE_DISABLED);
    }

    if (!PipIsDevNodeProblem(DeviceNode, CM_PROB_INVALID_DATA) &&
        !PipIsDevNodeProblem(DeviceNode, CM_PROB_OUT_OF_MEMORY) &&
        !PipIsDevNodeProblem(DeviceNode, CM_PROB_REGISTRY)) {

        PiLockPnpRegistry(FALSE);

        //
        // Save device location.
        //

        if ((instanceKey != NULL) && (location != NULL)) {
            _CmSetDeviceRegProp(PiPnpRtlCtx,
                                PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                instanceKey,
                                CM_REG_PROP_LOCATION_INFORMATION,
                                REG_SZ,
                                (PUCHAR)location,
                                (ULONG)((wcslen(location)+1) * sizeof(WCHAR)),
                                setPropertyFlags);
        }

        PnpSaveDeviceCapabilities(DeviceNode,
                                  &capabilities,
                                  (disposition == REG_CREATED_NEW_KEY));

        //
        // Device should be pushed through configuration if it is new or needs
        // to be reconfigured, as established by checking its ConfigFlags.
        //

        PNP_ASSERT(instanceKey != NULL);

        problem = 0;
        configureDevice = (disposition == REG_CREATED_NEW_KEY) ? TRUE : FALSE;
        propertySize = sizeof(configFlags);
        status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                     instanceKey,
                                     CM_REG_PROP_CONFIGFLAGS,
                                     &propType,
                                     (PUCHAR)&configFlags,
                                     &propertySize,
                                     0);

        if ((NT_SUCCESS(status)) &&
            (propType == REG_DWORD) &&
            (propertySize == sizeof(configFlags))) {

            if (configFlags & CONFIGFLAG_REINSTALL) {

                problem = CM_PROB_REINSTALL;

                if (!(configFlags & CONFIGFLAG_DISABLED)) {

                    configureDevice = TRUE;
                }

            } else if (configFlags & CONFIGFLAG_FAILEDINSTALL) {

                problem = CM_PROB_FAILED_INSTALL;
            }

        } else {

            configFlags = 0;
            problem = CM_PROB_NOT_CONFIGURED;
            configureDevice = TRUE;
        }

        if (problem) {

            if (capabilities.RawDeviceOK) {

                configFlags |= CONFIGFLAG_FINISH_INSTALL;
                _CmSetDeviceRegProp(PiPnpRtlCtx,
                                    PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                    instanceKey,
                                    CM_REG_PROP_CONFIGFLAGS,
                                    REG_DWORD,
                                    (PUCHAR)&configFlags,
                                    sizeof(configFlags),
                                    setPropertyFlags);

            } else if (problem == CM_PROB_FAILED_INSTALL) {

                PipSetDevNodeProblem(DeviceNode,
                                     problem,
                                     PiDevCfgGetFailedInstallProblemStatus(
                                        PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                        instanceKey));

            } else {

                PipSetDevNodeProblem(DeviceNode, problem, STATUS_SUCCESS);
            }
        }
        status = PnpMapDeviceObjectToDeviceInstance(DeviceNode->PhysicalDeviceObject,
                                                    PnpGetDeviceInstancePath(DeviceNode));
        PNP_ASSERT(NT_SUCCESS(status));
        if (!NT_SUCCESS(status)) {

            finalStatus = status;
        }

        //
        // Indicate that this device arrived during boot if this is boot mode,
        // or mark it to possibly be updated.
        //

        if (PnpBootMode) {
            DeviceNode->StateFlags |= DNSF_ARRIVED_DURING_BOOT;

        } else if (configFlags & CONFIGFLAG_BOOT_DEVICE) {
            checkDeviceNeedsUpdate = TRUE;
        }

        //
        // Check if the device has firmware dependencies.  The device should be
        // pushed through configuration if it does.
        //

        if ((!configureDevice) &&
            (_PnpGetObjectProperty(PiPnpRtlCtx,
                                   PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                   PNP_OBJECT_TYPE_DEVICE,
                                   instanceKey,
                                   NULL,
                                   &DEVPKEY_Device_FirmwareDependencies,
                                   &propertyType,
                                   NULL,
                                   0,
                                   &propertySize,
                                   0) == STATUS_BUFFER_TOO_SMALL) &&
            (propertyType == DEVPROP_TYPE_STRING_LIST)) {

            configureDevice = TRUE;
        }

        PiUnlockPnpRegistry();
    }

    //
    // Query HardwareIDs, CompatibleIDs, ContainerID.
    //

    PnpQueryHardwareIDs(DeviceNode,
                        &hwIDs,
                        &hwIDsSize);

#if !defined(XBOX_SYSTEMOS)

    KseAddHardwareId( hwIDs );

#endif

    PnpQueryCompatibleIDs(DeviceNode,
                          &compatibleIDs,
                          &compatibleIDsSize);

    PnpGenerateDeviceIdsHash(hwIDs,
                             compatibleIDs,
                             &DeviceNode->DeviceIdsHash);

    PnpQueryContainerID(DeviceNode,
                        &baseContainerId);

    if (baseContainerId != NULL) {

        //
        // Verify that reported container ID is a valid GUID.
        //

        RtlInitUnicodeString(&unicodeString, baseContainerId);

        status = RtlGUIDFromString(&unicodeString, &guidBaseContainerId);

        if (NT_SUCCESS(status)) {
            IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                         "PnpQueryContainerID: "
                         "Queried Container ID %wZ for %wZ from %wZ\n",
                         &unicodeString,
                         PnpGetDeviceInstancePath(DeviceNode),
                         &DeviceNode->Parent->ServiceName));

        } else {
            IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                         "PnpQueryContainerID: "
                         "Bad Container ID (%wZ) reported for %wZ by %wZ (status %08lx)\n",
                         &unicodeString,
                         PnpGetDeviceInstancePath(DeviceNode),
                         &DeviceNode->Parent->ServiceName,
                         status));

            //
            // Free invalid container ID.
            //

            ExFreePool(baseContainerId);
            baseContainerId = NULL;
        }
    }

    //
    // Query devnode for a removable device override.
    //

    status = PiQueryRemovableDeviceOverride(DeviceNode,
                                            hwIDs,
                                            compatibleIDs,
                                            &removable);

    if (NT_SUCCESS(status)) {
        IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                     "PiProcessNewDeviceNode: "
                     "Found removable device override (%d) for %wZ\n",
                     (removable) ? 1 : 0,
                     PnpGetDeviceInstancePath(DeviceNode)));

    } else {

        //
        // Use the queried device capabilities to determine the removability of
        // the devnode when there is no removable device override.
        //

        if (capabilities.DockDevice) {

            //
            // Always treat dock devices as non-removable so that they are
            // grouped together with their parenting devices.
            //

            removable = FALSE;

        } else if (capabilities.Reserved1) {

            //
            // Use "integrated hardware" capability to indicate that this
            // device should be non-removable for grouping purposes since it
            // integrates into the container/chassis of its parent device,
            // even though it may technically be physically removable or
            // ejectable. This capability is set by ACPI bus filter driver
            // on any devices that have EJx ACPI namespace objects, which
            // gives them the EjectSupported and Removable capabilities.
            //

            removable = FALSE;

        } else {

            //
            // Use removable device capability, as reported by the bus driver.
            //

            removable = (capabilities.Removable != 0);
        }
    }

    //
    // Need to generate a container ID if there is a removable device override
    // for this devnode or the bus driver did not provide a container ID.
    //

    if ((NT_SUCCESS(status)) || (baseContainerId == NULL)) {

        //
        // Generate a container ID for the devnode, using the instance key to
        // check for an existing container ID, if required. Newly created
        // instance keys will not have container IDs, so a NULL is provided to
        // avoid the query entirely.
        //

        status = PipGenerateContainerID(DeviceNode,
                                        (disposition != REG_CREATED_NEW_KEY) ? instanceKey : NULL,
                                        removable,
                                        baseContainerId,
                                        &newBaseContainerId);

        if (NT_SUCCESS(status)) {

            //
            // Use new container ID, freeing queried container ID if needed.
            //

            if (baseContainerId != NULL) {
                ExFreePool(baseContainerId);
            }

            baseContainerId = newBaseContainerId;
        }
    }

    //
    // Store final base container ID on devnode.
    //

    if (baseContainerId != NULL) {
        RtlInitUnicodeString(&unicodeString, baseContainerId);

        status = RtlGUIDFromString(&unicodeString, &DeviceNode->ContainerID);

        if (!NT_SUCCESS(status)) {
            RtlZeroMemory(&DeviceNode->ContainerID, sizeof(GUID));
        }
    } else {
        RtlZeroMemory(&DeviceNode->ContainerID, sizeof(GUID));
    }

    status = STATUS_SUCCESS;

    PiLockPnpRegistry(FALSE);

    PipSetDevNodeFlags(DeviceNode, DNF_IDS_QUERIED);

    PiUnlockPnpRegistry();

    //
    // Save the base container id in the registry and
    // update the device's container membership.
    //

    if ((instanceKey != NULL) &&
        (baseContainerId != NULL)) {

        //
        // Prevent a race with mergin of containers in PiDcHandleCustomDeviceEvent.
        //

        PiLockPnpRegistry(TRUE);

        PiDcUpdateDeviceContainerMembership(PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                            instanceKey,
                                            baseContainerId);

        PiUnlockPnpRegistry();
    }

    PiLockPnpRegistry(FALSE);

    if (instanceKey != NULL) {

        //
        // Only check if the ids have changed if the device was not just
        // created.  If the device instance key was just created, there will be
        // no ids saved so the ids will definitely be different.
        //

        if ((!configureDevice) &&
            (disposition != REG_CREATED_NEW_KEY)) {

            status = PnpCheckDeviceIdsChanged(DeviceNode,
                                              instanceKey,
                                              hwIDs,
                                              hwIDsSize,
                                              TRUE,
                                              &idsChanged);

            if (!NT_SUCCESS(status)) {
                idsChanged = FALSE;
            }

            if (!idsChanged) {
                status = PnpCheckDeviceIdsChanged(DeviceNode,
                                                  instanceKey,
                                                  compatibleIDs,
                                                  compatibleIDsSize,
                                                  FALSE,
                                                  &idsChanged);

                if (!NT_SUCCESS(status)) {
                    idsChanged = FALSE;
                }
            }
        }

        //
        // Set the new IDs before checking for if the device needs an update.
        // If the new IDs are not set first, checking for if the device needs an
        // update will use the old IDs which is incorrect.
        //

        if (hwIDs != NULL) {
            _CmSetDeviceRegProp(PiPnpRtlCtx,
                                PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                instanceKey,
                                CM_REG_PROP_HARDWAREID,
                                REG_MULTI_SZ,
                                (PUCHAR)hwIDs,
                                hwIDsSize,
                                setPropertyFlags);
        }

        if (compatibleIDs != NULL) {
            _CmSetDeviceRegProp(PiPnpRtlCtx,
                                PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                instanceKey,
                                CM_REG_PROP_COMPATIBLEIDS,
                                REG_MULTI_SZ,
                                (PUCHAR)compatibleIDs,
                                compatibleIDsSize,
                                setPropertyFlags);
        }

        //
        // Check if the device should be marked for reinstall, either because
        // its ids changed or it was determined that there may be a better match.
        //

        if ((idsChanged) ||
            (checkDeviceNeedsUpdate)) {

            //
            // Check if device needs to be updated with a new driver.
            //

            if (!configureDevice) {
                status = PpDevCfgCheckDeviceNeedsUpdate(PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                        instanceKey,
                                                        &configFlagsUpdate);

                if (NT_SUCCESS(status)) {
                    configFlags |= configFlagsUpdate;

                } else {
                    configFlagsUpdate = 0;
                }
            }

            //
            // Update config flags if the device is marked as boot device, or
            // needs the reinstall flag.
            //

            if ((configFlags & CONFIGFLAG_BOOT_DEVICE) ||
                (configFlagsUpdate & CONFIGFLAG_REINSTALL)) {

                //
                // Remove boot device flag.
                //

                configFlags &= ~CONFIGFLAG_BOOT_DEVICE;

                //
                // Set new config flags on the device.
                //

                _CmSetDeviceRegProp(PiPnpRtlCtx,
                                    PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                    instanceKey,
                                    CM_REG_PROP_CONFIGFLAGS,
                                    REG_DWORD,
                                    (PUCHAR)&configFlags,
                                    sizeof(configFlags),
                                    setPropertyFlags);
            }

            if (configFlags & CONFIGFLAG_REINSTALL) {
                configureDevice = TRUE;
                PipSetDevNodeProblem(DeviceNode, CM_PROB_REINSTALL, STATUS_SUCCESS);
            }
        }
    }

    PiUnlockPnpRegistry();

    if (instanceKey != NULL) {

        //
        // Set DEVPKEY_Device_BusReportedDeviceDesc.
        //

        alternateStringAvailable = FALSE;
        descriptionLength = 0;
        tempString = NULL;
        tempStringLength = 0;

        if (description != NULL) {
            descriptionLength = (ULONG)((wcslen(description) + 1) * sizeof(WCHAR));
            alternateStringAvailable = PnpFindAlternateStringData(description,
                                                                  descriptionLength,
                                                                  &tempString,
                                                                  &tempStringLength);
        }

        //
        // IoSetDevicePropertyData is blocked from setting the bus reported
        // device description since drivers shouldn't be allowed to set it.
        // Because of that, pnprtl has to be used directly to set this property.
        //

        status = _PnpSetObjectProperty(PiPnpRtlCtx,
                                       PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                       PNP_OBJECT_TYPE_DEVICE,
                                       instanceKey,
                                       NULL,
                                       &DEVPKEY_Device_BusReportedDeviceDesc,
                                       alternateStringAvailable ?
                                           DEVPROP_TYPE_STRING_INDIRECT : DEVPROP_TYPE_STRING,
                                       (PUCHAR)description,
                                       descriptionLength,
                                       setPropertyFlags);

        //
        // Retrieve and save DEVPKEY_Device_LocationPaths.
        //

        status = PnpGetDeviceLocationStrings(DeviceNode,
                                             (PZZWSTR*)&tempString,
                                             &tempStringLength);

        if (NT_SUCCESS(status)) {
            _PnpSetObjectProperty(PiPnpRtlCtx,
                                  PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                  PNP_OBJECT_TYPE_DEVICE,
                                  instanceKey,
                                  NULL,
                                  &DEVPKEY_Device_LocationPaths,
                                  DEVPROP_TYPE_STRING_LIST,
                                  (PUCHAR)tempString,
                                  tempStringLength * sizeof(WCHAR),
                                  setPropertyFlags);

            ExFreePool(tempString);
        }
    }

    PnpQueryBusInformation(DeviceNode);

    //
    // Configure device with a driver if possible.
    //

    if ((!capabilities.HardwareDisabled) &&
        (!PipIsDevNodeProblem(DeviceNode, CM_PROB_NEED_RESTART))) {

        if (configureDevice) {
            PpDevCfgProcessDevice(DeviceNode,
                                  instanceKey,
                                  ((disposition == REG_CREATED_NEW_KEY) ?
                                       PP_DEVCFG_CONFIGURE_NEW_DEVICE : 0));

        } else {
            PpDevCfgProcessDeviceOperations(DeviceNode, instanceKey);
        }
    }

    PiQueryResourceRequirements(DeviceNode);

    PNP_ASSERT(!PipDoesDevNodeHaveProblem(DeviceNode) ||
               PipIsDevNodeProblem(DeviceNode, CM_PROB_NOT_CONFIGURED) ||
               PipIsDevNodeProblem(DeviceNode, CM_PROB_REINSTALL) ||
               PipIsDevNodeProblem(DeviceNode, CM_PROB_FAILED_INSTALL) ||
               PipIsDevNodeProblem(DeviceNode, CM_PROB_PARTIAL_LOG_CONF) ||
               PipIsDevNodeProblem(DeviceNode, CM_PROB_HARDWARE_DISABLED) ||
               PipIsDevNodeProblem(DeviceNode, CM_PROB_NEED_RESTART) ||
               PipIsDevNodeProblem(DeviceNode, CM_PROB_DUPLICATE_DEVICE) ||
               PipIsDevNodeProblem(DeviceNode, CM_PROB_INVALID_DATA) ||
               PipIsDevNodeProblem(DeviceNode, CM_PROB_OUT_OF_MEMORY) ||
               PipIsDevNodeProblem(DeviceNode, CM_PROB_REGISTRY));

    if (!PipIsDevNodeProblem(DeviceNode, CM_PROB_DISABLED) &&
        !PipIsDevNodeProblem(DeviceNode, CM_PROB_HARDWARE_DISABLED) &&
        !PipIsDevNodeProblem(DeviceNode, CM_PROB_NEED_RESTART) &&
        !PipIsDevNodeProblem(DeviceNode, CM_PROB_INVALID_DATA) &&
        !PipIsDevNodeProblem(DeviceNode, CM_PROB_OUT_OF_MEMORY) &&
        !PipIsDevNodeProblem(DeviceNode, CM_PROB_REGISTRY)) {

        PnpIsDeviceInstanceEnabled(instanceKey,
                                   PnpGetDeviceInstancePath(DeviceNode),
                                   TRUE);
    }

    if (instanceKey != NULL) {
        //
        // Initialize the session ID based on the parent.
        //
        PnpInitializeSessionId(DeviceNode,
                               instanceKey,
                               (disposition == REG_CREATED_NEW_KEY));
    }

    PiQueryAndAllocateBootResources(DeviceNode);

    status = STATUS_SUCCESS;

    //
    // Finalize the enumeration and send notifications.
    //

    if (!PipIsDevNodeProblem(DeviceNode, CM_PROB_INVALID_DATA) &&
        !PipIsDevNodeProblem(DeviceNode, CM_PROB_OUT_OF_MEMORY) &&
        !PipIsDevNodeProblem(DeviceNode, CM_PROB_REGISTRY)) {

        PiLockPnpRegistry(FALSE);
        PnpSaveDeviceCapabilities(DeviceNode,
                                  &capabilities,
                                  (disposition == REG_CREATED_NEW_KEY));
        PiUnlockPnpRegistry();
        PpHotSwapUpdateRemovalPolicy(DeviceNode);
        status = PpDeviceRegistration(PnpGetDeviceInstancePath(DeviceNode),
                                      TRUE,
                                      &DeviceNode->ServiceName,
                                      FALSE);

        if (!NT_SUCCESS(status)) {
            PNP_SAVE_FAILURE_INFO(DeviceNode, status);
            PipSetDevNodeProblem(DeviceNode, CM_PROB_REGISTRY, status);
        }
    }

    if (!PipIsDevNodeProblem(DeviceNode, CM_PROB_INVALID_DATA) &&
        !PipIsDevNodeProblem(DeviceNode, CM_PROB_OUT_OF_MEMORY) &&
        !PipIsDevNodeProblem(DeviceNode, CM_PROB_REGISTRY)) {

        //
        // Let the bus driver know that the device has been enumerated by PnP.
        // Do this before notifying anyone that a new device has been enumerated.
        //

        PnpIrpDeviceEnumerated(deviceObject);

        //
        // Raise PnpRtl events for the updated NtPlugPlay properties.
        //

        _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                     NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_IS_PRESENT);

        _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                     NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_PARENT);

        //
        // Add an event so user-mode will attempt to install this device later.
        //

        PnpSetPlugPlayEvent(&GUID_DEVICE_ENUMERATED, deviceObject);

        //
        // Notify the dependency manager in case this PDO is
        // involved in any dependency relations.
        //

        PnpNewDeviceNodeDependencyCheck(DeviceNode);
    }

    //
    // Cleanup.
    //

    if (hwIDs) {
        ExFreePool(hwIDs);
    }

    if (compatibleIDs) {
        ExFreePool(compatibleIDs);
    }

    if (baseContainerId) {
        ExFreePool(baseContainerId);
    }

    if (instanceKey) {
        ZwClose(instanceKey);
    }

    if (instanceID) {
        ExFreePool(instanceID);
    }

    if (location) {
        ExFreePool(location);
    }

    if (description) {
        ExFreePool(description);
    }

    if (busID) {
        ExFreePool(busID);
    }

    if (hPnpOpCtx) {
        PiPnpRtlEndOperation(hPnpOpCtx);
    }

    //
    // Trace an event indicating a new device has finished being processed.  This
    // can help in diagnosing performance issues.
    //
    EventWriteKMPnPEvt_ProcessNewDevice_Stop(NULL, DeviceNode);
    if (DeviceNode->DockInfo.DockStatus != DOCK_NOTDOCKDEVICE) {
        PnpTraceDockDeviceEnumeration(DeviceNode, finalStatus);
    }

    return finalStatus;
}

NTSTATUS
PipGenerateContainerID(
    __in        PDEVICE_NODE DeviceNode,
    __in_opt    HANDLE       DeviceInstanceKey,
    __in        BOOLEAN      Removable,
    __in_opt    PCWSTR       QueriedContainerID,
    __deref_out PWSTR        *ContainerID
    )

/*++

Routine Description:

    This function will generate a container ID for a devnode that does not
    report one.

    Removable devnodes are given newly generated container IDs to identify
    them as entry points into new device containers, where all child devnodes
    are considered part of the container unless they too are removable.

    Non-removable devnodes inherit container IDs from their parent devnodes,
    which are expected to already have their container IDs from earlier
    initialization in the devnode tree.

    Existing generated container IDs are persisted, so a new container ID is
    only generated for a removable devnode when it does not already have a
    container ID or the existing container ID was previously inherited from
    a parent devnode.

Arguments:

    DeviceNode         - Devnode to generate a container ID for.

    DeviceInstanceKey  - Optional opened device instance key to query for an
                         existing container ID, where NULL indicates that
                         devnode is new and does not have a container ID yet.

    Removable          - TRUE indicates that devnode is removable,
                         FALSE indicates that devnode is not removable.

    QueriedContainerID - Optional queried container ID to use instead of
                         generating a new one when devnode is removable.

    ContainerID        - Returns an allocated buffer with the container ID.

Return Value:

    NTSTATUS.

--*/

{
    BOOLEAN         generate = FALSE;
    GUID            guidContainerID;
    PDEVICE_NODE    parentNode;
    ULONG           propertySize;
    ULONG           propertyType;
    NTSTATUS        status = STATUS_SUCCESS;
    WCHAR           tempContainerID[GUID_STRING_LEN];
    UNICODE_STRING  unicodeContainerID;

    PAGED_CODE();

    PNP_ASSERT(DeviceNode != NULL);
    PNP_ASSERT(DeviceNode->Parent != NULL);
    PNP_ASSERT(ContainerID != NULL);

    *ContainerID = NULL;

    PiWstrToUnicodeString(&unicodeContainerID, NULL);

    //
    // Generate a new container ID when the device is removable.
    //
    if (Removable) {
        //
        // Generate a new container ID, unless the devnode already has one
        // and it was not previously inherited from the parent devnode, or
        // a queried container ID is available.
        //
        if (QueriedContainerID != NULL) {
            //
            // Use queried container ID instead of generating a new one.
            //
            if (!RtlCreateUnicodeString(&unicodeContainerID,
                                        QueriedContainerID)) {
                status = STATUS_INSUFFICIENT_RESOURCES;
            }
        } else if (DeviceInstanceKey != NULL) {
            //
            // Query device instance key for existing container ID.
            //
            PiLockPnpRegistry(FALSE);

            propertySize = GUID_STRING_LEN * sizeof(WCHAR);
            status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                         PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                         DeviceInstanceKey,
                                         CM_REG_PROP_BASE_CONTAINERID,
                                         &propertyType,
                                         (PUCHAR)tempContainerID,
                                         &propertySize,
                                         0);

            PiUnlockPnpRegistry();

            if (NT_SUCCESS(status)) {
                //
                // Verify that the existing container ID is valid.
                //
                if (propertyType == REG_SZ) {
                    if (RtlCreateUnicodeString(&unicodeContainerID,
                                               (PCWSTR)tempContainerID)) {
                        status = RtlGUIDFromString(&unicodeContainerID,
                                                   &guidContainerID);

                        if (!NT_SUCCESS(status)) {
                            RtlFreeUnicodeString(&unicodeContainerID);
                        }
                    } else {
                        status = STATUS_INSUFFICIENT_RESOURCES;
                    }
                } else {
                    status = STATUS_INVALID_PARAMETER;
                }

            } else if (status == STATUS_BUFFER_TOO_SMALL) {
                //
                // If the provided buffer was too small to hold the property, then
                // the property must not contain a valid GUID.  Return the error
                // that would be returned by RtlGUIDFromString if the property had
                // actually been retrieved and passed to that function.
                //
                status = STATUS_INVALID_PARAMETER;
            }

            //
            // Check if the existing container ID was inherited from any
            // parenting devnodes, including root devnode.
            //
            if (NT_SUCCESS(status)) {
                for (parentNode = DeviceNode->Parent;
                     parentNode != NULL;
                     parentNode = parentNode->Parent) {
                    //
                    // Check for matching container IDs, indicating that the
                    // current container ID must have once been inherited from
                    // the parent devnode and a new one should be generated.
                    //
                    if (IopCompareGuid(&parentNode->ContainerID,
                                       &guidContainerID)) {
                        RtlFreeUnicodeString(&unicodeContainerID);
                        generate = TRUE;
                        break;
                    }
                }
            } else {
                //
                // Generate new container ID when existing one in unavailable.
                //
                generate = TRUE;
            }
        } else {
            //
            // Generate new container ID for new devnode.
            //
            generate = TRUE;
        }

        if (generate) {
            //
            // Generate a GUID as the container ID.
            //
            status = ExUuidCreate(&guidContainerID);

            //
            // Check if the GUID was successfully created, where a local only
            // GUID is acceptable since it is better than no GUID at all.
            //
            if ((NT_SUCCESS(status)) ||
                (status == RPC_NT_UUID_LOCAL_ONLY)) {
                //
                // Convert the container ID GUID to a string.
                //
                status = RtlStringFromGUID(&guidContainerID,
                                           &unicodeContainerID);
            }

            if (NT_SUCCESS(status)) {
                IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                             "PipGenerateContainerID: "
                             "Generated Container ID %wZ for %wZ\n",
                             &unicodeContainerID,
                             PnpGetDeviceInstancePath(DeviceNode)));

            } else {
                IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                             "PipGenerateContainerID: "
                             "Failed to generate Container ID for %wZ (status %08lx)\n",
                             PnpGetDeviceInstancePath(DeviceNode),
                             status));
            }
        } else {
            //
            // Leave existing container ID on devnode.
            //
            IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                         "PipGenerateContainerID: "
                         "Leaving existing Container ID %wZ for %wZ\n",
                         &unicodeContainerID,
                         PnpGetDeviceInstancePath(DeviceNode)));
        }
    } else {
        //
        // Inherit container ID from the parent devnode.
        //
        status = RtlStringFromGUID(&DeviceNode->Parent->ContainerID,
                                   &unicodeContainerID);

        if (NT_SUCCESS(status)) {
            IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                         "PipGenerateContainerID: "
                         "Inheriting Container ID %wZ for %wZ from %wZ\n",
                         &unicodeContainerID,
                         PnpGetDeviceInstancePath(DeviceNode),
                         PnpGetDeviceInstancePath(DeviceNode->Parent)));
        }
    }

    //
    // Return allocated buffer with container ID on success.
    //
    if ((NT_SUCCESS(status)) &&
        (unicodeContainerID.Buffer != NULL)) {
        //
        // Allocate paged buffer for container ID string.
        //
        *ContainerID = (PWCHAR)ExAllocatePool(PagedPool,
                                              unicodeContainerID.MaximumLength);

        if (*ContainerID != NULL) {
            //
            // Copy GUID string to container ID string.
            //
            RtlCopyMemory(*ContainerID,
                          unicodeContainerID.Buffer,
                          unicodeContainerID.MaximumLength);

        } else {
            status = STATUS_INSUFFICIENT_RESOURCES;
        }

        RtlFreeUnicodeString(&unicodeContainerID);
    }

    return status;
}

NTSTATUS
PipCallDriverAddDevice(
    __in PDEVICE_NODE DeviceNode,
    __in PADD_CONTEXT Context
    )

/*++

Routine Description:

    This function checks if the driver for the DeviceNode is present and loads
    the driver if necessary.

Arguments:

    DeviceNode - Supplies a pointer to the device node to be enumerated.

    Context - Supplies a pointer to ADD_CONTEXT to control how the device be added.

Return Value:

    NTSTATUS code.

--*/

{
    WCHAR classGuid[GUID_STRING_LEN];
    HANDLE classKey = NULL;
    PDEVICE_OBJECT deviceObject;
    BOOLEAN deviceRaw = FALSE;
    ULONG dwordValue = 0;
    PDEVICE_OBJECT fdoDeviceObject;
    HANDLE instanceKey = NULL;
    USHORT node;
    PUCHAR propertyBuffer;
    ULONG propertySize;
    ULONG propertyType;
    ULONG proximityId;
    QUERY_CONTEXT queryContext = {0};
    BOOLEAN rawStack;
    ULONG size;
    NTSTATUS status;
    PDEVICE_OBJECT tempFdo;
    PDEVICE_OBJECT topOfLowerFilterStack;
    PDEVICE_OBJECT topOfPdoStack;
    ULONG type;
    UNICODE_STRING unicodeClassGuid;
    BOOLEAN usePdoCharacteristics = TRUE;

    PAGED_CODE();

    classGuid[0] = UNICODE_NULL;
    propertyBuffer = NULL;
    topOfPdoStack = NULL;
    topOfLowerFilterStack = NULL;

    //
    // The device node may have been started at this point.  This is because
    // some ill-behaved miniport drivers call IopReportedDetectedDevice at
    // DriverEntry for the devices which we already know about.
    //

    ASSERT_INITED(DeviceNode->PhysicalDeviceObject);

    //
    // Check if a previous attempt to load drivers for this device failed due to
    // the current add context not allowing new driver loads (since the disk
    // stack is likely not up).
    //
    // NOTE: This is an important boot time optimization. This should be checked
    // first thing in this function.
    //

    if ((DeviceNode->RequiresUnloadedDriver != FALSE) &&
        (Context->LoadDriver == FALSE)) {

        status = STATUS_UNSUCCESSFUL;
        goto Cleanup;

    } else {
        DeviceNode->RequiresUnloadedDriver = FALSE;
    }

    //
    // Open the instance key for this devnode.
    //

    status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                 PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                 CM_KEY_TYPE_DEVICE_INSTANCE,
                                 0,
                                 KEY_READ,
                                 FALSE,
                                 &instanceKey,
                                 NULL);

    if (!NT_SUCCESS(status)) {

        IopDbgPrint((IOP_LOADUNLOAD_ERROR_LEVEL,
                     __FUNCTION__ "-\t\tError %#08lx opening device instance key %wZ!\n",
                     status,
                     PnpGetDeviceInstancePath(DeviceNode)));

        goto Cleanup;
    }

    //
    // Validate that this device is not used by the debugger, when it is make
    // sure the driver has opted in to co-exist with the debugging device and
    // wants to be loaded. When the driver has not opted in and the device is
    // used for debugging, do not call add device.
    //

    if (DeviceNode->PhysicalDeviceObject->Flags & DO_DEVICE_USED_BY_DEBUGGER) {
        status = _PnpGetObjectProperty(PiPnpRtlCtx,
                                       PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                       PNP_OBJECT_TYPE_DEVICE,
                                       instanceKey,
                                       NULL,
                                       &DEVPKEY_Device_DebuggerSafe,
                                       &type,
                                       (PUCHAR)&dwordValue,
                                       sizeof(dwordValue),
                                       &size,
                                       0);

        //
        // In case the key was not retrievable or set to zero.
        //

        if ((!NT_SUCCESS(status)) ||
            (type != DEVPROP_TYPE_UINT32) ||
            (size != sizeof(dwordValue)) ||
            (dwordValue == 0)) {

            PnpRequestDeviceRemoval(DeviceNode,
                                    FALSE,
                                    CM_PROB_USED_BY_DEBUGGER,
                                    STATUS_SUCCESS);

            status = STATUS_PNP_RESTART_ENUMERATION;
            goto Cleanup;
        }
    }

    //
    // Update NUMA information for this device.
    //

    if (DeviceNode->NumaNodeIndex == PNP_NUMA_NODE_UNKNOWN) {
        if (PnpQueryProximityNode != NULL) {

            //
            // Check if the NUMA proximity domain has been specified for this
            // device.
            //

            status = _PnpGetObjectProperty(PiPnpRtlCtx,
                                           PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                           PNP_OBJECT_TYPE_DEVICE,
                                           instanceKey,
                                           NULL,
                                           &DEVPKEY_Numa_Proximity_Domain,
                                           &type,
                                           (PUCHAR)&dwordValue,
                                           sizeof(dwordValue),
                                           &size,
                                           0);

            if (NT_SUCCESS(status) && (dwordValue <= MAXUCHAR)) {

                //
                // Translate the proximity id into NUMA node.
                //

                proximityId = dwordValue;
                status = PnpQueryProximityNode(proximityId, &node);
                if (NT_SUCCESS(status)) {
                    DeviceNode->NumaNodeIndex = node;
                }
            }
        }

        if (DeviceNode->NumaNodeIndex == PNP_NUMA_NODE_UNKNOWN) {

            //
            // If firmware did not specify a valid proximity id, inherit
            // NUMA information from the parent device.
            //

            if (DeviceNode->Parent) {
                DeviceNode->NumaNodeIndex = DeviceNode->Parent->NumaNodeIndex;

            } else {
                DeviceNode->NumaNodeIndex = PNP_NUMA_NODE_NONE;
            }
        }

        //
        // Store the node as a device property so that it can be queried
        // externally.
        //

        if (DeviceNode->NumaNodeIndex != PNP_NUMA_NODE_NONE) {
            dwordValue = DeviceNode->NumaNodeIndex;
            _PnpSetObjectProperty(PiPnpRtlCtx,
                                  PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                  PNP_OBJECT_TYPE_DEVICE,
                                  instanceKey,
                                  NULL,
                                  &DEVPKEY_Device_Numa_Node,
                                  DEVPROP_TYPE_UINT32,
                                  (PUCHAR)&dwordValue,
                                  sizeof(dwordValue),
                                  0);
        }
    }

    //
    // Get the class value to locate the class key for this devnode.
    //

    propertySize = GUID_STRING_LEN * sizeof(WCHAR);
    status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                 PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                 instanceKey,
                                 CM_REG_PROP_CLASSGUID,
                                 &propertyType,
                                 (PUCHAR)classGuid,
                                 &propertySize,
                                 0);

    PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);
    PNP_ASSERT(status != STATUS_BUFFER_OVERFLOW);

    if (NT_SUCCESS(status)) {

        if ((propertyType == REG_SZ) && (propertySize != 0)) {

            status = RtlInitUnicodeStringEx(&unicodeClassGuid, classGuid);

            PNP_ASSERT(NT_SUCCESS(status));
            status = STATUS_SUCCESS;

            IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                         __FUNCTION__"-\t\tClass is %wZ for %wZ\n",
                         &unicodeClassGuid,
                         PnpGetDeviceInstancePath(DeviceNode)));

            if (InitSafeBootMode) {

                if (!IopSafebootDriverLoad(&unicodeClassGuid)) {

                    PWSTR description;
                    NTSTATUS s;

                    s = STATUS_SUCCESS;

                    //
                    // Don't load this driver.
                    //

                    IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                                 __FUNCTION__ "- SAFEBOOT, skipping loading driver for device class %wZ\n",
                                 &unicodeClassGuid));

                    //
                    // Take a guess at how much buffer might be needed for the device
                    // description.
                    //

                    propertySize = 256;
                    description = ExAllocatePool(PagedPool, propertySize);

                    if (description == NULL) {
                        s = STATUS_INSUFFICIENT_RESOURCES;
                    }

                    if (NT_SUCCESS(s)) {
                        s = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                                PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                instanceKey,
                                                CM_REG_PROP_DEVICEDESC,
                                                &propertyType,
                                                (PUCHAR)description,
                                                &propertySize,
                                                0);

                        if (s == STATUS_BUFFER_TOO_SMALL) {

                            //
                            // Buffer was too small.  Allocate a buffer that is
                            // correct size.
                            //

                            ExFreePool(description);

                            description = ExAllocatePool(PagedPool, propertySize);

                            if (description == NULL) {
                                s = STATUS_INSUFFICIENT_RESOURCES;
                            }

                            if (NT_SUCCESS(s)) {
                                s = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                                        PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                        instanceKey,
                                                        CM_REG_PROP_DEVICEDESC,
                                                        &propertyType,
                                                        (PUCHAR)description,
                                                        &propertySize,
                                                        0);

                                PNP_ASSERT(s != STATUS_BUFFER_TOO_SMALL);
                            }
                        }
                    }

                    if (NT_SUCCESS(s)) {

                        UNICODE_STRING ClassString;

                        RtlInitUnicodeString(&ClassString, description);
                        IopBootLog(&ClassString, FALSE);

                    } else {

                        IopBootLog(&unicodeClassGuid, FALSE);
                    }

                    if (description != NULL) {
                        ExFreePool(description);
                    }

                    status = STATUS_UNSUCCESSFUL;
                    goto Cleanup;
                }
            }

            if (NT_SUCCESS(status)) {
                status = _CmOpenInstallerClassRegKey(PiPnpRtlCtx,
                                                     classGuid,
                                                     CM_KEY_TYPE_DEVICE_INSTALLER_CLASS,
                                                     0,
                                                     KEY_READ,
                                                     FALSE,
                                                     &classKey,
                                                     NULL);

                if (!NT_SUCCESS(status)) {
                    IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                                 __FUNCTION__ "-\tError %#08lx opening key %wZ!\n",
                                 status,
                                 &unicodeClassGuid));
                }
            }
        }
    }

    //
    // Check to see if there's a service assigned to this device node.  If
    // there's not then we can bail out without wasting too much time.
    //

    queryContext.DeviceNode = DeviceNode;
    queryContext.AddContext = Context;

    status = STATUS_SUCCESS;

    propertySize = MAX_SERVICE_NAME_LEN * sizeof(WCHAR);

    propertyBuffer = ExAllocatePool(PagedPool, propertySize);

    if (propertyBuffer == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
    }

    if (NT_SUCCESS(status)) {
        if ((DeviceNode->Flags & DNF_NO_LOWER_DEVICE_FILTERS) == 0){

            status = PnpCallDriverQueryServiceHelper(&propertyBuffer,
                                                     &propertySize,
                                                     CM_REG_PROP_LOWERFILTERS,
                                                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                     instanceKey,
                                                     FALSE,
                                                     LowerDeviceFilters,
                                                     &queryContext);

            if ((status == STATUS_OBJECT_NAME_NOT_FOUND) ||
                (status == STATUS_NOT_FOUND)) {

                PipSetDevNodeFlags(DeviceNode, DNF_NO_LOWER_DEVICE_FILTERS);
                status = STATUS_SUCCESS;
            }

            if (!NT_SUCCESS(status)) {

                IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                             __FUNCTION__ "-\t\tError %#08lx building list of lower device filters for %wZ\n",
                             status,
                             PnpGetDeviceInstancePath(DeviceNode)));

            }
        }
    }

    if (NT_SUCCESS(status)) {

        if ((classKey != NULL) &&
            ((DeviceNode->Flags & DNF_NO_LOWER_CLASS_FILTERS) == 0)) {

            status = PnpCallDriverQueryServiceHelper(&propertyBuffer,
                                                     &propertySize,
                                                     CM_REG_PROP_LOWERFILTERS,
                                                     classGuid,
                                                     classKey,
                                                     TRUE,
                                                     LowerClassFilters,
                                                     &queryContext);

            if ((status == STATUS_OBJECT_NAME_NOT_FOUND) ||
                (status == STATUS_NOT_FOUND)) {

                PipSetDevNodeFlags(DeviceNode, DNF_NO_LOWER_CLASS_FILTERS);
                status = STATUS_SUCCESS;
            }

            if (!NT_SUCCESS(status)) {
                IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                             __FUNCTION__ "-\t\tError %#08lx building list of "
                                          "lower class filters for %wZ!\n",
                             status,
                             PnpGetDeviceInstancePath(DeviceNode)));
            }
        }
    }

    if (NT_SUCCESS(status)) {

        if ((DeviceNode->Flags & DNF_NO_SERVICE) == 0) {

            status = PnpCallDriverQueryServiceHelper(&propertyBuffer,
                                                     &propertySize,
                                                     CM_REG_PROP_SERVICE,
                                                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                     instanceKey,
                                                     FALSE,
                                                     DeviceService,
                                                     &queryContext);

            if ((status == STATUS_OBJECT_NAME_NOT_FOUND) ||
                (status == STATUS_NOT_FOUND)) {

                status = STATUS_OBJECT_NAME_NOT_FOUND;
                PipSetDevNodeFlags(DeviceNode, DNF_NO_SERVICE);
            }

        } else {
            status = STATUS_OBJECT_NAME_NOT_FOUND;
        }

        if (!NT_SUCCESS(status)) {
            IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                         __FUNCTION__ "-\t\tError %#08lx processing service value for %wZ\n",
                         status,
                         PnpGetDeviceInstancePath(DeviceNode)));

        }

    }

    if (DeviceNode->Flags & DNF_LEGACY_DRIVER) {

        //
        // One of the services for this device is a legacy driver.  Don't try
        // to add any filters since we'll just mess up the device stack.
        //

        status = STATUS_SUCCESS;
        goto Cleanup;

    } else if (NT_SUCCESS(status)) {

        //
        // Call was successful so we must have been able to reference the
        // driver object.
        //

        PNP_ASSERT(queryContext.DriverLists[DeviceService] != NULL);

        if (queryContext.DriverLists[DeviceService]->NextEntry != NULL) {

            //
            // There's more than one service assigned to this device.  Configuration
            // error.

            IopDbgPrint((IOP_LOADUNLOAD_ERROR_LEVEL,
                         __FUNCTION__ "- Configuration error, more "
                         "than one service in driver list!\n"));

            PipSetDevNodeProblem(DeviceNode, CM_PROB_REGISTRY, STATUS_DEVICE_CONFIGURATION_ERROR);

            status = STATUS_UNSUCCESSFUL;

            goto Cleanup;
        }

        //
        // This is the only case (FDO specified) where we can ignore PDO's
        // characteristics.
        //

        usePdoCharacteristics = FALSE;

    } else if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
        if (IopDeviceNodeFlagsToCapabilities(DeviceNode)->RawDeviceOK) {

            //
            // Raw device access is okay.
            //

            PipClearDevNodeProblem(DeviceNode);
            usePdoCharacteristics = TRUE; // shouldn't need to do this, but better be safe than sorry
            deviceRaw = TRUE;
            status = STATUS_SUCCESS;

        } else {

            //
            // The device cannot be used raw.  Bail out now.
            //

            status = STATUS_UNSUCCESSFUL;
            goto Cleanup;
        }

    } else {

        //
        // something else went wrong while parsing the service key.  The
        // query routine will have set the flags appropriately so we can
        // just bail out.
        //

        goto Cleanup;

    }

    //
    // For each type of filter driver we want to build a list of the driver
    // objects to be loaded.  We'll build all the driver lists if we can
    // and deal with error conditions afterwards.
    //

     //
     // First get all the information we have to out of the instance key and
     // the device node.
     //

     if ((DeviceNode->Flags & DNF_NO_UPPER_DEVICE_FILTERS) == 0) {

         status = PnpCallDriverQueryServiceHelper(&propertyBuffer,
                                                  &propertySize,
                                                  CM_REG_PROP_UPPERFILTERS,
                                                  PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                  instanceKey,
                                                  FALSE,
                                                  UpperDeviceFilters,
                                                  &queryContext);

         if ((status == STATUS_OBJECT_NAME_NOT_FOUND) ||
             (status == STATUS_NOT_FOUND)) {

             PipSetDevNodeFlags(DeviceNode, DNF_NO_UPPER_DEVICE_FILTERS);
             status = STATUS_SUCCESS;
         }

         if (!NT_SUCCESS(status)) {

             IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                          __FUNCTION__ "-\t\tError %#08lx building list of upper "
                          "device filters for %wZ\n",
                          status,
                          PnpGetDeviceInstancePath(DeviceNode)));

         }
     }

     if (NT_SUCCESS(status)) {
         if (classKey &&
             ((DeviceNode->Flags & DNF_NO_UPPER_CLASS_FILTERS) == 0)) {

             status = PnpCallDriverQueryServiceHelper(&propertyBuffer,
                                                      &propertySize,
                                                      CM_REG_PROP_UPPERFILTERS,
                                                      classGuid,
                                                      classKey,
                                                      TRUE,
                                                      UpperClassFilters,
                                                      &queryContext);

             if ((status == STATUS_OBJECT_NAME_NOT_FOUND) ||
                 (status == STATUS_NOT_FOUND)) {

                 PipSetDevNodeFlags(DeviceNode, DNF_NO_UPPER_CLASS_FILTERS);
                 status = STATUS_SUCCESS;

             } else if (!NT_SUCCESS(status)) {
                 IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                              __FUNCTION__ "-\t\tError %#08lx building list of upper class filters for %wZ!\n",
                              status,
                              PnpGetDeviceInstancePath(DeviceNode)));
             }
         }
    }

    if (NT_SUCCESS(status)) {

        UCHAR serviceType = 0;
        PDRIVER_LIST_ENTRY listEntry = queryContext.DriverLists[serviceType];

        //
        // Make sure there's no more than one device service.  Anything else is
        // a configuration error.
        //

        PNP_ASSERT(!(DeviceNode->Flags & DNF_LEGACY_DRIVER));

        PNP_ASSERTMSG("Error - Device has no service but cannot be run RAW\n",
                      ((queryContext.DriverLists[DeviceService] != NULL) || (deviceRaw)));

        //
        // Do preinit work.
        //
        fdoDeviceObject = NULL;
        topOfPdoStack = IoGetAttachedDeviceReference(DeviceNode->PhysicalDeviceObject);

        //
        // It's okay to try adding all the drivers.
        //
        for (serviceType = 0; serviceType < MaximumAddStage; serviceType++) {

            if (serviceType == DeviceService) {

                topOfLowerFilterStack = IoGetAttachedDeviceReference(
                                            DeviceNode->PhysicalDeviceObject);

                if (deviceRaw && (queryContext.DriverLists[serviceType] == NULL)) {

                    //
                    // Mark the devnode as added, as it has no service.
                    //

                    PNP_ASSERT(queryContext.DriverLists[serviceType] == NULL);
                    PipSetDevNodeState(DeviceNode, DeviceNodeDriversAdded, NULL);

                } else {

                    //
                    // Since we are going to see a service, grab a pointer to
                    // the current top of the stack. While here, assert there
                    // is exactly one service driver to load...
                    //
                    PNP_ASSERT(queryContext.DriverLists[serviceType]);
                    PNP_ASSERT(!queryContext.DriverLists[serviceType]->NextEntry);
                }
            }

            for (listEntry = queryContext.DriverLists[serviceType];
                listEntry != NULL;
                listEntry = listEntry->NextEntry) {

                PDRIVER_ADD_DEVICE addDeviceRoutine;

                IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                             __FUNCTION__ "-\tAdding driver %wZ\n",
                             &listEntry->DriverObject->DriverName));

                PNP_ASSERT(listEntry->DriverObject);
                PNP_ASSERT(listEntry->DriverObject->DriverExtension);
                PNP_ASSERT(listEntry->DriverObject->DriverExtension->AddDevice);

                //
                // Invoke the driver's AddDevice() entry point.
                //
                addDeviceRoutine =
                    listEntry->DriverObject->DriverExtension->AddDevice;

                status = PnpCallAddDevice(DeviceNode,
                                          listEntry->DriverObject,
                                          addDeviceRoutine,
                                          serviceType);
                IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                             __FUNCTION__ "-\t\tAddDevice from %wZ returned %#08lx\n",
                             &listEntry->DriverObject->DriverName,
                             status));

                listEntry->DriverObject->Flags |= DRVO_ADDDEVICE_CALLED;

                if (NT_SUCCESS(status)) {

                    //
                    // If this is a service, mark the  it is legal for a filter to succeed AddDevice
                    // but fail to attach anything to the top of the stack.
                    //
                    if (serviceType == DeviceService) {

                        fdoDeviceObject = topOfLowerFilterStack->AttachedDevice;
                        PNP_ASSERT(fdoDeviceObject ||
                                   IopDeviceNodeFlagsToCapabilities(DeviceNode)->RawDeviceOK);
                    }

                    PipSetDevNodeState(DeviceNode, DeviceNodeDriversAdded, NULL);

                } else if (serviceType == DeviceService) {

                    //
                    // Mark the stack appropriately.
                    //
                    IovUtilMarkStack(DeviceNode->PhysicalDeviceObject,
                                     topOfPdoStack->AttachedDevice,
                                     fdoDeviceObject,
                                     FALSE);

                    //
                    // If service fails, then add failed. (Alternately, if
                    // filter drivers return failure, we keep going.)
                    //
                    DeviceNode->CompletionStatus = status;

                    PnpRequestDeviceRemoval(DeviceNode,
                                            FALSE,
                                            CM_PROB_FAILED_ADD,
                                            status);

                    status = STATUS_PNP_RESTART_ENUMERATION;
                    goto Cleanup;
                }

                tempFdo = IoGetAttachedDeviceReference(
                            DeviceNode->PhysicalDeviceObject);
                ASSERT_INITED(tempFdo);
                ObDereferenceObject(tempFdo);
            }
        }

        //
        // Mark the stack appropriately. We tell the verifier the stack is raw
        // if the fdo is NULL and we made it this far.
        //
        if ((fdoDeviceObject == NULL) || deviceRaw) {
            rawStack = TRUE;
        }
        else {
            rawStack = FALSE;
        }

        IovUtilMarkStack(DeviceNode->PhysicalDeviceObject,
                         topOfPdoStack->AttachedDevice,
                         fdoDeviceObject,
                         rawStack);

        //
        // change PDO and all attached objects
        // to have properties specified in the registry
        //
        status = PipChangeDeviceObjectFromRegistryProperties(DeviceNode->PhysicalDeviceObject,
                                                             instanceKey,
                                                             classGuid,
                                                             classKey,
                                                             usePdoCharacteristics);
        if (!NT_SUCCESS(status)) {

            //
            // The registry properties are critical and we will assign a problem
            // to the device if there was some failure in applying those.
            //
            PnpRequestDeviceRemoval(DeviceNode,
                                    FALSE,
                                    CM_PROB_SETPROPERTIES_FAILED,
                                    status);

            status = STATUS_PNP_RESTART_ENUMERATION;
            goto Cleanup;
        }

    } else {

        IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                     __FUNCTION__ "- Error %#08lx while building upper filters list for %wZ\n",
                     status,
                     PnpGetDeviceInstancePath(DeviceNode)));

        goto Cleanup;
    }

    deviceObject = DeviceNode->PhysicalDeviceObject;

    status = IopQueryLegacyBusInformation(deviceObject,
                                          NULL,
                                          &DeviceNode->InterfaceType,
                                          &DeviceNode->BusNumber);

    if (NT_SUCCESS(status)) {

        IopInsertLegacyBusDeviceNode(DeviceNode,
                                     DeviceNode->InterfaceType,
                                     DeviceNode->BusNumber);

    } else {

        DeviceNode->InterfaceType = InterfaceTypeUndefined;
        DeviceNode->BusNumber = 0xfffffff0;
    }

    //
    // Raise PnpRtl event for updated device stack property.
    //
    _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                 PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                 NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STACK);

    status = STATUS_SUCCESS;

    PNP_ASSERT(DeviceNode->State == DeviceNodeDriversAdded);

Cleanup:

    {
        UCHAR i;

        //
        // Free the entries in the driver load list & release the references on
        // their driver objects.
        //

        for (i = 0; i < MaximumAddStage; i++) {

            PDRIVER_LIST_ENTRY listHead = queryContext.DriverLists[i];

            while (listHead != NULL) {

                PDRIVER_LIST_ENTRY tmp = listHead;

                listHead = listHead->NextEntry;

                PNP_ASSERT(tmp->DriverObject != NULL);

                //
                // Let the driver unload if it hasn't created any device
                // objects. We only do this if the paging stack is already
                // online (the same filter may be needed by more than one card).
                // IopInitializeBootDrivers will take care of cleaning up any
                // leftover drivers after boot.
                //

                if (PnPBootDriversInitialized) {
                    PnpUnloadAttachedDriver(tmp->DriverObject);
                }

                ObDereferenceObject(tmp->DriverObject);
                ExFreePool(tmp);
            }
        }
    }

    if (instanceKey != NULL) {
        ZwClose(instanceKey);
    }

    if (classKey != NULL) {
        ZwClose(classKey);
    }

    if (topOfPdoStack != NULL) {
        ObDereferenceObject(topOfPdoStack);
    }

    if (topOfLowerFilterStack != NULL) {
        ObDereferenceObject(topOfLowerFilterStack);
    }

    if (propertyBuffer != NULL) {
        ExFreePool(propertyBuffer);
    }

    return status;
}

NTSTATUS
PipCallDriverAddDeviceQueryRoutine(
    __in ULONG ValueType,
    __in_bcount(ValueLength) PWCHAR ValueData,
    __in ULONG ValueLength,
    __in PQUERY_CONTEXT Context,
    __in ULONG ServiceType
    )

/*++

Routine Description:

    This routine is called to build a list of driver objects which need to
    be Added to a physical device object.  Each time it is called with a
    service name it will locate a driver object for that device and append
    it to the proper driver list for the device node.

    In the event a driver object cannot be located or that it cannot be loaded
    at this time, this routine will return an error and will set the flags
    in the device node in the context appropriately.

Arguments:

    ValueName - the name of the value

    ValueType - the type of the value

    ValueData - the data in the value (unicode string data)

    ValueLength - the number of bytes in the value data

    Context - a structure which contains the device node, the context passed
              to PipCallDriverAddDevice and the driver lists for the device
              node.

    EntryContext - the index of the driver list the routine should append
                   nodes to.

Return Value:

    STATUS_SUCCESS if the driver was located and added to the list
    successfully or if there was a non-fatal error while handling the
    driver.

    an error value indicating why the driver could not be added to the list.

--*/

{
    UNICODE_STRING unicodeServiceName;
    UNICODE_STRING unicodeDriverName;
    ULONG i;
    ULONG loadType;
    PWSTR prefixString = L"\\Driver\\";
    BOOLEAN madeupService;
    USHORT groupIndex;
    PDRIVER_OBJECT driverObject = NULL;
    NTSTATUS status = STATUS_SUCCESS;
    NTSTATUS driverEntryStatus = STATUS_SUCCESS;
    NTSTATUS driverLoadStatus = STATUS_SUCCESS;
    BOOLEAN freeDriverName = FALSE;
    HANDLE serviceKey;
#if DBG
    PDRIVER_OBJECT tempDrvObj;
#endif

    PAGED_CODE();

    //
    // Preinit
    //
    serviceKey = NULL;

    //
    // First check and make sure that the value type is okay.  An invalid type
    // is not a fatal error.
    //
    if (ValueType != REG_SZ) {

        IopDbgPrint((IOP_LOADUNLOAD_ERROR_LEVEL,
                     __FUNCTION__ "-\t\tValueType %d invalid for ServiceType %d\n",
                     ValueType,
                     ServiceType));

        return STATUS_SUCCESS;
    }

    //
    // Make sure the string is a reasonable length.
    //
    if (ValueLength <= sizeof(WCHAR)) {

        //
        // Only output an error statement if the length is shorter than the
        // length of an empty multi-sz string
        //
        if (ValueLength < sizeof(WCHAR)) {
            IopDbgPrint((IOP_LOADUNLOAD_ERROR_LEVEL,
                         __FUNCTION__ "-\t\tValueLength %d is too short\n",
                         ValueLength));

            //
            // This registry value should represent a unicode string
            // so this condition should never be met.  Assert here so
            // if there is something corrupting the registry, it can
            // be caught early.
            //
            PNP_ASSERT(ValueLength >= sizeof(WCHAR));
        }

        return STATUS_SUCCESS;
    }

    unicodeServiceName.Length = 0;
    RtlInitUnicodeString(&unicodeServiceName, ValueData);

    IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                 __FUNCTION__ "-\t\t\tService Name %wZ\n",
                 &unicodeServiceName));

    //
    // Check the service name to see if it should be used directly to reference
    // the driver object.  If the string begins with "\Driver", make sure the
    // madeupService flag is set.
    //
    madeupService = TRUE;
    i = 0;

    while (*prefixString != UNICODE_NULL) {

        if (unicodeServiceName.Buffer[i] != *prefixString) {

            madeupService = FALSE;
            break;
        }

        i++;
        prefixString++;
    }

    //
    // Get the driver name from the service key. We need this to figure out
    // if the driver is already in memory.
    //

    unicodeDriverName.Length = 0;
    if (madeupService) {

        RtlInitUnicodeString(&unicodeDriverName, unicodeServiceName.Buffer);

    } else {

        //
        //

        if (Context->DeviceNode->ServiceName.Length == 0) {
            Context->DeviceNode->ServiceName = unicodeServiceName;
            Context->DeviceNode->ServiceName.Buffer = ExAllocatePoolWithTag(
                                                        PagedPool,
                                                        unicodeServiceName.MaximumLength,
                                                        PNP_POOLTAG_SERVICE_NAME);

            if (Context->DeviceNode->ServiceName.Buffer != NULL) {
                RtlCopyMemory( Context->DeviceNode->ServiceName.Buffer,
                               unicodeServiceName.Buffer,
                               unicodeServiceName.MaximumLength );
            } else {
                PiWstrToUnicodeString( &Context->DeviceNode->ServiceName, NULL );

                IopDbgPrint((IOP_LOADUNLOAD_ERROR_LEVEL,
                             __FUNCTION__ "-\t\t\tCannot allocate memory for service name in devnode\n"));

                status = STATUS_UNSUCCESSFUL;

                goto Cleanup;
            }
        }

        //
        // Check in the registry to find the name of the driver object
        // for this device.
        //
        status = PipOpenServiceEnumKeys(&unicodeServiceName,
                                        (KEY_READ | KEY_QUERY_VALUE),
                                        &serviceKey,
                                        NULL,
                                        FALSE);

        if (!NT_SUCCESS(status)) {

            //
            // Cannot open the service key for this driver.  This is a
            // fatal error.
            //

            IopDbgPrint((IOP_LOADUNLOAD_ERROR_LEVEL,
                         __FUNCTION__ "\t\t\tError %#08lx opening service key %wZ\n",
                         status,
                         &unicodeServiceName));

            PNP_SAVE_FAILURE_INFO(Context->DeviceNode, status);
            PipSetDevNodeProblem(Context->DeviceNode, CM_PROB_REGISTRY, status);

            goto Cleanup;
        }

        status = IopGetDriverNameFromKeyNode(serviceKey, &unicodeDriverName);

        if (!NT_SUCCESS(status)) {

            //
            // Can't get the driver name from the service key.  This is a
            // fatal error.
            //

            IopDbgPrint((IOP_LOADUNLOAD_ERROR_LEVEL,
                         __FUNCTION__ "-\t\t\tError %#08lx getting driver name\n",
                         status));

            PNP_SAVE_FAILURE_INFO(Context->DeviceNode, status);
            PipSetDevNodeProblem(Context->DeviceNode, CM_PROB_REGISTRY, status);
            goto Cleanup;

        } else {

            freeDriverName = TRUE;
        }

        //
        // Note that we don't close the service key here. We may need it later.
        //
    }

    IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                 __FUNCTION__ "-\t\t\tDriverName is %wZ\n",
                 &unicodeDriverName));

    driverObject = IopReferenceDriverObjectByName(&unicodeDriverName);

    if (driverObject == NULL) {

        //
        // We couldn't find a driver object.  It's possible the driver isn't
        // loaded & initialized so check to see if we can try to load it
        // now.
        //
        if (madeupService) {

            //
            // The madeup service's driver doesn't seem to exist yet.
            // We will fail the request without setting a problem code so
            // we will try it again later.  (Root Enumerated devices...)
            //
            IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                         __FUNCTION__ "-\t\t\tCannot find driver "
                         "object for madeup service\n"));

            status = STATUS_UNSUCCESSFUL;

            goto Cleanup;
        }

        //
        // Get the start type. We always need this in case the service is
        // disabled. Default to SERVICE_DISABLED if the service's start type
        // is missing or corrupted.
        //

        status = PnpGetServiceStartType(unicodeServiceName.Buffer,
                                        serviceKey,
                                        &loadType);

        if (!NT_SUCCESS(status)) {
            loadType = SERVICE_DISABLED;
        }

        if (ServiceType != DeviceService && !PnPBootDriversInitialized) {

            //
            // Get the group index. We need this because PnpLoadBootFilterDriver
            // uses the group index as an index into it's internally sorted
            // list of loaded boot drivers.
            //
            groupIndex = PpInitGetGroupOrderIndex(serviceKey);

            //
            // If we are in BootDriverInitialization phase and trying to load a
            // filter driver
            //
            status = PnpLoadBootFilterDriver(serviceKey,
                                             &unicodeDriverName,
                                             groupIndex,
                                             &driverObject);

            if (NT_SUCCESS(status)) {

                PNP_ASSERT(driverObject);
#if DBG
                tempDrvObj = IopReferenceDriverObjectByName(&unicodeDriverName);
                PNP_ASSERT(tempDrvObj == driverObject);
#else
                if(driverObject) ObReferenceObject(driverObject);
#endif
            } else if (status != STATUS_DRIVER_BLOCKED &&
                       status != STATUS_DRIVER_BLOCKED_CRITICAL) {

                //
                // If an attempt to load a non-boot start filter driver was
                // made then it most certainly will have failed and will fail
                // on every load until the disk is up an running.  Avoid the
                // unnecessary registry queries until it is ok to load drivers
                // from disk.  Note that some drivers may have been promoted
                // to boot start status so make sure to deal with such drivers
                // here as well.
                //

                if ((loadType != SERVICE_BOOT_START) &&
                    (Context->AddContext->LoadDriver == FALSE) &&
                    (PnpCheckPossibleBootStartDriver(serviceKey) == FALSE)) {

                    Context->DeviceNode->RequiresUnloadedDriver = TRUE;
                }

                goto Cleanup;
            }

        } else {

            if (loadType > Context->AddContext->DriverStartType) {

                if (loadType == SERVICE_DISABLED &&
                    !PipDoesDevNodeHaveProblem(Context->DeviceNode)) {
                    PipSetDevNodeProblem(Context->DeviceNode, CM_PROB_DISABLED_SERVICE, STATUS_SUCCESS);
                }

                //
                // The service is either disabled or we are not at the right
                // time to load it.  Don't load it, but make sure we can get
                // called again.  If a service is marked as demand start, we
                // always load it.
                //

                IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                             __FUNCTION__ "-\t\t\tService is disabled or not at right time to load it\n"));
                status = STATUS_UNSUCCESSFUL;
                goto Cleanup;
            }

            if (!Context->AddContext->LoadDriver) {

                //
                // We're not supposed to try and load a driver - most likely our
                // disk drivers aren't initialized yet.  We need to stop the add
                // process but we can't mark the devnode as failed or we won't
                // be called again when we can load the drivers.
                //

                IopDbgPrint((IOP_LOADUNLOAD_VERBOSE_LEVEL,
                             __FUNCTION__ "-\t\t\tNot allowed to load "
                             "drivers yet\n"));

                //
                // A non boot-start driver was not permitted to load since
                // the disk stack is not yet initialized.  Take note of this
                // and ignore all future queries for drivers for this stack
                // until the disk stack is up and running.  Note that some
                // drivers may have been promoted to boot start status so make
                // sure to deal with such drivers here as well.
                //

                if ((loadType != SERVICE_BOOT_START) &&
                    (PnpCheckPossibleBootStartDriver(serviceKey) == FALSE)) {

                    Context->DeviceNode->RequiresUnloadedDriver =
                        TRUE;
                }

                status = STATUS_UNSUCCESSFUL;
                goto Cleanup;
            }

            IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                         __FUNCTION__ "-\t\t\tLoading driver %wZ\n",
                         &unicodeServiceName));

            //
            // The handle we pass in here will be closed by IopLoadDriver.
            // Note that IopLoadDriver return success without actually
            // loading the driver. This happens in the safe mode boot case.
            //

            status = IopLoadDriver(serviceKey,
                                   FALSE,
                                   (ServiceType != DeviceService)? TRUE : FALSE,
                                   &driverEntryStatus);

            //
            // IopLoadDriver closed the handle.  Make sure this function doesn't
            // also try to close it.
            //

            serviceKey = NULL;

            //
            // Preserve the return of IopLoadDriver for possible use later when
            // setting a problem code.
            //

            driverLoadStatus = status;

            //
            // Convert the status values into something more definite.
            //

            if (!NT_SUCCESS(status)) {
                if (status == STATUS_FAILED_DRIVER_ENTRY) {

                    //
                    // Preserve insufficient resources return by the driver
                    //

                    if (driverEntryStatus == STATUS_INSUFFICIENT_RESOURCES) {
                        status = STATUS_INSUFFICIENT_RESOURCES;
                    }

                } else if ((status != STATUS_INSUFFICIENT_RESOURCES) &&
                           (status != STATUS_PLUGPLAY_NO_DEVICE) &&
                           (status != STATUS_DRIVER_FAILED_PRIOR_UNLOAD) &&
                           (status != STATUS_DRIVER_BLOCKED) &&
                           (status != STATUS_DRIVER_BLOCKED_CRITICAL) &&
                           (status != STATUS_NOT_SAFE_MODE_DRIVER) &&
                           (status != STATUS_INVALID_IMAGE_HASH)) {

                    //
                    // Assume this happened because the driver could not be
                    // loaded.
                    //

                    status = STATUS_DRIVER_UNABLE_TO_LOAD;
                }
            }
            if (PnPInitialized) {
                IopCallDriverReinitializationRoutines(FALSE);
            }

            //
            // Try and get a pointer to the driver object for the service.
            //

            driverObject = IopReferenceDriverObjectByName(&unicodeDriverName);
            if (driverObject == NULL) {

                PNP_ASSERT(status != STATUS_SUCCESS);

                if (status == STATUS_NOT_SAFE_MODE_DRIVER) {

                    //
                    // Driver was not loaded because of safe mode.
                    //

                    PNP_ASSERT(InitSafeBootMode != FALSE);
                }

                if (PnpBootMode) {
                    if ((status != STATUS_FAILED_DRIVER_ENTRY) &&
                        (status != STATUS_NOT_SAFE_MODE_DRIVER) &&
                        (status != STATUS_DRIVER_BLOCKED_CRITICAL) &&
                        (status != STATUS_DRIVER_BLOCKED) &&
                        (status != STATUS_INVALID_IMAGE_HASH)) {

                        //
                        // We will retry these devices once UserMode PNPMGR
                        // starts.
                        //

                        status = STATUS_DRIVER_UNABLE_TO_LOAD;
                    }
                }
            }
        }
    }

    //
    // If we still dont have a driver object, then something failed.
    //

    if (driverObject == NULL) {

        //
        // Apparently the load didn't work out very well.
        //

        PNP_ASSERT(!NT_SUCCESS(status));

        IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                     __FUNCTION__ "-\t\t\tUnable to reference "
                     "driver %wZ (%x)\n",
                     &unicodeDriverName,
                     status));

        //
        // It may be useful if the user/admin can also see the driver
        // name if it failed to load.  The following trace will
        // send this information to the system event log.
        //

        PnpDiagnosticTraceDeviceOperation(
            &KMPnPEvt_DriverLoad_Fail,
            PnpGetDeviceInstancePath(Context->DeviceNode),
            status,
            &unicodeDriverName,
            0);

        if (!PipDoesDevNodeHaveProblem(Context->DeviceNode)) {
            switch(status) {

                case STATUS_ILL_FORMED_SERVICE_ENTRY:
                    PipSetDevNodeProblem(Context->DeviceNode, CM_PROB_DRIVER_SERVICE_KEY_INVALID, status);
                    break;

                case STATUS_INSUFFICIENT_RESOURCES:
                    PipSetDevNodeProblem(Context->DeviceNode, CM_PROB_OUT_OF_MEMORY, status);
                    break;

                case STATUS_PLUGPLAY_NO_DEVICE:
                    PipSetDevNodeProblem(Context->DeviceNode, CM_PROB_LEGACY_SERVICE_NO_DEVICES, status);
                    break;

                case STATUS_DRIVER_FAILED_PRIOR_UNLOAD:
                    PipSetDevNodeProblem(Context->DeviceNode, CM_PROB_DRIVER_FAILED_PRIOR_UNLOAD, status);
                    break;

                case STATUS_IMAGE_ALREADY_LOADED_AS_DLL:
                case STATUS_DRIVER_UNABLE_TO_LOAD:
                    PipSetDevNodeProblem(Context->DeviceNode, CM_PROB_DRIVER_FAILED_LOAD, driverLoadStatus);
                    break;

                case STATUS_FAILED_DRIVER_ENTRY:
                    PipSetDevNodeProblem(Context->DeviceNode, CM_PROB_FAILED_DRIVER_ENTRY, driverEntryStatus);
                    break;

                case STATUS_DRIVER_BLOCKED_CRITICAL:
                    PipSetDevNodeProblem(Context->DeviceNode, CM_PROB_DRIVER_BLOCKED, status);
                    PipSetDevNodeFlags(Context->DeviceNode, DNF_DRIVER_BLOCKED);
                    break;

                case STATUS_DRIVER_BLOCKED:
                    PipSetDevNodeFlags(Context->DeviceNode, DNF_DRIVER_BLOCKED);
                    status = STATUS_SUCCESS;
                    break;

                case STATUS_INVALID_IMAGE_HASH:
                    PipSetDevNodeProblem(Context->DeviceNode, CM_PROB_UNSIGNED_DRIVER, status);
                    break;

                case STATUS_NOT_SAFE_MODE_DRIVER:

                //
                // FALL THROUGH...
                //

                default:
                    PNP_ASSERT(0);
                    PipSetDevNodeProblem(Context->DeviceNode, CM_PROB_FAILED_ADD, status);
                    break;
            }

            PNP_SAVE_FAILURE_INFO(Context->DeviceNode, status);

        } else {

            //
            // This occurs due to a race condition between UserMode PNP, and
            // Kernel PNP.
            // UserMode PNP modifies configflags for a device without any
            // synchronization with KernelMode PNP.
            // For example, a device may get marked CM_PROB_DISABLED when we
            // attempt to load the driver as the configflags get checked in the
            // context driver load.
            //
        }

        goto Cleanup;
    }

    IopDbgPrint((IOP_LOADUNLOAD_VERBOSE_LEVEL,
                 __FUNCTION__ "-\t\t\tDriver object found for %wZ\n",
                 &driverObject->DriverName));

    if (!(driverObject->Flags & DRVO_INITIALIZED)) {
        status = STATUS_UNSUCCESSFUL;
        goto Cleanup;
    }

    //
    // Check to see if the driver is a legacy driver rather than a Pnp one.
    //

    if (PnpIsLegacyDriver(driverObject)) {

        //
        // It is.  Since the legacy driver may have already obtained a
        // handle to the device object, we need to assume this device
        // has been added and started.
        //

        IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                     __FUNCTION__ "-\t\t\t%wZ is a legacy driver\n",
                     &driverObject->DriverName));

        if (ServiceType == DeviceService) {
            PipSetDevNodeFlags(Context->DeviceNode, DNF_LEGACY_DRIVER);

            PipSetDevNodeState(Context->DeviceNode, DeviceNodeStarted, NULL);

            status = STATUS_UNSUCCESSFUL;
        } else {

            //
            // We allow someone to plug in a legacy driver as a filter driver.
            // In this case, the legacy driver will be loaded but will not be part
            // of our pnp driver stack.
            //

            status = STATUS_SUCCESS;
        }
        goto Cleanup;
    }

    //
    // There's a chance the driver detected this PDO during it's driver entry
    // routine.  If it did then just bail out.
    //

    if (Context->DeviceNode->State != DeviceNodeInitialized &&
        Context->DeviceNode->State != DeviceNodeDriversAdded) {

        IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                     __FUNCTION__ "-\t\t\tDevNode was reported "
                     "as detected during driver entry\n"));
        status = STATUS_UNSUCCESSFUL;
        goto Cleanup;
    }

    //
    // Add the driver to the list.
    //

    {
        PDRIVER_LIST_ENTRY listEntry;
        PDRIVER_LIST_ENTRY *runner = &(Context->DriverLists[ServiceType]);

        status = STATUS_SUCCESS;

        //
        // Allocate a new list entry to queue this driver object for the caller
        //

        listEntry = ExAllocatePool(PagedPool, sizeof(DRIVER_LIST_ENTRY));

        if (listEntry == NULL) {

            IopDbgPrint((IOP_LOADUNLOAD_WARNING_LEVEL,
                         __FUNCTION__ "-\t\t\tUnable to allocate list entry\n"));

            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        listEntry->DriverObject = driverObject;
        driverObject = NULL;
        listEntry->NextEntry = NULL;

        while (*runner != NULL) {
            runner = &((*runner)->NextEntry);
        }

        *runner = listEntry;
    }

Cleanup:

    if (serviceKey) {
        ZwClose(serviceKey);
    }

    if (freeDriverName) {
        RtlFreeUnicodeString(&unicodeDriverName);
    }

    if (driverObject != NULL) {
        ObDereferenceObject(driverObject);
    }

    return status;
}


NTSTATUS
PiRestartDevice(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    )
{
    ADD_CONTEXT addContext;
    NTSTATUS status;
    PDEVICE_NODE deviceNode;

    PAGED_CODE();

    PNP_ASSERT(Request->DeviceObject != NULL);
    deviceNode = (PDEVICE_NODE)Request->DeviceObject->DeviceObjectExtension->DeviceNode;
    if (PipIsDevNodeDeleted(deviceNode)) {

        return STATUS_DELETE_PENDING;

    } else if (PipDoesDevNodeHaveProblem(deviceNode)) {

        return STATUS_UNSUCCESSFUL;
    }

    switch(deviceNode->State) {

        case DeviceNodeStartPending:
            //
            // Not wired up today, but if the device is starting then we should
            // in theory defer completing this request until the IRP is
            // completed.
            //
            PNP_ASSERT(0);

            //
            // Fall through
            //

        case DeviceNodeStarted:
        case DeviceNodeQueryStopped:
        case DeviceNodeStopped:
        case DeviceNodeRestartCompletion:
        case DeviceNodeEnumeratePending:
            return STATUS_SUCCESS;

        case DeviceNodeInitialized:


        case DeviceNodeRemoved:
            PNP_ASSERT(!(deviceNode->UserFlags & DNUF_WILL_BE_REMOVED));
            PnpRestartDeviceNode(deviceNode);
            break;

        case DeviceNodeUninitialized:
        case DeviceNodeDriversAdded:
        case DeviceNodeResourcesAssigned:
        case DeviceNodeEnumerateCompletion:
        case DeviceNodeStartCompletion:
        case DeviceNodeStartPostWork:


            break;

        case DeviceNodeAwaitingQueuedDeletion:
        case DeviceNodeAwaitingQueuedRemoval:
        case DeviceNodeQueryRemoved:
        case DeviceNodeRemovePendingCloses:
        case DeviceNodeDeletePendingCloses:
            return STATUS_UNSUCCESSFUL;

        case DeviceNodeDeleted:
        case DeviceNodeUnspecified:
        default:
            PNP_ASSERT(0);
            return STATUS_UNSUCCESSFUL;
    }

    if (Request->RequestType == StartDevice) {

        addContext.DriverStartType = SERVICE_DEMAND_START;
        addContext.LoadDriver = PnPBootDriversInitialized;

        ObReferenceObject(deviceNode->PhysicalDeviceObject);

        status = PipProcessDevNodeTree(deviceNode,
                                       Request,
                                       &addContext,
                                       EnumTypeNone,
                                       FALSE,          // ReallocateResources
                                       FALSE,          // Synchronous
                                       FALSE);         // ProcessOnlyIntermediateStates
    }

    return STATUS_SUCCESS;
}


NTSTATUS
PipMakeGloballyUniqueId(
    __in        PDEVICE_OBJECT ParentDeviceObject,
    __in_opt    PWSTR          UniqueId,
    __deref_out PWSTR          *GloballyUniqueId
    )
{
    PWSTR end;
    HANDLE enumHandle;
    ULONG Hash;
    ULONG hashInstance;
    PWSTR id;
    HANDLE instanceKey;
    UCHAR keyBuffer[FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data) + sizeof(ULONG)];
    PKEY_VALUE_PARTIAL_INFORMATION keyValue;
    ULONG length;
    PDEVICE_NODE parentNode;
    PWSTR Prefix;
    ULONG prefixSize;
    NTSTATUS status;
    PKEY_VALUE_PARTIAL_INFORMATION stringValueBuffer;
    ULONG uniqueIdValue;
    UNICODE_STRING valueName;

    PAGED_CODE();

    id = NULL;
    Prefix = NULL;
    stringValueBuffer = NULL;

    //
    // We need to build an instance id to uniquely identify this
    // device.  We will accomplish this by producing a prefix that will be
    // prepended to the non-unique device id supplied.
    //

    //
    // To 'unique-ify' the child's instance ID, we will retrieve
    // the unique "UniqueParentID" number that has been assigned
    // to the parent and use it to construct a prefix.  This is
    // the legacy mechanism supported here so that existing device
    // settings are not lost on upgrade.
    //

    PiLockPnpRegistry(FALSE);

    parentNode = PP_DO_TO_DN(ParentDeviceObject);

    //
    // Open the instance key for this devnode.
    //

    status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                 PnpGetDeviceInstancePath(parentNode)->Buffer,
                                 CM_KEY_TYPE_DEVICE_INSTANCE,
                                 0,
                                 KEY_READ | KEY_WRITE,
                                 FALSE,
                                 &instanceKey,
                                 NULL);

    if (!NT_SUCCESS(status)) {

        IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                     "PipMakeGloballyUniqueId:\tUnable to open registry key for %wZ (status %08lx)\n",
                     PnpGetDeviceInstancePath(parentNode),
                     status));
        goto clean0;
    }

    //
    // Attempt to retrieve the "UniqueParentID" value from the device
    // instance key.
    //

    keyValue = (PKEY_VALUE_PARTIAL_INFORMATION)keyBuffer;
    PiWstrToUnicodeString(&valueName, REGSTR_VALUE_UNIQUE_PARENT_ID);

    status = ZwQueryValueKey(instanceKey,
                             &valueName,
                             KeyValuePartialInformation,
                             keyValue,
                             sizeof(keyBuffer),
                             &length
                             );

    if (NT_SUCCESS(status)) {

        PNP_ASSERT(keyValue->Type == REG_DWORD);
        PNP_ASSERT(keyValue->DataLength == sizeof(ULONG));

        if ((keyValue->Type != REG_DWORD) ||
            (keyValue->DataLength != sizeof(ULONG))) {

            status = STATUS_INVALID_PARAMETER;
            goto clean2;
        }

        uniqueIdValue = *(PULONG)(keyValue->Data);

        //
        // OK, we have a unique parent ID number to prefix to the
        // instance ID.
        //

        prefixSize = 9 * sizeof(WCHAR);
        Prefix = (PWSTR)ExAllocatePool(PagedPool, prefixSize);
        if (!Prefix) {

            status = STATUS_INSUFFICIENT_RESOURCES;
            goto clean2;
        }
        RtlStringCbPrintfW(Prefix, prefixSize, L"%x", uniqueIdValue);

    } else {
        //
        // This is the current mechanism for finding existing
        // device instance prefixes and calculating new ones if
        // required.
        //
        //
        // Attempt to retrieve the "ParentIdPrefix" value from the device
        // instance key.
        //

        PiWstrToUnicodeString(&valueName, REGSTR_VALUE_PARENT_ID_PREFIX);
        length = (MAX_PARENT_PREFIX + 1) * sizeof(WCHAR) +
            FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data);
        stringValueBuffer = ExAllocatePool(PagedPool, length);
        if (stringValueBuffer) {

            status = ZwQueryValueKey(instanceKey,
                                     &valueName,
                                     KeyValuePartialInformation,
                                     stringValueBuffer,
                                     length,
                                     &length);
        } else {

            status = STATUS_INSUFFICIENT_RESOURCES;
            goto clean2;
        }

        if (NT_SUCCESS(status)) {

            PNP_ASSERT(stringValueBuffer->Type == REG_SZ);

            if (stringValueBuffer->Type != REG_SZ) {

                status = STATUS_INVALID_PARAMETER;
                goto clean2;
            }

            //
            // Parent has already been assigned a "ParentIdPrefix".
            //

            prefixSize = stringValueBuffer->DataLength;
            Prefix = (PWSTR) ExAllocatePool(PagedPool, prefixSize);
            if (!Prefix) {

                status = STATUS_INSUFFICIENT_RESOURCES;
                goto clean2;
            }
            RtlStringCbCopyW(Prefix,
                             prefixSize,
                             (PWSTR)stringValueBuffer->Data);

        } else {
            //
            // Parent has not been assigned a "ParentIdPrefix".
            // Compute the prefix:
            //    * Compute Hash
            //    * Look for value of the form:
            //        NextParentId.<level>.<hash>:REG_DWORD: <NextInstance>
            //      under CCS\Enum.  If not present, create it.
            //    * Assign the new "ParentIdPrefix" which will be of
            //      of the form:
            //        <level>&<hash>&<instance>
            //

            //
            // Allocate a buffer once for the NextParentId... value
            // and for the prefix.
            //

            length = (ULONG)CSTRLEN(REGSTR_VALUE_NEXT_PARENT_ID) + 2 + 8 + 8 + 1;
            if (length < MAX_PARENT_PREFIX + 1) {

                length = MAX_PARENT_PREFIX + 1;
            }

            //
            // Device instances are case in-sensitive.  Upcase before
            // performing hash to ensure that the hash is case-insensitve.
            //

            status = RtlUpcaseUnicodeString(&valueName,
                                            PnpGetDeviceInstancePath(parentNode),
                                            TRUE);
            if (!NT_SUCCESS(status)) {

                goto clean2;
            }
            HASH_UNICODE_STRING(&valueName, &Hash);
            RtlFreeUnicodeString(&valueName);

            prefixSize = length * sizeof(WCHAR);
            Prefix = (PWSTR) ExAllocatePool(PagedPool, prefixSize);
            if (!Prefix) {

                status = STATUS_INSUFFICIENT_RESOURCES;
                goto clean2;
            }

            //
            // Check for existence of "NextParentId...." value and update.
            //

            RtlStringCbPrintfW(Prefix,
                               prefixSize,
                               L"%s.%x.%x",
                               REGSTR_VALUE_NEXT_PARENT_ID,
                               Hash,
                               parentNode->Level);

            status = _PnpCtxGetCachedContextBaseKey(PiPnpRtlCtx,
                                                    PNPCTX_BASE_KEY_TYPE_ENUM,
                                                    &enumHandle);

            if (!NT_SUCCESS(status)) {

                goto clean2;
            }

            RtlInitUnicodeString(&valueName, Prefix);
            keyValue = (PKEY_VALUE_PARTIAL_INFORMATION)keyBuffer;
            status = ZwQueryValueKey(enumHandle,
                                     &valueName,
                                     KeyValuePartialInformation,
                                     keyValue,
                                     sizeof(keyBuffer),
                                     &length
                                     );
            if ((NT_SUCCESS(status)) &&
                (keyValue->Type == REG_DWORD) &&
                (keyValue->DataLength == sizeof(ULONG))) {

                hashInstance = *(PULONG)(keyValue->Data);
            } else {

                hashInstance = 0;
            }

            hashInstance++;

            status = ZwSetValueKey(enumHandle,
                                   &valueName,
                                   TITLE_INDEX_VALUE,
                                   REG_DWORD,
                                   &hashInstance,
                                   sizeof(hashInstance)
                                   );
            if (!NT_SUCCESS(status)) {

                goto clean2;
            }

            hashInstance--;

            //
            // Create actual ParentIdPrefix string.
            //

            PiWstrToUnicodeString(&valueName, REGSTR_VALUE_PARENT_ID_PREFIX);
            end = Prefix;
            RtlStringCchPrintfExW(Prefix,
                                  prefixSize / sizeof(WCHAR),
                                  &end,
                                  NULL,
                                  0,
                                  L"%x&%x&%x",
                                  parentNode->Level,
                                  Hash,
                                  hashInstance);

            length = (ULONG)(end - Prefix) + 1;
            status = ZwSetValueKey(instanceKey,
                                   &valueName,
                                   TITLE_INDEX_VALUE,
                                   REG_SZ,
                                   Prefix,
                                   length * sizeof(WCHAR)
                                   );
            if (!NT_SUCCESS(status)) {

                goto clean2;
            }
        }
    }

    //
    // Construct the instance id from the non-unique id (if any)
    // provided by the child and the prefix we've constructed.
    //
    // length = Prefix + '&' + UniqueId + '\0'
    //

    length = (ULONG)(wcslen(Prefix) + 1 + (UniqueId ? wcslen(UniqueId) : 0) + 1);
    id = (PWSTR)ExAllocatePool(PagedPool, length * sizeof(WCHAR));
    if (!id) {

        status = STATUS_INSUFFICIENT_RESOURCES;
    } else if (UniqueId) {
        RtlStringCchPrintfW(id, length, L"%s&%s", Prefix, UniqueId);

    } else {
        RtlStringCchCopyW(id, length, Prefix);

    }

clean2:

    //
    // DO NOT close enumHandle.  It is a cached handle and MUST NOT be closed.
    //

    ZwClose(instanceKey);

clean0:

    PiUnlockPnpRegistry();

    if (stringValueBuffer) {

        ExFreePool(stringValueBuffer);
    }

    if (Prefix) {

        ExFreePool(Prefix);
    }

    *GloballyUniqueId = id;

    return status;
}


BOOLEAN
PipGetRegistryDwordWithFallback(
    __in    CM_REG_PROP RegistryProperty,
    __in    PCWSTR DeviceInstanceId,
    __in    HANDLE DeviceInstanceKey,
    __in    PCWSTR ClassGuid,
    __in    HANDLE InstallerClassKey,
    __inout PULONG Value
    )
/*++

Routine Description:

    If
        (1) If the device has a "RegistryProperty" property that is REG_DWORD, return it
    Else If
        (2) If the installer class has a "RegistryProperty" property that is REG_DWORD, return it
    Else
        (3) Leave Value untouched and return error

Arguments:

    RegistryProperty   - The property to query
    DeviceInstanceId   - The device instance path of the device
    DeviceInstanceKey  - A handle to the device instance key for this device
    ClassGuid          - The installer class guid
    InstallerClassKey  - If non-null, check for this property under the class
    Value              - __in = default value, __out = actual value

Return Value:

    TRUE if value found

--*/
{
    ULONG propertySize;
    ULONG propertyType = 0;
    BOOLEAN set;
    NTSTATUS status;
    ULONG tempValue = 0;

    PAGED_CODE();

    PNP_ASSERT(DeviceInstanceId != NULL);
    PNP_ASSERT(DeviceInstanceKey != NULL);

    set = FALSE;

    propertySize = sizeof(tempValue);
    status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                 DeviceInstanceId,
                                 DeviceInstanceKey,
                                 RegistryProperty,
                                 &propertyType,
                                 (PUCHAR)&tempValue,
                                 &propertySize,
                                 0);

    if ((NT_SUCCESS(status)) &&
        (propertyType == REG_DWORD) &&
        (propertySize == sizeof(tempValue))) {

        //
        // Successfully retrieved the property from the device.  Return it to the
        // caller.
        //

        *Value = tempValue;
        set = TRUE;
        goto exit;
    }

    //
    // Unable to successfully get property from device.  Try retrieving it from the class.
    //

    if (InstallerClassKey != NULL) {
        PNP_ASSERT(ClassGuid != NULL);
        PNP_ASSERT(ClassGuid[0] != UNICODE_NULL);

        propertySize = sizeof(tempValue);
        status = _CmGetInstallerClassRegProp(PiPnpRtlCtx,
                                             ClassGuid,
                                             InstallerClassKey,
                                             RegistryProperty,
                                             &propertyType,
                                             (PUCHAR)&tempValue,
                                             &propertySize,
                                             0);
        if ((NT_SUCCESS(status)) &&
            (propertyType == REG_DWORD) &&
            (propertySize == sizeof(tempValue))) {

            //
            // Successfully retrieved the property from the class.  Return it to the
            // caller.
            //

            *Value = tempValue;
            set = TRUE;
            goto exit;
        }
    }

  exit:

    return set;
}

PSECURITY_DESCRIPTOR
PipGetRegistrySecurityWithFallback(
    __in    CM_REG_PROP RegistryProperty,
    __in    PCWSTR DeviceInstanceId,
    __in    HANDLE DeviceInstanceKey,
    __in_opt PCWSTR ClassGuid,
    __in_opt HANDLE InstallerClassKey
    )
/*++

Routine Description:

    If
        (1) If the device has a "RegistryProperty" property that is
        REG_BINARY and appears to be a valid security descriptor, return it
    Else
        (2) do same for installer class
    Else
        (3) Return NULL

Arguments:

    RegistryProperty   - The property to query
    DeviceInstanceId   - The device instance path of the device
    DeviceInstanceKey  - A handle to the device instance key for this device
    ClassGuid          - The installer class guid
    InstallerClassKey  - If non-null, check for this property under the class

Return Value:

    Security Descriptor if found, else NULL

--*/
{
    PSECURITY_DESCRIPTOR allocDesc;
    ULONG propertySize;
    ULONG propertyType = 0;
    PSECURITY_DESCRIPTOR secDesc = NULL;
    BOOLEAN set;
    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(DeviceInstanceId != NULL);
    PNP_ASSERT(DeviceInstanceKey != NULL);

    allocDesc = NULL;
    secDesc = NULL;
    set = FALSE;

    //
    // Use an initial guess for how big the buffer will need to be.
    //

    propertySize = 128;
    secDesc = ExAllocatePool(PagedPool, propertySize);

    if (secDesc == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                 DeviceInstanceId,
                                 DeviceInstanceKey,
                                 RegistryProperty,
                                 &propertyType,
                                 (PUCHAR)secDesc,
                                 &propertySize,
                                 0);

    //
    // If the initial guess wasn't large enough, reallocate and retrieve
    // the property again.
    //
    if (status == STATUS_BUFFER_TOO_SMALL) {
        ExFreePool(secDesc);

        secDesc = ExAllocatePool(PagedPool, propertySize);

        if (secDesc == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto exit;
        }

        status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                     DeviceInstanceId,
                                     DeviceInstanceKey,
                                     RegistryProperty,
                                     &propertyType,
                                     (PUCHAR)secDesc,
                                     &propertySize,
                                     0);

        PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);
    }

    if ((NT_SUCCESS(status)) &&
        (propertyType == REG_BINARY)) {

        //
        // Successfully retrieved the property from the device. Validate the
        // security descriptor before capturing it
        //

        if (RtlValidRelativeSecurityDescriptor(secDesc,
                                               propertySize,
                                               0) != FALSE) {

            status = SeCaptureSecurityDescriptor(secDesc,
                                                 KernelMode,
                                                 PagedPool,
                                                 TRUE,
                                                 &allocDesc);

            if (NT_SUCCESS(status)) {
                set = TRUE;
                goto exit;
            }
        }
    }

    //
    // Unable to successfully get property from device.  Try retrieving it from the class.
    //

    if (InstallerClassKey != NULL) {
        PNP_ASSERT(ClassGuid != NULL);
        PNP_ASSERT(ClassGuid[0] != UNICODE_NULL);

        status = _CmGetInstallerClassRegProp(PiPnpRtlCtx,
                                             ClassGuid,
                                             InstallerClassKey,
                                             RegistryProperty,
                                             &propertyType,
                                             (PUCHAR)secDesc,
                                             &propertySize,
                                             0);
        //
        // If the initial guess wasn't large enough, reallocate and retrieve
        // the property again.
        //
        if (status == STATUS_BUFFER_TOO_SMALL) {
            ExFreePool(secDesc);

            secDesc = ExAllocatePool(PagedPool, propertySize);

            if (secDesc == NULL) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                goto exit;
            }

            status = _CmGetInstallerClassRegProp(PiPnpRtlCtx,
                                                 ClassGuid,
                                                 InstallerClassKey,
                                                 RegistryProperty,
                                                 &propertyType,
                                                 (PUCHAR)secDesc,
                                                 &propertySize,
                                                 0);

            PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);
        }

        if ((NT_SUCCESS(status)) &&
            (propertyType == REG_BINARY)) {

            //
            // Successfully retrieved the property from the class. Validate the
            // security descriptor before capturing it
            //

            if (RtlValidRelativeSecurityDescriptor(secDesc,
                                                   propertySize,
                                                   0) != FALSE) {

                status = SeCaptureSecurityDescriptor(secDesc,
                                                     KernelMode,
                                                     PagedPool,
                                                     TRUE,
                                                     &allocDesc);

                if (NT_SUCCESS(status)) {
                    set = TRUE;
                    goto exit;
                }
            }
        }
    }

  exit:

    if (secDesc != NULL) {
        ExFreePool(secDesc);
    }

    if (set) {
        return allocDesc;
    }

    return NULL;
}

#if FAULT_INJECT_SETPROPERTIES
//
// Fault injection for invalid IDs
//
ULONG PiFailSetProperties = 0;
#endif

NTSTATUS
PipChangeDeviceObjectFromRegistryProperties(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in HANDLE DeviceInstanceKey,
    __in PCWSTR ClassGuid,
    __in HANDLE InstallerClassKey,
    __in BOOLEAN UsePdoCharacteristics
    )
/*++

Routine Description:

    This routine will obtain settings from either
    (1) DevNode settings (via DeviceInstanceKey) or
    (2) Class settings (via DeviceClassPropKey)
    applying to PDO and all attached device objects

    Properties set/ changed are:

        * DeviceType - the I/O system type for the device object
        * DeviceCharacteristics - the I/O system characteristic flags to be
                                  set for the device object
        * Exclusive - the device can only be accessed exclusively
        * Security - security for the device

    The routine will then use the DeviceType and DeviceCharacteristics specified
    to determine whether a VPB should be allocated as well as to set default
    security if none is specified in the registry.

Arguments:

    PhysicalDeviceObject - the PDO we are to configure

    DeviceClassPropKey - a handle to Control\<Class>\Properties protected key
    DeviceInstanceKey      - a handle to Enum\<Instance>  protected key

Return Value:

    status

--*/
{
    PACL acl;
    PACL allocatedAcl;
    PACL allocatedSacl;
    UCHAR buffer[SECURITY_DESCRIPTOR_MIN_LENGTH];
    ULONG characteristics;
    BOOLEAN characteristicsSpec;
    BOOLEAN defaultSecurity;
    PDEVICE_NODE deviceNode;
    ULONG deviceType;
    BOOLEAN deviceTypeSpec;
    ULONG exclusive;
    BOOLEAN exclusiveSpec;
    BOOLEAN hasName;
    BOOLEAN present;
    BOOLEAN useClassFallback;
    ULONG prevCharacteristics;
    SECURITY_INFORMATION securityInformation;
    PSECURITY_DESCRIPTOR securityDescriptor;
    PSID sid;
    PDEVICE_OBJECT StackIterator;
    NTSTATUS status;
    BOOLEAN tmp;

    PAGED_CODE();

    //
    // Sanity check inputs.
    //

    PNP_ASSERT(PhysicalDeviceObject);
    deviceNode = PP_DO_TO_DN(PhysicalDeviceObject);
    PNP_ASSERT(deviceNode);
    IopDbgPrint((IOP_ENUMERATION_VERBOSE_LEVEL,
                 "PipChangeDeviceObjectFromRegistryProperties: Modifying device stack for PDO: %wZ\n",
                 PnpGetDeviceInstancePath(deviceNode)));

    //
    // Initialize locals so we can cleanup properly on exit.
    //

    securityDescriptor = NULL;
    defaultSecurity = FALSE;
    allocatedAcl = NULL;
    allocatedSacl = NULL;
    deviceType = 0;
    exclusive = 0;
    characteristics = 0;

    //
    // If the driver already overwrote security during AddDevice, we don't fall back to Class
    // security.
    //
    useClassFallback = ((PhysicalDeviceObject->DeviceObjectExtension->ExtensionFlags & DOE_DEFAULT_SD_PRESENT) != 0);

    //
    // Get the device type, characteristics and exclusive properties specified
    // in the registry (typically installed via an INF). DeviceInstanceKey is
    // preferred over DeviceClassPropKey.
    //

    deviceTypeSpec = PipGetRegistryDwordWithFallback(CM_REG_PROP_DEVTYPE,
                                                     PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                                     DeviceInstanceKey,
                                                     ClassGuid,
                                                     InstallerClassKey,
                                                     &deviceType);

    exclusiveSpec = PipGetRegistryDwordWithFallback(CM_REG_PROP_EXCLUSIVE,
                                                    PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                                    DeviceInstanceKey,
                                                    ClassGuid,
                                                    InstallerClassKey,
                                                    &exclusive);

    characteristicsSpec = PipGetRegistryDwordWithFallback(CM_REG_PROP_CHARACTERISTICS,
                                                          PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                                          DeviceInstanceKey,
                                                          ClassGuid,
                                                          InstallerClassKey,
                                                          &characteristics);

    //
    // Build the characteristics for the entire stack. Unless specified or RAW,
    // PDO characteristics are ignored.
    //

    if (UsePdoCharacteristics || PhysicalDeviceObject->AttachedDevice == NULL) {

        //
        // Use the PDO.
        //

        StackIterator = PhysicalDeviceObject;
        IopDbgPrint((IOP_ENUMERATION_WARNING_LEVEL,
                     "PipChangeDeviceObjectFromRegistryProperties: Assuming PDO for %wZ is being used RAW\n",
                     PnpGetDeviceInstancePath(deviceNode)));

    } else {

        //
        // Skip the PDO.
        //

        StackIterator = PhysicalDeviceObject->AttachedDevice;
        IopDbgPrint((IOP_ENUMERATION_VERBOSE_LEVEL,
                     "PipChangeDeviceObjectFromRegistryProperties: Ignoring PDO's settings for %wZ\n",
                     PnpGetDeviceInstancePath(deviceNode)));
    }

    //
    // Build the mask of device stack characteristics.
    //

    prevCharacteristics = 0;
    for (;
         StackIterator != NULL;
         StackIterator = StackIterator->AttachedDevice) {

        prevCharacteristics |= StackIterator->Characteristics;
    }

    //
    // Build the new characteristics mask.
    //

    characteristics |= prevCharacteristics;
    characteristics &= FILE_CHARACTERISTICS_PROPAGATED;

    //
    // Get the security descriptor specified in the registry (typically
    // installed via an INF). DeviceInstanceKey is preferred over DeviceClassPropKey.
    //

    securityInformation = 0;
    securityDescriptor = PipGetRegistrySecurityWithFallback(CM_REG_PROP_SECURITY,
                                                            PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                                            DeviceInstanceKey,
                                                            useClassFallback ? ClassGuid : NULL,
                                                            useClassFallback ? InstallerClassKey : NULL);

    if (securityDescriptor == NULL) {

        //
        // If the registry specifies a device type but no security descriptor,
        // we will create a default one for the specified device type.
        //

        if (deviceTypeSpec) {
            if (PhysicalDeviceObject->Flags & DO_DEVICE_HAS_NAME) {
                hasName = TRUE;

            } else {
                hasName = FALSE;
            }

            RtlZeroMemory(buffer, sizeof(buffer));
            securityDescriptor = IopCreateDefaultDeviceSecurityDescriptor(
                                    (DEVICE_TYPE)deviceType,
                                    characteristics,
                                    hasName,
                                    &buffer[0],
                                    &allocatedAcl,
                                    &allocatedSacl,
                                    &securityInformation);

            if (securityDescriptor) {
                defaultSecurity = TRUE; // forced default security descriptor

            } else {
                IopDbgPrint((IOP_ENUMERATION_WARNING_LEVEL,
                             "PipChangeDeviceObjectFromRegistryProperties: Was not able to get default security descriptor\n"));

                status = STATUS_UNSUCCESSFUL;
                goto cleanup;
            }
        }

    } else {

        //
        // See what information is in the captured descriptor so we can build
        // up a securityInformation block to go with it.
        //

        status = RtlGetOwnerSecurityDescriptor(securityDescriptor, &sid, &tmp);
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        if (sid) {
            securityInformation |= OWNER_SECURITY_INFORMATION;
        }

        status = RtlGetGroupSecurityDescriptor(securityDescriptor, &sid, &tmp);
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        if (sid) {
            securityInformation |= GROUP_SECURITY_INFORMATION;
        }

        status = RtlGetSaclSecurityDescriptor(securityDescriptor,
                                              &present,
                                              &acl,
                                              &tmp);

        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        if (present) {
            securityInformation |= SACL_SECURITY_INFORMATION;
        }

        status = RtlGetDaclSecurityDescriptor(securityDescriptor,
                                              &present,
                                              &acl,
                                              &tmp);

        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        if (present) {
            securityInformation |= DACL_SECURITY_INFORMATION;
        }
    }

#if DBG

    if (    deviceTypeSpec == FALSE &&
            characteristicsSpec == FALSE &&
            exclusiveSpec == FALSE &&
            securityDescriptor == NULL) {

        IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                    "PipChangeDeviceObjectFromRegistryProperties: No property changes for %wZ\n",
                     PnpGetDeviceInstancePath(deviceNode)));

    } else {
        if (deviceTypeSpec) {
            IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                         "PipChangeDeviceObjectFromRegistryProperties: Overide DeviceType=%08x\n",
                         deviceType));
        }

        if (characteristicsSpec) {
            IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                         "PipChangeDeviceObjectFromRegistryProperties: Overide DeviceCharacteristics=%08x\n",
                         characteristics));
        }

        if (exclusiveSpec) {
            IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                         "PipChangeDeviceObjectFromRegistryProperties: Overide Exclusive=%d\n",
                         (exclusive ? 1 : 0)));
        }

        if (defaultSecurity) {
            IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                         "PipChangeDeviceObjectFromRegistryProperties: Overide Security based on DeviceType & DeviceCharacteristics\n"));
        }

        if (securityDescriptor == NULL) {
            IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                         "PipChangeDeviceObjectFromRegistryProperties: Overide Security\n"));
        }
    }

#endif

    //
    // Modify PDO properties.
    //

    if (deviceTypeSpec) {
        PhysicalDeviceObject->DeviceType = deviceType;
    }

    if (exclusiveSpec) {
        if (exclusive) {

            //
            // Exclusivity flag applies only to the PDO.
            // If any other object in the stack is named, then this flag should
            // not be relied upon.
            //

            PhysicalDeviceObject->Flags |= DO_EXCLUSIVE;
        }
    }

    //
    // PDO may lose some characteristics (and get some new) based on what the
    // rest of the stack specify.
    //

    PhysicalDeviceObject->Characteristics &= ~FILE_CHARACTERISTICS_PROPAGATED;
    PhysicalDeviceObject->Characteristics |= characteristics;

    //
    // Apply the same characteristics to the entire stack. This will always add
    // characteristics since the mask is a superset of characteristics of each
    // object in the stack.
    //

    for (   StackIterator = PhysicalDeviceObject->AttachedDevice;
            StackIterator != NULL;
            StackIterator = StackIterator->AttachedDevice) {

        StackIterator->Characteristics |= characteristics;
    }

    //
    // Apply the security descriptor, if any, to the whole stack.
    //

    status = STATUS_SUCCESS;
    if (securityDescriptor != NULL) {
        status = ObSetSecurityObjectByPointer(PhysicalDeviceObject,
                                              securityInformation,
                                              securityDescriptor);

        if (!NT_SUCCESS(status)) {
            IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                         "PipChangeDeviceObjectFromRegistryProperties: Set security failed (%08x)\n",
                         status));
        }
    }

    //
    // Check if any driver in the stack does not support asynchronous operations.
    //

    for (StackIterator = PhysicalDeviceObject;
         StackIterator;
         StackIterator = StackIterator->AttachedDevice) {

        if (StackIterator->DriverObject->Flags & DRVO_ASYNC_PNP_UNSUPPORTED) {
            PP_DO_TO_DN(PhysicalDeviceObject)->Flags |= (DNF_ASYNC_START_NOT_SUPPORTED | DNF_ASYNC_ENUMERATION_NOT_SUPPORTED);
        }

        //
        // If we used the default security descriptor set a flag in the
        // extension of any device object that received it.
        //

        if (defaultSecurity && StackIterator->SecurityDescriptor) {
            StackIterator->DeviceObjectExtension->ExtensionFlags |= DOE_DEFAULT_SD_PRESENT;
        }
    }

cleanup:

    //
    // Cleanup.
    //

    if ((securityDescriptor != NULL) && !defaultSecurity) {
        ExFreePool(securityDescriptor);
    }

    if (allocatedAcl) {
        ExFreePool(allocatedAcl);
    }

    if (allocatedSacl) {
        ExFreePool(allocatedSacl);
    }

#if FAULT_INJECT_SETPROPERTIES

    if (NT_SUCCESS(status)) {
        static LARGE_INTEGER seed = {0};
        if(seed.LowPart == 0) {
            KeQuerySystemTime(&seed);
        }

        if(PnPBootDriversInitialized &&
           PiFailSetProperties &&
           RtlRandom(&seed.LowPart) % 10 > 5) {

            status = STATUS_UNSUCCESSFUL;
        }
    }

#endif

    return status;
}


NTSTATUS
PipProcessDevNodeTree(
    IN  PDEVICE_NODE        SubtreeRootDeviceNode,
    IN  PPNP_DEVICE_ACTION_ENTRY  Request,
    IN  PADD_CONTEXT        AddContext,
    IN  ENUM_TYPE           EnumType,
    IN  LOGICAL             ReallocateResources,
    IN  LOGICAL             Synchronous,
    IN  LOGICAL             ProcessOnlyIntermediateStates
    )
/*--

Routine Description:

    This function is called to handle state transitions related to starting
    Devnodes.  The basic sequence of operations is inheritted from the previous
    implementation.

    Resources freed
        1)  Allocate resources to all candidates in the tree.
        2)  Traverse the tree searching for a Devnodes ready to be started.
        3)  Start the Devnode.
        4)  Enumerate its children.
        5)  Initialize all the children up to the point of resource allocation.
        6)  Continue searching for DevNodes to start, if one is found return to
            step 3.
        7)  Once the entire tree is processed start over at step 1 until either
            no children are enumerated or no resources are allocated.

    A Devnode's resource requirements change
        If the Devnode wasn't started then treat it the same as the Resources
        freed case.  If it was started then it would have been handled directly
        by our caller.


    Start Devnodes during boot
        1)  Allocate resources to all candidates in the tree (based on
            IopBootConfigsReserved).
        2)  Traverse the tree searching for Devnodes ready to be started.
        3)  Start the Devnode.
        4)  Enumerate its children.
        5)  Initialize all the children up to the point of resource allocation.
        6)  Continue searching for DevNodes to start, if one is found return to
            step 3.

    Devnode newly created by user-mode.
        1)  Reset Devnode to uninitialized state.
        2)  Process Devnode to DeviceNodeDriversAdded state.
        3)  Allocate resources to this Devnode.
        4)  Start the Devnode.
        5)  Enumerate its children.
        6)  Initialize any children up to the point of resource allocation.
        7)  Allocate resources to all candidates in the tree below the initial
            Devnode.
        8)  Traverse the tree starting at the initial Devnode searching for
            a Devnode ready to be started.
        9)  Start the Devnode.
        10) Enumerate its children.
        11) Initialize all the children up to the point of resource allocation.
        12) Start over at step 7 until either no children are enumerated or no
            resources are allocated.

    Device node newly created by IoReportDetectedDevice.
        1)  Do post start IRP processing
        2)  Continue from step 5 of the process for Devnodes newly created by
            user-mode.

    Reenumeration of a single Devnode (and processing of changes resulting from
    that enumeration)

        1)  Enumerate Devnode's children
        2)  Initialize any children up to the point of resource allocation.
        3)  Allocate resources to all candidates in the tree below the initial
            Devnode.
        4)  Traverse the tree starting at the initial Devnode searching for
            a Devnode ready to be started.
        5)  Start the Devnode.
        6)  Enumerate its children.
        7)  Initialize all the children up to the point of resource allocation.
        8)  Start over at step 3 until either no children are enumerated or no
            resources are allocated.

    Reenumeration of a subtree.




Parameters:

    SubtreeRootDeviceNode - Root of this tree walk. Depending on the
                            ProcessOnlyIntermediaryStates parameter, the
                            PDO for this devnode may need to be referenced.

    ReallocateResources - TRUE iff resource reallocation should be attempted.

    EnumType - Specifies type of enumeration.

    Synchronous - TRUE iff the operation should be performed synchronously
                  (always TRUE currently)

    ProcessOnlyIntermediateStates - TRUE if only intermediary states should be
                                    processed. If FALSE, the caller places
                                    a reference on the PDO that this routine
                                    will drop.

    AddContext - Constraints for AddDevice

    Request - Device action worker that triggered this processing.

Return Value:

    NTSTATUS - Note: Always successful if ProcessOnlyIntermediaryStates is TRUE.

++*/
{
    NTSTATUS status, finalStatus;
    PDEVICE_NODE originalSubtree, currentNode;
    ULONG reenumAttempts;
    WCHAR buffer[MAX_INSTANCE_PATH_LENGTH];
    UNICODE_STRING unicodeString;
    PDEVICE_OBJECT subtreeRootDeviceObject;
    LOGICAL restartEnumeration, needResourcesAssigned;
    LOGICAL moreProcessingNeeded, completedOperation, operationsPending, rebalancePerformed;
    LOGICAL waitForCompletion;
    enum {
        SameNode,
        SiblingNode,
        ChildNode
    } nextNode;

    PAGED_CODE();

    //
    // Collapse enum requests in the device action queue.
    //
    originalSubtree = SubtreeRootDeviceNode;
    if (Request &&
        !Request->ReorderingBarrier &&
        EnumType != EnumTypeShallow &&
        !ProcessOnlyIntermediateStates) {

        if (PiCollapseEnumRequests(&Request->ListEntry)) {

            SubtreeRootDeviceNode = IopRootDeviceNode;
        }
    }

    finalStatus = STATUS_SUCCESS;
    reenumAttempts = 0;
    restartEnumeration = FALSE;
    operationsPending = FALSE;
    needResourcesAssigned = FALSE;

    do {

        if (!ProcessOnlyIntermediateStates) {

            if (EventEnabledKMPnPEvt_AssignResources_Start()) {
                EventWriteKMPnPEvt_AssignResources_Start(NULL,
                                                         PnpGetDeviceInstancePath(SubtreeRootDeviceNode)->Buffer);
            }

            //
            // Assign resources.
            //
            rebalancePerformed = FALSE;
            needResourcesAssigned = PnpProcessAssignResources(SubtreeRootDeviceNode,
                                                              ReallocateResources,
                                                              &rebalancePerformed);

            if (EventEnabledKMPnPEvt_AssignResources_Stop()) {
                EventWriteKMPnPEvt_AssignResources_Stop(NULL,
                                                        PnpGetDeviceInstancePath(SubtreeRootDeviceNode)->Buffer);
            }

            if (rebalancePerformed != FALSE) {

                //
                // Before we do any other processing, we need to restart
                // all rebalance participants.
                //
                status = PipProcessDevNodeTree(
                            IopRootDeviceNode,
                            Request,
                            AddContext,
                            EnumType,
                            FALSE,
                            FALSE,
                            TRUE
                            );
                if (status == STATUS_PNP_RESTART_ENUMERATION) {

                    restartEnumeration = TRUE;
                }
            }
        }
        waitForCompletion = FALSE;
        currentNode = SubtreeRootDeviceNode;
        moreProcessingNeeded = TRUE;
        do {

            nextNode    = SiblingNode;
            if (restartEnumeration) {

                //
                // Someone failed a restart after rebalance.
                //
                goto restart;
            }

            //
            // Check for start dependencies and set the problem code
            // accordingly only if the device node has been
            // initialized and no existing problem code exists.
            //

            if ((currentNode->State != DeviceNodeUninitialized) &&
                (PipDoesDevNodeHaveProblem(currentNode) == FALSE) &&
                (PnpCheckForActiveDependencies(currentNode, PNP_DEPENDENCY_TYPE_START))) {

                PipSetDevNodeProblem(currentNode,
                                     CM_PROB_WAITING_ON_DEPENDENCY,
                                     STATUS_SUCCESS);

            }

            //
            // Dont process devnodes with problems.
            //
            if (PipDoesDevNodeHaveProblem(currentNode)) {

                goto moveToNextNode;
            }

            //
            // Process the current devnode based on its current state.
            //
            status = STATUS_SUCCESS;
            switch (currentNode->State) {

            case DeviceNodeUninitialized:

                if ((EnumType == EnumTypeIgnoreUninstalledDevices) &&
                    (currentNode->Flags & DNF_UNINSTALLED)) {

                    break;
                }

                if (!ProcessOnlyIntermediateStates) {
                    
                    //
                    // Check for initialization dependencies and set the problem code.
                    //

                    if (PnpCheckForActiveDependencies(currentNode,
                                                      PNP_DEPENDENCY_TYPE_INITIALIZATION)) {

                        PipSetDevNodeProblem(currentNode,
                                             CM_PROB_WAITING_ON_DEPENDENCY,
                                             STATUS_SUCCESS);

                        break;
                    }

                    status = PiProcessNewDeviceNode(currentNode);
                    if (NT_SUCCESS(status)) {
                        nextNode = SameNode;
                    }
                }

                break;

            case DeviceNodeInitialized:

                PNP_ASSERT(currentNode->PowerFlags == 0);

                if (!ProcessOnlyIntermediateStates) {
                    status = PipCallDriverAddDevice(currentNode,
                                                    AddContext);
                    if (NT_SUCCESS(status)) {

                        //
                        // Allow the power manager the opportunity
                        // to prepare for this device to start.
                        //

                        PoFxPrepareDevice(currentNode, FALSE);
                        nextNode = SameNode;
                        needResourcesAssigned = TRUE;
                    }
                }

                break;

            case DeviceNodeResourcesAssigned:

                if (!ProcessOnlyIntermediateStates) {
                    PNP_ASSERT((currentNode->StateFlags & DNSF_POFX_START_ENUM) == 0);

                    PoFxActivateDevice(currentNode->PhysicalDeviceObject);
                    currentNode->StateFlags |= DNSF_POFX_START_ENUM;
                    status = PipProcessStartPhase1(currentNode,
                                                   Synchronous,
                                                   operationsPending);

                    if (status == STATUS_PENDING) {
                        operationsPending = TRUE;

                    } else {
                        nextNode = SameNode;
                    }
                }

                break;

            case DeviceNodeStartCompletion:

                status = PipProcessStartPhase2(currentNode);

                if (NT_SUCCESS(status)) {
                    nextNode = SameNode;

                } else {
                    status = STATUS_PNP_RESTART_ENUMERATION;
                    PNP_ASSERT(currentNode->State != DeviceNodeStartCompletion);
                }

                break;

            case DeviceNodeStartPostWork:

                if ((currentNode->Flags & DNF_WAITING_FOR_FDO) != 0) {
                    nextNode = SiblingNode;

                } else {
                    status = PipProcessStartPhase3(currentNode, EnumType != EnumTypeShallow);
                    if (NT_SUCCESS(status)) {
                        nextNode = SameNode;

                    } else {
                        status = STATUS_PNP_RESTART_ENUMERATION;

                        //
                        // N.B. 2008/11/19 (pareshm):
                        //
                        // For async start operations, if a device returns
                        // a failed state in the subsequent
                        // IRP_MN_QUERY_DEVICE_STATE after succeeding its start
                        // irp, this code path may be exercised. Such is the
                        // case for i8042prt controlled devices. This assert
                        // will hence fire unnecessarily and is commented out
                        // accordingly.
                        //
                        // PNP_ASSERT(!ProcessOnlyIntermediateStates);
                        //

                    }
                }

                break;

            case DeviceNodeStarted:

                //
                // If this is a shallow enum request, then only
                // walk the child list of the target device node.
                //

                if ((EnumType == EnumTypeShallow) &&
                    (currentNode != SubtreeRootDeviceNode)) {

                    break;
                }

                nextNode = ChildNode;
                if (!ProcessOnlyIntermediateStates) {
                    if ((currentNode->Flags & DNF_REENUMERATE)) {
                        status = PipEnumerateDevice(currentNode,
                                                    Synchronous,
                                                    operationsPending);

                        if (status == STATUS_PENDING) {
                            operationsPending = TRUE;
                            nextNode = SiblingNode;

                        } else if (NT_SUCCESS(status)) {
                            nextNode = SameNode;
                        }
                    }
                }

                break;

            case DeviceNodeEnumerateCompletion:

                if ((currentNode->StateFlags & DNSF_POFX_START_ENUM) != 0) {
                    PoFxIdleDevice(currentNode->PhysicalDeviceObject);
                    currentNode->StateFlags &= ~DNSF_POFX_START_ENUM;
                }

                status = PipEnumerateCompleted(currentNode);
                nextNode = ChildNode;
                break;

            case DeviceNodeStopped:

                status = PipProcessRestartPhase1(currentNode,
                                                 Synchronous,
                                                 operationsPending);

                if (status == STATUS_PENDING) {
                    operationsPending = TRUE;

                } else {
                    nextNode = SameNode;
                }

                break;

            case DeviceNodeRestartCompletion:

                status = PipProcessRestartPhase2(currentNode);
                if (NT_SUCCESS(status)) {
                    nextNode = SameNode;

                } else {
                    status = STATUS_PNP_RESTART_ENUMERATION;
                    PNP_ASSERT(currentNode->State != DeviceNodeRestartCompletion);
                }

                break;

            case DeviceNodeDriversAdded:
            case DeviceNodeStartPending:
            case DeviceNodeEnumeratePending:
            case DeviceNodeAwaitingQueuedDeletion:
            case DeviceNodeAwaitingQueuedRemoval:
            case DeviceNodeRemovePendingCloses:
            case DeviceNodeRemoved:
                break;

            case DeviceNodeQueryStopped:
            case DeviceNodeQueryRemoved:
            case DeviceNodeDeletePendingCloses:
            case DeviceNodeDeleted:
            case DeviceNodeUnspecified:
            default:
                PNP_ASSERT(0);
                break;
            };

            if (status != STATUS_PNP_RESTART_ENUMERATION) {

                //
                // Pull out any completed action.
                //

                completedOperation = FALSE;
                status = PnpDeviceCompletionProcessCompletedRequests(FALSE,
                                                                     FALSE,
                                                                     &completedOperation);

                if (status != STATUS_PNP_RESTART_ENUMERATION) {

                    //
                    // Wait if neccessary on all passes except first one.
                    //

                    if (!completedOperation) {
                        if (waitForCompletion) {
                            waitForCompletion = FALSE;
                            status = PnpDeviceCompletionProcessCompletedRequests(
                                        FALSE,
                                        TRUE,
                                        &completedOperation);
                            if (!completedOperation) {
                                operationsPending = FALSE;
                            }
                        }
                    }
                }
            }

            if (status == STATUS_PNP_RESTART_ENUMERATION) {

                if (!ProcessOnlyIntermediateStates) {
                    restartEnumeration = TRUE;

                } else {
                    finalStatus = status;
                }
            }

restart:
            //
            // Check if we need to restart since a removal could have gotten
            // queued which needs to be processed before we can proceed.
            //
            if (restartEnumeration) {
                restartEnumeration = FALSE;

                //
                // Wait for currently dispatched asynchronous operations to
                // complete.
                //

                PnpDeviceCompletionProcessCompletedRequests(TRUE,
                                                            TRUE,
                                                            &completedOperation);

                operationsPending = FALSE;

                //
                // Move the devnodes off their intermediate states.
                //

                PipProcessDevNodeTree(IopRootDeviceNode,
                                      Request,
                                      AddContext,
                                      EnumType,
                                      ReallocateResources,
                                      Synchronous,
                                      TRUE);

                PnpAssertDevnodesInConsistentState();
                if (reenumAttempts < MAX_REENUMERATION_ATTEMPTS) {
                    ULONG DeviceInstancePathLength =
                        PnpGetDeviceInstancePath(SubtreeRootDeviceNode)->Length / sizeof(WCHAR);

                    PNP_ASSERT(DeviceInstancePathLength < MAX_INSTANCE_PATH_LENGTH);
                    __analysis_assume(DeviceInstancePathLength < MAX_INSTANCE_PATH_LENGTH);
                    RtlCopyMemory(
                        buffer,
                        PnpGetDeviceInstancePath(SubtreeRootDeviceNode)->Buffer,
                        DeviceInstancePathLength * sizeof(WCHAR));

                    buffer[DeviceInstancePathLength] = UNICODE_NULL;
                }

                ObDereferenceObject(originalSubtree->PhysicalDeviceObject);
                PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);
                PnpSynchronizeDeviceEventQueue();
                PpDevNodeLockTree(PPL_TREEOP_ALLOW_READS);
                if (reenumAttempts >= MAX_REENUMERATION_ATTEMPTS) {
                    IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                                 "Restarted reenumeration %d times, giving up!\n",
                                 reenumAttempts));
                    PNP_ASSERT(reenumAttempts < MAX_REENUMERATION_ATTEMPTS);
                    return STATUS_UNSUCCESSFUL;
                }

                RtlInitUnicodeString(&unicodeString, buffer);
                subtreeRootDeviceObject = PnpDeviceObjectFromDeviceInstance(
                                            &unicodeString);

                if (!subtreeRootDeviceObject) {
                    return STATUS_UNSUCCESSFUL;
                }

                SubtreeRootDeviceNode = PP_DO_TO_DN(subtreeRootDeviceObject);
                originalSubtree = currentNode = SubtreeRootDeviceNode;
                continue;
            }

moveToNextNode:
            //
            // Trace progress of starting a newly configured device.
            //
            if ((!ProcessOnlyIntermediateStates) &&
                ((currentNode->StateFlags & DNSF_CONFIGURED_AWAITING_START) != 0)) {
                PpDevCfgTraceDeviceStart(currentNode);
            }

            if ((operationsPending != FALSE) &&
                (nextNode != SameNode) &&
                ((PnpAsyncOptions & PNP_ASYNC_TREE_WIDE) == 0)) {

                //
                // If there are operations pending, always move to the next
                // sibling. This keeps us on the same bus until all pending
                // operations complete.
                //

                nextNode = SiblingNode;
            }

            switch (nextNode) {

            case SameNode:

                //
                // NOP.
                //
                break;

            case ChildNode:

                if (currentNode->Child) {
                    currentNode = currentNode->Child;
                    break;
                }

                //
                // FALLTHRU - No more children so advance to sibling
                //

            case SiblingNode:

                for (;;) {

                    //
                    // We are done walking the subtree if we walked back up to
                    // where we started from.
                    //

                    if (currentNode == SubtreeRootDeviceNode) {
                        if (!operationsPending) {
                            moreProcessingNeeded = FALSE;
                        }

                        waitForCompletion = TRUE;
                        break;
                    }

                    if (currentNode->Sibling) {
                        currentNode = currentNode->Sibling;
                        break;
                    }

                    //
                    // We are at the end of the bus for the currentNode's
                    // parent.
                    //

                    if ((operationsPending != FALSE) &&
                        (PnpAsyncOptions != 0) &&
                        ((PnpAsyncOptions & PNP_ASYNC_TREE_WIDE) == 0)) {

                        //
                        // If there are pending operations, move to the start
                        // of the bus.
                        //

                        if (currentNode->Parent) {
                            currentNode = currentNode->Parent->Child;
                        }

                        waitForCompletion = TRUE;
                        break;
                    }

                    if (currentNode->Parent) {
                        currentNode = currentNode->Parent;
                    }
                }

                break;

            default:

                PNP_ASSERT(0);
                break;
            };

        } while (moreProcessingNeeded);

    } while (needResourcesAssigned);

    //
    // Sanity-check and clean-up on the way out.
    //

    if (!ProcessOnlyIntermediateStates) {
         PnpAssertDevnodesInConsistentState();
         ObDereferenceObject(originalSubtree->PhysicalDeviceObject);
    }

    return finalStatus;
}

NTSTATUS
PiProcessHaltDevice(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    )
/*++

Routine Description:

    This routine simulates a surprise removal scenario on the passed in device
    node.

Arguments:

    DeviceNode - DeviceNode to halt

    Flags - PNP_HALT_ALLOW_NONDISABLEABLE_DEVICES - Allows halt on nodes
                                                    marked non-disableable.

Return Value:

    NTSTATUS.

--*/
{
    ULONG flags = (ULONG)Request->RequestArgument;
    PDEVICE_NODE deviceNode;

    PAGED_CODE();

    PNP_ASSERT(Request->DeviceObject != NULL);
    deviceNode = (PDEVICE_NODE)Request->DeviceObject->DeviceObjectExtension->DeviceNode;
    if (PipIsDevNodeDeleted(deviceNode)) {

        return STATUS_DELETE_PENDING;
    }

    if (flags & (~PNP_HALT_ALLOW_NONDISABLEABLE_DEVICES)) {

        return STATUS_INVALID_PARAMETER_2;
    }

    if (deviceNode->Flags & (DNF_MADEUP | DNF_LEGACY_DRIVER)) {

        //
        // Sending surprise removes to legacy devnodes would be a bad idea.
        // Today, if a legacy devnode fails it is manually taken to the removed
        // state rather than being put through the engine.
        //
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    if ((!(deviceNode->Flags & PNP_HALT_ALLOW_NONDISABLEABLE_DEVICES)) &&
        deviceNode->DisableableDepends) {

        return STATUS_INVALID_DEVICE_REQUEST;
    }

    if (deviceNode->State != DeviceNodeStarted) {

        return STATUS_INVALID_DEVICE_STATE;
    }

    PnpRequestDeviceRemoval(deviceNode,
                            FALSE,
                            CM_PROB_HALTED,
                            STATUS_SUCCESS);

    return STATUS_SUCCESS;
}

VOID
PpResetProblemDevices(
    __in  PDEVICE_NODE    DeviceNode,
    __in  ULONG           Problem
    )
/*++

Routine Description:

    This routine resets all non-configured devices *beneath* the passed in
    devnode so a subsequent enum will kick off new hardware installation
    on them.

Arguments:

    DeviceNode - DeviceNode to halt

Return Value:

    None.

--*/
{
    CLEAR_PROBLEM_CONTEXT context;

    PAGED_CODE();

    context.Problem = Problem;
    context.Restart = FALSE;
    context.Limit   = 0;

    PpDevNodeLockTree(PPL_TREEOP_ALLOW_READS);

    PipForDeviceNodeSubtree(
        DeviceNode,
        PiResetProblemDevicesWorker,
        &context
        );

    PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);
}

NTSTATUS
PiResetProblemDevicesWorker(
    __in  PDEVICE_NODE            DeviceNode,
    __in  PCLEAR_PROBLEM_CONTEXT  Context
    )
/*++

Routine Description:

    This is a worker routine for PiResetNonConfiguredDevices. If the devnode
    has the problem CM_PROB_NOT_CONFIGURED, the devnode is reset so a
    subsequent reenumeration will bring it back.

Arguments:

    DeviceNode - Device to reset if it has the correct problem.

    Context - Not used.

Return Value:

    NTSTATUS, non-successful statuses terminate the tree walk.

--*/
{
    PAGED_CODE();

    if (PipIsDevNodeProblem(DeviceNode, Context->Problem)) {

        if (Context->Limit) {

            if (DeviceNode->DriverUnloadRetryCount >= Context->Limit) {

                return STATUS_SUCCESS;
            }

            DeviceNode->DriverUnloadRetryCount++;
        }

        //
        // We only need to queue it as an enumeration will drop behind it soon
        // afterwards...
        //
        PnpRequestDeviceAction(DeviceNode->PhysicalDeviceObject,
                               ClearDeviceProblem,
                               TRUE,
                               0,
                               NULL,
                               NULL);
        if (Context->Restart) {

            PnpRequestDeviceAction(DeviceNode->PhysicalDeviceObject,
                                   StartDevice,
                                   TRUE,
                                   0,
                                   NULL,
                                   NULL);
        }
    }

    return STATUS_SUCCESS;
}

VOID
PiMarkDeviceTreeForReenumeration(
    __in  PDEVICE_NODE DeviceNode,
    __in  BOOLEAN Subtree
    )
/*++

Routine Description:

    This routine marks the devnode for reenumeration.

Arguments:

    DeviceNode  - DeviceNode to mark for re-enumeration

    Subtree     - If TRUE, the entire subtree is marked for re-enumeration.

Return Value:

    None.

--*/
{
    PAGED_CODE();

    PPDEVNODE_ASSERT_LOCK_HELD(PPL_TREEOP_ALLOW_READS);

    PiMarkDeviceTreeForReenumerationWorker(DeviceNode, NULL);

    if (Subtree) {

        PipForDeviceNodeSubtree(
            DeviceNode,
            PiMarkDeviceTreeForReenumerationWorker,
            NULL
            );
    }
}

NTSTATUS
PiMarkDeviceTreeForReenumerationWorker(
    __in  PDEVICE_NODE    DeviceNode,
    __in_opt PVOID           Context
    )
/*++

Routine Description:

    This is a worker routine for PiMarkDeviceTreeForReenumeration. It marks all
    started devnodes with DNF_REENUMERATE so that the subsequent tree
    processing will reenumerate the device.

Arguments:

    DeviceNode - Device to mark if started.

    Context - Not used.

Return Value:

    NTSTATUS, non-successful statuses terminate the tree walk.

--*/
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(Context);

    if (DeviceNode->State == DeviceNodeStarted) {

        if (DeviceNode->Flags & DNF_REENUMERATE) {

            IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                         __FUNCTION__ "- %wZ already marked for reenumeration\n",
                         PnpGetDeviceInstancePath(DeviceNode)));
        } else {

            IopDbgPrint((IOP_ENUMERATION_VERBOSE_LEVEL,
                         __FUNCTION__ "- Reenumerating %wZ\n",
                         PnpGetDeviceInstancePath(DeviceNode)));
        }

        PipSetDevNodeFlags(DeviceNode, DNF_REENUMERATE);
    }

    return STATUS_SUCCESS;
}

BOOLEAN
PiCollapseEnumRequests(
    PLIST_ENTRY ListHead
    )
/*++

Routine Description:

    This function collapses reenumeration requests in the device action queue.

Parameters:

    ListHead - The collapses requests get added to the end of this list.

ReturnValue:

    None.

--*/
{
    KIRQL oldIrql;
    PPNP_DEVICE_ACTION_ENTRY  request;
    PLIST_ENTRY entry, next, last;
    PDEVICE_NODE deviceNode;

    ExAcquireSpinLock(&PnpSpinLock, &oldIrql);
    last = ListHead->Blink;
    //
    // Walk the list and build the list of collapsed requests.
    //
    for (entry = PnpEnumerationRequestList.Flink;
         entry != &PnpEnumerationRequestList;
         entry = next) {

        next = entry->Flink;
        request = CONTAINING_RECORD(entry, PNP_DEVICE_ACTION_ENTRY, ListEntry);
        if (request->ReorderingBarrier) {
            break;
        }
        switch(request->RequestType) {
        case ReenumerateRootDevices:
        case ReenumerateDeviceTree:
        case RestartEnumeration:
            //
            // Add it to our request list and mark the subtree.
            //
            RemoveEntryList(entry);
            InsertTailList(ListHead, entry);
            break;

        default:
            break;
        }
    }
    ExReleaseSpinLock(&PnpSpinLock, oldIrql);
    entry = last->Flink;
    while (entry != ListHead) {

        request = CONTAINING_RECORD(entry, PNP_DEVICE_ACTION_ENTRY, ListEntry);
        deviceNode = (PDEVICE_NODE)request->DeviceObject->DeviceObjectExtension->DeviceNode;
        PiMarkDeviceTreeForReenumeration(deviceNode, TRUE);
        ObDereferenceObject(request->DeviceObject);
        request->DeviceObject = NULL;
        entry = entry->Flink;
    }

    return (last != ListHead->Blink)? TRUE : FALSE;
}

NTSTATUS
PiProcessAddBootDevices(
    __in PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    This function invokes AddDevice for the specified boot device object.

Parameters:

    DeviceObject - Supplies a pointer to the device object for which to call
        AddDevice.

ReturnValue:

    STATUS_SUCCESS.

--*/
{

    PDEVICE_NODE deviceNode;
    ADD_CONTEXT addContext;
    HPNPOPCTX hPnpOpCtx;
    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(DeviceObject != NULL);
    hPnpOpCtx = NULL;
    deviceNode = (PDEVICE_NODE)DeviceObject->DeviceObjectExtension->DeviceNode;

    //
    // If the device has been added (or failed) skip it.
    //
    // If we know the device is a duplicate of another device which
    // has been enumerated at this point. we will skip this device.
    //

    if (deviceNode->State == DeviceNodeInitialized &&
        !PipDoesDevNodeHaveProblem(deviceNode) &&
        !(deviceNode->Flags & DNF_DUPLICATE) &&
        deviceNode->DuplicatePDO == NULL) {

        PiPnpRtlBeginOperation(&hPnpOpCtx); // If this fails, then the events will not be batched.

        //
        // Invoke driver's AddDevice Entry for the device.
        //
        addContext.DriverStartType = SERVICE_BOOT_START;
        addContext.LoadDriver = PnPBootDriversInitialized;

        status = PipCallDriverAddDevice(deviceNode, &addContext);
        if (NT_SUCCESS(status)) {

            //
            // Allow the power manager the opportunity
            // to prepare for this device to start.
            //

            PoFxPrepareDevice(deviceNode, FALSE);
        }
    }

    if (hPnpOpCtx) {

        PiPnpRtlEndOperation(hPnpOpCtx);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PiProcessClearDeviceProblem(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    )
/*++

Routine Description:

    This function processes the ClearDeviceProblem device action.

Parameters:

    Request - ClearDeviceProblem device action request.

    DeviceNode - Devnode on which the action needs to be performed.

ReturnValue:

    STATUS_SUCCESS or STATUS_INVALID_PARAMETER_2.

--*/
{
    NTSTATUS status;
    PDEVICE_NODE deviceNode;
    HPNPOPCTX hPnpOpCtx;

    PAGED_CODE();

    status = STATUS_SUCCESS;
    hPnpOpCtx = NULL;
    PNP_ASSERT(Request->DeviceObject != NULL);
    deviceNode = (PDEVICE_NODE)Request->DeviceObject->DeviceObjectExtension->DeviceNode;
    if (deviceNode->State == DeviceNodeUninitialized ||
        deviceNode->State == DeviceNodeInitialized ||
        deviceNode->State == DeviceNodeRemoved) {

        if (PipDoesDevNodeHaveProblem(deviceNode)) {

            if ((Request->RequestType == ClearDeviceProblem) &&
                (PipIsProblemReadonly(deviceNode, deviceNode->Problem))) {

                //
                // ClearDeviceProblem is a user mode request, and we don't let
                // user mode clear readonly problems!
                //
                status = STATUS_INVALID_PARAMETER_2;

            } else if ((Request->RequestType == ClearEjectProblem) &&
                       (!PipIsDevNodeProblem(deviceNode, CM_PROB_HELD_FOR_EJECT))) {

                //
                // Clear eject problem means clear CM_PROB_HELD_FOR_EJECT. If
                // it received another problem, we leave it alone.
                //
                status = STATUS_INVALID_DEVICE_REQUEST;

            } else {

                PiPnpRtlBeginOperation(&hPnpOpCtx); // If this fails, then the events will not be batched.

                PipClearDevNodeFlags(deviceNode, DNF_HAS_PRIVATE_PROBLEM);
                PipClearDevNodeProblem(deviceNode);
                if (deviceNode->State != DeviceNodeUninitialized) {
                    PnpRestartDeviceNode(deviceNode);
                }

                PNP_ASSERT(status == STATUS_SUCCESS);
            }

        }
    } else if (PipIsDevNodeDeleted(deviceNode)) {

        status = STATUS_DELETE_PENDING;
    }

    if (hPnpOpCtx) {

        PiPnpRtlEndOperation(hPnpOpCtx);
    }

    return status;
}

NTSTATUS
PpProcessClearProblem(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    )
/*++

Routine Description:

    This function processes the ClearProblem device action.

Parameters:

    Request - ClearDeviceProblem device action request.

ReturnValue:

    STATUS_SUCCESS or STATUS_INVALID_PARAMETER_2.

--*/
{
    CLEAR_PROBLEM_CONTEXT context;
    PDEVICE_NODE deviceNode;

    PAGED_CODE();

    context.Problem = (ULONG)(ULONG_PTR)Request->RequestArgument;
    context.Restart = TRUE;
    context.Limit   = PNP_DRIVER_UNLOAD_RETRY_LIMIT;

    PNP_ASSERT(Request->DeviceObject != NULL);

    deviceNode = PP_DO_TO_DN(Request->DeviceObject);
    if (PipIsDevNodeDeleted(deviceNode)) {
        return STATUS_DELETE_PENDING;
    }

    PipForDeviceNodeSubtree(deviceNode,
                            PiResetProblemDevicesWorker,
                            &context);

    return STATUS_SUCCESS;
}

NTSTATUS
PiProcessRequeryDeviceState(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    )
/*++

Routine Description:

    This function processes the RequeryDeviceState device action.

Parameters:

    Request - RequeryDeviceState device action request.

    DeviceNode - Devnode on which the action needs to be performed.

ReturnValue:

    STATUS_SUCCESS.

--*/
{
    PDEVICE_NODE deviceNode;
    NTSTATUS status;

    PAGED_CODE();

    status = STATUS_SUCCESS;
    PNP_ASSERT(Request->DeviceObject != NULL);
    deviceNode = (PDEVICE_NODE)Request->DeviceObject->DeviceObjectExtension->DeviceNode;
    if (deviceNode->State == DeviceNodeStarted) {
        PiProcessQueryDeviceState(Request->DeviceObject);

    } else if (PipIsDevNodeDeleted(deviceNode)) {
        status = STATUS_DELETE_PENDING;
    }

    return status;
}

NTSTATUS
PiProcessResourceRequirementsChanged(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    )
/*++

Routine Description:

    This function processes the ResourceRequirementsChanged device action.

Parameters:

    Request - ResourceRequirementsChanged device action request.

    DeviceNode - Devnode on which the action needs to be performed.

ReturnValue:

    STATUS_SUCCESS or STATUS_UNSUCCESSFUL.

--*/
{
    NTSTATUS status;
    ADD_CONTEXT addContext;
    PDEVICE_NODE deviceNode;
    LOGICAL synchronous;

    PAGED_CODE();

    PNP_ASSERT(Request->DeviceObject != NULL);

    deviceNode = PP_DO_TO_DN(Request->DeviceObject);

    //
    // Take a reference on the target device node in case it gets removed.
    //

    ObReferenceObject(Request->DeviceObject);

    PNP_ASSERT((deviceNode->StateFlags &DNSF_POFX_REQUIREMENTS_CHANGED) == 0);

    //
    // If the device has been deleted, nothing left to do.
    //

    if (PipIsDevNodeDeleted(deviceNode)) {
        status = STATUS_DELETE_PENDING;
        goto ResourceRequirementsChangedExit;
    }

    PoFxActivateDevice(Request->DeviceObject);
    deviceNode->StateFlags |= DNSF_POFX_REQUIREMENTS_CHANGED;

    //
    // Clear the NO_RESOURCE_REQUIRED flags.
    //
    PipClearDevNodeFlags(deviceNode, DNF_NO_RESOURCE_REQUIRED);
    //
    // If for some reason this device did not start, we need to clear some flags
    // such that it can be started later.  In this case, we call IopRequestDeviceEnumeration
    // with NULL device object, so the devices will be handled in non-started case.  They will
    // be assigned resources, started and enumerated.
    //
    PipSetDevNodeFlags(deviceNode, DNF_RESOURCE_REQUIREMENTS_CHANGED);
    PipClearDevNodeProblem(deviceNode);
    //
    // If the device is already started, we call IopRequestDeviceEnumeration with
    // the device object.
    //
    if (deviceNode->State == DeviceNodeStarted) {

        if (Request->RequestArgument == FALSE) {

            PipSetDevNodeFlags(deviceNode, DNF_NON_STOPPED_REBALANCE);

        } else {
            //
            // Explicitly clear it.
            //
            PipClearDevNodeFlags(deviceNode, DNF_NON_STOPPED_REBALANCE);
        }

        //
        // Reallocate resources for this devNode.
        //

        PnpReallocateResources(deviceNode);

        addContext.DriverStartType = SERVICE_DEMAND_START;
        addContext.LoadDriver = PnPBootDriversInitialized;

        if (Request->CompletionEvent) {

            synchronous = TRUE;
        } else {

            synchronous = FALSE;
        }

        ObReferenceObject(IopRootDeviceNode->PhysicalDeviceObject);
        status = PipProcessDevNodeTree(IopRootDeviceNode,
                                       Request,
                                       &addContext,
                                       EnumTypeNone,
                                       FALSE,          // ReallocateResources
                                       synchronous,    // Synchronous
                                       FALSE);          // ProcessOnlyIntermediateStates

        PNP_ASSERT(NT_SUCCESS(status));
        if (!NT_SUCCESS(status)) {

            status = STATUS_SUCCESS;
        }
    } else {

        status = STATUS_UNSUCCESSFUL;
    }

ResourceRequirementsChangedExit:

    //
    // Remove the active reference on the devnode.
    // There is the possibility that in the process of rebalancing resources,
    // the target device may be removed and restarted.  One example is where the
    // driver stack fails the start irp after rebalance.  This would trigger a
    // surprise removal and in turn block the current resource request while the
    // event queue (and the remove) is flushed.  If a query remove request for
    // the same device makes it way into the event queue prior to the flush, the
    // removed device will be re-enumerated and started (once the resource
    // request resumes).  The side effect is that the device node is no longer
    // active biased due to the current resource request.  As such, only remove
    // the active bias if the corresponding power flag is present.
    //

    if ((deviceNode->StateFlags & DNSF_POFX_REQUIREMENTS_CHANGED) != 0) {
        PoFxIdleDevice(deviceNode->PhysicalDeviceObject);
        deviceNode->StateFlags &= ~DNSF_POFX_REQUIREMENTS_CHANGED;
    }

    ObDereferenceObject(Request->DeviceObject);

    return status;
}

NTSTATUS
PiProcessReenumeration(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    )
/*++

Routine Description:

    This function processes the RestartEnumeration\ReenumerateRootDevices\
    ReenumerateDeviceTree\ReenumerateDeviceOnly device action.

Parameters:

    RequestList - List of reenumeration requests.

ReturnValue:

    STATUS_SUCCESS.

--*/
{
    PDEVICE_NODE deviceNode;
    ADD_CONTEXT addContext;
    ENUM_TYPE enumType;

    PAGED_CODE();

    PNP_ASSERT(Request->DeviceObject != NULL);
    deviceNode = (PDEVICE_NODE)Request->DeviceObject->DeviceObjectExtension->DeviceNode;
    if (PipIsDevNodeDeleted(deviceNode)) {

        //
        // Drop the ref since the caller is not going to.
        //
        ObDereferenceObject(Request->DeviceObject);

        return STATUS_DELETE_PENDING;
    }

    //
    // Reference the device object so it is valid for the duration of this
    // call. PipProcessDevNodeTree will dereference the device object (possibly
    // more than once, depending on why the references are there) and if it
    // removes the last reference, the device object will not be valid for the
    // call to PnpLogActionQueueEvent after PipProcessDevNodeTree returns.
    //

    ObReferenceObject(Request->DeviceObject);
    if (Request->RequestType == ReenumerateDeviceOnly) {
        if (((BOOLEAN)(Request->RequestArgument)) != FALSE) {
            PipClearDevNodeFlags(deviceNode, DNF_WAITING_FOR_FDO);
        }

        enumType = EnumTypeShallow;

    } else {
        enumType = EnumTypeDeep;
    }

    PnpLogActionQueueEvent(deviceNode,
                           Request->RequestType,
                           PNP_ACTION_START);

    PiMarkDeviceTreeForReenumeration(
        deviceNode,
        enumType != EnumTypeShallow ? TRUE : FALSE);

    addContext.DriverStartType = SERVICE_DEMAND_START;
    addContext.LoadDriver = PnPBootDriversInitialized;

    PipProcessDevNodeTree(deviceNode,
                          Request,
                          &addContext,
                          enumType,
                          FALSE,          // ReallocateResources
                          FALSE,          // Synchronous
                          FALSE);

    PnpLogActionQueueEvent(deviceNode,
                           Request->RequestType,
                           PNP_ACTION_STOP);

    //
    // Release the reference that was taken above, now that the device object is
    // no longer needed.
    //

    ObDereferenceObject(Request->DeviceObject);
    return STATUS_SUCCESS;
}

NTSTATUS
PiProcessSetDeviceProblem(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    )
/*++

Routine Description:

    This function processes the SetDeviceProblem device action.

Parameters:

    Request - SetDeviceProblem device action request.

    DeviceNode - Devnode on which the action needs to be performed.

ReturnValue:

    STATUS_SUCCESS or STATUS_INVALID_PARAMETER_2.

--*/
{
    PPLUGPLAY_CONTROL_STATUS_DATA statusData;
    ULONG   flags, userFlags;
    NTSTATUS status;
    PDEVICE_NODE deviceNode;
    HPNPOPCTX hPnpOpCtx = NULL;

    PAGED_CODE();

    PNP_ASSERT(Request->DeviceObject != NULL);
    statusData = (PPLUGPLAY_CONTROL_STATUS_DATA)Request->RequestArgument;
    deviceNode = (PDEVICE_NODE)Request->DeviceObject->DeviceObjectExtension->DeviceNode;
    if (PipIsDevNodeDeleted(deviceNode)) {
        status = STATUS_DELETE_PENDING;
        goto exit;
    }
    status = STATUS_SUCCESS;
    userFlags = 0;
    flags = 0;

    PiPnpRtlBeginOperation(&hPnpOpCtx);

    if (statusData->DeviceStatus & DN_WILL_BE_REMOVED) {

        userFlags |= DNUF_WILL_BE_REMOVED;
    }
    if (statusData->DeviceStatus & DN_NEED_RESTART) {

        userFlags |= DNUF_NEED_RESTART;
    }
    if (statusData->DeviceStatus & DN_PRIVATE_PROBLEM) {

        flags |= DNF_HAS_PRIVATE_PROBLEM;
    }
    if (statusData->DeviceStatus & DN_HAS_PROBLEM) {

        flags |= DNF_HAS_PROBLEM;
    }
    if (statusData->DeviceProblem == CM_PROB_NEED_RESTART) {

        PNP_ASSERT(flags & DNF_HAS_PROBLEM);

        //
        // If the device is in a small set of states where it isn't started and
        // usermode can set a problem code on it, a problem code should be set
        // on it, so the device doesn't get started.  It was decided that the
        // system should be restarted for this device to function fully, so the
        // device shouldn't be started until the system is restarted.
        //
        // However, if device is not in those states, don't set a problem code.
        // Usermode is never allowed to set a problem code if the device is not
        // in those states, and this should not be an exception.
        //

        if ((deviceNode->State != DeviceNodeInitialized) &&
            (deviceNode->State != DeviceNodeRemoved)) {

            //
            // The device is not in a state where setting a problem code is
            // allowed.  Clear the DNF_HAS_PROBLEM in the flags so it doesn't
            // fall through into trying to set a problem code below (and
            // cause this call to return an error).
            //

            flags &= ~DNF_HAS_PROBLEM;
        }

        //
        // Regardless of whether or not a problem code will be set on this device,
        // update its user flags to indicate the system should be restarted for
        // this device to function fully.
        //

        userFlags |= DNUF_NEED_RESTART;
    }

    if (flags & (DNF_HAS_PROBLEM | DNF_HAS_PRIVATE_PROBLEM)) {

        //
        // We allow any private problem code to be set. For standard PNP problems, it has
        // to be one of the ones we allow UserMode to set.
        //
        if (((deviceNode->State == DeviceNodeInitialized) ||
                (deviceNode->State == DeviceNodeRemoved)) &&
            ((flags & DNF_HAS_PRIVATE_PROBLEM) ||
                (statusData->DeviceProblem < NUM_CM_PROB &&
                !PipIsProblemReadonly(deviceNode, statusData->DeviceProblem)))) {

            PipSetDevNodeProblem(deviceNode, statusData->DeviceProblem, statusData->ProblemStatus);
            PipSetDevNodeFlags(deviceNode, flags);
            PipSetDevNodeUserFlags(deviceNode, userFlags);

        } else {

            status = STATUS_INVALID_PARAMETER_2;
        }
    } else {

        PipSetDevNodeFlags(deviceNode, flags);
        PipSetDevNodeUserFlags(deviceNode, userFlags);
    }

exit:
    if (statusData->Flags & PNP_DEVICESTATUS_ASYNCHRONOUS) {

        //
        // In the asynchronous case, a copy of the status data was
        // allocated.  Free it here.
        //

        ExFreePoolWithTag(statusData, PNP_POOLTAG_ASYNC_SET_STATUS);
    }

    if (hPnpOpCtx) {

        PiPnpRtlEndOperation(hPnpOpCtx);
    }

    return status;
}

NTSTATUS
PiProcessStartSystemDevices(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    )

/*++

Routine Description:

    This function processes the StartSystemDevicesPass0 device action.

Parameters:

    RequestList - List of reenumeration requests.

ReturnValue:

    STATUS_SUCCESS.

--*/

{

    PDEVICE_NODE deviceNode;
    ADD_CONTEXT addContext;

    PAGED_CODE();

    PnpDiagnosticTrace(&KMPnPEvt_SystemStartPnPEnum_Start, 0, NULL);
    deviceNode =
        (PDEVICE_NODE)Request->DeviceObject->DeviceObjectExtension->DeviceNode;

    addContext.DriverStartType = SERVICE_DEMAND_START;
    addContext.LoadDriver = PnPBootDriversInitialized;
    PipProcessDevNodeTree(deviceNode,
                          Request,
                          &addContext,
                          EnumTypeNone,
                          FALSE,
                          FALSE,
                          FALSE);

    PnpDiagnosticTrace(&KMPnPEvt_SystemStartPnPEnum_Stop, 0, NULL);
    return STATUS_SUCCESS;
}

VOID
PnpCompleteSystemStartProcess(
    VOID
    )

/*++

Routine Description:

    This function processes the StartSystemDevicesPass1 device action.

Parameters:

    RequestList - List of reenumeration requests.

ReturnValue:

    STATUS_SUCCESS.

--*/

{

    PAGED_CODE();

    PnpDiagnosticTrace(&KMPnPEvt_SystemStartLegacyEnum_Start, 0, NULL);

    //
    // IopCallDriverReinitializationRoutines is also called by the IO
    // manager.  For kernel pnp tracing purposes, only the following call is
    // off interest.  As such diagnostic traces are made outside the call
    // itself.
    //

    PnpDiagnosticTrace(&KMPnPEvt_SystemStartDriverReinit_Start, 0, NULL);
    IopCallDriverReinitializationRoutines(TRUE);
    PnpDiagnosticTrace(&KMPnPEvt_SystemStartDriverReinit_Stop, 0, NULL);

    //
    // Mark pnp has completed the driver loading for both system and
    // autoload drivers.
    //

    PnPInitialized = TRUE;
    KeSetEvent(&PnpSystemDeviceEnumerationComplete,
               0,
               FALSE);

    //
    // We don't need the group order list anymore. Release the cached data
    // associated with it.
    //

    PiInitReleaseCachedGroupInformation();

    //
    // Release the Boot Driver Database information.
    //

#if !defined(XBOX_SYSTEMOS)

    PpReleaseBootDDB();

#endif

    //
    // Mark to the kernel shim engine the fact that the
    // boot phase is pretty much done.
    //

#if !defined(XBOX_SYSTEMOS)

    KseShimDatabaseBootRelease();

#endif

    PnpDiagnosticTrace(&KMPnPEvt_SystemStartLegacyEnum_Stop, 0, NULL);

    //
    // Notify any consumers of overall driver init phase end.
    //

    PnpDiagnosticTraceDriverInitPhaseStop(&KMPnPEvt_DriverInitPhase_Stop);
    return;
}

NTSTATUS
PiConfigureDevice(
    __in PPNP_DEVICE_ACTION_ENTRY  Request
    )

/*++

Routine Description:

    This function processes the ConfigureDevice and ConfigureDeviceClass
    device actions.

Parameters:

    Request - ConfigureDevice device action request.

Return Value:

    NTSTATUS.

--*/

{
    NTSTATUS status;
    PDEVICE_NODE deviceNode;
    ULONG problemLast = 0;

    PAGED_CODE();

    PNP_ASSERT(Request->DeviceObject != NULL);

    deviceNode = (PDEVICE_NODE)Request->DeviceObject->DeviceObjectExtension->DeviceNode;

    if (PipIsDevNodeDeleted(deviceNode)) {
        status = STATUS_DELETE_PENDING;
        goto Cleanup;
    }

    switch (deviceNode->State) {

    case DeviceNodeAwaitingQueuedDeletion:
    case DeviceNodeAwaitingQueuedRemoval:
    case DeviceNodeQueryRemoved:
    case DeviceNodeRemovePendingCloses:
    case DeviceNodeDeletePendingCloses:
    case DeviceNodeDeleted:
    case DeviceNodeUnspecified:
        status = STATUS_UNSUCCESSFUL;
        goto Cleanup;

    default:
        break;
    }

    switch (Request->RequestType) {

    case ConfigureDevice:
        //
        // Configure device.
        //
        status = PpDevCfgProcessDevice(deviceNode, NULL, 0);
        break;

    case ConfigureDeviceClass:
        //
        // Configure device class.
        //
        status = PpDevCfgProcessDeviceClass(deviceNode);
        break;

    case ConfigureDeviceExtensions:
        //
        // Configure device extensions.
        //
        status = PpDevCfgProcessDeviceExtensions(deviceNode);
        break;

    case ConfigureDeviceReset:
        //
        // Configure device reset.
        //
        status = PpDevCfgProcessDeviceReset(deviceNode);
        break;

    default:
        PNP_ASSERT(0);
        status = STATUS_UNSUCCESSFUL;
        break;
    }

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

Cleanup:
    return status;
}

NTSTATUS
PnpCallAddDevice (
    __in PDEVICE_NODE       DeviceNode,
    __in PDRIVER_OBJECT     DriverObject,
    __in PDRIVER_ADD_DEVICE AddDeviceRoutine,
    __in ULONG              ServiceType
    )

/*++

Routine Description:

    This routine gets the NUMA proximity domain property for the device. If
    its not available for this device, it inherits the domain from its parent.
    It then invokes the AddDevice routine on one of the processors in the NUMA
    domain. This has the effect of the device extension getting allocated
    from local NUMA memory. This is *supposed* to enable better performance.

Arguments:

    DeviceNode - Supplies a pointer to the device.

    DriverObject - Supplies a pointer to the Driver whose AddDevice will be
        invoked.

    AddDeviceRoutine - Supplies a pointer to the AddDevice routine.

    ServiceType - PNP service type (service\filter etc).

Return Value:

    NTSTATUS.

--*/

{
    GROUP_AFFINITY oldAffinity;
    LOGICAL restoreAffinity;
    NTSTATUS status;

    PAGED_CODE();

    EventWriteKMPnPEvt_DeviceAdd_Start(NULL,
                                       ServiceType,
                                       DriverObject->DriverName.Length / sizeof(WCHAR),
                                       DriverObject->DriverName.Buffer,
                                       PnpGetDeviceInstancePath(DeviceNode)->Buffer);

    PNP_ASSERTMSG("NUMA node should be initialized at this point!",
                  DeviceNode->NumaNodeIndex != PNP_NUMA_NODE_UNKNOWN);

    //
    // Set the thread affinity to one of the processors in the local NUMA
    // domain.
    //

    restoreAffinity =
        PnpSetDeviceAffinityThread(DeviceNode->PhysicalDeviceObject,
                                   &oldAffinity);

    //
    // Call the AddDevice routine where the driver will create the device
    // extension in local NUMA memory.
    //

    status = PpvUtilCallAddDevice(DeviceNode->PhysicalDeviceObject,
                                  DriverObject,
                                  AddDeviceRoutine,
                                  VerifierTypeFromServiceType(ServiceType));

    if (restoreAffinity != FALSE) {

        //
        // Return to the original processor if affinity was changed before calling the
        // driver.
        //

        KeRevertToUserGroupAffinityThread(&oldAffinity);
    }

    EventWriteKMPnPEvt_DeviceAdd_Stop(NULL, status);
    return status;
}

#if DBG
VOID
PnpAssertDevnodesInConsistentState(
    VOID
    )
{
    PDEVICE_NODE deviceNode;

    PAGED_CODE();

    PNP_ASSERT(PnpDeviceCompletionQueueIsEmpty(&PnpDeviceCompletionQueue));

    deviceNode = IopRootDeviceNode;

    do {

        PNP_ASSERT(deviceNode->State == DeviceNodeUninitialized ||
               deviceNode->State == DeviceNodeInitialized ||
               deviceNode->State == DeviceNodeDriversAdded ||
               deviceNode->State == DeviceNodeResourcesAssigned ||
               deviceNode->State == DeviceNodeStarted ||
               deviceNode->State == DeviceNodeStartPostWork ||
               deviceNode->State == DeviceNodeAwaitingQueuedDeletion ||
               deviceNode->State == DeviceNodeAwaitingQueuedRemoval ||
               deviceNode->State == DeviceNodeRemovePendingCloses ||
               deviceNode->State == DeviceNodeRemoved);

        if (deviceNode->Child != NULL) {

            deviceNode = deviceNode->Child;

        } else {

            while (deviceNode->Sibling == NULL) {

                if (deviceNode->Parent != NULL) {
                    deviceNode = deviceNode->Parent;
                } else {
                    break;
                }
            }

            if (deviceNode->Sibling != NULL) {
                deviceNode = deviceNode->Sibling;
            }
        }

    } while (deviceNode != IopRootDeviceNode);
}
#endif

VOID
PnpLogDuplicateDevice (
    __in PWSTR BusId,
    __in PWSTR InstanceId
    )

/*++

Routine Description:

    This routine logs the bus id and the conflicting instance id for a device
    that is found to have the same device instance path as a previously
    enumerated device.  The conflict is logged in the error log.  The routine
    will attempt to log both the bus id and the instance id, otherwise the
    bus id will be truncated and as a last resort the instance id will be
    truncated as well.

Arguments:

    BusId - Supplies a pointer to the bus id (i.e PCI) to be logged.

    InstanceId - Supplies a pointer to the instance id to be logged.

Return Value:

    None

--*/

{
    UNICODE_STRING busIdString;
    UNICODE_STRING instanceIdString;

    PAGED_CODE();

    RtlInitUnicodeString(&busIdString, BusId);
    RtlInitUnicodeString(&instanceIdString, InstanceId);

    if ((instanceIdString.MaximumLength + sizeof(UNICODE_NULL)) <=
            ERROR_LOG_MAXIMUM_SIZE) {

        //
        // truncate the bus Id if necessary
        //

        if ((instanceIdString.MaximumLength + busIdString.MaximumLength) >
            ERROR_LOG_MAXIMUM_SIZE) {

            USHORT requiredLength = ERROR_LOG_MAXIMUM_SIZE -
                instanceIdString.MaximumLength;

            busIdString.Length = requiredLength - sizeof(UNICODE_NULL);
        }

    } else {

        USHORT requiredLength = ERROR_LOG_MAXIMUM_SIZE;

        //
        // There is only enough space for the instance id which may
        // require truncation.  If the instance id fits exactly
        // within the required space, then the length of the
        // instance string will be calculated to be the same.
        //

        instanceIdString.Length = requiredLength - sizeof(UNICODE_NULL);
        busIdString.Length = 0;
    }

    PnpLogEvent((busIdString.Length ? &busIdString : NULL),
                &instanceIdString,
                STATUS_PNP_INVALID_ID,
                NULL,
                0);

    return;
}

NTSTATUS
PnpCallDriverQueryServiceHelper(
    __deref_in_bcount(*propertyBufferSize) PUCHAR * propertyBuffer,
    __in PULONG propertyBufferSize,
    __in CM_REG_PROP property,
    __in PWSTR objectName,
    __in HANDLE objectKey,
    __in BOOLEAN isClassProperty,
    __in ADD_DRIVER_STAGE serviceType,
    __in PQUERY_CONTEXT queryContext
    )
{
    PWSTR currentService;
    ULONG currentServiceLength;
    ULONG propertyType;
    ULONG requiredSize;
    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(propertyBuffer != NULL);
    PNP_ASSERT(*propertyBuffer != NULL);
    PNP_ASSERT(objectName != NULL);
    PNP_ASSERT(objectKey != NULL);

    requiredSize = *propertyBufferSize;

    if (isClassProperty) {

        status = _CmGetInstallerClassRegProp(PiPnpRtlCtx,
                                             objectName,
                                             objectKey,
                                             property,
                                             &propertyType,
                                             *propertyBuffer,
                                             &requiredSize,
                                             0);

    } else {

        status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                     objectName,
                                     objectKey,
                                     property,
                                     &propertyType,
                                     *propertyBuffer,
                                     &requiredSize,
                                     0);

    }

    if (status == STATUS_BUFFER_TOO_SMALL) {

        ExFreePool(*propertyBuffer);

        *propertyBufferSize = requiredSize;

        *propertyBuffer = ExAllocatePool(PagedPool, *propertyBufferSize);

        if (*propertyBuffer == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        if (isClassProperty) {

            status = _CmGetInstallerClassRegProp(PiPnpRtlCtx,
                                                 objectName,
                                                 objectKey,
                                                 property,
                                                 &propertyType,
                                                 *propertyBuffer,
                                                 &requiredSize,
                                                 0);

        } else {

            status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                         objectName,
                                         objectKey,
                                         property,
                                         &propertyType,
                                         *propertyBuffer,
                                         &requiredSize,
                                         0);

        }

        PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);
    }

    if (!NT_SUCCESS(status)) {
        goto Cleanup;
    }

    if (propertyType == REG_MULTI_SZ) {

        if ((*propertyBufferSize < sizeof(WCHAR)) ||
            (requiredSize < sizeof(WCHAR))) {

            //
            // The buffer isn't large enough to hold a single unicode character
            // or the data returned for the property isn't large enough to hold a
            // single unicode character. Don't try to read from the buffer. This
            // should never happen, so assert.
            //

            PNP_ASSERT(FALSE);
            status = STATUS_UNSUCCESSFUL;
            goto Cleanup;
        }

        for (currentService = (PWSTR)(*propertyBuffer);
             *currentService;
             currentService += currentServiceLength) {

            currentServiceLength = (ULONG)(wcslen(currentService) + 1);

            status = RtlULongSub(requiredSize, currentServiceLength * sizeof(WCHAR), &requiredSize);

            if (!NT_SUCCESS(status)) {

                //
                // The length of the current service string goes past the end of
                // the property buffer.  Don't call the callback with this value.
                // This should never happen so assert if it does.
                //
                // Note that this isn't treated as an error.  This extra data in the
                // buffer is just ignored.
                //

                PNP_ASSERT(FALSE);
                status = STATUS_SUCCESS;
                goto Cleanup;
            }

            status = PipCallDriverAddDeviceQueryRoutine(REG_SZ,
                                                        currentService,
                                                        currentServiceLength * sizeof(WCHAR),
                                                        queryContext,
                                                        serviceType);

            if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }

            if (requiredSize < sizeof(WCHAR)) {

                //
                // Make sure there are enough bytes left in the property buffer
                // to at least be a single unicode character for the next iteration
                // of the loop. This should never happen so assert if it gets here.
                //
                // Note that this isn't treated as an error.  The extra byte in the
                // buffer is just ignored.
                //

                PNP_ASSERT(FALSE);
                goto Cleanup;
            }

        }

    } else {

        status = PipCallDriverAddDeviceQueryRoutine(propertyType,
                                                    (PWSTR)(*propertyBuffer),
                                                    requiredSize,
                                                    queryContext,
                                                    serviceType);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }
    }

  Cleanup:

    return status;
}

NTSTATUS
PnpGetServiceStartType(
    __in PCWSTR ServiceName,
    __in HANDLE ServiceKey,
    __out PULONG StartType
    )

/*++

Routine Description:

    This routine retrieves the start type of the specified service. It takes
    into account relevant start overrides that may alter the value from what is
    stored in the global services key.

Arguments:

    SerivceName - Supplies the name of the service to get the start type for.

    ServiceKey - Supplies a handle to the key for the service in the global
        services key.

    StartType - Returns the start type of the specified service.

Return Value:

    NTSTATUS code.

--*/

{

    HANDLE overrideKey;
    static HANDLE profileServicesKey = NULL;
    ULONG regSize;
    ULONG regType;
    NTSTATUS status;
    static HANDLE wildcardKey = NULL;

    PAGED_CODE();

    *StartType = SERVICE_DISABLED;

    overrideKey = NULL;
    status = STATUS_SUCCESS;

    //
    // Check for a manufacturing override to the start type.  No override being
    // present is not an error.
    //

    if (ExIsManufacturingModeEnabled()) {
        if (profileServicesKey == NULL) {
            status = _PnpCtxRegOpenKey(PiPnpRtlCtx,
                                       PNPRTL_HKEY_LOCAL_MACHINE,
                                       (REGSTR_KEY_MANUFACTURING_MODE REGSTR_KEY_MANUFACTURING_CURRENT L"\\" REGSTR_KEY_MANUFACTURING_SERVICES L"\\"),
                                       0,
                                       KEY_READ,
                                       &profileServicesKey);

            if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
                profileServicesKey = INVALID_HANDLE_VALUE;
            }
        } else if (profileServicesKey == INVALID_HANDLE_VALUE) {

            //
            // The key has already been attempted to be opened and it does not
            // exist.
            //

            status = STATUS_OBJECT_NAME_NOT_FOUND;
        } else {
            status = STATUS_SUCCESS;
        }

        if (NT_SUCCESS(status)) {

            //
            // Look for a override for this specific service.
            //

            status = _PnpCtxRegOpenKey(PiPnpRtlCtx,
                                       profileServicesKey,
                                       ServiceName,
                                       0,
                                       KEY_READ,
                                       &overrideKey);

            if (NT_SUCCESS(status)) {
                regSize = sizeof(ULONG);
                status = _PnpCtxRegQueryValue(PiPnpRtlCtx,
                                              overrideKey,
                                              REGSTR_VALUE_SERVICE_START,
                                              &regType,
                                              StartType,
                                              &regSize);

                if (NT_SUCCESS(status) && (regType == REG_DWORD)) {

                    //
                    // Manufacturing start override found.  Return this start
                    // type.
                    //

                    goto GetServiceStartTypeEnd;
                }
            }

            //
            // Look for a wildcard override.
            //

            if (wildcardKey == NULL) {
                status = _PnpCtxRegOpenKey(PiPnpRtlCtx,
                                           profileServicesKey,
                                           REGSTR_KEY_MANUFACTURING_DRIVER_WILDCARD,
                                           0,
                                           KEY_READ,
                                           &wildcardKey);

                if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
                    wildcardKey = INVALID_HANDLE_VALUE;
                }
            } else if (wildcardKey == INVALID_HANDLE_VALUE) {

                //
                // The key has already been attempted to be opened and it does not
                // exist.
                //

                status = STATUS_OBJECT_NAME_NOT_FOUND;
            } else {
                status = STATUS_SUCCESS;
            }

            if (NT_SUCCESS(status)) {
                regSize = sizeof(ULONG);
                status = _PnpCtxRegQueryValue(PiPnpRtlCtx,
                                              wildcardKey,
                                              REGSTR_VALUE_SERVICE_START,
                                              &regType,
                                              StartType,
                                              &regSize);

                if (NT_SUCCESS(status) && (regType == REG_DWORD)) {

                    //
                    // Manufacturing start override found.  Return this start
                    // type.
                    //

                    goto GetServiceStartTypeEnd;
                }
            }
        }
    }

    //
    // Look for the global start type.
    //

    regSize = sizeof(ULONG);
    status = _PnpCtxRegQueryValue(PiPnpRtlCtx,
                                  ServiceKey,
                                  REGSTR_VALUE_SERVICE_START,
                                  &regType,
                                  StartType,
                                  &regSize);

    if (!NT_SUCCESS(status)) {
        goto GetServiceStartTypeEnd;

    } else if (regType != REG_DWORD) {

        //
        // The value is the wrong type.  Act like the value doesn't exist.
        //

        status = STATUS_OBJECT_NAME_NOT_FOUND;
        *StartType = SERVICE_DISABLED;
        goto GetServiceStartTypeEnd;
    }

  GetServiceStartTypeEnd:

    if (overrideKey != NULL) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, overrideKey);
    }

    return status;
}

BOOLEAN
PnpCheckPossibleBootStartDriver(
    __in HANDLE ServiceKey
    )

/*++

Routine Description:

    This routine checks if the service contains a BootFlags value.  Although
    some drivers may not be marked as SERVICE_BOOT_START, they may still get
    promoted to boot start status if the BootFlags entry is not zero.  This is
    to support drivers that must be conditionally boot start such as iSCSI.

Arguments:

    ServiceKey - Supplies a handle to the key for the service in the global
        services key.

Return Value:

    TRUE if the service contains a BootFlags value, FALSE otherwise

--*/

{
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;
    BOOLEAN possibleBootStartDriver;
    NTSTATUS status;

    PAGED_CODE();

    possibleBootStartDriver = FALSE;

    status = IopGetRegistryValue(ServiceKey,
                                 L"BootFlags",
                                 0,
                                 &keyValueInformation);

    if (NT_SUCCESS(status)) {
        if ((keyValueInformation->Type == REG_DWORD) &&
            (keyValueInformation->DataLength == sizeof(ULONG)) &&
            ((*(PULONG)KEY_VALUE_DATA(keyValueInformation)) != 0)) {

            possibleBootStartDriver = TRUE;
        }

        ExFreePool(keyValueInformation);
    }

    return possibleBootStartDriver;
}

