/*++
Copyright (c) Microsoft Corporation

Module Name:

StateMachineEngine.c

Abstract:

     This header file contains State Machines for UCX .

--*/



#include "pch.h"

#include "StateMachineEngine.tmh"

/*
FORCEINLINE
__drv_acquiresExclusiveResource(SMEngineLock)
VOID
StateMachineEngine_AcquireLock(
    __in
        PKSPIN_LOCK StateLock,
    __in __drv_savesIRQL
        PKIRQL      irql
)
{
    KeAcquireSpinLock(StateLock, irql);
}

FORCEINLINE
__drv_mustHold(SMEngineLock)
__drv_releasesExclusiveResource(SMEngineLock)
VOID
StateMachineEngine_ReleaseLock(
    __in
        PKSPIN_LOCK StateLock,
    __in __drv_restoresIRQL
        KIRQL       irql
)
{
    KeReleaseSpinLock(StateLock, irql);
}
*/

//__drv_mustHold(SMEngineLock)
SM_ENGINE_EVENT
StateMachineEngine_DequeueEvent(
    __in
        PSM_ENGINE_CONTEXT                      SmEngineContext,
    __in
        PSM_ENGINE_STATE_TABLE_ENTRY            CurrentState
    )
/*++

Routine Description:

    Retrieves the next event to handle and determines what the next state
    should be.
    Note:  The state machine lock must be held when this function is called


--*/

{

    SM_ENGINE_EVENT           event;

    TRY {

        WHILE (TRUE) {

            if (FALSE == IsQueueEmpty(&SmEngineContext->NormalPriQueue)) {

                event = DequeueEvent(&SmEngineContext->NormalPriQueue);

            } else if ((CurrentState->StateFlags & SmEngineStateFlagHandlesLowPriEvents)
                       && (FALSE == IsQueueEmpty(&SmEngineContext->LowPriQueue))) {

                event = DequeueEvent(&SmEngineContext->LowPriQueue);

            } else {

                event = SmEngineEventNull;
                break;
            }

            break;

        }


    } FINALLY {

    }

    return event;

} // StateMachineEngine_DequeueEvent

//__drv_mustHold(SMEngineLock)
__drv_requiresIRQL(DISPATCH_LEVEL)
PFN_SM_ENGINE_AUTO_EVENT_HANDLER
StateMachineEngine_GetAutoEventHandler(
    __in
        PSM_ENGINE_CONTEXT             SmEngineContext,
    __in
        PSM_ENGINE_STATE_TABLE_ENTRY   CurrentState,
    __in
        SM_ENGINE_EVENT                Event
    )
/*++

Routine Description:

    Determines if there is an auto event handler for this state.
    If there is one, that handler is returned.

    Note:  The state machine lock must be held when this function is called

Arguments:

    SmEngineContext - State Machine Engine Context

    CurrentState - The current state of the state machine.

    Event - Event that is being handled by State Machine.

Return:
    Returns the Auto Event Handler if any.

--*/
{

    ULONG                            index;
    PFN_SM_ENGINE_AUTO_EVENT_HANDLER autoEventHandler;

    TRY {

        //
        // Ok, so now we have an event, we need to figure out which state is
        // next.
        //

        autoEventHandler = NULL;
        for (index = 0;
              CurrentState->AutoEventTransitions[index].Event != SmEngineEventNull;
              index++) {

            if (CurrentState->AutoEventTransitions[index].Event == Event) {

                autoEventHandler = CurrentState->AutoEventTransitions[index].AutoEventHandler;

                NT_ASSERT(autoEventHandler != NULL);

                RecordState(SmEngineContext, CurrentState->State, Event);

                break;

            }

        }

    } FINALLY {


    }

    return autoEventHandler;

} // StateMachineEngine_GetAutoEventHandler


//__drv_mustHold(SMEngineLock)
__drv_requiresIRQL(DISPATCH_LEVEL) 
PSM_ENGINE_STATE_TABLE_ENTRY
StateMachineEngine_GetNextState(
    __in
        PSM_ENGINE_CONTEXT             SmEngineContext,
    __in
        PSM_ENGINE_STATE_TABLE_ENTRY   CurrentState,
    __in
        SM_ENGINE_EVENT                Event
    )
/*++

Routine Description:

    Retrieves the next event to handle and determines what the next state
    should be.
    Note:  The state machine lock must be held when this function is called

Arguments:

    SmEngineContext - State Machine Engine Context

    CurrentState - The current state of the state machine.

    Event - Event that is being handled by State Machine.

Return:
    Returns the next state.

--*/
{
    
    PSM_ENGINE_STATE_TABLE_ENTRY    nextStateEntry;    
    ULONG                           nextStateIndex;
    ULONG                           index;
    
    TRY {                                             
        
        //
        // Ok, so now we have an event, we need to figure out which state is
        // next.
        //

        for (index = 0;
              CurrentState->Transitions[index].Event != SmEngineEventNull;
              index++) {

            if (CurrentState->Transitions[index].Event == Event) {

                break;

            }

        }

        //
        // If the found event is SmEngineEventNull, then something is wrong.
        // We do not expect this event.
        //
        if (CurrentState->Transitions[index].Event == SmEngineEventNull) {

            Ucx_DbgBreak("Unexpected Event");

            nextStateEntry = NULL;
            RecordState(SmEngineContext, UcxEndpointStateNull, Event);

        } else {

            nextStateIndex = CurrentState->Transitions[index].TargetStateIndex;
            nextStateEntry = SmEngineContext->StateTable[nextStateIndex];
            RecordState(SmEngineContext, nextStateEntry->State, Event);

        }


    } FINALLY {


    }

    return nextStateEntry;

} // StateMachineEngine_GetNextState

UCX_FORWARD_PROGRESS_WORKITEM_CALLBACK StateMachineEngine_OnPassive;

VOID
StateMachineEngine_OnPassive(
    PDEVICE_OBJECT                    DeviceObject,
    PVOID                             SmEngineContextAsPVOID,
    PUCX_FORWARD_PROGRESS_WORKITEM    UcxForwardProgressWorkItem
)
/*++

Routine Description:

    OnPassive Callback that is called when State Machine Engine wants to invoke a StateEntryFunction
    on Passive Level.

Arguments:

    DeviceObject - (Unused) The Wdm Device Object

    SmEngineContext - State Machine Engine Context

    UcxForwardProgressWorkItem - (UnUsed) Pointer to a Ucx Forward Progress Workitem

--*/
{
    PSM_ENGINE_CONTEXT smEngineContext = (PSM_ENGINE_CONTEXT) SmEngineContextAsPVOID;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(UcxForwardProgressWorkItem);

    StateMachineEngine_EventAdd(smEngineContext, SmEngineEventOnPassive);

}

VOID
StateMachineEngine_ReleaseResources(
    __in
        PSM_ENGINE_CONTEXT            SmEngineContext
)
/*++

Routine Description:

    OnPassive Callback that is called when State Machine Engine wants to free
    resources
Arguments:

    SmEngineContext - State Machine Engine Context

--*/
{

    TRY {
        Controller_ForwardProgressWorkItemDelete(SmEngineContext->UcxForwardProgressWorkItem);
    } FINALLY {
    }
}

BOOLEAN
StateMachineEngine_EventAdd(
    __in
        PSM_ENGINE_CONTEXT       SmEngineContext,
    __in
        SM_ENGINE_EVENT          Event
    )
/*++

Routine Description:

    This routine handles adding and processing new events

Arguments:

    SmEngineContext - State Machine Engine Context

    Event - Event that is to be added to the State Machine.

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
    PSM_ENGINE_STATE_TABLE_ENTRY      currentStateEntry;
    PSM_ENGINE_STATE_TABLE_ENTRY      nextStateEntry;
    PFN_SM_ENGINE_AUTO_EVENT_HANDLER  autoEventHandler;
    BOOLEAN                           acceptEvent;
    BOOLEAN                           releaseSpinLock;
    BOOLEAN                           releaseRef = FALSE;
    SM_ENGINE_EVENT                   nextEvent;
    KIRQL                             irql;

    TRY {

        //
        // First, grab the state machine lock
        //

        KeAcquireSpinLock(&SmEngineContext->StateLock, &irql);

        releaseSpinLock = TRUE;

        //
        // Log this event in the history
        //
        RecordEvent(SmEngineContext, Event);
        
        currentStateEntry = SmEngineContext->CurrentStateEntry;
        
        //
        // Special Check for The AbortUrb from client to maintain compatiblity.
        // This ensure that only certain states handle the Abort Urb, in other cases the
        // abort urb must be failed on the same thread.
        //
        if (SmEngineContext->SpecialHandlingForAbortUrb != 0) {
            if (Event == UcxEndpointEventAbortUrbFromClient) {
                if (FALSE == TEST_FLAG(currentStateEntry->StateFlags,
                                       SmEngineStateFlagHandlesAbortUrb)) {
                    acceptEvent = FALSE;
                    LEAVE;
                }
            }
        }

        //
        // Special check for ControllerResetEventHCDNeedsReset and
        // ControllerResetEventResetCompleteFromHCD events.
        // These events can come many times, and this check ensure that maximum 1 of each
        // event is stored in the event queue at a time. States must still handle repeated
        // events, we are just protecting the queue from overflowing with too many in a
        // short time.
        //
        if (SmEngineContext->SpecialHandlingForControllerResetEventsFromHCD != 0) {
            if (ControllerResetEventHCDNeedsReset == Event) {
                if (SmEngineContext->EventHCDNeedsResetIsQueued == 1) {
                    acceptEvent = FALSE;
                    LEAVE;
                } else {
                    SmEngineContext->EventHCDNeedsResetIsQueued = 1;
                }
            } else if (ControllerResetEventResetCompleteFromHCD == Event) {
                if (SmEngineContext->EventResetCompleteFromHCDIsQueued == 1) {
                    acceptEvent = FALSE;
                    LEAVE;
                } else {
                    SmEngineContext->EventResetCompleteFromHCDIsQueued = 1;
                }
            }
        }

        acceptEvent = TRUE;

        //
        // NOTE: UcxEndpointEventControllerResetComplete needs Special Handling since
        // it needs to be handled when the state machine is Paused.
        //
        if (Event == UcxEndpointEventControllerResetComplete) {
            if (SmEngineContext->StateMachinePaused == FALSE ||
                SmEngineContext->StateMachineRunning == FALSE) {
                SmEngineContext->UnexpectedEventCount++;
                //
                // On recieving UcxEndpointEventControllerResetComplete
                // State machine should be in a Paused State
                //
                Ucx_DbgBreak("On recieving UcxEndpointEventControllerResetComplete State machine should be in a Paused State");
                LEAVE;
            }
        }

        if (Event == SmEngineEventOnPassive) {
            if (SmEngineContext->StateMachinePausedForPassive == FALSE ||
                SmEngineContext->StateMachineRunning == FALSE) {
                SmEngineContext->UnexpectedEventCount++;
                //
                // On recieving UcxEndpointEventControllerResetComplete, State machine should be 
                // in a Paused For Passive State
                //
                Ucx_DbgBreak("On recieving UcxEndpointEventControllerResetComplete, State machine should be in a Paused For Passive State");
                LEAVE;
            }
        }

        if (Event != SmEngineEventOnPassive &&
            Event != UcxEndpointEventControllerResetComplete) {

            //
            // Ok, time to enqueue this event
            //
            switch (EVENT_PRIORITY(Event)) {
            case SmEngineEventPriorityNormal:
                EnqueueEvent(&SmEngineContext->NormalPriQueue,
                             Event);
                break;
            case SmEngineEventPriorityLow:
                EnqueueEvent(&SmEngineContext->LowPriQueue,
                             Event);
                break;
            default:
                NT_ASSERTMSG("Invalid Type of Event %x", Event);
                break;
            }

            if (SmEngineContext->StateMachineRunning == TRUE) {
                //
                // The state machine is already handling an event, so we need
                // to bail out
                //
                LEAVE;
            }

            SmEngineContext->StateMachineRunning = TRUE;

        }
        //
        // As the state machine runs, events get handled for our Owner Object.
        // One of the event could actually delete the Owner object.
        // Thus to prevent a bugcheck, hold an additional reference on
        // the Object, and its needed children.
        //
        WdfObjectReferenceWithTag(SmEngineContext->Owner, TAG(StateMachineEngine_EventAdd));

        releaseRef = TRUE;

        if (Event == UcxEndpointEventControllerResetComplete) {

            //
            // This is a special Event that needs to be handled when the state machine is
            // in a pused state.
            //
            NT_ASSERT(SmEngineContext->StateMachinePaused);

            nextEvent = Event;

        } else if (Event == SmEngineEventOnPassive) {

            //
            // This is a special Event that is used to indicate that the state machine
            // engine has transitioned to Passive Level.
            //
            RecordState(SmEngineContext, currentStateEntry->State, Event);
            
            SmEngineContext->StateMachinePausedForPassive = FALSE;

            KeReleaseSpinLock(&SmEngineContext->StateLock, irql);

            //
            // Now that we are on Passive call the state machine'e callback routine.
            //
            nextEvent = currentStateEntry->StateEntryFunction(
                SmEngineContext->StateEntryFunctionContext);

            KeAcquireSpinLock(&SmEngineContext->StateLock, &irql);

        } else {

            nextEvent = SmEngineEventNull;
        }

        //
        // Now start handling the events
        //
        WHILE (TRUE) {

            if (nextEvent == SmEngineEventNull) {

                nextEvent = StateMachineEngine_DequeueEvent(SmEngineContext, currentStateEntry);

            }

            if (SmEngineContext->SpecialHandlingForControllerResetEventsFromHCD != 0) {
                if (ControllerResetEventHCDNeedsReset == nextEvent) {
                    NT_ASSERT(SmEngineContext->EventHCDNeedsResetIsQueued == 1);
                    SmEngineContext->EventHCDNeedsResetIsQueued = 0;
                }
                if (ControllerResetEventResetCompleteFromHCD == nextEvent) {
                    NT_ASSERT(SmEngineContext->EventResetCompleteFromHCDIsQueued == 1);
                    SmEngineContext->EventResetCompleteFromHCDIsQueued = 0;
                }
            }

            if (nextEvent == SmEngineEventNull) {

                //
                // No More Events
                //

                SmEngineContext->StateMachineRunning = FALSE;

                LEAVE;

            }

            autoEventHandler = StateMachineEngine_GetAutoEventHandler(SmEngineContext,
                                                                      currentStateEntry,
                                                                      nextEvent);

            if (autoEventHandler) {

                KeReleaseSpinLock(&SmEngineContext->StateLock, irql);

                nextEvent = (*autoEventHandler)(SmEngineContext->StateEntryFunctionContext);

                if (nextEvent == SmEngineEventPaused) {
                    releaseSpinLock = FALSE;
                    //
                    // We need to bail out here.
                    // UcxEndpointEventControllerResetComplete would be the one
                    // responsible to resume the statemachine.
                    //
                    LEAVE;
                }
                KeAcquireSpinLock(&SmEngineContext->StateLock, &irql);
                continue;

            }

            nextStateEntry = StateMachineEngine_GetNextState(SmEngineContext, 
                                                             currentStateEntry, 
                                                             nextEvent);

            nextEvent = SmEngineEventNull;
            
            if (nextStateEntry == NULL) {
                //
                // Case of an unexpected Event. Try to make further progress by ignorning the event.
                //
                SmEngineContext->UnexpectedEventCount++;
                continue;
            }
            
            SmEngineContext->CurrentStateEntry = nextStateEntry;
            currentStateEntry = nextStateEntry;

            if (currentStateEntry->StateEntryFunction != NULL) {

                if (TEST_FLAG(currentStateEntry->StateFlags, SmEngineStateFlagRequiresPassive) &&
                    (irql != PASSIVE_LEVEL)) {

                    //
                    // The State Entry routine must be called at Passive Level, however we are
                    // not at passive level.
                    //
                    SmEngineContext->StateMachinePausedForPassive = TRUE;

                    RecordState(SmEngineContext,
                                SmEngineStateWaitingForPassive,
                                SmEngineEventNeedsPassive);

                    Controller_ForwardProgressWorkItemEnqueue(
                        SmEngineContext->UcxForwardProgressWorkItem,
                        StateMachineEngine_OnPassive,
                        SmEngineContext,
                        FALSE);

                    LEAVE;
                }

                KeReleaseSpinLock(&SmEngineContext->StateLock, irql);


                nextEvent = currentStateEntry->StateEntryFunction(
                    SmEngineContext->StateEntryFunctionContext);

                if (nextEvent == SmEngineEventPaused) {
                    releaseSpinLock = FALSE;
                    //
                    // We need to bail out here.
                    // UcxEndpointEventControllerResetComplete would be the one
                    // responsible to resume the statemachine.
                    //
                    LEAVE;
                }
                KeAcquireSpinLock(&SmEngineContext->StateLock, &irql);

            }

        }

    } FINALLY {

        if (releaseSpinLock) {
            KeReleaseSpinLock(&SmEngineContext->StateLock, irql);
        }

        if (releaseRef) {
            WdfObjectDereferenceWithTag(SmEngineContext->Owner, TAG(StateMachineEngine_EventAdd));
        }

    }

    return acceptEvent;

} // StateMachineEngine_EventAdd

_Must_inspect_result_
NTSTATUS
StateMachineEngine_Init(
    __in
        PUCXCONTROLLER_PRIVCONTEXT                UcxControllerContext,
    __out
        PSM_ENGINE_CONTEXT                        SmEngineContext,
    __in
        WDFOBJECT                                 Owner,
    __in
        PVOID                                     StateEntryFunctionContext,
    __in
        ULONG                                     InitialStateIndex,
    __in
        PSM_ENGINE_STATE_TABLE_ENTRY*             StateTable,
    __in
        PFN_SM_ENGINE_ETW_WRITE_STATE_EVENT       EtwWriteStateEvent,
    __in
        PFN_SM_ENGINE_ETW_WRITE_STATE_TRANSITION  EtwWriteStateTransition
)
/*++

Routine Description:

    This routine Initializes the StateMachine Engine Context

Arguments:

    UcxControllerContext - The Ucx Controller Context

    SmEngineContext - State Machine Engine Context, to be initialized

    Owner - WdfObject indicating the owner of the state machine

    StateEntryFunctionContext - Pointer to the context that is passed in each state entry function
    
    InitialStateIndex - Index of the Initial State in which the state machine engine will be in
    
    StateTable - Pointer to the state table.        

Return:

    STATUS_INSUFFICIENT_RESOURCES - insufficient resources
    NTSTATUS

Comments:

    For state entry function on Passive Level, we use UCX_FORWARD_PROGRESS_WORKITEM.
    For that we must allocate a Wdm WorkItem

    Caller must call StateMachineEngine_ReleaseResources to release any resources acquired.

--*/
{
    PDEVICE_OBJECT                  wdmDeviceObject;
    NTSTATUS                        status;
    PUCX_FORWARD_PROGRESS_WORKITEM  ucxWorkItem;

    TRY {

        RtlZeroMemory(SmEngineContext, sizeof(SM_ENGINE_CONTEXT));
        SmEngineContext->Owner = Owner;
        SmEngineContext->StateEntryFunctionContext = StateEntryFunctionContext;
        
        // The State table is generated by the Visio to C convertor. Since the tables are 
        // built statically in the code, not modified at run time, and not exposed to any component 
        // outside ucx, there is
        // little need to pass information about sizes of the table. However Prefast complains
        // about it. So suppressing that error
        #pragma prefast(suppress: 26007)
        SmEngineContext->CurrentStateEntry = StateTable[InitialStateIndex];
        SmEngineContext->StateTable = StateTable;
        KeInitializeSpinLock(&SmEngineContext->StateLock);

        wdmDeviceObject = WdfDeviceWdmGetDeviceObject(UcxControllerContext->ParentWdfDevice);

        ucxWorkItem = Controller_ForwardProgressWorkItemAllocate(UcxControllerContext,
                                                                 wdmDeviceObject,
                                                                 UCX_FORWARD_PROGRESS_WORKITEM_NO_FLAGS);

        if (ucxWorkItem == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        SmEngineContext->UcxForwardProgressWorkItem = ucxWorkItem;
        SmEngineContext->EtwWriteStateEvent = EtwWriteStateEvent;
        SmEngineContext->EtwWriteStateTransition = EtwWriteStateTransition;

        status = STATUS_SUCCESS;

        RecordEvent(SmEngineContext, SmEngineEventStart);
        RecordState(SmEngineContext, SmEngineContext->CurrentStateEntry->State, SmEngineEventStart);

        //
        // Initial state must not have a state entry function (because our code currently does
        // not support executing it)
        //
        NT_ASSERT(StateTable[InitialStateIndex]->StateEntryFunction == NULL);

    } FINALLY {
    }

    return status;
}

