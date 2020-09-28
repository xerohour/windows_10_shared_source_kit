
/*++
Copyright (c) Microsoft Corporation

Module Name:

PSMPublic_AutoGen.h

Abstract:

     This header file contains State Machines for USBHUB.
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/

#pragma once



//
// Event List For the State Machines
//
typedef enum _PSM_EVENT{
    PsmEventNull                                     = 0x000003e8,
    PsmEventContinueIfNoSubState                     = 0x000003e9 | EventPrioritySync,
    PsmEventTimerFired                               = 0x000003ea | EventPriorityNormal,
    PsmEventDeviceDetached                           = 0x00000bbc | EventPriorityNormal,
    PsmEventDevicePrepareForHibernation              = 0x00000bbc | EventPriorityLow,
    PsmEventDeviceRequestCycle                       = 0x00000bc0 | EventPriorityLow,
    PsmEventDeviceRequestDisable                     = 0x00000bc4 | EventPriorityLow,
    PsmEventDeviceRequestDisableSuperSpeed           = 0x00000bc8 | EventPriorityLow,
    PsmEventDeviceRequestReset                       = 0x00000bcc | EventPriorityLow,
    PsmEventDeviceRequestResume                      = 0x00000bd0 | EventPriorityLow,
    PsmEventDeviceRequestSuspend                     = 0x00000bd4 | EventPriorityLow,
    PsmEventDeviceRequestWarmReset                   = 0x00000bd8 | EventPriorityLow,
    PsmEventDeviceSetU1Timeout                       = 0x00000bdc | EventPriorityLow,
    PsmEventDeviceSetU2Timeout                       = 0x00000be0 | EventPriorityLow,
    PsmEventHubReset                                 = 0x00000bc0 | EventPriorityNormal,
    PsmEventHubResume                                = 0x00000bc4 | EventPriorityNormal,
    PsmEventHubResumeInS0                            = 0x00000bc8 | EventPriorityNormal,
    PsmEventHubResumeWithReset                       = 0x00000bcc | EventPriorityNormal,
    PsmEventHubStart                                 = 0x00000bd0 | EventPriorityNormal,
    PsmEventHubStatusChange                          = 0x00000bd4 | EventPriorityNormal,
    PsmEventHubStop                                  = 0x00000bd8 | EventPriorityNormal,
    PsmEventHubSurpriseRemove                        = 0x00000bdc | EventPriorityNormal,
    PsmEventHubSuspend                               = 0x00000be0 | EventPriorityNormal,
    PsmEventNo                                       = 0x00000bbc | EventPrioritySync,
    PsmEventObjectCleanup                            = 0x00000be4 | EventPriorityNormal,
    PsmEventOperationFailure                         = 0x00000bc0 | EventPrioritySync,
    PsmEventOperationSuccess                         = 0x00000bc4 | EventPrioritySync,
    PsmEventPortConnectChange                        = 0x00000bc8 | EventPrioritySync,
    PsmEventPortCycleOnError                         = 0x00000bcc | EventPrioritySync,
    PsmEventPortDisableAndCycleOnError               = 0x00000bd0 | EventPrioritySync,
    PsmEventPortDisabled                             = 0x00000bd4 | EventPrioritySync,
    PsmEventPortEnabledOnConnectError                = 0x00000bd8 | EventPrioritySync,
    PsmEventPortEnabledWhileResetError               = 0x00000bdc | EventPrioritySync,
    PsmEventPortError                                = 0x00000be0 | EventPrioritySync,
    PsmEventPortErrorWithChange                      = 0x00000be4 | EventPrioritySync,
    PsmEventPortIgnoreError                          = 0x00000be8 | EventPrioritySync,
    PsmEventPortLinkStateError                       = 0x00000bec | EventPrioritySync,
    PsmEventPortNeedsReset                           = 0x00000bf0 | EventPrioritySync,
    PsmEventPortNoChange                             = 0x00000bf4 | EventPrioritySync,
    PsmEventPortOverCurrent                          = 0x00000bf8 | EventPrioritySync,
    PsmEventPortOverCurrentCleared                   = 0x00000bfc | EventPrioritySync,
    PsmEventPortResetComplete                        = 0x00000c00 | EventPrioritySync,
    PsmEventPortResetHubOnError                      = 0x00000c04 | EventPrioritySync,
    PsmEventPortResetInProgress                      = 0x00000c08 | EventPrioritySync,
    PsmEventPortResetPollingTimerFired               = 0x00000bbc | EventPriorityCritical,
    PsmEventPortResumed                              = 0x00000c0c | EventPrioritySync,
    PsmEventTransferFailure                          = 0x00000bc0 | EventPriorityCritical,
    PsmEventTransferSuccess                          = 0x00000bc4 | EventPriorityCritical,
    PsmEventUserInitiatedResetOnOverCurrent          = 0x00000bec | EventPriorityNormal,
    PsmEventYes                                      = 0x00000c10 | EventPrioritySync,
} PSM_EVENT, *PPSM_EVENT;



//
// Definitions for State Machine Operations 
//
PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_AckPortChangeUsingControlTransfer;

PSM_OPERATION_SYNC_DISPATCH         HUBMUX_AcquireInterruptReference;

PSM_OPERATION_ASYNC_DISPATCH        HUBMUX_AcquirePortPowerReference;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_AttachDevice;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_CancelControlTransfer;

PSM_OPERATION_SYNC_DISPATCH         HUBHTX_CheckAndSelectIfAny20PortChangeSet;

PSM_OPERATION_SYNC_DISPATCH         HUBHTX_CheckAndSelectIfAny30PortChangeSet;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckAndSelectIfConnectChangeSetOnResume;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfConnectedDeviceSupportsReAttach;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfDebounceTimeIsNeeded;

PSM_OPERATION_SYNC_DISPATCH         HUBHTX_CheckIfDisableSuperSpeedHackFlagIsSet;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfHubIsArmedForWake;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfOldDeviceIsStillAttached;

PSM_OPERATION_SYNC_DISPATCH         HUBHTX_CheckIfPowerOnStartHackIsPresent;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_CheckIfResetCompletionRequiresPolling;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_ClearCurrentPortStatus;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_CreateNewDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_DetachDevice;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_DisablePortUsingControlTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_DisableRemoteWakeOnPortUsingControlTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_EnableRemoteWakeOnPortUsingControlTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_FixPortStateOnStartResume;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_FlushUserInitiatedResetOnOverCurrent;

PSM_OPERATION_ASYNC_PASSIVE         HUBMISC_GenerateUserNotificationForPersistentOverCurrent;

PSM_OPERATION_SYNC_DISPATCH         HUBHTX_Get20PortChangeEvent;

PSM_OPERATION_SYNC_DISPATCH         HUBHTX_Get30PortChangeEvent;

PSM_OPERATION_SYNC_DISPATCH         HUBHTX_GetErrorResponseEventForInvalidPortStatusInPendingReset;

PSM_OPERATION_SYNC_DISPATCH         HUBHTX_GetErrorResponseEventFromPortStatus;

PSM_OPERATION_SYNC_DISPATCH         HUBHTX_GetGenericErrorResponseEventForInvalidPortStatus;

PSM_OPERATION_SYNC_DISPATCH         HUBHTX_GetPortLostChangeEvent;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_GetPortStatusUsingControlTransfer;

PSM_OPERATION_SYNC_DISPATCH         HUBHTX_IncrementAndCheckIfOverCurrentCountExceededMax;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_InitializePortChangeBits;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_Is30DevicePresent;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_IsConnectBitOne;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_IsConnectBitOneOnResume;

PSM_OPERATION_SYNC_DISPATCH         HUBHTX_IsCurrentPortEventResumed;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_IsD3ColdSupported;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_IsLinkStateU0;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_IsOvercurrentBitOne;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_IsPortPoweredOn;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_LogPortLinkStateError;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_NotifyDeviceRemovalIfDeviceSupportsReattach;

PSM_OPERATION_ASYNC_DISPATCH        HUBDSM_QueueDisabledToDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBDSM_QueueFailedToDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBDSM_QueueResetCompleteToDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBDSM_QueueResetFailedDuetoPendingHubSuspendToDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBDSM_QueueResetTimedOutToDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBDSM_QueueResumedToDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBDSM_QueueResumeFailedDuetoPendingHubSuspendToDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBDSM_QueueResumeTimedOutToDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBDSM_QueueStateDisabledToDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBDSM_QueueStateEnabledOnReconnectToDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBDSM_QueueStateEnabledToDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBDSM_QueueStateSuspendedToDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBDSM_QueueSuspendedToDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBDSM_QueueTimeoutUpdatedToDSM;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_ReAttachDevice;

PSM_OPERATION_ASYNC_PASSIVE         HUBMUX_ReEnableInterruptTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMUX_ReEnableInterruptTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMUX_ReleaseInterruptReference;

PSM_OPERATION_ASYNC_DISPATCH        HUBMUX_ReleasePortPowerReference;

PSM_OPERATION_ASYNC_DISPATCH        HUBMUX_ReleaseResetReference;

PSM_OPERATION_ASYNC_DISPATCH        HUBMUX_ResetHub;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_ResetPortUsingControlTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_Resume30PortUsingControlTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_ResumePortUsingControlTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_SetDeviceFlagToForceResetOnEnumeration;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_SetLinkStateToRxDetectUsingControlTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_SetLinkStateToSSDisabledUsingControlTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_SetPortAndSpeedFlagsFor30Device;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_SetPortFlagFor20TypeDevice;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_SetPortRemoteWakeMaskUsingControlTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_SettingPortPowerUsingControlTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_SetU1TimeoutUsingControlTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_SetU2TimeoutUsingControlTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartD3ColdReconnectTimer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartDebounceTimer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartOverCurrentTimer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartPortPowerOnTimer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartReconnectOnResumeTimer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartResetCompletionTimer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartResumeCompletionTimer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartResumeRecoveryTimer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartSuperSpeedDisableTimer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_StartTimerForResetCompletionPolling;

PSM_OPERATION_SYNC_DISPATCH         HUBMISC_StopPortTimer;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_Suspend30PortUsingControlTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_SuspendPortUsingControlTransfer;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_UpdateCurrentPortStatusAsDisabled;

PSM_OPERATION_ASYNC_DISPATCH        HUBMISC_UpdateCurrentPortStatusAsSuspended;

PSM_OPERATION_ASYNC_DISPATCH        HUBHTX_WarmResetPortUsingControlTransfer;

