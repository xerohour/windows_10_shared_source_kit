
/*++
Copyright (c) Microsoft Corporation

Module Name:

PSM30_AutoGen.h

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/

#pragma once



//
// Enum of States in all the State Machines
//
typedef enum _PSM30_STATE {
    Psm30StateIgnored                                = 1000,
    Psm30StateEmptySubState                          = 1001,
    Psm30StateNull                                   = 1002,
    Psm30StateCompleteWithStatusFailed               = 1003,
    Psm30StateRequestPortCycle                       = 1004,
    Psm30StateSignalQueryDeviceTextEvent             = 1005,
    Psm30StateCompleteFDORequestWithStatusFailed     = 1006,
    Psm30StateCompleteGetPortStatusWithInternalError = 1007,
    Psm30StateCompleteGetPortStatusWithNoSuchDevice  = 1008,
    Psm30StateCompletePdoPreStart                    = 1009,
    Psm30StateSignalPnpPowerEvent                    = 1010,
    Psm30StatePoweredOffDisconnected                 = 5000,
    Psm30StateAcquiringInterruptReferenceOnHSMInDisabled = 5001,
    Psm30StateAcquiringInterruptReferenceOnHSMInEnabled = 5002,
    Psm30StateAcquiringInterruptReferenceOnHSMInEnabledOnWarmReset = 5003,
    Psm30StateAcquiringInterruptReferenceOnHSMOnResetRequest = 5004,
    Psm30StateAcquiringInterruptReferenceOnResuming  = 5005,
    Psm30StateAcquiringPowerReferenceAfterDisablingSuperSpeed = 5006,
    Psm30StateAcquiringPowerReferenceonPoweredOffConnectedErrorInS0 = 5007,
    Psm30StateAcquiringPowerReferenceonPoweredOffDisconnectedInS0 = 5008,
    Psm30StateAcquiringPowerReferenceonPoweredOffSuspendedInS0 = 5009,
    Psm30StateAcquiringReferenceInConnectedWithResetNeeded = 5010,
    Psm30StateCancellingResetOnSurpriseRemove        = 5011,
    Psm30StateCancellingSetPortPower                 = 5012,
    Psm30StateCheckingIfADeviceIsPresent             = 5013,
    Psm30StateCheckingIfDebounceIsNeeded             = 5014,
    Psm30StateCheckingIfLinkStateIsU0                = 5015,
    Psm30StateCheckingIfOvercurrentBitOne            = 5016,
    Psm30StateCheckingIfPersistentOvercurrent        = 5017,
    Psm30StateCheckingIfSuperSpeedShouldBeDisabled   = 5018,
    Psm30StateCheckingIsD3ColdIsEnabled              = 5019,
    Psm30StateConnectedEnabled                       = 5020,
    Psm30StateConnectedError                         = 5021,
    Psm30StateCreatingDevice                         = 5022,
    Psm30StateCreatingDeviceInConnectedWithResetInProgress = 5023,
    Psm30StateCreatingDeviceInConnectedWithResetNeeded = 5024,
    Psm30StateDetachingDeviceFromPortOnDisconnect    = 5025,
    Psm30StateDetachingDeviceFromPortOnDisconnectOnTimedOut = 5026,
    Psm30StateDetachingDeviceFromPortOnDisconnectWithTimer = 5027,
    Psm30StateDetachingDeviceFromPortOnOverCurrent   = 5028,
    Psm30StateDetachingDeviceFromPortOnOverCurrentClear = 5029,
    Psm30StateDetachingDeviceFromPortOnOverCurrentClearOnTimedOut = 5030,
    Psm30StateDetachingDeviceFromPortOnOverCurrentClearWithTimer = 5031,
    Psm30StateDetachingDeviceFromPortOnOverCurrentOnTimedOut = 5032,
    Psm30StateDetachingDeviceFromPortOnOverCurrentWithTimer = 5033,
    Psm30StateDetachingDeviceFromPortOnPortCycle     = 5034,
    Psm30StateDetachingDeviceOnCleanup               = 5035,
    Psm30StateDisablingSuperSpeedOnPortForUnsupportedHubs = 5036,
    Psm30StateDisablingSuperSpeedOnPortOnDeviceRequest = 5037,
    Psm30StateDisconnected                           = 5038,
    Psm30StateDisconnectedSuperSpeedDisabled         = 5039,
    Psm30StateDoesDeviceSupportsReAttach             = 5040,
    Psm30StateDoesDeviceSupportsReAttachWithResetInProgress = 5041,
    Psm30StateDoesDeviceSupportsReAttachWithResetNeeded = 5042,
    Psm30StateEnablingSuperSpeedOnStopSuspend        = 5043,
    Psm30StateEnablingSuperSpeedOnTimerExpiration    = 5044,
    Psm30StateFlushingStaleDeviceEvents              = 5045,
    Psm30StateFlushingStaleDeviceEventsWithResetInProgress = 5046,
    Psm30StateFlushingStaleDeviceEventsWithResetNeeded = 5047,
    Psm30StateFlushingUserResetForOverCurrentOnHubStopSuspend = 5048,
    Psm30StateFlushingUserResetForOverCurrentOnOverCurrentClear = 5049,
    Psm30StateGettingPortChangeOnStartResumeFromConnectedError = 5050,
    Psm30StateGettingPortStatusOnResetTimeout        = 5051,
    Psm30StateGettingPortStatusOnResumeRequest       = 5052,
    Psm30StateGettingPortStatusOnResumeTimeout       = 5053,
    Psm30StateInitiatingResetPort                    = 5054,
    Psm30StateInitiatingResume                       = 5055,
    Psm30StateInitiatingWarmResetPort                = 5056,
    Psm30StateIsDeviceInU0OnResumeRequest            = 5057,
    Psm30StateIsDeviceInU0OnResumeTimeout            = 5058,
    Psm30StateIsOldDevicePresent                     = 5059,
    Psm30StateIsOldDevicePresentInConnectedWithResetNeeded = 5060,
    Psm30StateIsOldDevicePresentWithResetInProgress  = 5061,
    Psm30StateIssuingAttachDeviceToDsm               = 5062,
    Psm30StateIssuingAttachDeviceWithResetToDsm      = 5063,
    Psm30StateIssuingAttachDeviceWithResetToDsmWithResetInProgress = 5064,
    Psm30StateIssuingDetachDeviceOnCycleFromPoweredOffDisabled = 5065,
    Psm30StateIssuingDisabledToDSMFromDisabled       = 5066,
    Psm30StateIssuingHubReset                        = 5067,
    Psm30StateIssuingHubResetFromDisconnected        = 5068,
    Psm30StateIssuingHubResetOnDisableFailure        = 5069,
    Psm30StateIssuingHubResetOnTimedOut              = 5070,
    Psm30StateIssuingHubResetWithPendingTimer        = 5071,
    Psm30StateIssuingHubResetWithTimer               = 5072,
    Psm30StateIssuingPortDisabledToDevice            = 5073,
    Psm30StateIssuingPortDisableFailedToDevice       = 5074,
    Psm30StateIssuingPortFailedToDSMOnLinkStateError = 5075,
    Psm30StateIssuingPortResetTimedOutToDSM          = 5076,
    Psm30StateIssuingReAttachDeviceToBootDevice      = 5077,
    Psm30StateIssuingReAttachDeviceToBootDeviceWithResetInProgress = 5078,
    Psm30StateIssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled = 5079,
    Psm30StateIssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled = 5080,
    Psm30StateIssuingResetFailedDuetoPendingHubSuspendToDSMWhileResetInProgress = 5081,
    Psm30StateIssuingResumedToDeviceSM               = 5082,
    Psm30StateIssuingSuspendedToDeviceSM             = 5083,
    Psm30StateNotifyingUserAboutPersistentOverCurrent = 5084,
    Psm30StatePoweredOffConnectedError               = 5085,
    Psm30StatePoweredOffConnectedWithResetNeededAndDeviceDetached = 5086,
    Psm30StatePoweredOffSuspended                    = 5087,
    Psm30StateQueryingPortStatusOnOverCurrent        = 5088,
    Psm30StateQueueingPortFailureToDSMOnTransferFailure = 5089,
    Psm30StateQueueingPortFailureToDSMOnTransferFailureForLpmTimeout = 5090,
    Psm30StateQueueingPortResumeFailedDueToPendingSuspend = 5091,
    Psm30StateQueueingPortResumeTimedOut             = 5092,
    Psm30StateQueueingPortTimeoutUpdatedToDSM        = 5093,
    Psm30StateQueueStateDisabledToDSMOnStartFromConnectedError = 5094,
    Psm30StateReleasingInterruptReferenceAfterResetFailure = 5095,
    Psm30StateReleasingInterruptReferenceAndStoppingTimer = 5096,
    Psm30StateReleasingInterruptReferenceOnResetTransferFailure = 5097,
    Psm30StateReleasingInterruptReferenceOnResumed   = 5098,
    Psm30StateReleasingInterruptReferenceOnSuccessfulReset = 5099,
    Psm30StateReleasingInterruptReferenceOnTransferFailureInResuming = 5100,
    Psm30StateReleasingPortPowerReferenceInSuperSpeedDisabled = 5101,
    Psm30StateReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded = 5102,
    Psm30StateReleasingPowerReferenceonPoweredOffDisabled = 5103,
    Psm30StateReleasingPowerReferenceonPoweredOffDisconnected = 5104,
    Psm30StateReleasingResetReferenceInDisconnected  = 5105,
    Psm30StateReleasingResetReferenceInSuperSpeedDisabled = 5106,
    Psm30StateReleasingResetReferenceOnDeviceRemoval = 5107,
    Psm30StateResettingHubAndFlushingUserInitiatedResetOnOverCurrent = 5108,
    Psm30StateSettingPortPowerOnOverCurrent          = 5109,
    Psm30StateSettingRemotWakeMask                   = 5110,
    Psm30StateSettingRemotWakeMaskInDisconected      = 5111,
    Psm30StateSettingU1TImeOut                       = 5112,
    Psm30StateSettingU2TImeOut                       = 5113,
    Psm30StateStartingAndAcquiringReferenceOnHubStart = 5114,
    Psm30StateStartingAndAcquiringReferenceOnSystemResume = 5115,
    Psm30StateStartingDisableSuperSpeedTimer         = 5116,
    Psm30StateStartingOverCurrentTimer               = 5117,
    Psm30StateStartingResumeTimer                    = 5118,
    Psm30StateStoppingDisableSuperSpeedTimer         = 5119,
    Psm30StateStoppingResumeTimer                    = 5120,
    Psm30StateStoppingTimerAndQueueingHubResetInDisconnected = 5121,
    Psm30StateStoppingTimerOnDisconnect              = 5122,
    Psm30StateStoppingTimerOnOverCurrent             = 5123,
    Psm30StateStoppingTimerOnOverCurrentClear        = 5124,
    Psm30StateStoppingTimerOnResetComplete           = 5125,
    Psm30StateSuperSpeedDisabled                     = 5126,
    Psm30StateSuperSpeedDisabledStopped              = 5127,
    Psm30StateSuspended                              = 5128,
    Psm30StateSuspendedDisabled                      = 5129,
    Psm30StateSuspendedInD3Cold                      = 5130,
    Psm30StateSuspending                             = 5131,
    Psm30StateSuspendingPortOnDisableRequest         = 5132,
    Psm30StateSuspendingPortOnHubSuspend             = 5133,
    Psm30StateWaitingForDebounceTimer                = 5134,
    Psm30StateWaitingForHubStopSuspend               = 5135,
    Psm30StateWaitingForHubStopSuspendInDisconnected = 5136,
    Psm30StateWaitingForHubStopSuspendOnAttachFailure = 5137,
    Psm30StateWaitingForHubStopSuspendOnAttachWithResetFailure = 5138,
    Psm30StateWaitingForHubStopSuspendOrTimer        = 5139,
    Psm30StateWaitingForOldDeviceToDetach            = 5140,
    Psm30StateWaitingForOldDeviceToDetachInConnectedWithResetNeeded = 5141,
    Psm30StateWaitingForOldDeviceToDetachWithResetInProgress = 5142,
    Psm30StateWaitingForOverCurrentClear             = 5143,
    Psm30StateWaitingForResetComplete                = 5144,
    Psm30StateWaitingForResetRequestFromDSMWithResetInPorgress = 5145,
    Psm30StateWaitingForResetTimerToFlush            = 5146,
    Psm30StateWaitingForResumeComplete               = 5147,
    Psm30StateWaitingForResumeTimerToFlush           = 5148,
    Psm30StateWaitingForSuperSpeedDisableTimerToFlush = 5149,
    Psm30StateWaitingForTimerOnResetInDisconnected   = 5150,
    Psm30StateWaitingForTimerToFlushOnDisconnect     = 5151,
    Psm30StateWaitingForTimerToFlushOnHubStopSuspend = 5152,
    Psm30StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected = 5153,
    Psm30StateWaitingForTimerToFlushOnOverCurrent    = 5154,
    Psm30StateWaitingForTimerToFlushOnOverCurrentClear = 5155,
    Psm30StateWaitingForUserResetOnOverCurrent       = 5156,
    Psm30StateWaitingToBeDeleted                     = 5157,
    Psm30StateAckingPortChange                       = 5158,
    Psm30StateCheckIfThereIsAPortChange              = 5159,
    Psm30StateCheckIfThereIsAPortChangeOnGetPortStatus = 5160,
    Psm30StateEnablingInterruptsAndGettingPortEvent  = 5161,
    Psm30StateEnablingInterruptsOnFailure            = 5162,
    Psm30StateGettingPortStatus                      = 5163,
    Psm30StateInitializingCumulativePortChangeBits   = 5164,
    Psm30StateReturningPortResetHubOnError           = 5165,
    Psm30StateAckingPortChangeInNoChange             = 5166,
    Psm30StateCheckIfThereIsAPortChangeInNoChange    = 5167,
    Psm30StateCheckIfThereIsAPortChangeOnGetPortStatusInNoChange = 5168,
    Psm30StateEnablingInterruptsAndGettingPortEventInNoChange = 5169,
    Psm30StateGettingPortStatusInNoChange            = 5170,
    Psm30StateInitializingCumulativePortChangeBitsInNoChange = 5171,
    Psm30StateReturningPortResetHubOnErrorInNoChange = 5172,
    Psm30StateAckingPortChangeOnStartResume          = 5173,
    Psm30StateAcquiringPortReferenceAndGettingPortEvent = 5174,
    Psm30StateAcquiringPortReferenceOnFailure        = 5175,
    Psm30StateCheckIfThereIsAPortChangeOnStartResume = 5176,
    Psm30StateCheckIfThereIsAPortChangeOnStartResumeOnGetPortStatus = 5177,
    Psm30StateGettingPortStatusOnStartResume         = 5178,
    Psm30StateInitializingCumulativePortChangeBitsOnStartResume = 5179,
    Psm30StateReturningPortResetHubOnErrorOnStartResume = 5180,
    Psm30StateGettingPortChange                      = 5181,
    Psm30StateReturningConnectChangeOnLinkError      = 5182,
    Psm30StateReturningPortResetHubOnErrorInPortChangeHandler = 5183,
    Psm30StateWaitingForPortChangeEvent              = 5184,
    Psm30StateAcquiringPortReferenceOnTransferFailure = 5185,
    Psm30StateCheckingIfPortIsPoweredOnStart         = 5186,
    Psm30StateCheckingIfPowerOnStartHackIsPresent    = 5187,
    Psm30StateGettingPortStatusOnStart               = 5188,
    Psm30StateGettingPortStatusOnStartBeforePoweringOn = 5189,
    Psm30StatePoweringOnPortOnStart                  = 5190,
    Psm30StateReturningPortConnectChangeOnLinkErrorOnStart = 5191,
    Psm30StateReturningPortResetHubOnErrorOnStart    = 5192,
    Psm30StateWaitingForPortPowerOnTimerOnStart      = 5193,
    Psm30StateCheckingIfDeviceIsStillConnectedOnResume = 5194,
    Psm30StateGettingPortStatusOnResume              = 5195,
    Psm30StateGettingPortStatusOnResumeAfterWaiting  = 5196,
    Psm30StateQueueingStateDisabledOnConnectChangeOnResume = 5197,
    Psm30StateQueueingStateDisabledOnFailure         = 5198,
    Psm30StateQueueingStateDisabledOnResetInProgress = 5199,
    Psm30StateQueueingStateEnabledOnResume           = 5200,
    Psm30StateQueueingStateSuspendedOnResume         = 5201,
    Psm30StateReturningHubStopOnResume               = 5202,
    Psm30StateReturningPortConnectChange             = 5203,
    Psm30StateReturningPortNeedsResetOnResume        = 5204,
    Psm30StateReturningPortOperationSuccessOnResume  = 5205,
    Psm30StateReturningPortResetHubOnErrorOnResume   = 5206,
    Psm30StateReturningPortResetInProgressOnResume   = 5207,
    Psm30StateReturningPortResumedOnResume           = 5208,
    Psm30StateStartingTimerForAllowingReconnect      = 5209,
    Psm30StateStoppingReconnectTimerOnHubStatusChange = 5210,
    Psm30StateStoppingReconnectTimerOnHubStop        = 5211,
    Psm30StateWaitingForDeviceToReconnect            = 5212,
    Psm30StateWaitingForReconnectTimerToFlushOnHubStatusChange = 5213,
    Psm30StateWaitingForReconnectTimerToFlushOnHubStop = 5214,
    Psm30StateGettingPortStatusInResuming            = 5215,
    Psm30StateReturningConnectChangeOnLinkErrorInResuming = 5216,
    Psm30StateReturningPortResetHubOnErrorInResuming = 5217,
    Psm30StateWaitingForPortChangeEventInResuming    = 5218,
    Psm30StateGettingPortStatusInWaitingForStopSuspend = 5219,
    Psm30StateQueueingPortEventFailureToDSM          = 5220,
    Psm30StateWaitingForDevicePortEvents             = 5221,
    Psm30StateGettingPortStatusInWaitingForStopSuspendInDisconnected = 5222,
    Psm30StateWaitingForDevicePortEventsInDisconnected = 5223,
    Psm30StateCheckingIfLinkIsInU0AfterResetPolling  = 5224,
    Psm30StateCheckingIfResetCompletionRequiresPolling = 5225,
    Psm30StateGettingPortStatusInReset               = 5226,
    Psm30StateGettingPortStatusInResetAfterPollingInterval = 5227,
    Psm30StateReturningPortResetCompleted            = 5228,
    Psm30StateReturningPortResetHubOnErrorInReset    = 5229,
    Psm30StateWaitingForPollingIntervalForResetCompletion = 5230,
    Psm30StateWaitingForPortChangeEventInReset       = 5231,
    Psm30StateCheckingIfBootDeviceIsConnected        = 5232,
    Psm30StateCheckingIfParentHubIsArmedForWake      = 5233,
    Psm30StateDisablingRemoteWakeOnPort              = 5234,
    Psm30StateEnablingRemoteWakeOnPort               = 5235,
    Psm30StateIssuingHubResetOnRemoteWakeMaskFailure = 5236,
    Psm30StateReturningOperationSucessFromArmingForWake = 5237,
    Psm30StateAcquiringPowerReferenceOnHubResumeInD3Cold = 5238,
    Psm30StateCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold = 5239,
    Psm30StateCheckingIfDeviceIsConnectedOnPortChangeInD3Cold = 5240,
    Psm30StateCheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume = 5241,
    Psm30StatePoweredOffOnHubSuspendFromD3Cold       = 5242,
    Psm30StateQueueingEnabledToDSMOnReconnectAfterResumeRequestFromD3Cold = 5243,
    Psm30StateQueueingEnabledToDSMOnReconnectFromD3Cold = 5244,
    Psm30StateQueueingStateSuspendedOnHubResumeInD3Cold = 5245,
    Psm30StateReleasingPowerReferenceOnHubS0IdleInD3Cold = 5246,
    Psm30StateReturningConnectChangeFromD3Cold       = 5247,
    Psm30StateReturningHubStopFromSuspendedInD3Cold  = 5248,
    Psm30StateReturningOverCurrentClearedFromD3Cold  = 5249,
    Psm30StateReturningOverCurrentFromD3Cold         = 5250,
    Psm30StateReturningPortResetHubOnErrorFromD3Cold = 5251,
    Psm30StateReturningPortResumedFromD3Cold         = 5252,
    Psm30StateStartingTimerForAllowingReconnectOnResumingFromD3Cold = 5253,
    Psm30StateStoppingReconnectTimerOnDeviceReAttachAfterD3Cold = 5254,
    Psm30StateStoppingReconnectTimerOnHubStopInD3Cold = 5255,
    Psm30StateStoppingReconnectTimerOnOvercurrentClearedInD3Cold = 5256,
    Psm30StateStoppingReconnectTimerOnOvercurrentInD3Cold = 5257,
    Psm30StateStoppingReconnectTimerOnResetHubErrorInD3Cold = 5258,
    Psm30StateWaitingForDeviceRequestResumeOnD3Cold  = 5259,
    Psm30StateWaitingForDeviceToReconnectOnResumeFromD3Cold = 5260,
    Psm30StateWaitingForPortChangeEventInD3Cold      = 5261,
    Psm30StateWaitingForReconnectTimerToFlushOnHubStopInD3Cold = 5262,
    Psm30StateWaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold = 5263,
    Psm30StateWaitingForReconnectTimerToFlushOnOvercurrentInD3Cold = 5264,
    Psm30StateWaitingForReconnectTimerToFlushOnReattachAfterD3Cold = 5265,
    Psm30StateWaitingForReconnectTimerToFlushOnResetHubErrorInD3Cold = 5266,
} PSM30_STATE, *PPSM30_STATE;





//
// Enum of ETW state names
//
typedef enum _PSM30_STATE_ETW {
    Psm30StateEtwPoweredOffDisconnected              = 5000,
    Psm30StateEtwAcquiringInterruptReferenceOnHSMInDisabled = 5001,
    Psm30StateEtwAcquiringInterruptReferenceOnHSMInEnabled = 5002,
    Psm30StateEtwAcquiringInterruptReferenceOnHSMInEnabledOnWarmReset = 5003,
    Psm30StateEtwAcquiringInterruptReferenceOnHSMOnResetRequest = 5004,
    Psm30StateEtwAcquiringInterruptReferenceOnResuming = 5005,
    Psm30StateEtwAcquiringPowerReferenceAfterDisablingSuperSpeed = 5006,
    Psm30StateEtwAcquiringPowerReferenceonPoweredOffConnectedErrorInS0 = 5007,
    Psm30StateEtwAcquiringPowerReferenceonPoweredOffDisconnectedInS0 = 5008,
    Psm30StateEtwAcquiringPowerReferenceonPoweredOffSuspendedInS0 = 5009,
    Psm30StateEtwAcquiringReferenceInConnectedWithResetNeeded = 5010,
    Psm30StateEtwCancellingResetOnSurpriseRemove     = 5011,
    Psm30StateEtwCancellingSetPortPower              = 5012,
    Psm30StateEtwCheckingIfADeviceIsPresent          = 5013,
    Psm30StateEtwCheckingIfDebounceIsNeeded          = 5014,
    Psm30StateEtwCheckingIfLinkStateIsU0             = 5015,
    Psm30StateEtwCheckingIfOvercurrentBitOne         = 5266,
    Psm30StateEtwCheckingIfPersistentOvercurrent     = 5265,
    Psm30StateEtwCheckingIfSuperSpeedShouldBeDisabled = 5016,
    Psm30StateEtwCheckingIsD3ColdIsEnabled           = 5017,
    Psm30StateEtwConnectedEnabled                    = 5018,
    Psm30StateEtwConnectedError                      = 5019,
    Psm30StateEtwCreatingDevice                      = 5020,
    Psm30StateEtwCreatingDeviceInConnectedWithResetInProgress = 5021,
    Psm30StateEtwCreatingDeviceInConnectedWithResetNeeded = 5022,
    Psm30StateEtwDetachingDeviceFromPortOnDisconnect = 5023,
    Psm30StateEtwDetachingDeviceFromPortOnDisconnectOnTimedOut = 5024,
    Psm30StateEtwDetachingDeviceFromPortOnDisconnectWithTimer = 5025,
    Psm30StateEtwDetachingDeviceFromPortOnOverCurrent = 5026,
    Psm30StateEtwDetachingDeviceFromPortOnOverCurrentClear = 5027,
    Psm30StateEtwDetachingDeviceFromPortOnOverCurrentClearOnTimedOut = 5028,
    Psm30StateEtwDetachingDeviceFromPortOnOverCurrentClearWithTimer = 5029,
    Psm30StateEtwDetachingDeviceFromPortOnOverCurrentOnTimedOut = 5030,
    Psm30StateEtwDetachingDeviceFromPortOnOverCurrentWithTimer = 5031,
    Psm30StateEtwDetachingDeviceFromPortOnPortCycle  = 5032,
    Psm30StateEtwDetachingDeviceOnCleanup            = 5033,
    Psm30StateEtwDisablingSuperSpeedOnPortForUnsupportedHubs = 5034,
    Psm30StateEtwDisablingSuperSpeedOnPortOnDeviceRequest = 5035,
    Psm30StateEtwDisconnected                        = 5036,
    Psm30StateEtwDisconnectedSuperSpeedDisabled      = 5037,
    Psm30StateEtwDoesDeviceSupportsReAttach          = 5038,
    Psm30StateEtwDoesDeviceSupportsReAttachWithResetInProgress = 5263,
    Psm30StateEtwDoesDeviceSupportsReAttachWithResetNeeded = 5039,
    Psm30StateEtwEnablingSuperSpeedOnStopSuspend     = 5040,
    Psm30StateEtwEnablingSuperSpeedOnTimerExpiration = 5041,
    Psm30StateEtwFlushingStaleDeviceEvents           = 5042,
    Psm30StateEtwFlushingStaleDeviceEventsWithResetInProgress = 5043,
    Psm30StateEtwFlushingStaleDeviceEventsWithResetNeeded = 5044,
    Psm30StateEtwFlushingUserResetForOverCurrentOnHubStopSuspend = 5045,
    Psm30StateEtwFlushingUserResetForOverCurrentOnOverCurrentClear = 5046,
    Psm30StateEtwGettingPortChangeOnStartResumeFromConnectedError = 5047,
    Psm30StateEtwGettingPortStatusOnResetTimeout     = 5048,
    Psm30StateEtwGettingPortStatusOnResumeRequest    = 5049,
    Psm30StateEtwGettingPortStatusOnResumeTimeout    = 5050,
    Psm30StateEtwInitiatingResetPort                 = 5051,
    Psm30StateEtwInitiatingResume                    = 5052,
    Psm30StateEtwInitiatingWarmResetPort             = 5053,
    Psm30StateEtwIsDeviceInU0OnResumeRequest         = 5054,
    Psm30StateEtwIsDeviceInU0OnResumeTimeout         = 5055,
    Psm30StateEtwIsOldDevicePresent                  = 5056,
    Psm30StateEtwIsOldDevicePresentInConnectedWithResetNeeded = 5057,
    Psm30StateEtwIsOldDevicePresentWithResetInProgress = 5058,
    Psm30StateEtwIssuingAttachDeviceToDsm            = 5059,
    Psm30StateEtwIssuingAttachDeviceWithResetToDsm   = 5060,
    Psm30StateEtwIssuingAttachDeviceWithResetToDsmWithResetInProgress = 5061,
    Psm30StateEtwIssuingDetachDeviceOnCycleFromPoweredOffDisabled = 5062,
    Psm30StateEtwIssuingDisabledToDSMFromDisabled    = 5063,
    Psm30StateEtwIssuingHubReset                     = 5064,
    Psm30StateEtwIssuingHubResetFromDisconnected     = 5065,
    Psm30StateEtwIssuingHubResetOnDisableFailure     = 5066,
    Psm30StateEtwIssuingHubResetOnTimedOut           = 5067,
    Psm30StateEtwIssuingHubResetWithPendingTimer     = 5068,
    Psm30StateEtwIssuingHubResetWithTimer            = 5069,
    Psm30StateEtwIssuingPortDisabledToDevice         = 5070,
    Psm30StateEtwIssuingPortDisableFailedToDevice    = 5071,
    Psm30StateEtwIssuingPortFailedToDSMOnLinkStateError = 5072,
    Psm30StateEtwIssuingPortResetTimedOutToDSM       = 5073,
    Psm30StateEtwIssuingReAttachDeviceToBootDevice   = 5074,
    Psm30StateEtwIssuingReAttachDeviceToBootDeviceWithResetInProgress = 5264,
    Psm30StateEtwIssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled = 5075,
    Psm30StateEtwIssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled = 5076,
    Psm30StateEtwIssuingResetFailedDuetoPendingHubSuspendToDSMWhileResetInProgress = 5077,
    Psm30StateEtwIssuingResumedToDeviceSM            = 5078,
    Psm30StateEtwIssuingSuspendedToDeviceSM          = 5079,
    Psm30StateEtwNotifyingUserAboutPersistentOverCurrent = 5080,
    Psm30StateEtwPoweredOffConnectedError            = 5081,
    Psm30StateEtwPoweredOffConnectedWithResetNeededAndDeviceDetached = 5082,
    Psm30StateEtwPoweredOffSuspended                 = 5083,
    Psm30StateEtwQueryingPortStatusOnOverCurrent     = 5267,
    Psm30StateEtwQueueingPortFailureToDSMOnTransferFailure = 5084,
    Psm30StateEtwQueueingPortFailureToDSMOnTransferFailureForLpmTimeout = 5085,
    Psm30StateEtwQueueingPortResumeFailedDueToPendingSuspend = 5086,
    Psm30StateEtwQueueingPortResumeTimedOut          = 5087,
    Psm30StateEtwQueueingPortTimeoutUpdatedToDSM     = 5088,
    Psm30StateEtwQueueStateDisabledToDSMOnStartFromConnectedError = 5089,
    Psm30StateEtwReleasingInterruptReferenceAfterResetFailure = 5090,
    Psm30StateEtwReleasingInterruptReferenceAndStoppingTimer = 5091,
    Psm30StateEtwReleasingInterruptReferenceOnResetTransferFailure = 5092,
    Psm30StateEtwReleasingInterruptReferenceOnResumed = 5093,
    Psm30StateEtwReleasingInterruptReferenceOnSuccessfulReset = 5094,
    Psm30StateEtwReleasingInterruptReferenceOnTransferFailureInResuming = 5095,
    Psm30StateEtwReleasingPortPowerReferenceInSuperSpeedDisabled = 5096,
    Psm30StateEtwReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded = 5097,
    Psm30StateEtwReleasingPowerReferenceonPoweredOffDisabled = 5098,
    Psm30StateEtwReleasingPowerReferenceonPoweredOffDisconnected = 5099,
    Psm30StateEtwReleasingResetReferenceInDisconnected = 5100,
    Psm30StateEtwReleasingResetReferenceInSuperSpeedDisabled = 5101,
    Psm30StateEtwReleasingResetReferenceOnDeviceRemoval = 5102,
    Psm30StateEtwResettingHubAndFlushingUserInitiatedResetOnOverCurrent = 5103,
    Psm30StateEtwSettingPortPowerOnOverCurrent       = 5104,
    Psm30StateEtwSettingRemotWakeMask                = 5105,
    Psm30StateEtwSettingRemotWakeMaskInDisconected   = 5106,
    Psm30StateEtwSettingU1TImeOut                    = 5107,
    Psm30StateEtwSettingU2TImeOut                    = 5108,
    Psm30StateEtwStartingAndAcquiringReferenceOnHubStart = 5109,
    Psm30StateEtwStartingAndAcquiringReferenceOnSystemResume = 5110,
    Psm30StateEtwStartingDisableSuperSpeedTimer      = 5111,
    Psm30StateEtwStartingOverCurrentTimer            = 5112,
    Psm30StateEtwStartingResumeTimer                 = 5113,
    Psm30StateEtwStoppingDisableSuperSpeedTimer      = 5114,
    Psm30StateEtwStoppingResumeTimer                 = 5115,
    Psm30StateEtwStoppingTimerAndQueueingHubResetInDisconnected = 5116,
    Psm30StateEtwStoppingTimerOnDisconnect           = 5117,
    Psm30StateEtwStoppingTimerOnOverCurrent          = 5118,
    Psm30StateEtwStoppingTimerOnOverCurrentClear     = 5119,
    Psm30StateEtwStoppingTimerOnResetComplete        = 5120,
    Psm30StateEtwSuperSpeedDisabled                  = 5121,
    Psm30StateEtwSuperSpeedDisabledStopped           = 5122,
    Psm30StateEtwSuspended                           = 5123,
    Psm30StateEtwSuspendedDisabled                   = 5124,
    Psm30StateEtwSuspendedInD3Cold                   = 5125,
    Psm30StateEtwSuspending                          = 5126,
    Psm30StateEtwSuspendingPortOnDisableRequest      = 5127,
    Psm30StateEtwSuspendingPortOnHubSuspend          = 5128,
    Psm30StateEtwWaitingForDebounceTimer             = 5129,
    Psm30StateEtwWaitingForHubStopSuspend            = 5130,
    Psm30StateEtwWaitingForHubStopSuspendInDisconnected = 5131,
    Psm30StateEtwWaitingForHubStopSuspendOnAttachFailure = 5132,
    Psm30StateEtwWaitingForHubStopSuspendOnAttachWithResetFailure = 5133,
    Psm30StateEtwWaitingForHubStopSuspendOrTimer     = 5134,
    Psm30StateEtwWaitingForOldDeviceToDetach         = 5135,
    Psm30StateEtwWaitingForOldDeviceToDetachInConnectedWithResetNeeded = 5136,
    Psm30StateEtwWaitingForOldDeviceToDetachWithResetInProgress = 5137,
    Psm30StateEtwWaitingForOverCurrentClear          = 5138,
    Psm30StateEtwWaitingForResetComplete             = 5139,
    Psm30StateEtwWaitingForResetRequestFromDSMWithResetInPorgress = 5140,
    Psm30StateEtwWaitingForResetTimerToFlush         = 5141,
    Psm30StateEtwWaitingForResumeComplete            = 5142,
    Psm30StateEtwWaitingForResumeTimerToFlush        = 5143,
    Psm30StateEtwWaitingForSuperSpeedDisableTimerToFlush = 5144,
    Psm30StateEtwWaitingForTimerOnResetInDisconnected = 5145,
    Psm30StateEtwWaitingForTimerToFlushOnDisconnect  = 5146,
    Psm30StateEtwWaitingForTimerToFlushOnHubStopSuspend = 5147,
    Psm30StateEtwWaitingForTimerToFlushOnHubStopSuspendInDisconnected = 5148,
    Psm30StateEtwWaitingForTimerToFlushOnOverCurrent = 5149,
    Psm30StateEtwWaitingForTimerToFlushOnOverCurrentClear = 5150,
    Psm30StateEtwWaitingForUserResetOnOverCurrent    = 5151,
    Psm30StateEtwWaitingToBeDeleted                  = 5152,
    Psm30StateEtwAckingPortChange                    = 5153,
    Psm30StateEtwCheckIfThereIsAPortChange           = 5154,
    Psm30StateEtwCheckIfThereIsAPortChangeOnGetPortStatus = 5155,
    Psm30StateEtwEnablingInterruptsAndGettingPortEvent = 5156,
    Psm30StateEtwEnablingInterruptsOnFailure         = 5157,
    Psm30StateEtwGettingPortStatus                   = 5158,
    Psm30StateEtwInitializingCumulativePortChangeBits = 5159,
    Psm30StateEtwReturningPortResetHubOnError        = 5160,
    Psm30StateEtwAckingPortChangeInNoChange          = 5161,
    Psm30StateEtwCheckIfThereIsAPortChangeInNoChange = 5162,
    Psm30StateEtwCheckIfThereIsAPortChangeOnGetPortStatusInNoChange = 5163,
    Psm30StateEtwEnablingInterruptsAndGettingPortEventInNoChange = 5164,
    Psm30StateEtwGettingPortStatusInNoChange         = 5165,
    Psm30StateEtwInitializingCumulativePortChangeBitsInNoChange = 5166,
    Psm30StateEtwReturningPortResetHubOnErrorInNoChange = 5167,
    Psm30StateEtwAckingPortChangeOnStartResume       = 5168,
    Psm30StateEtwAcquiringPortReferenceAndGettingPortEvent = 5169,
    Psm30StateEtwAcquiringPortReferenceOnFailure     = 5170,
    Psm30StateEtwCheckIfThereIsAPortChangeOnStartResume = 5171,
    Psm30StateEtwCheckIfThereIsAPortChangeOnStartResumeOnGetPortStatus = 5172,
    Psm30StateEtwGettingPortStatusOnStartResume      = 5173,
    Psm30StateEtwInitializingCumulativePortChangeBitsOnStartResume = 5174,
    Psm30StateEtwReturningPortResetHubOnErrorOnStartResume = 5175,
    Psm30StateEtwGettingPortChange                   = 5176,
    Psm30StateEtwReturningConnectChangeOnLinkError   = 5177,
    Psm30StateEtwReturningPortResetHubOnErrorInPortChangeHandler = 5178,
    Psm30StateEtwWaitingForPortChangeEvent           = 5179,
    Psm30StateEtwAcquiringPortReferenceOnTransferFailure = 5180,
    Psm30StateEtwCheckingIfPortIsPoweredOnStart      = 5181,
    Psm30StateEtwCheckingIfPowerOnStartHackIsPresent = 5182,
    Psm30StateEtwGettingPortStatusOnStart            = 5183,
    Psm30StateEtwGettingPortStatusOnStartBeforePoweringOn = 5184,
    Psm30StateEtwPoweringOnPortOnStart               = 5185,
    Psm30StateEtwReturningPortConnectChangeOnLinkErrorOnStart = 5186,
    Psm30StateEtwReturningPortResetHubOnErrorOnStart = 5187,
    Psm30StateEtwWaitingForPortPowerOnTimerOnStart   = 5188,
    Psm30StateEtwCheckingIfDeviceIsStillConnectedOnResume = 5189,
    Psm30StateEtwGettingPortStatusOnResume           = 5190,
    Psm30StateEtwGettingPortStatusOnResumeAfterWaiting = 5191,
    Psm30StateEtwQueueingStateDisabledOnConnectChangeOnResume = 5192,
    Psm30StateEtwQueueingStateDisabledOnFailure      = 5193,
    Psm30StateEtwQueueingStateDisabledOnResetInProgress = 5194,
    Psm30StateEtwQueueingStateEnabledOnResume        = 5195,
    Psm30StateEtwQueueingStateSuspendedOnResume      = 5196,
    Psm30StateEtwReturningHubStopOnResume            = 5197,
    Psm30StateEtwReturningPortConnectChange          = 5198,
    Psm30StateEtwReturningPortNeedsResetOnResume     = 5199,
    Psm30StateEtwReturningPortOperationSuccessOnResume = 5200,
    Psm30StateEtwReturningPortResetHubOnErrorOnResume = 5201,
    Psm30StateEtwReturningPortResetInProgressOnResume = 5202,
    Psm30StateEtwReturningPortResumedOnResume        = 5203,
    Psm30StateEtwStartingTimerForAllowingReconnect   = 5204,
    Psm30StateEtwStoppingReconnectTimerOnHubStatusChange = 5205,
    Psm30StateEtwStoppingReconnectTimerOnHubStop     = 5206,
    Psm30StateEtwWaitingForDeviceToReconnect         = 5207,
    Psm30StateEtwWaitingForReconnectTimerToFlushOnHubStatusChange = 5208,
    Psm30StateEtwWaitingForReconnectTimerToFlushOnHubStop = 5209,
    Psm30StateEtwGettingPortStatusInResuming         = 5210,
    Psm30StateEtwReturningConnectChangeOnLinkErrorInResuming = 5211,
    Psm30StateEtwReturningPortResetHubOnErrorInResuming = 5212,
    Psm30StateEtwWaitingForPortChangeEventInResuming = 5213,
    Psm30StateEtwGettingPortStatusInWaitingForStopSuspend = 5214,
    Psm30StateEtwQueueingPortEventFailureToDSM       = 5215,
    Psm30StateEtwWaitingForDevicePortEvents          = 5216,
    Psm30StateEtwGettingPortStatusInWaitingForStopSuspendInDisconnected = 5217,
    Psm30StateEtwWaitingForDevicePortEventsInDisconnected = 5218,
    Psm30StateEtwCheckingIfLinkIsInU0AfterResetPolling = 5219,
    Psm30StateEtwCheckingIfResetCompletionRequiresPolling = 5220,
    Psm30StateEtwGettingPortStatusInReset            = 5221,
    Psm30StateEtwGettingPortStatusInResetAfterPollingInterval = 5222,
    Psm30StateEtwReturningPortResetCompleted         = 5223,
    Psm30StateEtwReturningPortResetHubOnErrorInReset = 5224,
    Psm30StateEtwWaitingForPollingIntervalForResetCompletion = 5225,
    Psm30StateEtwWaitingForPortChangeEventInReset    = 5226,
    Psm30StateEtwCheckingIfBootDeviceIsConnected     = 5227,
    Psm30StateEtwCheckingIfParentHubIsArmedForWake   = 5228,
    Psm30StateEtwDisablingRemoteWakeOnPort           = 5229,
    Psm30StateEtwEnablingRemoteWakeOnPort            = 5230,
    Psm30StateEtwIssuingHubResetOnRemoteWakeMaskFailure = 5231,
    Psm30StateEtwReturningOperationSucessFromArmingForWake = 5232,
    Psm30StateEtwAcquiringPowerReferenceOnHubResumeInD3Cold = 5233,
    Psm30StateEtwCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold = 5234,
    Psm30StateEtwCheckingIfDeviceIsConnectedOnPortChangeInD3Cold = 5235,
    Psm30StateEtwCheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume = 5236,
    Psm30StateEtwPoweredOffOnHubSuspendFromD3Cold    = 5237,
    Psm30StateEtwQueueingEnabledToDSMOnReconnectAfterResumeRequestFromD3Cold = 5238,
    Psm30StateEtwQueueingEnabledToDSMOnReconnectFromD3Cold = 5239,
    Psm30StateEtwQueueingStateSuspendedOnHubResumeInD3Cold = 5240,
    Psm30StateEtwReleasingPowerReferenceOnHubS0IdleInD3Cold = 5241,
    Psm30StateEtwReturningConnectChangeFromD3Cold    = 5242,
    Psm30StateEtwReturningHubStopFromSuspendedInD3Cold = 5243,
    Psm30StateEtwReturningOverCurrentClearedFromD3Cold = 5244,
    Psm30StateEtwReturningOverCurrentFromD3Cold      = 5245,
    Psm30StateEtwReturningPortResetHubOnErrorFromD3Cold = 5246,
    Psm30StateEtwReturningPortResumedFromD3Cold      = 5247,
    Psm30StateEtwStartingTimerForAllowingReconnectOnResumingFromD3Cold = 5248,
    Psm30StateEtwStoppingReconnectTimerOnDeviceReAttachAfterD3Cold = 5249,
    Psm30StateEtwStoppingReconnectTimerOnHubStopInD3Cold = 5250,
    Psm30StateEtwStoppingReconnectTimerOnOvercurrentClearedInD3Cold = 5251,
    Psm30StateEtwStoppingReconnectTimerOnOvercurrentInD3Cold = 5252,
    Psm30StateEtwStoppingReconnectTimerOnResetHubErrorInD3Cold = 5253,
    Psm30StateEtwWaitingForDeviceRequestResumeOnD3Cold = 5254,
    Psm30StateEtwWaitingForDeviceToReconnectOnResumeFromD3Cold = 5255,
    Psm30StateEtwWaitingForPortChangeEventInD3Cold   = 5257,
    Psm30StateEtwWaitingForReconnectTimerToFlushOnHubStopInD3Cold = 5258,
    Psm30StateEtwWaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold = 5259,
    Psm30StateEtwWaitingForReconnectTimerToFlushOnOvercurrentInD3Cold = 5260,
    Psm30StateEtwWaitingForReconnectTimerToFlushOnReattachAfterD3Cold = 5261,
    Psm30StateEtwWaitingForReconnectTimerToFlushOnResetHubErrorInD3Cold = 5262,
} PSM30_STATE_ETW, *PPSM30_STATE_ETW;



//
// Definitions for State Entry Functions 
//
STATE_ENTRY_FUNCTION          HUBPSM30_AcquiringInterruptReferenceOnHSMInDisabled;

STATE_ENTRY_FUNCTION          HUBPSM30_AcquiringInterruptReferenceOnHSMInEnabled;

STATE_ENTRY_FUNCTION          HUBPSM30_AcquiringInterruptReferenceOnHSMInEnabledOnWarmReset;

STATE_ENTRY_FUNCTION          HUBPSM30_AcquiringInterruptReferenceOnHSMOnResetRequest;

STATE_ENTRY_FUNCTION          HUBPSM30_AcquiringInterruptReferenceOnResuming;

STATE_ENTRY_FUNCTION          HUBPSM30_AcquiringPowerReferenceAfterDisablingSuperSpeed;

STATE_ENTRY_FUNCTION          HUBPSM30_AcquiringPowerReferenceonPoweredOffConnectedErrorInS0;

STATE_ENTRY_FUNCTION          HUBPSM30_AcquiringPowerReferenceonPoweredOffDisconnectedInS0;

STATE_ENTRY_FUNCTION          HUBPSM30_AcquiringPowerReferenceonPoweredOffSuspendedInS0;

STATE_ENTRY_FUNCTION          HUBPSM30_AcquiringReferenceInConnectedWithResetNeeded;

STATE_ENTRY_FUNCTION          HUBPSM30_CancellingResetOnSurpriseRemove;

STATE_ENTRY_FUNCTION          HUBPSM30_CancellingSetPortPower;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfADeviceIsPresent;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfDebounceIsNeeded;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfLinkStateIsU0;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfOvercurrentBitOne;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfPersistentOvercurrent;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfSuperSpeedShouldBeDisabled;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIsD3ColdIsEnabled;

STATE_ENTRY_FUNCTION          HUBPSM30_ConnectedEnabled;

STATE_ENTRY_FUNCTION          HUBPSM30_ConnectedError;

STATE_ENTRY_FUNCTION          HUBPSM30_CreatingDevice;

STATE_ENTRY_FUNCTION          HUBPSM30_CreatingDeviceInConnectedWithResetInProgress;

STATE_ENTRY_FUNCTION          HUBPSM30_CreatingDeviceInConnectedWithResetNeeded;

STATE_ENTRY_FUNCTION          HUBPSM30_DetachingDeviceFromPortOnDisconnect;

STATE_ENTRY_FUNCTION          HUBPSM30_DetachingDeviceFromPortOnDisconnectOnTimedOut;

STATE_ENTRY_FUNCTION          HUBPSM30_DetachingDeviceFromPortOnDisconnectWithTimer;

STATE_ENTRY_FUNCTION          HUBPSM30_DetachingDeviceFromPortOnOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM30_DetachingDeviceFromPortOnOverCurrentClear;

STATE_ENTRY_FUNCTION          HUBPSM30_DetachingDeviceFromPortOnOverCurrentClearOnTimedOut;

STATE_ENTRY_FUNCTION          HUBPSM30_DetachingDeviceFromPortOnOverCurrentClearWithTimer;

STATE_ENTRY_FUNCTION          HUBPSM30_DetachingDeviceFromPortOnOverCurrentOnTimedOut;

STATE_ENTRY_FUNCTION          HUBPSM30_DetachingDeviceFromPortOnOverCurrentWithTimer;

STATE_ENTRY_FUNCTION          HUBPSM30_DetachingDeviceFromPortOnPortCycle;

STATE_ENTRY_FUNCTION          HUBPSM30_DetachingDeviceOnCleanup;

STATE_ENTRY_FUNCTION          HUBPSM30_DisablingSuperSpeedOnPortForUnsupportedHubs;

STATE_ENTRY_FUNCTION          HUBPSM30_DisablingSuperSpeedOnPortOnDeviceRequest;

STATE_ENTRY_FUNCTION          HUBPSM30_Disconnected;

STATE_ENTRY_FUNCTION          HUBPSM30_DisconnectedSuperSpeedDisabled;

STATE_ENTRY_FUNCTION          HUBPSM30_DoesDeviceSupportsReAttach;

STATE_ENTRY_FUNCTION          HUBPSM30_DoesDeviceSupportsReAttachWithResetInProgress;

STATE_ENTRY_FUNCTION          HUBPSM30_DoesDeviceSupportsReAttachWithResetNeeded;

STATE_ENTRY_FUNCTION          HUBPSM30_EnablingSuperSpeedOnStopSuspend;

STATE_ENTRY_FUNCTION          HUBPSM30_EnablingSuperSpeedOnTimerExpiration;

STATE_ENTRY_FUNCTION          HUBPSM30_FlushingStaleDeviceEvents;

STATE_ENTRY_FUNCTION          HUBPSM30_FlushingStaleDeviceEventsWithResetInProgress;

STATE_ENTRY_FUNCTION          HUBPSM30_FlushingStaleDeviceEventsWithResetNeeded;

STATE_ENTRY_FUNCTION          HUBPSM30_FlushingUserResetForOverCurrentOnHubStopSuspend;

STATE_ENTRY_FUNCTION          HUBPSM30_FlushingUserResetForOverCurrentOnOverCurrentClear;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortChangeOnStartResumeFromConnectedError;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatusOnResetTimeout;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatusOnResumeRequest;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatusOnResumeTimeout;

STATE_ENTRY_FUNCTION          HUBPSM30_InitiatingResetPort;

STATE_ENTRY_FUNCTION          HUBPSM30_InitiatingResume;

STATE_ENTRY_FUNCTION          HUBPSM30_InitiatingWarmResetPort;

STATE_ENTRY_FUNCTION          HUBPSM30_IsDeviceInU0OnResumeRequest;

STATE_ENTRY_FUNCTION          HUBPSM30_IsDeviceInU0OnResumeTimeout;

STATE_ENTRY_FUNCTION          HUBPSM30_IsOldDevicePresent;

STATE_ENTRY_FUNCTION          HUBPSM30_IsOldDevicePresentInConnectedWithResetNeeded;

STATE_ENTRY_FUNCTION          HUBPSM30_IsOldDevicePresentWithResetInProgress;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingAttachDeviceToDsm;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingAttachDeviceWithResetToDsm;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingAttachDeviceWithResetToDsmWithResetInProgress;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingDetachDeviceOnCycleFromPoweredOffDisabled;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingDisabledToDSMFromDisabled;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingHubReset;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingHubResetFromDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingHubResetOnDisableFailure;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingHubResetOnTimedOut;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingHubResetWithPendingTimer;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingHubResetWithTimer;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingPortDisabledToDevice;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingPortDisableFailedToDevice;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingPortFailedToDSMOnLinkStateError;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingPortResetTimedOutToDSM;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingReAttachDeviceToBootDevice;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingReAttachDeviceToBootDeviceWithResetInProgress;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMWhileResetInProgress;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingResumedToDeviceSM;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingSuspendedToDeviceSM;

STATE_ENTRY_FUNCTION          HUBPSM30_NotifyingUserAboutPersistentOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM30_PoweredOffConnectedError;

STATE_ENTRY_FUNCTION          HUBPSM30_PoweredOffConnectedWithResetNeededAndDeviceDetached;

STATE_ENTRY_FUNCTION          HUBPSM30_PoweredOffDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM30_PoweredOffSuspended;

STATE_ENTRY_FUNCTION          HUBPSM30_QueryingPortStatusOnOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueingPortFailureToDSMOnTransferFailure;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueingPortFailureToDSMOnTransferFailureForLpmTimeout;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueingPortResumeFailedDueToPendingSuspend;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueingPortResumeTimedOut;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueingPortTimeoutUpdatedToDSM;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueStateDisabledToDSMOnStartFromConnectedError;

STATE_ENTRY_FUNCTION          HUBPSM30_ReleasingInterruptReferenceAfterResetFailure;

STATE_ENTRY_FUNCTION          HUBPSM30_ReleasingInterruptReferenceAndStoppingTimer;

STATE_ENTRY_FUNCTION          HUBPSM30_ReleasingInterruptReferenceOnResetTransferFailure;

STATE_ENTRY_FUNCTION          HUBPSM30_ReleasingInterruptReferenceOnResumed;

STATE_ENTRY_FUNCTION          HUBPSM30_ReleasingInterruptReferenceOnSuccessfulReset;

STATE_ENTRY_FUNCTION          HUBPSM30_ReleasingInterruptReferenceOnTransferFailureInResuming;

STATE_ENTRY_FUNCTION          HUBPSM30_ReleasingPortPowerReferenceInSuperSpeedDisabled;

STATE_ENTRY_FUNCTION          HUBPSM30_ReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded;

STATE_ENTRY_FUNCTION          HUBPSM30_ReleasingPowerReferenceonPoweredOffDisabled;

STATE_ENTRY_FUNCTION          HUBPSM30_ReleasingPowerReferenceonPoweredOffDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM30_ReleasingResetReferenceInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM30_ReleasingResetReferenceInSuperSpeedDisabled;

STATE_ENTRY_FUNCTION          HUBPSM30_ReleasingResetReferenceOnDeviceRemoval;

STATE_ENTRY_FUNCTION          HUBPSM30_ResettingHubAndFlushingUserInitiatedResetOnOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM30_SettingPortPowerOnOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM30_SettingRemotWakeMask;

STATE_ENTRY_FUNCTION          HUBPSM30_SettingRemotWakeMaskInDisconected;

STATE_ENTRY_FUNCTION          HUBPSM30_SettingU1TImeOut;

STATE_ENTRY_FUNCTION          HUBPSM30_SettingU2TImeOut;

STATE_ENTRY_FUNCTION          HUBPSM30_StartingAndAcquiringReferenceOnHubStart;

STATE_ENTRY_FUNCTION          HUBPSM30_StartingAndAcquiringReferenceOnSystemResume;

STATE_ENTRY_FUNCTION          HUBPSM30_StartingDisableSuperSpeedTimer;

STATE_ENTRY_FUNCTION          HUBPSM30_StartingOverCurrentTimer;

STATE_ENTRY_FUNCTION          HUBPSM30_StartingResumeTimer;

STATE_ENTRY_FUNCTION          HUBPSM30_StoppingDisableSuperSpeedTimer;

STATE_ENTRY_FUNCTION          HUBPSM30_StoppingResumeTimer;

STATE_ENTRY_FUNCTION          HUBPSM30_StoppingTimerAndQueueingHubResetInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM30_StoppingTimerOnDisconnect;

STATE_ENTRY_FUNCTION          HUBPSM30_StoppingTimerOnOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM30_StoppingTimerOnOverCurrentClear;

STATE_ENTRY_FUNCTION          HUBPSM30_StoppingTimerOnResetComplete;

STATE_ENTRY_FUNCTION          HUBPSM30_SuperSpeedDisabled;

STATE_ENTRY_FUNCTION          HUBPSM30_SuperSpeedDisabledStopped;

STATE_ENTRY_FUNCTION          HUBPSM30_Suspended;

STATE_ENTRY_FUNCTION          HUBPSM30_SuspendedDisabled;

STATE_ENTRY_FUNCTION          HUBPSM30_SuspendedInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_Suspending;

STATE_ENTRY_FUNCTION          HUBPSM30_SuspendingPortOnDisableRequest;

STATE_ENTRY_FUNCTION          HUBPSM30_SuspendingPortOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForDebounceTimer;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForHubStopSuspend;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForHubStopSuspendInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForHubStopSuspendOnAttachFailure;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForHubStopSuspendOnAttachWithResetFailure;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForHubStopSuspendOrTimer;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForOldDeviceToDetach;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForOldDeviceToDetachInConnectedWithResetNeeded;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForOldDeviceToDetachWithResetInProgress;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForOverCurrentClear;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForResetComplete;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForResetRequestFromDSMWithResetInPorgress;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForResetTimerToFlush;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForResumeComplete;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForResumeTimerToFlush;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForSuperSpeedDisableTimerToFlush;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForTimerOnResetInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForTimerToFlushOnDisconnect;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForTimerToFlushOnHubStopSuspend;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForTimerToFlushOnHubStopSuspendInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForTimerToFlushOnOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForTimerToFlushOnOverCurrentClear;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForUserResetOnOverCurrent;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingToBeDeleted;

STATE_ENTRY_FUNCTION          HUBPSM30_AckingPortChange;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckIfThereIsAPortChange;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckIfThereIsAPortChangeOnGetPortStatus;

STATE_ENTRY_FUNCTION          HUBPSM30_EnablingInterruptsAndGettingPortEvent;

STATE_ENTRY_FUNCTION          HUBPSM30_EnablingInterruptsOnFailure;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatus;

STATE_ENTRY_FUNCTION          HUBPSM30_InitializingCumulativePortChangeBits;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortResetHubOnError;

STATE_ENTRY_FUNCTION          HUBPSM30_AckingPortChangeInNoChange;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckIfThereIsAPortChangeInNoChange;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckIfThereIsAPortChangeOnGetPortStatusInNoChange;

STATE_ENTRY_FUNCTION          HUBPSM30_EnablingInterruptsAndGettingPortEventInNoChange;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatusInNoChange;

STATE_ENTRY_FUNCTION          HUBPSM30_InitializingCumulativePortChangeBitsInNoChange;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortResetHubOnErrorInNoChange;

STATE_ENTRY_FUNCTION          HUBPSM30_AckingPortChangeOnStartResume;

STATE_ENTRY_FUNCTION          HUBPSM30_AcquiringPortReferenceAndGettingPortEvent;

STATE_ENTRY_FUNCTION          HUBPSM30_AcquiringPortReferenceOnFailure;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckIfThereIsAPortChangeOnStartResume;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckIfThereIsAPortChangeOnStartResumeOnGetPortStatus;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatusOnStartResume;

STATE_ENTRY_FUNCTION          HUBPSM30_InitializingCumulativePortChangeBitsOnStartResume;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortResetHubOnErrorOnStartResume;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortChange;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningConnectChangeOnLinkError;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortResetHubOnErrorInPortChangeHandler;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForPortChangeEvent;

STATE_ENTRY_FUNCTION          HUBPSM30_AcquiringPortReferenceOnTransferFailure;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfPortIsPoweredOnStart;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfPowerOnStartHackIsPresent;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatusOnStart;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatusOnStartBeforePoweringOn;

STATE_ENTRY_FUNCTION          HUBPSM30_PoweringOnPortOnStart;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortConnectChangeOnLinkErrorOnStart;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortResetHubOnErrorOnStart;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForPortPowerOnTimerOnStart;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfDeviceIsStillConnectedOnResume;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatusOnResume;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatusOnResumeAfterWaiting;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueingStateDisabledOnConnectChangeOnResume;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueingStateDisabledOnFailure;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueingStateDisabledOnResetInProgress;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueingStateEnabledOnResume;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueingStateSuspendedOnResume;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningHubStopOnResume;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortConnectChange;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortNeedsResetOnResume;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortOperationSuccessOnResume;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortResetHubOnErrorOnResume;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortResetInProgressOnResume;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortResumedOnResume;

STATE_ENTRY_FUNCTION          HUBPSM30_StartingTimerForAllowingReconnect;

STATE_ENTRY_FUNCTION          HUBPSM30_StoppingReconnectTimerOnHubStatusChange;

STATE_ENTRY_FUNCTION          HUBPSM30_StoppingReconnectTimerOnHubStop;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForDeviceToReconnect;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForReconnectTimerToFlushOnHubStatusChange;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForReconnectTimerToFlushOnHubStop;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatusInResuming;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningConnectChangeOnLinkErrorInResuming;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortResetHubOnErrorInResuming;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForPortChangeEventInResuming;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatusInWaitingForStopSuspend;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueingPortEventFailureToDSM;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForDevicePortEvents;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatusInWaitingForStopSuspendInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForDevicePortEventsInDisconnected;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfLinkIsInU0AfterResetPolling;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfResetCompletionRequiresPolling;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatusInReset;

STATE_ENTRY_FUNCTION          HUBPSM30_GettingPortStatusInResetAfterPollingInterval;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortResetCompleted;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortResetHubOnErrorInReset;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForPollingIntervalForResetCompletion;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForPortChangeEventInReset;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfBootDeviceIsConnected;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfParentHubIsArmedForWake;

STATE_ENTRY_FUNCTION          HUBPSM30_DisablingRemoteWakeOnPort;

STATE_ENTRY_FUNCTION          HUBPSM30_EnablingRemoteWakeOnPort;

STATE_ENTRY_FUNCTION          HUBPSM30_IssuingHubResetOnRemoteWakeMaskFailure;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningOperationSucessFromArmingForWake;

STATE_ENTRY_FUNCTION          HUBPSM30_AcquiringPowerReferenceOnHubResumeInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfDeviceIsConnectedOnConnectChangeForD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfDeviceIsConnectedOnPortChangeInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_CheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume;

STATE_ENTRY_FUNCTION          HUBPSM30_PoweredOffOnHubSuspendFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueingEnabledToDSMOnReconnectAfterResumeRequestFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueingEnabledToDSMOnReconnectFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_QueueingStateSuspendedOnHubResumeInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_ReleasingPowerReferenceOnHubS0IdleInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningConnectChangeFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningHubStopFromSuspendedInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningOverCurrentClearedFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningOverCurrentFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortResetHubOnErrorFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_ReturningPortResumedFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_StartingTimerForAllowingReconnectOnResumingFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_StoppingReconnectTimerOnDeviceReAttachAfterD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_StoppingReconnectTimerOnHubStopInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_StoppingReconnectTimerOnOvercurrentClearedInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_StoppingReconnectTimerOnOvercurrentInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_StoppingReconnectTimerOnResetHubErrorInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForDeviceRequestResumeOnD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForDeviceToReconnectOnResumeFromD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForPortChangeEventInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForReconnectTimerToFlushOnHubStopInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForReconnectTimerToFlushOnOvercurrentInD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForReconnectTimerToFlushOnReattachAfterD3Cold;

STATE_ENTRY_FUNCTION          HUBPSM30_WaitingForReconnectTimerToFlushOnResetHubErrorInD3Cold;




//
// Sub State Entries for the states in the State Machine
//


SUBSM_ENTRY Psm30SubSmConnectedEnabled[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmConnectedError[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmDisconnected[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmDisconnectedSuperSpeedDisabled[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForDevicePortEventsInDisconnected },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmGettingPortChangeOnStartResumeFromConnectedError[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateInitializingCumulativePortChangeBitsOnStartResume },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmGettingPortStatusOnResetTimeout[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateInitializingCumulativePortChangeBitsInNoChange },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmIssuingHubReset[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForDevicePortEvents },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmIssuingHubResetFromDisconnected[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForDevicePortEventsInDisconnected },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmSettingRemotWakeMask[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateCheckingIfParentHubIsArmedForWake },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmSettingRemotWakeMaskInDisconected[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateCheckingIfParentHubIsArmedForWake },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmStartingAndAcquiringReferenceOnHubStart[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateGettingPortStatusOnStartBeforePoweringOn },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmStartingAndAcquiringReferenceOnSystemResume[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateGettingPortStatusOnResume },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmSuperSpeedDisabled[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForDevicePortEventsInDisconnected },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmSuspended[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmSuspendedDisabled[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmSuspendedInD3Cold[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEventInD3Cold },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForDebounceTimer[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForHubStopSuspend[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForDevicePortEvents },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForHubStopSuspendInDisconnected[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForDevicePortEventsInDisconnected },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForHubStopSuspendOnAttachFailure[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForDevicePortEventsInDisconnected },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForHubStopSuspendOnAttachWithResetFailure[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForDevicePortEventsInDisconnected },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForHubStopSuspendOrTimer[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForDevicePortEvents },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForOldDeviceToDetach[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForOldDeviceToDetachInConnectedWithResetNeeded[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForOldDeviceToDetachWithResetInProgress[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForOverCurrentClear[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForResetComplete[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEventInReset },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForResetRequestFromDSMWithResetInPorgress[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEventInReset },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForResetTimerToFlush[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForResumeComplete[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEventInResuming },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForResumeTimerToFlush[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForTimerOnResetInDisconnected[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForDevicePortEventsInDisconnected },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForTimerToFlushOnDisconnect[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForTimerToFlushOnOverCurrent[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForTimerToFlushOnOverCurrentClear[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForUserResetOnOverCurrent[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmGettingPortChange[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateInitializingCumulativePortChangeBits },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmGettingPortStatusOnStart[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateInitializingCumulativePortChangeBitsOnStartResume },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmGettingPortStatusOnResume[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateInitializingCumulativePortChangeBitsOnStartResume },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmGettingPortStatusOnResumeAfterWaiting[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateInitializingCumulativePortChangeBits },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmGettingPortStatusInResuming[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateInitializingCumulativePortChangeBits },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmGettingPortStatusInWaitingForStopSuspend[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateInitializingCumulativePortChangeBits },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmGettingPortStatusInWaitingForStopSuspendInDisconnected[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateInitializingCumulativePortChangeBits },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmGettingPortStatusInReset[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateInitializingCumulativePortChangeBits },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmGettingPortStatusInResetAfterPollingInterval[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateInitializingCumulativePortChangeBitsInNoChange },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForDeviceRequestResumeOnD3Cold[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForDeviceToReconnectOnResumeFromD3Cold[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


SUBSM_ENTRY Psm30SubSmWaitingForPortChangeEventInD3Cold[] = {
    // SubSmFilter, SubSmInitialState
    { Psm30SubSmFlagAny , Psm30StateWaitingForPortChangeEvent },
    { Psm30SubSmFlagNone , Psm30StateNull }
};


//
// State Entries for the states in the State Machine
//
STATE_ENTRY   Psm30StateEntryAcquiringInterruptReferenceOnHSMInDisabled = {
    // State ETW Name
    Psm30StateEtwAcquiringInterruptReferenceOnHSMInDisabled,
    // State Entry Function
    HUBPSM30_AcquiringInterruptReferenceOnHSMInDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateInitiatingWarmResetPort },
        { PsmEventOperationFailure ,    Psm30StateIssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAcquiringInterruptReferenceOnHSMInEnabled = {
    // State ETW Name
    Psm30StateEtwAcquiringInterruptReferenceOnHSMInEnabled,
    // State Entry Function
    HUBPSM30_AcquiringInterruptReferenceOnHSMInEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateInitiatingResetPort },
        { PsmEventOperationFailure ,    Psm30StateIssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAcquiringInterruptReferenceOnHSMInEnabledOnWarmReset = {
    // State ETW Name
    Psm30StateEtwAcquiringInterruptReferenceOnHSMInEnabledOnWarmReset,
    // State Entry Function
    HUBPSM30_AcquiringInterruptReferenceOnHSMInEnabledOnWarmReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateInitiatingWarmResetPort },
        { PsmEventOperationFailure ,    Psm30StateIssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAcquiringInterruptReferenceOnHSMOnResetRequest = {
    // State ETW Name
    Psm30StateEtwAcquiringInterruptReferenceOnHSMOnResetRequest,
    // State Entry Function
    HUBPSM30_AcquiringInterruptReferenceOnHSMOnResetRequest,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateIssuingResetFailedDuetoPendingHubSuspendToDSMWhileResetInProgress },
        { PsmEventOperationSuccess ,    Psm30StateWaitingForResetComplete },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAcquiringInterruptReferenceOnResuming = {
    // State ETW Name
    Psm30StateEtwAcquiringInterruptReferenceOnResuming,
    // State Entry Function
    HUBPSM30_AcquiringInterruptReferenceOnResuming,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateGettingPortStatusOnResumeRequest },
        { PsmEventOperationFailure ,    Psm30StateQueueingPortResumeFailedDueToPendingSuspend },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAcquiringPowerReferenceAfterDisablingSuperSpeed = {
    // State ETW Name
    Psm30StateEtwAcquiringPowerReferenceAfterDisablingSuperSpeed,
    // State Entry Function
    HUBPSM30_AcquiringPowerReferenceAfterDisablingSuperSpeed,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateSuperSpeedDisabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAcquiringPowerReferenceonPoweredOffConnectedErrorInS0 = {
    // State ETW Name
    Psm30StateEtwAcquiringPowerReferenceonPoweredOffConnectedErrorInS0,
    // State Entry Function
    HUBPSM30_AcquiringPowerReferenceonPoweredOffConnectedErrorInS0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateConnectedError },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAcquiringPowerReferenceonPoweredOffDisconnectedInS0 = {
    // State ETW Name
    Psm30StateEtwAcquiringPowerReferenceonPoweredOffDisconnectedInS0,
    // State Entry Function
    HUBPSM30_AcquiringPowerReferenceonPoweredOffDisconnectedInS0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAcquiringPowerReferenceonPoweredOffSuspendedInS0 = {
    // State ETW Name
    Psm30StateEtwAcquiringPowerReferenceonPoweredOffSuspendedInS0,
    // State Entry Function
    HUBPSM30_AcquiringPowerReferenceonPoweredOffSuspendedInS0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateSuspended },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAcquiringReferenceInConnectedWithResetNeeded = {
    // State ETW Name
    Psm30StateEtwAcquiringReferenceInConnectedWithResetNeeded,
    // State Entry Function
    HUBPSM30_AcquiringReferenceInConnectedWithResetNeeded,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateIsOldDevicePresentInConnectedWithResetNeeded },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCancellingResetOnSurpriseRemove = {
    // State ETW Name
    Psm30StateEtwCancellingResetOnSurpriseRemove,
    // State Entry Function
    HUBPSM30_CancellingResetOnSurpriseRemove,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm30StateWaitingForHubStopSuspend },
        { PsmEventTransferSuccess ,     Psm30StateWaitingForHubStopSuspend },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCancellingSetPortPower = {
    // State ETW Name
    Psm30StateEtwCancellingSetPortPower,
    // State Entry Function
    HUBPSM30_CancellingSetPortPower,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventTransferFailure ,     Psm30StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventTransferSuccess ,     Psm30StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfADeviceIsPresent = {
    // State ETW Name
    Psm30StateEtwCheckingIfADeviceIsPresent,
    // State Entry Function
    HUBPSM30_CheckingIfADeviceIsPresent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateCheckingIfLinkStateIsU0 },
        { PsmEventNo ,                  Psm30StateDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfDebounceIsNeeded = {
    // State ETW Name
    Psm30StateEtwCheckingIfDebounceIsNeeded,
    // State Entry Function
    HUBPSM30_CheckingIfDebounceIsNeeded,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateWaitingForDebounceTimer },
        { PsmEventNo ,                  Psm30StateDoesDeviceSupportsReAttach },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfLinkStateIsU0 = {
    // State ETW Name
    Psm30StateEtwCheckingIfLinkStateIsU0,
    // State Entry Function
    HUBPSM30_CheckingIfLinkStateIsU0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateIsOldDevicePresent },
        { PsmEventNo ,                  Psm30StateIsOldDevicePresentInConnectedWithResetNeeded },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfOvercurrentBitOne = {
    // State ETW Name
    Psm30StateEtwCheckingIfOvercurrentBitOne,
    // State Entry Function
    HUBPSM30_CheckingIfOvercurrentBitOne,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateStartingOverCurrentTimer },
        { PsmEventNo ,                  Psm30StateDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfPersistentOvercurrent = {
    // State ETW Name
    Psm30StateEtwCheckingIfPersistentOvercurrent,
    // State Entry Function
    HUBPSM30_CheckingIfPersistentOvercurrent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateNotifyingUserAboutPersistentOverCurrent },
        { PsmEventNo ,                  Psm30StateSettingPortPowerOnOverCurrent },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfSuperSpeedShouldBeDisabled = {
    // State ETW Name
    Psm30StateEtwCheckingIfSuperSpeedShouldBeDisabled,
    // State Entry Function
    HUBPSM30_CheckingIfSuperSpeedShouldBeDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateDisablingSuperSpeedOnPortForUnsupportedHubs },
        { PsmEventNo ,                  Psm30StateStartingAndAcquiringReferenceOnHubStart },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIsD3ColdIsEnabled = {
    // State ETW Name
    Psm30StateEtwCheckingIsD3ColdIsEnabled,
    // State Entry Function
    HUBPSM30_CheckingIsD3ColdIsEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateSuspendedInD3Cold },
        { PsmEventNo ,                  Psm30StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryConnectedEnabled = {
    // State ETW Name
    Psm30StateEtwConnectedEnabled,
    // State Entry Function
    HUBPSM30_ConnectedEnabled,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm30SubSmConnectedEnabled,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm30StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubReset },
        { PsmEventPortOverCurrent ,     Psm30StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventDeviceRequestDisable ,Psm30StateSuspendingPortOnDisableRequest },
        { PsmEventDeviceRequestSuspend ,Psm30StateSuspending },
        { PsmEventDevicePrepareForHibernation ,Psm30StateIssuingSuspendedToDeviceSM },
        { PsmEventDeviceRequestReset ,  Psm30StateAcquiringInterruptReferenceOnHSMInEnabled },
        { PsmEventDeviceRequestWarmReset ,Psm30StateAcquiringInterruptReferenceOnHSMInEnabledOnWarmReset },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventDeviceRequestCycle ,  Psm30StateDetachingDeviceFromPortOnPortCycle },
        { PsmEventDeviceSetU1Timeout ,  Psm30StateSettingU1TImeOut },
        { PsmEventDeviceSetU2Timeout ,  Psm30StateSettingU2TImeOut },
        { PsmEventDeviceRequestDisableSuperSpeed ,Psm30StateDisablingSuperSpeedOnPortOnDeviceRequest },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryConnectedError = {
    // State ETW Name
    Psm30StateEtwConnectedError,
    // State Entry Function
    HUBPSM30_ConnectedError,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    Psm30SubSmConnectedError,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm30StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubReset },
        { PsmEventPortOverCurrent ,     Psm30StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventDeviceRequestCycle ,  Psm30StateDetachingDeviceFromPortOnPortCycle },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventDeviceRequestReset ,  Psm30StateAcquiringInterruptReferenceOnHSMInDisabled },
        { PsmEventDeviceRequestDisableSuperSpeed ,Psm30StateDisablingSuperSpeedOnPortOnDeviceRequest },
        { PsmEventDeviceRequestDisable ,Psm30StateSuspendingPortOnDisableRequest },
        { PsmEventDeviceRequestWarmReset ,Psm30StateAcquiringInterruptReferenceOnHSMInDisabled },
        { PsmEventHubSuspend ,          Psm30StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCreatingDevice = {
    // State ETW Name
    Psm30StateEtwCreatingDevice,
    // State Entry Function
    HUBPSM30_CreatingDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateIssuingAttachDeviceToDsm },
        { PsmEventOperationFailure ,    Psm30StateIsOldDevicePresent },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCreatingDeviceInConnectedWithResetInProgress = {
    // State ETW Name
    Psm30StateEtwCreatingDeviceInConnectedWithResetInProgress,
    // State Entry Function
    HUBPSM30_CreatingDeviceInConnectedWithResetInProgress,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateCreatingDeviceInConnectedWithResetInProgress },
        { PsmEventOperationSuccess ,    Psm30StateIssuingAttachDeviceWithResetToDsmWithResetInProgress },
        { PsmEventOperationFailure ,    Psm30StateCreatingDeviceInConnectedWithResetInProgress },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCreatingDeviceInConnectedWithResetNeeded = {
    // State ETW Name
    Psm30StateEtwCreatingDeviceInConnectedWithResetNeeded,
    // State Entry Function
    HUBPSM30_CreatingDeviceInConnectedWithResetNeeded,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateIsOldDevicePresentInConnectedWithResetNeeded },
        { PsmEventOperationSuccess ,    Psm30StateIssuingAttachDeviceWithResetToDsm },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDetachingDeviceFromPortOnDisconnect = {
    // State ETW Name
    Psm30StateEtwDetachingDeviceFromPortOnDisconnect,
    // State Entry Function
    HUBPSM30_DetachingDeviceFromPortOnDisconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateCheckingIfADeviceIsPresent },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDetachingDeviceFromPortOnDisconnectOnTimedOut = {
    // State ETW Name
    Psm30StateEtwDetachingDeviceFromPortOnDisconnectOnTimedOut,
    // State Entry Function
    HUBPSM30_DetachingDeviceFromPortOnDisconnectOnTimedOut,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateCheckingIfADeviceIsPresent },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDetachingDeviceFromPortOnDisconnectWithTimer = {
    // State ETW Name
    Psm30StateEtwDetachingDeviceFromPortOnDisconnectWithTimer,
    // State Entry Function
    HUBPSM30_DetachingDeviceFromPortOnDisconnectWithTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateStoppingTimerOnDisconnect },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDetachingDeviceFromPortOnOverCurrent = {
    // State ETW Name
    Psm30StateEtwDetachingDeviceFromPortOnOverCurrent,
    // State Entry Function
    HUBPSM30_DetachingDeviceFromPortOnOverCurrent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateStartingOverCurrentTimer },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDetachingDeviceFromPortOnOverCurrentClear = {
    // State ETW Name
    Psm30StateEtwDetachingDeviceFromPortOnOverCurrentClear,
    // State Entry Function
    HUBPSM30_DetachingDeviceFromPortOnOverCurrentClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateSettingPortPowerOnOverCurrent },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDetachingDeviceFromPortOnOverCurrentClearOnTimedOut = {
    // State ETW Name
    Psm30StateEtwDetachingDeviceFromPortOnOverCurrentClearOnTimedOut,
    // State Entry Function
    HUBPSM30_DetachingDeviceFromPortOnOverCurrentClearOnTimedOut,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateSettingPortPowerOnOverCurrent },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDetachingDeviceFromPortOnOverCurrentClearWithTimer = {
    // State ETW Name
    Psm30StateEtwDetachingDeviceFromPortOnOverCurrentClearWithTimer,
    // State Entry Function
    HUBPSM30_DetachingDeviceFromPortOnOverCurrentClearWithTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateStoppingTimerOnOverCurrentClear },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDetachingDeviceFromPortOnOverCurrentOnTimedOut = {
    // State ETW Name
    Psm30StateEtwDetachingDeviceFromPortOnOverCurrentOnTimedOut,
    // State Entry Function
    HUBPSM30_DetachingDeviceFromPortOnOverCurrentOnTimedOut,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateStartingOverCurrentTimer },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDetachingDeviceFromPortOnOverCurrentWithTimer = {
    // State ETW Name
    Psm30StateEtwDetachingDeviceFromPortOnOverCurrentWithTimer,
    // State Entry Function
    HUBPSM30_DetachingDeviceFromPortOnOverCurrentWithTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateStoppingTimerOnOverCurrent },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDetachingDeviceFromPortOnPortCycle = {
    // State ETW Name
    Psm30StateEtwDetachingDeviceFromPortOnPortCycle,
    // State Entry Function
    HUBPSM30_DetachingDeviceFromPortOnPortCycle,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateWaitingForOldDeviceToDetachInConnectedWithResetNeeded },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDetachingDeviceOnCleanup = {
    // State ETW Name
    Psm30StateEtwDetachingDeviceOnCleanup,
    // State Entry Function
    HUBPSM30_DetachingDeviceOnCleanup,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm30StateWaitingToBeDeleted },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDisablingSuperSpeedOnPortForUnsupportedHubs = {
    // State ETW Name
    Psm30StateEtwDisablingSuperSpeedOnPortForUnsupportedHubs,
    // State Entry Function
    HUBPSM30_DisablingSuperSpeedOnPortForUnsupportedHubs,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferSuccess ,     Psm30StateAcquiringPowerReferenceAfterDisablingSuperSpeed },
        { PsmEventTransferFailure ,     Psm30StateAcquiringPowerReferenceAfterDisablingSuperSpeed },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDisablingSuperSpeedOnPortOnDeviceRequest = {
    // State ETW Name
    Psm30StateEtwDisablingSuperSpeedOnPortOnDeviceRequest,
    // State Entry Function
    HUBPSM30_DisablingSuperSpeedOnPortOnDeviceRequest,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm30StateIssuingHubResetFromDisconnected },
        { PsmEventTransferSuccess ,     Psm30StateStartingDisableSuperSpeedTimer },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDisconnected = {
    // State ETW Name
    Psm30StateEtwDisconnected,
    // State Entry Function
    HUBPSM30_Disconnected,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    Psm30SubSmDisconnected,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceRequestSuspend ,Psm30StateIgnored },
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventDeviceSetU2Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceRequestReset ,  Psm30StateIgnored },
        { PsmEventDeviceSetU1Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceRequestDisable ,Psm30StateIgnored },
        { PsmEventDeviceRequestCycle ,  Psm30StateIgnored },
        { PsmEventDeviceRequestWarmReset ,Psm30StateIgnored },
        { PsmEventDevicePrepareForHibernation ,Psm30StateIgnored },
        { PsmEventDeviceRequestDisableSuperSpeed ,Psm30StateIgnored },
        { PsmEventDeviceRequestResume , Psm30StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm30StateSettingPortPowerOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateCheckingIfADeviceIsPresent },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubResetFromDisconnected },
        { PsmEventPortOverCurrent ,     Psm30StateStartingOverCurrentTimer },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventHubSuspend ,          Psm30StateSettingRemotWakeMaskInDisconected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDisconnectedSuperSpeedDisabled = {
    // State ETW Name
    Psm30StateEtwDisconnectedSuperSpeedDisabled,
    // State Entry Function
    HUBPSM30_DisconnectedSuperSpeedDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmDisconnectedSuperSpeedDisabled,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventTimerFired ,          Psm30StateEnablingSuperSpeedOnTimerExpiration },
        { PsmEventHubSuspend ,          Psm30StateEnablingSuperSpeedOnStopSuspend },
        { PsmEventHubStop ,             Psm30StateEnablingSuperSpeedOnStopSuspend },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDoesDeviceSupportsReAttach = {
    // State ETW Name
    Psm30StateEtwDoesDeviceSupportsReAttach,
    // State Entry Function
    HUBPSM30_DoesDeviceSupportsReAttach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateIssuingReAttachDeviceToBootDevice },
        { PsmEventNo ,                  Psm30StateCreatingDevice },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDoesDeviceSupportsReAttachWithResetInProgress = {
    // State ETW Name
    Psm30StateEtwDoesDeviceSupportsReAttachWithResetInProgress,
    // State Entry Function
    HUBPSM30_DoesDeviceSupportsReAttachWithResetInProgress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateIssuingReAttachDeviceToBootDeviceWithResetInProgress },
        { PsmEventNo ,                  Psm30StateCreatingDeviceInConnectedWithResetInProgress },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDoesDeviceSupportsReAttachWithResetNeeded = {
    // State ETW Name
    Psm30StateEtwDoesDeviceSupportsReAttachWithResetNeeded,
    // State Entry Function
    HUBPSM30_DoesDeviceSupportsReAttachWithResetNeeded,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateIssuingReAttachDeviceToBootDevice },
        { PsmEventNo ,                  Psm30StateCreatingDeviceInConnectedWithResetNeeded },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryEnablingSuperSpeedOnStopSuspend = {
    // State ETW Name
    Psm30StateEtwEnablingSuperSpeedOnStopSuspend,
    // State Entry Function
    HUBPSM30_EnablingSuperSpeedOnStopSuspend,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferSuccess ,     Psm30StateStoppingDisableSuperSpeedTimer },
        { PsmEventTransferFailure ,     Psm30StateStoppingDisableSuperSpeedTimer },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryEnablingSuperSpeedOnTimerExpiration = {
    // State ETW Name
    Psm30StateEtwEnablingSuperSpeedOnTimerExpiration,
    // State Entry Function
    HUBPSM30_EnablingSuperSpeedOnTimerExpiration,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferSuccess ,     Psm30StateDisconnected },
        { PsmEventTransferFailure ,     Psm30StateIssuingHubResetFromDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryFlushingStaleDeviceEvents = {
    // State ETW Name
    Psm30StateEtwFlushingStaleDeviceEvents,
    // State Entry Function
    HUBPSM30_FlushingStaleDeviceEvents,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceRequestWarmReset ,Psm30StateIgnored },
        { PsmEventDevicePrepareForHibernation ,Psm30StateIgnored },
        { PsmEventDeviceRequestReset ,  Psm30StateIgnored },
        { PsmEventDeviceRequestCycle ,  Psm30StateIgnored },
        { PsmEventDeviceRequestResume , Psm30StateIgnored },
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventDeviceSetU1Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceRequestSuspend ,Psm30StateIgnored },
        { PsmEventDeviceRequestDisable ,Psm30StateIgnored },
        { PsmEventDeviceSetU2Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceRequestDisableSuperSpeed ,Psm30StateIgnored },
        { PsmEventOperationSuccess ,    Psm30StateCheckingIfDebounceIsNeeded },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryFlushingStaleDeviceEventsWithResetInProgress = {
    // State ETW Name
    Psm30StateEtwFlushingStaleDeviceEventsWithResetInProgress,
    // State Entry Function
    HUBPSM30_FlushingStaleDeviceEventsWithResetInProgress,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceSetU1Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventDevicePrepareForHibernation ,Psm30StateIgnored },
        { PsmEventDeviceRequestSuspend ,Psm30StateIgnored },
        { PsmEventDeviceRequestCycle ,  Psm30StateIgnored },
        { PsmEventDeviceRequestReset ,  Psm30StateIgnored },
        { PsmEventDeviceRequestDisable ,Psm30StateIgnored },
        { PsmEventDeviceRequestResume , Psm30StateIgnored },
        { PsmEventDeviceSetU2Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceRequestWarmReset ,Psm30StateIgnored },
        { PsmEventDeviceRequestDisableSuperSpeed ,Psm30StateIgnored },
        { PsmEventOperationSuccess ,    Psm30StateDoesDeviceSupportsReAttachWithResetInProgress },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryFlushingStaleDeviceEventsWithResetNeeded = {
    // State ETW Name
    Psm30StateEtwFlushingStaleDeviceEventsWithResetNeeded,
    // State Entry Function
    HUBPSM30_FlushingStaleDeviceEventsWithResetNeeded,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceSetU1Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceRequestCycle ,  Psm30StateIgnored },
        { PsmEventDeviceRequestWarmReset ,Psm30StateIgnored },
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventDeviceRequestSuspend ,Psm30StateIgnored },
        { PsmEventDeviceRequestResume , Psm30StateIgnored },
        { PsmEventDeviceRequestDisable ,Psm30StateIgnored },
        { PsmEventDeviceRequestReset ,  Psm30StateIgnored },
        { PsmEventDevicePrepareForHibernation ,Psm30StateIgnored },
        { PsmEventDeviceSetU2Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceRequestDisableSuperSpeed ,Psm30StateIgnored },
        { PsmEventOperationSuccess ,    Psm30StateDoesDeviceSupportsReAttachWithResetNeeded },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryFlushingUserResetForOverCurrentOnHubStopSuspend = {
    // State ETW Name
    Psm30StateEtwFlushingUserResetForOverCurrentOnHubStopSuspend,
    // State Entry Function
    HUBPSM30_FlushingUserResetForOverCurrentOnHubStopSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventUserInitiatedResetOnOverCurrent ,Psm30StateIgnored },
        { PsmEventOperationSuccess ,    Psm30StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryFlushingUserResetForOverCurrentOnOverCurrentClear = {
    // State ETW Name
    Psm30StateEtwFlushingUserResetForOverCurrentOnOverCurrentClear,
    // State Entry Function
    HUBPSM30_FlushingUserResetForOverCurrentOnOverCurrentClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventUserInitiatedResetOnOverCurrent ,Psm30StateIgnored },
        { PsmEventOperationSuccess ,    Psm30StateSettingPortPowerOnOverCurrent },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortChangeOnStartResumeFromConnectedError = {
    // State ETW Name
    Psm30StateEtwGettingPortChangeOnStartResumeFromConnectedError,
    // State Entry Function
    HUBPSM30_GettingPortChangeOnStartResumeFromConnectedError,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmGettingPortChangeOnStartResumeFromConnectedError,
    // Event State Pairs for Transitions
    {
        { PsmEventPortOverCurrentCleared ,Psm30StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubReset },
        { PsmEventPortOverCurrent ,     Psm30StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventPortResetInProgress , Psm30StateWaitingForResetRequestFromDSMWithResetInPorgress },
        { PsmEventPortResumed ,         Psm30StateConnectedError },
        { PsmEventPortLinkStateError ,  Psm30StateConnectedError },
        { PsmEventPortError ,           Psm30StateConnectedError },
        { PsmEventPortNoChange ,        Psm30StateConnectedError },
        { PsmEventPortResetComplete ,   Psm30StateConnectedError },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatusOnResetTimeout = {
    // State ETW Name
    Psm30StateEtwGettingPortStatusOnResetTimeout,
    // State Entry Function
    HUBPSM30_GettingPortStatusOnResetTimeout,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    Psm30SubSmGettingPortStatusOnResetTimeout,
    // Event State Pairs for Transitions
    {
        { PsmEventPortNoChange ,        Psm30StateIssuingPortResetTimedOutToDSM },
        { PsmEventPortResetComplete ,   Psm30StateReleasingInterruptReferenceOnSuccessfulReset },
        { PsmEventPortResetInProgress , Psm30StateIssuingHubResetOnTimedOut },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubResetOnTimedOut },
        { PsmEventPortResumed ,         Psm30StateIssuingHubResetOnTimedOut },
        { PsmEventPortError ,           Psm30StateIssuingHubResetOnTimedOut },
        { PsmEventPortOverCurrent ,     Psm30StateDetachingDeviceFromPortOnOverCurrentOnTimedOut },
        { PsmEventPortOverCurrentCleared ,Psm30StateDetachingDeviceFromPortOnOverCurrentClearOnTimedOut },
        { PsmEventPortConnectChange ,   Psm30StateDetachingDeviceFromPortOnDisconnectOnTimedOut },
        { PsmEventPortLinkStateError ,  Psm30StateIssuingPortFailedToDSMOnLinkStateError },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatusOnResumeRequest = {
    // State ETW Name
    Psm30StateEtwGettingPortStatusOnResumeRequest,
    // State Entry Function
    HUBPSM30_GettingPortStatusOnResumeRequest,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferSuccess ,     Psm30StateIsDeviceInU0OnResumeRequest },
        { PsmEventTransferFailure ,     Psm30StateReleasingInterruptReferenceOnTransferFailureInResuming },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatusOnResumeTimeout = {
    // State ETW Name
    Psm30StateEtwGettingPortStatusOnResumeTimeout,
    // State Entry Function
    HUBPSM30_GettingPortStatusOnResumeTimeout,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferSuccess ,     Psm30StateIsDeviceInU0OnResumeTimeout },
        { PsmEventTransferFailure ,     Psm30StateQueueingPortFailureToDSMOnTransferFailure },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryInitiatingResetPort = {
    // State ETW Name
    Psm30StateEtwInitiatingResetPort,
    // State Entry Function
    HUBPSM30_InitiatingResetPort,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferSuccess ,     Psm30StateWaitingForResetComplete },
        { PsmEventTransferFailure ,     Psm30StateReleasingInterruptReferenceOnResetTransferFailure },
        { PsmEventHubSurpriseRemove ,   Psm30StateCancellingResetOnSurpriseRemove },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryInitiatingResume = {
    // State ETW Name
    Psm30StateEtwInitiatingResume,
    // State Entry Function
    HUBPSM30_InitiatingResume,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventTransferSuccess ,     Psm30StateStartingResumeTimer },
        { PsmEventTransferFailure ,     Psm30StateReleasingInterruptReferenceOnTransferFailureInResuming },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryInitiatingWarmResetPort = {
    // State ETW Name
    Psm30StateEtwInitiatingWarmResetPort,
    // State Entry Function
    HUBPSM30_InitiatingWarmResetPort,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferSuccess ,     Psm30StateWaitingForResetComplete },
        { PsmEventTransferFailure ,     Psm30StateReleasingInterruptReferenceOnResetTransferFailure },
        { PsmEventHubSurpriseRemove ,   Psm30StateCancellingResetOnSurpriseRemove },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIsDeviceInU0OnResumeRequest = {
    // State ETW Name
    Psm30StateEtwIsDeviceInU0OnResumeRequest,
    // State Entry Function
    HUBPSM30_IsDeviceInU0OnResumeRequest,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateReleasingInterruptReferenceOnResumed },
        { PsmEventNo ,                  Psm30StateInitiatingResume },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIsDeviceInU0OnResumeTimeout = {
    // State ETW Name
    Psm30StateEtwIsDeviceInU0OnResumeTimeout,
    // State Entry Function
    HUBPSM30_IsDeviceInU0OnResumeTimeout,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateReleasingInterruptReferenceOnResumed },
        { PsmEventNo ,                  Psm30StateQueueingPortResumeTimedOut },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIsOldDevicePresent = {
    // State ETW Name
    Psm30StateEtwIsOldDevicePresent,
    // State Entry Function
    HUBPSM30_IsOldDevicePresent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateWaitingForOldDeviceToDetach },
        { PsmEventNo ,                  Psm30StateFlushingStaleDeviceEvents },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIsOldDevicePresentInConnectedWithResetNeeded = {
    // State ETW Name
    Psm30StateEtwIsOldDevicePresentInConnectedWithResetNeeded,
    // State Entry Function
    HUBPSM30_IsOldDevicePresentInConnectedWithResetNeeded,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateWaitingForOldDeviceToDetachInConnectedWithResetNeeded },
        { PsmEventNo ,                  Psm30StateFlushingStaleDeviceEventsWithResetNeeded },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIsOldDevicePresentWithResetInProgress = {
    // State ETW Name
    Psm30StateEtwIsOldDevicePresentWithResetInProgress,
    // State Entry Function
    HUBPSM30_IsOldDevicePresentWithResetInProgress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateWaitingForOldDeviceToDetachWithResetInProgress },
        { PsmEventNo ,                  Psm30StateFlushingStaleDeviceEventsWithResetInProgress },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingAttachDeviceToDsm = {
    // State ETW Name
    Psm30StateEtwIssuingAttachDeviceToDsm,
    // State Entry Function
    HUBPSM30_IssuingAttachDeviceToDsm,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForHubStopSuspendOnAttachFailure },
        { PsmEventOperationSuccess ,    Psm30StateConnectedEnabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingAttachDeviceWithResetToDsm = {
    // State ETW Name
    Psm30StateEtwIssuingAttachDeviceWithResetToDsm,
    // State Entry Function
    HUBPSM30_IssuingAttachDeviceWithResetToDsm,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForHubStopSuspendOnAttachWithResetFailure },
        { PsmEventOperationSuccess ,    Psm30StateConnectedError },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingAttachDeviceWithResetToDsmWithResetInProgress = {
    // State ETW Name
    Psm30StateEtwIssuingAttachDeviceWithResetToDsmWithResetInProgress,
    // State Entry Function
    HUBPSM30_IssuingAttachDeviceWithResetToDsmWithResetInProgress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForHubStopSuspendOnAttachWithResetFailure },
        { PsmEventOperationSuccess ,    Psm30StateWaitingForResetRequestFromDSMWithResetInPorgress },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingDetachDeviceOnCycleFromPoweredOffDisabled = {
    // State ETW Name
    Psm30StateEtwIssuingDetachDeviceOnCycleFromPoweredOffDisabled,
    // State Entry Function
    HUBPSM30_IssuingDetachDeviceOnCycleFromPoweredOffDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StatePoweredOffConnectedWithResetNeededAndDeviceDetached },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingDisabledToDSMFromDisabled = {
    // State ETW Name
    Psm30StateEtwIssuingDisabledToDSMFromDisabled,
    // State Entry Function
    HUBPSM30_IssuingDisabledToDSMFromDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateSuspendedDisabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingHubReset = {
    // State ETW Name
    Psm30StateEtwIssuingHubReset,
    // State Entry Function
    HUBPSM30_IssuingHubReset,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm30SubSmIssuingHubReset,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventHubSuspend ,          Psm30StatePoweredOffSuspended },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingHubResetFromDisconnected = {
    // State ETW Name
    Psm30StateEtwIssuingHubResetFromDisconnected,
    // State Entry Function
    HUBPSM30_IssuingHubResetFromDisconnected,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm30SubSmIssuingHubResetFromDisconnected,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventHubSuspend ,          Psm30StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingHubResetOnDisableFailure = {
    // State ETW Name
    Psm30StateEtwIssuingHubResetOnDisableFailure,
    // State Entry Function
    HUBPSM30_IssuingHubResetOnDisableFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingHubResetOnTimedOut = {
    // State ETW Name
    Psm30StateEtwIssuingHubResetOnTimedOut,
    // State Entry Function
    HUBPSM30_IssuingHubResetOnTimedOut,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateWaitingForHubStopSuspend },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingHubResetWithPendingTimer = {
    // State ETW Name
    Psm30StateEtwIssuingHubResetWithPendingTimer,
    // State Entry Function
    HUBPSM30_IssuingHubResetWithPendingTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateWaitingForTimerOnResetInDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingHubResetWithTimer = {
    // State ETW Name
    Psm30StateEtwIssuingHubResetWithTimer,
    // State Entry Function
    HUBPSM30_IssuingHubResetWithTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReleasingInterruptReferenceAndStoppingTimer },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingPortDisabledToDevice = {
    // State ETW Name
    Psm30StateEtwIssuingPortDisabledToDevice,
    // State Entry Function
    HUBPSM30_IssuingPortDisabledToDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateSuspendedDisabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingPortDisableFailedToDevice = {
    // State ETW Name
    Psm30StateEtwIssuingPortDisableFailedToDevice,
    // State Entry Function
    HUBPSM30_IssuingPortDisableFailedToDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateConnectedError },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingPortFailedToDSMOnLinkStateError = {
    // State ETW Name
    Psm30StateEtwIssuingPortFailedToDSMOnLinkStateError,
    // State Entry Function
    HUBPSM30_IssuingPortFailedToDSMOnLinkStateError,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReleasingInterruptReferenceAfterResetFailure },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingPortResetTimedOutToDSM = {
    // State ETW Name
    Psm30StateEtwIssuingPortResetTimedOutToDSM,
    // State Entry Function
    HUBPSM30_IssuingPortResetTimedOutToDSM,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReleasingInterruptReferenceAfterResetFailure },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingReAttachDeviceToBootDevice = {
    // State ETW Name
    Psm30StateEtwIssuingReAttachDeviceToBootDevice,
    // State Entry Function
    HUBPSM30_IssuingReAttachDeviceToBootDevice,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateConnectedEnabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingReAttachDeviceToBootDeviceWithResetInProgress = {
    // State ETW Name
    Psm30StateEtwIssuingReAttachDeviceToBootDeviceWithResetInProgress,
    // State Entry Function
    HUBPSM30_IssuingReAttachDeviceToBootDeviceWithResetInProgress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateWaitingForResetRequestFromDSMWithResetInPorgress },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled = {
    // State ETW Name
    Psm30StateEtwIssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled,
    // State Entry Function
    HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateConnectedError },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled = {
    // State ETW Name
    Psm30StateEtwIssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled,
    // State Entry Function
    HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateConnectedEnabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingResetFailedDuetoPendingHubSuspendToDSMWhileResetInProgress = {
    // State ETW Name
    Psm30StateEtwIssuingResetFailedDuetoPendingHubSuspendToDSMWhileResetInProgress,
    // State Entry Function
    HUBPSM30_IssuingResetFailedDuetoPendingHubSuspendToDSMWhileResetInProgress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateWaitingForResetRequestFromDSMWithResetInPorgress },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingResumedToDeviceSM = {
    // State ETW Name
    Psm30StateEtwIssuingResumedToDeviceSM,
    // State Entry Function
    HUBPSM30_IssuingResumedToDeviceSM,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateConnectedEnabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingSuspendedToDeviceSM = {
    // State ETW Name
    Psm30StateEtwIssuingSuspendedToDeviceSM,
    // State Entry Function
    HUBPSM30_IssuingSuspendedToDeviceSM,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateSuspended },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryNotifyingUserAboutPersistentOverCurrent = {
    // State ETW Name
    Psm30StateEtwNotifyingUserAboutPersistentOverCurrent,
    // State Entry Function
    HUBPSM30_NotifyingUserAboutPersistentOverCurrent,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateWaitingForUserResetOnOverCurrent },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryPoweredOffConnectedError = {
    // State ETW Name
    Psm30StateEtwPoweredOffConnectedError,
    // State Entry Function
    HUBPSM30_PoweredOffConnectedError,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceSetU1Timeout ,  Psm30StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventHubStatusChange ,     Psm30StateIgnored },
        { PsmEventDeviceRequestResume , Psm30StateIgnored },
        { PsmEventDeviceRequestDisable ,Psm30StateIgnored },
        { PsmEventDeviceRequestSuspend ,Psm30StateIgnored },
        { PsmEventDeviceRequestWarmReset ,Psm30StateIgnored },
        { PsmEventDevicePrepareForHibernation ,Psm30StateIgnored },
        { PsmEventHubStop ,             Psm30StateIgnored },
        { PsmEventDeviceRequestReset ,  Psm30StateIgnored },
        { PsmEventDeviceSetU2Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceRequestDisableSuperSpeed ,Psm30StateIgnored },
        { PsmEventHubReset ,            Psm30StateReleasingResetReferenceOnDeviceRemoval },
        { PsmEventHubResume ,           Psm30StateQueueStateDisabledToDSMOnStartFromConnectedError },
        { PsmEventDeviceRequestCycle ,  Psm30StateIssuingDetachDeviceOnCycleFromPoweredOffDisabled },
        { PsmEventObjectCleanup ,       Psm30StateDetachingDeviceOnCleanup },
        { PsmEventHubResumeWithReset ,  Psm30StateQueueStateDisabledToDSMOnStartFromConnectedError },
        { PsmEventHubResumeInS0 ,       Psm30StateAcquiringPowerReferenceonPoweredOffConnectedErrorInS0 },
        { PsmEventHubStart ,            Psm30StateGettingPortChangeOnStartResumeFromConnectedError },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryPoweredOffConnectedWithResetNeededAndDeviceDetached = {
    // State ETW Name
    Psm30StateEtwPoweredOffConnectedWithResetNeededAndDeviceDetached,
    // State Entry Function
    HUBPSM30_PoweredOffConnectedWithResetNeededAndDeviceDetached,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceRequestSuspend ,Psm30StateIgnored },
        { PsmEventDeviceRequestResume , Psm30StateIgnored },
        { PsmEventDevicePrepareForHibernation ,Psm30StateIgnored },
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventDeviceRequestWarmReset ,Psm30StateIgnored },
        { PsmEventDeviceRequestDisable ,Psm30StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventDeviceRequestReset ,  Psm30StateIgnored },
        { PsmEventHubStop ,             Psm30StateIgnored },
        { PsmEventDeviceSetU2Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceRequestCycle ,  Psm30StateIgnored },
        { PsmEventDeviceSetU1Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceRequestDisableSuperSpeed ,Psm30StateIgnored },
        { PsmEventHubReset ,            Psm30StateReleasingResetReferenceInDisconnected },
        { PsmEventHubResume ,           Psm30StateAcquiringReferenceInConnectedWithResetNeeded },
        { PsmEventHubResumeWithReset ,  Psm30StateAcquiringReferenceInConnectedWithResetNeeded },
        { PsmEventHubStart ,            Psm30StateAcquiringReferenceInConnectedWithResetNeeded },
        { PsmEventHubResumeInS0 ,       Psm30StateAcquiringReferenceInConnectedWithResetNeeded },
        { PsmEventObjectCleanup ,       Psm30StateWaitingToBeDeleted },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryPoweredOffDisconnected = {
    // State ETW Name
    Psm30StateEtwPoweredOffDisconnected,
    // State Entry Function
    HUBPSM30_PoweredOffDisconnected,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceRequestWarmReset ,Psm30StateIgnored },
        { PsmEventDeviceRequestReset ,  Psm30StateIgnored },
        { PsmEventDeviceRequestCycle ,  Psm30StateIgnored },
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventDeviceSetU1Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceRequestResume , Psm30StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventDeviceRequestSuspend ,Psm30StateIgnored },
        { PsmEventDeviceSetU2Timeout ,  Psm30StateIgnored },
        { PsmEventDevicePrepareForHibernation ,Psm30StateIgnored },
        { PsmEventHubStop ,             Psm30StateIgnored },
        { PsmEventDeviceRequestDisable ,Psm30StateIgnored },
        { PsmEventDeviceRequestDisableSuperSpeed ,Psm30StateIgnored },
        { PsmEventHubResume ,           Psm30StateStartingAndAcquiringReferenceOnHubStart },
        { PsmEventHubResumeWithReset ,  Psm30StateStartingAndAcquiringReferenceOnHubStart },
        { PsmEventObjectCleanup ,       Psm30StateWaitingToBeDeleted },
        { PsmEventHubResumeInS0 ,       Psm30StateAcquiringPowerReferenceonPoweredOffDisconnectedInS0 },
        { PsmEventHubReset ,            Psm30StateReleasingResetReferenceInDisconnected },
        { PsmEventHubStart ,            Psm30StateCheckingIfSuperSpeedShouldBeDisabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryPoweredOffSuspended = {
    // State ETW Name
    Psm30StateEtwPoweredOffSuspended,
    // State Entry Function
    HUBPSM30_PoweredOffSuspended,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStatusChange ,     Psm30StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventHubResumeInS0 ,       Psm30StateAcquiringPowerReferenceonPoweredOffSuspendedInS0 },
        { PsmEventHubStop ,             Psm30StatePoweredOffConnectedError },
        { PsmEventHubResume ,           Psm30StateStartingAndAcquiringReferenceOnSystemResume },
        { PsmEventHubResumeWithReset ,  Psm30StateStartingAndAcquiringReferenceOnSystemResume },
        { PsmEventDeviceRequestCycle ,  Psm30StateIssuingDetachDeviceOnCycleFromPoweredOffDisabled },
        { PsmEventObjectCleanup ,       Psm30StateDetachingDeviceOnCleanup },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueryingPortStatusOnOverCurrent = {
    // State ETW Name
    Psm30StateEtwQueryingPortStatusOnOverCurrent,
    // State Entry Function
    HUBPSM30_QueryingPortStatusOnOverCurrent,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventTransferSuccess ,     Psm30StateCheckingIfOvercurrentBitOne },
        { PsmEventTransferFailure ,     Psm30StateIssuingHubResetFromDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueingPortFailureToDSMOnTransferFailure = {
    // State ETW Name
    Psm30StateEtwQueueingPortFailureToDSMOnTransferFailure,
    // State Entry Function
    HUBPSM30_QueueingPortFailureToDSMOnTransferFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateIssuingHubReset },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueingPortFailureToDSMOnTransferFailureForLpmTimeout = {
    // State ETW Name
    Psm30StateEtwQueueingPortFailureToDSMOnTransferFailureForLpmTimeout,
    // State Entry Function
    HUBPSM30_QueueingPortFailureToDSMOnTransferFailureForLpmTimeout,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateConnectedEnabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueingPortResumeFailedDueToPendingSuspend = {
    // State ETW Name
    Psm30StateEtwQueueingPortResumeFailedDueToPendingSuspend,
    // State Entry Function
    HUBPSM30_QueueingPortResumeFailedDueToPendingSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateSuspended },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueingPortResumeTimedOut = {
    // State ETW Name
    Psm30StateEtwQueueingPortResumeTimedOut,
    // State Entry Function
    HUBPSM30_QueueingPortResumeTimedOut,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateConnectedError },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueingPortTimeoutUpdatedToDSM = {
    // State ETW Name
    Psm30StateEtwQueueingPortTimeoutUpdatedToDSM,
    // State Entry Function
    HUBPSM30_QueueingPortTimeoutUpdatedToDSM,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateConnectedEnabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueStateDisabledToDSMOnStartFromConnectedError = {
    // State ETW Name
    Psm30StateEtwQueueStateDisabledToDSMOnStartFromConnectedError,
    // State Entry Function
    HUBPSM30_QueueStateDisabledToDSMOnStartFromConnectedError,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateGettingPortChangeOnStartResumeFromConnectedError },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReleasingInterruptReferenceAfterResetFailure = {
    // State ETW Name
    Psm30StateEtwReleasingInterruptReferenceAfterResetFailure,
    // State Entry Function
    HUBPSM30_ReleasingInterruptReferenceAfterResetFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateConnectedError },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReleasingInterruptReferenceAndStoppingTimer = {
    // State ETW Name
    Psm30StateEtwReleasingInterruptReferenceAndStoppingTimer,
    // State Entry Function
    HUBPSM30_ReleasingInterruptReferenceAndStoppingTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForHubStopSuspendOrTimer },
        { PsmEventOperationSuccess ,    Psm30StateWaitingForHubStopSuspend },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReleasingInterruptReferenceOnResetTransferFailure = {
    // State ETW Name
    Psm30StateEtwReleasingInterruptReferenceOnResetTransferFailure,
    // State Entry Function
    HUBPSM30_ReleasingInterruptReferenceOnResetTransferFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateIssuingHubReset },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReleasingInterruptReferenceOnResumed = {
    // State ETW Name
    Psm30StateEtwReleasingInterruptReferenceOnResumed,
    // State Entry Function
    HUBPSM30_ReleasingInterruptReferenceOnResumed,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateIssuingResumedToDeviceSM },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReleasingInterruptReferenceOnSuccessfulReset = {
    // State ETW Name
    Psm30StateEtwReleasingInterruptReferenceOnSuccessfulReset,
    // State Entry Function
    HUBPSM30_ReleasingInterruptReferenceOnSuccessfulReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateConnectedEnabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReleasingInterruptReferenceOnTransferFailureInResuming = {
    // State ETW Name
    Psm30StateEtwReleasingInterruptReferenceOnTransferFailureInResuming,
    // State Entry Function
    HUBPSM30_ReleasingInterruptReferenceOnTransferFailureInResuming,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateQueueingPortFailureToDSMOnTransferFailure },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReleasingPortPowerReferenceInSuperSpeedDisabled = {
    // State ETW Name
    Psm30StateEtwReleasingPortPowerReferenceInSuperSpeedDisabled,
    // State Entry Function
    HUBPSM30_ReleasingPortPowerReferenceInSuperSpeedDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateSuperSpeedDisabledStopped },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded = {
    // State ETW Name
    Psm30StateEtwReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded,
    // State Entry Function
    HUBPSM30_ReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StatePoweredOffConnectedWithResetNeededAndDeviceDetached },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReleasingPowerReferenceonPoweredOffDisabled = {
    // State ETW Name
    Psm30StateEtwReleasingPowerReferenceonPoweredOffDisabled,
    // State Entry Function
    HUBPSM30_ReleasingPowerReferenceonPoweredOffDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StatePoweredOffConnectedError },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReleasingPowerReferenceonPoweredOffDisconnected = {
    // State ETW Name
    Psm30StateEtwReleasingPowerReferenceonPoweredOffDisconnected,
    // State Entry Function
    HUBPSM30_ReleasingPowerReferenceonPoweredOffDisconnected,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StatePoweredOffDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReleasingResetReferenceInDisconnected = {
    // State ETW Name
    Psm30StateEtwReleasingResetReferenceInDisconnected,
    // State Entry Function
    HUBPSM30_ReleasingResetReferenceInDisconnected,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StatePoweredOffDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReleasingResetReferenceInSuperSpeedDisabled = {
    // State ETW Name
    Psm30StateEtwReleasingResetReferenceInSuperSpeedDisabled,
    // State Entry Function
    HUBPSM30_ReleasingResetReferenceInSuperSpeedDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateSuperSpeedDisabledStopped },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReleasingResetReferenceOnDeviceRemoval = {
    // State ETW Name
    Psm30StateEtwReleasingResetReferenceOnDeviceRemoval,
    // State Entry Function
    HUBPSM30_ReleasingResetReferenceOnDeviceRemoval,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StatePoweredOffDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryResettingHubAndFlushingUserInitiatedResetOnOverCurrent = {
    // State ETW Name
    Psm30StateEtwResettingHubAndFlushingUserInitiatedResetOnOverCurrent,
    // State Entry Function
    HUBPSM30_ResettingHubAndFlushingUserInitiatedResetOnOverCurrent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventUserInitiatedResetOnOverCurrent ,Psm30StateIgnored },
        { PsmEventOperationSuccess ,    Psm30StateWaitingForHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntrySettingPortPowerOnOverCurrent = {
    // State ETW Name
    Psm30StateEtwSettingPortPowerOnOverCurrent,
    // State Entry Function
    HUBPSM30_SettingPortPowerOnOverCurrent,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventHubStop ,             Psm30StateCancellingSetPortPower },
        { PsmEventHubSuspend ,          Psm30StateCancellingSetPortPower },
        { PsmEventTransferSuccess ,     Psm30StateQueryingPortStatusOnOverCurrent },
        { PsmEventTransferFailure ,     Psm30StateIssuingHubResetFromDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntrySettingRemotWakeMask = {
    // State ETW Name
    Psm30StateEtwSettingRemotWakeMask,
    // State Entry Function
    HUBPSM30_SettingRemotWakeMask,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    Psm30SubSmSettingRemotWakeMask,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StatePoweredOffSuspended },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntrySettingRemotWakeMaskInDisconected = {
    // State ETW Name
    Psm30StateEtwSettingRemotWakeMaskInDisconected,
    // State Entry Function
    HUBPSM30_SettingRemotWakeMaskInDisconected,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    Psm30SubSmSettingRemotWakeMaskInDisconected,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntrySettingU1TImeOut = {
    // State ETW Name
    Psm30StateEtwSettingU1TImeOut,
    // State Entry Function
    HUBPSM30_SettingU1TImeOut,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm30StateQueueingPortFailureToDSMOnTransferFailureForLpmTimeout },
        { PsmEventTransferSuccess ,     Psm30StateQueueingPortTimeoutUpdatedToDSM },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntrySettingU2TImeOut = {
    // State ETW Name
    Psm30StateEtwSettingU2TImeOut,
    // State Entry Function
    HUBPSM30_SettingU2TImeOut,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm30StateQueueingPortFailureToDSMOnTransferFailureForLpmTimeout },
        { PsmEventTransferSuccess ,     Psm30StateQueueingPortTimeoutUpdatedToDSM },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStartingAndAcquiringReferenceOnHubStart = {
    // State ETW Name
    Psm30StateEtwStartingAndAcquiringReferenceOnHubStart,
    // State Entry Function
    HUBPSM30_StartingAndAcquiringReferenceOnHubStart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmStartingAndAcquiringReferenceOnHubStart,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortNoChange ,        Psm30StateDisconnected },
        { PsmEventPortResetInProgress , Psm30StateIsOldDevicePresentWithResetInProgress },
        { PsmEventPortOverCurrentCleared ,Psm30StateSettingPortPowerOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateCheckingIfADeviceIsPresent },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubResetFromDisconnected },
        { PsmEventPortOverCurrent ,     Psm30StateStartingOverCurrentTimer },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventHubSuspend ,          Psm30StateSettingRemotWakeMaskInDisconected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStartingAndAcquiringReferenceOnSystemResume = {
    // State ETW Name
    Psm30StateEtwStartingAndAcquiringReferenceOnSystemResume,
    // State Entry Function
    HUBPSM30_StartingAndAcquiringReferenceOnSystemResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmStartingAndAcquiringReferenceOnSystemResume,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortResumed ,         Psm30StateConnectedEnabled },
        { PsmEventOperationSuccess ,    Psm30StateSuspended },
        { PsmEventPortResetInProgress , Psm30StateWaitingForResetRequestFromDSMWithResetInPorgress },
        { PsmEventPortOverCurrentCleared ,Psm30StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubReset },
        { PsmEventPortOverCurrent ,     Psm30StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventPortNeedsReset ,      Psm30StateConnectedError },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStartingDisableSuperSpeedTimer = {
    // State ETW Name
    Psm30StateEtwStartingDisableSuperSpeedTimer,
    // State Entry Function
    HUBPSM30_StartingDisableSuperSpeedTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateDisconnectedSuperSpeedDisabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStartingOverCurrentTimer = {
    // State ETW Name
    Psm30StateEtwStartingOverCurrentTimer,
    // State Entry Function
    HUBPSM30_StartingOverCurrentTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateWaitingForOverCurrentClear },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStartingResumeTimer = {
    // State ETW Name
    Psm30StateEtwStartingResumeTimer,
    // State Entry Function
    HUBPSM30_StartingResumeTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateWaitingForResumeComplete },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStoppingDisableSuperSpeedTimer = {
    // State ETW Name
    Psm30StateEtwStoppingDisableSuperSpeedTimer,
    // State Entry Function
    HUBPSM30_StoppingDisableSuperSpeedTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForSuperSpeedDisableTimerToFlush },
        { PsmEventOperationSuccess ,    Psm30StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStoppingResumeTimer = {
    // State ETW Name
    Psm30StateEtwStoppingResumeTimer,
    // State Entry Function
    HUBPSM30_StoppingResumeTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForResumeTimerToFlush },
        { PsmEventOperationSuccess ,    Psm30StateReleasingInterruptReferenceOnResumed },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStoppingTimerAndQueueingHubResetInDisconnected = {
    // State ETW Name
    Psm30StateEtwStoppingTimerAndQueueingHubResetInDisconnected,
    // State Entry Function
    HUBPSM30_StoppingTimerAndQueueingHubResetInDisconnected,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForTimerOnResetInDisconnected },
        { PsmEventOperationSuccess ,    Psm30StateWaitingForHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStoppingTimerOnDisconnect = {
    // State ETW Name
    Psm30StateEtwStoppingTimerOnDisconnect,
    // State Entry Function
    HUBPSM30_StoppingTimerOnDisconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForTimerToFlushOnDisconnect },
        { PsmEventOperationSuccess ,    Psm30StateCheckingIfADeviceIsPresent },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStoppingTimerOnOverCurrent = {
    // State ETW Name
    Psm30StateEtwStoppingTimerOnOverCurrent,
    // State Entry Function
    HUBPSM30_StoppingTimerOnOverCurrent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForTimerToFlushOnOverCurrent },
        { PsmEventOperationSuccess ,    Psm30StateStartingOverCurrentTimer },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStoppingTimerOnOverCurrentClear = {
    // State ETW Name
    Psm30StateEtwStoppingTimerOnOverCurrentClear,
    // State Entry Function
    HUBPSM30_StoppingTimerOnOverCurrentClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForTimerToFlushOnOverCurrentClear },
        { PsmEventOperationSuccess ,    Psm30StateSettingPortPowerOnOverCurrent },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStoppingTimerOnResetComplete = {
    // State ETW Name
    Psm30StateEtwStoppingTimerOnResetComplete,
    // State Entry Function
    HUBPSM30_StoppingTimerOnResetComplete,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForResetTimerToFlush },
        { PsmEventOperationSuccess ,    Psm30StateReleasingInterruptReferenceOnSuccessfulReset },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntrySuperSpeedDisabled = {
    // State ETW Name
    Psm30StateEtwSuperSpeedDisabled,
    // State Entry Function
    HUBPSM30_SuperSpeedDisabled,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    Psm30SubSmSuperSpeedDisabled,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventHubSuspend ,          Psm30StateReleasingPortPowerReferenceInSuperSpeedDisabled },
        { PsmEventHubStop ,             Psm30StateReleasingPortPowerReferenceInSuperSpeedDisabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntrySuperSpeedDisabledStopped = {
    // State ETW Name
    Psm30StateEtwSuperSpeedDisabledStopped,
    // State Entry Function
    HUBPSM30_SuperSpeedDisabledStopped,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventHubStop ,             Psm30StateIgnored },
        { PsmEventHubResumeInS0 ,       Psm30StateAcquiringPowerReferenceAfterDisablingSuperSpeed },
        { PsmEventHubStart ,            Psm30StateDisablingSuperSpeedOnPortForUnsupportedHubs },
        { PsmEventHubReset ,            Psm30StateReleasingResetReferenceInSuperSpeedDisabled },
        { PsmEventHubResumeWithReset ,  Psm30StateDisablingSuperSpeedOnPortForUnsupportedHubs },
        { PsmEventHubResume ,           Psm30StateAcquiringPowerReferenceAfterDisablingSuperSpeed },
        { PsmEventObjectCleanup ,       Psm30StateWaitingToBeDeleted },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntrySuspended = {
    // State ETW Name
    Psm30StateEtwSuspended,
    // State Entry Function
    HUBPSM30_Suspended,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    Psm30SubSmSuspended,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubReset },
        { PsmEventPortOverCurrentCleared ,Psm30StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm30StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateCheckingIsD3ColdIsEnabled },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventHubSuspend ,          Psm30StateSettingRemotWakeMask },
        { PsmEventDeviceRequestWarmReset ,Psm30StateAcquiringInterruptReferenceOnHSMInDisabled },
        { PsmEventDeviceRequestReset ,  Psm30StateAcquiringInterruptReferenceOnHSMInDisabled },
        { PsmEventDeviceRequestCycle ,  Psm30StateDetachingDeviceFromPortOnPortCycle },
        { PsmEventDeviceRequestDisable ,Psm30StateIssuingDisabledToDSMFromDisabled },
        { PsmEventDeviceRequestResume , Psm30StateAcquiringInterruptReferenceOnResuming },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntrySuspendedDisabled = {
    // State ETW Name
    Psm30StateEtwSuspendedDisabled,
    // State Entry Function
    HUBPSM30_SuspendedDisabled,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    Psm30SubSmSuspendedDisabled,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm30StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubReset },
        { PsmEventPortOverCurrent ,     Psm30StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventDeviceRequestDisable ,Psm30StateIssuingDisabledToDSMFromDisabled },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventHubSuspend ,          Psm30StateSettingRemotWakeMask },
        { PsmEventDeviceRequestWarmReset ,Psm30StateAcquiringInterruptReferenceOnHSMInDisabled },
        { PsmEventDeviceRequestReset ,  Psm30StateAcquiringInterruptReferenceOnHSMInDisabled },
        { PsmEventDeviceRequestCycle ,  Psm30StateDetachingDeviceFromPortOnPortCycle },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntrySuspendedInD3Cold = {
    // State ETW Name
    Psm30StateEtwSuspendedInD3Cold,
    // State Entry Function
    HUBPSM30_SuspendedInD3Cold,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagAllowsHubSuspend,
    // Sub State Machine Information
    Psm30SubSmSuspendedInD3Cold,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortResumed ,         Psm30StateConnectedEnabled },
        { PsmEventPortOverCurrentCleared ,Psm30StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubReset },
        { PsmEventPortOverCurrent ,     Psm30StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventDeviceRequestDisable ,Psm30StateIssuingDisabledToDSMFromDisabled },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventHubSuspend ,          Psm30StateSettingRemotWakeMask },
        { PsmEventDeviceRequestWarmReset ,Psm30StateAcquiringInterruptReferenceOnHSMInDisabled },
        { PsmEventDeviceRequestReset ,  Psm30StateAcquiringInterruptReferenceOnHSMInDisabled },
        { PsmEventDeviceRequestCycle ,  Psm30StateDetachingDeviceFromPortOnPortCycle },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntrySuspending = {
    // State ETW Name
    Psm30StateEtwSuspending,
    // State Entry Function
    HUBPSM30_Suspending,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventTransferSuccess ,     Psm30StateIssuingSuspendedToDeviceSM },
        { PsmEventTransferFailure ,     Psm30StateQueueingPortFailureToDSMOnTransferFailure },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntrySuspendingPortOnDisableRequest = {
    // State ETW Name
    Psm30StateEtwSuspendingPortOnDisableRequest,
    // State Entry Function
    HUBPSM30_SuspendingPortOnDisableRequest,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventTransferFailure ,     Psm30StateIssuingPortDisableFailedToDevice },
        { PsmEventTransferSuccess ,     Psm30StateIssuingPortDisabledToDevice },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntrySuspendingPortOnHubSuspend = {
    // State ETW Name
    Psm30StateEtwSuspendingPortOnHubSuspend,
    // State Entry Function
    HUBPSM30_SuspendingPortOnHubSuspend,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventTransferSuccess ,     Psm30StateReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded },
        { PsmEventTransferFailure ,     Psm30StateIssuingHubResetOnDisableFailure },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForDebounceTimer = {
    // State ETW Name
    Psm30StateEtwWaitingForDebounceTimer,
    // State Entry Function
    HUBPSM30_WaitingForDebounceTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForDebounceTimer,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm30StateStoppingTimerOnOverCurrentClear },
        { PsmEventPortResetHubOnError , Psm30StateStoppingTimerAndQueueingHubResetInDisconnected },
        { PsmEventPortOverCurrent ,     Psm30StateStoppingTimerOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateStoppingTimerOnDisconnect },
        { PsmEventTimerFired ,          Psm30StateDoesDeviceSupportsReAttach },
        { PsmEventHubStop ,             Psm30StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventHubSuspend ,          Psm30StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForHubStopSuspend = {
    // State ETW Name
    Psm30StateEtwWaitingForHubStopSuspend,
    // State Entry Function
    HUBPSM30_WaitingForHubStopSuspend,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm30SubSmWaitingForHubStopSuspend,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventHubSuspend ,          Psm30StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForHubStopSuspendInDisconnected = {
    // State ETW Name
    Psm30StateEtwWaitingForHubStopSuspendInDisconnected,
    // State Entry Function
    HUBPSM30_WaitingForHubStopSuspendInDisconnected,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm30SubSmWaitingForHubStopSuspendInDisconnected,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventHubSuspend ,          Psm30StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForHubStopSuspendOnAttachFailure = {
    // State ETW Name
    Psm30StateEtwWaitingForHubStopSuspendOnAttachFailure,
    // State Entry Function
    HUBPSM30_WaitingForHubStopSuspendOnAttachFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForHubStopSuspendOnAttachFailure,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded },
        { PsmEventHubSuspend ,          Psm30StateSuspendingPortOnHubSuspend },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForHubStopSuspendOnAttachWithResetFailure = {
    // State ETW Name
    Psm30StateEtwWaitingForHubStopSuspendOnAttachWithResetFailure,
    // State Entry Function
    HUBPSM30_WaitingForHubStopSuspendOnAttachWithResetFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForHubStopSuspendOnAttachWithResetFailure,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventHubSuspend ,          Psm30StateReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForHubStopSuspendOrTimer = {
    // State ETW Name
    Psm30StateEtwWaitingForHubStopSuspendOrTimer,
    // State Entry Function
    HUBPSM30_WaitingForHubStopSuspendOrTimer,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm30SubSmWaitingForHubStopSuspendOrTimer,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventTimerFired ,          Psm30StateWaitingForHubStopSuspend },
        { PsmEventHubStop ,             Psm30StateWaitingForTimerToFlushOnHubStopSuspend },
        { PsmEventHubSuspend ,          Psm30StateWaitingForTimerToFlushOnHubStopSuspend },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForOldDeviceToDetach = {
    // State ETW Name
    Psm30StateEtwWaitingForOldDeviceToDetach,
    // State Entry Function
    HUBPSM30_WaitingForOldDeviceToDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForOldDeviceToDetach,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubResetFromDisconnected },
        { PsmEventPortOverCurrentCleared ,Psm30StateSettingPortPowerOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateCheckingIfADeviceIsPresent },
        { PsmEventPortOverCurrent ,     Psm30StateStartingOverCurrentTimer },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded },
        { PsmEventDeviceDetached ,      Psm30StateFlushingStaleDeviceEvents },
        { PsmEventHubSuspend ,          Psm30StateSuspendingPortOnHubSuspend },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForOldDeviceToDetachInConnectedWithResetNeeded = {
    // State ETW Name
    Psm30StateEtwWaitingForOldDeviceToDetachInConnectedWithResetNeeded,
    // State Entry Function
    HUBPSM30_WaitingForOldDeviceToDetachInConnectedWithResetNeeded,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForOldDeviceToDetachInConnectedWithResetNeeded,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortConnectChange ,   Psm30StateCheckingIfADeviceIsPresent },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubResetFromDisconnected },
        { PsmEventPortOverCurrentCleared ,Psm30StateSettingPortPowerOnOverCurrent },
        { PsmEventPortOverCurrent ,     Psm30StateStartingOverCurrentTimer },
        { PsmEventHubSuspend ,          Psm30StateReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded },
        { PsmEventDeviceDetached ,      Psm30StateFlushingStaleDeviceEventsWithResetNeeded },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForOldDeviceToDetachWithResetInProgress = {
    // State ETW Name
    Psm30StateEtwWaitingForOldDeviceToDetachWithResetInProgress,
    // State Entry Function
    HUBPSM30_WaitingForOldDeviceToDetachWithResetInProgress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForOldDeviceToDetachWithResetInProgress,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortConnectChange ,   Psm30StateCheckingIfADeviceIsPresent },
        { PsmEventPortOverCurrentCleared ,Psm30StateSettingPortPowerOnOverCurrent },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubResetFromDisconnected },
        { PsmEventPortOverCurrent ,     Psm30StateStartingOverCurrentTimer },
        { PsmEventDeviceDetached ,      Psm30StateFlushingStaleDeviceEventsWithResetInProgress },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded },
        { PsmEventHubSuspend ,          Psm30StateSuspendingPortOnHubSuspend },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForOverCurrentClear = {
    // State ETW Name
    Psm30StateEtwWaitingForOverCurrentClear,
    // State Entry Function
    HUBPSM30_WaitingForOverCurrentClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForOverCurrentClear,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventPortResetHubOnError , Psm30StateStoppingTimerAndQueueingHubResetInDisconnected },
        { PsmEventPortConnectChange ,   Psm30StateWaitingForOverCurrentClear },
        { PsmEventPortOverCurrentCleared ,Psm30StateStoppingTimerOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm30StateStoppingTimerOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateWaitingForOverCurrentClear },
        { PsmEventTimerFired ,          Psm30StateCheckingIfPersistentOvercurrent },
        { PsmEventHubStop ,             Psm30StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventHubSuspend ,          Psm30StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForResetComplete = {
    // State ETW Name
    Psm30StateEtwWaitingForResetComplete,
    // State Entry Function
    HUBPSM30_WaitingForResetComplete,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForResetComplete,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResetComplete ,   Psm30StateStoppingTimerOnResetComplete },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubResetWithTimer },
        { PsmEventPortOverCurrentCleared ,Psm30StateDetachingDeviceFromPortOnOverCurrentClearWithTimer },
        { PsmEventPortOverCurrent ,     Psm30StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortConnectChange ,   Psm30StateDetachingDeviceFromPortOnDisconnectWithTimer },
        { PsmEventTimerFired ,          Psm30StateGettingPortStatusOnResetTimeout },
        { PsmEventHubSurpriseRemove ,   Psm30StateReleasingInterruptReferenceAndStoppingTimer },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForResetRequestFromDSMWithResetInPorgress = {
    // State ETW Name
    Psm30StateEtwWaitingForResetRequestFromDSMWithResetInPorgress,
    // State Entry Function
    HUBPSM30_WaitingForResetRequestFromDSMWithResetInPorgress,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm30SubSmWaitingForResetRequestFromDSMWithResetInPorgress,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventDeviceSetU1Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceSetU2Timeout ,  Psm30StateIgnored },
        { PsmEventPortResetComplete ,   Psm30StateConnectedEnabled },
        { PsmEventPortOverCurrentCleared ,Psm30StateDetachingDeviceFromPortOnOverCurrentClear },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubReset },
        { PsmEventPortOverCurrent ,     Psm30StateDetachingDeviceFromPortOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateDetachingDeviceFromPortOnDisconnect },
        { PsmEventDeviceRequestCycle ,  Psm30StateDetachingDeviceFromPortOnPortCycle },
        { PsmEventDeviceRequestReset ,  Psm30StateAcquiringInterruptReferenceOnHSMOnResetRequest },
        { PsmEventHubStop ,             Psm30StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventDeviceRequestWarmReset ,Psm30StateAcquiringInterruptReferenceOnHSMOnResetRequest },
        { PsmEventDeviceRequestDisable ,Psm30StateSuspendingPortOnDisableRequest },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForResetTimerToFlush = {
    // State ETW Name
    Psm30StateEtwWaitingForResetTimerToFlush,
    // State Entry Function
    HUBPSM30_WaitingForResetTimerToFlush,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForResetTimerToFlush,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubResetWithTimer },
        { PsmEventPortOverCurrentCleared ,Psm30StateDetachingDeviceFromPortOnOverCurrentClearWithTimer },
        { PsmEventPortOverCurrent ,     Psm30StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortConnectChange ,   Psm30StateDetachingDeviceFromPortOnDisconnectWithTimer },
        { PsmEventTimerFired ,          Psm30StateReleasingInterruptReferenceOnSuccessfulReset },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForResumeComplete = {
    // State ETW Name
    Psm30StateEtwWaitingForResumeComplete,
    // State Entry Function
    HUBPSM30_WaitingForResumeComplete,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForResumeComplete,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResumed ,         Psm30StateStoppingResumeTimer },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubResetWithTimer },
        { PsmEventPortOverCurrentCleared ,Psm30StateDetachingDeviceFromPortOnOverCurrentClearWithTimer },
        { PsmEventPortOverCurrent ,     Psm30StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortConnectChange ,   Psm30StateDetachingDeviceFromPortOnDisconnectWithTimer },
        { PsmEventTimerFired ,          Psm30StateGettingPortStatusOnResumeTimeout },
        { PsmEventHubSurpriseRemove ,   Psm30StateReleasingInterruptReferenceAndStoppingTimer },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForResumeTimerToFlush = {
    // State ETW Name
    Psm30StateEtwWaitingForResumeTimerToFlush,
    // State Entry Function
    HUBPSM30_WaitingForResumeTimerToFlush,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForResumeTimerToFlush,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubResetWithTimer },
        { PsmEventPortOverCurrentCleared ,Psm30StateDetachingDeviceFromPortOnOverCurrentClearWithTimer },
        { PsmEventPortOverCurrent ,     Psm30StateDetachingDeviceFromPortOnOverCurrentWithTimer },
        { PsmEventPortConnectChange ,   Psm30StateDetachingDeviceFromPortOnDisconnectWithTimer },
        { PsmEventTimerFired ,          Psm30StateReleasingInterruptReferenceOnResumed },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForSuperSpeedDisableTimerToFlush = {
    // State ETW Name
    Psm30StateEtwWaitingForSuperSpeedDisableTimerToFlush,
    // State Entry Function
    HUBPSM30_WaitingForSuperSpeedDisableTimerToFlush,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventTimerFired ,          Psm30StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForTimerOnResetInDisconnected = {
    // State ETW Name
    Psm30StateEtwWaitingForTimerOnResetInDisconnected,
    // State Entry Function
    HUBPSM30_WaitingForTimerOnResetInDisconnected,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    Psm30SubSmWaitingForTimerOnResetInDisconnected,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventHubSuspend ,          Psm30StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventTimerFired ,          Psm30StateWaitingForHubStopSuspendInDisconnected },
        { PsmEventHubStop ,             Psm30StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForTimerToFlushOnDisconnect = {
    // State ETW Name
    Psm30StateEtwWaitingForTimerToFlushOnDisconnect,
    // State Entry Function
    HUBPSM30_WaitingForTimerToFlushOnDisconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForTimerToFlushOnDisconnect,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubResetWithPendingTimer },
        { PsmEventPortOverCurrent ,     Psm30StateWaitingForTimerToFlushOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateWaitingForTimerToFlushOnDisconnect },
        { PsmEventPortConnectChange ,   Psm30StateWaitingForTimerToFlushOnDisconnect },
        { PsmEventPortOverCurrentCleared ,Psm30StateWaitingForTimerToFlushOnOverCurrentClear },
        { PsmEventHubSuspend ,          Psm30StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventHubStop ,             Psm30StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventTimerFired ,          Psm30StateCheckingIfADeviceIsPresent },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForTimerToFlushOnHubStopSuspend = {
    // State ETW Name
    Psm30StateEtwWaitingForTimerToFlushOnHubStopSuspend,
    // State Entry Function
    HUBPSM30_WaitingForTimerToFlushOnHubStopSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStatusChange ,     Psm30StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventTimerFired ,          Psm30StateReleasingPowerReferenceonPoweredOffDisabled },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForTimerToFlushOnHubStopSuspendInDisconnected = {
    // State ETW Name
    Psm30StateEtwWaitingForTimerToFlushOnHubStopSuspendInDisconnected,
    // State Entry Function
    HUBPSM30_WaitingForTimerToFlushOnHubStopSuspendInDisconnected,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventTimerFired ,          Psm30StateReleasingPowerReferenceonPoweredOffDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForTimerToFlushOnOverCurrent = {
    // State ETW Name
    Psm30StateEtwWaitingForTimerToFlushOnOverCurrent,
    // State Entry Function
    HUBPSM30_WaitingForTimerToFlushOnOverCurrent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForTimerToFlushOnOverCurrent,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm30StateWaitingForTimerToFlushOnOverCurrentClear },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubResetWithPendingTimer },
        { PsmEventPortConnectChange ,   Psm30StateWaitingForTimerToFlushOnOverCurrent },
        { PsmEventPortOverCurrent ,     Psm30StateWaitingForTimerToFlushOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateWaitingForTimerToFlushOnOverCurrent },
        { PsmEventPortOverCurrent ,     Psm30StateWaitingForTimerToFlushOnOverCurrent },
        { PsmEventTimerFired ,          Psm30StateStartingOverCurrentTimer },
        { PsmEventHubStop ,             Psm30StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventHubSuspend ,          Psm30StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForTimerToFlushOnOverCurrentClear = {
    // State ETW Name
    Psm30StateEtwWaitingForTimerToFlushOnOverCurrentClear,
    // State Entry Function
    HUBPSM30_WaitingForTimerToFlushOnOverCurrentClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForTimerToFlushOnOverCurrentClear,
    // Event State Pairs for Transitions
    {
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm30StateWaitingForTimerToFlushOnOverCurrentClear },
        { PsmEventPortResetHubOnError , Psm30StateIssuingHubResetWithPendingTimer },
        { PsmEventPortOverCurrentCleared ,Psm30StateWaitingForTimerToFlushOnOverCurrentClear },
        { PsmEventPortConnectChange ,   Psm30StateWaitingForTimerToFlushOnOverCurrentClear },
        { PsmEventPortOverCurrent ,     Psm30StateWaitingForTimerToFlushOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateWaitingForTimerToFlushOnOverCurrentClear },
        { PsmEventHubStop ,             Psm30StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventTimerFired ,          Psm30StateSettingPortPowerOnOverCurrent },
        { PsmEventHubSuspend ,          Psm30StateWaitingForTimerToFlushOnHubStopSuspendInDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForUserResetOnOverCurrent = {
    // State ETW Name
    Psm30StateEtwWaitingForUserResetOnOverCurrent,
    // State Entry Function
    HUBPSM30_WaitingForUserResetOnOverCurrent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForUserResetOnOverCurrent,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventHubSurpriseRemove ,   Psm30StateIgnored },
        { PsmEventPortOverCurrentCleared ,Psm30StateFlushingUserResetForOverCurrentOnOverCurrentClear },
        { PsmEventPortConnectChange ,   Psm30StateWaitingForUserResetOnOverCurrent },
        { PsmEventPortResetHubOnError , Psm30StateResettingHubAndFlushingUserInitiatedResetOnOverCurrent },
        { PsmEventPortConnectChange ,   Psm30StateWaitingForUserResetOnOverCurrent },
        { PsmEventPortOverCurrent ,     Psm30StateStartingOverCurrentTimer },
        { PsmEventUserInitiatedResetOnOverCurrent ,Psm30StateSettingPortPowerOnOverCurrent },
        { PsmEventHubStop ,             Psm30StateFlushingUserResetForOverCurrentOnHubStopSuspend },
        { PsmEventHubSuspend ,          Psm30StateFlushingUserResetForOverCurrentOnHubStopSuspend },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingToBeDeleted = {
    // State ETW Name
    Psm30StateEtwWaitingToBeDeleted,
    // State Entry Function
    HUBPSM30_WaitingToBeDeleted,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAckingPortChange = {
    // State ETW Name
    Psm30StateEtwAckingPortChange,
    // State Entry Function
    HUBPSM30_AckingPortChange,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm30StateEnablingInterruptsOnFailure },
        { PsmEventTransferSuccess ,     Psm30StateCheckIfThereIsAPortChange },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckIfThereIsAPortChange = {
    // State ETW Name
    Psm30StateEtwCheckIfThereIsAPortChange,
    // State Entry Function
    HUBPSM30_CheckIfThereIsAPortChange,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortError ,           Psm30StateEnablingInterruptsOnFailure },
        { PsmEventYes ,                 Psm30StateAckingPortChange },
        { PsmEventNo ,                  Psm30StateGettingPortStatus },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckIfThereIsAPortChangeOnGetPortStatus = {
    // State ETW Name
    Psm30StateEtwCheckIfThereIsAPortChangeOnGetPortStatus,
    // State Entry Function
    HUBPSM30_CheckIfThereIsAPortChangeOnGetPortStatus,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateAckingPortChange },
        { PsmEventPortError ,           Psm30StateEnablingInterruptsOnFailure },
        { PsmEventNo ,                  Psm30StateEnablingInterruptsAndGettingPortEvent },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryEnablingInterruptsAndGettingPortEvent = {
    // State ETW Name
    Psm30StateEtwEnablingInterruptsAndGettingPortEvent,
    // State Entry Function
    HUBPSM30_EnablingInterruptsAndGettingPortEvent,
    // State Flags
    StateFlagEndSubStateMachine|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryEnablingInterruptsOnFailure = {
    // State ETW Name
    Psm30StateEtwEnablingInterruptsOnFailure,
    // State Entry Function
    HUBPSM30_EnablingInterruptsOnFailure,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReturningPortResetHubOnError },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatus = {
    // State ETW Name
    Psm30StateEtwGettingPortStatus,
    // State Entry Function
    HUBPSM30_GettingPortStatus,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm30StateEnablingInterruptsOnFailure },
        { PsmEventTransferSuccess ,     Psm30StateCheckIfThereIsAPortChangeOnGetPortStatus },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryInitializingCumulativePortChangeBits = {
    // State ETW Name
    Psm30StateEtwInitializingCumulativePortChangeBits,
    // State Entry Function
    HUBPSM30_InitializingCumulativePortChangeBits,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateGettingPortStatus },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortResetHubOnError = {
    // State ETW Name
    Psm30StateEtwReturningPortResetHubOnError,
    // State Entry Function
    HUBPSM30_ReturningPortResetHubOnError,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAckingPortChangeInNoChange = {
    // State ETW Name
    Psm30StateEtwAckingPortChangeInNoChange,
    // State Entry Function
    HUBPSM30_AckingPortChangeInNoChange,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm30StateReturningPortResetHubOnErrorInNoChange },
        { PsmEventTransferSuccess ,     Psm30StateCheckIfThereIsAPortChangeInNoChange },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckIfThereIsAPortChangeInNoChange = {
    // State ETW Name
    Psm30StateEtwCheckIfThereIsAPortChangeInNoChange,
    // State Entry Function
    HUBPSM30_CheckIfThereIsAPortChangeInNoChange,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortError ,           Psm30StateReturningPortResetHubOnErrorInNoChange },
        { PsmEventYes ,                 Psm30StateAckingPortChangeInNoChange },
        { PsmEventNo ,                  Psm30StateGettingPortStatusInNoChange },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckIfThereIsAPortChangeOnGetPortStatusInNoChange = {
    // State ETW Name
    Psm30StateEtwCheckIfThereIsAPortChangeOnGetPortStatusInNoChange,
    // State Entry Function
    HUBPSM30_CheckIfThereIsAPortChangeOnGetPortStatusInNoChange,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortError ,           Psm30StateReturningPortResetHubOnErrorInNoChange },
        { PsmEventYes ,                 Psm30StateAckingPortChangeInNoChange },
        { PsmEventNo ,                  Psm30StateEnablingInterruptsAndGettingPortEventInNoChange },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryEnablingInterruptsAndGettingPortEventInNoChange = {
    // State ETW Name
    Psm30StateEtwEnablingInterruptsAndGettingPortEventInNoChange,
    // State Entry Function
    HUBPSM30_EnablingInterruptsAndGettingPortEventInNoChange,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatusInNoChange = {
    // State ETW Name
    Psm30StateEtwGettingPortStatusInNoChange,
    // State Entry Function
    HUBPSM30_GettingPortStatusInNoChange,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm30StateReturningPortResetHubOnErrorInNoChange },
        { PsmEventTransferSuccess ,     Psm30StateCheckIfThereIsAPortChangeOnGetPortStatusInNoChange },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryInitializingCumulativePortChangeBitsInNoChange = {
    // State ETW Name
    Psm30StateEtwInitializingCumulativePortChangeBitsInNoChange,
    // State Entry Function
    HUBPSM30_InitializingCumulativePortChangeBitsInNoChange,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateGettingPortStatusInNoChange },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortResetHubOnErrorInNoChange = {
    // State ETW Name
    Psm30StateEtwReturningPortResetHubOnErrorInNoChange,
    // State Entry Function
    HUBPSM30_ReturningPortResetHubOnErrorInNoChange,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAckingPortChangeOnStartResume = {
    // State ETW Name
    Psm30StateEtwAckingPortChangeOnStartResume,
    // State Entry Function
    HUBPSM30_AckingPortChangeOnStartResume,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm30StateAcquiringPortReferenceOnFailure },
        { PsmEventTransferSuccess ,     Psm30StateCheckIfThereIsAPortChangeOnStartResume },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAcquiringPortReferenceAndGettingPortEvent = {
    // State ETW Name
    Psm30StateEtwAcquiringPortReferenceAndGettingPortEvent,
    // State Entry Function
    HUBPSM30_AcquiringPortReferenceAndGettingPortEvent,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAcquiringPortReferenceOnFailure = {
    // State ETW Name
    Psm30StateEtwAcquiringPortReferenceOnFailure,
    // State Entry Function
    HUBPSM30_AcquiringPortReferenceOnFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReturningPortResetHubOnErrorOnStartResume },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckIfThereIsAPortChangeOnStartResume = {
    // State ETW Name
    Psm30StateEtwCheckIfThereIsAPortChangeOnStartResume,
    // State Entry Function
    HUBPSM30_CheckIfThereIsAPortChangeOnStartResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortError ,           Psm30StateAcquiringPortReferenceOnFailure },
        { PsmEventYes ,                 Psm30StateAckingPortChangeOnStartResume },
        { PsmEventNo ,                  Psm30StateGettingPortStatusOnStartResume },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckIfThereIsAPortChangeOnStartResumeOnGetPortStatus = {
    // State ETW Name
    Psm30StateEtwCheckIfThereIsAPortChangeOnStartResumeOnGetPortStatus,
    // State Entry Function
    HUBPSM30_CheckIfThereIsAPortChangeOnStartResumeOnGetPortStatus,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateAckingPortChangeOnStartResume },
        { PsmEventPortError ,           Psm30StateAcquiringPortReferenceOnFailure },
        { PsmEventNo ,                  Psm30StateAcquiringPortReferenceAndGettingPortEvent },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatusOnStartResume = {
    // State ETW Name
    Psm30StateEtwGettingPortStatusOnStartResume,
    // State Entry Function
    HUBPSM30_GettingPortStatusOnStartResume,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm30StateAcquiringPortReferenceOnFailure },
        { PsmEventTransferSuccess ,     Psm30StateCheckIfThereIsAPortChangeOnStartResumeOnGetPortStatus },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryInitializingCumulativePortChangeBitsOnStartResume = {
    // State ETW Name
    Psm30StateEtwInitializingCumulativePortChangeBitsOnStartResume,
    // State Entry Function
    HUBPSM30_InitializingCumulativePortChangeBitsOnStartResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateGettingPortStatusOnStartResume },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortResetHubOnErrorOnStartResume = {
    // State ETW Name
    Psm30StateEtwReturningPortResetHubOnErrorOnStartResume,
    // State Entry Function
    HUBPSM30_ReturningPortResetHubOnErrorOnStartResume,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortChange = {
    // State ETW Name
    Psm30StateEtwGettingPortChange,
    // State Entry Function
    HUBPSM30_GettingPortChange,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    Psm30SubSmGettingPortChange,
    // Event State Pairs for Transitions
    {
        { PsmEventPortError ,           Psm30StateReturningPortResetHubOnErrorInPortChangeHandler },
        { PsmEventPortLinkStateError ,  Psm30StateReturningConnectChangeOnLinkError },
        { PsmEventPortResetInProgress , Psm30StateReturningPortResetHubOnErrorInPortChangeHandler },
        { PsmEventPortNoChange ,        Psm30StateWaitingForPortChangeEvent },
        { PsmEventPortResumed ,         Psm30StateWaitingForPortChangeEvent },
        { PsmEventPortResetComplete ,   Psm30StateReturningPortResetHubOnErrorInPortChangeHandler },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningConnectChangeOnLinkError = {
    // State ETW Name
    Psm30StateEtwReturningConnectChangeOnLinkError,
    // State Entry Function
    HUBPSM30_ReturningConnectChangeOnLinkError,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortResetHubOnErrorInPortChangeHandler = {
    // State ETW Name
    Psm30StateEtwReturningPortResetHubOnErrorInPortChangeHandler,
    // State Entry Function
    HUBPSM30_ReturningPortResetHubOnErrorInPortChangeHandler,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForPortChangeEvent = {
    // State ETW Name
    Psm30StateEtwWaitingForPortChangeEvent,
    // State Entry Function
    HUBPSM30_WaitingForPortChangeEvent,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStatusChange ,     Psm30StateGettingPortChange },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAcquiringPortReferenceOnTransferFailure = {
    // State ETW Name
    Psm30StateEtwAcquiringPortReferenceOnTransferFailure,
    // State Entry Function
    HUBPSM30_AcquiringPortReferenceOnTransferFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReturningPortResetHubOnErrorOnStart },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfPortIsPoweredOnStart = {
    // State ETW Name
    Psm30StateEtwCheckingIfPortIsPoweredOnStart,
    // State Entry Function
    HUBPSM30_CheckingIfPortIsPoweredOnStart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateGettingPortStatusOnStart },
        { PsmEventNo ,                  Psm30StatePoweringOnPortOnStart },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfPowerOnStartHackIsPresent = {
    // State ETW Name
    Psm30StateEtwCheckingIfPowerOnStartHackIsPresent,
    // State Entry Function
    HUBPSM30_CheckingIfPowerOnStartHackIsPresent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateGettingPortStatusOnStartBeforePoweringOn },
        { PsmEventNo ,                  Psm30StateGettingPortStatusOnStart },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatusOnStart = {
    // State ETW Name
    Psm30StateEtwGettingPortStatusOnStart,
    // State Entry Function
    HUBPSM30_GettingPortStatusOnStart,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    Psm30SubSmGettingPortStatusOnStart,
    // Event State Pairs for Transitions
    {
        { PsmEventPortError ,           Psm30StateReturningPortResetHubOnErrorOnStart },
        { PsmEventPortResumed ,         Psm30StateReturningPortConnectChangeOnLinkErrorOnStart },
        { PsmEventPortResetComplete ,   Psm30StateReturningPortConnectChangeOnLinkErrorOnStart },
        { PsmEventPortLinkStateError ,  Psm30StateReturningPortConnectChangeOnLinkErrorOnStart },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatusOnStartBeforePoweringOn = {
    // State ETW Name
    Psm30StateEtwGettingPortStatusOnStartBeforePoweringOn,
    // State Entry Function
    HUBPSM30_GettingPortStatusOnStartBeforePoweringOn,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferSuccess ,     Psm30StateCheckingIfPortIsPoweredOnStart },
        { PsmEventTransferFailure ,     Psm30StateAcquiringPortReferenceOnTransferFailure },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryPoweringOnPortOnStart = {
    // State ETW Name
    Psm30StateEtwPoweringOnPortOnStart,
    // State Entry Function
    HUBPSM30_PoweringOnPortOnStart,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm30StateAcquiringPortReferenceOnTransferFailure },
        { PsmEventTransferSuccess ,     Psm30StateWaitingForPortPowerOnTimerOnStart },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortConnectChangeOnLinkErrorOnStart = {
    // State ETW Name
    Psm30StateEtwReturningPortConnectChangeOnLinkErrorOnStart,
    // State Entry Function
    HUBPSM30_ReturningPortConnectChangeOnLinkErrorOnStart,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortResetHubOnErrorOnStart = {
    // State ETW Name
    Psm30StateEtwReturningPortResetHubOnErrorOnStart,
    // State Entry Function
    HUBPSM30_ReturningPortResetHubOnErrorOnStart,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForPortPowerOnTimerOnStart = {
    // State ETW Name
    Psm30StateEtwWaitingForPortPowerOnTimerOnStart,
    // State Entry Function
    HUBPSM30_WaitingForPortPowerOnTimerOnStart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventTimerFired ,          Psm30StateGettingPortStatusOnStart },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfDeviceIsStillConnectedOnResume = {
    // State ETW Name
    Psm30StateEtwCheckingIfDeviceIsStillConnectedOnResume,
    // State Entry Function
    HUBPSM30_CheckingIfDeviceIsStillConnectedOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateQueueingStateDisabledOnConnectChangeOnResume },
        { PsmEventNo ,                  Psm30StateStartingTimerForAllowingReconnect },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatusOnResume = {
    // State ETW Name
    Psm30StateEtwGettingPortStatusOnResume,
    // State Entry Function
    HUBPSM30_GettingPortStatusOnResume,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    Psm30SubSmGettingPortStatusOnResume,
    // Event State Pairs for Transitions
    {
        { PsmEventPortNoChange ,        Psm30StateQueueingStateSuspendedOnResume },
        { PsmEventPortResumed ,         Psm30StateQueueingStateEnabledOnResume },
        { PsmEventPortConnectChange ,   Psm30StateCheckingIfDeviceIsStillConnectedOnResume },
        { PsmEventPortResetInProgress , Psm30StateQueueingStateDisabledOnResetInProgress },
        { PsmEventPortLinkStateError ,  Psm30StateQueueingStateDisabledOnFailure },
        { PsmEventPortResetComplete ,   Psm30StateQueueingStateDisabledOnFailure },
        { PsmEventPortError ,           Psm30StateReturningPortResetHubOnErrorOnResume },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatusOnResumeAfterWaiting = {
    // State ETW Name
    Psm30StateEtwGettingPortStatusOnResumeAfterWaiting,
    // State Entry Function
    HUBPSM30_GettingPortStatusOnResumeAfterWaiting,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    Psm30SubSmGettingPortStatusOnResumeAfterWaiting,
    // Event State Pairs for Transitions
    {
        { PsmEventPortLinkStateError ,  Psm30StateQueueingStateDisabledOnFailure },
        { PsmEventPortResumed ,         Psm30StateReturningPortResetHubOnErrorOnResume },
        { PsmEventPortNoChange ,        Psm30StateReturningPortConnectChange },
        { PsmEventPortError ,           Psm30StateReturningPortResetHubOnErrorOnResume },
        { PsmEventPortConnectChange ,   Psm30StateQueueingStateDisabledOnConnectChangeOnResume },
        { PsmEventPortResetComplete ,   Psm30StateReturningPortResetHubOnErrorOnResume },
        { PsmEventPortResetInProgress , Psm30StateReturningPortResetHubOnErrorOnResume },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueingStateDisabledOnConnectChangeOnResume = {
    // State ETW Name
    Psm30StateEtwQueueingStateDisabledOnConnectChangeOnResume,
    // State Entry Function
    HUBPSM30_QueueingStateDisabledOnConnectChangeOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReturningPortResumedOnResume },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueingStateDisabledOnFailure = {
    // State ETW Name
    Psm30StateEtwQueueingStateDisabledOnFailure,
    // State Entry Function
    HUBPSM30_QueueingStateDisabledOnFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReturningPortNeedsResetOnResume },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueingStateDisabledOnResetInProgress = {
    // State ETW Name
    Psm30StateEtwQueueingStateDisabledOnResetInProgress,
    // State Entry Function
    HUBPSM30_QueueingStateDisabledOnResetInProgress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReturningPortResetInProgressOnResume },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueingStateEnabledOnResume = {
    // State ETW Name
    Psm30StateEtwQueueingStateEnabledOnResume,
    // State Entry Function
    HUBPSM30_QueueingStateEnabledOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReturningPortResumedOnResume },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueingStateSuspendedOnResume = {
    // State ETW Name
    Psm30StateEtwQueueingStateSuspendedOnResume,
    // State Entry Function
    HUBPSM30_QueueingStateSuspendedOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReturningPortOperationSuccessOnResume },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningHubStopOnResume = {
    // State ETW Name
    Psm30StateEtwReturningHubStopOnResume,
    // State Entry Function
    HUBPSM30_ReturningHubStopOnResume,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortConnectChange = {
    // State ETW Name
    Psm30StateEtwReturningPortConnectChange,
    // State Entry Function
    HUBPSM30_ReturningPortConnectChange,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortNeedsResetOnResume = {
    // State ETW Name
    Psm30StateEtwReturningPortNeedsResetOnResume,
    // State Entry Function
    HUBPSM30_ReturningPortNeedsResetOnResume,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortOperationSuccessOnResume = {
    // State ETW Name
    Psm30StateEtwReturningPortOperationSuccessOnResume,
    // State Entry Function
    HUBPSM30_ReturningPortOperationSuccessOnResume,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortResetHubOnErrorOnResume = {
    // State ETW Name
    Psm30StateEtwReturningPortResetHubOnErrorOnResume,
    // State Entry Function
    HUBPSM30_ReturningPortResetHubOnErrorOnResume,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortResetInProgressOnResume = {
    // State ETW Name
    Psm30StateEtwReturningPortResetInProgressOnResume,
    // State Entry Function
    HUBPSM30_ReturningPortResetInProgressOnResume,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortResumedOnResume = {
    // State ETW Name
    Psm30StateEtwReturningPortResumedOnResume,
    // State Entry Function
    HUBPSM30_ReturningPortResumedOnResume,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStartingTimerForAllowingReconnect = {
    // State ETW Name
    Psm30StateEtwStartingTimerForAllowingReconnect,
    // State Entry Function
    HUBPSM30_StartingTimerForAllowingReconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateWaitingForDeviceToReconnect },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStoppingReconnectTimerOnHubStatusChange = {
    // State ETW Name
    Psm30StateEtwStoppingReconnectTimerOnHubStatusChange,
    // State Entry Function
    HUBPSM30_StoppingReconnectTimerOnHubStatusChange,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForReconnectTimerToFlushOnHubStatusChange },
        { PsmEventOperationSuccess ,    Psm30StateGettingPortStatusOnResumeAfterWaiting },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStoppingReconnectTimerOnHubStop = {
    // State ETW Name
    Psm30StateEtwStoppingReconnectTimerOnHubStop,
    // State Entry Function
    HUBPSM30_StoppingReconnectTimerOnHubStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForReconnectTimerToFlushOnHubStop },
        { PsmEventOperationSuccess ,    Psm30StateReturningHubStopOnResume },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForDeviceToReconnect = {
    // State ETW Name
    Psm30StateEtwWaitingForDeviceToReconnect,
    // State Entry Function
    HUBPSM30_WaitingForDeviceToReconnect,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStop ,             Psm30StateStoppingReconnectTimerOnHubStop },
        { PsmEventHubStatusChange ,     Psm30StateStoppingReconnectTimerOnHubStatusChange },
        { PsmEventTimerFired ,          Psm30StateReturningPortConnectChange },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForReconnectTimerToFlushOnHubStatusChange = {
    // State ETW Name
    Psm30StateEtwWaitingForReconnectTimerToFlushOnHubStatusChange,
    // State Entry Function
    HUBPSM30_WaitingForReconnectTimerToFlushOnHubStatusChange,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTimerFired ,          Psm30StateGettingPortStatusOnResumeAfterWaiting },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForReconnectTimerToFlushOnHubStop = {
    // State ETW Name
    Psm30StateEtwWaitingForReconnectTimerToFlushOnHubStop,
    // State Entry Function
    HUBPSM30_WaitingForReconnectTimerToFlushOnHubStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTimerFired ,          Psm30StateReturningHubStopOnResume },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatusInResuming = {
    // State ETW Name
    Psm30StateEtwGettingPortStatusInResuming,
    // State Entry Function
    HUBPSM30_GettingPortStatusInResuming,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    Psm30SubSmGettingPortStatusInResuming,
    // Event State Pairs for Transitions
    {
        { PsmEventPortNoChange ,        Psm30StateWaitingForPortChangeEventInResuming },
        { PsmEventPortLinkStateError ,  Psm30StateReturningConnectChangeOnLinkErrorInResuming },
        { PsmEventPortError ,           Psm30StateReturningPortResetHubOnErrorInResuming },
        { PsmEventPortResetInProgress , Psm30StateReturningPortResetHubOnErrorInResuming },
        { PsmEventPortResetComplete ,   Psm30StateReturningPortResetHubOnErrorInResuming },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningConnectChangeOnLinkErrorInResuming = {
    // State ETW Name
    Psm30StateEtwReturningConnectChangeOnLinkErrorInResuming,
    // State Entry Function
    HUBPSM30_ReturningConnectChangeOnLinkErrorInResuming,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortResetHubOnErrorInResuming = {
    // State ETW Name
    Psm30StateEtwReturningPortResetHubOnErrorInResuming,
    // State Entry Function
    HUBPSM30_ReturningPortResetHubOnErrorInResuming,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForPortChangeEventInResuming = {
    // State ETW Name
    Psm30StateEtwWaitingForPortChangeEventInResuming,
    // State Entry Function
    HUBPSM30_WaitingForPortChangeEventInResuming,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStatusChange ,     Psm30StateGettingPortStatusInResuming },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatusInWaitingForStopSuspend = {
    // State ETW Name
    Psm30StateEtwGettingPortStatusInWaitingForStopSuspend,
    // State Entry Function
    HUBPSM30_GettingPortStatusInWaitingForStopSuspend,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    Psm30SubSmGettingPortStatusInWaitingForStopSuspend,
    // Event State Pairs for Transitions
    {
        { PsmEventPortNoChange ,        Psm30StateWaitingForDevicePortEvents },
        { PsmEventPortResetComplete ,   Psm30StateWaitingForDevicePortEvents },
        { PsmEventPortResetHubOnError , Psm30StateWaitingForDevicePortEvents },
        { PsmEventPortLinkStateError ,  Psm30StateWaitingForDevicePortEvents },
        { PsmEventPortOverCurrentCleared ,Psm30StateWaitingForDevicePortEvents },
        { PsmEventPortResetInProgress , Psm30StateWaitingForDevicePortEvents },
        { PsmEventPortError ,           Psm30StateWaitingForDevicePortEvents },
        { PsmEventPortOverCurrent ,     Psm30StateWaitingForDevicePortEvents },
        { PsmEventPortResumed ,         Psm30StateWaitingForDevicePortEvents },
        { PsmEventPortConnectChange ,   Psm30StateWaitingForDevicePortEvents },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueingPortEventFailureToDSM = {
    // State ETW Name
    Psm30StateEtwQueueingPortEventFailureToDSM,
    // State Entry Function
    HUBPSM30_QueueingPortEventFailureToDSM,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateWaitingForDevicePortEvents },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForDevicePortEvents = {
    // State ETW Name
    Psm30StateEtwWaitingForDevicePortEvents,
    // State Entry Function
    HUBPSM30_WaitingForDevicePortEvents,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceRequestCycle ,  Psm30StateIgnored },
        { PsmEventDeviceSetU1Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceSetU2Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceRequestWarmReset ,Psm30StateQueueingPortEventFailureToDSM },
        { PsmEventDeviceRequestDisable ,Psm30StateQueueingPortEventFailureToDSM },
        { PsmEventHubStatusChange ,     Psm30StateGettingPortStatusInWaitingForStopSuspend },
        { PsmEventDeviceRequestSuspend ,Psm30StateQueueingPortEventFailureToDSM },
        { PsmEventDeviceRequestReset ,  Psm30StateQueueingPortEventFailureToDSM },
        { PsmEventDeviceRequestResume , Psm30StateQueueingPortEventFailureToDSM },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatusInWaitingForStopSuspendInDisconnected = {
    // State ETW Name
    Psm30StateEtwGettingPortStatusInWaitingForStopSuspendInDisconnected,
    // State Entry Function
    HUBPSM30_GettingPortStatusInWaitingForStopSuspendInDisconnected,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    Psm30SubSmGettingPortStatusInWaitingForStopSuspendInDisconnected,
    // Event State Pairs for Transitions
    {
        { PsmEventPortNoChange ,        Psm30StateWaitingForDevicePortEventsInDisconnected },
        { PsmEventPortOverCurrentCleared ,Psm30StateWaitingForDevicePortEventsInDisconnected },
        { PsmEventPortResetHubOnError , Psm30StateWaitingForDevicePortEventsInDisconnected },
        { PsmEventPortLinkStateError ,  Psm30StateWaitingForDevicePortEventsInDisconnected },
        { PsmEventPortResetComplete ,   Psm30StateWaitingForDevicePortEventsInDisconnected },
        { PsmEventPortResetInProgress , Psm30StateWaitingForDevicePortEventsInDisconnected },
        { PsmEventPortError ,           Psm30StateWaitingForDevicePortEventsInDisconnected },
        { PsmEventPortOverCurrent ,     Psm30StateWaitingForDevicePortEventsInDisconnected },
        { PsmEventPortResumed ,         Psm30StateWaitingForDevicePortEventsInDisconnected },
        { PsmEventPortConnectChange ,   Psm30StateWaitingForDevicePortEventsInDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForDevicePortEventsInDisconnected = {
    // State ETW Name
    Psm30StateEtwWaitingForDevicePortEventsInDisconnected,
    // State Entry Function
    HUBPSM30_WaitingForDevicePortEventsInDisconnected,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceRequestResume , Psm30StateIgnored },
        { PsmEventDeviceRequestCycle ,  Psm30StateIgnored },
        { PsmEventDeviceSetU2Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceRequestReset ,  Psm30StateIgnored },
        { PsmEventDeviceRequestDisable ,Psm30StateIgnored },
        { PsmEventDeviceRequestSuspend ,Psm30StateIgnored },
        { PsmEventDeviceSetU1Timeout ,  Psm30StateIgnored },
        { PsmEventDeviceDetached ,      Psm30StateIgnored },
        { PsmEventDeviceRequestWarmReset ,Psm30StateIgnored },
        { PsmEventDeviceRequestDisableSuperSpeed ,Psm30StateIgnored },
        { PsmEventHubStatusChange ,     Psm30StateGettingPortStatusInWaitingForStopSuspendInDisconnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfLinkIsInU0AfterResetPolling = {
    // State ETW Name
    Psm30StateEtwCheckingIfLinkIsInU0AfterResetPolling,
    // State Entry Function
    HUBPSM30_CheckingIfLinkIsInU0AfterResetPolling,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateReturningPortResetCompleted },
        { PsmEventNo ,                  Psm30StateReturningPortResetHubOnErrorInReset },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfResetCompletionRequiresPolling = {
    // State ETW Name
    Psm30StateEtwCheckingIfResetCompletionRequiresPolling,
    // State Entry Function
    HUBPSM30_CheckingIfResetCompletionRequiresPolling,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateWaitingForPollingIntervalForResetCompletion },
        { PsmEventNo ,                  Psm30StateWaitingForPortChangeEventInReset },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatusInReset = {
    // State ETW Name
    Psm30StateEtwGettingPortStatusInReset,
    // State Entry Function
    HUBPSM30_GettingPortStatusInReset,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    Psm30SubSmGettingPortStatusInReset,
    // Event State Pairs for Transitions
    {
        { PsmEventPortNoChange ,        Psm30StateWaitingForPortChangeEventInReset },
        { PsmEventPortResumed ,         Psm30StateReturningPortResetHubOnErrorInReset },
        { PsmEventPortLinkStateError ,  Psm30StateWaitingForPortChangeEventInReset },
        { PsmEventPortResetInProgress , Psm30StateCheckingIfResetCompletionRequiresPolling },
        { PsmEventPortError ,           Psm30StateReturningPortResetHubOnErrorInReset },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryGettingPortStatusInResetAfterPollingInterval = {
    // State ETW Name
    Psm30StateEtwGettingPortStatusInResetAfterPollingInterval,
    // State Entry Function
    HUBPSM30_GettingPortStatusInResetAfterPollingInterval,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    Psm30SubSmGettingPortStatusInResetAfterPollingInterval,
    // Event State Pairs for Transitions
    {
        { PsmEventPortLinkStateError ,  Psm30StateReturningPortResetHubOnErrorInReset },
        { PsmEventPortNoChange ,        Psm30StateCheckingIfLinkIsInU0AfterResetPolling },
        { PsmEventPortResetInProgress , Psm30StateGettingPortStatusInReset },
        { PsmEventPortResumed ,         Psm30StateReturningPortResetHubOnErrorInReset },
        { PsmEventPortResumed ,         Psm30StateReturningPortResetHubOnErrorInReset },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortResetCompleted = {
    // State ETW Name
    Psm30StateEtwReturningPortResetCompleted,
    // State Entry Function
    HUBPSM30_ReturningPortResetCompleted,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortResetHubOnErrorInReset = {
    // State ETW Name
    Psm30StateEtwReturningPortResetHubOnErrorInReset,
    // State Entry Function
    HUBPSM30_ReturningPortResetHubOnErrorInReset,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForPollingIntervalForResetCompletion = {
    // State ETW Name
    Psm30StateEtwWaitingForPollingIntervalForResetCompletion,
    // State Entry Function
    HUBPSM30_WaitingForPollingIntervalForResetCompletion,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResetPollingTimerFired ,Psm30StateGettingPortStatusInResetAfterPollingInterval },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForPortChangeEventInReset = {
    // State ETW Name
    Psm30StateEtwWaitingForPortChangeEventInReset,
    // State Entry Function
    HUBPSM30_WaitingForPortChangeEventInReset,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStatusChange ,     Psm30StateGettingPortStatusInReset },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfBootDeviceIsConnected = {
    // State ETW Name
    Psm30StateEtwCheckingIfBootDeviceIsConnected,
    // State Entry Function
    HUBPSM30_CheckingIfBootDeviceIsConnected,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateEnablingRemoteWakeOnPort },
        { PsmEventNo ,                  Psm30StateDisablingRemoteWakeOnPort },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfParentHubIsArmedForWake = {
    // State ETW Name
    Psm30StateEtwCheckingIfParentHubIsArmedForWake,
    // State Entry Function
    HUBPSM30_CheckingIfParentHubIsArmedForWake,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateEnablingRemoteWakeOnPort },
        { PsmEventNo ,                  Psm30StateCheckingIfBootDeviceIsConnected },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryDisablingRemoteWakeOnPort = {
    // State ETW Name
    Psm30StateEtwDisablingRemoteWakeOnPort,
    // State Entry Function
    HUBPSM30_DisablingRemoteWakeOnPort,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferFailure ,     Psm30StateIssuingHubResetOnRemoteWakeMaskFailure },
        { PsmEventTransferSuccess ,     Psm30StateReturningOperationSucessFromArmingForWake },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryEnablingRemoteWakeOnPort = {
    // State ETW Name
    Psm30StateEtwEnablingRemoteWakeOnPort,
    // State Entry Function
    HUBPSM30_EnablingRemoteWakeOnPort,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferSuccess ,     Psm30StateReturningOperationSucessFromArmingForWake },
        { PsmEventTransferFailure ,     Psm30StateIssuingHubResetOnRemoteWakeMaskFailure },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryIssuingHubResetOnRemoteWakeMaskFailure = {
    // State ETW Name
    Psm30StateEtwIssuingHubResetOnRemoteWakeMaskFailure,
    // State Entry Function
    HUBPSM30_IssuingHubResetOnRemoteWakeMaskFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReturningOperationSucessFromArmingForWake },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningOperationSucessFromArmingForWake = {
    // State ETW Name
    Psm30StateEtwReturningOperationSucessFromArmingForWake,
    // State Entry Function
    HUBPSM30_ReturningOperationSucessFromArmingForWake,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryAcquiringPowerReferenceOnHubResumeInD3Cold = {
    // State ETW Name
    Psm30StateEtwAcquiringPowerReferenceOnHubResumeInD3Cold,
    // State Entry Function
    HUBPSM30_AcquiringPowerReferenceOnHubResumeInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateWaitingForPortChangeEventInD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold = {
    // State ETW Name
    Psm30StateEtwCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold,
    // State Entry Function
    HUBPSM30_CheckingIfDeviceIsConnectedOnConnectChangeForD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateQueueingEnabledToDSMOnReconnectFromD3Cold },
        { PsmEventNo ,                  Psm30StateWaitingForPortChangeEventInD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfDeviceIsConnectedOnPortChangeInD3Cold = {
    // State ETW Name
    Psm30StateEtwCheckingIfDeviceIsConnectedOnPortChangeInD3Cold,
    // State Entry Function
    HUBPSM30_CheckingIfDeviceIsConnectedOnPortChangeInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateWaitingForDeviceRequestResumeOnD3Cold },
        { PsmEventNo ,                  Psm30StateReturningConnectChangeFromD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryCheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume = {
    // State ETW Name
    Psm30StateEtwCheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume,
    // State Entry Function
    HUBPSM30_CheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm30StateStoppingReconnectTimerOnDeviceReAttachAfterD3Cold },
        { PsmEventNo ,                  Psm30StateWaitingForDeviceToReconnectOnResumeFromD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryPoweredOffOnHubSuspendFromD3Cold = {
    // State ETW Name
    Psm30StateEtwPoweredOffOnHubSuspendFromD3Cold,
    // State Entry Function
    HUBPSM30_PoweredOffOnHubSuspendFromD3Cold,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubResumeInS0 ,       Psm30StateAcquiringPowerReferenceOnHubResumeInD3Cold },
        { PsmEventHubResumeWithReset ,  Psm30StateQueueingStateSuspendedOnHubResumeInD3Cold },
        { PsmEventHubResume ,           Psm30StateQueueingStateSuspendedOnHubResumeInD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueingEnabledToDSMOnReconnectAfterResumeRequestFromD3Cold = {
    // State ETW Name
    Psm30StateEtwQueueingEnabledToDSMOnReconnectAfterResumeRequestFromD3Cold,
    // State Entry Function
    HUBPSM30_QueueingEnabledToDSMOnReconnectAfterResumeRequestFromD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateReturningPortResumedFromD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueingEnabledToDSMOnReconnectFromD3Cold = {
    // State ETW Name
    Psm30StateEtwQueueingEnabledToDSMOnReconnectFromD3Cold,
    // State Entry Function
    HUBPSM30_QueueingEnabledToDSMOnReconnectFromD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateWaitingForDeviceRequestResumeOnD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryQueueingStateSuspendedOnHubResumeInD3Cold = {
    // State ETW Name
    Psm30StateEtwQueueingStateSuspendedOnHubResumeInD3Cold,
    // State Entry Function
    HUBPSM30_QueueingStateSuspendedOnHubResumeInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateAcquiringPowerReferenceOnHubResumeInD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReleasingPowerReferenceOnHubS0IdleInD3Cold = {
    // State ETW Name
    Psm30StateEtwReleasingPowerReferenceOnHubS0IdleInD3Cold,
    // State Entry Function
    HUBPSM30_ReleasingPowerReferenceOnHubS0IdleInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StatePoweredOffOnHubSuspendFromD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningConnectChangeFromD3Cold = {
    // State ETW Name
    Psm30StateEtwReturningConnectChangeFromD3Cold,
    // State Entry Function
    HUBPSM30_ReturningConnectChangeFromD3Cold,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningHubStopFromSuspendedInD3Cold = {
    // State ETW Name
    Psm30StateEtwReturningHubStopFromSuspendedInD3Cold,
    // State Entry Function
    HUBPSM30_ReturningHubStopFromSuspendedInD3Cold,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningOverCurrentClearedFromD3Cold = {
    // State ETW Name
    Psm30StateEtwReturningOverCurrentClearedFromD3Cold,
    // State Entry Function
    HUBPSM30_ReturningOverCurrentClearedFromD3Cold,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningOverCurrentFromD3Cold = {
    // State ETW Name
    Psm30StateEtwReturningOverCurrentFromD3Cold,
    // State Entry Function
    HUBPSM30_ReturningOverCurrentFromD3Cold,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortResetHubOnErrorFromD3Cold = {
    // State ETW Name
    Psm30StateEtwReturningPortResetHubOnErrorFromD3Cold,
    // State Entry Function
    HUBPSM30_ReturningPortResetHubOnErrorFromD3Cold,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryReturningPortResumedFromD3Cold = {
    // State ETW Name
    Psm30StateEtwReturningPortResumedFromD3Cold,
    // State Entry Function
    HUBPSM30_ReturningPortResumedFromD3Cold,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStartingTimerForAllowingReconnectOnResumingFromD3Cold = {
    // State ETW Name
    Psm30StateEtwStartingTimerForAllowingReconnectOnResumingFromD3Cold,
    // State Entry Function
    HUBPSM30_StartingTimerForAllowingReconnectOnResumingFromD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateWaitingForDeviceToReconnectOnResumeFromD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStoppingReconnectTimerOnDeviceReAttachAfterD3Cold = {
    // State ETW Name
    Psm30StateEtwStoppingReconnectTimerOnDeviceReAttachAfterD3Cold,
    // State Entry Function
    HUBPSM30_StoppingReconnectTimerOnDeviceReAttachAfterD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm30StateQueueingEnabledToDSMOnReconnectAfterResumeRequestFromD3Cold },
        { PsmEventOperationFailure ,    Psm30StateWaitingForReconnectTimerToFlushOnReattachAfterD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStoppingReconnectTimerOnHubStopInD3Cold = {
    // State ETW Name
    Psm30StateEtwStoppingReconnectTimerOnHubStopInD3Cold,
    // State Entry Function
    HUBPSM30_StoppingReconnectTimerOnHubStopInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForReconnectTimerToFlushOnHubStopInD3Cold },
        { PsmEventOperationSuccess ,    Psm30StateReturningHubStopFromSuspendedInD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStoppingReconnectTimerOnOvercurrentClearedInD3Cold = {
    // State ETW Name
    Psm30StateEtwStoppingReconnectTimerOnOvercurrentClearedInD3Cold,
    // State Entry Function
    HUBPSM30_StoppingReconnectTimerOnOvercurrentClearedInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold },
        { PsmEventOperationSuccess ,    Psm30StateReturningOverCurrentClearedFromD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStoppingReconnectTimerOnOvercurrentInD3Cold = {
    // State ETW Name
    Psm30StateEtwStoppingReconnectTimerOnOvercurrentInD3Cold,
    // State Entry Function
    HUBPSM30_StoppingReconnectTimerOnOvercurrentInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForReconnectTimerToFlushOnOvercurrentInD3Cold },
        { PsmEventOperationSuccess ,    Psm30StateReturningOverCurrentFromD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryStoppingReconnectTimerOnResetHubErrorInD3Cold = {
    // State ETW Name
    Psm30StateEtwStoppingReconnectTimerOnResetHubErrorInD3Cold,
    // State Entry Function
    HUBPSM30_StoppingReconnectTimerOnResetHubErrorInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationFailure ,    Psm30StateWaitingForReconnectTimerToFlushOnResetHubErrorInD3Cold },
        { PsmEventOperationSuccess ,    Psm30StateReturningPortResetHubOnErrorFromD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForDeviceRequestResumeOnD3Cold = {
    // State ETW Name
    Psm30StateEtwWaitingForDeviceRequestResumeOnD3Cold,
    // State Entry Function
    HUBPSM30_WaitingForDeviceRequestResumeOnD3Cold,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    Psm30SubSmWaitingForDeviceRequestResumeOnD3Cold,
    // Event State Pairs for Transitions
    {
        { PsmEventPortConnectChange ,   Psm30StateCheckingIfDeviceIsConnectedOnPortChangeInD3Cold },
        { PsmEventPortOverCurrentCleared ,Psm30StateReturningOverCurrentClearedFromD3Cold },
        { PsmEventPortOverCurrent ,     Psm30StateReturningOverCurrentFromD3Cold },
        { PsmEventPortResetHubOnError , Psm30StateReturningPortResetHubOnErrorFromD3Cold },
        { PsmEventHubStop ,             Psm30StateReturningHubStopFromSuspendedInD3Cold },
        { PsmEventDeviceRequestResume , Psm30StateReturningPortResumedFromD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForDeviceToReconnectOnResumeFromD3Cold = {
    // State ETW Name
    Psm30StateEtwWaitingForDeviceToReconnectOnResumeFromD3Cold,
    // State Entry Function
    HUBPSM30_WaitingForDeviceToReconnectOnResumeFromD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm30SubSmWaitingForDeviceToReconnectOnResumeFromD3Cold,
    // Event State Pairs for Transitions
    {
        { PsmEventPortOverCurrentCleared ,Psm30StateStoppingReconnectTimerOnOvercurrentClearedInD3Cold },
        { PsmEventPortOverCurrent ,     Psm30StateStoppingReconnectTimerOnOvercurrentInD3Cold },
        { PsmEventPortResetHubOnError , Psm30StateStoppingReconnectTimerOnResetHubErrorInD3Cold },
        { PsmEventPortConnectChange ,   Psm30StateCheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume },
        { PsmEventHubStop ,             Psm30StateStoppingReconnectTimerOnHubStopInD3Cold },
        { PsmEventTimerFired ,          Psm30StateReturningConnectChangeFromD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForPortChangeEventInD3Cold = {
    // State ETW Name
    Psm30StateEtwWaitingForPortChangeEventInD3Cold,
    // State Entry Function
    HUBPSM30_WaitingForPortChangeEventInD3Cold,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    Psm30SubSmWaitingForPortChangeEventInD3Cold,
    // Event State Pairs for Transitions
    {
        { PsmEventPortConnectChange ,   Psm30StateCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold },
        { PsmEventPortOverCurrentCleared ,Psm30StateReturningOverCurrentClearedFromD3Cold },
        { PsmEventPortOverCurrent ,     Psm30StateReturningOverCurrentFromD3Cold },
        { PsmEventPortResetHubOnError , Psm30StateReturningPortResetHubOnErrorFromD3Cold },
        { PsmEventDeviceRequestResume , Psm30StateStartingTimerForAllowingReconnectOnResumingFromD3Cold },
        { PsmEventHubSuspend ,          Psm30StateReleasingPowerReferenceOnHubS0IdleInD3Cold },
        { PsmEventHubStop ,             Psm30StateReturningHubStopFromSuspendedInD3Cold },
        { PsmEventDeviceRequestDisable ,Psm30StateStartingTimerForAllowingReconnectOnResumingFromD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForReconnectTimerToFlushOnHubStopInD3Cold = {
    // State ETW Name
    Psm30StateEtwWaitingForReconnectTimerToFlushOnHubStopInD3Cold,
    // State Entry Function
    HUBPSM30_WaitingForReconnectTimerToFlushOnHubStopInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTimerFired ,          Psm30StateReturningHubStopFromSuspendedInD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold = {
    // State ETW Name
    Psm30StateEtwWaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold,
    // State Entry Function
    HUBPSM30_WaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStop ,             Psm30StateWaitingForReconnectTimerToFlushOnHubStopInD3Cold },
        { PsmEventTimerFired ,          Psm30StateReturningOverCurrentClearedFromD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForReconnectTimerToFlushOnOvercurrentInD3Cold = {
    // State ETW Name
    Psm30StateEtwWaitingForReconnectTimerToFlushOnOvercurrentInD3Cold,
    // State Entry Function
    HUBPSM30_WaitingForReconnectTimerToFlushOnOvercurrentInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStop ,             Psm30StateWaitingForReconnectTimerToFlushOnHubStopInD3Cold },
        { PsmEventTimerFired ,          Psm30StateReturningOverCurrentFromD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForReconnectTimerToFlushOnReattachAfterD3Cold = {
    // State ETW Name
    Psm30StateEtwWaitingForReconnectTimerToFlushOnReattachAfterD3Cold,
    // State Entry Function
    HUBPSM30_WaitingForReconnectTimerToFlushOnReattachAfterD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTimerFired ,          Psm30StateQueueingEnabledToDSMOnReconnectAfterResumeRequestFromD3Cold },
        { PsmEventHubStop ,             Psm30StateWaitingForReconnectTimerToFlushOnHubStopInD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};



STATE_ENTRY   Psm30StateEntryWaitingForReconnectTimerToFlushOnResetHubErrorInD3Cold = {
    // State ETW Name
    Psm30StateEtwWaitingForReconnectTimerToFlushOnResetHubErrorInD3Cold,
    // State Entry Function
    HUBPSM30_WaitingForReconnectTimerToFlushOnResetHubErrorInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStop ,             Psm30StateWaitingForReconnectTimerToFlushOnHubStopInD3Cold },
        { PsmEventTimerFired ,          Psm30StateReturningPortResetHubOnErrorFromD3Cold },
        { PsmEventNull ,                Psm30StateNull },
    }
};






//
// Global List of States in all the State Machines
//
PSTATE_ENTRY    PSM30StateTable[] = {
    &Psm30StateEntryPoweredOffDisconnected,
    &Psm30StateEntryAcquiringInterruptReferenceOnHSMInDisabled,
    &Psm30StateEntryAcquiringInterruptReferenceOnHSMInEnabled,
    &Psm30StateEntryAcquiringInterruptReferenceOnHSMInEnabledOnWarmReset,
    &Psm30StateEntryAcquiringInterruptReferenceOnHSMOnResetRequest,
    &Psm30StateEntryAcquiringInterruptReferenceOnResuming,
    &Psm30StateEntryAcquiringPowerReferenceAfterDisablingSuperSpeed,
    &Psm30StateEntryAcquiringPowerReferenceonPoweredOffConnectedErrorInS0,
    &Psm30StateEntryAcquiringPowerReferenceonPoweredOffDisconnectedInS0,
    &Psm30StateEntryAcquiringPowerReferenceonPoweredOffSuspendedInS0,
    &Psm30StateEntryAcquiringReferenceInConnectedWithResetNeeded,
    &Psm30StateEntryCancellingResetOnSurpriseRemove,
    &Psm30StateEntryCancellingSetPortPower,
    &Psm30StateEntryCheckingIfADeviceIsPresent,
    &Psm30StateEntryCheckingIfDebounceIsNeeded,
    &Psm30StateEntryCheckingIfLinkStateIsU0,
    &Psm30StateEntryCheckingIfOvercurrentBitOne,
    &Psm30StateEntryCheckingIfPersistentOvercurrent,
    &Psm30StateEntryCheckingIfSuperSpeedShouldBeDisabled,
    &Psm30StateEntryCheckingIsD3ColdIsEnabled,
    &Psm30StateEntryConnectedEnabled,
    &Psm30StateEntryConnectedError,
    &Psm30StateEntryCreatingDevice,
    &Psm30StateEntryCreatingDeviceInConnectedWithResetInProgress,
    &Psm30StateEntryCreatingDeviceInConnectedWithResetNeeded,
    &Psm30StateEntryDetachingDeviceFromPortOnDisconnect,
    &Psm30StateEntryDetachingDeviceFromPortOnDisconnectOnTimedOut,
    &Psm30StateEntryDetachingDeviceFromPortOnDisconnectWithTimer,
    &Psm30StateEntryDetachingDeviceFromPortOnOverCurrent,
    &Psm30StateEntryDetachingDeviceFromPortOnOverCurrentClear,
    &Psm30StateEntryDetachingDeviceFromPortOnOverCurrentClearOnTimedOut,
    &Psm30StateEntryDetachingDeviceFromPortOnOverCurrentClearWithTimer,
    &Psm30StateEntryDetachingDeviceFromPortOnOverCurrentOnTimedOut,
    &Psm30StateEntryDetachingDeviceFromPortOnOverCurrentWithTimer,
    &Psm30StateEntryDetachingDeviceFromPortOnPortCycle,
    &Psm30StateEntryDetachingDeviceOnCleanup,
    &Psm30StateEntryDisablingSuperSpeedOnPortForUnsupportedHubs,
    &Psm30StateEntryDisablingSuperSpeedOnPortOnDeviceRequest,
    &Psm30StateEntryDisconnected,
    &Psm30StateEntryDisconnectedSuperSpeedDisabled,
    &Psm30StateEntryDoesDeviceSupportsReAttach,
    &Psm30StateEntryDoesDeviceSupportsReAttachWithResetInProgress,
    &Psm30StateEntryDoesDeviceSupportsReAttachWithResetNeeded,
    &Psm30StateEntryEnablingSuperSpeedOnStopSuspend,
    &Psm30StateEntryEnablingSuperSpeedOnTimerExpiration,
    &Psm30StateEntryFlushingStaleDeviceEvents,
    &Psm30StateEntryFlushingStaleDeviceEventsWithResetInProgress,
    &Psm30StateEntryFlushingStaleDeviceEventsWithResetNeeded,
    &Psm30StateEntryFlushingUserResetForOverCurrentOnHubStopSuspend,
    &Psm30StateEntryFlushingUserResetForOverCurrentOnOverCurrentClear,
    &Psm30StateEntryGettingPortChangeOnStartResumeFromConnectedError,
    &Psm30StateEntryGettingPortStatusOnResetTimeout,
    &Psm30StateEntryGettingPortStatusOnResumeRequest,
    &Psm30StateEntryGettingPortStatusOnResumeTimeout,
    &Psm30StateEntryInitiatingResetPort,
    &Psm30StateEntryInitiatingResume,
    &Psm30StateEntryInitiatingWarmResetPort,
    &Psm30StateEntryIsDeviceInU0OnResumeRequest,
    &Psm30StateEntryIsDeviceInU0OnResumeTimeout,
    &Psm30StateEntryIsOldDevicePresent,
    &Psm30StateEntryIsOldDevicePresentInConnectedWithResetNeeded,
    &Psm30StateEntryIsOldDevicePresentWithResetInProgress,
    &Psm30StateEntryIssuingAttachDeviceToDsm,
    &Psm30StateEntryIssuingAttachDeviceWithResetToDsm,
    &Psm30StateEntryIssuingAttachDeviceWithResetToDsmWithResetInProgress,
    &Psm30StateEntryIssuingDetachDeviceOnCycleFromPoweredOffDisabled,
    &Psm30StateEntryIssuingDisabledToDSMFromDisabled,
    &Psm30StateEntryIssuingHubReset,
    &Psm30StateEntryIssuingHubResetFromDisconnected,
    &Psm30StateEntryIssuingHubResetOnDisableFailure,
    &Psm30StateEntryIssuingHubResetOnTimedOut,
    &Psm30StateEntryIssuingHubResetWithPendingTimer,
    &Psm30StateEntryIssuingHubResetWithTimer,
    &Psm30StateEntryIssuingPortDisabledToDevice,
    &Psm30StateEntryIssuingPortDisableFailedToDevice,
    &Psm30StateEntryIssuingPortFailedToDSMOnLinkStateError,
    &Psm30StateEntryIssuingPortResetTimedOutToDSM,
    &Psm30StateEntryIssuingReAttachDeviceToBootDevice,
    &Psm30StateEntryIssuingReAttachDeviceToBootDeviceWithResetInProgress,
    &Psm30StateEntryIssuingResetFailedDuetoPendingHubSuspendToDSMInDisabled,
    &Psm30StateEntryIssuingResetFailedDuetoPendingHubSuspendToDSMInEnabled,
    &Psm30StateEntryIssuingResetFailedDuetoPendingHubSuspendToDSMWhileResetInProgress,
    &Psm30StateEntryIssuingResumedToDeviceSM,
    &Psm30StateEntryIssuingSuspendedToDeviceSM,
    &Psm30StateEntryNotifyingUserAboutPersistentOverCurrent,
    &Psm30StateEntryPoweredOffConnectedError,
    &Psm30StateEntryPoweredOffConnectedWithResetNeededAndDeviceDetached,
    &Psm30StateEntryPoweredOffSuspended,
    &Psm30StateEntryQueryingPortStatusOnOverCurrent,
    &Psm30StateEntryQueueingPortFailureToDSMOnTransferFailure,
    &Psm30StateEntryQueueingPortFailureToDSMOnTransferFailureForLpmTimeout,
    &Psm30StateEntryQueueingPortResumeFailedDueToPendingSuspend,
    &Psm30StateEntryQueueingPortResumeTimedOut,
    &Psm30StateEntryQueueingPortTimeoutUpdatedToDSM,
    &Psm30StateEntryQueueStateDisabledToDSMOnStartFromConnectedError,
    &Psm30StateEntryReleasingInterruptReferenceAfterResetFailure,
    &Psm30StateEntryReleasingInterruptReferenceAndStoppingTimer,
    &Psm30StateEntryReleasingInterruptReferenceOnResetTransferFailure,
    &Psm30StateEntryReleasingInterruptReferenceOnResumed,
    &Psm30StateEntryReleasingInterruptReferenceOnSuccessfulReset,
    &Psm30StateEntryReleasingInterruptReferenceOnTransferFailureInResuming,
    &Psm30StateEntryReleasingPortPowerReferenceInSuperSpeedDisabled,
    &Psm30StateEntryReleasingPowerReferenceonPoweredOffConnectedWithResetNeeded,
    &Psm30StateEntryReleasingPowerReferenceonPoweredOffDisabled,
    &Psm30StateEntryReleasingPowerReferenceonPoweredOffDisconnected,
    &Psm30StateEntryReleasingResetReferenceInDisconnected,
    &Psm30StateEntryReleasingResetReferenceInSuperSpeedDisabled,
    &Psm30StateEntryReleasingResetReferenceOnDeviceRemoval,
    &Psm30StateEntryResettingHubAndFlushingUserInitiatedResetOnOverCurrent,
    &Psm30StateEntrySettingPortPowerOnOverCurrent,
    &Psm30StateEntrySettingRemotWakeMask,
    &Psm30StateEntrySettingRemotWakeMaskInDisconected,
    &Psm30StateEntrySettingU1TImeOut,
    &Psm30StateEntrySettingU2TImeOut,
    &Psm30StateEntryStartingAndAcquiringReferenceOnHubStart,
    &Psm30StateEntryStartingAndAcquiringReferenceOnSystemResume,
    &Psm30StateEntryStartingDisableSuperSpeedTimer,
    &Psm30StateEntryStartingOverCurrentTimer,
    &Psm30StateEntryStartingResumeTimer,
    &Psm30StateEntryStoppingDisableSuperSpeedTimer,
    &Psm30StateEntryStoppingResumeTimer,
    &Psm30StateEntryStoppingTimerAndQueueingHubResetInDisconnected,
    &Psm30StateEntryStoppingTimerOnDisconnect,
    &Psm30StateEntryStoppingTimerOnOverCurrent,
    &Psm30StateEntryStoppingTimerOnOverCurrentClear,
    &Psm30StateEntryStoppingTimerOnResetComplete,
    &Psm30StateEntrySuperSpeedDisabled,
    &Psm30StateEntrySuperSpeedDisabledStopped,
    &Psm30StateEntrySuspended,
    &Psm30StateEntrySuspendedDisabled,
    &Psm30StateEntrySuspendedInD3Cold,
    &Psm30StateEntrySuspending,
    &Psm30StateEntrySuspendingPortOnDisableRequest,
    &Psm30StateEntrySuspendingPortOnHubSuspend,
    &Psm30StateEntryWaitingForDebounceTimer,
    &Psm30StateEntryWaitingForHubStopSuspend,
    &Psm30StateEntryWaitingForHubStopSuspendInDisconnected,
    &Psm30StateEntryWaitingForHubStopSuspendOnAttachFailure,
    &Psm30StateEntryWaitingForHubStopSuspendOnAttachWithResetFailure,
    &Psm30StateEntryWaitingForHubStopSuspendOrTimer,
    &Psm30StateEntryWaitingForOldDeviceToDetach,
    &Psm30StateEntryWaitingForOldDeviceToDetachInConnectedWithResetNeeded,
    &Psm30StateEntryWaitingForOldDeviceToDetachWithResetInProgress,
    &Psm30StateEntryWaitingForOverCurrentClear,
    &Psm30StateEntryWaitingForResetComplete,
    &Psm30StateEntryWaitingForResetRequestFromDSMWithResetInPorgress,
    &Psm30StateEntryWaitingForResetTimerToFlush,
    &Psm30StateEntryWaitingForResumeComplete,
    &Psm30StateEntryWaitingForResumeTimerToFlush,
    &Psm30StateEntryWaitingForSuperSpeedDisableTimerToFlush,
    &Psm30StateEntryWaitingForTimerOnResetInDisconnected,
    &Psm30StateEntryWaitingForTimerToFlushOnDisconnect,
    &Psm30StateEntryWaitingForTimerToFlushOnHubStopSuspend,
    &Psm30StateEntryWaitingForTimerToFlushOnHubStopSuspendInDisconnected,
    &Psm30StateEntryWaitingForTimerToFlushOnOverCurrent,
    &Psm30StateEntryWaitingForTimerToFlushOnOverCurrentClear,
    &Psm30StateEntryWaitingForUserResetOnOverCurrent,
    &Psm30StateEntryWaitingToBeDeleted,
    &Psm30StateEntryAckingPortChange,
    &Psm30StateEntryCheckIfThereIsAPortChange,
    &Psm30StateEntryCheckIfThereIsAPortChangeOnGetPortStatus,
    &Psm30StateEntryEnablingInterruptsAndGettingPortEvent,
    &Psm30StateEntryEnablingInterruptsOnFailure,
    &Psm30StateEntryGettingPortStatus,
    &Psm30StateEntryInitializingCumulativePortChangeBits,
    &Psm30StateEntryReturningPortResetHubOnError,
    &Psm30StateEntryAckingPortChangeInNoChange,
    &Psm30StateEntryCheckIfThereIsAPortChangeInNoChange,
    &Psm30StateEntryCheckIfThereIsAPortChangeOnGetPortStatusInNoChange,
    &Psm30StateEntryEnablingInterruptsAndGettingPortEventInNoChange,
    &Psm30StateEntryGettingPortStatusInNoChange,
    &Psm30StateEntryInitializingCumulativePortChangeBitsInNoChange,
    &Psm30StateEntryReturningPortResetHubOnErrorInNoChange,
    &Psm30StateEntryAckingPortChangeOnStartResume,
    &Psm30StateEntryAcquiringPortReferenceAndGettingPortEvent,
    &Psm30StateEntryAcquiringPortReferenceOnFailure,
    &Psm30StateEntryCheckIfThereIsAPortChangeOnStartResume,
    &Psm30StateEntryCheckIfThereIsAPortChangeOnStartResumeOnGetPortStatus,
    &Psm30StateEntryGettingPortStatusOnStartResume,
    &Psm30StateEntryInitializingCumulativePortChangeBitsOnStartResume,
    &Psm30StateEntryReturningPortResetHubOnErrorOnStartResume,
    &Psm30StateEntryGettingPortChange,
    &Psm30StateEntryReturningConnectChangeOnLinkError,
    &Psm30StateEntryReturningPortResetHubOnErrorInPortChangeHandler,
    &Psm30StateEntryWaitingForPortChangeEvent,
    &Psm30StateEntryAcquiringPortReferenceOnTransferFailure,
    &Psm30StateEntryCheckingIfPortIsPoweredOnStart,
    &Psm30StateEntryCheckingIfPowerOnStartHackIsPresent,
    &Psm30StateEntryGettingPortStatusOnStart,
    &Psm30StateEntryGettingPortStatusOnStartBeforePoweringOn,
    &Psm30StateEntryPoweringOnPortOnStart,
    &Psm30StateEntryReturningPortConnectChangeOnLinkErrorOnStart,
    &Psm30StateEntryReturningPortResetHubOnErrorOnStart,
    &Psm30StateEntryWaitingForPortPowerOnTimerOnStart,
    &Psm30StateEntryCheckingIfDeviceIsStillConnectedOnResume,
    &Psm30StateEntryGettingPortStatusOnResume,
    &Psm30StateEntryGettingPortStatusOnResumeAfterWaiting,
    &Psm30StateEntryQueueingStateDisabledOnConnectChangeOnResume,
    &Psm30StateEntryQueueingStateDisabledOnFailure,
    &Psm30StateEntryQueueingStateDisabledOnResetInProgress,
    &Psm30StateEntryQueueingStateEnabledOnResume,
    &Psm30StateEntryQueueingStateSuspendedOnResume,
    &Psm30StateEntryReturningHubStopOnResume,
    &Psm30StateEntryReturningPortConnectChange,
    &Psm30StateEntryReturningPortNeedsResetOnResume,
    &Psm30StateEntryReturningPortOperationSuccessOnResume,
    &Psm30StateEntryReturningPortResetHubOnErrorOnResume,
    &Psm30StateEntryReturningPortResetInProgressOnResume,
    &Psm30StateEntryReturningPortResumedOnResume,
    &Psm30StateEntryStartingTimerForAllowingReconnect,
    &Psm30StateEntryStoppingReconnectTimerOnHubStatusChange,
    &Psm30StateEntryStoppingReconnectTimerOnHubStop,
    &Psm30StateEntryWaitingForDeviceToReconnect,
    &Psm30StateEntryWaitingForReconnectTimerToFlushOnHubStatusChange,
    &Psm30StateEntryWaitingForReconnectTimerToFlushOnHubStop,
    &Psm30StateEntryGettingPortStatusInResuming,
    &Psm30StateEntryReturningConnectChangeOnLinkErrorInResuming,
    &Psm30StateEntryReturningPortResetHubOnErrorInResuming,
    &Psm30StateEntryWaitingForPortChangeEventInResuming,
    &Psm30StateEntryGettingPortStatusInWaitingForStopSuspend,
    &Psm30StateEntryQueueingPortEventFailureToDSM,
    &Psm30StateEntryWaitingForDevicePortEvents,
    &Psm30StateEntryGettingPortStatusInWaitingForStopSuspendInDisconnected,
    &Psm30StateEntryWaitingForDevicePortEventsInDisconnected,
    &Psm30StateEntryCheckingIfLinkIsInU0AfterResetPolling,
    &Psm30StateEntryCheckingIfResetCompletionRequiresPolling,
    &Psm30StateEntryGettingPortStatusInReset,
    &Psm30StateEntryGettingPortStatusInResetAfterPollingInterval,
    &Psm30StateEntryReturningPortResetCompleted,
    &Psm30StateEntryReturningPortResetHubOnErrorInReset,
    &Psm30StateEntryWaitingForPollingIntervalForResetCompletion,
    &Psm30StateEntryWaitingForPortChangeEventInReset,
    &Psm30StateEntryCheckingIfBootDeviceIsConnected,
    &Psm30StateEntryCheckingIfParentHubIsArmedForWake,
    &Psm30StateEntryDisablingRemoteWakeOnPort,
    &Psm30StateEntryEnablingRemoteWakeOnPort,
    &Psm30StateEntryIssuingHubResetOnRemoteWakeMaskFailure,
    &Psm30StateEntryReturningOperationSucessFromArmingForWake,
    &Psm30StateEntryAcquiringPowerReferenceOnHubResumeInD3Cold,
    &Psm30StateEntryCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold,
    &Psm30StateEntryCheckingIfDeviceIsConnectedOnPortChangeInD3Cold,
    &Psm30StateEntryCheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume,
    &Psm30StateEntryPoweredOffOnHubSuspendFromD3Cold,
    &Psm30StateEntryQueueingEnabledToDSMOnReconnectAfterResumeRequestFromD3Cold,
    &Psm30StateEntryQueueingEnabledToDSMOnReconnectFromD3Cold,
    &Psm30StateEntryQueueingStateSuspendedOnHubResumeInD3Cold,
    &Psm30StateEntryReleasingPowerReferenceOnHubS0IdleInD3Cold,
    &Psm30StateEntryReturningConnectChangeFromD3Cold,
    &Psm30StateEntryReturningHubStopFromSuspendedInD3Cold,
    &Psm30StateEntryReturningOverCurrentClearedFromD3Cold,
    &Psm30StateEntryReturningOverCurrentFromD3Cold,
    &Psm30StateEntryReturningPortResetHubOnErrorFromD3Cold,
    &Psm30StateEntryReturningPortResumedFromD3Cold,
    &Psm30StateEntryStartingTimerForAllowingReconnectOnResumingFromD3Cold,
    &Psm30StateEntryStoppingReconnectTimerOnDeviceReAttachAfterD3Cold,
    &Psm30StateEntryStoppingReconnectTimerOnHubStopInD3Cold,
    &Psm30StateEntryStoppingReconnectTimerOnOvercurrentClearedInD3Cold,
    &Psm30StateEntryStoppingReconnectTimerOnOvercurrentInD3Cold,
    &Psm30StateEntryStoppingReconnectTimerOnResetHubErrorInD3Cold,
    &Psm30StateEntryWaitingForDeviceRequestResumeOnD3Cold,
    &Psm30StateEntryWaitingForDeviceToReconnectOnResumeFromD3Cold,
    &Psm30StateEntryWaitingForPortChangeEventInD3Cold,
    &Psm30StateEntryWaitingForReconnectTimerToFlushOnHubStopInD3Cold,
    &Psm30StateEntryWaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold,
    &Psm30StateEntryWaitingForReconnectTimerToFlushOnOvercurrentInD3Cold,
    &Psm30StateEntryWaitingForReconnectTimerToFlushOnReattachAfterD3Cold,
    &Psm30StateEntryWaitingForReconnectTimerToFlushOnResetHubErrorInD3Cold,
};
