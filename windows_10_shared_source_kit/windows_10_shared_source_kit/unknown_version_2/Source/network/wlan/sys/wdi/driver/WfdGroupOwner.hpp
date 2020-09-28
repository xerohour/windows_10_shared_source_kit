#pragma once

class CSetAPGroupStartParametersJob :
    public CPropertyCacheSetBufferOidJob
{
public:
    CSetAPGroupStartParametersJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CPropertyCacheSetBufferOidJob(
            WfcPortPropertyP2PGroupStartParameters,
            ActivityId ) { }
};

class CSetWpsEnabledJob :
    public CSimpleSetDeviceCommandOidJob
{
public:
    CSetWpsEnabledJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CSimpleSetDeviceCommandOidJob(ActivityId)
    {
    }

public:
    NDIS_STATUS GetDeviceCommandData(
        _In_ PVOID             pInformationBuffer,
        _In_ UINT              InformationBufferLength,
        _In_ PUINT             pBytesRead,
        _Inout_ PUINT          pBytesNeeded,
        _In_ UINT              ReservedHeaderLength,
        _Out_ ULONG*           pCommandId,
        _Out_ ULONG*           pCommandBufferLength,
        _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer);

    NDIS_STATUS OnDeviceCommandCompletion(
        _In_ NDIS_STATUS OperationStatus,
        _In_ ULONG MessageLength,
        _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader );

protected:
    WDI_SET_P2P_WPS_ENABLED_PARAMETERS m_WpsEnabled;
};

class CSetEndDwellOnChannel :
    public CSimpleSetDeviceCommandOidJob
{
public:
    CSetEndDwellOnChannel(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CSimpleSetDeviceCommandOidJob(ActivityId)
    {
    }

public:
    NDIS_STATUS GetDeviceCommandData(
        _In_ PVOID             pInformationBuffer,
        _In_ UINT              InformationBufferLength,
        _In_ PUINT             pBytesRead,
        _Inout_ PUINT          pBytesNeeded,
        _In_ UINT              ReservedHeaderLength,
        _Out_ ULONG*           pCommandId,
        _Out_ ULONG*           pCommandBufferLength,
        _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer);

    NDIS_STATUS OnDeviceCommandCompletion(
        _In_ NDIS_STATUS OperationStatus,
        _In_ ULONG MessageLength,
        _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader );

protected:
    WDI_SET_END_DWELL_TIME_PARAMETERS   m_EndDwellTime;
};

class CStartAPJob : public CDeviceCommandOidJobBase
{
protected:
    bool m_LegacyAP;
    WDI_TASK_START_AP_PARAMETERS m_TaskParameters;

    WDI_AUTH_ALGORITHM m_authAlgoList[1]; 
    WDI_CIPHER_ALGORITHM m_unicastCipherAlgoList[1]; 
    WDI_CIPHER_ALGORITHM m_multicastCipherAlgoList[1]; 

public:
    CStartAPJob(
        _In_ bool LegacyAP,
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CDeviceCommandOidJobBase(ActivityId),
        m_LegacyAP(LegacyAP)
    {
        m_authAlgoList [0] = WDI_AUTH_ALGO_RSNA_PSK;            // DOT11_AUTH_ALGO_RSNA_PSK;
        m_unicastCipherAlgoList [0] = WDI_CIPHER_ALGO_CCMP;     // DOT11_CIPHER_ALGO_CCMP;                                              
        m_multicastCipherAlgoList [0] = WDI_CIPHER_ALGO_CCMP;   // DOT11_CIPHER_ALGO_CCMP;                                              
    }

    ~CStartAPJob()
    {

    }

public:
    //
    // COidJobBase Implementation
    //
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest );

    void OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus );

protected:
    //
    // CDeviceCommandOidJobBase Implementation
    //
    NDIS_STATUS QueryNextCommandInfo(
        _In_ ULONG ReservedHeaderLength,
        _Inout_ WFC_DEVICE_COMMAND_TOKEN* pCommandToken,
        _Out_ WFC_DEVICE_COMMAND_TYPE* pCommandType,
        _Out_ UINT32* pMessageID,
        _Out_ ULONG* pCommandBufferLength,
        _Outptr_result_buffer_(*pCommandBufferLength) BYTE** ppCommandBuffer );

    void OnNotifyJobCompletion(
        _In_ WFC_DEVICE_COMMAND_TOKEN LastCommandToken,
        _In_ NDIS_STATUS OperationStatus );

    NDIS_STATUS GetCipherForGO(
        _In_ UINT32 elementCount,
        _In_reads_(elementCount) WDI_CIPHER_ALGORITHM *pCipherAlgoList,
        _Out_ WDI_CIPHER_ALGORITHM *pCipherAlgo );
};


class CSendAssociationResponseJob : public CDeviceCommandOidJobBase
{
public:
    CSendAssociationResponseJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CDeviceCommandOidJobBase(ActivityId),
        m_pSendCompleteIndication(NULL)
    {

    }

    ~CSendAssociationResponseJob()
    {
        if ( m_pSendCompleteIndication != NULL )
        {
            delete [] m_pSendCompleteIndication;
        }
    }

public:
    //
    // COidJobBase Implementation
    //
    NDIS_STATUS InitializeFromOidRequest(
        _In_ PNDIS_OID_REQUEST pOidRequest );

    void OnOidJobCompletedEvent(
        _In_ NDIS_STATUS NdisStatus );

public:
    //
    // CDeviceCommandOidJobBase implementation
    //
    NDIS_STATUS QueryNextCommandInfo(
        _In_ ULONG ReservedHeaderLength,
        _Inout_ WFC_DEVICE_COMMAND_TOKEN* pCommandToken,
        _Out_ WFC_DEVICE_COMMAND_TYPE* pCommandType,
        _Out_ UINT32* pMessageID,
        _Out_ ULONG* pCommandBufferLength,
        _Outptr_result_buffer_(*pCommandBufferLength) BYTE** ppCommandBuffer );

    NDIS_STATUS HandleCommandCompletion(
        _In_ WFC_DEVICE_COMMAND_TOKEN CommandToken,
        _In_ NDIS_STATUS OperationStatus,
        _In_ NDIS_STATUS WiFiStatus,
        _In_ ULONG CommandCompletionBufferLength,
        _In_reads_bytes_(CommandCompletionBufferLength) BYTE* pCommandCompletionBuffer,
        _Out_ bool* pQueryNextCommand );

    void OnNotifyJobCompletion(
        _In_ WFC_DEVICE_COMMAND_TOKEN LastCommandToken,
        _In_ NDIS_STATUS OperationStatus );


protected:
    PVOID m_OidRequestId;
    NDIS_PORT_NUMBER m_OidPortNumber;

    NDIS_STATUS m_SendCompleteIndicationType;
    ULONG m_SendCompleteIndicationLength;
    BYTE* m_pSendCompleteIndication;

    WDI_TASK_SEND_AP_ASSOCIATION_RESPONSE_PARAMETERS m_ResponseParameters;
};

class CAPResetJob : public COidJobBase
{
public:
    CAPResetJob (_In_ WFC_ACTIVITY_ID ActivityId) :
         m_ResetState (APResetStateInit),
         m_StopApTask (ActivityId),
         m_pCommandBuffer (NULL),
         COidJobBase (ActivityId)
    {
    }

    CAPResetJob::~CAPResetJob()
    {
        if ( m_pCommandBuffer )
        {
            FreeGenerated( m_pCommandBuffer );
        }
    }

public:
    NDIS_STATUS InitializeFromJob (
        _In_ CAdapter* pAdapter,
        _In_ NDIS_PORT_NUMBER m_NdisPortNumber );

public:

    void OnJobCancelled()
    {

    }

    void OnJobStarted ();

    void OnJobStepCompleted (
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);
    
    NDIS_STATUS InitializeFromOidRequest (
        _In_ PNDIS_OID_REQUEST);
   
    void OnOidJobCompletedEvent (
        _In_ NDIS_STATUS);
   
protected:
    
    enum
    {
        APResetStateInit,
        APResetStateFlushPeers,
        APResetStateStoppingAP
    } m_ResetState;

    Task  m_StopApTask;
    BYTE *m_pCommandBuffer;
};

//==============================================================================================

/*
  Handles the following job:
    WiFiJobP2PEnableHrdssDevices
*/
class CP2PEnableHrdssDevices :
    public CSimpleSetOidJob
{
public:
    CP2PEnableHrdssDevices(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CSimpleSetOidJob( ActivityId )
    {
    }

    NDIS_STATUS SetData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded
        )
    {
        NDIS_STATUS ndisStatus;

        UNREFERENCED_PARAMETER(pInformationBuffer);
        UNREFERENCED_PARAMETER(InformationBufferLength);
        UNREFERENCED_PARAMETER(pBytesRead);
        UNREFERENCED_PARAMETER(pBytesNeeded);

        ndisStatus = GetPortPropertyCache()->SetPropertyBoolean(WfcPortProperty802_11bDataRateSupport, TRUE);

        return ndisStatus;
    }
};


