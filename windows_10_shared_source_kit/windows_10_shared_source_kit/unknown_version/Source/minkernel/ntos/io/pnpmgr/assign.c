/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    assign.c

Abstract:

    This module implements assignment of hardware resources.

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

const WCHAR PnpWstrRaw[]                  = L".Raw";
const WCHAR PnpWstrTranslated[]           = L".Translated";

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PnpGetResourceRequirementsForAssignTable)
#pragma alloc_text(PAGE, PnpFreeResourceRequirementsForAssignTable)
#pragma alloc_text(PAGE, PnpBuildCmResourceLists)
#pragma alloc_text(PAGE, PnpBuildCmResourceList)
#pragma alloc_text(PAGE, PnpReleaseResourcesInternal)
#pragma alloc_text(PAGE, PnpAssignResourcesToDevices)
#pragma alloc_text(PAGE, PnpProcessAssignResources)
#pragma alloc_text(PAGE, PnpProcessAssignResourcesWorker)
#pragma alloc_text(PAGE, PnpRestoreResourcesInternal)
#pragma alloc_text(PAGE, PnpReallocateResources)
#pragma alloc_text(PAGE, PnpAllocateResources)
#pragma alloc_text(PAGE, PnpRearrangeAssignTable)
#pragma alloc_text(PAGE, PnpCompareResourceRequestPriority)

#if DBG

#pragma alloc_text(PAGE, PnpDumpResourceRequirementsList)
#pragma alloc_text(PAGE, PnpDumpResourceDescriptor)
#pragma alloc_text(PAGE, PnpDumpCmResourceDescriptor)
#pragma alloc_text(PAGE, PnpDumpCmResourceList)

#endif  // DBG

#endif  // ALLOC_PRAGMA

NTSTATUS
PnpGetResourceRequirementsForAssignTable (
    __in  PPNP_RESOURCE_REQUEST   RequestTable,
    __in_ecount(0)  PPNP_RESOURCE_REQUEST   RequestTableEnd,
    __out PULONG                  DeviceCount
    )

/*++

Routine Description:

    This function gets resource requirements for entries in the request table.

Parameters:

    RequestTable    - Start of request table.

    RequestTableEnd - End of request table.

    DeviceCount     - Gets number of devices with non-NULL requirements.

Return Value:

    STATUS_SUCCESS if we got one non-NULL requirement, else STATUS_UNSUCCESSFUL.

--*/

{
    PPNP_RESOURCE_REQUEST           request;
    NTSTATUS                        status;
    PDEVICE_NODE                    deviceNode;
    ULONG                           length;
    PIO_RESOURCE_REQUIREMENTS_LIST  filteredList;
    LOGICAL                         exactMatch;
    PREQ_LIST                       reqList;

    PAGED_CODE();

    filteredList = NULL;
    *DeviceCount = 0;
    for (   request = RequestTable;
            request < RequestTableEnd;
            request++) {
        //
        // Skip uninteresting entries.
        //
        request->ReqList = NULL;
        if (request->Flags & IOP_ASSIGN_IGNORE) {

            continue;
        }
        request->ResourceAssignment             = NULL;
        request->TranslatedResourceAssignment   = NULL;
        deviceNode                              = PP_DO_TO_DN(
                                                    request->PhysicalDevice);
        IopAcquireResourceListLock(&PiResourceListLock);
        if (    (deviceNode->Flags & DNF_RESOURCE_REQUIREMENTS_CHANGED) &&
                deviceNode->ResourceRequirements) {

            ExFreePool(deviceNode->ResourceRequirements);
            deviceNode->ResourceRequirements = NULL;
            PipClearDevNodeFlags(deviceNode, DNF_RESOURCE_REQUIREMENTS_NEED_FILTERED);
            //
            // Mark that caller needs to clear DNF_RESOURCE_REQUIREMENTS_CHANGED
            // flag on success.
            //
            request->Flags |= IOP_ASSIGN_CLEAR_RESOURCE_REQUIREMENTS_CHANGE_FLAG;
        }
        IopReleaseResourceListLock(&PiResourceListLock);
        if (!request->ResourceRequirements) {

            if (    deviceNode->ResourceRequirements &&
                    !(deviceNode->Flags & DNF_RESOURCE_REQUIREMENTS_NEED_FILTERED)) {

                IopDbgPrint((   IOP_RESOURCE_VERBOSE_LEVEL,
                                "Resource requirements list already exists for "
                                "%wZ\n",
                                PnpGetDeviceInstancePath(deviceNode)));

                request->ResourceRequirements   = deviceNode->ResourceRequirements;
                request->AllocationType         = ArbiterRequestPnpEnumerated;
            } else {

                IopDbgPrint((   IOP_RESOURCE_INFO_LEVEL,
                                "Query Resource requirements list for %wZ...\n",
                                PnpGetDeviceInstancePath(deviceNode)));

                status = IopQueryDeviceResources(
                            request->PhysicalDevice,
                            QUERY_RESOURCE_REQUIREMENTS,
                            &request->ResourceRequirements,
                            &length);
                if (    !NT_SUCCESS(status) ||
                        !request->ResourceRequirements) {
                    //
                    // Success with NULL ResourceRequirements means no resource
                    // required.
                    //
                    request->Flags  |= IOP_ASSIGN_IGNORE;
                    request->Status = status;
                    continue;
                }
                IopAcquireResourceListLock(&PiResourceListLock);
                if (deviceNode->ResourceRequirements) {

                    ExFreePool(deviceNode->ResourceRequirements);
                    PipClearDevNodeFlags(deviceNode, DNF_RESOURCE_REQUIREMENTS_NEED_FILTERED);
                }
                deviceNode->ResourceRequirements = request->ResourceRequirements;
                IopReleaseResourceListLock(&PiResourceListLock);
            }
        }
        //
        // For non-stop case, even though the res req list has changed, we need
        // to guarantee that it will get its current setting, even if the new
        // requirements do not cover the current setting.
        //
        if (request->Flags & IOP_ASSIGN_KEEP_CURRENT_CONFIG) {

            PNP_ASSERT(
                deviceNode->ResourceRequirements ==
                    request->ResourceRequirements);
            status = PnpFilterResourceRequirementsList(
                         request->ResourceRequirements,
                         deviceNode->ResourceList,
                         &filteredList,
                         &exactMatch);
            if (NT_SUCCESS(status) && filteredList) {
                //
                // No need to free the original request->ResourceRequirements
                // since its cached in deviceNode->ResourceRequirements.
                //
                request->ResourceRequirements = filteredList;
            } else {
                //
                // Clear the flag so we dont free request->ResourceRequirements.
                //
                request->Flags &= ~IOP_ASSIGN_KEEP_CURRENT_CONFIG;
            }
        }
        PnpDumpResourceRequirementsList(request->ResourceRequirements);
        //
        // Convert the requirements list to our internal representation.
        //
        status = IopResourceRequirementsListToReqList(
                        request,
                        &request->ReqList);
        if (NT_SUCCESS(status) && request->ReqList) {

            reqList = (PREQ_LIST)request->ReqList;
            //
            // Sort the list such that higher priority alternatives are placed
            // in the front of the list.
            //
            IopRearrangeReqList(reqList);
            if (reqList->BestAlternative) {
                //
                // Requests from less flexible devices get higher priority.
                //
                request->Priority = (reqList->AlternativeCount < 3)?
                                        0 : reqList->AlternativeCount;
                request->Status = status;
                (*DeviceCount)++;
                continue;
            }
            //
            // This device has no soft configuration.
            //
            PnpFreeResourceRequirementsForAssignTable(request, request + 1);
            status = STATUS_DEVICE_CONFIGURATION_ERROR;
        }

        request->Status = status;
        request->Flags  |= IOP_ASSIGN_IGNORE;
    }

    return (*DeviceCount)? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}

VOID
PnpFreeResourceRequirementsForAssignTable (
    __in PPNP_RESOURCE_REQUEST RequestTable,
    __in_ecount(0) PPNP_RESOURCE_REQUEST RequestTableEnd
    )

/*++

Routine Description:

    For each resource request in the table, this routine frees its
    associated REQ_LIST.

Parameters:

    RequestTable    - Start of request table.

    RequestTableEnd - End of request table.

Return Value:

    None.

--*/

{
    PPNP_RESOURCE_REQUEST request;

    PAGED_CODE();

    for (request = RequestTable; request < RequestTableEnd; request++) {

        IopFreeReqList(request->ReqList);
        request->ReqList = NULL;
        if (    request->Flags & IOP_ASSIGN_KEEP_CURRENT_CONFIG &&
                request->ResourceRequirements) {
            //
            // The REAL resreq list is cached in DeviceNode->ResourceRequirements.
            // We need to free the filtered list.
            //
            ExFreePool(request->ResourceRequirements);
            request->ResourceRequirements = NULL;
        }
    }
}

VOID
PnpBuildCmResourceLists(
    __in PPNP_RESOURCE_REQUEST AssignTable,
    __in_ecount(0) PPNP_RESOURCE_REQUEST AssignTableEnd,
    __in BOOLEAN UpdateDevnode
    )

/*++

Routine Description:

    For each AssignTable entry, this routine queries device's IO resource requirements
    list and converts it to our internal REQ_LIST format.

Parameters:

    AssignTable - supplies a pointer to the first entry of a PNP_RESOURCE_REQUEST table.

    AssignTableEnd - supplies a pointer to the end of PNP_RESOURCE_REQUEST table.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    PPNP_RESOURCE_REQUEST assignEntry;
    PDEVICE_NODE deviceNode;
    PDEVICE_OBJECT physicalDevice;
    ULONG previousRetries;
    ULONG retryCount;

    PAGED_CODE();

    //
    // Clear the existing resource lists from the affected device nodes.
    //

    if (UpdateDevnode != FALSE) {
        for (assignEntry = AssignTable; assignEntry < AssignTableEnd; ++assignEntry) {
            deviceNode = PP_DO_TO_DN(assignEntry->PhysicalDevice);
            IopAcquireResourceListLock(&PiResourceListLock);
            if (deviceNode->ResourceList) {
                ExFreePool(deviceNode->ResourceList);
                deviceNode->ResourceList = NULL;
            }

            if (deviceNode->ResourceListTranslated) {
                ExFreePool(deviceNode->ResourceListTranslated);
                deviceNode->ResourceListTranslated = NULL;
            }

            IopReleaseResourceListLock(&PiResourceListLock);
        }
    }

    //
    // Go thru each entry, for each Physical device object, we build a CmResourceList
    // from its ListOfAssignedResources.
    //

    retryCount = MAXULONG;
    do {
        previousRetries = retryCount;
        retryCount = 0;
        for (assignEntry = AssignTable; assignEntry < AssignTableEnd; ++assignEntry) {
            assignEntry->ResourceAssignment = NULL;
            if (assignEntry->Flags & IOP_ASSIGN_IGNORE || assignEntry->Flags & IOP_ASSIGN_RETRY) {
                continue;
            }

            if (assignEntry->Flags & IOP_ASSIGN_EXCLUDE) {
                assignEntry->Status = STATUS_UNSUCCESSFUL;
                continue;
            }

            //
            // On iterations that are done to resolve retries, only process
            // entries that require a retry.
            //

            if ((previousRetries != MAXULONG) && (assignEntry->Status != STATUS_RETRY)) {
                continue;
            }

            assignEntry->Status = STATUS_SUCCESS;
            PnpBuildCmResourceList (assignEntry, TRUE);

            //
            // Count the number of entries that require a retry.  This occurs if
            // the translators required to build the resource lists do not have
            // enough information to perform the translation.  In this case,
            // complete resource list generation for the rest of the table and
            // return to this entry at the end.  In cases where, for instance,
            // the translation of a parent device is necessary to translate the
            // child device resources, this will allow the proper ordering to
            // occur.
            //

            if (assignEntry->Status == STATUS_RETRY) {
                retryCount += 1;
                continue;
            }

            //
            // If requested, update the device node state with this assignment.
            //

            if (UpdateDevnode != FALSE) {
                deviceNode = PP_DO_TO_DN(assignEntry->PhysicalDevice);
                if (assignEntry->Flags &
                    IOP_ASSIGN_CLEAR_RESOURCE_REQUIREMENTS_CHANGE_FLAG) {

                    PipClearDevNodeFlags(deviceNode, DNF_RESOURCE_REQUIREMENTS_CHANGED | DNF_NON_STOPPED_REBALANCE);
                }

                IopAcquireResourceListLock(&PiResourceListLock);
                deviceNode->ResourceList = assignEntry->ResourceAssignment;
                if (deviceNode->ResourceList == NULL) {
                    PipSetDevNodeFlags(deviceNode, DNF_NO_RESOURCE_REQUIRED);
                }

                deviceNode->ResourceListTranslated = assignEntry->TranslatedResourceAssignment;
                IopReleaseResourceListLock(&PiResourceListLock);
            }

            if (assignEntry->ResourceAssignment) {
                physicalDevice = assignEntry->PhysicalDevice;
                deviceNode = PP_DO_TO_DN(physicalDevice);
                IopWriteAllocatedResourcesToRegistry(
                    deviceNode,
                    assignEntry->ResourceAssignment,
                    PnpDetermineResourceListSize(assignEntry->ResourceAssignment));

                IopDbgPrint((
                    IOP_RESOURCE_INFO_LEVEL,
                    "Building CM resource lists for %wZ...\n",
                    PnpGetDeviceInstancePath(deviceNode)));

                IopDbgPrint((
                    IOP_RESOURCE_INFO_LEVEL,
                    "Raw resources "));

                PnpDumpCmResourceList(assignEntry->ResourceAssignment);
                IopDbgPrint((
                    IOP_RESOURCE_INFO_LEVEL,
                    "Translated resources "));

                PnpDumpCmResourceList(assignEntry->TranslatedResourceAssignment);
            }
        }
    } while ((retryCount > 0) && (retryCount < previousRetries));
}

VOID
PnpReleaseResourcesInternal (
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This routine releases the assigned resources for device specified by DeviceNode.
    Note, this routine does not reset the resource related fields in DeviceNode structure.

Parameters:

    DeviceNode - specifies the device node whose resources are goint to be released.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    PDEVICE_NODE device;
    PLIST_ENTRY listHead, listEntry;
    PPI_RESOURCE_ARBITER_ENTRY arbiterEntry;
    ARBITER_LIST_ENTRY arbiterListEntry;
    INTERFACE_TYPE interfaceType;
    ULONG busNumber, listCount, i, j, size;
    PCM_RESOURCE_LIST resourceList;
    PCM_FULL_RESOURCE_DESCRIPTOR cmFullDesc;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR cmPartDesc;
    LOGICAL search = TRUE;

#if DBG

    NTSTATUS status;

#endif

    InitializeListHead(&arbiterListEntry.ListEntry);
    arbiterListEntry.AlternativeCount = 0;
    arbiterListEntry.Alternatives = NULL;
    arbiterListEntry.PhysicalDeviceObject = DeviceNode->PhysicalDeviceObject;
    arbiterListEntry.Flags = 0;
    arbiterListEntry.WorkSpace = 0;
    arbiterListEntry.Assignment = NULL;
    arbiterListEntry.RequestSource = ArbiterRequestPnpEnumerated;

    resourceList = DeviceNode->ResourceList;
    if (resourceList == NULL) {
        resourceList = DeviceNode->BootResources;
    }
    if (resourceList && resourceList->Count > 0) {
        listCount = resourceList->Count;
        cmFullDesc = &resourceList->List[0];
    } else {
        listCount = 1;
        resourceList = NULL;
        cmFullDesc = NULL;
    }
    for (i = 0; i < listCount; i++) {

        if (resourceList) {
            interfaceType = cmFullDesc->InterfaceType;
            busNumber = cmFullDesc->BusNumber;
            if (interfaceType == InterfaceTypeUndefined) {
                interfaceType = PnpDefaultInterfaceType;
            }
        } else {
            interfaceType = PnpDefaultInterfaceType;
            busNumber = 0;
        }

        if (DeviceNode != IopRootDeviceNode) {

            device = DeviceNode->Parent;
        } else {

            device = DeviceNode;
        }
        search = TRUE;
        while (device) {
            if ((device == IopRootDeviceNode) && search) {
                device = IopFindLegacyBusDeviceNode (
                                 interfaceType,
                                 busNumber
                                 );

                //
                // If we did not find a PDO, try again with InterfaceType == Isa. This allows
                // drivers that request Internal to get resources even if there is no PDO
                // that is Internal. (but if there is an Internal PDO, they get that one)
                //

                if ((device == IopRootDeviceNode) && (interfaceType == Internal)) {
                    device = IopFindLegacyBusDeviceNode(Isa, 0);
                }
                search = FALSE;

            }
            listHead = &device->DeviceArbiterList;
            listEntry = listHead->Flink;
            while (listEntry != listHead) {
                arbiterEntry = CONTAINING_RECORD(listEntry, PI_RESOURCE_ARBITER_ENTRY, DeviceArbiterList);
                if (arbiterEntry->ArbiterInterface != NULL) {
                    search = FALSE;
                    PNP_ASSERT(IsListEmpty(&arbiterEntry->ResourceList));
                    InitializeListHead(&arbiterEntry->ResourceList);  // Recover from assert
                    InsertTailList(&arbiterEntry->ResourceList, &arbiterListEntry.ListEntry);

#if DBG

                    status =

#endif

                    IopCallArbiter(arbiterEntry,
                                   ArbiterActionTestAllocation,
                                   &arbiterEntry->ResourceList,
                                   NULL,
                                   NULL);

#if DBG

                    PNP_ASSERT(status == STATUS_SUCCESS);
                    status =

#endif

                    IopCallArbiter(arbiterEntry,
                                   ArbiterActionCommitAllocation,
                                   NULL,
                                   NULL,
                                   NULL
                                   );
#if DBG

                    PNP_ASSERT(status == STATUS_SUCCESS);

#endif

                    RemoveEntryList(&arbiterListEntry.ListEntry);
                    InitializeListHead(&arbiterListEntry.ListEntry);
                }
                listEntry = listEntry->Flink;
            }
            device = device->Parent;
        }

        //
        // If there are more than 1 list, move to next list
        //

        if (listCount > 1) {
            cmPartDesc = &cmFullDesc->PartialResourceList.PartialDescriptors[0];
            for (j = 0; j < cmFullDesc->PartialResourceList.Count; j++) {
                size = 0;
                switch (cmPartDesc->Type) {
                case CmResourceTypeDeviceSpecific:
                     size = cmPartDesc->u.DeviceSpecificData.DataSize;
                     break;
                }
                cmPartDesc++;
                cmPartDesc = (PCM_PARTIAL_RESOURCE_DESCRIPTOR) ((PUCHAR)cmPartDesc + size);
            }
            cmFullDesc = (PCM_FULL_RESOURCE_DESCRIPTOR)cmPartDesc;
        }
    }

    IopWriteAllocatedResourcesToRegistry(DeviceNode, NULL, 0);
}

NTSTATUS
PnpAssignResourcesToDevices (
    __in ULONG                                        DeviceCount,
    __inout_ecount(DeviceCount) PPNP_RESOURCE_REQUEST RequestTable,
    __out PLOGICAL                                    RebalancePerformed
    )
/*++

Routine Description:

    This routine takes an input array of PNP_RESOURCE_REQUEST structures, and
    allocates resource for the physical device object specified in
    the structure.   The allocated resources are automatically recorded
    in the registry.

Arguments:

    DeviceCount - Supplies the number of device objects whom we need to
                  allocate resources to.  This is the number of entries in the
                  RequestTable.

    RequestTable - Supplies an array of PNP_RESOURCE_REQUEST structures, each
                   of which contains the Physical Device Object to allocate
                   resources to.

    RebalancePerformed - Supplies a pointer to a boolean that indicates if a
                         rebalance occurred and was successful.

Return Value:

    The status returned is the final completion status of the operation.

    NOTE:
    If NTSTATUS_SUCCESS is returned, the resource allocation for *all* the devices
    specified is succeeded.  Otherwise, one or more are failed and caller must
    examine the ResourceAssignment pointer in each PNP_RESOURCE_REQUEST structure to
    determine which devices failed and which succeeded.

--*/
{
    PDEVICE_NODE deviceNode;
    NTSTATUS status;
    ULONG i;

    PAGED_CODE();

    PNP_ASSERT(DeviceCount != 0);

    for (i = 0; i < DeviceCount; i++) {

        //
        // Initialize table entry.
        //

        deviceNode = (PDEVICE_NODE)(RequestTable[i].PhysicalDevice->DeviceObjectExtension->DeviceNode);

        if (deviceNode->Flags & DNF_MADEUP) {

            ULONG           propertySize = 0;
            DEVPROPTYPE     propertyType = DEVPROP_TYPE_EMPTY;
            DEVPROP_BOOLEAN reportedDevice;

            reportedDevice = DEVPROP_FALSE;

            status = _PnpGetObjectProperty(PiPnpRtlCtx,
                                           PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                           PNP_OBJECT_TYPE_DEVICE,
                                           NULL,
                                           NULL,
                                           &DEVPKEY_Device_Reported,
                                           &propertyType,
                                           (PUCHAR)&reportedDevice,
                                           sizeof(reportedDevice),
                                           &propertySize,
                                           0);

            if ((NT_SUCCESS(status)) &&
                (propertyType == DEVPROP_TYPE_BOOLEAN) &&
                (propertySize == sizeof(reportedDevice)) &&
                (reportedDevice == DEVPROP_TRUE)) {

                //
                // Change the AllocationType for reported devices.
                //

                RequestTable[i].AllocationType = ArbiterRequestLegacyReported;
            }
        }
    }

    //
    // Allocate memory to build a IOP_ASSIGN table to call PnpAllocateResources()
    //
    status = PnpAllocateResources(DeviceCount,
                                  RequestTable,
                                  FALSE,
                                  RebalancePerformed);
    return status;
}

LOGICAL
PnpProcessAssignResources (
   __in   PDEVICE_NODE    DeviceNode,
   __in   LOGICAL         Reallocation,
   __out  LOGICAL        *RebalancePerformed
   )
/*++

Routine Description:

    This function attempts to assign resources to device under the subtree on
    which AddDevice has been performed. Prior to the completion of all Boot Bus
    Extenders in the system, this routine attempts allocation first so that devices
    with no requirements and no boot config get processed. If there are no such devices,
    then it attempts to allocate resources for devices with boot config. If there are no
    devices with boot config, then other devices (requirements but no boot config)
    get processed. During later part of boot, it attempts allocation only once
    (since we should have already reserved all the boot configs).

Parameters:

    DeviceNode - specifies the root of the subtree under which resources will be
                 allocated.

    Reallocation - if TRUE, we will attempt allocation for devices with resource conflict
                   problem in addition to other devices.

    RebalancePerformed - Supplies a pointer to a boolean that indicates if a
                         rebalance occurred and was successful.

Return Value:

    TRUE if resources got assigned to any device, otherwise FALSE.

--*/
{
    LOGICAL resourcesAssigned;
    PPNP_ASSIGN_RESOURCES_CONTEXT context;
    PPNP_RESOURCE_REQUEST requestTable;
    PDEVICE_NODE deviceNode;
    ULONG count, i;

    PAGED_CODE();

    resourcesAssigned = FALSE;
    context = NULL;
    requestTable = NULL;
    //
    // Allocate and init memory for resource context
    //
    context = PnpCreateAssignResourcesContext(Reallocation);
    if (!context) {

        goto cleanup;
    }

    //
    // Collect devices that can be assigned resources.
    //
    PnpProcessAssignResourcesWorker(DeviceNode, context);
    count = context->DeviceCount;
    if (count == 0) {

        goto cleanup;
    }

    //
    // Allocate the resource request table.
    //
    requestTable = PnpCreateResourceRequest(count);
    if (!requestTable) {

        goto cleanup;
    }

    for (i = 0; i < count; i++) {

        PnpInitializeResourceRequest(&requestTable[i],
                                     context->DeviceList[i],
                                     NULL,
                                     ArbiterRequestPnpEnumerated);
    }

    //
    // Assign resources
    //
    PnpAssignResourcesToDevices(count, requestTable, RebalancePerformed);

    //
    // Check the results
    //
    for (i = 0; i < count; i++) {

        deviceNode = PP_DO_TO_DN(requestTable[i].PhysicalDevice);

        if (NT_SUCCESS(requestTable[i].Status)) {

            resourcesAssigned = TRUE;
            if (requestTable[i].ResourceAssignment) {
                IopAcquireResourceListLock(&PiResourceListLock);
                deviceNode->ResourceList = requestTable[i].ResourceAssignment;
                deviceNode->ResourceListTranslated = requestTable[i].TranslatedResourceAssignment;
                IopReleaseResourceListLock(&PiResourceListLock);
            } else {

                PipSetDevNodeFlags(deviceNode, DNF_NO_RESOURCE_REQUIRED);
            }

            PipSetDevNodeState(deviceNode, DeviceNodeResourcesAssigned, NULL);
            PipClearDevNodeUserFlags(deviceNode, DNUF_NEED_RESTART);

        } else {

            //
            // Set resource allocation problem codes based on status code.
            //
            switch (requestTable[i].Status) {

            case STATUS_RETRY:

                break;

            case STATUS_DEVICE_CONFIGURATION_ERROR:

                PipSetDevNodeProblem(deviceNode, CM_PROB_NO_SOFTCONFIG, requestTable[i].Status);
                break;

            case STATUS_PNP_BAD_MPS_TABLE:

                PipSetDevNodeProblem(deviceNode, CM_PROB_BIOS_TABLE, requestTable[i].Status);
                break;

            case STATUS_PNP_TRANSLATION_FAILED:

                PipSetDevNodeProblem(deviceNode, CM_PROB_TRANSLATION_FAILED, requestTable[i].Status);
                break;

            case STATUS_PNP_IRQ_TRANSLATION_FAILED:

                PipSetDevNodeProblem(deviceNode, CM_PROB_IRQ_TRANSLATION_FAILED, requestTable[i].Status);
                break;

            case STATUS_RESOURCE_TYPE_NOT_FOUND:

                PipSetDevNodeProblem(deviceNode, CM_PROB_UNKNOWN_RESOURCE, requestTable[i].Status);
                break;

            case STATUS_BAD_MCFG_TABLE:

                PipSetDevNodeProblem(deviceNode, CM_PROB_BIOS_TABLE, requestTable[i].Status);
                break;

            default:

                PipSetDevNodeProblem(deviceNode, CM_PROB_NORMAL_CONFLICT, requestTable[i].Status);
                break;
            }
        }
    }

cleanup:
    if (requestTable != NULL) {
        PnpDestroyResourceRequest(&requestTable);
    }

    if (context != NULL) {
        PnpDestroyAssignResourcesContext(&context);
    }

    return resourcesAssigned;
}

NTSTATUS
PnpProcessAssignResourcesWorker (
   __in PDEVICE_NODE                    DeviceNode,
   __in PPNP_ASSIGN_RESOURCES_CONTEXT   ResourceContext
   )
/*++

Routine Description:

    This functions searches the DeviceNode subtree to locate all the device objects
    which have been successfully added to their drivers and waiting for resources to
    be started.

Parameters:

    DeviceNode - specifies the device node whose subtree is to be checked for AssignRes.

    Context - specifies a pointer to a structure to pass resource assignment information.

Return Value:

    TRUE.

--*/
{
    PDEVICE_NODE currentNode;
    LOGICAL processingComplete;

    PAGED_CODE();

    processingComplete = FALSE;
    currentNode = DeviceNode;
    do {

        PNP_ASSERT(currentNode->State != DeviceNodeResourcesAssigned);

        if (ResourceContext->IncludeFailedDevices) {

            //
            // If we are reallocating resources, clear appropriate problems.
            //
            if (PipIsDevNodeProblem(currentNode, CM_PROB_NORMAL_CONFLICT) ||
                PipIsDevNodeProblem(currentNode, CM_PROB_TRANSLATION_FAILED) ||
                PipIsDevNodeProblem(currentNode, CM_PROB_IRQ_TRANSLATION_FAILED)) {

                PipClearDevNodeProblem(currentNode);
            }
        }

        if (!PipDoesDevNodeHaveProblem(currentNode)) {

            //
            // If drivers are added for this device, add it to our list.
            //
            if (currentNode->State == DeviceNodeDriversAdded) {

                ResourceContext->DeviceList[ResourceContext->DeviceCount] =
                    currentNode->PhysicalDeviceObject;
                ResourceContext->DeviceCount++;
            }
        }

        if (currentNode->Child) {

            currentNode = currentNode->Child;
        } else {

            for (;;) {

                if (currentNode == DeviceNode) {

                    processingComplete = TRUE;
                    break;
                }

                if (currentNode->Sibling) {

                    currentNode = currentNode->Sibling;
                    break;
                }

                if (currentNode->Parent) {

                    currentNode = currentNode->Parent;
                }
            }
        }

    } while (!processingComplete);

    return STATUS_SUCCESS;
}

NTSTATUS
PnpRestoreResourcesInternal (
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This routine reassigns the released resources for device specified by DeviceNode.

Parameters:

    DeviceNode - specifies the device node whose resources are goint to be released.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    PNP_RESOURCE_REQUEST requestTable;
    NTSTATUS status;
    LIST_ENTRY  activeArbiterList;
    PIO_RESOURCE_REQUIREMENTS_LIST requirements;

    if (DeviceNode->ResourceList == NULL) {

        return STATUS_SUCCESS;
    }

    requirements = PnpCmResourcesToIoResources(0,
                                               DeviceNode->ResourceList,
                                               LCPRI_FORCECONFIG);

    if (requirements == NULL) {

        IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                     "Not enough memory to clean up rebalance failure\n"));

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    PnpInitializeResourceRequest(&requestTable,
                                 DeviceNode->PhysicalDeviceObject,
                                 requirements,
                                 ArbiterRequestPnpEnumerated);

    //
    // rebuild internal representation of the resource requirements list
    //

    status = IopResourceRequirementsListToReqList(&requestTable,
                                                  &requestTable.ReqList);

    if (!NT_SUCCESS(status) || requestTable.ReqList == NULL) {

        IopDbgPrint((IOP_RESOURCE_ERROR_LEVEL,
                     "Not enough memory to restore previous resources\n"));

        ExFreePool (requestTable.ResourceRequirements);
        return status;

    } else {

        PREQ_LIST reqList;

        reqList = (PREQ_LIST)requestTable.ReqList;

        //
        // Sort the ReqList such that the higher priority Alternative list are
        // placed in the front of the list.
        //

        IopRearrangeReqList(reqList);
        if (reqList->BestAlternative == NULL) {

            PnpFreeResourceRequirementsForAssignTable(&requestTable,
                                                      (&requestTable) + 1);
            return STATUS_DEVICE_CONFIGURATION_ERROR;

        }
    }

    status = PnpFindBestConfiguration(&requestTable, 1, &activeArbiterList);
    PnpFreeResourceRequirementsForAssignTable(&requestTable, (&requestTable) + 1);
    if (NT_SUCCESS(status)) {
        //
        // Ask the arbiters to commit this configuration.
        //
        status = IopCommitConfiguration(&activeArbiterList);
    }
    if (!NT_SUCCESS(status)) {
        IopDbgPrint((
            IOP_RESOURCE_ERROR_LEVEL,
            "PnpRestoreResourcesInternal: BOOT conflict for %wZ\n",
            PnpGetDeviceInstancePath(DeviceNode)));
    }
    if (requestTable.ResourceAssignment) {
        ExFreePool(requestTable.ResourceAssignment);
    }
    if (requestTable.TranslatedResourceAssignment) {
        ExFreePool(requestTable.TranslatedResourceAssignment);
    }
    IopWriteAllocatedResourcesToRegistry(DeviceNode,
                                         DeviceNode->ResourceList,
                                         PnpDetermineResourceListSize(DeviceNode->ResourceList));
    return status;
}

VOID
PnpReallocateResources (
    __in PDEVICE_NODE DeviceNode
    )
/*++

Routine Description:

    This routine performs the real work for IoInvalidateDeviceState - ResourceRequirementsChanged.

Arguments:

    DeviceNode - Supplies a pointer to the device node.

Return Value:

    None.

--*/
{
    PNP_RESOURCE_REQUEST requestTable;
    PPNP_RESOURCE_REQUEST requestTablep;
    ULONG deviceCount, oldFlags;
    NTSTATUS status;
    LIST_ENTRY  activeArbiterList;

    PAGED_CODE();

    //
    // Grab the IO registry semaphore to make sure no other device is
    // reporting it's resource usage while we are searching for conflicts.
    //

    IopLockResourceManager();

    //
    // Check the flags after acquiring the semaphore.
    //

    if (DeviceNode->Flags & DNF_RESOURCE_REQUIREMENTS_CHANGED) {

        //
        // Save the flags which we may have to restore in case of failure.
        //

        oldFlags = DeviceNode->Flags & DNF_NO_RESOURCE_REQUIRED;
        PipClearDevNodeFlags(DeviceNode, DNF_NO_RESOURCE_REQUIRED);
        if (DeviceNode->Flags & DNF_NON_STOPPED_REBALANCE) {

            //
            // Set up parameters to call real routine
            //

            PnpInitializeResourceRequest(&requestTable,
                                         DeviceNode->PhysicalDeviceObject,
                                         NULL,
                                         ArbiterRequestPnpEnumerated);

            requestTable.Flags |= IOP_ASSIGN_NO_REBALANCE + IOP_ASSIGN_KEEP_CURRENT_CONFIG;
            requestTablep = &requestTable;
            status = PnpGetResourceRequirementsForAssignTable(requestTablep,
                                                              requestTablep + 1,
                                                              &deviceCount);

            if (deviceCount) {

                //
                // Release the current resources to the arbiters.
                // Memory for ResourceList is not released.
                //

                if (DeviceNode->ResourceList) {
                    PnpReleaseResourcesInternal(DeviceNode);
                }

                //
                // Try to do the assignment.
                //

                status = PnpFindBestConfiguration(
                            requestTablep,
                            deviceCount,
                            &activeArbiterList);

                if (NT_SUCCESS(status)) {

                    //
                    // Ask the arbiters to commit this configuration.
                    //

                    status = IopCommitConfiguration(&activeArbiterList);
                }

                if (NT_SUCCESS(status)) {
                    PipClearDevNodeFlags(DeviceNode, DNF_RESOURCE_REQUIREMENTS_CHANGED | DNF_NON_STOPPED_REBALANCE);
                    PnpBuildCmResourceLists(requestTablep, requestTablep + 1, TRUE);
                    PNP_ASSERT(DeviceNode->State == DeviceNodeStarted);
                    status = PnpStartDeviceNode(DeviceNode,
                                                TRUE,
                                                TRUE);

                    if (!NT_SUCCESS(status)) {
                        PnpRequestDeviceRemoval(DeviceNode,
                                                FALSE,
                                                CM_PROB_NORMAL_CONFLICT,
                                                status);
                    }

                } else {
                    NTSTATUS restoreResourcesStatus;
                    restoreResourcesStatus = PnpRestoreResourcesInternal(DeviceNode);
                    if (!NT_SUCCESS(restoreResourcesStatus)) {
                        PNP_ASSERT(NT_SUCCESS(restoreResourcesStatus));
                        PnpRequestDeviceRemoval(DeviceNode,
                                                FALSE,
                                                CM_PROB_NEED_RESTART,
                                                restoreResourcesStatus);
                    }
                }

                PnpFreeResourceRequirementsForAssignTable(requestTablep, requestTablep + 1);
            }

        } else {

            //
            // The device needs to be stopped to change resources.
            //

            status = PnpRebalance(DeviceNode, NULL, 0, FALSE);
        }

        //
        // Restore the flags in case of failure.
        //

        if (!NT_SUCCESS(status)) {
            PipClearDevNodeFlags(DeviceNode, DNF_NO_RESOURCE_REQUIRED);
            PipSetDevNodeFlags(DeviceNode, oldFlags);
        }

    } else {
        IopDbgPrint((
            IOP_RESOURCE_ERROR_LEVEL,
            "Resource requirements not changed in "
            "PnpReallocateResources, returning error!\n"));
    }

    IopUnlockResourceManager();
}

VOID
PnpBuildCmResourceList (
    __inout PPNP_RESOURCE_REQUEST AssignEntry,
    __in LOGICAL PublishAsResourceMap
    )
/*++

Routine Description:

    This routine walks REQ_LIST of the AssignEntry to build a corresponding
    Cm Resource lists.  It also reports the resources to ResourceMap.

Parameters:

    AssignEntry - Supplies a pointer to an IOP_ASSIGN_REQUEST structure

    PublishAsResourceMap - provides a boolean to indicate whether to publish
        this to the registry or not.

Return Value:

    None.  The ResourceAssignment in AssignEntry is initialized.

--*/

{
    NTSTATUS status;
    HANDLE resourceMapKey;
    PDEVICE_OBJECT physicalDevice;
    PREQ_LIST reqList = AssignEntry->ReqList;
    PREQ_ALTERNATIVE reqAlternative;
    PREQ_DESC reqDesc, reqDescx;
    PIO_RESOURCE_DESCRIPTOR privateData;
    ULONG count = 0, size, i;
    PCM_RESOURCE_LIST cmResources, cmResourcesRaw;
    PCM_FULL_RESOURCE_DESCRIPTOR cmFullResource, cmFullResourceRaw;
    PCM_PARTIAL_RESOURCE_LIST cmPartialList, cmPartialListRaw;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR cmDescriptor, cmDescriptorRaw, assignment, tAssignment;

#if DBG

    PCM_PARTIAL_RESOURCE_DESCRIPTOR cmDescriptorEnd, cmDescriptorEndRaw;

#endif

    PAGED_CODE();

    //
    // Determine the size of the CmResourceList
    //
    reqAlternative = *reqList->SelectedAlternative;
    for (i = 0; i < reqAlternative->DescCount; i++) {

        reqDesc = reqAlternative->DescTable[i];
        count += reqDesc->DevicePrivateCount + 1;
    }

    PNP_ASSERT(count != 0);
    if (count == 0) {
        AssignEntry->Status = STATUS_INTERNAL_ERROR;
        AssignEntry->ResourceAssignment = NULL;
        AssignEntry->TranslatedResourceAssignment = NULL;
        return;
    }

    size = sizeof(CM_RESOURCE_LIST) + sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR) * (count - 1);
    cmResources = (PCM_RESOURCE_LIST) ExAllocatePool(PagedPool, size);
    if (!cmResources) {
        //
        // If we can not find memory, the resources will not be committed by arbiter.
        //
        IopDbgPrint((
            IOP_RESOURCE_WARNING_LEVEL,
            "Not enough memory to build Translated CmResourceList\n"));

        AssignEntry->Status = STATUS_INSUFFICIENT_RESOURCES;
        AssignEntry->ResourceAssignment = NULL;
        AssignEntry->TranslatedResourceAssignment = NULL;
        return;
    }
    cmResourcesRaw = (PCM_RESOURCE_LIST) ExAllocatePool(PagedPool, size);
    if (!cmResourcesRaw) {

        IopDbgPrint((
            IOP_RESOURCE_WARNING_LEVEL,
            "Not enough memory to build Raw CmResourceList\n"));

        ExFreePool(cmResources);
        AssignEntry->Status = STATUS_INSUFFICIENT_RESOURCES;
        AssignEntry->ResourceAssignment = NULL;
        AssignEntry->TranslatedResourceAssignment = NULL;
        return;
    }
    cmResources->Count = 1;
    cmFullResource = cmResources->List;
    //
    // The CmResourceList we build here does not distinguish the
    // Interface Type on descriptor level.  This should be fine because
    // for IoReportResourceUsage we ignore the CmResourceList we build
    // here.
    //
    cmFullResource->InterfaceType = reqList->InterfaceType;
    cmFullResource->BusNumber = reqList->BusNumber;
    cmPartialList = &cmFullResource->PartialResourceList;
    cmPartialList->Version = 1;
    cmPartialList->Revision = 1;
    cmPartialList->Count = count;
    cmDescriptor = cmPartialList->PartialDescriptors;

#if DBG

    cmDescriptorEnd = cmDescriptor + count;

#endif

    cmResourcesRaw->Count = 1;
    cmFullResourceRaw = cmResourcesRaw->List;
    cmFullResourceRaw->InterfaceType = reqList->InterfaceType;
    cmFullResourceRaw->BusNumber = reqList->BusNumber;
    cmPartialListRaw = &cmFullResourceRaw->PartialResourceList;
    cmPartialListRaw->Version = 1;
    cmPartialListRaw->Revision = 1;
    cmPartialListRaw->Count = count;
    cmDescriptorRaw = cmPartialListRaw->PartialDescriptors;

#if DBG

    cmDescriptorEndRaw = cmDescriptorRaw + count;

#endif

    for (i = 0; i < reqAlternative->DescCount; i++) {

        reqDesc = reqAlternative->DescTable[i];

        if (reqDesc->ArbitrationRequired) {
            //
            // Get raw assignment and copy it to our raw resource list
            //
            reqDescx = reqDesc->TranslatedReqDesc;
            if (reqDescx->AlternativeTable.Result != ArbiterResultNullRequest) {

                status = IopParentToRawTranslation(reqDescx);
                if (!NT_SUCCESS(status)) {

                    IopDbgPrint((
                        IOP_RESOURCE_WARNING_LEVEL,
                        "Parent To Raw translation failed\n"));
                    ExFreePool(cmResources);
                    ExFreePool(cmResourcesRaw);
                    if (status == STATUS_RETRY) {
                        AssignEntry->Status = status;

                    } else {
                        AssignEntry->Status = STATUS_INSUFFICIENT_RESOURCES;
                    }

                    AssignEntry->ResourceAssignment = NULL;
                    return;
                }
                assignment = reqDesc->AlternativeTable.Assignment;
            } else {
                assignment = reqDescx->AlternativeTable.Assignment;
            }
            *cmDescriptorRaw = *assignment;
            cmDescriptorRaw++;

            //
            // Translate assignment and copy it to our translated resource list
            //
            if (reqDescx->AlternativeTable.Result != ArbiterResultNullRequest) {
                status = IopChildToRootTranslation(
                            PP_DO_TO_DN(reqDesc->AlternativeTable.PhysicalDeviceObject),
                            reqDesc->InterfaceType,
                            reqDesc->BusNumber,
                            reqDesc->AlternativeTable.RequestSource,
                            &reqDesc->Allocation,
                            &tAssignment
                            );
                if (!NT_SUCCESS(status)) {
                    IopDbgPrint((
                        IOP_RESOURCE_WARNING_LEVEL,
                        "Child to Root translation failed\n"));
                    ExFreePool(cmResources);
                    ExFreePool(cmResourcesRaw);
                    if (status == STATUS_RETRY) {
                        AssignEntry->Status = status;

                    } else {
                        AssignEntry->Status = STATUS_INSUFFICIENT_RESOURCES;
                    }

                    AssignEntry->ResourceAssignment = NULL;
                    return;
                }
                *cmDescriptor = *tAssignment;
                ExFreePool(tAssignment);
            } else {
                *cmDescriptor = *(reqDescx->AlternativeTable.Assignment);
            }
            cmDescriptor++;

        } else {
            *cmDescriptorRaw = reqDesc->Allocation;
            *cmDescriptor = reqDesc->Allocation;
            cmDescriptorRaw++;
            cmDescriptor++;
        }

        //
        // Next copy the device private descriptors to CmResourceLists
        //

        count = reqDesc->DevicePrivateCount;
        privateData = reqDesc->DevicePrivate;
        while (count != 0) {

            cmDescriptor->Type = cmDescriptorRaw->Type = CmResourceTypeDevicePrivate;
            cmDescriptor->ShareDisposition = cmDescriptorRaw->ShareDisposition =
                         CmResourceShareDeviceExclusive;
            cmDescriptor->Flags = cmDescriptorRaw->Flags = privateData->Flags;
            RtlMoveMemory(&cmDescriptorRaw->u.DevicePrivate,
                          &privateData->u.DevicePrivate,
                          sizeof(cmDescriptorRaw->u.DevicePrivate.Data)
                          );
            RtlMoveMemory(&cmDescriptor->u.DevicePrivate,
                          &privateData->u.DevicePrivate,
                          sizeof(cmDescriptor->u.DevicePrivate.Data)
                          );
            privateData++;
            cmDescriptorRaw++;
            cmDescriptor++;
            count--;
            PNP_ASSERT(cmDescriptorRaw <= cmDescriptorEndRaw);
            PNP_ASSERT(cmDescriptor <= cmDescriptorEnd);
        }
        PNP_ASSERT(cmDescriptor <= cmDescriptorEnd);
        PNP_ASSERT(cmDescriptorRaw <= cmDescriptorEndRaw);

    }

    //
    // report assigned resources to ResourceMap
    //

    if (PublishAsResourceMap != FALSE) {

        physicalDevice = AssignEntry->PhysicalDevice;

        //
        // Open ResourceMap key
        //

        status = IopCreateRegistryKeyEx( &resourceMapKey,
                                        (HANDLE) NULL,
                                        &CmRegistryMachineHardwareResourceMapName,
                                        KEY_READ | KEY_WRITE,
                                        REG_OPTION_VOLATILE,
                                        NULL
                                        );
        if (NT_SUCCESS(status )) {
            WCHAR DeviceBuffer[256];
            POBJECT_NAME_INFORMATION NameInformation;
            ULONG NameLength;
            UNICODE_STRING UnicodeClassName;
            UNICODE_STRING UnicodeDriverName;
            UNICODE_STRING UnicodeDeviceName;

            PiWstrToUnicodeString(&UnicodeClassName, PNPMGR_STR_PNP_MANAGER);

            PiWstrToUnicodeString(&UnicodeDriverName, REGSTR_KEY_PNP_DRIVER);

            NameInformation = (POBJECT_NAME_INFORMATION) DeviceBuffer;
            status = ObQueryNameString( physicalDevice,
                                        NameInformation,
                                        sizeof( DeviceBuffer ),
                                        &NameLength );
            if (NT_SUCCESS(status)) {
                NameInformation->Name.MaximumLength = sizeof(DeviceBuffer) - sizeof(OBJECT_NAME_INFORMATION);
                if (NameInformation->Name.Length == 0) {
                    NameInformation->Name.Buffer = (PVOID)((ULONG_PTR)DeviceBuffer + sizeof(OBJECT_NAME_INFORMATION));
                }

                UnicodeDeviceName = NameInformation->Name;
                RtlAppendUnicodeToString(&UnicodeDeviceName, PnpWstrRaw);

                //
                // IopWriteResourceList should remove all the device private and device
                // specifiec descriptors.
                //

                status = IopWriteResourceList(
                            resourceMapKey,
                            &UnicodeClassName,
                            &UnicodeDriverName,
                            &UnicodeDeviceName,
                            cmResourcesRaw,
                            size
                            );
                if (NT_SUCCESS(status)) {
                    UnicodeDeviceName = NameInformation->Name;
                    RtlAppendUnicodeToString (&UnicodeDeviceName, PnpWstrTranslated);
                    status = IopWriteResourceList(
                                resourceMapKey,
                                &UnicodeClassName,
                                &UnicodeDriverName,
                                &UnicodeDeviceName,
                                cmResources,
                                size
                                );
                }
            }
            ZwClose(resourceMapKey);
        }
    }

    AssignEntry->ResourceAssignment = cmResourcesRaw;
    AssignEntry->TranslatedResourceAssignment = cmResources;
}

NTSTATUS
PnpAllocateResources (
    __in ULONG                                           RequestCount,
    __inout_ecount(RequestCount) PPNP_RESOURCE_REQUEST   Request,
    __in LOGICAL                                         ResourceManagerLocked,
    __out_opt PLOGICAL                                   RebalancePerformed
    )

/*++

Routine Description:

    For each PNP_RESOURCE_REQUEST, this routine queries the device's IO
    Resource Requirements List, converts it to our internal REQ_LIST format and
    then calls a worker routine to perform resource assignment.

Parameters:

    RequestCount - Supplies the number of device objects whom we need to
                   allocate resources to.  This is the number of entries in the
                   RequestTable.

    Request - Supplies an array of PNP_RESOURCE_REQUEST structures, each of
              which contains the Physical Device Object to allocate resources
              to.

    ResourceManagerLocked - Indicates whether the PpRegistrySemaphore was
                            acquired by the caller.

    RebalancePerformed - Supplies a pointer to a boolean that indicates if a
                         rebalance occurred and was successful.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    NTSTATUS                status;
    PPNP_RESOURCE_REQUEST   requestTable;
    PPNP_RESOURCE_REQUEST   requestTableEnd;
    ULONG                   deviceCount;
    PPNP_RESOURCE_REQUEST   requestEntry;
    LIST_ENTRY              activeArbiterList;
    LOGICAL                 processingComplete;
    PDEVICE_NODE            deviceNode;
    LOGICAL                 attemptGroup;
    LOGICAL                 resourcesAssigned;

    PAGED_CODE();

    deviceCount = 0;

    if (ARGUMENT_PRESENT(RebalancePerformed)) {
        *RebalancePerformed = FALSE;
    }

    //
    // Lock the resource manager if the caller has not locked already.
    // This is to serialize allocations and releases of resources from the
    // arbiters.
    //

    if (!ResourceManagerLocked) {
        IopLockResourceManager();
    }

    requestTable    = Request;
    deviceCount = RequestCount;
    requestTableEnd = requestTable + deviceCount;
    status = PnpGetResourceRequirementsForAssignTable(requestTable,
                requestTableEnd,
                &deviceCount);

    if (NT_SUCCESS(status)) {

        PNP_ASSERT(deviceCount != 0);

        attemptGroup = TRUE;

        processingComplete = FALSE;
        if (!IopBootConfigsReserved) {

            //
            // STEP#1: Only process devices with no requirements.
            //
            for (requestEntry = requestTable; requestEntry < requestTableEnd; requestEntry++) {

                if (NT_SUCCESS(requestEntry->Status) &&
                    requestEntry->ResourceRequirements == NULL) {

                    processingComplete = TRUE;
                    break;
                }
            }

            if (requestEntry != requestTableEnd) {

                //
                // We found at least one device with no resource requirements.
                //
                for (requestEntry = requestTable; requestEntry < requestTableEnd; requestEntry++) {

                    deviceNode = PP_DO_TO_DN(requestEntry->PhysicalDevice);

                    if (NT_SUCCESS(requestEntry->Status) &&
                        requestEntry->ResourceRequirements == NULL) {
                        IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                                     "PnpAllocateResources: Processing device %wZ with no resource requirements\n",
                                     PnpGetDeviceInstancePath(deviceNode)));

                    } else {

                        IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                                     "PnpAllocateResources: Ignoring resource consuming device %wZ\n",
                                     PnpGetDeviceInstancePath(deviceNode)));
                        requestEntry->Flags |= IOP_ASSIGN_IGNORE;
                        requestEntry->Status = STATUS_RETRY;
                    }
                }
            }

            if (processingComplete) {

                goto cleanup;
            }
        }

        //
        // STEP#2: All devices are resource consuming. Only process devices
        // with boot config.
        //
        for (requestEntry = requestTable; requestEntry < requestTableEnd; requestEntry++) {

            deviceNode = PP_DO_TO_DN(requestEntry->PhysicalDevice);

            if ((deviceNode->Flags & DNF_HAS_BOOT_CONFIG) &&
                requestEntry->ResourceRequirements) {

                attemptGroup = FALSE;
                break;
            }
        }

        if (requestEntry != requestTableEnd) {

            //
            // There is at least one device with boot config. Process only the ones with
            // boot config.
            //
            for (requestEntry = requestTable; requestEntry < requestTableEnd; requestEntry++) {

                deviceNode = PP_DO_TO_DN(requestEntry->PhysicalDevice);
                if ((deviceNode->Flags & DNF_HAS_BOOT_CONFIG) &&
                    requestEntry->ResourceRequirements) {

                    IopDbgPrint((
                        IOP_RESOURCE_INFO_LEVEL,
                        "Processing BOOT config device %wZ...\n",
                        PnpGetDeviceInstancePath(deviceNode))
                        );
                } else {

                    //
                    // Mark everyone with STATUS_RETRY except devices with no
                    // requirements.
                    //
                    if (!(requestEntry->Flags & IOP_ASSIGN_IGNORE)) {

                        requestEntry->Flags |= IOP_ASSIGN_IGNORE;
                        requestEntry->Status = STATUS_RETRY;
                        deviceCount--;
                    }
                }
            }
        }

        //
        // STEP#3: Process requests not marked as to be ignored.
        //
        if (!deviceCount) {

            status = STATUS_UNSUCCESSFUL;
            goto cleanup;
        }

        if (deviceCount != RequestCount) {

            //
            // Move the uninteresting devices to the end of the table.
            //
            for (requestEntry = requestTable; requestEntry < requestTableEnd; ) {

                PNP_RESOURCE_REQUEST temp;

                deviceNode = PP_DO_TO_DN(requestEntry->PhysicalDevice);

                if (!(requestEntry->Flags & IOP_ASSIGN_IGNORE)) {

                    IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                                 "Processing resource consuming device %wZ...\n",
                                 PnpGetDeviceInstancePath(deviceNode)));
                    requestEntry++;
                    continue;
                }
                temp = *requestEntry;
                *requestEntry = *(requestTableEnd - 1);
                *(requestTableEnd - 1) = temp;
                requestTableEnd--;
            }
        }
        PNP_ASSERT((ULONG)(requestTableEnd - requestTable) == deviceCount);
        PNP_ASSERT(deviceCount <= RequestCount);
        __analysis_assume(deviceCount <= RequestCount);

        //
        // Sort the AssignTable
        //
        PnpRearrangeAssignTable(requestTable, deviceCount);

        if (attemptGroup) {

            status = PnpFindBestConfiguration(requestTable, deviceCount, &activeArbiterList);
        } else {

            status = STATUS_UNSUCCESSFUL;
        }
        if (!NT_SUCCESS(status)) {

            //
            // Try one device at a time.
            //
            resourcesAssigned = FALSE;
            for (requestEntry = requestTable; requestEntry < requestTableEnd; requestEntry++) {

                deviceNode = PP_DO_TO_DN(requestEntry->PhysicalDevice);
                IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                             "Trying to allocate resources for %wZ.\n",
                             PnpGetDeviceInstancePath(deviceNode)));

                status = PnpFindBestConfiguration(requestEntry, 1, &activeArbiterList);
                if (NT_SUCCESS(status)) {

                    //
                    // Ask the arbiters to commit this configuration.
                    //
                    resourcesAssigned = TRUE;
                    status = IopCommitConfiguration(&activeArbiterList);
                    if (NT_SUCCESS(status)) {

                        PnpBuildCmResourceLists(requestEntry, requestEntry + 1, FALSE);

                    } else {

                        requestEntry->Status = STATUS_CONFLICTING_ADDRESSES;
                    }

                } else if (status == STATUS_INSUFFICIENT_RESOURCES) {

                    IopDbgPrint((
                        IOP_RESOURCE_WARNING_LEVEL,
                        "IopAllocateResource: Failed to allocate Pool.\n"));
                    break;

                } else {

                    //
                    // Attempt rebalance if we have not assigned resources to any device
                    // yet in this group and its ok to rebalance.
                    //
                    if (!resourcesAssigned && !(requestTable->Flags & IOP_ASSIGN_NO_REBALANCE)) {

                        IopDbgPrint((IOP_RESOURCE_INFO_LEVEL, "PnpAllocateResources: Initiating REBALANCE...\n"));

                        PipSetDevNodeFlags(deviceNode, DNF_NEEDS_REBALANCE);
                        status = PnpRebalance(deviceNode, requestEntry, 1, FALSE);
                        PipClearDevNodeFlags(deviceNode, DNF_NEEDS_REBALANCE);
                        if (!NT_SUCCESS(status)) {

                            //
                            // If the conflict was with the MCFG range, and a rebalance
                            // could not move the device to a different location, then
                            // make sure this error persists so the correct problem code
                            // is assigned.
                            //

                            if (status == STATUS_BAD_MCFG_TABLE) {
                                requestEntry->Status = status;
                            } else {
                                requestEntry->Status = STATUS_CONFLICTING_ADDRESSES;
                            }
                        } else {
                            if (ARGUMENT_PRESENT(RebalancePerformed)) {
                                *RebalancePerformed = TRUE;
                            }

                            //
                            // Since we have assigned resources during rebalance, we need
                            // to restart the stopped devices before we assign any new
                            // resources.
                            // We move to the next entry since this one has succeeded and
                            // we dont want it to be tagged for retry.
                            //
                            requestEntry++;
                            break;
                        }
                    } else {

                        requestEntry->Flags |= IOP_ASSIGN_IGNORE;
                        requestEntry->Status = STATUS_RETRY;
                    }
                }
            }

            //
            // If we ran out of memory, then set the appropriate status
            // on remaining devices. On success, set STATUS_RETRY on the
            // rest so we will attempt allocation again after the current
            // device is started.
            //
            for (; requestEntry < requestTableEnd; requestEntry++) {

                if (status == STATUS_INSUFFICIENT_RESOURCES) {

                    requestEntry->Status = STATUS_INSUFFICIENT_RESOURCES;
                } else {

                    requestEntry->Status = STATUS_RETRY;
                    requestEntry->Flags |= IOP_ASSIGN_IGNORE;
                }
            }

        } else {

            //
            // Ask the arbiters to commit this configuration.
            //
            status = IopCommitConfiguration(&activeArbiterList);
            for (requestEntry = requestTable;
                 requestEntry < requestTableEnd;
                 requestEntry++) {

                if (NT_SUCCESS(status)) {

                    PnpBuildCmResourceLists(requestEntry, requestEntry + 1, FALSE);

                } else {

                    requestEntry->Status = STATUS_CONFLICTING_ADDRESSES;
                }
            }
        }

        IopReleaseFilteredBootResources(requestTable, requestTableEnd);

cleanup:
        //
        // Cleanup.
        //
        PnpFreeResourceRequirementsForAssignTable(requestTable, requestTableEnd);
    }

    //
    // Unlock on the way out if we locked.
    //
    if (!ResourceManagerLocked) {

        IopUnlockResourceManager();
    }

    return status;
}

int
__cdecl
PnpCompareResourceRequestPriority (
    const void *arg1,
    const void *arg2
    )

/*++

    This function is used in C run time sort. It compares the priority of
    PNP_RESOURCE_REQUEST in arg1 and arg2.

Parameters:

    arg1    - LHS PPNP_RESOURCE_REQUEST

    arg2    - RHS PPNP_RESOURCE_REQUEST

Return Value:

    < 0 if arg1 < arg2
    = 0 if arg1 = arg2
    > 0 if arg1 > arg2

--*/

{
    PPNP_RESOURCE_REQUEST rr1 = (PPNP_RESOURCE_REQUEST)arg1;
    PPNP_RESOURCE_REQUEST rr2 = (PPNP_RESOURCE_REQUEST)arg2;

    PAGED_CODE();

    if (rr1->Priority == rr2->Priority) {

        if (rr1->Position > rr2->Position) {

            return 1;
        } else if (rr1->Position < rr2->Position) {

            return -1;
        } else {

            PNP_ASSERT(0);
            if ((ULONG_PTR)rr1 < (ULONG_PTR)rr2) {

                return -1;
            } else {

                return 1;
            }
        }
    }
    if (rr1->Priority > rr2->Priority) {

        return 1;
    } else {

        return -1;
    }
}

VOID
PnpRearrangeAssignTable (
    __in_ecount(Count) PPNP_RESOURCE_REQUEST    RequestTable,
    __in ULONG                                  Count
    )

/*++

Routine Description:

    This routine sorts the resource requirements table in ascending priority
    order.

Parameters:

    RequestTable    - Table of resources requests to be sorted.

    Count           - Length of the RequestTable.

Return Value:

    None.

--*/

{
    ULONG   i;

    PAGED_CODE();

    if (Count > 1) {

        for (i = 0; i < Count; i++) {

            RequestTable[i].Position = i;
        }
        qsort((void *)RequestTable,
              Count,
              sizeof(PNP_RESOURCE_REQUEST),
              PnpCompareResourceRequestPriority);
    }
}

#if DBG

VOID
PnpDumpResourceRequirementsList (
    __in PIO_RESOURCE_REQUIREMENTS_LIST IoResources
    )

/*++

Routine Description:

    This routine dumps IoResources

Parameters:

    IoResources - Supplies a pointer to the IO resource requirements list

Return Value:

    None.

--*/

{
    PIO_RESOURCE_LIST       IoResourceList;
    PIO_RESOURCE_DESCRIPTOR IoResourceDescriptor;
    PIO_RESOURCE_DESCRIPTOR IoResourceDescriptorEnd;
    LONG                    IoResourceListCount;

    PAGED_CODE();

    if (IoResources == NULL) {

        return;
    }
    IoResourceList      = IoResources->List;
    IoResourceListCount = (LONG) IoResources->AlternativeLists;
    IopDbgPrint((
        IOP_RESOURCE_VERBOSE_LEVEL,
        "ResReqList: Interface: %x, Bus: %x, Slot: %x, AlternativeLists: %x\n",
         IoResources->InterfaceType,
         IoResources->BusNumber,
         IoResources->SlotNumber,
         IoResources->AlternativeLists));
    while (--IoResourceListCount >= 0) {

        IopDbgPrint((
            IOP_RESOURCE_VERBOSE_LEVEL,
            "  Alternative List: DescCount: %x\n",
            IoResourceList->Count));
        IoResourceDescriptor = IoResourceList->Descriptors;
        IoResourceDescriptorEnd = IoResourceDescriptor + IoResourceList->Count;
        while(IoResourceDescriptor < IoResourceDescriptorEnd) {

            PnpDumpResourceDescriptor("    ", IoResourceDescriptor++);
        }
        IoResourceList = (PIO_RESOURCE_LIST) IoResourceDescriptorEnd;
    }
    IopDbgPrint((IOP_RESOURCE_VERBOSE_LEVEL,"\n"));
}

VOID
PnpDumpResourceDescriptor (
    __in PCHAR Indent,
    __in PIO_RESOURCE_DESCRIPTOR  Desc
    )
{
    PAGED_CODE();

    IopDbgPrint((
        IOP_RESOURCE_VERBOSE_LEVEL,
        "%sOpt: %x, Share: %x\t",
        Indent,
        Desc->Option,
        Desc->ShareDisposition));
    switch (Desc->Type) {
    case CmResourceTypePort:
    case CmResourceTypeMemory:
    case CmResourceTypeMemoryLarge: {
        ULONGLONG DecodedLength;
        ULONGLONG DecodedAlignment;
        ULONGLONG MinimumAddress;
        ULONGLONG MaximumAddress;

        DecodedLength = RtlIoDecodeMemIoResource(Desc,
                                                 &DecodedAlignment,
                                                 &MinimumAddress,
                                                 &MaximumAddress);
        IopDbgPrint((
            IOP_RESOURCE_VERBOSE_LEVEL,
            "%s Min: %x:%08x, Max: %x:%08x, Algn: %I64x, Len %I64x\n",
            Desc->Type == CmResourceTypePort ? "IO" : "MEM",
            (ULONG)(MinimumAddress >> 32),
            (ULONG)(MinimumAddress),
            (ULONG)(MaximumAddress >> 32),
            (ULONG)(MaximumAddress),
            DecodedAlignment,
            DecodedLength));
            break;
    }

    case CmResourceTypeInterrupt:

        IopDbgPrint((
            IOP_RESOURCE_VERBOSE_LEVEL,
            "INT Min: %x, Max: %x\n",
            Desc->u.Interrupt.MinimumVector,
            Desc->u.Interrupt.MaximumVector));
            break;

    case CmResourceTypeDma:

        IopDbgPrint((
            IOP_RESOURCE_VERBOSE_LEVEL,
            "DMA Min: %x, Max: %x\n",
            Desc->u.Dma.MinimumChannel,
            Desc->u.Dma.MaximumChannel));
            break;

    case CmResourceTypeDevicePrivate:

        IopDbgPrint((
            IOP_RESOURCE_VERBOSE_LEVEL,
            "DevicePrivate Data: %x, %x, %x\n",
            Desc->u.DevicePrivate.Data[0],
            Desc->u.DevicePrivate.Data[1],
            Desc->u.DevicePrivate.Data[2]));
            break;

    default:

        IopDbgPrint((
            IOP_RESOURCE_VERBOSE_LEVEL,
            "Unknown Descriptor type %x\n",
            Desc->Type));
            break;
    }
}

VOID
PnpDumpCmResourceList (
    __in PCM_RESOURCE_LIST CmList
    )
/*++

Routine Description:

    This routine displays CM resource list.

Arguments:

    CmList - CM resource list to be dumped.

Return Value:

    None.

--*/
{
    PCM_FULL_RESOURCE_DESCRIPTOR    fullDesc;
    PCM_PARTIAL_RESOURCE_LIST       partialDesc;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR desc;
    ULONG                           count;
    ULONG                           i;

    PAGED_CODE();

    if (CmList->Count > 0) {

        if (CmList) {

            fullDesc = &CmList->List[0];
            IopDbgPrint((
                IOP_RESOURCE_VERBOSE_LEVEL,
                "Cm Resource List -\n"));
            IopDbgPrint((
                IOP_RESOURCE_VERBOSE_LEVEL,
                "  List Count = %x, Bus Number = %x\n",
                CmList->Count,
                fullDesc->BusNumber));
            partialDesc = &fullDesc->PartialResourceList;
            IopDbgPrint((
                IOP_RESOURCE_VERBOSE_LEVEL,
                "  Version = %x, Revision = %x, Desc count = %x\n",
                partialDesc->Version,
                partialDesc->Revision,
                partialDesc->Count));
            count = partialDesc->Count;
            desc = &partialDesc->PartialDescriptors[0];
            for (i = 0; i < count; i++) {

                PnpDumpCmResourceDescriptor("    ", desc);
                desc++;
            }
        }
    }
}

VOID
PnpDumpCmResourceDescriptor (
    __in PCHAR Indent,
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Desc
    )
/*++

Routine Description:

    This routine displays a IO_RESOURCE_DESCRIPTOR.

Parameters:

    Indent - # char of indentation.

    Desc - CM_RESOURCE_DESCRIPTOR to be displayed.

Return Value:

    None.

--*/
{
    PAGED_CODE();

    switch (Desc->Type) {
    case CmResourceTypePort:
    case CmResourceTypeMemory:
    case CmResourceTypeMemoryLarge: {
        ULONGLONG DecodedLength;
        ULONGLONG Start;

        DecodedLength = RtlCmDecodeMemIoResource(Desc, &Start);
        IopDbgPrint((
            IOP_RESOURCE_VERBOSE_LEVEL,
            "%s%s Start: %x:%08x, Length:  %I64x\n",
            Indent,
            Desc->Type == CmResourceTypePort ? "IO" : "MEM",
            (ULONG)(Start >> 32),
            (ULONG)(Start),
            DecodedLength));
        break;
    }

    case CmResourceTypeInterrupt:

        IopDbgPrint((
            IOP_RESOURCE_VERBOSE_LEVEL,
            "%sINT Level: %x, Vector: %x, Affinity: %x\n",
            Indent,
            Desc->u.Interrupt.Level,
            Desc->u.Interrupt.Vector,
            Desc->u.Interrupt.Affinity));
        break;

    case CmResourceTypeDma:

        IopDbgPrint((
            IOP_RESOURCE_VERBOSE_LEVEL,
            "%sDMA Channel: %x, Port: %x\n",
            Indent,
            Desc->u.Dma.Channel,
            Desc->u.Dma.Port));
        break;
    }
}

#endif
