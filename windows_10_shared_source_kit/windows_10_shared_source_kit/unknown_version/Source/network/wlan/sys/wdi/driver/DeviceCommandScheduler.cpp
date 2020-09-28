#include "precomp.hpp"
#include "DeviceCommandScheduler.hpp"
#include "DeviceCommandScheduler.tmh"
#include "ctlPlane.hpp"

DeviceCommandScheduler::DeviceCommandScheduler() :
    m_pEventQueue(NULL),
    m_pActiveDeviceCommand(NULL),
    m_pActiveDeviceCommandId(0),
    m_pControlPlane(NULL),
    m_ResetForPortPending(false),
    m_PortResetId(0),
    m_pResetForPortCompletionCallback(NULL),
    m_State(DeviceCommandSchedulerStateInit),
    m_ShuttingDown(true),
    m_TimerRegistrationToken(WFC_TIMER_REGISTRATION_TOKEN_INVALID),
    m_pTimerRegistrationContext(NULL),
///    m_DebugDiagnosticsTriggerCommand(0),
///    m_DebugDiagnosticsTriggerTime(0),
    m_FailsafeLowerLayerCompletionEvent(true, 0)
{
    InitializeListHead(&m_DeviceCommandQueue);
}

NDIS_STATUS DeviceCommandScheduler::Initialize(
    _In_ EventQueue* pEventQueue,
    _In_ CCtlPlane*  pControlPlane,
    _In_ CAdapter*   pAdapter)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG ulTimeout = DEFAULT_COMMAND_TIMEOUT;
    TraceEntry();
    
    if ( pEventQueue == NULL ||
         pControlPlane == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    m_pEventQueue = pEventQueue;
    m_pControlPlane = pControlPlane;
    m_pAdapter = pAdapter;

    status = m_pAdapter->GetAdapterPropertyCache()->
                GetPropertyULong(WfcAdapterCommandTimeout, &ulTimeout);
    if (status != NDIS_STATUS_SUCCESS)
    {
        //this should really never happen
        NT_ASSERT(FALSE);
    }
    WFCTrace("Use a command timeout value %x", ulTimeout);
    m_ulCommandTimeout = ulTimeout;

    status = m_pEventQueue->RegisterTimeoutCallbackWithLevelAndReuse(
        0,
        this,
        ulTimeout,
        NULL,
        WDF_EXECUTION_LEVEL::WdfExecutionLevelPassive,
        true,
        &m_pTimerRegistrationContext,
        &m_TimerRegistrationToken );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        NT_ASSERT(FALSE);
        // Continue normally else we will stall
    }

    m_ShuttingDown = false;

exit:
    TraceExit(status);
    return status;
}

CAdapter *
DeviceCommandScheduler::GetAdapter()
{
    return m_pAdapter;
}

DeviceCommandScheduler::~DeviceCommandScheduler()
{
    DeviceCommand* pPendingDeviceCommand = DequeueNextCommand();

    if (m_ShuttingDown)
    {
        // Already cleaned up
        return;
    }
    
    m_ShuttingDown = true;

    m_pEventQueue->DeregisterTimeoutCallback(
        m_TimerRegistrationToken );

    while( pPendingDeviceCommand != NULL )
    {
        WFCInfo("Flushing pending command from queue %p", pPendingDeviceCommand);
        m_pEventQueue->DeferOperationCompletion(
            pPendingDeviceCommand->m_pCompletionCallback,
            NDIS_STATUS_INVALID_STATE,
            pPendingDeviceCommand,
            &pPendingDeviceCommand->m_FailsafeCompletionEvent );

        pPendingDeviceCommand = DequeueNextCommand();
    }
}


NDIS_STATUS DeviceCommandScheduler::StartCommand(
    _In_ DeviceCommand*                pDeviceCommand,
    _In_ IOperationCompletionCallback* pCompletionCallback, // Completion callback. Could be Task or CJobBase
    _In_ CJobBase*                     pParentJob)
{
    NDIS_STATUS status             = NDIS_STATUS_SUCCESS;
    Event*      pQueueCommandEvent = NULL;

    if ( pDeviceCommand      == NULL ||
         pCompletionCallback == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( m_ShuttingDown )
    {
        WFCError("Tried to start new command while shutting down.");
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    pQueueCommandEvent = &pDeviceCommand->m_FailsafeCompletionEvent;

    status = pQueueCommandEvent->PopulateEvent(
        WiFiEventDeviceCommandSchedulerQueueDeviceCommand,
        (IEventQueueCallback*)this,
        pDeviceCommand );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    status = pDeviceCommand->set_CompletionCallback(
        pCompletionCallback );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    // Save this for debugging purposes
    pDeviceCommand->set_ParentJob(pParentJob);

    //
    // to guarantee commands are sent at PASSIVE that we promose IHVs
    //
    pQueueCommandEvent->set_RequiresPassiveIrql(TRUE);

    status = m_pEventQueue->QueueEvent(
        pQueueCommandEvent );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    //
    // Event Queue has taken ownership of this event.
    //
    pQueueCommandEvent = NULL;

exit:
    if ( pQueueCommandEvent != NULL )
    {
        delete pQueueCommandEvent;
    }

    return status;
}


void DeviceCommandScheduler::CancelTask(
    _In_ DeviceCommand* pPendingTaskDeviceCommand )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CancelDeviceCommand* pCancelDeviceCommand = NULL;

    TraceEntry();

    if ( pPendingTaskDeviceCommand == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    pCancelDeviceCommand = new CancelDeviceCommand(
        pPendingTaskDeviceCommand->get_ActivityId() );

    if ( pCancelDeviceCommand == NULL )
    {
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    status = pCancelDeviceCommand->Initialize(
        pPendingTaskDeviceCommand,
        m_pAdapter );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    WFCInfo(
        "Issuing cancel for %p",
        pPendingTaskDeviceCommand );

    status = StartCommand(
        pCancelDeviceCommand,
        (IOperationCompletionCallback*)this,
        pPendingTaskDeviceCommand->get_ParentJob());

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    pCancelDeviceCommand = NULL;

exit:
    if ( pCancelDeviceCommand != NULL )
    {
        delete pCancelDeviceCommand;
    }

    TraceExit(status);
}


void DeviceCommandScheduler::OnEventQueueCallback(
    _In_ WFC_EVENT_TYPE        Type,
    _In_opt_ NDIS_STATUS        Status,
    _In_opt_ WFC_SENDER_OBJECT SenderObject,
    _In_opt_ WFC_EVENT_PAYLOAD Payload )
{
    PWFC_MESSAGE_METADATA pResponse;
    ULONG totalLength;

    UNREFERENCED_PARAMETER(Status);

    switch(Type)
    {

    case WiFiEventDeviceCommandSchedulerQueueDeviceCommand:
        NT_ASSERT_ASSUME(Payload != NULL);
        OnQueueDeviceCommandEventHandler(
            ((DeviceCommand*)Payload) );
        break;

    case WiFiEventDeviceCommandSchedulerQueueCtlPlaneSendCommandComplete:
        NT_ASSERT_ASSUME( Payload != NULL );
        pResponse = ( PWFC_MESSAGE_METADATA ) Payload;
        totalLength = pResponse->MessageLength + FIELD_OFFSET(WFC_MESSAGE_METADATA, Message);
        ProcessCtlPlaneSendCommandCompleteEvent( totalLength,
                pResponse,
                (IWdiMessageMemoryManager*)SenderObject);
        break;

    default:
        NT_ASSERT(FALSE);
        break;
    };
}


void DeviceCommandScheduler::OnQueueDeviceCommandEventHandler(
    _In_ DeviceCommand* pDeviceCommand )
{
    bool listWasEmpty;

    NT_ASSERT(pDeviceCommand != NULL);

    listWasEmpty = IsListEmpty(
        &m_DeviceCommandQueue ) ? true : false;

    InsertTailList(
        &m_DeviceCommandQueue,
        pDeviceCommand->get_ListEntry() );

    if ( listWasEmpty )
    {
        OnIssueNextCommandHandler();
    }
}


void DeviceCommandScheduler::OnIssueNextCommandHandler()
{
    NDIS_STATUS    status             = NDIS_STATUS_SUCCESS;
    DeviceCommand* pNextDeviceCommand = NULL;
    bool           startedCommand     = false;

    TraceEntry();

    if ( m_pActiveDeviceCommand != NULL )
    {
        //
        // Bail out if a command is already being processed.
        //
        goto exit;
    }

    set_InternalState(
        DeviceCommandSchedulerStateSendingCommand );

    //
    // To prevent command scheduler from stalling when a command send
    // fails, we need to loop until a command is started (or the list
    // becomes/is empty).
    //
    while( !startedCommand &&
           !IsListEmpty(&m_DeviceCommandQueue) )
    {
        pNextDeviceCommand = DequeueNextCommand();

        if ( pNextDeviceCommand == NULL )
        {
            NT_ASSERT(FALSE);
            break;
        }

        int timeoutValue = m_ulCommandTimeout;

        (void)pNextDeviceCommand->get_CommandType(&m_pActiveDeviceCommandId);
        if (m_pActiveDeviceCommandId == WDI_TASK_OPEN)
        {
            // Use a custom timeout for the open command
            timeoutValue = OPEN_COMMAND_TIMEOUT; 
        }

        if (m_pActiveDeviceCommandId == WDI_SET_POWER_STATE)
        {
            // Use a custom timeout for the set power command
            timeoutValue = SET_POWER_COMMAND_TIMEOUT; 
        }

        status = m_pEventQueue->StartTimer(m_pTimerRegistrationContext, timeoutValue);
        if ( status != NDIS_STATUS_SUCCESS )
        {
            NT_ASSERT(FALSE);
            // Continue normally else we will stall
        }

        m_pActiveDeviceCommand = pNextDeviceCommand;
        m_pActiveDeviceCommand->SetPending(true);

        status = SendCommandToLowerLayer(
            pNextDeviceCommand );

        if ( status != NDIS_STATUS_PENDING )
        {
            OnActiveCommandCompleteHandler(
                status,
                false);
        }
        else
        {
            startedCommand = true;
        }
    }

    if ( startedCommand )
    {
        set_InternalState(
            DeviceCommandSchedulerStateWaitForCommandCompletion );
    }
    else
    {
        set_InternalState(
            DeviceCommandSchedulerStateInit );
    }

exit:

    TraceExit(status);
}

void DeviceCommandScheduler::OnActiveCommandCompleteHandler(
    _In_ NDIS_STATUS status,
    _In_ bool ReQueueCommand)
{
    DeviceCommand* pCompletedDeviceCommand = NULL;

    TraceEntry();

    if ( m_pActiveDeviceCommand == NULL )
    {
        NT_ASSERT(FALSE);
        goto exit;
    }

    m_pActiveDeviceCommand->SetPending(false);
    pCompletedDeviceCommand = m_pActiveDeviceCommand;
    m_pActiveDeviceCommand  = NULL;
    m_pActiveDeviceCommandId = 0;

    if (ReQueueCommand)
    {
        // Put this back in the queue
        InsertHeadList(
            &m_DeviceCommandQueue,
            pCompletedDeviceCommand->get_ListEntry());
    }
    else
    {   
        // Complete the command
        status = m_pEventQueue->DeferOperationCompletion(
            pCompletedDeviceCommand->m_pCompletionCallback,
            status,
            pCompletedDeviceCommand,
            &pCompletedDeviceCommand->m_FailsafeCompletionEvent );
        NT_ASSERT(status == NDIS_STATUS_SUCCESS);
    }
    
    m_pEventQueue->StopTimer(m_pTimerRegistrationContext);
    OnIssueNextCommandHandler();

exit:

    TraceExit(status);
}


void DeviceCommandScheduler::CompleteSendCommand_PostHangCheck(
    _In_ ULONG  TotalResponseLength,
    _In_reads_bytes_(TotalResponseLength) PWFC_MESSAGE_METADATA pResponse )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceEntry();
    
    NT_ASSERT(m_pActiveDeviceCommand != NULL);
    NT_ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

    if ( m_pActiveDeviceCommand == NULL )
    {
        //
        // Got a completion while there was no active command in the
        // device command scheduler. Buggy device?
        //
        NT_ASSERT(FALSE);
        goto exit;
    }

    if (pResponse->RetryOnSmallBuffer&& 
        (pResponse->Response()->OidCompletionStatus == NDIS_STATUS_BUFFER_OVERFLOW))
    {
        // Caller wants us to retry with bigger buffer
        // Let us submit the SendCommand again
        PWFC_MESSAGE_METADATA pCommandHeader;
        ULONG           inputBufferLength;

        status = m_pActiveDeviceCommand->get_InputBuffer(
            &inputBufferLength,
            (BYTE **)&pCommandHeader );
        
        WFCTrace( "Sending Command %d again with bigger buffer %d", pCommandHeader->MessageId,
            pResponse->Response()->OidBytesNeeded);

        // Reissue the request with a bigger buffer
        pCommandHeader->Command()->MinimumOutputLength = 
            pResponse->Response()->OidBytesNeeded;

        // Requeue this command
        OnActiveCommandCompleteHandler(
            NDIS_STATUS_SUCCESS, 
            true);
            
    }
    else
    {
        // Let the command complete
        
        //
        // This command may fail if the buffer can't be copied for some
        // reason, but we must still complete to the caller to avoid
        // hanging the stack.
        //
        status = m_pActiveDeviceCommand->set_CommandResult(
            TotalResponseLength,
            (BYTE *)pResponse
            );


        // Complete the command. We dont need to go through
        // the event queue
        OnActiveCommandCompleteHandler();
    }
    
exit:

    TraceExit(status);
}



void DeviceCommandScheduler::ProcessCtlPlaneSendCommandCompleteEvent(
    _In_ ULONG  TotalResponseLength,
    _In_reads_bytes_(TotalResponseLength) PWFC_MESSAGE_METADATA pResponse,
    _In_ IWdiMessageMemoryManager* pBufferManager)
/*++

    Completion from LE. We need to check if this is a hung command which we have completed.
    Drop it if yes.

--*/
{
    TraceEntry();

    if ( m_pEventQueue->m_HungResetRecoveryIterations &&
        (!pResponse->NonWdiOid) &&  // not support this (double buffering) yet
         m_pEventQueue->m_HungAtWdiCommandPasses == 
            ++m_pEventQueue->m_CountCommandsToLe )
    {
        //
        // Inject hang by not completing it when it's completed.
        //
        WFCError( "Inject hang by hiding cmd completion of id=%d type=%d\n",
                pResponse->MessageId, pResponse->MessageType );
        //DbgPrintEx(0,0, "Inject hang by hiding completion of id=%d type=%d\n",
        //            pResponse->MessageId, pResponse->MessageType );
        goto exit;
    }

    //
    // Have we requested RR. Lock is not needed because this is not in event queue
    //
    if ( m_pAdapter->m_lResetRecovery > 0 )
    {
        //
        // check if this is a hung command which we have completed.
        //
        if ( pResponse->Message.TransactionId == m_HungCommandToken )
        {
            WFCInfo( "HungCommand id %d finally returned, drop it", m_HungCommandToken);
            DbgPrintEx(0,0, "HungCommand id %d finally returned, drop it\n", m_HungCommandToken);
            goto exit;
        }
    }

    CompleteSendCommand_PostHangCheck( TotalResponseLength, pResponse );

exit:
    if ( pBufferManager )
    {
        pBufferManager->FreeMemory( (BYTE*)pResponse );
    }
    TraceExit(0);
}


void DeviceCommandScheduler::OnCtlPlaneWdiOidCompleteCallback(
    _In_ ULONG  TotalResponseLength,
    _In_reads_bytes_(TotalResponseLength) PWFC_MESSAGE_METADATA pResponse, // Caller owns memory
    _In_ IWdiMessageMemoryManager *pBufferManager )
/*++

    This method serailizes the LE completion into event Q.
    One of the purposes is to avoid race with OnTimerCallback processing.

--*/
{
    NDIS_STATUS status;

    TotalResponseLength; // ref for free build

    TraceEntry();

    NT_ASSERT( TotalResponseLength == pResponse->MessageLength + 
            FIELD_OFFSET(WFC_MESSAGE_METADATA, Message));

    Event * pEvent = new Event(false, 0);  // !preallocated, activityId uses 0

    if ( pEvent == NULL )
    {
        WFCInfo( "Failed allocate event for LowerEdgeCommandComplete Use failsafe" );

        pEvent = &m_FailsafeLowerLayerCompletionEvent;
    }

    // The miniport may have completed this at dispatch. Move to passive IRQL
    // to ensure that if we call them back (eg. with new OID or something) we are
    // at passive. This affects the DeviceCommand completion since it will happen
    // at PASSIVE. If we want to avoid this IRQL change we can skip this passive 
    // requirement here and do it when we are queuing the next command.
    pEvent->set_RequiresPassiveIrql(TRUE);
    
    status = pEvent->PopulateEvent(
        WiFiEventDeviceCommandSchedulerQueueCtlPlaneSendCommandComplete,
        (IEventQueueCallback*)this,
        pResponse,      // payload
        pBufferManager, // pEventPayload carries buffer manager
        pResponse->Response()->OidCompletionStatus);
    NT_ASSERT(status == NDIS_STATUS_SUCCESS);

    status = m_pEventQueue->QueueEvent( pEvent );
    NT_ASSERT(status == NDIS_STATUS_SUCCESS);

    TraceExit(status);
}

void DeviceCommandScheduler::s_CtlPlaneWdiOidCompleteCallback(
    _In_ PVOID  CallContext,
    _In_ ULONG  TotalResponseLength,
    _In_reads_bytes_(TotalResponseLength) PWFC_MESSAGE_METADATA pResponse, // Caller owns memory
    _In_ IWdiMessageMemoryManager *pBufferManager)
{
    if ( CallContext != NULL )
    {
        ((DeviceCommandScheduler*)CallContext)->OnCtlPlaneWdiOidCompleteCallback(
            TotalResponseLength,
            pResponse,
            pBufferManager);
    }
    else
    {
        NT_ASSERT(FALSE);
    }

    //
    // will free it after set_CommandResult
    //pBufferManager->FreeMemory((BYTE *)pResponse);
}


DeviceCommand* DeviceCommandScheduler::DequeueNextCommand()
{
    DeviceCommand* pToReturn      = NULL;
    PLIST_ENTRY    pNextListEntry = NULL;

    if ( IsListEmpty(&m_DeviceCommandQueue) )
    {
        return NULL;
    }

    pNextListEntry = RemoveHeadList(
        &m_DeviceCommandQueue );

    pToReturn = DeviceCommand::get_DeviceCommandFromListEntry(
        pNextListEntry );

    return pToReturn;
}


NDIS_STATUS DeviceCommandScheduler::SendCommandToLowerLayer(
    _In_ DeviceCommand* pDeviceCommand )
{
    NDIS_STATUS     status = NDIS_STATUS_SUCCESS;
    NTSTATUS        ctlPlaneStatus;
    ULONG           commandId;
    PWFC_MESSAGE_METADATA pCommandHeader;
    ULONG           inputBufferLength;

    TraceEntry();

    if ( pDeviceCommand == NULL )
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    status = pDeviceCommand->get_InputBuffer(
        &inputBufferLength,
        (BYTE **)&pCommandHeader );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    status = pDeviceCommand->get_CommandType(
        &commandId );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    pCommandHeader->MessageId = commandId;
    
    ctlPlaneStatus = m_pControlPlane->SendCommand(
        pCommandHeader,
        &s_CtlPlaneWdiOidCompleteCallback,
        this );

    if ( ctlPlaneStatus == STATUS_SUCCESS )
    {
        status = NDIS_STATUS_PENDING;
    }
    else
    {
        status = NDIS_STATUS_FAILURE;
    }

exit:
    TraceExit(status);
    return status;
}


void DeviceCommandScheduler::set_InternalState(
    _In_ DEVICE_COMMAND_SCHEDULER_STATE newState )
{
    m_State = newState;
}


void DeviceCommandScheduler::OnOperationCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    TraceEntry();

    if ( SenderObject != NULL )
    {
        CancelDeviceCommand* pCancelCommand = (CancelDeviceCommand*)SenderObject;
        delete pCancelCommand;
    }
    else
    {
        NT_ASSERT(FALSE);
    }

    TraceExit(OperationStatus);
}

// Fired from the event queue
void DeviceCommandScheduler::OnTimerCallback(
    _In_opt_ WFC_TIMER_CONTEXT )
/*++
    //
    // WARNING!WARNING!WARNING!
    // This callback can fire much later than when the timer actually expired.
    // So the current command can theoretically be different than the one for which the timer 
    // expired. However this timercallback does not care. It just issues a reset of the adapter
    //
--*/
{
    long rrCount = 0;
    BOOLEAN bResetRecoveryOff = FALSE;

    TraceEntry();
    //
    // If the timer got fired while the command completed
    // we will be unable to stop the timer. In that case the device commadn will be null and so
    // we can skip the reset.
    // However the timer can fire while another command
    // we will be unable to stop the timer. In that case the device command will be null and so
    // we can skip the reset. However the timer can fire while another command is in flight.
    //

    //
    // Late completion of hung Command will be drop at ProcessCtlPlaneSendCommandCompleteEvent
    //
    if ( this->m_pActiveDeviceCommand != nullptr )
    {
        CJobBase *pSourceJob;
        WFCError( "M1-M3 timer elapsed! command %!MESSAGE_ID! (%d)", m_pActiveDeviceCommandId, m_pActiveDeviceCommandId);
        NdisWriteErrorLogEntry(m_pAdapter->GetMiniportAdapterHandle(),
                            NDIS_ERROR_CODE_HARDWARE_FAILURE,
                            2,
                            COMMAND_TIMER_ELAPSED,
                            m_pActiveDeviceCommandId);

        bResetRecoveryOff = m_pAdapter->GetAdapterPropertyCache()->GetPropertyBooleanOrDefault(
            WfcAdapterResetRecoveryOff, FALSE );

        InterlockedIncrement( &m_pAdapter->m_lResetTriggerCount);

        GetAdapter()->TraceLoggingResetRecovery( COMMAND_TIMER_ELAPSED,
                                    m_pActiveDeviceCommandId,
                                    m_pAdapter->m_lResetTriggerCount,
                                    m_pAdapter->m_OemPLDRSupported,
                                    bResetRecoveryOff );

        if ( (m_pAdapter->m_OemPLDRSupported == 0) && 
             (m_pAdapter->GetMiniportDriver()->GetNdisCharacteristics()->ResetHandlerEx))
        {
            // OEM PLDR not supported, but IHV supports MiniportReset, return without doing more work.
            WFCError( "Command timeout but OEM disables PLDR AND IHV supports MiniportReset; bail out (without completing command)" );
            goto exit;
        }

        // Switch our behavior
        rrCount = InterlockedIncrement( &m_pAdapter->m_lResetRecovery );
        if ( rrCount > 1 ) 
        {
            ULONG commandId;
            m_pActiveDeviceCommand->get_CommandType(&commandId);
            // artifact of hang simulation may allow extra cmd to timeout
            WFCError( "Command timed out; But Reset Recovery already occurred. rr=%d Id=%d",
                rrCount, commandId );
            goto exit;
        }

        // Get the source job for this device command
        pSourceJob = m_pActiveDeviceCommand->get_ParentJob();
        NT_ASSERT(pSourceJob != NULL);
        
        //
        // Go get Diagnose info
        //
        m_pAdapter->TriggerControlPathDiagnostics(m_pActiveDeviceCommandId);
        m_pAdapter->CollectDebugData(m_pActiveDeviceCommand,
                NULL,
                pSourceJob
                );

        //
        // Diagnosis taken, check if OEM does not enable UEFI support
        //
        if ( m_pAdapter->m_OemPLDRSupported == 0 ) 
        {
            WFCError( "Command timeout but OEM disables PLDR; bail out" );
            goto exit;
        }

        //
        // Use CompleteSendCommand_PostHangCheck instead of 
        // ProcessCtlPlaneSendCommandCompleteEvent so reponse is not freed
        //
        // Until we double buffered unrecognized OID.
        // We can not complete such OID, not safe to do.
        // We can only wait for LE to return to unwind
        //
        if ( (pSourceJob != NULL) &&
             (pSourceJob->get_JobType() != WiFiJobUnrecognizedOidHandler) )
        {
            WFC_MESSAGE_METADATA response;
            ULONG cbInput;
            WFC_MESSAGE_METADATA * pbInput = NULL;

            response.Initialize(WfcMessageResponse);
            //
            // Input starts with the same content as repsonse
            //
            (void) m_pActiveDeviceCommand->get_InputBuffer( &cbInput, (PBYTE*)&pbInput );
            ASSERT( pbInput != NULL  && cbInput >= sizeof( response ));

            if ( pbInput && cbInput >= sizeof(response))
            {
                //
                // copy input info
                //
                response.MessageId = pbInput->MessageId;
                response.Response()->MappedWdiOid = pbInput->Command()->MappedWdiOid;
                response.Message.PortId= pbInput->Message.PortId;
                response.Message.TransactionId= pbInput->Message.TransactionId;
                
            }
            else
            {
                ASSERT(0);
            }
            response.MessageLength = sizeof( response );
            response.Response()->OidCompletionStatus = NDIS_STATUS_DEVICE_FAILED;

            WFCInfo( "Completing pending command %!MESSAGE_ID! (%d) on M3 timeout", 
                response.MessageId, response.MessageId);

            //
            // remember this command that it is completed here b/c hang
            //
            m_pActiveDeviceCommand->get_CommandType( &m_HungCommandId );
            m_pActiveDeviceCommand->get_CommandToken( &m_HungCommandToken );

            //
            // Use the no check hang version so we dont free response.
            // Complete the command so the NDIS command can unwind.
            //
            CompleteSendCommand_PostHangCheck( sizeof(response), &response );
        }

        if ( rrCount == 1 )
        {
            //
            // if RR is disabled, dont request it.
            //
            if (m_pAdapter->IsRecoverable() && bResetRecoveryOff == FALSE )
            {
                ASSERT(FALSE);
                GetAdapter()->ReenumerateFailedAdapter();
            }
        }
    }
exit:
    TraceExit(NDIS_STATUS_SUCCESS);
}

