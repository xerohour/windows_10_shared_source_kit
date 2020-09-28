/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    offload.cpp

Abstract:

    Implementation for generic request handler

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "offload.tmh"

NDIS_STATUS CResumeOffloadJob::InitializeForResume( 
    _In_ CAdapter * pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _In_ ULONG OidInfoBufferSize,
    _In_ BYTE * pbOidInfoBuffer )
{
    NDIS_STATUS status;
    POID_HANDLER_ENTRY pHandlerInfo;

    m_pAdapter = pAdapter;
    m_NdisPortNumber = NdisPortNumber;

    //
    // Make a dummy OID so we can reuse SetPacketFilterJob
    //
    RtlZeroMemory( &m_OidRequest, sizeof( NDIS_OID_REQUEST) );
    m_OidRequest.RequestType = NdisRequestSetInformation;
    m_OidRequest.PortNumber = NdisPortNumber;
    m_OidRequest.DATA.SET_INFORMATION.Oid = OID_GEN_CURRENT_PACKET_FILTER;
    m_OidRequest.DATA.SET_INFORMATION.InformationBufferLength = OidInfoBufferSize;
    m_OidRequest.DATA.SET_INFORMATION.InformationBuffer = (PVOID) pbOidInfoBuffer;
    pHandlerInfo = COidJobBase::s_FindOidHandlerEntry( OID_PM_ADD_PROTOCOL_OFFLOAD);
    NT_ASSERT( pHandlerInfo );

    status = COidJobBase::InitializeForOid(
                        pHandlerInfo->SetMethodJobType,
                        pAdapter,
                        pHandlerInfo,
                        &m_OidRequest);
    NT_ASSERT( NT_SUCCESS( status ) );

    return status;
}



//
// CSetAddPmProtocolOffloadJob   implementation
//
NDIS_STATUS CSetAddPmProtocolOffloadJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();
    
    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;

    m_pPmProtocolOffload = (PNDIS_PM_PROTOCOL_OFFLOAD)
            pOidRequest->DATA.SET_INFORMATION.InformationBuffer;

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID CSetAddPmProtocolOffloadJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceEntry();

    TraceExit(NdisStatus);
}

VOID CSetAddPmProtocolOffloadJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    NDIS_STATUS wiFiStatus;
    NDIS_STATUS ndisStatus;

    TraceActEntry();

    if ( NT_SUCCESS( NdisStatus ) ) 
    {
        NdisStatus = CMessageHelper::GetStatusFromCommandResult( 
                        &m_DeviceCommand, 
                        &wiFiStatus );
        if ( NdisStatus == NDIS_STATUS_SUCCESS )
        {
            ndisStatus = AddOffloadToPropertyCache();
            UNREFERENCED_PARAMETER( ndisStatus );
            NT_ASSERT( ndisStatus == NDIS_STATUS_SUCCESS );
        }
    }

    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit( NdisStatus );
}

void CSetAddPmProtocolOffloadJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    m_pPort = GetAdapter()->GetPortFromNdisPortNumber( m_OidPortNumber );
    NT_ASSERT( m_pPort );

    ndisStatus = StartSetAddPmProtocolOffloadCommand();
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        CompleteJob( ndisStatus );
    }

    TraceActExit( ndisStatus );
}

NDIS_STATUS CSetAddPmProtocolOffloadJob::AddOffloadToPropertyCache()
{
    NDIS_STATUS status;

    CPort *pPort = m_pAdapter->GetPortFromNdisPortNumber( m_NdisPortNumber );
    CPropertyCache * pNewPropertyCache = new CPropertyCache;
    CPropertyCache * pPortPropertyCache;
    CPropertyCache * pFirstPropertyCache;

    if ( pNewPropertyCache == NULL )
    {
        WFCError( "AddOffloadToPropertyCache failed alloc CPropertyCache" );
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }
    status = pNewPropertyCache->Initialize( WfcPortPropertyNameMax, NULL );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError( "AddWolPatternToPropertyCache failed init new CPropertyCache 0x%x", status );
        goto Exit;
    }

    DtPrint( "Add to propertycache offload id=%d\n", m_pPmProtocolOffload->ProtocolOffloadId);
    status = pNewPropertyCache->SetPropertyBuffer(
                WfcPortPropertyProtocolOffload,
                GetOidRequest()->DATA.SET_INFORMATION.InformationBufferLength,
                (BYTE*)GetOidRequest()->DATA.SET_INFORMATION.InformationBuffer);
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError("AddOffloadToPropertyCache failed 0x%x", status);
        goto Exit;
    }

    pPortPropertyCache = pPort->GetPropertyCache();
    status = pPortPropertyCache->GetPropertyPropertyCacheList(
                    WfcPortPropertyProtocolOffloadList,
                    &pFirstPropertyCache );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        pFirstPropertyCache = NULL;
    }

    //
    // append
    //
    if ( pFirstPropertyCache  != NULL )
    {
        pNewPropertyCache->Next = pFirstPropertyCache;
    }
    else
    {
        pNewPropertyCache->Next = NULL;
    }

    status = pPortPropertyCache->SetPropertyPropertyCacheList(
                WfcPortPropertyProtocolOffloadList,
                pNewPropertyCache);
    if ( status == NDIS_STATUS_SUCCESS )
    {
        pNewPropertyCache = NULL;
    }

    Exit:
    if ( pNewPropertyCache )
    {
        delete pNewPropertyCache;
    }
    return status;
}


void CSetAddPmProtocolOffloadJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(SenderObject);
    TraceActEntry();

    CompleteJob(OperationStatus);
    TraceActExit( OperationStatus );
}


void CSetAddPmProtocolOffloadJob::OnJobCancelled()
{
    // We are not cancellable

    TraceActEntry();
    TraceActExit( STATUS_SUCCESS );
}


NDIS_STATUS
CSetAddPmProtocolOffloadJob::StartSetAddPmProtocolOffloadCommand(
    )
/*++

    Private function to send SetAddPmProtocolOffload command

--*/
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;
    UINT32 cbRead;

    TraceActEntry();

    // if this object is reused, make sure we free the buffer which was normally free in dtor
    if ( m_pbCommandBuffer )
    {
        FreeGenerated( m_pbCommandBuffer );
        m_pbCommandBuffer = NULL;
    }

    //
    // set up command message
    //
    ndisStatus = GenerateOffloadTlv( 
            m_pPmProtocolOffload,
            sizeof(WFC_MESSAGE_METADATA),
            &m_pbCommandBuffer,
            &m_cbCommandSize,
            &cbRead);

    if ( ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to Generate Protocol Ofload Tlv (%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = m_DeviceCommand.Initialize(
        m_pPort-> GetPortId(),
        WDI_SET_ADD_PM_PROTOCOL_OFFLOAD,
        m_cbCommandSize,
        m_pbCommandBuffer );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for SetAddWolPattern (%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = QueueDeviceCommand(&m_DeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start SetAddWolPattern cmd (%x)", ndisStatus);
        goto Exit;
    }

    Exit:
    {
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            CompleteJob( ndisStatus );
        }
        TraceActExit( ndisStatus );
        return ndisStatus;
    }
}


NDIS_STATUS 
CSetAddPmProtocolOffloadJob::GenerateOffloadTlv(
    _In_ PNDIS_PM_PROTOCOL_OFFLOAD pPmProtocolOffload,
    _In_ ULONG ReserveHeaderSize,
    _Outptr_result_buffer_(* pcbPmProtocolOffload ) BYTE** ppbPmProtocolOffload,
    _Out_ ULONG * pcbPmProtocolOffload,
    _Out_ UINT32 * pcbNeeded )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PNDIS_PM_PROTOCOL_OFFLOAD pPmProtOffloadTemp = pPmProtocolOffload;
    WDI_SET_ADD_PM_PROTOCOL_OFFLOAD_PARAMETERS_PARAMETERS parameters;
    UINT32 cbRead = 0;

    NT_ASSERT( ppbPmProtocolOffload && pcbPmProtocolOffload && pcbNeeded );
    *ppbPmProtocolOffload = NULL;
    *pcbPmProtocolOffload = 0;
    *pcbNeeded = 0;

    //
    //First get the size for tlv
    //
    do {
        switch ( pPmProtOffloadTemp->ProtocolOffloadType )
        {
            case NdisPMProtocolOffloadIdIPv4ARP:
                cbRead += sizeof( NDIS_PM_PROTOCOL_OFFLOAD );
                parameters.Optional.ipv4ARPOffload_IsPresent = TRUE;
                parameters.ipv4ARPOffload.ProtocolOffloadId = pPmProtOffloadTemp->ProtocolOffloadId;
                RtlCopyMemory( parameters.ipv4ARPOffload.RemoteIPV4Address,
                    pPmProtOffloadTemp->ProtocolOffloadParameters.IPv4ARPParameters.RemoteIPv4Address,
                    4 );
                RtlCopyMemory( parameters.ipv4ARPOffload.HostIPV4Address,
                    pPmProtOffloadTemp->ProtocolOffloadParameters.IPv4ARPParameters.HostIPv4Address,
                    4 );
                RtlCopyMemory( parameters.ipv4ARPOffload.MacAddress.Address,
                    pPmProtOffloadTemp->ProtocolOffloadParameters.IPv4ARPParameters.MacAddress,
                    6 );
                break;
                
            case NdisPMProtocolOffloadIdIPv6NS:
                parameters.Optional.ipv6ARPOffload_IsPresent = TRUE;
                parameters.ipv6ARPOffload.ProtocolOffloadId = pPmProtOffloadTemp->ProtocolOffloadId;
                RtlCopyMemory( parameters.ipv6ARPOffload.RemoteIPV6Address,
                    pPmProtOffloadTemp->ProtocolOffloadParameters.IPv6NSParameters.RemoteIPv6Address,
                    16 );
                RtlCopyMemory( parameters.ipv6ARPOffload.SolicitdNodeIPv6Address,
                    pPmProtOffloadTemp->ProtocolOffloadParameters.IPv6NSParameters.SolicitedNodeIPv6Address,
                    16 );
                RtlCopyMemory( parameters.ipv6ARPOffload.TargetIPV6Address1,
                    &pPmProtOffloadTemp->ProtocolOffloadParameters.IPv6NSParameters.TargetIPv6Addresses[0][0],
                    16 );
                RtlCopyMemory( parameters.ipv6ARPOffload.TargetIPV6Address2,
                    &pPmProtOffloadTemp->ProtocolOffloadParameters.IPv6NSParameters.TargetIPv6Addresses[1][0],
                    16 );
                RtlCopyMemory( parameters.ipv6ARPOffload.MacAddress.Address,
                    pPmProtOffloadTemp->ProtocolOffloadParameters.IPv6NSParameters.MacAddress,
                    6 );
                break;

            case NdisPMProtocolOffload80211RSNRekey:
                parameters.Optional.DOT11RSNREKeyOffload_IsPresent = TRUE;
                parameters.DOT11RSNREKeyOffload.ProtocolOffloadId = pPmProtOffloadTemp->ProtocolOffloadId;
                parameters.DOT11RSNREKeyOffload.ReplayCounter = pPmProtOffloadTemp->ProtocolOffloadParameters.
                    Dot11RSNRekeyParameters.KeyReplayCounter;
                RtlCopyMemory( &parameters.DOT11RSNREKeyOffload.KCK_CONTENT[0],
                    &pPmProtOffloadTemp->ProtocolOffloadParameters.Dot11RSNRekeyParameters.KCK[0],
                    DOT11_RSN_KCK_LENGTH );
                RtlCopyMemory( &parameters.DOT11RSNREKeyOffload.KEK_CONTENT[0],
                    &pPmProtOffloadTemp->ProtocolOffloadParameters.Dot11RSNRekeyParameters.KEK[0],
                    DOT11_RSN_KEK_LENGTH );
                break;

            default:
                NT_ASSERT( 0 );
                break;
        }
        if ( pPmProtOffloadTemp->NextProtocolOffloadOffset == 0 ) {
            break;
        }
        else {
            BYTE * pb = (BYTE*)pPmProtOffloadTemp+ pPmProtOffloadTemp->NextProtocolOffloadOffset;
            pPmProtOffloadTemp = (PNDIS_PM_PROTOCOL_OFFLOAD)pb;
        }
    } while ( pPmProtOffloadTemp );

    *pcbNeeded = cbRead;

    //
    // allocate and create TLVs
    //
    ndisStatus = GenerateWdiSetAddPmProtocolOffload( &parameters, ReserveHeaderSize, this->m_pAdapter->GetTlvContext(), pcbPmProtocolOffload, ppbPmProtocolOffload );

    return ndisStatus;
}



//++ CSetRemovePmProtocolOffloadJob ++

NDIS_STATUS CSetRemovePmProtocolOffloadJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;
    m_OffloadId = *(UINT32*)pOidRequest->DATA.SET_INFORMATION.InformationBuffer;

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID CSetRemovePmProtocolOffloadJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceEntry();

    TraceExit(NdisStatus);
}

NDIS_STATUS
CSetRemovePmProtocolOffloadJob::RemoveOffloadFromPropertyCache()
{
    NDIS_STATUS status=NDIS_STATUS_SUCCESS;

    CPort *pPort = m_pAdapter->GetPortFromNdisPortNumber( m_NdisPortNumber );
    CPropertyCache * pPortPropertyCache;
    CPropertyCache * pCurrPropertyCache;
    CPropertyCache *pPrevPropertyCache=NULL;
    CPropertyCache * pFirstPropertyCache;

    PNDIS_PM_PROTOCOL_OFFLOAD pProtocolOffload;
    ULONG ulSize;
    ULONG ulOffloadId = m_OffloadId;

    pPortPropertyCache = pPort->GetPropertyCache();
    status = pPortPropertyCache->GetPropertyPropertyCacheList(
                        WfcPortPropertyProtocolOffloadList,
                        &pFirstPropertyCache );

    if ( status != NDIS_STATUS_SUCCESS || pFirstPropertyCache == NULL )
    {
        WFCError("RemoveOffloadFromPropertyCache list is empty");
        goto Exit;
    }

    pCurrPropertyCache = pFirstPropertyCache;

    DtPrint( "Removing from propertycache Offload id=%d\n", ulOffloadId);

    for ( ;pCurrPropertyCache!= NULL; pCurrPropertyCache = pCurrPropertyCache->Next )
    {
        status = pCurrPropertyCache->GetPropertyBuffer(
                    WfcPortPropertyProtocolOffload,
                    &ulSize,
                    (BYTE**)&pProtocolOffload);
        if ( status == STATUS_SUCCESS )
        {
            DtPrintOff( "Comparing Offload Id against %d\n", 
                pProtocolOffload->ProtocolOffloadId );
            if ( ulSize >= sizeof( NDIS_PM_PROTOCOL_OFFLOAD ) &&
                pProtocolOffload->ProtocolOffloadId == ulOffloadId )
            {
                break;
            }
            pPrevPropertyCache = pCurrPropertyCache;
        }
    }

    if ( pCurrPropertyCache )
    {
        if ( pPrevPropertyCache == NULL )
        {
            //first one is the one to remove
            pFirstPropertyCache = pCurrPropertyCache->Next;
        }
        else
        {
            pPrevPropertyCache->Next = pCurrPropertyCache->Next;
        }
        delete pCurrPropertyCache;

        status = pPortPropertyCache->SetPropertyPropertyCacheList(
                        WfcPortPropertyProtocolOffloadList,
                        pFirstPropertyCache );
        NT_ASSERT( status == NDIS_STATUS_SUCCESS );
    }
    else
    {
        WFCError("RemoveOffloadFromPropertyCache id=%d does not exist", ulOffloadId);
    }

    Exit:
    return status;
}


VOID CSetRemovePmProtocolOffloadJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceActEntry();

    // Complete the OID back to NDIS
    NDIS_STATUS wiFiStatus;
    NDIS_STATUS localStatus;

    if ( NT_SUCCESS( NdisStatus ) ) 
    {
        NdisStatus = CMessageHelper::GetStatusFromCommandResult( &m_DeviceCommand, &wiFiStatus );
    }

    //
    // Driver Verifier in HCK allows only NOT_ACCESPTED, REQUEST_ABORTED or SUCESS.
    // anything else will cause a bugcheck.
    //
    if ( NdisStatus != NDIS_STATUS_SUCCESS ) {
        WFCActError("SetRemoveProtocolOffload error status 0x%x -> NDIS_STATUS_REQUEST_ABORTED", NdisStatus);
        NdisStatus = NDIS_STATUS_REQUEST_ABORTED;
    }
    else
    {
        localStatus = RemoveOffloadFromPropertyCache();
        UNREFERENCED_PARAMETER( localStatus );
        NT_ASSERT( localStatus == NDIS_STATUS_SUCCESS );
    }

    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit( NdisStatus );
}

void CSetRemovePmProtocolOffloadJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    m_pPort = GetAdapter()->GetPortFromNdisPortNumber( m_OidPortNumber );
    NT_ASSERT( m_pPort );

    if ( m_NdisPortNumber != NDIS_DEFAULT_PORT_NUMBER )
    {
        ndisStatus = STATUS_INVALID_DEVICE_REQUEST;
    }
    else 
    {
        ndisStatus = StartSetRemovePmProtocolOffloadCommand();
    }

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        CompleteJob( ndisStatus );
    }

    TraceActExit( ndisStatus );
}

void CSetRemovePmProtocolOffloadJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);
    TraceActEntry();
    CompleteJob(OperationStatus);
    TraceActExit( STATUS_SUCCESS );
}

void CSetRemovePmProtocolOffloadJob::OnJobCancelled()
{
    // We are not cancellable

    TraceActEntry();
    TraceActExit( STATUS_SUCCESS );
}


NDIS_STATUS
CSetRemovePmProtocolOffloadJob::StartSetRemovePmProtocolOffloadCommand()
/*++

    Private function to send SetRemovePmProtocolOffload command

--*/
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;
    WDI_SET_REMOVE_PM_PROTOCOL_OFFLOAD_PARAMETERS parameters;
    const PLACEMENT_NEW_ALLOCATION_CONTEXT pnac = { sizeof( m_pbCommandBuffer ), m_pbCommandBuffer };
    TLV_CONTEXT tlvCtx = *m_pAdapter->GetTlvContext();
    BYTE * pb = NULL;
    ULONG cb = 0;

    TraceActEntry();

    //
    // set up command message
    //
    tlvCtx.AllocationContext = (ULONG_PTR)(&pnac);
    parameters.RemovePMOffload = m_OffloadId;
    ndisStatus = GenerateWdiSetRemovePmProtocolOffload( &parameters, sizeof( WFC_MESSAGE_METADATA ), &tlvCtx, &cb, &pb );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError( "Failed to generate TLV for SetRemovePmProtocolOffload (0x%x)", ndisStatus );
        goto Exit;
    }
    NT_ASSERT( pb == m_pbCommandBuffer );
    NT_ASSERT( cb == m_cbCommand );

    ndisStatus = m_DeviceCommand.Initialize(
        m_pPort-> GetPortId(),
        WDI_SET_REMOVE_PM_PROTOCOL_OFFLOAD,
        m_cbCommand,
        m_pbCommandBuffer );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for SetRemovePmProtocolOffload (%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = QueueDeviceCommand(&m_DeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start SetRemovePmProtocolOffload cmd (%x)", ndisStatus);
        goto Exit;
    }

    Exit:
    {
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            CompleteJob( ndisStatus );
        }
        TraceActExit( ndisStatus );
        return ndisStatus;
    }
}


//++ CGetPmProtocolOffloadJob ++

NDIS_STATUS CGetPmProtocolOffloadJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;
    m_OffloadId = *(UINT32*)pOidRequest->DATA.SET_INFORMATION.InformationBuffer;

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID CGetPmProtocolOffloadJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceEntry();

    TraceExit(NdisStatus);
}

VOID CGetPmProtocolOffloadJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    WDI_GET_PM_PROTOCOL_OFFLOAD_RESULTS results;

    TraceActEntry();

    if ( NdisStatus != NDIS_STATUS_SUCCESS ) 
    {
        goto Exit;
    }
    
    //
    // get the reported info
    //
    BYTE* pb;
    ULONG cb;
    PWFC_MESSAGE_METADATA pMsgHdr;
    
    NdisStatus = m_DeviceCommand.get_CommandResult( &cb, &pb );
    if ( NdisStatus != NDIS_STATUS_SUCCESS ) 
    {
        goto Exit;
    }

    pMsgHdr = (PWFC_MESSAGE_METADATA) pb;
    NdisStatus = pMsgHdr->Message.Status;

    if ( NdisStatus != NDIS_STATUS_SUCCESS ) 
    {
        WFCActError("Get PmProtocolOffload WiFiStatus2Ndis 0x%x", NdisStatus);
        goto Exit;
    }

    NdisStatus = ParseWdiGetPmProtocolOffload( cb - sizeof( WFC_MESSAGE_METADATA ), pb + sizeof( WFC_MESSAGE_METADATA ), m_pAdapter->GetTlvContext(), &results );
    if (NdisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError( "Get PmProtocolOffload Parse failed 0x%x", NdisStatus );
        goto Exit;
    }

    PNDIS_PM_PROTOCOL_OFFLOAD pPmProtocolOffload =
        (PNDIS_PM_PROTOCOL_OFFLOAD)QUERY_OID_BUFFER( m_pOidRequest );

    if (results.Optional.ipv4ARPOffload_IsPresent)
    {
        NT_ASSERTMSG( "Can only get one offload", !results.Optional.ipv6ARPOffload_IsPresent );
        NT_ASSERTMSG( "Can only get one offload", !results.Optional.DOT11RSNREKeyOffload_IsPresent );

        RtlCopyMemory(
            pPmProtocolOffload->ProtocolOffloadParameters.IPv4ARPParameters.RemoteIPv4Address,
            results.ipv4ARPOffload.RemoteIPV4Address,
            4 );
        RtlCopyMemory(
            pPmProtocolOffload->ProtocolOffloadParameters.IPv4ARPParameters.HostIPv4Address,
            results.ipv4ARPOffload.HostIPV4Address,
            4 );
        RtlCopyMemory(
            pPmProtocolOffload->ProtocolOffloadParameters.IPv4ARPParameters.MacAddress,
            results.ipv4ARPOffload.MacAddress.Address,
            6 );
    }
    else if (results.Optional.ipv6ARPOffload_IsPresent)
    {
        NT_ASSERTMSG( "Can only get one offload", !results.Optional.DOT11RSNREKeyOffload_IsPresent );

        RtlCopyMemory(
            pPmProtocolOffload->ProtocolOffloadParameters.IPv6NSParameters.RemoteIPv6Address,
            results.ipv6ARPOffload.RemoteIPV6Address,
            16 );
        RtlCopyMemory(
            pPmProtocolOffload->ProtocolOffloadParameters.IPv6NSParameters.SolicitedNodeIPv6Address,
            results.ipv6ARPOffload.SolicitdNodeIPv6Address,
            16 );
        RtlCopyMemory(
            pPmProtocolOffload->ProtocolOffloadParameters.IPv6NSParameters.MacAddress,
            results.ipv6ARPOffload.MacAddress.Address,
            6 );
        RtlCopyMemory(
            pPmProtocolOffload->ProtocolOffloadParameters.IPv6NSParameters.TargetIPv6Addresses[0],
            results.ipv6ARPOffload.TargetIPV6Address1,
            16 );
        RtlCopyMemory(
            pPmProtocolOffload->ProtocolOffloadParameters.IPv6NSParameters.TargetIPv6Addresses[1],
            results.ipv6ARPOffload.TargetIPV6Address2,
            16 );
    }
    else if (results.Optional.DOT11RSNREKeyOffload_IsPresent)
    {
        RtlCopyMemory(
            pPmProtocolOffload->ProtocolOffloadParameters.Dot11RSNRekeyParameters.KCK,
            results.DOT11RSNREKeyOffload.KCK_CONTENT,
            16 );
        RtlCopyMemory(
            pPmProtocolOffload->ProtocolOffloadParameters.Dot11RSNRekeyParameters.KEK,
            results.DOT11RSNREKeyOffload.KEK_CONTENT,
            16 );

        pPmProtocolOffload->ProtocolOffloadParameters.Dot11RSNRekeyParameters.KeyReplayCounter =
            results.DOT11RSNREKeyOffload.ReplayCounter;
    }
    else
    {
        NdisStatus = NDIS_STATUS_INVALID_DATA;
        WFCActError( "Get PmProtocolOffload Output should have at least one protocol 0x%x", NdisStatus );
        goto Exit;
    }
    
    pPmProtocolOffload->Header.Type       = NDIS_OBJECT_TYPE_DEFAULT;
    pPmProtocolOffload->Header.Revision   = NDIS_PM_PROTOCOL_OFFLOAD_REVISION_1;
    pPmProtocolOffload->Header.Size       = NDIS_SIZEOF_NDIS_PM_PROTOCOL_OFFLOAD_REVISION_1;

    QUERY_OID_BYTES_WRITTEN(m_pOidRequest) = sizeof(NDIS_PM_PROTOCOL_OFFLOAD);

Exit:

    //
    // Complete the OID back to NDIS
    //
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit( NdisStatus );

}

void CGetPmProtocolOffloadJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    m_pPort = GetAdapter()->GetPortFromNdisPortNumber( m_OidPortNumber );
    NT_ASSERT( m_pPort );

    if ( m_NdisPortNumber != NDIS_DEFAULT_PORT_NUMBER )
    {
        ndisStatus = STATUS_INVALID_DEVICE_REQUEST;
    }
    else 
    {
        ndisStatus = StartGetPmProtocolOffloadCommand();
    }

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        CompleteJob( ndisStatus );
    }

    TraceActExit( ndisStatus );
}

void CGetPmProtocolOffloadJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);
    TraceActEntry();
    CompleteJob(OperationStatus);
    TraceActExit( STATUS_SUCCESS );
}

void CGetPmProtocolOffloadJob::OnJobCancelled()
{
    // We are not cancellable

    TraceActEntry();
    TraceActExit( STATUS_SUCCESS );
}


NDIS_STATUS
CGetPmProtocolOffloadJob::StartGetPmProtocolOffloadCommand()
/*++

    Private function to send GetPmProtocolOffload command

--*/
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;
    WDI_GET_PM_PROTOCOL_OFFLOAD_PARAMETERS parameters;
    const PLACEMENT_NEW_ALLOCATION_CONTEXT pnac = { sizeof( m_pbCommandBuffer ), m_pbCommandBuffer };
    TLV_CONTEXT tlvCtx = *m_pAdapter->GetTlvContext();
    BYTE * pb = NULL;
    ULONG cb = 0;

    TraceActEntry();

    //
    // set up command message
    //
    tlvCtx.AllocationContext = (ULONG_PTR)(&pnac);
    parameters.GetPMOffload = m_OffloadId;
    ndisStatus = GenerateWdiGetPmProtocolOffload( &parameters, sizeof( WFC_MESSAGE_METADATA ), &tlvCtx, &cb, &pb );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError( "Failed to generate TLV for GetPmProtocolOffload (0x%x)", ndisStatus );
        goto Exit;
    }
    NT_ASSERT( pb == m_pbCommandBuffer );
    NT_ASSERT( cb == m_cbCommand );

    ndisStatus = m_DeviceCommand.Initialize(
        m_pPort-> GetPortId(),
        WDI_GET_PM_PROTOCOL_OFFLOAD,
        m_cbCommand,
        m_pbCommandBuffer );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for GetPmProtocolOffload (%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = QueueDeviceCommand(&m_DeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start GetPmProtocolOffload cmd (%x)", ndisStatus);
        goto Exit;
    }

    Exit:
    {
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            CompleteJob( ndisStatus );
        }
        TraceActExit( ndisStatus );
        return ndisStatus;
    }
}



