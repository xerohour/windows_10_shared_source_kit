#include "precomp.hpp"
#include "ActiveJobsList.hpp"
#include "ActiveJobsList.tmh"

ActiveJobsList::ActiveJobsList() :
    m_pEventQueue(NULL),
    m_pCommandScheduler(NULL)
{
    RtlZeroMemory(m_ResetPortContext, sizeof(m_ResetPortContext));
    InitializeListHead(
        &m_JobList );

    for (ULONG i = 0; i < WFC_CONSTANT::MaximumNumberOfPorts; i++)
    {
        m_ResetPortContext[i].IsResetPending = false;
    }
}


NDIS_STATUS ActiveJobsList::Initialize(
        _In_ EventQueue*             pEventQueue,
        _In_ DeviceCommandScheduler* pCommandScheduler  )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceEntry();

    if ( pEventQueue == NULL ||
         pCommandScheduler == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    m_pEventQueue = pEventQueue;
    m_pCommandScheduler = pCommandScheduler;

exit:
    TraceExit(status);
    return status;
}


NDIS_STATUS ActiveJobsList::StartAddNewJob(
    _In_ CJobBase*                     pNewJob,
    _In_ IOperationCompletionCallback* pCompletionCallback )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    Event* pAddNewJobEvent = NULL;

    TraceEntry();

    if ( pNewJob == NULL ||
         pCompletionCallback == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    status = pNewJob->set_ActiveJobCompletionCallback(
        pCompletionCallback );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    status = pNewJob->get_FailsafeCompletionEvent(
        &pAddNewJobEvent );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    status = pAddNewJobEvent->PopulateEvent(
        WiFiEventAddActiveJob,
        (IEventQueueCallback*)this,
        pNewJob );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    status = m_pEventQueue->QueueEvent(
        pAddNewJobEvent );

    if ( status != STATUS_SUCCESS )
    {
        goto exit;
    }

exit:

    TraceExit(status);
    return status;
}

void ActiveJobsList::OnOperationCompleted(
    _In_ NDIS_STATUS        OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    NDIS_STATUS                   status = NDIS_STATUS_SUCCESS;
    CJobBase*                     pCompletedJob;
    Event*                        pSafeCompletionEvent;
    IOperationCompletionCallback* pActiveJobsCallback;

    NT_ASSERT(SenderObject != NULL);

    TraceEntry();

    if ( SenderObject == NULL )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    pCompletedJob = (CJobBase*)SenderObject;

    pCompletedJob->get_ActiveJobCompletionCallback(
        &pActiveJobsCallback );

    pCompletedJob->get_FailsafeCompletionEvent(
        &pSafeCompletionEvent );

    if ( pSafeCompletionEvent == NULL )
    {
        NT_ASSERT(FALSE);
        goto exit;
    }

    RemoveEntryList(
        pCompletedJob->get_ActiveJobListEntry() );
    InitializeListHead(
        pCompletedJob->get_ActiveJobListEntry() );

    status = m_pEventQueue->DeferOperationCompletion(
        pActiveJobsCallback,
        OperationStatus,
        pCompletedJob,
        pSafeCompletionEvent );

    ProcessJobQueueForResetRequests(false);

exit:
    TraceExit(status);
}

void ActiveJobsList::OnEventQueueCallback(
    _In_ WFC_EVENT_TYPE        Type,
    _In_opt_ NDIS_STATUS        Status,
    _In_opt_ WFC_SENDER_OBJECT SenderObject,
    _In_opt_ WFC_EVENT_PAYLOAD Payload )
{
    UNREFERENCED_PARAMETER(Status);
    UNREFERENCED_PARAMETER(SenderObject);

    switch(Type)
    {
    case WiFiEventAddActiveJob:

        NT_ASSERT(Payload != NULL);

        if ( Payload != NULL )
        {
            OnAddJobEventHandler(
                (CJobBase*)Payload );
        }

        break;

    default:

        NT_ASSERT(FALSE);

    };
}

void ActiveJobsList::OnAddJobEventHandler(
    _In_ CJobBase* pNewJob )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceEntry();

    if ( pNewJob == NULL )
    {
        NT_ASSERT(FALSE);
        return;
    }

    InsertTailList(
        &m_JobList,
        pNewJob->get_ActiveJobListEntry() );

    status = pNewJob->StartJob(
        (IOperationCompletionCallback*) this );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

exit:

    if ( status != NDIS_STATUS_SUCCESS )
    {
        OnOperationCompleted(
            status,
            pNewJob );
    }
        
    TraceExit(status);
}



NDIS_STATUS ActiveJobsList::StartResetForPort(
    _In_ NDIS_PORT_NUMBER              NdisPortNumber,
    _In_ IOperationCompletionCallback* pCompletionCallback )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_RESOURCES;
    for (ULONG i = 0; i < WFC_CONSTANT::MaximumNumberOfPorts; i++)
    {
        if (m_ResetPortContext[i].IsResetPending == false)
        {
            WFCInfo("Queueing Active Jobs reset for port %x", NdisPortNumber);
        
            m_ResetPortContext[i].IsResetPending = true;
            m_ResetPortContext[i].NdisPortNumber = NdisPortNumber;
            m_ResetPortContext[i].pCompletionCallback = pCompletionCallback;

            ndisStatus = NDIS_STATUS_PENDING;

            ProcessJobQueueForResetRequests(true);            
            break;
        }
        else
        {
            // Two people cannot be waiting for reset on the same port
            NT_ASSERT(m_ResetPortContext[i].NdisPortNumber != NdisPortNumber);
        }
    }

    return ndisStatus;
}

VOID ActiveJobsList::CompleteResetForPort( 
    _In_ PRESET_PORT_REQUEST_CONTEXT   pRequestContext
    )
{
    WFCInfo("Completing Active Jobs reset for port %x", pRequestContext->NdisPortNumber);

    m_pEventQueue->DeferOperationCompletion(
        pRequestContext->pCompletionCallback,
        NDIS_STATUS_SUCCESS,
        NULL,
        NULL);

    pRequestContext->IsResetPending = false;    
}

void ActiveJobsList::ProcessJobQueueForResetRequests(
    _In_ bool   CancelQueuedJobs
    )
{
    // Job types that can get reset
    WFC_JOB_TYPE ResettableJobTypes[] = {
        WiFiJobScanOidRequest,
        WiFiJobANQPQuery,
        WiFiJobAssociationParametersRequestResponder,
        WiFiJobRoam
        };
    ULONG  numPendingJobsOnPort = 0;
    
    for (ULONG i = 0; i < WFC_CONSTANT::MaximumNumberOfPorts; i++)
    {
        if (m_ResetPortContext[i].IsResetPending == true)
        {
            // Port has a pending reset request. Walk the jobs queue & determine if all 
            // the "resettable" jobs have been completed
            PLIST_ENTRY pCurrentEntry = m_JobList.Flink;

            numPendingJobsOnPort = 0;
            while ( pCurrentEntry != &m_JobList )
            {
                CJobBase* pCurrentCandidate = CJobBase::get_JobFromListEntry(
                    pCurrentEntry );

                if ( pCurrentCandidate->get_NdisPortNumber() == m_ResetPortContext[i].NdisPortNumber)
                {
                    // Check if this is one of the resettable jobs
                    for (ULONG j = 0; j < sizeof(ResettableJobTypes)/sizeof(WFC_JOB_TYPE); j++)
                    {
                        if (pCurrentCandidate->get_JobType() == ResettableJobTypes[j] )
                        {                            
                            // Resettable job that is still in the queue. Cannot finish reset yet
                            numPendingJobsOnPort++;

                            if (CancelQueuedJobs)
                            {
                                // Ask the job to be cancelled
                                WFCInfo("Cancelling queued active job [O:%p][A:%x]", pCurrentCandidate, pCurrentCandidate->get_ActivityId());                                
                                pCurrentCandidate->CancelJob();
                            }
                        }
                    }
                }
                pCurrentEntry = pCurrentEntry->Flink;                
            }

            if (numPendingJobsOnPort == 0)
            {
                CompleteResetForPort(&m_ResetPortContext[i]);
            }
        }
    }        
}
