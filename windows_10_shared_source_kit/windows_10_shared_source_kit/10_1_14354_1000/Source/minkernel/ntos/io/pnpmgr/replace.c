/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    replace.c

Abstract:

    This module implements the partition unit replace.

Author:

    Santosh Jodh (santoshj) 14-03-2006

Environment:

    KernelMode only.

Revision History:

--*/

#include "pnpmgrp.h"
#include "halpnpp.h"
#pragma hdrstop

#define MAX_MEMORY_BITMAP_SIZE      0x80000000

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PnpReplacePartitionUnit)
#pragma alloc_text(PAGE, PnprAddDeviceResources)
#pragma alloc_text(PAGE, PnprAddMemoryResources)
#pragma alloc_text(PAGE, PnprAddProcessorResources)
#pragma alloc_text(PAGE, PnprCollectResources)
#pragma alloc_text(PAGE, PnprGetPluginDriverImagePath)
#pragma alloc_text(PAGE, PnprIdentifyUnits)
#pragma alloc_text(PAGE, PnprIsMemoryDevice)
#pragma alloc_text(PAGE, PnprIsProcessorDevice)
#pragma alloc_text(PAGE, PnprLegacyDeviceDriversPresent)
#pragma alloc_text(PAGE, PnprLoadPluginDriver)
#pragma alloc_text(PAGE, PnprLockPagesForReplace)
#pragma alloc_text(PAGE, PnprLogFailureEvent)
#pragma alloc_text(PAGE, PnprLogStartEvent)
#pragma alloc_text(PAGE, PnprLogSuccessEvent)
#pragma alloc_text(PAGE, PnprAllocateMappingReserves)
#pragma alloc_text(PAGE, PnprInitializeMappingReserve)
#pragma alloc_text(PAGE, PnprFreeMappingReserve)
#pragma alloc_text(PAGE, PnprMmAddRange)
#pragma alloc_text(PAGE, PnprMmConstruct)
#pragma alloc_text(PAGE, PnprMmFree)
#pragma alloc_text(PAGE, PnprReplaceStart)
#pragma alloc_text(PAGE, PnprStartMirroring)
#pragma alloc_text(PAGE, PnprUnloadPluginDriver)
#pragma alloc_text(PAGE, PnprUnlockPagesForReplace)
#pragma alloc_text(PAGELK, PnprGetMillisecondCounter)
#pragma alloc_text(PAGELK, PnprEndMirroring)
#pragma alloc_text(PAGELK, PnprInitiateReplaceOperation)
#pragma alloc_text(PAGELK, PnprMapPhysicalPages)
#pragma alloc_text(PAGELK, PnprMapTargetSparePhysicalPages)
#pragma alloc_text(PAGELK, PnprMarkOrMirrorPages)
#pragma alloc_text(PAGELK, PnprMirrorMarkedPages)
#pragma alloc_text(PAGELK, PnprQuiesceDevices)
#pragma alloc_text(PAGELK, PnprQuiesceProcessorDpc)
#pragma alloc_text(PAGELK, PnprQuiesceProcessors)
#pragma alloc_text(PAGELK, PnprQuiesceWorker)
#pragma alloc_text(PAGELK, PnprWakeDevices)
#pragma alloc_text(PAGELK, PnprWakeProcessors)
#endif // ALLOC_PRAGMA

KEVENT PnpReplaceEvent;
PPNP_REPLACE_CONTEXT PnprContext;

VOID
PnpReplacePartitionUnit (
    __in PVOID ReplaceArguments
    )

/*++

Routine Description:

    This routine is the worker for replacing the target partition unit with the
    spare partition unit.

Arguments:

    ReplaceContext - Supplies a pointer to the context structure that is used
        to maintain the state associated with the replace.

Return Value:

    None.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(ReplaceArguments);

    NT_ASSERT(!"PnpReplacePartitionUnit should never be called\n");

#else

    PPNP_REPLACE_ARGUMENTS Arguments;
    GROUP_AFFINITY ControlAffinity;
    ULONG Flags;
    ULONG Index;
    BOOLEAN LegacyVeto;
    KAFFINITY_EX NonTargetMask;
    GROUP_AFFINITY OldAffinity;
    PNP_REPLACE_POWER_CONTEXT PowerContext;
    ULONG ProcessorCount;
    PPNP_REPLACE_CONTEXT ReplaceContext;
    NTSTATUS Status;
    PPNP_REPLACE_PROCESSOR_LIST TargetProcessors;

    PAGED_CODE();

    ReplaceContext = NULL;
    Arguments = (PPNP_REPLACE_ARGUMENTS)ReplaceArguments;

    PNP_ASSERT(Arguments != NULL);

    //
    // Query the current processor count for per-processor operations in this
    // process.  By running in a PnP thread, this routine prevents starting of
    // new processors, so this value is static.
    //

    ProcessorCount = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);

    //
    // Serialize with other replace operations.
    //

    ACQUIRE_REPLACE_LOCK();

    if ((Arguments->Flags & PNP_REPLACE_TEST_QUIESCE) == 0) {

        ASSERT_PDO(Arguments->SparePdo);
        ASSERT_PDO(Arguments->TargetPdo);

        PnprLogStartEvent(Arguments->TargetPdo, Arguments->SparePdo);
    }

    ReplaceContext = ExAllocatePoolWithTag(NonPagedPoolNx,
                                           sizeof(PNP_REPLACE_CONTEXT),
                                           PNP_POOLTAG_PARTITION_RESOURCE_LIST);

    if (ReplaceContext == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto ReplacePartitionUnitEnd;
    }

    RtlZeroMemory(ReplaceContext, sizeof(PNP_REPLACE_CONTEXT));
    ReplaceContext->Target.Pdo = Arguments->TargetPdo;
    ReplaceContext->Spare.Pdo = Arguments->SparePdo;
    ReplaceContext->Flags = Arguments->Flags;
    PnprContext = ReplaceContext;
    InitializeListHead(&ReplaceContext->TargetMemoryMapList);

    //
    // Fail if dynamic partitioning is not supported on this SKU.
    //

    if ((KeDynamicPartitioningSupported == FALSE) &&
        ((Arguments->Flags & PNP_REPLACE_TEST_QUIESCE) == 0)) {

        Status = STATUS_NOT_SUPPORTED;
        PNPR_REPLACE_FAILURE(UnsupportedBySku);
        goto ReplacePartitionUnitEnd;
    }

    //
    // Fail if there is a legacy (non WDM) driver loaded on the system.  Such
    // drivers will prevent the system from being properly quiesced.
    //

    LegacyVeto = PnprLegacyDeviceDriversPresent();
    if (LegacyVeto != FALSE) {
        Status = STATUS_BAD_DEVICE_TYPE;
        PNPR_REPLACE_FAILURE(QuiesceFailure);
        goto ReplacePartitionUnitEnd;
    }

    //
    // Reset the counter used to time the replace operation.  This call will
    // return zero by design, and so saving the return value is unnecessary.
    //

    PnprGetMillisecondCounter(TRUE);

    //
    // If the test flag is set, simply quiesce the devices and wake them back
    // up again.  This is used to put the machine through the paces it can
    // expect to see during a full replace.
    //

    if (Arguments->Flags & PNP_REPLACE_TEST_QUIESCE) {
        PnprLockPagesForReplace();
        PnprQuiesceDevices(&PowerContext);
        PnprWakeDevices(&PowerContext);
        PnprUnlockPagesForReplace();
        Status = STATUS_SUCCESS;
        goto ReplacePartitionUnitEnd;
    }

    //
    // Get the identifiers of the target and spare modules, validating that the
    // spare is a suitable replacement.
    //

    Status = PnprIdentifyUnits(ReplaceContext->Target.Pdo,
                               ReplaceContext->Spare.Pdo,
                               &ReplaceContext->Target.UniqueId,
                               &ReplaceContext->Spare.UniqueId);

    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(PartitionUnitDoesNotExist);
        goto ReplacePartitionUnitEnd;
    }

    //
    // Collect the memory and processor resources that will be participating
    // in the replace.
    //

    Status = PnprCollectResources(&ReplaceContext->Target,
                                  &ReplaceContext->Spare);

    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(GeneralFailure);
        goto ReplacePartitionUnitEnd;
    }

    //
    // Attempt to load the plugin driver. The driver must be present for
    // memory replace, but it is not strictly necessary for CPU-only replace.
    //

    Status = PnprLoadPluginDriver(&ReplaceContext->ImageHandle,
                                  &ReplaceContext->DriverInterface);

    //
    // If the driver is loaded, query both the driver and the firmware
    // to see what features they support. If the driver wasn't loaded,
    // the replace can proceed only if memory is not being replaced.
    //

    Flags = 0;
    if (NT_SUCCESS(Status)) {
        Flags = PnprQueryReplaceFeatures(&ReplaceContext->DriverInterface,
                                         &ReplaceContext->Spare);

        ReplaceContext->Flags |= Flags;

    } else if (ReplaceContext->Target.Memory->Count != 0) {
        PNPR_REPLACE_FAILURE(GeneralFailure);
        goto ReplacePartitionUnitEnd;
    }

    //
    // If memory is being replaced, ensure that the driver interface exposes
    // all of the necessary functions. If it does, prepare that portion of
    // the replace.
    //

    if (ReplaceContext->Target.Memory->Count != 0) {
        if (((ReplaceContext->DriverInterface.Flags & PNP_REPLACE_MEMORY_SUPPORTED) == 0) ||
            (ReplaceContext->DriverInterface.MirrorPhysicalMemory == NULL)) {

            Status = STATUS_NOT_SUPPORTED;
            PNPR_REPLACE_FAILURE(ChipsetDriverNotSupported);
            goto ReplacePartitionUnitEnd;
        }

        if (((ReplaceContext->DriverInterface.Flags & PNP_REPLACE_PROCESSOR_SUPPORTED) != 0) &&
            (ReplaceContext->DriverInterface.SetProcessorId == NULL)) {

            Status = STATUS_NOT_SUPPORTED;
            PNPR_REPLACE_FAILURE(ChipsetDriverNotSupported);
            goto ReplacePartitionUnitEnd;
        }

        //
        // Construct a bitmap of the pages contained in the target module.
        //

        Status = PnprMmConstruct(ReplaceContext->Target.Memory,
                                 &ReplaceContext->TargetMemoryMapList);

        if (!NT_SUCCESS(Status)) {
            PNPR_REPLACE_FAILURE(GeneralFailure);
            goto ReplacePartitionUnitEnd;
        }

        //
        // Memory replace is impossible if the spares have no memory units.
        //

        NT_ASSERT(ReplaceContext->Spare.Memory->Count != 0);

        //
        // If hardware mirroring is in use, set the length of the spare memory
        // to zero so that the plugin driver can detect this. Otherwise, make
        // sure the driver support to copy memory without hardware is in place.
        //

        if (Flags & PNPR_HARDWARE_MIRRORING) {
            ReplaceContext->Spare.Memory->Count = 0;

        } else {
            if ((ReplaceContext->DriverInterface.GetMemoryDestination == NULL) ||
                ((ReplaceContext->Flags & PNPR_HARDWARE_QUIESCE) != 0)) {

                Status = STATUS_NOT_SUPPORTED;
                PNPR_REPLACE_FAILURE(ChipsetDriverNotSupported);
                goto ReplacePartitionUnitEnd;
            }
        }

        //
        // Allocate per-processor mapping reserves used to map memory during the
        // replace.
        //

        Status = PnprAllocateMappingReserves(&ReplaceContext->TargetReserves,
                                             &ReplaceContext->SpareReserves,
                                             ProcessorCount);

        if (!NT_SUCCESS(Status)) {
            PNPR_REPLACE_FAILURE(AllocationFailure);
            goto ReplacePartitionUnitEnd;
        }

        //
        // Initialize a spinlock protecting the bitmap of pages to copy.
        //

        KeInitializeSpinLock(&ReplaceContext->MemoryMapListLock);
    }

    //
    // If hardware quiesce is to be used, the driver must include a method to
    // support it.
    //

    if (((ReplaceContext->Flags & PNPR_HARDWARE_QUIESCE) != 0) &&
        ((ReplaceContext->DriverInterface.EnableDisableHardwareQuiesce == NULL))) {

        Status = STATUS_NOT_SUPPORTED;
        PNPR_REPLACE_FAILURE(ChipsetDriverNotSupported);
        goto ReplacePartitionUnitEnd;
    }

    //
    // Select a control processor for the replace operation and affinitize the
    // thread to it.  If processor 0 is not a target for replacement then it
    // will be elected the control processor.  Otherwise pick another processor.
    //

    ReplaceContext->ProcessorCount = ProcessorCount;
    KeCopyAffinityEx(&NonTargetMask, (PKAFFINITY_EX)&KeActiveProcessors);
    TargetProcessors = ReplaceContext->Target.Processors;

    //
    // This constraint is already annotated on the
    // PNP_REPLACE_PROCESSOR_LIST struct's GroupCount field
    // This __analysis_assume is needed due to Esp:896.
    //
    __analysis_assume(TargetProcessors->GroupCount <= MAXIMUM_GROUPS);

    for (Index = 0; Index < TargetProcessors->GroupCount; Index += 1) {
        NonTargetMask.Bitmap[Index] &= ~TargetProcessors->Affinity[Index];
    }

    if (KeIsEmptyAffinityEx(&NonTargetMask) != FALSE) {
        Status = STATUS_BAD_DEVICE_TYPE;
        PNPR_REPLACE_FAILURE(PartitionUnitResourceNotSupported);
        goto ReplacePartitionUnitEnd;
    }

    if (KeCheckProcessorAffinityEx(&NonTargetMask, 0) != FALSE) {
        ReplaceContext->ControlProcessor = 0;

    } else {
        ReplaceContext->ControlProcessor =
            KeFindFirstSetLeftAffinityEx(&NonTargetMask);
    }

    //
    // Initiate the replace operation.
    //

    KeProcessorGroupAffinity(&ControlAffinity,
                             ReplaceContext->ControlProcessor);

    KeSetSystemGroupAffinityThread(&ControlAffinity, &OldAffinity);
    Status = PnprInitiateReplaceOperation();
    KeRevertToUserGroupAffinityThread(&OldAffinity);

ReplacePartitionUnitEnd:

    if ((Arguments->Flags & PNP_REPLACE_TEST_QUIESCE) == 0) {
        if (NT_SUCCESS(Status)) {
            ReplaceContext->Timing.ReplaceEnd = PnprGetMillisecondCounter(FALSE);
            PnprLogSuccessEvent();

        } else {
            PnprLogFailureEvent(Arguments->TargetPdo, Arguments->SparePdo, Status);
        }
    }

    if (ReplaceContext != NULL) {
        if (ReplaceContext->ImageHandle != NULL) {
            PnprUnloadPluginDriver(ReplaceContext->ImageHandle,
                                   &ReplaceContext->DriverInterface);
        }

        if ((ReplaceContext->TargetReserves != NULL) &&
            (ReplaceContext->SpareReserves != NULL)) {

            for (Index = 0; Index < ProcessorCount; Index += 1) {
                PnprFreeMappingReserve(&ReplaceContext->TargetReserves[Index]);
                PnprFreeMappingReserve(&ReplaceContext->SpareReserves[Index]);
            }

            ExFreePoolWithTag(ReplaceContext->TargetReserves,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);

            ExFreePoolWithTag(ReplaceContext->SpareReserves,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);
        }


        PnprMmFree(&ReplaceContext->TargetMemoryMapList);
        if (ReplaceContext->Target.Processors != NULL) {
            ExFreePoolWithTag(ReplaceContext->Target.Processors,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);

            ReplaceContext->Target.Processors = NULL;
        }

        if (ReplaceContext->Target.Memory != NULL) {
            ExFreePoolWithTag(ReplaceContext->Target.Memory,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);

            ReplaceContext->Target.Memory = NULL;
        }

        if (ReplaceContext->Spare.Processors != NULL) {
            ExFreePoolWithTag(ReplaceContext->Spare.Processors,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);

            ReplaceContext->Spare.Processors = NULL;
        }

        if (ReplaceContext->Spare.Memory != NULL) {
            ExFreePoolWithTag(ReplaceContext->Spare.Memory,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);

            ReplaceContext->Spare.Memory = NULL;
        }

        ExFreePoolWithTag(ReplaceContext, PNP_POOLTAG_PARTITION_RESOURCE_LIST);
    }

    RELEASE_REPLACE_LOCK();

    Arguments->Status = Status;
    KeSetEvent(&Arguments->CompletionEvent, IO_NO_INCREMENT, FALSE);

#endif

    return;
}

NTSTATUS
PnprAddDeviceResources (
    __in PDEVICE_NODE DeviceNode,
    __in PPNPR_RESOURCE_CONTEXT ResourceContext
    )

/*++

Routine Description:

    This routine collects information about the memory or processor resources
    of a device within a replaceable partition.

Arguments:

    DeviceNode - Supplies a pointer to a devicce in the target partition.

    ResourceContext - Supplies a pointer to the context used to collecte
        partition resources.

Return Value:

    STATUS_SUCCESS if the device resources were added successfully.

    Otherwise, an error status code.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(DeviceNode);
    UNREFERENCED_PARAMETER(ResourceContext);

    NT_ASSERT(!"PnprAddDeviceResources should never be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    ULONG ApicId;
    PDEVICE_OBJECT DeviceObject;
    BOOLEAN Match;
    PPNP_REPLACEABLE_UNIT PartitionUnit;
    ULONG ProcessorNumber;

    PAGED_CODE();

    PartitionUnit = ResourceContext->Unit;
    DeviceObject = DeviceNode->PhysicalDeviceObject;

    //
    // If the device is a memory device, add its memory ranges.  If it is a
    // processor, add its APIC ID.
    //

    Match = PnprIsMemoryDevice(DeviceObject, ResourceContext->IsTarget);
    if (Match != FALSE) {
        return PnprAddMemoryResources(DeviceObject, &PartitionUnit->Memory);
    }

    Match = PnprIsProcessorDevice(DeviceObject,
                                  ResourceContext->IsTarget,
                                  &ProcessorNumber,
                                  &ApicId);

    if (Match != FALSE) {
        return PnprAddProcessorResources(DeviceObject,
                                         ProcessorNumber,
                                         ApicId,
                                         &PartitionUnit->Processors);
    }

    return STATUS_SUCCESS;

#endif

}

NTSTATUS
PnprAddMemoryResources (
    __in PDEVICE_OBJECT DeviceObject,
    __inout PPNP_REPLACE_MEMORY_LIST *MemoryListOut
    )

/*++

Routine Description:

    This routine adds the memory resources exposed by a given memory device to
    the partition memory resource list.

Arguments:

    DeviceObject - Supplies the device object for which the resources are to be
        added.

    MemoryListOut - Supplies a pointer to the memory list that receives the
        memory device resources.  This list must be allocated from non-paged
        pool and may be freed and re-allocated by this routine if it is not
        large enough to hold the device memory resources.

Return Value:

    STATUS_SUCCESS if the memory resources are successfully added.

    STATUS_INSUFFICIENT_RESOURCES if there is a memory allocation error.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(MemoryListOut);

    NT_ASSERT(!"PnprAddMemoryResources should never be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptor;
    PCM_FULL_RESOURCE_DESCRIPTOR FullDescriptor;
    ULONG FullIndex;
    ULONG Index;
    ULONGLONG Length;
    PPNP_REPLACE_MEMORY_LIST MemoryList;
    ULONG NewCount;
    PPNP_REPLACE_MEMORY_LIST NewList;
    ULONG NewSize;
    PPNP_REPLACE_MEMORY_LIST OriginalMemoryList;
    PCM_RESOURCE_LIST ResourceList;
    ULONG Size;
    ULONGLONG Start;
    NTSTATUS Status;

    PAGED_CODE();

    MemoryList = *MemoryListOut;
    OriginalMemoryList = *MemoryListOut;

    PNP_ASSERT(MemoryList != NULL);

    //
    // Get the device resource list.  If the device does not have any resources,
    // exit immediately.
    //

    ResourceList = PP_DO_TO_DN(DeviceObject)->ResourceList;
    if (ResourceList == NULL) {
        Status = STATUS_SUCCESS;
        PNPR_REPLACE_FAILURE(PartitionUnitResourceNotSupported);
        goto AddMemoryResourcesEnd;
    }

    //
    // Walk the resource list looking for memory resources.
    //

    FullDescriptor = &ResourceList->List[0];
    for (FullIndex = 0; FullIndex < ResourceList->Count; FullIndex += 1) {
        for (Index = 0;
             Index < FullDescriptor->PartialResourceList.Count;
             Index += 1) {

            Descriptor =
                &FullDescriptor->PartialResourceList.PartialDescriptors[Index];

            if ((Descriptor->Type != CmResourceTypeMemory) &&
                (Descriptor->Type != CmResourceTypeMemoryLarge)) {

                continue;
            }

            //
            // A memory resource exists.  If the list is not large enough,
            // expand it and copy the existing contents.
            //

            if (MemoryList->Count == MemoryList->AllocatedCount) {
                NewCount = MemoryList->Count + MINIMUM_RESOURCE_EXTENSION;
                Size = MEMORY_LIST_SIZE(MemoryList->Count);
                NewSize = MEMORY_LIST_SIZE(NewCount);

                NewList = ExAllocatePoolWithTag(
                              NonPagedPoolNx,
                              NewSize,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);

                if (NewList == NULL) {
                    Status = STATUS_INSUFFICIENT_RESOURCES;
                    PNPR_REPLACE_FAILURE(AllocationFailure);
                    goto AddMemoryResourcesEnd;
                }

                RtlCopyMemory(NewList, MemoryList, Size);
                NewList->AllocatedCount = NewCount;

                //
                // Free the old list, unless it is the original input list, in
                // which case keep it around so that in case of failure the
                // input state can be restored to the caller.
                //

                if (MemoryList != OriginalMemoryList) {
                    ExFreePoolWithTag(MemoryList,
                                      PNP_POOLTAG_PARTITION_RESOURCE_LIST);
                }

                MemoryList = NewList;
            }

            //
            // Add the range to the list.
            //

            Length = RtlCmDecodeMemIoResource(Descriptor, &Start);
            MemoryList->Ranges[MemoryList->Count].Address.QuadPart = Start;
            MemoryList->Ranges[MemoryList->Count].Length = Length;
            MemoryList->TotalLength += Length;
            MemoryList->Count += 1;
        }

        Size = FIELD_OFFSET(CM_FULL_RESOURCE_DESCRIPTOR,
                            PartialResourceList.PartialDescriptors);

        Size += (Index + sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR));
        FullDescriptor = (PCM_FULL_RESOURCE_DESCRIPTOR)
                         ((PUCHAR)FullDescriptor + Size);
    }

    Status = STATUS_SUCCESS;

AddMemoryResourcesEnd:
    if (NT_SUCCESS(Status)) {
        *MemoryListOut = MemoryList;
        if (MemoryList != OriginalMemoryList) {
            ExFreePoolWithTag(OriginalMemoryList,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);
        }

    } else {
        *MemoryListOut = OriginalMemoryList;
        if (MemoryList != OriginalMemoryList) {
            ExFreePoolWithTag(MemoryList,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);
        }
    }

    return Status;

#endif

}

NTSTATUS
PnprAddProcessorResources (
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG ProcessorIndex,
    __in ULONG ApicId,
    __inout PPNP_REPLACE_PROCESSOR_LIST *ProcessorListOut
    )

/*++

Routine Description:

    This routine adds the resources of the given processor to a list of
    processors.  For a processor, the device resource is its physical APIC ID.

Arguments:

    DeviceObject - Supplies the device object of the processor device.

    ProcessorIndex - Supplies the NT number of the processor.  For spare
        processors, PNPR_SPARE_PROCESSOR is supplied.

    ApicId - Supplies the physical APIC ID of the processor.

    ProcessorListOut - Supplies a pointer to the processor list that receives
        the processor device description.  This list must be allocated from
        non-paged pool and may be freed and re-allocated by this routine if it
        is not large enough to hold the device resources.

Return Value:

    STATUS_SUCCESS if the processor list is filled in correctly.

    STATUS_INSUFFICIENT_RESOURCES if there is a memory allocation failure.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(ProcessorIndex);
    UNREFERENCED_PARAMETER(ApicId);
    UNREFERENCED_PARAMETER(ProcessorListOut);

    NT_ASSERT(!"PnprAddProcessorResources should never be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    ULONG NewCount;
    PPNP_REPLACE_PROCESSOR_LIST NewList;
    ULONG NewSize;
    PROCESSOR_NUMBER Number = {0};
    PPNP_REPLACE_PROCESSOR_LIST ProcessorList;
    ULONG Size;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(DeviceObject);

    //
    // Each processor requires only one entry in the list.  Expand the list if
    // there is no room for this processor.
    //

    ProcessorList = *ProcessorListOut;
    if (ProcessorList->Count == ProcessorList->AllocatedCount) {
        NewCount = ProcessorList->Count + MINIMUM_RESOURCE_EXTENSION;
        Size = PROCESSOR_LIST_SIZE(ProcessorList->Count);
        NewSize = PROCESSOR_LIST_SIZE(NewCount);

        NewList = ExAllocatePoolWithTag(
                      NonPagedPoolNx,
                      NewSize,
                      PNP_POOLTAG_PARTITION_RESOURCE_LIST);

        if (NewList == NULL) {
            PNPR_REPLACE_FAILURE(AllocationFailure);
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        RtlCopyMemory(NewList, ProcessorList, Size);
        ExFreePoolWithTag(ProcessorList, PNP_POOLTAG_PARTITION_RESOURCE_LIST);
        ProcessorList = NewList;
        ProcessorList->AllocatedCount = NewCount;
    }

    //
    // Add the processor APIC ID to the list.
    //

    if (ProcessorIndex != PNPR_SPARE_PROCESSOR) {
        KeGetProcessorNumberFromIndex(ProcessorIndex, &Number);
        ProcessorList->Affinity[Number.Group] |= AFFINITY_MASK(Number.Number);
    }

    ProcessorList->ApicIds[ProcessorList->Count] = ApicId;
    ProcessorList->Count += 1;
    *ProcessorListOut = ProcessorList;
    return STATUS_SUCCESS;

#endif

}

ULONG
PnprQueryReplaceFeatures (
    __in PPNP_REPLACE_DRIVER_INTERFACE DriverInterface,
    __in PPNP_REPLACEABLE_UNIT Spare
    )

/*++

Routine Description:

    This routine queries the replace driver interface and firmware
    to determine what features the firmware supports during a replace.

Arguments:

    DriverInterface - Supplies the plug-in driver interface.

    Spare - Supplies the spare partitionable unit.

Return Value:

    A set of flags indicating the firmware's available features:

    PNPR_HARDWARE_MIRRORING - Full mirroring is provided by the hardware.

    PNPR_HARDWARE_PAGE_COPY - Page by page hardware support is provided.

    PNPR_HARDWARE_QUIESCE - A hardware quiesce engine is available.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(DriverInterface);
    UNREFERENCED_PARAMETER(Spare);

    NT_ASSERT(!"PnprQueryReplaceFeatures should never be called\n");

    return 0;

#else

    ULONG Flags;
    ULONG Properties;
    PARTITION_UNIT_INTERFACE SpareUnitInterface;
    NTSTATUS Status;

    //
    // Hardware copying can be expressed either through BIOS flags or
    // driver interface flags.  Driver interface flags take precedence.
    //

    Flags = 0;
    if (DriverInterface->Flags & PNP_REPLACE_HARDWARE_MEMORY_MIRRORING) {
        Flags |= PNPR_HARDWARE_MIRRORING;

    } else if (DriverInterface->Flags & PNP_REPLACE_HARDWARE_PAGE_COPY) {
        Flags |= PNPR_HARDWARE_PAGE_COPY;
    }

    if (DriverInterface->Flags & PNP_REPLACE_HARDWARE_QUIESCE) {
        Flags |= PNPR_HARDWARE_QUIESCE;
    }

    if (Flags != 0) {
        return Flags;
    }

    //
    // The driver did not provide any information about its capabilities,
    // so ask the firmware for information instead.
    //

    Status = PnpQueryInterface(Spare->Pdo,
                               &GUID_PARTITION_UNIT_INTERFACE_STANDARD,
                               PARTITION_UNIT_INTERFACE_VERSION,
                               sizeof(SpareUnitInterface),
                               NULL,
                               (PINTERFACE)&SpareUnitInterface);

    if (!NT_SUCCESS(Status)) {
        return FALSE;
    }

    Flags = 0;
    Status = SpareUnitInterface.GetSpareProperties(SpareUnitInterface.Context,
                                                   &Properties);

    if (!NT_SUCCESS(Status)) {
        goto QueryFeaturesExit;
    }

    if (Properties & SPARE_PROPERTY_HARDWARE_MIRRORING) {
        Flags |= PNPR_HARDWARE_MIRRORING;

    } else if (Properties & SPARE_PROPERTY_HARDWARE_PAGE_COPY) {
        Flags |= PNPR_HARDWARE_PAGE_COPY;
    }

    if (Properties & SPARE_PROPERTY_HARDWARE_QUIESCE) {
        Flags |= PNPR_HARDWARE_QUIESCE;
    }

QueryFeaturesExit:
    SpareUnitInterface.InterfaceDereference(SpareUnitInterface.Context);
    return Flags;

#endif

}

NTSTATUS
PnprCollectResources (
    __inout PPNP_REPLACEABLE_UNIT Target,
    __inout PPNP_REPLACEABLE_UNIT Spare
    )

/*++

Routine Description:

    This routine collects the processor and memory resources that will
    participate in the replace operation.

Arguments:

    Target - Supplies a pointer to the target replaceable unit.

    Spare - Supplies a pointer to the spare replaceable unit.

Return Value:

    STATUS_SUCCESS if the resources could be collected successfully.

    STATUS_INSUFFICIENT_RESOURCES if there was a memory allocation failure.

    Another error status code if there was an error collecting resources.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(Spare);

    NT_ASSERT(!"PnprCollectResources should never be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    ENUM_CONTEXT EnumContext;
    PNPR_RESOURCE_CONTEXT ResourceContext;
    ULONG Size;
    NTSTATUS Status;

    PAGED_CODE();

    PNP_ASSERT(Target->Processors == NULL);
    PNP_ASSERT(Target->Memory == NULL);
    PNP_ASSERT(Spare->Processors == NULL);
    PNP_ASSERT(Spare->Memory == NULL);

    //
    // Allocate minimum size lists to hold the processor resources.
    // These will be expanded as needed.
    //

    Size = PROCESSOR_LIST_SIZE(MINIMUM_RESOURCE_EXTENSION);
    Target->Processors = ExAllocatePoolWithTag(
                             NonPagedPoolNx,
                             Size,
                             PNP_POOLTAG_PARTITION_RESOURCE_LIST);

    if (Target->Processors == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        PNPR_REPLACE_FAILURE(AllocationFailure);
        goto CollectResourcesEnd;
    }

    RtlZeroMemory(Target->Processors, Size);
    Target->Processors->AllocatedCount = MINIMUM_RESOURCE_EXTENSION;
    Spare->Processors = ExAllocatePoolWithTag(
                            NonPagedPoolNx,
                            Size,
                            PNP_POOLTAG_PARTITION_RESOURCE_LIST);

    if (Spare->Processors == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        PNPR_REPLACE_FAILURE(AllocationFailure);
        goto CollectResourcesEnd;
    }


    RtlZeroMemory(Spare->Processors, Size);
    Spare->Processors->AllocatedCount = MINIMUM_RESOURCE_EXTENSION;

    //
    // Allocate a buffer to hold the processor affinity.
    //

    Size = sizeof(PKAFFINITY) * MAXIMUM_GROUPS;
    Target->Processors->Affinity = ExAllocatePoolWithTag(
                                       NonPagedPoolNx,
                                       Size,
                                       PNP_POOLTAG_PARTITION_RESOURCE_LIST);

    if (Target->Processors->Affinity == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        PNPR_REPLACE_FAILURE(AllocationFailure);
        goto CollectResourcesEnd;
    }

    RtlZeroMemory(Target->Processors->Affinity, Size);
    Target->Processors->GroupCount = MAXIMUM_GROUPS;

    //
    // Do the same for the memory lists.
    //

    Size = MEMORY_LIST_SIZE(MINIMUM_RESOURCE_EXTENSION);
    Target->Memory = ExAllocatePoolWithTag(
                         NonPagedPoolNx,
                         Size,
                         PNP_POOLTAG_PARTITION_RESOURCE_LIST);

    if (Target->Memory == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        PNPR_REPLACE_FAILURE(AllocationFailure);
        goto CollectResourcesEnd;
    }

    Spare->Memory = ExAllocatePoolWithTag(
                        NonPagedPoolNx,
                        Size,
                        PNP_POOLTAG_PARTITION_RESOURCE_LIST);

    if (Spare->Memory == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        PNPR_REPLACE_FAILURE(AllocationFailure);
        goto CollectResourcesEnd;
    }

    RtlZeroMemory(Target->Memory, Size);
    RtlZeroMemory(Spare->Memory, Size);
    Target->Memory->AllocatedCount = MINIMUM_RESOURCE_EXTENSION;
    Spare->Memory->AllocatedCount = MINIMUM_RESOURCE_EXTENSION;

    //
    // Collect the target resources for each device in the tree rooted at the
    // given module device, and then do the same for the spare.
    //

    ResourceContext.Unit = Target;
    ResourceContext.IsTarget = TRUE;
    EnumContext.CallersCallback = PnprAddDeviceResources;
    EnumContext.CallersContext = &ResourceContext;
    Status = PipForAllChildDeviceNodes(PP_DO_TO_DN(Target->Pdo),
                                       PipForAllDeviceNodesCallback,
                                       (PVOID)&EnumContext);

    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(GeneralFailure);
        goto CollectResourcesEnd;
    }

    ResourceContext.Unit = Spare;
    ResourceContext.IsTarget = FALSE;
    Status = PipForAllChildDeviceNodes(PP_DO_TO_DN(Spare->Pdo),
                                       PipForAllDeviceNodesCallback,
                                       (PVOID)&EnumContext);

    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(GeneralFailure);
        goto CollectResourcesEnd;
    }

    //
    // Fail if the collection process did not find any processors or memory to
    // participate in the replace, or if the spare resources are not at least
    // as large a set as the target.
    //

    if ((Target->Processors->Count == 0) && (Target->Memory->Count == 0)) {
        Status = STATUS_INVALID_PARAMETER;
        PNPR_REPLACE_FAILURE(PartitionUnitNotReady);
        goto CollectResourcesEnd;
    }

    if (((Spare->Memory->TotalLength != 0) &&
         (Target->Memory->TotalLength > Spare->Memory->TotalLength)) ||
        (Target->Processors->Count > Spare->Processors->Count)) {

        Status = STATUS_INVALID_PARAMETER;
        PNPR_REPLACE_FAILURE(PartitionUnitsNotCompatible);
        goto CollectResourcesEnd;
    }

CollectResourcesEnd:
    if (!NT_SUCCESS(Status)) {
        if (Target->Processors != NULL) {
            if (Target->Processors->Affinity != NULL) {
                ExFreePoolWithTag(Target->Processors->Affinity,
                                  PNP_POOLTAG_PARTITION_RESOURCE_LIST);
            }

            ExFreePoolWithTag(Target->Processors,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);

            Target->Processors = NULL;
        }

        if (Target->Memory != NULL) {
            ExFreePoolWithTag(Target->Memory,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);

            Target->Memory = NULL;
        }

        if (Spare->Processors != NULL) {
            ExFreePoolWithTag(Spare->Processors,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);

            Spare->Processors = NULL;
        }

        if (Spare->Memory != NULL) {
            ExFreePoolWithTag(Spare->Memory,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);

            Spare->Memory = NULL;
        }
    }

    return Status;

#endif

}

VOID
PnprCompleteWake (
    VOID
    )

/*++

Routine Description:

    This routine completes a quiesce operation by restoring
    processor-specific information and unlocking any pages that
    were locked for the replace operation.

Arguments:

    None.

Return Value:

    None.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprCompleteWake should never be called\n");

#else

    PnprUnlockPagesForReplace();

#endif

    return;
}

NTSTATUS
PnprIdentifyUnits (
    __in PDEVICE_OBJECT TargetPdo,
    __in PDEVICE_OBJECT SparePdo,
    __out PULONG64 TargetUniqueId,
    __out PULONG64 SpareUniqueId
    )

/*++

Routine Description:

    This routine retrieves the unique IDs representing the target and spare
    module devices.  It also checks these devices to verify that the spare is
    a suitable replacement for the target.

Arguments:

    TargetPdo - Supplies a pointer to the device object for the target module.

    SparePdo - Supplies a pointer to the device object for the spare module.

    TargetUniqueId - Supplies a pointer to a variable that receives the ID of
        the target module.

    SpareUniqueId - Supplies a pointer to a variable that receives the ID of
        the spare module.

Return Value:

    STATUS_SUCCESS if the given PDOs could be identified and are suitable
        replaceable modules.

    Otherwise, an error status code.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(TargetPdo);
    UNREFERENCED_PARAMETER(SparePdo);
    UNREFERENCED_PARAMETER(TargetUniqueId);
    UNREFERENCED_PARAMETER(SpareUniqueId);

    NT_ASSERT(!"PnprIdentifyUnits should never be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    PARTITION_UNIT_INTERFACE SpareUnitInterface;
    NTSTATUS Status;
    PARTITION_UNIT_INTERFACE TargetUnitInterface;
    BOOLEAN Valid;

    PAGED_CODE();

    RtlZeroMemory(&SpareUnitInterface, sizeof(PARTITION_UNIT_INTERFACE));
    RtlZeroMemory(&TargetUnitInterface, sizeof(PARTITION_UNIT_INTERFACE));

    //
    // Get the partition unit interface for both the spare and the target.
    //

    Status = PnpQueryInterface(SparePdo,
                               &GUID_PARTITION_UNIT_INTERFACE_STANDARD,
                               PARTITION_UNIT_INTERFACE_VERSION,
                               sizeof(SpareUnitInterface),
                               NULL,
                               (PINTERFACE)&SpareUnitInterface);

    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(PartitionUnitDoesNotExist);
        goto IdentifyUnitsEnd;
    }

    Status = PnpQueryInterface(TargetPdo,
                               &GUID_PARTITION_UNIT_INTERFACE_STANDARD,
                               PARTITION_UNIT_INTERFACE_VERSION,
                               sizeof(TargetUnitInterface),
                               NULL,
                               (PINTERFACE)&TargetUnitInterface);

    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(PartitionUnitDoesNotExist);
        goto IdentifyUnitsEnd;
    }

    if ((TargetUnitInterface.GetUniqueId == NULL) ||
        (SpareUnitInterface.GetUniqueId == NULL) ||
        (SpareUnitInterface.IsValidSpare == NULL)) {

        Status = STATUS_INVALID_PARAMETER;
        goto IdentifyUnitsEnd;
    }

    //
    // Retrieve the unique IDs of the units.
    //

    Status = TargetUnitInterface.GetUniqueId(TargetUnitInterface.Context,
                                             TargetUniqueId);

    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(PartitionUnitDoesNotExist);
        goto IdentifyUnitsEnd;
    }

    Status = SpareUnitInterface.GetUniqueId(SpareUnitInterface.Context,
                                            SpareUniqueId);

    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(PartitionUnitDoesNotExist);
        goto IdentifyUnitsEnd;
    }

    //
    // Query the spare to be sure that it is a valid spare for the target.
    //

    Valid = SpareUnitInterface.IsValidSpare(SpareUnitInterface.Context,
                                            *TargetUniqueId);

    if (Valid == FALSE) {
        Status = STATUS_INVALID_PARAMETER;
        PNPR_REPLACE_FAILURE(PartitionUnitsNotCompatible);
        goto IdentifyUnitsEnd;
    }

    Status = STATUS_SUCCESS;

IdentifyUnitsEnd:
    if (SpareUnitInterface.Context != NULL) {
        SpareUnitInterface.InterfaceDereference(SpareUnitInterface.Context);
    }

    if (TargetUnitInterface.Context != NULL) {
        TargetUnitInterface.InterfaceDereference(TargetUnitInterface.Context);
    }

    return Status;

#endif

}

BOOLEAN
PnprIsMemoryDevice (
    __in PDEVICE_OBJECT DeviceObject,
    __in BOOLEAN IsTarget
    )

/*++

Routine Description:

    This routine determines whether the given device is a memory device.

Arguments:

    DeviceObject - Supplies the device object to check.

    IsTarget - Supplies a boolean value indicating whether the device is in the
        target partition of the replace, or if it is a spare.

Return Value:

    TRUE if the device is a memory device.

    FALSE otherwise.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(IsTarget);

    NT_ASSERT(!"PnprIsMemoryDevice should not be called\n");

    return FALSE;

#else

    BOOLEAN ReturnValue;
    ULONG Size;
    DEVPROP_BOOLEAN Spare;
    NTSTATUS Status;
    PWSTR SymbolicLinkList;
    DEVPROPTYPE Type;

    PAGED_CODE();

    //
    // A memory device that is part of the running system exposes a device
    // interface identifying it.  Spare memory devices expose a device property.
    //

    ReturnValue = FALSE;
    SymbolicLinkList = NULL;
    if (IsTarget != FALSE) {
        Status = IoGetDeviceInterfaces(&GUID_DEVICE_MEMORY,
                                       DeviceObject,
                                       0,
                                       &SymbolicLinkList);

        if (NT_SUCCESS(Status) && (SymbolicLinkList[0] != UNICODE_NULL)) {
            ReturnValue = TRUE;
        }

    } else {
        Status = IoGetDevicePropertyData(DeviceObject,
                                         &DEVPKEY_Spare_Memory,
                                         0,
                                         0,
                                         sizeof(Spare),
                                         &Spare,
                                         &Size,
                                         &Type);

        if (NT_SUCCESS(Status) &&
            (Type == DEVPROP_TYPE_BOOLEAN) &&
            (Spare != DEVPROP_FALSE)) {

            ReturnValue = TRUE;
        }
    }

    if (SymbolicLinkList != NULL) {
        ExFreePool(SymbolicLinkList);
    }

    return ReturnValue;

#endif

}

BOOLEAN
PnprIsProcessorDevice (
    __in PDEVICE_OBJECT DeviceObject,
    __in BOOLEAN IsTarget,
    __out PULONG ProcessorNumber,
    __out PULONG ApicId
    )

/*++

Routine Description:

    This routine determines whether the given device is a processor device.

Arguments:

    DeviceObject - Supplies the device object to check.

    IsTarget - Supplies a boolean value that indicates whether the device is in
        the partition that is the target of the replace.

    ProcessorNumber - Supplies a reference to a variable that receives the NT
        processor number of the processor, if the device is a target processor.
        If the device is a spare processor, it receives PNPR_SPARE_PROCESSOR.


    ApicId - Supplies a pointer to a variable that receives the physical APIC
        ID of the processor, if the device is a processor.

Return Value:

    TRUE if the device is a processor.

    FALSE otherwise.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(IsTarget);
    UNREFERENCED_PARAMETER(ProcessorNumber);
    UNREFERENCED_PARAMETER(ApicId);

    NT_ASSERT(!"PnprIsProcessorDevice should not be called\n");

    return FALSE;

#else

    ULONG NtNumber;
    BOOLEAN ReturnValue;
    ULONG Size;
    NTSTATUS Status;
    PWSTR SymbolicLinkList;
    DEVPROPTYPE Type;

    PAGED_CODE();

    //
    // A processor that is part of the running system exposes a device interface
    // to identify itself.  A spare processor exposes a device property.
    //

    ReturnValue = FALSE;
    if (IsTarget != FALSE) {
        Status = IoGetDeviceInterfaces(&GUID_DEVICE_PROCESSOR,
                                       DeviceObject,
                                       0,
                                       &SymbolicLinkList);

        if (!NT_SUCCESS(Status) || (SymbolicLinkList[0] == UNICODE_NULL)) {
            goto IsProcessorDeviceEnd;
        }

        ExFreePool(SymbolicLinkList);

        //
        // The NT processor number of a running processor is in a device
        // property.  Use this to look up the processor's PRCB, which contains
        // the physical APIC ID.
        //

        Status = IoGetDevicePropertyData(DeviceObject,
                                         &PROCESSOR_NUMBER_PKEY,
                                         0,
                                         0,
                                         sizeof(ULONG),
                                         &NtNumber,
                                         &Size,
                                         &Type);

        if (!NT_SUCCESS(Status) || (Type != DEVPROP_TYPE_UINT32)) {
            goto IsProcessorDeviceEnd;
        }

        Status = HalGetProcessorIdByNtNumber(NtNumber, ApicId);
        if (!NT_SUCCESS(Status)) {
            goto IsProcessorDeviceEnd;
        }

        *ProcessorNumber = NtNumber;
        ReturnValue = TRUE;

    } else {
        Status = IoGetDevicePropertyData(DeviceObject,
                                         &DEVPKEY_Spare_Processor_Apic_Id,
                                         0,
                                         0,
                                         sizeof(ULONG),
                                         ApicId,
                                         &Size,
                                         &Type);

        if (NT_SUCCESS(Status) && (Type == DEVPROP_TYPE_UINT32)) {
            *ProcessorNumber = PNPR_SPARE_PROCESSOR;
            ReturnValue = TRUE;
        }
    }

IsProcessorDeviceEnd:
    return ReturnValue;

#endif

}

NTSTATUS
PnprInitiateReplaceOperation (
    VOID
    )

/*++

Routine Description:

    This routine initiates the replace operation.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprInitiateReplaceOperation should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    NTSTATUS EndStatus;
    KIRQL Irql;
    MM_MIRROR_DISPATCH MirrorDispatch;
    BOOLEAN HardwareMirroringQuiesceComplete;
    BOOLEAN ReplaceStarted;
    NTSTATUS Status;
    PWORK_QUEUE_ITEM WorkItem;

    //
    // Initialize the events that will be used to control the flow of the work
    // item, and queue the work item.
    //

    HardwareMirroringQuiesceComplete = FALSE;
    ReplaceStarted = FALSE;
    WorkItem = NULL;
    KeInitializeEvent(&PnprContext->BeginQuiesce, NotificationEvent, FALSE);
    KeInitializeEvent(&PnprContext->QuiesceComplete, NotificationEvent, FALSE);
    KeInitializeEvent(&PnprContext->BeginWake, NotificationEvent, FALSE);
    KeInitializeEvent(&PnprContext->WakeComplete, NotificationEvent, FALSE);

    //
    // Allocate space for a workitem that will quiesce all devices before the
    // replace and then wake them after the replace is complete.  This pool
    // allocation will be freed by the workitem handler just before it runs to
    // completion.
    //

    if ((PnprContext->Flags & PNPR_HARDWARE_QUIESCE) == 0) {
        WorkItem = ExAllocatePoolWithTag(NonPagedPoolNx,
                                         sizeof(WORK_QUEUE_ITEM),
                                         PNP_POOLTAG_PARTITION_RESOURCE_LIST);

        if (WorkItem == NULL) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            PNPR_REPLACE_FAILURE(AllocationFailure);
            goto InitiateReplaceOperationEnd;
        }

        ExInitializeWorkItem(WorkItem, PnprQuiesceWorker, WorkItem);
        ExQueueWorkItem(WorkItem, CriticalWorkQueue);
    }

    //
    // Report the parameters of the replace to the HAL and plugin driver to
    // allow them to initialize.
    //

    Status = PnprReplaceStart();
    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(GeneralFailure);
        goto InitiateReplaceOperationEnd;
    }

    ReplaceStarted = TRUE;

    //
    // Initiate the replace operation.  The control flow of the replace
    // operation is performed by the memory duplication routine making calls to
    // callback functions for the start of mirroring and the end of two passes
    // (pass 0 and pass 1) of mirroring, which will quiesce the system and swap
    // the partitions.
    //
    // N.B. - Memory mirroring has some delicate restrictions.  It must be
    //        called at passive level.  The memory manager will acquire mutexes
    //        which will disable kernel APCs, limiting the routines that can
    //        be called in the callbacks.  The PFN lock is held for the duration
    //        of phase 1 mirroring, meaning that arbitrary threads that might
    //        allocate memory cannot run after the end of phase 0.  It is legal
    //        for the callback handling the end of phase 0 to raise IRQL to
    //        accomplish this, and the mirroring routine will restore the
    //        original input IRQL before completing the mirror.  Because
    //        arbitrary threads can run during phase 0, there exists a
    //        limitation that the mirroring callback cannot map the pages to be
    //        mirrored during this time.  It must either mirror the requested
    //        pages using a hardware DMA engine or save the page information and
    //        copy it during phase 1.
    //
    // If no memory is being replaced, calling the memory manager to duplicate
    // memory is unnecessary.  In this case quiesce and swap directly.
    //

    if ((PnprContext->Target.Memory->Count != 0) &&
        ((PnprContext->Flags & PNPR_HARDWARE_MIRRORING) == 0)) {

        RtlZeroMemory(&MirrorDispatch, sizeof(MM_MIRROR_DISPATCH));
        MirrorDispatch.StartMirroring = PnprStartMirroring;
        MirrorDispatch.EndMirroring = PnprEndMirroring;
        MirrorDispatch.MirrorPhysicalMemory = PnprMirrorPhysicalMemory;
        MirrorDispatch.MirrorVerify = NULL;
        MirrorDispatch.Flags = MM_PHASE0_SOFTWARE_COPYING;
        Status = MmDuplicateMemory(&MirrorDispatch);

        //
        // Memory duplication may return the sentinel status code returned by
        // this module during the mirror process.  If this occurs, update the
        // status code to something more suitable for the caller.
        //

        if (Status == STATUS_WAKE_SYSTEM) {
            Status = STATUS_SUCCESS;
        }

    } else {

        //
        // If hardware memory mirroring is in use, initiate the mirror now.
        // The target and spare memory are coherent after this call returns.
        //

        if (PnprContext->Target.Memory->Count != 0) {

            NT_ASSERT(PnprContext->DriverInterface.InitiateHardwareMirror != NULL);

            Status = PnprContext->DriverInterface.InitiateHardwareMirror(
                         PnprContext->DriverContext);

            if (!NT_SUCCESS(Status)) {
                PNPR_REPLACE_FAILURE(ChipsetDriverFailure);
                goto InitiateReplaceOperationEnd;
            }
        }

        //
        // The memory is coherent.  Quiesce the system to swap the processors.
        //

        Irql = KeGetCurrentIrql();

        //
        // Quiesce and swap the target processors.
        //

        Status = PnprQuiesce();
        if (NT_SUCCESS(Status)) {
            HardwareMirroringQuiesceComplete = TRUE;
            Status = PnprSwap();
            if (!NT_SUCCESS(Status)) {
                HalDpReplaceControl(HAL_DP_REPLACE_PHASE_CANCEL,
                                    PnprContext->HalContext);

                PNPR_REPLACE_FAILURE(GeneralFailure);
            }

            //
            // Wake up all the other processors.
            //

            PnprWakeProcessors();

        } else {
            PNPR_REPLACE_FAILURE(GeneralFailure);
        }

        //
        // Quiescing of the processors raises the IRQL to dispatch level.
        // Memory mirroring restores the input IRQL, but this must be done
        // directly here.
        //

        KeLowerIrql(Irql);
        if (!NT_SUCCESS(Status)) {
            goto InitiateReplaceOperationEnd;
        }
    }

    //
    // Return the system from its quiesced state. In the hardware quiesce
    // case, just clean up memory needed to hold processor-specific
    // state. In the non-quiesce case, however, set the device quiesce
    // work item to wake the devices. In this latter case, the quiesce
    // will be canceled if it has not occurred by this point.
    //

InitiateReplaceOperationEnd:

    if ((PnprContext->Flags & PNPR_HARDWARE_QUIESCE) != 0) {

        NT_ASSERT(WorkItem == NULL);

        if (HardwareMirroringQuiesceComplete != FALSE) {
            PnprCompleteWake();
        }

    } else {
        if (WorkItem != NULL) {
            KeSetEvent(&PnprContext->BeginWake, IO_NO_INCREMENT, FALSE);
            KeWaitForSingleObject(&PnprContext->WakeComplete,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);
        }
    }

    if (PnprContext->HalContext != NULL) {
        HalDpReplaceEnd(PnprContext->HalContext);
    }

    if (ReplaceStarted != FALSE) {
        if (PnprContext->Target.Memory->Count != 0) {
            EndStatus = PnprContext->DriverInterface.EndReplace(
                            PnprContext->DriverContext);

            if (NT_SUCCESS(Status)) {
                Status = EndStatus;
            }
        }
    }

    return Status;

#endif

}

NTSTATUS
PnprQuiesce (
    VOID
    )

/*++

Routine Description:

    This routine is called on the control processor to quiesce the system.  A
    large part of quiescing is done on a work item since this routine may be
    called inside of the context of a memory mirroring operation, which may
    disable kernel APCs to the current thread, limiting the operations that can
    be done on this thread.

Arguments:

    None.  The global replace context provides the parameters.

Return Value:

    NT status code.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprQuiesce should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    NTSTATUS Status = STATUS_SUCCESS;


    //
    // Signal the quiesce work item to perform the device quiesce and associated
    // other quiesce steps, and wait until it has completed that task. If
    // devices are not being quiesced, manually store some per-processor context
    // that will be needed later when the spares are brought online.
    //

    if((PnprContext->Flags & PNPR_HARDWARE_QUIESCE) == 0) {
        KeSetEvent(&PnprContext->BeginQuiesce, IO_NO_INCREMENT, TRUE);
        KeWaitForSingleObject(&PnprContext->QuiesceComplete,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);

        Status = PnprContext->QuiesceStatus;
        if (!NT_SUCCESS(Status)) {
            PNPR_REPLACE_FAILURE(GeneralFailure);
            return Status;
        }

    } else {
        PnprLockPagesForReplace();
    }

    //
    // Quiesce the processors directly since this involves raising IRQL on this
    // processor and returning at raised IRQL.  This cannot be done inside a
    // work item.
    //

    PnprQuiesceProcessors();

    return Status;

#endif

}

NTSTATUS
PnprQuiesceDevices (
    __out PPNP_REPLACE_POWER_CONTEXT Context
    )

/*++

Routine Description:

    This routine will attempt to place all devices in the system in D3.  This is
    accomplished by calling into the power manager to broadcast an S4 IRP.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(Context);

    NT_ASSERT(!"PnprQuiesceDevices should never be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    NTSTATUS Status;

    PnprContext->Timing.DeviceQuiesceStart = PnprGetMillisecondCounter(FALSE);
    RtlZeroMemory(Context, sizeof(PNP_REPLACE_POWER_CONTEXT));
    Context->Flags = POWER_ACTION_CRITICAL | POWER_ACTION_PSEUDO_TRANSITION;
    Context->Win32Parameters.SystemAction = PowerActionSleep;
    Context->Win32Parameters.MinSystemState = PowerSystemHibernate;
    Context->Win32Parameters.Flags = Context->Flags;

    //
    // Block console session switches.
    //

    Context->ConsoleId = PoBlockConsoleSwitch(&Context->Win32Parameters);
    Status = PoStartPowerStateTasks(&Context->Win32Parameters);
    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(QuiesceFailure);
        goto SleepDevicesEnd;
    }

    //
    // Notify the power manager that a replace is starting. GDI will be turned
    // off as a result of this.
    //

    PoStartPartitionReplace(&Context->Win32Parameters, Context->ConsoleId);

    //
    // Initialize the power broadcast for the replace.
    //

    Context->Broadcast.CurrentState = PowerSystemWorking;
    Context->Broadcast.EffectiveState = PowerSystemHibernate;
    Context->Broadcast.TargetState = PowerSystemHibernate;
    Context->Broadcast.NextState = PowerSystemWorking;
    Context->Broadcast.PowerAction = PowerActionSleep;
    Context->Broadcast.Flags = Context->Flags;
    Status = PoInitializeBroadcast();
    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(QuiesceFailure);
        goto SleepDevicesEnd;
    }

    //
    // Query all devices for an S4 transition.  The result can be ignored.
    //

    Context->Broadcast.IrpMinor = IRP_MN_QUERY_POWER;
    PoBroadcastSystemState(&Context->Broadcast);

    //
    // Send system devices to S4.
    //

    Context->Broadcast.IrpMinor = IRP_MN_SET_POWER;
    Status = PoBroadcastSystemState(&Context->Broadcast);
    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(QuiesceFailure);
        goto SleepDevicesEnd;
    }

SleepDevicesEnd:
    PnprContext->Timing.DeviceQuiesceEnd = PnprGetMillisecondCounter(FALSE);
    return Status;

#endif

}

VOID
PnprQuiesceProcessorDpc (
    __in struct _KDPC  *Dpc,
    __in PVOID  DeferredContext,
    __in PVOID  SystemArgument1,
    __in PVOID  SystemArgument2
    )

/*++

Routine Description:

    This is the per-processor routine for sending processors to the HAL during
    a replace operation.

Arguments:

    Dpc - Supplies a reference to the DPC object.

    DeferredContext - Supplies a reference to a variable that will be
        incremented to indicate to the caller that the DPC routine has begun
        execution.

Return Value:

    None.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(DeferredContext);
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);

    NT_ASSERT(!"PnprQuiesceProcessorDpc should not be called\n");

#else

    ULONG CurrentProcessor;
    ULONG64 ExtendedFeaturesMask;
    XSTATE_SAVE ExtendedState;
    BOOLEAN ExtendedStateSaved;
    PVOID InterruptReplayState;
    PROCESSOR_NUMBER Number = {0};
    ULONG ProcessorCount;
    ULONG ProcessorId;
    BOOLEAN SavedInterruptFlag;
    volatile BOOLEAN *Started;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(Dpc);

    Started = (volatile BOOLEAN *)DeferredContext;
    *Started = TRUE;
    SavedInterruptFlag = TRUE;
    InterruptReplayState = NULL;
    ProcessorCount = PtrToUlong(SystemArgument1);
    CurrentProcessor = PtrToUlong(SystemArgument2);

    //
    // The target processors must be synchronized with the control processor
    // before entering the HAL. If hardware quiesce is in use, the control
    // processor will not increment this barrier until level-triggered
    // interrupts are disabled.
    //

    InterlockedIncrement(&PnprContext->EntryBarrier);
    while (PnprContext->EntryBarrier < (LONG)ProcessorCount) {
        KeYieldProcessor();
    }

    //
    // If a hardware quiesce engine is present, it will block I/O between
    // CPUs and devices. Interrupts must be disabled before the engine is
    // enabled to ensure that no interrupts are handled while it is on.
    //
    // N.B. It is safe to have interrupts enabled on a CPU if that CPU
    //      is guaranteed not to have any interrupts pending. This is
    //      not true of any of the processors that are active at this
    //      point, since they can receive an interrupt between the time
    //      that they disable interrupts and the time the engine is
    //      enabled.
    //

    if ((PnprContext->Flags & PNPR_HARDWARE_QUIESCE) != 0) {
        SavedInterruptFlag = KeDisableInterrupts();

        //
        // Synchronize with the control processor to alert it that interrupts
        // have been disabled. After this point, the control processor will
        // enable the hardware quiesce engine. Interrupts must remain disabled
        // on all the processors currently present (i.e., every processor
        // except the spares) to ensure that no interrupt handlers are
        // entered until the quiesce engine is disabled.
        //

        InterlockedIncrement(&PnprContext->InterruptDisableBarrier);
        while (PnprContext->InterruptDisableBarrier < (LONG)ProcessorCount) {
            KeYieldProcessor();
        }
    }

    //
    // Only enter the HAL for target processors.  When it returns, it is running
    // on the spare.
    //

    KeGetProcessorNumberFromIndex(CurrentProcessor, &Number);
    if (IS_TARGET_PROCESSOR(&Number)) {
        while (PnprContext->SwapPhase < SWAP_PHASE_TARGET_REPLACE) {
            KeYieldProcessor();
        }

        KeSuspendClockTimerSafe();
        ExtendedStateSaved = FALSE;
        ExtendedFeaturesMask = 
            (((SharedUserData->XState.EnabledFeatures) |
              (SharedUserData->XState.EnabledSupervisorFeatures)) &
             ~XSTATE_MASK_LEGACY);

#if defined(_X86_)

        ExtendedFeaturesMask |= XSTATE_MASK_LEGACY;

#endif

        if (ExtendedFeaturesMask != 0) {
            Status = KeSaveExtendedAndSupervisorState(ExtendedFeaturesMask,
                                                      &ExtendedState);

            if (NT_SUCCESS(Status)) {
                ExtendedStateSaved = TRUE;
            }
        }

        HalGetProcessorIdByNtNumber(CurrentProcessor, &ProcessorId);
        if (PnprContext->DriverInterface.Flags & PNP_REPLACE_PROCESSOR_SUPPORTED) {
            PnprContext->DriverInterface.SetProcessorId(
                PnprContext->DriverContext,
                ProcessorId,
                TRUE);
        }

        //
        // If hardware quiesce is available, the spares will need to replay
        // interrupts that the targets weren't able to process before the swap.
        // Query the HAL to get the state the spares will need to do the replay.
        //

        if ((PnprContext->Flags & PNPR_HARDWARE_QUIESCE) != 0) {
            HalDpGetInterruptReplayState(PnprContext->HalContext,
                                         &InterruptReplayState);
        }

        HalDpReplaceTarget(PnprContext->HalContext);
        if (PnprContext->DriverInterface.Flags & PNP_REPLACE_PROCESSOR_SUPPORTED) {
            PnprContext->DriverInterface.SetProcessorId(
                PnprContext->DriverContext,
                ProcessorId,
                FALSE);
        }

        //
        // Resume clock ticks.
        //

        KeResumeClockTimerSafe();

        //
        // At this point, the DPC is executing on the spares instead of the
        // targets. Under hardware quiesce, the spares can now reenable
        // interrupts; because they were just started, they have no external
        // interrupts pending. Non-spares might still have pending interrupts
        // and need to wait for the quiesce engine to be disabled before
        // they are allowed to handle those interrupts.
        //

        if ((PnprContext->Flags & PNPR_HARDWARE_QUIESCE) != 0) {
            KeEnableInterrupts(SavedInterruptFlag);
        }

#if defined(_X86_) || defined(_AMD64_)

        KeRestoreProcessorSpecificFeatures();

#endif

        if (ExtendedStateSaved != FALSE) {
            KeRestoreExtendedAndSupervisorState(&ExtendedState);
        }

        InterlockedIncrement(&PnprContext->SwapBarrier);
    }

    //
    // If memory is to be copied without a hardware engine, assist with the
    // process.  The control processor will release the others to begin the
    // memory copy as soon as the quiesce is complete.
    //

    if ((PnprContext->Target.Memory->Count != 0) &&
        ((PnprContext->Flags & PNPR_HARDWARE_MIRRORING) == 0)) {

        while (PnprContext->SwapPhase < SWAP_PHASE_COPY_MEMORY) {
            KeYieldProcessor();
        }

        Status = PnprMirrorMarkedPages();
        if (!NT_SUCCESS(Status)) {
            PNPR_REPLACE_FAILURE(GeneralFailure);
        }

        InterlockedIncrement(&PnprContext->SwapBarrier);
    }

    //
    // Wait for the control processor to indicate that the swap is about to
    // occur.
    //

    while (PnprContext->SwapPhase < SWAP_PHASE_SWAP_PREPARE) {
        KeYieldProcessor();
    }

    //
    // Save the local stack limits so that the control processor can re-copy
    // this processor's stack before the swap.
    //

    PnprGetStackLimits(&PnprContext->CurrentStackPointer[CurrentProcessor],
                       &PnprContext->StackLimitPointer[CurrentProcessor]);

    NT_ASSERT(PnprContext->StackLimitPointer[CurrentProcessor] >
              PnprContext->CurrentStackPointer[CurrentProcessor]);

    //
    // Indicate that the swap can proceed.
    //

    InterlockedIncrement(&PnprContext->SwapBarrier);

    //
    // All of the processors (excluding the control processor) will wait here
    // until the control processor has cleared the spin barrier, thus waking up
    // all of the processors.  Wait for the swap to occur before altering the
    // exit barrier, because the swap can alter the context memory.
    //

    while (PnprContext->SwapPhase < SWAP_PHASE_SWAPPED) {
        KeYieldProcessor();
    }

    //
    // In the hardware quiesce case, wait for the control processor to indicate
    // that it has disabled the hardware quiesce engine before proceeding.
    //

    if ((PnprContext->Flags & PNPR_HARDWARE_QUIESCE) != 0) {
        while (PnprContext->SwapPhase < SWAP_PHASE_QUIESCE_ENGINE_OFF) {
            KeYieldProcessor();
        }

        //
        // The quiesce engine has been disabled. It is now safe for the
        // uninvolved processors (i.e., non-spares) to reenable interrupts
        // since communication with external devices has been restored.
        // The spares enabled interrupts as soon as they were brought
        // online. It is now safe for them to replay interrupts that were
        // pending on the targets when they went offline.
        //

        if (IS_TARGET_PROCESSOR(&Number) == FALSE) {
            KeEnableInterrupts(SavedInterruptFlag);

        } else {
            HalDpReplayInterrupts(InterruptReplayState);
        }
    }

    //
    // The swap has occurred. Indicate that the processor has been
    // re-initialized and wait for all processors to complete.
    //

    InterlockedIncrement(&PnprContext->ExitBarrier);
    while (PnprContext->ExitBarrier < (LONG)ProcessorCount) {
        KeYieldProcessor();
    }

#endif

    return;
}

VOID
PnprQuiesceProcessors (
    VOID
    )

/*++

Routine Description:

    This routine will send all processors other than the control processor to
    the HAL in preparation for the replace.  The function will return at
    dispatch level.

Arguments:

    None.

Return Value:

    None.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprQuiesceProcessors should not be called\n");

#else

    KAFFINITY_EX ActiveProcessors;
    volatile BOOLEAN DpcStarted;
    KAFFINITY_ENUMERATION_CONTEXT EnumerationContext;
    ULONG Index;
    ULONG NtNumber;
    ULONG ProcessorCount;
    KDPC QuiesceDpc;
    NTSTATUS Status;
    PPNP_REPLACE_PROCESSOR_LIST TargetProcessors;

    PNP_ASSERT(KeGetCurrentProcessorIndex() == PnprContext->ControlProcessor);

    //
    // Obtain the processor count.  This will be passed to the DPC handler for
    // use in synchronization.
    //

    ProcessorCount = PnprContext->ProcessorCount;

    //
    // Raise to dispatch level to prevent other threads from being scheduled on
    // the control processor.
    //

    KfRaiseIrql(DISPATCH_LEVEL);

    //
    // Initialize the DPC object and the event to be used to signal the control
    // processor that the DPC routine is running. That event is the context
    // passed to the DPC routine.
    //

    KeInitializeDpc(&QuiesceDpc, PnprQuiesceProcessorDpc, (PVOID)&DpcStarted);
    KeSetImportanceDpc(&QuiesceDpc, HighImportance);

    //
    // Cycle through all other processors and queue the DPC.  Each time the DPC
    // is queued, wait until the DPC routine has started running.  In this way,
    // the same DPC object can be used for all of the processors.
    //

    ActiveProcessors = KeActiveProcessors;
    KeRemoveProcessorAffinityEx(&ActiveProcessors,
                                PnprContext->ControlProcessor);

    FOR_EACH_PROCESSOR(&EnumerationContext,
                       &ActiveProcessors,
                       &NtNumber) {

        KeSetTargetProcessorIndexDpc(&QuiesceDpc, NtNumber);
        DpcStarted = FALSE;
        KeInsertQueueDpc(&QuiesceDpc,
                         UlongToPtr(ProcessorCount),
                         UlongToPtr(NtNumber));

        while (DpcStarted == FALSE) {
            KeYieldProcessor();
        }
    }

    //
    // Replay of level-triggered interrupts in the hardware quiesce
    // case is not supported. These interrupts must be masked before
    // the targets disable interrupts. Otherwise, a target processor
    // might accept and hold pending a level-triggered interrupt
    // that cannot be replayed on the spare.
    //
    // All of the DPCs have started, so disable level-triggered interrupts.
    //

    if ((PnprContext->Flags & PNPR_HARDWARE_QUIESCE) != 0) {
        Status = HalDpMaskLevelTriggeredInterrupts();
        if (!NT_SUCCESS(Status)) {
            PNPR_REPLACE_FAILURE(GeneralFailure);
            KeActiveProcessors = ActiveProcessors;
            return;
        }
    }

    //
    // The control processor now calls into the HAL to perform system-wide
    // preparation for the actual replace operation. This can be done
    // at this point because the other processors are in their DPCs. They
    // will wait to cross the next barrier before disabling interrupts.
    //

    HalDpReplaceControl(HAL_DP_REPLACE_PHASE_QUIESCE, PnprContext->HalContext);

    //
    // Suspend clock interrupt generation.
    //

    KeSuspendClockTimerSafe();

    PnprContext->Timing.QuiesceTime = PnprGetMillisecondCounter(FALSE);

    //
    // Ensure that all of the target processors have started their DPC
    // and read their parameters before allowing them to continue.
    //

    InterlockedIncrement(&PnprContext->EntryBarrier);
    while (PnprContext->EntryBarrier < (LONG)ProcessorCount) {
        KeYieldProcessor();
    }

    //
    // It's not safe to enable the hardware quiesce engine (if it's in use)
    // unless interrupts are disabled on all of the targets. Ensure that
    // this is the case before enabling the engine. Note that if hardware
    // quiesce is not available, the targets won't block on this barrier.
    //

    if ((PnprContext->Flags & PNPR_HARDWARE_QUIESCE) != 0) {
        InterlockedIncrement(&PnprContext->InterruptDisableBarrier);
        while (PnprContext->InterruptDisableBarrier < (LONG)ProcessorCount) {
            KeYieldProcessor();
        }

        PnprContext->DriverInterface.EnableDisableHardwareQuiesce(
                                        PnprContext->DriverContext,
                                        TRUE);

        PnprContext->Timing.QuiesceEnabled = PnprGetMillisecondCounter(FALSE);
    }

    //
    // Preparation is complete.  Allow the target processors to go offline.
    //

    ActiveProcessors = KeActiveProcessors;
    TargetProcessors = PnprContext->Target.Processors;

    //
    // This constraint is already annotated on the
    // PNP_REPLACE_PROCESSOR_LIST struct's GroupCount field
    // This __analysis_assume is needed due to Esp:896.
    //

    __analysis_assume(TargetProcessors->GroupCount <= MAXIMUM_GROUPS);
    for (Index = 0; Index < TargetProcessors->GroupCount; Index += 1) {
        KeActiveProcessors.Bitmap[Index] &= ~TargetProcessors->Affinity[Index];
    }

    PnprContext->SwapBarrier = 0;
    PnprContext->SwapPhase = SWAP_PHASE_TARGET_REPLACE;

    //
    // Allow the target processors to go offline and start up the spares.
    //

    if (PnprContext->Target.Processors->Count != 0) {
        PnprContext->Timing.TargetsOffline = PnprGetMillisecondCounter(FALSE);
        Status = HalDpReplaceControl(HAL_DP_REPLACE_PHASE_SWAP,
                                     PnprContext->HalContext);
        PnprContext->Timing.SparesStarted = PnprGetMillisecondCounter(FALSE);

        if (!NT_SUCCESS(Status)) {
            PNPR_REPLACE_FAILURE(GeneralFailure);
            KeActiveProcessors = ActiveProcessors;
            return;
        }
    }

    KeActiveProcessors = ActiveProcessors;

    //
    // Wait for all of the spares to be reinitialized and spinning.
    //

    while (PnprContext->SwapBarrier <
           (LONG)PnprContext->Target.Processors->Count) {

        KeYieldProcessor();
    }

#endif

    return;
}

VOID
PnprQuiesceWorker (
    __in PVOID Parameter
    )

/*++

Routine Description:

    This work item is responsible for asynchronously quiescing and waking the
    devices in the system.  This operation must be performed at passive level,
    on the same thread and with interrupts enabled.

Arguments:

    Parameter - The work queue item used to queue this workitem.  This is used
        to free the memory once the processing is complete.

Return Value:

    None.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(Parameter);

    NT_ASSERT(!"PnprQuiesceWorker should not be called\n");

#else

    NTSTATUS Status;
    PNP_REPLACE_POWER_CONTEXT PowerContext;
    PVOID Object[2];

    //
    // Wait for the signal to put devices to sleep.  Also allow the signal to
    // wake devices to satisfy the wait.  If the signal to wake devices is set
    // before this routine signals that the devices have been quiesced, then
    // this is interpreted as a cancellation of the device quiesce step.  Just
    // return.
    //

    Object[0] = &PnprContext->BeginQuiesce;
    Object[1] = &PnprContext->BeginWake;
    Status = KeWaitForMultipleObjects(2,
                                      &Object[0],
                                      WaitAny,
                                      Executive,
                                      KernelMode,
                                      FALSE,
                                      NULL,
                                      NULL);

    if (Status == STATUS_WAIT_1) {
        goto QuiesceWorkerEnd;
    }

    //
    // Perform quiesce steps.
    //

    PnprLockPagesForReplace();
    Status = PnprQuiesceDevices(&PowerContext);
    PnprContext->QuiesceStatus = Status;

    //
    // Notify the controlling thread that the quiesce is complete.
    //

    KeSetEvent(&PnprContext->QuiesceComplete, IO_NO_INCREMENT, FALSE);
    if (NT_SUCCESS(Status)) {

        //
        // Wait for the signal to indicate that the quiesce steps should be
        // undone.  Once the event is signalled, proceed to the wake steps.
        //

        KeWaitForSingleObject(&PnprContext->BeginWake,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);

        PnprWakeDevices(&PowerContext);
    }

    PnprCompleteWake();

QuiesceWorkerEnd:

    //
    // Signal the event indicating that the device wake is complete.  This will
    // also be signalled if the quiesce was a failure or was cancelled before
    // it could be initiated.  This would happen if the begin wake event is
    // signalled before the begin quiesce event is signalled.
    //

    KeSetEvent(&PnprContext->WakeComplete, IO_NO_INCREMENT, FALSE);

    //
    // Free this work item's context.
    //

    ExFreePoolWithTag(Parameter, PNP_POOLTAG_PARTITION_RESOURCE_LIST);

#endif

    return;
}

NTSTATUS
PnprReplaceStart (
    VOID
    )

/*++

Routine Description:

    This routine initializes the replace parameters and calls into the HAL and
    replace driver interfaces (if necessary) to allow them to initialize.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprReplaceStart should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    PPNP_REPLACE_PARAMETERS DriverParameters;
    HAL_DP_REPLACE_PARAMETERS HalParameters;
    NTSTATUS Status;

    PAGED_CODE();

    Status = STATUS_SUCCESS;
    DriverParameters = &PnprContext->DriverParameters;

    //
    // Inform the HAL that a replace is about to take place.  Populate the
    // parameter structure and make the call.
    //

    RtlZeroMemory(&HalParameters, sizeof(HAL_DP_REPLACE_PARAMETERS));
    HalParameters.TargetProcessors = PnprContext->Target.Processors;
    HalParameters.SpareProcessors = PnprContext->Spare.Processors;
    if (PnprContext->DriverInterface.Flags & PNP_REPLACE_PROCESSOR_SUPPORTED) {
        HalParameters.Flags |= HAL_DP_REPLACE_PROCESSOR_ID_RECONFIGURE;
    }

    if (PnprContext->Flags & PNPR_HARDWARE_QUIESCE) {
        HalParameters.Flags |= HAL_DP_REPLACE_HARDWARE_QUIESCE;
    }

    Status = HalDpReplaceBegin(&HalParameters, &PnprContext->HalContext);
    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(PartitionUnitResourceNotSupported);
        goto ReplaceStartEnd;
    }

    if (PnprContext->Target.Memory->Count != 0) {
        RtlZeroMemory(DriverParameters, sizeof(PNP_REPLACE_PARAMETERS));
        DriverParameters->Size = sizeof(PNP_REPLACE_PARAMETERS);
        DriverParameters->Version = PNP_REPLACE_PARAMETERS_VERSION;
        DriverParameters->Target = PnprContext->Target.UniqueId;
        DriverParameters->Spare = PnprContext->Spare.UniqueId;
        DriverParameters->TargetProcessors = PnprContext->Target.Processors;
        DriverParameters->SpareProcessors = PnprContext->Spare.Processors;
        DriverParameters->TargetMemory = PnprContext->Target.Memory;
        DriverParameters->SpareMemory = PnprContext->Spare.Memory;
        DriverParameters->MapMemory = PnprMapTargetSparePhysicalPages;

        //
        // A plugin driver is not used to replace processors.  Provide a NULL
        // target and spare processor list.
        //

        Status = PnprContext->DriverInterface.BeginReplace(
                     DriverParameters,
                     &PnprContext->DriverContext);

        if (!NT_SUCCESS(Status)) {
            PNPR_REPLACE_FAILURE(ChipsetDriverFailure);
            goto ReplaceStartEnd;
        }
    }

ReplaceStartEnd:
    return Status;

#endif

}

NTSTATUS
PnprSwap (
    VOID
    )

/*++

Routine Description:

    This routine copies the target partition state to the spare.

Arguments:

    None.  The global replace context provides the parameters.

Return Value:

    NT status code.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprSwap should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    ULONG Index;
    PPNPR_MAPPING_RESERVE MapReserve;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;

    PNP_ASSERT(KeGetCurrentProcessorIndex() == PnprContext->ControlProcessor);

    //
    // Start by mirroring the pages that have been queued for copying.
    //

    if ((PnprContext->Target.Memory->Count != 0) &&
        ((PnprContext->Flags & PNPR_HARDWARE_MIRRORING) == 0)) {

        if (PnprContext->SwapPhase < SWAP_PHASE_COPY_MEMORY) {
            PnprContext->SwapPhase = SWAP_PHASE_COPY_MEMORY;
        }

        Status = PnprMirrorMarkedPages();
        if (!NT_SUCCESS(Status)) {
            PNPR_REPLACE_FAILURE(GeneralFailure);
            goto SwapEnd;
        }

        PnprContext->SwapBarrier = 1;
        PnprContext->SwapPhase = SWAP_PHASE_EXIT_COPY;
        while (PnprContext->SwapBarrier < (LONG)PnprContext->ProcessorCount) {
            KeYieldProcessor();
        }
    }

    //
    // Copy any platform memory that is not managed by the memory manager.
    //

    if (PnprContext->DriverInterface.MirrorPlatformMemory != NULL) {
        Status = PnprContext->DriverInterface.MirrorPlatformMemory(
                     PnprContext->DriverContext);

        if (!NT_SUCCESS(Status)) {
            PNPR_REPLACE_FAILURE(ChipsetDriverFailure);
            goto SwapEnd;
        }
    }

    //
    // Rendezvous the other processors before the swap.
    //

    PnprContext->SwapBarrier = 1;
    PnprContext->SwapPhase = SWAP_PHASE_SWAP_PREPARE;
    while (PnprContext->SwapBarrier < (LONG)PnprContext->ProcessorCount) {
        KeYieldProcessor();
    }

    //
    // Copy the final necessary pages and perform the swap.
    //

    Status = PnprSwapFinalize();

    //
    // The swap has occurred.  Unmap the reserves  used during the swap.  Do
    // this here since these unmaps are handled specially during a memory
    // mirroring operation.
    //

    if (PnprContext->Target.Memory->Count != 0) {
        for (Index = 0; Index < PnprContext->ProcessorCount; Index += 1) {
            MapReserve = &PnprContext->TargetReserves[Index];
            if ((MapReserve != NULL) &&
                (MapReserve->Mdl != NULL) &&
                (MapReserve->Mdl->MdlFlags & MDL_MAPPED_TO_SYSTEM_VA)) {

                PNP_ASSERT(MapReserve->Reserve != NULL);

                MmUnmapReservedMapping(MapReserve->Reserve,
                                       PNP_POOLTAG_PARTITION_RESOURCE_LIST,
                                       MapReserve->Mdl);
            }

            MapReserve = &PnprContext->SpareReserves[Index];
            if ((MapReserve != NULL) &&
                (MapReserve->Mdl != NULL) &&
                (MapReserve->Mdl->MdlFlags & MDL_MAPPED_TO_SYSTEM_VA)) {

                PNP_ASSERT(MapReserve->Reserve != NULL);

                MmUnmapReservedMapping(MapReserve->Reserve,
                                       PNP_POOLTAG_PARTITION_RESOURCE_LIST,
                                       MapReserve->Mdl);
            }
        }
    }

SwapEnd:
    PnprContext->SwapPhase = SWAP_PHASE_SWAPPED;
    return Status;

#endif

}

NTSTATUS
PnprSwapFinalize (
    VOID
    )

/*++

Routine Description:

    This routine performs the final chipset swap.  It also copies any memory
    used during the mirroring process in order to get an accurate copy of memory
    in the spare before swapping.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS if the operation succeeds.

    An error status code if the plugin driver fails the swap call.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprSwapFinalize should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    NTSTATUS Status;

    Status = STATUS_SUCCESS;

    //
    // Re-copy the pages that were used during the memory copy process.  These
    // pages might have stale copies in the spare.
    //
    // N.B. - Once this routine returns, any memory modified before the swap
    //        occurs will be stale after the swap.
    //

    if ((PnprContext->Target.Memory->Count != 0) &&
        ((PnprContext->Flags & PNPR_HARDWARE_MIRRORING) == 0)) {

        PnprRecopyMirrorPages();
    }

    PnprContext->Flags &= ~PNPR_SWAP_IN_PROGRESS;

    //
    // Memory has been copied.  Flush each processor's cache so that the chipset
    // reconfiguration does not result in stale data.
    //

    if ((PnprContext->Flags & PNPR_HARDWARE_QUIESCE) == 0) {
        KeSweepLocalCaches();
    }

    //
    // Prevent the compiler from re-ordering any code after the swap, since the
    // stack of this processor will be accurate at the time of the recopy above.
    //

    KeMemoryBarrierWithoutFence();

    //
    // Perform the final chipset reconfiguration.
    //

    if (PnprContext->DriverInterface.Swap != NULL) {
        PnprContext->Timing.ChipsetSwapCalled = PnprGetMillisecondCounter(FALSE);
        Status = PnprContext->DriverInterface.Swap(PnprContext->DriverContext);
        if (!NT_SUCCESS(Status)) {
            PNPR_REPLACE_FAILURE(ChipsetDriverFailure);
        }
        PnprContext->Timing.ChipsetSwapComplete = PnprGetMillisecondCounter(FALSE);
    }

    //
    // Return immediately after the swap, so that there is no stack utilization
    // in this frame after the swap.
    //

    return Status;

#endif

}

NTSTATUS
PnprWakeDevices (
    __inout PPNP_REPLACE_POWER_CONTEXT Context
    )

/*++

Routine Description:

    This routine will attempt to return all devices in the system to D0. This
    is accomplished by calling into the power manager to broadcast an S4 IRP.

Arguments:

    None

Return Value:

    NT status code.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(Context);

    NT_ASSERT(!"PnprWakeDevices should never be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    PnprContext->Timing.DeviceWakeStart = PnprGetMillisecondCounter(FALSE);
    Context->Broadcast.Waking = TRUE;
    PoBroadcastSystemState(&Context->Broadcast);
    PoClearBroadcast();

    //
    // Notify the power manager that the replace has ended.
    //

    PoEndPartitionReplace(&Context->Win32Parameters, Context->ConsoleId);
    PoEndPowerStateTasks(&Context->Win32Parameters);
    PoUnblockConsoleSwitch(&Context->Win32Parameters, Context->ConsoleId);
    PnprContext->Timing.DeviceWakeEnd = PnprGetMillisecondCounter(FALSE);
    return STATUS_SUCCESS;

#endif

}

VOID
PnprWakeProcessors (
    VOID
    )

/*++

Routine Description:

    This routine will return the system to full operation after a hot-replace
    has been completed.  The processor will be released at elevated IRQL so
    that the IRQL can be lowered by MmDuplicateMemory.

Arguments:

    None.

Return Value:

    None.  The system will not recover from a failure of this nature.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprWakeProcessors should never be called\n");

#else

    ULONGLONG Time;

    //
    // The other processors are synchronized and waiting to be
    // woken up. Because they have all their necessary state and are
    // simply waiting on a barrier, it's safe to disable the hardware
    // quiesce engine here.
    //

    if ((PnprContext->Flags & PNPR_HARDWARE_QUIESCE) != 0) {
        PnprContext->DriverInterface.EnableDisableHardwareQuiesce(
                PnprContext->DriverContext, FALSE);

        PnprContext->Timing.QuiesceDisabled = PnprGetMillisecondCounter(FALSE);
        PnprContext->SwapPhase = SWAP_PHASE_QUIESCE_ENGINE_OFF;
    }

    //
    // Perform some additional processor state restoration which needs to
    // broadcast IPIs to all processors.
    //

#if defined(_X86_) || defined(_AMD64_)

    //
    // Both x86 and amd64 require MTRR restoration after a hot-replace
    // operation.
    //

    KeRestoreMtrrBroadcast();

#endif

    //
    // The TSCs in the spare processors have been initialized with a TSC
    // value captured right before the target processor(s) went offline.
    // Synchronize the processor TSCs to account for this small skew
    // introduced during the replace operation. It is safe to do this
    // in the hardware quiesce case because interrupts have been enabled
    // on the spares.
    //

    HalTscSynchronization(FALSE, NULL);

    //
    // Resume clock ticks.
    //

    KeResumeClockTimerSafe();

    //
    // Restore ACPI events.
    //

    HalDpReplaceControl(HAL_DP_REPLACE_PHASE_WAKE, PnprContext->HalContext);

    //
    // Compute the elapsed time during the quiesce in 100ns units.
    //

    Time = PnprGetMillisecondCounter(FALSE);
    Time -= PnprContext->Timing.QuiesceTime;
    PnprContext->Timing.QuiesceTime = Time * 1000 * 10;

    //
    // Release all processors.  By this point, all other processors are back
    // from the HAL and are spinning in the quiesce processors DPC routine.
    //

    PnprContext->SwapPhase = SWAP_PHASE_RELEASE;
    InterlockedIncrement(&PnprContext->ExitBarrier);

    //
    // The other processors are running, so it's safe to resume
    // delivery of level-triggered interrupts.
    //

    if ((PnprContext->Flags & PNPR_HARDWARE_QUIESCE) != 0) {
        HalDpUnmaskLevelTriggeredInterrupts();
    }

#endif

    return;
}

NTSTATUS
PnprStartMirroring (
    VOID
    )

/*++

Routine Description:

    This callback is made at the beginning of the memory duplication process.
    Notthing is done.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprStartMirroring should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    PnprContext->MirrorPhase = 0;
    PnprContext->Flags |= PNPR_SWAP_IN_PROGRESS;
    return STATUS_SUCCESS;

#endif

}

NTSTATUS
PnprEndMirroring (
    __in ULONG PassNumber
    )

/*++

Routine Description:

    This routine gets invoked at the end of the memory mirroring phase during
    hot replace.

Arguments:

    PassNumber - Supplies the mirror pass number.

Return Value:

    In phase 0, STATUS_SUCCESS.  In phase 1, NT status code.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(PassNumber);

    NT_ASSERT(!"PnprEndMirroring should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    NTSTATUS Status;

    PNP_ASSERT(PassNumber <= 1);
    PNP_ASSERT(PnprContext != NULL);
    PNP_ASSERT(KeGetCurrentProcessorIndex() == PnprContext->ControlProcessor);

    Status = STATUS_SUCCESS;
    if (PassNumber == 0) {
        PnprContext->MirrorPhase = 1;
        Status = PnprQuiesce();

    } else if (PassNumber == 1) {
        Status = PnprSwap();
        if (!NT_SUCCESS(Status)) {
            HalDpReplaceControl(HAL_DP_REPLACE_PHASE_CANCEL,
                                PnprContext->HalContext);
        }

        //
        // After the swap, wakeup the processors.  Do not delegate this to
        // the quiesce/wake work item, since the memory manager will lower
        // IRQL before returning from memory mirroring.  The processors should
        // be released before IRQL returns to passive level.
        //

        PnprWakeProcessors();

        //
        // Return a sentinel status code to the memory mirroring engine.  This
        // code indicates that the system is now running on the memory that is
        // the target of the mirror.  This means that pages that were not copied
        // by the mirror must not have any assumptions made about them - they
        // cannot be assumed to be zeroed, for instance.  The status code allows
        // the memory manager to adjust its state accordingly.
        //

        if (NT_SUCCESS(Status)) {
            Status = STATUS_WAKE_SYSTEM;
        }
    }

    return Status;

#endif

}

NTSTATUS
PnprMarkOrMirrorPages (
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in LARGE_INTEGER NumberOfBytes,
    __in BOOLEAN Mirror
    )

/*++

Routine Description:

    This routine marks pages for duplication once the system is quiescent. The
    pages to be duplicated are maintained in a list of bitmaps, each one
    corresponding to a range in the replace memory list.  If the range is not
    part of the target module, then it is ignored.

Arguments:

    PhysicalAddress - Supplies the physical address of the start of the range
        to be marked for duplication.  This must be page aligned.

    NumberOfBytes - Supplies the number of bytes to be marked for duplication.
        This must be aligned to a page size.

    Mirror - Supplies a boolean value indicating whether the pages should be
        mirrored or marked for later mirroring.

Return Value:

    STATUS_SUCCESS.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(PhysicalAddress);
    UNREFERENCED_PARAMETER(NumberOfBytes);
    UNREFERENCED_PARAMETER(Mirror);

    NT_ASSERT(!"PnprMarkOrMirrorPages should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    PFN_NUMBER EndPfn;
    PPNPR_MEMORY_BITMAP_ENTRY Entry;
    PFN_NUMBER EntryEndPfn;
    PFN_NUMBER EntryStartPfn;
    PLIST_ENTRY Links;
    KLOCK_QUEUE_HANDLE LockHandle;
    PREPLACE_MIRROR_PHYSICAL_MEMORY MirrorRoutine;
    PHYSICAL_ADDRESS OverlapAddress;
    PFN_NUMBER OverlapEnd;
    LARGE_INTEGER OverlapLength;
    PFN_NUMBER OverlapStart;
    PFN_NUMBER Pages;
    PFN_NUMBER StartPfn;
    NTSTATUS Status;

    //
    // Assert that the parameters are honoring page alignment.
    //

    NT_ASSERT((PhysicalAddress.QuadPart & (PAGE_SIZE - 1)) == 0);
    NT_ASSERT((NumberOfBytes.QuadPart & (PAGE_SIZE - 1)) == 0);

    //
    // Convert the input address to starting and ending physical page numbers.
    //

    StartPfn = (PFN_NUMBER)((ULONG64)PhysicalAddress.QuadPart >> PAGE_SHIFT);
    Pages = (PFN_NUMBER)((ULONG64)NumberOfBytes.QuadPart >> PAGE_SHIFT);
    EndPfn = StartPfn + Pages - 1;

    //
    // Iterate through the memory map list searching for ranges of pages that
    // have overlap with the specified range of pages.  It has already been
    // determined the the ranges in the list are mutually exclusive and non-
    // empty.
    //

    for (Links = PnprContext->TargetMemoryMapList.Flink;
         Links != &PnprContext->TargetMemoryMapList;
         Links = Links->Flink) {

        Entry = CONTAINING_RECORD(Links, PNPR_MEMORY_BITMAP_ENTRY, Links);
        EntryStartPfn = Entry->StartPfn;
        EntryEndPfn = EntryStartPfn + Entry->Pages - 1;

        //
        // If the range to be marked is below the current range, then the range
        // is not represented in the target module.  This is because the list is
        // sorted in order of starting PFN and there are no overlapping ranges.
        //

        if (EndPfn < EntryStartPfn) {
            break;
        }

        //
        // Calculate the possible overlap between the range in the range of the
        // current bitmap entry.
        //

        OverlapStart = (PFN_NUMBER)max(StartPfn, EntryStartPfn);
        OverlapEnd = (PFN_NUMBER)min(EndPfn, EntryEndPfn);

        //
        // If there is no overlap, move on to the next entry.
        //

        if (OverlapStart > OverlapEnd) {
            continue;
        }

        //
        // If there is overlap, then either clear the bits correponding to the
        // overlapping pages or mirror the memory directly.
        //
        // N.B. Bit index zero of this bitmap correponds to the page specified
        // by the starting PFN of the bitmap entry.
        //

        if (Mirror != FALSE) {
            OverlapAddress.QuadPart = OverlapStart << PAGE_SHIFT;
            OverlapLength.QuadPart =
                (OverlapEnd - OverlapStart + 1) << PAGE_SHIFT;

            MirrorRoutine = PnprContext->DriverInterface.MirrorPhysicalMemory;
            Status = MirrorRoutine(PnprContext->DriverContext,
                                   OverlapAddress,
                                   OverlapLength);

            if (!NT_SUCCESS(Status)) {
                PNPR_REPLACE_FAILURE(ChipsetDriverFailure);
            }

        } else {
            IopDbgPrint((IOP_IOAPI_INFO_LEVEL,
                         "Marking for mirror at 0x%I64x (PFN: 0x%I64x) for 0x%I64x bytes (0x%I64x pages)\n",
                         ((ULONGLONG)OverlapStart) << PAGE_SHIFT,
                         (ULONGLONG)OverlapStart,
                         ((ULONGLONG)(OverlapEnd - OverlapStart + 1)) << PAGE_SHIFT,
                         (ULONGLONG)(OverlapEnd - OverlapStart + 1)));

            KeAcquireInStackQueuedSpinLock(&PnprContext->MemoryMapListLock,
                                           &LockHandle);

            RtlClearBits(&Entry->Bitmap,
                         (ULONG)(OverlapStart - EntryStartPfn),
                         (ULONG)(OverlapEnd - OverlapStart + 1));

            KeReleaseInStackQueuedSpinLock(&LockHandle);
        }
    }

    return STATUS_SUCCESS;

#endif

}

NTSTATUS
PnprMirrorMarkedPages (
    VOID
    )

/*++

Routine Description:

    This routine is invoked to mirror physical memory during hot replace.  The
    pages to be mirrored are determined from the memory map list that has been
    constructed in callbacks from MmDuplicateMemory.

    N.B. This routine is called simultaneously on multiple different processors.
         It must be multi-thread safe.

Arguments:

    None.

Return Value:

    The mirroring callback status code returned from the chipset plugin
        driver.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprMirrorMarkedPages should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    PPNPR_MEMORY_BITMAP_ENTRY Entry;
    BOOLEAN Exit;
    PLIST_ENTRY Links;
    KLOCK_QUEUE_HANDLE LockHandle;
    PREPLACE_MIRROR_PHYSICAL_MEMORY MirrorRoutine;
    ULONG Number;
    LARGE_INTEGER NumberOfBytes;
    ULONG Pages;
    PHYSICAL_ADDRESS PhysicalAddress;
    ULONG StartIndex;
    PFN_NUMBER StartPfn;
    NTSTATUS Status;

    //
    // This processor only participates in mirroring if it has a mapping
    // reserve allocated for it.
    //

    Number = KeGetCurrentProcessorIndex();
    if (PnprContext->TargetReserves[Number].Mdl == NULL) {
        Status = STATUS_SUCCESS;
        goto MirrorMarkedPagesEnd;
    }

    //
    // Always use the chipset driver to mirror the memory.  This may change if
    // support for non-driver assisted memory replace is ever implemented.
    //

    MirrorRoutine = PnprContext->DriverInterface.MirrorPhysicalMemory;
    Status = STATUS_SUCCESS;

    //
    // Search the list of memory bitmaps for ranges to mirror.  On the control
    // processor, process the list once.  On other processors, iterate until the
    // control processor signals that the list has been built completely.
    //

    if (Number == PnprContext->ControlProcessor) {
        Exit = TRUE;

    } else {
        Exit = FALSE;
    }

    do {
        for (Links = PnprContext->TargetMemoryMapList.Flink;
             Links != &PnprContext->TargetMemoryMapList;
             Links = Links->Flink) {

            Entry = CONTAINING_RECORD(Links, PNPR_MEMORY_BITMAP_ENTRY, Links);
            KeAcquireInStackQueuedSpinLockAtDpcLevel(
                &PnprContext->MemoryMapListLock,
                &LockHandle);

            Pages = RtlFindFirstRunClear(&Entry->Bitmap, &StartIndex);
            if (Pages > 0) {
                Pages = min(Pages, PNPR_MAXIMUM_PAGE_RUN * 20);
                RtlSetBits(&Entry->Bitmap, StartIndex, Pages);
            }

            KeReleaseInStackQueuedSpinLockFromDpcLevel(&LockHandle);
            while (Pages != 0) {
                StartPfn = Entry->StartPfn + StartIndex;
                PhysicalAddress.QuadPart = ((LONGLONG)StartPfn) << PAGE_SHIFT;
                NumberOfBytes.QuadPart = ((LONGLONG)Pages) << PAGE_SHIFT;

                IopDbgPrint((IOP_IOAPI_INFO_LEVEL,
                             "Mirroring at 0x%I64x (PFN: 0x%I64x) for 0x%I64x bytes (0x%I64x pages)\n",
                             (ULONGLONG)PhysicalAddress.QuadPart,
                             (ULONGLONG)StartPfn,
                             (ULONGLONG)NumberOfBytes.QuadPart,
                             (ULONGLONG)Pages));

                Status = MirrorRoutine(PnprContext->DriverContext,
                                       PhysicalAddress,
                                       NumberOfBytes);

                if (!NT_SUCCESS(Status)) {
                    PNPR_REPLACE_FAILURE(ChipsetDriverFailure);
                    goto MirrorMarkedPagesEnd;
                }

                //
                // Care was taken to allocate the bitmap with slightly more bits
                // than pages, and so there will always be one or more set bits
                // at the end of the bitmap.  There is no need to specifically
                // check whether the end of the bitmap has been reached.  Just
                // look for another run of clear bits.
                //

                StartIndex += Pages;
                KeAcquireInStackQueuedSpinLockAtDpcLevel(
                    &PnprContext->MemoryMapListLock,
                    &LockHandle);

                Pages = RtlFindNextForwardRunClear(&Entry->Bitmap,
                                                   StartIndex,
                                                   &StartIndex);

                if (Pages > 0) {
                    Pages = min(Pages, PNPR_MAXIMUM_PAGE_RUN * 20);
                    RtlSetBits(&Entry->Bitmap, StartIndex, Pages);
                }

                KeReleaseInStackQueuedSpinLockFromDpcLevel(&LockHandle);
            }

            if (PnprContext->SwapPhase == SWAP_PHASE_EXIT_COPY) {
                Exit = TRUE;
                break;
            }
        }
    } while (Exit == FALSE);

MirrorMarkedPagesEnd:
    if (Number != PnprContext->ControlProcessor) {
        while (PnprContext->SwapPhase < SWAP_PHASE_EXIT_COPY) {
            KeYieldProcessor();
        }
    }

    return Status;

#endif

}

NTSTATUS
PnprMirrorPhysicalMemory (
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in LARGE_INTEGER NumberOfBytes
    )

/*++

Routine Description:

    This routine mirrors physical memory from the target to the spare partition
    unit.  This only copies the pages immediately if the pages can be copied
    underneath the OS, without mapping the memory for copying, and only then
    during phase 0 of the copy operation.  This is for two reasons.  First,
    phase 0 copies, which occur while the OS is active, run the risk of
    generating conflicting cache attributes, since the mapping done here would
    not be tracked with the mappings done by the owner of the page.  So phase 0
    requests that cannot be copied by hardware cannot be copied immediately.
    Second, all phase 1 copies are batched together to get maximal length page
    runs for efficient copying.

Arguments:

    PhysicalAddress - Supplies the physical address of the start of the range
        to be duplicated.  This must be page aligned.

    NumberOfBytes - Supplies the number of bytes to be duplicated.
        This must be aligned to a page size.

Return Value:

    STATUS_SUCCESS, or an error status code returned from the plug-in driver.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(PhysicalAddress);
    UNREFERENCED_PARAMETER(NumberOfBytes);

    NT_ASSERT(!"PnprMirrorPhysicalMemory should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    BOOLEAN Mirror;

    //
    // In phase 0, copy memory directly.  In phase 1, mark the pages to be
    // copied.  The marked pages will be copied once all pages to be copied have
    // been marked.
    //

    if (PnprContext->MirrorPhase > 0) {

        //
        // If this is the first call to mirror in phase 1, release the other
        // processors to begin copying the marked memory.
        //

        if (PnprContext->SwapPhase < SWAP_PHASE_COPY_MEMORY) {
            PnprContext->SwapPhase = SWAP_PHASE_COPY_MEMORY;
        }

        Mirror = FALSE;

        //
        // Read the performance counter.  While the system is quiesced this must
        // be called repeatedly to stay in sync - rollover interrupts are not
        // processed.
        //

        PnprGetMillisecondCounter(FALSE);

    } else {
        Mirror = TRUE;
    }

    return PnprMarkOrMirrorPages(PhysicalAddress, NumberOfBytes, Mirror);

#endif

}

NTSTATUS
PnprAllocateMappingReserves (
    __out PPNPR_MAPPING_RESERVE *TargetReservesOut,
    __out PPNPR_MAPPING_RESERVE *SpareReservesOut,
    __in ULONG ProcessorCount
    )

/*++

Routine Description:

    This routine allocates per-processor arrays of mapping reserves for use
    during memory mirroring.

Arguments:

    TargetReservesOut - Supplies a pointer to a variable that receives the
        address of an array of reserves for target memory.

    SpareReservesOut - Supplies a pointer to a variable that receives the
        address of an array of reserves for spare memory.

    ProcessorCount - Supplies the number of processors in the system, which
        is the number of elements in the target and spare reserve arrays.

Return Value:

    NT Status code.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(TargetReservesOut);
    UNREFERENCED_PARAMETER(SpareReservesOut);
    UNREFERENCED_PARAMETER(ProcessorCount);

    NT_ASSERT(!"PnprAllocateMappingReserves should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    ULONG Index;
    PPNPR_MAPPING_RESERVE SpareReserves;
    ULONG Size;
    NTSTATUS Status;
    PPNPR_MAPPING_RESERVE TargetReserves;

    PAGED_CODE();

    //
    // Allocate per-processor mapping reserves.  Processors will not be added
    // while the replace process is running, so the active processor count is
    // sufficient.
    //

    TargetReserves = NULL;
    SpareReserves = NULL;
    Size = sizeof(PNPR_MAPPING_RESERVE) * ProcessorCount;
    TargetReserves = ExAllocatePoolWithTag(NonPagedPoolNx,
                                           Size,
                                           PNP_POOLTAG_PARTITION_RESOURCE_LIST);

    if (TargetReserves == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto AllocateMappingReservesExit;
    }

    SpareReserves = ExAllocatePoolWithTag(NonPagedPoolNx,
                                          Size,
                                          PNP_POOLTAG_PARTITION_RESOURCE_LIST);

    if (SpareReserves == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto AllocateMappingReservesExit;
    }

    RtlZeroMemory(TargetReserves, Size);
    RtlZeroMemory(SpareReserves, Size);
    for (Index = 0; Index < ProcessorCount; Index += 1) {
        Status = PnprInitializeMappingReserve(MDL_PAGES_LOCKED,
                                              &TargetReserves[Index]);

        if (!NT_SUCCESS(Status)) {
            goto AllocateMappingReservesExit;
        }

        Status = PnprInitializeMappingReserve(MDL_PAGES_LOCKED | MDL_IO_SPACE,
                                              &SpareReserves[Index]);

        if (!NT_SUCCESS(Status)) {
            PnprFreeMappingReserve(&TargetReserves[Index]);
            goto AllocateMappingReservesExit;
        }
    }

    Status = STATUS_SUCCESS;

AllocateMappingReservesExit:
    if (NT_SUCCESS(Status)) {
        *TargetReservesOut = TargetReserves;
        *SpareReservesOut = SpareReserves;

    } else {

        //
        // If the allocation of reserves partially failed, but at least some
        // processors were allocated reserves, return successfully.  Processors
        // will only contribute to mirroring if a corresponding reserve was
        // allocated.  Ensure that at a minimum, the control processor has a
        // reserve allocated.
        //

        if ((TargetReserves != NULL) && (SpareReserves != NULL)) {
            if ((TargetReserves[0].Mdl != NULL) &&
                (SpareReserves[0].Mdl != NULL)) {

                if (TargetReserves[PnprContext->ControlProcessor].Mdl == NULL) {
                    RtlCopyMemory(&TargetReserves[PnprContext->ControlProcessor],
                                  &TargetReserves[0],
                                  sizeof(PNPR_MAPPING_RESERVE));

                    RtlCopyMemory(&SpareReserves[PnprContext->ControlProcessor],
                                  &SpareReserves[0],
                                  sizeof(PNPR_MAPPING_RESERVE));

                    RtlZeroMemory(&TargetReserves[0],
                                  sizeof(PNPR_MAPPING_RESERVE));

                    RtlZeroMemory(&SpareReserves[0],
                                  sizeof(PNPR_MAPPING_RESERVE));
                }

                Status = STATUS_SUCCESS;
                goto AllocateMappingReservesExit;
            }
        }

        if (TargetReserves != NULL) {
            ExFreePoolWithTag(TargetReserves,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);
        }

        if (SpareReserves != NULL) {
            ExFreePoolWithTag(SpareReserves,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);
        }
    }

    return Status;

#endif

}

NTSTATUS
PnprInitializeMappingReserve (
    __in CSHORT MdlFlags,
    __out PPNPR_MAPPING_RESERVE MapReserve
    )

/*++

Routine Description:

    This routine will initialize a reserve mapping and the associated memory
    descriptor list.

Arguments:

    MdlFlags - Supplies the memory descriptor flags to be applied each time the
        reserve mapping is used to map physical pages.

    MapReserve - Supplies a reference to a structure that will receive a
        reference to the new reserve mapping, the new memory descriptor list and
        the flags.

Return Value:

    NT status code.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(MdlFlags);
    UNREFERENCED_PARAMETER(MapReserve);

    NT_ASSERT(!"PnprInitializeMappingReserve should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    PVOID Reserve;
    PMDL Mdl;
    NTSTATUS Status;

    PAGED_CODE();

    Status = STATUS_SUCCESS;
    Reserve = MmAllocateMappingAddress(PNPR_MAXIMUM_PAGE_RUN << PAGE_SHIFT,
                                       PNP_POOLTAG_PARTITION_RESOURCE_LIST);

    if (Reserve == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        PNPR_REPLACE_FAILURE(AllocationFailure);
        goto InitializeMappingReserveEnd;
    }

    Mdl = IoAllocateMdl(Reserve,
                        PNPR_MAXIMUM_PAGE_RUN << PAGE_SHIFT,
                        FALSE,
                        FALSE,
                        NULL);

    if (Mdl == NULL) {
        MmFreeMappingAddress(Reserve,
                             PNP_POOLTAG_PARTITION_RESOURCE_LIST);

        Status = STATUS_INSUFFICIENT_RESOURCES;
        PNPR_REPLACE_FAILURE(AllocationFailure);
        goto InitializeMappingReserveEnd;
    }

    MapReserve->Mdl = Mdl;
    MapReserve->Reserve = Reserve;
    MapReserve->MdlFlags = MdlFlags;

InitializeMappingReserveEnd:
    return Status;

#endif

}

VOID
PnprFreeMappingReserve (
    __inout PPNPR_MAPPING_RESERVE MapReserve
    )

/*++

Routine Description:

    This routine will free the specified mapping reserve, unmapping any pages
    that may be mapped.  If this reserve has not been initialized, then this
    function does nothing.

Arguments:

     MapReserve - Supplies the memory descriptor list and mapping reserve to be
        freed.

Return Value:

    None.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(MapReserve);

    NT_ASSERT(!"PnprFreeMappingReserve should not be called\n");

#else

    PAGED_CODE();

    if (MapReserve->Mdl != NULL) {

        PNP_ASSERT((MapReserve->Mdl->MdlFlags & MDL_MAPPED_TO_SYSTEM_VA) == 0);

        IoFreeMdl(MapReserve->Mdl);
        MapReserve->Mdl = NULL;
    }

    if (MapReserve->Reserve != NULL) {
        MmFreeMappingAddress(MapReserve->Reserve,
                             PNP_POOLTAG_PARTITION_RESOURCE_LIST);

        MapReserve->Reserve = NULL;
    }

    MapReserve->MdlFlags = 0;

#endif

    return;
}

NTSTATUS
PnprMapPhysicalPages (
    __in PPNPR_MAPPING_RESERVE MapReserve,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __inout PLARGE_INTEGER NumberOfBytes,
    __deref_out PVOID *VirtualAddress
    )

/*++

Routine Description:

    This routine performs the actual mapping of physical memory.  If spare pages
    are being mapped, then they are mapped as I/O space.  If target pages are
    being mapped, then it is mapped as regular memory.

Arguments:

    MapReserve - Supplies a reference to a structure providing the memory
        descriptor list, mapping reserve and memory descriptor list flags to be
        used to complete the requested mapping.

    PhysicalAddress - Supplies the physical address of the memory to be mapped.
        This address must be on a page boundary.

    NumberOfBytes - Supplies the number of bytes to be mapped.  The value must
        represent a positive whole number of pages.

    VirtualAddress - Supplies a reference to a variable that, upon successful
        return, contains the virtual address of the resulting mapping.

Return Value:

    STATUS_SUCCESS if the memory regions are mapped successfully.

    STATUS_INVALID_PARAMETER if the physical addresses or byte count are not
        page aligned.

    STATUS_INSUFFICIENT_RESOURCES if there are not sufficient memory resources
        to perform the mapping.

    STATUS_UNABLE_TO_FREE_VM if the routine was called in a context where
        memory mapping is not allowed.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(MapReserve);
    UNREFERENCED_PARAMETER(PhysicalAddress);
    UNREFERENCED_PARAMETER(NumberOfBytes);
    UNREFERENCED_PARAMETER(VirtualAddress);

    NT_ASSERT(!"PnprMapPhysicalPages should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    PVOID Address;
    ULONGLONG MappedBytes;
    PFN_NUMBER Pages;
    PPFN_NUMBER PhysicalPage;
    PFN_NUMBER StartPage;

    if ((PnprContext->Flags & PNPR_SWAP_IN_PROGRESS) == 0) {
        PNPR_REPLACE_FAILURE(ChipsetDriverFailure);
        return STATUS_UNABLE_TO_FREE_VM;
    }

    if (((PhysicalAddress.QuadPart & (PAGE_SIZE - 1)) != 0) ||
        (NumberOfBytes->QuadPart == 0) ||
        ((NumberOfBytes->QuadPart & (PAGE_SIZE - 1)) != 0)) {

        PNPR_REPLACE_FAILURE(ChipsetDriverFailure);
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Limit number of pages that can be mapped at the same time.
    //

    MappedBytes = NumberOfBytes->QuadPart;
    if (MappedBytes > (PNPR_MAXIMUM_PAGE_RUN << PAGE_SHIFT)) {
        MappedBytes = (PNPR_MAXIMUM_PAGE_RUN << PAGE_SHIFT);
    }

    //
    // Unmap the MDL if it is already in use.
    //

    if (MapReserve->Mdl->MdlFlags & MDL_MAPPED_TO_SYSTEM_VA) {
        MmUnmapReservedMapping(MapReserve->Reserve,
                               PNP_POOLTAG_PARTITION_RESOURCE_LIST,
                               MapReserve->Mdl);
    }

    //
    // Manually initialize the MDL to span the specified physical pages.
    //

    MmInitializeMdl(MapReserve->Mdl, NULL, (SIZE_T)MappedBytes);
    PhysicalPage = MmGetMdlPfnArray(MapReserve->Mdl);
    Pages = (PFN_NUMBER)((ULONG64)MappedBytes >> PAGE_SHIFT);
    StartPage = (PFN_NUMBER)((ULONG64)PhysicalAddress.QuadPart >> PAGE_SHIFT);
    while (Pages != 0) {
        *PhysicalPage = StartPage;
        PhysicalPage += 1;
        StartPage += 1;
        Pages -= 1;
    }

    MapReserve->Mdl->MdlFlags |= MapReserve->MdlFlags;

    //
    // This memory map routine uses an MDL mapping reserved pages.  During phase
    // 1 memory mirroring, processors are rendezvoused, the memory manager holds
    // heavy internal locks, and interrupts may even be disable.  As a result
    // generic mapping APIs cannot be used.  Initialize the MDL to cover both
    // target and spare memory pages.
    //

    Address = MmMapLockedPagesWithReservedMapping(
                  MapReserve->Reserve,
                  PNP_POOLTAG_PARTITION_RESOURCE_LIST,
                  MapReserve->Mdl,
                  MmCached);

    if (Address == NULL) {
        PNPR_REPLACE_FAILURE(AllocationFailure);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    *VirtualAddress = Address;
    NumberOfBytes->QuadPart = MappedBytes;
    return STATUS_SUCCESS;

#endif

}

NTSTATUS
PnprMapTargetSparePhysicalPages (
    __in PHYSICAL_ADDRESS TargetPhysicalAddress,
    __in PHYSICAL_ADDRESS SparePhysicalAddress,
    __inout PLARGE_INTEGER NumberOfBytes,
    __deref_out PVOID *TargetAddress,
    __deref_out PVOID *SpareAddress
    )

/*++

Routine Description:

    This routine maps physical memory on behalf of the chipset plugin driver
    during a replace operation. The plugin driver uses this if the memory copy
    is performed by the CPU by mapping target and spare memory. This would get
    called from inside the physical memory mirroring entrypoint in the plugin
    driver.  The plugin driver must call this with contigous memory chunks -
    both in target and spare memory.

Arguments:

    TargetPhysicalAddress - Supplies the physical address in target memory to be
        mapped.

    SparePhysicalAddress - Supplies the physical address in spare memory to be
        mapped.

    NumberOfBytes - Supplies the size of target memory to be mapped in bytes.
        Its assumed that the spare memory size is same as target memory.

    TargetAddress - Supplies a pointer to a variable that receives the address
        of the mapped target physical memory.

    SpareAddress - Supplies a pointer to a variable that receives the address
        of the mapped spare physical memory.

Return Value:

    STATUS_SUCCESS if the memory regions are mapped successfully.

    STATUS_INVALID_PARAMETER if the physical addresses or byte count are not
        page aligned.

    STATUS_INSUFFICIENT_RESOURCES if there are not sufficient memory resources
        to perform the mapping.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(TargetPhysicalAddress);
    UNREFERENCED_PARAMETER(SparePhysicalAddress);
    UNREFERENCED_PARAMETER(NumberOfBytes);
    UNREFERENCED_PARAMETER(TargetAddress);
    UNREFERENCED_PARAMETER(SpareAddress);

    NT_ASSERT(!"PnprMapTargetSparePhysicalPages should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    ULONG Number;
    NTSTATUS Status;

    Number = KeGetCurrentProcessorIndex();
    Status = STATUS_SUCCESS;
    if (TargetPhysicalAddress.QuadPart != PNP_REPLACE_NO_MAP) {
        Status = PnprMapPhysicalPages(&PnprContext->TargetReserves[Number],
                                      TargetPhysicalAddress,
                                      NumberOfBytes,
                                      TargetAddress);

        if (!NT_SUCCESS(Status)) {
            PNPR_REPLACE_FAILURE(AllocationFailure);
            goto MapTargetSparePhysicalPagesEnd;
        }
    }

    if (SparePhysicalAddress.QuadPart != PNP_REPLACE_NO_MAP) {
        Status = PnprMapPhysicalPages(&PnprContext->SpareReserves[Number],
                                      SparePhysicalAddress,
                                      NumberOfBytes,
                                      SpareAddress);

        if (!NT_SUCCESS(Status)) {
            PNPR_REPLACE_FAILURE(AllocationFailure);
            goto MapTargetSparePhysicalPagesEnd;
        }
    }

MapTargetSparePhysicalPagesEnd:
    return Status;

#endif

}

NTSTATUS
PnprMmAddRange (
    __inout PLIST_ENTRY MemoryMapList,
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in LARGE_INTEGER Length
    )

/*++

Routine Description:

    This routine will construct one or more initialized bitmaps representing
    the pages contained in the supplied physical memory range.  The resulting
    entries will be added to the supplied list.

    N.B. This method of accounting will split the memory in the range up if the
         number of pages exceeds 0x80000000.  This is equivalent to 8TB of
         memory.  This is unlikely to occur any time soon, but it is accounted
         for.

    N.B. This function does not check for overlapping ranges.  Once the ranges
         have all been added to the list, the caller can very easily determine
         whether there are overlapping ranges.

Arguments:

    MemoryMapList - Supplies a reference to the head of the list into which the
        newly constructed bitmap entries will be added.

    PhysicalAddress - Supplies the starting physical address of the memory range
        to be added.

    Length - Supplies the length in bytes of the memory range.

Return Value:

    STATUS_SUCCESS if successful.

    STATUS_INSUFFICIENT_RESOURCES if a memory allocation has failed.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(MemoryMapList);
    UNREFERENCED_PARAMETER(PhysicalAddress);
    UNREFERENCED_PARAMETER(Length);

    NT_ASSERT(!"PnprMmAddRange should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    ULONG Bits;
    PPNPR_MEMORY_BITMAP_ENTRY Entry;
    PPNPR_MEMORY_BITMAP_ENTRY InsertEntry;
    PLIST_ENTRY InsertLinks;
    PFN_NUMBER Pages;
    ULONG Size;
    PFN_NUMBER StartPfn;
    NTSTATUS Status;
    PFN_NUMBER TotalPages;

    PAGED_CODE();

    Status = STATUS_SUCCESS;

    //
    // Sanity check to ensure that the physical address is on a page boundary
    // as well as that the length is a round number of pages.
    //

    NT_ASSERT((PhysicalAddress.QuadPart & (PAGE_SIZE - 1)) == 0);
    NT_ASSERT((Length.QuadPart & (PAGE_SIZE - 1)) == 0);

    //
    // Obtain the starting PFN and number of pages of the range.
    //

    StartPfn = (PFN_NUMBER)((ULONG64)PhysicalAddress.QuadPart) >> PAGE_SHIFT;
    TotalPages = (PFN_NUMBER)((ULONG64)Length.QuadPart) >> PAGE_SHIFT;

    //
    // Runtime library bitmaps can only represent MAXULONG bits.  To keep the
    // number of pages represented in each chunk neat, split the range into
    // chunks of 0x80000000 pages.
    //

    while (TotalPages > 0) {
        if (TotalPages > MAX_MEMORY_BITMAP_SIZE) {
            Pages = MAX_MEMORY_BITMAP_SIZE;

        } else {
            Pages = TotalPages;
        }

        //
        // Runtime library bitmaps require that the bitmap length be a multiple
        // of the number of bits in a ULONG.  Perform the necessary rounding up.
        // An extra ULONG is added to the end if the number of bits fits in
        // exactly.
        //

        Bits = (ULONG)((Pages + 32) & ~31);
        Size = sizeof(PNPR_MEMORY_BITMAP_ENTRY) + (Bits / 8);
        Entry = ExAllocatePoolWithTag(NonPagedPoolNx,
                                      Size,
                                      PNP_POOLTAG_MEMORY_BITMAP);

        //
        // It is the responsibility of the caller to clean up the list should
        // this function fail.
        //

        if (Entry == NULL) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            PNPR_REPLACE_FAILURE(AllocationFailure);
            goto AddRangeToMemoryMapListEnd;
        }

        Entry->StartPfn = StartPfn;
        Entry->Pages = Pages;
        RtlInitializeBitMap(&Entry->Bitmap,
                            Add2Ptr(Entry, sizeof(PNPR_MEMORY_BITMAP_ENTRY)),
                            Bits);

        IopDbgPrint((IOP_IOAPI_INFO_LEVEL,
                     "Added bitmap for memory at PFN 0x%I64x for 0x%I64x pages.\n",
                     (ULONGLONG)StartPfn,
                     (ULONGLONG)Pages));

        //
        // The runtime library bitmap makes it a lot easier to find runs of
        // clear bits than runs of set bits.  Initialize the bits to set, and
        // then later clear them to indicate that the page should be mirrored.
        //

        RtlSetAllBits(&Entry->Bitmap);

        //
        // Insert the entry into the list in ascending order.  This will make
        // for easier searching later.  Overlapping ranges are not allowed, but
        // that must be checked for by the caller.
        //

        for (InsertLinks = MemoryMapList->Blink;
             InsertLinks != MemoryMapList;
             InsertLinks = InsertLinks->Blink) {

            InsertEntry = CONTAINING_RECORD(InsertLinks,
                                            PNPR_MEMORY_BITMAP_ENTRY,
                                            Links);

            if (InsertEntry->StartPfn < Entry->StartPfn) {
                break;
            }
        }

        InsertHeadList(InsertLinks, &Entry->Links);

        //
        // Calculate the start of the next range of pages, as well as how many
        // pages are left to process.
        //

        StartPfn += Pages;
        TotalPages -= Pages;
    }

AddRangeToMemoryMapListEnd:
    return Status;

#endif

}

NTSTATUS
PnprMmConstruct (
    __in PPNP_REPLACE_MEMORY_LIST MemoryList,
    __inout PLIST_ENTRY MemoryMapList
    )

/*++

Routine Description:

    This routine will construct a list of bitmaps describing the physical pages
    corresponding to the ranges described in the supplied memory list.

    N.B. This list will be returned in ascending order of physical address.

    N.B. This function will fail if there are overlapping ranges specified in
         the memory resources.

Arguments:

    MemoryList - Supplies the description of the module's memory resources.

    MemoryMapList - Supplies a reference to an empty list that will be filled
        with the bitmaps describing the physical pages.

Return Value:

    STATUS_SUCCESS if successful.

    STATUS_INSUFFICIENT_RESOURCES in the event of a pool allocation failure.

    STATUS_INVALID_PARAMETER if two of the memory ranges overlap.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(MemoryList);
    UNREFERENCED_PARAMETER(MemoryMapList);

    NT_ASSERT(!"PnprMmConstruct should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    PHYSICAL_ADDRESS Address;
    ULONG Count;
    PPNPR_MEMORY_BITMAP_ENTRY Entry;
    LARGE_INTEGER Length;
    PLIST_ENTRY Links;
    PFN_NUMBER NextPfn;
    NTSTATUS Status;

    PAGED_CODE();

    Status = STATUS_SUCCESS;

    //
    // Add a new bitmap to the memory map list for each memory range in the
    // module replace memory list.
    //

    for (Count = 0; Count < MemoryList->Count; Count += 1) {
        Address.QuadPart = MemoryList->Ranges[Count].Address.QuadPart;
        Length.QuadPart = (LONGLONG)MemoryList->Ranges[Count].Length;
        Status = PnprMmAddRange(MemoryMapList, Address, Length);
        if (!NT_SUCCESS(Status)) {
            PNPR_REPLACE_FAILURE(GeneralFailure);
            goto ConstructMemoryMapListEnd;
        }
    }

    //
    // Now iterate through the memory map list and ensure that there are no
    // overlapping ranges and no zero length tanges.  Since the entries are in
    // order of starting PFN, the former is is a simple matter of making sure
    // that the starting PFN isn't less than the sum of the previous entry's PFN
    // and page count.
    //

    NextPfn = 0;
    for (Links = MemoryMapList->Flink;
         Links != MemoryMapList;
         Links = Links->Flink) {

        Entry = CONTAINING_RECORD(Links, PNPR_MEMORY_BITMAP_ENTRY, Links);
        if ((Entry->Pages == 0) || (Entry->StartPfn < NextPfn)) {
            Status = STATUS_INVALID_PARAMETER;
            PNPR_REPLACE_FAILURE(PartitionUnitResourceNotSupported);
            goto ConstructMemoryMapListEnd;
        }

        NextPfn = Entry->StartPfn + Entry->Pages;
    }

ConstructMemoryMapListEnd:
    if (!NT_SUCCESS(Status)) {
        PnprMmFree(MemoryMapList);
    }

    return Status;

#endif

}

VOID
PnprMmFree (
    __inout PLIST_ENTRY MemoryMapList
    )

/*++

Routine Description:

    This routine will free the storage occupied by the supplied memory map list.

Arguments:

    MemoryMapList - Supplies a reference to the memory map list to be freed.

Return Value:

    None.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(MemoryMapList);

    NT_ASSERT(!"PnprMmFree should not be called\n");

#else

    PLIST_ENTRY Links;
    PPNPR_MEMORY_BITMAP_ENTRY Entry;

    PAGED_CODE();

    while (!IsListEmpty(MemoryMapList)) {
        Links = RemoveHeadList(MemoryMapList);
        Entry = CONTAINING_RECORD(Links, PNPR_MEMORY_BITMAP_ENTRY, Links);
        ExFreePoolWithTag(Entry, PNP_POOLTAG_MEMORY_BITMAP);
    }

#endif

    return;
}

NTSTATUS
PnprGetPluginDriverImagePath (
    __inout PUNICODE_STRING ImagePath
    )

/*++

Routine Description:

    This routine gets the fully qualified path of the chipset driver.

Arguments:

    ImagePath - Supplies a pointer that receives the fully qualified path of the
        chipset interface driver.

Return Value:

    NTSTATUS.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(ImagePath);

    NT_ASSERT(!"PnprGetPluginDriverImagePath should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    ULONG BufferLength;
    HANDLE ControlPnpHandle;
    UNICODE_STRING ControlPnpName;
    HANDLE CurrentControlSetHandle;
    PWSTR ImagePathBuffer;
    ULONG InformationSize;
    PKEY_VALUE_PARTIAL_INFORMATION KeyInformation;
    OBJECT_ATTRIBUTES ObjectAttributes;
    NTSTATUS Status;
    UNICODE_STRING ValueName;

    PAGED_CODE();

    ImagePathBuffer = NULL;
    ControlPnpHandle = NULL;
    KeyInformation = NULL;

    //
    // Open a handle to the registry key that contains the value of the path
    // the the replace driver.
    //

    Status = _PnpCtxGetCachedContextBaseKey(PiPnpRtlCtx,
                                            PNPCTX_BASE_KEY_TYPE_CONTROLSET,
                                            &CurrentControlSetHandle);

    if (!NT_SUCCESS(Status)) {
        goto GetPluginDriverImagePathEnd;
    }

    RtlInitUnicodeString(&ControlPnpName, REGSTR_PATH_CONTROL_PNP);
    InitializeObjectAttributes(&ObjectAttributes,
                               &ControlPnpName,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               CurrentControlSetHandle,
                               NULL);

    Status = ZwOpenKey(&ControlPnpHandle, KEY_READ, &ObjectAttributes);
    if (!NT_SUCCESS(Status)) {
        goto GetPluginDriverImagePathEnd;
    }

    //
    // Read the value from the registry.  This should fail since the provided
    // buffer is too small.  The returned size is used to size the required
    // buffer.
    //

    RtlInitUnicodeString(&ValueName, PNPR_REPLACE_DRIVER_VALUE);
    Status = ZwQueryValueKey(ControlPnpHandle,
                             &ValueName,
                             KeyValuePartialInformation,
                             NULL,
                             0,
                             &InformationSize);

    //
    // This call should not succeed.  If it does succeed, then this should
    // constitute a failure of unknown origin.  Fail the attempt to obtain the
    // plugin driver image path.
    //

    if ((Status != STATUS_BUFFER_OVERFLOW) &&
        (Status != STATUS_BUFFER_TOO_SMALL)) {

        if (NT_SUCCESS(Status)) {
            Status = STATUS_UNSUCCESSFUL;
        }

        goto GetPluginDriverImagePathEnd;
    }

    PNP_ASSERT(InformationSize >= sizeof(KEY_VALUE_PARTIAL_INFORMATION));

    KeyInformation = ExAllocatePoolWithTag(PagedPool,
                                           InformationSize,
                                           PNP_POOLTAG_PARTITION_RESOURCE_LIST);

    if (KeyInformation == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto GetPluginDriverImagePathEnd;
    }

    Status = ZwQueryValueKey(ControlPnpHandle,
                             &ValueName,
                             KeyValuePartialInformation,
                             KeyInformation,
                             InformationSize,
                             &InformationSize);

    if (!NT_SUCCESS(Status)) {
        goto GetPluginDriverImagePathEnd;
    }

    //
    // If the data is not a string or the data length is not an even number,
    // then the registry information is not valid.
    //

    if (((KeyInformation->Type != REG_SZ) &&
         (KeyInformation->Type != REG_EXPAND_SZ)) ||
        ((KeyInformation->DataLength & 1) == 1)) {

        Status = STATUS_INVALID_PARAMETER;
        goto GetPluginDriverImagePathEnd;
    }

    //
    // This buffer length includes space to add a (possibly redundant) null
    // terminating character.  The data rertieved from the registry may or may
    // not already have a NULL terminator.
    //

    BufferLength = (sizeof(PNPR_REPLACE_DRIVER_ROOT) +
                    KeyInformation->DataLength) / sizeof(WCHAR);

    ImagePathBuffer =
        ExAllocatePoolWithTag(PagedPool,
                              BufferLength * sizeof(WCHAR),
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);

    if (ImagePath == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto GetPluginDriverImagePathEnd;
    }

    //
    // Copy the full path specification into the provided buffer.  The use of
    // wcsncat_s will ensure the proper handling of the case where the registry
    // data has not been NULL terminated.
    //

    wcscpy_s(ImagePathBuffer,
             BufferLength,
             PNPR_REPLACE_DRIVER_ROOT);

    wcsncat_s(ImagePathBuffer,
              BufferLength,
              (PWSTR)&KeyInformation->Data[0],
              KeyInformation->DataLength / sizeof(WCHAR));

    RtlInitUnicodeString(ImagePath, ImagePathBuffer);

GetPluginDriverImagePathEnd:

    //
    // DO NOT close CurrentControlSetHandle.  It is a cached handle and MUST NOT
    // be closed.
    //

    if (!NT_SUCCESS(Status)) {
        if (ImagePathBuffer != NULL) {
            ExFreePoolWithTag(ImagePathBuffer,
                              PNP_POOLTAG_PARTITION_RESOURCE_LIST);
        }
    }

    if (ControlPnpHandle != NULL) {
        ZwClose(ControlPnpHandle);
    }

    if (KeyInformation != NULL) {
        ExFreePoolWithTag(KeyInformation, PNP_POOLTAG_PARTITION_RESOURCE_LIST);
    }

    return Status;

#endif

}

NTSTATUS
PnprLoadPluginDriver (
    __out PVOID *ImageHandle,
    __out PPNP_REPLACE_DRIVER_INTERFACE DriverInterface
    )

/*++

Routine Description:

    This routine loads and initializes the chipset interface driver.

Arguments:

    ImageHandle - Supplies a pointer to a variable that receives the image
        handle to the plugin driver.  This must be passed to a subsequent call
        to unload the driver.

    DriverInterface - Supplies a pointer to a variable that receives the
        driver interface structure, including the driver callbacks that will be
        used to perform the replace.

Return Value:

    STATUS_SUCCESS if the plugin driver is loaded and initialized successfully.

    STATUS_NOT_SUPPORTED if the driver capabilities are less than the minimum
        requirements.

    Another error status code if loading the image or initializing the driver
        fails.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(ImageHandle);
    UNREFERENCED_PARAMETER(DriverInterface);

    NT_ASSERT(!"PnprLoadPluginDriver should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    PDRIVER_INITIALIZE Entrypoint;
    PVOID ImageBase;
    PLDR_DATA_TABLE_ENTRY ImageEntry;
    UNICODE_STRING ImagePath;
    NTSTATUS Status;

    PAGED_CODE();

    ImageEntry = NULL;
    RtlInitUnicodeString(&ImagePath, NULL);
    Status = PnprGetPluginDriverImagePath(&ImagePath);
    if (!NT_SUCCESS(Status)) {
        goto LoadPluginDriverEnd;
    }

    Status = MmLoadSystemImage(&ImagePath,
                               NULL,
                               NULL,
                               MM_LOAD_IMAGE_AND_LOCKDOWN,
                               &ImageEntry,
                               &ImageBase);

    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(AllocationFailure);
        goto LoadPluginDriverEnd;
    }

    //
    // Initialize the plugin driver.
    //

    RtlZeroMemory(DriverInterface, sizeof(PNP_REPLACE_DRIVER_INTERFACE));
    DriverInterface->Version = PNP_REPLACE_DRIVER_INTERFACE_VERSION;
    DriverInterface->Size = sizeof(PNP_REPLACE_DRIVER_INTERFACE);
    Entrypoint = (PDRIVER_INITIALIZE)(ULONG_PTR)ImageEntry->EntryPoint;
    Status = Entrypoint((PVOID)DriverInterface, NULL);
    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(ChipsetDriverFailure);
        goto LoadPluginDriverEnd;
    }

    //
    // The swap, begin replace and end replace entrypoints must be provided by
    // the driver.  Other entrypoints may be required depending on the type of
    // resources involved in the replace.  That validation is done outside of
    // this routine.
    //

    if ((DriverInterface->Size < PNP_REPLACE_DRIVER_INTERFACE_MINIMUM_SIZE) ||
        (DriverInterface->Swap == NULL) ||
        (DriverInterface->BeginReplace == NULL) ||
        (DriverInterface->EndReplace == NULL)) {

        PNPR_REPLACE_FAILURE(ChipsetDriverNotSupported);
        Status = STATUS_NOT_SUPPORTED;
        goto LoadPluginDriverEnd;
    }

    *ImageHandle = ImageEntry;
    Status = STATUS_SUCCESS;

LoadPluginDriverEnd:
    if (!NT_SUCCESS(Status)) {
        if (ImageEntry != NULL) {
            MmUnloadSystemImage(ImageEntry);
        }
    }

    if (ImagePath.Buffer != NULL) {
        ExFreePoolWithTag(ImagePath.Buffer,
                          PNP_POOLTAG_PARTITION_RESOURCE_LIST);
    }

    return Status;

#endif

}

VOID
PnprUnloadPluginDriver (
    __in PVOID ImageHandle,
    __in PPNP_REPLACE_DRIVER_INTERFACE DriverInterface
    )

/*++

Routine Description:

    This routine unloads the chipset interface driver.

Arguments:

    ImageHandle - Supplies a handle to the loaded driver image.

    DriverInterface - Supplies a pointer to the driver-initialized interface
        structure, including an entrypoint to allow the driver to clean up its
        global state.

Return Value:

    None.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(ImageHandle);
    UNREFERENCED_PARAMETER(DriverInterface);

    NT_ASSERT(!"PnprUnloadPluginDriver should not be called\n");

#else

    PAGED_CODE();

    //
    // Allow the plugin driver to cleanup before it is unloaded.
    //

    if (DriverInterface->Unload != NULL) {
        DriverInterface->Unload();
    }

    MmUnloadSystemImage(ImageHandle);

#endif

    return;
}

BOOLEAN
PnprLegacyDeviceDriversPresent (
    VOID
    )

/*++

Routine Description:

    This routine determines whether there are any lagacy (non-plug 'n play)
    device drivers running on the system.  This is useful because hot replace
    must not be attempted if such device drivers are running.

Arguments:

    None.

Return Value:

    TRUE if legacy device drivers are present in the system.

    FALSE otherwise.

--*/

{

    BOOLEAN LegacyDeviceDriversPresent;
    NTSTATUS Status;
    PWSTR VetoList;
    PNP_VETO_TYPE VetoType;

    PAGED_CODE();

    VetoList = NULL;
    LegacyDeviceDriversPresent = FALSE;
    Status = IoGetLegacyVetoList(&VetoList, &VetoType);
    if (NT_SUCCESS(Status)) {
        if (VetoType != PNP_VetoTypeUnknown) {
            LegacyDeviceDriversPresent = TRUE;
        }
    }

    if (VetoList != NULL) {
        ExFreePool(VetoList);
    }

    return LegacyDeviceDriversPresent;
}

ULONG
PnprGetMillisecondCounter (
    BOOLEAN Reset
    )

/*++

Routine Description:

    This routine will obtain the number of milliseconds elapsed since the last
    time the counter was reset.  To retain its accuracy, this function must be
    called at least once per day during a replace operation.  This counter is
    good for a period of around 49.7 days (or as many milliseconds that can be
    represented in a 32-bit unsigned integer.

Arguments:

    Reset - Supplies a boolean flag indicating whether the counter should be
        reset.  If this parameter is true, then the counter will be reset to
        zero and this function will return zero.

Return Value:

    The number of milliseconds elapsed since the last time the counter was
    reset.

--*/

{

    static ULONG CounterValue = 0;
    static ULONG LastMilliseconds = 0;
    ULONG Milliseconds;

#if defined(_IA64_)

    ULONG64 Counter;
    ULONG64 Frequency;

    //
    // IA64 systems are guaranteed to have a fixed frequency processor counter.
    // Use the processor counter in this case to avoid problems with missed
    // platform timer rollovers during quiesce.
    //

    Counter = ReadTimeStampCounter();
    Frequency = KeQueryCycleCounterFrequency(KeGetCurrentPrcb(), FALSE);
    Milliseconds = (ULONG)((Counter * 1000) / Frequency);

#else

    LARGE_INTEGER PerformanceCounter;
    LARGE_INTEGER PerformanceFrequency;

    PerformanceCounter = KeQueryPerformanceCounter(&PerformanceFrequency);
    Milliseconds = (ULONG)((PerformanceCounter.QuadPart * 1000) /
                           PerformanceFrequency.QuadPart);

#endif

    //
    // If the millisecond counter has wrapped around, then ULONG arithmetic will
    // take care of it.
    //

    if (Reset != FALSE) {
        CounterValue = 0;

    } else {
        CounterValue += (Milliseconds - LastMilliseconds);
    }

    LastMilliseconds = Milliseconds;
    return CounterValue;
}

VOID
PnprLockPagesForReplace (
    VOID
    )

/*++

Routine Description:

    This routine will cause all pagable code and data required for a replace
    operation to be locked into memory.  For the kernel, this equates to locking
    the PAGELK sections into memory.  For the HAL and microcode update module,
    a system callback is signalled to cause them to lock their required pagable
    sections.  Once the replace operation is complete, a call must be made to
    PnprUnlockPagesForReplace to unlock the locked pages.

Arguments:

    None.

Return Value:

    None.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprLockPagesForReplace should not be called\n");

#else

    PAGED_CODE();

    //
    // Lock the PAGELK kernel section in memory, and notify the HAL to do the
    // same.  This will also cause the HAL to instruct the microcode update
    // module to lock its PAGELK section in memory.
    //

    MmLockPagableSectionByHandle(ExPageLockHandle);
    ExNotifyCallback(ExCbPowerState, (PVOID)(LONG_PTR)PO_CB_SYSTEM_STATE_LOCK, (PVOID)0);

#endif

    return;
}

VOID
PnprUnlockPagesForReplace (
    VOID
    )

/*++

Routine Description:

    This routine will unlock pages that have been locked by a prior call to
    PnprLockPagesForReplace.

Arguments:

    None.

Return Value:

    None.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprUnlockPagesForReplace should not be called\n");

#else

    //
    // Unlock the PAGELK kernel section and notify the HAL to do the same. This
    // will also cause the HAL to instruct the microcode update module to unlock
    // its PAGELK section.
    //

    MmUnlockPagableImageSection(ExPageLockHandle);
    ExNotifyCallback(ExCbPowerState, (PVOID)(LONG_PTR)PO_CB_SYSTEM_STATE_LOCK, (PVOID)(LONG_PTR)1);

#endif

    return;
}

VOID
PnprLogFailureEvent (
    __in PDEVICE_OBJECT TargetPdo,
    __in PDEVICE_OBJECT SparePdo,
    __in NTSTATUS Status
    )

/*++

Routine Description:

    This routine will log an event indicating the failure of a replace
    operation.

Arguments:

    TargetPdo - Supplies a reference device object of the target partition
        unit.

    SparePdo - Supplies a reference device object of the spare partition unit.

    Status - The status code indicating the reason for failure.

Return Value:

    None.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(TargetPdo);
    UNREFERENCED_PARAMETER(SparePdo);
    UNREFERENCED_PARAMETER(Status);

    NT_ASSERT(!"PnprLogFailureEvent should never be called\n");

#else

    const WCHAR NullCharacter = UNICODE_NULL;
    EVENT_DATA_DESCRIPTOR EventData[7];
    PNPR_EXTENDED_STATUS ExtendedStatus;
    PUNICODE_STRING TargetPath;
    PUNICODE_STRING SparePath;
    ULONG FailureLine;

    PAGED_CODE();

    //
    // Target module.
    //

    TargetPath = PnpGetDeviceInstancePath(PP_DO_TO_DN(TargetPdo));
    EventDataDescCreate(&EventData[0], TargetPath->Buffer, TargetPath->Length);
    EventDataDescCreate(&EventData[1], &NullCharacter, sizeof(WCHAR));

    //
    // Spare module.
    //

    SparePath = PnpGetDeviceInstancePath(PP_DO_TO_DN(SparePdo));
    EventDataDescCreate(&EventData[2], SparePath->Buffer, SparePath->Length);
    EventDataDescCreate(&EventData[3], &NullCharacter, sizeof(WCHAR));

    //
    // Failure information
    //

    if (PnprContext == NULL) {
        FailureLine = 0;
        ExtendedStatus = GeneralFailure;

    } else {
        FailureLine = PnprContext->FailureLine;
        ExtendedStatus = PnprContext->ExtendedStatus;
    }

    EventDataDescCreate(&EventData[4], &Status, sizeof(NTSTATUS));
    EventDataDescCreate(&EventData[5], &FailureLine, sizeof(ULONG));
    EventDataDescCreate(&EventData[6], &ExtendedStatus, sizeof(ULONG));

    //
    // Write the event.
    //

    EtwWrite(PnpEtwHandle,
             &PNP_EVT_DP_REPLACE_FAILURE,
             NULL,
             RTL_NUMBER_OF(EventData),
             EventData);

#endif

    return;
}

VOID
PnprLogStartEvent (
    __in PDEVICE_OBJECT TargetPdo,
    __in PDEVICE_OBJECT SparePdo
    )

/*++

Routine Description:

    This routine will log an event indicating the start of a replace operation.

Arguments:

    TargetPdo - Supplies a reference device object of the target partition
        unit.

    SparePdo - Supplies a reference device object of the spare partition unit.

Return Value:

    None.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(TargetPdo);
    UNREFERENCED_PARAMETER(SparePdo);

    NT_ASSERT(!"PnprLogStartEvent should never be called\n");

#else

    const WCHAR NullCharacter = UNICODE_NULL;
    EVENT_DATA_DESCRIPTOR EventData[4];
    PUNICODE_STRING TargetPath;
    PUNICODE_STRING SparePath;

    PAGED_CODE();

    //
    // Target module.
    //

    TargetPath = PnpGetDeviceInstancePath(PP_DO_TO_DN(TargetPdo));
    EventDataDescCreate(&EventData[0], TargetPath->Buffer, TargetPath->Length);
    EventDataDescCreate(&EventData[1], &NullCharacter, sizeof(WCHAR));

    //
    // Spare module.
    //

    SparePath = PnpGetDeviceInstancePath(PP_DO_TO_DN(SparePdo));
    EventDataDescCreate(&EventData[2], SparePath->Buffer, SparePath->Length);
    EventDataDescCreate(&EventData[3], &NullCharacter, sizeof(WCHAR));

    //
    // Write the event.
    //

    EtwWrite(PnpEtwHandle,
             &PNP_EVT_DP_REPLACE_START,
             NULL,
             RTL_NUMBER_OF(EventData),
             EventData);

#endif

    return;
}

VOID
PnprLogSuccessEvent (
    VOID
    )

/*++

Routine Description:

    This routine will log a success event to the event log.

Arguments:

    None.

Return Value:

    None.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprLogSuccessEvent should never be called\n");

#else

    const WCHAR NullCharacter = UNICODE_NULL;
    ULONG64 AffinityValue;
    ULONG DataSize;
    ULONG Index;
    EVENT_DATA_DESCRIPTOR EventData[19];
    PUNICODE_STRING SparePath;
    PUNICODE_STRING TargetPath;
    ULONG TimeQuiesced;
    ULONG TimeToQuiesce;
    ULONG TimeTotal;
    ULONG TimeToWake;

    PAGED_CODE();

    if (EventData == NULL) {
        return;
    }

    TimeTotal = PnprContext->Timing.ReplaceEnd;
    TimeToQuiesce = PnprContext->Timing.DeviceQuiesceEnd -
        PnprContext->Timing.DeviceQuiesceStart;

    TimeQuiesced = PnprContext->Timing.DeviceWakeStart - PnprContext->Timing.DeviceQuiesceEnd;
    TimeToWake = PnprContext->Timing.DeviceWakeEnd - PnprContext->Timing.DeviceWakeStart;

    //
    // 64P-TODO: Log full group affinity.
    //

    AffinityValue = (ULONG64)PnprContext->Target.Processors->Affinity[0];

    //
    // What follows are the event data descriptors for the target partition
    // unit portion of the event payload.
    //

    TargetPath = PnpGetDeviceInstancePath(PP_DO_TO_DN(PnprContext->Target.Pdo));
    EventDataDescCreate(&EventData[0], TargetPath->Buffer, TargetPath->Length);
    EventDataDescCreate(&EventData[1], &NullCharacter, sizeof(WCHAR));
    EventDataDescCreate(&EventData[2], &AffinityValue, sizeof(ULONG64));
    EventDataDescCreate(&EventData[3],
                        &PnprContext->Target.Processors->Count,
                        sizeof(ULONG));

    EventDataDescCreate(&EventData[4],
                        &PnprContext->Target.Memory->Count,
                        sizeof(ULONG));

    EventDataDescCreate(&EventData[5],
                        &PnprContext->Target.Memory->TotalLength,
                        sizeof(ULONGLONG));

    //
    // What follows are the event data descriptors for the spare partition unit
    // portion of the event payload.
    //

    SparePath = PnpGetDeviceInstancePath(PP_DO_TO_DN(PnprContext->Spare.Pdo));
    EventDataDescCreate(&EventData[6], SparePath->Buffer, SparePath->Length);
    EventDataDescCreate(&EventData[7], &NullCharacter, sizeof(WCHAR));
    EventDataDescCreate(&EventData[8],
                        &PnprContext->Spare.Processors->Count,
                        sizeof(ULONG));

    EventDataDescCreate(&EventData[9],
                        &PnprContext->Spare.Memory->Count,
                        sizeof(ULONG));

    EventDataDescCreate(&EventData[10],
                        &PnprContext->Spare.Memory->TotalLength,
                        sizeof(ULONGLONG));

    //
    // What follows are the event data descriptors for the timing information
    // portion of the event payload.
    //

    EventDataDescCreate(&EventData[11], &TimeTotal, sizeof(ULONG));
    EventDataDescCreate(&EventData[12], &TimeToQuiesce, sizeof(ULONG));
    EventDataDescCreate(&EventData[13], &TimeQuiesced, sizeof(ULONG));
    EventDataDescCreate(&EventData[14], &TimeToWake, sizeof(ULONG));
    Index = 15;

    if (PnprContext->Target.Processors->Count > 0) {
        DataSize = sizeof(PnprContext->Target.Processors->ApicIds[0]) *
            PnprContext->Target.Processors->Count;

        EventDataDescCreate(&EventData[Index],
                            &PnprContext->Target.Processors->ApicIds[0],
                            DataSize);

        Index += 1;
    }

    if (PnprContext->Target.Memory->Count > 0) {
        DataSize = sizeof(PnprContext->Target.Memory->Ranges[0]) *
            PnprContext->Target.Memory->Count;

        EventDataDescCreate(&EventData[Index],
                            &PnprContext->Target.Memory->Ranges[0],
                            DataSize);

        Index += 1;
    }

    if (PnprContext->Spare.Processors->Count > 0) {
        DataSize = sizeof(PnprContext->Spare.Processors->ApicIds[0]) *
            PnprContext->Spare.Processors->Count;

        EventDataDescCreate(&EventData[Index],
                            &PnprContext->Spare.Processors->ApicIds[0],
                            DataSize);

        Index += 1;
    }

    if (PnprContext->Spare.Memory->Count > 0) {
        DataSize = sizeof(PnprContext->Spare.Memory->Ranges[0]) *
            PnprContext->Spare.Memory->Count;

        EventDataDescCreate(&EventData[Index],
                            &PnprContext->Spare.Memory->Ranges[0],
                            DataSize);

        Index += 1;
    }

    PNP_ASSERT(Index <= RTL_NUMBER_OF(EventData));

    EtwWrite(PnpEtwHandle,
             &PNP_EVT_DP_REPLACE_SUCCESS,
             NULL,
             Index,
             EventData);

#endif

    return;
}

NTSTATUS
PnprRecopyMirrorPages (
    VOID
    )

/*++

Routine Description:

    This routine re-copies the pages that were modified during the memory
    mirroring process.  These pages were copied at an arbitrary point during the
    mirroring process and therefore may be out of date.

Arguments:

    None.

Return Value:

   STATUS_SUCCESS if the memory is re-copied successfully.

   Otherwise, an appropriate error status code.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprRecopyMirrorPages should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    ULONG CurrentProcessor;
    ULONG Index;
    ULONG Size;
    ULONG_PTR StackPointer;
    NTSTATUS Status;

    CurrentProcessor = KeGetCurrentProcessorIndex();

    PNP_ASSERT(CurrentProcessor == PnprContext->ControlProcessor);

    //
    // Get the local stack information.
    //

    PnprGetStackLimits(&PnprContext->CurrentStackPointer[CurrentProcessor],
                       &PnprContext->StackLimitPointer[CurrentProcessor]);

    //
    // Copy per-processor data, both internal to the kernel and those used by
    // this module.
    //

    for (Index = 0; Index < PnprContext->ProcessorCount; Index += 1) {

        //
        // First copy the stacks of each processor.
        //

        StackPointer = PnprContext->CurrentStackPointer[Index];
        Size = (ULONG)(PnprContext->StackLimitPointer[Index] -
                       StackPointer);

        Status = PnprRecopyAddress((PVOID)StackPointer, Size);
        if (!NT_SUCCESS(Status)) {
            PNPR_REPLACE_FAILURE(GeneralFailure);
            return Status;
        }

        //
        // Copy the per-processor mapping reserve.  Skip this step for the
        // mapping reserve of the control processor, since that will be used to
        // perform this copy, and will therefore change over the course of this
        // routine.
        //

        if (Index != PnprContext->ControlProcessor) {
            Status = PnprRecopyMappingReserve(
                         &PnprContext->SpareReserves[Index]);

            if (!NT_SUCCESS(Status)) {
                PNPR_REPLACE_FAILURE(GeneralFailure);
                return Status;
            }

            Status = PnprRecopyMappingReserve(
                         &PnprContext->TargetReserves[Index]);

            if (!NT_SUCCESS(Status)) {
                PNPR_REPLACE_FAILURE(GeneralFailure);
                return Status;
            }
        }
    }

    //
    // Copy the context structure used by this module.
    //

    Status = PnprRecopyAddress(PnprContext, sizeof(PNP_REPLACE_CONTEXT));
    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(GeneralFailure);
        return Status;
    }

    //
    // Finally, copy the control processor's reserved mapping.  This copy will
    // include the pages mapping the reserved mapping itself, so that all pages
    // will be in sync after this final copy.
    //

    Status = PnprCopyReservedMapping();
    if (!NT_SUCCESS(Status)) {
        PNPR_REPLACE_FAILURE(GeneralFailure);
        return Status;
    }

    return STATUS_SUCCESS;

#endif

}

NTSTATUS
PnprRecopyMappingReserve (
    __in PPNPR_MAPPING_RESERVE MapReserve
    )

/*++

Routine Description:

    This routine recopies the memory associated with a per-processor mapping
    reserve.

Arguments:

    MapReserve - Supplies the mapping reserve to recopy.

Return Value:

    STATUS_SUCCESS if the mapping reserve is copied successfully.

    An error status code if the copy fails.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(MapReserve);

    NT_ASSERT(!"PnprRecopyMappingReserve should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    LARGE_INTEGER ByteCount;
    ULONG Index;
    PFN_NUMBER LastPfn;
    PMDL Mdl;
    PFN_NUMBER PfnNumber;
    PHYSICAL_ADDRESS PhysicalAddress;
    PVOID Reserve;
    NTSTATUS Status;

    //
    // First copy each of the pages containing PTEs for the reserve.
    //

    LastPfn = 0;
    Index = 0;
    do {
        Reserve = Add2Ptr(MapReserve->Reserve, Index * PAGE_SIZE);
        PfnNumber = MmContainingPageForReservedMapping(Reserve);

        //
        // Most PTEs will be in the same page.  Only copy this PTE page if it is
        // different from the page containing the prior PTE.
        //

        if (PfnNumber != LastPfn) {
            LastPfn = PfnNumber;
            PhysicalAddress.QuadPart = PfnNumber << PAGE_SHIFT;
            ByteCount.QuadPart = PAGE_SIZE;
            Status = PnprMarkOrMirrorPages(PhysicalAddress, ByteCount, TRUE);
            if (!NT_SUCCESS(Status)) {
                PNPR_REPLACE_FAILURE(GeneralFailure);
                return Status;
            }
        }

        Index += 1;

    } while (Index < PNPR_MAXIMUM_PAGE_RUN);

    //
    // Additionally, copy the MDL describing the mapped pages.
    //

    Mdl = MapReserve->Mdl;
    PnprRecopyAddress(Mdl, sizeof(MDL));
    return STATUS_SUCCESS;

#endif

}

NTSTATUS
PnprCopyReservedMapping (
    VOID
    )

/*++

Routine Description:

    This routine makes a final copy of the reserved mapping used to recopy the
    rest of the pages used during the mirror.  It needs to self-map all pages
    modified by the act of making the mapping, including the PTEs and MDLs used
    to construct this final mapping.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS if the reserved mapping is copied successfully.

    STATUS_INSUFFICIENT_RESOURCES if the mapping of these reserved pages fails.

--*/

{

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(!"PnprCopyReservedMapping should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    PVOID CurrentAddress;
    PFN_NUMBER Destination[PNPR_MAXIMUM_PAGE_RUN];
    PVOID DestinationAddress;
    PHYSICAL_ADDRESS DestinationPhysical;
    ULONG Index;
    PVOID LastAddress;
    PPNPR_MAPPING_RESERVE MapReserve;
    PPFN_NUMBER PfnArray;
    PFN_NUMBER PfnNumber;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    PFN_NUMBER Source[PNPR_MAXIMUM_PAGE_RUN];
    PVOID SourceAddress;
    PPNPR_MAPPING_RESERVE TargetReserve;

    //
    // Use the spare mapping reserve for this processor.  Spare reserve MDLs are
    // configured to allow I/O space mappings, which allows both target memory
    // and spare memory (which is not known to the memory manager) to be
    // described.
    //

    MapReserve = &PnprContext->SpareReserves[PnprContext->ControlProcessor];
    if (MapReserve->Mdl->MdlFlags & MDL_MAPPED_TO_SYSTEM_VA) {
        MmUnmapReservedMapping(MapReserve->Reserve,
                               PNP_POOLTAG_PARTITION_RESOURCE_LIST,
                               MapReserve->Mdl);
    }

    RtlZeroMemory(Source, PNPR_MAXIMUM_PAGE_RUN * sizeof(PFN_NUMBER));
    RtlZeroMemory(Destination, PNPR_MAXIMUM_PAGE_RUN * sizeof(PFN_NUMBER));

    //
    // Get the physical address of the reserve MDL, and add it to the list of
    // pages to be copied.
    //

    Index = 0;
    PhysicalAddress = MmGetPhysicalAddress(PAGE_ALIGN(MapReserve->Mdl));
    Status = PnprGetPageDestination(PhysicalAddress, &DestinationPhysical);
    if (NT_SUCCESS(Status) &&
        (DestinationPhysical.QuadPart != PNP_REPLACE_NO_MAP)) {

        Source[Index] = (PFN_NUMBER)((ULONG64)PhysicalAddress.QuadPart >> PAGE_SHIFT);
        Destination[Index] =
            (PFN_NUMBER)((ULONG64)DestinationPhysical.QuadPart >> PAGE_SHIFT);

        Index += 1;
    }

    //
    // Get the physical address of each of the PTEs used to map the reserve.
    // These PTE pages will be affected by this final mapping, and need to be
    // copied themselves.
    //

    CurrentAddress = MapReserve->Reserve;
    LastAddress = Add2Ptr(MapReserve->Reserve,
                          PNPR_MAXIMUM_PAGE_RUN << PAGE_SHIFT);

    while (CurrentAddress < LastAddress) {
        PfnNumber = MmContainingPageForReservedMapping(CurrentAddress);
        if ((Index == 0) || (PfnNumber != Source[Index - 1])) {
            PhysicalAddress.QuadPart = PfnNumber << PAGE_SHIFT;
            Status = PnprGetPageDestination(PhysicalAddress,
                                            &DestinationPhysical);

            if (NT_SUCCESS(Status) &&
                (DestinationPhysical.QuadPart != PNP_REPLACE_NO_MAP)) {

                PNP_ASSERT(Index < PNPR_MAXIMUM_PAGE_RUN);

                if (Index >= PNPR_MAXIMUM_PAGE_RUN) {
                    return STATUS_INVALID_PARAMETER;
                }

                Source[Index] =
                    (PFN_NUMBER)((ULONG64)PhysicalAddress.QuadPart >> PAGE_SHIFT);

                Destination[Index] =
                    (PFN_NUMBER)((ULONG64)DestinationPhysical.QuadPart >> PAGE_SHIFT);

                Index += 1;
            }
        }

        CurrentAddress = Add2Ptr(CurrentAddress, PAGE_SIZE);
    }

    //
    // Finally, add the PTEs and MDL used by the control processor's target
    // reserve, since this was used for all of the page re-copies up until this
    // one.
    //

    TargetReserve = &PnprContext->TargetReserves[PnprContext->ControlProcessor];
    PhysicalAddress = MmGetPhysicalAddress(PAGE_ALIGN(TargetReserve->Mdl));
    Status = PnprGetPageDestination(PhysicalAddress, &DestinationPhysical);
    if (NT_SUCCESS(Status) &&
        (DestinationPhysical.QuadPart != PNP_REPLACE_NO_MAP)) {

        PNP_ASSERT(Index < PNPR_MAXIMUM_PAGE_RUN);

        if (Index >= PNPR_MAXIMUM_PAGE_RUN) {
            return STATUS_INVALID_PARAMETER;
        }

        Source[Index] = (PFN_NUMBER)((ULONG64)PhysicalAddress.QuadPart >> PAGE_SHIFT);
        Destination[Index] =
            (PFN_NUMBER)((ULONG64)DestinationPhysical.QuadPart >> PAGE_SHIFT);

        Index += 1;
    }

    CurrentAddress = TargetReserve->Reserve;
    LastAddress = Add2Ptr(TargetReserve->Reserve,
                          PNPR_MAXIMUM_PAGE_RUN << PAGE_SHIFT);

    while (CurrentAddress < LastAddress) {
        PfnNumber = MmContainingPageForReservedMapping(CurrentAddress);

        //
        // The check here allows the Index to be one off the end of the array,
        // since the first access is going to be Index - 1. The check tightens
        // up below if the current index is actually written to.
        //

        PNP_ASSERT(Index <= PNPR_MAXIMUM_PAGE_RUN);

        if (Index > PNPR_MAXIMUM_PAGE_RUN) {
            return STATUS_INVALID_PARAMETER;
        }

        if ((Index == 0) || (PfnNumber != Source[Index - 1])) {
            PhysicalAddress.QuadPart = PfnNumber << PAGE_SHIFT;
            Status = PnprGetPageDestination(PhysicalAddress,
                                            &DestinationPhysical);

            if (NT_SUCCESS(Status) &&
                (DestinationPhysical.QuadPart != PNP_REPLACE_NO_MAP)) {

                PNP_ASSERT(Index < PNPR_MAXIMUM_PAGE_RUN);

                if (Index >= PNPR_MAXIMUM_PAGE_RUN) {
                    return STATUS_INVALID_PARAMETER;
                }

                Source[Index] =
                    (PFN_NUMBER)((ULONG64)PhysicalAddress.QuadPart >> PAGE_SHIFT);

                Destination[Index] =
                    (PFN_NUMBER)((ULONG64)DestinationPhysical.QuadPart >> PAGE_SHIFT);

                Index += 1;
            }
        }

        CurrentAddress = Add2Ptr(CurrentAddress, PAGE_SIZE);
    }

    //
    // If none of these pages fall in the target partition unit, no copy is
    // necessary.
    //

    if (Index == 0) {
        return STATUS_SUCCESS;
    }

    //
    // Initialize the MDL with both the source and destination pages.
    //

    NT_ASSERT((Index * 2) <= PNPR_MAXIMUM_PAGE_RUN);

    MmInitializeMdl(MapReserve->Mdl, 0, (Index * 2) << PAGE_SHIFT);
    PfnArray = MmGetMdlPfnArray(MapReserve->Mdl);
    RtlCopyMemory(PfnArray, Source, Index * sizeof(PFN_NUMBER));
    RtlCopyMemory(Add2Ptr(PfnArray, Index * sizeof(PFN_NUMBER)),
                  Destination,
                  Index * sizeof(PFN_NUMBER));

    MapReserve->Mdl->MdlFlags |= MapReserve->MdlFlags;

    //
    // Map these pages.  This will put the source pages at the beginning of the
    // mapped address, followed by the destination pages.
    //

    SourceAddress = MmMapLockedPagesWithReservedMapping(
                        MapReserve->Reserve,
                        PNP_POOLTAG_PARTITION_RESOURCE_LIST,
                        MapReserve->Mdl,
                        MmCached);

    if (SourceAddress == NULL) {
        PNPR_REPLACE_FAILURE(AllocationFailure);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    DestinationAddress = Add2Ptr(SourceAddress, Index << PAGE_SHIFT);

    //
    // Perform a simple memory copy of these pages.
    //

    RtlCopyMemory(DestinationAddress, SourceAddress, Index << PAGE_SHIFT);
    return STATUS_SUCCESS;

#endif

}

NTSTATUS
PnprRecopyAddress (
    __in PVOID Address,
    __in ULONG Size
    )

/*++

Routine Description:

    This routine copies a virtual buffer from the target partition unit to the
    spare.  This allows a buffer used during the memory copy to be re-copied
    after the larger mirror is complete.

Arguments:

    Address - Supplies the virtual address of the buffer to copy.

    Size - Supplies the size of the buffer.

Return Value:

    STATUS_SUCCESS if the buffer is re-copied successfully, or if the buffer is
        not in the target partition unit.

    An error status if the copy fails.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(Address);
    UNREFERENCED_PARAMETER(Size);

    NT_ASSERT(!"PnprRecopyAddress should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    LARGE_INTEGER ByteCount;
    ULONG_PTR CurrentAddress;
    ULONG_PTR LastAddress;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;

    NT_ASSERT(Size != 0);

    CurrentAddress = (ULONG_PTR)PAGE_ALIGN(Address);
    LastAddress = (ULONG_PTR)Add2Ptr(Address, Size);
    while (CurrentAddress < LastAddress) {

        //
        // Get the physical address of each page in the buffer, and copy it.
        //

        PhysicalAddress = MmGetPhysicalAddress((PVOID)CurrentAddress);
        ByteCount.QuadPart = PAGE_SIZE;
        Status = PnprMarkOrMirrorPages(PhysicalAddress, ByteCount, TRUE);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        CurrentAddress += PAGE_SIZE;
    }

    return STATUS_SUCCESS;

#endif

}

NTSTATUS
PnprGetPageDestination (
    __in PHYSICAL_ADDRESS SourceAddress,
    __out PPHYSICAL_ADDRESS DestinationAddress
    )

/*++

Routine Description:

    This routine gets the destination physical address for a page being
    replaced.

Arguments:

    SourceAddress - Supplies the physical address for the page being replaced.

    DestinationAddress - Supplies a pointer to a variable that receives the
        destination physical address for the page.

Return Value:

    STATUS_SUCCESS if the page destination was retrieved successfully.

    STATUS_NOT_FOUND if the page is not in the target partition unit.

--*/

{

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(SourceAddress);
    UNREFERENCED_PARAMETER(DestinationAddress);

    NT_ASSERT(!"PnprGetPageDestination should not be called\n");

    return STATUS_NOT_SUPPORTED;

#else

    PREPLACE_GET_MEMORY_DESTINATION GetMemoryDestination;
    ULONG Index;
    PPNP_REPLACE_MEMORY_LIST MemoryList;
    ULONGLONG RangeAddress;
    ULONGLONG RangeLength;
    ULONGLONG Source;
    NTSTATUS Status;

    GetMemoryDestination = PnprContext->DriverInterface.GetMemoryDestination;

    NT_ASSERT(GetMemoryDestination != NULL);

    //
    // Walk the target memory list to determine whether the page is in the
    // target partition unit.
    //

    Source = (ULONGLONG)SourceAddress.QuadPart;
    MemoryList = PnprContext->Target.Memory;
    for (Index = 0; Index < MemoryList->Count; Index += 1) {
        RangeAddress = MemoryList->Ranges[Index].Address.QuadPart;
        RangeLength = MemoryList->Ranges[Index].Length;
        if ((Source >= RangeAddress) &&
            (Source <= (RangeAddress + RangeLength - 1))) {

            //
            // The page is in the target.  Call the plugin driver to get the
            // page destination.
            //

            Status = GetMemoryDestination(PnprContext->DriverContext,
                                          SourceAddress,
                                          DestinationAddress);

            NT_ASSERT(NT_SUCCESS(Status));

            return Status;
        }
    }

    return STATUS_NOT_FOUND;

#endif

}

VOID
PnprGetStackLimits (
    __out PULONG_PTR CurrentPointer,
    __out PULONG_PTR LimitPointer
    )

/*++

Routine Description:

    This routine returns the stack limits of the currently executing processor
    context.

Arguments:

    CurrentPointer - Supplies a pointer to a variable that receives the current
        stack pointer at the time the pointer is captured.

    LimitPointer - Supplies a pointer to a variable that receives the top of the
        currently executing stack.

Return Value:

    None.

--*/

{

    ULONG_PTR Dummy;

#if defined(_IA64_)

    ULONG64 BackingStore;

#else

    KERNEL_STACK_LIMITS StackType;

#endif

    //
    // On IA64, include the register stack in the captured limits.  The register
    // stack grows upwards from the stack limit, so flush the RSE and capture
    // the current register stack pointer as the total stack limit.
    //
    // On other platforms, query the kernel for the stack limit.
    //

#if defined(_IA64_)

    RtlpFlushRSE(&BackingStore, &Dummy);
    *CurrentPointer = KeGetCurrentStackPointer();
    *LimitPointer = BackingStore;

#else

    *CurrentPointer = KeGetCurrentStackPointer();
    KeQueryCurrentStackInformation(&StackType, &Dummy, LimitPointer);

    NT_ASSERT((StackType == NormalStackLimits) || (StackType == DPCStackLimits));

#endif

    return;
}
