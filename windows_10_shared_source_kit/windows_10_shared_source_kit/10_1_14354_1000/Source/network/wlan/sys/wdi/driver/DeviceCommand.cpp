#include "precomp.hpp"

#include "DeviceCommand.hpp"
#include "DeviceCommand.tmh"

//
// Job-accessible APIs
//
DeviceCommand::DeviceCommand(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
    m_CommandId(0),
    m_InputBufferLength(0),
    m_InputBuffer(NULL),
    m_OutputBufferLength(0),
    m_OutputBuffer(NULL),
    m_CommandToken(WFC_COMMAND_TOKEN_INVALID),
    m_PortId(WDI_PORT_ID_ADAPTER),
    m_pCompletionCallback(NULL),
    m_FailsafeCompletionEvent(true, ActivityId),
    m_IsPending(false),
    m_IsInitialized(false),
    m_pParentJob(NULL),
    IActivityId(ActivityId)
{
    InitializeCppListEntry(
        this,
        &m_Link );
}

DeviceCommand::~DeviceCommand()
{
    Cleanup();
}

void DeviceCommand::Cleanup()
{
    if ( m_OutputBuffer != NULL )
    {
        delete [] m_OutputBuffer;
        m_OutputBuffer = NULL;
        m_OutputBufferLength = 0;
    }
}

LONG s_NextCommandToken = WFC_COMMAND_TOKEN_INVALID;
WFC_COMMAND_TOKEN DeviceCommand::s_get_NextCommandToken()
{
    LONG nextToken = InterlockedIncrement(
        &s_NextCommandToken );

    if ( nextToken == WFC_COMMAND_TOKEN_INVALID )
    {
        nextToken = InterlockedIncrement(
            &s_NextCommandToken );
    }

    return (WFC_COMMAND_TOKEN)nextToken;
}

//
// Input pointers in this case are owned by the caller. If device
// command is queued into device command scheduler, memory for
// pBuffer must not be freed until device command has completed.
//
NDIS_STATUS DeviceCommand::Initialize(
    _In_ WFC_PORT_ID                    PortId,
    _In_ ULONG                          CommandId,
    _In_ ULONG                           BufferLength,
    _In_reads_bytes_(BufferLength) BYTE* Buffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_MESSAGE_METADATA* pCommonHeader;
    TraceActEntry();

    if ( CommandId == 0 ||
         Buffer == NULL ||
         BufferLength < sizeof(WFC_MESSAGE_METADATA) )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    m_PortId = PortId;
    m_CommandId = CommandId;
    m_CommandToken = s_get_NextCommandToken();
    m_InputBufferLength = BufferLength;
    m_InputBuffer = Buffer;
    m_IsInitialized = true;

    pCommonHeader = (WFC_MESSAGE_METADATA*)m_InputBuffer;

    pCommonHeader->Initialize(WfcMessageCommand);

    pCommonHeader->MessageId = CommandId;    
    // This should only contain the raw length from the device
    pCommonHeader->MessageLength = (UINT32)m_InputBufferLength 
        - FIELD_OFFSET(WFC_MESSAGE_METADATA, Message);
    pCommonHeader->Message.TransactionId = m_CommandToken;
    pCommonHeader->Message.PortId = m_PortId;

exit:

    TraceActExit(status);
    return status;
}


void DeviceCommand::SetPending(
    bool IsPending )
{
    NT_ASSERT(m_IsPending != IsPending);
    m_IsPending = IsPending;

    // For debugging hangs, etc save the time at which we submitted the command
    m_DebugCommandSubmitTime = CSystem::get_CurrentTime();
}


//
// Returns local pointer to buffer result (not a copy). Buffer
// lifetime is tied to DeviceCommand lifetime, and may not be
// present if the command has failed.
//
NDIS_STATUS DeviceCommand::get_CommandResult(
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
        WFCActError("No output buffer to return.");
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    if ( m_OutputBufferLength < sizeof(WFC_MESSAGE_METADATA) )
    {
        WFCActError(
            "Output buffer is too small. Expected >= %u, Actual = %u",
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


NDIS_STATUS DeviceCommand::get_IsInitialized(
    _Out_ bool* pIsInitialized )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( pIsInitialized == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    *pIsInitialized = m_IsInitialized;

exit:

    TraceActExit(status);
    return status;
}


NDIS_STATUS DeviceCommand::get_CommandToken(
    _Out_ WFC_COMMAND_TOKEN *pCommandToken )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( pCommandToken == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    *pCommandToken = m_CommandToken;

exit:

    TraceActExit(status);
    return status;
}


NDIS_STATUS DeviceCommand::get_PortId(
    _Out_ WFC_PORT_ID *pPortId )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( pPortId == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    *pPortId = m_PortId;

exit:

    TraceActExit(status);
    return status;
}


NDIS_STATUS DeviceCommand::get_CommandType(
    _Out_ ULONG *pCommandId )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( pCommandId == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    *pCommandId = m_CommandId;

exit:

    TraceActExit(status);
    return status;
}


//
// Gets a buffer local to this DeviceCommand, which is the
// full M1 message buffer to the device.
//
NDIS_STATUS DeviceCommand::get_InputBuffer(
    _Out_ ULONG *                                     pBufferLength,
    _Outptr_result_bytebuffer_(*pBufferLength) BYTE** pBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( pBufferLength == NULL ||
         pBuffer == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( !m_IsInitialized )
    {
        WFCActError("Device command is not initialized.");
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    *pBufferLength = m_InputBufferLength;
    *pBuffer = m_InputBuffer;

exit:

    TraceActExit(status);
    return status;
}


//
// Device Command makes an internal copy of this buffer.
//
NDIS_STATUS DeviceCommand::set_CommandResult(
    _In_opt_ ULONG BufferLength,
    _In_reads_bytes_opt_(BufferLength) BYTE* Buffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( Buffer == NULL ||
         BufferLength == 0 )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if ( m_OutputBuffer != NULL )
    {
        delete [] m_OutputBuffer;
        m_OutputBuffer = NULL;
    }

    if ( BufferLength < sizeof(WFC_MESSAGE_METADATA) )
    {
        WFCActError(
            "Command result buffer isn't big enough. Expected >= %u, Actual = %u",
            (ULONG)sizeof(WFC_MESSAGE_METADATA),
            (ULONG)BufferLength );
        status = NDIS_STATUS_INVALID_LENGTH;
        goto exit;
    }

    m_OutputBuffer = new BYTE[BufferLength];

    if ( m_OutputBuffer == NULL )
    {
        WFCActError("Couldn't allocate copy for command result buffer copy.");
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    m_OutputBufferLength = BufferLength;

    RtlCopyMemory(
        m_OutputBuffer,
        Buffer,
        m_OutputBufferLength );

exit:

    TraceActExit(status);
    return status;
}


NDIS_STATUS DeviceCommand::set_CompletionCallback(
    _In_ IOperationCompletionCallback* pCompletionCallback )
{
    TraceActEntry();

    m_pCompletionCallback = pCompletionCallback;

    TraceActExit(NDIS_STATUS_SUCCESS);
    return NDIS_STATUS_SUCCESS;
}


PLIST_ENTRY DeviceCommand::get_ListEntry()
{
    return &m_Link.ListEntry;
}


DeviceCommand* DeviceCommand::get_DeviceCommandFromListEntry(
    _In_ PLIST_ENTRY pEntry )
{
    return (DeviceCommand*) GetCppObjectFromListEntry(
        pEntry );
}

void DeviceCommand::set_ParentJob(
    _In_opt_ CJobBase* pParentJob)
{
    m_pParentJob = pParentJob;
}

CJobBase* DeviceCommand::get_ParentJob()
{
    return m_pParentJob;
}

CancelDeviceCommand::CancelDeviceCommand(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
    DeviceCommand(ActivityId)
{

}

NDIS_STATUS CancelDeviceCommand::Initialize(
    _In_ DeviceCommand* pPendingDeviceCommand,
    _In_ CAdapter* pAdapter)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_MESSAGE_METADATA* pTaskCommandHeader;
    ULONG  taskCommandSize, newCommandSize;
    const PLACEMENT_NEW_ALLOCATION_CONTEXT pnac = { sizeof( m_CommandBuffer ), m_CommandBuffer };
    TLV_CONTEXT tlvCtx = *pAdapter->GetTlvContext();
    WDI_TASK_ABORT_PARAMETERS abortParams;
    BYTE * pbBuffer = NULL;

    TraceActEntry();

    if ( pPendingDeviceCommand == NULL )
    {
        WFCActError("Bad parameter to %!FUNC!");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    status = pPendingDeviceCommand->get_InputBuffer(
        &taskCommandSize,
        (BYTE**)&pTaskCommandHeader );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't get input buffer from pending device command.");
        goto exit;
    }

    if ( taskCommandSize < sizeof(WFC_MESSAGE_METADATA) )
    {
        WFCActError(
            "Task command is too small. Expected >= %u, Actual = %u",
            (ULONG)sizeof(WFC_MESSAGE_METADATA),
            (ULONG)taskCommandSize );
        status = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    // Fill in TLV
    abortParams.CancelParameters.OriginalPortId = pTaskCommandHeader->Message.PortId;
    // Device command only contains the command header
    abortParams.CancelParameters.OriginalTaskOID = pTaskCommandHeader->Command()->MappedWdiOid;
    abortParams.CancelParameters.OriginalTransactionId = pTaskCommandHeader->Message.TransactionId;
    tlvCtx.AllocationContext = (ULONG_PTR)(&pnac);

    status = GenerateWdiAbortTask( &abortParams, sizeof( WFC_MESSAGE_METADATA ), &tlvCtx, &newCommandSize, &pbBuffer );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError( "Couldn't generate device command." );
        goto exit;
    }
    NT_ASSERT( newCommandSize <= c_CommandSize );
    NT_ASSERT( pbBuffer == m_CommandBuffer );

    status = DeviceCommand::Initialize(
        pTaskCommandHeader->Message.PortId,
        WDI_ABORT_TASK,
        newCommandSize,
        pbBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't initialize device command header.");
        goto exit;
    }

exit:
    TraceActExit(status);
    return status;
}
