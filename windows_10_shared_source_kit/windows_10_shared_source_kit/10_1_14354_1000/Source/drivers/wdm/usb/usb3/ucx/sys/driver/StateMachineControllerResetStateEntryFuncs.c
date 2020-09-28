/*++
Copyright (c) Microsoft Corporation

Module Name:


Abstract:

     This header file contains State Machines for ControllerReset
     This has been generated automatically from a visio file.

--*/



#include "pch.h"
#include "StateMachineControllerResetStateEntryFuncs.tmh"
//--------------------------------------------------------------------------------------------------

//
// Auto Event Handlers
//
SM_ENGINE_EVENT
ControllerResetEventHandler_Ignore(
    __in
        PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetEventHandler_Ignore is called when the Ucx
    endpoint state machine wants to perform the Ignore Operation
    for an event.

Return Value:

    SmEngineEventNull
--*/
{
    UNREFERENCED_PARAMETER(UcxControllerContext);
    return SmEngineEventNull;
} // ControllerResetEventHandler_Ignore

SM_ENGINE_EVENT
ControllerResetEventHandler_UnblockControllerResetComplete(
    __in
        PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetEventHandler_UnblockControllerResetComplete is called when the Ucx
    endpoint state machine wants to perform the UnblockControllerResetComplete Operation
    for an event.

Return Value:

    SmEngineEventNull
--*/
{
    
    KeSetEvent(&UcxControllerContext->ControllerResetCompleteProcessed,
               IO_NO_INCREMENT,
               FALSE);                
    
    return SmEngineEventNull;
} // ControllerResetEventHandler_UnblockControllerResetComplete

SM_ENGINE_EVENT
ControllerResetEventHandler_FailResetFromHub(
    __in
        PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetEventHandler_CompleteResetFromHub is called when the Ucx
    endpoint state machine wants to perform the CompleteResetFromHub Operation
    for an event.

Return Value:

    SmEngineEventNull
--*/
{
    
    //
    // Complete Reset From Hub
    //
    RootHub_CompleteAsyncReset(UcxControllerContext->UcxRootHubContext, STATUS_NO_SUCH_DEVICE);
    
    return SmEngineEventNull;
} // ControllerResetEventHandler_CompleteResetFromHub

#if 0
VOID
ControllerResetEventHandler_SetDeprogrammedForAllDevices(
    __in
        PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetEventHandler_SetDeprogrammedForAllDevices is called when the Ucx
    endpoint state machine wants to perform the SetDeprogrammedForAllDevices Operation
    for an event.

Return Value:

    VOID
--*/
{
    KIRQL irql;
    
    TRY {
        
        KeAcquireSpinLock(&UcxControllerContext->TopologyLock, &irql);

        UsbDevice_WalkTree(UcxControllerContext->UcxRootHubContext->UcxUsbDeviceContextOfUcxRootHub,
                           TRUE,
                           UsbDevice_SetDeprogrammed,
                           NULL);

        KeReleaseSpinLock(&UcxControllerContext->TopologyLock, irql);

    } FINALLY {

    }
    return;
} // ControllerResetEventHandler_SetDeprogrammedForAllDevices
#endif

//--------------------------------------------------------------------------------------------------

//
// STATE ENTRY FUNCTIONS
//


FORCEINLINE
SM_ENGINE_EVENT
ControllerResetStateEntryFunc_CallHCDToReset(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_CallHCDToReset is called when the ControllerReset
    state machine enters the CallHCDToReset1 or CallHCDToReset2 State

    This routine calls the EvtControllerReset routine of the HCD
    
Return Value:

    SM_ENGINE_EVENT

--*/
{
    TRY {

        UcxControllerContext->Config.EvtControllerReset(UcxControllerContext->UcxController);
    
    } FINALLY {
    }
    
    return SmEngineEventNull;
} // ControllerResetStateEntryFunc_CallHCDToReset


SM_ENGINE_EVENT
ControllerResetStateEntryFunc_CallHCDToReset1(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_CallHCDToReset1 is called when the ControllerReset
    state machine enters the CallHCDToReset1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_CallHCDToReset(UcxControllerContext);
} // ControllerResetStateEntryFunc_CallHCDToReset1

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_CallHCDToReset2(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_CallHCDToReset2 is called when the ControllerReset
    state machine enters the CallHCDToReset2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_CallHCDToReset(UcxControllerContext);
} // ControllerResetStateEntryFunc_CallHCDToReset2

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_ResumeRootHubIO(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_ResumeRootHubIO is called when the ControllerReset
    state machine enters the ResumeRootHubIO1 or ResumeRootHubIO2 or ResumeRootHubIO3 State

    This routine calls into Roothub to Resume the Roothub Io
Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {

        RootHub_ResumeRootHubIO(UcxControllerContext->UcxRootHubContext);

    } FINALLY {
    }
    
    return ControllerResetEventOperationSuccess;
} // ControllerResetStateEntryFunc_ResumeRootHubIO

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_ResumeRootHubIO1(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_ResumeRootHubIO1 is called when the ControllerReset
    state machine enters the ResumeRootHubIO1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_ResumeRootHubIO(UcxControllerContext);
} // ControllerResetStateEntryFunc_ResumeRootHubIO1

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_ResumeRootHubIO2(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_ResumeRootHubIO2 is called when the ControllerReset
    state machine enters the ResumeRootHubIO2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_ResumeRootHubIO(UcxControllerContext);
} // ControllerResetStateEntryFunc_ResumeRootHubIO2

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_ResumeRootHubIO3(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_ResumeRootHubIO3 is called when the ControllerReset
    state machine enters the ResumeRootHubIO3 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_ResumeRootHubIO(UcxControllerContext);
} // ControllerResetStateEntryFunc_ResumeRootHubIO3

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_CompleteResetFromHub(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_CompleteResetFromHub is called when the ControllerReset
    state machine enters the CompleteResetFromHub State

    This routine calls into Roothub to Resume the Roothub Io
Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {

        //
        // Complete Reset From Hub
        //
        RootHub_CompleteAsyncReset(UcxControllerContext->UcxRootHubContext, STATUS_SUCCESS);

    } FINALLY {
    }
    
    return ControllerResetEventOperationSuccess;
} // ControllerResetStateEntryFunc_CompleteResetFromHub


SM_ENGINE_EVENT
ControllerResetStateEntryFunc_ResetComplete(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_ResetComplete is called when the ControllerReset
    state machine enters the ResetComplete1 or ResetComplete2 or ResetComplete3 State

    This routine calls into Roothub to Resume the Roothub Io
Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {

        //
        // Resume Reset References + Clear Reset In Progress.
        //
        Controller_ClearResetInProgress(UcxControllerContext);

    } FINALLY {
    }
    
    return ControllerResetEventOperationSuccess;
} // ControllerResetStateEntryFunc_ResetComplete

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_ResetComplete1(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_ResetComplete1 is called when the ControllerReset
    state machine enters the ResetComplete1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_ResetComplete(UcxControllerContext);
} // ControllerResetStateEntryFunc_ResetComplete1

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_ResetComplete2(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_ResetComplete2 is called when the ControllerReset
    state machine enters the ResetComplete2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_ResetComplete(UcxControllerContext);
} // ControllerResetStateEntryFunc_ResetComplete2

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_ResetComplete3(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_ResetComplete3 is called when the ControllerReset
    state machine enters the ResetComplete3 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_ResetComplete(UcxControllerContext);
} // ControllerResetStateEntryFunc_ResetComplete3

#if 0
SM_ENGINE_EVENT
ControllerResetStateEntryFunc_SetDeprogrammedForAllDevices(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_SetDeprogrammedForAllDevices is called when the ControllerReset
    state machine enters the SetDeprogrammedForAllDevices State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {

        ControllerResetEventHandler_SetDeprogrammedForAllDevices(UcxControllerContext);
        
    } FINALLY {
    }
    
    return ControllerResetEventOperationSuccess;
} // ControllerResetStateEntryFunc_SetDeprogrammedForAllDevices
#endif

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_StartFailingRootHubRequests(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_StartFailingRootHubRequests is called when the 
    ControllerReset state machine enters the StartFailingRootHubRequests1 
    or StartFailingRootHubRequests2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {

        //
        // StartFailingRootHubIo
        //
        RootHub_StartFailingRootHubIO(UcxControllerContext->UcxRootHubContext);

    } FINALLY {
    }
    
    return ControllerResetEventOperationSuccess;
} // ControllerResetStateEntryFunc_StartFailingRootHubRequests

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_StartFailingRootHubRequests1(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_StartFailingRootHubRequests1 is called when the 
    ControllerReset state machine enters the StartFailingRootHubRequests1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    return ControllerResetStateEntryFunc_StartFailingRootHubRequests(
        UcxControllerContext);

} // ControllerResetStateEntryFunc_StartFailingRootHubRequests1

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_StartFailingRootHubRequests2(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_StartFailingRootHubRequests2 is called when the 
    ControllerReset state machine enters the StartFailingRootHubRequests2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    return ControllerResetStateEntryFunc_StartFailingRootHubRequests(
        UcxControllerContext);

} // ControllerResetStateEntryFunc_StartFailingRootHubRequests2

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_StopBlockingResetComplete(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_StopBlockingResetComplete is called when the ControllerReset
    state machine enters the StopBlockingResetComplete1 or StopBlockingResetComplete2 
    or StopBlockingResetComplete3 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {

        UcxControllerContext->WaitOnControllerResetComplete = FALSE;
        
    } FINALLY {
    }
    
    return ControllerResetEventOperationSuccess;
} // ControllerResetStateEntryFunc_StopBlockingResetComplete

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_StopBlockingResetComplete1(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_StopBlockingResetComplete1 is called when the ControllerReset
    state machine enters the StopBlockingResetComplete1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    return ControllerResetStateEntryFunc_StopBlockingResetComplete(UcxControllerContext);

} // ControllerResetStateEntryFunc_StopBlockingResetComplete1

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_StopBlockingResetComplete2(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_StopBlockingResetComplete2 is called when the ControllerReset
    state machine enters the StopBlockingResetComplete2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    return ControllerResetStateEntryFunc_StopBlockingResetComplete(UcxControllerContext);

} // ControllerResetStateEntryFunc_StopBlockingResetComplete2

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_StopBlockingResetComplete3(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_StopBlockingResetComplete3 is called when the ControllerReset
    state machine enters the StopBlockingResetComplete3 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    return ControllerResetStateEntryFunc_StopBlockingResetComplete(UcxControllerContext);

} // ControllerResetStateEntryFunc_StopBlockingResetComplete3

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0 is called when the ControllerReset
    state machine enters the SignalOKForRHPdoToExitD0_1 or SignalOKForRHPdoToExitD0_2 or
    SignalOKForRHPdoToExitD0_3 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {

        UcxControllerContext->WaitOnControllerResetComplete = TRUE;
        
        KeSetEvent(&UcxControllerContext->RHPdoMayExitD0,
                   IO_NO_INCREMENT,
                   FALSE);  


    } FINALLY {
    }
    
    return ControllerResetEventOperationSuccess;
} // ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_1(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_1 is called when the ControllerReset
    state machine enters the SignalOKForRHPdoToExitD0_1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    return ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0(UcxControllerContext);

} // ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_1

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_2(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_2 is called when the ControllerReset
    state machine enters the SignalOKForRHPdoToExitD0_2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    return ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0(UcxControllerContext);

} // ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_2

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_3(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_3 is called when the ControllerReset
    state machine enters the SignalOKForRHPdoToExitD0_3 State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    return ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0(UcxControllerContext);

} // ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_3

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_StartPendingDeviceMgmtRequests(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_StartPendingDeviceMgmtRequests is called when the ControllerReset
    state machine enters the StartPendingDeviceMgmtRequests State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {
        
        //
        // StartPendingDeviceMgmtRequests
        //
        WdfIoQueueStop(UcxControllerContext->QueuePendUsbDeviceMgmtDuringControllerReset,
                       NULL,
                       NULL);


    } FINALLY {
    }
    
    return ControllerResetEventOperationSuccess;
} // ControllerResetStateEntryFunc_StartPendingDeviceMgmtRequests

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_CompletePendedDeviceMgmtRequests(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_CompletePendedDeviceMgmtRequests is called when the ControllerReset
    state machine enters the CompletePendedDeviceMgmtRequests State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {
        
        //
        // CompletePendedDeviceMgmtRequests
        //
        WdfIoQueueStart(UcxControllerContext->QueuePendUsbDeviceMgmtDuringControllerReset);


    } FINALLY {
    }
    
    return ControllerResetEventOperationSuccess;
} // ControllerResetStateEntryFunc_CompletePendedDeviceMgmtRequests

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_WaitingForResetFromHub(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_WaitingForResetFromHub is called when the ControllerReset
    state machine enters the WaitingForResetFromHub State

Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {
        
        //
        // CompletePendedDeviceMgmtRequests
        //
        WdfIoQueueStart(UcxControllerContext->QueuePendUsbDeviceMgmtDuringControllerReset);


    } FINALLY {
    }
    
    return SmEngineEventNull;
} // ControllerResetStateEntryFunc_WaitingForResetFromHub

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_PrepareForReset(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_PrepareForReset is called when the ControllerReset
    state machine enters the PrepareForReset1 or PrepareForReset2 or PrepareForReset3 State

    This routine calls into RootHub module to start failing the RootHub IO
Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {
        
        //
        // StartPendingDeviceMgmtRequests
        //
        WdfIoQueueStop(UcxControllerContext->QueuePendUsbDeviceMgmtDuringControllerReset,
                       NULL,
                       NULL);

        //
        // RecordResetInProgress
        //
        Controller_SetResetInProgress(UcxControllerContext);

        //
        // StartFailingRootHubIo
        //
        RootHub_StartFailingRootHubIO(UcxControllerContext->UcxRootHubContext);

        //
        // Drops the initial ResetReference that was acquired either at the time 
        // UCXCONTROLLER object was created or during Controller_AllowResetReferences
        //
        Controller_ResetReferenceRelease(UcxControllerContext);

    } FINALLY {
    }
    
    return SmEngineEventNull;
} // ControllerResetStateEntryFunc_PrepareForReset

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_PrepareForReset1(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_PrepareForReset1 is called when the ControllerReset
    state machine enters the PrepareForReset1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_PrepareForReset(UcxControllerContext);
} // ControllerResetStateEntryFunc_PrepareForReset1

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_PrepareForReset2(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_PrepareForReset2 is called when the ControllerReset
    state machine enters the PrepareForReset2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_PrepareForReset(UcxControllerContext);
} // ControllerResetStateEntryFunc_PrepareForReset2

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete is called when the ControllerReset
    state machine enters the NotifyUsbDevicesAndEndpointsOfResetComplete1 or NotifyUsbDevicesAndEndpointsOfResetComplete2 
    or RhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete State

    This routine calls into RootHub module to start failing the RootHub IO
Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {

        //
        // RecordResetInProgress
        //
        Controller_NotifyUsbDevicesAndEndpointsOfControllerResetComplete(UcxControllerContext);

    } FINALLY {
    }
    
    return ControllerResetEventOperationSuccess;
} // ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete1(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete1 is called when the ControllerReset
    state machine enters the NotifyUsbDevicesAndEndpointsOfResetComplete1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete(UcxControllerContext);
} // ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete1

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete2(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete2 is called when the ControllerReset
    state machine enters the NotifyUsbDevicesAndEndpointsOfResetComplete2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete(UcxControllerContext);
} // ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete2

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_RhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_RhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete is called when the ControllerReset
    state machine enters the RhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete(UcxControllerContext);
} // ControllerResetStateEntryFunc_RhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_RhPdoInDx_SimulatePortChanged1(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_RhPdoInDx_SimulatePortChanged1 is called when the ControllerReset
    state machine enters the RhPdoInDx_SimulatePortChanged1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    //
    // Simulate port change to induce the hub driver to bring the root hub to D0, where we can
    // reset the controller
    //
    RootHub_PortChanged(UcxControllerContext->UcxRootHubContext);
    return ControllerResetEventOperationSuccess;
} // ControllerResetStateEntryFunc_RhPdoInDx_SimulatePortChanged

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_RhPdoInDx_SimulatePortChanged2(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_RhPdoInDx_SimulatePortChanged2 is called when the ControllerReset
    state machine enters the RhPdoInDx_SimulatePortChanged2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    //
    // Simulate port change to induce the hub driver to bring the root hub to D0, where we can
    // reset the controller
    //
    RootHub_PortChanged(UcxControllerContext->UcxRootHubContext);
    return ControllerResetEventOperationSuccess;
} // ControllerResetStateEntryFunc_RhPdoInDx_SimulatePortChanged

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset is called when the ControllerReset
    state machine enters the PrepareUsbDevicesAndEndpointsForReset1 or PrepareUsbDevicesAndEndpointsForReset2 
    or RhPdoInDx_PrepareUsbDevicesAndEndpointsForReset State

    This routine calls into RootHub module to start failing the RootHub IO
Return Value:

    SM_ENGINE_EVENT

--*/
{

    TRY {

        //
        // RecordResetInProgress
        //
        Controller_PrepareUsbDevicesAndEndpointsForControllerReset(UcxControllerContext);

    } FINALLY {
    }
    
    return SmEngineEventNull;
} // ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset1(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset1 is called when the ControllerReset
    state machine enters the PrepareUsbDevicesAndEndpointsForReset1 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset(UcxControllerContext);
} // ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset1

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset2(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset2 is called when the ControllerReset
    state machine enters the PrepareUsbDevicesAndEndpointsForReset2 State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset(UcxControllerContext);
} // ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset2

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_RhPdoInDx_PrepareUsbDevicesAndEndpointsForReset(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_RhPdoInDx_PrepareUsbDevicesAndEndpointsForReset is called when the ControllerReset
    state machine enters the RhPdoInDx_PrepareUsbDevicesAndEndpointsForReset State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    return ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset(UcxControllerContext);
} // ControllerResetStateEntryFunc_RhPdoInDx_PrepareUsbDevicesAndEndpointsForReset

SM_ENGINE_EVENT
ControllerResetStateEntryFunc_RhPdoInDx_UnblockControllerResetComplete(
    __in
      PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    )
/*++
Routine Description:

    ControllerResetStateEntryFunc_UnblockControllerResetComplete is called when the ControllerReset
    state machine enters the UnblockControllerResetComplete State

Return Value:

    SM_ENGINE_EVENT

--*/
{
    (VOID) ControllerResetEventHandler_UnblockControllerResetComplete(UcxControllerContext);
    return ControllerResetEventOperationSuccess;
} // ControllerResetStateEntryFunc_UnblockControllerResetComplete




