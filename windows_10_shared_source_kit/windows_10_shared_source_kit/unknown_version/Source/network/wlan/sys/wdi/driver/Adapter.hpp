/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Adapter.hpp

Abstract:

    Header file for the Wi-Fi Class driver Adapter structure

Environment:

    Kernel mode

Revision History:

--*/
#pragma once


//==============================================================================================
class CPort;
class CMiniportPauseJob;
class CMiniportRestartJob;
class CMiniportSurpriseRemovalJob;

#define MAX_DIAGNOSTIC_FIRMWARE_BUFFER_SIZE 1024 // 1024 bytes 
#define MAX_DEFAULT_KEYS 4

//
// System-wide OEM settings to apply to each adapter
//
#define REG_KEY_WLANSVC_OEM L"\\REGISTRY\\MACHINE\\System\\CurrentControlSet\\Services\\Wlansvc\\parameters\\OEM\\SystemCapabilities"
#define REG_VALUE_OEM_PLDR L"WLANPlatformLevelDeviceResetSupported" // dword value ->bool
#define REG_VALUE_OEM_FLDR L"WLANFunctionLevelDeviceResetSupported" // dword value ->bool


// This is sequential order in which steps are performed
typedef enum _WFC_ADAPTER_INITIALIZE_STATE
{
    WfcAdapterInitializeNotStarted               = 0,
    WfcAdapterInitializeControlPathInitialized   = 0x00000001,
    WfcAdapterInitializeIhvAdapterAllocated      = 0x00000002,
    WfcAdapterInitializeOpenTaskCompleted        = 0x00000004,
    WfcAdapterInitializeDataPathInitialized      = 0x00000008,
    WfcAdapterInitializeDataPathStarted          = 0x00000010,
    WfcAdapterInitializeIhvOperationStarted      = 0x00000020,
    
    WfcAdapterInitializeCompleted                = 0x00008000
} WFC_ADAPTER_INITIALIZE_STATE;

typedef enum _WDI_USB_IDLE_STATE
{
    WdiUsbIdleStateInit             = 0, // initial state, non-usb stay here
    WdiUsbIdleStatePending,
    //WdiUsbIdleStateForcePending,
    WdiUsbIdleStateConfirm,
    //WdiUsbIdleStateForceConfirm
} WDI_USB_IDLE_STATE;

class CAdapter : 
    public INdisConversion, 
    public IPropertyCacheDirectory,
    public INotifyDeviceIndicationCallback,
    public IOperationCompletionCallback,
    public IWorkItemCallback
{
public:
    CAdapter(
        _In_ CMiniportDriver* MiniportDriver
        );

    ~CAdapter();

    NDIS_STATUS Initialize(
        _In_ NDIS_HANDLE MiniportAdapterHandle,
        _In_ PNDIS_MINIPORT_INIT_PARAMETERS     MiniportInitParameters,
        _In_ PNDIS_WDI_INIT_PARAMETERS          NdisWdiInitParameters        
        );

    VOID Halt(
        _In_  NDIS_HALT_ACTION        HaltAction        
        );


    NDIS_HANDLE GetMiniportAdapterHandle()
    {
        return m_MiniportAdapterHandle;
    }

    NDIS_HANDLE GetMiniportAdapterContext()
    {
        return m_MiniportAdapterContext;
    }

    VOID SetMiniportAdapterContext(
        _In_ NDIS_HANDLE MiniportAdapterContext)
    {
        m_MiniportAdapterContext = MiniportAdapterContext;
    }

    TAL_TXRX_HANDLE GetMiniportTalTxRxContext()
    {
        return m_MiniportTalTxRxContext;
    }

    VOID SetMiniportTalTxRxContext(
        _In_ TAL_TXRX_HANDLE MiniportTalTxRxContext)
    {
        m_MiniportTalTxRxContext = MiniportTalTxRxContext;
    }

    CMiniportDriver* GetMiniportDriver()
    {
        return m_MiniportDriver;
    }
    
    PLIST_ENTRY get_AdapterListEntry()
    {
        return &m_Link.ListEntry;
    }

    EventQueue* GetEventQueue()
    {
        return &m_EventQueue;
    }
    
    DeviceCommandScheduler* GetDeviceCommandScheduler()
    {
        return &m_CommandScheduler;
    }
    
    NotificationManager* GetNotificationManager()
    {
        return &m_NotificationManager;
    }
    
    ActiveJobsList* GetActiveJobsList()
    {
        return &m_ActiveJobsList;
    }
    
    SerializedJobList* GetSerializedJobList()
    {
        return &m_SerializedJobList;
    }

    BOOLEAN GetFirmwareOpenState()
    {
        return m_FirmwareOpenState;
    }

    VOID SetFirmwareOpenState( 
        _In_ BOOLEAN OpenState )
    {
        m_FirmwareOpenState = OpenState;
    }

    void UpdateRrHistory();

    VOID ReenumerateFailedAdapter();

    BOOLEAN IsRecoverable()
    {
        return (m_IsRecoverable);
    }

    NDIS_STATUS SetFirmwareCapabilitiesProperties(
        _In_ WDI_GET_ADAPTER_CAPABILITIES_PARAMETERS  *firmwareCapabilities);

    PWFC_DATAPATH_CAPABILITIES GetDatapathCapabilities()
    {
        return &m_DatapathCapabilities;
    }
    
    PNDIS_PM_CAPABILITIES GetPmCapabilities()
    {   
        return &m_PmCapabilities;
    }
    PNDIS_RECEIVE_FILTER_CAPABILITIES GetReceiveFilterCapabilities()
    {
        return &m_ReceiveFilterCapabilities;
    }

    void SetDatapathCapabilities( _In_ PWFC_DATAPATH_CAPABILITIES pDatapathCapabilities)
    {
        RtlCopyMemory( &m_DatapathCapabilities, pDatapathCapabilities, sizeof(*pDatapathCapabilities));
    }

    __inline PWFC_FRAME AllocateWfcFrame();

    __inline void FreeWfcFrame(
        _In_ PWFC_FRAME pWfcFrame);

    void SetPmCapabilities( _In_ PNDIS_PM_CAPABILITIES pPmCapabilities )
    {   
        RtlCopyMemory( &m_PmCapabilities, pPmCapabilities, sizeof(*pPmCapabilities));
    }
    void SetReceiveFilterCapabilities(
        _In_ PNDIS_RECEIVE_FILTER_CAPABILITIES pReceiveFilterCapabilities)
    {
        RtlCopyMemory( 
            &m_ReceiveFilterCapabilities, 
            pReceiveFilterCapabilities, 
            sizeof(*pReceiveFilterCapabilities));
    }

    NDIS_STATUS AddPort(_In_ CPort *pPort)
    {
        for (UINT8 i =0; i < c_MaxPorts; i++)
        {
            if (m_pPortList[i] == NULL)
            {
                m_pPortList[i] = pPort;
                return NDIS_STATUS_SUCCESS;
            }
        }

        return NDIS_STATUS_OPEN_LIST_FULL;
    }
    
    NDIS_STATUS RemovePort(_In_ CPort *pPort)
    {
        if (!pPort)
        {
            return NDIS_STATUS_INVALID_PARAMETER;
        }

        for (UINT8 i =0; i < c_MaxPorts; i++)
        {
            if (m_pPortList[i] == pPort)
            {
                m_pPortList[i] = NULL;
                return NDIS_STATUS_SUCCESS;
            }
        }

        return NDIS_STATUS_FILE_NOT_FOUND;
    }

    NDIS_STATUS ConvertNdisPortNumberToPortId(
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _Out_ WFC_PORT_ID *pPortId);

    NDIS_STATUS ConvertPortIdToNdisPortNumber(
        _In_ WFC_PORT_ID PortId,
        _Out_ NDIS_PORT_NUMBER *pNdisPortNumber);

    NDIS_STATUS GetNdisPortNumberForPortType(
        _In_ WFC_PORT_TYPE PortType,
        _Out_ NDIS_PORT_NUMBER *pNdisPortNumber);

    NDIS_STATUS GetPortTypeFromPortId(
        _In_ WFC_PORT_ID PortId,
        _Out_ WFC_PORT_TYPE *pPortType);

    NDIS_STATUS GetPortIdForPortType(
        _In_ WFC_PORT_TYPE PortType,
        _Out_ WFC_PORT_ID *pPortId);

    CPort* GetPortFromPortId(
        _In_ WFC_PORT_ID PortId);
   
    CPort* GetPortFromNdisPortNumber(
        _In_ NDIS_PORT_NUMBER NdisPortNumber);

    CPort* GetPortFromPortIdAndLock(
        _In_ WFC_PORT_ID PortId,
        _Out_ CWFCExclusiveAsyncGate **ppPortLockOut);

    BOOLEAN IsOperationalPowerState();

    CPort* GetPortFromNdisPortNumberAndLock(
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _Out_ CWFCExclusiveAsyncGate **ppPortLockOut);

    CBSSListManager* GetBSSListManager(_In_ WFC_PORT_TYPE PortType)
    {
        if ( PortType == WfcPortTypeExtSTA )
        {
            return &m_ExtStaBSSList;
        }
        else if ( PortType == WfcPortTypeWFDDevice )
        {
            return &m_DevicePortBSSList;
        }

        NT_ASSERT(FALSE);
        return NULL;
    }

    CServicesManager* GetServicesManager()
    {
        return &m_ServicesManager;
    }

    NDIS_STATUS PersistStateInRegistry();

    IConnectedPeerList* GetConnectedPeerList(_In_ WFC_PORT_ID WfcPortId);

public:
    // IPropertyCacheDirectory
    CPortPropertyCache* GetPortPropertyCache(
        _In_ WFC_PORT_ID PortId
        );

    CAdapterPropertyCache* GetAdapterPropertyCache()
    {
        return &m_AdapterPropertyCache;
    }

    //
    // This function persists software radio power.
    // Callers of this function should update the property cache to keep cache consistency.
    // When Adapter initializes, it reads all properties including this and
    // fill them in proprty cache.  So we know where we should be accross reboot.
    //
    NDIS_STATUS SetPersistentSoftwareRadioOffState(
        _In_ BOOLEAN    IsRadioOff );

public:
    NDIS_STATUS HandleOidRequest(
        _In_ PNDIS_OID_REQUEST  NdisRequest,
        _In_ BOOLEAN            DirectOid);
        

    VOID CompleteOidRequest(
        _In_  PNDIS_OID_REQUEST pNdisOidRequest,
        _In_  NDIS_STATUS       CompletionStatus
        );

    VOID IndicateStatus(
        _In_  NDIS_PORT_NUMBER  NdisPortNumber,
        _In_  NDIS_STATUS       StatusCode,
        _In_opt_  PVOID         RequestID,
        _In_  PVOID             pStatusBuffer,
        _In_  ULONG             StatusBufferSize
        );

    VOID TriggerControlPathDiagnostics(
        _In_ ULONG TriggeringCommand);

    VOID TraceLoggingDiagnosisDuration(
        _In_ ULONG ulMs );

    VOID TraceLoggingResetRecovery( 
        _In_ ULONG EventCode,
        _In_ ULONG CommandCode,
        _In_ ULONG rrCount,
        _In_ ULONG OemPldrSupported,
        _In_ BOOLEAN bResetRecoveryOff );

    NDIS_STATUS InitializeDataPath();

    NDIS_STATUS StartDataPath();
    //called as part of Halt job to deinit the NIC in LE
    VOID CleanupDataPath();

    // IOperationCompletionCallback
    void OnOperationCompleted(
      _In_ NDIS_STATUS OperationStatus,
      _In_opt_ WFC_SENDER_OBJECT SenderObject );

    // INotifyDeviceIndicationCallback
    void OnDeviceIndicationArrived(
        _In_ WDI_INDICATION_TYPE                 IndicationType,
        _In_ WFC_COMMAND_TOKEN                   CommandToken,
        _In_ WFC_PORT_ID                         PortId,
        _In_ ULONG                               EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    NDIS_STATUS RegisterForAdapterNotifications();
    NDIS_STATUS DeregisterFromAdapterNotifications();

    VOID
    OnOidRequestCompleteFromMiniport(
        _In_  PNDIS_OID_REQUEST       Request,
        _In_  NDIS_STATUS             Status
        );

    VOID
    OnStatusIndicationFromMiniport(
        _In_ PNDIS_STATUS_INDICATION  StatusIndication
        );

    NDIS_STATUS CAdapter::ControlResetRecovery();

    NDIS_STATUS CAdapter::SimulateFirmwareHang(
        _In_ ULONG HungResetRecoveryIterations );

    void OnOpenAdapterCompleted(
        _In_ NDIS_STATUS              CompletionStatus
        );
        
    void OnCloseAdapterCompleted(
        _In_ NDIS_STATUS              CompletionStatus
        );


public:
    // Helpers to help cleanup steps
    VOID MarkInitComplete(
        _In_ WFC_ADAPTER_INITIALIZE_STATE CompletedState
        );
    VOID UnmarkInitComplete(
        _In_ WFC_ADAPTER_INITIALIZE_STATE RollbackState
        );
    BOOLEAN IsInitComplete(
        _In_ WFC_ADAPTER_INITIALIZE_STATE StateToCheck
        );

    VOID SetResumeRestoreState( _In_ bool ResumeRestoreInProgress)
    {
        m_ResumeRestoreInProgress = ResumeRestoreInProgress;
    }

    bool IsResumeRestoreInProgress()
    {
        return m_ResumeRestoreInProgress;
    }

    WDI_USB_IDLE_STATE SetUsbIdleState( _In_ WDI_USB_IDLE_STATE UsbIdleState )
    {
        WDI_USB_IDLE_STATE oldState = m_UsbIdleState;
        m_UsbIdleState = UsbIdleState;
        return oldState;
    }

    WDI_USB_IDLE_STATE GetUsbIdleState()
    {
        return m_UsbIdleState;
    }

    bool Busy()
    /*++
        This is only queried when NDIS idle notification comes down.
        There should be no outstanding OID.
        We are only concerned with WDI internnaly generated activities (jobs).
        If LE is busy, callback to LE will tell us so.
    --*/
    {
        bool bBusy = true;

        //
        // is Serialize job queue empty
        //
        if ( !GetSerializedJobList()->JobListEmpty() )
        {
            goto exit;
        }

        //
        // is Active job queue empty
        //
        if ( !GetActiveJobsList()->JobListEmpty() )
        {
            goto exit;
        }

        //
        // no pending timers; not needed(?), Timer belongs to Job
        //
        // if ( m_nPendingTimer  != 0 ) goto exit;

        bBusy = false;
    exit:
        return bBusy;
    }

    NDIS_STATUS ReadRegDword(
            _In_ PWSTR PathName,

            _In_ PWSTR ValueName,
            _Inout_ DWORD * pData);

private:

    WFC_REGISTRATION_TOKEN m_EventRegistrationToken;

    void OnRadioStateIndication(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnFirmwareStalledIndication(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

    void OnIhvTaskRequestReceived(
        _In_ ULONG EventBufferLength,
        _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer );

private:
    CPP_LIST_ENTRY  m_Link;

    NDIS_HANDLE     m_MiniportAdapterHandle;
    NDIS_HANDLE     m_MiniportAdapterContext;
    CMiniportDriver* m_MiniportDriver;

    TAL_TXRX_HANDLE m_MiniportTalTxRxContext;

    CAdapterPropertyCache      m_AdapterPropertyCache;

    CCtlPlane                  m_CtlPlane;
    CGlobalCtlDevice*          m_pCtlDevice;
    EventQueue                 m_EventQueue;
    SerializedJobList          m_SerializedJobList;
    ActiveJobsList             m_ActiveJobsList;
    NotificationManager        m_NotificationManager;
    DeviceCommandScheduler     m_CommandScheduler;

    COidJobNotificationHandler m_OidJobNotificationHandler;

    CBSSListManager            m_ExtStaBSSList;
    CBSSListManager            m_DevicePortBSSList;

    ULONGLONG                      m_DebugDiagnosticsTriggerTime;
    ULONG                          m_DebugDiagnosticsTriggerCommand;

    PNDIS_OID_REQUEST           m_DebugLastOidRequest;
    NDIS_OID                    m_DebugLastOidValue;
    NDIS_STATUS                 m_DebugLastOidStatus;
    ULONGLONG                   m_DebugLastOidSubmitTime;
                                // The following three fields are dumped into the live dump and contain debug information
    UCHAR                       m_DebugHungDeviceCommand[sizeof(DeviceCommand)]; // contains the contents of CJobBase
    UCHAR                       m_DebugHungTask[sizeof(Task)]; // contains the contents of CJobBase
    UCHAR                       m_DebugHungJob[sizeof(CJobBase)]; // contains the contents of CJobBase
    CServicesManager            m_ServicesManager;

public:
    ULONG                       m_scanCount; // control instant connect at boot/resume
    WDI_USB_IDLE_STATE          m_UsbIdleState;
    PDOT11_CIPHER_DEFAULT_KEY_VALUE m_DefaultKeyValues[MAX_DEFAULT_KEYS];
    ULONG                           m_DefaultKeyLengths[MAX_DEFAULT_KEYS];
    ULONG                       m_DefaultKeyId;
    UCHAR                       m_DebugFirmwareBuffer[MAX_DIAGNOSTIC_FIRMWARE_BUFFER_SIZE];
private:
    NDIS_DEVICE_POWER_STATE     m_AdapterPowerState;
    BOOLEAN                     m_FirmwareOpenState;
    NDIS_PM_CAPABILITIES                             m_PmCapabilities;
    NDIS_RECEIVE_FILTER_CAPABILITIES                 m_ReceiveFilterCapabilities;
    WFC_DATAPATH_CAPABILITIES                        m_DatapathCapabilities;


    ULONG                       m_InitializationState;
    bool m_AdapterFailed;
    bool m_IsRecoverable;
    bool m_ResumeRestoreInProgress;

    IOperationCompletionCallback *m_PendingInitHaltJob;
private:
    //
    // Background Scan Heuristics
    //
    ULONGLONG m_NextAllowedScanTime;

public:
    ULONGLONG GetNextAllowedScanTime()
    {
        return m_NextAllowedScanTime;
    }

    void SetNextAllowedScanTime(
        _In_ ULONGLONG RelativeTimeMs )
    {
        ULONGLONG targetTime = CSystem::get_CurrentTime() +
            ( RelativeTimeMs * 10000 ); /* millisecond --> 100ns conversion */

        if ( targetTime > m_NextAllowedScanTime )
        {
            m_NextAllowedScanTime = targetTime;
        }
    }

private:
    //
    // The interface Guid associated with the miniport adapter 
    // to include in telemetry events
    //
    GUID m_NetGuid;

public:
    const GUID & GetNetGuid() const
    {
        return m_NetGuid;
    }

public:
    static const UINT8 c_MaxPorts = WFC_CONSTANT::MaximumNumberOfPorts;

    // List of ports. There might be gaps in this 
    // array, so every element needs to be looked at
    // and we dont really need to track count. The index
    // in this list is NOT the port ID
    CPort*  m_pPortList[c_MaxPorts];
    CWFCExclusiveAsyncGate  m_PortLockList[c_MaxPorts];

    CMiniportPauseJob            *m_MiniportPauseJob;
    CMiniportSurpriseRemovalJob  *m_MiniportSurpriseRemovalJob;
    CMiniportRestartJob          *m_MiniportRestartJob;

    NPAGED_LOOKASIDE_LIST     m_WfcFrameLookaside;
    bool m_WfcFrameLookasideInitialized;
    ULONG       m_WfcFrameSize;
    LONG        m_lResetTriggerCount;       // How many times has this been detected
    LONG        m_lResetRecovery;           // How many times have we attempted to do something for Reset
    LONG        m_lSurpriseRemove;
    LONG        m_lShutdown;
    DWORD       m_OemPLDRSupported; // 0->disabled, otherwise ->enabled
    DWORD       m_OemFLDRSupported; // ditto

    CFrameIdLookupTable        m_FrameIdTable;
    CPeerTable                 m_PeerTable;
    CTxMgr                     m_TxMgr;
    CRxMgr                     m_RxMgr;

    HANDLE          m_NICContext; //lower edge adapter (datapath) 

    NDIS_MINIPORT_WDI_DATA_HANDLERS  m_MiniportDataHandlers;
    UINT32          m_MiniportWdiFrameMetadataExtraSpace;
    NDIS_WDI_DATA_API                m_MiniportDataApi;

    VOID SendNetBufferLists(
        _In_ PNET_BUFFER_LIST   NetBufferList,
        NDIS_PORT_NUMBER   PortNumber,
        ULONG              SendFlags);

    VOID CancelSendNetBufferLists(
        _In_  PVOID  CancelId);

    NDIS_STATUS CAdapter::IndicateFrames(
        _In_ WFC_PORT_ID PortId,
        _In_ PNET_BUFFER_LIST pNblChain,
        _In_ UINT32 NumFrames,
        _In_ ULONG ReceiveFlags );

    NDIS_STATUS SendCompleteNbl(
        _In_ PNET_BUFFER_LIST pNbl);

#if WDI_DISABLED    
    IDatapathCallbacks  m_DataPathCallbacks;
    IDatapathFunctions  m_DataPathFunctions;    
#endif // WDI_DISABLED

    VOID ReturnNetBufferLists(
        _In_  PNET_BUFFER_LIST NetBufferLists,
        _In_  ULONG            ReturnFlags);

    NDIS_STATUS CopyNetBufferLists(
        _In_                          PNET_BUFFER_LIST  NetBufferLists,
        _Outptr_result_nullonfailure_ PNET_BUFFER_LIST *pCopiedNetBufferLists,
        _In_                          ULONG             DataBackFill,
        _In_                          bool              bAllocateWfcFrame);

    VOID FreeCopiedNetBufferLists(
        _In_                          PNET_BUFFER_LIST  CopiedNetBufferLists );

    NDIS_STATUS Pause(
        _In_  PNDIS_MINIPORT_PAUSE_PARAMETERS MiniportPauseParameters );
    
    NDIS_STATUS HandlePnPEventNotify(
        _In_  PNET_DEVICE_PNP_EVENT   NetDevicePnPEvent
        );

    NDIS_STATUS Restart(
        _In_ PNDIS_MINIPORT_RESTART_PARAMETERS   MiniportRestartParameters );

    VOID SetAdapterPowerState( NDIS_DEVICE_POWER_STATE PowerState )
    {
        m_AdapterPowerState = PowerState;
    }
    
    NDIS_DEVICE_POWER_STATE GetAdapterPowerState( )
    {
        return m_AdapterPowerState;
    }

    VOID CollectDebugData(
        _In_opt_ DeviceCommand *pDeviceCommand, 
        _In_opt_ Task *pTask, 
        _In_opt_ CJobBase *job
        );
        
    VOID CollectFirmwareDiagnosticData( _In_ ULONG BufferLength, _In_reads_bytes_(BufferLength)BYTE* Buffer);

private:
    NDIS_STATUS InitializeControlPath();

    VOID CleanupControlPath(
        _In_ bool   WaitForEventQueueToEmpty
        );

    NDIS_STATUS PreprocessOid(
        _In_  PNDIS_OID_REQUEST     NdisOidRequest,
        _Out_ POID_HANDLER_ENTRY    *ppOidHandler,
        _Out_ WFC_JOB_TYPE         *pHandlerJobType,
        _Out_ BOOLEAN               *pSerializedJob
        );


    VOID SetDataPathApis();

    NDIS_STATUS GetBandInformation(
        _In_ WDI_GET_ADAPTER_CAPABILITIES_PARAMETERS* pAdapterCapabilities,
        _Inout_ PULONG  puNumBands,
        _Inout_ PULONG  puBytesWritten,
        _Inout_ PULONG  puBufferSize,
        _Out_writes_bytes_opt_(*puBufferSize) PDOT11_BAND_CAPABILITIES pBandCapabilities
        );

    NDIS_STATUS SaveEnhancedCapabilities(
        _In_ WDI_GET_ADAPTER_CAPABILITIES_PARAMETERS* pAdapterCapabilities
        );

    void InitializeNPagedLookaside(
        _In_ ULONG ulSize,
        _In_ PNPAGED_LOOKASIDE_LIST pLookaside);

public:
    //
    // State for talking to the TLV parser/generator
    //
    __inline PCTLV_CONTEXT GetTlvContext()
    {
        return &m_TlvContext;
    }

    CNdisSpinLock* m_ISpinLockDelayDeactivation;
    CNdisSpinLock* m_ISpinLockDelayActivation;
    NDIS_PORT_NUMBER m_NdisPortNumToDeactivate[c_MaxPorts]; // nerver be port0
    NDIS_PORT_NUMBER m_NdisPortNumToActivate[c_MaxPorts];   // never be port 0
    IWorkItemContext *m_pWorkItemDeactivatePort;
    IWorkItemContext *m_pWorkItemActivatePort;
    BOOLEAN m_bDeactivateWorkerActive;
    BOOLEAN m_bActivateWorkerActive;

    void DelayDeactivateAndFreePort();
    void DelayActivatePort();

    //
    // IWorkItem impl.
    //
    void OnWorkItemCallback(
        _In_opt_ WFC_WORK_ITEM_CONTEXT WorkItemContext )
    {
         if ( m_pWorkItemDeactivatePort == (HANDLE)WorkItemContext)
         {
            DelayDeactivateAndFreePort();
         }
         else if ( m_pWorkItemActivatePort == (HANDLE)WorkItemContext)
         {
            DelayActivatePort();
         }
         else
         {
            NT_ASSERT(FALSE);
         }
     }

private:
    TLV_CONTEXT m_TlvContext;
};

#if WDI_DISABLED
//
// Datapath Inteface to Lower edge: datapath callbacks
//

NTSTATUS AdapterSendComplete(
    _In_ HANDLE AdapterContext,
    _In_ WFC_PORT_ID PortId,
    _In_ PTX_DESCRIPTOR pTxD);

NTSTATUS AdapterRequestReset(_In_ HANDLE AdapterContext);

#endif // WDI_DISABLED
