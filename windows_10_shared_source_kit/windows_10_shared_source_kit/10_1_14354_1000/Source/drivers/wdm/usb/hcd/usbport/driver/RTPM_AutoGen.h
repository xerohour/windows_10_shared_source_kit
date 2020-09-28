
/*++
Copyright (c) Microsoft Corporation

Module Name:

RTPM_AutoGen.h

Abstract:

     This header file contains State Machines for USBPORT
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/

#pragma once



//
// Enum of States in all the State Machines
//
typedef enum _RTPM_STATE {
    RtpmStateIgnored                                 = 1000,
    RtpmStateEmptySubState                           = 1001,
    RtpmStateNull                                    = 1002,
    RtpmStateInD0NotIdle                             = 2000,
    RtpmStateAcknowledgingPowerInD0                  = 2001,
    RtpmStateAcknowledgingPowerOnPowerFailure        = 2002,
    RtpmStateArmingForWakeFromS0                     = 2003,
    RtpmStateCompletingHubWaitWakeOnRequired         = 2004,
    RtpmStateCriticalPowerFailure                    = 2005,
    RtpmStateDisarmForWakeOnDxFailure                = 2006,
    RtpmStateDisarmingForWakeOnPowerRequired         = 2007,
    RtpmStateDxIdle                                  = 2008,
    RtpmStateInD0Idle                                = 2009,
    RtpmStateIndicatingCriticalPowerFailure          = 2010,
    RtpmStateReInitializingController                = 2011,
    RtpmStateReInitializingControllerOnWake          = 2012,
    RtpmStateRequestingD0OnWake                      = 2013,
    RtpmStateRequestingD0Required                    = 2014,
    RtpmStateRequestingDx                            = 2015,
    RtpmStateWaitingForD0CompletionRequired          = 2016,
    RtpmStateWaitingForD0CompletionWake              = 2017,
    RtpmStateWaitingForDxCompletion                  = 2018,
    RtpmStateWaitingForDxCompletionOnWake            = 2019,
} RTPM_STATE, *PRTPM_STATE;



//
// Definitions for State Entry Functions 
//
STATE_ENTRY_FUNCTION          USBPORTRTPM_AcknowledgingPowerInD0;

STATE_ENTRY_FUNCTION          USBPORTRTPM_AcknowledgingPowerOnPowerFailure;

STATE_ENTRY_FUNCTION          USBPORTRTPM_ArmingForWakeFromS0;

STATE_ENTRY_FUNCTION          USBPORTRTPM_CompletingHubWaitWakeOnRequired;

STATE_ENTRY_FUNCTION          USBPORTRTPM_CriticalPowerFailure;

STATE_ENTRY_FUNCTION          USBPORTRTPM_DisarmForWakeOnDxFailure;

STATE_ENTRY_FUNCTION          USBPORTRTPM_DisarmingForWakeOnPowerRequired;

STATE_ENTRY_FUNCTION          USBPORTRTPM_DxIdle;

STATE_ENTRY_FUNCTION          USBPORTRTPM_InD0Idle;

STATE_ENTRY_FUNCTION          USBPORTRTPM_InD0NotIdle;

STATE_ENTRY_FUNCTION          USBPORTRTPM_IndicatingCriticalPowerFailure;

STATE_ENTRY_FUNCTION          USBPORTRTPM_ReInitializingController;

STATE_ENTRY_FUNCTION          USBPORTRTPM_ReInitializingControllerOnWake;

STATE_ENTRY_FUNCTION          USBPORTRTPM_RequestingD0OnWake;

STATE_ENTRY_FUNCTION          USBPORTRTPM_RequestingD0Required;

STATE_ENTRY_FUNCTION          USBPORTRTPM_RequestingDx;

STATE_ENTRY_FUNCTION          USBPORTRTPM_WaitingForD0CompletionRequired;

STATE_ENTRY_FUNCTION          USBPORTRTPM_WaitingForD0CompletionWake;

STATE_ENTRY_FUNCTION          USBPORTRTPM_WaitingForDxCompletion;

STATE_ENTRY_FUNCTION          USBPORTRTPM_WaitingForDxCompletionOnWake;




//
// Sub State Entries for the states in the State Machine
//


//
// State Entries for the states in the State Machine
//
STATE_ENTRY   RtpmStateEntryAcknowledgingPowerInD0 = {
    // State Entry Function
    USBPORTRTPM_AcknowledgingPowerInD0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventOperationSuccess ,   RtpmStateInD0NotIdle },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryAcknowledgingPowerOnPowerFailure = {
    // State Entry Function
    USBPORTRTPM_AcknowledgingPowerOnPowerFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventOperationSuccess ,   RtpmStateCriticalPowerFailure },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryArmingForWakeFromS0 = {
    // State Entry Function
    USBPORTRTPM_ArmingForWakeFromS0,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventOperationFailure ,   RtpmStateInD0Idle },
        { RtpmEventOperationSuccess ,   RtpmStateRequestingDx },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryCompletingHubWaitWakeOnRequired = {
    // State Entry Function
    USBPORTRTPM_CompletingHubWaitWakeOnRequired,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventOperationSuccess ,   RtpmStateRequestingD0Required },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryCriticalPowerFailure = {
    // State Entry Function
    USBPORTRTPM_CriticalPowerFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventPowerNotRequired ,   RtpmStateCriticalPowerFailure },
        { RtpmEventPowerRequired ,      RtpmStateAcknowledgingPowerOnPowerFailure },
        { RtpmEventPowerNotRequired ,   RtpmStateCriticalPowerFailure },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryDisarmForWakeOnDxFailure = {
    // State Entry Function
    USBPORTRTPM_DisarmForWakeOnDxFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventWaitWakeFailure ,    RtpmStateInD0Idle },
        { RtpmEventWaitWakeSuccess ,    RtpmStateInD0Idle },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryDisarmingForWakeOnPowerRequired = {
    // State Entry Function
    USBPORTRTPM_DisarmingForWakeOnPowerRequired,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventWaitWakeSuccess ,    RtpmStateCompletingHubWaitWakeOnRequired },
        { RtpmEventWaitWakeFailure ,    RtpmStateRequestingD0Required },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryDxIdle = {
    // State Entry Function
    USBPORTRTPM_DxIdle,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventWaitWakeFailure ,    RtpmStateRequestingD0OnWake },
        { RtpmEventPowerRequired ,      RtpmStateDisarmingForWakeOnPowerRequired },
        { RtpmEventWaitWakeSuccess ,    RtpmStateRequestingD0OnWake },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryInD0Idle = {
    // State Entry Function
    USBPORTRTPM_InD0Idle,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventPowerRequired ,      RtpmStateAcknowledgingPowerInD0 },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryInD0NotIdle = {
    // State Entry Function
    USBPORTRTPM_InD0NotIdle,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventPowerNotRequired ,   RtpmStateArmingForWakeFromS0 },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryIndicatingCriticalPowerFailure = {
    // State Entry Function
    USBPORTRTPM_IndicatingCriticalPowerFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventOperationSuccess ,   RtpmStateCriticalPowerFailure },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryReInitializingController = {
    // State Entry Function
    USBPORTRTPM_ReInitializingController,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventOperationSuccess ,   RtpmStateAcknowledgingPowerInD0 },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryReInitializingControllerOnWake = {
    // State Entry Function
    USBPORTRTPM_ReInitializingControllerOnWake,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventOperationSuccess ,   RtpmStateInD0Idle },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryRequestingD0OnWake = {
    // State Entry Function
    USBPORTRTPM_RequestingD0OnWake,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventOperationSuccess ,   RtpmStateWaitingForD0CompletionWake },
        { RtpmEventOperationFailure ,   RtpmStateIndicatingCriticalPowerFailure },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryRequestingD0Required = {
    // State Entry Function
    USBPORTRTPM_RequestingD0Required,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventOperationFailure ,   RtpmStateIndicatingCriticalPowerFailure },
        { RtpmEventOperationSuccess ,   RtpmStateWaitingForD0CompletionRequired },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryRequestingDx = {
    // State Entry Function
    USBPORTRTPM_RequestingDx,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventOperationFailure ,   RtpmStateDisarmForWakeOnDxFailure },
        { RtpmEventOperationSuccess ,   RtpmStateWaitingForDxCompletion },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryWaitingForD0CompletionRequired = {
    // State Entry Function
    USBPORTRTPM_WaitingForD0CompletionRequired,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventD0CompleteSuccess ,  RtpmStateReInitializingController },
        { RtpmEventD0CompleteFailure ,  RtpmStateIndicatingCriticalPowerFailure },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryWaitingForD0CompletionWake = {
    // State Entry Function
    USBPORTRTPM_WaitingForD0CompletionWake,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventD0CompleteSuccess ,  RtpmStateReInitializingControllerOnWake },
        { RtpmEventD0CompleteFailure ,  RtpmStateIndicatingCriticalPowerFailure },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryWaitingForDxCompletion = {
    // State Entry Function
    USBPORTRTPM_WaitingForDxCompletion,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventDxCompleteSuccess ,  RtpmStateDxIdle },
        { RtpmEventDxCompleteFailure ,  RtpmStateIndicatingCriticalPowerFailure },
        { RtpmEventWaitWakeSuccess ,    RtpmStateWaitingForDxCompletionOnWake },
        { RtpmEventWaitWakeFailure ,    RtpmStateWaitingForDxCompletionOnWake },
        { RtpmEventNull ,               RtpmStateNull },
    }
};



STATE_ENTRY   RtpmStateEntryWaitingForDxCompletionOnWake = {
    // State Entry Function
    USBPORTRTPM_WaitingForDxCompletionOnWake,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { RtpmEventDxCompleteFailure ,  RtpmStateIndicatingCriticalPowerFailure },
        { RtpmEventDxCompleteSuccess ,  RtpmStateRequestingD0OnWake },
        { RtpmEventNull ,               RtpmStateNull },
    }
};






//
// Global List of States in all the State Machines
//
PSTATE_ENTRY    RTPMStateTable[] = {
    &RtpmStateEntryInD0NotIdle,
    &RtpmStateEntryAcknowledgingPowerInD0,
    &RtpmStateEntryAcknowledgingPowerOnPowerFailure,
    &RtpmStateEntryArmingForWakeFromS0,
    &RtpmStateEntryCompletingHubWaitWakeOnRequired,
    &RtpmStateEntryCriticalPowerFailure,
    &RtpmStateEntryDisarmForWakeOnDxFailure,
    &RtpmStateEntryDisarmingForWakeOnPowerRequired,
    &RtpmStateEntryDxIdle,
    &RtpmStateEntryInD0Idle,
    &RtpmStateEntryIndicatingCriticalPowerFailure,
    &RtpmStateEntryReInitializingController,
    &RtpmStateEntryReInitializingControllerOnWake,
    &RtpmStateEntryRequestingD0OnWake,
    &RtpmStateEntryRequestingD0Required,
    &RtpmStateEntryRequestingDx,
    &RtpmStateEntryWaitingForD0CompletionRequired,
    &RtpmStateEntryWaitingForD0CompletionWake,
    &RtpmStateEntryWaitingForDxCompletion,
    &RtpmStateEntryWaitingForDxCompletionOnWake,
};
