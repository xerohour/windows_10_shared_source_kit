/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Pnp.hpp

Abstract:

    Header file for the Ndis Pnp related jobs

Environment:

    Kernel mode

Revision History:

--*/
#pragma once

/*++

This class can be used to do a synchronous wait for a job to complete.
When the job completes, this object would set the specified event and delete
itself. The caller MUST own the event and would wait for this event.

This object is allocated by the caller, but if freed by itself
--*/
class CJobCompleteNotifier : public IOperationCompletionCallback
{
public:
    CJobCompleteNotifier();

    NDIS_STATUS Initialize(
        _In_ PNDIS_STATUS pCompletionStatus,
        _In_ IEventContext* pCompletionEvent);

    void OnOperationCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_opt_ WFC_SENDER_OBJECT SenderObject );
    
private:
    // Completion event sent to the caller
    IEventContext* m_pCompletionEvent;

    // Completion status
    PNDIS_STATUS m_pCompletionStatus;
};

//==============================================================================================

typedef enum _CREATE_PORT_JOB_STATE
{
    CreatePortJobNotStarted,
    CreatePortJobWaitingForPortCreate,
    CreatePortJobWaitingForPortReset
} CREATE_PORT_JOB_STATE;

/*++

This class performs the create port job
--*/
class CCreatePortJob : public CJobBase
{
public:

    CCreatePortJob(
        _In_ WFC_ACTIVITY_ID ActivityId );

    ~CCreatePortJob();

    NDIS_STATUS InitializeFromJob(
        _In_ CAdapter* pAdapter,
        _In_ WFC_PORT_TYPE PortType,
        _In_ UINT16 OpmodeMask,
        _In_ NDIS_PORT_NUMBER CreatedNdisPortNumber,
        _In_ BOOLEAN bCreateForResume = FALSE );

    void OnJobStarted();
    
    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    void OnJobCancelled();

    WFC_PORT_ID GetCreatedWfcPortId()
    {
        return m_CreatedPortId;
    }

    WDI_MAC_ADDRESS GetMacAddress()
    {
        return m_CreatePortIndicationParameters.PortAttributes.MacAddress;
    }

private:
    NDIS_STATUS StartCreatePortTask();
    
    NDIS_STATUS CompleteCreatePortTask(
        _In_ NDIS_STATUS OperationStatus);

    NDIS_STATUS StartPortResetJob();

    NDIS_STATUS CompletePortResetJob(
        _In_ NDIS_STATUS OperationStatus);
        
    void FinishJob(
        _In_ NDIS_STATUS JobCompleteStatus);

protected:
    Task        m_CreateTask;
    NDIS_PORT_NUMBER m_CreatedNdisPortNumber;
    WFC_PORT_ID m_CreatedPortId;

    CAdapter    *m_pAdapter;

private:
    WFC_PORT_TYPE m_PortType;
    WDI_OPERATION_MODE m_uOpmodeMask;
    
    CDot11ResetJob  m_ResetJob;

    BOOLEAN m_bCreateForResume;
    CPort *m_pPortToResume;     // used for CreateForResume

    // Total buffer we would send down
    BYTE*        m_CommandBuffer;

    CREATE_PORT_JOB_STATE m_JobState;

    WDI_INDICATION_CREATE_PORT_COMPLETE_PARAMETERS m_CreatePortIndicationParameters;
};

//
// 
//
class CCreateMacOidJob : public COidJobBase
{
public:
    CCreateMacOidJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        COidJobBase(ActivityId),
        m_CreatePortChildJob(ActivityId),
        m_CreatePortOidState(CreatePortOidStateInit),
        m_CreatedNdisPortNumber(INVALID_NDIS_PORT_NUMBER),
        m_Dot11MacParamsOpmodeMask(0),
        m_OidIsCompleted(FALSE),
        m_AsyncCallbackEvent(true, ActivityId),
        m_pPortToRecreate( NULL )
    {

    }

    NDIS_STATUS CCreateMacOidJob::InitializeForResume(
        _In_ CPort * pPort  );


    // COidJobBase
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest
        );
    
    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus
        )
    {
        // no-op is ok
        UNREFERENCED_PARAMETER(NdisStatus);
    }
    
    // CJobBase
    void OnJobStarted();
    
    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );
        
    void OnJobCancelled() { /* no-op */ }

    NDIS_STATUS AllocateNdisPort(
        _Out_ PNDIS_PORT_NUMBER  AllocatedPortNumber);

    CCreatePortJob m_CreatePortChildJob;
    Event m_AsyncCallbackEvent;

    ULONG m_Dot11MacParamsOpmodeMask;

    enum
    {
        CreatePortOidStateInit = 0,
        CreatePortOidStateWaitingForCreatePortChildJob,
        CreatePortOidStateWaitingForWorkItemCallback,
    } m_CreatePortOidState;

    NDIS_PORT_NUMBER m_CreatedNdisPortNumber;
    BOOLEAN m_OidIsCompleted;

    //
    // Adopt from Morpheus resume with a dummy OID
    //
    CPort * m_pPortToRecreate;
    NDIS_OID_REQUEST m_NdisOidRequest;
    union {
        DOT11_MAC_PARAMETERS Dot11MacParams;
        DOT11_MAC_INFO Dot11MacInfo;
    } m_u;
};


//==============================================================================================

typedef enum _MINIPORT_INITIALIZE_JOB_STATE
{
    MiniportInitializeJobNotStarted,
    MiniportInitializeJobPerformingAllocateAdapter,
    MiniportInitializeJobWaitingForOpenTaskComplete,
    MiniportInitializeJobWaitingForFirmwareCapabilities,
    MiniportInitializeJobWaitingForFirmwareConfiguration,
    MiniportInitializeJobWaitingForDataPathQueryInterfaceComplete,
    MiniportInitializeJobWaitingForCreatePortComplete,
    MiniportInitializeJobWaitingForSetRadioStateComplete,
    MiniportInitializeJobSettingMiniportAttributes,
    MiniportInitializeJobStartingIHVOperations,
    MiniportInitializeJobSucceeded
} MINIPORT_INITIALIZE_JOB_STATE;

/*++
This class performs the miniport initialize job
--*/
class CMiniportInitializeJob : public CJobBase
/*++

    This job is also used by Resume firmware to resume from S4.
    Watch m_bInitToResumeFirmware value

--*/
{
public:

    CMiniportInitializeJob(
        _In_ WFC_ACTIVITY_ID ActivityId );

    virtual ~CMiniportInitializeJob();

    NDIS_STATUS Initialize(
        _In_ CAdapter *pAdapter,
        _In_ PNDIS_MINIPORT_INIT_PARAMETERS     MiniportInitParameters,
        _In_ PNDIS_WDI_INIT_PARAMETERS          NdisWdiInitParameters
        );

    void OnJobStarted();
    
    virtual void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    void OnJobCancelled();

protected:
    NDIS_STATUS CompleteOpenTask(
        _In_ NDIS_STATUS OperationStatus);

    NDIS_STATUS StartSetFirmwareConfiguration();

    NDIS_STATUS CompleteSetFirmwareConfiguration(
        _In_ NDIS_STATUS OperationStatus);

        NDIS_STATUS StartDataPathQueryInterface();

        NDIS_STATUS CompleteDataPathQueryInterface(
            _In_ NDIS_STATUS OperationStatus);

    NDIS_STATUS StartSetRadioStateTask(_In_ BOOLEAN bRadioOff);

    NDIS_STATUS StartCreatePortJob( _In_ BOOLEAN bCreateForResume = FALSE);

    NDIS_STATUS CompleteCreatePortJob(
        _In_ NDIS_STATUS OperationStatus);

    
    void FinishJob(
        _In_ MINIPORT_INITIALIZE_JOB_STATE CompletingStep,
        _In_ NDIS_STATUS JobCompleteStatus);

private:
    NDIS_STATUS PerformAllocateAdapter();

    NDIS_STATUS PerformIHVOperationsStart();
    
    virtual NDIS_STATUS StartOpenTask();

    NDIS_STATUS StartGetFirmwareCapabilities();
    
    NDIS_STATUS CompleteGetFirmwareCapabilities(
        _In_ NDIS_STATUS OperationStatus);

    NDIS_STATUS ApplyDriverCapabilities();

    NDIS_STATUS SetMiniportAttributes(_In_ bool bResetRecoveryRegistration);
    NDIS_STATUS SetHardwareAssistAttributes();
    ///void InitializeDataPathQICommandBuffer();

    NDIS_STATUS ApplyAdapterCapabilitiesOverride();
    NDIS_STATUS CheckForNetworkAddressChange(
        PWDI_MAC_ADDRESS pConfiguredMacAddress
        );

protected:
    MINIPORT_INITIALIZE_JOB_STATE m_JobState;
    CAdapter    *m_pAdapter;
    Task        m_Task; // from private so it can be used by ResumeJob
    WFC_MESSAGE_METADATA m_SimpleCommandBuffer; // ditto
    BOOLEAN                            m_bInitToResumeFirmware;

private:

    PNDIS_MINIPORT_INIT_PARAMETERS     m_MiniportInitParameters;
    PNDIS_WDI_INIT_PARAMETERS          m_NdisWdiInitParameters;

    // Job that handles port creation
    CCreatePortJob  m_CreatePortJob;

    // if the pesisted radio state is off, we set the software Radio to off
    // use the shared m_Task but our own command buffer
    BYTE*                   m_pRadioTlvBuffer;
    ULONG                   m_RadioTlvLength;

    DeviceCommand m_FirmwareCapabilitiesCommand;

    WDI_GET_ADAPTER_CAPABILITIES_PARAMETERS m_AdapterCapabilities;

    DeviceCommand m_FirmwareConfigurationCommand;
    BYTE* m_pFirmwareConfigurationCommandBuffer;

#if WDI_DISABLED    
    DATAPATH_QI_CMD_BUFFER m_DataPathQICommandBuffer;
    DeviceCommand m_DataPathQueryInterfaceCommand;
    CSetRadioStateJob m_SetRadioStateOffJob;
#endif // WDI_DISABLED    
};

//==============================================================================================
typedef enum _DELETE_PORT_JOB_STATE
{
    DeletePortJobNotStarted,
    DeletePortJobWaitingForStopBackgroundDiscovery,
    DeletePortJobWaitingForPortDelete,
    DeletePortJobWaitingForPortJobsCleanup,
} DELETE_PORT_JOB_STATE;

/*++

This class performs the create port job
--*/
class CDeletePortJob : public CJobBase
{
public:
    CDeletePortJob(
        _In_ WFC_ACTIVITY_ID ActivityId );

    ~CDeletePortJob();

    NDIS_STATUS InitializeFromJob(
        _In_ CAdapter*  pAdapter,
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _In_opt_ INotifyOperationStarted *pNotifyOperationStarted);

    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    void OnJobCancelled();

private:
    NDIS_STATUS StartDeletePortTask();

    NDIS_STATUS CompleteDeletePortTask(
        _In_ NDIS_STATUS OperationStatus);

    NDIS_STATUS CDeletePortJob::StartPortJobCleanup();

    void FinishJob(
        _In_ NDIS_STATUS JobCompleteStatus);

private:
    CAdapter *m_pAdapter;
    NDIS_PORT_NUMBER m_PortNumberToDelete;
    INotifyOperationStarted *m_pNotifyOperationStarted;

    CPort *m_pPortToDelete;
    Task m_Task;

    BYTE* m_CommandBuffer;

    DELETE_PORT_JOB_STATE m_JobState;

    CSetWFDStopBackgroundDiscovery m_StopBackgroundDiscoveryJob;
};

//==============================================================================================
typedef enum _MINIPORT_HALT_JOB_STATE
{
    MiniportHaltJobNotStarted,
    MiniportHaltStoppingIHVOperations,
    MiniportHaltJobDeletingAllPorts,    
    MiniportHaltJobWaitingForDeletePortComplete,
    MiniportHaltJobWaitingForCloseTaskComplete
} MINIPORT_HALT_JOB_STATE;

/*++

This class performs the miniport halt job
--*/
class CMiniportHaltJob : public CJobBase
{
public:

    CMiniportHaltJob(
        _In_ WFC_ACTIVITY_ID ActivityId );

    NDIS_STATUS Initialize(_In_ CAdapter *pAdapter);

    void OnJobStarted();
    
    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    void OnJobCancelled();

private:
    void PersistPropertiesToRegistry();    

    void PerformFreeAdapter();

    NDIS_STATUS PerformIHVOperationsStop();    

    NDIS_STATUS StartDeleteAllPorts();

    NDIS_STATUS StartDeletePortJob(
        _In_ CPort* pPort);

    NDIS_STATUS CompleteDeletePortJob(
        _In_ NDIS_STATUS OperationStatus);

    NDIS_STATUS DataPathReleaseInterface();

    NDIS_STATUS StartCloseTask();

    NDIS_STATUS CompleteCloseTask(
        _In_ NDIS_STATUS OperationStatus);

    void FinishJob(
    _In_ NDIS_STATUS JobCompleteStatus);

private:
    CAdapter    *m_pAdapter;

    Task        m_Task;
    WFC_MESSAGE_METADATA m_CommandBuffer; 

    // Job that handles port deletion
    CDeletePortJob  m_DeletePortJob;

    MINIPORT_HALT_JOB_STATE m_JobState;
    
    BOOLEAN m_bAreAllPortsDeleted;
};


class CPortJobNotifier;

/*++
Base class for port asynchronous routines
--*/
class CMiniportJobBase : public CJobBase
{
    friend CPortJobNotifier;

public:
    virtual NDIS_STATUS Initialize(
        _In_ CAdapter *pAdapter,
        _In_ WFC_JOB_TYPE eJobType);

    NDIS_STATUS QueueSerialized();

protected:

    CMiniportJobBase(
        _In_ WFC_ACTIVITY_ID ActivityId);

    virtual ~CMiniportJobBase();

    void OnJobStarted();
    
    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );

    void OnJobCancelled();

    NDIS_STATUS StartPortJobNext();

    virtual NDIS_STATUS PortJobCallback() = 0;
    virtual void PortOperationCompletedCallback (NDIS_STATUS OperationStatus) = 0;

protected:

    CAdapter *m_pAdapter;
    
    //
    // port that will be paused next
    //    
    UINT32 m_PortIndexNext;

    CPortJobNotifier *m_PortJobNotifier;
};

/*++
This class is used to do a synchronous wait for a pause job to complete.
--*/
class CPortJobNotifier : 
    public IOperationCompletionCallback
{
public:
    CPortJobNotifier(
        _In_ CMiniportJobBase * pPortJob)  :
    m_pPortJob(pPortJob)
    {
    }

    void OnOperationCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_opt_ WFC_SENDER_OBJECT SenderObject )
    {
        UNREFERENCED_PARAMETER (SenderObject);
        m_pPortJob->PortOperationCompletedCallback (OperationStatus);        
    }
    
private:

    CMiniportJobBase *m_pPortJob;

};

/*++
This class performs the miniport pause job
--*/
class CMiniportPauseJob : public CMiniportJobBase
{
public:

    CMiniportPauseJob (
        _In_ WFC_ACTIVITY_ID ActivityId);

    VOID SetPauseParameters(
        _In_ PNDIS_MINIPORT_PAUSE_PARAMETERS   PauseParameters 
        )
    {
        m_PauseParameters = PauseParameters;
    }

protected:

    virtual NDIS_STATUS PortJobCallback();
    void PortOperationCompletedCallback (
        _In_ NDIS_STATUS OperationStatus);    

private:
    PNDIS_MINIPORT_PAUSE_PARAMETERS   m_PauseParameters;
};

/*++
This class performs the miniport restart job
--*/
class CMiniportRestartJob : public CMiniportJobBase
{
public:

    CMiniportRestartJob (
        _In_ WFC_ACTIVITY_ID ActivityId);

    VOID SetRestartParameters(
        _In_ PNDIS_MINIPORT_RESTART_PARAMETERS       RestartParameters        
        )
    {
        m_RestartParameters = RestartParameters;
    }

protected:

    virtual NDIS_STATUS PortJobCallback();
    virtual void PortOperationCompletedCallback (
        _In_ NDIS_STATUS OperationStatus);


private:
    PNDIS_MINIPORT_RESTART_PARAMETERS       m_RestartParameters;

};

/*++
This class implements the pausing job for an individual port 
--*/
class CPortPauseJob : public CMiniportJobBase
{
public:

    CPortPauseJob (
        _In_ WFC_ACTIVITY_ID ActivityId);

protected:

    virtual NDIS_STATUS PortJobCallback();
    void PortOperationCompletedCallback (
        _In_ NDIS_STATUS OperationStatus)
    {
        UNREFERENCED_PARAMETER(OperationStatus);
    }
};

//------------------------------------------------
class CDeleteMacOidJob : public COidJobBase, 
                         public INotifyOperationStarted
{
public:
    CDeleteMacOidJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        COidJobBase(ActivityId),
        m_DeletePortChildJob(ActivityId),
        m_DeleteMacOidState(DeleteMacOidStateInit),
        m_DeleteNdisPortNumber(INVALID_NDIS_PORT_NUMBER),
        m_pPortAccessGate(NULL),
        m_OidIsCompleted(FALSE),
        m_AsyncCallbackEvent(true, ActivityId)
    {

    }

    ~CDeleteMacOidJob();

    // COidJobBase
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest);
    
    VOID OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus)
    {
        // no-op is ok
        UNREFERENCED_PARAMETER(NdisStatus);
    }
    
    void OnOperationStarted(
      _In_ NDIS_STATUS OperationStatus,
      _In_opt_ WFC_SENDER_OBJECT SenderObject );

    // CJobBase
    void OnJobStarted();
    
    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject );
        
    void OnJobCancelled() { /* no-op */ }

    CDeletePortJob m_DeletePortChildJob;
    Event m_AsyncCallbackEvent;
    CWFCExclusiveAsyncGate *m_pPortAccessGate;
    BOOLEAN m_OidIsCompleted;

    enum
    {
        DeleteMacOidStateInit = 0,
        DeleteMacOidStateWaitingForWorkItem,
        DeleteMacOidStateWaitingForPausePortChildJob,
        DeleteMacOidStateWaitingForClosePortGateStep,
        DeleteMacOidStateWaitingForDeletePortJob,
    } m_DeleteMacOidState;

    NDIS_PORT_NUMBER m_DeleteNdisPortNumber;

};

/*++
This class performs the miniport Surprise Removal job
--*/
class CMiniportSurpriseRemovalJob : public CMiniportJobBase
{
public:

    CMiniportSurpriseRemovalJob (
        _In_ WFC_ACTIVITY_ID ActivityId);

    NDIS_STATUS QueueJob(
        _In_  PNET_DEVICE_PNP_EVENT   NetDevicePnPEvent,    
        _In_ IOperationCompletionCallback *pCompletionCallback);

protected:

    virtual NDIS_STATUS PortJobCallback();
    void PortOperationCompletedCallback (
        _In_ NDIS_STATUS OperationStatus);    
};


//++ CResetRecoveryJob - handle OID_DOT11_ADAPTER_RESET_RECOVERY
class CResetRecoveryJob: public CSimpleSetOidJob
/*++

    Simple job to handle OID_DOT11_ADAPTER_RESET_RECOVERY.
    If this is blocked behind a command, the command will be timed out.
    It's best to let the latter happen so debug information is collect on the command

--*/
{
public:
    CResetRecoveryJob( _In_ WFC_ACTIVITY_ID ActivityId )
            : CSimpleSetOidJob(ActivityId) 
    {
        // This job must be running at passive IRQL
        CJobBase::set_RequiresPassiveIrql(true);
    };

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        );
};

