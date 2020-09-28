
/*++
Copyright (c) Microsoft Corporation

Module Name:

ESM_AutoGen.h

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/

#pragma once



//
// Enum of States in all the State Machines
//
typedef enum _ESM_STATE {
    EsmStateIgnored                                  = 1000,
    EsmStateEmptySubState                            = 1001,
    EsmStateNull                                     = 1002,
    EsmStateCompleteWithStatusFailed                 = 1003,
    EsmStateRequestPortCycle                         = 1004,
    EsmStateSignalQueryDeviceTextEvent               = 1005,
    EsmStateCompleteFDORequestWithStatusFailed       = 1006,
    EsmStateCompleteGetPortStatusWithInternalError   = 1007,
    EsmStateCompleteGetPortStatusWithNoSuchDevice    = 1008,
    EsmStateDisabled                                 = 2000,
    EsmStateAcknowledgingControllerResetComplete     = 2001,
    EsmStateAcknowledgingControllerResetCompleteInDisabled = 2002,
    EsmStateCheckingIfControllerHasKnownIssuesInDeliveringFSE = 2003,
    EsmStateCheckingIfEndpointCanBeResetAfterHalt    = 2004,
    EsmStateCheckingIfEndpointShouldBeAutoReset      = 2005,
    EsmStateCheckingIfEndpointShouldBeReconfiguredAfterStop = 2006,
    EsmStateCheckingIfEndpointShouldBeReconfiguredOnResetWhenNotHalted = 2007,
    EsmStateCheckingIfEndpointShouldBeStopped        = 2008,
    EsmStateCheckingIfIgnoreContextErrorOnEPStopFlagIsSet = 2009,
    EsmStateCheckingIfStopEndpointCommandShouldBeDelayed = 2010,
    EsmStateCheckingIfStopEndpontReissueCountReached = 2011,
    EsmStateCompletingClientResetRequest             = 2012,
    EsmStateCompletingClientResetRequestAfterEndpointReconfigure = 2013,
    EsmStateCompletingClientResetRequestAfterEndpointReconfigureFailure = 2014,
    EsmStateCompletingClientResetRequestAfterResetFailure = 2015,
    EsmStateCompletingClientResetRequestDueToOutstandingTransfers = 2016,
    EsmStateCompletingResetRequestWhenEndpointNotHalted = 2017,
    EsmStateControllerResetInEnabled                 = 2018,
    EsmStateDroppingFSEAfterStop                     = 2019,
    EsmStateEnabledAndMapping                        = 2020,
    EsmStateNotifyingTransferRingsEndpointHalted     = 2021,
    EsmStateNotifyingTransferRingsEndpointResetFromClient = 2022,
    EsmStateNotifyingTransferRingsFSEReceived        = 2023,
    EsmStateNotifyingTransferRingsOkToReclaimTransfersOnCancel = 2024,
    EsmStateNotifyingUCXAboutTransferCancellation    = 2025,
    EsmStateProcessingExpectedEventTRBsAfterEndpointHalt = 2026,
    EsmStateProcessingExpectedEventTRBsAfterEndpointStop = 2027,
    EsmStatePurgingTransferRingQueuesIfNeeded        = 2028,
    EsmStateReclaimingCanceledTransfersAfterControllerSurpriseRemoval = 2029,
    EsmStateReclaimingTransfersAfterControlEndpointAutoReset = 2030,
    EsmStateReclaimingTransfersAfterControllerReset  = 2031,
    EsmStateReclaimingTransfersAfterEndpointStop     = 2032,
    EsmStateReconfiguringEndpointAfterStop           = 2033,
    EsmStateReconfiguringEndpointOnReset             = 2034,
    EsmStateRequeuingPendingTransfersAfterHaltClear  = 2035,
    EsmStateResettingControlEndpoint                 = 2036,
    EsmStateResettingControllerDueToRepeatedStopEndpointFailure = 2037,
    EsmStateResettingEndpoint                        = 2038,
    EsmStateRestartingTransferRingQueuesIfNeeded     = 2039,
    EsmStateSimulatingEndpointHaltReceivedOnTimeout  = 2040,
    EsmStateSimulatingExpectedEventTRBsProcessedAfterHaltOnTimeout = 2041,
    EsmStateSimulatingExpectedEventTRBsProcessedOnTimeout = 2042,
    EsmStateSimulatingFSEReceivedOnTimeout           = 2043,
    EsmStateStartingMapping                          = 2044,
    EsmStateStoppingEndpoint                         = 2045,
    EsmStateStoppingMappingAfterControllerSurpriseRemovalInEnabled = 2046,
    EsmStateStoppingMappingBeforeEndpointReconfigure = 2047,
    EsmStateStoppingMappingOnCancelStart             = 2048,
    EsmStateStoppingMappingOnHalt                    = 2049,
    EsmStateUpdatingTransferRingDequeuePointers      = 2050,
    EsmStateWaitingForControllerResetCompletion      = 2051,
    EsmStateWaitingForControllerResetOrSurpriseRemovalNotification = 2052,
    EsmStateWaitingForEndpointDisable                = 2053,
    EsmStateWaitingForFSEAfterStopEndpointCommandCompletion = 2054,
    EsmStateWaitingForHaltedCompletionCode           = 2055,
    EsmStateWaitingForResetRequestFromClient         = 2056,
    EsmStateWaitingToReissueStopEndpointCommand1     = 2057,
    EsmStateWaitingToReissueStopEndpointCommand2     = 2058,
} ESM_STATE, *PESM_STATE;





//
// Enum of ETW state names
//
typedef enum _ESM_STATE_ETW {
    EsmStateEtwDisabled                              = 2000,
    EsmStateEtwAcknowledgingControllerResetComplete  = 85,
    EsmStateEtwAcknowledgingControllerResetCompleteInDisabled = 120,
    EsmStateEtwCheckingIfControllerHasKnownIssuesInDeliveringFSE = 113,
    EsmStateEtwCheckingIfEndpointCanBeResetAfterHalt = 66,
    EsmStateEtwCheckingIfEndpointShouldBeAutoReset   = 5,
    EsmStateEtwCheckingIfEndpointShouldBeReconfiguredAfterStop = 71,
    EsmStateEtwCheckingIfEndpointShouldBeReconfiguredOnResetWhenNotHalted = 59,
    EsmStateEtwCheckingIfEndpointShouldBeStopped     = 4,
    EsmStateEtwCheckingIfIgnoreContextErrorOnEPStopFlagIsSet = 73,
    EsmStateEtwCheckingIfStopEndpointCommandShouldBeDelayed = 74,
    EsmStateEtwCheckingIfStopEndpontReissueCountReached = 75,
    EsmStateEtwCompletingClientResetRequest          = 6,
    EsmStateEtwCompletingClientResetRequestAfterEndpointReconfigure = 61,
    EsmStateEtwCompletingClientResetRequestAfterEndpointReconfigureFailure = 62,
    EsmStateEtwCompletingClientResetRequestAfterResetFailure = 63,
    EsmStateEtwCompletingClientResetRequestDueToOutstandingTransfers = 67,
    EsmStateEtwCompletingResetRequestWhenEndpointNotHalted = 60,
    EsmStateEtwControllerResetInEnabled              = 87,
    EsmStateEtwDroppingFSEAfterStop                  = 110,
    EsmStateEtwEnabledAndMapping                     = 64,
    EsmStateEtwNotifyingTransferRingsEndpointHalted  = 51,
    EsmStateEtwNotifyingTransferRingsEndpointResetFromClient = 55,
    EsmStateEtwNotifyingTransferRingsFSEReceived     = 111,
    EsmStateEtwNotifyingTransferRingsOkToReclaimTransfersOnCancel = 118,
    EsmStateEtwNotifyingUCXAboutTransferCancellation = 38,
    EsmStateEtwProcessingExpectedEventTRBsAfterEndpointHalt = 114,
    EsmStateEtwProcessingExpectedEventTRBsAfterEndpointStop = 115,
    EsmStateEtwPurgingTransferRingQueuesIfNeeded     = 69,
    EsmStateEtwReclaimingCanceledTransfersAfterControllerSurpriseRemoval = 119,
    EsmStateEtwReclaimingTransfersAfterControlEndpointAutoReset = 93,
    EsmStateEtwReclaimingTransfersAfterControllerReset = 94,
    EsmStateEtwReclaimingTransfersAfterEndpointStop  = 96,
    EsmStateEtwReconfiguringEndpointAfterStop        = 72,
    EsmStateEtwReconfiguringEndpointOnReset          = 56,
    EsmStateEtwRequeuingPendingTransfersAfterHaltClear = 99,
    EsmStateEtwResettingControlEndpoint              = 41,
    EsmStateEtwResettingControllerDueToRepeatedStopEndpointFailure = 76,
    EsmStateEtwResettingEndpoint                     = 42,
    EsmStateEtwRestartingTransferRingQueuesIfNeeded  = 70,
    EsmStateEtwSimulatingEndpointHaltReceivedOnTimeout = 107,
    EsmStateEtwSimulatingExpectedEventTRBsProcessedAfterHaltOnTimeout = 116,
    EsmStateEtwSimulatingExpectedEventTRBsProcessedOnTimeout = 117,
    EsmStateEtwSimulatingFSEReceivedOnTimeout        = 106,
    EsmStateEtwStartingMapping                       = 65,
    EsmStateEtwStoppingEndpoint                      = 43,
    EsmStateEtwStoppingMappingAfterControllerSurpriseRemovalInEnabled = 97,
    EsmStateEtwStoppingMappingBeforeEndpointReconfigure = 58,
    EsmStateEtwStoppingMappingOnCancelStart          = 44,
    EsmStateEtwStoppingMappingOnHalt                 = 45,
    EsmStateEtwUpdatingTransferRingDequeuePointers   = 46,
    EsmStateEtwWaitingForControllerResetCompletion   = 90,
    EsmStateEtwWaitingForControllerResetOrSurpriseRemovalNotification = 112,
    EsmStateEtwWaitingForEndpointDisable             = 91,
    EsmStateEtwWaitingForFSEAfterStopEndpointCommandCompletion = 18,
    EsmStateEtwWaitingForHaltedCompletionCode        = 10,
    EsmStateEtwWaitingForResetRequestFromClient      = 12,
    EsmStateEtwWaitingToReissueStopEndpointCommand1  = 77,
    EsmStateEtwWaitingToReissueStopEndpointCommand2  = 78,
} ESM_STATE_ETW, *PESM_STATE_ETW;



//
// Definitions for State Entry Functions 
//
STATE_ENTRY_FUNCTION          ESM_AcknowledgingControllerResetComplete;

STATE_ENTRY_FUNCTION          ESM_AcknowledgingControllerResetCompleteInDisabled;

STATE_ENTRY_FUNCTION          ESM_CheckingIfControllerHasKnownIssuesInDeliveringFSE;

STATE_ENTRY_FUNCTION          ESM_CheckingIfEndpointCanBeResetAfterHalt;

STATE_ENTRY_FUNCTION          ESM_CheckingIfEndpointShouldBeAutoReset;

STATE_ENTRY_FUNCTION          ESM_CheckingIfEndpointShouldBeReconfiguredAfterStop;

STATE_ENTRY_FUNCTION          ESM_CheckingIfEndpointShouldBeReconfiguredOnResetWhenNotHalted;

STATE_ENTRY_FUNCTION          ESM_CheckingIfEndpointShouldBeStopped;

STATE_ENTRY_FUNCTION          ESM_CheckingIfIgnoreContextErrorOnEPStopFlagIsSet;

STATE_ENTRY_FUNCTION          ESM_CheckingIfStopEndpointCommandShouldBeDelayed;

STATE_ENTRY_FUNCTION          ESM_CheckingIfStopEndpontReissueCountReached;

STATE_ENTRY_FUNCTION          ESM_CompletingClientResetRequest;

STATE_ENTRY_FUNCTION          ESM_CompletingClientResetRequestAfterEndpointReconfigure;

STATE_ENTRY_FUNCTION          ESM_CompletingClientResetRequestAfterEndpointReconfigureFailure;

STATE_ENTRY_FUNCTION          ESM_CompletingClientResetRequestAfterResetFailure;

STATE_ENTRY_FUNCTION          ESM_CompletingClientResetRequestDueToOutstandingTransfers;

STATE_ENTRY_FUNCTION          ESM_CompletingResetRequestWhenEndpointNotHalted;

STATE_ENTRY_FUNCTION          ESM_ControllerResetInEnabled;

STATE_ENTRY_FUNCTION          ESM_Disabled;

STATE_ENTRY_FUNCTION          ESM_DroppingFSEAfterStop;

STATE_ENTRY_FUNCTION          ESM_EnabledAndMapping;

STATE_ENTRY_FUNCTION          ESM_NotifyingTransferRingsEndpointHalted;

STATE_ENTRY_FUNCTION          ESM_NotifyingTransferRingsEndpointResetFromClient;

STATE_ENTRY_FUNCTION          ESM_NotifyingTransferRingsFSEReceived;

STATE_ENTRY_FUNCTION          ESM_NotifyingTransferRingsOkToReclaimTransfersOnCancel;

STATE_ENTRY_FUNCTION          ESM_NotifyingUCXAboutTransferCancellation;

STATE_ENTRY_FUNCTION          ESM_ProcessingExpectedEventTRBsAfterEndpointHalt;

STATE_ENTRY_FUNCTION          ESM_ProcessingExpectedEventTRBsAfterEndpointStop;

STATE_ENTRY_FUNCTION          ESM_PurgingTransferRingQueuesIfNeeded;

STATE_ENTRY_FUNCTION          ESM_ReclaimingCanceledTransfersAfterControllerSurpriseRemoval;

STATE_ENTRY_FUNCTION          ESM_ReclaimingTransfersAfterControlEndpointAutoReset;

STATE_ENTRY_FUNCTION          ESM_ReclaimingTransfersAfterControllerReset;

STATE_ENTRY_FUNCTION          ESM_ReclaimingTransfersAfterEndpointStop;

STATE_ENTRY_FUNCTION          ESM_ReconfiguringEndpointAfterStop;

STATE_ENTRY_FUNCTION          ESM_ReconfiguringEndpointOnReset;

STATE_ENTRY_FUNCTION          ESM_RequeuingPendingTransfersAfterHaltClear;

STATE_ENTRY_FUNCTION          ESM_ResettingControlEndpoint;

STATE_ENTRY_FUNCTION          ESM_ResettingControllerDueToRepeatedStopEndpointFailure;

STATE_ENTRY_FUNCTION          ESM_ResettingEndpoint;

STATE_ENTRY_FUNCTION          ESM_RestartingTransferRingQueuesIfNeeded;

STATE_ENTRY_FUNCTION          ESM_SimulatingEndpointHaltReceivedOnTimeout;

STATE_ENTRY_FUNCTION          ESM_SimulatingExpectedEventTRBsProcessedAfterHaltOnTimeout;

STATE_ENTRY_FUNCTION          ESM_SimulatingExpectedEventTRBsProcessedOnTimeout;

STATE_ENTRY_FUNCTION          ESM_SimulatingFSEReceivedOnTimeout;

STATE_ENTRY_FUNCTION          ESM_StartingMapping;

STATE_ENTRY_FUNCTION          ESM_StoppingEndpoint;

STATE_ENTRY_FUNCTION          ESM_StoppingMappingAfterControllerSurpriseRemovalInEnabled;

STATE_ENTRY_FUNCTION          ESM_StoppingMappingBeforeEndpointReconfigure;

STATE_ENTRY_FUNCTION          ESM_StoppingMappingOnCancelStart;

STATE_ENTRY_FUNCTION          ESM_StoppingMappingOnHalt;

STATE_ENTRY_FUNCTION          ESM_UpdatingTransferRingDequeuePointers;

STATE_ENTRY_FUNCTION          ESM_WaitingForControllerResetCompletion;

STATE_ENTRY_FUNCTION          ESM_WaitingForControllerResetOrSurpriseRemovalNotification;

STATE_ENTRY_FUNCTION          ESM_WaitingForEndpointDisable;

STATE_ENTRY_FUNCTION          ESM_WaitingForFSEAfterStopEndpointCommandCompletion;

STATE_ENTRY_FUNCTION          ESM_WaitingForHaltedCompletionCode;

STATE_ENTRY_FUNCTION          ESM_WaitingForResetRequestFromClient;

STATE_ENTRY_FUNCTION          ESM_WaitingToReissueStopEndpointCommand1;

STATE_ENTRY_FUNCTION          ESM_WaitingToReissueStopEndpointCommand2;




//
// Sub State Entries for the states in the State Machine
//


//
// State Entries for the states in the State Machine
//
STATE_ENTRY   EsmStateEntryAcknowledgingControllerResetComplete = {
    // State ETW Name
    EsmStateEtwAcknowledgingControllerResetComplete,
    // State Entry Function
    ESM_AcknowledgingControllerResetComplete,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateWaitingForEndpointDisable },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryAcknowledgingControllerResetCompleteInDisabled = {
    // State ETW Name
    EsmStateEtwAcknowledgingControllerResetCompleteInDisabled,
    // State Entry Function
    ESM_AcknowledgingControllerResetCompleteInDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateDisabled },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCheckingIfControllerHasKnownIssuesInDeliveringFSE = {
    // State ETW Name
    EsmStateEtwCheckingIfControllerHasKnownIssuesInDeliveringFSE,
    // State Entry Function
    ESM_CheckingIfControllerHasKnownIssuesInDeliveringFSE,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventYes ,                 EsmStateDroppingFSEAfterStop },
        { EsmEventNo ,                  EsmStateWaitingForFSEAfterStopEndpointCommandCompletion },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCheckingIfEndpointCanBeResetAfterHalt = {
    // State ETW Name
    EsmStateEtwCheckingIfEndpointCanBeResetAfterHalt,
    // State Entry Function
    ESM_CheckingIfEndpointCanBeResetAfterHalt,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventYes ,                 EsmStateResettingEndpoint },
        { EsmEventNo ,                  EsmStateCompletingClientResetRequestDueToOutstandingTransfers },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCheckingIfEndpointShouldBeAutoReset = {
    // State ETW Name
    EsmStateEtwCheckingIfEndpointShouldBeAutoReset,
    // State Entry Function
    ESM_CheckingIfEndpointShouldBeAutoReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventYes ,                 EsmStateResettingControlEndpoint },
        { EsmEventNo ,                  EsmStateProcessingExpectedEventTRBsAfterEndpointHalt },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCheckingIfEndpointShouldBeReconfiguredAfterStop = {
    // State ETW Name
    EsmStateEtwCheckingIfEndpointShouldBeReconfiguredAfterStop,
    // State Entry Function
    ESM_CheckingIfEndpointShouldBeReconfiguredAfterStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventYes ,                 EsmStateReconfiguringEndpointAfterStop },
        { EsmEventNo ,                  EsmStateUpdatingTransferRingDequeuePointers },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCheckingIfEndpointShouldBeReconfiguredOnResetWhenNotHalted = {
    // State ETW Name
    EsmStateEtwCheckingIfEndpointShouldBeReconfiguredOnResetWhenNotHalted,
    // State Entry Function
    ESM_CheckingIfEndpointShouldBeReconfiguredOnResetWhenNotHalted,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventYes ,                 EsmStateStoppingMappingBeforeEndpointReconfigure },
        { EsmEventNo ,                  EsmStateCompletingResetRequestWhenEndpointNotHalted },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCheckingIfEndpointShouldBeStopped = {
    // State ETW Name
    EsmStateEtwCheckingIfEndpointShouldBeStopped,
    // State Entry Function
    ESM_CheckingIfEndpointShouldBeStopped,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventYes ,                 EsmStateCheckingIfStopEndpointCommandShouldBeDelayed },
        { EsmEventNo ,                  EsmStateReclaimingTransfersAfterEndpointStop },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCheckingIfIgnoreContextErrorOnEPStopFlagIsSet = {
    // State ETW Name
    EsmStateEtwCheckingIfIgnoreContextErrorOnEPStopFlagIsSet,
    // State Entry Function
    ESM_CheckingIfIgnoreContextErrorOnEPStopFlagIsSet,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventYes ,                 EsmStateCheckingIfControllerHasKnownIssuesInDeliveringFSE },
        { EsmEventNo ,                  EsmStateResettingControllerDueToRepeatedStopEndpointFailure },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCheckingIfStopEndpointCommandShouldBeDelayed = {
    // State ETW Name
    EsmStateEtwCheckingIfStopEndpointCommandShouldBeDelayed,
    // State Entry Function
    ESM_CheckingIfStopEndpointCommandShouldBeDelayed,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventYes ,                 EsmStateWaitingToReissueStopEndpointCommand2 },
        { EsmEventNo ,                  EsmStateStoppingEndpoint },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCheckingIfStopEndpontReissueCountReached = {
    // State ETW Name
    EsmStateEtwCheckingIfStopEndpontReissueCountReached,
    // State Entry Function
    ESM_CheckingIfStopEndpontReissueCountReached,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventYes ,                 EsmStateCheckingIfIgnoreContextErrorOnEPStopFlagIsSet },
        { EsmEventNo ,                  EsmStateStoppingEndpoint },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCompletingClientResetRequest = {
    // State ETW Name
    EsmStateEtwCompletingClientResetRequest,
    // State Entry Function
    ESM_CompletingClientResetRequest,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateStartingMapping },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCompletingClientResetRequestAfterEndpointReconfigure = {
    // State ETW Name
    EsmStateEtwCompletingClientResetRequestAfterEndpointReconfigure,
    // State Entry Function
    ESM_CompletingClientResetRequestAfterEndpointReconfigure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateStartingMapping },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCompletingClientResetRequestAfterEndpointReconfigureFailure = {
    // State ETW Name
    EsmStateEtwCompletingClientResetRequestAfterEndpointReconfigureFailure,
    // State Entry Function
    ESM_CompletingClientResetRequestAfterEndpointReconfigureFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateWaitingForControllerResetOrSurpriseRemovalNotification },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCompletingClientResetRequestAfterResetFailure = {
    // State ETW Name
    EsmStateEtwCompletingClientResetRequestAfterResetFailure,
    // State Entry Function
    ESM_CompletingClientResetRequestAfterResetFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateWaitingForControllerResetOrSurpriseRemovalNotification },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCompletingClientResetRequestDueToOutstandingTransfers = {
    // State ETW Name
    EsmStateEtwCompletingClientResetRequestDueToOutstandingTransfers,
    // State Entry Function
    ESM_CompletingClientResetRequestDueToOutstandingTransfers,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateWaitingForResetRequestFromClient },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryCompletingResetRequestWhenEndpointNotHalted = {
    // State ETW Name
    EsmStateEtwCompletingResetRequestWhenEndpointNotHalted,
    // State Entry Function
    ESM_CompletingResetRequestWhenEndpointNotHalted,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateEnabledAndMapping },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryControllerResetInEnabled = {
    // State ETW Name
    EsmStateEtwControllerResetInEnabled,
    // State Entry Function
    ESM_ControllerResetInEnabled,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventControllerResetComplete ,EsmStateAcknowledgingControllerResetComplete },
        { EsmEventDisabled ,            EsmStateDisabled },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryDisabled = {
    // State ETW Name
    EsmStateEtwDisabled,
    // State Entry Function
    ESM_Disabled,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventControllerResetStart ,EsmStateIgnored },
        { EsmEventControllerSurpriseRemoved ,EsmStateIgnored },
        { EsmEventDisabled ,            EsmStateIgnored },
        { EsmEventControllerResetComplete ,EsmStateAcknowledgingControllerResetCompleteInDisabled },
        { EsmEventEnabled ,             EsmStateStartingMapping },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryDroppingFSEAfterStop = {
    // State ETW Name
    EsmStateEtwDroppingFSEAfterStop,
    // State Entry Function
    ESM_DroppingFSEAfterStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateCheckingIfEndpointShouldBeReconfiguredAfterStop },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryEnabledAndMapping = {
    // State ETW Name
    EsmStateEtwEnabledAndMapping,
    // State Entry Function
    ESM_EnabledAndMapping,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventControllerResetStart ,EsmStateControllerResetInEnabled },
        { EsmEventStreamsEnabled ,      EsmStateStartingMapping },
        { EsmEventTransferCanceled ,    EsmStateStoppingMappingOnCancelStart },
        { EsmEventControllerSurpriseRemoved ,EsmStateStoppingMappingAfterControllerSurpriseRemovalInEnabled },
        { EsmEventControllerResetComplete ,EsmStateAcknowledgingControllerResetComplete },
        { EsmEventDisabled ,            EsmStateDisabled },
        { EsmEventStreamsDisabled ,     EsmStateStartingMapping },
        { EsmEventHaltedCompletionCodeReceived ,EsmStateStoppingMappingOnHalt },
        { EsmEventResetRequestFromClient ,EsmStateNotifyingTransferRingsEndpointResetFromClient },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryNotifyingTransferRingsEndpointHalted = {
    // State ETW Name
    EsmStateEtwNotifyingTransferRingsEndpointHalted,
    // State Entry Function
    ESM_NotifyingTransferRingsEndpointHalted,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateCheckingIfEndpointShouldBeAutoReset },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryNotifyingTransferRingsEndpointResetFromClient = {
    // State ETW Name
    EsmStateEtwNotifyingTransferRingsEndpointResetFromClient,
    // State Entry Function
    ESM_NotifyingTransferRingsEndpointResetFromClient,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateCheckingIfEndpointShouldBeReconfiguredOnResetWhenNotHalted },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryNotifyingTransferRingsFSEReceived = {
    // State ETW Name
    EsmStateEtwNotifyingTransferRingsFSEReceived,
    // State Entry Function
    ESM_NotifyingTransferRingsFSEReceived,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateProcessingExpectedEventTRBsAfterEndpointStop },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryNotifyingTransferRingsOkToReclaimTransfersOnCancel = {
    // State ETW Name
    EsmStateEtwNotifyingTransferRingsOkToReclaimTransfersOnCancel,
    // State Entry Function
    ESM_NotifyingTransferRingsOkToReclaimTransfersOnCancel,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateWaitingForResetRequestFromClient },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryNotifyingUCXAboutTransferCancellation = {
    // State ETW Name
    EsmStateEtwNotifyingUCXAboutTransferCancellation,
    // State Entry Function
    ESM_NotifyingUCXAboutTransferCancellation,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOkToCancelTransfers , EsmStateReclaimingTransfersAfterEndpointStop },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryProcessingExpectedEventTRBsAfterEndpointHalt = {
    // State ETW Name
    EsmStateEtwProcessingExpectedEventTRBsAfterEndpointHalt,
    // State Entry Function
    ESM_ProcessingExpectedEventTRBsAfterEndpointHalt,
    // State Flags
    StateFlagStopTimerOnExit,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventTransferCanceled ,    EsmStateIgnored },
        { EsmEventTimerFired ,          EsmStateSimulatingExpectedEventTRBsProcessedAfterHaltOnTimeout },
        { EsmEventExpectedEventTRBsProcessed ,EsmStatePurgingTransferRingQueuesIfNeeded },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryProcessingExpectedEventTRBsAfterEndpointStop = {
    // State ETW Name
    EsmStateEtwProcessingExpectedEventTRBsAfterEndpointStop,
    // State Entry Function
    ESM_ProcessingExpectedEventTRBsAfterEndpointStop,
    // State Flags
    StateFlagStopTimerOnExit,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventExpectedEventTRBsProcessed ,EsmStateCheckingIfEndpointShouldBeReconfiguredAfterStop },
        { EsmEventTimerFired ,          EsmStateSimulatingExpectedEventTRBsProcessedOnTimeout },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryPurgingTransferRingQueuesIfNeeded = {
    // State ETW Name
    EsmStateEtwPurgingTransferRingQueuesIfNeeded,
    // State Entry Function
    ESM_PurgingTransferRingQueuesIfNeeded,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateNotifyingTransferRingsOkToReclaimTransfersOnCancel },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryReclaimingCanceledTransfersAfterControllerSurpriseRemoval = {
    // State ETW Name
    EsmStateEtwReclaimingCanceledTransfersAfterControllerSurpriseRemoval,
    // State Entry Function
    ESM_ReclaimingCanceledTransfersAfterControllerSurpriseRemoval,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventTransferCanceled ,    EsmStateIgnored },
        { EsmEventHaltedCompletionCodeReceived ,EsmStateIgnored },
        { EsmEventResetRequestFromClient ,EsmStateIgnored },
        { EsmEventDisabled ,            EsmStateDisabled },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryReclaimingTransfersAfterControlEndpointAutoReset = {
    // State ETW Name
    EsmStateEtwReclaimingTransfersAfterControlEndpointAutoReset,
    // State Entry Function
    ESM_ReclaimingTransfersAfterControlEndpointAutoReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventTransferCanceled ,    EsmStateIgnored },
        { EsmEventTransfersReclaimed ,  EsmStateStartingMapping },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryReclaimingTransfersAfterControllerReset = {
    // State ETW Name
    EsmStateEtwReclaimingTransfersAfterControllerReset,
    // State Entry Function
    ESM_ReclaimingTransfersAfterControllerReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventTransferCanceled ,    EsmStateIgnored },
        { EsmEventTransfersReclaimed ,  EsmStateAcknowledgingControllerResetComplete },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryReclaimingTransfersAfterEndpointStop = {
    // State ETW Name
    EsmStateEtwReclaimingTransfersAfterEndpointStop,
    // State Entry Function
    ESM_ReclaimingTransfersAfterEndpointStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventTransfersReclaimed ,  EsmStateStartingMapping },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryReconfiguringEndpointAfterStop = {
    // State ETW Name
    EsmStateEtwReconfiguringEndpointAfterStop,
    // State Entry Function
    ESM_ReconfiguringEndpointAfterStop,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventCommandCompletionFailure ,EsmStateWaitingForControllerResetOrSurpriseRemovalNotification },
        { EsmEventCommandCompletionSuccess ,EsmStateUpdatingTransferRingDequeuePointers },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryReconfiguringEndpointOnReset = {
    // State ETW Name
    EsmStateEtwReconfiguringEndpointOnReset,
    // State Entry Function
    ESM_ReconfiguringEndpointOnReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventCommandCompletionFailure ,EsmStateCompletingClientResetRequestAfterEndpointReconfigureFailure },
        { EsmEventCommandCompletionSuccess ,EsmStateCompletingClientResetRequestAfterEndpointReconfigure },
        { EsmEventCommonBufferAllocationFailure ,EsmStateCompletingClientResetRequestAfterEndpointReconfigure },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryRequeuingPendingTransfersAfterHaltClear = {
    // State ETW Name
    EsmStateEtwRequeuingPendingTransfersAfterHaltClear,
    // State Entry Function
    ESM_RequeuingPendingTransfersAfterHaltClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventTransfersReclaimed ,  EsmStateRestartingTransferRingQueuesIfNeeded },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryResettingControlEndpoint = {
    // State ETW Name
    EsmStateEtwResettingControlEndpoint,
    // State Entry Function
    ESM_ResettingControlEndpoint,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventTransferCanceled ,    EsmStateIgnored },
        { EsmEventCommandCompletionFailure ,EsmStateWaitingForControllerResetOrSurpriseRemovalNotification },
        { EsmEventCommandCompletionSuccess ,EsmStateReclaimingTransfersAfterControlEndpointAutoReset },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryResettingControllerDueToRepeatedStopEndpointFailure = {
    // State ETW Name
    EsmStateEtwResettingControllerDueToRepeatedStopEndpointFailure,
    // State Entry Function
    ESM_ResettingControllerDueToRepeatedStopEndpointFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateWaitingForControllerResetOrSurpriseRemovalNotification },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryResettingEndpoint = {
    // State ETW Name
    EsmStateEtwResettingEndpoint,
    // State Entry Function
    ESM_ResettingEndpoint,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventCommandCompletionFailure ,EsmStateCompletingClientResetRequestAfterResetFailure },
        { EsmEventCommandCompletionSuccess ,EsmStateRequeuingPendingTransfersAfterHaltClear },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryRestartingTransferRingQueuesIfNeeded = {
    // State ETW Name
    EsmStateEtwRestartingTransferRingQueuesIfNeeded,
    // State Entry Function
    ESM_RestartingTransferRingQueuesIfNeeded,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateCompletingClientResetRequest },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntrySimulatingEndpointHaltReceivedOnTimeout = {
    // State ETW Name
    EsmStateEtwSimulatingEndpointHaltReceivedOnTimeout,
    // State Entry Function
    ESM_SimulatingEndpointHaltReceivedOnTimeout,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventHaltedCompletionCodeReceived ,EsmStateNotifyingTransferRingsEndpointHalted },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntrySimulatingExpectedEventTRBsProcessedAfterHaltOnTimeout = {
    // State ETW Name
    EsmStateEtwSimulatingExpectedEventTRBsProcessedAfterHaltOnTimeout,
    // State Entry Function
    ESM_SimulatingExpectedEventTRBsProcessedAfterHaltOnTimeout,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventExpectedEventTRBsProcessed ,EsmStatePurgingTransferRingQueuesIfNeeded },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntrySimulatingExpectedEventTRBsProcessedOnTimeout = {
    // State ETW Name
    EsmStateEtwSimulatingExpectedEventTRBsProcessedOnTimeout,
    // State Entry Function
    ESM_SimulatingExpectedEventTRBsProcessedOnTimeout,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventExpectedEventTRBsProcessed ,EsmStateCheckingIfEndpointShouldBeReconfiguredAfterStop },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntrySimulatingFSEReceivedOnTimeout = {
    // State ETW Name
    EsmStateEtwSimulatingFSEReceivedOnTimeout,
    // State Entry Function
    ESM_SimulatingFSEReceivedOnTimeout,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventFSEReceived ,         EsmStateNotifyingTransferRingsFSEReceived },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryStartingMapping = {
    // State ETW Name
    EsmStateEtwStartingMapping,
    // State Entry Function
    ESM_StartingMapping,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventOperationSuccess ,    EsmStateEnabledAndMapping },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryStoppingEndpoint = {
    // State ETW Name
    EsmStateEtwStoppingEndpoint,
    // State Entry Function
    ESM_StoppingEndpoint,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventCommandCompletionFailure ,EsmStateWaitingForControllerResetOrSurpriseRemovalNotification },
        { EsmEventStopEndpointFailureWithEndpointHalted ,EsmStateWaitingForHaltedCompletionCode },
        { EsmEventStopEndpointFailureWithEndpointRunning ,EsmStateWaitingToReissueStopEndpointCommand2 },
        { EsmEventCommandCompletionSuccess ,EsmStateCheckingIfControllerHasKnownIssuesInDeliveringFSE },
        { EsmEventStopEndpointFailureWithEndpointStopped ,EsmStateWaitingToReissueStopEndpointCommand1 },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryStoppingMappingAfterControllerSurpriseRemovalInEnabled = {
    // State ETW Name
    EsmStateEtwStoppingMappingAfterControllerSurpriseRemovalInEnabled,
    // State Entry Function
    ESM_StoppingMappingAfterControllerSurpriseRemovalInEnabled,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventMappingStopped ,      EsmStateReclaimingCanceledTransfersAfterControllerSurpriseRemoval },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryStoppingMappingBeforeEndpointReconfigure = {
    // State ETW Name
    EsmStateEtwStoppingMappingBeforeEndpointReconfigure,
    // State Entry Function
    ESM_StoppingMappingBeforeEndpointReconfigure,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventMappingStopped ,      EsmStateReconfiguringEndpointOnReset },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryStoppingMappingOnCancelStart = {
    // State ETW Name
    EsmStateEtwStoppingMappingOnCancelStart,
    // State Entry Function
    ESM_StoppingMappingOnCancelStart,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventMappingStopped ,      EsmStateCheckingIfEndpointShouldBeStopped },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryStoppingMappingOnHalt = {
    // State ETW Name
    EsmStateEtwStoppingMappingOnHalt,
    // State Entry Function
    ESM_StoppingMappingOnHalt,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventTransferCanceled ,    EsmStateIgnored },
        { EsmEventMappingStopped ,      EsmStateNotifyingTransferRingsEndpointHalted },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryUpdatingTransferRingDequeuePointers = {
    // State ETW Name
    EsmStateEtwUpdatingTransferRingDequeuePointers,
    // State Entry Function
    ESM_UpdatingTransferRingDequeuePointers,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventCommandCompletionSuccess ,EsmStateNotifyingUCXAboutTransferCancellation },
        { EsmEventCommandCompletionFailure ,EsmStateWaitingForControllerResetOrSurpriseRemovalNotification },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryWaitingForControllerResetCompletion = {
    // State ETW Name
    EsmStateEtwWaitingForControllerResetCompletion,
    // State Entry Function
    ESM_WaitingForControllerResetCompletion,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventResetRequestFromClient ,EsmStateIgnored },
        { EsmEventHaltedCompletionCodeReceived ,EsmStateIgnored },
        { EsmEventTransferCanceled ,    EsmStateIgnored },
        { EsmEventDisabled ,            EsmStateDisabled },
        { EsmEventControllerResetComplete ,EsmStateReclaimingTransfersAfterControllerReset },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryWaitingForControllerResetOrSurpriseRemovalNotification = {
    // State ETW Name
    EsmStateEtwWaitingForControllerResetOrSurpriseRemovalNotification,
    // State Entry Function
    ESM_WaitingForControllerResetOrSurpriseRemovalNotification,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventHaltedCompletionCodeReceived ,EsmStateIgnored },
        { EsmEventTransferCanceled ,    EsmStateIgnored },
        { EsmEventDisabled ,            EsmStateDisabled },
        { EsmEventControllerResetStart ,EsmStateWaitingForControllerResetCompletion },
        { EsmEventControllerSurpriseRemoved ,EsmStateReclaimingCanceledTransfersAfterControllerSurpriseRemoval },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryWaitingForEndpointDisable = {
    // State ETW Name
    EsmStateEtwWaitingForEndpointDisable,
    // State Entry Function
    ESM_WaitingForEndpointDisable,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventControllerSurpriseRemoved ,EsmStateIgnored },
        { EsmEventDisabled ,            EsmStateDisabled },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryWaitingForFSEAfterStopEndpointCommandCompletion = {
    // State ETW Name
    EsmStateEtwWaitingForFSEAfterStopEndpointCommandCompletion,
    // State Entry Function
    ESM_WaitingForFSEAfterStopEndpointCommandCompletion,
    // State Flags
    StateFlagStopTimerOnExit,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventFSEReceived ,         EsmStateNotifyingTransferRingsFSEReceived },
        { EsmEventTimerFired ,          EsmStateSimulatingFSEReceivedOnTimeout },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryWaitingForHaltedCompletionCode = {
    // State ETW Name
    EsmStateEtwWaitingForHaltedCompletionCode,
    // State Entry Function
    ESM_WaitingForHaltedCompletionCode,
    // State Flags
    StateFlagStopTimerOnExit,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventTimerFired ,          EsmStateSimulatingEndpointHaltReceivedOnTimeout },
        { EsmEventHaltedCompletionCodeReceived ,EsmStateNotifyingTransferRingsEndpointHalted },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryWaitingForResetRequestFromClient = {
    // State ETW Name
    EsmStateEtwWaitingForResetRequestFromClient,
    // State Entry Function
    ESM_WaitingForResetRequestFromClient,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventTransferCanceled ,    EsmStateIgnored },
        { EsmEventControllerResetStart ,EsmStateWaitingForControllerResetCompletion },
        { EsmEventDisabled ,            EsmStateDisabled },
        { EsmEventResetRequestFromClient ,EsmStateCheckingIfEndpointCanBeResetAfterHalt },
        { EsmEventControllerSurpriseRemoved ,EsmStateReclaimingCanceledTransfersAfterControllerSurpriseRemoval },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryWaitingToReissueStopEndpointCommand1 = {
    // State ETW Name
    EsmStateEtwWaitingToReissueStopEndpointCommand1,
    // State Entry Function
    ESM_WaitingToReissueStopEndpointCommand1,
    // State Flags
    StateFlagStopTimerOnExit,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventTimerFired ,          EsmStateCheckingIfStopEndpontReissueCountReached },
        { EsmEventHaltedCompletionCodeReceived ,EsmStateNotifyingTransferRingsEndpointHalted },
        { EsmEventNull ,                EsmStateNull },
    }
};



STATE_ENTRY   EsmStateEntryWaitingToReissueStopEndpointCommand2 = {
    // State ETW Name
    EsmStateEtwWaitingToReissueStopEndpointCommand2,
    // State Entry Function
    ESM_WaitingToReissueStopEndpointCommand2,
    // State Flags
    StateFlagStopTimerOnExit,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { EsmEventTimerFired ,          EsmStateCheckingIfStopEndpontReissueCountReached },
        { EsmEventHaltedCompletionCodeReceived ,EsmStateNotifyingTransferRingsEndpointHalted },
        { EsmEventNull ,                EsmStateNull },
    }
};






//
// Global List of States in all the State Machines
//
PSTATE_ENTRY    ESMStateTable[] = {
    &EsmStateEntryDisabled,
    &EsmStateEntryAcknowledgingControllerResetComplete,
    &EsmStateEntryAcknowledgingControllerResetCompleteInDisabled,
    &EsmStateEntryCheckingIfControllerHasKnownIssuesInDeliveringFSE,
    &EsmStateEntryCheckingIfEndpointCanBeResetAfterHalt,
    &EsmStateEntryCheckingIfEndpointShouldBeAutoReset,
    &EsmStateEntryCheckingIfEndpointShouldBeReconfiguredAfterStop,
    &EsmStateEntryCheckingIfEndpointShouldBeReconfiguredOnResetWhenNotHalted,
    &EsmStateEntryCheckingIfEndpointShouldBeStopped,
    &EsmStateEntryCheckingIfIgnoreContextErrorOnEPStopFlagIsSet,
    &EsmStateEntryCheckingIfStopEndpointCommandShouldBeDelayed,
    &EsmStateEntryCheckingIfStopEndpontReissueCountReached,
    &EsmStateEntryCompletingClientResetRequest,
    &EsmStateEntryCompletingClientResetRequestAfterEndpointReconfigure,
    &EsmStateEntryCompletingClientResetRequestAfterEndpointReconfigureFailure,
    &EsmStateEntryCompletingClientResetRequestAfterResetFailure,
    &EsmStateEntryCompletingClientResetRequestDueToOutstandingTransfers,
    &EsmStateEntryCompletingResetRequestWhenEndpointNotHalted,
    &EsmStateEntryControllerResetInEnabled,
    &EsmStateEntryDroppingFSEAfterStop,
    &EsmStateEntryEnabledAndMapping,
    &EsmStateEntryNotifyingTransferRingsEndpointHalted,
    &EsmStateEntryNotifyingTransferRingsEndpointResetFromClient,
    &EsmStateEntryNotifyingTransferRingsFSEReceived,
    &EsmStateEntryNotifyingTransferRingsOkToReclaimTransfersOnCancel,
    &EsmStateEntryNotifyingUCXAboutTransferCancellation,
    &EsmStateEntryProcessingExpectedEventTRBsAfterEndpointHalt,
    &EsmStateEntryProcessingExpectedEventTRBsAfterEndpointStop,
    &EsmStateEntryPurgingTransferRingQueuesIfNeeded,
    &EsmStateEntryReclaimingCanceledTransfersAfterControllerSurpriseRemoval,
    &EsmStateEntryReclaimingTransfersAfterControlEndpointAutoReset,
    &EsmStateEntryReclaimingTransfersAfterControllerReset,
    &EsmStateEntryReclaimingTransfersAfterEndpointStop,
    &EsmStateEntryReconfiguringEndpointAfterStop,
    &EsmStateEntryReconfiguringEndpointOnReset,
    &EsmStateEntryRequeuingPendingTransfersAfterHaltClear,
    &EsmStateEntryResettingControlEndpoint,
    &EsmStateEntryResettingControllerDueToRepeatedStopEndpointFailure,
    &EsmStateEntryResettingEndpoint,
    &EsmStateEntryRestartingTransferRingQueuesIfNeeded,
    &EsmStateEntrySimulatingEndpointHaltReceivedOnTimeout,
    &EsmStateEntrySimulatingExpectedEventTRBsProcessedAfterHaltOnTimeout,
    &EsmStateEntrySimulatingExpectedEventTRBsProcessedOnTimeout,
    &EsmStateEntrySimulatingFSEReceivedOnTimeout,
    &EsmStateEntryStartingMapping,
    &EsmStateEntryStoppingEndpoint,
    &EsmStateEntryStoppingMappingAfterControllerSurpriseRemovalInEnabled,
    &EsmStateEntryStoppingMappingBeforeEndpointReconfigure,
    &EsmStateEntryStoppingMappingOnCancelStart,
    &EsmStateEntryStoppingMappingOnHalt,
    &EsmStateEntryUpdatingTransferRingDequeuePointers,
    &EsmStateEntryWaitingForControllerResetCompletion,
    &EsmStateEntryWaitingForControllerResetOrSurpriseRemovalNotification,
    &EsmStateEntryWaitingForEndpointDisable,
    &EsmStateEntryWaitingForFSEAfterStopEndpointCommandCompletion,
    &EsmStateEntryWaitingForHaltedCompletionCode,
    &EsmStateEntryWaitingForResetRequestFromClient,
    &EsmStateEntryWaitingToReissueStopEndpointCommand1,
    &EsmStateEntryWaitingToReissueStopEndpointCommand2,
};
