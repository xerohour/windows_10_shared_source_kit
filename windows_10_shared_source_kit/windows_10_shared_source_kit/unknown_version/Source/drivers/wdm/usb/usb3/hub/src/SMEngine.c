
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
#include "hsm_autogen.h"
#include "psm20_autogen.h"
#include "psm30_autogen.h"
#include "dsm_autogen.h"
#include "smengine.tmh"


const CHAR StateMachineTag[]        = "State Machine Tag";
extern CHAR UserModeFdoRequest[];


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBSM_AddHsmEvent(
    __inout
        PHUB_FDO_CONTEXT    HubContext,
    __in
        HSM_EVENT           Event
    )
/*++

Routine Description:

    This routine is the entry point for hub state machine.
    This function converts parameters into generic types
    and then calls into the generic state machine engine.

Arguments:

    HubContext - WDF context of the hub fdo

    Event - the event to be added

Returns:

    VOID

--*/
{

    EVENT    event;

    event.HsmEvent = Event;

    HUBSM_AddEvent(&HubContext->HsmContext,
                   event);

} //HUBSM_AddHsmEvent


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBSM_AddPsm20Event(
    __inout
        PPORT_CONTEXT       PortContext,
    __in
        PSM_EVENT           Event
    )
/*++

Routine Description:

    This routine is the entry point for port state machine.
    This function converts parameters into generic types
    and then calls into the generic state machine engine.

Arguments:

    PortContext - WDF context of the port object

    Event - the event to be added

Returns:

    VOID

--*/
{


    EVENT    event;

    event.PsmEvent = Event;

    HUBSM_AddEvent(&PortContext->PsmContext,
                   event);

} //HUBSM_AddPsm20Event


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBSM_AddDsmEvent(
    __inout
        PDEVICE_CONTEXT     DeviceContext,
    __in
        DSM_EVENT           Event
    )
/*++

Routine Description:

    This routine is the entry point for device state machine.
    This function converts parameters into generic types
    and then calls into the generic state machine engine.

Arguments:

    DeviceContext - WDF context of the device object

    Event - the event to be added

Returns:

    VOID

--*/
{
    EVENT    event;

    event.DsmEvent = Event;

    HUBSM_AddEvent(&DeviceContext->DsmContext,
                   event);

} //HUBSM_AddDsmEvent


__drv_maxIRQL(DISPATCH_LEVEL)
_Requires_lock_held_(SmContext->StateMachineLock)
EVENT
HUBSM_GetNextEvent(
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

        event = HUBSM_DequeueEvent(&SmContext->EventQueue,
                                   stateDispatchFlags);


    } FINALLY {

    }

    return event;

} //HUBSM_GetNextEvent


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBSM_LogUnhandledEvent(
    __inout
        PSM_CONTEXT         SmContext,
    __in
        EVENT               Event
    )
/*++

Routine Description:


Arguments:

    SmContext - Context for the state machine

    Event - Event that was not handled

Returns:


--*/
{

    PHUB_GLOBALS         hubGlobals;

    TRY {

        hubGlobals = GetHubGlobals(WdfGetDriver());

        switch (SmContext->StartIndex) {
        case StartIndexDsm:
            DbgTraceTo(hubGlobals->IfrLog,
                       TL_INFO,
                       Driver,
                       "DSM 0x%p: is not handling Event %!DSMEVENT!, Current Top Level State is %!DSMSTATE!",
                       SmContext->ParentContext,
                       Event.EventAsUlong,
                       SmContext->CurrentState[0].StateAsUlong);
            break;
        case StartIndexHsm:
            DbgTraceTo(hubGlobals->IfrLog,
                       TL_INFO,
                       Driver,
                       "HSM 0x%p: is not handling Event %!HSMEVENT!, Current Top Level State is %!HSMSTATE!",
                       SmContext->ParentContext,
                       Event.EventAsUlong,
                       SmContext->CurrentState[0].StateAsUlong);
            break;
        case StartIndexPsm20:
            DbgTraceTo(hubGlobals->IfrLog,
                       TL_INFO,
                       Driver,
                       "PSM20 0x%p: is not handling Event %!PSMEVENT!, Current Top Level State is %!PSM20STATE!",
                       SmContext->ParentContext,
                       Event.EventAsUlong,
                       SmContext->CurrentState[0].StateAsUlong);
            break;
        case StartIndexPsm30:
            DbgTraceTo(hubGlobals->IfrLog,
                       TL_INFO,
                       Driver,
                       "PSM30 0x%p: is not handling Event %!PSMEVENT!, Current Top Level State is %!PSM30STATE!",
                       SmContext->ParentContext,
                       Event.EventAsUlong,
                       SmContext->CurrentState[0].StateAsUlong);
            break;
        default:
            break;
        }

        //
        // Break in the fre build as well
        //
        HUBMISC_DbgBreak("Unhandled Event");

        EventWriteUSBHUB3_ETW_EVENT_UNHANDLED_STATE_MACHINE_EVENT(&SmContext->CurrentActivityId,
                                                                  SmContext->ParentHandle,
                                                                  SmContext->ParentContext,
                                                                  SmContext->StartIndex,
                                                                  SmContext->StateTable[SmContext->CurrentState[0].StateAsUlong - SmContext->StartIndex]->EtwName,
                                                                  Event.EventAsUlong,
                                                                  0);
    } FINALLY {

    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBSM_FindAndSetTargetState(
    __inout
        PSM_CONTEXT         SmContext,
    __in
        EVENT               Event,
    _Out_
        PBOOLEAN            PreviousStateNeedsStopTimerOnExit
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
    PPORT_CONTEXT               portContext;
    PDEVICE_CONTEXT             deviceContext;
    PHUB_FDO_CONTEXT            hubFdoContext;
    PIRP                        irp;
    PIO_STACK_LOCATION          nextSp;


    TRY {

        nullEvent.EventAsUlong = GenericEventNull;
        *PreviousStateNeedsStopTimerOnExit = FALSE;

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
            HUBSM_LogUnhandledEvent(SmContext,
                                    Event);


            stateChanged = FALSE;

            break;

        case DsmStateCompleteWithStatusFailed:

            deviceContext = (PDEVICE_CONTEXT)(SmContext->ParentContext);

            WdfRequestComplete(deviceContext->CurrentClientRequest,
                               STATUS_UNSUCCESSFUL);

            if (deviceContext->DeviceStateFlags.ActivityIdSetForDsmRequests == 1) {

                RtlZeroMemory(&SmContext->CurrentActivityId, sizeof(GUID));
                INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &deviceContext->DeviceStateFlags, ActivityIdSetForDsmRequests);
            }
            stateChanged = FALSE;

            break;

        case DsmStateRequestPortCycle:

            portContext = ((PDEVICE_CONTEXT)(SmContext->ParentContext))->PortContext;

            portContext->PsmContext.AddEvent(portContext,
                                             PsmEventDeviceRequestCycle);

            stateChanged = FALSE;

            break;

        case DsmStateSignalQueryDeviceTextEvent:

            deviceContext = (PDEVICE_CONTEXT)(SmContext->ParentContext);
            KeSetEvent(&deviceContext->QueryDeviceTextEvent,
                       IO_NO_INCREMENT,
                       FALSE);

            stateChanged = FALSE;

            break;

        case DsmStateSignalPnpPowerEvent:

            deviceContext = (PDEVICE_CONTEXT)(SmContext->ParentContext);
            KeSetEvent(&deviceContext->PnPPowerEvent,
                       IO_NO_INCREMENT,
                       FALSE);

            stateChanged = FALSE;

            break;

        case DsmStateCompletePdoPreStart:

            deviceContext = (PDEVICE_CONTEXT)(SmContext->ParentContext);
            KeSetEvent(&deviceContext->PreStartEvent,
                       IO_NO_INCREMENT,
                       FALSE);

            stateChanged = FALSE;

            break;

        case DsmStateCompleteFDORequestWithStatusFailed:

            deviceContext = (PDEVICE_CONTEXT)(SmContext->ParentContext);

            // We are not going to send the request to the controller stack.
            // So we should zero out the stack location that was prepared
            // and also signal the event for the caller to know.
            //
            irp = WdfRequestWdmGetIrp(deviceContext->PreformattedGetDescriptorRequest);
            nextSp = IoGetNextIrpStackLocation(irp);

            RtlZeroMemory(nextSp,
                          sizeof(IO_STACK_LOCATION));

            irp->IoStatus.Status = STATUS_UNSUCCESSFUL;

            HUBFDO_CompleteGetDescriptorRequest(deviceContext->HubFdoContext,
                                                deviceContext->PortContext->PortInformation.PortNumber,
                                                deviceContext->PreformattedGetDescriptorRequest,
                                                STATUS_UNSUCCESSFUL,
                                                0);
            //
            // Release the WDF reference that was acquired on behalf
            // of the user mode request from the Ioctl handler
            //
            WdfObjectDereferenceWithTag(WdfObjectContextGetObject(deviceContext),
                                        (PVOID) UserModeFdoRequest);
            stateChanged = FALSE;

            break;

        case HsmStateCompleteGetPortStatusWithNoSuchDevice:

            hubFdoContext = (PHUB_FDO_CONTEXT)(SmContext->ParentContext);

            HUBHTX_CompleteGetPortStatusWithFailedStatus(hubFdoContext,
                                                         STATUS_NO_SUCH_DEVICE);

            stateChanged = FALSE;

            break;

        case HsmStateCompleteGetPortStatusWithInternalError:

            hubFdoContext = (PHUB_FDO_CONTEXT)(SmContext->ParentContext);

            HUBHTX_CompleteGetPortStatusWithFailedStatus(hubFdoContext,
                                                         STATUS_ADAPTER_HARDWARE_ERROR);

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

                HUBSM_RecordState(SmContext ,
                                  SmContext->CurrentState[i - 1],
                                  i - 1,
                                  nullEvent);


                NT_ASSERTMSG("Parent state moved but the child state doesn't allow it",
                             stateTable[tableIndex]->StateFlags &
                             StateFlagPopIfParentStateChanges);
            }
        }

        //
        // Before making the state transition, make a note of the
        // StopTimerOnExit property of the previous state
        //
        tableIndex = SmContext->CurrentState[depth].StateAsUlong - startIndex;
        //stateEntry = stateTable[tableIndex];

        if (stateTable[tableIndex]->StateFlags & StateFlagStopTimerOnExit) {
            *PreviousStateNeedsStopTimerOnExit = TRUE;
        }

        //
        // Make the state transition
        //
        SmContext->CurrentState[depth].StateAsUlong = nextState.StateAsUlong;
        SmContext->CurrentStateDepth = depth;


        //
        // Record ths state transition
        //
        HUBSM_RecordState(SmContext ,
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

            HUBMISC_DbgBreak("Break On State Transition");
        }

    } FINALLY {

    }

    return stateChanged;

} //HUBSM_FindAndSetTargetState


__drv_maxIRQL(DISPATCH_LEVEL)
EVENT
HUBSM_ExecuteEntryFunctionsAndPushPopStateMachinesForCurrentState(
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
            // the hub satisifes the sub state machine criterion
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
            // applicable to this hub, then it we need to queue the special
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
            HUBSM_RecordState(SmContext ,
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

                HUBMISC_EnqueueWorkerItem(SmContext->HubFdoContext,
                                          SmContext->SmWorker,
                                          HUBSM_EvtSmWorkItem,
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

                    HUBSM_PurgeEvent(SmContext,
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
                HUBSM_RecordState(SmContext ,
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

} //HUBSM_ExecuteEntryFunctionsAndPushPopStateMachinesForCurrentState

// _Requires_lock_not_held_(SmContext->StateMachineLock)
VOID
HUBSM_EvtSmWorkItem(
        PDEVICE_OBJECT                    DeviceObject,
        PSM_CONTEXT                       SmContext,
        PUCX_FORWARD_PROGRESS_WORKITEM    WorkItem
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

        _Benign_race_begin_
        NT_ASSERTMSG("State Machine Worker Item entered when State Machine not running",
                     SmContext->StateMachineRunning == TRUE);
        _Benign_race_end_

        //
        // Complete the current transition that was in progress and was
        // disrupted because we needed to go to passive level
        //
        syncEvent = HUBSM_ExecuteEntryFunctionsAndPushPopStateMachinesForCurrentState(SmContext);

        NT_ASSERTMSG("A worker item was started when one was already running",
                     syncEvent.EventAsUlong != GenericEventQuitForWorkerItem);

        //
        // Now just continue the state machine engine
        //
        KeAcquireSpinLock(&SmContext->StateMachineLock,
                          &irql);

        HUBSM_RunStateMachine(SmContext,
                              syncEvent,
                              &irql,
                              TRUE);

        KeReleaseSpinLock(&SmContext->StateMachineLock,
                          irql);

    } FINALLY {

    }

    return;

}  //HUBSM_EvtSmWorkItem

_IRQL_requires_max_(DISPATCH_LEVEL)
_Requires_lock_not_held_(SmContext->StateMachineLock)
VOID
HUBSM_AddEvent(
    __inout
        PSM_CONTEXT         SmContext,
    __in
        EVENT               Event
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

    KIRQL               irql;
    EVENT               syncEvent;


    TRY {

        KeAcquireSpinLock(&SmContext->StateMachineLock,
                          &irql);
        //
        // Log this event in the history
        //
        HUBSM_RecordEvent(SmContext,
                          Event);
        //
        // Special processing for LPM timeout events
        // Before queueing a new one, purge any old ones
        // as the new one overrides them
        //
        if ((Event.EventAsUlong == PsmEventDeviceSetU1Timeout) ||
            (Event.EventAsUlong == PsmEventDeviceSetU2Timeout) ||
            (Event.EventAsUlong == DsmEventLPMPowerSettingChange) ||
            (Event.EventAsUlong == DsmEventNoPingResponseError)) {
            HUBSM_PurgeEvent(SmContext,
                             &SmContext->EventQueue,
                             Event,
                             &irql);
        }

        //
        // There is a common pattern in the state machine where a timer
        // is started in a state and then the state waits for the timer to
        // fire. However, if the state makes any other transition, other
        // than the TimerFired, it needs to stop and flush the timer
        // before proceeding. Since flushing the timer is an async process,
        // Encoding this pattern in the state machine can result in a large
        // number of extra states. To avoid these states, we have added a
        // concept of auto stopping a timer in the state machine itself.
        // For any state that it marked as "StopTimerOnExit", the state machine
        // engine will take care of stopping the timer and waiting for it to
        // flush before allowing any transitions that change the current state
        //
        // If the WaitingForTimerFired is set, it indicates that the state machine
        // tried to stop the timer but failed. So now the state machine is not
        // processing any more events and is only waiting for TimerFired event
        //
        if (SmContext->WaitingForTimerFired != FALSE) {

            NT_ASSERT(SmContext->StateMachineRunning == TRUE);

            if (Event.EventAsUlong == GenericEventTimerFired) {
                SmContext->WaitingForTimerFired = FALSE;
                //
                // Note that when WaitingForTimerFired gets set, the transition is almost
                // complete except for running the state entry functions and pushing the
                // sub states. So we will start from where we left off.
                //
                syncEvent = HUBSM_ExecuteEntryFunctionsAndPushPopStateMachinesForCurrentState(SmContext);
                HUBSM_RunStateMachine(SmContext,
                                      syncEvent,
                                      &irql,
                                      FALSE);
            } else {
                HUBSM_EnqueueEvent(&SmContext->EventQueue,
                                   Event);
            }

            LEAVE;
        }

        //
        // Enqueue this event in the queue of the correct priority
        //
        HUBSM_EnqueueEvent(&SmContext->EventQueue,
                           Event);


        //
        // Start the state machine, if not alredy running
        //
        if (FALSE == SmContext->StateMachineRunning) {

            SmContext->StateMachineRunning = TRUE;

            //
            // The caller of this function is responsible for
            // ensuring that the target object is valid (i.e.
            // the caller holds a reference on the object) when
            // the call is being made. But as part of the state
            // machine running, it is possible that the caller
            // reference goes down. After that point, the state
            // machine engine still need to access the object for
            // a short duration. So this reference is the state
            // machine reference that will be released when the
            // the state machine stopps running.
            //
            WdfObjectReferenceWithTag(WdfObjectContextGetObject(SmContext->ParentContext),
                                      (PVOID) StateMachineTag);

            syncEvent.EventAsUlong = GenericEventNull;

            HUBSM_RunStateMachine(SmContext,
                                  syncEvent,
                                  &irql,
                                  FALSE);
        }

    } FINALLY {

        KeReleaseSpinLock(&SmContext->StateMachineLock,
                          irql);
    }

    return;

} // HUBSM_AddEvent


__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
HUBSM_ExecuteDequeueAction(
    __inout
        PSM_CONTEXT         SmContext,
    __in
        EVENT               Event
    )
/*++

Routine Description:

    This routine executes any special processing that needs to
    be done when a particular event is dequeued.

Arguments:

    SmContext - Context for the state machine
    Event - the event being dequeued

Returns:

    VOID
--*/
{
    PPORT_CONTEXT               portContext;
    PDEVICE_CONTEXT             deviceContext;
    NTSTATUS                    status;
    KIRQL                       currentIrql;


    switch(Event.EventAsUlong){

    case PsmEventDeviceDetached:

        //
        // Special step for a PSM20 event, that helps reduce
        // State explosion. Since the enum values do not overlap,
        // we can safely assume that the statemachine is PSM
        //

        portContext = ((PPORT_CONTEXT)(SmContext->ParentContext));

        if (portContext->PortFlags.ConnectedDeviceSupportsReAttach == 0) {
            //
            // Deleting the device does not mean it will go away.
            // PDO might have a reference or we could infact be running
            // from within the context of worker item of DSM. But KMDF
            // will invoke the Cleanup for DSM. From within that cleanup,
            // we wait for the worker item to finish. Therefore, we don't
            // want KMDF to invoke the Cleanup from this thread , otherwise
            // we will end up deadlocking ourself. So what we do here is
            // to temperorily raise IRQL to dispatch so that KMDF will be
            // forced to use its own thread go to passive (we have marked
            // DSM to require passive for cleanup).
            //
            KeRaiseIrql(DISPATCH_LEVEL,
                        &currentIrql);

            WdfObjectDelete(WdfObjectContextGetObject(portContext->DeviceContext));

            KeLowerIrql(currentIrql);

            portContext->DeviceContext = NULL;

        } else if (portContext->DeviceContext->DeviceStateFlags.DifferentDeviceOnBootDevicePort == 1) {

            DbgTraceTo(portContext->IfrLog,
                       TL_INFO,
                       Device,
                       "Wrong Device on the boot device port is being detached");

            INTERLOCKED_CLEAR_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &portContext->DeviceContext->DeviceStateFlags, DifferentDeviceOnBootDevicePort);

        }
        //
        // It is important that we set this to 0 only
        // after we are completely done with the old
        // device. Setting this to 0, allows a new
        // device to start enumerating
        //
        INTERLOCKED_CLEAR_BIT_FIELD(USB_PORT_FLAGS, &portContext->PortFlags, DeviceConnected);
        //
        // We update the connection status for the benefit of
        // a user mode app querying for it. Overcurrent takes
        // precedence over disconnected
        //
        if (portContext->ConnectionStatus != DeviceCausedOvercurrent) {
            portContext->ConnectionStatus = NoDeviceConnected;
        }

        break;

    case DsmEventClientRequestReset:
    case DsmEventClientRequestResetPipe:
    case DsmEventClientRequestSelectConfiguration:
    case DsmEventClientRequestSelectNullConfiguration:
    case DsmEventClientRequestSetInterface:
    case DsmEventClientRequestSyncClearStall:
    case DsmEventClientRequestSyncResetPipe:

        deviceContext = ((PDEVICE_CONTEXT)(SmContext->ParentContext));
        //
        // For all the client requests events, associate the pre-allocated
        // control transfer and UCX request with the activity ID
        // of the current client request
        //
        status = HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(deviceContext->CurrentClientRequest),
                                          &SmContext->CurrentActivityId);

        if (NT_SUCCESS(status)) {

            HUBMISC_SetActivityIdIrp(deviceContext->ControlRequest.Irp,
                                     &SmContext->CurrentActivityId);
            HUBMISC_SetActivityIdIrp(deviceContext->UcxRequestIrp,
                                     &SmContext->CurrentActivityId);

            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &deviceContext->DeviceStateFlags, ActivityIdSetForDsmRequests);
        }

        break;

    case DsmEventPDOD0Entry:
    case DsmEventPDOD0Exit:
    case DsmEventPDOD0ExitFinal:

        deviceContext = ((PDEVICE_CONTEXT)(SmContext->ParentContext));
        //
        // For all these PDO events, associate the pre-allocated
        // control transfer and UCX request with the activity ID
        // that was allocated in the WDF callbacks
        //
        if (deviceContext->DeviceStateFlags.WdfActivityIdValid == 1) {

            SmContext->CurrentActivityId = deviceContext->WdfActivityId;

            HUBMISC_SetActivityIdIrp(deviceContext->ControlRequest.Irp,
                                     &SmContext->CurrentActivityId);
            HUBMISC_SetActivityIdIrp(deviceContext->UcxRequestIrp,
                                     &SmContext->CurrentActivityId);

            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_STATE_FLAGS, &deviceContext->DeviceStateFlags, ActivityIdSetForDsmRequests);
        }

        break;

    }




} //HUBSM_ExecuteDequeueAction

__drv_maxIRQL(DISPATCH_LEVEL)
_IRQL_requires_same_
_Requires_lock_held_(SmContext->StateMachineLock)
VOID
HUBSM_RunStateMachine(
    _Inout_
        PSM_CONTEXT         SmContext,
    _In_
        EVENT               SyncEvent,
    _Inout_
        PKIRQL              Irql,
    _In_
        BOOLEAN             RunningInWorkerItem
    )
/*++

Routine Description:

    This routine is the generic state machine engine
    for the all the state machines in the hub. It
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
    PPORT_CONTEXT               portContext;
    NTSTATUS                    status;
    BOOLEAN                     previousStateNeedsStopTimerOnExit;
    EVENT                       timerFiredEvent;
#if DBG
    KIRQL                       entryIrql;
#endif
    ULONG                       numberOfAsyncEventsProcessed;

    TRY {
#if DBG
        entryIrql = *Irql;
#endif

        numberOfAsyncEventsProcessed = 0;

        previousStateNeedsStopTimerOnExit = FALSE;

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

                nextEvent = HUBSM_GetNextEvent(SmContext);

                //
                // If there are no events left in the queue that
                // needs to be handled by the current state, then
                // we should quit
                //
                if (nextEvent.EventAsUlong == GenericEventNull) {

                    SmContext->StateMachineRunning = FALSE;
                    //
                    // Release the reference that the state machine acquired
                    // on the target object, when it started running
                    //
                    WdfObjectDereferenceWithTag(WdfObjectContextGetObject(SmContext->ParentContext),
                                                (PVOID) StateMachineTag);

                    LEAVE;
                }

                if (++numberOfAsyncEventsProcessed == MAXULONG) {
                    //
                    // State machine has been running continuosly for
                    // way too long. It must be in some kind of loop.
                    // Break in the debugger if attached. Otherwise
                    // continue. It is ok for the count to wrap
                    // around
                    //
                    HUBMISC_DbgBreak("State machine running for too long");
                }

                //
                // Execute any special actions that needs to be
                // done for event being dequeued. Since that
                // action might involve calling into other
                // external components, let us release the lock
                // for now
                //
                KeReleaseSpinLock(&SmContext->StateMachineLock,
                                  *Irql);

                HUBSM_ExecuteDequeueAction(SmContext,
                                           nextEvent);

                KeAcquireSpinLock(&SmContext->StateMachineLock,
                                  Irql);

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

            stateChanged = HUBSM_FindAndSetTargetState(SmContext,
                                                       nextEvent,
                                                       &previousStateNeedsStopTimerOnExit);

            if (stateChanged) {

                if ((SmContext->StartIndex == StartIndexPsm20) ||
                    (SmContext->StartIndex == StartIndexPsm30)) {

                    portContext = (PPORT_CONTEXT)(SmContext->ParentContext);

                    if (HUBSM_DoesCurrentStateAllowHubSuspend(SmContext)) {
                        //
                        // If we are entering a state that allows hub suspend then
                        // release any power reference that we are holding
                        //
                        if (portContext->MuxContext.PortFlags.WdfPowerReferenceHeld == 1) {
                            WdfDeviceResumeIdle(WdfObjectContextGetObject(portContext->HubFdoContext));
                            INTERLOCKED_CLEAR_BIT_FIELD(PORT_MUX_FLAGS, &portContext->MuxContext.PortFlags, WdfPowerReferenceHeld);
                        }
                    } else {
                        //
                        // If we are entering a state that does not allow hub suspend
                        // then ensure that we acquire a power reference. However, we
                        // do that only of have currently holding a hub power reference.
                        // KMDF complains if we try to acquire a power reference while
                        // not in D0
                        //
                        if ((portContext->HubFdoContext->HubInformation.HubFlags.CanAcquirePowerReference == 1) &&
                            (portContext->MuxContext.PortFlags.WdfPowerReferenceHeld == 0)) {

                            status = WdfDeviceStopIdle(WdfObjectContextGetObject(portContext->HubFdoContext),
                                                       FALSE);
                            if (NT_SUCCESS(status)) {
                                INTERLOCKED_SET_BIT_FIELD(PORT_MUX_FLAGS, &portContext->MuxContext.PortFlags, WdfPowerReferenceHeld);
                            }
                        }
                    }

                }

                //
                // There is a common pattern in the state machine where a timer
                // is started in a state and then the state waits for the timer to
                // fire. However, if the state makes any other transition, other
                // than the TimerFired, it needs to stop and flush the timer
                // before proceeding. Since flushing the timer is an async process,
                // Encoding this pattern in the state machine can result in a large
                // number of extra states. To avoid these states, we have added a
                // concept of auto stopping a timer in the state machine itself.
                // For any state that it marked as "StopTimerOnExit", the state machine
                // engine will take care of stopping the timer and waiting for it to
                // flush before allowing any transitions that change the current state
                //
                if (previousStateNeedsStopTimerOnExit != FALSE) {

                    previousStateNeedsStopTimerOnExit = FALSE;

                    //
                    // If the transition is being made on TimerFired, then we do not need
                    // to anything. Otherwise we try to stop the timer
                    //

                    if ((nextEvent.EventAsUlong != GenericEventTimerFired) &&
                        (ExCancelTimer(SmContext->Timer, NULL) == FALSE)) {

                        //
                        // We failed to stop the timer. That means we now
                        // have to wait for the TimerFired event.
                        //
                        KeAcquireSpinLock(&SmContext->StateMachineLock,
                                          Irql);
                        lockAcquired = TRUE;

                        timerFiredEvent.EventAsUlong = GenericEventTimerFired;
                        if (HUBSM_DequeueSpecificEvent(&SmContext->EventQueue,
                                                       timerFiredEvent)) {
                            //
                            // TimerFired was already in the queue and was
                            // dequeued. So we are done.
                            //

                        } else {
                            //
                            // TimerFired was not found. We will halt the state machine
                            // till we see that event. When an event is being queued after
                            // this point, there will be a check made for the TimerFired
                            // event and only on find that event, we will continue
                            //
                            SmContext->WaitingForTimerFired = TRUE;
                            LEAVE;

                        }
                        KeReleaseSpinLock(&SmContext->StateMachineLock,
                                          *Irql);
                        lockAcquired = FALSE;

                    }
                }
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
                // Also, if we have been running in the context of the caller and 
                // have processed too many async events, we should shift to the
                // worker item so that the thread can unwind and the caller can make
                // progress. One example of where it is important is the following
                // scenario: Due to a buggy hardware, the PSM keeps getting a port 
                // status change. DSM needs to send an event to PSM to recover from
                // the bad hardware. If the PSM happens to be running in the context
                // of DSM, we end up in kind of a live lock because DSM cannot queue
                // the required event till PSM stops running and the thread unwinds
                //
                if (((currentStateEntry->StateFlags & StateFlagRequiresPassive) &&
                     (KeGetCurrentIrql() != PASSIVE_LEVEL)) || 
                    ((RunningInWorkerItem == FALSE) &&
                     (numberOfAsyncEventsProcessed > MAX_ASYNC_EVENTS_IN_ARBITRARY_CONTEXT))) {
                    
                    HUBMISC_EnqueueWorkerItem(SmContext->HubFdoContext,
                                              SmContext->SmWorker,
                                              HUBSM_EvtSmWorkItem,
                                              SmContext,
                                              SmContext->NeedsForwardProgress);

                    LEAVE;
                }



                syncEvent = HUBSM_ExecuteEntryFunctionsAndPushPopStateMachinesForCurrentState(SmContext);

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
    NT_ASSERT(entryIrql == *Irql);
    return;

} // HUBSM_RunStateMachine


__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
HUBSM_EnqueueEvent(
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

} //HUBSM_EnqueueEvent



__drv_maxIRQL(DISPATCH_LEVEL)
_Must_inspect_result_
FORCEINLINE
EVENT
HUBSM_DequeueEvent(
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

} //HUBSM_DequeueEvent

__drv_maxIRQL(DISPATCH_LEVEL)
_Must_inspect_result_
FORCEINLINE
BOOLEAN
HUBSM_DequeueSpecificEvent(
    __in_ecount(MAX_QUEUE_DEPTH)
        PEVENT_QUEUE    Queue,
    __in
        EVENT           Event
    )
/*++

Routine Description:

    This routine attempt to remove the specific event from the
    queue. If the event was found and removed, it returns TRUE

Arguments:

    Queue - Event queue from which event should be removed

Returns:

    BOOLEAN - Whether the event was found and removed

--*/
{
    UCHAR           i, j;
    BOOLEAN         found;

    TRY {
        found = FALSE;

        for (i = Queue->QueueHead; i != Queue->QueueTail; i = QUEUE_MASK & (i + 1)) {

            if (Queue->Events[i].EventAsUlong == Event.EventAsUlong) {
                found = TRUE;
                break;
            }
        }

        if (i == Queue->QueueTail) {
            LEAVE;
        }

        for (j = i; j != Queue->QueueHead; j = QUEUE_MASK & (j - 1)) {
            Queue->Events[j] = Queue->Events[QUEUE_MASK & (j - 1)];
        }

        Queue->Events[Queue->QueueHead].EventAsUlong = GenericEventNull;

        Queue->QueueHead = (Queue->QueueHead + 1) & QUEUE_MASK;

    } FINALLY {
    }
    return found;

} //HUBSM_DequeueSpecificEvent


__drv_maxIRQL(DISPATCH_LEVEL)
_Requires_lock_held_(SmContext->StateMachineLock)
FORCEINLINE
VOID
HUBSM_RecordEvent(
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

} //HUBSM_RecordEvent


__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
HUBSM_RecordState(
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
        // Note that we are using a NULL handle in the following traces.
        // This is by design because we want this special behavior that
        // the traces don't go in the IFR but they still appear as part
        // of WPP traces
        //
        switch (SmContext->StartIndex) {
        case StartIndexDsm:
            DbgTraceTo(NULL,
                       TL_INFO,
                       Driver,
                       "Transition for 0x%p: %!DSMSTATE! --%!DSMEVENT!--> %!DSMSTATE!",
                       SmContext->ParentContext,
                       SmContext->StateHistory[(SmContext->StateHistoryIndex - 1) & STATE_HISTORY_MASK].State.StateAsUlong,
                       Event.EventAsUlong,
                       State.StateAsUlong);
            break;
        case StartIndexHsm:
            DbgTraceTo(NULL,
                       TL_INFO,
                       Driver,
                       "Transition for 0x%p: %!HSMSTATE! --%!HSMEVENT!--> %!HSMSTATE!",
                       SmContext->ParentContext,
                       SmContext->StateHistory[(SmContext->StateHistoryIndex - 1) & STATE_HISTORY_MASK].State.StateAsUlong,
                       Event.EventAsUlong,
                       State.StateAsUlong);
            break;
        case StartIndexPsm20:
            DbgTraceTo(NULL,
                       TL_INFO,
                       Driver,
                       "Transition for 0x%p: %!PSM20STATE! --%!PSMEVENT!--> %!PSM20STATE!",
                       SmContext->ParentContext,
                       SmContext->StateHistory[(SmContext->StateHistoryIndex - 1) & STATE_HISTORY_MASK].State.StateAsUlong,
                       Event.EventAsUlong,
                       State.StateAsUlong);
            break;
        case StartIndexPsm30:
            DbgTraceTo(NULL,
                       TL_INFO,
                       Driver,
                       "Transition for 0x%p: %!PSM30STATE! --%!PSMEVENT!--> %!PSM30STATE!",
                       SmContext->ParentContext,
                       SmContext->StateHistory[(SmContext->StateHistoryIndex - 1) & STATE_HISTORY_MASK].State.StateAsUlong,
                       Event.EventAsUlong,
                       State.StateAsUlong);
            break;
        default:
            NT_ASSERTMSG("Unexpected StartIndex", FALSE);
            break;
        }


        //
        // Note that the debugger extension will be able to rebuilt the
        // full state transitions by using the current state snapshot and
        // walking backwards using this log
        //

        SmContext->StateHistory[SmContext->StateHistoryIndex].State = State;
        SmContext->StateHistory[SmContext->StateHistoryIndex].Depth = Depth;
        SmContext->StateHistory[SmContext->StateHistoryIndex].Event = Event;
        SmContext->StateHistoryIndex = (SmContext->StateHistoryIndex + 1) & STATE_HISTORY_MASK;

        EventWriteUSBHUB3_ETW_EVENT_STATE_MACHINE(&SmContext->CurrentActivityId,
                                                  SmContext->ParentHandle,
                                                  SmContext->ParentContext,
                                                  SmContext->StartIndex,
                                                  SmContext->StateTable[State.StateAsUlong - SmContext->StartIndex]->EtwName,
                                                  Event.EventAsUlong,
                                                  Depth);


    } FINALLY {

    }

    return;

}//HUBSM_RecordState


__drv_maxIRQL(DISPATCH_LEVEL)
_Requires_lock_held_(SmContext->StateMachineLock)
FORCEINLINE
VOID
HUBSM_PurgeEvent(
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

        if (eventPurged) {
            KeReleaseSpinLock(&SmContext->StateMachineLock,
                              *Irql);

            HUBSM_ExecuteDequeueAction(SmContext,
                                       Event);

            KeAcquireSpinLock(&SmContext->StateMachineLock,
                              Irql);

        }

    } FINALLY {

    }

    return;

} //HUBSM_PurgeEvent


__drv_maxIRQL(DISPATCH_LEVEL)
_Must_inspect_result_
FORCEINLINE
BOOLEAN
HUBSM_DoesCurrentStateAllowHubSuspend(
    __in
        PSM_CONTEXT         SmContext
    )
/*++

Routine Description:

    This routine determines if the current
    state allows hub to go to suspend

Arguments:

    SmContext

Returns:

    BOOLEAN - Retruns true if the state allows hub suspend

--*/
{
    PSTATE_ENTRY                *stateTable;
    ULONG                       startIndex;
    PSTATE_ENTRY                stateEntry;
    ULONG                       depth;
    ULONG                       tableIndex;


    startIndex = SmContext->StartIndex;
    stateTable = SmContext->StateTable;
    depth = SmContext->CurrentStateDepth;

    //
    // A state that is marked as PopIfParentStateChanges inherits
    // state properties from its parent. So we need to go up the
    // tree till a state found on which this is not set
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
    // A state is found which has its own flags. Extract
    // AllowsHubSuspend from it
    //
    return (stateEntry->StateFlags & StateFlagAllowsHubSuspend);

} //HUBSM_DoesCurrentStateAllowHubSuspend

