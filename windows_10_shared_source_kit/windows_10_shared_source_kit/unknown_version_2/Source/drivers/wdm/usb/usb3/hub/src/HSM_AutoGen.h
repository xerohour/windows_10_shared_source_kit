
/*++
Copyright (c) Microsoft Corporation

Module Name:

HSM_AutoGen.h

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/

#pragma once



//
// Enum of States in all the State Machines
//
typedef enum _HSM_STATE {
    HsmStateIgnored                                  = 1000,
    HsmStateEmptySubState                            = 1001,
    HsmStateNull                                     = 1002,
    HsmStateCompleteWithStatusFailed                 = 1003,
    HsmStateRequestPortCycle                         = 1004,
    HsmStateSignalQueryDeviceTextEvent               = 1005,
    HsmStateCompleteFDORequestWithStatusFailed       = 1006,
    HsmStateCompleteGetPortStatusWithInternalError   = 1007,
    HsmStateCompleteGetPortStatusWithNoSuchDevice    = 1008,
    HsmStateCompletePdoPreStart                      = 1009,
    HsmStateSignalPnpPowerEvent                      = 1010,
    HsmStateWaitingForAddDevice                      = 2000,
    HsmStateAcquiringWdfPowerReferenceInitialD0Entry = 2001,
    HsmStateAcquiringWdfPowerReferenceOnInterrupt    = 2002,
    HsmStateAcquiringWdfPowerReferenceOnInterruptFailureAndCheckingIfRootHub = 2003,
    HsmStateAcquiringWdfPowerReferenceOnReset        = 2004,
    HsmStateAcquiringWdfPowerReferenceOnResumeInSX   = 2005,
    HsmStateAcquiringWdfReferenceOnResumeInS0        = 2006,
    HsmStateBugcheckingSystemOnExcessiveResetsForHubInBootPath = 2007,
    HsmStateCancellingInterruptTransferOnReset       = 2008,
    HsmStateCancellingInterruptTransferOnStop        = 2009,
    HsmStateCancellingInterruptTransferOnSuspend     = 2010,
    HsmStateCheckIfThereIsAValidHubChange            = 2011,
    HsmStateCheckingForUnexpectedPowerLoss           = 2012,
    HsmStateCheckingIfHubIsInBootPath                = 2013,
    HsmStateCheckingIfResetByParent                  = 2014,
    HsmStateCheckingIfResetByParentWithResetPending  = 2015,
    HsmStateCheckingIfResetRecoveryHasBeenInvokedTooManyTimes = 2016,
    HsmStateCheckingPSMInterruptReferencesOnReset    = 2017,
    HsmStateCheckingPSMInterruptReferencesOnResetInNoInterrupt = 2018,
    HsmStateCheckingPSMInterruptReferencesOnResetInNoInterruptOnFatalError = 2019,
    HsmStateCheckingPSMInterruptReferencesOnStop     = 2020,
    HsmStateCheckingPSMInterruptReferencesOnStopInNoInterrupt = 2021,
    HsmStateCheckingPSMInterruptReferencesOnSuspend  = 2022,
    HsmStateCheckingPSMInterruptReferencesOnSxInNoInterrupt = 2023,
    HsmStateCheckOverCurrentBit                      = 2024,
    HsmStateCompletingPDORequestForGetPortStatus     = 2025,
    HsmStateCompletingPDORequestForGetPortStatusFromConfigWithIntTransfer = 2026,
    HsmStateCompletingPDORequestForGetPortStatusFromConfigWithIntTransferWithNoSuchDevice = 2027,
    HsmStateCompletingPDORequestForGetPortStatusInSuspended = 2028,
    HsmStateCompletingPDORequestForGetPortStatusWithNoSuchDevice = 2029,
    HsmStateConfiguredWithIntTransfer                = 2030,
    HsmStateConfiguredWithoutInterruptTransfer       = 2031,
    HsmStateConfiguring                              = 2032,
    HsmStateConfiguringHubAfterReset                 = 2033,
    HsmStateConfiguringHubAfterResetOnSystemResume   = 2034,
    HsmStateCreatingChildPSMs                        = 2035,
    HsmStateErrorOnWaitingForOvercurrentClear        = 2036,
    HsmStateGettingAdditionalConfigInfo              = 2037,
    HsmStateGettingHubPortStatus                     = 2038,
    HsmStateGettingHubPortStatusForLostChanges       = 2039,
    HsmStateGettingOverCurrentBitStatus              = 2040,
    HsmStateGettingPortStatusOnPDORequest            = 2041,
    HsmStateGettingPortStatusOnPDORequestFromConfigWithIntTransfer = 2042,
    HsmStateHasResetCountReachedMaximum              = 2043,
    HsmStateHasResetPipeCountReachedMaximum          = 2044,
    HsmStateHubStatusError                           = 2045,
    HsmStateInitializingResetCount                   = 2046,
    HsmStateIsItHubChange                            = 2047,
    HsmStateLoggingHubWasReset                       = 2048,
    HsmStateNotifyingHubResetToPortsOnReset          = 2049,
    HsmStateNotifyingHubResetToPortsOnResetOnFatalError = 2050,
    HsmStateNotifyingHubResetToPortsOnResetOnResume  = 2051,
    HsmStateNotifyingHubResumeInS0ToDevices          = 2052,
    HsmStateNotifyingHubResumeToDevices              = 2053,
    HsmStateNotifyingHubResumeWithResetToDevices     = 2054,
    HsmStateNotifyingHubResumeWithResetToDevicesOnFailure = 2055,
    HsmStateNotifyingHubStopToDevices                = 2056,
    HsmStateNotifyingHubStopToDevicesOnSuspriseRemove = 2057,
    HsmStateNotifyingHubStopToPorts                  = 2058,
    HsmStateNotifyingHubStopToPortsOnFatalError      = 2059,
    HsmStateNotifyingHubStopToPortsOnReset           = 2060,
    HsmStateQueryingForHubHackFlags                  = 2061,
    HsmStateQueueingFakeStatusChangeOnResumeInS0     = 2062,
    HsmStateQueueingHubStatusForLostChanges          = 2063,
    HsmStateQueueingPortStatusChangeEvents           = 2064,
    HsmStateQueuingHubChange                         = 2065,
    HsmStateReConfiguring                            = 2066,
    HsmStateReportingErrorToPnp                      = 2067,
    HsmStateReportingReStartFailure                  = 2068,
    HsmStateReportingStartFailure                    = 2069,
    HsmStateResettingHub                             = 2070,
    HsmStateResettingHubOnResume                     = 2071,
    HsmStateResettingInterruptPipeOnFailure          = 2072,
    HsmStateSendingAckForHubChange                   = 2073,
    HsmStateSendingInterruptTransfer                 = 2074,
    HsmStateSendingInterruptTransferAfterResettingPipe = 2075,
    HsmStateSignalingPnPPowerEventOnPendingStop      = 2076,
    HsmStateSignalingPnPPowerEventOnStart            = 2077,
    HsmStateSignalingPnPPowerEventOnStop             = 2078,
    HsmStateSignallingPnpPowerEventForSuspended      = 2079,
    HsmStateSignallingPnpPowerEventInUnconfigured    = 2080,
    HsmStateStartingTimerForResetRetry               = 2081,
    HsmStateStopped                                  = 2082,
    HsmStateSupriseRemoved                           = 2083,
    HsmStateSuspended                                = 2084,
    HsmStateSuspendedInRecycle                       = 2085,
    HsmStateSuspendedWithPendingReset                = 2086,
    HsmStateUnblockingResumeOnHubResetFailure        = 2087,
    HsmStateUnblockResumeWaitingForStop              = 2088,
    HsmStateUnconfigured                             = 2089,
    HsmStateWaitingForD0Entry                        = 2090,
    HsmStateWaitingForDevicesToAcquireReferenceOnStart = 2091,
    HsmStateWaitingForDevicesToReleaseReferenceOnD0ExitFinalFromReset = 2092,
    HsmStateWaitingForDevicesToReleaseReferenceOnSuspend = 2093,
    HsmStateWaitingForEnableInterruptsOnReset        = 2094,
    HsmStateWaitingForEnableInterruptsOnStop         = 2095,
    HsmStateWaitingForEnableInterruptsOnSx           = 2096,
    HsmStateWaitingForInterruptReferencesReleasedOnFatalError = 2097,
    HsmStateWaitingForInterruptReferencesReleasedOnReset = 2098,
    HsmStateWaitingForInterruptReferencesReleasedOnStop = 2099,
    HsmStateWaitingForInterruptReferencesReleasedOnSuspend = 2100,
    HsmStateWaitingForOvercurrentToClear             = 2101,
    HsmStateWaitingForPortsToAcquireReferenceOnResume = 2102,
    HsmStateWaitingForPortsToAcquireReferenceOnResumeInS0 = 2103,
    HsmStateWaitingForPortsToAcquireReferenceOnResumeWithReset = 2104,
    HsmStateWaitingForPortsToAcquireReferenceOnStart = 2105,
    HsmStateWaitingForPortsToAcquireReferencePostReset = 2106,
    HsmStateWaitingForPortsToReleaseReferenceOnReset = 2107,
    HsmStateWaitingForPortsToReleaseReferenceOnResetOnResume = 2108,
    HsmStateWaitingForPortsToReleaseReferenceOnStop  = 2109,
    HsmStateWaitingForPortsToReleaseReferenceOnSuspend = 2110,
    HsmStateWaitingForPSMInterruptReferencesOnReset  = 2111,
    HsmStateWaitingForPSMInterruptReferencesOnStop   = 2112,
    HsmStateWaitingForPSMInterruptReferencesOnSuspend = 2113,
    HsmStateWaitingForReleaseHardware                = 2114,
    HsmStateWaitingForReleaseHardwareOnReStartFailure = 2115,
    HsmStateWaitingForReleaseHardwareOnStartFailure  = 2116,
    HsmStateWaitingForResetRetryTimer                = 2117,
    HsmStateWaitingForStop                           = 2118,
    HsmStateWaitingToBeDeleted                       = 2119,
    HsmStateCheckingBytesReturnedInHubConfigDescriptor = 2120,
    HsmStateCheckingIfHubDescriptorRetryCountExceededMax = 2121,
    HsmStateConfiguringAfterSetConfig                = 2122,
    HsmStateGettingHubConfigurationDescriptorWithDefaultLength = 2123,
    HsmStateGettingHubConfigurationDescriptorWithReturnedLength = 2124,
    HsmStateGettingHubDescriptor                     = 2125,
    HsmStateGettingHubStatus                         = 2126,
    HsmStateInitializingHubDescriptorRetryCount      = 2127,
    HsmStateReturningConfigurationFailure            = 2128,
    HsmStateReturningConfigurationSuccess            = 2129,
    HsmStateSettingHubConfiguration                  = 2130,
    HsmStateUpdatingHubInfoInUCX                     = 2131,
    HsmStateValidatingAndParsingHubDescriptor        = 2132,
    HsmStateValidatingAndParsingHubPowerStatus       = 2133,
    HsmStateValidatingHubConfigurationDescriptor     = 2134,
    HsmStateWaitingForHubDescriptorRetryTimer        = 2135,
    HsmStateGettingRootHub20PortsInfo                = 2136,
    HsmStateGettingRootHub30PortsInfo                = 2137,
    HsmStateGettingRootHubInfo                       = 2138,
    HsmStateReturningConfigurationFailureForRootHub  = 2139,
    HsmStateReturningConfigurationSuccessForRootHub  = 2140,
    HsmStateGettingAdditionalInfoFromParent          = 2141,
    HsmStateReturningOperationFailure                = 2142,
    HsmStateReturningOperationSuccess                = 2143,
    HsmStateGettingHubDescriptorToCheckForPowerLoss  = 2144,
    HsmStateReturningOperationFailureInCheckingForPowerLoss = 2145,
    HsmStateReturningOperationSuccessInCheckingForPowerLoss = 2146,
    HsmStateReturningOperationFailureInSettingHubDepth = 2147,
    HsmStateReturningOperationSuccessInSettingHubDepth = 2148,
    HsmStateSettingHubDepth                          = 2149,
} HSM_STATE, *PHSM_STATE;





//
// Enum of ETW state names
//
typedef enum _HSM_STATE_ETW {
    HsmStateEtwWaitingForAddDevice                   = 2000,
    HsmStateEtwAcquiringWdfPowerReferenceInitialD0Entry = 2001,
    HsmStateEtwAcquiringWdfPowerReferenceOnInterrupt = 2002,
    HsmStateEtwAcquiringWdfPowerReferenceOnInterruptFailureAndCheckingIfRootHub = 2147,
    HsmStateEtwAcquiringWdfPowerReferenceOnReset     = 2004,
    HsmStateEtwAcquiringWdfPowerReferenceOnResumeInSX = 2005,
    HsmStateEtwAcquiringWdfReferenceOnResumeInS0     = 2006,
    HsmStateEtwBugcheckingSystemOnExcessiveResetsForHubInBootPath = 2007,
    HsmStateEtwCancellingInterruptTransferOnReset    = 2008,
    HsmStateEtwCancellingInterruptTransferOnStop     = 2009,
    HsmStateEtwCancellingInterruptTransferOnSuspend  = 2010,
    HsmStateEtwCheckIfThereIsAValidHubChange         = 2011,
    HsmStateEtwCheckingForUnexpectedPowerLoss        = 2012,
    HsmStateEtwCheckingIfHubIsInBootPath             = 2013,
    HsmStateEtwCheckingIfResetByParent               = 2014,
    HsmStateEtwCheckingIfResetByParentWithResetPending = 2015,
    HsmStateEtwCheckingIfResetRecoveryHasBeenInvokedTooManyTimes = 2016,
    HsmStateEtwCheckingPSMInterruptReferencesOnReset = 2017,
    HsmStateEtwCheckingPSMInterruptReferencesOnResetInNoInterrupt = 2018,
    HsmStateEtwCheckingPSMInterruptReferencesOnResetInNoInterruptOnFatalError = 2019,
    HsmStateEtwCheckingPSMInterruptReferencesOnStop  = 2020,
    HsmStateEtwCheckingPSMInterruptReferencesOnStopInNoInterrupt = 2021,
    HsmStateEtwCheckingPSMInterruptReferencesOnSuspend = 2022,
    HsmStateEtwCheckingPSMInterruptReferencesOnSxInNoInterrupt = 2023,
    HsmStateEtwCheckOverCurrentBit                   = 2024,
    HsmStateEtwCompletingPDORequestForGetPortStatus  = 2025,
    HsmStateEtwCompletingPDORequestForGetPortStatusFromConfigWithIntTransfer = 2026,
    HsmStateEtwCompletingPDORequestForGetPortStatusFromConfigWithIntTransferWithNoSuchDevice = 2027,
    HsmStateEtwCompletingPDORequestForGetPortStatusInSuspended = 2028,
    HsmStateEtwCompletingPDORequestForGetPortStatusWithNoSuchDevice = 2029,
    HsmStateEtwConfiguredWithIntTransfer             = 2030,
    HsmStateEtwConfiguredWithoutInterruptTransfer    = 2031,
    HsmStateEtwConfiguring                           = 2032,
    HsmStateEtwConfiguringHubAfterReset              = 2033,
    HsmStateEtwConfiguringHubAfterResetOnSystemResume = 2034,
    HsmStateEtwCreatingChildPSMs                     = 2035,
    HsmStateEtwErrorOnWaitingForOvercurrentClear     = 2036,
    HsmStateEtwGettingAdditionalConfigInfo           = 2037,
    HsmStateEtwGettingHubPortStatus                  = 2038,
    HsmStateEtwGettingHubPortStatusForLostChanges    = 2039,
    HsmStateEtwGettingOverCurrentBitStatus           = 2040,
    HsmStateEtwGettingPortStatusOnPDORequest         = 2041,
    HsmStateEtwGettingPortStatusOnPDORequestFromConfigWithIntTransfer = 2042,
    HsmStateEtwHasResetCountReachedMaximum           = 2043,
    HsmStateEtwHasResetPipeCountReachedMaximum       = 2148,
    HsmStateEtwHubStatusError                        = 2044,
    HsmStateEtwInitializingResetCount                = 2045,
    HsmStateEtwIsItHubChange                         = 2046,
    HsmStateEtwLoggingHubWasReset                    = 2047,
    HsmStateEtwNotifyingHubResetToPortsOnReset       = 2048,
    HsmStateEtwNotifyingHubResetToPortsOnResetOnFatalError = 2049,
    HsmStateEtwNotifyingHubResetToPortsOnResetOnResume = 2050,
    HsmStateEtwNotifyingHubResumeInS0ToDevices       = 2051,
    HsmStateEtwNotifyingHubResumeToDevices           = 2052,
    HsmStateEtwNotifyingHubResumeWithResetToDevices  = 2053,
    HsmStateEtwNotifyingHubResumeWithResetToDevicesOnFailure = 2054,
    HsmStateEtwNotifyingHubStopToDevices             = 2055,
    HsmStateEtwNotifyingHubStopToDevicesOnSuspriseRemove = 2056,
    HsmStateEtwNotifyingHubStopToPorts               = 2057,
    HsmStateEtwNotifyingHubStopToPortsOnFatalError   = 2058,
    HsmStateEtwNotifyingHubStopToPortsOnReset        = 2059,
    HsmStateEtwQueryingForHubHackFlags               = 2060,
    HsmStateEtwQueueingFakeStatusChangeOnResumeInS0  = 2061,
    HsmStateEtwQueueingHubStatusForLostChanges       = 2062,
    HsmStateEtwQueueingPortStatusChangeEvents        = 2063,
    HsmStateEtwQueuingHubChange                      = 2064,
    HsmStateEtwReConfiguring                         = 2065,
    HsmStateEtwReportingErrorToPnp                   = 2066,
    HsmStateEtwReportingReStartFailure               = 2067,
    HsmStateEtwReportingStartFailure                 = 2068,
    HsmStateEtwResettingHub                          = 2069,
    HsmStateEtwResettingHubOnResume                  = 2070,
    HsmStateEtwResettingInterruptPipeOnFailure       = 2149,
    HsmStateEtwSendingAckForHubChange                = 2071,
    HsmStateEtwSendingInterruptTransfer              = 2072,
    HsmStateEtwSendingInterruptTransferAfterResettingPipe = 2150,
    HsmStateEtwSignalingPnPPowerEventOnPendingStop   = 2073,
    HsmStateEtwSignalingPnPPowerEventOnStart         = 2074,
    HsmStateEtwSignalingPnPPowerEventOnStop          = 2075,
    HsmStateEtwSignallingPnpPowerEventForSuspended   = 2076,
    HsmStateEtwSignallingPnpPowerEventInUnconfigured = 2077,
    HsmStateEtwStartingTimerForResetRetry            = 2078,
    HsmStateEtwStopped                               = 2079,
    HsmStateEtwSupriseRemoved                        = 2080,
    HsmStateEtwSuspended                             = 2081,
    HsmStateEtwSuspendedInRecycle                    = 2082,
    HsmStateEtwSuspendedWithPendingReset             = 2083,
    HsmStateEtwUnblockingResumeOnHubResetFailure     = 2084,
    HsmStateEtwUnblockResumeWaitingForStop           = 2085,
    HsmStateEtwUnconfigured                          = 2086,
    HsmStateEtwWaitingForD0Entry                     = 2087,
    HsmStateEtwWaitingForDevicesToAcquireReferenceOnStart = 2088,
    HsmStateEtwWaitingForDevicesToReleaseReferenceOnD0ExitFinalFromReset = 2089,
    HsmStateEtwWaitingForDevicesToReleaseReferenceOnSuspend = 2090,
    HsmStateEtwWaitingForEnableInterruptsOnReset     = 2091,
    HsmStateEtwWaitingForEnableInterruptsOnStop      = 2092,
    HsmStateEtwWaitingForEnableInterruptsOnSx        = 2093,
    HsmStateEtwWaitingForInterruptReferencesReleasedOnFatalError = 2094,
    HsmStateEtwWaitingForInterruptReferencesReleasedOnReset = 2095,
    HsmStateEtwWaitingForInterruptReferencesReleasedOnStop = 2096,
    HsmStateEtwWaitingForInterruptReferencesReleasedOnSuspend = 2097,
    HsmStateEtwWaitingForOvercurrentToClear          = 2098,
    HsmStateEtwWaitingForPortsToAcquireReferenceOnResume = 2099,
    HsmStateEtwWaitingForPortsToAcquireReferenceOnResumeInS0 = 2100,
    HsmStateEtwWaitingForPortsToAcquireReferenceOnResumeWithReset = 2101,
    HsmStateEtwWaitingForPortsToAcquireReferenceOnStart = 2102,
    HsmStateEtwWaitingForPortsToAcquireReferencePostReset = 2103,
    HsmStateEtwWaitingForPortsToReleaseReferenceOnReset = 2104,
    HsmStateEtwWaitingForPortsToReleaseReferenceOnResetOnResume = 2105,
    HsmStateEtwWaitingForPortsToReleaseReferenceOnStop = 2106,
    HsmStateEtwWaitingForPortsToReleaseReferenceOnSuspend = 2107,
    HsmStateEtwWaitingForPSMInterruptReferencesOnReset = 2108,
    HsmStateEtwWaitingForPSMInterruptReferencesOnStop = 2109,
    HsmStateEtwWaitingForPSMInterruptReferencesOnSuspend = 2110,
    HsmStateEtwWaitingForReleaseHardware             = 2111,
    HsmStateEtwWaitingForReleaseHardwareOnReStartFailure = 2112,
    HsmStateEtwWaitingForReleaseHardwareOnStartFailure = 2113,
    HsmStateEtwWaitingForResetRetryTimer             = 2114,
    HsmStateEtwWaitingForStop                        = 2115,
    HsmStateEtwWaitingToBeDeleted                    = 2116,
    HsmStateEtwCheckingBytesReturnedInHubConfigDescriptor = 2117,
    HsmStateEtwCheckingIfHubDescriptorRetryCountExceededMax = 2118,
    HsmStateEtwConfiguringAfterSetConfig             = 2119,
    HsmStateEtwGettingHubConfigurationDescriptorWithDefaultLength = 2120,
    HsmStateEtwGettingHubConfigurationDescriptorWithReturnedLength = 2121,
    HsmStateEtwGettingHubDescriptor                  = 2122,
    HsmStateEtwGettingHubStatus                      = 2123,
    HsmStateEtwInitializingHubDescriptorRetryCount   = 2124,
    HsmStateEtwReturningConfigurationFailure         = 2125,
    HsmStateEtwReturningConfigurationSuccess         = 2126,
    HsmStateEtwSettingHubConfiguration               = 2127,
    HsmStateEtwUpdatingHubInfoInUCX                  = 2128,
    HsmStateEtwValidatingAndParsingHubDescriptor     = 2129,
    HsmStateEtwValidatingAndParsingHubPowerStatus    = 2130,
    HsmStateEtwValidatingHubConfigurationDescriptor  = 2131,
    HsmStateEtwWaitingForHubDescriptorRetryTimer     = 2132,
    HsmStateEtwGettingRootHub20PortsInfo             = 2133,
    HsmStateEtwGettingRootHub30PortsInfo             = 2134,
    HsmStateEtwGettingRootHubInfo                    = 2135,
    HsmStateEtwReturningConfigurationFailureForRootHub = 2136,
    HsmStateEtwReturningConfigurationSuccessForRootHub = 2137,
    HsmStateEtwGettingAdditionalInfoFromParent       = 2138,
    HsmStateEtwReturningOperationFailure             = 2139,
    HsmStateEtwReturningOperationSuccess             = 2140,
    HsmStateEtwGettingHubDescriptorToCheckForPowerLoss = 2141,
    HsmStateEtwReturningOperationFailureInCheckingForPowerLoss = 2142,
    HsmStateEtwReturningOperationSuccessInCheckingForPowerLoss = 2143,
    HsmStateEtwReturningOperationFailureInSettingHubDepth = 2144,
    HsmStateEtwReturningOperationSuccessInSettingHubDepth = 2145,
    HsmStateEtwSettingHubDepth                       = 2146,
} HSM_STATE_ETW, *PHSM_STATE_ETW;



//
// Definitions for State Entry Functions 
//
STATE_ENTRY_FUNCTION          HUBHSM_AcquiringWdfPowerReferenceInitialD0Entry;

STATE_ENTRY_FUNCTION          HUBHSM_AcquiringWdfPowerReferenceOnInterrupt;

STATE_ENTRY_FUNCTION          HUBHSM_AcquiringWdfPowerReferenceOnInterruptFailureAndCheckingIfRootHub;

STATE_ENTRY_FUNCTION          HUBHSM_AcquiringWdfPowerReferenceOnReset;

STATE_ENTRY_FUNCTION          HUBHSM_AcquiringWdfPowerReferenceOnResumeInSX;

STATE_ENTRY_FUNCTION          HUBHSM_AcquiringWdfReferenceOnResumeInS0;

STATE_ENTRY_FUNCTION          HUBHSM_BugcheckingSystemOnExcessiveResetsForHubInBootPath;

STATE_ENTRY_FUNCTION          HUBHSM_CancellingInterruptTransferOnReset;

STATE_ENTRY_FUNCTION          HUBHSM_CancellingInterruptTransferOnStop;

STATE_ENTRY_FUNCTION          HUBHSM_CancellingInterruptTransferOnSuspend;

STATE_ENTRY_FUNCTION          HUBHSM_CheckIfThereIsAValidHubChange;

STATE_ENTRY_FUNCTION          HUBHSM_CheckingForUnexpectedPowerLoss;

STATE_ENTRY_FUNCTION          HUBHSM_CheckingIfHubIsInBootPath;

STATE_ENTRY_FUNCTION          HUBHSM_CheckingIfResetByParent;

STATE_ENTRY_FUNCTION          HUBHSM_CheckingIfResetByParentWithResetPending;

STATE_ENTRY_FUNCTION          HUBHSM_CheckingIfResetRecoveryHasBeenInvokedTooManyTimes;

STATE_ENTRY_FUNCTION          HUBHSM_CheckingPSMInterruptReferencesOnReset;

STATE_ENTRY_FUNCTION          HUBHSM_CheckingPSMInterruptReferencesOnResetInNoInterrupt;

STATE_ENTRY_FUNCTION          HUBHSM_CheckingPSMInterruptReferencesOnResetInNoInterruptOnFatalError;

STATE_ENTRY_FUNCTION          HUBHSM_CheckingPSMInterruptReferencesOnStop;

STATE_ENTRY_FUNCTION          HUBHSM_CheckingPSMInterruptReferencesOnStopInNoInterrupt;

STATE_ENTRY_FUNCTION          HUBHSM_CheckingPSMInterruptReferencesOnSuspend;

STATE_ENTRY_FUNCTION          HUBHSM_CheckingPSMInterruptReferencesOnSxInNoInterrupt;

STATE_ENTRY_FUNCTION          HUBHSM_CheckOverCurrentBit;

STATE_ENTRY_FUNCTION          HUBHSM_CompletingPDORequestForGetPortStatus;

STATE_ENTRY_FUNCTION          HUBHSM_CompletingPDORequestForGetPortStatusFromConfigWithIntTransfer;

STATE_ENTRY_FUNCTION          HUBHSM_CompletingPDORequestForGetPortStatusFromConfigWithIntTransferWithNoSuchDevice;

STATE_ENTRY_FUNCTION          HUBHSM_CompletingPDORequestForGetPortStatusInSuspended;

STATE_ENTRY_FUNCTION          HUBHSM_CompletingPDORequestForGetPortStatusWithNoSuchDevice;

STATE_ENTRY_FUNCTION          HUBHSM_ConfiguredWithIntTransfer;

STATE_ENTRY_FUNCTION          HUBHSM_ConfiguredWithoutInterruptTransfer;

STATE_ENTRY_FUNCTION          HUBHSM_Configuring;

STATE_ENTRY_FUNCTION          HUBHSM_ConfiguringHubAfterReset;

STATE_ENTRY_FUNCTION          HUBHSM_ConfiguringHubAfterResetOnSystemResume;

STATE_ENTRY_FUNCTION          HUBHSM_CreatingChildPSMs;

STATE_ENTRY_FUNCTION          HUBHSM_ErrorOnWaitingForOvercurrentClear;

STATE_ENTRY_FUNCTION          HUBHSM_GettingAdditionalConfigInfo;

STATE_ENTRY_FUNCTION          HUBHSM_GettingHubPortStatus;

STATE_ENTRY_FUNCTION          HUBHSM_GettingHubPortStatusForLostChanges;

STATE_ENTRY_FUNCTION          HUBHSM_GettingOverCurrentBitStatus;

STATE_ENTRY_FUNCTION          HUBHSM_GettingPortStatusOnPDORequest;

STATE_ENTRY_FUNCTION          HUBHSM_GettingPortStatusOnPDORequestFromConfigWithIntTransfer;

STATE_ENTRY_FUNCTION          HUBHSM_HasResetCountReachedMaximum;

STATE_ENTRY_FUNCTION          HUBHSM_HasResetPipeCountReachedMaximum;

STATE_ENTRY_FUNCTION          HUBHSM_HubStatusError;

STATE_ENTRY_FUNCTION          HUBHSM_InitializingResetCount;

STATE_ENTRY_FUNCTION          HUBHSM_IsItHubChange;

STATE_ENTRY_FUNCTION          HUBHSM_LoggingHubWasReset;

STATE_ENTRY_FUNCTION          HUBHSM_NotifyingHubResetToPortsOnReset;

STATE_ENTRY_FUNCTION          HUBHSM_NotifyingHubResetToPortsOnResetOnFatalError;

STATE_ENTRY_FUNCTION          HUBHSM_NotifyingHubResetToPortsOnResetOnResume;

STATE_ENTRY_FUNCTION          HUBHSM_NotifyingHubResumeInS0ToDevices;

STATE_ENTRY_FUNCTION          HUBHSM_NotifyingHubResumeToDevices;

STATE_ENTRY_FUNCTION          HUBHSM_NotifyingHubResumeWithResetToDevices;

STATE_ENTRY_FUNCTION          HUBHSM_NotifyingHubResumeWithResetToDevicesOnFailure;

STATE_ENTRY_FUNCTION          HUBHSM_NotifyingHubStopToDevices;

STATE_ENTRY_FUNCTION          HUBHSM_NotifyingHubStopToDevicesOnSuspriseRemove;

STATE_ENTRY_FUNCTION          HUBHSM_NotifyingHubStopToPorts;

STATE_ENTRY_FUNCTION          HUBHSM_NotifyingHubStopToPortsOnFatalError;

STATE_ENTRY_FUNCTION          HUBHSM_NotifyingHubStopToPortsOnReset;

STATE_ENTRY_FUNCTION          HUBHSM_QueryingForHubHackFlags;

STATE_ENTRY_FUNCTION          HUBHSM_QueueingFakeStatusChangeOnResumeInS0;

STATE_ENTRY_FUNCTION          HUBHSM_QueueingHubStatusForLostChanges;

STATE_ENTRY_FUNCTION          HUBHSM_QueueingPortStatusChangeEvents;

STATE_ENTRY_FUNCTION          HUBHSM_QueuingHubChange;

STATE_ENTRY_FUNCTION          HUBHSM_ReConfiguring;

STATE_ENTRY_FUNCTION          HUBHSM_ReportingErrorToPnp;

STATE_ENTRY_FUNCTION          HUBHSM_ReportingReStartFailure;

STATE_ENTRY_FUNCTION          HUBHSM_ReportingStartFailure;

STATE_ENTRY_FUNCTION          HUBHSM_ResettingHub;

STATE_ENTRY_FUNCTION          HUBHSM_ResettingHubOnResume;

STATE_ENTRY_FUNCTION          HUBHSM_ResettingInterruptPipeOnFailure;

STATE_ENTRY_FUNCTION          HUBHSM_SendingAckForHubChange;

STATE_ENTRY_FUNCTION          HUBHSM_SendingInterruptTransfer;

STATE_ENTRY_FUNCTION          HUBHSM_SendingInterruptTransferAfterResettingPipe;

STATE_ENTRY_FUNCTION          HUBHSM_SignalingPnPPowerEventOnPendingStop;

STATE_ENTRY_FUNCTION          HUBHSM_SignalingPnPPowerEventOnStart;

STATE_ENTRY_FUNCTION          HUBHSM_SignalingPnPPowerEventOnStop;

STATE_ENTRY_FUNCTION          HUBHSM_SignallingPnpPowerEventForSuspended;

STATE_ENTRY_FUNCTION          HUBHSM_SignallingPnpPowerEventInUnconfigured;

STATE_ENTRY_FUNCTION          HUBHSM_StartingTimerForResetRetry;

STATE_ENTRY_FUNCTION          HUBHSM_Stopped;

STATE_ENTRY_FUNCTION          HUBHSM_SupriseRemoved;

STATE_ENTRY_FUNCTION          HUBHSM_Suspended;

STATE_ENTRY_FUNCTION          HUBHSM_SuspendedInRecycle;

STATE_ENTRY_FUNCTION          HUBHSM_SuspendedWithPendingReset;

STATE_ENTRY_FUNCTION          HUBHSM_UnblockingResumeOnHubResetFailure;

STATE_ENTRY_FUNCTION          HUBHSM_UnblockResumeWaitingForStop;

STATE_ENTRY_FUNCTION          HUBHSM_Unconfigured;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForAddDevice;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForD0Entry;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForDevicesToAcquireReferenceOnStart;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForDevicesToReleaseReferenceOnD0ExitFinalFromReset;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForDevicesToReleaseReferenceOnSuspend;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForEnableInterruptsOnReset;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForEnableInterruptsOnStop;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForEnableInterruptsOnSx;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForInterruptReferencesReleasedOnFatalError;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForInterruptReferencesReleasedOnReset;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForInterruptReferencesReleasedOnStop;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForInterruptReferencesReleasedOnSuspend;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForOvercurrentToClear;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForPortsToAcquireReferenceOnResume;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForPortsToAcquireReferenceOnResumeInS0;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForPortsToAcquireReferenceOnResumeWithReset;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForPortsToAcquireReferenceOnStart;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForPortsToAcquireReferencePostReset;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForPortsToReleaseReferenceOnReset;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForPortsToReleaseReferenceOnResetOnResume;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForPortsToReleaseReferenceOnStop;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForPortsToReleaseReferenceOnSuspend;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForPSMInterruptReferencesOnReset;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForPSMInterruptReferencesOnStop;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForPSMInterruptReferencesOnSuspend;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForReleaseHardware;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForReleaseHardwareOnReStartFailure;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForReleaseHardwareOnStartFailure;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForResetRetryTimer;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForStop;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingToBeDeleted;

STATE_ENTRY_FUNCTION          HUBHSM_CheckingBytesReturnedInHubConfigDescriptor;

STATE_ENTRY_FUNCTION          HUBHSM_CheckingIfHubDescriptorRetryCountExceededMax;

STATE_ENTRY_FUNCTION          HUBHSM_ConfiguringAfterSetConfig;

STATE_ENTRY_FUNCTION          HUBHSM_GettingHubConfigurationDescriptorWithDefaultLength;

STATE_ENTRY_FUNCTION          HUBHSM_GettingHubConfigurationDescriptorWithReturnedLength;

STATE_ENTRY_FUNCTION          HUBHSM_GettingHubDescriptor;

STATE_ENTRY_FUNCTION          HUBHSM_GettingHubStatus;

STATE_ENTRY_FUNCTION          HUBHSM_InitializingHubDescriptorRetryCount;

STATE_ENTRY_FUNCTION          HUBHSM_ReturningConfigurationFailure;

STATE_ENTRY_FUNCTION          HUBHSM_ReturningConfigurationSuccess;

STATE_ENTRY_FUNCTION          HUBHSM_SettingHubConfiguration;

STATE_ENTRY_FUNCTION          HUBHSM_UpdatingHubInfoInUCX;

STATE_ENTRY_FUNCTION          HUBHSM_ValidatingAndParsingHubDescriptor;

STATE_ENTRY_FUNCTION          HUBHSM_ValidatingAndParsingHubPowerStatus;

STATE_ENTRY_FUNCTION          HUBHSM_ValidatingHubConfigurationDescriptor;

STATE_ENTRY_FUNCTION          HUBHSM_WaitingForHubDescriptorRetryTimer;

STATE_ENTRY_FUNCTION          HUBHSM_GettingRootHub20PortsInfo;

STATE_ENTRY_FUNCTION          HUBHSM_GettingRootHub30PortsInfo;

STATE_ENTRY_FUNCTION          HUBHSM_GettingRootHubInfo;

STATE_ENTRY_FUNCTION          HUBHSM_ReturningConfigurationFailureForRootHub;

STATE_ENTRY_FUNCTION          HUBHSM_ReturningConfigurationSuccessForRootHub;

STATE_ENTRY_FUNCTION          HUBHSM_GettingAdditionalInfoFromParent;

STATE_ENTRY_FUNCTION          HUBHSM_ReturningOperationFailure;

STATE_ENTRY_FUNCTION          HUBHSM_ReturningOperationSuccess;

STATE_ENTRY_FUNCTION          HUBHSM_GettingHubDescriptorToCheckForPowerLoss;

STATE_ENTRY_FUNCTION          HUBHSM_ReturningOperationFailureInCheckingForPowerLoss;

STATE_ENTRY_FUNCTION          HUBHSM_ReturningOperationSuccessInCheckingForPowerLoss;

STATE_ENTRY_FUNCTION          HUBHSM_ReturningOperationFailureInSettingHubDepth;

STATE_ENTRY_FUNCTION          HUBHSM_ReturningOperationSuccessInSettingHubDepth;

STATE_ENTRY_FUNCTION          HUBHSM_SettingHubDepth;




//
// Sub State Entries for the states in the State Machine
//


SUBSM_ENTRY HsmSubSmCheckingForUnexpectedPowerLoss[] = {
    // SubSmFilter, SubSmInitialState
    { HsmSubSmFlagHub20|HsmSubSmFlagHub30 , HsmStateGettingHubDescriptorToCheckForPowerLoss },
    { HsmSubSmFlagNone , HsmStateNull }
};


SUBSM_ENTRY HsmSubSmConfiguring[] = {
    // SubSmFilter, SubSmInitialState
    { HsmSubSmFlagHubRoot , HsmStateGettingRootHubInfo },
    { HsmSubSmFlagHub20|HsmSubSmFlagHub30 , HsmStateInitializingHubDescriptorRetryCount },
    { HsmSubSmFlagNone , HsmStateNull }
};


SUBSM_ENTRY HsmSubSmConfiguringHubAfterReset[] = {
    // SubSmFilter, SubSmInitialState
    { HsmSubSmFlagHub30 , HsmStateSettingHubDepth },
    { HsmSubSmFlagNone , HsmStateNull }
};


SUBSM_ENTRY HsmSubSmConfiguringHubAfterResetOnSystemResume[] = {
    // SubSmFilter, SubSmInitialState
    { HsmSubSmFlagHub30 , HsmStateSettingHubDepth },
    { HsmSubSmFlagNone , HsmStateNull }
};


SUBSM_ENTRY HsmSubSmGettingAdditionalConfigInfo[] = {
    // SubSmFilter, SubSmInitialState
    { HsmSubSmFlagHub20|HsmSubSmFlagHub30 , HsmStateGettingAdditionalInfoFromParent },
    { HsmSubSmFlagNone , HsmStateNull }
};


SUBSM_ENTRY HsmSubSmReConfiguring[] = {
    // SubSmFilter, SubSmInitialState
    { HsmSubSmFlagHub20|HsmSubSmFlagHub30 , HsmStateInitializingHubDescriptorRetryCount },
    { HsmSubSmFlagNone , HsmStateNull }
};


SUBSM_ENTRY HsmSubSmConfiguringAfterSetConfig[] = {
    // SubSmFilter, SubSmInitialState
    { HsmSubSmFlagHub30 , HsmStateSettingHubDepth },
    { HsmSubSmFlagNone , HsmStateNull }
};


//
// State Entries for the states in the State Machine
//
STATE_ENTRY   HsmStateEntryAcquiringWdfPowerReferenceInitialD0Entry = {
    // State ETW Name
    HsmStateEtwAcquiringWdfPowerReferenceInitialD0Entry,
    // State Entry Function
    HUBHSM_AcquiringWdfPowerReferenceInitialD0Entry,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateWaitingForPortsToAcquireReferenceOnStart },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryAcquiringWdfPowerReferenceOnInterrupt = {
    // State ETW Name
    HsmStateEtwAcquiringWdfPowerReferenceOnInterrupt,
    // State Entry Function
    HUBHSM_AcquiringWdfPowerReferenceOnInterrupt,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateIsItHubChange },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryAcquiringWdfPowerReferenceOnInterruptFailureAndCheckingIfRootHub = {
    // State ETW Name
    HsmStateEtwAcquiringWdfPowerReferenceOnInterruptFailureAndCheckingIfRootHub,
    // State Entry Function
    HUBHSM_AcquiringWdfPowerReferenceOnInterruptFailureAndCheckingIfRootHub,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateCheckingPSMInterruptReferencesOnResetInNoInterrupt },
        { HsmEventNo ,                  HsmStateHasResetPipeCountReachedMaximum },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryAcquiringWdfPowerReferenceOnReset = {
    // State ETW Name
    HsmStateEtwAcquiringWdfPowerReferenceOnReset,
    // State Entry Function
    HUBHSM_AcquiringWdfPowerReferenceOnReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateCheckingPSMInterruptReferencesOnReset },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryAcquiringWdfPowerReferenceOnResumeInSX = {
    // State ETW Name
    HsmStateEtwAcquiringWdfPowerReferenceOnResumeInSX,
    // State Entry Function
    HUBHSM_AcquiringWdfPowerReferenceOnResumeInSX,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateCheckingIfResetByParent },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryAcquiringWdfReferenceOnResumeInS0 = {
    // State ETW Name
    HsmStateEtwAcquiringWdfReferenceOnResumeInS0,
    // State Entry Function
    HUBHSM_AcquiringWdfReferenceOnResumeInS0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateWaitingForPortsToAcquireReferenceOnResumeInS0 },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryBugcheckingSystemOnExcessiveResetsForHubInBootPath = {
    // State ETW Name
    HsmStateEtwBugcheckingSystemOnExcessiveResetsForHubInBootPath,
    // State Entry Function
    HUBHSM_BugcheckingSystemOnExcessiveResetsForHubInBootPath,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCancellingInterruptTransferOnReset = {
    // State ETW Name
    HsmStateEtwCancellingInterruptTransferOnReset,
    // State Entry Function
    HUBHSM_CancellingInterruptTransferOnReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventInterruptTransferSuccess ,HsmStateNotifyingHubStopToPortsOnReset },
        { HsmEventInterruptTransferFailure ,HsmStateNotifyingHubStopToPortsOnReset },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCancellingInterruptTransferOnStop = {
    // State ETW Name
    HsmStateEtwCancellingInterruptTransferOnStop,
    // State Entry Function
    HUBHSM_CancellingInterruptTransferOnStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventInterruptTransferSuccess ,HsmStateNotifyingHubStopToDevices },
        { HsmEventInterruptTransferFailure ,HsmStateNotifyingHubStopToDevices },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCancellingInterruptTransferOnSuspend = {
    // State ETW Name
    HsmStateEtwCancellingInterruptTransferOnSuspend,
    // State Entry Function
    HUBHSM_CancellingInterruptTransferOnSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventInterruptTransferSuccess ,HsmStateWaitingForDevicesToReleaseReferenceOnSuspend },
        { HsmEventInterruptTransferFailure ,HsmStateWaitingForDevicesToReleaseReferenceOnSuspend },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckIfThereIsAValidHubChange = {
    // State ETW Name
    HsmStateEtwCheckIfThereIsAValidHubChange,
    // State Entry Function
    HUBHSM_CheckIfThereIsAValidHubChange,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateSendingAckForHubChange },
        { HsmEventHubError ,            HsmStateHubStatusError },
        { HsmEventNo ,                  HsmStateSendingInterruptTransfer },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckingForUnexpectedPowerLoss = {
    // State ETW Name
    HsmStateEtwCheckingForUnexpectedPowerLoss,
    // State Entry Function
    HUBHSM_CheckingForUnexpectedPowerLoss,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    HsmSubSmCheckingForUnexpectedPowerLoss,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationFailure ,    HsmStateResettingHubOnResume },
        { HsmEventOperationSuccess ,    HsmStateWaitingForPortsToAcquireReferenceOnResume },
        { HsmEventContinueIfNoSubState ,HsmStateWaitingForPortsToAcquireReferenceOnResume },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckingIfHubIsInBootPath = {
    // State ETW Name
    HsmStateEtwCheckingIfHubIsInBootPath,
    // State Entry Function
    HUBHSM_CheckingIfHubIsInBootPath,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateBugcheckingSystemOnExcessiveResetsForHubInBootPath },
        { HsmEventNo ,                  HsmStateReportingErrorToPnp },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckingIfResetByParent = {
    // State ETW Name
    HsmStateEtwCheckingIfResetByParent,
    // State Entry Function
    HUBHSM_CheckingIfResetByParent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateConfiguringHubAfterResetOnSystemResume },
        { HsmEventNo ,                  HsmStateCheckingForUnexpectedPowerLoss },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckingIfResetByParentWithResetPending = {
    // State ETW Name
    HsmStateEtwCheckingIfResetByParentWithResetPending,
    // State Entry Function
    HUBHSM_CheckingIfResetByParentWithResetPending,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateResettingHubOnResume },
        { HsmEventNo ,                  HsmStateResettingHubOnResume },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckingIfResetRecoveryHasBeenInvokedTooManyTimes = {
    // State ETW Name
    HsmStateEtwCheckingIfResetRecoveryHasBeenInvokedTooManyTimes,
    // State Entry Function
    HUBHSM_CheckingIfResetRecoveryHasBeenInvokedTooManyTimes,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateCheckingIfHubIsInBootPath },
        { HsmEventNo ,                  HsmStateInitializingResetCount },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckingPSMInterruptReferencesOnReset = {
    // State ETW Name
    HsmStateEtwCheckingPSMInterruptReferencesOnReset,
    // State Entry Function
    HUBHSM_CheckingPSMInterruptReferencesOnReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateCancellingInterruptTransferOnReset },
        { HsmEventNo ,                  HsmStateWaitingForPSMInterruptReferencesOnReset },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckingPSMInterruptReferencesOnResetInNoInterrupt = {
    // State ETW Name
    HsmStateEtwCheckingPSMInterruptReferencesOnResetInNoInterrupt,
    // State Entry Function
    HUBHSM_CheckingPSMInterruptReferencesOnResetInNoInterrupt,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateNotifyingHubStopToPortsOnReset },
        { HsmEventNo ,                  HsmStateWaitingForInterruptReferencesReleasedOnReset },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckingPSMInterruptReferencesOnResetInNoInterruptOnFatalError = {
    // State ETW Name
    HsmStateEtwCheckingPSMInterruptReferencesOnResetInNoInterruptOnFatalError,
    // State Entry Function
    HUBHSM_CheckingPSMInterruptReferencesOnResetInNoInterruptOnFatalError,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateNotifyingHubStopToPortsOnFatalError },
        { HsmEventNo ,                  HsmStateWaitingForInterruptReferencesReleasedOnFatalError },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckingPSMInterruptReferencesOnStop = {
    // State ETW Name
    HsmStateEtwCheckingPSMInterruptReferencesOnStop,
    // State Entry Function
    HUBHSM_CheckingPSMInterruptReferencesOnStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateCancellingInterruptTransferOnStop },
        { HsmEventNo ,                  HsmStateWaitingForPSMInterruptReferencesOnStop },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckingPSMInterruptReferencesOnStopInNoInterrupt = {
    // State ETW Name
    HsmStateEtwCheckingPSMInterruptReferencesOnStopInNoInterrupt,
    // State Entry Function
    HUBHSM_CheckingPSMInterruptReferencesOnStopInNoInterrupt,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateNotifyingHubStopToDevices },
        { HsmEventNo ,                  HsmStateWaitingForInterruptReferencesReleasedOnStop },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckingPSMInterruptReferencesOnSuspend = {
    // State ETW Name
    HsmStateEtwCheckingPSMInterruptReferencesOnSuspend,
    // State Entry Function
    HUBHSM_CheckingPSMInterruptReferencesOnSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateCancellingInterruptTransferOnSuspend },
        { HsmEventNo ,                  HsmStateWaitingForPSMInterruptReferencesOnSuspend },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckingPSMInterruptReferencesOnSxInNoInterrupt = {
    // State ETW Name
    HsmStateEtwCheckingPSMInterruptReferencesOnSxInNoInterrupt,
    // State Entry Function
    HUBHSM_CheckingPSMInterruptReferencesOnSxInNoInterrupt,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateWaitingForDevicesToReleaseReferenceOnSuspend },
        { HsmEventNo ,                  HsmStateWaitingForInterruptReferencesReleasedOnSuspend },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckOverCurrentBit = {
    // State ETW Name
    HsmStateEtwCheckOverCurrentBit,
    // State Entry Function
    HUBHSM_CheckOverCurrentBit,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateCheckingPSMInterruptReferencesOnResetInNoInterrupt },
        { HsmEventNo ,                  HsmStateErrorOnWaitingForOvercurrentClear },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCompletingPDORequestForGetPortStatus = {
    // State ETW Name
    HsmStateEtwCompletingPDORequestForGetPortStatus,
    // State Entry Function
    HUBHSM_CompletingPDORequestForGetPortStatus,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateConfiguredWithoutInterruptTransfer },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCompletingPDORequestForGetPortStatusFromConfigWithIntTransfer = {
    // State ETW Name
    HsmStateEtwCompletingPDORequestForGetPortStatusFromConfigWithIntTransfer,
    // State Entry Function
    HUBHSM_CompletingPDORequestForGetPortStatusFromConfigWithIntTransfer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateConfiguredWithIntTransfer },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCompletingPDORequestForGetPortStatusFromConfigWithIntTransferWithNoSuchDevice = {
    // State ETW Name
    HsmStateEtwCompletingPDORequestForGetPortStatusFromConfigWithIntTransferWithNoSuchDevice,
    // State Entry Function
    HUBHSM_CompletingPDORequestForGetPortStatusFromConfigWithIntTransferWithNoSuchDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateConfiguredWithIntTransfer },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCompletingPDORequestForGetPortStatusInSuspended = {
    // State ETW Name
    HsmStateEtwCompletingPDORequestForGetPortStatusInSuspended,
    // State Entry Function
    HUBHSM_CompletingPDORequestForGetPortStatusInSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateSuspended },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCompletingPDORequestForGetPortStatusWithNoSuchDevice = {
    // State ETW Name
    HsmStateEtwCompletingPDORequestForGetPortStatusWithNoSuchDevice,
    // State Entry Function
    HUBHSM_CompletingPDORequestForGetPortStatusWithNoSuchDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateConfiguredWithoutInterruptTransfer },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryConfiguredWithIntTransfer = {
    // State ETW Name
    HsmStateEtwConfiguredWithIntTransfer,
    // State Entry Function
    HUBHSM_ConfiguredWithIntTransfer,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventResetHub ,            HsmStateAcquiringWdfPowerReferenceOnReset },
        { HsmEventFDOD0Exit ,           HsmStateCheckingPSMInterruptReferencesOnSuspend },
        { HsmEventInterruptTransferSuccess ,HsmStateAcquiringWdfPowerReferenceOnInterrupt },
        { HsmEventInterruptTransferFailure ,HsmStateAcquiringWdfPowerReferenceOnInterruptFailureAndCheckingIfRootHub },
        { HsmEventFDOD0ExitFinal ,      HsmStateCheckingPSMInterruptReferencesOnStop },
        { HsmEventPDORequestGetPortStatus ,HsmStateGettingPortStatusOnPDORequestFromConfigWithIntTransfer },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryConfiguredWithoutInterruptTransfer = {
    // State ETW Name
    HsmStateEtwConfiguredWithoutInterruptTransfer,
    // State Entry Function
    HUBHSM_ConfiguredWithoutInterruptTransfer,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventResetHub ,            HsmStateWaitingForEnableInterruptsOnReset },
        { HsmEventFDOD0ExitFinal ,      HsmStateWaitingForEnableInterruptsOnStop },
        { HsmEventPortEnableInterruptTransfer ,HsmStateSendingInterruptTransfer },
        { HsmEventPDORequestGetPortStatus ,HsmStateGettingPortStatusOnPDORequest },
        { HsmEventFDOD0Exit ,           HsmStateWaitingForEnableInterruptsOnSx },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryConfiguring = {
    // State ETW Name
    HsmStateEtwConfiguring,
    // State Entry Function
    HUBHSM_Configuring,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    HsmSubSmConfiguring,
    // Event State Pairs for Transitions
    {
        { HsmEventHubConfigurationSuccess ,HsmStateQueryingForHubHackFlags },
        { HsmEventHubConfigurationFailure ,HsmStateReportingStartFailure },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryConfiguringHubAfterReset = {
    // State ETW Name
    HsmStateEtwConfiguringHubAfterReset,
    // State Entry Function
    HUBHSM_ConfiguringHubAfterReset,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    HsmSubSmConfiguringHubAfterReset,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationFailure ,    HsmStateCheckingPSMInterruptReferencesOnResetInNoInterrupt },
        { HsmEventContinueIfNoSubState ,HsmStateWaitingForPortsToAcquireReferencePostReset },
        { HsmEventOperationSuccess ,    HsmStateWaitingForPortsToAcquireReferencePostReset },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryConfiguringHubAfterResetOnSystemResume = {
    // State ETW Name
    HsmStateEtwConfiguringHubAfterResetOnSystemResume,
    // State Entry Function
    HUBHSM_ConfiguringHubAfterResetOnSystemResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    HsmSubSmConfiguringHubAfterResetOnSystemResume,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationFailure ,    HsmStateNotifyingHubResumeWithResetToDevicesOnFailure },
        { HsmEventContinueIfNoSubState ,HsmStateWaitingForPortsToAcquireReferenceOnResumeWithReset },
        { HsmEventOperationSuccess ,    HsmStateWaitingForPortsToAcquireReferenceOnResumeWithReset },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCreatingChildPSMs = {
    // State ETW Name
    HsmStateEtwCreatingChildPSMs,
    // State Entry Function
    HUBHSM_CreatingChildPSMs,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateWaitingForD0Entry },
        { HsmEventOperationFailure ,    HsmStateReportingStartFailure },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryErrorOnWaitingForOvercurrentClear = {
    // State ETW Name
    HsmStateEtwErrorOnWaitingForOvercurrentClear,
    // State Entry Function
    HUBHSM_ErrorOnWaitingForOvercurrentClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventHubIgnoreError ,      HsmStateCheckingPSMInterruptReferencesOnResetInNoInterrupt },
        { HsmEventHubFatalError ,       HsmStateCheckingPSMInterruptReferencesOnResetInNoInterruptOnFatalError },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingAdditionalConfigInfo = {
    // State ETW Name
    HsmStateEtwGettingAdditionalConfigInfo,
    // State Entry Function
    HUBHSM_GettingAdditionalConfigInfo,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    HsmSubSmGettingAdditionalConfigInfo,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationFailure ,    HsmStateReportingStartFailure },
        { HsmEventOperationSuccess ,    HsmStateConfiguring },
        { HsmEventContinueIfNoSubState ,HsmStateConfiguring },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingHubPortStatus = {
    // State ETW Name
    HsmStateEtwGettingHubPortStatus,
    // State Entry Function
    HUBHSM_GettingHubPortStatus,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventTransferSuccess ,     HsmStateCheckIfThereIsAValidHubChange },
        { HsmEventTransferFailure ,     HsmStateCheckingPSMInterruptReferencesOnResetInNoInterrupt },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingHubPortStatusForLostChanges = {
    // State ETW Name
    HsmStateEtwGettingHubPortStatusForLostChanges,
    // State Entry Function
    HUBHSM_GettingHubPortStatusForLostChanges,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventTransferSuccess ,     HsmStateQueueingHubStatusForLostChanges },
        { HsmEventTransferFailure ,     HsmStateCheckingPSMInterruptReferencesOnResetInNoInterrupt },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingOverCurrentBitStatus = {
    // State ETW Name
    HsmStateEtwGettingOverCurrentBitStatus,
    // State Entry Function
    HUBHSM_GettingOverCurrentBitStatus,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventTransferFailure ,     HsmStateErrorOnWaitingForOvercurrentClear },
        { HsmEventTransferSuccess ,     HsmStateCheckOverCurrentBit },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingPortStatusOnPDORequest = {
    // State ETW Name
    HsmStateEtwGettingPortStatusOnPDORequest,
    // State Entry Function
    HUBHSM_GettingPortStatusOnPDORequest,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventTransferSuccess ,     HsmStateCompletingPDORequestForGetPortStatus },
        { HsmEventTransferFailure ,     HsmStateCompletingPDORequestForGetPortStatusWithNoSuchDevice },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingPortStatusOnPDORequestFromConfigWithIntTransfer = {
    // State ETW Name
    HsmStateEtwGettingPortStatusOnPDORequestFromConfigWithIntTransfer,
    // State Entry Function
    HUBHSM_GettingPortStatusOnPDORequestFromConfigWithIntTransfer,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventTransferSuccess ,     HsmStateCompletingPDORequestForGetPortStatusFromConfigWithIntTransfer },
        { HsmEventTransferFailure ,     HsmStateCompletingPDORequestForGetPortStatusFromConfigWithIntTransferWithNoSuchDevice },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryHasResetCountReachedMaximum = {
    // State ETW Name
    HsmStateEtwHasResetCountReachedMaximum,
    // State Entry Function
    HUBHSM_HasResetCountReachedMaximum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateReportingErrorToPnp },
        { HsmEventNo ,                  HsmStateResettingHub },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryHasResetPipeCountReachedMaximum = {
    // State ETW Name
    HsmStateEtwHasResetPipeCountReachedMaximum,
    // State Entry Function
    HUBHSM_HasResetPipeCountReachedMaximum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateCheckingPSMInterruptReferencesOnResetInNoInterrupt },
        { HsmEventNo ,                  HsmStateResettingInterruptPipeOnFailure },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryHubStatusError = {
    // State ETW Name
    HsmStateEtwHubStatusError,
    // State Entry Function
    HUBHSM_HubStatusError,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventHubFatalError ,       HsmStateCheckingPSMInterruptReferencesOnResetInNoInterrupt },
        { HsmEventHubIgnoreError ,      HsmStateSendingInterruptTransfer },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryInitializingResetCount = {
    // State ETW Name
    HsmStateEtwInitializingResetCount,
    // State Entry Function
    HUBHSM_InitializingResetCount,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateResettingHub },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryIsItHubChange = {
    // State ETW Name
    HsmStateEtwIsItHubChange,
    // State Entry Function
    HUBHSM_IsItHubChange,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateGettingHubPortStatus },
        { HsmEventNo ,                  HsmStateQueueingPortStatusChangeEvents },
        { HsmEventHubError ,            HsmStateHubStatusError },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryLoggingHubWasReset = {
    // State ETW Name
    HsmStateEtwLoggingHubWasReset,
    // State Entry Function
    HUBHSM_LoggingHubWasReset,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateStartingTimerForResetRetry },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryNotifyingHubResetToPortsOnReset = {
    // State ETW Name
    HsmStateEtwNotifyingHubResetToPortsOnReset,
    // State Entry Function
    HUBHSM_NotifyingHubResetToPortsOnReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortResetReferencesReleased ,HsmStateCheckingIfResetRecoveryHasBeenInvokedTooManyTimes },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryNotifyingHubResetToPortsOnResetOnFatalError = {
    // State ETW Name
    HsmStateEtwNotifyingHubResetToPortsOnResetOnFatalError,
    // State Entry Function
    HUBHSM_NotifyingHubResetToPortsOnResetOnFatalError,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortResetReferencesReleased ,HsmStateReportingErrorToPnp },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryNotifyingHubResetToPortsOnResetOnResume = {
    // State ETW Name
    HsmStateEtwNotifyingHubResetToPortsOnResetOnResume,
    // State Entry Function
    HUBHSM_NotifyingHubResetToPortsOnResetOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortResetReferencesReleased ,HsmStateUnblockingResumeOnHubResetFailure },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryNotifyingHubResumeInS0ToDevices = {
    // State ETW Name
    HsmStateEtwNotifyingHubResumeInS0ToDevices,
    // State Entry Function
    HUBHSM_NotifyingHubResumeInS0ToDevices,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventDevicePowerReferencesAcquired ,HsmStateQueueingFakeStatusChangeOnResumeInS0 },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryNotifyingHubResumeToDevices = {
    // State ETW Name
    HsmStateEtwNotifyingHubResumeToDevices,
    // State Entry Function
    HUBHSM_NotifyingHubResumeToDevices,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventDevicePowerReferencesAcquired ,HsmStateGettingHubPortStatusForLostChanges },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryNotifyingHubResumeWithResetToDevices = {
    // State ETW Name
    HsmStateEtwNotifyingHubResumeWithResetToDevices,
    // State Entry Function
    HUBHSM_NotifyingHubResumeWithResetToDevices,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventDevicePowerReferencesAcquired ,HsmStateGettingHubPortStatusForLostChanges },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryNotifyingHubResumeWithResetToDevicesOnFailure = {
    // State ETW Name
    HsmStateEtwNotifyingHubResumeWithResetToDevicesOnFailure,
    // State Entry Function
    HUBHSM_NotifyingHubResumeWithResetToDevicesOnFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventDevicePowerReferencesAcquired ,HsmStateWaitingForPortsToReleaseReferenceOnResetOnResume },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryNotifyingHubStopToDevices = {
    // State ETW Name
    HsmStateEtwNotifyingHubStopToDevices,
    // State Entry Function
    HUBHSM_NotifyingHubStopToDevices,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventDevicePowerReferencesReleased ,HsmStateNotifyingHubStopToPorts },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryNotifyingHubStopToDevicesOnSuspriseRemove = {
    // State ETW Name
    HsmStateEtwNotifyingHubStopToDevicesOnSuspriseRemove,
    // State Entry Function
    HUBHSM_NotifyingHubStopToDevicesOnSuspriseRemove,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventDeviceStopAfterSuspendAcked ,HsmStateSupriseRemoved },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryNotifyingHubStopToPorts = {
    // State ETW Name
    HsmStateEtwNotifyingHubStopToPorts,
    // State Entry Function
    HUBHSM_NotifyingHubStopToPorts,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateWaitingForPortsToReleaseReferenceOnStop },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryNotifyingHubStopToPortsOnFatalError = {
    // State ETW Name
    HsmStateEtwNotifyingHubStopToPortsOnFatalError,
    // State Entry Function
    HUBHSM_NotifyingHubStopToPortsOnFatalError,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortPowerReferencesReleased ,HsmStateNotifyingHubResetToPortsOnResetOnFatalError },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryNotifyingHubStopToPortsOnReset = {
    // State ETW Name
    HsmStateEtwNotifyingHubStopToPortsOnReset,
    // State Entry Function
    HUBHSM_NotifyingHubStopToPortsOnReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateWaitingForPortsToReleaseReferenceOnReset },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryQueryingForHubHackFlags = {
    // State ETW Name
    HsmStateEtwQueryingForHubHackFlags,
    // State Entry Function
    HUBHSM_QueryingForHubHackFlags,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateCreatingChildPSMs },
        { HsmEventOperationFailure ,    HsmStateCreatingChildPSMs },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryQueueingFakeStatusChangeOnResumeInS0 = {
    // State ETW Name
    HsmStateEtwQueueingFakeStatusChangeOnResumeInS0,
    // State Entry Function
    HUBHSM_QueueingFakeStatusChangeOnResumeInS0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateConfiguredWithoutInterruptTransfer },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryQueueingHubStatusForLostChanges = {
    // State ETW Name
    HsmStateEtwQueueingHubStatusForLostChanges,
    // State Entry Function
    HUBHSM_QueueingHubStatusForLostChanges,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateSendingInterruptTransfer },
        { HsmEventHubLocalPowerGood ,   HsmStateSendingInterruptTransfer },
        { HsmEventHubLocalPowerLost ,   HsmStateSendingInterruptTransfer },
        { HsmEventHubOverCurrentCleared ,HsmStateCheckingPSMInterruptReferencesOnResetInNoInterrupt },
        { HsmEventHubOverCurrent ,      HsmStateWaitingForOvercurrentToClear },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryQueueingPortStatusChangeEvents = {
    // State ETW Name
    HsmStateEtwQueueingPortStatusChangeEvents,
    // State Entry Function
    HUBHSM_QueueingPortStatusChangeEvents,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateConfiguredWithoutInterruptTransfer },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryQueuingHubChange = {
    // State ETW Name
    HsmStateEtwQueuingHubChange,
    // State Entry Function
    HUBHSM_QueuingHubChange,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventHubOverCurrentCleared ,HsmStateCheckingPSMInterruptReferencesOnResetInNoInterrupt },
        { HsmEventHubLocalPowerGood ,   HsmStateSendingInterruptTransfer },
        { HsmEventHubOverCurrent ,      HsmStateWaitingForOvercurrentToClear },
        { HsmEventHubLocalPowerLost ,   HsmStateSendingInterruptTransfer },
        { HsmEventHubError ,            HsmStateHubStatusError },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryReConfiguring = {
    // State ETW Name
    HsmStateEtwReConfiguring,
    // State Entry Function
    HUBHSM_ReConfiguring,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    HsmSubSmReConfiguring,
    // Event State Pairs for Transitions
    {
        { HsmEventHubConfigurationFailure ,HsmStateReportingReStartFailure },
        { HsmEventHubConfigurationSuccess ,HsmStateWaitingForD0Entry },
        { HsmEventContinueIfNoSubState ,HsmStateWaitingForD0Entry },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryReportingErrorToPnp = {
    // State ETW Name
    HsmStateEtwReportingErrorToPnp,
    // State Entry Function
    HUBHSM_ReportingErrorToPnp,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateWaitingForStop },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryReportingReStartFailure = {
    // State ETW Name
    HsmStateEtwReportingReStartFailure,
    // State Entry Function
    HUBHSM_ReportingReStartFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateWaitingForReleaseHardwareOnReStartFailure },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryReportingStartFailure = {
    // State ETW Name
    HsmStateEtwReportingStartFailure,
    // State Entry Function
    HUBHSM_ReportingStartFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateWaitingForReleaseHardwareOnStartFailure },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryResettingHub = {
    // State ETW Name
    HsmStateEtwResettingHub,
    // State Entry Function
    HUBHSM_ResettingHub,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventResetHub ,            HsmStateIgnored },
        { HsmEventResetHubFailed ,      HsmStateLoggingHubWasReset },
        { HsmEventResetHubComplete ,    HsmStateConfiguringHubAfterReset },
        { HsmEventResetHubFailedOnRemoval ,HsmStateStartingTimerForResetRetry },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryResettingHubOnResume = {
    // State ETW Name
    HsmStateEtwResettingHubOnResume,
    // State Entry Function
    HUBHSM_ResettingHubOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventResetHubFailed ,      HsmStateNotifyingHubResumeWithResetToDevicesOnFailure },
        { HsmEventResetHubComplete ,    HsmStateConfiguringHubAfterResetOnSystemResume },
        { HsmEventResetHubFailedOnRemoval ,HsmStateNotifyingHubResumeWithResetToDevicesOnFailure },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryResettingInterruptPipeOnFailure = {
    // State ETW Name
    HsmStateEtwResettingInterruptPipeOnFailure,
    // State Entry Function
    HUBHSM_ResettingInterruptPipeOnFailure,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventResetInterruptPipeFailure ,HsmStateCheckingPSMInterruptReferencesOnResetInNoInterrupt },
        { HsmEventResetInterruptPipeSuccess ,HsmStateSendingInterruptTransferAfterResettingPipe },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntrySendingAckForHubChange = {
    // State ETW Name
    HsmStateEtwSendingAckForHubChange,
    // State Entry Function
    HUBHSM_SendingAckForHubChange,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventTransferSuccess ,     HsmStateQueuingHubChange },
        { HsmEventTransferFailure ,     HsmStateCheckingPSMInterruptReferencesOnResetInNoInterrupt },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntrySendingInterruptTransfer = {
    // State ETW Name
    HsmStateEtwSendingInterruptTransfer,
    // State Entry Function
    HUBHSM_SendingInterruptTransfer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateConfiguredWithIntTransfer },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntrySendingInterruptTransferAfterResettingPipe = {
    // State ETW Name
    HsmStateEtwSendingInterruptTransferAfterResettingPipe,
    // State Entry Function
    HUBHSM_SendingInterruptTransferAfterResettingPipe,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateConfiguredWithIntTransfer },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntrySignalingPnPPowerEventOnPendingStop = {
    // State ETW Name
    HsmStateEtwSignalingPnPPowerEventOnPendingStop,
    // State Entry Function
    HUBHSM_SignalingPnPPowerEventOnPendingStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateSuspendedInRecycle },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntrySignalingPnPPowerEventOnStart = {
    // State ETW Name
    HsmStateEtwSignalingPnPPowerEventOnStart,
    // State Entry Function
    HUBHSM_SignalingPnPPowerEventOnStart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateGettingHubPortStatusForLostChanges },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntrySignalingPnPPowerEventOnStop = {
    // State ETW Name
    HsmStateEtwSignalingPnPPowerEventOnStop,
    // State Entry Function
    HUBHSM_SignalingPnPPowerEventOnStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateStopped },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntrySignallingPnpPowerEventForSuspended = {
    // State ETW Name
    HsmStateEtwSignallingPnpPowerEventForSuspended,
    // State Entry Function
    HUBHSM_SignallingPnpPowerEventForSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateSuspended },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntrySignallingPnpPowerEventInUnconfigured = {
    // State ETW Name
    HsmStateEtwSignallingPnpPowerEventInUnconfigured,
    // State Entry Function
    HUBHSM_SignallingPnpPowerEventInUnconfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateUnconfigured },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryStartingTimerForResetRetry = {
    // State ETW Name
    HsmStateEtwStartingTimerForResetRetry,
    // State Entry Function
    HUBHSM_StartingTimerForResetRetry,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateWaitingForResetRetryTimer },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryStopped = {
    // State ETW Name
    HsmStateEtwStopped,
    // State Entry Function
    HUBHSM_Stopped,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPDORequestGetPortStatus ,HsmStateCompleteGetPortStatusWithNoSuchDevice },
        { HsmEventResetHub ,            HsmStateIgnored },
        { HsmEventFDOPrepareHardware ,  HsmStateReConfiguring },
        { HsmEventFDODeviceCleanup ,    HsmStateWaitingToBeDeleted },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntrySupriseRemoved = {
    // State ETW Name
    HsmStateEtwSupriseRemoved,
    // State Entry Function
    HUBHSM_SupriseRemoved,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventResetHub ,            HsmStateIgnored },
        { HsmEventPDORequestGetPortStatus ,HsmStateCompleteGetPortStatusWithNoSuchDevice },
        { HsmEventFDODeviceCleanup ,    HsmStateWaitingToBeDeleted },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntrySuspended = {
    // State ETW Name
    HsmStateEtwSuspended,
    // State Entry Function
    HUBHSM_Suspended,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventFDOReleaseHardware ,  HsmStateNotifyingHubStopToDevicesOnSuspriseRemove },
        { HsmEventResetHub ,            HsmStateSuspendedWithPendingReset },
        { HsmEventFDOD0EntryFromSx ,    HsmStateAcquiringWdfPowerReferenceOnResumeInSX },
        { HsmEventPDORequestGetPortStatus ,HsmStateCompletingPDORequestForGetPortStatusInSuspended },
        { HsmEventFDOD0Entry ,          HsmStateAcquiringWdfReferenceOnResumeInS0 },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntrySuspendedInRecycle = {
    // State ETW Name
    HsmStateEtwSuspendedInRecycle,
    // State Entry Function
    HUBHSM_SuspendedInRecycle,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPDORequestGetPortStatus ,HsmStateCompleteGetPortStatusWithInternalError },
        { HsmEventResetHub ,            HsmStateIgnored },
        { HsmEventFDOD0Entry ,          HsmStateUnblockResumeWaitingForStop },
        { HsmEventFDOReleaseHardware ,  HsmStateNotifyingHubStopToDevicesOnSuspriseRemove },
        { HsmEventFDOD0EntryFromSx ,    HsmStateUnblockResumeWaitingForStop },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntrySuspendedWithPendingReset = {
    // State ETW Name
    HsmStateEtwSuspendedWithPendingReset,
    // State Entry Function
    HUBHSM_SuspendedWithPendingReset,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPDORequestGetPortStatus ,HsmStateCompleteGetPortStatusWithInternalError },
        { HsmEventResetHub ,            HsmStateIgnored },
        { HsmEventFDOReleaseHardware ,  HsmStateNotifyingHubStopToDevicesOnSuspriseRemove },
        { HsmEventFDOD0EntryFromSx ,    HsmStateCheckingIfResetByParentWithResetPending },
        { HsmEventFDOD0Entry ,          HsmStateCheckingIfResetByParentWithResetPending },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryUnblockingResumeOnHubResetFailure = {
    // State ETW Name
    HsmStateEtwUnblockingResumeOnHubResetFailure,
    // State Entry Function
    HUBHSM_UnblockingResumeOnHubResetFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateInitializingResetCount },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryUnblockResumeWaitingForStop = {
    // State ETW Name
    HsmStateEtwUnblockResumeWaitingForStop,
    // State Entry Function
    HUBHSM_UnblockResumeWaitingForStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateWaitingForStop },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryUnconfigured = {
    // State ETW Name
    HsmStateEtwUnconfigured,
    // State Entry Function
    HUBHSM_Unconfigured,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventFDOPrepareHardware ,  HsmStateGettingAdditionalConfigInfo },
        { HsmEventFDODeviceCleanup ,    HsmStateWaitingToBeDeleted },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForAddDevice = {
    // State ETW Name
    HsmStateEtwWaitingForAddDevice,
    // State Entry Function
    HUBHSM_WaitingForAddDevice,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventFDODeviceAdd ,        HsmStateSignallingPnpPowerEventInUnconfigured },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForD0Entry = {
    // State ETW Name
    HsmStateEtwWaitingForD0Entry,
    // State Entry Function
    HUBHSM_WaitingForD0Entry,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPDORequestGetPortStatus ,HsmStateCompleteGetPortStatusWithNoSuchDevice },
        { HsmEventFDOD0Entry ,          HsmStateAcquiringWdfPowerReferenceInitialD0Entry },
        { HsmEventFDOReleaseHardware ,  HsmStateSignalingPnPPowerEventOnStop },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForDevicesToAcquireReferenceOnStart = {
    // State ETW Name
    HsmStateEtwWaitingForDevicesToAcquireReferenceOnStart,
    // State Entry Function
    HUBHSM_WaitingForDevicesToAcquireReferenceOnStart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventDevicePowerReferencesAcquired ,HsmStateSignalingPnPPowerEventOnStart },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForDevicesToReleaseReferenceOnD0ExitFinalFromReset = {
    // State ETW Name
    HsmStateEtwWaitingForDevicesToReleaseReferenceOnD0ExitFinalFromReset,
    // State Entry Function
    HUBHSM_WaitingForDevicesToReleaseReferenceOnD0ExitFinalFromReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventDevicePowerReferencesReleased ,HsmStateWaitingForReleaseHardware },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForDevicesToReleaseReferenceOnSuspend = {
    // State ETW Name
    HsmStateEtwWaitingForDevicesToReleaseReferenceOnSuspend,
    // State Entry Function
    HUBHSM_WaitingForDevicesToReleaseReferenceOnSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventDevicePowerReferencesReleased ,HsmStateWaitingForPortsToReleaseReferenceOnSuspend },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForEnableInterruptsOnReset = {
    // State ETW Name
    HsmStateEtwWaitingForEnableInterruptsOnReset,
    // State Entry Function
    HUBHSM_WaitingForEnableInterruptsOnReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortEnableInterruptTransfer ,HsmStateCheckingPSMInterruptReferencesOnResetInNoInterrupt },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForEnableInterruptsOnStop = {
    // State ETW Name
    HsmStateEtwWaitingForEnableInterruptsOnStop,
    // State Entry Function
    HUBHSM_WaitingForEnableInterruptsOnStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortEnableInterruptTransfer ,HsmStateCheckingPSMInterruptReferencesOnStopInNoInterrupt },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForEnableInterruptsOnSx = {
    // State ETW Name
    HsmStateEtwWaitingForEnableInterruptsOnSx,
    // State Entry Function
    HUBHSM_WaitingForEnableInterruptsOnSx,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortEnableInterruptTransfer ,HsmStateCheckingPSMInterruptReferencesOnSxInNoInterrupt },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForInterruptReferencesReleasedOnFatalError = {
    // State ETW Name
    HsmStateEtwWaitingForInterruptReferencesReleasedOnFatalError,
    // State Entry Function
    HUBHSM_WaitingForInterruptReferencesReleasedOnFatalError,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortInterruptReferencesReleased ,HsmStateNotifyingHubStopToPortsOnFatalError },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForInterruptReferencesReleasedOnReset = {
    // State ETW Name
    HsmStateEtwWaitingForInterruptReferencesReleasedOnReset,
    // State Entry Function
    HUBHSM_WaitingForInterruptReferencesReleasedOnReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortInterruptReferencesReleased ,HsmStateNotifyingHubStopToPortsOnReset },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForInterruptReferencesReleasedOnStop = {
    // State ETW Name
    HsmStateEtwWaitingForInterruptReferencesReleasedOnStop,
    // State Entry Function
    HUBHSM_WaitingForInterruptReferencesReleasedOnStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortInterruptReferencesReleased ,HsmStateNotifyingHubStopToDevices },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForInterruptReferencesReleasedOnSuspend = {
    // State ETW Name
    HsmStateEtwWaitingForInterruptReferencesReleasedOnSuspend,
    // State Entry Function
    HUBHSM_WaitingForInterruptReferencesReleasedOnSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortInterruptReferencesReleased ,HsmStateWaitingForDevicesToReleaseReferenceOnSuspend },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForOvercurrentToClear = {
    // State ETW Name
    HsmStateEtwWaitingForOvercurrentToClear,
    // State Entry Function
    HUBHSM_WaitingForOvercurrentToClear,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventTimerFired ,          HsmStateGettingOverCurrentBitStatus },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForPortsToAcquireReferenceOnResume = {
    // State ETW Name
    HsmStateEtwWaitingForPortsToAcquireReferenceOnResume,
    // State Entry Function
    HUBHSM_WaitingForPortsToAcquireReferenceOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortPowerReferencesAcquired ,HsmStateNotifyingHubResumeToDevices },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForPortsToAcquireReferenceOnResumeInS0 = {
    // State ETW Name
    HsmStateEtwWaitingForPortsToAcquireReferenceOnResumeInS0,
    // State Entry Function
    HUBHSM_WaitingForPortsToAcquireReferenceOnResumeInS0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortPowerReferencesAcquired ,HsmStateNotifyingHubResumeInS0ToDevices },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForPortsToAcquireReferenceOnResumeWithReset = {
    // State ETW Name
    HsmStateEtwWaitingForPortsToAcquireReferenceOnResumeWithReset,
    // State Entry Function
    HUBHSM_WaitingForPortsToAcquireReferenceOnResumeWithReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortPowerReferencesAcquired ,HsmStateNotifyingHubResumeWithResetToDevices },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForPortsToAcquireReferenceOnStart = {
    // State ETW Name
    HsmStateEtwWaitingForPortsToAcquireReferenceOnStart,
    // State Entry Function
    HUBHSM_WaitingForPortsToAcquireReferenceOnStart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortPowerReferencesAcquired ,HsmStateWaitingForDevicesToAcquireReferenceOnStart },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForPortsToAcquireReferencePostReset = {
    // State ETW Name
    HsmStateEtwWaitingForPortsToAcquireReferencePostReset,
    // State Entry Function
    HUBHSM_WaitingForPortsToAcquireReferencePostReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortPowerReferencesAcquired ,HsmStateGettingHubPortStatusForLostChanges },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForPortsToReleaseReferenceOnReset = {
    // State ETW Name
    HsmStateEtwWaitingForPortsToReleaseReferenceOnReset,
    // State Entry Function
    HUBHSM_WaitingForPortsToReleaseReferenceOnReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortPowerReferencesReleased ,HsmStateNotifyingHubResetToPortsOnReset },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForPortsToReleaseReferenceOnResetOnResume = {
    // State ETW Name
    HsmStateEtwWaitingForPortsToReleaseReferenceOnResetOnResume,
    // State Entry Function
    HUBHSM_WaitingForPortsToReleaseReferenceOnResetOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortPowerReferencesReleased ,HsmStateNotifyingHubResetToPortsOnResetOnResume },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForPortsToReleaseReferenceOnStop = {
    // State ETW Name
    HsmStateEtwWaitingForPortsToReleaseReferenceOnStop,
    // State Entry Function
    HUBHSM_WaitingForPortsToReleaseReferenceOnStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortPowerReferencesReleased ,HsmStateWaitingForReleaseHardware },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForPortsToReleaseReferenceOnSuspend = {
    // State ETW Name
    HsmStateEtwWaitingForPortsToReleaseReferenceOnSuspend,
    // State Entry Function
    HUBHSM_WaitingForPortsToReleaseReferenceOnSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPortPowerReferencesReleased ,HsmStateSignallingPnpPowerEventForSuspended },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForPSMInterruptReferencesOnReset = {
    // State ETW Name
    HsmStateEtwWaitingForPSMInterruptReferencesOnReset,
    // State Entry Function
    HUBHSM_WaitingForPSMInterruptReferencesOnReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventInterruptTransferFailure ,HsmStateWaitingForInterruptReferencesReleasedOnReset },
        { HsmEventPortInterruptReferencesReleased ,HsmStateCancellingInterruptTransferOnReset },
        { HsmEventInterruptTransferSuccess ,HsmStateWaitingForInterruptReferencesReleasedOnReset },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForPSMInterruptReferencesOnStop = {
    // State ETW Name
    HsmStateEtwWaitingForPSMInterruptReferencesOnStop,
    // State Entry Function
    HUBHSM_WaitingForPSMInterruptReferencesOnStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventInterruptTransferSuccess ,HsmStateWaitingForInterruptReferencesReleasedOnStop },
        { HsmEventPortInterruptReferencesReleased ,HsmStateCancellingInterruptTransferOnStop },
        { HsmEventInterruptTransferFailure ,HsmStateWaitingForInterruptReferencesReleasedOnStop },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForPSMInterruptReferencesOnSuspend = {
    // State ETW Name
    HsmStateEtwWaitingForPSMInterruptReferencesOnSuspend,
    // State Entry Function
    HUBHSM_WaitingForPSMInterruptReferencesOnSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventInterruptTransferSuccess ,HsmStateWaitingForInterruptReferencesReleasedOnSuspend },
        { HsmEventPortInterruptReferencesReleased ,HsmStateCancellingInterruptTransferOnSuspend },
        { HsmEventInterruptTransferFailure ,HsmStateWaitingForInterruptReferencesReleasedOnSuspend },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForReleaseHardware = {
    // State ETW Name
    HsmStateEtwWaitingForReleaseHardware,
    // State Entry Function
    HUBHSM_WaitingForReleaseHardware,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventResetHub ,            HsmStateIgnored },
        { HsmEventPDORequestGetPortStatus ,HsmStateCompleteGetPortStatusWithNoSuchDevice },
        { HsmEventFDOReleaseHardware ,  HsmStateSignalingPnPPowerEventOnStop },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForReleaseHardwareOnReStartFailure = {
    // State ETW Name
    HsmStateEtwWaitingForReleaseHardwareOnReStartFailure,
    // State Entry Function
    HUBHSM_WaitingForReleaseHardwareOnReStartFailure,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPDORequestGetPortStatus ,HsmStateCompleteGetPortStatusWithNoSuchDevice },
        { HsmEventResetHub ,            HsmStateIgnored },
        { HsmEventFDOReleaseHardware ,  HsmStateSignalingPnPPowerEventOnStop },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForReleaseHardwareOnStartFailure = {
    // State ETW Name
    HsmStateEtwWaitingForReleaseHardwareOnStartFailure,
    // State Entry Function
    HUBHSM_WaitingForReleaseHardwareOnStartFailure,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPDORequestGetPortStatus ,HsmStateCompleteGetPortStatusWithNoSuchDevice },
        { HsmEventResetHub ,            HsmStateIgnored },
        { HsmEventFDOReleaseHardware ,  HsmStateSignallingPnpPowerEventInUnconfigured },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForResetRetryTimer = {
    // State ETW Name
    HsmStateEtwWaitingForResetRetryTimer,
    // State Entry Function
    HUBHSM_WaitingForResetRetryTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventPDORequestGetPortStatus ,HsmStateCompleteGetPortStatusWithInternalError },
        { HsmEventResetHub ,            HsmStateIgnored },
        { HsmEventTimerFired ,          HsmStateHasResetCountReachedMaximum },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForStop = {
    // State ETW Name
    HsmStateEtwWaitingForStop,
    // State Entry Function
    HUBHSM_WaitingForStop,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventResetHub ,            HsmStateIgnored },
        { HsmEventPDORequestGetPortStatus ,HsmStateCompleteGetPortStatusWithNoSuchDevice },
        { HsmEventFDOD0Exit ,           HsmStateSignalingPnPPowerEventOnPendingStop },
        { HsmEventFDOD0ExitFinal ,      HsmStateWaitingForDevicesToReleaseReferenceOnD0ExitFinalFromReset },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingToBeDeleted = {
    // State ETW Name
    HsmStateEtwWaitingToBeDeleted,
    // State Entry Function
    HUBHSM_WaitingToBeDeleted,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckingBytesReturnedInHubConfigDescriptor = {
    // State ETW Name
    HsmStateEtwCheckingBytesReturnedInHubConfigDescriptor,
    // State Entry Function
    HUBHSM_CheckingBytesReturnedInHubConfigDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateGettingHubConfigurationDescriptorWithReturnedLength },
        { HsmEventNo ,                  HsmStateValidatingHubConfigurationDescriptor },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryCheckingIfHubDescriptorRetryCountExceededMax = {
    // State ETW Name
    HsmStateEtwCheckingIfHubDescriptorRetryCountExceededMax,
    // State Entry Function
    HUBHSM_CheckingIfHubDescriptorRetryCountExceededMax,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventYes ,                 HsmStateReturningConfigurationFailure },
        { HsmEventNo ,                  HsmStateWaitingForHubDescriptorRetryTimer },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryConfiguringAfterSetConfig = {
    // State ETW Name
    HsmStateEtwConfiguringAfterSetConfig,
    // State Entry Function
    HUBHSM_ConfiguringAfterSetConfig,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    HsmSubSmConfiguringAfterSetConfig,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationFailure ,    HsmStateReturningConfigurationFailure },
        { HsmEventContinueIfNoSubState ,HsmStateReturningConfigurationSuccess },
        { HsmEventOperationSuccess ,    HsmStateReturningConfigurationSuccess },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingHubConfigurationDescriptorWithDefaultLength = {
    // State ETW Name
    HsmStateEtwGettingHubConfigurationDescriptorWithDefaultLength,
    // State Entry Function
    HUBHSM_GettingHubConfigurationDescriptorWithDefaultLength,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventTransferFailure ,     HsmStateReturningConfigurationFailure },
        { HsmEventTransferSuccess ,     HsmStateCheckingBytesReturnedInHubConfigDescriptor },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingHubConfigurationDescriptorWithReturnedLength = {
    // State ETW Name
    HsmStateEtwGettingHubConfigurationDescriptorWithReturnedLength,
    // State Entry Function
    HUBHSM_GettingHubConfigurationDescriptorWithReturnedLength,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventTransferFailure ,     HsmStateReturningConfigurationFailure },
        { HsmEventTransferSuccess ,     HsmStateValidatingHubConfigurationDescriptor },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingHubDescriptor = {
    // State ETW Name
    HsmStateEtwGettingHubDescriptor,
    // State Entry Function
    HUBHSM_GettingHubDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventTransferFailure ,     HsmStateCheckingIfHubDescriptorRetryCountExceededMax },
        { HsmEventTransferSuccess ,     HsmStateValidatingAndParsingHubDescriptor },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingHubStatus = {
    // State ETW Name
    HsmStateEtwGettingHubStatus,
    // State Entry Function
    HUBHSM_GettingHubStatus,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventTransferFailure ,     HsmStateReturningConfigurationFailure },
        { HsmEventTransferSuccess ,     HsmStateValidatingAndParsingHubPowerStatus },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryInitializingHubDescriptorRetryCount = {
    // State ETW Name
    HsmStateEtwInitializingHubDescriptorRetryCount,
    // State Entry Function
    HUBHSM_InitializingHubDescriptorRetryCount,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateGettingHubDescriptor },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryReturningConfigurationFailure = {
    // State ETW Name
    HsmStateEtwReturningConfigurationFailure,
    // State Entry Function
    HUBHSM_ReturningConfigurationFailure,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryReturningConfigurationSuccess = {
    // State ETW Name
    HsmStateEtwReturningConfigurationSuccess,
    // State Entry Function
    HUBHSM_ReturningConfigurationSuccess,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntrySettingHubConfiguration = {
    // State ETW Name
    HsmStateEtwSettingHubConfiguration,
    // State Entry Function
    HUBHSM_SettingHubConfiguration,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventConfigurationFailure ,HsmStateReturningConfigurationFailure },
        { HsmEventConfigurationSuccess ,HsmStateConfiguringAfterSetConfig },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryUpdatingHubInfoInUCX = {
    // State ETW Name
    HsmStateEtwUpdatingHubInfoInUCX,
    // State Entry Function
    HUBHSM_UpdatingHubInfoInUCX,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventIoctlSuccess ,        HsmStateSettingHubConfiguration },
        { HsmEventIoctlFailure ,        HsmStateReturningConfigurationFailure },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryValidatingAndParsingHubDescriptor = {
    // State ETW Name
    HsmStateEtwValidatingAndParsingHubDescriptor,
    // State Entry Function
    HUBHSM_ValidatingAndParsingHubDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationFailure ,    HsmStateReturningConfigurationFailure },
        { HsmEventOperationSuccess ,    HsmStateGettingHubStatus },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryValidatingAndParsingHubPowerStatus = {
    // State ETW Name
    HsmStateEtwValidatingAndParsingHubPowerStatus,
    // State Entry Function
    HUBHSM_ValidatingAndParsingHubPowerStatus,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateGettingHubConfigurationDescriptorWithDefaultLength },
        { HsmEventOperationFailure ,    HsmStateReturningConfigurationFailure },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryValidatingHubConfigurationDescriptor = {
    // State ETW Name
    HsmStateEtwValidatingHubConfigurationDescriptor,
    // State Entry Function
    HUBHSM_ValidatingHubConfigurationDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventOperationSuccess ,    HsmStateUpdatingHubInfoInUCX },
        { HsmEventOperationFailure ,    HsmStateReturningConfigurationFailure },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryWaitingForHubDescriptorRetryTimer = {
    // State ETW Name
    HsmStateEtwWaitingForHubDescriptorRetryTimer,
    // State Entry Function
    HUBHSM_WaitingForHubDescriptorRetryTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventTimerFired ,          HsmStateGettingHubDescriptor },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingRootHub20PortsInfo = {
    // State ETW Name
    HsmStateEtwGettingRootHub20PortsInfo,
    // State Entry Function
    HUBHSM_GettingRootHub20PortsInfo,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventIoctlFailure ,        HsmStateReturningConfigurationFailureForRootHub },
        { HsmEventIoctlSuccess ,        HsmStateGettingRootHub30PortsInfo },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingRootHub30PortsInfo = {
    // State ETW Name
    HsmStateEtwGettingRootHub30PortsInfo,
    // State Entry Function
    HUBHSM_GettingRootHub30PortsInfo,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventIoctlSuccess ,        HsmStateReturningConfigurationSuccessForRootHub },
        { HsmEventIoctlFailure ,        HsmStateReturningConfigurationFailureForRootHub },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingRootHubInfo = {
    // State ETW Name
    HsmStateEtwGettingRootHubInfo,
    // State Entry Function
    HUBHSM_GettingRootHubInfo,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventIoctlFailure ,        HsmStateReturningConfigurationFailureForRootHub },
        { HsmEventIoctlSuccess ,        HsmStateGettingRootHub20PortsInfo },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryReturningConfigurationFailureForRootHub = {
    // State ETW Name
    HsmStateEtwReturningConfigurationFailureForRootHub,
    // State Entry Function
    HUBHSM_ReturningConfigurationFailureForRootHub,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryReturningConfigurationSuccessForRootHub = {
    // State ETW Name
    HsmStateEtwReturningConfigurationSuccessForRootHub,
    // State Entry Function
    HUBHSM_ReturningConfigurationSuccessForRootHub,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingAdditionalInfoFromParent = {
    // State ETW Name
    HsmStateEtwGettingAdditionalInfoFromParent,
    // State Entry Function
    HUBHSM_GettingAdditionalInfoFromParent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventIoctlFailure ,        HsmStateReturningOperationFailure },
        { HsmEventIoctlSuccess ,        HsmStateReturningOperationSuccess },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryReturningOperationFailure = {
    // State ETW Name
    HsmStateEtwReturningOperationFailure,
    // State Entry Function
    HUBHSM_ReturningOperationFailure,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryReturningOperationSuccess = {
    // State ETW Name
    HsmStateEtwReturningOperationSuccess,
    // State Entry Function
    HUBHSM_ReturningOperationSuccess,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryGettingHubDescriptorToCheckForPowerLoss = {
    // State ETW Name
    HsmStateEtwGettingHubDescriptorToCheckForPowerLoss,
    // State Entry Function
    HUBHSM_GettingHubDescriptorToCheckForPowerLoss,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventTransferFailure ,     HsmStateReturningOperationFailureInCheckingForPowerLoss },
        { HsmEventTransferSuccess ,     HsmStateReturningOperationSuccessInCheckingForPowerLoss },
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryReturningOperationFailureInCheckingForPowerLoss = {
    // State ETW Name
    HsmStateEtwReturningOperationFailureInCheckingForPowerLoss,
    // State Entry Function
    HUBHSM_ReturningOperationFailureInCheckingForPowerLoss,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryReturningOperationSuccessInCheckingForPowerLoss = {
    // State ETW Name
    HsmStateEtwReturningOperationSuccessInCheckingForPowerLoss,
    // State Entry Function
    HUBHSM_ReturningOperationSuccessInCheckingForPowerLoss,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryReturningOperationFailureInSettingHubDepth = {
    // State ETW Name
    HsmStateEtwReturningOperationFailureInSettingHubDepth,
    // State Entry Function
    HUBHSM_ReturningOperationFailureInSettingHubDepth,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntryReturningOperationSuccessInSettingHubDepth = {
    // State ETW Name
    HsmStateEtwReturningOperationSuccessInSettingHubDepth,
    // State Entry Function
    HUBHSM_ReturningOperationSuccessInSettingHubDepth,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventNull ,                HsmStateNull },
    }
};



STATE_ENTRY   HsmStateEntrySettingHubDepth = {
    // State ETW Name
    HsmStateEtwSettingHubDepth,
    // State Entry Function
    HUBHSM_SettingHubDepth,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { HsmEventTransferFailure ,     HsmStateReturningOperationFailureInSettingHubDepth },
        { HsmEventTransferSuccess ,     HsmStateReturningOperationSuccessInSettingHubDepth },
        { HsmEventNull ,                HsmStateNull },
    }
};






//
// Global List of States in all the State Machines
//
PSTATE_ENTRY    HSMStateTable[] = {
    &HsmStateEntryWaitingForAddDevice,
    &HsmStateEntryAcquiringWdfPowerReferenceInitialD0Entry,
    &HsmStateEntryAcquiringWdfPowerReferenceOnInterrupt,
    &HsmStateEntryAcquiringWdfPowerReferenceOnInterruptFailureAndCheckingIfRootHub,
    &HsmStateEntryAcquiringWdfPowerReferenceOnReset,
    &HsmStateEntryAcquiringWdfPowerReferenceOnResumeInSX,
    &HsmStateEntryAcquiringWdfReferenceOnResumeInS0,
    &HsmStateEntryBugcheckingSystemOnExcessiveResetsForHubInBootPath,
    &HsmStateEntryCancellingInterruptTransferOnReset,
    &HsmStateEntryCancellingInterruptTransferOnStop,
    &HsmStateEntryCancellingInterruptTransferOnSuspend,
    &HsmStateEntryCheckIfThereIsAValidHubChange,
    &HsmStateEntryCheckingForUnexpectedPowerLoss,
    &HsmStateEntryCheckingIfHubIsInBootPath,
    &HsmStateEntryCheckingIfResetByParent,
    &HsmStateEntryCheckingIfResetByParentWithResetPending,
    &HsmStateEntryCheckingIfResetRecoveryHasBeenInvokedTooManyTimes,
    &HsmStateEntryCheckingPSMInterruptReferencesOnReset,
    &HsmStateEntryCheckingPSMInterruptReferencesOnResetInNoInterrupt,
    &HsmStateEntryCheckingPSMInterruptReferencesOnResetInNoInterruptOnFatalError,
    &HsmStateEntryCheckingPSMInterruptReferencesOnStop,
    &HsmStateEntryCheckingPSMInterruptReferencesOnStopInNoInterrupt,
    &HsmStateEntryCheckingPSMInterruptReferencesOnSuspend,
    &HsmStateEntryCheckingPSMInterruptReferencesOnSxInNoInterrupt,
    &HsmStateEntryCheckOverCurrentBit,
    &HsmStateEntryCompletingPDORequestForGetPortStatus,
    &HsmStateEntryCompletingPDORequestForGetPortStatusFromConfigWithIntTransfer,
    &HsmStateEntryCompletingPDORequestForGetPortStatusFromConfigWithIntTransferWithNoSuchDevice,
    &HsmStateEntryCompletingPDORequestForGetPortStatusInSuspended,
    &HsmStateEntryCompletingPDORequestForGetPortStatusWithNoSuchDevice,
    &HsmStateEntryConfiguredWithIntTransfer,
    &HsmStateEntryConfiguredWithoutInterruptTransfer,
    &HsmStateEntryConfiguring,
    &HsmStateEntryConfiguringHubAfterReset,
    &HsmStateEntryConfiguringHubAfterResetOnSystemResume,
    &HsmStateEntryCreatingChildPSMs,
    &HsmStateEntryErrorOnWaitingForOvercurrentClear,
    &HsmStateEntryGettingAdditionalConfigInfo,
    &HsmStateEntryGettingHubPortStatus,
    &HsmStateEntryGettingHubPortStatusForLostChanges,
    &HsmStateEntryGettingOverCurrentBitStatus,
    &HsmStateEntryGettingPortStatusOnPDORequest,
    &HsmStateEntryGettingPortStatusOnPDORequestFromConfigWithIntTransfer,
    &HsmStateEntryHasResetCountReachedMaximum,
    &HsmStateEntryHasResetPipeCountReachedMaximum,
    &HsmStateEntryHubStatusError,
    &HsmStateEntryInitializingResetCount,
    &HsmStateEntryIsItHubChange,
    &HsmStateEntryLoggingHubWasReset,
    &HsmStateEntryNotifyingHubResetToPortsOnReset,
    &HsmStateEntryNotifyingHubResetToPortsOnResetOnFatalError,
    &HsmStateEntryNotifyingHubResetToPortsOnResetOnResume,
    &HsmStateEntryNotifyingHubResumeInS0ToDevices,
    &HsmStateEntryNotifyingHubResumeToDevices,
    &HsmStateEntryNotifyingHubResumeWithResetToDevices,
    &HsmStateEntryNotifyingHubResumeWithResetToDevicesOnFailure,
    &HsmStateEntryNotifyingHubStopToDevices,
    &HsmStateEntryNotifyingHubStopToDevicesOnSuspriseRemove,
    &HsmStateEntryNotifyingHubStopToPorts,
    &HsmStateEntryNotifyingHubStopToPortsOnFatalError,
    &HsmStateEntryNotifyingHubStopToPortsOnReset,
    &HsmStateEntryQueryingForHubHackFlags,
    &HsmStateEntryQueueingFakeStatusChangeOnResumeInS0,
    &HsmStateEntryQueueingHubStatusForLostChanges,
    &HsmStateEntryQueueingPortStatusChangeEvents,
    &HsmStateEntryQueuingHubChange,
    &HsmStateEntryReConfiguring,
    &HsmStateEntryReportingErrorToPnp,
    &HsmStateEntryReportingReStartFailure,
    &HsmStateEntryReportingStartFailure,
    &HsmStateEntryResettingHub,
    &HsmStateEntryResettingHubOnResume,
    &HsmStateEntryResettingInterruptPipeOnFailure,
    &HsmStateEntrySendingAckForHubChange,
    &HsmStateEntrySendingInterruptTransfer,
    &HsmStateEntrySendingInterruptTransferAfterResettingPipe,
    &HsmStateEntrySignalingPnPPowerEventOnPendingStop,
    &HsmStateEntrySignalingPnPPowerEventOnStart,
    &HsmStateEntrySignalingPnPPowerEventOnStop,
    &HsmStateEntrySignallingPnpPowerEventForSuspended,
    &HsmStateEntrySignallingPnpPowerEventInUnconfigured,
    &HsmStateEntryStartingTimerForResetRetry,
    &HsmStateEntryStopped,
    &HsmStateEntrySupriseRemoved,
    &HsmStateEntrySuspended,
    &HsmStateEntrySuspendedInRecycle,
    &HsmStateEntrySuspendedWithPendingReset,
    &HsmStateEntryUnblockingResumeOnHubResetFailure,
    &HsmStateEntryUnblockResumeWaitingForStop,
    &HsmStateEntryUnconfigured,
    &HsmStateEntryWaitingForD0Entry,
    &HsmStateEntryWaitingForDevicesToAcquireReferenceOnStart,
    &HsmStateEntryWaitingForDevicesToReleaseReferenceOnD0ExitFinalFromReset,
    &HsmStateEntryWaitingForDevicesToReleaseReferenceOnSuspend,
    &HsmStateEntryWaitingForEnableInterruptsOnReset,
    &HsmStateEntryWaitingForEnableInterruptsOnStop,
    &HsmStateEntryWaitingForEnableInterruptsOnSx,
    &HsmStateEntryWaitingForInterruptReferencesReleasedOnFatalError,
    &HsmStateEntryWaitingForInterruptReferencesReleasedOnReset,
    &HsmStateEntryWaitingForInterruptReferencesReleasedOnStop,
    &HsmStateEntryWaitingForInterruptReferencesReleasedOnSuspend,
    &HsmStateEntryWaitingForOvercurrentToClear,
    &HsmStateEntryWaitingForPortsToAcquireReferenceOnResume,
    &HsmStateEntryWaitingForPortsToAcquireReferenceOnResumeInS0,
    &HsmStateEntryWaitingForPortsToAcquireReferenceOnResumeWithReset,
    &HsmStateEntryWaitingForPortsToAcquireReferenceOnStart,
    &HsmStateEntryWaitingForPortsToAcquireReferencePostReset,
    &HsmStateEntryWaitingForPortsToReleaseReferenceOnReset,
    &HsmStateEntryWaitingForPortsToReleaseReferenceOnResetOnResume,
    &HsmStateEntryWaitingForPortsToReleaseReferenceOnStop,
    &HsmStateEntryWaitingForPortsToReleaseReferenceOnSuspend,
    &HsmStateEntryWaitingForPSMInterruptReferencesOnReset,
    &HsmStateEntryWaitingForPSMInterruptReferencesOnStop,
    &HsmStateEntryWaitingForPSMInterruptReferencesOnSuspend,
    &HsmStateEntryWaitingForReleaseHardware,
    &HsmStateEntryWaitingForReleaseHardwareOnReStartFailure,
    &HsmStateEntryWaitingForReleaseHardwareOnStartFailure,
    &HsmStateEntryWaitingForResetRetryTimer,
    &HsmStateEntryWaitingForStop,
    &HsmStateEntryWaitingToBeDeleted,
    &HsmStateEntryCheckingBytesReturnedInHubConfigDescriptor,
    &HsmStateEntryCheckingIfHubDescriptorRetryCountExceededMax,
    &HsmStateEntryConfiguringAfterSetConfig,
    &HsmStateEntryGettingHubConfigurationDescriptorWithDefaultLength,
    &HsmStateEntryGettingHubConfigurationDescriptorWithReturnedLength,
    &HsmStateEntryGettingHubDescriptor,
    &HsmStateEntryGettingHubStatus,
    &HsmStateEntryInitializingHubDescriptorRetryCount,
    &HsmStateEntryReturningConfigurationFailure,
    &HsmStateEntryReturningConfigurationSuccess,
    &HsmStateEntrySettingHubConfiguration,
    &HsmStateEntryUpdatingHubInfoInUCX,
    &HsmStateEntryValidatingAndParsingHubDescriptor,
    &HsmStateEntryValidatingAndParsingHubPowerStatus,
    &HsmStateEntryValidatingHubConfigurationDescriptor,
    &HsmStateEntryWaitingForHubDescriptorRetryTimer,
    &HsmStateEntryGettingRootHub20PortsInfo,
    &HsmStateEntryGettingRootHub30PortsInfo,
    &HsmStateEntryGettingRootHubInfo,
    &HsmStateEntryReturningConfigurationFailureForRootHub,
    &HsmStateEntryReturningConfigurationSuccessForRootHub,
    &HsmStateEntryGettingAdditionalInfoFromParent,
    &HsmStateEntryReturningOperationFailure,
    &HsmStateEntryReturningOperationSuccess,
    &HsmStateEntryGettingHubDescriptorToCheckForPowerLoss,
    &HsmStateEntryReturningOperationFailureInCheckingForPowerLoss,
    &HsmStateEntryReturningOperationSuccessInCheckingForPowerLoss,
    &HsmStateEntryReturningOperationFailureInSettingHubDepth,
    &HsmStateEntryReturningOperationSuccessInSettingHubDepth,
    &HsmStateEntrySettingHubDepth,
};
