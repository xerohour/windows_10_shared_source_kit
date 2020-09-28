/*++
Copyright (c) Microsoft Corporation

Module Name:

StateMachineUcxEndpointStateEntryFuncs.c

Abstract:

    State entry functions for Endpoint State Machine
    This code is *not* automatically generated

--*/



#include "pch.h"

#include "StateMachineUcxEndpointStateEntryFuncs.tmh"

SM_ENGINE_EVENT
UcxEndpointEventHandler_CompletePendingOperation(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointEventHandler_CompletePendingOperation is called when the Ucx
    endpoint state machine wants to perform the CompletePendingOperation Operation
    for an event.

Return Value:

    SmEngineEventNull
--*/
{

    TRY {

        UsbDevice_PendingOperationCompleteByEndpoint(UcxEndpointContext->UcxUsbDeviceContext);        

    } FINALLY {
    }

    return SmEngineEventNull;
} // UcxEndpointEventHandler_CompletePendingOperation

SM_ENGINE_EVENT
UcxEndpointEventHandler_ClearEnabledFlagAndCompletePendingOperation(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointEventHandler_ClearEnabledFlagAndCompletePendingOperation is called when the Ucx
    endpoint state machine wants to perform the ClearEnabledFlagAndCompletePendingOperation Operation
    for an event.

Return Value:

    SmEngineEventNull
--*/
{
    TRY {

        //
        // Note: Following assert cannot be placed in the code since the endpoint might 
        // have gotten disabled due to a controller reset. 
        // In that case the UcxEndpointContext->Enabled boolean can be cleared multiple times by way
        // of the hub driver requesting both USBDEVICE_RESET and USBDEVICE_DISABLE, without
        // necessarily being aware of the controller reset.
        // NT_ASSERT(UcxEndpointContext->Enabled);
        //
        UcxEndpointContext->Enabled = FALSE;
        
        UsbDevice_PendingOperationCompleteByEndpoint(UcxEndpointContext->UcxUsbDeviceContext);        

    } FINALLY {
    }

    return SmEngineEventNull;
} // UcxEndpointEventHandler_ClearEnabledFlagAndCompletePendingOperation

SM_ENGINE_EVENT
UcxEndpointEventHandler_CompletePendingTreePurge(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointEventHandler_CompletePendingTreePurge is called when the Ucx
    endpoint state machine wants to perform the CompleteTreePurge Operation
    for an event.

Return Value:

    SmEngineEventNull
--*/
{
    TRY {

        UsbDevice_PendingTreePurgeCompleteByEndpoint(UcxEndpointContext);

    } FINALLY {
    }

    return SmEngineEventNull;
} // UcxEndpointEventHandler_CompletePendingTreePurge

SM_ENGINE_EVENT
UcxEndpointEventHandler_Ignore(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointEventHandler_Ignore is called when the Ucx
    endpoint state machine wants to perform the Ignore Operation
    for an event.

Return Value:

    SmEngineEventNull
--*/
{
    UNREFERENCED_PARAMETER(UcxEndpointContext);
    return SmEngineEventNull;
} // UcxEndpointEventHandler_Ignore

SM_ENGINE_EVENT
UcxEndpointEventHandler_OkToResetController(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointEventHandler_OkToResetController is called when the Ucx
    endpoint state machine wants to perform the OkToResetController Operation
    for an event.

Return Value:

    SmEngineEventPause
--*/
{

    UNREFERENCED_PARAMETER(UcxEndpointContext);

    TRY {
        
        //
        // Inform Controler that PrepareForReset is complete.
        //
        Controller_PendingPrepareForResetOperationComplete(
            UcxEndpointContext->UcxControllerContext);


    } FINALLY {
    }
    
    return SmEngineEventNull;
} // UcxEndpointEventHandler_OkToResetController

SM_ENGINE_EVENT
UcxEndpointEventHandler_OkToResetControllerAndPauseSM(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointEventHandler_OkToResetControllerAndPauseSM is called when the Ucx
    endpoint state machine wants to perform the OkToResetControllerAndPauseSM Operation
    for an event.

Return Value:

    SmEngineEventPause
--*/
{

    UNREFERENCED_PARAMETER(UcxEndpointContext);

    TRY {
        
        //
        // It is important to Pause the state machine before carrying out the 
        // step that would resume the state machine. 
        //
        UcxEndpointContext->SmEngineContext.StateMachinePaused = TRUE;

        //
        // Inform Controler that PrepareForReset in complete.
        // Note that the revent to resume the paused state machine can follow any time 
        //
        Controller_PendingPrepareForResetOperationComplete(
            UcxEndpointContext->UcxControllerContext);


    } FINALLY {
    }
    
    //
    // It is very important to return the Paused Event here since we need to ensure that the
    // state machine does not continue processing events in this tread. Please note that 
    // by the time this function returns, it is possible that the statemachine has already
    // resumed, so state machine cannot look at its internal state to make that determination.
    //
    return SmEngineEventPaused;
} // UcxEndpointEventHandler_OkToResetControllerAndPauseSM

SM_ENGINE_EVENT
UcxEndpointEventHandler_ResumeSM(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointEventHandler_ResumeSM is called when the Ucx
    endpoint state machine wants to perform the ResumeSM Operation
    for an event.

Return Value:

    SmEngineEventNull
--*/
{

    TRY {

        UcxEndpointContext->SmEngineContext.StateMachinePaused = FALSE;

    } FINALLY {
    }
    
    return SmEngineEventNull;
} // UcxEndpointEventHandler_ResumeSM

VOID
UcxEndpoint_CompleteDisableStaticStreamsIrp(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
{
    PIRP irp;
    PUCXSSTREAMS_PRIVCONTEXT ucxSStreamsContext;
    KIRQL irql;
    
    TRY {
        
        irp = UcxEndpointContext->PendingStreamsDisableIrp;
        UcxEndpointContext->PendingStreamsDisableIrp = NULL;
        NT_ASSERT(NULL != irp);

        ucxSStreamsContext = UcxEndpointContext->UcxSStreamsContext;        
        KeAcquireSpinLock(&UcxEndpointContext->UcxControllerContext->TopologyLock, &irql);
        UcxEndpointContext->UcxSStreamsContext = NULL;        
        KeReleaseSpinLock(&UcxEndpointContext->UcxControllerContext->TopologyLock, irql);

        WdfObjectDelete(ucxSStreamsContext->UcxSStreams);

        InterlockedDecrement(&UcxEndpointContext->StreamsEnabled);

        Xrb_IoCompleteRequest_StatusAlreadySet(irp);
    
    } FINALLY {
    
    }

}

FORCEINLINE
VOID
UcxEndpoint_CompleteResetIrp(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
{
    PIRP irp;
    
    TRY {
        
        irp = UcxEndpointContext->PendingResetIrp;
        UcxEndpointContext->PendingResetIrp = NULL;
        NT_ASSERT(NULL != irp);

        IoCompleteRequest(irp, IO_NO_INCREMENT);
    
    } FINALLY {
    
    }

}

FORCEINLINE
VOID
UcxEndpoint_Resetting(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
{
    WDFREQUEST request;
    
    TRY {
        
        request = UcxEndpointContext->PendingResetRequest;
        UcxEndpointContext->PendingResetRequest = NULL;
        NT_ASSERT(NULL != request);
        UcxEndpointContext->EvtCallbacks.EvtEndpointReset(
            UcxEndpointContext->UcxControllerContext->UcxController,
            UcxEndpointContext->UcxEndpoint,
            request);

    } FINALLY {
    
    }

}

VOID
UcxEndpoint_ForwardDisableStaticStreamsRequestToPendingDeviceMgmtQueue(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:


Return Value:


--*/
{
    WDFREQUEST request;
    
    TRY {
        
        request = UcxEndpointContext->PendingStreamsDisableRequest;
        UcxEndpointContext->PendingStreamsDisableRequest = NULL;
        
        WdfRequestForwardToIoQueue( 
            request,
            UcxEndpointContext->UcxControllerContext->QueuePendUsbDeviceMgmtDuringControllerReset);

    } FINALLY {
    
    }
    
    return;
}

//----------------------------------------------------------------------------------------

FORCEINLINE
SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Aborting(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Aborting is called when the Ucx
    endpoint state machine enters the Aborting1 or Aborting2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    TRY {
    
        UcxEndpointContext->EvtCallbacks.EvtEndpointAbort(
            UcxEndpointContext->UcxControllerContext->UcxController,
            UcxEndpointContext->UcxEndpoint);

    } FINALLY {

    }
    
    return SmEngineEventNull;
} // UcxEndpointStateEntryFunc_Aborting

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Aborting1(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Aborting1 is called when the Ucx
    endpoint state machine enters the Aborting1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    
    return UcxEndpointStateEntryFunc_Aborting(UcxEndpointContext);

} // UcxEndpointStateEntryFunc_Aborting1

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Aborting2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Aborting2 is called when the Ucx
    endpoint state machine enters the Aborting2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    
    return UcxEndpointStateEntryFunc_Aborting(UcxEndpointContext);

} // UcxEndpointStateEntryFunc_Aborting2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_ClearingEnabledFlag(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_ClearingEnabledFlag is called when the Ucx
    endpoint state machine enters the ClearingEnabledFlag State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    TRY {

        //
        // Note: Following assert cannot be placed in the code since the endpoint might 
        // have gotten disabled due to a controller reset. 
        // In that case the UcxEndpointContext->Enabled boolean can be cleared multiple times by way
        // of the hub driver requesting both USBDEVICE_RESET and USBDEVICE_DISABLE, without
        // necessarily being aware of the controller reset.
        // NT_ASSERT(UcxEndpointContext->Enabled);
        //
        UcxEndpointContext->Enabled = FALSE;

    } FINALLY {

    }

    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_ClearingEnabledFlag

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingAbortUrbIfNotAlreadyCancelled1(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_CompletingAbortUrbIfNotAlreadyCancelled1 is called when the Ucx
    endpoint state machine enters the CompletingAbortUrbIfNotAlreadyCancelled1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {
        
        Endpoint_FindAndCompleteAbortUrbIfNotAlreadyCancelled(UcxEndpointContext);
    
    } FINALLY {
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingAbortUrbIfNotAlreadyCancelled1

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingAbortUrbIfNotAlreadyCancelled2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_CompletingAbortUrbIfNotAlreadyCancelled2 is called when the Ucx
    endpoint state machine enters the CompletingAbortUrbIfNotAlreadyCancelled2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {
        
        Endpoint_FindAndCompleteAbortUrbIfNotAlreadyCancelled(UcxEndpointContext);
    
    } FINALLY {
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingAbortUrbIfNotAlreadyCancelled2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp1(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp1 is called when the Ucx
    endpoint state machine enters the CompletingDisableStaticStreamsIrp1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    TRY {
        
        UcxEndpoint_CompleteDisableStaticStreamsIrp(UcxEndpointContext);
    
    } FINALLY {
    
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp1

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp2 is called when the Ucx
    endpoint state machine enters the CompletingDisableStaticStreamsIrp2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    TRY {
        
        UcxEndpoint_CompleteDisableStaticStreamsIrp(UcxEndpointContext);
    
    } FINALLY {
    
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp4(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp4 is called when the Ucx
    endpoint state machine enters the CompletingDisableStaticStreamsIrp4 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    
    TRY {
        
        UcxEndpoint_CompleteDisableStaticStreamsIrp(UcxEndpointContext);

    } FINALLY {
    
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp4

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsWdfRequest2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsWdfRequest2 is called when the Ucx
    endpoint state machine enters the CompletingDisableStaticStreamsWdfRequest2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    
    TRY {
        
        UcxEndpoint_ForwardDisableStaticStreamsRequestToPendingDeviceMgmtQueue(UcxEndpointContext);

    } FINALLY {
    
    }
    
    return SmEngineEventNull;
} // UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsWdfRequest2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingEnableStaticStreamsIrp1(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_CompletingEnableStaticStreamsIrp1 is called when the Ucx
    endpoint state machine enters the CompletingEnableStaticStreamsIrp1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    PIRP irp;
    
    TRY {
        
        irp = UcxEndpointContext->PendingStreamsEnableIrp;
        UcxEndpointContext->PendingStreamsEnableIrp = NULL;
        NT_ASSERT(NULL != irp);

        if (!NT_SUCCESS(irp->IoStatus.Status)) {
            InterlockedDecrement(&UcxEndpointContext->StreamsEnabled);
        }

        Xrb_IoCompleteRequest_StatusAlreadySet(irp);
    
    } FINALLY {
    
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingEnableStaticStreamsIrp1

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingEnableStaticStreamsIrp2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_CompletingEnableStaticStreamsIrp2 is called when the Ucx
    endpoint state machine enters the CompletingEnableStaticStreamsIrp2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    PIRP irp;
    
    TRY {
        
        irp = UcxEndpointContext->PendingStreamsEnableIrp;
        UcxEndpointContext->PendingStreamsEnableIrp = NULL;
        NT_ASSERT(NULL != irp);

        if (!NT_SUCCESS(irp->IoStatus.Status)) {
            InterlockedDecrement(&UcxEndpointContext->StreamsEnabled);
        }

        Xrb_IoCompleteRequest_StatusAlreadySet(irp);
    
    } FINALLY {
    
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingEnableStaticStreamsIrp2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingEndpointResetIrp1(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_CompletingEndpointResetIrp1 is called when the Ucx
    endpoint state machine enters the CompletingEndpointResetIrp1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    TRY {
        
        UcxEndpoint_CompleteResetIrp(UcxEndpointContext);

    } FINALLY {
    
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingEndpointResetIrp1

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingEndpointResetIrp2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_CompletingEndpointResetIrp2 is called when the Ucx
    endpoint state machine enters the CompletingEndpointResetIrp2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    TRY {
        
        UcxEndpoint_CompleteResetIrp(UcxEndpointContext);

    } FINALLY {
    
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingEndpointResetIrp2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingEndpointResetIrp4(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_CompletingEndpointResetIrp4 is called when the Ucx
    endpoint state machine enters the CompletingEndpointResetIrp4 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    TRY {
        
        UcxEndpoint_CompleteResetIrp(UcxEndpointContext);

    } FINALLY {
    
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingEndpointResetIrp4

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingPendingOperation1(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:
    UcxEndpointStateEntryFunc_CompletingPendingOperation1 is called when the Ucx 
    endpoint state machine enters the CompletingPendingOperation1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {        
        
         (VOID) UcxEndpointEventHandler_CompletePendingOperation(UcxEndpointContext);        
    
    } FINALLY {    
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingPendingOperation1

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingPendingOperation2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:
    UcxEndpointStateEntryFunc_CompletingPendingOperation2 is called when the Ucx 
    endpoint state machine enters the CompletingPendingOperation2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {        
        
         (VOID) UcxEndpointEventHandler_CompletePendingOperation(UcxEndpointContext);        
    
    } FINALLY {    
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingPendingOperation2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingPendingOperation3(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:
    UcxEndpointStateEntryFunc_CompletingPendingOperation3 is called when the Ucx 
    endpoint state machine enters the CompletingPendingOperation3 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {        
        
         (VOID) UcxEndpointEventHandler_CompletePendingOperation(UcxEndpointContext);        
    
    } FINALLY {    
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingPendingOperation3

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingPendingOperation4(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:
    UcxEndpointStateEntryFunc_CompletingPendingOperation4 is called when the Ucx 
    endpoint state machine enters the CompletingPendingOperation4 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {        
        
         (VOID) UcxEndpointEventHandler_CompletePendingOperation(UcxEndpointContext);        
    
    } FINALLY {    
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingPendingOperation4

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_CompletingPendingTreePurge(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:
    UcxEndpointStateEntryFunc_CompletingPendingTreePurge is called when the Ucx 
    endpoint state machine enters the CompletingPendingTreePurge State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {        
        
         (VOID) UcxEndpointEventHandler_CompletePendingTreePurge(UcxEndpointContext);        
    
    } FINALLY {    
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_CompletingPendingTreePurge

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Deleting(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Deleting is called when the Ucx
    endpoint state machine enters the Deleting State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    KIRQL                     irql;
    PUCXUSBDEVICE_PRIVCONTEXT ucxUsbDeviceContext;
    
    TRY {

        ucxUsbDeviceContext = UcxEndpointContext->UcxUsbDeviceContext;

        if (ucxUsbDeviceContext->AllowIoOnInvalidPipeHandles) {
            KeAcquireSpinLock(&ucxUsbDeviceContext->InvalidHandleTrakingLock, &irql);
            
            RemoveEntryList(&UcxEndpointContext->EndpointListEntryForInvalidHandleTraking);
            InitializeListEntry(&UcxEndpointContext->EndpointListEntryForInvalidHandleTraking);

            KeReleaseSpinLock(&ucxUsbDeviceContext->InvalidHandleTrakingLock, irql);
        }
        
        KeAcquireSpinLock(&UcxEndpointContext->UcxControllerContext->TopologyLock, &irql);

        UcxEndpointContext->UcxControllerContext->ChildEndpointListCount--;
        RemoveEntryList(&UcxEndpointContext->EndpointListEntry);

        KeReleaseSpinLock(&UcxEndpointContext->UcxControllerContext->TopologyLock, irql);
        
        LogInfo(FlagEndpoint, "UcxEndpoint 0x%p, being deleted", UcxEndpointContext->UcxEndpoint);

        Etw_UcxEndpointDelete(NULL, UcxEndpointGetContext(UcxEndpointContext->UcxEndpoint));

        //
        // Note: It is important that we call WdfObjectDelete before the reference is dropped, 
        // since endpoint may already be 'deleted' due to a its parent being deleted. 
        //
        WdfObjectDelete(UcxEndpointContext->UcxEndpoint);

        //
        // This reference was acquired during create.
        //
        WdfObjectDereferenceWithTag(UcxEndpointContext->UcxEndpoint,
                                    TAG(EXPORT(UcxEndpointCreate)));

    } FINALLY {
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_Deleting

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_DeletingStale(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_DeletingStale is called when the Ucx
    endpoint state machine enters the DeletingStale State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    TRY {

        LogInfo(FlagEndpoint, "UcxEndpoint 0x%p, being deleted", UcxEndpointContext->UcxEndpoint);

        Etw_UcxEndpointDelete(NULL, UcxEndpointGetContext(UcxEndpointContext->UcxEndpoint));

        //
        // Note: It is important that we call WdfObjectDelete before the reference is dropped, 
        // since endpoint may already be 'deleted' due to a its parent being deleted. 
        //
        WdfObjectDelete(UcxEndpointContext->UcxEndpoint);

        //
        // This reference was acquired during create.
        //
        WdfObjectDereferenceWithTag(UcxEndpointContext->UcxEndpoint,
                                    TAG(EXPORT(UcxEndpointCreate)));

    } FINALLY {
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_DeletingStale

FORCEINLINE
SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Common_DisablingStaticStreams(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Common_DisablingStaticStreams is called when the Ucx
    endpoint state machine enters the DisablingStaticStreamsXXX State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    WDFREQUEST request;
    
    TRY {

        request = UcxEndpointContext->PendingStreamsDisableRequest;
        UcxEndpointContext->PendingStreamsDisableRequest = NULL;
        NT_ASSERT(NULL != request);
        UcxEndpointContext->EvtCallbacks.EvtEndpointStaticStreamsDisable(
            UcxEndpointContext->UcxEndpoint,
            UcxEndpointContext->UcxSStreamsContext->UcxSStreams,
            request);

    } FINALLY {
    
    }
    
    return SmEngineEventNull;
} // UcxEndpointStateEntryFunc_Common_DisablingStaticStreams

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_DisablingStaticStreams(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_DisablingStaticStreams is called when the Ucx
    endpoint state machine enters the DisablingStaticStreams State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return UcxEndpointStateEntryFunc_Common_DisablingStaticStreams(UcxEndpointContext);
} // UcxEndpointStateEntryFunc_DisablingStaticStreams

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_DisablingStaticStreams2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_DisablingStaticStreams2 is called when the Ucx
    endpoint state machine enters the DisablingStaticStreams2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return UcxEndpointStateEntryFunc_Common_DisablingStaticStreams(UcxEndpointContext);
} // UcxEndpointStateEntryFunc_DisablingStaticStreams2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_EnablingStaticStreams(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_EnablingStaticStreams is called when the Ucx
    endpoint state machine enters the EnablingStaticStreams State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    WDFREQUEST request;
    
    TRY {
        
        request = UcxEndpointContext->PendingStreamsEnableRequest;
        UcxEndpointContext->PendingStreamsEnableRequest = NULL;
        NT_ASSERT(NULL != request);
        UcxEndpointContext->EvtCallbacks.EvtEndpointStaticStreamsEnable(
            UcxEndpointContext->UcxEndpoint,
            UcxEndpointContext->UcxSStreamsContext->UcxSStreams,
            request);

    } FINALLY {
    
    }
    
    return SmEngineEventNull;
} // UcxEndpointStateEntryFunc_EnablingStaticStreams

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_FailingEnableStaticStreamsIrp(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_FailingEnableStaticStreamsIrp is called when the Ucx
    endpoint state machine enters the FailingEnableStaticStreamsIrp State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    PIRP irp;
    
    TRY {
        
        irp = UcxEndpointContext->PendingStreamsEnableIrp;
        UcxEndpointContext->PendingStreamsEnableIrp = NULL;
        NT_ASSERT(NULL != irp);

        InterlockedDecrement(&UcxEndpointContext->StreamsEnabled);

        irp->IoStatus.Status = STATUS_INVALID_DEVICE_STATE;
        irp->IoStatus.Information = 0;
        Xrb_IoCompleteRequest_StatusAlreadySet(irp);
    
    } FINALLY {
    
    }
    
    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_FailingEnableStaticStreamsIrp

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_FailingEndpointResetWdfRequest(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_FailingEndpointResetWdfRequest is called when the Ucx
    endpoint state machine enters the FailingEndpointResetWdfRequest State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    WDFREQUEST request;
    
    TRY {
        
        request = UcxEndpointContext->PendingResetRequest;
        UcxEndpointContext->PendingResetRequest = NULL;
        NT_ASSERT(NULL != request);

        WdfRequestComplete(request, STATUS_NO_SUCH_DEVICE);
    
    } FINALLY {
    
    }

    return SmEngineEventNull;
} // UcxEndpointStateEntryFunc_FailingEndpointResetWdfRequest

FORCEINLINE
SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Common_IsAbleToStart(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++

Routine Description:

    UcxEndpointStateEntryFunc_Common_IsAbleToStart is called when the Ucx endpoint state machine
    enters the UcxEndpointStateEntryFunc_IsAbleToStartXxx State.

Arguments:

    SM_ENGINE_EVENT: Yes or No

Comments:
    

--*/
{
    PUCXUSBDEVICE_PRIVCONTEXT ucxUsbDeviceContext;
    SM_ENGINE_EVENT           event;

    TRY {

        ucxUsbDeviceContext = UcxEndpointContext->UcxUsbDeviceContext;

        //
        // There are the following reasons we would disallow an unpurge.
        // *  The endpoint is not enabled in the hardware; OR,
        // *  Device was disconnected and will never come back; OR,
        // *  Device is currently not enabled; OR, 
        // *  Device's endpoints are not currently programmed in the controller and must be
        //    reprogrammed first; OR,
        // *  Device's parent is purged. Hub driver guarantees it won't attempt to bring a purged
        //    parent's child out of purge unless it performed the purge tree (or disconnect)
        //    recovery action higher in the tree.
        //

        if (UcxEndpointContext->Enabled == FALSE) {
            
            event = UcxEndpointEventNo;
            LEAVE;
        }

        if (ucxUsbDeviceContext->Disconnected) {

            event = UcxEndpointEventNo;
            LEAVE;
        }

        if (ucxUsbDeviceContext->DeprogrammedDueToControllerReset) {

            event = UcxEndpointEventNo;
            LEAVE;
        }

        if (ucxUsbDeviceContext->Enabled == FALSE) {

            event = UcxEndpointEventNo;
            LEAVE;
        }

        if (ucxUsbDeviceContext->AllowEndpointsToExitTreePurge == FALSE) {

            event = UcxEndpointEventNo;
            LEAVE;
        }

        event = UcxEndpointEventYes;

    } FINALLY {

    }

    return event;
} // UcxEndpointStateEntryFunc_Common_IsAbleToStart

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_IsAbleToStart1(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_IsAbleToStart1 is called when the Ucx
    endpoint state machine enters the IsAbleToStart1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return UcxEndpointStateEntryFunc_Common_IsAbleToStart(UcxEndpointContext);
} // UcxEndpointStateEntryFunc_IsAbleToStart1

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_IsAbleToStart2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_IsAbleToStart2 is called when the Ucx
    endpoint state machine enters the IsAbleToStart2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return UcxEndpointStateEntryFunc_Common_IsAbleToStart(UcxEndpointContext);
} // UcxEndpointStateEntryFunc_IsAbleToStart2

FORCEINLINE
SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Common_IsDeprogrammedDueToControllerReset(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Common_IsDeprogrammedDueToControllerReset is called when the Ucx
    endpoint state machine enters the IsDeprogrammedDueToControllerResetXXX State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    SM_ENGINE_EVENT event;

    TRY {
        
        if (UcxEndpointContext->UcxUsbDeviceContext->DeprogrammedDueToControllerReset) {
            event = UcxEndpointEventYes;
        } else {
            event = UcxEndpointEventNo;
        }

    } FINALLY {
    }
    
    return event;
} // UcxEndpointStateEntryFunc_Common_IsDeprogrammedDueToControllerReset

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_IsDeprogrammedDueToControllerReset4(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_IsDeprogrammedDueToControllerReset4 is called when the Ucx
    endpoint state machine enters the IsDeprogrammedDueToControllerReset4 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return UcxEndpointStateEntryFunc_Common_IsDeprogrammedDueToControllerReset(UcxEndpointContext);
} // UcxEndpointStateEntryFunc_IsDeprogrammedDueToControllerReset4

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_IsDeprogrammedDueToControllerResetOrDisconnected(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_IsDeprogrammedDueToControllerResetOrDisconnected is called when the Ucx
    endpoint state machine enters the IsDeprogrammedDueToControllerResetOrDisconnected State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    SM_ENGINE_EVENT event;

    TRY {
        
        if (UcxEndpointContext->UcxUsbDeviceContext->DeprogrammedDueToControllerReset ||
            UcxEndpointContext->UcxUsbDeviceContext->Disconnected) {

            event = UcxEndpointEventYes;

        } else {

            event = UcxEndpointEventNo;
        }

    } FINALLY {

    }
    
    return event;
} // UcxEndpointStateEntryFunc_IsDeprogrammedDueToControllerResetOrDisconnected

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_NeedToSetStale(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_NeedToSetStale is called when the Ucx
    endpoint state machine enters the NeedToSetStale State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    SM_ENGINE_EVENT           event;

    TRY {

        if (UcxEndpointContext->ExposedToClient) {
            event = UcxEndpointEventYes;
        } else {
            event = UcxEndpointEventNo;
        }

    } FINALLY {
    }
    
    return event;
} // UcxEndpointStateEntryFunc_NeedToSetStale

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_OkToResetControllerAndPauseSM1(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_OkToResetControllerAndPauseSM1 is called when the Ucx
    endpoint state machine enters the OkToResetControllerAndPauseSM1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    SM_ENGINE_EVENT event;
    
    TRY {
        
        event = UcxEndpointEventHandler_OkToResetControllerAndPauseSM(UcxEndpointContext);
    
    } FINALLY {
    
    }
    
    return event;
} // UcxEndpointStateEntryFunc_OkToResetControllerAndPauseSM1

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_OkToResetControllerAndPauseSM2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_OkToResetControllerAndPauseSM2 is called when the Ucx
    endpoint state machine enters the OkToResetControllerAndPauseSM2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    SM_ENGINE_EVENT event;
    
    TRY {
        
        event = UcxEndpointEventHandler_OkToResetControllerAndPauseSM(UcxEndpointContext);
    
    } FINALLY {
    
    }
    
    return event;
} // UcxEndpointStateEntryFunc_OkToResetControllerAndPauseSM2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Purging1(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Purging1 is called when the Ucx
    endpoint state machine enters the Purging1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {
        
        UcxEndpointContext->EvtCallbacks.EvtEndpointPurge(
            UcxEndpointContext->UcxControllerContext->UcxController,
            UcxEndpointContext->UcxEndpoint);

    } FINALLY {
    
    }
    
    return SmEngineEventNull;
} // UcxEndpointStateEntryFunc_Purging1

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Purging2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Purging2 is called when the Ucx
    endpoint state machine enters the Purging2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {
        
        UcxEndpointContext->EvtCallbacks.EvtEndpointPurge(
            UcxEndpointContext->UcxControllerContext->UcxController,
            UcxEndpointContext->UcxEndpoint);

    } FINALLY {
    
    }
    
    return UcxEndpointEventOperationSuccess; 
    // TODO: PostWin8: this does not match the other purging entry returns
} // UcxEndpointStateEntryFunc_Purging2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Purging3(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Purging3 is called when the Ucx
    endpoint state machine enters the Purging3 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {
        
        UcxEndpointContext->EvtCallbacks.EvtEndpointPurge(
            UcxEndpointContext->UcxControllerContext->UcxController,
            UcxEndpointContext->UcxEndpoint);

    } FINALLY {
    
    }
    
    return SmEngineEventNull;
} // UcxEndpointStateEntryFunc_Purging3

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Purging4(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Purging4 is called when the Ucx
    endpoint state machine enters the Purging4 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {
        
        UcxEndpointContext->EvtCallbacks.EvtEndpointPurge(
            UcxEndpointContext->UcxControllerContext->UcxController,
            UcxEndpointContext->UcxEndpoint);

    } FINALLY {
    
    }
    
    return SmEngineEventNull;
} // UcxEndpointStateEntryFunc_Purging4

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Purging5(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Purging5 is called when the Ucx
    endpoint state machine enters the Purging5 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {
        
        UcxEndpointContext->EvtCallbacks.EvtEndpointPurge(
            UcxEndpointContext->UcxControllerContext->UcxController,
            UcxEndpointContext->UcxEndpoint);

    } FINALLY {
    
    }
    
    return SmEngineEventNull;
} // UcxEndpointStateEntryFunc_Purging5

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_PurgingOnControllerReset(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_PurgingOnControllerReset is called when the Ucx
    endpoint state machine enters the PurgingOnControllerReset State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {
        
        UcxEndpointContext->EvtCallbacks.EvtEndpointPurge(
            UcxEndpointContext->UcxControllerContext->UcxController,
            UcxEndpointContext->UcxEndpoint);

    } FINALLY {
    
    }
    
    return UcxEndpointEventOperationSuccess; 
    // TODO: PostWin8: this does not match the other purging entry returns
} // UcxEndpointStateEntryFunc_PurgingOnControllerReset

FORCEINLINE
SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Common_RejectingEnableStaticStreamsWdfRequest(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Common_RejectingEnableStaticStreamsWdfRequest is called when the Ucx
    endpoint state machine enters the RejectingEnableStaticStreamsWdfRequestXXX State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    WDFREQUEST request;
    
    TRY {
        
        request = UcxEndpointContext->PendingStreamsEnableRequest;
        UcxEndpointContext->PendingStreamsEnableRequest = NULL;
        NT_ASSERT(NULL != request);
        WdfRequestComplete(request, STATUS_INVALID_DEVICE_STATE);
    
    } FINALLY {
    
    }
    
    return SmEngineEventNull;
} // UcxEndpointStateEntryFunc_Common_RejectingEnableStaticStreamsWdfRequest

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_RejectingEnableStaticStreamsWdfRequest2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_RejectingEnableStaticStreamsWdfRequest2 is called when the Ucx
    endpoint state machine enters the RejectingEnableStaticStreamsWdfRequest2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return UcxEndpointStateEntryFunc_Common_RejectingEnableStaticStreamsWdfRequest(UcxEndpointContext);
} // UcxEndpointStateEntryFunc_RejectingEnableStaticStreamsWdfRequest2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_ResettingEndpoint1(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_ResettingEndpoint1 is called when the Ucx
    endpoint state machine enters the ResettingEndpoint1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    TRY {

        UcxEndpoint_Resetting(UcxEndpointContext);

    } FINALLY {

    }
    return SmEngineEventNull;
} // UcxEndpointStateEntryFunc_ResettingEndpoint1

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_ResettingEndpoint2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_ResettingEndpoint2 is called when the Ucx
    endpoint state machine enters the ResettingEndpoint2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    TRY {

        UcxEndpoint_Resetting(UcxEndpointContext);

    } FINALLY {

    }
    return SmEngineEventNull;
} // UcxEndpointStateEntryFunc_ResettingEndpoint2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_ResumeSMAndWaitForPendingOperation1(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_ResumeSMAndWaitForPendingOperation1 is called when the Ucx
    endpoint state machine enters the ResumeSMAndWaitForPendingOperation1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return UcxEndpointEventHandler_ResumeSM(UcxEndpointContext);
} // UcxEndpointStateEntryFunc_ResumeSMAndWaitForPendingOperation1

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_ResumeSMAndWaitForPendingOperation2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_ResumeSMAndWaitForPendingOperation2 is called when the Ucx
    endpoint state machine enters the ResumeSMAndWaitForPendingOperation2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return UcxEndpointEventHandler_ResumeSM(UcxEndpointContext);
} // UcxEndpointStateEntryFunc_ResumeSMAndWaitForPendingOperation2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_SetStaleAndDeletePreviousStale(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_SetStaleAndDeletePreviousStale is called when the Ucx
    endpoint state machine enters the SetStaleAndDeletePreviousStale State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    KIRQL                     irql;
    PUCXUSBDEVICE_PRIVCONTEXT ucxUsbDeviceContext;
    PUCXENDPOINT_PRIVCONTEXT  currUcxEndpointContext;
    PUCXENDPOINT_PRIVCONTEXT  nextUcxEndpointContext;
    BOOLEAN                   found = FALSE;
    SM_ENGINE_EVENT           event;

    TRY {

        ucxUsbDeviceContext = UcxEndpointContext->UcxUsbDeviceContext;
        
        if (ucxUsbDeviceContext->AllowIoOnInvalidPipeHandles) {
            KeAcquireSpinLock(&ucxUsbDeviceContext->InvalidHandleTrakingLock, &irql);
            
            RemoveEntryList(&UcxEndpointContext->EndpointListEntryForInvalidHandleTraking);
            InitializeListEntry(&UcxEndpointContext->EndpointListEntryForInvalidHandleTraking);

            KeReleaseSpinLock(&ucxUsbDeviceContext->InvalidHandleTrakingLock, irql);
        }
        
        KeAcquireSpinLock(&UcxEndpointContext->UcxControllerContext->TopologyLock, &irql);

        //
        // Scan through the State Handle List and see if there is already a stale handle with this
        // endpoint number. If we find one, we can delete that stale handle now.
        //
        FOR_ALL_IN_LIST_SAFE(UCXENDPOINT_PRIVCONTEXT, 
                             &UcxEndpointContext->UcxUsbDeviceContext->StaleEndpointListHead,
                             EndpointListEntry,
                             currUcxEndpointContext,
                             nextUcxEndpointContext) {
            
            if (currUcxEndpointContext->EndpointDescriptor.bEndpointAddress == UcxEndpointContext->EndpointDescriptor.bEndpointAddress){
                
                found = TRUE;
                RemoveEntryList(&currUcxEndpointContext->EndpointListEntry);
                InitializeListEntry(&currUcxEndpointContext->EndpointListEntry);
                break;
            }            
        }

        //
        // Now mark the UcxEndpointContext pipe as invalid by clearing the Self pointer; and move
        // this endpoint to the stale list.
        //
        UcxEndpointContext->UcxControllerContext->ChildEndpointListCount--;
        RemoveEntryList(&UcxEndpointContext->EndpointListEntry);
        UcxEndpointContext->UcxPipeInfo.Self = NULL;

        if (UcxEndpointContext->UcxUsbDeviceContext->PendingDelete) {
            //
            // UsbDevice is already in a Pending Delete State. 
            // So we cannot add this endpoint to StaleEndpointListHead.
            //
            event = UcxEndpointEventUsbDeviceDeleting;
        } else {
            
            InsertTailList(&UcxEndpointContext->UcxUsbDeviceContext->StaleEndpointListHead,
                           &UcxEndpointContext->EndpointListEntry);
            
            event = UcxEndpointEventOperationSuccess;
        }


        KeReleaseSpinLock(&UcxEndpointContext->UcxControllerContext->TopologyLock, irql);

        if (found) {
            Endpoint_EventAdd(currUcxEndpointContext, UcxEndpointEventDeleteStale);
        }

    } FINALLY {
    }
    
    return event;
} // UcxEndpointStateEntryFunc_SetStaleAndDeletePreviousStale

FORCEINLINE
SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Common_SettingEnabledFlag(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++

Routine Description:

    UcxEndpointStateEntryFunc_Common_SettingEnabledFlag is called when the Ucx endpoint state machine
    enters the UcxEndpointStateEntryFunc_SettingEnabledFlagXxx State.

Arguments:

    SM_ENGINE_EVENT: Yes or No

Comments:
    

--*/
{
    TRY {

        //
        // Note: Following assert cannot be placed in the code since the endpoint might 
        // have gotten disabled due to a controller reset. 
        // In that case the UcxEndpointContext->Enabled boolean is not cleared and
        // the hub driver is allowed to skip disabling the 'deprogrammed due to controller reset'
        // endpoint before re-enabling it. 
        // NT_ASSERT(FALSE == UcxEndpointContext->Enabled);
        // 
        UcxEndpointContext->Enabled = TRUE;

    } FINALLY {

    }

    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_Common_SettingEnabledFlag

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_SettingEnabledFlag1(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_SettingEnabledFlag1 is called when the Ucx
    endpoint state machine enters the SettingEnabledFlag1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return UcxEndpointStateEntryFunc_Common_SettingEnabledFlag(UcxEndpointContext);
} // UcxEndpointStateEntryFunc_SettingEnabledFlag1

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_SettingEnabledFlag2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_SettingEnabledFlag2 is called when the Ucx
    endpoint state machine enters the SettingEnabledFlag2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return UcxEndpointStateEntryFunc_Common_SettingEnabledFlag(UcxEndpointContext);
} // UcxEndpointStateEntryFunc_SettingEnabledFlag2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Starting1(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Starting1 is called when the Ucx
    endpoint state machine enters the Starting1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {

        //
        // Synchronous call to HCD
        //
        UcxEndpointContext->EvtCallbacks.EvtEndpointStart( 
            UcxEndpointContext->UcxControllerContext->UcxController,
            UcxEndpointContext->UcxEndpoint);

    } FINALLY {

    }

    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_Starting1

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Starting2(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Starting2 is called when the Ucx
    endpoint state machine enters the Starting2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {

        //
        // Synchronous call to HCD
        //
        UcxEndpointContext->EvtCallbacks.EvtEndpointStart( 
            UcxEndpointContext->UcxControllerContext->UcxController,
            UcxEndpointContext->UcxEndpoint);

    } FINALLY {

    }

    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_Starting2

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_Starting3(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_Starting3 is called when the Ucx
    endpoint state machine enters the Starting3 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {

        //
        // Synchronous call to HCD
        //
        UcxEndpointContext->EvtCallbacks.EvtEndpointStart( 
            UcxEndpointContext->UcxControllerContext->UcxController,
            UcxEndpointContext->UcxEndpoint);

    } FINALLY {

    }

    return UcxEndpointEventOperationSuccess;
} // UcxEndpointStateEntryFunc_Starting3

SM_ENGINE_EVENT
UcxEndpointStateEntryFunc_WaitingForPurgeComplete1(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    UcxEndpointStateEntryFunc_WaitingForPurgeComplete1 is called when the Ucx
    endpoint state machine enters the WaitingForPurgeComplete1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    SM_ENGINE_EVENT event;

    TRY {
        
        event = UcxEndpointEventHandler_OkToResetControllerAndPauseSM(UcxEndpointContext);
    
    } FINALLY {
    
    }
    
    return event;
} // UcxEndpointStateEntryFunc_WaitingForPurgeComplete1


//----------------------------------------------------------------------------//

BOOLEAN
Endpoint_EventAdd(
    __in
        PUCXENDPOINT_PRIVCONTEXT      UcxEndpointContext,
    __in
        SM_ENGINE_EVENT               Event               
    )
/*++

Routine Description:

    This routine handles adding and processing new events

Arguments:

    UcxEndpointContext - Ucx context of the endpoint object
    
    Event - Event that is to be added to the Endpoint State Machine.
    
Return: 
 
    BOOLEAN that returns true if the Event was accepted and was scheduled.
    FALSE otherwise.
    
Comments:
    Due to compatibility there are restrictions on when can we handle the Abort requests from 
    So certain states in the Endpoint State Machine reject the UcxEndpointEventAbortUrbFromClient. 
    This is done so that the Abort Request can be failed on the calling thread itself (condition
    needed to meet compat)
    
--*/

{
    return StateMachineEngine_EventAdd(&UcxEndpointContext->SmEngineContext,
                                       (SM_ENGINE_EVENT)Event);
    
} // Endpoint_EventAdd
