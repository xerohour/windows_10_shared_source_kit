
/*++
Copyright (c) Microsoft Corporation

Module Name:

HSMPublic_AutoGen.h

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/

#pragma once



//
// Event List For the State Machines
//
typedef enum _HSM_EVENT{
    HsmEventNull                                     = 0x000003e8,
    HsmEventContinueIfNoSubState                     = 0x000003e9 | EventPrioritySync,
    HsmEventTimerFired                               = 0x000003ea | EventPriorityNormal,
    HsmEventConfigurationFailure                     = 0x000007d4 | EventPriorityNormal,
    HsmEventConfigurationSuccess                     = 0x000007d8 | EventPriorityNormal,
    HsmEventDevicePowerReferencesAcquired            = 0x000007dc | EventPriorityNormal,
    HsmEventDevicePowerReferencesReleased            = 0x000007e0 | EventPriorityNormal,
    HsmEventDeviceStopAfterSuspendAcked              = 0x000007e4 | EventPriorityNormal,
    HsmEventFDOD0Entry                               = 0x000007d4 | EventPriorityLow,
    HsmEventFDOD0EntryFromSx                         = 0x000007d8 | EventPriorityLow,
    HsmEventFDOD0Exit                                = 0x000007dc | EventPriorityLow,
    HsmEventFDOD0ExitFinal                           = 0x000007e0 | EventPriorityLow,
    HsmEventFDODeviceAdd                             = 0x000007e4 | EventPriorityLow,
    HsmEventFDODeviceCleanup                         = 0x000007e8 | EventPriorityLow,
    HsmEventFDOPrepareHardware                       = 0x000007ec | EventPriorityLow,
    HsmEventFDOReleaseHardware                       = 0x000007f0 | EventPriorityLow,
    HsmEventHubConfigurationFailure                  = 0x000007d8 | EventPrioritySync,
    HsmEventHubConfigurationSuccess                  = 0x000007dc | EventPrioritySync,
    HsmEventHubError                                 = 0x000007e8 | EventPriorityNormal,
    HsmEventHubFatalError                            = 0x000007e0 | EventPrioritySync,
    HsmEventHubIgnoreError                           = 0x000007e4 | EventPrioritySync,
    HsmEventHubLocalPowerGood                        = 0x000007e8 | EventPrioritySync,
    HsmEventHubLocalPowerLost                        = 0x000007ec | EventPrioritySync,
    HsmEventHubOverCurrent                           = 0x000007f0 | EventPrioritySync,
    HsmEventHubOverCurrentCleared                    = 0x000007f4 | EventPrioritySync,
    HsmEventInterruptTransferFailure                 = 0x000007ec | EventPriorityNormal,
    HsmEventInterruptTransferSuccess                 = 0x000007f0 | EventPriorityNormal,
    HsmEventIoctlFailure                             = 0x000007f4 | EventPriorityNormal,
    HsmEventIoctlSuccess                             = 0x000007f8 | EventPriorityNormal,
    HsmEventNo                                       = 0x000007f8 | EventPrioritySync,
    HsmEventOperationFailure                         = 0x000007fc | EventPrioritySync,
    HsmEventOperationPending                         = 0x00000800 | EventPrioritySync,
    HsmEventOperationSuccess                         = 0x00000804 | EventPrioritySync,
    HsmEventPDORequestGetPortStatus                  = 0x000007f4 | EventPriorityLow,
    HsmEventPortEnableInterruptTransfer              = 0x000007fc | EventPriorityNormal,
    HsmEventPortInterruptReferencesReleased          = 0x00000800 | EventPriorityNormal,
    HsmEventPortPowerReferencesAcquired              = 0x00000804 | EventPriorityNormal,
    HsmEventPortPowerReferencesReleased              = 0x00000808 | EventPriorityNormal,
    HsmEventPortResetReferencesReleased              = 0x0000080c | EventPriorityNormal,
    HsmEventPrepareForUpStreamReset                  = 0x000007f8 | EventPriorityLow,
    HsmEventResetHub                                 = 0x000007fc | EventPriorityLow,
    HsmEventResetHubComplete                         = 0x00000810 | EventPriorityNormal,
    HsmEventResetHubFailed                           = 0x00000814 | EventPriorityNormal,
    HsmEventResetHubFailedOnRemoval                  = 0x00000818 | EventPriorityNormal,
    HsmEventResetInterruptPipeFailure                = 0x000007dc | EventPriorityCritical,
    HsmEventResetInterruptPipeSuccess                = 0x000007e0 | EventPriorityCritical,
    HsmEventResetRegistrationComplete                = 0x0000081c | EventPriorityNormal,
    HsmEventResetUnregistrationComplete              = 0x00000820 | EventPriorityNormal,
    HsmEventTransferFailure                          = 0x000007d4 | EventPriorityCritical,
    HsmEventTransferSuccess                          = 0x000007d8 | EventPriorityCritical,
    HsmEventUpstreamResetComplete                    = 0x00000828 | EventPriorityNormal,
    HsmEventYes                                      = 0x00000808 | EventPrioritySync,
} HSM_EVENT, *PHSM_EVENT;



//
// Definitions for State Machine Operations 
//
HSM_OPERATION_ASYNC_DISPATCH        HUBHTX_AckHubChangeUsingControlTransfer;

HSM_OPERATION_ASYNC_DISPATCH        HUBFDO_AcquireWdfPowerReference;

HSM_OPERATION_ASYNC_DISPATCH        HUBFDO_BugcheckSystemOnBootHubFailure;

HSM_OPERATION_ASYNC_DISPATCH        HUBHTX_CancelHubControlTransfer;

HSM_OPERATION_ASYNC_DISPATCH        HUBHTX_CancelInterruptTransfer;

HSM_OPERATION_SYNC_DISPATCH         HUBHTX_CheckAndSelectIfAnyHubChangeSet;

HSM_OPERATION_SYNC_DISPATCH         HUBMUX_CheckIfAllPSMInterruptReferencesAreReleased;

HSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfHubDepthIsZero;

HSM_OPERATION_SYNC_DISPATCH         HUBFDO_CheckIfHubIsInBootPath;

HSM_OPERATION_ASYNC_DISPATCH        HUBHTX_CompleteGetPortStatusWithNoSuchDevice;

HSM_OPERATION_ASYNC_DISPATCH        HUBHTX_CompleteGetPortStatusWithPortStatusResult;

HSM_OPERATION_SYNC_DISPATCH         HUBMUX_CreateChildPSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_EnableResetHub;

HSM_OPERATION_ASYNC_DISPATCH        HUBUCX_GetControllerInfo;

HSM_OPERATION_SYNC_DISPATCH         HUBHTX_GetErrorResponseEventOnBadHubPortStatus;

HSM_OPERATION_SYNC_DISPATCH         HUBHTX_GetErrorResponseEventOnBadStatusChange;

HSM_OPERATION_SYNC_DISPATCH         HUBHTX_GetErrorResponseEventOnPersistentOverCurrent;

HSM_OPERATION_SYNC_DISPATCH         HUBHTX_GetHubChangeEvent;

HSM_OPERATION_ASYNC_DISPATCH        HUBHTX_GetHubConfigDescriptorWithDefaultSizeUsingControlTransfer;

HSM_OPERATION_ASYNC_DISPATCH        HUBHTX_GetHubConfigDescriptorWithReturnedLengthUsingControlTransfer;

HSM_OPERATION_ASYNC_DISPATCH        HUBHTX_GetHubDescriptorUsingControlTransfer;

HSM_OPERATION_SYNC_DISPATCH         HUBHTX_GetHubLostChangeEvent;

HSM_OPERATION_ASYNC_DISPATCH        HUBHTX_GetHubStandardStatusUsingControlTransfer;

HSM_OPERATION_ASYNC_DISPATCH        HUBHTX_GetHubStatusUsingControlTransfer;

HSM_OPERATION_ASYNC_DISPATCH        HUBPARENT_GetInfoFromParentUsingParentIoctl;

HSM_OPERATION_ASYNC_DISPATCH        HUBHTX_GetRequestedPortStatusUsingControlTransfer;

HSM_OPERATION_ASYNC_DISPATCH        HUBUCX_GetRootHub20PortsInfoUsingUCXIoctl;

HSM_OPERATION_ASYNC_DISPATCH        HUBUCX_GetRootHub30PortsInfoUsingUCXIoctl;

HSM_OPERATION_ASYNC_DISPATCH        HUBUCX_GetRootHubInfoUsingUCXIoctl;

HSM_OPERATION_SYNC_DISPATCH         HUBMISC_IncrementAndCheckIfHubDescriptorRetryCountExceededMax;

HSM_OPERATION_SYNC_DISPATCH         HUBMISC_IncrementAndCheckIfInterruptPipeResetCountExceededMaximum;

HSM_OPERATION_SYNC_PASSIVE          HUBMISC_IncrementAndCheckIfRecoveryCountExceededLimit;

HSM_OPERATION_SYNC_DISPATCH         HUBMISC_IncrementAndCheckIfResetCountExceededMaximum;

HSM_OPERATION_ASYNC_DISPATCH        HUBMISC_InitializeHubDescriptorRetryCount;

HSM_OPERATION_ASYNC_DISPATCH        HUBMISC_InitializeHubResetCount;

HSM_OPERATION_SYNC_DISPATCH         HUBHTX_IsBytesReturnedLessThanTotalLengthForHubConfigDescriptor;

HSM_OPERATION_SYNC_DISPATCH         HUBHTX_IsHubChangeBitSet;

HSM_OPERATION_SYNC_DISPATCH         HUBHTX_IsOverCurrentBitCleared;

HSM_OPERATION_ASYNC_PASSIVE         HUBFDO_LogHubResetRecoveryWasInvoked;

HSM_OPERATION_ASYNC_DISPATCH        HUBFDO_LogHubWasResetOnResume;

HSM_OPERATION_SYNC_PASSIVE          HUBFDO_QueryHubErrataFlags;

HSM_OPERATION_SYNC_DISPATCH         HUBPARENT_QueryParentIfDeviceWasReset;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueFakeStatusChangeToAllPSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueuePrepareResetToAllDSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueResetToAllPSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueResumeInS0ToAllPSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueResumeInS0ToSuspendedDSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueResumeToAllPSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueResumeToSuspendedDSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueResumeWithResetToAllPSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueResumeWithResetToSuspendedDSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueStartToAllPSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueStartToStoppedDSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueStatusChangeToPSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueStopAfterSuspendToAllDSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueStopToAllDSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueStopToAllPSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueSurpriseRemovalToAllPSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueSuspendToAllDSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBMUX_QueueSuspendToAllPSMs;

HSM_OPERATION_ASYNC_DISPATCH        HUBFDO_ReleaseWdfPowerReference;

HSM_OPERATION_ASYNC_PASSIVE         HUBFDO_ReportDeviceFailureToPnp;

HSM_OPERATION_ASYNC_DISPATCH        HUBMISC_ResetCountForResetInterruptPipe;

HSM_OPERATION_ASYNC_DISPATCH        HUBPARENT_ResetHub;

HSM_OPERATION_ASYNC_DISPATCH        HUBPARENT_ResetInterruptPipe;

HSM_OPERATION_ASYNC_DISPATCH        HUBHTX_SendInterruptTransfer;

HSM_OPERATION_ASYNC_DISPATCH        HUBPARENT_SetHubConfiguration;

HSM_OPERATION_ASYNC_DISPATCH        HUBHTX_SetHubDepthUsingControlTransfer;

HSM_OPERATION_ASYNC_DISPATCH        HUBFDO_SignalPnpPowerEvent;

HSM_OPERATION_ASYNC_DISPATCH        HUBFDO_SignalPnpPowerFailureEvent;

HSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartHubDescriptorRetryTimer;

HSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartHubOverCurrentTimer;

HSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartResetRetryTimer;

HSM_OPERATION_SYNC_DISPATCH         HUBMISC_StopHubTimer;

HSM_OPERATION_ASYNC_DISPATCH        HUBUCX_UpdateHubInformationUsingUCXIoctl;

HSM_OPERATION_SYNC_DISPATCH         HUBHTX_ValidateAndCacheHubConfigDescriptor;

HSM_OPERATION_SYNC_DISPATCH         HUBHTX_ValidateAndParseDevicePowerStatus;

HSM_OPERATION_SYNC_DISPATCH         HUBHTX_ValidateAndParseHubDescriptor;

