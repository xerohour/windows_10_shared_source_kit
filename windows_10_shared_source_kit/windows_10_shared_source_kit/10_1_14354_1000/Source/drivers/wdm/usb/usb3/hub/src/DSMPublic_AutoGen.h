
/*++
Copyright (c) Microsoft Corporation

Module Name:

DSMPublic_AutoGen.h

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/

#pragma once



//
// Event List For the State Machines
//
typedef enum _DSM_EVENT{
    DsmEventNull                                     = 0x000003e8,
    DsmEventContinueIfNoSubState                     = 0x000003e9 | EventPrioritySync,
    DsmEventTimerFired                               = 0x000003ea | EventPriorityNormal,
    DsmEventBootResetTimerFired                      = 0x00000fa4 | EventPriorityLow,
    DsmEventClientRequestCycle                       = 0x00000fa8 | EventPriorityLow,
    DsmEventClientRequestIdle                        = 0x00000fac | EventPriorityLow,
    DsmEventClientRequestOpenOrCloseStreams          = 0x00000fb0 | EventPriorityLow,
    DsmEventClientRequestReset                       = 0x00000fb4 | EventPriorityLow,
    DsmEventClientRequestResetPipe                   = 0x00000fb8 | EventPriorityLow,
    DsmEventClientRequestSelectConfiguration         = 0x00000fbc | EventPriorityLow,
    DsmEventClientRequestSelectNullConfiguration     = 0x00000fc0 | EventPriorityLow,
    DsmEventClientRequestSetInterface                = 0x00000fc4 | EventPriorityLow,
    DsmEventClientRequestSyncClearStall              = 0x00000fc8 | EventPriorityLow,
    DsmEventClientRequestSyncResetPipe               = 0x00000fcc | EventPriorityLow,
    DsmEventDeviceDisableUState                      = 0x00000fa8 | EventPrioritySync,
    DsmEventDeviceEnableUState                       = 0x00000fac | EventPrioritySync,
    DsmEventDeviceFailEnumeration                    = 0x00000fb0 | EventPrioritySync,
    DsmEventDeviceHotReset                           = 0x00000fb4 | EventPrioritySync,
    DsmEventDeviceIgnoreError                        = 0x00000fb8 | EventPrioritySync,
    DsmEventDeviceNoReset                            = 0x00000fbc | EventPrioritySync,
    DsmEventDeviceRenumerateOnHubResume              = 0x00000fc0 | EventPrioritySync,
    DsmEventDeviceResumed                            = 0x00000fc4 | EventPrioritySync,
    DsmEventDeviceResumedOnHubResume                 = 0x00000fc8 | EventPrioritySync,
    DsmEventDeviceRetryEnum                          = 0x00000fcc | EventPrioritySync,
    DsmEventDeviceSuspendedOnHubResume               = 0x00000fd0 | EventPrioritySync,
    DsmEventDeviceWarmReset                          = 0x00000fd4 | EventPrioritySync,
    DsmEventFDORequestGetDescriptor                  = 0x00000fd0 | EventPriorityLow,
    DsmEventHubResume                                = 0x00000fa4 | EventPriorityNormal,
    DsmEventHubResumeInS0                            = 0x00000fa8 | EventPriorityNormal,
    DsmEventHubResumeWithReset                       = 0x00000fac | EventPriorityNormal,
    DsmEventHubStart                                 = 0x00000fb0 | EventPriorityNormal,
    DsmEventHubStop                                  = 0x00000fb4 | EventPriorityNormal,
    DsmEventHubStopAfterSuspend                      = 0x00000fb8 | EventPriorityNormal,
    DsmEventHubStopWithReferenceAcquired             = 0x00000fd8 | EventPrioritySync,
    DsmEventHubSuspend                               = 0x00000fbc | EventPriorityNormal,
    DsmEventLPMPowerSettingChange                    = 0x00000fd4 | EventPriorityLow,
    DsmEventNo                                       = 0x00000fdc | EventPrioritySync,
    DsmEventNoPingResponseError                      = 0x00000fd8 | EventPriorityLow,
    DsmEventOperationFailure                         = 0x00000fe0 | EventPrioritySync,
    DsmEventOperationFailureWithDeviceEnabled        = 0x00000fe4 | EventPrioritySync,
    DsmEventOperationPending                         = 0x00000fe8 | EventPrioritySync,
    DsmEventOperationSuccess                         = 0x00000fec | EventPrioritySync,
    DsmEventPDOCleanup                               = 0x00000fdc | EventPriorityLow,
    DsmEventPDOD0Entry                               = 0x00000fe0 | EventPriorityLow,
    DsmEventPDOD0Exit                                = 0x00000fe4 | EventPriorityLow,
    DsmEventPDOD0ExitFinal                           = 0x00000fe8 | EventPriorityLow,
    DsmEventPDOInstallMSOSExt                        = 0x00000ff8 | EventPriorityLow,
    DsmEventPDOPrepareForHibernation                 = 0x00000fec | EventPriorityLow,
    DsmEventPDOPreStart                              = 0x00000ffc | EventPriorityLow,
    DsmEventPDOReportedMissing                       = 0x00000ff0 | EventPriorityLow,
    DsmEventPortAttachDevice                         = 0x00000fc0 | EventPriorityNormal,
    DsmEventPortDetachDevice                         = 0x00000fc4 | EventPriorityNormal,
    DsmEventPortDetachDeviceWithDeviceEnabled        = 0x00000ff0 | EventPrioritySync,
    DsmEventPortDisabled                             = 0x00000fc8 | EventPriorityNormal,
    DsmEventPortDisabledOnHubSuspend                 = 0x00000ff4 | EventPrioritySync,
    DsmEventPortFailed                               = 0x00000fcc | EventPriorityNormal,
    DsmEventPortReAttachDevice                       = 0x00000fd0 | EventPriorityNormal,
    DsmEventPortResetComplete                        = 0x00000fd4 | EventPriorityNormal,
    DsmEventPortResetFailedDueToPendingSuspend       = 0x00000fd8 | EventPriorityNormal,
    DsmEventPortResetTimedOut                        = 0x00000fdc | EventPriorityNormal,
    DsmEventPortResumed                              = 0x00000fe0 | EventPriorityNormal,
    DsmEventPortResumedFromD3Cold                    = 0x00000fe4 | EventPriorityNormal,
    DsmEventPortResumeFailedDueToPendingSuspend      = 0x00000fe8 | EventPriorityNormal,
    DsmEventPortResumeTimedOut                       = 0x00000fec | EventPriorityNormal,
    DsmEventPortStateDisabled                        = 0x00000ff0 | EventPriorityNormal,
    DsmEventPortStateEnabled                         = 0x00000ff4 | EventPriorityNormal,
    DsmEventPortStateEnabledOnReconnect              = 0x00000ff8 | EventPriorityNormal,
    DsmEventPortStateSuspended                       = 0x00000ffc | EventPriorityNormal,
    DsmEventPortSuspended                            = 0x00001000 | EventPriorityNormal,
    DsmEventPortTimeoutUpdated                       = 0x00001004 | EventPriorityNormal,
    DsmEventQueryDeviceText                          = 0x00000ff4 | EventPriorityLow,
    DsmEventTransferFailure                          = 0x00000fa4 | EventPriorityCritical,
    DsmEventTransferStall                            = 0x00000fa8 | EventPriorityCritical,
    DsmEventTransferSuccess                          = 0x00000fac | EventPriorityCritical,
    DsmEventUCXClientRequestComplete                 = 0x00000fb0 | EventPriorityCritical,
    DsmEventUCXIoctlFailure                          = 0x00000fb4 | EventPriorityCritical,
    DsmEventUCXIoctlFailureDueToExitLatencyTooLarge  = 0x00000fb8 | EventPriorityCritical,
    DsmEventUCXIoctlSuccess                          = 0x00000fbc | EventPriorityCritical,
    DsmEventYes                                      = 0x00000ff8 | EventPrioritySync,
} DSM_EVENT, *PDSM_EVENT;



//
// Definitions for State Machine Operations 
//
DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_AbortDeviceIoUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBMUX_AckStopAfterSuspend;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_AcquireAddress0OwnershipUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBMUX_AcquireDevicePowerReference;

DSM_OPERATION_SYNC_DISPATCH         HUBPDO_AcquireWdfPowerReferenceOnHub;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_AcquireWdfReferenceOnPort;

DSM_OPERATION_SYNC_PASSIVE          HUBMISC_AddDeviceInfoToTheGlobalChildList;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_AllocateBufferForMSOSExtendedPropertyDescriptor;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_ArmDeviceForWakeUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_BugcheckSystemOnBootDeviceEnumerationFailure;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_CancelControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_CancelUCXIoctl;

DSM_OPERATION_SYNC_DISPATCH         HUBUCX_CheckIf20LPMIsEnabledAndUpdateUsingUCXIoctl;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIf20TypeDeviceNeedsSecondReset;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_CheckIfAlternateInterfaceLeftToBeSet;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_CheckIfAlternateModeStringDescriptorShouldBeQueried;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_CheckIfBillboardBOSDescriptorIsPresent;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_CheckIfBillboardStringDescriptorShouldBeQueried;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfBOSDescriptorQueryShouldBeSkipped;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_CheckIfCompleteBOSDescriptorHasBeenRetrieved;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfContainerIdShouldBeQueried;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfDeviceIsAtSuperSpeedAndNeedsToBeDisabled;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfDevicePDChargingPolicySupported;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfDeviceProgrammingWasLost;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfDeviceShouldBeDisabledOnRemove;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfDeviceShouldBeDisarmedForWakeOnResume;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfDeviceSpeedChanged;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfDualRoleSupported;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfEnableU1NeedsToBeUpdated;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfEnableU2NeedsToBeUpdated;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfEndpointPurgeListIsNonEmpty;

DSM_OPERATION_SYNC_DISPATCH         HUBUCX_CheckIfEndpointsNeedToBeDisabledOnConfigFailure;

DSM_OPERATION_SYNC_DISPATCH         HUBUCX_CheckIfEndpointsNeedToBeDisabledOrEnabled;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfFirstEnumTry;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfIProductIsZero;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfLTMShouldBeEnabled;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfMaxExitLatencyNeedsToBeDecreased;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfMaxExitLatencyNeedsToBeIncreased;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfMsOs20DescriptorSupported;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfMsOs20RegistryValuesShouldBeInstalled;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfMSOSDescriptorShouldBeQueried;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfMSOSExtendedConfigDescriptorIsSupported;

DSM_OPERATION_SYNC_PASSIVE          HUBMISC_CheckIfMSOSExtendedPropertyDescriptorShouldBeQueried;

DSM_OPERATION_SYNC_DISPATCH         HUBPDO_CheckIfPdoIsStarted;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfProductIdDescriptorShouldBeQueried;

DSM_OPERATION_SYNC_DISPATCH         HUBPDO_CheckIfResetOnLastResumeFlagIsSet;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfSameDeviceIsConnected;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfSerialNumberIsIdentical;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfSerialNumberShouldBeQueriedOnRenumeration;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfSerialNumberStringIndexIsZero;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfSetIsochDelayShouldBeSkipped;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfSetSelShouldBeSkipped;

DSM_OPERATION_SYNC_DISPATCH         HUBPDO_CheckIfTargetPipeIsForZeroBw;

DSM_OPERATION_SYNC_DISPATCH         HUBPDO_CheckIfTargetPipeIsIsochronous;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfU1TimeoutNeedsToBeUpdated;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfU2ShouldBeSetForEnumeratedDevice;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfU2TimeoutNeedsToBeUpdated;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfWaitRequiredAfterSetAddress;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckMSOSDescriptorIfContainerIdIsSupported;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_ClearDeviceNeedsReprogrammingFlag;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_ClearEndpointHaltUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_ClearFailIo;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_ClearResetOnLastResumeFlag;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_CompleteClientSerialRequestWithLastStatus;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_CompleteClientSerialRequestWithStatusFailed;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_CompleteClientSerialRequestWithStatusSuccess;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_CompletePdoPreStart;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_CompleteWaitWake;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_CompleteWaitWakeAsCancelled;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_ComputeMaximumExitLatency;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_ComputeU1Timeout;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_ComputeU2Timeout;

DSM_OPERATION_SYNC_PASSIVE          HUBUCX_CreateDefaultEndpointInUCX;

DSM_OPERATION_SYNC_PASSIVE          HUBUCX_CreateDeviceInUCX;

DSM_OPERATION_SYNC_PASSIVE          HUBUCX_CreateEndpointsInUCX;

DSM_OPERATION_SYNC_PASSIVE          HUBPDO_CreatePdo;

DSM_OPERATION_SYNC_PASSIVE          HUBPDO_CreateUnknownPdo;

DSM_OPERATION_ASYNC_PASSIVE         HUBUCX_DeleteDefaultEndpointFromUCX;

DSM_OPERATION_ASYNC_PASSIVE         HUBUCX_DeleteDeviceFromUCX;

DSM_OPERATION_ASYNC_PASSIVE         HUBUCX_DeleteEndpointsAndFreeResourcesForCurrentConfiguration;

DSM_OPERATION_ASYNC_PASSIVE         HUBUCX_DeleteEndpointsAndFreeResourcesForNewInterface;

DSM_OPERATION_ASYNC_PASSIVE         HUBUCX_DeleteEndpointsAndFreeResourcesForOldConfiguration;

DSM_OPERATION_ASYNC_PASSIVE         HUBUCX_DeleteEndpointsAndFreeResourcesForOldInterface;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_DetermineDeviceResetTypeRequiredAndClearResetFlags;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_DisableAllEndpointsForCurrentConfigurationUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_DisableAllEndpointsForNewInterfaceUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_DisableDeviceAndDefaultEndpointUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_DisableEndpointsInPendingDisableListUsingUCXIoctl;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_DisableU1U2OnExitLatencyTooLargeError;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_DisableU1UsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_DisableU2UsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_DisarmDeviceForWakeUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_DiscardSerialNumber;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_EnableDeviceAndDefaultEndpointUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_EnableDisableEndpointsUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_EnableLTMUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_EnableU1UsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_EnableU2UsingControlTransfer;

DSM_OPERATION_SYNC_DISPATCH         HUBPDO_FindAndStoreMatchingInternalPipeHandle;

DSM_OPERATION_ASYNC_PASSIVE         HUBUCX_ForwardRequestToUCX;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_FreeBufferForMSOSExtendedPropertyDescriptor;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetAlternateModeStringDescriptorUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetBillboardStringDescriptorUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetBOSDescriptorHeaderUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetBOSDescriptorUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetConfigDescriptorWithDefaultSizeUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetConfigDescriptorWithReturnedLengthUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetDescriptorForFdoRequestUsingPreformattedUrb;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetDeviceDescriptorUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetDeviceQualifierDescriptorUsingControlTransfer;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_GetErrorResponseOnBOSDescriptorFailure;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_GetErrorResponseOnBOSDescriptorHeaderFailure;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_GetErrorResponseOnDeviceQualifierDescriptorFailure;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_GetErrorResponseOnLanguageIdDescriptorFailure;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_GetErrorResponseOnMSOSContainerIdDescriptorFailure;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_GetErrorResponseOnMSOSContainerIdDescriptorHeaderFailure;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_GetErrorResponseOnMSOSDescriptorFailure;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_GetErrorResponseOnMSOSExtendedConfigDescriptorFailure;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_GetErrorResponseOnMSOSExtendedConfigDescriptorHeaderFailure;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_GetErrorResponseOnProductStringDescriptorFailure;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_GetErrorResponseOnSerialNumberDescriptorFailure;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_GetGenericErrorResponseOnDescriptorFailure;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetInitialDeviceDescriptorUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetLanguageIdStringDescriptorUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetMSOS20DescriptorSetUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetMSOSContainerIdDescriptorHeaderUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetMSOSContainerIdDescriptorUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetMSOSDescriptorUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetMSOSExtendedConfigDescriptorHeaderUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetMSOSExtendedConfigDescriptorUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetMSOSExtendedPropertyDescriptorHeaderUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetMSOSExtendedPropertyDescriptorUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetProductIdStringDescriptorUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetRemoteWakeCapabilityUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_GetSerialNumberStringDescriptorUsingControlTransfer;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_IncrementAndCheckIfDuplicateSerialNumberRetryCountExceededLimit;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_IncrementEnumRetryCountAndCheckIfReachedMax;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_InitializeDuplicateSerialNumberRetryCount;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_InitializeEnumRetryCount;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_InitializeU1U2Flags;

DSM_OPERATION_SYNC_PASSIVE          HUBMISC_InstallMsOs20RegistryProperties;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_IsBytesReturnedLessThanTotalLengthForConfigDescriptor;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_IsDeviceArmedForWake;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_IsDeviceSuperSpeedCapable;

DSM_OPERATION_SYNC_DISPATCH         HUBPDO_IsHiberFileEnabled;

DSM_OPERATION_SYNC_DISPATCH         HUBPDO_IsItABootDevice;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_IsMsOs20AltEnumCmdNeededAfterGettingBos;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_IsMsOs20AltEnumCmdNeededInEnum;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_IsMsOs20AltEnumCmdNeededOnReenum;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_IsMsOs20AltEnumNeeded;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_IsResetOnResumeInS0Set;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_IsResetOnResumeInSxSet;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_LogDeviceReEnumeration;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_LogResetTimeout;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_MarkDeviceAsDisconnectedInGlobalChildList;

DSM_OPERATION_ASYNC_PASSIVE         HUBMISC_MarkDeviceAsNotSupportingContainerId;

DSM_OPERATION_ASYNC_PASSIVE         HUBMISC_MarkDeviceAsNotSupportingMSOSDescriptor;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_MarkEndpointsToBeEnabledAsDisabled;

DSM_OPERATION_ASYNC_PASSIVE         HUBPDO_NotifyDeviceDisconnected;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_NotifyDeviceReconnected;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_NotifyDeviceResetUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_NotifyDifferentDeviceAttachedOnBootDevicePort;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_NotifyEnumerationFailureForBootDevice;

DSM_OPERATION_SYNC_PASSIVE          HUBMISC_ParseAndWriteCustomPropertiesInRegistry;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_PerformPostSurpriseRemovalRecoveryActionsIfBootDevice;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_PrepareEndpointAndInterfaceListsForConfiguringDeviceOnReset;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_PrepareEndpointAndInterfaceListsForConfiguringDeviceOnSelectConfiguration;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_PrepareEndpointAndInterfaceListsForConfiguringDeviceOnSelectInterface;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_PurgeDeviceIoForSuspendUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_PurgeDeviceIoUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_PurgeDeviceTreeIoUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_PurgeEndpointsUsingUCXIoctl;

DSM_OPERATION_SYNC_PASSIVE          HUBMISC_QueryAndCacheRegistryValuesForDevice;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_QueryCachedRegistryIfIgnoreHWSerNumIsSet;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_QueryCachedRegistryIfRequestConfigDescOnResetIsSet;

DSM_OPERATION_ASYNC_DISPATCH        HUBPSM_QueueDeviceDetachedToPSM;

DSM_OPERATION_ASYNC_DISPATCH        HUBPSM_QueuePrepareForHibernationToPSM;

DSM_OPERATION_ASYNC_DISPATCH        HUBPSM_QueueRequestCycleToPSM;

DSM_OPERATION_ASYNC_DISPATCH        HUBPSM_QueueRequestDisableSuperSpeedToPSM;

DSM_OPERATION_ASYNC_DISPATCH        HUBPSM_QueueRequestDisableToPSM;

DSM_OPERATION_ASYNC_DISPATCH        HUBPSM_QueueRequestResetToPSM;

DSM_OPERATION_ASYNC_DISPATCH        HUBPSM_QueueRequestResumeToPSM;

DSM_OPERATION_ASYNC_DISPATCH        HUBPSM_QueueRequestSuspendToPSM;

DSM_OPERATION_ASYNC_DISPATCH        HUBPSM_QueueRequestWarmResetToPSM;

DSM_OPERATION_ASYNC_DISPATCH        HUBPSM_QueueSetU1TimeoutToPSM;

DSM_OPERATION_ASYNC_DISPATCH        HUBPSM_QueueSetU2TimeoutToPSM;

DSM_OPERATION_SYNC_DISPATCH         HUBMUX_RegisterWithHSM;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_ReleaseAddress0OwnershipFromUCX;

DSM_OPERATION_ASYNC_DISPATCH        HUBMUX_ReleaseDevicePowerReference;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_ReleaseWdfPowerReferenceOnHub;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_ReleaseWdfReferenceOnPort;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_RemoveDeviceInfoFromGlobalChildList;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_ReportDeviceAsMissing;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_ReportDeviceFailure;

DSM_OPERATION_SYNC_DISPATCH         HUBPDO_ReportNewDeviceToWdf;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_ResetEndpointAndTransferStateUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_ResetEndpointUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_SendMsOs20AltEnumCommandUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_SendUsbFeaturesVendorCmdUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_SetAttachStatusToFailure;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_SetAttachStatusToSuccess;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_SetConfigurationInformationInClientRequest;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_SetDeviceAddressUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_SetDeviceConfigurationUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_SetDeviceFlagToForceResetOnEnumeration;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_SetDeviceInterfaceUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_SetDeviceLostPowerFlag;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_SetDeviceNullConfigurationUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_SetDevicePDChargingPolicyUsingControlTransfer;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_SetDeviceVersionFlag;

DSM_OPERATION_SYNC_PASSIVE          HUBMISC_SetExtPropDescSemaphoreInRegistry;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_SetFailIo;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_SetInstallMSOSExtEventProcessed;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_SetInterfaceInformationInClientRequest;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_SetIsochDelayUsingControlTransfer;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_SetLatencyAdjustmentOnNoPingResponse;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_SetResetOnLastResumeFlag;

DSM_OPERATION_ASYNC_DISPATCH        HUBDTX_SetSelUsingControlTransfer;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_SetSpeedFlagFor20TypeDevice;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_SetSpeedFlagFor30TypeDevice;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_SetSystemWakeSource;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_ShouldDeviceBeArmedForWake;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_SignalPnpPowerEvent;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_SignalPnpPowerFailureEvent;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_SignalQueryDeviceTextEvent;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_StartDeviceIoUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartDuplicateDeviceTimer;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartEnumRetryTimer;

DSM_OPERATION_ASYNC_DISPATCH        HUBPDO_StartNoDriverFoundTimer;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartPostAddressTimer;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartPostResetExtendedTimer;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartPostResetTimerFor20;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartPostResetTimerFor30;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_StopTimer;

DSM_OPERATION_ASYNC_PASSIVE         HUBMISC_StoreDeviceMSOSVendorCodeInRegsitry;

DSM_OPERATION_ASYNC_DISPATCH        HUBMUX_UnregisterWithHSM;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_UpdateDefaultEndpointUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_UpdateDeviceExitLatencyUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBUCX_UpdateDeviceInfoUsingUCXIoctl;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_UpdateDeviceU1StatusToDisabled;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_UpdateDeviceU1StatusToEnabled;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_UpdateDeviceU2StatusToDisabled;

DSM_OPERATION_ASYNC_DISPATCH        HUBMISC_UpdateDeviceU2StatusToEnabled;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateAndCacheAlternateModeStringDescriptor;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateAndCacheBillboardStringDescriptor;

DSM_OPERATION_SYNC_PASSIVE          HUBDTX_ValidateAndCacheBOSDescriptor;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateAndCacheBOSDescriptorHeader;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateAndCacheConfigDescriptor;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateAndCacheDeviceDescriptor;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateAndCacheDeviceQualifierDescriptor;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateAndCacheLanguageIdStringDescriptor;

DSM_OPERATION_SYNC_PASSIVE          HUBDTX_ValidateAndCacheMSOS20DescriptorSet;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateAndCacheMSOSContainerIdDescriptor;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateAndCacheMSOSContainerIdDescriptorHeader;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateAndCacheMSOSDescriptor;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateAndCacheMSOSExtendedConfigDescriptor;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateAndCacheMSOSExtendedConfigDescriptorHeader;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_ValidateAndCacheMSOSExtendedPropertyDescriptor;

DSM_OPERATION_SYNC_DISPATCH         HUBMISC_ValidateAndCacheMSOSExtendedPropertyDescriptorHeader;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateAndCacheProductIdStringDescriptor;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateAndCacheSerialNumberStringDescriptor;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateFirstDeviceDescriptorFor20;

DSM_OPERATION_SYNC_DISPATCH         HUBDTX_ValidateInterfaceStatusForRemoteWakeCapability;

DSM_OPERATION_SYNC_PASSIVE          HUBPDO_ValidateSelectConfigUrb;

DSM_OPERATION_SYNC_DISPATCH         HUBPDO_ValidateSelectInterfaceUrb;

