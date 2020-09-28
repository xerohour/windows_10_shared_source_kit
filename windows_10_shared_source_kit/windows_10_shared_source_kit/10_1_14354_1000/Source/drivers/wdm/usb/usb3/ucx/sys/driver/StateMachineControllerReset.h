
/*++
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !!    DO NOT MODIFY THIS FILE MANUALLY     !!
 !!    This file is created by              !!
 !!    StateMachineConverter.ps1            !!
 !!    in this directory.                   !!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Copyright (c) Microsoft Corporation

Module Name:

StateMachineControllerReset.h

Abstract:

     This header file contains State Machines for ControllerReset
     This has been generated automatically from a visio file.

--*/


#pragma once

//
// Index of States in all the State Machines
//
enum _CONTROLLER_RESET_STATE_INDEX {
    ControllerResetStateCallHCDToReset1Index,
    ControllerResetStateCallHCDToReset2Index,
    ControllerResetStateCompletePendedDeviceMgmtRequestsIndex,
    ControllerResetStateCompleteResetFromHubIndex,
    ControllerResetStateControllerFailedAndRHPdoInD0Index,
    ControllerResetStateControllerFailedAndRhPdoInDxIndex,
    ControllerResetStateNotifyUsbDevicesAndEndpointsOfResetComplete1Index,
    ControllerResetStateNotifyUsbDevicesAndEndpointsOfResetComplete2Index,
    ControllerResetStatePrepareForReset1Index,
    ControllerResetStatePrepareForReset2Index,
    ControllerResetStatePrepareUsbDevicesAndEndpointsForReset1Index,
    ControllerResetStatePrepareUsbDevicesAndEndpointsForReset2Index,
    ControllerResetStateResetComplete1Index,
    ControllerResetStateResetComplete2Index,
    ControllerResetStateResetComplete3Index,
    ControllerResetStateResumeRootHubIO1Index,
    ControllerResetStateResumeRootHubIO2Index,
    ControllerResetStateRHPdoInD0Index,
    ControllerResetStateRhPdoInDxIndex,
    ControllerResetStateRhPdoInDx_ControllerResetCompletedIndex,
    ControllerResetStateRhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetCompleteIndex,
    ControllerResetStateRhPdoInDx_PendingControllerResetIndex,
    ControllerResetStateRhPdoInDx_PendingControllerReset_ControllerResetCompletedIndex,
    ControllerResetStateRhPdoInDx_PrepareUsbDevicesAndEndpointsForResetIndex,
    ControllerResetStateRhPdoInDx_SimulatePortChanged1Index,
    ControllerResetStateRhPdoInDx_SimulatePortChanged2Index,
    ControllerResetStateRhPdoInDx_UnblockControllerResetCompleteIndex,
    ControllerResetStateSignalOKForRHPdoToExitD0_1Index,
    ControllerResetStateSignalOKForRHPdoToExitD0_2Index,
    ControllerResetStateSignalOKForRHPdoToExitD0_3Index,
    ControllerResetStateStartFailingRootHubRequests1Index,
    ControllerResetStateStartFailingRootHubRequests2Index,
    ControllerResetStateStartPendingDeviceMgmtRequestsIndex,
    ControllerResetStateStopBlockingResetComplete1Index,
    ControllerResetStateStopBlockingResetComplete2Index,
    ControllerResetStateStopBlockingResetComplete3Index,
    ControllerResetStateWaitForResetCompleteFromHCD1Index,
    ControllerResetStateWaitForResetCompleteFromHCD2Index,
    ControllerResetStateWaitingForResetFromHubIndex,
    ControllerResetStateNullIndex
} CONTROLLER_RESET_STATE_INDEX;
//
// Enum of States in all the State Machines
//
#define CONTROLLER_RESET_STATE\
    ControllerResetStateCallHCDToReset1=0x57a74001,\
    ControllerResetStateCallHCDToReset2=0x57a74002,\
    ControllerResetStateCompletePendedDeviceMgmtRequests=0x57a74003,\
    ControllerResetStateCompleteResetFromHub=0x57a74004,\
    ControllerResetStateControllerFailedAndRHPdoInD0=0x57a74022,\
    ControllerResetStateControllerFailedAndRhPdoInDx=0x57a74023,\
    ControllerResetStateNotifyUsbDevicesAndEndpointsOfResetComplete1=0x57a74005,\
    ControllerResetStateNotifyUsbDevicesAndEndpointsOfResetComplete2=0x57a74006,\
    ControllerResetStatePrepareForReset1=0x57a74007,\
    ControllerResetStatePrepareForReset2=0x57a74008,\
    ControllerResetStatePrepareUsbDevicesAndEndpointsForReset1=0x57a74009,\
    ControllerResetStatePrepareUsbDevicesAndEndpointsForReset2=0x57a7400a,\
    ControllerResetStateResetComplete1=0x57a7400b,\
    ControllerResetStateResetComplete2=0x57a7400c,\
    ControllerResetStateResetComplete3=0x57a74024,\
    ControllerResetStateResumeRootHubIO1=0x57a7400d,\
    ControllerResetStateResumeRootHubIO2=0x57a7400e,\
    ControllerResetStateRHPdoInD0=0x57a7400f,\
    ControllerResetStateRhPdoInDx=0x57a74010,\
    ControllerResetStateRhPdoInDx_ControllerResetCompleted=0x57a74011,\
    ControllerResetStateRhPdoInDx_NotifyUsbDevicesAndEndpointsOfResetComplete=0x57a74012,\
    ControllerResetStateRhPdoInDx_PendingControllerReset=0x57a74013,\
    ControllerResetStateRhPdoInDx_PendingControllerReset_ControllerResetCompleted=0x57a74014,\
    ControllerResetStateRhPdoInDx_PrepareUsbDevicesAndEndpointsForReset=0x57a74015,\
    ControllerResetStateRhPdoInDx_SimulatePortChanged1=0x57a74020,\
    ControllerResetStateRhPdoInDx_SimulatePortChanged2=0x57a74021,\
    ControllerResetStateRhPdoInDx_UnblockControllerResetComplete=0x57a74017,\
    ControllerResetStateSignalOKForRHPdoToExitD0_1=0x57a74018,\
    ControllerResetStateSignalOKForRHPdoToExitD0_2=0x57a74019,\
    ControllerResetStateSignalOKForRHPdoToExitD0_3=0x57a74025,\
    ControllerResetStateStartFailingRootHubRequests1=0x57a74029,\
    ControllerResetStateStartFailingRootHubRequests2=0x57a7402a,\
    ControllerResetStateStartPendingDeviceMgmtRequests=0x57a7401a,\
    ControllerResetStateStopBlockingResetComplete1=0x57a7401b,\
    ControllerResetStateStopBlockingResetComplete2=0x57a7401c,\
    ControllerResetStateStopBlockingResetComplete3=0x57a74028,\
    ControllerResetStateWaitForResetCompleteFromHCD1=0x57a7401d,\
    ControllerResetStateWaitForResetCompleteFromHCD2=0x57a7401e,\
    ControllerResetStateWaitingForResetFromHub=0x57a7401f,\
    ControllerResetStateNull,\



//
// Event List For the State Machines
//
#define CONTROLLER_RESET_EVENT\
    ControllerResetEventControllerFailed                                 = 0xebe40090 | SmEngineEventPriorityLow,\
    ControllerResetEventHCDNeedsReset                                    = 0xebe40010 | SmEngineEventPriorityNormal,\
    ControllerResetEventHubNeedsReset                                    = 0xebe40020 | SmEngineEventPriorityLow,\
    ControllerResetEventNoActiveResetReferences                          = 0xebe40030 | SmEngineEventPriorityNormal,\
    ControllerResetEventOperationSuccess                                 = 0xebe40040 | SmEngineEventPrioritySync,\
    ControllerResetEventResetCompleteFromHCD                             = 0xebe40050 | SmEngineEventPriorityNormal,\
    ControllerResetEventRHPdoEnteredD0                                   = 0xebe40060 | SmEngineEventPriorityLow,\
    ControllerResetEventRHPdoWantToExitD0                                = 0xebe40070 | SmEngineEventPriorityLow,\
    ControllerResetEventUsbDevicesAndEndpointsReadyForControllerReset    = 0xebe40080 | SmEngineEventPriorityNormal,\


typedef struct _SM_ENGINE_STATE_TABLE_ENTRY *PSM_ENGINE_STATE_TABLE_ENTRY;
extern PSM_ENGINE_STATE_TABLE_ENTRY ControllerResetStateTable[];
