
/*++
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !!    DO NOT MODIFY THIS FILE MANUALLY     !!
 !!    This file is created by              !!
 !!    StateMachineConverter.ps1            !!
 !!    in this directory.                   !!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Copyright (c) Microsoft Corporation

Module Name:

StateMachineUcxEndpoint.c

Abstract:

     This header file contains State Machines for UcxEndpoint
     This has been generated automatically from a visio file.

--*/


#include "pch.h"
#include "StateMachineUcxEndpoint.tmh"
//
// Auto Event Handlers
//
SM_ENGINE_AUTO_EVENT_HANDLER    UcxEndpointEventHandler_ClearEnabledFlagAndCompletePendingOperation ;
SM_ENGINE_AUTO_EVENT_HANDLER    UcxEndpointEventHandler_CompletePendingOperation ;
SM_ENGINE_AUTO_EVENT_HANDLER    UcxEndpointEventHandler_CompletePendingTreePurge ;
SM_ENGINE_AUTO_EVENT_HANDLER    UcxEndpointEventHandler_Ignore ;
SM_ENGINE_AUTO_EVENT_HANDLER    UcxEndpointEventHandler_OkToResetController ;
SM_ENGINE_AUTO_EVENT_HANDLER    UcxEndpointEventHandler_OkToResetControllerAndPauseSM ;
SM_ENGINE_AUTO_EVENT_HANDLER    UcxEndpointEventHandler_ResumeSM ;





//
// Definitions for State Entry Functions 
//
SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_Aborting1;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_Aborting2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_ClearingEnabledFlag;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingAbortUrbIfNotAlreadyCancelled1;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingAbortUrbIfNotAlreadyCancelled2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp1;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp4;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsWdfRequest2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingEnableStaticStreamsIrp1;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingEnableStaticStreamsIrp2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingEndpointResetIrp1;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingEndpointResetIrp2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingEndpointResetIrp4;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingPendingOperation1;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingPendingOperation2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingPendingOperation3;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingPendingOperation4;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_CompletingPendingTreePurge;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_Created;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_Deleting;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_DeletingStale;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_DisablingStaticStreams;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_DisablingStaticStreams2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_Enabled;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_EnablingStaticStreams;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_FailingEnableStaticStreamsIrp;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_FailingEndpointResetWdfRequest;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_IsAbleToStart1;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_IsAbleToStart2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_IsDeprogrammedDueToControllerReset4;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_IsDeprogrammedDueToControllerResetOrDisconnected;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_NeedToSetStale;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_OkToResetControllerAndPauseSM1;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_OkToResetControllerAndPauseSM2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_PendingDestroy;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_PurgedOrDisabled;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_Purging1;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_Purging2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_Purging3;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_Purging4;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_Purging5;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_PurgingOnControllerReset;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_RejectingEnableStaticStreamsWdfRequest2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_ResettingEndpoint1;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_ResettingEndpoint2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_ResumeSMAndWaitForPendingOperation1;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_ResumeSMAndWaitForPendingOperation2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_SetStaleAndDeletePreviousStale;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_SettingEnabledFlag1;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_SettingEnabledFlag2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_Stale;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_Starting1;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_Starting2;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_Starting3;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_WaitingForAbortUrbToBeQueued;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_WaitingForPurgeComplete1;

SM_ENGINE_STATE_ENTRY_FUNCTION          UcxEndpointStateEntryFunc_WaitingForPurgeComplete2;




//
// State Entries for the states in the State Machine
//
SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_Aborting1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_Aborting1[] = {
    { UcxEndpointEventAbortComplete ,UcxEndpointStateCompletingAbortUrbIfNotAlreadyCancelled1Index },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointStateOkToResetControllerAndPauseSM1Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryAborting1 = {
    // State Enum
    UcxEndpointStateAborting1,
    // State Entry Function
    UcxEndpointStateEntryFunc_Aborting1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_Aborting1,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_Aborting1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_Aborting2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_Aborting2[] = {
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointStateOkToResetControllerAndPauseSM2Index },
    { UcxEndpointEventAbortComplete ,UcxEndpointStateStarting3Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryAborting2 = {
    // State Enum
    UcxEndpointStateAborting2,
    // State Entry Function
    UcxEndpointStateEntryFunc_Aborting2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_Aborting2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_Aborting2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_ClearingEnabledFlag[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_ClearingEnabledFlag[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStatePurging1Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryClearingEnabledFlag = {
    // State Enum
    UcxEndpointStateClearingEnabledFlag,
    // State Entry Function
    UcxEndpointStateEntryFunc_ClearingEnabledFlag,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_ClearingEnabledFlag,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_ClearingEnabledFlag
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingAbortUrbIfNotAlreadyCancelled1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingAbortUrbIfNotAlreadyCancelled1[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStateStarting2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingAbortUrbIfNotAlreadyCancelled1 = {
    // State Enum
    UcxEndpointStateCompletingAbortUrbIfNotAlreadyCancelled1,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingAbortUrbIfNotAlreadyCancelled1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingAbortUrbIfNotAlreadyCancelled1,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingAbortUrbIfNotAlreadyCancelled1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingAbortUrbIfNotAlreadyCancelled2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingAbortUrbIfNotAlreadyCancelled2[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStatePurging2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingAbortUrbIfNotAlreadyCancelled2 = {
    // State Enum
    UcxEndpointStateCompletingAbortUrbIfNotAlreadyCancelled2,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingAbortUrbIfNotAlreadyCancelled2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingAbortUrbIfNotAlreadyCancelled2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingAbortUrbIfNotAlreadyCancelled2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingDisableStaticStreamsIrp1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingDisableStaticStreamsIrp1[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStateEnabledIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingDisableStaticStreamsIrp1 = {
    // State Enum
    UcxEndpointStateCompletingDisableStaticStreamsIrp1,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingDisableStaticStreamsIrp1,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingDisableStaticStreamsIrp1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingDisableStaticStreamsIrp2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingDisableStaticStreamsIrp2[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStatePurgedOrDisabledIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingDisableStaticStreamsIrp2 = {
    // State Enum
    UcxEndpointStateCompletingDisableStaticStreamsIrp2,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingDisableStaticStreamsIrp2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingDisableStaticStreamsIrp2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingDisableStaticStreamsIrp4[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingDisableStaticStreamsIrp4[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStatePurging2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingDisableStaticStreamsIrp4 = {
    // State Enum
    UcxEndpointStateCompletingDisableStaticStreamsIrp4,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsIrp4,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingDisableStaticStreamsIrp4,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingDisableStaticStreamsIrp4
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingDisableStaticStreamsWdfRequest2[] = {
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetControllerAndPauseSM },
    { UcxEndpointEventControllerResetComplete ,UcxEndpointEventHandler_ResumeSM },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingDisableStaticStreamsWdfRequest2[] = {
    { UcxEndpointEventStaticStreamsDisableComplete ,UcxEndpointStateCompletingDisableStaticStreamsIrp2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingDisableStaticStreamsWdfRequest2 = {
    // State Enum
    UcxEndpointStateCompletingDisableStaticStreamsWdfRequest2,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingDisableStaticStreamsWdfRequest2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingDisableStaticStreamsWdfRequest2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingDisableStaticStreamsWdfRequest2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingEnableStaticStreamsIrp1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingEnableStaticStreamsIrp1[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStateEnabledIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingEnableStaticStreamsIrp1 = {
    // State Enum
    UcxEndpointStateCompletingEnableStaticStreamsIrp1,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingEnableStaticStreamsIrp1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingEnableStaticStreamsIrp1,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingEnableStaticStreamsIrp1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingEnableStaticStreamsIrp2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingEnableStaticStreamsIrp2[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStatePurgedOrDisabledIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingEnableStaticStreamsIrp2 = {
    // State Enum
    UcxEndpointStateCompletingEnableStaticStreamsIrp2,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingEnableStaticStreamsIrp2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingEnableStaticStreamsIrp2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingEnableStaticStreamsIrp2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingEndpointResetIrp1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingEndpointResetIrp1[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStateEnabledIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingEndpointResetIrp1 = {
    // State Enum
    UcxEndpointStateCompletingEndpointResetIrp1,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingEndpointResetIrp1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingEndpointResetIrp1,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingEndpointResetIrp1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingEndpointResetIrp2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingEndpointResetIrp2[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStatePurgedOrDisabledIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingEndpointResetIrp2 = {
    // State Enum
    UcxEndpointStateCompletingEndpointResetIrp2,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingEndpointResetIrp2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingEndpointResetIrp2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingEndpointResetIrp2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingEndpointResetIrp4[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingEndpointResetIrp4[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStatePurging2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingEndpointResetIrp4 = {
    // State Enum
    UcxEndpointStateCompletingEndpointResetIrp4,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingEndpointResetIrp4,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingEndpointResetIrp4,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingEndpointResetIrp4
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingPendingOperation1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingPendingOperation1[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStateEnabledIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingPendingOperation1 = {
    // State Enum
    UcxEndpointStateCompletingPendingOperation1,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingPendingOperation1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingPendingOperation1,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingPendingOperation1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingPendingOperation2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingPendingOperation2[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStatePurgedOrDisabledIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingPendingOperation2 = {
    // State Enum
    UcxEndpointStateCompletingPendingOperation2,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingPendingOperation2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingPendingOperation2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingPendingOperation2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingPendingOperation3[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingPendingOperation3[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStatePurging2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingPendingOperation3 = {
    // State Enum
    UcxEndpointStateCompletingPendingOperation3,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingPendingOperation3,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingPendingOperation3,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingPendingOperation3
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingPendingOperation4[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingPendingOperation4[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStatePurgedOrDisabledIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingPendingOperation4 = {
    // State Enum
    UcxEndpointStateCompletingPendingOperation4,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingPendingOperation4,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingPendingOperation4,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingPendingOperation4
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_CompletingPendingTreePurge[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_CompletingPendingTreePurge[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStatePurgedOrDisabledIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCompletingPendingTreePurge = {
    // State Enum
    UcxEndpointStateCompletingPendingTreePurge,
    // State Entry Function
    UcxEndpointStateEntryFunc_CompletingPendingTreePurge,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_CompletingPendingTreePurge,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_CompletingPendingTreePurge
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_Created[] = {
    { UcxEndpointEventStartFromHub ,UcxEndpointEventHandler_CompletePendingOperation },
    { UcxEndpointEventDisconnectFromHub ,UcxEndpointEventHandler_Ignore },
    { UcxEndpointEventPurgeFromHub ,UcxEndpointEventHandler_CompletePendingOperation },
    { UcxEndpointEventControllerResetComplete ,UcxEndpointEventHandler_ResumeSM },
    { UcxEndpointEventUsbDeviceResetFromHub ,UcxEndpointEventHandler_CompletePendingOperation },
    { UcxEndpointEventTreePurgeFromHub ,UcxEndpointEventHandler_CompletePendingTreePurge },
    { UcxEndpointEventUsbDeviceResetComplete ,UcxEndpointEventHandler_CompletePendingOperation },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetControllerAndPauseSM },
    { UcxEndpointEventAbortFromHub ,UcxEndpointEventHandler_CompletePendingOperation },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_Created[] = {
    { UcxEndpointEventDelete ,      UcxEndpointStateNeedToSetStaleIndex },
    { UcxEndpointEventEnableComplete ,UcxEndpointStateSettingEnabledFlag1Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryCreated = {
    // State Enum
    UcxEndpointStateCreated,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagHandlesLowPriEvents|SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_Created,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_Created
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_Deleting[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_Deleting[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStatePendingDestroyIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryDeleting = {
    // State Enum
    UcxEndpointStateDeleting,
    // State Entry Function
    UcxEndpointStateEntryFunc_Deleting,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_Deleting,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_Deleting
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_DeletingStale[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_DeletingStale[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStatePendingDestroyIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryDeletingStale = {
    // State Enum
    UcxEndpointStateDeletingStale,
    // State Entry Function
    UcxEndpointStateEntryFunc_DeletingStale,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_DeletingStale,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_DeletingStale
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_DisablingStaticStreams[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_DisablingStaticStreams[] = {
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointStateOkToResetControllerAndPauseSM1Index },
    { UcxEndpointEventStaticStreamsDisableComplete ,UcxEndpointStateCompletingDisableStaticStreamsIrp1Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryDisablingStaticStreams = {
    // State Enum
    UcxEndpointStateDisablingStaticStreams,
    // State Entry Function
    UcxEndpointStateEntryFunc_DisablingStaticStreams,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_DisablingStaticStreams,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_DisablingStaticStreams
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_DisablingStaticStreams2[] = {
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetControllerAndPauseSM },
    { UcxEndpointEventControllerResetComplete ,UcxEndpointEventHandler_ResumeSM },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_DisablingStaticStreams2[] = {
    { UcxEndpointEventStaticStreamsDisableComplete ,UcxEndpointStateCompletingDisableStaticStreamsIrp2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryDisablingStaticStreams2 = {
    // State Enum
    UcxEndpointStateDisablingStaticStreams2,
    // State Entry Function
    UcxEndpointStateEntryFunc_DisablingStaticStreams2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_DisablingStaticStreams2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_DisablingStaticStreams2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_Enabled[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_Enabled[] = {
    { UcxEndpointEventStaticStreamsEnableFromClient ,UcxEndpointStateEnablingStaticStreamsIndex },
    { UcxEndpointEventStaticStreamsDisableFromClient ,UcxEndpointStateDisablingStaticStreamsIndex },
    { UcxEndpointEventAbortFromHub ,UcxEndpointStateAborting2Index },
    { UcxEndpointEventPrepareDisableFromHub ,UcxEndpointStateClearingEnabledFlagIndex },
    { UcxEndpointEventAbortUrbFromClient ,UcxEndpointStateWaitingForAbortUrbToBeQueuedIndex },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointStatePurgingOnControllerResetIndex },
    { UcxEndpointEventPurgeFromHub ,UcxEndpointStatePurging1Index },
    { UcxEndpointEventUsbDeviceResetFromHub ,UcxEndpointStateClearingEnabledFlagIndex },
    { UcxEndpointEventTreePurgeFromHub ,UcxEndpointStatePurging4Index },
    { UcxEndpointEventDisconnectFromHub ,UcxEndpointStatePurging3Index },
    { UcxEndpointEventEndpointResetFromHub ,UcxEndpointStateResettingEndpoint1Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryEnabled = {
    // State Enum
    UcxEndpointStateEnabled,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagHandlesLowPriEvents|SmEngineStateFlagNoStateEntryFunction|SmEngineStateFlagHandlesAbortUrb,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_Enabled,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_Enabled
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_EnablingStaticStreams[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_EnablingStaticStreams[] = {
    { UcxEndpointEventStaticStreamsEnableComplete ,UcxEndpointStateCompletingEnableStaticStreamsIrp1Index },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointStateOkToResetControllerAndPauseSM1Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryEnablingStaticStreams = {
    // State Enum
    UcxEndpointStateEnablingStaticStreams,
    // State Entry Function
    UcxEndpointStateEntryFunc_EnablingStaticStreams,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_EnablingStaticStreams,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_EnablingStaticStreams
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_FailingEnableStaticStreamsIrp[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_FailingEnableStaticStreamsIrp[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStatePurging2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryFailingEnableStaticStreamsIrp = {
    // State Enum
    UcxEndpointStateFailingEnableStaticStreamsIrp,
    // State Entry Function
    UcxEndpointStateEntryFunc_FailingEnableStaticStreamsIrp,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_FailingEnableStaticStreamsIrp,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_FailingEnableStaticStreamsIrp
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_FailingEndpointResetWdfRequest[] = {
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetControllerAndPauseSM },
    { UcxEndpointEventControllerResetComplete ,UcxEndpointEventHandler_ResumeSM },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_FailingEndpointResetWdfRequest[] = {
    { UcxEndpointEventEndpointResetComplete ,UcxEndpointStateCompletingEndpointResetIrp2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryFailingEndpointResetWdfRequest = {
    // State Enum
    UcxEndpointStateFailingEndpointResetWdfRequest,
    // State Entry Function
    UcxEndpointStateEntryFunc_FailingEndpointResetWdfRequest,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_FailingEndpointResetWdfRequest,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_FailingEndpointResetWdfRequest
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_IsAbleToStart1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_IsAbleToStart1[] = {
    { UcxEndpointEventYes ,         UcxEndpointStateStarting1Index },
    { UcxEndpointEventNo ,          UcxEndpointStateCompletingPendingOperation2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryIsAbleToStart1 = {
    // State Enum
    UcxEndpointStateIsAbleToStart1,
    // State Entry Function
    UcxEndpointStateEntryFunc_IsAbleToStart1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_IsAbleToStart1,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_IsAbleToStart1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_IsAbleToStart2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_IsAbleToStart2[] = {
    { UcxEndpointEventYes ,         UcxEndpointStateCompletingPendingOperation1Index },
    { UcxEndpointEventNo ,          UcxEndpointStatePurging5Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryIsAbleToStart2 = {
    // State Enum
    UcxEndpointStateIsAbleToStart2,
    // State Entry Function
    UcxEndpointStateEntryFunc_IsAbleToStart2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_IsAbleToStart2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_IsAbleToStart2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_IsDeprogrammedDueToControllerReset4[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_IsDeprogrammedDueToControllerReset4[] = {
    { UcxEndpointEventYes ,         UcxEndpointStateCompletingDisableStaticStreamsWdfRequest2Index },
    { UcxEndpointEventNo ,          UcxEndpointStateDisablingStaticStreams2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryIsDeprogrammedDueToControllerReset4 = {
    // State Enum
    UcxEndpointStateIsDeprogrammedDueToControllerReset4,
    // State Entry Function
    UcxEndpointStateEntryFunc_IsDeprogrammedDueToControllerReset4,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_IsDeprogrammedDueToControllerReset4,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_IsDeprogrammedDueToControllerReset4
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_IsDeprogrammedDueToControllerResetOrDisconnected[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_IsDeprogrammedDueToControllerResetOrDisconnected[] = {
    { UcxEndpointEventYes ,         UcxEndpointStateFailingEndpointResetWdfRequestIndex },
    { UcxEndpointEventNo ,          UcxEndpointStateResettingEndpoint2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryIsDeprogrammedDueToControllerResetOrDisconnected = {
    // State Enum
    UcxEndpointStateIsDeprogrammedDueToControllerResetOrDisconnected,
    // State Entry Function
    UcxEndpointStateEntryFunc_IsDeprogrammedDueToControllerResetOrDisconnected,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_IsDeprogrammedDueToControllerResetOrDisconnected,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_IsDeprogrammedDueToControllerResetOrDisconnected
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_NeedToSetStale[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_NeedToSetStale[] = {
    { UcxEndpointEventYes ,         UcxEndpointStateSetStaleAndDeletePreviousStaleIndex },
    { UcxEndpointEventNo ,          UcxEndpointStateDeletingIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryNeedToSetStale = {
    // State Enum
    UcxEndpointStateNeedToSetStale,
    // State Entry Function
    UcxEndpointStateEntryFunc_NeedToSetStale,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_NeedToSetStale,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_NeedToSetStale
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_OkToResetControllerAndPauseSM1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_OkToResetControllerAndPauseSM1[] = {
    { UcxEndpointEventControllerResetComplete ,UcxEndpointStateResumeSMAndWaitForPendingOperation1Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryOkToResetControllerAndPauseSM1 = {
    // State Enum
    UcxEndpointStateOkToResetControllerAndPauseSM1,
    // State Entry Function
    UcxEndpointStateEntryFunc_OkToResetControllerAndPauseSM1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_OkToResetControllerAndPauseSM1,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_OkToResetControllerAndPauseSM1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_OkToResetControllerAndPauseSM2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_OkToResetControllerAndPauseSM2[] = {
    { UcxEndpointEventControllerResetComplete ,UcxEndpointStateResumeSMAndWaitForPendingOperation2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryOkToResetControllerAndPauseSM2 = {
    // State Enum
    UcxEndpointStateOkToResetControllerAndPauseSM2,
    // State Entry Function
    UcxEndpointStateEntryFunc_OkToResetControllerAndPauseSM2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_OkToResetControllerAndPauseSM2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_OkToResetControllerAndPauseSM2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_PendingDestroy[] = {
    { UcxEndpointEventUsbDeviceResetFromHub ,UcxEndpointEventHandler_CompletePendingOperation },
    { UcxEndpointEventDisconnectFromHub ,UcxEndpointEventHandler_Ignore },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetController },
    { UcxEndpointEventPurgeFromHub ,UcxEndpointEventHandler_CompletePendingOperation },
    { UcxEndpointEventTreePurgeFromHub ,UcxEndpointEventHandler_CompletePendingTreePurge },
    { UcxEndpointEventStartFromHub ,UcxEndpointEventHandler_CompletePendingOperation },
    { UcxEndpointEventAbortFromHub ,UcxEndpointEventHandler_CompletePendingOperation },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_PendingDestroy[] = {
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryPendingDestroy = {
    // State Enum
    UcxEndpointStatePendingDestroy,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagHandlesLowPriEvents|SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_PendingDestroy,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_PendingDestroy
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_PurgedOrDisabled[] = {
    { UcxEndpointEventPrepareDisableFromHub ,UcxEndpointEventHandler_ClearEnabledFlagAndCompletePendingOperation },
    { UcxEndpointEventUsbDeviceResetFromHub ,UcxEndpointEventHandler_ClearEnabledFlagAndCompletePendingOperation },
    { UcxEndpointEventTreePurgeFromHub ,UcxEndpointEventHandler_CompletePendingTreePurge },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetControllerAndPauseSM },
    { UcxEndpointEventControllerResetComplete ,UcxEndpointEventHandler_ResumeSM },
    { UcxEndpointEventPurgeFromHub ,UcxEndpointEventHandler_CompletePendingOperation },
    { UcxEndpointEventDisconnectFromHub ,UcxEndpointEventHandler_Ignore },
    { UcxEndpointEventAbortFromHub ,UcxEndpointEventHandler_CompletePendingOperation },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_PurgedOrDisabled[] = {
    { UcxEndpointEventDelete ,      UcxEndpointStateNeedToSetStaleIndex },
    { UcxEndpointEventUsbDeviceResetComplete ,UcxEndpointStateSettingEnabledFlag2Index },
    { UcxEndpointEventEnableComplete ,UcxEndpointStateSettingEnabledFlag2Index },
    { UcxEndpointEventStartFromHub ,UcxEndpointStateIsAbleToStart1Index },
    { UcxEndpointEventStaticStreamsEnableFromClient ,UcxEndpointStateRejectingEnableStaticStreamsWdfRequest2Index },
    { UcxEndpointEventEndpointResetFromHub ,UcxEndpointStateIsDeprogrammedDueToControllerResetOrDisconnectedIndex },
    { UcxEndpointEventStaticStreamsDisableFromClient ,UcxEndpointStateIsDeprogrammedDueToControllerReset4Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryPurgedOrDisabled = {
    // State Enum
    UcxEndpointStatePurgedOrDisabled,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagHandlesLowPriEvents|SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_PurgedOrDisabled,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_PurgedOrDisabled
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_Purging1[] = {
    { UcxEndpointEventControllerResetComplete ,UcxEndpointEventHandler_ResumeSM },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetControllerAndPauseSM },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_Purging1[] = {
    { UcxEndpointEventPurgeComplete ,UcxEndpointStateCompletingPendingOperation2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryPurging1 = {
    // State Enum
    UcxEndpointStatePurging1,
    // State Entry Function
    UcxEndpointStateEntryFunc_Purging1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_Purging1,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_Purging1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_Purging2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_Purging2[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStateWaitingForPurgeComplete2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryPurging2 = {
    // State Enum
    UcxEndpointStatePurging2,
    // State Entry Function
    UcxEndpointStateEntryFunc_Purging2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_Purging2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_Purging2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_Purging3[] = {
    { UcxEndpointEventControllerResetComplete ,UcxEndpointEventHandler_ResumeSM },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetControllerAndPauseSM },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_Purging3[] = {
    { UcxEndpointEventPurgeComplete ,UcxEndpointStatePurgedOrDisabledIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryPurging3 = {
    // State Enum
    UcxEndpointStatePurging3,
    // State Entry Function
    UcxEndpointStateEntryFunc_Purging3,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_Purging3,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_Purging3
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_Purging4[] = {
    { UcxEndpointEventControllerResetComplete ,UcxEndpointEventHandler_ResumeSM },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetControllerAndPauseSM },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_Purging4[] = {
    { UcxEndpointEventPurgeComplete ,UcxEndpointStateCompletingPendingTreePurgeIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryPurging4 = {
    // State Enum
    UcxEndpointStatePurging4,
    // State Entry Function
    UcxEndpointStateEntryFunc_Purging4,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_Purging4,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_Purging4
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_Purging5[] = {
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetControllerAndPauseSM },
    { UcxEndpointEventControllerResetComplete ,UcxEndpointEventHandler_ResumeSM },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_Purging5[] = {
    { UcxEndpointEventPurgeComplete ,UcxEndpointStateCompletingPendingOperation4Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryPurging5 = {
    // State Enum
    UcxEndpointStatePurging5,
    // State Entry Function
    UcxEndpointStateEntryFunc_Purging5,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_Purging5,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_Purging5
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_PurgingOnControllerReset[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_PurgingOnControllerReset[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStateWaitingForPurgeComplete1Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryPurgingOnControllerReset = {
    // State Enum
    UcxEndpointStatePurgingOnControllerReset,
    // State Entry Function
    UcxEndpointStateEntryFunc_PurgingOnControllerReset,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_PurgingOnControllerReset,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_PurgingOnControllerReset
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_RejectingEnableStaticStreamsWdfRequest2[] = {
    { UcxEndpointEventControllerResetComplete ,UcxEndpointEventHandler_ResumeSM },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetControllerAndPauseSM },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_RejectingEnableStaticStreamsWdfRequest2[] = {
    { UcxEndpointEventStaticStreamsEnableComplete ,UcxEndpointStateCompletingEnableStaticStreamsIrp2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryRejectingEnableStaticStreamsWdfRequest2 = {
    // State Enum
    UcxEndpointStateRejectingEnableStaticStreamsWdfRequest2,
    // State Entry Function
    UcxEndpointStateEntryFunc_RejectingEnableStaticStreamsWdfRequest2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_RejectingEnableStaticStreamsWdfRequest2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_RejectingEnableStaticStreamsWdfRequest2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_ResettingEndpoint1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_ResettingEndpoint1[] = {
    { UcxEndpointEventEndpointResetComplete ,UcxEndpointStateCompletingEndpointResetIrp1Index },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointStateOkToResetControllerAndPauseSM1Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryResettingEndpoint1 = {
    // State Enum
    UcxEndpointStateResettingEndpoint1,
    // State Entry Function
    UcxEndpointStateEntryFunc_ResettingEndpoint1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_ResettingEndpoint1,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_ResettingEndpoint1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_ResettingEndpoint2[] = {
    { UcxEndpointEventControllerResetComplete ,UcxEndpointEventHandler_ResumeSM },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetControllerAndPauseSM },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_ResettingEndpoint2[] = {
    { UcxEndpointEventEndpointResetComplete ,UcxEndpointStateCompletingEndpointResetIrp2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryResettingEndpoint2 = {
    // State Enum
    UcxEndpointStateResettingEndpoint2,
    // State Entry Function
    UcxEndpointStateEntryFunc_ResettingEndpoint2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_ResettingEndpoint2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_ResettingEndpoint2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_ResumeSMAndWaitForPendingOperation1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_ResumeSMAndWaitForPendingOperation1[] = {
    { UcxEndpointEventAbortComplete ,UcxEndpointStateCompletingAbortUrbIfNotAlreadyCancelled2Index },
    { UcxEndpointEventAbortUrbQueued ,UcxEndpointStateCompletingAbortUrbIfNotAlreadyCancelled2Index },
    { UcxEndpointEventStaticStreamsEnableComplete ,UcxEndpointStateFailingEnableStaticStreamsIrpIndex },
    { UcxEndpointEventStaticStreamsDisableComplete ,UcxEndpointStateCompletingDisableStaticStreamsIrp4Index },
    { UcxEndpointEventEndpointResetComplete ,UcxEndpointStateCompletingEndpointResetIrp4Index },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointStateOkToResetControllerAndPauseSM1Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryResumeSMAndWaitForPendingOperation1 = {
    // State Enum
    UcxEndpointStateResumeSMAndWaitForPendingOperation1,
    // State Entry Function
    UcxEndpointStateEntryFunc_ResumeSMAndWaitForPendingOperation1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_ResumeSMAndWaitForPendingOperation1,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_ResumeSMAndWaitForPendingOperation1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_ResumeSMAndWaitForPendingOperation2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_ResumeSMAndWaitForPendingOperation2[] = {
    { UcxEndpointEventAbortComplete ,UcxEndpointStateCompletingPendingOperation3Index },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointStateOkToResetControllerAndPauseSM2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryResumeSMAndWaitForPendingOperation2 = {
    // State Enum
    UcxEndpointStateResumeSMAndWaitForPendingOperation2,
    // State Entry Function
    UcxEndpointStateEntryFunc_ResumeSMAndWaitForPendingOperation2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_ResumeSMAndWaitForPendingOperation2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_ResumeSMAndWaitForPendingOperation2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_SetStaleAndDeletePreviousStale[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_SetStaleAndDeletePreviousStale[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStateStaleIndex },
    { UcxEndpointEventUsbDeviceDeleting ,UcxEndpointStateDeletingStaleIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntrySetStaleAndDeletePreviousStale = {
    // State Enum
    UcxEndpointStateSetStaleAndDeletePreviousStale,
    // State Entry Function
    UcxEndpointStateEntryFunc_SetStaleAndDeletePreviousStale,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_SetStaleAndDeletePreviousStale,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_SetStaleAndDeletePreviousStale
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_SettingEnabledFlag1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_SettingEnabledFlag1[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStateIsAbleToStart2Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntrySettingEnabledFlag1 = {
    // State Enum
    UcxEndpointStateSettingEnabledFlag1,
    // State Entry Function
    UcxEndpointStateEntryFunc_SettingEnabledFlag1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_SettingEnabledFlag1,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_SettingEnabledFlag1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_SettingEnabledFlag2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_SettingEnabledFlag2[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStateIsAbleToStart1Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntrySettingEnabledFlag2 = {
    // State Enum
    UcxEndpointStateSettingEnabledFlag2,
    // State Entry Function
    UcxEndpointStateEntryFunc_SettingEnabledFlag2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_SettingEnabledFlag2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_SettingEnabledFlag2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_Stale[] = {
    { UcxEndpointEventDisconnectFromHub ,UcxEndpointEventHandler_Ignore },
    { UcxEndpointEventPurgeFromHub ,UcxEndpointEventHandler_CompletePendingOperation },
    { UcxEndpointEventUsbDeviceResetFromHub ,UcxEndpointEventHandler_CompletePendingOperation },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetController },
    { UcxEndpointEventTreePurgeFromHub ,UcxEndpointEventHandler_CompletePendingTreePurge },
    { UcxEndpointEventStartFromHub ,UcxEndpointEventHandler_CompletePendingOperation },
    { UcxEndpointEventAbortFromHub ,UcxEndpointEventHandler_CompletePendingOperation },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_Stale[] = {
    { UcxEndpointEventDeleteStale , UcxEndpointStateDeletingStaleIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryStale = {
    // State Enum
    UcxEndpointStateStale,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagHandlesLowPriEvents|SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_Stale,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_Stale
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_Starting1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_Starting1[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStateCompletingPendingOperation1Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryStarting1 = {
    // State Enum
    UcxEndpointStateStarting1,
    // State Entry Function
    UcxEndpointStateEntryFunc_Starting1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_Starting1,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_Starting1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_Starting2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_Starting2[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStateEnabledIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryStarting2 = {
    // State Enum
    UcxEndpointStateStarting2,
    // State Entry Function
    UcxEndpointStateEntryFunc_Starting2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_Starting2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_Starting2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_Starting3[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_Starting3[] = {
    { UcxEndpointEventOperationSuccess ,UcxEndpointStateCompletingPendingOperation1Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryStarting3 = {
    // State Enum
    UcxEndpointStateStarting3,
    // State Entry Function
    UcxEndpointStateEntryFunc_Starting3,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_Starting3,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_Starting3
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_WaitingForAbortUrbToBeQueued[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_WaitingForAbortUrbToBeQueued[] = {
    { UcxEndpointEventAbortUrbQueued ,UcxEndpointStateAborting1Index },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointStateOkToResetControllerAndPauseSM1Index },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryWaitingForAbortUrbToBeQueued = {
    // State Enum
    UcxEndpointStateWaitingForAbortUrbToBeQueued,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_WaitingForAbortUrbToBeQueued,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_WaitingForAbortUrbToBeQueued
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_WaitingForPurgeComplete1[] = {
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetControllerAndPauseSM },
    { UcxEndpointEventControllerResetComplete ,UcxEndpointEventHandler_ResumeSM },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_WaitingForPurgeComplete1[] = {
    { UcxEndpointEventPurgeComplete ,UcxEndpointStatePurgedOrDisabledIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryWaitingForPurgeComplete1 = {
    // State Enum
    UcxEndpointStateWaitingForPurgeComplete1,
    // State Entry Function
    UcxEndpointStateEntryFunc_WaitingForPurgeComplete1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_WaitingForPurgeComplete1,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_WaitingForPurgeComplete1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   UcxEndpointAutoEventTransitions_WaitingForPurgeComplete2[] = {
    { UcxEndpointEventControllerResetComplete ,UcxEndpointEventHandler_ResumeSM },
    { UcxEndpointEventPrepareForControllerReset ,UcxEndpointEventHandler_OkToResetControllerAndPauseSM },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   UcxEndpointStateTransitions_WaitingForPurgeComplete2[] = {
    { UcxEndpointEventPurgeComplete ,UcxEndpointStatePurgedOrDisabledIndex },
    { SmEngineEventNull ,           UcxEndpointStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   UcxEndpointStateTableEntryWaitingForPurgeComplete2 = {
    // State Enum
    UcxEndpointStateWaitingForPurgeComplete2,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    UcxEndpointAutoEventTransitions_WaitingForPurgeComplete2,
    // Event State Pairs for Transitions
    UcxEndpointStateTransitions_WaitingForPurgeComplete2
};










//
// Global List of States in all the State Machines
//
PSM_ENGINE_STATE_TABLE_ENTRY    UcxEndpointStateTable[] =
{
    &UcxEndpointStateTableEntryAborting1,
    &UcxEndpointStateTableEntryAborting2,
    &UcxEndpointStateTableEntryClearingEnabledFlag,
    &UcxEndpointStateTableEntryCompletingAbortUrbIfNotAlreadyCancelled1,
    &UcxEndpointStateTableEntryCompletingAbortUrbIfNotAlreadyCancelled2,
    &UcxEndpointStateTableEntryCompletingDisableStaticStreamsIrp1,
    &UcxEndpointStateTableEntryCompletingDisableStaticStreamsIrp2,
    &UcxEndpointStateTableEntryCompletingDisableStaticStreamsIrp4,
    &UcxEndpointStateTableEntryCompletingDisableStaticStreamsWdfRequest2,
    &UcxEndpointStateTableEntryCompletingEnableStaticStreamsIrp1,
    &UcxEndpointStateTableEntryCompletingEnableStaticStreamsIrp2,
    &UcxEndpointStateTableEntryCompletingEndpointResetIrp1,
    &UcxEndpointStateTableEntryCompletingEndpointResetIrp2,
    &UcxEndpointStateTableEntryCompletingEndpointResetIrp4,
    &UcxEndpointStateTableEntryCompletingPendingOperation1,
    &UcxEndpointStateTableEntryCompletingPendingOperation2,
    &UcxEndpointStateTableEntryCompletingPendingOperation3,
    &UcxEndpointStateTableEntryCompletingPendingOperation4,
    &UcxEndpointStateTableEntryCompletingPendingTreePurge,
    &UcxEndpointStateTableEntryCreated,
    &UcxEndpointStateTableEntryDeleting,
    &UcxEndpointStateTableEntryDeletingStale,
    &UcxEndpointStateTableEntryDisablingStaticStreams,
    &UcxEndpointStateTableEntryDisablingStaticStreams2,
    &UcxEndpointStateTableEntryEnabled,
    &UcxEndpointStateTableEntryEnablingStaticStreams,
    &UcxEndpointStateTableEntryFailingEnableStaticStreamsIrp,
    &UcxEndpointStateTableEntryFailingEndpointResetWdfRequest,
    &UcxEndpointStateTableEntryIsAbleToStart1,
    &UcxEndpointStateTableEntryIsAbleToStart2,
    &UcxEndpointStateTableEntryIsDeprogrammedDueToControllerReset4,
    &UcxEndpointStateTableEntryIsDeprogrammedDueToControllerResetOrDisconnected,
    &UcxEndpointStateTableEntryNeedToSetStale,
    &UcxEndpointStateTableEntryOkToResetControllerAndPauseSM1,
    &UcxEndpointStateTableEntryOkToResetControllerAndPauseSM2,
    &UcxEndpointStateTableEntryPendingDestroy,
    &UcxEndpointStateTableEntryPurgedOrDisabled,
    &UcxEndpointStateTableEntryPurging1,
    &UcxEndpointStateTableEntryPurging2,
    &UcxEndpointStateTableEntryPurging3,
    &UcxEndpointStateTableEntryPurging4,
    &UcxEndpointStateTableEntryPurging5,
    &UcxEndpointStateTableEntryPurgingOnControllerReset,
    &UcxEndpointStateTableEntryRejectingEnableStaticStreamsWdfRequest2,
    &UcxEndpointStateTableEntryResettingEndpoint1,
    &UcxEndpointStateTableEntryResettingEndpoint2,
    &UcxEndpointStateTableEntryResumeSMAndWaitForPendingOperation1,
    &UcxEndpointStateTableEntryResumeSMAndWaitForPendingOperation2,
    &UcxEndpointStateTableEntrySetStaleAndDeletePreviousStale,
    &UcxEndpointStateTableEntrySettingEnabledFlag1,
    &UcxEndpointStateTableEntrySettingEnabledFlag2,
    &UcxEndpointStateTableEntryStale,
    &UcxEndpointStateTableEntryStarting1,
    &UcxEndpointStateTableEntryStarting2,
    &UcxEndpointStateTableEntryStarting3,
    &UcxEndpointStateTableEntryWaitingForAbortUrbToBeQueued,
    &UcxEndpointStateTableEntryWaitingForPurgeComplete1,
    &UcxEndpointStateTableEntryWaitingForPurgeComplete2,
};




