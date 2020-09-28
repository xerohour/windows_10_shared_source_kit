
/*++
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !!    DO NOT MODIFY THIS FILE MANUALLY     !!
 !!    This file is created by              !!
 !!    StateMachineConverter.ps1            !!
 !!    in this directory.                   !!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Copyright (c) Microsoft Corporation

Module Name:

StateMachineControllerReset.c

Abstract:

     This header file contains State Machines for ControllerReset
     This has been generated automatically from a visio file.

--*/


#include "pch.h"
#include "StateMachineControllerReset.tmh"
//
// Auto Event Handlers
//
SM_ENGINE_AUTO_EVENT_HANDLER    ControllerResetEventHandler_FailResetFromHub ;
SM_ENGINE_AUTO_EVENT_HANDLER    ControllerResetEventHandler_Ignore ;
SM_ENGINE_AUTO_EVENT_HANDLER    ControllerResetEventHandler_UnblockControllerResetComplete ;





//
// Definitions for State Entry Functions 
//
SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_CallHCDToReset1;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_CallHCDToReset2;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_CompletePendedDeviceMgmtRequests;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_CompleteResetFromHub;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_ControllerFailedAndRHPdoInD0;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_ControllerFailedAndRhPdoInDx;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete1;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete2;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_PrepareForReset1;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_PrepareForReset2;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset1;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset2;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_ResetComplete1;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_ResetComplete2;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_ResetComplete3;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_ResumeRootHubIO1;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_ResumeRootHubIO2;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_RHPdoInD0;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_RhPdoInDx;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_RhPdoInDx_ControllerResetCompleted;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_RhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_RhPdoInDx_PendingControllerReset;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_RhPdoInDx_PendingControllerReset_ControllerResetCompleted;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_RhPdoInDx_PrepareUsbDevicesAndEndpointsForReset;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_RhPdoInDx_SimulatePortChanged1;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_RhPdoInDx_SimulatePortChanged2;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_RhPdoInDx_UnblockControllerResetComplete;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_1;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_2;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_3;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_StartFailingRootHubRequests1;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_StartFailingRootHubRequests2;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_StartPendingDeviceMgmtRequests;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_StopBlockingResetComplete1;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_StopBlockingResetComplete2;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_StopBlockingResetComplete3;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_WaitForResetCompleteFromHCD1;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_WaitForResetCompleteFromHCD2;

SM_ENGINE_STATE_ENTRY_FUNCTION          ControllerResetStateEntryFunc_WaitingForResetFromHub;




//
// State Entries for the states in the State Machine
//
SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_CallHCDToReset1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_CallHCDToReset1[] = {
    { ControllerResetEventResetCompleteFromHCD ,ControllerResetStateNotifyUsbDevicesAndEndpointsOfResetComplete1Index },
    { ControllerResetEventHCDNeedsReset ,ControllerResetStateWaitForResetCompleteFromHCD1Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryCallHCDToReset1 = {
    // State Enum
    ControllerResetStateCallHCDToReset1,
    // State Entry Function
    ControllerResetStateEntryFunc_CallHCDToReset1,
    // State Flags
    SmEngineStateFlagRequiresPassive,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_CallHCDToReset1,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_CallHCDToReset1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_CallHCDToReset2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_CallHCDToReset2[] = {
    { ControllerResetEventResetCompleteFromHCD ,ControllerResetStateNotifyUsbDevicesAndEndpointsOfResetComplete2Index },
    { ControllerResetEventHCDNeedsReset ,ControllerResetStateWaitForResetCompleteFromHCD2Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryCallHCDToReset2 = {
    // State Enum
    ControllerResetStateCallHCDToReset2,
    // State Entry Function
    ControllerResetStateEntryFunc_CallHCDToReset2,
    // State Flags
    SmEngineStateFlagRequiresPassive,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_CallHCDToReset2,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_CallHCDToReset2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_CompletePendedDeviceMgmtRequests[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_CompletePendedDeviceMgmtRequests[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateResumeRootHubIO1Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryCompletePendedDeviceMgmtRequests = {
    // State Enum
    ControllerResetStateCompletePendedDeviceMgmtRequests,
    // State Entry Function
    ControllerResetStateEntryFunc_CompletePendedDeviceMgmtRequests,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_CompletePendedDeviceMgmtRequests,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_CompletePendedDeviceMgmtRequests
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_CompleteResetFromHub[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_CompleteResetFromHub[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateRHPdoInD0Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryCompleteResetFromHub = {
    // State Enum
    ControllerResetStateCompleteResetFromHub,
    // State Entry Function
    ControllerResetStateEntryFunc_CompleteResetFromHub,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_CompleteResetFromHub,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_CompleteResetFromHub
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_ControllerFailedAndRHPdoInD0[] = {
    { ControllerResetEventHCDNeedsReset ,ControllerResetEventHandler_Ignore },
    { ControllerResetEventHubNeedsReset ,ControllerResetEventHandler_FailResetFromHub },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_ControllerFailedAndRHPdoInD0[] = {
    { ControllerResetEventRHPdoWantToExitD0 ,ControllerResetStateSignalOKForRHPdoToExitD0_3Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryControllerFailedAndRHPdoInD0 = {
    // State Enum
    ControllerResetStateControllerFailedAndRHPdoInD0,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagHandlesLowPriEvents|SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_ControllerFailedAndRHPdoInD0,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_ControllerFailedAndRHPdoInD0
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_ControllerFailedAndRhPdoInDx[] = {
    { ControllerResetEventResetCompleteFromHCD ,ControllerResetEventHandler_UnblockControllerResetComplete },
    { ControllerResetEventHCDNeedsReset ,ControllerResetEventHandler_Ignore },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_ControllerFailedAndRhPdoInDx[] = {
    { ControllerResetEventRHPdoEnteredD0 ,ControllerResetStateStopBlockingResetComplete3Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryControllerFailedAndRhPdoInDx = {
    // State Enum
    ControllerResetStateControllerFailedAndRhPdoInDx,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagHandlesLowPriEvents|SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_ControllerFailedAndRhPdoInDx,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_ControllerFailedAndRhPdoInDx
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_NotifyUsbDevicesAndEndpointsOfResetComplete1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_NotifyUsbDevicesAndEndpointsOfResetComplete1[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateWaitingForResetFromHubIndex },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryNotifyUsbDevicesAndEndpointsOfResetComplete1 = {
    // State Enum
    ControllerResetStateNotifyUsbDevicesAndEndpointsOfResetComplete1,
    // State Entry Function
    ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_NotifyUsbDevicesAndEndpointsOfResetComplete1,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_NotifyUsbDevicesAndEndpointsOfResetComplete1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_NotifyUsbDevicesAndEndpointsOfResetComplete2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_NotifyUsbDevicesAndEndpointsOfResetComplete2[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateCompletePendedDeviceMgmtRequestsIndex },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryNotifyUsbDevicesAndEndpointsOfResetComplete2 = {
    // State Enum
    ControllerResetStateNotifyUsbDevicesAndEndpointsOfResetComplete2,
    // State Entry Function
    ControllerResetStateEntryFunc_NotifyUsbDevicesAndEndpointsOfResetComplete2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_NotifyUsbDevicesAndEndpointsOfResetComplete2,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_NotifyUsbDevicesAndEndpointsOfResetComplete2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_PrepareForReset1[] = {
    { ControllerResetEventHCDNeedsReset ,ControllerResetEventHandler_Ignore },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_PrepareForReset1[] = {
    { ControllerResetEventNoActiveResetReferences ,ControllerResetStatePrepareUsbDevicesAndEndpointsForReset1Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryPrepareForReset1 = {
    // State Enum
    ControllerResetStatePrepareForReset1,
    // State Entry Function
    ControllerResetStateEntryFunc_PrepareForReset1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_PrepareForReset1,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_PrepareForReset1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_PrepareForReset2[] = {
    { ControllerResetEventHCDNeedsReset ,ControllerResetEventHandler_Ignore },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_PrepareForReset2[] = {
    { ControllerResetEventNoActiveResetReferences ,ControllerResetStatePrepareUsbDevicesAndEndpointsForReset2Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryPrepareForReset2 = {
    // State Enum
    ControllerResetStatePrepareForReset2,
    // State Entry Function
    ControllerResetStateEntryFunc_PrepareForReset2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_PrepareForReset2,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_PrepareForReset2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_PrepareUsbDevicesAndEndpointsForReset1[] = {
    { ControllerResetEventHCDNeedsReset ,ControllerResetEventHandler_Ignore },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_PrepareUsbDevicesAndEndpointsForReset1[] = {
    { ControllerResetEventUsbDevicesAndEndpointsReadyForControllerReset ,ControllerResetStateCallHCDToReset1Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryPrepareUsbDevicesAndEndpointsForReset1 = {
    // State Enum
    ControllerResetStatePrepareUsbDevicesAndEndpointsForReset1,
    // State Entry Function
    ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_PrepareUsbDevicesAndEndpointsForReset1,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_PrepareUsbDevicesAndEndpointsForReset1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_PrepareUsbDevicesAndEndpointsForReset2[] = {
    { ControllerResetEventHCDNeedsReset ,ControllerResetEventHandler_Ignore },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_PrepareUsbDevicesAndEndpointsForReset2[] = {
    { ControllerResetEventUsbDevicesAndEndpointsReadyForControllerReset ,ControllerResetStateCallHCDToReset2Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryPrepareUsbDevicesAndEndpointsForReset2 = {
    // State Enum
    ControllerResetStatePrepareUsbDevicesAndEndpointsForReset2,
    // State Entry Function
    ControllerResetStateEntryFunc_PrepareUsbDevicesAndEndpointsForReset2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_PrepareUsbDevicesAndEndpointsForReset2,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_PrepareUsbDevicesAndEndpointsForReset2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_ResetComplete1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_ResetComplete1[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateCompleteResetFromHubIndex },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryResetComplete1 = {
    // State Enum
    ControllerResetStateResetComplete1,
    // State Entry Function
    ControllerResetStateEntryFunc_ResetComplete1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_ResetComplete1,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_ResetComplete1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_ResetComplete2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_ResetComplete2[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateSignalOKForRHPdoToExitD0_2Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryResetComplete2 = {
    // State Enum
    ControllerResetStateResetComplete2,
    // State Entry Function
    ControllerResetStateEntryFunc_ResetComplete2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_ResetComplete2,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_ResetComplete2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_ResetComplete3[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_ResetComplete3[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateStartFailingRootHubRequests1Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryResetComplete3 = {
    // State Enum
    ControllerResetStateResetComplete3,
    // State Entry Function
    ControllerResetStateEntryFunc_ResetComplete3,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_ResetComplete3,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_ResetComplete3
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_ResumeRootHubIO1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_ResumeRootHubIO1[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateResetComplete1Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryResumeRootHubIO1 = {
    // State Enum
    ControllerResetStateResumeRootHubIO1,
    // State Entry Function
    ControllerResetStateEntryFunc_ResumeRootHubIO1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_ResumeRootHubIO1,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_ResumeRootHubIO1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_ResumeRootHubIO2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_ResumeRootHubIO2[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateResetComplete2Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryResumeRootHubIO2 = {
    // State Enum
    ControllerResetStateResumeRootHubIO2,
    // State Entry Function
    ControllerResetStateEntryFunc_ResumeRootHubIO2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_ResumeRootHubIO2,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_ResumeRootHubIO2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_RHPdoInD0[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_RHPdoInD0[] = {
    { ControllerResetEventControllerFailed ,ControllerResetStateStartFailingRootHubRequests1Index },
    { ControllerResetEventHCDNeedsReset ,ControllerResetStatePrepareForReset1Index },
    { ControllerResetEventHubNeedsReset ,ControllerResetStatePrepareForReset2Index },
    { ControllerResetEventRHPdoWantToExitD0 ,ControllerResetStateSignalOKForRHPdoToExitD0_1Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryRHPdoInD0 = {
    // State Enum
    ControllerResetStateRHPdoInD0,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagHandlesLowPriEvents|SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_RHPdoInD0,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_RHPdoInD0
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_RhPdoInDx[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_RhPdoInDx[] = {
    { ControllerResetEventResetCompleteFromHCD ,ControllerResetStateRhPdoInDx_PrepareUsbDevicesAndEndpointsForResetIndex },
    { ControllerResetEventHCDNeedsReset ,ControllerResetStateRhPdoInDx_SimulatePortChanged1Index },
    { ControllerResetEventRHPdoEnteredD0 ,ControllerResetStateStopBlockingResetComplete1Index },
    { ControllerResetEventControllerFailed ,ControllerResetStateStartFailingRootHubRequests2Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryRhPdoInDx = {
    // State Enum
    ControllerResetStateRhPdoInDx,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagHandlesLowPriEvents|SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_RhPdoInDx,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_RhPdoInDx
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_RhPdoInDx_ControllerResetCompleted[] = {
    { ControllerResetEventResetCompleteFromHCD ,ControllerResetEventHandler_UnblockControllerResetComplete },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_RhPdoInDx_ControllerResetCompleted[] = {
    { ControllerResetEventHCDNeedsReset ,ControllerResetStateRhPdoInDx_SimulatePortChanged2Index },
    { ControllerResetEventControllerFailed ,ControllerResetStateStartFailingRootHubRequests2Index },
    { ControllerResetEventRHPdoEnteredD0 ,ControllerResetStateStopBlockingResetComplete1Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryRhPdoInDx_ControllerResetCompleted = {
    // State Enum
    ControllerResetStateRhPdoInDx_ControllerResetCompleted,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagHandlesLowPriEvents|SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_RhPdoInDx_ControllerResetCompleted,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_RhPdoInDx_ControllerResetCompleted
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_RhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_RhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateRhPdoInDx_UnblockControllerResetCompleteIndex },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryRhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete = {
    // State Enum
    ControllerResetStateRhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete,
    // State Entry Function
    ControllerResetStateEntryFunc_RhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_RhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_RhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_RhPdoInDx_PendingControllerReset[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_RhPdoInDx_PendingControllerReset[] = {
    { ControllerResetEventResetCompleteFromHCD ,ControllerResetStateRhPdoInDx_PrepareUsbDevicesAndEndpointsForResetIndex },
    { ControllerResetEventRHPdoEnteredD0 ,ControllerResetStateStopBlockingResetComplete2Index },
    { ControllerResetEventControllerFailed ,ControllerResetStateStartFailingRootHubRequests2Index },
    { ControllerResetEventHCDNeedsReset ,ControllerResetStateRhPdoInDx_SimulatePortChanged1Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryRhPdoInDx_PendingControllerReset = {
    // State Enum
    ControllerResetStateRhPdoInDx_PendingControllerReset,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagHandlesLowPriEvents|SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_RhPdoInDx_PendingControllerReset,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_RhPdoInDx_PendingControllerReset
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_RhPdoInDx_PendingControllerReset_ControllerResetCompleted[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_RhPdoInDx_PendingControllerReset_ControllerResetCompleted[] = {
    { ControllerResetEventResetCompleteFromHCD ,ControllerResetStateRhPdoInDx_UnblockControllerResetCompleteIndex },
    { ControllerResetEventControllerFailed ,ControllerResetStateStartFailingRootHubRequests2Index },
    { ControllerResetEventRHPdoEnteredD0 ,ControllerResetStateStopBlockingResetComplete2Index },
    { ControllerResetEventHCDNeedsReset ,ControllerResetStateRhPdoInDx_SimulatePortChanged2Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryRhPdoInDx_PendingControllerReset_ControllerResetCompleted = {
    // State Enum
    ControllerResetStateRhPdoInDx_PendingControllerReset_ControllerResetCompleted,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagHandlesLowPriEvents|SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_RhPdoInDx_PendingControllerReset_ControllerResetCompleted,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_RhPdoInDx_PendingControllerReset_ControllerResetCompleted
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_RhPdoInDx_PrepareUsbDevicesAndEndpointsForReset[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_RhPdoInDx_PrepareUsbDevicesAndEndpointsForReset[] = {
    { ControllerResetEventUsbDevicesAndEndpointsReadyForControllerReset ,ControllerResetStateRhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetCompleteIndex },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryRhPdoInDx_PrepareUsbDevicesAndEndpointsForReset = {
    // State Enum
    ControllerResetStateRhPdoInDx_PrepareUsbDevicesAndEndpointsForReset,
    // State Entry Function
    ControllerResetStateEntryFunc_RhPdoInDx_PrepareUsbDevicesAndEndpointsForReset,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_RhPdoInDx_PrepareUsbDevicesAndEndpointsForReset,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_RhPdoInDx_PrepareUsbDevicesAndEndpointsForReset
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_RhPdoInDx_SimulatePortChanged1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_RhPdoInDx_SimulatePortChanged1[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateRhPdoInDx_PendingControllerResetIndex },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryRhPdoInDx_SimulatePortChanged1 = {
    // State Enum
    ControllerResetStateRhPdoInDx_SimulatePortChanged1,
    // State Entry Function
    ControllerResetStateEntryFunc_RhPdoInDx_SimulatePortChanged1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_RhPdoInDx_SimulatePortChanged1,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_RhPdoInDx_SimulatePortChanged1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_RhPdoInDx_SimulatePortChanged2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_RhPdoInDx_SimulatePortChanged2[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateRhPdoInDx_PendingControllerReset_ControllerResetCompletedIndex },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryRhPdoInDx_SimulatePortChanged2 = {
    // State Enum
    ControllerResetStateRhPdoInDx_SimulatePortChanged2,
    // State Entry Function
    ControllerResetStateEntryFunc_RhPdoInDx_SimulatePortChanged2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_RhPdoInDx_SimulatePortChanged2,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_RhPdoInDx_SimulatePortChanged2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_RhPdoInDx_UnblockControllerResetComplete[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_RhPdoInDx_UnblockControllerResetComplete[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateRhPdoInDx_ControllerResetCompletedIndex },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryRhPdoInDx_UnblockControllerResetComplete = {
    // State Enum
    ControllerResetStateRhPdoInDx_UnblockControllerResetComplete,
    // State Entry Function
    ControllerResetStateEntryFunc_RhPdoInDx_UnblockControllerResetComplete,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_RhPdoInDx_UnblockControllerResetComplete,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_RhPdoInDx_UnblockControllerResetComplete
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_SignalOKForRHPdoToExitD0_1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_SignalOKForRHPdoToExitD0_1[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateRhPdoInDxIndex },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntrySignalOKForRHPdoToExitD0_1 = {
    // State Enum
    ControllerResetStateSignalOKForRHPdoToExitD0_1,
    // State Entry Function
    ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_SignalOKForRHPdoToExitD0_1,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_SignalOKForRHPdoToExitD0_1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_SignalOKForRHPdoToExitD0_2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_SignalOKForRHPdoToExitD0_2[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateRhPdoInDx_ControllerResetCompletedIndex },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntrySignalOKForRHPdoToExitD0_2 = {
    // State Enum
    ControllerResetStateSignalOKForRHPdoToExitD0_2,
    // State Entry Function
    ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_SignalOKForRHPdoToExitD0_2,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_SignalOKForRHPdoToExitD0_2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_SignalOKForRHPdoToExitD0_3[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_SignalOKForRHPdoToExitD0_3[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateControllerFailedAndRhPdoInDxIndex },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntrySignalOKForRHPdoToExitD0_3 = {
    // State Enum
    ControllerResetStateSignalOKForRHPdoToExitD0_3,
    // State Entry Function
    ControllerResetStateEntryFunc_SignalOKForRHPdoToExitD0_3,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_SignalOKForRHPdoToExitD0_3,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_SignalOKForRHPdoToExitD0_3
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_StartFailingRootHubRequests1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_StartFailingRootHubRequests1[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateControllerFailedAndRHPdoInD0Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryStartFailingRootHubRequests1 = {
    // State Enum
    ControllerResetStateStartFailingRootHubRequests1,
    // State Entry Function
    ControllerResetStateEntryFunc_StartFailingRootHubRequests1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_StartFailingRootHubRequests1,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_StartFailingRootHubRequests1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_StartFailingRootHubRequests2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_StartFailingRootHubRequests2[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateControllerFailedAndRhPdoInDxIndex },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryStartFailingRootHubRequests2 = {
    // State Enum
    ControllerResetStateStartFailingRootHubRequests2,
    // State Entry Function
    ControllerResetStateEntryFunc_StartFailingRootHubRequests2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_StartFailingRootHubRequests2,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_StartFailingRootHubRequests2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_StartPendingDeviceMgmtRequests[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_StartPendingDeviceMgmtRequests[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStatePrepareUsbDevicesAndEndpointsForReset1Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryStartPendingDeviceMgmtRequests = {
    // State Enum
    ControllerResetStateStartPendingDeviceMgmtRequests,
    // State Entry Function
    ControllerResetStateEntryFunc_StartPendingDeviceMgmtRequests,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_StartPendingDeviceMgmtRequests,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_StartPendingDeviceMgmtRequests
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_StopBlockingResetComplete1[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_StopBlockingResetComplete1[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateRHPdoInD0Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryStopBlockingResetComplete1 = {
    // State Enum
    ControllerResetStateStopBlockingResetComplete1,
    // State Entry Function
    ControllerResetStateEntryFunc_StopBlockingResetComplete1,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_StopBlockingResetComplete1,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_StopBlockingResetComplete1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_StopBlockingResetComplete2[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_StopBlockingResetComplete2[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStatePrepareForReset1Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryStopBlockingResetComplete2 = {
    // State Enum
    ControllerResetStateStopBlockingResetComplete2,
    // State Entry Function
    ControllerResetStateEntryFunc_StopBlockingResetComplete2,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_StopBlockingResetComplete2,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_StopBlockingResetComplete2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_StopBlockingResetComplete3[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_StopBlockingResetComplete3[] = {
    { ControllerResetEventOperationSuccess ,ControllerResetStateControllerFailedAndRHPdoInD0Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryStopBlockingResetComplete3 = {
    // State Enum
    ControllerResetStateStopBlockingResetComplete3,
    // State Entry Function
    ControllerResetStateEntryFunc_StopBlockingResetComplete3,
    // State Flags
    SmEngineStateFlagNone,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_StopBlockingResetComplete3,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_StopBlockingResetComplete3
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_WaitForResetCompleteFromHCD1[] = {
    { ControllerResetEventHCDNeedsReset ,ControllerResetEventHandler_Ignore },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_WaitForResetCompleteFromHCD1[] = {
    { ControllerResetEventResetCompleteFromHCD ,ControllerResetStateCallHCDToReset1Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryWaitForResetCompleteFromHCD1 = {
    // State Enum
    ControllerResetStateWaitForResetCompleteFromHCD1,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_WaitForResetCompleteFromHCD1,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_WaitForResetCompleteFromHCD1
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_WaitForResetCompleteFromHCD2[] = {
    { ControllerResetEventHCDNeedsReset ,ControllerResetEventHandler_Ignore },
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_WaitForResetCompleteFromHCD2[] = {
    { ControllerResetEventResetCompleteFromHCD ,ControllerResetStateCallHCDToReset2Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryWaitForResetCompleteFromHCD2 = {
    // State Enum
    ControllerResetStateWaitForResetCompleteFromHCD2,
    // State Entry Function
    NULL,
    // State Flags
    SmEngineStateFlagNoStateEntryFunction,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_WaitForResetCompleteFromHCD2,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_WaitForResetCompleteFromHCD2
};



SM_ENGINE_AUTO_EVENT_TRANSITION   ControllerResetAutoEventTransitions_WaitingForResetFromHub[] = {
    { SmEngineEventNull ,           NULL },
};


SM_ENGINE_STATE_TRANSITION   ControllerResetStateTransitions_WaitingForResetFromHub[] = {
    { ControllerResetEventHCDNeedsReset ,ControllerResetStateStartPendingDeviceMgmtRequestsIndex },
    { ControllerResetEventControllerFailed ,ControllerResetStateResetComplete3Index },
    { ControllerResetEventRHPdoWantToExitD0 ,ControllerResetStateResumeRootHubIO2Index },
    { ControllerResetEventHubNeedsReset ,ControllerResetStateResumeRootHubIO1Index },
    { SmEngineEventNull ,           ControllerResetStateNullIndex },
};


SM_ENGINE_STATE_TABLE_ENTRY   ControllerResetStateTableEntryWaitingForResetFromHub = {
    // State Enum
    ControllerResetStateWaitingForResetFromHub,
    // State Entry Function
    ControllerResetStateEntryFunc_WaitingForResetFromHub,
    // State Flags
    SmEngineStateFlagHandlesLowPriEvents,
    // Auto Event Handlers
    ControllerResetAutoEventTransitions_WaitingForResetFromHub,
    // Event State Pairs for Transitions
    ControllerResetStateTransitions_WaitingForResetFromHub
};










//
// Global List of States in all the State Machines
//
PSM_ENGINE_STATE_TABLE_ENTRY    ControllerResetStateTable[] =
{
    &ControllerResetStateTableEntryCallHCDToReset1,
    &ControllerResetStateTableEntryCallHCDToReset2,
    &ControllerResetStateTableEntryCompletePendedDeviceMgmtRequests,
    &ControllerResetStateTableEntryCompleteResetFromHub,
    &ControllerResetStateTableEntryControllerFailedAndRHPdoInD0,
    &ControllerResetStateTableEntryControllerFailedAndRhPdoInDx,
    &ControllerResetStateTableEntryNotifyUsbDevicesAndEndpointsOfResetComplete1,
    &ControllerResetStateTableEntryNotifyUsbDevicesAndEndpointsOfResetComplete2,
    &ControllerResetStateTableEntryPrepareForReset1,
    &ControllerResetStateTableEntryPrepareForReset2,
    &ControllerResetStateTableEntryPrepareUsbDevicesAndEndpointsForReset1,
    &ControllerResetStateTableEntryPrepareUsbDevicesAndEndpointsForReset2,
    &ControllerResetStateTableEntryResetComplete1,
    &ControllerResetStateTableEntryResetComplete2,
    &ControllerResetStateTableEntryResetComplete3,
    &ControllerResetStateTableEntryResumeRootHubIO1,
    &ControllerResetStateTableEntryResumeRootHubIO2,
    &ControllerResetStateTableEntryRHPdoInD0,
    &ControllerResetStateTableEntryRhPdoInDx,
    &ControllerResetStateTableEntryRhPdoInDx_ControllerResetCompleted,
    &ControllerResetStateTableEntryRhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete,
    &ControllerResetStateTableEntryRhPdoInDx_PendingControllerReset,
    &ControllerResetStateTableEntryRhPdoInDx_PendingControllerReset_ControllerResetCompleted,
    &ControllerResetStateTableEntryRhPdoInDx_PrepareUsbDevicesAndEndpointsForReset,
    &ControllerResetStateTableEntryRhPdoInDx_SimulatePortChanged1,
    &ControllerResetStateTableEntryRhPdoInDx_SimulatePortChanged2,
    &ControllerResetStateTableEntryRhPdoInDx_UnblockControllerResetComplete,
    &ControllerResetStateTableEntrySignalOKForRHPdoToExitD0_1,
    &ControllerResetStateTableEntrySignalOKForRHPdoToExitD0_2,
    &ControllerResetStateTableEntrySignalOKForRHPdoToExitD0_3,
    &ControllerResetStateTableEntryStartFailingRootHubRequests1,
    &ControllerResetStateTableEntryStartFailingRootHubRequests2,
    &ControllerResetStateTableEntryStartPendingDeviceMgmtRequests,
    &ControllerResetStateTableEntryStopBlockingResetComplete1,
    &ControllerResetStateTableEntryStopBlockingResetComplete2,
    &ControllerResetStateTableEntryStopBlockingResetComplete3,
    &ControllerResetStateTableEntryWaitForResetCompleteFromHCD1,
    &ControllerResetStateTableEntryWaitForResetCompleteFromHCD2,
    &ControllerResetStateTableEntryWaitingForResetFromHub,
};




