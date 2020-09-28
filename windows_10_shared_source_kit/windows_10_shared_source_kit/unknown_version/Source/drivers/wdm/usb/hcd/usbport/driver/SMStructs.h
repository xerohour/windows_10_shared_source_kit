
/*++        
Copyright (c) Microsoft Corporation

Module Name:

    SMStructs.h

Abstract:

    This file contains structures and defintions for
    the usbport runtime power managment state machines.

--*/

#pragma once                                                                      

//
// Internal typedefs
//
#define MAX_QUEUE_DEPTH     16
#define MAX_STATE_DEPTH     1
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

#define TRY
#define LEAVE   goto __tryLabel;
#define FINALLY goto __tryLabel; __tryLabel:
#undef __try
#undef __finally


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

    StartIndexRtpm      =   2000

} START_INDICES;

// 
// Forward declarations for state and event enums. These will 
// actually be defined in the individual state machine files.
//
typedef enum _RTPM_EVENT RTPM_EVENT;


typedef enum _RTPM_STATE RTPM_STATE;

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
    RTPM_STATE      RtpmState;

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

    //
    // This is used internally by the state machine
    // engine to indicate that a worker item has
    // been queued and so the engine should quit.
    // This is only used as a return value in the 
    // internal state machine functions and is not 
    // a real event.
    //
    GenericEventQuitForWorkerItem                      = 1002 
    
} GENERIC_EVENT;

//
// We define a union of all types so that it can be used in the generic
// state machine engine
//
typedef union _EVENT {
    
    ULONG           EventAsUlong;
    RTPM_EVENT      RtpmEvent;

} EVENT;             

typedef struct _STATE_LOG {
    
    EVENT           Event;
    STATE           State;
    ULONG           Depth;

} STATE_LOG, *PSTATE_LOG;

typedef struct _EVENT_QUEUE {

    EVENT           Events[MAX_QUEUE_DEPTH];
    UCHAR           QueueHead;
    UCHAR           QueueTail;

} EVENT_QUEUE, *PEVENT_QUEUE;

//
// Function type used by the entry functions for all
// state machines
//
__drv_maxIRQL(DISPATCH_LEVEL)
__drv_neverHold(StateMachineLock)
typedef
VOID
(USBPORTSM_ADD_EVENT)(
    __inout 
        PVOID               Context,
    __in    
        ULONG               Event
    );                       

typedef USBPORTSM_ADD_EVENT *PUSBPORTSM_ADD_EVENT;

USBPORTSM_ADD_EVENT USBPORTSM_AddEvent;


typedef struct _STATE_ENTRY *PSTATE_ENTRY;


typedef struct _SM_CONTEXT {

    STATE_LOG           StateHistory[STATE_HISTORY_DEPTH];
    EVENT               EventHistory[EVENT_HISTORY_DEPTH];
    UCHAR               StateHistoryIndex;
    UCHAR               EventHistoryIndex;
    KSPIN_LOCK          StateMachineLock;
    BOOLEAN             StateMachineRunning;
    STATE               CurrentState[MAX_STATE_DEPTH];
    ULONG               CurrentStateDepth;
    EVENT_QUEUE         EventQueue; 
    ULONG               SubSmFlags;  
    PSTATE_ENTRY *      StateTable;   
    ULONG               StartIndex;  
    BOOLEAN             NeedsForwardProgress;   
    GUID                CurrentActivityId;
    PDEVICE_OBJECT      FdoDeviceObject;
    BOOLEAN             DebugBreakOnStateTransition;
    USB_WORKITEM_HANDLE SmWorker;

} SM_CONTEXT, *PSM_CONTEXT;

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
// RTPM Helper function that can be run at disptach
//
typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(RTPM_OPERATION_ASYNC_DISPATCH)(
    __in PDEVICE_OBJECT         FdoDeviceObject
    );

typedef RTPM_OPERATION_ASYNC_DISPATCH* PRTPM_OPERATION_ASYNC_DISPATCH;

//
// RTPM Helper function that can be run at only passive
//
typedef
__drv_maxIRQL(PASSIVE_LEVEL)
VOID
(RTPM_OPERATION_ASYNC_PASSIVE)(
    __in PDEVICE_OBJECT         FdoDeviceObject
    );

typedef RTPM_OPERATION_ASYNC_PASSIVE* PRTPM_OPERATION_ASYNC_PASSIVE;  

//
// RTPM Helper function that can be run at disptach and returns
// and event
//
typedef
__drv_maxIRQL(DISPATCH_LEVEL)
RTPM_EVENT
(RTPM_OPERATION_SYNC_DISPATCH)(
    __in PDEVICE_OBJECT         FdoDeviceObject
    );

typedef RTPM_OPERATION_SYNC_DISPATCH* PRTPM_OPERATION_SYNC_DISPATCH;  

//
// RTPM Helper function that can be run at only passive and returns 
// and event
//
typedef
__drv_maxIRQL(PASSIVE_LEVEL)
RTPM_EVENT
(RTPM_OPERATION_SYNC_PASSIVE)(
    __in PDEVICE_OBJECT         FdoDeviceObject
    );

typedef RTPM_OPERATION_SYNC_PASSIVE* PRTPM_OPERATION_SYNC_PASSIVE;    

 

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
    StateFlagAllowsHubSuspend            =   0x0020   

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
        ULONG AllowsHubSuspend:1;

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
typedef enum _RTPM_SUBSM_FLAGS {

    HsmSubSmFlagNone                        =   0,
    HsmSubSmFlagAny                        =     0x000000ff

} RTPM_SUBSM_FLAGS;                                         

typedef union _RTPM_SUBSM_FLAGS_DEBUG {
    
    RTPM_SUBSM_FLAGS  SubSmFlags;

} RTPM_SUBSM_FLAGS_DEBUG;

C_ASSERT(sizeof(RTPM_SUBSM_FLAGS_DEBUG) == sizeof(RTPM_SUBSM_FLAGS));     

                                         

//
// Union is defined so that generic state machine engine can use it
//
typedef union _SUBSM_FLAGS {

    ULONG                   SubSmFlagsAsUlong;
    RTPM_SUBSM_FLAGS        RtpmSubSmFlags;

} SUBSM_FLAGS;

typedef struct _SUBSM_ENTRY {

    SUBSM_FLAGS             SubSmFlags;
    STATE                   SubSmStartState;

} SUBSM_ENTRY, *PSUBSM_ENTRY;



typedef struct _STATE_ENTRY {

    PSTATE_ENTRY_FUNCTION   EntryFunc;
    STATE_FLAGS             StateFlags;
    PSUBSM_ENTRY            SubSmEntries;
    STATE_TRANSITION        Transitions[];

} STATE_ENTRY, *PSTATE_ENTRY;

//UCX_FORWARD_PROGRESS_WORKITEM_CALLBACK HUBSM_EvtSmWorkItem;

//
// Internal Function declarations
//

__drv_maxIRQL(DISPATCH_LEVEL)
__drv_neverHold(StateMachineLock)
VOID
USBPORTRTPM_AddEvent(
    __inout 
        PSM_CONTEXT         SmContext,
    __in    
        RTPM_EVENT          RtpmEvent
    );                                                                     

__drv_maxIRQL(DISPATCH_LEVEL)
__drv_neverHold(StateMachineLock)
BOOLEAN
USBPORTRTPM_FindAndSetTargetState(
    __inout 
        PSM_CONTEXT         SmContext,
    __in    
        EVENT               Event
    );                                                                     

__drv_maxIRQL(DISPATCH_LEVEL)
__drv_mustHold(StateMachineLock)
EVENT
USBPORTRTPM_GetNextEvent(
    __inout 
        PSM_CONTEXT         SmContext
    );                                                                     

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
    );                                                                     

__drv_maxIRQL(DISPATCH_LEVEL)
__drv_neverHold(StateMachineLock)
EVENT
USBPORTRTPM_ExecuteEntryFunctionsAndPushPopStateMachinesForCurrentState(
    __inout 
        PSM_CONTEXT         SmContext
    );

__drv_maxIRQL(DISPATCH_LEVEL)
__drv_mustHold(StateMachineLock)
FORCEINLINE
VOID
USBPORTRTPM_EnqueueEvent(
    __in_ecount(MAX_QUEUE_DEPTH)    
        PEVENT_QUEUE        Queue,
    __in    
        EVENT               Event
    );    
      
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
    );        
      
__drv_maxIRQL(DISPATCH_LEVEL)
__drv_mustHold(StateMachineLock)
FORCEINLINE
VOID
USBPORTRTPM_RecordEvent(
    __in
        PSM_CONTEXT         SmContext,
    __in    
        EVENT               Event
    );
      
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
    );       
      
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
    );

NTSTATUS
USBPORTRTPM_InitializeRtpmSm(
    __in
        PDEVICE_OBJECT  FdoDeviceObject
    );


VOID
USBPORTRTPM_FreeRtpmSm(
    __in
        PDEVICE_OBJECT  FdoDeviceObject
    );

VOID 
USBPORTRTPM_EvtSmWorkItem(
        PDEVICE_OBJECT                    DeviceObject,
        PSM_CONTEXT                       SmContext,
        PIO_WORKITEM                      WorkItem
    );
