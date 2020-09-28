eCheckingIfDeviceIsConnectedOnPortChangeInD3ColdOnResume },
        { PsmEventHubStop ,             Psm20StateWaitingForDebounceTimerToFlushOnHubStopOnResume },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDeviceRequestResumeOnD3Cold = {
    // State ETW Name
    Psm20StateEtwWaitingForDeviceRequestResumeOnD3Cold,
    // State Entry Function
    HUBPSM20_WaitingForDeviceRequestResumeOnD3Cold,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    Psm20SubSmWaitingForDeviceRequestResumeOnD3Cold,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResetComplete ,   Psm20StateCheckingIfDeviceIsConnectedOnPortChangeInD3Cold },
        { PsmEventPortConnectChange ,   Psm20StateCheckingIfDeviceIsConnectedOnPortChangeInD3Cold },
        { PsmEventPortResumed ,         Psm20StateCheckingIfDeviceIsConnectedOnPortChangeInD3Cold },
        { PsmEventPortDisabled ,        Psm20StateCheckingIfDeviceIsConnectedOnPortChangeInD3Cold },
        { PsmEventPortError ,           Psm20StateCheckingIfDeviceIsConnectedOnPortChangeInD3Cold },
        { PsmEventPortOverCurrent ,     Psm20StateReturningPortOverCurrentFromSuspended },
        { PsmEventPortOverCurrentCleared ,Psm20StateReturningPortOverCurrentClearedFromSuspended },
        { PsmEventDeviceRequestResume , Psm20StateQueueingDisabledToDSMOnReconnectFromD3Cold },
        { PsmEventDeviceRequestDisable ,Psm20StateQueueingDisabledToDSMOnReconnectFromD3Cold },
        { PsmEventHubStop ,             Psm20StateReturningHubStopFromSuspendedInD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDeviceToReconnectOnResumeFromD3Cold = {
    // State ETW Name
    Psm20StateEtwWaitingForDeviceToReconnectOnResumeFromD3Cold,
    // State Entry Function
    HUBPSM20_WaitingForDeviceToReconnectOnResumeFromD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    Psm20SubSmWaitingForDeviceToReconnectOnResumeFromD3Cold,
    // Event State Pairs for Transitions
    {
        { PsmEventPortResumed ,         Psm20StateCheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume },
        { PsmEventPortConnectChange ,   Psm20StateCheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume },
        { PsmEventPortError ,           Psm20StateCheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume },
        { PsmEventPortResetComplete ,   Psm20StateCheckingIfDeviceIsConnectedOnPortChangInD3ColdOnResume },
        { PsmEventPortOverCurrent ,     Psm20StateStoppingReconnectTimerOnOvercurrentInD3Cold },
        { PsmEventPortOverCurrentCleared ,Psm20StateStoppingReconnectTimerOnOvercurrentClearedInD3Cold },
        { PsmEventPortDisabled ,        Psm20StateStoppingReconnectTimerOnDeviceReAttachAfterD3Cold },
        { PsmEventHubStop ,             Psm20StateStoppingReconnectTimerOnHubStopInD3Cold },
        { PsmEventTimerFired ,          Psm20StateReturningPortConnectChangeFromSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForPortChangeEventInD3Cold = {
    // State ETW Name
    Psm20StateEtwWaitingForPortChangeEventInD3Cold,
    // State Entry Function
    HUBPSM20_WaitingForPortChangeEventInD3Cold,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    Psm20SubSmWaitingForPortChangeEventInD3Cold,
    // Event State Pairs for Transitions
    {
        { PsmEventPortDisabled ,        Psm20StateCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold },
        { PsmEventPortResetComplete ,   Psm20StateCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold },
        { PsmEventPortConnectChange ,   Psm20StateCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold },
        { PsmEventPortError ,           Psm20StateCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold },
        { PsmEventPortResumed ,         Psm20StateCheckingIfDeviceIsConnectedOnConnectChangeForD3Cold },
        { PsmEventPortOverCurrent ,     Psm20StateReturningPortOverCurrentFromSuspended },
        { PsmEventPortOverCurrentCleared ,Psm20StateReturningPortOverCurrentClearedFromSuspended },
        { PsmEventDeviceRequestResume , Psm20StateStartingTimerForAllowingReconnectOnResumingFromD3Cold },
        { PsmEventHubSuspend ,          Psm20StateReleasingPowerReferenceOnHubS0IdleInD3Cold },
        { PsmEventHubStop ,             Psm20StateReturningHubStopFromSuspendedInD3Cold },
        { PsmEventDeviceRequestDisable ,Psm20StateStartingTimerForAllowingReconnectOnResumingFromD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForPortChangeEventInSuspended = {
    // State ETW Name
    Psm20StateEtwWaitingForPortChangeEventInSuspended,
    // State Entry Function
    HUBPSM20_WaitingForPortChangeEventInSuspended,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStatusChange ,     Psm20StateInitializingCumulativePortChangeBitsInSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForReconnectTimerToFlushOnHubStopInD3Cold = {
    // State ETW Name
    Psm20StateEtwWaitingForReconnectTimerToFlushOnHubStopInD3Cold,
    // State Entry Function
    HUBPSM20_WaitingForReconnectTimerToFlushOnHubStopInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTimerFired ,          Psm20StateReturningHubStopFromSuspendedInD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold = {
    // State ETW Name
    Psm20StateEtwWaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold,
    // State Entry Function
    HUBPSM20_WaitingForReconnectTimerToFlushOnOvercurrentClearedInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStop ,             Psm20StateWaitingForReconnectTimerToFlushOnHubStopInD3Cold },
        { PsmEventTimerFired ,          Psm20StateReturningPortOverCurrentClearedFromSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForReconnectTimerToFlushOnOvercurrentInD3Cold = {
    // State ETW Name
    Psm20StateEtwWaitingForReconnectTimerToFlushOnOvercurrentInD3Cold,
    // State Entry Function
    HUBPSM20_WaitingForReconnectTimerToFlushOnOvercurrentInD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventHubStop ,             Psm20StateWaitingForReconnectTimerToFlushOnHubStopInD3Cold },
        { PsmEventTimerFired ,          Psm20StateReturningPortOverCurrentFromSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForReconnectTimerToFlushOnReattachAfterD3Cold = {
    // State ETW Name
    Psm20StateEtwWaitingForReconnectTimerToFlushOnReattachAfterD3Cold,
    // State Entry Function
    HUBPSM20_WaitingForReconnectTimerToFlushOnReattachAfterD3Cold,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTimerFired ,          Psm20StateStartingDebounceTimerOnResumeFromD3ColdOnResume },
        { PsmEventHubStop ,             Psm20StateWaitingForReconnectTimerToFlushOnHubStopInD3Cold },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForResumeRecoveryTimerOnResumeInSuspended = {
    // State ETW Name
    Psm20StateEtwWaitingForResumeRecoveryTimerOnResumeInSuspended,
    // State Entry Function
    HUBPSM20_WaitingForResumeRecoveryTimerOnResumeInSuspended,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTimerFired ,          Psm20StateQueueingResumeToDSMFromSuspended },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAckingPortChangeInInWaitingForStopSuspend = {
    // State ETW Name
    Psm20StateEtwAckingPortChangeInInWaitingForStopSuspend,
    // State Entry Function
    HUBPSM20_AckingPortChangeInInWaitingForStopSuspend,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferSuccess ,     Psm20StateQueueingEnableInterruptTransferInWaitingForStopSuspend },
        { PsmEventTransferFailure ,     Psm20StateQueueingEnableInterruptTransferInWaitingForStopSuspend },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckIfThereIsAPortChangeInInWaitingForStopSuspend = {
    // State ETW Name
    Psm20StateEtwCheckIfThereIsAPortChangeInInWaitingForStopSuspend,
    // State Entry Function
    HUBPSM20_CheckIfThereIsAPortChangeInInWaitingForStopSuspend,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateAckingPortChangeInInWaitingForStopSuspend },
        { PsmEventPortError ,           Psm20StateQueueingEnableInterruptTransferInWaitingForStopSuspend },
        { PsmEventNo ,                  Psm20StateQueueingEnableInterruptTransferInWaitingForStopSuspend },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryGettingPortStatusInWaitingForStopSuspend = {
    // State ETW Name
    Psm20StateEtwGettingPortStatusInWaitingForStopSuspend,
    // State Entry Function
    HUBPSM20_GettingPortStatusInWaitingForStopSuspend,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferSuccess ,     Psm20StateCheckIfThereIsAPortChangeInInWaitingForStopSuspend },
        { PsmEventTransferFailure ,     Psm20StateQueueingEnableInterruptTransferInWaitingForStopSuspend },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingEnableInterruptTransferInWaitingForStopSuspend = {
    // State ETW Name
    Psm20StateEtwQueueingEnableInterruptTransferInWaitingForStopSuspend,
    // State Entry Function
    HUBPSM20_QueueingEnableInterruptTransferInWaitingForStopSuspend,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForDevicePortEvents },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingPortEventFailureToDSM = {
    // State ETW Name
    Psm20StateEtwQueueingPortEventFailureToDSM,
    // State Entry Function
    HUBPSM20_QueueingPortEventFailureToDSM,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForDevicePortEvents },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDevicePortEvents = {
    // State ETW Name
    Psm20StateEtwWaitingForDevicePortEvents,
    // State Entry Function
    HUBPSM20_WaitingForDevicePortEvents,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceRequestCycle ,  Psm20StateIgnored },
        { PsmEventDeviceRequestDisable ,Psm20StateQueueingPortEventFailureToDSM },
        { PsmEventHubStatusChange ,     Psm20StateGettingPortStatusInWaitingForStopSuspend },
        { PsmEventDeviceRequestSuspend ,Psm20StateQueueingPortEventFailureToDSM },
        { PsmEventDevicePrepareForHibernation ,Psm20StateQueueingPortEventFailureToDSM },
        { PsmEventDeviceRequestReset ,  Psm20StateQueueingPortEventFailureToDSM },
        { PsmEventDeviceRequestResume , Psm20StateQueueingPortEventFailureToDSM },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryAckingPortChangeInInWaitingForStopSuspendInDisconnected = {
    // State ETW Name
    Psm20StateEtwAckingPortChangeInInWaitingForStopSuspendInDisconnected,
    // State Entry Function
    HUBPSM20_AckingPortChangeInInWaitingForStopSuspendInDisconnected,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferSuccess ,     Psm20StateQueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected },
        { PsmEventTransferFailure ,     Psm20StateQueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryCheckIfThereIsAPortChangeInInWaitingForStopSuspendInDisconnected = {
    // State ETW Name
    Psm20StateEtwCheckIfThereIsAPortChangeInInWaitingForStopSuspendInDisconnected,
    // State Entry Function
    HUBPSM20_CheckIfThereIsAPortChangeInInWaitingForStopSuspendInDisconnected,
    // State Flags
    StateFlagNone,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventYes ,                 Psm20StateAckingPortChangeInInWaitingForStopSuspendInDisconnected },
        { PsmEventPortError ,           Psm20StateQueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected },
        { PsmEventNo ,                  Psm20StateQueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryGettingPortStatusInWaitingForStopSuspendInDisconnected = {
    // State ETW Name
    Psm20StateEtwGettingPortStatusInWaitingForStopSuspendInDisconnected,
    // State Entry Function
    HUBPSM20_GettingPortStatusInWaitingForStopSuspendInDisconnected,
    // State Flags
    StateFlagHandlesCriticalEventsOnly,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventTransferSuccess ,     Psm20StateCheckIfThereIsAPortChangeInInWaitingForStopSuspendInDisconnected },
        { PsmEventTransferFailure ,     Psm20StateQueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryQueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected = {
    // State ETW Name
    Psm20StateEtwQueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected,
    // State Entry Function
    HUBPSM20_QueueingEnableInterruptTransferInWaitingForStopSuspendInDisconnected,
    // State Flags
    StateFlagRequiresPassive,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventOperationSuccess ,    Psm20StateWaitingForDevicePortEventsInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};



STATE_ENTRY   Psm20StateEntryWaitingForDevicePortEventsInDisconnected = {
    // State ETW Name
    Psm20StateEtwWaitingForDevicePortEventsInDisconnected,
    // State Entry Function
    HUBPSM20_WaitingForDevicePortEventsInDisconnected,
    // State Flags
    StateFlagPopIfParentStateChanges,
    // Sub State Machine Information
    NULL,
    // Event State Pairs for Transitions
    {
        { PsmEventDeviceRequestSuspend ,Psm20StateIgnored },
        { PsmEventDeviceRequestReset ,  Psm20StateIgnored },
        { PsmEventDeviceRequestResume , Psm20StateIgnored },
        { PsmEventDeviceRequestDisable ,Psm20StateIgnored },
        { PsmEventDeviceDetached ,      Psm20StateIgnored },
        { PsmEventDeviceRequestCycle ,  Psm20StateIgnored },
        { PsmEventDevicePrepareForHibernation ,Psm20StateIgnored },
        { PsmEventHubStatusChange ,     Psm20StateGettingPortStatusInWaitingForStopSuspendInDisconnected },
        { PsmEventNull ,                Psm20StateNull },
    }
};






//
// Global List of States in all the State Machines
//
PSTATE_ENTRY    PSM20StateTable[] = {
    &Psm20StateEntryPoweredOffDisconnected,
    &Psm20StateEntryAcquiringInterruptReferenceOnHSMInDisabled,
    &Psm20StateEntryAcquiringInterruptReferenceOnHSMInEnabled,
    &Psm20StateEntryAcquiringInterruptReferenceOnResuming,
    &Psm20StateEntryAcquiringPowerReferenceFromPoweredOffDisabledInS0,
    &Psm20StateEntryAcquiringPowerReferenceFromPoweredOffDisconnectedInS0,
    &Psm20StateEntryAcquiringPowerReferenceFromPoweredOffSuspendedInS0,
    &Psm20StateEntryCancellingDisableOnHubStopSuspend,
    &Psm20StateEntryCancellingDisablingPortOnPortCycleOnHubStop,
    &Psm20StateEntryCancellingDisablingPortOnPortCycleWithTimerOnHubStop,
    &Psm20StateEntryCancellingQueryPortStatus,
    &Psm20StateEntryCancellingResetOnSurpriseRemove,
    &Psm20StateEntryCancellingSetPortPower,
    &Psm20StateEntryCheckingIfConnectBitOne,
    &Psm20StateEntryCheckingIfOvercurrentBitOne,
    &Psm20StateEntryCheckingIfPersistentOvercurrent,
    &Psm20StateEntryConnectedDisabled,
    &Psm20StateEntryConnectedEnabled,
    &Psm20StateEntryCreatingDevice,
    &Psm20StateEntryDetachingDeviceFromPortOnCycleOnHubStop,
    &Psm20StateEntryDetachingDeviceFromPortOnDisconnect,
    &Psm20StateEntryDetachingDeviceFromPortOnDisconnectWithTimer,
    &Psm20StateEntryDetachingDeviceFromPortOnOverCurrent,
    &Psm20StateEntryDetachingDeviceFromPortOnOverCurrentClear,
    &Psm20StateEntryDetachingDeviceFromPortOnOverCurrentClearWithTimer,
    &Psm20StateEntryDetachingDeviceFromPortOnOverCurrentWithTimer,
    &Psm20StateEntryDetachingDeviceFromPortOnPortCycle,
    &Psm20StateEntryDetachingDeviceOnCleanup,
    &Psm20StateEntryDetachingDeviceOnDisableAndCycle,
    &Psm20StateEntryDetachingDeviceOnHubReset,
    &Psm20StateEntryDetachingDeviceOnPortCycleWithTimer,
    &Psm20StateEntryDetachingDeviceOnPortDisableAndCycleWithTimer,
    &Psm20StateEntryDisablingOnHubSuspendWithTimer,
    &Psm20StateEntryDisablingPortBeforeConnecting,
    &Psm20StateEntryDisablingPortOnCycle,
    &Psm20StateEntryDisablingPortOnDeviceRequest,
    &Psm20StateEntryDisablingPortOnHubSuspend,
    &Psm20StateEntryDisablingPortOnPortCycleWithTimer,
    &Psm20StateEntryDisablingPortOnTimeOut,
    &Psm20StateEntryDisconnected,
    &Psm20StateEntryErrorOnAcquringReferenceOnHubResume,
    &Psm20StateEntryErrorOnConnectedDisabled,
    &Psm20StateEntryErrorOnConnectedEnabled,
    &Psm20StateEntryErrorOnDisconnected,
    &Psm20StateEntryErrorOnIssuingResetComplete,
    &Psm20StateEntryErrorOnResettingEnabled,
    &Psm20StateEntryErrorOnResettingPort,
    &Psm20StateEntryErrorOnStartingAndAcquiringReferenceOnHubResumeFromDisabled,
    