/*++
Copyright (c) Microsoft Corporation

Module Name:

    interrupter.c

Abstract:

    USB XHCI Driver

--*/

#include "pch.h"

#include "interrupter.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, Interrupter_Create)
#pragma alloc_text(PAGE, Interrupter_CreateInterrupter)
#pragma alloc_text(PAGE, Interrupter_PrepareHardware)
#pragma alloc_text(PAGE, Interrupter_DetermineSegmentSizeAndCount)
#pragma alloc_text(PAGE, Interrupter_PrepareInterrupter)
#pragma alloc_text(PAGE, Interrupter_ReleaseHardware)
#pragma alloc_text(PAGE, Interrupter_ReleaseInterrupter)
#pragma alloc_text(PAGE, Interrupter_DetermineSecondaryInterrupterCount)
#pragma alloc_text(PAGE, Interrupter_FilterRemoveResourceRequirements)
#pragma alloc_text(PAGE, Interrupter_FilterAddResourceRequirements)
#pragma alloc_text(PAGE, Interrupter_FilterAllMSIResources)
#pragma alloc_text(PAGE, Interrupter_FilterMessageDescriptors)
#pragma alloc_text(PAGE, Interrupter_AffinitizeMSIXDescriptors)
#pragma alloc_text(PAGE, Interrupter_PopulateInterrupterLookupTable)
#pragma alloc_text(PAGE, Interrupter_IoResList_GetMessageCountAndType)
#pragma alloc_text(PAGE, Interrupter_CmResList_GetInterrupterCountAndType)
#endif

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_Create(
    __in
        WDFDEVICE   WdfDevice,
    __in
        HANDLE      ControllerHandle,
    __inout
        PHANDLE     ObjectHandle
    )
/*++

Routine Description:

    This function creates an interrupter object and initializes the data
    associated with the interrupter.

--*/
{
    PINTERRUPTER_DATA           interrupterData;
    PPRIMARY_INTERRUPTER_DATA   primaryInterrupterData;
    NTSTATUS                    status;

    PAGED_CODE();

    TRY {

        //
        // Create the WDF interrupt object for the primary interrupter.
        //
        status = Interrupter_CreateInterrupter(WdfDevice,
                                               ControllerHandle,
                                               0,
                                               NULL,
                                               NULL,
                                               &interrupterData,
                                               &primaryInterrupterData);

        if (!NT_SUCCESS(status)) {

            LogError(Controller_GetRecorderLog(ControllerHandle), Interrupter,
                     "Interrupter_CreateInterrupter Failed %!STATUS!", status);
            LEAVE;
        }

        primaryInterrupterData->PrimaryInterrupter = interrupterData;

        //
        // Return primary interrupter data as the handle to the interrupter object.
        //
        *ObjectHandle = (HANDLE)primaryInterrupterData;

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_CreateInterrupter(
    __in
        WDFDEVICE                       WdfDevice,
    __in
        HANDLE                          ControllerHandle,
    __in
        ULONG                           MessageNumber,
    __in_opt
        PCM_PARTIAL_RESOURCE_DESCRIPTOR InterruptRaw,
    __in_opt
        PCM_PARTIAL_RESOURCE_DESCRIPTOR InterruptTranslated,
    __out
        PINTERRUPTER_DATA               *InterrupterData,
    __out_opt
        PPRIMARY_INTERRUPTER_DATA       *PrimaryInterrupterData
    )
/*++

Routine Description:

    This function creates the WDF interrupt object.

Arguments:

    WdfDevice - Handle to Wdf device object.

    ControllerHandle - Handle to Controller object.

    MessageNumber - MSI message number associated with this interrupter.

    InterruptRaw - Pointer to the raw interrupt resource descriptor corresponding to this
        interrupter. N.B: This descriptor must belong to the WDFCMRESLIST object supplied
        by the framework in the EvtDeviceRemoveAddedResources callback.

    InterruptTranslated - Pointer to the raw interrupt resource descriptor corresponding
        to this interrupter. N.B: This descriptor must belong to the WDFCMRESLIST object
        supplied by the framework in the EvtDeviceRemoveAddedResources callback.

    InterrupterData - Receives a pointer to the newly created interrupt object.

    PrimaryInterrupterData - Optionally receives a pointer to the primary interrupter data
        object if the interrupter created is the primary interrupter.

--*/
{
    NTSTATUS                    status;
    WDF_INTERRUPT_CONFIG        wdfInterruptConfig;
    WDF_OBJECT_ATTRIBUTES       wdfAttributes;
    WDFINTERRUPT                wdfInterrupt;
    PINTERRUPTER_DATA           interrupterData;
    PPRIMARY_INTERRUPTER_DATA   primaryInterrupterData;

    PAGED_CODE();

    TRY {

        //
        // InterruptRaw and InterruptTranslated are required if WdfInterruptCreate is
        // called from PrepareHardware callback.
        //
        WDF_INTERRUPT_CONFIG_INIT(&wdfInterruptConfig,
                                  Interrupter_WdfEvtInterruptIsr,
                                  Interrupter_WdfEvtInterruptDpc);

        wdfInterruptConfig.EvtInterruptEnable   = Interrupter_WdfEvtInterruptEnable;
        wdfInterruptConfig.EvtInterruptDisable  = Interrupter_WdfEvtInterruptDisable;
        wdfInterruptConfig.InterruptRaw         = InterruptRaw;
        wdfInterruptConfig.InterruptTranslated  = InterruptTranslated;

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, INTERRUPTER_DATA);
        wdfAttributes.EvtCleanupCallback = Interrupter_WdfEvtCleanupCallback;

        status = WdfInterruptCreate(WdfDevice,
                                    &wdfInterruptConfig,
                                    &wdfAttributes,
                                    &wdfInterrupt);

        if (!NT_SUCCESS(status)) {

            LogError(Controller_GetRecorderLog(ControllerHandle), Interrupter,
                     "WdfInterruptCreate Failed %!STATUS!", status);
            LEAVE;
        }

        LogInfo(Controller_GetRecorderLog(ControllerHandle), Interrupter,
                "wdfInterrupt 0x%p MessageNumber %d", wdfInterrupt, MessageNumber);

        interrupterData = GetInterrupterData(wdfInterrupt);

        interrupterData->WdfInterrupt       = wdfInterrupt;
        interrupterData->ControllerHandle   = ControllerHandle;
        interrupterData->MessageNumber      = MessageNumber;

        if (InterruptTranslated) {
            interrupterData->Affinity       = InterruptTranslated->u.MessageInterrupt.Raw.Affinity;
#if defined(NT_PROCESSOR_GROUPS)
            interrupterData->Group          = InterruptTranslated->u.MessageInterrupt.Raw.Group;
#else
            interrupterData->Group          = 0;
#endif
        } else {
            interrupterData->Affinity       = 0;
            interrupterData->Group          = 0;
        }

        KeInitializeSpinLock(&interrupterData->EventRingLock);

        Counter_CreateInterrupterInstance(interrupterData);

        *InterrupterData = interrupterData;

        //
        // For the primary interrupter, create an additional data to store info
        // tracked with the primary interrupter.
        //
        if (PrimaryInterrupterData) {

            WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, PRIMARY_INTERRUPTER_DATA);

            status = WdfObjectAllocateContext(wdfInterrupt,
                                              &wdfAttributes,
                                              &primaryInterrupterData);

            if (!NT_SUCCESS(status)) {

                LogError(Controller_GetRecorderLog(ControllerHandle), Interrupter,
                         "WdfObjectAllocateContext Failed %!STATUS!", status);
                LEAVE;
            }

            primaryInterrupterData->WdfDevice        = WdfDevice;
            primaryInterrupterData->ControllerHandle = ControllerHandle;

            *PrimaryInterrupterData = primaryInterrupterData;
        }

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_PrepareHardware(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDFCMRESLIST                WdfResourcesRaw,
    __in
        WDFCMRESLIST                WdfResourcesTranslated
    )
/*++

Routine Description:

    This function notifies the interrupters of a WDF PrepareHardware event.

--*/
{
    HANDLE                          controllerHandle;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor;
    PINTERRUPTER_DATA               interrupterData;
    ULONG                           interrupterDataArraySize;
    ULONG                           assignedInterruptCount;
    ULONG                           messageNumber;
    ULONG                           resourceListIndex;
    NTSTATUS                        status;

    PAGED_CODE();

    TRY {

        controllerHandle = PrimaryInterrupterData->ControllerHandle;

        Interrupter_CmResList_GetInterrupterCountAndType(PrimaryInterrupterData,
                                                         WdfResourcesRaw,
                                                         &assignedInterruptCount,
                                                         &PrimaryInterrupterData->Mechanism);

        if (assignedInterruptCount == 0) {

            status = STATUS_INSUFFICIENT_RESOURCES;
            LogError(Controller_GetRecorderLog(controllerHandle), Interrupter,
                     "No Interrupt resources provided");
            LEAVE;
        }

        //
        // Allocate a buffer to store pointers to all of the interrupters in the primary
        // interrupter data. If the buffer was allocated in a previous PrepareHardware, it
        // must be freed in the corresponding ReleaseHardware.
        //
        NT_ASSERT(PrimaryInterrupterData->InterrupterDataArray == NULL);

        interrupterDataArraySize = sizeof(PINTERRUPTER_DATA) * assignedInterruptCount;

        PrimaryInterrupterData->InterrupterDataArray =
            ExAllocatePoolWithTag(NonPagedPool, interrupterDataArraySize, DriverAllocationTag);

        if (PrimaryInterrupterData->InterrupterDataArray == NULL) {

            status = STATUS_INSUFFICIENT_RESOURCES;
            LogError(Controller_GetRecorderLog(controllerHandle), Interrupter,
                     "Failed to allocate InterrupterDataArray");
            LEAVE;
        }

        RtlZeroMemory(PrimaryInterrupterData->InterrupterDataArray, interrupterDataArraySize);

        //
        // The Primary interrupter was created in AddDevice, prepare it here.
        //
        status =
            Interrupter_PrepareInterrupter(PrimaryInterrupterData->PrimaryInterrupter,
                                           (PrimaryInterrupterData->Mechanism != InterruptMechanism_LineBased));

        if (!NT_SUCCESS(status)) {

            LogError(Controller_GetRecorderLog(controllerHandle), Interrupter,
                     "Interrupter_PrepareInterrupter 0 Failed %!STATUS!", status);
            LEAVE;
        }

        Interrupter_SetInterrupterCount(PrimaryInterrupterData, 1);

        PrimaryInterrupterData->InterrupterDataArray[0] = PrimaryInterrupterData->PrimaryInterrupter;

        //
        // Single interrupter policy has to be forced for all 0.96 controllers.
        // For 0.96 controllers that we do not know about, this flag gets set
        // in PrepareHardware. But the secondary interrupter count was determined
        // in filter resource requirements, and may be non-zero. For that case,
        // override the secondary interrupter count to zero here.
        //
        if (Controller_GetDeviceFlags(controllerHandle).UseSingleInterrupter) {

            Interrupter_SetSecondaryInterrupterCount(PrimaryInterrupterData, 0);
        }

        //
        // Create the secondary interrupter objects and prepare them. Secondary
        // interrupters are only created for MSI-X.
        //
        // N.B: Secondary interrupter count > 0 does not imply message count > 1.
        //      Secondary interrupter count was determined in filter resource requirements
        //      based on tentative resource assignments. The message count here is based
        //      on the actual resources assigned.
        //
        if ((Interrupter_GetSecondaryInterrupterCount(PrimaryInterrupterData) > 0) &&
            (assignedInterruptCount > 1) &&
            (PrimaryInterrupterData->Mechanism == InterruptMechanism_MSIX)) {

            NT_ASSERT(Interrupter_GetSecondaryInterrupterCount(PrimaryInterrupterData) <=
                      (assignedInterruptCount - 1));

            messageNumber = 0;

            for (resourceListIndex = 0;
                 resourceListIndex < WdfCmResourceListGetCount(WdfResourcesTranslated);
                 resourceListIndex += 1) {

                descriptor = WdfCmResourceListGetDescriptor(WdfResourcesTranslated, resourceListIndex);

                if (IsCmResourceDescriptorMSI(descriptor) == FALSE) {
                    continue;
                }

                if (messageNumber == 0) {
                    messageNumber += 1;
                    continue;
                }

                status = Interrupter_CreateInterrupter(
                             PrimaryInterrupterData->WdfDevice,
                             controllerHandle,
                             messageNumber,
                             WdfCmResourceListGetDescriptor(WdfResourcesRaw, resourceListIndex),
                             WdfCmResourceListGetDescriptor(WdfResourcesTranslated, resourceListIndex),
                             &interrupterData,
                             NULL);

                if (!NT_SUCCESS(status)) {

                    LogError(Controller_GetRecorderLog(controllerHandle), Interrupter,
                             "Interrupter_CreateWdfInterrupt (%u) Failed %!STATUS!",
                             messageNumber, status);
                    LEAVE;
                }

                status = Interrupter_PrepareInterrupter(interrupterData, TRUE);

                if (!NT_SUCCESS(status)) {

                    LogError(Controller_GetRecorderLog(controllerHandle), Interrupter,
                             "Interrupter_PrepareInterrupter %u Failed %!STATUS!",
                             messageNumber,
                             status);
                    LEAVE;
                }

                PrimaryInterrupterData->InterrupterDataArray[messageNumber] = interrupterData;

                messageNumber += 1;
            }

            Interrupter_SetInterrupterCount(PrimaryInterrupterData,
                                            (Interrupter_GetInterrupterCount(PrimaryInterrupterData) +
                                             Interrupter_GetSecondaryInterrupterCount(PrimaryInterrupterData)));
        }

        //
        // Populate the processor to interrupter lookup table based on affinities.
        //
        status = Interrupter_PopulateInterrupterLookupTable(PrimaryInterrupterData);

        if (!NT_SUCCESS(status)) {

            LogError(Controller_GetRecorderLog(controllerHandle), Interrupter,
                     "Interrupter_PopulateInterrupterLookupTable Failed %!STATUS!",
                     status);
            LEAVE;
        }

    } FINALLY {

        NT_ASSERT(Interrupter_GetInterrupterCount(PrimaryInterrupterData) <=
                  Register_GetSupportedNumberOfInterrupters(
                        Controller_GetRegisterHandle(PrimaryInterrupterData->ControllerHandle)));

    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_DetermineSegmentSizeAndCount(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
/*++

Routine Description:

    Makes policy decision about the size and count of segments to allocate
    for the Event Ring, taking into account - hardware support, software
    defaults, and test hooks.

--*/
{
    ULONG               hardwareEventRingSegmentMax;
    HANDLE              registerHandle;
    ULONG               segmentCount;
    ULONG               softwareEventRingSegmentMax;
    PTEST_CONFIGURATION testConfiguration;

    PAGED_CODE();

    TRY {

        registerHandle    = Controller_GetRegisterHandle(InterrupterData->ControllerHandle);
        testConfiguration = Controller_GetTestConfiguration(InterrupterData->ControllerHandle);

        //
        // For controllers that need to be reset upon the event ring full error,
        // allow a higher number of segments to avoid the event ring full condition.
        //
        if (Controller_GetDeviceFlags(InterrupterData->ControllerHandle).EnableErrorRecoveryOnEventRingFull) {
            softwareEventRingSegmentMax = EVENT_RING_SEGMENT_COUNT_TO_AVOID_FULL;
        } else {
            softwareEventRingSegmentMax = EVENT_RING_SEGMENT_COUNT_DEFAULT;
        }

        hardwareEventRingSegmentMax = Register_GetSupportedEventRingSegmentCount(registerHandle);

        segmentCount = Min(hardwareEventRingSegmentMax, softwareEventRingSegmentMax);

        if ((testConfiguration->EventRingSegmentCount != 0) &&
            (testConfiguration->EventRingSegmentCount != ULONG_MAX)) {

            if (testConfiguration->EventRingSegmentCount < segmentCount) {
                segmentCount = testConfiguration->EventRingSegmentCount;
            } else {
                segmentCount = Min(hardwareEventRingSegmentMax, testConfiguration->EventRingSegmentCount);
            }
        }

        if ((segmentCount > 2) &&
            Controller_GetDeviceFlags(InterrupterData->ControllerHandle).MaxEventRingSegmentCount2) {

            segmentCount = 2;
        }

        //
        // UsedEventRingSegments variable is unused. It is kept around to keep the
        // debugger extension working.
        //
        InterrupterData->SegmentCount          = segmentCount;
        InterrupterData->UsedEventRingSegments = segmentCount;

        //
        // Since the CommonBuffer object only supports two sizes, test configuration
        // only has meaning if it specifies SMALL_SEGMENT_SIZE.
        //
        if (testConfiguration->EventRingSegmentSize == SMALL_SEGMENT_SIZE) {
            InterrupterData->SegmentSize = testConfiguration->EventRingSegmentSize;
        } else {
            InterrupterData->SegmentSize = LARGE_SEGMENT_SIZE;
        }

        InterrupterData->TRBsPerSegment = (InterrupterData->SegmentSize / sizeof(TRB));

    } FINALLY {

    }

    return;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_PrepareInterrupter(
    __in
        PINTERRUPTER_DATA   InterrupterData,
    __in
        BOOLEAN             IsMessageSignaled
    )
/*++

Routine Description:

    This function is called on a prepare hardware event and allocates any
    resources required by the interrupter.

--*/
{
    PBUFFER_DATA                bufferData;
    HANDLE                      commonBufferHandle;
    PEVENT_RING_SEGMENT_TABLE   eventRingSegmentTable;
    RECORDER_LOG_CREATE_PARAMS  recorderLogCreateParams;
    HANDLE                      registerHandle;
    ULONG                       segmentIndex;
    NTSTATUS                    status;

    PAGED_CODE();

    TRY {

        registerHandle     = Controller_GetRegisterHandle(InterrupterData->ControllerHandle);
        commonBufferHandle = Controller_GetCommonBufferHandle(InterrupterData->ControllerHandle);

        InitializeListHead(&InterrupterData->BufferDataList);

        InterrupterData->Flags.MessageSignaled = IsMessageSignaled;

        Interrupter_DetermineSegmentSizeAndCount(InterrupterData);

        //
        // Acquire the common buffer for the event ring segment table.
        //
        InterrupterData->EventRingTableBufferData =
            CommonBuffer_AcquireBuffer(commonBufferHandle,
                                       (InterrupterData->SegmentCount * sizeof(EVENT_RING_SEGMENT_TABLE)),
                                       InterrupterData,
                                       INTERRUPTER_ALLOC_TAG1);

        if (InterrupterData->EventRingTableBufferData == NULL) {

            LogError(Controller_GetRecorderLog(InterrupterData->ControllerHandle), Interrupter,
                     "CommonBuffer_AcquireBuffer returned NULL");
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        //
        // Acquire the common buffer for the event ring segments. Each ring segment is
        // of the same length and the number to acquire was previously determined.
        //
        status = CommonBuffer_AcquireBuffers(commonBufferHandle,
                                             InterrupterData->SegmentCount,
                                             InterrupterData->SegmentSize,
                                             &InterrupterData->BufferDataList,
                                             InterrupterData,
                                             INTERRUPTER_ALLOC_TAG2,
                                             NULL);

        if (!NT_SUCCESS(status)) {

            LogError(Controller_GetRecorderLog(InterrupterData->ControllerHandle), Interrupter,
                     "CommonBuffer_AcquireBuffers Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // Cache the pointer to this interrupter's registers on the xHCI controller.
        //
        InterrupterData->InterrupterRegisterSet =
            Register_GetInterrupterRegisterSet(registerHandle,
                                               InterrupterData->MessageNumber);

        //
        // Build the event ring. This includes setting the physical address for each event
        // ring segment in the event ring segment table and setting the registers on the xHCI
        // controller for processing the event ring.
        //
        Register_WriteUlong(
            &InterrupterData->InterrupterRegisterSet->EventRingSegmentTableSizeRegister.AsUlong32,
            0);

        //
        // Initialize the Event Ring Segment Table.
        //
        eventRingSegmentTable =
            (PEVENT_RING_SEGMENT_TABLE)InterrupterData->EventRingTableBufferData->VirtualAddress;

        NT_ASSERT((eventRingSegmentTable[0].BaseAddress & 0xF) == FALSE);

        segmentIndex = 0;
        FOR_ALL_IN_LIST(BUFFER_DATA, &InterrupterData->BufferDataList, ListEntry, bufferData) {
            eventRingSegmentTable[segmentIndex].BaseAddress = bufferData->LogicalAddress.QuadPart;
            eventRingSegmentTable[segmentIndex].SegmentSize = InterrupterData->TRBsPerSegment;
            segmentIndex += 1;
        }

        //
        // Allocate a work item that is used when the DPC watchdog timer is about
        // to expire. This is only allocated if IoTryQueueWorkItem is available. If
        // this function is unavailable, then the driver doesn't use the work item.
        //
        if (g_IoTryQueueWorkItem) {

            InterrupterData->IoWorkItem =
                IoAllocateWorkItem(Controller_GetWdmDeviceObject(InterrupterData->ControllerHandle));

            KeInitializeEvent(&InterrupterData->WorkItemEvent, NotificationEvent, TRUE);

        } else {

            InterrupterData->IoWorkItem = NULL;
        }

        RECORDER_LOG_CREATE_PARAMS_INIT(&recorderLogCreateParams, NULL);

#if defined (DBG)
        recorderLogCreateParams.TotalBufferSize = 8192;
        recorderLogCreateParams.ErrorPartitionSize = 200;
#else
        recorderLogCreateParams.TotalBufferSize = 1024;
        recorderLogCreateParams.ErrorPartitionSize = 200;
#endif

        RtlStringCchPrintfA(recorderLogCreateParams.LogIdentifier,
                            sizeof(recorderLogCreateParams.LogIdentifier),
                            "%02d INT%02d",
                            Controller_GetInstanceNumber(InterrupterData->ControllerHandle),
                            InterrupterData->MessageNumber);

        if (!NT_SUCCESS(WppRecorderLogCreate(&recorderLogCreateParams, &InterrupterData->RecorderLog))) {
            InterrupterData->RecorderLog = Controller_GetRecorderLog(InterrupterData->ControllerHandle);
        }

        LogInfo(Controller_GetRecorderLog(InterrupterData->ControllerHandle), Interrupter,
                "InterrupterRegisterSet 0x%p SegmentSize %u SegmentCount %u",
                InterrupterData->InterrupterRegisterSet,
                InterrupterData->SegmentSize,
                InterrupterData->SegmentCount);

        InterrupterData->State = InterrupterState_Prepared;

        status = STATUS_SUCCESS;

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (InterrupterData->EventRingTableBufferData != NULL) {

                CommonBuffer_ReleaseBuffer(commonBufferHandle,
                                           InterrupterData->EventRingTableBufferData);

                InterrupterData->EventRingTableBufferData = NULL;
            }

            if (IsListEmpty(&InterrupterData->BufferDataList) == FALSE) {

                CommonBuffer_ReleaseBuffers(commonBufferHandle, &InterrupterData->BufferDataList);
            }
        }
    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_ReleaseHardware(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData
    )
/*++

Routine Description:

    This function is called on a release hardware event and releases the
    interrupter and any associated resources.

--*/
{
    ULONG   i;

    PAGED_CODE();

    TRY {

        if (PrimaryInterrupterData->InterrupterDataArray != NULL) {

            for (i = 0; i < Interrupter_GetInterrupterCount(PrimaryInterrupterData); i += 1) {

                Interrupter_ReleaseInterrupter(PrimaryInterrupterData->InterrupterDataArray[i]);
            }

            ExFreePoolWithTag(PrimaryInterrupterData->InterrupterDataArray, DriverAllocationTag);
            PrimaryInterrupterData->InterrupterDataArray = NULL;
        }

        if (PrimaryInterrupterData->InterrupterLookup != NULL) {
            ExFreePoolWithTag(PrimaryInterrupterData->InterrupterLookup, DriverAllocationTag);
            PrimaryInterrupterData->InterrupterLookup = NULL;
        }

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_ReleaseInterrupter(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
/*++

Routine Description:

    This function is called on a release hardware event and frees any
    resources acquired by the interrupter.

--*/
{
    HANDLE  commonBufferHandle;

    PAGED_CODE();

    TRY {

        commonBufferHandle = Controller_GetCommonBufferHandle(InterrupterData->ControllerHandle);

        if (InterrupterData->State == InterrupterState_Prepared) {

            Register_WriteUlong(
                &InterrupterData->InterrupterRegisterSet->EventRingSegmentTableSizeRegister.AsUlong32,
                0);

            Register_WriteUlong64(
                &InterrupterData->InterrupterRegisterSet->EventRingSegmentTableBaseAddress.QuadPart,
                0,
                (Controller_GetDeviceFlags(InterrupterData->ControllerHandle).Use32BitRegisterAccess == 1));

            Register_WriteUlong64(
                &InterrupterData->InterrupterRegisterSet->EventRingDequeuePointerRegister.AsUlong64,
                0,
                (Controller_GetDeviceFlags(InterrupterData->ControllerHandle).Use32BitRegisterAccess == 1));

            if (InterrupterData->EventRingTableBufferData != NULL) {

                CommonBuffer_ReleaseBuffer(commonBufferHandle,
                                           InterrupterData->EventRingTableBufferData);

                InterrupterData->EventRingTableBufferData = NULL;
            }

            if (IsListEmpty(&InterrupterData->BufferDataList) == FALSE) {

                CommonBuffer_ReleaseBuffers(commonBufferHandle, &InterrupterData->BufferDataList);
            }

            if (InterrupterData->IoWorkItem != NULL) {
                IoFreeWorkItem(InterrupterData->IoWorkItem);
                InterrupterData->IoWorkItem = NULL;
            }

            if ((InterrupterData->RecorderLog != NULL) &&
                (InterrupterData->RecorderLog != Controller_GetRecorderLog(InterrupterData->ControllerHandle))) {
                WppRecorderLogDelete(InterrupterData->RecorderLog);
            }
            InterrupterData->RecorderLog = NULL;
        }

        InterrupterData->State = InterrupterState_Released;

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Interrupter_Initialize(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData
    )
/*++

Routine Description:

    This routine zero initialize the event ring segment buffers and interrupter
    state like current segment, cycle state, dequeue index etc.

    This routine is called during D0 entry and also during non-PnP reset
    recovery after the host controller has been reset.

    The host controller should be halted when this routine is called.

--*/
{
    PINTERRUPTER_DATA   interrupterData;
    PBUFFER_DATA        bufferData;
    ULONG               i;

    TRY {

        for (i = 0; i < Interrupter_GetInterrupterCount(PrimaryInterrupterData); i++) {

            interrupterData = PrimaryInterrupterData->InterrupterDataArray[i];

            //
            // Zero initialize the event ring segment buffers.
            //
            bufferData = (PBUFFER_DATA)interrupterData->BufferDataList.Flink;

            while (bufferData != (PBUFFER_DATA)&interrupterData->BufferDataList) {

                RtlZeroMemory(bufferData->VirtualAddress, bufferData->Size);

                bufferData = (PBUFFER_DATA)bufferData->ListEntry.Flink;
            }

            //
            // Set the current TRB to point to the first TRB in the first
            // event ring segment.  This value is used later to manage the
            // current location in the event ring.
            //
            interrupterData->CurrentBufferData =
                (PBUFFER_DATA)interrupterData->BufferDataList.Flink;

            interrupterData->CurrentRingSegment =
                (PTRB)interrupterData->CurrentBufferData->VirtualAddress;

            interrupterData->CycleState     = 1;
            interrupterData->DequeueIndex   = 0;
            interrupterData->DequeueSegment = 0;
        }

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Interrupter_UpdateERDP(
    __in
        PINTERRUPTER_DATA   interrupterData,
    __in
        BOOLEAN             ClearEventHandlerBusy
    )
{
    ERDP    eventRingDequeuePointer;

    //
    // Cache the current TRB info regarding this event (segment, dequeue pointer),
    // that is used later to write to the event ring dequeue pointer.
    //
    eventRingDequeuePointer.AsUlong64 = interrupterData->CurrentBufferData->LogicalAddress.QuadPart;
    eventRingDequeuePointer.AsUlong64 += (sizeof(TRB) * interrupterData->DequeueIndex);

    eventRingDequeuePointer.DequeueErstSegmentIndex = interrupterData->DequeueSegment;

    //
    // N.B: EventHandlerBusy is 'write 1 to clear'.
    //
    if (ClearEventHandlerBusy) {
        eventRingDequeuePointer.EventHandlerBusy = 1;
    } else {
        eventRingDequeuePointer.EventHandlerBusy = 0;
    }

    LogVerbose(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
               "Update ERDP 0x%I64x", eventRingDequeuePointer.AsUlong64);

    Register_WriteUlong64(
        &interrupterData->InterrupterRegisterSet->EventRingDequeuePointerRegister.AsUlong64,
        eventRingDequeuePointer.AsUlong64,
        (Controller_GetDeviceFlags(interrupterData->ControllerHandle).Use32BitRegisterAccess == 1));

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Interrupter_InterrupterRegisterIntialize(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData
    )
/*++

Routine Description:

    This routine initializes (or re-initializes) the following XHCI host
    controller registers:

    5.5.2.3.1 Event Ring Segment Table Size Register (ERSTSZ)
    5.5.2.3.2 Event Ring Segment Table Base Address Register (ERSTBA)
    5.5.2.3.3 Event Ring Dequeue Pointer Register (ERDP)

    Initializing the ERSTBA register has the side effect of initializing
    the Event Ring State Machine internal variables (ERSTE, ERST Count,
    EREP, and TRB Count).

--*/
{
    ERDP                eventRingDequeuePointer;
    ERSTSZ              eventRingSegmentSize;
    ULONG               i;
    PINTERRUPTER_DATA   interrupterData;

    TRY {

        for (i = 0; i < Interrupter_GetInterrupterCount(PrimaryInterrupterData); i++) {

            interrupterData = PrimaryInterrupterData->InterrupterDataArray[i];

            eventRingDequeuePointer.AsUlong64 = interrupterData->CurrentBufferData->LogicalAddress.QuadPart;
            eventRingDequeuePointer.AsUlong64 += (sizeof(TRB) * interrupterData->DequeueIndex);

            LogInfo(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                    "InterrupterData 0x%p CurrentBufferData 0x%p CycleState %d Segment %d Index %3d ERDP 0x%I64x",
                    interrupterData, interrupterData->CurrentBufferData,
                    interrupterData->CycleState, interrupterData->DequeueSegment, interrupterData->DequeueIndex,
                    eventRingDequeuePointer.AsUlong64);

            //
            // 4.9.4 Event Ring Management
            //
            // Prior to writing the ERST Base Address (ERSTBA) register
            // system software shall:
            //
            // Write the ERST Size (ERSTSZ) Register with the number of
            // valid entries in the ERST.
            //
            // Write the Event Ring Dequeue Pointer (ERDP) Register with the
            // value of ERST(0).BaseAddress.
            //
            // Then:
            //
            // Write the ERST Base Address (ERSTBA) register with the value
            // of ERST(0).BaseAddress. When the ERSTBA register is written,
            // the Event Ring State Machine (refer to Figure 20) is set to
            // the Start state.
            //
            // Writing the ERSTBA initializes the Event Ring State Machine
            // internal variables (ERSTE, ERST Count, EREP, and TRB Count)
            // and advances it to wait for Run/Stop (R/S) to be asserted or
            // an event to be posted
            //

            //
            // 5.5.2.3.1 Event Ring Segment Table Size Register (ERSTSZ)
            //
            eventRingSegmentSize.AsUlong32 = 0;
            eventRingSegmentSize.Size = interrupterData->SegmentCount;

            Register_WriteUlong(
                &interrupterData->InterrupterRegisterSet->EventRingSegmentTableSizeRegister.AsUlong32,
                eventRingSegmentSize.AsUlong32);

            //
            // 5.5.2.3.3 Event Ring Dequeue Pointer Register (ERDP)
            //
            Interrupter_UpdateERDP(interrupterData, TRUE);

            //
            // 5.5.2.3.2 Event Ring Segment Table Base Address Register (ERSTBA)
            //
            Register_WriteUlong64(
                &interrupterData->InterrupterRegisterSet->EventRingSegmentTableBaseAddress.QuadPart,
                interrupterData->EventRingTableBufferData->LogicalAddress.QuadPart,
                (Controller_GetDeviceFlags(interrupterData->ControllerHandle).Use32BitRegisterAccess == 1));
        }

    } FINALLY {

    }

    return;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_D0Entry(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDF_POWER_DEVICE_STATE      WdfPreviousState,
    __in
        BOOLEAN                     RestoreState
    )
/*++

Routine Description:

    This function is called when the controller enters D0.

--*/
{
    TRY {

        LogVerbose(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                   "PrimaryInterrupterData 0x%p %!WdfPowerDeviceState!",
                   PrimaryInterrupterData, WdfPreviousState);

        if (Controller_IsControllerAccessible(PrimaryInterrupterData->ControllerHandle) == FALSE) {
            LEAVE;
        }

        //
        // Zero-initialize the Interrupter state (eg. event ring buffers, current
        // segment, dequeue pointer etc.) if this is the first entry into D0, or
        // this is a resume from Dx but the restore state operation failed.
        //
        if (RestoreState == FALSE) {

            Interrupter_Initialize(PrimaryInterrupterData);
        }

        Interrupter_InterrupterRegisterIntialize(PrimaryInterrupterData);

    } FINALLY {

    }

    return STATUS_SUCCESS;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_D0ExitPreInterruptsDisabled(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDF_POWER_DEVICE_STATE      WdfTargetState
    )
{
    PINTERRUPTER_DATA   interrupterData;
    ULONG               i;
    KIRQL               irql;
    LARGE_INTEGER       timeOut;
    NTSTATUS            status;

    UNREFERENCED_PARAMETER(WdfTargetState);

    TRY {

        for (i = 0; i < Interrupter_GetInterrupterCount(PrimaryInterrupterData); i++) {

            interrupterData = PrimaryInterrupterData->InterrupterDataArray[i];

            //
            // There's nothing to do if the work item doesn't exist.
            //
            if (interrupterData->IoWorkItem == NULL) {
                continue;
            }

            KeAcquireSpinLock(&interrupterData->EventRingLock, &irql);
            interrupterData->Flags.PendingDisable = 1;
            KeReleaseSpinLock(&interrupterData->EventRingLock, irql);

            //
            // Make sure the work item is not running.
            //
            timeOut.QuadPart = WDF_REL_TIMEOUT_IN_SEC(60);

            do {

                status = KeWaitForSingleObject(&interrupterData->WorkItemEvent,
                                               Executive,
                                               KernelMode,
                                               FALSE,
                                               &timeOut);

                if (status == STATUS_TIMEOUT) {

                    LogInfo(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                            "InterruptDisable is waiting on WorkItem completion for interrupter %d", i);

                } else  {

                    NT_ASSERT(NT_SUCCESS(status));
                    break;
                }

            } WHILE (TRUE);
        }

    } FINALLY {

    }

    return STATUS_SUCCESS;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_D0ExitStopped(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDF_POWER_DEVICE_STATE      WdfTargetState
    )
/*++

Routine Description:

    This function is called when the controller exits D0 after the
    controller has been halted.

    Currently it does nothing, other than logging the current values of event
    ring segment, dequeue index etc. for each interrupter.

--*/
{
    ULONG   i;

    TRY {

        LogVerbose(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                   "PrimaryInterrupterData 0x%p %!WdfPowerDeviceState!",
                   PrimaryInterrupterData, WdfTargetState);

        for (i = 0; i < Interrupter_GetInterrupterCount(PrimaryInterrupterData); i++) {

            PINTERRUPTER_DATA   interrupterData;
            ERDP                eventRingDequeuePointer;

            interrupterData = PrimaryInterrupterData->InterrupterDataArray[i];

            eventRingDequeuePointer.AsUlong64 = interrupterData->CurrentBufferData->LogicalAddress.QuadPart;
            eventRingDequeuePointer.AsUlong64 += (sizeof(TRB) * interrupterData->DequeueIndex);

            LogInfo(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                    "InterrupterData 0x%p CurrentBufferData 0x%p CycleState %d Segment %d Index %3d ERDP 0x%I64x",
                    interrupterData, interrupterData->CurrentBufferData,
                    interrupterData->CycleState, interrupterData->DequeueSegment, interrupterData->DequeueIndex,
                    eventRingDequeuePointer.AsUlong64);
        }

    } FINALLY {

    }

    return;
}

VOID
Interrupter_WdfEvtCleanupCallback(
    WDFOBJECT   WdfInterrupt
    )
/*++

Routine Description:

    The driver's EvtCleanupCallback event callback function removes the driver's
    references on an object so that the object can be deleted.

--*/
{
    PINTERRUPTER_DATA           interrupterData;
    PPRIMARY_INTERRUPTER_DATA   primaryInterrupterData;

    TRY {

        interrupterData = GetInterrupterData(WdfInterrupt);

        LogVerbose(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                   "WdfInterrupt 0x%p interrupterData 0x%p", WdfInterrupt,
                   interrupterData);

        Counter_CloseInterrupterInstance(interrupterData);

        //
        // Retrieve the primary interrupter data if this is the primary interrupter.
        //
        if (interrupterData->MessageNumber == 0) {
            primaryInterrupterData = GetPrimaryInterrupterData(WdfInterrupt);
        } else {
            primaryInterrupterData = NULL;
        }

        if (primaryInterrupterData != NULL) {

            //
            // These should have been freed in ReleaseHardware.
            //
            NT_ASSERT(primaryInterrupterData->InterrupterDataArray == NULL);
            NT_ASSERT(primaryInterrupterData->InterrupterLookup == NULL);
        }

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Interrupter_InterruptEnable(
    __in
        PINTERRUPTER_DATA   InterrupterData
    )
/*++

Routine Description:

    This routine initializes (or re-initializes) the following XHCI host
    controller registers to enable host controller interrupt generation
    for the specified interruper:

    5.5.2.2 Interrupter Moderation Register (IMOD)
    5.5.2.1 Interrupter Management Register (IMAN)

    This routine is called when the EvtInterruptEnable event callback
    function is called for the interrupter.

    This routine is also called when the non-PnP controller reset
    process is in progress.

--*/
{
    IMOD    interrupterModeration;
    IMAN    interrupterManagement;

    TRY {

        //
        // 5.5.2.2 Interrupter Moderation Register (IMOD)
        //
        // The interval is specified in 250ns increments.
        // The default value is 4000 == 1ms.
        // A value of 200 == 50us.
        // A larger value can result in more idle time on the bus
        // between transfers and lower overall data transfer rates.
        //
        interrupterModeration.AsUlong32 = 0;
        interrupterModeration.Inverval = DEFAULT_MODERATION_INTERVAL;
        Register_WriteUlong(&InterrupterData->InterrupterRegisterSet->InterrupterModeration.AsUlong32,
                            interrupterModeration.AsUlong32);

        //
        // 5.5.2.1 Interrupter Management Register (IMAN)
        //
        interrupterManagement.AsUlong32 =
            Register_ReadUlong(&InterrupterData->InterrupterRegisterSet->InterrupterManagement.AsUlong32);

        interrupterManagement.Enable = 1;
        Register_WriteUlong(&InterrupterData->InterrupterRegisterSet->InterrupterManagement.AsUlong32,
                            interrupterManagement.AsUlong32);

    } FINALLY {

    }

    return;
}

NTSTATUS
Interrupter_WdfEvtInterruptEnable(
    WDFINTERRUPT    WdfInterrupt,
    WDFDEVICE       AssociatedDevice
    )
/*++

Routine Description:

    A driver's EvtInterruptEnable event callback function enables
    a specified hardware interrupt.

--*/
{
    PINTERRUPTER_DATA   interrupterData;

    UNREFERENCED_PARAMETER(AssociatedDevice);

    TRY {

        interrupterData = GetInterrupterData(WdfInterrupt);

        LogInfo(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                "Enabling Interrupter %d", interrupterData->MessageNumber);

        interrupterData->Flags.Enabled = 1;
        interrupterData->Flags.PendingDisable = 0;

        if (interrupterData->MessageNumber != 0) {
            NT_ASSERT(interrupterData->Flags.MessageSignaled == 1);
        }

        NT_ASSERT(interrupterData->InterrupterRegisterSet != NULL);

        Interrupter_InterruptEnable(interrupterData);

    } FINALLY {

    }

    return STATUS_SUCCESS;
}

NTSTATUS
Interrupter_WdfEvtInterruptDisable(
    WDFINTERRUPT    WdfInterrupt,
    WDFDEVICE       AssociatedDevice
    )
/*++

Routine Description:

    A driver's EvtInterruptDisable event callback function disables
    a specified hardware interrupt.

--*/
{
    PINTERRUPTER_DATA   interrupterData;
    IMAN                interrupterManagement;

    UNREFERENCED_PARAMETER(AssociatedDevice);

    TRY {

        interrupterData = GetInterrupterData(WdfInterrupt);

        LogInfo(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                "Disabling Interrupter %d", interrupterData->MessageNumber);

        if (interrupterData->MessageNumber != 0) {
            NT_ASSERT(interrupterData->Flags.MessageSignaled == 1);
        }

        interrupterData->Flags.Enabled = 0;

        interrupterManagement.AsUlong32 =
            Register_ReadUlong(&interrupterData->InterrupterRegisterSet->InterrupterManagement.AsUlong32);

        interrupterManagement.Enable = 0;
        Register_WriteUlong(&interrupterData->InterrupterRegisterSet->InterrupterManagement.AsUlong32,
                            interrupterManagement.AsUlong32);

    } FINALLY {

    }

    return STATUS_SUCCESS;
}

BOOLEAN
Interrupter_WdfEvtInterruptIsr(
    WDFINTERRUPT    WdfInterrupt,
    ULONG           MessageId
    )
/*++

Routine Description:

    A driver's EvtInterruptIsr event callback function services
    a hardware interrupt.

Return Value:

    The EvtInterruptIsr callback function must return TRUE if the function
    services the hardware interrupt. Otherwise, this function must return FALSE.

--*/
{
    PINTERRUPTER_DATA   interrupterData;
    IMAN                interrupterManagement;

    interrupterData = GetInterrupterData(WdfInterrupt);

    //
    // 4.17.2 Interrupt Moderation
    //
    // If PCI Message Signaled Interrupts (MSI or MSI-X) are enabled,
    // then the assertion of the Interrupt Pending (IP) flag in Figure
    // 30 generates a PCI Dword write. The IP flag is automatically
    // cleared by the completion of the PCI write.
    //
    if (MessageId != 0) {
        interrupterData->Counter.IsrCount++;
        WdfInterruptQueueDpcForIsr(WdfInterrupt);
        return TRUE;
    }

    if (interrupterData->Flags.MessageSignaled == 1) {
        interrupterData->Counter.IsrCount++;
        WdfInterruptQueueDpcForIsr(WdfInterrupt);
        return TRUE;
    }

    //
    // If the PCI Interrupt Pin mechanism is enabled, then the assertion
    // of Interrupt Pending (IP) asserts the appropriate PCI INTx# pin.
    // And the IP flag is cleared by software writing the IMAN register.
    //
    interrupterManagement.AsUlong32 =
        Register_ReadUlong(&interrupterData->InterrupterRegisterSet->InterrupterManagement.AsUlong32);

    if (interrupterManagement.Pending == 1) {

        Register_WriteUlong(&interrupterData->InterrupterRegisterSet->InterrupterManagement.AsUlong32,
                            interrupterManagement.AsUlong32);

        interrupterData->Counter.IsrCount++;
        WdfInterruptQueueDpcForIsr(WdfInterrupt);
        return TRUE;

    } else {

        return FALSE;
    }
}

VOID
Interrupter_IoWorkItemRequeueDpc(
    _In_
        PVOID           IoObject,
    _In_opt_
        PVOID           Context,
    _In_
        PIO_WORKITEM    IoWorkItem
    )
{
    KIRQL               irql;
    PINTERRUPTER_DATA   interrupterData;

    UNREFERENCED_PARAMETER(IoObject);
    UNREFERENCED_PARAMETER(IoWorkItem);

    TRY {

        interrupterData = (PINTERRUPTER_DATA)Context;

        interrupterData->Counter.WorkItemCount++;

        //
        // Event handler busy needs to be cleared in order for the DPC to
        // process pending event TRBs. This needs to be done after the
        // workitem has been uninitialized. Otherwise it is possible that
        // multiple work items can be queued.
        //
        KeAcquireSpinLock(&interrupterData->EventRingLock, &irql);
        interrupterData->Flags.DpcRunning = 0;
        KeReleaseSpinLock(&interrupterData->EventRingLock, irql);

        interrupterData->Counter.QueuedDpc++;
        WdfInterruptQueueDpcForIsr(interrupterData->WdfInterrupt);

    } FINALLY {

        KeSetEvent(&interrupterData->WorkItemEvent, IO_NO_INCREMENT, FALSE);
    }

    return;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
BOOLEAN
Interrupter_IsDpcExceededWatchdogLimit(
    __in
        PULONG                  DpcTimeLimit,
    __in
        PULONG                  DpcWatchdogLimit,
    __in
        PINTERRUPTER_COUNTER    InterrupterCounter
        )
/*++

Routine Description:

    Determines whether the dpc is in danger of running too long.
    A string of sequential DPCs can only run for a limited amount of time before
    the passive-level threads on the system are starved; this triggers the
    DPC watchdog timer.

Return Value:

    If this function detects that the DPC execution time, as a percentage of the
    limit, has exceeded the set maximum, it will return TRUE.

--*/
{
    KDPC_WATCHDOG_INFORMATION   watchdogInfo;
    NTSTATUS                    status;
    BOOLEAN                     exceededLimit;

    TRY {

        exceededLimit = FALSE;

        status = KeQueryDpcWatchdogInformation(&watchdogInfo);

        if (NT_SUCCESS(status)) {

            //
            // Only calculate the DPC limits once per DPC.
            //
            if (*DpcTimeLimit == 0) {

                *DpcTimeLimit = (watchdogInfo.DpcTimeLimit * DPC_TIME_THRESHOLD_PERCENTAGE) / 100;
            }

            if (*DpcWatchdogLimit == 0) {

                *DpcWatchdogLimit =
                    (watchdogInfo.DpcWatchdogLimit * DPC_WATCHDOG_THRESHOLD_PERCENTAGE) / 100;
            }

            //
            // Use the DPC time limit if specified and if not, then use the DPC watchdog limit.
            //
            if (watchdogInfo.DpcTimeCount < *DpcTimeLimit) {

                InterrupterCounter->DpcTimeExpired++;
                exceededLimit = TRUE;
                LEAVE;
            }

            if (watchdogInfo.DpcWatchdogCount < *DpcWatchdogLimit) {

                InterrupterCounter->DpcWatchdogExpired++;
                exceededLimit = TRUE;
                LEAVE;
            }
        }

    } FINALLY {

    }

    return exceededLimit;
}

VOID
Interrupter_WdfEvtInterruptDpc(
    WDFINTERRUPT    WdfInterrupt,
    WDFOBJECT       WdfAssociatedObject
    )
/*++

Routine Description:

    A driver's EvtInterruptDpc event callback function processes interrupt
    information that the driver's EvtInterruptIsr callback function has stored.

--*/
{
    PINTERRUPTER_DATA           interrupterData;
    PPRIMARY_INTERRUPTER_DATA   primaryInterrupterData;
    KIRQL                       irql;
    PTRB                        currentTrb;
    TRB                         localTrb;
    HANDLE                      deviceSlotHandle;
    BOOLEAN                     requeueDpc;
    ULONG                       eventsProcessed;
    ULONG                       dpcTimeLimit;
    ULONG                       dpcWatchdogLimit;
    PTD_TRACKING_DATA           tdTrackingData;
    HANDLE                      usbDeviceHandle;

    TRY {

        interrupterData = GetInterrupterData(WdfInterrupt);
        deviceSlotHandle = Controller_GetDeviceSlotHandle(interrupterData->ControllerHandle);

        LogVerbose(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                   "WdfInterrupt 0x%p WdfAssociatedObject 0x%p", WdfInterrupt, WdfAssociatedObject);

        requeueDpc = FALSE;

        interrupterData->Counter.DpcCount++;

        //
        // Retrieve the primary interrupter data if this is the primary interrupter.
        //
        if (interrupterData->MessageNumber == 0) {
            primaryInterrupterData = GetPrimaryInterrupterData(WdfInterrupt);
        } else {
            primaryInterrupterData = NULL;
        }

        KeAcquireSpinLock(&interrupterData->EventRingLock, &irql);

        //
        // The EventHandlerBusy is used to prevent concurrent execution of this DPC. If
        // this value is true, then there is either another DPC draining the event ring
        // or a work item has been queued that will queue another DPC to drain the
        // event ring.
        //
        if (interrupterData->Flags.DpcRunning == 1) {
            KeReleaseSpinLock(&interrupterData->EventRingLock, irql);
            interrupterData->Counter.EventRingBusy++;
            LEAVE;
        }

        interrupterData->Flags.DpcRunning = 1;
        KeReleaseSpinLock(&interrupterData->EventRingLock, irql);

        eventsProcessed = 0;
        dpcTimeLimit = 0;
        dpcWatchdogLimit = 0;

        currentTrb = &interrupterData->CurrentRingSegment[interrupterData->DequeueIndex];

        //
        // Process events until a cycle bit transition, which means that the event
        // ring is empty.
        //
        while (currentTrb->TransferEvent.CycleBit == interrupterData->CycleState) {

            LogVerbose(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                       "currentTrb 0x%p DequeueIndex %d", currentTrb, interrupterData->DequeueIndex);

            //
            // Check to see if too much time has been spent in the DPC. If too much time
            // has been spent in the DPC, then the DPC is requeued and the rest of the
            // events a processed on the subsequent DPC. Check this only after a certain
            // number of events have been processed, instead of for each event.
            //
            if (((eventsProcessed % INTERRUPTER_MAX_EVENTS_HANDLED_PER_DPC_TIMEOUT_CHECK) == 0) &&
                (Interrupter_IsDpcExceededWatchdogLimit(&dpcTimeLimit,
                                                        &dpcWatchdogLimit,
                                                        &interrupterData->Counter))) {

                LogVerbose(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                           "Requeuing DPC after processing %d events", eventsProcessed);
                Interrupter_UpdateERDP(interrupterData, FALSE);
                requeueDpc = TRUE;
                LEAVE;
            }

            //
            // Work on a local copy of the TRB to reduce unnecessary accesses to CommonBuffer,
            // which can be expensive on systems where CommonBuffer accesses are non-cached.
            //
            // N.B.: TRB must only be copied locally after the CycleBit is verified to be valid.
            //       Copying it first and then checking the CycleBit is incorrect because the
            //       CycleBit which is in the high DWORD can become valid after some of the low
            //       bytes are copied, and the low bytes can be invalid when they were copied.
            //
            RtlCopyMemory(&localTrb, currentTrb, sizeof(TRB));

            LogInfo(interrupterData->RecorderLog, Interrupter,
                    "[%3d] SlotId %2d EndpointId %2d ED %d Pointer %-16I64x Length %-8d %!TrbCompletionCode! %!TrbType!",
                    interrupterData->DequeueIndex,
                    localTrb.TransferEvent.SlotId,
                    localTrb.TransferEvent.EndpointId,
                    localTrb.TransferEvent.EventData,
                    localTrb.TransferEvent.Pointer.QuadPart,
                    localTrb.TransferEvent.TransferLength,
                    (TRB_COMPLETION_CODE)localTrb.TransferEvent.CompletionCode,
                    (TRB_TYPE)localTrb.TransferEvent.Type);

            interrupterData->Counter.EventCount += 1;

            //
            // Update the Dequeue index to the next TRB in the event ring. If this is the
            // last TRB in the event ring, move to the next event ring segment.
            //
            // Note that this is done prior to dispatching the current TRB to ease
            // debugging. The debug tools will not display the current TRB if the
            // Dequeue/segment update occurs after the currentTRB is dispatched.
            //
            interrupterData->DequeueIndex++;

            if (interrupterData->DequeueIndex == interrupterData->TRBsPerSegment) {

                LogVerbose(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                           "End of event ring segment: DequeueIndex %d DequeueSegment %d CycleState %d",
                           interrupterData->DequeueIndex, interrupterData->DequeueSegment,
                           interrupterData->CycleState);

                interrupterData->DequeueIndex = 0;

                interrupterData->DequeueSegment++;

                if (interrupterData->DequeueSegment == interrupterData->SegmentCount) {

                    interrupterData->CycleState = (interrupterData->CycleState == 1) ? 0 : 1;

                    LogVerbose(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                               "End of event ring segment table: DequeueSegment %d CycleState %d",
                               interrupterData->DequeueSegment, interrupterData->CycleState);

                    interrupterData->DequeueSegment = 0;

                    //
                    // Set the current buffer data to point to the first entry in the
                    // buffer data list (this is the first event ring segment).
                    //
                    interrupterData->CurrentBufferData =
                        (PBUFFER_DATA)interrupterData->BufferDataList.Flink;

                } else {

                    //
                    // Set the current buffer data to the next entry in the buffer
                    // data list.
                    //
                    interrupterData->CurrentBufferData =
                        (PBUFFER_DATA)interrupterData->CurrentBufferData->ListEntry.Flink;

                    NT_ASSERT(interrupterData->CurrentBufferData !=
                              (PBUFFER_DATA)&interrupterData->BufferDataList);
                }

                //
                // Set the current ring segment to reflect the current buffer data.
                //
                interrupterData->CurrentRingSegment =
                    (PTRB)interrupterData->CurrentBufferData->VirtualAddress;
            }

            //
            // If the a vendor completion code of 199 is received and the device flag is
            // set to ignore this completion code, ignore this TRB.
            //
            if ((localTrb.TransferEvent.CompletionCode == TRB_TYPE_VENDOR_CODE_199) &&
                Controller_GetDeviceFlags(interrupterData->ControllerHandle).IgnoreCompletionCode199) {

                LogWarning(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                           "Skipping TRB 0x%p with %!TrbCompletionCode! and %!TrbType!",
                           currentTrb,
                           localTrb.TransferEvent.CompletionCode,
                           (TRB_TYPE)localTrb.TransferEvent.Type);

            } else if (localTrb.TransferEvent.Type == TRB_TYPE_TRANSFER_EVENT) {

                LogVerbose(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                           "%!TrbType! %!TrbCompletionCode!",
                           (TRB_TYPE)localTrb.TransferEvent.Type,
                           (TRB_COMPLETION_CODE)localTrb.TransferEvent.CompletionCode);

                //
                // Do this check upfront because if this condition is satisfied, the
                // localTrb.TransferEvent.Pointer value is 0. Rest of the code expects
                // (Pointer != 0) if (EventData == 1).
                //
                if ((localTrb.TransferEvent.EventData == 1) &&
                    (Controller_GetDeviceFlags(interrupterData->ControllerHandle).IgnoreEventDataBitForTransferRingEmptyEvent) &&
                    ((localTrb.TransferEvent.CompletionCode == TRB_COMPLETION_CODE_RING_OVERRUN) ||
                     (localTrb.TransferEvent.CompletionCode == TRB_COMPLETION_CODE_RING_UNDERRUN))) {

                    localTrb.TransferEvent.EventData = 0;
                }

                //
                // If the completed TRB is a non-isoch Event Data TRB, and we trust the controller
                // to give us the right pointer in a timely manner, then the request is forwarded
                // directly to the transfer ring. Otherwise, it is first directed to the USB device
                // registered for the slot.
                //
                tdTrackingData = (PTD_TRACKING_DATA)&localTrb.TransferEvent.Pointer;

                if ((localTrb.TransferEvent.EventData == 1) &&
                    (tdTrackingData->EndpointType != USB_ENDPOINT_TYPE_ISOCHRONOUS) &&
                    (Controller_GetDeviceFlags(interrupterData->ControllerHandle).ValidateTransferEventPointer == 0)) {

                    TR_TransferEventHandler(&localTrb.TransferEvent, NULL);

                } else {

                    usbDeviceHandle =
                        DeviceSlot_GetUsbDeviceHandle(deviceSlotHandle,
                                                      localTrb.TransferEvent.SlotId);

                    //
                    // There have been cases where the endpoint has been deconfigured
                    // - due to hw issue - but a transfer event TRB is still received.
                    // In that case, gracefully ignore this transfer event TRB.
                    //
                    if (usbDeviceHandle) {

                        UsbDevice_TransferEventHandler(usbDeviceHandle, &localTrb.TransferEvent);

                    } else if (tdTrackingData->EndpointType != USB_ENDPOINT_TYPE_ISOCHRONOUS) {

                        LogWarning(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                                   "TRB 0x%p SlotId %d EndpointId %d references NULL UsbDevice",
                                   currentTrb, localTrb.TransferEvent.SlotId,
                                   localTrb.TransferEvent.EndpointId);
                        NT_ASSERTMSG("SlotId references NULL endpoint", 0);
                    }
                }

            } else if ((localTrb.TransferEvent.Type == TRB_TYPE_HOST_CONTROLLER_EVENT) &&
                       (localTrb.HostControllerEvent.CompletionCode ==
                        TRB_COMPLETION_CODE_EVENT_RING_FULL_ERROR)) {

                interrupterData->Counter.EventRingFull++;

                LogWarning(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                           "Event Ring is Full");

                if (Controller_GetDeviceFlags(interrupterData->ControllerHandle).EnableErrorRecoveryOnEventRingFull) {

                    Controller_ReportFatalError(interrupterData->ControllerHandle,
                                                Controller_FatalErrorReset,
                                                USB3_WER_BUGCODE_USBXHCI_EVENT_RING_FULL_ERROR,
                                                NULL,
                                                NULL,
                                                NULL);
                }

            } else {

                //
                // If it's not a transfer event, then this should be the primary
                // interrupter. Add additional verification to make sure.
                //
                if (interrupterData->MessageNumber != 0) {

                    LogWarning(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                               "Detected invalid TRB type %!TrbType! on secondary interrupter",
                               (TRB_TYPE)localTrb.TransferEvent.Type);
                    NT_ASSERTMSG("Detected invalid TRB type on secondary interrupter", 0);

                } else {

                    NT_ASSERT(primaryInterrupterData != NULL);

                    LogVerbose(Controller_GetRecorderLog(interrupterData->ControllerHandle), Interrupter,
                               "%!TrbType! %!TrbCompletionCode!",
                               (TRB_TYPE)localTrb.TransferEvent.Type,
                               (TRB_COMPLETION_CODE)localTrb.TransferEvent.CompletionCode);

                    switch (localTrb.TransferEvent.Type) {

                    case TRB_TYPE_COMMAND_COMPLETION_EVENT:




                        Command_HandleCommandCompletionEvent(
                            Controller_GetCommandHandle(interrupterData->ControllerHandle),
                            &localTrb);

                        break;

                    case TRB_TYPE_PORT_STATUS_CHANGE_EVENT:

                        RootHub_HandlePortStatusChangeEvent(
                            Controller_GetRootHubHandle(interrupterData->ControllerHandle),
                            &localTrb);

                        break;

                    case TRB_TYPE_DEVICE_NOTIFICATION_EVENT:

                        usbDeviceHandle = DeviceSlot_GetUsbDeviceHandle(
                            deviceSlotHandle,
                            localTrb.DeviceNotificationEvent.SlotId);

                        if (usbDeviceHandle != NULL) {
                            UsbDevice_DeviceNotificationEventHandler(usbDeviceHandle, &localTrb);
                        }

                        break;

                    case TRB_TYPE_HOST_CONTROLLER_EVENT:

                        LogWarning(Controller_GetRecorderLog(primaryInterrupterData->ControllerHandle), Interrupter,
                                   "%!TrbType! %!TrbCompletionCode!",
                                   (TRB_TYPE)localTrb.TransferEvent.Type,
                                   (TRB_COMPLETION_CODE)localTrb.TransferEvent.CompletionCode);
                        break;

                    case TRB_TYPE_MFINDEX_WRAP_EVENT:

                        NT_ASSERTMSG("Unexpected MFINDEX wrap event", FALSE);
                        break;

                    default:
                        LogWarning(Controller_GetRecorderLog(primaryInterrupterData->ControllerHandle), Interrupter,
                                   "Detected unsupported or invalid TRB type %!TrbType!",
                                   (TRB_TYPE)localTrb.TransferEvent.Type);
                        break;
                    }
                }
            }

            currentTrb = &interrupterData->CurrentRingSegment[interrupterData->DequeueIndex];

            //
            // Update the ERDP at regular intervals to give ownership of processed events
            // back to the controller. Do not update the EventHandlerBusy bit because we
            // are still processing events and do not need another interrupt. Only do this
            // if we know there are more events to be processed after this one.
            //
            if ((eventsProcessed != 0) &&
                ((eventsProcessed % INTERRUPTER_MAX_EVENTS_PROCESSED_PER_ERDP_UPDATE) == 0) &&
                (currentTrb->TransferEvent.CycleBit == interrupterData->CycleState)) {

                Interrupter_UpdateERDP(interrupterData, FALSE);
            }

            eventsProcessed++;
        }

        //
        // Write the ERDP to clear the event handler busy (EHB) bit.
        //
        // N.B: According to Spec, software is not allowed to write the same value of
        // Event Ring Dequeue Pointer in two consecutive writes. But there have been
        // cases where the controller interrupted the host with no events to process.
        // In that case, if software did not update the ERDP to clear the EHB, controller
        // will not generate any more interrupts. In order to make forward progress,
        // we update the ERDP with the same value of dequeue pointer if
        // (eventsProcessed == 0).
        //
        // 4.9.4 Software writes to the ERDP register shall always advance the Event Ring
        // Dequeue Pointer value, i.e. software shall not write the same value to the ERDP
        // register on two consecutive write operations.
        //
        // 5.5.2.3.3 Software shall not write ERDP consecutively with the same value unless
        // it is a FULL to EMPTY advancement of the Event Ring.
        //
        KeAcquireSpinLock(&interrupterData->EventRingLock, &irql);
        interrupterData->Flags.DpcRunning = 0;
        Interrupter_UpdateERDP(interrupterData, TRUE);
        KeReleaseSpinLock(&interrupterData->EventRingLock, irql);

    } FINALLY {

        //
        // If the DPC is requeued, the ERDP register is not updated, but the internal
        // event handler busy boolean is. This prevents the controller from asserting
        // the interrupt and guarantees that the subsequent DPC will continue draining
        // events from the event ring.
        //
        if (requeueDpc) {

            BOOLEAN queued;

            queued = FALSE;

            interrupterData->Counter.RequeueCount++;

            KeAcquireSpinLock(&interrupterData->EventRingLock, &irql);

            if (interrupterData->Flags.PendingDisable == 0) {

                KeClearEvent(&interrupterData->WorkItemEvent);

                KeReleaseSpinLock(&interrupterData->EventRingLock, irql);

                if (Controller_IsForwardProgressEnabled(interrupterData->ControllerHandle)) {

                    //
                    // IoWorkItem is only allocated if g_IoTryQueueWorkItem is set, otherwise
                    // this value is always NULL.
                    //
                    if (interrupterData->IoWorkItem) {

                        NT_ASSERT(g_IoTryQueueWorkItem != NULL);

                        queued = (g_IoTryQueueWorkItem)(interrupterData->IoWorkItem,
                                                        Interrupter_IoWorkItemRequeueDpc,
                                                        CriticalWorkQueue,
                                                        interrupterData);
                    }

                    //
                    // If the work item was not successfully queued, try requeuing the DPC directly.
                    // Normally this avoided, since a straight requeue of the DPC does not guarantee
                    // that the current processor will return to passive level IRQL and can still
                    // result in a DPC watchdog timer expiration.
                    //
                    if (queued == FALSE) {

                        //
                        // Event handler busy needs to be cleared or else the queued
                        // DPC will exit before processing any event TRBs.
                        //
                        KeAcquireSpinLock(&interrupterData->EventRingLock, &irql);
                        interrupterData->Flags.DpcRunning = 0;
                        KeReleaseSpinLock(&interrupterData->EventRingLock, irql);

                        //
                        // The work item was not queued. In that case, the work item event
                        // needs to set so rundown is not blocked waiting for event.
                        //
                        KeSetEvent(&interrupterData->WorkItemEvent, IO_NO_INCREMENT, FALSE);

                        interrupterData->Counter.QueuedDpc++;
                        WdfInterruptQueueDpcForIsr(WdfInterrupt);
                    }

                } else {

                    IoQueueWorkItemEx(interrupterData->IoWorkItem,
                                      Interrupter_IoWorkItemRequeueDpc,
                                      CriticalWorkQueue,
                                      interrupterData);

                    queued = TRUE;
                }

                if (queued) {

                    interrupterData->Counter.QueuedWorkItem++;
                }

            } else {

                KeReleaseSpinLock(&interrupterData->EventRingLock, irql);
            }
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Interrupter_ControllerResetPostReset(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData
    )
/*++

Routine Description:

    This routine is called when the non-PnP controller reset process is
    in progress. The controller has been reset and the controller has
    not yet been started again.

--*/
{
    PINTERRUPTER_DATA   interrupterData;
    ULONG               i;

    TRY {

        if (Controller_IsControllerAccessible(PrimaryInterrupterData->ControllerHandle) == FALSE) {
            LEAVE;
        }

        //
        // Zero initialize the event ring segment buffers and
        // re-initialize the following XHCI host controller registers:
        //
        // 5.5.2.3.1 Event Ring Segment Table Size Register (ERSTSZ)
        // 5.5.2.3.2 Event Ring Segment Table Base Address Register (ERSTBA)
        // 5.5.2.3.3 Event Ring Dequeue Pointer Register (ERDP)
        //
        Interrupter_Initialize(PrimaryInterrupterData);

        Interrupter_InterrupterRegisterIntialize(PrimaryInterrupterData);

        //
        // Re-initialize the following XHCI host controller registers to
        // enable host controller interrupt generation for each enabled
        // interruper:
        //
        // 5.5.2.2 Interrupter Moderation Register (IMOD)
        // 5.5.2.1 Interrupter Management Register (IMAN)
        //
        for (i = 0; i < Interrupter_GetInterrupterCount(PrimaryInterrupterData); i++) {

            interrupterData = PrimaryInterrupterData->InterrupterDataArray[i];

            if (interrupterData->Flags.Enabled != 0) {

                Interrupter_InterruptEnable(interrupterData);
            }
        }

    } FINALLY {
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_DetermineSecondaryInterrupterCount(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        ULONG                       SupportedInterrupterCount
    )
/*++

Routine Description:

    This function determines the number of secondary interrupters to create,
    taking into account the number of interrupters supported and number of
    processors on the system. The constraints for determining number of secondary
    interrupters are as follows:

    1. There is always one primary interrupter that is not affinitized to any
       processor. So number of secondary interrupters that can be created is
       one less than the number of interrupters supported.
    2. A secondary interrupter may be affinitized to one processor, or more
       than one processor if there are more processors than interrupters.
    3. More than one interrupter cannot be affinitized to a given processor,
       because it simply does not make sense.

    Policy is to try to evenly spread the secondary interrupters across the most
    number of processors. Below table gives examples of various confirugations and
    the number of secondary interrupters that will be picked by this algorithm in
    each case.

    --------------------------------------------------------------------------------------------------------------
    | #Proc | #Interrupters | #SecondaryInterrupters | #UnusedInterrupters | #ProcPerInterrupter | #ProcLeftOver |
    --------|---------------|------------------------|---------------------|---------------------|---------------|
    | 1     | 1             | 0                      | 0                   | N/A                 | N/A           |
    | 1     | 8             | 0                      | 7                   | N/A                 | N/A           |
    | 4     | 8             | 4                      | 3                   | 1                   | 0             |
    | 8     | 8             | 4*                     | 3                   | 2                   | 0             |
    | 16    | 8             | 4*                     | 3                   | 4                   | 0             |
    | 7     | 8             | 7                      | 0                   | 1                   | 0             |
    | 9     | 8             | 4*                     | 3                   | 2                   | 1**           |
    --------------------------------------------------------------------------------------------------------------

    *  3 interrupters go unused in this case, because 4 is the number that helps
       spread the interrupter evenly across the processors.

    ** ProcLeftOver is affinitized to the Primary interrupter.

--*/
{
    ULONG   activeProcessorCount;
    ULONG   secondaryInterrupterCount;
    ULONG   secondaryInterruptersMax;

    PAGED_CODE();

    TRY {

        activeProcessorCount = KeQueryActiveProcessorCountEx(0);

        //
        // Multiple interrupters are not supported on some controllers.
        //
        if (Controller_GetDeviceFlags(PrimaryInterrupterData->ControllerHandle).UseSingleInterrupter) {

            secondaryInterrupterCount = 0;
            LEAVE;
        }

        //
        // Deduct 1 from the number of interrupters supported by the controller to account
        // for the primary interrupter.
        //
        secondaryInterruptersMax = SupportedInterrupterCount - 1;

        if (secondaryInterruptersMax == 0) {
            secondaryInterrupterCount = 0;
            LEAVE;
        }

        if (secondaryInterruptersMax > MAX_SECONDARY_INTERRUPTERS) {
            secondaryInterruptersMax = MAX_SECONDARY_INTERRUPTERS;
        }

        if (activeProcessorCount <= secondaryInterruptersMax) {

            //
            // Less logical processors than available interrupters. Create one secondary
            // interrupter for each logical processor.
            //
            secondaryInterrupterCount = activeProcessorCount;

        } else {

            //
            // More logical processors than available interrupters. In this case, pick
            // a number of interrupters that divides the number of logical processors by
            // a certain factor. If the division is not even, then the extra processors
            // will fallback to the primary interrupter as their target.
            //
            secondaryInterrupterCount = activeProcessorCount / 2;

            while (secondaryInterrupterCount > secondaryInterruptersMax) {
                secondaryInterrupterCount /= 2;
            }
        }

    } FINALLY {

        LogInfo(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                "Logical Processors %u, Supported Interrupters %u, Secondary Interrupters %u",
                activeProcessorCount, SupportedInterrupterCount, secondaryInterrupterCount);

        Interrupter_SetSecondaryInterrupterCount(PrimaryInterrupterData, secondaryInterrupterCount);
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_FilterAllMSIResources(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDFIORESREQLIST             IoResourceRequirementsList
    )
/*++

Routine Description:

    This routine will remove all MSI resource descriptors from the supplied Resource
    Requirements List. It will also ensure that the resulting resource requirements
    list has atleast one line-based interrupt descriptor.

--*/
{
    PIO_RESOURCE_DESCRIPTOR descriptor;
    ULONG                   descriptorIndex;
    ULONG                   lineBasedDescriptorCount;
    BOOLEAN                 lineBasedDescriptorFound;
    ULONG                   listIndex;
    IO_RESOURCE_DESCRIPTOR  newDescriptor;
    WDFIORESLIST            resourceList;
    NTSTATUS                status;

    PAGED_CODE();

    TRY {

        lineBasedDescriptorFound = FALSE;
        listIndex = 0;

        while (listIndex < WdfIoResourceRequirementsListGetCount(IoResourceRequirementsList)) {

            lineBasedDescriptorCount = 0;
            resourceList = WdfIoResourceRequirementsListGetIoResList(IoResourceRequirementsList, listIndex);

            //
            // Remove MSI resource descriptors from the resource list. If a line-based
            // descriptor is found, change its type (from alternate) to default.
            //
            descriptorIndex = 0;
            while (descriptorIndex < WdfIoResourceListGetCount(resourceList)) {

                descriptor = WdfIoResourceListGetDescriptor(resourceList, descriptorIndex);

                if (descriptor->Type != CmResourceTypeInterrupt) {
                    descriptorIndex += 1;
                    continue;
                }

                if (IsIoResourceDescriptorMSI(descriptor) != FALSE) {

                    WdfIoResourceListRemove(resourceList, descriptorIndex);

                    LogInfo(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                            "Resource List %u, removed message interrupt descriptor %u",
                            listIndex,
                            descriptorIndex);

                } else {

                    NT_ASSERTMSG("Expected only one line-based descriptor per resource list",
                                 (lineBasedDescriptorCount == 0));

                    //
                    // Line-based descriptor: change its type (from alternate) to default.
                    //
                    RtlCopyMemory(&newDescriptor, descriptor, sizeof(IO_RESOURCE_DESCRIPTOR));
                    newDescriptor.Option = 0;
                    WdfIoResourceListUpdateDescriptor(resourceList, &newDescriptor, descriptorIndex);

                    LogInfo(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                            "Resource List %u, updated line based interrupt descriptor %u",
                            listIndex,
                            descriptorIndex);

                    lineBasedDescriptorFound = TRUE;
                    lineBasedDescriptorCount += 1;
                    descriptorIndex += 1;
                }
            }

            //
            // If this resource list did not have any line-based descriptors, remove the
            // entire list.
            //
            if (lineBasedDescriptorCount == 0) {

                WdfIoResourceRequirementsListRemove(IoResourceRequirementsList, listIndex);

                LogInfo(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                        "Removed Resource list %u with no line-based descriptors",
                        listIndex);

            } else {

                listIndex += 1;
            }
        }

        if (lineBasedDescriptorFound == FALSE) {

            LogError(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                     "No line-based descriptors found");
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_AffinitizeMSIXDescriptors(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDFIORESREQLIST             IoResourceRequirementsList,
    __in
        ULONG                       ListIndex
    )
/*++

Routine Description:

    Affinitize each of the MSI-X descriptors in the specified Io Resource List to appropriate
    processors.

    If a system has >64 processors, we will affinitize secondary interrupters to processors
    in group 0 only. Effectively, all other processors are affinitized to the primary
    interrupter, in the sense that if we receive a transfer on one of them, we will set the
    primary interrupter as the interrupt target. And primary interrupter is affinitized to
    ALL processors in group 0. So those transfers will complete on any processor of group 0.
    This is not ideal, but is a good enough implementation, and arguably still better than only
    using one interrupter.

    TODO: PostWin8: Take processor groups and Numa nodes into account. Refer
          PciGetMessageAffinityMask(). Also, #define NT_PROCESSOR_GROUPS in pch.h and
          remove the #if defined(NT_PROCESSOR_GROUPS) code from this file.

Arguments:

    PrimaryInterrupterData - Handle to the primary interrupter data.

    IoResourceRequirementsList - IO Resource Requirements list supplied in
        IRP_MN_FILTER_RESOURCE_REQUIREMENTS.

    ListIndex - The index of the Io Resource List whose descriptors are to be affinitized.

--*/
{
    PIO_RESOURCE_DESCRIPTOR descriptor;
    ULONG                   descriptorIndex;
    ULONG                   i;
    ULONG                   msiDescriptorIndex;
    ULONG                   processorNumber;
    ULONG                   processorsPerInterrupter;
    WDFIORESLIST            resourceList;
    IO_RESOURCE_DESCRIPTOR  updatedDescriptor;

    PAGED_CODE();

    TRY {

        if (Interrupter_GetSecondaryInterrupterCount(PrimaryInterrupterData) == 0) {
            LEAVE;
        }

        NT_ASSERT(KeQueryActiveProcessorCountEx(0) >=
                  Interrupter_GetSecondaryInterrupterCount(PrimaryInterrupterData));

        processorsPerInterrupter = KeQueryActiveProcessorCountEx(0) /
                                   Interrupter_GetSecondaryInterrupterCount(PrimaryInterrupterData);

        resourceList = WdfIoResourceRequirementsListGetIoResList(IoResourceRequirementsList, ListIndex);

        processorNumber = 0;
        msiDescriptorIndex = 0;

        for (descriptorIndex = 0;
             descriptorIndex < WdfIoResourceListGetCount(resourceList);
             descriptorIndex += 1) {

            descriptor = WdfIoResourceListGetDescriptor(resourceList, descriptorIndex);

            if (IsIoResourceDescriptorMSI(descriptor) == FALSE) {
                continue;
            }

            msiDescriptorIndex += 1;

            RtlCopyMemory(&updatedDescriptor, descriptor, sizeof(IO_RESOURCE_DESCRIPTOR));

            updatedDescriptor.Flags |= CM_RESOURCE_INTERRUPT_POLICY_INCLUDED;
            updatedDescriptor.u.Interrupt.AffinityPolicy = IrqPolicySpecifiedProcessors;
            updatedDescriptor.u.Interrupt.TargetedProcessors = 0;

#if defined(NT_PROCESSOR_GROUPS)
            updatedDescriptor.u.Interrupt.Group = 0;
#else
            //
            // Do not affinitize the primary interrupter.
            //
            if (msiDescriptorIndex != 1) {

                for (i = 0; i < processorsPerInterrupter; i += 1) {
                    updatedDescriptor.u.Interrupt.TargetedProcessors |= AFFINITY_MASK(processorNumber);
                    processorNumber += 1;
                }
            }
#endif

            WdfIoResourceListUpdateDescriptor(resourceList, &updatedDescriptor, descriptorIndex);

            LogInfo(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                    "Affinitized msi-x descriptor (%u, %u) to 0x%I64X",
                    ListIndex,
                    descriptorIndex,
                    (ULONG64)updatedDescriptor.u.Interrupt.TargetedProcessors);
        }

        NT_ASSERT(msiDescriptorIndex == (1 + Interrupter_GetSecondaryInterrupterCount(PrimaryInterrupterData)));

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_FilterMessageDescriptors(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDFIORESREQLIST             IoResourceRequirementsList,
    __in
        ULONG                       ListIndex,
    __in
        ULONG                       MessagesDesired
    )
/*++

Routine Description:

    This routine removes all but the specified number of MSI and MSIX resource
    messages from the specified Resource Requirements List.

--*/
{
    PIO_RESOURCE_DESCRIPTOR descriptor;
    ULONG                   descriptorIndex;
    ULONG                   descriptorCount;
    ULONG                   messageCount;
    ULONG                   messageCountNew;
    WDFIORESLIST            resourceList;
    IO_RESOURCE_DESCRIPTOR  newDescriptor;
    ULONG                   shift;

    PAGED_CODE();

    TRY {

        resourceList = WdfIoResourceRequirementsListGetIoResList(IoResourceRequirementsList, ListIndex);
        descriptorCount = WdfIoResourceListGetCount(resourceList);

        for (descriptorIndex = 0;
             descriptorIndex < descriptorCount;
             descriptorIndex++) {

            descriptor = WdfIoResourceListGetDescriptor(resourceList, descriptorIndex);

            if (IsIoResourceDescriptorMSI(descriptor) == FALSE) {

                continue;
            }

            //
            // If no more messages are needed, just remove the descriptor.
            // Remember to decrement the index and count, as removing the descriptor
            // will reduce the list size and shift everything past the descriptor
            // being removed.
            //

            if (MessagesDesired == 0) {
                LogInfo(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                        "Resource List %u, removed interrupt descriptor at index %u",
                        ListIndex,
                        descriptorIndex);

                WdfIoResourceListRemove(resourceList, descriptorIndex);
                NT_ASSERT(descriptorIndex > 0);
                descriptorIndex--;
                NT_ASSERT(descriptorCount > 0);
                descriptorCount--;
                continue;
            }

            messageCount = descriptor->u.Interrupt.MaximumVector -
                           descriptor->u.Interrupt.MinimumVector + 1;

            //
            // If the number of messages desired is greater than or equal to the
            // number satsified by this descriptor, don't touch the descriptor, but
            // adjust the MessagesDesired.
            //

            if (MessagesDesired >= messageCount) {
                MessagesDesired -= messageCount;
                continue;
            }


            //
            // The number of messages available in this descriptor is greater than
            // the number desired, but the number desired must be at least 1.
            // Therefore, this must be an MSI message descriptor. Update the
            // descriptor to claim only as many messages needed to satisfy the
            // number desired.
            //

            LogInfo(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                    "Resource List %u, reduce message count at index %u",
                    ListIndex,
                    descriptorIndex);

            //
            // MSI messages must be allocated as a power of 2.
            // Round up to the nearest power of 2.
            //

            BitScanReverse(&shift, (MessagesDesired << 1) - 1);
            messageCountNew = 1 << shift;

            NT_ASSERT(messageCountNew <= messageCount);

            //
            // Update the descriptor with the number to allocate.
            //

            RtlCopyMemory(&newDescriptor, descriptor, sizeof(IO_RESOURCE_DESCRIPTOR));
            newDescriptor.u.Interrupt.MaximumVector =
                CM_RESOURCE_INTERRUPT_MESSAGE_TOKEN;

            newDescriptor.u.Interrupt.MinimumVector =
                CM_RESOURCE_INTERRUPT_MESSAGE_TOKEN - messageCountNew + 1;

            WdfIoResourceListUpdateDescriptor(resourceList, &newDescriptor, descriptorIndex);
            MessagesDesired = 0;
        }

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_FilterRemoveResourceRequirements(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDFIORESREQLIST             IoResourceRequirementsList
    )
/*++

Routine Description:

    This routine will remove certain interrupt resource descriptors or entire resource
    requirements lists under certain conditions.

--*/
{
    BOOLEAN     isMSIX;
    ULONG       listIndex;
    ULONG       messageCount;
    ULONG       messagesDesired;
    NTSTATUS    status;

    PAGED_CODE();

    TRY {

        //
        // If a device flag requires that line-based interrupts be forced for this controller,
        // then all MSI resources are removed.
        //
        if (Controller_GetDeviceFlags(PrimaryInterrupterData->ControllerHandle).UseLineBasedInterrupts) {

            Interrupter_SetSecondaryInterrupterCount(PrimaryInterrupterData, 0);
            status = Interrupter_FilterAllMSIResources(PrimaryInterrupterData, IoResourceRequirementsList);
            LEAVE;
        }

        //
        // If MSI-X interrupts are supported by the device, determine the appropriate number of
        // secondary interrupts to create. Then filter the resource list to keep only the number
        // of MSI-X descriptors equal to the number of secondary interrupters we intend to create.
        //
        Interrupter_IoResList_GetMessageCountAndType(IoResourceRequirementsList,
                                                     &listIndex,
                                                     &messageCount,
                                                     &isMSIX);

        if (isMSIX != FALSE) {

            NT_ASSERT(messageCount >= 1);

            Interrupter_DetermineSecondaryInterrupterCount(PrimaryInterrupterData, messageCount);

        } else {

            //
            // MSI interrupts don't use any secondary interrupters.
            //

            Interrupter_SetSecondaryInterrupterCount(PrimaryInterrupterData, 0);
        }

        messagesDesired = 1 + Interrupter_GetSecondaryInterrupterCount(PrimaryInterrupterData);
        Interrupter_FilterMessageDescriptors(PrimaryInterrupterData,
                                          IoResourceRequirementsList,
                                          listIndex,
                                          messagesDesired);

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_FilterAddResourceRequirements(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDFIORESREQLIST             IoResourceRequirementsList
    )
/*++

Routine Description:

    This routine will set the affinity for MSI-x resource descriptors, if present
    in the resource requirements list.

    N.B: There is a flag IrqPolicySpreadMessagesAcrossAllProcessors that is supposed to
         affinitize interrupters across processors, but it is not honored by kmdf.
         Hence the need to set the affinities here.

--*/
{
    BOOLEAN     isMSIX;
    ULONG       listIndex;
    ULONG       messageCount;
    NTSTATUS    status;

    PAGED_CODE();

    TRY {

        if (Interrupter_GetSecondaryInterrupterCount(PrimaryInterrupterData) == 0) {
            status = STATUS_SUCCESS;
            LEAVE;
        }

        Interrupter_IoResList_GetMessageCountAndType(IoResourceRequirementsList,
                                                     &listIndex,
                                                     &messageCount,
                                                     &isMSIX);

        NT_ASSERT(isMSIX != FALSE);

        NT_ASSERT(messageCount ==
                  (1 + Interrupter_GetSecondaryInterrupterCount(PrimaryInterrupterData)));

        Interrupter_AffinitizeMSIXDescriptors(PrimaryInterrupterData,
                                              IoResourceRequirementsList,
                                              listIndex);

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Interrupter_PopulateInterrupterLookupTable(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData
    )
/*++

Routine Description:

    Allocate and populate a "processor to interrupter" lookup table based on interrupter
    affinities. The size of the interrupter lookup table is equal to the number of
    logical processors available on the system. If a processor is hot-added, we will
    receive Release/Prepare hardware callbacks, and since this allocation is done in
    PrepareHardware callback, it takes hot-add processors into account.

--*/
{
    KAFFINITY           affinity;
    UCHAR               bitIndex;
    PINTERRUPTER_DATA   interrupterData;
    ULONG               interrupterLookupSize;
    ULONG               processorIndex;
    PROCESSOR_NUMBER    processorNumber;
    USHORT              messageNumber;
    NTSTATUS            status;

    PAGED_CODE();

    TRY {

        //
        // If the lookup table was allocated in a previous PrepareHardware, it must
        // be freed in the corresponding ReleaseHardware.
        //
        NT_ASSERT(PrimaryInterrupterData->InterrupterLookup == NULL);

        PrimaryInterrupterData->ActiveProcessorCount =
            KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);

        interrupterLookupSize = (PrimaryInterrupterData->ActiveProcessorCount * sizeof(USHORT));

        PrimaryInterrupterData->InterrupterLookup =
            ExAllocatePoolWithTag(NonPagedPool, interrupterLookupSize, DriverAllocationTag);

        if (PrimaryInterrupterData->InterrupterLookup == NULL) {

            status = STATUS_INSUFFICIENT_RESOURCES;
            LogError(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                     "Failed to allocate InterrupterLookup buffer");
            LEAVE;
        }

        RtlZeroMemory(PrimaryInterrupterData->InterrupterLookup, interrupterLookupSize);

        for (messageNumber = 1;
             messageNumber < Interrupter_GetInterrupterCount(PrimaryInterrupterData);
             messageNumber += 1) {

            interrupterData = PrimaryInterrupterData->InterrupterDataArray[messageNumber];

            processorNumber.Reserved = 0;
            processorNumber.Group    = interrupterData->Group;

            affinity = interrupterData->Affinity;

            for (bitIndex = 0;
                 ((affinity != 0) && (bitIndex < sizeof(KAFFINITY) * 8));
                 bitIndex += 1) {

                if ((affinity & AFFINITY_MASK(bitIndex)) == 0) {
                    continue;
                }

                affinity &= ~AFFINITY_MASK(bitIndex);

                processorNumber.Number = bitIndex;

                processorIndex = KeGetProcessorIndexFromNumber(&processorNumber);

                NT_ASSERT(processorIndex < interrupterLookupSize);

                NT_ASSERTMSG("Found two interrupters affinitized to the same processor",
                             PrimaryInterrupterData->InterrupterLookup[processorIndex] == 0);

                PrimaryInterrupterData->InterrupterLookup[processorIndex] = messageNumber;
            }
        }

        status = STATUS_SUCCESS;

    } FINALLY {

        //
        // If secondary interrupters are used, each lookup table entry should point to
        // a secondary interrupter, unless
        //
        // [1] The processor belongs to a group other than 0. Secondary interrupters are currently
        //     only affinitized to processors in group 0.
        //
        // [2] The number of secondary interrupters does not evenly divide the number of
        //     logical processors, in which case, the extra processors will have the primary
        //     interrupter as the target.
        //
        // [3] The entry belongs to a processor that was hot-added. This is because affinities
        //     are assigned only once, before start device.
        //
        if (Interrupter_GetInterrupterCount(PrimaryInterrupterData) > 1) {

            for (processorIndex = 0;
                 processorIndex < PrimaryInterrupterData->ActiveProcessorCount;
                 processorIndex += 1) {

                if (PrimaryInterrupterData->InterrupterLookup[processorIndex] != 0) {
                    continue;
                }

                LogInfo(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                        "No secondary interrupter affinitized to processor %u", processorIndex);
            }
        }
    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_IoResList_GetMessageCountAndType(
    __in
        WDFIORESREQLIST IoResourceRequirementsList,
    __out
        PULONG          ListIndex,
    __out
        PULONG          MessageCount,
    __out
        PBOOLEAN        IsMSIX
    )
/*++

Routine Description:

    Count the number of MSI messages in the supplied IO Resource Requirements List.
    The corresponding resource descriptors may be multi-message MSI or MSI-X. Below
    facts will be used to infer whether or not the returned message count is for MSI-X.

    1.  An IO Resource Requirements list has one or more Io Resource lists. One Io Resource
        list can have only one type of MSI resource descriptors.
    2.  If an Io Resource list has multi-message MSI descriptor, it will have only one such
        descriptor, and its message count may be > 1.
    3.  If an Io Resource list has MSI-X descriptors, it will have one such descriptor for
        each message.

Arguments:

    IoResourceRequirementsList - IO Resource Requirements list supplied in
        IRP_MN_FILTER_RESOURCE_REQUIREMENTS.

    ListIndex - The index of the Io Resource List for which the message count is returned.

    MessageCount - Receives the count of MSI messages in one of the IO Resource lists. If
        more than one IO Resource list has MSI descriptors, the count for the list with
        the maximum messages is returned.

    IsMSIX - Receives a boolean indicating whether the returned count is for MSI-X messages.
        This is true if the corresponding IO Resource list has more than one MSI resource
        descriptor.

--*/
{
    PIO_RESOURCE_DESCRIPTOR descriptor;
    ULONG                   descriptorIndex;
    BOOLEAN                 isMSIX;
    BOOLEAN                 isMSIXPrevious;
    ULONG                   listIndex;
    ULONG                   listIndexPrevious;
    ULONG                   messageCount;
    ULONG                   messageCountPrevious;
    WDFIORESLIST            resourceList;

    PAGED_CODE();

    TRY {

        listIndexPrevious    = 0;
        messageCountPrevious = 0;
        isMSIXPrevious       = FALSE;

        for (listIndex = 0;
             listIndex < WdfIoResourceRequirementsListGetCount(IoResourceRequirementsList);
             listIndex += 1) {

            resourceList = WdfIoResourceRequirementsListGetIoResList(IoResourceRequirementsList, listIndex);

            messageCount = 0;
            isMSIX = FALSE;

            for (descriptorIndex = 0;
                 descriptorIndex < WdfIoResourceListGetCount(resourceList);
                 descriptorIndex += 1) {

                descriptor = WdfIoResourceListGetDescriptor(resourceList, descriptorIndex);

                if (IsIoResourceDescriptorMSI(descriptor) == FALSE) {
                    continue;
                }

                if (descriptor->Option & IO_RESOURCE_ALTERNATIVE) {
                    continue;
                }

                //
                // If this list has more than one MSI descriptor, then descriptor type is MSI-X.
                //
                if (messageCount != 0) {
                    isMSIX = TRUE;
                }

                //
                // For MSI-X, MaximumVector = MinimumVector, always. That is, only an MSI 2.2
                // descriptor can have messageCount > 1.
                //
                messageCount += (1 +
                                 descriptor->u.Interrupt.MaximumVector -
                                 descriptor->u.Interrupt.MinimumVector);
            }

            //
            // In case of multiple Io Resource lists with MSI descriptors, these are the tie-breakers:
            // 1. MSI-X takes precedence over multi-message MSI.
            // 2. Bigger message count takes precedence.
            //
            if (isMSIX) {

                if (!isMSIXPrevious || (messageCount > messageCountPrevious)) {

                    isMSIXPrevious       = TRUE;
                    messageCountPrevious = messageCount;
                    listIndexPrevious    = listIndex;
                }

            } else {

                if (!isMSIXPrevious && (messageCount > messageCountPrevious)) {

                    messageCountPrevious = messageCount;
                    listIndexPrevious    = listIndex;
                }
            }
        }

    } FINALLY {

        *ListIndex    = listIndexPrevious;
        *MessageCount = messageCountPrevious;
        *IsMSIX       = isMSIXPrevious;
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Interrupter_CmResList_GetInterrupterCountAndType(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        WDFCMRESLIST                WdfResourcesRaw,
    __out
        PULONG                      InterruptCount,
    __out
        InterruptMechanism*         Mechanism
    )
/*++

Routine Description:

    This routine calculates the number of interrupt resources assigned by the system
    to this device. Also returns the interrupt mechanism.

    N.B: For MSI/MSI-X, if the system could not grant the number of messages requested
         in the Io Resource Requirements List, it will only grant one message.

--*/
{
    PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor;
    ULONG                           descriptorIndex;
    ULONG                           localInterruptCount;
    InterruptMechanism              mechanism;

    PAGED_CODE();

    TRY {

        localInterruptCount = 0;
        mechanism = InterruptMechanism_LineBased;

        //
        // Find the interrupt resources the system assigned for the controller.
        //
        for (descriptorIndex = 0;
             descriptorIndex < WdfCmResourceListGetCount(WdfResourcesRaw);
             descriptorIndex++) {

            descriptor = WdfCmResourceListGetDescriptor(WdfResourcesRaw, descriptorIndex);

            if (descriptor->Type != CmResourceTypeInterrupt) {
                continue;
            }

            LogInfo(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                    "Processing Resource %d: CmResourceTypeInterrupt Flags 0x%x",
                    descriptorIndex, descriptor->Flags);

            if (IsCmResourceDescriptorMSI(descriptor)) {

                LogInfo(Controller_GetRecorderLog(PrimaryInterrupterData->ControllerHandle), Interrupter,
                        "MSI(X) Interrupt: MessageCount %d Vector 0x%x",
                        descriptor->u.MessageInterrupt.Raw.MessageCount,
                        descriptor->u.MessageInterrupt.Raw.Vector);

                mechanism = InterruptMechanism_MultiMessageMSI;

                //
                // More than one MSI descriptor in a single resource list implies MSI-X.
                //
                if (localInterruptCount != 0) {
                    mechanism = InterruptMechanism_MSIX;
                }

                localInterruptCount += descriptor->u.MessageInterrupt.Raw.MessageCount;

            } else {

                localInterruptCount += 1;
            }
        }

    } FINALLY {

        *InterruptCount = localInterruptCount;
        *Mechanism = mechanism;
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Interrupter_FlushEvent(
    __in
        PPRIMARY_INTERRUPTER_DATA   PrimaryInterrupterData,
    __in
        USHORT                      InterrupterTarget
    )
/*++

Routine Description:

    This routine will ensure that if a DPC is currently processing events from
    the specified event ring, the current event is fully processed, before returning.
    The intent is to synchronize the processing of transfer event completion event
    on a secondary event ring with that of any endpoint-related command completion
    event on the primary event ring. This is only needed when secondary event rings
    are in use.

--*/
{
    PINTERRUPTER_DATA   interrupterData;
    ULONG               previousDequeueIndex;

    TRY {

        if (InterrupterTarget == 0) {
            LEAVE;
        }

        interrupterData = PrimaryInterrupterData->InterrupterDataArray[InterrupterTarget];

        //
        // Done if there is no Dpc currently executing.
        //
        if (interrupterData->Flags.DpcRunning == 0) {
            LEAVE;
        }

        previousDequeueIndex = interrupterData->DequeueIndex;

        //
        // Loop until the current Dpc finishes execution, or the DequeueIndex
        // changes (increments).
        //
        while (interrupterData->DequeueIndex == previousDequeueIndex) {

            if (interrupterData->Flags.DpcRunning == 0) {
                LEAVE;
            }

            KeStallExecutionProcessor(10);
        }

    } FINALLY {

    }

    return;
}
