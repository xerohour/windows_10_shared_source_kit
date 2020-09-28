#pragma once

typedef enum _WFC_TASK_STATE
{
    WiFiTaskStateInit,
    WiFiTaskStateStarting,
    WiFiTaskStatePending,
    WiFiTaskStateCompleted
} WFC_TASK_STATE;

#define MIN_TASK_TIMEOUT 10000 // 10 seconds
#define DEFAULT_TASK_TIMEOUT 30000 // 30 seconds
#define MAX_TASK_TIMEOUT 60000 // 60 seconds

class Task : IOperationCompletionCallback,
             IEventQueueCallback,
             INotifyDeviceIndicationCallback,
             IActivityId,
             ITimerCallback
{
public:
    //
    // Parent APIs
    //
    explicit Task(
        _In_ WFC_ACTIVITY_ID ActivityId );
    virtual ~Task();

    NDIS_STATUS Initialize(
        _In_ NotificationManager*          pNotificationManager,
        _In_ DeviceCommandScheduler*       pDeviceCommandScheduler,
        _In_ EventQueue*                   pEventQueue,
        _In_ ULONG                         RelativeTimeoutMs);

    NDIS_STATUS Initialize(
        _In_ NotificationManager*          pNotificationManager,
        _In_ DeviceCommandScheduler*       pDeviceCommandScheduler,
        _In_ EventQueue*                   pEventQueue);

    NDIS_STATUS StartTask(
        _In_ IOperationCompletionCallback* pCompletionCallback,
        _In_ CJobBase*                     pParentJob );

    NDIS_STATUS get_OutputBuffer(
        _Out_ ULONG *                                     pBufferLength,
        _Outptr_result_bytebuffer_(*pBufferLength) BYTE** pBuffer );

    void CancelTask();

    INotifyOperationStarted* EventTaskStarted;

    NDIS_STATUS Task::get_TaskDeviceCommand(
        _Out_ DeviceCommand** ppDeviceCommand );

public:
    //
    // IEventQueueCallback implementation
    //
    void OnEventQueueCallback(
        _In_ WFC_EVENT_TYPE        Type,
        _In_opt_ NDIS_STATUS        Status,
        _In_opt_ WFC_SENDER_OBJECT SenderObject,
        _In_opt_ WFC_EVENT_PAYLOAD Payload );

public:
    //
    // IOperationCompletionCallback Implementation
    // WFC_SENDER_OBJECT must be DeviceCommandScheduler*
    //
    void OnOperationCompleted(
        _In_ NDIS_STATUS        OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

public:
    //
    // OnDeviceIndicationArrived Implementation
    //
    void OnDeviceIndicationArrived(
        _In_ WDI_INDICATION_TYPE                 IndicationType,
        _In_ WFC_COMMAND_TOKEN                   CommandToken,
        _In_ WFC_PORT_ID                         PortId,
        _In_ ULONG                                EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

public:
    //
    // ITimerCallback Implementation
    //
    void OnTimerCallback(
        _In_opt_ WFC_TIMER_CONTEXT TimerContext );

protected:
    //
    // Send M1
    //
    void OnStartTaskHandler();

    //
    // M3 Received
    //
    void OnTaskStartedHandler(
        _In_ NDIS_STATUS CallStatus );

    //
    // M4 Received, M1 Send Failed, or Receive M3 Failed
    //
    void OnTaskCompletedHandler(
        _In_ NDIS_STATUS                              Status,
        _In_opt_ ULONG                                EventBufferLength = 0,
        _In_reads_bytes_opt_(EventBufferLength) BYTE* EventBuffer       = NULL );

protected:
    //
    // Helpers
    //
    NDIS_STATUS IssueCancelToCommandScheduler();
    NDIS_STATUS IssueDeviceCommandToCommandScheduler();

    NDIS_STATUS set_InternalState(
        _In_ WFC_TASK_STATE NewState );

    void Cleanup();

protected:
    WFC_TASK_STATE               m_State;
    bool                          m_IsTaskCancelled;
    DeviceCommand                 m_TaskDeviceCommand;
    EventQueue*                   m_pEventQueue;
    DeviceCommandScheduler*       m_pDeviceCommandScheduler;
    IOperationCompletionCallback* m_pTaskCompletionCallback;
    WFC_REGISTRATION_TOKEN       m_NotificationRegistrationToken;
    NotificationManager*          m_pNotificationManager;
    Event                         m_FailsafeStartTaskEvent;
    Event                         m_FailsafeStartedEvent;
    bool                          m_IsDeviceCommandPending;

    ULONG                         m_OutputBufferLength;
    BYTE*                         m_OutputBuffer;

    NDIS_STATUS                   m_CallStatus;

    // TODO: Watchdog timer for M4 after M3 received (is this necessary?)

    WFC_TIMER_REGISTRATION_TOKEN m_TimerRegistrationToken;
    TimerRegistrationContext*    m_pTimerRegistrationContext;
    bool                         m_bTimerStopped;
    ULONG                        m_RelativeTimeoutInMSec;
    CJobBase*                    m_pParentJob;    
};
