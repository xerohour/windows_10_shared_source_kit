/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    WabiUtil.cpp

Abstract:

    Implementation for Wabi helpers

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "WabiUtil.tmh"

// Function returns failure both if the TLV is not present
// or if the NDIS_STATUS in the TLV is a failure. If the 
// TLV is not present, the NDIS_STATUS would be success
NDIS_STATUS CMessageHelper::GetStatusFromTaskOutput(
    _In_ Task* pTask,
    _Out_opt_ NDIS_STATUS* pStatus)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG  bufferLength;
    BYTE   *pBuffer = NULL;

    ndisStatus = pTask->get_OutputBuffer(&bufferLength, 
        &pBuffer);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to obtain output buffer from task (0x%x)", ndisStatus);
        goto Exit;
    }

    if (bufferLength < sizeof(WFC_MESSAGE_METADATA))
    {
        WFCError("Not enough data in output buffer from task");
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        goto Exit;
    }

    ndisStatus = ((WFC_MESSAGE_METADATA*)pBuffer)->Message.Status;
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Command failed by device. Status 0x%08x", ndisStatus);
    }
    
Exit:
    if (pStatus)
    {
        *pStatus = ndisStatus;
    }
    return ndisStatus;
}

// Same rules as GetStatusFromTaskOutput
NDIS_STATUS CMessageHelper::GetStatusFromCommandResult(
    _In_ DeviceCommand* pCommand,
    _Out_opt_ NDIS_STATUS* pStatus)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG  bufferLength;
    BYTE   *pBuffer = NULL;

    ndisStatus = pCommand->get_CommandResult(&bufferLength, 
        &pBuffer);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to obtain command result from command (0x%x)", ndisStatus);
        goto Exit;
    }

    if (bufferLength < sizeof(WFC_MESSAGE_METADATA))
    {
        WFCError("Not enough data in command result from command");
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        goto Exit;
    }

    ndisStatus = ((WFC_MESSAGE_METADATA*)pBuffer)->Response()->OidCompletionStatus;
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("OID failed by device. Status 0x%08x", ndisStatus);
    }

    ndisStatus = ((WFC_MESSAGE_METADATA*)pBuffer)->Message.Status;
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("OID failed by device. Status 0x%08x", ndisStatus);
    }
    
Exit:
    if (pStatus)
    {
        *pStatus = ndisStatus;
    }
    return ndisStatus;
}

VOID CMessageHelper::LogTLV(
    _In_ ULONG PeerVersion,
    _In_ ULONG DataSize,
    _In_reads_bytes_(DataSize)BYTE*   pDataBuffer
    )
{
    TraceUnknownTlvByteStream( PeerVersion, DataSize, pDataBuffer );
}

VOID CMessageHelper::LogMessage(
    _In_ UCHAR MessageIndex,
    _In_ ULONG MessageId,
    _In_ ULONG PeerVersion,
    _In_ ULONG MessageLength,
    _In_reads_bytes_opt_(MessageLength) PWDI_MESSAGE_HEADER pMessageHeader
    )
{
    BYTE * pCurrent;
    ULONG remainingLength;

    if (!WPP_LEVEL_ENABLED(TRACE_LEVEL_VERBOSE)) 
    {
        // WIFICLASS_TRACE level is not enabled, dont do all this work
        return;
    }

    if ((MessageLength < sizeof( WDI_MESSAGE_HEADER )) || (NULL == pMessageHeader))
    {
        return;
    }

    switch (MessageIndex)
    {
        case 1:
            {
                WFCTrace("[MSG 1] Command %!MESSAGE_ID! (%d)", MessageId, MessageId);
                WFCTrace("[MSG]   Length          = %d", MessageLength);
                WFCTrace("[MSG]   Transaction ID  = %d", pMessageHeader->TransactionId);
                WFCTrace("[MSG]   Port ID         = %d", pMessageHeader->PortId);
            }
            break;

        case 3:
            {
                WFCTrace("[MSG 3] Response %!MESSAGE_ID! (%d)", MessageId, MessageId);
                WFCTrace("[MSG]   Length          = %d", MessageLength);
                WFCTrace("[MSG]   Transaction ID  = %d", pMessageHeader->TransactionId);
                WFCTrace("[MSG]   Port ID         = %d", pMessageHeader->PortId);
                WFCTrace("[MSG]   Status          = 0x%x", pMessageHeader->Status);
            }
            break;

        case 4:
            {
                if (pMessageHeader->TransactionId != 0)
                {
                    // M4
                    WFCTrace("[MSG 4] Indication %!MESSAGE_ID! (%d)", MessageId, MessageId);
                }
                else
                {
                    // Unsolicited indication
                    WFCTrace("[MSG 0] Indication %!MESSAGE_ID! (%d)", MessageId, MessageId);
                }
                WFCTrace("[MSG]   Length          = %d", MessageLength);
                WFCTrace("[MSG]   Transaction ID  = %d", pMessageHeader->TransactionId);
                WFCTrace("[MSG]   Port ID         = %d", pMessageHeader->PortId);                
                WFCTrace("[MSG]   Status          = 0x%x", pMessageHeader->Status);
            }
            break;

        default:
            break;
    }
    remainingLength = MessageLength - sizeof(WDI_MESSAGE_HEADER);
    pCurrent = (BYTE*)pMessageHeader + sizeof(WDI_MESSAGE_HEADER);

    // Skip known non-TLV messages
    
    // Skip internal message
    if (MessageId > WDI_MESSAGE_ID_OS_BASE)
        return;

    // Here we assume that everything below is a TLV. That is not a guarantee
    TraceMessageTlvByteStream( MessageId, MessageIndex == 1, PeerVersion, remainingLength, pCurrent );
}

NDIS_STATUS CMessageHelper::FitMessageToBufferSize(
    _In_reads_bytes_(InputMessageSize) PWFC_MESSAGE_METADATA pMessageMetadata,
    _In_ const ULONG InputMessageSize,
    _In_ const ULONG BufferLimit,
    _In_ const UINT16 TlvIdToPrune,
    _Deref_out_range_(0, InputMessageSize) ULONG* pOutputMessageSize
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BYTE* pTlvData;
    USHORT tlvType;
    ULONG fullTlvLength;
    ULONG tlvBlobLength;
    ULONG truncatableSize = 0, nonTruncatableSize = 0;
    ULONG allowedTruncatableSize = 0;
    TraceEntry();

    if ((BufferLimit < sizeof(WDI_MESSAGE_HEADER)) ||
        (InputMessageSize < sizeof(WFC_MESSAGE_METADATA)))
    {
        WFCError("[MsgLimit] Invalid Buffer Limit %d, Input Message Size %d specified", BufferLimit, InputMessageSize);
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }
    
    // Skip the internal part of the message metadata. It doesnt count against the limit
    if ((InputMessageSize - FIELD_OFFSET(WFC_MESSAGE_METADATA, Message))  <= BufferLimit)
    {
        // No changes needed
        *pOutputMessageSize = InputMessageSize;
        ndisStatus = NDIS_STATUS_SUCCESS;
        goto exit;
    }

    // We will have to truncate
    WFCInfo("[MsgLimit] Need to fit message TLV of %d bytes into %d bytes", InputMessageSize, BufferLimit);

    // Skip past the WDI_MESSAGE_HEADER for all the TLV processing    
    pTlvData = ((PUINT8)&(pMessageMetadata->Message)) + sizeof(WDI_MESSAGE_HEADER);
    tlvBlobLength = InputMessageSize - FIELD_OFFSET(WFC_MESSAGE_METADATA, Message) - sizeof(WDI_MESSAGE_HEADER);

    // This length is expected
    nonTruncatableSize = sizeof(WDI_MESSAGE_HEADER);

    // We do a two pass approach - First pass find the size of the non-truncatable TLVs.
    // Second pass, truncate the ones we are OK

    while (tlvBlobLength >= WDI_TLV_HEADER_SIZE)
    {
        tlvType = *((USHORT*)pTlvData);
        fullTlvLength = *((USHORT*)(pTlvData + sizeof( USHORT ))) + WDI_TLV_HEADER_SIZE;

        if ((fullTlvLength > tlvBlobLength))
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            WFCError("[MsgLimit] First Pass - Invalid TLV/Data Length");
            goto exit;
        }

        if (tlvType == TlvIdToPrune)
        {
            // This is a TLV we can prune
            truncatableSize += fullTlvLength;
        }
        else
        {
            // This is a TLV we cannot prune
            nonTruncatableSize += fullTlvLength;

            if (truncatableSize != 0)
            {
                WFCError("[MsgLimit] Non-Prunable TLV %d found after prunable TLVs", tlvType);
                NT_ASSERTMSG("Non-Prunable TLV found after prunable TLVs", FALSE);
                ndisStatus = NDIS_STATUS_INVALID_DATA;
                goto exit;
            }
        }

        if (!NT_SUCCESS(
            RtlULongSub(
            tlvBlobLength,
            fullTlvLength,
            &tlvBlobLength ) ))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            WFCError("[MsgLimit] First Pass - Airthmethic underflow computing TLV/Data Length");
            goto exit;
        }

        pTlvData += fullTlvLength;
    }

    if (tlvBlobLength > 0)
    {
        WFCError("[MsgLimit] Extra data of %d bytes at the end of TLV blob", tlvBlobLength);
        NT_ASSERTMSG("Extra data at end of the TLV data blob", FALSE);
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    WFCTrace("[MsgLimit] Prunable Length = %d Non Prunable Length = %d Total = %d", truncatableSize, nonTruncatableSize, InputMessageSize);

    if (nonTruncatableSize > BufferLimit)
    {
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        WFCError("[MsgLimit] More non Prunable TLV %d than allowed by Buffer Limit %d", nonTruncatableSize, BufferLimit);
        goto exit;
    }

    // This is how much truncatable data we can save
    allowedTruncatableSize = BufferLimit - nonTruncatableSize;
    WFCTrace("[MsgLimit] Will shrink prunable TLVs to %d bytes", allowedTruncatableSize);

    // Do a second loop of this buffer and if its a prunable TLV and we are over limit, dont save it
    
    pTlvData = ((PUINT8)&(pMessageMetadata->Message)) + sizeof(WDI_MESSAGE_HEADER);
    pTlvData = ((PUINT8)&(pMessageMetadata->Message)) + sizeof(WDI_MESSAGE_HEADER);
    tlvBlobLength = InputMessageSize - FIELD_OFFSET(WFC_MESSAGE_METADATA, Message) - sizeof(WDI_MESSAGE_HEADER);

    truncatableSize = 0;
    //outWdiMessageLength = FIELD_OFFSET(WFC_MESSAGE_METADATA, Message) + sizeof(WDI_MESSAGE_HEADER);

    while (tlvBlobLength >= WDI_TLV_HEADER_SIZE)
    {
        tlvType = *((USHORT*)pTlvData);
        fullTlvLength = *((USHORT*)(pTlvData + sizeof(USHORT))) + WDI_TLV_HEADER_SIZE;

        if ((fullTlvLength > tlvBlobLength))
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            WFCError("[MsgLimit] Second Pass - Invalid TLV/Data Length");
            goto exit;
        }

        if (tlvType == TlvIdToPrune)
        {
            // This is a TLV we can prune
            if ((truncatableSize + fullTlvLength) > allowedTruncatableSize)
            {
                // Can no longer add any truncatable TLVs
                WFCTrace("[MsgLimit] Pruning TLV of length %d at offset %d", 
                    fullTlvLength - WDI_TLV_HEADER_SIZE, 
                    InputMessageSize - tlvBlobLength
                    );
                break;
            }
            else
            {
                truncatableSize += fullTlvLength;            
            }
        }

        if (!NT_SUCCESS(
            RtlULongSub(
            tlvBlobLength,
            fullTlvLength,
            &tlvBlobLength ) ))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            WFCError("[MsgLimit] Airthmethic underflow computing TLV/Data Length");
            goto exit;
        }

        pTlvData += fullTlvLength;
    }    

    // Save the computed final length
    *pOutputMessageSize = FIELD_OFFSET(WFC_MESSAGE_METADATA, Message) + truncatableSize + nonTruncatableSize;
    WFCInfo("[MsgLimit] Total length of fitted message is %d bytes", *pOutputMessageSize);

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

WDI_SCAN_TYPE CDot11ToWabiConverter::MapScanType(
    DOT11_SCAN_TYPE Dot11ScanType
    )
{
    // Auto has Active & Passive bits set, so check Auto first
    if ((Dot11ScanType & dot11_scan_type_auto) == dot11_scan_type_auto)
        return WDI_SCAN_TYPE_AUTO;
    if (Dot11ScanType & dot11_scan_type_active)
        return WDI_SCAN_TYPE_ACTIVE_ONLY;
    if (Dot11ScanType & dot11_scan_type_passive)
        return WDI_SCAN_TYPE_PASSIVE_ONLY;

    NT_ASSERT(FALSE);

    return WDI_SCAN_TYPE_MAX_VALUE;
}

WDI_AUTH_ALGORITHM CDot11ToWabiConverter::MapAuthAlgorithm(
    DOT11_AUTH_ALGORITHM AuthAlgo
    )
{
    switch (AuthAlgo)
    {
        case DOT11_AUTH_ALGO_80211_OPEN:
            return WDI_AUTH_ALGO_80211_OPEN;
        case DOT11_AUTH_ALGO_80211_SHARED_KEY:
            return WDI_AUTH_ALGO_80211_SHARED_KEY;
        case DOT11_AUTH_ALGO_WPA:
            return WDI_AUTH_ALGO_WPA;
        case DOT11_AUTH_ALGO_WPA_PSK:
            return WDI_AUTH_ALGO_WPA_PSK;
        case DOT11_AUTH_ALGO_WPA_NONE:
            return WDI_AUTH_ALGO_WPA_NONE;
        case DOT11_AUTH_ALGO_RSNA:
            return WDI_AUTH_ALGO_RSNA;
        case DOT11_AUTH_ALGO_RSNA_PSK:
            return WDI_AUTH_ALGO_RSNA_PSK;
        default:
            return (WDI_AUTH_ALGORITHM)AuthAlgo;
        
    }
}


WDI_CIPHER_ALGORITHM CDot11ToWabiConverter::MapCipherAlgorithm(
    DOT11_CIPHER_ALGORITHM CipherAlgo
    )
{
    switch (CipherAlgo)
    {
        case DOT11_CIPHER_ALGO_NONE:
            return WDI_CIPHER_ALGO_NONE;
        case DOT11_CIPHER_ALGO_WEP40:
            return WDI_CIPHER_ALGO_WEP40;
        case DOT11_CIPHER_ALGO_TKIP:
            return WDI_CIPHER_ALGO_TKIP;
        case DOT11_CIPHER_ALGO_CCMP:
            return WDI_CIPHER_ALGO_CCMP;
        case DOT11_CIPHER_ALGO_GCMP:
            return WDI_CIPHER_ALGO_GCMP;
        case DOT11_CIPHER_ALGO_WEP104:
            return WDI_CIPHER_ALGO_WEP104;
        case DOT11_CIPHER_ALGO_BIP:
            return WDI_CIPHER_ALGO_BIP;
        case DOT11_CIPHER_ALGO_WPA_USE_GROUP:
            return WDI_CIPHER_ALGO_WPA_USE_GROUP;
        case DOT11_CIPHER_ALGO_WEP:
            return WDI_CIPHER_ALGO_WEP;
        default:
            return (WDI_CIPHER_ALGORITHM)CipherAlgo;
        
    }
}



WDI_EXEMPTION_ACTION_TYPE CDot11ToWabiConverter::MapExemptionActionType(
    USHORT ExemptionActionType
    )
{
    //As long as these are the same simply cast
    C_ASSERT(DOT11_EXEMPT_NO_EXEMPTION == WDI_EXEMPT_NO_EXEMPTION);
    C_ASSERT(DOT11_EXEMPT_ALWAYS == WDI_EXEMPT_ALWAYS);
    C_ASSERT(DOT11_EXEMPT_ON_KEY_MAPPING_KEY_UNAVAILABLE == WDI_EXEMPT_ON_KEY_MAPPING_KEY_UNAVAILABLE);

    return (WDI_EXEMPTION_ACTION_TYPE)ExemptionActionType;
}

WDI_EXEMPTION_PACKET_TYPE CDot11ToWabiConverter::MapExemptionPacketType(
    USHORT ExemptionPacketType
    )
{
    switch (ExemptionPacketType)
    {
        case DOT11_EXEMPT_UNICAST:
            return WDI_EXEMPT_PACKET_TYPE_UNICAST;
        case DOT11_EXEMPT_MULTICAST:
            return WDI_EXEMPT_PACKET_TYPE_MULTICAST;
        case DOT11_EXEMPT_BOTH:
            return WDI_EXEMPT_PACKET_TYPE_BOTH;
    }

    NT_ASSERT(FALSE);
    return WDI_EXEMPT_PACKET_TYPE_BOTH;        
}

WDI_CIPHER_KEY_DIRECTION CDot11ToWabiConverter::MapCipherKeyDirection(
    DOT11_DIRECTION Direction
    )
{
    switch (Direction)
    {
        case DOT11_DIR_INBOUND:
            return WDI_CIPHER_KEY_DIRECTION_INBOUND;
        case DOT11_DIR_OUTBOUND:
            return WDI_CIPHER_KEY_DIRECTION_OUTBOUND;
        case DOT11_DIR_BOTH:
            return WDI_CIPHER_KEY_DIRECTION_BOTH;
    }
    NT_ASSERT(FALSE);
    return WDI_CIPHER_KEY_DIRECTION_BOTH;

}

WDI_P2P_DISCOVER_TYPE CDot11ToWabiConverter::MapP2PDiscoverType(
    DOT11_WFD_DISCOVER_TYPE P2PDiscoverType
    )
{
    switch (P2PDiscoverType)
    {
        case dot11_wfd_discover_type_scan_only:
            return WDI_P2P_DISCOVER_TYPE_SCAN_ONLY;
        case dot11_wfd_discover_type_find_only:
            return WDI_P2P_DISCOVER_TYPE_FIND_ONLY;
        case dot11_wfd_discover_type_auto:
            return WDI_P2P_DISCOVER_TYPE_AUTO;
        case dot11_wfd_discover_type_scan_social_channels:
            return WDI_P2P_DISCOVER_TYPE_SCAN_SOCIAL_CHANNELS;
    }

    NT_ASSERT(FALSE);
    return WDI_P2P_DISCOVER_TYPE_SCAN_ONLY;
}

WDI_P2P_SCAN_TYPE CDot11ToWabiConverter::MapP2PScanType(
    DOT11_WFD_SCAN_TYPE P2PScanType
    )
{
    switch (P2PScanType)
    {
        case dot11_wfd_scan_type_active:
            return WDI_P2P_SCAN_TYPE_ACTIVE;
        case dot11_wfd_scan_type_passive:
            return WDI_P2P_SCAN_TYPE_PASSIVE;
        case dot11_wfd_scan_type_auto:
            return WDI_P2P_SCAN_TYPE_AUTO;
    }

    NT_ASSERT(FALSE);
    return WDI_P2P_SCAN_TYPE_AUTO;
}


UINT32 CDot11ToWabiConverter::MapPacketFilterType(
    ULONG filterType)
{
    UINT32 mappedPacketFilterType = 0;

    if(filterType & NDIS_PACKET_TYPE_DIRECTED)
        mappedPacketFilterType |= WDI_PACKET_FILTER_DIRECTED;
    if(filterType & NDIS_PACKET_TYPE_MULTICAST)
        mappedPacketFilterType |= WDI_PACKET_FILTER_MULTICAST;
    if(filterType & NDIS_PACKET_TYPE_ALL_MULTICAST)
        mappedPacketFilterType |= WDI_PACKET_FILTER_ALL_MULTICAST;
    if(filterType & NDIS_PACKET_TYPE_BROADCAST)
        mappedPacketFilterType |= WDI_PACKET_FILTER_BROADCAST;
    if(filterType & NDIS_PACKET_TYPE_PROMISCUOUS)
        mappedPacketFilterType |= WDI_PACKET_FILTER_PROMISCUOUS;
    if(filterType & NDIS_PACKET_TYPE_802_11_RAW_DATA)
        mappedPacketFilterType |= WDI_PACKET_FILTER_802_11_RAW_DATA;
    if(filterType & NDIS_PACKET_TYPE_802_11_DIRECTED_MGMT)
        mappedPacketFilterType |= WDI_PACKET_FILTER_802_11_DIRECTED_MGMT;
    if(filterType & NDIS_PACKET_TYPE_802_11_BROADCAST_MGMT)
        mappedPacketFilterType |= WDI_PACKET_FILTER_802_11_BROADCAST_MGMT;
    if(filterType & NDIS_PACKET_TYPE_802_11_MULTICAST_MGMT)
        mappedPacketFilterType |= WDI_PACKET_FILTER_802_11_MULTICAST_MGMT;
    if(filterType & NDIS_PACKET_TYPE_802_11_ALL_MULTICAST_MGMT)
        mappedPacketFilterType |= WDI_PACKET_FILTER_802_11_ALL_MULTICAST_MGMT;
    if(filterType & NDIS_PACKET_TYPE_802_11_PROMISCUOUS_MGMT)
        mappedPacketFilterType |= WDI_PACKET_FILTER_802_11_PROMISCUOUS_MGMT;
    if(filterType & NDIS_PACKET_TYPE_802_11_RAW_MGMT)
        mappedPacketFilterType |= WDI_PACKET_FILTER_802_11_RAW_MGMT;
    if (filterType & NDIS_PACKET_TYPE_802_11_DIRECTED_CTRL)
        mappedPacketFilterType |= WDI_PACKET_FILTER_802_11_DIRECTED_CTRL;
    if(filterType & NDIS_PACKET_TYPE_802_11_BROADCAST_CTRL)
        mappedPacketFilterType |= WDI_PACKET_FILTER_802_11_BROADCAST_CTRL;
    if(filterType & NDIS_PACKET_TYPE_802_11_PROMISCUOUS_CTRL)
        mappedPacketFilterType |= WDI_PACKET_FILTER_802_11_PROMISCUOUS_CTRL;
    if(filterType == NDIS_PACKET_TYPE_ALL_802_11_FILTERS)
        mappedPacketFilterType |= WDI_PACKET_FILTER_ALL;
    
    return mappedPacketFilterType;
}

WDI_OPERATION_MODE CDot11ToWabiConverter::MapOperationMode(
    PDOT11_CURRENT_OPERATION_MODE pDot11CurrentOperationMode)
{
    ULONG   uOpmodeMask = 0;

    if ((pDot11CurrentOperationMode->uCurrentOpMode & DOT11_OPERATION_MODE_EXTENSIBLE_STATION) != 0)
    {
        uOpmodeMask |= WDI_OPERATION_MODE_STA;
    }

    if ((pDot11CurrentOperationMode->uCurrentOpMode & DOT11_OPERATION_MODE_WFD_DEVICE) != 0)
    {
        uOpmodeMask |= WDI_OPERATION_MODE_P2P_DEVICE;
    }

    if ((pDot11CurrentOperationMode->uCurrentOpMode & DOT11_OPERATION_MODE_WFD_GROUP_OWNER) != 0)
    {
        uOpmodeMask |= WDI_OPERATION_MODE_P2P_GO;
    }

    if ((pDot11CurrentOperationMode->uCurrentOpMode & DOT11_OPERATION_MODE_WFD_CLIENT) != 0)
    {
        uOpmodeMask |= WDI_OPERATION_MODE_P2P_CLIENT;
    }

    return ((WDI_OPERATION_MODE) uOpmodeMask);
}


NDIS_STATUS CDot11ToWabiConverter::MapP2PChannel(
    _In_  PDOT11_WFD_CHANNEL    pDot11WfdChannel,
    _Out_ PWDI_P2P_CHANNEL     pWdiP2PChannel
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    RtlCopyMemory( pWdiP2PChannel->CountryOrRegionString,
        pDot11WfdChannel->CountryRegionString,
        sizeof(DOT11_COUNTRY_OR_REGION_STRING));

    pWdiP2PChannel->OperatingClass = pDot11WfdChannel->OperatingClass;
    pWdiP2PChannel->ChannelNumber = pDot11WfdChannel->ChannelNumber;

    return ndisStatus;
}


WDI_P2P_DISCOVER_TYPE CDot11ToWabiConverter::MapDiscoverType(
    DOT11_WFD_DISCOVER_TYPE DiscoverType
    )
{
    switch (DiscoverType)
    {
        case dot11_wfd_discover_type_scan_only:
            return WDI_P2P_DISCOVER_TYPE_SCAN_ONLY;
        case dot11_wfd_discover_type_find_only:
            return WDI_P2P_DISCOVER_TYPE_FIND_ONLY;
        case dot11_wfd_discover_type_auto:
            return WDI_P2P_DISCOVER_TYPE_AUTO;
        case dot11_wfd_discover_type_scan_social_channels:
            return WDI_P2P_DISCOVER_TYPE_SCAN_SOCIAL_CHANNELS;
        default:
            return (WDI_P2P_DISCOVER_TYPE) DiscoverType;
    }
}


WDI_P2P_SERVICE_DISCOVERY_TYPE CDot11ToWabiConverter::MapServiceDiscoveryType(
    DOT11_WFD_SERVICE_DISCOVERY_TYPE SvcDiscoveryType
    )
{
    switch (SvcDiscoveryType)
    {
        case dot11_wfd_service_discovery_type_no_service_discovery:
            return WDI_P2P_SERVICE_DISCOVERY_TYPE_NO_SERVICE_DISCOVERY;
        case dot11_wfd_service_discovery_type_service_name_only:
            return WDI_P2P_SERVICE_DISCOVERY_TYPE_SERVICE_NAME_ONLY;
        case dot11_wfd_service_discovery_type_service_information:
            return WDI_P2P_SERVICE_DISCOVERY_TYPE_SERVICE_INFORMATION;
        default:
            return (WDI_P2P_SERVICE_DISCOVERY_TYPE) SvcDiscoveryType;
    }
}


NDIS_STATUS CDot11ToWabiConverter::MapWFDChannelInfo(
    _In_ PBYTE pbHeader,
    _In_ UINT MaxBufferLength,
    _Inout_ DWORD *pdwOffset,
    _In_ PDOT11_WFD_REMOTE_CHANNEL_AVAILABILITY_INFO  pDot11ChannelInfo,
    _Out_ PWDI_P2P_DISCOVERY_CHANNEL_SETTINGS_CONTAINER pWdiChannelSettings
    )
{
    DWORD   i, dwOffset = *pdwOffset;
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    pWdiChannelSettings->ListenDuration.CycleDuration = pDot11ChannelInfo->ListenDuration.CycleDuration;
    pWdiChannelSettings->ListenDuration.ListenActiveDuration = pDot11ChannelInfo->ListenDuration.ListenActiveDuration;

    if (MaxBufferLength < dwOffset +
                          FIELD_OFFSET(DOT11_WFD_REMOTE_CHANNEL_AVAILABILITY_INFO,ucBuffer) +
                          pDot11ChannelInfo->uNumBandChannels * sizeof(DOT11_WFD_BAND_CHANNELS))
    {
        WFCError("Invalid NumChannels = %d in pDot11ChannelInfo %d < <%d+%d>",
            pDot11ChannelInfo->uNumBandChannels, MaxBufferLength, dwOffset,
            pDot11ChannelInfo->uNumBandChannels * sizeof(DOT11_WFD_BAND_CHANNELS));
        return (NDIS_STATUS_INVALID_DATA);
    }

    status = pWdiChannelSettings->BandChannelList.AllocateElements(pDot11ChannelInfo->uNumBandChannels, 0);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to allocate memory for %d BandChannels, Error=0x%08x",
            pDot11ChannelInfo->uNumBandChannels, status);
        return (status);
    }

    dwOffset += FIELD_OFFSET(DOT11_WFD_REMOTE_CHANNEL_AVAILABILITY_INFO,ucBuffer);
    for (i=0; i<pDot11ChannelInfo->uNumBandChannels; i++)
    {
        PDOT11_WFD_BAND_CHANNELS pDot11BandChannels = (DOT11_WFD_BAND_CHANNELS *) (pbHeader + dwOffset);
        PWDI_BAND_CHANNEL_LIST_CONTAINER pWdiBandChannels = &pWdiChannelSettings->BandChannelList.pElements[i];

        dwOffset += FIELD_OFFSET(DOT11_WFD_BAND_CHANNELS,Channels) + (pDot11BandChannels->uNumChannels * sizeof(ULONG));

        WFCInfo ("\t\t[%d] Band=<%d>, NumChannels=<%d>, Channels=<%d ...>\n",
            i, pDot11BandChannels->uBandID, pDot11BandChannels->uNumChannels,
            (pDot11BandChannels->uNumChannels > 0 ? pDot11BandChannels->Channels[0] : 0));

        pWdiBandChannels->BandID = pDot11BandChannels->uBandID;

        pWdiBandChannels->ChannelList.SimpleAssign(
            (UINT32 *) pDot11BandChannels->Channels,
            pDot11BandChannels->uNumChannels);
    }

    *pdwOffset = dwOffset;
    return (status);
}


UINT8 CDot11ToWabiConverter::MapWFDGroupCapabilities(
    _In_    BOOLEAN bGroupOwner,
    _In_    BOOLEAN bPersistentGroupEnabled,
    _In_    BOOLEAN bGroupLimitReached,
    _In_    BOOLEAN bIntraBSSDistributionSupported,
    _In_    BOOLEAN bCrossConnectionSupported,
    _In_    BOOLEAN bPersistentReconnectSupported,
    _In_    BOOLEAN bGroupFormationEnabled,
    _In_    BOOLEAN bEapolKeyIpAddressAllocationSupported
    )
{
    UINT8 GroupCapabilityBitmap = 0;

    if (bGroupOwner)
    {
        GroupCapabilityBitmap |= DOT11_WFD_GROUP_CAPABILITY_GROUP_OWNER;
    }
    if (bPersistentGroupEnabled)
    {
        GroupCapabilityBitmap |= DOT11_WFD_GROUP_CAPABILITY_PERSISTENT_GROUP;
    }
    if (bGroupLimitReached)
    {
        GroupCapabilityBitmap |= DOT11_WFD_GROUP_CAPABILITY_GROUP_LIMIT_REACHED;
    }
    if (bIntraBSSDistributionSupported)
    {
        GroupCapabilityBitmap |= DOT11_WFD_GROUP_CAPABILITY_INTRABSS_DISTRIBUTION_SUPPORTED;
    }
    if (bCrossConnectionSupported)
    {
        GroupCapabilityBitmap |= DOT11_WFD_GROUP_CAPABILITY_CROSS_CONNECTION_SUPPORTED;
    }
    if (bPersistentReconnectSupported)
    {
        GroupCapabilityBitmap |= DOT11_WFD_GROUP_CAPABILITY_PERSISTENT_RECONNECT_SUPPORTED;
    }
    if (bGroupFormationEnabled)
    {
        GroupCapabilityBitmap |= DOT11_WFD_GROUP_CAPABILITY_IN_GROUP_FORMATION;
    }
    if (bEapolKeyIpAddressAllocationSupported)
    {
        GroupCapabilityBitmap |= DOT11_WFD_GROUP_CAPABILITY_EAPOL_KEY_IP_ADDRESS_ALLOCATION_SUPPORTED;
    }

    return (GroupCapabilityBitmap);
}


UINT8 CDot11ToWabiConverter::MapWFDDeviceCapabilities(
    _In_    BOOLEAN bServiceDiscoveryEnabled,
    _In_    BOOLEAN bClientDiscoverabilityEnabled,
    _In_    BOOLEAN bConcurrentOperationSupported,
    _In_    BOOLEAN bInfrastructureManagementEnabled,
    _In_    BOOLEAN bDeviceLimitReached,
    _In_    BOOLEAN bInvitationProcedureEnabled
    )
{
    UINT8 DeviceCapabilityBitmap = 0;

    if (bServiceDiscoveryEnabled)
    {
        DeviceCapabilityBitmap |= DOT11_WFD_DEVICE_CAPABILITY_SERVICE_DISCOVERY;
    }
    if (bClientDiscoverabilityEnabled)
    {
        DeviceCapabilityBitmap |= DOT11_WFD_DEVICE_CAPABILITY_P2P_CLIENT_DISCOVERABILITY;
    }
    if (bConcurrentOperationSupported)
    {
        DeviceCapabilityBitmap |= DOT11_WFD_DEVICE_CAPABILITY_CONCURRENT_OPERATION;
    }
    if (bInfrastructureManagementEnabled)
    {
        DeviceCapabilityBitmap |= DOT11_WFD_DEVICE_CAPABILITY_P2P_INFRASTRUCTURE_MANAGED;
    }
    if (bDeviceLimitReached)
    {
        DeviceCapabilityBitmap |= DOT11_WFD_DEVICE_CAPABILITY_P2P_DEVICE_LIMIT;
    }
    if (bInvitationProcedureEnabled)
    {
        DeviceCapabilityBitmap |= DOT11_WFD_DEVICE_CAPABILITY_P2P_INVITATION_PROCEDURE;
    }

    return (DeviceCapabilityBitmap);
}

NDIS_STATUS CDot11ToWabiConverter::SplitToArrayOfByteArrays(
    _In_bytecount_(InputBufferLength) UINT8* pInputBuffer,
    _In_ UINT InputBufferLength,
    _Out_ ArrayOfElements<WDI_BYTE_BLOB> *pArrayOfByteArrays
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    UINT numberOfByteArrays = 0;
    UINT currentTlvLength = 0;

    // Caller should make sure that we have atleast some data
    if (InputBufferLength < 0)
    {        
        WFCError("[IhvExt] Function called with 0 bytes");
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    // Each TLV can hold USHORT entries. Determine how many TLVs we will have
    // 0 byte = INVALID_DATA above, 1 byte = 1 TLV, 64K = 1 TLV, 64K+1 = 2 TLVs,
    numberOfByteArrays = ((InputBufferLength - 1) / WDI_TLV_LENGTH_LIMIT) + 1;

    ndisStatus = pArrayOfByteArrays->AllocateElements(numberOfByteArrays, 0);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[IhvExt] Unable to allocate %d ByteArrays. Status = 0x%08x", numberOfByteArrays, ndisStatus);
        goto exit;
    }
    
    WFCTrace("[IhvExt] %d bytes of data split into %d TLVs", InputBufferLength, numberOfByteArrays);
    for (UINT i = 0; i < numberOfByteArrays; i++)
    {
        currentTlvLength = (InputBufferLength > WDI_TLV_LENGTH_LIMIT) ? WDI_TLV_LENGTH_LIMIT : InputBufferLength;
        
        // Point the ByteArray's data to the original data in the buffer
        pArrayOfByteArrays->pElements[i].SimpleAssign(
            pInputBuffer,
            currentTlvLength
            );

        // Move the buffers 
        pInputBuffer += currentTlvLength;
        InputBufferLength -= currentTlvLength;
    }

    // No more data should be remaining
    NT_ASSERT(InputBufferLength == 0);
    
exit:
    
    return ndisStatus;
}
DOT11_ASSOC_STATUS CWabiToDot11Converter::MapAssocStatus(
    WDI_ASSOC_STATUS WabiStatus,
    UINT16           ReasonCode,
    bool             RestrictToNwifiStatus      // Defaults to true
    )
{
    switch (WabiStatus)
    {
        case WDI_ASSOC_STATUS_SUCCESS:
            return DOT11_ASSOC_STATUS_SUCCESS;  // Existing
        case WDI_ASSOC_STATUS_FAILURE:
            return DOT11_ASSOC_STATUS_FAILURE;  // Existing
        case WDI_ASSOC_STATUS_UNREACHABLE:
            return DOT11_ASSOC_STATUS_UNREACHABLE;  // Existing
        case WDI_ASSOC_STATUS_RADIO_OFF:
            return DOT11_ASSOC_STATUS_RADIO_OFF;    // Existing
        case WDI_ASSOC_STATUS_PHY_DISABLED:
            return DOT11_ASSOC_STATUS_PHY_DISABLED; // Existing
        case WDI_ASSOC_STATUS_ABORTED:
            return DOT11_ASSOC_STATUS_CANCELLED;    // Existing
        case WDI_ASSOC_STATUS_CANDIDATE_LIST_EXHAUSTED:
            return DOT11_ASSOC_STATUS_CANDIDATE_LIST_EXHAUSTED; // Existing

        /*
         * Beacon/Probe Response Receive Error
         */
        case WDI_ASSOC_STATUS_PROBE_TX_FAILURE:
            return RestrictToNwifiStatus ? 
                DOT11_ASSOC_STATUS_UNREACHABLE : 
                DOT11_ASSOC_STATUS_PROBE_TX_FAILURE ;  // Maybe Remapped
                
        case WDI_ASSOC_STATUS_NO_BEACON_PROBE_RESPONSE:
            return RestrictToNwifiStatus ? 
                DOT11_ASSOC_STATUS_UNREACHABLE :
                DOT11_ASSOC_STATUS_NO_BEACON_PROBE_RESPONSE;  // Maybe Remapped

        /*
         * 802.11 Authentication sequence errors
         */
        case WDI_ASSOC_STATUS_AUTH_REQUEST_NO_ACK:
            return RestrictToNwifiStatus ? 
                DOT11_ASSOC_STATUS_UNREACHABLE : 
                DOT11_ASSOC_STATUS_AUTH_REQUEST_NO_ACK;  // Maybe Remapped
                
        case WDI_ASSOC_STATUS_NO_AUTH_RESPONSE:
            return RestrictToNwifiStatus ? 
                DOT11_ASSOC_STATUS_UNREACHABLE :
                DOT11_ASSOC_STATUS_NO_AUTH_RESPONSE;  // Maybe Remapped
                
        case WDI_ASSOC_STATUS_AUTH_RESPONSE_CAPABILITY_MISMATCH:
            return RestrictToNwifiStatus ? 
                DOT11_ASSOC_STATUS_UNREACHABLE : 
                DOT11_ASSOC_STATUS_AUTH_RESPONSE_CAPABILITY_MISMATCH;  // Maybe Remapped
                
        case WDI_ASSOC_STATUS_BAD_AUTH_RESPONSE:
            return RestrictToNwifiStatus ? 
                DOT11_ASSOC_STATUS_UNREACHABLE : 
                DOT11_ASSOC_STATUS_BAD_AUTH_RESPONSE;  // Maybe Remapped
                
        case WDI_ASSOC_STATUS_AUTH_FAILED_BY_PEER:
            return RestrictToNwifiStatus ? 
                (DOT11_ASSOC_STATUS_ASSOCIATION_RESPONSE | ReasonCode) :
                (DOT11_ASSOC_STATUS_AUTH_FAILED_BY_PEER | ReasonCode);  // Maybe Remapped to Existing
                
        case WDI_ASSOC_STATUS_AUTH_EXCHANGE_FAILURE:
            return RestrictToNwifiStatus ? 
                DOT11_ASSOC_STATUS_UNREACHABLE : 
                DOT11_ASSOC_STATUS_AUTH_EXCHANGE_FAILURE;  // Maybe Remapped
            
        /*
         * 802.11 Association sequence errors
         */
        case WDI_ASSOC_STATUS_ASSOC_REQUEST_NO_ACK:
            return RestrictToNwifiStatus ? 
                DOT11_ASSOC_STATUS_UNREACHABLE :
                DOT11_ASSOC_STATUS_ASSOC_REQUEST_NO_ACK;  // Maybe Remapped
                
        case WDI_ASSOC_STATUS_NO_ASSOC_RESPONSE:
            return RestrictToNwifiStatus ? 
                DOT11_ASSOC_STATUS_UNREACHABLE : 
                DOT11_ASSOC_STATUS_NO_ASSOC_RESPONSE;  // Maybe Remapped
                
        case WDI_ASSOC_STATUS_ASSOC_RESPONSE_CAPABILITY_MISMATCH:
            return RestrictToNwifiStatus ? 
                DOT11_ASSOC_STATUS_UNREACHABLE :
                DOT11_ASSOC_STATUS_ASSOC_RESPONSE_CAPABILITY_MISMATCH;  // Maybe Remapped
                
        case WDI_ASSOC_STATUS_BAD_ASSOC_RESPONSE:
            return RestrictToNwifiStatus ? 
                DOT11_ASSOC_STATUS_UNREACHABLE :
                DOT11_ASSOC_STATUS_BAD_ASSOC_RESPONSE;  // Maybe Remapped
                
        case WDI_ASSOC_STATUS_ASSOC_FAILED_BY_PEER:
            return RestrictToNwifiStatus ?
                (DOT11_ASSOC_STATUS_ASSOCIATION_RESPONSE | ReasonCode) :
                (DOT11_ASSOC_STATUS_ASSOC_FAILED_BY_PEER | ReasonCode);  // Maybe Remapped to Existing
            
        case WDI_ASSOC_STATUS_ASSOC_EXCHANGE_FAILURE:
            return RestrictToNwifiStatus ? 
                DOT11_ASSOC_STATUS_UNREACHABLE :
                DOT11_ASSOC_STATUS_ASSOC_EXCHANGE_FAILURE;  // Maybe Remapped

        /* 
         * Disconnected (disassociated/deauthenticated) locally
         */
        case WDI_ASSOC_STATUS_DISASSOCIATED_BY_HOST:
            return DOT11_ASSOC_STATUS_DISASSOCIATED_BY_OS;          // Existing
            
        case WDI_ASSOC_STATUS_DISASSOCIATE_BY_DEVICE_RESET:
            return RestrictToNwifiStatus ?
                DOT11_ASSOC_STATUS_DISASSOCIATED_BY_RESET :
                DOT11_ASSOC_STATUS_DISASSOCIATE_BY_DEVICE_RESET;    // Maybe remapped
            
        case WDI_ASSOC_STATUS_DISASSOCIATE_UNABLE_TO_MAINTAIN:
            return RestrictToNwifiStatus ? 
                DOT11_DISASSOC_REASON_PEER_UNREACHABLE :
                DOT11_ASSOC_STATUS_DISASSOCIATE_UNABLE_TO_MAINTAIN;      // Maybe Remapped        
                
        case WDI_ASSOC_STATUS_DISASSOCIATE_NOT_VISIBLE:
            return RestrictToNwifiStatus ? 
                DOT11_DISASSOC_REASON_PEER_UNREACHABLE :      
                DOT11_ASSOC_STATUS_DISASSOCIATE_NOT_VISIBLE;        // Maybe remapped
                
        case WDI_ASSOC_STATUS_DISASSOCIATE_NEEDED_REASSOC:
            return RestrictToNwifiStatus ?
                DOT11_DISASSOC_REASON_PEER_UNREACHABLE :
                DOT11_ASSOC_STATUS_DISASSOCIATE_NEEDED_REASSOC;      // Maybe Remapped
            
        /*
         * Disconnected (disassociated/deauthenticated) remotely
         */
        case WDI_ASSOC_STATUS_PEER_DEAUTHENTICATED:
            return (DOT11_ASSOC_STATUS_PEER_DEAUTHENTICATED | ReasonCode);  // Existing
            
        case WDI_ASSOC_STATUS_PEER_DISASSOCIATED:
            return (DOT11_ASSOC_STATUS_PEER_DISASSOCIATED | ReasonCode);    // Existing
            
        /*
         * Roaming needed triggers. If disconnect
         * reasons are applicable, they should be used
         */
        case WDI_ASSOC_STATUS_ROAMING_BETTER_AP_FOUND:
            return DOT11_ASSOC_STATUS_ROAMING_BETTER_AP_FOUND;  // Existing
            
        case WDI_ASSOC_STATUS_ROAMING_ASSOCIATION_LOST:
            return DOT11_ASSOC_STATUS_ROAMING_ASSOCIATION_LOST; // Existing
            
        case WDI_ASSOC_STATUS_ROAMING_LOW_LINK_QUALITY:
            return RestrictToNwifiStatus ? 
                DOT11_ASSOC_STATUS_ROAMING_BETTER_AP_FOUND :
                DOT11_ASSOC_STATUS_ROAMING_LOW_LINK_QUALITY;  // Maybe Remapped
            
    }
    if (WabiStatus > DOT11_ASSOC_STATUS_IHV_START)
    {
        // IHV specific status is forwarded through
        return WabiStatus;
    }

    return DOT11_ASSOC_STATUS_FAILURE;
}

VOID CWabiToDot11Converter::MapDataRateEntry(
    PWDI_DATA_RATE_ENTRY   pWabiEntry,
    PDOT11_DATA_RATE_MAPPING_ENTRY pDot11Entry,
    UINT8             LastUsedNonStandardRateIndex
    )
{
    // These are the standard rates which need the same index
    static const UINT8 standardRateValues[] = {
        2, 4, 6, 9, 11, 12, 18, 22, 24, 36, 
        44, 48, 54, 66, 72, 96, 108
    };

    // These are the non standard rate indexes we can use. These are assigned sequentially
    static const UINT8 nonStandardRateIndexes[] = {
        1, 3, 5, 7, 8, 10, 
        13, 14, 15, 16, 17, 19, 20, 
        21, 23, 25, 26, 27, 28, 29, 30, 
        31, 32, 33, 34, 35, 37, 38, 39, 40, 
        41, 42, 43, 45, 46, 47, 48, 49, 50, 
        51, 52, 53, 55, 56, 57, 58, 59, 60, 
        61, 62, 63, 64, 65, 67, 68, 69, 70, 
        71, 73, 74, 75, 76, 77, 78, 79, 80, 
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 
        91, 92, 93, 94, 95, 97, 98, 99, 100,
        101, 102, 103, 104, 105, 106, 107, 109, 110
    };

    pDot11Entry->usDataRateValue = pWabiEntry->DataRateValue;

    // Assume its a non-standard rate
    pDot11Entry->ucDataRateFlag = DOT11_DATA_RATE_NON_STANDARD;    
    for (UINT8 j = 0; j < sizeof(standardRateValues); j++)
    {
        if (pWabiEntry->DataRateValue == standardRateValues[j])
        {
            // Is standard rate
            pDot11Entry->ucDataRateFlag = 0;
            break;
        }        
    }

    // OS expects different logic for indexes for standard & non-standard rates    
    if (pDot11Entry->ucDataRateFlag == DOT11_DATA_RATE_NON_STANDARD)
    {
        // Find the last used non-standard one & pick the next one
        pDot11Entry->ucDataRateIndex = 0;
        for (UINT8 i = 0; i < sizeof(nonStandardRateIndexes); i++)
        {
            if (nonStandardRateIndexes[i] > LastUsedNonStandardRateIndex)
            {
                // We crossed the last rate we used. Use the next one
                pDot11Entry->ucDataRateIndex = nonStandardRateIndexes[i];
                break;
            }
        }

        // Should have found a good rate to use since we can report at most 100 entries
        // and the combined tables above go upto 110 entries
        ASSERT(pDot11Entry->ucDataRateIndex != 0);        
    }
    else 
    {
        // Use rate as index
        pDot11Entry->ucDataRateIndex = (UINT8)pDot11Entry->usDataRateValue;
    }
}

DOT11_DS_INFO CWabiToDot11Converter::MapDsInfo(
    WDI_DS_INFO      DsInfo
    )
{
    switch (DsInfo)
    {
        case WDI_DS_CHANGED:
            return DOT11_DS_CHANGED;
        case WDI_DS_UNCHANGED:
            return DOT11_DS_UNCHANGED;
        case WDI_DS_UNKNOWN:
            return DOT11_DS_UNKNOWN;
    }

    return DOT11_DS_UNKNOWN;
}

DOT11_AUTH_ALGORITHM CWabiToDot11Converter::MapAuthAlgorithm(
    WDI_AUTH_ALGORITHM AuthAlgo
    )
{
    switch (AuthAlgo)
    {
    case WDI_AUTH_ALGO_80211_OPEN:
        return DOT11_AUTH_ALGO_80211_OPEN;
    case WDI_AUTH_ALGO_80211_SHARED_KEY:
        return DOT11_AUTH_ALGO_80211_SHARED_KEY;
    case WDI_AUTH_ALGO_WPA:
        return DOT11_AUTH_ALGO_WPA;
    case WDI_AUTH_ALGO_WPA_PSK:
        return DOT11_AUTH_ALGO_WPA_PSK;
    case WDI_AUTH_ALGO_WPA_NONE:
        return DOT11_AUTH_ALGO_WPA_NONE;
    case WDI_AUTH_ALGO_RSNA:
        return DOT11_AUTH_ALGO_RSNA;
    case WDI_AUTH_ALGO_RSNA_PSK:
        return DOT11_AUTH_ALGO_RSNA_PSK;
    default:
        return (DOT11_AUTH_ALGORITHM)AuthAlgo;

    }
}


DOT11_CIPHER_ALGORITHM CWabiToDot11Converter::MapCipherAlgorithm(
    WDI_CIPHER_ALGORITHM CipherAlgo
    )
{
    switch (CipherAlgo)
    {
    case WDI_CIPHER_ALGO_NONE:
        return DOT11_CIPHER_ALGO_NONE;
    case WDI_CIPHER_ALGO_WEP40:
        return DOT11_CIPHER_ALGO_WEP40;
    case WDI_CIPHER_ALGO_TKIP:
        return DOT11_CIPHER_ALGO_TKIP;
    case WDI_CIPHER_ALGO_CCMP:
        return DOT11_CIPHER_ALGO_CCMP;
    case WDI_CIPHER_ALGO_GCMP:
        return DOT11_CIPHER_ALGO_GCMP;
    case WDI_CIPHER_ALGO_WEP104:
        return DOT11_CIPHER_ALGO_WEP104;
    case WDI_CIPHER_ALGO_BIP:
        return DOT11_CIPHER_ALGO_BIP;
    case WDI_CIPHER_ALGO_WPA_USE_GROUP:
        return DOT11_CIPHER_ALGO_WPA_USE_GROUP;
    case WDI_CIPHER_ALGO_WEP:
        return DOT11_CIPHER_ALGO_WEP;
    default:
        return (DOT11_CIPHER_ALGORITHM)CipherAlgo;

    }
}


DOT11_ANQP_QUERY_RESULT CWabiToDot11Converter::MapANQPStatusCode(
    WDI_ANQP_QUERY_STATUS anqpStatusCode
    )
{
    switch (anqpStatusCode)
    {
    case WDI_ANQP_QUERY_STATUS_SUCCESS:
        return dot11_ANQP_query_result_success;
    case WDI_ANQP_QUERY_STATUS_FAILURE:
        return dot11_ANQP_query_result_failure;
    case WDI_ANQP_QUERY_STATUS_TIMED_OUT:
        return dot11_ANQP_query_result_timed_out;
    case WDI_ANQP_QUERY_STATUS_RESOURCES:
        return dot11_ANQP_query_result_resources;
    case WDI_ANQP_QUERY_STATUS_ADVERTISEMENT_PROTOCOL_NOT_SUPPORTED_ON_REMOTE:
        return dot11_ANQP_query_result_advertisement_protocol_not_supported_on_remote;
    case WDI_ANQP_QUERY_STATUS_GAS_PROTOCOL_FAILURE:
        return dot11_ANQP_query_result_gas_protocol_failure;
    case WDI_ANQP_QUERY_STATUS_ADVERTISEMENT_SERVER_NOT_RESPONDING:
        return dot11_ANQP_query_result_advertisement_server_not_responding;
    case WDI_ANQP_QUERY_STATUS_ACCESS_ISSUES:
        return dot11_ANQP_query_result_access_issues;
    default:
        return dot11_ANQP_query_result_failure;
    }
}

NDIS_STATUS CWabiToDot11Converter::MapTcpOffloadCapabilitiesToNdisOffload(
    _In_ WDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER TcpOffloadCapabilities,
    _Out_ PNDIS_OFFLOAD offloadAttributes
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    offloadAttributes->Checksum.IPv4Transmit.Encapsulation = TcpOffloadCapabilities.ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.Encapsulation;
    
    /* Convert the Encapsulation type from 802.11 to 802.3 */
    if (offloadAttributes->Checksum.IPv4Transmit.Encapsulation == WDI_ENCAPSULATION_IEEE_802_11)
    {
        offloadAttributes->Checksum.IPv4Transmit.Encapsulation = NDIS_ENCAPSULATION_IEEE_802_3;
    }

    offloadAttributes->Checksum.IPv4Transmit.IpChecksum = TcpOffloadCapabilities.ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.IpChecksum;
    offloadAttributes->Checksum.IPv4Transmit.IpOptionsSupported = TcpOffloadCapabilities.ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.IpOptionsSupported;
    offloadAttributes->Checksum.IPv4Transmit.TcpChecksum = TcpOffloadCapabilities.ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.TcpChecksum;
    offloadAttributes->Checksum.IPv4Transmit.TcpOptionsSupported = TcpOffloadCapabilities.ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.TcpOptionsSupported;
    offloadAttributes->Checksum.IPv4Transmit.UdpChecksum = TcpOffloadCapabilities.ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.UdpChecksum;

    offloadAttributes->Checksum.IPv4Receive.Encapsulation = TcpOffloadCapabilities.ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.Encapsulation;
    if (offloadAttributes->Checksum.IPv4Receive.Encapsulation == WDI_ENCAPSULATION_IEEE_802_11)
    {
        offloadAttributes->Checksum.IPv4Receive.Encapsulation = NDIS_ENCAPSULATION_IEEE_802_3;
    }

    offloadAttributes->Checksum.IPv4Receive.IpChecksum = TcpOffloadCapabilities.ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.IpChecksum;
    offloadAttributes->Checksum.IPv4Receive.IpOptionsSupported = TcpOffloadCapabilities.ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.IpOptionsSupported;
    offloadAttributes->Checksum.IPv4Receive.TcpChecksum = TcpOffloadCapabilities.ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.TcpChecksum;
    offloadAttributes->Checksum.IPv4Receive.TcpOptionsSupported = TcpOffloadCapabilities.ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.TcpOptionsSupported;
    offloadAttributes->Checksum.IPv4Receive.UdpChecksum = TcpOffloadCapabilities.ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.UdpChecksum;

    offloadAttributes->Checksum.IPv6Transmit.Encapsulation = TcpOffloadCapabilities.ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.Encapsulation;
    if (offloadAttributes->Checksum.IPv6Transmit.Encapsulation == WDI_ENCAPSULATION_IEEE_802_11)
    {
        offloadAttributes->Checksum.IPv6Transmit.Encapsulation = NDIS_ENCAPSULATION_IEEE_802_3;
    }

    offloadAttributes->Checksum.IPv6Transmit.IpExtensionHeadersSupported = TcpOffloadCapabilities.ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.IpExtensionHeadersSupported;
    offloadAttributes->Checksum.IPv6Transmit.TcpChecksum = TcpOffloadCapabilities.ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.TcpChecksum;
    offloadAttributes->Checksum.IPv6Transmit.TcpOptionsSupported = TcpOffloadCapabilities.ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.TcpOptionsSupported;
    offloadAttributes->Checksum.IPv6Transmit.UdpChecksum = TcpOffloadCapabilities.ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.UdpChecksum;

    offloadAttributes->Checksum.IPv6Receive.Encapsulation = TcpOffloadCapabilities.ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.Encapsulation;
    if (offloadAttributes->Checksum.IPv6Receive.Encapsulation == WDI_ENCAPSULATION_IEEE_802_11)
    {
        offloadAttributes->Checksum.IPv6Receive.Encapsulation = NDIS_ENCAPSULATION_IEEE_802_3;
    }

    offloadAttributes->Checksum.IPv6Receive.IpExtensionHeadersSupported = TcpOffloadCapabilities.ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.IpExtensionHeadersSupported;
    offloadAttributes->Checksum.IPv6Receive.TcpChecksum = TcpOffloadCapabilities.ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.TcpChecksum;
    offloadAttributes->Checksum.IPv6Receive.TcpOptionsSupported = TcpOffloadCapabilities.ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.TcpOptionsSupported;
    offloadAttributes->Checksum.IPv6Receive.UdpChecksum = TcpOffloadCapabilities.ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.UdpChecksum;

    offloadAttributes->LsoV1.IPv4.Encapsulation = TcpOffloadCapabilities.LsoV1Capabilities.Encapsulation;
    if (offloadAttributes->LsoV1.IPv4.Encapsulation == WDI_ENCAPSULATION_IEEE_802_11)
    {
        offloadAttributes->LsoV1.IPv4.Encapsulation = NDIS_ENCAPSULATION_IEEE_802_3;
    }

    offloadAttributes->LsoV1.IPv4.IpOptions = TcpOffloadCapabilities.LsoV1Capabilities.IpOptions;
    offloadAttributes->LsoV1.IPv4.MaxOffLoadSize = TcpOffloadCapabilities.LsoV1Capabilities.MaxOffLoadSize;
    offloadAttributes->LsoV1.IPv4.MinSegmentCount = TcpOffloadCapabilities.LsoV1Capabilities.MinSegmentCount;
    offloadAttributes->LsoV1.IPv4.TcpOptions = TcpOffloadCapabilities.LsoV1Capabilities.TcpOptions;

    offloadAttributes->LsoV2.IPv4.Encapsulation = TcpOffloadCapabilities.LsoV2Capabilities.Ipv4LSO.EncapsulationV4;
    if (offloadAttributes->LsoV2.IPv4.Encapsulation == WDI_ENCAPSULATION_IEEE_802_11)
    {
        offloadAttributes->LsoV2.IPv4.Encapsulation = NDIS_ENCAPSULATION_IEEE_802_3;
    }

    offloadAttributes->LsoV2.IPv4.MaxOffLoadSize = TcpOffloadCapabilities.LsoV2Capabilities.Ipv4LSO.MaxOffloadSizeV4;
    offloadAttributes->LsoV2.IPv4.MinSegmentCount = TcpOffloadCapabilities.LsoV2Capabilities.Ipv4LSO.MinSegmentCountV4;

    offloadAttributes->LsoV2.IPv6.Encapsulation = TcpOffloadCapabilities.LsoV2Capabilities.Ipv6LSO.EncapsulationV6;
    if (offloadAttributes->LsoV2.IPv6.Encapsulation == WDI_ENCAPSULATION_IEEE_802_11)
    {
        offloadAttributes->LsoV2.IPv6.Encapsulation = NDIS_ENCAPSULATION_IEEE_802_3;
    }

    offloadAttributes->LsoV2.IPv6.IpExtensionHeadersSupported = TcpOffloadCapabilities.LsoV2Capabilities.Ipv6LSO.IpExtensionHeadersSupported;
    offloadAttributes->LsoV2.IPv6.MaxOffLoadSize = TcpOffloadCapabilities.LsoV2Capabilities.Ipv6LSO.MaxOffloadSizeV6;
    offloadAttributes->LsoV2.IPv6.MinSegmentCount = TcpOffloadCapabilities.LsoV2Capabilities.Ipv6LSO.MinSegmentCountV6;
    offloadAttributes->LsoV2.IPv6.TcpOptionsSupported = TcpOffloadCapabilities.LsoV2Capabilities.Ipv6LSO.TcpOptionsSupported;

    offloadAttributes->Rsc.IPv4.Enabled = TcpOffloadCapabilities.ReceiveOffloadCapabilities.Ipv4Enabled;
    offloadAttributes->Rsc.IPv6.Enabled = TcpOffloadCapabilities.ReceiveOffloadCapabilities.Ipv6Enabled;

    return ndisStatus;
}

NDIS_STATUS CWabiToDot11Converter::CombineArrayOfByteArrays(
    _In_ ArrayOfElements<WDI_BYTE_BLOB> *pArrayOfByteArrays,
    _Outptr_result_bytebuffer_(*pOutputBufferLength) UINT8** ppOutputBuffer,
    _Out_ PUINT pOutputBufferLength,
    _Out_ bool* pbNeedsFree
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    UINT totalLength = 0;
    UINT8* pCombinedBuffer = NULL;
    UINT8* pCurrentOffset;
    *pbNeedsFree = false;
    *ppOutputBuffer = NULL;
    *pOutputBufferLength = 0;

    if (pArrayOfByteArrays->ElementCount == 0)
    {
        // Nothing is needed
        WFCTrace("[IhvExt] Zero ByteArrays provided");
        goto exit;
    }
    else if (pArrayOfByteArrays->ElementCount == 1)
    {
        // No need to allocate and copy. Use the original buffer
        *ppOutputBuffer = pArrayOfByteArrays->pElements[0].pElements;
        *pOutputBufferLength = pArrayOfByteArrays->pElements[0].ElementCount;
        WFCTrace("[IhvExt] Single ByteArrays with %d bytes provided", *pOutputBufferLength);
        goto exit;
    }

    // Need to actually combine the buffer
    for (UINT i = 0; i < pArrayOfByteArrays->ElementCount; i++)
    {
        totalLength += pArrayOfByteArrays->pElements[i].ElementCount;
    }
    WFCTrace("[IhvExt] %d TLVs contained %d bytes of total data", 
        pArrayOfByteArrays->ElementCount,
        totalLength);

    pCombinedBuffer = new UINT8[totalLength];
    if (pCombinedBuffer == NULL)
    {
        WFCError("[IhvExt] Failed to allocate buffer with %d bytes", totalLength);
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    pCurrentOffset = pCombinedBuffer;

    for (UINT i = 0; i < pArrayOfByteArrays->ElementCount; i++)
    {
        NT_ASSERT_ASSUME((pCurrentOffset + pArrayOfByteArrays->pElements[i].ElementCount) <= (pCombinedBuffer + totalLength));
        
        RtlCopyMemory(pCurrentOffset,
            pArrayOfByteArrays->pElements[i].pElements,
            pArrayOfByteArrays->pElements[i].ElementCount
            );

        pCurrentOffset += pArrayOfByteArrays->pElements[i].ElementCount;
    }

    *ppOutputBuffer = pCombinedBuffer;
    *pOutputBufferLength = totalLength;

    *pbNeedsFree = true;
    pCombinedBuffer = NULL;

exit:
    if (pCombinedBuffer)
    {
        delete[] pCombinedBuffer;
    }

    return ndisStatus;
}

DOT11_P2P_CHANNEL_INDICATE_REASON CWabiToDot11Converter::MapP2PChannelIndicateReason(
    _In_ WDI_P2P_CHANNEL_INDICATE_REASON WdiReason
    )
{
    switch ( WdiReason)
    {
        case WDI_P2P_CHANNEL_INDICATE_REASON_UNKNOWN:
        {
            return DOT11_P2P_CHANNEL_INDICATE_REASON_UNKNOWN;
        }

        case WDI_P2P_CHANNEL_INDICATE_REASON_NEW_CONNECTION:
        {
            return DOT11_P2P_CHANNEL_INDICATE_REASON_NEW_CONNECTION;
        }

        case WDI_P2P_CHANNEL_INDICATE_REASON_ECSA_REQUESTED:
        {
            return DOT11_P2P_CHANNEL_INDICATE_REASON_ECSA_REQUESTED;
        }

        case WDI_P2P_CHANNEL_INDICATE_REASON_ECSA_GO_INITIATED:
        {
            return DOT11_P2P_CHANNEL_INDICATE_REASON_ECSA_GO_INITIATED;
        }
    }

    return DOT11_P2P_CHANNEL_INDICATE_REASON_UNKNOWN;
}

