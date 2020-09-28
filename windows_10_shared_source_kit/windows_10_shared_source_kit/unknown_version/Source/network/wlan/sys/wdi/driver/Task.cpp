#include "precomp.hpp"
#include "Task.hpp"
#include "Task.tmh"

Task::Task(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
    m_State(WiFiTaskStateInit),
    m_IsTaskCancelled(false),
    m_TaskDeviceCommand(ActivityId),
    m_pEventQueue(NULL),
    m_pDeviceCommandScheduler(NULL),
    m_pTaskCompletionCallback(NULL),
    m_NotificationRegistrationToken(WFC_REGISTRATION_TOKEN_INVALID),
    m_pNotificationManager(NULL),
    m_FailsafeStartedEvent(true, ActivityId),
    m_FailsafeStartTaskEvent(true, ActivityId),
    m_OutputBuffer(NULL),
    m_OutputBufferLength(0),
    m_IsDeviceCommandPending(false),
    m_CallStatus(NDIS_STATUS_FAILURE),
    EventTaskStarted(NULL),
    IActivityId(ActivityId),
    m_TimerRegistrationToken(WFC_TIMER_REGISTRATION_TOKEN_INVALID),
    m_pParentJob(NULL),
    m_bTimerStopped (false)
{
}


Task::~Task()
{
    if ( m_NotificationRegistrationToken != WFC_REGISTRATION_TOKEN_INVALID )
    {
        m_pNotificationManager->DeregisterNotificationHandler(
            m_NotificationRegistrationToken );
        m_NotificationRegistrationToken = WFC_REGISTRATION_TOKEN_INVALID;
    }

    if ( m_TimerRegistrationToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID )
    {
        m_pEventQueue->DeregisterTimeoutCallback(
            m_TimerRegistrationToken );
        m_TimerRegistrationToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    if ( m_OutputBuffer != NULL )
    {
        delete [] m_OutputBuffer;
        m_OutputBuffer = NULL;
        m_OutputBufferLength = 0;
    }
}

//
// Returns local pointer to buffer result (not a copy). Buffer
// lifetime is tied to Task lifetime, and may not be
// present if the Task has failed.
//
NDIS_STATUS Task::get_OutputBuffer(
    _Out_ ULONG *                                     pBufferLength,
    _Outptr_result_bytebuffer_(*pBufferLength) BYTE** pBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( pBuffer == NULL ||
         pBufferLength == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( m_OutputBuffer == NULL )
    {
        WFCActError("Output (M4) buffer is NULL.");
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    if ( m_OutputBufferLength < sizeof(WFC_MESSAGE_METADATA) )
    {
        WFCActError(
            "Output buffer isn't big enough. Expected >= %u, Actual = %u",
            (ULONG)sizeof(WFC_MESSAGE_METADATA),
            (ULONG)m_OutputBufferLength );
        status = NDIS_STATUS_INVALID_LENGTH;
        goto exit;
    }
   
    *pBufferLength = m_OutputBufferLength;
    *pBuffer = m_OutputBuffer;

exit:

    TraceActExit(status);
    return status;
}

// Use the value from the registry key
NDIS_STATUS Task::Initialize(
    _In_ NotificationManager*          pNotificationManager,
    _In_ DeviceCommandScheduler*       pDeviceCommandScheduler,
    _In_ EventQueue*                   pEventQueue)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG ulTimeout = DEFAULT_TASK_TIMEOUT;
    TraceActEntry();
    status = pDeviceCommandScheduler->GetAdapter()->GetAdapterPropertyCache()->GetPropertyULong(WfcAdapterTaskTimeout, &ulTimeout);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Unable to get property for WfcAdapterTaskTimeout, using default value = %d", DEFAULT_TASK_TIMEOUT);
        NT_ASSERT(FALSE);
        ulTimeout = DEFAULT_TASK_TIMEOUT;
    }
    WFCActTrace("Use a task timeout value %x", ulTimeout);
    status = Initialize(pNotificationManager, pDeviceCommandScheduler, pEventQueue, ulTimeout);
    TraceActExit(status);
    return status;
}

NDIS_STATUS Task::Initialize(
    _In_ NotificationManager*          pNotificationManager,
    _In_ DeviceCommandScheduler*       pDeviceCommandScheduler,
    _In_ EventQueue*                   pEventQueue,
    _In_ ULONG                         RelativeTimeoutInMSec)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( pNotificationManager == NULL ||
         pDeviceCommandScheduler == NULL ||
         pEventQueue == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    // If this is being reused, deregister before reuse
    if ( m_TimerRegistrationToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID )
    {
        m_pEventQueue->DeregisterTimeoutCallback(
            m_TimerRegistrationToken );
        m_TimerRegistrationToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    m_pNotificationManager = pNotificationManager;
    m_pDeviceCommandScheduler = pDeviceCommandScheduler;
    m_pEventQueue = pEventQueue;
    m_RelativeTimeoutInMSec = RelativeTimeoutInMSec;
    m_IsTaskCancelled = false;

    status = m_pEventQueue->RegisterTimeoutCallbackWithLevelAndReuse(
        get_ActivityId(),
        this,
        RelativeTimeoutInMSec, // 20 seconds
        NULL,
        WDF_EXECUTION_LEVEL::WdfExecutionLevelPassive,
        true,
        &m_pTimerRegistrationContext,
        &m_TimerRegistrationToken );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't register watchdog timer for M4 callback.");
        goto exit;
    }

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS Task::StartTask(
    _In_ IOperationCompletionCallback* pCompletionCallback,
    _In_ CJobBase*                 pParentJob)
{
    NDIS_STATUS status       = NDIS_STATUS_SUCCESS;
    bool        stateChanged = false;
    bool        isCommandInitialized;

    if ( pCompletionCallback == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = STATUS_INVALID_PARAMETER;
        goto exit;
    }

    status = set_InternalState(
        WiFiTaskStateInit );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        NT_ASSERT(FALSE);
        WFCActError("Couldn't set internal state to WiFiTaskStateInit");
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    if ( m_TaskDeviceCommand.get_IsInitialized(
             &isCommandInitialized ) != NDIS_STATUS_SUCCESS ||
         !isCommandInitialized )
    {
        WFCActError("Task's device command isn't initialized.");
        status = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    stateChanged = true;
    m_pTaskCompletionCallback = pCompletionCallback;
    m_pParentJob = pParentJob;

    status = m_FailsafeStartTaskEvent.PopulateEvent(
        WiFiEventStartTask,
        (IEventQueueCallback*)this );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't populate event.");
        goto exit;
    }

    status = set_InternalState(
        WiFiTaskStateStarting );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        NT_ASSERT(FALSE);
        WFCActError("Couldn't set internal state to WiFiTaskStateStarting.");
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    status = m_pEventQueue->QueueEvent(
        &m_FailsafeStartTaskEvent );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }
    
exit:

    if ( status != NDIS_STATUS_SUCCESS )
    {
        if ( stateChanged )
        {
            (VOID)set_InternalState(
                WiFiTaskStateInit );
        }
    }

    return status;
}


void Task::CancelTask()
{
    TraceActEntry();

    if ( m_IsTaskCancelled )
    {
        WFCActError("Task is already cancelled.");
        goto exit;
    }

    m_IsTaskCancelled = true;

    if ( m_State == WiFiTaskStatePending )
    {
        m_pDeviceCommandScheduler->CancelTask(
            &m_TaskDeviceCommand );
    }

exit:
    TraceActExit(NDIS_STATUS_SUCCESS);
}


void Task::OnEventQueueCallback(
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
    case WiFiEventStartTask:
        OnStartTaskHandler();
        break;
    default:
        ASSERT(FALSE);
        break;
    };

}


void Task::OnOperationCompleted(
    _In_ NDIS_STATUS        OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(SenderObject);

    NT_ASSERT(SenderObject == &m_TaskDeviceCommand);

    OnTaskStartedHandler(
        OperationStatus );
}


void Task::OnStartTaskHandler()
{
    NDIS_STATUS        status = NDIS_STATUS_SUCCESS;
    WFC_COMMAND_TOKEN commandToken;
    WFC_PORT_ID       portId;

    status = m_TaskDeviceCommand.get_CommandToken(
        &commandToken );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't get command token from Task's device command.");
        goto exit;
    }

    status = m_TaskDeviceCommand.get_PortId(
        &portId );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't get port ID from Task's device command.");
        goto exit;
    }

    //
    // Register for all notifications that match the given command
    // token. This completion should be generic, so caller (in this
    // case the Task object) isn't required to provide the type of
    // completion ahead of time.
    //
    status = m_pNotificationManager->RegisterForNotifications(
        WiFiIndicationInvalid,
        commandToken,
        portId,
        (INotifyDeviceIndicationCallback*)this,
        &m_NotificationRegistrationToken );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't register to listen for M4 callback.");
        goto exit;
    }

    status = m_pEventQueue->StartTimer(m_pTimerRegistrationContext, m_RelativeTimeoutInMSec);
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't register watchdog timer for M4 callback.");
        goto exit;
    }
    m_bTimerStopped = false;

    status = IssueDeviceCommandToCommandScheduler();

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't issue device command to lower layer.");
        goto exit;
    }

    m_IsDeviceCommandPending = true;

exit:
    if ( status != NDIS_STATUS_SUCCESS )
    {
        OnTaskCompletedHandler(
            status );
    }

}


void Task::OnTaskStartedHandler(
    _In_ NDIS_STATUS CallStatus )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    //
    // This is the completion for the device command to start the
    // task. Set the pending flag to false so completion callback can
    // be called.
    //
    m_IsDeviceCommandPending = false;

    WFCActInfo("M3 Arrived");

    if ( m_State == WiFiTaskStateCompleted )
    {
        WFCActInfo("M4 already arrived, bailing out.");
        status = m_CallStatus;
        goto exit;
    }

    if ( CallStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("M3 Failed with 0x%x", CallStatus);
        status = CallStatus;
        goto exit;
    }

    status = set_InternalState(
        WiFiTaskStatePending );

    if ( status != STATUS_SUCCESS )
    {
        WFCActError("Couldn't set internal state to pending.");
        goto exit;
    }

    status = CMessageHelper::GetStatusFromCommandResult(&m_TaskDeviceCommand,
                    NULL);
    if ( status != STATUS_SUCCESS )
    {
        WFCActError(
            "M3 NDIS Status is not successful. Got 0x%x", status);
        goto exit;
    }

    if ( m_IsTaskCancelled )
    {
        WFCActError("Task got cancelled, sending cancel to command scheduler.");
        m_pDeviceCommandScheduler->CancelTask(
            &m_TaskDeviceCommand );
    }

exit:

    //
    // Always notify operation started.
    //
    if ( EventTaskStarted != NULL )
    {
        NDIS_STATUS deferStatus = m_pEventQueue->DeferNotifyOperationStarted(
            EventTaskStarted,
            status,
            this,
            &m_FailsafeStartedEvent );

        UNREFERENCED_PARAMETER(deferStatus);
        NT_ASSERT(deferStatus == NDIS_STATUS_SUCCESS);
    }

    if ( status != NDIS_STATUS_SUCCESS ||
         m_State == WiFiTaskStateCompleted )
    {
        OnTaskCompletedHandler(
            status );
    }

    TraceActExit(status);
}


void Task::OnTaskCompletedHandler(
    _In_ NDIS_STATUS                              CallStatus,
    _In_opt_ ULONG                                EventBufferLength,
    _In_reads_bytes_opt_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    //
    // If the task has already been completed, we don't need to
    // inspect the input parameters because final status has already
    // been committed to the object.
    //
    if ( m_State == WiFiTaskStateCompleted )
    {
        WFCActInfo("M4 already arrived, skipping to completion callback.");
        // This is normally ok. But when we simulate cmd hang, 
        // A timed out cmd may have a success task. Dont use the task m_CallStatus.
        if ( m_CallStatus == NDIS_STATUS_SUCCESS )
        {
            status = CallStatus;
        }
        goto exit;
    }

    if ( CallStatus == NDIS_STATUS_SUCCESS &&
         ( EventBuffer == NULL ||
           EventBufferLength < sizeof(WFC_MESSAGE_METADATA) ) )
    {
        WFCActError(
            "Call was successful, but event buffer was bad. EventBuffer = %p, EventBufferLength = %u",
            EventBuffer,
            (ULONG)EventBufferLength );
        status = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    status = CallStatus;

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Task execution failed with 0x%x", status);
        goto exit;
    }

    if ( m_OutputBuffer != NULL )
    {
        delete [] m_OutputBuffer;
        m_OutputBuffer = NULL;
        m_OutputBufferLength = 0;
    }

    m_OutputBuffer = new BYTE[EventBufferLength];

    if ( m_OutputBuffer == NULL )
    {
        WFCActError("Couldn't make a copy of M4 buffer.");
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    m_OutputBufferLength = EventBufferLength;

    RtlCopyMemory(
        m_OutputBuffer,
        EventBuffer,
        EventBufferLength );

    WFCActInfo(
        "M4 Completed with status 0x%x",
        ((WFC_MESSAGE_METADATA*)m_OutputBuffer)->Message.Status);

exit:

    //
    // If the device command to start this task is still pending we
    // can't issue operation completion until that command completes.
    //
    if ( !m_IsDeviceCommandPending )
    {
        status = m_pEventQueue->DeferOperationCompletion(
            m_pTaskCompletionCallback,
            status,
            this,
            &m_TaskDeviceCommand.m_FailsafeCompletionEvent );
        NT_ASSERT(status == NDIS_STATUS_SUCCESS);
    }

    m_CallStatus = status;

    (VOID)set_InternalState(
        WiFiTaskStateCompleted );

    if ( m_NotificationRegistrationToken != WFC_REGISTRATION_TOKEN_INVALID )
    {
        status = m_pNotificationManager->DeregisterNotificationHandler(
            m_NotificationRegistrationToken );
        m_NotificationRegistrationToken = WFC_REGISTRATION_TOKEN_INVALID;
        NT_ASSERT(status == NDIS_STATUS_SUCCESS);
    }

    if (m_bTimerStopped == false)
    {
        m_pEventQueue->StopTimer(m_pTimerRegistrationContext);
        m_bTimerStopped = true;
    }

    TraceActExit(status);
}


NDIS_STATUS Task::set_InternalState(
    _In_ WFC_TASK_STATE NewState )
{
    //TODO: Add state validation

    m_State = NewState;

    return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS Task::IssueDeviceCommandToCommandScheduler()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    status = m_pDeviceCommandScheduler->StartCommand(
        &m_TaskDeviceCommand,
        (IOperationCompletionCallback*)this,
        m_pParentJob);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't start command to device scheduler.");
        goto exit;
    }

    WFCActInfo("M1 sent to device command scheduler");

exit:

    TraceActExit(status);
    return status;
}

void Task::OnDeviceIndicationArrived(
    _In_ WDI_INDICATION_TYPE                 IndicationType,
    _In_ WFC_COMMAND_TOKEN                   CommandToken,
    _In_ WFC_PORT_ID                         PortId,
    _In_ ULONG                                EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    WFC_COMMAND_TOKEN commandToken;
    WFC_PORT_ID portId;

    UNREFERENCED_PARAMETER(IndicationType);

    if ( m_TaskDeviceCommand.get_CommandToken(
             &commandToken ) == NDIS_STATUS_SUCCESS &&
         m_TaskDeviceCommand.get_PortId(
             &portId) == NDIS_STATUS_SUCCESS &&
         commandToken == CommandToken &&
         portId == PortId )
    {
        OnTaskCompletedHandler(
            NDIS_STATUS_SUCCESS,
            EventBufferLength,
            EventBuffer );
    }
    else
    {
        NT_ASSERT(FALSE);
    }
}


void Task::OnTimerCallback(
    _In_opt_ WFC_TIMER_CONTEXT )
/*++

    // WARNING!WARNING!WARNING!
    // This callback can fire much later than when the timer actually expired.
    // So the current command can theoretically be different than the one for which the timer 
    // expired. However this timercallback does not care. It just issues a reset of the adapter

--*/
{
    TraceEntry();
    ULONG taskId;
    CAdapter *pAdapter = m_pDeviceCommandScheduler->GetAdapter();
    ULONG rrCount =0;
    BOOLEAN bResetRecoveryOff = FALSE;

    m_TaskDeviceCommand.get_CommandType(&taskId);
    WFCActError( "M4 timer elapsed! task %!MESSAGE_ID! (%d)", taskId, taskId);
        
    NdisWriteErrorLogEntry(pAdapter->GetMiniportAdapterHandle(),
                            NDIS_ERROR_CODE_HARDWARE_FAILURE,
                            2,
                            TASK_TIMER_ELAPSED,
                            taskId);

    bResetRecoveryOff = m_pDeviceCommandScheduler->GetAdapter()->GetAdapterPropertyCache()->
            GetPropertyBooleanOrDefault(WfcAdapterResetRecoveryOff, FALSE);

    InterlockedIncrement( &pAdapter->m_lResetTriggerCount);    
    pAdapter->TraceLoggingResetRecovery( TASK_TIMER_ELAPSED,
                                taskId,
                                pAdapter->m_lResetTriggerCount,
                                pAdapter->m_OemPLDRSupported,
                                bResetRecoveryOff );

    if ( (pAdapter->m_OemPLDRSupported == 0) && 
         (pAdapter->GetMiniportDriver()->GetNdisCharacteristics()->ResetHandlerEx))
    {
        // OEM PLDR not supported, but IHV supports MiniportReset, return without doing more work.
        WFCActError( "Task timeout but OEM disabled PLDR AND IHV supports MiniportReset; bail out (after completing task)" );

        // Exit completes the task
        goto exit;                                    
    }
    

    rrCount = InterlockedIncrement( &pAdapter->m_lResetRecovery );
    if (  rrCount > 1 )
    {
        WFCActError( "Task timed out; Reset Recovery already occurred. Be done." );
        goto exit;
    }

    //
    // If the M4 timedout and we are still initializing, we just fail the task. Otherwise we try to
    // reset/recover the adapter. We cannot recover during initialization due to deadlock issues in PNP 
    // (PNP Holds the same lock for starting a device and surprise removal)
    //
    pAdapter->CollectDebugData(&m_TaskDeviceCommand,
            this,
            m_pParentJob);
            
    pAdapter->TriggerControlPathDiagnostics(taskId);

    //
    // Diagnosis taken, check if OEM does not enable UEFI support
    //
    if ( pAdapter->m_OemPLDRSupported == 0 ) 
    {
        WFCError( "Task timeout but OEM disables PLDR; bail out" );
        goto exit;
    }

    //
    // Kick off the reset and recovery process
    //
    if ( pAdapter->IsRecoverable() && bResetRecoveryOff == FALSE)
    {
        ASSERT(FALSE);
        m_pDeviceCommandScheduler->GetAdapter()->ReenumerateFailedAdapter();
    }

exit:
    //
    // Complete the task to unblock jobs in the queue.
    //
    OnTaskCompletedHandler(NDIS_STATUS_DEVICE_FAILED);

    TraceExit(NDIS_STATUS_SUCCESS);
}

NDIS_STATUS Task::get_TaskDeviceCommand(
    _Out_ DeviceCommand** ppDeviceCommand )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    if ( ppDeviceCommand == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    *ppDeviceCommand = &m_TaskDeviceCommand;

exit:
    TraceActExit(status);
    return status;
}
