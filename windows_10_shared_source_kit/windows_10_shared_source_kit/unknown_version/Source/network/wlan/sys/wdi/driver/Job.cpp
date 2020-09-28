#include "precomp.hpp"
#include "Job.hpp"
#include "Job.tmh"

bool CJobBase::get_IsPreemptable()
{
    return m_IsPreemptable;
}

    
bool CJobBase::get_CanPreempt()
{
    return m_CanPreempt;
}

    
WFC_SERIALIZED_JOB_PRIORITY_SCOPE CJobBase::get_PriorityScope()
{
    return m_PriorityScope;
}

    
WFC_SERIALIZED_JOB_PRIORITY CJobBase::get_JobPriority()
{
    return m_JobPriority;
}

void CJobBase::set_JobPriority(
    WFC_SERIALIZED_JOB_PRIORITY NewPriority)
{
    m_JobPriority = NewPriority;
}

void CJobBase::set_IsPreemptible(
    _In_ bool  NewIsPreemptible)
{
    m_IsPreemptable = NewIsPreemptible;
}

WFC_PORT_ID CJobBase::get_PortId()
{
    return m_PortId;
}

NDIS_PORT_NUMBER CJobBase::get_NdisPortNumber()
{
    return m_NdisPortNumber;
}

bool CJobBase::get_WasJobStarted()
{
    return m_OnJobStartCalled;
}    

bool CJobBase::get_IsDeferrable()
{
    return m_IsDeferrable;
}

void CJobBase::set_DeferHint(
    bool DeferHint)
{
    m_DeferHint = DeferHint;
}

bool CJobBase::get_DeferHint()
{
    return m_DeferHint;
}

NDIS_STATUS CJobBase::s_CreateInstanceFromOIDBuffer(
    _In_ WFC_JOB_TYPE                 JobType,
    _In_ DWORD                         BufferSize,
    _In_reads_bytes_(BufferSize) BYTE* Buffer,
    _In_ IOperationCompletionCallback* pCompletionCallback,
    _Out_ CJobBase**                   ppJob )
{
    UNREFERENCED_PARAMETER(JobType);
    UNREFERENCED_PARAMETER(BufferSize);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(pCompletionCallback);
    UNREFERENCED_PARAMETER(ppJob);

    return NDIS_STATUS_FAILURE;
}

NDIS_STATUS CJobBase::Initialize(
    _In_ EventQueue*                        pEventQueue,
    _In_ DeviceCommandScheduler*            pCommandScheduler,
    _In_ ActiveJobsList*                    pActiveJobsList,
    _In_ SerializedJobList*                 pSerializedJobList,    
    _In_ WFC_JOB_TYPE                      JobType,
    _In_ bool                               IsPreemptable,
    _In_ bool                               CanPreempt,
    _In_ WFC_SERIALIZED_JOB_PRIORITY_SCOPE PriorityScope,
    _In_ WFC_SERIALIZED_JOB_PRIORITY       JobPriority,
    _In_ IPropertyCacheDirectory*       pPropertyCacheDirectory,
    _In_opt_ INdisConversion*               pNdisConversion,
    _In_opt_ NDIS_PORT_NUMBER               NdisPortNumber,
    _In_opt_ bool                           IsDeferrable )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( pEventQueue == NULL ||
         pCommandScheduler == NULL ||
         pActiveJobsList == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( pNdisConversion == NULL &&
         ( CanPreempt || IsPreemptable ) )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    m_pEventQueue = pEventQueue;
    m_pDeviceCommandScheduler = pCommandScheduler;
    m_pActiveJobsList = pActiveJobsList;
    m_pSerializedJobList = pSerializedJobList;
    m_IsPreemptable = IsPreemptable;
    m_CanPreempt = CanPreempt;
    m_PriorityScope = PriorityScope;
    m_JobPriority = JobPriority;
    m_JobType = JobType;
    m_pNdisConversion = pNdisConversion;
    m_NdisPortNumber = NdisPortNumber;
    m_pPropertyCacheDirectory = pPropertyCacheDirectory;
    m_IsDeferrable = IsDeferrable;

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CJobBase::StartJob(
    _In_ IOperationCompletionCallback* pCompletionCallback )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( pCompletionCallback == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( m_State != WiFiJobStateInit )
    {
        WFCActError("Job is in the wrong state to be started. %u", m_State);
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    m_pJobCompletionCallback = pCompletionCallback;

    status = m_FailsafeDeferJobStartEvent.PopulateEvent(
        WiFiEventStartJob,
        (IEventQueueCallback*)this );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Failed to populate defer job start event.");
        goto exit;
    }
    m_FailsafeDeferJobStartEvent.set_RequiresPassiveIrql(this->get_RequiresPassiveIrql());
    
    status = m_pEventQueue->QueueEvent(
        &m_FailsafeDeferJobStartEvent );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Failed to queue defer job start event.");
        goto exit;
    }

    m_State = WiFiJobStateStartOrContinue;    

exit:
    TraceActExit(status);
    return status;
}
    
void CJobBase::OnOperationCompleted(
    _In_ NDIS_STATUS        OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    void* pExpectedSender = NULL;
    bool knownCommandWasPending = m_KnownCommandPending;
    bool bCallback = false;

    if (this->get_RequiresPassiveIrql() && (KeGetCurrentIrql() > PASSIVE_LEVEL))
    {
        NDIS_STATUS deferStatus = NDIS_STATUS_SUCCESS;
        
        // Instead of actually doing this, queue an event to do this work
        deferStatus  = m_FailsafeDeferJobOperationCompletedEvent.PopulateEvent(
            WiFiEventJobOperationCompletedCallback,
            (IEventQueueCallback*)this,
            NULL,
            SenderObject,
            OperationStatus
            );

        if ( deferStatus  != NDIS_STATUS_SUCCESS )
        {
            WFCActError("Failed to populate deferred job step completed event. Error = 0x%08x", deferStatus);
        }
        else
        {
            m_FailsafeDeferJobOperationCompletedEvent.set_RequiresPassiveIrql(this->get_RequiresPassiveIrql());
            
            deferStatus  = m_pEventQueue->QueueEvent(
                &m_FailsafeDeferJobOperationCompletedEvent,
                true);

            if ( deferStatus  != NDIS_STATUS_SUCCESS )
            {
                WFCActError("Failed to queue deferred job step completed event.  Error = 0x%08x", deferStatus);
            }
        }

        if ( deferStatus == NDIS_STATUS_SUCCESS)
        {
            return;
        }
        else
        {
            // We need to move to the next step. We will call the operation at dispatch
            NT_ASSERT(FALSE);
            OperationStatus = deferStatus;
        }
    }

    NT_ASSERT(
        m_State == WiFiJobStateStepPending );

    if ( m_pDebugChildJob != NULL )
    {
        pExpectedSender = m_pDebugChildJob;
    }
    else if ( m_pDebugTaskCommand != NULL )
    {
        pExpectedSender = m_pDebugTaskCommand;
    }
    else if ( m_pDebugDeviceCommand != NULL )
    {
        pExpectedSender = m_pDebugDeviceCommand;
    }

    m_pDebugChildJob = NULL;
    m_pDebugTaskCommand = NULL;
    m_pDebugDeviceCommand = NULL;
    m_KnownCommandPending = false;
    m_State = WiFiJobStateStartOrContinue;

    if ( !knownCommandWasPending )
    {
        bCallback = true;
    }
    else if ( pExpectedSender != NULL &&
         SenderObject == pExpectedSender )
    {
        bCallback = true;
    }

    if ( bCallback )
    {
        OnJobStepCompleted(
            OperationStatus,
            SenderObject );
    }
    else
    {
        NT_ASSERT(FALSE);
    }
}


void CJobBase::OnEventQueueCallback(
    _In_ WFC_EVENT_TYPE        Type,
    _In_opt_ NDIS_STATUS        Status,
    _In_opt_ WFC_SENDER_OBJECT SenderObject,
    _In_opt_ WFC_EVENT_PAYLOAD Payload )
{
    UNREFERENCED_PARAMETER(Status);
    UNREFERENCED_PARAMETER(SenderObject);
    UNREFERENCED_PARAMETER(Payload);

    switch(Type)
    {
    case WiFiEventStartJob:

        OnStartJobEventHandler();

        break;

    case WiFiEventJobOperationCompletedCallback:
        OnOperationCompleted(
            Status,
            SenderObject
            );
        break;
        
    default:
        NT_ASSERT(FALSE);
    }
}


void CJobBase::OnStartJobEventHandler()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // The WiFiEventStartJob should have been queued to be require passive
    // So we dont need to do special work here
    NT_ASSERT(!this->get_RequiresPassiveIrql() || (KeGetCurrentIrql() == PASSIVE_LEVEL));

    if ( m_NdisPortNumber != INVALID_NDIS_PORT_NUMBER )
    {
        if ( m_pNdisConversion != NULL )
        {
            status = m_pNdisConversion->ConvertNdisPortNumberToPortId(
                m_NdisPortNumber,
                &m_PortId );

            if ( status != NDIS_STATUS_SUCCESS )
            {
                WFCActError("Couldn't convert NDIS Port number to Port ID.");
                goto exit;
            }

            if ( m_pPropertyCacheDirectory != NULL )
            {
                if ( m_pPropertyCacheDirectory->GetPortPropertyCache(m_PortId) == NULL )
                {
                    WFCActError("Couldn't obtain Port Property cache");
                    status = NDIS_STATUS_INVALID_PORT_STATE;
                    goto exit;
                }
            }

        }
    }

    if ( m_pNdisConversion == NULL || m_pNdisConversion->IsOperationalPowerState())
    {
        m_OnJobStartCalled = true;
        OnJobStarted();
    }
    else 
    {
        status = NDIS_STATUS_INVALID_STATE;
    }

exit:

    if ( status != NDIS_STATUS_SUCCESS )
    {
        CompleteJob(
            status );
    }

    TraceActExit(status);
}


void CJobBase::CancelJob()
{
    if (get_WasJobStarted())
    {
        OnJobCancelled();
    }
}

void CJobBase::OnAborted()
{
    WFCActInfo("JobType %u was aborted without starting. Default Implementation Called.", m_JobType);
}

CJobBase::CJobBase(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
    m_State(WiFiJobStateInit),
    m_JobType(WiFiJobTypeInvalid),
    m_OnJobStartCalled(false),
    EventJobStarted(NULL),
    m_pEventQueue(NULL),
    m_pDeviceCommandScheduler(NULL),
    m_pActiveJobsList(NULL),
    m_pDebugChildJob(NULL),
    m_pDebugTaskCommand(NULL),
    m_pDebugDeviceCommand(NULL),
    m_KnownCommandPending(false),
    m_pJobCompletionCallback(NULL),
    m_FailsafeJobStartedEvent(true, ActivityId),
    m_FailsafeJobCompletionEvent(true, ActivityId),
    m_FailsafeDeferJobStartEvent(true, ActivityId),
    m_FailsafeDeferJobOperationCompletedEvent(true, ActivityId),
    m_pActiveJobCompletionCallback(NULL),
    m_IsPreemptable(false),
    m_CanPreempt(false),
    m_RequiresPassiveIrql(false),
    m_PriorityScope(WiFiPriorityScopeAdapter),
    m_JobPriority(WiFiSerializedJobPriority_0),
    m_pSerializedJobCompletionCallback(NULL),
    m_PortId(WDI_PORT_ID_ADAPTER),
    m_pPropertyCacheDirectory(NULL),
    m_pNdisConversion(NULL),
    m_NdisPortNumber(INVALID_NDIS_PORT_NUMBER),
    IActivityId(ActivityId)
{
    InitializeCppListEntry(
        this,
        &m_ActiveJobLink );

    InitializeCppListEntry(
        this,
        &m_SerializedJobLink );
}


NDIS_STATUS CJobBase::StartChildJob(
    _In_ CJobBase* pChildJob )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( pChildJob == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( m_State != WiFiJobStateStartOrContinue )
    {
        WFCActError("Job not in a good state to start a child job. %u", m_State);
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    status = m_pActiveJobsList->StartAddNewJob(
        pChildJob,
        (IOperationCompletionCallback*)this);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't add job to active job's list.");
        goto exit;
    }

    NT_ASSERT(
        m_pDebugChildJob == NULL &&
        m_pDebugTaskCommand == NULL &&
        m_pDebugDeviceCommand == NULL );

    m_State = WiFiJobStateStepPending;
    m_pDebugChildJob = pChildJob;
    m_KnownCommandPending = true;

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CJobBase::QueueSerializedJob(
    _In_ CJobBase* pChildJob )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( pChildJob == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( m_State != WiFiJobStateStartOrContinue )
    {
        WFCActError("Job not in a good state to start a child job. %u", m_State);
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    status = m_pSerializedJobList->QueueSerializedJob(
        pChildJob,
        (IOperationCompletionCallback*)this );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't queue job in serialized job's list.");
        goto exit;
    }

    NT_ASSERT(
        m_pDebugChildJob == NULL &&
        m_pDebugTaskCommand == NULL &&
        m_pDebugDeviceCommand == NULL );

    m_State = WiFiJobStateStepPending;
    m_pDebugChildJob = pChildJob;
    m_KnownCommandPending = true;

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CJobBase::StartTask(
    _In_ Task* pTaskCommand )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( pTaskCommand == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( m_State != WiFiJobStateStartOrContinue )
    {
        WFCActError("Job is in bad state to start new task. %u", m_State);
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    status = pTaskCommand->StartTask(
        (IOperationCompletionCallback*)this,
        this);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Failed to start task command.");
        goto exit;
    }

    NT_ASSERT(
        m_pDebugChildJob == NULL &&
        m_pDebugTaskCommand == NULL &&
        m_pDebugDeviceCommand == NULL );

    m_State = WiFiJobStateStepPending;
    m_pDebugTaskCommand = pTaskCommand;
    m_KnownCommandPending = true;

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CJobBase::InitializeAndStartTask(
    _In_ Task* pTaskCommand,
    _In_ ULONG CommandId,
    _In_ ULONG MessageBufferLength,
    _In_reads_bytes_(MessageBufferLength) BYTE* pMessageBuffer,
    _In_ NotificationManager* pNotificationManager,
    _In_opt_ ULONG* pRelativeTimeoutInMSec )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    DeviceCommand* pDeviceCommand;

    status = pTaskCommand->get_TaskDeviceCommand(
        &pDeviceCommand );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't get device command from task object.");
        goto exit;
    }

    status = pDeviceCommand->Initialize(
        get_PortId(),
        CommandId,
        MessageBufferLength,
        pMessageBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't initialize task device command.");
        goto exit;
    }

    if ( pRelativeTimeoutInMSec == NULL )
    {
        status = pTaskCommand->Initialize(
            pNotificationManager,
            m_pDeviceCommandScheduler,
            m_pEventQueue );
    }
    else
    {
        status = pTaskCommand->Initialize(
            pNotificationManager,
            m_pDeviceCommandScheduler,
            m_pEventQueue,
            *pRelativeTimeoutInMSec );
    }

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't initialize task.");
        goto exit;
    }

    status = StartTask(pTaskCommand);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't start task.");
        goto exit;
    }

exit:
    return status;
}


NDIS_STATUS CJobBase::QueueDeviceCommand(
    _In_ DeviceCommand* pDeviceCommand )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( pDeviceCommand == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( m_State != WiFiJobStateStartOrContinue )
    {
        WFCActError("Job is in bad state to start new device command. %u", m_State);
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    status = m_pDeviceCommandScheduler->StartCommand(
        pDeviceCommand,
        (IOperationCompletionCallback*)this,
        this);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Failed to queue new device command.");
        goto exit;
    }

    NT_ASSERT(
        m_pDebugChildJob == NULL &&
        m_pDebugTaskCommand == NULL &&
        m_pDebugDeviceCommand == NULL );

    m_State = WiFiJobStateStepPending;
    m_pDebugDeviceCommand = pDeviceCommand;
    m_KnownCommandPending = true;

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CJobBase::SetPending()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( m_State != WiFiJobStateStartOrContinue )
    {
        WFCActError("Job is not in a good state to set pending. %u", m_State);
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    NT_ASSERT(
        m_pDebugChildJob == NULL &&
        m_pDebugTaskCommand == NULL &&
        m_pDebugDeviceCommand == NULL );

    m_State = WiFiJobStateStepPending;
    m_KnownCommandPending = false;

exit:
    TraceActExit(status);
    return status;
}

//
// Cancels pending child job, task command, or device command if
// this job is currently waiting for one to complete.
//
// Success if cancel was issued, or if there was nothing pending.
//
// Returns not supported if SetPending() was used instead of
// job/task/command helpers.
//
NDIS_STATUS CJobBase::CancelActivePendingCommand()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( m_State != WiFiJobStateStepPending )
    {
        WFCActError("Job is not pending. Cancel is unexpected. %u", m_State);
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    if ( !m_KnownCommandPending )
    {
        WFCActError("Called %!FUNC! for unknown command type.");
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_NOT_SUPPORTED;
        goto exit;
    }

    if ( m_pDebugChildJob != NULL )
    {
        NT_ASSERT(
            m_pDebugTaskCommand == NULL &&
            m_pDebugDeviceCommand == NULL );

        m_pDebugChildJob->OnJobCancelled();
    }
    else if ( m_pDebugTaskCommand != NULL )
    {
        NT_ASSERT(
            m_pDebugChildJob == NULL &&
            m_pDebugDeviceCommand == NULL );

        m_pDebugTaskCommand->CancelTask();
    }

exit:
    TraceActExit(status);
    return status;
}

void CJobBase::NotifyStarted(
    _In_ NDIS_STATUS ErrorStatus )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    if ( EventJobStarted != NULL )
    {
        WFCActTrace("Callback is registered, deferring operation started notification.");
        m_FailsafeDeferJobStartEvent.set_RequiresWorkerThread(true);
        status = m_pEventQueue->DeferNotifyOperationStarted(
            EventJobStarted,
            ErrorStatus,
            this,
            &m_FailsafeJobStartedEvent );
    }

    NT_ASSERT(status == NDIS_STATUS_SUCCESS);

    TraceActExit(status);
}

NDIS_STATUS CJobBase::CompleteJob(
    _In_ NDIS_STATUS ErrorStatus )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( m_State != WiFiJobStateStartOrContinue )
    {
        WFCActError("Job is in a bad state to get completed. %u", m_State);
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    m_State = WiFiJobStateInit;

    status = m_pEventQueue->DeferOperationCompletion(
        m_pJobCompletionCallback,
        ErrorStatus,
        this,
        &m_FailsafeJobCompletionEvent );

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CJobBase::get_FailsafeCompletionEvent(
    _Out_ Event** ppFailsafeCompletionEvent )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    if ( ppFailsafeCompletionEvent == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    *ppFailsafeCompletionEvent = &m_FailsafeJobCompletionEvent;

exit:
    return status;
}


PLIST_ENTRY CJobBase::get_ActiveJobListEntry()
{
    return &m_ActiveJobLink.ListEntry;
}


PLIST_ENTRY CJobBase::get_SerializedJobListEntry()
{
    return &m_SerializedJobLink.ListEntry;
}


CJobBase* CJobBase::get_JobFromListEntry(
    _In_ PLIST_ENTRY pListEntry )
{
    return (CJobBase*)GetCppObjectFromListEntry(
        pListEntry );
}

NDIS_STATUS CJobBase::set_ActiveJobCompletionCallback(
    _In_ IOperationCompletionCallback* pCompletionCallback )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    if ( pCompletionCallback == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    m_pActiveJobCompletionCallback = pCompletionCallback;

exit:
    return status;
}


NDIS_STATUS CJobBase::get_ActiveJobCompletionCallback(
    _Out_ IOperationCompletionCallback** ppCompletionCallback )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    if ( ppCompletionCallback == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    *ppCompletionCallback = m_pActiveJobCompletionCallback;

exit:
    return status;    
}



NDIS_STATUS CJobBase::set_SerializedJobCompletionCallback(
    _In_ IOperationCompletionCallback* pCompletionCallback )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    if ( pCompletionCallback == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    m_pSerializedJobCompletionCallback = pCompletionCallback;

exit:
    return status;
}

NDIS_STATUS CJobBase::get_SerializedJobCompletionCallback(
    _Out_ IOperationCompletionCallback** ppCompletionCallback )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    if ( ppCompletionCallback == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    *ppCompletionCallback = m_pSerializedJobCompletionCallback;

exit:
    return status;    
}

void CJobBase::set_RequiresPassiveIrql(
    _In_ bool RequiresPassiveIrql )
{
    m_RequiresPassiveIrql = RequiresPassiveIrql;
}

bool CJobBase::get_RequiresPassiveIrql()
{
    return m_RequiresPassiveIrql;
}

WFC_JOB_TYPE CJobBase::get_JobType()
{
    return m_JobType;
}

NDIS_STATUS CJobBase::get_PortPropertyCache(
    _Out_ CPortPropertyCache** ppPortPropertyCache )

{
    *ppPortPropertyCache = m_pPropertyCacheDirectory->GetPortPropertyCache(
        m_PortId);

    // At the job base level, we may not have a port
    if (*ppPortPropertyCache == NULL) 
        return NDIS_STATUS_FILE_NOT_FOUND;

    return NDIS_STATUS_SUCCESS;
}

CAdapterPropertyCache* CJobBase::get_AdapterPropertyCache()
{
    return m_pPropertyCacheDirectory->GetAdapterPropertyCache();
}
