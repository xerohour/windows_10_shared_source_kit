#pragma once

//
// Helper class for jobs that need to issue one or more property/task
// device commands to the device.
//
// NOTE: Whatever derives from this base class must also provide
// implementations for any relevant COidJobBase functions related to
// being called from the context of an OID.
//
// This is meant to reduce required boiler plate asynchronous handler
// code for sending commands to the device.
//
class CDeviceCommandOidJobBase : public COidJobBase
{
public:
    CDeviceCommandOidJobBase(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        COidJobBase(ActivityId),
        m_IsCancelled(false),
        m_TaskCommand(ActivityId),
        m_pDeviceCommand(NULL),
        m_pCurrentTlvBuffer(NULL),
        m_CurrentTlvBufferLength(0),
        m_CurrentCommandToken(NULL),
        m_CurrentCommandType(WfcDeviceCommandTypeInvalid),
        m_CurrentIoctl(0x0)
    {

    }

private:
    bool                     m_IsCancelled;
    WFC_DEVICE_COMMAND_TOKEN m_CurrentCommandToken;
    WFC_DEVICE_COMMAND_TYPE  m_CurrentCommandType;
    UINT32                   m_CurrentIoctl;
    ULONG                    m_CurrentTlvBufferLength;
    BYTE*                    m_pCurrentTlvBuffer;
    Task                     m_TaskCommand;
    DeviceCommand*           m_pDeviceCommand;

    void HandleNextDeviceCommand();
    void CompleteJobHelper(
        _In_ NDIS_STATUS OperationStatus );

protected:
    //
    // Pure virtual function call to query information about the next
    // command to send down to the device.
    //
    // Implementation can either request to send a property (M1-M3) or
    // task command (M1-M4).
    //
    // Return values are required to populate the command to the send
    // to the device.
    //
    // If this function returns NDIS_STATUS_SUCCESS:
    //
    // - The memory pointed to in *ppCommandBuffer _is owned by
    // caller_, and will be freed appropriately (on destruction, or
    // before issuing the next command).
    //
    // Note: *pCommandToken starts as NULL on the first call, and this
    // function may override the value to whatever it wants. The last
    // value this is set to will be passed back in subsequent calls to
    // HandleCommandCompletion and OnNotifyJobCompletion.
    //
    virtual NDIS_STATUS QueryNextCommandInfo(
        _In_ ULONG ReservedHeaderLength,
        _Inout_ WFC_DEVICE_COMMAND_TOKEN* pCommandToken,
        _Out_ WFC_DEVICE_COMMAND_TYPE* pCommandType,
        _Out_ UINT32* pIoctlCode,
        _Out_ ULONG* pCommandBufferLength,
        _Outptr_result_buffer_(*pCommandBufferLength) BYTE** ppCommandBuffer ) = 0;

    //
    // Function call to handle a command result.
    //
    // CommandToken is the last command token returned by QueryNextCommandInfo.
    //
    // When OperationStatus == NDIS_STATUS_SUCCESS:
    // If a task command is being completed, completion buffer will be the TLV buffer of the M4 message.
    // If a property command is being completed, completion buffer will be the TLV buffer of the M3 message.
    //
    // If this function returns NDIS_STATUS_SUCCESS, caller will
    // examine *pQueryNextCommand to decide whether or not to call
    // QueryNextCommandInfo again and send another command.
    //
    // Default implementation does not inspect contents of result
    // buffer, propagates OperationStatus failure code, and on
    // success, sets *pQueryNextCommand to false (only 1 command will
    // run!).
    //
    virtual NDIS_STATUS HandleCommandCompletion(
        _In_ WFC_DEVICE_COMMAND_TOKEN CommandToken,
        _In_ NDIS_STATUS OperationStatus,
        _In_ NDIS_STATUS WiFiStatus,
        _In_ ULONG CommandCompletionBufferLength,
        _In_reads_bytes_(CommandCompletionBufferLength) BYTE* pCommandCompletionBuffer,
        _Out_ bool* pQueryNextCommand );

    //
    // Function to notify that the CJobBase::CompleteJob is about to
    // be called.
    //
    // This function can be called:
    //   - On job cancellation
    //
    //   - On HandleCommandCompletion failure
    //
    //       NOTE: This implies either a device command failure or
    //             HandleCommandCompletion was unhappy with the result.
    //
    //   - After the last successful HandleCommandCompletion call with
    //   *pQueryNextCommand = false
    //
    // If this job is not started as an OID job, this is the last opportunity
    // before the destructor to do synchronous clean-up, or to set up
    // appropriate state on completion.
    //
    virtual void OnNotifyJobCompletion(
        _In_ WFC_DEVICE_COMMAND_TOKEN LastCommandToken,
        _In_ NDIS_STATUS OperationStatus ) = 0;
    
public:
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    void OnJobCancelled();
};
