/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    pnppower.c

Abstract:

    This file contains the routines that integrate PnP and Power

Author:

    Adrian J. Oney (AdriaO) 01-19-1999

Revision History:

    Modified for nt kernel.

--*/

#include "pnpmgrp.h"

#define PO_ORDER_LEVEL_BOOT_VOLUME (PO_ORDER_PAGABLE)

//
// Internal References
//

PPO_DEVICE_NOTIFY
IopPrepareDeviceNotify (
    __inout PDEVICE_NODE DeviceNode
    );

PWSTR
IopCaptureObjectName (
    __in PVOID    Object
    );

BOOLEAN
IopCheckDeviceFlags (
    __in PPO_DEVICE_NOTIFY Notify,
    __in ULONG Flags
    );

BOOLEAN
IopCheckDeviceType (
    __in PPO_DEVICE_NOTIFY Notify,
    __in ULONG Type
    );

VOID
IopFreePoDeviceNotifyListHead (
    __inout PLIST_ENTRY ListHead
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, IopWarmEjectDevice)
#pragma alloc_text(PAGE, IoDiagTraceDevicesRundown)
#pragma alloc_text(PAGELK, IoBuildPoDeviceNotifyList)
#pragma alloc_text(PAGELK, IoFreePoDeviceNotifyList)
#pragma alloc_text(PAGELK, IopFreePoDeviceNotifyListHead)
#pragma alloc_text(PAGELK, IopCaptureObjectName)
#pragma alloc_text(PAGELK, IopCheckDeviceFlags)
#pragma alloc_text(PAGELK, IopCheckDeviceType)
#pragma alloc_text(PAGELK, IopPrepareDeviceNotify)
#endif

BOOLEAN
IopCheckDeviceFlags (
    __in PPO_DEVICE_NOTIFY Notify,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine checks all device objects in the device stack associated with
    the notify to see if any of them have the specified flags set.

Arguments:

    Notify - Suppolies a pointer to the notify representing a device stack.

    Flags - Supplies the device flags to search for.

Return Value:

    TRUE if at least one of the device objects has at least one of the flags
    set; FALSE otherwise.

--*/

{

    PDEVICE_OBJECT Device;
    BOOLEAN Match;

    Match = FALSE;
    for (Device = Notify->TargetDevice;
         Device != NULL;
         Device = Device->DeviceObjectExtension->AttachedTo) {

        if ((Device->Flags & Flags) != 0) {
            Match = TRUE;
            break;
        }
    }

    return Match;
}

BOOLEAN
IopCheckDeviceType (
    __in PPO_DEVICE_NOTIFY Notify,
    __in ULONG Type
    )

/*++

Routine Description:

    This routine checks all device objects in the device stack associated with
    the notify to see if any of them are a specific device type.

Arguments:

    Notify - Suppolies a pointer to the notify representing a device stack.

    Type - Supplies the device type to search for.

Return Value:

    TRUE if at least one of the device objects matches the desired device type;
    FALSE otherwise.

--*/

{

    PDEVICE_OBJECT Device;
    BOOLEAN Match;

    Match = FALSE;
    for (Device = Notify->TargetDevice;
         Device != NULL;
         Device = Device->DeviceObjectExtension->AttachedTo) {

        if (Device->DeviceType == Type) {
            Match = TRUE;
            break;
        }
    }

    return Match;
}

VOID
IoBuildPoDeviceNotifyList (
    __inout PPO_DEVICE_NOTIFY_ORDER Order
    )
/*++

Routine Description:

    This function allocates all of the necessary notify structures for each
    device and places those notifies on the right order level, taking into
    account the pagability of the device, whether it's a video device, whether
    it's a root-enumerated device, and also parent-child relationships.

Arguments:

    Order - pointer to the order structure to populate

Return Value:

    Returns TRUE if all disks in the system are using boot config resources
    only; FALSE otherwise.

Notes:

    None.

--*/
{

    ULONG                   i;
    PLIST_ENTRY             link;
    PPO_DEVICE_NOTIFY       notify;
    PDEVICE_NODE            node;
    PDEVICE_NODE            parent;
    PPO_RELATION            powerRelation;
    LIST_ENTRY              RebaseList;
    PPO_DEVICE_NOTIFY       tempnotify;
    BOOLEAN                 update;

    PNP_ASSERT(Order->Locked == FALSE);

    InitializeListHead(&RebaseList);

    //
    // Block PnP operations like rebalance.
    //

    PnpLockDeviceActionQueue();
    RtlZeroMemory(Order, sizeof (*Order));
    for (i=0; i <= PO_ORDER_MAXIMUM; i++) {
        InitializeListHead(&Order->OrderLevel[i].WaitSleep);
        InitializeListHead(&Order->OrderLevel[i].ReadySleep);
        InitializeListHead(&Order->OrderLevel[i].ReadyS0);
        InitializeListHead(&Order->OrderLevel[i].WaitS0);
    }

    //
    // ADRIAO 01/12/1999 N.B. -
    //
    // Note that we include devices without the started flag. However, two
    // things prevent us from excluding devices that aren't started:
    // 1) We must be able to send power messages to a device we are warm
    //    undocking.
    // 2) Many devices may not be started, that is no guarentee they are in D3!
    //    For example, they could easily have a boot config, and PNP still
    //    relies heavily on BIOS boot configs to keep us from placing hardware
    //    ontop of other devices with boot configs we haven't found or started
    //    yet!
    //

    FOR_NODE_WALK(node, IopRootDeviceNode) {
        notify = IopPrepareDeviceNotify(node);
        notify->OrderLevel = 0;

        //
        // If any device in this stack is marked pagable, consider the whole
        // stack to be pagable.
        //

        if (IopCheckDeviceFlags(notify, DO_POWER_PAGABLE) != FALSE) {
            notify->OrderLevel |= PO_ORDER_PAGABLE;
        }

        //
        // If the node is root-enumerated, put it on the rebase list so
        // we can mark it and all of its children later.
        //
        // If the node is a leaf node it is ready to receive Sx irps.
        //

        if ((node->Parent == IopRootDeviceNode) &&
            (node->InterfaceType != Internal) &&
            ((node->Flags & DNF_HAL_NODE) == 0)) {

            InsertTailList(&RebaseList, &notify->Link);
        }
    }

    //
    // Rebase anything on the rebase list to be after the normal pnp stuff.
    //

    while (!IsListEmpty(&RebaseList)) {
        link = RemoveHeadList(&RebaseList);
        notify = CONTAINING_RECORD(link, PO_DEVICE_NOTIFY, Link);
        notify->OrderLevel |= PO_ORDER_ROOT_ENUM;

        //
        // Now rebase all of the node's children.
        //

        parent = CONTAINING_RECORD(notify, DEVICE_NODE, Notify);
        FOR_NODE_WALK(node, parent) {
            node->Notify.OrderLevel |= PO_ORDER_ROOT_ENUM;
        }
    }

    //
    // Mark all descendants (both real and power) of pagable devices pagable as
    // well.
    //
    // N.B. The multi-pass approach is used to avoid recursion.
    //

    do {
        update = FALSE;

        //
        // Mark real descendants as pagable if necessary.
        //

        FOR_NODE_WALK(parent, IopRootDeviceNode) {
            if ((parent->Notify.OrderLevel & PO_ORDER_PAGABLE) == 0) {
                continue;
            }

            FOR_NODE_WALK(node, parent) {
                node->Notify.OrderLevel |= PO_ORDER_PAGABLE;
            }
        }

        //
        // Mark power descendants as pagable if necessary.
        //

        FOR_NODE_WALK(parent, IopRootDeviceNode) {
            notify = &parent->Notify;
            if ((notify->OrderLevel & PO_ORDER_PAGABLE) == 0) {
                continue;
            }

            for (link = notify->PowerChildren.Flink;
                 link != &notify->PowerChildren;
                 link = link->Flink) {

                powerRelation = CONTAINING_RECORD(link,
                                                  PO_RELATION,
                                                  ParentLink);

                tempnotify = powerRelation->ChildNotify;
                if ((tempnotify->OrderLevel & PO_ORDER_PAGABLE) == 0) {
                    update = TRUE;
                    tempnotify->OrderLevel |= PO_ORDER_PAGABLE;
                }
            }
        }

    } while (update != FALSE);

    //
    // Move all pagable devices up one level to create a reserved level for the
    // pagable boot volume.
    //

    FOR_NODE_WALK(node, IopRootDeviceNode) { 
        notify = &node->Notify;
        if ((notify->OrderLevel & PO_ORDER_PAGABLE) == 0) {
            continue;
        }

        notify->OrderLevel += 1;
        if ((IopCheckDeviceType(notify, FILE_DEVICE_DISK) != FALSE) &&
            (IopCheckDeviceFlags(notify, DO_SYSTEM_BOOT_PARTITION) !=
             FALSE)) {

            notify->OrderLevel = PO_ORDER_LEVEL_BOOT_VOLUME;
        }
    }

    //
    // Make sure all parents (both real and power) are at an order level <= the
    // chidren.
    //
    // N.B. The multi-pass approach is used to avoid recursion.
    //
    // N.B. A device should never cross the pagable/non-pagable boundary at this
    //      point.
    //

    do {
        update = FALSE;

        //
        // Move real parents to a lower order level if necessary.
        //

        FOR_NODE_WALK(node, IopRootDeviceNode) {
            if (node->Parent == IopRootDeviceNode) {
                continue;
            }
    
            notify = &node->Notify;
            tempnotify = &node->Parent->Notify;
            if (tempnotify->OrderLevel > notify->OrderLevel) {

                PNP_ASSERT((tempnotify->OrderLevel >= DO_POWER_PAGABLE) ==
                           (notify->OrderLevel >= DO_POWER_PAGABLE));

                tempnotify->OrderLevel = notify->OrderLevel;
            }
        }

        //
        // Move power parents to a lower order level if necessary.
        //

        FOR_NODE_WALK(node, IopRootDeviceNode) {
            notify = &node->Notify;
            for (link = notify->PowerParents.Flink;
                 link != &notify->PowerParents;
                 link = link->Flink) {
    
                powerRelation = CONTAINING_RECORD(link, PO_RELATION, ChildLink);
                tempnotify = powerRelation->ParentNotify;
                if (tempnotify->OrderLevel > notify->OrderLevel) {

                    PNP_ASSERT((tempnotify->OrderLevel >= DO_POWER_PAGABLE) ==
                               (notify->OrderLevel >= DO_POWER_PAGABLE));

                    update = TRUE;
                    tempnotify->OrderLevel = notify->OrderLevel;
                }
            }
        }

    } while (update != FALSE);

    //
    // Update the parent, child and device counts and insert each notify into
    // its order level.
    //

    FOR_NODE_WALK(node, IopRootDeviceNode) {
        notify = &node->Notify;
        Order->OrderLevel[notify->OrderLevel].DeviceCount += 1;
        if (node->Parent != IopRootDeviceNode) {
            notify->ParentCount += 1;
            node->Parent->Notify.ChildCount += 1;
        }

        for (link = notify->PowerParents.Flink;
             link != &notify->PowerParents;
             link = link->Flink) {

            notify->ParentCount += 1;
        }

        for (link = notify->PowerChildren.Flink;
             link != &notify->PowerChildren;
             link = link->Flink) {

            notify->ChildCount += 1;
        }

        InsertTailList(&Order->OrderLevel[notify->OrderLevel].WaitSleep,
                       &notify->Link);
    }

    //
    // The engine lock is released when the notify list is freed.
    //

    Order->WarmEjectPdoPointer = &IopWarmEjectPdo;
    Order->Locked = TRUE;
    return;
}

VOID
IopFreePoDeviceNotifyListHead (
    __inout PLIST_ENTRY ListHead
    )

/*++

Routine Description:

    This routine frees all notify structures off of the passed in list.

Arguments:

    ListHead - Supplies the list of notify structures to free.

Return Value:

    None.

--*/

{
    PLIST_ENTRY             Link;
    PPO_DEVICE_NOTIFY       Notify;

    while (!IsListEmpty(ListHead)) {

        Link = RemoveHeadList(ListHead);
        Notify = CONTAINING_RECORD (Link, PO_DEVICE_NOTIFY, Link);

        ObDereferenceObject (Notify->DeviceObject);
        ObDereferenceObject (Notify->TargetDevice);

        if (Notify->DeviceName != NULL) {
            ExFreePoolWithTag(Notify->DeviceName, IOP_DPWR_TAG);
        }

        if (Notify->DriverName != NULL) {
            ExFreePoolWithTag(Notify->DriverName, IOP_DPWR_TAG);
        }
    }
}

VOID
IoFreePoDeviceNotifyList (
    __inout PPO_DEVICE_NOTIFY_ORDER  Order
    )

/*++

Routine Description:

    This routine frees all memory associated with the device notify order.

Arguments:

    Order - Supplies the device notify order to free.

Return Value:

    None.

--*/

{
    ULONG i;

    if (Order->Locked == FALSE) {
        goto FreePoDeviceNotifyListEnd;
    }

    //
    // Free the resources from the notify list
    //

    for (i = 0; i <= PO_ORDER_MAXIMUM; i++) {
        IopFreePoDeviceNotifyListHead(&Order->OrderLevel[i].WaitSleep);
        IopFreePoDeviceNotifyListHead(&Order->OrderLevel[i].ReadySleep);
        IopFreePoDeviceNotifyListHead(&Order->OrderLevel[i].ReadyS0);
        IopFreePoDeviceNotifyListHead(&Order->OrderLevel[i].WaitS0);
    }

    Order->Locked = FALSE;
    PnpUnlockDeviceActionQueue();

FreePoDeviceNotifyListEnd:
    return;
}

PWCHAR
IopCaptureObjectName (
    __in PVOID    Object
    )
{
    NTSTATUS                    Status;
    UCHAR                       Buffer[512];
    POBJECT_NAME_INFORMATION    ObName;
    ULONG                       len;
    PWCHAR                      Name;

    ObName = (POBJECT_NAME_INFORMATION) Buffer;
    Status = ObQueryNameString (
                Object,
                ObName,
                sizeof (Buffer),
                &len
                );

    Name = NULL;
    if (NT_SUCCESS(Status) && ObName->Name.Buffer) {
        Name = ExAllocatePoolWithTag (
                    NonPagedPoolNx,
                    ObName->Name.Length + sizeof(WCHAR),
                    IOP_DPWR_TAG
                    );

        if (Name) {
            RtlCopyMemory(Name, ObName->Name.Buffer, ObName->Name.Length);
            Name[ObName->Name.Length/sizeof(WCHAR)] = UNICODE_NULL;
        }
    }

    return Name;
}

NTSTATUS
IopWarmEjectDevice(
   __in PDEVICE_OBJECT       DeviceToEject,
   __in SYSTEM_POWER_STATE   LightestSleepState
   )
/*++

Routine Description:

    This function is invoked to initiate a warm eject. The eject progresses
    from S1 to the passed in lightest sleep state.

Arguments:

    DeviceToEject       - The device to eject

    LightestSleepState  - The lightest S state (at least S1) that the device
                          may be ejected in. This might be S4 if we are truely
                          low on power.

Return Value:

    NTSTATUS value.

--*/
{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(DeviceToEject);
    UNREFERENCED_PARAMETER(LightestSleepState);

    NT_ASSERT(!"IopWarmEjectDevice - not supported\n");

    return STATUS_NOT_SUPPORTED;

#else

    NTSTATUS       status;

    PAGED_CODE();

    //
    // Acquire the warm eject device lock. A warm eject requires we enter a
    // specific S-state, and two different devices may have conflicting options.
    // Therefore only one is allowed to occur at once.
    //
    // Note that this function is called in the context of a work item, so we
    // don't have to worry about suspend attacks.
    //
    status = KeWaitForSingleObject(
        &IopWarmEjectLock,
        Executive,
        KernelMode,
        FALSE,
        NULL
        );

    PNP_ASSERT(status == STATUS_SUCCESS) ;

    //
    // Acquire engine lock. We are not allowed to set or clear this field
    // unless we are under this lock.
    //
    PpDevNodeLockTree(PPL_TREEOP_ALLOW_READS);

    //
    // Set the current Pdo to eject.
    //
    PNP_ASSERT(IopWarmEjectPdo == NULL);
    IopWarmEjectPdo = DeviceToEject;

    //
    // Release the engine lock.
    //
    PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);

    //
    // Sleep...
    //
    // ADRIAO NOTE 2002/03/31 - Note that this is invoked in the system context,
    //                          not winlogon. This means that the invoking user
    //                          will not have his SeShutdownPrivilege checked.
    //                          However, SeLoadUnloadDriver was checked, which
    //                          itself is synonomous with Admin.
    //
    status = NtInitiatePowerAction(
        PowerActionWarmEject,
        LightestSleepState,
        POWER_ACTION_QUERY_ALLOWED |
        POWER_ACTION_UI_ALLOWED,
        FALSE // Asynchronous == FALSE
        );


    //
    // Tell someone if we didn't succeed.
    //
    // Don't throw UI on every failure because some failures
    // have been handled already (e.g. a specific device veto'ing
    // the action).  Therefore, we'll check some specific error codes.
    //
    if( status == STATUS_PRIVILEGE_NOT_HELD ) {

        //
        // Intentionally ignore the return code here.  We don't
        // want to step on our 'status' variable, and besides, there's
        // not much we could here on failure anyway.
        //
        PnpSetPowerVetoEvent(PowerActionWarmEject,
                             NULL,
                             NULL,
                             DeviceToEject,
                             PNP_VetoInsufficientRights,
                             NULL);

    }


    //
    // Acquire the engine lock. We are not allowed to set or clear this field
    // unless we are under this lock.
    //
    PpDevNodeLockTree(PPL_TREEOP_ALLOW_READS);

    //
    // Clear the current PDO to eject, and see if the Pdo was actually picked
    // up.
    //
    if (IopWarmEjectPdo) {

        if (NT_SUCCESS(status)) {

            //
            // If our device wasn't picked up, the return of
            // NtInitiatePowerAction should *not* be successful!
            //
            PNP_ASSERT(0);
            status = STATUS_UNSUCCESSFUL;
        }

        IopWarmEjectPdo = NULL;
    }

    //
    // Release the engine lock.
    //

    PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);

    //
    // Release the warm eject device lock
    //

    KeSetEvent(&IopWarmEjectLock,
               IO_NO_INCREMENT,
               FALSE);

    return status;

#endif

}

PPO_DEVICE_NOTIFY
IopPrepareDeviceNotify (
    __inout PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This routine allocate and partially initializes a device notify structure
    for the passed-in device node.

Arguments:

    DeviceNode - Supplies the device node for this device notify.

Return Value:

    On success, this routines a valid pointer to a device notify; otherwise, it
    returns NULL.

--*/

{

    PPO_DEVICE_NOTIFY Notify;

    PNP_ASSERT(DeviceNode != NULL);

    Notify = &DeviceNode->Notify;
    Notify->OrderLevel = 0;
    Notify->ChildCount = 0;
    Notify->ActiveChild = 0;
    Notify->ParentCount = 0;
    Notify->ActiveParent = 0;
    Notify->DeviceObject = DeviceNode->PhysicalDeviceObject;
    Notify->TargetDevice = IoGetAttachedDeviceReference(
                            DeviceNode->PhysicalDeviceObject);

    Notify->DriverName = IopCaptureObjectName(
        Notify->TargetDevice->DriverObject);

    Notify->DeviceName = IopCaptureObjectName(Notify->TargetDevice);
    ObReferenceObject(Notify->DeviceObject);
    return Notify;
}

VOID
IoDiagTraceDevicesRundown (
    VOID
    )

/*++

Routine Description:

    This routine traces all the devices in the system and logs rundown information
    about them.

Arguments:

    None

Return Value:

    None

--*/

{

    PDEVICE_NODE node;

    PAGED_CODE();

    //
    // Lock the device action queue to prevent pnp activity
    // when we walk the device node tree.
    //

    IoControlPnpDeviceActionQueue(TRUE);

    FOR_NODE_WALK(node, IopRootDeviceNode) {

        PoDiagTraceDeviceRundown(node,
                                 &node->InstancePath,
                                 &node->ServiceName,
                                 node->CurrentPowerState.DeviceState);

    }

    IoControlPnpDeviceActionQueue(FALSE);
    return;
}
