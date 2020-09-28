
/*++

Copyright (c) Microsoft Corporation

Module Name:

    hsmmux.c

Abstract:

    Support functions for HSM's "middle layer" which multiplexes all of the
    events to and from the different DSMs and PSMs.

Author:



Environment:

    Kernel mode only.

--*/

#include "pch.h"
#include "hsmmux.tmh"

const CHAR RegistrationTag[]        = "DSM Registration Tag";
const CHAR SurpriseRemoveTag[]      = "DSM Surprise Remove Tag";
const CHAR PnPPowerTag[]            = "DSM PnPPower Tag";
const CHAR PsmTag[]                 = "PSM Tag";



__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HSMMUX_InitializeHSMMuxContext (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This routine initializes the HSM's Mux context

Return Value:

    VOID

--*/
{
    PHSM_MUX_CONTEXT    muxContext;

    TRY {

        muxContext = &HubFdoContext->MuxContext;

        RtlZeroMemory(muxContext, sizeof(HSM_MUX_CONTEXT));

        InitializeListHead(&muxContext->PSMListHead);
        InitializeListHead(&muxContext->DSMListHead);

        KeInitializeSpinLock(&muxContext->MuxLock);

    } FINALLY {

    }

}


extern PSTATE_ENTRY PSM20StateTable[];
extern PSTATE_ENTRY PSM30StateTable[];
EVT_WDF_TIMER  HUBMISC_PsmResetCompletePollingTimer;

WDFOBJECT
HUBMUX_CreatePSM (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext,
    __in
        PPORT_INFORMATION       PortInformation
    )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Allocates and initializes memory for child PSMs

Arguments

Return Value:

    WDFOBJECT handle to a newly created and initialized WDFOBJECT
        representing the PSM, otherwise NULL

--*/
{
    WDFOBJECT                   psmObject;
    PPORT_CONTEXT               portContext;
    WDF_TIMER_CONFIG            timerConfig;
    WDF_OBJECT_ATTRIBUTES       objectAttributes;
    NTSTATUS                    status;
    RECORDER_LOG_CREATE_PARAMS  recorderLogCreateParams;

    TRY {

        portContext = NULL;

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&objectAttributes,
                                                PORT_CONTEXT);

        objectAttributes.ParentObject = HubFdoContext->HubFdoDevice;
        objectAttributes.ExecutionLevel = WdfExecutionLevelPassive;
        objectAttributes.EvtCleanupCallback = HUBMISC_EvtPsmCleanup;
        objectAttributes.EvtDestroyCallback = HUBMISC_EvtPsmDestroy;

        status = WdfObjectCreate(&objectAttributes,
                                 &psmObject);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(HubFdoContext->IfrLog, TL_ERROR, Port, "WdfObjectCreate for Port Failed %!STATUS!", status);
            psmObject = NULL;
            LEAVE;

        }

        //
        // Even though HSM is a parent of PSM, it is possible that HSM
        // might go away before PSM if DSM happened to keep a reference
        // on PSM. Therefore we take an explicit reference on HSM here and
        // release it only when PSM is being destroyed. Note that after
        // this point, even if anything fails, we are still guaranteed
        // to get the destroy callback and therefore will not leak any
        // reference.
        //
        WdfObjectReferenceWithTag(WdfObjectContextGetObject(HubFdoContext),
                                  (PVOID) PsmTag);

        portContext = GetPortContext(psmObject);

        RtlZeroMemory(portContext, sizeof(PORT_CONTEXT));

        portContext->HubFdoContext = HubFdoContext;
        portContext->PortInformation = *PortInformation;

        //
        // Create a recorder log for the port.  If this fails then the default log will be
        // be used instead.
        //
        RECORDER_LOG_CREATE_PARAMS_INIT_APPEND_POINTER(&recorderLogCreateParams,
                                                       NULL,
                                                       portContext);

        recorderLogCreateParams.TotalBufferSize = 1024;
        recorderLogCreateParams.ErrorPartitionSize = 512;

        RtlStringCchPrintfA(recorderLogCreateParams.LogIdentifier,
                            sizeof(recorderLogCreateParams.LogIdentifier),
                            "hub%d port%d",
                            HubFdoContext->HubInformation.HubNumber,
                            PortInformation->PortNumber);

        status = WppRecorderLogCreate(&recorderLogCreateParams, &portContext->IfrLog);

        if (!NT_SUCCESS(status)) {
            portContext->IfrLog = HubFdoContext->IfrLog;
        }

        //
        // Create our pre-allocated request for port control transfers
        //
        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
        objectAttributes.ParentObject = psmObject;

        status = WdfRequestCreate(&objectAttributes,
                                  WdfDeviceGetIoTarget(HubFdoContext->HubFdoDevice),
                                  &portContext->PortControlRequest.WdfRequest);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(portContext->IfrLog, TL_ERROR, Port, "WdfRequestCreate for Port Control Transfer Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // Store the IRP for debugger extension
        //
        portContext->PortControlRequest.Irp =
            WdfRequestWdmGetIrp(portContext->PortControlRequest.WdfRequest);



        KeInitializeSpinLock(&portContext->PsmContext.StateMachineLock);


        //
        // Set initial state for PSM
        //
        portContext->PsmContext.CurrentStateDepth = 0;
        portContext->PsmContext.SubSmFlags = 0;
        portContext->PsmContext.ParentContext = portContext;
        portContext->PsmContext.AddEvent = HUBSM_AddPsm20Event;
        portContext->PsmContext.ParentHandle = portContext->HubFdoContext->HubParentInterface.Hub;
        portContext->PsmContext.HubFdoContext = portContext->HubFdoContext;

        //
        // Create the PSM timer
        //
        portContext->PsmContext.Timer =
            ExAllocateTimer(HUBMISC_PsmEventTimer,
                            portContext,
                            EX_TIMER_HIGH_RESOLUTION);

        if (portContext->PsmContext.Timer == NULL) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "ExAllocateTimer Failed");
            LEAVE;
        }

        //
        // Create the PSM timer for reset completion polling, if needed
        //
        if (HubFdoContext->HubInformation.HubFlags.AddDelayAfterResetComplete == 1) {
            WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
            objectAttributes.ParentObject = psmObject;

            WDF_TIMER_CONFIG_INIT( &timerConfig,
                                   HUBMISC_PsmResetCompletePollingTimer);

            status = WdfTimerCreate( &timerConfig,
                                     &objectAttributes,
                                     &portContext->ResetPollingTimer);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(portContext->IfrLog, TL_ERROR, Port, "WdfTimerCreate Failed %!STATUS!", status);
                LEAVE;
            }
        }

        //
        // Create the worker item for the state machine that will be used to
        // execute state entry functions that require passive level. To
        // ensure forward progress for boot devices, we shall use a special
        // type of worker items, implemented by UCX
        //
        portContext->PsmContext.SmWorker =
            HUBMISC_AllocateWorkerItem(portContext->HubFdoContext,
                                       UCX_FORWARD_PROGRESS_WORKITEM_FLAG_NEED_FLUSH_FUNCTIONALITY);

        if (portContext->PsmContext.SmWorker == NULL) {

            DbgTraceTo(portContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "HUBMISC_AllocateWorkerItem Failed");
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }



    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (psmObject != NULL) {
                WdfObjectDelete(psmObject);
                psmObject = NULL;
            }

        } else {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_INFO,
                       Hub,
                       "Hub 0x%p created a child port 0x%p for its port number %d",
                       HubFdoContext,
                       portContext,
                       PortInformation->PortNumber);

        }

    }

    return psmObject;

}


HSM_EVENT
HUBMUX_CreateChildPSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Allocates and initializes memory for child PSMs

Return Value:

    HsmEventOperationSuccess
    HsmEventOperationFailure

--*/
{
    USHORT                  i;
    USHORT                  highestPortNumber;
    PPSM_MUX_CONTEXT        psmContext;
    PPORT_CONTEXT           portContext;
    WDFOBJECT               psmObject;
    BOOLEAN                 cleanupPSMs;
    HSM_EVENT               result;
    PORT_INFORMATION        portInfo;
    WDF_OBJECT_ATTRIBUTES   attributes;
    WDFMEMORY               statusChangeBitMapMemory;
    NTSTATUS                status;
    USHORT                  sizeBasedOnPortNumber;

    TRY {

        cleanupPSMs = FALSE;

        //
        // Account for a case when NumberOf20Ports == 0 to make sure that
        // the condition First20PortNumber <= Last20PortNumber used elsewhere is always false
        //
        HubFdoContext->HubInformation.First20PortNumber = USHORT_MAX;
        HubFdoContext->HubInformation.Last20PortNumber = 0;

        for (i = 0; i < HubFdoContext->HubInformation.NumberOf20Ports; i++) {

            RtlZeroMemory(&portInfo, sizeof(portInfo));

            //
            // Assume devices attached to the port are removable
            //
            INTERLOCKED_SET_BIT_FIELD(PORT_FLAGS, &portInfo.PortFlags, PortFlagRemovable);

            if (HubFdoContext->HubParentInterface.HubDepth == 0) {
                PROOTHUB_20PORT_INFO rootPortInfo;

                //
                // For root 2.0 ports extract the port information from the UCX ROOTHUB_20PORT_INFO
                // structure
                //
                rootPortInfo = HUBMISC_GetRootHub20PortInfo(HubFdoContext, i);

                portInfo.PortNumber = rootPortInfo->PortNumber;

                if (rootPortInfo->Removable == TriStateFalse) {
                    INTERLOCKED_CLEAR_BIT_FIELD(PORT_FLAGS, &portInfo.PortFlags, PortFlagRemovable);
                }

                if (rootPortInfo->IntegratedHubImplemented == TriStateTrue) {
                    INTERLOCKED_SET_BIT_FIELD(PORT_FLAGS, &portInfo.PortFlags, PortFlagIntegratedHubImplemented);
                }

                portInfo.PortFlags.PortFlagUsb20HardwareLpmSupported =
                    rootPortInfo->ControllerUsb20HardwareLpmFlags.L1CapabilitySupported;

                portInfo.PortFlags.PortFlagUsb20HardwareLpmBeslCapabilitySupported =
                    rootPortInfo->ControllerUsb20HardwareLpmFlags.BeslLpmCapabilitySupported;

                portInfo.TotalHubDepth = rootPortInfo->HubDepth;

                //
                // Set default hub depth if the depth is unknown or too big
                //
                if ((portInfo.TotalHubDepth == 0) || (portInfo.TotalHubDepth > 5)) {
                    portInfo.TotalHubDepth = 5;
                }

            } else {

                //
                // External hubs port numbers are 1-based and sequential
                //
                portInfo.PortNumber = i + 1;

                if (HUBMISC_IsBitSet(
                        &HubFdoContext->HubDescriptor.Hub20Descriptor.bRemoveAndPowerMask,
                        portInfo.PortNumber)) {

                    INTERLOCKED_CLEAR_BIT_FIELD(PORT_FLAGS, &portInfo.PortFlags, PortFlagRemovable);
                }

                portInfo.TotalHubDepth = HubFdoContext->HubInfoFromParent.TotalHubDepth;
            }

            if (portInfo.PortNumber > HubFdoContext->HubInformation.Last20PortNumber) {

                HubFdoContext->HubInformation.Last20PortNumber = portInfo.PortNumber;

            }

            if (portInfo.PortNumber < HubFdoContext->HubInformation.First20PortNumber) {

                HubFdoContext->HubInformation.First20PortNumber = portInfo.PortNumber;

            }

            portInfo.PortProtocol = PortProtocolUsb20;

            psmObject = HUBMUX_CreatePSM(HubFdoContext,
                                         &portInfo);


            if (psmObject == NULL) {

                //
                // We are going to need to clean up the already allocated PSMs
                //
                cleanupPSMs = TRUE;
                result = HsmEventOperationFailure;
                LEAVE;

            }

            portContext = GetPortContext(psmObject);

            //
            // Set it up so that this object uses PSM20
            //
            portContext->PsmContext.CurrentState[0].Psm20State = StartIndexPsm20;
            portContext->PsmContext.StateTable = PSM20StateTable;
            portContext->PsmContext.StartIndex = StartIndexPsm20;

            InsertTailList(&HubFdoContext->MuxContext.PSMListHead,
                           &portContext->MuxContext.ListEntry);

            HubFdoContext->MuxContext.NumberOfPSMs++;

        }

        //
        // Account for a case when NumberOf30Ports == 0 to make sure that
        // the condition First30PortNumber <= Last30PortNumber used elsewhere is always false
        //
        HubFdoContext->HubInformation.First30PortNumber = USHORT_MAX;
        HubFdoContext->HubInformation.Last30PortNumber = 0;

		for (i = 0; i < HubFdoContext->HubInformation.NumberOf30Ports; i++) {

            RtlZeroMemory(&portInfo, sizeof(portInfo));

            //
            // Assume devices attached to the port are removable
            //
            INTERLOCKED_SET_BIT_FIELD(PORT_FLAGS, &portInfo.PortFlags, PortFlagRemovable);

            if (HubFdoContext->HubParentInterface.HubDepth == 0) {
                PROOTHUB_30PORT_INFO_EX rootPortInfo;

                //
                // For root 3.0 ports extract the port information from the UCX ROOTHUB_30PORT_INFO
                // structure
                //
                rootPortInfo = HUBMISC_GetRootHub30PortInfo(HubFdoContext, i);

                portInfo.PortNumber = rootPortInfo->PortNumber;

                if (rootPortInfo->Removable == TriStateFalse) {
                    INTERLOCKED_CLEAR_BIT_FIELD(PORT_FLAGS, &portInfo.PortFlags, PortFlagRemovable);
                }

                if (rootPortInfo->DebugCapable == TriStateTrue) {
                    INTERLOCKED_SET_BIT_FIELD(PORT_FLAGS, &portInfo.PortFlags, PortFlagDebugCapable);
                }

                //
                // For root ports, we always support EXT_PORT_STATUS in Get Port Status
                // (Enhanced SuperSpeed status), as the xHCI driver provides
                // corresponding support for any 3.0/3.1 (SS/SSP) ports
                //
                INTERLOCKED_SET_BIT_FIELD(PORT_FLAGS, &portInfo.PortFlags, PortFlagEnhancedSuperSpeed);

                //
                // Allow SuperSpeedPlusIsoch companion descriptors for 3.1+ ports only
                //
                if (rootPortInfo->MinorRevision >= 1) {
                    portInfo.SuperSpeedPlusIsochBurstCount = HubFdoContext->HubInformation.SuperSpeedPlusIsochBurstCount;
                }

                portInfo.SublinkSpeedAttr = rootPortInfo->Speeds;
                portInfo.SublinkSpeedAttrCount = rootPortInfo->SpeedsCount;
                portInfo.TotalHubDepth = rootPortInfo->HubDepth;

                //
                // Set default hub depth if the depth is unknown or too big
                //
                if ((portInfo.TotalHubDepth == 0) || (portInfo.TotalHubDepth > 5)) {
                    portInfo.TotalHubDepth = 5;
                }

            } else {

                //
                // External hubs port numbers are 1-based and sequential
                //
                portInfo.PortNumber = i + 1;

                //
                // USB 3.0 hub descriptor format
                // USB 3.0 Spec, 10.13.2.1
                //
                if (HUBMISC_IsBitSet(
                        &HubFdoContext->HubDescriptor.Hub30Descriptor.DeviceRemovable,
                        portInfo.PortNumber)) {

                    INTERLOCKED_CLEAR_BIT_FIELD(PORT_FLAGS, &portInfo.PortFlags, PortFlagRemovable);
                }

                if (HubFdoContext->HubParentInterface.IsEnhancedSuperSpeed) {
                    INTERLOCKED_SET_BIT_FIELD(PORT_FLAGS, &portInfo.PortFlags, PortFlagEnhancedSuperSpeed);
                    portInfo.SuperSpeedPlusIsochBurstCount = HubFdoContext->HubInformation.SuperSpeedPlusIsochBurstCount;
                }

                portInfo.SublinkSpeedAttr = HubFdoContext->HubInfoFromParent.SublinkSpeedAttr;
                portInfo.SublinkSpeedAttrCount = HubFdoContext->HubInfoFromParent.SublinkSpeedAttrCount;
                portInfo.TotalHubDepth = HubFdoContext->HubInfoFromParent.TotalHubDepth;
            }

            if (portInfo.PortNumber > HubFdoContext->HubInformation.Last30PortNumber) {

                HubFdoContext->HubInformation.Last30PortNumber = portInfo.PortNumber;

            }

            if (portInfo.PortNumber < HubFdoContext->HubInformation.First30PortNumber) {

                HubFdoContext->HubInformation.First30PortNumber = portInfo.PortNumber;

            }

            portInfo.PortProtocol = PortProtocolUsb30;

            psmObject = HUBMUX_CreatePSM(HubFdoContext,
                                         &portInfo);

            if (psmObject == NULL) {
                //
                // We are going to need to clean up the already allocated PSMs
                //
                cleanupPSMs = TRUE;
                result = HsmEventOperationFailure;
                LEAVE;
            }
            portContext = GetPortContext(psmObject);

            //
            // Set it up so that this object uses PSM30
            //
            portContext->PsmContext.CurrentState[0].Psm30State = StartIndexPsm30;
            portContext->PsmContext.StateTable = PSM30StateTable;
            portContext->PsmContext.StartIndex = StartIndexPsm30;

            InsertTailList(&HubFdoContext->MuxContext.PSMListHead,
                           &portContext->MuxContext.ListEntry);
            HubFdoContext->MuxContext.NumberOfPSMs++;

        }

        highestPortNumber = max(HubFdoContext->HubInformation.Last20PortNumber,
                                HubFdoContext->HubInformation.Last30PortNumber);

        HubFdoContext->HubInformation.NumberOfPorts = highestPortNumber;

        NT_ASSERT(highestPortNumber != 0);

        sizeBasedOnPortNumber =   max(sizeof(USHORT),
                                      (highestPortNumber / 8) + sizeof(UCHAR));


        //
        // For non root hubs, we use InterruptPipeMaxPacketSize as the TransferSize.
        // So we need to make sure the buffer is not less than InterruptPipeMaxPacketSize
        //
        HubFdoContext->StatusChangeInterruptRequest.MaxStatusChangeBitMapSize =
            max(sizeBasedOnPortNumber,
                HubFdoContext->HubInformation.InterruptPipeMaxPacketSize);

        //
        // Allocate and initialize the status change bitmap
        //
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = HubFdoContext->HubFdoDevice;

        //
        // Rtl bitmap functions require the bitmap to be ULONG aligned.  This code
        // relies on WdfMemoryCreate returning memory that is at least ULONG aligned.
        //
        status = WdfMemoryCreate(&attributes,
                                 NonPagedPool,
                                 USBHUB3_HUB_TAG,
                                 WDF_ALIGN_SIZE_UP(HubFdoContext->StatusChangeInterruptRequest.MaxStatusChangeBitMapSize,
                                                   sizeof(ULONG)),
                                 &statusChangeBitMapMemory,
                                 &HubFdoContext->StatusChangeInterruptRequest.StatusChangeBitMap);

        if (!NT_SUCCESS(status)) {
            result = HsmEventOperationFailure;
            cleanupPSMs = TRUE;
            LEAVE;
        }

        result = HsmEventOperationSuccess;

        INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &HubFdoContext->HubInformation.HubFlags, Configured);

    } FINALLY {

        if (cleanupPSMs) {

            while(!IsListEmpty(&HubFdoContext->MuxContext.PSMListHead)) {

                psmContext = CONTAINING_RECORD(RemoveHeadList(&HubFdoContext->MuxContext.PSMListHead),
                                               PSM_MUX_CONTEXT,
                                               ListEntry);
                portContext = CONTAINING_RECORD(psmContext,
                                                PORT_CONTEXT,
                                                MuxContext);

                HubFdoContext->MuxContext.NumberOfPSMs--;

                //
                // Actually delete the object representing the PSM
                //
                WdfObjectDelete(WdfObjectContextGetObject(portContext));

            }

        }

    }

    return result;

}



VOID
HUBMUX_QueuePowerUpEventToPSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext,
    __in
        PSM_EVENT               PowerUpEvent
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Queues a HubStart event to all PSMs, and also enables the PSMs to
    acquire interrupt references.

Return Value:

    VOID

--*/
{
    KIRQL               irql;
    PHSM_MUX_CONTEXT    hsmMuxContext;
    PPORT_CONTEXT       currentPort;
    BOOLEAN             completeHere;

    TRY {

        hsmMuxContext = &HubFdoContext->MuxContext;
        completeHere = FALSE;

        //
        // We need to now allow the PSMs to acquire interrupt references
        // again, if it was previously disabled.
        //
        KeAcquireSpinLock(&HubFdoContext->MuxContext.MuxLock,
                          &irql);

        INTERLOCKED_CLEAR_BIT_FIELD(HSM_MUX_FLAGS, &hsmMuxContext->HubMuxFlags, FailDsmRegistration);

        INTERLOCKED_CLEAR_BIT_FIELD(HSM_MUX_FLAGS, &HubFdoContext->MuxContext.HubMuxFlags, FailInterruptReferenceAcquired);

        KeReleaseSpinLock(&HubFdoContext->MuxContext.MuxLock,
                          irql);

        NT_ASSERT(hsmMuxContext->OutstandingPnPOperations == 0);

        //
        // If for some reason there aren't any PSMs, then we
        // will complete this here
        //
        if (hsmMuxContext->NumberOfPSMs == 0) {

            completeHere = TRUE;
            LEAVE;

        }

        //
        // There shouldn't be any PSMs that are holding power
        // references at this point, so the number of pending operations
        // will be the number of PSMs
        //
        hsmMuxContext->OutstandingPnPOperations = hsmMuxContext->NumberOfPSMs;

        //
        // Now we can send the events
        //
        FOR_ALL_IN_LIST(PORT_CONTEXT,
                        &HubFdoContext->MuxContext.PSMListHead,
                        MuxContext.ListEntry,
                        currentPort) {

            NT_ASSERT(currentPort->MuxContext.PortFlags.PowerReferenceHeld == 0);
            NT_ASSERT(currentPort->MuxContext.PortFlags.PnpOperationPending == 0);
            INTERLOCKED_SET_BIT_FIELD(PORT_MUX_FLAGS, &currentPort->MuxContext.PortFlags, PnpOperationPending);

            currentPort->PsmContext.AddEvent(currentPort,
                                             PowerUpEvent);

        }

    } FINALLY {

        if (completeHere) {
            HUBSM_AddHsmEvent(HubFdoContext,
                              HsmEventPortPowerReferencesAcquired);
        }

    }
}

VOID
HUBMUX_QueueStartToAllPSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Queues a HubStart event to all PSMs, and also enables the PSMs to
    acquire interrupt references.

Return Value:

    VOID

--*/
{
    INTERLOCKED_CLEAR_BIT_FIELD(HSM_MUX_FLAGS, &HubFdoContext->MuxContext.HubMuxFlags, HubResetInProgress);

    HUBMUX_QueuePowerUpEventToPSMs(HubFdoContext,
                                   PsmEventHubStart);
}

VOID
HUBMUX_QueueResumeToAllPSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Queues a HubResume event to all PSMs, and also enables the PSMs to
    acquire interrupt references.

Return Value:

    VOID

--*/
{

    HUBMUX_QueuePowerUpEventToPSMs(HubFdoContext,
                                   PsmEventHubResume);

}

VOID
HUBMUX_QueueResumeInS0ToAllPSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Queues a HubResume event to all PSMs, and also enables the PSMs to
    acquire interrupt references.

Return Value:

    VOID

--*/
{
    HUBMUX_QueuePowerUpEventToPSMs(HubFdoContext,
                                   PsmEventHubResumeInS0);

}


VOID
HUBMUX_QueueResumeWithResetToAllPSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Queues a HubResumeWithReset to all PSMs, and also enables the PSMs to
    acquire interrupt references.

Return Value:

    VOID

--*/
{
    HUBMUX_QueuePowerUpEventToPSMs(HubFdoContext,
                                   PsmEventHubResumeWithReset);

}

VOID
HUBMUX_QueueStopToAllPSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Queues a HubStop event to All PSMs.

Return Value:

    VOID

--*/
{
    PHSM_MUX_CONTEXT    hsmMuxContext;
    PPORT_CONTEXT       currentPort;
    BOOLEAN             completeHere = FALSE;
    ULONG               pendingCount;
    ULONG               pendingPSMs;

    TRY {

        hsmMuxContext = &HubFdoContext->MuxContext;

        NT_ASSERT(hsmMuxContext->OutstandingPnPOperations == 0);

        //
        // If for some reason there aren't any PSMs, then we
        // will complete this here
        //
        if (hsmMuxContext->NumberOfPSMs == 0) {
            completeHere = TRUE;
            LEAVE;
        }

        //
        // There may not be any PSMs that are actually holding power references
        // in which case there is no acknowledgement of this event from the PSM
        // So the number outstanding would be the number of power references
        // held.
        //
        pendingCount = hsmMuxContext->NumberOfPowerReferencesHeld;
        hsmMuxContext->OutstandingPnPOperations = pendingCount;
        if (pendingCount == 0) {

            //
            // In this case, ReleasePowerReference will never be called, so
            // we need to generate the PowerReferencesReleased event to unblock
            // the HSM
            //
            completeHere = TRUE;

        }

        //
        // Now we can actually send the events
        //
        pendingPSMs = 0;
        FOR_ALL_IN_LIST(PORT_CONTEXT,
                        &HubFdoContext->MuxContext.PSMListHead,
                        MuxContext.ListEntry,
                        currentPort) {

            NT_ASSERT(currentPort->MuxContext.PortFlags.PnpOperationPending == 0);
            if (currentPort->MuxContext.PortFlags.PowerReferenceHeld == 1) {
                //
                // Lets keep track of these so that we can verify that this count
                // matches what we calculated before
                //
                pendingPSMs++;
                INTERLOCKED_SET_BIT_FIELD(PORT_MUX_FLAGS, &currentPort->MuxContext.PortFlags, PnpOperationPending);
            }

            currentPort->PsmContext.AddEvent(currentPort,
                                             PsmEventHubStop);
        }

        NT_ASSERT(pendingCount == pendingPSMs);

    } FINALLY {

        if (completeHere) {
            HUBSM_AddHsmEvent(HubFdoContext,
                              HsmEventPortPowerReferencesReleased);
        }

    }

}


VOID
HUBMUX_QueueSuspendToAllPSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Queues a HubSuspend event to All PSMs.

Return Value:

    VOID

--*/
{
    PHSM_MUX_CONTEXT    hsmMuxContext;
    PPORT_CONTEXT       currentPort;
    BOOLEAN             completeHere;

    TRY {

        completeHere = FALSE;

        hsmMuxContext = &HubFdoContext->MuxContext;

        NT_ASSERT(hsmMuxContext->OutstandingPnPOperations == 0);
        NT_ASSERT(hsmMuxContext->NumberOfPowerReferencesHeld == hsmMuxContext->NumberOfPSMs);

        //
        // If for some reason there aren't any PSMs, then we
        // will complete this here
        //
        if (hsmMuxContext->NumberOfPSMs == 0) {
            completeHere = TRUE;
            LEAVE;
        }

        //
        // All of the PSMs should be holding power references at this point,
        // so the number of pending operations will be the number of PSMs
        //
        hsmMuxContext->OutstandingPnPOperations = hsmMuxContext->NumberOfPSMs;

        //
        // Now we can send the events
        //
        FOR_ALL_IN_LIST(PORT_CONTEXT,
                        &HubFdoContext->MuxContext.PSMListHead,
                        MuxContext.ListEntry,
                        currentPort) {

            NT_ASSERT(currentPort->MuxContext.PortFlags.PowerReferenceHeld == 1);
            NT_ASSERT(currentPort->MuxContext.PortFlags.PnpOperationPending == 0);
            INTERLOCKED_SET_BIT_FIELD(PORT_MUX_FLAGS, &currentPort->MuxContext.PortFlags, PnpOperationPending);

            currentPort->PsmContext.AddEvent(currentPort,
                                             PsmEventHubSuspend);

        }

    } FINALLY {

        if (completeHere) {
            HUBSM_AddHsmEvent(HubFdoContext,
                              HsmEventPortPowerReferencesReleased);
        }

    }

}

VOID
HUBMUX_AcquirePortPowerReference (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Acquires  a power reference in HSM.  After Middle layer has queued
    HsmEventHubStart, HsmEventHubResume or HsmEventHubResumeWithReset,
    it keeps track of the PSMs that have or have not acquired power reference.
    If all the ports have acquired power references, then it queue a
    HsmEventPowerReferencesAcquired into HSM.

Return Value:

    VOID

--*/
{
    PHUB_FDO_CONTEXT    hubFdoContext;
    PPSM_MUX_CONTEXT    psmMuxContext;
    PHSM_MUX_CONTEXT    hsmMuxContext;
    ULONG               pendingPnpOperations;

    TRY {

        hubFdoContext = PortContext->HubFdoContext;
        psmMuxContext = &PortContext->MuxContext;
        hsmMuxContext = &hubFdoContext->MuxContext;

        NT_ASSERT(psmMuxContext->PortFlags.PnpOperationPending == 1);
        NT_ASSERT(psmMuxContext->PortFlags.PowerReferenceHeld == 0);

        INTERLOCKED_CLEAR_BIT_FIELD(PORT_MUX_FLAGS, &psmMuxContext->PortFlags, PnpOperationPending);
        INTERLOCKED_SET_BIT_FIELD(PORT_MUX_FLAGS, &psmMuxContext->PortFlags, PowerReferenceHeld);

        InterlockedIncrementULONG(&hsmMuxContext->NumberOfPowerReferencesHeld);
        pendingPnpOperations = InterlockedDecrementULONG(&hsmMuxContext->OutstandingPnPOperations);

        if (pendingPnpOperations == 0) {

            //
            // This is the last one, so lets notify the HSM
            //
            NT_ASSERT(hsmMuxContext->NumberOfPowerReferencesHeld == hsmMuxContext->NumberOfPSMs);

            HUBSM_AddHsmEvent(hubFdoContext,
                              HsmEventPortPowerReferencesAcquired);

        }

    } FINALLY {

    }

}


VOID
HUBMUX_ReleasePortPowerReference (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Release power reference of this port to HSM. After Middle layer has
    queued HsmEventHubStop,  HsmEventHubSuspend tp PSMs, it keeps track of the
    PSMs that have or have not released power reference. If all the ports have
    acquired power references, then it queue a HsmEventPowerReferencesReleased
    into HSM.

Return Value:

    VOID

--*/
{
    PHUB_FDO_CONTEXT    hubFdoContext;
    PPSM_MUX_CONTEXT    psmMuxContext;
    PHSM_MUX_CONTEXT    hsmMuxContext;
    ULONG               pendingPnpOperations;

    TRY {

        hubFdoContext = PortContext->HubFdoContext;
        psmMuxContext = &PortContext->MuxContext;
        hsmMuxContext = &hubFdoContext->MuxContext;

        NT_ASSERT(psmMuxContext->PortFlags.PnpOperationPending == 1);
        NT_ASSERT(psmMuxContext->PortFlags.PowerReferenceHeld == 1);

        INTERLOCKED_CLEAR_BIT_FIELD(PORT_MUX_FLAGS, &psmMuxContext->PortFlags, PnpOperationPending);
        INTERLOCKED_CLEAR_BIT_FIELD(PORT_MUX_FLAGS, &psmMuxContext->PortFlags, PowerReferenceHeld);

        InterlockedDecrementULONG(&hsmMuxContext->NumberOfPowerReferencesHeld);
        pendingPnpOperations = InterlockedDecrementULONG(&hsmMuxContext->OutstandingPnPOperations);

        if (pendingPnpOperations == 0) {

            //
            // This is the last one, so lets notify the HSM
            //
            NT_ASSERT(hsmMuxContext->NumberOfPowerReferencesHeld == 0);

            HUBSM_AddHsmEvent(hubFdoContext,
                              HsmEventPortPowerReferencesReleased);

        }
        //
        // In case a power reference was acquired because of a pending operation
        // in the controller and was never released, this is a good time to
        // clean it up
        //
        if (PortContext->PortFlags.PendingPowerReferenceOnHub == 1) {

            WdfDeviceResumeIdle(PortContext->HubFdoContext->HubFdoDevice);
            INTERLOCKED_CLEAR_BIT_FIELD(USB_PORT_FLAGS, &PortContext->PortFlags, PendingPowerReferenceOnHub);
        }                                                                                                    

    } FINALLY {

    }

}


VOID
HUBMUX_ReleaseResetReference (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)


Return Value:

    VOID

--*/
{
    PHUB_FDO_CONTEXT    hubFdoContext;
    PPSM_MUX_CONTEXT    psmMuxContext;
    PHSM_MUX_CONTEXT    hsmMuxContext;
    ULONG               pendingResetOperations;

    TRY {

        hubFdoContext = PortContext->HubFdoContext;
        psmMuxContext = &PortContext->MuxContext;
        hsmMuxContext = &hubFdoContext->MuxContext;

        INTERLOCKED_CLEAR_BIT_FIELD(PORT_MUX_FLAGS, &psmMuxContext->PortFlags, ResetPending);

        pendingResetOperations =
            InterlockedDecrementULONG(&hsmMuxContext->OutstandingResetOperations);

        if (pendingResetOperations == 0) {

            //
            // This is the last one, so lets notify the HSM
            //
            HUBSM_AddHsmEvent(hubFdoContext,
                              HsmEventPortResetReferencesReleased);

        }

    } FINALLY {

    }

}


VOID
HUBMUX_QueueSurpriseRemoveToAllSMs(
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This function isn't called by the HSM, as it doesn't handle
    surprise remove.  Instead it is called directly from the
    WDF Event callback.  This function is responsible for queuing
    the events to all PSMs and DSMs


Return Value:

    VOID

--*/
{
    PHSM_MUX_CONTEXT    hsmMuxContext;
    PPORT_CONTEXT       currentPort;
    KIRQL               irql;

    TRY {

        hsmMuxContext = &HubFdoContext->MuxContext;

        FOR_ALL_IN_LIST(PORT_CONTEXT,
                        &hsmMuxContext->PSMListHead,
                        MuxContext.ListEntry,
                        currentPort) {

            currentPort->PsmContext.AddEvent(currentPort,
                                             PsmEventHubSurpriseRemove);

        }

        KeAcquireSpinLock(&hsmMuxContext->MuxLock,
                          &irql);

        //
        // Since we are going down, lets not let anyone get added to the list
        //
        INTERLOCKED_SET_BIT_FIELD(HSM_MUX_FLAGS, &hsmMuxContext->HubMuxFlags, FailDsmRegistration);

        KeReleaseSpinLock(&hsmMuxContext->MuxLock,
                          irql);

    } FINALLY {

    }

}


VOID
HUBMUX_QueueResetToAllPSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Queues a HubReset event to all PSMs.

Return Value:

    VOID

--*/
{

    PHSM_MUX_CONTEXT    hsmMuxContext;
    PPORT_CONTEXT       currentPort;

    TRY {

        hsmMuxContext = &HubFdoContext->MuxContext;
        INTERLOCKED_SET_BIT_FIELD(HSM_MUX_FLAGS, &hsmMuxContext->HubMuxFlags, HubResetInProgress);
        //
        // Make sure we set the number outstanding BEFORE we start queuing the events
        // in order to ensure when they complete, we don't bounce off zero and
        // complete the operation too soon.
        //
        hsmMuxContext->OutstandingResetOperations = hsmMuxContext->NumberOfPSMs;

        FOR_ALL_IN_LIST(PORT_CONTEXT,
                        &hsmMuxContext->PSMListHead,
                        MuxContext.ListEntry,
                        currentPort) {

            NT_ASSERT(currentPort->MuxContext.PortFlags.ResetPending == 0);
            INTERLOCKED_SET_BIT_FIELD(PORT_MUX_FLAGS, &currentPort->MuxContext.PortFlags, ResetPending);
            currentPort->PsmContext.AddEvent(currentPort,
                                             PsmEventHubReset);

        }
    } FINALLY {

    }

}


VOID
HUBMUX_QueueStatusChangeToPSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Queues a HubStatusChange event to All PSMs that have the change bit set
    for them.

Return Value:

    VOID

--*/
{
    PHSM_MUX_CONTEXT            hsmMuxContext;
    LONG                        numberOfChanges;
    PPORT_CONTEXT               currentPort;
    LONG                        numberOfChangesSent;
    BOOLEAN                     completeHere;
    LONG                        difference;


    TRY {

        completeHere = FALSE;
        hsmMuxContext = &HubFdoContext->MuxContext;

        //
        // I'm going to assume that the HSM cleared the hub change bit for now
        // If it isn't, then we may need to decrement the numberOfChanges by 1
        //
        NT_ASSERT(RtlCheckBit(&HubFdoContext->StatusChangeInterruptRequest.StatusChangeBitMapHeader,
                              0) == FALSE);


        //
        // Lets first count the maximumum number of port changes we will handle
        // This may be more than we actually handle, but we don't want the number
        // to bounce off zero
        //
        numberOfChanges = RtlNumberOfSetBits(&HubFdoContext->StatusChangeInterruptRequest.StatusChangeBitMapHeader);

        if (numberOfChanges == 0) {
            //
            // Make a note of the fact that we got interrupt with no changes.
            // We will use this information when sending the interrupt next
            // time
            //
            HubFdoContext->StatusChangeInterruptRequest.NoChangesInLastInterrupt = TRUE;
            completeHere = TRUE;
            LEAVE;
        }

        hsmMuxContext->OutstandingPortChanges = numberOfChanges;

        numberOfChangesSent = 0;
        FOR_ALL_IN_LIST(PORT_CONTEXT,
                        &hsmMuxContext->PSMListHead,
                        MuxContext.ListEntry,
                        currentPort) {

            NT_ASSERT(currentPort->MuxContext.PortFlags.PortChangePending == 0);

            //
            // Determine if there is a change for this port
            //
            if (RtlCheckBit(&HubFdoContext->StatusChangeInterruptRequest.StatusChangeBitMapHeader,
                            currentPort->PortInformation.PortNumber)) {

                //
                // This one changed, so lets send the event
                //
                INTERLOCKED_SET_BIT_FIELD(PORT_MUX_FLAGS, &currentPort->MuxContext.PortFlags, PortChangePending);
                numberOfChangesSent++;
                currentPort->PsmContext.AddEvent(currentPort,
                                                 PsmEventHubStatusChange);

            }
        }

        //
        // It may be that there were some status bits set for which we don't
        // actually have port objects (due to a hub error possibly), so we need to
        // make sure that we don't get stuck waiting for someone to decrement our
        // count of pending changes.
        //
        NT_ASSERT(numberOfChanges >= numberOfChangesSent);
        if (numberOfChanges > numberOfChangesSent) {

            //
            // Determine amount we need to decrement
            //
            difference = numberOfChanges - numberOfChangesSent;
            numberOfChanges = InterlockedSubtractULONG(&hsmMuxContext->OutstandingPortChanges,
                                                       difference);

            if (numberOfChanges == 0) {

                //
                // In this case, all of the PSMs must have already re-enabled the interrupts
                // before we got here, so we need to send the completion event here
                //
                completeHere = TRUE;

            }

        }



    } FINALLY {

        if (completeHere) {

            HUBSM_AddHsmEvent(HubFdoContext,
                              HsmEventPortEnableInterruptTransfer);

        }

    }

}

VOID
HUBMUX_QueueFakeStatusChangeToAllPSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Queues a Fake HubStatusChange event to All PSMs. The reason for
    doing this step is to maintain compat with Win7 stack in the
    sceanrio where hub resumes from Selective Suspend

Return Value:

    VOID

--*/
{
    PHSM_MUX_CONTEXT            hsmMuxContext;
    PPORT_CONTEXT               currentPort;

    TRY {

        hsmMuxContext = &HubFdoContext->MuxContext;
        hsmMuxContext->OutstandingPortChanges = hsmMuxContext->NumberOfPSMs;

        FOR_ALL_IN_LIST(PORT_CONTEXT,
                        &hsmMuxContext->PSMListHead,
                        MuxContext.ListEntry,
                        currentPort) {

            NT_ASSERT(currentPort->MuxContext.PortFlags.PortChangePending == 0);

                INTERLOCKED_SET_BIT_FIELD(PORT_MUX_FLAGS, &currentPort->MuxContext.PortFlags, PortChangePending);
                currentPort->PsmContext.AddEvent(currentPort,
                                                 PsmEventHubStatusChange);
        }

    } FINALLY {


    }
}


VOID
HUBMUX_ReEnableInterruptTransfer(
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Middle layer keeps track of PSMs that are processing StatusChange
    and are preventing HSM from re-enabling interrupt transfer.
    This function lets middle layer know that this PSM is ready with
    enabling interrupt transfer again. If all the PSMs are ok with enabling
    interrupts, then it queues HsmEventEnableInterruptTransfer event into HSM

Return Value:


--*/
{
    PHUB_FDO_CONTEXT    hubFdoContext;
    PPSM_MUX_CONTEXT    psmMuxContext;
    PHSM_MUX_CONTEXT    hsmMuxContext;
    ULONG               pendingPortChanges;

    TRY {

        hubFdoContext = PortContext->HubFdoContext;
        psmMuxContext = &PortContext->MuxContext;
        hsmMuxContext = &hubFdoContext->MuxContext;

        NT_ASSERT(psmMuxContext->PortFlags.PortChangePending == 1);
        INTERLOCKED_CLEAR_BIT_FIELD(PORT_MUX_FLAGS, &psmMuxContext->PortFlags, PortChangePending);

        pendingPortChanges = InterlockedDecrementULONG(&hsmMuxContext->OutstandingPortChanges);
        if (pendingPortChanges == 0) {
            //
            // The last PSM has said it is ok to reenable interrupts.
            //
            HUBSM_AddHsmEvent(hubFdoContext,
                              HsmEventPortEnableInterruptTransfer);

        }

    } FINALLY {

    }

}


HSM_EVENT
HUBMUX_CheckIfAllPSMInterruptReferencesAreReleased (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Checks if all the interrupt references have been released.

    If all interupt references have not been released, enables the
    InterruptReferencesReleased event. Future requests for
    acquiring interrupt references should be failed once this function
    has been called.

Return Value:

    TRUE if all interrupt references have been released

    FALSE if all interrupt references have not been released.

--*/
{
    HSM_EVENT           result;
    PHSM_MUX_CONTEXT    muxContext;
    KIRQL               irql;

    TRY {

        muxContext = &HubFdoContext->MuxContext;
        result = HsmEventYes;

        KeAcquireSpinLock(&muxContext->MuxLock,
                          &irql);

        if (muxContext->NumberOfInterruptReferencesHeld > 0) {

            //
            // A PSM is still holding an Interrupt Reference, so
            // we need to tell the HSM NO
            //
            result = HsmEventNo;


            //
            // Set a flag so that we can start generating the
            // InterruptReferencesReleased event.
            //
            INTERLOCKED_SET_BIT_FIELD(HSM_MUX_FLAGS, &muxContext->HubMuxFlags, InterruptReferenceReleasePending);

        }

        //
        // Set a flag so that we start failing any future AcquireInterruptReference
        // calls.
        //
        INTERLOCKED_SET_BIT_FIELD(HSM_MUX_FLAGS, &muxContext->HubMuxFlags, FailInterruptReferenceAcquired);

        KeReleaseSpinLock(&muxContext->MuxLock,
                          irql);



    } FINALLY {

    }

    return result;

}


VOID
HUBMUX_ReleaseInterruptReference (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Release Interrupt reference of this port  to HSM.
    Middle layer keeps track of the PSMs that are holding interrupt
    reference i.e. they are preventing HSM from disabling interrupt transfer.
    If the HSM has called into middle layer before (by calling
    CheckIfAllPSMInterruptReferencesAreReleased), and if this was the last
    interrupt reference, then it will queue HsmEventInterruptReferencesReleased
    into HSM.

Return Value:

    VOID

--*/
{
    PHUB_FDO_CONTEXT    hubFdoContext;
    PPSM_MUX_CONTEXT    psmMuxContext;
    PHSM_MUX_CONTEXT    hsmMuxContext;
    KIRQL               irql;
    BOOLEAN             sendEventToHSM;

    TRY {

        hubFdoContext = PortContext->HubFdoContext;
        psmMuxContext = &PortContext->MuxContext;
        hsmMuxContext = &hubFdoContext->MuxContext;
        sendEventToHSM = FALSE;

        KeAcquireSpinLock(&hsmMuxContext->MuxLock,
                          &irql);

        //
        // We are using DbgBreak rather than asserts here because we
        // have seen this condition being violated in the fre builds
        // and we need to catch the issue as soon as it happens to
        // debug it
        //
        if (psmMuxContext->PortFlags.InterruptReferenceHeld == 0) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Breaking in the debugger because Port is releasing interrupt reference when it is not holding it");

            HUBMISC_DbgBreak("Port is releasing interrupt reference when it is not holding it");
        }
        INTERLOCKED_CLEAR_BIT_FIELD(PORT_MUX_FLAGS, &psmMuxContext->PortFlags, InterruptReferenceHeld);

        if (hsmMuxContext->NumberOfInterruptReferencesHeld <= 0) {

            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Breaking in the debugger because Hub interrupt reference is being released too many times");

            HUBMISC_DbgBreak("Hub interrupt reference is being released too many times");

        } else {

            hsmMuxContext->NumberOfInterruptReferencesHeld--;

        }

        //
        // Determine if we need to send a InterruptReferencesReleased event
        // to the HSM.
        //
        if ((hsmMuxContext->NumberOfInterruptReferencesHeld == 0) &&
            (hsmMuxContext->HubMuxFlags.InterruptReferenceReleasePending == 1)) {

            INTERLOCKED_CLEAR_BIT_FIELD(HSM_MUX_FLAGS, &hsmMuxContext->HubMuxFlags, InterruptReferenceReleasePending);
            sendEventToHSM = TRUE;

        }

        KeReleaseSpinLock(&hsmMuxContext->MuxLock,
                          irql);

        if (sendEventToHSM) {

            HUBSM_AddHsmEvent(hubFdoContext,
                              HsmEventPortInterruptReferencesReleased);

        }

    } FINALLY {

    }

}


PSM_EVENT
HUBMUX_AcquireInterruptReference (
    __in
        PPORT_CONTEXT      PortContext
    )
/*++

Routine Description:

    Tries to acquire  Interrupt reference  to HSM. Middle layer keeps
    track of the PSMs that are holding interrupt reference i.e. they
    are preventing HSM from disabling interrupt transfer. If the HSM
    has called into middle layer before (by calling
    CheckIfAllPSMInterruptReferencesAreReleased), Acquires an interrupt
    reference on HSM then this function should return failure and should
    not increment the interrupt reference count. Otherwise it should
    increment the interrupt reference count and return success.

Return Value:

    PsmEventOperationSuccess
    PsmEventOperationFailure

--*/
{
    PHUB_FDO_CONTEXT    hubFdoContext;
    PPSM_MUX_CONTEXT    psmMuxContext;
    PHSM_MUX_CONTEXT    hsmMuxContext;
    KIRQL               irql;
    PSM_EVENT           result;

    TRY {

        hubFdoContext = PortContext->HubFdoContext;
        psmMuxContext = &PortContext->MuxContext;
        hsmMuxContext = &hubFdoContext->MuxContext;

        KeAcquireSpinLock(&hsmMuxContext->MuxLock,
                          &irql);

        NT_ASSERT(psmMuxContext->PortFlags.InterruptReferenceHeld == 0);

        //
        // Determine if we should fail this request
        //
        if (hsmMuxContext->HubMuxFlags.FailInterruptReferenceAcquired == 1) {
            result = PsmEventOperationFailure;
            LEAVE;
        }

        INTERLOCKED_SET_BIT_FIELD(PORT_MUX_FLAGS, &psmMuxContext->PortFlags, InterruptReferenceHeld);
        hsmMuxContext->NumberOfInterruptReferencesHeld++;

        result = PsmEventOperationSuccess;

    } FINALLY {

        KeReleaseSpinLock(&hsmMuxContext->MuxLock,
                          irql);

    }

    return result;

}


VOID
HUBMUX_EnableResetHub (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    This function enables queuing of ResetHub event into
    HSM when requested by PSM/DSM

Return Value:

    VOID

--*/
{

    KIRQL       irql;

    TRY {

        KeAcquireSpinLock(&HubFdoContext->MuxContext.MuxLock,
                          &irql);
        INTERLOCKED_SET_BIT_FIELD(HSM_MUX_FLAGS, &HubFdoContext->MuxContext.HubMuxFlags, HubResetEnabled);
        KeReleaseSpinLock(&HubFdoContext->MuxContext.MuxLock,
                          irql);

    } FINALLY {

    }

}


VOID
HUBMUX_ResetHub (
    __in
        PPORT_CONTEXT           PortContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)
    This function invokes the function to reset the hub, if not
    already resetting

Return Value:

    VOID

--*/
{
    TRY {

        //
        // We don't care about the return value. If there is already
        // a reset in progress or if the hub is going down, then we
        // are fine
        //
        HUBMUX_CheckAndQueueIfResetHubIsEnabled(PortContext->HubFdoContext);

    } FINALLY {


    }

}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBMUX_CheckAndQueueIfResetHubIsEnabled (
    __in
        PHUB_FDO_CONTEXT            HubFdoContext
    )
/*++

Routine Description:

    This function checks if hub is in a state where it can be reset
    If yes, then it queues a hubreset event into HSM and returns
    whether it was able to queue the event or not

Return Value:

    TRUE if it was able to queue the ResetHub event into HSM,
    FALSE otherwise

--*/
{
    KIRQL                   irql;
    BOOLEAN                 sendReset;

    TRY {

        sendReset = FALSE;

        KeAcquireSpinLock(&HubFdoContext->MuxContext.MuxLock,
                          &irql);

        if (HubFdoContext->MuxContext.HubMuxFlags.HubResetEnabled == 1) {

            INTERLOCKED_CLEAR_BIT_FIELD(HSM_MUX_FLAGS, &HubFdoContext->MuxContext.HubMuxFlags, HubResetEnabled);
            sendReset = TRUE;

        }

        KeReleaseSpinLock(&HubFdoContext->MuxContext.MuxLock,
                          irql);


    } FINALLY {

        if (sendReset) {
            HUBSM_AddHsmEvent(HubFdoContext,
                              HsmEventResetHub);
        }

    }

    return sendReset;

}


VOID
HUBMUX_QueuePowerDownEventToDSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext,
    __in
        DSM_EVENT               PowerDownEvent
    )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Queues a HubStop event to All DSMs.

Return Value:

    VOID

--*/
{
    PHSM_MUX_CONTEXT    hsmMuxContext;
    PDEVICE_CONTEXT     currentDevice;
    PDEVICE_CONTEXT     nextDevice;
    BOOLEAN             completeHere;
    ULONG               numToSend;
    ULONG               pendingCount;
    KIRQL               irql;
    LIST_ENTRY          tempList;

    TRY {

        completeHere = FALSE;
        hsmMuxContext = &HubFdoContext->MuxContext;

        NT_ASSERT(hsmMuxContext->OutstandingPnPOperations == 0);

        //
        // First lets go find all of the DSMs to send this to
        //
        InitializeListHead(&tempList);
        KeAcquireSpinLock(&hsmMuxContext->MuxLock,
                          &irql);

        INTERLOCKED_SET_BIT_FIELD(HSM_MUX_FLAGS, &hsmMuxContext->HubMuxFlags, FailDsmRegistration);

        numToSend = 0;
        pendingCount = 0;
        FOR_ALL_IN_LIST(DEVICE_CONTEXT,
                        &hsmMuxContext->DSMListHead,
                        MuxContext.ListEntry,
                        currentDevice) {

            if (currentDevice->MuxContext.DeviceFlags.PowerReferenceHeld == 1) {

                pendingCount++;
                INTERLOCKED_SET_BIT_FIELD(DEVICE_MUX_FLAGS, &currentDevice->MuxContext.DeviceFlags, PnpOperationPending);

            }

            numToSend++;
            InsertTailList(&tempList,
                           &currentDevice->MuxContext.TempListEntry);
            //
            // We need to acquire a reference to the WDFOBJECT
            // representing the DSM so that we can keep it from going
            // away while we are sending events to it.
            //
            WdfObjectReferenceWithTag(WdfObjectContextGetObject(currentDevice),
                                      (PVOID) PnPPowerTag);

        }

        hsmMuxContext->OutstandingPnPOperations = pendingCount;

        KeReleaseSpinLock(&hsmMuxContext->MuxLock,
                          irql);

        //
        // If there weren't any, we need to complete this
        // operation here
        //
        if (numToSend == 0) {
            completeHere = TRUE;
            LEAVE;
        }

        //
        // There may not be any DSMs that are actually holding power references
        // in which case there is no acknowledgement of this event from the DSM
        // So the number outstanding would be the number of power references
        // held.
        //
        if (pendingCount == 0) {
            //
            // In this case, ReleasePowerReference will never be called, so
            // we need to generate the PowerReferencesReleased event to unblock
            // the HSM
            //
            completeHere = TRUE;
        }

        //
        // Now we can actually send the events
        //
        FOR_ALL_IN_LIST_SAFE(DEVICE_CONTEXT,
                             &tempList,
                             MuxContext.TempListEntry,
                             currentDevice,
                             nextDevice) {

            HUBSM_AddDsmEvent(currentDevice,
                              PowerDownEvent);
            //
            // Now we can release the reference to the DSM since we are
            // finished touching it
            //
            WdfObjectDereferenceWithTag(WdfObjectContextGetObject(currentDevice),
                                        (PVOID) PnPPowerTag);

        }

    } FINALLY {

        if (completeHere) {
            HUBSM_AddHsmEvent(HubFdoContext,
                              HsmEventDevicePowerReferencesReleased);
        }

    }

}

VOID
HUBMUX_QueueStopAfterSuspendToAllDSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Queues a HubStopAfterSuspend event to All DSMs.

Return Value:

    VOID

--*/
{
    PHSM_MUX_CONTEXT    hsmMuxContext;
    PDEVICE_CONTEXT     currentDevice;
    PDEVICE_CONTEXT     nextDevice;
    BOOLEAN             completeHere;
    ULONG               numToSend;
    ULONG               pendingCount;
    KIRQL               irql;
    LIST_ENTRY          tempList;

    TRY {

        completeHere = FALSE;
        hsmMuxContext = &HubFdoContext->MuxContext;

        NT_ASSERT(hsmMuxContext->OutstandingPnPOperations == 0);

        //
        // First lets go find all of the DSMs to send this to
        //
        InitializeListHead(&tempList);
        KeAcquireSpinLock(&hsmMuxContext->MuxLock,
                          &irql);

        INTERLOCKED_SET_BIT_FIELD(HSM_MUX_FLAGS, &hsmMuxContext->HubMuxFlags, FailDsmRegistration);

        numToSend = 0;
        pendingCount = 0;
        FOR_ALL_IN_LIST(DEVICE_CONTEXT,
                        &hsmMuxContext->DSMListHead,
                        MuxContext.ListEntry,
                        currentDevice) {

            pendingCount++;
            INTERLOCKED_SET_BIT_FIELD(DEVICE_MUX_FLAGS, &currentDevice->MuxContext.DeviceFlags, PnpOperationPending);

            numToSend++;
            InsertTailList(&tempList,
                           &currentDevice->MuxContext.TempListEntry);
            //
            // We need to acquire a reference to the WDFOBJECT
            // representing the DSM so that we can keep it from going
            // away while we are sending events to it.
            //
            WdfObjectReferenceWithTag(WdfObjectContextGetObject(currentDevice),
                                      (PVOID) PnPPowerTag);

        }

        hsmMuxContext->OutstandingPnPOperations = pendingCount;

        KeReleaseSpinLock(&hsmMuxContext->MuxLock,
                          irql);

        //
        // If there weren't any, we need to complete this
        // operation here
        //
        if (numToSend == 0) {
            completeHere = TRUE;
            LEAVE;
        }

        //
        // There may not be any DSMs that are actually holding power references
        // in which case there is no acknowledgement of this event from the DSM
        // So the number outstanding would be the number of power references
        // held.
        //
        if (pendingCount == 0) {
            //
            // In this case, ReleasePowerReference will never be called, so
            // we need to generate the PowerReferencesReleased event to unblock
            // the HSM
            //
            completeHere = TRUE;
        }

        //
        // Now we can actually send the events
        //
        FOR_ALL_IN_LIST_SAFE(DEVICE_CONTEXT,
                             &tempList,
                             MuxContext.TempListEntry,
                             currentDevice,
                             nextDevice) {

            HUBSM_AddDsmEvent(currentDevice,
                              DsmEventHubStopAfterSuspend);
            //
            // Now we can release the reference to the DSM since we are
            // finished touching it
            //
            WdfObjectDereferenceWithTag(WdfObjectContextGetObject(currentDevice),
                                        (PVOID) PnPPowerTag);

        }

    } FINALLY {

        if (completeHere) {
            HUBSM_AddHsmEvent(HubFdoContext,
                              HsmEventDeviceStopAfterSuspendAcked);
        }

    }

}

VOID
HUBMUX_QueueStopToAllDSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Queues a HubStop event to All DSMs.

Return Value:

    VOID

--*/
{
    HUBMUX_QueuePowerDownEventToDSMs(HubFdoContext,
                                     DsmEventHubStop);
}

VOID
HUBMUX_QueueSuspendToAllDSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Queues a HubSuspend event to All DSMs.

Return Value:

    VOID

--*/
{
    HUBMUX_QueuePowerDownEventToDSMs(HubFdoContext,
                                     DsmEventHubSuspend);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBMUX_PropogateDeviceProgrammingLostFlagsToAllDSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

Return Value:

    VOID

--*/
{
    PHSM_MUX_CONTEXT    hsmMuxContext;
    PDEVICE_CONTEXT     currentDevice;
    KIRQL               irql;

    TRY {

        hsmMuxContext = &HubFdoContext->MuxContext;

        KeAcquireSpinLock(&hsmMuxContext->MuxLock,
                          &irql);

        FOR_ALL_IN_LIST(DEVICE_CONTEXT,
                        &hsmMuxContext->DSMListHead,
                        MuxContext.ListEntry,
                        currentDevice) {

            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &currentDevice->DeviceStateFlags, DeviceReprogrammingPending);
        }

        KeReleaseSpinLock(&hsmMuxContext->MuxLock,
                          irql);

    } FINALLY {

    }

}


VOID
HUBMUX_QueuePowerUpEventToDSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext,
    __in
        DSM_EVENT               PowerUpEvent
    )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Queues HubResume events to all DSMs to which HubSuspend was sent while
    hub was suspending.

    During hub resume PSM may start enumerating a new device, therefore
    some DSMs may not have suspended.

Return Value:

    VOID

--*/
{
    KIRQL               irql;
    PHSM_MUX_CONTEXT    hsmMuxContext;
    PDEVICE_CONTEXT     currentDevice;
    PDEVICE_CONTEXT     nextDevice;
    BOOLEAN             completeHere;
    LIST_ENTRY          tempList;
    ULONG               numToSend;

    TRY {

        completeHere = FALSE;
        hsmMuxContext = &HubFdoContext->MuxContext;

        //
        // We need to now allow the PSMs to acquire interrupt references
        // again, if it was previously disabled.
        //

        NT_ASSERT(hsmMuxContext->OutstandingPnPOperations == 0);


        //
        // First we need find all of the DSMs that we should send the
        // event to
        //
        InitializeListHead(&tempList);
        KeAcquireSpinLock(&hsmMuxContext->MuxLock,
                          &irql);

        numToSend = 0;

        FOR_ALL_IN_LIST(DEVICE_CONTEXT,
                        &hsmMuxContext->DSMListHead,
                        MuxContext.ListEntry,
                        currentDevice) {

            if (currentDevice->MuxContext.DeviceFlags.Stopped) {

                INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_MUX_FLAGS, &currentDevice->MuxContext.DeviceFlags, Stopped);
                INTERLOCKED_SET_BIT_FIELD(DEVICE_MUX_FLAGS, &currentDevice->MuxContext.DeviceFlags, PnpOperationPending);
                numToSend++;
                InsertTailList(&tempList,
                               &currentDevice->MuxContext.TempListEntry);
                //
                // We need to acquire a reference to the WDFOBJECT
                // representing the DSM so that we can keep it from going
                // away while we are sending events to it.
                //
                WdfObjectReferenceWithTag(WdfObjectContextGetObject(currentDevice),
                                          (PVOID) PnPPowerTag);

            }

        }

        //
        // There shouldn't be any DSMs that are holding power
        // references at this point, so the number of pending operations
        // will be the number of DSMs to send to
        //
        hsmMuxContext->OutstandingPnPOperations = numToSend;

        KeReleaseSpinLock(&hsmMuxContext->MuxLock,
                          irql);

        //
        // If there weren't any DSMs, we need to complete this
        // operation here
        //
        if (numToSend == 0) {
            completeHere = TRUE;
            LEAVE;
        }

        //
        // Now we can send the events
        //
        FOR_ALL_IN_LIST_SAFE(DEVICE_CONTEXT,
                             &tempList,
                             MuxContext.TempListEntry,
                             currentDevice,
                             nextDevice) {

            HUBSM_AddDsmEvent(currentDevice,
                              PowerUpEvent);
            //
            // Now we can release the reference to the DSM since we are
            // finished touching it
            //
            WdfObjectDereferenceWithTag(WdfObjectContextGetObject(currentDevice),
                                        (PVOID) PnPPowerTag);

        }

    } FINALLY {

        if (completeHere) {
            HUBSM_AddHsmEvent(HubFdoContext,
                              HsmEventDevicePowerReferencesAcquired);
        }

    }

}

VOID
HUBMUX_QueueResumeToSuspendedDSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Queues HubResume events to all DSMs to which HubSuspend was sent while
    hub was suspending.

    During hub resume PSM may start enumerating a new device, therefore
    some DSMs may not have suspended.

Return Value:

    VOID

--*/
{
    HUBMUX_QueuePowerUpEventToDSMs(HubFdoContext,
                                   DsmEventHubResume);

}


VOID
HUBMUX_QueueResumeWithResetToSuspendedDSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Queues HubResumeWithReset to All DSMs that were suspended
    during hub suspend.

Return Value:

    VOID

--*/
{
    HUBMUX_QueuePowerUpEventToDSMs(HubFdoContext,
                                   DsmEventHubResumeWithReset);

}

VOID
HUBMUX_QueueStartToStoppedDSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Queues a HubStart event to All DSMs that were stopped
    during hub stop.

Return Value:

    VOID

--*/
{
    HUBMUX_QueuePowerUpEventToDSMs(HubFdoContext,
                                   DsmEventHubStart);

}

VOID
HUBMUX_QueueResumeInS0ToSuspendedDSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Queues HubResumeWithReset to All DSMs that were suspended
    during hub suspend.

Return Value:

    VOID

--*/
{
    HUBMUX_QueuePowerUpEventToDSMs(HubFdoContext,
                                   DsmEventHubResumeInS0);

}


VOID
HUBMUX_QueueSurpriseRemovalToAllPSMs (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and PSMs (middle layer)

    Queues a SurpriseRemoval event to All PSMs.

Return Value:

    VOID

--*/
{
    PHSM_MUX_CONTEXT    hsmMuxContext;
    PPORT_CONTEXT       currentPort;
    KIRQL               irql;

    TRY {

        hsmMuxContext = &HubFdoContext->MuxContext;

        FOR_ALL_IN_LIST(PORT_CONTEXT,
                        &hsmMuxContext->PSMListHead,
                        MuxContext.ListEntry,
                        currentPort) {

            currentPort->PsmContext.AddEvent(currentPort,
                                             PsmEventHubSurpriseRemove);

        }

        KeAcquireSpinLock(&hsmMuxContext->MuxLock,
                          &irql);

        //
        // Since we are going down, lets not let anyone get added to the list
        //
        INTERLOCKED_SET_BIT_FIELD(HSM_MUX_FLAGS, &hsmMuxContext->HubMuxFlags, FailDsmRegistration);

        KeReleaseSpinLock(&hsmMuxContext->MuxLock,
                          irql);

    } FINALLY {

    }

}


DSM_EVENT
HUBMUX_RegisterWithHSM(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Attempt to register with HSM. If HSM is already in the process
    of going down, registration will fail.

Return Value:

    DsmEventOperationSuccess, DsmEventOperationFailure

--*/
{
    DSM_EVENT           result;
    KIRQL               irql;
    PHSM_MUX_CONTEXT    hsmMuxContext;

    TRY {

        hsmMuxContext = &DeviceContext->HubFdoContext->MuxContext;

        KeAcquireSpinLock(&hsmMuxContext->MuxLock,
                          &irql);

        //
        // If we are in the process of going down, we don't want
        // to succeed this
        //
        if (hsmMuxContext->HubMuxFlags.FailDsmRegistration == 1) {
            result = DsmEventOperationFailure;
            LEAVE;
        }

        //
        // By default, the DSM gets a free power reference
        //
        INTERLOCKED_SET_BIT_FIELD(DEVICE_MUX_FLAGS, &DeviceContext->MuxContext.DeviceFlags, PowerReferenceHeld);
        hsmMuxContext->NumberOfDevicePowerReferencesHeld++;

        InsertTailList(&hsmMuxContext->DSMListHead,
                       &DeviceContext->MuxContext.ListEntry);

        result = DsmEventOperationSuccess;

    } FINALLY {

        KeReleaseSpinLock(&hsmMuxContext->MuxLock,
                          irql);

        if (result == DsmEventOperationSuccess) {
            //
            // We need to acquire a reference to the WDFOBJECT representing the DSM
            // so that we can keep it from going away while we are sending events to it.
            //
            WdfObjectReferenceWithTag(WdfObjectContextGetObject(DeviceContext),
                                      (PVOID) RegistrationTag);

        }

    }

    return result;

}


VOID
HUBMUX_UnregisterWithHSM(
    __in
        PDEVICE_CONTEXT     DeviceContext
        )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Unregister with HSM for its notifications. This calls acts an implicit
    acknowledgement for any pending Hub operation - HubStart, HubStop,
    HubSuspend, HubResume or HubResumeWithReset.

Return Value:

    VOID

--*/
{
    KIRQL               irql;
    PHSM_MUX_CONTEXT    hsmMuxContext;
    BOOLEAN             released;
    ULONG               pendingPnpOperations;
    BOOLEAN             pnpOperationCompleted;

    TRY {

        if (DeviceContext->DeviceStateFlags.ActivityIdSetForDsmRequests == 1) {

            RtlZeroMemory(&DeviceContext->DsmContext.CurrentActivityId, sizeof(GUID));
            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &DeviceContext->DeviceStateFlags, ActivityIdSetForDsmRequests);
        }

        hsmMuxContext = &DeviceContext->HubFdoContext->MuxContext;

        KeAcquireSpinLock(&hsmMuxContext->MuxLock,
                          &irql);
        released = FALSE;
        pnpOperationCompleted = FALSE;

        //
        // If this DSM has a reference held we need to release it, and potentially
        // complete any pending DSM operation
        //
        if (DeviceContext->MuxContext.DeviceFlags.PowerReferenceHeld) {

            released = TRUE;
            hsmMuxContext->NumberOfDevicePowerReferencesHeld--;
            INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_MUX_FLAGS, &DeviceContext->MuxContext.DeviceFlags, PowerReferenceHeld);

        }

        if (DeviceContext->MuxContext.DeviceFlags.PnpOperationPending) {

            pendingPnpOperations = InterlockedDecrementULONG(&hsmMuxContext->OutstandingPnPOperations);
            if (pendingPnpOperations == 0) {
                pnpOperationCompleted = TRUE;
            }

        }

        RemoveEntryList(&DeviceContext->MuxContext.ListEntry);

        KeReleaseSpinLock(&hsmMuxContext->MuxLock,
                          irql);

        WdfObjectDereferenceWithTag(WdfObjectContextGetObject(DeviceContext),
                                    (PVOID) RegistrationTag);

    } FINALLY {

        //
        // See if this would have completed a pnp operation and
        // send the appropriate event to the HSM
        //
        if (pnpOperationCompleted) {

            if (released) {
                HUBSM_AddHsmEvent(DeviceContext->HubFdoContext,
                                  HsmEventDevicePowerReferencesReleased);
            } else {
                HUBSM_AddHsmEvent(DeviceContext->HubFdoContext,
                                  HsmEventDevicePowerReferencesAcquired);
            }

        }

    }

}


VOID
HUBMUX_AcquireDevicePowerReference(
    __in
        PDEVICE_CONTEXT     DeviceContext
        )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Acquires a power reference on HSM.  After Middle layer has queued
    HsmEventHubStart, HsmEventHubResume or HsmEventHubResumeWithReset,
    it keeps track of the DSMs that have or have not acquired power reference.
    If all the devices have acquired power references, then it queue a
    HsmEventDevicePowerReferencesAcquired into HSM.

Return Value:

    VOID

--*/
{
    PHUB_FDO_CONTEXT    hubFdoContext;
    PDSM_MUX_CONTEXT    dsmMuxContext;
    PHSM_MUX_CONTEXT    hsmMuxContext;
    ULONG               pendingPnpOperations;
    KIRQL               irql;

    TRY {

        hubFdoContext = DeviceContext->HubFdoContext;
        dsmMuxContext = &DeviceContext->MuxContext;
        hsmMuxContext = &hubFdoContext->MuxContext;

        KeAcquireSpinLock(&hsmMuxContext->MuxLock,
                          &irql);

        NT_ASSERT(dsmMuxContext->DeviceFlags.PnpOperationPending == 1);
        NT_ASSERT(dsmMuxContext->DeviceFlags.PowerReferenceHeld == 0);

        INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_MUX_FLAGS, &dsmMuxContext->DeviceFlags, PnpOperationPending);
        INTERLOCKED_SET_BIT_FIELD(DEVICE_MUX_FLAGS, &dsmMuxContext->DeviceFlags, PowerReferenceHeld);
        INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_MUX_FLAGS, &dsmMuxContext->DeviceFlags, Stopped);

        pendingPnpOperations = InterlockedDecrementULONG(&hsmMuxContext->OutstandingPnPOperations);
        hsmMuxContext->NumberOfDevicePowerReferencesHeld++;

        KeReleaseSpinLock(&hsmMuxContext->MuxLock,
                          irql);

        if (pendingPnpOperations == 0) {
            //
            // This is the last one, so lets notify the HSM
            //
            HUBSM_AddHsmEvent(hubFdoContext,
                              HsmEventDevicePowerReferencesAcquired);
        }

    } FINALLY {

    }

}


VOID
HUBMUX_ReleaseDevicePowerReference(
    __in
        PDEVICE_CONTEXT     DeviceContext
        )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Release power reference of this device to HSM. After Middle layer
    has queued HsmEventHubStop,  HsmEventHubSuspend to DSMs, it keeps track
    of the DSMs that have or have not released power reference. If all the
    ports have acquired power references, then it queue a
    HsmEventDevicePowerReferencesReleased into HSM.

    One thing to note is that any point, HSM keeps track of all DSMs
    and also "Stopped/Suspended DSMs". When a DSM releases power
    reference, it goes in the list of Stopped/Suspended DSMs. When the
    hub powers up again, it needs to send start/resume to only these DSMs.
    There is a window where PSMs acquire power reference and when HSM sends
    start/resume to DSMs, so additional DSMs could register with HSM within that
    window - those DSMs will not be in the list of Stopped/Suspended DSMs.

Return Value:

    VOID

--*/
{
    PHUB_FDO_CONTEXT    hubFdoContext;
    PDSM_MUX_CONTEXT    dsmMuxContext;
    PHSM_MUX_CONTEXT    hsmMuxContext;
    ULONG               pendingPnpOperations;
    ULONG               powerReferencesHeld;
    KIRQL               irql;

    TRY {

        hubFdoContext = DeviceContext->HubFdoContext;
        dsmMuxContext = &DeviceContext->MuxContext;
        hsmMuxContext = &hubFdoContext->MuxContext;

        KeAcquireSpinLock(&hsmMuxContext->MuxLock,
                          &irql);
        NT_ASSERT(dsmMuxContext->DeviceFlags.PnpOperationPending == 1);
        NT_ASSERT(dsmMuxContext->DeviceFlags.PowerReferenceHeld == 1);

        INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_MUX_FLAGS, &dsmMuxContext->DeviceFlags, PnpOperationPending);
        INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_MUX_FLAGS, &dsmMuxContext->DeviceFlags, PowerReferenceHeld);
        INTERLOCKED_SET_BIT_FIELD(DEVICE_MUX_FLAGS, &dsmMuxContext->DeviceFlags, Stopped);

        powerReferencesHeld = InterlockedDecrementULONG(&hsmMuxContext->NumberOfDevicePowerReferencesHeld);
        pendingPnpOperations = InterlockedDecrementULONG(&hsmMuxContext->OutstandingPnPOperations);

        KeReleaseSpinLock(&hsmMuxContext->MuxLock,
                          irql);

        if (pendingPnpOperations == 0) {

            //
            // This is the last one, so lets notify the HSM
            //
            NT_ASSERT(powerReferencesHeld == 0);

            HUBSM_AddHsmEvent(hubFdoContext,
                              HsmEventDevicePowerReferencesReleased);

        }

    } FINALLY {

    }

}


VOID
HUBMUX_AckStopAfterSuspend(
    __in
        PDEVICE_CONTEXT     DeviceContext
        )
/*++

Routine Description:

    This is a mediator function between HSM and DSMs (middle layer)

    Provide an acknoledgement that the DSM has completed the processing
    of the StopAfterSuspend event. After Middle layer has queued
    HsmEventHubSuspend to DSMs, it keeps track of the DSMs that have or
    have not acknowledged. If all the devices have acknowledged, it
    queues a HsmEventDeviceStopAfterSuspendAcked into HSM.

Return Value:

    VOID

--*/
{
    PHUB_FDO_CONTEXT    hubFdoContext;
    PDSM_MUX_CONTEXT    dsmMuxContext;
    PHSM_MUX_CONTEXT    hsmMuxContext;
    ULONG               pendingPnpOperations;
    KIRQL               irql;

    TRY {

        hubFdoContext = DeviceContext->HubFdoContext;
        dsmMuxContext = &DeviceContext->MuxContext;
        hsmMuxContext = &hubFdoContext->MuxContext;

        KeAcquireSpinLock(&hsmMuxContext->MuxLock,
                          &irql);
        NT_ASSERT(dsmMuxContext->DeviceFlags.PnpOperationPending == 1);

        INTERLOCKED_CLEAR_BIT_FIELD(DEVICE_MUX_FLAGS, &dsmMuxContext->DeviceFlags, PnpOperationPending);
        INTERLOCKED_SET_BIT_FIELD(DEVICE_MUX_FLAGS, &dsmMuxContext->DeviceFlags, Stopped);

        pendingPnpOperations = InterlockedDecrementULONG(&hsmMuxContext->OutstandingPnPOperations);

        KeReleaseSpinLock(&hsmMuxContext->MuxLock,
                          irql);

        if (pendingPnpOperations == 0) {

            //
            // This is the last one, so lets notify the HSM
            //

            HUBSM_AddHsmEvent(hubFdoContext,
                              HsmEventDeviceStopAfterSuspendAcked);

        }

    } FINALLY {

    }

}
