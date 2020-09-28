/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    rxmgr.cpp

Abstract:

    Contains code for the WDI RxMgr.

Environment:

    Kernel mode

Author:


--*/
#include "precomp.hpp"
#include "rxmgr.tmh"

NDIS_IO_WORKITEM_FUNCTION RxMgrWorkItemRoutine;


NDIS_STATUS CRxPeerTable::Initialize(
    _In_ CAdapter * pAdapter,
    _In_ UINT8 MaxNumPeers)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();
    m_pAdapter = pAdapter;
    m_MaxNumPeers = MaxNumPeers;
    ndisStatus = CNdisSpinLock::CreateInstance( &m_LockRxPeerTable );
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError( "[rxmgr] CRxPeerTable failed to create Lock" );
        goto exit;
    }

exit:
    TraceExit( ndisStatus );
    return ndisStatus;
};


NDIS_STATUS CRxPeerTable::RxPeerEntryAdd(
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ PCWDI_MAC_ADDRESS PeerAddr)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    UINT i;
    CAutoLock lock( m_LockRxPeerTable );

    TraceEntry();
    
    for ( i =0; i < m_TableSize; i++ )
    {
        if ( m_PeerEntryArray[i].RxPeerState == RxPeerStateInvalid )
        {
            //
            // Found an empty slot
            //
            break;
        }
    }
    if ( i >= m_TableSize ) 
    {
        ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
    }
    else
    {
        CPort* pPort = NULL;
        pPort = m_pAdapter->GetPortFromPortId(PortId);
        if (!pPort)
        {
            ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        }
        else
        {
            // If Port is stopped/paused, new peers are stopped
            if (!pPort->PortCanSendAndReceive())
            {
                m_PeerEntryArray[i].RxPeerState = RxPeerStateStopped;
            }
            else
            {
                // Peers on a station port are paused until we recieve association completion
                // This assumes that AddPeer *always* comes before assocation completion!
                if (pPort->PortMustPauseRxUntilAssociateComplete())
                {
                    m_PeerEntryArray[i].RxPeerState = RxPeerStatePausedForAssoc;
                }
                else 
                {
                    m_PeerEntryArray[i].RxPeerState = RxPeerStateStarted;
                }
            }
            m_PeerEntryArray[i].PortId = PortId;
            m_PeerEntryArray[i].PeerId = PeerId;
            RtlCopyMemory(&m_PeerEntryArray[i].PeerAddr,
                PeerAddr,
                sizeof(WDI_MAC_ADDRESS));

            WFCTrace( "[rxmgr] Added RxPeerId: %d, Port: %x, MAC: %!MACADDR!, State: %!WFC_RX_PEER_STATE!, index: %u",
                PeerId, PortId, m_PeerEntryArray[i].PeerAddr.Address, m_PeerEntryArray[i].RxPeerState, i );
        }

    }

    TraceExit( ndisStatus );
    return ndisStatus;
}


NDIS_STATUS CRxPeerTable::RxPeerEntryRemove(
    _In_ WDI_PEER_ID PeerId)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_INVALID_DATA;
    UINT i;
    CAutoLock lock( m_LockRxPeerTable );

    TraceEntry();
    
    for ( i =0; i < m_TableSize; i++ )
    {
        if ((m_PeerEntryArray[i].RxPeerState != RxPeerStateInvalid) &&
            (m_PeerEntryArray[i].PeerId == PeerId))
        {
            //
            // Found the slot
            //
            if (RxPeerStatePausedForAssocWithData == m_PeerEntryArray[i].RxPeerState)
            {
                // If the peer is paused, notify the caller to start the work item
                // to release the frames
                // This is safe if and only if, the LE does *NOT* recycle the PeerId
                // before we drain the queue.  Hence the WFCError and NT_ASSERT
                ndisStatus = NDIS_STATUS_INDICATION_REQUIRED;
                WFCError( "[rxmgr] PeerId %d (%!MACADDR!) is deleted with pending frames! Will cause bugs if PeerId is re-used before RX queue is drained.",
                    PeerId, m_PeerEntryArray[i].PeerAddr.Address );
                NT_ASSERTMSG_NOASSUME( "Do not delete peers until all pending frames have been returned to LE (wait for RX_RESUME)", FALSE );
            }
            else
            {
                ndisStatus = NDIS_STATUS_SUCCESS;
            }
            m_PeerEntryArray[i].RxPeerState = RxPeerStateInvalid;
            break;
        }
    }

    TraceExit( ndisStatus );
    return ndisStatus;
}


NDIS_STATUS CRxPeerTable::RxPeerGetPortIdState( 
    _In_ WDI_PEER_ID PeerId,
    _Out_ WDI_PORT_ID* pPortId,
    _Out_ WFC_RX_PEER_STATE * pRxPeerState )
/*++

    Look up port id and peer state for a given peer

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_INVALID_DATA;
    UINT i;
    CAutoLock lock( m_LockRxPeerTable );

    TraceEntry();
    for ( i =0; i < m_TableSize; i++ )
    {
        if ((m_PeerEntryArray[i].RxPeerState != RxPeerStateInvalid) &&
            (m_PeerEntryArray[i].PeerId == PeerId))
        {
            //
            // Found the slot
            //
            *pPortId = m_PeerEntryArray[i].PortId;
            *pRxPeerState = m_PeerEntryArray[i].RxPeerState;
            ndisStatus = NDIS_STATUS_SUCCESS;
            break;
        }
    }

    TraceExit ( ndisStatus );
    return ndisStatus;
}


NDIS_STATUS CRxPeerTable::RxPeersForPortSetState( 
    _In_ WDI_PORT_ID PortId,
    _In_ WFC_RX_PEER_STATE RxPeerState )
/*++

    Go thru peer table to set all peer states of the port.

    Returns NDIS_STATUS_INDICATION_REQUIRED if at least one peer is RxPeerStatePausedForAssocWithData.
    Otherwise returns NDIS_STATUS_SUCCESS (even if no peers exist for this port).

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    UINT i;
    CAutoLock lock( m_LockRxPeerTable );

    TraceEntry();

    //
    // Ports can only stop or start
    //
    NT_ASSERT( (RxPeerState == RxPeerStateStarted) ||
        (RxPeerState == RxPeerStateStopped) );

    for (i = 0; i < m_TableSize; i++)
    {
        if ((m_PeerEntryArray[i].RxPeerState != RxPeerStateInvalid) &&
            (m_PeerEntryArray[i].PortId == PortId))
        {
            //
            // Found the slot
            //
            WFCTrace( "[rxmgr] Update RxPeerId %d from %!WFC_RX_PEER_STATE! to %!WFC_RX_PEER_STATE!", 
                m_PeerEntryArray[i].PeerId,
                m_PeerEntryArray[i].RxPeerState,
                RxPeerState );

            //
            // Either we're stopping the port and so the peers can be in many states,
            // Or the peers should all be stopped and we're starting the port.
            //
            // Exception might be the self-peer on a restart...
            //
            NT_ASSERT( (RxPeerState == RxPeerStateStopped) ||
                (m_PeerEntryArray[i].RxPeerState == RxPeerStateStopped) ||
                (m_PeerEntryArray[i].RxPeerState == RxPeerStatePausedForAssoc) );
            if (m_PeerEntryArray[i].RxPeerState == RxPeerStatePausedForAssocWithData)
            {
                // Caller needs to start the work item so peers can stop/flush now that
                // they're no longer paused.
                ndisStatus = NDIS_STATUS_INDICATION_REQUIRED;
                NT_ASSERT_NOASSUME( RxPeerState == RxPeerStateStopped );
            }
            m_PeerEntryArray[i].RxPeerState = RxPeerState;
        }
    }

    TraceExit( ndisStatus );
    return ndisStatus;
}


NDIS_STATUS CRxPeerTable::RxPeerPauseWithData(
    _In_ WDI_PEER_ID PeerId )
    /*++

    Performs an 'atomic' change of the peer state for the given peer
    from RxPeerStatePausedForAssoc to RxPeerStatePausedForAssocWithData.

    Returns NDIS_STATUS_SUCCESS if the peer is RxPeerStatePausedForAssoc.
    Returns NDIS_STATUS_INDICATION_REQUIRED if the peer is in any other state.

    --*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
    UINT i;
    // Don't need to use InterlockedCompareExchange because we're already inside a lock
    CAutoLock lock( m_LockRxPeerTable );

    TraceEntry( );

    for (i = 0; i < m_TableSize; i++)
    {
        if ((m_PeerEntryArray[i].RxPeerState != RxPeerStateInvalid) &&
            (m_PeerEntryArray[i].PeerId == PeerId))
        {
            //
            // Found the slot
            //

            if (RxPeerStatePausedForAssocWithData == m_PeerEntryArray[i].RxPeerState)
            {
                // If old state was 'WithData', then Nothing to do
                WFCTrace( "[rxmgr] RxPeerId %d already RxPeerStatePausedForAssocWithData",
                    m_PeerEntryArray[i].PeerId );
                ndisStatus = NDIS_STATUS_SUCCESS;
            }
            else if (RxPeerStatePausedForAssoc == m_PeerEntryArray[i].RxPeerState)
            {
                // If no data, then just update state
                WFCTrace( "[rxmgr] Update RxPeerId %d from RxPeerStatePausedForAssoc to RxPeerStatePausedForAssocWithData",
                    m_PeerEntryArray[i].PeerId );
                m_PeerEntryArray[i].RxPeerState = RxPeerStatePausedForAssocWithData;
                ndisStatus = NDIS_STATUS_SUCCESS;
            }
            else
            {
                // This leaves Stopped or Started, neither of which need updating
                // But do require the caller to start the work item
                WFCTrace( "[rxmgr] *NOT* Updating RxPeerId %d because it is not paused: %!WFC_RX_PEER_STATE!",
                    m_PeerEntryArray[i].PeerId,
                    m_PeerEntryArray[i].RxPeerState );
                ndisStatus = NDIS_STATUS_INDICATION_REQUIRED;
            }
            break;
        }
    }

    TraceExit( ndisStatus );
    return ndisStatus;
}

NDIS_STATUS CRxPeerTable::RxPeerUnpause(
    _In_ WDI_PEER_ID PeerId )
    /*++

    Performs an 'atomic' update on the the given peer from RxPeerStatePausedForAssoc[WithData] to RxPeerStateStarted.

    Returns NDIS_STATUS_INDICATION_REQUIRED if peer had data (and caller needs to queue the work item).
    Returns NDIS_STATUS_SUCCESS if the peer didn't have data.

    --*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
    UINT i;
    // Don't need to use InterlockedCompareExchange because we're already inside a lock
    CAutoLock lock( m_LockRxPeerTable );

    TraceEntry();

    for (i = 0; i < m_TableSize; i++)
    {
        if ((m_PeerEntryArray[i].RxPeerState != RxPeerStateInvalid) &&
            (m_PeerEntryArray[i].PeerId == PeerId))
        {
            //
            // Found the slot
            //

            if (RxPeerStatePausedForAssocWithData == m_PeerEntryArray[i].RxPeerState)
            {
                // If old state was 'WithData', then tell the caller to start the work item
                ndisStatus = NDIS_STATUS_INDICATION_REQUIRED;
            }
            else if (RxPeerStatePausedForAssoc == m_PeerEntryArray[i].RxPeerState)
            {
                // If no data, then just update state
                ndisStatus = NDIS_STATUS_SUCCESS;
            }
            else
            {
                // This leaves Stopped or Started, neither of which need updating
                WFCInfo( "WARN:[rxmgr] *NOT* Updating RxPeerId %d because it is not paused: %!WFC_RX_PEER_STATE!",
                    m_PeerEntryArray[i].PeerId,
                    m_PeerEntryArray[i].RxPeerState );
                ndisStatus = NDIS_STATUS_INVALID_STATE;
                break;
            }
            WFCTrace( "[rxmgr] Update RxPeerId %d from %!WFC_RX_PEER_STATE! to RxPeerStateStarted",
                m_PeerEntryArray[i].PeerId,
                m_PeerEntryArray[i].RxPeerState );
            m_PeerEntryArray[i].RxPeerState = RxPeerStateStarted;
            break;
        }
    }

    TraceExit( ndisStatus );
    return ndisStatus;
}

NDIS_STATUS CRxPeerTable::RxPeerGetPeerId(
    _In_ PCWDI_MAC_ADDRESS PeerAddr,
    _Out_ WDI_PEER_ID *pPeerId)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_INVALID_DATA;
    UINT i;
    CAutoLock lock(m_LockRxPeerTable);

    TraceEntry();
    for (i = 0; i < m_TableSize; i++)
    {
        if ((m_PeerEntryArray[i].RxPeerState != RxPeerStateInvalid) &&
            AreMacAddressEqual(&m_PeerEntryArray[i].PeerAddr, PeerAddr))
        {
            //
            // Found the slot
            //
            *pPeerId = m_PeerEntryArray[i].PeerId;
            ndisStatus = NDIS_STATUS_SUCCESS;
            break;
        }
    }
    TraceExit( ndisStatus );
    return ndisStatus;
}


NDIS_STATUS CRxMgr::Initialize(
    _In_ CAdapter * pAdapter,
    _In_ PWFC_DATAPATH_CAPABILITIES pWfcDataPathCapabilities,
    _In_ PNDIS_MINIPORT_WDI_DATA_HANDLERS pMpWdiDataHandlers
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    m_pAdapter = pAdapter;
    m_hTalTxRx = m_pAdapter->GetMiniportTalTxRxContext();
    m_pWfcDataPathCapabilities = pWfcDataPathCapabilities;
    m_pMpWdiDataHandlers = pMpWdiDataHandlers;

    ndisStatus = m_RxPeerTable.Initialize( pAdapter, 255 );

    TraceExit( ndisStatus );
    return ndisStatus;
};


void CRxMgr::Deinitialize()
/*
    Cleanup cannot fail, and must work assuming only the constructor has run and some or all of Initialize
*/
{
    NT_ASSERT( m_PendingNbls == NULL );
    if (m_hRxWorkItem != NULL)
    {
        NdisFreeIoWorkItem( m_hRxWorkItem );
        m_hRxWorkItem = NULL;
    }

    m_pAdapter = NULL;
    m_hTalTxRx = NULL;
    m_pWfcDataPathCapabilities = NULL;
    m_pMpWdiDataHandlers = NULL;

}


NDIS_STATUS CRxMgr::RxProcessAndIndicateNblChain(
    _In_ PNET_BUFFER_LIST pPendingNbls,
    _In_ WDI_RX_INDICATION_LEVEL IndicationLevel
    )
/*++
    Process the NblChain stored in pPendingNbls but only up to the limits set in m_DpcNblLimit and m_DpcWatchdogLimit.
    Assumes Classification (determining PeerId and ExTid) have already been performed.

    S1->PayLoadType->MSDUFrag->FIPS->AggregateInOneNbl
                                                    ->No->Defrag->SecDecap&MICCheck->PauseIfLimtExceed.->AdapterPort
                                ->AMSDU->deAggregated->Process 802.3->802.11 ==
                                               -> No->Deaggr ==

     AdapterPort->PortCanRx->AllocDot11ExtStaRecvContext->AddToActiveList->IndicateToOS.

--*/
{
    PNET_BUFFER_LIST pNblCurr, pNblNext;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WFC_RX_PEER_STATE rxPeerState;
    NblChain chainToReturn, chainToIndicate, chainToPause;
    PWDI_FRAME_METADATA pWiFiFrameMetadata;
    PWFC_FRAME pWfcFrame;
    WFC_PORT_ID portId = WDI_PORT_ID_ADAPTER;
    WFC_PORT_ID portIdCurr = WDI_PORT_ID_ADAPTER;
    ULONG tick = 0;
    KDPC_WATCHDOG_INFORMATION watchdogInfo = { 0 };
    ULONG dpcTimeLimit = 0, dpcWatchdogLimit = 0;
    NTSTATUS ntStatus;
    WDI_PEER_ID pausedForAssocPeerId = WDI_PEER_ANY;

    TraceEntry();

    if (IndicationLevel != WDI_RX_INDICATION_PASSIVE)
    {
        //
        // Even if WDI_RX_INDICATION_FROM_RX_RESUME_FRAMES was run at PASSIVE_LEVEL
        // and thus not subject to  the dpc watchdogs, it is possible that the LE raised
        // the IRQL. Calling KeQueryDpcWatchdogInformation handles this just fine by
        // returning STATUS_UNSUCESSFUL, and is about as fast as KeGetCurrentIrql, so
        // we just call one API instead of 2, and hence no logging on the error path
        // because failure is expected.
        //
        ntStatus = KeQueryDpcWatchdogInformation( &watchdogInfo );
        if (NT_SUCCESS( ntStatus ))
        {
            if (watchdogInfo.DpcTimeLimit > 0)
            {
                // Set our limit to the lesser of 30% of the total or 50% of the remaining
                dpcTimeLimit = MIN( (watchdogInfo.DpcTimeLimit / 10) * 3, (watchdogInfo.DpcTimeCount / 10) * 5 );
                WFCTrace( "[rxmgr] Dpc limits: DpcTimeLimit=%u DpcTimeCount=%u dpcTimeLimit=%u",
                    watchdogInfo.DpcTimeLimit, watchdogInfo.DpcTimeCount, dpcTimeLimit );
            }
            if (watchdogInfo.DpcWatchdogLimit > 0)
            {
                // Set our limit to the lesser of 30% of the total or 50% of the remaining
                dpcWatchdogLimit = MIN( (watchdogInfo.DpcWatchdogLimit / 10) * 3, (watchdogInfo.DpcWatchdogCount / 10) * 5 );
                WFCTrace( "[rxmgr] Dispatch limits: DpcWatchdogLimit=%u DpcWatchdogCount=%u dpcWatchdogLimit=%u",
                    watchdogInfo.DpcWatchdogLimit, watchdogInfo.DpcWatchdogCount, dpcWatchdogLimit );
            }
        }
    }

    for (pNblCurr = pPendingNbls; pNblCurr != NULL; pNblCurr = pNblNext)
    {
        // Check NDIS's throttle level
        if ((m_DpcNblCount + chainToIndicate.GetCount()) >= m_DpcNblLimit)
        {
            WFCInfo( "[rxmgr] Stopping indicating due to NDIS throttle limits: m_DpcNblLimit=%u m_DpcNblCount=%u chainToIndicate=%u",
                m_DpcNblLimit, m_DpcNblCount, chainToIndicate.GetCount() );
            break;
        }

        // Aproximately once every ~64 NBLs  check and see how we're doing against the DPC watchdog limits
        if ((0 == (++tick & 0x3f)) && ((dpcTimeLimit > 0) || (dpcWatchdogLimit > 0)))
        {

            // Check for a Dpc Timeout
            ntStatus = KeQueryDpcWatchdogInformation( &watchdogInfo );

            // No way to 'timeout' if we're not running in a DPC
            if (NT_SUCCESS( ntStatus ) && ((watchdogInfo.DpcTimeCount < dpcTimeLimit) ||
                (watchdogInfo.DpcWatchdogCount < dpcWatchdogLimit)))
            {
                WFCInfo( "[rxmgr] Stopping indicating due to Dpc/Dispatch limits: DpcTimeCount=%u DpcWatchdogCount=%u",
                    watchdogInfo.DpcTimeCount, watchdogInfo.DpcWatchdogCount );
                break;
            }
        }

        pNblNext = NET_BUFFER_LIST_NEXT_NBL(pNblCurr);
        NET_BUFFER_LIST_NEXT_NBL(pNblCurr) = NULL;

        pWiFiFrameMetadata = GetWiFiFrameMetadataFromNbl( pNblCurr );
        pWfcFrame = WFC_FRAME_FROM_WDI_FRAME( pWiFiFrameMetadata );
        pWfcFrame->FrameType = RX_FRAME_TYPE_MSDU; // Set so it can be freed properly, may be override later

        ndisStatus = m_RxPeerTable.RxPeerGetPortIdState(pWfcFrame->u.Rx.PeerId, &portId, &rxPeerState);
        if ( ndisStatus != NDIS_STATUS_SUCCESS  || rxPeerState <= RxPeerStateStopped )
        {
            chainToReturn.AddTail( pNblCurr );
            continue;
        }
        else if (rxPeerState == RxPeerStatePausedForAssoc)
        {
            WFCInfo( "[rxmgr] Pausing RX data path to wait for association on control path." );
            //
            // The LE gave us a data packet before the control path completed the ASSOCIATION
            // indication, (possibly just a race) so we need to wait for the indication to complete
            // before passing the data packets up the stack.
            //
            chainToPause.AddTail( pNblCurr );
            if (chainToPause.GetCount() == 1)
            {
                pausedForAssocPeerId = pWfcFrame->u.Rx.PeerId;
            }
            continue;
        }
        pWfcFrame->u.Rx.PortId = portId;

        if ( portId != portIdCurr )
        {
            if (0 != chainToIndicate.GetCount( ))
            {
                //
                //Got a frame for a different port, ship the current list up if any
                // This may be the first time in the loop where there is nothing
                //
                WFCTrace( "[rxmgr] Indicating %u NBLs on Port %u", chainToIndicate.GetCount( ), portIdCurr );
                RxIndicatePortFrames( portIdCurr, chainToIndicate.GetHead(), chainToIndicate.GetCount(), GetReceiveFlags( IndicationLevel ) );
                chainToIndicate.Reset( );
            }
            portIdCurr = portId;
        }

        if (pWiFiFrameMetadata->u.rxMetaData.PayloadType == WDI_FRAME_MSDU)
        {
            // Set earlier
            ASSERT( pWfcFrame->FrameType == RX_FRAME_TYPE_MSDU );
        }
        else if (pWiFiFrameMetadata->u.rxMetaData.PayloadType == WDI_FRAME_MSDU_FRAGMENT)
        {
            BOOLEAN bFIPSMode = FALSE;
            NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

            ndisStatus = m_pAdapter->GetPortPropertyCache(pWfcFrame->u.Rx.PortId)->GetPropertyBoolean(
                (WFC_PROPERTY_NAME)WfcPortPropertySafeModeEnabled,
                &bFIPSMode);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                ASSERT(FALSE);
                WFCError("[rxmgr] Cannot query WfcPortPropertySafeModeEnabled (0x%08X) assuming non FIPS mode", ndisStatus);
                bFIPSMode = FALSE;
            }

            if (!bFIPSMode)
            {
                WFCError("[rxmgr] Unexpected Fragmented payload: %x in Non-FIPS mode.", pWiFiFrameMetadata->u.rxMetaData.PayloadType);
                chainToReturn.AddTail(pNblCurr);
                continue;
            }

            /*FIPS mode defragmentation*/
            pNblCurr = RxDefragmentMSDU(pWiFiFrameMetadata, portId, pWfcFrame->u.Rx.PeerId);
            if (NULL == pNblCurr)
            {
                // This was just an internal fragment nothing to see here
                continue;
            }

            //
            // Reget the frame from the new defragmented NBL, in case it changed
            //
            pWiFiFrameMetadata = GetWiFiFrameMetadataFromNbl( pNblCurr );
            pWfcFrame = WFC_FRAME_FROM_WDI_FRAME( pWiFiFrameMetadata );
            ASSERT( pWfcFrame->u.Rx.PortId == portId );

#if SUPPORT_MSDU_DEFRAG
            ASSERT( pWiFiFrameMetadata->u.rxMetaData.PayloadType == WDI_FRAME_MSDU );
#endif
        }
        else
        {
            //
            // TODO: Support AMSDU
            //

            //
            // Unsupported Payload type
            //
            WFCError( "[rxmgr] Unsupported payload type: %x", pWiFiFrameMetadata->u.rxMetaData.PayloadType );
            chainToReturn.AddTail( pNblCurr );
            continue;
        }


        //
        // stage 1s : translate to 802.11 parameters
        //
        ndisStatus = RxNblAddRecvContext( pWfcFrame, pWfcFrame->u.Rx.Rssi );
        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            chainToIndicate.AddTail( pNblCurr );
        }
        else
        {
            chainToReturn.AddTail( pNblCurr );
        }
    }

    // Return, indicate or re-queue as appropriate
    if (0 != chainToReturn.GetCount( ))
    {
        WFCInfo( "[rxmgr] Returning %u NBLs to LE due to errors", chainToReturn.GetCount() );
        OnNdisReturnNetBufferLists( chainToReturn.GetHead( ), 0 );
    }

    if (0 != chainToIndicate.GetCount( ))
    {
        //
        // Indicate up the last batch of frames
        //
        ASSERT( portId == portIdCurr ); // this is the last batch, they must be equal.
        WFCTrace( "[rxmgr] Indicating %u NBLs on Port %u", chainToIndicate.GetCount( ), portIdCurr );
        RxIndicatePortFrames( portIdCurr, chainToIndicate.GetHead( ), chainToIndicate.GetCount( ), GetReceiveFlags( IndicationLevel ) );
    }

    // We shouldn't be pausing for timeouts on passive-level indications
    NT_ASSERT( (pNblCurr == NULL) || (IndicationLevel != WDI_RX_INDICATION_PASSIVE) );

    // Splice any paused packets onto the head of the packets saved for later
    if (chainToPause.GetCount() != 0)
    {
        NT_ASSERT( pausedForAssocPeerId != WDI_PEER_ANY );
        if (pNblCurr != NULL)
        {
            NT_ASSERT( IndicationLevel != WDI_RX_INDICATION_PASSIVE );
            chainToPause.AddChainTail( pNblCurr );
        }
        pNblCurr = chainToPause.GetHead();
    }
    else
    {
        NT_ASSERT( pausedForAssocPeerId == WDI_PEER_ANY );
    }

    // All inner-loop failures are 'handled' by returning the frames to the LE
    // Thus this function only returns NDIS_STATUS_PAUSED or NDIS_STATUS_SUCCCESS
    if (pNblCurr != NULL)
    {
        RequeuePendingRxNbls( pNblCurr, pausedForAssocPeerId );
        ndisStatus = NDIS_STATUS_PAUSED;
    }
    else
    {
        RxStateChangeDrained();
        ndisStatus = NDIS_STATUS_SUCCESS;
    }

    TraceExit( ndisStatus );
    return ndisStatus;
}

void CRxMgr::RequeuePendingRxNbls(
    _In_ PNET_BUFFER_LIST pNblChain,
    _In_ WDI_PEER_ID     WaitForAssociationPeerId
    )
{
    NDIS_STATUS ndisStatus;

    TraceEntry();

    if (m_fDataIndLowResources)
    {
        PNET_BUFFER_LIST pNblChainCopy = NULL;
        WFCInfo( "[rxmgr] Low resources flag set, copying unprocessed NBLs" );

        ndisStatus = m_pAdapter->CopyNetBufferLists( pNblChain, &pNblChainCopy, 0, true );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError( "[rxmgr] CopyNetBufferLists failed (0x%08X), using originals", ndisStatus );
        }
        else
        {
            //
            // Copy succeeded, return the originals
            //
            RxReturnFrames( pNblChain );

            //
            // Set the cloned flag on the copies so we can cleanup appropriately
            //
            for (PNET_BUFFER_LIST pNblCurrCopy = pNblChainCopy;
                pNblCurrCopy;
                pNblCurrCopy = NET_BUFFER_LIST_NEXT_NBL( pNblCurrCopy ))
            {
                WFC_FRAME_FROM_WDI_FRAME( GetWiFiFrameMetadataFromNbl( pNblCurrCopy ) )->u.Rx.Cloned = true;
            }            

            //
            // Save the copies to send later, and clear the flag (otherwise upper layers might do another copy!)
            //
            pNblChain = pNblChainCopy;
            m_fDataIndLowResources = false;
        }
    }

    if (NULL == m_hRxWorkItem)
    {
        m_hRxWorkItem = NdisAllocateIoWorkItem( m_pAdapter->GetMiniportAdapterHandle() );
    }

    if (NULL == m_hRxWorkItem)
    {
        //
        // Failed to allocate a work item to process the remaining packets
        // Drop them 
        //
        WFCError( "[rxmgr] Failed to allocate work item for paused port, dropping frames!" );
        ndisStatus = NDIS_STATUS_RESOURCES;
        OnNdisReturnNetBufferLists( pNblChain, 0 );
        RxStateChangeDrained( );
    }
    else
    {
        m_PendingNbls = pNblChain;
        //
        // Here we have to synchronize with the control path just a little
        // If the control path has already switched this peer to running, then queue
        // the work item.  Otherwise, just change the state and expect the control
        // path to queue the work item.
        //
        if ((WaitForAssociationPeerId == WDI_PEER_ANY) ||
            (NDIS_STATUS_INDICATION_REQUIRED == m_RxPeerTable.RxPeerPauseWithData( WaitForAssociationPeerId )))
        {
            WFCInfo( "[rxmgr] Requeuing unprocessed NBLs" );
            NdisQueueIoWorkItem( m_hRxWorkItem, RxMgrWorkItemRoutine, this );
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        else
        {
            WFCInfo( "[rxmgr] Saving but not yet requeuing unprocessed NBLs" );
            ndisStatus = NDIS_STATUS_PENDING;
        }
    }

    TraceExit( ndisStatus );
}

void CRxMgr::RxInOrderDataInd(
        _In_ WDI_RX_INDICATION_LEVEL IndicationLevel,
        _In_ WDI_PEER_ID PeerId,
        _In_ WDI_EXTENDED_TID ExTid,
        _In_ PNDIS_RECEIVE_THROTTLE_PARAMETERS pRxThrottleParams,
        _Out_ NDIS_STATUS * pWifiStatus
        )
/*++

    Retrieve NBLs from LE/Miniport and begin processing

--*/
{
    NDIS_STATUS ndisStatus;
    PNET_BUFFER_LIST pNbl;
    UINT8   rssi=0;

    *pWifiStatus=STATUS_SUCCESS;

    TraceEntry();

    if (IndicationLevel & WDI_RX_INDICATION_FLAG_RESOURCES)
    {
        m_fDataIndLowResources = true;
        // Clear the flag so we're left with just the level
        IndicationLevel = (WDI_RX_INDICATION_LEVEL)(IndicationLevel & ~WDI_RX_INDICATION_FLAG_RESOURCES);
    }
    else
    {
        m_fDataIndLowResources = false;
    }

    // A little parameter validation
    NT_ASSERTMSG( "IndicationLevel should be WDI_RX_INDICATION_FROM_RX_RESUME_FRAMES",
        (IndicationLevel == WDI_RX_INDICATION_FROM_RX_RESUME_FRAMES) ||
        (m_pRxResumeThread == NULL) || (m_pRxResumeThread != KeGetCurrentThread()) );
    NT_ASSERTMSG( "IndicationLevel should not be WDI_RX_INDICATION_FROM_RX_RESUME_FRAMES",
        (IndicationLevel != WDI_RX_INDICATION_FROM_RX_RESUME_FRAMES) ||
        (m_pRxResumeThread == KeGetCurrentThread( )) );
    NT_ASSERTMSG( "IndicationLevel should not be WDI_RX_INDICATION_PASSIVE",
        (IndicationLevel != WDI_RX_INDICATION_PASSIVE) ||
        (KeGetCurrentIrql() == PASSIVE_LEVEL) );
    NT_ASSERTMSG( "IndicationLevel should be WDI_RX_INDICATION_PASSIVE",
        (IndicationLevel == WDI_RX_INDICATION_FROM_RX_RESUME_FRAMES) ||
        (IndicationLevel == WDI_RX_INDICATION_PASSIVE) ||
        (KeGetCurrentIrql( ) != PASSIVE_LEVEL) );
    NT_ASSERTMSG( "Missing pRxThrottleParams with IndicationLevel of WDI_RX_INDICATION_DISPATCH_FIRST_OF_DPC",
        (IndicationLevel != WDI_RX_INDICATION_DISPATCH_FIRST_OF_DPC) ||
        (pRxThrottleParams != NULL) );

    NT_ASSERTMSG( "Should only get indications if not paused", !RxStateHasData() );
    RxStateChangeData();

    if (IndicationLevel == WDI_RX_INDICATION_DISPATCH_FIRST_OF_DPC)
    {
        //
        // A dynamic limit from NDIS. Remember it as current limit
        //
        WFCTrace( "[rxmgr] NDIS Throttle NBLS %u", pRxThrottleParams->MaxNblsToIndicate );
        m_DpcNblLimit = pRxThrottleParams->MaxNblsToIndicate;
        m_DpcNblCount = 0;

        //
        // Never set this to true because we've taken ownership of all the NBLs
        // so the LE/miniport should not have it's DPC queued until we've drained
        // our queue.  We accomplish that by #1 returning NDIS_STATUS_PAUSED, and
        // then requeueing our own work item.  This should cause the LE/miniport to
        // stop interrupts and data indications until we give the go ahead. Our work
        // item processes the already recieved NBLs until we've finally drained our
        // queue, then we issue an RX_RESUME to the LE/miniport.  Which should
        // trigger more data, and possibly re-enabling of interrupts.
        //
        pRxThrottleParams->MoreNblsPending = FALSE;
    }
    else if (IndicationLevel == WDI_RX_INDICATION_PASSIVE)
    {
        // No limits when running at passive level
        m_DpcNblLimit = ULONG_MAX;
        m_DpcNblCount = 0;
    }
    else
    {
        ASSERT( (IndicationLevel == WDI_RX_INDICATION_FROM_RX_RESUME_FRAMES) ||
            (IndicationLevel == WDI_RX_INDICATION_DISPATCH_GENERAL) );
        //
        // Potentially running at dispatch level, but not the first one in a DPC
        // So do not reset any of our counters!
        //
    }

    //
    // Replace any invalid TIDs with WDI_EXT_TID_UNKNOWN
    // in case any code accidentally sign-extended the 5-bit
    // WDI_EXT_TID_UNKNOWN (31==0x1f) into the 8-bit
    // WDI_EXTENDED_TID and ended up with 0xFF
    //
    if (!IsValidTid( ExTid ))
    {
        ExTid = WDI_EXT_TID_UNKNOWN;
    }

    //
    // Get MPDUs
    //
    m_pMpWdiDataHandlers->RxGetMpdusHandler( m_hTalTxRx, PeerId, ExTid, &pNbl);

    if ( pNbl == NULL )
    {
        //
        // Some LE's indicate and then use the call-back to process the received data
        // (defragment, block-ack, management, etc.) and thus might not have any
        // real data to process here.
        //
        ndisStatus = NDIS_STATUS_SUCCESS;
        WFCInfo( "[rxmgr] Data indication, but RxGetMpdusHandler returned empty list!" );
        RxStateChangeDrained();
    }
    else
    {
        NblChain chainToReturn, chainToKeep;
        PNET_BUFFER_LIST pNblNext;

        //
        // Rssi is only available at the 1st MSDU, get it now.
        //
        m_pMpWdiDataHandlers->RxPpduRssiHandler( m_hTalTxRx, pNbl, &rssi );

        //
        // Do the (hopefully quick) work to classify everything now
        //
        for (PNET_BUFFER_LIST pNblCurr = pNbl; pNblCurr != NULL; pNblCurr = pNblNext)
        {
            pNblNext = NET_BUFFER_LIST_NEXT_NBL( pNblCurr );
            PWDI_FRAME_METADATA pWiFiFrameMetadata = GetWiFiFrameMetadataFromNbl( pNblCurr );
            PWFC_FRAME pWfcFrame = WFC_FRAME_FROM_WDI_FRAME( pWiFiFrameMetadata );

            NT_ASSERTMSG( "LE must set NET_BUFFER_LIST_MINIPORT_RESERVED(pNbl)[0] to the WDI_FRAME_METADATA",
                pWiFiFrameMetadata != NULL );
            NT_ASSERTMSG( "LE must set WDI_FRAME_METADATA::pNBL to the NET_BUFFER_LIST",
                pWiFiFrameMetadata->pNBL == pNblCurr );

            pWfcFrame->u.Rx.Cloned = FALSE; // Set so it can be freed properly

            //
            // classify NBL's of this indication which should be the same class.
            //
            pWfcFrame->u.Rx.PeerId = PeerId;
            pWfcFrame->u.Rx.ExTid = ExTid;
            ndisStatus = RxClassify(&pWfcFrame->u.Rx.PeerId, &pWfcFrame->u.Rx.ExTid, pNblCurr);


            NET_BUFFER_LIST_NEXT_NBL( pNblCurr ) = NULL;
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                chainToReturn.AddTail( pNblCurr );
            }
            else
            {
                pWfcFrame->u.Rx.Rssi = rssi;
                chainToKeep.AddTail( pNblCurr );
            }
        }

        // Return or keep as appropriate
        if (0 != chainToReturn.GetCount( ))
        {
            WFCInfo( "[rxmgr] Returning %u NBLs due to classification failures", chainToReturn.GetCount() );
            m_pMpWdiDataHandlers->RxReturnFramesHandler( m_hTalTxRx, chainToReturn.GetHead( ) );
        }

        if (chainToKeep.GetCount( ))
        {
            WFCTrace( "[rxmgr] Begin processing %u NBLs", chainToKeep.GetCount( ) );
            ndisStatus = RxProcessAndIndicateNblChain( chainToKeep.GetHead( ), IndicationLevel );
        }
        else
        {
            ndisStatus = NDIS_STATUS_SUCCESS;
            RxStateChangeDrained( );
        }
    }

    if ((ndisStatus == NDIS_STATUS_SUCCESS) && (IndicationLevel == WDI_RX_INDICATION_DISPATCH_FIRST_OF_DPC))
    {
        // The first indication of a new DPC finished without a pause.
        // If the previous DPC also finished without a Pause,
        // decrease the throttling to allow more data per DPC.
        if (!m_fPreviousDataIndWasPaused)
        {
            RxSendDecreaseThrottle();
        }
        m_fPreviousDataIndWasPaused = false;
    }
    else if (ndisStatus == NDIS_STATUS_PAUSED)
    {
        // Should *never* pause on passive indications
        ASSERT( IndicationLevel != WDI_RX_INDICATION_PASSIVE );

        // Conversely if we have to pause, tell the LE to slow down (increase throttle).
        RxSendIncreaseThrottle();
        m_fPreviousDataIndWasPaused = true;
    }

    *pWifiStatus = ndisStatus;

    TraceExit( ndisStatus );

    return;
}


bool SmoothedThrottleLevel::IncreaseThrottle()
{
    bool result = false;

    TraceEntry();

    ULONG newValue = m_value + Delta;

    //
    // Some simple smoothing
    // The actual throttle we tell the LE is in the hi byte, and
    // as long as we stay in the same level then don't send anything
    // down to the LE.  When we do change levels, jump above the
    // middle of the next level, so we have faster throttling
    // but slower un-throttling.
    //
    if (ToLevel(newValue) != ToLevel(m_value))
    {
        switch (ToLevel( m_value ))
        {
        case WDI_RxThrottleLevelNone:
            m_value = (WDI_RxThrottleLevelMedium << ShiftAmount) + (Range * 2 / 3);
            result = true;
            break;

        case WDI_RxThrottleLevelMedium:
            m_value = (WDI_RxThrottleLevelAggressive << ShiftAmount) + (Range * 3 / 4);
            result = true;
            break;

        case WDI_RxThrottleLevelAggressive:
            m_value = (WDI_RxThrottleLevelMax << ShiftAmount) + Range;
            result = true;
            break;

        case WDI_RxThrottleLevelMax:
            // Already Maxed out, nothing to do
            WFCInfo( "[rxmgr] Throttle already maxed out" );
            result = false;
            break;

        default:
            WFCError( "[rxmgr] Invalid Throttle State %X", m_value );
            result = false;
            break;
        }

    }
    else
    {
        WFCTrace( "[rxmgr] Internal Throttle increase to %X", m_value );
        m_value = newValue;
        result = false;
    }


    TraceExit( result );
    return result;
}

bool SmoothedThrottleLevel::DecreaseThrottle( )
{
    bool result = false;

    TraceEntry( );

    ULONG newValue = m_value - Delta;

    //
    // Some simple smoothing
    // The actual throttle we tell the LE is in the hi byte, and
    // as long as we stay in the same level then don't send anything
    // down to the LE.  When we do change levels, jump below the
    // middle of the next level, so we have faster un-throttling
    // but slower re-throttling.
    //
    if (ToLevel( newValue ) != ToLevel( m_value ))
    {
        switch (ToLevel( m_value ))
        {
        case WDI_RxThrottleLevelNone:
            // Already Maxed out, nothing to do
            WFCTrace( "[rxmgr] Throttle already min-ed out" );
            result = false;
            break;

        case WDI_RxThrottleLevelMedium:
            m_value = (WDI_RxThrottleLevelNone << ShiftAmount);
            result = true;
            break;

        case WDI_RxThrottleLevelAggressive:
            m_value = (WDI_RxThrottleLevelMedium << ShiftAmount) + (Range * 1 / 4);
            result = true;
            break;

        case WDI_RxThrottleLevelMax:
            m_value = (WDI_RxThrottleLevelAggressive << ShiftAmount) + (Range * 1 / 3);
            result = true;
            break;

        default:
            WFCError( "[rxmgr] Invalid Throttle State %X", m_value );
            result = false;
            break;
        }

    }
    else
    {
        WFCTrace( "[rxmgr] Internal Throttle decrease to %X", m_value );
        m_value = newValue;
        result = false;
    }

    TraceExit( result );

    return result;
}

void CRxMgr::RxSendDecreaseThrottle( )
/*++
  LE/Miniport is not sending enough buffers, tell them to speed up
--*/
{
    TraceEntry( );

    if (m_RxThrottleLevel.DecreaseThrottle())
    {
        WFCInfo( "[rxmgr] Decreasing Throttle level %X to LE", m_RxThrottleLevel.GetLogValue() );
        m_pMpWdiDataHandlers->RxThrottleHandler( m_hTalTxRx, m_RxThrottleLevel );
    }

    TraceExit( STATUS_SUCCESS );
}

void CRxMgr::RxSendIncreaseThrottle( )
/*++
  LE/Miniport is sending too many buffers, tell them to slow down
--*/
{
    TraceEntry( );

    if (m_RxThrottleLevel.IncreaseThrottle( ))
    {
        WFCInfo( "[rxmgr] Increasing Throttle level %X to LE", m_RxThrottleLevel.GetLogValue() );
        m_pMpWdiDataHandlers->RxThrottleHandler( m_hTalTxRx, m_RxThrottleLevel );
    }

    TraceExit( STATUS_SUCCESS );
}


void CRxMgr::RxReturnFrames( PNET_BUFFER_LIST pNBL )
{
    m_pMpWdiDataHandlers->RxReturnFramesHandler( m_hTalTxRx, pNBL );
}

void CRxMgr::RxIndicatePortFrames(
    _In_ WFC_PORT_ID PortId,
    _In_ PNET_BUFFER_LIST pNblChain,
    _In_ UINT numNbls,
    _In_ ULONG ReceiveFlags)
/*++

    Indicate a list a frame for the same port in the list order.
    If something goes wrong, return the list to LE/Miniport.

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    m_DpcNblCount += numNbls;

    //
    // indicate frames
    //
    ndisStatus = m_pAdapter->IndicateFrames( PortId, pNblChain, numNbls, ReceiveFlags );

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        //
        // the full list is not indicated (or it's fully indicated if success )
        //
        OnNdisReturnNetBufferLists( pNblChain, 0 );
    }
 }


void
CRxMgr:: OnNdisReturnNetBufferLists(
        _In_  PNET_BUFFER_LIST    pNbls,
        _In_  ULONG               ReturnFlags
        )
/*++

    Ndis returns us Rx indicated NBLs.
    Free memory we (wdi) added to the frames and return them to LE

--*/
{
    PNET_BUFFER_LIST pNbl, pNblNext;
    PWDI_FRAME_METADATA pWiFiFrameMetadata;
    PWFC_FRAME pWfcFrame;
    NblChain chainToReturn, chainOfCloned;

    TraceEntry();
    UNREFERENCED_PARAMETER( ReturnFlags ); // unused, this only says if Dispatch_level call

    for ( pNbl = pNbls; pNbl != NULL; pNbl = pNblNext )
    {
        pNblNext = NET_BUFFER_LIST_NEXT_NBL( pNbl );
        NET_BUFFER_LIST_NEXT_NBL( pNbl ) = NULL;

        pWiFiFrameMetadata = GetWiFiFrameMetadataFromNbl( pNbl );
        pWfcFrame = WFC_FRAME_FROM_WDI_FRAME( pWiFiFrameMetadata );
        
        switch ( pWfcFrame->FrameType )
        {
            case RX_FRAME_TYPE_MSDU :
                if (pWfcFrame->u.Rx.pDot11RecvContext)
                {
                    //no allocation for RecvContext, just clear pointer here
                    pWfcFrame->u.Rx.pDot11RecvContext = NULL;
                }
                if (pWfcFrame->u.Rx.Cloned)
                {
                    // LE doesn't know about this guy, so we just need to cleanup our stuff
                    chainOfCloned.AddTail( pNbl );
                }
                else
                {
                    chainToReturn.AddTail( pNbl );
                }
                break;

            case RX_FRAME_TYPE_FIPS_DEFRAGMENTED_MSDU:
            {
                /* Un-do the steps done for FIPS mode fragments */
                /* Reconnect the first and the second NBL */
                /* Then cleanup as if it was a normal MSDU */
                NET_BUFFER_LIST_NEXT_NBL( pNbl ) = pWfcFrame->u.Rx.MSDUFragmentNblChain;
                PNET_BUFFER_LIST pNblPrev = pNbl;
                                        
                for ( PNET_BUFFER_LIST pNblCurrent = NET_BUFFER_LIST_NEXT_NBL( pNbl );
                    pNblCurrent != NULL; 
                    pNblCurrent = NET_BUFFER_LIST_NEXT_NBL( pNblCurrent ) )
                { 
                    PNET_BUFFER pNbTraverse = NET_BUFFER_LIST_FIRST_NB( pNblPrev );
                    while ( NET_BUFFER_NEXT_NB( pNbTraverse ) != NET_BUFFER_LIST_FIRST_NB( pNblCurrent ) )
                    {
                        pNbTraverse = NET_BUFFER_NEXT_NB( pNbTraverse );
                    }
                    NET_BUFFER_NEXT_NB( pNbTraverse ) = NULL;
                    pNblPrev = pNblCurrent;
                }
                NET_BUFFER_LIST_NEXT_NBL( pNblPrev ) = pNblNext;
                pNblNext = pNbl;
                pWfcFrame->FrameType = RX_FRAME_TYPE_MSDU;

                break;
            }

#if SUPPORT_MSDU_DEFRAG
            case RX_FRAME_TYPE_DEFRAGMENTED_MSDU:
                {
                    PNET_BUFFER_LIST *ppNbl2;
                    PNET_BUFFER pNbSwap;
                    PNET_BUFFER_LIST pDefraggedNbl = pWfcFrame->u.Rx.MSDUFragmentNblChain;

                    ASSERT( pDefraggedNbl );
                    ASSERT( NET_BUFFER_LIST_FIRST_NB( pDefraggedNbl ) );

                    // Swap back in the original NET_BUFFER(s)
                    pNbSwap = NET_BUFFER_LIST_FIRST_NB( pDefraggedNbl );
                    NET_BUFFER_LIST_FIRST_NB( pDefraggedNbl ) = NET_BUFFER_LIST_FIRST_NB( pNbl );
                    NET_BUFFER_LIST_FIRST_NB( pNbl ) = pNbSwap;

                    // Loop to find the end fo the framgent NBLs
                    for (ppNbl2 = &NET_BUFFER_LIST_NEXT_NBL( pDefraggedNbl ); *ppNbl2 != NULL; ppNbl2 = &NET_BUFFER_LIST_NEXT_NBL( *ppNbl2 ))
                    { }
                    *ppNbl2 = pNblNext;
                    NET_BUFFER_LIST_NEXT_NBL( pNbl ) = NET_BUFFER_LIST_NEXT_NBL( pDefraggedNbl );
                    NET_BUFFER_LIST_NEXT_NBL( pDefraggedNbl ) = NULL;

                    // Clean up the defragmented NBL/NBs
                    NdisFreeReassembledNetBufferList( pDefraggedNbl, pWfcFrame->u.Rx.cbMSDUFragmentMPDUHeaderSize, 0 );
                    pWfcFrame->u.Rx.MSDUFragmentNblChain = NULL;
                    pWfcFrame->u.Rx.cbMSDUFragmentMPDUHeaderSize = 0;

                    // Now cleanup the fragments as if they were regular MSDUs
                    pNblNext = pNbl;
                    pWfcFrame->FrameType = RX_FRAME_TYPE_MSDU;

                }
                break;
#endif // SUPPORT_MSDU_DEFRAG

            case RX_FRAME_TYPE_AMSDU:
            default:
                //<ToDo>
                NT_ASSERT( 0 );
                break;
        }
    }

    if (chainToReturn.GetCount() > 0)
    {
        m_pMpWdiDataHandlers->RxReturnFramesHandler( m_hTalTxRx, chainToReturn.GetHead() );
    }

    if (chainOfCloned.GetCount() > 0)
    {
        m_pAdapter->FreeCopiedNetBufferLists( chainOfCloned.GetHead() );
    }

    TraceExit(NDIS_STATUS_SUCCESS);
}

VOID CRxMgr::WorkItemRoutine()
    /*++

    WorkItem callback to resume processing and indicating already received NblChain
    stored in m_PendingNbls.  Assumes RxEngine is paused.

    If we sucessfully process the existing m_PendingNbls chain, then invoke
    RX_RESUME.

    --*/
{
    NDIS_STATUS ndisStatus;
    PNET_BUFFER_LIST pPendingNbls = m_PendingNbls;

    TraceEntry( );

    NT_ASSERTMSG( "Logic error, work item invoked with bad state", RxStateHasData( ) );
    NT_ASSERTMSG( "Logic error, work item invoked with no pending NBLs", pPendingNbls );

    // Running at passive, so no limits!
    m_DpcNblLimit = ULONG_MAX;
    m_DpcNblCount = 0;

    ndisStatus = RxProcessAndIndicateNblChain( pPendingNbls, WDI_RX_INDICATION_PASSIVE );

    //
    // This assert might fire if multiple peers are associating *AND* data for the peers
    // arrived before the association completed.  We resumed because one of the peers
    // finished association, but the resume didn't really succeed because we still have
    // packets for the partially associated peer, and had to re-pause (or more accurately
    // just not tell the LE to resume).  I think this is highly unlikely, so if it happens
    // it would be nice to know about it, so I'm leaving the assert in here.
    //
    NT_ASSERTMSG_NOASSUME( "Resume should always work, except for association case (see comment in code)",
        ndisStatus == NDIS_STATUS_SUCCESS);

    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        // It drained the backlog, so re-enable the LE/miniport
        // Note that this might trigger more indications
        NT_ASSERTMSG( "State wrong after successful indication", !RxStateHasData( ) );
        NT_ASSERTMSG( "Logic error, Another thread is resuming?", (m_pRxResumeThread == NULL) );
        m_pRxResumeThread = KeGetCurrentThread( );
        m_DpcNblCount = 0;
        m_DpcNblLimit = ULONG_MAX;
        m_PendingNbls = NULL;
        m_pMpWdiDataHandlers->RxResumeHandler( m_hTalTxRx );
        m_pRxResumeThread = NULL;
    }

    TraceExit( ndisStatus );
}

NDIS_STATUS
CRxMgr::RxClassify(
    _Inout_ WDI_PEER_ID *pPeerId,
    _Inout_ WDI_EXTENDED_TID *pExTid,
    _In_ PNET_BUFFER_LIST pNbl)
/*++
    If *pPeerId and/or pExTid are wildcarded,
    Extract and return the PeerId and ExtTid. Use RxPeerEntryTable to get the PortId
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    if ((*pPeerId == WDI_PEER_ANY) || !IsValidTid(*pExTid))
    {
        PNET_BUFFER netBuffer = pNbl->FirstNetBuffer;
        BYTE rgBuffer[DOT11_QOS_DATA_LONG_HEADER_SIZE];

        // First crack the MAC header to look for the TA (Transmitter Address) to determine the peer
        PDOT11_DATA_SHORT_HEADER pDot11Header =
            reinterpret_cast<PDOT11_DATA_SHORT_HEADER>(NdisGetDataBuffer(netBuffer,
                sizeof(DOT11_DATA_SHORT_HEADER), &rgBuffer, 1, 0));

        // Even though we passed in a copy buffer, NdisGetDataBuffer can still return NULL when
        // The NET_BUFFER isn't big enough (or possibly other error conditions), so reject the packet
        if (NULL == pDot11Header)
        {
            WFCError( "[rxmgr] Failed to obtain DOT11_DATA_SHORT_HEADER (NB DataLength: %u)",
                NET_BUFFER_DATA_LENGTH(netBuffer) );
            ndisStatus = NDIS_STATUS_INVALID_PACKET;
            goto exit;
        }

        PCWDI_MAC_ADDRESS pAddress = reinterpret_cast<PWDI_MAC_ADDRESS>(&pDot11Header->Address2);
        ndisStatus = m_RxPeerTable.RxPeerGetPeerId(pAddress, pPeerId);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("[rxmgr] Failed to map MAC address [%02X:%02X:%02X:%02X:%02X:%02X] to PeerId", 
                pAddress->Address[0], pAddress->Address[1], pAddress->Address[2],
                pAddress->Address[3], pAddress->Address[4], pAddress->Address[5]);
            goto exit;
        }

        // Now Check if we have QOS information to see if we can get the TID (traffic identifier)
        DOT11_FRAME_CTRL frameControl = pDot11Header->FrameControl;
        if ((frameControl.Type == DOT11_FRAME_TYPE_DATA) &&
            (frameControl.Subtype & DOT11_DATA_SUBTYPE_QOS_FLAG))
        {
            // Check FromDS & ToDS to figure if we have DOT11_QOS_DATA_SHORT_HEADER or DOT11_QOS_DATA_LONG_HEADER
            // So we know where to extract the QoSControl field from
            PDOT11_80211E_CONTROL pQoSControl;
            PVOID pBuffer = NULL;
            if (frameControl.FromDS && frameControl.ToDS)
            {
                pBuffer = NdisGetDataBuffer( netBuffer, sizeof( PDOT11_QOS_DATA_LONG_HEADER ), &rgBuffer, 1, 0 );
                pQoSControl = reinterpret_cast<PDOT11_80211E_CONTROL>(
                    &reinterpret_cast<PDOT11_QOS_DATA_LONG_HEADER>(pBuffer)->QoSControl);
            }
            else
            {
                pBuffer = NdisGetDataBuffer( netBuffer, sizeof( PDOT11_QOS_DATA_SHORT_HEADER ), &rgBuffer, 1, 0 );
                pQoSControl = reinterpret_cast<PDOT11_80211E_CONTROL>(
                    &reinterpret_cast<PDOT11_QOS_DATA_SHORT_HEADER>(pBuffer)->QoSControl);
            }
            // We may have potentially re-used the same buffer so clear out these now potentially stale pointers
            pDot11Header = NULL;

            // Even though we passed in a copy buffer, NdisGetDataBuffer can still return NULL when
            // The NET_BUFFER isn't big enough (or possibly other error conditions), so reject the packet
            if (NULL == pBuffer)
            {
                WFCError( "[rxmgr] Failed to obtain DOT11_QOS_CONTROL (NB DataLength: %u, Type: %hu, Subtype: %hx FromDS: %hu, ToDS: %hu)",
                    NET_BUFFER_DATA_LENGTH( netBuffer ), frameControl.Type, frameControl.Subtype, frameControl.FromDS, frameControl.ToDS );
                ndisStatus = NDIS_STATUS_INVALID_PACKET;
                goto exit;
            }

            *pExTid = pQoSControl->TID;
        }
        else
        {
            *pExTid = WDI_EXT_TID_NON_QOS;
        }
    }

exit:

    TraceExit( ndisStatus );
    return ndisStatus;
}

NDIS_STATUS
CRxMgr::RxNblAddRecvContext( 
    _In_ PWFC_FRAME pWfcFrame,
    _In_ UINT8 Rssi
    )
/*++

    Process one NBL of one WFC frame. Add 802.11 data.
    If successful, the NBL is destined to NDIS, otherwise to return to LE.

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PNET_BUFFER_LIST pNbl = pWfcFrame->WiFiFrameMetadata.pNBL;
    PDOT11_EXTSTA_RECV_CONTEXT pDot11RecvContext = NULL;

    TraceEntry();

    ASSERT( pNbl );
    ASSERT( pWfcFrame->u.Rx.pDot11RecvContext == NULL );

    //C_ASSERT to make sure that the unused space is large enough for the DOT11_EXTSTA_RECV_CONTEXT with the necessary alignment
    C_ASSERT(sizeof(WDI_TX_METADATA) >= (unsigned int)ALIGN_UP_POINTER(sizeof(WDI_RX_METADATA),PDOT11_EXTSTA_RECV_CONTEXT) + sizeof(DOT11_EXTSTA_RECV_CONTEXT));
    pDot11RecvContext = reinterpret_cast<PDOT11_EXTSTA_RECV_CONTEXT>
        ALIGN_UP_POINTER((&pWfcFrame->WiFiFrameMetadata.u.rxMetaData) + 1, PDOT11_EXTSTA_RECV_CONTEXT);

    WFC_ASSIGN_NDIS_OBJECT_HEADER(pDot11RecvContext->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_EXTSTA_RECV_CONTEXT_REVISION_1,
        sizeof(DOT11_EXTSTA_RECV_CONTEXT));
    //
    // It appears that per frame phyId and center frequency require deep calls (CPU cycles) and 
    // the values could be handy from LE/Firmware. 
    // The fields seem unused for functions except raw mode or tests.
    // <ToDo> If necessary, it may be sensible to let LE/FW fill the information in WiFiFrameMetadata. 
    //
    //there are quite a few fields, need from LE or need to derive
    //
    pDot11RecvContext->lRSSI = Rssi; //RSSI;
    pDot11RecvContext->uReceiveFlags = 0;            //  *RAW_PACKET*
    pDot11RecvContext->uPhyId = dot11_phy_type_erp;  // see todo above
    pDot11RecvContext->uChCenterFrequency = 2437;      //see todo above
    pDot11RecvContext->ullTimestamp = 0;            // for raw 802.11 packets

    // TODO : fix this
    pDot11RecvContext->ucDataRate = 0; // (UCHAR) pRxD->WiFiRxD.DataRate;

    // copied from wificlass
    pDot11RecvContext->usNumberOfMPDUsReceived = 1;
    pDot11RecvContext->uSizeMediaSpecificInfo = 0;
    pDot11RecvContext->pvMediaSpecificInfo = NULL;

   // copied from wificlass
    /*
    pRxD->WiFiRxD.Format;    
    pRxD->WiFiRxD.MACHeaderLength;   
    pRxD->WiFiRxD.SNR;
    pRxD->WiFiRxD.NoiseFloor;
    */
    // end of TODO:
    
    // 802.1Q information Find the Tid to put in WMM
    NDIS_NET_BUFFER_LIST_8021Q_INFO ieee8021Q;
    UINT8 exTid = pWfcFrame->u.Rx.ExTid;


    ieee8021Q.Value = NULL;
    if ( exTid == WDI_EXT_TID_NON_QOS )
    {
        exTid = 0;
    }

    ieee8021Q.WLanTagHeader.WMMInfo = exTid;
    NET_BUFFER_LIST_INFO(pNbl, Ieee8021QNetBufferListInfo) = ieee8021Q.Value;

    NET_BUFFER_LIST_INFO(pNbl, MediaSpecificInformation) = pDot11RecvContext;

    pWfcFrame->u.Rx.pDot11RecvContext = pDot11RecvContext;
    pDot11RecvContext = NULL;

    TraceExit( ndisStatus );
    return ndisStatus;
}


NDIS_STATUS CRxMgr::RxPortStop(
        _In_ WDI_PORT_ID PortId,
        _In_ WFC_RX_STOP_REASON RxStopReason )
/*++

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    WFCInfo("[Rxmgr]PortStop [PortId:%d] [RxStopReason:%d]", PortId, RxStopReason);

    ndisStatus = m_RxPeerTable.RxPeersForPortSetState( PortId, RxPeerStateStopped );
    if (ndisStatus == NDIS_STATUS_INDICATION_REQUIRED)
    {
        // One of the peers associated with this stopping port had data
        // waiting for an association complete, queue up the the work item
        // so those packets can get flushed
        if (RxStopReason == RxStopReasonPause)
        {
            //in the case of a dot11reset there is no issue dropping the packets since they aren't useful
            //however in case of a pause this could cause useful packets to be flush so increase the trace level
            WFCError("[rxmgr] Requeueing unprocess NBLs due to port pause. Potentially useful NBLs may be dropped.");
        }
        else
        {
            WFCInfo("[rxmgr] Requeuing unprocessed NBLs due to port stop");
        }
        NdisQueueIoWorkItem( m_hRxWorkItem, RxMgrWorkItemRoutine, this );
        ndisStatus = NDIS_STATUS_SUCCESS;
    }

    //
    // Reorder buffer is cleaned up after TAL has issued a stop confirm
    //

    NDIS_STATUS talStatus = NDIS_STATUS_SUCCESS;
    NT_ASSERTMSG( "Stopping multiple ports simultaneously not allowed", m_PortIdWaitStopFlush == WDI_PORT_ANY );
    m_PortIdWaitStopFlush = PortId;
    RxStateChangeStop();
    m_pMpWdiDataHandlers->RxStopHandler( m_hTalTxRx, PortId, &talStatus );
    if ( talStatus == NDIS_STATUS_SUCCESS )
    {
        RxStopConfirm();
    }
    else 
    {
        NT_ASSERT( talStatus == NDIS_STATUS_PENDING );
    }

    TraceExit( ndisStatus );
    return ndisStatus;
}


NDIS_STATUS CRxMgr::RxPortRestart(
    _In_ WFC_PORT_ID PortId )
/*++

    Manage our state table can call LE

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    ndisStatus = m_RxPeerTable.RxPeersForPortSetState( PortId, RxPeerStateStarted );
    if (ndisStatus == NDIS_STATUS_INDICATION_REQUIRED)
    {
        // Somehow a peer was still waiting for association (maybe the self peer)
        // and then got data, and now we're restarting?
        // Better queue up the work item
        WFCError( "[rxmgr] Requeuing unprocessed NBLs due to port restart?" );
        NT_ASSERTMSG_NOASSUME( "Restarting with pendng data. Previous stop gone wrong?", FALSE );
        NdisQueueIoWorkItem( m_hRxWorkItem, RxMgrWorkItemRoutine, this );
        ndisStatus = NDIS_STATUS_SUCCESS;
    }
    m_pMpWdiDataHandlers->RxRestartHandler( m_hTalTxRx, PortId );

    TraceExit( ndisStatus );
    return ndisStatus;
}

void CRxMgr::RxStopConfirm()
/*++

    We always follow RXStop with an RxFlush

--*/
{
    TraceEntry();

    NT_ASSERTMSG( "Stop Confirm with unknown port", m_PortIdWaitStopFlush != WDI_PORT_ANY );

    RxStateChangeStopConfirm();

    //
    // Now that the LE is no longer giving us fragments for the stopped port
    // we can clean out any partial fragments
    //
    if (m_pRxDefragTable != NULL)
    {
        m_pRxDefragTable->RemovePortFragments( m_PortIdWaitStopFlush );
    }

    m_pMpWdiDataHandlers->RxFlushHandler( m_hTalTxRx, m_PortIdWaitStopFlush );

    TraceExit( NDIS_STATUS_SUCCESS );
}

void CRxMgr::RxFlushConfirm()
/*++

    RxFlush is always async; this concludes RxFlush
    if we do dot11reset,  tell LE to restart the port.

--*/
{
    TraceEntry();

    RxStateChangeFlushConfirm();

    TraceExit(NDIS_STATUS_SUCCESS);
}

void CRxMgr::RxIndicateStopFlushComplete( )
/*++

    Do the final work after control path and data path have both confirmed
    that the port is stopped and all relevant NBLs have been rejected or
    indicated up.

--*/
{
    CPort * pPort = NULL;

    TraceEntry();

    NT_ASSERTMSG( "Stop Complete with unknown port", m_PortIdWaitStopFlush != WDI_PORT_ANY );

    pPort = m_pAdapter->GetPortFromPortId( m_PortIdWaitStopFlush );
    m_PortIdWaitStopFlush = WDI_PORT_ANY;

    if (pPort == NULL)
    {
        WFCError( "[rxmgr] RxFlushConfirm can't map portId to port" );
    }
    else
    {
        pPort->OnRxFlushConfirm();
    }
    TraceExit( NDIS_STATUS_SUCCESS );
}

_Use_decl_annotations_
VOID RxMgrWorkItemRoutine(
    PVOID       Context,
    NDIS_HANDLE NdisIoWorkItemHandle )
    /*++

    Thunk to RxMgr work item method

    --*/
{
    CRxMgr * pRxMgr = reinterpret_cast<CRxMgr*>(Context);

    UNREFERENCED_PARAMETER( NdisIoWorkItemHandle );

    pRxMgr->WorkItemRoutine( );
}

PNET_BUFFER_LIST 
CRxMgr::RxDefragmentMSDU(
    _In_ PWDI_FRAME_METADATA pWiFiFrameMetadata,
    _In_ WDI_PORT_ID portId,
    _In_ WDI_PEER_ID peerId)
{
    DOT11_DATA_SHORT_HEADER dot11Header;
    NDIS_STATUS ndisStatus;
    PNET_BUFFER_LIST pNbl;

    TraceEntry();

    ASSERT(pWiFiFrameMetadata->u.rxMetaData.PayloadType == WDI_FRAME_MSDU_FRAGMENT);

    if (NULL == m_pRxDefragTable)
    {
        CRxDefragTable * pRxDefragTable = new CRxDefragTable( m_pAdapter );
        if ((NULL == pRxDefragTable) || (NDIS_STATUS_SUCCESS != pRxDefragTable->Initialize()))
        {
            ASSERT(FALSE);
            WFCError("[rxmgr] Failed to allocate/initialize RxDefragTable, can't defragment MSDU, dropping MPDU");
            m_pMpWdiDataHandlers->RxReturnFramesHandler( m_hTalTxRx, pWiFiFrameMetadata->pNBL );
            return NULL;
        }
        KeMemoryBarrier(); // Need to ensure ctor+Initialize complete before publishing
        m_pRxDefragTable = pRxDefragTable;
    }

    // Extract the  fragment information
    PDOT11_DATA_SHORT_HEADER pDot11Header = reinterpret_cast<PDOT11_DATA_SHORT_HEADER>(
        NdisGetDataBuffer( NET_BUFFER_LIST_FIRST_NB( pWiFiFrameMetadata->pNBL ),
        DOT11_DATA_SHORT_HEADER_SIZE, &dot11Header, 1, 0 ) );

    ASSERT( pDot11Header ); // Can't be NULL because we passed in a buffer
    __analysis_assume( pDot11Header );
    
    ndisStatus = m_pRxDefragTable->AddFragment( pWiFiFrameMetadata->pNBL,
        portId, peerId,
        pDot11Header->SequenceControl,
        pDot11Header->FrameControl.MoreFrag,
        m_fDataIndLowResources,
        &pNbl );
    
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        ASSERT( pNbl == NULL );
        return NULL;
    }

    ASSERT( pNbl );

    // We have a complete MSDU stored as a chain of MPDU NBLs
    // Put it into a single NBL and indicate it up

    BOOLEAN bFIPSMode = FALSE;

    ndisStatus = m_pAdapter->GetPortPropertyCache(portId)->GetPropertyBoolean(
        (WFC_PROPERTY_NAME)WfcPortPropertySafeModeEnabled,
        &bFIPSMode);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        ASSERT(FALSE);
        WFCError("[rxmgr] Cannot query WfcPortPropertySafeModeEnabled (0x%08X) assuming non FIPS mode", ndisStatus);
        bFIPSMode = FALSE;
    }

    if( bFIPSMode )
    {
        pWiFiFrameMetadata = GetWiFiFrameMetadataFromNbl(pNbl);
        PWFC_FRAME pWfcFrame = WFC_FRAME_FROM_WDI_FRAME( pWiFiFrameMetadata );

        /* Store the second NBL in the chain so that it can be used for un-doing the steps when returning the NBLs */
        pWfcFrame->u.Rx.MSDUFragmentNblChain = NET_BUFFER_LIST_NEXT_NBL( pNbl );

        /*Set Frame type for FIPS mode defragmentation*/
        pWfcFrame->FrameType = RX_FRAME_TYPE_FIPS_DEFRAGMENTED_MSDU;

        /*
        The following are the steps:
        1: Loop through the NBLs, and connect the Tail NB of previous NBL with first NB of current NBL
        2: Disconnect the first and second NBL in the chain
        Now the structure is such that each fragment is present in a separate NB, all present inside a single NBL */
        for (PNET_BUFFER_LIST pNblPrev = pNbl, pNblCurrent = NET_BUFFER_LIST_NEXT_NBL( pNbl );
            pNblCurrent != NULL;
            pNblPrev = pNblCurrent, pNblCurrent = NET_BUFFER_LIST_NEXT_NBL( pNblPrev ))
        { 
            /* If the previous NBL has more than one NB. Loop to find the tail NB in the NBL and connect it to the first NB in current NBL. */
            PNET_BUFFER pNbTraverse = NET_BUFFER_LIST_FIRST_NB( pNblPrev );
            while ( NET_BUFFER_NEXT_NB( pNbTraverse ) != NULL )
            {
                pNbTraverse = NET_BUFFER_NEXT_NB( pNbTraverse );
            }
            NET_BUFFER_NEXT_NB( pNbTraverse ) = NET_BUFFER_LIST_FIRST_NB( pNblCurrent );
        }
        NET_BUFFER_LIST_NEXT_NBL( pNbl ) = NULL;
    }
    else
    {
#if SUPPORT_MSDU_DEFRAG
        pWiFiFrameMetadata = GetWiFiFrameMetadataFromNbl( pNbl );
        ASSERT( pWiFiFrameMetadata->u.rxMetaData.PayloadType == WDI_FRAME_MSDU_FRAGMENT );
        pWiFiFrameMetadata->u.rxMetaData.PayloadType = WDI_FRAME_MSDU;

        // Walk the chain of NBLs (each NBL==each MPDU) and re-assemble them into a single
        // NET_BUFFER_LIST with a single NET_BUFFER with multiple MDLs
        ULONG cbHeaderSize = DOT11_DATA_SHORT_HEADER_SIZE;
        if (pDot11Header->FrameControl.FromDS && pDot11Header->FrameControl.ToDS)
        {
            cbHeaderSize = DOT11_DATA_LONG_HEADER_SIZE;
        }
        if ((pDot11Header->FrameControl.Type == DOT11_FRAME_TYPE_DATA) &&
            (pDot11Header->FrameControl.Subtype & DOT11_DATA_SUBTYPE_QOS_FLAG))
        {
            cbHeaderSize += DOT11_QOS_CONTROL_HEADER_SIZE;
        }

        // This will create a new NET_BUFFER_LIST with a new single
        // NET_BUFFER, with a new list of MDLs that will point to the same memory, but without
        // the MPDU headers.  Thus creating a single MSDU.
        PNET_BUFFER_LIST pDefraggedNbl = NdisAllocateReassembledNetBufferList( pNbl, NULL, cbHeaderSize, cbHeaderSize, 0, 0 );
        if (NULL == pDefraggedNbl)
        {
            WFCError( "[rxmgr] Failed to allocate pDefraggedNbl, dropping defragmented MSDU" );
            m_pMpWdiDataHandlers->RxReturnFramesHandler( m_hTalTxRx, pNbl );
            return NULL;
        }

        // Back up so we can add a new MPDU header in front of the MSDU
        ndisStatus = NdisRetreatNetBufferDataStart( pDefraggedNbl->FirstNetBuffer, cbHeaderSize, 0, NULL );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError( "[rxmgr] Failed to retreat for pDefraggedNbl header (ndisStatus = 0x%08X), dropping defragmented MSDU", ndisStatus );
            m_pMpWdiDataHandlers->RxReturnFramesHandler( m_hTalTxRx, pNbl );
            NdisFreeReassembledNetBufferList( pDefraggedNbl, cbHeaderSize, 0 );
            return NULL;
        }

        // Copy the MPDU header (from the first MSDU fragment) into the new buffer
        ULONG cbCopied = 0;
        ndisStatus = NdisCopyFromNetBufferToNetBuffer( NET_BUFFER_LIST_FIRST_NB( pDefraggedNbl ), 0, cbHeaderSize, NET_BUFFER_LIST_FIRST_NB( pNbl ), 0, &cbCopied );
        if ((ndisStatus != NDIS_STATUS_SUCCESS) || (cbCopied != cbHeaderSize))
        {
            WFCError( "[rxmgr] Failed to copy header to pDefraggedNbl (ndisStatus = 0x%08X), dropping defragmented MSDU", ndisStatus );
            m_pMpWdiDataHandlers->RxReturnFramesHandler( m_hTalTxRx, pNbl );
            NdisFreeReassembledNetBufferList( pDefraggedNbl, cbHeaderSize, 0 );
            return NULL;
        }
        // Now fix it up do it doesn't look like an MSDU fragment
        pDot11Header = reinterpret_cast<PDOT11_DATA_SHORT_HEADER>(
            NdisGetDataBuffer( NET_BUFFER_LIST_FIRST_NB( pDefraggedNbl ),
            DOT11_DATA_SHORT_HEADER_SIZE, NULL, 1, 0 ));
        ASSERT( pDot11Header ); // We pre-reserved and then retreated, so this should be one buffer
        if (NULL == pDot11Header)
        {
            WFCError( "[rxmgr] Failed to mutate header in pDefraggedNbl, dropping defragmented MSDU" );
            m_pMpWdiDataHandlers->RxReturnFramesHandler( m_hTalTxRx, pNbl );
            NdisFreeReassembledNetBufferList( pDefraggedNbl, cbHeaderSize, 0 );
            return NULL;
        }
        ASSERT( pDot11Header->FrameControl.MoreFrag );
        pDot11Header->FrameControl.MoreFrag = 0;
        ASSERT( pDot11Header->SequenceControl.FragmentNumber == 0 );

        //
        // TODO: figure out security decapsulation and MIC check
        //

        ASSERT( NULL == NET_BUFFER_LIST_NEXT_NBL( pDefraggedNbl ) );
        NET_BUFFER_LIST_NEXT_NBL( pDefraggedNbl ) = NET_BUFFER_LIST_NEXT_NBL( pNbl );
        NET_BUFFER_LIST_NEXT_NBL( pNbl ) = NULL;
        PNET_BUFFER pSwapNB = NET_BUFFER_LIST_FIRST_NB( pDefraggedNbl );
        NET_BUFFER_LIST_FIRST_NB( pDefraggedNbl ) = NET_BUFFER_LIST_FIRST_NB( pNbl );
        NET_BUFFER_LIST_FIRST_NB( pNbl ) = pSwapNB;
        PWFC_FRAME pWfcFrame = WFC_FRAME_FROM_WDI_FRAME( pWiFiFrameMetadata );
        pWfcFrame->FrameType = RX_FRAME_TYPE_DEFRAGMENTED_MSDU;
        pWfcFrame->u.Rx.MSDUFragmentNblChain = pDefraggedNbl;
        pWfcFrame->u.Rx.cbMSDUFragmentMPDUHeaderSize = cbHeaderSize;
#endif // SUPPORT_MSDU_DEFRAG
    }

    return pNbl;
}

NDIS_STATUS CRxMgr::RemovePeerId(
    _In_ WDI_PEER_ID PeerId
    )
{
    NDIS_STATUS ndisStatus;

    TraceEntry();

    ndisStatus = m_RxPeerTable.RxPeerEntryRemove( PeerId );
    if (ndisStatus == NDIS_STATUS_INDICATION_REQUIRED)
    {
        // This peer has data waiting for an association complete, queue up the 
        // workitem to process frames
        WFCInfo( "[rxmgr] Requeuing unprocessed NBLs due to peer (%d) delete", PeerId );
        NdisQueueIoWorkItem( m_hRxWorkItem, RxMgrWorkItemRoutine, this );
        ndisStatus = NDIS_STATUS_SUCCESS;
    }
    if (NULL != m_pRxDefragTable)
    {
        m_pRxDefragTable->RemovePeerFragments( PeerId );
    }

    TraceExit( ndisStatus );
    return ndisStatus;
}

void CRxMgr::OnAssociationComplete(
    _In_ WDI_PEER_ID PeerId
    )
{
    NDIS_STATUS status;
    
    TraceEntry();

    status = m_RxPeerTable.RxPeerUnpause( PeerId );
    if (NDIS_STATUS_INDICATION_REQUIRED == status)
    {
        WFCInfo( "[rxmgr] Requeuing unprocessed NBLs" );
        NdisQueueIoWorkItem( m_hRxWorkItem, RxMgrWorkItemRoutine, this );
    }

    TraceExit( status );
}

void CRxMgr::RxStateChangeFlushConfirm( )
/*++

    Atomic state transition to acknowledge a flush confirm.
    Treats any existing data as potentially pre-stop and thus
    only completing the Stop/Flush if there is no data in-progress.

--*/
{
    volatile LONG oldState;
    LONG newState;
    do
    {
        oldState = m_RxState;
        NT_ASSERTMSG( "Illegal flush confirm without flush", (oldState & rmsStopFlushDrainMask) == rmsWaitingForFlush );

        if (oldState == rmsWaitingForFlush)
        {
            newState = rmsWaitingForData;
        }
        else if (oldState == rmsWaitingForFlushWithData )
        {
            newState = rmsWaitingForDrain;
        }
        else
        {
            WFCError( "[rxmgr] FlushConfirm indicated from wrong state: %x", oldState );
            break;
        }
    } while (oldState != InterlockedCompareExchange( (volatile LONG*)&m_RxState, newState, oldState ));

    if (oldState == rmsWaitingForFlush)
    {
        RxIndicateStopFlushComplete( );
    }
}

