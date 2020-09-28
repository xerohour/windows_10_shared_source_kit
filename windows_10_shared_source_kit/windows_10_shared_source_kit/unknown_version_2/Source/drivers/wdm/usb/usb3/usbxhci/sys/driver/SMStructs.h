
/*++        
Copyright (c) Microsoft Corporation

Module Name:

    SMStructs.h

Abstract:

    This file contains structures and defintions for
    the hub state machines.

--*/

#pragma once                                                                      

//
// Internal typedefs
//
#define MAX_QUEUE_DEPTH     16
#define MAX_STATE_DEPTH     7
#define QUEUE_MASK          0x0f
#define STATE_HISTORY_DEPTH 64
#define STATE_HISTORY_MASK  0x3f
#define EVENT_HISTORY_DEPTH 16
#define EVENT_HISTORY_MASK  0x0f

#define EVENT_PRIORITY_MASK 0x00000003

#define EVENT_PRIORITY(e) ((e).EventAsUlong & EVENT_PRIORITY_MASK)

typedef enum _EVENT_PRIORITY{
    EventPriorityCritical         = 0,
    EventPrioritySync             = 1,
    EventPriorityNormal           = 2,
    EventPriorityLow              = 3
} EVENT_PRIORITY, *PEVENT_PRIORITY;


//
// These indices determine at which number the state and event
// enumerations will start for each state machine. Even though
// these enums are independent, we make sure that their values
// don't overlap (except for some special enums -null,ignored,
// etc.). This ensures that when we dump the union of the enums,
// exactly one of the members will have a matching name.
// Also, the starting state of the sub state machine is guaranteed
// to have the StartIndex as the Enum value for it. This helps us
// prevent exposing external code to specific state names in the 
// state machines
//
typedef enum _START_INDICES {

    StartIndexEsm     =   2000,

} START_INDICES;

// 
// Forward declarations for state and event enums. These will 
// actually be defined in the individual state machine files.
//
typedef enum _ESM_EVENT ESM_EVENT;


typedef enum _ESM_STATE ESM_STATE;


//
// The generic state enums will overlap with every other enum.
// The idea is that the state machine engine can be made 
// state machine agnostic
//   
typedef enum _GENERIC_STATE {

    GenericStateIgnored                                  = 1000,
    GenericStateEmptySubState                            = 1001,
    GenericStateNull                                     = 1002

} GENERIC_STATE;

//
// We define a union of all types so that it can be used in the generic
// state machine engine
//
typedef union _STATE {

    ULONG           StateAsUlong;
    GENERIC_STATE   GenericState;
    ESM_STATE       EsmState;

} STATE;        

//
// The generic event enums will overlap with every other enum.
// The idea is that the state machine engine can be made 
// state machine agnostic
//   
typedef enum _GENERIC_EVENT {

    GenericEventNull                                   = 1000,
    //
    // This event is queued by the state machine engine
    // when it finds that there are sub state machines
    // present for a state but none match the current
    // object. This helps prevent the need for having
    // Empty Sub state machines. This event actually
    // appears in the state machine, so need to have the 
    // right priority
    //
    GenericEventContinueIfNoSubState                   = 1001 | EventPrioritySync,

    GenericEventTimerFired                             = 0x0000007c | EventPriorityNormal,

    //
    // This is used internally by the state machine
    // engine to indicate that a worker item has
    // been queued and so the engine should quit.
    // This is only used as a return value in the 
    // internal state machine functions and is not 
    // a real event.
    //
    GenericEventQuitForWorkerItem                      = 1003

} GENERIC_EVENT;

//
// We define a union of all types so that it can be used in the generic
// state machine engine
//
typedef union _EVENT {

    ULONG           EventAsUlong;
    ESM_EVENT       EsmEvent;

} EVENT;             

typedef struct _STATE_LOG {

    EVENT           Event;
    STATE           State;
    ULONG           Depth;

} STATE_LOG, *PSTATE_LOG;

typedef struct _EVENTS_QUEUE {

    EVENT           Events[MAX_QUEUE_DEPTH];
    UCHAR           QueueHead;
    UCHAR           QueueTail;

} EVENTS_QUEUE, *PEVENTS_QUEUE;

typedef struct _STATE_ENTRY *PSTATE_ENTRY;

typedef struct _ENDPOINT_DATA *PENDPOINT_DATA;

typedef struct _SM_CONTEXT {

    STATE_LOG           StateHistory[STATE_HISTORY_DEPTH];
    EVENT               EventHistory[EVENT_HISTORY_DEPTH];
    UCHAR               StateHistoryIndex;
    UCHAR               EventHistoryIndex;
    KSPIN_LOCK          StateMachineLock;
    BOOLEAN             StateMachineRunning;
    STATE               CurrentState[MAX_STATE_DEPTH];
    ULONG               CurrentStateDepth;
    EVENTS_QUEUE        EventQueue; 
    ULONG               SubSmFlags;
    PVOID               ParentContext;
    PSTATE_ENTRY *      StateTable;   
    ULONG               StartIndex;  
    PIO_WORKITEM        WorkItem;   
    GUID                CurrentActivityId;
    WDFTIMER            Timer;
    BOOLEAN             WaitingForTimerFired;    
    BOOLEAN             DebugBreakOnStateTransition;

} SM_CONTEXT, *PSM_CONTEXT;

//
// Entry function for the Endpoint State Machine.
//
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
ESM_AddEsmEvent(
    __inout
        PENDPOINT_DATA    EndpointData,
    __in
        ESM_EVENT         Event
    );

//
// Function type used by all state machines for their entry functions
//                                                                   
typedef
ULONG
(STATE_ENTRY_FUNCTION)(
    __in 
        PSM_CONTEXT         SmContext
    );

typedef STATE_ENTRY_FUNCTION* PSTATE_ENTRY_FUNCTION;

//
// ESM Helper function that can be run at disptach
//
typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(ESM_OPERATION_ASYNC_DISPATCH)(
    __in PENDPOINT_DATA          EndpointData
    );

typedef ESM_OPERATION_ASYNC_DISPATCH* PESM_OPERATION_ASYNC_DISPATCH;

//
// ESM Helper function that can be run at only passive
//
typedef
__drv_maxIRQL(PASSIVE_LEVEL)
VOID
(ESM_OPERATION_ASYNC_PASSIVE)(
    __in PENDPOINT_DATA          EndpointData
    );

typedef ESM_OPERATION_ASYNC_PASSIVE* PESM_OPERATION_ASYNC_PASSIVE;  

//
// ESM Helper function that can be run at disptach and returns
// and event
//
typedef
__drv_maxIRQL(DISPATCH_LEVEL)
ESM_EVENT
(ESM_OPERATION_SYNC_DISPATCH)(
    __in PENDPOINT_DATA          EndpointData
    );

typedef ESM_OPERATION_SYNC_DISPATCH* PESM_OPERATION_SYNC_DISPATCH;  

//
// ESM Helper function that can be run at only passive and returns 
// and event
//
typedef
__drv_maxIRQL(PASSIVE_LEVEL)
ESM_EVENT
(ESM_OPERATION_SYNC_PASSIVE)(
    __in PENDPOINT_DATA          EndpointData
    );

typedef ESM_OPERATION_SYNC_PASSIVE* PESM_OPERATION_SYNC_PASSIVE;    

//
// Pair defining which target state to go to , on getting an event
//
typedef struct _STATE_TRANSITION {

    EVENT                   Event;
    STATE                   TargetState;

} STATE_TRANSITION, *PSTATE_TRANSITION;

//
// State Flags that descibe the characteristics of a state
// like its event dispatch policy, its state entry function
// requirements ,etc.
//     
typedef enum _STATE_FLAGS {

    StateFlagNone                        =   0x0000,
    StateFlagHandlesCriticalEventsOnly   =   0x0001,
    StateFlagHandlesLowPriEvents         =   0x0002,
    StateFlagPopIfParentStateChanges     =   0x0004,
    StateFlagEndSubStateMachine          =   0x0008,
    StateFlagRequiresPassive             =   0x0010,   
    StateFlagStopTimerOnExit             =   0x0020,

} STATE_FLAGS;                                                          
typedef union _STATE_FLAGS_DEBUG {

    STATE_FLAGS  StateFlags;

    //
    // Each field in this struct correspond to ascending enumerant values
    // in StateFlags.
    // NOTE:  you should never touch these fields in any code, they are here
    //        strictly for use by the debugger extension so that it doesn't
    //       have to result StateFlags enumerant name strings into values.
    //        
    struct {

        ULONG None:1;
        ULONG HandlesCriticalEventsOnly:1;
        ULONG HandlesLowPriEvents:1;
        ULONG PopIfParentStateChanges:1;
        ULONG EndSubStateMachine:1;
        ULONG FlagRequiresPassive:1;
        ULONG StopTimerOnExit:1;        

    }StateFlagsByName;

} STATE_FLAGS_DEBUG; 

C_ASSERT(sizeof(STATE_FLAGS_DEBUG) == sizeof(STATE_FLAGS));

//
// These flags are associated with every sub state machine
// of a HSM state and also with the given hub. Only if the 
// given hub SubSmFlags are a subset of the SubSmFlags 
// associated with the sub state machine, that sub state 
// machine is entered
//                                                         
typedef enum _ESM_SUBSM_FLAGS {

    EsmSubSmFlagNone                        =   0,
    EsmSubSmFlagHubRoot                     =   0x00000001,
    //
    // When no flags are specified for a sub state ,that 
    // indicates the sub state machine is applicable for
    // all hubs
    //
    EsmSubSmFlagAny                        =     0x000000ff

} ESM_SUBSM_FLAGS;                                         

typedef union _ESM_SUBSM_FLAGS_DEBUG {

    ESM_SUBSM_FLAGS  SubSmFlags;

    //
    // Each field in this struct correspond to ascending enumerant values
    // in EsmSubSmFlags.
    // NOTE:  you should never touch these fields in any code, they are here
    //        strictly for use by the debugger extension so that it doesn't
    //       have to result EsmSubSmFlags enumerant name strings into values.
    //        
    struct {

        ULONG HubRoot:1;

    }EsmSubSmFlagsByName;

} ESM_SUBSM_FLAGS_DEBUG;

C_ASSERT(sizeof(ESM_SUBSM_FLAGS_DEBUG) == sizeof(ESM_SUBSM_FLAGS));     

//
// Union is defined so that generic state machine engine can use it
//
typedef union _SUBSM_FLAGS {

    ULONG                   SubSmFlagsAsUlong;
    ESM_SUBSM_FLAGS       EsmSubSmFlags;

} SUBSM_FLAGS;

typedef struct _SUBSM_ENTRY {

    SUBSM_FLAGS             SubSmFlags;
    STATE                   SubSmStartState;

} SUBSM_ENTRY, *PSUBSM_ENTRY;

//
// Disabling the following warning. The use of 0 sized array is OK and intended.
// warning C4200: nonstandard extension used : zero-sized array in struct/union
// 
#pragma warning(disable:4200)

typedef struct _STATE_ENTRY {

    ULONG                   EtwName;
    PSTATE_ENTRY_FUNCTION   EntryFunc;
    STATE_FLAGS             StateFlags;
    PSUBSM_ENTRY            SubSmEntries;
    STATE_TRANSITION        Transitions[];

} STATE_ENTRY, *PSTATE_ENTRY;

#pragma warning(default:4200)

//
// Internal Function declarations
//

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
ESM_AddEvent(
    __inout 
        PSM_CONTEXT         SmContext,
    __in    
        EVENT               Event
    );                                                                     

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
ESM_FindAndSetTargetState(
    __inout 
        PSM_CONTEXT         SmContext,
    __in    
        EVENT               Event,
    __out
        PBOOLEAN            PreviousStateNeedsStopTimerOnExit
    );                                                                     

__drv_maxIRQL(DISPATCH_LEVEL)
EVENT
ESM_GetNextEvent(
    __inout 
        PSM_CONTEXT         SmContext
    );                                                                     

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
ESM_RunStateMachine(
    __inout 
        PSM_CONTEXT         SmContext,
    __in
        EVENT               SyncEvent,
    __inout
        PKIRQL              Irql     
    );                                                                     

__drv_maxIRQL(DISPATCH_LEVEL)
EVENT
ESM_ExecuteEntryFunctionsAndPushPopStateMachinesForCurrentState(
    __inout 
        PSM_CONTEXT         SmContext
    );

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
ESM_EnqueueEvent(
    __in_ecount(MAX_QUEUE_DEPTH)    
        PEVENTS_QUEUE       Queue,
    __in    
        EVENT               Event
    );    

__drv_maxIRQL(DISPATCH_LEVEL)
_Must_inspect_result_
FORCEINLINE
EVENT
ESM_DequeueEvent(
    __in_ecount(MAX_QUEUE_DEPTH)    
        PEVENTS_QUEUE   Queue,
    __in
        ULONG           StateDispatchFlags
    );        

__drv_maxIRQL(DISPATCH_LEVEL)
_Must_inspect_result_
FORCEINLINE
BOOLEAN
ESM_DequeueSpecificEvent(
    __in_ecount(MAX_QUEUE_DEPTH)    
        PEVENTS_QUEUE   Queue,
    __in
        EVENT           Event
    );

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
ESM_RecordEvent(
    __in
        PSM_CONTEXT         SmContext,
    __in    
        EVENT               Event
    );

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
ESM_RecordState(
    __in      
        PSM_CONTEXT         SmContext,
    __in      
        STATE               State,
    __in      
        ULONG               Depth,    
    __in      
        EVENT               Event
    );       

__drv_maxIRQL(DISPATCH_LEVEL)
_Requires_lock_held_(SmContext->StateMachineLock)
FORCEINLINE
VOID
ESM_PurgeEvent(
    __inout 
    PSM_CONTEXT         SmContext,
    __in_ecount(MAX_QUEUE_DEPTH)    
        PEVENTS_QUEUE       Queue,
    __in    
        EVENT               Event,
    __inout
        PKIRQL              Irql     
    );


IO_WORKITEM_ROUTINE     ESM_SmWorker;
