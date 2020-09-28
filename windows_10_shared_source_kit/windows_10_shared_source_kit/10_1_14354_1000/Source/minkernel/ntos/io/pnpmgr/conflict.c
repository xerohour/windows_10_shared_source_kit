/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    conflict.c

Abstract:

    This module implements the handling of hardware resource conflicts.

Author:

    Santosh Jodh (santoshj) 08-02-2004

Environment:

    KernelMode only.

Revision History:

--*/

#include "pnpmgrp.h"
#pragma hdrstop

#ifdef ALLOC_PRAGMA

#pragma alloc_text(PAGE, IopQueryConflictList)
#pragma alloc_text(PAGE, IopQueryConflictListInternal)
#pragma alloc_text(PAGE, IopQueryConflictFillConflicts)
#pragma alloc_text(PAGE, IopQueryConflictFillString)
//#pragma alloc_text(NONPAGE, IopEliminateBogusConflict)

#endif // ALLOC_PRAGMA

NTSTATUS
IopQueryConflictList(
    __in PDEVICE_OBJECT        PhysicalDeviceObject,
    __in_bcount(ResourceListSize) PCM_RESOURCE_LIST  ResourceList,
    __in_range(>=, sizeof(CM_RESOURCE_LIST)) ULONG              ResourceListSize,
    __out_bcount(ConflictListSize) PPLUGPLAY_CONTROL_CONFLICT_LIST ConflictList,
    __in ULONG              ConflictListSize,
    __in ULONG              Flags
    )
/*++

Routine Description:

    This routine performs the querying of device conflicts
    returning data in ConflictList

Arguments:

    PhysicalDeviceObject PDO of device to Query
    ResourceList      CM resource list containing single resource to query
    ResourceListSize  Size of ResourceList
    ConflictList      Conflict list to fill query details in
    ConflictListSize  Size of buffer that we can fill with Conflict information
    Flags             Currently unused (zero) for future passing of flags

Return Value:

    Should be success in most cases

--*/
{
    NTSTATUS status;

    PAGED_CODE();

    IopLockResourceManager();

    status = IopQueryConflictListInternal(PhysicalDeviceObject, ResourceList, ResourceListSize, ConflictList, ConflictListSize, Flags);

    IopUnlockResourceManager();

    return status;
}



BOOLEAN
IopEliminateBogusConflict(
    __in PDEVICE_OBJECT   PhysicalDeviceObject,
    __in PDEVICE_OBJECT   ConflictDeviceObject
    )
/*++

Routine Description:

    Determine if we're really conflicting with ourselves
    if this is the case, we ignore it

Arguments:

    PhysicalDeviceObject  PDO we're performing the test for
    ConflictDeviceObject  The object we've determined is conflicting

Return Value:

    TRUE to eliminate the conflict

--*/
{
    PDEVICE_NODE deviceNode;
    PDRIVER_OBJECT driverObject;
    KIRQL           irql;
    PDEVICE_OBJECT  attachedDevice;

    //
    // simple cases
    //
    if (PhysicalDeviceObject == NULL || ConflictDeviceObject == NULL) {
        return FALSE;
    }
    //
    // if ConflictDeviceObject is on PDO's stack, this is a non-conflict
    // nb at least PDO has to be checked
    //
    irql = KeAcquireQueuedSpinLock( LockQueueIoDatabaseLock );

    for (attachedDevice = PhysicalDeviceObject;
         attachedDevice;
         attachedDevice = attachedDevice->AttachedDevice) {

        if (attachedDevice == ConflictDeviceObject) {
            KeReleaseQueuedSpinLock( LockQueueIoDatabaseLock, irql );
            return TRUE;
        }
    }

    KeReleaseQueuedSpinLock( LockQueueIoDatabaseLock, irql );

    //
    // legacy case
    //
    deviceNode = PP_DO_TO_DN(PhysicalDeviceObject);
    PNP_ASSERT(deviceNode);
    if (deviceNode->Flags & DNF_LEGACY_DRIVER) {
        //
        // hmmm, let's see if our ConflictDeviceObject is resources associated with a legacy device
        //
        if (ConflictDeviceObject->Flags & DO_BUS_ENUMERATED_DEVICE) {
            //
            // if not, we have a legacy conflicting with non-legacy, we're interested!
            //
            return FALSE;
        }
        //
        // FDO, report driver name
        //
        driverObject = ConflictDeviceObject->DriverObject;
        if(driverObject == NULL) {
            //
            // should not be NULL
            //
            PNP_ASSERT(driverObject);
            return FALSE;
        }
        //
        // compare deviceNode->Service with driverObject->Service
        //
        if (deviceNode->ServiceName.Length != 0 &&
            deviceNode->ServiceName.Length == driverObject->DriverExtension->ServiceKeyName.Length &&
            RtlCompareUnicodeString(&deviceNode->ServiceName,&driverObject->DriverExtension->ServiceKeyName,TRUE)==0) {
            //
            // the driver's service name is the same that this PDO is associated with
            // by ignoring it we could end up ignoring conflicts of simular types of legacy devices
            // but since these have to be hand-config'd anyhow, it's prob better than having false conflicts
            //
            return TRUE;
        }

    }
    return FALSE;
}


NTSTATUS
IopQueryConflictFillString(
    __in PDEVICE_OBJECT        DeviceObject,
    __out_ecount_opt(*Length) PWSTR  Buffer,
    __inout PULONG             Length,
    __inout_opt PULONG             Flags
    )
/*++

Routine Description:

    Obtain string or string-length for details of conflicting device

Arguments:

    DeviceObject        Device object we want Device-Instance-String or Service Name
    Buffer              Buffer to Fill, NULL if we just want length
    Length              Filled with length of Buffer, including terminated NULL (Words)
    Flags               Apropriate flags set describing what the string represents

Return Value:

    Should be success in most cases

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    PDEVICE_NODE deviceNode;
    PDRIVER_OBJECT driverObject;
    PUNICODE_STRING infoString = NULL;
    ULONG MaxLength = 0;        // words
    ULONG ReqLength = 0;        // words
    ULONG flags = 0;

    PAGED_CODE();

    if (Length != NULL) {
        MaxLength = *Length;
    }

    if (Flags != NULL) {
        flags = *Flags;
    }

    if (DeviceObject == NULL) {
        //
        // unknown
        //
        goto final;

    }

    if ((DeviceObject->Flags & DO_BUS_ENUMERATED_DEVICE) == 0 ) {
        //
        // FDO, report driver name
        //
        driverObject = DeviceObject->DriverObject;
        if(driverObject == NULL) {
            //
            // should not be NULL
            //
            PNP_ASSERT(driverObject);
            goto final;
        }
        infoString = & (driverObject->DriverName);
        flags |= PNP_CE_LEGACY_DRIVER;
        goto final;
    }

    //
    // we should in actual fact have a PDO
    //
    if (DeviceObject->DeviceObjectExtension == NULL) {
        //
        // should not be NULL
        //
        PNP_ASSERT(DeviceObject->DeviceObjectExtension);
        goto final;
    }

    deviceNode = PP_DO_TO_DN(DeviceObject);
    if (deviceNode == NULL) {
        //
        // should not be NULL
        //
        PNP_ASSERT(deviceNode);
        goto final;
    }

    if (deviceNode == IopRootDeviceNode) {
        //
        // owned by root device
        //
        flags |= PNP_CE_ROOT_OWNED;

    } else if (deviceNode -> Parent == NULL) {
        //
        // faked out PDO - must be legacy device
        //
        driverObject = (PDRIVER_OBJECT)(deviceNode->DuplicatePDO);
        if(driverObject == NULL) {
            //
            // should not be NULL
            //
            PNP_ASSERT(driverObject);
            goto final;
        }
        infoString = & (driverObject->DriverName);
        flags |= PNP_CE_LEGACY_DRIVER;
        goto final;
    }

    //
    // we should be happy with what we have
    //
    infoString = PnpGetDeviceInstancePath(deviceNode);

final:

    if (infoString != NULL) {
        //
        // we have a string to copy
        //
        if ((Buffer != NULL) && (MaxLength*sizeof(WCHAR) > infoString->Length)) {
            RtlCopyMemory(Buffer, infoString->Buffer, infoString->Length);
        }
        ReqLength += infoString->Length / sizeof(WCHAR);
    }

    if ((Buffer != NULL) && (MaxLength > ReqLength)) {
        Buffer[ReqLength] = 0;
    }

    ReqLength++;

    if (Length != NULL) {
        *Length = ReqLength;
    }
    if (Flags != NULL) {
        *Flags = flags;
    }

    return status;
}


NTSTATUS
IopQueryConflictFillConflicts(
    __in_opt PDEVICE_OBJECT                  PhysicalDeviceObject,
    __in ULONG                        ConflictCount,
    __in_ecount_opt(ConflictCount) PARBITER_CONFLICT_INFO       ConflictInfoList,
    __out_bcount(ConflictListSize) PPLUGPLAY_CONTROL_CONFLICT_LIST ConflictList,
    __in ULONG                        ConflictListSize,
    __in ULONG                        Flags
    )
/*++

Routine Description:

    Fill ConflictList with information on as many conflicts as possible

Arguments:

    PhysicalDeviceObject The PDO we're performing the test on
    ConflictCount       Number of Conflicts.
    ConflictInfoList    List of conflicting device info, can be NULL if ConflictCount is 0
    ConflictList        Structure to fill in with conflicts
    ConflictListSize    Size of Conflict List
    Flags               if non-zero, dummy conflict is created

Return Value:

    Should be success in most cases

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG ConflictListIdealSize;
    ULONG ConflictListCount;
    ULONG Index;
    ULONG ConflictIndex = 0;
    ULONG EntrySize;
    ULONG ConflictStringsOffset;
    ULONG stringSize;
    ULONG stringTotalSize;
    ULONG DummyCount;
    PPLUGPLAY_CONTROL_CONFLICT_STRINGS ConfStrings;

    PAGED_CODE();

    //
    // determine how many conflicts we can
    //
    // for each conflict
    // translate to bus/resource/address in respect to conflicting device
    // add to conflict list
    //
    //

    //
    // preprocessing - given our ConflictInfoList and ConflictCount
    // remove any that appear to be bogus - ie, that are the same device that we are testing against
    // this stops mostly legacy issues
    //
    for(Index = 0;Index < ConflictCount; Index++) {
        if (IopEliminateBogusConflict(PhysicalDeviceObject,ConflictInfoList[Index].OwningObject)) {

            IopDbgPrint((
                IOP_RESOURCE_VERBOSE_LEVEL,
                "IopQueryConflictFillConflicts: eliminating \"identical\" PDO"
                " %08x conflicting with self (%08x)\n",
                ConflictInfoList[Index].OwningObject,
                PhysicalDeviceObject));
            //
            // move the last listed conflict into this space
            //
            if (Index+1 < ConflictCount) {
                RtlCopyMemory(&ConflictInfoList[Index],&ConflictInfoList[ConflictCount-1],sizeof(ARBITER_CONFLICT_INFO));
            }
            //
            // account for deleting this item
            //
            ConflictCount--;
            Index--;
        }
    }

    //
    // preprocessing - in our conflict list, we may have PDO's for legacy devices, and resource nodes for the same
    // or other duplicate entities (we only ever want to report a conflict once, even if there's multiple conflicting ranges)
    //

  RestartScan:

    for(Index = 0;Index < ConflictCount; Index++) {
        if (ConflictInfoList[Index].OwningObject != NULL) {

            ULONG Index2;

            for (Index2 = Index+1; Index2 < ConflictCount; Index2++) {
                if (IopEliminateBogusConflict(ConflictInfoList[Index].OwningObject,ConflictInfoList[Index2].OwningObject)) {
                    //
                    // Index2 is considered a dup of Index
                    //

                    IopDbgPrint((
                        IOP_RESOURCE_VERBOSE_LEVEL,
                        "IopQueryConflictFillConflicts: eliminating \"identical\" PDO"
                        " %08x conflicting with PDO %08x\n",
                        ConflictInfoList[Index2].OwningObject,
                        ConflictInfoList[Index].OwningObject));
                    //
                    // move the last listed conflict into this space
                    //
                    if (Index2+1 < ConflictCount) {
                        RtlCopyMemory(&ConflictInfoList[Index2],&ConflictInfoList[ConflictCount-1],sizeof(ARBITER_CONFLICT_INFO));
                    }
                    //
                    // account for deleting this item
                    //
                    ConflictCount--;
                    Index2--;
                } else if (IopEliminateBogusConflict(ConflictInfoList[Index2].OwningObject,ConflictInfoList[Index].OwningObject)) {
                    //
                    // Index is considered a dup of Index2 (some legacy case)
                    //
                    IopDbgPrint((
                        IOP_RESOURCE_VERBOSE_LEVEL,
                        "IopQueryConflictFillConflicts: eliminating \"identical\" PDO"
                        " %08x conflicting with PDO %08x\n",
                        ConflictInfoList[Index2].OwningObject,
                        ConflictInfoList[Index].OwningObject));
                    //
                    // move the one we want (Index2) into the space occupied by Index
                    //
                    RtlCopyMemory(&ConflictInfoList[Index],&ConflictInfoList[Index2],sizeof(ARBITER_CONFLICT_INFO));
                    //
                    // move the last listed conflict into the space we just created
                    //
                    if (Index2+1 < ConflictCount) {
                        RtlCopyMemory(&ConflictInfoList[Index2],&ConflictInfoList[ConflictCount-1],sizeof(ARBITER_CONFLICT_INFO));
                    }
                    //
                    // account for deleting this item
                    //
                    ConflictCount--;
                    //
                    // but as this is quirky, restart the scan
                    //
                    goto RestartScan;
                }
            }
        }
    }

    //
    // preprocessing - if we have any known reported conflicts, don't report back any unknown
    //

    for(Index = 0;Index < ConflictCount; Index++) {
        //
        // find first unknown
        //
        if (ConflictInfoList[Index].OwningObject == NULL) {
            //
            // eliminate all other unknowns
            //

            ULONG Index2;

            for (Index2 = Index+1; Index2 < ConflictCount; Index2++) {
                if (ConflictInfoList[Index2].OwningObject == NULL) {

                    IopDbgPrint((
                        IOP_RESOURCE_VERBOSE_LEVEL,
                        "IopQueryConflictFillConflicts: eliminating extra"
                        " unknown\n"));
                    //
                    // move the last listed conflict into this space
                    //
                    if (Index2+1 < ConflictCount) {
                        RtlCopyMemory(&ConflictInfoList[Index2],&ConflictInfoList[ConflictCount-1],sizeof(ARBITER_CONFLICT_INFO));
                    }
                    //
                    // account for deleting this item
                    //
                    ConflictCount--;
                    Index2--;
                }
            }

            if(ConflictCount != 1) {

                IopDbgPrint((
                    IOP_RESOURCE_VERBOSE_LEVEL,
                    "IopQueryConflictFillConflicts: eliminating first unknown\n"
                    ));
                //
                // there were others, so ignore the unknown
                //
                if (Index+1 < ConflictCount) {
                    RtlCopyMemory(&ConflictInfoList[Index],&ConflictInfoList[ConflictCount-1],sizeof(ARBITER_CONFLICT_INFO));
                }
                ConflictCount --;
            }

            break;
        }
    }

    //
    // set number of actual and listed conflicts
    //

    ConflictListIdealSize = (sizeof(PLUGPLAY_CONTROL_CONFLICT_LIST) - sizeof(PLUGPLAY_CONTROL_CONFLICT_ENTRY)) + sizeof(PLUGPLAY_CONTROL_CONFLICT_STRINGS);
    ConflictListCount = 0;
    stringTotalSize = 0;
    DummyCount = 0;

    PNP_ASSERT(ConflictListSize >= ConflictListIdealSize); // we should have checked to see if buffer is at least this big
    __analysis_assume(ConflictListSize >= ConflictListIdealSize);

    IopDbgPrint((
        IOP_RESOURCE_VERBOSE_LEVEL,
        "IopQueryConflictFillConflicts: Detected %d conflicts\n",
        ConflictCount));

    //
    // estimate sizes
    //
    if (Flags) {
        //
        // flags entry required (ie resource not available for some specified reason)
        //
        stringSize = 1; // null-length string
        EntrySize = sizeof(PLUGPLAY_CONTROL_CONFLICT_ENTRY);
        EntrySize += sizeof(WCHAR) * stringSize;

        if((ConflictListIdealSize+EntrySize) <= ConflictListSize) {
            //
            // we can fit this one in
            //
            ConflictListCount++;
            DummyCount++;
            stringTotalSize += stringSize;
        }
        ConflictListIdealSize += EntrySize;
    }
    //
    // report conflicts
    //
    for(Index = 0; Index < ConflictCount; Index ++) {

        stringSize = 0;
        IopQueryConflictFillString(ConflictInfoList[Index].OwningObject,NULL,&stringSize,NULL);

        //
        // account for entry
        //
        EntrySize = sizeof(PLUGPLAY_CONTROL_CONFLICT_ENTRY);
        EntrySize += sizeof(WCHAR) * stringSize;

        if((ConflictListIdealSize+EntrySize) <= ConflictListSize) {
            //
            // we can fit this one in
            //
            ConflictListCount++;
            stringTotalSize += stringSize;
        }
        ConflictListIdealSize += EntrySize;
    }

    ConflictList->ConflictsCounted = ConflictCount+DummyCount; // number of conflicts detected including any dummy conflict
    ConflictList->ConflictsListed = ConflictListCount;         // how many we could fit in
    ConflictList->RequiredBufferSize = ConflictListIdealSize;  // how much buffer space to supply on next call

    IopDbgPrint((
        IOP_RESOURCE_VERBOSE_LEVEL,
        "IopQueryConflictFillConflicts: Listing %d conflicts\n",
        ConflictListCount));
    IopDbgPrint((
        IOP_RESOURCE_VERBOSE_LEVEL,
        "IopQueryConflictFillConflicts: Need %08x bytes to list all conflicts\n",
        ConflictListIdealSize));

    //
    // Find the end of the array of conflict entries.  This is where the conflict strings will start.
    // There should always be atleast enough space for a single string entry at the end of the buffer.
    // If this is not the case, then fail.
    // 
    
    ConfStrings = (PPLUGPLAY_CONTROL_CONFLICT_STRINGS)&(ConflictList->ConflictEntry[ConflictListCount]);
    if ((ULONG_PTR)ConfStrings > 
        ((ULONG_PTR)ConflictList + ConflictListSize - 
         sizeof(PLUGPLAY_CONTROL_CONFLICT_STRINGS))){
        
       status = STATUS_BUFFER_TOO_SMALL; 
       goto exit;
    } 
    
    ConfStrings->NullDeviceInstance = (ULONG)(-1);
    ConflictStringsOffset = 0;
    if (DummyCount != 0) {
        ConflictList->ConflictEntry[0].DeviceInstance = ConflictStringsOffset;
        ConflictList->ConflictEntry[0].DeviceFlags = Flags;
        ConflictList->ConflictEntry[0].ResourceType = 0;
        ConflictList->ConflictEntry[0].ResourceStart = 0;
        ConflictList->ConflictEntry[0].ResourceEnd = 0;
        ConflictList->ConflictEntry[0].ResourceFlags = 0;

        ConfStrings->DeviceInstanceStrings[0] = 0; // null string
        stringTotalSize--;
        ConflictStringsOffset++;
        ConflictIndex = 1;
        IopDbgPrint((IOP_RESOURCE_VERBOSE_LEVEL,
                     "IopQueryConflictFillConflicts: Listing flags %08x\n",
                     Flags));
    }
    
    //
    // get/fill in details for all those we can fit into the buffer
    //
    for(Index = 0; ConflictIndex < ConflictListCount ; Index++, ConflictIndex++) {

        PNP_ASSERT(Index < ConflictCount);
        //
        // assign conflict information
        //
        ConflictList->ConflictEntry[ConflictIndex].DeviceInstance = ConflictStringsOffset;
        ConflictList->ConflictEntry[ConflictIndex].DeviceFlags = 0;
        ConflictList->ConflictEntry[ConflictIndex].ResourceType = 0; // NYI
        ConflictList->ConflictEntry[ConflictIndex].ResourceStart = (ULONGLONG)(1); // for now, return totally invalid range (1-0)
        ConflictList->ConflictEntry[ConflictIndex].ResourceEnd = 0;
        ConflictList->ConflictEntry[ConflictIndex].ResourceFlags = 0;

        //
        // fill string details
        //
        stringSize = stringTotalSize;
        IopQueryConflictFillString(ConflictInfoList[Index].OwningObject,
                                    &(ConfStrings->DeviceInstanceStrings[ConflictStringsOffset]),
                                    &stringSize,
                                    &(ConflictList->ConflictEntry[ConflictIndex].DeviceFlags));
        stringTotalSize -= stringSize;
        IopDbgPrint((
            IOP_RESOURCE_VERBOSE_LEVEL,
            "IopQueryConflictFillConflicts: Listing \"%S\"\n",
            &(ConfStrings->DeviceInstanceStrings[ConflictStringsOffset])));
        ConflictStringsOffset += stringSize;
    }

    //
    // another NULL at end of strings (this is accounted for in the PPLUGPLAY_CONTROL_CONFLICT_STRINGS structure)
    //
#pragma prefast(suppress:__WARNING_POTENTIAL_BUFFER_OVERFLOW_HIGH_PRIORITY, "PreFAST can't determine that each call to IopQueryConflictFillString will return the same string length given the same device object.")
    ConfStrings->DeviceInstanceStrings[ConflictStringsOffset] = 0;

exit:
    return status;
}

NTSTATUS
IopQueryConflictListInternal(
    __in PDEVICE_OBJECT        PhysicalDeviceObject,
    __in_bcount(ResourceListSize) PCM_RESOURCE_LIST  ResourceList,
    __in_range(>=, sizeof(CM_RESOURCE_LIST)) ULONG              ResourceListSize,
    __out_bcount(ConflictListSize) PPLUGPLAY_CONTROL_CONFLICT_LIST ConflictList,
    __in ULONG              ConflictListSize,
    __in ULONG              Flags
    )
/*++

Routine Description:

    Version of IopQueryConflictList without the locking

--*/
{
    NTSTATUS status;
    PDEVICE_NODE deviceNode;
    PIO_RESOURCE_REQUIREMENTS_LIST ioResources;
    PREQ_LIST reqList;
    PREQ_DESC reqDesc, reqDescTranslated;
    PPI_RESOURCE_ARBITER_ENTRY arbiterEntry;
    PREQ_ALTERNATIVE RA;
    PPREQ_ALTERNATIVE reqAlternative;
    ULONG ConflictCount;
    PARBITER_CONFLICT_INFO ConflictInfoList;
    PIO_RESOURCE_DESCRIPTOR ConflictDesc = NULL;
    ULONG ReqDescCount;
    PREQ_DESC *ReqDescTable;
    PIO_RESOURCE_REQUIREMENTS_LIST pIoReqList;
    PVOID ExtParams[4];
    PNP_RESOURCE_REQUEST request;

    UNREFERENCED_PARAMETER(Flags);
    UNREFERENCED_PARAMETER(ResourceListSize);

    PAGED_CODE();

    PNP_ASSERT(PhysicalDeviceObject);
    PNP_ASSERT(ResourceList);
    PNP_ASSERT(ResourceListSize >= sizeof(CM_RESOURCE_LIST));
    PNP_ASSERT(ResourceList->Count == 1);
    PNP_ASSERT(ResourceList->List[0].PartialResourceList.Count == 1);
    PNP_ASSERT(ConflictList);
    PNP_ASSERT(ConflictListSize >= MIN_CONFLICT_LIST_SIZE);
    __analysis_assume(ConflictListSize >= MIN_CONFLICT_LIST_SIZE);

    //
    // Initialize locals so we can cleanup on the way out.
    //
    ioResources = NULL;
    reqList = NULL;
    //
    // Pre-initialize returned data.
    //
    ConflictList->ConflictsCounted = 0;
    ConflictList->ConflictsListed = 0;
    ConflictList->RequiredBufferSize = MIN_CONFLICT_LIST_SIZE;
    //
    // Retrieve the devnode from the PDO
    //
    deviceNode = PP_DO_TO_DN(PhysicalDeviceObject);
    if (!deviceNode) {

        status = STATUS_NO_SUCH_DEVICE;
        goto Clean0;
    }
    //
    // Validate resource type.
    //
    switch(ResourceList->List[0].PartialResourceList.PartialDescriptors[0].Type) {

        case CmResourceTypePort:
        case CmResourceTypeMemory:
        case CmResourceTypeMemoryLarge:
            if(ResourceList->List[0].PartialResourceList.PartialDescriptors[0].u.Generic.Length == 0) {
                //
                // Zero length resource can never conflict.
                //
                status = STATUS_SUCCESS;
                goto Clean0;
            }
            break;

        case CmResourceTypeInterrupt:
        case CmResourceTypeDma:
            break;

        default:
            PNP_ASSERT(0);
            status = STATUS_INVALID_PARAMETER;
            goto Clean0;
    }
    //
    // Get the interface type from the devnode.
    //
    pIoReqList = deviceNode->ResourceRequirements;
    if (deviceNode->ChildInterfaceType != InterfaceTypeUndefined) {

        ResourceList->List[0].InterfaceType = deviceNode->ChildInterfaceType;
    } else if (pIoReqList && pIoReqList->InterfaceType != InterfaceTypeUndefined) {

        ResourceList->List[0].InterfaceType = pIoReqList->InterfaceType;
    } else {

        ResourceList->List[0].InterfaceType = PnpDefaultInterfaceType;
    }
    //
    // Map certain interface types to default one.
    //
    if (ResourceList->List[0].InterfaceType == PCMCIABus) {

        ResourceList->List[0].InterfaceType = PnpDefaultInterfaceType;
    }
    //
    // Get the bus number from the devnode.
    //
    if (deviceNode->ChildBusNumber != (ULONG)-1) {

        ResourceList->List[0].BusNumber = deviceNode->ChildBusNumber;
    } else if (pIoReqList && pIoReqList->BusNumber != (ULONG)-1) {

        ResourceList->List[0].BusNumber = pIoReqList->BusNumber;
    } else {

        ResourceList->List[0].BusNumber = 0;
    }
    //
    // Convert CM Resource List to an IO Resource Requirements List.
    //
    ioResources = PnpCmResourcesToIoResources(0,
                                              ResourceList,
                                              LCPRI_FORCECONFIG);
    if (!ioResources) {

        status = STATUS_INVALID_PARAMETER;
        goto Clean0;
    }
    //
    // Convert IO Resource Requirements List to Req List so we can call the arbiters.
    //
    PnpInitializeResourceRequest(&request,
                                 PhysicalDeviceObject,
                                 ioResources,
                                 ArbiterRequestUndefined);

    status = IopResourceRequirementsListToReqList(&request, &reqList);
    if (!NT_SUCCESS(status)) {

        goto Clean0;
    }
    if (reqList == NULL) {

        status = STATUS_INVALID_PARAMETER;
        goto Clean0;
    }
    reqAlternative = reqList->AlternativeTable;
    RA = *reqAlternative;
    reqList->SelectedAlternative = reqAlternative;

    ReqDescCount = RA->DescCount;
    ReqDescTable = RA->DescTable;
    //
    // We should only get one descriptor.
    //
    if (ReqDescCount != 1) {

        status = STATUS_INVALID_PARAMETER;
        goto Clean0;
    }
    reqDesc = *ReqDescTable;
    if (!reqDesc->ArbitrationRequired) {

        status = STATUS_INVALID_PARAMETER;
        goto Clean0;
    }
    reqDescTranslated = reqDesc->TranslatedReqDesc;
    arbiterEntry = reqDesc->u.Arbiter;
    PNP_ASSERT(arbiterEntry);
    //
    // The descriptor of interest - translated, first non-special alternative
    // in the table.
    //
    ConflictDesc = reqDescTranslated->AlternativeTable.Alternatives;
    if( ConflictDesc->Type == CmResourceTypeConfigData ||
        ConflictDesc->Type == CmResourceTypeReserved) {

        ConflictDesc++;
    }
    //
    // Call the arbiter to get the actual conflict information.
    //
    ConflictCount = 0;
    ConflictInfoList = NULL;

    ExtParams[0] = PhysicalDeviceObject;
    ExtParams[1] = ConflictDesc;
    ExtParams[2] = &ConflictCount;
    ExtParams[3] = &ConflictInfoList;
    status = IopCallArbiter(
                arbiterEntry,
                ArbiterActionQueryConflict,
                ExtParams,
                NULL,
                NULL);
    if (NT_SUCCESS(status)) {
        //
        // Get the conflict information.
        //
        status = IopQueryConflictFillConflicts(
                    PhysicalDeviceObject,
                    ConflictCount,
                    ConflictInfoList,
                    ConflictList,
                    ConflictListSize,
                    0);
        if(ConflictInfoList != NULL) {

            ExFreePool(ConflictInfoList);
        }

    } else if(status == STATUS_RANGE_NOT_FOUND) {
        //
        // fill in with flag indicating bad range (this means range is not available)
        // ConflictInfoList should not be allocated
        //
        status = IopQueryConflictFillConflicts(
                    NULL,
                    0,
                    NULL,
                    ConflictList,
                    ConflictListSize,
                    PNP_CE_TRANSLATE_FAILED);
    }

Clean0:
    //
    // Clean up.
    //
    IopCheckDataStructures(IopRootDeviceNode);

    if (ioResources) {

        ExFreePool(ioResources);
    }
    if (reqList) {

        IopFreeReqList(reqList);
    }

    return status;
}

