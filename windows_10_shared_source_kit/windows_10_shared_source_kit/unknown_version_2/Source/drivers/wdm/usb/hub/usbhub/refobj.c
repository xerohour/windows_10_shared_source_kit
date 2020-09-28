/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    refobj.c

     3-10-2002

Abstract:

    Provides general purpose synchronization funtions for referencing
    the hub FDO

    When an object refernces the hub it creates an connection
    (obj->hub) by adding the object to the hubs refernce list.

    Functions associated with the object can then safely accesss the hub
    extension until it is removed from the reference list.

    The HubFdo is not removed until the refernce list is empty


    These functions have built in debug features that allow you
    to determine who/where took a particular reference.  This works
    much better than the traditional inc, dec counter method.


    Thread 1                             Thread 2
        +                                   +
        |                                   |   Create(ObjY)
        |- Create(ObjX)                     | - h = UsbhInitializeSyncObj(ObjX,
        |- Register(ObjX)                   |                             ObjY)
        |                                   |
        |                                   | - UsbhRefSyncObject(h)
        |                                   | - UsbhDerefSyncObject(h)
        |                                   | - UsbhRefSyncObject(h)
        |                                   | - UsbhDerefSyncObject(h)
        |-UsbhWaitForSyncObjs(ObjX) <--+    |
        |-Unregister(ObjX)             |    |
        |-Delete (ObjX)                |    |
        |                              |    |
        |                              |    | - Delete(ObjY)
        |                              +----+ - UsbhFreeSynObj(h)
        +                                   +


In the above example ObjY is connected to ObjX through syncobjs preventing
ObjX from beiing deleted while ObjY has refernces to it.

    SyncObjStates



Module interface:

    UsbhReferenceListAdd -

    UsbhReferenceListRemove -
    UsbhRefSyncObject -
    UsbhDefSyncObject -

    UsbhWaitForSyncObjs
Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/

#include "hubdef.h"
#include "pch.h"

extern KSPIN_LOCK HubSpinLock;

#if 0
KIRQL
UsbhLock(
    VOID
    )
{
    KIRQL irql;

    KeAcquireSpinLock(&HubSpinLock, &irql);

    return irql;
}


VOID
UsbhUnlock(
    KIRQL Irql
    )
{
    KeReleaseSpinLock(&HubSpinLock, Irql);
}
#endif

NTSTATUS
UsbhReferenceListOpen(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Starts the refernce list

    This is called from Add device so that it is available as early as possible

Arguments:


Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);

    InitializeListHead(&hubExt->ObjReferenceList);
    INIT_EVENT_NOT_SIGNALED(&hubExt->HubReferenceEvent);

    hubExt->ReferenceListOpen = TRUE;
    // add ourselves to the list so that there is always
    // one entry
    // we reference the FDO with our FDO, this must the very first call
    // to the reflib when the hub loads
    nts = UsbhReferenceListAdd(HubFdo, HubFdo, hFDO);

    if (NT_ERROR(nts)) {
        hubExt->ReferenceListOpen = FALSE;
    }

    LOG(HubFdo, LOG_HUB, 'rflO', 0, nts);

    return nts;

}


VOID
UsbhReferenceListClose(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Closes the refernce list

Arguments:


Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    HGSL lock;

    lock.Locked = 0;
    LockGSL(&lock);

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_HUB, 'rfcL', hubExt->ReferenceListOpen, 0);
    if (!hubExt->ReferenceListOpen) {
        unLockGSL(&lock);
        return;
    }
    hubExt->ReferenceListOpen = FALSE;
    unLockGSL(&lock);

    // remove the FDO->FDO link
    UsbhReferenceListRemove(HubFdo, HubFdo);

    // wait for list to be empty
    WAIT_EVENT(&hubExt->HubReferenceEvent);

}


NTSTATUS
UsbhReferenceListAdd(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_aliasesMem PVOID Object,
    __in ULONG Tag
    )
/*++

Routine Description:

    Adds an Object to the hubs reference list, an object may only
    occur one time in the list.

    This function creates the link Object -> HubFdo

Arguments:

    Object - the object we want to connect to the hub eg timer, PDO etc
    Tag - tag for this item so we know what object is -- useful for
        debugging.

Return Value:

    nt status

--*/
{
    PHUB_REFERENCE_LIST_ENTRY rfListEntry;
    PDEVICE_EXTENSION_HUB  hubExt;
    HGSL lock;

    lock.Locked = 0;
    LockGSL(&lock);

    hubExt = FdoExt(HubFdo);

    // cannot add to a hub that is not started
    if (!hubExt->ReferenceListOpen) {
        unLockGSL(&lock);
        return STATUS_UNSUCCESSFUL;
    }

/*
#if DBG
    {
    PLIST_ENTRY listEntry;
    // make sure it is not already on the list
    GET_HEAD_LIST(hubExt->ObjReferenceList, listEntry);

    while (listEntry &&
           listEntry != &hubExt->ObjReferenceList) {
        // extract entry
        rfListEntry = (PHUB_REFERENCE_LIST_ENTRY) CONTAINING_RECORD(
                        listEntry,
                        struct _HUB_REFERENCE_LIST_ENTRY,
                        ReferenceLink);

        UsbhAssert(HubFdo, rfListEntry->Sig == SIG_RFLIST);
        UsbhAssert(HubFdo, rfListEntry->Object != Object);

        listEntry = rfListEntry->ReferenceLink.Flink;
    }
    }
#endif //DBG
*/

#ifndef USB_FORCE_MEMORY_STRESS
    UsbhAllocatePool_Z(rfListEntry, NonPagedPool, sizeof(struct _HUB_REFERENCE_LIST_ENTRY));
#else
    rfListEntry = NULL;
#endif

    if (rfListEntry) {
        rfListEntry->Sig = SIG_RFLIST;
        rfListEntry->Tag = Tag;
        rfListEntry->Object = Object;
        rfListEntry->HubFdo = HubFdo;
        InsertTailList(&hubExt->ObjReferenceList, &rfListEntry->ReferenceLink);
    } else {
        hubExt->ObjRefCountNoRes++;        
    }

    unLockGSL(&lock);
    return STATUS_SUCCESS;
}


VOID
UsbhReferenceListRemove(
    PDEVICE_OBJECT HubFdo,
    PVOID Object
    )
/*++

Routine Description:

    Removes a refernce to the hub, if the list becomes empty
    then the refernce event is signaled

Arguments:

    Object - object to remove

Return Value:

    none

--*/
{
    PHUB_REFERENCE_LIST_ENTRY rfListEntry;
    PDEVICE_EXTENSION_HUB  hubExt;
    PLIST_ENTRY listEntry;
    HGSL lock;

    lock.Locked = 0;
    LockGSL(&lock);

    hubExt = FdoExt(HubFdo);

    GET_HEAD_LIST(hubExt->ObjReferenceList, listEntry);

    rfListEntry = NULL;

    while (listEntry &&
           listEntry != &hubExt->ObjReferenceList) {
        // extract entry
        rfListEntry = (PHUB_REFERENCE_LIST_ENTRY) CONTAINING_RECORD(
                        listEntry,
                        struct _HUB_REFERENCE_LIST_ENTRY,
                        ReferenceLink);

        UsbhFatalAssert(HubFdo,rfListEntry != NULL, HubFdo);
        UsbhFatalAssert(HubFdo,rfListEntry->Sig == SIG_RFLIST, HubFdo);

        if (rfListEntry->Object == Object) {
            break;
        }
                
        listEntry = rfListEntry->ReferenceLink.Flink;
        UsbhFatalAssert(HubFdo,listEntry != NULL, HubFdo);
    }

   

    if (rfListEntry && 
        listEntry != &hubExt->ObjReferenceList) {
        UsbhAssert(HubFdo, rfListEntry->Object == Object);

        RemoveEntryList(listEntry);
        UsbhFreePool(rfListEntry);
    } else {
        UsbhAssert(HubFdo, hubExt->ObjRefCountNoRes);
        if (hubExt->ObjRefCountNoRes) {
            hubExt->ObjRefCountNoRes--;
        }
    }

    // if all references are gone signal the refernce event

    if (IsListEmpty(&hubExt->ObjReferenceList) &&
        !hubExt->ObjRefCountNoRes) {
        // signal OK to remove hub
        SET_EVENT(&hubExt->HubReferenceEvent);
        // hubExt may be gone at this point
    }

    unLockGSL(&lock);

}




