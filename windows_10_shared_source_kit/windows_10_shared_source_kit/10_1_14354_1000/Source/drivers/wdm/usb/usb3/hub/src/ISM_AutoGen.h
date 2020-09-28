
/*++
Copyright (c) Microsoft Corporation

Module Name:

ISM_AutoGen.h

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/

#pragma once



//
// Enum of States in all the State Machines
//
typedef enum _ISM_STATE {
    IsmStateIgnored                                  = 1000,
    IsmStateNull                                     = 1001,
    IsmStateCompleteWithStatusDeviceBusy             = 1002,
    IsmStateCompleteWithStatusNoSuchDevice           = 1003,
    IsmStateStartState                               = 6000,
    IsmStateCallbackCompleted                        = 6001,
    IsmStateCheckingIfSSIsEnabled                    = 6002,
    IsmStateCompletingIdleIrpWithStatusCancelled     = 6003,
    IsmStateCompletingIdleIrpWithStatusCancelledInSSDisabled = 6004,
    IsmStateCompletingIdleIrpWithStatusCancelledReturnPending = 6005,
    IsmStateCompletingIdleIrpWithStatusInvalidDeviceState = 6006,
    IsmStateCompletingIdleIrpWithStatusSuccess       = 6007,
    IsmStateCompletingRequestWithStatusDeviceGone    = 6008,
    IsmStateIdleRequestPendingWithSSDisabled         = 6009,
    IsmStateInvokingClientCallback                   = 6010,
    IsmStateMarkingRequestCancellable                = 6011,
    IsmStateMarkingRequestCancellableInSSDisabled    = 6012,
    IsmStateNoIdleRequest                            = 6013,
    IsmStateNoIdleRequestWithSelectiveSuspendDisabled = 6014,
    IsmStatePDORemoved                               = 6015,
    IsmStateQueuingWorkerItem                        = 6016,
    IsmStateStoringRequestInSSDisabled               = 6017,
    IsmStateStoringRequestInSSEnabled                = 6018,
    IsmStateUnMarkingRequestAsCancelable             = 6019,
    IsmStateUnMarkingRequestCancellable              = 6020,
    IsmStateUnMarkingRequestCancellableOnD0          = 6021,
    IsmStateWaitingForCallbackComplete               = 6022,
    IsmStateWaitingForCallbackOnCancelD0             = 6023,
    IsmStateWaitingForCallbackOnCancellation         = 6024,
    IsmStateWaitingForCallbackOnCancelledD3          = 6025,
    IsmStateWaitingForCallbackOnD0                   = 6026,
    IsmStateWaitingForCallbackOnD3                   = 6027,
    IsmStateWaitingForCallbackOnRemove               = 6028,
    IsmStateWaitingForCancellation                   = 6029,
    IsmStateWaitingForCancellationOnRemove           = 6030,
    IsmStateWaitingForWorkerItem                     = 6031,
} ISM_STATE, *PISM_STATE;





//
// Enum of ETW state names
//
typedef enum _ISM_STATE_ETW {
    IsmStateEtwStartState                            = 6000,
    IsmStateEtwCallbackCompleted                     = 6001,
    IsmStateEtwCheckingIfSSIsEnabled                 = 6002,
    IsmStateEtwCompletingIdleIrpWithStatusCancelled  = 6003,
    IsmStateEtwCompletingIdleIrpWithStatusCancelledInSSDisabled = 6004,
    IsmStateEtwCompletingIdleIrpWithStatusCancelledReturnPending = 6029,
    IsmStateEtwCompletingIdleIrpWithStatusInvalidDeviceState = 6005,
    IsmStateEtwCompletingIdleIrpWithStatusSuccess    = 6006,
    IsmStateEtwCompletingRequestWithStatusDeviceGone = 6007,
    IsmStateEtwIdleRequestPendingWithSSDisabled      = 6008,
    IsmStateEtwInvokingClientCallback                = 6009,
    IsmStateEtwMarkingRequestCancellable             = 6010,
    IsmStateEtwMarkingRequestCancellableInSSDisabled = 6011,
    IsmStateEtwNoIdleRequest                         = 6012,
    IsmStateEtwNoIdleRequestWithSelectiveSuspendDisabled = 6013,
    IsmStateEtwPDORemoved                            = 6014,
    IsmStateEtwQueuingWorkerItem                     = 6015,
    IsmStateEtwStoringRequestInSSDisabled            = 6016,
    IsmStateEtwStoringRequestInSSEnabled             = 6017,
    IsmStateEtwUnMarkingRequestAsCancelable          = 6018,
    IsmStateEtwUnMarkingRequestCancellable           = 6019,
    IsmStateEtwUnMarkingRequestCancellableOnD0       = 6020,
    IsmStateEtwWaitingForCallbackComplete            = 6021,
    IsmStateEtwWaitingForCallbackOnCancelD0          = 6030,
    IsmStateEtwWaitingForCallbackOnCancellation      = 6022,
    IsmStateEtwWaitingForCallbackOnCancelledD3       = 6031,
    IsmStateEtwWaitingForCallbackOnD0                = 6023,
    IsmStateEtwWaitingForCallbackOnD3                = 6024,
    IsmStateEtwWaitingForCallbackOnRemove            = 6025,
    IsmStateEtwWaitingForCancellation                = 6026,
    IsmStateEtwWaitingForCancellationOnRemove        = 6027,
    IsmStateEtwWaitingForWorkerItem                  = 6028,
} ISM_STATE_ETW, *PISM_STATE_ETW;



//
// Definitions for State Entry Functions 
//
ISM_STATE_ENTRY_FUNCTION          HUBISM_CheckingIfSSIsEnabled;

ISM_STATE_ENTRY_FUNCTION          HUBISM_CompletingIdleIrpWithStatusCancelled;

ISM_STATE_ENTRY_FUNCTION          HUBISM_CompletingIdleIrpWithStatusCancelledInSSDisabled;

ISM_STATE_ENTRY_FUNCTION          HUBISM_CompletingIdleIrpWithStatusCancelledReturnPending;

ISM_STATE_ENTRY_FUNCTION          HUBISM_CompletingIdleIrpWithStatusInvalidDeviceState;

ISM_STATE_ENTRY_FUNCTION          HUBISM_CompletingIdleIrpWithStatusSuccess;

ISM_STATE_ENTRY_FUNCTION          HUBISM_CompletingRequestWithStatusDeviceGone;

ISM_STATE_ENTRY_FUNCTION          HUBISM_InvokingClientCallback;

ISM_STATE_ENTRY_FUNCTION          HUBISM_MarkingRequestCancellable;

ISM_STATE_ENTRY_FUNCTION          HUBISM_MarkingRequestCancellableInSSDisabled;

ISM_STATE_ENTRY_FUNCTION          HUBISM_QueuingWorkerItem;

ISM_STATE_ENTRY_FUNCTION          HUBISM_StoringRequestInSSDisabled;

ISM_STATE_ENTRY_FUNCTION          HUBISM_StoringRequestInSSEnabled;

ISM_STATE_ENTRY_FUNCTION          HUBISM_UnMarkingRequestAsCancelable;

ISM_STATE_ENTRY_FUNCTION          HUBISM_UnMarkingRequestCancellable;

ISM_STATE_ENTRY_FUNCTION          HUBISM_UnMarkingRequestCancellableOnD0;






//
// State Entries for the states in the State Machine
//
ISM_STATE_ENTRY   IsmStateEntryCallbackCompleted = {
    // State ETW Name
    IsmStateEtwCallbackCompleted,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventWorkerRoutine ,       IsmStateIgnored },
        { IsmEventNewRequest ,          IsmStateCompleteWithStatusDeviceBusy },
        { IsmEventPDOCleanup ,          IsmStateUnMarkingRequestCancellable },
        { IsmEventIrpCancelled ,        IsmStateCompletingIdleIrpWithStatusCancelled },
        { IsmEventPDOD0 ,               IsmStateUnMarkingRequestCancellableOnD0 },
        { IsmEventPDOD3 ,               IsmStateUnMarkingRequestAsCancelable },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryCheckingIfSSIsEnabled = {
    // State ETW Name
    IsmStateEtwCheckingIfSSIsEnabled,
    // State Entry Function
    HUBISM_CheckingIfSSIsEnabled,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationFailure ,    IsmStateNoIdleRequestWithSelectiveSuspendDisabled },
        { IsmEventOperationSuccess ,    IsmStateNoIdleRequest },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryCompletingIdleIrpWithStatusCancelled = {
    // State ETW Name
    IsmStateEtwCompletingIdleIrpWithStatusCancelled,
    // State Entry Function
    HUBISM_CompletingIdleIrpWithStatusCancelled,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStateNoIdleRequest },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryCompletingIdleIrpWithStatusCancelledInSSDisabled = {
    // State ETW Name
    IsmStateEtwCompletingIdleIrpWithStatusCancelledInSSDisabled,
    // State Entry Function
    HUBISM_CompletingIdleIrpWithStatusCancelledInSSDisabled,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStateNoIdleRequestWithSelectiveSuspendDisabled },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryCompletingIdleIrpWithStatusCancelledReturnPending = {
    // State ETW Name
    IsmStateEtwCompletingIdleIrpWithStatusCancelledReturnPending,
    // State Entry Function
    HUBISM_CompletingIdleIrpWithStatusCancelledReturnPending,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStateNoIdleRequest },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryCompletingIdleIrpWithStatusInvalidDeviceState = {
    // State ETW Name
    IsmStateEtwCompletingIdleIrpWithStatusInvalidDeviceState,
    // State Entry Function
    HUBISM_CompletingIdleIrpWithStatusInvalidDeviceState,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStateNoIdleRequest },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryCompletingIdleIrpWithStatusSuccess = {
    // State ETW Name
    IsmStateEtwCompletingIdleIrpWithStatusSuccess,
    // State Entry Function
    HUBISM_CompletingIdleIrpWithStatusSuccess,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStateNoIdleRequest },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryCompletingRequestWithStatusDeviceGone = {
    // State ETW Name
    IsmStateEtwCompletingRequestWithStatusDeviceGone,
    // State Entry Function
    HUBISM_CompletingRequestWithStatusDeviceGone,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStatePDORemoved },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryIdleRequestPendingWithSSDisabled = {
    // State ETW Name
    IsmStateEtwIdleRequestPendingWithSSDisabled,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventNewRequest ,          IsmStateCompleteWithStatusDeviceBusy },
        { IsmEventPDOD3 ,               IsmStateIgnored },
        { IsmEventPDOD0 ,               IsmStateIgnored },
        { IsmEventIrpCancelled ,        IsmStateCompletingIdleIrpWithStatusCancelledInSSDisabled },
        { IsmEventPDOCleanup ,          IsmStateUnMarkingRequestCancellable },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryInvokingClientCallback = {
    // State ETW Name
    IsmStateEtwInvokingClientCallback,
    // State Entry Function
    HUBISM_InvokingClientCallback,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStateWaitingForCallbackComplete },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryMarkingRequestCancellable = {
    // State ETW Name
    IsmStateEtwMarkingRequestCancellable,
    // State Entry Function
    HUBISM_MarkingRequestCancellable,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStateQueuingWorkerItem },
        { IsmEventOperationFailure ,    IsmStateCompletingIdleIrpWithStatusCancelledReturnPending },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryMarkingRequestCancellableInSSDisabled = {
    // State ETW Name
    IsmStateEtwMarkingRequestCancellableInSSDisabled,
    // State Entry Function
    HUBISM_MarkingRequestCancellableInSSDisabled,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStateIdleRequestPendingWithSSDisabled },
        { IsmEventOperationFailure ,    IsmStateCompletingIdleIrpWithStatusCancelledInSSDisabled },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryNoIdleRequest = {
    // State ETW Name
    IsmStateEtwNoIdleRequest,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventIrpCancelled ,        IsmStateIgnored },
        { IsmEventPDOD3 ,               IsmStateIgnored },
        { IsmEventWorkerRoutine ,       IsmStateIgnored },
        { IsmEventPDOD0 ,               IsmStateIgnored },
        { IsmEventNewRequest ,          IsmStateStoringRequestInSSEnabled },
        { IsmEventPDOCleanup ,          IsmStatePDORemoved },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryNoIdleRequestWithSelectiveSuspendDisabled = {
    // State ETW Name
    IsmStateEtwNoIdleRequestWithSelectiveSuspendDisabled,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventPDOD3 ,               IsmStateIgnored },
        { IsmEventPDOD0 ,               IsmStateIgnored },
        { IsmEventNewRequest ,          IsmStateStoringRequestInSSDisabled },
        { IsmEventPDOCleanup ,          IsmStatePDORemoved },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryPDORemoved = {
    // State ETW Name
    IsmStateEtwPDORemoved,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventWorkerRoutine ,       IsmStateIgnored },
        { IsmEventNewRequest ,          IsmStateCompleteWithStatusNoSuchDevice },
        { IsmEventIrpCancelled ,        IsmStateIgnored },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryQueuingWorkerItem = {
    // State ETW Name
    IsmStateEtwQueuingWorkerItem,
    // State Entry Function
    HUBISM_QueuingWorkerItem,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStateWaitingForWorkerItem },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryStartState = {
    // State ETW Name
    IsmStateEtwStartState,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventStart ,               IsmStateCheckingIfSSIsEnabled },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryStoringRequestInSSDisabled = {
    // State ETW Name
    IsmStateEtwStoringRequestInSSDisabled,
    // State Entry Function
    HUBISM_StoringRequestInSSDisabled,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStateMarkingRequestCancellableInSSDisabled },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryStoringRequestInSSEnabled = {
    // State ETW Name
    IsmStateEtwStoringRequestInSSEnabled,
    // State Entry Function
    HUBISM_StoringRequestInSSEnabled,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStateMarkingRequestCancellable },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryUnMarkingRequestAsCancelable = {
    // State ETW Name
    IsmStateEtwUnMarkingRequestAsCancelable,
    // State Entry Function
    HUBISM_UnMarkingRequestAsCancelable,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStateCompletingIdleIrpWithStatusInvalidDeviceState },
        { IsmEventOperationFailure ,    IsmStateWaitingForCancellation },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryUnMarkingRequestCancellable = {
    // State ETW Name
    IsmStateEtwUnMarkingRequestCancellable,
    // State Entry Function
    HUBISM_UnMarkingRequestCancellable,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStateCompletingRequestWithStatusDeviceGone },
        { IsmEventOperationFailure ,    IsmStateWaitingForCancellationOnRemove },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryUnMarkingRequestCancellableOnD0 = {
    // State ETW Name
    IsmStateEtwUnMarkingRequestCancellableOnD0,
    // State Entry Function
    HUBISM_UnMarkingRequestCancellableOnD0,
    // Event State Pairs for Transitions
    {
        { IsmEventOperationSuccess ,    IsmStateCompletingIdleIrpWithStatusSuccess },
        { IsmEventOperationFailure ,    IsmStateWaitingForCancellation },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryWaitingForCallbackComplete = {
    // State ETW Name
    IsmStateEtwWaitingForCallbackComplete,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventNewRequest ,          IsmStateCompleteWithStatusDeviceBusy },
        { IsmEventWorkerRoutine ,       IsmStateIgnored },
        { IsmEventCallbackReturned ,    IsmStateCallbackCompleted },
        { IsmEventPDOD0 ,               IsmStateWaitingForCallbackOnD0 },
        { IsmEventIrpCancelled ,        IsmStateWaitingForCallbackOnCancellation },
        { IsmEventPDOD3 ,               IsmStateWaitingForCallbackOnD3 },
        { IsmEventPDOCleanup ,          IsmStateWaitingForCallbackOnRemove },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryWaitingForCallbackOnCancelD0 = {
    // State ETW Name
    IsmStateEtwWaitingForCallbackOnCancelD0,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventWorkerRoutine ,       IsmStateIgnored },
        { IsmEventPDOD0 ,               IsmStateIgnored },
        { IsmEventPDOD3 ,               IsmStateIgnored },
        { IsmEventNewRequest ,          IsmStateCompleteWithStatusDeviceBusy },
        { IsmEventCallbackReturned ,    IsmStateCompletingIdleIrpWithStatusSuccess },
        { IsmEventPDOCleanup ,          IsmStateWaitingForCallbackOnRemove },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryWaitingForCallbackOnCancellation = {
    // State ETW Name
    IsmStateEtwWaitingForCallbackOnCancellation,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventPDOD0 ,               IsmStateIgnored },
        { IsmEventPDOD3 ,               IsmStateIgnored },
        { IsmEventNewRequest ,          IsmStateCompleteWithStatusDeviceBusy },
        { IsmEventWorkerRoutine ,       IsmStateIgnored },
        { IsmEventCallbackReturned ,    IsmStateCompletingIdleIrpWithStatusCancelled },
        { IsmEventPDOCleanup ,          IsmStateWaitingForCallbackOnRemove },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryWaitingForCallbackOnCancelledD3 = {
    // State ETW Name
    IsmStateEtwWaitingForCallbackOnCancelledD3,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventWorkerRoutine ,       IsmStateIgnored },
        { IsmEventPDOD0 ,               IsmStateIgnored },
        { IsmEventPDOD3 ,               IsmStateIgnored },
        { IsmEventNewRequest ,          IsmStateCompleteWithStatusDeviceBusy },
        { IsmEventCallbackReturned ,    IsmStateCompletingIdleIrpWithStatusInvalidDeviceState },
        { IsmEventPDOCleanup ,          IsmStateWaitingForCallbackOnRemove },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryWaitingForCallbackOnD0 = {
    // State ETW Name
    IsmStateEtwWaitingForCallbackOnD0,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventNewRequest ,          IsmStateCompleteWithStatusDeviceBusy },
        { IsmEventPDOD0 ,               IsmStateIgnored },
        { IsmEventWorkerRoutine ,       IsmStateIgnored },
        { IsmEventPDOD3 ,               IsmStateIgnored },
        { IsmEventCallbackReturned ,    IsmStateUnMarkingRequestCancellableOnD0 },
        { IsmEventPDOCleanup ,          IsmStateWaitingForCallbackOnRemove },
        { IsmEventIrpCancelled ,        IsmStateWaitingForCallbackOnCancelD0 },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryWaitingForCallbackOnD3 = {
    // State ETW Name
    IsmStateEtwWaitingForCallbackOnD3,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventWorkerRoutine ,       IsmStateIgnored },
        { IsmEventPDOD0 ,               IsmStateIgnored },
        { IsmEventNewRequest ,          IsmStateCompleteWithStatusDeviceBusy },
        { IsmEventPDOD3 ,               IsmStateIgnored },
        { IsmEventIrpCancelled ,        IsmStateWaitingForCallbackOnCancelledD3 },
        { IsmEventCallbackReturned ,    IsmStateUnMarkingRequestAsCancelable },
        { IsmEventPDOCleanup ,          IsmStateWaitingForCallbackOnRemove },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryWaitingForCallbackOnRemove = {
    // State ETW Name
    IsmStateEtwWaitingForCallbackOnRemove,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventNewRequest ,          IsmStateCompleteWithStatusNoSuchDevice },
        { IsmEventIrpCancelled ,        IsmStateIgnored },
        { IsmEventWorkerRoutine ,       IsmStateIgnored },
        { IsmEventCallbackReturned ,    IsmStatePDORemoved },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryWaitingForCancellation = {
    // State ETW Name
    IsmStateEtwWaitingForCancellation,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventNewRequest ,          IsmStateCompleteWithStatusDeviceBusy },
        { IsmEventWorkerRoutine ,       IsmStateIgnored },
        { IsmEventPDOD0 ,               IsmStateIgnored },
        { IsmEventPDOD3 ,               IsmStateIgnored },
        { IsmEventIrpCancelled ,        IsmStateCompletingIdleIrpWithStatusCancelled },
        { IsmEventPDOCleanup ,          IsmStateWaitingForCancellationOnRemove },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryWaitingForCancellationOnRemove = {
    // State ETW Name
    IsmStateEtwWaitingForCancellationOnRemove,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventNewRequest ,          IsmStateCompleteWithStatusNoSuchDevice },
        { IsmEventWorkerRoutine ,       IsmStateIgnored },
        { IsmEventIrpCancelled ,        IsmStateCompletingRequestWithStatusDeviceGone },
        { IsmEventNull ,                IsmStateNull },
    }
};



ISM_STATE_ENTRY   IsmStateEntryWaitingForWorkerItem = {
    // State ETW Name
    IsmStateEtwWaitingForWorkerItem,
    // State Entry Function
    NULL,
    // Event State Pairs for Transitions
    {
        { IsmEventNewRequest ,          IsmStateCompleteWithStatusDeviceBusy },
        { IsmEventWorkerRoutine ,       IsmStateInvokingClientCallback },
        { IsmEventPDOD0 ,               IsmStateUnMarkingRequestCancellableOnD0 },
        { IsmEventPDOCleanup ,          IsmStateUnMarkingRequestCancellable },
        { IsmEventIrpCancelled ,        IsmStateCompletingIdleIrpWithStatusCancelled },
        { IsmEventPDOD3 ,               IsmStateUnMarkingRequestAsCancelable },
        { IsmEventNull ,                IsmStateNull },
    }
};






//
// Global List of States in all the State Machines
//
PISM_STATE_ENTRY    ISMStateTable[] = {
    &IsmStateEntryStartState,
    &IsmStateEntryCallbackCompleted,
    &IsmStateEntryCheckingIfSSIsEnabled,
    &IsmStateEntryCompletingIdleIrpWithStatusCancelled,
    &IsmStateEntryCompletingIdleIrpWithStatusCancelledInSSDisabled,
    &IsmStateEntryCompletingIdleIrpWithStatusCancelledReturnPending,
    &IsmStateEntryCompletingIdleIrpWithStatusInvalidDeviceState,
    &IsmStateEntryCompletingIdleIrpWithStatusSuccess,
    &IsmStateEntryCompletingRequestWithStatusDeviceGone,
    &IsmStateEntryIdleRequestPendingWithSSDisabled,
    &IsmStateEntryInvokingClientCallback,
    &IsmStateEntryMarkingRequestCancellable,
    &IsmStateEntryMarkingRequestCancellableInSSDisabled,
    &IsmStateEntryNoIdleRequest,
    &IsmStateEntryNoIdleRequestWithSelectiveSuspendDisabled,
    &IsmStateEntryPDORemoved,
    &IsmStateEntryQueuingWorkerItem,
    &IsmStateEntryStoringRequestInSSDisabled,
    &IsmStateEntryStoringRequestInSSEnabled,
    &IsmStateEntryUnMarkingRequestAsCancelable,
    &IsmStateEntryUnMarkingRequestCancellable,
    &IsmStateEntryUnMarkingRequestCancellableOnD0,
    &IsmStateEntryWaitingForCallbackComplete,
    &IsmStateEntryWaitingForCallbackOnCancelD0,
    &IsmStateEntryWaitingForCallbackOnCancellation,
    &IsmStateEntryWaitingForCallbackOnCancelledD3,
    &IsmStateEntryWaitingForCallbackOnD0,
    &IsmStateEntryWaitingForCallbackOnD3,
    &IsmStateEntryWaitingForCallbackOnRemove,
    &IsmStateEntryWaitingForCancellation,
    &IsmStateEntryWaitingForCancellationOnRemove,
    &IsmStateEntryWaitingForWorkerItem,
};
