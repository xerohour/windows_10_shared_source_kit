/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    irp.c

Abstract:

    This module contains IRP related routines.

Author:

    Shie-Lin Tzong (shielint) 13-Sept-1996

Revision History:

--*/

#include "pnpmgrp.h"
#pragma hdrstop

#define FAULT_INJECT_INVALID_ID DBG

#if DBG

#define PnpIrpStatusTracking(Status, IrpCode, Device)                    \
    if (PnpIrpMask & (1 << IrpCode)) {                                   \
        if (!NT_SUCCESS(Status) || Status == STATUS_PENDING) {           \
            DbgPrint(" ++ %s Driver ( %wZ ) return status %08lx\n",      \
                     IrpName[IrpCode],                                   \
                     &Device->DriverObject->DriverName,                  \
                     Status);                                            \
        }                                                                \
    }

ULONG PnpIrpMask;
const PCHAR IrpName[] = {
    "IRP_MN_START_DEVICE - ",                 // 0x00
    "IRP_MN_QUERY_REMOVE_DEVICE - ",          // 0x01
    "IRP_MN_REMOVE_DEVICE - ",                // 0x02
    "IRP_MN_CANCEL_REMOVE_DEVICE - ",         // 0x03
    "IRP_MN_STOP_DEVICE - ",                  // 0x04
    "IRP_MN_QUERY_STOP_DEVICE - ",            // 0x05
    "IRP_MN_CANCEL_STOP_DEVICE - ",           // 0x06
    "IRP_MN_QUERY_DEVICE_RELATIONS - ",       // 0x07
    "IRP_MN_QUERY_INTERFACE - ",              // 0x08
    "IRP_MN_QUERY_CAPABILITIES - ",           // 0x09
    "IRP_MN_QUERY_RESOURCES - ",              // 0x0A
    "IRP_MN_QUERY_RESOURCE_REQUIREMENTS - ",  // 0x0B
    "IRP_MN_QUERY_DEVICE_TEXT - ",            // 0x0C
    "IRP_MN_FILTER_RESOURCE_REQUIREMENTS - ", // 0x0D
    "INVALID_IRP_CODE - ",                    //
    "IRP_MN_READ_CONFIG - ",                  // 0x0F
    "IRP_MN_WRITE_CONFIG - ",                 // 0x10
    "IRP_MN_EJECT - ",                        // 0x11
    "IRP_MN_SET_LOCK - ",                     // 0x12
    "IRP_MN_QUERY_ID - ",                     // 0x13
    "IRP_MN_QUERY_PNP_DEVICE_STATE - ",       // 0x14
    "IRP_MN_QUERY_BUS_INFORMATION - ",        // 0x15
    "IRP_MN_DEVICE_USAGE_NOTIFICATION - ",    // 0x16
    NULL
};
#else
#define PnpIrpStatusTracking(Status, IrpCode, Device)
#endif

NTSTATUS
PiGetDefaultMessageString (
    __in HANDLE ServiceKey,
    __in ULONG MessageNumber,
    __deref_out PWSTR *DefaultString
    );

NTSTATUS
PiNormalizeDeviceText (
    __in PWSTR DriverText,
    __deref_out PWSTR *NormalizedText
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, IopSynchronousCall)
#pragma alloc_text(PAGE, IopEjectDevice)
#pragma alloc_text(PAGE, IopCancelPendingEject)
#pragma alloc_text(PAGE, IopRemoveDevice)
#pragma alloc_text(PAGE, PiIrpQueryRemoveDevice)
#pragma alloc_text(PAGE, IopQueryDeviceResources)
#pragma alloc_text(PAGE, IopQueryDockRemovalInterface)
#pragma alloc_text(PAGE, IopQueryLegacyBusInformation)
#pragma alloc_text(PAGE, IopQueryResourceHandlerInterface)
#pragma alloc_text(PAGE, IopQueryReconfiguration)
#pragma alloc_text(PAGE, IopFilterResourceRequirementsCall)
#pragma alloc_text(PAGE, IopQueryDeviceState)
#pragma alloc_text(PAGE, IopIncDisableableDepends)
#pragma alloc_text(PAGE, IopDecDisableableDepends)
#pragma alloc_text(PAGE, PnpQueryDeviceText)
#pragma alloc_text(PAGE, PnpIrpQueryID)
#pragma alloc_text(PAGE, PpIrpQueryResourceRequirements)
#pragma alloc_text(PAGE, PpIrpQueryCapabilities)
#pragma alloc_text(PAGE, PpIrpQueryBusInformation)
#pragma alloc_text(PAGE, PnpAsynchronousCall)
#pragma alloc_text(PAGE, PiGetDefaultMessageString)
#pragma alloc_text(PAGE, PiNormalizeDeviceText)

//#pragma alloc_text(NONPAGED, PnpDiagnosticCompletionRoutine)
//#pragma alloc_text(NONPAGED, PnpDeviceCompletionRoutine)

#endif  // ALLOC_PRAGMA

NTSTATUS
IopSynchronousCall(
    __in     PDEVICE_OBJECT         DeviceObject,
    __in     PIO_STACK_LOCATION     TopStackLocation,
    __in     NTSTATUS               DefaultStatus,
    __inout_opt ULONG_PTR           DefaultInformation OPTIONAL,
    __deref_opt_inout PULONG_PTR    Information
    )

/*++

Routine Description:

    This function sends a synchronous irp to the top level device
    object which roots on DeviceObject.

Parameters:

    DeviceObject - Supplies the device object of the target device.

    TopStackLocation - Supplies a pointer to the parameter block for the irp.

    Information - Supplies a pointer to a variable to receive the returned
                  information of the irp.

Return Value:

    NTSTATUS code.

--*/

{
    KEVENT event;
    PIRP irp;
    PIO_STACK_LOCATION irpSp;
    PDEVICE_OBJECT newTargetDevice;
    NTSTATUS status;
    IO_STATUS_BLOCK statusBlock;
    PDEVICE_OBJECT targetDevice;

    PAGED_CODE();

    //
    // Get a pointer to the topmost device object in the stack of devices,
    // beginning with the deviceObject.
    //

    targetDevice = IoGetAttachedDeviceReference(DeviceObject);

    //
    // Some IRPs may come in via an unsynchronized mechanism
    // (i.e. IoGetDeviceProperty, user mode control api, etc.).
    // To protect against sending this to an uninitialized stack, check the
    // state of the top most device object. If still initializing, send the irp
    // to the next lower device object.
    //

    if ((targetDevice->Flags & DO_DEVICE_INITIALIZING) != 0) {

        //
        // Get a reference on the next lower device object.
        //

        newTargetDevice = IoGetLowerDeviceObject(targetDevice);

        //
        // The next lower device could be null if its marked for unload.  In
        // this is the case, it's game over.
        //

        if (newTargetDevice == NULL) {
            status = STATUS_INVALID_DEVICE_STATE;
            goto SynchronousCallExit;
        }

        PNP_ASSERT((newTargetDevice->Flags & DO_DEVICE_INITIALIZING) == 0);

        ObDereferenceObject(targetDevice);
        targetDevice = newTargetDevice;
    }

    //
    // Begin by allocating the IRP for this request.  Do not charge quota to
    // the current process for this IRP.
    //

    irp = IoAllocateIrp(targetDevice->StackSize, FALSE);
    if (irp != NULL){
        SPECIALIRP_WATERMARK_IRP(irp, IRP_SYSTEM_RESTRICTED);

        //
        // Initialize it to failure.
        //

        irp->IoStatus.Status = statusBlock.Status = DefaultStatus;
        irp->IoStatus.Information = statusBlock.Information = DefaultInformation;

        //
        // Set the pointer to the status block and initialized event.
        //

        KeInitializeEvent(&event,
                          SynchronizationEvent,
                          FALSE);

        irp->UserIosb = &statusBlock;
        irp->UserEvent = &event;

        //
        // Set the address of the current thread
        //

        irp->Tail.Overlay.Thread = PsGetCurrentThread();

        //
        // Queue this irp onto the current thread
        //

        IopQueueThreadIrp(irp);

        //
        // Get a pointer to the stack location of the first driver which will be
        // invoked.  This is where the function codes and parameters are set.
        //

        irpSp = IoGetNextIrpStackLocation(irp);

        //
        // Copy in the caller-supplied stack location contents
        //

        *irpSp = *TopStackLocation;

        //
        // Call the driver
        //

        status = IoCallDriver(targetDevice, irp);
        PnpIrpStatusTracking(status,
                             TopStackLocation->MinorFunction,
                             targetDevice);

        //
        // If a driver returns STATUS_PENDING, we will wait for it to complete
        //

        if (status == STATUS_PENDING) {
            KeWaitForSingleObject(&event,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  (PLARGE_INTEGER)NULL);
            status = statusBlock.Status;
        }

        if (Information != NULL) {
            *Information = statusBlock.Information;
        }

        PNP_ASSERT(status != STATUS_PENDING);

    } else {
        status = STATUS_INSUFFICIENT_RESOURCES;
    }

SynchronousCallExit:
    ObDereferenceObject(targetDevice);
    return status;
}

NTSTATUS
IopEjectDevice(
    __in      PDEVICE_OBJECT                  DeviceObject,
    __inout  PPENDING_RELATIONS_LIST_ENTRY   PendingEntry
    )

/*++

Routine Description:

    This function sends an eject device irp to the top level device
    object which roots on DeviceObject.

Parameters:

    DeviceObject - Supplies a pointer to the device object of the device being
                   removed.

Return Value:

    NTSTATUS code.

--*/

{
    PIO_STACK_LOCATION irpSp;
    NTSTATUS status;
    PDEVICE_OBJECT deviceObject;
    PIRP irp;

    PAGED_CODE();

    if (PendingEntry->LightestSleepState != PowerSystemWorking) {

        //
        // We have to warm eject.
        //
        if (PendingEntry->DockInterface) {

            PendingEntry->DockInterface->ProfileDepartureSetMode(
                PendingEntry->DockInterface->Context,
                PDS_UPDATE_ON_EJECT
                );
        }

        PendingEntry->EjectIrp = NULL;

        InitializeListHead( &PendingEntry->Link );

        PnpQueuePendingEject(PendingEntry);

        ExInitializeWorkItem(&PendingEntry->WorkItem,
                             PnpProcessCompletedEject,
                             PendingEntry);

        ExQueueWorkItem( &PendingEntry->WorkItem, DelayedWorkQueue );
        return STATUS_SUCCESS;
    }

    if (PendingEntry->DockInterface) {

        //
        // Notify dock that now is a good time to update it's hardware profile.
        //
        PendingEntry->DockInterface->ProfileDepartureSetMode(
            PendingEntry->DockInterface->Context,
            PDS_UPDATE_ON_INTERFACE
            );

        PendingEntry->DockInterface->ProfileDepartureUpdate(
            PendingEntry->DockInterface->Context
            );

        if (PendingEntry->DisplaySafeRemovalDialog) {

            PendingEntry->DisplaySafeRemovalDialog = FALSE;
        }
    }

    //
    // Get a pointer to the topmost device object in the stack of devices,
    // beginning with the deviceObject.
    //

    deviceObject = IoGetAttachedDeviceReference(DeviceObject);

    //
    // Allocate an I/O Request Packet (IRP) for this device removal operation.
    //

    irp = IoAllocateIrp( (CCHAR) (deviceObject->StackSize), FALSE );
    if (!irp) {

        PendingEntry->EjectIrp = NULL;

        InitializeListHead( &PendingEntry->Link );

        PnpQueuePendingEject(PendingEntry);

        ExInitializeWorkItem(&PendingEntry->WorkItem,
                             PnpProcessCompletedEject,
                             PendingEntry);

        ExQueueWorkItem( &PendingEntry->WorkItem, DelayedWorkQueue );

        ObDereferenceObject(deviceObject);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    SPECIALIRP_WATERMARK_IRP(irp, IRP_SYSTEM_RESTRICTED);

    //
    // Initialize it to failure.
    //

    irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
    irp->IoStatus.Information = 0;

    //
    // Get a pointer to the next stack location in the packet.  This location
    // will be used to pass the function codes and parameters to the first
    // driver.
    //

    irpSp = IoGetNextIrpStackLocation(irp);

    //
    // Initialize the stack location to pass to IopSynchronousCall()
    //

    RtlZeroMemory(irpSp, sizeof(IO_STACK_LOCATION));

    //
    // Set the function codes.
    //

    irpSp->MajorFunction = IRP_MJ_PNP;
    irpSp->MinorFunction = IRP_MN_EJECT;

    //
    // Fill in the IRP according to this request.
    //

    irp->Tail.Overlay.Thread = PsGetCurrentThread();
    irp->RequestorMode = KernelMode;
    irp->UserIosb = NULL;
    irp->UserEvent = NULL;

    PendingEntry->EjectIrp = irp;
    PendingEntry->Lock = IRPLOCK_CANCELABLE;

    PnpQueuePendingEject(PendingEntry);

    IoSetCompletionRoutine(irp,
                           IopDeviceEjectComplete,
                           PendingEntry,       /* Completion context */
                           TRUE,               /* Invoke on success  */
                           TRUE,               /* Invoke on error    */
                           TRUE                /* Invoke on cancel   */
                           );

    status = IoCallDriver( deviceObject, irp );

    ObDereferenceObject(deviceObject);
    return status;
}

_Function_class_(IO_COMPLETION_ROUTINE)
_IRQL_requires_same_
NTSTATUS
IopDeviceEjectComplete (
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp,
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context
    )
{
    PPENDING_RELATIONS_LIST_ENTRY entry = (PPENDING_RELATIONS_LIST_ENTRY)Context;
    IRPLOCK oldState;

    UNREFERENCED_PARAMETER( DeviceObject );

    PNP_ASSERT(entry->EjectIrp == Irp);

    //
    // Indicate the IRP has been completed. After this point, the IRP may be
    // freed.
    //
    oldState = InterlockedExchange((PLONG) &entry->Lock, IRPLOCK_COMPLETED);

    //
    // Queue a work item to finish up the eject.  We queue a work item because
    // we are probably running at dispatch level in some random context.
    //

    ExInitializeWorkItem(&entry->WorkItem,
                         PnpProcessCompletedEject,
                         entry);

    ExQueueWorkItem( &entry->WorkItem, DelayedWorkQueue );

    if (oldState != IRPLOCK_CANCEL_STARTED) {

        //
        // The oldstate was either IRPLOCK_CANCELABLE, or
        // IRPLOCK_CANCEL_COMPLETE.
        //
        IoFreeIrp( Irp );

    } else {

        //
        // The IRP is actively being cancelled. When the cancelling routine
        // tries to change the state, it will find out it owns the IRP cleanup.
        //
        NOTHING;
    }

    return STATUS_MORE_PROCESSING_REQUIRED;

}

VOID
IopCancelPendingEject(
    __in PPENDING_RELATIONS_LIST_ENTRY Entry
    )
/*++

Routine Description:

    This function safely cancels a pending eject. The caller must ensure
    relation list containing the IRP lock is valid throughout the duration
    of this call. The IRP is not gauranteed to have been completed by the
    time this call returns.

Parameters:

    Entry - Relation list containing the eject IRP to cancel.

Return Value:

    None.

--*/
{
    if (Entry->EjectIrp == NULL) {

        return;
    }

    if (InterlockedExchange((PLONG) &Entry->Lock, IRPLOCK_CANCEL_STARTED) == IRPLOCK_CANCELABLE) {

        //
        // We got it to the IRP before it was completed. We can cancel
        // the IRP without fear of losing it, as the completion routine
        // won't let go of the IRP until we say so.
        //
        IoCancelIrp(Entry->EjectIrp);

        //
        // Release the completion routine. If it already got there,
        // then we need to handle post-processing ourselves. Otherwise we got
        // through IoCancelIrp before the IRP completed entirely.
        //
        if (InterlockedExchange((PLONG) &Entry->Lock, IRPLOCK_CANCEL_COMPLETE) == IRPLOCK_COMPLETED) {

            //
            // Free the IRP.
            //
            IoFreeIrp(Entry->EjectIrp);
        }

    } else {

        //
        // The entry was completed, meaning the IRP is gone.
        //
        NOTHING;
    }
}

NTSTATUS
IopRemoveDevice (
    __in PDEVICE_OBJECT TargetDevice,
    __in ULONG IrpMinorCode
    )

/*++

Routine Description:

    This function sends a requested DeviceRemoval related irp to the top level device
    object which roots on TargetDevice.  If there is a VPB associated with the
    TargetDevice, the corresponding filesystem's VDO will be used.  Otherwise
    the irp will be sent directly to the target device/ or its assocated device
    object.

Parameters:

    TargetDevice - Supplies the device object of the device being removed.

    Operation - Specifies the operation requested.
        The following IRP codes are used with IRP_MJ_DEVICE_CHANGE for removing
        devices:
            IRP_MN_QUERY_REMOVE_DEVICE
            IRP_MN_CANCEL_REMOVE_DEVICE
            IRP_MN_REMOVE_DEVICE
            IRP_MN_EJECT
Return Value:

    NTSTATUS code.

--*/
{
    IO_STACK_LOCATION irpSp;
    NTSTATUS status;
    LOGICAL isMountable = FALSE;
    PDEVICE_OBJECT mountedDevice;
    PNP_LOCK_MOUNTABLE_DEVICE_CONTEXT lockContext = {0};
    PDEVICE_NODE deviceNode;
    HPNPOPCTX hPnpOpCtx;

    PAGED_CODE();

    PNP_ASSERT(IrpMinorCode == IRP_MN_QUERY_REMOVE_DEVICE ||
               IrpMinorCode == IRP_MN_CANCEL_REMOVE_DEVICE ||
               IrpMinorCode == IRP_MN_REMOVE_DEVICE ||
               IrpMinorCode == IRP_MN_SURPRISE_REMOVAL ||
               IrpMinorCode == IRP_MN_EJECT);

    isMountable = FALSE;
    deviceNode = PP_DO_TO_DN(TargetDevice);
    hPnpOpCtx = NULL;

    PiPnpRtlBeginOperation(&hPnpOpCtx); // If this fails, then the events will not be batched.

    if (IrpMinorCode == IRP_MN_REMOVE_DEVICE) {
        IopUncacheInterfaceInformation(TargetDevice, TRUE);
    }

    //
    // Initialize the stack location to pass to IopSynchronousCall()
    //

    RtlZeroMemory(&irpSp, sizeof(IO_STACK_LOCATION));
    irpSp.MajorFunction = IRP_MJ_PNP;
    irpSp.MinorFunction = (UCHAR)IrpMinorCode;

    //
    // Check to see if there's a VPB anywhere in the device stack.  If there
    // is then we'll have to lock the stack. This is to make sure that the VPB
    // does not go away while the operation is in the file system and that no
    // one new can mount on the device if the FS decides to bail out.
    //

    mountedDevice = PnpFindMountableDevice(TargetDevice);
    if (mountedDevice != NULL) {

        //
        // Mark the VPB to prevent mounts during PNP removal.
        //

        isMountable = TRUE;
        PnpLockMountableDevice(TargetDevice);
        mountedDevice = PnpMarkDeviceForRemove(TargetDevice,
                                               TRUE,
                                               &lockContext);

        PnpUnlockMountableDevice(TargetDevice);

    } else {
        PNP_ASSERTMSG("Mass storage device does not have VPB - this is odd",
                      !((TargetDevice->Type == FILE_DEVICE_DISK) ||
                        (TargetDevice->Type == FILE_DEVICE_CD_ROM) ||
                        (TargetDevice->Type == FILE_DEVICE_TAPE) ||
                        (TargetDevice->Type == FILE_DEVICE_VIRTUAL_DISK)));
        mountedDevice = TargetDevice;
    }

    //
    // Make the call and return.
    //

    if (IrpMinorCode == IRP_MN_SURPRISE_REMOVAL ||
        IrpMinorCode == IRP_MN_REMOVE_DEVICE) {

        //
        // if device was not disableable, we cleanup the tree
        // and debug-trace that we surprise-removed a non-disableable device
        //

        if (deviceNode->UserFlags & DNUF_NOT_DISABLEABLE) {

            //
            // this device was marked as disableable, update the depends
            // before this device disappears
            // (by momentarily marking this node as disableable)
            //

            PipClearDevNodeUserFlags(deviceNode, DNUF_NOT_DISABLEABLE);
            IopDecDisableableDepends(deviceNode);
        }
    }

    if (IrpMinorCode == IRP_MN_REMOVE_DEVICE)
    {
        PiSwProcessParentRemoveIrp(TargetDevice);

        PiDestroyDriverSwDevices(deviceNode);
    }

    status = IopSynchronousCall(mountedDevice,
                                &irpSp,
                                STATUS_NOT_SUPPORTED,
                                0,
                                NULL);
    IopDbgPrint((IOP_INFO_LEVEL,
                 "IopRemoveDevice: MinorCode = %d, Status = %08x\n",
                 IrpMinorCode,
                 status));
    if (isMountable) {

        PnpLockMountableDevice(TargetDevice);
        if (IrpMinorCode == IRP_MN_REMOVE_DEVICE ||
            IrpMinorCode == IRP_MN_CANCEL_REMOVE_DEVICE) {

            //
            // Allow new mounts.
            //

            PnpMarkDeviceForRemove(TargetDevice,
                                   FALSE,
                                   &lockContext);
        }

        if (lockContext.FsDevice != NULL) {
            IopDecrementDeviceObjectHandleCount(lockContext.FsDevice);
            ObDereferenceObject(lockContext.FsDevice);
        }

        PnpUnlockMountableDevice(TargetDevice);
        if ((IrpMinorCode == IRP_MN_QUERY_REMOVE_DEVICE && NT_SUCCESS(status)) ||
            (IrpMinorCode == IRP_MN_SURPRISE_REMOVAL)) {

            //
            // Successful query should follow up with invalidation of
            // all volumes which have been on this device but which are
            // not currently mounted.
            //

            status = IopInvalidateVolumesForDevice(TargetDevice);
        }
    }

    if (IrpMinorCode == IRP_MN_SURPRISE_REMOVAL ||
        IrpMinorCode == IRP_MN_REMOVE_DEVICE) {

        //
        // Notify the power manager to abandon this device.
        //

        PoFxAbandonDevice(deviceNode);

        //
        // Mark the device powerstate as PowerUnspecified
        //

        deviceNode->CurrentPowerState.DeviceState = PowerDeviceUnspecified;
    }

    if (IrpMinorCode == IRP_MN_REMOVE_DEVICE) {
        PipClearDevNodeFlags(deviceNode, DNF_LEGACY_DRIVER | DNF_REENUMERATE);
        if (deviceNode->PreviousParent != NULL &&
            (((ULONG_PTR) deviceNode->PreviousParent) & PNP_DN_PREVIOUS_PARENT_REFERENCED) != FALSE) {

            //
            // One more deleted child got his last remove sent.
            // Remove the tag PNP_DN_PREVIOUS_PARENT_REFERENCED.
            //
                
            deviceNode->PreviousParent = PNP_DN_PREVIOUS_PARENT(deviceNode);
            deviceNode->PreviousParent->DeletedChildren -= 1;

            ASSERT_NOT_NEGATIVE(deviceNode->PreviousParent->DeletedChildren);
        }
    }

    if (hPnpOpCtx) {

        PiPnpRtlEndOperation(hPnpOpCtx);
    }

    return status;
}

NTSTATUS
IopQueryDeviceResources (
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG ResourceType,
    __out PVOID *Resource,
    __out ULONG *Length
    )

/*++

Routine Description:

    This routine sends irp to queries resources or resource requirements list
    of the specified device object.

    If the device object is a detected device, its resources will be read from
    registry.  Otherwise, an irp is sent to the bus driver to query its resources.

Parameters:

    DeviceObject - Supplies the device object of the device being queries.

    ResourceType - 0 for device resources and 1 for resource requirements list.

    Resource - Supplies a pointer to a variable to receive the returned resources

    Length - Supplies a pointer to a variable to receive the length of the returned
             resources or resource requirements list.

Return Value:

    NTSTATUS code.

--*/
{
    IO_STACK_LOCATION irpSp;
    PDEVICE_NODE deviceNode;
    NTSTATUS status;
    PIO_RESOURCE_REQUIREMENTS_LIST resReqList, newResources;
    ULONG junk;
    PCM_RESOURCE_LIST cmList;
    PIO_RESOURCE_REQUIREMENTS_LIST filteredList, mergedList;
    LOGICAL exactMatch;

    PAGED_CODE();

#if DBG

    if ((ResourceType != QUERY_RESOURCE_LIST) &&
        (ResourceType != QUERY_RESOURCE_REQUIREMENTS)) {

        PNP_ASSERT(0);
        return STATUS_INVALID_PARAMETER_2;
    }
#endif

    *Resource = NULL;
    *Length = 0;

    //
    // Initialize the stack location to pass to IopSynchronousCall()
    //

    RtlZeroMemory(&irpSp, sizeof(IO_STACK_LOCATION));

    deviceNode = (PDEVICE_NODE) DeviceObject->DeviceObjectExtension->DeviceNode;

    if (ResourceType == QUERY_RESOURCE_LIST) {

        //
        // caller is asked for RESOURCE_LIST.  If this is a madeup device, we will
        // read it from registry.  Otherwise, we ask drivers.
        //

        if (deviceNode->Flags & DNF_MADEUP) {

            status = PnpGetDeviceResourcesFromRegistry(
                             DeviceObject,
                             ResourceType,
                             REGISTRY_ALLOC_CONFIG + REGISTRY_FORCED_CONFIG + REGISTRY_BOOT_CONFIG,
                             Resource,
                             Length);
            if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
                status = STATUS_SUCCESS;
            }
            return status;
        } else {
            irpSp.MinorFunction = IRP_MN_QUERY_RESOURCES;
            irpSp.MajorFunction = IRP_MJ_PNP;
            status = IopSynchronousCall(DeviceObject,
                                        &irpSp,
                                        STATUS_NOT_SUPPORTED,
                                        0,
                                        (PULONG_PTR)Resource);
            if (status == STATUS_NOT_SUPPORTED) {

                //
                // If driver doesn't implement this request, it
                // doesn't consume any resources.
                //

                *Resource = NULL;
                status = STATUS_SUCCESS;
            }
            if (NT_SUCCESS(status) && Resource) {
                *Length = PnpDetermineResourceListSize((PCM_RESOURCE_LIST)*Resource);
            }
            return status;
        }
    } else {

        //
        // Caller is asked for resource requirements list.  We will check:
        // if there is a ForcedConfig, it will be converted to resource requirements
        //     list and return.  Otherwise,
        // If there is an OVerrideConfigVector, we will use it as our
        //     FilterConfigVector.  Otherwise we ask driver for the config vector and
        //     use it as our FilterConfigVector.
        //     Finaly, we pass the FilterConfigVector to driver stack to let drivers
        //     filter the requirements.
        //

        status = PnpGetDeviceResourcesFromRegistry(
                         DeviceObject,
                         QUERY_RESOURCE_LIST,
                         REGISTRY_FORCED_CONFIG,
                         Resource,
                         &junk);
        if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
            status = PnpGetDeviceResourcesFromRegistry(
                             DeviceObject,
                             QUERY_RESOURCE_REQUIREMENTS,
                             REGISTRY_OVERRIDE_CONFIGVECTOR,
                             &resReqList,
                             &junk);
            if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
                if (deviceNode->Flags & DNF_MADEUP) {
                    status = PnpGetDeviceResourcesFromRegistry(
                                     DeviceObject,
                                     QUERY_RESOURCE_REQUIREMENTS,
                                     REGISTRY_BASIC_CONFIGVECTOR,
                                     &resReqList,
                                     &junk);
                    if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
                        status = STATUS_SUCCESS;
                        resReqList = NULL;
                    }
                } else {

                    //
                    // We are going to ask the bus driver ...
                    //

                    if (deviceNode->ResourceRequirements) {
                        PNP_ASSERT(deviceNode->Flags & DNF_RESOURCE_REQUIREMENTS_NEED_FILTERED);
                        resReqList = ExAllocatePool(PagedPool, deviceNode->ResourceRequirements->ListSize);
                        if (resReqList) {
                            RtlCopyMemory(resReqList,
                                         deviceNode->ResourceRequirements,
                                         deviceNode->ResourceRequirements->ListSize
                                         );
                            status = STATUS_SUCCESS;
                        } else {
                            return STATUS_NO_MEMORY;
                        }
                    } else {
                        status = PpIrpQueryResourceRequirements(DeviceObject, &resReqList);
                        if (status == STATUS_NOT_SUPPORTED) {

                            PNP_ASSERT(resReqList == NULL);
                            resReqList = NULL;
                            //
                            // If driver doesn't implement this request, it
                            // doesn't require any resources.
                            //
                            status = STATUS_SUCCESS;
                        }
                    }
                }
                if (!NT_SUCCESS(status)) {
                    return status;
                }
            }

            //
            // For devices with boot config, we need to filter the resource requirements
            // list against boot config.
            //

            status = PnpGetDeviceResourcesFromRegistry(
                             DeviceObject,
                             QUERY_RESOURCE_LIST,
                             REGISTRY_BOOT_CONFIG,
                             &cmList,
                             &junk);
            if (NT_SUCCESS(status) &&
                (!cmList || cmList->Count == 0 || cmList->List[0].InterfaceType != PCIBus)) {
                status = PnpFilterResourceRequirementsList (
                             resReqList,
                             cmList,
                             &filteredList,
                             &exactMatch);
                if (cmList) {
                    ExFreePool(cmList);
                }
                if (!NT_SUCCESS(status)) {
                    if (resReqList) {
                        ExFreePool(resReqList);
                    }
                    return status;
                } else {

                    //
                    // For non-root-enumerated devices, we merge filtered config with basic config
                    // vectors to form a new res req list.  For root-enumerated devices, we don't
                    // consider Basic config vector.
                    //

                    if (!(deviceNode->Flags & DNF_MADEUP) &&
                        (exactMatch == FALSE || resReqList->AlternativeLists > 1)) {
                        status = PnpMergeFilteredResourceRequirementsList (
                                 filteredList,
                                 resReqList,
                                 &mergedList
                                 );
                        if (resReqList) {
                            ExFreePool(resReqList);
                        }
                        if (filteredList) {
                            ExFreePool(filteredList);
                        }
                        if (NT_SUCCESS(status)) {
                            resReqList = mergedList;
                        } else {
                            return status;
                        }
                    } else {
                        if (resReqList) {
                            ExFreePool(resReqList);
                        }
                        resReqList = filteredList;
                    }
                }
            }

        } else {
            PNP_ASSERT(NT_SUCCESS(status) || (status == STATUS_TOO_LATE));

            //
            // We have Forced Config.  Convert it to resource requirements and return it.
            //

            if (*Resource) {
                resReqList = PnpCmResourcesToIoResources(0,
                                                         (PCM_RESOURCE_LIST)*Resource,
                                                         LCPRI_FORCECONFIG);
                ExFreePool(*Resource);
                if (resReqList) {
                    *Resource = (PVOID)resReqList;
                    *Length = resReqList->ListSize;
                } else {
                    *Resource = NULL;
                    *Length = 0;
                    status = STATUS_INSUFFICIENT_RESOURCES;
                    return status;
                }
            } else {
                resReqList = NULL;
            }
        }

        //
        // If we are here, we have a resource requirements list for drivers to examine ...
        // NOTE: Per Lonny's request, we let drivers filter ForcedConfig
        //

        status = IopFilterResourceRequirementsCall(DeviceObject,
                                                   resReqList,
                                                   &newResources);
        if (NT_SUCCESS(status)) {

            UNICODE_STRING unicodeName;
            HANDLE handle;

#if DBG
            if (newResources == NULL && resReqList) {
                IopDbgPrint((IOP_WARNING_LEVEL,
                             "PnpMgr: Non-NULL resource requirements list filtered to NULL!\n"));
            }
#endif
            if (newResources) {

                //
                // Make our own copy of the allocation. We do this so that the
                // verifier doesn't believe the driver has leaked memory if
                // unloaded.
                //
                *Length = newResources->ListSize;
                PNP_ASSERT(*Length);

                *Resource = (PVOID) ExAllocatePool(PagedPool, *Length);
                if (*Resource == NULL) {

                    ExFreePool(newResources);
                    return STATUS_INSUFFICIENT_RESOURCES;
                }

                RtlCopyMemory(*Resource, newResources, *Length);
                ExFreePool(newResources);

            } else {

                //
                // The driver stack must have freed the original requirements.
                // According to the DDK, the driver must free the original list
                // if the size is going to change during filtering.
                //
                *Length = 0;
                *Resource = NULL;
            }

            //
            // Write filtered res req to registry
            //

            status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                         PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                         CM_KEY_TYPE_DEVICE_CONTROL,
                                         0,
                                         KEY_READ,
                                         TRUE,
                                         &handle,
                                         NULL);

            if (NT_SUCCESS(status)) {

                PiWstrToUnicodeString(&unicodeName, REGSTR_VALUE_FILTERED_CONFIG_VECTOR);
                ZwSetValueKey(handle,
                              &unicodeName,
                              TITLE_INDEX_VALUE,
                              REG_RESOURCE_REQUIREMENTS_LIST,
                              *Resource,
                              *Length);
                ZwClose(handle);
            }

        } else {

            //
            // We ignore failure from IRP_MN_FILTER_RESOURCE_REQUIREMENTS.
            // The device will fail to start if the driver failed to filter
            // requirements and such a filtering was critical.
            //
            *Resource = resReqList;
            if (resReqList) {
                *Length = resReqList->ListSize;
            } else {
                *Length = 0;
            }
        }

        return STATUS_SUCCESS;
    }
}

NTSTATUS
IopQueryResourceHandlerInterface(
    __in RESOURCE_HANDLER_TYPE HandlerType,
    __in PDEVICE_OBJECT DeviceObject,
    __in UCHAR ResourceType,
    __out PVOID *Interface
    )

/*++

Routine Description:

    This routine queries the specified DeviceObject for the specified ResourceType
    resource translator.

Parameters:

    HandlerType - specifies Arbiter or Translator

    DeviceObject - Supplies a pointer to the Device object to be queried.

    ResourceType - Specifies the desired type of translator.

    Interface - supplies a variable to receive the desired interface.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/
{
    IO_STACK_LOCATION irpSp;
    NTSTATUS status;
    PINTERFACE interface;
    USHORT size;
    GUID interfaceType;
    PDEVICE_NODE deviceNode = (PDEVICE_NODE)DeviceObject->DeviceObjectExtension->DeviceNode;

    PAGED_CODE();

    //
    // If this device object is created by pnp mgr for legacy resource allocation,
    // skip it.
    //

    if ((deviceNode->DuplicatePDO == (PDEVICE_OBJECT) DeviceObject->DriverObject) ||
        !(DeviceObject->Flags & DO_BUS_ENUMERATED_DEVICE)) {
        return STATUS_NOT_SUPPORTED;
    }

    switch (HandlerType) {
    case ResourceTranslator:
        size = sizeof(TRANSLATOR_INTERFACE) + 4;  // Pnptest
        //size = sizeof(TRANSLATOR_INTERFACE);
        interfaceType = GUID_TRANSLATOR_INTERFACE_STANDARD;
        break;

    case ResourceArbiter:
        size = sizeof(ARBITER_INTERFACE);
        interfaceType = GUID_ARBITER_INTERFACE_STANDARD;
        break;

    case ResourceLegacyDeviceDetection:
        size = sizeof(LEGACY_DEVICE_DETECTION_INTERFACE);
        interfaceType = GUID_LEGACY_DEVICE_DETECTION_STANDARD;
        break;

    default:
        return STATUS_INVALID_PARAMETER;
    }

    interface = (PINTERFACE) ExAllocatePool(PagedPool, size);
    if (interface == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(interface, size);
    interface->Size = size;

    //
    // Initialize the stack location to pass to IopSynchronousCall()
    //

    RtlZeroMemory(&irpSp, sizeof(IO_STACK_LOCATION));

    //
    // Set the function codes.
    //

    irpSp.MajorFunction = IRP_MJ_PNP;
    irpSp.MinorFunction = IRP_MN_QUERY_INTERFACE;

    //
    // Set the pointer to the resource list
    //

    irpSp.Parameters.QueryInterface.InterfaceType = &interfaceType;
    irpSp.Parameters.QueryInterface.Size = interface->Size;
    irpSp.Parameters.QueryInterface.Version = interface->Version = 0;
    irpSp.Parameters.QueryInterface.Interface = interface;
    irpSp.Parameters.QueryInterface.InterfaceSpecificData = (PVOID) (ULONG_PTR) ResourceType;

    //
    // Make the call and return.
    //

    status = IopSynchronousCall(DeviceObject,
                                &irpSp,
                                STATUS_NOT_SUPPORTED,
                                0,
                                NULL);
    if (NT_SUCCESS(status)) {

        switch (HandlerType) {

        case ResourceTranslator:
            if (    ((PTRANSLATOR_INTERFACE)interface)->TranslateResources == NULL ||
                    ((PTRANSLATOR_INTERFACE)interface)->TranslateResourceRequirements == NULL) {

                IopDbgPrint((IOP_ERROR_LEVEL,
                             "!devstack %p returned success for IRP_MN_QUERY_INTERFACE (GUID_TRANSLATOR_INTERFACE_STANDARD) but did not fill in the required data\n",
                             DeviceObject));
                PNP_ASSERT(!NT_SUCCESS(status));
                status = STATUS_UNSUCCESSFUL;
            }
            break;

        case ResourceArbiter:
            if (((PARBITER_INTERFACE)interface)->ArbiterHandler == NULL) {

                IopDbgPrint((IOP_ERROR_LEVEL,
                             "!devstack %p returned success for IRP_MN_QUERY_INTERFACE (GUID_ARBITER_INTERFACE_STANDARD) but did not fill in the required data\n",
                             DeviceObject));
                PNP_ASSERT(!NT_SUCCESS(status));
                status = STATUS_UNSUCCESSFUL;
            }
            break;

        case ResourceLegacyDeviceDetection:
            if (((PLEGACY_DEVICE_DETECTION_INTERFACE)interface)->LegacyDeviceDetection == NULL) {

                IopDbgPrint((IOP_ERROR_LEVEL,
                             "!devstack %p returned success for IRP_MN_QUERY_INTERFACE (GUID_LEGACY_DEVICE_DETECTION_STANDARD) but did not fill in the required data\n",
                             DeviceObject));
                PNP_ASSERT(!NT_SUCCESS(status));
                status = STATUS_UNSUCCESSFUL;
            }
            break;

        default:
            //
            // This should never happen.
            //
            IopDbgPrint((IOP_ERROR_LEVEL,
                         "IopQueryResourceHandlerInterface: Possible stack corruption\n"));
            PNP_ASSERT(0);
            status = STATUS_INVALID_PARAMETER;
            break;
        }
    }

    if (NT_SUCCESS(status)) {

        *Interface = interface;
     } else {

         ExFreePool(interface);
     }

    return status;
}

NTSTATUS
IopQueryReconfiguration(
    __in UCHAR Request,
    __in PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine queries the specified DeviceObject for the specified ResourceType
    resource translator.

Parameters:

    HandlerType - specifies Arbiter or Translator

    DeviceObject - Supplies a pointer to the Device object to be queried.

    ResourceType - Specifies the desired type of translator.

    Interface - supplies a variable to receive the desired interface.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/
{
    IO_STACK_LOCATION irpSp;
    NTSTATUS status;
    PDEVICE_NODE deviceNode = (PDEVICE_NODE)DeviceObject->DeviceObjectExtension->DeviceNode;

    PAGED_CODE();

    switch (Request) {
    case IRP_MN_QUERY_STOP_DEVICE:

        if (deviceNode->State != DeviceNodeStarted) {

            IopDbgPrint((   IOP_RESOURCE_ERROR_LEVEL,
                            "An attempt made to send IRP_MN_QUERY_STOP_DEVICE to an unstarted device %wZ!\n",
                            PnpGetDeviceInstancePath(deviceNode)));
            PNP_ASSERT(0);
            return STATUS_UNSUCCESSFUL;
        }
        break;

    case IRP_MN_STOP_DEVICE:
        //
        // Fall through
        //
        if (deviceNode->State != DeviceNodeQueryStopped) {

            IopDbgPrint((   IOP_RESOURCE_ERROR_LEVEL,
                            "An attempt made to send IRP_MN_STOP_DEVICE to an unqueried device %wZ!\n",
                            PnpGetDeviceInstancePath(deviceNode)));
            PNP_ASSERT(0);
            return STATUS_UNSUCCESSFUL;
        }
        break;

    case IRP_MN_CANCEL_STOP_DEVICE:

        if (    deviceNode->State != DeviceNodeQueryStopped &&
                deviceNode->State != DeviceNodeStarted) {

            IopDbgPrint((   IOP_RESOURCE_ERROR_LEVEL,
                            "An attempt made to send IRP_MN_CANCEL_STOP_DEVICE to an unqueried\\unstarted device %wZ!\n",
                            PnpGetDeviceInstancePath(deviceNode)));
            PNP_ASSERT(0);
            return STATUS_UNSUCCESSFUL;
        }
        break;

    default:
        PNP_ASSERT(0);
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Initialize the stack location to pass to IopSynchronousCall()
    //

    RtlZeroMemory(&irpSp, sizeof(IO_STACK_LOCATION));

    //
    // Set the function codes.
    //

    irpSp.MajorFunction = IRP_MJ_PNP;
    irpSp.MinorFunction = Request;

    //
    // Make the call and return.
    //

    status = IopSynchronousCall(DeviceObject,
                                &irpSp,
                                STATUS_NOT_SUPPORTED,
                                0,
                                NULL);
    return status;
}

NTSTATUS
IopQueryLegacyBusInformation (
    __in PDEVICE_OBJECT DeviceObject,
    __out_opt LPGUID InterfaceGuid,
    __out_opt INTERFACE_TYPE *InterfaceType,
    __out_opt ULONG *BusNumber
    )

/*++

Routine Description:

    This routine queries the specified DeviceObject for its legacy bus
    information.

Parameters:

    DeviceObject - The device object to be queried.

    InterfaceGuid = Supplies a pointer to receive the device's interface type
        GUID.

    Interface = Supplies a pointer to receive the device's interface type.

    BusNumber = Supplies a pointer to receive the device's bus number.

Return Value:

    Returns NTSTATUS.

--*/
{
    IO_STACK_LOCATION irpSp;
    NTSTATUS status;
    PLEGACY_BUS_INFORMATION busInfo = NULL;

    PAGED_CODE();

    //
    // Initialize the stack location to pass to IopSynchronousCall()
    //

    RtlZeroMemory(&irpSp, sizeof(IO_STACK_LOCATION));

    //
    // Set the function codes.
    //

    irpSp.MajorFunction = IRP_MJ_PNP;
    irpSp.MinorFunction = IRP_MN_QUERY_LEGACY_BUS_INFORMATION;

    //
    // Make the call and return.
    //

    status = IopSynchronousCall(DeviceObject,
                                &irpSp,
                                STATUS_NOT_SUPPORTED,
                                0,
                                (PULONG_PTR)&busInfo);
    if (NT_SUCCESS(status)) {

        if (busInfo == NULL) {

            //
            // The device driver LIED to us.  Bad, bad, bad device driver.
            //

            PDEVICE_NODE deviceNode;

            deviceNode = DeviceObject->DeviceObjectExtension->DeviceNode;

            if (deviceNode && deviceNode->Parent && deviceNode->Parent->ServiceName.Buffer) {

                DbgPrint("*** IopQueryLegacyBusInformation - Driver %wZ returned STATUS_SUCCESS\n", &deviceNode->Parent->ServiceName);
                DbgPrint("    for IRP_MN_QUERY_LEGACY_BUS_INFORMATION, and a NULL POINTER.\n");
            }

            PNP_ASSERT(busInfo != NULL);

        } else {
            if (ARGUMENT_PRESENT(InterfaceGuid)) {
                *InterfaceGuid = busInfo->BusTypeGuid;
            }
            if (ARGUMENT_PRESENT(InterfaceType)) {
                *InterfaceType = busInfo->LegacyBusType;
            }
            if (ARGUMENT_PRESENT(BusNumber)) {
                *BusNumber = busInfo->BusNumber;
            }
            ExFreePool(busInfo);
        }
    }
    return status;
}

NTSTATUS
IopQueryDeviceState(
    __in PDEVICE_OBJECT DeviceObject,
    __out PPNP_DEVICE_STATE DeviceState
    )

/*++

Routine Description:

    This routine sends query device state irp to the specified device object.

Parameters:

    DeviceObjet - Supplies the device object of the device being queried.

Return Value:

    NTSTATUS code.

--*/

{
    IO_STACK_LOCATION irpSp;
    ULONG_PTR stateValue = 0;
    NTSTATUS status;

    PAGED_CODE();

    //
    // Initialize the stack location to pass to IopSynchronousCall()
    //

    RtlZeroMemory(&irpSp, sizeof(IO_STACK_LOCATION));

    //
    // Set the function codes.
    //

    irpSp.MajorFunction = IRP_MJ_PNP;
    irpSp.MinorFunction = IRP_MN_QUERY_PNP_DEVICE_STATE;

    //
    // Make the call.
    //

    status = IopSynchronousCall(DeviceObject,
                                &irpSp,
                                STATUS_NOT_SUPPORTED,
                                0,
                                &stateValue);

    //
    // Now perform the appropriate action based on the returned state
    //

    if (NT_SUCCESS(status)) {

        *DeviceState = (PNP_DEVICE_STATE)stateValue;
    }

    return status;
}

VOID
IopIncDisableableDepends(
    __inout PDEVICE_NODE DeviceNode
    )
/*++

Routine Description:

    Increments the DisableableDepends field of this devicenode
    and potentially every parent device node up the tree
    A parent devicenode is only incremented if the child in question
    is incremented from 0 to 1

Parameters:

    DeviceNode - Supplies the device node where the depends is to be incremented

Return Value:

    none.

--*/
{

    PAGED_CODE();

    while (DeviceNode != NULL) {

        LONG newval;

        newval = InterlockedIncrement((PLONG)&DeviceNode->DisableableDepends);
        if (newval != 1) {
            //
            // we were already non-disableable, so we don't have to bother
            // the parent.
            //
            break;
        } else {
            //
            // This device was just made not disableable.  Send a notification
            // indicating its status has changed.
            //
            if (PnpGetDeviceInstancePath(DeviceNode)->Buffer) {

                _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                             PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                             NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS);
            }
        }

        DeviceNode = DeviceNode ->Parent;

    }

}

VOID
IopDecDisableableDepends(
    __inout PDEVICE_NODE DeviceNode
    )
/*++

Routine Description:

    Decrements the DisableableDepends field of this devicenode
    and potentially every parent device node up the tree
    A parent devicenode is only decremented if the child in question
    is decremented from 1 to 0

Parameters:

    DeviceNode - Supplies the device node where the depends is to be decremented

Return Value:

    none.

--*/
{

    PAGED_CODE();

    while (DeviceNode != NULL) {

        LONG newval;

        newval = InterlockedDecrement((PLONG)&DeviceNode->DisableableDepends);
        if (newval != 0) {
            //
            // we are still non-disableable, so we don't have to bother the
            // parent.
            //
            break;
        } else {
            //
            // This device was just made disableable.  Send a notification
            // indicating its status has changed.
            //
            if (PnpGetDeviceInstancePath(DeviceNode)->Buffer) {

                _PnpRaiseNtPlugPlayDevicePropertyChangeEvent(PiPnpRtlCtx,
                                                             PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                                             NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_STATUS);
            }
        }

        DeviceNode = DeviceNode ->Parent;

    }

}

NTSTATUS
IopFilterResourceRequirementsCall(
    __in PDEVICE_OBJECT DeviceObject,
    __in_opt PIO_RESOURCE_REQUIREMENTS_LIST ResReqList ,
    __out PVOID *Information
    )

/*++

Routine Description:

    This function sends a synchronous filter resource requirements irp to the
    top level device object which roots on DeviceObject.

Parameters:

    DeviceObject - Supplies the device object of the device being removed.

    ResReqList   - Supplies a pointer to the resource requirements requiring
                   filtering.

    Information  - Supplies a pointer to a variable that receives the returned
                   information of the irp.

Return Value:

    NTSTATUS code.

--*/

{
    KEVENT event;
    PIRP irp;
    PIO_STACK_LOCATION irpSp;
    PULONG_PTR returnInfo;
    NTSTATUS status;
    IO_STATUS_BLOCK statusBlock;
    PDEVICE_OBJECT targetDevice;

    PAGED_CODE();

    returnInfo = (PULONG_PTR)Information;

    //
    // Get a pointer to the topmost device object in the stack of devices,
    // beginning with the deviceObject.
    //

    targetDevice = IoGetAttachedDeviceReference(DeviceObject);

    //
    // Begin by allocating the IRP for this request.  Do not charge quota to
    // the current process for this IRP.
    //

    irp = IoAllocateIrp(targetDevice->StackSize, FALSE);
    if (irp != NULL){
        SPECIALIRP_WATERMARK_IRP(irp, IRP_SYSTEM_RESTRICTED);

        //
        // Initialize it to success. This is a special hack for WDM (ie 9x)
        // compatibility. The driver verifier is in on this one.
        //

        if (ResReqList) {
            irp->IoStatus.Status = statusBlock.Status = STATUS_SUCCESS;
            irp->IoStatus.Information = statusBlock.Information = (ULONG_PTR)ResReqList;

        } else {
            irp->IoStatus.Status = statusBlock.Status = STATUS_NOT_SUPPORTED;
        }

        //
        // Set the pointer to the status block and initialized event.
        //

        KeInitializeEvent(&event,
                          SynchronizationEvent,
                          FALSE);

        irp->UserIosb = &statusBlock;
        irp->UserEvent = &event;

        //
        // Set the address of the current thread
        //

        irp->Tail.Overlay.Thread = PsGetCurrentThread();

        //
        // Queue this irp onto the current thread
        //

        IopQueueThreadIrp(irp);

        //
        // Get a pointer to the stack location of the first driver which will be
        // invoked.  This is where the function codes and parameters are set.
        //

        irpSp = IoGetNextIrpStackLocation(irp);

        //
        // Setup the stack location contents
        //

        irpSp->MinorFunction = IRP_MN_FILTER_RESOURCE_REQUIREMENTS;
        irpSp->MajorFunction = IRP_MJ_PNP;
        irpSp->Parameters.FilterResourceRequirements.IoResourceRequirementList = ResReqList;

        //
        // Call the driver
        //

        status = IoCallDriver(targetDevice, irp);

        PnpIrpStatusTracking(status,
                             IRP_MN_FILTER_RESOURCE_REQUIREMENTS,
                             targetDevice);

        //
        // If a driver returns STATUS_PENDING, we will wait for it to complete
        //

        if (status == STATUS_PENDING) {
            KeWaitForSingleObject(&event,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  (PLARGE_INTEGER)NULL);
            status = statusBlock.Status;
        }

        *returnInfo = (ULONG_PTR)statusBlock.Information;

    } else {
        status = STATUS_INSUFFICIENT_RESOURCES;
    }

    ObDereferenceObject(targetDevice);
    return status;
}

NTSTATUS
IopQueryDockRemovalInterface(
    __in      PDEVICE_OBJECT  DeviceObject,
    __inout  PDOCK_INTERFACE *DockInterface
    )

/*++

Routine Description:

    This routine queries the specified DeviceObject for the dock removal
    interface. We use this interface to send pseudo-remove's. We use this
    to solve the removal orderings problem.

Parameters:

    DeviceObject - Supplies a pointer to the Device object to be queried.

    Interface - supplies a variable to receive the desired interface.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/
{
    IO_STACK_LOCATION irpSp;
    NTSTATUS status;
    PINTERFACE interface;
    USHORT size;
    GUID interfaceType;

    PAGED_CODE();

    size = sizeof(DOCK_INTERFACE);
    interfaceType = GUID_DOCK_INTERFACE;
    interface = (PINTERFACE) ExAllocatePool(PagedPool, size);
    if (interface == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(interface, size);
    interface->Size = size;

    //
    // Initialize the stack location to pass to IopSynchronousCall()
    //

    RtlZeroMemory(&irpSp, sizeof(IO_STACK_LOCATION));

    //
    // Set the function codes.
    //

    irpSp.MajorFunction = IRP_MJ_PNP;
    irpSp.MinorFunction = IRP_MN_QUERY_INTERFACE;

    //
    // Set the pointer to the resource list
    //

    irpSp.Parameters.QueryInterface.InterfaceType = &interfaceType;
    irpSp.Parameters.QueryInterface.Size = interface->Size;
    irpSp.Parameters.QueryInterface.Version = interface->Version = 0;
    irpSp.Parameters.QueryInterface.Interface = interface;
    irpSp.Parameters.QueryInterface.InterfaceSpecificData = NULL;

    //
    // Make the call and return.
    //

    status = IopSynchronousCall(DeviceObject,
                                &irpSp,
                                STATUS_NOT_SUPPORTED,
                                0,
                                NULL);
    if (NT_SUCCESS(status)) {
        *DockInterface = (PDOCK_INTERFACE) interface;
    } else {
        ExFreePool(interface);
    }
    return status;
}

NTSTATUS
PnpQueryDeviceText (
    __in PDEVICE_OBJECT DeviceObject,
    __in DEVICE_TEXT_TYPE DeviceTextType,
    __in LCID POINTER_ALIGNMENT LocaleId,
    __deref_out PWSTR *DeviceText
   )

/*++

Routine Description:

    This routine will issue IRP_MN_QUERY_DEVICE_TEXT to the DeviceObject
    to retrieve its specified device text. If this routine fails,
    DeviceText will be set to NULL.


Arguments:

    DeviceObject - The device object the request should be sent to.

    DeviceTextType - Text type to be queried.

    LocaleId - LCID specifying the locale for the requested text.

    DeviceText - Receives the device text returned by the driver if any.
    Caller is expected to free the storage for DeviceText on success.

Return Value:

    NTSTATUS.

--*/

{
    PWSTR driverText = NULL;
    IO_STACK_LOCATION irpSp;
    NTSTATUS normalizationStatus;
    PWSTR normalizedText;
    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(DeviceTextType == DeviceTextDescription ||
               DeviceTextType == DeviceTextLocationInformation);

    *DeviceText = NULL;

    RtlZeroMemory(&irpSp, sizeof(IO_STACK_LOCATION));

    irpSp.MajorFunction = IRP_MJ_PNP;
    irpSp.MinorFunction = IRP_MN_QUERY_DEVICE_TEXT;

    irpSp.Parameters.QueryDeviceText.DeviceTextType = DeviceTextType;
    irpSp.Parameters.QueryDeviceText.LocaleId = LocaleId;

    status = IopSynchronousCall(DeviceObject,
                                &irpSp,
                                STATUS_NOT_SUPPORTED,
                                0,
                                (PULONG_PTR)&driverText);

    PNP_ASSERT(NT_SUCCESS(status) || (driverText == NULL));

    if (driverText == NULL) {
        status = STATUS_NOT_SUPPORTED;
    }

    if (!NT_SUCCESS(status)) {
        goto QueryDeviceTextEnd;
    }

    //
    // Use the driver-returned string unless the driver provides a string
    // containing a message table location describing the real string.  In this
    // case, normalize the string into a version that allows it to be converted
    // to a localized string later.
    //

    *DeviceText = driverText;
    normalizationStatus = PiNormalizeDeviceText(driverText,
                                                &normalizedText);

    if (NT_SUCCESS(normalizationStatus) && (normalizedText != NULL)) {
        *DeviceText = normalizedText;
        ExFreePool(driverText);
    }

QueryDeviceTextEnd:
    return status;
}

NTSTATUS
PiNormalizeDeviceText (
    __in PWSTR DriverText,
    __deref_out PWSTR *NormalizedText
    )

/*++

Routine Description:

    This routine converts driver-supplied text that points to a localizable
    string in a driver message table to a version that allows consumers to be
    able to locate the message file to construct a localized string.  This
    involves finding the path to the binary containing the message and loading
    the default language message as a fallback.

Arguments:

    DriverText - Supplies the driver-provided text to normalize.

    NormalizedText - Supplies a pointer to a variable that receives the
        normalized string, allocated from pool.  The caller is responsible for
        freeing this memory.

Return Value:

    NT Status value.  This routine returns STATUS_SUCCESS but sets the
    normalized text output parameter to NULL if the input string is not in a
    format that allows it to be normalized (i.e. it is in its final form at
    input time).  Callers must check for both success and a non-NULL output
    parameter before consuming the normalized string.

--*/

{

    PWCHAR ArgumentsString;
    PWCHAR DefaultString;
    UNICODE_STRING DriverPath;
    PWCHAR EndPointer;
    PKEY_BASIC_INFORMATION Information;
    ULONG Length;
    ULONG64 MessageNumber;
    PWCHAR MessageNumberString;
    OBJECT_ATTRIBUTES ObjectAttributes;
    BOOLEAN Prefix;
    UNICODE_STRING RelativeDriverPath;
    size_t RemainingBytes;
    PWCHAR ResultString;
    HANDLE ServiceKey;
    NTSTATUS Status;
    PWCHAR StringCopy;
    SIZE_T TotalStringSize;
    UNICODE_STRING UnicodeString;

    PAGED_CODE();

    *NormalizedText = NULL;
    DefaultString = NULL;
    Information = NULL;
    ResultString = NULL;
    ServiceKey = NULL;
    StringCopy = NULL;
    RtlInitUnicodeString(&DriverPath, NULL);

    //
    // The format of a string pointing to a message entry is:
    //
    // "DriverRegistryPath,MessageId,ReplacementArg1,ReplacementArg2,...
    //
    // The driver registry key is always in the machine portion of the registry.
    // If the string does not begin with this path, it is not a message entry
    // string.
    //

    Status = RtlInitUnicodeStringEx(&UnicodeString, DriverText);
    if (!NT_SUCCESS(Status)) {
        goto NormalizeDeviceTextEnd;
    }

    Prefix = RtlPrefixUnicodeString(&CmRegistryMachineName,
                                    &UnicodeString,
                                    TRUE);

    if (Prefix == FALSE) {
        Status = STATUS_SUCCESS;
        goto NormalizeDeviceTextEnd;
    }

    //
    // Make a copy of the driver string so that it can be modified to parse
    // into its requisite parts.
    //

    StringCopy = ExAllocatePool(PagedPool,
                                UnicodeString.Length + sizeof(UNICODE_NULL));

    if (StringCopy == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto NormalizeDeviceTextEnd;
    }

    RtlCopyMemory(StringCopy, DriverText, UnicodeString.Length);
    StringCopy[UnicodeString.Length / sizeof(WCHAR)] = UNICODE_NULL;

    //
    // After the registry path is a message number, optionally followed by
    // replacement arguments to fill into the localized message string.  While
    // parsing, make sure that the message number is actually a number.
    //

    MessageNumberString = wcschr(StringCopy, ',');
    if (MessageNumberString == NULL) {
        Status = STATUS_SUCCESS;
        goto NormalizeDeviceTextEnd;
    }

    *MessageNumberString = UNICODE_NULL;
    MessageNumberString += 1;
    ArgumentsString = wcschr(MessageNumberString, ',');
    if (ArgumentsString != NULL) {
        *ArgumentsString = UNICODE_NULL;
        ArgumentsString += 1;
    }

    MessageNumber = _wcstoi64(MessageNumberString, &EndPointer, 10);
    if (*EndPointer != UNICODE_NULL) {
        Status = STATUS_SUCCESS;
        goto NormalizeDeviceTextEnd;
    }

    //
    // Querying the driver binary path requires the relative path name of the
    // driver registry key.  Retrieve this.
    //

    RtlInitUnicodeString(&UnicodeString, StringCopy);
    InitializeObjectAttributes(&ObjectAttributes,
                               &UnicodeString,
                               OBJ_CASE_INSENSITIVE  | OBJ_KERNEL_HANDLE,
                               NULL,
                               NULL);

    Status = ZwOpenKey(&ServiceKey, KEY_READ, &ObjectAttributes);
    if (!NT_SUCCESS(Status)) {
        goto NormalizeDeviceTextEnd;
    }

    Status = ZwQueryKey(ServiceKey,
                        KeyBasicInformation,
                        NULL,
                        0,
                        &Length);

    if (Status != STATUS_BUFFER_TOO_SMALL) {
        goto NormalizeDeviceTextEnd;
    }

    Information = ExAllocatePool(PagedPool, Length);
    if (Information == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto NormalizeDeviceTextEnd;
    }

    Status = ZwQueryKey(ServiceKey,
                        KeyBasicInformation,
                        Information,
                        Length,
                        &Length);

    if (!NT_SUCCESS(Status)) {
        goto NormalizeDeviceTextEnd;
    }

    //
    // Retrieve the driver binary path from looking at service key information.
    // This path string begins with SystemRoot, which should be omitted in the
    // binary path ultimately placed into the final string.  Index past this
    // prefix.
    //

    UnicodeString.Length = (USHORT)Information->NameLength;
    UnicodeString.MaximumLength = (USHORT)Information->NameLength;
    UnicodeString.Buffer = Information->Name;
    Status = IopBuildFullDriverPath(&UnicodeString, ServiceKey, &DriverPath);
    if (!NT_SUCCESS(Status)) {
        goto NormalizeDeviceTextEnd;
    }

    PiWstrToUnicodeString(&UnicodeString, L"\\SystemRoot\\");
    Prefix = RtlPrefixUnicodeString(&UnicodeString,
                                    &DriverPath,
                                    TRUE);

    if (Prefix == FALSE) {
        goto NormalizeDeviceTextEnd;
    }

    RelativeDriverPath.Buffer = Add2Ptr(DriverPath.Buffer,
                                        sizeof(L"\\SystemRoot"));

    RelativeDriverPath.Length = DriverPath.Length - sizeof(L"\\SystemRoot");
    RelativeDriverPath.MaximumLength = DriverPath.MaximumLength -
                                       sizeof(L"\\SystemRoot");

    //
    // Extract the default string to be used in situations where fully
    // localized resource parsing is not available.
    //

    Status = PiGetDefaultMessageString(ServiceKey,
                                       (ULONG)MessageNumber,
                                       &DefaultString);

    if (!NT_SUCCESS(Status)) {
        goto NormalizeDeviceTextEnd;
    }

    //
    // Allocate space for the normalized string, which as a format of:
    //
    // "@path\filename.sys,#MessageId;DefaultString;(Arg1,Arg2,Arg3)
    //
    // In addition to the length of the strings, add space for the initial @,
    // the comma and # separating the filename from message ID, the ;
    // separating the message ID from the default string, and the NULL (5 total
    // extra characters).
    //
    // If the optional additional arguments exist, add room for them and the
    // brackets and semi-colon used to delimit them.
    //

    TotalStringSize = RelativeDriverPath.Length +
                      (wcslen(MessageNumberString) * sizeof(WCHAR)) +
                      (wcslen(DefaultString) * sizeof(WCHAR)) +
                      (5 * sizeof(WCHAR));

    if (ArgumentsString != NULL) {
        TotalStringSize += (wcslen(ArgumentsString) + 3) * sizeof(WCHAR);
    }

    ResultString = ExAllocatePool(PagedPool, TotalStringSize);
    if (ResultString == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto NormalizeDeviceTextEnd;
    }

    //
    // Construct the normalized string and return it to the caller.
    //

    Status = RtlStringCbPrintfExW(ResultString,
                                  TotalStringSize,
                                  &EndPointer,
                                  &RemainingBytes,
                                  0,
                                  L"@%s,#%s;%s",
                                  RelativeDriverPath.Buffer,
                                  MessageNumberString,
                                  DefaultString);

    if (!NT_SUCCESS(Status)) {
        goto NormalizeDeviceTextEnd;
    }

    if (ArgumentsString != NULL) {
        Status = RtlStringCbPrintfW(EndPointer,
                                    RemainingBytes,
                                    L";(%s)",
                                    ArgumentsString);

        if (!NT_SUCCESS(Status)) {
            goto NormalizeDeviceTextEnd;
        }
    }

    *NormalizedText = ResultString;
    Status = STATUS_SUCCESS;

NormalizeDeviceTextEnd:
    if (StringCopy != NULL) {
        ExFreePool(StringCopy);
    }

    if (Information != NULL) {
        ExFreePool(Information);
    }

    RtlFreeUnicodeString(&DriverPath);
    if (DefaultString != NULL) {
        ExFreePool(DefaultString);
    }

    if (ServiceKey != NULL) {
        ZwClose(ServiceKey);
    }

    if (!NT_SUCCESS(Status)) {
        if (ResultString != NULL) {
            ExFreePool(ResultString);
        }
    }

    return Status;
}

NTSTATUS
PiGetDefaultMessageString (
    __in HANDLE ServiceKey,
    __in ULONG MessageNumber,
    __deref_out PWSTR *DefaultString
    )

/*++

Routine Description:

    This routine queries the service binary described by the given service key
    for a message string.  It retrieves the default, language neutral version of
    the string for use in scenarios where the corresponding string from the
    correct locale cannot be found.

Arguments:

    ServiceKey - Supplies a handle to the service key for the driver to be
        queried.  This routine will only succeed if this driver is already
        loaded into memory.

    MessageNumber - Supplies the number of the message in the RT_MESSAGE
        resource entry of the service binary to be queried.

    DefaultString - Supplies a pointer to a variable that receives the default
        string read from the service binary.

Return Value:

    NT Status code.

--*/

{

    ANSI_STRING AnsiText;
    PWSTR Copy;
    BOOLEAN Created;
    UNICODE_STRING DriverName;
    PDRIVER_OBJECT DriverObject;
    PMESSAGE_RESOURCE_ENTRY MessageEntry;
    NTSTATUS Status;
    UNICODE_STRING Text;
    PWCHAR TrailingCharacters;

    PAGED_CODE();

    RtlInitUnicodeString(&DriverName, NULL);
    RtlInitUnicodeString(&Text, NULL);

    //
    // Get the driver object for the service, and find the requested message
    // in its resource section.  Get the language neutral string, since it is
    // to be used as default.
    //

    DriverObject = NULL;
    Status = IopGetDriverNameFromKeyNode(ServiceKey, &DriverName);
    if (!NT_SUCCESS(Status)) {
        goto GetDefaultMessageStringEnd;
    }

    DriverObject = IopReferenceDriverObjectByName(&DriverName);
    if (DriverObject == NULL) {
        Status = STATUS_UNSUCCESSFUL;
        goto GetDefaultMessageStringEnd;
    }

    Status = RtlFindMessage(DriverObject->DriverStart,
                            11, // RT_MESSAGETABLE
                            LANG_NEUTRAL,
                            MessageNumber,
                            &MessageEntry);

    if (!NT_SUCCESS(Status)) {
        goto GetDefaultMessageStringEnd;
    }

    //
    // Make a copy of this string, either directly if it is already unicode, or
    // by converting from one byte characters to two byte characters.
    //

    if (MessageEntry->Flags & MESSAGE_RESOURCE_UNICODE) {
        Created = RtlCreateUnicodeString(&Text, (PWCHAR)MessageEntry->Text);
        if (Created == FALSE) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto GetDefaultMessageStringEnd;
        }

    } else {
        RtlInitAnsiString(&AnsiText, (PCHAR)MessageEntry->Text);
        Status = RtlAnsiStringToUnicodeString(&Text, &AnsiText, TRUE);
        if (!NT_SUCCESS(Status)) {
            goto GetDefaultMessageStringEnd;
        }
    }

    //
    // Additionally, strip off the CR/LF that the resource compiler adds to the
    // end of a string.
    //

    if (Text.Length >= (2 * sizeof(WCHAR))) {
        TrailingCharacters =
            (PWCHAR)Add2Ptr(Text.Buffer, Text.Length - (2 * sizeof(WCHAR)));

        if (*TrailingCharacters == L'\r') {
            *TrailingCharacters = UNICODE_NULL;
            Text.Length -= (2 * sizeof(WCHAR));
        }
    }

    //
    // Make a copy of the resulting string so that it can be treated as garden
    // variety pool rather than as part of a unicode string allocation.
    //

    Copy = ExAllocatePool(PagedPool, Text.Length + sizeof(UNICODE_NULL));
    if (Copy == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto GetDefaultMessageStringEnd;
    }

    RtlCopyMemory(Copy, Text.Buffer, Text.Length);
    Copy[Text.Length / sizeof(WCHAR)] = UNICODE_NULL;
    *DefaultString = Copy;
    Status = STATUS_SUCCESS;

GetDefaultMessageStringEnd:
    RtlFreeUnicodeString(&DriverName);
    RtlFreeUnicodeString(&Text);
    if (DriverObject != NULL) {
        ObDereferenceObject(DriverObject);
    }

    return Status;
}

NTSTATUS
PpIrpQueryResourceRequirements(
    __in PDEVICE_OBJECT DeviceObject,
    __out PIO_RESOURCE_REQUIREMENTS_LIST *Requirements
   )

/*++

Routine Description:

    This routine will issue IRP_MN_QUERY_RESOURCE_REQUIREMENTS to the
    DeviceObject to retrieve its resource requirements. If this routine
    failes, Requirements will be set to NULL.

Arguments:

    DeviceObject - The device object the request should be sent to.

    Requirements - Receives the requirements returned by the driver if any.
    The caller is expected to free the storage for Requirements on success.

Return Value:

    NTSTATUS.

--*/

{
    IO_STACK_LOCATION irpSp;
    NTSTATUS status;

    PAGED_CODE();

    *Requirements = NULL;

    RtlZeroMemory(&irpSp, sizeof(IO_STACK_LOCATION));

    irpSp.MajorFunction = IRP_MJ_PNP;
    irpSp.MinorFunction = IRP_MN_QUERY_RESOURCE_REQUIREMENTS;

    status = IopSynchronousCall(DeviceObject,
                                &irpSp,
                                STATUS_NOT_SUPPORTED,
                                0,
                                (PULONG_PTR)Requirements);

    PNP_ASSERT(NT_SUCCESS(status) || (*Requirements == NULL));

    if (NT_SUCCESS(status)) {

        if(*Requirements == NULL) {

            status = STATUS_NOT_SUPPORTED;
        }
    } else {

        *Requirements = NULL;
    }

    return status;
}

#if FAULT_INJECT_INVALID_ID
//
// Fault injection for invalid IDs
//
ULONG PiFailQueryID = 0;
#endif

NTSTATUS
PnpIrpQueryID(
    __in PDEVICE_OBJECT DeviceObject,
    __in BUS_QUERY_ID_TYPE IDType,
    __deref_out PWSTR *ID
    )

/*++

Routine Description:

    This routine will issue IRP_MN_QUERY_ID to the DeviceObject
    to retrieve the specified ID. If this routine fails, ID will
    be set to NULL.

Arguments:

    DeviceObject - The device object the request should be sent to.

    IDType - Type of ID to be queried.

    ID - Receives the ID returned by the driver if any. The caller
    is expected to free the storage for ID on success.

Return Value:

    NTSTATUS.

--*/

{
    IO_STACK_LOCATION irpSp;
    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(IDType == BusQueryDeviceID || IDType == BusQueryInstanceID ||
               IDType == BusQueryHardwareIDs || IDType == BusQueryCompatibleIDs ||
               IDType == BusQueryDeviceSerialNumber || IDType == BusQueryContainerID);

    *ID = NULL;

    RtlZeroMemory(&irpSp, sizeof(IO_STACK_LOCATION));

    irpSp.MajorFunction = IRP_MJ_PNP;
    irpSp.MinorFunction = IRP_MN_QUERY_ID;

    irpSp.Parameters.QueryId.IdType = IDType;

    status = IopSynchronousCall(DeviceObject,
                                &irpSp,
                                STATUS_NOT_SUPPORTED,
                                0,
                                (PULONG_PTR)ID);

    PNP_ASSERT(NT_SUCCESS(status) || (*ID == NULL));

    if (NT_SUCCESS(status)) {

        if(*ID == NULL) {

            status = STATUS_NOT_SUPPORTED;
        }
    } else {

        *ID = NULL;
    }

#if FAULT_INJECT_INVALID_ID
    //
    // Fault injection for invalid IDs
    //
    if (*ID){

        static LARGE_INTEGER seed = {0};

        if(seed.LowPart == 0) {

            KeQuerySystemTime(&seed);
        }

        if(PnPBootDriversInitialized && PiFailQueryID && RtlRandom(&seed.LowPart) % 10 > 7) {

            **ID = L',';
        }
    }
#endif

    return status;
}

NTSTATUS
PpIrpQueryCapabilities(
    __in PDEVICE_OBJECT DeviceObject,
    __out PDEVICE_CAPABILITIES Capabilities
    )

/*++

Routine Description:

    This routine will issue IRP_MN_QUERY_CAPABILITIES to the DeviceObject
    to retrieve the pnp device capabilities.
    Should only be called twice - first from PipProcessNewDeviceNode,
    and second from PnpQueryAndSaveDeviceNodeCapabilities, called after
    device is started.
    If you consider calling this, see if DeviceNode->CapabilityFlags does
    what you need instead (accessed via IopDeviceNodeFlagsToCapabilities(...).

Arguments:

    DeviceObject - The device object the request should be sent to.

    Capabilities - A capabilities structure to be filled in by the driver.

Return Value:

    NTSTATUS.

--*/

{
    IO_STACK_LOCATION irpStack;

    PAGED_CODE();

    RtlZeroMemory(Capabilities, sizeof(DEVICE_CAPABILITIES));
    Capabilities->Size = sizeof(DEVICE_CAPABILITIES);
    Capabilities->Version = 1;
    Capabilities->Address = Capabilities->UINumber = (ULONG)-1;

    RtlZeroMemory(&irpStack, sizeof(IO_STACK_LOCATION));

    irpStack.MajorFunction = IRP_MJ_PNP;
    irpStack.MinorFunction = IRP_MN_QUERY_CAPABILITIES;

    irpStack.Parameters.DeviceCapabilities.Capabilities = Capabilities;

    return IopSynchronousCall(DeviceObject,
                              &irpStack,
                              STATUS_NOT_SUPPORTED,
                              0,
                              NULL);
}

NTSTATUS
PpIrpQueryBusInformation(
    __in PDEVICE_OBJECT DeviceObject,
    __out PPNP_BUS_INFORMATION *BusInfo
    )

/*++

Routine Description:

    This routine queries bus information. If this routine fails, BusInfo
    will be set to NULL.

Parameters:

    DeviceObject - Pointer to the Device object to be queried.

    BusInfo - Receives the bus information returned by the driver if any.
    The caller is expected to free the storage for BusInfo on success.

Return Value:

    NTSTATUS.

--*/
{
    IO_STACK_LOCATION irpSp;
    NTSTATUS status;
    PDEVICE_NODE deviceNode;

    PAGED_CODE();

    *BusInfo = NULL;

    RtlZeroMemory(&irpSp, sizeof(IO_STACK_LOCATION));

    irpSp.MajorFunction = IRP_MJ_PNP;
    irpSp.MinorFunction = IRP_MN_QUERY_BUS_INFORMATION;

    status = IopSynchronousCall(DeviceObject,
                                &irpSp,
                                STATUS_NOT_SUPPORTED,
                                0,
                                (PULONG_PTR)BusInfo);
    if (NT_SUCCESS(status)) {

        if (BusInfo == NULL) {
            //
            // The device driver LIED to us.  Bad, bad, bad device driver.
            //
            deviceNode = DeviceObject->DeviceObjectExtension->DeviceNode;
            if (deviceNode && deviceNode->Parent && deviceNode->Parent->ServiceName.Buffer) {

                DbgPrint("*** IopQueryPnpBusInformation - Driver %wZ returned STATUS_SUCCESS\n", &deviceNode->Parent->ServiceName);
                DbgPrint("    for IRP_MN_QUERY_BUS_INFORMATION, and a NULL POINTER.\n");
            }

            PNP_ASSERT(BusInfo != NULL);
            status = STATUS_NOT_SUPPORTED;
        }
    } else {

        *BusInfo = NULL;
    }

    return status;
}

_Function_class_(IO_COMPLETION_ROUTINE)
_IRQL_requires_same_
NTSTATUS
PnpDiagnosticCompletionRoutine (
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp,
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context
    )

/*++

Routine Description:

    This is the completion routine for IRP_MN_START_DEVICE.

Parameters:

    DeviceObject - device being started.

    Irp - IRP being completed.

    Context - completion context.

Return value:

    NTSTATUS.

--*/

{
    PDEVICE_OBJECT failingDevice;
    PPNP_DIAGNOSTIC_COMPLETION_CONTEXT queryContext;

    UNREFERENCED_PARAMETER(DeviceObject);

    queryContext = (PPNP_DIAGNOSTIC_COMPLETION_CONTEXT)Context;
    PNP_ASSERT(queryContext != NULL);

    //
    // Get the failing driver object and return that in the passed in context.
    //

    queryContext->Status = Irp->IoStatus.Status;
    if (!NT_SUCCESS(queryContext->Status)) {
        failingDevice = IoFindDeviceThatFailedIrp(Irp);
        if (failingDevice) {
            queryContext->DriverObject = failingDevice->DriverObject;
        }
    }

    //
    // Free the IRP since its no longer needed.
    //

    IoFreeIrp(Irp);
    KeSetEvent(&queryContext->Event, IO_NO_INCREMENT, FALSE);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS
PiIrpQueryRemoveDevice (
    __in PDEVICE_OBJECT TargetDevice,
    __in_opt PDRIVER_OBJECT *FailingDriver
    )

/*++

Routine Description:

    This function sends a requested DeviceRemoval related irp to the top level device
    object which roots on TargetDevice.  If there is a VPB associated with the
    TargetDevice, the corresponding filesystem's VDO will be used.  Otherwise
    the irp will be sent directly to the target device/ or its assocated device
    object.

Parameters:

    TargetDevice - Supplies the device object of the device being removed.

    Operation - Specifies the operation requested.
        The following IRP codes are used with IRP_MJ_DEVICE_CHANGE for removing
        devices:
            IRP_MN_QUERY_REMOVE_DEVICE
            IRP_MN_CANCEL_REMOVE_DEVICE
            IRP_MN_REMOVE_DEVICE
            IRP_MN_EJECT
Return Value:

    NTSTATUS code.

--*/
{
    IO_STACK_LOCATION irpSp;
    NTSTATUS status;
    LOGICAL isMountable = FALSE;
    PDEVICE_OBJECT mountedDevice;
    PNP_LOCK_MOUNTABLE_DEVICE_CONTEXT lockContext = {0};
    PNP_DIAGNOSTIC_COMPLETION_CONTEXT queryContext;

    PAGED_CODE();

    //
    // Initialize the stack location to pass to IopSynchronousCall()
    //

    isMountable = FALSE;
    RtlZeroMemory(&irpSp, sizeof(IO_STACK_LOCATION));

    irpSp.MajorFunction = IRP_MJ_PNP;
    irpSp.MinorFunction = (UCHAR)IRP_MN_QUERY_REMOVE_DEVICE;

    //
    // Check to see if there's a VPB anywhere in the device stack.  If there
    // is then we'll have to lock the stack. This is to make sure that the VPB
    // does not go away while the operation is in the file system and that no
    // one new can mount on the device if the FS decides to bail out.
    //
    mountedDevice = PnpFindMountableDevice(TargetDevice);
    if (mountedDevice != NULL) {

        //
        // Mark the VPB to prevent mounts during PNP removal.
        //

        isMountable = TRUE;
        PnpLockMountableDevice(TargetDevice);
        mountedDevice = PnpMarkDeviceForRemove(TargetDevice,
                                               TRUE,
                                               &lockContext);

        PnpUnlockMountableDevice(TargetDevice);

    } else {

        PNP_ASSERTMSG("Mass storage device does not have VPB - this is odd",
                      !((TargetDevice->Type == FILE_DEVICE_DISK) ||
                        (TargetDevice->Type == FILE_DEVICE_CD_ROM) ||
                        (TargetDevice->Type == FILE_DEVICE_TAPE) ||
                        (TargetDevice->Type == FILE_DEVICE_VIRTUAL_DISK)));

        mountedDevice = TargetDevice;
    }

    queryContext.Status = STATUS_UNSUCCESSFUL;
    queryContext.DriverObject = NULL;
    KeInitializeEvent(&queryContext.Event,
                      SynchronizationEvent,
                      FALSE);
    //
    // Make the call and return.
    //
    status = PnpAsynchronousCall(mountedDevice,
                                 &irpSp,
                                 PnpDiagnosticCompletionRoutine,
                                 &queryContext);
    if (status == STATUS_PENDING) {

        KeWaitForSingleObject(&queryContext.Event,
                              Executive,
                              KernelMode,
                              FALSE,
                              (PLARGE_INTEGER)NULL);

        status = queryContext.Status;
    }

    if (FailingDriver) {

        *FailingDriver = queryContext.DriverObject;
    }

    IopDbgPrint((IOP_INFO_LEVEL, "PiIrpQueryRemoveDevice(): Status = %08x\n", status));

    if (isMountable) {

        //
        // Allow FS tear down.
        //

        PnpLockMountableDevice(TargetDevice);

        if (lockContext.FsDevice != NULL) {
            IopDecrementDeviceObjectHandleCount(lockContext.FsDevice);
            ObDereferenceObject(lockContext.FsDevice);
        }

        PnpUnlockMountableDevice(TargetDevice);

        //
        // Successful query should follow up with invalidation of all volumes
        // which have been on this device but which are not currently mounted.
        //

        if (NT_SUCCESS(status)) {
            status = IopInvalidateVolumesForDevice(TargetDevice);
        }
    }

    return status;
}

NTSTATUS
PnpAsynchronousCall (
    __in      PDEVICE_OBJECT           TargetDevice,
    __in      PIO_STACK_LOCATION       TopStackLocation,
    __in_opt  PIO_COMPLETION_ROUTINE   CompletionRoutine,
    __in_opt  PVOID                    CompletionContext
    )

/*++

Routine Description:

    This function sends an  Asynchronous irp to the top level device
    object which roots on DeviceObject.

Parameters:

    DeviceObject - Supplies the device object of the device being removed.

    TopStackLocation - Supplies a pointer to the parameter block for the irp.

    CompletionContext -

    CompletionRoutine -

Return Value:

    NTSTATUS code.

--*/

{
    PDEVICE_NODE deviceNode;
    PDEVICE_OBJECT deviceObject;
    PIRP irp;
    PIO_STACK_LOCATION irpSp;
    NTSTATUS status;

    PAGED_CODE();

    //
    // Get a pointer to the topmost device object in the stack of devices,
    // beginning with the deviceObject.
    //

    deviceObject = IoGetAttachedDeviceReference(TargetDevice);

    //
    // Allocate an I/O Request Packet (IRP) for this device removal operation.
    //

    irp = IoAllocateIrp((CCHAR)(deviceObject->StackSize), FALSE);
    if (irp != NULL) {
        SPECIALIRP_WATERMARK_IRP(irp, IRP_SYSTEM_RESTRICTED);
        deviceNode = PP_DO_TO_DN(TargetDevice);
        if (deviceNode) {
            deviceNode->PendingIrp = irp;
        }

        //
        // Initialize it to failure.
        //

        irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
        irp->IoStatus.Information = 0;

        //
        // Get a pointer to the next stack location in the packet.  This location
        // will be used to pass the function codes and parameters to the first
        // driver.
        //

        irpSp = IoGetNextIrpStackLocation(irp);

        //
        // Fill in the IRP according to this request.
        //

        irp->Tail.Overlay.Thread = PsGetCurrentThread();
        irp->RequestorMode = KernelMode;
        irp->UserIosb = NULL;
        irp->UserEvent = NULL;

        //
        // Copy in the caller-supplied stack location contents
        //

        *irpSp = *TopStackLocation;

        IoSetCompletionRoutine(irp,
                               CompletionRoutine,
                               CompletionContext,  /* Completion context */
                               TRUE,               /* Invoke on success  */
                               TRUE,               /* Invoke on error    */
                               TRUE);              /* Invoke on cancel   */

        status = IoCallDriver(deviceObject, irp);

    } else {
        status = STATUS_INSUFFICIENT_RESOURCES;
    }

    ObDereferenceObject(deviceObject);
    return status;
}

_Function_class_(IO_COMPLETION_ROUTINE)
_IRQL_requires_same_
NTSTATUS
PnpDeviceCompletionRoutine (
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp,
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context
    )

/*++

Routine Description:

    This is the completion routine for IRP_MN_START_DEVICE.

Parameters:

    DeviceObject - device being started.

    Irp - IRP being completed.

    Context - completion context.

Return value:

    NTSTATUS.

--*/

{

    PPNP_DEVICE_COMPLETION_REQUEST request;
    PDEVICE_OBJECT failingDevice;
    PDRIVER_OBJECT failingDriver;

    UNREFERENCED_PARAMETER(DeviceObject);

    failingDriver = NULL;
    request = (PPNP_DEVICE_COMPLETION_REQUEST)Context;
    PNP_ASSERT(request->DeviceNode != NULL);
    PNP_ASSERT(request->DeviceNode->PhysicalDeviceObject != NULL);

    //
    // Update the fields in the entry from the IRP.
    //

    request->DeviceNode->PendingIrp = NULL;
    if (Irp->PendingReturned) {
        request->IrpPended = TRUE;
    }

    request->Information = (PVOID)Irp->IoStatus.Information;
    request->Status = Irp->IoStatus.Status;
    InterlockedIncrement(&request->ReferenceCount);

    //
    // Find the failing driver, if any.
    //

    if (!NT_SUCCESS(request->Status)) {
        

        failingDevice = IoFindDeviceThatFailedIrp(Irp);
        if (failingDevice != NULL) {
            failingDriver = failingDevice->DriverObject;
        }
    }

    //
    // Trace the device operation.
    //

    PnpDeviceCompletionQueueDispatchedEntryCompleted(&PnpDeviceCompletionQueue,
                                                     &request->ListEntry);

    if (request->CompletionState == DeviceNodeEnumerateCompletion) {
        PnpDiagnosticTraceObject(&KMPnPEvt_DeviceEnum_Stop,
                                 PnpGetDeviceInstancePath(request->DeviceNode));

    } else {
        PnpTraceStartDevice(request->DeviceNode,
                            request->Status,
                            failingDriver);
    }

    PnpDeviceCompletionRequestDestroy(request);

    //
    // Free the IRP since its no longer needed.
    //

    IoFreeIrp(Irp);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS
PnpStartDevice (
    __in      PDEVICE_OBJECT            Device,
    __in_opt  PIO_COMPLETION_ROUTINE    CompletionRoutine,
    __in_opt  PVOID                     Context
    )

/*++

Description

    This function sends the IRP_MN_START_DEVICE to start the device.

Parameters

    DeviceNode - Pointer to the device being started

    CompletionRoutine - Start completion routine for asynchronous starts

    CompletionContext - Context for the start completion routine

Return

    NTSTATUS.

--*/

{
    NTSTATUS status;
    IO_STACK_LOCATION irpSp;
    PDEVICE_NODE deviceNode;

    PAGED_CODE();

    PNP_ASSERT(Device);

    deviceNode = PP_DO_TO_DN(Device);

    //
    // Assert that its start dependencies are met.
    //

    PNP_ASSERT(PnpCheckForActiveDependencies(deviceNode,
                                             PNP_DEPENDENCY_TYPE_START) == FALSE);

    //
    // Allow the power manager the opportunity to prepare for this device to
    // start.
    //
    PoFxPrepareDevice(deviceNode, TRUE);

    //
    // Mark the device to be D0 
    //
    deviceNode->CurrentPowerState.DeviceState = PowerDeviceD0;

    //
    // Initialize the stack location for the start IRP.
    //
    PnpInitializeStackLocation(&irpSp, IRP_MN_START_DEVICE);

    //
    // Start parameters contain the allocated resources (both raw and translated).
    //
    irpSp.Parameters.StartDevice.AllocatedResources = deviceNode->ResourceList;
    irpSp.Parameters.StartDevice.AllocatedResourcesTranslated = deviceNode->ResourceListTranslated;

    //
    // Send the actual IRP to the driver stack.
    //
    status = PnpSendIrp(Device,
                        &irpSp,
                        0,
                        CompletionRoutine,
                        Context);

    return status;
}

NTSTATUS
PnpIrpDeviceEnumerated(
    __in PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine will issue IRP_MN_DEVICE_ENUMERATED to the DeviceObject to
    notify the bus driver that this device has now been enumerated by PNP.

Arguments:

    DeviceObject - The device object the request should be sent to.

Return Value:

    NTSTATUS.

--*/

{
    IO_STACK_LOCATION irpStack;

    PAGED_CODE();

    RtlZeroMemory(&irpStack, sizeof(IO_STACK_LOCATION));
    irpStack.MajorFunction = IRP_MJ_PNP;
    irpStack.MinorFunction = IRP_MN_DEVICE_ENUMERATED;
    return IopSynchronousCall(DeviceObject,
                              &irpStack,
                              STATUS_NOT_SUPPORTED,
                              0,
                              NULL);
}
