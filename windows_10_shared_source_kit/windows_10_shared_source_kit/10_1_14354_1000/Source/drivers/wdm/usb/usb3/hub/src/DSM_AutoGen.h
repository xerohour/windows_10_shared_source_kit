
/*++
Copyright (c) Microsoft Corporation

Module Name:

DSM_AutoGen.h

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/

#pragma once



//
// Enum of States in all the State Machines
//
typedef enum _DSM_STATE {
    DsmStateIgnored                                  = 1000,
    DsmStateEmptySubState                            = 1001,
    DsmStateNull                                     = 1002,
    DsmStateCompleteWithStatusFailed                 = 1003,
    DsmStateRequestPortCycle                         = 1004,
    DsmStateSignalQueryDeviceTextEvent               = 1005,
    DsmStateCompleteFDORequestWithStatusFailed       = 1006,
    DsmStateCompleteGetPortStatusWithInternalError   = 1007,
    DsmStateCompleteGetPortStatusWithNoSuchDevice    = 1008,
    DsmStateCompletePdoPreStart                      = 1009,
    DsmStateSignalPnpPowerEvent                      = 1010,
    DsmStateWaitingForPortAttach                     = 4000,
    DsmStateAckingD0ForFaultyClientDrivers           = 4001,
    DsmStateAckingHubStopAfterSuspendFromStoppedState = 4002,
    DsmStateAckingPdoPreStart                        = 4003,
    DsmStateAckingPdoPreStartFromStoppedEnabled      = 4004,
    DsmStateAckingQueryResourcesInConfiguredD0       = 4005,
    DsmStateAckingStopAfterSuspendOnDetach           = 4006,
    DsmStateAcquiringPowerReferenceOnHubOnAttach     = 4007,
    DsmStateAcquiringPowerReferenceOnHubStart        = 4008,
    DsmStateBugcheckingSystemOnBootDeviceEnumerationFailure = 4009,
    DsmStateCheckIfDeviceCanBeDisabledOnDriverNotFound = 4010,
    DsmStateCheckingForZeroBwPipeOnClientClearStall  = 4011,
    DsmStateCheckingForZeroBwPipeOnClientResetPipe   = 4012,
    DsmStateCheckingForZeroBwPipeOnClientSyncResetPipe = 4013,
    DsmStateCheckingIfBootDeviceOnD0Exit             = 4014,
    DsmStateCheckingIfBootDeviceOnDetach             = 4015,
    DsmStateCheckingIfBootDeviceOnReset              = 4016,
    DsmStateCheckingIfDeviceHasReceivedFirstStart    = 4017,
    DsmStateCheckingIfDeviceHasReceivedFirstStartInUncoonfigured = 4018,
    DsmStateCheckingIfDeviceShouldBeDisabled         = 4019,
    DsmStateCheckingIfItIsIsochronousEndpoint        = 4020,
    DsmStateCleaningUpAnyExistingConfigurationOnStart = 4021,
    DsmStateClearingEndpointHaltOnClientClearStall   = 4022,
    DsmStateClearingEndpointHaltOnClientResetPipe    = 4023,
    DsmStateClearingFailIoFlagOnSuccessfulRecovery   = 4024,
    DsmStateCompletingClientRequestInConfigured      = 4025,
    DsmStateCompletingClientRequestInConfiguredWithLastStatus = 4026,
    DsmStateCompletingClientRequestInConfiguredWithSpecialFile = 4027,
    DsmStateCompletingClientRequestInUnconfigured    = 4028,
    DsmStateCompletingClientRequestInUnConfiguredWithLastStatus = 4029,
    DsmStateCompletingClientRequestOnDetachInConfigured = 4030,
    DsmStateCompletingClientRequestOnDetachInDisabled = 4031,
    DsmStateCompletingClientRequestOnDetachWithDeviceProgrammed = 4032,
    DsmStateCompletingClientRequestOnFailureInConfigured = 4033,
    DsmStateCompletingClientRequestOnFailureInDisabled = 4034,
    DsmStateCompletingClientRequestOnFailureInUnconfigured = 4035,
    DsmStateCompletingClientUnconfigureRequest       = 4036,
    DsmStateCompletingClientUnconfigureRequestWithFailure = 4037,
    DsmStateCompletingD0EntryInConfigured            = 4038,
    DsmStateCompletingD0EntryInConfiguredWithSpecialFile = 4039,
    DsmStateCompletingD0EntryInConfiguredWithSpecialFileOnDetach = 4040,
    DsmStateCompletingD0EntryInConfiguredWithSpecialFileOnDetachWithDeviceEnabled = 4041,
    DsmStateCompletingD0EntryInUnconfigured          = 4042,
    DsmStateCompletingD0EntryOnDetachWithSpecialFile = 4043,
    DsmStateCompletingD0ExitAfterHubSuspend          = 4044,
    DsmStateCompletingD0ExitOnSuspendFailure         = 4045,
    DsmStateCompletingDeviceCleanup                  = 4046,
    DsmStateCompletingHubStopAfterSuspend            = 4047,
    DsmStateCompletingHubStopWithPortOff             = 4048,
    DsmStateCompletingPDOCleanupForUnknownDevice     = 4049,
    DsmStateCompletingPdoExitFinalOnDetach           = 4050,
    DsmStateCompletingPdoExitFinalOnDetachOnDeviceReportedMissing = 4051,
    DsmStateCompletingPdoReStartOnDetach             = 4052,
    DsmStateCompletingPdoReStartOnDetachWithDeviceEnabled = 4053,
    DsmStateCompletingPdoReStartOnOperationFailure   = 4054,
    DsmStateCompletingPnpEventOnDetachInConfigured   = 4055,
    DsmStateCompletingPnpEventOnDetachInDisabled     = 4056,
    DsmStateCompletingPnpEventOnDetachInUnConfigured = 4057,
    DsmStateCompletingPnpEventOnDetachWhileRenumeratingOnResume = 4058,
    DsmStateCompletingPnpEventOnFailureInDisabledWithPortOff = 4059,
    DsmStateCompletingPnpEventOnFailureWithEndpointsToBeDeleted = 4060,
    DsmStateConfiguredInD0                           = 4061,
    DsmStateConfiguredResumingOnHubResume            = 4062,
    DsmStateConfiguredSuspended                      = 4063,
    DsmStateConfiguredSuspendedOnHubSuspend          = 4064,
    DsmStateConfiguredSuspendedWithSpecialFile       = 4065,
    DsmStateConfiguredSuspending                     = 4066,
    DsmStateConfiguredSuspendingOnHubSuspend         = 4067,
    DsmStateConfiguredSuspendingWithSpecialFile      = 4068,
    DsmStateConfiguringDeviceFromCachedInformation   = 4069,
    DsmStateConfiguringDeviceFromCachedInformationOnHubResume = 4070,
    DsmStateConfiguringDeviceFromCachedInformationOnResume = 4071,
    DsmStateConfiguringDeviceFromCachedInformationOnResumeWithSpecialFile = 4072,
    DsmStateConfiguringDeviceFromCachedInformationWithSpecialFile = 4073,
    DsmStateConfiguringOnSelectConfiguration         = 4074,
    DsmStateCyclingPortOnRenumerationOfUnknownDevice = 4075,
    DsmStateD0EntryForUnknownDevice                  = 4076,
    DsmStateDeConfiguringDeviceOnClientRequest       = 4077,
    DsmStateDeConfiguringDeviceOnClientRequestFromUnconfigured = 4078,
    DsmStateDeletingDefaultEndpointAndDeviceOnDetachDuringEnum = 4079,
    DsmStateDeletingDeviceAndDefaultEndpointOnCleanup = 4080,
    DsmStateDeletingDeviceAndDefaultEndpointOnCleanupAfterDetach = 4081,
    DsmStateDeletingDeviceAndDefaultEndpointOnCleanupWithPortOff = 4082,
    DsmStateDeletingEndpoinstAndDeviceOnDetachOnCleanup = 4083,
    DsmStateDeletingEndpointsAndDeviceOnOnCleanup    = 4084,
    DsmStateDeporgrammingAllEndpointsOnCleanupFromEnumeratedConfigured = 4085,
    DsmStateDeporgrammingAllEndpointsOnHubStopFromEnumeratedConfigured = 4086,
    DsmStateDeprogrammingAllEndpointsOnHubSuspendFromEnumeratedConfigured = 4087,
    DsmStateDetachedAndDeprogrammedWithResetPending  = 4088,
    DsmStateDetachedWithResetPending                 = 4089,
    DsmStateDeviceDetachedAndDeporgrammedWithSpecialFile = 4090,
    DsmStateDeviceDetachedWithSpecialFile            = 4091,
    DsmStateDisablingBackPortInEnumeratedUnknown     = 4092,
    DsmStateDisablingBackPortInWaitingForD0EntryForFailedDevice = 4093,
    DsmStateDisablingDeviceAndDefaultEndpointInControllerOnDetachDuringEnum = 4094,
    DsmStateDisablingDeviceAndDefaultEndpointOnDetachOnHubStart = 4095,
    DsmStateDisablingDeviceAndDefaultEndpointOnEnumFailureOnHubStart = 4096,
    DsmStateDisablingDeviceAndDefaultEpInControllerOFailureWithPortOff = 4097,
    DsmStateDisablingDeviceAndDefaultEpInControllerOnCleanupWithPortOff = 4098,
    DsmStateDisablingDeviceAndDefaultEpInControllerOnD0ExitOnDetachWithPDOMissing = 4099,
    DsmStateDisablingDeviceAndDefaultEpInControllerOnDetach = 4100,
    DsmStateDisablingDeviceAndDefaultEpInControllerOnDetachOnHubStopWithPDOMissing = 4101,
    DsmStateDisablingDeviceAndDefaultEpInControllerOnDetachOnPDOCleaupWithPDOMissing = 4102,
    DsmStateDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspend = 4103,
    DsmStateDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspendOnDetach = 4104,
    DsmStateDisablingDeviceAndDefaultEpInControllerOnHubStopOnDetach = 4105,
    DsmStateDisablingDeviceInControllerOnCleanup     = 4106,
    DsmStateDisablingDeviceInControllerOnDetachOnCleanup = 4107,
    DsmStateDisablingDeviceInControllerOnEnumFailureOnPdoPreStart = 4108,
    DsmStateDisablingDeviceInControllerOnEnumFailureOnRestart = 4109,
    DsmStateDisablingDeviceInControllerOnHubStopFromStoppedEnabled = 4110,
    DsmStateDisablingDeviceInControllerOnHubStopInStopped = 4111,
    DsmStateDisablingDeviceInControllerOnHubStopInSuspended = 4112,
    DsmStateDisablingDeviceInControllerOnPDOStop     = 4113,
    DsmStateDisablingDeviceInControllerOnRenumerationWhileSuspended = 4114,
    DsmStateDisablingDeviceInControllerOnStartTimeout = 4115,
    DsmStateDisablingEndpointsInControllerOHubStopAfterSuspendOnDetach = 4116,
    DsmStateDisablingEndpointsInControllerOHubStopOnDetach = 4117,
    DsmStateDisablingEndpointsInControllerOnCleanupWithPortOff = 4118,
    DsmStateDisablingEndpointsInControllerOnClientRequest = 4119,
    DsmStateDisablingEndpointsInControllerOnD0ExitOnDetach = 4120,
    DsmStateDisablingEndpointsInControllerOnD0ExitOnDetachWithPDOMissing = 4121,
    DsmStateDisablingEndpointsInControllerOnDetachOnHubStopWithPDOMissing = 4122,
    DsmStateDisablingEndpointsInControllerOnDetachOnPDOCleanupWithPDOMissing = 4123,
    DsmStateDisablingEndpointsInControllerOnFailureWithPortOff = 4124,
    DsmStateDisablingEndpointsInControllerOnHubStopAfterSuspend = 4125,
    DsmStateDisablingEndpointsInControllerOnRenumerationWhileSuspended = 4126,
    DsmStateDisablingEndpointsInControllerOnStop     = 4127,
    DsmStateDisablingEndpointsOnDetachWithSpecialFile = 4128,
    DsmStateDisablingEndpointsOnDetachWithSpecialFileAndResetPending = 4129,
    DsmStateDisablingEndpointsOnFailureWithSpecialFile = 4130,
    DsmStateDisablingPortOnFailureInUnConfigured     = 4131,
    DsmStateDisablingPortOnHubStop                   = 4132,
    DsmStateDisablingPortOnHubStopFromStoppedEnabled = 4133,
    DsmStateDisablingPortOnPdoCleanup                = 4134,
    DsmStateDisablingPortOnPDOStop                   = 4135,
    DsmStateDisablingPortOnStartTimeout              = 4136,
    DsmStateEnumeratedAsFailedUnknown                = 4137,
    DsmStateEnumerating                              = 4138,
    DsmStateFlushingPD0D0ExitFinalInConfigured       = 4139,
    DsmStateFlushingPD0D0ExitFinalInUnConfigured     = 4140,
    DsmStateFowardingStreamsRequestToUcxInConfigured = 4141,
    DsmStateFowardingStreamsRequestToUcxInConfiguredSuspended = 4142,
    DsmStateFowardingStreamsRequestToUcxInConfiguredSuspendedWithSpecialFile = 4143,
    DsmStateFowardingStreamsRequestToUcxInDeviceDetachedAndDeporgrammedWithSpecialFile = 4144,
    DsmStateFowardingStreamsRequestToUcxInDeviceDetachedWithSpecialFile = 4145,
    DsmStateFowardingStreamsRequestToUcxInStopEnabled = 4146,
    DsmStateFowardingStreamsRequestToUcxInStopped    = 4147,
    DsmStateFowardingStreamsRequestToUcxInStoppedWithHubSuspended = 4148,
    DsmStateFowardingStreamsRequestToUcxInUnconfiguredInD0 = 4149,
    DsmStateFowardingStreamsRequestToUcxInUnconfiguredSuspended = 4150,
    DsmStateFowardingStreamsRequestToUcxInWaitingForDetachorResetWithDeviceDeprogrammed = 4151,
    DsmStateFowardingStreamsRequestToUcxInWaitingForDetatchorReset = 4152,
    DsmStateGettingMatchingPipeHandleOnClientClearStall = 4153,
    DsmStateGettingMatchingPipeHandleOnClientResetPipe = 4154,
    DsmStateGettingMatchingPipeHandleOnClientSyncResetPipe = 4155,
    DsmStateIssuingDeviceDetachedToPSMOnDetach       = 4156,
    DsmStateIssuingPortCycleAndCompletingClientRequestOnEnumFailue = 4157,
    DsmStateIssuingPortCycleAndSingalingPnpEventOnEnumFailueFromConfigured = 4158,
    DsmStateIssuingPortCycleOnEnumFailureOnHubResume = 4159,
    DsmStateIssuingPortCycleOnFailueWithEndpointsToBeDeleted = 4160,
    DsmStateNotifyingDeviceDisconnectedOnDetach      = 4161,
    DsmStateNotifyingDeviceDisconnectedOnDetachWithSpecialFile = 4162,
    DsmStatePreparingEndpointAndInterfaceListsOnClientReset = 4163,
    DsmStatePreparingEndpointAndInterfaceListsOnClientResetWithSpecialFile = 4164,
    DsmStatePreparingEndpointAndInterfaceListsOnResetAfterHubResume = 4165,
    DsmStatePreparingEndpointAndInterfaceListsOnResetAfterResume = 4166,
    DsmStatePreparingEndpointAndInterfaceListsOnResetAfterResumeWithSpecialFile = 4167,
    DsmStatePreparingEndpointAndInterfaceListsOnSelectConfiguration = 4168,
    DsmStatePreparingEndpointAndInterfaceListsOnSelectConfigurationFromConfigured = 4169,
    DsmStatePurgingDeviceIoOnDetachInConfigured      = 4170,
    DsmStatePurgingDeviceIoOnHubSuspend              = 4171,
    DsmStatePurgingDeviceIoOnHubSuspendFromSuspending = 4172,
    DsmStatePurgingDeviceIoOnPrepareForHibernate     = 4173,
    DsmStateQueryingDescriptorForFdoRequestFromConfigured = 4174,
    DsmStateQueryingDescriptorForFdoRequestFromStoppedEnumerated = 4175,
    DsmStateQueryingDescriptorForFdoRequestFromUnConfigured = 4176,
    DsmStateQueryingDeviceTextStringFromConfigured   = 4177,
    DsmStateQueryingDeviceTextStringFromStoppedEnumerated = 4178,
    DsmStateQueryingDeviceTextStringFromUnConfigured = 4179,
    DsmStateQueueingAttachFailureToPort              = 4180,
    DsmStateQueueingAttachSuccessToPort              = 4181,
    DsmStateQueueingDeviceDetachedToPsmForBootDeviceInUnconfigured = 4182,
    DsmStateQueueingDeviceDetachedToPsmForBootDeviceWithResetPending = 4183,
    DsmStateQueueingDeviceDetachedToPsmForBootDeviceWithResetPendingAndDeviceDeprogrammed = 4184,
    DsmStateQueueingPrepareForHiberToPSM             = 4185,
    DsmStateReDisablingPortAfterHubResume            = 4186,
    DsmStateReEnumeratingOnClientRequestInConfigured = 4187,
    DsmStateReEnumeratingOnClientRequestInConfiguredWithSpecialFile = 4188,
    DsmStateReEnumeratingOnClientRequestInUnconfigured = 4189,
    DsmStateReEnumeratingOnClientRequestOnReattach   = 4190,
    DsmStateReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed = 4191,
    DsmStateReEnumeratingOnHubResumeInConfigured     = 4192,
    DsmStateReEnumeratingOnHubResumeInUnConfigured   = 4193,
    DsmStateReEnumeratingOnHubStart                  = 4194,
    DsmStateReEnumeratingOnPdoPreStartWhileEnabled   = 4195,
    DsmStateReEnumeratingOnPdoStart                  = 4196,
    DsmStateReEnumeratingOnPdoStartWhileEnabled      = 4197,
    DsmStateReEnumeratingOnResumeInConfigured        = 4198,
    DsmStateReEnumeratingOnResumeInConfiguredWithSpecialFile = 4199,
    DsmStateReEnumeratingOnResumeInUnConfigured      = 4200,
    DsmStateRegisteringWithHSM                       = 4201,
    DsmStateReleasingDevicePowerReferenceOnStopAfterDetach = 4202,
    DsmStateReleasingPowerReferenceOnDetachInConfigured = 4203,
    DsmStateReleasingPowerReferenceOnHubOnDetach     = 4204,
    DsmStateReleasingPowerReferenceOnHubOnDetachOnEnumSuccess = 4205,
    DsmStateReleasingPowerReferenceOnHubOnEnumerationFailure = 4206,
    DsmStateReleasingPowerReferenceOnHubStopSuspend  = 4207,
    DsmStateReleasingPowerReferenceOnHubSuspendStopOnDetach = 4208,
    DsmStateReleasingPowerReferenceOnPortFailureInHubStopSuspend = 4209,
    DsmStateReleasingPowerReferenceOnSuspendFailureInConfigured = 4210,
    DsmStateReleasingPowerReferenceOnSuspendFailureInUnConfigured = 4211,
    DsmStateRemovingDeviceInfoFromGlobalListOnCleanup = 4212,
    DsmStateRemovingDeviceInfoFromGlobalListOnCleanupOnDetachInConfigured = 4213,
    DsmStateRemovingDeviceInfoFromGlobalListOnCleanupOnDetachInUnConfigured = 4214,
    DsmStateRemovingDeviceInfoFromGlobalListOnCleanUpOnHubStopOnDetach = 4215,
    DsmStateRemovingDeviceInfoFromGlobalListOnDeviceReportedMissing = 4216,
    DsmStateRemovingDeviceInfoFromGlobalListOnReportedMissingOnHubStopOnDetach = 4217,
    DsmStateReportingDeviceMissing                   = 4218,
    DsmStateReportingDeviceToPnp                     = 4219,
    DsmStateReportingFailedDeviceAsMissing           = 4220,
    DsmStateReportingFailedUnknownDeviceToPnp        = 4221,
    DsmStateRequestingCyclePortAndCompletingPnpEventOnFailureInUnconfigured = 4222,
    DsmStateRequestingPortCycleOnErrorFromConfigured = 4223,
    DsmStateResettingPipeInUCXOnClientReset          = 4224,
    DsmStateResettingPipeInUCXOnClientSyncResetPipe  = 4225,
    DsmStateResumingOnHubResumeInStopped             = 4226,
    DsmStateSettingConfigurationInformationInClientRequest = 4227,
    DsmStateSettingDeviceInterface                   = 4228,
    DsmStateSettingExitLatencyAdjustmentOnNoPingResponseError = 4229,
    DsmStateSettingFailIoFlagOnClientReset           = 4230,
    DsmStateSettingForceResetOnRenumFlag             = 4231,
    DsmStateSettingInterfaceInformationInClientRequest = 4232,
    DsmStateSettingLPMValuesInConfiguredOnSettingChange = 4233,
    DsmStateSettingLPMValuesInStoppedEnabled         = 4234,
    DsmStateSettingLPMValuesInUnConfiguredOnSettingChange = 4235,
    DsmStateSignalingPnpPowerEventOnDetachDuringRenumOnRestart = 4236,
    DsmStateSignalingPnpPowerEventOnStop             = 4237,
    DsmStateSignalingPnpPowerEventOnStopEnable       = 4238,
    DsmStateSignallingPnpEventOnHubSuspendFromUnconfiguredSuspending = 4239,
    DsmStateStartingDeviceIoOnHubResume              = 4240,
    DsmStateStopped                                  = 4241,
    DsmStateStoppedEnabled                           = 4242,
    DsmStateStoppedEnumerated                        = 4243,
    DsmStateStoppedEnumeratedConfiguredForFaultyClients = 4244,
    DsmStateStoppedEnumeratedWithHubSuspended        = 4245,
    DsmStateStoppedWithHubSuspended                  = 4246,
    DsmStateSuspendingOnHubSuspendInStopped          = 4247,
    DsmStateUnConfiguredInD0                         = 4248,
    DsmStateUnConfiguredResumingOnHubResume          = 4249,
    DsmStateUnconfiguredSuspended                    = 4250,
    DsmStateUnConfiguredSuspendedOnHubSuspend        = 4251,
    DsmStateUnconfiguredSuspending                   = 4252,
    DsmStateUnConfiguredSuspendingOnHubSuspend       = 4253,
    DsmStateUnregisteringWithHSMOnDetachAfterCleanup = 4254,
    DsmStateUnregisteringWithHsmOnDetachOnDeviceReportedMissing = 4255,
    DsmStateUnregsiteringWithHsmOnDetach             = 4256,
    DsmStateUnregsiteringWithHSMOnDetachOnPDOCleaupWithPDOMissing = 4257,
    DsmStateValidatingSelectConfigUrbFromConfigured  = 4258,
    DsmStateValidatingSelectConfigUrbFromUnConfigured = 4259,
    DsmStateValidatingSelectInterfaceUrbFromConfigured = 4260,
    DsmStateWaitingForD0EntryForFailedDevice         = 4261,
    DsmStateWaitingForDetachOrRemove                 = 4262,
    DsmStateWaitingForDetachOrRemoveInConfigured     = 4263,
    DsmStateWaitingForDetachOrRemoveInDisabledWithPortOff = 4264,
    DsmStateWaitingForDetachOrRemoveInUnConfigured   = 4265,
    DsmStateWaitingForDetachOrRemoveWithPortOff      = 4266,
    DsmStateWaitingForDetachOrReset                  = 4267,
    DsmStateWaitingForDetachOrResetWithDeviceDeporgrammed = 4268,
    DsmStateWaitingForDetachOrToBeDeleted            = 4269,
    DsmStateWaitingForDetachOrToBeDeletedWithPortOff = 4270,
    DsmStateWaitingForDisableCompleteOnHubStopFromStoppedEnabled = 4271,
    DsmStateWaitingForPDOD0ExitOnDetachInConfigured  = 4272,
    DsmStateWaitingForPDOD0ExitOnDetachInUnConfigured = 4273,
    DsmStateWaitingForPDOReportedMissing             = 4274,
    DsmStateWaitingForPortResumeResponseOnStop       = 4275,
    DsmStateWaitingForPortResumeResponseOnSuspend    = 4276,
    DsmStateWaitingForRemoveOnDetach                 = 4277,
    DsmStateWaitingForRemoveOrReprotedMissingOnHubStopOnDetach = 4278,
    DsmStateWaitingForReportedMissingOnDetachInConfigured = 4279,
    DsmStateWaitingForReportedMissingOnDetachInUnConfigured = 4280,
    DsmStateWaitingToBeDeleted                       = 4281,
    DsmStateQueryingLanguageSpecificProductIdString  = 4282,
    DsmStateReturningOperationSuccessInQueryingProductId = 4283,
    DsmStateSignalingDeviceQueryTextEvent            = 4284,
    DsmStateValidatingLanguageSpecificProductIdString = 4285,
    DsmStateAcquiringAddressZeroOwnershipInEnum      = 4286,
    DsmStateAcquiringPowerReferenceOnStartInEnum     = 4287,
    DsmStateCancellingAcquiringAddress0OnDetachInEnum = 4288,
    DsmStateCancellingRetryTimerOnDetachInEnum       = 4289,
    DsmStateCancellingRetryTimerOnStopSuspendInEnum  = 4290,
    DsmStateCheckingIfDevicePDChargingPolicySupported = 4291,
    DsmStateCheckingIfEnumRetryReachedMaximumInEnum  = 4292,
    DsmStateCheckingIfEnumRetryReachedMaximumInEnumWithAddr0Ownership = 4293,
    DsmStateClearingDeviceProgrammingLostFlagOnEnum  = 4294,
    DsmStateDeletingDefaultEndpointAndDeviceFromUCXOnDetachInEnumeratingAfterAddr = 4295,
    DsmStateDeletingDefaultEndpointAndDeviceFromUCXOnFailureInEnum = 4296,
    DsmStateDeletingDefaultEndpointAndDeviceOnPostAddressFailureInEnum = 4297,
    DsmStateDeletingDefaultEndpointAndDeviceOnStopSuspendAfterAddressInEnum = 4298,
    DsmStateDeletingDeviceAndDefaultEndpointOnDetachWithAddress0OwnershipInEnum = 4299,
    DsmStateDeletingDeviceAndDefaultEndpointOnStopSuspendWithAddress0OwnershipInEnum = 4300,
    DsmStateDisablingDeviceInControllerOnDetachInEnumeratingAfterAddr = 4301,
    DsmStateDisablingDeviceInControllerOnDetachWithAddress0OwnershipInEnum = 4302,
    DsmStateDisablingDeviceInControllerOnPostAddressEnumFailureInEnum = 4303,
    DsmStateDisablingDeviceInControllerOnSetAddressFailureInEnum = 4304,
    DsmStateDisablingDeviceInControllerOnStopSuspendAfterAddressInEnum = 4305,
    DsmStateDisablingDeviceInControllerOnStopSuspendWithAddress0OwnershipInEnum = 4306,
    DsmStateDisablingOnEnumAfterFailureInEnum        = 4307,
    DsmStateDisablingOnEnumAfterFailureInEnumWithAddress0Ownership = 4308,
    DsmStateDisablingOnStopSuspendInEnum             = 4309,
    DsmStateDisablingOnStopSuspendInEnumWithAddress0Ownership = 4310,
    DsmStateDisablingOnStopSuspendOnIoctlFailureInEnum = 4311,
    DsmStateEnumeratingAfterAddressInEnum            = 4312,
    DsmStateEnumeratingAtAddressZeroInEnum           = 4313,
    DsmStateInitializingEnumRetryCountInEnum         = 4314,
    DsmStateReleasingAddressZeroOwnershipInEnum      = 4315,
    DsmStateReleasingAddressZeroOwnershipOnDetach    = 4316,
    DsmStateReleasingAddressZeroOwnershipOnDetachInEnum = 4317,
    DsmStateReleasingAddressZeroOwnershipOnEnumFailure = 4318,
    DsmStateReleasingAddressZeroOwnershipOnStopSuspend = 4319,
    DsmStateReleasingAddressZeroOwnershipOnStopSuspendInEnum = 4320,
    DsmStateReleasingPowerReferenceInEnum            = 4321,
    DsmStateReturningOperationFailureInEnum          = 4322,
    DsmStateReturningOperationSuccessInEnum          = 4323,
    DsmStateReturningPortDetachDeviceInEnum          = 4324,
    DsmStateSettingDevicePDChargingPolicy            = 4325,
    DsmStateSettingLPMValues                         = 4326,
    DsmStateSettingNonZeroAddressInEnum              = 4327,
    DsmStateStartingTimerForEnumRetryInEnum          = 4328,
    DsmStateStartingTimerForEnumRetryInEnumWithAddr0Ownership = 4329,
    DsmStateStoppedSuspendedInEnum                   = 4330,
    DsmStateUpdatingDeviceInformationInEnum          = 4331,
    DsmStateWaitingForDisableCompleteOnStopSuspendAfterFailureInEnum = 4332,
    DsmStateWaitingForDisableCompleteOnStopSuspendInEnum = 4333,
    DsmStateWaitingForDisableCompleteOnStopSuspendInEnumWithAddress0Ownership = 4334,
    DsmStateWaitingForTimerToFlushOnDetachInEnum     = 4335,
    DsmStateWaitingForTimerToFlushOnStopSuspendInEnum = 4336,
    DsmStateWaitingForUCXIoctlOnDetachInEnum         = 4337,
    DsmStateWaitingForUCXIoctlOnStopSuspendInEnum    = 4338,
    DsmStateCheckingIfFirstEnumTryAfterReset1        = 4339,
    DsmStateCheckingIfFirstEnumTryAfterReset2        = 4340,
    DsmStateCheckingIfSecondResetIsNeeded            = 4341,
    DsmStateCreatingUCXDefaultEndpoint               = 4342,
    DsmStateCreatingUCXDevice                        = 4343,
    DsmStateDeletingUCXDefaultEndpointAndDeviceOnDetach = 4344,
    DsmStateDeletingUCXDefaultEndpointAndDeviceOnStop = 4345,
    DsmStateDeletingUCXDefaultEndpointAndDeviceOnSuspend = 4346,
    DsmStateDeletingUCXDefaultEndpointOnOperationFailure = 4347,
    DsmStateDeletingUCXDeviceOnOperationFailure      = 4348,
    DsmStateDisablingDeviceInUCXOnDetach             = 4349,
    DsmStateDisablingDeviceInUCXOnOperationFailure   = 4350,
    DsmStateDisablingDeviceInUCXOnStop               = 4351,
    DsmStateDisablingDeviceInUCXOnSuspend            = 4352,
    DsmStateDisablingPortOnHubStopInEnumAtAddr0      = 4353,
    DsmStateDisablingPortOnHubSuspendInEnumAtAddr0   = 4354,
    DsmStateEnablingDeviceInUCX                      = 4355,
    DsmStateGettingDeviceDescriptorInEnumAtZero      = 4356,
    DsmStateLoggingReset1TimeoutInEnumAtZero         = 4357,
    DsmStateLoggingReset2TimeoutInEnumAtZero         = 4358,
    DsmStateNotifyingDeviceResetToUCXAfterReset2     = 4359,
    DsmStateResetting1                               = 4360,
    DsmStateResetting2                               = 4361,
    DsmStateReturningDeviceOperationFailureInEnumAtZero = 4362,
    DsmStateReturningDeviceOperationSuccessInEnumAtZero = 4363,
    DsmStateReturningHubStopInEnumAtZero             = 4364,
    DsmStateReturningHubSuspendInEnumAtZero          = 4365,
    DsmStateReturningPortDetachDeviceInEnumAtZero    = 4366,
    DsmStateSettingSpeedFlagFor20Devices             = 4367,
    DsmStateStoppingTimerOnHubStopInEnumAtZero       = 4368,
    DsmStateStoppingTimerOnHubSuspendInEnumAtZero    = 4369,
    DsmStateStoppingTimerOnPortDetachInEnumAtZero    = 4370,
    DsmStateUpdatingDefaultEndpointMaxPacketSizeInEnum = 4371,
    DsmStateValidatingDeviceDescriptorInEnumAtZero   = 4372,
    DsmStateWaitingForPortReset1OrDisableOnHubStopInEnumAtZero = 4373,
    DsmStateWaitingForPortReset1OrDisableOnHubSuspendInEnumAtZero = 4374,
    DsmStateWaitingForPortReset2OnHubStopInEnumAtZero = 4375,
    DsmStateWaitingForPortReset2OnHubSuspendInEnumAtZero = 4376,
    DsmStateWaitingForPostReset1ExtendedTimer        = 4377,
    DsmStateWaitingForPostReset1Timer                = 4378,
    DsmStateWaitingForPostReset2ExtendedTimer        = 4379,
    DsmStateWaitingForPostReset2Timer                = 4380,
    DsmStateWaitingForStopSuspendOnReset1InEnumAtZero = 4381,
    DsmStateWaitingForStopSuspendOnReset2InEnumAtZero = 4382,
    DsmStateWaitingForTimerOnHubStopInEnumAtZero     = 4383,
    DsmStateWaitingForTimerOnHubSuspendInEnumAtZero  = 4384,
    DsmStateWaitingForTimerOnPortDetachInEnumAtZero  = 4385,
    DsmStateCreatingUCXDefaultEndpointFor30          = 4386,
    DsmStateCreatingUCXDevice30                      = 4387,
    DsmStateDeletingUCXDefaultEndpointOnOperationFailureFor30 = 4388,
    DsmStateDeletingUCXDefaultEndpointOnOperationFailureFor30OnDetach = 4389,
    DsmStateDeletingUCXDefaultEndpointOnOperationFailureFor30OnHubStop = 4390,
    DsmStateDeletingUCXDefaultEndpointOnOperationFailureFor30OnHubSuspend = 4391,
    DsmStateDeletingUCXDeviceOnOperationFailureFor30 = 4392,
    DsmStateDeletingUCXDeviceOnOperationFailureFor30OnDetach = 4393,
    DsmStateDeletingUCXDeviceOnOperationFailureFor30OnHubStop = 4394,
    DsmStateDeletingUCXDeviceOnOperationFailureFor30OnHubSuspend = 4395,
    DsmStateEnablingDeviceInUCXFor30                 = 4396,
    DsmStateResettingDeviceInEnumeration             = 4397,
    DsmStateReturningDetachInPreAddressEnumFor30     = 4398,
    DsmStateReturningDeviceOperationFailureInPreAddressEnumFor30 = 4399,
    DsmStateReturningDeviceOperationSuccessInPreAddressEnumFor30 = 4400,
    DsmStateReturningHubStopInPreAddressEnumFor30    = 4401,
    DsmStateReturningHubSuspendInPreAddressEnumFor30 = 4402,
    DsmStateSetting30Speed                           = 4403,
    DsmStateCancellingControlTransferOnDetachInEnumAfterAddr = 4404,
    DsmStateCancellingControlTransferOnHubStopInEnumAfterAddr = 4405,
    DsmStateCancellingControlTransferOnHubSuspendInEnumAfterAddr = 4406,
    DsmStateCheckingBytesReturnedInConfigDescriptor  = 4407,
    DsmStateCheckingIfAltEnumCmdCached               = 4408,
    DsmStateCheckingIfSuperSpeedNeedsToBeDisabled    = 4409,
    DsmStateDiasablingSuperSpeed                     = 4410,
    DsmStateGettingConfigurationDescriptorWithDefaultLength = 4411,
    DsmStateGettingConfigurationDescriptorWithReturnedLength = 4412,
    DsmStateGettingDeviceDescriptorAfterAddressing   = 4413,
    DsmStateGettingRemainingDescriptorsAfterConfigDescriptor = 4414,
    DsmStateQueryingRegistryValuesForDevice          = 4415,
    DsmStateReturningHubStopInEnumAfterAddr          = 4416,
    DsmStateReturningHubSuspendInEnumAfterAddr       = 4417,
    DsmStateReturningOperationFailureInEnumAfterAddr = 4418,
    DsmStateReturningOperationSuccessInEnumAfterAddr = 4419,
    DsmStateReturningPortDeviceDetachInEnumAfterAddr = 4420,
    DsmStateSendingAltEnumCmdInEnumAfterAddr         = 4421,
    DsmStateSettingVersionFlagInEnumerating          = 4422,
    DsmStateValidatingConfigurationDescriptor        = 4423,
    DsmStateValidatingDeviceDescriptorAfterAddressing = 4424,
    DsmStateCancellingControlTransferOnDetachInRemainingDescriptors = 4425,
    DsmStateCancellingControlTransferOnHubStopInRemainingDescriptors = 4426,
    DsmStateCancellingControlTransferOnHubSuspendInRemainingDescriptors = 4427,
    DsmStateCheckingIfIProductIdStringDescriptorShouldBeQueried = 4428,
    DsmStateCheckingIfIProductIsZero                 = 4429,
    DsmStateGettingBosDescriptorSet                  = 4430,
    DsmStateGettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch = 4431,
    DsmStateGettingLanguageIdStringDescriptor        = 4432,
    DsmStateGettingMSOSAndSerialNumberDescriptor     = 4433,
    DsmStateGettingProductIdStringDescriptor         = 4434,
    DsmStateReturningErrorResponseOnLanguageIdQuery  = 4435,
    DsmStateReturningErrorResponseOnProductStringQuery = 4436,
    DsmStateReturningHubStopInRemainingDescriptors   = 4437,
    DsmStateReturningHubSuspendInRemainingDescriptors = 4438,
    DsmStateReturningOperationFailureInRemainingDescriptors = 4439,
    DsmStateReturningOperationSuccessInRemainingDescriptors = 4440,
    DsmStateReturningPortDeviceDetachInRemainingDescriptors = 4441,
    DsmStateValidatingLanguageIdStringDescriptor     = 4442,
    DsmStateValidatingProductIdStringDescriptor      = 4443,
    DsmStateCheckingIfDeviceSupportsContainerId      = 4444,
    DsmStateCheckingIfIgnoreHWSerNumIsSet            = 4445,
    DsmStateCheckingIfMSOSContainerIdDescriptorIsSupported = 4446,
    DsmStateCheckingIfMSOSDescriptorShouldBeQueried  = 4447,
    DsmStateCheckingIfMSOSExtendedConfigDescriptorIsSupported = 4448,
    DsmStateCheckingIfSerialNumberStringIndexIsZero  = 4449,
    DsmStateGettingMSOSContainerIdDescriptor         = 4450,
    DsmStateGettingMSOSContainerIdHeaderDescriptor   = 4451,
    DsmStateGettingMSOSDescriptor                    = 4452,
    DsmStateGettingMSOSExtendedDescriptor            = 4453,
    DsmStateGettingMSOSExtendedDescriptorHeader      = 4454,
    DsmStateGettingSerialNumberStringDescriptor      = 4455,
    DsmStateMarkDeviceAsNotSupportingMSOSDescriptor  = 4456,
    DsmStateMarkDeviceAsSupportingMSOSDescriptor     = 4457,
    DsmStateMarkingDeviceAsNotSupportingContainerId  = 4458,
    DsmStateMarkingDeviceAsNotSupportingContainerIdBasedOnMSOSDescriptor = 4459,
    DsmStateReturningErrorResponseOnContainerIdHeaderQuery = 4460,
    DsmStateReturningErrorResponseOnContainerIdQuery = 4461,
    DsmStateReturningErrorResponseOnMSOSExtendedHeaderQuery = 4462,
    DsmStateReturningErrorResponseOnMSOSExtendedQuery = 4463,
    DsmStateReturningErrorResponseOnMSOSQuery        = 4464,
    DsmStateReturningErrorResponseOnSerialNumberQuery = 4465,
    DsmStateReturningOperationFailureInGettingDescriptorsForGreaterThan1x = 4466,
    DsmStateReturningOperationSuccessInGettingDescriptorsForGreaterThan1x = 4467,
    DsmStateValidatingMSOSContainerIdDescriptor      = 4468,
    DsmStateValidatingMSOSContainerIdHeaderDescriptor = 4469,
    DsmStateValidatingMSOSDescriptor                 = 4470,
    DsmStateValidatingMSOSExtendedConfigDescriptor   = 4471,
    DsmStateValidatingMSOSExtendedConfigDescriptorHeader = 4472,
    DsmStateValidatingSerialNumberStringDescriptor   = 4473,
    DsmStateCheckingIfIgnoreHWSerNumIsSetFor1xDevice = 4474,
    DsmStateCheckingIfSerialNumberStringIndexIsZeroFor1xDevice = 4475,
    DsmStateGettingSerialNumberStringDescriptorFor1xDevice = 4476,
    DsmStateReturningErrorResponseOnSerialNumberQueryFor1xDevice = 4477,
    DsmStateReturningOperationFailureInGettingSerialNumberFor1x = 4478,
    DsmStateReturningOperationSuccessInGettingSerialNumberFor1x = 4479,
    DsmStateValidatingSerialNumberStringDescriptorFor1xDevice = 4480,
    DsmStateCheckingIfWaitRequiredAfterSetAddress    = 4481,
    DsmStateReturningHubStopInSettingAddress         = 4482,
    DsmStateReturningHubSuspendInSettingAddress      = 4483,
    DsmStateReturningOperationFailureInSettingAddress = 4484,
    DsmStateReturningOperationSuccessInSettingAddress = 4485,
    DsmStateReturningPortDetachDeviceInSettingAddress = 4486,
    DsmStateSettingNonZeroAddress                    = 4487,
    DsmStateStoppingTimerOnHubStopInSettingAddress   = 4488,
    DsmStateStoppingTimerOnHubSuspendInSettingAddress = 4489,
    DsmStateStoppingTimerOnPortDetachInSettingAddress = 4490,
    DsmStateWaitingForTimerAfterSetAddress           = 4491,
    DsmStateWaitingForTimerOnHubStopInSettingAddress = 4492,
    DsmStateWaitingForTimerOnHubSuspendInSettingAddress = 4493,
    DsmStateWaitingForTimerOnPortDetachInSettingAddress = 4494,
    DsmStateCheckingBytesReturnedInAlternateConfigDescriptor = 4495,
    DsmStateCheckingIfAltEnumCommandNeeded           = 4496,
    DsmStateCheckingIfAlternateBOSDescriptorQueryShouldBeSkipped = 4497,
    DsmStateCheckingIfAlternateModeStringDescriptorShouldBeQueried = 4498,
    DsmStateCheckingIfBillboardBOSDescriptorIsPresent = 4499,
    DsmStateCheckingIfBillboardStringDescriptorShouldBeQueried = 4500,
    DsmStateCheckingIfBOSDescriptorQueryShouldBeSkipped = 4501,
    DsmStateCheckingIfCompleteAlternateBOSDescriptorWasRetrieved = 4502,
    DsmStateCheckingIfCompleteBOSDescriptorWasRetrieved = 4503,
    DsmStateCheckingIfDualRoleFeaturesSupported      = 4504,
    DsmStateGetMsOs20DescriptorSet                   = 4505,
    DsmStateGettingAlternateBOSDescriptor            = 4506,
    DsmStateGettingAlternateBOSDescriptorHeader      = 4507,
    DsmStateGettingAlternateConfigurationDescriptorWithDefaultLength = 4508,
    DsmStateGettingAlternateConfigurationDescriptorWithReturnedLength = 4509,
    DsmStateGettingAlternateDeviceDescriptor         = 4510,
    DsmStateGettingAlternateModeStringDescriptor     = 4511,
    DsmStateGettingBillboardStringDescriptor         = 4512,
    DsmStateGettingBOSDescriptor                     = 4513,
    DsmStateGettingBOSDescriptorHeader               = 4514,
    DsmStateIsMsOs20DescriptorSupported              = 4515,
    DsmStateQueryingRegistryValuesForAlternateDeviceEnumeration = 4516,
    DsmStateReturningErrorResponseOnBOSDescriptorHeaderQuery = 4517,
    DsmStateReturningErrorResponseOnBOSDescriptorQuery = 4518,
    DsmStateReturningOperationFailureInGettingBOSDescriptor = 4519,
    DsmStateReturningOperationSuccessInGettingBOSDescriptor = 4520,
    DsmStateSendingMsOs20AlternateEnumerationCommand = 4521,
    DsmStateSendingUsbFeaturesVendorCmd              = 4522,
    DsmStateSettingVersionFlagInAlternateEnumeration = 4523,
    DsmStateValidatingAlternateBOSDescriptor         = 4524,
    DsmStateValidatingAlternateBOSDescriptorHeader   = 4525,
    DsmStateValidatingAlternateConfigurationDescriptor = 4526,
    DsmStateValidatingAlternateDeviceDescriptor      = 4527,
    DsmStateValidatingAndLoggingAlternateModeStringDescriptor = 4528,
    DsmStateValidatingAndLoggingBillboardStringDescriptor = 4529,
    DsmStateValidatingBOSDescriptor                  = 4530,
    DsmStateValidatingBOSDescriptorHeader            = 4531,
    DsmStateValidatingMsOs20DescriptorSetIfPresent   = 4532,
    DsmStateGettingDeviceQualifierDescriptor         = 4533,
    DsmStateReturningErrorResponseOnDeviceQualifierQuery = 4534,
    DsmStateReturningOperationSuccessInCheckingFor20FullSpeed = 4535,
    DsmStateValidatingDeviceQualifierDescriptor      = 4536,
    DsmStateAddingDeviceToGlobalChildList            = 4537,
    DsmStateCheckingIfAddDeviceToGlobalListRetryCountExceededLimit = 4538,
    DsmStateCreatingChildPDOAndReportingToPnp        = 4539,
    DsmStateDiscardSerialNumber                      = 4540,
    DsmStateInitializingAddDeviceToGlobalListRetryCount = 4541,
    DsmStateRequestingDeviceCycleInReportingToPnp    = 4542,
    DsmStateReturningOperationSuccessInReportingToPnp = 4543,
    DsmStateReturningPortDetachDevice                = 4544,
    DsmStateStoppingTimerAndRequestingCycleOnStopSuspend = 4545,
    DsmStateStoppingTimerOnDetachInReportingToPnp    = 4546,
    DsmStateWaitingForDetachInReportingToPnp         = 4547,
    DsmStateWaitingForDetachOrTimerInReportingToPnp  = 4548,
    DsmStateWaitingForDuplicateDeviceToGoAway        = 4549,
    DsmStateWaitingForTimerToFlushOnDetachInReportingToPnp = 4550,
    DsmStateCreatingUnknownChildPDOAndReportingToPnp = 4551,
    DsmStateMarkingUnknownDeviceAsFailed             = 4552,
    DsmStateRequestingDeviceCycleForUnknownDevice    = 4553,
    DsmStateReturningOperationSuccessInReportingUnknownDevice = 4554,
    DsmStateReturningPortDetachDeviceInReportingUnknownDevice = 4555,
    DsmStateAckingResumed                            = 4556,
    DsmStateAcquiringPowerReferenceInEnabled         = 4557,
    DsmStateAcquiringPowerReferenceInSuspended       = 4558,
    DsmStateAcquiringPowerReferenceInSuspendedOnResumeWithReset = 4559,
    DsmStateAcquiringPowerReferenceOnHubResume       = 4560,
    DsmStateAcquiringPowerReferenceOnHubResumeWithReenumRequired = 4561,
    DsmStateAcquiringPowerReferenceOnHubResumeWtihReset = 4562,
    DsmStateAcquiringPowerReferenceOnResumeInS0      = 4563,
    DsmStateCheckingIfDeviceNeedsResetOnResumeInSxOnEnabled = 4564,
    DsmStateCheckingIfDeviceNeedsResetOnResumeInSxOnSuspended = 4565,
    DsmStateCompletingUnexpectedD0ExitInHubSuspended = 4566,
    DsmStateDisabledOrFailedInHubSuspend             = 4567,
    DsmStateDisablingPortOnPortEnabledInPendingHubStop = 4568,
    DsmStateReleasingReferenceOnHubSuspendAfterPSMSyncUp = 4569,
    DsmStateReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed = 4570,
    DsmStateReleasingReferenceOnHubSuspendOnDetachAfterPSMSyncUp = 4571,
    DsmStateResumingBeforeResetting                  = 4572,
    DsmStateReturningDeviceRenumerationOnHubResume   = 4573,
    DsmStateReturningDeviceResumedOnHubResume        = 4574,
    DsmStateReturningDeviceSuspendedOnHubResume      = 4575,
    DsmStateReturningHubStopWithReferenceAcquiredInHubSuspended = 4576,
    DsmStateReturningPortDetach                      = 4577,
    DsmStateSuspendedInHubSuspend                    = 4578,
    DsmStateSuspendingPortOnPortEnabledInPendingHubSuspend = 4579,
    DsmStateWaitingForHubResumeInEnabled             = 4580,
    DsmStateWaitingForHubResumeInSuspended           = 4581,
    DsmStateWaitingForHubResumeWithRenumRequired     = 4582,
    DsmStateWaitingForPSMSyncUp                      = 4583,
    DsmStateWaitingForPSMSyncUpOnPendingStop         = 4584,
    DsmStateWaitingForPSMSyncUpOnPendingSuspend      = 4585,
    DsmStateWaitingForPSMSyncUpOnResumeWithReset     = 4586,
    DsmStateWaitingForResumeResponseFromPortOnHubStop = 4587,
    DsmStateWaitingForResumeResponseFromPortOnHubSuspend = 4588,
    DsmStateAcquiringAddressZeroOwnershipOnRenum     = 4589,
    DsmStateAcquiringPowerReferenceOnStartInReEnum   = 4590,
    DsmStateCancellingAcquiringAddress0OnDetachOnRenum = 4591,
    DsmStateCancellingRetryTimerOnDetachOnRenum      = 4592,
    DsmStateCancellingRetryTimerOnStopSuspendOnRenum = 4593,
    DsmStateCheckingIfAltEnumCmdNeededInReenum       = 4594,
    DsmStateCheckingIfDevicePDChargingPolicySupportedInReEnum = 4595,
    DsmStateCheckingIfDevicePorgrammingWasLosttInUnconfigured = 4596,
    DsmStateCheckingIfDeviceProgrammingWasLosttInConfigured = 4597,
    DsmStateCheckingIfEnumRetryReachedMaximumOnRenum = 4598,
    DsmStateCheckingIfEnumRetryReachedMaximumOnRenumWithAddress0Ownership = 4599,
    DsmStateCheckingIfEnumRetryReachedMaximumWithDeviceDisabledOnRenum = 4600,
    DsmStateCheckingIfItIsBootDeviceOnIdComparisionFailure = 4601,
    DsmStateCheckingIfRequestConfigDescOnResetIsSet  = 4602,
    DsmStateCheckingIfSerialNumberShouldBeCompared   = 4603,
    DsmStateClearingDeviceLostProgammingFlagOnRestart = 4604,
    DsmStateClearingDeviceProgrammingLostFlagOnRenum = 4605,
    DsmStateComparingDeviceOnReEnumeration           = 4606,
    DsmStateComparingSerialNumberOnReEnumeration     = 4607,
    DsmStateDeconfiguringEndpointsInControllerBeforeRenumerating = 4608,
    DsmStateDisablingDeviceInControllerOnStopSuspendOnRenum = 4609,
    DsmStateDisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership = 4610,
    DsmStateDisablingOnEnumAfterFailureInReEnum      = 4611,
    DsmStateDisablingOnEnumAfterFailureInReEnumWithAddress0Ownership = 4612,
    DsmStateDisablingOnEnumAfterFailureWithDeviceDisabledInReEnum = 4613,
    DsmStateEnablingDeviceInControllerInRenum        = 4614,
    DsmStateEnumeratingAtAddressZeroOnRenum          = 4615,
    DsmStateGettingConfigDescriptorWhileRenuemrating = 4616,
    DsmStateGettingDeviceDescriptorOnRenum           = 4617,
    DsmStateGettingSerialNumberStringDescriptorWhileRenuemrating = 4618,
    DsmStateInitializingEnumRetryCountInReEnumDuringResetInConfigured = 4619,
    DsmStateInitializingEnumRetryCountInReEnumDuringResetInUnConfigured = 4620,
    DsmStateInitializingEnumRetryCountInReEnumDuringRestart = 4621,
    DsmStatePurgingDeviceTreeIoOnReEnumerationInConfigured = 4622,
    DsmStatePurgingDeviceTreeIoOnReEnumerationInUnConfigured = 4623,
    DsmStatePurgingIoOnEnumAfterFailureInReEnum      = 4624,
    DsmStatePurgingIoOnEnumAfterFailureInReEnumWithAddress0Ownership = 4625,
    DsmStateReleasingAddressZeroOwnershipOnDetachOnEnumFailure = 4626,
    DsmStateReleasingAddressZeroOwnershipOnDetachWithDeviceEnabledOnRenum = 4627,
    DsmStateReleasingAddressZeroOwnershipOnDetachWithTimer = 4628,
    DsmStateReleasingAddressZeroOwnershipOnEnumFailureInRenum = 4629,
    DsmStateReleasingAddressZeroOwnershipOnRenum     = 4630,
    DsmStateReleasingAddressZeroOwnershipOnStopSuspendOnEnumFailure = 4631,
    DsmStateReleasingAddressZeroOwnershipOnStopSuspendOnRenum = 4632,
    DsmStateReleasingAddressZeroOwnershipOnStopSuspendWithTimer = 4633,
    DsmStateReleasingPowerReferenceOnRenum           = 4634,
    DsmStateReturningOperationFailureOnRenum         = 4635,
    DsmStateReturningOperationFailureWithDeviceEnabledOnRenum = 4636,
    DsmStateReturningOperationSuccessOnRenum         = 4637,
    DsmStateReturningPortDetachDeviceOnRenum         = 4638,
    DsmStateReturningPortDetachDeviceWithDeviceEnabledOnRenum = 4639,
    DsmStateSendingMsOs20AltEnumCmdOnReenum          = 4640,
    DsmStateSettingDevicePDChargingPolicyInReEnum    = 4641,
    DsmStateSettingLPMValuesInReEnum                 = 4642,
    DsmStateSettingNonZeroAddressOnRenum             = 4643,
    DsmStateStartingTimerForEnumRetryOnRenum         = 4644,
    DsmStateStartingTimerForEnumRetryOnRenumWithAddress0Ownership = 4645,
    DsmStateStoppedSuspendedOnRenum                  = 4646,
    DsmStateWaitingForDetachAfterWrongDeviceDetectedOnBootDevicePort = 4647,
    DsmStateWaitingForDisableCompleteOnStopSuspendAfterFailureInReEnum = 4648,
    DsmStateWaitingForDisableCompleteOnStopSuspendOnRenum = 4649,
    DsmStateWaitingForDisableCompleteOnStopSuspendWithDeviceDisabledAfterFailureInReEnum = 4650,
    DsmStateWaitingForTimerToFlushOnDetachOnRenum    = 4651,
    DsmStateWaitingForTimerToFlushOnStopSuspendOnRenum = 4652,
    DsmStateWaitingForUCXIoctlOnDetachOnRenum        = 4653,
    DsmStateWaitingForUCXIoctlOnStopSuspend          = 4654,
    DsmStateCheckingIfAnyAlternateInterfaceLeft      = 4655,
    DsmStateConfiguringDevice                        = 4656,
    DsmStateCreatingNewEndpoints                     = 4657,
    DsmStateDeconfiguringEndpointsInControllerForDeviceConfiguration = 4658,
    DsmStateDeletingEndpointsForOldConfiguration     = 4659,
    DsmStateMarkingAnyEndpointsToBeEnableAsDisabled  = 4660,
    DsmStateProgrammingEndpointsAndSettingLPMValuesrForDeviceConfiguration = 4661,
    DsmStateProgrammingEndpointsInControllerForDeviceConfigurationFor20Devices = 4662,
    DsmStateReturningOperationFailureInReConfiguring = 4663,
    DsmStateReturningOperationSuccessInReConfiguring = 4664,
    DsmStateSettingDeviceInterfaceInConfiguring      = 4665,
    DsmStateDisablingPortOnHubSuspendInCleanup       = 4666,
    DsmStateFlushingHubPowerDownEvents               = 4667,
    DsmStateFlushingHubPowerUpEvents                 = 4668,
    DsmStateFlushingHubStopAfterSuspendEvent         = 4669,
    DsmStateFlushingPnpEvents                        = 4670,
    DsmStateFowardingStreamsRequestToUcxInWaitingForDevicePortEvents = 4671,
    DsmStateReleasingPowerReferenceOnHubSuspendInCleanup = 4672,
    DsmStateReturningPortDetachDeviceFromIgnoringDevicePort = 4673,
    DsmStateReturningPortDisabledOnHubSuspendInCleanup = 4674,
    DsmStateWaitingForDevicePortEvents               = 4675,
    DsmStateFlushingHubPowerDownEventsWithPortOff    = 4676,
    DsmStateFlushingHubPowerUpEventsWithPortOff      = 4677,
    DsmStateFlushingHubStopAfterSuspendEventWithPortOff = 4678,
    DsmStateFlushingPnpEventsWithPortOff             = 4679,
    DsmStateFowardingStreamsRequestToUcxInWaitingForDevicePortEventsWithPortOff = 4680,
    DsmStateWaitingForDevicePortEventsWithPortOff    = 4681,
    DsmStateDisablingPortOnHubSuspendForFailedDevice = 4682,
    DsmStateFailedDeviceHubSuspended                 = 4683,
    DsmStateFlushingHubPowerDownEventsForFailedDevice = 4684,
    DsmStateFlushingHubPowerUpEventsForFailedDevice  = 4685,
    DsmStateFlushingHubStopAfterSuspendEventForFailedDevice = 4686,
    DsmStateFlushingPnpEventsForFailedDevice         = 4687,
    DsmStateReleasingPowerReferenceOnDetachForFailedDevice = 4688,
    DsmStateReturningPortDetachDeviceForFailedDevice = 4689,
    DsmStateWaitingForDevicePortEventsForFailedDevice = 4690,
    DsmStateFlushingHubStartResumeEvent              = 4691,
    DsmStateFlushingHubSuspendEvent                  = 4692,
    DsmStateFlushingPnpEventExceptStop               = 4693,
    DsmStateFowardingStreamsRequestToUcxInWaitingForDevicePortEventsExceptStop = 4694,
    DsmStateWaitingForDevicePortEventsExceptStop     = 4695,
    DsmStateFlushingHubResumeEventForBootDevice      = 4696,
    DsmStateFlushingHubSuspendEventForBootDevice     = 4697,
    DsmStateFlushingPnpEventsForBootDevice           = 4698,
    DsmStateWaitingForDeviceHubEventsForBootDevice   = 4699,
    DsmStateAckingPortEventInD3Cold                  = 4700,
    DsmStateAckingPortResumed                        = 4701,
    DsmStateCheckingIfDeviceArmedForWakeOnResume     = 4702,
    DsmStateCheckingIfDeviceNeedsResetOnResumeInS0   = 4703,
    DsmStateCheckingIfDeviceShouldBeDisarmedForWakeOnResume = 4704,
    DsmStateCheckingIfResetOnLastResumeFlagIsSet     = 4705,
    DsmStateCheckingIfResetOnLastResumeFlagIsSetOnSuspended = 4706,
    DsmStateCompletingD0EntryOnDetach                = 4707,
    DsmStateCompletingD0EntryOnPortFailure           = 4708,
    DsmStateDisabledAfterD3Cold                      = 4709,
    DsmStateDisablingOnHubSuspendNeedingRenumeration = 4710,
    DsmStateDisablingPortOnStopFromSuspended         = 4711,
    DsmStateDisarmingDeviceForWakeOnD0Entry          = 4712,
    DsmStateDisarmingDeviceForWakeOnPortResumed      = 4713,
    DsmStatePurgingDeviceIoOnHubSuspendWhileWaitingForD0Entry = 4714,
    DsmStatePurgingDeviceIoOnHubSuspendWhileWaitingForD0EntryOnHwWake = 4715,
    DsmStateReleasingPowerReferenceOnDetachWhileSuspending = 4716,
    DsmStateReleasingPowerReferenceOnDetachWhileWaitingForHubSuspend = 4717,
    DsmStateReleasingPowerReferenceOnPortFailureWhileWaitingForHubSuspend = 4718,
    DsmStateResumingFromSelectiveSuspend             = 4719,
    DsmStateReturningDetachDevice                    = 4720,
    DsmStateReturningDeviceResumed                   = 4721,
    DsmStateReturningHubStopFromSuspended            = 4722,
    DsmStateReturningPortFailureOnResume             = 4723,
    DsmStateReturningRenumerateDeviceOnResume        = 4724,
    DsmStateSettingResetOnLastResumeDueToD3Cold      = 4725,
    DsmStateSettingResetOnLastResumeFlagForPDO       = 4726,
    DsmStateSettingResetOnLastResumeFlagForPDOAfterD3Cold = 4727,
    DsmStateStartingDeviceIoOnDeviceResumeOnHwWake   = 4728,
    DsmStateStartingDeviceIoOnDeviceResumeOnSwWake   = 4729,
    DsmStateSuspended                                = 4730,
    DsmStateSuspendedWithHubSuspended                = 4731,
    DsmStateSuspendedWithHubSuspendedInD3Cold        = 4732,
    DsmStateSuspendingBackUnarmedDevice              = 4733,
    DsmStateSuspendingWakeArmedDeviceOnHubSuspend    = 4734,
    DsmStateWaitingForD0EntryOnHwWake                = 4735,
    DsmStateWaitingForD0EntryOnPendingRenumerate     = 4736,
    DsmStateWaitingForSuspendCompleteOnHubSuspend    = 4737,
    DsmStateAbortingDeviceIoInSuspendingBeforeArmingForWakeFailure = 4738,
    DsmStateArmingDeviceForWake                      = 4739,
    DsmStateCheckingIfDeviceArmedForWakeOnSuspending = 4740,
    DsmStateCompletingWaitWakeOnDetachDuringSuspending = 4741,
    DsmStateCompletingWaitWakeOnSuspendFailure       = 4742,
    DsmStatePurgingDeviceIoInSuspending              = 4743,
    DsmStatePurgingDeviceIoInSuspendingAfterArmingForWake = 4744,
    DsmStatePurgingDeviceIoInSuspendingAfterArmingForWakeFailure = 4745,
    DsmStateReturningOperationFailureInSuspending    = 4746,
    DsmStateReturningOperationSuccessInSuspending    = 4747,
    DsmStateReturningPortDetachDeviceInSuspending    = 4748,
    DsmStateReturningUnexpectedHubSuspendInSuspending = 4749,
    DsmStateSuspending                               = 4750,
    DsmStateSuspendingPortOnFailureInSuspending      = 4751,
    DsmStateSuspendingWithArmedForWake               = 4752,
    DsmStateWaitingForPortResponseOnHubSuspend       = 4753,
    DsmStateCreatingNewEndpointsInSettingInterface   = 4754,
    DsmStateDeletingEndpointsForNewInterfaceOnFailure = 4755,
    DsmStateDeletingEndpointsForOldInterface         = 4756,
    DsmStateDeletingEndpointsForOldInterfaceOnFailure = 4757,
    DsmStateDisablingEndpointsForTheCurrentInterface = 4758,
    DsmStateDisablingEndpointsForTheNewInterfaceOnFailure = 4759,
    DsmStatePreparingEndpointAndInterfaceListsOnSelectInterface = 4760,
    DsmStateProgrammingEndpointsAndSettingLPMValuesInSettingInterface = 4761,
    DsmStateProgrammingEndpointsInControllerInSettingInterfaceFor20Devices = 4762,
    DsmStateReturningOperationFailureInSettingInterface = 4763,
    DsmStateReturningOperationSuccessInSettingInterface = 4764,
    DsmStateSettingDeviceInterfaceInSettingInterface = 4765,
    DsmStateSettingDeviceInterfaceInSettingInterfaceOnFailureForCompat = 4766,
    DsmStateCheckingIfDeviceSpeedChanged             = 4767,
    DsmStateCheckingIfFirstEnumTryInRenum            = 4768,
    DsmStateGettingDeviceDescriptorInReEnumAtZero    = 4769,
    DsmStateLoggingReset1TimeoutInReEnumAtZero       = 4770,
    DsmStateLoggingReset2TimeoutInReEnum             = 4771,
    DsmStateNotifyingDeviceResetToUCXAfterReset1InRenum = 4772,
    DsmStateNotifyingDeviceResetToUCXAfterReset2InRenum = 4773,
    DsmStateResetting1InRenum                        = 4774,
    DsmStateResetting2InRenum                        = 4775,
    DsmStateReturningDeviceOperationFailureInReEnumAtZero = 4776,
    DsmStateReturningDeviceOperationSuccessInReEnumAtZero = 4777,
    DsmStateReturningHubStopInReEnumAtZero           = 4778,
    DsmStateReturningHubSuspendInReEnumAtZero        = 4779,
    DsmStateReturningPortDetachDeviceInReEnumAtZero  = 4780,
    DsmStateStoppingTimerOnHubStopInReEnumAtZero     = 4781,
    DsmStateStoppingTimerOnHubSuspendInReEnumAtZero  = 4782,
    DsmStateStoppingTimerOnPortDetachInReEnumAtZero  = 4783,
    DsmStateValidatingDeviceDescriptorInReEnumAtZero = 4784,
    DsmStateWaitingForPortResetOnHubStopInReEnumAtZero = 4785,
    DsmStateWaitingForPortResetOnHubSuspendInReEnumAtZero = 4786,
    DsmStateWaitingForPostReset1TimerInRenum         = 4787,
    DsmStateWaitingForPostReset2ExtendedTimerInRenum = 4788,
    DsmStateWaitingForPostReset2TimerInRenum         = 4789,
    DsmStateWaitingForStopSuspendOnReset1InReEnumAtZero = 4790,
    DsmStateWaitingForTimerOnHubStopInReEnumAtZero   = 4791,
    DsmStateWaitingForTimerOnHubSuspendInReEnumAtZero = 4792,
    DsmStateWaitingForTimerOnPortDetachInReEnumAtZero = 4793,
    DsmStateCheckingIfFirstReEnumTryFor30Device      = 4794,
    DsmStateDeterminingPortResetTypeRequired         = 4795,
    DsmStateLoggingResetTimeoutInResettingFor30      = 4796,
    DsmStateNotifyingDeviceResetToUCXInResettingFor30 = 4797,
    DsmStateQueueingHotPortResetFor30                = 4798,
    DsmStateQueueingWarmPortResetFor30               = 4799,
    DsmStateReturningDeviceOperationFailureInResettingFor30 = 4800,
    DsmStateReturningDeviceOperationSuccessInInResettingFor30 = 4801,
    DsmStateReturningHubStopInResettingFor30         = 4802,
    DsmStateReturningHubSuspendInResettingFor30      = 4803,
    DsmStateReturningPortDetachDeviceInResettingFor30 = 4804,
    DsmStateStartingPostResetTimerFor30              = 4805,
    DsmStateStoppingTimerOnHubStopInResettingFor30   = 4806,
    DsmStateStoppingTimerOnHubSuspendInResettingFor30 = 4807,
    DsmStateStoppingTimerOnPortDetachInResettingFor30 = 4808,
    DsmStateWaitingForPortResetCompleteFor30         = 4809,
    DsmStateWaitingForPortResetOnHubStopInResettingFor30 = 4810,
    DsmStateWaitingForPortResetOnHubSuspendInResettingFor30 = 4811,
    DsmStateWaitingForStopSuspendOnReset1InResettingFor30 = 4812,
    DsmStateWaitingForTimerOnHubStopInResettingFor30 = 4813,
    DsmStateWaitingForTimerOnHubSuspendInResettingFor30 = 4814,
    DsmStateWaitingForTimerOnPortDetachInResettingFor30 = 4815,
    DsmStateAllocatingBufferForMsOsExtendedPropertyDescriptor = 4816,
    DsmStateCheckingIfMsOs20RegistryValuesShouldBeInstalled = 4817,
    DsmStateCheckingIfMSOSExtendedPropertyDescriptorShouldBeQueried = 4818,
    DsmStateFreeingBufferAllocatedForMSOSExtendedProperty = 4819,
    DsmStateGettingMSOSExtendedPropertyDescriptor    = 4820,
    DsmStateGettingMSOSExtendedPropertyDescriptorHeader = 4821,
    DsmStateInstallingMsOs20RegistryValues           = 4822,
    DsmStateSettingExtPropDescSemaphoreForMsOs2      = 4823,
    DsmStateSettingExtPropDescSemaphoreRegistryFlag  = 4824,
    DsmStateSignalingPnpEventForMSOSExtInstall       = 4825,
    DsmStateValidatingMSOSExtendedPropertyDescriptor = 4826,
    DsmStateValidatingMSOSExtendedPropertyDescriptorHeader = 4827,
    DsmStateWaitingForInstallMSOSExt                 = 4828,
    DsmStateWritingCustomPropertiesInRegistry        = 4829,
    DsmStateCheckingIfLTMShouldBeEnabled             = 4830,
    DsmStateCheckingIfSetIsochDelayShouldBeSkipped   = 4831,
    DsmStateCheckingIfSetSelShouldBeSkipped          = 4832,
    DsmStateEnablingLTM                              = 4833,
    DsmStateReturningOperationFailureInSettingLPMValues = 4834,
    DsmStateReturningOperationSuccessInSettingLPMValues = 4835,
    DsmStateSettingSEL                               = 4836,
    DsmStateSetttingIsochDelay                       = 4837,
    DsmStateCheckingIfEnableU1NeedsToBeUpdated       = 4838,
    DsmStateCheckingIfEnableU2NeedsToBeUpdated       = 4839,
    DsmStateCheckingIfEndpointsNeedToBeConfiguredAfterDecreasingExitLatency = 4840,
    DsmStateCheckingIfEndpointsNeedToBeProgrammedAfterIncreasingLatency = 4841,
    DsmStateCheckingIfEndpointsToBeDisabled          = 4842,
    DsmStateCheckingIfEndpointsToBeDisabledOnDetach  = 4843,
    DsmStateCheckingIfExitLatencyNeedsToBeDecreased  = 4844,
    DsmStateCheckingIfExitLatencyNeedsToBeIncreased  = 4845,
    DsmStateCheckingIfU1TimeoutNeedsToBeChanged      = 4846,
    DsmStateCheckingIfU2TimeoutNeedsToBeChanged      = 4847,
    DsmStateComputingU1U2TimeoutsAndExitLatency      = 4848,
    DsmStateDisablingEndpointsOnConfigFailure        = 4849,
    DsmStateDisablingEndpointsOnConfigFailureOnDetach = 4850,
    DsmStateDisablingU1                              = 4851,
    DsmStateDisablingU1U2OnExitLatencyTooLargeError  = 4852,
    DsmStateDisablingU2                              = 4853,
    DsmStateEnablingU1                               = 4854,
    DsmStateEnablingU2                               = 4855,
    DsmStateInitializingU1U2Flags                    = 4856,
    DsmStateProgrammingEndpointsInControllerAfterDecreasingExitLatency = 4857,
    DsmStateProgrammingEndpointsInControllerAfterIncreasingExitLatency = 4858,
    DsmStateQueuingSetU1TimeoutToPsm                 = 4859,
    DsmStateQueuingSetU2TimeoutToPsm                 = 4860,
    DsmStateReturningOperationFailureInUpdatingLPMValues = 4861,
    DsmStateReturningOperationSuccessInUpdatingLPMValues = 4862,
    DsmStateReturningPortDetachDeviceInUpdatingLPMValues = 4863,
    DsmStateUpdatingDeviceExitLatencyInTheControllerAfterDecreasingExitLatency = 4864,
    DsmStateUpdatingDeviceExitLatencyInTheControllerAfterIncreasingExitLatency = 4865,
    DsmStateUpdatingDeviceStatusToU1Disabled         = 4866,
    DsmStateUpdatingDeviceStatusToU1Enabled          = 4867,
    DsmStateUpdatingDeviceStatusToU2Disabled         = 4868,
    DsmStateUpdatingDeviceStatusToU2Enabled          = 4869,
    DsmStateComputingLPMTimeoutValuesInUnconfigured  = 4870,
    DsmStateQueuingSetU2TimeoutToPsmForEnumeratedDevice = 4871,
    DsmStateReturningOperationFailureInUpdatingLPMValuesInUnconfigured = 4872,
    DsmStateReturningOperationSuccessInUpdatingLPMValuesInUnconfigured = 4873,
    DsmStateGettingRemoteWakeCapability              = 4874,
    DsmStateReturningOperationFailureInGettingRemoteWakeCap = 4875,
    DsmStateReturningOperationSuccessInGettingRemoteWakeCap = 4876,
    DsmStateValidatingInterfaceStatusForRemoteWake   = 4877,
    DsmStateCheckingIf20LPMShouldBeEnabled           = 4878,
    DsmStateReturningOperationSuccessInSetting20LPMValues = 4879,
    DsmStateWaitingFor20LpmUpdateIoctlToComplete     = 4880,
} DSM_STATE, *PDSM_STATE;





//
// Enum of ETW state names
//
typedef enum _DSM_STATE_ETW {
    DsmStateEtwWaitingForPortAttach                  = 4000,
    DsmStateEtwAckingD0ForFaultyClientDrivers        = 4801,
    DsmStateEtwAckingHubStopAfterSuspendFromStoppedState = 4001,
    DsmStateEtwAckingPdoPreStart                     = 4822,
    DsmStateEtwAckingPdoPreStartFromStoppedEnabled   = 4881,
    DsmStateEtwAckingQueryResourcesInConfiguredD0    = 4888,
    DsmStateEtwAckingStopAfterSuspendOnDetach        = 4860,
    DsmStateEtwAcquiringPowerReferenceOnHubOnAttach  = 4002,
    DsmStateEtwAcquiringPowerReferenceOnHubStart     = 4003,
    DsmStateEtwBugcheckingSystemOnBootDeviceEnumerationFailure = 4004,
    DsmStateEtwCheckIfDeviceCanBeDisabledOnDriverNotFound = 4879,
    DsmStateEtwCheckingForZeroBwPipeOnClientClearStall = 4005,
    DsmStateEtwCheckingForZeroBwPipeOnClientResetPipe = 4006,
    DsmStateEtwCheckingForZeroBwPipeOnClientSyncResetPipe = 4007,
    DsmStateEtwCheckingIfBootDeviceOnD0Exit          = 4008,
    DsmStateEtwCheckingIfBootDeviceOnDetach          = 4009,
    DsmStateEtwCheckingIfBootDeviceOnReset           = 4010,
    DsmStateEtwCheckingIfDeviceHasReceivedFirstStart = 4011,
    DsmStateEtwCheckingIfDeviceHasReceivedFirstStartInUncoonfigured = 4012,
    DsmStateEtwCheckingIfDeviceShouldBeDisabled      = 4013,
    DsmStateEtwCheckingIfItIsIsochronousEndpoint     = 4014,
    DsmStateEtwCleaningUpAnyExistingConfigurationOnStart = 4015,
    DsmStateEtwClearingEndpointHaltOnClientClearStall = 4016,
    DsmStateEtwClearingEndpointHaltOnClientResetPipe = 4017,
    DsmStateEtwClearingFailIoFlagOnSuccessfulRecovery = 4018,
    DsmStateEtwCompletingClientRequestInConfigured   = 4019,
    DsmStateEtwCompletingClientRequestInConfiguredWithLastStatus = 4020,
    DsmStateEtwCompletingClientRequestInConfiguredWithSpecialFile = 4021,
    DsmStateEtwCompletingClientRequestInUnconfigured = 4022,
    DsmStateEtwCompletingClientRequestInUnConfiguredWithLastStatus = 4023,
    DsmStateEtwCompletingClientRequestOnDetachInConfigured = 4024,
    DsmStateEtwCompletingClientRequestOnDetachInDisabled = 4025,
    DsmStateEtwCompletingClientRequestOnDetachWithDeviceProgrammed = 4026,
    DsmStateEtwCompletingClientRequestOnFailureInConfigured = 4027,
    DsmStateEtwCompletingClientRequestOnFailureInDisabled = 4028,
    DsmStateEtwCompletingClientRequestOnFailureInUnconfigured = 4029,
    DsmStateEtwCompletingClientUnconfigureRequest    = 4030,
    DsmStateEtwCompletingClientUnconfigureRequestWithFailure = 4031,
    DsmStateEtwCompletingD0EntryInConfigured         = 4032,
    DsmStateEtwCompletingD0EntryInConfiguredWithSpecialFile = 4033,
    DsmStateEtwCompletingD0EntryInConfiguredWithSpecialFileOnDetach = 4034,
    DsmStateEtwCompletingD0EntryInConfiguredWithSpecialFileOnDetachWithDeviceEnabled = 4035,
    DsmStateEtwCompletingD0EntryInUnconfigured       = 4036,
    DsmStateEtwCompletingD0EntryOnDetachWithSpecialFile = 4037,
    DsmStateEtwCompletingD0ExitAfterHubSuspend       = 4038,
    DsmStateEtwCompletingD0ExitOnSuspendFailure      = 4039,
    DsmStateEtwCompletingDeviceCleanup               = 4040,
    DsmStateEtwCompletingHubStopAfterSuspend         = 4041,
    DsmStateEtwCompletingHubStopWithPortOff          = 4042,
    DsmStateEtwCompletingPDOCleanupForUnknownDevice  = 4043,
    DsmStateEtwCompletingPdoExitFinalOnDetach        = 4044,
    DsmStateEtwCompletingPdoExitFinalOnDetachOnDeviceReportedMissing = 4045,
    DsmStateEtwCompletingPdoReStartOnDetach          = 4826,
    DsmStateEtwCompletingPdoReStartOnDetachWithDeviceEnabled = 4827,
    DsmStateEtwCompletingPdoReStartOnOperationFailure = 4828,
    DsmStateEtwCompletingPnpEventOnDetachInConfigured = 4046,
    DsmStateEtwCompletingPnpEventOnDetachInDisabled  = 4047,
    DsmStateEtwCompletingPnpEventOnDetachInUnConfigured = 4048,
    DsmStateEtwCompletingPnpEventOnDetachWhileRenumeratingOnResume = 4049,
    DsmStateEtwCompletingPnpEventOnFailureInDisabledWithPortOff = 4050,
    DsmStateEtwCompletingPnpEventOnFailureWithEndpointsToBeDeleted = 4052,
    DsmStateEtwConfiguredInD0                        = 4053,
    DsmStateEtwConfiguredResumingOnHubResume         = 4054,
    DsmStateEtwConfiguredSuspended                   = 4055,
    DsmStateEtwConfiguredSuspendedOnHubSuspend       = 4056,
    DsmStateEtwConfiguredSuspendedWithSpecialFile    = 4057,
    DsmStateEtwConfiguredSuspending                  = 4058,
    DsmStateEtwConfiguredSuspendingOnHubSuspend      = 4059,
    DsmStateEtwConfiguredSuspendingWithSpecialFile   = 4060,
    DsmStateEtwConfiguringDeviceFromCachedInformation = 4061,
    DsmStateEtwConfiguringDeviceFromCachedInformationOnHubResume = 4062,
    DsmStateEtwConfiguringDeviceFromCachedInformationOnResume = 4063,
    DsmStateEtwConfiguringDeviceFromCachedInformationOnResumeWithSpecialFile = 4064,
    DsmStateEtwConfiguringDeviceFromCachedInformationWithSpecialFile = 4065,
    DsmStateEtwConfiguringOnSelectConfiguration      = 4066,
    DsmStateEtwCyclingPortOnRenumerationOfUnknownDevice = 4067,
    DsmStateEtwD0EntryForUnknownDevice               = 4068,
    DsmStateEtwDeConfiguringDeviceOnClientRequest    = 4069,
    DsmStateEtwDeConfiguringDeviceOnClientRequestFromUnconfigured = 4070,
    DsmStateEtwDeletingDefaultEndpointAndDeviceOnDetachDuringEnum = 4071,
    DsmStateEtwDeletingDeviceAndDefaultEndpointOnCleanup = 4072,
    DsmStateEtwDeletingDeviceAndDefaultEndpointOnCleanupAfterDetach = 4073,
    DsmStateEtwDeletingDeviceAndDefaultEndpointOnCleanupWithPortOff = 4074,
    DsmStateEtwDeletingEndpoinstAndDeviceOnDetachOnCleanup = 4075,
    DsmStateEtwDeletingEndpointsAndDeviceOnOnCleanup = 4076,
    DsmStateEtwDeporgrammingAllEndpointsOnCleanupFromEnumeratedConfigured = 4077,
    DsmStateEtwDeporgrammingAllEndpointsOnHubStopFromEnumeratedConfigured = 4078,
    DsmStateEtwDeprogrammingAllEndpointsOnHubSuspendFromEnumeratedConfigured = 4831,
    DsmStateEtwDetachedAndDeprogrammedWithResetPending = 4079,
    DsmStateEtwDetachedWithResetPending              = 4080,
    DsmStateEtwDeviceDetachedAndDeporgrammedWithSpecialFile = 4081,
    DsmStateEtwDeviceDetachedWithSpecialFile         = 4082,
    DsmStateEtwDisablingBackPortInEnumeratedUnknown  = 4083,
    DsmStateEtwDisablingBackPortInWaitingForD0EntryForFailedDevice = 4084,
    DsmStateEtwDisablingDeviceAndDefaultEndpointInControllerOnDetachDuringEnum = 4085,
    DsmStateEtwDisablingDeviceAndDefaultEndpointOnDetachOnHubStart = 4086,
    DsmStateEtwDisablingDeviceAndDefaultEndpointOnEnumFailureOnHubStart = 4087,
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOFailureWithPortOff = 4088,
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnCleanupWithPortOff = 4089,
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnD0ExitOnDetachWithPDOMissing = 4090,
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnDetach = 4091,
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnDetachOnHubStopWithPDOMissing = 4092,
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnDetachOnPDOCleaupWithPDOMissing = 4093,
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspend = 4094,
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspendOnDetach = 4861,
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnHubStopOnDetach = 4095,
    DsmStateEtwDisablingDeviceInControllerOnCleanup  = 4096,
    DsmStateEtwDisablingDeviceInControllerOnDetachOnCleanup = 4097,
    DsmStateEtwDisablingDeviceInControllerOnEnumFailureOnPdoPreStart = 4829,
    DsmStateEtwDisablingDeviceInControllerOnEnumFailureOnRestart = 4098,
    DsmStateEtwDisablingDeviceInControllerOnHubStopFromStoppedEnabled = 4099,
    DsmStateEtwDisablingDeviceInControllerOnHubStopInStopped = 4100,
    DsmStateEtwDisablingDeviceInControllerOnHubStopInSuspended = 4101,
    DsmStateEtwDisablingDeviceInControllerOnPDOStop  = 4102,
    DsmStateEtwDisablingDeviceInControllerOnRenumerationWhileSuspended = 4894,
    DsmStateEtwDisablingDeviceInControllerOnStartTimeout = 4823,
    DsmStateEtwDisablingEndpointsInControllerOHubStopAfterSuspendOnDetach = 4862,
    DsmStateEtwDisablingEndpointsInControllerOHubStopOnDetach = 4103,
    DsmStateEtwDisablingEndpointsInControllerOnCleanupWithPortOff = 4104,
    DsmStateEtwDisablingEndpointsInControllerOnClientRequest = 4105,
    DsmStateEtwDisablingEndpointsInControllerOnD0ExitOnDetach = 4106,
    DsmStateEtwDisablingEndpointsInControllerOnD0ExitOnDetachWithPDOMissing = 4107,
    DsmStateEtwDisablingEndpointsInControllerOnDetachOnHubStopWithPDOMissing = 4108,
    DsmStateEtwDisablingEndpointsInControllerOnDetachOnPDOCleanupWithPDOMissing = 4109,
    DsmStateEtwDisablingEndpointsInControllerOnFailureWithPortOff = 4110,
    DsmStateEtwDisablingEndpointsInControllerOnHubStopAfterSuspend = 4111,
    DsmStateEtwDisablingEndpointsInControllerOnRenumerationWhileSuspended = 4895,
    DsmStateEtwDisablingEndpointsInControllerOnStop  = 4112,
    DsmStateEtwDisablingEndpointsOnDetachWithSpecialFile = 4113,
    DsmStateEtwDisablingEndpointsOnDetachWithSpecialFileAndResetPending = 4114,
    DsmStateEtwDisablingEndpointsOnFailureWithSpecialFile = 4115,
    DsmStateEtwDisablingPortOnFailureInUnConfigured  = 4116,
    DsmStateEtwDisablingPortOnHubStop                = 4117,
    DsmStateEtwDisablingPortOnHubStopFromStoppedEnabled = 4118,
    DsmStateEtwDisablingPortOnPdoCleanup             = 4119,
    DsmStateEtwDisablingPortOnPDOStop                = 4120,
    DsmStateEtwDisablingPortOnStartTimeout           = 4824,
    DsmStateEtwEnumeratedAsFailedUnknown             = 4121,
    DsmStateEtwEnumerating                           = 4122,
    DsmStateEtwFlushingPD0D0ExitFinalInConfigured    = 4123,
    DsmStateEtwFlushingPD0D0ExitFinalInUnConfigured  = 4124,
    DsmStateEtwFowardingStreamsRequestToUcxInConfigured = 4125,
    DsmStateEtwFowardingStreamsRequestToUcxInConfiguredSuspended = 4126,
    DsmStateEtwFowardingStreamsRequestToUcxInConfiguredSuspendedWithSpecialFile = 4127,
    DsmStateEtwFowardingStreamsRequestToUcxInDeviceDetachedAndDeporgrammedWithSpecialFile = 4128,
    DsmStateEtwFowardingStreamsRequestToUcxInDeviceDetachedWithSpecialFile = 4129,
    DsmStateEtwFowardingStreamsRequestToUcxInStopEnabled = 4130,
    DsmStateEtwFowardingStreamsRequestToUcxInStopped = 4131,
    DsmStateEtwFowardingStreamsRequestToUcxInStoppedWithHubSuspended = 4132,
    DsmStateEtwFowardingStreamsRequestToUcxInUnconfiguredInD0 = 4133,
    DsmStateEtwFowardingStreamsRequestToUcxInUnconfiguredSuspended = 4134,
    DsmStateEtwFowardingStreamsRequestToUcxInWaitingForDetachorResetWithDeviceDeprogrammed = 4135,
    DsmStateEtwFowardingStreamsRequestToUcxInWaitingForDetatchorReset = 4136,
    DsmStateEtwGettingMatchingPipeHandleOnClientClearStall = 4137,
    DsmStateEtwGettingMatchingPipeHandleOnClientResetPipe = 4138,
    DsmStateEtwGettingMatchingPipeHandleOnClientSyncResetPipe = 4139,
    DsmStateEtwIssuingDeviceDetachedToPSMOnDetach    = 4142,
    DsmStateEtwIssuingPortCycleAndCompletingClientRequestOnEnumFailue = 4143,
    DsmStateEtwIssuingPortCycleAndSingalingPnpEventOnEnumFailueFromConfigured = 4144,
    DsmStateEtwIssuingPortCycleOnEnumFailureOnHubResume = 4145,
    DsmStateEtwIssuingPortCycleOnFailueWithEndpointsToBeDeleted = 4146,
    DsmStateEtwNotifyingDeviceDisconnectedOnDetach   = 4147,
    DsmStateEtwNotifyingDeviceDisconnectedOnDetachWithSpecialFile = 4148,
    DsmStateEtwPreparingEndpointAndInterfaceListsOnClientReset = 4149,
    DsmStateEtwPreparingEndpointAndInterfaceListsOnClientResetWithSpecialFile = 4150,
    DsmStateEtwPreparingEndpointAndInterfaceListsOnResetAfterHubResume = 4151,
    DsmStateEtwPreparingEndpointAndInterfaceListsOnResetAfterResume = 4152,
    DsmStateEtwPreparingEndpointAndInterfaceListsOnResetAfterResumeWithSpecialFile = 4153,
    DsmStateEtwPreparingEndpointAndInterfaceListsOnSelectConfiguration = 4154,
    DsmStateEtwPreparingEndpointAndInterfaceListsOnSelectConfigurationFromConfigured = 4155,
    DsmStateEtwPurgingDeviceIoOnDetachInConfigured   = 4156,
    DsmStateEtwPurgingDeviceIoOnHubSuspend           = 4157,
    DsmStateEtwPurgingDeviceIoOnHubSuspendFromSuspending = 4158,
    DsmStateEtwPurgingDeviceIoOnPrepareForHibernate  = 4159,
    DsmStateEtwQueryingDescriptorForFdoRequestFromConfigured = 4160,
    DsmStateEtwQueryingDescriptorForFdoRequestFromStoppedEnumerated = 4161,
    DsmStateEtwQueryingDescriptorForFdoRequestFromUnConfigured = 4162,
    DsmStateEtwQueryingDeviceTextStringFromConfigured = 4163,
    DsmStateEtwQueryingDeviceTextStringFromStoppedEnumerated = 4164,
    DsmStateEtwQueryingDeviceTextStringFromUnConfigured = 4165,
    DsmStateEtwQueueingAttachFailureToPort           = 4166,
    DsmStateEtwQueueingAttachSuccessToPort           = 4167,
    DsmStateEtwQueueingDeviceDetachedToPsmForBootDeviceInUnconfigured = 4168,
    DsmStateEtwQueueingDeviceDetachedToPsmForBootDeviceWithResetPending = 4169,
    DsmStateEtwQueueingDeviceDetachedToPsmForBootDeviceWithResetPendingAndDeviceDeprogrammed = 4170,
    DsmStateEtwQueueingPrepareForHiberToPSM          = 4171,
    DsmStateEtwReDisablingPortAfterHubResume         = 4172,
    DsmStateEtwReEnumeratingOnClientRequestInConfigured = 4173,
    DsmStateEtwReEnumeratingOnClientRequestInConfiguredWithSpecialFile = 4174,
    DsmStateEtwReEnumeratingOnClientRequestInUnconfigured = 4175,
    DsmStateEtwReEnumeratingOnClientRequestOnReattach = 4176,
    DsmStateEtwReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed = 4177,
    DsmStateEtwReEnumeratingOnHubResumeInConfigured  = 4178,
    DsmStateEtwReEnumeratingOnHubResumeInUnConfigured = 4179,
    DsmStateEtwReEnumeratingOnHubStart               = 4180,
    DsmStateEtwReEnumeratingOnPdoPreStartWhileEnabled = 4825,
    DsmStateEtwReEnumeratingOnPdoStart               = 4181,
    DsmStateEtwReEnumeratingOnPdoStartWhileEnabled   = 4182,
    DsmStateEtwReEnumeratingOnResumeInConfigured     = 4183,
    DsmStateEtwReEnumeratingOnResumeInConfiguredWithSpecialFile = 4184,
    DsmStateEtwReEnumeratingOnResumeInUnConfigured   = 4185,
    DsmStateEtwRegisteringWithHSM                    = 4186,
    DsmStateEtwReleasingDevicePowerReferenceOnStopAfterDetach = 4863,
    DsmStateEtwReleasingPowerReferenceOnDetachInConfigured = 4883,
    DsmStateEtwReleasingPowerReferenceOnHubOnDetach  = 4187,
    DsmStateEtwReleasingPowerReferenceOnHubOnDetachOnEnumSuccess = 4188,
    DsmStateEtwReleasingPowerReferenceOnHubOnEnumerationFailure = 4189,
    DsmStateEtwReleasingPowerReferenceOnHubStopSuspend = 4190,
    DsmStateEtwReleasingPowerReferenceOnHubSuspendStopOnDetach = 4191,
    DsmStateEtwReleasingPowerReferenceOnPortFailureInHubStopSuspend = 4192,
    DsmStateEtwReleasingPowerReferenceOnSuspendFailureInConfigured = 4193,
    DsmStateEtwReleasingPowerReferenceOnSuspendFailureInUnConfigured = 4194,
    DsmStateEtwRemovingDeviceInfoFromGlobalListOnCleanup = 4195,
    DsmStateEtwRemovingDeviceInfoFromGlobalListOnCleanupOnDetachInConfigured = 4196,
    DsmStateEtwRemovingDeviceInfoFromGlobalListOnCleanupOnDetachInUnConfigured = 4197,
    DsmStateEtwRemovingDeviceInfoFromGlobalListOnCleanUpOnHubStopOnDetach = 4198,
    DsmStateEtwRemovingDeviceInfoFromGlobalListOnDeviceReportedMissing = 4199,
    DsmStateEtwRemovingDeviceInfoFromGlobalListOnReportedMissingOnHubStopOnDetach = 4200,
    DsmStateEtwReportingDeviceMissing                = 4201,
    DsmStateEtwReportingDeviceToPnp                  = 4202,
    DsmStateEtwReportingFailedDeviceAsMissing        = 4203,
    DsmStateEtwReportingFailedUnknownDeviceToPnp     = 4204,
    DsmStateEtwRequestingCyclePortAndCompletingPnpEventOnFailureInUnconfigured = 4878,
    DsmStateEtwRequestingPortCycleOnErrorFromConfigured = 4205,
    DsmStateEtwResettingPipeInUCXOnClientReset       = 4206,
    DsmStateEtwResettingPipeInUCXOnClientSyncResetPipe = 4207,
    DsmStateEtwResumingOnHubResumeInStopped          = 4208,
    DsmStateEtwSettingConfigurationInformationInClientRequest = 4209,
    DsmStateEtwSettingDeviceInterface                = 4210,
    DsmStateEtwSettingExitLatencyAdjustmentOnNoPingResponseError = 4211,
    DsmStateEtwSettingFailIoFlagOnClientReset        = 4212,
    DsmStateEtwSettingForceResetOnRenumFlag          = 4880,
    DsmStateEtwSettingInterfaceInformationInClientRequest = 4213,
    DsmStateEtwSettingLPMValuesInConfiguredOnSettingChange = 4214,
    DsmStateEtwSettingLPMValuesInStoppedEnabled      = 4215,
    DsmStateEtwSettingLPMValuesInUnConfiguredOnSettingChange = 4216,
    DsmStateEtwSignalingPnpPowerEventOnDetachDuringRenumOnRestart = 4217,
    DsmStateEtwSignalingPnpPowerEventOnStop          = 4218,
    DsmStateEtwSignalingPnpPowerEventOnStopEnable    = 4219,
    DsmStateEtwSignallingPnpEventOnHubSuspendFromUnconfiguredSuspending = 4220,
    DsmStateEtwStartingDeviceIoOnHubResume           = 4221,
    DsmStateEtwStopped                               = 4222,
    DsmStateEtwStoppedEnabled                        = 4223,
    DsmStateEtwStoppedEnumerated                     = 4224,
    DsmStateEtwStoppedEnumeratedConfiguredForFaultyClients = 4225,
    DsmStateEtwStoppedEnumeratedWithHubSuspended     = 4226,
    DsmStateEtwStoppedWithHubSuspended               = 4227,
    DsmStateEtwSuspendingOnHubSuspendInStopped       = 4228,
    DsmStateEtwUnConfiguredInD0                      = 4229,
    DsmStateEtwUnConfiguredResumingOnHubResume       = 4230,
    DsmStateEtwUnconfiguredSuspended                 = 4231,
    DsmStateEtwUnConfiguredSuspendedOnHubSuspend     = 4232,
    DsmStateEtwUnconfiguredSuspending                = 4233,
    DsmStateEtwUnConfiguredSuspendingOnHubSuspend    = 4234,
    DsmStateEtwUnregisteringWithHSMOnDetachAfterCleanup = 4235,
    DsmStateEtwUnregisteringWithHsmOnDetachOnDeviceReportedMissing = 4236,
    DsmStateEtwUnregsiteringWithHsmOnDetach          = 4237,
    DsmStateEtwUnregsiteringWithHSMOnDetachOnPDOCleaupWithPDOMissing = 4238,
    DsmStateEtwValidatingSelectConfigUrbFromConfigured = 4239,
    DsmStateEtwValidatingSelectConfigUrbFromUnConfigured = 4240,
    DsmStateEtwValidatingSelectInterfaceUrbFromConfigured = 4241,
    DsmStateEtwWaitingForD0EntryForFailedDevice      = 4242,
    DsmStateEtwWaitingForDetachOrRemove              = 4243,
    DsmStateEtwWaitingForDetachOrRemoveInConfigured  = 4244,
    DsmStateEtwWaitingForDetachOrRemoveInDisabledWithPortOff = 4245,
    DsmStateEtwWaitingForDetachOrRemoveInUnConfigured = 4246,
    DsmStateEtwWaitingForDetachOrRemoveWithPortOff   = 4247,
    DsmStateEtwWaitingForDetachOrReset               = 4248,
    DsmStateEtwWaitingForDetachOrResetWithDeviceDeporgrammed = 4249,
    DsmStateEtwWaitingForDetachOrToBeDeleted         = 4250,
    DsmStateEtwWaitingForDetachOrToBeDeletedWithPortOff = 4251,
    DsmStateEtwWaitingForDisableCompleteOnHubStopFromStoppedEnabled = 4252,
    DsmStateEtwWaitingForPDOD0ExitOnDetachInConfigured = 4253,
    DsmStateEtwWaitingForPDOD0ExitOnDetachInUnConfigured = 4254,
    DsmStateEtwWaitingForPDOReportedMissing          = 4255,
    DsmStateEtwWaitingForPortResumeResponseOnStop    = 4256,
    DsmStateEtwWaitingForPortResumeResponseOnSuspend = 4257,
    DsmStateEtwWaitingForRemoveOnDetach              = 4258,
    DsmStateEtwWaitingForRemoveOrReprotedMissingOnHubStopOnDetach = 4259,
    DsmStateEtwWaitingForReportedMissingOnDetachInConfigured = 4260,
    DsmStateEtwWaitingForReportedMissingOnDetachInUnConfigured = 4261,
    DsmStateEtwWaitingToBeDeleted                    = 4262,
    DsmStateEtwQueryingLanguageSpecificProductIdString = 4263,
    DsmStateEtwReturningOperationSuccessInQueryingProductId = 4264,
    DsmStateEtwSignalingDeviceQueryTextEvent         = 4265,
    DsmStateEtwValidatingLanguageSpecificProductIdString = 4266,
    DsmStateEtwAcquiringAddressZeroOwnershipInEnum   = 4267,
    DsmStateEtwAcquiringPowerReferenceOnStartInEnum  = 4268,
    DsmStateEtwCancellingAcquiringAddress0OnDetachInEnum = 4269,
    DsmStateEtwCancellingRetryTimerOnDetachInEnum    = 4270,
    DsmStateEtwCancellingRetryTimerOnStopSuspendInEnum = 4271,
    DsmStateEtwCheckingIfDevicePDChargingPolicySupported = 4890,
    DsmStateEtwCheckingIfEnumRetryReachedMaximumInEnum = 4272,
    DsmStateEtwCheckingIfEnumRetryReachedMaximumInEnumWithAddr0Ownership = 4273,
    DsmStateEtwClearingDeviceProgrammingLostFlagOnEnum = 4274,
    DsmStateEtwDeletingDefaultEndpointAndDeviceFromUCXOnDetachInEnumeratingAfterAddr = 4275,
    DsmStateEtwDeletingDefaultEndpointAndDeviceFromUCXOnFailureInEnum = 4276,
    DsmStateEtwDeletingDefaultEndpointAndDeviceOnPostAddressFailureInEnum = 4277,
    DsmStateEtwDeletingDefaultEndpointAndDeviceOnStopSuspendAfterAddressInEnum = 4278,
    DsmStateEtwDeletingDeviceAndDefaultEndpointOnDetachWithAddress0OwnershipInEnum = 4279,
    DsmStateEtwDeletingDeviceAndDefaultEndpointOnStopSuspendWithAddress0OwnershipInEnum = 4280,
    DsmStateEtwDisablingDeviceInControllerOnDetachInEnumeratingAfterAddr = 4281,
    DsmStateEtwDisablingDeviceInControllerOnDetachWithAddress0OwnershipInEnum = 4282,
    DsmStateEtwDisablingDeviceInControllerOnPostAddressEnumFailureInEnum = 4283,
    DsmStateEtwDisablingDeviceInControllerOnSetAddressFailureInEnum = 4284,
    DsmStateEtwDisablingDeviceInControllerOnStopSuspendAfterAddressInEnum = 4285,
    DsmStateEtwDisablingDeviceInControllerOnStopSuspendWithAddress0OwnershipInEnum = 4286,
    DsmStateEtwDisablingOnEnumAfterFailureInEnum     = 4287,
    DsmStateEtwDisablingOnEnumAfterFailureInEnumWithAddress0Ownership = 4288,
    DsmStateEtwDisablingOnStopSuspendInEnum          = 4289,
    DsmStateEtwDisablingOnStopSuspendInEnumWithAddress0Ownership = 4290,
    DsmStateEtwDisablingOnStopSuspendOnIoctlFailureInEnum = 4291,
    DsmStateEtwEnumeratingAfterAddressInEnum         = 4292,
    DsmStateEtwEnumeratingAtAddressZeroInEnum        = 4293,
    DsmStateEtwInitializingEnumRetryCountInEnum      = 4294,
    DsmStateEtwReleasingAddressZeroOwnershipInEnum   = 4295,
    DsmStateEtwReleasingAddressZeroOwnershipOnDetach = 4296,
    DsmStateEtwReleasingAddressZeroOwnershipOnDetachInEnum = 4297,
    DsmStateEtwReleasingAddressZeroOwnershipOnEnumFailure = 4298,
    DsmStateEtwReleasingAddressZeroOwnershipOnStopSuspend = 4299,
    DsmStateEtwReleasingAddressZeroOwnershipOnStopSuspendInEnum = 4300,
    DsmStateEtwReleasingPowerReferenceInEnum         = 4301,
    DsmStateEtwReturningOperationFailureInEnum       = 4302,
    DsmStateEtwReturningOperationSuccessInEnum       = 4303,
    DsmStateEtwReturningPortDetachDeviceInEnum       = 4304,
    DsmStateEtwSettingDevicePDChargingPolicy         = 4891,
    DsmStateEtwSettingLPMValues                      = 4305,
    DsmStateEtwSettingNonZeroAddressInEnum           = 4306,
    DsmStateEtwStartingTimerForEnumRetryInEnum       = 4307,
    DsmStateEtwStartingTimerForEnumRetryInEnumWithAddr0Ownership = 4308,
    DsmStateEtwStoppedSuspendedInEnum                = 4309,
    DsmStateEtwUpdatingDeviceInformationInEnum       = 4310,
    DsmStateEtwWaitingForDisableCompleteOnStopSuspendAfterFailureInEnum = 4311,
    DsmStateEtwWaitingForDisableCompleteOnStopSuspendInEnum = 4312,
    DsmStateEtwWaitingForDisableCompleteOnStopSuspendInEnumWithAddress0Ownership = 4313,
    DsmStateEtwWaitingForTimerToFlushOnDetachInEnum  = 4314,
    DsmStateEtwWaitingForTimerToFlushOnStopSuspendInEnum = 4315,
    DsmStateEtwWaitingForUCXIoctlOnDetachInEnum      = 4316,
    DsmStateEtwWaitingForUCXIoctlOnStopSuspendInEnum = 4317,
    DsmStateEtwCheckingIfFirstEnumTryAfterReset1     = 4909,
    DsmStateEtwCheckingIfFirstEnumTryAfterReset2     = 4910,
    DsmStateEtwCheckingIfSecondResetIsNeeded         = 4319,
    DsmStateEtwCreatingUCXDefaultEndpoint            = 4320,
    DsmStateEtwCreatingUCXDevice                     = 4321,
    DsmStateEtwDeletingUCXDefaultEndpointAndDeviceOnDetach = 4322,
    DsmStateEtwDeletingUCXDefaultEndpointAndDeviceOnStop = 4323,
    DsmStateEtwDeletingUCXDefaultEndpointAndDeviceOnSuspend = 4324,
    DsmStateEtwDeletingUCXDefaultEndpointOnOperationFailure = 4325,
    DsmStateEtwDeletingUCXDeviceOnOperationFailure   = 4326,
    DsmStateEtwDisablingDeviceInUCXOnDetach          = 4327,
    DsmStateEtwDisablingDeviceInUCXOnOperationFailure = 4328,
    DsmStateEtwDisablingDeviceInUCXOnStop            = 4329,
    DsmStateEtwDisablingDeviceInUCXOnSuspend         = 4330,
    DsmStateEtwDisablingPortOnHubStopInEnumAtAddr0   = 4331,
    DsmStateEtwDisablingPortOnHubSuspendInEnumAtAddr0 = 4332,
    DsmStateEtwEnablingDeviceInUCX                   = 4333,
    DsmStateEtwGettingDeviceDescriptorInEnumAtZero   = 4334,
    DsmStateEtwLoggingReset1TimeoutInEnumAtZero      = 4335,
    DsmStateEtwLoggingReset2TimeoutInEnumAtZero      = 4336,
    DsmStateEtwNotifyingDeviceResetToUCXAfterReset2  = 4337,
    DsmStateEtwResetting1                            = 4338,
    DsmStateEtwResetting2                            = 4339,
    DsmStateEtwReturningDeviceOperationFailureInEnumAtZero = 4340,
    DsmStateEtwReturningDeviceOperationSuccessInEnumAtZero = 4341,
    DsmStateEtwReturningHubStopInEnumAtZero          = 4342,
    DsmStateEtwReturningHubSuspendInEnumAtZero       = 4343,
    DsmStateEtwReturningPortDetachDeviceInEnumAtZero = 4344,
    DsmStateEtwSettingSpeedFlagFor20Devices          = 4847,
    DsmStateEtwStoppingTimerOnHubStopInEnumAtZero    = 4345,
    DsmStateEtwStoppingTimerOnHubSuspendInEnumAtZero = 4346,
    DsmStateEtwStoppingTimerOnPortDetachInEnumAtZero = 4347,
    DsmStateEtwUpdatingDefaultEndpointMaxPacketSizeInEnum = 4348,
    DsmStateEtwValidatingDeviceDescriptorInEnumAtZero = 4349,
    DsmStateEtwWaitingForPortReset1OrDisableOnHubStopInEnumAtZero = 4350,
    DsmStateEtwWaitingForPortReset1OrDisableOnHubSuspendInEnumAtZero = 4351,
    DsmStateEtwWaitingForPortReset2OnHubStopInEnumAtZero = 4352,
    DsmStateEtwWaitingForPortReset2OnHubSuspendInEnumAtZero = 4353,
    DsmStateEtwWaitingForPostReset1ExtendedTimer     = 4911,
    DsmStateEtwWaitingForPostReset1Timer             = 4354,
    DsmStateEtwWaitingForPostReset2ExtendedTimer     = 4355,
    DsmStateEtwWaitingForPostReset2Timer             = 4356,
    DsmStateEtwWaitingForStopSuspendOnReset1InEnumAtZero = 4357,
    DsmStateEtwWaitingForStopSuspendOnReset2InEnumAtZero = 4358,
    DsmStateEtwWaitingForTimerOnHubStopInEnumAtZero  = 4359,
    DsmStateEtwWaitingForTimerOnHubSuspendInEnumAtZero = 4360,
    DsmStateEtwWaitingForTimerOnPortDetachInEnumAtZero = 4361,
    DsmStateEtwCreatingUCXDefaultEndpointFor30       = 4362,
    DsmStateEtwCreatingUCXDevice30                   = 4363,
    DsmStateEtwDeletingUCXDefaultEndpointOnOperationFailureFor30 = 4364,
    DsmStateEtwDeletingUCXDefaultEndpointOnOperationFailureFor30OnDetach = 4365,
    DsmStateEtwDeletingUCXDefaultEndpointOnOperationFailureFor30OnHubStop = 4366,
    DsmStateEtwDeletingUCXDefaultEndpointOnOperationFailureFor30OnHubSuspend = 4367,
    DsmStateEtwDeletingUCXDeviceOnOperationFailureFor30 = 4368,
    DsmStateEtwDeletingUCXDeviceOnOperationFailureFor30OnDetach = 4369,
    DsmStateEtwDeletingUCXDeviceOnOperationFailureFor30OnHubStop = 4370,
    DsmStateEtwDeletingUCXDeviceOnOperationFailureFor30OnHubSuspend = 4371,
    DsmStateEtwEnablingDeviceInUCXFor30              = 4372,
    DsmStateEtwResettingDeviceInEnumeration          = 4373,
    DsmStateEtwReturningDetachInPreAddressEnumFor30  = 4374,
    DsmStateEtwReturningDeviceOperationFailureInPreAddressEnumFor30 = 4375,
    DsmStateEtwReturningDeviceOperationSuccessInPreAddressEnumFor30 = 4376,
    DsmStateEtwReturningHubStopInPreAddressEnumFor30 = 4377,
    DsmStateEtwReturningHubSuspendInPreAddressEnumFor30 = 4378,
    DsmStateEtwSetting30Speed                        = 4889,
    DsmStateEtwCancellingControlTransferOnDetachInEnumAfterAddr = 4379,
    DsmStateEtwCancellingControlTransferOnHubStopInEnumAfterAddr = 4380,
    DsmStateEtwCancellingControlTransferOnHubSuspendInEnumAfterAddr = 4381,
    DsmStateEtwCheckingBytesReturnedInConfigDescriptor = 4382,
    DsmStateEtwCheckingIfAltEnumCmdCached            = 4868,
    DsmStateEtwCheckingIfSuperSpeedNeedsToBeDisabled = 4383,
    DsmStateEtwDiasablingSuperSpeed                  = 4384,
    DsmStateEtwGettingConfigurationDescriptorWithDefaultLength = 4385,
    DsmStateEtwGettingConfigurationDescriptorWithReturnedLength = 4386,
    DsmStateEtwGettingDeviceDescriptorAfterAddressing = 4387,
    DsmStateEtwGettingRemainingDescriptorsAfterConfigDescriptor = 4388,
    DsmStateEtwQueryingRegistryValuesForDevice       = 4389,
    DsmStateEtwReturningHubStopInEnumAfterAddr       = 4390,
    DsmStateEtwReturningHubSuspendInEnumAfterAddr    = 4391,
    DsmStateEtwReturningOperationFailureInEnumAfterAddr = 4392,
    DsmStateEtwReturningOperationSuccessInEnumAfterAddr = 4393,
    DsmStateEtwReturningPortDeviceDetachInEnumAfterAddr = 4394,
    DsmStateEtwSendingAltEnumCmdInEnumAfterAddr      = 4869,
    DsmStateEtwSettingVersionFlagInEnumerating       = 4395,
    DsmStateEtwValidatingConfigurationDescriptor     = 4396,
    DsmStateEtwValidatingDeviceDescriptorAfterAddressing = 4397,
    DsmStateEtwCancellingControlTransferOnDetachInRemainingDescriptors = 4398,
    DsmStateEtwCancellingControlTransferOnHubStopInRemainingDescriptors = 4399,
    DsmStateEtwCancellingControlTransferOnHubSuspendInRemainingDescriptors = 4400,
    DsmStateEtwCheckingIfIProductIdStringDescriptorShouldBeQueried = 4401,
    DsmStateEtwCheckingIfIProductIsZero              = 4402,
    DsmStateEtwGettingBosDescriptorSet               = 4809,
    DsmStateEtwGettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch = 4810,
    DsmStateEtwGettingLanguageIdStringDescriptor     = 4404,
    DsmStateEtwGettingMSOSAndSerialNumberDescriptor  = 4405,
    DsmStateEtwGettingProductIdStringDescriptor      = 4406,
    DsmStateEtwReturningErrorResponseOnLanguageIdQuery = 4407,
    DsmStateEtwReturningErrorResponseOnProductStringQuery = 4408,
    DsmStateEtwReturningHubStopInRemainingDescriptors = 4409,
    DsmStateEtwReturningHubSuspendInRemainingDescriptors = 4410,
    DsmStateEtwReturningOperationFailureInRemainingDescriptors = 4411,
    DsmStateEtwReturningOperationSuccessInRemainingDescriptors = 4412,
    DsmStateEtwReturningPortDeviceDetachInRemainingDescriptors = 4413,
    DsmStateEtwValidatingLanguageIdStringDescriptor  = 4414,
    DsmStateEtwValidatingProductIdStringDescriptor   = 4415,
    DsmStateEtwCheckingIfDeviceSupportsContainerId   = 4416,
    DsmStateEtwCheckingIfIgnoreHWSerNumIsSet         = 4417,
    DsmStateEtwCheckingIfMSOSContainerIdDescriptorIsSupported = 4418,
    DsmStateEtwCheckingIfMSOSDescriptorShouldBeQueried = 4419,
    DsmStateEtwCheckingIfMSOSExtendedConfigDescriptorIsSupported = 4420,
    DsmStateEtwCheckingIfSerialNumberStringIndexIsZero = 4421,
    DsmStateEtwGettingMSOSContainerIdDescriptor      = 4422,
    DsmStateEtwGettingMSOSContainerIdHeaderDescriptor = 4423,
    DsmStateEtwGettingMSOSDescriptor                 = 4424,
    DsmStateEtwGettingMSOSExtendedDescriptor         = 4425,
    DsmStateEtwGettingMSOSExtendedDescriptorHeader   = 4426,
    DsmStateEtwGettingSerialNumberStringDescriptor   = 4427,
    DsmStateEtwMarkDeviceAsNotSupportingMSOSDescriptor = 4428,
    DsmStateEtwMarkDeviceAsSupportingMSOSDescriptor  = 4429,
    DsmStateEtwMarkingDeviceAsNotSupportingContainerId = 4430,
    DsmStateEtwMarkingDeviceAsNotSupportingContainerIdBasedOnMSOSDescriptor = 4431,
    DsmStateEtwReturningErrorResponseOnContainerIdHeaderQuery = 4432,
    DsmStateEtwReturningErrorResponseOnContainerIdQuery = 4433,
    DsmStateEtwReturningErrorResponseOnMSOSExtendedHeaderQuery = 4434,
    DsmStateEtwReturningErrorResponseOnMSOSExtendedQuery = 4435,
    DsmStateEtwReturningErrorResponseOnMSOSQuery     = 4436,
    DsmStateEtwReturningErrorResponseOnSerialNumberQuery = 4437,
    DsmStateEtwReturningOperationFailureInGettingDescriptorsForGreaterThan1x = 4438,
    DsmStateEtwReturningOperationSuccessInGettingDescriptorsForGreaterThan1x = 4439,
    DsmStateEtwValidatingMSOSContainerIdDescriptor   = 4440,
    DsmStateEtwValidatingMSOSContainerIdHeaderDescriptor = 4441,
    DsmStateEtwValidatingMSOSDescriptor              = 4442,
    DsmStateEtwValidatingMSOSExtendedConfigDescriptor = 4443,
    DsmStateEtwValidatingMSOSExtendedConfigDescriptorHeader = 4444,
    DsmStateEtwValidatingSerialNumberStringDescriptor = 4445,
    DsmStateEtwCheckingIfIgnoreHWSerNumIsSetFor1xDevice = 4446,
    DsmStateEtwCheckingIfSerialNumberStringIndexIsZeroFor1xDevice = 4447,
    DsmStateEtwGettingSerialNumberStringDescriptorFor1xDevice = 4448,
    DsmStateEtwReturningErrorResponseOnSerialNumberQueryFor1xDevice = 4449,
    DsmStateEtwReturningOperationFailureInGettingSerialNumberFor1x = 4450,
    DsmStateEtwReturningOperationSuccessInGettingSerialNumberFor1x = 4451,
    DsmStateEtwValidatingSerialNumberStringDescriptorFor1xDevice = 4452,
    DsmStateEtwCheckingIfWaitRequiredAfterSetAddress = 4453,
    DsmStateEtwReturningHubStopInSettingAddress      = 4454,
    DsmStateEtwReturningHubSuspendInSettingAddress   = 4455,
    DsmStateEtwReturningOperationFailureInSettingAddress = 4456,
    DsmStateEtwReturningOperationSuccessInSettingAddress = 4457,
    DsmStateEtwReturningPortDetachDeviceInSettingAddress = 4458,
    DsmStateEtwSettingNonZeroAddress                 = 4459,
    DsmStateEtwStoppingTimerOnHubStopInSettingAddress = 4460,
    DsmStateEtwStoppingTimerOnHubSuspendInSettingAddress = 4461,
    DsmStateEtwStoppingTimerOnPortDetachInSettingAddress = 4462,
    DsmStateEtwWaitingForTimerAfterSetAddress        = 4463,
    DsmStateEtwWaitingForTimerOnHubStopInSettingAddress = 4464,
    DsmStateEtwWaitingForTimerOnHubSuspendInSettingAddress = 4465,
    DsmStateEtwWaitingForTimerOnPortDetachInSettingAddress = 4466,
    DsmStateEtwCheckingBytesReturnedInAlternateConfigDescriptor = 4832,
    DsmStateEtwCheckingIfAltEnumCommandNeeded        = 4871,
    DsmStateEtwCheckingIfAlternateBOSDescriptorQueryShouldBeSkipped = 4834,
    DsmStateEtwCheckingIfAlternateModeStringDescriptorShouldBeQueried = 4896,
    DsmStateEtwCheckingIfBillboardBOSDescriptorIsPresent = 4905,
    DsmStateEtwCheckingIfBillboardStringDescriptorShouldBeQueried = 4897,
    DsmStateEtwCheckingIfBOSDescriptorQueryShouldBeSkipped = 4468,
    DsmStateEtwCheckingIfCompleteAlternateBOSDescriptorWasRetrieved = 4835,
    DsmStateEtwCheckingIfCompleteBOSDescriptorWasRetrieved = 4469,
    DsmStateEtwCheckingIfDualRoleFeaturesSupported   = 4902,
    DsmStateEtwGetMsOs20DescriptorSet                = 4813,
    DsmStateEtwGettingAlternateBOSDescriptor         = 4836,
    DsmStateEtwGettingAlternateBOSDescriptorHeader   = 4837,
    DsmStateEtwGettingAlternateConfigurationDescriptorWithDefaultLength = 4838,
    DsmStateEtwGettingAlternateConfigurationDescriptorWithReturnedLength = 4839,
    DsmStateEtwGettingAlternateDeviceDescriptor      = 4845,
    DsmStateEtwGettingAlternateModeStringDescriptor  = 4898,
    DsmStateEtwGettingBillboardStringDescriptor      = 4899,
    DsmStateEtwGettingBOSDescriptor                  = 4471,
    DsmStateEtwGettingBOSDescriptorHeader            = 4472,
    DsmStateEtwIsMsOs20DescriptorSupported           = 4816,
    DsmStateEtwQueryingRegistryValuesForAlternateDeviceEnumeration = 4840,
    DsmStateEtwReturningErrorResponseOnBOSDescriptorHeaderQuery = 4473,
    DsmStateEtwReturningErrorResponseOnBOSDescriptorQuery = 4474,
    DsmStateEtwReturningOperationFailureInGettingBOSDescriptor = 4475,
    DsmStateEtwReturningOperationSuccessInGettingBOSDescriptor = 4476,
    DsmStateEtwSendingMsOs20AlternateEnumerationCommand = 4872,
    DsmStateEtwSendingUsbFeaturesVendorCmd           = 4903,
    DsmStateEtwSettingVersionFlagInAlternateEnumeration = 4841,
    DsmStateEtwValidatingAlternateBOSDescriptor      = 4842,
    DsmStateEtwValidatingAlternateBOSDescriptorHeader = 4843,
    DsmStateEtwValidatingAlternateConfigurationDescriptor = 4817,
    DsmStateEtwValidatingAlternateDeviceDescriptor   = 4844,
    DsmStateEtwValidatingAndLoggingAlternateModeStringDescriptor = 4900,
    DsmStateEtwValidatingAndLoggingBillboardStringDescriptor = 4901,
    DsmStateEtwValidatingBOSDescriptor               = 4477,
    DsmStateEtwValidatingBOSDescriptorHeader         = 4478,
    DsmStateEtwValidatingMsOs20DescriptorSetIfPresent = 4818,
    DsmStateEtwGettingDeviceQualifierDescriptor      = 4479,
    DsmStateEtwReturningErrorResponseOnDeviceQualifierQuery = 4480,
    DsmStateEtwReturningOperationSuccessInCheckingFor20FullSpeed = 4481,
    DsmStateEtwValidatingDeviceQualifierDescriptor   = 4482,
    DsmStateEtwAddingDeviceToGlobalChildList         = 4483,
    DsmStateEtwCheckingIfAddDeviceToGlobalListRetryCountExceededLimit = 4484,
    DsmStateEtwCreatingChildPDOAndReportingToPnp     = 4485,
    DsmStateEtwDiscardSerialNumber                   = 4486,
    DsmStateEtwInitializingAddDeviceToGlobalListRetryCount = 4487,
    DsmStateEtwRequestingDeviceCycleInReportingToPnp = 4488,
    DsmStateEtwReturningOperationSuccessInReportingToPnp = 4489,
    DsmStateEtwReturningPortDetachDevice             = 4490,
    DsmStateEtwStoppingTimerAndRequestingCycleOnStopSuspend = 4491,
    DsmStateEtwStoppingTimerOnDetachInReportingToPnp = 4492,
    DsmStateEtwWaitingForDetachInReportingToPnp      = 4493,
    DsmStateEtwWaitingForDetachOrTimerInReportingToPnp = 4494,
    DsmStateEtwWaitingForDuplicateDeviceToGoAway     = 4495,
    DsmStateEtwWaitingForTimerToFlushOnDetachInReportingToPnp = 4496,
    DsmStateEtwCreatingUnknownChildPDOAndReportingToPnp = 4497,
    DsmStateEtwMarkingUnknownDeviceAsFailed          = 4498,
    DsmStateEtwRequestingDeviceCycleForUnknownDevice = 4499,
    DsmStateEtwReturningOperationSuccessInReportingUnknownDevice = 4500,
    DsmStateEtwReturningPortDetachDeviceInReportingUnknownDevice = 4501,
    DsmStateEtwAckingResumed                         = 4502,
    DsmStateEtwAcquiringPowerReferenceInEnabled      = 4503,
    DsmStateEtwAcquiringPowerReferenceInSuspended    = 4504,
    DsmStateEtwAcquiringPowerReferenceInSuspendedOnResumeWithReset = 4505,
    DsmStateEtwAcquiringPowerReferenceOnHubResume    = 4506,
    DsmStateEtwAcquiringPowerReferenceOnHubResumeWithReenumRequired = 4507,
    DsmStateEtwAcquiringPowerReferenceOnHubResumeWtihReset = 4508,
    DsmStateEtwAcquiringPowerReferenceOnResumeInS0   = 4509,
    DsmStateEtwCheckingIfDeviceNeedsResetOnResumeInSxOnEnabled = 4510,
    DsmStateEtwCheckingIfDeviceNeedsResetOnResumeInSxOnSuspended = 4511,
    DsmStateEtwCompletingUnexpectedD0ExitInHubSuspended = 4512,
    DsmStateEtwDisabledOrFailedInHubSuspend          = 4513,
    DsmStateEtwDisablingPortOnPortEnabledInPendingHubStop = 4514,
    DsmStateEtwReleasingReferenceOnHubSuspendAfterPSMSyncUp = 4515,
    DsmStateEtwReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed = 4516,
    DsmStateEtwReleasingReferenceOnHubSuspendOnDetachAfterPSMSyncUp = 4882,
    DsmStateEtwResumingBeforeResetting               = 4517,
    DsmStateEtwReturningDeviceRenumerationOnHubResume = 4518,
    DsmStateEtwReturningDeviceResumedOnHubResume     = 4519,
    DsmStateEtwReturningDeviceSuspendedOnHubResume   = 4520,
    DsmStateEtwReturningHubStopWithReferenceAcquiredInHubSuspended = 4521,
    DsmStateEtwReturningPortDetach                   = 4522,
    DsmStateEtwSuspendedInHubSuspend                 = 4523,
    DsmStateEtwSuspendingPortOnPortEnabledInPendingHubSuspend = 4524,
    DsmStateEtwWaitingForHubResumeInEnabled          = 4525,
    DsmStateEtwWaitingForHubResumeInSuspended        = 4526,
    DsmStateEtwWaitingForHubResumeWithRenumRequired  = 4527,
    DsmStateEtwWaitingForPSMSyncUp                   = 4528,
    DsmStateEtwWaitingForPSMSyncUpOnPendingStop      = 4529,
    DsmStateEtwWaitingForPSMSyncUpOnPendingSuspend   = 4530,
    DsmStateEtwWaitingForPSMSyncUpOnResumeWithReset  = 4531,
    DsmStateEtwWaitingForResumeResponseFromPortOnHubStop = 4532,
    DsmStateEtwWaitingForResumeResponseFromPortOnHubSuspend = 4533,
    DsmStateEtwAcquiringAddressZeroOwnershipOnRenum  = 4534,
    DsmStateEtwAcquiringPowerReferenceOnStartInReEnum = 4535,
    DsmStateEtwCancellingAcquiringAddress0OnDetachOnRenum = 4536,
    DsmStateEtwCancellingRetryTimerOnDetachOnRenum   = 4537,
    DsmStateEtwCancellingRetryTimerOnStopSuspendOnRenum = 4538,
    DsmStateEtwCheckingIfAltEnumCmdNeededInReenum    = 4873,
    DsmStateEtwCheckingIfDevicePDChargingPolicySupportedInReEnum = 4892,
    DsmStateEtwCheckingIfDevicePorgrammingWasLosttInUnconfigured = 4539,
    DsmStateEtwCheckingIfDeviceProgrammingWasLosttInConfigured = 4540,
    DsmStateEtwCheckingIfEnumRetryReachedMaximumOnRenum = 4541,
    DsmStateEtwCheckingIfEnumRetryReachedMaximumOnRenumWithAddress0Ownership = 4542,
    DsmStateEtwCheckingIfEnumRetryReachedMaximumWithDeviceDisabledOnRenum = 4543,
    DsmStateEtwCheckingIfItIsBootDeviceOnIdComparisionFailure = 4544,
    DsmStateEtwCheckingIfRequestConfigDescOnResetIsSet = 4545,
    DsmStateEtwCheckingIfSerialNumberShouldBeCompared = 4546,
    DsmStateEtwClearingDeviceLostProgammingFlagOnRestart = 4547,
    DsmStateEtwClearingDeviceProgrammingLostFlagOnRenum = 4548,
    DsmStateEtwComparingDeviceOnReEnumeration        = 4549,
    DsmStateEtwComparingSerialNumberOnReEnumeration  = 4550,
    DsmStateEtwDeconfiguringEndpointsInControllerBeforeRenumerating = 4551,
    DsmStateEtwDisablingDeviceInControllerOnStopSuspendOnRenum = 4552,
    DsmStateEtwDisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership = 4553,
    DsmStateEtwDisablingOnEnumAfterFailureInReEnum   = 4554,
    DsmStateEtwDisablingOnEnumAfterFailureInReEnumWithAddress0Ownership = 4555,
    DsmStateEtwDisablingOnEnumAfterFailureWithDeviceDisabledInReEnum = 4556,
    DsmStateEtwEnablingDeviceInControllerInRenum     = 4557,
    DsmStateEtwEnumeratingAtAddressZeroOnRenum       = 4558,
    DsmStateEtwGettingConfigDescriptorWhileRenuemrating = 4559,
    DsmStateEtwGettingDeviceDescriptorOnRenum        = 4560,
    DsmStateEtwGettingSerialNumberStringDescriptorWhileRenuemrating = 4561,
    DsmStateEtwInitializingEnumRetryCountInReEnumDuringResetInConfigured = 4562,
    DsmStateEtwInitializingEnumRetryCountInReEnumDuringResetInUnConfigured = 4563,
    DsmStateEtwInitializingEnumRetryCountInReEnumDuringRestart = 4564,
    DsmStateEtwPurgingDeviceTreeIoOnReEnumerationInConfigured = 4566,
    DsmStateEtwPurgingDeviceTreeIoOnReEnumerationInUnConfigured = 4567,
    DsmStateEtwPurgingIoOnEnumAfterFailureInReEnum   = 4568,
    DsmStateEtwPurgingIoOnEnumAfterFailureInReEnumWithAddress0Ownership = 4569,
    DsmStateEtwReleasingAddressZeroOwnershipOnDetachOnEnumFailure = 4570,
    DsmStateEtwReleasingAddressZeroOwnershipOnDetachWithDeviceEnabledOnRenum = 4571,
    DsmStateEtwReleasingAddressZeroOwnershipOnDetachWithTimer = 4572,
    DsmStateEtwReleasingAddressZeroOwnershipOnEnumFailureInRenum = 4573,
    DsmStateEtwReleasingAddressZeroOwnershipOnRenum  = 4574,
    DsmStateEtwReleasingAddressZeroOwnershipOnStopSuspendOnEnumFailure = 4575,
    DsmStateEtwReleasingAddressZeroOwnershipOnStopSuspendOnRenum = 4576,
    DsmStateEtwReleasingAddressZeroOwnershipOnStopSuspendWithTimer = 4577,
    DsmStateEtwReleasingPowerReferenceOnRenum        = 4578,
    DsmStateEtwReturningOperationFailureOnRenum      = 4579,
    DsmStateEtwReturningOperationFailureWithDeviceEnabledOnRenum = 4580,
    DsmStateEtwReturningOperationSuccessOnRenum      = 4581,
    DsmStateEtwReturningPortDetachDeviceOnRenum      = 4582,
    DsmStateEtwReturningPortDetachDeviceWithDeviceEnabledOnRenum = 4583,
    DsmStateEtwSendingMsOs20AltEnumCmdOnReenum       = 4874,
    DsmStateEtwSettingDevicePDChargingPolicyInReEnum = 4893,
    DsmStateEtwSettingLPMValuesInReEnum              = 4584,
    DsmStateEtwSettingNonZeroAddressOnRenum          = 4585,
    DsmStateEtwStartingTimerForEnumRetryOnRenum      = 4586,
    DsmStateEtwStartingTimerForEnumRetryOnRenumWithAddress0Ownership = 4587,
    DsmStateEtwStoppedSuspendedOnRenum               = 4588,
    DsmStateEtwWaitingForDetachAfterWrongDeviceDetectedOnBootDevicePort = 4589,
    DsmStateEtwWaitingForDisableCompleteOnStopSuspendAfterFailureInReEnum = 4590,
    DsmStateEtwWaitingForDisableCompleteOnStopSuspendOnRenum = 4591,
    DsmStateEtwWaitingForDisableCompleteOnStopSuspendWithDeviceDisabledAfterFailureInReEnum = 4592,
    DsmStateEtwWaitingForTimerToFlushOnDetachOnRenum = 4593,
    DsmStateEtwWaitingForTimerToFlushOnStopSuspendOnRenum = 4594,
    DsmStateEtwWaitingForUCXIoctlOnDetachOnRenum     = 4595,
    DsmStateEtwWaitingForUCXIoctlOnStopSuspend       = 4596,
    DsmStateEtwCheckingIfAnyAlternateInterfaceLeft   = 4597,
    DsmStateEtwConfiguringDevice                     = 4598,
    DsmStateEtwCreatingNewEndpoints                  = 4599,
    DsmStateEtwDeconfiguringEndpointsInControllerForDeviceConfiguration = 4600,
    DsmStateEtwDeletingEndpointsForOldConfiguration  = 4601,
    DsmStateEtwMarkingAnyEndpointsToBeEnableAsDisabled = 4603,
    DsmStateEtwProgrammingEndpointsAndSettingLPMValuesrForDeviceConfiguration = 4854,
    DsmStateEtwProgrammingEndpointsInControllerForDeviceConfigurationFor20Devices = 4855,
    DsmStateEtwReturningOperationFailureInReConfiguring = 4605,
    DsmStateEtwReturningOperationSuccessInReConfiguring = 4606,
    DsmStateEtwSettingDeviceInterfaceInConfiguring   = 4607,
    DsmStateEtwDisablingPortOnHubSuspendInCleanup    = 4609,
    DsmStateEtwFlushingHubPowerDownEvents            = 4610,
    DsmStateEtwFlushingHubPowerUpEvents              = 4611,
    DsmStateEtwFlushingHubStopAfterSuspendEvent      = 4612,
    DsmStateEtwFlushingPnpEvents                     = 4613,
    DsmStateEtwFowardingStreamsRequestToUcxInWaitingForDevicePortEvents = 4614,
    DsmStateEtwReleasingPowerReferenceOnHubSuspendInCleanup = 4615,
    DsmStateEtwReturningPortDetachDeviceFromIgnoringDevicePort = 4864,
    DsmStateEtwReturningPortDisabledOnHubSuspendInCleanup = 4616,
    DsmStateEtwWaitingForDevicePortEvents            = 4617,
    DsmStateEtwFlushingHubPowerDownEventsWithPortOff = 4618,
    DsmStateEtwFlushingHubPowerUpEventsWithPortOff   = 4619,
    DsmStateEtwFlushingHubStopAfterSuspendEventWithPortOff = 4620,
    DsmStateEtwFlushingPnpEventsWithPortOff          = 4621,
    DsmStateEtwFowardingStreamsRequestToUcxInWaitingForDevicePortEventsWithPortOff = 4622,
    DsmStateEtwWaitingForDevicePortEventsWithPortOff = 4623,
    DsmStateEtwDisablingPortOnHubSuspendForFailedDevice = 4624,
    DsmStateEtwFailedDeviceHubSuspended              = 4867,
    DsmStateEtwFlushingHubPowerDownEventsForFailedDevice = 4625,
    DsmStateEtwFlushingHubPowerUpEventsForFailedDevice = 4626,
    DsmStateEtwFlushingHubStopAfterSuspendEventForFailedDevice = 4627,
    DsmStateEtwFlushingPnpEventsForFailedDevice      = 4628,
    DsmStateEtwReleasingPowerReferenceOnDetachForFailedDevice = 4884,
    DsmStateEtwReturningPortDetachDeviceForFailedDevice = 4865,
    DsmStateEtwWaitingForDevicePortEventsForFailedDevice = 4629,
    DsmStateEtwFlushingHubStartResumeEvent           = 4630,
    DsmStateEtwFlushingHubSuspendEvent               = 4631,
    DsmStateEtwFlushingPnpEventExceptStop            = 4632,
    DsmStateEtwFowardingStreamsRequestToUcxInWaitingForDevicePortEventsExceptStop = 4633,
    DsmStateEtwWaitingForDevicePortEventsExceptStop  = 4634,
    DsmStateEtwFlushingHubResumeEventForBootDevice   = 4635,
    DsmStateEtwFlushingHubSuspendEventForBootDevice  = 4636,
    DsmStateEtwFlushingPnpEventsForBootDevice        = 4637,
    DsmStateEtwWaitingForDeviceHubEventsForBootDevice = 4638,
    DsmStateEtwAckingPortEventInD3Cold               = 4639,
    DsmStateEtwAckingPortResumed                     = 4640,
    DsmStateEtwCheckingIfDeviceArmedForWakeOnResume  = 4641,
    DsmStateEtwCheckingIfDeviceNeedsResetOnResumeInS0 = 4642,
    DsmStateEtwCheckingIfDeviceShouldBeDisarmedForWakeOnResume = 4802,
    DsmStateEtwCheckingIfResetOnLastResumeFlagIsSet  = 4643,
    DsmStateEtwCheckingIfResetOnLastResumeFlagIsSetOnSuspended = 4644,
    DsmStateEtwCompletingD0EntryOnDetach             = 4645,
    DsmStateEtwCompletingD0EntryOnPortFailure        = 4646,
    DsmStateEtwDisabledAfterD3Cold                   = 4647,
    DsmStateEtwDisablingOnHubSuspendNeedingRenumeration = 4648,
    DsmStateEtwDisablingPortOnStopFromSuspended      = 4649,
    DsmStateEtwDisarmingDeviceForWakeOnD0Entry       = 4650,
    DsmStateEtwDisarmingDeviceForWakeOnPortResumed   = 4651,
    DsmStateEtwPurgingDeviceIoOnHubSuspendWhileWaitingForD0Entry = 4652,
    DsmStateEtwPurgingDeviceIoOnHubSuspendWhileWaitingForD0EntryOnHwWake = 4653,
    DsmStateEtwReleasingPowerReferenceOnDetachWhileSuspending = 4904,
    DsmStateEtwReleasingPowerReferenceOnDetachWhileWaitingForHubSuspend = 4886,
    DsmStateEtwReleasingPowerReferenceOnPortFailureWhileWaitingForHubSuspend = 4887,
    DsmStateEtwResumingFromSelectiveSuspend          = 4654,
    DsmStateEtwReturningDetachDevice                 = 4655,
    DsmStateEtwReturningDeviceResumed                = 4656,
    DsmStateEtwReturningHubStopFromSuspended         = 4657,
    DsmStateEtwReturningPortFailureOnResume          = 4658,
    DsmStateEtwReturningRenumerateDeviceOnResume     = 4659,
    DsmStateEtwSettingResetOnLastResumeDueToD3Cold   = 4660,
    DsmStateEtwSettingResetOnLastResumeFlagForPDO    = 4661,
    DsmStateEtwSettingResetOnLastResumeFlagForPDOAfterD3Cold = 4662,
    DsmStateEtwStartingDeviceIoOnDeviceResumeOnHwWake = 4663,
    DsmStateEtwStartingDeviceIoOnDeviceResumeOnSwWake = 4664,
    DsmStateEtwSuspended                             = 4665,
    DsmStateEtwSuspendedWithHubSuspended             = 4666,
    DsmStateEtwSuspendedWithHubSuspendedInD3Cold     = 4667,
    DsmStateEtwSuspendingBackUnarmedDevice           = 4668,
    DsmStateEtwSuspendingWakeArmedDeviceOnHubSuspend = 4669,
    DsmStateEtwWaitingForD0EntryOnHwWake             = 4670,
    DsmStateEtwWaitingForD0EntryOnPendingRenumerate  = 4671,
    DsmStateEtwWaitingForSuspendCompleteOnHubSuspend = 4672,
    DsmStateEtwAbortingDeviceIoInSuspendingBeforeArmingForWakeFailure = 4803,
    DsmStateEtwArmingDeviceForWake                   = 4673,
    DsmStateEtwCheckingIfDeviceArmedForWakeOnSuspending = 4674,
    DsmStateEtwCompletingWaitWakeOnDetachDuringSuspending = 4675,
    DsmStateEtwCompletingWaitWakeOnSuspendFailure    = 4676,
    DsmStateEtwPurgingDeviceIoInSuspending           = 4677,
    DsmStateEtwPurgingDeviceIoInSuspendingAfterArmingForWake = 4678,
    DsmStateEtwPurgingDeviceIoInSuspendingAfterArmingForWakeFailure = 4679,
    DsmStateEtwReturningOperationFailureInSuspending = 4680,
    DsmStateEtwReturningOperationSuccessInSuspending = 4681,
    DsmStateEtwReturningPortDetachDeviceInSuspending = 4682,
    DsmStateEtwReturningUnexpectedHubSuspendInSuspending = 4683,
    DsmStateEtwSuspending                            = 4684,
    DsmStateEtwSuspendingPortOnFailureInSuspending   = 4685,
    DsmStateEtwSuspendingWithArmedForWake            = 4686,
    DsmStateEtwWaitingForPortResponseOnHubSuspend    = 4687,
    DsmStateEtwCreatingNewEndpointsInSettingInterface = 4688,
    DsmStateEtwDeletingEndpointsForNewInterfaceOnFailure = 4689,
    DsmStateEtwDeletingEndpointsForOldInterface      = 4690,
    DsmStateEtwDeletingEndpointsForOldInterfaceOnFailure = 4691,
    DsmStateEtwDisablingEndpointsForTheCurrentInterface = 4692,
    DsmStateEtwDisablingEndpointsForTheNewInterfaceOnFailure = 4693,
    DsmStateEtwPreparingEndpointAndInterfaceListsOnSelectInterface = 4694,
    DsmStateEtwProgrammingEndpointsAndSettingLPMValuesInSettingInterface = 4856,
    DsmStateEtwProgrammingEndpointsInControllerInSettingInterfaceFor20Devices = 4857,
    DsmStateEtwReturningOperationFailureInSettingInterface = 4696,
    DsmStateEtwReturningOperationSuccessInSettingInterface = 4697,
    DsmStateEtwSettingDeviceInterfaceInSettingInterface = 4698,
    DsmStateEtwSettingDeviceInterfaceInSettingInterfaceOnFailureForCompat = 4699,
    DsmStateEtwCheckingIfDeviceSpeedChanged          = 4701,
    DsmStateEtwCheckingIfFirstEnumTryInRenum         = 4702,
    DsmStateEtwGettingDeviceDescriptorInReEnumAtZero = 4703,
    DsmStateEtwLoggingReset1TimeoutInReEnumAtZero    = 4704,
    DsmStateEtwLoggingReset2TimeoutInReEnum          = 4705,
    DsmStateEtwNotifyingDeviceResetToUCXAfterReset1InRenum = 4706,
    DsmStateEtwNotifyingDeviceResetToUCXAfterReset2InRenum = 4707,
    DsmStateEtwResetting1InRenum                     = 4708,
    DsmStateEtwResetting2InRenum                     = 4709,
    DsmStateEtwReturningDeviceOperationFailureInReEnumAtZero = 4710,
    DsmStateEtwReturningDeviceOperationSuccessInReEnumAtZero = 4711,
    DsmStateEtwReturningHubStopInReEnumAtZero        = 4712,
    DsmStateEtwReturningHubSuspendInReEnumAtZero     = 4713,
    DsmStateEtwReturningPortDetachDeviceInReEnumAtZero = 4714,
    DsmStateEtwStoppingTimerOnHubStopInReEnumAtZero  = 4715,
    DsmStateEtwStoppingTimerOnHubSuspendInReEnumAtZero = 4716,
    DsmStateEtwStoppingTimerOnPortDetachInReEnumAtZero = 4717,
    DsmStateEtwValidatingDeviceDescriptorInReEnumAtZero = 4718,
    DsmStateEtwWaitingForPortResetOnHubStopInReEnumAtZero = 4719,
    DsmStateEtwWaitingForPortResetOnHubSuspendInReEnumAtZero = 4720,
    DsmStateEtwWaitingForPostReset1TimerInRenum      = 4721,
    DsmStateEtwWaitingForPostReset2ExtendedTimerInRenum = 4722,
    DsmStateEtwWaitingForPostReset2TimerInRenum      = 4723,
    DsmStateEtwWaitingForStopSuspendOnReset1InReEnumAtZero = 4724,
    DsmStateEtwWaitingForTimerOnHubStopInReEnumAtZero = 4725,
    DsmStateEtwWaitingForTimerOnHubSuspendInReEnumAtZero = 4726,
    DsmStateEtwWaitingForTimerOnPortDetachInReEnumAtZero = 4727,
    DsmStateEtwCheckingIfFirstReEnumTryFor30Device   = 4728,
    DsmStateEtwDeterminingPortResetTypeRequired      = 4729,
    DsmStateEtwLoggingResetTimeoutInResettingFor30   = 4730,
    DsmStateEtwNotifyingDeviceResetToUCXInResettingFor30 = 4731,
    DsmStateEtwQueueingHotPortResetFor30             = 4732,
    DsmStateEtwQueueingWarmPortResetFor30            = 4733,
    DsmStateEtwReturningDeviceOperationFailureInResettingFor30 = 4734,
    DsmStateEtwReturningDeviceOperationSuccessInInResettingFor30 = 4735,
    DsmStateEtwReturningHubStopInResettingFor30      = 4736,
    DsmStateEtwReturningHubSuspendInResettingFor30   = 4737,
    DsmStateEtwReturningPortDetachDeviceInResettingFor30 = 4738,
    DsmStateEtwStartingPostResetTimerFor30           = 4739,
    DsmStateEtwStoppingTimerOnHubStopInResettingFor30 = 4740,
    DsmStateEtwStoppingTimerOnHubSuspendInResettingFor30 = 4741,
    DsmStateEtwStoppingTimerOnPortDetachInResettingFor30 = 4742,
    DsmStateEtwWaitingForPortResetCompleteFor30      = 4743,
    DsmStateEtwWaitingForPortResetOnHubStopInResettingFor30 = 4744,
    DsmStateEtwWaitingForPortResetOnHubSuspendInResettingFor30 = 4745,
    DsmStateEtwWaitingForStopSuspendOnReset1InResettingFor30 = 4746,
    DsmStateEtwWaitingForTimerOnHubStopInResettingFor30 = 4747,
    DsmStateEtwWaitingForTimerOnHubSuspendInResettingFor30 = 4748,
    DsmStateEtwWaitingForTimerOnPortDetachInResettingFor30 = 4749,
    DsmStateEtwAllocatingBufferForMsOsExtendedPropertyDescriptor = 4750,
    DsmStateEtwCheckingIfMsOs20RegistryValuesShouldBeInstalled = 4819,
    DsmStateEtwCheckingIfMSOSExtendedPropertyDescriptorShouldBeQueried = 4751,
    DsmStateEtwFreeingBufferAllocatedForMSOSExtendedProperty = 4752,
    DsmStateEtwGettingMSOSExtendedPropertyDescriptor = 4754,
    DsmStateEtwGettingMSOSExtendedPropertyDescriptorHeader = 4755,
    DsmStateEtwInstallingMsOs20RegistryValues        = 4820,
    DsmStateEtwSettingExtPropDescSemaphoreForMsOs2   = 4821,
    DsmStateEtwSettingExtPropDescSemaphoreRegistryFlag = 4758,
    DsmStateEtwSignalingPnpEventForMSOSExtInstall    = 4808,
    DsmStateEtwValidatingMSOSExtendedPropertyDescriptor = 4759,
    DsmStateEtwValidatingMSOSExtendedPropertyDescriptorHeader = 4760,
    DsmStateEtwWaitingForInstallMSOSExt              = 4807,
    DsmStateEtwWritingCustomPropertiesInRegistry     = 4761,
    DsmStateEtwCheckingIfLTMShouldBeEnabled          = 4908,
    DsmStateEtwCheckingIfSetIsochDelayShouldBeSkipped = 4762,
    DsmStateEtwCheckingIfSetSelShouldBeSkipped       = 4763,
    DsmStateEtwEnablingLTM                           = 4907,
    DsmStateEtwReturningOperationFailureInSettingLPMValues = 4764,
    DsmStateEtwReturningOperationSuccessInSettingLPMValues = 4765,
    DsmStateEtwSettingSEL                            = 4766,
    DsmStateEtwSetttingIsochDelay                    = 4767,
    DsmStateEtwCheckingIfEnableU1NeedsToBeUpdated    = 4768,
    DsmStateEtwCheckingIfEnableU2NeedsToBeUpdated    = 4769,
    DsmStateEtwCheckingIfEndpointsNeedToBeConfiguredAfterDecreasingExitLatency = 4848,
    DsmStateEtwCheckingIfEndpointsNeedToBeProgrammedAfterIncreasingLatency = 4849,
    DsmStateEtwCheckingIfEndpointsToBeDisabled       = 4858,
    DsmStateEtwCheckingIfEndpointsToBeDisabledOnDetach = 4875,
    DsmStateEtwCheckingIfExitLatencyNeedsToBeDecreased = 4770,
    DsmStateEtwCheckingIfExitLatencyNeedsToBeIncreased = 4771,
    DsmStateEtwCheckingIfU1TimeoutNeedsToBeChanged   = 4772,
    DsmStateEtwCheckingIfU2TimeoutNeedsToBeChanged   = 4773,
    DsmStateEtwComputingU1U2TimeoutsAndExitLatency   = 4774,
    DsmStateEtwDisablingEndpointsOnConfigFailure     = 4859,
    DsmStateEtwDisablingEndpointsOnConfigFailureOnDetach = 4876,
    DsmStateEtwDisablingU1                           = 4775,
    DsmStateEtwDisablingU1U2OnExitLatencyTooLargeError = 4776,
    DsmStateEtwDisablingU2                           = 4777,
    DsmStateEtwEnablingU1                            = 4778,
    DsmStateEtwEnablingU2                            = 4779,
    DsmStateEtwInitializingU1U2Flags                 = 4780,
    DsmStateEtwProgrammingEndpointsInControllerAfterDecreasingExitLatency = 4850,
    DsmStateEtwProgrammingEndpointsInControllerAfterIncreasingExitLatency = 4851,
    DsmStateEtwQueuingSetU1TimeoutToPsm              = 4781,
    DsmStateEtwQueuingSetU2TimeoutToPsm              = 4782,
    DsmStateEtwReturningOperationFailureInUpdatingLPMValues = 4783,
    DsmStateEtwReturningOperationSuccessInUpdatingLPMValues = 4784,
    DsmStateEtwReturningPortDetachDeviceInUpdatingLPMValues = 4877,
    DsmStateEtwUpdatingDeviceExitLatencyInTheControllerAfterDecreasingExitLatency = 4852,
    DsmStateEtwUpdatingDeviceExitLatencyInTheControllerAfterIncreasingExitLatency = 4853,
    DsmStateEtwUpdatingDeviceStatusToU1Disabled      = 4786,
    DsmStateEtwUpdatingDeviceStatusToU1Enabled       = 4787,
    DsmStateEtwUpdatingDeviceStatusToU2Disabled      = 4788,
    DsmStateEtwUpdatingDeviceStatusToU2Enabled       = 4789,
    DsmStateEtwComputingLPMTimeoutValuesInUnconfigured = 4790,
    DsmStateEtwQueuingSetU2TimeoutToPsmForEnumeratedDevice = 4791,
    DsmStateEtwReturningOperationFailureInUpdatingLPMValuesInUnconfigured = 4792,
    DsmStateEtwReturningOperationSuccessInUpdatingLPMValuesInUnconfigured = 4793,
    DsmStateEtwGettingRemoteWakeCapability           = 4794,
    DsmStateEtwReturningOperationFailureInGettingRemoteWakeCap = 4795,
    DsmStateEtwReturningOperationSuccessInGettingRemoteWakeCap = 4796,
    DsmStateEtwValidatingInterfaceStatusForRemoteWake = 4797,
    DsmStateEtwCheckingIf20LPMShouldBeEnabled        = 4798,
    DsmStateEtwReturningOperationSuccessInSetting20LPMValues = 4799,
    DsmStateEtwWaitingFor20LpmUpdateIoctlToComplete  = 4800,
} DSM_STATE_ETW, *PDSM_STATE_ETW;



//
// Definitions for State Entry Functions 
//
STATE_ENTRY_FUNCTION          HUBDSM_AckingD0ForFaultyClientDrivers;

STATE_ENTRY_FUNCTION          HUBDSM_AckingHubStopAfterSuspendFromStoppedState;

STATE_ENTRY_FUNCTION          HUBDSM_AckingPdoPreStart;

STATE_ENTRY_FUNCTION          HUBDSM_AckingPdoPreStartFromStoppedEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_AckingQueryResourcesInConfiguredD0;

STATE_ENTRY_FUNCTION          HUBDSM_AckingStopAfterSuspendOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_AcquiringPowerReferenceOnHubOnAttach;

STATE_ENTRY_FUNCTION          HUBDSM_AcquiringPowerReferenceOnHubStart;

STATE_ENTRY_FUNCTION          HUBDSM_BugcheckingSystemOnBootDeviceEnumerationFailure;

STATE_ENTRY_FUNCTION          HUBDSM_CheckIfDeviceCanBeDisabledOnDriverNotFound;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingForZeroBwPipeOnClientClearStall;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingForZeroBwPipeOnClientResetPipe;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingForZeroBwPipeOnClientSyncResetPipe;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfBootDeviceOnD0Exit;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfBootDeviceOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfBootDeviceOnReset;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDeviceHasReceivedFirstStart;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDeviceHasReceivedFirstStartInUncoonfigured;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDeviceShouldBeDisabled;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfItIsIsochronousEndpoint;

STATE_ENTRY_FUNCTION          HUBDSM_CleaningUpAnyExistingConfigurationOnStart;

STATE_ENTRY_FUNCTION          HUBDSM_ClearingEndpointHaltOnClientClearStall;

STATE_ENTRY_FUNCTION          HUBDSM_ClearingEndpointHaltOnClientResetPipe;

STATE_ENTRY_FUNCTION          HUBDSM_ClearingFailIoFlagOnSuccessfulRecovery;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingClientRequestInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingClientRequestInConfiguredWithLastStatus;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingClientRequestInConfiguredWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingClientRequestInUnconfigured;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingClientRequestInUnConfiguredWithLastStatus;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingClientRequestOnDetachInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingClientRequestOnDetachInDisabled;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingClientRequestOnDetachWithDeviceProgrammed;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingClientRequestOnFailureInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingClientRequestOnFailureInDisabled;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingClientRequestOnFailureInUnconfigured;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingClientUnconfigureRequest;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingClientUnconfigureRequestWithFailure;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingD0EntryInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingD0EntryInConfiguredWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingD0EntryInConfiguredWithSpecialFileOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingD0EntryInConfiguredWithSpecialFileOnDetachWithDeviceEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingD0EntryInUnconfigured;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingD0EntryOnDetachWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingD0ExitAfterHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingD0ExitOnSuspendFailure;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingDeviceCleanup;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingHubStopAfterSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingHubStopWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingPDOCleanupForUnknownDevice;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingPdoExitFinalOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingPdoExitFinalOnDetachOnDeviceReportedMissing;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingPdoReStartOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingPdoReStartOnDetachWithDeviceEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingPdoReStartOnOperationFailure;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingPnpEventOnDetachInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingPnpEventOnDetachInDisabled;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingPnpEventOnDetachInUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingPnpEventOnDetachWhileRenumeratingOnResume;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingPnpEventOnFailureInDisabledWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingPnpEventOnFailureWithEndpointsToBeDeleted;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguredInD0;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguredResumingOnHubResume;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguredSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguredSuspendedOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguredSuspendedWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguredSuspending;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguredSuspendingOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguredSuspendingWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguringDeviceFromCachedInformation;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguringDeviceFromCachedInformationOnHubResume;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguringDeviceFromCachedInformationOnResume;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguringDeviceFromCachedInformationOnResumeWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguringDeviceFromCachedInformationWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguringOnSelectConfiguration;

STATE_ENTRY_FUNCTION          HUBDSM_CyclingPortOnRenumerationOfUnknownDevice;

STATE_ENTRY_FUNCTION          HUBDSM_D0EntryForUnknownDevice;

STATE_ENTRY_FUNCTION          HUBDSM_DeConfiguringDeviceOnClientRequest;

STATE_ENTRY_FUNCTION          HUBDSM_DeConfiguringDeviceOnClientRequestFromUnconfigured;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingDefaultEndpointAndDeviceOnDetachDuringEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanup;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanupAfterDetach;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanupWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingEndpoinstAndDeviceOnDetachOnCleanup;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingEndpointsAndDeviceOnOnCleanup;

STATE_ENTRY_FUNCTION          HUBDSM_DeporgrammingAllEndpointsOnCleanupFromEnumeratedConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_DeporgrammingAllEndpointsOnHubStopFromEnumeratedConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_DeprogrammingAllEndpointsOnHubSuspendFromEnumeratedConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_DetachedAndDeprogrammedWithResetPending;

STATE_ENTRY_FUNCTION          HUBDSM_DetachedWithResetPending;

STATE_ENTRY_FUNCTION          HUBDSM_DeviceDetachedAndDeporgrammedWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_DeviceDetachedWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingBackPortInEnumeratedUnknown;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingBackPortInWaitingForD0EntryForFailedDevice;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceAndDefaultEndpointInControllerOnDetachDuringEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceAndDefaultEndpointOnDetachOnHubStart;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceAndDefaultEndpointOnEnumFailureOnHubStart;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceAndDefaultEpInControllerOFailureWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceAndDefaultEpInControllerOnCleanupWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceAndDefaultEpInControllerOnD0ExitOnDetachWithPDOMissing;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetachOnHubStopWithPDOMissing;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetachOnPDOCleaupWithPDOMissing;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspendOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnCleanup;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnDetachOnCleanup;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnEnumFailureOnPdoPreStart;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnEnumFailureOnRestart;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnHubStopFromStoppedEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnHubStopInStopped;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnHubStopInSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnPDOStop;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnRenumerationWhileSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnStartTimeout;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsInControllerOHubStopAfterSuspendOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsInControllerOHubStopOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsInControllerOnCleanupWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsInControllerOnClientRequest;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsInControllerOnD0ExitOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsInControllerOnD0ExitOnDetachWithPDOMissing;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsInControllerOnDetachOnHubStopWithPDOMissing;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsInControllerOnDetachOnPDOCleanupWithPDOMissing;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsInControllerOnFailureWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsInControllerOnHubStopAfterSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsInControllerOnRenumerationWhileSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsInControllerOnStop;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsOnDetachWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsOnDetachWithSpecialFileAndResetPending;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsOnFailureWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingPortOnFailureInUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingPortOnHubStop;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingPortOnHubStopFromStoppedEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingPortOnPdoCleanup;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingPortOnPDOStop;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingPortOnStartTimeout;

STATE_ENTRY_FUNCTION          HUBDSM_EnumeratedAsFailedUnknown;

STATE_ENTRY_FUNCTION          HUBDSM_Enumerating;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingPD0D0ExitFinalInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingPD0D0ExitFinalInUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInConfiguredSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInConfiguredSuspendedWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInDeviceDetachedAndDeporgrammedWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInDeviceDetachedWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInStopEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInStopped;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInStoppedWithHubSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInUnconfiguredInD0;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInUnconfiguredSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInWaitingForDetachorResetWithDeviceDeprogrammed;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInWaitingForDetatchorReset;

STATE_ENTRY_FUNCTION          HUBDSM_GettingMatchingPipeHandleOnClientClearStall;

STATE_ENTRY_FUNCTION          HUBDSM_GettingMatchingPipeHandleOnClientResetPipe;

STATE_ENTRY_FUNCTION          HUBDSM_GettingMatchingPipeHandleOnClientSyncResetPipe;

STATE_ENTRY_FUNCTION          HUBDSM_IssuingDeviceDetachedToPSMOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_IssuingPortCycleAndCompletingClientRequestOnEnumFailue;

STATE_ENTRY_FUNCTION          HUBDSM_IssuingPortCycleAndSingalingPnpEventOnEnumFailueFromConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_IssuingPortCycleOnEnumFailureOnHubResume;

STATE_ENTRY_FUNCTION          HUBDSM_IssuingPortCycleOnFailueWithEndpointsToBeDeleted;

STATE_ENTRY_FUNCTION          HUBDSM_NotifyingDeviceDisconnectedOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_NotifyingDeviceDisconnectedOnDetachWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_PreparingEndpointAndInterfaceListsOnClientReset;

STATE_ENTRY_FUNCTION          HUBDSM_PreparingEndpointAndInterfaceListsOnClientResetWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterHubResume;

STATE_ENTRY_FUNCTION          HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterResume;

STATE_ENTRY_FUNCTION          HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterResumeWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_PreparingEndpointAndInterfaceListsOnSelectConfiguration;

STATE_ENTRY_FUNCTION          HUBDSM_PreparingEndpointAndInterfaceListsOnSelectConfigurationFromConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_PurgingDeviceIoOnDetachInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_PurgingDeviceIoOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_PurgingDeviceIoOnHubSuspendFromSuspending;

STATE_ENTRY_FUNCTION          HUBDSM_PurgingDeviceIoOnPrepareForHibernate;

STATE_ENTRY_FUNCTION          HUBDSM_QueryingDescriptorForFdoRequestFromConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_QueryingDescriptorForFdoRequestFromStoppedEnumerated;

STATE_ENTRY_FUNCTION          HUBDSM_QueryingDescriptorForFdoRequestFromUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_QueryingDeviceTextStringFromConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_QueryingDeviceTextStringFromStoppedEnumerated;

STATE_ENTRY_FUNCTION          HUBDSM_QueryingDeviceTextStringFromUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_QueueingAttachFailureToPort;

STATE_ENTRY_FUNCTION          HUBDSM_QueueingAttachSuccessToPort;

STATE_ENTRY_FUNCTION          HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceInUnconfigured;

STATE_ENTRY_FUNCTION          HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceWithResetPending;

STATE_ENTRY_FUNCTION          HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceWithResetPendingAndDeviceDeprogrammed;

STATE_ENTRY_FUNCTION          HUBDSM_QueueingPrepareForHiberToPSM;

STATE_ENTRY_FUNCTION          HUBDSM_ReDisablingPortAfterHubResume;

STATE_ENTRY_FUNCTION          HUBDSM_ReEnumeratingOnClientRequestInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_ReEnumeratingOnClientRequestInConfiguredWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_ReEnumeratingOnClientRequestInUnconfigured;

STATE_ENTRY_FUNCTION          HUBDSM_ReEnumeratingOnClientRequestOnReattach;

STATE_ENTRY_FUNCTION          HUBDSM_ReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed;

STATE_ENTRY_FUNCTION          HUBDSM_ReEnumeratingOnHubResumeInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_ReEnumeratingOnHubResumeInUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_ReEnumeratingOnHubStart;

STATE_ENTRY_FUNCTION          HUBDSM_ReEnumeratingOnPdoPreStartWhileEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_ReEnumeratingOnPdoStart;

STATE_ENTRY_FUNCTION          HUBDSM_ReEnumeratingOnPdoStartWhileEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_ReEnumeratingOnResumeInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_ReEnumeratingOnResumeInConfiguredWithSpecialFile;

STATE_ENTRY_FUNCTION          HUBDSM_ReEnumeratingOnResumeInUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_RegisteringWithHSM;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingDevicePowerReferenceOnStopAfterDetach;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnDetachInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnHubOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnHubOnDetachOnEnumSuccess;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnHubOnEnumerationFailure;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnHubStopSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnHubSuspendStopOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnPortFailureInHubStopSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnSuspendFailureInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnSuspendFailureInUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanup;

STATE_ENTRY_FUNCTION          HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanupOnDetachInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanupOnDetachInUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanUpOnHubStopOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_RemovingDeviceInfoFromGlobalListOnDeviceReportedMissing;

STATE_ENTRY_FUNCTION          HUBDSM_RemovingDeviceInfoFromGlobalListOnReportedMissingOnHubStopOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_ReportingDeviceMissing;

STATE_ENTRY_FUNCTION          HUBDSM_ReportingDeviceToPnp;

STATE_ENTRY_FUNCTION          HUBDSM_ReportingFailedDeviceAsMissing;

STATE_ENTRY_FUNCTION          HUBDSM_ReportingFailedUnknownDeviceToPnp;

STATE_ENTRY_FUNCTION          HUBDSM_RequestingCyclePortAndCompletingPnpEventOnFailureInUnconfigured;

STATE_ENTRY_FUNCTION          HUBDSM_RequestingPortCycleOnErrorFromConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_ResettingPipeInUCXOnClientReset;

STATE_ENTRY_FUNCTION          HUBDSM_ResettingPipeInUCXOnClientSyncResetPipe;

STATE_ENTRY_FUNCTION          HUBDSM_ResumingOnHubResumeInStopped;

STATE_ENTRY_FUNCTION          HUBDSM_SettingConfigurationInformationInClientRequest;

STATE_ENTRY_FUNCTION          HUBDSM_SettingDeviceInterface;

STATE_ENTRY_FUNCTION          HUBDSM_SettingExitLatencyAdjustmentOnNoPingResponseError;

STATE_ENTRY_FUNCTION          HUBDSM_SettingFailIoFlagOnClientReset;

STATE_ENTRY_FUNCTION          HUBDSM_SettingForceResetOnRenumFlag;

STATE_ENTRY_FUNCTION          HUBDSM_SettingInterfaceInformationInClientRequest;

STATE_ENTRY_FUNCTION          HUBDSM_SettingLPMValuesInConfiguredOnSettingChange;

STATE_ENTRY_FUNCTION          HUBDSM_SettingLPMValuesInStoppedEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_SettingLPMValuesInUnConfiguredOnSettingChange;

STATE_ENTRY_FUNCTION          HUBDSM_SignalingPnpPowerEventOnDetachDuringRenumOnRestart;

STATE_ENTRY_FUNCTION          HUBDSM_SignalingPnpPowerEventOnStop;

STATE_ENTRY_FUNCTION          HUBDSM_SignalingPnpPowerEventOnStopEnable;

STATE_ENTRY_FUNCTION          HUBDSM_SignallingPnpEventOnHubSuspendFromUnconfiguredSuspending;

STATE_ENTRY_FUNCTION          HUBDSM_StartingDeviceIoOnHubResume;

STATE_ENTRY_FUNCTION          HUBDSM_Stopped;

STATE_ENTRY_FUNCTION          HUBDSM_StoppedEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_StoppedEnumerated;

STATE_ENTRY_FUNCTION          HUBDSM_StoppedEnumeratedConfiguredForFaultyClients;

STATE_ENTRY_FUNCTION          HUBDSM_StoppedEnumeratedWithHubSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_StoppedWithHubSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_SuspendingOnHubSuspendInStopped;

STATE_ENTRY_FUNCTION          HUBDSM_UnConfiguredInD0;

STATE_ENTRY_FUNCTION          HUBDSM_UnConfiguredResumingOnHubResume;

STATE_ENTRY_FUNCTION          HUBDSM_UnconfiguredSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_UnConfiguredSuspendedOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_UnconfiguredSuspending;

STATE_ENTRY_FUNCTION          HUBDSM_UnConfiguredSuspendingOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_UnregisteringWithHSMOnDetachAfterCleanup;

STATE_ENTRY_FUNCTION          HUBDSM_UnregisteringWithHsmOnDetachOnDeviceReportedMissing;

STATE_ENTRY_FUNCTION          HUBDSM_UnregsiteringWithHsmOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_UnregsiteringWithHSMOnDetachOnPDOCleaupWithPDOMissing;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingSelectConfigUrbFromConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingSelectConfigUrbFromUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingSelectInterfaceUrbFromConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForD0EntryForFailedDevice;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDetachOrRemove;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDetachOrRemoveInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDetachOrRemoveInDisabledWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDetachOrRemoveInUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDetachOrRemoveWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDetachOrReset;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDetachOrResetWithDeviceDeporgrammed;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDetachOrToBeDeleted;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDetachOrToBeDeletedWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDisableCompleteOnHubStopFromStoppedEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPDOD0ExitOnDetachInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPDOD0ExitOnDetachInUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPDOReportedMissing;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPortAttach;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPortResumeResponseOnStop;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPortResumeResponseOnSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForRemoveOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForRemoveOrReprotedMissingOnHubStopOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForReportedMissingOnDetachInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForReportedMissingOnDetachInUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingToBeDeleted;

STATE_ENTRY_FUNCTION          HUBDSM_QueryingLanguageSpecificProductIdString;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInQueryingProductId;

STATE_ENTRY_FUNCTION          HUBDSM_SignalingDeviceQueryTextEvent;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingLanguageSpecificProductIdString;

STATE_ENTRY_FUNCTION          HUBDSM_AcquiringAddressZeroOwnershipInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_AcquiringPowerReferenceOnStartInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_CancellingAcquiringAddress0OnDetachInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_CancellingRetryTimerOnDetachInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_CancellingRetryTimerOnStopSuspendInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDevicePDChargingPolicySupported;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfEnumRetryReachedMaximumInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfEnumRetryReachedMaximumInEnumWithAddr0Ownership;

STATE_ENTRY_FUNCTION          HUBDSM_ClearingDeviceProgrammingLostFlagOnEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingDefaultEndpointAndDeviceFromUCXOnDetachInEnumeratingAfterAddr;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingDefaultEndpointAndDeviceFromUCXOnFailureInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingDefaultEndpointAndDeviceOnPostAddressFailureInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingDefaultEndpointAndDeviceOnStopSuspendAfterAddressInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingDeviceAndDefaultEndpointOnDetachWithAddress0OwnershipInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingDeviceAndDefaultEndpointOnStopSuspendWithAddress0OwnershipInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnDetachInEnumeratingAfterAddr;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnDetachWithAddress0OwnershipInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnPostAddressEnumFailureInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnSetAddressFailureInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnStopSuspendAfterAddressInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnStopSuspendWithAddress0OwnershipInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingOnEnumAfterFailureInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingOnEnumAfterFailureInEnumWithAddress0Ownership;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingOnStopSuspendInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingOnStopSuspendInEnumWithAddress0Ownership;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingOnStopSuspendOnIoctlFailureInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_EnumeratingAfterAddressInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_EnumeratingAtAddressZeroInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_InitializingEnumRetryCountInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingAddressZeroOwnershipInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingAddressZeroOwnershipOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingAddressZeroOwnershipOnDetachInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingAddressZeroOwnershipOnEnumFailure;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDetachDeviceInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_SettingDevicePDChargingPolicy;

STATE_ENTRY_FUNCTION          HUBDSM_SettingLPMValues;

STATE_ENTRY_FUNCTION          HUBDSM_SettingNonZeroAddressInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_StartingTimerForEnumRetryInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_StartingTimerForEnumRetryInEnumWithAddr0Ownership;

STATE_ENTRY_FUNCTION          HUBDSM_StoppedSuspendedInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_UpdatingDeviceInformationInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDisableCompleteOnStopSuspendAfterFailureInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDisableCompleteOnStopSuspendInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDisableCompleteOnStopSuspendInEnumWithAddress0Ownership;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerToFlushOnDetachInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerToFlushOnStopSuspendInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForUCXIoctlOnDetachInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForUCXIoctlOnStopSuspendInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfFirstEnumTryAfterReset1;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfFirstEnumTryAfterReset2;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfSecondResetIsNeeded;

STATE_ENTRY_FUNCTION          HUBDSM_CreatingUCXDefaultEndpoint;

STATE_ENTRY_FUNCTION          HUBDSM_CreatingUCXDevice;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnStop;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingUCXDefaultEndpointOnOperationFailure;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingUCXDeviceOnOperationFailure;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInUCXOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInUCXOnOperationFailure;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInUCXOnStop;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInUCXOnSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingPortOnHubStopInEnumAtAddr0;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingPortOnHubSuspendInEnumAtAddr0;

STATE_ENTRY_FUNCTION          HUBDSM_EnablingDeviceInUCX;

STATE_ENTRY_FUNCTION          HUBDSM_GettingDeviceDescriptorInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_LoggingReset1TimeoutInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_LoggingReset2TimeoutInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_NotifyingDeviceResetToUCXAfterReset2;

STATE_ENTRY_FUNCTION          HUBDSM_Resetting1;

STATE_ENTRY_FUNCTION          HUBDSM_Resetting2;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningDeviceOperationFailureInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningDeviceOperationSuccessInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubStopInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubSuspendInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDetachDeviceInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_SettingSpeedFlagFor20Devices;

STATE_ENTRY_FUNCTION          HUBDSM_StoppingTimerOnHubStopInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_StoppingTimerOnHubSuspendInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_StoppingTimerOnPortDetachInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_UpdatingDefaultEndpointMaxPacketSizeInEnum;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingDeviceDescriptorInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPortReset1OrDisableOnHubStopInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPortReset1OrDisableOnHubSuspendInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPortReset2OnHubStopInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPortReset2OnHubSuspendInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPostReset1ExtendedTimer;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPostReset1Timer;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPostReset2ExtendedTimer;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPostReset2Timer;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForStopSuspendOnReset1InEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForStopSuspendOnReset2InEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerOnHubStopInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerOnHubSuspendInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerOnPortDetachInEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_CreatingUCXDefaultEndpointFor30;

STATE_ENTRY_FUNCTION          HUBDSM_CreatingUCXDevice30;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnHubStop;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingUCXDeviceOnOperationFailureFor30;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnHubStop;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_EnablingDeviceInUCXFor30;

STATE_ENTRY_FUNCTION          HUBDSM_ResettingDeviceInEnumeration;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningDetachInPreAddressEnumFor30;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningDeviceOperationFailureInPreAddressEnumFor30;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningDeviceOperationSuccessInPreAddressEnumFor30;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubStopInPreAddressEnumFor30;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubSuspendInPreAddressEnumFor30;

STATE_ENTRY_FUNCTION          HUBDSM_Setting30Speed;

STATE_ENTRY_FUNCTION          HUBDSM_CancellingControlTransferOnDetachInEnumAfterAddr;

STATE_ENTRY_FUNCTION          HUBDSM_CancellingControlTransferOnHubStopInEnumAfterAddr;

STATE_ENTRY_FUNCTION          HUBDSM_CancellingControlTransferOnHubSuspendInEnumAfterAddr;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingBytesReturnedInConfigDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfAltEnumCmdCached;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfSuperSpeedNeedsToBeDisabled;

STATE_ENTRY_FUNCTION          HUBDSM_DiasablingSuperSpeed;

STATE_ENTRY_FUNCTION          HUBDSM_GettingConfigurationDescriptorWithDefaultLength;

STATE_ENTRY_FUNCTION          HUBDSM_GettingConfigurationDescriptorWithReturnedLength;

STATE_ENTRY_FUNCTION          HUBDSM_GettingDeviceDescriptorAfterAddressing;

STATE_ENTRY_FUNCTION          HUBDSM_GettingRemainingDescriptorsAfterConfigDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_QueryingRegistryValuesForDevice;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubStopInEnumAfterAddr;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubSuspendInEnumAfterAddr;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureInEnumAfterAddr;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInEnumAfterAddr;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDeviceDetachInEnumAfterAddr;

STATE_ENTRY_FUNCTION          HUBDSM_SendingAltEnumCmdInEnumAfterAddr;

STATE_ENTRY_FUNCTION          HUBDSM_SettingVersionFlagInEnumerating;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingConfigurationDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingDeviceDescriptorAfterAddressing;

STATE_ENTRY_FUNCTION          HUBDSM_CancellingControlTransferOnDetachInRemainingDescriptors;

STATE_ENTRY_FUNCTION          HUBDSM_CancellingControlTransferOnHubStopInRemainingDescriptors;

STATE_ENTRY_FUNCTION          HUBDSM_CancellingControlTransferOnHubSuspendInRemainingDescriptors;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfIProductIdStringDescriptorShouldBeQueried;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfIProductIsZero;

STATE_ENTRY_FUNCTION          HUBDSM_GettingBosDescriptorSet;

STATE_ENTRY_FUNCTION          HUBDSM_GettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch;

STATE_ENTRY_FUNCTION          HUBDSM_GettingLanguageIdStringDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_GettingMSOSAndSerialNumberDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_GettingProductIdStringDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningErrorResponseOnLanguageIdQuery;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningErrorResponseOnProductStringQuery;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubStopInRemainingDescriptors;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubSuspendInRemainingDescriptors;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureInRemainingDescriptors;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInRemainingDescriptors;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDeviceDetachInRemainingDescriptors;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingLanguageIdStringDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingProductIdStringDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDeviceSupportsContainerId;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfIgnoreHWSerNumIsSet;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfMSOSContainerIdDescriptorIsSupported;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfMSOSDescriptorShouldBeQueried;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfMSOSExtendedConfigDescriptorIsSupported;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfSerialNumberStringIndexIsZero;

STATE_ENTRY_FUNCTION          HUBDSM_GettingMSOSContainerIdDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_GettingMSOSContainerIdHeaderDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_GettingMSOSDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_GettingMSOSExtendedDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_GettingMSOSExtendedDescriptorHeader;

STATE_ENTRY_FUNCTION          HUBDSM_GettingSerialNumberStringDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_MarkDeviceAsNotSupportingMSOSDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_MarkDeviceAsSupportingMSOSDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_MarkingDeviceAsNotSupportingContainerId;

STATE_ENTRY_FUNCTION          HUBDSM_MarkingDeviceAsNotSupportingContainerIdBasedOnMSOSDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningErrorResponseOnContainerIdHeaderQuery;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningErrorResponseOnContainerIdQuery;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningErrorResponseOnMSOSExtendedHeaderQuery;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningErrorResponseOnMSOSExtendedQuery;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningErrorResponseOnMSOSQuery;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningErrorResponseOnSerialNumberQuery;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureInGettingDescriptorsForGreaterThan1x;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInGettingDescriptorsForGreaterThan1x;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingMSOSContainerIdDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingMSOSContainerIdHeaderDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingMSOSDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingMSOSExtendedConfigDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingMSOSExtendedConfigDescriptorHeader;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingSerialNumberStringDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfIgnoreHWSerNumIsSetFor1xDevice;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfSerialNumberStringIndexIsZeroFor1xDevice;

STATE_ENTRY_FUNCTION          HUBDSM_GettingSerialNumberStringDescriptorFor1xDevice;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningErrorResponseOnSerialNumberQueryFor1xDevice;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureInGettingSerialNumberFor1x;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInGettingSerialNumberFor1x;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingSerialNumberStringDescriptorFor1xDevice;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfWaitRequiredAfterSetAddress;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubStopInSettingAddress;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubSuspendInSettingAddress;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureInSettingAddress;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInSettingAddress;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDetachDeviceInSettingAddress;

STATE_ENTRY_FUNCTION          HUBDSM_SettingNonZeroAddress;

STATE_ENTRY_FUNCTION          HUBDSM_StoppingTimerOnHubStopInSettingAddress;

STATE_ENTRY_FUNCTION          HUBDSM_StoppingTimerOnHubSuspendInSettingAddress;

STATE_ENTRY_FUNCTION          HUBDSM_StoppingTimerOnPortDetachInSettingAddress;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerAfterSetAddress;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerOnHubStopInSettingAddress;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerOnHubSuspendInSettingAddress;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerOnPortDetachInSettingAddress;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingBytesReturnedInAlternateConfigDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfAltEnumCommandNeeded;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfAlternateBOSDescriptorQueryShouldBeSkipped;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfAlternateModeStringDescriptorShouldBeQueried;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfBillboardBOSDescriptorIsPresent;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfBillboardStringDescriptorShouldBeQueried;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfBOSDescriptorQueryShouldBeSkipped;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfCompleteAlternateBOSDescriptorWasRetrieved;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfCompleteBOSDescriptorWasRetrieved;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDualRoleFeaturesSupported;

STATE_ENTRY_FUNCTION          HUBDSM_GetMsOs20DescriptorSet;

STATE_ENTRY_FUNCTION          HUBDSM_GettingAlternateBOSDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_GettingAlternateBOSDescriptorHeader;

STATE_ENTRY_FUNCTION          HUBDSM_GettingAlternateConfigurationDescriptorWithDefaultLength;

STATE_ENTRY_FUNCTION          HUBDSM_GettingAlternateConfigurationDescriptorWithReturnedLength;

STATE_ENTRY_FUNCTION          HUBDSM_GettingAlternateDeviceDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_GettingAlternateModeStringDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_GettingBillboardStringDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_GettingBOSDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_GettingBOSDescriptorHeader;

STATE_ENTRY_FUNCTION          HUBDSM_IsMsOs20DescriptorSupported;

STATE_ENTRY_FUNCTION          HUBDSM_QueryingRegistryValuesForAlternateDeviceEnumeration;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningErrorResponseOnBOSDescriptorHeaderQuery;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningErrorResponseOnBOSDescriptorQuery;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureInGettingBOSDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInGettingBOSDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_SendingMsOs20AlternateEnumerationCommand;

STATE_ENTRY_FUNCTION          HUBDSM_SendingUsbFeaturesVendorCmd;

STATE_ENTRY_FUNCTION          HUBDSM_SettingVersionFlagInAlternateEnumeration;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingAlternateBOSDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingAlternateBOSDescriptorHeader;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingAlternateConfigurationDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingAlternateDeviceDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingAndLoggingAlternateModeStringDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingAndLoggingBillboardStringDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingBOSDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingBOSDescriptorHeader;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingMsOs20DescriptorSetIfPresent;

STATE_ENTRY_FUNCTION          HUBDSM_GettingDeviceQualifierDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningErrorResponseOnDeviceQualifierQuery;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInCheckingFor20FullSpeed;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingDeviceQualifierDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_AddingDeviceToGlobalChildList;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfAddDeviceToGlobalListRetryCountExceededLimit;

STATE_ENTRY_FUNCTION          HUBDSM_CreatingChildPDOAndReportingToPnp;

STATE_ENTRY_FUNCTION          HUBDSM_DiscardSerialNumber;

STATE_ENTRY_FUNCTION          HUBDSM_InitializingAddDeviceToGlobalListRetryCount;

STATE_ENTRY_FUNCTION          HUBDSM_RequestingDeviceCycleInReportingToPnp;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInReportingToPnp;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDetachDevice;

STATE_ENTRY_FUNCTION          HUBDSM_StoppingTimerAndRequestingCycleOnStopSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_StoppingTimerOnDetachInReportingToPnp;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDetachInReportingToPnp;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDetachOrTimerInReportingToPnp;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDuplicateDeviceToGoAway;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerToFlushOnDetachInReportingToPnp;

STATE_ENTRY_FUNCTION          HUBDSM_CreatingUnknownChildPDOAndReportingToPnp;

STATE_ENTRY_FUNCTION          HUBDSM_MarkingUnknownDeviceAsFailed;

STATE_ENTRY_FUNCTION          HUBDSM_RequestingDeviceCycleForUnknownDevice;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInReportingUnknownDevice;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDetachDeviceInReportingUnknownDevice;

STATE_ENTRY_FUNCTION          HUBDSM_AckingResumed;

STATE_ENTRY_FUNCTION          HUBDSM_AcquiringPowerReferenceInEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_AcquiringPowerReferenceInSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_AcquiringPowerReferenceInSuspendedOnResumeWithReset;

STATE_ENTRY_FUNCTION          HUBDSM_AcquiringPowerReferenceOnHubResume;

STATE_ENTRY_FUNCTION          HUBDSM_AcquiringPowerReferenceOnHubResumeWithReenumRequired;

STATE_ENTRY_FUNCTION          HUBDSM_AcquiringPowerReferenceOnHubResumeWtihReset;

STATE_ENTRY_FUNCTION          HUBDSM_AcquiringPowerReferenceOnResumeInS0;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDeviceNeedsResetOnResumeInSxOnEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDeviceNeedsResetOnResumeInSxOnSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingUnexpectedD0ExitInHubSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_DisabledOrFailedInHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingPortOnPortEnabledInPendingHubStop;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingReferenceOnHubSuspendAfterPSMSyncUp;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingReferenceOnHubSuspendOnDetachAfterPSMSyncUp;

STATE_ENTRY_FUNCTION          HUBDSM_ResumingBeforeResetting;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningDeviceRenumerationOnHubResume;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningDeviceResumedOnHubResume;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningDeviceSuspendedOnHubResume;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubStopWithReferenceAcquiredInHubSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDetach;

STATE_ENTRY_FUNCTION          HUBDSM_SuspendedInHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_SuspendingPortOnPortEnabledInPendingHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForHubResumeInEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForHubResumeInSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForHubResumeWithRenumRequired;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPSMSyncUp;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPSMSyncUpOnPendingStop;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPSMSyncUpOnPendingSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPSMSyncUpOnResumeWithReset;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForResumeResponseFromPortOnHubStop;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForResumeResponseFromPortOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_AcquiringAddressZeroOwnershipOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_AcquiringPowerReferenceOnStartInReEnum;

STATE_ENTRY_FUNCTION          HUBDSM_CancellingAcquiringAddress0OnDetachOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_CancellingRetryTimerOnDetachOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_CancellingRetryTimerOnStopSuspendOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfAltEnumCmdNeededInReenum;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDevicePDChargingPolicySupportedInReEnum;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDevicePorgrammingWasLosttInUnconfigured;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDeviceProgrammingWasLosttInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfEnumRetryReachedMaximumOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfEnumRetryReachedMaximumOnRenumWithAddress0Ownership;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfEnumRetryReachedMaximumWithDeviceDisabledOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfItIsBootDeviceOnIdComparisionFailure;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfRequestConfigDescOnResetIsSet;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfSerialNumberShouldBeCompared;

STATE_ENTRY_FUNCTION          HUBDSM_ClearingDeviceLostProgammingFlagOnRestart;

STATE_ENTRY_FUNCTION          HUBDSM_ClearingDeviceProgrammingLostFlagOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_ComparingDeviceOnReEnumeration;

STATE_ENTRY_FUNCTION          HUBDSM_ComparingSerialNumberOnReEnumeration;

STATE_ENTRY_FUNCTION          HUBDSM_DeconfiguringEndpointsInControllerBeforeRenumerating;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnStopSuspendOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingOnEnumAfterFailureInReEnum;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingOnEnumAfterFailureInReEnumWithAddress0Ownership;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingOnEnumAfterFailureWithDeviceDisabledInReEnum;

STATE_ENTRY_FUNCTION          HUBDSM_EnablingDeviceInControllerInRenum;

STATE_ENTRY_FUNCTION          HUBDSM_EnumeratingAtAddressZeroOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_GettingConfigDescriptorWhileRenuemrating;

STATE_ENTRY_FUNCTION          HUBDSM_GettingDeviceDescriptorOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_GettingSerialNumberStringDescriptorWhileRenuemrating;

STATE_ENTRY_FUNCTION          HUBDSM_InitializingEnumRetryCountInReEnumDuringResetInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_InitializingEnumRetryCountInReEnumDuringResetInUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_InitializingEnumRetryCountInReEnumDuringRestart;

STATE_ENTRY_FUNCTION          HUBDSM_PurgingDeviceTreeIoOnReEnumerationInConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_PurgingDeviceTreeIoOnReEnumerationInUnConfigured;

STATE_ENTRY_FUNCTION          HUBDSM_PurgingIoOnEnumAfterFailureInReEnum;

STATE_ENTRY_FUNCTION          HUBDSM_PurgingIoOnEnumAfterFailureInReEnumWithAddress0Ownership;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingAddressZeroOwnershipOnDetachOnEnumFailure;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingAddressZeroOwnershipOnDetachWithDeviceEnabledOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingAddressZeroOwnershipOnDetachWithTimer;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingAddressZeroOwnershipOnEnumFailureInRenum;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingAddressZeroOwnershipOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendOnEnumFailure;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendWithTimer;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureWithDeviceEnabledOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDetachDeviceOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDetachDeviceWithDeviceEnabledOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_SendingMsOs20AltEnumCmdOnReenum;

STATE_ENTRY_FUNCTION          HUBDSM_SettingDevicePDChargingPolicyInReEnum;

STATE_ENTRY_FUNCTION          HUBDSM_SettingLPMValuesInReEnum;

STATE_ENTRY_FUNCTION          HUBDSM_SettingNonZeroAddressOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_StartingTimerForEnumRetryOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_StartingTimerForEnumRetryOnRenumWithAddress0Ownership;

STATE_ENTRY_FUNCTION          HUBDSM_StoppedSuspendedOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDetachAfterWrongDeviceDetectedOnBootDevicePort;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDisableCompleteOnStopSuspendAfterFailureInReEnum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDisableCompleteOnStopSuspendOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDisableCompleteOnStopSuspendWithDeviceDisabledAfterFailureInReEnum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerToFlushOnDetachOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerToFlushOnStopSuspendOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForUCXIoctlOnDetachOnRenum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForUCXIoctlOnStopSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfAnyAlternateInterfaceLeft;

STATE_ENTRY_FUNCTION          HUBDSM_ConfiguringDevice;

STATE_ENTRY_FUNCTION          HUBDSM_CreatingNewEndpoints;

STATE_ENTRY_FUNCTION          HUBDSM_DeconfiguringEndpointsInControllerForDeviceConfiguration;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingEndpointsForOldConfiguration;

STATE_ENTRY_FUNCTION          HUBDSM_MarkingAnyEndpointsToBeEnableAsDisabled;

STATE_ENTRY_FUNCTION          HUBDSM_ProgrammingEndpointsAndSettingLPMValuesrForDeviceConfiguration;

STATE_ENTRY_FUNCTION          HUBDSM_ProgrammingEndpointsInControllerForDeviceConfigurationFor20Devices;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureInReConfiguring;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInReConfiguring;

STATE_ENTRY_FUNCTION          HUBDSM_SettingDeviceInterfaceInConfiguring;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingPortOnHubSuspendInCleanup;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingHubPowerDownEvents;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingHubPowerUpEvents;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingHubStopAfterSuspendEvent;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingPnpEvents;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEvents;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnHubSuspendInCleanup;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDetachDeviceFromIgnoringDevicePort;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDisabledOnHubSuspendInCleanup;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDevicePortEvents;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingHubPowerDownEventsWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingHubPowerUpEventsWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingHubStopAfterSuspendEventWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingPnpEventsWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEventsWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDevicePortEventsWithPortOff;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingPortOnHubSuspendForFailedDevice;

STATE_ENTRY_FUNCTION          HUBDSM_FailedDeviceHubSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingHubPowerDownEventsForFailedDevice;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingHubPowerUpEventsForFailedDevice;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingHubStopAfterSuspendEventForFailedDevice;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingPnpEventsForFailedDevice;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnDetachForFailedDevice;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDetachDeviceForFailedDevice;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDevicePortEventsForFailedDevice;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingHubStartResumeEvent;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingHubSuspendEvent;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingPnpEventExceptStop;

STATE_ENTRY_FUNCTION          HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEventsExceptStop;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDevicePortEventsExceptStop;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingHubResumeEventForBootDevice;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingHubSuspendEventForBootDevice;

STATE_ENTRY_FUNCTION          HUBDSM_FlushingPnpEventsForBootDevice;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForDeviceHubEventsForBootDevice;

STATE_ENTRY_FUNCTION          HUBDSM_AckingPortEventInD3Cold;

STATE_ENTRY_FUNCTION          HUBDSM_AckingPortResumed;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDeviceArmedForWakeOnResume;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDeviceNeedsResetOnResumeInS0;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDeviceShouldBeDisarmedForWakeOnResume;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfResetOnLastResumeFlagIsSet;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfResetOnLastResumeFlagIsSetOnSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingD0EntryOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingD0EntryOnPortFailure;

STATE_ENTRY_FUNCTION          HUBDSM_DisabledAfterD3Cold;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingOnHubSuspendNeedingRenumeration;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingPortOnStopFromSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_DisarmingDeviceForWakeOnD0Entry;

STATE_ENTRY_FUNCTION          HUBDSM_DisarmingDeviceForWakeOnPortResumed;

STATE_ENTRY_FUNCTION          HUBDSM_PurgingDeviceIoOnHubSuspendWhileWaitingForD0Entry;

STATE_ENTRY_FUNCTION          HUBDSM_PurgingDeviceIoOnHubSuspendWhileWaitingForD0EntryOnHwWake;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnDetachWhileSuspending;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnDetachWhileWaitingForHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_ReleasingPowerReferenceOnPortFailureWhileWaitingForHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_ResumingFromSelectiveSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningDetachDevice;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningDeviceResumed;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubStopFromSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortFailureOnResume;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningRenumerateDeviceOnResume;

STATE_ENTRY_FUNCTION          HUBDSM_SettingResetOnLastResumeDueToD3Cold;

STATE_ENTRY_FUNCTION          HUBDSM_SettingResetOnLastResumeFlagForPDO;

STATE_ENTRY_FUNCTION          HUBDSM_SettingResetOnLastResumeFlagForPDOAfterD3Cold;

STATE_ENTRY_FUNCTION          HUBDSM_StartingDeviceIoOnDeviceResumeOnHwWake;

STATE_ENTRY_FUNCTION          HUBDSM_StartingDeviceIoOnDeviceResumeOnSwWake;

STATE_ENTRY_FUNCTION          HUBDSM_Suspended;

STATE_ENTRY_FUNCTION          HUBDSM_SuspendedWithHubSuspended;

STATE_ENTRY_FUNCTION          HUBDSM_SuspendedWithHubSuspendedInD3Cold;

STATE_ENTRY_FUNCTION          HUBDSM_SuspendingBackUnarmedDevice;

STATE_ENTRY_FUNCTION          HUBDSM_SuspendingWakeArmedDeviceOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForD0EntryOnHwWake;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForD0EntryOnPendingRenumerate;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForSuspendCompleteOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_AbortingDeviceIoInSuspendingBeforeArmingForWakeFailure;

STATE_ENTRY_FUNCTION          HUBDSM_ArmingDeviceForWake;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDeviceArmedForWakeOnSuspending;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingWaitWakeOnDetachDuringSuspending;

STATE_ENTRY_FUNCTION          HUBDSM_CompletingWaitWakeOnSuspendFailure;

STATE_ENTRY_FUNCTION          HUBDSM_PurgingDeviceIoInSuspending;

STATE_ENTRY_FUNCTION          HUBDSM_PurgingDeviceIoInSuspendingAfterArmingForWake;

STATE_ENTRY_FUNCTION          HUBDSM_PurgingDeviceIoInSuspendingAfterArmingForWakeFailure;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureInSuspending;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInSuspending;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDetachDeviceInSuspending;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningUnexpectedHubSuspendInSuspending;

STATE_ENTRY_FUNCTION          HUBDSM_Suspending;

STATE_ENTRY_FUNCTION          HUBDSM_SuspendingPortOnFailureInSuspending;

STATE_ENTRY_FUNCTION          HUBDSM_SuspendingWithArmedForWake;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPortResponseOnHubSuspend;

STATE_ENTRY_FUNCTION          HUBDSM_CreatingNewEndpointsInSettingInterface;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingEndpointsForNewInterfaceOnFailure;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingEndpointsForOldInterface;

STATE_ENTRY_FUNCTION          HUBDSM_DeletingEndpointsForOldInterfaceOnFailure;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsForTheCurrentInterface;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsForTheNewInterfaceOnFailure;

STATE_ENTRY_FUNCTION          HUBDSM_PreparingEndpointAndInterfaceListsOnSelectInterface;

STATE_ENTRY_FUNCTION          HUBDSM_ProgrammingEndpointsAndSettingLPMValuesInSettingInterface;

STATE_ENTRY_FUNCTION          HUBDSM_ProgrammingEndpointsInControllerInSettingInterfaceFor20Devices;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureInSettingInterface;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInSettingInterface;

STATE_ENTRY_FUNCTION          HUBDSM_SettingDeviceInterfaceInSettingInterface;

STATE_ENTRY_FUNCTION          HUBDSM_SettingDeviceInterfaceInSettingInterfaceOnFailureForCompat;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfDeviceSpeedChanged;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfFirstEnumTryInRenum;

STATE_ENTRY_FUNCTION          HUBDSM_GettingDeviceDescriptorInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_LoggingReset1TimeoutInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_LoggingReset2TimeoutInReEnum;

STATE_ENTRY_FUNCTION          HUBDSM_NotifyingDeviceResetToUCXAfterReset1InRenum;

STATE_ENTRY_FUNCTION          HUBDSM_NotifyingDeviceResetToUCXAfterReset2InRenum;

STATE_ENTRY_FUNCTION          HUBDSM_Resetting1InRenum;

STATE_ENTRY_FUNCTION          HUBDSM_Resetting2InRenum;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningDeviceOperationFailureInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningDeviceOperationSuccessInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubStopInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubSuspendInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDetachDeviceInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_StoppingTimerOnHubStopInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_StoppingTimerOnHubSuspendInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_StoppingTimerOnPortDetachInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingDeviceDescriptorInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPortResetOnHubStopInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPortResetOnHubSuspendInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPostReset1TimerInRenum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPostReset2ExtendedTimerInRenum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPostReset2TimerInRenum;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForStopSuspendOnReset1InReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerOnHubStopInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerOnHubSuspendInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerOnPortDetachInReEnumAtZero;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfFirstReEnumTryFor30Device;

STATE_ENTRY_FUNCTION          HUBDSM_DeterminingPortResetTypeRequired;

STATE_ENTRY_FUNCTION          HUBDSM_LoggingResetTimeoutInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_NotifyingDeviceResetToUCXInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_QueueingHotPortResetFor30;

STATE_ENTRY_FUNCTION          HUBDSM_QueueingWarmPortResetFor30;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningDeviceOperationFailureInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningDeviceOperationSuccessInInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubStopInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningHubSuspendInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDetachDeviceInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_StartingPostResetTimerFor30;

STATE_ENTRY_FUNCTION          HUBDSM_StoppingTimerOnHubStopInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_StoppingTimerOnHubSuspendInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_StoppingTimerOnPortDetachInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPortResetCompleteFor30;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPortResetOnHubStopInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForPortResetOnHubSuspendInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForStopSuspendOnReset1InResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerOnHubStopInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerOnHubSuspendInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForTimerOnPortDetachInResettingFor30;

STATE_ENTRY_FUNCTION          HUBDSM_AllocatingBufferForMsOsExtendedPropertyDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfMsOs20RegistryValuesShouldBeInstalled;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfMSOSExtendedPropertyDescriptorShouldBeQueried;

STATE_ENTRY_FUNCTION          HUBDSM_FreeingBufferAllocatedForMSOSExtendedProperty;

STATE_ENTRY_FUNCTION          HUBDSM_GettingMSOSExtendedPropertyDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_GettingMSOSExtendedPropertyDescriptorHeader;

STATE_ENTRY_FUNCTION          HUBDSM_InstallingMsOs20RegistryValues;

STATE_ENTRY_FUNCTION          HUBDSM_SettingExtPropDescSemaphoreForMsOs2;

STATE_ENTRY_FUNCTION          HUBDSM_SettingExtPropDescSemaphoreRegistryFlag;

STATE_ENTRY_FUNCTION          HUBDSM_SignalingPnpEventForMSOSExtInstall;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingMSOSExtendedPropertyDescriptor;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingMSOSExtendedPropertyDescriptorHeader;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingForInstallMSOSExt;

STATE_ENTRY_FUNCTION          HUBDSM_WritingCustomPropertiesInRegistry;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfLTMShouldBeEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfSetIsochDelayShouldBeSkipped;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfSetSelShouldBeSkipped;

STATE_ENTRY_FUNCTION          HUBDSM_EnablingLTM;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureInSettingLPMValues;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInSettingLPMValues;

STATE_ENTRY_FUNCTION          HUBDSM_SettingSEL;

STATE_ENTRY_FUNCTION          HUBDSM_SetttingIsochDelay;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfEnableU1NeedsToBeUpdated;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfEnableU2NeedsToBeUpdated;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfEndpointsNeedToBeConfiguredAfterDecreasingExitLatency;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfEndpointsNeedToBeProgrammedAfterIncreasingLatency;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfEndpointsToBeDisabled;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfEndpointsToBeDisabledOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfExitLatencyNeedsToBeDecreased;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfExitLatencyNeedsToBeIncreased;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfU1TimeoutNeedsToBeChanged;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIfU2TimeoutNeedsToBeChanged;

STATE_ENTRY_FUNCTION          HUBDSM_ComputingU1U2TimeoutsAndExitLatency;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsOnConfigFailure;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingEndpointsOnConfigFailureOnDetach;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingU1;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingU1U2OnExitLatencyTooLargeError;

STATE_ENTRY_FUNCTION          HUBDSM_DisablingU2;

STATE_ENTRY_FUNCTION          HUBDSM_EnablingU1;

STATE_ENTRY_FUNCTION          HUBDSM_EnablingU2;

STATE_ENTRY_FUNCTION          HUBDSM_InitializingU1U2Flags;

STATE_ENTRY_FUNCTION          HUBDSM_ProgrammingEndpointsInControllerAfterDecreasingExitLatency;

STATE_ENTRY_FUNCTION          HUBDSM_ProgrammingEndpointsInControllerAfterIncreasingExitLatency;

STATE_ENTRY_FUNCTION          HUBDSM_QueuingSetU1TimeoutToPsm;

STATE_ENTRY_FUNCTION          HUBDSM_QueuingSetU2TimeoutToPsm;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureInUpdatingLPMValues;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInUpdatingLPMValues;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningPortDetachDeviceInUpdatingLPMValues;

STATE_ENTRY_FUNCTION          HUBDSM_UpdatingDeviceExitLatencyInTheControllerAfterDecreasingExitLatency;

STATE_ENTRY_FUNCTION          HUBDSM_UpdatingDeviceExitLatencyInTheControllerAfterIncreasingExitLatency;

STATE_ENTRY_FUNCTION          HUBDSM_UpdatingDeviceStatusToU1Disabled;

STATE_ENTRY_FUNCTION          HUBDSM_UpdatingDeviceStatusToU1Enabled;

STATE_ENTRY_FUNCTION          HUBDSM_UpdatingDeviceStatusToU2Disabled;

STATE_ENTRY_FUNCTION          HUBDSM_UpdatingDeviceStatusToU2Enabled;

STATE_ENTRY_FUNCTION          HUBDSM_ComputingLPMTimeoutValuesInUnconfigured;

STATE_ENTRY_FUNCTION          HUBDSM_QueuingSetU2TimeoutToPsmForEnumeratedDevice;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureInUpdatingLPMValuesInUnconfigured;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInUpdatingLPMValuesInUnconfigured;

STATE_ENTRY_FUNCTION          HUBDSM_GettingRemoteWakeCapability;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationFailureInGettingRemoteWakeCap;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInGettingRemoteWakeCap;

STATE_ENTRY_FUNCTION          HUBDSM_ValidatingInterfaceStatusForRemoteWake;

STATE_ENTRY_FUNCTION          HUBDSM_CheckingIf20LPMShouldBeEnabled;

STATE_ENTRY_FUNCTION          HUBDSM_ReturningOperationSuccessInSetting20LPMValues;

STATE_ENTRY_FUNCTION          HUBDSM_WaitingFor20LpmUpdateIoctlToComplete;




//
// Sub State Entries for the states in the State Machine
//


SUBSM_ENTRY DsmSubSmConfiguredSuspended[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateSuspended },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmConfiguredSuspendedOnHubSuspend[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateSuspendedInHubSuspend },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmConfiguredSuspendedWithSpecialFile[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateSuspended },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmConfiguredSuspending[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateCheckingIfDeviceArmedForWakeOnSuspending },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmConfiguredSuspendingWithSpecialFile[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateCheckingIfDeviceArmedForWakeOnSuspending },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmConfiguringDeviceFromCachedInformation[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateDeconfiguringEndpointsInControllerForDeviceConfiguration },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmConfiguringDeviceFromCachedInformationOnHubResume[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateDeconfiguringEndpointsInControllerForDeviceConfiguration },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmConfiguringDeviceFromCachedInformationOnResume[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateDeconfiguringEndpointsInControllerForDeviceConfiguration },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmConfiguringDeviceFromCachedInformationOnResumeWithSpecialFile[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateDeconfiguringEndpointsInControllerForDeviceConfiguration },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmConfiguringDeviceFromCachedInformationWithSpecialFile[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateDeconfiguringEndpointsInControllerForDeviceConfiguration },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmConfiguringOnSelectConfiguration[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateDeconfiguringEndpointsInControllerForDeviceConfiguration },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmDeviceDetachedAndDeporgrammedWithSpecialFile[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDeviceHubEventsForBootDevice },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmDeviceDetachedWithSpecialFile[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDeviceHubEventsForBootDevice },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmEnumeratedAsFailedUnknown[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsForFailedDevice },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmEnumerating[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInEnum },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmQueryingDeviceTextStringFromConfigured[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateQueryingLanguageSpecificProductIdString },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmQueryingDeviceTextStringFromStoppedEnumerated[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateQueryingLanguageSpecificProductIdString },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmQueryingDeviceTextStringFromUnConfigured[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateQueryingLanguageSpecificProductIdString },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReEnumeratingOnClientRequestInConfigured[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInReEnumDuringResetInConfigured },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReEnumeratingOnClientRequestInConfiguredWithSpecialFile[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInReEnumDuringResetInConfigured },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReEnumeratingOnClientRequestInUnconfigured[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInReEnumDuringResetInUnConfigured },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReEnumeratingOnClientRequestOnReattach[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInReEnumDuringResetInUnConfigured },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInReEnumDuringRestart },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReEnumeratingOnHubResumeInConfigured[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInReEnumDuringResetInConfigured },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReEnumeratingOnHubResumeInUnConfigured[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInReEnumDuringResetInUnConfigured },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReEnumeratingOnHubStart[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInReEnumDuringResetInUnConfigured },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReEnumeratingOnPdoPreStartWhileEnabled[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInReEnumDuringRestart },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReEnumeratingOnPdoStart[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInReEnumDuringRestart },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReEnumeratingOnPdoStartWhileEnabled[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInReEnumDuringResetInUnConfigured },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReEnumeratingOnResumeInConfigured[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInReEnumDuringResetInConfigured },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReEnumeratingOnResumeInConfiguredWithSpecialFile[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInReEnumDuringResetInConfigured },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReEnumeratingOnResumeInUnConfigured[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingEnumRetryCountInReEnumDuringResetInUnConfigured },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReportingDeviceToPnp[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateInitializingAddDeviceToGlobalListRetryCount },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReportingFailedDeviceAsMissing[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsWithPortOff },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmReportingFailedUnknownDeviceToPnp[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateCreatingUnknownChildPDOAndReportingToPnp },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmSettingDeviceInterface[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStatePreparingEndpointAndInterfaceListsOnSelectInterface },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmSettingLPMValuesInConfiguredOnSettingChange[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagVersion3X|DsmSubSmFlagSpeedAny|DsmSubSmFlagPortAny , DsmStateInitializingU1U2Flags },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmSettingLPMValuesInStoppedEnabled[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagVersion3X|DsmSubSmFlagSpeedAny|DsmSubSmFlagPortAny , DsmStateComputingLPMTimeoutValuesInUnconfigured },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmSettingLPMValuesInUnConfiguredOnSettingChange[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagVersion3X|DsmSubSmFlagSpeedAny|DsmSubSmFlagPortAny , DsmStateComputingLPMTimeoutValuesInUnconfigured },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmStoppedEnabled[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForInstallMSOSExt },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmStoppedEnumerated[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForInstallMSOSExt },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmStoppedEnumeratedConfiguredForFaultyClients[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForInstallMSOSExt },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmStoppedEnumeratedWithHubSuspended[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateSuspendedInHubSuspend },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmStoppedWithHubSuspended[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateSuspendedInHubSuspend },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmUnconfiguredSuspended[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateSuspended },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmUnConfiguredSuspendedOnHubSuspend[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateSuspendedInHubSuspend },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmUnconfiguredSuspending[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateCheckingIfDeviceArmedForWakeOnSuspending },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForD0EntryForFailedDevice[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsForFailedDevice },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForDetachOrRemove[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEvents },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForDetachOrRemoveInConfigured[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsExceptStop },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForDetachOrRemoveInDisabledWithPortOff[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsWithPortOff },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForDetachOrRemoveInUnConfigured[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsExceptStop },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForDetachOrRemoveWithPortOff[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsWithPortOff },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForDetachOrReset[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDeviceHubEventsForBootDevice },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForDetachOrResetWithDeviceDeporgrammed[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDeviceHubEventsForBootDevice },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForDetachOrToBeDeleted[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEvents },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForDetachOrToBeDeletedWithPortOff[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsWithPortOff },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForPDOD0ExitOnDetachInConfigured[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsExceptStop },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForPDOD0ExitOnDetachInUnConfigured[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsExceptStop },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForPDOReportedMissing[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsWithPortOff },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForRemoveOnDetach[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsWithPortOff },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForRemoveOrReprotedMissingOnHubStopOnDetach[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsWithPortOff },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForReportedMissingOnDetachInConfigured[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsExceptStop },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmWaitingForReportedMissingOnDetachInUnConfigured[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateWaitingForDevicePortEventsExceptStop },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmEnumeratingAfterAddressInEnum[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateGettingDeviceDescriptorAfterAddressing },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmEnumeratingAtAddressZeroInEnum[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagPort20|DsmSubSmFlagVersionAny|DsmSubSmFlagSpeedAny , DsmStateResetting1 },
    { DsmSubSmFlagPort30|DsmSubSmFlagVersionAny|DsmSubSmFlagSpeedAny , DsmStateCreatingUCXDevice30 },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmSettingLPMValues[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagVersion3X|DsmSubSmFlagSpeedAny|DsmSubSmFlagPortAny , DsmStateCheckingIfSetIsochDelayShouldBeSkipped },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmSettingNonZeroAddressInEnum[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateSettingNonZeroAddress },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmStoppedSuspendedInEnum[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateSuspendedInHubSuspend },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmResettingDeviceInEnumeration[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateDeterminingPortResetTypeRequired },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmGettingRemainingDescriptorsAfterConfigDescriptor[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateGettingBosDescriptorSet },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmGettingBosDescriptorSet[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagVersion2X|DsmSubSmFlagVersion3X|DsmSubSmFlagSpeedAny|DsmSubSmFlagPortAny , DsmStateCheckingIfBOSDescriptorQueryShouldBeSkipped },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmGettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagVersion2X|DsmSubSmFlagVersion20|DsmSubSmFlagSpeedFull|DsmSubSmFlagPortAny , DsmStateGettingDeviceQualifierDescriptor },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmGettingMSOSAndSerialNumberDescriptor[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateCheckingIfMSOSDescriptorShouldBeQueried },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmEnumeratingAtAddressZeroOnRenum[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagPort30|DsmSubSmFlagVersionAny|DsmSubSmFlagSpeedAny , DsmStateDeterminingPortResetTypeRequired },
    { DsmSubSmFlagPort20|DsmSubSmFlagVersionAny|DsmSubSmFlagSpeedAny , DsmStateResetting1InRenum },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmSettingLPMValuesInReEnum[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagVersion3X|DsmSubSmFlagSpeedAny|DsmSubSmFlagPortAny , DsmStateCheckingIfSetIsochDelayShouldBeSkipped },
    { DsmSubSmFlagVersion2X|DsmSubSmFlagSpeedAny|DsmSubSmFlagPortAny , DsmStateCheckingIf20LPMShouldBeEnabled },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmSettingNonZeroAddressOnRenum[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateSettingNonZeroAddress },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmStoppedSuspendedOnRenum[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateSuspendedInHubSuspend },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmProgrammingEndpointsAndSettingLPMValuesrForDeviceConfiguration[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagVersion3X|DsmSubSmFlagSpeedAny|DsmSubSmFlagPortAny , DsmStateInitializingU1U2Flags },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmFailedDeviceHubSuspended[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateSuspendedInHubSuspend },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmSuspendedWithHubSuspended[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateSuspendedInHubSuspend },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmSuspendedWithHubSuspendedInD3Cold[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagAny , DsmStateSuspendedInHubSuspend },
    { DsmSubSmFlagNone , DsmStateNull }
};


SUBSM_ENTRY DsmSubSmProgrammingEndpointsAndSettingLPMValuesInSettingInterface[] = {
    // SubSmFilter, SubSmInitialState
    { DsmSubSmFlagVersion3X|DsmSubSmFlagSpeedAny|DsmSubSmFlagPortAny , DsmStateInitializingU1U2Flags },
    { DsmSubSmFlagNone , DsmStateNull }
};


//
// State Entries for the states in the State Machine
//
STATE_ENTRY   DsmStateEntryAckingD0ForFaultyClientDrivers = {
    // State ETW Name
    DsmStateEtwAckingD0ForFaultyClientDrivers,
    // State Entry Function
    HUBDSM_AckingD0ForFaultyClientDrivers,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguredInD0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAckingHubStopAfterSuspendFromStoppedState = {
    // State ETW Name
    DsmStateEtwAckingHubStopAfterSuspendFromStoppedState,
    // State Entry Function
    HUBDSM_AckingHubStopAfterSuspendFromStoppedState,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateStoppedWithHubSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAckingPdoPreStart = {
    // State ETW Name
    DsmStateEtwAckingPdoPreStart,
    // State Entry Function
    HUBDSM_AckingPdoPreStart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateStoppedEnumerated },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAckingPdoPreStartFromStoppedEnabled = {
    // State ETW Name
    DsmStateEtwAckingPdoPreStartFromStoppedEnabled,
    // State Entry Function
    HUBDSM_AckingPdoPreStartFromStoppedEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateStoppedEnabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAckingQueryResourcesInConfiguredD0 = {
    // State ETW Name
    DsmStateEtwAckingQueryResourcesInConfiguredD0,
    // State Entry Function
    HUBDSM_AckingQueryResourcesInConfiguredD0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguredInD0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAckingStopAfterSuspendOnDetach = {
    // State ETW Name
    DsmStateEtwAckingStopAfterSuspendOnDetach,
    // State Entry Function
    HUBDSM_AckingStopAfterSuspendOnDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForRemoveOrReprotedMissingOnHubStopOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAcquiringPowerReferenceOnHubOnAttach = {
    // State ETW Name
    DsmStateEtwAcquiringPowerReferenceOnHubOnAttach,
    // State Entry Function
    HUBDSM_AcquiringPowerReferenceOnHubOnAttach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationPending ,    DsmStateEnumerating },
        { DsmEventOperationSuccess ,    DsmStateEnumerating },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAcquiringPowerReferenceOnHubStart = {
    // State ETW Name
    DsmStateEtwAcquiringPowerReferenceOnHubStart,
    // State Entry Function
    HUBDSM_AcquiringPowerReferenceOnHubStart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateStopped },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryBugcheckingSystemOnBootDeviceEnumerationFailure = {
    // State ETW Name
    DsmStateEtwBugcheckingSystemOnBootDeviceEnumerationFailure,
    // State Entry Function
    HUBDSM_BugcheckingSystemOnBootDeviceEnumerationFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckIfDeviceCanBeDisabledOnDriverNotFound = {
    // State ETW Name
    DsmStateEtwCheckIfDeviceCanBeDisabledOnDriverNotFound,
    // State Entry Function
    HUBDSM_CheckIfDeviceCanBeDisabledOnDriverNotFound,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateDisablingDeviceInControllerOnStartTimeout },
        { DsmEventNo ,                  DsmStateSettingForceResetOnRenumFlag },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingForZeroBwPipeOnClientClearStall = {
    // State ETW Name
    DsmStateEtwCheckingForZeroBwPipeOnClientClearStall,
    // State Entry Function
    HUBDSM_CheckingForZeroBwPipeOnClientClearStall,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateCompletingClientRequestInConfigured },
        { DsmEventNo ,                  DsmStateClearingEndpointHaltOnClientClearStall },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingForZeroBwPipeOnClientResetPipe = {
    // State ETW Name
    DsmStateEtwCheckingForZeroBwPipeOnClientResetPipe,
    // State Entry Function
    HUBDSM_CheckingForZeroBwPipeOnClientResetPipe,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateCompletingClientRequestInConfigured },
        { DsmEventNo ,                  DsmStateCheckingIfItIsIsochronousEndpoint },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingForZeroBwPipeOnClientSyncResetPipe = {
    // State ETW Name
    DsmStateEtwCheckingForZeroBwPipeOnClientSyncResetPipe,
    // State Entry Function
    HUBDSM_CheckingForZeroBwPipeOnClientSyncResetPipe,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateCompletingClientRequestInConfigured },
        { DsmEventNo ,                  DsmStateResettingPipeInUCXOnClientSyncResetPipe },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfBootDeviceOnD0Exit = {
    // State ETW Name
    DsmStateEtwCheckingIfBootDeviceOnD0Exit,
    // State Entry Function
    HUBDSM_CheckingIfBootDeviceOnD0Exit,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateConfiguredSuspending },
        { DsmEventYes ,                 DsmStateConfiguredSuspendingWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfBootDeviceOnDetach = {
    // State ETW Name
    DsmStateEtwCheckingIfBootDeviceOnDetach,
    // State Entry Function
    HUBDSM_CheckingIfBootDeviceOnDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateWaitingForReportedMissingOnDetachInConfigured },
        { DsmEventYes ,                 DsmStateDisablingEndpointsOnDetachWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfBootDeviceOnReset = {
    // State ETW Name
    DsmStateEtwCheckingIfBootDeviceOnReset,
    // State Entry Function
    HUBDSM_CheckingIfBootDeviceOnReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateSettingFailIoFlagOnClientReset },
        { DsmEventNo ,                  DsmStateReEnumeratingOnClientRequestInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDeviceHasReceivedFirstStart = {
    // State ETW Name
    DsmStateEtwCheckingIfDeviceHasReceivedFirstStart,
    // State Entry Function
    HUBDSM_CheckingIfDeviceHasReceivedFirstStart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateConfiguredInD0 },
        { DsmEventNo ,                  DsmStateStoppedEnumeratedConfiguredForFaultyClients },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDeviceHasReceivedFirstStartInUncoonfigured = {
    // State ETW Name
    DsmStateEtwCheckingIfDeviceHasReceivedFirstStartInUncoonfigured,
    // State Entry Function
    HUBDSM_CheckingIfDeviceHasReceivedFirstStartInUncoonfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateStoppedEnumerated },
        { DsmEventYes ,                 DsmStateUnConfiguredInD0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDeviceShouldBeDisabled = {
    // State ETW Name
    DsmStateEtwCheckingIfDeviceShouldBeDisabled,
    // State Entry Function
    HUBDSM_CheckingIfDeviceShouldBeDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateSignalingPnpPowerEventOnStopEnable },
        { DsmEventYes ,                 DsmStateDisablingDeviceInControllerOnPDOStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfItIsIsochronousEndpoint = {
    // State ETW Name
    DsmStateEtwCheckingIfItIsIsochronousEndpoint,
    // State Entry Function
    HUBDSM_CheckingIfItIsIsochronousEndpoint,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateResettingPipeInUCXOnClientReset },
        { DsmEventNo ,                  DsmStateClearingEndpointHaltOnClientResetPipe },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCleaningUpAnyExistingConfigurationOnStart = {
    // State ETW Name
    DsmStateEtwCleaningUpAnyExistingConfigurationOnStart,
    // State Entry Function
    HUBDSM_CleaningUpAnyExistingConfigurationOnStart,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCompletingD0EntryInUnconfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryClearingEndpointHaltOnClientClearStall = {
    // State ETW Name
    DsmStateEtwClearingEndpointHaltOnClientClearStall,
    // State Entry Function
    HUBDSM_ClearingEndpointHaltOnClientClearStall,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateCompletingClientRequestInConfiguredWithLastStatus },
        { DsmEventTransferSuccess ,     DsmStateCompletingClientRequestInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryClearingEndpointHaltOnClientResetPipe = {
    // State ETW Name
    DsmStateEtwClearingEndpointHaltOnClientResetPipe,
    // State Entry Function
    HUBDSM_ClearingEndpointHaltOnClientResetPipe,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateResettingPipeInUCXOnClientReset },
        { DsmEventTransferFailure ,     DsmStateCompletingClientRequestInConfiguredWithLastStatus },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryClearingFailIoFlagOnSuccessfulRecovery = {
    // State ETW Name
    DsmStateEtwClearingFailIoFlagOnSuccessfulRecovery,
    // State Entry Function
    HUBDSM_ClearingFailIoFlagOnSuccessfulRecovery,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCompletingClientRequestInConfiguredWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingClientRequestInConfigured = {
    // State ETW Name
    DsmStateEtwCompletingClientRequestInConfigured,
    // State Entry Function
    HUBDSM_CompletingClientRequestInConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDeviceHasReceivedFirstStart },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingClientRequestInConfiguredWithLastStatus = {
    // State ETW Name
    DsmStateEtwCompletingClientRequestInConfiguredWithLastStatus,
    // State Entry Function
    HUBDSM_CompletingClientRequestInConfiguredWithLastStatus,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDeviceHasReceivedFirstStart },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingClientRequestInConfiguredWithSpecialFile = {
    // State ETW Name
    DsmStateEtwCompletingClientRequestInConfiguredWithSpecialFile,
    // State Entry Function
    HUBDSM_CompletingClientRequestInConfiguredWithSpecialFile,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguredInD0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingClientRequestInUnconfigured = {
    // State ETW Name
    DsmStateEtwCompletingClientRequestInUnconfigured,
    // State Entry Function
    HUBDSM_CompletingClientRequestInUnconfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDeviceHasReceivedFirstStartInUncoonfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingClientRequestInUnConfiguredWithLastStatus = {
    // State ETW Name
    DsmStateEtwCompletingClientRequestInUnConfiguredWithLastStatus,
    // State Entry Function
    HUBDSM_CompletingClientRequestInUnConfiguredWithLastStatus,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDeviceHasReceivedFirstStartInUncoonfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingClientRequestOnDetachInConfigured = {
    // State ETW Name
    DsmStateEtwCompletingClientRequestOnDetachInConfigured,
    // State Entry Function
    HUBDSM_CompletingClientRequestOnDetachInConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForReportedMissingOnDetachInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingClientRequestOnDetachInDisabled = {
    // State ETW Name
    DsmStateEtwCompletingClientRequestOnDetachInDisabled,
    // State Entry Function
    HUBDSM_CompletingClientRequestOnDetachInDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReportingDeviceMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingClientRequestOnDetachWithDeviceProgrammed = {
    // State ETW Name
    DsmStateEtwCompletingClientRequestOnDetachWithDeviceProgrammed,
    // State Entry Function
    HUBDSM_CompletingClientRequestOnDetachWithDeviceProgrammed,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingClientRequestOnFailureInConfigured = {
    // State ETW Name
    DsmStateEtwCompletingClientRequestOnFailureInConfigured,
    // State Entry Function
    HUBDSM_CompletingClientRequestOnFailureInConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDeviceHasReceivedFirstStart },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingClientRequestOnFailureInDisabled = {
    // State ETW Name
    DsmStateEtwCompletingClientRequestOnFailureInDisabled,
    // State Entry Function
    HUBDSM_CompletingClientRequestOnFailureInDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateIssuingPortCycleOnFailueWithEndpointsToBeDeleted },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingClientRequestOnFailureInUnconfigured = {
    // State ETW Name
    DsmStateEtwCompletingClientRequestOnFailureInUnconfigured,
    // State Entry Function
    HUBDSM_CompletingClientRequestOnFailureInUnconfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDeviceHasReceivedFirstStartInUncoonfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingClientUnconfigureRequest = {
    // State ETW Name
    DsmStateEtwCompletingClientUnconfigureRequest,
    // State Entry Function
    HUBDSM_CompletingClientUnconfigureRequest,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDeviceHasReceivedFirstStartInUncoonfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingClientUnconfigureRequestWithFailure = {
    // State ETW Name
    DsmStateEtwCompletingClientUnconfigureRequestWithFailure,
    // State Entry Function
    HUBDSM_CompletingClientUnconfigureRequestWithFailure,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDeviceHasReceivedFirstStartInUncoonfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingD0EntryInConfigured = {
    // State ETW Name
    DsmStateEtwCompletingD0EntryInConfigured,
    // State Entry Function
    HUBDSM_CompletingD0EntryInConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguredInD0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingD0EntryInConfiguredWithSpecialFile = {
    // State ETW Name
    DsmStateEtwCompletingD0EntryInConfiguredWithSpecialFile,
    // State Entry Function
    HUBDSM_CompletingD0EntryInConfiguredWithSpecialFile,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguredInD0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingD0EntryInConfiguredWithSpecialFileOnDetach = {
    // State ETW Name
    DsmStateEtwCompletingD0EntryInConfiguredWithSpecialFileOnDetach,
    // State Entry Function
    HUBDSM_CompletingD0EntryInConfiguredWithSpecialFileOnDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDeviceDetachedAndDeporgrammedWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingD0EntryInConfiguredWithSpecialFileOnDetachWithDeviceEnabled = {
    // State ETW Name
    DsmStateEtwCompletingD0EntryInConfiguredWithSpecialFileOnDetachWithDeviceEnabled,
    // State Entry Function
    HUBDSM_CompletingD0EntryInConfiguredWithSpecialFileOnDetachWithDeviceEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateQueueingDeviceDetachedToPsmForBootDeviceInUnconfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingD0EntryInUnconfigured = {
    // State ETW Name
    DsmStateEtwCompletingD0EntryInUnconfigured,
    // State Entry Function
    HUBDSM_CompletingD0EntryInUnconfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateUnConfiguredInD0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingD0EntryOnDetachWithSpecialFile = {
    // State ETW Name
    DsmStateEtwCompletingD0EntryOnDetachWithSpecialFile,
    // State Entry Function
    HUBDSM_CompletingD0EntryOnDetachWithSpecialFile,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateNotifyingDeviceDisconnectedOnDetachWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingD0ExitAfterHubSuspend = {
    // State ETW Name
    DsmStateEtwCompletingD0ExitAfterHubSuspend,
    // State Entry Function
    HUBDSM_CompletingD0ExitAfterHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguredSuspendedOnHubSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingD0ExitOnSuspendFailure = {
    // State ETW Name
    DsmStateEtwCompletingD0ExitOnSuspendFailure,
    // State Entry Function
    HUBDSM_CompletingD0ExitOnSuspendFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDisablingEndpointsOnFailureWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingDeviceCleanup = {
    // State ETW Name
    DsmStateEtwCompletingDeviceCleanup,
    // State Entry Function
    HUBDSM_CompletingDeviceCleanup,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachOrToBeDeleted },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingHubStopAfterSuspend = {
    // State ETW Name
    DsmStateEtwCompletingHubStopAfterSuspend,
    // State Entry Function
    HUBDSM_CompletingHubStopAfterSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachOrRemoveWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingHubStopWithPortOff = {
    // State ETW Name
    DsmStateEtwCompletingHubStopWithPortOff,
    // State Entry Function
    HUBDSM_CompletingHubStopWithPortOff,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachOrRemoveWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingPDOCleanupForUnknownDevice = {
    // State ETW Name
    DsmStateEtwCompletingPDOCleanupForUnknownDevice,
    // State Entry Function
    HUBDSM_CompletingPDOCleanupForUnknownDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateUnregisteringWithHSMOnDetachAfterCleanup },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingPdoExitFinalOnDetach = {
    // State ETW Name
    DsmStateEtwCompletingPdoExitFinalOnDetach,
    // State Entry Function
    HUBDSM_CompletingPdoExitFinalOnDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateUnregsiteringWithHsmOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingPdoExitFinalOnDetachOnDeviceReportedMissing = {
    // State ETW Name
    DsmStateEtwCompletingPdoExitFinalOnDetachOnDeviceReportedMissing,
    // State Entry Function
    HUBDSM_CompletingPdoExitFinalOnDetachOnDeviceReportedMissing,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateUnregisteringWithHsmOnDetachOnDeviceReportedMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingPdoReStartOnDetach = {
    // State ETW Name
    DsmStateEtwCompletingPdoReStartOnDetach,
    // State Entry Function
    HUBDSM_CompletingPdoReStartOnDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReportingDeviceMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingPdoReStartOnDetachWithDeviceEnabled = {
    // State ETW Name
    DsmStateEtwCompletingPdoReStartOnDetachWithDeviceEnabled,
    // State Entry Function
    HUBDSM_CompletingPdoReStartOnDetachWithDeviceEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingPdoReStartOnOperationFailure = {
    // State ETW Name
    DsmStateEtwCompletingPdoReStartOnOperationFailure,
    // State Entry Function
    HUBDSM_CompletingPdoReStartOnOperationFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateIssuingPortCycleOnFailueWithEndpointsToBeDeleted },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingPnpEventOnDetachInConfigured = {
    // State ETW Name
    DsmStateEtwCompletingPnpEventOnDetachInConfigured,
    // State Entry Function
    HUBDSM_CompletingPnpEventOnDetachInConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForReportedMissingOnDetachInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingPnpEventOnDetachInDisabled = {
    // State ETW Name
    DsmStateEtwCompletingPnpEventOnDetachInDisabled,
    // State Entry Function
    HUBDSM_CompletingPnpEventOnDetachInDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReportingDeviceMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingPnpEventOnDetachInUnConfigured = {
    // State ETW Name
    DsmStateEtwCompletingPnpEventOnDetachInUnConfigured,
    // State Entry Function
    HUBDSM_CompletingPnpEventOnDetachInUnConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingPnpEventOnDetachWhileRenumeratingOnResume = {
    // State ETW Name
    DsmStateEtwCompletingPnpEventOnDetachWhileRenumeratingOnResume,
    // State Entry Function
    HUBDSM_CompletingPnpEventOnDetachWhileRenumeratingOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingPnpEventOnFailureInDisabledWithPortOff = {
    // State ETW Name
    DsmStateEtwCompletingPnpEventOnFailureInDisabledWithPortOff,
    // State Entry Function
    HUBDSM_CompletingPnpEventOnFailureInDisabledWithPortOff,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateIssuingPortCycleOnFailueWithEndpointsToBeDeleted },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingPnpEventOnFailureWithEndpointsToBeDeleted = {
    // State ETW Name
    DsmStateEtwCompletingPnpEventOnFailureWithEndpointsToBeDeleted,
    // State Entry Function
    HUBDSM_CompletingPnpEventOnFailureWithEndpointsToBeDeleted,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateIssuingPortCycleOnFailueWithEndpointsToBeDeleted },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguredInD0 = {
    // State ETW Name
    DsmStateEtwConfiguredInD0,
    // State Entry Function
    HUBDSM_ConfiguredInD0,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventClientRequestCycle ,  DsmStateRequestPortCycle },
        { DsmEventPDOD0Entry ,          DsmStateAckingD0ForFaultyClientDrivers },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateDeConfiguringDeviceOnClientRequest },
        { DsmEventClientRequestReset ,  DsmStateCheckingIfBootDeviceOnReset },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInConfigured },
        { DsmEventClientRequestSelectConfiguration ,DsmStateValidatingSelectConfigUrbFromConfigured },
        { DsmEventClientRequestSetInterface ,DsmStateValidatingSelectInterfaceUrbFromConfigured },
        { DsmEventHubSuspend ,          DsmStatePurgingDeviceIoOnHubSuspend },
        { DsmEventPDOInstallMSOSExt ,   DsmStateAckingQueryResourcesInConfiguredD0 },
        { DsmEventPDOPrepareForHibernation ,DsmStatePurgingDeviceIoOnPrepareForHibernate },
        { DsmEventNoPingResponseError , DsmStateSettingExitLatencyAdjustmentOnNoPingResponseError },
        { DsmEventPortDetachDevice ,    DsmStatePurgingDeviceIoOnDetachInConfigured },
        { DsmEventPDOD0Exit ,           DsmStateCheckingIfBootDeviceOnD0Exit },
        { DsmEventLPMPowerSettingChange ,DsmStateSettingLPMValuesInConfiguredOnSettingChange },
        { DsmEventClientRequestSyncClearStall ,DsmStateGettingMatchingPipeHandleOnClientClearStall },
        { DsmEventQueryDeviceText ,     DsmStateQueryingDeviceTextStringFromConfigured },
        { DsmEventClientRequestSyncResetPipe ,DsmStateGettingMatchingPipeHandleOnClientSyncResetPipe },
        { DsmEventFDORequestGetDescriptor ,DsmStateQueryingDescriptorForFdoRequestFromConfigured },
        { DsmEventClientRequestResetPipe ,DsmStateGettingMatchingPipeHandleOnClientResetPipe },
        { DsmEventPDOD0ExitFinal ,      DsmStateDisablingEndpointsInControllerOnStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguredResumingOnHubResume = {
    // State ETW Name
    DsmStateEtwConfiguredResumingOnHubResume,
    // State Entry Function
    HUBDSM_ConfiguredResumingOnHubResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateNotifyingDeviceDisconnectedOnDetach },
        { DsmEventPortFailed ,          DsmStateRequestingPortCycleOnErrorFromConfigured },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateRequestingPortCycleOnErrorFromConfigured },
        { DsmEventPortResumed ,         DsmStateStartingDeviceIoOnHubResume },
        { DsmEventPortResumeTimedOut ,  DsmStateRequestingPortCycleOnErrorFromConfigured },
        { DsmEventPortStateDisabled ,   DsmStateRequestingPortCycleOnErrorFromConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguredSuspended = {
    // State ETW Name
    DsmStateEtwConfiguredSuspended,
    // State Entry Function
    HUBDSM_ConfiguredSuspended,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmConfiguredSuspended,
    // Event State Pairs for Transitions
    {
        { DsmEventClientRequestSelectConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSetInterface ,DsmStateCompleteWithStatusFailed },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventQueryDeviceText ,     DsmStateSignalQueryDeviceTextEvent },
        { DsmEventClientRequestCycle ,  DsmStateRequestPortCycle },
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestReset ,  DsmStateCompleteWithStatusFailed },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventDeviceRenumerateOnHubResume ,DsmStateReEnumeratingOnResumeInConfigured },
        { DsmEventDeviceResumed ,       DsmStateCompletingD0EntryInConfigured },
        { DsmEventHubStop ,             DsmStateDisablingEndpointsInControllerOnFailureWithPortOff },
        { DsmEventPortDetachDevice ,    DsmStateNotifyingDeviceDisconnectedOnDetach },
        { DsmEventPDOCleanup ,          DsmStateDisablingEndpointsInControllerOnCleanupWithPortOff },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInConfiguredSuspended },
        { DsmEventPortFailed ,          DsmStateRequestingPortCycleOnErrorFromConfigured },
        { DsmEventHubStopAfterSuspend , DsmStateDisablingEndpointsInControllerOnHubStopAfterSuspend },
        { DsmEventPDOPreStart ,         DsmStateDisablingEndpointsInControllerOnRenumerationWhileSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguredSuspendedOnHubSuspend = {
    // State ETW Name
    DsmStateEtwConfiguredSuspendedOnHubSuspend,
    // State Entry Function
    HUBDSM_ConfiguredSuspendedOnHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmConfiguredSuspendedOnHubSuspend,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceRenumerateOnHubResume ,DsmStateReEnumeratingOnHubResumeInConfigured },
        { DsmEventDeviceSuspendedOnHubResume ,DsmStateConfiguredResumingOnHubResume },
        { DsmEventDeviceResumedOnHubResume ,DsmStateStartingDeviceIoOnHubResume },
        { DsmEventPortDetachDevice ,    DsmStateNotifyingDeviceDisconnectedOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguredSuspendedWithSpecialFile = {
    // State ETW Name
    DsmStateEtwConfiguredSuspendedWithSpecialFile,
    // State Entry Function
    HUBDSM_ConfiguredSuspendedWithSpecialFile,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmConfiguredSuspendedWithSpecialFile,
    // Event State Pairs for Transitions
    {
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventQueryDeviceText ,     DsmStateSignalQueryDeviceTextEvent },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSetInterface ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventClientRequestReset ,  DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSelectConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventDeviceRenumerateOnHubResume ,DsmStateReEnumeratingOnResumeInConfiguredWithSpecialFile },
        { DsmEventDeviceResumed ,       DsmStateCompletingD0EntryInConfiguredWithSpecialFile },
        { DsmEventPortDetachDevice ,    DsmStateNotifyingDeviceDisconnectedOnDetachWithSpecialFile },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInConfiguredSuspendedWithSpecialFile },
        { DsmEventPortFailed ,          DsmStateDisablingEndpointsOnFailureWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguredSuspending = {
    // State ETW Name
    DsmStateEtwConfiguredSuspending,
    // State Entry Function
    HUBDSM_ConfiguredSuspending,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmConfiguredSuspending,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguredSuspended },
        { DsmEventOperationFailure ,    DsmStateConfiguredSuspended },
        { DsmEventPortDetachDevice ,    DsmStateCompletingPnpEventOnDetachInConfigured },
        { DsmEventHubSuspend ,          DsmStatePurgingDeviceIoOnHubSuspendFromSuspending },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguredSuspendingOnHubSuspend = {
    // State ETW Name
    DsmStateEtwConfiguredSuspendingOnHubSuspend,
    // State Entry Function
    HUBDSM_ConfiguredSuspendingOnHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortFailed ,          DsmStateReleasingPowerReferenceOnSuspendFailureInConfigured },
        { DsmEventPortSuspended ,       DsmStateConfiguredSuspendedOnHubSuspend },
        { DsmEventPortDetachDevice ,    DsmStateReleasingPowerReferenceOnDetachInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguredSuspendingWithSpecialFile = {
    // State ETW Name
    DsmStateEtwConfiguredSuspendingWithSpecialFile,
    // State Entry Function
    HUBDSM_ConfiguredSuspendingWithSpecialFile,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmConfiguredSuspendingWithSpecialFile,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguredSuspendedWithSpecialFile },
        { DsmEventOperationFailure ,    DsmStateCompletingD0ExitOnSuspendFailure },
        { DsmEventPortDetachDevice ,    DsmStateNotifyingDeviceDisconnectedOnDetachWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguringDeviceFromCachedInformation = {
    // State ETW Name
    DsmStateEtwConfiguringDeviceFromCachedInformation,
    // State Entry Function
    HUBDSM_ConfiguringDeviceFromCachedInformation,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmConfiguringDeviceFromCachedInformation,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateCompletingClientRequestOnFailureInUnconfigured },
        { DsmEventOperationSuccess ,    DsmStateCompletingClientRequestInConfigured },
        { DsmEventPortDetachDevice ,    DsmStateCompletingClientRequestOnDetachInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguringDeviceFromCachedInformationOnHubResume = {
    // State ETW Name
    DsmStateEtwConfiguringDeviceFromCachedInformationOnHubResume,
    // State Entry Function
    HUBDSM_ConfiguringDeviceFromCachedInformationOnHubResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmConfiguringDeviceFromCachedInformationOnHubResume,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguredInD0 },
        { DsmEventOperationFailure ,    DsmStateDisablingPortOnFailureInUnConfigured },
        { DsmEventPortDetachDevice ,    DsmStateNotifyingDeviceDisconnectedOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguringDeviceFromCachedInformationOnResume = {
    // State ETW Name
    DsmStateEtwConfiguringDeviceFromCachedInformationOnResume,
    // State Entry Function
    HUBDSM_ConfiguringDeviceFromCachedInformationOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmConfiguringDeviceFromCachedInformationOnResume,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCompletingD0EntryInConfigured },
        { DsmEventOperationFailure ,    DsmStateRequestingCyclePortAndCompletingPnpEventOnFailureInUnconfigured },
        { DsmEventPortDetachDevice ,    DsmStateCompletingPnpEventOnDetachInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguringDeviceFromCachedInformationOnResumeWithSpecialFile = {
    // State ETW Name
    DsmStateEtwConfiguringDeviceFromCachedInformationOnResumeWithSpecialFile,
    // State Entry Function
    HUBDSM_ConfiguringDeviceFromCachedInformationOnResumeWithSpecialFile,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmConfiguringDeviceFromCachedInformationOnResumeWithSpecialFile,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateBugcheckingSystemOnBootDeviceEnumerationFailure },
        { DsmEventOperationSuccess ,    DsmStateCompletingD0EntryInConfiguredWithSpecialFile },
        { DsmEventPortDetachDevice ,    DsmStateCompletingD0EntryOnDetachWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguringDeviceFromCachedInformationWithSpecialFile = {
    // State ETW Name
    DsmStateEtwConfiguringDeviceFromCachedInformationWithSpecialFile,
    // State Entry Function
    HUBDSM_ConfiguringDeviceFromCachedInformationWithSpecialFile,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmConfiguringDeviceFromCachedInformationWithSpecialFile,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateClearingFailIoFlagOnSuccessfulRecovery },
        { DsmEventOperationFailure ,    DsmStateBugcheckingSystemOnBootDeviceEnumerationFailure },
        { DsmEventPortDetachDevice ,    DsmStateDisablingEndpointsOnDetachWithSpecialFileAndResetPending },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguringOnSelectConfiguration = {
    // State ETW Name
    DsmStateEtwConfiguringOnSelectConfiguration,
    // State Entry Function
    HUBDSM_ConfiguringOnSelectConfiguration,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmConfiguringOnSelectConfiguration,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateSettingConfigurationInformationInClientRequest },
        { DsmEventOperationFailure ,    DsmStateCompletingClientUnconfigureRequestWithFailure },
        { DsmEventPortDetachDevice ,    DsmStateCompletingClientRequestOnDetachInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCyclingPortOnRenumerationOfUnknownDevice = {
    // State ETW Name
    DsmStateEtwCyclingPortOnRenumerationOfUnknownDevice,
    // State Entry Function
    HUBDSM_CyclingPortOnRenumerationOfUnknownDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateD0EntryForUnknownDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryD0EntryForUnknownDevice = {
    // State ETW Name
    DsmStateEtwD0EntryForUnknownDevice,
    // State Entry Function
    HUBDSM_D0EntryForUnknownDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateEnumeratedAsFailedUnknown },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeConfiguringDeviceOnClientRequest = {
    // State ETW Name
    DsmStateEtwDeConfiguringDeviceOnClientRequest,
    // State Entry Function
    HUBDSM_DeConfiguringDeviceOnClientRequest,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateDisablingEndpointsInControllerOnClientRequest },
        { DsmEventTransferSuccess ,     DsmStateDisablingEndpointsInControllerOnClientRequest },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeConfiguringDeviceOnClientRequestFromUnconfigured = {
    // State ETW Name
    DsmStateEtwDeConfiguringDeviceOnClientRequestFromUnconfigured,
    // State Entry Function
    HUBDSM_DeConfiguringDeviceOnClientRequestFromUnconfigured,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateCompletingClientRequestInUnconfigured },
        { DsmEventTransferFailure ,     DsmStateCompletingClientRequestInUnconfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingDefaultEndpointAndDeviceOnDetachDuringEnum = {
    // State ETW Name
    DsmStateEtwDeletingDefaultEndpointAndDeviceOnDetachDuringEnum,
    // State Entry Function
    HUBDSM_DeletingDefaultEndpointAndDeviceOnDetachDuringEnum,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateIssuingDeviceDetachedToPSMOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingDeviceAndDefaultEndpointOnCleanup = {
    // State ETW Name
    DsmStateEtwDeletingDeviceAndDefaultEndpointOnCleanup,
    // State Entry Function
    HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanup,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachOrToBeDeleted },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingDeviceAndDefaultEndpointOnCleanupAfterDetach = {
    // State ETW Name
    DsmStateEtwDeletingDeviceAndDefaultEndpointOnCleanupAfterDetach,
    // State Entry Function
    HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanupAfterDetach,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingToBeDeleted },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingDeviceAndDefaultEndpointOnCleanupWithPortOff = {
    // State ETW Name
    DsmStateEtwDeletingDeviceAndDefaultEndpointOnCleanupWithPortOff,
    // State Entry Function
    HUBDSM_DeletingDeviceAndDefaultEndpointOnCleanupWithPortOff,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachOrToBeDeletedWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingEndpoinstAndDeviceOnDetachOnCleanup = {
    // State ETW Name
    DsmStateEtwDeletingEndpoinstAndDeviceOnDetachOnCleanup,
    // State Entry Function
    HUBDSM_DeletingEndpoinstAndDeviceOnDetachOnCleanup,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateIssuingDeviceDetachedToPSMOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingEndpointsAndDeviceOnOnCleanup = {
    // State ETW Name
    DsmStateEtwDeletingEndpointsAndDeviceOnOnCleanup,
    // State Entry Function
    HUBDSM_DeletingEndpointsAndDeviceOnOnCleanup,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCompletingDeviceCleanup },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeporgrammingAllEndpointsOnCleanupFromEnumeratedConfigured = {
    // State ETW Name
    DsmStateEtwDeporgrammingAllEndpointsOnCleanupFromEnumeratedConfigured,
    // State Entry Function
    HUBDSM_DeporgrammingAllEndpointsOnCleanupFromEnumeratedConfigured,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingPortOnPdoCleanup },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeporgrammingAllEndpointsOnHubStopFromEnumeratedConfigured = {
    // State ETW Name
    DsmStateEtwDeporgrammingAllEndpointsOnHubStopFromEnumeratedConfigured,
    // State Entry Function
    HUBDSM_DeporgrammingAllEndpointsOnHubStopFromEnumeratedConfigured,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingPortOnHubStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeprogrammingAllEndpointsOnHubSuspendFromEnumeratedConfigured = {
    // State ETW Name
    DsmStateEtwDeprogrammingAllEndpointsOnHubSuspendFromEnumeratedConfigured,
    // State Entry Function
    HUBDSM_DeprogrammingAllEndpointsOnHubSuspendFromEnumeratedConfigured,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateSuspendingOnHubSuspendInStopped },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDetachedAndDeprogrammedWithResetPending = {
    // State ETW Name
    DsmStateEtwDetachedAndDeprogrammedWithResetPending,
    // State Entry Function
    HUBDSM_DetachedAndDeprogrammedWithResetPending,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventPortReAttachDevice ,  DsmStateReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDetachedWithResetPending = {
    // State ETW Name
    DsmStateEtwDetachedWithResetPending,
    // State Entry Function
    HUBDSM_DetachedWithResetPending,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventPortReAttachDevice ,  DsmStateReEnumeratingOnClientRequestOnReattach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeviceDetachedAndDeporgrammedWithSpecialFile = {
    // State ETW Name
    DsmStateEtwDeviceDetachedAndDeporgrammedWithSpecialFile,
    // State Entry Function
    HUBDSM_DeviceDetachedAndDeporgrammedWithSpecialFile,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmDeviceDetachedAndDeporgrammedWithSpecialFile,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventQueryDeviceText ,     DsmStateSignalQueryDeviceTextEvent },
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSetInterface ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSelectConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestReset ,  DsmStateDetachedAndDeprogrammedWithResetPending },
        { DsmEventPortReAttachDevice ,  DsmStateWaitingForDetachOrResetWithDeviceDeporgrammed },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInDeviceDetachedAndDeporgrammedWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeviceDetachedWithSpecialFile = {
    // State ETW Name
    DsmStateEtwDeviceDetachedWithSpecialFile,
    // State Entry Function
    HUBDSM_DeviceDetachedWithSpecialFile,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmDeviceDetachedWithSpecialFile,
    // Event State Pairs for Transitions
    {
        { DsmEventClientRequestSetInterface ,DsmStateCompleteWithStatusFailed },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventClientRequestSelectConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventQueryDeviceText ,     DsmStateSignalQueryDeviceTextEvent },
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestReset ,  DsmStateDetachedWithResetPending },
        { DsmEventPortReAttachDevice ,  DsmStateWaitingForDetachOrReset },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInDeviceDetachedWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingBackPortInEnumeratedUnknown = {
    // State ETW Name
    DsmStateEtwDisablingBackPortInEnumeratedUnknown,
    // State Entry Function
    HUBDSM_DisablingBackPortInEnumeratedUnknown,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortFailed ,          DsmStateEnumeratedAsFailedUnknown },
        { DsmEventPortDisabled ,        DsmStateEnumeratedAsFailedUnknown },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingBackPortInWaitingForD0EntryForFailedDevice = {
    // State ETW Name
    DsmStateEtwDisablingBackPortInWaitingForD0EntryForFailedDevice,
    // State Entry Function
    HUBDSM_DisablingBackPortInWaitingForD0EntryForFailedDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDisabled ,        DsmStateWaitingForD0EntryForFailedDevice },
        { DsmEventPortDetachDevice ,    DsmStateReportingFailedDeviceAsMissing },
        { DsmEventPortFailed ,          DsmStateWaitingForD0EntryForFailedDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceAndDefaultEndpointInControllerOnDetachDuringEnum = {
    // State ETW Name
    DsmStateEtwDisablingDeviceAndDefaultEndpointInControllerOnDetachDuringEnum,
    // State Entry Function
    HUBDSM_DisablingDeviceAndDefaultEndpointInControllerOnDetachDuringEnum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingDefaultEndpointAndDeviceOnDetachDuringEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceAndDefaultEndpointOnDetachOnHubStart = {
    // State ETW Name
    DsmStateEtwDisablingDeviceAndDefaultEndpointOnDetachOnHubStart,
    // State Entry Function
    HUBDSM_DisablingDeviceAndDefaultEndpointOnDetachOnHubStart,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateReportingDeviceMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceAndDefaultEndpointOnEnumFailureOnHubStart = {
    // State ETW Name
    DsmStateEtwDisablingDeviceAndDefaultEndpointOnEnumFailureOnHubStart,
    // State Entry Function
    HUBDSM_DisablingDeviceAndDefaultEndpointOnEnumFailureOnHubStart,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateIssuingPortCycleOnFailueWithEndpointsToBeDeleted },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceAndDefaultEpInControllerOFailureWithPortOff = {
    // State ETW Name
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOFailureWithPortOff,
    // State Entry Function
    HUBDSM_DisablingDeviceAndDefaultEpInControllerOFailureWithPortOff,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCompletingHubStopWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnCleanupWithPortOff = {
    // State ETW Name
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnCleanupWithPortOff,
    // State Entry Function
    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnCleanupWithPortOff,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingDeviceAndDefaultEndpointOnCleanupWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnD0ExitOnDetachWithPDOMissing = {
    // State ETW Name
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnD0ExitOnDetachWithPDOMissing,
    // State Entry Function
    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnD0ExitOnDetachWithPDOMissing,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCompletingPdoExitFinalOnDetachOnDeviceReportedMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnDetach = {
    // State ETW Name
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnDetach,
    // State Entry Function
    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetach,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCompletingPdoExitFinalOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnDetachOnHubStopWithPDOMissing = {
    // State ETW Name
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnDetachOnHubStopWithPDOMissing,
    // State Entry Function
    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetachOnHubStopWithPDOMissing,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateUnregisteringWithHsmOnDetachOnDeviceReportedMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnDetachOnPDOCleaupWithPDOMissing = {
    // State ETW Name
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnDetachOnPDOCleaupWithPDOMissing,
    // State Entry Function
    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnDetachOnPDOCleaupWithPDOMissing,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateUnregsiteringWithHSMOnDetachOnPDOCleaupWithPDOMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspend = {
    // State ETW Name
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspend,
    // State Entry Function
    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspend,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCompletingHubStopAfterSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspendOnDetach = {
    // State ETW Name
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspendOnDetach,
    // State Entry Function
    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspendOnDetach,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateAckingStopAfterSuspendOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnHubStopOnDetach = {
    // State ETW Name
    DsmStateEtwDisablingDeviceAndDefaultEpInControllerOnHubStopOnDetach,
    // State Entry Function
    HUBDSM_DisablingDeviceAndDefaultEpInControllerOnHubStopOnDetach,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateReleasingDevicePowerReferenceOnStopAfterDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnCleanup = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnCleanup,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnCleanup,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingEndpointsAndDeviceOnOnCleanup },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnDetachOnCleanup = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnDetachOnCleanup,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnDetachOnCleanup,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingEndpoinstAndDeviceOnDetachOnCleanup },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnEnumFailureOnPdoPreStart = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnEnumFailureOnPdoPreStart,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnEnumFailureOnPdoPreStart,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCompletingPdoReStartOnOperationFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnEnumFailureOnRestart = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnEnumFailureOnRestart,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnEnumFailureOnRestart,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCompletingPnpEventOnFailureInDisabledWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnHubStopFromStoppedEnabled = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnHubStopFromStoppedEnabled,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnHubStopFromStoppedEnabled,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingPortOnHubStopFromStoppedEnabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnHubStopInStopped = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnHubStopInStopped,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnHubStopInStopped,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateReleasingPowerReferenceOnHubStopSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnHubStopInSuspended = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnHubStopInSuspended,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnHubStopInSuspended,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateReleasingPowerReferenceOnHubStopSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnPDOStop = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnPDOStop,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnPDOStop,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingPortOnPDOStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnRenumerationWhileSuspended = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnRenumerationWhileSuspended,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnRenumerationWhileSuspended,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateReEnumeratingOnPdoPreStartWhileEnabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnStartTimeout = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnStartTimeout,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnStartTimeout,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingPortOnStartTimeout },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsInControllerOHubStopAfterSuspendOnDetach = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsInControllerOHubStopAfterSuspendOnDetach,
    // State Entry Function
    HUBDSM_DisablingEndpointsInControllerOHubStopAfterSuspendOnDetach,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspendOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsInControllerOHubStopOnDetach = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsInControllerOHubStopOnDetach,
    // State Entry Function
    HUBDSM_DisablingEndpointsInControllerOHubStopOnDetach,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingDeviceAndDefaultEpInControllerOnHubStopOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsInControllerOnCleanupWithPortOff = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsInControllerOnCleanupWithPortOff,
    // State Entry Function
    HUBDSM_DisablingEndpointsInControllerOnCleanupWithPortOff,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingDeviceAndDefaultEpInControllerOnCleanupWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsInControllerOnClientRequest = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsInControllerOnClientRequest,
    // State Entry Function
    HUBDSM_DisablingEndpointsInControllerOnClientRequest,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCompletingClientUnconfigureRequest },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsInControllerOnD0ExitOnDetach = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsInControllerOnD0ExitOnDetach,
    // State Entry Function
    HUBDSM_DisablingEndpointsInControllerOnD0ExitOnDetach,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingDeviceAndDefaultEpInControllerOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsInControllerOnD0ExitOnDetachWithPDOMissing = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsInControllerOnD0ExitOnDetachWithPDOMissing,
    // State Entry Function
    HUBDSM_DisablingEndpointsInControllerOnD0ExitOnDetachWithPDOMissing,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingDeviceAndDefaultEpInControllerOnD0ExitOnDetachWithPDOMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsInControllerOnDetachOnHubStopWithPDOMissing = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsInControllerOnDetachOnHubStopWithPDOMissing,
    // State Entry Function
    HUBDSM_DisablingEndpointsInControllerOnDetachOnHubStopWithPDOMissing,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingDeviceAndDefaultEpInControllerOnDetachOnHubStopWithPDOMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsInControllerOnDetachOnPDOCleanupWithPDOMissing = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsInControllerOnDetachOnPDOCleanupWithPDOMissing,
    // State Entry Function
    HUBDSM_DisablingEndpointsInControllerOnDetachOnPDOCleanupWithPDOMissing,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingDeviceAndDefaultEpInControllerOnDetachOnPDOCleaupWithPDOMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsInControllerOnFailureWithPortOff = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsInControllerOnFailureWithPortOff,
    // State Entry Function
    HUBDSM_DisablingEndpointsInControllerOnFailureWithPortOff,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingDeviceAndDefaultEpInControllerOFailureWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsInControllerOnHubStopAfterSuspend = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsInControllerOnHubStopAfterSuspend,
    // State Entry Function
    HUBDSM_DisablingEndpointsInControllerOnHubStopAfterSuspend,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsInControllerOnRenumerationWhileSuspended = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsInControllerOnRenumerationWhileSuspended,
    // State Entry Function
    HUBDSM_DisablingEndpointsInControllerOnRenumerationWhileSuspended,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingDeviceInControllerOnRenumerationWhileSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsInControllerOnStop = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsInControllerOnStop,
    // State Entry Function
    HUBDSM_DisablingEndpointsInControllerOnStop,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCheckingIfDeviceShouldBeDisabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsOnDetachWithSpecialFile = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsOnDetachWithSpecialFile,
    // State Entry Function
    HUBDSM_DisablingEndpointsOnDetachWithSpecialFile,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateQueueingDeviceDetachedToPsmForBootDeviceInUnconfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsOnDetachWithSpecialFileAndResetPending = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsOnDetachWithSpecialFileAndResetPending,
    // State Entry Function
    HUBDSM_DisablingEndpointsOnDetachWithSpecialFileAndResetPending,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDetachedWithResetPending },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsOnFailureWithSpecialFile = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsOnFailureWithSpecialFile,
    // State Entry Function
    HUBDSM_DisablingEndpointsOnFailureWithSpecialFile,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateWaitingForDetachOrReset },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingPortOnFailureInUnConfigured = {
    // State ETW Name
    DsmStateEtwDisablingPortOnFailureInUnConfigured,
    // State Entry Function
    HUBDSM_DisablingPortOnFailureInUnConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDisabled ,        DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventPortFailed ,          DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingPortOnHubStop = {
    // State ETW Name
    DsmStateEtwDisablingPortOnHubStop,
    // State Entry Function
    HUBDSM_DisablingPortOnHubStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReleasingPowerReferenceOnHubSuspendStopOnDetach },
        { DsmEventPortFailed ,          DsmStateDisablingDeviceInControllerOnHubStopInStopped },
        { DsmEventPortDisabled ,        DsmStateDisablingDeviceInControllerOnHubStopInStopped },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingPortOnHubStopFromStoppedEnabled = {
    // State ETW Name
    DsmStateEtwDisablingPortOnHubStopFromStoppedEnabled,
    // State Entry Function
    HUBDSM_DisablingPortOnHubStopFromStoppedEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDisableCompleteOnHubStopFromStoppedEnabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingPortOnPdoCleanup = {
    // State ETW Name
    DsmStateEtwDisablingPortOnPdoCleanup,
    // State Entry Function
    HUBDSM_DisablingPortOnPdoCleanup,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateDisablingDeviceInControllerOnDetachOnCleanup },
        { DsmEventPortDisabled ,        DsmStateDisablingDeviceInControllerOnCleanup },
        { DsmEventPortFailed ,          DsmStateDisablingDeviceInControllerOnCleanup },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingPortOnPDOStop = {
    // State ETW Name
    DsmStateEtwDisablingPortOnPDOStop,
    // State Entry Function
    HUBDSM_DisablingPortOnPDOStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateCompletingPnpEventOnDetachInDisabled },
        { DsmEventPortDisabled ,        DsmStateSignalingPnpPowerEventOnStop },
        { DsmEventPortFailed ,          DsmStateSignalingPnpPowerEventOnStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingPortOnStartTimeout = {
    // State ETW Name
    DsmStateEtwDisablingPortOnStartTimeout,
    // State Entry Function
    HUBDSM_DisablingPortOnStartTimeout,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReportingDeviceMissing },
        { DsmEventPortFailed ,          DsmStateStopped },
        { DsmEventPortDisabled ,        DsmStateStopped },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryEnumeratedAsFailedUnknown = {
    // State ETW Name
    DsmStateEtwEnumeratedAsFailedUnknown,
    // State Entry Function
    HUBDSM_EnumeratedAsFailedUnknown,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmEnumeratedAsFailedUnknown,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOCleanup ,          DsmStateCompletingDeviceCleanup },
        { DsmEventPDOD0Entry ,          DsmStateCyclingPortOnRenumerationOfUnknownDevice },
        { DsmEventPortDetachDevice ,    DsmStateReportingFailedDeviceAsMissing },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateDisablingBackPortInEnumeratedUnknown },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryEnumerating = {
    // State ETW Name
    DsmStateEtwEnumerating,
    // State Entry Function
    HUBDSM_Enumerating,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmEnumerating,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventOperationFailure ,    DsmStateReleasingPowerReferenceOnHubOnEnumerationFailure },
        { DsmEventOperationSuccess ,    DsmStateReleasingPowerReferenceOnHubOnDetachOnEnumSuccess },
        { DsmEventPortDetachDevice ,    DsmStateReleasingPowerReferenceOnHubOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingPD0D0ExitFinalInConfigured = {
    // State ETW Name
    DsmStateEtwFlushingPD0D0ExitFinalInConfigured,
    // State Entry Function
    HUBDSM_FlushingPD0D0ExitFinalInConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachOrRemoveInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingPD0D0ExitFinalInUnConfigured = {
    // State ETW Name
    DsmStateEtwFlushingPD0D0ExitFinalInUnConfigured,
    // State Entry Function
    HUBDSM_FlushingPD0D0ExitFinalInUnConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInConfigured = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInConfigured,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInConfigured,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateCheckingIfDeviceHasReceivedFirstStart },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInConfiguredSuspended = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInConfiguredSuspended,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInConfiguredSuspended,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateConfiguredSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInConfiguredSuspendedWithSpecialFile = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInConfiguredSuspendedWithSpecialFile,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInConfiguredSuspendedWithSpecialFile,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateConfiguredSuspendedWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInDeviceDetachedAndDeporgrammedWithSpecialFile = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInDeviceDetachedAndDeporgrammedWithSpecialFile,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInDeviceDetachedAndDeporgrammedWithSpecialFile,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateDeviceDetachedAndDeporgrammedWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInDeviceDetachedWithSpecialFile = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInDeviceDetachedWithSpecialFile,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInDeviceDetachedWithSpecialFile,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateDeviceDetachedWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInStopEnabled = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInStopEnabled,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInStopEnabled,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateStoppedEnabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInStopped = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInStopped,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInStopped,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateStopped },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInStoppedWithHubSuspended = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInStoppedWithHubSuspended,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInStoppedWithHubSuspended,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateStoppedWithHubSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInUnconfiguredInD0 = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInUnconfiguredInD0,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInUnconfiguredInD0,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateUnConfiguredInD0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInUnconfiguredSuspended = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInUnconfiguredSuspended,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInUnconfiguredSuspended,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateUnconfiguredSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInWaitingForDetachorResetWithDeviceDeprogrammed = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInWaitingForDetachorResetWithDeviceDeprogrammed,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInWaitingForDetachorResetWithDeviceDeprogrammed,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateWaitingForDetachOrResetWithDeviceDeporgrammed },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInWaitingForDetatchorReset = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInWaitingForDetatchorReset,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInWaitingForDetatchorReset,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateWaitingForDetachOrReset },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingMatchingPipeHandleOnClientClearStall = {
    // State ETW Name
    DsmStateEtwGettingMatchingPipeHandleOnClientClearStall,
    // State Entry Function
    HUBDSM_GettingMatchingPipeHandleOnClientClearStall,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingForZeroBwPipeOnClientClearStall },
        { DsmEventOperationFailure ,    DsmStateCompletingClientRequestInConfiguredWithLastStatus },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingMatchingPipeHandleOnClientResetPipe = {
    // State ETW Name
    DsmStateEtwGettingMatchingPipeHandleOnClientResetPipe,
    // State Entry Function
    HUBDSM_GettingMatchingPipeHandleOnClientResetPipe,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingForZeroBwPipeOnClientResetPipe },
        { DsmEventOperationFailure ,    DsmStateCompletingClientRequestInConfiguredWithLastStatus },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingMatchingPipeHandleOnClientSyncResetPipe = {
    // State ETW Name
    DsmStateEtwGettingMatchingPipeHandleOnClientSyncResetPipe,
    // State Entry Function
    HUBDSM_GettingMatchingPipeHandleOnClientSyncResetPipe,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingForZeroBwPipeOnClientSyncResetPipe },
        { DsmEventOperationFailure ,    DsmStateCompletingClientRequestInConfiguredWithLastStatus },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryIssuingDeviceDetachedToPSMOnDetach = {
    // State ETW Name
    DsmStateEtwIssuingDeviceDetachedToPSMOnDetach,
    // State Entry Function
    HUBDSM_IssuingDeviceDetachedToPSMOnDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateUnregisteringWithHSMOnDetachAfterCleanup },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryIssuingPortCycleAndCompletingClientRequestOnEnumFailue = {
    // State ETW Name
    DsmStateEtwIssuingPortCycleAndCompletingClientRequestOnEnumFailue,
    // State Entry Function
    HUBDSM_IssuingPortCycleAndCompletingClientRequestOnEnumFailue,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryIssuingPortCycleAndSingalingPnpEventOnEnumFailueFromConfigured = {
    // State ETW Name
    DsmStateEtwIssuingPortCycleAndSingalingPnpEventOnEnumFailueFromConfigured,
    // State Entry Function
    HUBDSM_IssuingPortCycleAndSingalingPnpEventOnEnumFailueFromConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryIssuingPortCycleOnEnumFailureOnHubResume = {
    // State ETW Name
    DsmStateEtwIssuingPortCycleOnEnumFailureOnHubResume,
    // State Entry Function
    HUBDSM_IssuingPortCycleOnEnumFailureOnHubResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryIssuingPortCycleOnFailueWithEndpointsToBeDeleted = {
    // State ETW Name
    DsmStateEtwIssuingPortCycleOnFailueWithEndpointsToBeDeleted,
    // State Entry Function
    HUBDSM_IssuingPortCycleOnFailueWithEndpointsToBeDeleted,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachOrRemoveInDisabledWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryNotifyingDeviceDisconnectedOnDetach = {
    // State ETW Name
    DsmStateEtwNotifyingDeviceDisconnectedOnDetach,
    // State Entry Function
    HUBDSM_NotifyingDeviceDisconnectedOnDetach,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForReportedMissingOnDetachInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryNotifyingDeviceDisconnectedOnDetachWithSpecialFile = {
    // State ETW Name
    DsmStateEtwNotifyingDeviceDisconnectedOnDetachWithSpecialFile,
    // State Entry Function
    HUBDSM_NotifyingDeviceDisconnectedOnDetachWithSpecialFile,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDisablingEndpointsOnDetachWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPreparingEndpointAndInterfaceListsOnClientReset = {
    // State ETW Name
    DsmStateEtwPreparingEndpointAndInterfaceListsOnClientReset,
    // State Entry Function
    HUBDSM_PreparingEndpointAndInterfaceListsOnClientReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguringDeviceFromCachedInformation },
        { DsmEventOperationFailure ,    DsmStateCompletingClientRequestOnFailureInUnconfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPreparingEndpointAndInterfaceListsOnClientResetWithSpecialFile = {
    // State ETW Name
    DsmStateEtwPreparingEndpointAndInterfaceListsOnClientResetWithSpecialFile,
    // State Entry Function
    HUBDSM_PreparingEndpointAndInterfaceListsOnClientResetWithSpecialFile,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguringDeviceFromCachedInformationWithSpecialFile },
        { DsmEventOperationFailure ,    DsmStateBugcheckingSystemOnBootDeviceEnumerationFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPreparingEndpointAndInterfaceListsOnResetAfterHubResume = {
    // State ETW Name
    DsmStateEtwPreparingEndpointAndInterfaceListsOnResetAfterHubResume,
    // State Entry Function
    HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterHubResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateDisablingPortOnFailureInUnConfigured },
        { DsmEventOperationSuccess ,    DsmStateConfiguringDeviceFromCachedInformationOnHubResume },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPreparingEndpointAndInterfaceListsOnResetAfterResume = {
    // State ETW Name
    DsmStateEtwPreparingEndpointAndInterfaceListsOnResetAfterResume,
    // State Entry Function
    HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateRequestingCyclePortAndCompletingPnpEventOnFailureInUnconfigured },
        { DsmEventOperationSuccess ,    DsmStateConfiguringDeviceFromCachedInformationOnResume },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPreparingEndpointAndInterfaceListsOnResetAfterResumeWithSpecialFile = {
    // State ETW Name
    DsmStateEtwPreparingEndpointAndInterfaceListsOnResetAfterResumeWithSpecialFile,
    // State Entry Function
    HUBDSM_PreparingEndpointAndInterfaceListsOnResetAfterResumeWithSpecialFile,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateBugcheckingSystemOnBootDeviceEnumerationFailure },
        { DsmEventOperationSuccess ,    DsmStateConfiguringDeviceFromCachedInformationOnResumeWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPreparingEndpointAndInterfaceListsOnSelectConfiguration = {
    // State ETW Name
    DsmStateEtwPreparingEndpointAndInterfaceListsOnSelectConfiguration,
    // State Entry Function
    HUBDSM_PreparingEndpointAndInterfaceListsOnSelectConfiguration,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguringOnSelectConfiguration },
        { DsmEventOperationFailure ,    DsmStateCompletingClientRequestOnFailureInUnconfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPreparingEndpointAndInterfaceListsOnSelectConfigurationFromConfigured = {
    // State ETW Name
    DsmStateEtwPreparingEndpointAndInterfaceListsOnSelectConfigurationFromConfigured,
    // State Entry Function
    HUBDSM_PreparingEndpointAndInterfaceListsOnSelectConfigurationFromConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguringOnSelectConfiguration },
        { DsmEventOperationFailure ,    DsmStateCompletingClientRequestInConfiguredWithLastStatus },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPurgingDeviceIoOnDetachInConfigured = {
    // State ETW Name
    DsmStateEtwPurgingDeviceIoOnDetachInConfigured,
    // State Entry Function
    HUBDSM_PurgingDeviceIoOnDetachInConfigured,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCheckingIfBootDeviceOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPurgingDeviceIoOnHubSuspend = {
    // State ETW Name
    DsmStateEtwPurgingDeviceIoOnHubSuspend,
    // State Entry Function
    HUBDSM_PurgingDeviceIoOnHubSuspend,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateConfiguredSuspendingOnHubSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPurgingDeviceIoOnHubSuspendFromSuspending = {
    // State ETW Name
    DsmStateEtwPurgingDeviceIoOnHubSuspendFromSuspending,
    // State Entry Function
    HUBDSM_PurgingDeviceIoOnHubSuspendFromSuspending,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCompletingD0ExitAfterHubSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPurgingDeviceIoOnPrepareForHibernate = {
    // State ETW Name
    DsmStateEtwPurgingDeviceIoOnPrepareForHibernate,
    // State Entry Function
    HUBDSM_PurgingDeviceIoOnPrepareForHibernate,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateQueueingPrepareForHiberToPSM },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueryingDescriptorForFdoRequestFromConfigured = {
    // State ETW Name
    DsmStateEtwQueryingDescriptorForFdoRequestFromConfigured,
    // State Entry Function
    HUBDSM_QueryingDescriptorForFdoRequestFromConfigured,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateCheckingIfDeviceHasReceivedFirstStart },
        { DsmEventTransferSuccess ,     DsmStateCheckingIfDeviceHasReceivedFirstStart },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueryingDescriptorForFdoRequestFromStoppedEnumerated = {
    // State ETW Name
    DsmStateEtwQueryingDescriptorForFdoRequestFromStoppedEnumerated,
    // State Entry Function
    HUBDSM_QueryingDescriptorForFdoRequestFromStoppedEnumerated,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateStoppedEnumerated },
        { DsmEventTransferFailure ,     DsmStateStoppedEnumerated },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueryingDescriptorForFdoRequestFromUnConfigured = {
    // State ETW Name
    DsmStateEtwQueryingDescriptorForFdoRequestFromUnConfigured,
    // State Entry Function
    HUBDSM_QueryingDescriptorForFdoRequestFromUnConfigured,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateUnConfiguredInD0 },
        { DsmEventTransferSuccess ,     DsmStateUnConfiguredInD0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueryingDeviceTextStringFromConfigured = {
    // State ETW Name
    DsmStateEtwQueryingDeviceTextStringFromConfigured,
    // State Entry Function
    HUBDSM_QueryingDeviceTextStringFromConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmQueryingDeviceTextStringFromConfigured,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDeviceHasReceivedFirstStart },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueryingDeviceTextStringFromStoppedEnumerated = {
    // State ETW Name
    DsmStateEtwQueryingDeviceTextStringFromStoppedEnumerated,
    // State Entry Function
    HUBDSM_QueryingDeviceTextStringFromStoppedEnumerated,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmQueryingDeviceTextStringFromStoppedEnumerated,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateStoppedEnumerated },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueryingDeviceTextStringFromUnConfigured = {
    // State ETW Name
    DsmStateEtwQueryingDeviceTextStringFromUnConfigured,
    // State Entry Function
    HUBDSM_QueryingDeviceTextStringFromUnConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmQueryingDeviceTextStringFromUnConfigured,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateUnConfiguredInD0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueueingAttachFailureToPort = {
    // State ETW Name
    DsmStateEtwQueueingAttachFailureToPort,
    // State Entry Function
    HUBDSM_QueueingAttachFailureToPort,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingToBeDeleted },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueueingAttachSuccessToPort = {
    // State ETW Name
    DsmStateEtwQueueingAttachSuccessToPort,
    // State Entry Function
    HUBDSM_QueueingAttachSuccessToPort,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateAcquiringPowerReferenceOnHubOnAttach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueueingDeviceDetachedToPsmForBootDeviceInUnconfigured = {
    // State ETW Name
    DsmStateEtwQueueingDeviceDetachedToPsmForBootDeviceInUnconfigured,
    // State Entry Function
    HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceInUnconfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDeviceDetachedWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueueingDeviceDetachedToPsmForBootDeviceWithResetPending = {
    // State ETW Name
    DsmStateEtwQueueingDeviceDetachedToPsmForBootDeviceWithResetPending,
    // State Entry Function
    HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceWithResetPending,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDetachedWithResetPending },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueueingDeviceDetachedToPsmForBootDeviceWithResetPendingAndDeviceDeprogrammed = {
    // State ETW Name
    DsmStateEtwQueueingDeviceDetachedToPsmForBootDeviceWithResetPendingAndDeviceDeprogrammed,
    // State Entry Function
    HUBDSM_QueueingDeviceDetachedToPsmForBootDeviceWithResetPendingAndDeviceDeprogrammed,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDetachedAndDeprogrammedWithResetPending },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueueingPrepareForHiberToPSM = {
    // State ETW Name
    DsmStateEtwQueueingPrepareForHiberToPSM,
    // State Entry Function
    HUBDSM_QueueingPrepareForHiberToPSM,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortSuspended ,       DsmStateConfiguredSuspendedWithSpecialFile },
        { DsmEventPortDetachDevice ,    DsmStateNotifyingDeviceDisconnectedOnDetachWithSpecialFile },
        { DsmEventPortFailed ,          DsmStateCompletingD0ExitOnSuspendFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReDisablingPortAfterHubResume = {
    // State ETW Name
    DsmStateEtwReDisablingPortAfterHubResume,
    // State Entry Function
    HUBDSM_ReDisablingPortAfterHubResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDisabled ,        DsmStateStopped },
        { DsmEventPortFailed ,          DsmStateStopped },
        { DsmEventHubStop ,             DsmStateWaitingForDisableCompleteOnHubStopFromStoppedEnabled },
        { DsmEventPortDetachDevice ,    DsmStateReportingDeviceMissing },
        { DsmEventHubSuspend ,          DsmStateWaitingForDisableCompleteOnHubStopFromStoppedEnabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReEnumeratingOnClientRequestInConfigured = {
    // State ETW Name
    DsmStateEtwReEnumeratingOnClientRequestInConfigured,
    // State Entry Function
    HUBDSM_ReEnumeratingOnClientRequestInConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmReEnumeratingOnClientRequestInConfigured,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStatePreparingEndpointAndInterfaceListsOnClientReset },
        { DsmEventOperationFailure ,    DsmStateCompletingClientRequestOnFailureInDisabled },
        { DsmEventOperationFailureWithDeviceEnabled ,DsmStateIssuingPortCycleAndCompletingClientRequestOnEnumFailue },
        { DsmEventPortDetachDeviceWithDeviceEnabled ,DsmStateCompletingClientRequestOnDetachWithDeviceProgrammed },
        { DsmEventPortDetachDevice ,    DsmStateCompletingClientRequestOnDetachInDisabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReEnumeratingOnClientRequestInConfiguredWithSpecialFile = {
    // State ETW Name
    DsmStateEtwReEnumeratingOnClientRequestInConfiguredWithSpecialFile,
    // State Entry Function
    HUBDSM_ReEnumeratingOnClientRequestInConfiguredWithSpecialFile,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmReEnumeratingOnClientRequestInConfiguredWithSpecialFile,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStatePreparingEndpointAndInterfaceListsOnClientResetWithSpecialFile },
        { DsmEventPortDetachDeviceWithDeviceEnabled ,DsmStateQueueingDeviceDetachedToPsmForBootDeviceWithResetPending },
        { DsmEventOperationFailureWithDeviceEnabled ,DsmStateBugcheckingSystemOnBootDeviceEnumerationFailure },
        { DsmEventOperationFailure ,    DsmStateBugcheckingSystemOnBootDeviceEnumerationFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReEnumeratingOnClientRequestInUnconfigured = {
    // State ETW Name
    DsmStateEtwReEnumeratingOnClientRequestInUnconfigured,
    // State Entry Function
    HUBDSM_ReEnumeratingOnClientRequestInUnconfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmReEnumeratingOnClientRequestInUnconfigured,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateCompletingClientRequestOnFailureInDisabled },
        { DsmEventOperationSuccess ,    DsmStateCompletingClientRequestInUnconfigured },
        { DsmEventOperationFailureWithDeviceEnabled ,DsmStateIssuingPortCycleAndCompletingClientRequestOnEnumFailue },
        { DsmEventPortDetachDeviceWithDeviceEnabled ,DsmStateCompletingClientRequestOnDetachWithDeviceProgrammed },
        { DsmEventPortDetachDevice ,    DsmStateCompletingClientRequestOnDetachInDisabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReEnumeratingOnClientRequestOnReattach = {
    // State ETW Name
    DsmStateEtwReEnumeratingOnClientRequestOnReattach,
    // State Entry Function
    HUBDSM_ReEnumeratingOnClientRequestOnReattach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmReEnumeratingOnClientRequestOnReattach,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStatePreparingEndpointAndInterfaceListsOnClientResetWithSpecialFile },
        { DsmEventPortDetachDeviceWithDeviceEnabled ,DsmStateQueueingDeviceDetachedToPsmForBootDeviceWithResetPending },
        { DsmEventOperationFailure ,    DsmStateBugcheckingSystemOnBootDeviceEnumerationFailure },
        { DsmEventOperationFailureWithDeviceEnabled ,DsmStateBugcheckingSystemOnBootDeviceEnumerationFailure },
        { DsmEventPortDetachDevice ,    DsmStateQueueingDeviceDetachedToPsmForBootDeviceWithResetPendingAndDeviceDeprogrammed },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed = {
    // State ETW Name
    DsmStateEtwReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed,
    // State Entry Function
    HUBDSM_ReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStatePreparingEndpointAndInterfaceListsOnClientResetWithSpecialFile },
        { DsmEventOperationFailure ,    DsmStateBugcheckingSystemOnBootDeviceEnumerationFailure },
        { DsmEventOperationFailureWithDeviceEnabled ,DsmStateBugcheckingSystemOnBootDeviceEnumerationFailure },
        { DsmEventPortDetachDeviceWithDeviceEnabled ,DsmStateQueueingDeviceDetachedToPsmForBootDeviceWithResetPending },
        { DsmEventPortDetachDevice ,    DsmStateQueueingDeviceDetachedToPsmForBootDeviceWithResetPendingAndDeviceDeprogrammed },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReEnumeratingOnHubResumeInConfigured = {
    // State ETW Name
    DsmStateEtwReEnumeratingOnHubResumeInConfigured,
    // State Entry Function
    HUBDSM_ReEnumeratingOnHubResumeInConfigured,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmReEnumeratingOnHubResumeInConfigured,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventOperationFailureWithDeviceEnabled ,DsmStateIssuingPortCycleOnEnumFailureOnHubResume },
        { DsmEventOperationSuccess ,    DsmStatePreparingEndpointAndInterfaceListsOnResetAfterHubResume },
        { DsmEventOperationFailure ,    DsmStateIssuingPortCycleOnFailueWithEndpointsToBeDeleted },
        { DsmEventPortDetachDeviceWithDeviceEnabled ,DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventPortDetachDevice ,    DsmStateReportingDeviceMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReEnumeratingOnHubResumeInUnConfigured = {
    // State ETW Name
    DsmStateEtwReEnumeratingOnHubResumeInUnConfigured,
    // State Entry Function
    HUBDSM_ReEnumeratingOnHubResumeInUnConfigured,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmReEnumeratingOnHubResumeInUnConfigured,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventOperationSuccess ,    DsmStateUnConfiguredInD0 },
        { DsmEventPortDetachDeviceWithDeviceEnabled ,DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventOperationFailure ,    DsmStateIssuingPortCycleOnFailueWithEndpointsToBeDeleted },
        { DsmEventOperationFailureWithDeviceEnabled ,DsmStateIssuingPortCycleOnEnumFailureOnHubResume },
        { DsmEventPortDetachDevice ,    DsmStateReportingDeviceMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReEnumeratingOnHubStart = {
    // State ETW Name
    DsmStateEtwReEnumeratingOnHubStart,
    // State Entry Function
    HUBDSM_ReEnumeratingOnHubStart,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmReEnumeratingOnHubStart,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventOperationSuccess ,    DsmStateStoppedEnumerated },
        { DsmEventOperationFailure ,    DsmStateIssuingPortCycleOnFailueWithEndpointsToBeDeleted },
        { DsmEventPortDetachDeviceWithDeviceEnabled ,DsmStateDisablingDeviceAndDefaultEndpointOnDetachOnHubStart },
        { DsmEventOperationFailureWithDeviceEnabled ,DsmStateDisablingDeviceAndDefaultEndpointOnEnumFailureOnHubStart },
        { DsmEventPDOCleanup ,          DsmStateDeletingEndpointsAndDeviceOnOnCleanup },
        { DsmEventPortDetachDevice ,    DsmStateReportingDeviceMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReEnumeratingOnPdoPreStartWhileEnabled = {
    // State ETW Name
    DsmStateEtwReEnumeratingOnPdoPreStartWhileEnabled,
    // State Entry Function
    HUBDSM_ReEnumeratingOnPdoPreStartWhileEnabled,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmReEnumeratingOnPdoPreStartWhileEnabled,
    // Event State Pairs for Transitions
    {
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventOperationSuccess ,    DsmStateAckingPdoPreStart },
        { DsmEventOperationFailureWithDeviceEnabled ,DsmStateDisablingDeviceInControllerOnEnumFailureOnPdoPreStart },
        { DsmEventPortDetachDeviceWithDeviceEnabled ,DsmStateCompletingPdoReStartOnDetachWithDeviceEnabled },
        { DsmEventOperationFailure ,    DsmStateCompletingPdoReStartOnOperationFailure },
        { DsmEventPortDetachDevice ,    DsmStateCompletingPdoReStartOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReEnumeratingOnPdoStart = {
    // State ETW Name
    DsmStateEtwReEnumeratingOnPdoStart,
    // State Entry Function
    HUBDSM_ReEnumeratingOnPdoStart,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmReEnumeratingOnPdoStart,
    // Event State Pairs for Transitions
    {
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventOperationSuccess ,    DsmStateCleaningUpAnyExistingConfigurationOnStart },
        { DsmEventOperationFailure ,    DsmStateCompletingPnpEventOnFailureInDisabledWithPortOff },
        { DsmEventPortDetachDeviceWithDeviceEnabled ,DsmStateSignalingPnpPowerEventOnDetachDuringRenumOnRestart },
        { DsmEventOperationFailureWithDeviceEnabled ,DsmStateDisablingDeviceInControllerOnEnumFailureOnRestart },
        { DsmEventPortDetachDevice ,    DsmStateCompletingPnpEventOnDetachInDisabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReEnumeratingOnPdoStartWhileEnabled = {
    // State ETW Name
    DsmStateEtwReEnumeratingOnPdoStartWhileEnabled,
    // State Entry Function
    HUBDSM_ReEnumeratingOnPdoStartWhileEnabled,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmReEnumeratingOnPdoStartWhileEnabled,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventOperationSuccess ,    DsmStateCleaningUpAnyExistingConfigurationOnStart },
        { DsmEventOperationFailure ,    DsmStateCompletingPnpEventOnFailureInDisabledWithPortOff },
        { DsmEventPortDetachDeviceWithDeviceEnabled ,DsmStateSignalingPnpPowerEventOnDetachDuringRenumOnRestart },
        { DsmEventOperationFailureWithDeviceEnabled ,DsmStateDisablingDeviceInControllerOnEnumFailureOnRestart },
        { DsmEventPortDetachDevice ,    DsmStateCompletingPnpEventOnDetachInDisabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReEnumeratingOnResumeInConfigured = {
    // State ETW Name
    DsmStateEtwReEnumeratingOnResumeInConfigured,
    // State Entry Function
    HUBDSM_ReEnumeratingOnResumeInConfigured,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmReEnumeratingOnResumeInConfigured,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventOperationFailure ,    DsmStateCompletingPnpEventOnFailureWithEndpointsToBeDeleted },
        { DsmEventOperationSuccess ,    DsmStatePreparingEndpointAndInterfaceListsOnResetAfterResume },
        { DsmEventOperationFailureWithDeviceEnabled ,DsmStateIssuingPortCycleAndSingalingPnpEventOnEnumFailueFromConfigured },
        { DsmEventPortDetachDeviceWithDeviceEnabled ,DsmStateCompletingPnpEventOnDetachWhileRenumeratingOnResume },
        { DsmEventPortDetachDevice ,    DsmStateCompletingPnpEventOnDetachInDisabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReEnumeratingOnResumeInConfiguredWithSpecialFile = {
    // State ETW Name
    DsmStateEtwReEnumeratingOnResumeInConfiguredWithSpecialFile,
    // State Entry Function
    HUBDSM_ReEnumeratingOnResumeInConfiguredWithSpecialFile,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmReEnumeratingOnResumeInConfiguredWithSpecialFile,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventOperationFailure ,    DsmStateBugcheckingSystemOnBootDeviceEnumerationFailure },
        { DsmEventOperationSuccess ,    DsmStatePreparingEndpointAndInterfaceListsOnResetAfterResumeWithSpecialFile },
        { DsmEventOperationFailureWithDeviceEnabled ,DsmStateBugcheckingSystemOnBootDeviceEnumerationFailure },
        { DsmEventPortDetachDeviceWithDeviceEnabled ,DsmStateCompletingD0EntryInConfiguredWithSpecialFileOnDetachWithDeviceEnabled },
        { DsmEventPortDetachDevice ,    DsmStateCompletingD0EntryInConfiguredWithSpecialFileOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReEnumeratingOnResumeInUnConfigured = {
    // State ETW Name
    DsmStateEtwReEnumeratingOnResumeInUnConfigured,
    // State Entry Function
    HUBDSM_ReEnumeratingOnResumeInUnConfigured,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmReEnumeratingOnResumeInUnConfigured,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventOperationSuccess ,    DsmStateCompletingD0EntryInUnconfigured },
        { DsmEventOperationFailureWithDeviceEnabled ,DsmStateIssuingPortCycleAndSingalingPnpEventOnEnumFailueFromConfigured },
        { DsmEventOperationFailure ,    DsmStateCompletingPnpEventOnFailureInDisabledWithPortOff },
        { DsmEventPortDetachDeviceWithDeviceEnabled ,DsmStateCompletingPnpEventOnDetachWhileRenumeratingOnResume },
        { DsmEventPortDetachDevice ,    DsmStateCompletingPnpEventOnDetachInDisabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryRegisteringWithHSM = {
    // State ETW Name
    DsmStateEtwRegisteringWithHSM,
    // State Entry Function
    HUBDSM_RegisteringWithHSM,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateQueueingAttachFailureToPort },
        { DsmEventOperationSuccess ,    DsmStateQueueingAttachSuccessToPort },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingDevicePowerReferenceOnStopAfterDetach = {
    // State ETW Name
    DsmStateEtwReleasingDevicePowerReferenceOnStopAfterDetach,
    // State Entry Function
    HUBDSM_ReleasingDevicePowerReferenceOnStopAfterDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForRemoveOrReprotedMissingOnHubStopOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnDetachInConfigured = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnDetachInConfigured,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnDetachInConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateNotifyingDeviceDisconnectedOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnHubOnDetach = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnHubOnDetach,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnHubOnDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateIssuingDeviceDetachedToPSMOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnHubOnDetachOnEnumSuccess = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnHubOnDetachOnEnumSuccess,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnHubOnDetachOnEnumSuccess,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReportingDeviceToPnp },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnHubOnEnumerationFailure = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnHubOnEnumerationFailure,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnHubOnEnumerationFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReportingFailedUnknownDeviceToPnp },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnHubStopSuspend = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnHubStopSuspend,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnHubStopSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateStoppedWithHubSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnHubSuspendStopOnDetach = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnHubSuspendStopOnDetach,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnHubSuspendStopOnDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnPortFailureInHubStopSuspend = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnPortFailureInHubStopSuspend,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnPortFailureInHubStopSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnSuspendFailureInConfigured = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnSuspendFailureInConfigured,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnSuspendFailureInConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateRequestingPortCycleOnErrorFromConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnSuspendFailureInUnConfigured = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnSuspendFailureInUnConfigured,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnSuspendFailureInUnConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryRemovingDeviceInfoFromGlobalListOnCleanup = {
    // State ETW Name
    DsmStateEtwRemovingDeviceInfoFromGlobalListOnCleanup,
    // State Entry Function
    HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanup,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDeletingDeviceAndDefaultEndpointOnCleanupAfterDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryRemovingDeviceInfoFromGlobalListOnCleanupOnDetachInConfigured = {
    // State ETW Name
    DsmStateEtwRemovingDeviceInfoFromGlobalListOnCleanupOnDetachInConfigured,
    // State Entry Function
    HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanupOnDetachInConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDisablingEndpointsInControllerOnDetachOnPDOCleanupWithPDOMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryRemovingDeviceInfoFromGlobalListOnCleanupOnDetachInUnConfigured = {
    // State ETW Name
    DsmStateEtwRemovingDeviceInfoFromGlobalListOnCleanupOnDetachInUnConfigured,
    // State Entry Function
    HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanupOnDetachInUnConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDisablingDeviceAndDefaultEpInControllerOnDetachOnPDOCleaupWithPDOMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryRemovingDeviceInfoFromGlobalListOnCleanUpOnHubStopOnDetach = {
    // State ETW Name
    DsmStateEtwRemovingDeviceInfoFromGlobalListOnCleanUpOnHubStopOnDetach,
    // State Entry Function
    HUBDSM_RemovingDeviceInfoFromGlobalListOnCleanUpOnHubStopOnDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateUnregsiteringWithHSMOnDetachOnPDOCleaupWithPDOMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryRemovingDeviceInfoFromGlobalListOnDeviceReportedMissing = {
    // State ETW Name
    DsmStateEtwRemovingDeviceInfoFromGlobalListOnDeviceReportedMissing,
    // State Entry Function
    HUBDSM_RemovingDeviceInfoFromGlobalListOnDeviceReportedMissing,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForRemoveOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryRemovingDeviceInfoFromGlobalListOnReportedMissingOnHubStopOnDetach = {
    // State ETW Name
    DsmStateEtwRemovingDeviceInfoFromGlobalListOnReportedMissingOnHubStopOnDetach,
    // State Entry Function
    HUBDSM_RemovingDeviceInfoFromGlobalListOnReportedMissingOnHubStopOnDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateUnregisteringWithHsmOnDetachOnDeviceReportedMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReportingDeviceMissing = {
    // State ETW Name
    DsmStateEtwReportingDeviceMissing,
    // State Entry Function
    HUBDSM_ReportingDeviceMissing,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateUnregsiteringWithHsmOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReportingDeviceToPnp = {
    // State ETW Name
    DsmStateEtwReportingDeviceToPnp,
    // State Entry Function
    HUBDSM_ReportingDeviceToPnp,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmReportingDeviceToPnp,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventOperationSuccess ,    DsmStateStoppedEnumerated },
        { DsmEventPortDetachDevice ,    DsmStateDisablingDeviceAndDefaultEndpointInControllerOnDetachDuringEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReportingFailedDeviceAsMissing = {
    // State ETW Name
    DsmStateEtwReportingFailedDeviceAsMissing,
    // State Entry Function
    HUBDSM_ReportingFailedDeviceAsMissing,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmReportingFailedDeviceAsMissing,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOReportedMissing ,  DsmStateIgnored },
        { DsmEventPDOCleanup ,          DsmStateCompletingPDOCleanupForUnknownDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReportingFailedUnknownDeviceToPnp = {
    // State ETW Name
    DsmStateEtwReportingFailedUnknownDeviceToPnp,
    // State Entry Function
    HUBDSM_ReportingFailedUnknownDeviceToPnp,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmReportingFailedUnknownDeviceToPnp,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventOperationSuccess ,    DsmStateWaitingForD0EntryForFailedDevice },
        { DsmEventPortDetachDevice ,    DsmStateIssuingDeviceDetachedToPSMOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryRequestingCyclePortAndCompletingPnpEventOnFailureInUnconfigured = {
    // State ETW Name
    DsmStateEtwRequestingCyclePortAndCompletingPnpEventOnFailureInUnconfigured,
    // State Entry Function
    HUBDSM_RequestingCyclePortAndCompletingPnpEventOnFailureInUnconfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDisablingPortOnFailureInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryRequestingPortCycleOnErrorFromConfigured = {
    // State ETW Name
    DsmStateEtwRequestingPortCycleOnErrorFromConfigured,
    // State Entry Function
    HUBDSM_RequestingPortCycleOnErrorFromConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachOrRemoveInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryResettingPipeInUCXOnClientReset = {
    // State ETW Name
    DsmStateEtwResettingPipeInUCXOnClientReset,
    // State Entry Function
    HUBDSM_ResettingPipeInUCXOnClientReset,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlFailure ,     DsmStateCompletingClientRequestInConfiguredWithLastStatus },
        { DsmEventUCXIoctlSuccess ,     DsmStateCompletingClientRequestInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryResettingPipeInUCXOnClientSyncResetPipe = {
    // State ETW Name
    DsmStateEtwResettingPipeInUCXOnClientSyncResetPipe,
    // State Entry Function
    HUBDSM_ResettingPipeInUCXOnClientSyncResetPipe,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlFailure ,     DsmStateCompletingClientRequestInConfiguredWithLastStatus },
        { DsmEventUCXIoctlSuccess ,     DsmStateCompletingClientRequestInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryResumingOnHubResumeInStopped = {
    // State ETW Name
    DsmStateEtwResumingOnHubResumeInStopped,
    // State Entry Function
    HUBDSM_ResumingOnHubResumeInStopped,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResumed ,         DsmStateStoppedEnumerated },
        { DsmEventPortResumeTimedOut ,  DsmStateReEnumeratingOnHubStart },
        { DsmEventHubStop ,             DsmStateWaitingForPortResumeResponseOnStop },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventPortFailed ,          DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventPortStateDisabled ,   DsmStateStoppedEnumerated },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateStoppedEnumerated },
        { DsmEventHubSuspend ,          DsmStateWaitingForPortResumeResponseOnSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingConfigurationInformationInClientRequest = {
    // State ETW Name
    DsmStateEtwSettingConfigurationInformationInClientRequest,
    // State Entry Function
    HUBDSM_SettingConfigurationInformationInClientRequest,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCompletingClientRequestInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingDeviceInterface = {
    // State ETW Name
    DsmStateEtwSettingDeviceInterface,
    // State Entry Function
    HUBDSM_SettingDeviceInterface,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmSettingDeviceInterface,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateCompletingClientRequestOnFailureInConfigured },
        { DsmEventOperationSuccess ,    DsmStateSettingInterfaceInformationInClientRequest },
        { DsmEventPortDetachDevice ,    DsmStateCompletingClientRequestOnDetachInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingExitLatencyAdjustmentOnNoPingResponseError = {
    // State ETW Name
    DsmStateEtwSettingExitLatencyAdjustmentOnNoPingResponseError,
    // State Entry Function
    HUBDSM_SettingExitLatencyAdjustmentOnNoPingResponseError,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateSettingLPMValuesInConfiguredOnSettingChange },
        { DsmEventOperationFailure ,    DsmStateRequestingPortCycleOnErrorFromConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingFailIoFlagOnClientReset = {
    // State ETW Name
    DsmStateEtwSettingFailIoFlagOnClientReset,
    // State Entry Function
    HUBDSM_SettingFailIoFlagOnClientReset,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReEnumeratingOnClientRequestInConfiguredWithSpecialFile },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingForceResetOnRenumFlag = {
    // State ETW Name
    DsmStateEtwSettingForceResetOnRenumFlag,
    // State Entry Function
    HUBDSM_SettingForceResetOnRenumFlag,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateStoppedEnabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingInterfaceInformationInClientRequest = {
    // State ETW Name
    DsmStateEtwSettingInterfaceInformationInClientRequest,
    // State Entry Function
    HUBDSM_SettingInterfaceInformationInClientRequest,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCompletingClientRequestInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingLPMValuesInConfiguredOnSettingChange = {
    // State ETW Name
    DsmStateEtwSettingLPMValuesInConfiguredOnSettingChange,
    // State Entry Function
    HUBDSM_SettingLPMValuesInConfiguredOnSettingChange,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmSettingLPMValuesInConfiguredOnSettingChange,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguredInD0 },
        { DsmEventContinueIfNoSubState ,DsmStateConfiguredInD0 },
        { DsmEventOperationFailure ,    DsmStateRequestingPortCycleOnErrorFromConfigured },
        { DsmEventPortDetachDevice ,    DsmStatePurgingDeviceIoOnDetachInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingLPMValuesInStoppedEnabled = {
    // State ETW Name
    DsmStateEtwSettingLPMValuesInStoppedEnabled,
    // State Entry Function
    HUBDSM_SettingLPMValuesInStoppedEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmSettingLPMValuesInStoppedEnabled,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateStoppedEnabled },
        { DsmEventOperationSuccess ,    DsmStateStoppedEnabled },
        { DsmEventContinueIfNoSubState ,DsmStateStoppedEnabled },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingLPMValuesInUnConfiguredOnSettingChange = {
    // State ETW Name
    DsmStateEtwSettingLPMValuesInUnConfiguredOnSettingChange,
    // State Entry Function
    HUBDSM_SettingLPMValuesInUnConfiguredOnSettingChange,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmSettingLPMValuesInUnConfiguredOnSettingChange,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateUnConfiguredInD0 },
        { DsmEventContinueIfNoSubState ,DsmStateUnConfiguredInD0 },
        { DsmEventOperationSuccess ,    DsmStateUnConfiguredInD0 },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySignalingPnpPowerEventOnDetachDuringRenumOnRestart = {
    // State ETW Name
    DsmStateEtwSignalingPnpPowerEventOnDetachDuringRenumOnRestart,
    // State Entry Function
    HUBDSM_SignalingPnpPowerEventOnDetachDuringRenumOnRestart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySignalingPnpPowerEventOnStop = {
    // State ETW Name
    DsmStateEtwSignalingPnpPowerEventOnStop,
    // State Entry Function
    HUBDSM_SignalingPnpPowerEventOnStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateStopped },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySignalingPnpPowerEventOnStopEnable = {
    // State ETW Name
    DsmStateEtwSignalingPnpPowerEventOnStopEnable,
    // State Entry Function
    HUBDSM_SignalingPnpPowerEventOnStopEnable,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateStoppedEnabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySignallingPnpEventOnHubSuspendFromUnconfiguredSuspending = {
    // State ETW Name
    DsmStateEtwSignallingPnpEventOnHubSuspendFromUnconfiguredSuspending,
    // State Entry Function
    HUBDSM_SignallingPnpEventOnHubSuspendFromUnconfiguredSuspending,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateUnConfiguredSuspendedOnHubSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStartingDeviceIoOnHubResume = {
    // State ETW Name
    DsmStateEtwStartingDeviceIoOnHubResume,
    // State Entry Function
    HUBDSM_StartingDeviceIoOnHubResume,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateConfiguredInD0 },
        { DsmEventUCXIoctlFailure ,     DsmStateConfiguredInD0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStopped = {
    // State ETW Name
    DsmStateEtwStopped,
    // State Entry Function
    HUBDSM_Stopped,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventClientRequestReset ,  DsmStateCompleteWithStatusFailed },
        { DsmEventQueryDeviceText ,     DsmStateSignalQueryDeviceTextEvent },
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSetInterface ,DsmStateCompleteWithStatusFailed },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventClientRequestCycle ,  DsmStateRequestPortCycle },
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSelectConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventPDOD0Entry ,          DsmStateReEnumeratingOnPdoStart },
        { DsmEventHubStop ,             DsmStateReleasingPowerReferenceOnHubStopSuspend },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInStopped },
        { DsmEventPortDetachDevice ,    DsmStateReportingDeviceMissing },
        { DsmEventPDOPreStart ,         DsmStateReEnumeratingOnPdoPreStartWhileEnabled },
        { DsmEventPDOCleanup ,          DsmStateDeletingEndpointsAndDeviceOnOnCleanup },
        { DsmEventHubSuspend ,          DsmStateReleasingPowerReferenceOnHubStopSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppedEnabled = {
    // State ETW Name
    DsmStateEtwStoppedEnabled,
    // State Entry Function
    HUBDSM_StoppedEnabled,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmStoppedEnabled,
    // Event State Pairs for Transitions
    {
        { DsmEventClientRequestSelectConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestCycle ,  DsmStateRequestPortCycle },
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventQueryDeviceText ,     DsmStateSignalQueryDeviceTextEvent },
        { DsmEventClientRequestReset ,  DsmStateCompleteWithStatusFailed },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSetInterface ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInStopEnabled },
        { DsmEventPDOD0Entry ,          DsmStateReEnumeratingOnPdoStartWhileEnabled },
        { DsmEventPDOPreStart ,         DsmStateAckingPdoPreStartFromStoppedEnabled },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventPDOCleanup ,          DsmStateDisablingPortOnPdoCleanup },
        { DsmEventLPMPowerSettingChange ,DsmStateSettingLPMValuesInStoppedEnabled },
        { DsmEventHubStop ,             DsmStateDisablingDeviceInControllerOnHubStopFromStoppedEnabled },
        { DsmEventHubSuspend ,          DsmStateDisablingDeviceInControllerOnHubStopFromStoppedEnabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppedEnumerated = {
    // State ETW Name
    DsmStateEtwStoppedEnumerated,
    // State Entry Function
    HUBDSM_StoppedEnumerated,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagStopTimerOnExit,
    // Sub State Machine Information
    DsmSubSmStoppedEnumerated,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventTimerFired ,          DsmStateCheckIfDeviceCanBeDisabledOnDriverNotFound },
        { DsmEventHubStop ,             DsmStateDisablingPortOnHubStop },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateDeConfiguringDeviceOnClientRequestFromUnconfigured },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateDeConfiguringDeviceOnClientRequestFromUnconfigured },
        { DsmEventClientRequestReset ,  DsmStateReEnumeratingOnClientRequestInUnconfigured },
        { DsmEventClientRequestSelectConfiguration ,DsmStateValidatingSelectConfigUrbFromUnConfigured },
        { DsmEventPDOPreStart ,         DsmStateAckingPdoPreStart },
        { DsmEventPDOCleanup ,          DsmStateDisablingPortOnPdoCleanup },
        { DsmEventHubSuspend ,          DsmStateSuspendingOnHubSuspendInStopped },
        { DsmEventPDOD0Entry ,          DsmStateCompletingD0EntryInUnconfigured },
        { DsmEventQueryDeviceText ,     DsmStateQueryingDeviceTextStringFromStoppedEnumerated },
        { DsmEventFDORequestGetDescriptor ,DsmStateQueryingDescriptorForFdoRequestFromStoppedEnumerated },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppedEnumeratedConfiguredForFaultyClients = {
    // State ETW Name
    DsmStateEtwStoppedEnumeratedConfiguredForFaultyClients,
    // State Entry Function
    HUBDSM_StoppedEnumeratedConfiguredForFaultyClients,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmStoppedEnumeratedConfiguredForFaultyClients,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventPDOPreStart ,         DsmStateCompletePdoPreStart },
        { DsmEventPDOD0Entry ,          DsmStateCompletingD0EntryInConfigured },
        { DsmEventPDOCleanup ,          DsmStateDeporgrammingAllEndpointsOnCleanupFromEnumeratedConfigured },
        { DsmEventPortDetachDevice ,    DsmStateNotifyingDeviceDisconnectedOnDetach },
        { DsmEventHubStop ,             DsmStateDeporgrammingAllEndpointsOnHubStopFromEnumeratedConfigured },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateDeConfiguringDeviceOnClientRequest },
        { DsmEventClientRequestReset ,  DsmStateCheckingIfBootDeviceOnReset },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInConfigured },
        { DsmEventClientRequestSelectConfiguration ,DsmStateValidatingSelectConfigUrbFromConfigured },
        { DsmEventClientRequestSetInterface ,DsmStateValidatingSelectInterfaceUrbFromConfigured },
        { DsmEventClientRequestSyncClearStall ,DsmStateGettingMatchingPipeHandleOnClientClearStall },
        { DsmEventQueryDeviceText ,     DsmStateQueryingDeviceTextStringFromConfigured },
        { DsmEventClientRequestSyncResetPipe ,DsmStateGettingMatchingPipeHandleOnClientSyncResetPipe },
        { DsmEventFDORequestGetDescriptor ,DsmStateQueryingDescriptorForFdoRequestFromConfigured },
        { DsmEventClientRequestResetPipe ,DsmStateGettingMatchingPipeHandleOnClientResetPipe },
        { DsmEventHubSuspend ,          DsmStateDeprogrammingAllEndpointsOnHubSuspendFromEnumeratedConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppedEnumeratedWithHubSuspended = {
    // State ETW Name
    DsmStateEtwStoppedEnumeratedWithHubSuspended,
    // State Entry Function
    HUBDSM_StoppedEnumeratedWithHubSuspended,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmStoppedEnumeratedWithHubSuspended,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventPDOPreStart ,         DsmStateCompletePdoPreStart },
        { DsmEventPDOInstallMSOSExt ,   DsmStateSignalPnpPowerEvent },
        { DsmEventHubStopWithReferenceAcquired ,DsmStateDisablingDeviceInControllerOnHubStopInStopped },
        { DsmEventDeviceResumedOnHubResume ,DsmStateStoppedEnumerated },
        { DsmEventDeviceSuspendedOnHubResume ,DsmStateResumingOnHubResumeInStopped },
        { DsmEventDeviceRenumerateOnHubResume ,DsmStateReEnumeratingOnHubStart },
        { DsmEventHubStop ,             DsmStateDisablingDeviceInControllerOnHubStopInSuspended },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventPDOCleanup ,          DsmStateDisablingDeviceInControllerOnCleanup },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppedWithHubSuspended = {
    // State ETW Name
    DsmStateEtwStoppedWithHubSuspended,
    // State Entry Function
    HUBDSM_StoppedWithHubSuspended,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmStoppedWithHubSuspended,
    // Event State Pairs for Transitions
    {
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventPDOPreStart ,         DsmStateCompletePdoPreStart },
        { DsmEventClientRequestSetInterface ,DsmStateCompleteWithStatusFailed },
        { DsmEventQueryDeviceText ,     DsmStateSignalQueryDeviceTextEvent },
        { DsmEventClientRequestCycle ,  DsmStateRequestPortCycle },
        { DsmEventClientRequestReset ,  DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSelectConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventPDOInstallMSOSExt ,   DsmStateSignalPnpPowerEvent },
        { DsmEventDeviceResumedOnHubResume ,DsmStateReDisablingPortAfterHubResume },
        { DsmEventHubStopWithReferenceAcquired ,DsmStateReleasingPowerReferenceOnHubStopSuspend },
        { DsmEventDeviceRenumerateOnHubResume ,DsmStateReDisablingPortAfterHubResume },
        { DsmEventDeviceSuspendedOnHubResume ,DsmStateReDisablingPortAfterHubResume },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInStoppedWithHubSuspended },
        { DsmEventPortDetachDevice ,    DsmStateReportingDeviceMissing },
        { DsmEventPDOCleanup ,          DsmStateDeletingEndpointsAndDeviceOnOnCleanup },
        { DsmEventHubStopAfterSuspend , DsmStateAckingHubStopAfterSuspendFromStoppedState },
        { DsmEventHubStart ,            DsmStateAcquiringPowerReferenceOnHubStart },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySuspendingOnHubSuspendInStopped = {
    // State ETW Name
    DsmStateEtwSuspendingOnHubSuspendInStopped,
    // State Entry Function
    HUBDSM_SuspendingOnHubSuspendInStopped,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortSuspended ,       DsmStateStoppedEnumeratedWithHubSuspended },
        { DsmEventPortFailed ,          DsmStateReleasingPowerReferenceOnPortFailureInHubStopSuspend },
        { DsmEventPortDetachDevice ,    DsmStateReleasingPowerReferenceOnHubSuspendStopOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUnConfiguredInD0 = {
    // State ETW Name
    DsmStateEtwUnConfiguredInD0,
    // State Entry Function
    HUBDSM_UnConfiguredInD0,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventQueryDeviceText ,     DsmStateSignalQueryDeviceTextEvent },
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestCycle ,  DsmStateRequestPortCycle },
        { DsmEventClientRequestSetInterface ,DsmStateCompleteWithStatusFailed },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventHubSuspend ,          DsmStateUnConfiguredSuspendingOnHubSuspend },
        { DsmEventQueryDeviceText ,     DsmStateQueryingDeviceTextStringFromUnConfigured },
        { DsmEventFDORequestGetDescriptor ,DsmStateQueryingDescriptorForFdoRequestFromUnConfigured },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateDeConfiguringDeviceOnClientRequestFromUnconfigured },
        { DsmEventClientRequestReset ,  DsmStateReEnumeratingOnClientRequestInUnconfigured },
        { DsmEventClientRequestSelectConfiguration ,DsmStateValidatingSelectConfigUrbFromUnConfigured },
        { DsmEventPDOD0Exit ,           DsmStateUnconfiguredSuspending },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventLPMPowerSettingChange ,DsmStateSettingLPMValuesInUnConfiguredOnSettingChange },
        { DsmEventPDOD0ExitFinal ,      DsmStateCheckingIfDeviceShouldBeDisabled },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInUnconfiguredInD0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUnConfiguredResumingOnHubResume = {
    // State ETW Name
    DsmStateEtwUnConfiguredResumingOnHubResume,
    // State Entry Function
    HUBDSM_UnConfiguredResumingOnHubResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResumed ,         DsmStateUnConfiguredInD0 },
        { DsmEventPortFailed ,          DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventPortResumeTimedOut ,  DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventPortStateDisabled ,   DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUnconfiguredSuspended = {
    // State ETW Name
    DsmStateEtwUnconfiguredSuspended,
    // State Entry Function
    HUBDSM_UnconfiguredSuspended,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmUnconfiguredSuspended,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventClientRequestSetInterface ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventQueryDeviceText ,     DsmStateSignalQueryDeviceTextEvent },
        { DsmEventClientRequestCycle ,  DsmStateRequestPortCycle },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSelectConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestReset ,  DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventDeviceRenumerateOnHubResume ,DsmStateReEnumeratingOnResumeInUnConfigured },
        { DsmEventDeviceResumed ,       DsmStateCompletingD0EntryInUnconfigured },
        { DsmEventPDOCleanup ,          DsmStateDisablingDeviceAndDefaultEpInControllerOnCleanupWithPortOff },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventHubStop ,             DsmStateDisablingDeviceAndDefaultEpInControllerOFailureWithPortOff },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInUnconfiguredSuspended },
        { DsmEventHubStopAfterSuspend , DsmStateDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspend },
        { DsmEventPortFailed ,          DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventPDOPreStart ,         DsmStateDisablingDeviceInControllerOnRenumerationWhileSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUnConfiguredSuspendedOnHubSuspend = {
    // State ETW Name
    DsmStateEtwUnConfiguredSuspendedOnHubSuspend,
    // State Entry Function
    HUBDSM_UnConfiguredSuspendedOnHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmUnConfiguredSuspendedOnHubSuspend,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceRenumerateOnHubResume ,DsmStateReEnumeratingOnHubResumeInUnConfigured },
        { DsmEventDeviceResumedOnHubResume ,DsmStateUnConfiguredInD0 },
        { DsmEventDeviceSuspendedOnHubResume ,DsmStateUnConfiguredResumingOnHubResume },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUnconfiguredSuspending = {
    // State ETW Name
    DsmStateEtwUnconfiguredSuspending,
    // State Entry Function
    HUBDSM_UnconfiguredSuspending,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmUnconfiguredSuspending,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateUnconfiguredSuspended },
        { DsmEventOperationFailure ,    DsmStateUnconfiguredSuspended },
        { DsmEventHubSuspend ,          DsmStateSignallingPnpEventOnHubSuspendFromUnconfiguredSuspending },
        { DsmEventPortDetachDevice ,    DsmStateCompletingPnpEventOnDetachInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUnConfiguredSuspendingOnHubSuspend = {
    // State ETW Name
    DsmStateEtwUnConfiguredSuspendingOnHubSuspend,
    // State Entry Function
    HUBDSM_UnConfiguredSuspendingOnHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortFailed ,          DsmStateReleasingPowerReferenceOnSuspendFailureInUnConfigured },
        { DsmEventPortSuspended ,       DsmStateUnConfiguredSuspendedOnHubSuspend },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUnregisteringWithHSMOnDetachAfterCleanup = {
    // State ETW Name
    DsmStateEtwUnregisteringWithHSMOnDetachAfterCleanup,
    // State Entry Function
    HUBDSM_UnregisteringWithHSMOnDetachAfterCleanup,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubResume ,           DsmStateIgnored },
        { DsmEventHubStart ,            DsmStateIgnored },
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventHubResumeWithReset ,  DsmStateIgnored },
        { DsmEventHubStopAfterSuspend , DsmStateIgnored },
        { DsmEventHubResumeInS0 ,       DsmStateIgnored },
        { DsmEventOperationSuccess ,    DsmStateWaitingToBeDeleted },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUnregisteringWithHsmOnDetachOnDeviceReportedMissing = {
    // State ETW Name
    DsmStateEtwUnregisteringWithHsmOnDetachOnDeviceReportedMissing,
    // State Entry Function
    HUBDSM_UnregisteringWithHsmOnDetachOnDeviceReportedMissing,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubResume ,           DsmStateIgnored },
        { DsmEventHubResumeWithReset ,  DsmStateIgnored },
        { DsmEventHubStart ,            DsmStateIgnored },
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventHubStopAfterSuspend , DsmStateIgnored },
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventHubResumeInS0 ,       DsmStateIgnored },
        { DsmEventOperationSuccess ,    DsmStateWaitingForRemoveOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUnregsiteringWithHsmOnDetach = {
    // State ETW Name
    DsmStateEtwUnregsiteringWithHsmOnDetach,
    // State Entry Function
    HUBDSM_UnregsiteringWithHsmOnDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubResume ,           DsmStateIgnored },
        { DsmEventHubResumeWithReset ,  DsmStateIgnored },
        { DsmEventHubStart ,            DsmStateIgnored },
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventHubStopAfterSuspend , DsmStateIgnored },
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventHubResumeInS0 ,       DsmStateIgnored },
        { DsmEventOperationSuccess ,    DsmStateWaitingForPDOReportedMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUnregsiteringWithHSMOnDetachOnPDOCleaupWithPDOMissing = {
    // State ETW Name
    DsmStateEtwUnregsiteringWithHSMOnDetachOnPDOCleaupWithPDOMissing,
    // State Entry Function
    HUBDSM_UnregsiteringWithHSMOnDetachOnPDOCleaupWithPDOMissing,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubResumeWithReset ,  DsmStateIgnored },
        { DsmEventHubStart ,            DsmStateIgnored },
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventHubStopAfterSuspend , DsmStateIgnored },
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventHubResume ,           DsmStateIgnored },
        { DsmEventHubResumeInS0 ,       DsmStateIgnored },
        { DsmEventOperationSuccess ,    DsmStateDeletingDeviceAndDefaultEndpointOnCleanupAfterDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingSelectConfigUrbFromConfigured = {
    // State ETW Name
    DsmStateEtwValidatingSelectConfigUrbFromConfigured,
    // State Entry Function
    HUBDSM_ValidatingSelectConfigUrbFromConfigured,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateCompletingClientRequestInConfiguredWithLastStatus },
        { DsmEventOperationSuccess ,    DsmStatePreparingEndpointAndInterfaceListsOnSelectConfigurationFromConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingSelectConfigUrbFromUnConfigured = {
    // State ETW Name
    DsmStateEtwValidatingSelectConfigUrbFromUnConfigured,
    // State Entry Function
    HUBDSM_ValidatingSelectConfigUrbFromUnConfigured,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStatePreparingEndpointAndInterfaceListsOnSelectConfiguration },
        { DsmEventOperationFailure ,    DsmStateCompletingClientRequestInUnConfiguredWithLastStatus },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingSelectInterfaceUrbFromConfigured = {
    // State ETW Name
    DsmStateEtwValidatingSelectInterfaceUrbFromConfigured,
    // State Entry Function
    HUBDSM_ValidatingSelectInterfaceUrbFromConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateSettingDeviceInterface },
        { DsmEventOperationFailure ,    DsmStateCompletingClientRequestOnFailureInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForD0EntryForFailedDevice = {
    // State ETW Name
    DsmStateEtwWaitingForD0EntryForFailedDevice,
    // State Entry Function
    HUBDSM_WaitingForD0EntryForFailedDevice,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForD0EntryForFailedDevice,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOCleanup ,          DsmStateCompletingDeviceCleanup },
        { DsmEventPDOD0Entry ,          DsmStateD0EntryForUnknownDevice },
        { DsmEventPortDetachDevice ,    DsmStateReportingFailedDeviceAsMissing },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateDisablingBackPortInWaitingForD0EntryForFailedDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDetachOrRemove = {
    // State ETW Name
    DsmStateEtwWaitingForDetachOrRemove,
    // State Entry Function
    HUBDSM_WaitingForDetachOrRemove,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForDetachOrRemove,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDisabledOnHubSuspend ,DsmStateWaitingForDetachOrRemoveWithPortOff },
        { DsmEventPDOCleanup ,          DsmStateDeletingDeviceAndDefaultEndpointOnCleanup },
        { DsmEventPortDetachDevice ,    DsmStateReportingDeviceMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDetachOrRemoveInConfigured = {
    // State ETW Name
    DsmStateEtwWaitingForDetachOrRemoveInConfigured,
    // State Entry Function
    HUBDSM_WaitingForDetachOrRemoveInConfigured,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForDetachOrRemoveInConfigured,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOCleanup ,          DsmStateDisablingEndpointsInControllerOnCleanupWithPortOff },
        { DsmEventHubStop ,             DsmStateDisablingEndpointsInControllerOnFailureWithPortOff },
        { DsmEventPortDetachDevice ,    DsmStateNotifyingDeviceDisconnectedOnDetach },
        { DsmEventHubStopAfterSuspend , DsmStateDisablingEndpointsInControllerOnHubStopAfterSuspend },
        { DsmEventPDOD0ExitFinal ,      DsmStateFlushingPD0D0ExitFinalInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDetachOrRemoveInDisabledWithPortOff = {
    // State ETW Name
    DsmStateEtwWaitingForDetachOrRemoveInDisabledWithPortOff,
    // State Entry Function
    HUBDSM_WaitingForDetachOrRemoveInDisabledWithPortOff,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForDetachOrRemoveInDisabledWithPortOff,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOCleanup ,          DsmStateDeletingDeviceAndDefaultEndpointOnCleanupWithPortOff },
        { DsmEventPortDetachDevice ,    DsmStateReportingDeviceMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDetachOrRemoveInUnConfigured = {
    // State ETW Name
    DsmStateEtwWaitingForDetachOrRemoveInUnConfigured,
    // State Entry Function
    HUBDSM_WaitingForDetachOrRemoveInUnConfigured,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForDetachOrRemoveInUnConfigured,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOCleanup ,          DsmStateDisablingDeviceAndDefaultEpInControllerOnCleanupWithPortOff },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventHubStop ,             DsmStateDisablingDeviceAndDefaultEpInControllerOFailureWithPortOff },
        { DsmEventPDOD0ExitFinal ,      DsmStateFlushingPD0D0ExitFinalInUnConfigured },
        { DsmEventHubStopAfterSuspend , DsmStateDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDetachOrRemoveWithPortOff = {
    // State ETW Name
    DsmStateEtwWaitingForDetachOrRemoveWithPortOff,
    // State Entry Function
    HUBDSM_WaitingForDetachOrRemoveWithPortOff,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForDetachOrRemoveWithPortOff,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReportingDeviceMissing },
        { DsmEventPDOCleanup ,          DsmStateDeletingDeviceAndDefaultEndpointOnCleanupWithPortOff },
        { DsmEventPortResumed ,         DsmStateWaitingForDetachOrRemove },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateWaitingForDetachOrRemove },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDetachOrReset = {
    // State ETW Name
    DsmStateEtwWaitingForDetachOrReset,
    // State Entry Function
    HUBDSM_WaitingForDetachOrReset,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForDetachOrReset,
    // Event State Pairs for Transitions
    {
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSetInterface ,DsmStateCompleteWithStatusFailed },
        { DsmEventQueryDeviceText ,     DsmStateSignalQueryDeviceTextEvent },
        { DsmEventClientRequestSelectConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventPDOPreStart ,         DsmStateCompletePdoPreStart },
        { DsmEventClientRequestReset ,  DsmStateReEnumeratingOnClientRequestOnReattach },
        { DsmEventPortDetachDevice ,    DsmStateQueueingDeviceDetachedToPsmForBootDeviceInUnconfigured },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInWaitingForDetatchorReset },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDetachOrResetWithDeviceDeporgrammed = {
    // State ETW Name
    DsmStateEtwWaitingForDetachOrResetWithDeviceDeporgrammed,
    // State Entry Function
    HUBDSM_WaitingForDetachOrResetWithDeviceDeporgrammed,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForDetachOrResetWithDeviceDeporgrammed,
    // Event State Pairs for Transitions
    {
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventQueryDeviceText ,     DsmStateSignalQueryDeviceTextEvent },
        { DsmEventClientRequestSetInterface ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventClientRequestSelectConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventPDOPreStart ,         DsmStateCompletePdoPreStart },
        { DsmEventClientRequestReset ,  DsmStateReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed },
        { DsmEventPortDetachDevice ,    DsmStateDeviceDetachedAndDeporgrammedWithSpecialFile },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInWaitingForDetachorResetWithDeviceDeprogrammed },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDetachOrToBeDeleted = {
    // State ETW Name
    DsmStateEtwWaitingForDetachOrToBeDeleted,
    // State Entry Function
    HUBDSM_WaitingForDetachOrToBeDeleted,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForDetachOrToBeDeleted,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDisabledOnHubSuspend ,DsmStateWaitingForDetachOrToBeDeletedWithPortOff },
        { DsmEventPortDetachDevice ,    DsmStateIssuingDeviceDetachedToPSMOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDetachOrToBeDeletedWithPortOff = {
    // State ETW Name
    DsmStateEtwWaitingForDetachOrToBeDeletedWithPortOff,
    // State Entry Function
    HUBDSM_WaitingForDetachOrToBeDeletedWithPortOff,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForDetachOrToBeDeletedWithPortOff,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateIssuingDeviceDetachedToPSMOnDetach },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateWaitingForDetachOrToBeDeleted },
        { DsmEventPortResumed ,         DsmStateWaitingForDetachOrToBeDeleted },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDisableCompleteOnHubStopFromStoppedEnabled = {
    // State ETW Name
    DsmStateEtwWaitingForDisableCompleteOnHubStopFromStoppedEnabled,
    // State Entry Function
    HUBDSM_WaitingForDisableCompleteOnHubStopFromStoppedEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReportingDeviceMissing },
        { DsmEventPortFailed ,          DsmStateReleasingPowerReferenceOnHubStopSuspend },
        { DsmEventPortDisabled ,        DsmStateReleasingPowerReferenceOnHubStopSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPDOD0ExitOnDetachInConfigured = {
    // State ETW Name
    DsmStateEtwWaitingForPDOD0ExitOnDetachInConfigured,
    // State Entry Function
    HUBDSM_WaitingForPDOD0ExitOnDetachInConfigured,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForPDOD0ExitOnDetachInConfigured,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOD0ExitFinal ,      DsmStateDisablingEndpointsInControllerOnD0ExitOnDetachWithPDOMissing },
        { DsmEventHubStopAfterSuspend , DsmStateDisablingEndpointsInControllerOnDetachOnHubStopWithPDOMissing },
        { DsmEventPDOCleanup ,          DsmStateDisablingEndpointsInControllerOnDetachOnPDOCleanupWithPDOMissing },
        { DsmEventHubStop ,             DsmStateDisablingEndpointsInControllerOnDetachOnHubStopWithPDOMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPDOD0ExitOnDetachInUnConfigured = {
    // State ETW Name
    DsmStateEtwWaitingForPDOD0ExitOnDetachInUnConfigured,
    // State Entry Function
    HUBDSM_WaitingForPDOD0ExitOnDetachInUnConfigured,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForPDOD0ExitOnDetachInUnConfigured,
    // Event State Pairs for Transitions
    {
        { DsmEventHubStop ,             DsmStateDisablingDeviceAndDefaultEpInControllerOnDetachOnHubStopWithPDOMissing },
        { DsmEventPDOCleanup ,          DsmStateDisablingDeviceAndDefaultEpInControllerOnDetachOnPDOCleaupWithPDOMissing },
        { DsmEventHubStopAfterSuspend , DsmStateDisablingDeviceAndDefaultEpInControllerOnDetachOnHubStopWithPDOMissing },
        { DsmEventPDOD0ExitFinal ,      DsmStateDisablingDeviceAndDefaultEpInControllerOnD0ExitOnDetachWithPDOMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPDOReportedMissing = {
    // State ETW Name
    DsmStateEtwWaitingForPDOReportedMissing,
    // State Entry Function
    HUBDSM_WaitingForPDOReportedMissing,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForPDOReportedMissing,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOCleanup ,          DsmStateRemovingDeviceInfoFromGlobalListOnCleanup },
        { DsmEventPDOReportedMissing ,  DsmStateRemovingDeviceInfoFromGlobalListOnDeviceReportedMissing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPortAttach = {
    // State ETW Name
    DsmStateEtwWaitingForPortAttach,
    // State Entry Function
    HUBDSM_WaitingForPortAttach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortAttachDevice ,    DsmStateRegisteringWithHSM },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPortResumeResponseOnStop = {
    // State ETW Name
    DsmStateEtwWaitingForPortResumeResponseOnStop,
    // State Entry Function
    HUBDSM_WaitingForPortResumeResponseOnStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResumed ,         DsmStateDisablingPortOnHubStop },
        { DsmEventPortFailed ,          DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventPortStateDisabled ,   DsmStateDisablingPortOnHubStop },
        { DsmEventPortResumeTimedOut ,  DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateDisablingPortOnHubStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPortResumeResponseOnSuspend = {
    // State ETW Name
    DsmStateEtwWaitingForPortResumeResponseOnSuspend,
    // State Entry Function
    HUBDSM_WaitingForPortResumeResponseOnSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortFailed ,          DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventPortResumeTimedOut ,  DsmStateWaitingForDetachOrRemoveInUnConfigured },
        { DsmEventPortResumed ,         DsmStateSuspendingOnHubSuspendInStopped },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForReportedMissingOnDetachInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForRemoveOnDetach = {
    // State ETW Name
    DsmStateEtwWaitingForRemoveOnDetach,
    // State Entry Function
    HUBDSM_WaitingForRemoveOnDetach,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForRemoveOnDetach,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOCleanup ,          DsmStateDeletingDeviceAndDefaultEndpointOnCleanupAfterDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForRemoveOrReprotedMissingOnHubStopOnDetach = {
    // State ETW Name
    DsmStateEtwWaitingForRemoveOrReprotedMissingOnHubStopOnDetach,
    // State Entry Function
    HUBDSM_WaitingForRemoveOrReprotedMissingOnHubStopOnDetach,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForRemoveOrReprotedMissingOnHubStopOnDetach,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOReportedMissing ,  DsmStateRemovingDeviceInfoFromGlobalListOnReportedMissingOnHubStopOnDetach },
        { DsmEventPDOCleanup ,          DsmStateRemovingDeviceInfoFromGlobalListOnCleanUpOnHubStopOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForReportedMissingOnDetachInConfigured = {
    // State ETW Name
    DsmStateEtwWaitingForReportedMissingOnDetachInConfigured,
    // State Entry Function
    HUBDSM_WaitingForReportedMissingOnDetachInConfigured,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    DsmSubSmWaitingForReportedMissingOnDetachInConfigured,
    // Event State Pairs for Transitions
    {
        { DsmEventHubStop ,             DsmStateDisablingEndpointsInControllerOHubStopOnDetach },
        { DsmEventPDOD0ExitFinal ,      DsmStateDisablingEndpointsInControllerOnD0ExitOnDetach },
        { DsmEventPDOReportedMissing ,  DsmStateWaitingForPDOD0ExitOnDetachInConfigured },
        { DsmEventHubStopAfterSuspend , DsmStateDisablingEndpointsInControllerOHubStopAfterSuspendOnDetach },
        { DsmEventPDOCleanup ,          DsmStateRemovingDeviceInfoFromGlobalListOnCleanupOnDetachInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForReportedMissingOnDetachInUnConfigured = {
    // State ETW Name
    DsmStateEtwWaitingForReportedMissingOnDetachInUnConfigured,
    // State Entry Function
    HUBDSM_WaitingForReportedMissingOnDetachInUnConfigured,
    // State Flags
    StateFlagHandlesLowPriEvents|StateFlagRequiresPassive,
    // Sub State Machine Information
    DsmSubSmWaitingForReportedMissingOnDetachInUnConfigured,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOReportedMissing ,  DsmStateWaitingForPDOD0ExitOnDetachInUnConfigured },
        { DsmEventPDOCleanup ,          DsmStateRemovingDeviceInfoFromGlobalListOnCleanupOnDetachInUnConfigured },
        { DsmEventPDOD0ExitFinal ,      DsmStateDisablingDeviceAndDefaultEpInControllerOnDetach },
        { DsmEventHubStopAfterSuspend , DsmStateDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspendOnDetach },
        { DsmEventHubStop ,             DsmStateDisablingDeviceAndDefaultEpInControllerOnHubStopOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingToBeDeleted = {
    // State ETW Name
    DsmStateEtwWaitingToBeDeleted,
    // State Entry Function
    HUBDSM_WaitingToBeDeleted,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOReportedMissing ,  DsmStateIgnored },
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueryingLanguageSpecificProductIdString = {
    // State ETW Name
    DsmStateEtwQueryingLanguageSpecificProductIdString,
    // State Entry Function
    HUBDSM_QueryingLanguageSpecificProductIdString,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateSignalingDeviceQueryTextEvent },
        { DsmEventTransferSuccess ,     DsmStateValidatingLanguageSpecificProductIdString },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInQueryingProductId = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInQueryingProductId,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInQueryingProductId,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySignalingDeviceQueryTextEvent = {
    // State ETW Name
    DsmStateEtwSignalingDeviceQueryTextEvent,
    // State Entry Function
    HUBDSM_SignalingDeviceQueryTextEvent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationSuccessInQueryingProductId },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingLanguageSpecificProductIdString = {
    // State ETW Name
    DsmStateEtwValidatingLanguageSpecificProductIdString,
    // State Entry Function
    HUBDSM_ValidatingLanguageSpecificProductIdString,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateSignalingDeviceQueryTextEvent },
        { DsmEventOperationFailure ,    DsmStateSignalingDeviceQueryTextEvent },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAcquiringAddressZeroOwnershipInEnum = {
    // State ETW Name
    DsmStateEtwAcquiringAddressZeroOwnershipInEnum,
    // State Entry Function
    HUBDSM_AcquiringAddressZeroOwnershipInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateCancellingAcquiringAddress0OnDetachInEnum },
        { DsmEventHubStop ,             DsmStateWaitingForUCXIoctlOnStopSuspendInEnum },
        { DsmEventUCXIoctlSuccess ,     DsmStateEnumeratingAtAddressZeroInEnum },
        { DsmEventUCXIoctlFailure ,     DsmStateCheckingIfEnumRetryReachedMaximumInEnum },
        { DsmEventHubSuspend ,          DsmStateWaitingForUCXIoctlOnStopSuspendInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAcquiringPowerReferenceOnStartInEnum = {
    // State ETW Name
    DsmStateEtwAcquiringPowerReferenceOnStartInEnum,
    // State Entry Function
    HUBDSM_AcquiringPowerReferenceOnStartInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateAcquiringAddressZeroOwnershipInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCancellingAcquiringAddress0OnDetachInEnum = {
    // State ETW Name
    DsmStateEtwCancellingAcquiringAddress0OnDetachInEnum,
    // State Entry Function
    HUBDSM_CancellingAcquiringAddress0OnDetachInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForUCXIoctlOnDetachInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCancellingRetryTimerOnDetachInEnum = {
    // State ETW Name
    DsmStateEtwCancellingRetryTimerOnDetachInEnum,
    // State Entry Function
    HUBDSM_CancellingRetryTimerOnDetachInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerToFlushOnDetachInEnum },
        { DsmEventOperationSuccess ,    DsmStateReturningPortDetachDeviceInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCancellingRetryTimerOnStopSuspendInEnum = {
    // State ETW Name
    DsmStateEtwCancellingRetryTimerOnStopSuspendInEnum,
    // State Entry Function
    HUBDSM_CancellingRetryTimerOnStopSuspendInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerToFlushOnStopSuspendInEnum },
        { DsmEventOperationSuccess ,    DsmStateDisablingOnStopSuspendOnIoctlFailureInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDevicePDChargingPolicySupported = {
    // State ETW Name
    DsmStateEtwCheckingIfDevicePDChargingPolicySupported,
    // State Entry Function
    HUBDSM_CheckingIfDevicePDChargingPolicySupported,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateSettingDevicePDChargingPolicy },
        { DsmEventNo ,                  DsmStateReturningOperationSuccessInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfEnumRetryReachedMaximumInEnum = {
    // State ETW Name
    DsmStateEtwCheckingIfEnumRetryReachedMaximumInEnum,
    // State Entry Function
    HUBDSM_CheckingIfEnumRetryReachedMaximumInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateStartingTimerForEnumRetryInEnum },
        { DsmEventYes ,                 DsmStateDisablingOnEnumAfterFailureInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfEnumRetryReachedMaximumInEnumWithAddr0Ownership = {
    // State ETW Name
    DsmStateEtwCheckingIfEnumRetryReachedMaximumInEnumWithAddr0Ownership,
    // State Entry Function
    HUBDSM_CheckingIfEnumRetryReachedMaximumInEnumWithAddr0Ownership,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateStartingTimerForEnumRetryInEnumWithAddr0Ownership },
        { DsmEventYes ,                 DsmStateDisablingOnEnumAfterFailureInEnumWithAddress0Ownership },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryClearingDeviceProgrammingLostFlagOnEnum = {
    // State ETW Name
    DsmStateEtwClearingDeviceProgrammingLostFlagOnEnum,
    // State Entry Function
    HUBDSM_ClearingDeviceProgrammingLostFlagOnEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateAcquiringAddressZeroOwnershipInEnum },
        { DsmEventNo ,                  DsmStateAcquiringAddressZeroOwnershipInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingDefaultEndpointAndDeviceFromUCXOnDetachInEnumeratingAfterAddr = {
    // State ETW Name
    DsmStateEtwDeletingDefaultEndpointAndDeviceFromUCXOnDetachInEnumeratingAfterAddr,
    // State Entry Function
    HUBDSM_DeletingDefaultEndpointAndDeviceFromUCXOnDetachInEnumeratingAfterAddr,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningPortDetachDeviceInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingDefaultEndpointAndDeviceFromUCXOnFailureInEnum = {
    // State ETW Name
    DsmStateEtwDeletingDefaultEndpointAndDeviceFromUCXOnFailureInEnum,
    // State Entry Function
    HUBDSM_DeletingDefaultEndpointAndDeviceFromUCXOnFailureInEnum,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfEnumRetryReachedMaximumInEnumWithAddr0Ownership },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingDefaultEndpointAndDeviceOnPostAddressFailureInEnum = {
    // State ETW Name
    DsmStateEtwDeletingDefaultEndpointAndDeviceOnPostAddressFailureInEnum,
    // State Entry Function
    HUBDSM_DeletingDefaultEndpointAndDeviceOnPostAddressFailureInEnum,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfEnumRetryReachedMaximumInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingDefaultEndpointAndDeviceOnStopSuspendAfterAddressInEnum = {
    // State ETW Name
    DsmStateEtwDeletingDefaultEndpointAndDeviceOnStopSuspendAfterAddressInEnum,
    // State Entry Function
    HUBDSM_DeletingDefaultEndpointAndDeviceOnStopSuspendAfterAddressInEnum,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReleasingPowerReferenceInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingDeviceAndDefaultEndpointOnDetachWithAddress0OwnershipInEnum = {
    // State ETW Name
    DsmStateEtwDeletingDeviceAndDefaultEndpointOnDetachWithAddress0OwnershipInEnum,
    // State Entry Function
    HUBDSM_DeletingDeviceAndDefaultEndpointOnDetachWithAddress0OwnershipInEnum,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReleasingAddressZeroOwnershipOnDetachInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingDeviceAndDefaultEndpointOnStopSuspendWithAddress0OwnershipInEnum = {
    // State ETW Name
    DsmStateEtwDeletingDeviceAndDefaultEndpointOnStopSuspendWithAddress0OwnershipInEnum,
    // State Entry Function
    HUBDSM_DeletingDeviceAndDefaultEndpointOnStopSuspendWithAddress0OwnershipInEnum,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDisablingOnStopSuspendInEnumWithAddress0Ownership },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnDetachInEnumeratingAfterAddr = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnDetachInEnumeratingAfterAddr,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnDetachInEnumeratingAfterAddr,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingDefaultEndpointAndDeviceFromUCXOnDetachInEnumeratingAfterAddr },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnDetachWithAddress0OwnershipInEnum = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnDetachWithAddress0OwnershipInEnum,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnDetachWithAddress0OwnershipInEnum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingDeviceAndDefaultEndpointOnDetachWithAddress0OwnershipInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnPostAddressEnumFailureInEnum = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnPostAddressEnumFailureInEnum,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnPostAddressEnumFailureInEnum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingDefaultEndpointAndDeviceOnPostAddressFailureInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnSetAddressFailureInEnum = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnSetAddressFailureInEnum,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnSetAddressFailureInEnum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingDefaultEndpointAndDeviceFromUCXOnFailureInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnStopSuspendAfterAddressInEnum = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnStopSuspendAfterAddressInEnum,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnStopSuspendAfterAddressInEnum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingDefaultEndpointAndDeviceOnStopSuspendAfterAddressInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnStopSuspendWithAddress0OwnershipInEnum = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnStopSuspendWithAddress0OwnershipInEnum,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnStopSuspendWithAddress0OwnershipInEnum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingDeviceAndDefaultEndpointOnStopSuspendWithAddress0OwnershipInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingOnEnumAfterFailureInEnum = {
    // State ETW Name
    DsmStateEtwDisablingOnEnumAfterFailureInEnum,
    // State Entry Function
    HUBDSM_DisablingOnEnumAfterFailureInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInEnum },
        { DsmEventHubStop ,             DsmStateWaitingForDisableCompleteOnStopSuspendAfterFailureInEnum },
        { DsmEventPortFailed ,          DsmStateReturningOperationFailureInEnum },
        { DsmEventHubSuspend ,          DsmStateWaitingForDisableCompleteOnStopSuspendAfterFailureInEnum },
        { DsmEventPortDisabled ,        DsmStateReturningOperationFailureInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingOnEnumAfterFailureInEnumWithAddress0Ownership = {
    // State ETW Name
    DsmStateEtwDisablingOnEnumAfterFailureInEnumWithAddress0Ownership,
    // State Entry Function
    HUBDSM_DisablingOnEnumAfterFailureInEnumWithAddress0Ownership,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubStop ,             DsmStateWaitingForDisableCompleteOnStopSuspendInEnumWithAddress0Ownership },
        { DsmEventPortDetachDevice ,    DsmStateReleasingAddressZeroOwnershipOnDetachInEnum },
        { DsmEventHubSuspend ,          DsmStateWaitingForDisableCompleteOnStopSuspendInEnumWithAddress0Ownership },
        { DsmEventPortFailed ,          DsmStateReleasingAddressZeroOwnershipOnEnumFailure },
        { DsmEventPortDisabled ,        DsmStateReleasingAddressZeroOwnershipOnEnumFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingOnStopSuspendInEnum = {
    // State ETW Name
    DsmStateEtwDisablingOnStopSuspendInEnum,
    // State Entry Function
    HUBDSM_DisablingOnStopSuspendInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDisableCompleteOnStopSuspendInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingOnStopSuspendInEnumWithAddress0Ownership = {
    // State ETW Name
    DsmStateEtwDisablingOnStopSuspendInEnumWithAddress0Ownership,
    // State Entry Function
    HUBDSM_DisablingOnStopSuspendInEnumWithAddress0Ownership,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDisableCompleteOnStopSuspendInEnumWithAddress0Ownership },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingOnStopSuspendOnIoctlFailureInEnum = {
    // State ETW Name
    DsmStateEtwDisablingOnStopSuspendOnIoctlFailureInEnum,
    // State Entry Function
    HUBDSM_DisablingOnStopSuspendOnIoctlFailureInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInEnum },
        { DsmEventPortFailed ,          DsmStateReleasingPowerReferenceInEnum },
        { DsmEventPortDisabled ,        DsmStateReleasingPowerReferenceInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryEnumeratingAfterAddressInEnum = {
    // State ETW Name
    DsmStateEtwEnumeratingAfterAddressInEnum,
    // State Entry Function
    HUBDSM_EnumeratingAfterAddressInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmEnumeratingAfterAddressInEnum,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateSettingLPMValues },
        { DsmEventOperationFailure ,    DsmStateDisablingDeviceInControllerOnPostAddressEnumFailureInEnum },
        { DsmEventPortDetachDevice ,    DsmStateDisablingDeviceInControllerOnDetachInEnumeratingAfterAddr },
        { DsmEventHubSuspend ,          DsmStateDisablingOnStopSuspendInEnum },
        { DsmEventHubStop ,             DsmStateDisablingOnStopSuspendInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryEnumeratingAtAddressZeroInEnum = {
    // State ETW Name
    DsmStateEtwEnumeratingAtAddressZeroInEnum,
    // State Entry Function
    HUBDSM_EnumeratingAtAddressZeroInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmEnumeratingAtAddressZeroInEnum,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateSettingNonZeroAddressInEnum },
        { DsmEventOperationFailure ,    DsmStateCheckingIfEnumRetryReachedMaximumInEnumWithAddr0Ownership },
        { DsmEventPortDetachDevice ,    DsmStateReleasingAddressZeroOwnershipOnDetachInEnum },
        { DsmEventHubStop ,             DsmStateDisablingOnStopSuspendInEnumWithAddress0Ownership },
        { DsmEventHubSuspend ,          DsmStateDisablingOnStopSuspendInEnumWithAddress0Ownership },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryInitializingEnumRetryCountInEnum = {
    // State ETW Name
    DsmStateEtwInitializingEnumRetryCountInEnum,
    // State Entry Function
    HUBDSM_InitializingEnumRetryCountInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateAcquiringAddressZeroOwnershipInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingAddressZeroOwnershipInEnum = {
    // State ETW Name
    DsmStateEtwReleasingAddressZeroOwnershipInEnum,
    // State Entry Function
    HUBDSM_ReleasingAddressZeroOwnershipInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateEnumeratingAfterAddressInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingAddressZeroOwnershipOnDetach = {
    // State ETW Name
    DsmStateEtwReleasingAddressZeroOwnershipOnDetach,
    // State Entry Function
    HUBDSM_ReleasingAddressZeroOwnershipOnDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCancellingRetryTimerOnDetachInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingAddressZeroOwnershipOnDetachInEnum = {
    // State ETW Name
    DsmStateEtwReleasingAddressZeroOwnershipOnDetachInEnum,
    // State Entry Function
    HUBDSM_ReleasingAddressZeroOwnershipOnDetachInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningPortDetachDeviceInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingAddressZeroOwnershipOnEnumFailure = {
    // State ETW Name
    DsmStateEtwReleasingAddressZeroOwnershipOnEnumFailure,
    // State Entry Function
    HUBDSM_ReleasingAddressZeroOwnershipOnEnumFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationFailureInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingAddressZeroOwnershipOnStopSuspend = {
    // State ETW Name
    DsmStateEtwReleasingAddressZeroOwnershipOnStopSuspend,
    // State Entry Function
    HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCancellingRetryTimerOnStopSuspendInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingAddressZeroOwnershipOnStopSuspendInEnum = {
    // State ETW Name
    DsmStateEtwReleasingAddressZeroOwnershipOnStopSuspendInEnum,
    // State Entry Function
    HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReleasingPowerReferenceInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceInEnum = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceInEnum,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateStoppedSuspendedInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureInEnum = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureInEnum,
    // State Entry Function
    HUBDSM_ReturningOperationFailureInEnum,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInEnum = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInEnum,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInEnum,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDetachDeviceInEnum = {
    // State ETW Name
    DsmStateEtwReturningPortDetachDeviceInEnum,
    // State Entry Function
    HUBDSM_ReturningPortDetachDeviceInEnum,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingDevicePDChargingPolicy = {
    // State ETW Name
    DsmStateEtwSettingDevicePDChargingPolicy,
    // State Entry Function
    HUBDSM_SettingDevicePDChargingPolicy,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateReturningOperationSuccessInEnum },
        { DsmEventTransferFailure ,     DsmStateDisablingDeviceInControllerOnPostAddressEnumFailureInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingLPMValues = {
    // State ETW Name
    DsmStateEtwSettingLPMValues,
    // State Entry Function
    HUBDSM_SettingLPMValues,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmSettingLPMValues,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateDisablingDeviceInControllerOnPostAddressEnumFailureInEnum },
        { DsmEventOperationSuccess ,    DsmStateUpdatingDeviceInformationInEnum },
        { DsmEventContinueIfNoSubState ,DsmStateUpdatingDeviceInformationInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingNonZeroAddressInEnum = {
    // State ETW Name
    DsmStateEtwSettingNonZeroAddressInEnum,
    // State Entry Function
    HUBDSM_SettingNonZeroAddressInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmSettingNonZeroAddressInEnum,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateDisablingDeviceInControllerOnSetAddressFailureInEnum },
        { DsmEventOperationSuccess ,    DsmStateReleasingAddressZeroOwnershipInEnum },
        { DsmEventHubStop ,             DsmStateDisablingDeviceInControllerOnStopSuspendWithAddress0OwnershipInEnum },
        { DsmEventPortDetachDevice ,    DsmStateDisablingDeviceInControllerOnDetachWithAddress0OwnershipInEnum },
        { DsmEventHubSuspend ,          DsmStateDisablingDeviceInControllerOnStopSuspendWithAddress0OwnershipInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStartingTimerForEnumRetryInEnum = {
    // State ETW Name
    DsmStateEtwStartingTimerForEnumRetryInEnum,
    // State Entry Function
    HUBDSM_StartingTimerForEnumRetryInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTimerFired ,          DsmStateAcquiringAddressZeroOwnershipInEnum },
        { DsmEventPortDetachDevice ,    DsmStateCancellingRetryTimerOnDetachInEnum },
        { DsmEventHubSuspend ,          DsmStateCancellingRetryTimerOnStopSuspendInEnum },
        { DsmEventHubStop ,             DsmStateCancellingRetryTimerOnStopSuspendInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStartingTimerForEnumRetryInEnumWithAddr0Ownership = {
    // State ETW Name
    DsmStateEtwStartingTimerForEnumRetryInEnumWithAddr0Ownership,
    // State Entry Function
    HUBDSM_StartingTimerForEnumRetryInEnumWithAddr0Ownership,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTimerFired ,          DsmStateEnumeratingAtAddressZeroInEnum },
        { DsmEventPortDetachDevice ,    DsmStateReleasingAddressZeroOwnershipOnDetach },
        { DsmEventHubSuspend ,          DsmStateReleasingAddressZeroOwnershipOnStopSuspend },
        { DsmEventHubStop ,             DsmStateReleasingAddressZeroOwnershipOnStopSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppedSuspendedInEnum = {
    // State ETW Name
    DsmStateEtwStoppedSuspendedInEnum,
    // State Entry Function
    HUBDSM_StoppedSuspendedInEnum,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    DsmSubSmStoppedSuspendedInEnum,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceResumedOnHubResume ,DsmStateClearingDeviceProgrammingLostFlagOnEnum },
        { DsmEventHubStopWithReferenceAcquired ,DsmStateReleasingPowerReferenceInEnum },
        { DsmEventDeviceSuspendedOnHubResume ,DsmStateClearingDeviceProgrammingLostFlagOnEnum },
        { DsmEventDeviceRenumerateOnHubResume ,DsmStateClearingDeviceProgrammingLostFlagOnEnum },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInEnum },
        { DsmEventHubStart ,            DsmStateAcquiringPowerReferenceOnStartInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUpdatingDeviceInformationInEnum = {
    // State ETW Name
    DsmStateEtwUpdatingDeviceInformationInEnum,
    // State Entry Function
    HUBDSM_UpdatingDeviceInformationInEnum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlFailure ,     DsmStateDisablingDeviceInControllerOnPostAddressEnumFailureInEnum },
        { DsmEventUCXIoctlSuccess ,     DsmStateCheckingIfDevicePDChargingPolicySupported },
        { DsmEventUCXIoctlFailureDueToExitLatencyTooLarge ,DsmStateDisablingDeviceInControllerOnPostAddressEnumFailureInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDisableCompleteOnStopSuspendAfterFailureInEnum = {
    // State ETW Name
    DsmStateEtwWaitingForDisableCompleteOnStopSuspendAfterFailureInEnum,
    // State Entry Function
    HUBDSM_WaitingForDisableCompleteOnStopSuspendAfterFailureInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInEnum },
        { DsmEventPortFailed ,          DsmStateReleasingPowerReferenceInEnum },
        { DsmEventPortDisabled ,        DsmStateReleasingPowerReferenceInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDisableCompleteOnStopSuspendInEnum = {
    // State ETW Name
    DsmStateEtwWaitingForDisableCompleteOnStopSuspendInEnum,
    // State Entry Function
    HUBDSM_WaitingForDisableCompleteOnStopSuspendInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateDisablingDeviceInControllerOnDetachInEnumeratingAfterAddr },
        { DsmEventPortFailed ,          DsmStateDisablingDeviceInControllerOnStopSuspendAfterAddressInEnum },
        { DsmEventPortDisabled ,        DsmStateDisablingDeviceInControllerOnStopSuspendAfterAddressInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDisableCompleteOnStopSuspendInEnumWithAddress0Ownership = {
    // State ETW Name
    DsmStateEtwWaitingForDisableCompleteOnStopSuspendInEnumWithAddress0Ownership,
    // State Entry Function
    HUBDSM_WaitingForDisableCompleteOnStopSuspendInEnumWithAddress0Ownership,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReleasingAddressZeroOwnershipOnDetachInEnum },
        { DsmEventPortDisabled ,        DsmStateReleasingAddressZeroOwnershipOnStopSuspendInEnum },
        { DsmEventPortFailed ,          DsmStateReleasingAddressZeroOwnershipOnStopSuspendInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerToFlushOnDetachInEnum = {
    // State ETW Name
    DsmStateEtwWaitingForTimerToFlushOnDetachInEnum,
    // State Entry Function
    HUBDSM_WaitingForTimerToFlushOnDetachInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventTimerFired ,          DsmStateReturningPortDetachDeviceInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerToFlushOnStopSuspendInEnum = {
    // State ETW Name
    DsmStateEtwWaitingForTimerToFlushOnStopSuspendInEnum,
    // State Entry Function
    HUBDSM_WaitingForTimerToFlushOnStopSuspendInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTimerFired ,          DsmStateDisablingOnStopSuspendOnIoctlFailureInEnum },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForTimerToFlushOnDetachInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForUCXIoctlOnDetachInEnum = {
    // State ETW Name
    DsmStateEtwWaitingForUCXIoctlOnDetachInEnum,
    // State Entry Function
    HUBDSM_WaitingForUCXIoctlOnDetachInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventUCXIoctlFailure ,     DsmStateReturningPortDetachDeviceInEnum },
        { DsmEventUCXIoctlSuccess ,     DsmStateReleasingAddressZeroOwnershipOnDetachInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForUCXIoctlOnStopSuspendInEnum = {
    // State ETW Name
    DsmStateEtwWaitingForUCXIoctlOnStopSuspendInEnum,
    // State Entry Function
    HUBDSM_WaitingForUCXIoctlOnStopSuspendInEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingOnStopSuspendInEnumWithAddress0Ownership },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForUCXIoctlOnDetachInEnum },
        { DsmEventUCXIoctlFailure ,     DsmStateDisablingOnStopSuspendOnIoctlFailureInEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfFirstEnumTryAfterReset1 = {
    // State ETW Name
    DsmStateEtwCheckingIfFirstEnumTryAfterReset1,
    // State Entry Function
    HUBDSM_CheckingIfFirstEnumTryAfterReset1,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateWaitingForPostReset1Timer },
        { DsmEventNo ,                  DsmStateWaitingForPostReset1ExtendedTimer },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfFirstEnumTryAfterReset2 = {
    // State ETW Name
    DsmStateEtwCheckingIfFirstEnumTryAfterReset2,
    // State Entry Function
    HUBDSM_CheckingIfFirstEnumTryAfterReset2,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateWaitingForPostReset2Timer },
        { DsmEventNo ,                  DsmStateWaitingForPostReset2ExtendedTimer },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfSecondResetIsNeeded = {
    // State ETW Name
    DsmStateEtwCheckingIfSecondResetIsNeeded,
    // State Entry Function
    HUBDSM_CheckingIfSecondResetIsNeeded,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateNotifyingDeviceResetToUCXAfterReset2 },
        { DsmEventYes ,                 DsmStateResetting2 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCreatingUCXDefaultEndpoint = {
    // State ETW Name
    DsmStateEtwCreatingUCXDefaultEndpoint,
    // State Entry Function
    HUBDSM_CreatingUCXDefaultEndpoint,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateEnablingDeviceInUCX },
        { DsmEventOperationFailure ,    DsmStateDeletingUCXDeviceOnOperationFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCreatingUCXDevice = {
    // State ETW Name
    DsmStateEtwCreatingUCXDevice,
    // State Entry Function
    HUBDSM_CreatingUCXDevice,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCreatingUCXDefaultEndpoint },
        { DsmEventOperationFailure ,    DsmStateReturningDeviceOperationFailureInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingUCXDefaultEndpointAndDeviceOnDetach = {
    // State ETW Name
    DsmStateEtwDeletingUCXDefaultEndpointAndDeviceOnDetach,
    // State Entry Function
    HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnDetach,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningPortDetachDeviceInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingUCXDefaultEndpointAndDeviceOnStop = {
    // State ETW Name
    DsmStateEtwDeletingUCXDefaultEndpointAndDeviceOnStop,
    // State Entry Function
    HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnStop,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningHubStopInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingUCXDefaultEndpointAndDeviceOnSuspend = {
    // State ETW Name
    DsmStateEtwDeletingUCXDefaultEndpointAndDeviceOnSuspend,
    // State Entry Function
    HUBDSM_DeletingUCXDefaultEndpointAndDeviceOnSuspend,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningHubSuspendInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingUCXDefaultEndpointOnOperationFailure = {
    // State ETW Name
    DsmStateEtwDeletingUCXDefaultEndpointOnOperationFailure,
    // State Entry Function
    HUBDSM_DeletingUCXDefaultEndpointOnOperationFailure,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDeletingUCXDeviceOnOperationFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingUCXDeviceOnOperationFailure = {
    // State ETW Name
    DsmStateEtwDeletingUCXDeviceOnOperationFailure,
    // State Entry Function
    HUBDSM_DeletingUCXDeviceOnOperationFailure,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningDeviceOperationFailureInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInUCXOnDetach = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInUCXOnDetach,
    // State Entry Function
    HUBDSM_DisablingDeviceInUCXOnDetach,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingUCXDefaultEndpointAndDeviceOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInUCXOnOperationFailure = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInUCXOnOperationFailure,
    // State Entry Function
    HUBDSM_DisablingDeviceInUCXOnOperationFailure,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingUCXDefaultEndpointOnOperationFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInUCXOnStop = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInUCXOnStop,
    // State Entry Function
    HUBDSM_DisablingDeviceInUCXOnStop,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingUCXDefaultEndpointAndDeviceOnStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInUCXOnSuspend = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInUCXOnSuspend,
    // State Entry Function
    HUBDSM_DisablingDeviceInUCXOnSuspend,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingUCXDefaultEndpointAndDeviceOnSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingPortOnHubStopInEnumAtAddr0 = {
    // State ETW Name
    DsmStateEtwDisablingPortOnHubStopInEnumAtAddr0,
    // State Entry Function
    HUBDSM_DisablingPortOnHubStopInEnumAtAddr0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortFailed ,          DsmStateDisablingDeviceInUCXOnStop },
        { DsmEventPortDetachDevice ,    DsmStateDisablingDeviceInUCXOnDetach },
        { DsmEventPortDisabled ,        DsmStateDisablingDeviceInUCXOnStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingPortOnHubSuspendInEnumAtAddr0 = {
    // State ETW Name
    DsmStateEtwDisablingPortOnHubSuspendInEnumAtAddr0,
    // State Entry Function
    HUBDSM_DisablingPortOnHubSuspendInEnumAtAddr0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortFailed ,          DsmStateDisablingDeviceInUCXOnSuspend },
        { DsmEventPortDisabled ,        DsmStateDisablingDeviceInUCXOnSuspend },
        { DsmEventPortDetachDevice ,    DsmStateDisablingDeviceInUCXOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryEnablingDeviceInUCX = {
    // State ETW Name
    DsmStateEtwEnablingDeviceInUCX,
    // State Entry Function
    HUBDSM_EnablingDeviceInUCX,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCheckingIfFirstEnumTryAfterReset1 },
        { DsmEventUCXIoctlFailure ,     DsmStateDeletingUCXDefaultEndpointOnOperationFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingDeviceDescriptorInEnumAtZero = {
    // State ETW Name
    DsmStateEtwGettingDeviceDescriptorInEnumAtZero,
    // State Entry Function
    HUBDSM_GettingDeviceDescriptorInEnumAtZero,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingDeviceDescriptorInEnumAtZero },
        { DsmEventTransferFailure ,     DsmStateValidatingDeviceDescriptorInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryLoggingReset1TimeoutInEnumAtZero = {
    // State ETW Name
    DsmStateEtwLoggingReset1TimeoutInEnumAtZero,
    // State Entry Function
    HUBDSM_LoggingReset1TimeoutInEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningDeviceOperationFailureInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryLoggingReset2TimeoutInEnumAtZero = {
    // State ETW Name
    DsmStateEtwLoggingReset2TimeoutInEnumAtZero,
    // State Entry Function
    HUBDSM_LoggingReset2TimeoutInEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDisablingDeviceInUCXOnOperationFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryNotifyingDeviceResetToUCXAfterReset2 = {
    // State ETW Name
    DsmStateEtwNotifyingDeviceResetToUCXAfterReset2,
    // State Entry Function
    HUBDSM_NotifyingDeviceResetToUCXAfterReset2,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateReturningDeviceOperationSuccessInEnumAtZero },
        { DsmEventUCXIoctlFailure ,     DsmStateDisablingDeviceInUCXOnOperationFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryResetting1 = {
    // State ETW Name
    DsmStateEtwResetting1,
    // State Entry Function
    HUBDSM_Resetting1,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResetComplete ,   DsmStateSettingSpeedFlagFor20Devices },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateWaitingForStopSuspendOnReset1InEnumAtZero },
        { DsmEventHubStop ,             DsmStateWaitingForPortReset1OrDisableOnHubStopInEnumAtZero },
        { DsmEventHubSuspend ,          DsmStateWaitingForPortReset1OrDisableOnHubSuspendInEnumAtZero },
        { DsmEventPortResetTimedOut ,   DsmStateLoggingReset1TimeoutInEnumAtZero },
        { DsmEventPortFailed ,          DsmStateReturningDeviceOperationFailureInEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryResetting2 = {
    // State ETW Name
    DsmStateEtwResetting2,
    // State Entry Function
    HUBDSM_Resetting2,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResetComplete ,   DsmStateCheckingIfFirstEnumTryAfterReset2 },
        { DsmEventPortDetachDevice ,    DsmStateDisablingDeviceInUCXOnDetach },
        { DsmEventPortResetTimedOut ,   DsmStateLoggingReset2TimeoutInEnumAtZero },
        { DsmEventHubStop ,             DsmStateWaitingForPortReset2OnHubStopInEnumAtZero },
        { DsmEventPortFailed ,          DsmStateDisablingDeviceInUCXOnOperationFailure },
        { DsmEventHubSuspend ,          DsmStateWaitingForPortReset2OnHubSuspendInEnumAtZero },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateWaitingForStopSuspendOnReset2InEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningDeviceOperationFailureInEnumAtZero = {
    // State ETW Name
    DsmStateEtwReturningDeviceOperationFailureInEnumAtZero,
    // State Entry Function
    HUBDSM_ReturningDeviceOperationFailureInEnumAtZero,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningDeviceOperationSuccessInEnumAtZero = {
    // State ETW Name
    DsmStateEtwReturningDeviceOperationSuccessInEnumAtZero,
    // State Entry Function
    HUBDSM_ReturningDeviceOperationSuccessInEnumAtZero,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubStopInEnumAtZero = {
    // State ETW Name
    DsmStateEtwReturningHubStopInEnumAtZero,
    // State Entry Function
    HUBDSM_ReturningHubStopInEnumAtZero,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubSuspendInEnumAtZero = {
    // State ETW Name
    DsmStateEtwReturningHubSuspendInEnumAtZero,
    // State Entry Function
    HUBDSM_ReturningHubSuspendInEnumAtZero,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDetachDeviceInEnumAtZero = {
    // State ETW Name
    DsmStateEtwReturningPortDetachDeviceInEnumAtZero,
    // State Entry Function
    HUBDSM_ReturningPortDetachDeviceInEnumAtZero,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingSpeedFlagFor20Devices = {
    // State ETW Name
    DsmStateEtwSettingSpeedFlagFor20Devices,
    // State Entry Function
    HUBDSM_SettingSpeedFlagFor20Devices,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCreatingUCXDevice },
        { DsmEventOperationFailure ,    DsmStateReturningDeviceOperationFailureInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppingTimerOnHubStopInEnumAtZero = {
    // State ETW Name
    DsmStateEtwStoppingTimerOnHubStopInEnumAtZero,
    // State Entry Function
    HUBDSM_StoppingTimerOnHubStopInEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerOnHubStopInEnumAtZero },
        { DsmEventOperationSuccess ,    DsmStateDisablingDeviceInUCXOnStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppingTimerOnHubSuspendInEnumAtZero = {
    // State ETW Name
    DsmStateEtwStoppingTimerOnHubSuspendInEnumAtZero,
    // State Entry Function
    HUBDSM_StoppingTimerOnHubSuspendInEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerOnHubSuspendInEnumAtZero },
        { DsmEventOperationSuccess ,    DsmStateDisablingDeviceInUCXOnSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppingTimerOnPortDetachInEnumAtZero = {
    // State ETW Name
    DsmStateEtwStoppingTimerOnPortDetachInEnumAtZero,
    // State Entry Function
    HUBDSM_StoppingTimerOnPortDetachInEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerOnPortDetachInEnumAtZero },
        { DsmEventOperationSuccess ,    DsmStateDisablingDeviceInUCXOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUpdatingDefaultEndpointMaxPacketSizeInEnum = {
    // State ETW Name
    DsmStateEtwUpdatingDefaultEndpointMaxPacketSizeInEnum,
    // State Entry Function
    HUBDSM_UpdatingDefaultEndpointMaxPacketSizeInEnum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCheckingIfSecondResetIsNeeded },
        { DsmEventUCXIoctlFailure ,     DsmStateDisablingDeviceInUCXOnOperationFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingDeviceDescriptorInEnumAtZero = {
    // State ETW Name
    DsmStateEtwValidatingDeviceDescriptorInEnumAtZero,
    // State Entry Function
    HUBDSM_ValidatingDeviceDescriptorInEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateUpdatingDefaultEndpointMaxPacketSizeInEnum },
        { DsmEventOperationFailure ,    DsmStateDisablingDeviceInUCXOnOperationFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPortReset1OrDisableOnHubStopInEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForPortReset1OrDisableOnHubStopInEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForPortReset1OrDisableOnHubStopInEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResetComplete ,   DsmStateReturningHubStopInEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInEnumAtZero },
        { DsmEventPortResetTimedOut ,   DsmStateReturningHubStopInEnumAtZero },
        { DsmEventPortFailed ,          DsmStateReturningHubStopInEnumAtZero },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateReturningHubStopInEnumAtZero },
        { DsmEventPortDisabled ,        DsmStateReturningHubStopInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPortReset1OrDisableOnHubSuspendInEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForPortReset1OrDisableOnHubSuspendInEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForPortReset1OrDisableOnHubSuspendInEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResetComplete ,   DsmStateReturningHubSuspendInEnumAtZero },
        { DsmEventPortDisabled ,        DsmStateReturningHubSuspendInEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInEnumAtZero },
        { DsmEventPortResetTimedOut ,   DsmStateReturningHubSuspendInEnumAtZero },
        { DsmEventPortFailed ,          DsmStateReturningHubSuspendInEnumAtZero },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateReturningHubSuspendInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPortReset2OnHubStopInEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForPortReset2OnHubStopInEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForPortReset2OnHubStopInEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateDisablingDeviceInUCXOnDetach },
        { DsmEventPortResetTimedOut ,   DsmStateDisablingDeviceInUCXOnStop },
        { DsmEventPortFailed ,          DsmStateDisablingDeviceInUCXOnStop },
        { DsmEventPortResetComplete ,   DsmStateDisablingDeviceInUCXOnStop },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateDisablingPortOnHubStopInEnumAtAddr0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPortReset2OnHubSuspendInEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForPortReset2OnHubSuspendInEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForPortReset2OnHubSuspendInEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResetTimedOut ,   DsmStateDisablingDeviceInUCXOnSuspend },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateDisablingPortOnHubSuspendInEnumAtAddr0 },
        { DsmEventPortFailed ,          DsmStateDisablingDeviceInUCXOnSuspend },
        { DsmEventPortResetComplete ,   DsmStateDisablingDeviceInUCXOnSuspend },
        { DsmEventPortDetachDevice ,    DsmStateDisablingDeviceInUCXOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPostReset1ExtendedTimer = {
    // State ETW Name
    DsmStateEtwWaitingForPostReset1ExtendedTimer,
    // State Entry Function
    HUBDSM_WaitingForPostReset1ExtendedTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTimerFired ,          DsmStateGettingDeviceDescriptorInEnumAtZero },
        { DsmEventHubStop ,             DsmStateStoppingTimerOnHubStopInEnumAtZero },
        { DsmEventHubSuspend ,          DsmStateStoppingTimerOnHubSuspendInEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateStoppingTimerOnPortDetachInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPostReset1Timer = {
    // State ETW Name
    DsmStateEtwWaitingForPostReset1Timer,
    // State Entry Function
    HUBDSM_WaitingForPostReset1Timer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTimerFired ,          DsmStateGettingDeviceDescriptorInEnumAtZero },
        { DsmEventHubStop ,             DsmStateStoppingTimerOnHubStopInEnumAtZero },
        { DsmEventHubSuspend ,          DsmStateStoppingTimerOnHubSuspendInEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateStoppingTimerOnPortDetachInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPostReset2ExtendedTimer = {
    // State ETW Name
    DsmStateEtwWaitingForPostReset2ExtendedTimer,
    // State Entry Function
    HUBDSM_WaitingForPostReset2ExtendedTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTimerFired ,          DsmStateNotifyingDeviceResetToUCXAfterReset2 },
        { DsmEventHubStop ,             DsmStateStoppingTimerOnHubStopInEnumAtZero },
        { DsmEventHubSuspend ,          DsmStateStoppingTimerOnHubSuspendInEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateStoppingTimerOnPortDetachInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPostReset2Timer = {
    // State ETW Name
    DsmStateEtwWaitingForPostReset2Timer,
    // State Entry Function
    HUBDSM_WaitingForPostReset2Timer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTimerFired ,          DsmStateNotifyingDeviceResetToUCXAfterReset2 },
        { DsmEventHubStop ,             DsmStateStoppingTimerOnHubStopInEnumAtZero },
        { DsmEventHubSuspend ,          DsmStateStoppingTimerOnHubSuspendInEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateStoppingTimerOnPortDetachInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForStopSuspendOnReset1InEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForStopSuspendOnReset1InEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForStopSuspendOnReset1InEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubSuspend ,          DsmStateReturningHubSuspendInEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInEnumAtZero },
        { DsmEventHubStop ,             DsmStateReturningHubStopInEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForStopSuspendOnReset2InEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForStopSuspendOnReset2InEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForStopSuspendOnReset2InEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateDisablingDeviceInUCXOnDetach },
        { DsmEventHubStop ,             DsmStateDisablingPortOnHubStopInEnumAtAddr0 },
        { DsmEventHubSuspend ,          DsmStateDisablingPortOnHubSuspendInEnumAtAddr0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerOnHubStopInEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForTimerOnHubStopInEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForTimerOnHubStopInEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateWaitingForTimerOnPortDetachInEnumAtZero },
        { DsmEventTimerFired ,          DsmStateDisablingDeviceInUCXOnStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerOnHubSuspendInEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForTimerOnHubSuspendInEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForTimerOnHubSuspendInEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateWaitingForTimerOnPortDetachInEnumAtZero },
        { DsmEventTimerFired ,          DsmStateDisablingDeviceInUCXOnSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerOnPortDetachInEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForTimerOnPortDetachInEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForTimerOnPortDetachInEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventTimerFired ,          DsmStateDisablingDeviceInUCXOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCreatingUCXDefaultEndpointFor30 = {
    // State ETW Name
    DsmStateEtwCreatingUCXDefaultEndpointFor30,
    // State Entry Function
    HUBDSM_CreatingUCXDefaultEndpointFor30,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateResettingDeviceInEnumeration },
        { DsmEventOperationFailure ,    DsmStateDeletingUCXDeviceOnOperationFailureFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCreatingUCXDevice30 = {
    // State ETW Name
    DsmStateEtwCreatingUCXDevice30,
    // State Entry Function
    HUBDSM_CreatingUCXDevice30,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCreatingUCXDefaultEndpointFor30 },
        { DsmEventOperationFailure ,    DsmStateReturningDeviceOperationFailureInPreAddressEnumFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingUCXDefaultEndpointOnOperationFailureFor30 = {
    // State ETW Name
    DsmStateEtwDeletingUCXDefaultEndpointOnOperationFailureFor30,
    // State Entry Function
    HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDeletingUCXDeviceOnOperationFailureFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingUCXDefaultEndpointOnOperationFailureFor30OnDetach = {
    // State ETW Name
    DsmStateEtwDeletingUCXDefaultEndpointOnOperationFailureFor30OnDetach,
    // State Entry Function
    HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnDetach,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDeletingUCXDeviceOnOperationFailureFor30OnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingUCXDefaultEndpointOnOperationFailureFor30OnHubStop = {
    // State ETW Name
    DsmStateEtwDeletingUCXDefaultEndpointOnOperationFailureFor30OnHubStop,
    // State Entry Function
    HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnHubStop,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDeletingUCXDeviceOnOperationFailureFor30OnHubStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingUCXDefaultEndpointOnOperationFailureFor30OnHubSuspend = {
    // State ETW Name
    DsmStateEtwDeletingUCXDefaultEndpointOnOperationFailureFor30OnHubSuspend,
    // State Entry Function
    HUBDSM_DeletingUCXDefaultEndpointOnOperationFailureFor30OnHubSuspend,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDeletingUCXDeviceOnOperationFailureFor30OnHubSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingUCXDeviceOnOperationFailureFor30 = {
    // State ETW Name
    DsmStateEtwDeletingUCXDeviceOnOperationFailureFor30,
    // State Entry Function
    HUBDSM_DeletingUCXDeviceOnOperationFailureFor30,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningDeviceOperationFailureInPreAddressEnumFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingUCXDeviceOnOperationFailureFor30OnDetach = {
    // State ETW Name
    DsmStateEtwDeletingUCXDeviceOnOperationFailureFor30OnDetach,
    // State Entry Function
    HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnDetach,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningDetachInPreAddressEnumFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingUCXDeviceOnOperationFailureFor30OnHubStop = {
    // State ETW Name
    DsmStateEtwDeletingUCXDeviceOnOperationFailureFor30OnHubStop,
    // State Entry Function
    HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnHubStop,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningHubStopInPreAddressEnumFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingUCXDeviceOnOperationFailureFor30OnHubSuspend = {
    // State ETW Name
    DsmStateEtwDeletingUCXDeviceOnOperationFailureFor30OnHubSuspend,
    // State Entry Function
    HUBDSM_DeletingUCXDeviceOnOperationFailureFor30OnHubSuspend,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningHubSuspendInPreAddressEnumFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryEnablingDeviceInUCXFor30 = {
    // State ETW Name
    DsmStateEtwEnablingDeviceInUCXFor30,
    // State Entry Function
    HUBDSM_EnablingDeviceInUCXFor30,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateReturningDeviceOperationSuccessInPreAddressEnumFor30 },
        { DsmEventUCXIoctlFailure ,     DsmStateDeletingUCXDefaultEndpointOnOperationFailureFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryResettingDeviceInEnumeration = {
    // State ETW Name
    DsmStateEtwResettingDeviceInEnumeration,
    // State Entry Function
    HUBDSM_ResettingDeviceInEnumeration,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    DsmSubSmResettingDeviceInEnumeration,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateSetting30Speed },
        { DsmEventOperationFailure ,    DsmStateDeletingUCXDefaultEndpointOnOperationFailureFor30 },
        { DsmEventHubStop ,             DsmStateDeletingUCXDefaultEndpointOnOperationFailureFor30OnHubStop },
        { DsmEventHubSuspend ,          DsmStateDeletingUCXDefaultEndpointOnOperationFailureFor30OnHubSuspend },
        { DsmEventPortDetachDevice ,    DsmStateDeletingUCXDefaultEndpointOnOperationFailureFor30OnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningDetachInPreAddressEnumFor30 = {
    // State ETW Name
    DsmStateEtwReturningDetachInPreAddressEnumFor30,
    // State Entry Function
    HUBDSM_ReturningDetachInPreAddressEnumFor30,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningDeviceOperationFailureInPreAddressEnumFor30 = {
    // State ETW Name
    DsmStateEtwReturningDeviceOperationFailureInPreAddressEnumFor30,
    // State Entry Function
    HUBDSM_ReturningDeviceOperationFailureInPreAddressEnumFor30,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningDeviceOperationSuccessInPreAddressEnumFor30 = {
    // State ETW Name
    DsmStateEtwReturningDeviceOperationSuccessInPreAddressEnumFor30,
    // State Entry Function
    HUBDSM_ReturningDeviceOperationSuccessInPreAddressEnumFor30,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubStopInPreAddressEnumFor30 = {
    // State ETW Name
    DsmStateEtwReturningHubStopInPreAddressEnumFor30,
    // State Entry Function
    HUBDSM_ReturningHubStopInPreAddressEnumFor30,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubSuspendInPreAddressEnumFor30 = {
    // State ETW Name
    DsmStateEtwReturningHubSuspendInPreAddressEnumFor30,
    // State Entry Function
    HUBDSM_ReturningHubSuspendInPreAddressEnumFor30,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySetting30Speed = {
    // State ETW Name
    DsmStateEtwSetting30Speed,
    // State Entry Function
    HUBDSM_Setting30Speed,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateEnablingDeviceInUCXFor30 },
        { DsmEventOperationFailure ,    DsmStateDeletingUCXDefaultEndpointOnOperationFailureFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCancellingControlTransferOnDetachInEnumAfterAddr = {
    // State ETW Name
    DsmStateEtwCancellingControlTransferOnDetachInEnumAfterAddr,
    // State Entry Function
    HUBDSM_CancellingControlTransferOnDetachInEnumAfterAddr,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateReturningPortDeviceDetachInEnumAfterAddr },
        { DsmEventTransferSuccess ,     DsmStateReturningPortDeviceDetachInEnumAfterAddr },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCancellingControlTransferOnHubStopInEnumAfterAddr = {
    // State ETW Name
    DsmStateEtwCancellingControlTransferOnHubStopInEnumAfterAddr,
    // State Entry Function
    HUBDSM_CancellingControlTransferOnHubStopInEnumAfterAddr,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateReturningHubStopInEnumAfterAddr },
        { DsmEventTransferSuccess ,     DsmStateReturningHubStopInEnumAfterAddr },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCancellingControlTransferOnHubSuspendInEnumAfterAddr = {
    // State ETW Name
    DsmStateEtwCancellingControlTransferOnHubSuspendInEnumAfterAddr,
    // State Entry Function
    HUBDSM_CancellingControlTransferOnHubSuspendInEnumAfterAddr,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateReturningHubSuspendInEnumAfterAddr },
        { DsmEventTransferSuccess ,     DsmStateReturningHubSuspendInEnumAfterAddr },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingBytesReturnedInConfigDescriptor = {
    // State ETW Name
    DsmStateEtwCheckingBytesReturnedInConfigDescriptor,
    // State Entry Function
    HUBDSM_CheckingBytesReturnedInConfigDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateValidatingConfigurationDescriptor },
        { DsmEventYes ,                 DsmStateGettingConfigurationDescriptorWithReturnedLength },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfAltEnumCmdCached = {
    // State ETW Name
    DsmStateEtwCheckingIfAltEnumCmdCached,
    // State Entry Function
    HUBDSM_CheckingIfAltEnumCmdCached,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateSendingAltEnumCmdInEnumAfterAddr },
        { DsmEventNo ,                  DsmStateCheckingIfSuperSpeedNeedsToBeDisabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfSuperSpeedNeedsToBeDisabled = {
    // State ETW Name
    DsmStateEtwCheckingIfSuperSpeedNeedsToBeDisabled,
    // State Entry Function
    HUBDSM_CheckingIfSuperSpeedNeedsToBeDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateDiasablingSuperSpeed },
        { DsmEventNo ,                  DsmStateGettingConfigurationDescriptorWithDefaultLength },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDiasablingSuperSpeed = {
    // State ETW Name
    DsmStateEtwDiasablingSuperSpeed,
    // State Entry Function
    HUBDSM_DiasablingSuperSpeed,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingConfigurationDescriptorWithDefaultLength = {
    // State ETW Name
    DsmStateEtwGettingConfigurationDescriptorWithDefaultLength,
    // State Entry Function
    HUBDSM_GettingConfigurationDescriptorWithDefaultLength,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateCancellingControlTransferOnDetachInEnumAfterAddr },
        { DsmEventHubSuspend ,          DsmStateCancellingControlTransferOnHubSuspendInEnumAfterAddr },
        { DsmEventHubStop ,             DsmStateCancellingControlTransferOnHubStopInEnumAfterAddr },
        { DsmEventTransferFailure ,     DsmStateReturningOperationFailureInEnumAfterAddr },
        { DsmEventTransferSuccess ,     DsmStateCheckingBytesReturnedInConfigDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingConfigurationDescriptorWithReturnedLength = {
    // State ETW Name
    DsmStateEtwGettingConfigurationDescriptorWithReturnedLength,
    // State Entry Function
    HUBDSM_GettingConfigurationDescriptorWithReturnedLength,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateCancellingControlTransferOnDetachInEnumAfterAddr },
        { DsmEventHubSuspend ,          DsmStateCancellingControlTransferOnHubSuspendInEnumAfterAddr },
        { DsmEventHubStop ,             DsmStateCancellingControlTransferOnHubStopInEnumAfterAddr },
        { DsmEventTransferFailure ,     DsmStateReturningOperationFailureInEnumAfterAddr },
        { DsmEventTransferSuccess ,     DsmStateValidatingConfigurationDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingDeviceDescriptorAfterAddressing = {
    // State ETW Name
    DsmStateEtwGettingDeviceDescriptorAfterAddressing,
    // State Entry Function
    HUBDSM_GettingDeviceDescriptorAfterAddressing,
    // State Flags
    StateFlagHandlesLowPriEvents,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateCancellingControlTransferOnDetachInEnumAfterAddr },
        { DsmEventHubSuspend ,          DsmStateCancellingControlTransferOnHubSuspendInEnumAfterAddr },
        { DsmEventHubStop ,             DsmStateCancellingControlTransferOnHubStopInEnumAfterAddr },
        { DsmEventTransferFailure ,     DsmStateReturningOperationFailureInEnumAfterAddr },
        { DsmEventTransferSuccess ,     DsmStateValidatingDeviceDescriptorAfterAddressing },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingRemainingDescriptorsAfterConfigDescriptor = {
    // State ETW Name
    DsmStateEtwGettingRemainingDescriptorsAfterConfigDescriptor,
    // State Entry Function
    HUBDSM_GettingRemainingDescriptorsAfterConfigDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmGettingRemainingDescriptorsAfterConfigDescriptor,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInEnumAfterAddr },
        { DsmEventOperationSuccess ,    DsmStateReturningOperationSuccessInEnumAfterAddr },
        { DsmEventHubStop ,             DsmStateReturningHubStopInEnumAfterAddr },
        { DsmEventHubSuspend ,          DsmStateReturningHubSuspendInEnumAfterAddr },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDeviceDetachInEnumAfterAddr },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueryingRegistryValuesForDevice = {
    // State ETW Name
    DsmStateEtwQueryingRegistryValuesForDevice,
    // State Entry Function
    HUBDSM_QueryingRegistryValuesForDevice,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfAltEnumCmdCached },
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInEnumAfterAddr },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubStopInEnumAfterAddr = {
    // State ETW Name
    DsmStateEtwReturningHubStopInEnumAfterAddr,
    // State Entry Function
    HUBDSM_ReturningHubStopInEnumAfterAddr,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubSuspendInEnumAfterAddr = {
    // State ETW Name
    DsmStateEtwReturningHubSuspendInEnumAfterAddr,
    // State Entry Function
    HUBDSM_ReturningHubSuspendInEnumAfterAddr,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureInEnumAfterAddr = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureInEnumAfterAddr,
    // State Entry Function
    HUBDSM_ReturningOperationFailureInEnumAfterAddr,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInEnumAfterAddr = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInEnumAfterAddr,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInEnumAfterAddr,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDeviceDetachInEnumAfterAddr = {
    // State ETW Name
    DsmStateEtwReturningPortDeviceDetachInEnumAfterAddr,
    // State Entry Function
    HUBDSM_ReturningPortDeviceDetachInEnumAfterAddr,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySendingAltEnumCmdInEnumAfterAddr = {
    // State ETW Name
    DsmStateEtwSendingAltEnumCmdInEnumAfterAddr,
    // State Entry Function
    HUBDSM_SendingAltEnumCmdInEnumAfterAddr,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateGettingDeviceDescriptorAfterAddressing },
        { DsmEventTransferFailure ,     DsmStateReturningOperationFailureInEnumAfterAddr },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingVersionFlagInEnumerating = {
    // State ETW Name
    DsmStateEtwSettingVersionFlagInEnumerating,
    // State Entry Function
    HUBDSM_SettingVersionFlagInEnumerating,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateQueryingRegistryValuesForDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingConfigurationDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingConfigurationDescriptor,
    // State Entry Function
    HUBDSM_ValidatingConfigurationDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInEnumAfterAddr },
        { DsmEventOperationSuccess ,    DsmStateGettingRemainingDescriptorsAfterConfigDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingDeviceDescriptorAfterAddressing = {
    // State ETW Name
    DsmStateEtwValidatingDeviceDescriptorAfterAddressing,
    // State Entry Function
    HUBDSM_ValidatingDeviceDescriptorAfterAddressing,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInEnumAfterAddr },
        { DsmEventOperationSuccess ,    DsmStateSettingVersionFlagInEnumerating },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCancellingControlTransferOnDetachInRemainingDescriptors = {
    // State ETW Name
    DsmStateEtwCancellingControlTransferOnDetachInRemainingDescriptors,
    // State Entry Function
    HUBDSM_CancellingControlTransferOnDetachInRemainingDescriptors,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateReturningPortDeviceDetachInRemainingDescriptors },
        { DsmEventTransferSuccess ,     DsmStateReturningPortDeviceDetachInRemainingDescriptors },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCancellingControlTransferOnHubStopInRemainingDescriptors = {
    // State ETW Name
    DsmStateEtwCancellingControlTransferOnHubStopInRemainingDescriptors,
    // State Entry Function
    HUBDSM_CancellingControlTransferOnHubStopInRemainingDescriptors,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateReturningHubStopInRemainingDescriptors },
        { DsmEventTransferSuccess ,     DsmStateReturningHubStopInRemainingDescriptors },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCancellingControlTransferOnHubSuspendInRemainingDescriptors = {
    // State ETW Name
    DsmStateEtwCancellingControlTransferOnHubSuspendInRemainingDescriptors,
    // State Entry Function
    HUBDSM_CancellingControlTransferOnHubSuspendInRemainingDescriptors,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateReturningHubSuspendInRemainingDescriptors },
        { DsmEventTransferSuccess ,     DsmStateReturningHubSuspendInRemainingDescriptors },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfIProductIdStringDescriptorShouldBeQueried = {
    // State ETW Name
    DsmStateEtwCheckingIfIProductIdStringDescriptorShouldBeQueried,
    // State Entry Function
    HUBDSM_CheckingIfIProductIdStringDescriptorShouldBeQueried,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateGettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch },
        { DsmEventYes ,                 DsmStateGettingProductIdStringDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfIProductIsZero = {
    // State ETW Name
    DsmStateEtwCheckingIfIProductIsZero,
    // State Entry Function
    HUBDSM_CheckingIfIProductIsZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateGettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch },
        { DsmEventNo ,                  DsmStateGettingLanguageIdStringDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingBosDescriptorSet = {
    // State ETW Name
    DsmStateEtwGettingBosDescriptorSet,
    // State Entry Function
    HUBDSM_GettingBosDescriptorSet,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmGettingBosDescriptorSet,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateGettingMSOSAndSerialNumberDescriptor },
        { DsmEventContinueIfNoSubState ,DsmStateGettingMSOSAndSerialNumberDescriptor },
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInRemainingDescriptors },
        { DsmEventPortDetachDevice ,    DsmStateCancellingControlTransferOnDetachInRemainingDescriptors },
        { DsmEventHubSuspend ,          DsmStateCancellingControlTransferOnHubSuspendInRemainingDescriptors },
        { DsmEventHubStop ,             DsmStateCancellingControlTransferOnHubStopInRemainingDescriptors },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch = {
    // State ETW Name
    DsmStateEtwGettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch,
    // State Entry Function
    HUBDSM_GettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmGettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInRemainingDescriptors },
        { DsmEventOperationSuccess ,    DsmStateReturningOperationSuccessInRemainingDescriptors },
        { DsmEventContinueIfNoSubState ,DsmStateReturningOperationSuccessInRemainingDescriptors },
        { DsmEventDeviceFailEnumeration ,DsmStateReturningOperationFailureInRemainingDescriptors },
        { DsmEventPortDetachDevice ,    DsmStateCancellingControlTransferOnDetachInRemainingDescriptors },
        { DsmEventHubSuspend ,          DsmStateCancellingControlTransferOnHubSuspendInRemainingDescriptors },
        { DsmEventHubStop ,             DsmStateCancellingControlTransferOnHubStopInRemainingDescriptors },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingLanguageIdStringDescriptor = {
    // State ETW Name
    DsmStateEtwGettingLanguageIdStringDescriptor,
    // State Entry Function
    HUBDSM_GettingLanguageIdStringDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnLanguageIdQuery },
        { DsmEventTransferSuccess ,     DsmStateValidatingLanguageIdStringDescriptor },
        { DsmEventPortDetachDevice ,    DsmStateCancellingControlTransferOnDetachInRemainingDescriptors },
        { DsmEventHubSuspend ,          DsmStateCancellingControlTransferOnHubSuspendInRemainingDescriptors },
        { DsmEventHubStop ,             DsmStateCancellingControlTransferOnHubStopInRemainingDescriptors },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingMSOSAndSerialNumberDescriptor = {
    // State ETW Name
    DsmStateEtwGettingMSOSAndSerialNumberDescriptor,
    // State Entry Function
    HUBDSM_GettingMSOSAndSerialNumberDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmGettingMSOSAndSerialNumberDescriptor,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfIProductIsZero },
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInRemainingDescriptors },
        { DsmEventPortDetachDevice ,    DsmStateCancellingControlTransferOnDetachInRemainingDescriptors },
        { DsmEventHubSuspend ,          DsmStateCancellingControlTransferOnHubSuspendInRemainingDescriptors },
        { DsmEventHubStop ,             DsmStateCancellingControlTransferOnHubStopInRemainingDescriptors },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingProductIdStringDescriptor = {
    // State ETW Name
    DsmStateEtwGettingProductIdStringDescriptor,
    // State Entry Function
    HUBDSM_GettingProductIdStringDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingProductIdStringDescriptor },
        { DsmEventPortDetachDevice ,    DsmStateCancellingControlTransferOnDetachInRemainingDescriptors },
        { DsmEventHubSuspend ,          DsmStateCancellingControlTransferOnHubSuspendInRemainingDescriptors },
        { DsmEventHubStop ,             DsmStateCancellingControlTransferOnHubStopInRemainingDescriptors },
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnProductStringQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningErrorResponseOnLanguageIdQuery = {
    // State ETW Name
    DsmStateEtwReturningErrorResponseOnLanguageIdQuery,
    // State Entry Function
    HUBDSM_ReturningErrorResponseOnLanguageIdQuery,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceIgnoreError ,   DsmStateGettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch },
        { DsmEventDeviceFailEnumeration ,DsmStateReturningOperationFailureInRemainingDescriptors },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningErrorResponseOnProductStringQuery = {
    // State ETW Name
    DsmStateEtwReturningErrorResponseOnProductStringQuery,
    // State Entry Function
    HUBDSM_ReturningErrorResponseOnProductStringQuery,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceIgnoreError ,   DsmStateGettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch },
        { DsmEventDeviceFailEnumeration ,DsmStateReturningOperationFailureInRemainingDescriptors },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubStopInRemainingDescriptors = {
    // State ETW Name
    DsmStateEtwReturningHubStopInRemainingDescriptors,
    // State Entry Function
    HUBDSM_ReturningHubStopInRemainingDescriptors,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubSuspendInRemainingDescriptors = {
    // State ETW Name
    DsmStateEtwReturningHubSuspendInRemainingDescriptors,
    // State Entry Function
    HUBDSM_ReturningHubSuspendInRemainingDescriptors,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureInRemainingDescriptors = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureInRemainingDescriptors,
    // State Entry Function
    HUBDSM_ReturningOperationFailureInRemainingDescriptors,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInRemainingDescriptors = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInRemainingDescriptors,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInRemainingDescriptors,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDeviceDetachInRemainingDescriptors = {
    // State ETW Name
    DsmStateEtwReturningPortDeviceDetachInRemainingDescriptors,
    // State Entry Function
    HUBDSM_ReturningPortDeviceDetachInRemainingDescriptors,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingLanguageIdStringDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingLanguageIdStringDescriptor,
    // State Entry Function
    HUBDSM_ValidatingLanguageIdStringDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnLanguageIdQuery },
        { DsmEventOperationSuccess ,    DsmStateCheckingIfIProductIdStringDescriptorShouldBeQueried },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingProductIdStringDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingProductIdStringDescriptor,
    // State Entry Function
    HUBDSM_ValidatingProductIdStringDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateGettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch },
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnProductStringQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDeviceSupportsContainerId = {
    // State ETW Name
    DsmStateEtwCheckingIfDeviceSupportsContainerId,
    // State Entry Function
    HUBDSM_CheckingIfDeviceSupportsContainerId,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateMarkingDeviceAsNotSupportingContainerIdBasedOnMSOSDescriptor },
        { DsmEventYes ,                 DsmStateCheckingIfIgnoreHWSerNumIsSet },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfIgnoreHWSerNumIsSet = {
    // State ETW Name
    DsmStateEtwCheckingIfIgnoreHWSerNumIsSet,
    // State Entry Function
    HUBDSM_CheckingIfIgnoreHWSerNumIsSet,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateCheckingIfSerialNumberStringIndexIsZero },
        { DsmEventYes ,                 DsmStateCheckingIfMSOSExtendedConfigDescriptorIsSupported },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfMSOSContainerIdDescriptorIsSupported = {
    // State ETW Name
    DsmStateEtwCheckingIfMSOSContainerIdDescriptorIsSupported,
    // State Entry Function
    HUBDSM_CheckingIfMSOSContainerIdDescriptorIsSupported,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateGettingMSOSContainerIdHeaderDescriptor },
        { DsmEventNo ,                  DsmStateReturningOperationSuccessInGettingDescriptorsForGreaterThan1x },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfMSOSDescriptorShouldBeQueried = {
    // State ETW Name
    DsmStateEtwCheckingIfMSOSDescriptorShouldBeQueried,
    // State Entry Function
    HUBDSM_CheckingIfMSOSDescriptorShouldBeQueried,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateCheckingIfIgnoreHWSerNumIsSet },
        { DsmEventYes ,                 DsmStateGettingMSOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfMSOSExtendedConfigDescriptorIsSupported = {
    // State ETW Name
    DsmStateEtwCheckingIfMSOSExtendedConfigDescriptorIsSupported,
    // State Entry Function
    HUBDSM_CheckingIfMSOSExtendedConfigDescriptorIsSupported,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateCheckingIfMSOSContainerIdDescriptorIsSupported },
        { DsmEventYes ,                 DsmStateGettingMSOSExtendedDescriptorHeader },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfSerialNumberStringIndexIsZero = {
    // State ETW Name
    DsmStateEtwCheckingIfSerialNumberStringIndexIsZero,
    // State Entry Function
    HUBDSM_CheckingIfSerialNumberStringIndexIsZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateCheckingIfMSOSExtendedConfigDescriptorIsSupported },
        { DsmEventNo ,                  DsmStateGettingSerialNumberStringDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingMSOSContainerIdDescriptor = {
    // State ETW Name
    DsmStateEtwGettingMSOSContainerIdDescriptor,
    // State Entry Function
    HUBDSM_GettingMSOSContainerIdDescriptor,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingMSOSContainerIdDescriptor },
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnContainerIdQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingMSOSContainerIdHeaderDescriptor = {
    // State ETW Name
    DsmStateEtwGettingMSOSContainerIdHeaderDescriptor,
    // State Entry Function
    HUBDSM_GettingMSOSContainerIdHeaderDescriptor,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingMSOSContainerIdHeaderDescriptor },
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnContainerIdHeaderQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingMSOSDescriptor = {
    // State ETW Name
    DsmStateEtwGettingMSOSDescriptor,
    // State Entry Function
    HUBDSM_GettingMSOSDescriptor,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnMSOSQuery },
        { DsmEventTransferSuccess ,     DsmStateValidatingMSOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingMSOSExtendedDescriptor = {
    // State ETW Name
    DsmStateEtwGettingMSOSExtendedDescriptor,
    // State Entry Function
    HUBDSM_GettingMSOSExtendedDescriptor,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingMSOSExtendedConfigDescriptor },
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnMSOSExtendedQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingMSOSExtendedDescriptorHeader = {
    // State ETW Name
    DsmStateEtwGettingMSOSExtendedDescriptorHeader,
    // State Entry Function
    HUBDSM_GettingMSOSExtendedDescriptorHeader,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingMSOSExtendedConfigDescriptorHeader },
        { DsmEventTransferFailure ,     DsmStateCheckingIfMSOSContainerIdDescriptorIsSupported },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingSerialNumberStringDescriptor = {
    // State ETW Name
    DsmStateEtwGettingSerialNumberStringDescriptor,
    // State Entry Function
    HUBDSM_GettingSerialNumberStringDescriptor,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnSerialNumberQuery },
        { DsmEventTransferSuccess ,     DsmStateValidatingSerialNumberStringDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryMarkDeviceAsNotSupportingMSOSDescriptor = {
    // State ETW Name
    DsmStateEtwMarkDeviceAsNotSupportingMSOSDescriptor,
    // State Entry Function
    HUBDSM_MarkDeviceAsNotSupportingMSOSDescriptor,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfIgnoreHWSerNumIsSet },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryMarkDeviceAsSupportingMSOSDescriptor = {
    // State ETW Name
    DsmStateEtwMarkDeviceAsSupportingMSOSDescriptor,
    // State Entry Function
    HUBDSM_MarkDeviceAsSupportingMSOSDescriptor,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDeviceSupportsContainerId },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryMarkingDeviceAsNotSupportingContainerId = {
    // State ETW Name
    DsmStateEtwMarkingDeviceAsNotSupportingContainerId,
    // State Entry Function
    HUBDSM_MarkingDeviceAsNotSupportingContainerId,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationSuccessInGettingDescriptorsForGreaterThan1x },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryMarkingDeviceAsNotSupportingContainerIdBasedOnMSOSDescriptor = {
    // State ETW Name
    DsmStateEtwMarkingDeviceAsNotSupportingContainerIdBasedOnMSOSDescriptor,
    // State Entry Function
    HUBDSM_MarkingDeviceAsNotSupportingContainerIdBasedOnMSOSDescriptor,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfIgnoreHWSerNumIsSet },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningErrorResponseOnContainerIdHeaderQuery = {
    // State ETW Name
    DsmStateEtwReturningErrorResponseOnContainerIdHeaderQuery,
    // State Entry Function
    HUBDSM_ReturningErrorResponseOnContainerIdHeaderQuery,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceIgnoreError ,   DsmStateMarkingDeviceAsNotSupportingContainerId },
        { DsmEventDeviceFailEnumeration ,DsmStateReturningOperationFailureInGettingDescriptorsForGreaterThan1x },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningErrorResponseOnContainerIdQuery = {
    // State ETW Name
    DsmStateEtwReturningErrorResponseOnContainerIdQuery,
    // State Entry Function
    HUBDSM_ReturningErrorResponseOnContainerIdQuery,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceFailEnumeration ,DsmStateReturningOperationFailureInGettingDescriptorsForGreaterThan1x },
        { DsmEventDeviceIgnoreError ,   DsmStateMarkingDeviceAsNotSupportingContainerId },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningErrorResponseOnMSOSExtendedHeaderQuery = {
    // State ETW Name
    DsmStateEtwReturningErrorResponseOnMSOSExtendedHeaderQuery,
    // State Entry Function
    HUBDSM_ReturningErrorResponseOnMSOSExtendedHeaderQuery,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceIgnoreError ,   DsmStateCheckingIfMSOSContainerIdDescriptorIsSupported },
        { DsmEventDeviceFailEnumeration ,DsmStateReturningOperationFailureInGettingDescriptorsForGreaterThan1x },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningErrorResponseOnMSOSExtendedQuery = {
    // State ETW Name
    DsmStateEtwReturningErrorResponseOnMSOSExtendedQuery,
    // State Entry Function
    HUBDSM_ReturningErrorResponseOnMSOSExtendedQuery,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceIgnoreError ,   DsmStateCheckingIfMSOSContainerIdDescriptorIsSupported },
        { DsmEventDeviceFailEnumeration ,DsmStateReturningOperationFailureInGettingDescriptorsForGreaterThan1x },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningErrorResponseOnMSOSQuery = {
    // State ETW Name
    DsmStateEtwReturningErrorResponseOnMSOSQuery,
    // State Entry Function
    HUBDSM_ReturningErrorResponseOnMSOSQuery,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceIgnoreError ,   DsmStateMarkDeviceAsNotSupportingMSOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningErrorResponseOnSerialNumberQuery = {
    // State ETW Name
    DsmStateEtwReturningErrorResponseOnSerialNumberQuery,
    // State Entry Function
    HUBDSM_ReturningErrorResponseOnSerialNumberQuery,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceIgnoreError ,   DsmStateCheckingIfMSOSExtendedConfigDescriptorIsSupported },
        { DsmEventDeviceFailEnumeration ,DsmStateReturningOperationFailureInGettingDescriptorsForGreaterThan1x },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureInGettingDescriptorsForGreaterThan1x = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureInGettingDescriptorsForGreaterThan1x,
    // State Entry Function
    HUBDSM_ReturningOperationFailureInGettingDescriptorsForGreaterThan1x,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInGettingDescriptorsForGreaterThan1x = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInGettingDescriptorsForGreaterThan1x,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInGettingDescriptorsForGreaterThan1x,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingMSOSContainerIdDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingMSOSContainerIdDescriptor,
    // State Entry Function
    HUBDSM_ValidatingMSOSContainerIdDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnContainerIdQuery },
        { DsmEventOperationSuccess ,    DsmStateReturningOperationSuccessInGettingDescriptorsForGreaterThan1x },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingMSOSContainerIdHeaderDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingMSOSContainerIdHeaderDescriptor,
    // State Entry Function
    HUBDSM_ValidatingMSOSContainerIdHeaderDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateGettingMSOSContainerIdDescriptor },
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnContainerIdHeaderQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingMSOSDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingMSOSDescriptor,
    // State Entry Function
    HUBDSM_ValidatingMSOSDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnMSOSQuery },
        { DsmEventOperationSuccess ,    DsmStateMarkDeviceAsSupportingMSOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingMSOSExtendedConfigDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingMSOSExtendedConfigDescriptor,
    // State Entry Function
    HUBDSM_ValidatingMSOSExtendedConfigDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnMSOSExtendedQuery },
        { DsmEventOperationSuccess ,    DsmStateCheckingIfMSOSContainerIdDescriptorIsSupported },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingMSOSExtendedConfigDescriptorHeader = {
    // State ETW Name
    DsmStateEtwValidatingMSOSExtendedConfigDescriptorHeader,
    // State Entry Function
    HUBDSM_ValidatingMSOSExtendedConfigDescriptorHeader,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateGettingMSOSExtendedDescriptor },
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnMSOSExtendedHeaderQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingSerialNumberStringDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingSerialNumberStringDescriptor,
    // State Entry Function
    HUBDSM_ValidatingSerialNumberStringDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfMSOSExtendedConfigDescriptorIsSupported },
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnSerialNumberQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfIgnoreHWSerNumIsSetFor1xDevice = {
    // State ETW Name
    DsmStateEtwCheckingIfIgnoreHWSerNumIsSetFor1xDevice,
    // State Entry Function
    HUBDSM_CheckingIfIgnoreHWSerNumIsSetFor1xDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateReturningOperationSuccessInGettingSerialNumberFor1x },
        { DsmEventNo ,                  DsmStateCheckingIfSerialNumberStringIndexIsZeroFor1xDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfSerialNumberStringIndexIsZeroFor1xDevice = {
    // State ETW Name
    DsmStateEtwCheckingIfSerialNumberStringIndexIsZeroFor1xDevice,
    // State Entry Function
    HUBDSM_CheckingIfSerialNumberStringIndexIsZeroFor1xDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateReturningOperationSuccessInGettingSerialNumberFor1x },
        { DsmEventNo ,                  DsmStateGettingSerialNumberStringDescriptorFor1xDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingSerialNumberStringDescriptorFor1xDevice = {
    // State ETW Name
    DsmStateEtwGettingSerialNumberStringDescriptorFor1xDevice,
    // State Entry Function
    HUBDSM_GettingSerialNumberStringDescriptorFor1xDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnSerialNumberQueryFor1xDevice },
        { DsmEventTransferSuccess ,     DsmStateValidatingSerialNumberStringDescriptorFor1xDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningErrorResponseOnSerialNumberQueryFor1xDevice = {
    // State ETW Name
    DsmStateEtwReturningErrorResponseOnSerialNumberQueryFor1xDevice,
    // State Entry Function
    HUBDSM_ReturningErrorResponseOnSerialNumberQueryFor1xDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceIgnoreError ,   DsmStateReturningOperationSuccessInGettingSerialNumberFor1x },
        { DsmEventDeviceFailEnumeration ,DsmStateReturningOperationFailureInGettingSerialNumberFor1x },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureInGettingSerialNumberFor1x = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureInGettingSerialNumberFor1x,
    // State Entry Function
    HUBDSM_ReturningOperationFailureInGettingSerialNumberFor1x,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInGettingSerialNumberFor1x = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInGettingSerialNumberFor1x,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInGettingSerialNumberFor1x,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingSerialNumberStringDescriptorFor1xDevice = {
    // State ETW Name
    DsmStateEtwValidatingSerialNumberStringDescriptorFor1xDevice,
    // State Entry Function
    HUBDSM_ValidatingSerialNumberStringDescriptorFor1xDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationSuccessInGettingSerialNumberFor1x },
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnSerialNumberQueryFor1xDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfWaitRequiredAfterSetAddress = {
    // State ETW Name
    DsmStateEtwCheckingIfWaitRequiredAfterSetAddress,
    // State Entry Function
    HUBDSM_CheckingIfWaitRequiredAfterSetAddress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateReturningOperationSuccessInSettingAddress },
        { DsmEventYes ,                 DsmStateWaitingForTimerAfterSetAddress },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubStopInSettingAddress = {
    // State ETW Name
    DsmStateEtwReturningHubStopInSettingAddress,
    // State Entry Function
    HUBDSM_ReturningHubStopInSettingAddress,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubSuspendInSettingAddress = {
    // State ETW Name
    DsmStateEtwReturningHubSuspendInSettingAddress,
    // State Entry Function
    HUBDSM_ReturningHubSuspendInSettingAddress,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureInSettingAddress = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureInSettingAddress,
    // State Entry Function
    HUBDSM_ReturningOperationFailureInSettingAddress,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInSettingAddress = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInSettingAddress,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInSettingAddress,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDetachDeviceInSettingAddress = {
    // State ETW Name
    DsmStateEtwReturningPortDetachDeviceInSettingAddress,
    // State Entry Function
    HUBDSM_ReturningPortDetachDeviceInSettingAddress,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingNonZeroAddress = {
    // State ETW Name
    DsmStateEtwSettingNonZeroAddress,
    // State Entry Function
    HUBDSM_SettingNonZeroAddress,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlFailure ,     DsmStateReturningOperationFailureInSettingAddress },
        { DsmEventUCXIoctlSuccess ,     DsmStateCheckingIfWaitRequiredAfterSetAddress },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppingTimerOnHubStopInSettingAddress = {
    // State ETW Name
    DsmStateEtwStoppingTimerOnHubStopInSettingAddress,
    // State Entry Function
    HUBDSM_StoppingTimerOnHubStopInSettingAddress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerOnHubStopInSettingAddress },
        { DsmEventOperationSuccess ,    DsmStateReturningHubStopInSettingAddress },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppingTimerOnHubSuspendInSettingAddress = {
    // State ETW Name
    DsmStateEtwStoppingTimerOnHubSuspendInSettingAddress,
    // State Entry Function
    HUBDSM_StoppingTimerOnHubSuspendInSettingAddress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerOnHubSuspendInSettingAddress },
        { DsmEventOperationSuccess ,    DsmStateReturningHubSuspendInSettingAddress },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppingTimerOnPortDetachInSettingAddress = {
    // State ETW Name
    DsmStateEtwStoppingTimerOnPortDetachInSettingAddress,
    // State Entry Function
    HUBDSM_StoppingTimerOnPortDetachInSettingAddress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerOnPortDetachInSettingAddress },
        { DsmEventOperationSuccess ,    DsmStateReturningPortDetachDeviceInSettingAddress },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerAfterSetAddress = {
    // State ETW Name
    DsmStateEtwWaitingForTimerAfterSetAddress,
    // State Entry Function
    HUBDSM_WaitingForTimerAfterSetAddress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubStop ,             DsmStateStoppingTimerOnHubStopInSettingAddress },
        { DsmEventHubSuspend ,          DsmStateStoppingTimerOnHubSuspendInSettingAddress },
        { DsmEventPortDetachDevice ,    DsmStateStoppingTimerOnPortDetachInSettingAddress },
        { DsmEventTimerFired ,          DsmStateReturningOperationSuccessInSettingAddress },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerOnHubStopInSettingAddress = {
    // State ETW Name
    DsmStateEtwWaitingForTimerOnHubStopInSettingAddress,
    // State Entry Function
    HUBDSM_WaitingForTimerOnHubStopInSettingAddress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateWaitingForTimerOnPortDetachInSettingAddress },
        { DsmEventTimerFired ,          DsmStateReturningHubStopInSettingAddress },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerOnHubSuspendInSettingAddress = {
    // State ETW Name
    DsmStateEtwWaitingForTimerOnHubSuspendInSettingAddress,
    // State Entry Function
    HUBDSM_WaitingForTimerOnHubSuspendInSettingAddress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateWaitingForTimerOnPortDetachInSettingAddress },
        { DsmEventTimerFired ,          DsmStateReturningHubSuspendInSettingAddress },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerOnPortDetachInSettingAddress = {
    // State ETW Name
    DsmStateEtwWaitingForTimerOnPortDetachInSettingAddress,
    // State Entry Function
    HUBDSM_WaitingForTimerOnPortDetachInSettingAddress,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventTimerFired ,          DsmStateReturningPortDetachDeviceInSettingAddress },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingBytesReturnedInAlternateConfigDescriptor = {
    // State ETW Name
    DsmStateEtwCheckingBytesReturnedInAlternateConfigDescriptor,
    // State Entry Function
    HUBDSM_CheckingBytesReturnedInAlternateConfigDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateGettingAlternateConfigurationDescriptorWithReturnedLength },
        { DsmEventYes ,                 DsmStateValidatingAlternateConfigurationDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfAltEnumCommandNeeded = {
    // State ETW Name
    DsmStateEtwCheckingIfAltEnumCommandNeeded,
    // State Entry Function
    HUBDSM_CheckingIfAltEnumCommandNeeded,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateSendingMsOs20AlternateEnumerationCommand },
        { DsmEventNo ,                  DsmStateIsMsOs20DescriptorSupported },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfAlternateBOSDescriptorQueryShouldBeSkipped = {
    // State ETW Name
    DsmStateEtwCheckingIfAlternateBOSDescriptorQueryShouldBeSkipped,
    // State Entry Function
    HUBDSM_CheckingIfAlternateBOSDescriptorQueryShouldBeSkipped,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateGettingAlternateBOSDescriptorHeader },
        { DsmEventYes ,                 DsmStateReturningOperationSuccessInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfAlternateModeStringDescriptorShouldBeQueried = {
    // State ETW Name
    DsmStateEtwCheckingIfAlternateModeStringDescriptorShouldBeQueried,
    // State Entry Function
    HUBDSM_CheckingIfAlternateModeStringDescriptorShouldBeQueried,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateCheckingIfAltEnumCommandNeeded },
        { DsmEventYes ,                 DsmStateGettingAlternateModeStringDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfBillboardBOSDescriptorIsPresent = {
    // State ETW Name
    DsmStateEtwCheckingIfBillboardBOSDescriptorIsPresent,
    // State Entry Function
    HUBDSM_CheckingIfBillboardBOSDescriptorIsPresent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateCheckingIfBillboardStringDescriptorShouldBeQueried },
        { DsmEventNo ,                  DsmStateCheckingIfAltEnumCommandNeeded },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfBillboardStringDescriptorShouldBeQueried = {
    // State ETW Name
    DsmStateEtwCheckingIfBillboardStringDescriptorShouldBeQueried,
    // State Entry Function
    HUBDSM_CheckingIfBillboardStringDescriptorShouldBeQueried,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateCheckingIfAlternateModeStringDescriptorShouldBeQueried },
        { DsmEventYes ,                 DsmStateGettingBillboardStringDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfBOSDescriptorQueryShouldBeSkipped = {
    // State ETW Name
    DsmStateEtwCheckingIfBOSDescriptorQueryShouldBeSkipped,
    // State Entry Function
    HUBDSM_CheckingIfBOSDescriptorQueryShouldBeSkipped,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateGettingBOSDescriptorHeader },
        { DsmEventYes ,                 DsmStateReturningOperationSuccessInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfCompleteAlternateBOSDescriptorWasRetrieved = {
    // State ETW Name
    DsmStateEtwCheckingIfCompleteAlternateBOSDescriptorWasRetrieved,
    // State Entry Function
    HUBDSM_CheckingIfCompleteAlternateBOSDescriptorWasRetrieved,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateValidatingAlternateBOSDescriptor },
        { DsmEventNo ,                  DsmStateGettingAlternateBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfCompleteBOSDescriptorWasRetrieved = {
    // State ETW Name
    DsmStateEtwCheckingIfCompleteBOSDescriptorWasRetrieved,
    // State Entry Function
    HUBDSM_CheckingIfCompleteBOSDescriptorWasRetrieved,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateGettingBOSDescriptor },
        { DsmEventYes ,                 DsmStateValidatingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDualRoleFeaturesSupported = {
    // State ETW Name
    DsmStateEtwCheckingIfDualRoleFeaturesSupported,
    // State Entry Function
    HUBDSM_CheckingIfDualRoleFeaturesSupported,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateCheckingIfBillboardBOSDescriptorIsPresent },
        { DsmEventYes ,                 DsmStateSendingUsbFeaturesVendorCmd },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGetMsOs20DescriptorSet = {
    // State ETW Name
    DsmStateEtwGetMsOs20DescriptorSet,
    // State Entry Function
    HUBDSM_GetMsOs20DescriptorSet,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingMsOs20DescriptorSetIfPresent },
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnBOSDescriptorQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingAlternateBOSDescriptor = {
    // State ETW Name
    DsmStateEtwGettingAlternateBOSDescriptor,
    // State Entry Function
    HUBDSM_GettingAlternateBOSDescriptor,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingAlternateBOSDescriptor },
        { DsmEventTransferFailure ,     DsmStateReturningOperationFailureInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingAlternateBOSDescriptorHeader = {
    // State ETW Name
    DsmStateEtwGettingAlternateBOSDescriptorHeader,
    // State Entry Function
    HUBDSM_GettingAlternateBOSDescriptorHeader,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingAlternateBOSDescriptorHeader },
        { DsmEventTransferFailure ,     DsmStateReturningOperationFailureInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingAlternateConfigurationDescriptorWithDefaultLength = {
    // State ETW Name
    DsmStateEtwGettingAlternateConfigurationDescriptorWithDefaultLength,
    // State Entry Function
    HUBDSM_GettingAlternateConfigurationDescriptorWithDefaultLength,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateCheckingBytesReturnedInAlternateConfigDescriptor },
        { DsmEventTransferFailure ,     DsmStateReturningOperationFailureInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingAlternateConfigurationDescriptorWithReturnedLength = {
    // State ETW Name
    DsmStateEtwGettingAlternateConfigurationDescriptorWithReturnedLength,
    // State Entry Function
    HUBDSM_GettingAlternateConfigurationDescriptorWithReturnedLength,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingAlternateConfigurationDescriptor },
        { DsmEventTransferFailure ,     DsmStateReturningOperationFailureInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingAlternateDeviceDescriptor = {
    // State ETW Name
    DsmStateEtwGettingAlternateDeviceDescriptor,
    // State Entry Function
    HUBDSM_GettingAlternateDeviceDescriptor,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingAlternateDeviceDescriptor },
        { DsmEventTransferFailure ,     DsmStateReturningOperationFailureInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingAlternateModeStringDescriptor = {
    // State ETW Name
    DsmStateEtwGettingAlternateModeStringDescriptor,
    // State Entry Function
    HUBDSM_GettingAlternateModeStringDescriptor,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnBOSDescriptorQuery },
        { DsmEventTransferSuccess ,     DsmStateValidatingAndLoggingAlternateModeStringDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingBillboardStringDescriptor = {
    // State ETW Name
    DsmStateEtwGettingBillboardStringDescriptor,
    // State Entry Function
    HUBDSM_GettingBillboardStringDescriptor,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingAndLoggingBillboardStringDescriptor },
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnBOSDescriptorQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingBOSDescriptor = {
    // State ETW Name
    DsmStateEtwGettingBOSDescriptor,
    // State Entry Function
    HUBDSM_GettingBOSDescriptor,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingBOSDescriptor },
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnBOSDescriptorQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingBOSDescriptorHeader = {
    // State ETW Name
    DsmStateEtwGettingBOSDescriptorHeader,
    // State Entry Function
    HUBDSM_GettingBOSDescriptorHeader,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingBOSDescriptorHeader },
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnBOSDescriptorHeaderQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryIsMsOs20DescriptorSupported = {
    // State ETW Name
    DsmStateEtwIsMsOs20DescriptorSupported,
    // State Entry Function
    HUBDSM_IsMsOs20DescriptorSupported,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateGetMsOs20DescriptorSet },
        { DsmEventNo ,                  DsmStateReturningOperationSuccessInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueryingRegistryValuesForAlternateDeviceEnumeration = {
    // State ETW Name
    DsmStateEtwQueryingRegistryValuesForAlternateDeviceEnumeration,
    // State Entry Function
    HUBDSM_QueryingRegistryValuesForAlternateDeviceEnumeration,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateGettingAlternateConfigurationDescriptorWithDefaultLength },
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningErrorResponseOnBOSDescriptorHeaderQuery = {
    // State ETW Name
    DsmStateEtwReturningErrorResponseOnBOSDescriptorHeaderQuery,
    // State Entry Function
    HUBDSM_ReturningErrorResponseOnBOSDescriptorHeaderQuery,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceFailEnumeration ,DsmStateReturningOperationFailureInGettingBOSDescriptor },
        { DsmEventDeviceIgnoreError ,   DsmStateReturningOperationSuccessInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningErrorResponseOnBOSDescriptorQuery = {
    // State ETW Name
    DsmStateEtwReturningErrorResponseOnBOSDescriptorQuery,
    // State Entry Function
    HUBDSM_ReturningErrorResponseOnBOSDescriptorQuery,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceFailEnumeration ,DsmStateReturningOperationFailureInGettingBOSDescriptor },
        { DsmEventDeviceIgnoreError ,   DsmStateReturningOperationSuccessInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureInGettingBOSDescriptor = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureInGettingBOSDescriptor,
    // State Entry Function
    HUBDSM_ReturningOperationFailureInGettingBOSDescriptor,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInGettingBOSDescriptor = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInGettingBOSDescriptor,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInGettingBOSDescriptor,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySendingMsOs20AlternateEnumerationCommand = {
    // State ETW Name
    DsmStateEtwSendingMsOs20AlternateEnumerationCommand,
    // State Entry Function
    HUBDSM_SendingMsOs20AlternateEnumerationCommand,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateGettingAlternateDeviceDescriptor },
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnBOSDescriptorQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySendingUsbFeaturesVendorCmd = {
    // State ETW Name
    DsmStateEtwSendingUsbFeaturesVendorCmd,
    // State Entry Function
    HUBDSM_SendingUsbFeaturesVendorCmd,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateCheckingIfBillboardBOSDescriptorIsPresent },
        { DsmEventTransferFailure ,     DsmStateReturningErrorResponseOnBOSDescriptorQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingVersionFlagInAlternateEnumeration = {
    // State ETW Name
    DsmStateEtwSettingVersionFlagInAlternateEnumeration,
    // State Entry Function
    HUBDSM_SettingVersionFlagInAlternateEnumeration,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateQueryingRegistryValuesForAlternateDeviceEnumeration },
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingAlternateBOSDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingAlternateBOSDescriptor,
    // State Entry Function
    HUBDSM_ValidatingAlternateBOSDescriptor,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateIsMsOs20DescriptorSupported },
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingAlternateBOSDescriptorHeader = {
    // State ETW Name
    DsmStateEtwValidatingAlternateBOSDescriptorHeader,
    // State Entry Function
    HUBDSM_ValidatingAlternateBOSDescriptorHeader,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfCompleteAlternateBOSDescriptorWasRetrieved },
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingAlternateConfigurationDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingAlternateConfigurationDescriptor,
    // State Entry Function
    HUBDSM_ValidatingAlternateConfigurationDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInGettingBOSDescriptor },
        { DsmEventOperationSuccess ,    DsmStateCheckingIfAlternateBOSDescriptorQueryShouldBeSkipped },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingAlternateDeviceDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingAlternateDeviceDescriptor,
    // State Entry Function
    HUBDSM_ValidatingAlternateDeviceDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateSettingVersionFlagInAlternateEnumeration },
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInGettingBOSDescriptor },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingAndLoggingAlternateModeStringDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingAndLoggingAlternateModeStringDescriptor,
    // State Entry Function
    HUBDSM_ValidatingAndLoggingAlternateModeStringDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfAlternateModeStringDescriptorShouldBeQueried },
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnBOSDescriptorQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingAndLoggingBillboardStringDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingAndLoggingBillboardStringDescriptor,
    // State Entry Function
    HUBDSM_ValidatingAndLoggingBillboardStringDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfAlternateModeStringDescriptorShouldBeQueried },
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnBOSDescriptorQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingBOSDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingBOSDescriptor,
    // State Entry Function
    HUBDSM_ValidatingBOSDescriptor,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDualRoleFeaturesSupported },
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnBOSDescriptorQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingBOSDescriptorHeader = {
    // State ETW Name
    DsmStateEtwValidatingBOSDescriptorHeader,
    // State Entry Function
    HUBDSM_ValidatingBOSDescriptorHeader,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfCompleteBOSDescriptorWasRetrieved },
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnBOSDescriptorHeaderQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingMsOs20DescriptorSetIfPresent = {
    // State ETW Name
    DsmStateEtwValidatingMsOs20DescriptorSetIfPresent,
    // State Entry Function
    HUBDSM_ValidatingMsOs20DescriptorSetIfPresent,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationSuccessInGettingBOSDescriptor },
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnBOSDescriptorQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingDeviceQualifierDescriptor = {
    // State ETW Name
    DsmStateEtwGettingDeviceQualifierDescriptor,
    // State Entry Function
    HUBDSM_GettingDeviceQualifierDescriptor,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingDeviceQualifierDescriptor },
        { DsmEventTransferFailure ,     DsmStateReturningOperationSuccessInCheckingFor20FullSpeed },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningErrorResponseOnDeviceQualifierQuery = {
    // State ETW Name
    DsmStateEtwReturningErrorResponseOnDeviceQualifierQuery,
    // State Entry Function
    HUBDSM_ReturningErrorResponseOnDeviceQualifierQuery,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceIgnoreError ,   DsmStateReturningOperationSuccessInCheckingFor20FullSpeed },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInCheckingFor20FullSpeed = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInCheckingFor20FullSpeed,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInCheckingFor20FullSpeed,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingDeviceQualifierDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingDeviceQualifierDescriptor,
    // State Entry Function
    HUBDSM_ValidatingDeviceQualifierDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationSuccessInCheckingFor20FullSpeed },
        { DsmEventOperationFailure ,    DsmStateReturningErrorResponseOnDeviceQualifierQuery },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAddingDeviceToGlobalChildList = {
    // State ETW Name
    DsmStateEtwAddingDeviceToGlobalChildList,
    // State Entry Function
    HUBDSM_AddingDeviceToGlobalChildList,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCreatingChildPDOAndReportingToPnp },
        { DsmEventOperationPending ,    DsmStateWaitingForDuplicateDeviceToGoAway },
        { DsmEventOperationFailure ,    DsmStateCheckingIfAddDeviceToGlobalListRetryCountExceededLimit },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfAddDeviceToGlobalListRetryCountExceededLimit = {
    // State ETW Name
    DsmStateEtwCheckingIfAddDeviceToGlobalListRetryCountExceededLimit,
    // State Entry Function
    HUBDSM_CheckingIfAddDeviceToGlobalListRetryCountExceededLimit,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateWaitingForDuplicateDeviceToGoAway },
        { DsmEventYes ,                 DsmStateDiscardSerialNumber },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCreatingChildPDOAndReportingToPnp = {
    // State ETW Name
    DsmStateEtwCreatingChildPDOAndReportingToPnp,
    // State Entry Function
    HUBDSM_CreatingChildPDOAndReportingToPnp,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationSuccessInReportingToPnp },
        { DsmEventOperationFailure ,    DsmStateRequestingDeviceCycleInReportingToPnp },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDiscardSerialNumber = {
    // State ETW Name
    DsmStateEtwDiscardSerialNumber,
    // State Entry Function
    HUBDSM_DiscardSerialNumber,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCreatingChildPDOAndReportingToPnp },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryInitializingAddDeviceToGlobalListRetryCount = {
    // State ETW Name
    DsmStateEtwInitializingAddDeviceToGlobalListRetryCount,
    // State Entry Function
    HUBDSM_InitializingAddDeviceToGlobalListRetryCount,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateAddingDeviceToGlobalChildList },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryRequestingDeviceCycleInReportingToPnp = {
    // State ETW Name
    DsmStateEtwRequestingDeviceCycleInReportingToPnp,
    // State Entry Function
    HUBDSM_RequestingDeviceCycleInReportingToPnp,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInReportingToPnp = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInReportingToPnp,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInReportingToPnp,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDetachDevice = {
    // State ETW Name
    DsmStateEtwReturningPortDetachDevice,
    // State Entry Function
    HUBDSM_ReturningPortDetachDevice,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppingTimerAndRequestingCycleOnStopSuspend = {
    // State ETW Name
    DsmStateEtwStoppingTimerAndRequestingCycleOnStopSuspend,
    // State Entry Function
    HUBDSM_StoppingTimerAndRequestingCycleOnStopSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForDetachOrTimerInReportingToPnp },
        { DsmEventOperationSuccess ,    DsmStateWaitingForDetachInReportingToPnp },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppingTimerOnDetachInReportingToPnp = {
    // State ETW Name
    DsmStateEtwStoppingTimerOnDetachInReportingToPnp,
    // State Entry Function
    HUBDSM_StoppingTimerOnDetachInReportingToPnp,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningPortDetachDevice },
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerToFlushOnDetachInReportingToPnp },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDetachInReportingToPnp = {
    // State ETW Name
    DsmStateEtwWaitingForDetachInReportingToPnp,
    // State Entry Function
    HUBDSM_WaitingForDetachInReportingToPnp,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDetachOrTimerInReportingToPnp = {
    // State ETW Name
    DsmStateEtwWaitingForDetachOrTimerInReportingToPnp,
    // State Entry Function
    HUBDSM_WaitingForDetachOrTimerInReportingToPnp,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateWaitingForTimerToFlushOnDetachInReportingToPnp },
        { DsmEventTimerFired ,          DsmStateWaitingForDetachInReportingToPnp },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDuplicateDeviceToGoAway = {
    // State ETW Name
    DsmStateEtwWaitingForDuplicateDeviceToGoAway,
    // State Entry Function
    HUBDSM_WaitingForDuplicateDeviceToGoAway,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubStop ,             DsmStateStoppingTimerAndRequestingCycleOnStopSuspend },
        { DsmEventHubSuspend ,          DsmStateStoppingTimerAndRequestingCycleOnStopSuspend },
        { DsmEventTimerFired ,          DsmStateAddingDeviceToGlobalChildList },
        { DsmEventPortDetachDevice ,    DsmStateStoppingTimerOnDetachInReportingToPnp },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerToFlushOnDetachInReportingToPnp = {
    // State ETW Name
    DsmStateEtwWaitingForTimerToFlushOnDetachInReportingToPnp,
    // State Entry Function
    HUBDSM_WaitingForTimerToFlushOnDetachInReportingToPnp,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventTimerFired ,          DsmStateReturningPortDetachDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCreatingUnknownChildPDOAndReportingToPnp = {
    // State ETW Name
    DsmStateEtwCreatingUnknownChildPDOAndReportingToPnp,
    // State Entry Function
    HUBDSM_CreatingUnknownChildPDOAndReportingToPnp,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateRequestingDeviceCycleForUnknownDevice },
        { DsmEventOperationSuccess ,    DsmStateMarkingUnknownDeviceAsFailed },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryMarkingUnknownDeviceAsFailed = {
    // State ETW Name
    DsmStateEtwMarkingUnknownDeviceAsFailed,
    // State Entry Function
    HUBDSM_MarkingUnknownDeviceAsFailed,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationSuccessInReportingUnknownDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryRequestingDeviceCycleForUnknownDevice = {
    // State ETW Name
    DsmStateEtwRequestingDeviceCycleForUnknownDevice,
    // State Entry Function
    HUBDSM_RequestingDeviceCycleForUnknownDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInReportingUnknownDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInReportingUnknownDevice = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInReportingUnknownDevice,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInReportingUnknownDevice,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDetachDeviceInReportingUnknownDevice = {
    // State ETW Name
    DsmStateEtwReturningPortDetachDeviceInReportingUnknownDevice,
    // State Entry Function
    HUBDSM_ReturningPortDetachDeviceInReportingUnknownDevice,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAckingResumed = {
    // State ETW Name
    DsmStateEtwAckingResumed,
    // State Entry Function
    HUBDSM_AckingResumed,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForHubResumeInEnabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAcquiringPowerReferenceInEnabled = {
    // State ETW Name
    DsmStateEtwAcquiringPowerReferenceInEnabled,
    // State Entry Function
    HUBDSM_AcquiringPowerReferenceInEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDeviceNeedsResetOnResumeInSxOnEnabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAcquiringPowerReferenceInSuspended = {
    // State ETW Name
    DsmStateEtwAcquiringPowerReferenceInSuspended,
    // State Entry Function
    HUBDSM_AcquiringPowerReferenceInSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDeviceNeedsResetOnResumeInSxOnSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAcquiringPowerReferenceInSuspendedOnResumeWithReset = {
    // State ETW Name
    DsmStateEtwAcquiringPowerReferenceInSuspendedOnResumeWithReset,
    // State Entry Function
    HUBDSM_AcquiringPowerReferenceInSuspendedOnResumeWithReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateResumingBeforeResetting },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAcquiringPowerReferenceOnHubResume = {
    // State ETW Name
    DsmStateEtwAcquiringPowerReferenceOnHubResume,
    // State Entry Function
    HUBDSM_AcquiringPowerReferenceOnHubResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForPSMSyncUp },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAcquiringPowerReferenceOnHubResumeWithReenumRequired = {
    // State ETW Name
    DsmStateEtwAcquiringPowerReferenceOnHubResumeWithReenumRequired,
    // State Entry Function
    HUBDSM_AcquiringPowerReferenceOnHubResumeWithReenumRequired,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningDeviceRenumerationOnHubResume },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAcquiringPowerReferenceOnHubResumeWtihReset = {
    // State ETW Name
    DsmStateEtwAcquiringPowerReferenceOnHubResumeWtihReset,
    // State Entry Function
    HUBDSM_AcquiringPowerReferenceOnHubResumeWtihReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForPSMSyncUpOnResumeWithReset },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAcquiringPowerReferenceOnResumeInS0 = {
    // State ETW Name
    DsmStateEtwAcquiringPowerReferenceOnResumeInS0,
    // State Entry Function
    HUBDSM_AcquiringPowerReferenceOnResumeInS0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningDeviceSuspendedOnHubResume },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDeviceNeedsResetOnResumeInSxOnEnabled = {
    // State ETW Name
    DsmStateEtwCheckingIfDeviceNeedsResetOnResumeInSxOnEnabled,
    // State Entry Function
    HUBDSM_CheckingIfDeviceNeedsResetOnResumeInSxOnEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateReturningDeviceResumedOnHubResume },
        { DsmEventYes ,                 DsmStateReturningDeviceRenumerationOnHubResume },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDeviceNeedsResetOnResumeInSxOnSuspended = {
    // State ETW Name
    DsmStateEtwCheckingIfDeviceNeedsResetOnResumeInSxOnSuspended,
    // State Entry Function
    HUBDSM_CheckingIfDeviceNeedsResetOnResumeInSxOnSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateReturningDeviceSuspendedOnHubResume },
        { DsmEventYes ,                 DsmStateResumingBeforeResetting },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingUnexpectedD0ExitInHubSuspended = {
    // State ETW Name
    DsmStateEtwCompletingUnexpectedD0ExitInHubSuspended,
    // State Entry Function
    HUBDSM_CompletingUnexpectedD0ExitInHubSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateSuspendedInHubSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisabledOrFailedInHubSuspend = {
    // State ETW Name
    DsmStateEtwDisabledOrFailedInHubSuspend,
    // State Entry Function
    HUBDSM_DisabledOrFailedInHubSuspend,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubResume ,           DsmStateAcquiringPowerReferenceOnHubResumeWtihReset },
        { DsmEventPortStateEnabled ,    DsmStateWaitingForHubResumeWithRenumRequired },
        { DsmEventHubResumeInS0 ,       DsmStateAcquiringPowerReferenceOnHubResumeWithReenumRequired },
        { DsmEventPortStateDisabled ,   DsmStateWaitingForHubResumeWithRenumRequired },
        { DsmEventPortStateSuspended ,  DsmStateWaitingForHubResumeWithRenumRequired },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateWaitingForHubResumeWithRenumRequired },
        { DsmEventHubResumeWithReset ,  DsmStateAcquiringPowerReferenceOnHubResumeWtihReset },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingPortOnPortEnabledInPendingHubStop = {
    // State ETW Name
    DsmStateEtwDisablingPortOnPortEnabledInPendingHubStop,
    // State Entry Function
    HUBDSM_DisablingPortOnPortEnabledInPendingHubStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetach },
        { DsmEventPortFailed ,          DsmStateReturningHubStopWithReferenceAcquiredInHubSuspended },
        { DsmEventPortDisabled ,        DsmStateReturningHubStopWithReferenceAcquiredInHubSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingReferenceOnHubSuspendAfterPSMSyncUp = {
    // State ETW Name
    DsmStateEtwReleasingReferenceOnHubSuspendAfterPSMSyncUp,
    // State Entry Function
    HUBDSM_ReleasingReferenceOnHubSuspendAfterPSMSyncUp,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateSuspendedInHubSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed = {
    // State ETW Name
    DsmStateEtwReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed,
    // State Entry Function
    HUBDSM_ReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDisabledOrFailedInHubSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingReferenceOnHubSuspendOnDetachAfterPSMSyncUp = {
    // State ETW Name
    DsmStateEtwReleasingReferenceOnHubSuspendOnDetachAfterPSMSyncUp,
    // State Entry Function
    HUBDSM_ReleasingReferenceOnHubSuspendOnDetachAfterPSMSyncUp,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningPortDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryResumingBeforeResetting = {
    // State ETW Name
    DsmStateEtwResumingBeforeResetting,
    // State Entry Function
    HUBDSM_ResumingBeforeResetting,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResumed ,         DsmStateReturningDeviceRenumerationOnHubResume },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateReturningDeviceRenumerationOnHubResume },
        { DsmEventPortResumeTimedOut ,  DsmStateReturningDeviceRenumerationOnHubResume },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetach },
        { DsmEventPortFailed ,          DsmStateReturningDeviceRenumerationOnHubResume },
        { DsmEventHubStop ,             DsmStateWaitingForResumeResponseFromPortOnHubStop },
        { DsmEventHubSuspend ,          DsmStateWaitingForResumeResponseFromPortOnHubSuspend },
        { DsmEventPortStateDisabled ,   DsmStateReturningDeviceRenumerationOnHubResume },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningDeviceRenumerationOnHubResume = {
    // State ETW Name
    DsmStateEtwReturningDeviceRenumerationOnHubResume,
    // State Entry Function
    HUBDSM_ReturningDeviceRenumerationOnHubResume,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningDeviceResumedOnHubResume = {
    // State ETW Name
    DsmStateEtwReturningDeviceResumedOnHubResume,
    // State Entry Function
    HUBDSM_ReturningDeviceResumedOnHubResume,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningDeviceSuspendedOnHubResume = {
    // State ETW Name
    DsmStateEtwReturningDeviceSuspendedOnHubResume,
    // State Entry Function
    HUBDSM_ReturningDeviceSuspendedOnHubResume,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubStopWithReferenceAcquiredInHubSuspended = {
    // State ETW Name
    DsmStateEtwReturningHubStopWithReferenceAcquiredInHubSuspended,
    // State Entry Function
    HUBDSM_ReturningHubStopWithReferenceAcquiredInHubSuspended,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDetach = {
    // State ETW Name
    DsmStateEtwReturningPortDetach,
    // State Entry Function
    HUBDSM_ReturningPortDetach,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySuspendedInHubSuspend = {
    // State ETW Name
    DsmStateEtwSuspendedInHubSuspend,
    // State Entry Function
    HUBDSM_SuspendedInHubSuspend,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubResume ,           DsmStateAcquiringPowerReferenceOnHubResume },
        { DsmEventPortStateDisabled ,   DsmStateWaitingForHubResumeWithRenumRequired },
        { DsmEventPortStateSuspended ,  DsmStateWaitingForHubResumeInSuspended },
        { DsmEventPDOD0Exit ,           DsmStateCompletingUnexpectedD0ExitInHubSuspended },
        { DsmEventHubResumeInS0 ,       DsmStateAcquiringPowerReferenceOnResumeInS0 },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateWaitingForHubResumeWithRenumRequired },
        { DsmEventPortStateEnabled ,    DsmStateWaitingForHubResumeInEnabled },
        { DsmEventHubResumeWithReset ,  DsmStateAcquiringPowerReferenceOnHubResumeWtihReset },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySuspendingPortOnPortEnabledInPendingHubSuspend = {
    // State ETW Name
    DsmStateEtwSuspendingPortOnPortEnabledInPendingHubSuspend,
    // State Entry Function
    HUBDSM_SuspendingPortOnPortEnabledInPendingHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortSuspended ,       DsmStateReleasingReferenceOnHubSuspendAfterPSMSyncUp },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetach },
        { DsmEventPortFailed ,          DsmStateReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForHubResumeInEnabled = {
    // State ETW Name
    DsmStateEtwWaitingForHubResumeInEnabled,
    // State Entry Function
    HUBDSM_WaitingForHubResumeInEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubResume ,           DsmStateAcquiringPowerReferenceInEnabled },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetach },
        { DsmEventHubResumeWithReset ,  DsmStateAcquiringPowerReferenceOnHubResumeWithReenumRequired },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForHubResumeInSuspended = {
    // State ETW Name
    DsmStateEtwWaitingForHubResumeInSuspended,
    // State Entry Function
    HUBDSM_WaitingForHubResumeInSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubResume ,           DsmStateAcquiringPowerReferenceInSuspended },
        { DsmEventPortResumed ,         DsmStateAckingResumed },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetach },
        { DsmEventHubResumeWithReset ,  DsmStateAcquiringPowerReferenceInSuspendedOnResumeWithReset },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForHubResumeWithRenumRequired = {
    // State ETW Name
    DsmStateEtwWaitingForHubResumeWithRenumRequired,
    // State Entry Function
    HUBDSM_WaitingForHubResumeWithRenumRequired,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubResume ,           DsmStateAcquiringPowerReferenceOnHubResumeWithReenumRequired },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetach },
        { DsmEventHubResumeWithReset ,  DsmStateAcquiringPowerReferenceOnHubResumeWithReenumRequired },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPSMSyncUp = {
    // State ETW Name
    DsmStateEtwWaitingForPSMSyncUp,
    // State Entry Function
    HUBDSM_WaitingForPSMSyncUp,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortStateDisabled ,   DsmStateReturningDeviceRenumerationOnHubResume },
        { DsmEventPortStateEnabled ,    DsmStateCheckingIfDeviceNeedsResetOnResumeInSxOnEnabled },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetach },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateReturningDeviceRenumerationOnHubResume },
        { DsmEventPortStateSuspended ,  DsmStateReturningDeviceSuspendedOnHubResume },
        { DsmEventHubSuspend ,          DsmStateWaitingForPSMSyncUpOnPendingSuspend },
        { DsmEventHubStop ,             DsmStateWaitingForPSMSyncUpOnPendingStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPSMSyncUpOnPendingStop = {
    // State ETW Name
    DsmStateEtwWaitingForPSMSyncUpOnPendingStop,
    // State Entry Function
    HUBDSM_WaitingForPSMSyncUpOnPendingStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortStateEnabled ,    DsmStateDisablingPortOnPortEnabledInPendingHubStop },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetach },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateDisablingPortOnPortEnabledInPendingHubStop },
        { DsmEventPortStateSuspended ,  DsmStateReturningHubStopWithReferenceAcquiredInHubSuspended },
        { DsmEventPortStateDisabled ,   DsmStateReturningHubStopWithReferenceAcquiredInHubSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPSMSyncUpOnPendingSuspend = {
    // State ETW Name
    DsmStateEtwWaitingForPSMSyncUpOnPendingSuspend,
    // State Entry Function
    HUBDSM_WaitingForPSMSyncUpOnPendingSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortStateSuspended ,  DsmStateReleasingReferenceOnHubSuspendAfterPSMSyncUp },
        { DsmEventPortStateDisabled ,   DsmStateReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateSuspendingPortOnPortEnabledInPendingHubSuspend },
        { DsmEventPortStateEnabled ,    DsmStateSuspendingPortOnPortEnabledInPendingHubSuspend },
        { DsmEventPortDetachDevice ,    DsmStateReleasingReferenceOnHubSuspendOnDetachAfterPSMSyncUp },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPSMSyncUpOnResumeWithReset = {
    // State ETW Name
    DsmStateEtwWaitingForPSMSyncUpOnResumeWithReset,
    // State Entry Function
    HUBDSM_WaitingForPSMSyncUpOnResumeWithReset,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortStateEnabled ,    DsmStateReturningDeviceRenumerationOnHubResume },
        { DsmEventPortStateSuspended ,  DsmStateResumingBeforeResetting },
        { DsmEventHubSuspend ,          DsmStateWaitingForPSMSyncUpOnPendingSuspend },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetach },
        { DsmEventPortStateDisabled ,   DsmStateReturningDeviceRenumerationOnHubResume },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateReturningDeviceRenumerationOnHubResume },
        { DsmEventHubStop ,             DsmStateWaitingForPSMSyncUpOnPendingStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForResumeResponseFromPortOnHubStop = {
    // State ETW Name
    DsmStateEtwWaitingForResumeResponseFromPortOnHubStop,
    // State Entry Function
    HUBDSM_WaitingForResumeResponseFromPortOnHubStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResumed ,         DsmStateDisablingPortOnPortEnabledInPendingHubStop },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateReturningHubStopWithReferenceAcquiredInHubSuspended },
        { DsmEventPortResumeTimedOut ,  DsmStateReturningHubStopWithReferenceAcquiredInHubSuspended },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetach },
        { DsmEventPortStateDisabled ,   DsmStateReturningHubStopWithReferenceAcquiredInHubSuspended },
        { DsmEventPortFailed ,          DsmStateReturningHubStopWithReferenceAcquiredInHubSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForResumeResponseFromPortOnHubSuspend = {
    // State ETW Name
    DsmStateEtwWaitingForResumeResponseFromPortOnHubSuspend,
    // State Entry Function
    HUBDSM_WaitingForResumeResponseFromPortOnHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResumed ,         DsmStateSuspendingPortOnPortEnabledInPendingHubSuspend },
        { DsmEventPortFailed ,          DsmStateReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetach },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateSuspendingPortOnPortEnabledInPendingHubSuspend },
        { DsmEventPortStateDisabled ,   DsmStateReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed },
        { DsmEventPortResumeTimedOut ,  DsmStateReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAcquiringAddressZeroOwnershipOnRenum = {
    // State ETW Name
    DsmStateEtwAcquiringAddressZeroOwnershipOnRenum,
    // State Entry Function
    HUBDSM_AcquiringAddressZeroOwnershipOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateCancellingAcquiringAddress0OnDetachOnRenum },
        { DsmEventHubStop ,             DsmStateWaitingForUCXIoctlOnStopSuspend },
        { DsmEventUCXIoctlSuccess ,     DsmStateEnumeratingAtAddressZeroOnRenum },
        { DsmEventUCXIoctlFailure ,     DsmStateCheckingIfEnumRetryReachedMaximumOnRenum },
        { DsmEventHubSuspend ,          DsmStateWaitingForUCXIoctlOnStopSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAcquiringPowerReferenceOnStartInReEnum = {
    // State ETW Name
    DsmStateEtwAcquiringPowerReferenceOnStartInReEnum,
    // State Entry Function
    HUBDSM_AcquiringPowerReferenceOnStartInReEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateEnablingDeviceInControllerInRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCancellingAcquiringAddress0OnDetachOnRenum = {
    // State ETW Name
    DsmStateEtwCancellingAcquiringAddress0OnDetachOnRenum,
    // State Entry Function
    HUBDSM_CancellingAcquiringAddress0OnDetachOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForUCXIoctlOnDetachOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCancellingRetryTimerOnDetachOnRenum = {
    // State ETW Name
    DsmStateEtwCancellingRetryTimerOnDetachOnRenum,
    // State Entry Function
    HUBDSM_CancellingRetryTimerOnDetachOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerToFlushOnDetachOnRenum },
        { DsmEventOperationSuccess ,    DsmStateReturningPortDetachDeviceWithDeviceEnabledOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCancellingRetryTimerOnStopSuspendOnRenum = {
    // State ETW Name
    DsmStateEtwCancellingRetryTimerOnStopSuspendOnRenum,
    // State Entry Function
    HUBDSM_CancellingRetryTimerOnStopSuspendOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerToFlushOnStopSuspendOnRenum },
        { DsmEventOperationSuccess ,    DsmStateDisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfAltEnumCmdNeededInReenum = {
    // State ETW Name
    DsmStateEtwCheckingIfAltEnumCmdNeededInReenum,
    // State Entry Function
    HUBDSM_CheckingIfAltEnumCmdNeededInReenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateSendingMsOs20AltEnumCmdOnReenum },
        { DsmEventNo ,                  DsmStateCheckingIfSerialNumberShouldBeCompared },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDevicePDChargingPolicySupportedInReEnum = {
    // State ETW Name
    DsmStateEtwCheckingIfDevicePDChargingPolicySupportedInReEnum,
    // State Entry Function
    HUBDSM_CheckingIfDevicePDChargingPolicySupportedInReEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateReturningOperationSuccessOnRenum },
        { DsmEventYes ,                 DsmStateSettingDevicePDChargingPolicyInReEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDevicePorgrammingWasLosttInUnconfigured = {
    // State ETW Name
    DsmStateEtwCheckingIfDevicePorgrammingWasLosttInUnconfigured,
    // State Entry Function
    HUBDSM_CheckingIfDevicePorgrammingWasLosttInUnconfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateEnablingDeviceInControllerInRenum },
        { DsmEventNo ,                  DsmStateAcquiringAddressZeroOwnershipOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDeviceProgrammingWasLosttInConfigured = {
    // State ETW Name
    DsmStateEtwCheckingIfDeviceProgrammingWasLosttInConfigured,
    // State Entry Function
    HUBDSM_CheckingIfDeviceProgrammingWasLosttInConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateEnablingDeviceInControllerInRenum },
        { DsmEventNo ,                  DsmStateDeconfiguringEndpointsInControllerBeforeRenumerating },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfEnumRetryReachedMaximumOnRenum = {
    // State ETW Name
    DsmStateEtwCheckingIfEnumRetryReachedMaximumOnRenum,
    // State Entry Function
    HUBDSM_CheckingIfEnumRetryReachedMaximumOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStatePurgingIoOnEnumAfterFailureInReEnum },
        { DsmEventNo ,                  DsmStateStartingTimerForEnumRetryOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfEnumRetryReachedMaximumOnRenumWithAddress0Ownership = {
    // State ETW Name
    DsmStateEtwCheckingIfEnumRetryReachedMaximumOnRenumWithAddress0Ownership,
    // State Entry Function
    HUBDSM_CheckingIfEnumRetryReachedMaximumOnRenumWithAddress0Ownership,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStatePurgingIoOnEnumAfterFailureInReEnumWithAddress0Ownership },
        { DsmEventNo ,                  DsmStateStartingTimerForEnumRetryOnRenumWithAddress0Ownership },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfEnumRetryReachedMaximumWithDeviceDisabledOnRenum = {
    // State ETW Name
    DsmStateEtwCheckingIfEnumRetryReachedMaximumWithDeviceDisabledOnRenum,
    // State Entry Function
    HUBDSM_CheckingIfEnumRetryReachedMaximumWithDeviceDisabledOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateEnablingDeviceInControllerInRenum },
        { DsmEventYes ,                 DsmStateDisablingOnEnumAfterFailureWithDeviceDisabledInReEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfItIsBootDeviceOnIdComparisionFailure = {
    // State ETW Name
    DsmStateEtwCheckingIfItIsBootDeviceOnIdComparisionFailure,
    // State Entry Function
    HUBDSM_CheckingIfItIsBootDeviceOnIdComparisionFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateCheckingIfEnumRetryReachedMaximumOnRenum },
        { DsmEventYes ,                 DsmStateWaitingForDetachAfterWrongDeviceDetectedOnBootDevicePort },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfRequestConfigDescOnResetIsSet = {
    // State ETW Name
    DsmStateEtwCheckingIfRequestConfigDescOnResetIsSet,
    // State Entry Function
    HUBDSM_CheckingIfRequestConfigDescOnResetIsSet,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateGettingConfigDescriptorWhileRenuemrating },
        { DsmEventNo ,                  DsmStateSettingLPMValuesInReEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfSerialNumberShouldBeCompared = {
    // State ETW Name
    DsmStateEtwCheckingIfSerialNumberShouldBeCompared,
    // State Entry Function
    HUBDSM_CheckingIfSerialNumberShouldBeCompared,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateCheckingIfRequestConfigDescOnResetIsSet },
        { DsmEventYes ,                 DsmStateGettingSerialNumberStringDescriptorWhileRenuemrating },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryClearingDeviceLostProgammingFlagOnRestart = {
    // State ETW Name
    DsmStateEtwClearingDeviceLostProgammingFlagOnRestart,
    // State Entry Function
    HUBDSM_ClearingDeviceLostProgammingFlagOnRestart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateEnablingDeviceInControllerInRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryClearingDeviceProgrammingLostFlagOnRenum = {
    // State ETW Name
    DsmStateEtwClearingDeviceProgrammingLostFlagOnRenum,
    // State Entry Function
    HUBDSM_ClearingDeviceProgrammingLostFlagOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateEnablingDeviceInControllerInRenum },
        { DsmEventYes ,                 DsmStateEnablingDeviceInControllerInRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryComparingDeviceOnReEnumeration = {
    // State ETW Name
    DsmStateEtwComparingDeviceOnReEnumeration,
    // State Entry Function
    HUBDSM_ComparingDeviceOnReEnumeration,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateCheckingIfItIsBootDeviceOnIdComparisionFailure },
        { DsmEventYes ,                 DsmStateCheckingIfAltEnumCmdNeededInReenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryComparingSerialNumberOnReEnumeration = {
    // State ETW Name
    DsmStateEtwComparingSerialNumberOnReEnumeration,
    // State Entry Function
    HUBDSM_ComparingSerialNumberOnReEnumeration,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateCheckingIfItIsBootDeviceOnIdComparisionFailure },
        { DsmEventYes ,                 DsmStateCheckingIfRequestConfigDescOnResetIsSet },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeconfiguringEndpointsInControllerBeforeRenumerating = {
    // State ETW Name
    DsmStateEtwDeconfiguringEndpointsInControllerBeforeRenumerating,
    // State Entry Function
    HUBDSM_DeconfiguringEndpointsInControllerBeforeRenumerating,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateAcquiringAddressZeroOwnershipOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnStopSuspendOnRenum = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnStopSuspendOnRenum,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnStopSuspendOnRenum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateReleasingPowerReferenceOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership = {
    // State ETW Name
    DsmStateEtwDisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership,
    // State Entry Function
    HUBDSM_DisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateWaitingForDisableCompleteOnStopSuspendOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingOnEnumAfterFailureInReEnum = {
    // State ETW Name
    DsmStateEtwDisablingOnEnumAfterFailureInReEnum,
    // State Entry Function
    HUBDSM_DisablingOnEnumAfterFailureInReEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceWithDeviceEnabledOnRenum },
        { DsmEventPortFailed ,          DsmStateReturningOperationFailureWithDeviceEnabledOnRenum },
        { DsmEventPortDisabled ,        DsmStateReturningOperationFailureWithDeviceEnabledOnRenum },
        { DsmEventHubSuspend ,          DsmStateWaitingForDisableCompleteOnStopSuspendAfterFailureInReEnum },
        { DsmEventHubStop ,             DsmStateWaitingForDisableCompleteOnStopSuspendAfterFailureInReEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingOnEnumAfterFailureInReEnumWithAddress0Ownership = {
    // State ETW Name
    DsmStateEtwDisablingOnEnumAfterFailureInReEnumWithAddress0Ownership,
    // State Entry Function
    HUBDSM_DisablingOnEnumAfterFailureInReEnumWithAddress0Ownership,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReleasingAddressZeroOwnershipOnDetachOnEnumFailure },
        { DsmEventHubSuspend ,          DsmStateReleasingAddressZeroOwnershipOnStopSuspendOnEnumFailure },
        { DsmEventHubStop ,             DsmStateReleasingAddressZeroOwnershipOnStopSuspendOnEnumFailure },
        { DsmEventPortDisabled ,        DsmStateReleasingAddressZeroOwnershipOnEnumFailureInRenum },
        { DsmEventPortFailed ,          DsmStateReleasingAddressZeroOwnershipOnEnumFailureInRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingOnEnumAfterFailureWithDeviceDisabledInReEnum = {
    // State ETW Name
    DsmStateEtwDisablingOnEnumAfterFailureWithDeviceDisabledInReEnum,
    // State Entry Function
    HUBDSM_DisablingOnEnumAfterFailureWithDeviceDisabledInReEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceOnRenum },
        { DsmEventPortFailed ,          DsmStateReturningOperationFailureOnRenum },
        { DsmEventPortDisabled ,        DsmStateReturningOperationFailureOnRenum },
        { DsmEventHubSuspend ,          DsmStateWaitingForDisableCompleteOnStopSuspendWithDeviceDisabledAfterFailureInReEnum },
        { DsmEventHubStop ,             DsmStateWaitingForDisableCompleteOnStopSuspendWithDeviceDisabledAfterFailureInReEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryEnablingDeviceInControllerInRenum = {
    // State ETW Name
    DsmStateEtwEnablingDeviceInControllerInRenum,
    // State Entry Function
    HUBDSM_EnablingDeviceInControllerInRenum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlFailure ,     DsmStateCheckingIfEnumRetryReachedMaximumWithDeviceDisabledOnRenum },
        { DsmEventUCXIoctlSuccess ,     DsmStateAcquiringAddressZeroOwnershipOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryEnumeratingAtAddressZeroOnRenum = {
    // State ETW Name
    DsmStateEtwEnumeratingAtAddressZeroOnRenum,
    // State Entry Function
    HUBDSM_EnumeratingAtAddressZeroOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmEnumeratingAtAddressZeroOnRenum,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateSettingNonZeroAddressOnRenum },
        { DsmEventOperationFailure ,    DsmStateCheckingIfEnumRetryReachedMaximumOnRenumWithAddress0Ownership },
        { DsmEventPortDetachDevice ,    DsmStateReleasingAddressZeroOwnershipOnDetachWithDeviceEnabledOnRenum },
        { DsmEventHubSuspend ,          DsmStateReleasingAddressZeroOwnershipOnStopSuspendOnRenum },
        { DsmEventHubStop ,             DsmStateReleasingAddressZeroOwnershipOnStopSuspendOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingConfigDescriptorWhileRenuemrating = {
    // State ETW Name
    DsmStateEtwGettingConfigDescriptorWhileRenuemrating,
    // State Entry Function
    HUBDSM_GettingConfigDescriptorWhileRenuemrating,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateSettingLPMValuesInReEnum },
        { DsmEventTransferFailure ,     DsmStateCheckingIfEnumRetryReachedMaximumOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingDeviceDescriptorOnRenum = {
    // State ETW Name
    DsmStateEtwGettingDeviceDescriptorOnRenum,
    // State Entry Function
    HUBDSM_GettingDeviceDescriptorOnRenum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateComparingDeviceOnReEnumeration },
        { DsmEventTransferFailure ,     DsmStateCheckingIfEnumRetryReachedMaximumOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingSerialNumberStringDescriptorWhileRenuemrating = {
    // State ETW Name
    DsmStateEtwGettingSerialNumberStringDescriptorWhileRenuemrating,
    // State Entry Function
    HUBDSM_GettingSerialNumberStringDescriptorWhileRenuemrating,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateCheckingIfEnumRetryReachedMaximumOnRenum },
        { DsmEventTransferSuccess ,     DsmStateComparingSerialNumberOnReEnumeration },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryInitializingEnumRetryCountInReEnumDuringResetInConfigured = {
    // State ETW Name
    DsmStateEtwInitializingEnumRetryCountInReEnumDuringResetInConfigured,
    // State Entry Function
    HUBDSM_InitializingEnumRetryCountInReEnumDuringResetInConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStatePurgingDeviceTreeIoOnReEnumerationInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryInitializingEnumRetryCountInReEnumDuringResetInUnConfigured = {
    // State ETW Name
    DsmStateEtwInitializingEnumRetryCountInReEnumDuringResetInUnConfigured,
    // State Entry Function
    HUBDSM_InitializingEnumRetryCountInReEnumDuringResetInUnConfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStatePurgingDeviceTreeIoOnReEnumerationInUnConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryInitializingEnumRetryCountInReEnumDuringRestart = {
    // State ETW Name
    DsmStateEtwInitializingEnumRetryCountInReEnumDuringRestart,
    // State Entry Function
    HUBDSM_InitializingEnumRetryCountInReEnumDuringRestart,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateClearingDeviceLostProgammingFlagOnRestart },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPurgingDeviceTreeIoOnReEnumerationInConfigured = {
    // State ETW Name
    DsmStateEtwPurgingDeviceTreeIoOnReEnumerationInConfigured,
    // State Entry Function
    HUBDSM_PurgingDeviceTreeIoOnReEnumerationInConfigured,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCheckingIfDeviceProgrammingWasLosttInConfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPurgingDeviceTreeIoOnReEnumerationInUnConfigured = {
    // State ETW Name
    DsmStateEtwPurgingDeviceTreeIoOnReEnumerationInUnConfigured,
    // State Entry Function
    HUBDSM_PurgingDeviceTreeIoOnReEnumerationInUnConfigured,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCheckingIfDevicePorgrammingWasLosttInUnconfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPurgingIoOnEnumAfterFailureInReEnum = {
    // State ETW Name
    DsmStateEtwPurgingIoOnEnumAfterFailureInReEnum,
    // State Entry Function
    HUBDSM_PurgingIoOnEnumAfterFailureInReEnum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingOnEnumAfterFailureInReEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPurgingIoOnEnumAfterFailureInReEnumWithAddress0Ownership = {
    // State ETW Name
    DsmStateEtwPurgingIoOnEnumAfterFailureInReEnumWithAddress0Ownership,
    // State Entry Function
    HUBDSM_PurgingIoOnEnumAfterFailureInReEnumWithAddress0Ownership,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingOnEnumAfterFailureInReEnumWithAddress0Ownership },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingAddressZeroOwnershipOnDetachOnEnumFailure = {
    // State ETW Name
    DsmStateEtwReleasingAddressZeroOwnershipOnDetachOnEnumFailure,
    // State Entry Function
    HUBDSM_ReleasingAddressZeroOwnershipOnDetachOnEnumFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningPortDetachDeviceWithDeviceEnabledOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingAddressZeroOwnershipOnDetachWithDeviceEnabledOnRenum = {
    // State ETW Name
    DsmStateEtwReleasingAddressZeroOwnershipOnDetachWithDeviceEnabledOnRenum,
    // State Entry Function
    HUBDSM_ReleasingAddressZeroOwnershipOnDetachWithDeviceEnabledOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningPortDetachDeviceWithDeviceEnabledOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingAddressZeroOwnershipOnDetachWithTimer = {
    // State ETW Name
    DsmStateEtwReleasingAddressZeroOwnershipOnDetachWithTimer,
    // State Entry Function
    HUBDSM_ReleasingAddressZeroOwnershipOnDetachWithTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCancellingRetryTimerOnDetachOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingAddressZeroOwnershipOnEnumFailureInRenum = {
    // State ETW Name
    DsmStateEtwReleasingAddressZeroOwnershipOnEnumFailureInRenum,
    // State Entry Function
    HUBDSM_ReleasingAddressZeroOwnershipOnEnumFailureInRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationFailureWithDeviceEnabledOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingAddressZeroOwnershipOnRenum = {
    // State ETW Name
    DsmStateEtwReleasingAddressZeroOwnershipOnRenum,
    // State Entry Function
    HUBDSM_ReleasingAddressZeroOwnershipOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateGettingDeviceDescriptorOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingAddressZeroOwnershipOnStopSuspendOnEnumFailure = {
    // State ETW Name
    DsmStateEtwReleasingAddressZeroOwnershipOnStopSuspendOnEnumFailure,
    // State Entry Function
    HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendOnEnumFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDisableCompleteOnStopSuspendAfterFailureInReEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingAddressZeroOwnershipOnStopSuspendOnRenum = {
    // State ETW Name
    DsmStateEtwReleasingAddressZeroOwnershipOnStopSuspendOnRenum,
    // State Entry Function
    HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingAddressZeroOwnershipOnStopSuspendWithTimer = {
    // State ETW Name
    DsmStateEtwReleasingAddressZeroOwnershipOnStopSuspendWithTimer,
    // State Entry Function
    HUBDSM_ReleasingAddressZeroOwnershipOnStopSuspendWithTimer,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCancellingRetryTimerOnStopSuspendOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnRenum = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnRenum,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateStoppedSuspendedOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureOnRenum = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureOnRenum,
    // State Entry Function
    HUBDSM_ReturningOperationFailureOnRenum,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureWithDeviceEnabledOnRenum = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureWithDeviceEnabledOnRenum,
    // State Entry Function
    HUBDSM_ReturningOperationFailureWithDeviceEnabledOnRenum,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessOnRenum = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessOnRenum,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessOnRenum,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDetachDeviceOnRenum = {
    // State ETW Name
    DsmStateEtwReturningPortDetachDeviceOnRenum,
    // State Entry Function
    HUBDSM_ReturningPortDetachDeviceOnRenum,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDetachDeviceWithDeviceEnabledOnRenum = {
    // State ETW Name
    DsmStateEtwReturningPortDetachDeviceWithDeviceEnabledOnRenum,
    // State Entry Function
    HUBDSM_ReturningPortDetachDeviceWithDeviceEnabledOnRenum,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySendingMsOs20AltEnumCmdOnReenum = {
    // State ETW Name
    DsmStateEtwSendingMsOs20AltEnumCmdOnReenum,
    // State Entry Function
    HUBDSM_SendingMsOs20AltEnumCmdOnReenum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateCheckingIfEnumRetryReachedMaximumOnRenum },
        { DsmEventTransferSuccess ,     DsmStateCheckingIfSerialNumberShouldBeCompared },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingDevicePDChargingPolicyInReEnum = {
    // State ETW Name
    DsmStateEtwSettingDevicePDChargingPolicyInReEnum,
    // State Entry Function
    HUBDSM_SettingDevicePDChargingPolicyInReEnum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateReturningOperationSuccessOnRenum },
        { DsmEventTransferFailure ,     DsmStateCheckingIfEnumRetryReachedMaximumOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingLPMValuesInReEnum = {
    // State ETW Name
    DsmStateEtwSettingLPMValuesInReEnum,
    // State Entry Function
    HUBDSM_SettingLPMValuesInReEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmSettingLPMValuesInReEnum,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateCheckingIfEnumRetryReachedMaximumOnRenum },
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDevicePDChargingPolicySupportedInReEnum },
        { DsmEventContinueIfNoSubState ,DsmStateCheckingIfDevicePDChargingPolicySupportedInReEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingNonZeroAddressOnRenum = {
    // State ETW Name
    DsmStateEtwSettingNonZeroAddressOnRenum,
    // State Entry Function
    HUBDSM_SettingNonZeroAddressOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    DsmSubSmSettingNonZeroAddressOnRenum,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateCheckingIfEnumRetryReachedMaximumOnRenumWithAddress0Ownership },
        { DsmEventOperationSuccess ,    DsmStateReleasingAddressZeroOwnershipOnRenum },
        { DsmEventPortDetachDevice ,    DsmStateReleasingAddressZeroOwnershipOnDetachWithDeviceEnabledOnRenum },
        { DsmEventHubStop ,             DsmStateReleasingAddressZeroOwnershipOnStopSuspendOnRenum },
        { DsmEventHubSuspend ,          DsmStateReleasingAddressZeroOwnershipOnStopSuspendOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStartingTimerForEnumRetryOnRenum = {
    // State ETW Name
    DsmStateEtwStartingTimerForEnumRetryOnRenum,
    // State Entry Function
    HUBDSM_StartingTimerForEnumRetryOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTimerFired ,          DsmStateAcquiringAddressZeroOwnershipOnRenum },
        { DsmEventPortDetachDevice ,    DsmStateCancellingRetryTimerOnDetachOnRenum },
        { DsmEventHubSuspend ,          DsmStateCancellingRetryTimerOnStopSuspendOnRenum },
        { DsmEventHubStop ,             DsmStateCancellingRetryTimerOnStopSuspendOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStartingTimerForEnumRetryOnRenumWithAddress0Ownership = {
    // State ETW Name
    DsmStateEtwStartingTimerForEnumRetryOnRenumWithAddress0Ownership,
    // State Entry Function
    HUBDSM_StartingTimerForEnumRetryOnRenumWithAddress0Ownership,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubSuspend ,          DsmStateReleasingAddressZeroOwnershipOnStopSuspendWithTimer },
        { DsmEventPortDetachDevice ,    DsmStateReleasingAddressZeroOwnershipOnDetachWithTimer },
        { DsmEventTimerFired ,          DsmStateEnumeratingAtAddressZeroOnRenum },
        { DsmEventHubStop ,             DsmStateReleasingAddressZeroOwnershipOnStopSuspendWithTimer },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppedSuspendedOnRenum = {
    // State ETW Name
    DsmStateEtwStoppedSuspendedOnRenum,
    // State Entry Function
    HUBDSM_StoppedSuspendedOnRenum,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    DsmSubSmStoppedSuspendedOnRenum,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceSuspendedOnHubResume ,DsmStateClearingDeviceProgrammingLostFlagOnRenum },
        { DsmEventHubStopWithReferenceAcquired ,DsmStateReleasingPowerReferenceOnRenum },
        { DsmEventDeviceResumedOnHubResume ,DsmStateClearingDeviceProgrammingLostFlagOnRenum },
        { DsmEventDeviceRenumerateOnHubResume ,DsmStateClearingDeviceProgrammingLostFlagOnRenum },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceOnRenum },
        { DsmEventHubStart ,            DsmStateAcquiringPowerReferenceOnStartInReEnum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDetachAfterWrongDeviceDetectedOnBootDevicePort = {
    // State ETW Name
    DsmStateEtwWaitingForDetachAfterWrongDeviceDetectedOnBootDevicePort,
    // State Entry Function
    HUBDSM_WaitingForDetachAfterWrongDeviceDetectedOnBootDevicePort,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceWithDeviceEnabledOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDisableCompleteOnStopSuspendAfterFailureInReEnum = {
    // State ETW Name
    DsmStateEtwWaitingForDisableCompleteOnStopSuspendAfterFailureInReEnum,
    // State Entry Function
    HUBDSM_WaitingForDisableCompleteOnStopSuspendAfterFailureInReEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceWithDeviceEnabledOnRenum },
        { DsmEventPortFailed ,          DsmStateDisablingDeviceInControllerOnStopSuspendOnRenum },
        { DsmEventPortDisabled ,        DsmStateDisablingDeviceInControllerOnStopSuspendOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDisableCompleteOnStopSuspendOnRenum = {
    // State ETW Name
    DsmStateEtwWaitingForDisableCompleteOnStopSuspendOnRenum,
    // State Entry Function
    HUBDSM_WaitingForDisableCompleteOnStopSuspendOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceOnRenum },
        { DsmEventPortFailed ,          DsmStateReleasingPowerReferenceOnRenum },
        { DsmEventPortDisabled ,        DsmStateReleasingPowerReferenceOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDisableCompleteOnStopSuspendWithDeviceDisabledAfterFailureInReEnum = {
    // State ETW Name
    DsmStateEtwWaitingForDisableCompleteOnStopSuspendWithDeviceDisabledAfterFailureInReEnum,
    // State Entry Function
    HUBDSM_WaitingForDisableCompleteOnStopSuspendWithDeviceDisabledAfterFailureInReEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDisabled ,        DsmStateReleasingPowerReferenceOnRenum },
        { DsmEventPortFailed ,          DsmStateReleasingPowerReferenceOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerToFlushOnDetachOnRenum = {
    // State ETW Name
    DsmStateEtwWaitingForTimerToFlushOnDetachOnRenum,
    // State Entry Function
    HUBDSM_WaitingForTimerToFlushOnDetachOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventTimerFired ,          DsmStateReturningPortDetachDeviceWithDeviceEnabledOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerToFlushOnStopSuspendOnRenum = {
    // State ETW Name
    DsmStateEtwWaitingForTimerToFlushOnStopSuspendOnRenum,
    // State Entry Function
    HUBDSM_WaitingForTimerToFlushOnStopSuspendOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTimerFired ,          DsmStateDisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership },
        { DsmEventPortDetachDevice ,    DsmStateWaitingForTimerToFlushOnDetachOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForUCXIoctlOnDetachOnRenum = {
    // State ETW Name
    DsmStateEtwWaitingForUCXIoctlOnDetachOnRenum,
    // State Entry Function
    HUBDSM_WaitingForUCXIoctlOnDetachOnRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventUCXIoctlFailure ,     DsmStateReturningPortDetachDeviceWithDeviceEnabledOnRenum },
        { DsmEventUCXIoctlSuccess ,     DsmStateReleasingAddressZeroOwnershipOnDetachWithDeviceEnabledOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForUCXIoctlOnStopSuspend = {
    // State ETW Name
    DsmStateEtwWaitingForUCXIoctlOnStopSuspend,
    // State Entry Function
    HUBDSM_WaitingForUCXIoctlOnStopSuspend,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlFailure ,     DsmStateDisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership },
        { DsmEventUCXIoctlSuccess ,     DsmStateReleasingAddressZeroOwnershipOnStopSuspendOnRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfAnyAlternateInterfaceLeft = {
    // State ETW Name
    DsmStateEtwCheckingIfAnyAlternateInterfaceLeft,
    // State Entry Function
    HUBDSM_CheckingIfAnyAlternateInterfaceLeft,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateCreatingNewEndpoints },
        { DsmEventYes ,                 DsmStateSettingDeviceInterfaceInConfiguring },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryConfiguringDevice = {
    // State ETW Name
    DsmStateEtwConfiguringDevice,
    // State Entry Function
    HUBDSM_ConfiguringDevice,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateCheckingIfAnyAlternateInterfaceLeft },
        { DsmEventTransferFailure ,     DsmStateMarkingAnyEndpointsToBeEnableAsDisabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCreatingNewEndpoints = {
    // State ETW Name
    DsmStateEtwCreatingNewEndpoints,
    // State Entry Function
    HUBDSM_CreatingNewEndpoints,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateProgrammingEndpointsAndSettingLPMValuesrForDeviceConfiguration },
        { DsmEventOperationFailure ,    DsmStateMarkingAnyEndpointsToBeEnableAsDisabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeconfiguringEndpointsInControllerForDeviceConfiguration = {
    // State ETW Name
    DsmStateEtwDeconfiguringEndpointsInControllerForDeviceConfiguration,
    // State Entry Function
    HUBDSM_DeconfiguringEndpointsInControllerForDeviceConfiguration,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingEndpointsForOldConfiguration },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingEndpointsForOldConfiguration = {
    // State ETW Name
    DsmStateEtwDeletingEndpointsForOldConfiguration,
    // State Entry Function
    HUBDSM_DeletingEndpointsForOldConfiguration,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateConfiguringDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryMarkingAnyEndpointsToBeEnableAsDisabled = {
    // State ETW Name
    DsmStateEtwMarkingAnyEndpointsToBeEnableAsDisabled,
    // State Entry Function
    HUBDSM_MarkingAnyEndpointsToBeEnableAsDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationFailureInReConfiguring },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryProgrammingEndpointsAndSettingLPMValuesrForDeviceConfiguration = {
    // State ETW Name
    DsmStateEtwProgrammingEndpointsAndSettingLPMValuesrForDeviceConfiguration,
    // State Entry Function
    HUBDSM_ProgrammingEndpointsAndSettingLPMValuesrForDeviceConfiguration,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    DsmSubSmProgrammingEndpointsAndSettingLPMValuesrForDeviceConfiguration,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationSuccessInReConfiguring },
        { DsmEventContinueIfNoSubState ,DsmStateProgrammingEndpointsInControllerForDeviceConfigurationFor20Devices },
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInReConfiguring },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryProgrammingEndpointsInControllerForDeviceConfigurationFor20Devices = {
    // State ETW Name
    DsmStateEtwProgrammingEndpointsInControllerForDeviceConfigurationFor20Devices,
    // State Entry Function
    HUBDSM_ProgrammingEndpointsInControllerForDeviceConfigurationFor20Devices,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateReturningOperationSuccessInReConfiguring },
        { DsmEventUCXIoctlFailure ,     DsmStateReturningOperationFailureInReConfiguring },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureInReConfiguring = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureInReConfiguring,
    // State Entry Function
    HUBDSM_ReturningOperationFailureInReConfiguring,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInReConfiguring = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInReConfiguring,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInReConfiguring,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingDeviceInterfaceInConfiguring = {
    // State ETW Name
    DsmStateEtwSettingDeviceInterfaceInConfiguring,
    // State Entry Function
    HUBDSM_SettingDeviceInterfaceInConfiguring,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateCheckingIfAnyAlternateInterfaceLeft },
        { DsmEventTransferFailure ,     DsmStateMarkingAnyEndpointsToBeEnableAsDisabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingPortOnHubSuspendInCleanup = {
    // State ETW Name
    DsmStateEtwDisablingPortOnHubSuspendInCleanup,
    // State Entry Function
    HUBDSM_DisablingPortOnHubSuspendInCleanup,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceFromIgnoringDevicePort },
        { DsmEventPortFailed ,          DsmStateReleasingPowerReferenceOnHubSuspendInCleanup },
        { DsmEventPortDisabled ,        DsmStateReleasingPowerReferenceOnHubSuspendInCleanup },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingHubPowerDownEvents = {
    // State ETW Name
    DsmStateEtwFlushingHubPowerDownEvents,
    // State Entry Function
    HUBDSM_FlushingHubPowerDownEvents,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEvents },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingHubPowerUpEvents = {
    // State ETW Name
    DsmStateEtwFlushingHubPowerUpEvents,
    // State Entry Function
    HUBDSM_FlushingHubPowerUpEvents,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEvents },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingHubStopAfterSuspendEvent = {
    // State ETW Name
    DsmStateEtwFlushingHubStopAfterSuspendEvent,
    // State Entry Function
    HUBDSM_FlushingHubStopAfterSuspendEvent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEvents },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingPnpEvents = {
    // State ETW Name
    DsmStateEtwFlushingPnpEvents,
    // State Entry Function
    HUBDSM_FlushingPnpEvents,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEvents },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInWaitingForDevicePortEvents = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInWaitingForDevicePortEvents,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEvents,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateWaitingForDevicePortEvents },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnHubSuspendInCleanup = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnHubSuspendInCleanup,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnHubSuspendInCleanup,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningPortDisabledOnHubSuspendInCleanup },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDetachDeviceFromIgnoringDevicePort = {
    // State ETW Name
    DsmStateEtwReturningPortDetachDeviceFromIgnoringDevicePort,
    // State Entry Function
    HUBDSM_ReturningPortDetachDeviceFromIgnoringDevicePort,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDisabledOnHubSuspendInCleanup = {
    // State ETW Name
    DsmStateEtwReturningPortDisabledOnHubSuspendInCleanup,
    // State Entry Function
    HUBDSM_ReturningPortDisabledOnHubSuspendInCleanup,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDevicePortEvents = {
    // State ETW Name
    DsmStateEtwWaitingForDevicePortEvents,
    // State Entry Function
    HUBDSM_WaitingForDevicePortEvents,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventPortStateDisabled ,   DsmStateIgnored },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventPortStateEnabled ,    DsmStateIgnored },
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSetInterface ,DsmStateCompleteWithStatusFailed },
        { DsmEventPortResetTimedOut ,   DsmStateIgnored },
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventClientRequestReset ,  DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestCycle ,  DsmStateIgnored },
        { DsmEventPortDisabled ,        DsmStateIgnored },
        { DsmEventPortFailed ,          DsmStateIgnored },
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventPortSuspended ,       DsmStateIgnored },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateIgnored },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateIgnored },
        { DsmEventPortStateSuspended ,  DsmStateIgnored },
        { DsmEventClientRequestSelectConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventPortResumed ,         DsmStateIgnored },
        { DsmEventPDOPreStart ,         DsmStateCompletePdoPreStart },
        { DsmEventHubResume ,           DsmStateFlushingHubPowerUpEvents },
        { DsmEventHubStart ,            DsmStateFlushingHubPowerUpEvents },
        { DsmEventHubStopAfterSuspend , DsmStateFlushingHubStopAfterSuspendEvent },
        { DsmEventPDOD0ExitFinal ,      DsmStateFlushingPnpEvents },
        { DsmEventHubResumeWithReset ,  DsmStateFlushingHubPowerUpEvents },
        { DsmEventHubSuspend ,          DsmStateDisablingPortOnHubSuspendInCleanup },
        { DsmEventPDOD0Entry ,          DsmStateFlushingPnpEvents },
        { DsmEventHubResumeInS0 ,       DsmStateFlushingHubPowerUpEvents },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInWaitingForDevicePortEvents },
        { DsmEventHubStop ,             DsmStateFlushingHubPowerDownEvents },
        { DsmEventPDOInstallMSOSExt ,   DsmStateFlushingPnpEvents },
        { DsmEventPDOD0Exit ,           DsmStateFlushingPnpEvents },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingHubPowerDownEventsWithPortOff = {
    // State ETW Name
    DsmStateEtwFlushingHubPowerDownEventsWithPortOff,
    // State Entry Function
    HUBDSM_FlushingHubPowerDownEventsWithPortOff,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEventsWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingHubPowerUpEventsWithPortOff = {
    // State ETW Name
    DsmStateEtwFlushingHubPowerUpEventsWithPortOff,
    // State Entry Function
    HUBDSM_FlushingHubPowerUpEventsWithPortOff,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEventsWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingHubStopAfterSuspendEventWithPortOff = {
    // State ETW Name
    DsmStateEtwFlushingHubStopAfterSuspendEventWithPortOff,
    // State Entry Function
    HUBDSM_FlushingHubStopAfterSuspendEventWithPortOff,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEventsWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingPnpEventsWithPortOff = {
    // State ETW Name
    DsmStateEtwFlushingPnpEventsWithPortOff,
    // State Entry Function
    HUBDSM_FlushingPnpEventsWithPortOff,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEventsWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInWaitingForDevicePortEventsWithPortOff = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInWaitingForDevicePortEventsWithPortOff,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEventsWithPortOff,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateWaitingForDevicePortEventsWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDevicePortEventsWithPortOff = {
    // State ETW Name
    DsmStateEtwWaitingForDevicePortEventsWithPortOff,
    // State Entry Function
    HUBDSM_WaitingForDevicePortEventsWithPortOff,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResetComplete ,   DsmStateIgnored },
        { DsmEventPortStateDisabled ,   DsmStateIgnored },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestCycle ,  DsmStateIgnored },
        { DsmEventClientRequestSetInterface ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestReset ,  DsmStateCompleteWithStatusFailed },
        { DsmEventPortDisabled ,        DsmStateIgnored },
        { DsmEventClientRequestSelectConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventPortResetTimedOut ,   DsmStateIgnored },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateIgnored },
        { DsmEventPortStateEnabled ,    DsmStateIgnored },
        { DsmEventPortSuspended ,       DsmStateIgnored },
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventPortFailed ,          DsmStateIgnored },
        { DsmEventPortStateSuspended ,  DsmStateIgnored },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventPDOPreStart ,         DsmStateCompletePdoPreStart },
        { DsmEventHubResume ,           DsmStateFlushingHubPowerUpEventsWithPortOff },
        { DsmEventHubStart ,            DsmStateFlushingHubPowerUpEventsWithPortOff },
        { DsmEventPDOD0ExitFinal ,      DsmStateFlushingPnpEventsWithPortOff },
        { DsmEventHubResumeWithReset ,  DsmStateFlushingHubPowerUpEventsWithPortOff },
        { DsmEventHubStop ,             DsmStateFlushingHubPowerDownEventsWithPortOff },
        { DsmEventPDOD0Entry ,          DsmStateFlushingPnpEventsWithPortOff },
        { DsmEventHubResumeInS0 ,       DsmStateFlushingHubPowerUpEventsWithPortOff },
        { DsmEventHubStopAfterSuspend , DsmStateFlushingHubStopAfterSuspendEventWithPortOff },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInWaitingForDevicePortEventsWithPortOff },
        { DsmEventHubSuspend ,          DsmStateFlushingHubPowerDownEventsWithPortOff },
        { DsmEventPDOInstallMSOSExt ,   DsmStateFlushingPnpEventsWithPortOff },
        { DsmEventPDOD0Exit ,           DsmStateFlushingPnpEventsWithPortOff },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingPortOnHubSuspendForFailedDevice = {
    // State ETW Name
    DsmStateEtwDisablingPortOnHubSuspendForFailedDevice,
    // State Entry Function
    HUBDSM_DisablingPortOnHubSuspendForFailedDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReleasingPowerReferenceOnDetachForFailedDevice },
        { DsmEventPortDisabled ,        DsmStateFailedDeviceHubSuspended },
        { DsmEventPortFailed ,          DsmStateFailedDeviceHubSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFailedDeviceHubSuspended = {
    // State ETW Name
    DsmStateEtwFailedDeviceHubSuspended,
    // State Entry Function
    HUBDSM_FailedDeviceHubSuspended,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    DsmSubSmFailedDeviceHubSuspended,
    // Event State Pairs for Transitions
    {
        { DsmEventHubStopWithReferenceAcquired ,DsmStateFlushingHubPowerDownEventsForFailedDevice },
        { DsmEventDeviceRenumerateOnHubResume ,DsmStateWaitingForDevicePortEventsForFailedDevice },
        { DsmEventDeviceSuspendedOnHubResume ,DsmStateWaitingForDevicePortEventsForFailedDevice },
        { DsmEventDeviceResumedOnHubResume ,DsmStateWaitingForDevicePortEventsForFailedDevice },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceForFailedDevice },
        { DsmEventHubStopAfterSuspend , DsmStateFlushingHubStopAfterSuspendEventForFailedDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingHubPowerDownEventsForFailedDevice = {
    // State ETW Name
    DsmStateEtwFlushingHubPowerDownEventsForFailedDevice,
    // State Entry Function
    HUBDSM_FlushingHubPowerDownEventsForFailedDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEventsForFailedDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingHubPowerUpEventsForFailedDevice = {
    // State ETW Name
    DsmStateEtwFlushingHubPowerUpEventsForFailedDevice,
    // State Entry Function
    HUBDSM_FlushingHubPowerUpEventsForFailedDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEventsForFailedDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingHubStopAfterSuspendEventForFailedDevice = {
    // State ETW Name
    DsmStateEtwFlushingHubStopAfterSuspendEventForFailedDevice,
    // State Entry Function
    HUBDSM_FlushingHubStopAfterSuspendEventForFailedDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEventsForFailedDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingPnpEventsForFailedDevice = {
    // State ETW Name
    DsmStateEtwFlushingPnpEventsForFailedDevice,
    // State Entry Function
    HUBDSM_FlushingPnpEventsForFailedDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEventsForFailedDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnDetachForFailedDevice = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnDetachForFailedDevice,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnDetachForFailedDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningPortDetachDeviceForFailedDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDetachDeviceForFailedDevice = {
    // State ETW Name
    DsmStateEtwReturningPortDetachDeviceForFailedDevice,
    // State Entry Function
    HUBDSM_ReturningPortDetachDeviceForFailedDevice,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDevicePortEventsForFailedDevice = {
    // State ETW Name
    DsmStateEtwWaitingForDevicePortEventsForFailedDevice,
    // State Entry Function
    HUBDSM_WaitingForDevicePortEventsForFailedDevice,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventPDOPreStart ,         DsmStateCompletePdoPreStart },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventPortStateEnabled ,    DsmStateIgnored },
        { DsmEventPortStateDisabled ,   DsmStateIgnored },
        { DsmEventPortStateSuspended ,  DsmStateIgnored },
        { DsmEventPDOD0ExitFinal ,      DsmStateFlushingPnpEventsForFailedDevice },
        { DsmEventHubResumeInS0 ,       DsmStateFlushingHubPowerUpEventsForFailedDevice },
        { DsmEventHubSuspend ,          DsmStateDisablingPortOnHubSuspendForFailedDevice },
        { DsmEventHubStart ,            DsmStateFlushingHubPowerUpEventsForFailedDevice },
        { DsmEventHubStopAfterSuspend , DsmStateFlushingHubStopAfterSuspendEventForFailedDevice },
        { DsmEventHubStop ,             DsmStateFlushingHubPowerDownEventsForFailedDevice },
        { DsmEventPDOInstallMSOSExt ,   DsmStateFlushingPnpEventsForFailedDevice },
        { DsmEventPDOD0Exit ,           DsmStateFlushingPnpEventsForFailedDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingHubStartResumeEvent = {
    // State ETW Name
    DsmStateEtwFlushingHubStartResumeEvent,
    // State Entry Function
    HUBDSM_FlushingHubStartResumeEvent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEventsExceptStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingHubSuspendEvent = {
    // State ETW Name
    DsmStateEtwFlushingHubSuspendEvent,
    // State Entry Function
    HUBDSM_FlushingHubSuspendEvent,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEventsExceptStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingPnpEventExceptStop = {
    // State ETW Name
    DsmStateEtwFlushingPnpEventExceptStop,
    // State Entry Function
    HUBDSM_FlushingPnpEventExceptStop,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDevicePortEventsExceptStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFowardingStreamsRequestToUcxInWaitingForDevicePortEventsExceptStop = {
    // State ETW Name
    DsmStateEtwFowardingStreamsRequestToUcxInWaitingForDevicePortEventsExceptStop,
    // State Entry Function
    HUBDSM_FowardingStreamsRequestToUcxInWaitingForDevicePortEventsExceptStop,
    // State Flags
    StateFlagHandlesCriticalEventsOnly|StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXClientRequestComplete ,DsmStateWaitingForDevicePortEventsExceptStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDevicePortEventsExceptStop = {
    // State ETW Name
    DsmStateEtwWaitingForDevicePortEventsExceptStop,
    // State Entry Function
    HUBDSM_WaitingForDevicePortEventsExceptStop,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateIgnored },
        { DsmEventPortDisabled ,        DsmStateIgnored },
        { DsmEventPortSuspended ,       DsmStateIgnored },
        { DsmEventPortResetComplete ,   DsmStateIgnored },
        { DsmEventClientRequestSelectConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestReset ,  DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestCycle ,  DsmStateIgnored },
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSetInterface ,DsmStateCompleteWithStatusFailed },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventPortStateSuspended ,  DsmStateIgnored },
        { DsmEventPortStateEnabled ,    DsmStateIgnored },
        { DsmEventPortResetTimedOut ,   DsmStateIgnored },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventPortFailed ,          DsmStateIgnored },
        { DsmEventPortStateDisabled ,   DsmStateIgnored },
        { DsmEventClientRequestSelectNullConfiguration ,DsmStateCompleteWithStatusFailed },
        { DsmEventPDOPreStart ,         DsmStateCompletePdoPreStart },
        { DsmEventHubResume ,           DsmStateFlushingHubStartResumeEvent },
        { DsmEventHubResumeWithReset ,  DsmStateFlushingHubStartResumeEvent },
        { DsmEventPDOD0Exit ,           DsmStateFlushingPnpEventExceptStop },
        { DsmEventHubStart ,            DsmStateFlushingHubStartResumeEvent },
        { DsmEventHubResumeInS0 ,       DsmStateFlushingHubStartResumeEvent },
        { DsmEventHubSuspend ,          DsmStateFlushingHubSuspendEvent },
        { DsmEventClientRequestOpenOrCloseStreams ,DsmStateFowardingStreamsRequestToUcxInWaitingForDevicePortEventsExceptStop },
        { DsmEventPDOInstallMSOSExt ,   DsmStateFlushingPnpEventExceptStop },
        { DsmEventPDOD0Entry ,          DsmStateFlushingPnpEventExceptStop },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingHubResumeEventForBootDevice = {
    // State ETW Name
    DsmStateEtwFlushingHubResumeEventForBootDevice,
    // State Entry Function
    HUBDSM_FlushingHubResumeEventForBootDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDeviceHubEventsForBootDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingHubSuspendEventForBootDevice = {
    // State ETW Name
    DsmStateEtwFlushingHubSuspendEventForBootDevice,
    // State Entry Function
    HUBDSM_FlushingHubSuspendEventForBootDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDeviceHubEventsForBootDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFlushingPnpEventsForBootDevice = {
    // State ETW Name
    DsmStateEtwFlushingPnpEventsForBootDevice,
    // State Entry Function
    HUBDSM_FlushingPnpEventsForBootDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForDeviceHubEventsForBootDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForDeviceHubEventsForBootDevice = {
    // State ETW Name
    DsmStateEtwWaitingForDeviceHubEventsForBootDevice,
    // State Entry Function
    HUBDSM_WaitingForDeviceHubEventsForBootDevice,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventClientRequestResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSyncClearStall ,DsmStateCompleteWithStatusFailed },
        { DsmEventClientRequestSyncResetPipe ,DsmStateCompleteWithStatusFailed },
        { DsmEventLPMPowerSettingChange ,DsmStateIgnored },
        { DsmEventFDORequestGetDescriptor ,DsmStateCompleteFDORequestWithStatusFailed },
        { DsmEventNoPingResponseError , DsmStateIgnored },
        { DsmEventHubResume ,           DsmStateFlushingHubResumeEventForBootDevice },
        { DsmEventPDOInstallMSOSExt ,   DsmStateFlushingPnpEventsForBootDevice },
        { DsmEventHubResumeWithReset ,  DsmStateFlushingHubResumeEventForBootDevice },
        { DsmEventHubResumeInS0 ,       DsmStateFlushingHubResumeEventForBootDevice },
        { DsmEventHubSuspend ,          DsmStateFlushingHubSuspendEventForBootDevice },
        { DsmEventPDOD0Exit ,           DsmStateFlushingPnpEventsForBootDevice },
        { DsmEventPDOD0Entry ,          DsmStateFlushingPnpEventsForBootDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAckingPortEventInD3Cold = {
    // State ETW Name
    DsmStateEtwAckingPortEventInD3Cold,
    // State Entry Function
    HUBDSM_AckingPortEventInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDisabledAfterD3Cold },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAckingPortResumed = {
    // State ETW Name
    DsmStateEtwAckingPortResumed,
    // State Entry Function
    HUBDSM_AckingPortResumed,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfDeviceArmedForWakeOnResume },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDeviceArmedForWakeOnResume = {
    // State ETW Name
    DsmStateEtwCheckingIfDeviceArmedForWakeOnResume,
    // State Entry Function
    HUBDSM_CheckingIfDeviceArmedForWakeOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateStartingDeviceIoOnDeviceResumeOnHwWake },
        { DsmEventNo ,                  DsmStateSuspendingBackUnarmedDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDeviceNeedsResetOnResumeInS0 = {
    // State ETW Name
    DsmStateEtwCheckingIfDeviceNeedsResetOnResumeInS0,
    // State Entry Function
    HUBDSM_CheckingIfDeviceNeedsResetOnResumeInS0,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateCheckingIfResetOnLastResumeFlagIsSet },
        { DsmEventYes ,                 DsmStateSettingResetOnLastResumeFlagForPDO },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDeviceShouldBeDisarmedForWakeOnResume = {
    // State ETW Name
    DsmStateEtwCheckingIfDeviceShouldBeDisarmedForWakeOnResume,
    // State Entry Function
    HUBDSM_CheckingIfDeviceShouldBeDisarmedForWakeOnResume,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateCheckingIfDeviceNeedsResetOnResumeInS0 },
        { DsmEventYes ,                 DsmStateDisarmingDeviceForWakeOnD0Entry },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfResetOnLastResumeFlagIsSet = {
    // State ETW Name
    DsmStateEtwCheckingIfResetOnLastResumeFlagIsSet,
    // State Entry Function
    HUBDSM_CheckingIfResetOnLastResumeFlagIsSet,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateReturningRenumerateDeviceOnResume },
        { DsmEventNo ,                  DsmStateReturningDeviceResumed },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfResetOnLastResumeFlagIsSetOnSuspended = {
    // State ETW Name
    DsmStateEtwCheckingIfResetOnLastResumeFlagIsSetOnSuspended,
    // State Entry Function
    HUBDSM_CheckingIfResetOnLastResumeFlagIsSetOnSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateWaitingForD0EntryOnPendingRenumerate },
        { DsmEventNo ,                  DsmStateSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingD0EntryOnDetach = {
    // State ETW Name
    DsmStateEtwCompletingD0EntryOnDetach,
    // State Entry Function
    HUBDSM_CompletingD0EntryOnDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningDetachDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingD0EntryOnPortFailure = {
    // State ETW Name
    DsmStateEtwCompletingD0EntryOnPortFailure,
    // State Entry Function
    HUBDSM_CompletingD0EntryOnPortFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningPortFailureOnResume },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisabledAfterD3Cold = {
    // State ETW Name
    DsmStateEtwDisabledAfterD3Cold,
    // State Entry Function
    HUBDSM_DisabledAfterD3Cold,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOD0Entry ,          DsmStateReturningRenumerateDeviceOnResume },
        { DsmEventHubSuspend ,          DsmStateSuspendedWithHubSuspendedInD3Cold },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingOnHubSuspendNeedingRenumeration = {
    // State ETW Name
    DsmStateEtwDisablingOnHubSuspendNeedingRenumeration,
    // State Entry Function
    HUBDSM_DisablingOnHubSuspendNeedingRenumeration,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReleasingPowerReferenceOnDetachWhileSuspending },
        { DsmEventPortFailed ,          DsmStateReturningPortFailureOnResume },
        { DsmEventPortDisabled ,        DsmStateSuspendedWithHubSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingPortOnStopFromSuspended = {
    // State ETW Name
    DsmStateEtwDisablingPortOnStopFromSuspended,
    // State Entry Function
    HUBDSM_DisablingPortOnStopFromSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReturningDetachDevice },
        { DsmEventPortFailed ,          DsmStateReturningHubStopFromSuspended },
        { DsmEventPortDisabled ,        DsmStateReturningHubStopFromSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisarmingDeviceForWakeOnD0Entry = {
    // State ETW Name
    DsmStateEtwDisarmingDeviceForWakeOnD0Entry,
    // State Entry Function
    HUBDSM_DisarmingDeviceForWakeOnD0Entry,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateCheckingIfDeviceNeedsResetOnResumeInS0 },
        { DsmEventTransferFailure ,     DsmStateCheckingIfDeviceNeedsResetOnResumeInS0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisarmingDeviceForWakeOnPortResumed = {
    // State ETW Name
    DsmStateEtwDisarmingDeviceForWakeOnPortResumed,
    // State Entry Function
    HUBDSM_DisarmingDeviceForWakeOnPortResumed,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateCheckingIfDeviceNeedsResetOnResumeInS0 },
        { DsmEventTransferFailure ,     DsmStateCheckingIfDeviceNeedsResetOnResumeInS0 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPurgingDeviceIoOnHubSuspendWhileWaitingForD0Entry = {
    // State ETW Name
    DsmStateEtwPurgingDeviceIoOnHubSuspendWhileWaitingForD0Entry,
    // State Entry Function
    HUBDSM_PurgingDeviceIoOnHubSuspendWhileWaitingForD0Entry,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDisablingOnHubSuspendNeedingRenumeration },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPurgingDeviceIoOnHubSuspendWhileWaitingForD0EntryOnHwWake = {
    // State ETW Name
    DsmStateEtwPurgingDeviceIoOnHubSuspendWhileWaitingForD0EntryOnHwWake,
    // State Entry Function
    HUBDSM_PurgingDeviceIoOnHubSuspendWhileWaitingForD0EntryOnHwWake,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateSuspendingWakeArmedDeviceOnHubSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnDetachWhileSuspending = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnDetachWhileSuspending,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnDetachWhileSuspending,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningDetachDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnDetachWhileWaitingForHubSuspend = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnDetachWhileWaitingForHubSuspend,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnDetachWhileWaitingForHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningDetachDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReleasingPowerReferenceOnPortFailureWhileWaitingForHubSuspend = {
    // State ETW Name
    DsmStateEtwReleasingPowerReferenceOnPortFailureWhileWaitingForHubSuspend,
    // State Entry Function
    HUBDSM_ReleasingPowerReferenceOnPortFailureWhileWaitingForHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningPortFailureOnResume },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryResumingFromSelectiveSuspend = {
    // State ETW Name
    DsmStateEtwResumingFromSelectiveSuspend,
    // State Entry Function
    HUBDSM_ResumingFromSelectiveSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortFailed ,          DsmStateCompletingD0EntryOnPortFailure },
        { DsmEventPortResumed ,         DsmStateStartingDeviceIoOnDeviceResumeOnSwWake },
        { DsmEventPortResumeTimedOut ,  DsmStateCompletingD0EntryOnPortFailure },
        { DsmEventPortDetachDevice ,    DsmStateCompletingD0EntryOnDetach },
        { DsmEventPortStateDisabled ,   DsmStateSettingResetOnLastResumeFlagForPDOAfterD3Cold },
        { DsmEventPortResumeFailedDueToPendingSuspend ,DsmStateCompletingD0EntryOnPortFailure },
        { DsmEventPortStateEnabledOnReconnect ,DsmStateSettingResetOnLastResumeFlagForPDOAfterD3Cold },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningDetachDevice = {
    // State ETW Name
    DsmStateEtwReturningDetachDevice,
    // State Entry Function
    HUBDSM_ReturningDetachDevice,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningDeviceResumed = {
    // State ETW Name
    DsmStateEtwReturningDeviceResumed,
    // State Entry Function
    HUBDSM_ReturningDeviceResumed,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubStopFromSuspended = {
    // State ETW Name
    DsmStateEtwReturningHubStopFromSuspended,
    // State Entry Function
    HUBDSM_ReturningHubStopFromSuspended,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortFailureOnResume = {
    // State ETW Name
    DsmStateEtwReturningPortFailureOnResume,
    // State Entry Function
    HUBDSM_ReturningPortFailureOnResume,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningRenumerateDeviceOnResume = {
    // State ETW Name
    DsmStateEtwReturningRenumerateDeviceOnResume,
    // State Entry Function
    HUBDSM_ReturningRenumerateDeviceOnResume,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingResetOnLastResumeDueToD3Cold = {
    // State ETW Name
    DsmStateEtwSettingResetOnLastResumeDueToD3Cold,
    // State Entry Function
    HUBDSM_SettingResetOnLastResumeDueToD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateAckingPortResumed },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingResetOnLastResumeFlagForPDO = {
    // State ETW Name
    DsmStateEtwSettingResetOnLastResumeFlagForPDO,
    // State Entry Function
    HUBDSM_SettingResetOnLastResumeFlagForPDO,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningRenumerateDeviceOnResume },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingResetOnLastResumeFlagForPDOAfterD3Cold = {
    // State ETW Name
    DsmStateEtwSettingResetOnLastResumeFlagForPDOAfterD3Cold,
    // State Entry Function
    HUBDSM_SettingResetOnLastResumeFlagForPDOAfterD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningRenumerateDeviceOnResume },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStartingDeviceIoOnDeviceResumeOnHwWake = {
    // State ETW Name
    DsmStateEtwStartingDeviceIoOnDeviceResumeOnHwWake,
    // State Entry Function
    HUBDSM_StartingDeviceIoOnDeviceResumeOnHwWake,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlFailure ,     DsmStateWaitingForD0EntryOnHwWake },
        { DsmEventUCXIoctlSuccess ,     DsmStateWaitingForD0EntryOnHwWake },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStartingDeviceIoOnDeviceResumeOnSwWake = {
    // State ETW Name
    DsmStateEtwStartingDeviceIoOnDeviceResumeOnSwWake,
    // State Entry Function
    HUBDSM_StartingDeviceIoOnDeviceResumeOnSwWake,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCheckingIfDeviceShouldBeDisarmedForWakeOnResume },
        { DsmEventUCXIoctlFailure ,     DsmStateCheckingIfDeviceShouldBeDisarmedForWakeOnResume },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySuspended = {
    // State ETW Name
    DsmStateEtwSuspended,
    // State Entry Function
    HUBDSM_Suspended,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortStateEnabledOnReconnect ,DsmStateSettingResetOnLastResumeDueToD3Cold },
        { DsmEventHubSuspend ,          DsmStateSuspendedWithHubSuspended },
        { DsmEventPDOD0Entry ,          DsmStateResumingFromSelectiveSuspend },
        { DsmEventPortResumed ,         DsmStateAckingPortResumed },
        { DsmEventPortStateDisabled ,   DsmStateAckingPortEventInD3Cold },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySuspendedWithHubSuspended = {
    // State ETW Name
    DsmStateEtwSuspendedWithHubSuspended,
    // State Entry Function
    HUBDSM_SuspendedWithHubSuspended,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    DsmSubSmSuspendedWithHubSuspended,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceRenumerateOnHubResume ,DsmStateWaitingForD0EntryOnPendingRenumerate },
        { DsmEventDeviceResumedOnHubResume ,DsmStateCheckingIfDeviceArmedForWakeOnResume },
        { DsmEventDeviceSuspendedOnHubResume ,DsmStateCheckingIfResetOnLastResumeFlagIsSetOnSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySuspendedWithHubSuspendedInD3Cold = {
    // State ETW Name
    DsmStateEtwSuspendedWithHubSuspendedInD3Cold,
    // State Entry Function
    HUBDSM_SuspendedWithHubSuspendedInD3Cold,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    DsmSubSmSuspendedWithHubSuspendedInD3Cold,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceResumedOnHubResume ,DsmStateDisabledAfterD3Cold },
        { DsmEventDeviceSuspendedOnHubResume ,DsmStateDisabledAfterD3Cold },
        { DsmEventDeviceRenumerateOnHubResume ,DsmStateDisabledAfterD3Cold },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySuspendingBackUnarmedDevice = {
    // State ETW Name
    DsmStateEtwSuspendingBackUnarmedDevice,
    // State Entry Function
    HUBDSM_SuspendingBackUnarmedDevice,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortSuspended ,       DsmStateSuspended },
        { DsmEventHubSuspend ,          DsmStateWaitingForSuspendCompleteOnHubSuspend },
        { DsmEventPortDetachDevice ,    DsmStateReturningDetachDevice },
        { DsmEventPortFailed ,          DsmStateReturningPortFailureOnResume },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySuspendingWakeArmedDeviceOnHubSuspend = {
    // State ETW Name
    DsmStateEtwSuspendingWakeArmedDeviceOnHubSuspend,
    // State Entry Function
    HUBDSM_SuspendingWakeArmedDeviceOnHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForSuspendCompleteOnHubSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForD0EntryOnHwWake = {
    // State ETW Name
    DsmStateEtwWaitingForD0EntryOnHwWake,
    // State Entry Function
    HUBDSM_WaitingForD0EntryOnHwWake,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOD0Entry ,          DsmStateDisarmingDeviceForWakeOnPortResumed },
        { DsmEventHubSuspend ,          DsmStatePurgingDeviceIoOnHubSuspendWhileWaitingForD0EntryOnHwWake },
        { DsmEventHubStop ,             DsmStateDisablingPortOnStopFromSuspended },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForD0EntryOnPendingRenumerate = {
    // State ETW Name
    DsmStateEtwWaitingForD0EntryOnPendingRenumerate,
    // State Entry Function
    HUBDSM_WaitingForD0EntryOnPendingRenumerate,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOD0Entry ,          DsmStateSettingResetOnLastResumeFlagForPDO },
        { DsmEventHubSuspend ,          DsmStatePurgingDeviceIoOnHubSuspendWhileWaitingForD0Entry },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForSuspendCompleteOnHubSuspend = {
    // State ETW Name
    DsmStateEtwWaitingForSuspendCompleteOnHubSuspend,
    // State Entry Function
    HUBDSM_WaitingForSuspendCompleteOnHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortSuspended ,       DsmStateSuspendedWithHubSuspended },
        { DsmEventPortDetachDevice ,    DsmStateReleasingPowerReferenceOnDetachWhileWaitingForHubSuspend },
        { DsmEventPortFailed ,          DsmStateReleasingPowerReferenceOnPortFailureWhileWaitingForHubSuspend },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAbortingDeviceIoInSuspendingBeforeArmingForWakeFailure = {
    // State ETW Name
    DsmStateEtwAbortingDeviceIoInSuspendingBeforeArmingForWakeFailure,
    // State Entry Function
    HUBDSM_AbortingDeviceIoInSuspendingBeforeArmingForWakeFailure,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateArmingDeviceForWake },
        { DsmEventUCXIoctlFailure ,     DsmStateArmingDeviceForWake },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryArmingDeviceForWake = {
    // State ETW Name
    DsmStateEtwArmingDeviceForWake,
    // State Entry Function
    HUBDSM_ArmingDeviceForWake,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStatePurgingDeviceIoInSuspendingAfterArmingForWake },
        { DsmEventTransferFailure ,     DsmStatePurgingDeviceIoInSuspendingAfterArmingForWakeFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDeviceArmedForWakeOnSuspending = {
    // State ETW Name
    DsmStateEtwCheckingIfDeviceArmedForWakeOnSuspending,
    // State Entry Function
    HUBDSM_CheckingIfDeviceArmedForWakeOnSuspending,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStatePurgingDeviceIoInSuspending },
        { DsmEventYes ,                 DsmStateAbortingDeviceIoInSuspendingBeforeArmingForWakeFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingWaitWakeOnDetachDuringSuspending = {
    // State ETW Name
    DsmStateEtwCompletingWaitWakeOnDetachDuringSuspending,
    // State Entry Function
    HUBDSM_CompletingWaitWakeOnDetachDuringSuspending,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningPortDetachDeviceInSuspending },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCompletingWaitWakeOnSuspendFailure = {
    // State ETW Name
    DsmStateEtwCompletingWaitWakeOnSuspendFailure,
    // State Entry Function
    HUBDSM_CompletingWaitWakeOnSuspendFailure,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationFailureInSuspending },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPurgingDeviceIoInSuspending = {
    // State ETW Name
    DsmStateEtwPurgingDeviceIoInSuspending,
    // State Entry Function
    HUBDSM_PurgingDeviceIoInSuspending,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateSuspending },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPurgingDeviceIoInSuspendingAfterArmingForWake = {
    // State ETW Name
    DsmStateEtwPurgingDeviceIoInSuspendingAfterArmingForWake,
    // State Entry Function
    HUBDSM_PurgingDeviceIoInSuspendingAfterArmingForWake,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateSuspendingWithArmedForWake },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPurgingDeviceIoInSuspendingAfterArmingForWakeFailure = {
    // State ETW Name
    DsmStateEtwPurgingDeviceIoInSuspendingAfterArmingForWakeFailure,
    // State Entry Function
    HUBDSM_PurgingDeviceIoInSuspendingAfterArmingForWakeFailure,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateSuspendingPortOnFailureInSuspending },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureInSuspending = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureInSuspending,
    // State Entry Function
    HUBDSM_ReturningOperationFailureInSuspending,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInSuspending = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInSuspending,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInSuspending,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDetachDeviceInSuspending = {
    // State ETW Name
    DsmStateEtwReturningPortDetachDeviceInSuspending,
    // State Entry Function
    HUBDSM_ReturningPortDetachDeviceInSuspending,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningUnexpectedHubSuspendInSuspending = {
    // State ETW Name
    DsmStateEtwReturningUnexpectedHubSuspendInSuspending,
    // State Entry Function
    HUBDSM_ReturningUnexpectedHubSuspendInSuspending,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySuspending = {
    // State ETW Name
    DsmStateEtwSuspending,
    // State Entry Function
    HUBDSM_Suspending,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortSuspended ,       DsmStateReturningOperationSuccessInSuspending },
        { DsmEventHubSuspend ,          DsmStateWaitingForPortResponseOnHubSuspend },
        { DsmEventPortFailed ,          DsmStateReturningOperationFailureInSuspending },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInSuspending },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySuspendingPortOnFailureInSuspending = {
    // State ETW Name
    DsmStateEtwSuspendingPortOnFailureInSuspending,
    // State Entry Function
    HUBDSM_SuspendingPortOnFailureInSuspending,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortFailed ,          DsmStateCompletingWaitWakeOnSuspendFailure },
        { DsmEventHubSuspend ,          DsmStateWaitingForPortResponseOnHubSuspend },
        { DsmEventPortSuspended ,       DsmStateCompletingWaitWakeOnSuspendFailure },
        { DsmEventPortDetachDevice ,    DsmStateCompletingWaitWakeOnDetachDuringSuspending },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySuspendingWithArmedForWake = {
    // State ETW Name
    DsmStateEtwSuspendingWithArmedForWake,
    // State Entry Function
    HUBDSM_SuspendingWithArmedForWake,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortFailed ,          DsmStateCompletingWaitWakeOnSuspendFailure },
        { DsmEventHubSuspend ,          DsmStateWaitingForPortResponseOnHubSuspend },
        { DsmEventPortSuspended ,       DsmStateReturningOperationSuccessInSuspending },
        { DsmEventPortDetachDevice ,    DsmStateCompletingWaitWakeOnDetachDuringSuspending },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPortResponseOnHubSuspend = {
    // State ETW Name
    DsmStateEtwWaitingForPortResponseOnHubSuspend,
    // State Entry Function
    HUBDSM_WaitingForPortResponseOnHubSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortSuspended ,       DsmStateReturningUnexpectedHubSuspendInSuspending },
        { DsmEventPortFailed ,          DsmStateReturningUnexpectedHubSuspendInSuspending },
        { DsmEventPortDisabled ,        DsmStateReturningUnexpectedHubSuspendInSuspending },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInSuspending },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCreatingNewEndpointsInSettingInterface = {
    // State ETW Name
    DsmStateEtwCreatingNewEndpointsInSettingInterface,
    // State Entry Function
    HUBDSM_CreatingNewEndpointsInSettingInterface,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateProgrammingEndpointsAndSettingLPMValuesInSettingInterface },
        { DsmEventOperationFailure ,    DsmStateDisablingEndpointsForTheCurrentInterface },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingEndpointsForNewInterfaceOnFailure = {
    // State ETW Name
    DsmStateEtwDeletingEndpointsForNewInterfaceOnFailure,
    // State Entry Function
    HUBDSM_DeletingEndpointsForNewInterfaceOnFailure,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateDeletingEndpointsForOldInterfaceOnFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingEndpointsForOldInterface = {
    // State ETW Name
    DsmStateEtwDeletingEndpointsForOldInterface,
    // State Entry Function
    HUBDSM_DeletingEndpointsForOldInterface,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationSuccessInSettingInterface },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeletingEndpointsForOldInterfaceOnFailure = {
    // State ETW Name
    DsmStateEtwDeletingEndpointsForOldInterfaceOnFailure,
    // State Entry Function
    HUBDSM_DeletingEndpointsForOldInterfaceOnFailure,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationFailureInSettingInterface },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsForTheCurrentInterface = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsForTheCurrentInterface,
    // State Entry Function
    HUBDSM_DisablingEndpointsForTheCurrentInterface,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateSettingDeviceInterfaceInSettingInterfaceOnFailureForCompat },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsForTheNewInterfaceOnFailure = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsForTheNewInterfaceOnFailure,
    // State Entry Function
    HUBDSM_DisablingEndpointsForTheNewInterfaceOnFailure,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateDeletingEndpointsForNewInterfaceOnFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryPreparingEndpointAndInterfaceListsOnSelectInterface = {
    // State ETW Name
    DsmStateEtwPreparingEndpointAndInterfaceListsOnSelectInterface,
    // State Entry Function
    HUBDSM_PreparingEndpointAndInterfaceListsOnSelectInterface,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCreatingNewEndpointsInSettingInterface },
        { DsmEventOperationFailure ,    DsmStateDeletingEndpointsForOldInterfaceOnFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryProgrammingEndpointsAndSettingLPMValuesInSettingInterface = {
    // State ETW Name
    DsmStateEtwProgrammingEndpointsAndSettingLPMValuesInSettingInterface,
    // State Entry Function
    HUBDSM_ProgrammingEndpointsAndSettingLPMValuesInSettingInterface,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    DsmSubSmProgrammingEndpointsAndSettingLPMValuesInSettingInterface,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateSettingDeviceInterfaceInSettingInterface },
        { DsmEventOperationFailure ,    DsmStateSettingDeviceInterfaceInSettingInterfaceOnFailureForCompat },
        { DsmEventContinueIfNoSubState ,DsmStateProgrammingEndpointsInControllerInSettingInterfaceFor20Devices },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryProgrammingEndpointsInControllerInSettingInterfaceFor20Devices = {
    // State ETW Name
    DsmStateEtwProgrammingEndpointsInControllerInSettingInterfaceFor20Devices,
    // State Entry Function
    HUBDSM_ProgrammingEndpointsInControllerInSettingInterfaceFor20Devices,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateSettingDeviceInterfaceInSettingInterface },
        { DsmEventUCXIoctlFailure ,     DsmStateSettingDeviceInterfaceInSettingInterfaceOnFailureForCompat },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureInSettingInterface = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureInSettingInterface,
    // State Entry Function
    HUBDSM_ReturningOperationFailureInSettingInterface,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInSettingInterface = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInSettingInterface,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInSettingInterface,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingDeviceInterfaceInSettingInterface = {
    // State ETW Name
    DsmStateEtwSettingDeviceInterfaceInSettingInterface,
    // State Entry Function
    HUBDSM_SettingDeviceInterfaceInSettingInterface,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateDeletingEndpointsForOldInterface },
        { DsmEventTransferFailure ,     DsmStateDisablingEndpointsForTheNewInterfaceOnFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingDeviceInterfaceInSettingInterfaceOnFailureForCompat = {
    // State ETW Name
    DsmStateEtwSettingDeviceInterfaceInSettingInterfaceOnFailureForCompat,
    // State Entry Function
    HUBDSM_SettingDeviceInterfaceInSettingInterfaceOnFailureForCompat,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateDeletingEndpointsForNewInterfaceOnFailure },
        { DsmEventTransferFailure ,     DsmStateDeletingEndpointsForNewInterfaceOnFailure },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfDeviceSpeedChanged = {
    // State ETW Name
    DsmStateEtwCheckingIfDeviceSpeedChanged,
    // State Entry Function
    HUBDSM_CheckingIfDeviceSpeedChanged,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateWaitingForPostReset1TimerInRenum },
        { DsmEventYes ,                 DsmStateReturningDeviceOperationFailureInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfFirstEnumTryInRenum = {
    // State ETW Name
    DsmStateEtwCheckingIfFirstEnumTryInRenum,
    // State Entry Function
    HUBDSM_CheckingIfFirstEnumTryInRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateWaitingForPostReset2TimerInRenum },
        { DsmEventNo ,                  DsmStateWaitingForPostReset2ExtendedTimerInRenum },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingDeviceDescriptorInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwGettingDeviceDescriptorInReEnumAtZero,
    // State Entry Function
    HUBDSM_GettingDeviceDescriptorInReEnumAtZero,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingDeviceDescriptorInReEnumAtZero },
        { DsmEventTransferFailure ,     DsmStateValidatingDeviceDescriptorInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryLoggingReset1TimeoutInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwLoggingReset1TimeoutInReEnumAtZero,
    // State Entry Function
    HUBDSM_LoggingReset1TimeoutInReEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningDeviceOperationFailureInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryLoggingReset2TimeoutInReEnum = {
    // State ETW Name
    DsmStateEtwLoggingReset2TimeoutInReEnum,
    // State Entry Function
    HUBDSM_LoggingReset2TimeoutInReEnum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningDeviceOperationFailureInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryNotifyingDeviceResetToUCXAfterReset1InRenum = {
    // State ETW Name
    DsmStateEtwNotifyingDeviceResetToUCXAfterReset1InRenum,
    // State Entry Function
    HUBDSM_NotifyingDeviceResetToUCXAfterReset1InRenum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCheckingIfDeviceSpeedChanged },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryNotifyingDeviceResetToUCXAfterReset2InRenum = {
    // State ETW Name
    DsmStateEtwNotifyingDeviceResetToUCXAfterReset2InRenum,
    // State Entry Function
    HUBDSM_NotifyingDeviceResetToUCXAfterReset2InRenum,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCheckingIfFirstEnumTryInRenum },
        { DsmEventUCXIoctlFailure ,     DsmStateReturningDeviceOperationFailureInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryResetting1InRenum = {
    // State ETW Name
    DsmStateEtwResetting1InRenum,
    // State Entry Function
    HUBDSM_Resetting1InRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResetComplete ,   DsmStateNotifyingDeviceResetToUCXAfterReset1InRenum },
        { DsmEventHubStop ,             DsmStateWaitingForPortResetOnHubStopInReEnumAtZero },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateWaitingForStopSuspendOnReset1InReEnumAtZero },
        { DsmEventHubSuspend ,          DsmStateWaitingForPortResetOnHubSuspendInReEnumAtZero },
        { DsmEventPortResetTimedOut ,   DsmStateLoggingReset1TimeoutInReEnumAtZero },
        { DsmEventPortFailed ,          DsmStateReturningDeviceOperationFailureInReEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryResetting2InRenum = {
    // State ETW Name
    DsmStateEtwResetting2InRenum,
    // State Entry Function
    HUBDSM_Resetting2InRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResetComplete ,   DsmStateNotifyingDeviceResetToUCXAfterReset2InRenum },
        { DsmEventPortResetTimedOut ,   DsmStateLoggingReset2TimeoutInReEnum },
        { DsmEventPortFailed ,          DsmStateReturningDeviceOperationFailureInReEnumAtZero },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateWaitingForStopSuspendOnReset1InReEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInReEnumAtZero },
        { DsmEventHubStop ,             DsmStateWaitingForPortResetOnHubStopInReEnumAtZero },
        { DsmEventHubSuspend ,          DsmStateWaitingForPortResetOnHubSuspendInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningDeviceOperationFailureInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwReturningDeviceOperationFailureInReEnumAtZero,
    // State Entry Function
    HUBDSM_ReturningDeviceOperationFailureInReEnumAtZero,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningDeviceOperationSuccessInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwReturningDeviceOperationSuccessInReEnumAtZero,
    // State Entry Function
    HUBDSM_ReturningDeviceOperationSuccessInReEnumAtZero,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubStopInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwReturningHubStopInReEnumAtZero,
    // State Entry Function
    HUBDSM_ReturningHubStopInReEnumAtZero,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubSuspendInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwReturningHubSuspendInReEnumAtZero,
    // State Entry Function
    HUBDSM_ReturningHubSuspendInReEnumAtZero,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDetachDeviceInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwReturningPortDetachDeviceInReEnumAtZero,
    // State Entry Function
    HUBDSM_ReturningPortDetachDeviceInReEnumAtZero,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppingTimerOnHubStopInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwStoppingTimerOnHubStopInReEnumAtZero,
    // State Entry Function
    HUBDSM_StoppingTimerOnHubStopInReEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerOnHubStopInReEnumAtZero },
        { DsmEventOperationSuccess ,    DsmStateReturningHubStopInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppingTimerOnHubSuspendInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwStoppingTimerOnHubSuspendInReEnumAtZero,
    // State Entry Function
    HUBDSM_StoppingTimerOnHubSuspendInReEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerOnHubSuspendInReEnumAtZero },
        { DsmEventOperationSuccess ,    DsmStateReturningHubSuspendInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppingTimerOnPortDetachInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwStoppingTimerOnPortDetachInReEnumAtZero,
    // State Entry Function
    HUBDSM_StoppingTimerOnPortDetachInReEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerOnPortDetachInReEnumAtZero },
        { DsmEventOperationSuccess ,    DsmStateReturningPortDetachDeviceInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingDeviceDescriptorInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwValidatingDeviceDescriptorInReEnumAtZero,
    // State Entry Function
    HUBDSM_ValidatingDeviceDescriptorInReEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateResetting2InRenum },
        { DsmEventOperationFailure ,    DsmStateReturningDeviceOperationFailureInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPortResetOnHubStopInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForPortResetOnHubStopInReEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForPortResetOnHubStopInReEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInReEnumAtZero },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateReturningHubStopInReEnumAtZero },
        { DsmEventPortResetComplete ,   DsmStateReturningHubStopInReEnumAtZero },
        { DsmEventPortResetTimedOut ,   DsmStateReturningHubStopInReEnumAtZero },
        { DsmEventPortFailed ,          DsmStateReturningHubStopInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPortResetOnHubSuspendInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForPortResetOnHubSuspendInReEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForPortResetOnHubSuspendInReEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResetComplete ,   DsmStateReturningHubSuspendInReEnumAtZero },
        { DsmEventPortResetTimedOut ,   DsmStateReturningHubSuspendInReEnumAtZero },
        { DsmEventPortFailed ,          DsmStateReturningHubSuspendInReEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInReEnumAtZero },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateReturningHubSuspendInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPostReset1TimerInRenum = {
    // State ETW Name
    DsmStateEtwWaitingForPostReset1TimerInRenum,
    // State Entry Function
    HUBDSM_WaitingForPostReset1TimerInRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTimerFired ,          DsmStateGettingDeviceDescriptorInReEnumAtZero },
        { DsmEventHubStop ,             DsmStateStoppingTimerOnHubStopInReEnumAtZero },
        { DsmEventHubSuspend ,          DsmStateStoppingTimerOnHubSuspendInReEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateStoppingTimerOnPortDetachInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPostReset2ExtendedTimerInRenum = {
    // State ETW Name
    DsmStateEtwWaitingForPostReset2ExtendedTimerInRenum,
    // State Entry Function
    HUBDSM_WaitingForPostReset2ExtendedTimerInRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTimerFired ,          DsmStateReturningDeviceOperationSuccessInReEnumAtZero },
        { DsmEventHubStop ,             DsmStateStoppingTimerOnHubStopInReEnumAtZero },
        { DsmEventHubSuspend ,          DsmStateStoppingTimerOnHubSuspendInReEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateStoppingTimerOnPortDetachInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPostReset2TimerInRenum = {
    // State ETW Name
    DsmStateEtwWaitingForPostReset2TimerInRenum,
    // State Entry Function
    HUBDSM_WaitingForPostReset2TimerInRenum,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTimerFired ,          DsmStateReturningDeviceOperationSuccessInReEnumAtZero },
        { DsmEventHubStop ,             DsmStateStoppingTimerOnHubStopInReEnumAtZero },
        { DsmEventHubSuspend ,          DsmStateStoppingTimerOnHubSuspendInReEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateStoppingTimerOnPortDetachInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForStopSuspendOnReset1InReEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForStopSuspendOnReset1InReEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForStopSuspendOnReset1InReEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubSuspend ,          DsmStateReturningHubSuspendInReEnumAtZero },
        { DsmEventHubStop ,             DsmStateReturningHubStopInReEnumAtZero },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerOnHubStopInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForTimerOnHubStopInReEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForTimerOnHubStopInReEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateWaitingForTimerOnPortDetachInReEnumAtZero },
        { DsmEventTimerFired ,          DsmStateReturningHubStopInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerOnHubSuspendInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForTimerOnHubSuspendInReEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForTimerOnHubSuspendInReEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateWaitingForTimerOnPortDetachInReEnumAtZero },
        { DsmEventTimerFired ,          DsmStateReturningHubSuspendInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerOnPortDetachInReEnumAtZero = {
    // State ETW Name
    DsmStateEtwWaitingForTimerOnPortDetachInReEnumAtZero,
    // State Entry Function
    HUBDSM_WaitingForTimerOnPortDetachInReEnumAtZero,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventTimerFired ,          DsmStateReturningPortDetachDeviceInReEnumAtZero },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfFirstReEnumTryFor30Device = {
    // State ETW Name
    DsmStateEtwCheckingIfFirstReEnumTryFor30Device,
    // State Entry Function
    HUBDSM_CheckingIfFirstReEnumTryFor30Device,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateNotifyingDeviceResetToUCXInResettingFor30 },
        { DsmEventNo ,                  DsmStateStartingPostResetTimerFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDeterminingPortResetTypeRequired = {
    // State ETW Name
    DsmStateEtwDeterminingPortResetTypeRequired,
    // State Entry Function
    HUBDSM_DeterminingPortResetTypeRequired,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventDeviceHotReset ,      DsmStateQueueingHotPortResetFor30 },
        { DsmEventDeviceNoReset ,       DsmStateNotifyingDeviceResetToUCXInResettingFor30 },
        { DsmEventDeviceWarmReset ,     DsmStateQueueingWarmPortResetFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryLoggingResetTimeoutInResettingFor30 = {
    // State ETW Name
    DsmStateEtwLoggingResetTimeoutInResettingFor30,
    // State Entry Function
    HUBDSM_LoggingResetTimeoutInResettingFor30,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningDeviceOperationFailureInResettingFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryNotifyingDeviceResetToUCXInResettingFor30 = {
    // State ETW Name
    DsmStateEtwNotifyingDeviceResetToUCXInResettingFor30,
    // State Entry Function
    HUBDSM_NotifyingDeviceResetToUCXInResettingFor30,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateReturningDeviceOperationSuccessInInResettingFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueueingHotPortResetFor30 = {
    // State ETW Name
    DsmStateEtwQueueingHotPortResetFor30,
    // State Entry Function
    HUBDSM_QueueingHotPortResetFor30,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForPortResetCompleteFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueueingWarmPortResetFor30 = {
    // State ETW Name
    DsmStateEtwQueueingWarmPortResetFor30,
    // State Entry Function
    HUBDSM_QueueingWarmPortResetFor30,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForPortResetCompleteFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningDeviceOperationFailureInResettingFor30 = {
    // State ETW Name
    DsmStateEtwReturningDeviceOperationFailureInResettingFor30,
    // State Entry Function
    HUBDSM_ReturningDeviceOperationFailureInResettingFor30,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningDeviceOperationSuccessInInResettingFor30 = {
    // State ETW Name
    DsmStateEtwReturningDeviceOperationSuccessInInResettingFor30,
    // State Entry Function
    HUBDSM_ReturningDeviceOperationSuccessInInResettingFor30,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubStopInResettingFor30 = {
    // State ETW Name
    DsmStateEtwReturningHubStopInResettingFor30,
    // State Entry Function
    HUBDSM_ReturningHubStopInResettingFor30,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningHubSuspendInResettingFor30 = {
    // State ETW Name
    DsmStateEtwReturningHubSuspendInResettingFor30,
    // State Entry Function
    HUBDSM_ReturningHubSuspendInResettingFor30,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDetachDeviceInResettingFor30 = {
    // State ETW Name
    DsmStateEtwReturningPortDetachDeviceInResettingFor30,
    // State Entry Function
    HUBDSM_ReturningPortDetachDeviceInResettingFor30,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStartingPostResetTimerFor30 = {
    // State ETW Name
    DsmStateEtwStartingPostResetTimerFor30,
    // State Entry Function
    HUBDSM_StartingPostResetTimerFor30,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTimerFired ,          DsmStateNotifyingDeviceResetToUCXInResettingFor30 },
        { DsmEventHubStop ,             DsmStateStoppingTimerOnHubStopInResettingFor30 },
        { DsmEventHubSuspend ,          DsmStateStoppingTimerOnHubSuspendInResettingFor30 },
        { DsmEventPortDetachDevice ,    DsmStateStoppingTimerOnPortDetachInResettingFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppingTimerOnHubStopInResettingFor30 = {
    // State ETW Name
    DsmStateEtwStoppingTimerOnHubStopInResettingFor30,
    // State Entry Function
    HUBDSM_StoppingTimerOnHubStopInResettingFor30,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerOnHubStopInResettingFor30 },
        { DsmEventOperationSuccess ,    DsmStateReturningHubStopInResettingFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppingTimerOnHubSuspendInResettingFor30 = {
    // State ETW Name
    DsmStateEtwStoppingTimerOnHubSuspendInResettingFor30,
    // State Entry Function
    HUBDSM_StoppingTimerOnHubSuspendInResettingFor30,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerOnHubSuspendInResettingFor30 },
        { DsmEventOperationSuccess ,    DsmStateReturningHubSuspendInResettingFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryStoppingTimerOnPortDetachInResettingFor30 = {
    // State ETW Name
    DsmStateEtwStoppingTimerOnPortDetachInResettingFor30,
    // State Entry Function
    HUBDSM_StoppingTimerOnPortDetachInResettingFor30,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateWaitingForTimerOnPortDetachInResettingFor30 },
        { DsmEventOperationSuccess ,    DsmStateReturningPortDetachDeviceInResettingFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPortResetCompleteFor30 = {
    // State ETW Name
    DsmStateEtwWaitingForPortResetCompleteFor30,
    // State Entry Function
    HUBDSM_WaitingForPortResetCompleteFor30,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubStop ,             DsmStateWaitingForPortResetOnHubStopInResettingFor30 },
        { DsmEventPortResetComplete ,   DsmStateCheckingIfFirstReEnumTryFor30Device },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateWaitingForStopSuspendOnReset1InResettingFor30 },
        { DsmEventHubSuspend ,          DsmStateWaitingForPortResetOnHubSuspendInResettingFor30 },
        { DsmEventPortResetTimedOut ,   DsmStateLoggingResetTimeoutInResettingFor30 },
        { DsmEventPortFailed ,          DsmStateReturningDeviceOperationFailureInResettingFor30 },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInResettingFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPortResetOnHubStopInResettingFor30 = {
    // State ETW Name
    DsmStateEtwWaitingForPortResetOnHubStopInResettingFor30,
    // State Entry Function
    HUBDSM_WaitingForPortResetOnHubStopInResettingFor30,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInResettingFor30 },
        { DsmEventPortResetComplete ,   DsmStateReturningHubStopInResettingFor30 },
        { DsmEventPortResetTimedOut ,   DsmStateReturningHubStopInResettingFor30 },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateReturningHubStopInResettingFor30 },
        { DsmEventPortFailed ,          DsmStateReturningHubStopInResettingFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForPortResetOnHubSuspendInResettingFor30 = {
    // State ETW Name
    DsmStateEtwWaitingForPortResetOnHubSuspendInResettingFor30,
    // State Entry Function
    HUBDSM_WaitingForPortResetOnHubSuspendInResettingFor30,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortResetComplete ,   DsmStateReturningHubSuspendInResettingFor30 },
        { DsmEventPortResetTimedOut ,   DsmStateReturningHubSuspendInResettingFor30 },
        { DsmEventPortResetFailedDueToPendingSuspend ,DsmStateReturningHubSuspendInResettingFor30 },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInResettingFor30 },
        { DsmEventPortFailed ,          DsmStateReturningHubSuspendInResettingFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForStopSuspendOnReset1InResettingFor30 = {
    // State ETW Name
    DsmStateEtwWaitingForStopSuspendOnReset1InResettingFor30,
    // State Entry Function
    HUBDSM_WaitingForStopSuspendOnReset1InResettingFor30,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubSuspend ,          DsmStateReturningHubSuspendInResettingFor30 },
        { DsmEventHubStop ,             DsmStateReturningHubStopInResettingFor30 },
        { DsmEventPortDetachDevice ,    DsmStateReturningPortDetachDeviceInResettingFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerOnHubStopInResettingFor30 = {
    // State ETW Name
    DsmStateEtwWaitingForTimerOnHubStopInResettingFor30,
    // State Entry Function
    HUBDSM_WaitingForTimerOnHubStopInResettingFor30,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateWaitingForTimerOnPortDetachInResettingFor30 },
        { DsmEventTimerFired ,          DsmStateReturningHubStopInResettingFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerOnHubSuspendInResettingFor30 = {
    // State ETW Name
    DsmStateEtwWaitingForTimerOnHubSuspendInResettingFor30,
    // State Entry Function
    HUBDSM_WaitingForTimerOnHubSuspendInResettingFor30,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortDetachDevice ,    DsmStateWaitingForTimerOnPortDetachInResettingFor30 },
        { DsmEventTimerFired ,          DsmStateReturningHubSuspendInResettingFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForTimerOnPortDetachInResettingFor30 = {
    // State ETW Name
    DsmStateEtwWaitingForTimerOnPortDetachInResettingFor30,
    // State Entry Function
    HUBDSM_WaitingForTimerOnPortDetachInResettingFor30,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventHubStop ,             DsmStateIgnored },
        { DsmEventHubSuspend ,          DsmStateIgnored },
        { DsmEventTimerFired ,          DsmStateReturningPortDetachDeviceInResettingFor30 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryAllocatingBufferForMsOsExtendedPropertyDescriptor = {
    // State ETW Name
    DsmStateEtwAllocatingBufferForMsOsExtendedPropertyDescriptor,
    // State Entry Function
    HUBDSM_AllocatingBufferForMsOsExtendedPropertyDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateGettingMSOSExtendedPropertyDescriptor },
        { DsmEventOperationFailure ,    DsmStateSignalingPnpEventForMSOSExtInstall },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfMsOs20RegistryValuesShouldBeInstalled = {
    // State ETW Name
    DsmStateEtwCheckingIfMsOs20RegistryValuesShouldBeInstalled,
    // State Entry Function
    HUBDSM_CheckingIfMsOs20RegistryValuesShouldBeInstalled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateSignalingPnpEventForMSOSExtInstall },
        { DsmEventYes ,                 DsmStateInstallingMsOs20RegistryValues },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfMSOSExtendedPropertyDescriptorShouldBeQueried = {
    // State ETW Name
    DsmStateEtwCheckingIfMSOSExtendedPropertyDescriptorShouldBeQueried,
    // State Entry Function
    HUBDSM_CheckingIfMSOSExtendedPropertyDescriptorShouldBeQueried,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateSettingExtPropDescSemaphoreRegistryFlag },
        { DsmEventNo ,                  DsmStateCheckingIfMsOs20RegistryValuesShouldBeInstalled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryFreeingBufferAllocatedForMSOSExtendedProperty = {
    // State ETW Name
    DsmStateEtwFreeingBufferAllocatedForMSOSExtendedProperty,
    // State Entry Function
    HUBDSM_FreeingBufferAllocatedForMSOSExtendedProperty,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateSignalingPnpEventForMSOSExtInstall },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingMSOSExtendedPropertyDescriptor = {
    // State ETW Name
    DsmStateEtwGettingMSOSExtendedPropertyDescriptor,
    // State Entry Function
    HUBDSM_GettingMSOSExtendedPropertyDescriptor,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingMSOSExtendedPropertyDescriptor },
        { DsmEventTransferFailure ,     DsmStateFreeingBufferAllocatedForMSOSExtendedProperty },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingMSOSExtendedPropertyDescriptorHeader = {
    // State ETW Name
    DsmStateEtwGettingMSOSExtendedPropertyDescriptorHeader,
    // State Entry Function
    HUBDSM_GettingMSOSExtendedPropertyDescriptorHeader,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateValidatingMSOSExtendedPropertyDescriptorHeader },
        { DsmEventTransferFailure ,     DsmStateSignalingPnpEventForMSOSExtInstall },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryInstallingMsOs20RegistryValues = {
    // State ETW Name
    DsmStateEtwInstallingMsOs20RegistryValues,
    // State Entry Function
    HUBDSM_InstallingMsOs20RegistryValues,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateSignalingPnpEventForMSOSExtInstall },
        { DsmEventOperationSuccess ,    DsmStateSettingExtPropDescSemaphoreForMsOs2 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingExtPropDescSemaphoreForMsOs2 = {
    // State ETW Name
    DsmStateEtwSettingExtPropDescSemaphoreForMsOs2,
    // State Entry Function
    HUBDSM_SettingExtPropDescSemaphoreForMsOs2,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateSignalingPnpEventForMSOSExtInstall },
        { DsmEventOperationSuccess ,    DsmStateSignalingPnpEventForMSOSExtInstall },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingExtPropDescSemaphoreRegistryFlag = {
    // State ETW Name
    DsmStateEtwSettingExtPropDescSemaphoreRegistryFlag,
    // State Entry Function
    HUBDSM_SettingExtPropDescSemaphoreRegistryFlag,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateGettingMSOSExtendedPropertyDescriptorHeader },
        { DsmEventOperationFailure ,    DsmStateSignalingPnpEventForMSOSExtInstall },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySignalingPnpEventForMSOSExtInstall = {
    // State ETW Name
    DsmStateEtwSignalingPnpEventForMSOSExtInstall,
    // State Entry Function
    HUBDSM_SignalingPnpEventForMSOSExtInstall,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWaitingForInstallMSOSExt },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingMSOSExtendedPropertyDescriptor = {
    // State ETW Name
    DsmStateEtwValidatingMSOSExtendedPropertyDescriptor,
    // State Entry Function
    HUBDSM_ValidatingMSOSExtendedPropertyDescriptor,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateWritingCustomPropertiesInRegistry },
        { DsmEventOperationFailure ,    DsmStateFreeingBufferAllocatedForMSOSExtendedProperty },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingMSOSExtendedPropertyDescriptorHeader = {
    // State ETW Name
    DsmStateEtwValidatingMSOSExtendedPropertyDescriptorHeader,
    // State Entry Function
    HUBDSM_ValidatingMSOSExtendedPropertyDescriptorHeader,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateAllocatingBufferForMsOsExtendedPropertyDescriptor },
        { DsmEventOperationFailure ,    DsmStateSignalingPnpEventForMSOSExtInstall },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingForInstallMSOSExt = {
    // State ETW Name
    DsmStateEtwWaitingForInstallMSOSExt,
    // State Entry Function
    HUBDSM_WaitingForInstallMSOSExt,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPDOInstallMSOSExt ,   DsmStateCheckingIfMSOSExtendedPropertyDescriptorShouldBeQueried },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWritingCustomPropertiesInRegistry = {
    // State ETW Name
    DsmStateEtwWritingCustomPropertiesInRegistry,
    // State Entry Function
    HUBDSM_WritingCustomPropertiesInRegistry,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateFreeingBufferAllocatedForMSOSExtendedProperty },
        { DsmEventOperationSuccess ,    DsmStateFreeingBufferAllocatedForMSOSExtendedProperty },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfLTMShouldBeEnabled = {
    // State ETW Name
    DsmStateEtwCheckingIfLTMShouldBeEnabled,
    // State Entry Function
    HUBDSM_CheckingIfLTMShouldBeEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateReturningOperationSuccessInSettingLPMValues },
        { DsmEventYes ,                 DsmStateEnablingLTM },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfSetIsochDelayShouldBeSkipped = {
    // State ETW Name
    DsmStateEtwCheckingIfSetIsochDelayShouldBeSkipped,
    // State Entry Function
    HUBDSM_CheckingIfSetIsochDelayShouldBeSkipped,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateCheckingIfSetSelShouldBeSkipped },
        { DsmEventNo ,                  DsmStateSetttingIsochDelay },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfSetSelShouldBeSkipped = {
    // State ETW Name
    DsmStateEtwCheckingIfSetSelShouldBeSkipped,
    // State Entry Function
    HUBDSM_CheckingIfSetSelShouldBeSkipped,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateCheckingIfLTMShouldBeEnabled },
        { DsmEventNo ,                  DsmStateSettingSEL },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryEnablingLTM = {
    // State ETW Name
    DsmStateEtwEnablingLTM,
    // State Entry Function
    HUBDSM_EnablingLTM,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateReturningOperationFailureInSettingLPMValues },
        { DsmEventTransferSuccess ,     DsmStateReturningOperationSuccessInSettingLPMValues },
        { DsmEventTransferStall ,       DsmStateReturningOperationSuccessInSettingLPMValues },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureInSettingLPMValues = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureInSettingLPMValues,
    // State Entry Function
    HUBDSM_ReturningOperationFailureInSettingLPMValues,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInSettingLPMValues = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInSettingLPMValues,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInSettingLPMValues,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySettingSEL = {
    // State ETW Name
    DsmStateEtwSettingSEL,
    // State Entry Function
    HUBDSM_SettingSEL,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateCheckingIfLTMShouldBeEnabled },
        { DsmEventTransferFailure ,     DsmStateReturningOperationFailureInSettingLPMValues },
        { DsmEventTransferStall ,       DsmStateCheckingIfLTMShouldBeEnabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntrySetttingIsochDelay = {
    // State ETW Name
    DsmStateEtwSetttingIsochDelay,
    // State Entry Function
    HUBDSM_SetttingIsochDelay,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateCheckingIfSetSelShouldBeSkipped },
        { DsmEventTransferFailure ,     DsmStateCheckingIfSetSelShouldBeSkipped },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfEnableU1NeedsToBeUpdated = {
    // State ETW Name
    DsmStateEtwCheckingIfEnableU1NeedsToBeUpdated,
    // State Entry Function
    HUBDSM_CheckingIfEnableU1NeedsToBeUpdated,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfU2TimeoutNeedsToBeChanged },
        { DsmEventDeviceDisableUState , DsmStateDisablingU1 },
        { DsmEventDeviceEnableUState ,  DsmStateEnablingU1 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfEnableU2NeedsToBeUpdated = {
    // State ETW Name
    DsmStateEtwCheckingIfEnableU2NeedsToBeUpdated,
    // State Entry Function
    HUBDSM_CheckingIfEnableU2NeedsToBeUpdated,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfExitLatencyNeedsToBeDecreased },
        { DsmEventDeviceDisableUState , DsmStateDisablingU2 },
        { DsmEventDeviceEnableUState ,  DsmStateEnablingU2 },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfEndpointsNeedToBeConfiguredAfterDecreasingExitLatency = {
    // State ETW Name
    DsmStateEtwCheckingIfEndpointsNeedToBeConfiguredAfterDecreasingExitLatency,
    // State Entry Function
    HUBDSM_CheckingIfEndpointsNeedToBeConfiguredAfterDecreasingExitLatency,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateProgrammingEndpointsInControllerAfterDecreasingExitLatency },
        { DsmEventNo ,                  DsmStateReturningOperationSuccessInUpdatingLPMValues },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfEndpointsNeedToBeProgrammedAfterIncreasingLatency = {
    // State ETW Name
    DsmStateEtwCheckingIfEndpointsNeedToBeProgrammedAfterIncreasingLatency,
    // State Entry Function
    HUBDSM_CheckingIfEndpointsNeedToBeProgrammedAfterIncreasingLatency,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateProgrammingEndpointsInControllerAfterIncreasingExitLatency },
        { DsmEventNo ,                  DsmStateUpdatingDeviceExitLatencyInTheControllerAfterIncreasingExitLatency },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfEndpointsToBeDisabled = {
    // State ETW Name
    DsmStateEtwCheckingIfEndpointsToBeDisabled,
    // State Entry Function
    HUBDSM_CheckingIfEndpointsToBeDisabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateDisablingEndpointsOnConfigFailure },
        { DsmEventNo ,                  DsmStateReturningOperationFailureInUpdatingLPMValues },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfEndpointsToBeDisabledOnDetach = {
    // State ETW Name
    DsmStateEtwCheckingIfEndpointsToBeDisabledOnDetach,
    // State Entry Function
    HUBDSM_CheckingIfEndpointsToBeDisabledOnDetach,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateReturningPortDetachDeviceInUpdatingLPMValues },
        { DsmEventYes ,                 DsmStateDisablingEndpointsOnConfigFailureOnDetach },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfExitLatencyNeedsToBeDecreased = {
    // State ETW Name
    DsmStateEtwCheckingIfExitLatencyNeedsToBeDecreased,
    // State Entry Function
    HUBDSM_CheckingIfExitLatencyNeedsToBeDecreased,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateUpdatingDeviceExitLatencyInTheControllerAfterDecreasingExitLatency },
        { DsmEventNo ,                  DsmStateCheckingIfEndpointsNeedToBeConfiguredAfterDecreasingExitLatency },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfExitLatencyNeedsToBeIncreased = {
    // State ETW Name
    DsmStateEtwCheckingIfExitLatencyNeedsToBeIncreased,
    // State Entry Function
    HUBDSM_CheckingIfExitLatencyNeedsToBeIncreased,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateCheckingIfEndpointsNeedToBeProgrammedAfterIncreasingLatency },
        { DsmEventNo ,                  DsmStateCheckingIfU1TimeoutNeedsToBeChanged },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfU1TimeoutNeedsToBeChanged = {
    // State ETW Name
    DsmStateEtwCheckingIfU1TimeoutNeedsToBeChanged,
    // State Entry Function
    HUBDSM_CheckingIfU1TimeoutNeedsToBeChanged,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateQueuingSetU1TimeoutToPsm },
        { DsmEventNo ,                  DsmStateCheckingIfEnableU1NeedsToBeUpdated },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIfU2TimeoutNeedsToBeChanged = {
    // State ETW Name
    DsmStateEtwCheckingIfU2TimeoutNeedsToBeChanged,
    // State Entry Function
    HUBDSM_CheckingIfU2TimeoutNeedsToBeChanged,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventYes ,                 DsmStateQueuingSetU2TimeoutToPsm },
        { DsmEventNo ,                  DsmStateCheckingIfEnableU2NeedsToBeUpdated },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryComputingU1U2TimeoutsAndExitLatency = {
    // State ETW Name
    DsmStateEtwComputingU1U2TimeoutsAndExitLatency,
    // State Entry Function
    HUBDSM_ComputingU1U2TimeoutsAndExitLatency,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfExitLatencyNeedsToBeIncreased },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsOnConfigFailure = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsOnConfigFailure,
    // State Entry Function
    HUBDSM_DisablingEndpointsOnConfigFailure,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateReturningOperationFailureInUpdatingLPMValues },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingEndpointsOnConfigFailureOnDetach = {
    // State ETW Name
    DsmStateEtwDisablingEndpointsOnConfigFailureOnDetach,
    // State Entry Function
    HUBDSM_DisablingEndpointsOnConfigFailureOnDetach,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateReturningPortDetachDeviceInUpdatingLPMValues },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingU1 = {
    // State ETW Name
    DsmStateEtwDisablingU1,
    // State Entry Function
    HUBDSM_DisablingU1,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateUpdatingDeviceStatusToU1Disabled },
        { DsmEventTransferFailure ,     DsmStateUpdatingDeviceStatusToU1Disabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingU1U2OnExitLatencyTooLargeError = {
    // State ETW Name
    DsmStateEtwDisablingU1U2OnExitLatencyTooLargeError,
    // State Entry Function
    HUBDSM_DisablingU1U2OnExitLatencyTooLargeError,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationFailure ,    DsmStateCheckingIfEndpointsToBeDisabled },
        { DsmEventOperationSuccess ,    DsmStateComputingU1U2TimeoutsAndExitLatency },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryDisablingU2 = {
    // State ETW Name
    DsmStateEtwDisablingU2,
    // State Entry Function
    HUBDSM_DisablingU2,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateUpdatingDeviceStatusToU2Disabled },
        { DsmEventTransferFailure ,     DsmStateUpdatingDeviceStatusToU2Disabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryEnablingU1 = {
    // State ETW Name
    DsmStateEtwEnablingU1,
    // State Entry Function
    HUBDSM_EnablingU1,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateUpdatingDeviceStatusToU1Enabled },
        { DsmEventTransferFailure ,     DsmStateUpdatingDeviceStatusToU1Enabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryEnablingU2 = {
    // State ETW Name
    DsmStateEtwEnablingU2,
    // State Entry Function
    HUBDSM_EnablingU2,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferSuccess ,     DsmStateUpdatingDeviceStatusToU2Enabled },
        { DsmEventTransferFailure ,     DsmStateUpdatingDeviceStatusToU2Enabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryInitializingU1U2Flags = {
    // State ETW Name
    DsmStateEtwInitializingU1U2Flags,
    // State Entry Function
    HUBDSM_InitializingU1U2Flags,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateComputingU1U2TimeoutsAndExitLatency },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryProgrammingEndpointsInControllerAfterDecreasingExitLatency = {
    // State ETW Name
    DsmStateEtwProgrammingEndpointsInControllerAfterDecreasingExitLatency,
    // State Entry Function
    HUBDSM_ProgrammingEndpointsInControllerAfterDecreasingExitLatency,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateReturningOperationSuccessInUpdatingLPMValues },
        { DsmEventUCXIoctlFailureDueToExitLatencyTooLarge ,DsmStateDisablingU1U2OnExitLatencyTooLargeError },
        { DsmEventUCXIoctlFailure ,     DsmStateCheckingIfEndpointsToBeDisabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryProgrammingEndpointsInControllerAfterIncreasingExitLatency = {
    // State ETW Name
    DsmStateEtwProgrammingEndpointsInControllerAfterIncreasingExitLatency,
    // State Entry Function
    HUBDSM_ProgrammingEndpointsInControllerAfterIncreasingExitLatency,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateUpdatingDeviceExitLatencyInTheControllerAfterIncreasingExitLatency },
        { DsmEventUCXIoctlFailureDueToExitLatencyTooLarge ,DsmStateDisablingU1U2OnExitLatencyTooLargeError },
        { DsmEventUCXIoctlFailure ,     DsmStateCheckingIfEndpointsToBeDisabled },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueuingSetU1TimeoutToPsm = {
    // State ETW Name
    DsmStateEtwQueuingSetU1TimeoutToPsm,
    // State Entry Function
    HUBDSM_QueuingSetU1TimeoutToPsm,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortTimeoutUpdated ,  DsmStateCheckingIfEnableU1NeedsToBeUpdated },
        { DsmEventPortDetachDevice ,    DsmStateCheckingIfEndpointsToBeDisabledOnDetach },
        { DsmEventPortFailed ,          DsmStateCheckingIfEnableU1NeedsToBeUpdated },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueuingSetU2TimeoutToPsm = {
    // State ETW Name
    DsmStateEtwQueuingSetU2TimeoutToPsm,
    // State Entry Function
    HUBDSM_QueuingSetU2TimeoutToPsm,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortTimeoutUpdated ,  DsmStateCheckingIfEnableU2NeedsToBeUpdated },
        { DsmEventPortDetachDevice ,    DsmStateCheckingIfEndpointsToBeDisabledOnDetach },
        { DsmEventPortFailed ,          DsmStateCheckingIfEnableU2NeedsToBeUpdated },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureInUpdatingLPMValues = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureInUpdatingLPMValues,
    // State Entry Function
    HUBDSM_ReturningOperationFailureInUpdatingLPMValues,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInUpdatingLPMValues = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInUpdatingLPMValues,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInUpdatingLPMValues,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningPortDetachDeviceInUpdatingLPMValues = {
    // State ETW Name
    DsmStateEtwReturningPortDetachDeviceInUpdatingLPMValues,
    // State Entry Function
    HUBDSM_ReturningPortDetachDeviceInUpdatingLPMValues,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUpdatingDeviceExitLatencyInTheControllerAfterDecreasingExitLatency = {
    // State ETW Name
    DsmStateEtwUpdatingDeviceExitLatencyInTheControllerAfterDecreasingExitLatency,
    // State Entry Function
    HUBDSM_UpdatingDeviceExitLatencyInTheControllerAfterDecreasingExitLatency,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCheckingIfEndpointsNeedToBeConfiguredAfterDecreasingExitLatency },
        { DsmEventUCXIoctlFailureDueToExitLatencyTooLarge ,DsmStateDisablingU1U2OnExitLatencyTooLargeError },
        { DsmEventUCXIoctlFailure ,     DsmStateCheckingIfEndpointsNeedToBeConfiguredAfterDecreasingExitLatency },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUpdatingDeviceExitLatencyInTheControllerAfterIncreasingExitLatency = {
    // State ETW Name
    DsmStateEtwUpdatingDeviceExitLatencyInTheControllerAfterIncreasingExitLatency,
    // State Entry Function
    HUBDSM_UpdatingDeviceExitLatencyInTheControllerAfterIncreasingExitLatency,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateCheckingIfU1TimeoutNeedsToBeChanged },
        { DsmEventUCXIoctlFailureDueToExitLatencyTooLarge ,DsmStateDisablingU1U2OnExitLatencyTooLargeError },
        { DsmEventUCXIoctlFailure ,     DsmStateCheckingIfU1TimeoutNeedsToBeChanged },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUpdatingDeviceStatusToU1Disabled = {
    // State ETW Name
    DsmStateEtwUpdatingDeviceStatusToU1Disabled,
    // State Entry Function
    HUBDSM_UpdatingDeviceStatusToU1Disabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfU2TimeoutNeedsToBeChanged },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUpdatingDeviceStatusToU1Enabled = {
    // State ETW Name
    DsmStateEtwUpdatingDeviceStatusToU1Enabled,
    // State Entry Function
    HUBDSM_UpdatingDeviceStatusToU1Enabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfU2TimeoutNeedsToBeChanged },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUpdatingDeviceStatusToU2Disabled = {
    // State ETW Name
    DsmStateEtwUpdatingDeviceStatusToU2Disabled,
    // State Entry Function
    HUBDSM_UpdatingDeviceStatusToU2Disabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfExitLatencyNeedsToBeDecreased },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryUpdatingDeviceStatusToU2Enabled = {
    // State ETW Name
    DsmStateEtwUpdatingDeviceStatusToU2Enabled,
    // State Entry Function
    HUBDSM_UpdatingDeviceStatusToU2Enabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateCheckingIfExitLatencyNeedsToBeDecreased },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryComputingLPMTimeoutValuesInUnconfigured = {
    // State ETW Name
    DsmStateEtwComputingLPMTimeoutValuesInUnconfigured,
    // State Entry Function
    HUBDSM_ComputingLPMTimeoutValuesInUnconfigured,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateReturningOperationSuccessInUpdatingLPMValuesInUnconfigured },
        { DsmEventYes ,                 DsmStateQueuingSetU2TimeoutToPsmForEnumeratedDevice },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryQueuingSetU2TimeoutToPsmForEnumeratedDevice = {
    // State ETW Name
    DsmStateEtwQueuingSetU2TimeoutToPsmForEnumeratedDevice,
    // State Entry Function
    HUBDSM_QueuingSetU2TimeoutToPsmForEnumeratedDevice,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventPortTimeoutUpdated ,  DsmStateReturningOperationSuccessInUpdatingLPMValuesInUnconfigured },
        { DsmEventPortFailed ,          DsmStateReturningOperationFailureInUpdatingLPMValuesInUnconfigured },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureInUpdatingLPMValuesInUnconfigured = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureInUpdatingLPMValuesInUnconfigured,
    // State Entry Function
    HUBDSM_ReturningOperationFailureInUpdatingLPMValuesInUnconfigured,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInUpdatingLPMValuesInUnconfigured = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInUpdatingLPMValuesInUnconfigured,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInUpdatingLPMValuesInUnconfigured,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryGettingRemoteWakeCapability = {
    // State ETW Name
    DsmStateEtwGettingRemoteWakeCapability,
    // State Entry Function
    HUBDSM_GettingRemoteWakeCapability,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventTransferFailure ,     DsmStateReturningOperationFailureInGettingRemoteWakeCap },
        { DsmEventTransferSuccess ,     DsmStateValidatingInterfaceStatusForRemoteWake },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationFailureInGettingRemoteWakeCap = {
    // State ETW Name
    DsmStateEtwReturningOperationFailureInGettingRemoteWakeCap,
    // State Entry Function
    HUBDSM_ReturningOperationFailureInGettingRemoteWakeCap,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInGettingRemoteWakeCap = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInGettingRemoteWakeCap,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInGettingRemoteWakeCap,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryValidatingInterfaceStatusForRemoteWake = {
    // State ETW Name
    DsmStateEtwValidatingInterfaceStatusForRemoteWake,
    // State Entry Function
    HUBDSM_ValidatingInterfaceStatusForRemoteWake,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventOperationSuccess ,    DsmStateReturningOperationSuccessInGettingRemoteWakeCap },
        { DsmEventOperationFailure ,    DsmStateReturningOperationFailureInGettingRemoteWakeCap },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryCheckingIf20LPMShouldBeEnabled = {
    // State ETW Name
    DsmStateEtwCheckingIf20LPMShouldBeEnabled,
    // State Entry Function
    HUBDSM_CheckingIf20LPMShouldBeEnabled,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNo ,                  DsmStateReturningOperationSuccessInSetting20LPMValues },
        { DsmEventYes ,                 DsmStateWaitingFor20LpmUpdateIoctlToComplete },
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryReturningOperationSuccessInSetting20LPMValues = {
    // State ETW Name
    DsmStateEtwReturningOperationSuccessInSetting20LPMValues,
    // State Entry Function
    HUBDSM_ReturningOperationSuccessInSetting20LPMValues,
    // State Flags
    StateFlagEndSubStateMachine,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventNull ,                DsmStateNull },
    }
};



STATE_ENTRY   DsmStateEntryWaitingFor20LpmUpdateIoctlToComplete = {
    // State ETW Name
    DsmStateEtwWaitingFor20LpmUpdateIoctlToComplete,
    // State Entry Function
    HUBDSM_WaitingFor20LpmUpdateIoctlToComplete,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { DsmEventUCXIoctlSuccess ,     DsmStateReturningOperationSuccessInSetting20LPMValues },
        { DsmEventUCXIoctlFailure ,     DsmStateReturningOperationSuccessInSetting20LPMValues },
        { DsmEventNull ,                DsmStateNull },
    }
};






//
// Global List of States in all the State Machines
//
PSTATE_ENTRY    DSMStateTable[] = {
    &DsmStateEntryWaitingForPortAttach,
    &DsmStateEntryAckingD0ForFaultyClientDrivers,
    &DsmStateEntryAckingHubStopAfterSuspendFromStoppedState,
    &DsmStateEntryAckingPdoPreStart,
    &DsmStateEntryAckingPdoPreStartFromStoppedEnabled,
    &DsmStateEntryAckingQueryResourcesInConfiguredD0,
    &DsmStateEntryAckingStopAfterSuspendOnDetach,
    &DsmStateEntryAcquiringPowerReferenceOnHubOnAttach,
    &DsmStateEntryAcquiringPowerReferenceOnHubStart,
    &DsmStateEntryBugcheckingSystemOnBootDeviceEnumerationFailure,
    &DsmStateEntryCheckIfDeviceCanBeDisabledOnDriverNotFound,
    &DsmStateEntryCheckingForZeroBwPipeOnClientClearStall,
    &DsmStateEntryCheckingForZeroBwPipeOnClientResetPipe,
    &DsmStateEntryCheckingForZeroBwPipeOnClientSyncResetPipe,
    &DsmStateEntryCheckingIfBootDeviceOnD0Exit,
    &DsmStateEntryCheckingIfBootDeviceOnDetach,
    &DsmStateEntryCheckingIfBootDeviceOnReset,
    &DsmStateEntryCheckingIfDeviceHasReceivedFirstStart,
    &DsmStateEntryCheckingIfDeviceHasReceivedFirstStartInUncoonfigured,
    &DsmStateEntryCheckingIfDeviceShouldBeDisabled,
    &DsmStateEntryCheckingIfItIsIsochronousEndpoint,
    &DsmStateEntryCleaningUpAnyExistingConfigurationOnStart,
    &DsmStateEntryClearingEndpointHaltOnClientClearStall,
    &DsmStateEntryClearingEndpointHaltOnClientResetPipe,
    &DsmStateEntryClearingFailIoFlagOnSuccessfulRecovery,
    &DsmStateEntryCompletingClientRequestInConfigured,
    &DsmStateEntryCompletingClientRequestInConfiguredWithLastStatus,
    &DsmStateEntryCompletingClientRequestInConfiguredWithSpecialFile,
    &DsmStateEntryCompletingClientRequestInUnconfigured,
    &DsmStateEntryCompletingClientRequestInUnConfiguredWithLastStatus,
    &DsmStateEntryCompletingClientRequestOnDetachInConfigured,
    &DsmStateEntryCompletingClientRequestOnDetachInDisabled,
    &DsmStateEntryCompletingClientRequestOnDetachWithDeviceProgrammed,
    &DsmStateEntryCompletingClientRequestOnFailureInConfigured,
    &DsmStateEntryCompletingClientRequestOnFailureInDisabled,
    &DsmStateEntryCompletingClientRequestOnFailureInUnconfigured,
    &DsmStateEntryCompletingClientUnconfigureRequest,
    &DsmStateEntryCompletingClientUnconfigureRequestWithFailure,
    &DsmStateEntryCompletingD0EntryInConfigured,
    &DsmStateEntryCompletingD0EntryInConfiguredWithSpecialFile,
    &DsmStateEntryCompletingD0EntryInConfiguredWithSpecialFileOnDetach,
    &DsmStateEntryCompletingD0EntryInConfiguredWithSpecialFileOnDetachWithDeviceEnabled,
    &DsmStateEntryCompletingD0EntryInUnconfigured,
    &DsmStateEntryCompletingD0EntryOnDetachWithSpecialFile,
    &DsmStateEntryCompletingD0ExitAfterHubSuspend,
    &DsmStateEntryCompletingD0ExitOnSuspendFailure,
    &DsmStateEntryCompletingDeviceCleanup,
    &DsmStateEntryCompletingHubStopAfterSuspend,
    &DsmStateEntryCompletingHubStopWithPortOff,
    &DsmStateEntryCompletingPDOCleanupForUnknownDevice,
    &DsmStateEntryCompletingPdoExitFinalOnDetach,
    &DsmStateEntryCompletingPdoExitFinalOnDetachOnDeviceReportedMissing,
    &DsmStateEntryCompletingPdoReStartOnDetach,
    &DsmStateEntryCompletingPdoReStartOnDetachWithDeviceEnabled,
    &DsmStateEntryCompletingPdoReStartOnOperationFailure,
    &DsmStateEntryCompletingPnpEventOnDetachInConfigured,
    &DsmStateEntryCompletingPnpEventOnDetachInDisabled,
    &DsmStateEntryCompletingPnpEventOnDetachInUnConfigured,
    &DsmStateEntryCompletingPnpEventOnDetachWhileRenumeratingOnResume,
    &DsmStateEntryCompletingPnpEventOnFailureInDisabledWithPortOff,
    &DsmStateEntryCompletingPnpEventOnFailureWithEndpointsToBeDeleted,
    &DsmStateEntryConfiguredInD0,
    &DsmStateEntryConfiguredResumingOnHubResume,
    &DsmStateEntryConfiguredSuspended,
    &DsmStateEntryConfiguredSuspendedOnHubSuspend,
    &DsmStateEntryConfiguredSuspendedWithSpecialFile,
    &DsmStateEntryConfiguredSuspending,
    &DsmStateEntryConfiguredSuspendingOnHubSuspend,
    &DsmStateEntryConfiguredSuspendingWithSpecialFile,
    &DsmStateEntryConfiguringDeviceFromCachedInformation,
    &DsmStateEntryConfiguringDeviceFromCachedInformationOnHubResume,
    &DsmStateEntryConfiguringDeviceFromCachedInformationOnResume,
    &DsmStateEntryConfiguringDeviceFromCachedInformationOnResumeWithSpecialFile,
    &DsmStateEntryConfiguringDeviceFromCachedInformationWithSpecialFile,
    &DsmStateEntryConfiguringOnSelectConfiguration,
    &DsmStateEntryCyclingPortOnRenumerationOfUnknownDevice,
    &DsmStateEntryD0EntryForUnknownDevice,
    &DsmStateEntryDeConfiguringDeviceOnClientRequest,
    &DsmStateEntryDeConfiguringDeviceOnClientRequestFromUnconfigured,
    &DsmStateEntryDeletingDefaultEndpointAndDeviceOnDetachDuringEnum,
    &DsmStateEntryDeletingDeviceAndDefaultEndpointOnCleanup,
    &DsmStateEntryDeletingDeviceAndDefaultEndpointOnCleanupAfterDetach,
    &DsmStateEntryDeletingDeviceAndDefaultEndpointOnCleanupWithPortOff,
    &DsmStateEntryDeletingEndpoinstAndDeviceOnDetachOnCleanup,
    &DsmStateEntryDeletingEndpointsAndDeviceOnOnCleanup,
    &DsmStateEntryDeporgrammingAllEndpointsOnCleanupFromEnumeratedConfigured,
    &DsmStateEntryDeporgrammingAllEndpointsOnHubStopFromEnumeratedConfigured,
    &DsmStateEntryDeprogrammingAllEndpointsOnHubSuspendFromEnumeratedConfigured,
    &DsmStateEntryDetachedAndDeprogrammedWithResetPending,
    &DsmStateEntryDetachedWithResetPending,
    &DsmStateEntryDeviceDetachedAndDeporgrammedWithSpecialFile,
    &DsmStateEntryDeviceDetachedWithSpecialFile,
    &DsmStateEntryDisablingBackPortInEnumeratedUnknown,
    &DsmStateEntryDisablingBackPortInWaitingForD0EntryForFailedDevice,
    &DsmStateEntryDisablingDeviceAndDefaultEndpointInControllerOnDetachDuringEnum,
    &DsmStateEntryDisablingDeviceAndDefaultEndpointOnDetachOnHubStart,
    &DsmStateEntryDisablingDeviceAndDefaultEndpointOnEnumFailureOnHubStart,
    &DsmStateEntryDisablingDeviceAndDefaultEpInControllerOFailureWithPortOff,
    &DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnCleanupWithPortOff,
    &DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnD0ExitOnDetachWithPDOMissing,
    &DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnDetach,
    &DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnDetachOnHubStopWithPDOMissing,
    &DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnDetachOnPDOCleaupWithPDOMissing,
    &DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspend,
    &DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnHubStopAfterSuspendOnDetach,
    &DsmStateEntryDisablingDeviceAndDefaultEpInControllerOnHubStopOnDetach,
    &DsmStateEntryDisablingDeviceInControllerOnCleanup,
    &DsmStateEntryDisablingDeviceInControllerOnDetachOnCleanup,
    &DsmStateEntryDisablingDeviceInControllerOnEnumFailureOnPdoPreStart,
    &DsmStateEntryDisablingDeviceInControllerOnEnumFailureOnRestart,
    &DsmStateEntryDisablingDeviceInControllerOnHubStopFromStoppedEnabled,
    &DsmStateEntryDisablingDeviceInControllerOnHubStopInStopped,
    &DsmStateEntryDisablingDeviceInControllerOnHubStopInSuspended,
    &DsmStateEntryDisablingDeviceInControllerOnPDOStop,
    &DsmStateEntryDisablingDeviceInControllerOnRenumerationWhileSuspended,
    &DsmStateEntryDisablingDeviceInControllerOnStartTimeout,
    &DsmStateEntryDisablingEndpointsInControllerOHubStopAfterSuspendOnDetach,
    &DsmStateEntryDisablingEndpointsInControllerOHubStopOnDetach,
    &DsmStateEntryDisablingEndpointsInControllerOnCleanupWithPortOff,
    &DsmStateEntryDisablingEndpointsInControllerOnClientRequest,
    &DsmStateEntryDisablingEndpointsInControllerOnD0ExitOnDetach,
    &DsmStateEntryDisablingEndpointsInControllerOnD0ExitOnDetachWithPDOMissing,
    &DsmStateEntryDisablingEndpointsInControllerOnDetachOnHubStopWithPDOMissing,
    &DsmStateEntryDisablingEndpointsInControllerOnDetachOnPDOCleanupWithPDOMissing,
    &DsmStateEntryDisablingEndpointsInControllerOnFailureWithPortOff,
    &DsmStateEntryDisablingEndpointsInControllerOnHubStopAfterSuspend,
    &DsmStateEntryDisablingEndpointsInControllerOnRenumerationWhileSuspended,
    &DsmStateEntryDisablingEndpointsInControllerOnStop,
    &DsmStateEntryDisablingEndpointsOnDetachWithSpecialFile,
    &DsmStateEntryDisablingEndpointsOnDetachWithSpecialFileAndResetPending,
    &DsmStateEntryDisablingEndpointsOnFailureWithSpecialFile,
    &DsmStateEntryDisablingPortOnFailureInUnConfigured,
    &DsmStateEntryDisablingPortOnHubStop,
    &DsmStateEntryDisablingPortOnHubStopFromStoppedEnabled,
    &DsmStateEntryDisablingPortOnPdoCleanup,
    &DsmStateEntryDisablingPortOnPDOStop,
    &DsmStateEntryDisablingPortOnStartTimeout,
    &DsmStateEntryEnumeratedAsFailedUnknown,
    &DsmStateEntryEnumerating,
    &DsmStateEntryFlushingPD0D0ExitFinalInConfigured,
    &DsmStateEntryFlushingPD0D0ExitFinalInUnConfigured,
    &DsmStateEntryFowardingStreamsRequestToUcxInConfigured,
    &DsmStateEntryFowardingStreamsRequestToUcxInConfiguredSuspended,
    &DsmStateEntryFowardingStreamsRequestToUcxInConfiguredSuspendedWithSpecialFile,
    &DsmStateEntryFowardingStreamsRequestToUcxInDeviceDetachedAndDeporgrammedWithSpecialFile,
    &DsmStateEntryFowardingStreamsRequestToUcxInDeviceDetachedWithSpecialFile,
    &DsmStateEntryFowardingStreamsRequestToUcxInStopEnabled,
    &DsmStateEntryFowardingStreamsRequestToUcxInStopped,
    &DsmStateEntryFowardingStreamsRequestToUcxInStoppedWithHubSuspended,
    &DsmStateEntryFowardingStreamsRequestToUcxInUnconfiguredInD0,
    &DsmStateEntryFowardingStreamsRequestToUcxInUnconfiguredSuspended,
    &DsmStateEntryFowardingStreamsRequestToUcxInWaitingForDetachorResetWithDeviceDeprogrammed,
    &DsmStateEntryFowardingStreamsRequestToUcxInWaitingForDetatchorReset,
    &DsmStateEntryGettingMatchingPipeHandleOnClientClearStall,
    &DsmStateEntryGettingMatchingPipeHandleOnClientResetPipe,
    &DsmStateEntryGettingMatchingPipeHandleOnClientSyncResetPipe,
    &DsmStateEntryIssuingDeviceDetachedToPSMOnDetach,
    &DsmStateEntryIssuingPortCycleAndCompletingClientRequestOnEnumFailue,
    &DsmStateEntryIssuingPortCycleAndSingalingPnpEventOnEnumFailueFromConfigured,
    &DsmStateEntryIssuingPortCycleOnEnumFailureOnHubResume,
    &DsmStateEntryIssuingPortCycleOnFailueWithEndpointsToBeDeleted,
    &DsmStateEntryNotifyingDeviceDisconnectedOnDetach,
    &DsmStateEntryNotifyingDeviceDisconnectedOnDetachWithSpecialFile,
    &DsmStateEntryPreparingEndpointAndInterfaceListsOnClientReset,
    &DsmStateEntryPreparingEndpointAndInterfaceListsOnClientResetWithSpecialFile,
    &DsmStateEntryPreparingEndpointAndInterfaceListsOnResetAfterHubResume,
    &DsmStateEntryPreparingEndpointAndInterfaceListsOnResetAfterResume,
    &DsmStateEntryPreparingEndpointAndInterfaceListsOnResetAfterResumeWithSpecialFile,
    &DsmStateEntryPreparingEndpointAndInterfaceListsOnSelectConfiguration,
    &DsmStateEntryPreparingEndpointAndInterfaceListsOnSelectConfigurationFromConfigured,
    &DsmStateEntryPurgingDeviceIoOnDetachInConfigured,
    &DsmStateEntryPurgingDeviceIoOnHubSuspend,
    &DsmStateEntryPurgingDeviceIoOnHubSuspendFromSuspending,
    &DsmStateEntryPurgingDeviceIoOnPrepareForHibernate,
    &DsmStateEntryQueryingDescriptorForFdoRequestFromConfigured,
    &DsmStateEntryQueryingDescriptorForFdoRequestFromStoppedEnumerated,
    &DsmStateEntryQueryingDescriptorForFdoRequestFromUnConfigured,
    &DsmStateEntryQueryingDeviceTextStringFromConfigured,
    &DsmStateEntryQueryingDeviceTextStringFromStoppedEnumerated,
    &DsmStateEntryQueryingDeviceTextStringFromUnConfigured,
    &DsmStateEntryQueueingAttachFailureToPort,
    &DsmStateEntryQueueingAttachSuccessToPort,
    &DsmStateEntryQueueingDeviceDetachedToPsmForBootDeviceInUnconfigured,
    &DsmStateEntryQueueingDeviceDetachedToPsmForBootDeviceWithResetPending,
    &DsmStateEntryQueueingDeviceDetachedToPsmForBootDeviceWithResetPendingAndDeviceDeprogrammed,
    &DsmStateEntryQueueingPrepareForHiberToPSM,
    &DsmStateEntryReDisablingPortAfterHubResume,
    &DsmStateEntryReEnumeratingOnClientRequestInConfigured,
    &DsmStateEntryReEnumeratingOnClientRequestInConfiguredWithSpecialFile,
    &DsmStateEntryReEnumeratingOnClientRequestInUnconfigured,
    &DsmStateEntryReEnumeratingOnClientRequestOnReattach,
    &DsmStateEntryReEnumeratingOnClientRequestOnReattachWithDeviceDeprogrammed,
    &DsmStateEntryReEnumeratingOnHubResumeInConfigured,
    &DsmStateEntryReEnumeratingOnHubResumeInUnConfigured,
    &DsmStateEntryReEnumeratingOnHubStart,
    &DsmStateEntryReEnumeratingOnPdoPreStartWhileEnabled,
    &DsmStateEntryReEnumeratingOnPdoStart,
    &DsmStateEntryReEnumeratingOnPdoStartWhileEnabled,
    &DsmStateEntryReEnumeratingOnResumeInConfigured,
    &DsmStateEntryReEnumeratingOnResumeInConfiguredWithSpecialFile,
    &DsmStateEntryReEnumeratingOnResumeInUnConfigured,
    &DsmStateEntryRegisteringWithHSM,
    &DsmStateEntryReleasingDevicePowerReferenceOnStopAfterDetach,
    &DsmStateEntryReleasingPowerReferenceOnDetachInConfigured,
    &DsmStateEntryReleasingPowerReferenceOnHubOnDetach,
    &DsmStateEntryReleasingPowerReferenceOnHubOnDetachOnEnumSuccess,
    &DsmStateEntryReleasingPowerReferenceOnHubOnEnumerationFailure,
    &DsmStateEntryReleasingPowerReferenceOnHubStopSuspend,
    &DsmStateEntryReleasingPowerReferenceOnHubSuspendStopOnDetach,
    &DsmStateEntryReleasingPowerReferenceOnPortFailureInHubStopSuspend,
    &DsmStateEntryReleasingPowerReferenceOnSuspendFailureInConfigured,
    &DsmStateEntryReleasingPowerReferenceOnSuspendFailureInUnConfigured,
    &DsmStateEntryRemovingDeviceInfoFromGlobalListOnCleanup,
    &DsmStateEntryRemovingDeviceInfoFromGlobalListOnCleanupOnDetachInConfigured,
    &DsmStateEntryRemovingDeviceInfoFromGlobalListOnCleanupOnDetachInUnConfigured,
    &DsmStateEntryRemovingDeviceInfoFromGlobalListOnCleanUpOnHubStopOnDetach,
    &DsmStateEntryRemovingDeviceInfoFromGlobalListOnDeviceReportedMissing,
    &DsmStateEntryRemovingDeviceInfoFromGlobalListOnReportedMissingOnHubStopOnDetach,
    &DsmStateEntryReportingDeviceMissing,
    &DsmStateEntryReportingDeviceToPnp,
    &DsmStateEntryReportingFailedDeviceAsMissing,
    &DsmStateEntryReportingFailedUnknownDeviceToPnp,
    &DsmStateEntryRequestingCyclePortAndCompletingPnpEventOnFailureInUnconfigured,
    &DsmStateEntryRequestingPortCycleOnErrorFromConfigured,
    &DsmStateEntryResettingPipeInUCXOnClientReset,
    &DsmStateEntryResettingPipeInUCXOnClientSyncResetPipe,
    &DsmStateEntryResumingOnHubResumeInStopped,
    &DsmStateEntrySettingConfigurationInformationInClientRequest,
    &DsmStateEntrySettingDeviceInterface,
    &DsmStateEntrySettingExitLatencyAdjustmentOnNoPingResponseError,
    &DsmStateEntrySettingFailIoFlagOnClientReset,
    &DsmStateEntrySettingForceResetOnRenumFlag,
    &DsmStateEntrySettingInterfaceInformationInClientRequest,
    &DsmStateEntrySettingLPMValuesInConfiguredOnSettingChange,
    &DsmStateEntrySettingLPMValuesInStoppedEnabled,
    &DsmStateEntrySettingLPMValuesInUnConfiguredOnSettingChange,
    &DsmStateEntrySignalingPnpPowerEventOnDetachDuringRenumOnRestart,
    &DsmStateEntrySignalingPnpPowerEventOnStop,
    &DsmStateEntrySignalingPnpPowerEventOnStopEnable,
    &DsmStateEntrySignallingPnpEventOnHubSuspendFromUnconfiguredSuspending,
    &DsmStateEntryStartingDeviceIoOnHubResume,
    &DsmStateEntryStopped,
    &DsmStateEntryStoppedEnabled,
    &DsmStateEntryStoppedEnumerated,
    &DsmStateEntryStoppedEnumeratedConfiguredForFaultyClients,
    &DsmStateEntryStoppedEnumeratedWithHubSuspended,
    &DsmStateEntryStoppedWithHubSuspended,
    &DsmStateEntrySuspendingOnHubSuspendInStopped,
    &DsmStateEntryUnConfiguredInD0,
    &DsmStateEntryUnConfiguredResumingOnHubResume,
    &DsmStateEntryUnconfiguredSuspended,
    &DsmStateEntryUnConfiguredSuspendedOnHubSuspend,
    &DsmStateEntryUnconfiguredSuspending,
    &DsmStateEntryUnConfiguredSuspendingOnHubSuspend,
    &DsmStateEntryUnregisteringWithHSMOnDetachAfterCleanup,
    &DsmStateEntryUnregisteringWithHsmOnDetachOnDeviceReportedMissing,
    &DsmStateEntryUnregsiteringWithHsmOnDetach,
    &DsmStateEntryUnregsiteringWithHSMOnDetachOnPDOCleaupWithPDOMissing,
    &DsmStateEntryValidatingSelectConfigUrbFromConfigured,
    &DsmStateEntryValidatingSelectConfigUrbFromUnConfigured,
    &DsmStateEntryValidatingSelectInterfaceUrbFromConfigured,
    &DsmStateEntryWaitingForD0EntryForFailedDevice,
    &DsmStateEntryWaitingForDetachOrRemove,
    &DsmStateEntryWaitingForDetachOrRemoveInConfigured,
    &DsmStateEntryWaitingForDetachOrRemoveInDisabledWithPortOff,
    &DsmStateEntryWaitingForDetachOrRemoveInUnConfigured,
    &DsmStateEntryWaitingForDetachOrRemoveWithPortOff,
    &DsmStateEntryWaitingForDetachOrReset,
    &DsmStateEntryWaitingForDetachOrResetWithDeviceDeporgrammed,
    &DsmStateEntryWaitingForDetachOrToBeDeleted,
    &DsmStateEntryWaitingForDetachOrToBeDeletedWithPortOff,
    &DsmStateEntryWaitingForDisableCompleteOnHubStopFromStoppedEnabled,
    &DsmStateEntryWaitingForPDOD0ExitOnDetachInConfigured,
    &DsmStateEntryWaitingForPDOD0ExitOnDetachInUnConfigured,
    &DsmStateEntryWaitingForPDOReportedMissing,
    &DsmStateEntryWaitingForPortResumeResponseOnStop,
    &DsmStateEntryWaitingForPortResumeResponseOnSuspend,
    &DsmStateEntryWaitingForRemoveOnDetach,
    &DsmStateEntryWaitingForRemoveOrReprotedMissingOnHubStopOnDetach,
    &DsmStateEntryWaitingForReportedMissingOnDetachInConfigured,
    &DsmStateEntryWaitingForReportedMissingOnDetachInUnConfigured,
    &DsmStateEntryWaitingToBeDeleted,
    &DsmStateEntryQueryingLanguageSpecificProductIdString,
    &DsmStateEntryReturningOperationSuccessInQueryingProductId,
    &DsmStateEntrySignalingDeviceQueryTextEvent,
    &DsmStateEntryValidatingLanguageSpecificProductIdString,
    &DsmStateEntryAcquiringAddressZeroOwnershipInEnum,
    &DsmStateEntryAcquiringPowerReferenceOnStartInEnum,
    &DsmStateEntryCancellingAcquiringAddress0OnDetachInEnum,
    &DsmStateEntryCancellingRetryTimerOnDetachInEnum,
    &DsmStateEntryCancellingRetryTimerOnStopSuspendInEnum,
    &DsmStateEntryCheckingIfDevicePDChargingPolicySupported,
    &DsmStateEntryCheckingIfEnumRetryReachedMaximumInEnum,
    &DsmStateEntryCheckingIfEnumRetryReachedMaximumInEnumWithAddr0Ownership,
    &DsmStateEntryClearingDeviceProgrammingLostFlagOnEnum,
    &DsmStateEntryDeletingDefaultEndpointAndDeviceFromUCXOnDetachInEnumeratingAfterAddr,
    &DsmStateEntryDeletingDefaultEndpointAndDeviceFromUCXOnFailureInEnum,
    &DsmStateEntryDeletingDefaultEndpointAndDeviceOnPostAddressFailureInEnum,
    &DsmStateEntryDeletingDefaultEndpointAndDeviceOnStopSuspendAfterAddressInEnum,
    &DsmStateEntryDeletingDeviceAndDefaultEndpointOnDetachWithAddress0OwnershipInEnum,
    &DsmStateEntryDeletingDeviceAndDefaultEndpointOnStopSuspendWithAddress0OwnershipInEnum,
    &DsmStateEntryDisablingDeviceInControllerOnDetachInEnumeratingAfterAddr,
    &DsmStateEntryDisablingDeviceInControllerOnDetachWithAddress0OwnershipInEnum,
    &DsmStateEntryDisablingDeviceInControllerOnPostAddressEnumFailureInEnum,
    &DsmStateEntryDisablingDeviceInControllerOnSetAddressFailureInEnum,
    &DsmStateEntryDisablingDeviceInControllerOnStopSuspendAfterAddressInEnum,
    &DsmStateEntryDisablingDeviceInControllerOnStopSuspendWithAddress0OwnershipInEnum,
    &DsmStateEntryDisablingOnEnumAfterFailureInEnum,
    &DsmStateEntryDisablingOnEnumAfterFailureInEnumWithAddress0Ownership,
    &DsmStateEntryDisablingOnStopSuspendInEnum,
    &DsmStateEntryDisablingOnStopSuspendInEnumWithAddress0Ownership,
    &DsmStateEntryDisablingOnStopSuspendOnIoctlFailureInEnum,
    &DsmStateEntryEnumeratingAfterAddressInEnum,
    &DsmStateEntryEnumeratingAtAddressZeroInEnum,
    &DsmStateEntryInitializingEnumRetryCountInEnum,
    &DsmStateEntryReleasingAddressZeroOwnershipInEnum,
    &DsmStateEntryReleasingAddressZeroOwnershipOnDetach,
    &DsmStateEntryReleasingAddressZeroOwnershipOnDetachInEnum,
    &DsmStateEntryReleasingAddressZeroOwnershipOnEnumFailure,
    &DsmStateEntryReleasingAddressZeroOwnershipOnStopSuspend,
    &DsmStateEntryReleasingAddressZeroOwnershipOnStopSuspendInEnum,
    &DsmStateEntryReleasingPowerReferenceInEnum,
    &DsmStateEntryReturningOperationFailureInEnum,
    &DsmStateEntryReturningOperationSuccessInEnum,
    &DsmStateEntryReturningPortDetachDeviceInEnum,
    &DsmStateEntrySettingDevicePDChargingPolicy,
    &DsmStateEntrySettingLPMValues,
    &DsmStateEntrySettingNonZeroAddressInEnum,
    &DsmStateEntryStartingTimerForEnumRetryInEnum,
    &DsmStateEntryStartingTimerForEnumRetryInEnumWithAddr0Ownership,
    &DsmStateEntryStoppedSuspendedInEnum,
    &DsmStateEntryUpdatingDeviceInformationInEnum,
    &DsmStateEntryWaitingForDisableCompleteOnStopSuspendAfterFailureInEnum,
    &DsmStateEntryWaitingForDisableCompleteOnStopSuspendInEnum,
    &DsmStateEntryWaitingForDisableCompleteOnStopSuspendInEnumWithAddress0Ownership,
    &DsmStateEntryWaitingForTimerToFlushOnDetachInEnum,
    &DsmStateEntryWaitingForTimerToFlushOnStopSuspendInEnum,
    &DsmStateEntryWaitingForUCXIoctlOnDetachInEnum,
    &DsmStateEntryWaitingForUCXIoctlOnStopSuspendInEnum,
    &DsmStateEntryCheckingIfFirstEnumTryAfterReset1,
    &DsmStateEntryCheckingIfFirstEnumTryAfterReset2,
    &DsmStateEntryCheckingIfSecondResetIsNeeded,
    &DsmStateEntryCreatingUCXDefaultEndpoint,
    &DsmStateEntryCreatingUCXDevice,
    &DsmStateEntryDeletingUCXDefaultEndpointAndDeviceOnDetach,
    &DsmStateEntryDeletingUCXDefaultEndpointAndDeviceOnStop,
    &DsmStateEntryDeletingUCXDefaultEndpointAndDeviceOnSuspend,
    &DsmStateEntryDeletingUCXDefaultEndpointOnOperationFailure,
    &DsmStateEntryDeletingUCXDeviceOnOperationFailure,
    &DsmStateEntryDisablingDeviceInUCXOnDetach,
    &DsmStateEntryDisablingDeviceInUCXOnOperationFailure,
    &DsmStateEntryDisablingDeviceInUCXOnStop,
    &DsmStateEntryDisablingDeviceInUCXOnSuspend,
    &DsmStateEntryDisablingPortOnHubStopInEnumAtAddr0,
    &DsmStateEntryDisablingPortOnHubSuspendInEnumAtAddr0,
    &DsmStateEntryEnablingDeviceInUCX,
    &DsmStateEntryGettingDeviceDescriptorInEnumAtZero,
    &DsmStateEntryLoggingReset1TimeoutInEnumAtZero,
    &DsmStateEntryLoggingReset2TimeoutInEnumAtZero,
    &DsmStateEntryNotifyingDeviceResetToUCXAfterReset2,
    &DsmStateEntryResetting1,
    &DsmStateEntryResetting2,
    &DsmStateEntryReturningDeviceOperationFailureInEnumAtZero,
    &DsmStateEntryReturningDeviceOperationSuccessInEnumAtZero,
    &DsmStateEntryReturningHubStopInEnumAtZero,
    &DsmStateEntryReturningHubSuspendInEnumAtZero,
    &DsmStateEntryReturningPortDetachDeviceInEnumAtZero,
    &DsmStateEntrySettingSpeedFlagFor20Devices,
    &DsmStateEntryStoppingTimerOnHubStopInEnumAtZero,
    &DsmStateEntryStoppingTimerOnHubSuspendInEnumAtZero,
    &DsmStateEntryStoppingTimerOnPortDetachInEnumAtZero,
    &DsmStateEntryUpdatingDefaultEndpointMaxPacketSizeInEnum,
    &DsmStateEntryValidatingDeviceDescriptorInEnumAtZero,
    &DsmStateEntryWaitingForPortReset1OrDisableOnHubStopInEnumAtZero,
    &DsmStateEntryWaitingForPortReset1OrDisableOnHubSuspendInEnumAtZero,
    &DsmStateEntryWaitingForPortReset2OnHubStopInEnumAtZero,
    &DsmStateEntryWaitingForPortReset2OnHubSuspendInEnumAtZero,
    &DsmStateEntryWaitingForPostReset1ExtendedTimer,
    &DsmStateEntryWaitingForPostReset1Timer,
    &DsmStateEntryWaitingForPostReset2ExtendedTimer,
    &DsmStateEntryWaitingForPostReset2Timer,
    &DsmStateEntryWaitingForStopSuspendOnReset1InEnumAtZero,
    &DsmStateEntryWaitingForStopSuspendOnReset2InEnumAtZero,
    &DsmStateEntryWaitingForTimerOnHubStopInEnumAtZero,
    &DsmStateEntryWaitingForTimerOnHubSuspendInEnumAtZero,
    &DsmStateEntryWaitingForTimerOnPortDetachInEnumAtZero,
    &DsmStateEntryCreatingUCXDefaultEndpointFor30,
    &DsmStateEntryCreatingUCXDevice30,
    &DsmStateEntryDeletingUCXDefaultEndpointOnOperationFailureFor30,
    &DsmStateEntryDeletingUCXDefaultEndpointOnOperationFailureFor30OnDetach,
    &DsmStateEntryDeletingUCXDefaultEndpointOnOperationFailureFor30OnHubStop,
    &DsmStateEntryDeletingUCXDefaultEndpointOnOperationFailureFor30OnHubSuspend,
    &DsmStateEntryDeletingUCXDeviceOnOperationFailureFor30,
    &DsmStateEntryDeletingUCXDeviceOnOperationFailureFor30OnDetach,
    &DsmStateEntryDeletingUCXDeviceOnOperationFailureFor30OnHubStop,
    &DsmStateEntryDeletingUCXDeviceOnOperationFailureFor30OnHubSuspend,
    &DsmStateEntryEnablingDeviceInUCXFor30,
    &DsmStateEntryResettingDeviceInEnumeration,
    &DsmStateEntryReturningDetachInPreAddressEnumFor30,
    &DsmStateEntryReturningDeviceOperationFailureInPreAddressEnumFor30,
    &DsmStateEntryReturningDeviceOperationSuccessInPreAddressEnumFor30,
    &DsmStateEntryReturningHubStopInPreAddressEnumFor30,
    &DsmStateEntryReturningHubSuspendInPreAddressEnumFor30,
    &DsmStateEntrySetting30Speed,
    &DsmStateEntryCancellingControlTransferOnDetachInEnumAfterAddr,
    &DsmStateEntryCancellingControlTransferOnHubStopInEnumAfterAddr,
    &DsmStateEntryCancellingControlTransferOnHubSuspendInEnumAfterAddr,
    &DsmStateEntryCheckingBytesReturnedInConfigDescriptor,
    &DsmStateEntryCheckingIfAltEnumCmdCached,
    &DsmStateEntryCheckingIfSuperSpeedNeedsToBeDisabled,
    &DsmStateEntryDiasablingSuperSpeed,
    &DsmStateEntryGettingConfigurationDescriptorWithDefaultLength,
    &DsmStateEntryGettingConfigurationDescriptorWithReturnedLength,
    &DsmStateEntryGettingDeviceDescriptorAfterAddressing,
    &DsmStateEntryGettingRemainingDescriptorsAfterConfigDescriptor,
    &DsmStateEntryQueryingRegistryValuesForDevice,
    &DsmStateEntryReturningHubStopInEnumAfterAddr,
    &DsmStateEntryReturningHubSuspendInEnumAfterAddr,
    &DsmStateEntryReturningOperationFailureInEnumAfterAddr,
    &DsmStateEntryReturningOperationSuccessInEnumAfterAddr,
    &DsmStateEntryReturningPortDeviceDetachInEnumAfterAddr,
    &DsmStateEntrySendingAltEnumCmdInEnumAfterAddr,
    &DsmStateEntrySettingVersionFlagInEnumerating,
    &DsmStateEntryValidatingConfigurationDescriptor,
    &DsmStateEntryValidatingDeviceDescriptorAfterAddressing,
    &DsmStateEntryCancellingControlTransferOnDetachInRemainingDescriptors,
    &DsmStateEntryCancellingControlTransferOnHubStopInRemainingDescriptors,
    &DsmStateEntryCancellingControlTransferOnHubSuspendInRemainingDescriptors,
    &DsmStateEntryCheckingIfIProductIdStringDescriptorShouldBeQueried,
    &DsmStateEntryCheckingIfIProductIsZero,
    &DsmStateEntryGettingBosDescriptorSet,
    &DsmStateEntryGettingDeviceQualifierDescriptorsAndCheckingForSpeedMismatch,
    &DsmStateEntryGettingLanguageIdStringDescriptor,
    &DsmStateEntryGettingMSOSAndSerialNumberDescriptor,
    &DsmStateEntryGettingProductIdStringDescriptor,
    &DsmStateEntryReturningErrorResponseOnLanguageIdQuery,
    &DsmStateEntryReturningErrorResponseOnProductStringQuery,
    &DsmStateEntryReturningHubStopInRemainingDescriptors,
    &DsmStateEntryReturningHubSuspendInRemainingDescriptors,
    &DsmStateEntryReturningOperationFailureInRemainingDescriptors,
    &DsmStateEntryReturningOperationSuccessInRemainingDescriptors,
    &DsmStateEntryReturningPortDeviceDetachInRemainingDescriptors,
    &DsmStateEntryValidatingLanguageIdStringDescriptor,
    &DsmStateEntryValidatingProductIdStringDescriptor,
    &DsmStateEntryCheckingIfDeviceSupportsContainerId,
    &DsmStateEntryCheckingIfIgnoreHWSerNumIsSet,
    &DsmStateEntryCheckingIfMSOSContainerIdDescriptorIsSupported,
    &DsmStateEntryCheckingIfMSOSDescriptorShouldBeQueried,
    &DsmStateEntryCheckingIfMSOSExtendedConfigDescriptorIsSupported,
    &DsmStateEntryCheckingIfSerialNumberStringIndexIsZero,
    &DsmStateEntryGettingMSOSContainerIdDescriptor,
    &DsmStateEntryGettingMSOSContainerIdHeaderDescriptor,
    &DsmStateEntryGettingMSOSDescriptor,
    &DsmStateEntryGettingMSOSExtendedDescriptor,
    &DsmStateEntryGettingMSOSExtendedDescriptorHeader,
    &DsmStateEntryGettingSerialNumberStringDescriptor,
    &DsmStateEntryMarkDeviceAsNotSupportingMSOSDescriptor,
    &DsmStateEntryMarkDeviceAsSupportingMSOSDescriptor,
    &DsmStateEntryMarkingDeviceAsNotSupportingContainerId,
    &DsmStateEntryMarkingDeviceAsNotSupportingContainerIdBasedOnMSOSDescriptor,
    &DsmStateEntryReturningErrorResponseOnContainerIdHeaderQuery,
    &DsmStateEntryReturningErrorResponseOnContainerIdQuery,
    &DsmStateEntryReturningErrorResponseOnMSOSExtendedHeaderQuery,
    &DsmStateEntryReturningErrorResponseOnMSOSExtendedQuery,
    &DsmStateEntryReturningErrorResponseOnMSOSQuery,
    &DsmStateEntryReturningErrorResponseOnSerialNumberQuery,
    &DsmStateEntryReturningOperationFailureInGettingDescriptorsForGreaterThan1x,
    &DsmStateEntryReturningOperationSuccessInGettingDescriptorsForGreaterThan1x,
    &DsmStateEntryValidatingMSOSContainerIdDescriptor,
    &DsmStateEntryValidatingMSOSContainerIdHeaderDescriptor,
    &DsmStateEntryValidatingMSOSDescriptor,
    &DsmStateEntryValidatingMSOSExtendedConfigDescriptor,
    &DsmStateEntryValidatingMSOSExtendedConfigDescriptorHeader,
    &DsmStateEntryValidatingSerialNumberStringDescriptor,
    &DsmStateEntryCheckingIfIgnoreHWSerNumIsSetFor1xDevice,
    &DsmStateEntryCheckingIfSerialNumberStringIndexIsZeroFor1xDevice,
    &DsmStateEntryGettingSerialNumberStringDescriptorFor1xDevice,
    &DsmStateEntryReturningErrorResponseOnSerialNumberQueryFor1xDevice,
    &DsmStateEntryReturningOperationFailureInGettingSerialNumberFor1x,
    &DsmStateEntryReturningOperationSuccessInGettingSerialNumberFor1x,
    &DsmStateEntryValidatingSerialNumberStringDescriptorFor1xDevice,
    &DsmStateEntryCheckingIfWaitRequiredAfterSetAddress,
    &DsmStateEntryReturningHubStopInSettingAddress,
    &DsmStateEntryReturningHubSuspendInSettingAddress,
    &DsmStateEntryReturningOperationFailureInSettingAddress,
    &DsmStateEntryReturningOperationSuccessInSettingAddress,
    &DsmStateEntryReturningPortDetachDeviceInSettingAddress,
    &DsmStateEntrySettingNonZeroAddress,
    &DsmStateEntryStoppingTimerOnHubStopInSettingAddress,
    &DsmStateEntryStoppingTimerOnHubSuspendInSettingAddress,
    &DsmStateEntryStoppingTimerOnPortDetachInSettingAddress,
    &DsmStateEntryWaitingForTimerAfterSetAddress,
    &DsmStateEntryWaitingForTimerOnHubStopInSettingAddress,
    &DsmStateEntryWaitingForTimerOnHubSuspendInSettingAddress,
    &DsmStateEntryWaitingForTimerOnPortDetachInSettingAddress,
    &DsmStateEntryCheckingBytesReturnedInAlternateConfigDescriptor,
    &DsmStateEntryCheckingIfAltEnumCommandNeeded,
    &DsmStateEntryCheckingIfAlternateBOSDescriptorQueryShouldBeSkipped,
    &DsmStateEntryCheckingIfAlternateModeStringDescriptorShouldBeQueried,
    &DsmStateEntryCheckingIfBillboardBOSDescriptorIsPresent,
    &DsmStateEntryCheckingIfBillboardStringDescriptorShouldBeQueried,
    &DsmStateEntryCheckingIfBOSDescriptorQueryShouldBeSkipped,
    &DsmStateEntryCheckingIfCompleteAlternateBOSDescriptorWasRetrieved,
    &DsmStateEntryCheckingIfCompleteBOSDescriptorWasRetrieved,
    &DsmStateEntryCheckingIfDualRoleFeaturesSupported,
    &DsmStateEntryGetMsOs20DescriptorSet,
    &DsmStateEntryGettingAlternateBOSDescriptor,
    &DsmStateEntryGettingAlternateBOSDescriptorHeader,
    &DsmStateEntryGettingAlternateConfigurationDescriptorWithDefaultLength,
    &DsmStateEntryGettingAlternateConfigurationDescriptorWithReturnedLength,
    &DsmStateEntryGettingAlternateDeviceDescriptor,
    &DsmStateEntryGettingAlternateModeStringDescriptor,
    &DsmStateEntryGettingBillboardStringDescriptor,
    &DsmStateEntryGettingBOSDescriptor,
    &DsmStateEntryGettingBOSDescriptorHeader,
    &DsmStateEntryIsMsOs20DescriptorSupported,
    &DsmStateEntryQueryingRegistryValuesForAlternateDeviceEnumeration,
    &DsmStateEntryReturningErrorResponseOnBOSDescriptorHeaderQuery,
    &DsmStateEntryReturningErrorResponseOnBOSDescriptorQuery,
    &DsmStateEntryReturningOperationFailureInGettingBOSDescriptor,
    &DsmStateEntryReturningOperationSuccessInGettingBOSDescriptor,
    &DsmStateEntrySendingMsOs20AlternateEnumerationCommand,
    &DsmStateEntrySendingUsbFeaturesVendorCmd,
    &DsmStateEntrySettingVersionFlagInAlternateEnumeration,
    &DsmStateEntryValidatingAlternateBOSDescriptor,
    &DsmStateEntryValidatingAlternateBOSDescriptorHeader,
    &DsmStateEntryValidatingAlternateConfigurationDescriptor,
    &DsmStateEntryValidatingAlternateDeviceDescriptor,
    &DsmStateEntryValidatingAndLoggingAlternateModeStringDescriptor,
    &DsmStateEntryValidatingAndLoggingBillboardStringDescriptor,
    &DsmStateEntryValidatingBOSDescriptor,
    &DsmStateEntryValidatingBOSDescriptorHeader,
    &DsmStateEntryValidatingMsOs20DescriptorSetIfPresent,
    &DsmStateEntryGettingDeviceQualifierDescriptor,
    &DsmStateEntryReturningErrorResponseOnDeviceQualifierQuery,
    &DsmStateEntryReturningOperationSuccessInCheckingFor20FullSpeed,
    &DsmStateEntryValidatingDeviceQualifierDescriptor,
    &DsmStateEntryAddingDeviceToGlobalChildList,
    &DsmStateEntryCheckingIfAddDeviceToGlobalListRetryCountExceededLimit,
    &DsmStateEntryCreatingChildPDOAndReportingToPnp,
    &DsmStateEntryDiscardSerialNumber,
    &DsmStateEntryInitializingAddDeviceToGlobalListRetryCount,
    &DsmStateEntryRequestingDeviceCycleInReportingToPnp,
    &DsmStateEntryReturningOperationSuccessInReportingToPnp,
    &DsmStateEntryReturningPortDetachDevice,
    &DsmStateEntryStoppingTimerAndRequestingCycleOnStopSuspend,
    &DsmStateEntryStoppingTimerOnDetachInReportingToPnp,
    &DsmStateEntryWaitingForDetachInReportingToPnp,
    &DsmStateEntryWaitingForDetachOrTimerInReportingToPnp,
    &DsmStateEntryWaitingForDuplicateDeviceToGoAway,
    &DsmStateEntryWaitingForTimerToFlushOnDetachInReportingToPnp,
    &DsmStateEntryCreatingUnknownChildPDOAndReportingToPnp,
    &DsmStateEntryMarkingUnknownDeviceAsFailed,
    &DsmStateEntryRequestingDeviceCycleForUnknownDevice,
    &DsmStateEntryReturningOperationSuccessInReportingUnknownDevice,
    &DsmStateEntryReturningPortDetachDeviceInReportingUnknownDevice,
    &DsmStateEntryAckingResumed,
    &DsmStateEntryAcquiringPowerReferenceInEnabled,
    &DsmStateEntryAcquiringPowerReferenceInSuspended,
    &DsmStateEntryAcquiringPowerReferenceInSuspendedOnResumeWithReset,
    &DsmStateEntryAcquiringPowerReferenceOnHubResume,
    &DsmStateEntryAcquiringPowerReferenceOnHubResumeWithReenumRequired,
    &DsmStateEntryAcquiringPowerReferenceOnHubResumeWtihReset,
    &DsmStateEntryAcquiringPowerReferenceOnResumeInS0,
    &DsmStateEntryCheckingIfDeviceNeedsResetOnResumeInSxOnEnabled,
    &DsmStateEntryCheckingIfDeviceNeedsResetOnResumeInSxOnSuspended,
    &DsmStateEntryCompletingUnexpectedD0ExitInHubSuspended,
    &DsmStateEntryDisabledOrFailedInHubSuspend,
    &DsmStateEntryDisablingPortOnPortEnabledInPendingHubStop,
    &DsmStateEntryReleasingReferenceOnHubSuspendAfterPSMSyncUp,
    &DsmStateEntryReleasingReferenceOnHubSuspendAfterPSMSyncUpInDisabledOrFailed,
    &DsmStateEntryReleasingReferenceOnHubSuspendOnDetachAfterPSMSyncUp,
    &DsmStateEntryResumingBeforeResetting,
    &DsmStateEntryReturningDeviceRenumerationOnHubResume,
    &DsmStateEntryReturningDeviceResumedOnHubResume,
    &DsmStateEntryReturningDeviceSuspendedOnHubResume,
    &DsmStateEntryReturningHubStopWithReferenceAcquiredInHubSuspended,
    &DsmStateEntryReturningPortDetach,
    &DsmStateEntrySuspendedInHubSuspend,
    &DsmStateEntrySuspendingPortOnPortEnabledInPendingHubSuspend,
    &DsmStateEntryWaitingForHubResumeInEnabled,
    &DsmStateEntryWaitingForHubResumeInSuspended,
    &DsmStateEntryWaitingForHubResumeWithRenumRequired,
    &DsmStateEntryWaitingForPSMSyncUp,
    &DsmStateEntryWaitingForPSMSyncUpOnPendingStop,
    &DsmStateEntryWaitingForPSMSyncUpOnPendingSuspend,
    &DsmStateEntryWaitingForPSMSyncUpOnResumeWithReset,
    &DsmStateEntryWaitingForResumeResponseFromPortOnHubStop,
    &DsmStateEntryWaitingForResumeResponseFromPortOnHubSuspend,
    &DsmStateEntryAcquiringAddressZeroOwnershipOnRenum,
    &DsmStateEntryAcquiringPowerReferenceOnStartInReEnum,
    &DsmStateEntryCancellingAcquiringAddress0OnDetachOnRenum,
    &DsmStateEntryCancellingRetryTimerOnDetachOnRenum,
    &DsmStateEntryCancellingRetryTimerOnStopSuspendOnRenum,
    &DsmStateEntryCheckingIfAltEnumCmdNeededInReenum,
    &DsmStateEntryCheckingIfDevicePDChargingPolicySupportedInReEnum,
    &DsmStateEntryCheckingIfDevicePorgrammingWasLosttInUnconfigured,
    &DsmStateEntryCheckingIfDeviceProgrammingWasLosttInConfigured,
    &DsmStateEntryCheckingIfEnumRetryReachedMaximumOnRenum,
    &DsmStateEntryCheckingIfEnumRetryReachedMaximumOnRenumWithAddress0Ownership,
    &DsmStateEntryCheckingIfEnumRetryReachedMaximumWithDeviceDisabledOnRenum,
    &DsmStateEntryCheckingIfItIsBootDeviceOnIdComparisionFailure,
    &DsmStateEntryCheckingIfRequestConfigDescOnResetIsSet,
    &DsmStateEntryCheckingIfSerialNumberShouldBeCompared,
    &DsmStateEntryClearingDeviceLostProgammingFlagOnRestart,
    &DsmStateEntryClearingDeviceProgrammingLostFlagOnRenum,
    &DsmStateEntryComparingDeviceOnReEnumeration,
    &DsmStateEntryComparingSerialNumberOnReEnumeration,
    &DsmStateEntryDeconfiguringEndpointsInControllerBeforeRenumerating,
    &DsmStateEntryDisablingDeviceInControllerOnStopSuspendOnRenum,
    &DsmStateEntryDisablingDeviceInControllerOnStopSuspendOnRenumWithAddress0Ownership,
    &DsmStateEntryDisablingOnEnumAfterFailureInReEnum,
    &DsmStateEntryDisablingOnEnumAfterFailureInReEnumWithAddress0Ownership,
    &DsmStateEntryDisablingOnEnumAfterFailureWithDeviceDisabledInReEnum,
    &DsmStateEntryEnablingDeviceInControllerInRenum,
    &DsmStateEntryEnumeratingAtAddressZeroOnRenum,
    &DsmStateEntryGettingConfigDescriptorWhileRenuemrating,
    &DsmStateEntryGettingDeviceDescriptorOnRenum,
    &DsmStateEntryGettingSerialNumberStringDescriptorWhileRenuemrating,
    &DsmStateEntryInitializingEnumRetryCountInReEnumDuringResetInConfigured,
    &DsmStateEntryInitializingEnumRetryCountInReEnumDuringResetInUnConfigured,
    &DsmStateEntryInitializingEnumRetryCountInReEnumDuringRestart,
    &DsmStateEntryPurgingDeviceTreeIoOnReEnumerationInConfigured,
    &DsmStateEntryPurgingDeviceTreeIoOnReEnumerationInUnConfigured,
    &DsmStateEntryPurgingIoOnEnumAfterFailureInReEnum,
    &DsmStateEntryPurgingIoOnEnumAfterFailureInReEnumWithAddress0Ownership,
    &DsmStateEntryReleasingAddressZeroOwnershipOnDetachOnEnumFailure,
    &DsmStateEntryReleasingAddressZeroOwnershipOnDetachWithDeviceEnabledOnRenum,
    &DsmStateEntryReleasingAddressZeroOwnershipOnDetachWithTimer,
    &DsmStateEntryReleasingAddressZeroOwnershipOnEnumFailureInRenum,
    &DsmStateEntryReleasingAddressZeroOwnershipOnRenum,
    &DsmStateEntryReleasingAddressZeroOwnershipOnStopSuspendOnEnumFailure,
    &DsmStateEntryReleasingAddressZeroOwnershipOnStopSuspendOnRenum,
    &DsmStateEntryReleasingAddressZeroOwnershipOnStopSuspendWithTimer,
    &DsmStateEntryReleasingPowerReferenceOnRenum,
    &DsmStateEntryReturningOperationFailureOnRenum,
    &DsmStateEntryReturningOperationFailureWithDeviceEnabledOnRenum,
    &DsmStateEntryReturningOperationSuccessOnRenum,
    &DsmStateEntryReturningPortDetachDeviceOnRenum,
    &DsmStateEntryReturningPortDetachDeviceWithDeviceEnabledOnRenum,
    &DsmStateEntrySendingMsOs20AltEnumCmdOnReenum,
    &DsmStateEntrySettingDevicePDChargingPolicyInReEnum,
    &DsmStateEntrySettingLPMValuesInReEnum,
    &DsmStateEntrySettingNonZeroAddressOnRenum,
    &DsmStateEntryStartingTimerForEnumRetryOnRenum,
    &DsmStateEntryStartingTimerForEnumRetryOnRenumWithAddress0Ownership,
    &DsmStateEntryStoppedSuspendedOnRenum,
    &DsmStateEntryWaitingForDetachAfterWrongDeviceDetectedOnBootDevicePort,
    &DsmStateEntryWaitingForDisableCompleteOnStopSuspendAfterFailureInReEnum,
    &DsmStateEntryWaitingForDisableCompleteOnStopSuspendOnRenum,
    &DsmStateEntryWaitingForDisableCompleteOnStopSuspendWithDeviceDisabledAfterFailureInReEnum,
    &DsmStateEntryWaitingForTimerToFlushOnDetachOnRenum,
    &DsmStateEntryWaitingForTimerToFlushOnStopSuspendOnRenum,
    &DsmStateEntryWaitingForUCXIoctlOnDetachOnRenum,
    &DsmStateEntryWaitingForUCXIoctlOnStopSuspend,
    &DsmStateEntryCheckingIfAnyAlternateInterfaceLeft,
    &DsmStateEntryConfiguringDevice,
    &DsmStateEntryCreatingNewEndpoints,
    &DsmStateEntryDeconfiguringEndpointsInControllerForDeviceConfiguration,
    &DsmStateEntryDeletingEndpointsForOldConfiguration,
    &DsmStateEntryMarkingAnyEndpointsToBeEnableAsDisabled,
    &DsmStateEntryProgrammingEndpointsAndSettingLPMValuesrForDeviceConfiguration,
    &DsmStateEntryProgrammingEndpointsInControllerForDeviceConfigurationFor20Devices,
    &DsmStateEntryReturningOperationFailureInReConfiguring,
    &DsmStateEntryReturningOperationSuccessInReConfiguring,
    &DsmStateEntrySettingDeviceInterfaceInConfiguring,
    &DsmStateEntryDisablingPortOnHubSuspendInCleanup,
    &DsmStateEntryFlushingHubPowerDownEvents,
    &DsmStateEntryFlushingHubPowerUpEvents,
    &DsmStateEntryFlushingHubStopAfterSuspendEvent,
    &DsmStateEntryFlushingPnpEvents,
    &DsmStateEntryFowardingStreamsRequestToUcxInWaitingForDevicePortEvents,
    &DsmStateEntryReleasingPowerReferenceOnHubSuspendInCleanup,
    &DsmStateEntryReturningPortDetachDeviceFromIgnoringDevicePort,
    &DsmStateEntryReturningPortDisabledOnHubSuspendInCleanup,
    &DsmStateEntryWaitingForDevicePortEvents,
    &DsmStateEntryFlushingHubPowerDownEventsWithPortOff,
    &DsmStateEntryFlushingHubPowerUpEventsWithPortOff,
    &DsmStateEntryFlushingHubStopAfterSuspendEventWithPortOff,
    &DsmStateEntryFlushingPnpEventsWithPortOff,
    &DsmStateEntryFowardingStreamsRequestToUcxInWaitingForDevicePortEventsWithPortOff,
    &DsmStateEntryWaitingForDevicePortEventsWithPortOff,
    &DsmStateEntryDisablingPortOnHubSuspendForFailedDevice,
    &DsmStateEntryFailedDeviceHubSuspended,
    &DsmStateEntryFlushingHubPowerDownEventsForFailedDevice,
    &DsmStateEntryFlushingHubPowerUpEventsForFailedDevice,
    &DsmStateEntryFlushingHubStopAfterSuspendEventForFailedDevice,
    &DsmStateEntryFlushingPnpEventsForFailedDevice,
    &DsmStateEntryReleasingPowerReferenceOnDetachForFailedDevice,
    &DsmStateEntryReturningPortDetachDeviceForFailedDevice,
    &DsmStateEntryWaitingForDevicePortEventsForFailedDevice,
    &DsmStateEntryFlushingHubStartResumeEvent,
    &DsmStateEntryFlushingHubSuspendEvent,
    &DsmStateEntryFlushingPnpEventExceptStop,
    &DsmStateEntryFowardingStreamsRequestToUcxInWaitingForDevicePortEventsExceptStop,
    &DsmStateEntryWaitingForDevicePortEventsExceptStop,
    &DsmStateEntryFlushingHubResumeEventForBootDevice,
    &DsmStateEntryFlushingHubSuspendEventForBootDevice,
    &DsmStateEntryFlushingPnpEventsForBootDevice,
    &DsmStateEntryWaitingForDeviceHubEventsForBootDevice,
    &DsmStateEntryAckingPortEventInD3Cold,
    &DsmStateEntryAckingPortResumed,
    &DsmStateEntryCheckingIfDeviceArmedForWakeOnResume,
    &DsmStateEntryCheckingIfDeviceNeedsResetOnResumeInS0,
    &DsmStateEntryCheckingIfDeviceShouldBeDisarmedForWakeOnResume,
    &DsmStateEntryCheckingIfResetOnLastResumeFlagIsSet,
    &DsmStateEntryCheckingIfResetOnLastResumeFlagIsSetOnSuspended,
    &DsmStateEntryCompletingD0EntryOnDetach,
    &DsmStateEntryCompletingD0EntryOnPortFailure,
    &DsmStateEntryDisabledAfterD3Cold,
    &DsmStateEntryDisablingOnHubSuspendNeedingRenumeration,
    &DsmStateEntryDisablingPortOnStopFromSuspended,
    &DsmStateEntryDisarmingDeviceForWakeOnD0Entry,
    &DsmStateEntryDisarmingDeviceForWakeOnPortResumed,
    &DsmStateEntryPurgingDeviceIoOnHubSuspendWhileWaitingForD0Entry,
    &DsmStateEntryPurgingDeviceIoOnHubSuspendWhileWaitingForD0EntryOnHwWake,
    &DsmStateEntryReleasingPowerReferenceOnDetachWhileSuspending,
    &DsmStateEntryReleasingPowerReferenceOnDetachWhileWaitingForHubSuspend,
    &DsmStateEntryReleasingPowerReferenceOnPortFailureWhileWaitingForHubSuspend,
    &DsmStateEntryResumingFromSelectiveSuspend,
    &DsmStateEntryReturningDetachDevice,
    &DsmStateEntryReturningDeviceResumed,
    &DsmStateEntryReturningHubStopFromSuspended,
    &DsmStateEntryReturningPortFailureOnResume,
    &DsmStateEntryReturningRenumerateDeviceOnResume,
    &DsmStateEntrySettingResetOnLastResumeDueToD3Cold,
    &DsmStateEntrySettingResetOnLastResumeFlagForPDO,
    &DsmStateEntrySettingResetOnLastResumeFlagForPDOAfterD3Cold,
    &DsmStateEntryStartingDeviceIoOnDeviceResumeOnHwWake,
    &DsmStateEntryStartingDeviceIoOnDeviceResumeOnSwWake,
    &DsmStateEntrySuspended,
    &DsmStateEntrySuspendedWithHubSuspended,
    &DsmStateEntrySuspendedWithHubSuspendedInD3Cold,
    &DsmStateEntrySuspendingBackUnarmedDevice,
    &DsmStateEntrySuspendingWakeArmedDeviceOnHubSuspend,
    &DsmStateEntryWaitingForD0EntryOnHwWake,
    &DsmStateEntryWaitingForD0EntryOnPendingRenumerate,
    &DsmStateEntryWaitingForSuspendCompleteOnHubSuspend,
    &DsmStateEntryAbortingDeviceIoInSuspendingBeforeArmingForWakeFailure,
    &DsmStateEntryArmingDeviceForWake,
    &DsmStateEntryCheckingIfDeviceArmedForWakeOnSuspending,
    &DsmStateEntryCompletingWaitWakeOnDetachDuringSuspending,
    &DsmStateEntryCompletingWaitWakeOnSuspendFailure,
    &DsmStateEntryPurgingDeviceIoInSuspending,
    &DsmStateEntryPurgingDeviceIoInSuspendingAfterArmingForWake,
    &DsmStateEntryPurgingDeviceIoInSuspendingAfterArmingForWakeFailure,
    &DsmStateEntryReturningOperationFailureInSuspending,
    &DsmStateEntryReturningOperationSuccessInSuspending,
    &DsmStateEntryReturningPortDetachDeviceInSuspending,
    &DsmStateEntryReturningUnexpectedHubSuspendInSuspending,
    &DsmStateEntrySuspending,
    &DsmStateEntrySuspendingPortOnFailureInSuspending,
    &DsmStateEntrySuspendingWithArmedForWake,
    &DsmStateEntryWaitingForPortResponseOnHubSuspend,
    &DsmStateEntryCreatingNewEndpointsInSettingInterface,
    &DsmStateEntryDeletingEndpointsForNewInterfaceOnFailure,
    &DsmStateEntryDeletingEndpointsForOldInterface,
    &DsmStateEntryDeletingEndpointsForOldInterfaceOnFailure,
    &DsmStateEntryDisablingEndpointsForTheCurrentInterface,
    &DsmStateEntryDisablingEndpointsForTheNewInterfaceOnFailure,
    &DsmStateEntryPreparingEndpointAndInterfaceListsOnSelectInterface,
    &DsmStateEntryProgrammingEndpointsAndSettingLPMValuesInSettingInterface,
    &DsmStateEntryProgrammingEndpointsInControllerInSettingInterfaceFor20Devices,
    &DsmStateEntryReturningOperationFailureInSettingInterface,
    &DsmStateEntryReturningOperationSuccessInSettingInterface,
    &DsmStateEntrySettingDeviceInterfaceInSettingInterface,
    &DsmStateEntrySettingDeviceInterfaceInSettingInterfaceOnFailureForCompat,
    &DsmStateEntryCheckingIfDeviceSpeedChanged,
    &DsmStateEntryCheckingIfFirstEnumTryInRenum,
    &DsmStateEntryGettingDeviceDescriptorInReEnumAtZero,
    &DsmStateEntryLoggingReset1TimeoutInReEnumAtZero,
    &DsmStateEntryLoggingReset2TimeoutInReEnum,
    &DsmStateEntryNotifyingDeviceResetToUCXAfterReset1InRenum,
    &DsmStateEntryNotifyingDeviceResetToUCXAfterReset2InRenum,
    &DsmStateEntryResetting1InRenum,
    &DsmStateEntryResetting2InRenum,
    &DsmStateEntryReturningDeviceOperationFailureInReEnumAtZero,
    &DsmStateEntryReturningDeviceOperationSuccessInReEnumAtZero,
    &DsmStateEntryReturningHubStopInReEnumAtZero,
    &DsmStateEntryReturningHubSuspendInReEnumAtZero,
    &DsmStateEntryReturningPortDetachDeviceInReEnumAtZero,
    &DsmStateEntryStoppingTimerOnHubStopInReEnumAtZero,
    &DsmStateEntryStoppingTimerOnHubSuspendInReEnumAtZero,
    &DsmStateEntryStoppingTimerOnPortDetachInReEnumAtZero,
    &DsmStateEntryValidatingDeviceDescriptorInReEnumAtZero,
    &DsmStateEntryWaitingForPortResetOnHubStopInReEnumAtZero,
    &DsmStateEntryWaitingForPortResetOnHubSuspendInReEnumAtZero,
    &DsmStateEntryWaitingForPostReset1TimerInRenum,
    &DsmStateEntryWaitingForPostReset2ExtendedTimerInRenum,
    &DsmStateEntryWaitingForPostReset2TimerInRenum,
    &DsmStateEntryWaitingForStopSuspendOnReset1InReEnumAtZero,
    &DsmStateEntryWaitingForTimerOnHubStopInReEnumAtZero,
    &DsmStateEntryWaitingForTimerOnHubSuspendInReEnumAtZero,
    &DsmStateEntryWaitingForTimerOnPortDetachInReEnumAtZero,
    &DsmStateEntryCheckingIfFirstReEnumTryFor30Device,
    &DsmStateEntryDeterminingPortResetTypeRequired,
    &DsmStateEntryLoggingResetTimeoutInResettingFor30,
    &DsmStateEntryNotifyingDeviceResetToUCXInResettingFor30,
    &DsmStateEntryQueueingHotPortResetFor30,
    &DsmStateEntryQueueingWarmPortResetFor30,
    &DsmStateEntryReturningDeviceOperationFailureInResettingFor30,
    &DsmStateEntryReturningDeviceOperationSuccessInInResettingFor30,
    &DsmStateEntryReturningHubStopInResettingFor30,
    &DsmStateEntryReturningHubSuspendInResettingFor30,
    &DsmStateEntryReturningPortDetachDeviceInResettingFor30,
    &DsmStateEntryStartingPostResetTimerFor30,
    &DsmStateEntryStoppingTimerOnHubStopInResettingFor30,
    &DsmStateEntryStoppingTimerOnHubSuspendInResettingFor30,
    &DsmStateEntryStoppingTimerOnPortDetachInResettingFor30,
    &DsmStateEntryWaitingForPortResetCompleteFor30,
    &DsmStateEntryWaitingForPortResetOnHubStopInResettingFor30,
    &DsmStateEntryWaitingForPortResetOnHubSuspendInResettingFor30,
    &DsmStateEntryWaitingForStopSuspendOnReset1InResettingFor30,
    &DsmStateEntryWaitingForTimerOnHubStopInResettingFor30,
    &DsmStateEntryWaitingForTimerOnHubSuspendInResettingFor30,
    &DsmStateEntryWaitingForTimerOnPortDetachInResettingFor30,
    &DsmStateEntryAllocatingBufferForMsOsExtendedPropertyDescriptor,
    &DsmStateEntryCheckingIfMsOs20RegistryValuesShouldBeInstalled,
    &DsmStateEntryCheckingIfMSOSExtendedPropertyDescriptorShouldBeQueried,
    &DsmStateEntryFreeingBufferAllocatedForMSOSExtendedProperty,
    &DsmStateEntryGettingMSOSExtendedPropertyDescriptor,
    &DsmStateEntryGettingMSOSExtendedPropertyDescriptorHeader,
    &DsmStateEntryInstallingMsOs20RegistryValues,
    &DsmStateEntrySettingExtPropDescSemaphoreForMsOs2,
    &DsmStateEntrySettingExtPropDescSemaphoreRegistryFlag,
    &DsmStateEntrySignalingPnpEventForMSOSExtInstall,
    &DsmStateEntryValidatingMSOSExtendedPropertyDescriptor,
    &DsmStateEntryValidatingMSOSExtendedPropertyDescriptorHeader,
    &DsmStateEntryWaitingForInstallMSOSExt,
    &DsmStateEntryWritingCustomPropertiesInRegistry,
    &DsmStateEntryCheckingIfLTMShouldBeEnabled,
    &DsmStateEntryCheckingIfSetIsochDelayShouldBeSkipped,
    &DsmStateEntryCheckingIfSetSelShouldBeSkipped,
    &DsmStateEntryEnablingLTM,
    &DsmStateEntryReturningOperationFailureInSettingLPMValues,
    &DsmStateEntryReturningOperationSuccessInSettingLPMValues,
    &DsmStateEntrySettingSEL,
    &DsmStateEntrySetttingIsochDelay,
    &DsmStateEntryCheckingIfEnableU1NeedsToBeUpdated,
    &DsmStateEntryCheckingIfEnableU2NeedsToBeUpdated,
    &DsmStateEntryCheckingIfEndpointsNeedToBeConfiguredAfterDecreasingExitLatency,
    &DsmStateEntryCheckingIfEndpointsNeedToBeProgrammedAfterIncreasingLatency,
    &DsmStateEntryCheckingIfEndpointsToBeDisabled,
    &DsmStateEntryCheckingIfEndpointsToBeDisabledOnDetach,
    &DsmStateEntryCheckingIfExitLatencyNeedsToBeDecreased,
    &DsmStateEntryCheckingIfExitLatencyNeedsToBeIncreased,
    &DsmStateEntryCheckingIfU1TimeoutNeedsToBeChanged,
    &DsmStateEntryCheckingIfU2TimeoutNeedsToBeChanged,
    &DsmStateEntryComputingU1U2TimeoutsAndExitLatency,
    &DsmStateEntryDisablingEndpointsOnConfigFailure,
    &DsmStateEntryDisablingEndpointsOnConfigFailureOnDetach,
    &DsmStateEntryDisablingU1,
    &DsmStateEntryDisablingU1U2OnExitLatencyTooLargeError,
    &DsmStateEntryDisablingU2,
    &DsmStateEntryEnablingU1,
    &DsmStateEntryEnablingU2,
    &DsmStateEntryInitializingU1U2Flags,
    &DsmStateEntryProgrammingEndpointsInControllerAfterDecreasingExitLatency,
    &DsmStateEntryProgrammingEndpointsInControllerAfterIncreasingExitLatency,
    &DsmStateEntryQueuingSetU1TimeoutToPsm,
    &DsmStateEntryQueuingSetU2TimeoutToPsm,
    &DsmStateEntryReturningOperationFailureInUpdatingLPMValues,
    &DsmStateEntryReturningOperationSuccessInUpdatingLPMValues,
    &DsmStateEntryReturningPortDetachDeviceInUpdatingLPMValues,
    &DsmStateEntryUpdatingDeviceExitLatencyInTheControllerAfterDecreasingExitLatency,
    &DsmStateEntryUpdatingDeviceExitLatencyInTheControllerAfterIncreasingExitLatency,
    &DsmStateEntryUpdatingDeviceStatusToU1Disabled,
    &DsmStateEntryUpdatingDeviceStatusToU1Enabled,
    &DsmStateEntryUpdatingDeviceStatusToU2Disabled,
    &DsmStateEntryUpdatingDeviceStatusToU2Enabled,
    &DsmStateEntryComputingLPMTimeoutValuesInUnconfigured,
    &DsmStateEntryQueuingSetU2TimeoutToPsmForEnumeratedDevice,
    &DsmStateEntryReturningOperationFailureInUpdatingLPMValuesInUnconfigured,
    &DsmStateEntryReturningOperationSuccessInUpdatingLPMValuesInUnconfigured,
    &DsmStateEntryGettingRemoteWakeCapability,
    &DsmStateEntryReturningOperationFailureInGettingRemoteWakeCap,
    &DsmStateEntryReturningOperationSuccessInGettingRemoteWakeCap,
    &DsmStateEntryValidatingInterfaceStatusForRemoteWake,
    &DsmStateEntryCheckingIf20LPMShouldBeEnabled,
    &DsmStateEntryReturningOperationSuccessInSetting20LPMValues,
    &DsmStateEntryWaitingFor20LpmUpdateIoctlToComplete,
};
