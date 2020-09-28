/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    relations.c

Abstract:

    This module contains routines to manipulate relations list.  Relation lists
    are used by Plug and Play during the processing of device removal and
    ejection.

    These routines are all pageable and can't be called at raised IRQL or with
    a spinlock held.

Author:

    Robert Nelson (robertn) Apr, 1998.
    Minsang Kim (mikim) Aug, 2013.

Revision History:

--*/

#include "pnpmgrp.h"
#pragma hdrstop

#include "relationsp.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, IopAddRelationToList)
#pragma alloc_text(PAGE, IopAllocateRelationList)
#pragma alloc_text(PAGE, IopInitEnumerationMarker)
#pragma alloc_text(PAGE, IopReverseEnumerationOrder)
#pragma alloc_text(PAGE, IopEnumerateRelations)
#pragma alloc_text(PAGE, IopFreeRelationList)
#pragma alloc_text(PAGE, IopGetRelationsCount)
#pragma alloc_text(PAGE, IopGetRelationsTaggedCount)
#pragma alloc_text(PAGE, IopIsRelationInList)
#pragma alloc_text(PAGE, IopMergeRelationLists)
#pragma alloc_text(PAGE, IopRemoveDeviceRelationsFromList)
#pragma alloc_text(PAGE, IopRemoveRelationFromList)
#pragma alloc_text(PAGE, IopRemoveCurrentRelationFromList)
#pragma alloc_text(PAGE, IopSetAllRelationsTags)
#pragma alloc_text(PAGE, IopSetRelationsTag)
#pragma alloc_text(PAGE, IopIsDescendantNode)
#pragma alloc_text(PAGE, IopCheckIfMergeRequired)
#pragma alloc_text(PAGE, IopRelationListElementAt)
#pragma alloc_text(PAGE, PiQueryPowerRelations)
#pragma alloc_text(PAGE, PiQueryPowerDependencyRelations)
#pragma alloc_text(PAGE, PiValidatePowerRelations)
#pragma alloc_text(PAGE, PnpQueryDeviceRelations)
#pragma alloc_text(PAGE, PiAllocateDeviceObjectList)
#pragma alloc_text(PAGE, PiClearDeviceObjectList)
#pragma alloc_text(PAGE, PipFreeDeviceObjectList)
#pragma alloc_text(PAGE, PipDeviceObjectListElementAt)
#pragma alloc_text(PAGE, IopSortRelationListForRemove)
#pragma alloc_text(PAGE, PipDeviceObjectListAdd)
#pragma alloc_text(PAGE, PipDeviceObjectListRemove)
#pragma alloc_text(PAGE, PipIsDeviceInDeviceObjectList)
#pragma alloc_text(PAGE, PipDeviceObjectListIndexOf)
#pragma alloc_text(PAGE, PipSortDeviceObjectList)
#pragma alloc_text(PAGE, PipVisitDeviceObjectListEntry)
#pragma alloc_text(PAGE, PipGrowDeviceObjectList)
#endif

NTSTATUS
IopAddRelationToList(
    __in PRELATION_LIST List,
    __in PDEVICE_OBJECT DeviceObject,
    __in DEVICE_RELATION_LEVEL RelationLevel,
    __in DEVICE_TAGGED_FOR_REMOVAL Tagged
    )

/*++

Routine Description:

    Adds an element to a relation list.

Arguments:

    List                Relation list to which the DeviceObject is added.

    DeviceObject        DeviceObject to be added to List.  It must be a
                        PhysicalDeviceObject (PDO).

    RelationLevel       Indicates the level of relationship with respect to the
                        target device. This field is updated to a higher vlaue
                        (max enum value) if the device already exists in the list.

    Tagged              Indicates whether DeviceObject should be tagged in List.
                        If the device already exists in the list, then Tagged
                        field will perform an OR operation (prefer TRUE value).

Return Value:

    STATUS_SUCCESS

        The DeviceObject was added successfully.

    STATUS_OBJECT_NAME_COLLISION

        The DeviceObject already exists in the relation list.
        This is not necessarily an error; existing entry is updated
        with new information.

    STATUS_NO_SUCH_DEVICE

        DeviceObject is not a PhysicalDeviceObject (PDO), it doesn't have a
        DEVICE_NODE associated with it.

--*/

{

    PDEVICE_NODE DeviceNode;
    PDEVICE_OBJECT_LIST_ENTRY ListEntry;
    NTSTATUS Status;

    PAGED_CODE();

    DeviceNode = PP_DO_TO_DN(DeviceObject);
    if (DeviceNode == NULL) {

        //
        // DeviceObject is not a PhysicalDeviceObject (PDO).
        //

        Status = STATUS_NO_SUCH_DEVICE;
        goto Exit;
    }

    //
    // Check if the device already exists in the list.
    //

    if (PipIsDeviceInDeviceObjectList(List->DeviceObjectList,
                                      DeviceObject,
                                      &ListEntry) != FALSE) {
                               
        //
        // DeviceObject already exists in the list.  However
        // the relation level may differ.  We will
        // override it with a higher level.  This could
        // happen if we merged two relation lists.
        //

        ListEntry->RelationLevel = max(RelationLevel,
                                       ListEntry->RelationLevel);
                        
        if (Tagged == DEVICE_TAGGED) {
            if (DEVICE_OBJECT_LIST_ENTRY_IS_TAGGED(ListEntry) == FALSE) {

                //
                // Existing entry was not tagged, but now it is tagged.
                // Update the tag count.
                //

                List->DeviceObjectList->TagCount += 1;
            }

            ListEntry->Flags |= DEVICE_OBJECT_LIST_ENTRY_TAGGED;
        }

        Status = STATUS_OBJECT_NAME_COLLISION;
        goto Exit;
    }

    //
    // DeviceObject does not already exist in the list.
    // Add a new entry to the end of the list.
    //

    Status = PipDeviceObjectListAdd(&List->DeviceObjectList,
                                    DeviceObject,
                                    RelationLevel,
                                    Tagged);

    if (NT_SUCCESS(Status)) {
        List->Sorted = FALSE;
        IopDbgPrint((IOP_LOADUNLOAD_INFO_LEVEL,
                     "%wZ added as a relation (RelationLevel: %d)\n",
                     PnpGetDeviceInstancePath(DeviceNode),
                     RelationLevel));
    }

Exit:

    return Status;
}

PRELATION_LIST
IopAllocateRelationList(
    __in PNP_DEVICE_DELETE_TYPE OperationCode
    )

/*++

Routine Description:

    Allocate a new Relations List.

Arguments:

    OperationCode - Type of operation the relation list is being allocated for.

Return Value:

    Newly allocated list if enough PagedPool is available, otherwise NULL.

--*/

{

    PRELATION_LIST RelationList;

    PAGED_CODE();

    RelationList = (PRELATION_LIST) PnpAllocateCriticalMemory(
        OperationCode,
        PagedPool,
        sizeof(*RelationList),
        PNP_POOLTAG_DEVICE_REMOVAL);

    if (RelationList == NULL) {
        goto Exit;
    }

    RelationList->DeviceObjectList = (PDEVICE_OBJECT_LIST) PiAllocateDeviceObjectList(
        OperationCode,
        DEVICE_OBJECT_LIST_INITIAL_SIZE);

    if (RelationList->DeviceObjectList == NULL) {
        IopFreeRelationList(RelationList);
        RelationList = NULL;
        goto Exit;
    }

    RelationList->Sorted = FALSE;

Exit:

    return RelationList;
}

VOID
IopInitEnumerationMarker(
    __in RELATION_LIST_ENUMERATION_MODE Mode,
    __out PRELATION_LIST_MARKER Marker
    )

/*++

Routine Description:

    Initialize marker used for IopEnumerateRelations.
    See definition of RELATION_LIST_ENUMERATION_MODE for comments.

Arguments:

    Mode - Supplies the enumeration mode.

    Marker - Supplies a pointer to receive initialized marker.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PNP_ASSERT(Marker != NULL);
    
    Marker->Mode = Mode;
    Marker->Counter = 0;
}

VOID
IopReverseEnumerationOrder(
    __in PRELATION_LIST List,
    __inout PRELATION_LIST_MARKER Marker
    )

/*++

Routine Description:

    Given a marker from another enumeration, re-initialize the marker so that
    next enumeration will happen in the reverse order. Currently, the old
    enumeration order must be FORWARD.
    
    For example, while enumerating if the caller received A, B, C. Call
    IopReverseEnumerationOrder and re-start (or continue) the enumeration,
    then caller will receive C, B, A.

Arguments:

    List - Supplies a pointer to relation list.

    Marker - Supplies a pointer to enumeration marker.

Return Value:

    NT status code.

--*/

{

    PAGED_CODE();

    //
    // Only forward -> backward flip is supported.
    // Index must be greater than zero (zero means enumeration has not been
    // started).
    //

    if (Marker->Mode != ENUMERATE_FORWARD || Marker->Counter == 0) {
        PNP_ASSERT(FALSE);
        IopInitEnumerationMarker(ENUMERATE_INVALID, Marker);
        goto Exit;
    }

    //
    // Revert to previously enumerated element (remember: marker always points
    // at next element to enumerate).
    //

    Marker->Counter -= 1;

    //
    // Convert to the equivalent index used by backward enumeration (counts
    // from the end).
    //
    
    Marker->Counter = IopGetRelationsCount(List) - 1 - Marker->Counter;

    //
    // Change the mode.
    //

    Marker->Mode = ENUMERATE_BACKWARD;

Exit:

    return;
}

BOOLEAN
IopEnumerateRelations(
    __in PRELATION_LIST List,
    __inout PRELATION_LIST_MARKER Marker,
    __out PDEVICE_OBJECT *DeviceObject,
    __out_opt PDEVICE_RELATION_LEVEL RelationLevel,
    __out_opt PDEVICE_TAGGED_FOR_REMOVAL Tagged
    )

/*++

Routine Description:

    Enumerates the relations in a list.

Arguments:

    List                Relation list to be enumerated.

    Marker              Cookie used to maintain current place in the list.  It
                        must be initialized using IopInitEnumerationMarker.

                        The caller does not need to manually modify the value.
                        The value passed in determines the current enumeration
                        index. The value returned is the index to be used in the
                        next call, given that return value of this function is
                        TRUE.

                        The most significant two bits are used to indicate mode
                        (enumeration order). The rest is a unsigned integer
                        used for counting.

    DeviceObject        Returned device object in the current iteration step.

    RelationLevel       Relation level of the returned DeviceObject to the
                        target device.

    Tagged              If specified then it is set if the relation is tagged
                        otherwise it is cleared.

Return Value:

    TRUE - Returned values are valid, and iteration should continue.

    FALSE - There are no more relations.

--*/

{

    ULONG Index;
    BOOLEAN Valid;
    NTSTATUS Status;

    PAGED_CODE();

    PNP_ASSERT(DeviceObject != NULL);

    *DeviceObject = NULL;
    if (ARGUMENT_PRESENT(RelationLevel)) {
        *RelationLevel = RELATION_LEVEL_REMOVE_EJECT;
    }

    if (ARGUMENT_PRESENT(Tagged)) {
        *Tagged = DEVICE_UNTAGGED;
    }

    if (Marker->Mode != ENUMERATE_ANY_ORDER && List->Sorted == FALSE) {
            
        //
        // The caller specified that the order matters, but the list is not
        // sorted.
        //

        PNP_ASSERT(FALSE);
        Valid = FALSE;
        goto Exit;
    }

    //
    // Check the index if it is within bounds.
    //

    if (Marker->Counter >= IopGetRelationsCount(List)) {
        Valid = FALSE;
        goto Exit;
    }

    switch(Marker->Mode) {
        case ENUMERATE_FORWARD:
        case ENUMERATE_ANY_ORDER:
            Index = Marker->Counter;
            break;

        case ENUMERATE_BACKWARD:
            
            //
            // When iterating backwards, count from the end (last element is 0).
            //

            Index = IopGetRelationsCount(List) - 1 - Marker->Counter;
            break;

        default:
            PNP_ASSERT(FALSE);
            Valid = FALSE;
            goto Exit;
            break;
    }

    //
    // Retrieve properties of the device at the index.
    //

    Status = IopRelationListElementAt(List,
                                      Index,
                                      DeviceObject,
                                      RelationLevel,
                                      Tagged);

    if (NT_SUCCESS(Status)) {
        PNP_ASSERT(DeviceObject != NULL);
        Valid = TRUE;

    } else {
        PNP_ASSERT(FALSE);
        Valid = FALSE;
    }

    //
    // Marker always counts up, regardless of enumeration order.
    //

    Marker->Counter += 1;

Exit:

    return Valid;
}

VOID
IopFreeRelationList(
    __in PRELATION_LIST List
    )

/*++

Routine Description:

    Free a relation list allocated by IopAllocateRelationList.

Arguments:

    List    The list to be freed.

Return Value:

    None.

--*/

{
    
    PAGED_CODE();

    if (List == NULL) {
        PNP_ASSERT(FALSE);
        goto Exit;
    }

    if (List->DeviceObjectList != NULL) {
        PipFreeDeviceObjectList(List->DeviceObjectList);
        List->DeviceObjectList = NULL;
    }

    ExFreePoolWithTag(List, PNP_POOLTAG_DEVICE_REMOVAL);

Exit:

    return;
}

ULONG
IopGetRelationsCount(
    __in PRELATION_LIST List
    )

/*++

Routine Description:

    Returns the total number of relations (Device Objects) in all the entries.

Arguments:

    List    Relation List.

Return Value:

    Count of relations (Device Objects).

--*/

{

    PAGED_CODE();

    PNP_ASSERT(List->DeviceObjectList != NULL);

    return List->DeviceObjectList->Count;
}

ULONG
IopGetRelationsTaggedCount(
    __in PRELATION_LIST List
    )

/*++

Routine Description:

    Returns the total number of relations (Device Objects) in all the entries
    which are tagged.

Arguments:

    List    Relation List.

Return Value:

    Count of tagged relations (Device Objects).

--*/

{

    PAGED_CODE();

    PNP_ASSERT(List != NULL);
    PNP_ASSERT(List->DeviceObjectList != NULL);

    return List->DeviceObjectList->TagCount;
}

BOOLEAN
IopIsRelationInList(
    __in PRELATION_LIST List,
    __in PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    Checks if a relation (Device Object) exists in the specified relation list.

Arguments:

    List            Relation list to check.

    DeviceObject    Relation to be checked.


Return Value:

    TRUE

        Relation exists.

    FALSE

        Relation is not in the list.

--*/

{
    
    PAGED_CODE();

    PNP_ASSERT(List != NULL);
    PNP_ASSERT(DeviceObject != NULL);
    PNP_ASSERT(List->DeviceObjectList != NULL);

    return PipIsDeviceInDeviceObjectList(List->DeviceObjectList,
                                         DeviceObject,
                                         NULL);
}

NTSTATUS
IopMergeRelationLists(
    __inout PRELATION_LIST TargetList,
    __in PRELATION_LIST SourceList,
    __in BOOLEAN MergeTags
    )

/*++

Routine Description:

    Merges two relation lists by copying all the relations from the source list
    to the target list.  Source list remains unchanged.

Arguments:

    TargetList   List to which the relations from Sourcelist are added.

    SourceList   List of relations to be added to TargetList.

    MergeTags    If TRUE, devices marked as tagged in SourceList or TargetList
                 will tagged in the resulting TargetList (union operation).

                 Otherwise, only the devices marked as tagged in TargetList
                 will remain tagged in the resulting TargetList. In other words,
                 any new devices added by SourceList will have their tagged
                 value ignored, but duplicate devices will keep their old
                 tagged value.

Return Value:

    NT status code.

--*/

{

    ULONG Count;
    NTSTATUS FinalStatus;
    BOOLEAN IsDuplicate;
    DEVICE_TAGGED_FOR_REMOVAL Tagged;
    PDEVICE_OBJECT_LIST_ENTRY TargetListEntry;
    ULONG TargetOriginalSize;
    PDEVICE_OBJECT_LIST_ENTRY SourceListEntry;
    ULONG SourceOriginalSize;
    NTSTATUS Status;

    PAGED_CODE();

    PNP_ASSERT(TargetList != NULL);
    PNP_ASSERT(TargetList->DeviceObjectList != NULL);
    PNP_ASSERT(SourceList != NULL);
    PNP_ASSERT(SourceList->DeviceObjectList != NULL);

    FinalStatus = STATUS_SUCCESS;
    TargetOriginalSize = IopGetRelationsCount(TargetList);
    SourceOriginalSize = IopGetRelationsCount(SourceList);

    //
    // Iterate through each device in the source list.
    //

    for (Count = 0; Count < IopGetRelationsCount(SourceList); Count += 1) {
        SourceListEntry = &SourceList->DeviceObjectList->Devices[Count];

        //
        // Find if a duplicate exists in the target list.
        // Only need to search through the original elements (ignore newly
        // merged in elements from the source list).
        //

        IsDuplicate = PipDeviceObjectListIndexOf(
            TargetList->DeviceObjectList,
            SourceListEntry->DeviceObject,
            TargetOriginalSize,
            &TargetListEntry) >= 0;

        if (IsDuplicate != FALSE) {
            PNP_ASSERT(TargetListEntry->DeviceObject == SourceListEntry->DeviceObject);

            //
            // The device already exists in the target list.
            //

            if (MergeTags != FALSE &&
                DEVICE_OBJECT_LIST_ENTRY_IS_TAGGED(SourceListEntry)) {

                //
                // Tag the target device too.
                //

                Tagged = DEVICE_TAGGED;

            } else {

                //
                // Use existing value of the target. Use UNTAGGED here since
                // inserting a duplicate device will do union operation, thus
                // keeping the old value.
                //

                Tagged = DEVICE_UNTAGGED;
            }

        } else {

            //
            // For non-duplicates, bring over the new tag value.
            //

            Tagged = DEVICE_OBJECT_LIST_ENTRY_TAGGED_STATUS(SourceListEntry);
        }

        //
        // In either case, insert the device into the target list.
        // If it was a duplicate, its properties will be merged.
        //

        Status = IopAddRelationToList(TargetList,
                                      SourceListEntry->DeviceObject,
                                      SourceListEntry->RelationLevel,
                                      Tagged);

        //
        // STATUS_OBJECT_NAME_COLLISION is expected if the device already
        // existed in the list.
        //

        if (IsDuplicate != FALSE && Status == STATUS_OBJECT_NAME_COLLISION) {
            Status = STATUS_SUCCESS;
        }

        if (!NT_SUCCESS(Status)) {
            FinalStatus = Status;
            goto Exit;
        }
    }
    
    if (IopGetRelationsCount(TargetList) == 0) {

        //
        // The target list was initially empty. Elements in the source list
        // were copied while preserving order, so there is no need to sort
        // again if the source list was already sorted.
        //

        TargetList->Sorted = SourceList->Sorted;
    }

Exit:

    return FinalStatus;
}

NTSTATUS
IopRemoveDeviceRelationsFromList(
    __in PRELATION_LIST List
    )

/*++

Routine Description:

    Removes all the relations marked as DeviceRelation (eject or removal relations)
    from a relation list.

Arguments:

    List    List from which to remove the relations.


Return Value:

    STATUS_SUCCESS

        The relations were removed successfully.

--*/

{

    PDEVICE_OBJECT DeviceObject;
    LONG Index;
    DEVICE_RELATION_LEVEL RelationLevel;
    NTSTATUS Status;

    PAGED_CODE();

    PNP_ASSERT(List != NULL);
    PNP_ASSERT(List->DeviceObjectList != NULL);

    //
    // Loop through the end to beginning since elements right to the current
    // index may be shifted.
    //

    for (Index = IopGetRelationsCount(List) - 1; Index >= 0; Index -= 1) {
        Status = IopRelationListElementAt(List,
                                          Index,
                                          &DeviceObject,
                                          &RelationLevel,
                                          NULL);

        NT_ASSERT(NT_SUCCESS(Status));

        if (RelationLevel == RELATION_LEVEL_REMOVE_EJECT) {
            Status = PipDeviceObjectListRemove(List->DeviceObjectList,
                                               (ULONG) Index);

            NT_ASSERT(NT_SUCCESS(Status));
        }
    }

    //
    // Removing elements from a sorted list still results in a sorted list.
    //

    return STATUS_SUCCESS;
}

NTSTATUS
IopRemoveRelationFromList(
    __in PRELATION_LIST List,
    __in PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    Removes a relation from a relation list.

Arguments:

    List            List from which to remove the relation.

    DeviceObject    Relation to remove.

Return Value:

    STATUS_SUCCESS

        The relation was removed successfully.

    STATUS_NO_SUCH_DEVICE

        The relation doesn't exist in the list.

--*/

{

    NTSTATUS Status;
    PDEVICE_OBJECT_LIST_ENTRY Device;
    ULONG Index;

    PAGED_CODE();

    PNP_ASSERT(List != NULL);
    PNP_ASSERT(List->DeviceObjectList != NULL);

    Index = PipDeviceObjectListIndexOf(List->DeviceObjectList,
                                       DeviceObject,
                                       IopGetRelationsCount(List),
                                       &Device);

    if (Index == -1) {
        Status = STATUS_NO_SUCH_DEVICE;
        goto Exit;
    }

    Status = PipDeviceObjectListRemove(List->DeviceObjectList, Index);
    NT_ASSERT(NT_SUCCESS(Status));

    //
    // Removing elements from a sorted list still results in a sorted list.
    //

Exit:

    return Status;
}

NTSTATUS
IopRemoveCurrentRelationFromList(
    __in PRELATION_LIST List,
    __in PDEVICE_OBJECT DeviceObject,
    __in PRELATION_LIST_MARKER Marker
    )

/*++

Routine Description:

    Removes a relation from a relation list during an emueration, updating
    the marker to point at the next element in enumeration order.

Arguments:

    List - Supplies a pointer to list from which to remove the relation.

    DeviceObject - Supplies a pointer to device object to remove.

    Marker - Supplies a pointer to enumeration marker to update. The last
        device enumerated will be removed, and not the next device that
        marker currently points at. Only Forward and Any Order enumerations
        are supported.

Return Value:

    NT status code.

--*/

{
    
    PDEVICE_OBJECT DeviceObjectAtIndex;
    ULONG Index;
    NTSTATUS Status;

    PAGED_CODE();

    DeviceObjectAtIndex = NULL;

    if (Marker->Mode != ENUMERATE_FORWARD &&
        Marker->Mode != ENUMERATE_ANY_ORDER) {

        PNP_ASSERT(FALSE);
        Status = STATUS_NOT_SUPPORTED;
        goto Exit;
    }

    //
    // Marker always points at the next device. Here, the previously
    // enumerated device will be removed, so move the marker one step
    // back. For Forward and Any Order enumerations, the array index
    // equals counter value.
    //

    Index = Marker->Counter - 1;

    //
    // Just to be sure, check that the device object matches up.
    //

    Status = PipDeviceObjectListElementAt(List->DeviceObjectList,
                                          Index,
                                          &DeviceObjectAtIndex,
                                          NULL,
                                          NULL);

    NT_ASSERT(NT_SUCCESS(Status));

    if (DeviceObjectAtIndex != DeviceObject) {
        NT_ASSERT(FALSE);
        Status = STATUS_NO_SUCH_DEVICE;
        goto Exit;
    }

    //
    // Remove the device object at the index.
    //

    Status = PipDeviceObjectListRemove(List->DeviceObjectList, Index);

    NT_ASSERT(NT_SUCCESS(Status));

    //
    // Update the marker.
    //

    switch (Marker->Mode) {
        case ENUMERATE_FORWARD:
        case ENUMERATE_ANY_ORDER:

            //
            // An element was deleted at Index, and all elements from Index+1
            // to the last element have been shifted left by one. Therefore,
            // to get the next item, re-use the old array index.
            //

            Marker->Counter = Index;
            break;

        case ENUMERATE_BACKWARD:
        case ENUMERATE_INVALID:
        default:
            PNP_ASSERT(FALSE);
            Status = STATUS_NOT_SUPPORTED;
            break;
    }

Exit:

    return Status;
}

VOID
IopSetAllRelationsTags(
    __in PRELATION_LIST List,
    __in BOOLEAN Tagged
    )

/*++

Routine Description:

    Tags or untags all the relations in a relations list.

Arguments:

    List    Relation list containing relations to be tagged or untagged.

    Tagged  TRUE if the relations should be tagged, FALSE if they are to be
            untagged.

Return Value:

    None.

--*/

{

    ULONG Index;
    PDEVICE_OBJECT_LIST_ENTRY ListEntry;

    PAGED_CODE();

    PNP_ASSERT(List != NULL);
    PNP_ASSERT(List->DeviceObjectList != NULL);

    //
    // For each device in the list.
    //

    for (Index = 0; Index < IopGetRelationsCount(List); Index += 1) {
        ListEntry = &List->DeviceObjectList->Devices[Index];

        //
        // Set or clear the tag based on the argument Tagged.
        //

        if (Tagged) {
            ListEntry->Flags |= DEVICE_OBJECT_LIST_ENTRY_TAGGED;

        } else {
            ListEntry->Flags &= ~DEVICE_OBJECT_LIST_ENTRY_TAGGED;
        }
    }
    
    //
    // If we are setting the tags then update the TagCount to the number of
    // relations in the list.  Otherwise reset it to zero.
    //

    List->DeviceObjectList->TagCount = Tagged ? IopGetRelationsCount(List) : 0;
}

NTSTATUS
IopSetRelationsTag(
    __in PRELATION_LIST List,
    __in PDEVICE_OBJECT DeviceObject,
    __in DEVICE_TAGGED_FOR_REMOVAL Tagged
    )

/*++

Routine Description:

    Sets or clears a tag on a specified relation in a relations list.  This
    routine is also used by some callers to determine if a relation exists in
    a list and if so to set the tag.

Arguments:

    List            List containing relation to be tagged or untagged.

    DeviceObject    Relation to be tagged or untagged.

    Tagged          Supplies value for determining if the device should be
                    tagged.

Return Value:

    STATUS_SUCCESS

        The relation was tagged successfully.

    STATUS_NO_SUCH_DEVICE

        The relation doesn't exist in the list.

--*/

{
    
    PDEVICE_OBJECT_LIST_ENTRY Device;
    NTSTATUS Status;

    PAGED_CODE();

    PNP_ASSERT(List != NULL);
    PNP_ASSERT(List->DeviceObjectList != NULL);

    if (PipIsDeviceInDeviceObjectList(List->DeviceObjectList,
                                      DeviceObject,
                                      &Device)) {

        //
        // We found a match
        //

        if (DEVICE_OBJECT_LIST_ENTRY_IS_TAGGED(Device)) {

            //
            // The relation is already tagged so to simplify the logic below
            // decrement the TagCount.  We'll increment it later if the caller
            // still wants it to be tagged.
            //

            List->DeviceObjectList->TagCount -= 1;
        }

        if (Tagged == DEVICE_TAGGED) {

            //
            // Set the tag and increment the number of tagged relations.
            //

            Device->Flags |= DEVICE_OBJECT_LIST_ENTRY_TAGGED;
            List->DeviceObjectList->TagCount += 1;

        } else {

            //
            // Clear the tag.
            //

            Device->Flags &= ~DEVICE_OBJECT_LIST_ENTRY_TAGGED;
        }

        Status = STATUS_SUCCESS;

    } else {
        Status = STATUS_NO_SUCH_DEVICE;
    }

    return Status;
}

BOOLEAN
IopIsDescendantNode (
    __in PRELATION_LIST List,
    __in PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    Determines if the device object is a descendant node in the context
    of the relation list. A node in a relation list is a descendant node
    if and only if its parent is in the same relation list.

    Identifying descendant nodes is useful for determining which device
    nodes to remove or delete for surprise fail scenarios.

Arguments:

    List - Supplies a pointer to the relation list for checking for
        descendant node.

    DeviceObject - Supplies a pointer to the device to check if it is
        a descendant node. It must be in the relation list.

Return Value:

    TRUE if the node is a descendant, FALSE otherwise.
    
--*/

{
    PDEVICE_NODE DeviceNode;
    PDEVICE_NODE ParentNode;
    
    PAGED_CODE();

    PNP_ASSERT(List != NULL);
    PNP_ASSERT(DeviceObject != NULL);
    
    //
    // The device object must be in the relation list.
    //

    PNP_ASSERT(IopIsRelationInList(List, DeviceObject) != FALSE);

    //
    // Determine the parent.
    //

    DeviceNode = PP_DO_TO_DN(DeviceObject);
    PNP_ASSERT(DeviceNode != NULL);
    if (DeviceNode->Parent == NULL) {
        ParentNode = PNP_DN_PREVIOUS_PARENT(DeviceNode);

    } else {
        ParentNode = DeviceNode->Parent;
    }

    PNP_ASSERT(ParentNode != NULL);

    //
    // Check if the parent node is also in the list.
    // If it is, then the device is a descendant.
    //

    return IopIsRelationInList(List, ParentNode->PhysicalDeviceObject);
}

_Use_decl_annotations_
BOOLEAN
IopCheckIfMergeRequired (
    PRELATION_LIST PendingList,
    PRELATION_LIST NewList
    )

/*++

Routine Description:

    Check if a merge is required between two pending removal relation lists.
    A merge is required if any device in the pending list has a parent,
    previous parent, or a provider in the new list, or if there is an overlap.

Arguments:

    PendingList - Supplies a relation list that is currently in the surprise
        remove queue; the "target" of the merge.

    NewList - Supplies a relation list that is being checked for merge
        candidate; the "source" of the merge.

Return Value:

    TRUE if a merge is required; FALSE otherwise.

--*/

{

    ULONG DependencyTypes;
    PDEVICE_NODE DeviceNode;
    PDEVICE_OBJECT DeviceObject;
    PLIST_ENTRY ListEntry;
    PLIST_ENTRY ListHead;
    PDEVICE_NODE ParentNode;
    PDEVICE_OBJECT ProviderDevice;
    RELATION_LIST_MARKER Marker;

    PAGED_CODE();

    PNP_ASSERT(PendingList != NULL);

    PNP_ASSERT(NewList != NULL);

    IopInitEnumerationMarker(ENUMERATE_ANY_ORDER, &Marker);
    while (IopEnumerateRelations(PendingList,
                                 &Marker,
                                 &DeviceObject,
                                 NULL,
                                 NULL)) {

        DeviceNode = PP_DO_TO_DN(DeviceObject);
        PNP_ASSERT(DeviceNode != NULL);

        //
        // Check if the device itself is in the new list.
        //

        if (IopIsRelationInList(NewList, DeviceObject) != FALSE) {
            return TRUE;
        }

        //
        // Get the parent.
        //

        if (DeviceNode->Parent == NULL) {
            ParentNode = PNP_DN_PREVIOUS_PARENT(DeviceNode);

        } else {
            ParentNode = DeviceNode->Parent;
        }

        PNP_ASSERT(ParentNode != NULL);

        //
        // Check if the parent is part of the new list.
        //

        if (IopIsRelationInList(NewList,
                                ParentNode->PhysicalDeviceObject) != FALSE) {

            return TRUE;
        }

        //
        // Check all providers if any of them are in the new list.
        //

        ListHead = PiGetProviderList(DeviceNode->PhysicalDeviceObject);
        ListEntry = ListHead->Flink;
        while (ListEntry != ListHead) {
            PiEnumerateProviderListEntry(ListEntry,
                                         &ProviderDevice,
                                         &DependencyTypes);

            PNP_ASSERT((DependencyTypes &
                        ~(PNP_DEPENDENCY_TYPE_INITIALIZATION |
                          PNP_DEPENDENCY_TYPE_START)) == 0);

            ListEntry = ListEntry->Flink;
            if (ProviderDevice == NULL) {
                continue;
            }

            if (IopIsRelationInList(NewList, ProviderDevice) != FALSE) {

                //
                // A provider of this device exists in the new list.
                //

                return TRUE;
            }
        }
    }

    return FALSE;
}

NTSTATUS
IopRelationListElementAt (
    __in PRELATION_LIST List,
    __in ULONG Index,
    __out PDEVICE_OBJECT *DeviceObject,
    __out_opt PDEVICE_RELATION_LEVEL RelationLevel,
    __out_opt PDEVICE_TAGGED_FOR_REMOVAL Tagged
    )
{

    PAGED_CODE();

    PNP_ASSERT(List != NULL);
    PNP_ASSERT(List->DeviceObjectList != NULL);
    PNP_ASSERT(DeviceObject != NULL);

    return PipDeviceObjectListElementAt(List->DeviceObjectList,
                                        Index,
                                        DeviceObject,
                                        RelationLevel,
                                        Tagged);
}

NTSTATUS
PiQueryPowerRelations (
    __in PDEVICE_NODE DevNode,
    __in BOOLEAN InternalRelations
    )

/*++

Routine Description:

    This routine queries for new power relations for the device.

Arguments:

    DevNode - Supplies a pointer to the device node of the device stack whose
        power relations will be updated.

    InternalRelations - Supplies a value indicating whether the power relations
        are being built due to a change to internally-tracked relations.

Return Value:

    Standard NTSTATUS values.

--*/

{

    PDEVICE_RELATIONS DeviceRelations;
    ULONG Index;
    NTSTATUS InternalStatus;
    PLIST_ENTRY Link;
    PPO_DEVICE_NOTIFY Notify;
    PPO_RELATION PowerRelation;
    NTSTATUS Status;
    PDEVICE_NODE VirtualParent;

    PAGED_CODE();

    PNP_ASSERT(DevNode != NULL);

    Status = STATUS_SUCCESS;
    DeviceRelations = NULL;
    if (InternalRelations == FALSE) {
        Status = PnpQueryDeviceRelations(DevNode->PhysicalDeviceObject,
                                         PowerRelations,
                                         NULL,
                                         &DeviceRelations);

        if (!NT_SUCCESS(Status)) {
            DeviceRelations = NULL;
            goto QueryPowerRelationsEnd;
        }

        //
        // Remove any existing power relations from this device.
        //

        Notify = &DevNode->Notify;
        Link = Notify->PowerParents.Flink;
        while (Link != &Notify->PowerParents) {
            PowerRelation = CONTAINING_RECORD(Link, PO_RELATION, ChildLink);
            Link = Link->Flink;
            RemoveEntryList(&PowerRelation->ChildLink);
            RemoveEntryList(&PowerRelation->ParentLink);
            ExFreePoolWithTag(PowerRelation, IOP_DPWR_TAG);
        }

        if ((DeviceRelations == NULL) || (DeviceRelations->Count == 0)) {
            Status = STATUS_SUCCESS;
            goto QueryPowerRelationsEnd;
        }

        //
        // Build power relations for each virtual parent returned by the query.
        //

        for (Index = 0; Index < DeviceRelations->Count; Index += 1) {

            ASSERT_PDO(DeviceRelations->Objects[Index]);

            VirtualParent = PP_DO_TO_DN(DeviceRelations->Objects[Index]);

            //
            // Ignore devices that have been unlinked from the tree.
            // Power relations are unlinked when the device is unlinked from
            // the device tree, so past that point the device should not
            // participate in power relations.
            //

            if (VirtualParent->Parent == NULL) {
                continue;
            }

            PowerRelation = ExAllocatePoolWithTag(NonPagedPoolNx,
                                                  sizeof(PO_RELATION),
                                                  IOP_DPWR_TAG);

            if (PowerRelation == NULL) {
                Status = STATUS_INSUFFICIENT_RESOURCES;
                goto QueryPowerRelationsEnd;
            }

            PowerRelation->InternallyReported = FALSE;
            PowerRelation->ChildNotify = Notify;
            PowerRelation->ParentNotify = &VirtualParent->Notify;
            InsertTailList(&Notify->PowerParents, &PowerRelation->ChildLink);
            InsertTailList(&VirtualParent->Notify.PowerChildren,
                           &PowerRelation->ParentLink);
        }
    }

    //
    // Add any internal power relationships due to PnP dependencies.
    //

QueryPowerRelationsEnd:
    InternalStatus = PiQueryPowerDependencyRelations(DevNode);
    if (NT_SUCCESS(Status) && !NT_SUCCESS(InternalStatus)) {
        Status = InternalStatus;
    }

    PiValidatePowerRelations(DevNode);

    //
    // Dereference each device object returned form the query and free the
    // buffer.
    //

    if (DeviceRelations != NULL) {
        for (Index = 0; Index < DeviceRelations->Count; Index += 1) {
            ObDereferenceObject(DeviceRelations->Objects[Index]);
        }

        ExFreePool(DeviceRelations);
    }

    //
    // Notify Object Change system that the Power relations have changed.
    // This is only necessary if the device has been initialized to the
    // point where change notifications make sense.
    //

    PpDevNodeLockTree(PPL_SIMPLE_READ);
    if (DevNode->State != DeviceNodeUnspecified &&
        DevNode->State != DeviceNodeUninitialized &&
        DevNode->State != DeviceNodeDeletePendingCloses &&
        DevNode->State != DeviceNodeDeleted) {

        PiPnpRtlPdoRaiseNtPlugPlayPropertyChangeEvent(DevNode->PhysicalDeviceObject, 
                                                      NT_PLUGPLAY_MAPPED_DEVICE_PROPERTY_POWER_RELATIONS);
    }

    PpDevNodeUnlockTree(PPL_SIMPLE_READ);
    return Status;
}

NTSTATUS
PiQueryPowerDependencyRelations(
    __in PDEVICE_NODE DevNode
    )
    
/*++
    
    Routine Description:
    
        This routine appends power relations for the device based on internal
        PnP dependencies.
    
    Arguments:
    
        Devnode - Supplies a pointer to the device node of the device stack whose
            power relations will be updated.
    
    Return Value:
    
        Standard NTSTATUS values.
    
    --*/
    
{

    PLIST_ENTRY DependencyProviderListHead;
    PLIST_ENTRY Link;
    PLIST_ENTRY NextProvider;
    PPO_DEVICE_NOTIFY Notify;
    PPO_RELATION PowerRelation;
    PDEVICE_OBJECT ProviderDevice;
    NTSTATUS Status;
    PDEVICE_NODE VirtualParent;

    PAGED_CODE();

    PNP_ASSERT(DevNode != NULL);

    //
    // Remove any internal power relations that are no longer valid.
    //

    PnpAcquireDependencyRelationsLock(FALSE);
    DependencyProviderListHead = PiGetProviderList(DevNode->PhysicalDeviceObject);
    Notify = &DevNode->Notify;
    Link = Notify->PowerParents.Flink;
    Status = STATUS_SUCCESS;
    while (Link != &Notify->PowerParents) {
        PowerRelation = CONTAINING_RECORD(Link, PO_RELATION, ChildLink);
        Link = Link->Flink;
        if (PowerRelation->InternallyReported == FALSE) {
            continue;
        }

        NextProvider = DependencyProviderListHead->Flink;
        while (NextProvider != DependencyProviderListHead) {
            PiEnumerateProviderListEntry(NextProvider,
                                         &ProviderDevice,
                                         NULL);

            if (ProviderDevice != NULL) {
                VirtualParent = PP_DO_TO_DN(ProviderDevice);
                if (VirtualParent != NULL &&
                    PowerRelation->ParentNotify == &VirtualParent->Notify) {

                    break;
                }
            }

            NextProvider = NextProvider->Flink;
        }

        if (NextProvider == DependencyProviderListHead) {
            RemoveEntryList(&PowerRelation->ChildLink);
            RemoveEntryList(&PowerRelation->ParentLink);
            ExFreePoolWithTag(PowerRelation, IOP_DPWR_TAG);
        }
    }

    //
    // Add new internal power relations.
    //

    NextProvider = DependencyProviderListHead->Flink;
    while (NextProvider != DependencyProviderListHead) {
        PiEnumerateProviderListEntry(NextProvider,
                                     &ProviderDevice,
                                     NULL);

        NextProvider = NextProvider->Flink;
        if (ProviderDevice == NULL) {
            continue;
        }

        VirtualParent = PP_DO_TO_DN(ProviderDevice);

        //
        // If the virtual parent is not in the device tree, it is too late;
        // the device is being removed and can no longer participate in power
        // relations.
        //

        if (VirtualParent == NULL || VirtualParent->Parent == NULL) {
            continue;
        }

        Link = Notify->PowerParents.Flink;
        while (Link != &Notify->PowerParents) {
            PowerRelation = CONTAINING_RECORD(Link, PO_RELATION, ChildLink);
            if (PowerRelation->ParentNotify == &VirtualParent->Notify) {
                break;
            }

            Link = Link->Flink;            
        }

        if (Link != &Notify->PowerParents) {

            //
            // This power dependency was already reported.
            //

            continue;
        }

        PowerRelation = (PPO_RELATION) ExAllocatePoolWithTag(NonPagedPoolNx,
                                                             sizeof(PO_RELATION),
                                                             IOP_DPWR_TAG);
    
        if (PowerRelation == NULL) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto QueryPowerDependencyRelationsEnd;
        }

        PowerRelation->InternallyReported = TRUE;
        PowerRelation->ChildNotify = Notify;
        PowerRelation->ParentNotify = &VirtualParent->Notify;
        InsertTailList(&Notify->PowerParents, &PowerRelation->ChildLink);
        InsertTailList(&VirtualParent->Notify.PowerChildren,
                       &PowerRelation->ParentLink);
    }

QueryPowerDependencyRelationsEnd:
    PnpReleaseDependencyRelationsLock();
    return Status;
}

VOID
PiValidatePowerRelations (
    __inout PDEVICE_NODE DevNode
    )

/*++

Routine Description:

    This routine validates a new set of power relations to make sure no cyclic
    dependencies are introduced that would deadlock a power transition.

    N.B. This routine bugchecks if a cycle is detected.

Arguments:

    DevNode - Supplies a pointer to the device node that has reported new power
        relations.

Return Value:

    None

--*/

{

    LIST_ENTRY Ancestors;
    PDEVICE_NODE Candidate;
    PLIST_ENTRY Link;
    PPO_RELATION PowerRelation;
    PDEVICE_NODE Temp;

    PAGED_CODE();

    //
    // Seed the virtual ancestors list with the device's virtual parents.
    //

    InitializeListHead(&Ancestors);
    while (IsListEmpty(&DevNode->Notify.PowerParents) == FALSE) {
        InsertTailList(&Ancestors,
                       RemoveHeadList(&DevNode->Notify.PowerParents));
    }

    //
    // Build up the entire virtual ancestry of devices. For each device, check
    // the physical ancestry to see if it eventually points back to the device
    // originating this power request (which would result in a deadlock during a
    // sleep transition).
    //

    for (Link = Ancestors.Flink; Link != &Ancestors; Link = Link->Flink) {
        PowerRelation = CONTAINING_RECORD(Link, PO_RELATION, ChildLink);
        Candidate = CONTAINING_RECORD(PowerRelation->ParentNotify,
                                      DEVICE_NODE,
                                      Notify);

        while (IsListEmpty(&Candidate->Notify.PowerParents) == FALSE) {
            InsertTailList(&Ancestors,
                           RemoveHeadList(&Candidate->Notify.PowerParents));
        }

        //
        // Make sure this candidate device node is not a descendant of the child
        // device with new power relations.
        //

        for (Temp = Candidate; Temp != NULL; Temp = Temp->Parent) {
            if (Temp == DevNode) {
                KeBugCheckEx(PNP_DETECTED_FATAL_ERROR,
                             0xc,
                             (ULONG_PTR)DevNode->PhysicalDeviceObject,
                             (ULONG_PTR)Candidate->PhysicalDeviceObject,
                             0);
            }
        }
    }

    //
    // Move all the power relations back to their original locations.
    //

    while (IsListEmpty(&Ancestors) == FALSE) {
        Link = RemoveHeadList(&Ancestors);
        PowerRelation = CONTAINING_RECORD(Link, PO_RELATION, ChildLink);
        InsertTailList(&PowerRelation->ChildNotify->PowerParents,
                       &PowerRelation->ChildLink);
    }

    return;
}

NTSTATUS
PnpQueryDeviceRelations (
    __in PDEVICE_OBJECT             Device,
    __in DEVICE_RELATION_TYPE       RelationType,
    __in_opt PIO_COMPLETION_ROUTINE CompletionRoutine,
    __in_opt PVOID                  Context
    )

/*++

Description

    This function sends the IRP_MN_QUERY_DEVICE_RELATIONS to query the device relations.

Parameters

    Device - Pointer to the device whose relations are to be queried..

    RelationType - Type of relation being queried.

    CompletionRoutine - Pointer to the completion routine for asynchronous operation.

    Context - Completion routine context.

Return

    NTSTATUS.

--*/

{
    IO_STACK_LOCATION irpSp;
    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(Device);

    //
    // Initialize the stack location for the query device relations IRP.
    //
    PnpInitializeStackLocation(&irpSp, IRP_MN_QUERY_DEVICE_RELATIONS);

    //
    // Set the type of relation being queried.
    //
    irpSp.Parameters.QueryDeviceRelations.Type = RelationType;

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


PDEVICE_OBJECT_LIST
PiAllocateDeviceObjectList (
    __in PNP_DEVICE_DELETE_TYPE OperationCode,
    __in ULONG MaxCount
    )

/*++

Routine Description:

    Allocate memory for DEVICE_OBJECT_LIST.

Arguments:

    OperationCode - Supplies the device deletion type code.

    MaxCount - Supplies the maximum number of device objects that the list
        can hold.

Return Value:

    Pointer to allocated DEVICE_OBJECT_LIST structure.
    NULL if memory failed to allocate.

--*/

{

    PDEVICE_OBJECT_LIST list;
    SIZE_T listSize;

    PAGED_CODE();
    
    listSize = RTL_SIZEOF_THROUGH_FIELD(DEVICE_OBJECT_LIST,
                                        Devices[MaxCount-1]);

    list = (PDEVICE_OBJECT_LIST) PnpAllocateCriticalMemory(
        OperationCode,
        PagedPool,
        listSize,
        PNP_POOLTAG_DEVICE_REMOVAL);
    
    if (list != NULL) {
        list->Count = 0;
        list->TagCount = 0;
        list->OperationCode = OperationCode;
        list->MaxCount = MaxCount;
        PiClearDeviceObjectList(list);
    }

    return list;
}

VOID
PiClearDeviceObjectList (
    __inout PDEVICE_OBJECT_LIST List
    )

/*++

Routine Description:

    Clear the list of device objects, dereferencing exising elements.

Arguments:

    List - List of device objects to be cleared.

Return Value:

    None.

--*/

{

    ULONG index;

    PAGED_CODE();

    PNP_ASSERT(List != NULL);
    PNP_ASSERT(List->MaxCount > 0);

    //
    // Dereference existing device objects in the list.
    //

    for (index = 0; index < List->Count; index++) {
        ObDereferenceObject(List->Devices[index].DeviceObject);
    }

    //
    // Reset all fields, except MaxCount.
    //

    RtlZeroMemory(&List->Devices,
                  sizeof(DEVICE_OBJECT_LIST_ENTRY) * List->MaxCount);

    List->Count = 0;
    List->TagCount = 0;
}

VOID
PipFreeDeviceObjectList (
    __in PDEVICE_OBJECT_LIST List
    )

/*++

Routine Description:

    Free a device node allocated by PiAllocateDeviceObjectList.
    Dereferenes all device objects in the list.

Arguments:

    List - Supplies a pointer to the list to be freed.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    //
    // Call clear to dereference objects.
    //

    PiClearDeviceObjectList(List);
    ExFreePool(List);
}

NTSTATUS
IopSortRelationListForRemove (
    __in PRELATION_LIST RelationList
    )

/*++

Routine Description:

    Perform a topological sort of devices in RelationList and output to DeviceObjectList.
    The topological sorting is done based on:

        * parent/child relationship
        * dependencies.

    Device removal should be processed in order of the list. In other words, the first
    device in the list should be removed first.

    Removal/ejection relations should already be factored in, simply by being a part of 
    the RelationList. This is because we do not guarantee removal order for these
    relations. However, devices added through relations can still have their own
    parent/child relationship and dependencies, so we consider them in the sorting process.

Arguments:
    
    RelationList - At the end, it will contain a sorted list of devices in which
        the partial ordering is consistent the removal order. The first device in
        the list should be removed first, and the last device should be removed last.

Return Value:

    NT Status Code.

--*/

{
    
    ULONG dependencyTypes;
    PDEVICE_OBJECT_LIST_ENTRY device;
    PDEVICE_NODE deviceNode;
    PDEVICE_OBJECT deviceObject;
    BOOLEAN hasProviders;
    ULONG index;
    ULONG leafDeviceCount;
    PLIST_ENTRY listEntry;
    PLIST_ENTRY listHead;
    BOOLEAN parentInRelationList;
    PDEVICE_NODE previousParent;
    PDEVICE_OBJECT providerDevice;
    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(RelationList != NULL);

    if (RelationList->Sorted != FALSE) {
        status = STATUS_SUCCESS;
        goto Exit;
    }
    
    PnpAcquireDependencyRelationsLock(FALSE);
    leafDeviceCount = 0;

    //
    // Loop through the unsorted list in any order and mark leaf nodes
    // that need to be removed last (used as starting point graph traversal).
    //

    for (index = 0; index < IopGetRelationsCount(RelationList); index +=1) {
        device = &RelationList->DeviceObjectList->Devices[index];
        deviceObject = device->DeviceObject;
        deviceNode = PP_DO_TO_DN(deviceObject);
        
        PNP_ASSERT(deviceNode != NULL);

        //
        // Is the parent of this device in the relationList?
        //

        if (deviceNode->Parent == NULL) {
            previousParent = PNP_DN_PREVIOUS_PARENT(deviceNode);
            if (previousParent == NULL) {

                //
                // This device is an orphan.
                //

                parentInRelationList = FALSE;

            } else {

                //
                // Check if previous parent is in the same list.
                //

                parentInRelationList = IopIsRelationInList(
                    RelationList,
                    previousParent->PhysicalDeviceObject);
            }

        } else {

            //
            // Check if actual parent is in the same list.
            //

            parentInRelationList = IopIsRelationInList(
                    RelationList,
                    deviceNode->Parent->PhysicalDeviceObject);

        }

        //
        // Does this device have any satisfied providers?
        // (i.e., does it depend on other active devices?)
        //

        hasProviders = FALSE;
        listHead = PiGetProviderList(deviceObject);
        listEntry = listHead->Flink;

        while (listEntry != listHead) {
            PiEnumerateProviderListEntry(listEntry, &providerDevice, &dependencyTypes);
            PNP_ASSERT((dependencyTypes &
                        ~(PNP_DEPENDENCY_TYPE_INITIALIZATION |
                          PNP_DEPENDENCY_TYPE_START)) == 0);

            if (providerDevice != NULL &&
                IopIsRelationInList(RelationList, providerDevice)) {

                hasProviders = TRUE;
                break;
            }

            listEntry = listEntry->Flink;
        }

        if (parentInRelationList == FALSE && hasProviders == FALSE) {
            
            //
            // This is a "leaf" device; it has no incoming edges, so they are removed last.
            //

            device->Flags |= DEVICE_OBJECT_LIST_ENTRY_LEAF;
            leafDeviceCount++;

        } else {
            device->Flags &= ~DEVICE_OBJECT_LIST_ENTRY_LEAF;
        }
    }

    if (leafDeviceCount == 0) {

        //
        // There should be at least one leaf node, so that traversal can start.
        // If there is none, suspect that there is a cycle in the dependency graph.
        // In this case, leave the relation list unsorted.
        //

        PNP_ASSERT(FALSE);
        status = STATUS_UNSUCCESSFUL;

    } else {
        
        //
        // Perform the topological sort.
        //

        status = PipSortDeviceObjectList(&RelationList->DeviceObjectList);
        if (NT_SUCCESS(status)) {
            RelationList->Sorted = TRUE;
        }
    }

    PnpReleaseDependencyRelationsLock();

Exit:

    return status;
}

_Use_decl_annotations_
NTSTATUS
PipDeviceObjectListAdd (
    PDEVICE_OBJECT_LIST *List,
    PDEVICE_OBJECT Device,
    DEVICE_RELATION_LEVEL RelationLevel,
    DEVICE_TAGGED_FOR_REMOVAL Tagged
    )

/*++

Routine Description:

    Insert a device object into a DEVICE_OBJECT_LIST.

Arguments:

    List - Supplies the list to insert the device object.

    Target - Supplies the target device object insert.

    RelationLevel - Supplies the relationship of this device to the target
        device.

    Tagged - Supplies if the device is tagged for removal.

Return Value:

    A pointer to newly created list entry (DEVICE_OBJECT_LIST_ENTRY) in the
    list.

--*/

{

    PDEVICE_OBJECT_LIST_ENTRY Entry;
    NTSTATUS Status;
    PDEVICE_OBJECT_LIST CurrentList;

    PAGED_CODE();
    
    PNP_ASSERT(List != NULL);
    PNP_ASSERT(*List != NULL);
    PNP_ASSERT(Device != NULL);

    CurrentList = *List;

#if DBG

    if (PipIsDeviceInDeviceObjectList(CurrentList, Device, NULL)) {

        //
        // This is an attempt to add an existing item again.
        //

        PNP_ASSERT(FALSE);
        Status = STATUS_OBJECT_NAME_COLLISION;
        goto Exit;
    }

#endif

    if (CurrentList->Count + 1 == CurrentList->MaxCount) {
        Status = PipGrowDeviceObjectList(&CurrentList);
        if (!NT_SUCCESS(Status)) {
            goto Exit;
        }
    }

    //
    // There must be enough room for another entry.
    //
    
    PNP_ASSERT(CurrentList->Count + 1 <= CurrentList->MaxCount);

    Entry = &CurrentList->Devices[CurrentList->Count];
    ObReferenceObject(Device);
    Entry->DeviceObject = Device;
    Entry->RelationLevel = RelationLevel;
    Entry->Flags = 0;
    if (Tagged == DEVICE_TAGGED) {
        Entry->Flags |= DEVICE_OBJECT_LIST_ENTRY_TAGGED;
        CurrentList->TagCount += 1;
    }

    CurrentList->Count += 1;
    Status = STATUS_SUCCESS;

Exit:

    *List = CurrentList;
    return Status;
}

NTSTATUS
PipDeviceObjectListRemove(
    __in PDEVICE_OBJECT_LIST List,
    __in ULONG Index
    )
{

    NTSTATUS Status;
    PDEVICE_OBJECT_LIST_ENTRY Device;

    PAGED_CODE();

    if (Index >= List->Count) {
        Status = STATUS_ARRAY_BOUNDS_EXCEEDED;
        goto Exit;
    }

    Device = &List->Devices[Index];
    ObDereferenceObject(Device->DeviceObject);
    if (DEVICE_OBJECT_LIST_ENTRY_IS_TAGGED(Device)) {
        List->TagCount -= 1;
    }

    //
    // If not the last element, shift the following elements to the left.
    //

    if (Index < List->Count - 1) {
        RtlMoveMemory(&List->Devices[Index],
                      &List->Devices[Index + 1],
                      (List->Count - Index - 1) * sizeof(DEVICE_OBJECT_LIST_ENTRY));
    }

    List->Count -= 1;
    Status = STATUS_SUCCESS;

Exit:

    return Status;
}

BOOLEAN
PipIsDeviceInDeviceObjectList (
    __in PDEVICE_OBJECT_LIST List,
    __in PDEVICE_OBJECT Target,
    __out_opt PDEVICE_OBJECT_LIST_ENTRY *Result
    )

/*++

Routine Description:

    Search for a particular device object in a list of device objects.

Arguments:

    List - Supplies the list to perform the search on.

    Target - Supplies the target device object to search for in the list.

    Result - If specified, returns a pointer to corresponding 
        DEVICE_OBJECT_LIST_ENTRY from the list if the target is found.
        If target is not found, it will contain NULL.

Return Value:

    TRUE if the target is found in the list, FALSE otherwise.

--*/

{
    
    ULONG Index; 

    PAGED_CODE();

    Index = PipDeviceObjectListIndexOf(List, Target, List->Count, Result);
    if (Index == -1) {
        return FALSE;

    } else {
        return TRUE;
    }
}

LONG
PipDeviceObjectListIndexOf (
    __in PDEVICE_OBJECT_LIST List,
    __in PDEVICE_OBJECT Target,
    __in ULONG EndIndex,
    __out_opt PDEVICE_OBJECT_LIST_ENTRY *Result
    )

/*++

Routine Description:

    Find the array index of the given device object in the list.
    Search is performed within indices [0, End) (i.e., check elements from
    indices 0 to End - 1).

Arguments:

    List - Supplies a pointer to the list of device objects.

    Target - Supplies a pointer to the target device object to search for.

    EndIndex - Supplies the end index; exclusive. Pass in the size of the
        array to search the entire list.
    
    Result - Supplies a pointer to receive the list entry.

Return Value:

    Array index of the device in the list, -1 if not found.

--*/

{

    LONG index;
    BOOLEAN found;

    PAGED_CODE();

    PNP_ASSERT(List != NULL);
    PNP_ASSERT(Target != NULL);
    PNP_ASSERT(EndIndex <= List->Count);
    PNP_ASSERT(EndIndex < LONG_MAX);

    found = FALSE;
    for (index = 0; index < (LONG) EndIndex; index++) {
        if (Target == List->Devices[index].DeviceObject) {
            found = TRUE;
            if (ARGUMENT_PRESENT(Result)) {
                *Result = &List->Devices[index];
            }

            break;
        }
    }

    if (found == FALSE) {
        index = -1;
    }

    if (found == FALSE && ARGUMENT_PRESENT(Result)) {
        *Result = NULL;
    }

    return index;
}

NTSTATUS
PipDeviceObjectListElementAt (
    __in PDEVICE_OBJECT_LIST List,
    __in ULONG Index,
    __out PDEVICE_OBJECT *DeviceObject,
    __out_opt PDEVICE_RELATION_LEVEL RelationLevel,
    __out_opt PDEVICE_TAGGED_FOR_REMOVAL Tagged
    )

/*++

Routine Description:

    Find the element at given index in the given list of device objects.

Arguments:

    List - Supplies the list of device objects.

    Index - Supplies the index for the list item.

    DeviceObject - Returns a pointer to the list item at the given index.
    
    DirectDescendant - If specified, returns TRUE if DeviceObject is a direct descendent,
        FALSE otherwise.

Return Value:

    NT status code.

--*/

{

    PDEVICE_OBJECT_LIST_ENTRY Device;
    NTSTATUS Status;

    PAGED_CODE();

    if (List == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    if (Index >= List->Count) {
        Status = STATUS_ARRAY_BOUNDS_EXCEEDED;
        goto Exit;
    }

    Device = &List->Devices[Index];
    *DeviceObject = Device->DeviceObject;
    if (ARGUMENT_PRESENT(RelationLevel)) {
        *RelationLevel = Device->RelationLevel;
    }

    if (ARGUMENT_PRESENT(Tagged)) {
        if ((Device->Flags & DEVICE_OBJECT_LIST_ENTRY_TAGGED) == 0) {
            *Tagged = DEVICE_UNTAGGED;
        } else {
            *Tagged = DEVICE_TAGGED;
        }
    }

    Status = STATUS_SUCCESS;

Exit:

    return Status;
}

_Use_decl_annotations_
NTSTATUS
PipSortDeviceObjectList (
    PDEVICE_OBJECT_LIST *List
    )

/*++

Routine Description:

    Sort a list of device objects according to the removal order.
    The sort is performed in-place.

    Removal order takes into account the following:

        * Parent-child relationship (remove child first, then parent)
        * Dependencies - (remove dependent frirst, then provider)

Arguments:

    List - Supplies a pointer to a pointer to the list of device objects to be
        sorted. The caller will provide a unsorted list; when the call is
        complete, this will point to a different (sorted) list. The unsorted
        list will be freed.

Return Value:

    NT status code.

--*/

{

    
    PDEVICE_OBJECT_LIST_ENTRY Device;
    PDEVICE_OBJECT_LIST_ENTRY DevicesList;
    ULONG Index;
    ULONG Ordinal;
    ULONG ListSize;

    PAGED_CODE();

    PNP_ASSERT(List != NULL);
    PNP_ASSERT(*List != NULL);

    PiAssertDependencyRelationsLockHeld(FALSE);

    ListSize = (*List)->Count;
    DevicesList = (*List)->Devices;


    //
    // Make sure that none of nodes in the list is marked as visited.
    //
    
    for (Index = 0; Index < ListSize; Index += 1) {
        Device = &DevicesList[Index];
        Device->Flags &= ~DEVICE_OBJECT_LIST_ENTRY_VISITED;
    }

    //
    // Start post-order depth-first search starting with the leaf nodes.
    //

    Ordinal = 1;
    for (Index = 0; Index < ListSize; Index += 1) {
        Device = &DevicesList[Index];
        if ((Device->Flags & DEVICE_OBJECT_LIST_ENTRY_LEAF) != FALSE) {
            PipVisitDeviceObjectListEntry(&DevicesList[Index],
                                          *List,
                                          &Ordinal);
        }
    }

    //
    // Rearrange array elements according to their new ordinal.
    //

    qsort(DevicesList, ListSize, sizeof(*Device), PipSortDevicesByOrdinal);
    return STATUS_SUCCESS;
}

_Use_decl_annotations_
NTSTATUS
PipVisitDeviceObjectListEntry (
    PDEVICE_OBJECT_LIST_ENTRY Device,
    PDEVICE_OBJECT_LIST List,
    PULONG Ordinal
    )
/*++

Routine Description:

    Recursively visit device objects, and its related devices (children and dependents).

    It is a post-order depth-first traversal.
    When a node is processed, it is added to the sorted list.

Arguments:

    Device - Supplies the current device being visited.

    List - Supplies the list of device objects to be sorted. After the
        function returns, the Ordinal field of the array element will be
        updated to indicate the order.

    Ordinal - Supplies a pointer to the next ordinal to be assigned to an
        array element. This pointer will be passed around throughout the
        graph traversal, being incremented every time a device is visited
        for the last time.

Return Value:

    NT status code.

--*/
{

    PDEVICE_NODE childDeviceNode;
    ULONG dependencyTypes;
    PDEVICE_NODE deviceNode;
    PDEVICE_OBJECT dependentDevice;
    NTSTATUS finalStatus;
    ULONG index;
    PLIST_ENTRY listEntry;
    PLIST_ENTRY listHead;
    PDEVICE_OBJECT_LIST_ENTRY relatedDevice;

    PAGED_CODE();

    PNP_ASSERT(Device != NULL);
    PNP_ASSERT(List != NULL);
    PiAssertDependencyRelationsLockHeld(FALSE);

    finalStatus = STATUS_SUCCESS;
    relatedDevice = NULL;
    if ((Device->Flags & DEVICE_OBJECT_LIST_ENTRY_VISITED) == FALSE) {
        Device->Flags |= DEVICE_OBJECT_LIST_ENTRY_VISITED;
        deviceNode = PP_DO_TO_DN(Device->DeviceObject);

        //
        // Visit its children. It should already be in the unsorted list.
        // Instead of looping through deviceNode->Child, we do the opposite
        // and look for any device nodes that has its parent set as the
        // current device. This is becuase if the current device was
        // surprise-removed while its old child was pending remove,
        // the parent doesn't know about the child, but the child does.
        //

        for (index = 0; index < List->Count; index++) {
            relatedDevice = &List->Devices[index];
            childDeviceNode = PP_DO_TO_DN(relatedDevice->DeviceObject);

            PNP_ASSERT(childDeviceNode != NULL);

            if (childDeviceNode->Parent == deviceNode) {
                PipVisitDeviceObjectListEntry(relatedDevice, List, Ordinal);

            } else if (PNP_DN_PREVIOUS_PARENT(childDeviceNode) == deviceNode) {
                PipVisitDeviceObjectListEntry(relatedDevice, List, Ordinal);
            }
        }
        
        //
        // Loop through dependents, visit it if it's in the unsorted list.
        //

        listHead = PiGetDependentList(Device->DeviceObject);
        listEntry = listHead->Flink;
        while (listEntry != listHead) {
            PiEnumerateDependentListEntry(listEntry, &dependentDevice, &dependencyTypes);
            PNP_ASSERT((dependencyTypes &
                        ~(PNP_DEPENDENCY_TYPE_INITIALIZATION |
                          PNP_DEPENDENCY_TYPE_START)) == 0);

            listEntry = listEntry->Flink;
            if (dependentDevice == NULL) {
                continue;
            }

            if (PipIsDeviceInDeviceObjectList(List,
                                              dependentDevice,
                                              &relatedDevice)) {

                PipVisitDeviceObjectListEntry(relatedDevice, List, Ordinal);
            }
        }

        //
        // This is the last time this node is visited during traversal.
        // In other words, all of its descendants / dependents have been
        // visited. Assign an ordinal to this device so it can be sorted later.
        //

        Device->Ordinal = *Ordinal;
        *Ordinal += 1;
    }

    return finalStatus;
}

NTSTATUS
PipGrowDeviceObjectList (
    __inout PDEVICE_OBJECT_LIST *List
    )

/*++

Routine Description:

    Given a list, replace with a longer list with identical contents.
    The list will retain references on the device objects.

Arguments:

    List - Supplies a pointer to the list to grow.

Return Value:

    NT status code.

--*/

{

    PDEVICE_OBJECT Device;
    ULONG Index;
    PDEVICE_OBJECT_LIST NewList;
    PDEVICE_OBJECT_LIST OldList;
    DEVICE_RELATION_LEVEL RelationLevel;
    DEVICE_TAGGED_FOR_REMOVAL Tagged;
    NTSTATUS Status;

    PAGED_CODE();

    Device = NULL;
    RelationLevel = RELATION_LEVEL_REMOVE_EJECT;
    Tagged = DEVICE_UNTAGGED;
    OldList = *List;
    NewList = NULL;

    //
    // Make sure that max size is not exceeded.
    //

    if ((OldList->MaxCount * DEVICE_OBJECT_LIST_GROWTH_RATE)
        > DEVICE_OBJECT_LIST_MAX_SIZE) {

        Status = STATUS_ARRAY_BOUNDS_EXCEEDED;
        goto Exit;
    }

    //
    // Allocate a new list with bigger max size.
    //

    NewList = PiAllocateDeviceObjectList(
        OldList->OperationCode,
        OldList->MaxCount * DEVICE_OBJECT_LIST_GROWTH_RATE);

    if (NewList == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    //
    // Copy over contents of the old list into the new list.
    //

    for (Index = 0; Index < OldList->Count; Index += 1) {
        Status = PipDeviceObjectListElementAt(OldList,
                                              Index,
                                              &Device,
                                              &RelationLevel,
                                              &Tagged);

        NT_ASSERT(NT_SUCCESS(Status));

        //
        // Adding via this call will add another reference on the PDOs.
        //

        Status = PipDeviceObjectListAdd(&NewList,
                                        Device,
                                        RelationLevel,
                                        Tagged);

        //
        // The call must succeed, since there is always enough room.
        //

        NT_ASSERT(NT_SUCCESS(Status));
    }

    PNP_ASSERT(OldList->Count == NewList->Count);
    PNP_ASSERT(OldList->TagCount == NewList->TagCount);

    //
    // The old list is no longer needed. Freeing the list will cause the PDOs
    // to be dereferenced (which was added when adding to old list).
    //

    PipFreeDeviceObjectList(OldList);
    OldList = NULL;
    Status = STATUS_SUCCESS;

Exit:

    *List = NewList;
    return Status;
}

int
__cdecl
PipSortDevicesByOrdinal (
    __in const void *Arg1,
    __in const void *Arg2
    )

/*++

Routine Description:

    Comparator function to be used for sorting relation list according to
    the assigned ordinal value.

Arguments:

    Arg1 - Supplies pointer to the first element to compare.

    Arg2 - Supplies pointer to the second element to compare.

Return Value:

    A negative number if Arg1 should come before Arg2,
    a positive number if Arg1 should come after Arg2,
    Arg1 is equivalent to Arg2.

--*/

{

    PDEVICE_OBJECT_LIST_ENTRY Device1;
    PDEVICE_OBJECT_LIST_ENTRY Device2;

    Device1 = (PDEVICE_OBJECT_LIST_ENTRY) Arg1;
    Device2 = (PDEVICE_OBJECT_LIST_ENTRY) Arg2;
    return Device1->Ordinal - Device2->Ordinal;
}