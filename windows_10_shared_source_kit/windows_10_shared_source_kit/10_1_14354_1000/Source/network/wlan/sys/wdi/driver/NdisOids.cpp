/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    NdisOids.cpp

Abstract:

    Implementation for NDIS OID request handler

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "NdisOids.tmh"

NDIS_STATUS CSetPacketFilterJob::GetDeviceCommandData(
    _In_ PVOID             pInformationBuffer,
    _In_ UINT              InformationBufferLength,
    _In_ PUINT             pBytesRead,
    _In_ PUINT             pBytesNeeded,
    _In_ UINT              ReservedHeaderLength,
    _Out_ ULONG* pCommandId,
    _Out_ ULONG*           pCommandBufferLength,
    _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE**           pCommandBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG newPacketFilter;
    WDI_SET_RECEIVE_PACKET_FILTER_PARAMETERS receivePacketFilterParams;

    TraceActEntry();
    *pCommandId = WDI_SET_RECEIVE_PACKET_FILTER;

    *pBytesNeeded = sizeof(ULONG);

    if ( InformationBufferLength < sizeof(ULONG) )
    {
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }
    newPacketFilter = *((ULONG*) pInformationBuffer);
    *pBytesRead = InformationBufferLength;
    
    receivePacketFilterParams.PacketFilterType = CDot11ToWabiConverter::MapPacketFilterType(newPacketFilter);

    status = GenerateWdiSetReceivePacketFilter(
        &receivePacketFilterParams,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        pCommandBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't generate packet filter params tlv.");
        goto exit;
    }

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CSetPacketFilterJob::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG MessageLength,
    _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader )
{
    UNREFERENCED_PARAMETER(MessageLength);
    UNREFERENCED_PARAMETER(pMessageHeader);

    if (OperationStatus == NDIS_STATUS_SUCCESS)
    {
        // Cast is safe because buffer length is checked in CSetPacketFilterJob::GetDeviceCommandData
        GetPortPropertyCache()->SetPropertyULong(
            (WFC_PROPERTY_NAME)WfcPortPropertyNamePacketFilter,
            *((ULONG*)m_pInformationBuffer));
    }
    else
    {
        WFCError("WORKAROUND: HCK: Fake succeeding PACKET_FILTER set (Original Failure = 0x%08x)", OperationStatus);
        OperationStatus = NDIS_STATUS_SUCCESS;
    }
    
    return OperationStatus;
}


NDIS_STATUS CSetMulticastListJob::GetDeviceCommandData(
    _In_ PVOID             pInformationBuffer,
    _In_ UINT              InformationBufferLength,
    _In_ PUINT             pBytesRead,
    _In_ PUINT             pBytesNeeded,
    _In_ UINT              ReservedHeaderLength,
    _Out_ ULONG* pCommandId,
    _Out_ ULONG*           pCommandBufferLength,
    _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE**           pCommandBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG max_list_size = 0;
    WDI_SET_MULTICAST_LIST_PARAMETERS multicastListParams;
    
    TraceActEntry();

    *pBytesNeeded = 0;

    *pBytesRead = InformationBufferLength;
    *pCommandId = WDI_SET_MULTICAST_LIST;
    *pCommandBufferLength = 0;
    *pCommandBuffer = NULL;

    // validate info buffer
    status = m_pAdapter->GetAdapterPropertyCache()->GetMaxMulticastListSize(&max_list_size);

  
    if(status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't query max multicast list size property.");
        status = NDIS_STATUS_FAILURE;
        goto exit;
    }

    if (InformationBufferLength > (max_list_size * sizeof(WDI_MAC_ADDRESS))) 
    {
        WFCActErrorStatus("Multicast list set buffer contains too many entries: (%d) max size: (%d)\n", InformationBufferLength/sizeof(WDI_MAC_ADDRESS), max_list_size);
        status = NDIS_STATUS_MULTICAST_FULL;
        *pBytesNeeded = max_list_size * sizeof(WDI_MAC_ADDRESS);
        goto exit;
    }

    multicastListParams.MulticastList.ElementCount = InformationBufferLength / sizeof(WDI_MAC_ADDRESS);
        
    if (multicastListParams.MulticastList.ElementCount == 0)
    {
        multicastListParams.Optional.MulticastList_IsPresent = false;
    }
    else
    {
        multicastListParams.Optional.MulticastList_IsPresent = true;
        status = multicastListParams.MulticastList.AllocateElements(
            multicastListParams.MulticastList.ElementCount,
            0
            );
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Failed to allocate memory for multicast list with %u elements. Error = 0x%08x", multicastListParams.MulticastList.ElementCount, status);
            goto exit;
        }

        RtlCopyMemory(
            multicastListParams.MulticastList.pElements,
            pInformationBuffer,
            InformationBufferLength
            );
    }

    status = GenerateWdiSetMulticastList(&multicastListParams,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        pCommandBuffer
        );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't generate multicast list tlv.");
        goto exit;
    }
    
exit:

    TraceActExit(status);
    return status;
}

NDIS_STATUS CSetMulticastListJob::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG MessageLength,
    _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader )
{
    UNREFERENCED_PARAMETER(MessageLength);
    UNREFERENCED_PARAMETER(pMessageHeader);

    if (OperationStatus == NDIS_STATUS_SUCCESS)
    {
        GetPortPropertyCache()->SetPropertyBuffer(
            m_MulticastPropertyName,
            m_InformationBufferLength,
             (BYTE*) m_pInformationBuffer);
    }
    
    return OperationStatus;
}

//==============================================================================================
NDIS_STATUS CQueryMulticastListJob ::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _In_ PUINT  pBytesWritten,
    _In_ PUINT  pBytesNeeded
    )
{
    BYTE* pMulticastList =  NULL;
    ULONG MulticastListLength = 0;
    NDIS_STATUS ndisStatus;

    *pBytesNeeded = *pBytesWritten = 0;
    
    ndisStatus = GetPortPropertyCache()->GetPropertyBuffer(
        m_MulticastPropertyName,
        &MulticastListLength,
        &pMulticastList);

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {       
        WFCActError("Unable to find WfcPortPropertyNameMulticastList property in port Property Cache. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    *pBytesNeeded = MulticastListLength;

    if(InformationBufferLength < *pBytesNeeded)
    {
        ndisStatus = NDIS_STATUS_BUFFER_TOO_SHORT;
        WFCActError("CQueryMulticastListJob ::QueryData. Buffer too short Error = 0x%08x", ndisStatus);
        goto exit;
    }

    if (MulticastListLength > 0)
    {
        NdisMoveMemory (pInformationBuffer, pMulticastList, MulticastListLength);
    }
    
    *pBytesWritten = *pBytesNeeded;

exit:
    return ndisStatus;
};


//==============================================================================================
NDIS_STATUS CQueryVendorDescriptionJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _In_ PUINT  pBytesWritten,
    _In_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_NOT_SUPPORTED;

    UNREFERENCED_PARAMETER(pInformationBuffer);
    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(pBytesWritten);
    UNREFERENCED_PARAMETER(pBytesNeeded);
    WFCActError("Query for OID_GEN_VENDOR_DESCRIPTION not supported");
    
    return ndisStatus;
}
//==============================================================================================
NDIS_STATUS CQueryVendorDriverVersionJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _In_ PUINT  pBytesWritten,
    _In_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG versionInfo = (m_pAdapter->GetMiniportDriver()->GetNdisCharacteristics()->MajorDriverVersion << 16) | 
            m_pAdapter->GetMiniportDriver()->GetNdisCharacteristics()->MinorDriverVersion;
        
    UNREFERENCED_PARAMETER(InformationBufferLength);
    *pBytesNeeded = sizeof(ULONG);
    NdisMoveMemory(pInformationBuffer, &versionInfo, sizeof(ULONG));
    *pBytesWritten = sizeof(ULONG);
    return ndisStatus;
}

//==============================================================================================
NDIS_STATUS CQueryVendorIDJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _In_ PUINT  pBytesWritten,
    _In_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CPort* pPort = m_pAdapter->GetPortFromNdisPortNumber(
        get_NdisPortNumber());

    UNREFERENCED_PARAMETER(InformationBufferLength);
    TraceActEntry();

    NT_ASSERT(pInformationBuffer);

    *pBytesNeeded = 4;
    RtlZeroMemory(pInformationBuffer, 4);

    // Copy the first three bytes of the MAC address as out Vendor ID (4th byte is 0)
    RtlCopyMemory(pInformationBuffer, 
        pPort->GetMacAddress(), 
        3
        );   

    *pBytesWritten = *pBytesNeeded;

    TraceActExit(status);
    return status;
}

NDIS_STATUS CTcpOffloadSetParametersJob::GetDeviceCommandData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded,
    _In_ UINT   ReservedHeaderLength,
    _Out_ ULONG* pCommandId,
    _Out_ ULONG* pCommandBufferLength,
    _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WDI_SET_TCP_OFFLOAD_PARAMS_PARAMETERS TcpOffloadParameters;
    NDIS_OFFLOAD_PARAMETERS offloadParams = *(NDIS_OFFLOAD_PARAMETERS*)pInformationBuffer;

    *pBytesRead = InformationBufferLength;
    
    TcpOffloadParameters.SetOffloadParameters.IPsecV1 = offloadParams.IPsecV1;
    TcpOffloadParameters.SetOffloadParameters.IPv4Checksum = offloadParams.IPv4Checksum;
    TcpOffloadParameters.SetOffloadParameters.LsoV1= offloadParams.LsoV1;
    TcpOffloadParameters.SetOffloadParameters.LsoV2IPv4 = offloadParams.LsoV2IPv4;
    TcpOffloadParameters.SetOffloadParameters.LsoV2IPv6 = offloadParams.LsoV2IPv6;
    TcpOffloadParameters.SetOffloadParameters.TcpConnectionIPv4 = offloadParams.TcpConnectionIPv4;
    TcpOffloadParameters.SetOffloadParameters.TcpConnectionIPv6 = offloadParams.TcpConnectionIPv6;
    TcpOffloadParameters.SetOffloadParameters.TCPIPv4Checksum = offloadParams.TCPIPv4Checksum;
    TcpOffloadParameters.SetOffloadParameters.TCPIPv6Checksum = offloadParams.TCPIPv6Checksum;
    TcpOffloadParameters.SetOffloadParameters.UDPIPv4Checksum = offloadParams.UDPIPv4Checksum;
    TcpOffloadParameters.SetOffloadParameters.UDPIPv6Checksum = offloadParams.UDPIPv6Checksum;
    TcpOffloadParameters.SetOffloadParameters.RscIPv4 = offloadParams.RscIPv4;
    TcpOffloadParameters.SetOffloadParameters.RscIPv6 = offloadParams.RscIPv6;

    TcpOffloadParameters.SetOffloadParameters.Flags = offloadParams.Flags;
    TcpOffloadParameters.SetOffloadParameters.IPsecV2 = offloadParams.IPsecV2;
    TcpOffloadParameters.SetOffloadParameters.IPsecV2IPv4 = offloadParams.IPsecV2IPv4;
    TcpOffloadParameters.SetOffloadParameters.EncapsulatedPacketTaskOffload = offloadParams.EncapsulatedPacketTaskOffload;
    TcpOffloadParameters.SetOffloadParameters.EncapsulationTypes = offloadParams.EncapsulationTypes;

    ndisStatus = GenerateWdiSetTcpOffloadParameters(&TcpOffloadParameters,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        pCommandBuffer);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError( "Couldn't generate TCP Offload parameters tlv (0x%08X).", ndisStatus );
        if (NT_SUCCESS( ndisStatus ))
        {
            ndisStatus = NDIS_STATUS_FAILURE;
        }
        goto exit;
    }

    *pCommandId = WDI_SET_TCP_OFFLOAD_PARAMETERS;
    *pBytesNeeded = *pBytesRead;


exit:
    TraceActExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTcpEncapsulationOffloadParametersJob::GetDeviceCommandData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded,
    _In_ UINT   ReservedHeaderLength,
    _Out_ ULONG* pCommandId,
    _Out_ ULONG* pCommandBufferLength,
    _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    NDIS_OFFLOAD_ENCAPSULATION encapsulationOffloadParameters = *(NDIS_OFFLOAD_ENCAPSULATION*)pInformationBuffer;
    WDI_SET_ENCAPSULATION_OFFLOAD_PARAMS_PARAMETERS setEncapsulationOffloadParameters;

    if ((encapsulationOffloadParameters.Header.Type != NDIS_OBJECT_TYPE_OFFLOAD_ENCAPSULATION) ||
        (encapsulationOffloadParameters.Header.Revision < NDIS_OFFLOAD_ENCAPSULATION_REVISION_1) ||
        (encapsulationOffloadParameters.Header.Size < NDIS_SIZEOF_OFFLOAD_ENCAPSULATION_REVISION_1))
    {
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto exit;
    }

    *pBytesRead = InformationBufferLength;

    /*Note: We have currently added support just for the IpV4 and IpV6 Enabled fields
    There are two other parameters defined in the NDIS_OFFLOAD_ENCAPSULATION structure which we don't send 
    1: The encapsulation type - can have only one value WDI_ENCAPSULATION_802_11 
    and it need not be sent to the LE. 
    2: Header size - It is the ethernet header size used by the protocol driver
    This is not being used by the LE and so we do not send it currently
    
    In future, if they need them we can add it to the parser*/
    setEncapsulationOffloadParameters.EnabledV4 = (UINT8)encapsulationOffloadParameters.IPv4.Enabled;
    setEncapsulationOffloadParameters.EnabledV6 = (UINT8)encapsulationOffloadParameters.IPv6.Enabled;

    ndisStatus = GenerateWdiSetEncapsulationOffload(&setEncapsulationOffloadParameters,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext(),
        pCommandBufferLength,
        pCommandBuffer
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Couldn't generate Encapsulation Offload parameters tlv (0x%08X).", ndisStatus);
        if (NT_SUCCESS(ndisStatus))
        {
            ndisStatus = NDIS_STATUS_FAILURE;
        }
        goto exit;
    }

    *pCommandId = WDI_SET_ENCAPSULATION_OFFLOAD;
    *pBytesNeeded = *pBytesRead;


exit:
    TraceActExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTcpOffloadCurrentConfigJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER TcpOffloadCapabilities;

    PNDIS_OFFLOAD pOffloadAttributes;
    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    *pBytesNeeded = sizeof(NDIS_OFFLOAD);

    if (InformationBufferLength < *pBytesNeeded)
    {
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }
    RtlZeroMemory(pInformationBuffer, InformationBufferLength);

    pOffloadAttributes = (PNDIS_OFFLOAD)pInformationBuffer;
    status = pPort->GetPropertyCache()->GetTCPOffloadParametersFromPortPropertyCache(
        &TcpOffloadCapabilities);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Couldn't retrieve Offload Parameters from property cache, returning NULL");
        pOffloadAttributes = NULL;
        goto exit;
    }

    BOOLEAN bTaskOffloadDisabled = false;
    status = pPort->GetPropertyCache()->GetPropertyBoolean(
        WfcPortPropertyTaskOffloadsDisabledForFIPS,
        &bTaskOffloadDisabled
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Could not query WfcAdapterPropertyTaskOffloadsDisabledForFIPS (0x%08X) assuming non FIPS mode", status);
        bTaskOffloadDisabled = FALSE;
    }

    if (bTaskOffloadDisabled)
    {
        pPort->PopulateTaskOffloadDisabledParameters(&TcpOffloadCapabilities);
    }

    WFC_ASSIGN_NDIS_OBJECT_HEADER(pOffloadAttributes->Header,
        NDIS_OBJECT_TYPE_OFFLOAD,
        NDIS_OFFLOAD_REVISION_3,
        NDIS_SIZEOF_NDIS_OFFLOAD_REVISION_3);

    CWabiToDot11Converter::MapTcpOffloadCapabilitiesToNdisOffload(TcpOffloadCapabilities, pOffloadAttributes);
    *pBytesWritten = *pBytesNeeded;

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CTcpOffloadHardwareCapsJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER TcpOffloadCapabilities;

    PNDIS_OFFLOAD pOffloadAttributes;

    *pBytesNeeded = sizeof(NDIS_OFFLOAD);

    if (InformationBufferLength < *pBytesNeeded)
    {
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }
    RtlZeroMemory(pInformationBuffer, InformationBufferLength);

    pOffloadAttributes = (PNDIS_OFFLOAD)pInformationBuffer;
    status = m_pAdapter->GetAdapterPropertyCache()->GetTCPOffloadParametersFromPropertyCache(
        &TcpOffloadCapabilities);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Couldn't retrieve Offload Parameters from property cache, returning NULL");
        pOffloadAttributes = NULL;
        goto exit;
    }

    WFC_ASSIGN_NDIS_OBJECT_HEADER(pOffloadAttributes->Header,
        NDIS_OBJECT_TYPE_OFFLOAD,
        NDIS_OFFLOAD_REVISION_3,
        NDIS_SIZEOF_NDIS_OFFLOAD_REVISION_3);

    CWabiToDot11Converter::MapTcpOffloadCapabilitiesToNdisOffload(TcpOffloadCapabilities, pOffloadAttributes);
    *pBytesWritten = *pBytesNeeded;

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CTcpOffloadOffloadRscStatisticsJob::GetDeviceCommandId(
    _Out_ ULONG* pCommandId)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    *pCommandId = WDI_TCP_RSC_STATISTICS;

    TraceActExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTcpOffloadOffloadRscStatisticsJob::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG CommandBufferLength,
    _In_reads_bytes_(CommandBufferLength) BYTE* pCommandBuffer,
    _In_ PVOID pInformationBuffer,
    _In_ UINT InformationBufferLength,
    _Inout_ PUINT pBytesWritten,
    _Inout_ PUINT pBytesNeeded)
{
    PNDIS_RSC_STATISTICS_INFO pRscStatisticsInfo;
    WDI_TCP_RSC_STATISTICS_PARAMETERS RscStatisticsInfoParams;
    RtlZeroMemory(pInformationBuffer, InformationBufferLength);
    pRscStatisticsInfo = (PNDIS_RSC_STATISTICS_INFO)pInformationBuffer;

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("TCP offload rsc statistics failed by device. Error = 0x%08x)", OperationStatus);
        goto exit;
    }

    WFC_ASSIGN_NDIS_OBJECT_HEADER(
        pRscStatisticsInfo->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        NDIS_RSC_STATISTICS_REVISION_1,
        NDIS_SIZEOF_RSC_STATISTICS_REVISION_1
        );

    OperationStatus = ParseWdiTcpRscStatistics(
        CommandBufferLength - sizeof(WFC_MESSAGE_METADATA),
        pCommandBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &RscStatisticsInfoParams
        );
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError ("Couldn't Parse TCP RSC Statistics (0x%x)", OperationStatus);
        goto exit;
    }

    *pBytesWritten = sizeof(NDIS_RSC_STATISTICS_INFO);

    pRscStatisticsInfo->CoalescedOctets = RscStatisticsInfoParams.RscStatsParams.CoalescedOctets;
    pRscStatisticsInfo->CoalescedPkts = RscStatisticsInfoParams.RscStatsParams.CoalescedPkts;
    pRscStatisticsInfo->CoalesceEvents = RscStatisticsInfoParams.RscStatsParams.CoalesceEvents;
    pRscStatisticsInfo->Aborts = RscStatisticsInfoParams.RscStatsParams.Aborts;

    *pBytesNeeded = *pBytesWritten;
exit:
    TraceActExit(OperationStatus);
    return OperationStatus;
}

//==============================================================================================

NDIS_STATUS CQuerySupportedGuidsJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _In_ PUINT  pBytesWritten,
    _In_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS ndisStatus;
    ULONG SupportedGuidsLength = 0;
    UINT16 numSupportedGuids = 0;
    BYTE* pSupportedGuids = NULL;

    ndisStatus = m_pAdapter->GetAdapterPropertyCache()->GetPropertyList(WfcAdapterSupportedGuids,
        &SupportedGuidsLength,
        &numSupportedGuids,
        (BYTE **)&pSupportedGuids);

    if (( ndisStatus != NDIS_STATUS_SUCCESS ) ||
        ( SupportedGuidsLength == 0 ))
    {       
        WFCActError("Unable to find WfcAdapterSupportedGuids property in adapter Property Cache. Error = 0x%08x, SupportedGuidsLength=%d",
            ndisStatus, SupportedGuidsLength);

        *pBytesWritten = 0;
        ndisStatus = NDIS_STATUS_SUCCESS;
        goto exit;
    }

    *pBytesNeeded = SupportedGuidsLength;

    if(InformationBufferLength < *pBytesNeeded)
    {
        ndisStatus = NDIS_STATUS_BUFFER_TOO_SHORT;
        WFCActError("CQuerySupportedGuidsJob ::QueryData. Buffer too short <%d < %d>",
            InformationBufferLength, *pBytesNeeded);
        goto exit;
    }

    if (SupportedGuidsLength > 0)
    {
        NdisMoveMemory (pInformationBuffer, pSupportedGuids, SupportedGuidsLength);
    }
    
    *pBytesWritten = *pBytesNeeded;

exit:
    return ndisStatus;
}

//==============================================================================================

CUnrecognizedOidHandlerJob::CUnrecognizedOidHandlerJob(
    _In_ WFC_ACTIVITY_ID ActivityId) 
    : COidJobBase(ActivityId),
    m_DeviceCommand(ActivityId),
    m_DirectOid(false)
{
    RtlZeroMemory(&m_CommandHeader, sizeof(m_CommandHeader));
}

CUnrecognizedOidHandlerJob::~CUnrecognizedOidHandlerJob ()
{
}

// COidJobBase
NDIS_STATUS CUnrecognizedOidHandlerJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{    
    UNREFERENCED_PARAMETER(pOidRequest);
    if (m_pHandlerInfo->IsDirectOid)
        m_DirectOid = true;
    
    return NDIS_STATUS_SUCCESS;
}

VOID CUnrecognizedOidHandlerJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);        
}
    
// CJobBase
void CUnrecognizedOidHandlerJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();
    status = m_DeviceCommand.Initialize(
        get_PortId(),
        WDI_INTERNAL_RAW_OID_REQUEST,
        sizeof(WFC_MESSAGE_METADATA),
        (BYTE *)&m_CommandHeader);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't initialize device command with given OID request.");
        goto exit;
    }


    if (m_DirectOid)
    {
        m_CommandHeader.UseDirectOidCall = TRUE;
    }
    m_CommandHeader.NonWdiOid = TRUE;
    
    m_CommandHeader.Command()->pOriginalOidRequest = COidJobBase::GetOidRequest();    
    
    status = QueueDeviceCommand(
        &m_DeviceCommand);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Queue device command failed.");
        goto exit;
    }

    WFCActInfo("Device Command Queued. Waiting for completion.");
        
exit:
    if ( status != NDIS_STATUS_SUCCESS )
    {
        // Just complete the job
        CompleteJob(status);
    }

    TraceActExit(status);
}


void CUnrecognizedOidHandlerJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(SenderObject);
    NT_ASSERT(
        SenderObject == &m_DeviceCommand );
    
    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Device command failed with status %x.", OperationStatus);
        goto exit;
    }

    WFCActInfo("Device Command succeeded.");
    
exit:
    // Notify the complete
    CompleteJob(
        OperationStatus );
}

void CUnrecognizedOidHandlerJob::OnJobCancelled()
{

}

