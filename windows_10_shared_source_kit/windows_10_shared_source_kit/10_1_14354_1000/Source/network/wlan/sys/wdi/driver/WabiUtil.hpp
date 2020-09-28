/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    WabiUtil.hpp

Abstract:

    Header file for the helpers for Wabi layer

Environment:

    Kernel mode

Revision History:

--*/
#pragma once

class CMessageHelper
{
public:
    static NDIS_STATUS GetStatusFromTaskOutput(
        _In_ Task* pTask,
        _Out_opt_ NDIS_STATUS* pStatus);

    static NDIS_STATUS GetStatusFromCommandResult(
        _In_ DeviceCommand* pCommand,
        _Out_opt_ NDIS_STATUS* pStatus);

    static VOID CMessageHelper::LogTLV(
        _In_ ULONG PeerVersion,
        _In_ ULONG DataSize,
        _In_reads_bytes_(DataSize)BYTE*   pDataBuffer
        );
        
    static VOID LogMessage(
        _In_ UCHAR MessageIndex,
        _In_ ULONG MessageId,
        _In_ ULONG PeerVersion,
        _In_ ULONG MessageLength,
        _In_reads_bytes_opt_(MessageLength) PWDI_MESSAGE_HEADER pMessageHeader // TLV follows header
        );

    static NDIS_STATUS FitMessageToBufferSize(
        _In_reads_bytes_(InputMessageSize) PWFC_MESSAGE_METADATA pMessageMetadata,
        _In_ const ULONG InputMessageSize,
        _In_ const ULONG BufferLimit,
        _In_ const UINT16 TlvIdToPrune,
        _Deref_out_range_(0, InputMessageSize) ULONG* pOutputMessageSize
        );
    
};
class CDot11ToWabiConverter
{
public:
    static WDI_SCAN_TYPE MapScanType(
        DOT11_SCAN_TYPE Dot11ScanType
        );

    static WDI_AUTH_ALGORITHM MapAuthAlgorithm(
        DOT11_AUTH_ALGORITHM AuthAlgo
        );

    static WDI_CIPHER_ALGORITHM MapCipherAlgorithm(
        DOT11_CIPHER_ALGORITHM CipherAlgo
        );

    static WDI_EXEMPTION_ACTION_TYPE MapExemptionActionType(
        USHORT ExemptionActionType
        );
        
    static WDI_EXEMPTION_PACKET_TYPE MapExemptionPacketType(
        USHORT ExemptionPacketType
        );

    static WDI_CIPHER_KEY_DIRECTION MapCipherKeyDirection(
        DOT11_DIRECTION Direction
        );        

    static WDI_P2P_DISCOVER_TYPE MapP2PDiscoverType(
        DOT11_WFD_DISCOVER_TYPE P2PDiscoverType
        );

    static WDI_P2P_SCAN_TYPE MapP2PScanType(
        DOT11_WFD_SCAN_TYPE P2PScanType
        );

    static UINT32 MapPacketFilterType(
        ULONG filterType
        );

    static WDI_OPERATION_MODE CDot11ToWabiConverter::MapOperationMode(
        PDOT11_CURRENT_OPERATION_MODE pDot11CurrentOperationMode
        );

    static NDIS_STATUS CDot11ToWabiConverter::MapP2PChannel(
        _In_  PDOT11_WFD_CHANNEL    pDot11WfdChannel,
        _Out_ PWDI_P2P_CHANNEL     pWiFiP2PChannel
        );

    static WDI_P2P_DISCOVER_TYPE CDot11ToWabiConverter::MapDiscoverType(
        DOT11_WFD_DISCOVER_TYPE DiscoverType
        );

    static WDI_P2P_SERVICE_DISCOVERY_TYPE CDot11ToWabiConverter::MapServiceDiscoveryType(
        DOT11_WFD_SERVICE_DISCOVERY_TYPE SvcDiscoveryType
        );

    static NDIS_STATUS CDot11ToWabiConverter::MapWFDChannelInfo(
        _In_ PBYTE pbHeader,
        _In_ UINT MaxBufferLength,
        _Inout_ DWORD *pdwOffset,
        _In_ PDOT11_WFD_REMOTE_CHANNEL_AVAILABILITY_INFO  pDot11ChannelInfo,
        _Out_ PWDI_P2P_DISCOVERY_CHANNEL_SETTINGS_CONTAINER pWiFiChannelSettings
        );

    static UINT8 CDot11ToWabiConverter::MapWFDGroupCapabilities(
        _In_    BOOLEAN bGroupOwner,
        _In_    BOOLEAN bPersistentGroupEnabled,
        _In_    BOOLEAN bGroupLimitReached,
        _In_    BOOLEAN bIntraBSSDistributionSupported,
        _In_    BOOLEAN bCrossConnectionSupported,
        _In_    BOOLEAN bPersistentReconnectSupported,
        _In_    BOOLEAN bGroupFormationEnabled,
        _In_    BOOLEAN bEapolKeyIpAddressAllocationSupported
        );

    static UINT8 CDot11ToWabiConverter::MapWFDDeviceCapabilities(
        _In_    BOOLEAN bServiceDiscoveryEnabled,
        _In_    BOOLEAN bClientDiscoverabilityEnabled,
        _In_    BOOLEAN bConcurrentOperationSupported,
        _In_    BOOLEAN bInfrastructureManagementEnabled,
        _In_    BOOLEAN bDeviceLimitReached,
        _In_    BOOLEAN bInvitationProcedureEnabled
        );

    static NDIS_STATUS SplitToArrayOfByteArrays(
        _In_bytecount_(InputBufferLength) UINT8* pInputBuffer,
        _In_ UINT InputBufferLength,
        _Out_ ArrayOfElements<WDI_BYTE_BLOB> *pArrayOfByteArrays
        );
        
};

class CWabiToDot11Converter
{
public:
    static DOT11_ASSOC_STATUS MapAssocStatus(
        WDI_ASSOC_STATUS WabiStatus,
        UINT16           ReasonCode,
        bool             RestrictToNwifiStatus = true
        );

    static VOID MapDataRateEntry(
        PWDI_DATA_RATE_ENTRY   pWabiEntry,
        PDOT11_DATA_RATE_MAPPING_ENTRY pDot11Entry,
        UINT8             LastUsedNonStandardRateIndex
        );

    static DOT11_DS_INFO MapDsInfo(
        WDI_DS_INFO      DsInfo
        );
  
    static DOT11_AUTH_ALGORITHM MapAuthAlgorithm(
        WDI_AUTH_ALGORITHM AuthAlgo
        );

    static DOT11_CIPHER_ALGORITHM MapCipherAlgorithm(
        WDI_CIPHER_ALGORITHM CipherAlgo
        );

    static DOT11_ANQP_QUERY_RESULT MapANQPStatusCode(
        WDI_ANQP_QUERY_STATUS WiFiStatusCode
        );

    static NDIS_STATUS MapTcpOffloadCapabilitiesToNdisOffload(
        _In_ WDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER TcpOffloadCapabilities,
        _Out_ PNDIS_OFFLOAD offloadAttributes
        );

    static NDIS_STATUS CombineArrayOfByteArrays(
        _In_ ArrayOfElements<WDI_BYTE_BLOB> *pArrayOfByteArrays,
        _Outptr_result_bytebuffer_(*pOutputBufferLength) UINT8** ppOutputBuffer,
        _Out_ PUINT pOutputBufferLength,
        _Out_ bool* pbNeedsFree
        );

    static DOT11_P2P_CHANNEL_INDICATE_REASON MapP2PChannelIndicateReason(
        _In_ WDI_P2P_CHANNEL_INDICATE_REASON WdiReason
        );
};

