/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    hwconfig.c

Abstract:

    This module implements the hardware resource configuration engine.

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

#if DBG

ULONG IopUseTimeout = 0;

ULONG PnpResDebugTranslationFailureCount = 32;  // get count in both this line and the next.
PNPRESDEBUGTRANSLATIONFAILURE PnpResDebugTranslationFailureArray[32];
PNPRESDEBUGTRANSLATIONFAILURE *PnpResDebugTranslationFailure = PnpResDebugTranslationFailureArray;

#endif  // DBG

#ifdef ALLOC_PRAGMA

#pragma alloc_text(PAGE, IopTestConfiguration)
#pragma alloc_text(PAGE, IopRetestConfiguration)
#pragma alloc_text(PAGE, IopCommitConfiguration)
#pragma alloc_text(PAGE, IopCountMaximumConfigurations)
#pragma alloc_text(PAGE, PnpSelectFirstConfiguration)
#pragma alloc_text(PAGE, IopSelectNextConfiguration)
#pragma alloc_text(PAGE, IopCleanupSelectedConfiguration)
#pragma alloc_text(PAGE, IopComputeConfigurationPriority)
#pragma alloc_text(PAGE, IopSaveRestoreConfiguration)
#pragma alloc_text(PAGE, IopAddRemoveReqDescs)
#pragma alloc_text(PAGE, PnpFindBestConfiguration)
#pragma alloc_text(PAGE, PnpFindBestConfigurationWorker)
#pragma alloc_text(PAGE, IopFreeReqList)
#pragma alloc_text(PAGE, IopFreeReqAlternative)
#pragma alloc_text(PAGE, IopResourceRequirementsListToReqList)
#pragma alloc_text(PAGE, IopRearrangeReqList)
#pragma alloc_text(PAGE, IopCompareReqAlternativePriority)
#pragma alloc_text(PAGE, IopSetupArbiterAndTranslators)
#pragma alloc_text(PAGE, IopFindResourceHandlerInfo)
#pragma alloc_text(PAGE, IopCallArbiter)
#pragma alloc_text(PAGE, IopTranslateAndAdjustReqDesc)
#pragma alloc_text(PAGE, PnpLookupArbitersNewResources)
#pragma alloc_text(PAGE, IopParentToRawTranslation)
#pragma alloc_text(PAGE, IopChildToRootTranslation)

#if DBG

#pragma alloc_text(PAGE, PiDumpArbiterList)
#pragma alloc_text(PAGE, PiDumpArbiterListEntry)
#pragma alloc_text(PAGE, IopCheckDataStructures)
#pragma alloc_text(PAGE, IopCheckDataStructuresWorker)

#endif // DBG

#endif // ALLOC_PRAGMA

NTSTATUS
IopTestConfiguration (
    __in PPNP_RESOURCE_REQUEST RequestTable,
    __in ULONG RequestTableCount,
    __inout PLIST_ENTRY ArbiterList
    )

/*++

Routine Description:

    This routine calls the arbiters in the specified list for TestAllocation.

Parameters:

    ArbiterList - Head of list of arbiters to be called.

Return Value:

    STATUS_SUCCESS if all arbiters succeed, else first failure code.

--*/

{

    NTSTATUS                    status;
    PLIST_ENTRY                 listEntry;
    PPI_RESOURCE_ARBITER_ENTRY  arbiterEntry;
    ARBITER_PARAMETERS          p;
    PARBITER_INTERFACE          arbiterInterface;

    PAGED_CODE();

    status = STATUS_SUCCESS;
    for (   listEntry = ArbiterList->Flink;
            listEntry != ArbiterList;
            listEntry = listEntry->Flink) {

        arbiterEntry = CONTAINING_RECORD(
                            listEntry,
                            PI_RESOURCE_ARBITER_ENTRY,
                            ActiveArbiterList);
        ASSERT(IsListEmpty(&arbiterEntry->ResourceList) == FALSE);
        if (arbiterEntry->ResourcesChanged == FALSE) {

            if (arbiterEntry->State & PI_ARBITER_TEST_FAILED) {
                //
                // If the resource requirements are the same and
                // it failed before, return failure.
                //
                status = STATUS_UNSUCCESSFUL;
                break;
            }
        } else {

            arbiterInterface = arbiterEntry->ArbiterInterface;
            //
            // Call the arbiter to test the new configuration.
            //
            p.Parameters.TestAllocation.ArbitrationList     =
                                                    &arbiterEntry->ResourceList;
            p.Parameters.TestAllocation.AllocateFromCount   = 0;
            p.Parameters.TestAllocation.AllocateFrom        = NULL;

            PnpLookupArbitersNewResources(RequestTable,
                                          RequestTableCount,
                                          arbiterEntry->DeviceNode,
                                          arbiterEntry->ResourceType,
                                          &p.Parameters.TestAllocation.AllocateFrom,
                                          &p.Parameters.TestAllocation.AllocateFromCount);

            IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                         "Testing allocation for resource type 0x%x with arbiter on %wZ\n",
                         arbiterEntry->ResourceType,
                         PnpGetDeviceInstancePath(arbiterEntry->DeviceNode)));

            PiDumpArbiterList(&arbiterEntry->ResourceList);

            status = arbiterInterface->ArbiterHandler(
                                            arbiterInterface->Context,
                                            ArbiterActionTestAllocation,
                                            &p);
            if (NT_SUCCESS(status)) {

                IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                             "Arbiter on %wZ for resource type 0x%X succeeded TestAllocation\n",
                             PnpGetDeviceInstancePath(arbiterEntry->DeviceNode),
                             arbiterEntry->ResourceType));

                arbiterEntry->State &= ~PI_ARBITER_TEST_FAILED;
                arbiterEntry->State |= PI_ARBITER_HAS_SOMETHING;
                arbiterEntry->ResourcesChanged = FALSE;

            } else {

                IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                             "Arbiter on %wZ for resource type 0x%X failed TestAllocation (status = 0x%8.8X)\n",
                             PnpGetDeviceInstancePath(arbiterEntry->DeviceNode),
                             arbiterEntry->ResourceType,
                             status));

                //
                // Keep track of the failed allocation information.
                //

                PnpLogDeviceConflictingResource(arbiterEntry,
                                                arbiterInterface->Context);

                //
                // This configuration does not work
                // (no need to try other arbiters).
                //
                arbiterEntry->State |= PI_ARBITER_TEST_FAILED;
                break;
            }
        }
    }

    return status;
}

NTSTATUS
IopRetestConfiguration (
    __in PPNP_RESOURCE_REQUEST RequestTable,
    __in ULONG RequestTableCount,
    __inout PLIST_ENTRY ArbiterList
    )

/*++

Routine Description:

    This routine calls the arbiters in the specified list for RetestAllocation.

Parameters:

    ArbiterList - Head of list of arbiters to be called.

Return Value:

    STATUS_SUCCESS if all arbiters succeed, else first failure code.

--*/

{
    NTSTATUS                    retestStatus;
    PLIST_ENTRY                 listEntry;
    PPI_RESOURCE_ARBITER_ENTRY  arbiterEntry;
    ARBITER_PARAMETERS          p;
    PARBITER_INTERFACE          arbiterInterface;

    PAGED_CODE();

    retestStatus = STATUS_SUCCESS;
    listEntry    = ArbiterList->Flink;
    while (listEntry != ArbiterList) {

        arbiterEntry = CONTAINING_RECORD(
                        listEntry,
                        PI_RESOURCE_ARBITER_ENTRY,
                        ActiveArbiterList);
        listEntry = listEntry->Flink;
        if (arbiterEntry->ResourcesChanged == FALSE) {

            continue;
        }
        ASSERT(IsListEmpty(&arbiterEntry->ResourceList) == FALSE);
        arbiterInterface = arbiterEntry->ArbiterInterface;
        //
        // Call the arbiter to retest the configuration.
        //
        p.Parameters.RetestAllocation.ArbitrationList     =
                                                    &arbiterEntry->ResourceList;
        p.Parameters.RetestAllocation.AllocateFromCount   = 0;
        p.Parameters.RetestAllocation.AllocateFrom        = NULL;

        PnpLookupArbitersNewResources(RequestTable,
                                      RequestTableCount,
                                      arbiterEntry->DeviceNode,
                                      arbiterEntry->ResourceType,
                                      &p.Parameters.TestAllocation.AllocateFrom,
                                      &p.Parameters.TestAllocation.AllocateFromCount);

        IopDbgPrint((IOP_RESOURCE_INFO_LEVEL,
                     "Retesting allocation for resource type 0x%x with arbiter on %wZ\n",
                     arbiterEntry->ResourceType,
                     PnpGetDeviceInstancePath(arbiterEntry->DeviceNode)));

        PiDumpArbiterList(&arbiterEntry->ResourceList);

        retestStatus = arbiterInterface->ArbiterHandler(
                                            arbiterInterface->Context,
                                            ArbiterActionRetestAllocation,
                                            &p);
        if (!NT_SUCCESS(retestStatus)) {

            IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                         "Arbiter on %wZ for resource type 0x%X failed RetestAllocation (status = 0x%8.8X)\n",
                         PnpGetDeviceInstancePath(arbiterEntry->DeviceNode),
                         arbiterEntry->ResourceType,
                         retestStatus));
            break;

        } else {

            IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                         "Arbiter on %wZ for resource type 0x%X succeeded RetestAllocation\n",
                         PnpGetDeviceInstancePath(arbiterEntry->DeviceNode),
                         arbiterEntry->ResourceType));
        }
    }

    PNP_ASSERT(NT_SUCCESS(retestStatus));

    return retestStatus;
}

NTSTATUS
IopCommitConfiguration (
    __inout  PLIST_ENTRY ArbiterList
    )

/*++

Routine Description:

    This routine calls the arbiters in the specified list for CommitAllocation.

    This routine is called after PnpFindBestConfiguration() succeeds.

Parameters:

    ArbiterList - Head of list of arbiters to be called.

Return Value:

    STATUS_SUCCESS if all arbiters succeed, else first failure code.

--*/

{
    PPI_RESOURCE_ARBITER_ENTRY  arbiterEntry;
    PARBITER_INTERFACE          arbiterInterface;
    NTSTATUS                    commitStatus;
    PLIST_ENTRY                 listEntry;
    NTSTATUS                    returnStatus;

    PAGED_CODE();

    returnStatus = STATUS_SUCCESS;
    listEntry    = ArbiterList->Flink;
    while (listEntry != ArbiterList) {
        arbiterEntry = CONTAINING_RECORD(
                        listEntry,
                        PI_RESOURCE_ARBITER_ENTRY,
                        ActiveArbiterList);

        listEntry = listEntry->Flink;
        PNP_ASSERT(IsListEmpty(&arbiterEntry->ResourceList) == FALSE);
        arbiterInterface = arbiterEntry->ArbiterInterface;

        //
        // Call the arbiter to commit the configuration.
        //
        
        commitStatus = arbiterInterface->ArbiterHandler(
                           arbiterInterface->Context,
                           ArbiterActionCommitAllocation,
                           NULL);

        if (!NT_SUCCESS(commitStatus)) {

            //
            // It's unusual that the arbiter succeeded in TestAllocation (and
            // RetestAllocation, if RetestAllocation was called) but failed in
            // CommitAllocation.
            //

            PNP_ASSERT(FALSE);
            returnStatus = commitStatus;
        }

        //
        // Reset the arbiter entry even if we didn't commit, to ensure that we
        // will have no LIST_ENTRYs that will become dangling after the
        // eventual IopFreeReqAlternative().
        //

        IopInitializeArbiterEntryState(arbiterEntry);
        PNP_ASSERT(IsListEmpty(&arbiterEntry->ResourceList) != FALSE);
    }

    IopCheckDataStructures(IopRootDeviceNode);
    return returnStatus;
}

ULONG64
IopCountMaximumConfigurations (
    __in_ecount(RequestTableCount)  PPNP_RESOURCE_REQUEST    RequestTable,
    __in                            ULONG                    RequestTableCount
)

/*++

Routine Description:

    This routine returns the number of configurations that will be returned by
    PnpSelectFirstConfiguration() and IopSelectNextConfiguration().

    This needs to be called again for an updated calculation if you save a
    configuration using IopSaveRestoreConfiguration(). This updated result will
    be smaller.

    This does not take into account how many configurations have already been
    tested.

Parameters:

    RequestTable        - Table of resource requests.

    RequestTableCount   - Number of requests in the request table.

Return Value:

    The maximum number of configurations.

--*/

{
    ULONG64 numConfigs;
    PREQ_LIST reqList;
    ULONG tableIndex;

    PAGED_CODE();

    numConfigs = 1;

    for (tableIndex = 0; tableIndex < RequestTableCount; tableIndex++) {
        reqList = RequestTable[tableIndex].ReqList;
        if ((reqList->BestAlternative != NULL) &&
            (reqList->BestAlternative != reqList->AlternativeTable)) {

            numConfigs *= (ULONG64)(reqList->BestAlternative -
                                    reqList->AlternativeTable);
        }
    }

    return numConfigs;
}

VOID
PnpSelectFirstConfiguration (
    __in_ecount(RequestTableCount)  PPNP_RESOURCE_REQUEST   RequestTable,
    __in                            ULONG                   RequestTableCount,
    __inout                         PLIST_ENTRY             ActiveArbiterList,
    __in                            ULONG                   Phase
    )

/*++

Routine Description:

    This routine selects the first possible configuration and adds the
    descriptors to their corresponding arbiter lists. The arbiters used are
    linked into the list of active arbiters.

Parameters:

    RequestTable        - Table of resource requests.

    RequestTableCount   - Number of requests in the request table.

    ActiveArbiterList   - Head of list which contains arbiters used for the
        first selected configuration.

    Phase               - Specifies the allocation phase. Boot conflicts are not
        allowed in phase 0.

Return Value:

    None.

--*/

{
    ULONG descIndex;
    ULONG tableIndex;
    PREQ_ALTERNATIVE reqAlternative;
    PREQ_DESC reqDesc;
    PREQ_LIST reqList;

    PAGED_CODE();

    //
    // For each resource request, select the first alternative (which is also
    // the most preferred one) and update the arbiters with all the descriptors
    // for that alternative.
    //

    for (tableIndex = 0; tableIndex < RequestTableCount; tableIndex++) {
        reqList                         = RequestTable[tableIndex].ReqList;
        reqList->SelectedAlternative    = &reqList->AlternativeTable[0];
        reqAlternative                  = *(reqList->SelectedAlternative);

        //
        // If first phase failed, retry allowing boot conflicts.
        //

        if (Phase != 0) {
            for (descIndex = 0; descIndex < reqAlternative->DescCount; descIndex++) {
                reqDesc = reqAlternative->DescTable[descIndex];
                if (reqDesc->ArbitrationRequired != FALSE) {
                    if (reqAlternative->Priority == LCPRI_BOOTCONFIG) {
                        reqDesc->AlternativeTable.Flags |= ARBITER_FLAG_BOOT_CONFIG;
                    }
                }
            }
        }

        IopAddRemoveReqDescs(reqAlternative->DescTable,
                             reqAlternative->DescCount,
                             ActiveArbiterList,
                             TRUE);
    }
}

BOOLEAN
IopSelectNextConfiguration (
    __in_ecount(RequestTableCount)  PPNP_RESOURCE_REQUEST    RequestTable,
    __in                            ULONG                    RequestTableCount,
    __inout                         PLIST_ENTRY              ActiveArbiterList
    )

/*++

Routine Description:

    This routine selects the next possible configuration and adds the
    descriptors to their corresponding arbiter lists. The arbiters used are
    linked into the list of active arbiters.

    This routine is called if the first configuration returned by
    PnpSelectFirstConfiguration, or a previous configuration returned by
    IopSelectNextConfiguration, was rejected by any arbiter's TestAllocation
    action.

    A "configuration" is a combination of selected alternatives, one for each
    resource request.

Parameters:

    RequestTable        - Table of resource requests.

    RequestTableCount   - Number of requests in the request table.

    ActiveArbiterList   - Head of list which contains arbiters used for the
        currently selected configuration.

Return Value:

    FALSE if this the currently selected configuration is the last possible,
    else TRUE.

--*/

{
    ULONG               tableIndex;
    PREQ_ALTERNATIVE    reqAlternative;
    PREQ_LIST           reqList;

    PAGED_CODE();

    //
    // Loop through all the resource requests...
    //

    for (tableIndex = 0; tableIndex < RequestTableCount; tableIndex++) {
        reqList         = RequestTable[tableIndex].ReqList;
        reqAlternative  = *(reqList->SelectedAlternative);

        //
        // Remove all the descriptors from the currently selected alternative
        // for this resource request.
        //

        IopAddRemoveReqDescs(
            reqAlternative->DescTable,
            reqAlternative->DescCount,
            NULL,
            FALSE);

        //
        // Try a less preferable alternative for this resource request.
        // Incrementing works because the list is sorted from most to least
        // preferable.
        //
        // Conceptually, if each resource request's selected alternative is
        // a digit and the entire configuration is a number composed of these
        // digits stored backwards, this is like adding '1' to the number.
        //

        if (++reqList->SelectedAlternative < reqList->BestAlternative) {

            //
            // The next possible configuration has been found.
            //

            break;
        }

        //
        // This point has been reached because the least preferable
        // alternative has already been tried for this resource request.
        // Return to trying the most preferable alternative and continue
        // this loop so that the next resource request can try a less
        // preferable alternative - maybe such a combination will be accepted
        // by the arbiters.
        //
        // To continue the "configuration is a number stored backwards"
        // analogy, this is like adding '1' to the number caused a carry
        // to the next digit (hence the increment in the next iteration of
        // the loop), so the current digit has to be set to 0.
        //

        reqList->SelectedAlternative = &reqList->AlternativeTable[0];
    }

    //
    // We are done if there is no next possible configuration.
    //
    // To continue the analogy, this is like if the largest number was already
    // used before the current invocation of the function and the current
    // invocation's addition of '1' caused a wrap-around of the number to 0.
    //

    if (tableIndex == RequestTableCount) {
        return FALSE;
    }

    //
    // Loop through all the resource requests...
    //

    for (tableIndex = 0; tableIndex < RequestTableCount; tableIndex++) {
        reqList         = RequestTable[tableIndex].ReqList;
        reqAlternative  = *(reqList->SelectedAlternative);

        //
        // Add all the descriptors to the currently selected alternative
        // for this resource request.
        //
        // The previous "for" loop had removed all the descriptors before it
        // tried to increment the selected alternative.
        //

        IopAddRemoveReqDescs(
            reqAlternative->DescTable,
            reqAlternative->DescCount,
            ActiveArbiterList,
            TRUE);

        //
        // If this was the last resource request from the previous "for" loop
        // that all the descriptors were removed from, exit since the remaining
        // resource requests' descriptors were untouched and are still valid.
        //

        if (reqList->SelectedAlternative != &reqList->AlternativeTable[0]) {
            break;
        }
    }

    return TRUE;
}

VOID
IopCleanupSelectedConfiguration (
    __in_ecount(RequestTableCount) PPNP_RESOURCE_REQUEST    RequestTable,
    __in ULONG                    RequestTableCount
    )

/*++

Routine Description:

    This routine removes the descriptors from their corresponding arbiter
    lists.

Parameters:

    RequestTable        - Table of resource requests.

    RequestTableCount   - Number of requests in the request table.

Return Value:

    None.

--*/

{
    ULONG               tableIndex;
    PREQ_ALTERNATIVE    reqAlternative;
    PREQ_LIST           reqList;

    PAGED_CODE();
    //
    // For each entry in the request table, remove all the descriptors
    // from the currently selected alternative.
    //
    for (tableIndex = 0; tableIndex < RequestTableCount; tableIndex++) {

        reqList         = RequestTable[tableIndex].ReqList;
        reqAlternative  = *(reqList->SelectedAlternative);
        IopAddRemoveReqDescs(
            reqAlternative->DescTable,
            reqAlternative->DescCount,
            NULL,
            FALSE);
    }
}

ULONG
IopComputeConfigurationPriority (
    __in_ecount(RequestTableCount) PPNP_RESOURCE_REQUEST    RequestTable,
    __in ULONG                    RequestTableCount
    )

/*++

Routine Description:

    This routine computes the overall priority of the set of selected
    alternatives for all requests in the request table.

Parameters:

    RequestTable        - Table of resource requests.

    RequestTableCount   - Number of requests in the request table.

Return Value:

    Computed priority for this configuration.

--*/

{
    ULONG               tableIndex;
    ULONG               priority;
    PREQ_ALTERNATIVE    reqAlternative;
    PREQ_LIST           reqList;

    PAGED_CODE();

    //
    // Compute the current configuration's overall priority as the sum of the
    // priorities of the alternatives in the request table.
    //

    priority = 0;
    for (tableIndex = 0; tableIndex < RequestTableCount; tableIndex++) {

        reqList         = RequestTable[tableIndex].ReqList;
        reqAlternative  = *(reqList->SelectedAlternative);
        priority        += reqAlternative->Priority;
    }

    return priority;
}

VOID
IopSaveRestoreConfiguration (
    __in_ecount(RequestTableCount)      PPNP_RESOURCE_REQUEST   RequestTable,
    __in      ULONG                   RequestTableCount,
    __inout  PLIST_ENTRY             ArbiterList,
    __in      BOOLEAN                 Save
    )

/*++

Routine Description:

    This routine saves/restores the currently selected configuration.

Parameters:

    RequestTable        - Table of resource requests.

    RequestTableCount   - Number of requests in the request table.

    ArbiterList         - Head of list which contains arbiters used for the
                          currently selected configuration.

    Save                - Specifies if the configuration is to be saved or
                          restored.

Return Value:

    None.

--*/

{
    ULONG                       tableIndex;
    PREQ_ALTERNATIVE            reqAlternative;
    PREQ_DESC                   reqDesc;
    PREQ_DESC                   *reqDescpp;
    PREQ_DESC                   *reqDescTableEnd;
    PLIST_ENTRY                 listEntry;
    PPI_RESOURCE_ARBITER_ENTRY  arbiterEntry;
    PREQ_LIST                   reqList;

    PAGED_CODE();

    IopDbgPrint((
        IOP_RESOURCE_TRACE_LEVEL,
        "%s configuration\n",
        (Save)? "Saving" : "Restoring"));
    //
    // For each entry in the request table, save information for
    // following RETEST.
    //
    for (tableIndex = 0; tableIndex < RequestTableCount; tableIndex++) {

        reqList                     = RequestTable[tableIndex].ReqList;
        if (Save) {

            reqList->BestAlternative        = reqList->SelectedAlternative;
        } else {

            reqList->SelectedAlternative    = reqList->BestAlternative;
        }
        reqAlternative              = *(reqList->BestAlternative);
        reqDescTableEnd             = reqAlternative->DescTable +
                                        reqAlternative->DescCount;
        for (   reqDescpp = reqAlternative->DescTable;
                reqDescpp < reqDescTableEnd;
                reqDescpp++) {

            if ((*reqDescpp)->ArbitrationRequired == FALSE) {

                continue;
            }
            //
            // Save\restore information for the descriptor.
            //
            reqDesc = (*reqDescpp)->TranslatedReqDesc;
            if (Save != FALSE) {

                reqDesc->BestAlternativeTable  = reqDesc->AlternativeTable;
                reqDesc->BestAllocation        = reqDesc->Allocation;
            } else {

                reqDesc->AlternativeTable  = reqDesc->BestAlternativeTable;
                reqDesc->Allocation        = reqDesc->BestAllocation;
            }
        }
    }
    //
    // For each entry in the currently active arbiter list,
    // save information for following RETEST.
    //
    listEntry = ArbiterList->Flink;
    while (listEntry != ArbiterList) {
        arbiterEntry = CONTAINING_RECORD(
                        listEntry,
                        PI_RESOURCE_ARBITER_ENTRY,
                        ActiveArbiterList);
        if (Save != FALSE) {
            arbiterEntry->BestResourceList  = arbiterEntry->ResourceList;
            arbiterEntry->BestConfig        = arbiterEntry->ActiveArbiterList;
        } else {
            arbiterEntry->ResourceList      = arbiterEntry->BestResourceList;
            arbiterEntry->ActiveArbiterList = arbiterEntry->BestConfig;
        }
        listEntry = listEntry->Flink;
    }
}

VOID
IopAddRemoveReqDescs (
    __in_ecount(ReqDescCount)      PREQ_DESC   *ReqDescTable,
    __in      ULONG       ReqDescCount,
    __inout_opt PLIST_ENTRY ActiveArbiterList,
    __in      BOOLEAN     Add
    )

/*++

Routine Description:

    This routine adds\removes the descriptors to\from the arbiter lists. It
    also updates the list of arbiters involved.

Parameters:

    RequestTable        - Table of resource requests.

    RequestTableCount   - Number of requests in the request table.

    ActiveArbiterList   - Head of list which contains arbiters used for the
                          currently selected configuration.

    Add                 - Specifies if the descriptors are to be added or
                          removed.

Return Value:

    None.

--*/

{
    ULONG                       tableIndex;
    PREQ_DESC                   reqDesc;
    PREQ_DESC                   reqDescTranslated;
    PPI_RESOURCE_ARBITER_ENTRY  arbiterEntry;
    PREQ_ALTERNATIVE            reqAlternative;
    PREQ_LIST                   reqList;
    PDEVICE_NODE                deviceNode;

    PAGED_CODE();

    if (ReqDescCount == 0) {

        return;
    }

    reqList         = ReqDescTable[0]->ReqAlternative->ReqList;
    reqAlternative  = *reqList->SelectedAlternative;
    deviceNode      = PP_DO_TO_DN(reqList->Request->PhysicalDevice);
    IopDbgPrint((
        IOP_RESOURCE_VERBOSE_LEVEL,
        "%s %d/%d req alt %s the arbiters for %wZ\n",
        (Add)? "Adding" : "Removing",
        reqAlternative->ReqAlternativeIndex + 1,
        reqList->AlternativeCount,
        (Add)? "to" : "from",
        PnpGetDeviceInstancePath(deviceNode)));
    for (tableIndex = 0; tableIndex < ReqDescCount; tableIndex++) {

        reqDesc = ReqDescTable[tableIndex];
        if (reqDesc->ArbitrationRequired == FALSE) {

            continue;
        }
        arbiterEntry = reqDesc->u.Arbiter;
        PNP_ASSERT(arbiterEntry);
        if (arbiterEntry->State & PI_ARBITER_HAS_SOMETHING) {

            arbiterEntry->State &= ~PI_ARBITER_HAS_SOMETHING;
            arbiterEntry->ArbiterInterface->ArbiterHandler(
                                    arbiterEntry->ArbiterInterface->Context,
                                    ArbiterActionRollbackAllocation,
                                    NULL);
        }
        arbiterEntry->ResourcesChanged  = TRUE;
        reqDescTranslated               = reqDesc->TranslatedReqDesc;
        if (Add != FALSE) {

            InitializeListHead(&reqDescTranslated->AlternativeTable.ListEntry);
            InsertTailList(
                &arbiterEntry->ResourceList,
                &reqDescTranslated->AlternativeTable.ListEntry);
            if (IsListEmpty(&arbiterEntry->ActiveArbiterList)) {

                PLIST_ENTRY                 listEntry;
                PPI_RESOURCE_ARBITER_ENTRY  entry;
                //
                // Insert the entry into the sorted list
                // (sorted by depth in the tree).
                //
                for (   listEntry = ActiveArbiterList->Flink;
                        listEntry != ActiveArbiterList;
                        listEntry = listEntry->Flink) {

                    entry = CONTAINING_RECORD(
                                listEntry,
                                PI_RESOURCE_ARBITER_ENTRY,
                                ActiveArbiterList);
                    if (entry->DeviceNode->Level >= arbiterEntry->DeviceNode->Level) {

                        break;
                    }
                }
                arbiterEntry->ActiveArbiterList.Flink   = listEntry;
                arbiterEntry->ActiveArbiterList.Blink   = listEntry->Blink;
                listEntry->Blink->Flink = &arbiterEntry->ActiveArbiterList;
                listEntry->Blink        = &arbiterEntry->ActiveArbiterList;
            }
        } else {

            PNP_ASSERT(IsListEmpty(&arbiterEntry->ResourceList) == FALSE);
            RemoveEntryList(&reqDescTranslated->AlternativeTable.ListEntry);
            InitializeListHead(&reqDescTranslated->AlternativeTable.ListEntry);
            if (IsListEmpty(&arbiterEntry->ResourceList)) {

                RemoveEntryList(&arbiterEntry->ActiveArbiterList);
                InitializeListHead(&arbiterEntry->ActiveArbiterList);
            }
        }
    }
}

NTSTATUS
PnpFindBestConfiguration (
    __in_ecount(RequestTableCount)  PPNP_RESOURCE_REQUEST   RequestTable,
    __in                            ULONG                   RequestTableCount,
    __inout                         PLIST_ENTRY             ActiveArbiterList
    )

/*++

Routine Description:

    This routine attempts to satisfy the resource requests for all the entries
    in the request table. It also attempts to find the best possible overall
    solution.

Parameters:

    RequestTable        - Table of resource requests.

    RequestTableCount   - Number of requests in the request table.

    ActiveArbiterList   - List used to link active arbiters.

Return Value:

    Final status.

--*/

{
    ULONG phase;
    NTSTATUS status;

    PAGED_CODE();

    status = STATUS_SUCCESS;
    for (phase = 0; phase < 2; phase++) {
        status = PnpFindBestConfigurationWorker(RequestTable,
                                                RequestTableCount,
                                                ActiveArbiterList,
                                                phase);
        if (NT_SUCCESS(status)) {
            break;
        }
    }

    return status;
}

NTSTATUS
PnpFindBestConfigurationWorker (
    __in_ecount(RequestTableCount)  PPNP_RESOURCE_REQUEST   RequestTable,
    __in                            ULONG                   RequestTableCount,
    __inout                         PLIST_ENTRY             ActiveArbiterList,
    __in                            ULONG                   Phase
    )

/*++

Routine Description:

    This routine attempts to satisfy the resource requests for all the entries
    in the request table. It also attempts to find the best possible overall
    solution.

Parameters:

    RequestTable        - Table of resource requests.

    RequestTableCount   - Number of requests in the request table.

    ActiveArbiterList   - List used to link active arbiters.

    Phase               - Specifies allocation phase. Boot conflicts are not
        allowed in phase 0.

Return Value:

    Final status.

--*/

{
    LIST_ENTRY      bestArbiterList;
    ULONG           bestPriority;
    LARGE_INTEGER   currentTime;
    ULONG64         configCount;
    ULONG64         configUpto;
    LARGE_INTEGER   lastTimeoutTime;
    ULONG64         minutesLeft;
    ULONG           priority;
    LARGE_INTEGER   startTime;
    NTSTATUS        status;
    ULONG           stopOnTimeout;
    ULONG           timeDiff;
    ULONG           timeout;

    PAGED_CODE();

    //
    // Initialize the arbiter lists used during the search for the best
    // configuration.
    //

    InitializeListHead(ActiveArbiterList);
    InitializeListHead(&bestArbiterList);

    //
    // Get a consistent snapshot as this macro could theoretically return a
    // different value at another time (e.g. on CHK builds, if IopUseTimeout is
    // changed in the debugger).
    //

    stopOnTimeout = IopStopOnTimeout();

    //
    // Start the search from the first possible configuration.
    // Possible configurations are already sorted by priority.
    //

    PnpSelectFirstConfiguration(RequestTable,
                                RequestTableCount,
                                ActiveArbiterList,
                                Phase);

    //
    // Search for all configurations that work, updating
    // the best configuration until we have tried all
    // possible configurations or timeout has expired.
    //

    KeQuerySystemTime(&startTime);
    lastTimeoutTime = startTime;
    timeout = PnpFindBestConfigurationTimeout;
    bestPriority = (ULONG)-1;
    if (stopOnTimeout == FALSE) {
        configCount = IopCountMaximumConfigurations(RequestTable, RequestTableCount);
        configUpto = 1;

    } else {

        //
        // Suppress "C4701: potentially uninitialized local variable used".
        // We aren't actually going to use these variables.
        //

        configCount = 1;
        configUpto = 1;
    }

    do {

        //
        // Test the arbiters for this combination.
        //

        status = IopTestConfiguration(RequestTable, RequestTableCount, ActiveArbiterList);
        if (NT_SUCCESS(status)) {

            //
            // Since the configurations are sorted, we don't need to try others
            // if there is only one entry in the request table.
            //

            bestArbiterList = *ActiveArbiterList;
            if (RequestTableCount == 1) {
                break;
            }

            //
            // Save this configuration if it is better than the best one found
            // so far.
            //


            priority = IopComputeConfigurationPriority(RequestTable,
                                                       RequestTableCount);

            if (priority < bestPriority) {
                bestPriority = priority;
                IopSaveRestoreConfiguration(RequestTable,
                                            RequestTableCount,
                                            ActiveArbiterList,
                                            TRUE);

                if (stopOnTimeout == FALSE) {
                    configCount = IopCountMaximumConfigurations(RequestTable,
                                                                RequestTableCount);
                }
            }
        }

        //
        // Check if timeout has expired.
        //

        KeQuerySystemTime(&currentTime);
        timeDiff = (ULONG)((currentTime.QuadPart - lastTimeoutTime.QuadPart) / 10000);
        if (timeDiff >= timeout) {
            if (stopOnTimeout == FALSE) {

                //
                // Re-initialize the time of the last timeout so we spew only
                // every timeout interval. And adjust the timeout interval so
                // that we don't spew more often than every 30secs.
                //

                lastTimeoutTime = currentTime;
                if (timeout < PnpFindBestConfigurationDebugSpewTimeout) {
                    timeout = PnpFindBestConfigurationDebugSpewTimeout;
                }

                //
                // Print an informative message that will help developers triage
                // "The CHK kernel hangs at startup in arbitration code":
                //
                // * If you don't see this message every
                //    max(30secs,PnpFindBestConfigurationTimeout), the arbiter
                //    on the call stack when you break in is most likely the
                //    code that is taking way too long.
                //
                // * Otherwise, at least you get a time estimate and know that
                //   the system will actually boot eventually. If there are
                //   a huge number of configurations and there are many
                //   devices, the slow startup is most likely by design since
                //   there are simply so many combinations of alternative
                //   resource lists (REQ_LIST/IO_RESOURCE_LIST) to try out.
                //
                // The time estimate assumes that the average time to test each
                // future configuration will be the same as for past
                // configurations:
                //
                // * 10000000 converts to seconds.
                //
                // * 60 converts to minutes. We divide this last in order to
                //   give us a little bit more precision without too much added
                //   chance of overflow. Having said that, the overflow that
                //   occurs in extreme cases do not matter since this is just a
                //   debug message. "+ 1" ensures we won't be at 0 minutes left
                //   for almost a minute.
                //

                minutesLeft = ((currentTime.QuadPart - startTime.QuadPart) /
                               10000000);
                minutesLeft = ((minutesLeft * (configCount - configUpto)) /
                                configUpto);
                minutesLeft = ((minutesLeft / 60) + 1);
                IopDbgPrint((
                    IOP_RESOURCE_ERROR_LEVEL,
                    "PnpFindBestConfigurationWorker: Timeout expired,"
                    " tested %I64u/%I64u configurations for %u"
                    " device(s), estimated time remaining:"
                    " %I64umins.\n",
                    configUpto,
                    configCount,
                    RequestTableCount,
                    minutesLeft));

            } else {
                IopDbgPrint((
                    IOP_RESOURCE_WARNING_LEVEL,
                    "PnpFindBestConfigurationWorker: Timeout expired, terminating search!\n"));

                IopCleanupSelectedConfiguration(RequestTable,
                                                RequestTableCount);

                break;
            }
        }

        if (stopOnTimeout == FALSE) {
            configUpto++;

            //
            // In the highly unlikely case of wrap-around, avoid a
            // divide-by-zero in the time estimation code above.
            //

            if (configUpto == 0) {
                configUpto = 1;
            }
        }

        //
        // Select the next possible combination of configurations.
        //

    } while (IopSelectNextConfiguration(RequestTable,
                                        RequestTableCount,
                                        ActiveArbiterList) != FALSE);

    //
    // Check if we found any working configuration.
    //

    if (IsListEmpty(&bestArbiterList)) {

        //
        // Do not overwrite any MCFG conflict errors as we
        // want this to propogate up the stack so the conflicting
        // device is set with appropriate problem code.
        //

        if (status != STATUS_BAD_MCFG_TABLE) {
            status = STATUS_UNSUCCESSFUL;
        }

    } else {

        //
        // Restore the saved configuration.
        //

        status = STATUS_SUCCESS;
        if (RequestTableCount != 1) {
            *ActiveArbiterList = bestArbiterList;
            IopSaveRestoreConfiguration(RequestTable,
                                        RequestTableCount,
                                        ActiveArbiterList,
                                        FALSE);

            //
            // Retest this configuration since this may not be the
            // last one tested.
            //

            status = IopRetestConfiguration(RequestTable,
                                            RequestTableCount,
                                            ActiveArbiterList);
        }
    }

    return status;
}

VOID
IopFreeReqAlternative (
    __in PREQ_ALTERNATIVE ReqAlternative
    )

/*++

Routine Description:

    This routine release the storage for the ReqAlternative by freeing the
    contained descriptors.

Parameters:

    ReqList - REQ_ALTERNATIVE to be freed.

Return Value:

    None.

--*/

{
    PREQ_DESC   reqDesc;
    PREQ_DESC   reqDescx;
    ULONG       i;

    PAGED_CODE();

    if (ReqAlternative) {
        //
        // Free all REQ_DESC making this REQ_ALTERNATIVE.
        //
        for (i = 0; i < ReqAlternative->DescCount; i++) {
            //
            // Free the list of translated REQ_DESCs for this REQ_DESC.
            //
            reqDesc     = ReqAlternative->DescTable[i];
            reqDescx    = reqDesc->TranslatedReqDesc;
            while (reqDescx && IS_TRANSLATED_REQ_DESC(reqDescx)) {
                //
                // Free storage for alternative descriptors if any.
                //
                if (reqDescx->AlternativeTable.Alternatives) {

                    ExFreePool(reqDescx->AlternativeTable.Alternatives);
                    reqDescx->AlternativeTable.Alternatives = NULL;
                }
                reqDesc     = reqDescx;
                reqDescx    = reqDescx->TranslatedReqDesc;
                ExFreePool(reqDesc);
            }
        }
    }
}

VOID
IopFreeReqList (
    __in PREQ_LIST ReqList
    )

/*++

Routine Description:

    This routine release the storage for the ReqList by freeing the contained
    alternatives.

Parameters:

    ReqList - REQ_LIST to be freed.

Return Value:

    None.

--*/

{
    ULONG i;

    PAGED_CODE();

    if (ReqList) {
        //
        // Free all alternatives making this REQ_LIST.
        //
        for (i = 0; i < ReqList->AlternativeCount; i++) {

            IopFreeReqAlternative(ReqList->AlternativeTable[i]);
            ReqList->AlternativeTable[i] = NULL;
        }
        ExFreePool(ReqList);
    }
}

NTSTATUS
IopResourceRequirementsListToReqList(
    __in  PPNP_RESOURCE_REQUEST   Request,
    __out PVOID                   *ResReqList
    )

/*++

Routine Description:

    This routine processes the input Io resource requirements list and
    generates an internal REQ_LIST and its related structures.

Parameters:

    IoResources - supplies a pointer to the Io resource requirements List.

    PhysicalDevice - supplies a pointer to the physical device object requesting
            the resources.

    ReqList - supplies a pointer to a variable to receive the returned REQ_LIST.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    PIO_RESOURCE_REQUIREMENTS_LIST  ioResources;
    LONG                            ioResourceListCount;
    PIO_RESOURCE_LIST               ioResourceList;
    PIO_RESOURCE_DESCRIPTOR         ioResourceDescriptor;
    PIO_RESOURCE_DESCRIPTOR         ioResourceDescriptorEnd;
    PIO_RESOURCE_DESCRIPTOR         firstDescriptor;
    PUCHAR                          coreEnd;
    BOOLEAN                         noAlternativeDescriptor;
    ULONG                           reqDescAlternativeCount;
    ULONG                           alternativeDescriptorCount;
    ULONG                           reqAlternativeCount;
    PREQ_LIST                       reqList;
    INTERFACE_TYPE                  interfaceType;
    ULONG                           busNumber;
    NTSTATUS                        status;
    NTSTATUS                        failureStatus;
    NTSTATUS                        finalStatus;
    LOGICAL                         resourcesRequired;

    PAGED_CODE();

    *ResReqList = NULL;
    //
    // Make sure there is some resource requirement to be converted.
    //
    resourcesRequired = FALSE;
    ioResources         = Request->ResourceRequirements;
    ioResourceListCount = (LONG)ioResources->AlternativeLists;
    if (ioResourceListCount == 0) {

        IopDbgPrint((
            IOP_RESOURCE_INFO_LEVEL,
            "No ResReqList to convert to ReqList\n"));
        return STATUS_SUCCESS;
    }
    //
    // ***** Phase 1 *****
    //
    // Parse the requirements list to validate it and determine the sizes of
    // internal structures.
    //
    ioResourceList              = ioResources->List;
    coreEnd                     = (PUCHAR)ioResources + ioResources->ListSize;
    reqDescAlternativeCount     = 0;
    alternativeDescriptorCount  = 0;
    while (--ioResourceListCount >= 0) {

        ioResourceDescriptor    = ioResourceList->Descriptors;
        ioResourceDescriptorEnd = ioResourceDescriptor + ioResourceList->Count;
        if (ioResourceDescriptor == ioResourceDescriptorEnd) {
            //
            // An alternative list with zero descriptor count.
            //
            IopDbgPrint((IOP_RESOURCE_WARNING_LEVEL,
                         "Alternative list with zero descriptor count for %wZ!!!\n",
                         PnpGetDeviceInstancePath(PP_DO_TO_DN(Request->PhysicalDevice))));

            return STATUS_SUCCESS;
        }
        //
        // Perform sanity check. On failure, simply return failure status.
        //
        if (    ioResourceDescriptor > ioResourceDescriptorEnd ||
                (PUCHAR)ioResourceDescriptor > coreEnd ||
                (PUCHAR)ioResourceDescriptorEnd > coreEnd) {
            //
            // The structure header is invalid (excluding the variable length
            // Descriptors array) or,
            // IoResourceDescriptorEnd is the result of arithmetic overflow or,
            // the descriptor array is outside of the valid memory.
            //
            IopDbgPrint((IOP_RESOURCE_ERROR_LEVEL, "Invalid ResReqList\n"));
            goto InvalidParameter;
        }
        if (ioResourceDescriptor->Type == CmResourceTypeConfigData) {

            ioResourceDescriptor++;
        }
        firstDescriptor         = ioResourceDescriptor;
        noAlternativeDescriptor = TRUE;
        while (ioResourceDescriptor < ioResourceDescriptorEnd) {

            switch (ioResourceDescriptor->Type) {
            case CmResourceTypeConfigData:

                 IopDbgPrint((
                    IOP_RESOURCE_ERROR_LEVEL,
                    "Invalid ResReq list !!!\n"
                    "\tConfigData descriptors are per-LogConf and should be at "
                    "the beginning of an AlternativeList\n"));
                 goto InvalidParameter;

            case CmResourceTypeDevicePrivate:

                 while (    ioResourceDescriptor < ioResourceDescriptorEnd &&
                            ioResourceDescriptor->Type == CmResourceTypeDevicePrivate) {

                     if (ioResourceDescriptor == firstDescriptor) {

                        IopDbgPrint((
                            IOP_RESOURCE_ERROR_LEVEL,
                            "Invalid ResReq list !!!\n"
                            "\tThe first descriptor of a LogConf can not be a "
                            "DevicePrivate descriptor.\n"));
                        goto InvalidParameter;
                     }
                     reqDescAlternativeCount++;
                     ioResourceDescriptor++;
                 }
                 noAlternativeDescriptor = TRUE;
                 break;

            default:

                reqDescAlternativeCount++;
                //
                // For non-arbitrated resource type, set its Option to preferred
                // such that we won't get confused.
                //
                if (    (ioResourceDescriptor->Type & CmResourceTypeNonArbitrated) ||
                        ioResourceDescriptor->Type == CmResourceTypeNull) {

                    if (ioResourceDescriptor->Type == CmResourceTypeReserved) {

                        reqDescAlternativeCount--;
                    }
                    ioResourceDescriptor->Option = IO_RESOURCE_PREFERRED;
                    ioResourceDescriptor++;
                    noAlternativeDescriptor = TRUE;
                    break;
                }
                if (ioResourceDescriptor->Option & IO_RESOURCE_ALTERNATIVE) {

                    if (noAlternativeDescriptor) {

                        IopDbgPrint((
                            IOP_RESOURCE_ERROR_LEVEL,
                            "Invalid ResReq list !!!\n"
                            "\tAlternative descriptor without Default or "
                            "Preferred descriptor.\n"));
                       goto InvalidParameter;
                    }
                    alternativeDescriptorCount++;
                } else {

                    noAlternativeDescriptor = FALSE;
                }
                ioResourceDescriptor++;
                break;
            }
        }
        PNP_ASSERT(ioResourceDescriptor == ioResourceDescriptorEnd);
        ioResourceList = (PIO_RESOURCE_LIST)ioResourceDescriptorEnd;
    }
    //
    // ***** Phase 2 *****
    //
    // Allocate structures and initialize them according to caller's Io ResReq list.
    //
    {
        ULONG               reqDescCount;
        IOP_POOL            reqAlternativePool;
        IOP_POOL            reqDescPool;
        ULONG               reqListPoolSize;
        ULONG               reqAlternativePoolSize;
        ULONG               reqDescPoolSize;
        PUCHAR              poolStart;
        ULONG               poolSize;
        IOP_POOL            outerPool;
        PREQ_ALTERNATIVE    reqAlternative;
        PPREQ_ALTERNATIVE   reqAlternativePP;
        ULONG               reqAlternativeIndex;
        PREQ_DESC           reqDesc;
        PREQ_DESC           *reqDescPP;
        ULONG               reqDescIndex;
        PARBITER_LIST_ENTRY arbiterListEntry;
#if DBG

        PPREQ_ALTERNATIVE   reqAlternativeEndPP;

#endif
        failureStatus           = STATUS_UNSUCCESSFUL;
        finalStatus             = STATUS_SUCCESS;
        ioResourceList          = ioResources->List;
        ioResourceListCount     = ioResources->AlternativeLists;
        reqAlternativeCount     = ioResourceListCount;
        reqDescCount            = reqDescAlternativeCount -
                                    alternativeDescriptorCount;

        status = RtlULongMult(reqDescCount, sizeof(REQ_DESC), &reqDescPoolSize);
        if (!NT_SUCCESS(status)) {
            return STATUS_BUFFER_OVERFLOW;
        }

        //
        // reqAlternativePoolSize = reqAlternativeCount *
        //                          (sizeof(REQ_ALTERNATIVE) +
        //                              (reqDescCount - 1) *
        //                                  sizeof(PREQ_DESC));
        //

        if (reqDescCount > 1) {
            status = RtlULongMult((reqDescCount - 1),
                                  (ULONG)sizeof(PREQ_DESC),
                                  &reqAlternativePoolSize);

        } else {

            reqAlternativePoolSize = 0;
            status = STATUS_SUCCESS;
        }

        if (NT_SUCCESS(status)) {
            status = RtlULongAdd(reqAlternativePoolSize,
                                 (ULONG)sizeof(REQ_ALTERNATIVE),
                                 &reqAlternativePoolSize);
        }

        if (NT_SUCCESS(status)) {
            status = RtlULongMult(reqAlternativePoolSize,
                                  reqAlternativeCount,
                                  &reqAlternativePoolSize);
        }

        if (!NT_SUCCESS(status)) {
            return STATUS_BUFFER_OVERFLOW;
        }

        //
        // reqListPoolSize = sizeof(REQ_LIST) +
        //                       (reqAlternativeCount - 1) *
        //                           sizeof(PREQ_ALTERNATIVE);
        //

        if (reqAlternativeCount > 1) {
            status = RtlULongMult((reqAlternativeCount - 1),
                                  (ULONG)sizeof(PREQ_ALTERNATIVE),
                                  &reqListPoolSize);

        } else {
 
            reqListPoolSize = 0;
            status = STATUS_SUCCESS;
        }

        if (NT_SUCCESS(status)) {
            status = RtlULongAdd(reqListPoolSize,
                                 (ULONG)sizeof(REQ_LIST),
                                 &reqListPoolSize);
        }

        if (!NT_SUCCESS(status)) {
            return STATUS_BUFFER_OVERFLOW;
        }

        //
        // poolSize= reqListPoolSize + reqAlternativePoolSize + reqDescPoolSize;
        //

        status = RtlULongAdd(reqListPoolSize,
                             reqAlternativePoolSize,
                             &poolSize);

        if (NT_SUCCESS(status)) {
            status = RtlULongAdd(poolSize, reqDescPoolSize, &poolSize);
        }

        if (!NT_SUCCESS(status)) {
            return STATUS_BUFFER_OVERFLOW;
        }

        poolStart = ExAllocatePool(PagedPool, poolSize);
        if (!poolStart) {

            return STATUS_INSUFFICIENT_RESOURCES;
        }
        //
        // Initialize the main pool.
        //
        IopInitPool(&outerPool, poolStart, poolSize);
        //
        // First part of the pool is used by REQ_LIST.
        //
        IopAllocPool(&reqList, &outerPool, reqListPoolSize);
        //
        // Second part of the main pool is used by REQ_ALTERNATIVEs.
        //
        IopAllocPool(&poolStart, &outerPool, reqAlternativePoolSize);
        IopInitPool(&reqAlternativePool, poolStart, reqAlternativePoolSize);
        //
        // Last part of the main pool is used by REQ_DESCs.
        //
        IopAllocPool(&poolStart, &outerPool, reqDescPoolSize);
        IopInitPool(&reqDescPool, poolStart, reqDescPoolSize);
        if (ioResources->InterfaceType == InterfaceTypeUndefined) {

            interfaceType = PnpDefaultInterfaceType;
        } else {

            interfaceType = ioResources->InterfaceType;
        }
        busNumber = ioResources->BusNumber;
        //
        // Initialize REQ_LIST.
        //
        reqList->AlternativeCount       = reqAlternativeCount;
        reqList->Request                = Request;
        reqList->BusNumber              = busNumber;
        reqList->InterfaceType          = interfaceType;
        reqList->SelectedAlternative    = NULL;
        //
        // Initialize memory for REQ_ALTERNATIVEs.
        //
        reqAlternativePP = reqList->AlternativeTable;
        RtlZeroMemory(
            reqAlternativePP,
            reqAlternativeCount * sizeof(PREQ_ALTERNATIVE));
#if DBG
        reqAlternativeEndPP = reqAlternativePP + reqAlternativeCount;
#endif
        reqAlternativeIndex = 0;
        while (--ioResourceListCount >= 0) {

            ioResourceDescriptor    = ioResourceList->Descriptors;
            ioResourceDescriptorEnd = ioResourceDescriptor +
                                        ioResourceList->Count;
            IopAllocPool(
                &reqAlternative,
                &reqAlternativePool,
                FIELD_OFFSET(REQ_ALTERNATIVE, DescTable));
            PNP_ASSERT(reqAlternativePP < reqAlternativeEndPP);
            *reqAlternativePP++ = reqAlternative;
            reqAlternative->ReqList             = reqList;
            reqAlternative->ReqAlternativeIndex = reqAlternativeIndex++;
            reqAlternative->DescCount           = 0;
            //
            // First descriptor of CmResourceTypeConfigData contains priority
            // information.
            //
            if (ioResourceDescriptor->Type == CmResourceTypeConfigData) {

                reqAlternative->Priority = ioResourceDescriptor->u.ConfigData.Priority;
                ioResourceDescriptor++;
            } else {

                reqAlternative->Priority = LCPRI_NORMAL;
            }
            reqDescPP = reqAlternative->DescTable;
            reqDescIndex = 0;
            while (ioResourceDescriptor < ioResourceDescriptorEnd) {

                if (ioResourceDescriptor->Type == CmResourceTypeReserved) {

                    interfaceType = ioResourceDescriptor->u.DevicePrivate.Data[0];
                    if (interfaceType == InterfaceTypeUndefined) {

                        interfaceType = PnpDefaultInterfaceType;
                    }
                    busNumber = ioResourceDescriptor->u.DevicePrivate.Data[1];
                    ioResourceDescriptor++;
                } else {
                    //
                    // Allocate and initialize REQ_DESC.
                    //
                    IopAllocPool(&reqDesc, &reqDescPool, sizeof(REQ_DESC));
                    reqAlternative->DescCount++;
                    *reqDescPP++                    = reqDesc;
#pragma prefast(suppress:__WARNING_BUFFER_OVERFLOW, "Prefast analysis doesn't relate the number of descriptors we find in this loop with the number we found in the first loop when calculating required sizes")
                    reqDesc->ReqAlternative         = reqAlternative;
                    reqDesc->TranslatedReqDesc      = reqDesc;
                    reqDesc->ReqDescIndex           = reqDescIndex++;
                    reqDesc->DevicePrivateCount     = 0;
                    reqDesc->DevicePrivate          = NULL;
                    reqDesc->InterfaceType          = interfaceType;
                    reqDesc->BusNumber              = busNumber;
                    reqDesc->ArbitrationRequired    =
                        (ioResourceDescriptor->Type & CmResourceTypeNonArbitrated ||
                            ioResourceDescriptor->Type == CmResourceTypeNull)?
                                FALSE : TRUE;
                    //
                    // Allocate and initialize arbiter entry for this REQ_DESC.
                    //
                    IopAllocPool(&poolStart, &reqAlternativePool, sizeof(PVOID));
                    PNP_ASSERT((PREQ_DESC*)poolStart == (reqDescPP - 1));
                    arbiterListEntry = &reqDesc->AlternativeTable;
                    InitializeListHead(&arbiterListEntry->ListEntry);
                    arbiterListEntry->AlternativeCount      = 0;
                    arbiterListEntry->Alternatives          = ioResourceDescriptor;
                    arbiterListEntry->PhysicalDeviceObject  = Request->PhysicalDevice;
                    arbiterListEntry->RequestSource         = Request->AllocationType;
                    arbiterListEntry->WorkSpace             = 0;
                    arbiterListEntry->InterfaceType         = interfaceType;
                    arbiterListEntry->SlotNumber            = ioResources->SlotNumber;
                    arbiterListEntry->BusNumber             = ioResources->BusNumber;
                    arbiterListEntry->Assignment            = &reqDesc->Allocation;
                    arbiterListEntry->Result                = ArbiterResultUndefined;
                    arbiterListEntry->Flags                 = 0;
                    if (reqDesc->ArbitrationRequired) {

                        resourcesRequired = TRUE;
                        //
                        // The BestAlternativeTable and BestAllocation are not initialized.
                        // They will be initialized when needed.

                        //
                        // Initialize the Cm partial resource descriptor to NOT_ALLOCATED.
                        //
                        reqDesc->Allocation.Type = CmResourceTypeMaximum;

                        PNP_ASSERT((ioResourceDescriptor->Option & IO_RESOURCE_ALTERNATIVE) == 0);

                        arbiterListEntry->AlternativeCount++;
                        ioResourceDescriptor++;
                        while (ioResourceDescriptor < ioResourceDescriptorEnd) {

                            if (ioResourceDescriptor->Type == CmResourceTypeDevicePrivate) {

                                reqDesc->DevicePrivate = ioResourceDescriptor;
                                while ( ioResourceDescriptor < ioResourceDescriptorEnd &&
                                        ioResourceDescriptor->Type == CmResourceTypeDevicePrivate) {

                                    reqDesc->DevicePrivateCount++;
                                    ioResourceDescriptor++;
                                }
                                break;
                            }
                            if (!(ioResourceDescriptor->Option & IO_RESOURCE_ALTERNATIVE)) {

                                break;
                            }
                            arbiterListEntry->AlternativeCount++;
                            ioResourceDescriptor++;
                        }
                        //
                        // Next query Arbiter and Translator interfaces for the
                        // resource descriptor.
                        //
                        status = IopSetupArbiterAndTranslators(reqDesc);
                        if (!NT_SUCCESS(status)) {

                            IopDbgPrint((
                                IOP_RESOURCE_ERROR_LEVEL, "Unable to setup "
                                "Arbiter and Translators\n"));
                            reqAlternativeIndex--;
                            reqAlternativePP--;
                            reqList->AlternativeCount--;
                            IopFreeReqAlternative(reqAlternative);
                            failureStatus = status;
                            break;
                        }
                    } else {

                        reqDesc->Allocation.Type    = ioResourceDescriptor->Type;
                        reqDesc->Allocation.ShareDisposition =
                            ioResourceDescriptor->ShareDisposition;
                        reqDesc->Allocation.Flags   = ioResourceDescriptor->Flags;
                        reqDesc->Allocation.u.DevicePrivate.Data[0] =
                            ioResourceDescriptor->u.DevicePrivate.Data[0];
                        reqDesc->Allocation.u.DevicePrivate.Data[1] =
                            ioResourceDescriptor->u.DevicePrivate.Data[1];
                        reqDesc->Allocation.u.DevicePrivate.Data[2] =
                            ioResourceDescriptor->u.DevicePrivate.Data[2];

                        if (ioResourceDescriptor->Type == CmResourceTypeConnection) {
                            //
                            // The connection resource is required to be passed to the
                            // driver regardless of any other interesting arbitrable
                            // resources.
                            //
                            resourcesRequired = TRUE;
                        }

                        ioResourceDescriptor++;
                    }
                }
                if (ioResourceDescriptor >= ioResourceDescriptorEnd) {

                    break;
                }
            }
            ioResourceList = (PIO_RESOURCE_LIST)ioResourceDescriptorEnd;
        }
        if (reqAlternativeIndex == 0) {

            finalStatus = failureStatus;
            IopFreeReqList(reqList);
        }
    }

    if (finalStatus == STATUS_SUCCESS) {

        if (resourcesRequired) {

            *ResReqList = reqList;
        } else {

            IopFreeReqList(reqList);
        }
    }
    return finalStatus;

InvalidParameter:

    return STATUS_INVALID_PARAMETER;
}

VOID
IopRearrangeReqList (
    __in PREQ_LIST ReqList
    )

/*++

Routine Description:

    This routine sorts the REQ_LIST in ascending priority order of
    REQ_ALTERNATIVES.

Parameters:

    ReqList - Pointer to the REQ_LIST to be sorted.

Return Value:

    None.

--*/

{
    PPREQ_ALTERNATIVE alternative;
    PPREQ_ALTERNATIVE lastAlternative;
    ULONG i;

    PAGED_CODE();

    if (ReqList->AlternativeCount > 1) {

        for (i = 0; i < ReqList->AlternativeCount; i++) {

            ReqList->AlternativeTable[i]->Position = i;
        }
        qsort(
            (void *)ReqList->AlternativeTable,
            ReqList->AlternativeCount,
            sizeof(PREQ_ALTERNATIVE),
            IopCompareReqAlternativePriority);
    }
    //
    // Set the BestAlternative so that we try alternatives with
    // priority <= LCPRI_LASTSOFTCONFIG.
    //
    alternative = &ReqList->AlternativeTable[0];
    for (   lastAlternative = alternative + ReqList->AlternativeCount;
            alternative < lastAlternative;
            alternative++) {

        if ((*alternative)->Priority > LCPRI_LASTSOFTCONFIG) {

            break;
        }
    }

    if (alternative == &ReqList->AlternativeTable[0]) {

        PDEVICE_NODE deviceNode;

        deviceNode = PP_DO_TO_DN(ReqList->Request->PhysicalDevice);
        IopDbgPrint((
            IOP_RESOURCE_WARNING_LEVEL,
            "Invalid priorities in the logical configs for %wZ\n",
            PnpGetDeviceInstancePath(deviceNode)));
        ReqList->BestAlternative = NULL;
    } else {

        ReqList->BestAlternative = alternative;
    }
}

int
__cdecl
IopCompareReqAlternativePriority (
    __in const void *arg1,
    __in const void *arg2
    )

/*++

Routine Description:

    This function is used in C run time sort. It compares the priority of
    REQ_ALTERNATIVE in arg1 and arg2.

Parameters:

    arg1    - LHS PREQ_ALTERNATIVE

    arg2    - RHS PREQ_ALTERNATIVE

Return Value:

    < 0 if arg1 < arg2
    = 0 if arg1 = arg2
    > 0 if arg1 > arg2

--*/

{
    PREQ_ALTERNATIVE ra1 = *(PPREQ_ALTERNATIVE)arg1;
    PREQ_ALTERNATIVE ra2 = *(PPREQ_ALTERNATIVE)arg2;

    PAGED_CODE();

    if (ra1->Priority == ra2->Priority) {

        if (ra1->Position > ra2->Position) {

            return 1;
        } else if (ra1->Position < ra2->Position) {

            return -1;
        } else {

            PNP_ASSERT(0);
            if ((ULONG_PTR)ra1 < (ULONG_PTR)ra2) {

                return -1;
            } else {

                return 1;
            }
        }
    }
    if (ra1->Priority > ra2->Priority) {

        return 1;
    } else {

        return -1;
    }
}

BOOLEAN
IoTranslateBusAddress(
    __in INTERFACE_TYPE InterfaceType,
    __in ULONG BusNumber,
    __in PHYSICAL_ADDRESS BusAddress,
    __inout PULONG AddressSpace,
    __out PPHYSICAL_ADDRESS TranslatedAddress
    )

/*++

Routine Description:

    This function tries to translate the given bus relative address into a
    processor relative one.  The given address is assumed to be relative to
    the bus specified by the given InterfaceType and BusNumber.

    N.B. Ideally, this routine would fail for addresses that don't really fall
         on the given bus.  Unfortunately, there isn't a mechanism that will
         tell us whether an address falls on a given bus under all of the
         circumstances in which this routine can be called.  As a result this
         function unconditionally applies translations from the devnode
         associated with the given bus back to the root, always succeeding the
         translation.

    N.B. Some legacy drivers (e.g. vga.sys) use initialization schemes that
         require HalTranslateBusAddress to fail when passed certain invalid
         addresses that will be successfully translated by this routine.  In
         order to account for this, we assume that translations for PCI
         devices are given special treatment and are routed through the PCI
         driver, where a determination can be made about whether an address is
         valid on a given bus, before they arrive here.

Arguments:

    InterfaceType - Provides the type of bus associated with the address
        that we're translating.

    BusNumber - Supplies the bus number associated with the address being
        translated.

    BusAddress - Gives the bus relative address that needs to be translated
        into a processor relative equivalent.

    AddressSpace - On input, this parameter indicates which address space
        contains the given BusAddress.  On output, we'll store the address
        space that should be used from the processor to access the given bus
        relative address.  This accounts for things like memory to I/O
        translation.  This implementation supports ADDRESS_SPACE_MEMORY (0)
        and ADDRESS_SPACE_PORT (1).

    TranslatedAddress - Provides the location that we should use to store
        the address that should be used, within the returned address space, to
        access the given bus relative address from the processor.

Return Value:

    TranslateBusAddress returns TRUE when the translation succeeds and FALSE
    when it fails, indicating an attempt to translate an address that isn't
    valid on the given bus.  For the reasons discussed above, this function
    will only return FALSE in the presence of insufficient resources or a
    failure code from one of the invoked translators.

--*/

{
    BOOLEAN TranslatorFound;
    PDEVICE_NODE DeviceNode;
    PINTERFACE Interface;
    PTRANSLATOR_INTERFACE Translator;
    PPI_RESOURCE_TRANSLATOR_ENTRY TranslatorEntry;
    CM_RESOURCE_TYPE ResourceType;
    CM_PARTIAL_RESOURCE_DESCRIPTOR RawDescriptor;
    CM_PARTIAL_RESOURCE_DESCRIPTOR TranslatedDescriptor;
    USHORT ResourceFlags;
    NTSTATUS Status;
    KIRQL Irql;

    //
    // When carrying out this translation, we may have to query different
    // device stacks for translator interfaces.  The IRP_MN_QUERY_INTERFACE
    // IRPs that carry this out need to be sent at PASSIVE_LEVEL.
    //
    // Even if QUERY_INTERFACE IRPs aren't needed, the translators themselves
    // expect to be called below DISPATCH_LEVEL.  They may be able to run at
    // APC_LEVEL, however no documentation exists on translators so we'll err
    // on the side of caution and never call them above PASSIVE_LEVEL.
    //
    // As a result, we revert to the original HAL behavior of trivially
    // succeeding this translation if the IRQL is too high.
    //

    Irql = KeGetCurrentIrql();

    if (Irql > PASSIVE_LEVEL) {
        *TranslatedAddress = BusAddress;
        return TRUE;
    }

    //
    // This routine can be called very early during system initialization.  If
    // we're being called before phase 0 PnP manager initialization has
    // completed, then just succeed the translation and return.
    //

    if (IopRootDeviceNode == NULL) {
        *TranslatedAddress = BusAddress;
        return TRUE;
    }

    //
    // Convert the given bus-relative address into the partial resource
    // descriptor format that will be expected by the translators.
    //

    if (*AddressSpace == ADDRESS_SPACE_MEMORY) {
        ResourceType = CmResourceTypeMemory;
        ResourceFlags = CM_RESOURCE_MEMORY_READ_WRITE;

    } else if (*AddressSpace == ADDRESS_SPACE_PORT) {
        ResourceType = CmResourceTypePort;
        ResourceFlags = CM_RESOURCE_PORT_IO;

    } else {
        return FALSE;
    }

    RawDescriptor.Type = (UCHAR) ResourceType;
    RawDescriptor.ShareDisposition = CmResourceShareShared;
    RawDescriptor.Flags = ResourceFlags;
    RawDescriptor.u.Generic.Start = BusAddress;
    RawDescriptor.u.Generic.Length = 1;

    //
    // Lock the device node tree so it won't change while we are traversing it
    // searching for translators.
    //
    // N.B. No extra synchronization is required when calling
    //      IopFindLegacyBusDeviceNode.  The only race condition of concern is
    //      one where removal of a device node corrupts a legacy bus device
    //      node list while this routine is traversing it.  Addition to these
    //      lists is done in a manner that never puts the list into an
    //      inconsistent state.  The device tree lock prevents the remove race
    //      since removal is always done with the device tree lock acquired at
    //      PPL_TREEOP_BLOCK_READS level.
    //
    // N.B. It might be nice to acquire the resource manager lock to ensure
    //      that no concurrent resource allocation operation is contending for
    //      access to the translators.  That said, this routine is called both
    //      with and without the resource manager lock held and we have no way
    //      of distinguishing between the two cases.  Whether the lock is held
    //      depends upon whether the driver calls this routine in the context
    //      of an IRP that is sent by the kernel mode PnP manager with the
    //      resource manager lock held.
    //
    //      This isn't disastrous since the lock shouldn't be strictly
    //      necessary.  Translators shouldn't maintain state across calls and
    //      should therefore be able to deal with being invoked from multiple
    //      places (e.g. this routine and a concurrent operation somewhere
    //      else).
    //

    PpDevNodeLockTree(PPL_SIMPLE_READ);

    //
    // Next map the given InterfaceType and BusNumber to the device node that
    // owns the corresponding bus.  We'll walk from there to the root to catch
    // the needed translators.
    //
    // N.B. This routine returns IopRootDeviceNode if the given legacy bus
    //      identifier can't be mapped to a device node.
    //

    DeviceNode = IopFindLegacyBusDeviceNode(InterfaceType, BusNumber);

    //
    // Now walk back to the root, calling every translator we run into along
    // the way.
    //

    while (DeviceNode != IopRootDeviceNode) {

        //
        // Check if an appropriate translator has been cached at this device
        // node.
        //

        TranslatorFound = IopFindResourceHandlerInfo(
                               ResourceTranslator,
                               DeviceNode,
                               RawDescriptor.Type,
                               &TranslatorEntry);

        if (TranslatorFound == FALSE) {

            //
            // An appropriate translator hasn't been cached, try querying
            // for one.  If this doesn't return a translator, continue on to
            // our parent in the device tree.
            //

            Status = IopQueryResourceHandlerInterface(
                            ResourceTranslator,
                            DeviceNode->PhysicalDeviceObject,
                            RawDescriptor.Type,
                            &Interface);

            if (!NT_SUCCESS(Status)) {
                goto MoveToParent;
            }

            Translator = (PTRANSLATOR_INTERFACE) Interface;

        //
        // IopFindResourceHandlerInfo returns TRUE with a NULL TranslatorEntry
        // to indicate that we've already queried for this translator and
        // found that it doesn't exist on this devnode.  In this case just
        // move on to our parent.
        //

        } else if (TranslatorEntry == NULL) {

            goto MoveToParent;

        //
        // If IopFindResourceHandlerInfo returned TRUE along with a non-NULL
        // TranslatorEntry then we know that we have a translator.
        //

        } else {

            Translator = TranslatorEntry->TranslatorInterface;

        }

        //
        // At this point we have a translator in hand.  Call into it to
        // translate our current raw resources up one level in the device
        // tree.
        //

        Status = Translator->TranslateResources(
                        Translator->Context,
                        &RawDescriptor,
                        TranslateChildToParent,
                        0,
                        NULL,
                        NULL,
                        &TranslatedDescriptor);

        //
        // If we just queried this translator interface, release it before
        // checking the return code to simplify error handling.
        //

        if (TranslatorFound == FALSE) {
            Translator->InterfaceDereference(Translator->Context);
            ExFreePool(Translator);
        }

        //
        // Assume that the translation has failed if the translator returned
        // an error code.
        //

        if (!NT_SUCCESS(Status)) {
            PpDevNodeUnlockTree(PPL_SIMPLE_READ);
            return FALSE;
        }

        //
        // If we just called a non-hierarchical translator that returned
        // STATUS_TRANSLATION_COMPLETE, then break out of the loop.  Otherwise
        // move up to our parent in the device tree and continue.
        //

        RtlCopyMemory(&RawDescriptor,
                      &TranslatedDescriptor,
                      sizeof(RawDescriptor));

        if (Status == STATUS_TRANSLATION_COMPLETE) {
            break;
        }

MoveToParent:

        DeviceNode = DeviceNode->Parent;

    }

    PpDevNodeUnlockTree(PPL_SIMPLE_READ);

    //
    // If we make it out of the loop then we were able to call all of the
    // relevant translators.  Convert the resulting translated resources into
    // the address space and translated address that will be returned to the
    // caller.
    //
    // N.B. We need to read RawDescriptor here in case the given bus number
    //      mapped to the root and we never executed the above loop.
    //

    ResourceType = (CM_RESOURCE_TYPE) RawDescriptor.Type;

    if ((ResourceType == CmResourceTypeMemory) ||
        (ResourceType == CmResourceTypeMemoryLarge)) {
        *AddressSpace = ADDRESS_SPACE_MEMORY;

    } else if (ResourceType == CmResourceTypePort) {
        *AddressSpace = ADDRESS_SPACE_PORT;

    } else {
        PNP_ASSERT(FALSE);
        return FALSE;
    }

    *TranslatedAddress = RawDescriptor.u.Generic.Start;

    return TRUE;
}

NTSTATUS
IopSetupArbiterAndTranslators(
    __in PREQ_DESC ReqDesc
    )

/*++

Routine Description:

    This routine searches the arbiter and translators which arbitrates and translate
    the resources for the specified device.  This routine tries to find all the
    translator on the path of current device node to root device node

Parameters:

    ReqDesc - supplies a pointer to REQ_DESC which contains all the required information

Return Value:

    NTSTATUS value to indicate success or failure.

--*/

{
    PLIST_ENTRY listHead;
    PPI_RESOURCE_ARBITER_ENTRY arbiterEntry;
    PDEVICE_OBJECT deviceObject = ReqDesc->AlternativeTable.PhysicalDeviceObject;
    PDEVICE_NODE deviceNode;
    PREQ_DESC reqDesc = ReqDesc, translatedReqDesc;
    BOOLEAN found, arbiterFound = FALSE, restartedAlready;
    BOOLEAN  searchTranslator = TRUE, translatorFound = FALSE;
    NTSTATUS status;
    PPI_RESOURCE_TRANSLATOR_ENTRY translatorEntry;
    UCHAR resourceType = ReqDesc->TranslatedReqDesc->AlternativeTable.Alternatives->Type;
    PINTERFACE interface;
    USHORT resourceMask;

    // 
    // Correction for large resource descriptors.  We want arbiters to treat
    // a large memory resource descriptor as they would a regular memory descriptor.
    //

    if (resourceType == CmResourceTypeMemoryLarge) {
        resourceType = CmResourceTypeMemory;
    }

    if ((ReqDesc->AlternativeTable.RequestSource == ArbiterRequestHalReported) &&
        (ReqDesc->InterfaceType == Internal)) {

        // Trust hal if it says internal bus.

        restartedAlready = TRUE;
    } else {
        restartedAlready = FALSE;
    }

    //
    // If ReqDesc contains DeviceObject, this is for regular resources allocation
    // or boot resources preallocation.  Otherwise, it is for resources reservation.
    //

    if (deviceObject && ReqDesc->AlternativeTable.RequestSource != ArbiterRequestHalReported) {
        deviceNode = PP_DO_TO_DN(deviceObject);
        // We want to start with the deviceNode instead of its parent.  Because the
        // deviceNode may provide a translator interface.
        // deviceNode = deviceNode->Parent;
    } else {

        //
        // For resource reservation, we always need to find the arbiter and translators
        // so set the device node to Root.
        //

        deviceNode = IopRootDeviceNode;
    }
    while (deviceNode) {
        if ((deviceNode == IopRootDeviceNode) && (translatorFound == FALSE)) {

            //
            // If we reach the root and have not find any translator, the device is on the
            // wrong way.
            //

            if (restartedAlready == FALSE) {
                restartedAlready = TRUE;

                deviceNode = IopFindLegacyBusDeviceNode (
                                 ReqDesc->InterfaceType,
                                 ReqDesc->BusNumber
                                 );

                //
                // If we did not find a PDO, try again with InterfaceType == Isa. This allows
                // drivers that request Internal to get resources even if there is no PDO
                // that is Internal. (but if there is an Internal PDO, they get that one)
                //

                if ((deviceNode == IopRootDeviceNode) &&
                    (ReqDesc->ReqAlternative->ReqList->InterfaceType == Internal)) {
                    deviceNode = IopFindLegacyBusDeviceNode(
                                 Isa,
                                 0
                                 );
                }

                //if ((PVOID)deviceNode == deviceObject->DeviceObjectExtension->DeviceNode) {
                //    deviceNode = IopRootDeviceNode;
                //} else {
                    continue;
                //}
            }
        }

        //
        // Check is there an arbiter for the device node?
        //   if yes, set up ReqDesc->u.Arbiter and set ArbiterFound to true.
        //   else move up to the parent of current device node.
        //

        if ((arbiterFound == FALSE) && (deviceNode->PhysicalDeviceObject != deviceObject)) {
            found = IopFindResourceHandlerInfo(
                               ResourceArbiter,
                               deviceNode,
                               resourceType,
                               &arbiterEntry);
            if (found == FALSE) {

                //
                // no information found on arbiter.  Try to query arbiter interface ...
                //

                if (resourceType <= PI_MAXIMUM_RESOURCE_TYPE_TRACKED) {
                    resourceMask = 1 << resourceType;
                } else {
                    resourceMask = 0;
                }
                status = IopQueryResourceHandlerInterface(ResourceArbiter,
                                                          deviceNode->PhysicalDeviceObject,
                                                          resourceType,
                                                          &interface);
                deviceNode->QueryArbiterMask |= resourceMask;
                if (!NT_SUCCESS(status)) {
                    deviceNode->NoArbiterMask |= resourceMask;
                    if (resourceType <= PI_MAXIMUM_RESOURCE_TYPE_TRACKED) {
                        found = TRUE;
                    } else {
                        interface = NULL;
                    }
                }
                if (found == FALSE) {
                    arbiterEntry = (PPI_RESOURCE_ARBITER_ENTRY)ExAllocatePool(
                                       PagedPool,
                                       sizeof(PI_RESOURCE_ARBITER_ENTRY));
                    if (!arbiterEntry) {
                        status = STATUS_INSUFFICIENT_RESOURCES;
                        return status;
                    }
                    IopInitializeArbiterEntryState(arbiterEntry);
                    InitializeListHead(&arbiterEntry->DeviceArbiterList);
                    arbiterEntry->ResourceType      = resourceType;
                    arbiterEntry->DeviceNode        = deviceNode;
                    listHead = &deviceNode->DeviceArbiterList;
                    InsertTailList(listHead, &arbiterEntry->DeviceArbiterList);
                    arbiterEntry->ArbiterInterface = (PARBITER_INTERFACE)interface;
                    if (!interface) {

                        //
                        // if interface is NULL we really don't have arbiter.
                        //

                        arbiterEntry = NULL;
                    }
                }
            }

            //
            // If there is an desired resourcetype arbiter in the device node, make sure
            // it handle this resource requriements.
            //

            if (arbiterEntry) {
                arbiterFound = TRUE;
                if (arbiterEntry->ArbiterInterface->Flags & ARBITER_PARTIAL) {

                    //
                    // If the arbiter is partial, ask if it handles the resources
                    // if not, goto its parent.
                    //

                    status = IopCallArbiter(
                                arbiterEntry,
                                ArbiterActionQueryArbitrate,
                                ReqDesc->TranslatedReqDesc,
                                NULL,
                                NULL
                                );
                    if (!NT_SUCCESS(status)) {
                        arbiterFound = FALSE;
                    }
                }
            }
            if (arbiterFound) {
                ReqDesc->u.Arbiter = arbiterEntry;

                //
                // Initialize the arbiter entry
                //

                arbiterEntry->State = 0;
                arbiterEntry->ResourcesChanged = FALSE;
            }

        }

        if (searchTranslator) {
            //
            // First, check if there is a translator for the device node?
            // If yes, translate the req desc and link it to the front of ReqDesc->TranslatedReqDesc
            // else do nothing.
            //

            found = IopFindResourceHandlerInfo(
                        ResourceTranslator,
                        deviceNode,
                        resourceType,
                        &translatorEntry);

            if (found == FALSE) {

                //
                // no information found on translator.  Try to query translator interface ...
                //

                if (resourceType <= PI_MAXIMUM_RESOURCE_TYPE_TRACKED) {
                    resourceMask = 1 << resourceType;
                } else {
                    resourceMask = 0;
                }
                status = IopQueryResourceHandlerInterface(ResourceTranslator,
                                                          deviceNode->PhysicalDeviceObject,
                                                          resourceType,
                                                          &interface);
                deviceNode->QueryTranslatorMask |= resourceMask;
                if (!NT_SUCCESS(status)) {
                    deviceNode->NoTranslatorMask |= resourceMask;
                    if (resourceType <= PI_MAXIMUM_RESOURCE_TYPE_TRACKED) {
                        found = TRUE;
                    } else {
                        interface = NULL;
                    }
                }
                if (found == FALSE) {
                    translatorEntry = (PPI_RESOURCE_TRANSLATOR_ENTRY)ExAllocatePool(
                                       PagedPool,
                                       sizeof(PI_RESOURCE_TRANSLATOR_ENTRY));
                    if (!translatorEntry) {
                        status = STATUS_INSUFFICIENT_RESOURCES;
                        return status;
                    }
                    translatorEntry->ResourceType = resourceType;
                    InitializeListHead(&translatorEntry->DeviceTranslatorList);
                    translatorEntry->TranslatorInterface = (PTRANSLATOR_INTERFACE)interface;
                    translatorEntry->DeviceNode = deviceNode;
                    listHead = &deviceNode->DeviceTranslatorList;
                    InsertTailList(listHead, &translatorEntry->DeviceTranslatorList);
                    if (!interface) {

                        //
                        // if interface is NULL we really don't have translator.
                        //

                        translatorEntry = NULL;
                    }
                }
            }
            if (translatorEntry) {
                translatorFound = TRUE;
            }
            if ((arbiterFound == FALSE) && translatorEntry) {

                //
                // Find a translator to translate the req desc ... Translate it and link it to
                // the front of ReqDesc->TranslatedReqDesc such that the first in the list is for
                // the Arbiter to use.
                //

                reqDesc = ReqDesc->TranslatedReqDesc;
                status = IopTranslateAndAdjustReqDesc(
                              reqDesc,
                              translatorEntry,
                              &translatedReqDesc);
                if (NT_SUCCESS(status)) {
                    PNP_ASSERT(translatedReqDesc);
                    resourceType = translatedReqDesc->AlternativeTable.Alternatives->Type;

                    //
                    // Compensate for large memory descriptors.  They must be treated the same
                    // as regular memory descriptors.
                    //

                    if (resourceType == CmResourceTypeMemoryLarge) {
                        resourceType = CmResourceTypeMemory;
                    }
                    
                    translatedReqDesc->TranslatedReqDesc = ReqDesc->TranslatedReqDesc;
                    ReqDesc->TranslatedReqDesc = translatedReqDesc;
                    //
                    // If the translator is non-hierarchial and performs a complete
                    // translation to root (eg ISA interrups for PCI devices) then
                    // don't pass translations to parent.
                    //

                    if (status == STATUS_TRANSLATION_COMPLETE) {
                        searchTranslator = FALSE;
                    }
                } else {
                    IopDbgPrint((
                        IOP_RESOURCE_INFO_LEVEL,
                        "resreq list TranslationAndAdjusted failed\n"
                        ));
                    return status;
                }
            }

        }

        //
        // Move up to current device node's parent
        //

        deviceNode = deviceNode->Parent;
    }

    if (arbiterFound) {

        return STATUS_SUCCESS;
    } else {
        //
        // We should BugCheck in this case.
        //
        IopDbgPrint((
            IOP_RESOURCE_ERROR_LEVEL,
            "can not find resource type %x arbiter\n",
            resourceType));

        PNP_ASSERT(arbiterFound);

        return STATUS_RESOURCE_TYPE_NOT_FOUND;
    }

}

BOOLEAN
IopFindResourceHandlerInfo (
    __in  RESOURCE_HANDLER_TYPE    HandlerType,
    __in  PDEVICE_NODE             DeviceNode,
    __in  UCHAR                    ResourceType,
    __out PVOID                   *HandlerEntry
    )

/*++

Routine Description:

    This routine finds the desired resource handler interface for the specified
    resource type in the specified Device node.

Parameters:

    HandlerType     - Specifies the type of handler needed.

    DeviceNode      - Specifies the devicenode on which to search for handler.

    ResourceType    - Specifies the type of resource.

    HandlerEntry    - Supplies a pointer to a variable to receive the handler.

Return Value:

    TRUE + non-NULL HandlerEntry : Found handler info and there is a handler
    TRUE + NULL HandlerEntry     : Found handler info but there is NO handler
    FALSE + NULL HandlerEntry    : No handler info found

--*/
{
    USHORT                      resourceMask;
    USHORT                      noHandlerMask;
    USHORT                      queryHandlerMask;
    PLIST_ENTRY                 listHead;
    PLIST_ENTRY                 entry;
    PPI_RESOURCE_ARBITER_ENTRY  arbiterEntry;

    *HandlerEntry   = NULL;
    switch (HandlerType) {
    case ResourceArbiter:

        noHandlerMask       = DeviceNode->NoArbiterMask;
        queryHandlerMask    = DeviceNode->QueryArbiterMask;
        listHead            = &DeviceNode->DeviceArbiterList;
        break;

    case ResourceTranslator:

        noHandlerMask       = DeviceNode->NoTranslatorMask;
        queryHandlerMask    = DeviceNode->QueryTranslatorMask;
        listHead            = &DeviceNode->DeviceTranslatorList;
        break;

    default:

        return FALSE;
    }
    resourceMask    = 1 << ResourceType;
    if (noHandlerMask & resourceMask) {
        //
        // There is no desired handler for the resource type.
        //
        return TRUE;
    }
    if (    (queryHandlerMask & resourceMask) ||
            ResourceType > PI_MAXIMUM_RESOURCE_TYPE_TRACKED) {

        entry = listHead->Flink;
        while (entry != listHead) {

            arbiterEntry = CONTAINING_RECORD(
                                entry,
                                PI_RESOURCE_ARBITER_ENTRY,
                                DeviceArbiterList);
            if (arbiterEntry->ResourceType == ResourceType) {

                if (    ResourceType <= PI_MAXIMUM_RESOURCE_TYPE_TRACKED ||
                        arbiterEntry->ArbiterInterface) {

                    *HandlerEntry = arbiterEntry;
                }
                return TRUE;
            }
            entry = entry->Flink;
        }
        if (queryHandlerMask & resourceMask) {
            //
            // There must be one.
            //
            PNP_ASSERT(entry != listHead);
        }
    }

    return FALSE;
}

NTSTATUS
IopCallArbiter(
    __in PPI_RESOURCE_ARBITER_ENTRY ArbiterEntry,
    __in ARBITER_ACTION Command,
    __in_opt PVOID Input1,
    __in_opt  PVOID Input2,
    __in_opt PVOID Input3
    )

/*++

Routine Description:

    This routine builds a Parameter block from Input structure and calls specified
    arbiter to carry out the Command.

Parameters:

    ArbiterEntry - Supplies a pointer to our PI_RESOURCE_ARBITER_ENTRY such that
                   we know everything about the arbiter.

    Command - Supplies the Action code for the arbiter.

    Input - Supplies a PVOID pointer to a structure.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/
{
    ARBITER_PARAMETERS parameters;
    PARBITER_INTERFACE arbiterInterface = ArbiterEntry->ArbiterInterface;
    NTSTATUS status;
    PARBITER_LIST_ENTRY arbiterListEntry;
    LIST_ENTRY listHead;
    PVOID *ExtParams;

    switch (Command) {
    case ArbiterActionTestAllocation:

        //
        // For ArbiterActionTestAllocation, the Input is a pointer to the doubly
        // linked list of ARBITER_LIST_ENTRY's.
        //

        parameters.Parameters.TestAllocation.ArbitrationList = (PLIST_ENTRY)Input1;
        parameters.Parameters.TestAllocation.AllocateFromCount = (ULONG)((ULONG_PTR)Input2);
        parameters.Parameters.TestAllocation.AllocateFrom =
                                            (PCM_PARTIAL_RESOURCE_DESCRIPTOR)Input3;

        status = (arbiterInterface->ArbiterHandler)(arbiterInterface->Context,
                                                    Command,
                                                    &parameters);
        break;

    case ArbiterActionRetestAllocation:

        //
        // For ArbiterActionRetestAllocation, the Input is a pointer to the doubly
        // linked list of ARBITER_LIST_ENTRY's.
        //

        parameters.Parameters.RetestAllocation.ArbitrationList = (PLIST_ENTRY)Input1;
        parameters.Parameters.RetestAllocation.AllocateFromCount = (ULONG)((ULONG_PTR)Input2);
        parameters.Parameters.RetestAllocation.AllocateFrom =
                                            (PCM_PARTIAL_RESOURCE_DESCRIPTOR)Input3;

        status = (arbiterInterface->ArbiterHandler)(arbiterInterface->Context,
                                                    Command,
                                                    &parameters);

    case ArbiterActionBootAllocation:

        //
        // For ArbiterActionBootAllocation, the input is a pointer to the doubly
        // linked list of ARBITER_LIST_ENTRY'S.
        //

        parameters.Parameters.BootAllocation.ArbitrationList = (PLIST_ENTRY)Input1;

        status = (arbiterInterface->ArbiterHandler)(
                      arbiterInterface->Context,
                      Command,
                      &parameters
                      );
        break;

    case ArbiterActionQueryArbitrate:

        //
        // For QueryArbiter, the input is a pointer to REQ_DESC
        //

        arbiterListEntry = &((PREQ_DESC)Input1)->AlternativeTable;
        PNP_ASSERT(IsListEmpty(&arbiterListEntry->ListEntry));
        listHead = arbiterListEntry->ListEntry;
        arbiterListEntry->ListEntry.Flink = arbiterListEntry->ListEntry.Blink = &listHead;
        parameters.Parameters.QueryArbitrate.ArbitrationList = &listHead;
        status = (arbiterInterface->ArbiterHandler)(
                      arbiterInterface->Context,
                      Command,
                      &parameters
                      );
        arbiterListEntry->ListEntry = listHead;
        break;

    case ArbiterActionCommitAllocation:
    case ArbiterActionWriteReservedResources:

        //
        // Commit, Rollback and WriteReserved do not have parmater.
        //

        status = (arbiterInterface->ArbiterHandler)(
                      arbiterInterface->Context,
                      Command,
                      NULL
                      );
        break;

    case ArbiterActionQueryAllocatedResources:
        status = STATUS_NOT_IMPLEMENTED;
        break;

    case ArbiterActionQueryConflict:
        //
        // For QueryConflict
        // Ex0 is PDO
        // Ex1 is PIO_RESOURCE_DESCRIPTOR
        // Ex2 is PULONG
        // Ex3 is PARBITER_CONFLICT_INFO *
        ExtParams = (PVOID*)Input1;

        parameters.Parameters.QueryConflict.PhysicalDeviceObject = (PDEVICE_OBJECT)ExtParams[0];
        parameters.Parameters.QueryConflict.ConflictingResource = (PIO_RESOURCE_DESCRIPTOR)ExtParams[1];
        parameters.Parameters.QueryConflict.ConflictCount = (PULONG)ExtParams[2];
        parameters.Parameters.QueryConflict.Conflicts = (PARBITER_CONFLICT_INFO *)ExtParams[3];
        status = (arbiterInterface->ArbiterHandler)(
                      arbiterInterface->Context,
                      Command,
                      &parameters
                      );
        break;

    default:
        status = STATUS_INVALID_PARAMETER;
        break;
    }

    return status;
}

NTSTATUS
IopTranslateAndAdjustReqDesc(
    __in PREQ_DESC ReqDesc,
    __in PPI_RESOURCE_TRANSLATOR_ENTRY TranslatorEntry,
    __out PREQ_DESC *TranslatedReqDesc
    )

/*++

Routine Description:

    This routine translates and adjusts ReqDesc IoResourceDescriptors to
    their translated and adjusted form.

Parameters:

    ReqDesc - supplies a pointer to the REQ_DESC to be translated.

    TranslatorEntry - supplies a pointer to the translator infor structure.

    TranslatedReqDesc - supplies a pointer to a variable to receive the
                        translated REQ_DESC.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/
{
    ULONG i, total = 0, *targetCount;
    PTRANSLATOR_INTERFACE translator = TranslatorEntry->TranslatorInterface;
    PIO_RESOURCE_DESCRIPTOR ioDesc, *target, tIoDesc;
    PREQ_DESC tReqDesc;
    PARBITER_LIST_ENTRY arbiterEntry;
    NTSTATUS status = STATUS_UNSUCCESSFUL, returnStatus = STATUS_SUCCESS;
    BOOLEAN reqTranslated = FALSE;

    if (ReqDesc->AlternativeTable.AlternativeCount == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    *TranslatedReqDesc = NULL;

    target = (PIO_RESOURCE_DESCRIPTOR *) ExAllocatePool(
                           PagedPool,
                           sizeof(PIO_RESOURCE_DESCRIPTOR) * ReqDesc->AlternativeTable.AlternativeCount
                           );
    if (target == NULL) {
        IopDbgPrint((
            IOP_RESOURCE_WARNING_LEVEL,
            "Not Enough memory to perform resreqlist adjustment\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    RtlZeroMemory(target, sizeof(PIO_RESOURCE_DESCRIPTOR) * ReqDesc->AlternativeTable.AlternativeCount);

    targetCount = (PULONG) ExAllocatePool(
                           PagedPool,
                           sizeof(ULONG) * ReqDesc->AlternativeTable.AlternativeCount
                           );
    if (targetCount == NULL) {
        IopDbgPrint((
            IOP_RESOURCE_WARNING_LEVEL,
            "Not Enough memory to perform resreqlist adjustment\n"));
        ExFreePool(target);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(targetCount, sizeof(ULONG) * ReqDesc->AlternativeTable.AlternativeCount);

    //
    // Determine the number of IO_RESOURCE_DESCRIPTORs after translation.
    //

    ioDesc = ReqDesc->AlternativeTable.Alternatives;
    for (i = 0; i < ReqDesc->AlternativeTable.AlternativeCount; i++) {
        status = (translator->TranslateResourceRequirements)(
                           translator->Context,
                           ioDesc,
                           ReqDesc->AlternativeTable.PhysicalDeviceObject,
                           &targetCount[i],
                           &target[i]
                           );
        if (!NT_SUCCESS(status) || targetCount[i] == 0) {
            IopDbgPrint((
                IOP_RESOURCE_WARNING_LEVEL,
                "Translator failed to adjust resreqlist\n"));
            target[i] = ioDesc;
            targetCount[i] = 0;
            total++;
        } else {
            total += targetCount[i];
            reqTranslated = TRUE;
        }
        ioDesc++;
        if (NT_SUCCESS(status) && (returnStatus != STATUS_TRANSLATION_COMPLETE)) {
            returnStatus = status;
        }
    }

    if (!reqTranslated) {

        IopDbgPrint((
            IOP_RESOURCE_WARNING_LEVEL,
            "Failed to translate any requirement for %wZ!\n",
            PnpGetDeviceInstancePath(PP_DO_TO_DN(ReqDesc->AlternativeTable.PhysicalDeviceObject))));
        returnStatus = status;
    }

    //
    // Allocate memory for the adjusted/translated resources descriptors
    //

    tIoDesc = (PIO_RESOURCE_DESCRIPTOR) ExAllocatePool(
                           PagedPool,
                           total * sizeof(IO_RESOURCE_DESCRIPTOR));
    if (!tIoDesc) {
        IopDbgPrint((
            IOP_RESOURCE_WARNING_LEVEL,
            "Not Enough memory to perform resreqlist adjustment\n"));
        returnStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    tReqDesc = (PREQ_DESC) ExAllocatePool(PagedPool, sizeof(REQ_DESC));
    if (tReqDesc == NULL) {
        IopDbgPrint((
            IOP_RESOURCE_WARNING_LEVEL,
            "Not Enough memory to perform resreqlist adjustment\n"));
        ExFreePool(tIoDesc);
        returnStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    //
    // Create and initialize a new REQ_DESC for the translated/adjusted io resources
    //

    RtlCopyMemory(tReqDesc, ReqDesc, sizeof(REQ_DESC));

    //
    // Set the translated req desc's ReqAlternative to NULL to indicated this
    // is not the original req desc.
    //

    tReqDesc->ReqAlternative = NULL;

    tReqDesc->u.Translator = TranslatorEntry;
    tReqDesc->TranslatedReqDesc = NULL;
    arbiterEntry = &tReqDesc->AlternativeTable;
    InitializeListHead(&arbiterEntry->ListEntry);
    arbiterEntry->AlternativeCount = total;
    arbiterEntry->Alternatives = tIoDesc;
    arbiterEntry->Assignment = &tReqDesc->Allocation;

    ioDesc = ReqDesc->AlternativeTable.Alternatives;
    for (i = 0; i < ReqDesc->AlternativeTable.AlternativeCount; i++) {
        if (targetCount[i] != 0) {
            RtlCopyMemory(tIoDesc, target[i], targetCount[i] * sizeof(IO_RESOURCE_DESCRIPTOR));
            tIoDesc += targetCount[i];
        } else {

            //
            // Make it become impossible to satisfy.
            //

            RtlCopyMemory(tIoDesc, ioDesc, sizeof(IO_RESOURCE_DESCRIPTOR));
            switch (tIoDesc->Type) {
            case CmResourceTypePort:
            case CmResourceTypeMemory:
            case CmResourceTypeMemoryLarge:
                tIoDesc->u.Port.MinimumAddress.LowPart = 2;
                tIoDesc->u.Port.MinimumAddress.HighPart = 0;
                tIoDesc->u.Port.MaximumAddress.LowPart = 1;
                tIoDesc->u.Port.MaximumAddress.HighPart = 0;
                break;
            case CmResourceTypeBusNumber:
                tIoDesc->u.BusNumber.MinBusNumber = 2;
                tIoDesc->u.BusNumber.MaxBusNumber = 1;
                break;

            case CmResourceTypeInterrupt:
                tIoDesc->u.Interrupt.MinimumVector = 2;
                tIoDesc->u.Interrupt.MaximumVector = 1;
                break;

            case CmResourceTypeDma:
                tIoDesc->u.Dma.MinimumChannel = 2;
                tIoDesc->u.Dma.MaximumChannel = 1;
                break;
            default:
                PNP_ASSERT(0);
                break;
            }
            tIoDesc += 1;
        }
        ioDesc++;

    }

#if DBG
    //
    // Verify the adjusted resource descriptors are valid
    //

    ioDesc = arbiterEntry->Alternatives;
    PNP_ASSERT((ioDesc->Option & IO_RESOURCE_ALTERNATIVE) == 0);
    ioDesc++;
    for (i = 1; i < total; i++) {
        PNP_ASSERT(ioDesc->Option & IO_RESOURCE_ALTERNATIVE);
        ioDesc++;
    }
#endif
    *TranslatedReqDesc = tReqDesc;
exit:
    for (i = 0; i < ReqDesc->AlternativeTable.AlternativeCount; i++) {
        if (targetCount[i] != 0) {
            PNP_ASSERT(target[i]);
            ExFreePool(target[i]);
        }
    }
    ExFreePool(target);
    ExFreePool(targetCount);
    return returnStatus;
}

NTSTATUS
PnpLookupArbitersNewResources (
    __in_ecount(RequestTableCount) PPNP_RESOURCE_REQUEST RequestTable,
    __in ULONG                                RequestTableCount,
    __in PDEVICE_NODE                         DeviceNode,
    __in UCHAR                                ResourceType,
    __out PCM_PARTIAL_RESOURCE_DESCRIPTOR     *AllocateFrom,
    __out PULONG                              AllocateFromCount
    )

/*++

Routine Description:

    This routine looks up the resource of the specified type computed for the arbiter devnode.
    This gets used to initialize the AllocateFrom field of ARBITER_PARAMETERS for ArbiterActionTestAllocation
    and ArbiterActionRetestAllocation.

Parameters:

    RequestTable        - Table of resource requests being processed.

    RequestTableCount   - Number of entries in the request table.

    DeviceNode          - Arbiter devnode.

    ResourceType        - Resource type to be looked up.

    AllocateFrom        - The descriptors of ResourceType get returned in this.

    AllocateFromCount   - Number of descriptors found.

Return Value:

    STATUS_SUCCESS if lookup is successful, else STATUS_UNSUCCESSFUL.

--*/

{
    PREQ_ALTERNATIVE reqAlternative;
    PREQ_DESC reqDesc;
    ULONG i, count;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR cmDescriptor;

    PAGED_CODE();
    //
    // Lookup the arbiter devnode in the current request table.
    //
    for (i = 0; i < RequestTableCount; i++) {

        if (RequestTable[i].PhysicalDevice == DeviceNode->PhysicalDeviceObject) {

            break;
        }
    }
    if (i < RequestTableCount) {
        //
        // Count the number of descriptors of the specified type.
        //
        count = 0;
        reqAlternative = *((PREQ_LIST)RequestTable[i].ReqList)->SelectedAlternative;
        for (i = 0; i < reqAlternative->DescCount; i++) {

            reqDesc = reqAlternative->DescTable[i]->TranslatedReqDesc;

            // 
            // We must explicity map large memory resource descriptors
            // to the regular memory type to ensure they are treated
            // the same
            //
            
            if ((reqDesc->Allocation.Type == ResourceType) ||
                (reqDesc->Allocation.Type == CmResourceTypeMemoryLarge 
                    && ResourceType == CmResourceTypeMemory)) {

                count++;
            }
        }
        if (count != 0) {

            //
            // Allocate and fill in the descriptors of specified type.
            //
            cmDescriptor = (PCM_PARTIAL_RESOURCE_DESCRIPTOR) ExAllocatePool(PagedPool,
                                                                            sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR) *
                                                                            count);
            if (cmDescriptor == NULL) {

                return STATUS_INSUFFICIENT_RESOURCES;
            }

            *AllocateFromCount  = count;
            *AllocateFrom       = cmDescriptor;
            for (i = 0; i < reqAlternative->DescCount; i++) {

                reqDesc = reqAlternative->DescTable[i]->TranslatedReqDesc;

                // 
                // We must explicity map large memory resource descriptors
                // to the regular memory type to ensure they are treated
                // the same
                //
                
                if ((reqDesc->Allocation.Type == ResourceType) ||
                    (reqDesc->Allocation.Type == CmResourceTypeMemoryLarge 
                    && ResourceType == CmResourceTypeMemory)) {

                    PNP_ASSERT(count > 0);
                    count--;
                    *cmDescriptor = reqDesc->Allocation;
                    cmDescriptor++;
                }
            }
        }

        return STATUS_SUCCESS;
    }

    return STATUS_UNSUCCESSFUL;
}

NTSTATUS
IopParentToRawTranslation(
    __inout PREQ_DESC ReqDesc
    )

/*++

Routine Description:

    This routine translates an CmPartialResourceDescriptors
    from their translated form to their raw counterparts..

Parameters:

    ReqDesc - supplies a translated ReqDesc to be translated back to its raw form

Return Value:

    Status code that indicates whether or not the function was successful.

--*/
{
    PTRANSLATOR_INTERFACE translator;
    NTSTATUS status = STATUS_SUCCESS;
    PREQ_DESC rawReqDesc;

    if (ReqDesc->AlternativeTable.AlternativeCount == 0 ||

        ReqDesc->Allocation.Type == CmResourceTypeMaximum) {
        IopDbgPrint((
            IOP_RESOURCE_ERROR_LEVEL,
            "Invalid ReqDesc for parent-to-raw translation.\n"));

        return STATUS_INVALID_PARAMETER;
    }

    //
    // If this ReqDesc is the raw reqDesc then we are done.
    // Else call its translator to translate the resource and leave the result
    // in its raw (next level) reqdesc.
    //

    if (IS_TRANSLATED_REQ_DESC(ReqDesc)) {
        rawReqDesc = ReqDesc->TranslatedReqDesc;
        translator = ReqDesc->u.Translator->TranslatorInterface;
        status = (translator->TranslateResources)(
                      translator->Context,
                      ReqDesc->AlternativeTable.Assignment,
                      TranslateParentToChild,
                      rawReqDesc->AlternativeTable.AlternativeCount,
                      rawReqDesc->AlternativeTable.Alternatives,
                      rawReqDesc->AlternativeTable.PhysicalDeviceObject,
                      rawReqDesc->AlternativeTable.Assignment
                      );
        if (NT_SUCCESS(status)) {

            //
            // If the translator is non-hierarchial and performs a complete
            // translation to root (eg ISA interrups for PCI devices) then
            // don't pass translations to parent.
            //

            PNP_ASSERT(status != STATUS_TRANSLATION_COMPLETE);
            status = IopParentToRawTranslation(rawReqDesc);
        }
    }
    return status;
}

NTSTATUS
IopChildToRootTranslation(
    __in_opt PDEVICE_NODE DeviceNode,
    __in INTERFACE_TYPE InterfaceType,
    __in ULONG BusNumber,
    __in ARBITER_REQUEST_SOURCE ArbiterRequestSource,
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Source,
    __out PCM_PARTIAL_RESOURCE_DESCRIPTOR *Target
    )

/*++

Routine Description:

    This routine translates a CmPartialResourceDescriptors from
    their intermediate translated form to their final translated form.
    The translated CM_PARTIAL_RESOURCE_DESCRIPTOR is returned via Target variable.

    The caller is responsible to release the translated descriptor.

Parameters:

    DeviceNode - Specified the device object.  If The DeviceNode is specified,
                 the InterfaceType and BusNumber are ignored and we will
                 use DeviceNode as a starting point to find various translators to
                 translate the Source descriptor.  If DeviceNode is not specified,
                 the InterfaceType and BusNumber must be specified.

    InterfaceType, BusNumber - must be supplied if DeviceNode is not specified.

    Source - A pointer to the resource descriptor to be translated.

    Target - Supplies an address to receive the translated resource descriptor.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/
{
    PDEVICE_OBJECT deviceObject;
    PDEVICE_NODE currentDeviceNode;
    PLIST_ENTRY listHead, nextEntry;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR target, source, tmp;
    PPI_RESOURCE_TRANSLATOR_ENTRY translatorEntry;
    PTRANSLATOR_INTERFACE translator;
    NTSTATUS status = STATUS_SUCCESS;
    BOOLEAN done = FALSE, foundTranslator = FALSE, restartedAlready;

    if (ArbiterRequestSource == ArbiterRequestHalReported) {
       restartedAlready = TRUE;
    } else {
       restartedAlready = FALSE;
    }

    source = (PCM_PARTIAL_RESOURCE_DESCRIPTOR) ExAllocatePool(
                         PagedPool,
                         sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)
                         );
    if (source == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    target = (PCM_PARTIAL_RESOURCE_DESCRIPTOR) ExAllocatePool(
                         PagedPool,
                         sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)
                         );
    if (target == NULL) {
        ExFreePool(source);
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    *source = *Source;

    //
    // Move up to current device node's parent to start translation
    //

    if (!ARGUMENT_PRESENT(DeviceNode)) {
        currentDeviceNode = IopFindLegacyBusDeviceNode (InterfaceType, BusNumber);
        deviceObject = NULL;
    } else {
        // We want to start with the deviceNode instead of its parent.  Because the
        // deviceNode may provide a translator interface.
        currentDeviceNode = DeviceNode;
        deviceObject = DeviceNode->PhysicalDeviceObject;
    }
    while (currentDeviceNode && !done) {

        if ((currentDeviceNode == IopRootDeviceNode) && (foundTranslator == FALSE)) {
            if (restartedAlready == FALSE) {
                restartedAlready = TRUE;
                currentDeviceNode = IopFindLegacyBusDeviceNode (InterfaceType, BusNumber);

                //
                // If we did not find a PDO, try again with InterfaceType == Isa. This allows
                // drivers that request Internal to get resources even if there is no PDO
                // that is Internal. (but if there is an Internal PDO, they get that one)
                //

                if ((currentDeviceNode == IopRootDeviceNode) && (InterfaceType == Internal)) {
                    currentDeviceNode = IopFindLegacyBusDeviceNode(Isa, 0);
                }

                continue;
            }
        }
        //
        // First, check if there is a translator for the device node?
        // If yes, translate the req desc and link it to the front of ReqDesc->TranslatedReqDesc
        // else do nothing.
        //

        listHead = &currentDeviceNode->DeviceTranslatorList;
        nextEntry = listHead->Flink;
        for (; nextEntry != listHead; nextEntry = nextEntry->Flink) {
            translatorEntry = CONTAINING_RECORD(nextEntry, PI_RESOURCE_TRANSLATOR_ENTRY, DeviceTranslatorList);
            if (translatorEntry->ResourceType == Source->Type) {
                translator = translatorEntry->TranslatorInterface;
                if (translator != NULL) {

                    //
                    // Find a translator to translate the req desc ... Translate it and link it to
                    // the front of ReqDesc->TranslatedReqDesc.
                    //

                    status = (translator->TranslateResources) (
                                  translator->Context,
                                  source,
                                  TranslateChildToParent,
                                  0,
                                  NULL,
                                  deviceObject,
                                  target
                                  );
                    if (NT_SUCCESS(status)) {
                        tmp = source;
                        source = target;
                        target = tmp;

                        //
                        // If the translator is non-hierarchial and performs a complete
                        // translation to root (eg ISA interrups for PCI devices) then
                        // don't pass translations to parent.
                        //

                        if (status == STATUS_TRANSLATION_COMPLETE) {
                            done = TRUE;
                        }

                    } else {

                        if(DeviceNode && (status != STATUS_RETRY)) {

                            IopDbgPrint((
                                IOP_RESOURCE_ERROR_LEVEL,
                                "Child to Root Translation failed with status %x\n"
                                "        DeviceNode %08x (PDO %08x)\n"
                                "        Resource Type %02x Data %08x %08x %08x\n",
                                status,
                                DeviceNode,
                                DeviceNode->PhysicalDeviceObject,
                                source->Type,
                                source->u.DevicePrivate.Data[0],
                                source->u.DevicePrivate.Data[1],
                                source->u.DevicePrivate.Data[2]
                                ));

                            PNP_ASSERT(FALSE);

                            IopRecordTranslationFailure(DeviceNode, *source);
                        }
                        goto exit;
                    }
                }
                break;
            }
        }

        //
        // Move up to current device node's parent
        //

        currentDeviceNode = currentDeviceNode->Parent;
    }
    *Target = source;
    ExFreePool(target);
    return status;
exit:
    ExFreePool(source);
    ExFreePool(target);
    return status;
}

#if DBG

VOID
PiDumpArbiterListEntry(
    __in PARBITER_LIST_ENTRY  ArbiterListEntry,
    __in BOOLEAN              NewOwner
    )
{
    ULONG i;

    if (NewOwner) {
        //
        // Dump owner information if owner changed.
        //
        IopDbgPrint((IOP_RESOURCE_VERBOSE_LEVEL,
                     "Owner %wZ, Flags 0x%x, Interface 0x%x, Source 0x%x\n",
                     PnpGetDeviceInstancePath(PP_DO_TO_DN(ArbiterListEntry->PhysicalDeviceObject)),
                     ArbiterListEntry->Flags,
                     ArbiterListEntry->InterfaceType,
                     ArbiterListEntry->RequestSource));
    }

    //
    // Dump all the alternatives in this entry.
    //
    for (i = 0; i < ArbiterListEntry->AlternativeCount; i++) {

        PnpDumpResourceDescriptor("    ", &ArbiterListEntry->Alternatives[i]);

        if (i < (ArbiterListEntry->AlternativeCount - 1)) {

            IopDbgPrint((IOP_RESOURCE_VERBOSE_LEVEL, "|\n"));
        }
    }
}

VOID
PiDumpArbiterList(
    __in PLIST_ENTRY ArbitrationList
    )
{
    PLIST_ENTRY entry;
    PARBITER_LIST_ENTRY arbiterListEntry;
    BOOLEAN newOwner;
    PDEVICE_OBJECT previousOwner;

    IopDbgPrint((IOP_RESOURCE_VERBOSE_LEVEL,
                 "***ARBITRATION LIST***\n"));

    previousOwner = NULL;
    newOwner = FALSE;
    entry = ArbitrationList->Flink;
    while (entry != ArbitrationList) {

        arbiterListEntry = CONTAINING_RECORD(entry, ARBITER_LIST_ENTRY, ListEntry);

        //
        // Check if device object has changed.
        //
        if (previousOwner != arbiterListEntry->PhysicalDeviceObject) {

            previousOwner = arbiterListEntry->PhysicalDeviceObject;
            newOwner = TRUE;
        }

        PiDumpArbiterListEntry(arbiterListEntry, newOwner);

        if (newOwner) {

            newOwner = FALSE;
        }

        entry = entry->Flink;

        if (entry != ArbitrationList) {

            IopDbgPrint((IOP_RESOURCE_VERBOSE_LEVEL, "&\n"));
        }
    }
}

VOID
IopCheckDataStructures (
    __in PDEVICE_NODE DeviceNode
    )

{
    PDEVICE_NODE    sibling;

    PAGED_CODE();

    //
    // Process all the siblings.
    //
    for (sibling = DeviceNode; sibling; sibling = sibling->Sibling) {

        IopCheckDataStructuresWorker(sibling);
    }
    for (sibling = DeviceNode; sibling; sibling = sibling->Sibling) {
        //
        // Recursively check all the children.
        //
        if (sibling->Child) {
            IopCheckDataStructures(sibling->Child);
        }
    }
}

VOID
IopCheckDataStructuresWorker (
    __in PDEVICE_NODE Device
    )

/*++

Routine Description:

    This routine sanity checks the arbiter related data structures for the
    specified device.

Parameters:

    DeviceNode - Device node whose structures are to be checked.

Return Value:

    None.

--*/

{
    PLIST_ENTRY listHead, listEntry;
    PPI_RESOURCE_ARBITER_ENTRY arbiterEntry;

    PAGED_CODE();

    listHead    = &Device->DeviceArbiterList;
    listEntry   = listHead->Flink;
    while (listEntry != listHead) {

        arbiterEntry = CONTAINING_RECORD(
                        listEntry,
                        PI_RESOURCE_ARBITER_ENTRY,
                        DeviceArbiterList);
        if (arbiterEntry->ArbiterInterface != NULL) {

            if (!IsListEmpty(&arbiterEntry->ResourceList)) {
                IopDbgPrint((
                    IOP_RESOURCE_ERROR_LEVEL,
                    "Arbiter on %wZ should have empty resource list\n",
                    PnpGetDeviceInstancePath(Device)));

                PNP_ASSERT(FALSE);
            }
            if (!IsListEmpty(&arbiterEntry->ActiveArbiterList)) {
                IopDbgPrint((
                    IOP_RESOURCE_ERROR_LEVEL,
                    "Arbiter on %wZ should not be in the active arbiter list\n",
                    PnpGetDeviceInstancePath(Device)));

                PNP_ASSERT(FALSE);
            }
        }
        listEntry = listEntry->Flink;
    }
}
#endif // DBG

