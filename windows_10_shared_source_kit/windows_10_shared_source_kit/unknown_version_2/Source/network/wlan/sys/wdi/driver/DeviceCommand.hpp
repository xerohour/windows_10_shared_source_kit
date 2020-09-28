#pragma once

class CJobBase;

class DeviceCommand : public IActivityId
{

public:
    //
    // Job-accessible APIs
    //
    DeviceCommand(
        _In_ WFC_ACTIVITY_ID ActivityId );
    virtual ~DeviceCommand();

    //
    // Input pointers in this case are owned by the caller. If device
    // command is queued into device command scheduler, memory for
    // pBuffer must not be freed until device command has completed.
    //
    NDIS_STATUS Initialize(
        _In_ WFC_PORT_ID                    PortId,
        _In_ ULONG                          CommandId,
        _In_ ULONG                           BufferLength,
        _In_reads_bytes_(BufferLength) BYTE* Buffer );

    void SetPending(
        bool IsPending );

    //
    // Returns local pointer to buffer result (not a copy). Buffer
    // lifetime is tied to DeviceCommand lifetime, and may not be
    // present if the command has failed.
    //
    NDIS_STATUS get_CommandResult(
        _Out_ ULONG *                                     pBufferLength,
        _Outptr_result_bytebuffer_(*pBufferLength) BYTE** pBuffer );

    NDIS_STATUS get_IsInitialized(
        _Out_ bool* pIsInitialized );

public:

    //
    // Device Command Scheduler APIs
    //
    NDIS_STATUS get_CommandToken(
        _Out_ WFC_COMMAND_TOKEN *pCommandToken );

    NDIS_STATUS get_PortId(
        _Out_ WFC_PORT_ID *pPortId );

    NDIS_STATUS get_CommandType(
        _Out_ ULONG *pCommandId );

    //
    // Gets a buffer local to this DeviceCommand, which is the
    // full M1 message buffer to the device.
    //
    NDIS_STATUS get_InputBuffer(
        _Out_ ULONG *                                     pBufferLength,
        _Outptr_result_bytebuffer_(*pBufferLength) BYTE** pBuffer );

    //
    // Device Command makes an internal copy of this buffer.
    //
    NDIS_STATUS set_CommandResult(
        _In_opt_ ULONG  BufferLength,
        _In_reads_bytes_opt_(BufferLength) BYTE* Buffer );

    IOperationCompletionCallback*  get_CompletionCallback()
    {
        return m_pCompletionCallback;
    }

    NDIS_STATUS set_CompletionCallback(
        _In_ IOperationCompletionCallback* pCompletionCallback );

    PLIST_ENTRY get_ListEntry();

    static DeviceCommand* get_DeviceCommandFromListEntry(
        _In_ PLIST_ENTRY pLink );

    void set_ParentJob(
        _In_opt_ CJobBase* pParentJob);

    CJobBase* get_ParentJob();

    Event                          m_FailsafeCompletionEvent;
    IOperationCompletionCallback*  m_pCompletionCallback;


protected:
    void Cleanup();
    static WFC_COMMAND_TOKEN s_get_NextCommandToken();

    ULONG                          m_CommandId;
    CPP_LIST_ENTRY                 m_Link;
    ULONG                          m_InputBufferLength;
    BYTE*                          m_InputBuffer;
    ULONG                          m_OutputBufferLength;
    BYTE*                          m_OutputBuffer;
    WFC_COMMAND_TOKEN             m_CommandToken;
    WFC_PORT_ID                   m_PortId;
    bool                           m_IsPending;
    bool                           m_IsInitialized;
    CJobBase*                      m_pParentJob;
    ULONGLONG                      m_DebugCommandSubmitTime;
};

class CancelDeviceCommand :
    public DeviceCommand
{
public:
    CancelDeviceCommand(
        _In_ WFC_ACTIVITY_ID ActivityId );

    NDIS_STATUS Initialize(
        _In_ DeviceCommand* pPendingDeviceCommand,
        _In_ CAdapter* pAdapter);

protected:
    //
    // Note: Cancel command buffer is 2 common headers stacked on top
    // of each other with a TLV between then. Top one is for "Abort" header, second one is
    // information about which command to cancel (same as the command
    // that was initially started input buffer header).
    //
    //
    static const UINT8 c_CommandSize = sizeof( WFC_MESSAGE_METADATA ) +
        WDI_TLV_HEADER_SIZE +
        sizeof( WDI_CANCEL_PARAMETERS_STRUCT );

    // Total buffer we would send down
    BYTE        m_CommandBuffer[c_CommandSize];

};
