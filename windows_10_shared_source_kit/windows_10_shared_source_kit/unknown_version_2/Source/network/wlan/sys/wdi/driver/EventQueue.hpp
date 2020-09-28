#pragma once

class EventQueue : public IEventQueueCallback,
                   public IWorkItemCallback
{
public:
    EventQueue();
    ~EventQueue();

    NDIS_STATUS Initialize(
        _In_ CGlobalCtlDevice* pCtlDevice );

    
    VOID WaitForQueueToEmpty();
    
    //
    // Special behavior: If the event queue goes from empty to
    // non-empty within the context of this function, control will not
    // be returned to the caller until the event queue is empty again.
    //
    NDIS_STATUS QueueEvent(
        _In_ Event* pEvent,
        _In_opt_ bool AtHead = false);    

    HANDLE get_ActiveThreadHandle();

public:
    //
    // Helpers for INotifyOperationStarted and IOperationCompletionCallback
    // fitting into the event queueing model.
    // 
    // This makes the EventQueue become a client to itself
    // for a events related to the interfaces mentioned above.
    //
    NDIS_STATUS DeferNotifyOperationStarted(
        _In_ INotifyOperationStarted* pOperationStarted,
        _In_ NDIS_STATUS OperationStatus = STATUS_SUCCESS,
        _In_opt_ WFC_SENDER_OBJECT SenderObject = NULL,
        _In_opt_ Event* pPreallocatedEvent = NULL );

    NDIS_STATUS DeferOperationCompletion(
        _In_ IOperationCompletionCallback* pCompletionCallback,
        _In_ NDIS_STATUS OperationStatus = STATUS_SUCCESS,
        _In_opt_ WFC_SENDER_OBJECT SenderObject = NULL,
        _In_opt_ Event* pPreallocatedEvent = NULL );

    NDIS_STATUS DeferTimeoutCallback(
        _In_ TimerRegistrationContext* pTimerRegistrationContext,
        _In_opt_ Event* pTargetEvent );

    NDIS_STATUS RegisterTimeoutCallback(
        _In_ WFC_ACTIVITY_ID ActivityId,
        _In_ ITimerCallback* pTimerCallback,
        _In_ ULONG RelativeTimeoutMs,
        _In_opt_ WFC_TIMER_CONTEXT TimerContext,
        _Out_ WFC_TIMER_REGISTRATION_TOKEN* pRegistrationToken );

    NDIS_STATUS RegisterTimeoutCallbackWithLevelAndReuse(
        _In_ WFC_ACTIVITY_ID ActivityId,
        _In_ ITimerCallback* pTimerCallback,
        _In_ ULONG RelativeTimeoutMs,
        _In_opt_ WFC_TIMER_CONTEXT TimerContext,
        _In_ WDF_EXECUTION_LEVEL level,
        _In_ bool bReuseTimer,
        _Out_ TimerRegistrationContext** ppRegistrationContext,
        _Out_ WFC_TIMER_REGISTRATION_TOKEN* pRegistrationToken );

    NDIS_STATUS DeregisterTimeoutCallback(
        _In_ WFC_TIMER_REGISTRATION_TOKEN RegistrationToken );

    NDIS_STATUS DeferToWorkItem(
        _In_ IOperationCompletionCallback* pCompletionCallback,
        _In_ Event* pEvent );

    NDIS_STATUS StartTimer(
        _In_ TimerRegistrationContext *pTimerContext,
        _In_ ULONG RelativeTimeoutInMs );

    void StopTimer(
        _In_ TimerRegistrationContext *pTimerContext );
public:
    //
    // IEventQueueCallback implementation
    //
    void OnEventQueueCallback(
        _In_ WFC_EVENT_TYPE Type,
        _In_opt_ NDIS_STATUS Status,
        _In_opt_ WFC_SENDER_OBJECT SenderObject,
        _In_opt_ WFC_EVENT_PAYLOAD Payload );

public:
    //
    // IWorkItemCallback implementation
    //
    void OnWorkItemCallback(
        _In_opt_ WFC_WORK_ITEM_CONTEXT WorkItemContext );

    //
    // Control injection of artifical Firmware hangs.
    //
    // m_HungResetRecoveryIterations is a count down number. 
    // If it is 0, no Hang simulation. If it is > 0, simulation is active.
    // Each NIC start, the count down is decremented by 1 and persisted to reg.
    //
    // m_CountCommandsToLe is count of commands to LE for the NIC
    // When it reaches m_HungResetRecoveryIterations+m_HungAtWdiCommandPasses
    // simulate a firmware hang. 
    //
    // m_HungAtWdiCommandPasses is updated to store 
    // m_HungResetRecoveryIterations+m_HungAtWdiCommandPasses
    // so we use it to comare to m_CountCommandsToLe w/o adding at every check.
    //
    ULONG                       m_HungResetRecoveryIterations;
    ULONG                       m_HungAtWdiCommandPasses;
    ULONG                       m_CountCommandsToLe;

protected:
    void Cleanup();

    NDIS_STATUS PopulateAndQueueDeferredCallback(
        _In_ WFC_EVENT_TYPE EventType,
        _In_ WFC_EVENT_PAYLOAD pDeferredCallback,
        _In_ WFC_SENDER_OBJECT SenderObject,
        _In_ NDIS_STATUS OperationStatus,
        _In_opt_ Event* pTargetEvent );

    void ProcessEventQueueUntilEmpty();

    Event* DequeueEvent();

    void ProcessEvent(
        _In_ Event* event );

    void HandleTimerCallback(
        _In_ TimerRegistrationContext* pRegistrationContext );

    CNdisSpinLock*    m_pEventQueueLock;
    CGlobalCtlDevice* m_pCtlDevice;
    bool              m_IsBeingProcessed;

    bool              m_WorkItemQueued;
    IWorkItemContext* m_pWorkItem;

    LIST_ENTRY        m_EventQueue;
    HANDLE            m_EventQueueThreadHandle;
    LIST_ENTRY        m_TimerRegistrations;
};
