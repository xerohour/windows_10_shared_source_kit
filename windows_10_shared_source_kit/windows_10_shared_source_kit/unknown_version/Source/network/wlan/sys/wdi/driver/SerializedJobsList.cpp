#include "precomp.hpp"
#include "SerializedJobsList.hpp"
#include "SerializedJobsList.tmh"

SerializedJobList::SerializedJobList() :
    m_ResetForPortPending(false),
    m_PortResetId(WDI_PORT_ID_ADAPTER),
    m_pResetForPortCompletionCallback(NULL),
    m_pEventQueue(NULL),
    m_pActiveJobsList(NULL),
    m_pActiveSerializedJob(NULL),
    m_AllowNewJobs(true)
{
    InitializeListHead(
        &m_JobList );
}


NDIS_STATUS SerializedJobList::Initialize(    
    _In_ EventQueue*     pEventQueue,
    _In_ ActiveJobsList* pActiveJobsList )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceEntry();

    if ( pEventQueue == NULL ||
         pActiveJobsList == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    m_pEventQueue = pEventQueue;
    m_pActiveJobsList = pActiveJobsList;

exit:
    TraceExit(status);
    return status;
}


NDIS_STATUS SerializedJobList::QueueSerializedJob(
    _In_ CJobBase*                     pSerializedJob,
    _In_ IOperationCompletionCallback* pCompletionCallback )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    Event* pQueueSerializedJobEvent = NULL;

    TraceEntry();

    if ( pSerializedJob == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( !m_AllowNewJobs )
    {
        WFCError("Cannot queue new serialized job, job queue is not accepting new jobs.");
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    status = pSerializedJob->set_SerializedJobCompletionCallback(
        pCompletionCallback );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    status = pSerializedJob->get_FailsafeCompletionEvent(
        &pQueueSerializedJobEvent );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    status = pQueueSerializedJobEvent->PopulateEvent(
        WiFiEventQueueSerializedJob,
        (IEventQueueCallback*)this,
        pSerializedJob );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    status = m_pEventQueue->QueueEvent(
        pQueueSerializedJobEvent );

    if ( status != STATUS_SUCCESS )
    {
        goto exit;
    }

exit:

    TraceExit(status);
    return status;
}


void SerializedJobList::FlushQueuedJobs(
    _In_ NDIS_PORT_NUMBER NdisPortNumber
    )
{
    PLIST_ENTRY pCurrentEntry = m_JobList.Flink;
    PLIST_ENTRY pNextEntry = NULL; 
    TraceEntry();

    //
    // Flush must be called from event queue thread to avoid
    // synchronization race conditions.
    //
    NT_ASSERT(
        CDebugMetadata::get_CurrentThreadHandle() ==
        m_pEventQueue->get_ActiveThreadHandle() );

    if ( IsListEmpty(&m_JobList) )
    {
        goto exit;
    }

    while ( pCurrentEntry != &m_JobList )
    {
        WFC_JOB_TYPE jobType = WiFiJobTypeInvalid;
        CJobBase* pCurrentJob = CJobBase::get_JobFromListEntry(
            pCurrentEntry );
        bool bFlushJob = true;

        pNextEntry = pCurrentEntry->Flink;        

        // If a filter is provided, use it
        if (NdisPortNumber != INVALID_NDIS_PORT_NUMBER)
        {
            // Check if this job is for the port number we want
            // to flush jobs for. This is slightly problematic if some
            // Adapter job tried to use NDIS_DEFAULT_PORT_NUMBER. However
            // a flush on that port is only called on port delete, which is
            // when we are halting, so its OK if we end up flushing the 
            // non-running adapter jobs
            if (pCurrentJob->get_NdisPortNumber() != NdisPortNumber)
            {
                // Not the port we want to be flushing for
                WFCTrace(
                    "[A:%x] Not flushing job whose port number %d is not the one we are flushing %d.",
                    pCurrentJob->get_ActivityId(),
                    pCurrentJob->get_NdisPortNumber(),                    
                    NdisPortNumber );
                
                bFlushJob = false;
            }
        }

        if (bFlushJob)
        {
            // Will flush this job
            
            jobType = pCurrentJob->get_JobType();

            //
            // TODO: Add some callback into the job to signal "you never
            // ran, do any necessary clean-up".
            //
            // Without this, it is expected that the job will do any
            // necessary cleanup when it doesn't run in its destructor, or
            // in the parent completion callback. Improved contract would
            // have a call into the job (optionally implemented) to signal
            // this condition.
            //

            WFCInfo(
                "[A:%x] Flushing job (Type: %u) from queue.",
                pCurrentJob->get_ActivityId(),
                jobType );

            // Abort queued job
            this->CompleteQueuedJob(pCurrentJob,
                NDIS_STATUS_REQUEST_ABORTED
                );
        }

        pCurrentEntry = pNextEntry;
    }

exit:
    TraceExit(NDIS_STATUS_SUCCESS);
}

void SerializedJobList::OnEventQueueCallback(
    _In_ WFC_EVENT_TYPE        Type,
    _In_opt_ NDIS_STATUS        Status,
    _In_opt_ WFC_SENDER_OBJECT SenderObject,
    _In_opt_ WFC_EVENT_PAYLOAD Payload )
{
    UNREFERENCED_PARAMETER(Status);
    UNREFERENCED_PARAMETER(SenderObject);

    switch(Type)
    {
    case WiFiEventQueueSerializedJob:

        NT_ASSERT(Payload != NULL);

        if ( Payload != NULL )
        {
            AddJobToQueueHandler(
                (CJobBase*)Payload );
        }

        break;

    default:

        NT_ASSERT(FALSE);

    };
}

void SerializedJobList::CompleteQueuedJob(
    _In_ CJobBase* pAbortedJob,
    _In_ NDIS_STATUS CompletionStatus
    )
{
    NDIS_STATUS                   status = NDIS_STATUS_SUCCESS;
    Event*                        pSafeCompletionEvent;
    IOperationCompletionCallback* pSerializedJobCallback;

    TraceEntry();

    pAbortedJob->get_SerializedJobCompletionCallback(
        &pSerializedJobCallback );

    pAbortedJob->get_FailsafeCompletionEvent(
        &pSafeCompletionEvent );

    // Remove it from the list
    RemoveEntryList(
        pAbortedJob->get_SerializedJobListEntry() );
    InitializeListHead(
        pAbortedJob->get_SerializedJobListEntry() );

    // Notify the job that its aborted so it know that it didnt even run
    pAbortedJob->OnAborted();

    if ( pSerializedJobCallback != NULL )
    {
        // Signal the job that it has completed
        status = m_pEventQueue->DeferOperationCompletion(
            pSerializedJobCallback,
            CompletionStatus,
            pAbortedJob,
            pSafeCompletionEvent );
    }
    else
    {
        WFCTrace("Not completing serialized job callback because there isn't one.");
    }

    TraceExit(status);
}

void SerializedJobList::OnOperationCompleted(
    _In_ NDIS_STATUS        OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    NDIS_STATUS                   status = NDIS_STATUS_SUCCESS;
    CJobBase*                     pCompletedJob;
    Event*                        pSafeCompletionEvent;
    IOperationCompletionCallback* pSerializedJobCallback;

    NT_ASSERT(SenderObject != NULL);

    TraceEntry();

    if ( SenderObject == NULL )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    pCompletedJob = (CJobBase*)SenderObject;

    pCompletedJob->get_SerializedJobCompletionCallback(
        &pSerializedJobCallback );

    pCompletedJob->get_FailsafeCompletionEvent(
        &pSafeCompletionEvent );

    RemoveEntryList(
        pCompletedJob->get_SerializedJobListEntry() );
    InitializeListHead(
        pCompletedJob->get_SerializedJobListEntry() );
    m_pActiveSerializedJob = NULL;

    if ( pSerializedJobCallback != NULL )
    {
        status = m_pEventQueue->DeferOperationCompletion(
            pSerializedJobCallback,
            OperationStatus,
            pCompletedJob,
            pSafeCompletionEvent );
    }
    else
    {
        WFCTrace("Not completing serialized job callback because there isn't one.");
    }

exit:

    ActivateNextSerializedJob();

    TraceExit(status);
}


void SerializedJobList::AddJobToQueueHandler(
    _In_ CJobBase* pNewJob )
{
    TraceEntry();

    if ( pNewJob == NULL )
    {
        goto exit;
    }

    if ( !m_AllowNewJobs )
    {
        WFCError("Cannot queue new serialized job, job queue is not accepting new jobs.");

        OnOperationCompleted(
            NDIS_STATUS_INVALID_STATE,
            pNewJob );

        goto exit;
    }

    InsertTailList(
        &m_JobList,
        pNewJob->get_SerializedJobListEntry() );

    ActivateNextSerializedJob();

exit:
    TraceExit(NDIS_STATUS_SUCCESS);
}

void SerializedJobList::CancelJob(
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _In_ WFC_JOB_TYPE     JobType,
    _In_opt_ CJobBase*    pJob
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CJobBase* currentJob;

    TraceEntry();

    currentJob = this->GetActiveSerializedJob();    
    if (currentJob && 
        (currentJob->get_NdisPortNumber() == NdisPortNumber) &&
        (currentJob->get_JobType() == JobType) &&
        ((pJob == NULL) || (pJob == currentJob)))
    {
        // Job is active in the queue. Cancel it
        WFCInfo("Cancelling active serialized job [O:%p][A:%x]", currentJob, currentJob->get_ActivityId());
        
        currentJob->CancelJob();
    }    
    else 
    {
        WFCInfo("Searching for job type %d, Port %x in serialized queue", JobType, NdisPortNumber);
        // Not active
        if (pJob == NULL)
        {        
            // Find matching job type & complete it
            currentJob = this->FindQueuedSerializedJob (
                NdisPortNumber,
                JobType);

            if (currentJob)
            {
                WFCInfo("Aborting job from serialized queue [O:%p][A:%x]", currentJob, currentJob->get_ActivityId());

                this->CompleteQueuedJob(currentJob,
                    NDIS_STATUS_REQUEST_ABORTED
                    );                
            }
        }
        else
        {
            WFCInfo("Aborting from serialized queue [O:%p][A:%x]", pJob, pJob->get_ActivityId());

            this->CompleteQueuedJob(pJob,
                NDIS_STATUS_REQUEST_ABORTED
                );
        }
    }
    TraceExit (ndisStatus);
}


void SerializedJobList::PreemptActiveJobIfNecessary(
    _In_ CJobBase* pBestCandidate )
{
    TraceEntry();

    if ( m_pActiveSerializedJob == NULL )
    {
        NT_ASSERT(FALSE);
        goto exit;
    }

    if ( !pBestCandidate->get_CanPreempt() ||
         !m_pActiveSerializedJob->get_IsPreemptable() )
    {
        goto exit;
    }

    if ( pBestCandidate->get_PriorityScope() == WiFiPriorityScopePort &&
         pBestCandidate->get_NdisPortNumber() != m_pActiveSerializedJob->get_NdisPortNumber() )
    {
        goto exit;
    }

    if ( pBestCandidate->get_JobPriority() >=
         m_pActiveSerializedJob->get_JobPriority() )
    {
        // Even if the priority of incoming job is lower, if current job supports deferral, 
        // request the job to defer if possible
        if (m_pActiveSerializedJob->get_IsDeferrable())
        {
            WFCInfo("Requesting active serialized job %d to defer itself early so that new job %d gets to run",
                m_pActiveSerializedJob->get_JobType(), pBestCandidate->get_JobType());
            m_pActiveSerializedJob->set_DeferHint(true);
        }
        goto exit;
    }

    WFCInfo("Cancelling existing task %d to schedule new task %d",
            m_pActiveSerializedJob->get_JobType(), pBestCandidate->get_JobType());
    m_pActiveSerializedJob->CancelJob();

exit:
    TraceExit(NDIS_STATUS_SUCCESS);
}


void SerializedJobList::ActivateNextSerializedJob()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CJobBase* pNextBestJob;

    TraceEntry();

    do
    {
        pNextBestJob = ChooseBestCandidate();

        if ( pNextBestJob == NULL )
        {
            goto exit;
        }

        if ( m_pActiveSerializedJob != NULL )
        {
            PreemptActiveJobIfNecessary(
                pNextBestJob );

            goto exit;
        }
        else
        {
            RemoveEntryList(
                pNextBestJob->get_SerializedJobListEntry() );
            InitializeListHead(
                pNextBestJob->get_SerializedJobListEntry() );

            m_pActiveSerializedJob = pNextBestJob;

            status = m_pActiveJobsList->StartAddNewJob(
                pNextBestJob,
                (IOperationCompletionCallback*)this );

            if ( status == NDIS_STATUS_SUCCESS )
            {
                goto exit;
            }

            if ( status != NDIS_STATUS_SUCCESS )
            {
                OnOperationCompleted(
                    status,
                    pNextBestJob );
            }
        }
    } while ( pNextBestJob != NULL );

exit:
    TraceExit(status);
}


CJobBase* SerializedJobList::ChooseBestCandidate()
{
    CJobBase* pBestPortScopeCandidate = NULL;
    CJobBase* pBestCandidate = NULL;
    CJobBase* pBestDeferrableCandidate = NULL;

    PLIST_ENTRY pCurrentEntry = m_JobList.Flink;

    if ( IsListEmpty(&m_JobList) )
    {
        goto exit;
    }

    while ( pCurrentEntry != &m_JobList )
    {
        CJobBase* pCurrentCandidate = CJobBase::get_JobFromListEntry(
            pCurrentEntry );

        //
        // TODO: Confirm if this is right - Port Scope is only
        // relevant in relation to the currently active serialized job
        // (if there is one).
        //
        if ( m_pActiveSerializedJob != NULL && 
             pCurrentCandidate->get_NdisPortNumber() == m_pActiveSerializedJob->get_NdisPortNumber() &&
             pCurrentCandidate->get_PriorityScope() == WiFiPriorityScopePort &&
             pCurrentCandidate->get_JobPriority() <= m_pActiveSerializedJob->get_JobPriority() )
        {
            pBestPortScopeCandidate = pCurrentCandidate;
        }

        // If there are non-deferrable jobs waiting in the queue, allow those to run before any deferrable job
        // even if they are lower priority than the deferrable job. Assumption is that the non-deferrable jobs 
        // won't take long to complete and the deferrable job may have been requeued to allow these jobs to run.
        if ( pCurrentCandidate->get_IsDeferrable() )
        {
            if ( pBestDeferrableCandidate == NULL ||
                 pCurrentCandidate->get_JobPriority() < pBestDeferrableCandidate->get_JobPriority() )
            {
                pBestDeferrableCandidate = pCurrentCandidate;
            }
        }
        else
        {
            if ( pBestCandidate == NULL ||
                 pCurrentCandidate->get_JobPriority() < pBestCandidate->get_JobPriority() )
            {
                pBestCandidate = pCurrentCandidate;
            }
        }

        pCurrentEntry = pCurrentEntry->Flink;
    }

    if ( pBestCandidate == NULL )
    {
        pBestCandidate = pBestDeferrableCandidate;
    }

    if ( pBestPortScopeCandidate != NULL &&
         pBestCandidate->get_JobPriority() <= pBestPortScopeCandidate->get_JobPriority() )
    {
        pBestCandidate = pBestPortScopeCandidate;
    }

exit:
    return pBestCandidate;
}

CJobBase* SerializedJobList::FindQueuedSerializedJob(
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _In_ WFC_JOB_TYPE     JobType,
    _In_ bool CheckPortNumber )
{
    CJobBase* pFindJob = NULL;

    PLIST_ENTRY pCurrentEntry = m_JobList.Flink;

    if ( IsListEmpty(&m_JobList) )
    {
        goto exit;
    }

    while ( pCurrentEntry != &m_JobList )
    {
        CJobBase* pCurrentCandidate = CJobBase::get_JobFromListEntry(
            pCurrentEntry );

        if ( pCurrentCandidate->get_JobType() == JobType &&
             ( !CheckPortNumber || pCurrentCandidate->get_NdisPortNumber() == NdisPortNumber ) )
        {
            pFindJob = pCurrentCandidate;
            goto exit;
        }

        pCurrentEntry = pCurrentEntry->Flink;
    }

exit:

    return pFindJob;
}
