#pragma once


class SerializedJobList : IOperationCompletionCallback,
                          IEventQueueCallback
{
public:
    //
    // Parent API Set
    //
    SerializedJobList();

    NDIS_STATUS Initialize(
        _In_ EventQueue*     pEventQueue,
        _In_ ActiveJobsList* pActiveJobsList );

    NDIS_STATUS QueueSerializedJob(
        _In_ CJobBase*                     pSerializedJob,
        _In_ IOperationCompletionCallback* pCompletionCallback );

    void set_AllowNewJobs(bool Allow)
    {
        m_AllowNewJobs = Allow;
    }

    // Flush jobs for specified port number. INVALID_NDIS_PORT_NUMBER is for
    // all ports/adapters. NDIS_DEFAULT_PORT_NUMBER may also delete adapter jobs
    void FlushQueuedJobs(
        _In_ NDIS_PORT_NUMBER NdisPortNumber
        );

    CJobBase* GetActiveSerializedJob()
    {
        return m_pActiveSerializedJob;
    }

    CJobBase* FindQueuedSerializedJob(
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _In_ WFC_JOB_TYPE     JobType,
        _In_ bool CheckPortNumber = true );

    void CancelJob(
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _In_ WFC_JOB_TYPE     JobType,
        _In_opt_ CJobBase*    pJob
        );
        
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
    //
    // WFC_SENDER_OBJECT must be CJobBase*
    //
    // If this completion triggers another completion, this
    // WFC_SENDER_OBJECT will be forwarded to caller, unchanged.
    //
    void OnOperationCompleted(
        _In_ NDIS_STATUS        OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    //
    // check if joblist empty
    //
    BOOLEAN JobListEmpty()
    {
        return IsListEmpty( &m_JobList );
    }

protected:
    //
    // Event Queue Callbacks
    //
    void AddJobToQueueHandler(
        _In_ CJobBase* Job );

    void CompleteQueuedJob(
        _In_ CJobBase* pAbortedJob,
        _In_ NDIS_STATUS CompletionStatus
        );

protected:
    //
    // Helpers and members
    //
    void            PreemptActiveJobIfNecessary(
        _In_ CJobBase* pBestCandidate );

    void            ActivateNextSerializedJob();

    CJobBase*       ChooseBestCandidate();

    bool                          m_ResetForPortPending;
    WFC_PORT_ID                  m_PortResetId;
    IOperationCompletionCallback* m_pResetForPortCompletionCallback;
    EventQueue*                   m_pEventQueue;
    ActiveJobsList*               m_pActiveJobsList;
    CJobBase*                     m_pActiveSerializedJob;
    LIST_ENTRY                    m_JobList;

    bool                          m_AllowNewJobs;
};
