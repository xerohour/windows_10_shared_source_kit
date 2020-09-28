/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Coalesce.cpp

Abstract:

    Implementation for generic request handler

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "Coalesce.tmh"

NDIS_STATUS CResumeCoalesceJob::InitializeForResume( 
    _In_ CAdapter * pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _In_ ULONG OidInfoBufferSize,
    _In_ BYTE * pbOidInfoBuffer )
{
    NDIS_STATUS status;
    POID_HANDLER_ENTRY pHandlerInfo;
    NDIS_OID oid = OID_RECEIVE_FILTER_SET_FILTER;

    m_pAdapter = pAdapter;
    m_NdisPortNumber = NdisPortNumber;

    //
    // Make a dummy OID so we can reuse SetPacketFilterJob
    //
    RtlZeroMemory( &m_OidRequest, sizeof( NDIS_OID_REQUEST) );
    m_OidRequest.RequestType = NdisRequestMethod;
    m_OidRequest.PortNumber = NdisPortNumber;
    m_OidRequest.DATA.SET_INFORMATION.Oid = oid;
    m_OidRequest.DATA.METHOD_INFORMATION.InformationBuffer = (PVOID) pbOidInfoBuffer;
    m_OidRequest.DATA.METHOD_INFORMATION.InputBufferLength = OidInfoBufferSize;
    m_OidRequest.DATA.METHOD_INFORMATION.OutputBufferLength = OidInfoBufferSize;
    pHandlerInfo = COidJobBase::s_FindOidHandlerEntry( oid);
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
//++ CSetReceiveCoalescingJob   implementation =====
//
NDIS_STATUS CSetReceiveCoalescingJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();
    
    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;

    m_pReceiveFilterParams = (PNDIS_RECEIVE_FILTER_PARAMETERS)
            pOidRequest->DATA.SET_INFORMATION.InformationBuffer;

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID CSetReceiveCoalescingJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceEntry();

    TraceExit(NdisStatus);
}

VOID CSetReceiveCoalescingJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceActEntry();

    // Complete the OID back to NDIS
    NDIS_STATUS wiFiStatus;
    NDIS_STATUS ndisStatus;

    if ( NT_SUCCESS( NdisStatus ) ) 
    {
        NdisStatus = CMessageHelper::GetStatusFromCommandResult( 
            &m_DeviceCommand,
            &wiFiStatus );
        if ( NT_SUCCESS(NdisStatus)) 
        {
            ndisStatus = AddReceiveCoalesceToPropertyCache();
            UNREFERENCED_PARAMETER( ndisStatus );
            NT_ASSERT( ndisStatus == NDIS_STATUS_SUCCESS );
        }
    }

    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit( NdisStatus );
}

void CSetReceiveCoalescingJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    m_pPort = GetAdapter()->GetPortFromNdisPortNumber( m_OidPortNumber );
    NT_ASSERT( m_pPort );

    ndisStatus = StartSetReceiveCoalescingCommand();
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        CompleteJob( ndisStatus );
    }

    TraceActExit( ndisStatus );
}

void CSetReceiveCoalescingJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);
    TraceActEntry();

    CompleteJob(OperationStatus);
    TraceActExit( OperationStatus );
}

void CSetReceiveCoalescingJob::OnJobCancelled()
{
    // We are not cancellable

    TraceActEntry();
    TraceActExit( STATUS_SUCCESS );
}


NDIS_STATUS
CSetReceiveCoalescingJob::AddReceiveCoalesceToPropertyCache()
{
    NDIS_STATUS status;

    CPort * pPort = GetAdapter()->GetPortFromNdisPortNumber( m_NdisPortNumber );
    CPropertyCache * pNewPropertyCache = new CPropertyCache;
    CPropertyCache * pPortPropertyCache;
    CPropertyCache * pFirstPropertyCache;

    if ( pNewPropertyCache == NULL )
    {
        WFCError( "AddReceiveCoalesceToPropertyCache failed alloc CPropertyCache" );
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }
    status = pNewPropertyCache->Initialize( WfcPortPropertyNameMax, NULL );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError( "AddWolPatternToPropertyCache failed init new CPropertyCache 0x%x", status );
        goto Exit;
    }

    status = pNewPropertyCache->SetPropertyBuffer(
                WfcPortPropertyReceiveCoalesce,
                GetOidRequest()->DATA.SET_INFORMATION.InformationBufferLength,
                (BYTE*)GetOidRequest()->DATA.SET_INFORMATION.InformationBuffer);
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError("AddReceiveCoalesceToPropertyCache failed 0x%x", status);
        goto Exit;
    }

    pPortPropertyCache = pPort->GetPropertyCache();
    status = pPortPropertyCache->GetPropertyPropertyCacheList(
                    WfcPortPropertyReceiveCoalesceList,
                    &pFirstPropertyCache );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        pFirstPropertyCache = NULL;
    }

    PNDIS_RECEIVE_FILTER_PARAMETERS pCoalesceParams = (PNDIS_RECEIVE_FILTER_PARAMETERS)
                GetOidRequest()->DATA.SET_INFORMATION.InformationBuffer;
    UNREFERENCED_PARAMETER(pCoalesceParams); // referenced in case build without DtPrint.
    DtPrint( "Add to propertyCache coalesce id=%d @%p\n" , pCoalesceParams->FilterId, pNewPropertyCache);
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
                WfcPortPropertyReceiveCoalesceList,
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


NDIS_STATUS
CSetReceiveCoalescingJob::StartSetReceiveCoalescingCommand(
    )
/*++

    Private function to send SetReceiveCoalescing command

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
    ndisStatus = GenerateReceiveCoalescingTlv( 
            m_pReceiveFilterParams,
            sizeof(WFC_MESSAGE_METADATA),
            &m_pbCommandBuffer,
            &m_cbCommand,
            &cbRead);

    if ( ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to Generate ReceiveFilterSetFilter Tlv (%x)", ndisStatus);
        goto Exit;
    }

    DtPrint( "ctor coalese alloc TLV cmd %p\n", m_pbCommandBuffer );

    ndisStatus = m_DeviceCommand.Initialize(
        m_pPort-> GetPortId(),
        WDI_SET_RECEIVE_COALESCING,
        m_cbCommand,
        m_pbCommandBuffer );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for SetReceivePacketFilter 0x%x", ndisStatus);
        goto Exit;
    }

    ((PWFC_MESSAGE_METADATA)m_pbCommandBuffer)->Command()->MinimumOutputLength = 
            METHOD_OID_OUTPUT_LENGTH(GetOidRequest());

    ndisStatus = QueueDeviceCommand(&m_DeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start SetReceivePacketFilter cmd (%x)", ndisStatus);
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
CSetReceiveCoalescingJob::GenerateReceiveCoalescingTlv(
    _In_ PNDIS_RECEIVE_FILTER_PARAMETERS pReceiveFilterParams,
    _In_ UINT32 ReserveHeaderSize,
    _Outptr_result_buffer_(* pcbReceiveFilterParams) BYTE** ppbReceiveFilterParams,
    _Out_ ULONG * pcbReceiveFilterParams,
    _Out_ UINT32 * pcbNeeded )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    UINT32 cbRead =0;
    BYTE * pbFilterFieldContentSrc;
    WDI_SET_RECEIVE_COALESCING_PARAMETERS TlvCoalescingInfo;
    PNDIS_RECEIVE_FILTER_FIELD_PARAMETERS pNdisReceiveFilterField;
    UINT32 eArray;
    UINT32 i;

    NT_ASSERT( ppbReceiveFilterParams && pcbReceiveFilterParams&& pcbNeeded );

    if ( pReceiveFilterParams->FieldParametersArrayElementSize < sizeof(NDIS_OBJECT_HEADER) ||
        pReceiveFilterParams->FieldParametersArrayElementSize > sizeof(NDIS_RECEIVE_FILTER_FIELD_PARAMETERS))
    {
        ndisStatus = STATUS_INVALID_PARAMETER;
        WFCActError("Invlid PNDIS_RECEIVE_FILTER_FIELD_PARAMETERS (%x)", ndisStatus);
        goto Exit;
    }

    eArray = pReceiveFilterParams->FieldParametersArrayNumElements;
    TlvCoalescingInfo.AddReceiveCoalescing.Optional.ReceiveCoalescingField_IsPresent = ((eArray > 0) ? TRUE : FALSE);

    TlvCoalescingInfo.AddReceiveCoalescing.ReceivingCoalesingConfig.QueueId = pReceiveFilterParams->QueueId;
    TlvCoalescingInfo.AddReceiveCoalescing.ReceivingCoalesingConfig.FilterId = pReceiveFilterParams->FilterId;
    TlvCoalescingInfo.AddReceiveCoalescing.ReceivingCoalesingConfig.MaxCoalescingDelay = pReceiveFilterParams->MaxCoalescingDelay;

    TlvCoalescingInfo.AddReceiveCoalescing.ReceiveCoalescingField.AllocateElements( eArray, 0 );

    pbFilterFieldContentSrc = (BYTE*) pReceiveFilterParams + 
                pReceiveFilterParams->FieldParametersArrayOffset;

    for ( i=0; i < eArray; i++ )
    {
        pNdisReceiveFilterField = (PNDIS_RECEIVE_FILTER_FIELD_PARAMETERS)pbFilterFieldContentSrc;
        WDI_RECEIVE_COALESCING_FIELD * pTlvReceiveFilterField = &(TlvCoalescingInfo.AddReceiveCoalescing.ReceiveCoalescingField.pElements[i]);

        pTlvReceiveFilterField->Flags = pNdisReceiveFilterField->Flags;
        pTlvReceiveFilterField->FrameHeader = pNdisReceiveFilterField->FrameHeader;
        pTlvReceiveFilterField->ReceiveFilterTest = pNdisReceiveFilterField->ReceiveFilterTest;
        RtlCopyMemory (&pTlvReceiveFilterField->HeaderField,
                        &pNdisReceiveFilterField->HeaderField,
                        sizeof(UINT32));
        RtlCopyMemory( pTlvReceiveFilterField->FieldValue.Byte,
                        &pNdisReceiveFilterField->FieldValue,
                        16);
        RtlCopyMemory( pTlvReceiveFilterField->ResultValue.Byte,
                        &pNdisReceiveFilterField->ResultValue,
                        16);

        pbFilterFieldContentSrc += pReceiveFilterParams->FieldParametersArrayElementSize;
    }

    ndisStatus = GenerateWdiSetReceiveCoalescing( &TlvCoalescingInfo, ReserveHeaderSize, this->m_pAdapter->GetTlvContext(), pcbReceiveFilterParams, ppbReceiveFilterParams );

    Exit:
       if ( ndisStatus >= NDIS_STATUS_SUCCESS )
       {
            *pcbNeeded = cbRead;
       }

       return ndisStatus;
}
//-- CSetReceiveCoalescingJob   implementation =====


//
//++ CSetClearReceiveCoalescingJob   implementation  =====
//
NDIS_STATUS CSetClearReceiveCoalescingJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    PNDIS_RECEIVE_FILTER_CLEAR_PARAMETERS  pClearFilterParams;

    TraceEntry();
    
    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;

    pClearFilterParams= (PNDIS_RECEIVE_FILTER_CLEAR_PARAMETERS)SET_OID_BUFFER(pOidRequest);
    m_FilterId = pClearFilterParams->FilterId;

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID CSetClearReceiveCoalescingJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceEntry();

    TraceExit(NdisStatus);
}

VOID CSetClearReceiveCoalescingJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceActEntry();

    // Complete the OID back to NDIS
    NDIS_STATUS wiFiStatus;
    NDIS_STATUS localStatus;

    if ( NT_SUCCESS( NdisStatus ) ) 
    {
        NdisStatus = CMessageHelper::GetStatusFromCommandResult( 
            &m_DeviceCommand, 
            &wiFiStatus );
        if ( NT_SUCCESS(NdisStatus)) 
        {
            localStatus = RemoveReceiveCoalesceFromPropertyCache();
            UNREFERENCED_PARAMETER( localStatus );
            NT_ASSERT( localStatus == NDIS_STATUS_SUCCESS );
        }
    }

    if (NdisStatus != NDIS_STATUS_SUCCESS)
    {
        if (TEST_FLAG(m_pPort->GetPnpState(), PORT_SURPRISE_REMOVED))
        {
            WFCActInfo("Changing the surprise removal completion status for Clear Receive "
                 "Coalescing filter from 0x%08x to NDIS_STATUS_NOT_ACCEPTED", NdisStatus);
            NdisStatus = NDIS_STATUS_NOT_ACCEPTED;
        }
        else
        {
            // We have to fake the completion status to avoid NDIS from bugchecking
            NT_ASSERT(FALSE);
            WFCActError("Forcing the completion status for Clear Receive Coalescing filter "
                "from 0x%08x to NDIS_STATUS_SUCCESS", NdisStatus);
            NdisStatus = NDIS_STATUS_SUCCESS;
        }
    }

    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit( NdisStatus );
}

void CSetClearReceiveCoalescingJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();
    
    m_pPort = GetAdapter()->GetPortFromNdisPortNumber( m_OidPortNumber );
    NT_ASSERT( m_pPort );

    ndisStatus = StartSetClearReceiveCoalescingCommand();
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        CompleteJob( ndisStatus );
    }

    TraceActExit( ndisStatus );
}

void CSetClearReceiveCoalescingJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);
    TraceActEntry();
    
    CompleteJob(OperationStatus);
    TraceActExit( OperationStatus );
}

void CSetClearReceiveCoalescingJob::OnJobCancelled()
{
    // We are not cancellable

    TraceActEntry();
    TraceActExit( STATUS_SUCCESS );
}


NDIS_STATUS
CSetClearReceiveCoalescingJob::RemoveReceiveCoalesceFromPropertyCache()
{
    NDIS_STATUS status=NDIS_STATUS_SUCCESS;

    CPort *pPort = m_pAdapter->GetPortFromNdisPortNumber( m_NdisPortNumber );
    CPropertyCache * pPortPropertyCache;
    CPropertyCache * pCurrPropertyCache;
    CPropertyCache * pPrevPropertyCache=NULL;
    CPropertyCache *pFirstPropertyCache=NULL;

    PNDIS_RECEIVE_FILTER_PARAMETERS pReceiveCoalesce;
    ULONG ulSize;
    ULONG ulFilterId = m_FilterId;

    pPortPropertyCache = pPort->GetPropertyCache();
    status = pPortPropertyCache->GetPropertyPropertyCacheList(
                        WfcPortPropertyReceiveCoalesceList,
                        &pFirstPropertyCache );
    NT_ASSERT( status == NDIS_STATUS_SUCCESS );
    if ( status != NDIS_STATUS_SUCCESS || pFirstPropertyCache == NULL )
    {
        WFCError("RemoveReceiveCoalesceFromPropertyCache list is empty");
        goto Exit;
    }

    pCurrPropertyCache = pFirstPropertyCache;

    DtPrint( "Remove from propertycache Coalesce %d\n", ulFilterId );
    for ( ;pCurrPropertyCache!= NULL; pCurrPropertyCache = pCurrPropertyCache->Next )
    {
        status = pCurrPropertyCache->GetPropertyBuffer(
                    WfcPortPropertyReceiveCoalesce,
                    &ulSize,
                    (BYTE**)&pReceiveCoalesce);
        if ( status == STATUS_SUCCESS )
        {
            DtPrintOff( "Compare against id=%d @%p\n", 
                    pReceiveCoalesce->FilterId,
                    pCurrPropertyCache);
            if ( ulSize >= sizeof( NDIS_RECEIVE_FILTER_PARAMETERS ) &&
                pReceiveCoalesce->FilterId == ulFilterId )
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
                        WfcPortPropertyReceiveCoalesceList,
                        pFirstPropertyCache );
        NT_ASSERT( status == NDIS_STATUS_SUCCESS );
    }
    else
    {
        WFCError("RemoveReceiveCoalesceFromPropertyCache id=%d does not exist", ulFilterId);
    }

    Exit:
    return status;
}


NDIS_STATUS
CSetClearReceiveCoalescingJob::StartSetClearReceiveCoalescingCommand(
)
/*++

    Private function to send SetClearReceiveCoalescing command

    --*/
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;
    WDI_SET_CLEAR_RECEIVE_COALESCING_PARAMETERS tlvClear;
    const PLACEMENT_NEW_ALLOCATION_CONTEXT pnac = { sizeof( m_pbCommandBuffer ), m_pbCommandBuffer };
    TLV_CONTEXT tlvCtx = *m_pAdapter->GetTlvContext();
    BYTE * pbBuffer = NULL;
    ULONG cbBuffer;

    TraceActEntry();

    //
    // set up command message
    //
    tlvClear.FilterId = m_FilterId;
    tlvCtx.AllocationContext = (ULONG_PTR)(&pnac);
    ndisStatus = GenerateWdiSetClearReceiveCoalescing( &tlvClear, sizeof( WFC_MESSAGE_METADATA ), &tlvCtx, &cbBuffer, &pbBuffer );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError( "Failed to generate Device Command for SetClearReceiveCoalescing 0x%x", ndisStatus );
        goto Exit;
    }
    NT_ASSERT( pbBuffer == m_pbCommandBuffer );
    NT_ASSERT( cbBuffer == m_cbCommand );

    ndisStatus = m_DeviceCommand.Initialize(
        m_pPort-> GetPortId(),
        WDI_SET_CLEAR_RECEIVE_COALESCING,
        cbBuffer,
        pbBuffer );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for SetClearReceiveCoalescing 0x%x", ndisStatus);
        goto Exit;
    }
 
    ndisStatus = QueueDeviceCommand(&m_DeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start SetClearReceiveCoalescing cmd (%x)", ndisStatus);
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

//-- CSetClearReceiveCoalescingJob   implementation  =====


//
//++ CGetReceiveCoalescingMatchCountJob   implementation  =====
//
NDIS_STATUS CGetReceiveCoalescingMatchCountJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();
    
    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;

    // QUERY_OID_BUFFER ->DATA.GET_INFORMATION.InformationBuffer;
    //NT_ASSERT( QUERY_OID_BUFFER_LENGTH( pOidRequest ) >= sizeof(UINT64));
    //m_pOidOutput = (*UINT64) QUERY_OID_BUFFER( m_pOidRequest );

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID CGetReceiveCoalescingMatchCountJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceEntry();

    TraceExit(NdisStatus);
}

VOID CGetReceiveCoalescingMatchCountJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    WDI_GET_RECEIVE_COALESCING_MATCH_COUNT_PARAMETERS MatchCountTlv;

    TraceActEntry();

    if ( NdisStatus != NDIS_STATUS_SUCCESS ) 
    {
        goto final;
    }
    
    //
    // get the reported count TLV
    //
    BYTE* pb;
    ULONG cb;
    PWFC_MESSAGE_METADATA pMsgHdr;
    
    NdisStatus = m_DeviceCommand.get_CommandResult( &cb, &pb );
    if ( NdisStatus != NDIS_STATUS_SUCCESS ) 
    {
        goto final;
    }

    pMsgHdr = (PWFC_MESSAGE_METADATA) pb;
    NdisStatus = pMsgHdr->Message.Status;

    if ( NdisStatus != NDIS_STATUS_SUCCESS ) 
    {
        WFCActError("GetReceiveCoalescingMatchCount WiFiStatus2Ndis 0x%x", NdisStatus);
        goto final;
    }

    NdisStatus = ParseWdiGetReceiveCoalescingMatchCount( cb - sizeof( WFC_MESSAGE_METADATA ), pb + sizeof( WFC_MESSAGE_METADATA ), m_pAdapter->GetTlvContext(), &MatchCountTlv );
    if (NdisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError( "GetReceiveCoalescingMatchCount faield to parse 0x%08X\n", NdisStatus );
        goto final;
    }

    *(UINT64*) (QUERY_OID_BUFFER( m_pOidRequest )) = MatchCountTlv.MatchCount;
    QUERY_OID_BYTES_WRITTEN(m_pOidRequest) = sizeof( UINT64);
    QUERY_OID_BYTES_NEEDED(m_pOidRequest) = sizeof( UINT64);

final:

    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit( NdisStatus );
}

void CGetReceiveCoalescingMatchCountJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    m_pPort = GetAdapter()->GetPortFromNdisPortNumber( m_OidPortNumber );
    NT_ASSERT( m_pPort );

    ndisStatus = StartGetReceiveCoalescingMatchCountCommand();
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        CompleteJob( ndisStatus );
    }

    TraceActExit( ndisStatus );
}

void CGetReceiveCoalescingMatchCountJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);
    TraceActEntry();
    if ( OperationStatus == NDIS_STATUS_SUCCESS )
    {
        //
        // get output message result
        //
        
        (m_pOidRequest);
    }
    
    CompleteJob(OperationStatus);
    TraceActExit( OperationStatus );
}

void CGetReceiveCoalescingMatchCountJob::OnJobCancelled()
{
    // We are not cancellable

    TraceActEntry();
    TraceActExit( STATUS_SUCCESS );
}


NDIS_STATUS
CGetReceiveCoalescingMatchCountJob::StartGetReceiveCoalescingMatchCountCommand(
    )
/*++

    Private function to send GetReceiveCoalescingMatchCount command

--*/
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;

    TraceActEntry();

    //
    // command message has header but no payload
    //

    //
    // initialize the command
    //
    ndisStatus = m_DeviceCommand.Initialize(
        m_pPort-> GetPortId(),
        WDI_GET_RECEIVE_COALESCING_MATCH_COUNT,
        m_cbCommand,
        m_pbCommandBuffer );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for GetReceiveCoalescingMatchCount 0x%x", ndisStatus);
        goto Exit;
    }
 
    ndisStatus = QueueDeviceCommand(&m_DeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start GetReceiveCoalescingMatchCount cmd (%x)", ndisStatus);
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

//-- CGetReceiveCoalescingMatchCountJob   implementation  =====

