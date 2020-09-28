
/*++
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !!    DO NOT MODIFY THIS FILE MANUALLY     !!
 !!    This file is created by              !!
 !!    StateMachineConverter.ps1            !!
 !!    in this directory.                   !!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Copyright (c) Microsoft Corporation

Module Name:

StateMachineUcxEndpoint.h

Abstract:

     This header file contains State Machines for UcxEndpoint
     This has been generated automatically from a visio file.

--*/


#pragma once

//
// Index of States in all the State Machines
//
enum _UCXENDPOINT_STATE_INDEX {
    UcxEndpointStateAborting1Index,
    UcxEndpointStateAborting2Index,
    UcxEndpointStateClearingEnabledFlagIndex,
    UcxEndpointStateCompletingAbortUrbIfNotAlreadyCancelled1Index,
    UcxEndpointStateCompletingAbortUrbIfNotAlreadyCancelled2Index,
    UcxEndpointStateCompletingDisableStaticStreamsIrp1Index,
    UcxEndpointStateCompletingDisableStaticStreamsIrp2Index,
    UcxEndpointStateCompletingDisableStaticStreamsIrp4Index,
    UcxEndpointStateCompletingDisableStaticStreamsWdfRequest2Index,
    UcxEndpointStateCompletingEnableStaticStreamsIrp1Index,
    UcxEndpointStateCompletingEnableStaticStreamsIrp2Index,
    UcxEndpointStateCompletingEndpointResetIrp1Index,
    UcxEndpointStateCompletingEndpointResetIrp2Index,
    UcxEndpointStateCompletingEndpointResetIrp4Index,
    UcxEndpointStateCompletingPendingOperation1Index,
    UcxEndpointStateCompletingPendingOperation2Index,
    UcxEndpointStateCompletingPendingOperation3Index,
    UcxEndpointStateCompletingPendingOperation4Index,
    UcxEndpointStateCompletingPendingTreePurgeIndex,
    UcxEndpointStateCreatedIndex,
    UcxEndpointStateDeletingIndex,
    UcxEndpointStateDeletingStaleIndex,
    UcxEndpointStateDisablingStaticStreamsIndex,
    UcxEndpointStateDisablingStaticStreams2Index,
    UcxEndpointStateEnabledIndex,
    UcxEndpointStateEnablingStaticStreamsIndex,
    UcxEndpointStateFailingEnableStaticStreamsIrpIndex,
    UcxEndpointStateFailingEndpointResetWdfRequestIndex,
    UcxEndpointStateIsAbleToStart1Index,
    UcxEndpointStateIsAbleToStart2Index,
    UcxEndpointStateIsDeprogrammedDueToControllerReset4Index,
    UcxEndpointStateIsDeprogrammedDueToControllerResetOrDisconnectedIndex,
    UcxEndpointStateNeedToSetStaleIndex,
    UcxEndpointStateOkToResetControllerAndPauseSM1Index,
    UcxEndpointStateOkToResetControllerAndPauseSM2Index,
    UcxEndpointStatePendingDestroyIndex,
    UcxEndpointStatePurgedOrDisabledIndex,
    UcxEndpointStatePurging1Index,
    UcxEndpointStatePurging2Index,
    UcxEndpointStatePurging3Index,
    UcxEndpointStatePurging4Index,
    UcxEndpointStatePurging5Index,
    UcxEndpointStatePurgingOnControllerResetIndex,
    UcxEndpointStateRejectingEnableStaticStreamsWdfRequest2Index,
    UcxEndpointStateResettingEndpoint1Index,
    UcxEndpointStateResettingEndpoint2Index,
    UcxEndpointStateResumeSMAndWaitForPendingOperation1Index,
    UcxEndpointStateResumeSMAndWaitForPendingOperation2Index,
    UcxEndpointStateSetStaleAndDeletePreviousStaleIndex,
    UcxEndpointStateSettingEnabledFlag1Index,
    UcxEndpointStateSettingEnabledFlag2Index,
    UcxEndpointStateStaleIndex,
    UcxEndpointStateStarting1Index,
    UcxEndpointStateStarting2Index,
    UcxEndpointStateStarting3Index,
    UcxEndpointStateWaitingForAbortUrbToBeQueuedIndex,
    UcxEndpointStateWaitingForPurgeComplete1Index,
    UcxEndpointStateWaitingForPurgeComplete2Index,
    UcxEndpointStateNullIndex
} UCXENDPOINT_STATE_INDEX;
//
// Enum of States in all the State Machines
//
#define UCXENDPOINT_STATE\
    UcxEndpointStateAborting1=0x57a73050,\
    UcxEndpointStateAborting2=0x57a73051,\
    UcxEndpointStateClearingEnabledFlag=0x57a7304c,\
    UcxEndpointStateCompletingAbortUrbIfNotAlreadyCancelled1=0x57a73002,\
    UcxEndpointStateCompletingAbortUrbIfNotAlreadyCancelled2=0x57a73003,\
    UcxEndpointStateCompletingDisableStaticStreamsIrp1=0x57a73004,\
    UcxEndpointStateCompletingDisableStaticStreamsIrp2=0x57a73005,\
    UcxEndpointStateCompletingDisableStaticStreamsIrp4=0x57a73007,\
    UcxEndpointStateCompletingDisableStaticStreamsWdfRequest2=0x57a73008,\
    UcxEndpointStateCompletingEnableStaticStreamsIrp1=0x57a7300a,\
    UcxEndpointStateCompletingEnableStaticStreamsIrp2=0x57a7300b,\
    UcxEndpointStateCompletingEndpointResetIrp1=0x57a7300d,\
    UcxEndpointStateCompletingEndpointResetIrp2=0x57a7300e,\
    UcxEndpointStateCompletingEndpointResetIrp4=0x57a73010,\
    UcxEndpointStateCompletingPendingOperation1=0x57a73040,\
    UcxEndpointStateCompletingPendingOperation2=0x57a73014,\
    UcxEndpointStateCompletingPendingOperation3=0x57a73041,\
    UcxEndpointStateCompletingPendingOperation4=0x57a73042,\
    UcxEndpointStateCompletingPendingTreePurge=0x57a73046,\
    UcxEndpointStateCreated=0x57a73016,\
    UcxEndpointStateDeleting=0x57a73017,\
    UcxEndpointStateDeletingStale=0x57a73018,\
    UcxEndpointStateDisablingStaticStreams=0x57a73019,\
    UcxEndpointStateDisablingStaticStreams2=0x57a73039,\
    UcxEndpointStateEnabled=0x57a7301c,\
    UcxEndpointStateEnablingStaticStreams=0x57a7301d,\
    UcxEndpointStateFailingEnableStaticStreamsIrp=0x57a7301f,\
    UcxEndpointStateFailingEndpointResetWdfRequest=0x57a73047,\
    UcxEndpointStateIsAbleToStart1=0x57a73043,\
    UcxEndpointStateIsAbleToStart2=0x57a73044,\
    UcxEndpointStateIsDeprogrammedDueToControllerReset4=0x57a73023,\
    UcxEndpointStateIsDeprogrammedDueToControllerResetOrDisconnected=0x57a73048,\
    UcxEndpointStateNeedToSetStale=0x57a73024,\
    UcxEndpointStateOkToResetControllerAndPauseSM1=0x57a73053,\
    UcxEndpointStateOkToResetControllerAndPauseSM2=0x57a73054,\
    UcxEndpointStatePendingDestroy=0x57a73049,\
    UcxEndpointStatePurgedOrDisabled=0x57a7304f,\
    UcxEndpointStatePurging1=0x57a73027,\
    UcxEndpointStatePurging2=0x57a73028,\
    UcxEndpointStatePurging3=0x57a7303b,\
    UcxEndpointStatePurging4=0x57a7303c,\
    UcxEndpointStatePurging5=0x57a73045,\
    UcxEndpointStatePurgingOnControllerReset=0x57a73029,\
    UcxEndpointStateRejectingEnableStaticStreamsWdfRequest2=0x57a7302b,\
    UcxEndpointStateResettingEndpoint1=0x57a7302d,\
    UcxEndpointStateResettingEndpoint2=0x57a7302e,\
    UcxEndpointStateResumeSMAndWaitForPendingOperation1=0x57a73055,\
    UcxEndpointStateResumeSMAndWaitForPendingOperation2=0x57a73056,\
    UcxEndpointStateSetStaleAndDeletePreviousStale=0x57a73030,\
    UcxEndpointStateSettingEnabledFlag1=0x57a7304d,\
    UcxEndpointStateSettingEnabledFlag2=0x57a7304e,\
    UcxEndpointStateStale=0x57a73031,\
    UcxEndpointStateStarting1=0x57a7304a,\
    UcxEndpointStateStarting2=0x57a7304b,\
    UcxEndpointStateStarting3=0x57a73052,\
    UcxEndpointStateWaitingForAbortUrbToBeQueued=0x57a73033,\
    UcxEndpointStateWaitingForPurgeComplete1=0x57a73034,\
    UcxEndpointStateWaitingForPurgeComplete2=0x57a73035,\
    UcxEndpointStateNull,\



//
// Event List For the State Machines
//
#define UCXENDPOINT_EVENT\
    UcxEndpointEventAbortComplete                     = 0xebe30010 | SmEngineEventPriorityNormal,\
    UcxEndpointEventAbortFromHub                      = 0xebe301c0 | SmEngineEventPriorityLow,\
    UcxEndpointEventAbortUrbFromClient                = 0xebe30020 | SmEngineEventPriorityLow,\
    UcxEndpointEventAbortUrbQueued                    = 0xebe30030 | SmEngineEventPriorityNormal,\
    UcxEndpointEventControllerResetComplete           = 0xebe30040 | SmEngineEventPrioritySpecial,\
    UcxEndpointEventDelete                            = 0xebe30050 | SmEngineEventPriorityLow,\
    UcxEndpointEventDeleteStale                       = 0xebe30060 | SmEngineEventPriorityNormal,\
    UcxEndpointEventDisconnectFromHub                 = 0xebe30070 | SmEngineEventPriorityLow,\
    UcxEndpointEventEnableComplete                    = 0xebe30080 | SmEngineEventPriorityLow,\
    UcxEndpointEventEndpointResetComplete             = 0xebe30090 | SmEngineEventPriorityNormal,\
    UcxEndpointEventEndpointResetFromHub              = 0xebe300a0 | SmEngineEventPriorityLow,\
    UcxEndpointEventNo                                = 0xebe300b0 | SmEngineEventPrioritySync,\
    UcxEndpointEventOperationSuccess                  = 0xebe300c0 | SmEngineEventPrioritySync,\
    UcxEndpointEventPrepareDisableFromHub             = 0xebe300d0 | SmEngineEventPriorityLow,\
    UcxEndpointEventPrepareForControllerReset         = 0xebe300e0 | SmEngineEventPriorityNormal,\
    UcxEndpointEventPurgeComplete                     = 0xebe300f0 | SmEngineEventPriorityNormal,\
    UcxEndpointEventPurgeFromHub                      = 0xebe30100 | SmEngineEventPriorityLow,\
    UcxEndpointEventStartFromHub                      = 0xebe30120 | SmEngineEventPriorityLow,\
    UcxEndpointEventStaticStreamsDisableComplete      = 0xebe30130 | SmEngineEventPriorityNormal,\
    UcxEndpointEventStaticStreamsDisableFromClient    = 0xebe30140 | SmEngineEventPriorityLow,\
    UcxEndpointEventStaticStreamsEnableComplete       = 0xebe30150 | SmEngineEventPriorityNormal,\
    UcxEndpointEventStaticStreamsEnableFromClient     = 0xebe30160 | SmEngineEventPriorityLow,\
    UcxEndpointEventTreePurgeFromHub                  = 0xebe301b0 | SmEngineEventPriorityLow,\
    UcxEndpointEventUsbDeviceDeleting                 = 0xebe30170 | SmEngineEventPrioritySync,\
    UcxEndpointEventUsbDeviceResetComplete            = 0xebe30180 | SmEngineEventPriorityLow,\
    UcxEndpointEventUsbDeviceResetFromHub             = 0xebe30190 | SmEngineEventPriorityLow,\
    UcxEndpointEventYes                               = 0xebe301a0 | SmEngineEventPrioritySync,\


typedef struct _SM_ENGINE_STATE_TABLE_ENTRY *PSM_ENGINE_STATE_TABLE_ENTRY;
extern PSM_ENGINE_STATE_TABLE_ENTRY UcxEndpointStateTable[];
