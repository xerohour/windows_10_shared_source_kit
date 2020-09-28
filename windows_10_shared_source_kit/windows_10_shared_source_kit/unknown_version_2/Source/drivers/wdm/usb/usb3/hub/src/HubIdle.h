
/*++        
Copyright (c) Microsoft Corporation

Module Name:

    HubIdle.h

Abstract:

    This file contains structures and defintions for
    the Idle Irp state machine.

--*/

#pragma once                                                                      

//
// Internal typedefs
//
#define ISM_STATE_HISTORY_DEPTH 16
#define ISM_STATE_HISTORY_MASK  0xf
                                           

// 
// Forward declarations for state and event enums. These will 
// actually be defined in the individual state machine files.
//
typedef enum _ISM_EVENT ISM_EVENT;
typedef enum _ISM_STATE ISM_STATE;

typedef enum _ISM_POSTLOCK_ACTION {
    
    IsmPostLockActionNone,
    IsmPostLockActionCompleteIrpWithStatusCancelled,
    IsmPostLockActionCompleteIrpWithStatusDeviceBusy,
    IsmPostLockActionCompleteIrpWithStatusNoSuchDevice,
    IsmPostLockActionCompleteIrpWithStatusInvalidDeviceState,
    IsmPostLockActionCompleteIrpWithStatusSuccess,
    IsmPostLockActionInvokeClientCallback,
    IsmPostLockActionQueueWorkItem,
    IsmPostLockActionCompleteIrpWithStatusCancelledAndReturnPending

}ISM_POSTLOCK_ACTION;

typedef struct _ISM_STATE_LOG {
    
    ISM_EVENT       Event;
    ISM_STATE       State;

} ISM_STATE_LOG, *PISM_STATE_LOG;


typedef const struct _ISM_STATE_ENTRY *PISM_STATE_ENTRY;

typedef struct _ISM_CONTEXT {

    ISM_STATE_LOG           StateHistory[ISM_STATE_HISTORY_DEPTH];
    UCHAR                   StateHistoryIndex;
    _Has_lock_kind_(_Lock_kind_spin_lock_)
        KSPIN_LOCK              StateMachineLock;
    ISM_STATE               CurrentState;
    PISM_STATE_ENTRY *      StateTable;   
    ULONG                   StartIndex;  
    PIRP                    IdleIrp;
    ISM_POSTLOCK_ACTION     PostLockAction;
    WDFWORKITEM             IdleWorker;     

} ISM_CONTEXT, *PISM_CONTEXT;

typedef struct _IDLE_WORKER_CONTEXT {

    PISM_CONTEXT             SmContext;
    
} IDLE_WORKER_CONTEXT, *PIDLE_WORKER_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(IDLE_WORKER_CONTEXT, GetIdleWorkItemContext)

EVT_WDF_WORKITEM    HUBIDLE_EvtIdleWorkItem;


//
// Function type used by all state machines for their entry functions
//                                                                   
typedef
ISM_EVENT
(ISM_STATE_ENTRY_FUNCTION)(
    _In_ 
        PISM_CONTEXT        SmContext,
    _In_opt_
        PIRP                IdleIrp
    );

typedef ISM_STATE_ENTRY_FUNCTION* PISM_STATE_ENTRY_FUNCTION;

//
// ISM Helper function
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
_Requires_lock_held_(IdleContext->StateMachineLock)
ISM_EVENT
(ISM_OPERATION_SYNC_DISPATCH)(
    _In_ 
        PISM_CONTEXT        IdleContext,
    _In_opt_
        PIRP                IdleIrp
    );

typedef ISM_OPERATION_SYNC_DISPATCH* PISM_OPERATION_SYNC_DISPATCH;

typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
_Requires_lock_held_(IdleContext->StateMachineLock)
VOID
(ISM_OPERATION_ASYNC_DISPATCH)(
    _In_ 
        PISM_CONTEXT        IdleContext,
    _In_opt_
        PIRP                IdleIrp
    );

typedef ISM_OPERATION_ASYNC_DISPATCH* PISM_OPERATION_ASYNC_DISPATCH;

//
// Pair defining which target state to go to , on getting an event
//
typedef const struct _ISM_STATE_TRANSITION {

    ISM_EVENT                   Event;
    ISM_STATE                   TargetState;

} ISM_STATE_TRANSITION, *PISM_STATE_TRANSITION;
                                                                        
//
// Disabling the following warning. The use of 0 sized array is OK and intended.
// warning C4200: nonstandard extension used : zero-sized array in struct/union
// 
#pragma warning(disable:4200)

typedef const struct _ISM_STATE_ENTRY {

    ULONG                       EtwName;
    PISM_STATE_ENTRY_FUNCTION   EntryFunc;
    ISM_STATE_TRANSITION        Transitions[];

} ISM_STATE_ENTRY, *PISM_STATE_ENTRY;

#pragma warning(default:4200)

//
// Internal Function declarations
//

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
    );                       


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
    );       
      
DRIVER_CANCEL HUBIDLE_CancelIdle;
