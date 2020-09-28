
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

//
// If the state machine has been running in the context of the caller 
// and has processed too many async events, we shift to the worker item 
// so that the thread can unwind and the caller can make progress.
// This value defines the maximum number of those async events. The
// value of 100 is somewhat arbitrary chosen based on the fact that 
// the longest sequence of consecutive async events is oberved to be
// 20 in case of device enumeration. 
//
#define MAX_ASYNC_EVENTS_IN_ARBITRARY_CONTEXT 100
    
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

    StartIndexHsm       =   2000,
    StartIndexPsm20     =   3000,
    StartIndexDsm       =   4000,
    StartIndexPsm30     =   5000,
    StartIndexIsm       =   6000

} START_INDICES;

//
// Forward declarations for state and event enums. These will
// actually be defined in the individual state machine files.
//
typedef enum _HSM_EVENT HSM_EVENT;
typedef enum _PSM_EVENT PSM_EVENT;
typedef enum _DSM_EVENT DSM_EVENT;


typedef enum _HSM_STATE HSM_STATE;
typedef enum _PSM20_STATE PSM20_STATE;
typedef enum _PSM30_STATE PSM30_STATE;
typedef enum _DSM_STATE DSM_STATE;


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
    HSM_STATE       HsmState;
    PSM20_STATE     Psm20State;
    PSM30_STATE     Psm30State;
    DSM_STATE       DsmState;

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

    GenericEventTimerFired                             = 1002 | EventPriorityNormal,

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
    HSM_EVENT       HsmEvent;
    PSM_EVENT       PsmEvent;
    DSM_EVENT       DsmEvent;

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
typedef
VOID
(HUBSM_ADD_EVENT)(
    __inout
        PVOID               Context,
    __in
        ULONG               Event
    );

typedef HUBSM_ADD_EVENT *PHUBSM_ADD_EVENT;

HUBSM_ADD_EVENT HUBSM_AddHsmEvent;

HUBSM_ADD_EVENT HUBSM_AddPsm20Event;

HUBSM_ADD_EVENT HUBSM_AddDsmEvent;

typedef const struct _STATE_ENTRY *PSTATE_ENTRY;

typedef struct _HUB_FDO_CONTEXT *PHUB_FDO_CONTEXT;

typedef struct _PORT_CONTEXT *PPORT_CONTEXT;

typedef struct _DEVICE_CONTEXT *PDEVICE_CONTEXT;

typedef struct _SM_CONTEXT {

    STATE_LOG           StateHistory[STATE_HISTORY_DEPTH];
    EVENT               EventHistory[EVENT_HISTORY_DEPTH];
    UCHAR               StateHistoryIndex;
    UCHAR               EventHistoryIndex;
    _Has_lock_kind_(_Lock_kind_spin_lock_)
        KSPIN_LOCK          StateMachineLock;
    _Guarded_by_(StateMachineLock)
        BOOLEAN             StateMachineRunning;
    STATE               CurrentState[MAX_STATE_DEPTH];
    ULONG               CurrentStateDepth;
    _Guarded_by_(StateMachineLock)
        EVENT_QUEUE         EventQueue; 
    ULONG               SubSmFlags;
    PVOID               ParentContext;
    PHUBSM_ADD_EVENT    AddEvent;
    PSTATE_ENTRY *      StateTable;
    ULONG               StartIndex;
    PUCX_FORWARD_PROGRESS_WORKITEM
                        SmWorker;
    PHUB_FDO_CONTEXT    HubFdoContext;
    BOOLEAN             NeedsForwardProgress;
    GUID                CurrentActivityId;
    UCXUSBDEVICE        ParentHandle;
    PEX_TIMER           Timer;
    BOOLEAN             WaitingForTimerFired;
    BOOLEAN             DebugBreakOnStateTransition;

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
// HSM Helper function that can be run at disptach
//
typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(HSM_OPERATION_ASYNC_DISPATCH)(
    __in PHUB_FDO_CONTEXT       HubFdoContext
    );

typedef HSM_OPERATION_ASYNC_DISPATCH* PHSM_OPERATION_ASYNC_DISPATCH;

//
// HSM Helper function that can be run at only passive
//
typedef
__drv_maxIRQL(PASSIVE_LEVEL)
VOID
(HSM_OPERATION_ASYNC_PASSIVE)(
    __in PHUB_FDO_CONTEXT       HubFdoContext
    );

typedef HSM_OPERATION_ASYNC_PASSIVE* PHSM_OPERATION_ASYNC_PASSIVE;

//
// HSM Helper function that can be run at disptach and returns
// and event
//
typedef
__drv_maxIRQL(DISPATCH_LEVEL)
HSM_EVENT
(HSM_OPERATION_SYNC_DISPATCH)(
    __in PHUB_FDO_CONTEXT       HubFdoContext
    );

typedef HSM_OPERATION_SYNC_DISPATCH* PHSM_OPERATION_SYNC_DISPATCH;

//
// HSM Helper function that can be run at only passive and returns
// and event
//
typedef
__drv_maxIRQL(PASSIVE_LEVEL)
HSM_EVENT
(HSM_OPERATION_SYNC_PASSIVE)(
    __in PHUB_FDO_CONTEXT       HubFdoContext
    );

typedef HSM_OPERATION_SYNC_PASSIVE* PHSM_OPERATION_SYNC_PASSIVE;

//
// PSM Helper function that can be run at disptach
//
typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(PSM_OPERATION_ASYNC_DISPATCH)(
    __in PPORT_CONTEXT       PortContext
    );

typedef PSM_OPERATION_ASYNC_DISPATCH* PPSM_OPERATION_ASYNC_DISPATCH;

//
// PSM Helper function that can be run at only passive
//
typedef
__drv_maxIRQL(PASSIVE_LEVEL)
VOID
(PSM_OPERATION_ASYNC_PASSIVE)(
    __in PPORT_CONTEXT       PortContext
    );

typedef PSM_OPERATION_ASYNC_PASSIVE* PPSM_OPERATION_ASYNC_PASSIVE;

//
// PSM Helper function that can be run at disptach and returns
// and event
//
typedef
__drv_maxIRQL(DISPATCH_LEVEL)
PSM_EVENT
(PSM_OPERATION_SYNC_DISPATCH)(
    __in PPORT_CONTEXT       PortContext
    );

typedef PSM_OPERATION_SYNC_DISPATCH* PPSM_OPERATION_SYNC_DISPATCH;

//
// PSM Helper function that can be run at only passive and returns
// and event
//
typedef
__drv_maxIRQL(PASSIVE_LEVEL)
PSM_EVENT
(PSM_OPERATION_SYNC_PASSIVE)(
    __in PPORT_CONTEXT       PortContext
    );

typedef PSM_OPERATION_SYNC_PASSIVE* PPSM_OPERATION_SYNC_PASSIVE;

//
// DSM Helper function that can be run at disptach
//
typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(DSM_OPERATION_ASYNC_DISPATCH)(
    __in PDEVICE_CONTEXT       DeviceContext
    );

typedef DSM_OPERATION_ASYNC_DISPATCH* PDSM_OPERATION_ASYNC_DISPATCH;

//
// DSM Helper function that can be run at only passive
//
typedef
__drv_maxIRQL(PASSIVE_LEVEL)
VOID
(DSM_OPERATION_ASYNC_PASSIVE)(
    __in PDEVICE_CONTEXT       DeviceContext
    );

typedef DSM_OPERATION_ASYNC_PASSIVE* PDSM_OPERATION_ASYNC_PASSIVE;

//
// DSM Helper function that can be run at disptach and returns
// and event
//
typedef
__drv_maxIRQL(DISPATCH_LEVEL)
DSM_EVENT
(DSM_OPERATION_SYNC_DISPATCH)(
    __in PDEVICE_CONTEXT       DeviceContext
    );

typedef DSM_OPERATION_SYNC_DISPATCH* PDSM_OPERATION_SYNC_DISPATCH;

//
// DSM Helper function that can be run at only passive and returns
// and event
//
typedef
__drv_maxIRQL(PASSIVE_LEVEL)
DSM_EVENT
(DSM_OPERATION_SYNC_PASSIVE)(
    __in PDEVICE_CONTEXT       DeviceContext
    );

typedef DSM_OPERATION_SYNC_PASSIVE* PDSM_OPERATION_SYNC_PASSIVE;

//
// Pair defining which target state to go to , on getting an event
//
typedef const struct _STATE_TRANSITION {

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
    StateFlagAllowsHubSuspend            =   0x0020,
    StateFlagStopTimerOnExit             =   0x0040

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
typedef enum _HSM_SUBSM_FLAGS {

    HsmSubSmFlagNone                        =   0,
    HsmSubSmFlagHub20                       =   0x00000001,
    HsmSubSmFlagHub30                       =   0x00000002,
    HsmSubSmFlagHubRoot                     =   0x00000004,
    //
    // When no flags are specified for a sub state ,that
    // indicates the sub state machine is applicable for
    // all hubs
    //
    HsmSubSmFlagAny                        =     0x000000ff

} HSM_SUBSM_FLAGS;

typedef union _HSM_SUBSM_FLAGS_DEBUG {

    HSM_SUBSM_FLAGS  SubSmFlags;

    //
    // Each field in this struct correspond to ascending enumerant values
    // in HsmSubSmFlags.
    // NOTE:  you should never touch these fields in any code, they are here
    //        strictly for use by the debugger extension so that it doesn't
    //       have to result HsmSubSmFlags enumerant name strings into values.
    //
    struct {

        ULONG Hub20:1;
        ULONG Hub30:1;
        ULONG HubRoot:1;

    }HsmSubSmFlagsByName;

} HSM_SUBSM_FLAGS_DEBUG;

C_ASSERT(sizeof(HSM_SUBSM_FLAGS_DEBUG) == sizeof(HSM_SUBSM_FLAGS));

typedef enum _PSM20_SUBSM_FLAGS {

    Psm20SubSmFlagNone  = 0,
    Psm20SubSmFlagAny   = 1

} PSM20_SUBSM_FLAGS;


typedef enum _PSM30_SUBSM_FLAGS {

    Psm30SubSmFlagNone  = 0,
    Psm30SubSmFlagAny   = 1

} PSM30_SUBSM_FLAGS;

typedef enum _DSM_SUBSM_FLAGS {

    DsmSubSmFlagNone                    =   0,

    DsmSubSmFlagVersion10               =   0x00000001,
    DsmSubSmFlagVersion1X               =   0x00000002,
    DsmSubSmFlagVersion20               =   0x00000004,
    DsmSubSmFlagVersion2X               =   0x00000008,
    DsmSubSmFlagVersion3X               =   0x00000010,

    DsmSubSmFlagVersionAny              =   0x000000ff,

    DsmSubSmFlagSpeedHigh               =   0x00000100,
    DsmSubSmFlagSpeedFull               =   0x00000200,
    DsmSubSmFlagSpeedLow                =   0x00000400,
    DsmSubSmFlagSpeedSuper              =   0x00000800,

    DsmSubSmFlagSpeedAny                =   0x0000ff00,

    DsmSubSmFlagPort20                  =   0x00010000,
    DsmSubSmFlagPort30                  =   0x00020000,

    DsmSubSmFlagPortAny                 =   0x00ff0000,

    DsmSubSmFlagAny                     =   0x00ffffff

} DSM_SUBSM_FLAGS;

//
// Union is defined so that generic state machine engine can use it
//
typedef union _SUBSM_FLAGS {

    ULONG                   SubSmFlagsAsUlong;
    HSM_SUBSM_FLAGS         HsmSubSmFlags;
    PSM20_SUBSM_FLAGS       Psm20SubSmFlags;
    PSM30_SUBSM_FLAGS       Psm30SubSmFlags;
    DSM_SUBSM_FLAGS         DsmSubSmFlags;

} SUBSM_FLAGS;

typedef const struct _SUBSM_ENTRY {

    SUBSM_FLAGS             SubSmFlags;
    STATE                   SubSmStartState;

} SUBSM_ENTRY, *PSUBSM_ENTRY;


//
// Disabling the following warning. The use of 0 sized array is OK and intended.
// warning C4200: nonstandard extension used : zero-sized array in struct/union
//
#pragma warning(disable:4200)

typedef const struct _STATE_ENTRY {

    ULONG                   EtwName;
    PSTATE_ENTRY_FUNCTION   EntryFunc;
    STATE_FLAGS             StateFlags;
    PSUBSM_ENTRY            SubSmEntries;
    STATE_TRANSITION        Transitions[];

} STATE_ENTRY, *PSTATE_ENTRY;

#pragma warning(default:4200)

UCX_FORWARD_PROGRESS_WORKITEM_CALLBACK HUBSM_EvtSmWorkItem;

//
// Internal Function declarations
//

_Requires_lock_not_held_(SmContext->StateMachineLock)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBSM_AddEvent(
    __inout
        PSM_CONTEXT         SmContext,
    __in
        EVENT               Event
    );

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
HUBSM_FindAndSetTargetState(
    __inout
        PSM_CONTEXT         SmContext,
    __in
        EVENT               Event,
    _Out_
        PBOOLEAN            PreviousStateNeedsStopTimerOnExit
    );

__drv_maxIRQL(DISPATCH_LEVEL)
_Requires_lock_held_(SmContext->StateMachineLock)
EVENT
HUBSM_GetNextEvent(
    __inout
        PSM_CONTEXT         SmContext
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
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
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
EVENT
HUBSM_ExecuteEntryFunctionsAndPushPopStateMachinesForCurrentState(
    __inout
        PSM_CONTEXT         SmContext
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
VOID
HUBSM_EnqueueEvent(
    __in_ecount(MAX_QUEUE_DEPTH)
        PEVENT_QUEUE        Queue,
    __in
        EVENT               Event
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
_Must_inspect_result_
FORCEINLINE
EVENT
HUBSM_DequeueEvent(
    __in_ecount(MAX_QUEUE_DEPTH)
        PEVENT_QUEUE    Queue,
    __in
        ULONG           StateDispatchFlags
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
_Must_inspect_result_
FORCEINLINE
BOOLEAN
HUBSM_DequeueSpecificEvent(
    __in_ecount(MAX_QUEUE_DEPTH)
        PEVENT_QUEUE    Queue,
    __in
        EVENT           Event
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
_Requires_lock_held_(SmContext->StateMachineLock)
FORCEINLINE
VOID
HUBSM_RecordEvent(
    __in
        PSM_CONTEXT         SmContext,
    __in
        EVENT               Event
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
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
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
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
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
_Must_inspect_result_
FORCEINLINE
BOOLEAN
HUBSM_DoesCurrentStateAllowHubSuspend(
    __in
        PSM_CONTEXT         SmContext
    );

