/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    bootcfg.c

Abstract:

    This module implements the handling of boot configurations.

Author:

    Santosh Jodh (santoshj) 08-02-2004

Environment:

    KernelMode only.

Revision History:

--*/

#include "pnpmgrp.h"
#pragma hdrstop

//
// Regular data segment
//
#ifdef ALLOC_DATA_PRAGMA
#pragma  data_seg()
#endif

//
// Variable to hold boot allocation routine.
//
PIO_ALLOCATE_BOOT_RESOURCES_ROUTINE IopAllocateBootResourcesRoutine;

#ifdef ALLOC_PRAGMA

#pragma alloc_text(INIT, IopReportBootResources)
#pragma alloc_text(INIT, IopAllocateLegacyBootResources)
#pragma alloc_text(PAGE, IopBootAllocation)
#pragma alloc_text(PAGE, IopAllocateBootResources)
#pragma alloc_text(PAGE, IopAllocateBootResourcesInternal)
#pragma alloc_text(PAGE, IopCreateCmResourceList)
#pragma alloc_text(PAGE, IopCombineCmResourceList)
#pragma alloc_text(PAGE, IopReleaseFilteredBootResources)
#pragma alloc_text(PAGE, PnpNeedToReleaseBootResources)
#pragma alloc_text(PAGE, PnpIsRangeWithin)

#endif // ALLOC_PRAGMA

NTSTATUS
IopAllocateBootResources (
    __in ARBITER_REQUEST_SOURCE   ArbiterRequestSource,
    __in PDEVICE_OBJECT           DeviceObject,
    __in PCM_RESOURCE_LIST        BootResources
    )

/*++

Routine Description:

    This routine allocates boot resources.
    Before all Boot Bux Extenders are processed, this routine is called only
    for non-madeup devices since arbiters for their boot resources should
    already be initialized by the time the time they got enumerated.
    After all Boot Bus Extenders are processed, this routine is used for all
    boot allocations.

Parameters:

    ArbiterRequestSource    - Source of this resource request.

    DeviceObject            - If non-NULL, the boot resources are
        pre-allocated. These resources will not be given out until they are
        released to the arbiters. If NULL, the boot resources get reserved and
        may be given out if there is no other choice.

    BootResources           - Supplies a pointer to the BOOT resources. If
        DeviceObject is NULL, caller should release this pool.

Return Value:

    The status returned is the final completion status of the operation.

--*/
{
    NTSTATUS    status;

    PAGED_CODE();

    IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                 "Allocating boot resources...\n"));
    //
    // Claim the lock so no other resource allocations\releases can take place.
    //
    IopLockResourceManager();
    //
    // Call the function that does the real work.
    //
    status = IopAllocateBootResourcesInternal(
                ArbiterRequestSource,
                DeviceObject,
                BootResources);
    //
    // Unblock other resource allocations\releases.
    //
    IopUnlockResourceManager();

    return status;
}

NTSTATUS
IopReportBootResources (
    __in ARBITER_REQUEST_SOURCE   ArbiterRequestSource,
    __in PDEVICE_OBJECT           DeviceObject,
    __in PCM_RESOURCE_LIST        BootResources
    )

/*++

Routine Description:

    This routine is used to report boot resources.
    This routine gets called before all Boot Bus Extenders are processed. It
    calls the actual allocation function for non-madeup devices. For others,
    it delays the allocation. The postponed allocations take place when the
    arbiters come online by calling IopAllocateLegacyBootResources. Once all
    Boot Bus Extenders are processed, the calls get routed to
    IopAllocateBootResources directly.

Parameters:

    ArbiterRequestSource    - Source of this resource request.

    DeviceObject            - If non-NULL, the boot resources are
        pre-allocated. These resources will not be given out until they are
        released to the arbiters. If NULL, the boot resources get reserved and
        may be given out if there is no other choice.

    BootResources           - Supplies a pointer to the BOOT resources. If
        DeviceObject is NULL, caller should release this pool.

Return Value:

    The status returned is the final completion status of the operation.

--*/
{
    ULONG                           size;
    PDEVICE_NODE                    deviceNode;
    PIOP_RESERVED_RESOURCES_RECORD  resourceRecord;

    IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                 "Reporting boot resources...\n"));

    if ((size = PnpDetermineResourceListSize(BootResources)) == 0) {

        return STATUS_SUCCESS;
    }
    if (DeviceObject) {

        deviceNode = PP_DO_TO_DN(DeviceObject);
        PNP_ASSERT(deviceNode);
        if (!(deviceNode->Flags & DNF_MADEUP)) {
            //
            // Allocate BOOT configs for non-madeup devices right away.
            //
            return IopAllocateBootResources(
                    ArbiterRequestSource,
                    DeviceObject,
                    BootResources);
        }
        if (!deviceNode->BootResources) {

            deviceNode->BootResources = ExAllocatePool(PagedPool, size);
            if (!deviceNode->BootResources) {

                return STATUS_INSUFFICIENT_RESOURCES;
            }
            RtlCopyMemory(deviceNode->BootResources, BootResources, size);
        }
    } else {

        deviceNode = NULL;
    }
    //
    // Delay BOOT allocation since arbiters may not be around.
    //
    resourceRecord = (PIOP_RESERVED_RESOURCES_RECORD) ExAllocatePool(
                        PagedPool,
                        sizeof(IOP_RESERVED_RESOURCES_RECORD));
    if (!resourceRecord) {
        //
        // Free memory we allocated and return failure.
        //
        if (deviceNode && deviceNode->BootResources) {

            ExFreePool(deviceNode->BootResources);
            deviceNode->BootResources = NULL;
        }
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    if (deviceNode) {

        resourceRecord->ReservedResources   = deviceNode->BootResources;
    } else {

        resourceRecord->ReservedResources   = BootResources;
    }
    resourceRecord->DeviceObject            = DeviceObject;
    //
    // Link this record into our list.
    //
    resourceRecord->Next                    = IopInitReservedResourceList;
    IopInitReservedResourceList             = resourceRecord;

    return STATUS_SUCCESS;
}

NTSTATUS
IopAllocateLegacyBootResources (
    __in INTERFACE_TYPE   InterfaceType,
    __in ULONG            BusNumber
    )

/*++

Routine Description:

    This routine is called to reserve legacy BOOT resources for the specified
    InterfaceType and BusNumber. This is done everytime a new bus with a legacy
    InterfaceType gets enumerated.

Parameters:

    InterfaceType   - Legacy InterfaceType.

    BusNumber       - Legacy BusNumber.

Return Value:

    The status returned is the final completion status of the operation.

--*/

{
    NTSTATUS                        status;
    PIOP_RESERVED_RESOURCES_RECORD  resourceRecord;
    PIOP_RESERVED_RESOURCES_RECORD  prevRecord;
    PCM_RESOURCE_LIST               newList;
    PCM_RESOURCE_LIST               remainingList;
    PCM_RESOURCE_LIST               resourceList;

    if (IopInitHalDeviceNode && IopInitHalResources) {

        remainingList = NULL;
        newList = IopCreateCmResourceList(
                    IopInitHalResources,
                    InterfaceType,
                    BusNumber,
                    &remainingList);
        if (newList) {
            //
            // Sanity check that there was no error.
            //
            if (remainingList == NULL) {
                //
                // Full match.
                //
                PNP_ASSERT(newList == IopInitHalResources);
            } else {
                //
                // Partial match.
                //
                PNP_ASSERT(IopInitHalResources != newList);
                PNP_ASSERT(IopInitHalResources != remainingList);
            }
            if (remainingList) {

                ExFreePool(IopInitHalResources);
            }
            IopInitHalResources         = remainingList;
            remainingList               = IopInitHalDeviceNode->BootResources;
            PipSetDevNodeFlags(IopInitHalDeviceNode, DNF_HAS_BOOT_CONFIG);

            IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                         "Allocating HAL reported resources on interface=%x and bus number=%x...\n",
                         InterfaceType,
                         BusNumber));

            status = IopAllocateBootResources(ArbiterRequestHalReported,
                                              IopInitHalDeviceNode->PhysicalDeviceObject,
                                              newList);
            IopInitHalDeviceNode->BootResources = IopCombineCmResourceList(
                                                    remainingList,
                                                    newList);
            PNP_ASSERT(IopInitHalDeviceNode->BootResources);
            //
            // Free previous BOOT config if any.
            //
            if (remainingList) {

                ExFreePool(remainingList);
            }
        } else {
            //
            // No match. Sanity check that there was no error.
            //
            PNP_ASSERT(remainingList && remainingList == IopInitHalResources);
        }
    }
    prevRecord      = NULL;
    resourceRecord  = IopInitReservedResourceList;
    while (resourceRecord) {

        resourceList = resourceRecord->ReservedResources;
        if (    resourceList->List[0].InterfaceType == InterfaceType &&
                resourceList->List[0].BusNumber == BusNumber) {

            IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                         "Allocating boot config for made-up device on interface=%x and bus number=%x...\n",
                         InterfaceType,
                         BusNumber));

            status = IopAllocateBootResources(ArbiterRequestPnpEnumerated,
                                              resourceRecord->DeviceObject,
                                              resourceList);
            if (resourceRecord->DeviceObject == NULL) {

                ExFreePool(resourceList);
            }
            if (prevRecord) {

                prevRecord->Next            = resourceRecord->Next;
            } else {

                IopInitReservedResourceList = resourceRecord->Next;
            }
            ExFreePool(resourceRecord);
            if (prevRecord) {

                resourceRecord = prevRecord->Next;
            } else {

                resourceRecord = IopInitReservedResourceList;
            }
        } else {

            prevRecord      = resourceRecord;
            resourceRecord  = resourceRecord->Next;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
IopAllocateBootResourcesInternal (
    __in ARBITER_REQUEST_SOURCE   ArbiterRequestSource,
    __in PDEVICE_OBJECT           DeviceObject,
    __in PCM_RESOURCE_LIST        BootResources
    )

/*++

Routine Description:

    This routine reports boot resources for the specified device to
    arbiters.

Parameters:

    ArbiterRequestSource    - Source of this resource request.

    DeviceObject            - If non-NULL, the boot resources are
        pre-allocated. These resources will not be given out until they are
        released to the arbiters. If NULL, the boot resources get reserved and
        may be given out if there is no other choice.

    BootResources           - Supplies a pointer to the BOOT resources. If
        DeviceObject is NULL, caller should release this pool.

Return Value:

    The status returned is the final completion status of the operation.

--*/

{
    PDEVICE_NODE deviceNode;
    PIO_RESOURCE_REQUIREMENTS_LIST ioResources;
    PNP_RESOURCE_REQUEST request;
    NTSTATUS status;

    PAGED_CODE();

    RtlZeroMemory(&request, sizeof(PNP_RESOURCE_REQUEST));
    deviceNode = PP_DO_TO_DN(DeviceObject);

    PNP_ASSERT(deviceNode != NULL);

    ioResources = PnpCmResourcesToIoResources(0,
                                              BootResources,
                                              LCPRI_BOOTCONFIG);
    if (ioResources == NULL) {
        status = STATUS_UNSUCCESSFUL;
        goto Exit;
    }

    IopDbgPrint((IOP_RESOURCE_VERBOSE_LEVEL,
                 "\n===================================\n"));

    IopDbgPrint((IOP_RESOURCE_VERBOSE_LEVEL, "Boot Resource List:: "));
    PnpDumpResourceRequirementsList(ioResources);

    IopDbgPrint((IOP_RESOURCE_VERBOSE_LEVEL,
                 " ++++++++++++++++++++++++++++++\n"));

    request.AllocationType = ArbiterRequestSource;
    request.ResourceRequirements = ioResources;
    request.PhysicalDevice = DeviceObject;
    status = IopResourceRequirementsListToReqList(&request, &request.ReqList);
    if (!NT_SUCCESS(status) || (request.ReqList == NULL)) {
        goto Exit;
    }

    status = IopBootAllocation(request.ReqList);
    if (!NT_SUCCESS(status)) {
        goto Exit;
    }

    PipSetDevNodeFlags(deviceNode, DNF_BOOT_CONFIG_RESERVED);
    PnpBuildCmResourceList(&request, FALSE);
    if (!NT_SUCCESS(request.Status)) {
        status = request.Status;
        goto Exit;
    }

    if (deviceNode->BootResources == NULL) {
        deviceNode->BootResources = request.ResourceAssignment;

    } else {
        if (request.ResourceAssignment != NULL) {
            ExFreePool(request.ResourceAssignment);
            request.ResourceAssignment = NULL;
        }
    }

    if (deviceNode->BootResourcesTranslated != NULL) {
        ExFreePool(deviceNode->BootResourcesTranslated);
        deviceNode->BootResourcesTranslated = NULL;
    }

    deviceNode->BootResourcesTranslated = request.TranslatedResourceAssignment;
    status = STATUS_SUCCESS;

Exit:

    if (request.ReqList != NULL) {
        IopFreeReqList(request.ReqList);
    }

    if (ioResources != NULL) {
        ExFreePool(ioResources);
    }

    if (!NT_SUCCESS(status)) {

        if (deviceNode->BootResources != NULL) {
            ExFreePool(deviceNode->BootResources);
            deviceNode->BootResources = NULL;
        }

        if (deviceNode->BootResourcesTranslated != NULL) {
            ExFreePool(deviceNode->BootResourcesTranslated);
            deviceNode->BootResourcesTranslated = NULL;
        }

        IopDbgPrint((IOP_RESOURCE_ERROR_LEVEL,
                     "IopAllocateBootResourcesInternal: Failed with status = %08X\n",
                     status));
    }

    return status;
}

NTSTATUS
IopBootAllocation (
    __in PREQ_LIST ReqList
    )

/*++

Routine Description:

    This routine calls the arbiters for the ReqList to do BootAllocation.

Parameters:

    ReqList - List of BOOT resources in internal format.

Return Value:

    The status returned is the final completion status of the operation.

--*/

{
    NTSTATUS                    status;
    NTSTATUS                    returnStatus;
    LIST_ENTRY                  activeArbiterList;
    PLIST_ENTRY                 listEntry;
    PPI_RESOURCE_ARBITER_ENTRY  arbiterEntry;
    ARBITER_PARAMETERS          p;

    PAGED_CODE();

    returnStatus = STATUS_SUCCESS;
    InitializeListHead(&activeArbiterList);
    ReqList->SelectedAlternative = ReqList->AlternativeTable;
    IopAddRemoveReqDescs(   (*ReqList->SelectedAlternative)->DescTable,
                            (*ReqList->SelectedAlternative)->DescCount,
                            &activeArbiterList,
                            TRUE);
    listEntry = activeArbiterList.Flink;
    while (listEntry != &activeArbiterList){

        arbiterEntry = CONTAINING_RECORD(
                        listEntry,
                        PI_RESOURCE_ARBITER_ENTRY,
                        ActiveArbiterList);
        listEntry = listEntry->Flink;
        if (arbiterEntry->ResourcesChanged == FALSE) {

            continue;
        }
        PNP_ASSERT(IsListEmpty(&arbiterEntry->ResourceList) == FALSE);
        p.Parameters.BootAllocation.ArbitrationList =
            &arbiterEntry->ResourceList;
        status = arbiterEntry->ArbiterInterface->ArbiterHandler(
                    arbiterEntry->ArbiterInterface->Context,
                    ArbiterActionBootAllocation,
                    &p);

        if (!NT_SUCCESS(status)) {

            PARBITER_LIST_ENTRY arbiterListEntry;

            arbiterListEntry = (PARBITER_LIST_ENTRY)
                                arbiterEntry->ResourceList.Flink;

            IopDbgPrint((IOP_RESOURCE_ERROR_LEVEL,
                         "Allocate Boot Resources Failed ::\n\tCount = %x, PDO = %x\n",
                         arbiterListEntry->AlternativeCount,
                         arbiterListEntry->PhysicalDeviceObject));

            PnpDumpResourceDescriptor("\t", arbiterListEntry->Alternatives);
            returnStatus = status;
        }
        IopInitializeArbiterEntryState(arbiterEntry);
    }

    IopCheckDataStructures(IopRootDeviceNode);

    return returnStatus;
}

PCM_RESOURCE_LIST
IopCreateCmResourceList (
    __in PCM_RESOURCE_LIST    ResourceList,
    __in INTERFACE_TYPE       InterfaceType,
    __in ULONG                BusNumber,
    __out PCM_RESOURCE_LIST   *RemainingList
    )

/*++

Routine Description:

    This routine returns the CM_RESOURCE_LIST portion out of the specified list
    that matches the specified BusNumber and InterfaceType.

Parameters:

    ResourceList    - Input resource list.

    InterfaceType   - Interface type.

    BusNumber       - Bus number.

    RemainingList   - Portion not matching BusNumber and InterfaceType.

Return Value:

    Returns the matching CM_RESOURCE_LIST if successful, else NULL.

--*/

{
    ULONG                           i;
    ULONG                           j;
    ULONGLONG                       totalSize;
    ULONGLONG                       matchSize;
    ULONGLONG                       listSize;
    ULONGLONG                       remainingSize;
    PCM_RESOURCE_LIST               newList;
    PCM_FULL_RESOURCE_DESCRIPTOR    fullResourceDesc;
    PCM_FULL_RESOURCE_DESCRIPTOR    newFullResourceDesc;
    PCM_FULL_RESOURCE_DESCRIPTOR    remainingFullResourceDesc;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR partialDescriptor;

    PAGED_CODE();

    fullResourceDesc    = &ResourceList->List[0];
    totalSize           = FIELD_OFFSET(CM_RESOURCE_LIST, List);
    matchSize           = 0;
    remainingSize       = 0;
    
    //
    // Determine the size of memory to be allocated for the matching resource
    // list.
    //
    
    for (i = 0; i < ResourceList->Count; i++) {

        //
        // Add the size of this descriptor.
        //

        listSize = FIELD_OFFSET(CM_FULL_RESOURCE_DESCRIPTOR,
                                PartialResourceList) +
                   FIELD_OFFSET(CM_PARTIAL_RESOURCE_LIST,
                                PartialDescriptors);

        partialDescriptor =
            &fullResourceDesc->PartialResourceList.PartialDescriptors[0];

        for (j = 0; j < fullResourceDesc->PartialResourceList.Count; j++) {
            ULONGLONG descriptorSize = sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR);
            if (partialDescriptor->Type == CmResourceTypeDeviceSpecific) {
                descriptorSize +=
                    partialDescriptor->u.DeviceSpecificData.DataSize;
            }
            
            listSize += descriptorSize;
            partialDescriptor = (PCM_PARTIAL_RESOURCE_DESCRIPTOR)
                                    ((PUCHAR)partialDescriptor +
                                            descriptorSize);
        }
        
        if (    fullResourceDesc->InterfaceType == InterfaceType &&
                fullResourceDesc->BusNumber == BusNumber) {

            matchSize += listSize;
        }

        totalSize += listSize;
        fullResourceDesc = (PCM_FULL_RESOURCE_DESCRIPTOR)
                                  ((PUCHAR)fullResourceDesc + listSize);
    }

    if (!matchSize) {
        *RemainingList  = ResourceList;
        return NULL;
    }

    matchSize += FIELD_OFFSET(CM_RESOURCE_LIST, List);
    if (matchSize == totalSize) {
        *RemainingList  = NULL;
        return ResourceList;
    }

    //
    // Allocate memory for both lists.
    //

    PNP_ASSERT(matchSize <= MAXSIZE_T);
    if (matchSize > MAXSIZE_T) {
        *RemainingList = NULL;
        return NULL;
    }
    
    newList = (PCM_RESOURCE_LIST)ExAllocatePool(PagedPool, (SIZE_T)matchSize);
    if (newList == NULL) {
        *RemainingList = NULL;
        return NULL;
    }

    remainingSize = totalSize - matchSize + FIELD_OFFSET(CM_RESOURCE_LIST, List);
    PNP_ASSERT(remainingSize <= MAXSIZE_T);
    if (remainingSize > MAXSIZE_T) {
        *RemainingList = NULL;
        ExFreePool(newList);
        return NULL;
    }

    *RemainingList = (PCM_RESOURCE_LIST)ExAllocatePool(PagedPool,
                                                       (SIZE_T)remainingSize);
    
    if (*RemainingList == NULL) {
        ExFreePool(newList);
        return NULL;
    }
    
    newList->Count              = 0;
    (*RemainingList)->Count     = 0;
    newFullResourceDesc         = &newList->List[0];
    remainingFullResourceDesc   = &(*RemainingList)->List[0];
    fullResourceDesc            = &ResourceList->List[0];
    for (i = 0; i < ResourceList->Count; i++) {
        listSize = FIELD_OFFSET(CM_FULL_RESOURCE_DESCRIPTOR,
                                PartialResourceList) +
                   FIELD_OFFSET(CM_PARTIAL_RESOURCE_LIST,
                                PartialDescriptors);
        
        partialDescriptor =
            &fullResourceDesc->PartialResourceList.PartialDescriptors[0];

        for (j = 0; j < fullResourceDesc->PartialResourceList.Count; j++) {
            ULONGLONG descriptorSize = sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR);
            if (partialDescriptor->Type == CmResourceTypeDeviceSpecific) {
                descriptorSize +=
                    partialDescriptor->u.DeviceSpecificData.DataSize;
            }

            listSize += descriptorSize;
            partialDescriptor = (PCM_PARTIAL_RESOURCE_DESCRIPTOR)
                                    ((PUCHAR)partialDescriptor +
                                        descriptorSize);
        }
        
        if (    fullResourceDesc->InterfaceType == InterfaceType &&
                fullResourceDesc->BusNumber == BusNumber) {

            newList->Count++;

            //
            // The computed listSize has been been validated to not overflow
            // SIZE_T when validating matchSize and totalSize.
            //
             
            PNP_ASSERT(listSize <= MAXSIZE_T);
            __analysis_assume(listSize <= MAXSIZE_T);
            RtlCopyMemory(newFullResourceDesc, fullResourceDesc, (SIZE_T)listSize);
            newFullResourceDesc = (PCM_FULL_RESOURCE_DESCRIPTOR)
                                          ((PUCHAR)newFullResourceDesc +
                                            listSize);
        } else {
            (*RemainingList)->Count++;

            //
            // The computed listSize has been been validated to not overflow
            // SIZE_T when validating matchSize and totalSize.
            //
             
            PNP_ASSERT(listSize <= MAXSIZE_T);
            __analysis_assume(listSize <= MAXSIZE_T);
            RtlCopyMemory(
                remainingFullResourceDesc,
                fullResourceDesc,
                (SIZE_T)listSize);

            remainingFullResourceDesc = (PCM_FULL_RESOURCE_DESCRIPTOR)
                                          ((PUCHAR)remainingFullResourceDesc +
                                            listSize);
        }
        
        fullResourceDesc = (PCM_FULL_RESOURCE_DESCRIPTOR)
                                  ((PUCHAR)fullResourceDesc +
                                    listSize);
    }

    return newList;
}

PCM_RESOURCE_LIST
IopCombineCmResourceList (
    __in PCM_RESOURCE_LIST ResourceListA,
    __in PCM_RESOURCE_LIST ResourceListB
    )

/*++

Routine Description:

    This routine combines the two CM_RESOURCE_LISTs and returns the resulting
    CM_RESOURCE_LIST.

Parameters:

    ResourceListA - ListA.

    ResourceListB - ListB.

Return Value:

    Returns the combined CM_RESOURCE_LIST if successful, else NULL.

--*/

{
    PCM_RESOURCE_LIST   newList;
    ULONG               sizeA;
    ULONG               sizeB;
    ULONG               size;
    ULONG               diff;

    PAGED_CODE();

    if (ResourceListA == NULL) {

        return ResourceListB;
    }

    if (ResourceListB == NULL) {

        return ResourceListA;
    }
    newList = NULL;
    sizeA   = PnpDetermineResourceListSize(ResourceListA);
    sizeB   = PnpDetermineResourceListSize(ResourceListB);
    if (sizeA && sizeB) {

        diff = sizeof(CM_RESOURCE_LIST) - sizeof(CM_FULL_RESOURCE_DESCRIPTOR);
        size = sizeA + sizeB - diff;
        newList = (PCM_RESOURCE_LIST)ExAllocatePool(PagedPool, size);
        if (newList) {

            RtlCopyMemory(newList, ResourceListA, sizeA);
            RtlCopyMemory(
                (PUCHAR)newList + sizeA,
                (PUCHAR)ResourceListB + diff,
                sizeB - diff);
            newList->Count += ResourceListB->Count;
        }
    }

    return newList;
}

LOGICAL
PnpIsRangeWithin (
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Inner,
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Outer
    )

/*++

Description

    This function checks if the inner range lies within the outer range or is same as
    the outer range. In addition to the start and length, the attributes for the ranges
    have to be the same.

Parameters

    Inner - Pointer to the inner range descriptor.

    Outer - Pointer to the outer range descriptor

Return

    TRUE if the Inner range lies completely within the Outer range and has identical
    attributes.

-*/

{
    LOGICAL contained;
    ULONG type;
    ULONGLONG innerMax, innerMin, outerMax, outerMin;

    PAGED_CODE();

    PNP_ASSERT(Inner);
    PNP_ASSERT(Outer);

    PNP_ASSERT(Inner->Type == Outer->Type);

    type = Inner->Type;

    PNP_ASSERT(type != CmResourceTypeNull);
    PNP_ASSERT(type != CmResourceTypeDeviceSpecific);
    PNP_ASSERT(type < CmResourceTypeMaximum);

    //
    // Assume that the its not contained.
    //
    contained = FALSE;

    //
    // Get the start and end of the ranges.
    //
    switch (type) {

    case CmResourceTypeMemory:
    case CmResourceTypeMemoryLarge:
    case CmResourceTypePort: {
        ULONGLONG decodedLength;

        decodedLength = RtlCmDecodeMemIoResource(Inner, &innerMin);
        innerMax = innerMin + decodedLength - 1;

        decodedLength = RtlCmDecodeMemIoResource(Outer, &outerMin);
        outerMax = outerMin + decodedLength - 1;

        break;
    }

    case CmResourceTypeBusNumber:

        innerMin = Inner->u.BusNumber.Start;
        innerMax = Inner->u.BusNumber.Start + Inner->u.BusNumber.Length - 1;

        outerMin = Outer->u.BusNumber.Start;
        outerMax = Outer->u.BusNumber.Start + Outer->u.BusNumber.Length - 1;

        break;

    case CmResourceTypeInterrupt:

        innerMin = innerMax = Inner->u.Interrupt.Vector;

        outerMin = outerMax = Outer->u.Interrupt.Vector;

        break;

    case CmResourceTypeDma:

        innerMin = innerMax = Inner->u.Dma.Channel;

        outerMin = outerMax = Outer->u.Dma.Channel;

        break;

    default:

        PNP_ASSERT(0);

        innerMin = innerMax = 0;

        outerMin = outerMax = 0;

        break;

    }

    PNP_ASSERT(innerMin <= innerMax);
    PNP_ASSERT(outerMin <= outerMax);

    if (innerMin >= outerMin && innerMax <= outerMax &&
        Inner->ShareDisposition == Outer->ShareDisposition &&
        Inner->Flags == Outer->Flags) {

        //
        // Inner range lies completely within the outer range and
        // has the exact same attributes.
        //
        contained = TRUE;
    }

    return contained;
}

LOGICAL
PnpNeedToReleaseBootResources (
    __in PDEVICE_NODE       DeviceNode,
    __in PCM_RESOURCE_LIST  AllocatedResources,
    __out PLOGICAL          HasMoved
    )

/*++

Description

    This routine checks the AllocatedResources against boot allocated resources.
    If the allocated resources do not cover all the resource types in boot resources,
    in another words some types of boot resources have not been released by arbiter,
    we will return TRUE to indicate we need to release the boot resources manually.

Parameters

    DeviceNode -  A device node

    AllocatedResources - the resources assigned to the devicenode by arbiters.

    HasMoved - Pointer that recieves TRUE if the device was moved off its boot config.

Return

    TRUE if one or more resource type from the boot configuration is missing in the
    allocated configuration. This would happen for example if the function driver did not
    need an IRQ and filtered it out but the BIOS had assigned one to the device.

--*/

{
    PCM_FULL_RESOURCE_DESCRIPTOR cmFullDesc_a, cmFullDesc_b;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR cmDescriptor_a, cmDescriptor_b;
    ULONG size_a, size_b, i, j, k;
    LOGICAL returnValue, found, moved;
    PCM_RESOURCE_LIST bootResources;

    PAGED_CODE();

    PNP_ASSERT(DeviceNode);
    PNP_ASSERT(AllocatedResources);
    PNP_ASSERT(HasMoved);

    returnValue = FALSE;
    *HasMoved = FALSE;

    bootResources = DeviceNode->BootResources;
    if (AllocatedResources->Count == 1 && bootResources && bootResources->Count != 0) {

        cmFullDesc_a = &AllocatedResources->List[0];
        cmFullDesc_b = &bootResources->List[0];
        for (i = 0; i < bootResources->Count; i++) {

            cmDescriptor_b = &cmFullDesc_b->PartialResourceList.PartialDescriptors[0];
            for (j = 0; j < cmFullDesc_b->PartialResourceList.Count; j++) {

                size_b = 0;

                switch (cmDescriptor_b->Type) {

                case CmResourceTypeNull:

                    break;

                case CmResourceTypeDeviceSpecific:

                     size_b = cmDescriptor_b->u.DeviceSpecificData.DataSize;

                     break;

                default:

                     if (cmDescriptor_b->Type < CmResourceTypeMaximum) {

                         found = FALSE;
                         moved = TRUE;
                         cmDescriptor_a = &cmFullDesc_a->PartialResourceList.PartialDescriptors[0];
                         for (k = 0; k < cmFullDesc_a->PartialResourceList.Count; k++) {

                             size_a = 0;
                             if (cmDescriptor_a->Type == CmResourceTypeDeviceSpecific) {

                                 size_a = cmDescriptor_a->u.DeviceSpecificData.DataSize;

                             } else if (cmDescriptor_b->Type == cmDescriptor_a->Type) {

                                 found = TRUE;

                                 if (PnpIsRangeWithin(cmDescriptor_b, cmDescriptor_a)) {

                                     moved = FALSE;
                                     break;
                                 }
                             }

                             cmDescriptor_a++;
                             cmDescriptor_a = (PCM_PARTIAL_RESOURCE_DESCRIPTOR) ((PUCHAR)cmDescriptor_a + size_a);
                         }

                         if (found == FALSE) {

                             //
                             // Some descriptor got filtered out.
                             //
                             returnValue = TRUE;
                             goto exit;
                         }

                         if (moved) {

                             //
                             // Device has been moved off some range in its boot
                             // configuration.
                             //
                             *HasMoved = TRUE;
                             goto exit;
                         }
                     }

                     break;
                }

                cmDescriptor_b++;
                cmDescriptor_b = (PCM_PARTIAL_RESOURCE_DESCRIPTOR) ((PUCHAR)cmDescriptor_b + size_b);
            }

            cmFullDesc_b = (PCM_FULL_RESOURCE_DESCRIPTOR)cmDescriptor_b;
        }
    }

exit:

    return returnValue;
}

NTSTATUS
PnpReleaseBootResourcesForFilteredRequirements (
    __in PPNP_RESOURCE_REQUEST AssignedResourceRequest
    )

/*++

Routine Description:

    This routine is invoked to release the corresponding boot resources from
    arbiters whose requirements are filtered out.

Parameters:

    AssignedResourceRequest - Supplies a pointer for assigned resource request.

Return Value:

    NT Status value.

--*/

{

    ARBITER_LIST_ENTRY ArbiterListEntry;
    PREQ_ALTERNATIVE AssignedReqAlternative;
    PREQ_DESC AssignedReqDesc;
    ULONG AssignedReqDescIndex;
    PREQ_LIST AssignedReqList;
    PPI_RESOURCE_ARBITER_ENTRY AssignedReqResourceArbiter;
    PREQ_ALTERNATIVE BootReqAlternative;
    PREQ_DESC BootReqDesc;
    ULONG BootReqDescIndex;
    PREQ_LIST BootReqList;
    PPI_RESOURCE_ARBITER_ENTRY BootReqResourceArbiter;
    PNP_RESOURCE_REQUEST BootRequest;
    PIO_RESOURCE_REQUIREMENTS_LIST BootRequirements;
    PCM_RESOURCE_LIST BootResources;
    PDEVICE_NODE DeviceNode;
    PDEVICE_OBJECT DeviceObject;
    NTSTATUS Status;

    BootRequirements = NULL;
    RtlZeroMemory(&BootRequest, sizeof(PNP_RESOURCE_REQUEST));

    //
    // Given a boot resource list, form a req desc table such that every
    // resource type is associated with an appropriate arbiter.
    //

    DeviceObject = AssignedResourceRequest->PhysicalDevice;
    DeviceNode = PP_DO_TO_DN(DeviceObject);
    BootResources = DeviceNode->BootResources;
    if (BootResources == NULL) {
        Status = STATUS_UNSUCCESSFUL;
        goto Exit;
    }

    BootRequirements = PnpCmResourcesToIoResources(0,
                                                   BootResources,
                                                   LCPRI_BOOTCONFIG);

    if (BootRequirements == NULL) {
        Status = STATUS_UNSUCCESSFUL;
        goto Exit;
    }

    BootRequest.AllocationType = ArbiterRequestPnpEnumerated;
    BootRequest.ResourceRequirements = BootRequirements;
    BootRequest.PhysicalDevice = DeviceObject;
    Status = IopResourceRequirementsListToReqList(&BootRequest,
                                                  &BootRequest.ReqList);

    if (!NT_SUCCESS(Status) || (BootRequest.ReqList == NULL)) {
        goto Exit;
    }

    BootReqList = (PREQ_LIST)(BootRequest.ReqList);

    PNP_ASSERT(BootReqList->AlternativeCount > 0);

    BootReqAlternative = BootReqList->AlternativeTable[0];

    //
    // Given an assigned resource request, grab a selected alternative that
    // has a req desc table that links every resource type with its arbiters.
    //

    AssignedReqList = (PREQ_LIST)(AssignedResourceRequest->ReqList);

    PNP_ASSERT(AssignedReqList->AlternativeCount > 0);

    AssignedReqAlternative = *(AssignedReqList->SelectedAlternative);

    //
    // Run thru the boot resource arbiter list and compare it with assigned
    // resource arbiter list. If a specific resource type is found in the
    // boot resource list but not in the assigned list, then invoke its
    // boot resource arbiter to release the boot resource.
    //
    // N.B. This does not handle the case where a boot resource list has
    //      multiple descriptors of one resource type that correspond to
    //      different instances of arbiters and when a subset of requirement
    //      for the resource type is filtered out.
    //

    Status = STATUS_SUCCESS;
    ArbiterListEntry.AlternativeCount = 0;
    ArbiterListEntry.Alternatives = NULL;
    ArbiterListEntry.PhysicalDeviceObject = DeviceNode->PhysicalDeviceObject;
    ArbiterListEntry.Flags = 0;
    ArbiterListEntry.WorkSpace = 0;
    ArbiterListEntry.Assignment = NULL;
    ArbiterListEntry.RequestSource = ArbiterRequestPnpEnumerated;
    for (BootReqDescIndex = 0;
         BootReqDescIndex < BootReqAlternative->DescCount;
         BootReqDescIndex++) {

        BootReqDesc = BootReqAlternative->DescTable[BootReqDescIndex];
        BootReqResourceArbiter = BootReqDesc->u.Arbiter;
        if (BootReqResourceArbiter != NULL) {
            for (AssignedReqDescIndex = 0;
                 AssignedReqDescIndex < AssignedReqAlternative->DescCount;
                 AssignedReqDescIndex++) {

                AssignedReqDesc = 
                    AssignedReqAlternative->DescTable[AssignedReqDescIndex];

                AssignedReqResourceArbiter = AssignedReqDesc->u.Arbiter;
                if (AssignedReqResourceArbiter != NULL) {
                    if (BootReqResourceArbiter->ResourceType ==
                        AssignedReqResourceArbiter->ResourceType) {

                        break;
                    }
                }
            }

            //
            // Reached the end of assigned resource arbiter list but did not
            // find the corresponding boot resource type. Invoke the boot
            // resource arbiter instance to free this resource.
            //

            if (AssignedReqDescIndex == AssignedReqAlternative->DescCount) {

                PNP_ASSERT(IsListEmpty(&BootReqResourceArbiter->ResourceList));

                InitializeListHead(&ArbiterListEntry.ListEntry);
                InsertTailList(&BootReqResourceArbiter->ResourceList,
                               &ArbiterListEntry.ListEntry);

                Status = IopCallArbiter(BootReqResourceArbiter,
                                        ArbiterActionTestAllocation,
                                        &BootReqResourceArbiter->ResourceList,
                                        NULL,
                                        NULL);

                RemoveEntryList(&ArbiterListEntry.ListEntry);
                if (!NT_SUCCESS(Status)) {
                    break;
                }

                Status = IopCallArbiter(BootReqResourceArbiter,
                                        ArbiterActionCommitAllocation,
                                        NULL,
                                        NULL,
                                        NULL);

                if (!NT_SUCCESS(Status)) {
                    break;
                }
            }
        }
    }

Exit:
    if (BootRequest.ReqList != NULL) {
        IopFreeReqList(BootRequest.ReqList);
    }

    if (BootRequirements != NULL) {
        ExFreePool(BootRequirements);
    }

    return Status;
}

VOID
IopReleaseFilteredBootResources (
    __in PPNP_RESOURCE_REQUEST AssignTable,
    __in_ecount(0) PPNP_RESOURCE_REQUEST AssignTableEnd
    )

/*++

Routine Description:

    For each AssignTable entry, this routine checks if we need to manually release the device's
    boot resources.

Parameters:

    AssignTable - supplies a pointer to the first entry of a PNP_RESOURCE_REQUEST table.

    AssignTableEnd - supplies a pointer to the end of PNP_RESOURCE_REQUEST table.

Return Value:

    None.

--*/

{
    NTSTATUS status;
    PPNP_RESOURCE_REQUEST assignEntry;
    PDEVICE_OBJECT physicalDevice;
    PDEVICE_NODE deviceNode;
    LOGICAL retry, moved;

    PAGED_CODE();

    //
    // Assume we dont have to retry previously conflicting devices. If some device moves
    // off its boot configuration or has a resource type filtered out, then we retry
    // allocating resources for previously conflicting devices since new resources are
    // potentially available.
    //
    retry = FALSE;

    //
    // Go thru each entry, for each Physical device object, we build a CmResourceList
    // from its ListOfAssignedResources.
    //
    for (assignEntry = AssignTable; assignEntry < AssignTableEnd; ++assignEntry) {

        if (assignEntry->Flags & (IOP_ASSIGN_IGNORE | IOP_ASSIGN_RETRY)) {

            continue;
        }

        if (assignEntry->Status == STATUS_SUCCESS &&
            assignEntry->AllocationType == ArbiterRequestPnpEnumerated &&
            assignEntry->ResourceAssignment) {

            moved = FALSE;

            physicalDevice = assignEntry->PhysicalDevice;
            deviceNode = PP_DO_TO_DN(physicalDevice);
            //
            // Release the device's boot resources if desired
            // (If a driver filters its res req list and removes some boot resources, after arbiter satisfies
            // the new res req list, the filtered out boot resources do not get
            // released by arbiters.  Because they no longer passed to arbiters. )
            // I am not 100% sure we should release the filtered boot resources.  But that's what arbiters try
            // to achieve.  So, we will do it.
            //
            if (PnpNeedToReleaseBootResources(deviceNode,
                                              assignEntry->ResourceAssignment,
                                              &moved)) {

                IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                             "%wZ has one or more boot resource filtered out!\n",
                             PnpGetDeviceInstancePath(deviceNode)));

                //
                // We are releasing some resources. We should retry previously conflicting
                // devices.
                //
                retry = TRUE;
                status = PnpReleaseBootResourcesForFilteredRequirements(assignEntry);
                PipClearDevNodeFlags(deviceNode, DNF_BOOT_CONFIG_RESERVED);  // Keep DeviceNode->BootResources
                if (!NT_SUCCESS(status)) {

                    IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                                 "Failed to release boot resources"
                                 "for filtered requirements on %wZ\n",
                                 PnpGetDeviceInstancePath(deviceNode)));

                    PNP_ASSERT(status == STATUS_SUCCESS);

                    assignEntry->Flags = IOP_ASSIGN_EXCLUDE;
                    assignEntry->Status = status;

                    ExFreePool(assignEntry->ResourceAssignment);
                    assignEntry->ResourceAssignment = NULL;
                }

                deviceNode->ResourceList = NULL;

            } else if (moved) {

                //
                // Device moved off its boot configuration. We should retry previously
                // conflicting devices.
                //
                retry = TRUE;

                IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                             "%wZ moved off its boot configuration!\n",
                             PnpGetDeviceInstancePath(deviceNode)));
            }
        }

        if ((assignEntry->Flags & IOP_ASSIGN_EXCLUDE) ||
            assignEntry->ResourceAssignment == NULL) {

            //
            // Allow MCFG conflict error to propagate up the stack.
            //
            
            if (assignEntry->Status != STATUS_BAD_MCFG_TABLE) {
                assignEntry->Status = STATUS_CONFLICTING_ADDRESSES;
            }
        }
    }

    if (retry) {

        //
        // We released some resources, try to satisfy devices
        // with resource conflict.
        //
        PnpRequestDeviceAction(NULL, AssignResources, FALSE, 0, NULL, NULL);
    }
}
