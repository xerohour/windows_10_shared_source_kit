#include "Precomp.hpp"
#include "FrameworkHelpers.hpp"
#include "FrameworkHelpers.tmh"

void CDeviceCommandOidJobBase::CompleteJobHelper(
    _In_ NDIS_STATUS OperationStatus )
{
    TraceActEntry();

    OnNotifyJobCompletion(
        m_CurrentCommandToken,
        OperationStatus );

    m_CurrentCommandToken = NULL;
    m_CurrentCommandType = WfcDeviceCommandTypeInvalid;
    m_CurrentIoctl = 0x0;

    if ( m_pCurrentTlvBuffer!= NULL )
    {
        delete [] m_pCurrentTlvBuffer;
        m_pCurrentTlvBuffer = NULL;
        m_CurrentTlvBufferLength = 0;
    }

    CompleteJob(OperationStatus);

    TraceActExit(OperationStatus);
}

void CDeviceCommandOidJobBase::HandleNextDeviceCommand()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    if ( m_IsCancelled )
    {
        status = NDIS_STATUS_REQUEST_ABORTED;
        WFCActErrorStatus("Device Command Job was cancelled, bailing out.");
        goto exit;
    }

    if ( m_pCurrentTlvBuffer!= NULL )
    {
        delete [] m_pCurrentTlvBuffer;
        m_pCurrentTlvBuffer = NULL;
    }

    status = QueryNextCommandInfo(
        sizeof(WFC_MESSAGE_METADATA),
        &m_CurrentCommandToken,
        &m_CurrentCommandType,
        &m_CurrentIoctl,
        &m_CurrentTlvBufferLength,
        &m_pCurrentTlvBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't create task TLV buffer.");
        goto exit;
    }

    status = m_pDeviceCommand->Initialize(
        get_PortId(),
        m_CurrentIoctl,
        m_CurrentTlvBufferLength,
        m_pCurrentTlvBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't initialize device command.");
        goto exit;
    }

    if ( m_CurrentCommandType == WfcDeviceCommandTypeTask )
    {
        status = m_TaskCommand.Initialize(
            m_pAdapter->GetNotificationManager(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetEventQueue() );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't initialize task command.");
            goto exit;
        }

        status = StartTask(
            &m_TaskCommand );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't start task command.");
            goto exit;
        }
    }
    else if ( m_CurrentCommandType == WfcDeviceCommandTypeProperty )
    {
        status = QueueDeviceCommand(
            m_pDeviceCommand );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't queue device command to task scheduler.");
            goto exit;
        }
    }

exit:

    if ( status != NDIS_STATUS_SUCCESS )
    {
        CompleteJobHelper(status);
    }

    TraceActExit(status);
}

void CDeviceCommandOidJobBase::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    NotifyStarted(
        NDIS_STATUS_SUCCESS );

    status = m_TaskCommand.get_TaskDeviceCommand(
        &m_pDeviceCommand );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        NT_ASSERT(FALSE);
        goto exit;
    }

    HandleNextDeviceCommand();

exit:
    if ( status != NDIS_STATUS_SUCCESS )
    {
        CompleteJobHelper(status);
    }
}

void CDeviceCommandOidJobBase::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    NDIS_STATUS status = OperationStatus;
    NDIS_STATUS wabiStatus = NDIS_STATUS_FAILURE;
    ULONG commandResultBufferLength = 0;
    BYTE* pCommandResultBuffer = NULL;
    bool bQueryNextCommand = false;

    UNREFERENCED_PARAMETER(SenderObject);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    if ( m_CurrentCommandType == WfcDeviceCommandTypeProperty )
    {
        NT_ASSERT(SenderObject == m_pDeviceCommand);

        status = m_pDeviceCommand->get_CommandResult(
            &commandResultBufferLength,
            &pCommandResultBuffer );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            NT_ASSERT(FALSE);
            WFCActErrorStatus("Command failed by lower edge");
            status = NDIS_STATUS_FAILURE;
            goto exit;
        }

        status = CMessageHelper::GetStatusFromCommandResult(
            m_pDeviceCommand,
            &wabiStatus );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't get WABI status from command result.");
            goto exit;
        }

    }
    else if ( m_CurrentCommandType == WfcDeviceCommandTypeTask )
    {
        NT_ASSERT(SenderObject == &m_TaskCommand);

        status = m_TaskCommand.get_OutputBuffer(
            &commandResultBufferLength,
            &pCommandResultBuffer );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            NT_ASSERT(FALSE);
            WFCActErrorStatus("Task failed by lower edge (couldn't get output buffer, not supported?).");
            status = NDIS_STATUS_FAILURE;
            goto exit;
        }

        status = CMessageHelper::GetStatusFromTaskOutput(
            &m_TaskCommand,
            &wabiStatus );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't get WABI status from task result.");
            goto exit;
        }
    }

    if ( wabiStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActInfo("[TOKEN:%p] Device command/task failed with WDI_STATUS 0x%x.", m_CurrentCommandToken, wabiStatus);
        status = wabiStatus;
        goto exit;
    }

exit:
    status = HandleCommandCompletion(
        m_CurrentCommandToken,
        status,
        wabiStatus,
        commandResultBufferLength,
        pCommandResultBuffer,
        &bQueryNextCommand );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("HandleCommandCompletion came back with a failure. Completing job.");
        CompleteJobHelper(status);
    }
    else if ( !bQueryNextCommand )
    {
        WFCActInfo("No more commands to process, completing job.");
        CompleteJobHelper(status);
    }
    else
    {
        WFCActInfo("More commands available, processing next command.");
        HandleNextDeviceCommand();
    }

    TraceActExit(status);
}

void CDeviceCommandOidJobBase::OnJobCancelled()
{
    TraceActEntry();

    if ( m_IsCancelled )
    {
        WFCActInfo("Already cancelled, bailing out of cancel attempt.");
        goto exit;
    }

    m_IsCancelled = true;

    if ( m_CurrentCommandType == WfcDeviceCommandTypeTask )
    {
        WFCActInfo("Got a cancel request while a task was pending.");
        CancelActivePendingCommand();
    }
    else
    {
        WFCActInfo("Got a cancel request with no task pending. Job will be cancelled at next available opportunity.");
    }

exit:
    TraceActExit(NDIS_STATUS_SUCCESS);
}


//
// Default implementation for CDeviceCommandOidJobBase::HandleCommandCompletion.
//
// This is useful for jobs that only need to send one device command and don't
// care about the content of the result.
//
// Overriding this function allows the child class to both inspect the
// result, and decide whether or not they want to send down another
// command.
//
NDIS_STATUS CDeviceCommandOidJobBase::HandleCommandCompletion(
    _In_ WFC_DEVICE_COMMAND_TOKEN CommandToken,
    _In_ NDIS_STATUS OperationStatus,
    _In_ NDIS_STATUS WiFiStatus,
    _In_ ULONG,
    _In_reads_bytes_(CommandCompletionBufferLength) BYTE*,
    _Out_ bool* pQueryNextCommand )
{
    NDIS_STATUS status = OperationStatus;

    TraceActEntry();

    *pQueryNextCommand = false;

    WFCActInfo(
        "[TOKEN=%p] %!FUNC! - Command completed with status 0x%x (WiFiStatus: 0x%x)",
        CommandToken,
        status,
        WiFiStatus );

    TraceActExit(status);
    return status;
}
