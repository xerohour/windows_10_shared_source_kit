#pragma once

/*
  Handles the following jobs:
    WiFiJobP2PSetDeviceCapability
    WiFiJobP2PSetAdditionalIE
    WiFiJobP2PSetDeviceInfo
    WiFiJobP2PSetGroupOwnerCapability
    WiFiJobP2PSetSecondaryDeviceTypeList
    WiFiJobP2PSetAdvertisementList
*/
class CSetWFDAdvertisementSettings : public CSimpleSetDeviceCommandOidJob
{
public:
    CSetWFDAdvertisementSettings(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CSimpleSetDeviceCommandOidJob(ActivityId)
    {

    }

    NDIS_STATUS GetDeviceCommandData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesRead,
        _In_ PUINT  pBytesNeeded,
        _In_ UINT   ReservedHeaderLength,
        _Out_ ULONG* pIoctlCode,
        _Out_ ULONG* pCommandBufferLength,
        _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer );

    NDIS_STATUS OnDeviceCommandCompletion(
        _In_ NDIS_STATUS OperationStatus,
        _In_ ULONG MessageLength,
        _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader );

protected:
    WDI_SET_ADVERTISEMENT_INFORMATION_PARAMETERS m_AdvertisementParameters;
};


/*
  Handles the following jobs:
    WiFiJobP2PSetListenState
    WiFiJobP2PSetListenStatePassiveAvailability
*/
class CSetWFDListenState : public CSimpleSetDeviceCommandOidJob
{
public:
    CSetWFDListenState(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CSimpleSetDeviceCommandOidJob(ActivityId)
    {
        RtlZeroMemory( &m_ListenStateParameters, sizeof(WDI_SET_P2P_LISTEN_STATE_PARAMETERS ));
    }

    ~CSetWFDListenState()
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
    WDI_SET_P2P_LISTEN_STATE_PARAMETERS m_ListenStateParameters;
};


/*
  Handles the following jobs:
  WiFiJobP2PSetDesiredGroupID
*/
class CSetP2PDesiredGroupIDJob :
    public CPropertyCacheSetBufferOidJob
{
public:
    CSetP2PDesiredGroupIDJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CPropertyCacheSetBufferOidJob(
            WfcPortPropertyP2PDesiredGroupID,
            ActivityId ) { }
};


/*
  Handles the following jobs:
  WiFiJobP2PDisconnectPeerRequest
 */
class CP2PDisconnectJob : public CDeviceCommandOidJobBase
{
public:
    CP2PDisconnectJob(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CDeviceCommandOidJobBase(ActivityId)
    {

    }

    ~CP2PDisconnectJob()
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

public:
    NDIS_STATUS QueryNextCommandInfo(
        _In_ ULONG ReservedHeaderLength,
        _Inout_ WFC_DEVICE_COMMAND_TOKEN* pCommandToken,
        _Out_ WFC_DEVICE_COMMAND_TYPE* pCommandType,
        _Out_ UINT32* pIoctlCode,
        _Out_ ULONG* pCommandBufferLength,
        _Outptr_result_buffer_(*pCommandBufferLength) BYTE** ppCommandBuffer );

    void OnNotifyJobCompletion(
        _In_ WFC_DEVICE_COMMAND_TOKEN LastCommandToken,
        _In_ NDIS_STATUS OperationStatus );

protected:
    WDI_TASK_DISCONNECT_PARAMETERS m_DisconnectParams;
};


//==============================================================================================

/*
  Handles the following jobs:
    WiFiJobP2PGetAdvertisementList
*/

class CGetWFDAdvertisementSettings : public CSimpleQueryOidJob
{
public:
    CGetWFDAdvertisementSettings(
        _In_ WFC_ACTIVITY_ID ActivityId )
            : CSimpleQueryOidJob(ActivityId)
    {
    }

    NDIS_STATUS QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesWritten,
        _Inout_ PUINT  pBytesNeeded
        );

private:
    WFC_PROPERTY_NAME m_WFDAdvertisementPropertyName;
};


//==============================================================================================

/*
  Handles the following jobs:
    WiFiJobP2PStartBackgroundDiscovery
*/
class CSetWFDStartBackgroundDiscovery : public CSimpleSetDeviceCommandOidJob
{
public:
    CSetWFDStartBackgroundDiscovery(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CSimpleSetDeviceCommandOidJob(ActivityId)
    {
    }

    ~CSetWFDStartBackgroundDiscovery()
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

    static NDIS_STATUS CSetWFDStartBackgroundDiscovery::BuildBackgroundDiscoveryWdiBuffer(
        _In_  UINT uDot11BufferLength,
        _In_reads_bytes_(uDot11BufferLength)  DOT11_WFD_BACKGROUND_DISCOVERY_REQUEST *pDot11WFDDiscovery,
        _Out_ WDI_SET_P2P_START_BACKGROUND_DISCOVERY_PARAMETERS *pWdiDiscoveryParameters
        );

protected:
    WDI_SET_P2P_START_BACKGROUND_DISCOVERY_PARAMETERS m_BackgroundDiscoveryParameters;
};



//==============================================================================================

/*
  Handles the following jobs:
    WiFiJobP2PStopBackgroundDiscovery
*/
class CSetWFDStopBackgroundDiscovery : public CSimpleSetDeviceCommandOidJob
{
public:
    CSetWFDStopBackgroundDiscovery(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CSimpleSetDeviceCommandOidJob(ActivityId)
    {
    }

    ~CSetWFDStopBackgroundDiscovery()
    {

    }

public:
    NDIS_STATUS InitializeFromJob (
        _In_ CAdapter* pAdapter,
        _In_ NDIS_PORT_NUMBER m_NdisPortNumber );

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
};



