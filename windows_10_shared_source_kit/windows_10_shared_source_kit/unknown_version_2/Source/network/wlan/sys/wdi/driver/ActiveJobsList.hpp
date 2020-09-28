#pragma once


typedef struct _RESET_PORT_REQUEST_CONTEXT
{
    bool  IsResetPending;
    NDIS_PORT_NUMBER NdisPortNumber;
    IOperationCompletionCallback* pCompletionCallback;
} RESET_PORT_REQUEST_CONTEXT, *PRESET_PORT_REQUEST_CONTEXT;


//
// "Wafer thin" class to keep track of all parallel requests
//
class ActiveJobsList : IOperationCompletionCallback,
                       IEventQueueCallback
{
public:
    ActiveJobsList();

    NDIS_STATUS Initialize(
        _In_ EventQueue*             pEventQueue,
        _In_ DeviceCommandScheduler* pCommandScheduler );

    NDIS_STATUS StartAddNewJob(
        _In_ CJobBase*                     pNewJob,
        _In_ IOperationCompletionCallback* pCompletionCallback );

    NDIS_STATUS StartResetForPort(
        _In_ NDIS_PORT_NUMBER              NdisPortNumber,
        _In_ IOperationCompletionCallback* pCompletionCallback );

    BOOLEAN JobListEmpty()
    {
        return IsListEmpty( &m_JobList );
    }

public:
    //
    // IOperationCompletionCallback Implementation
    //
    // WFC_SENDER_OBJECT must be CJobBase*
    //
    // If this completion triggers another completion, this
    // WFC_SENDER_OBJECT will be forwarded to caller, unchanged.
    //
    void OnOperationCompleted(
        _In_ NDIS_STATUS        OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

public:
    //
    // IEventQueueCallback Implementation
    //
    void OnEventQueueCallback(
        _In_ WFC_EVENT_TYPE        Type,
        _In_opt_ NDIS_STATUS        Status,
        _In_opt_ WFC_SENDER_OBJECT SenderObject,
        _In_opt_ WFC_EVENT_PAYLOAD Payload );

protected:
    //
    // Event Queue Callbacks
    //
    void OnAddJobEventHandler(
        _In_ CJobBase* pNewJob );

    VOID CompleteResetForPort( 
        _In_ PRESET_PORT_REQUEST_CONTEXT   pRequestContext
        );

    void ProcessJobQueueForResetRequests(_In_ bool   CancelQueuedJobs);

    EventQueue* m_pEventQueue;
    DeviceCommandScheduler* m_pCommandScheduler;
    LIST_ENTRY              m_JobList;

    RESET_PORT_REQUEST_CONTEXT  m_ResetPortContext[WFC_CONSTANT::MaximumNumberOfPorts];
};
