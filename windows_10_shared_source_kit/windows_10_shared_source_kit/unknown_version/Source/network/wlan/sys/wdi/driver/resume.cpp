/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Resume.cpp

Abstract:

    Implementation for resume functions

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "Resume.tmh"


NDIS_STATUS CResumePacketFilterJob::InitializeForResume( 
    _In_ CAdapter * pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber )
{
    NDIS_STATUS status;
    CPort * pPort = pAdapter->GetPortFromNdisPortNumber( NdisPortNumber );
    POID_HANDLER_ENTRY pHandlerInfo;
    NDIS_OID oid = OID_GEN_CURRENT_PACKET_FILTER;

    m_pAdapter = pAdapter;
    m_NdisPortNumber = NdisPortNumber;
    //
    // Make a dummy OID so we can reuse SetPacketFilterJob
    //
    PNDIS_OID_REQUEST pOidRequest = (PNDIS_OID_REQUEST)m_pbOidPacketFilter;

    RtlZeroMemory( m_pbOidPacketFilter, c_OidRequestSize );
    pOidRequest->RequestType = NdisRequestSetInformation;
    pOidRequest->PortNumber = NdisPortNumber;
    pOidRequest->DATA.SET_INFORMATION.Oid = oid;
    pOidRequest->DATA.SET_INFORMATION.InformationBuffer = (PVOID)
            (m_pbOidPacketFilter+sizeof(NDIS_OID_REQUEST));
    status = pPort->GetPropertyCache()->GetPropertyULong( 
            WfcPortPropertyNamePacketFilter,
            (PULONG) (m_pbOidPacketFilter+sizeof(NDIS_OID_REQUEST)));
    NT_ASSERT( status == NDIS_STATUS_SUCCESS );
    pOidRequest->DATA.SET_INFORMATION.InformationBufferLength = sizeof(ULONG);
    pHandlerInfo = COidJobBase::s_FindOidHandlerEntry(oid);
    NT_ASSERT( pHandlerInfo );

    status = COidJobBase::InitializeForOid(
                        pHandlerInfo->SetMethodJobType,
                        pAdapter,
                        pHandlerInfo,
                        pOidRequest);
    NT_ASSERT( NT_SUCCESS( status ) );

    return status;
}


NDIS_STATUS CResumeMulticastListJob::InitializeForResume( 
    _In_ CAdapter * pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _In_ WFC_PROPERTY_NAME MulticastPropertyName,
    _In_ ULONG OidInfoBufferSize,
    _In_ BYTE * pbOidInfoBuffer )
{
    NDIS_STATUS status;
    POID_HANDLER_ENTRY pHandlerInfo;
    NDIS_OID oid;

    m_pAdapter = pAdapter;
    m_NdisPortNumber = NdisPortNumber;
    m_MulticastPropertyName = MulticastPropertyName;
    if ( MulticastPropertyName == WfcPortProperty8023MulticastList )
    {
        oid = OID_802_3_MULTICAST_LIST;
    }
    else
    {
        oid = OID_DOT11_MULTICAST_LIST;
    }
    //
    // Make a dummy OID so we can reuse SetPacketFilterJob
    //
    PNDIS_OID_REQUEST pOidRequest = &m_OidRequest;

    RtlZeroMemory( pOidRequest, sizeof(NDIS_OID_REQUEST) );
    pOidRequest->RequestType = NdisRequestSetInformation;
    pOidRequest->PortNumber = NdisPortNumber;
    m_OidInfoBufferSize = OidInfoBufferSize;
    m_pbOidInfoBuffer = pbOidInfoBuffer;
    pOidRequest->DATA.SET_INFORMATION.Oid = oid;
    pOidRequest->DATA.SET_INFORMATION.InformationBufferLength = OidInfoBufferSize;
    pOidRequest->DATA.SET_INFORMATION.InformationBuffer = (PVOID) pbOidInfoBuffer;
    pHandlerInfo = COidJobBase::s_FindOidHandlerEntry(oid);
    NT_ASSERT( pHandlerInfo );

    status = COidJobBase::InitializeForOid(
                        pHandlerInfo->SetMethodJobType,
                        pAdapter,
                        pHandlerInfo,
                        pOidRequest);
    NT_ASSERT( NT_SUCCESS( status ) );

    return status;
}

NDIS_STATUS CResumeFirmwareJob::InitializeExplicit(
    _In_ CAdapter *pAdapter )
{
    m_pAdapter = pAdapter;
    m_NdisPortNumber = NDIS_DEFAULT_PORT_NUMBER;
    m_bInitToResumeFirmware = TRUE; // set this so CMiniportInitializeJob works accordingly
    return CJobBase::Initialize(
            m_pAdapter->GetEventQueue(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetActiveJobsList(),
            m_pAdapter->GetSerializedJobList(),
            WiFiJobResumeFirmware,
            FALSE,
            FALSE,
            WiFiPriorityScopeAdapter,
            WiFiSerializedJobPriority_1,
            (IPropertyCacheDirectory *)pAdapter,
            NULL,
            NDIS_DEFAULT_PORT_NUMBER);
};


NDIS_STATUS CResumeFirmwareJob::ResumeFirmwareContext(
    _Out_ BOOLEAN* pbCompleteJob )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CPort * pPort = m_pAdapter->GetPortFromNdisPortNumber(NDIS_DEFAULT_PORT_NUMBER);
    CPort * pPortToCreate;

    NT_ASSERT( pPort );
    *pbCompleteJob = FALSE;

    NT_ASSERT( m_JobState == MiniportInitializeJobSettingMiniportAttributes );

    //
    // List of jobs to resume firmware context
    // Packet Filter
    // Multicast filter : WfcPortProperty8023MulticastList, WfcPortPropertyDot11MulticastList,
    // (MediaStreamingMoce- may be unnecessary)
    // AutoPSM
    // protocol offload list
    // WolPattern list
    // D0Coalescing
    // Non-default ports:
    //      CreatePort
    //      AutoPsm
    //      Advertisement
    //

    if ( m_JobStateSub == JobStateSubNotStarted )
    {
        DtPrint( "Start ResumeFirmwareContext\n" );
        m_JobStateSub = JobStateResumeFwAwaitResumePacketFilterComplete;
        status = m_ResumePacketFilterJob.InitializeForResume( 
                    m_pAdapter,
                    NDIS_DEFAULT_PORT_NUMBER);
        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCError( "Failed to init ResumePacketFilter job st=0x%x", status);
            DtErrMsg( "Reseume Init PacketFilterJob error=0x%x\n", status );
            goto Exit;
        }

        status = StartChildJob( &m_ResumePacketFilterJob );
        DtPrint( "ResumeContext start packetFilter status 0x%x\n", status );
        if ( status == NDIS_STATUS_SUCCESS )
        {
            goto Exit;
        }
    }

    else if ( m_JobStateSub == JobStateResumeFwAwaitResumePacketFilterComplete )
    {
        ULONG ulOidInfoBufferSize;
        BYTE *pbOidInfoBuffer;

        m_JobStateSub = JobStateResumeFwAwaitResumeMulticastList8023Complete;

        //
        // Get the list from property cache. If it does not exist, skip
        //
        status = pPort->GetPropertyCache()->GetPropertyBuffer(
                    WfcPortProperty8023MulticastList,
                    &ulOidInfoBufferSize,
                    &pbOidInfoBuffer );

        if ( status != NDIS_STATUS_SUCCESS ||
                ulOidInfoBufferSize == 0 )
        {
            // was not set, fall thru to next step
            DtPrint( "Reseume 8023 mcast filter none or 0x%x\n", status );
        }
        else 
        {
            status = m_ResumeMulticastListJob.InitializeForResume( 
                        m_pAdapter, 
                        NDIS_DEFAULT_PORT_NUMBER,
                        WfcPortProperty8023MulticastList,
                        ulOidInfoBufferSize,
                        pbOidInfoBuffer);
            if ( status != NDIS_STATUS_SUCCESS )
            {
                WFCError( "Failed to init ResumeMulticastList job st=0x%x", status);
                DtErrMsg( "Reseume init 8023 mcast filter failed 0x%x\n", status );
                // fall thru to next step
            }
            else
            {
                status = StartChildJob( &m_ResumeMulticastListJob );
                if ( status == NDIS_STATUS_SUCCESS )
                {
                    DtPrint( "Reseume start 8023 mcast filter\n" );
                    goto Exit;
                }
                DtErrMsg( "Reseume start 8023 mcast filter failed 0x%x\n", status );
            }
        }
    }

    if ( m_JobStateSub == JobStateResumeFwAwaitResumeMulticastList8023Complete )
    {
        ULONG ulOidInfoBuffer;
        BYTE * pbOidInfoBuffer;

        m_JobStateSub = JobStateResumeFwAwaitResumeMulticastListDot11Complete;

        //
        // Get the list from property cache. If it does not exist, skip
        //
        status = pPort->GetPropertyCache()->GetPropertyBuffer(
                    WfcPortPropertyDot11MulticastList,
                    &ulOidInfoBuffer,
                    &pbOidInfoBuffer );

        if ( status != NDIS_STATUS_SUCCESS || ulOidInfoBuffer == 0 )
        {
            // was not set, fall thru to next step
            DtPrint( "Reseume get dot11 mcast filter none or 0x%x\n", status );
        }
        else 
        {
            status = m_ResumeMulticastListJob.InitializeForResume( 
                        m_pAdapter,
                        NDIS_DEFAULT_PORT_NUMBER,
                        WfcPortPropertyDot11MulticastList,
                        ulOidInfoBuffer,
                        pbOidInfoBuffer);
            if ( status != NDIS_STATUS_SUCCESS )
            {
                WFCError( "Failed to init ResumeMulticastList job st=0x%x", status);
                DtErrMsg( "Reseume init dot11 mcast filter failed 0x%x\n", status );
                // fall thru to next step
            }
            else
            {
                status = StartChildJob( &m_ResumeMulticastListJob );
                DtPrint( "ResumeCOntext start Mcast dot11 status=0x%x\n", status );
                if ( status == NDIS_STATUS_SUCCESS )
                {
                    DtErrMsg( "Reseume start dot11 mcast filter failed 0x%x\n", status );
                    goto Exit;
                }
            }
        }
    }

    if ( m_JobStateSub == JobStateResumeFwAwaitResumeMulticastListDot11Complete )
    {
        BOOLEAN bAutoPsm;

        m_JobStateSub = JobStateResumeFwAwaitResumeAutoPsmComplete;

        //
        // Get the list from property cache. If it does not exist, skip
        //
        status = pPort->GetPropertyCache()->GetPropertyBoolean(
                    WfcPortPropertyAutoPsm,
                    &bAutoPsm );
 
        if ( status != NDIS_STATUS_SUCCESS || bAutoPsm == FALSE )
        {
            // was not set, fall thru to next step
            DtPrint( "Reseume Get auto PSM not set or 0x%x\n", status );
        }
        else 
        {
            status = m_ResumeAutoPsmJob.InitializeForResume( 
                        m_pAdapter,
                        NDIS_DEFAULT_PORT_NUMBER );
            if ( status != NDIS_STATUS_SUCCESS )
            {
                WFCError( "Failed to init ResumeAutoPsm job st=0x%x", status);
                // fall thru to next step
                DtErrMsg( "Resume Init auto PSM failed 0x%x\n", status );
                // fall thru to next step
            }
            else
            {
                status = StartChildJob( &m_ResumeAutoPsmJob );
                DtPrint( "ResumeContext start autoPsm status=0x%x\n", status );
                if ( status == NDIS_STATUS_SUCCESS )
                {
                    goto Exit;
                }
                else
                {
                    DtErrMsg( "Resume start auto PSM failed 0x%x\n", status );
                    // fall thru to next step
                }
            }
        }
    }

    if ( m_JobStateSub == JobStateResumeFwAwaitResumeAutoPsmComplete )
    {
        m_JobStateSub = JobStateResumeFwAwaitResumeOffloadsComplete;
        status = pPort->GetPropertyCache()->GetPropertyPropertyCacheList(
            WfcPortPropertyProtocolOffloadList,
            &m_pCurrPropertyCache );
        if ( status != NDIS_STATUS_SUCCESS || m_pCurrPropertyCache == NULL )
        {
            // not set, fall thru to next step
            DtErrMsg( "Reseume Get offload none or failed 0x%x\n", status );
            m_pCurrPropertyCache = NULL;
        }
    }

    if ( m_JobStateSub == JobStateResumeFwAwaitResumeOffloadsComplete )
    {
        if ( m_pCurrPropertyCache )
        {
            ULONG ulOidInfoBuffer;
            BYTE * pbOidInfoBuffer;

            status = m_pCurrPropertyCache->GetPropertyBuffer(
                    WfcPortPropertyProtocolOffload,
                    &ulOidInfoBuffer,
                    &pbOidInfoBuffer );
            NT_ASSERT( status == NDIS_STATUS_SUCCESS && ulOidInfoBuffer );

            status = m_ResumeOffloadJob.InitializeForResume( 
                        m_pAdapter,
                        NDIS_DEFAULT_PORT_NUMBER,
                        ulOidInfoBuffer,
                        pbOidInfoBuffer);
            if ( status != NDIS_STATUS_SUCCESS )
            {
                WFCError( "Failed to init ResumeMulticastList job st=0x%x", status);
                DtErrMsg( "Reseume Init offload failed 0x%x\n", status );
                // fall thru to next step
            }
            else
            {
                status = StartChildJob( &m_ResumeOffloadJob );
                DtPrint( "ResumeContext start offload status=0x%x\n", status);
                if ( status == NDIS_STATUS_SUCCESS )
                {
                    // come back again and again until this reach the end (NULL)
                    m_pCurrPropertyCache = m_pCurrPropertyCache->Next;
                    goto Exit;
                }
                DtErrMsg( "Reseume start offload failed 0x%x\n", status );
            }
        }
        else
        {
            // no more; prepare for next step
            m_JobStateSub = JobStateResumeFwAwaitResumeWolPatternsComplete;
            status = pPort->GetPropertyCache()->GetPropertyPropertyCacheList(
                                WfcPortPropertyWolPatternList,
                                &m_pCurrPropertyCache);
            if ( status != NDIS_STATUS_SUCCESS || m_pCurrPropertyCache == NULL )
            {
                // not set, fall thru to next step
                DtErrMsg( "Reseume Get wol Pattern none or failed 0x%x\n", status );
                m_pCurrPropertyCache = NULL;
            }
        }
    }

    if ( m_JobStateSub == JobStateResumeFwAwaitResumeWolPatternsComplete )
    {
        if ( m_pCurrPropertyCache )
        {
            ULONG ulSize;
            BYTE * pbOidInfoBuffer;

            status = m_pCurrPropertyCache->GetPropertyBuffer(
                    WfcPortPropertyWolPattern,
                    &ulSize,
                    &pbOidInfoBuffer );
            NT_ASSERT( status == NDIS_STATUS_SUCCESS && ulSize );

            status = m_ResumeWolPatternJob.InitializeForResume( 
                        m_pAdapter,
                        NDIS_DEFAULT_PORT_NUMBER,
                        ulSize,
                        pbOidInfoBuffer);
            if ( status != NDIS_STATUS_SUCCESS )
            {
                DtErrMsg( "Reseume init wol Pattern failed 0x%x\n", status );
                WFCError( "Failed to init ResumeWolPattern job st=0x%x", status);
                // fall thru to next step
            }
            else
            {
                status = StartChildJob( &m_ResumeWolPatternJob );
                DtPrint( "ResumeContext start WolPattern status=0x%x\n", status );
                if ( status == NDIS_STATUS_SUCCESS )
                {
                    // come back again and again until this reach the end (NULL)
                    m_pCurrPropertyCache = m_pCurrPropertyCache->Next;
                    goto Exit;
                }
                DtErrMsg( "Reseume start wol Pattern failed 0x%x\n", status );
            }
        }
        else
        {
            // no more; prepare for next step
            m_JobStateSub = JobStateResumeFwAwaitResumeCoalescingComplete;
            status = pPort->GetPropertyCache()->GetPropertyPropertyCacheList(
                                WfcPortPropertyReceiveCoalesceList,
                                &m_pCurrPropertyCache);
            if ( status != NDIS_STATUS_SUCCESS || m_pCurrPropertyCache == NULL )
            {
                // not set, fall thru to next step
                DtPrint( "Reseume get coalesce none or failed 0x%x\n", status );
                m_pCurrPropertyCache = NULL;
            }
        }
    }

    if ( m_JobStateSub == JobStateResumeFwAwaitResumeCoalescingComplete )
    {
        if ( m_pCurrPropertyCache )
        {
            ULONG ulSize;
            BYTE * pbOidInfoBuffer;

            status = m_pCurrPropertyCache->GetPropertyBuffer(
                    WfcPortPropertyReceiveCoalesce,
                    &ulSize,
                    &pbOidInfoBuffer );
            NT_ASSERT( status == NDIS_STATUS_SUCCESS && ulSize );

            status = m_ResumeCoalesceJob.InitializeForResume( 
                        m_pAdapter,
                        NDIS_DEFAULT_PORT_NUMBER,
                        ulSize,
                        pbOidInfoBuffer);
            if ( status != NDIS_STATUS_SUCCESS )
            {
                WFCError( "Failed to init ResumeWolPattern job st=0x%x", status);
                DtErrMsg( "Reseume init coalesce failed 0x%x\n", status );
                // fall thru to next step
            }
            else
            {
                status = StartChildJob( &m_ResumeCoalesceJob );
                DtPrint( "ResumeContext start Coalesce status=0x%x\n", status );
                if ( status == NDIS_STATUS_SUCCESS )
                {
                    // come back again and again until this reach the end (NULL)
                    m_pCurrPropertyCache = m_pCurrPropertyCache->Next;
                    goto Exit;
                }
                DtErrMsg( "Reseume start coalesce failed 0x%x\n", status );
            }
        }
        else
        {
            //
            // no more coalsece work; prepare for next step
            //
            m_JobStateSub = JobStateResumeFwAwaitResumePrivacyExemptionComplete;
            m_PortLoop = 0;
        }
    }

    if ( m_JobStateSub == JobStateResumeFwAwaitResumePrivacyExemptionComplete )
    {
        //
        // resume privacy exemption list if set
        //
        ULONG ulSize;
        UINT16 listCount;
        PDOT11_PRIVACY_EXEMPTION pPrivacyExemptions;

        m_JobStateSub = JobStateResumeFwAwaitResumeDefaultKeyComplete;
        m_ResumeDefaultKeyJob.m_NextKeyIndex = 0;

        status = pPort->GetPropertyCache()->GetPropertyList(
                WfcPortPropertyNamePrivacyExemptionList,
                &ulSize,
                &listCount,
                (PBYTE*)&pPrivacyExemptions );

        if ( status == NDIS_STATUS_SUCCESS && listCount != 0 )
        {
            DtPrint( "Reseume PrivacyExemption listCount=%d\n", listCount );
            WFCInfo( "Reseume PrivacyExemption listCount=%d", listCount );

            if ( ulSize >= sizeof( DOT11_PRIVACY_EXEMPTION ) * listCount )
            {
                for (int i=0; i < listCount ;i++ )
                {
                    WFCInfo( "Cached [%d] EtherType=0x%4x ActionType=0x%4x PacketType=0x%4x ",
                        i, 
                        pPrivacyExemptions[i].usEtherType,
                        pPrivacyExemptions[i].usExemptionActionType,
                        pPrivacyExemptions[i].usExemptionPacketType );
                }
            }
            else
            {
                WFCError( "PrivacyExempt in property cache bad size %d < elements %d*%d",
                        ulSize,
                        sizeof( DOT11_PRIVACY_EXEMPTION ),
                        listCount);
                ASSERT(0);
            }

            status = m_ResumePrivacyExemptionJob.InitializeForResume( m_pAdapter,
                        NDIS_DEFAULT_PORT_NUMBER,
                        ulSize,
                        listCount,
                        pPrivacyExemptions);
            if ( status != NDIS_STATUS_SUCCESS )
            {
                WFCError( "Failed to init PrivacyExemption job st=0x%x", status);
                DtErrMsg( "Reseume init PrivacyExemption failed 0x%x\n", status );
                // fall thru to next step
            }
            else
            {
                status = StartChildJob( &m_ResumePrivacyExemptionJob );
                DtPrint( "ResumeContext start PrivacyExempltion status=0x%x\n", status );
                WFCInfo( "ResumeContext start PrivacyExempltion status=0x%x", status );
                if ( status == NDIS_STATUS_SUCCESS )
                {
                    // come back to the next step
                    goto Exit;
                }
                DtErrMsg( "Reseume start PrivacyExemption job failed 0x%x\n", status );
                WFCError( "Reseume start PrivacyExemption job failed 0x%x", status );
                // fall thru to next step
            }
        }
        else
        {
            DtPrint( "Reseume start PrivacyExemption job list empty\n" );
            WFCError( "Reseume start PrivacyExemption job list empty" );
            //  fall thru to the next step
        }
    }

    if( m_JobStateSub == JobStateResumeFwAwaitResumeDefaultKeyComplete )
    {
        //
        // resume all default keys until no more
        //
        ULONG ulKeyIndex = m_ResumeDefaultKeyJob.GetNextKeyIndex( m_pAdapter );

        if ( ulKeyIndex < MAX_DEFAULT_KEYS )
        {
            status = m_ResumeDefaultKeyJob.InitializeForResume( m_pAdapter,
                            NDIS_DEFAULT_PORT_NUMBER,
                            ulKeyIndex );
            if ( status == NDIS_STATUS_SUCCESS )
            {
                status = StartChildJob( &m_ResumeDefaultKeyJob );
                if ( status == NDIS_STATUS_SUCCESS )
                {
                    // come back again until no more
                    goto Exit;
                }
            }
        }
        //
        // any failure or no more defaulr key, fall thru to go to the next steep
        //
        m_JobStateSub = JobStateResumeFwAwaitResumeDefaultKeyIdComplete;
    }

    if ( m_JobStateSub == JobStateResumeFwAwaitResumeDefaultKeyIdComplete )
    {
        m_JobStateSub = JobStateResumeFwAwaitResumeP2pPortComplete;
        status = m_ResumeDefaultKeyIdJob.InitializeForResume( m_pAdapter, NDIS_DEFAULT_PORT_NUMBER );
        if ( status == NDIS_STATUS_SUCCESS )
        {
            status = StartChildJob( &m_ResumeDefaultKeyIdJob );
            if ( status == NDIS_STATUS_SUCCESS )
            {
                // wait it to complete
                goto Exit;
            }
        }
        //
        // any failure fall thru to the next atep; if there is no default id (it's ok) and no p2p ports
        // we would propagate the error which is benigh but set it to success to avoid futire complications
        //
        status = NDIS_STATUS_SUCCESS;
    }

    if ( m_JobStateSub == JobStateResumeFwAwaitResumeP2pPortComplete )
    {
        pPortToCreate = GetNextP2pPort();
        if ( pPortToCreate )
        {
            status = m_ResumeP2pPortJob.InitializeForResume(
                        m_pAdapter, 
                        pPortToCreate->GetNdisPortNumber());
            if ( status != NDIS_STATUS_SUCCESS )
            {
                DtErrMsg( "Reseume init P2p port failed 0x%x\n", status );
                WFCError( "m_ResumeP2pPortJob InitializeForResume 0x%x", status);
                goto Exit;
            }

            status = StartChildJob( &m_ResumeP2pPortJob );
            DtPrint( "ResumeContext start P2pPort %d status=0x%x\n", 
                pPortToCreate->GetNdisPortNumber(),
                status );
            goto Exit;
        }
        else
        {
            // no more port
            m_JobStateSub = JobStateResumeFwAwaitComplete;
        }
    }

    Exit:
    {
        if ( status != NDIS_STATUS_SUCCESS || 
            m_JobStateSub == JobStateResumeFwAwaitComplete )
        {
            WFCError( "ResumeContext complete status=0x%x\n", status );
            *pbCompleteJob = TRUE;
        }
    }

    //
    // dont return failure. move along. as it is less of the evils
    //
    status = NDIS_STATUS_SUCCESS;
    return status;
}


NDIS_STATUS CResumeFirmwareJob::StartOpenTask()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    DeviceCommand* pDeviceCommand = NULL;

    TraceActEntry();
    
    // Send the open task to the device
    ndisStatus = m_Task.Initialize(
            m_pAdapter->GetNotificationManager(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetEventQueue()
            );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Open Task (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = m_Task.get_TaskDeviceCommand(&pDeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to get Device Command for Open Task (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = pDeviceCommand->Initialize(
        WDI_PORT_ID_ADAPTER,
        WDI_TASK_OPEN,
        sizeof(WFC_MESSAGE_METADATA),
        (BYTE *)&m_SimpleCommandBuffer
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for Open Task (0x%x)", ndisStatus);
        goto Exit;
    }

    // Save the start state so that when complete happens, we know where we are
    m_JobState = MiniportInitializeJobWaitingForOpenTaskComplete;

    ndisStatus = StartTask(&m_Task);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Open Task (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:

    TraceActExit(ndisStatus);    

    return ndisStatus;
}



CPort * CResumeFirmwareJob::GetNextP2pPort()
{
    CPort * pPort = NULL;
    for ( ;m_PortLoop < m_pAdapter->c_MaxPorts; m_PortLoop++ )
    {
        if ( m_pAdapter->m_pPortList[m_PortLoop] == NULL )
        {
            continue;
        }
        if ( m_pAdapter->m_pPortList[m_PortLoop]->GetNdisPortNumber() 
            == NDIS_DEFAULT_PORT_NUMBER )
        {
            continue;
        }
        else
        {
            pPort = m_pAdapter->m_pPortList[m_PortLoop];
            m_PortLoop++;
            break;
        }
    }
    return pPort;
}


void CResumeFirmwareJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
/*++

    Override the same in CMiniportInitialize. We have our own steps

--*/
{
    BOOLEAN completeJob = FALSE;
    BOOLEAN bSoftwareRadioOff = FALSE;
    MINIPORT_INITIALIZE_JOB_STATE lastAttemptedState = m_JobState;

    TraceActEntry();

    UNREFERENCED_PARAMETER(SenderObject);

    switch(m_JobState)
    {
        case MiniportInitializeJobWaitingForOpenTaskComplete:
            {
                OperationStatus = CompleteOpenTask(OperationStatus);
                DtPrint( "ResumeFirmware fwOpen completed 0x%x\n", OperationStatus );
                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    //
                    // We already have the firmware cap in the property cache
                    // skip the Get/Set firmware capabilities. Set FW config
                    //
                    lastAttemptedState = MiniportInitializeJobWaitingForFirmwareConfiguration;
                    OperationStatus = StartSetFirmwareConfiguration();
                    DtPrint( "ResumeFirmware start fw config status=0x%x\n", OperationStatus );
                }
            }
            break;

        case MiniportInitializeJobWaitingForFirmwareConfiguration:
            {
                OperationStatus = CompleteSetFirmwareConfiguration(OperationStatus);
                DtPrint( "ResumeFirmware fwConfig complete 0x%x\n", OperationStatus );

                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    lastAttemptedState = MiniportInitializeJobWaitingForCreatePortComplete;
                    OperationStatus = StartCreatePortJob( TRUE );
                    DtPrint( "ResumeFirmware start create 0 port 0x%x\n", OperationStatus );
                }
            }
            break;

        case MiniportInitializeJobWaitingForCreatePortComplete:
            {
                OperationStatus = CompleteCreatePortJob(OperationStatus);
                DtPrint( "ResumeFirmware Complete create 0 port 0x%x\n", OperationStatus );
                if ( OperationStatus != NDIS_STATUS_SUCCESS )
                {
                    //completeJob = TRUE; implied by the failed status
                    DtErrMsg( "Reseume CreatePort error=0x%x\n", OperationStatus );
                    break;
                }
                lastAttemptedState = MiniportInitializeJobWaitingForSetRadioStateComplete;

                //
                // get sw radio off setting. If true, we  must set radio off
                //
                OperationStatus = m_pAdapter->GetAdapterPropertyCache()->
                    GetPropertyBoolean( WfcAdapterSoftwareRadioOff, &bSoftwareRadioOff);

                if ( OperationStatus != NDIS_STATUS_SUCCESS )
                {
                    WFCError("Failed to GetSoftwareRadioOff property in the cache");
                    //completeJob = TRUE; implied by the failed status
                    DtErrMsg( "Reseume GetSoftwareRadioOff error=0x%x\n", OperationStatus );
                    break;
                }

                DtPrint( "ReseumFirmware SwRadio is %d\n", !bSoftwareRadioOff );
                if ( bSoftwareRadioOff )
                {
                    OperationStatus = StartSetRadioStateTask( TRUE );
                    if ( OperationStatus != NDIS_STATUS_SUCCESS )
                    {
                        WFCError("Failed to StartSetRadioOffTask");
                        DtErrMsg( "Reseume StartSetRadioOffTask error=0x%x\n", OperationStatus );
                        //completeJob = TRUE; implied by the failed status
                        break;
                    }
                    else 
                    {
                        m_JobState = MiniportInitializeJobWaitingForSetRadioStateComplete;
                        break;
                    }
                }
                else
                {
                    //
                    // persist radio power is On, no needs to change FW
                    // fall thru to the next step
                    //
                    m_JobState = MiniportInitializeJobWaitingForSetRadioStateComplete;
                }
            }

        case MiniportInitializeJobWaitingForSetRadioStateComplete:
            if (OperationStatus != NDIS_STATUS_SUCCESS)
            {
                DtErrMsg( "SetRadioOff complete status 0x%x\n", OperationStatus );
                WFCActError("Ignoring failure of SetRadio Job (0x%x)", OperationStatus);
                OperationStatus = NDIS_STATUS_SUCCESS;
            }

            m_JobState = MiniportInitializeJobSettingMiniportAttributes; // signal end of ResumeFw
            m_JobStateSub = JobStateSubNotStarted;
            // fall thru to the next step

        case MiniportInitializeJobSettingMiniportAttributes:
            NT_ASSERT( m_JobState == MiniportInitializeJobSettingMiniportAttributes );
            if ( m_JobStateSub != JobStateResumeFwAwaitComplete )
            {
                OperationStatus = ResumeFirmwareContext( &completeJob );
            }
            else
            {
                completeJob = TRUE;
            }
    }

    if ((OperationStatus != NDIS_STATUS_SUCCESS) ||
        (completeJob == TRUE))
    {
        DtPrint( "Complete ResumeFirmwareJob status=0x%x\n", OperationStatus );
        // dont use FinishJob() it will register miniport attributes whch is extraneous
        CompleteJob( OperationStatus );
    }
    
    TraceActExit(OperationStatus);
}


/*++

    CResumeAutoPsmJob::InitializeForResume
    
--*/
NDIS_STATUS CResumeAutoPsmJob::InitializeForResume( 
    _In_ CAdapter * pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    PNDIS_OID_REQUEST pOidRequest = (PNDIS_OID_REQUEST)m_pbOidAudoPsm;
    PDOT11_POWER_MGMT_AUTO_MODE_ENABLED_INFO pAutoPsmInfo;
    NDIS_OID oid = OID_DOT11_POWER_MGMT_MODE_AUTO_ENABLED;
    POID_HANDLER_ENTRY pHandlerInfo;

    m_pAdapter = pAdapter;
    m_NdisPortNumber = NdisPortNumber;

    //
    // make a dummy OID
    //
    RtlZeroMemory( m_pbOidAudoPsm, c_OidRequestSize );
    pOidRequest->PortNumber = NdisPortNumber;
    pOidRequest->DATA.SET_INFORMATION.Oid = oid;
    pOidRequest->DATA.SET_INFORMATION.InformationBuffer = 
            (PVOID)(m_pbOidAudoPsm+sizeof(NDIS_OID_REQUEST));
    pOidRequest->DATA.SET_INFORMATION.InformationBufferLength = 
            sizeof( DOT11_POWER_MGMT_AUTO_MODE_ENABLED_INFO );

    pAutoPsmInfo = (PDOT11_POWER_MGMT_AUTO_MODE_ENABLED_INFO)
            pOidRequest->DATA.SET_INFORMATION.InformationBuffer;
   pAutoPsmInfo->bEnabled = TRUE;           // always TRUE. We dont call if it is FALSE

    pHandlerInfo = COidJobBase::s_FindOidHandlerEntry(oid);
    NT_ASSERT( pHandlerInfo );

    status = COidJobBase::InitializeForOid(
                        pHandlerInfo->SetMethodJobType,
                        pAdapter,
                        pHandlerInfo,
                        pOidRequest);

    return status;
}

/*++

    CResumeWolPatternJob::InitializeForResume

--*/

NDIS_STATUS CResumeWolPatternJob::InitializeForResume( 
    _In_ CAdapter * pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _In_ ULONG ulOidInfoBuffer,
    _In_ BYTE *pbOidInfoBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_OID oid = OID_PM_ADD_WOL_PATTERN;
    POID_HANDLER_ENTRY pHandlerInfo;

    m_pAdapter = pAdapter;
    m_NdisPortNumber = NdisPortNumber;

    //
    // Make a dummy OID so we can reuse SetPacketFilterJob
    //
    RtlZeroMemory( &m_OidRequest, sizeof( NDIS_OID_REQUEST) );
    m_OidRequest.RequestType = NdisRequestSetInformation;
    m_OidRequest.PortNumber = NdisPortNumber;
    m_OidRequest.DATA.SET_INFORMATION.Oid = oid;
    m_OidRequest.DATA.SET_INFORMATION.InformationBufferLength = ulOidInfoBuffer;
    m_OidRequest.DATA.SET_INFORMATION.InformationBuffer = (PVOID) pbOidInfoBuffer;
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
// CResumeP2pPortJob
//

NDIS_STATUS
CResumeP2pPortJob::InitializeForResume(
    _In_ CAdapter * pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber )
{
    m_pAdapter = pAdapter;
    m_NdisPortNumber = NdisPortNumber;
    return CJobBase::Initialize(
            m_pAdapter->GetEventQueue(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetActiveJobsList(),
            m_pAdapter->GetSerializedJobList(),
            WiFiJobResumeP2pPort,
            FALSE,
            FALSE,
            WiFiPriorityScopeAdapter,
            WiFiSerializedJobPriority_1,
            (IPropertyCacheDirectory *)pAdapter,
            NULL,
            NdisPortNumber);
}

void
CResumeP2pPortJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    m_pPort = m_pAdapter->GetPortFromNdisPortNumber( m_NdisPortNumber );

    if ( m_pPort == NULL )
    {
        WFCError( "CResumeP2pPortJob::InitializeForResume status=0x%x", status );
        goto Exit;
    }

    m_ResumeP2pPortStep = ResumeP2pPortStepInitialized;
    m_AdvertisementSettingLoop = 0;
    OnJobStepCompleted( status, this);

Exit:
    if ( status != NDIS_STATUS_SUCCESS )
    {
        CompleteJob( status );
    }

    return;
}

void
CResumeP2pPortJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    
    OperationStatus, SenderObject; //referenced
    BOOLEAN bAutoPsm = FALSE;

    switch ( m_ResumeP2pPortStep )
    {
        case ResumeP2pPortStepInitialized:
            status = m_CreateMacOidJob.InitializeForResume( m_pPort );
            if ( status != NDIS_STATUS_SUCCESS )
            {
                DtErrMsg( "Resume P2pPort init CreateMac failed 0x%x\n", status );
                WFCError( "m_CreateMacOidJob InitializeForResume 0x%x", status);
                goto Exit;
            }

            m_ResumeP2pPortStep = ResumeP2pPortStepAwaitCreateMacCompletion;
            status = StartChildJob( &m_CreateMacOidJob );
            DtPrint( "ResumeP2pPort start create status=0x%x\n", status );
            if ( status != NDIS_STATUS_SUCCESS )
           {
               WFCError( "m_CreateMacOidJob StartChildJob 0x%x", status);
               goto Exit;
           }
            break;

        case ResumeP2pPortStepAwaitCreateMacCompletion:
            // if auto psm was set, set it.
            m_ResumeP2pPortStep = ResumeP2pPortStepAwaitAutoPsmCompletion;
            status = m_pPort->GetPropertyCache()->GetPropertyBoolean(
                        WfcPortPropertyAutoPsm,
                        &bAutoPsm );
            
            if ( status != NDIS_STATUS_SUCCESS || bAutoPsm == FALSE )
            {
                // was not set, fall thru to next step
                DtPrint( "Reseume Get auto PSM not set or 0x%x\n", status );
            }
            else 
            {
                status = m_ResumeAutoPsm.InitializeForResume( 
                            m_pAdapter,
                            m_pPort->GetNdisPortNumber());
                if ( status != NDIS_STATUS_SUCCESS )
                {
                     DtErrMsg("Resume P2p audoPsm init failed 0x%x\n", status );
                     WFCError( "m_ResumeAutoPsm InitializeForResume 0x%x", status);
                     goto Exit;
                }

                status = StartChildJob( &m_ResumeAutoPsm );
                DtPrint( "ResumeP2p start audoPsm status=0x%x\n", status);
                if ( status != NDIS_STATUS_SUCCESS )
                {
                    WFCError( "m_CreateMacOidJob StartChildJob 0x%x", status);
                    goto Exit;
                }
                break;
            }

        case ResumeP2pPortStepAwaitAutoPsmCompletion:
            {
                ULONG  ulInfoBuffer;
                BYTE * pbInfoBuffer;
                NDIS_OID oid;

                status = GetNextAdvertisement( 
                                &oid,
                                &ulInfoBuffer, 
                                &pbInfoBuffer );
                if ( status == NDIS_STATUS_SUCCESS )
                {
                    status = m_ResumeAdvSettingJob.InitializeForResume( 
                                m_pAdapter,
                                m_NdisPortNumber,
                                oid,
                                ulInfoBuffer,
                                pbInfoBuffer);
                     if ( status != NDIS_STATUS_SUCCESS )
                     {
                        DtErrMsg( "Resume P2p adv failed 0x%x\n", status );
                         WFCError( "m_ResumeAutoPsm InitializeForResume 0x%x", status);
                         goto Exit;
                     }

                    // in this step until adv items are up.
                     m_ResumeP2pPortStep = ResumeP2pPortStepAwaitAutoPsmCompletion;
                     status = StartChildJob( &m_ResumeAdvSettingJob );
                     DtPrint( "Resume P2p start adv status=0x%x\n", status);
                     if ( status != NDIS_STATUS_SUCCESS )
                    {
                        WFCError( "m_CreateMacOidJob StartChildJob 0x%x", status);
                        goto Exit;
                    }
                }
                else
                {
                    if ( status == STATUS_NO_MORE_ENTRIES )
                    {
                        status = NDIS_STATUS_SUCCESS;
                    }
                    m_ResumeP2pPortStep = ResumeP2pPortStepAwaitSetAdvCompletion;
                }
            }
            break;

        case ResumeP2pPortStepAwaitSetAdvCompletion:
            break;
    }

Exit:
    if ( status != NDIS_STATUS_SUCCESS || 
            m_ResumeP2pPortStep == ResumeP2pPortStepAwaitSetAdvCompletion )
    {
        DtPrint( "Resume P2p complete status=0x%x\n", status);
        CompleteJob( status );
    }
}


NDIS_STATUS 
CResumeP2pPortJob::GetNextAdvertisement(
    _Out_ NDIS_OID * pOid,
    _Out_ ULONG* pulInfoBuffer,
    _Out_ BYTE ** ppbInfoBuffer )
{
    NDIS_STATUS status = STATUS_NO_MORE_ENTRIES;
    WFC_PORT_PROPERTY_NAME advertisementName;

    *pOid = (NDIS_OID)0;
    
    for ( ;m_AdvertisementSettingLoop < m_cAdvertisementNames; )
    {
        advertisementName = m_AdvertisementNames[m_AdvertisementSettingLoop++];

        status = m_pPort->GetPropertyCache()->GetPropertyBuffer(
                advertisementName,
                pulInfoBuffer,
                ppbInfoBuffer );

         if ( status != NDIS_STATUS_SUCCESS )
         {
            WFCError( "GetP2pAdvertisementSettings 0x%x", status );
            continue;
         }

         if ( *pulInfoBuffer == 0 )
         {
            DtPrint( "GetP2pAdvertisementSettings name=%d size=0\n", advertisementName);
            continue;
         }

         switch ( advertisementName )
         {
            case WfcPortPropertyP2PDeviceCaps:
                *pOid = OID_DOT11_WFD_DEVICE_CAPABILITY;
                break;
            case WfcPortPropertyP2PAdditionalIEs:
                *pOid = OID_DOT11_WFD_ADDITIONAL_IE;
                break;
            case WfcPortPropertyP2PDeviceInfo:
                *pOid = OID_DOT11_WFD_DEVICE_INFO;
                break;
            case WfcPortPropertyP2PGroupCaps:
                *pOid = OID_DOT11_WFD_GROUP_OWNER_CAPABILITY;
                break;
            case WfcPortPropertyP2PSecondaryDeviceTypeList:
                *pOid = OID_DOT11_WFD_SECONDARY_DEVICE_TYPE_LIST;
                break;
            case WfcPortPropertyP2PAdvertisementList:
                *pOid = OID_DOT11_WFD_SERVICE_ADVERTISEMENT_LIST;
                break;
            case WfcPortPropertyP2PBackgroundDiscoverySettings:
                *pOid = OID_DOT11_WFD_START_BACKGROUND_DISCOVERY;
                break;
            default:
                status = STATUS_NO_MORE_ENTRIES;
         }
         break;
     }
     if ( m_AdvertisementSettingLoop >= m_cAdvertisementNames )
     {
        status = STATUS_NO_MORE_ENTRIES;
     }

    return status;
}


NDIS_STATUS CResumeAdvSettingJob::InitializeForResume( 
    _In_ CAdapter * pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _In_ NDIS_OID Oid,
    _In_ ULONG OidInfoBufferSize,
    _In_ BYTE * pbOidInfoBuffer )
{
    NDIS_STATUS status;
    POID_HANDLER_ENTRY pHandlerInfo;

    TraceActEntry();

    m_pAdapter = pAdapter;
    m_NdisPortNumber = NdisPortNumber;

    //
    // Make a dummy OID so we can reuse SetPacketFilterJob
    //
    RtlZeroMemory( &m_OidRequest, sizeof(NDIS_OID_REQUEST) );
    m_OidRequest.RequestType = NdisRequestSetInformation;
    m_OidRequest.PortNumber = NdisPortNumber;
    m_OidRequest.DATA.SET_INFORMATION.Oid = Oid;
    m_OidRequest.DATA.SET_INFORMATION.InformationBufferLength = OidInfoBufferSize;
    m_OidRequest.DATA.SET_INFORMATION.InformationBuffer = (PVOID) pbOidInfoBuffer;
    pHandlerInfo = COidJobBase::s_FindOidHandlerEntry( Oid);
    NT_ASSERT( pHandlerInfo );

    status = COidJobBase::InitializeForOid(
                        pHandlerInfo->SetMethodJobType,
                        pAdapter,
                        pHandlerInfo,
                        &m_OidRequest);
    NT_ASSERT( NT_SUCCESS( status ) );

    TraceActExit(status);
    return status;
}

