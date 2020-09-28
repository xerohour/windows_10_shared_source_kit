//
//    Copyright (C) Microsoft.  All rights reserved.
//
#define EVENT_HISTORY_DEPTH 16
#define STATE_HISTORY_DEPTH 16
#define EVENT_HISTORY_MASK  0xf 
#define STATE_HISTORY_MASK  0x0f
#define MAX_QUEUE_DEPTH     16
#define QUEUE_MASK          0x0f

typedef enum _SM_ENGINE_STATE {

    SmEngineStateWaitingForPassive = 0x57A70000, // Only used for State Log
    SmEngineStatePaused = 0x57A70001, // Only used for State Log
    SmEngineStateUnexpected = 0x57A70BAD,
    CONTROLLER_RESET_STATE
    UCXENDPOINT_STATE
} SM_ENGINE_STATE, *PSM_ENGINE_STATE;

//
// Event Priorities
//
typedef enum _SM_ENGINE_EVENT_PRIORITY{
    SmEngineEventPriorityLow      = 0,
    SmEngineEventPriorityNormal   = 1,
    SmEngineEventPrioritySync     = 2,
    SmEngineEventPriorityCritical = 3,
    SmEngineEventPrioritySpecial  = 15
} SM_ENGINE_EVENT_PRIORITY;


typedef enum _SM_ENGINE_EVENT {
    //
    // 4 LSB bits define Priority
    // Remaining bits are the Event Data
    //
    SmEngineEventNull=0,
    SmEngineEventOnPassive=0xebe00000 | SmEngineEventPrioritySpecial,  // Special Event
    SmEngineEventNeedsPassive = 0xebe00010, // Only used for State Log
    SmEngineEventPaused = 0xebe00020, // Special Event to inform the state machine that the
    SmEngineEventStart = 0xebe00030, // Only used for State Log
                                       // state entry function has paused the State machine.
    CONTROLLER_RESET_EVENT
    UCXENDPOINT_EVENT
} SM_ENGINE_EVENT;

#define EVENT_PRIORITY(event) ((event) & 0x0000000F)


typedef struct _SM_ENGINE_STATE_LOG {
    SM_ENGINE_EVENT   Event;
    SM_ENGINE_STATE   State;
} SM_ENGINE_STATE_LOG, *PSM_ENGINE_STATE_LOG;

typedef struct _SM_ENGINE_EVENT_QUEUE {
    SM_ENGINE_EVENT     Events[MAX_QUEUE_DEPTH];
    UCHAR               QueueHead;
    UCHAR               QueueTail;
} SM_ENGINE_EVENT_QUEUE, *PSM_ENGINE_EVENT_QUEUE;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
_Must_inspect_result_
SM_ENGINE_EVENT
(SM_ENGINE_STATE_ENTRY_FUNCTION)(
    __in PVOID  StateEntryFunctionContext
    );


typedef SM_ENGINE_STATE_ENTRY_FUNCTION *PFN_SM_ENGINE_STATE_ENTRY_FUNCTION;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
SM_ENGINE_EVENT
(SM_ENGINE_AUTO_EVENT_HANDLER)(
    __in PVOID  StateEntryFunctionContext
    );

typedef SM_ENGINE_AUTO_EVENT_HANDLER *PFN_SM_ENGINE_AUTO_EVENT_HANDLER;

typedef struct _SM_ENGINE_CONTEXT *PSM_ENGINE_CONTEXT;

typedef
VOID
(SM_ENGINE_ETW_WRITE_STATE_TRANSITION)(
    __in_opt
        LPCGUID               ActivityId,
    __in 
        WDFOBJECT             Owner,
    __in
        PSM_ENGINE_CONTEXT    SmEngineContext,
    __in 
        SM_ENGINE_EVENT       Event,
    __in
        SM_ENGINE_STATE       State
    );

typedef SM_ENGINE_ETW_WRITE_STATE_TRANSITION *PFN_SM_ENGINE_ETW_WRITE_STATE_TRANSITION;

typedef
VOID
(SM_ENGINE_ETW_WRITE_STATE_EVENT)(
    __in_opt
        LPCGUID               ActivityId,
    __in 
        WDFOBJECT             Owner,
    __in
        PSM_ENGINE_CONTEXT    SmEngineContext,
    __in 
        SM_ENGINE_EVENT       Event
    );

typedef SM_ENGINE_ETW_WRITE_STATE_EVENT *PFN_SM_ENGINE_ETW_WRITE_STATE_EVENT;

typedef enum _SM_ENGINE_STATE_FLAGS {
	SmEngineStateFlagNone = 0,
	SmEngineStateFlagHandlesAbortUrb = 0x1,
	SmEngineStateFlagHandlesLowPriEvents = 0x2,
	SmEngineStateFlagNoStateEntryFunction = 0x4,
    SmEngineStateFlagRequiresPassive = 0x8,
} SM_ENGINE_STATE_FLAGS;

typedef struct _SM_ENGINE_STATE_TRANSITION {
    SM_ENGINE_EVENT       Event;
    ULONG                 TargetStateIndex;
} SM_ENGINE_STATE_TRANSITION, *PSM_ENGINE_STATE_TRANSITION;

typedef struct _SM_ENGINE_AUTO_EVENT_TRANSITION {
    SM_ENGINE_EVENT                        Event;
    PFN_SM_ENGINE_AUTO_EVENT_HANDLER       AutoEventHandler;
} SM_ENGINE_AUTO_EVENT_TRANSITION, *PSM_ENGINE_AUTO_EVENT_TRANSITION;


typedef struct _SM_ENGINE_STATE_TABLE_ENTRY {
    SM_ENGINE_STATE                      State;
    PFN_SM_ENGINE_STATE_ENTRY_FUNCTION   StateEntryFunction;
    SM_ENGINE_STATE_FLAGS                StateFlags;
    PSM_ENGINE_AUTO_EVENT_TRANSITION     AutoEventTransitions;
    PSM_ENGINE_STATE_TRANSITION          Transitions;
} SM_ENGINE_STATE_TABLE_ENTRY, *PSM_ENGINE_STATE_TABLE_ENTRY;

typedef struct _SM_ENGINE_CONTEXT {    
    WDFOBJECT                                 Owner;
    PVOID                                     StateEntryFunctionContext;
    PSM_ENGINE_STATE_TABLE_ENTRY*             StateTable;
    KSPIN_LOCK                                StateLock;
    PSM_ENGINE_STATE_TABLE_ENTRY              CurrentStateEntry;
    SM_ENGINE_STATE                           StateBase;
    SM_ENGINE_EVENT_QUEUE                     NormalPriQueue;
    SM_ENGINE_EVENT_QUEUE                     LowPriQueue; 
    PUCX_FORWARD_PROGRESS_WORKITEM            UcxForwardProgressWorkItem;
    SM_ENGINE_STATE_LOG                       StateHistory[STATE_HISTORY_DEPTH];
    SM_ENGINE_EVENT                           EventHistory[EVENT_HISTORY_DEPTH];
    UCHAR                                     StateHistoryIndex;
    UCHAR                                     EventHistoryIndex;
    BOOLEAN                                   StateMachineRunning;
    BOOLEAN                                   StateMachinePaused;
    BOOLEAN                                   StateMachinePausedForPassive;
    UCHAR                                     SpecialHandlingForAbortUrb:1;
    UCHAR                                     SpecialHandlingForControllerResetEventsFromHCD:1;
    UCHAR                                     EventHCDNeedsResetIsQueued:1;
    UCHAR                                     EventResetCompleteFromHCDIsQueued:1;
    UCHAR                                     :4;
    ULONG                                     UnexpectedEventCount;
    PFN_SM_ENGINE_ETW_WRITE_STATE_EVENT       EtwWriteStateEvent;
    PFN_SM_ENGINE_ETW_WRITE_STATE_TRANSITION  EtwWriteStateTransition;
} SM_ENGINE_CONTEXT, *PSM_ENGINE_CONTEXT;

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
);

VOID
StateMachineEngine_ReleaseResources(
    __in
        PSM_ENGINE_CONTEXT            SmEngineContext
);

BOOLEAN
StateMachineEngine_EventAdd(
    __in
        PSM_ENGINE_CONTEXT       SmEngineContext,
    __in
        SM_ENGINE_EVENT          Event               
    );

//__drv_mustHold(SpinLockObj)
FORCEINLINE 
VOID
EnqueueEvent( 
    __in 
        PSM_ENGINE_EVENT_QUEUE          EventQueue, 
    __in 
        SM_ENGINE_EVENT                 Event)
{
    EventQueue->Events[EventQueue->QueueTail] = Event;
    EventQueue->QueueTail = (EventQueue->QueueTail + 1) & QUEUE_MASK;
    NT_ASSERTMSG("EventQueue Overflow",
                 EventQueue->QueueTail != EventQueue->QueueHead);
}

//__drv_mustHold(SpinLockObj)
FORCEINLINE
SM_ENGINE_EVENT 
DequeueEvent(
    __in 
        PSM_ENGINE_EVENT_QUEUE        EventQueue
    )
{
    SM_ENGINE_EVENT event;
    event = EventQueue->Events[EventQueue->QueueHead];
    EventQueue->QueueHead = (EventQueue->QueueHead + 1) & QUEUE_MASK;
    return event;
}   

//__drv_mustHold(SpinLockObj)
FORCEINLINE 
BOOLEAN
IsQueueEmpty(
    __in 
        PSM_ENGINE_EVENT_QUEUE        EventQueue
    )
{
    return (EventQueue->QueueTail == EventQueue->QueueHead);
}

//__drv_mustHold(SpinLockObj)
FORCEINLINE
VOID
RecordEvent(
    __in 
        PSM_ENGINE_CONTEXT        SmEngineContext,
    __in
        SM_ENGINE_EVENT           Event) 
{
    SmEngineContext->EventHistory [ SmEngineContext->EventHistoryIndex] = Event;
    SmEngineContext->EventHistoryIndex = 
        (SmEngineContext->EventHistoryIndex + 1) & EVENT_HISTORY_MASK;
    SmEngineContext->EtwWriteStateEvent(NULL, 
                                        SmEngineContext->Owner,
                                        SmEngineContext,
                                        Event);
}
    
    
//__drv_mustHold(SpinLockObj)
FORCEINLINE
VOID
RecordState(
    __in 
        PSM_ENGINE_CONTEXT            SmEngineContext,
    __in
        SM_ENGINE_STATE               State,
    __in
        SM_ENGINE_EVENT               Event
    ) 
{   
    SmEngineContext->StateHistory[SmEngineContext->StateHistoryIndex].State = State;
    SmEngineContext->StateHistory[SmEngineContext->StateHistoryIndex].Event = Event;
    SmEngineContext->StateHistoryIndex = 
        (SmEngineContext->StateHistoryIndex + 1) & STATE_HISTORY_MASK;
    SmEngineContext->EtwWriteStateTransition(NULL, 
                                             SmEngineContext->Owner,
                                             SmEngineContext,
                                             Event,
                                             State);
}

