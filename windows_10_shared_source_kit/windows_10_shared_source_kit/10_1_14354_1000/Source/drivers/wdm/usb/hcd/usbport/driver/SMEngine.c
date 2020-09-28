
/*++        
Copyright (c) Microsoft Corporation

Module Name:

    SMEngine.c

Abstract:

    This file contains the generic state machine engine,
    used by all state machines. It also contains the 
    entry points of all the state machines. 

--*/

#include "common.h"
#include "rtpm_autogen.h"

#ifdef USBPORT_WPP
#include "smengine.tmh"
#endif
      

const CHAR StateMachineTag[]        = "State Machine Tag";
                                                                     

__drv_maxIRQL(DISPATCH_LEVEL)
__drv_mustHold(StateMachineLock)
EVENT
USBPORTRTPM_GetNextEvent(
    __inout 
        PSM_CONTEXT         SmContext
    )
/*++

Routine Description:

    This routine looks at the event disptach policy 
    of the current state and the various event queues
    to fetch the next event that should be handled by
    the state machine engine

Arguments: 
    
    SmContext - Context for the state machine 
    
Returns:

    EVENT - Next event that the state machine needs to handle

--*/                                
{

    PSTATE_ENTRY                *stateTable;
    ULONG                       startIndex;
    PSTATE_ENTRY                stateEntry;
    EVENT                       event;
    ULONG                       stateDispatchFlags;
    ULONG                       depth;  
    ULONG                       tableIndex;
    

    TRY {

        startIndex = SmContext->StartIndex;
        
        stateTable = SmContext->StateTable;

        depth = SmContext->CurrentStateDepth;

        //
        // Which event to dequeue depends on the dispatch policy of the 
        // the current state. A state that is marked as PopIfParentStateChanges 
        // inherits dispatch policy from its parent. So we need to go up the 
        // tree till a state found which governs its own dispatch policy
        //

        tableIndex = SmContext->CurrentState[depth].StateAsUlong - startIndex;

        stateEntry = stateTable[tableIndex];

        while ((depth > 0) &&
               (stateEntry->StateFlags & StateFlagPopIfParentStateChanges)) {

            depth--; 
            
            tableIndex = SmContext->CurrentState[depth].StateAsUlong - startIndex;

            stateEntry = stateTable[tableIndex];

        } 

        //
        // If the depth went to zero and the state is still marked 
        // as PopIfParentStateChanges, then it is a error.
        //
        if (stateEntry->StateFlags & StateFlagPopIfParentStateChanges) {

            NT_ASSERTMSG("State is incorrectly marked as a pop state.",
                         FALSE);
        }                                                           

        //
        // A state is found which has its own dispatch policy
        // Extract the dispatch flags.
        //
        stateDispatchFlags = stateEntry->StateFlags & 
                             (StateFlagHandlesLowPriEvents | 
                              StateFlagHandlesCriticalEventsOnly);


        //
        // Look in the decreasing order of priority and 
        // find the event to handle. Note that a state
        // can defer/mask low prioirty or normal prioirity
        // events. In such a case, the event will remain 
        // queued for some future state to handle it.
        //

        event = USBPORTRTPM_DequeueEvent(&SmContext->EventQueue,
                                         stateDispatchFlags);


    } FINALLY {

    }

    return event;

} //USBPORTRTPM_GetNextEvent


__drv_maxIRQL(DISPATCH_LEVEL)
__drv_neverHold(StateMachineLock)
BOOLEAN
USBPORTRTPM_FindAndSetTargetState(
    __inout 
        PSM_CONTEXT         SmContext,
    __in    
        EVENT               Event
    )
/*++

Routine Description:

    This routine takes the event that is being handled by
    the state machine and figures out the Target State that
    the State Machine should move to. It also changes the 
    current state and depth accordingly.It is possible that
    the state that handles the event is at a higher level than
    the current state, in which case this function will also
    Pop the intermediate sub state machines. 
    There are some special states that indicate that the state
    machine engine just needs to do some action and the actual
    state of the state machine does not need to change. The 
    function returns if the state was changed or not.

Arguments:
    
    SmContext - Context for the state machine 
    
    Event - Event to be handled

Returns:

    BOOLEAN - Indicates whether the function changed the current
              state of the state machine or not.  

--*/                                
{

    STATE                       nextState;
    ULONG                       depth;  
    ULONG                       transIndex;
    PSTATE_TRANSITION           transitions;
    ULONG                       tableIndex;
    BOOLEAN                     stateChanged;
    PSTATE_ENTRY                *stateTable;
    ULONG                       startIndex;
    ULONG                       i;
    EVENT                       nullEvent;


    TRY {
        
        nullEvent.EventAsUlong = GenericEventNull;

        depth = SmContext->CurrentStateDepth;
        stateTable = SmContext->StateTable;
        startIndex = SmContext->StartIndex;

        //
        // Start from the current state and keep going
        // upwards(i.e. to its parent) till we find a state 
        // that handles the event. Note that ignoring an event
        // is also considered handling. Also note that a parent
        // state does not have an option of deferring an event.
        //
        
        WHILE(TRUE) {            

            tableIndex = SmContext->CurrentState[depth].StateAsUlong - startIndex;

            transitions = stateTable[tableIndex]->Transitions;       

            //
            // Look in the transition table of the current state.
            // The transition table also includes ignored events.
            //

            nextState.StateAsUlong = GenericStateNull;
            
            for (transIndex = 0;
                 transitions[transIndex].Event.EventAsUlong != GenericEventNull; 
                 transIndex++) {

                if (Event.EventAsUlong ==
                    transitions[transIndex].Event.EventAsUlong) {

                    //
                    //  Found a transition for the current event. 
                    //                                           
                    nextState = transitions[transIndex].TargetState;
                    break;
                }
            }

            if ((nextState.StateAsUlong != GenericStateNull) ||
                (depth == 0)) {

                break;
            }

            //
            // Go one level up and see if that state handles the event
            //
            depth--;

        }


        //
        // There are special target states that the state machine understands. 
        // For such target states, we don't really need to change the state
        // We just need to do a special operation. For all other states, we 
        // will do the regular processing and run the state entry function.
        // The motivation behind having these special states is to prevent
        // cluterring up the state machine with these simple failure paths.
        //                                                       
        switch (nextState.StateAsUlong) {
        
        case GenericStateIgnored:
            //
            // Sync Events should never be ignored
            //
            NT_ASSERTMSG("A Sync Event is being ignored",
                         EVENT_PRIORITY(Event) != EventPrioritySync);    

            stateChanged = FALSE;

            break;                                               

        case GenericStateNull:
            //
            // Found a bug - not handling an event that we should be.
            // Ignore it and hope that things keep working. 
            //
            NT_ASSERTMSG("Unhandled Event",
                         FALSE);
            
            //
            // Break in the fre build as well
            //
            //USBPORTRTPM_DbgBreak("Unhandled Event");

            //
            // TODO: Log ETW event
            //

            stateChanged = FALSE;

            break;


        default:

            stateChanged = TRUE;                                           

        }

        if (FALSE == stateChanged) {

            LEAVE;
        }                           

        //
        // If a parent state moves when the child state machine was 
        // still running, then it is an implicit pop of the child state
        // machine. Validate that the child state machine was in a state 
        // which has the flag STATE_FLAG_POP_IF_PARENT_STATE_CHANGES
        // Otherwise the parent state is moving unexpectedly and it is
        // a bug. 
        // 
        // The sole purpose of the following If block is aid debugging. It
        // does not really do anything as such
        //
        if (depth != SmContext->CurrentStateDepth) {

            for (i = SmContext->CurrentStateDepth; i > depth ; i--) {
                
                tableIndex = SmContext->CurrentState[i].StateAsUlong - startIndex;

                //
                // Record the "Pop" operation. The fact that we are storing 
                // NullEvent and that the depth is one less than the last
                // depth, will tell the debugger extension that this is a
                // Pop operation. We store the state that was before the 
                // Pop, so that it is possible to build the complete state
                // transitions starting from the end and going backwards.
                //

                USBPORTRTPM_RecordState(SmContext , 
                                        SmContext->CurrentState[i - 1], 
                                        i - 1, 
                                        nullEvent); 
                
                
                NT_ASSERTMSG("Parent state moved but the child state doesn't allow it",
                             stateTable[tableIndex]->StateFlags &
                             StateFlagPopIfParentStateChanges);
            }
        }       

        //
        // Make the state transition
        //
        SmContext->CurrentState[depth].StateAsUlong = nextState.StateAsUlong;
        SmContext->CurrentStateDepth = depth;


        //
        // Record ths state transition
        //                                                        
        USBPORTRTPM_RecordState(SmContext , 
                                nextState, 
                                depth, 
                                Event); 
        
        stateChanged = TRUE;  

        //
        // This flag is only for the purpose of debugging. It
        // allows the user to monitor the state transitions
        // for a particular object
        //
        if (SmContext->DebugBreakOnStateTransition) {

      //      USBPORTRTPM_DbgBreak("Break On State Transition");
        }

    } FINALLY {

    }

    return stateChanged;

} //USBPORTRTPM_FindAndSetTargetState


__drv_maxIRQL(DISPATCH_LEVEL)
__drv_neverHold(StateMachineLock)
EVENT
USBPORTRTPM_ExecuteEntryFunctionsAndPushPopStateMachinesForCurrentState(
    __inout 
        PSM_CONTEXT         SmContext
    )
/*++

Routine Description:

    This routine first executes the state entry function of the 
    current state at the current depth. Then it checks if the
    current state requires state machines to be pushed or popped.
    As it pushes any sub state machines, it also exceutes their
    state entry functions. As it pops state machines, it flushes
    events for those states.It returns the sync event returned by
    any of these state entry functions. State Machine design guarantees
    that only the child most state entry function is allowed to
    return an event.

Arguments:                     
    
    SmContext - Context for the state machine

Returns:

    EVENT - Sync event returned by any of the state entry functions. 
            There will be utmost one state that returns an event.
            
--*/                                
{

    PSTATE_ENTRY                currentStateEntry;
    EVENT                       syncEvent;
    STATE                       matchingSubSmStartState;
    ULONG                       depth;  
    ULONG                       transIndex;
    PSTATE_TRANSITION           transitions;
    PSUBSM_ENTRY                subSmEntries;
    ULONG                       subSmIndex;
    ULONG                       tableIndex;
    PSTATE_ENTRY                *stateTable;
    ULONG                       startIndex;
    EVENT                       nullEvent;
    KIRQL                       irql;


    TRY { 
            
        nullEvent.EventAsUlong = GenericEventNull;

        stateTable = SmContext->StateTable;
        
        startIndex = SmContext->StartIndex;
        
        depth = SmContext->CurrentStateDepth;

        tableIndex = SmContext->CurrentState[depth].StateAsUlong - startIndex;

        currentStateEntry = stateTable[tableIndex];

        //
        // Call the state entry function. If that function 
        // returns an event , it is a sync event that will
        // be returned back from this function
        // event queues
        //                                
        syncEvent.EventAsUlong = currentStateEntry->EntryFunc(SmContext);   
                                                                         
        //
        // If the target state has a child sub state machine, then
        // it needs to be "pushed". There can be multiple levels
        // of this push.
        //                                                                  
        subSmEntries =  currentStateEntry->SubSmEntries;

        while (subSmEntries != NULL) {

            //
            // Any state that has a substate should not return a sync Event
            //
            NT_ASSERTMSG("A State has a SubState as well as Sync Event",
                         syncEvent.EventAsUlong == GenericEventNull);


            //
            // Check the sub state machine flags to see if
            // the sub state machine criterion are satisfied
            //         
            
            matchingSubSmStartState.StateAsUlong = GenericStateNull;

            for (subSmIndex = 0; 
                 subSmEntries[subSmIndex].SubSmStartState.StateAsUlong != GenericStateNull; 
                 subSmIndex++) {

                if ((subSmEntries[subSmIndex].SubSmFlags.SubSmFlagsAsUlong & 
                     SmContext->SubSmFlags) == SmContext->SubSmFlags) {

                    //
                    //  Found a matching sub state machine
                    //
                    matchingSubSmStartState = subSmEntries[subSmIndex].SubSmStartState;

                    break;
                }
            }                                                               

            //
            // If this state has sub state entries but none of them are 
            // applicable to this state, then it we need to queue the special
            // event GenericEventContinueIfNoSubState
            //                                                              
            if (matchingSubSmStartState.StateAsUlong == GenericStateNull) {
                syncEvent.EventAsUlong =  GenericEventContinueIfNoSubState;
                break;
            }

            //
            // We found a sub state machine that is applicable, "push" that
            // sub state machine
            //                                                              
            depth++;

            SmContext->CurrentState[depth] =  matchingSubSmStartState;
            SmContext->CurrentStateDepth = depth;

            //
            // Record this transition. The fact that we are pushing a nullEvent
            // will indicate to the debugger extension that we either pushed 
            // or popped a sub state machine. The fact that the depth is one
            // greater than the last depth will indicate that it is a Push.
            //                                                              
            USBPORTRTPM_RecordState(SmContext , 
                                    matchingSubSmStartState, 
                                    depth, 
                                    nullEvent);                                   
            
            currentStateEntry = stateTable[matchingSubSmStartState.StateAsUlong - startIndex];

            //
            // If this state entry function reuqires passive and we are not at
            // passive, then we need to quit and queue the state machine worker
            // item, which will continue the work.
            //
            if ((currentStateEntry->StateFlags & StateFlagRequiresPassive) &&
                (KeGetCurrentIrql() != PASSIVE_LEVEL)) {

                USBPORT_QueueWorkItem(SmContext->FdoDeviceObject,
                                      SmContext->SmWorker,
                                      USBPORTRTPM_EvtSmWorkItem,
                                      DelayedWorkQueue,
                                      SmContext,
                                      SmContext->NeedsForwardProgress);

                //
                // Return a special event, that will indicate to the caller
                // that a worker item has been queued, so the caller should
                // also leave the state machine engine.
                //                

                syncEvent.EventAsUlong = GenericEventQuitForWorkerItem;

                LEAVE;
            }   

            syncEvent.EventAsUlong = currentStateEntry->EntryFunc(SmContext);      

            subSmEntries = currentStateEntry->SubSmEntries;
        }                                                  

        //
        // If the current state is the final state of a sub state machine,
        // then it needs to be popped. This popping can happen for multiple
        // levels.Also as we leave a particular state, then we need to flush 
        // any stale events that are ignored by that state. Note that if the 
        // target state does not have any sub state machine, even then we 
        // want to flush the ignored events and hence the loop atleast runs 
        // once.
        //

        //
        // Since we are going to be touching event queues, we need to acquire
        // state machine lock
        //                                                 
        KeAcquireSpinLock(&SmContext->StateMachineLock,
                          &irql);

        WHILE (TRUE) {

            //
            // Flush Ignored events from all queues
            //

            transitions = currentStateEntry->Transitions;
            
            transIndex = 0;

            while ((transitions[transIndex].Event.EventAsUlong != GenericEventNull) &&
                   (transitions[transIndex].TargetState.StateAsUlong == GenericStateIgnored)) {

                    USBPORTRTPM_PurgeEvent(SmContext,
                                           &SmContext->EventQueue,
                                           transitions[transIndex].Event,
                                           &irql);
                    transIndex++;
            }

            //
            // Check if this state is marked as final state - If yes then 
            // "pop" the sub state machine
            //  
            if (currentStateEntry->StateFlags & StateFlagEndSubStateMachine) {

                depth--;

                //
                // Record the "Pop" operation. The fact that we are storing 
                // NullEvent and that the depth is one less than the last
                // depth, will tell the debugger extension that this is a
                // Pop operation. We store the state that was before the 
                // Pop, so that it is possible to build the complete state
                // transitions starting from the end and going backwards.
                //                                                            
                USBPORTRTPM_RecordState(SmContext , 
                                        SmContext->CurrentState[depth], 
                                        depth, 
                                        nullEvent);                                 
                
                tableIndex = SmContext->CurrentState[depth].StateAsUlong 
                             - startIndex;
                currentStateEntry = stateTable[tableIndex];

            } else {

                break;
            }

        }  

        KeReleaseSpinLock(&SmContext->StateMachineLock,
                          irql);

        //
        // Because of pop of state machines, the depth have changed. 
        // Save it in the SmContext for future use.
        // 
        SmContext->CurrentStateDepth = depth;    

    } FINALLY {

    }

    return syncEvent;

} //USBPORTRTPM_ExecuteEntryFunctionsAndPushPopStateMachinesForCurrentState

VOID 
USBPORTRTPM_EvtSmWorkItem(
        PDEVICE_OBJECT                    DeviceObject,
        PSM_CONTEXT                       SmContext,
        PIO_WORKITEM                      WorkItem
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
    EVENT                       syncEvent;
    KIRQL                       irql;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(WorkItem);

    TRY {


        NT_ASSERTMSG("State Machine Worker Item entered when State Machine not running",
                     SmContext->StateMachineRunning == TRUE);
        
        //
        // Complete the current transition that was in progress and was
        // disrupted because we needed to go to passive level
        //
        syncEvent = USBPORTRTPM_ExecuteEntryFunctionsAndPushPopStateMachinesForCurrentState(SmContext);

        NT_ASSERTMSG("A worker item was started when one was already running",
                     syncEvent.EventAsUlong != GenericEventQuitForWorkerItem); 
        
        //
        // Now just continue the state machine engine
        //
        KeAcquireSpinLock(&SmContext->StateMachineLock,
                          &irql);
        
        USBPORTRTPM_RunStateMachine(SmContext,
                                    syncEvent,
                                    &irql);

        KeReleaseSpinLock(&SmContext->StateMachineLock,
                          irql);

    } FINALLY {

    }

    return;                               

}  //USBPORTRTPM_EvtSmWorkItem                  


__drv_maxIRQL(DISPATCH_LEVEL)
__drv_neverHold(StateMachineLock)
VOID
USBPORTRTPM_AddEvent(
    __inout
        PSM_CONTEXT         SmContext,
    __in    
        RTPM_EVENT          RtpmEvent
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

    EVENT               nullEvent;      
    KIRQL               irql;
    EVENT               event;

    TRY {                                 

        event.RtpmEvent = RtpmEvent;
        KeAcquireSpinLock(&SmContext->StateMachineLock,
                          &irql);
        //
        // Log this event in the history
        //
        USBPORTRTPM_RecordEvent(SmContext, 
                                event);         
        
        //
        // Enqueue this event in the queue of the correct priority
        //               
        USBPORTRTPM_EnqueueEvent(&SmContext->EventQueue,
                                 event);    
        //
        // Start the state machine, if not alredy running
        // 
        if (FALSE == SmContext->StateMachineRunning) {
            
            SmContext->StateMachineRunning = TRUE;


            nullEvent.EventAsUlong = GenericEventNull;
            
            USBPORTRTPM_RunStateMachine(SmContext,
                                        nullEvent,
                                        &irql);
        }             

    } FINALLY {

        KeReleaseSpinLock(&SmContext->StateMachineLock,
                          irql);
    }

    return;

} // USBPORTRTPM_AddEvent                          


__drv_maxIRQL(DISPATCH_LEVEL)
__drv_mustHold(StateMachineLock)
VOID
USBPORTRTPM_RunStateMachine(
    __inout 
        PSM_CONTEXT         SmContext,
    __in
        EVENT               SyncEvent,
    __inout
        PKIRQL              Irql
    )
/*++

Routine Description:

    This routine is the generic state machine engine
    for the runtime power managment state machine. It 
    keeps dequeuing events from the event queues and 
    handles them, based on their priority.
 
Arguments:                                   
    
    SmContext - Context for the state machine 
    
    SyncEvent - Event returned by the last state transition
                If this is not NULL, then this will be
                handled before looking at any event queues
                The only case where it can be not NULl is 
                where the state machine engine is resuming
                its run from a worker thread 
    
    Irql     -  Pointer to Irql at which the state machine 
                Lock was acquired. This function might release
                and re-acquire the lock. So this parameter 
                also acts as out parameter. Even though the 
                value passed out should always be same as 
                value passed in
Returns:

    VOID

--*/                                
{

    PSTATE_ENTRY                currentStateEntry;
    EVENT                       nextEvent;
    EVENT                       syncEvent;
    ULONG                       tableIndex;
    PSTATE_ENTRY *              stateTable;
    ULONG                       startIndex;
    BOOLEAN                     stateChanged;
    BOOLEAN                     lockAcquired;

    TRY {                                    

        stateTable = SmContext->StateTable;
        startIndex = SmContext->StartIndex;  

        syncEvent = SyncEvent;               
        lockAcquired = TRUE;

        WHILE (TRUE) {

            //
            // If there is a sync Event, then handle it first
            // If there is no sync Event, then get the event
            // from the queues
            //
            if (syncEvent.EventAsUlong == GenericEventNull) {
                
                nextEvent = USBPORTRTPM_GetNextEvent(SmContext);
                
                //
                // If there are no events left in the queue that
                // needs to be handled by the current state, then
                // we should quit
                //
                if (nextEvent.EventAsUlong == GenericEventNull) {

                    SmContext->StateMachineRunning = FALSE;

                    LEAVE;
                }                

            } else {
                
                nextEvent = syncEvent;
                syncEvent.EventAsUlong = GenericEventNull;

            }

            
            //
            // To transition into the new state, we might have to
            // call into external world. So we must release the 
            // lock now. State machine lock guards the event queues and 
            // the StateMachineRunning bit , so we need to re-acquire the
            // lock when we are about to touch them again.
            //
            KeReleaseSpinLock(&SmContext->StateMachineLock,
                              *Irql);
            lockAcquired = FALSE;
            
            stateChanged = USBPORTRTPM_FindAndSetTargetState(SmContext,
                                                             nextEvent);

            if (stateChanged) {
                
                
                //
                // We transitioned to a new state. We will complete that
                // transition by executing the state entry functions
                //

                tableIndex = SmContext->CurrentState[SmContext->CurrentStateDepth].StateAsUlong 
                             - startIndex;

                currentStateEntry = stateTable[tableIndex];

                //
                // If this state entry function reuqires passive and we are not at
                // passive, then we need to quit and queue the state machine worker
                // item, which will continue the work.
                //
                if ((currentStateEntry->StateFlags & StateFlagRequiresPassive) &&
                    (KeGetCurrentIrql() != PASSIVE_LEVEL)) {

                    USBPORT_QueueWorkItem(SmContext->FdoDeviceObject,
                                          SmContext->SmWorker,
                                          USBPORTRTPM_EvtSmWorkItem,
                                          DelayedWorkQueue,
                                          SmContext,
                                          SmContext->NeedsForwardProgress);
                    
                    LEAVE;
                }

                syncEvent = USBPORTRTPM_ExecuteEntryFunctionsAndPushPopStateMachinesForCurrentState(SmContext);

                if (syncEvent.EventAsUlong == GenericEventQuitForWorkerItem) {
                    
                    //
                    // One of the sub state entry functions required PASSIVE and a 
                    // worker item has been queued for that purpose. So we need
                    // to quit
                    //
                    LEAVE;                                        
                }                                                 
            }                      

            //
            // Re-acquire the lock because we are going to touch the event queues
            // again.
            //
            KeAcquireSpinLock(&SmContext->StateMachineLock,
                              Irql);
            lockAcquired = TRUE;
        }   

    } FINALLY {

        if (lockAcquired == FALSE) {

            KeAcquireSpinLock(&SmContext->StateMachineLock,
                              Irql);
        }
    }

    return;

} // USBPORTRTPM_RunStateMachine


__drv_maxIRQL(DISPATCH_LEVEL)
__drv_mustHold(StateMachineLock)
FORCEINLINE
VOID
USBPORTRTPM_EnqueueEvent(
    __in_ecount(MAX_QUEUE_DEPTH)    
        PEVENT_QUEUE        Queue,
    __in    
        EVENT               Event
    )       
/*++

Routine Description:

    This routine adds an event to the tail of the queue

Arguments:                
    
    Queue - Event queue to which the event should be added    
    Event - the event to be added      

Returns:

    VOID                  
--*/ 
{  
    Queue->Events[Queue->QueueTail] = Event;

    Queue->QueueTail = (Queue->QueueTail + 1) & QUEUE_MASK;

    NT_ASSERTMSG("Queue is full",
                 Queue->QueueTail != Queue->QueueHead);

} //USBPORTRTPM_EnqueueEvent    



__drv_maxIRQL(DISPATCH_LEVEL)
__drv_mustHold(StateMachineLock)
_Must_inspect_result_
FORCEINLINE
EVENT
USBPORTRTPM_DequeueEvent(
    __in_ecount(MAX_QUEUE_DEPTH)    
        PEVENT_QUEUE    Queue,
    __in
        ULONG           StateDispatchFlags
    )       
/*++

Routine Description:

    This routine removes an event from the head of the queue

Arguments:                                                 
    
    Queue - Event queue from which event should be removed  

Returns:

    HsmEvent - Event removed

--*/
{
    EVENT           dequeuedEvent;
    UCHAR           i, j;
    EVENT_PRIORITY  eventPri;

    dequeuedEvent.EventAsUlong = GenericEventNull;

    TRY {
        for (i = Queue->QueueHead; i != Queue->QueueTail; i = QUEUE_MASK & (i + 1)) {

            eventPri = EVENT_PRIORITY(Queue->Events[i]);

            //
            // Critical Events are always picked up. Normal Pri
            // events are picked up as long as state does not 
            // declare HandlesCriticalEventsOnly. Low Pri events
            // are handled only if state declares HandlesLowPriEvents
            //
            if ((eventPri == EventPriorityCritical) ||
                ((eventPri == EventPriorityNormal) && 
                 ((StateDispatchFlags & StateFlagHandlesCriticalEventsOnly) == 0)) ||
                ((eventPri == EventPriorityLow) && 
                 (StateDispatchFlags & StateFlagHandlesLowPriEvents))) {

                break;
            }   
        }  

        if (i == Queue->QueueTail) {
            LEAVE;
        }

        dequeuedEvent.EventAsUlong = Queue->Events[i].EventAsUlong;
        
        for (j = i; j != Queue->QueueHead; j = QUEUE_MASK & (j - 1)) {
            Queue->Events[j] = Queue->Events[QUEUE_MASK & (j - 1)];
        }   

        Queue->Events[Queue->QueueHead].EventAsUlong = GenericEventNull;

        Queue->QueueHead = (Queue->QueueHead + 1) & QUEUE_MASK;
        
    } FINALLY {
    }
    return dequeuedEvent;

} //USBPORTRTPM_DequeueEvent


__drv_maxIRQL(DISPATCH_LEVEL)
__drv_mustHold(StateMachineLock)
FORCEINLINE
VOID
USBPORTRTPM_RecordEvent(
    __in
        PSM_CONTEXT         SmContext,
    __in    
        EVENT               Event
    ) 
/*++

Routine Description:

    This routine records an event in the log for debugging

Arguments:
    
    SmContext 
    Event - Event to be logged  

Returns:

    VOID

--*/
{
    TRY {

        SmContext->EventHistory[SmContext->EventHistoryIndex] = Event;
        SmContext->EventHistoryIndex = (SmContext->EventHistoryIndex + 1) & EVENT_HISTORY_MASK;

    } FINALLY {

    }

    return;

} //USBPORTRTPM_RecordEvent
                                                                                               

__drv_maxIRQL(DISPATCH_LEVEL)
__drv_mustHold(StateMachineLock)
FORCEINLINE
VOID
USBPORTRTPM_RecordState(
    __in      
        PSM_CONTEXT         SmContext,
    __in      
        STATE               State,
    __in      
        ULONG               Depth,    
    __in      
        EVENT               Event
    )       
/*++

Routine Description:

    This routine records a state transition in the log for debugging

Arguments:                                                                                     
    
    SmContext 
    State - The new state being entered
    Depth - The current state depth
    Event - The event that resulted in this transition  

Returns:

    VOID

--*/ 
{
    TRY {
        //
        // Note that the debugger extension will be able to rebuilt the 
        // full state transitions by using the current state snapshot and 
        // walking backwards using this log
        //

        SmContext->StateHistory[SmContext->StateHistoryIndex].State = State;
        SmContext->StateHistory[SmContext->StateHistoryIndex].Depth = Depth;
        SmContext->StateHistory[SmContext->StateHistoryIndex].Event = Event;
        SmContext->StateHistoryIndex = (SmContext->StateHistoryIndex + 1) & STATE_HISTORY_MASK;


        /*EventWriteUSBHUB3_ETW_EVENT_STATE_MACHINE(&SmContext->CurrentActivityId,
                                                  SmContext->ParentHandle,
                                                  SmContext->ParentContext,
                                                  SmContext->StartIndex,
                                                  State.StateAsUlong,
                                                  Event.EventAsUlong,
                                                  Depth);*/


    } FINALLY {

    }

    return;

}//USBPORTRTPM_RecordState                                                                           


__drv_maxIRQL(DISPATCH_LEVEL)
__drv_mustHold(StateMachineLock)
FORCEINLINE
VOID
USBPORTRTPM_PurgeEvent(
    __inout 
        PSM_CONTEXT         SmContext,
    __in_ecount(MAX_QUEUE_DEPTH)    
        PEVENT_QUEUE        Queue,
    __in    
        EVENT               Event,
    __inout
        PKIRQL              Irql     
    )       
/*++

Routine Description:

    This routine looks for the given event in the queue and
    deleted all its instances from the queue

Arguments:

    SmContext - Context for the state machine
    Queue - The queue from which the event will be purged
    Event - The event that needs to be purged

Returns:

    VOID

--*/
{
    UCHAR       i;
    UCHAR       newTail;  
    BOOLEAN     eventPurged;

    TRY {
        
        eventPurged = FALSE;
        newTail = Queue->QueueHead;

        for (i = Queue->QueueHead; i != Queue->QueueTail; i = QUEUE_MASK & (i + 1)) {

            //
            // In the new queue,we will only copy events that
            // don't match the passed Event
            //
            if (Queue->Events[i].EventAsUlong != Event.EventAsUlong) {
                
                Queue->Events[newTail] = Queue->Events[i];
                
                newTail = ( newTail + 1) & QUEUE_MASK; 
            } else {

                //
                // We don't know of any event till now
                // that has two instances of the queued
                // at the same time. So instead of breaking
                // out of the loop, we put an assert here
                // so as to catch any bugs in this logic
                //
                NT_ASSERT(eventPurged == FALSE);
                eventPurged = TRUE;

            }
        }  
        
        //
        // For the sake of debugging, let us explicitly NULL the
        // now unused places
        //
        for (i = newTail; i != Queue->QueueTail; i = QUEUE_MASK & (i + 1)) {

            Queue->Events[ i & QUEUE_MASK].EventAsUlong = GenericEventNull;
        }  
        
        Queue->QueueTail = newTail;


    } FINALLY {

    }      

    return;

} //USBPORTRTPM_PurgeEvent                                                                           


NTSTATUS
USBPORTRTPM_InitializeRtpmSm(
    __in
        PDEVICE_OBJECT  FdoDeviceObject
    )
{
    PSM_CONTEXT         smContext;
    PDEVICE_EXTENSION   devExt;


    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);
    smContext = &devExt->Fdo.RuntimePMContext.RtpmSmContext;

    RtlZeroMemory(smContext,
                  sizeof(*smContext));

    KeInitializeSpinLock(&smContext->StateMachineLock);

    smContext->CurrentStateDepth = 0;
    smContext->SubSmFlags = 0;
    smContext->FdoDeviceObject = FdoDeviceObject;
    smContext->CurrentState[0].RtpmState = StartIndexRtpm;

    smContext->StateTable = RTPMStateTable;
    smContext->StartIndex = StartIndexRtpm;
    

    smContext->SmWorker = USBPORT_AllocateWorkItem(FdoDeviceObject,
                                                   TRUE);

    if (!smContext->SmWorker) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    return STATUS_SUCCESS;
}

VOID
USBPORTRTPM_FreeRtpmSm(
    __in
        PDEVICE_OBJECT  FdoDeviceObject
    )
{

    PDEVICE_EXTENSION   devExt;
    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    if (devExt->Fdo.RuntimePMContext.RtpmSmContext.SmWorker != NULL) {
        USBPORT_FreeWorkItem(devExt->Fdo.RuntimePMContext.RtpmSmContext.SmWorker);
        devExt->Fdo.RuntimePMContext.RtpmSmContext.SmWorker = NULL;
    }

}

