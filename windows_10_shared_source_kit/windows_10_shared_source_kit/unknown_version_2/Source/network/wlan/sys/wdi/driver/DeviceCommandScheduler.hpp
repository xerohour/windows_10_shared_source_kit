#pragma once

typedef enum _DEVICE_COMMAND_SCHEDULER_STATE
{
    DeviceCommandSchedulerStateInit,
    DeviceCommandSchedulerStateSendingCommand,
    DeviceCommandSchedulerStateWaitForCommandCompletion,
    DeviceCommandSchedulerStateCommandComplete
} DEVICE_COMMAND_SCHEDULER_STATE;

#define DEFAULT_COMMAND_TIMEOUT 10000 // 10 seconds
#define MIN_COMMAND_TIMEOUT 250 // 250 msec
#define MAX_COMMAND_TIMEOUT 30000 // 30 second
#define OPEN_COMMAND_TIMEOUT 10000 // 10 seconds
#define SET_POWER_COMMAND_TIMEOUT 10000 // 10 seconds

//
// These are types of FW Hang/RR trigger
//
#define COMMAND_TIMER_ELAPSED 1
#define TASK_TIMER_ELAPSED 2
#define RESET_RECOVERY_OID_REQUEST 3
#define RESET_RECOVERY_FIRMWARE_STALLED 4

class DeviceCommandScheduler : IEventQueueCallback,
                               IOperationCompletionCallback,
                               ITimerCallback
{
public:
    DeviceCommandScheduler();
    ~DeviceCommandScheduler();

    NDIS_STATUS Initialize(
        _In_ EventQueue* pEventQueue,
        _In_ CCtlPlane*   pCtlPlane,
        _In_ CAdapter*   pAdapter);

    //
    // Fails if there is a hard reset pending
    //
    NDIS_STATUS StartCommand(
        _In_ DeviceCommand*                pDeviceCommand,
        _In_ IOperationCompletionCallback* pCompletionCallback,
        _In_ CJobBase*                     pParentJob);

    void CancelTask(
        _In_ DeviceCommand* pPendingTaskDeviceCommand );

    CAdapter* GetAdapter();

    //NDIS_STATUS StartResetForPort(
    //_In_ WFC_PORT_ID  PortId,
    //_In_ IOperationCompletionCallback* pCompletionCallback );

    //
    // TODO: Maybe make a special completion interface for
    // resetforport and hardreset.
    // Need to decide either of the following or something else:
    //    Require "reinitialize" before we will accept new requests.
    //    Permanently dead-end this object once this function is called
    //
    //NDIS_STATUS DoHardReset(
    //_In_ IOperationCompletionCallback* pCompletionCallback );

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
    // ITimerCallback implementation
    //
    void OnTimerCallback(
        _In_opt_ WFC_TIMER_CONTEXT );

public:
    //
    // IOperationCompletionCallback implementation
    //
    void OnOperationCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    DeviceCommand * GetActiveCommand()
    {
        return m_pActiveDeviceCommand;
    }

protected:
    void OnQueueDeviceCommandEventHandler(
        _In_ DeviceCommand* pDeviceCommand );

    //
    // Send M1 if there is no active command already
    // Do nothing in hard reset mode
    //
    void OnIssueNextCommandHandler();
    void OnActiveCommandCompleteHandler(
        _In_ NDIS_STATUS status = NDIS_STATUS_SUCCESS,
        _In_ bool ReQueueCommand = false);

    //
    // Cancel all queued commands
    // Issue hard reset for active command (if there is one)
    // Set hard reset mode
    //
    //void OnDoHardResetHandler();

protected:
    //
    // Lower Layer Callback for M3 completion (not serialized through event queue)
    //
    void CompleteSendCommand_PostHangCheck(
        _In_ ULONG  TotalResponseLength,
        _In_reads_bytes_(TotalResponseLength) PWFC_MESSAGE_METADATA pResponse );

    void ProcessCtlPlaneSendCommandCompleteEvent(
        _In_ ULONG                           BufferLength,
        _In_reads_bytes_(BufferLength) PWFC_MESSAGE_METADATA Buffer,
        _In_ IWdiMessageMemoryManager* pBufferManager);

    void OnCtlPlaneWdiOidCompleteCallback(
        _In_ ULONG  TotalResponseLength,
        _In_reads_bytes_(TotalResponseLength) PWFC_MESSAGE_METADATA pResponse,
        _In_ IWdiMessageMemoryManager *pBufferManager );

    //
    // CallContext must be DeviceCommandScheduler*
    //
    static void s_CtlPlaneWdiOidCompleteCallback(
        _In_ PVOID  CallContext,
        _In_ ULONG  TotalResponseLength,
        _In_reads_bytes_(TotalResponseLength) PWFC_MESSAGE_METADATA pResponse, // Caller owns memory
        _In_ IWdiMessageMemoryManager *pBufferManager);

protected:
    //
    // Helpers
    //
    DeviceCommand* DequeueNextCommand();

    NDIS_STATUS SendCommandToLowerLayer(
        _In_ DeviceCommand* pDeviceCommand );

    void set_InternalState(
        _In_ DEVICE_COMMAND_SCHEDULER_STATE newState );

    //NDIS_STATUS FlushPendingDeviceCommandsForPort(
    //_In_ WFC_PORT_ID PortId );

    //NDIS_STATUS FlushDeviceCommandQueue();

    //
    // "Cancel IRP" to lower layer helper
    //
    //NDIS_STATUS HardResetActiveCommand();

protected:
    CCtlPlane*                  m_pControlPlane;
    EventQueue*                m_pEventQueue;
    DeviceCommand*            m_pActiveDeviceCommand;
    ULONG                          m_HungCommandId;
    WFC_COMMAND_TOKEN              m_HungCommandToken;
    ULONG                          m_pActiveDeviceCommandId;
    LIST_ENTRY                     m_DeviceCommandQueue;
    bool                           m_ResetForPortPending;
    WFC_PORT_ID                    m_PortResetId;
    IOperationCompletionCallback*  m_pResetForPortCompletionCallback;
    Event                          m_FailsafeLowerLayerCompletionEvent;
    DEVICE_COMMAND_SCHEDULER_STATE m_State;
    bool                           m_ShuttingDown;
    CAdapter*                      m_pAdapter;
    ULONG                          m_ulCommandTimeout;

///    ULONGLONG                      m_DebugDiagnosticsTriggerTime;
///    ULONG                          m_DebugDiagnosticsTriggerCommand;
    
    WFC_TIMER_REGISTRATION_TOKEN   m_TimerRegistrationToken;
    TimerRegistrationContext*      m_pTimerRegistrationContext; // for fast access
};
