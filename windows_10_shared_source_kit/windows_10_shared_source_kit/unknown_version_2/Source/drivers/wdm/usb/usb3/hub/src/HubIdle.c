
/*++        
Copyright (c) Microsoft Corporation

Module Name:

    SMEngine.c

Abstract:

    This file contains the generic state machine engine,
    used by all state machines. It also contains the 
    entry points of all the state machines. 

--*/

#include <pch.h>
#include "ism_autogen.h"
#include "hubidle.tmh"

_IRQL_requires_max_(DISPATCH_LEVEL)
_Requires_lock_not_held_(SmContext->StateMachineLock)
NTSTATUS
HUBIDLE_AddEvent(
    _Inout_ 
        PISM_CONTEXT        SmContext,
    _In_    
        ISM_EVENT           Event,
    _In_opt_
        PIRP                IdleIrp          
    )
/*++

Routine Description:

    This function inserts the event in the correct queue 
    based on the event prioirity tand then runs the 
    state machine engine, if not already running

Arguments:                                
    
    SmContext - Context for the state machine
    Event - Event to be added             

Returns:

    VOID

--*/    
{

    PISM_STATE_ENTRY                currentStateEntry;
    ISM_EVENT                       nextEvent;
    ULONG                           tableIndex;
    PISM_STATE_ENTRY *              stateTable;
    ULONG                           startIndex;
    BOOLEAN                         stateChanged;
    KIRQL                           irql;
    NTSTATUS                        status;
    ISM_POSTLOCK_ACTION             postLockAction;
    PUSB_IDLE_CALLBACK_INFO         idleCallbackInfo;
    PIRP                            irpToBeCompleted;
    PISM_STATE_TRANSITION           transitions;
    ULONG                           transIndex;
    ISM_STATE                       nextState;
    PIDLE_WORKER_CONTEXT            idleWorkerContext;
    PHUB_PDO_CONTEXT                pdoContext;
    GUID                            activityId;
    BOOLEAN                         returnPending;



    TRY {                                    

        status = STATUS_PENDING;
        irpToBeCompleted = NULL;
        returnPending = FALSE;

        stateTable = SmContext->StateTable;
        startIndex = SmContext->StartIndex;  
        nextEvent = Event;            
        
        pdoContext = CONTAINING_RECORD(SmContext,
                                       HUB_PDO_CONTEXT,
                                       IdleIrpContext);


        KeAcquireSpinLock(&SmContext->StateMachineLock,
                          &irql);            

        while (nextEvent != IsmEventNull) {
            
            tableIndex = SmContext->CurrentState - startIndex;
            transitions = stateTable[tableIndex]->Transitions;       

            //
            // Look in the transition table of the current state.
            // The transition table also includes ignored events.
            //

            nextState = IsmStateNull;

            for (transIndex = 0;
                 transitions[transIndex].Event != IsmEventNull; 
                 transIndex++) {

                if (nextEvent == transitions[transIndex].Event) {

                    //
                    //  Found a transition for the current event. 
                    //                                           
                    nextState = transitions[transIndex].TargetState;
                    break;
                }
            }                     

            switch (nextState) {

            case IsmStateIgnored:
                if (Event != nextEvent) {
                    //
                    // Sync Events should never be ignored
                    //
                    DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                               TL_ERROR,
                               Device,
                               "Idle State Machine is ignoring a Sync Event");

                    HUBMISC_DbgBreak("Idle State Machine is ignoring a Sync Event");    
                }

                stateChanged = FALSE;
                break;                                               

            case IsmStateNull:
                //
                // Found a bug - not handling an event that we should be.
                // Ignore it in the fre build and hope that things keep
                // working. 
                //
                DbgTraceTo(pdoContext->AssociatedDSM->PortContext->IfrLog,
                           TL_ERROR,
                           Device,
                           "Idle State Machine is Not handling an event %d",
                           Event);

                HUBMISC_DbgBreak("Unhandled Event in Idle State Machine");                                 

                EventWriteUSBHUB3_ETW_EVENT_UNHANDLED_STATE_MACHINE_EVENT(0,
                                                                          NULL,
                                                                          NULL,
                                                                          SmContext->StartIndex,
                                                                          SmContext->CurrentState,
                                                                          Event,
                                                                          0);

                stateChanged = FALSE;
                break;

            case IsmStateCompleteWithStatusNoSuchDevice:
                status = STATUS_NO_SUCH_DEVICE;
                irpToBeCompleted = IdleIrp;
                stateChanged = FALSE;
                break;

            case IsmStateCompleteWithStatusDeviceBusy:
                status = STATUS_DEVICE_BUSY;
                irpToBeCompleted = IdleIrp;
                stateChanged = FALSE;
                break;


            default:                
                stateChanged = TRUE;  

            }
            

            if (FALSE == stateChanged) {

                break;
            }                           

            //
            // Record ths state transition
            //                                                        
            HUBIDLE_RecordState(SmContext, 
                                nextState, 
                                nextEvent); 

            //
            // Make the state transition
            //
            SmContext->CurrentState = nextState;
            
            currentStateEntry = stateTable[nextState - startIndex];

            if (currentStateEntry->EntryFunc) {
                nextEvent = currentStateEntry->EntryFunc(SmContext,
                                                         IdleIrp);      
            } else {
                nextEvent = IsmEventNull;
            }
        }

        //
        // Retrieve the PostLockAction into a local variable
        // before releasing the lock
        //
        postLockAction = SmContext->PostLockAction;
        SmContext->PostLockAction = IsmPostLockActionNone; 

        //
        // If the postlockaction if one of completing the
        // stored idle IRP, then also retrieve the IRP
        //
        switch (postLockAction) {
        
        case IsmPostLockActionCompleteIrpWithStatusCancelled:
        case IsmPostLockActionCompleteIrpWithStatusCancelledAndReturnPending:
        case IsmPostLockActionCompleteIrpWithStatusDeviceBusy:
        case IsmPostLockActionCompleteIrpWithStatusNoSuchDevice:
        case IsmPostLockActionCompleteIrpWithStatusInvalidDeviceState:
        case IsmPostLockActionCompleteIrpWithStatusSuccess:
            
            irpToBeCompleted = SmContext->IdleIrp;
            SmContext->IdleIrp = NULL;
            break;                    
        }                             

        KeReleaseSpinLock(&SmContext->StateMachineLock,
                          irql);                       

        switch (postLockAction) {
        
        case IsmPostLockActionCompleteIrpWithStatusCancelled:
            status = STATUS_CANCELLED;
            break;

        case IsmPostLockActionCompleteIrpWithStatusCancelledAndReturnPending:
            status = STATUS_CANCELLED;
            returnPending = TRUE;
            break;

        case IsmPostLockActionCompleteIrpWithStatusDeviceBusy:
            status = STATUS_DEVICE_BUSY;
            break;

        case IsmPostLockActionCompleteIrpWithStatusNoSuchDevice:
            status = STATUS_NO_SUCH_DEVICE;
            break;

        case IsmPostLockActionCompleteIrpWithStatusInvalidDeviceState:
            status = STATUS_POWER_STATE_INVALID;
            break;

        case IsmPostLockActionCompleteIrpWithStatusSuccess:
            status = STATUS_SUCCESS;
            break;      
       
        case IsmPostLockActionInvokeClientCallback:            

            RtlZeroMemory(&activityId, sizeof(GUID));
            
            if (IdleIrp != NULL) {
                HUBMISC_GetActivityIdIrp(IdleIrp, &activityId);
            }

            EventWriteUSBHUB3_ETW_EVENT_DEVICE_IDLE_CALLBACK(
                &activityId,
                pdoContext->AssociatedDSM->UsbDevice);

                idleCallbackInfo = (PUSB_IDLE_CALLBACK_INFO)
                IoGetCurrentIrpStackLocation(SmContext->IdleIrp)->Parameters.DeviceIoControl.Type3InputBuffer;
            
            idleCallbackInfo->IdleCallback(idleCallbackInfo->IdleContext);

            HUBIDLE_AddEvent(SmContext,
                             IsmEventCallbackReturned,
                             NULL);
            break;

        case IsmPostLockActionQueueWorkItem:

            idleWorkerContext = GetIdleWorkItemContext(SmContext->IdleWorker);
            idleWorkerContext->SmContext = SmContext;                         
            WdfWorkItemEnqueue(SmContext->IdleWorker);
            break;


        }
        if (irpToBeCompleted) {
            RtlZeroMemory(&activityId, sizeof(GUID));
            HUBMISC_GetActivityIdIrp(irpToBeCompleted, &activityId);

            EventWriteUSBHUB3_ETW_EVENT_DEVICE_SUBMIT_IDLE_NOTIFICATION_COMPLETE(
                &activityId,
                pdoContext->AssociatedDSM->UsbDevice,
                status);

            irpToBeCompleted->IoStatus.Status = status;

            IoCompleteRequest(irpToBeCompleted,
                              IO_NO_INCREMENT);  
        }

    } FINALLY {

    }

    if (returnPending) {
        return STATUS_PENDING;
    }
    
    return status;

}

_Use_decl_annotations_
VOID 
HUBIDLE_EvtIdleWorkItem(
    WDFWORKITEM WorkItem
    )
/*++

Routine Description:

    This is the callback for the state machine worker item. When 
    the state machine engine is running at disptach level and is 
    about to execute a state entry function that requires Passive 
    level , then it quits and queues this worker item. This worker
    item is responsible for finishing the state transition that was
    in progress and then call back into the state machine engine
    
Arguments:                                                           
    
    WorkItem - WorkItem for the state machine engine

Returns:                                                             

    VOID
    
--*/   
{
    PIDLE_WORKER_CONTEXT        idleWorkerContext;
    PISM_CONTEXT                smContext;


    TRY {

        idleWorkerContext = GetIdleWorkItemContext(WorkItem);
        
        smContext = idleWorkerContext->SmContext;

        HUBIDLE_AddEvent(smContext,
                         IsmEventWorkerRoutine,
                         NULL);


    } FINALLY {

    }

    return;                               

}  //HUBIDLE_EvtIdleWorkItem                  

_IRQL_requires_max_(DISPATCH_LEVEL)
_Requires_lock_held_(SmContext->StateMachineLock)
FORCEINLINE
VOID
HUBIDLE_RecordState(
    _In_      
        PISM_CONTEXT            SmContext,
    _In_      
        ISM_STATE               State,
    _In_      
        ISM_EVENT               Event
    )
/*++

Routine Description:

    This routine records a state transition in the log for debugging

Arguments:                                                                                     
    
    SmContext 
    State - The new state being entered
    Event - The event that resulted in this transition  

Returns:

    VOID

--*/ 
{
    PHUB_PDO_CONTEXT    pdoContext;

    TRY {

        SmContext->StateHistory[SmContext->StateHistoryIndex].State = State;
        SmContext->StateHistory[SmContext->StateHistoryIndex].Event = Event;
        SmContext->StateHistoryIndex = (SmContext->StateHistoryIndex + 1) & ISM_STATE_HISTORY_MASK;
        
        pdoContext = (PHUB_PDO_CONTEXT)CONTAINING_RECORD(SmContext, 
                                                         HUB_PDO_CONTEXT, 
                                                         IdleIrpContext);

        EventWriteUSBHUB3_ETW_EVENT_STATE_MACHINE(0,
                                                  pdoContext->AssociatedDSM->UsbDevice,
                                                  SmContext,
                                                  SmContext->StartIndex,
                                                  SmContext->StateTable[State - SmContext->StartIndex]->EtwName,
                                                  Event,
                                                  0);


    } FINALLY {

    }

    return;

}//HUBIDLE_RecordState                                                                           



_Use_decl_annotations_
ISM_EVENT
HUBIDLE_CheckIfSelectiveSuspendIsEnabled (
    PISM_CONTEXT            IdleContext,
    PIRP                    Irp
    )
/*++

Routine Description:
    
    This function checks if the Selective Suspend
    has been dsiabled by either global policy or
    by the parent controller. If it is not disabled
    then it returns OperationSucces, otherwise returns
    OperationFailure.

Arguments:                                                                                     
    
    IdleContext 
    Irp - Must be NULL

Returns:

    Ism_Event - OperationSuccess or OperationFailure

--*/
{
    
    ISM_EVENT           result;
    PHUB_PDO_CONTEXT    pdoContext;

    UNREFERENCED_PARAMETER(Irp);

    pdoContext = CONTAINING_RECORD(IdleContext,
                                   HUB_PDO_CONTEXT,
                                   IdleIrpContext);
     
    if (pdoContext->HubFdoContext->HubInformation.HubFlags.SelectiveSuspendNotSupportedByParentStack){
        
        result = IsmEventOperationFailure;

    } else {
        
        result = IsmEventOperationSuccess;
    }   

    return result;
}

_Use_decl_annotations_
ISM_EVENT
HUBIDLE_MarkIrpAsCancellable (
    PISM_CONTEXT            IdleContext,
    PIRP                    Irp
    )
/*++

Routine Description:
    
    This function attempts to set the cancellation
    routine on the Idle Irp. If is able to set the 
    routine so that the routine will be called when
    the IRP gets cancelled, this returns OperationSuccess.
    If the cancellation routine will never be called,
    because the IRP had already been cancelled, then
    it returns OperationFailure.

Arguments:                                                                                     
    
    IdleContext 
    Irp - Must be NULL

Returns:

    Ism_Event - OperationSuccess or OperationFailure

--*/
{
    
    PDRIVER_CANCEL  oldCancelRoutine;
    ISM_EVENT       result;

    UNREFERENCED_PARAMETER(Irp);
    _Analysis_assume_lock_held_(IdleContext->StateMachineLock);

    result = IsmEventOperationSuccess;

    oldCancelRoutine = IoSetCancelRoutine(IdleContext->IdleIrp,
                                          HUBIDLE_CancelIdle);

    NT_ASSERT(oldCancelRoutine == NULL);

    if (IdleContext->IdleIrp->Cancel) {
        oldCancelRoutine = IoSetCancelRoutine(IdleContext->IdleIrp,
                                              NULL);
        if (oldCancelRoutine != NULL) {
            result = IsmEventOperationFailure;
        }

    }

    return result;    
}

_Use_decl_annotations_
ISM_EVENT
HUBIDLE_UnMarkIrpAsCancellable (
    PISM_CONTEXT            IdleContext,
    PIRP                    Irp
    )
/*++

Routine Description:
    
    This function attempts to remove the cancellation
    routine on the Idle Irp. If is able to remove it 
    in time, so that the cancellation routine will not
    be called, then it returns OperationSuccess. If the
    cancellation routine is going to be called, then
    it returns OperationFailure

Arguments:                                                                                     
    
    IdleContext 
    Irp - Must be NULL

Returns:

    Ism_Event - OperationSuccess or OperationFailure

--*/
{
    
    PDRIVER_CANCEL  oldCancelRoutine;
    ISM_EVENT       result;

    UNREFERENCED_PARAMETER(Irp);
    _Analysis_assume_lock_held_(IdleContext->StateMachineLock);

    result = IsmEventOperationSuccess;

    oldCancelRoutine = IoSetCancelRoutine(IdleContext->IdleIrp,
                                          NULL);
    
    if (oldCancelRoutine == NULL) {
        result = IsmEventOperationFailure;
    }

    return result;
}

_Use_decl_annotations_
VOID
HUBIDLE_StoreRequest (
    PISM_CONTEXT            SmContext,
    PIRP                    IdleIrp
    )
/*++

Routine Description:
    
    Stores the IRP in the IdleContext
    
Arguments:                                                                                     
    
    IdleContext 
    Irp - Given Idle Irp

Returns:


--*/
{
    _Analysis_assume_lock_held_(SmContext->StateMachineLock);
    
    NT_ASSERT(IdleIrp != NULL);
    NT_ASSERT(SmContext->IdleIrp == NULL);

    SmContext->IdleIrp = IdleIrp;
    IoMarkIrpPending(IdleIrp);
}

_Use_decl_annotations_
VOID
HUBIDLE_CancelIdle(
    PDEVICE_OBJECT          Pdo,
    PIRP                    CancelIrp
    )
/*++

Routine Description:

    Cancellation Routine for the PDO Idle Irp

Arguments:

Return Value:

    none.

--*/
{
    PHUB_PDO_CONTEXT    pdoContext;

    pdoContext =  (PHUB_PDO_CONTEXT)Pdo->DeviceExtension;
    
    //
    // Release cancel spinlock immediately,
    // We are protected by the Idle State Machine Lock
    //
    IoReleaseCancelSpinLock(CancelIrp->CancelIrql);

    HUBIDLE_AddEvent(&pdoContext->IdleIrpContext,
                     IsmEventIrpCancelled,
                     NULL);  


}


