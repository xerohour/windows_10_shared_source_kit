/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Port.cpp

Abstract:

    Implementation for Port functions

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "Port.tmh"

#define IEEE_ETHERNET_HEADER_SIZE 14

CPort::CPort() :
    m_pAdapter(NULL),
    m_NdisPortNumber(INVALID_NDIS_PORT_NUMBER),
    m_WfcPortId(WDI_PORT_ID_ADAPTER),
    m_WfcPortType(WfcPortTypeUnspecified),
    m_PortPowerState(NdisDeviceStateUnspecified), 
    m_State(PORT_PAUSED),
    m_PauseRefCount(1),
    m_TxDId(0),
    m_PortLock(NULL),
    m_Dot11ResetCallback(NULL),
    m_PauseCallback(NULL),
    m_JobsCompleteCallback(NULL),
    m_Dot11State(WfcPortDot11StateInit),
    m_Dot11ResetCallbackEvent(TRUE),
    m_PauseCallbackEvent(TRUE),
    m_JobsCompleteCallbackEvent(TRUE),
    m_KeyConfigured(FALSE),
    m_EventRegistrationToken(WFC_REGISTRATION_TOKEN_INVALID),
    m_NeighborRequestTimerToken(WFC_TIMER_REGISTRATION_TOKEN_INVALID),
    m_OpportunisticRoamTimerToken(WFC_TIMER_REGISTRATION_TOKEN_INVALID),
    m_bOpportunisticRoamRequested(false),
    m_UpdateStateInProgress(FALSE),
    m_rxPendingFrames(0),
    m_txPendingFrames(0),
    m_fWaitForRxReturn(WaitForRxReturn),
    m_pRoamJob(NULL),
    m_pReconnectJob(NULL),
    m_pDisconnectJob(NULL),
    m_pAssociationParametersRequestResponderJob(NULL),
    m_pPeerList(NULL),
    m_pConnectHistory(NULL),
    m_ScanSkipCount(0),
    m_FIPSMode(WDI_FIPS_MODE_UNKNOWN),
    m_LastPMKIDRequest(0),
    m_TxPortQueue(),
    m_bIsTxPortQueueAllowed(false),
    m_BSSTransitionRequestFrameReceived(false),
    m_pRoamTraceLoggingData(NULL),
    m_QueuedJobCount(0)
{    
    RtlZeroMemory(m_MacAddress, sizeof(DOT11_MAC_ADDRESS));
    RtlZeroMemory(m_MacAddressForScan, sizeof(DOT11_MAC_ADDRESS));
    
#ifdef WDI_FUTURE_COMPLIANT_FASTROAM_CACHE
    RtlZeroMemory(&m_TransitionInfoCache, sizeof(WFC_TRANSITION_INFO));
    RtlZeroMemory(&m_LastSsid, sizeof(DOT11_SSID));
    RtlZeroMemory(&m_LastChannelInfo, sizeof(WFC_BSS_ENTRY_CHANNEL_INFO));
    RtlZeroMemory(&m_LastBssid, sizeof(DOT11_MAC_ADDRESS));
#endif
}

CPort::~CPort()
{
    // Jobs must be cleaned up
    NT_ASSERTMSG("Better AP Roam job not empty when port is being deleted", !m_pRoamJob);
    if ( m_pRoamJob ) 
    {
        delete ( m_pRoamJob );
        m_pRoamJob = NULL;
    }

    NT_ASSERTMSG("Reconnect roam job not empty when port is being deleted", !m_pReconnectJob);
    if ( m_pReconnectJob ) 
    {
        delete ( m_pReconnectJob );
        m_pReconnectJob = NULL;
    }

    NT_ASSERTMSG("Disconnect job not empty when port is being deleted", !m_pDisconnectJob);
    if ( m_pDisconnectJob ) 
    {
        delete ( m_pDisconnectJob );
        m_pDisconnectJob = NULL;
    }

    if ( m_PortLock ) 
    {
        delete ( m_PortLock );
    }

    if ( m_pPeerList )
    {
        delete (m_pPeerList);
    }

    if ( m_pConnectHistory )
    {
        delete (m_pConnectHistory);
    }
    
    if ( m_EventRegistrationToken != WFC_REGISTRATION_TOKEN_INVALID )
    {
        m_pAdapter->GetNotificationManager()->DeregisterNotificationHandler(
            m_EventRegistrationToken );
        m_EventRegistrationToken = WFC_REGISTRATION_TOKEN_INVALID;
    }

    if (m_NeighborRequestTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        m_pAdapter->GetEventQueue()->DeregisterTimeoutCallback(
            m_NeighborRequestTimerToken);
        m_NeighborRequestTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    if (m_OpportunisticRoamTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        m_pAdapter->GetEventQueue()->DeregisterTimeoutCallback(
            m_OpportunisticRoamTimerToken);
        m_OpportunisticRoamTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    if ( m_pbNloCommand )
    {
        delete[] m_pbNloCommand;
    }

    if (m_pRoamTraceLoggingData)
    {
        delete m_pRoamTraceLoggingData;
        m_pRoamTraceLoggingData = NULL;
    }
}

NDIS_STATUS 
CPort::Initialize(
    _In_ CAdapter* pAdapter,
    _In_ WFC_PORT_TYPE PortType,
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _In_ UINT16 PortId,
    _In_reads_bytes_(6) DOT11_MAC_ADDRESS MacAddress)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
#if WDI_DISABLED
    NET_BUFFER_LIST_POOL_PARAMETERS NBLPoolParams = { 0 };
    NET_BUFFER_POOL_PARAMETERS NBPoolParams = { 0 };
#endif // WDI_DISABLED

    TraceEntry();

    m_WfcPortType = PortType;
    m_pAdapter = pAdapter;
    m_NdisPortNumber = NdisPortNumber;
    m_PortPowerState = NdisDeviceStateD0;

    m_liNloStart.QuadPart = 0;
    m_pbNloCommand = NULL;
    m_cbNloCommand = 0;

    m_WfcPortId = PortId;
    RtlCopyMemory(m_MacAddress, MacAddress, sizeof(DOT11_MAC_ADDRESS));
    RtlCopyMemory(m_MacAddressForScan, MacAddress, sizeof(DOT11_MAC_ADDRESS));

#ifdef WDI_FUTURE_COMPLIANT_FASTROAM_CACHE
    m_TransitionInfoCache.NumberTransitionEntries = WFC_TRANSITION_ENTRY_COUNT;
    m_TransitionInfoCache.NumberDestinationEntries = WFC_TRANSITION_DEST_ENTRY_COUNT;
#endif

    // Initialize the property cache
    ndisStatus = m_PortPropertyCache.Initialize(
        PortType,
        pAdapter->GetAdapterPropertyCache()
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Failed to reset property cache. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    ndisStatus = CNdisSpinLock::CreateInstance (&m_PortLock);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        goto exit;
    }

    m_bIsTxAbortConfirm = TRUE;
    m_bIsRxFlushConfirm = TRUE;

    m_pPeerList = new CConnectedPeerList();
    if (m_pPeerList == NULL)
    {
        WFCPortError("Failed to allocate Connected Peers List");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    //
    // STA port needs to listen for PMKID candidate list changes.
    //
    if ( PortType == WfcPortTypeExtSTA )
    {
        CBSSListManager* pBSSListManager = m_pAdapter->GetBSSListManager(WfcPortTypeExtSTA);

        ndisStatus = pBSSListManager->RegisterForChangeNotifications(
            this );

        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCPortError("Failed to register for BSS List Changes. 0x%x", ndisStatus);
            goto exit;
        }
    }

    // Use 1 for ExtSTA, so we can validate stuff. Use higher number for AP    
    ndisStatus = m_pPeerList->Initialize(
        m_pAdapter,
        m_WfcPortId,
        (PortType == WfcPortTypeExtSTA) ? 1 : WFC_CONSTANT::APAssociationTableSize );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Failed to initialize Connected Peers List. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    m_pConnectHistory = new CNetworkHistoryList();
    if (m_pConnectHistory == NULL)
    {
        WFCPortError("Failed to allocate Connected Network List");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    ndisStatus = m_pConnectHistory->Initialize(
        m_pAdapter,
        m_WfcPortId,
        WFC_CONSTANT::PortConnectHistoryListSize);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Failed to initialize Connected Networks List. Error = 0x%08x", ndisStatus);
        goto exit;
    }
     
    //
    // Register for unsolicited notifications destined for this port
    //
    ndisStatus = m_pAdapter->GetNotificationManager()->RegisterForNotifications(
        WiFiIndicationInvalid,
        WFC_COMMAND_TOKEN_INVALID,
        m_WfcPortId,
        this,
        &m_EventRegistrationToken );
    if(ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Couldn't register for unsolicited port notifications.");
        goto exit;
    }

    //
    // The initial state is paused. NDIS issues restart first. 
    //
    m_State = PORT_PAUSED;
    m_PauseRefCount = 1;
    m_bIsTxPortQueueAllowed = false;

    //
    // This member is set by NDIS and used once at setting Dx
    //
    m_bValidPmParameters = FALSE;
    RtlZeroMemory( &m_NdisPmParameters, sizeof( m_NdisPmParameters));

exit:
    TraceExit (ndisStatus);
    return ndisStatus;
}

VOID
CPort::OnTxAbortConfirm()
{
    TraceEntry();

    //check for duplicate TxAbortConfirm
    if (m_bIsTxAbortConfirm == TRUE)
    {
        WFCPortInfo("Duplicate TxAbortConfirm");
        goto exit;
    }
    m_bIsTxAbortConfirm = TRUE;
    CompletePendingCancelSendsOrHaltJobs();
exit:
    TraceExit (0);
    return;
}

VOID
CPort::OnRxFlushConfirm()
{
    TraceEntry();

    //check for duplicate RxFlushConfirm
    if (m_bIsRxFlushConfirm == TRUE)
    {
        WFCPortInfo("Duplicate RxFlushConfirm");
        goto exit;
    }
    m_bIsRxFlushConfirm = TRUE;
    CompletePendingCancelSendsOrHaltJobs();
exit:
    TraceExit (0);
    return;
}

VOID 
CPort::CompletePendingCancelSendsOrHaltJobs()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    EventQueue *eventQueue;
    IOperationCompletionCallback *dot11ResetCallback = NULL;    
    IOperationCompletionCallback *pauseCallback = NULL;   

    TraceEntry();

    BOOLEAN  nicResetPort = FALSE;
    BOOLEAN  bRestartDatapath = FALSE;
    {           
        CAutoLock lock (m_PortLock);

        //
        // if both Send and Receive lists are empty then process the state change operation and TxAbort and RxFlush have been confirmed.
        //
        if (m_txPendingFrames > 0                           ||
            ((m_rxPendingFrames > 0) && m_fWaitForRxReturn) ||
            m_bIsTxAbortConfirm != TRUE                     ||
            m_bIsRxFlushConfirm != TRUE                        )
        {
            return;
        }
    
        //
        // Receive and Transmit threads may race; only one of them should reset the state and call the LE
        // 
        if (m_UpdateStateInProgress)
        {
            WFCPortInfo ("port datapath update state is in progress");
            return;
        }

        m_UpdateStateInProgress = TRUE;

        //
        // Call LE NICResetPort outside the lock. The call needs to be done in passive level
        //
        if ( TEST_PORT_STATUS (this, PORT_IN_RESET_ASYNC) )
        {
            nicResetPort = TRUE;            
        }
    }

    if (nicResetPort)
    {        
        WFCPortInfo("calling TalTxRxResetPort");
        m_pAdapter->m_MiniportDataHandlers.TalTxRxResetPortHandler(m_pAdapter->GetMiniportTalTxRxContext(),
                                                                   GetPortId());
    }

    {
        CAutoLock lock(m_PortLock);

        if (TEST_PORT_STATUS (this, PORT_PAUSING) )
        {
            CLEAR_PORT_STATUS (this, PORT_PAUSING);
            SET_PORT_STATUS (this, PORT_PAUSED);

            pauseCallback = m_PauseCallback;
            m_PauseCallback = NULL;

        }
        
        if (TEST_PORT_STATUS (this, PORT_IN_RESET_ASYNC))
        {
            CLEAR_PORT_STATUS (this, PORT_IN_RESET_ASYNC);

            //
            // call LE to restart data path if not paused
            //
            if (!TEST_PORT_STATUS (this, PORT_PAUSED) )
            {
                bRestartDatapath = TRUE;
            }

            dot11ResetCallback = m_Dot11ResetCallback;
            m_Dot11ResetCallback = NULL;
        }

        //
        // done with status update
        //
        m_UpdateStateInProgress = FALSE;

    }

    eventQueue = m_pAdapter->GetEventQueue();

    //
    // indicate the pause completions
    //
    if (pauseCallback)
    {
        WFCPortInfo ("calling DeferOperationCompletion for pauseCallback (%p)", pauseCallback);

        ndisStatus = eventQueue->DeferOperationCompletion(pauseCallback,
                                                        NDIS_STATUS_SUCCESS,
                                                        this,
                                                        &m_PauseCallbackEvent);
        if (ndisStatus != NDIS_STATUS_SUCCESS)    
        {
            WFCPortError("[Port] eventQueue->DeferOperationCompletion(m_PauseCallback) failed %u", ndisStatus);
        }
    }

    //
    // indicate the reset completions
    //
    if (dot11ResetCallback)
    {
        WFCPortInfo ("calling DeferOperationCompletion for dot11ResetCallback (%p)", dot11ResetCallback);

        ndisStatus = eventQueue->DeferOperationCompletion(dot11ResetCallback,
                                                        NDIS_STATUS_SUCCESS,
                                                        this,
                                                        &m_Dot11ResetCallbackEvent);
        if (ndisStatus != NDIS_STATUS_SUCCESS)    
        {
            WFCPortError("[Port] eventQueue->DeferOperationCompletion(dot11ResetCallback) failed %u", ndisStatus);
        }       
    }

    //Restart datapath if necessary
    if (bRestartDatapath)
    {
        WFCPortInfo("restarting datapath");
        RestartSendAndReceivesLE();
    }

    TraceExit (ndisStatus);

}

NDIS_STATUS 
CPort::Dot11Reset(
    _In_ IOperationCompletionCallback *completionCallback)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(completionCallback);

    bool callStopSendAndReceivesLE;

    TraceEntry();

    WFCPortInfo ("NdisPortNumber %u, state %u ", m_NdisPortNumber, m_State);


    //
    // reset m_KeyConfigured. ExemptionAction will have DOT11_EXEMPT_ALWAYS value untill the flag is set to true
    // 
    m_KeyConfigured = FALSE;

    {           
        CAutoLock lock(m_PortLock);
        //
        // dont call to LE if the state is pausing or already paused 
        //
        if (TEST_PORT_STATUS (this, PORT_PAUSED | PORT_PAUSING) )
        {
            callStopSendAndReceivesLE = false; 
        }
        else
        {
            callStopSendAndReceivesLE = true;
            //If we stop datapath m_bIsTxAbortConfirm and m_bIsRxFlushConfirm need to be reset to false
            m_bIsTxAbortConfirm = FALSE;
            m_bIsRxFlushConfirm = FALSE;
            m_fWaitForRxReturn = NoWaitForRxReturn;
        }
    }

    //
    // pausing or paused means that StopSendAndReceivesLE alread has been called
    //
    if (callStopSendAndReceivesLE)
    {
        //
        // call to LE
        //
        StopSendAndReceivesLE( RxStopReasonDot11Reset );
    }
    
    {           
        CAutoLock lock(m_PortLock);
    
        //
        // if reset is already pending then fail the current one synchronously.
        //
        if ( TEST_PORT_STATUS (this, PORT_IN_RESET_2) )
        {
            WFCPortError ("reset is alreay set: invalid state");
            ndisStatus = NDIS_STATUS_INVALID_PORT_STATE;
            goto exit;
        }

        //
        // if no active sends or receives are pending then return NDIS_STATUS_SUCCESS, the caller should not expect completion        
        //
        if (m_txPendingFrames == 0                            &&
            ((m_rxPendingFrames == 0) || !m_fWaitForRxReturn) &&
            m_bIsTxAbortConfirm == TRUE                       &&
            m_bIsRxFlushConfirm == TRUE                          )
        {
            ndisStatus = NDIS_STATUS_SUCCESS;        
            WFCPortInfo ("completing the reset synchronously");
            //
            // Set the Sync Reset flag. This will prevent m_txSendActiveList and m_rxReceivectiveList lists become full
            //
            SET_PORT_STATUS (this, PORT_IN_RESET_SYNC);
        }
        else
        {
            //
            // the status cannot be paused if the lists are not empty
            //
            ASSERT( !TEST_PORT_STATUS (this, PORT_PAUSED));

            // 
            // Set the Async Reset flag.
            //        
            SET_PORT_STATUS (this, PORT_IN_RESET_ASYNC); 

            ndisStatus = NDIS_STATUS_PENDING;

            m_Dot11ResetCallback = completionCallback;
        }    
    }

    //
    // if the reset is completing syncronously then call to LE (if needed) and remove the reset flag
    //
    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        WFCPortInfo ("calling TalTxRxResetPort");

        //
        // call LE to reset the port
        //

        m_pAdapter->m_MiniportDataHandlers.TalTxRxResetPortHandler(m_pAdapter->GetMiniportTalTxRxContext(),
                                                                   GetPortId());
        //
        // restart data path if not pausing or paused
        //
        if (callStopSendAndReceivesLE)
        {
            RestartSendAndReceivesLE();
        }

        {
            CAutoLock lock(m_PortLock);
            //
            // Reset the Sync Reset flag
            //
            CLEAR_PORT_STATUS (this, PORT_IN_RESET_SYNC);
        }
    }

exit:
    TraceExit (ndisStatus);

    return ndisStatus;
}

NDIS_STATUS
CPort::Halt()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry ();

    {
        CAutoLock lock(m_PortLock);

        //
        // port should already be paused
        //
        if ( (m_State & (~PORT_SURPRISE_REMOVED)) != PORT_PAUSED )
        {
            ndisStatus = NDIS_STATUS_INVALID_PORT_STATE;
            goto exit;
        }
         
        SET_PORT_STATUS(this, PORT_HALTED);
    }
   
exit:

    TraceExit (ndisStatus);
    return ndisStatus;
}

void
CPort::SurpriseRemoved()
{
    {
        CAutoLock lock(m_PortLock);
            
        WFCPortInfo ("NdisPortNumber %u, state %u", m_NdisPortNumber, m_State);

        //
        // all sends and receives will be completed immediately after this point
        //
        SET_PORT_STATUS(this, PORT_SURPRISE_REMOVED);
    }
    //stop Tx and issue abort to LE
    m_pAdapter->m_TxMgr.StopCancelSend(GetPortId(),WDI_PEER_ANY,TxStopReasonControlPath);
    //TODO stop Rx

    //Flush NBLs in TxPortQueue;
    CompleteTxPortQueue();
}

NDIS_STATUS
CPort::Pause(
    _In_ IOperationCompletionCallback * completionCallback,
    _In_ PauseWaitForRxReturn           fWaitForRxReturn )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    {
        CAutoLock lock(m_PortLock);
            
        WFCPortInfo( "NdisPortNumber %u, state %u, m_PauseRefCount %u, fWaitForRxReturn %u, m_fWaitForRxReturn %u",
            m_NdisPortNumber, m_State, m_PauseRefCount, fWaitForRxReturn, m_fWaitForRxReturn );

        m_PauseRefCount++;
        if (m_PauseRefCount > 1)
        {
            // must be paused or pausing
            ASSERT (TEST_PORT_STATUS(this, PORT_PAUSING) || TEST_PORT_STATUS(this, PORT_PAUSED));

            // If this is a real pause (meaning we must wait for RX frames to be returned), but the port
            // is already paused, the previous pause must have already waited.
            // i.e. we support internal pause (fWaitForRxReturn is false) after a real pause, but
            // we don't support internal pause first.  This is safe because internal pause is *only*
            // used for low power (NDIS should bring us back to D0 before pausing) and dot11_reset
            // (transient state serialized with pausing).
            NT_ASSERTMSG( "Miniport pause not expected inside internal pause", (!fWaitForRxReturn || m_fWaitForRxReturn) );

            ndisStatus = NDIS_STATUS_SUCCESS;
            goto exit;
        }

        //
        // all sends and receives will be completed immediately after this point
        //
        SET_PORT_STATUS(this, PORT_PAUSING);
        // set m_bIsTxAbortConfirm and m_bIsRxFlushConfirm to FALSE to prevent race with check for 
        // serialized completion below
        m_bIsTxAbortConfirm = FALSE;
        m_bIsRxFlushConfirm = FALSE;
        m_fWaitForRxReturn = fWaitForRxReturn;
    }

    //
    // We should call into LE before m_PauseCallback assignment. Otherwise, because of race condition, the m_PauseCallback may be called before StopSendAndReceivesLE() call.
    // And m_PauseCallback won't be called untill this function returns (the below lock released).
    //
    StopSendAndReceivesLE( RxStopReasonPause );

    {
        CAutoLock lock(m_PortLock);

        //
        // if no active send or receives are pending then return NDIS_STATUS_SUCCESS, the caller should not expect completion
        //
        if (m_txPendingFrames == 0                            &&
            ((m_rxPendingFrames == 0) || !m_fWaitForRxReturn) &&
            m_bIsTxAbortConfirm == TRUE                       &&
            m_bIsRxFlushConfirm == TRUE                          )
        {
            WFCPortInfo ("Completing Synchronously NdisPortNumber %u, state %u, m_PauseRefCount %u ", 
                    m_NdisPortNumber, 
                    m_State, 
                    m_PauseRefCount);

            ndisStatus = NDIS_STATUS_SUCCESS;
            //
            // this bit already can be cleared because of race condition (in CompletePendingCancelSendsOrHaltJobs())
            //
            CLEAR_PORT_STATUS (this, PORT_PAUSING);
            
            //
            // this bit already can be set because of race condition (in CompletePendingCancelSendsOrHaltJobs())
            //
            SET_PORT_STATUS(this, PORT_PAUSED);
        }
        else
        {
            m_PauseCallback = completionCallback;

            ndisStatus = NDIS_STATUS_PENDING;
        }
    }

exit:

    TraceExit (ndisStatus);

    return ndisStatus;
}

NDIS_STATUS
CPort::Restart()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    {
        CAutoLock lock(m_PortLock);

        WFCPortInfo ("NdisPortNumber %u, state %u, m_PauseRefCount %u ", m_NdisPortNumber, m_State, m_PauseRefCount);
        
        //
        // the port should be in paused state only
        //
        if ((m_State & (~PORT_IN_RESET_2)) != PORT_PAUSED)
        {
            ndisStatus = NDIS_STATUS_INVALID_PORT_STATE;
            goto exit;
        }

        ASSERT(m_PauseRefCount != 0);

        m_PauseRefCount--;
        if (m_PauseRefCount > 0)
        {
            ndisStatus = NDIS_STATUS_SUCCESS;
            goto exit;
        }

        CLEAR_PORT_STATUS(this, PORT_PAUSED);
    }

    WFCPortInfo ("Restarting the LE, NdisPortNumber %u, state %u, m_PauseRefCount %u ", m_NdisPortNumber, m_State, m_PauseRefCount);

    RestartSendAndReceivesLE();

exit:

    TraceExit (ndisStatus);

    return ndisStatus;
}


void
CPort::UpdateLinkState()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_LINK_STATE connectedLinkState;

    ULONG cachedTxLinkSpeedInKbps;
    ULONG cachedRxLinkSpeedInKbps;

    TraceEntry();

    ASSIGN_NDIS_OBJECT_HEADER(
        connectedLinkState.Header, 
        NDIS_OBJECT_TYPE_DEFAULT,
        NDIS_LINK_STATE_REVISION_1,
        NDIS_SIZEOF_LINK_STATE_REVISION_1 );

    // Always reporting media connected
    connectedLinkState.MediaConnectState = MediaConnectStateConnected;
    connectedLinkState.MediaDuplexState = MediaDuplexStateHalf;

    status = m_PortPropertyCache.GetPropertyULong(
        (WFC_PROPERTY_NAME)WfcPortPropertyNameTxLinkSpeed,
        &cachedTxLinkSpeedInKbps );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError(
            "Port Tx link speed property was unavailable. Skipping link state notification." );
        goto exit;
    }

    status = m_PortPropertyCache.GetPropertyULong(
        (WFC_PROPERTY_NAME)WfcPortPropertyNameRxLinkSpeed,
        &cachedRxLinkSpeedInKbps);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCPortError(
            "Port Rx link speed property was unavailable. Skipping link state notification.");
        goto exit;
    }

    //
    // NDIS Link speed is in bits per second
    //
    connectedLinkState.XmitLinkSpeed = cachedTxLinkSpeedInKbps * 1000;
    connectedLinkState.RcvLinkSpeed = cachedRxLinkSpeedInKbps * 1000;

    connectedLinkState.PauseFunctions = NdisPauseFunctionsUnsupported;
    connectedLinkState.AutoNegotiationFlags = NDIS_LINK_STATE_DUPLEX_AUTO_NEGOTIATED;
    
    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_LINK_STATE,
        NULL,
        &connectedLinkState,
        sizeof(connectedLinkState) );

exit:
    TraceExit(status);
}

void CPort::UpdateLinkQuality(
    _In_ CConnectedPeer *pPeer
    )
{

    BYTE linkQualityBuffer[sizeof(DOT11_LINK_QUALITY_PARAMETERS) +
                           sizeof(DOT11_LINK_QUALITY_ENTRY)] = {0};

    DOT11_LINK_QUALITY_PARAMETERS* pLinkQualityParameters = (DOT11_LINK_QUALITY_PARAMETERS*)linkQualityBuffer;
    DOT11_LINK_QUALITY_ENTRY* pLinkQualityEntry = (DOT11_LINK_QUALITY_ENTRY*)(pLinkQualityParameters + 1);

    
    ASSIGN_NDIS_OBJECT_HEADER(
        pLinkQualityParameters->Header, 
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_LINK_QUALITY_PARAMETERS_REVISION_1,
        sizeof(*pLinkQualityParameters) );

    pLinkQualityParameters->uLinkQualityListSize = 1;
    pLinkQualityParameters->uLinkQualityListOffset = sizeof(*pLinkQualityParameters);

    RtlCopyMemory(
        pLinkQualityEntry->PeerMacAddr,
        pPeer->GetMacAddress(),
        sizeof(DOT11_MAC_ADDRESS) );

    pLinkQualityEntry->ucLinkQuality = pPeer->GetLinkQuality();

    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_DOT11_LINK_QUALITY,
        NULL,
        pLinkQualityParameters,
        sizeof(linkQualityBuffer) );        

    return;
}

VOID
CPort::CompleteTxPortQueue()
{
    PNET_BUFFER_LIST pNblToComplete = NULL;

    TraceEntry();

    // SendComplete anything in the TxPortQueue
    { // TxPrequeueLock
        CAutoLock lock(m_pAdapter->m_TxMgr.GetPrequeueLock());

        if (m_TxPortQueue.GetCount() > 0)
        {
            LONG prevCount = m_TxPortQueue.GetCount();
            WFCPortInfo("[Port][Send][Complete] %lu NBLs in TxPortQueue.  SendCompleting all.", m_TxPortQueue.GetCount());
            pNblToComplete = m_TxPortQueue.GetHead();
            m_TxPortQueue.Reset();

            InterlockedAdd(&m_pAdapter->m_TxMgr.m_TotalPrequeueCount, -prevCount);
        }
    } // TxPrequeueLock

    if (pNblToComplete != NULL)
    {
        PNET_BUFFER_LIST pCurNbl = NULL;

        for (pCurNbl = pNblToComplete; pCurNbl != NULL; pCurNbl = NET_BUFFER_LIST_NEXT_NBL(pCurNbl))
        {
            NET_BUFFER_LIST_STATUS(pCurNbl) = NDIS_STATUS_SUCCESS;
        }

        WFCPortInfo("[Port][Send][Complete] returning; first NBL is (%p)", pNblToComplete);

        NdisMSendNetBufferListsComplete(m_pAdapter->GetMiniportAdapterHandle(),
            pNblToComplete,
            0);
    }


    TraceExit(0);
}

VOID
CPort::StopSendAndReceivesLE(
    _In_ WFC_RX_STOP_REASON RxStopReason )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();
    //prevent new frames from entering prequeue
    { // TxPrequeueLock
        CAutoLock lock(m_pAdapter->m_TxMgr.GetPrequeueLock());

        m_bIsTxPortQueueAllowed = false;

    } // TxPrequeueLock

    //complete frames in the prequeue
    CompleteTxPortQueue();

    //
    // call to LE 
    //
    ndisStatus = m_pAdapter->m_TxMgr.StopCancelSend(GetPortId(), WDI_PEER_ANY, TxStopReasonControlPath);
    // if status is pending then the abort confirm will set it to true later
    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        m_bIsTxAbortConfirm = TRUE;
    }

    ndisStatus = m_pAdapter->m_RxMgr.RxPortStop(GetPortId(),RxStopReason);
    if (ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError( "RxPortStop failed" );
        m_bIsRxFlushConfirm = TRUE;
    }
    TraceExit(0);
}

VOID
CPort::RestartSendAndReceivesLE()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    //
    // call to LE 
    //
    ndisStatus = m_pAdapter->m_TxMgr.RestartSend(GetPortId(),WDI_PEER_ANY);
    ndisStatus = m_pAdapter->m_RxMgr.RxPortRestart(GetPortId());
    NT_ASSERT(ndisStatus == NDIS_STATUS_SUCCESS);

    { // TxPrequeueLock
        CAutoLock lock(m_pAdapter->m_TxMgr.GetPrequeueLock());

        m_bIsTxPortQueueAllowed = true;

    } // TxPrequeueLock

    TraceExit( ndisStatus );
}

NDIS_STATUS 
CPort::GetPortErrorStatus()
{
    NDIS_STATUS ndisStatus;

    if (TEST_PORT_STATUS (this, PORT_PAUSED))
    {
        ndisStatus = NDIS_STATUS_PAUSED;
    }
    else if (TEST_PORT_STATUS (this, PORT_IN_RESET_2))
    {
        ndisStatus = NDIS_STATUS_RESET_IN_PROGRESS;
    }
    else
    {
        ndisStatus = NDIS_STATUS_FAILURE;       // return a generic error
    }

    return ndisStatus;
}

void CPort::SetMacAddress(_In_ DOT11_MAC_ADDRESS* pMacAddress)
{
    if (m_NdisPortNumber != NDIS_DEFAULT_PORT_NUMBER)
    {
        WFCPortTrace("[Port][MacRandom] MAC Address change not supported for port %d (Current %!MACADDR!, Ignored %!MACADDR!)", 
            m_NdisPortNumber,
            m_MacAddress,
            *pMacAddress
            );
        return;
    }
    
    if (!RtlEqualMemory(
        pMacAddress,
        &m_MacAddress,
        sizeof(DOT11_MAC_ADDRESS)))
    {
        NDIS_IF_PHYSICAL_ADDRESS physicalAddress;

        WFCPortTrace( "[Port][MacRandom] Signalling change of MAC Address to NDIS: %!MACADDR!" , *pMacAddress);


        // First record the change
        RtlCopyMemory(&m_MacAddress, pMacAddress, sizeof(DOT11_MAC_ADDRESS));

        // Then, tell NDIS about it
        physicalAddress.Length = sizeof(DOT11_MAC_ADDRESS);
        RtlCopyMemory(physicalAddress.Address, pMacAddress, sizeof(DOT11_MAC_ADDRESS));

        m_pAdapter->IndicateStatus(
            m_NdisPortNumber,
            NDIS_STATUS_CURRENT_MAC_ADDRESS_CHANGE,
            NULL,
            &physicalAddress,
            sizeof(NDIS_IF_PHYSICAL_ADDRESS));
    }
    else
    {
        WFCPortTrace("[Port][MacRandom] Setting MAC Address to current value: %!MACADDR!", *pMacAddress);
    }
}

NDIS_STATUS
CPort::AddWfcFrameToPendingCount(
    _In_ PWFC_FRAME pWfcFrame)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    {
        CAutoLock lock(m_PortLock);

        if (!PortCanSendAndReceive())
        {
            ndisStatus = GetPortErrorStatus();
            goto exit;
        }

        m_txPendingFrames++;
        pWfcFrame->u.Tx.bIsPortPending = TRUE;
    }
exit:
    return ndisStatus;
}

VOID 
CPort::SendNetBufferLists(
    _In_  PNET_BUFFER_LIST NetBufferLists,
    _In_  ULONG            SendFlags)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    NDIS_STATUS failedCompletionStatus = NDIS_STATUS_SUCCESS;

    PNET_BUFFER_LIST failedNBLs = NULL;
    PNET_BUFFER_LIST currentNetBufferList = NULL;
    PNET_BUFFER_LIST nextNetBufferList = NULL;

    PWFC_FRAME pWfcFrame = NULL;

    BOOLEAN updatePortStatus = FALSE;
    BOOLEAN bTargetDescResources = FALSE;

    TraceEntry();

    if (!PortCanSendAndReceive())
    {
        WFCPortTrace ("[Port][Send] Failing sends as port is not ready for Send (State = 0x%08x)", m_State);
        // complete packet 
        failedNBLs = NetBufferLists;
        failedCompletionStatus = GetPortErrorStatus();
        goto exit;
    }

    if (m_pAdapter->m_TxMgr.m_TotalPrequeueCount > 0)
    { // TxPrequeueLock
        CAutoLock lock(m_pAdapter->m_TxMgr.GetPrequeueLock());

        //if there are any prequeued NBLs because of a lack of TxTargetDescs queue these frames
        if (m_pAdapter->m_TxMgr.m_TotalPrequeueCount > 0 &&
            m_bIsTxPortQueueAllowed                         )
        {
            ULONG prevCount = m_TxPortQueue.GetCount();
            m_TxPortQueue.AddChainTail(NetBufferLists);
            WFCPortTrace("[Port][Send] %lu frames in TxPortQueue", m_TxPortQueue.GetCount());
            InterlockedAdd(&m_pAdapter->m_TxMgr.m_TotalPrequeueCount, 
                           m_TxPortQueue.GetCount() - prevCount);
            goto exit;
        }
        else if (!m_bIsTxPortQueueAllowed)
        {
            //If Port Queue is not allowed fail the send
            WFCPortInfo("[Port][Send] Port prequeueing not allowed.  Failing sends.");
            failedNBLs = NetBufferLists;
            failedCompletionStatus = NDIS_STATUS_SUCCESS;
            goto exit;
        }
    } // TxPrequeueLock

    // Process each NBL individually
    currentNetBufferList = NetBufferLists;

    while (currentNetBufferList != NULL)
    {
        //
        // Cache a reference to the next NBL right away. We will process one NBL
        // at a time so the CurrentNBL next pointer will be set to NULL.
        //
        nextNetBufferList = NET_BUFFER_LIST_NEXT_NBL(currentNetBufferList);
        NET_BUFFER_LIST_NEXT_NBL(currentNetBufferList) = NULL;

        ndisStatus = m_pAdapter->m_TxMgr.InitializeTxWfcFrame(currentNetBufferList,GetPortId(),m_KeyConfigured);

        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            // add to port active send list
            pWfcFrame = WFC_FRAME_FROM_WDI_FRAME(GetWiFiFrameMetadataFromNbl(currentNetBufferList));
            ndisStatus = AddWfcFrameToPendingCount(pWfcFrame);
            if (ndisStatus == NDIS_STATUS_SUCCESS)
            {
                ndisStatus = m_pAdapter->m_TxMgr.ProcessNBL(currentNetBufferList, &bTargetDescResources);
            }
        }

        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            if(GetWiFiFrameMetadataFromNbl(currentNetBufferList))
            {
                FreeTxWfcFrame(currentNetBufferList, &updatePortStatus);
            }

            if (bTargetDescResources)
            {
                BOOLEAN bIsPrequeueWorkitemNeeded = FALSE;
                // queue the rest of the NBL chain in the prequeue
                NET_BUFFER_LIST_NEXT_NBL(currentNetBufferList) = nextNetBufferList;
                { // TxPrequeueLock
                    CAutoLock lock(m_pAdapter->m_TxMgr.GetPrequeueLock());

                    //only prequeue if prequeueing is allowed on this port
                    if (m_bIsTxPortQueueAllowed)
                    {
                        ULONG prevCount = m_TxPortQueue.GetCount();
                        m_TxPortQueue.AddChainTail(currentNetBufferList);
                        WFCPortTrace("[Port][Send] TargetDescInit failed for resources. Queueing %lu frames in TxPortQueue", m_TxPortQueue.GetCount() - prevCount);
                        InterlockedAdd(&m_pAdapter->m_TxMgr.m_TotalPrequeueCount,
                                       m_TxPortQueue.GetCount() - prevCount);

                        bIsPrequeueWorkitemNeeded = TRUE;
                    }
                    else
                    {
                        WFCPortTrace("[Port][Send] TargetDescInit failed for resources and prequeueing is not allowd.  Dropping frames");
                        // fail the rest of the NBL chain
                        NET_BUFFER_LIST_NEXT_NBL(currentNetBufferList) = nextNetBufferList;
                        failedNBLs = currentNetBufferList;
                        failedCompletionStatus = NDIS_STATUS_SUCCESS;
                    }
                } // TxPrequeueLock

                if (bIsPrequeueWorkitemNeeded)
                {
                    //schedule the workitem if a TxTargetDescInit was tried and failed
                    m_pAdapter->m_TxMgr.SchedulePrequeueWorkitem();
                }

                ndisStatus = NDIS_STATUS_SUCCESS;
            }
            else
            {
                // fail the rest of the NBL chain
                NET_BUFFER_LIST_NEXT_NBL(currentNetBufferList) = nextNetBufferList;
                failedNBLs = currentNetBufferList;
                failedCompletionStatus = ndisStatus;

                WFCPortError("[Port][Send] Failed to allocate WFC_FRAME and send to LE. Dropping the packet (0x%08x)",
                    ndisStatus);
            }

            goto exit;
        }

        currentNetBufferList = nextNetBufferList;
    }

exit:
    m_pAdapter->m_TxMgr.ServiceQueues();

    if (failedNBLs != NULL)
    {      
        // We just complete the NBLs back to NDIS
        for(currentNetBufferList = failedNBLs;
            currentNetBufferList != NULL;
            currentNetBufferList = NET_BUFFER_LIST_NEXT_NBL(currentNetBufferList))
        {
            NET_BUFFER_LIST_STATUS(currentNetBufferList) = failedCompletionStatus;
        }
        
        NdisMSendNetBufferListsComplete(m_pAdapter->GetMiniportAdapterHandle(),
                                        failedNBLs,
                                        (NDIS_TEST_SEND_AT_DISPATCH_LEVEL(SendFlags) ? NDIS_SEND_COMPLETE_FLAGS_DISPATCH_LEVEL : 0));
    }
    if (updatePortStatus)
    {
        CompletePendingCancelSendsOrHaltJobs();
    }

    TraceExit(ndisStatus);
}

NDIS_STATUS CPort::SendQueuedNetBufferLists()
/*++
    Returns NDIS_STATUS_SUCCESS if there are either no frames in queue or all frames in queue 
    were successfully passed to the TxMgr.
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    PNET_BUFFER_LIST pCurNbl = NULL;

    NblChain failedNblChain;

    PWFC_FRAME pWfcFrame = NULL;

    BOOLEAN updatePortStatus = FALSE;
    BOOLEAN bTargetDescResources = FALSE;
    BOOLEAN bCanPortSend = TRUE;

    TraceEntry();

    if (!PortCanSendAndReceive())
    {
        WFCPortTrace("[Port][Send] Failing sends as port is not ready for Send (State = 0x%08x)", m_State);
        bCanPortSend = FALSE;
        goto exit;
    }

    { // TxPrequeueLock
        CAutoLock lock(m_pAdapter->m_TxMgr.GetPrequeueLock());

        if (m_pAdapter->m_TxMgr.m_TotalPrequeueCount == 0 ||
            m_TxPortQueue.GetCount() == 0                    )
        {
            WFCPortTrace("[Port][Send]No NBLs queued in TxPortQueue. Returning.");
            goto exit;
        }
        //grab head but dont change m_TotalPrequeueCount
        pCurNbl = m_TxPortQueue.RemoveHead();
        
    } //TxPrequeueLock

    while (pCurNbl != NULL)
    {
        ndisStatus = m_pAdapter->m_TxMgr.InitializeTxWfcFrame(pCurNbl, GetPortId(), m_KeyConfigured);

        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            // add to port active send list
            pWfcFrame = WFC_FRAME_FROM_WDI_FRAME(GetWiFiFrameMetadataFromNbl(pCurNbl));
            ndisStatus = AddWfcFrameToPendingCount(pWfcFrame);
            if (ndisStatus == NDIS_STATUS_SUCCESS)
            {
                ndisStatus = m_pAdapter->m_TxMgr.ProcessNBL(pCurNbl, &bTargetDescResources);
            }

            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                if (GetWiFiFrameMetadataFromNbl(pCurNbl))
                {
                    FreeTxWfcFrame(pCurNbl, &updatePortStatus);
                }

                if (bTargetDescResources)
                {
                    { // TxPrequeueLock
                        CAutoLock lock(m_pAdapter->m_TxMgr.GetPrequeueLock());

                        if (m_bIsTxPortQueueAllowed)
                        {
                            m_TxPortQueue.AddHead(pCurNbl);
                            WFCPortInfo("[Port][Send]TargetDescInit failed for resources.  Adding NBL to head of TxPortQueue and returning failure.");
                            goto exit;
                        }
                        else
                        {
                            NET_BUFFER_LIST_STATUS(pCurNbl) = NDIS_STATUS_SUCCESS;
                            failedNblChain.AddTail(pCurNbl);
                            WFCPortInfo("[Port][Send]TargetDescInit failed for resources but the port is not in a state to send dropping packet");
                        }
                    } // TxPrequeueLock
                }
                else
                {
                    //fail this NBL and continue.
                    NET_BUFFER_LIST_STATUS(pCurNbl) = NDIS_STATUS_SUCCESS;
                    failedNblChain.AddTail(pCurNbl);
                    WFCPortError("[Port][Send] Failed to allocate WFC_FRAME and send to LE. Dropping the packet (0x%08x)",
                        ndisStatus);
                }
            }
        }
        else
        {
            //fail this NBL and continue
            NET_BUFFER_LIST_STATUS(pCurNbl) = NDIS_STATUS_SUCCESS;
            failedNblChain.AddTail(pCurNbl);
            WFCPortError("[Port][Send] Failed to allocate WFC_FRAME and send to LE. Dropping the packet (0x%08x)",
                ndisStatus);
        }

        { // TxPrequeueLock
            CAutoLock lock(m_pAdapter->m_TxMgr.GetPrequeueLock());

            //At this point the total count should be adjusted
            InterlockedDecrement(&m_pAdapter->m_TxMgr.m_TotalPrequeueCount);
            pCurNbl = m_TxPortQueue.RemoveHead();
        } // TxPrequeueLock
    }

    //Check if the TxPortQueue is empty, if return success.  An NBL 
    // could have come to the port after the RemoveHead but before this check
    // so reaching this point does not automaticallt mean that flag should be cleared
    { // TxPrequeueLock
        CAutoLock lock(m_pAdapter->m_TxMgr.GetPrequeueLock());
        if (m_TxPortQueue.GetCount() == 0)
        {
            WFCPortInfo("[Port][Send]TxPortQueue has been emptied.");
        }
        else
        {
            WFCPortInfo("[Port][Send]TxPortQueue is still not empty.");
            ndisStatus = NDIS_STATUS_PENDING;
        }
    } // TxPrequeueLock

exit:
    //Work item will call ServiceQueues.
    if (failedNblChain.GetHead())
    {
        WFCPortInfo("[Port][Send][Complete] SendQueueNetBufferLists failed some NBLs; first NBL is (%p), count is %lu",
            failedNblChain.GetHead(),
            failedNblChain.GetCount());
        NdisMSendNetBufferListsComplete(m_pAdapter->GetMiniportAdapterHandle(),
                                        failedNblChain.GetHead(),
                                        0);
    }
    if (updatePortStatus)
    {
        CompletePendingCancelSendsOrHaltJobs();
    }
    if (!bCanPortSend)
    {
        CompleteTxPortQueue();
    }
    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CPort::FreeTxWfcFrame(
    _In_ PNET_BUFFER_LIST pNbl,
    _Out_ PBOOLEAN pUpdatePortStatus)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PNET_BUFFER_LIST pCurNbl = NULL;

    PWFC_FRAME pWfcFrame = NULL;
    PWDI_FRAME_METADATA pWifiMeta = NULL;

    LONG countAddedToPort = 0;

    TraceEntry();

    *pUpdatePortStatus = FALSE;
    //iterate through NBLs
    for (pCurNbl = pNbl; pCurNbl != NULL; pCurNbl = NET_BUFFER_LIST_NEXT_NBL(pCurNbl))
    {
        pWifiMeta = GetWiFiFrameMetadataFromNbl(pCurNbl);
        if (pWifiMeta == NULL)
        {
            WFCPortError("[Port][Send]WifiFrameMetadata not present in NBL");
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            continue;
        }
        pWfcFrame = WFC_FRAME_FROM_WDI_FRAME(pWifiMeta);

        //check to see if NBL was cloned and needs to be cleaned up
        if (pWfcFrame->u.Tx.pCloneNbl)
        {
            WFCPortTrace("[Port][Send]cleaning up cloned NBL");
            m_pAdapter->m_TxMgr.CleanupCloneNbl(pCurNbl, pWfcFrame->u.Tx.pCloneNbl, pWfcFrame);
        }

        //check to see if frame was added to m_txPendingFrames
        if (pWfcFrame->u.Tx.bIsPortPending)
        {
            countAddedToPort++;
        }

        ndisStatus = m_pAdapter->m_FrameIdTable.FreeAndUnmapFrameId(pWfcFrame);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCPortError("[Port][Send]Unable to free frameId (0x%x)", ndisStatus);
        }

        m_pAdapter->FreeWfcFrame(pWfcFrame);
        pCurNbl->MiniportReserved[0] = NULL;
    }

    //remove from m_txPendingFrames
    {
        CAutoLock lock(m_PortLock);
        m_txPendingFrames -= countAddedToPort;
        if (0 == m_txPendingFrames)
        {

            WFCPortTrace("[Port][Send]no active sends, update port status");
            //if m_txPendingFrames is zero then check if we should complete any pending reset/pause jobs
            *pUpdatePortStatus = TRUE;
        }
    }

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID CPort::SendCompleteNetBufferLists(
    _In_ PNET_BUFFER_LIST pNbl)
{
    BOOLEAN updatePortStatus = FALSE;

    TraceEntry();

    WFCPortTrace("[Port][Send][Complete]");

    //Call FreeTxWfcFrame on chain
    FreeTxWfcFrame(pNbl, &updatePortStatus);

    // complete NBLs
    if(pNbl != NULL)
    {
        WFCPortTrace("[Port][Send][Complete] returning; first NBL is (%p)", pNbl);

        NdisMSendNetBufferListsComplete(m_pAdapter->GetMiniportAdapterHandle(),
                                        pNbl,
                                        0);
    }

    // if the flag is set then try to update the port status and complete any pending reset or pause jobs
    if(updatePortStatus)
    {
        CompletePendingCancelSendsOrHaltJobs();
    }

    TraceExit(NDIS_STATUS_SUCCESS);
}

VOID 
CPort::ReturnNBLs(
    _In_ LONG NumNBLs)
{
    WFCPortTrace ("[Port][ReturnNBLs] returning %li NBLs", NumNBLs);

    //
    // if m_rxPendingFrames is now 0 then attempt to update the port status: complete any pending reset or pause jobs
    //
    if (0 == InterlockedAdd( &m_rxPendingFrames,-NumNBLs))
    {
        CompletePendingCancelSendsOrHaltJobs();
    }
}


NDIS_STATUS CPort::IndicateFrames(
    _In_ PNET_BUFFER_LIST pNblChain,
    _In_ UINT32 NumFrames,
    _In_ ULONG ReceiveFlags)
/*++

    Gate NBL chain to indicate up from RxMgr 
    All NBLs belong to this port

    Success: all Nbls are indicated.
    Or: None is indicated. The Nbls should be returned to
        LE by the caller at RxMgr after Recv context is freed

    NB: caller from CAdapter has held the port lock

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    WFCPortTrace ("[Port][Receive] portNo %u to indicate pNblChain = %p count=%d", 
                m_NdisPortNumber, 
                pNblChain, 
                NumFrames);

    if (!PortCanSendAndReceive())
    {
        WFCPortInfo ("[Port][Receive] portNo %u state is %u", 
                m_NdisPortNumber, 
                m_State);

        ndisStatus = NDIS_STATUS_INVALID_PORT_STATE;
        goto exit;
    }

    InterlockedAdd( &m_rxPendingFrames, NumFrames );

    //
    // indicate the packets to ndis
    //
    NdisMIndicateReceiveNetBufferLists(
        m_pAdapter->GetMiniportAdapterHandle(),
        pNblChain,
        m_NdisPortNumber,
        NumFrames,
        ReceiveFlags);

    if (ReceiveFlags & NDIS_RECEIVE_FLAGS_RESOURCES)
    {
        //
        // When this flag is set NDIS won't invoke MiniportReturnNetBufferLists because
        // it is implied that they are returned as soon as the call returns.
        // WDI has a different contract with the LE: we always return the buffers,
        // so here we bridge the gap between the two APIs by injecting a call to our 
        // MiniportReturnNetBufferLists.
        //
        m_pAdapter->ReturnNetBufferLists( pNblChain, 0 );
    }

exit:
    return ndisStatus;
}

NDIS_STATUS
CPort::RegisterForDisassociationNotifications(
    _In_opt_ IDisassociationCompleteCallback* pCallback)
{

    if (pCallback != NULL &&
        m_pDisassociationCompleteCallback != NULL)
    {
        NT_ASSERT(FALSE);
        return NDIS_STATUS_INVALID_STATE;
    }

    m_pDisassociationCompleteCallback = pCallback;
    return NDIS_STATUS_SUCCESS;
}

void CPort::OnDeviceIndicationArrived(
    _In_ WDI_INDICATION_TYPE                 IndicationType,
    _In_ WFC_COMMAND_TOKEN                    CommandToken,
    _In_ WFC_PORT_ID                          PortId,
    _In_ ULONG                                EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    UNREFERENCED_PARAMETER(CommandToken);
    UNREFERENCED_PARAMETER(PortId);

    NT_ASSERT(PortId == GetPortId());

    switch( IndicationType )
    {
    case WDI_INDICATION_DISASSOCIATION:
        OnDisassociated(
            EventBufferLength,
            EventBuffer );
        break;

    case WDI_INDICATION_ROAMING_NEEDED:
        OnRoamingNeeded(
            EventBufferLength,
            EventBuffer );
        break;

    case WDI_INDICATION_LINK_STATE_CHANGE:
        OnLinkStateChange(
            EventBufferLength,
            EventBuffer );
        break;
    case WDI_INDICATION_FT_ASSOC_PARAMS_NEEDED:
        OnFTAssocParamsNeededIndicationArrived(
            EventBufferLength,
            EventBuffer );
        break;
    case WDI_INDICATION_IHV_EVENT:
        OnIhvEvent(
            EventBufferLength,
            EventBuffer );
        break;
        
    case WDI_INDICATION_ACTION_FRAME_RECEIVED:
        OnIncomingActionFrameReceived(
            EventBufferLength,
            EventBuffer );
        break;

    case WDI_INDICATION_TASK_OFFLOAD_CURRENT_CONFIG:
        OnTaskOffloadCurrentConfigReceived(
            EventBufferLength,
            EventBuffer );
        break;

    case WDI_INDICATION_BSS_ENTRY_LIST:
        OnBSSEntryNotification(
            EventBufferLength,
            EventBuffer );
        break;
        
    case WDI_INDICATION_NLO_DISCOVERY:
        OnNloDiscovery(
            EventBufferLength,
            EventBuffer );
        break;

    case WDI_INDICATION_WAKE_REASON:
        OnWakeReason(
            EventBufferLength,
            EventBuffer );
        break;

    case WDI_INDICATION_TKIP_MIC_FAILURE:
        OnTkipMicFailure(
            EventBufferLength,
            EventBuffer );
        break;

    case WDI_INDICATION_AP_ASSOCIATION_REQUEST_RECEIVED:
        OnIncomingAssociationRequestReceived(
            EventBufferLength,
            EventBuffer );
        break;

    case WDI_INDICATION_P2P_ACTION_FRAME_RECEIVED:
        OnIncomingP2PActionFrameReceived(
            EventBufferLength,
            EventBuffer );
        break;

    case WDI_INDICATION_P2P_GROUP_OPERATING_CHANNEL:
        OnOperatingChannelIndication(
            EventBufferLength,
            EventBuffer );
        break;

    case WDI_INDICATION_P2P_OPERATING_CHANNEL_ATTRIBUTES:
        OnOperatingChannelAttributesIndication(
            EventBufferLength,
            EventBuffer );
        break;

    case WDI_INDICATION_ASSOCIATION_PARAMETERS_REQUEST:
        OnAssociationParametersRequestReceived(
            EventBufferLength,
            EventBuffer );
        break;

    case WDI_INDICATION_STOP_AP:
        OnStopAPReceived(
            EventBufferLength,
            EventBuffer );
        break;

    case WDI_INDICATION_CAN_SUSTAIN_AP:
        OnCanSustainAPReceived(
            EventBufferLength,
            EventBuffer );
        break;

    case WDI_INDICATION_IHV_TASK_REQUEST:
        OnIhvTaskRequestReceived(
            EventBufferLength, 
            EventBuffer );
        break;
    }

}

NDIS_STATUS CPort::WaitForPendingJobCompletion(
    _In_ IOperationCompletionCallback * completionCallback)
/*++
    If there are no jobs queued by the port then return success.
    otherwise set m_JobsCompleteCallback and return pending
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    //if there are outstanding jobs queued then set the callback and return pending
    if (m_QueuedJobCount != 0)
    {
        m_JobsCompleteCallback = completionCallback;
        ndisStatus = NDIS_STATUS_PENDING;
    }

    WFCPortInfo("There are %u queued jobs.", m_QueuedJobCount);

    TraceExit(ndisStatus);

    return ndisStatus;
}

void CPort::OnOperationCompleted(
      _In_ NDIS_STATUS OperationStatus,
      _In_opt_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);

    EventQueue *eventQueue;
    IOperationCompletionCallback *jobsCompleteCallback = NULL;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    NT_ASSERTMSG("m_QueuedJobCount should be non-zero in this path.", m_QueuedJobCount);

    m_QueuedJobCount--;

    if ( SenderObject == m_pRoamJob )
    {
        NT_ASSERT(m_pRoamJob != NULL);

        WFCPortInfo(
            "[Roam][BetterAP] Better Roam job completed with status %x. Cleaning up.",
            OperationStatus );

        delete m_pRoamJob;
        m_pRoamJob = NULL;

        // If we have a pending IHV request roam that we havent honored
        // yet, requeue this job
        // delete port job clears this property to prevent requeue
        if (m_PortPropertyCache.GetPropertyBooleanOrDefault(
                WfcPortPropertyIhvRoamRequested,
                FALSE
                ))
        {
            NDIS_STATUS requeueStatus;
            PULONGLONG  pRequestTime;
            ULONG       timeFieldSize;
            ULONG       uRoamWabiReason;

            // Check how long it has been
            requeueStatus = m_PortPropertyCache.GetPropertyBuffer(
                WfcPortPropertyTimeOfIhvRoamRequest,
                &timeFieldSize, (BYTE **)&pRequestTime);
            if ((requeueStatus == NDIS_STATUS_SUCCESS) && (timeFieldSize == sizeof(ULONGLONG)))
            {
                if (CSystem::get_CurrentTime() < (*pRequestTime + WFC_CONSTANT::IhvRoamRequestTimeout))
                {                            
                    WFCPortInfo("[Roam][IHV][BetterAP] Requeue Roam request for pending IHV roaming needed indication");

                    requeueStatus = m_PortPropertyCache.GetPropertyULong(
                        WfcPortPropertyIhvRoamReason,
                        &uRoamWabiReason);
                    if (requeueStatus != NDIS_STATUS_SUCCESS ||
                        uRoamWabiReason == 0xFFFF)
                    {
                        WFCPortError("[Roam][IHV][BetterAP] Failed to get roam reason property cache. Error = 0x%08x. Setting WDI_ASSOC_STATUS_ROAMING_BETTER_AP_FOUND as reason", requeueStatus);
                        uRoamWabiReason = (ULONG)WDI_ASSOC_STATUS_ROAMING_BETTER_AP_FOUND;
                        // Continue
                    }

                    /* Set this re-queued roam job to have PASSIVE_IRQL. This is because, when there LE indicates ROAMING_NEEDED at dispatch,
                    and a reset happens, we fail the request. However, this function sees the request and sends the roam task at dispatch, causing
                    the DPC watchdog timer to to be hit. Hence we set this roam job to be at passive IRQL*/
                    requeueStatus = StartRoamJob(
                        &m_pRoamJob,
                        WfcRoamConnectTriggerToFindBetterAP_Critical,
                        (WDI_ASSOC_STATUS)uRoamWabiReason,
                        CConnectHelpers::GetRoamConfig(WfcRoamConnectTriggerToFindBetterAP_Critical),
                        WdfExecutionLevelPassive
                        );
                    if ( requeueStatus != NDIS_STATUS_SUCCESS )
                    {
                        WFCPortError("[Roam][IHV][BetterAP] Failed to requeue Roam request for pending IHV roaming needed indication. Error = 0x%08x", requeueStatus);
                    }
                }
                else
                {
                    WFCPortInfo("[Roam][IHV][BetterAP] Not requeuing IHV Roam request as it has expired");
                }
            }
            else
            {
                WFCPortError("[Roam][IHV][BetterAP] Failed to obtain IHV roam request time. Error = 0x%08x, Size = %u", requeueStatus, timeFieldSize);
            }
        }
    }
    else if ( SenderObject == m_pReconnectJob )
    {
        NT_ASSERT(m_pReconnectJob != NULL);

        WFCPortInfo(
            "Reconnect job completed with status %x. Cleaning up.",
            OperationStatus );

        delete m_pReconnectJob;
        m_pReconnectJob = NULL;
    }
    else if ( SenderObject == m_pDisconnectJob )
    {
        NT_ASSERT(m_pDisconnectJob != NULL);

        WFCPortInfo(
            "Disconnect job completed with status %x. Cleaning up.",
            OperationStatus );

        delete m_pDisconnectJob;
        m_pDisconnectJob = NULL;
    }
    else if ( SenderObject == m_pAssociationParametersRequestResponderJob)
    {
        NT_ASSERT(m_pAssociationParametersRequestResponderJob != NULL);


        WFCPortInfo(
            "Association parameters responder job completed with status %x. Cleaning up.",
            OperationStatus );

        delete m_pAssociationParametersRequestResponderJob;
        m_pAssociationParametersRequestResponderJob = NULL;
    }
    else
    {
        // Only jobs can be sending these. One job is the CIhvTaskRequestJob
        CJobBase *pJobBase = (CJobBase *)SenderObject;
        
        delete pJobBase;
    }

    //is there a job waiting on the port's jobs to be completed?
    if (m_QueuedJobCount == 0 && m_JobsCompleteCallback)
    {
        jobsCompleteCallback = m_JobsCompleteCallback;
        m_JobsCompleteCallback = NULL;

        eventQueue = m_pAdapter->GetEventQueue();

        WFCPortInfo("calling DeferOperationCompletion for JobsCompleteCallback (%p)", jobsCompleteCallback);

        ndisStatus = eventQueue->DeferOperationCompletion(jobsCompleteCallback,
                                                          NDIS_STATUS_SUCCESS,
                                                          this,
                                                          &m_JobsCompleteCallbackEvent);

        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCPortError("[Port] eventQueue-DeferOperationCompletion(jobsCompleteCallback) failed %u", ndisStatus);
        }

    }

    TraceExit(ndisStatus);
}

void CPort::IndicateSinglePeerDisassociated(
    _In_ CConnectedPeer* pPeer,
    _In_ UINT16 ReasonCode,
    _In_ WDI_ASSOC_STATUS WabiAssocStatus )
{
    DOT11_DISASSOCIATION_PARAMETERS dot11DisassocParams;

    ASSIGN_NDIS_OBJECT_HEADER(
        dot11DisassocParams.Header, 
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_DISASSOCIATION_PARAMETERS_REVISION_1,
        sizeof(dot11DisassocParams) );
        
    RtlCopyMemory(
        &dot11DisassocParams.MacAddr,
        pPeer->GetMacAddress(),
        sizeof(DOT11_MAC_ADDRESS));
    
    dot11DisassocParams.uReason = CWabiToDot11Converter::MapAssocStatus(
        WabiAssocStatus,
        ReasonCode );

    WFCPortInfo("[Connect] Indicating disassociation to NDIS with status 0x%08x", dot11DisassocParams.uReason);
    
    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_DOT11_DISASSOCIATION,
        NULL,
        &dot11DisassocParams,
        sizeof(dot11DisassocParams) );
}

void CPort::IndicateAllPeersDisassociated(
    _In_ UINT16 ReasonCode,
    _In_ WDI_ASSOC_STATUS WabiAssocStatus )
{
    PEERLIST_ENUM_CONTEXT enumPeersContext;
    CConnectedPeer* pCurrentPeer = m_pPeerList->GetFirst(
        &enumPeersContext );
    ULONG peerCount = 0;

    TraceEntry();

    while ( pCurrentPeer != NULL )
    {
        this->IndicateSinglePeerDisassociated(
            pCurrentPeer,
            ReasonCode,
            WabiAssocStatus );

        pCurrentPeer = m_pPeerList->GetNext(&enumPeersContext);
        peerCount++;
    }

    WFCPortInfo(
        "Disassociated %d peers due to broadcast disassociated indication. (Reason 0x%x)",
        peerCount,
        WabiAssocStatus );

    TraceExit(NDIS_STATUS_SUCCESS);
}

void CPort::OnDisassociated(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_INDICATION_DISASSOCIATION_PARAMETERS disassocParameters;
    ULONG opMode;
    CConnectedPeer *pPeer = NULL;
    WDI_MAC_ADDRESS broadcastMac = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    UINT16 reasonCode = 0;
    
    TraceEntry();

    WFCTrace("[LinkChangeTime]Disassociation");

    status = ParseWdiIndicationDisassociation(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &disassocParameters );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't parse incoming disassociation indication. Error = 0x%08x", status);
        goto exit;
    }

    WFCPortInfo("[Connect][Link] Got disassociated notification for %!MACADDR!", 
        disassocParameters.DisconnectIndicationParameters.MacAddress.Address);

    if (disassocParameters.DisassociationFrame.ElementCount >= sizeof(UINT16))
    {
        reasonCode = *((UINT16 *)disassocParameters.DisassociationFrame.pElements);
    }
    else if (disassocParameters.DeauthFrame.ElementCount >= sizeof(UINT16))
    {
        reasonCode = *((UINT16 *)disassocParameters.DeauthFrame.pElements);
    }

    if (m_pRoamTraceLoggingData != NULL)
    {
        m_pRoamTraceLoggingData->disassociationIndicationTime = CSystem::get_CurrentTime();
    }

    status = m_PortPropertyCache.GetPropertyULong(
        WfcPortCurrentOperationalMode,
        &opMode );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        NT_ASSERT(FALSE);
        opMode = WDI_OPERATION_MODE_STA;
    }

    if ( ( opMode & WDI_OPERATION_MODE_P2P_GO ) != 0 &&
         RtlEqualMemory(
             &broadcastMac,
             disassocParameters.DisconnectIndicationParameters.MacAddress.Address,
             sizeof(broadcastMac) ) )
    {
        WFCPortInfo("Received broadcast disassociation indication from GO. Indicating disassociated for all peer objects.");

        IndicateAllPeersDisassociated(
            reasonCode,
            disassocParameters.DisconnectIndicationParameters.DisassociationWABIReason );

        m_pPeerList->FlushPeerList();
    }
    else
    {
        // Find this peer
        pPeer = m_pPeerList->FindPeerByAddress(disassocParameters.DisconnectIndicationParameters.MacAddress.Address);
        if (pPeer) 
        {
            // First mark the AP as something we have failed association with
            NT_ASSERT(pPeer->GetAssociationState() == dot11_assoc_state_auth_assoc);
            
            WFC_PORT_TYPE scanCacheSource = GetPortType() == WfcPortTypeWFDRole ? WfcPortTypeWFDDevice : WfcPortTypeExtSTA;
            CBSSEntry* pFailedBssEntry = m_pAdapter->GetBSSListManager(
                scanCacheSource )->FindBSSEntry(
                    (disassocParameters.DisconnectIndicationParameters.MacAddress.Address));

            if ( pFailedBssEntry != NULL )
            {
                // If we lost the AP, then we also mark it as not visible
                if (CConnectHelpers::IsPeerNotReachable(disassocParameters.DisconnectIndicationParameters.DisassociationWABIReason))
                {
                    // We could use just blocking logic here to cause this AP to be
                    // excluded, but we specifically dont want to have this AP 
                    // in our selection list, so keeping it separate rather than
                    // check this state in two places
                    pFailedBssEntry->SetVisibility(false);

                    WFCPortTrace("[BlockedList] Blocking BSSID = %!MACADDR! due to bss unreachable", 
                        (disassocParameters.DisconnectIndicationParameters.MacAddress.Address));

                    // Mark this AP as something we havent been able to hear from
                    pFailedBssEntry->Block(WFC_BLOCKED_REASON_ASSOC_OR_DISASSOC_BSS_UNREACHABLE);
                }
                else if (CConnectHelpers::IsFailedByPeerAction(disassocParameters.DisconnectIndicationParameters.DisassociationWABIReason))
                {
                    // If disassociated because of something the AP sent us, blocked this AP so that 
                    // we will rank it lower
                    WFCPortTrace("[BlockedList] Blocking BSSID = %!MACADDR! due to recent disassociation",
                        (disassocParameters.DisconnectIndicationParameters.MacAddress.Address));
                    pFailedBssEntry->Block(WFC_BLOCKED_REASON_ASSOC_REJECTED_OR_PEER_DISCONNECTED);                    
                }
                
                // Clear the associated flag
                pFailedBssEntry->SetAssociatedFlag(false);
            }

            if (GetDot11State() == WfcPortDot11StateOp)
            {
                // If we are supposed to be in Op State, we need to tell the OS about
                // this disassociation (else the job that triggered the disassociation would do it)

                this->IndicateSinglePeerDisassociated(
                    pPeer,
                    reasonCode,
                    disassocParameters.DisconnectIndicationParameters.DisassociationWABIReason );
            }
        }            
    }

    // Reset relevant state about disassociation
    if ((opMode & 
         (WDI_OPERATION_MODE_STA |
          WDI_OPERATION_MODE_P2P_CLIENT)) != 0)
    {
        // Key is no longer configured
        WFCPortTrace("Resetting key configured state to FALSE.");        
        m_KeyConfigured = FALSE;            
    }

    //reset FIPS mode - queried on first Rx packet of next association
    m_FIPSMode = WDI_FIPS_MODE_UNKNOWN;

    m_pPeerList->DeletePeer(disassocParameters.DisconnectIndicationParameters.MacAddress.Address);

    //Cancel opportunistic roam timer (if running)
    CancelOpportunisticRoam();

    // If someone is registered, notify them about the disassociation
    if (m_pDisassociationCompleteCallback)
    {
        m_pDisassociationCompleteCallback->OnDisassociationProcessed();
    }

    if ((GetDot11State() == WfcPortDot11StateOp) && 
        ((opMode &   
                (WDI_OPERATION_MODE_STA |
                 WDI_OPERATION_MODE_P2P_CLIENT)) != 0))
    {
        if (((opMode & WDI_OPERATION_MODE_P2P_CLIENT) != 0) &&
            CConnectHelpers::IsFailedByPeerAction(disassocParameters.DisconnectIndicationParameters.DisassociationWABIReason))
        {
            // We are in a WFD Client mode dont trigger reconnect if peer disconnect us
            WFCPortTrace("[Roam] Not triggering reconnect as disassociated by Peer Wi-Fi Direct GO %!MACADDR!", 
                disassocParameters.DisconnectIndicationParameters.MacAddress.Address);
        }
        else
        {
            // If we are in a station mode and are expected to be connected or are P2P Client
            // and not actively disconnected by GO, trigger reconnect
            WFCPortTrace("[Roam] Trigger reconnect as disassociated from AP %!MACADDR!", 
                disassocParameters.DisconnectIndicationParameters.MacAddress.Address);
                
            // Trigger reconnect
            (VOID)TriggerReconnect(disassocParameters.DisconnectIndicationParameters.DisassociationWABIReason,
                                   WfcRoamConnectTriggerAssociationLoss,
                                   CConnectHelpers::GetRoamConfig(WfcRoamConnectTriggerAssociationLoss));
        }
    }

exit:
    TraceExit(NDIS_STATUS_SUCCESS);
}

void CPort::OnTaskOffloadCurrentConfigReceived(_In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_INDICATION_TASK_OFFLOAD_CURRENT_CONFIG_PARAMETERS offloadParameters;
    NDIS_OFFLOAD offloadAttributes;

    status = ParseWdiIndicationTaskOffloadCurrentConfig(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &offloadParameters
        );

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Failed to parse offload config (0x%x)", status);
        goto Exit;
    }

    WFC_ASSIGN_NDIS_OBJECT_HEADER(offloadAttributes.Header,
        NDIS_OBJECT_TYPE_OFFLOAD,
        NDIS_OFFLOAD_REVISION_3,
        NDIS_SIZEOF_NDIS_OFFLOAD_REVISION_3);

    CWabiToDot11Converter::MapTcpOffloadCapabilitiesToNdisOffload(
        offloadParameters.TcpOffloadCapabilities, &offloadAttributes);

    GetPropertyCache()->SetTCPOffloadParametersPortPropertyCache(
        &offloadParameters.TcpOffloadCapabilities);

    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_TASK_OFFLOAD_CURRENT_CONFIG,
        NULL,
        &offloadAttributes,
        sizeof(offloadAttributes)
        ); 
    
Exit:
    TraceExit(status);
}

void CPort::UpdateTaskOffloadCurrentConfigForFIPS(
    _In_ bool bEnable
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_OFFLOAD offloadAttributes;
    WDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER TcpOffloadCapabilities;
//    NDIS_OFFLOAD_ENCAPSULATION encapsulationOffloadParameters;

    WFC_ASSIGN_NDIS_OBJECT_HEADER(offloadAttributes.Header,
        NDIS_OBJECT_TYPE_OFFLOAD,
        NDIS_OFFLOAD_REVISION_3,
        NDIS_SIZEOF_NDIS_OFFLOAD_REVISION_3);

    status = GetPropertyCache()->GetTCPOffloadParametersFromPortPropertyCache(
        &TcpOffloadCapabilities);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Couldn't retrieve Offload Parameters from property cache or TCP offload capabilities wasnt reported by adapter");
        goto Exit;
    }

    //If FIPS is enabled, disable task offloads; else report the original adapter property cache values
    if (bEnable)
    {
        PopulateTaskOffloadDisabledParameters(&TcpOffloadCapabilities);
    }

    CWabiToDot11Converter::MapTcpOffloadCapabilitiesToNdisOffload(
        TcpOffloadCapabilities, &offloadAttributes);

    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_TASK_OFFLOAD_HARDWARE_CAPABILITIES,
        NULL,
        &offloadAttributes,
        sizeof(offloadAttributes)
        );

Exit:
    TraceExit(status);
}

void CPort::PopulateTaskOffloadDisabledParameters(
    _Inout_ PWDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER pTcpOffloadCapabilities
    )
{
    pTcpOffloadCapabilities->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.Encapsulation = 0; // NDIS_ENCAPSULATION_NOT_SUPPORTED
    pTcpOffloadCapabilities->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.IpOptionsSupported = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.TcpOptionsSupported = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.TcpChecksum = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.UdpChecksum = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.IpChecksum = NDIS_OFFLOAD_NOT_SUPPORTED;

    pTcpOffloadCapabilities->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.Encapsulation = 0; // NDIS_ENCAPSULATION_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.IpOptionsSupported = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.TcpOptionsSupported = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.TcpChecksum = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.UdpChecksum = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.IpChecksum = NDIS_OFFLOAD_NOT_SUPPORTED;

    pTcpOffloadCapabilities->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.Encapsulation = 0; // NDIS_ENCAPSULATION_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.IpExtensionHeadersSupported = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.TcpOptionsSupported = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.TcpChecksum = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.UdpChecksum = NDIS_OFFLOAD_NOT_SUPPORTED;

    pTcpOffloadCapabilities->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.Encapsulation = 0; // NDIS_ENCAPSULATION_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.IpExtensionHeadersSupported = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.TcpOptionsSupported = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.TcpChecksum = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.UdpChecksum = NDIS_OFFLOAD_NOT_SUPPORTED;

    pTcpOffloadCapabilities->LsoV1Capabilities.Encapsulation = 0; // NDIS_ENCAPSULATION_NOT_SUPPORTED;

    pTcpOffloadCapabilities->LsoV1Capabilities.TcpOptions = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->LsoV1Capabilities.IpOptions = NDIS_OFFLOAD_NOT_SUPPORTED;

    pTcpOffloadCapabilities->LsoV2Capabilities.Ipv4LSO.EncapsulationV4 = 0; // NDIS_ENCAPSULATION_NOT_SUPPORTED;

    pTcpOffloadCapabilities->LsoV2Capabilities.Ipv6LSO.EncapsulationV6 = 0; // NDIS_ENCAPSULATION_NOT_SUPPORTED;
    pTcpOffloadCapabilities->LsoV2Capabilities.Ipv6LSO.IpExtensionHeadersSupported = NDIS_OFFLOAD_NOT_SUPPORTED;
    pTcpOffloadCapabilities->LsoV2Capabilities.Ipv6LSO.TcpOptionsSupported = NDIS_OFFLOAD_NOT_SUPPORTED;

    pTcpOffloadCapabilities->ReceiveOffloadCapabilities.Ipv4Enabled = FALSE;
    pTcpOffloadCapabilities->ReceiveOffloadCapabilities.Ipv6Enabled = FALSE;
}

void CPort::OnLinkStateChange(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_STATUS propertyCacheStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    WDI_INDICATION_LINK_STATE_CHANGE_PARAMETERS LinkStateParams;

    status = ParseWdiIndicationLinkStateChange(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &LinkStateParams);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't parse link state change parameters.");
        goto exit;
    }

    //
    // TODO: For AP - Add MacAddress to WABI Link State Change & parse
    // appropriately.
    //

    ULONG oldLinkSpeed;
    bool LinkSpeedChanged = false;

    propertyCacheStatus = m_PortPropertyCache.GetPropertyULong(
        (WFC_PROPERTY_NAME)WfcPortPropertyNameTxLinkSpeed,
        &oldLinkSpeed );

    if ( propertyCacheStatus != NDIS_STATUS_SUCCESS || LinkStateParams.LinkStateChangeParameters.TxLinkSpeed != oldLinkSpeed)
    {
        WFCPortInfo("Tx Link speed updated to 0x%x", LinkStateParams.LinkStateChangeParameters.TxLinkSpeed);

        m_PortPropertyCache.SetPropertyULong(
            (WFC_PROPERTY_NAME)WfcPortPropertyNameTxLinkSpeed,
            LinkStateParams.LinkStateChangeParameters.TxLinkSpeed);
        LinkSpeedChanged = true;        
    }
    
    propertyCacheStatus = m_PortPropertyCache.GetPropertyULong(
        (WFC_PROPERTY_NAME)WfcPortPropertyNameRxLinkSpeed,
        &oldLinkSpeed);

    if ( propertyCacheStatus != NDIS_STATUS_SUCCESS || LinkStateParams.LinkStateChangeParameters.RxLinkSpeed != oldLinkSpeed)
    {
        WFCPortInfo("Rx Link speed updated to 0x%x", LinkStateParams.LinkStateChangeParameters.RxLinkSpeed);

        m_PortPropertyCache.SetPropertyULong(
            (WFC_PROPERTY_NAME)WfcPortPropertyNameRxLinkSpeed,
            LinkStateParams.LinkStateChangeParameters.RxLinkSpeed);
        LinkSpeedChanged = true;        
    }

    if (LinkSpeedChanged)
    {
        UpdateLinkState();
    }

    CConnectedPeer *pPeer = NULL;
    if ( !ETH_IS_BROADCAST(LinkStateParams.LinkStateChangeParameters.PeerMACAddress.Address) )
    {
        // Find the particular peer
        pPeer = m_pPeerList->FindPeerByAddress(LinkStateParams.LinkStateChangeParameters.PeerMACAddress.Address);
    }
    else
    {
        // Since we don't have MAC address in indication, go with the associated peer
        pPeer = m_pPeerList->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);
    }
    
    if (pPeer != NULL)
    {
        CBSSEntry* pBssEntry = GetBSSListManager()->FindBSSEntry(
                *pPeer->GetMacAddress());

        if (pBssEntry)
        {
            // Update the link quality field in the BSS table so that when we are ranking, etc
            // this AP is ranked properly and the AP is marked still available
            pBssEntry->OnLinkQualityUpdate(LinkStateParams.LinkStateChangeParameters.LinkQuality);
        }

        // Check if we need to act on the link quality
        UCHAR oldLinkQuality;

        propertyCacheStatus = m_PortPropertyCache.GetPropertyUchar(
            (WFC_PROPERTY_NAME)WfcPortPropertyNameLinkQuality,
            &oldLinkQuality );

        if ( propertyCacheStatus != NDIS_STATUS_SUCCESS || LinkStateParams.LinkStateChangeParameters.LinkQuality != oldLinkQuality)
        {
            WFCPortInfo("Link quality updated to 0x%x", LinkStateParams.LinkStateChangeParameters.LinkQuality);
            
            m_PortPropertyCache.SetPropertyUchar(
                (WFC_PROPERTY_NAME)WfcPortPropertyNameLinkQuality,
                LinkStateParams.LinkStateChangeParameters.LinkQuality);

            pPeer->SetLinkQuality(LinkStateParams.LinkStateChangeParameters.LinkQuality);
            UpdateLinkQuality(pPeer);
        
            if (DetermineIfSwitchToBetterAPNeeded(pPeer))
            {
                WFCPortInfo("[Roam][BetterAP][Link] Attempting better AP roam due to low link quality %d", 
                    LinkStateParams.LinkStateChangeParameters.LinkQuality);

                status = TriggerWdiInitiatedRoam(
                    WfcRoamConnectTriggerToFindBetterAP_BestEffort,
                    WDI_ASSOC_STATUS_ROAMING_BETTER_AP_FOUND,
                    CConnectHelpers::GetRoamConfig(WfcRoamConnectTriggerToFindBetterAP_BestEffort));
                if (status != NDIS_STATUS_SUCCESS)
                {
                    WFCPortError("Couldn't initiate roaming attempt");
                    status = NDIS_STATUS_SUCCESS;
                }
            }            
        }                
    }
    
exit:
    TraceExit(status);
}

void CPort::OnFTAssocParamsNeededIndicationArrived(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_FT_AUTHENTICATION_RESPONSE_PARAMETERS pFastTransitionAuthResponseParameters;
    WDI_INDICATION_FT_ASSOC_PARAMS_NEEDED_PARAMETERS FTAssocParameters;
    BYTE* pb;
    ULONG toAlloc = sizeof(DOT11_FT_AUTHENTICATION_RESPONSE_PARAMETERS);
    TraceEntry();

    ndisStatus = ParseWdiIndicationFtAssocParamsNeeded(EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext(),
        &FTAssocParameters);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Couldn't parse FT association parameters (WDI_INDICATION_FT_ASSOC_PARAMS_NEEDED). Error: 0x%08x", ndisStatus);
        goto exit;
    }
    toAlloc += FTAssocParameters.AuthResponse.ElementCount;
    toAlloc += FTAssocParameters.AuthRequest.ElementCount;
    pb = new BYTE[toAlloc];
    if (pb == NULL)
    {
        WFCPortError("Failed to allocate memory for Authentication response parameters.");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    RtlZeroMemory(pb, toAlloc);

    pFastTransitionAuthResponseParameters = (PDOT11_FT_AUTHENTICATION_RESPONSE_PARAMETERS)pb;

    pFastTransitionAuthResponseParameters->Header.Revision = DOT11_FT_AUTHENTICATION_RESPONSE_PARAMETERS_REVISION_1;
    pFastTransitionAuthResponseParameters->Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    pFastTransitionAuthResponseParameters->Header.Size = SIZEOF_DOT11_FT_AUTHENTICATION_RESPONSE_PARAMETERS_REVISION_1;

    pFastTransitionAuthResponseParameters->uAuthenticationReqOffset = sizeof(DOT11_FT_AUTHENTICATION_RESPONSE_PARAMETERS);
    pFastTransitionAuthResponseParameters->uAuthenticationReqSize = FTAssocParameters.AuthRequest.ElementCount;

    pFastTransitionAuthResponseParameters->uAuthenticationRespOffset = sizeof(DOT11_FT_AUTHENTICATION_RESPONSE_PARAMETERS)
        +pFastTransitionAuthResponseParameters->uAuthenticationReqSize;
    pFastTransitionAuthResponseParameters->uAuthenticationRespSize = FTAssocParameters.AuthResponse.ElementCount;

    RtlCopyMemory(
        ((PUCHAR)pFastTransitionAuthResponseParameters + pFastTransitionAuthResponseParameters->uAuthenticationReqOffset),
        FTAssocParameters.AuthRequest.pElements,
        pFastTransitionAuthResponseParameters->uAuthenticationReqSize
        );

    RtlCopyMemory(
        ((PUCHAR)pFastTransitionAuthResponseParameters + pFastTransitionAuthResponseParameters->uAuthenticationRespOffset),
        FTAssocParameters.AuthResponse.pElements,
        pFastTransitionAuthResponseParameters->uAuthenticationRespSize
        );

    BYTE* SNonce = NULL;
    ULONG SNonceSize = 0;
    ndisStatus = m_PortPropertyCache.GetPropertyBuffer((WFC_PROPERTY_NAME)WfcPortPropertySNonce, &SNonceSize,
        &SNonce);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Error: Unable to retrieve SNonce from property cache. Status = 0x%08x", ndisStatus);
        goto exit;
    }
    
    if (SNonceSize == 0 || SNonce == NULL)
    {
        WFCError("Error: SNonce is NULL.");
        goto exit;
    }
    
    RtlCopyMemory(
            pFastTransitionAuthResponseParameters->SNonce,
            SNonce,
            SIZEOF_NONCE);

    RtlCopyMemory(
        pFastTransitionAuthResponseParameters->MacAddr,
        FTAssocParameters.BssId.Address,
        sizeof(WDI_MAC_ADDRESS));

    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_DOT11_FT_TRANSITION_PARAMETERS_NEEDED,
        NULL,
        (PVOID)(pFastTransitionAuthResponseParameters),
        toAlloc);

exit:
    TraceExit(ndisStatus);
}

void CPort::OnRoamingNeeded(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    WDI_INDICATION_ROAMING_NEEDED_PARAMETERS roamingNeededParameters;

    TraceEntry();

    status = ParseWdiIndicationRoamingNeeded(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &roamingNeededParameters
        );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't parse roaming needed parameters. 0x%x", status);
        goto exit;
    }

    WFCPortTrace("[Roam][Link] Trigger roam as roaming needed indication received");
    TriggerIHVRequestedRoam(
        WfcRoamConnectTriggerToFindBetterAP_Critical,
        roamingNeededParameters.RoamingReason,
        CConnectHelpers::GetRoamConfig(WfcRoamConnectTriggerToFindBetterAP_Critical));

exit:
    TraceExit(status);
}

void CPort::OnIhvEvent(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_INDICATION_IHV_EVENT_PARAMETERS indicationData;
    BYTE* pIhvData = NULL;
    UINT ihvDataLength = 0;
    bool bNeedsFree = false;
    
    TraceEntry();

    // We will copy whatever data the IHV filled in
    
    status = ParseWdiIndicationIhvEvent(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        ((PUCHAR)EventBuffer) + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &indicationData
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("[IHVExt] Failed to parse NIC specific extension event TLVs. Error = 0x%08x", status);
        goto exit;
    }

    if (indicationData.Optional.IHVData_IsPresent)
    {
        // If the total data is greater than 64K, it would be
        // mapped into more than one TLV
        status = CWabiToDot11Converter::CombineArrayOfByteArrays(
            &indicationData.IHVData,            
            &pIhvData,
            &ihvDataLength,
            &bNeedsFree
            );
            
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCError("[IHVExt] Failed to merge NIC specific extension event TLVs into single buffer. Error = 0x%08x", status);
            goto exit;
        }
    }

    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_MEDIA_SPECIFIC_INDICATION,
        NULL,
        pIhvData,
        ihvDataLength );

exit:        
    if (bNeedsFree)
    {
        delete[] pIhvData;
    }
    TraceExit(status);
}

bool CPort::DetermineIfSwitchToBetterAPNeeded(
    _In_ CConnectedPeer* pPeer
    )
{
    bool bAttemptRoam = false;
    PROAM_CONTROL_PARAMETERS pRoamControl = CConnectHelpers::GetRoamControlParameters(
        m_pAdapter->GetAdapterPropertyCache());
    ULONGLONG currentTime;
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    BOOLEAN isEnterpriseOrMultipleAP = FALSE;
    PPEER_ROAM_HISTORY pCurrentAPHistory = pPeer->GetRoamHistory();
    UCHAR currentLinkQuality = pPeer->GetLinkQuality();
    //
    // Step 1 - Determine if we need to find a better AP
    //

    // Check if this is P2P. For P2P we wont trigger better AP roaming
    if (GetPortType() != WfcPortTypeExtSTA)
    {
        WFCPortTrace("[Roam][BetterAP] Not roaming as this is a P2P connection");
        goto exit;
    }
    
    // If new link quality is high enough, then dont attempt to roam
    if (currentLinkQuality >= pRoamControl->BetterAPRoamLinkQualityThreshold)
    {
        WFCPortTrace("[Roam][BetterAP] Not roaming as Link quality of network not low enough (%d)", currentLinkQuality);
        goto exit;
    }

    // Determine if this is a WPA/WPA2 Enterprise network or not
    if (!pRoamControl->BetterAPRoamOnNonEnterpriseNetworks)
    {
        ULONG bufferSize;
        UINT16 authAlgoCount;
        DOT11_AUTH_ALGORITHM* pAuthAlgos;
        status = m_PortPropertyCache.GetPropertyList(
            WfcPortPropertyAuthAlgoList, 
            &bufferSize,
            &authAlgoCount,
            (BYTE **)&pAuthAlgos);

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCPortError("Couldn't recall auth algorithm list.");
            goto exit;
        }

        for( UINT16 i = 0; i < authAlgoCount; ++i )
        {
            if ( (pAuthAlgos[i] == DOT11_AUTH_ALGO_RSNA ) ||
                  (pAuthAlgos[i] == DOT11_AUTH_ALGO_WPA))
            {
                isEnterpriseOrMultipleAP = TRUE;
                break;
            }
        }

        if (!isEnterpriseOrMultipleAP)
        {
            // Not enterprise security. Check if we have seen multiple APs in this network
            status = m_PortPropertyCache.GetPropertyBoolean(
                WfcPortPropertyMultipleBSSIDNetwork,
                &isEnterpriseOrMultipleAP );            
            if ((status != NDIS_STATUS_SUCCESS) || (!isEnterpriseOrMultipleAP))
            {                
                WFCPortTrace("[Roam][BetterAP] Not roaming as this is not an enterprise or multiple BSSID network");
                goto exit;
            }
        }
    }

    //
    // Step 2 - Throttle roaming too often
    //
    currentTime = CSystem::get_CurrentTime();

    // Determine if the link quality has dropped significantly since
    // last time we roamed (or reached high). If it hasn't, then we do additional checks
    if ((currentLinkQuality < pCurrentAPHistory->LinkQualityHistory) &&         // Has dropped
        ((currentLinkQuality + pRoamControl->APLinkQualitySignificantChangeBar)
            > pCurrentAPHistory->LinkQualityHistory))                           // But not by much
    {
        // Not changed much since we last attempted (or associated). Determine if we should roam
        WFCPortTrace("[Roam][BetterAP] Validating roaming frequency as Link quality of network not significantly changed (Current: %d Old: %d)",             
            currentLinkQuality, pCurrentAPHistory->LinkQualityHistory);

        // If we just recently attempted roamed (or just associated), then dont roam
        if (currentTime < (pCurrentAPHistory->RoamAttemptTime + 
                ((ULONGLONG)pRoamControl->BetterAPRoamReattemptTimeGapSeconds) * 10000000))
        {
            WFCPortTrace("[Roam][BetterAP] Not roaming this quickly after last association/roam");
            goto exit;
        }
        
        // If we have tried roaming too often then dont retry
        if (pCurrentAPHistory->RoamAttemptCount > pRoamControl->BetterAPRoamReattemptCount)
        {
            WFCPortTrace("[Roam][BetterAP] Not roaming as Link quality has not changed significantly since last check");
            goto exit;
        }
    }

    //
    // Step 3 - Save state to help roaming later
    //

    // We are going to roam, remember state that we will use for next roam
    pCurrentAPHistory->RoamAttemptTime = currentTime;
    pCurrentAPHistory->RoamAttemptCount++;

    WFCPortTrace("[Roam][BetterAP] Will attempt roam from  %!MACADDR! of LinkQuality %d (count: %d)", 
        *(pPeer->GetMacAddress()),
        currentLinkQuality,
        pCurrentAPHistory->RoamAttemptCount);
        
    bAttemptRoam = true;
    
exit:
    if (!bAttemptRoam)
    {
        // Save the maximum link quality reported by the device
        if (pCurrentAPHistory->LinkQualityHistory < currentLinkQuality)
        {    
            pCurrentAPHistory->LinkQualityHistory = currentLinkQuality;
        }
    }
    else
    {
        // Roaming. Save this as a the link quality history
        pCurrentAPHistory->LinkQualityHistory = currentLinkQuality;
    }
    
    return bAttemptRoam;    
}


NDIS_STATUS CPort::TriggerDisconnectAndReconnect(
    _In_ WDI_ASSOC_STATUS DisconnectWabiReason,
    _In_ UINT16 Disassociation80211ReasonCode
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CDisconnectJob *pNewDisconnectJob = NULL;

    TraceEntry();

    if ( GetDot11State() == WfcPortDot11StateInit)
    {
        WFCPortInfo("[Roam] Not currently connected, ignoring disconnect request notification.");
        goto exit;
    }

    if ( m_pDisconnectJob != NULL )
    {
        WFCPortInfo("[Roam] Already disconnecting [O:%p], will not attempt to disconnect again.", m_pDisconnectJob);
        goto exit;
    }
    WFCPortInfo("[Roam] Disconnect and Reconnect operation triggered");


    //
    // This function only returns errors after this point. Before here
    // this function can be a no-op, but now the function must handle
    // the disconnect request, and report failures if it can't.
    //

    pNewDisconnectJob = new CDisconnectJob(
        IActivityId::get_NextActivityId() );

    if ( pNewDisconnectJob == NULL )
    {
        status = NDIS_STATUS_RESOURCES;
        WFCPortError("Couldn't create new disconnect job.");
        goto exit;
    }

    status = pNewDisconnectJob->InitializeFromJob(
        m_pAdapter,
        m_NdisPortNumber,
        false,
        DisconnectWabiReason,
        Disassociation80211ReasonCode);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't initialize new disconnect job.");
        goto exit;
    }

    status = m_pAdapter->GetSerializedJobList()->QueueSerializedJob(
        pNewDisconnectJob,
        this );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't queue new disconnect job to job serializer.");
        goto exit;
    }

    m_QueuedJobCount++;
    
    m_pDisconnectJob = pNewDisconnectJob;
    pNewDisconnectJob = NULL;

exit:
    if ( pNewDisconnectJob != NULL )
    {
        delete pNewDisconnectJob;
    }

    TraceExit(status);
    return status;
}

NDIS_STATUS CPort::TriggerReconnect(
    _In_ WDI_ASSOC_STATUS RoamWabiReason,
    _In_ WFC_ROAM_CONNECT_TRIGGER RoamTrigger,
    _In_ WDI_ROAM_CONFIGURATION_FLAGS RoamConfigFlags)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceEntry();

    if ( m_pReconnectJob != NULL )
    {
        WFCPortInfo("[Roam] Already reconnecting [O:%p], will not attempt to reconnect again.", m_pReconnectJob);
        goto exit;
    }
    WFCPortInfo("[Roam] Triggering Roam for Reconnect");

    status = StartRoamJob(
        &m_pReconnectJob,
        RoamTrigger,
        RoamWabiReason,
        RoamConfigFlags);
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't initialize new roaming job.");
    }

exit:    
    TraceExit(status);
    return status;
}

NDIS_STATUS CPort::TriggerWdiInitiatedRoam(
    _In_ WFC_ROAM_CONNECT_TRIGGER RoamTrigger,
    _In_ WDI_ASSOC_STATUS RoamWabiReason,
    _In_ WDI_ROAM_CONFIGURATION_FLAGS RoamConfigFlags)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    
    TraceEntry();

    if ( (m_pRoamJob != NULL) || ( m_pReconnectJob != NULL ) || ( m_pDisconnectJob != NULL ))
    {
        WFCPortInfo("[Roam][BetterAP] Already roaming/reconnecting/disconnecting, will not attempt to Best Effort roam again.");
        status = NDIS_STATUS_FAILURE;
        goto exit;
    }
    WFCPortInfo("[Roam][BetterAP]  Triggering Best Effort Roam");

    WFCTrace("[LinkChangeTime]LinkQuality Roam");

    status = StartRoamJob(
        &m_pRoamJob,
        RoamTrigger,
        RoamWabiReason,
        RoamConfigFlags);
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't initialize new roaming job.");
        goto exit;
    }

exit:        
    TraceExit(status);
    return status;
}

NDIS_STATUS CPort::TriggerIHVRequestedRoam(
    _In_ WFC_ROAM_CONNECT_TRIGGER RoamTrigger,
    _In_ WDI_ASSOC_STATUS RoamWabiReason,
    _In_ WDI_ROAM_CONFIGURATION_FLAGS RoamConfigFlags)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONGLONG currentTime;
    BOOLEAN throttleRoamingNeeded = FALSE;
    
    TraceEntry();

    // Check if we are throttling IHV requests. The throttling must be OFF by default
    // and when ON we will only allow one pending event and if there is one already,
    // ignore the new one
    throttleRoamingNeeded = m_pAdapter->GetAdapterPropertyCache()->GetPropertyBooleanOrDefault(
        WfcAdapterPropertyRegRoamThrottleRoamingNeeded,
        FALSE
        );
    if ((throttleRoamingNeeded) && (m_pRoamJob != NULL))
    {
        // Throttling is ON and we have a pending roam job, dont even set the flag or time
        WFCPortInfo("[Roam][IHV][BetterAP] Already roaming and IHV requested roam throttling ON. Will not attempt to Best Effort roam again.");
        goto exit;
    }

    // Save flag that an IHV roam is requested. The flag is set everytime
    status = GetPropertyCache()->SetPropertyBoolean(
        WfcPortPropertyIhvRoamRequested,
        TRUE
        );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("[Roam][IHV][BetterAP] Couldn't set flag that IHV roam has been requested. Error = 0x%08x", status);
        // Continue
    }

    // Save the time this request was queued so that we can timeout/debug
    currentTime = CSystem::get_CurrentTime();    
    status = m_PortPropertyCache.SetPropertyBuffer(
        WfcPortPropertyTimeOfIhvRoamRequest,
        sizeof(ULONGLONG),
        (BYTE *)&currentTime);
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("[Roam][IHV][BetterAP] Couldn't save time of IHV roam request. Error = 0x%08x", status);
        // Continue
    }    

    status = m_PortPropertyCache.SetPropertyULong(
        WfcPortPropertyIhvRoamReason,
        RoamWabiReason);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("[Roam][IHV][BetterAP] Failed to set roam reason property cache. Error = 0x%08x", status);
        // Continue
    }

    // Now decide if we will actually queue the roam or depend on an existing one
    if (m_pRoamJob != NULL)
    {
        WFCPortInfo("[Roam][IHV][BetterAP] Already roaming so deferring IHV requested Best Effort roam.");
        goto exit;
    }

    WFCPortInfo("[Roam][IHV][BetterAP] Triggering IHV requested Best Effort Roam");
    WFCTrace("[LinkChangeTime]IHV Roam Needed");

    status = StartRoamJob(
        &m_pRoamJob,
        RoamTrigger,
        RoamWabiReason,
        RoamConfigFlags);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("[Roam][IHV][BetterAP]Couldn't initialize new roaming job.");
        goto exit;
    }    

exit:        
    TraceExit(status);
    return status;
}

void CPort::QueueOpportunisticRoam()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    bool bMediaStreamingMode = false;
    ULONG opportunisticRoamDelayMs = WFC_CONSTANT::OpportunisticRoamDelayMs;

    TraceEntry();

    if (m_pAdapter->GetAdapterPropertyCache()->GetPropertyBooleanOrDefault(
            WfcAdapterPropertyRegDisableOpportunisticRoam,
            FALSE                                                         ))
    {
        WFCPortInfo("[Roam][Opportunistic]Opportunistic Roam disabled.");
        ndisStatus = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    if (GetPortType() != WfcPortTypeExtSTA)
    {
        WFCPortInfo("[Roam][Opportunistic]Not queueing opportunistic roam as this is P2P port.");
        ndisStatus = NDIS_STATUS_INVALID_PORT_STATE;
        goto exit;
    }

    //don't queue opportunistic roam if media streaming mode set
    bMediaStreamingMode = (m_pAdapter->GetAdapterPropertyCache()->GetPropertyULongOrDefault(
        WfcAdapterPropertyMediaStreamingRefcount, 0) > 0) ? true : false;
    if (bMediaStreamingMode)
    {
        WFCPortInfo("[Roam][Opportunistic]Media streaming ref count non-zero.  Skipping opportunistic roam.");
        ndisStatus = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    opportunisticRoamDelayMs = m_pAdapter->GetAdapterPropertyCache()->GetPropertyULongOrDefault(
        WfcAdapterPropertyRegOpportunisticRoamDelay,
        WFC_CONSTANT::OpportunisticRoamDelayMs);

    //Cancel opportunistic roam timer if running before queuing a new one
    CancelOpportunisticRoam();

    ndisStatus = m_pAdapter->GetEventQueue()->RegisterTimeoutCallbackWithLevelAndReuse(
        IActivityId::get_NextActivityId(),
        this,
        opportunisticRoamDelayMs,
        &m_OpportunisticRoamTimerToken,
        WDF_EXECUTION_LEVEL::WdfExecutionLevelPassive,
        false,
        NULL,
        &m_OpportunisticRoamTimerToken
        );

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("[Roam][Opportunistic]Failed to register for opportunistic roam timer callback (0x%x)", ndisStatus);
        goto exit;
    }
    WFCPortInfo("[Roam][Opportunistic]Opportunistic roam timer has been queued.");
    //succeeded.  set the requested flag
    m_bOpportunisticRoamRequested = true;

exit:

    TraceExit(ndisStatus);
}

void CPort::CancelOpportunisticRoam()
{
    TraceEntry();
    //clear flag to prevent callback from proceeding with roam
    m_bOpportunisticRoamRequested = false;

    if (m_OpportunisticRoamTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        m_pAdapter->GetEventQueue()->DeregisterTimeoutCallback(m_OpportunisticRoamTimerToken);
        m_OpportunisticRoamTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;

        WFCPortInfo("Opportunistic roam timer canceled");
    }
}

NDIS_STATUS CPort::StartRoamJob(
    _Out_ CRoamReconnectJob **ppRoamJob,
    _In_ WFC_ROAM_CONNECT_TRIGGER RoamTrigger,
    _In_ WDI_ASSOC_STATUS RoamWabiReason,
    _In_ WDI_ROAM_CONFIGURATION_FLAGS RoamConfigFlags,
    _In_ WDF_EXECUTION_LEVEL ExecutionLevel
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CRoamReconnectJob *pNewRoamJob = NULL;
    ULONG opMode;

    TraceEntry();

    *ppRoamJob = NULL;

    if ( GetDot11State() == WfcPortDot11StateInit)
    {
        WFCPortInfo("[Roam]Not currently connected, ignoring roaming needed notification.");
        goto exit;
    }

    WFCPortInfo("[Roam] Roam operation triggered. Roam trigger %d", RoamTrigger);

    //
    // This function only returns errors after this point. Before here
    // this function can be a no-op, but now the function must handle
    // the roam request, and report failures if it can't.
    //

    pNewRoamJob = new CRoamReconnectJob(
        IActivityId::get_NextActivityId() );

    if ( pNewRoamJob == NULL )
    {
        status = NDIS_STATUS_RESOURCES;
        WFCPortError("Couldn't create new roaming job.");
        goto exit;
    }

    status = m_PortPropertyCache.GetPropertyULong(
        WfcPortCurrentOperationalMode,
        &opMode );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        NT_ASSERT(FALSE);
        opMode = WDI_OPERATION_MODE_STA;
    }

    status = pNewRoamJob->Initialize(
        m_pAdapter,
        m_NdisPortNumber,
        (opMode & WDI_OPERATION_MODE_P2P_CLIENT) ? true : false,
        RoamTrigger,
        RoamWabiReason,
        RoamConfigFlags
        );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't initialize new roaming job.");
        goto exit;
    }

    if (ExecutionLevel == WdfExecutionLevelPassive)
    {
        pNewRoamJob->set_RequiresPassiveIrql(true);
    }

    // Start the non-serialized roam job
    status = m_pAdapter->GetActiveJobsList()->StartAddNewJob(
        pNewRoamJob,
        this );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't add new roaming job to active jobs list.");
        goto exit;
    }

    m_QueuedJobCount++;
    
    // Save it as whatever the caller wanted it to be saved
    *ppRoamJob = pNewRoamJob;
    pNewRoamJob = NULL;

exit:
    if ( pNewRoamJob != NULL )
    {
        delete pNewRoamJob;
    }

    TraceExit(status);
    return status;
}

void CPort::OnIncomingActionFrameReceived(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_INDICATION_ACTION_FRAME_RECEIVED_PARAMETERS actionFrameParams;
    PDOT11_GENERIC_ACTION_FRAME pActionFrame = NULL;

    TraceEntry();

    status = ParseWdiIndicationActionFrameReceived(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext(),
        &actionFrameParams);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Couldn't parse incoming action frame parameters. 0x%x", status);
        goto exit;
    }

    if (actionFrameParams.ActionFrameBody.ElementCount < sizeof(DOT11_GENERIC_ACTION_FRAME))
    {
        WFCPortError("Empty action frame body received.");
        goto exit;
    }

    pActionFrame = (PDOT11_GENERIC_ACTION_FRAME)actionFrameParams.ActionFrameBody.pElements;

    switch (pActionFrame->Category)
    {
        case WDI_ACTION_FRAME_CATEGORY_PUBLIC:
        {
            OnPublicActionFrameReceived(
                &actionFrameParams.SourceAddress,
                &actionFrameParams.ChannelInfo,
                actionFrameParams.ActionFrameBody.ElementCount,
                actionFrameParams.ActionFrameBody.pElements);

            break;
        }

        case WDI_ACTION_FRAME_CATEGORY_RADIO_MEASUREMENT:
        {
            //
            // Parse Neighbor Report Reponse frame
            //
            PDOT11_NEIGHBOR_REPORT_FRAME pNeighborReportFrame = (PDOT11_NEIGHBOR_REPORT_FRAME)pActionFrame;
            if (actionFrameParams.ActionFrameBody.ElementCount >= FIELD_OFFSET(DOT11_NEIGHBOR_REPORT_FRAME, InfoElements) &&
                pNeighborReportFrame->Action == WDI_RADIO_MEASUREMENT_ACTION_NEIGHBOR_REPORT_RESPONSE)
            {
                OnNeighborReportReceived(
                    actionFrameParams.ActionFrameBody.ElementCount - FIELD_OFFSET(DOT11_NEIGHBOR_REPORT_FRAME, InfoElements),
                    actionFrameParams.ActionFrameBody.pElements + FIELD_OFFSET(DOT11_NEIGHBOR_REPORT_FRAME, InfoElements),
                    false
                    );
            }
            break;
        }
        case WDI_ACTION_FRAME_CATEGORY_WNM:
        {
            //
            // Parse if BSS Transition Management Request frame
            //
            if (pActionFrame->Action == WDI_WNM_ACTION_BSS_TRANSITION_MANAGEMENT_REQUEST)
            {
                // Ensure that 11v BSS Transition capability is turned ON in the adapter capability.
                NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
                ULONG uBufferLength;
                BYTE* pBuffer = NULL;
                ndisStatus = m_pAdapter->GetAdapterPropertyCache()->GetPropertyBuffer(
                    WfcAdapterPropertyEnhancedCapabilities,
                    &uBufferLength,
                    &pBuffer
                    );
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    break;
                }

                PDOT11_ADAPTER_CAPABILITIES pAdapterCapabilities = (PDOT11_ADAPTER_CAPABILITIES)pBuffer;

                if (m_pAdapter->GetMiniportDriver()->GetWdiCharacteristics()->WdiVersion >= WDI_VERSION_1_0_1)
                {
                    if (pAdapterCapabilities->BSSTransitionSupported)
                    {
                        OnBssTransitionManagementRequestReceived(
                            actionFrameParams.ActionFrameBody.ElementCount,
                            actionFrameParams.ActionFrameBody.pElements);
                    }
                }
                else
                {
                    OnBssTransitionManagementRequestReceived(
                        actionFrameParams.ActionFrameBody.ElementCount,
                        actionFrameParams.ActionFrameBody.pElements);
                }
            }
            break;
        }
        default:
            WFCPortTrace("Unrecognized action frame received, Category=%d, ElementCount=%d",
                pActionFrame->Category, actionFrameParams.ActionFrameBody.ElementCount);
            break;
    }

exit:
    TraceExit(status);
}

void CPort::OnNeighborReportReceived(
    _In_ ULONG NeighborReportBlobSize,
    _In_reads_bytes_(NeighborReportBlobSize) BYTE* pNeighborReportBlob,
    _In_ bool bIsNeighborReportFromBSSTransitionRequest)
{

    TraceEntry();
    //
    // Flush current neighbor report
    //
    CBSSListManager* pBSSListManager = m_pAdapter->GetBSSListManager(WfcPortTypeExtSTA);
    pBSSListManager->FlushAllNeighborReports();
    WDI_SET_NEIGHBOR_REPORT_ENTRIES_PARAMETERS neighborReportEntries;
    CSetNeighborReportEntriesJob* pNeighborReportSenderJob = NULL;
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG currentNeighborReport = 0;
    neighborReportEntries.NeighborReportEntry.pElements = NULL;

    ULONG uNumberOfNeighborReports = NeighborReportBlobSize / (sizeof(DOT11_NEIGHBOR_REPORT_IE)+sizeof(DOT11_INFO_ELEMENT));
    bool bIsAllocationSuccess = false;

    if (uNumberOfNeighborReports > 0)
    {
        status = neighborReportEntries.NeighborReportEntry.AllocateElements(uNumberOfNeighborReports, 0);
        if (status == NDIS_STATUS_SUCCESS)
        {
            bIsAllocationSuccess = true;
        }
        else
        {
            WFCPortError("[NeighborReport] Failed to allocate %u elements for Neighbor Report TLV. Status = 0x%08x. Not sending WDI_SET_NEIGHBOR_REPORT_ENTRIES", uNumberOfNeighborReports, status);
            bIsAllocationSuccess = false;
        }
    }

    while (NeighborReportBlobSize > 0)
    {
        PDOT11_INFO_ELEMENT pHeader = (PDOT11_INFO_ELEMENT)pNeighborReportBlob;

        // Validate neighbor report element
        if (NeighborReportBlobSize < sizeof(DOT11_INFO_ELEMENT) ||
            NeighborReportBlobSize < pHeader->Length + sizeof(DOT11_INFO_ELEMENT) ||
            NeighborReportBlobSize < sizeof(DOT11_NEIGHBOR_REPORT_IE) + sizeof(DOT11_INFO_ELEMENT) ||
            pHeader->ElementID != DOT11_INFO_ELEMENT_ID_NEIGHBOR_REPORT)
        {
            break;
        }

        PDOT11_NEIGHBOR_REPORT_IE pNeighborReportElement = (PDOT11_NEIGHBOR_REPORT_IE)(pNeighborReportBlob + sizeof(DOT11_INFO_ELEMENT));

        if (bIsAllocationSuccess && 
            currentNeighborReport < uNumberOfNeighborReports)
        {
            RtlCopyMemory(
                neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].Bssid.Address,
                pNeighborReportElement->BSSID,
                sizeof(DOT11_MAC_ADDRESS)
                );
            neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].BSSIDInfo.APReachability = pNeighborReportElement->BSSIDInfo.APReachability;
            neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].BSSIDInfo.APSD = pNeighborReportElement->BSSIDInfo.APSD;
            neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].BSSIDInfo.DelayedBlockAck = pNeighborReportElement->BSSIDInfo.DelayedBlockAck;
            neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].BSSIDInfo.HighThroughput = pNeighborReportElement->BSSIDInfo.HighThroughput;
            neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].BSSIDInfo.ImmediateBlockAck = pNeighborReportElement->BSSIDInfo.ImmediateBlockAck;
            neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].BSSIDInfo.KeyScope = pNeighborReportElement->BSSIDInfo.KeyScope;
            neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].BSSIDInfo.MobilityDomain = pNeighborReportElement->BSSIDInfo.MobilityDomain;
            neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].BSSIDInfo.QoS = pNeighborReportElement->BSSIDInfo.QoS;
            neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].BSSIDInfo.RadioMeasurement = pNeighborReportElement->BSSIDInfo.RadioMeasurement;
            neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].BSSIDInfo.Security = pNeighborReportElement->BSSIDInfo.Security;
            neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].BSSIDInfo.SpectrumManagement = pNeighborReportElement->BSSIDInfo.SpectrumManagement;

            neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].ChannelNumber = pNeighborReportElement->Channel;
            neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].OperatingClass = pNeighborReportElement->OperatingClass;
            neighborReportEntries.NeighborReportEntry.pElements[currentNeighborReport].PhyType = (WDI_PHY_TYPE)pNeighborReportElement->PhyType;
            currentNeighborReport += 1;
        }

        CBSSEntry* pBssEntry = pBSSListManager->FindBSSEntry(
            pNeighborReportElement->BSSID);

        if (pBssEntry != NULL)
        {
            // Add neighbor report to existing BSS entry
            pBssEntry->AddNeighborReport(pNeighborReportElement);

            if (bIsNeighborReportFromBSSTransitionRequest)
            {
                pBssEntry->SetIsSuggestedByAPBSSTransitionRequest(true);
            }
        }
        else
        {
            // Create new BSS entry
            pBssEntry = new CBSSEntry(pNeighborReportElement->BSSID);
            if (pBssEntry == NULL)
            {
                break;
            }
            pBssEntry->AddNeighborReport(pNeighborReportElement);
            pBssEntry->SetChannelInfo(
                pNeighborReportElement->Channel,
                ConvertOperatingClassToBand(pNeighborReportElement->OperatingClass)
                );
            if (bIsNeighborReportFromBSSTransitionRequest)
            {
                pBssEntry->SetIsSuggestedByAPBSSTransitionRequest(true);
            }

            //Currently AddBSSEntry doesn't return a failure code.
            //If in the future it does the allocated BssEntry should be freed,
            // the error is logged however it will not prevent the rest of the neighbor report
            // from being parsed and added to queue.
            NDIS_STATUS ndisStatus = pBSSListManager->AddBSSEntry(pBssEntry);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCPortError("Failed to add BssEntry for %!MACADDR!. (0x%x)",
                    pNeighborReportElement->BSSID,
                    ndisStatus);

                delete pBssEntry;
            }
        }

        // Move to next neighbor report element
        pNeighborReportBlob += sizeof(DOT11_INFO_ELEMENT) + pHeader->Length;
        NeighborReportBlobSize -= sizeof(DOT11_INFO_ELEMENT) + pHeader->Length;
    }

    if (bIsAllocationSuccess && currentNeighborReport > 0)
    {
        //Set element count to be the actual number of neighbor report entries in parsing.
        neighborReportEntries.NeighborReportEntry.ElementCount = currentNeighborReport;
        pNeighborReportSenderJob = new CSetNeighborReportEntriesJob(
            IActivityId::get_NextActivityId()
            );
        if (pNeighborReportSenderJob == NULL)
        {
            WFCError("Failed to allocate memory for CSetNeighborReportEntriesJob.");
            status = NDIS_STATUS_RESOURCES;
            goto exit;
        }

        status = pNeighborReportSenderJob->InitializeFromNeighborReportEntries(
            m_pAdapter,
            m_NdisPortNumber,
            m_WfcPortId,
            &neighborReportEntries
            );
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCError("Failed to initialize CSetNeighborReportEntriesJob. Error = 0x%08x", status);
            goto exit;
        }

        // Queue the job
        status = m_pAdapter->GetActiveJobsList()->StartAddNewJob(
            pNeighborReportSenderJob,
            this
            );
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCError("Failed to queue CSetNeighborReportEntriesJob job. Error = 0x08%x",
                status
                );
            goto exit;
        }

        m_QueuedJobCount++;
    }
exit:
    if (status != NDIS_STATUS_SUCCESS)
    {
        if (pNeighborReportSenderJob != NULL)
        {
            delete pNeighborReportSenderJob;
            pNeighborReportSenderJob = NULL;
        }
    }
    TraceExit(NDIS_STATUS_SUCCESS);
}

void CPort::QueueNeighborReportRequest()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CConnectedPeer *pPeer = GetConnectedPeerList()
        ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);

    TraceEntry();

    if (pPeer)
    {
        CBSSEntry* pBssEntry = m_pAdapter->GetBSSListManager(
            WfcPortTypeExtSTA)->FindBSSEntry(*pPeer->GetMacAddress());

        //
        // Send neighbor report request if supported
        //
        if (pBssEntry &&
            pBssEntry->IsNeighborReportSupported() &&
            m_pAdapter->GetAdapterPropertyCache()->GetPropertyBooleanOrDefault( WfcAdapterPropertyActionFramesSupported, FALSE ))
        {
            // Deregister previous timeout callback if it's valid, before registering another one
            if (m_NeighborRequestTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
            {
                m_pAdapter->GetEventQueue()->DeregisterTimeoutCallback(m_NeighborRequestTimerToken);
                m_NeighborRequestTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
            }

            ndisStatus = m_pAdapter->GetEventQueue()->RegisterTimeoutCallbackWithLevelAndReuse(
                IActivityId::get_NextActivityId(),
                this,
                WFC_CONSTANT::RequestNeighborReportDelayMs,
                &m_NeighborRequestTimerToken,
                WDF_EXECUTION_LEVEL::WdfExecutionLevelPassive,
                false,
                NULL,
                &m_NeighborRequestTimerToken
                );

            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCPortError("Failed to register for send neighbor report request timer callback (0x%x)", ndisStatus);
                goto exit;
            }

            WFCPortInfo("Neighbor report request timer has been queued");
        }
    }

exit:

    TraceExit(ndisStatus);
}


// 
// Timer callback for queueing neighbor report request and opportunistic roam
//
void CPort::OnTimerCallback(
    _In_opt_ WFC_TIMER_CONTEXT pvContext)
{
    TraceEntry();

    if (&m_NeighborRequestTimerToken == pvContext)
    {
        OnNeighborReportRequestTimerCallback();
    }
    else if (&m_OpportunisticRoamTimerToken == pvContext)
    {
        OnOpportunisticRoamTimerCallback();
    }
    else
    {
        WFCPortError("Port timer callback with unknown context");
        NT_ASSERTMSG("Unknown timer callback context", 0);
    }

    TraceExit(NDIS_STATUS_SUCCESS);
}

void CPort::OnNeighborReportRequestTimerCallback()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CSendActionFrameRequestJob *pNeighborRequestJob = NULL;

    TraceEntry();

    WFCPortInfo("Neighbor report request timer callback");

    pNeighborRequestJob = new CSendNeighborReportRequestJob(IActivityId::get_NextActivityId());
    if (pNeighborRequestJob == NULL)
    {
        ndisStatus = NDIS_STATUS_RESOURCES;
        WFCPortError("Couldn't create new pNeighborRequestJob.");
        goto exit;
    }

    ndisStatus = pNeighborRequestJob->Initialize(
        m_pAdapter,
        m_NdisPortNumber);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Couldn't initialize pNeighborRequestJob.");
        goto exit;
    }

    ndisStatus = m_pAdapter->GetSerializedJobList()->QueueSerializedJob(
        pNeighborRequestJob,
        this);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Couldn't queue new pNeighborRequestJob to job serializer.");
        goto exit;
    }

    m_QueuedJobCount++;

exit:

    if ((ndisStatus != NDIS_STATUS_SUCCESS) && (pNeighborRequestJob != NULL))
    {
        delete pNeighborRequestJob;
    }

    TraceExit(ndisStatus);
}

void CPort::OnOpportunisticRoamTimerCallback()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CConnectedPeer *pPeer = NULL;
    bool bMediaStreamingMode = false;

    TraceEntry();

    WFCPortInfo("[Roam][Opportunistic]Opportunistic roam timer callback");

    if (!m_bOpportunisticRoamRequested)
    {
        WFCPortInfo("[Roam][Opportunistic]Opportunistic roam is no longer needed.  Exiting.");
        goto exit;
    }
    m_bOpportunisticRoamRequested = false;

    //Don't trigger opportunistic roam if media streaming ref count non-zero.
    bMediaStreamingMode = (m_pAdapter->GetAdapterPropertyCache()->GetPropertyULongOrDefault(
                                WfcAdapterPropertyMediaStreamingRefcount, 0) > 0) ? true : false;
    if (bMediaStreamingMode)
    {
        WFCPortInfo("[Roam][Opportunistic]Media streaming ref count non-zero.  Skipping opportunistic roam.");
        ndisStatus = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    pPeer = m_pPeerList->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);
    if (pPeer == NULL)
    {
        WFCPortInfo("[Roam][Opportunistic]Not connected to AP.  Skipping opportunistic roam.");
        ndisStatus = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    WFCPortInfo("[Roam][Opportunistic]Attempting opportunistic roam");

    ndisStatus = TriggerWdiInitiatedRoam(WfcRoamConnectTriggerToFindBetterAP_Opportunistic,
                                         WDI_ASSOC_STATUS_ROAMING_BETTER_AP_FOUND,
                                         CConnectHelpers::GetRoamConfig(WfcRoamConnectTriggerToFindBetterAP_Opportunistic));
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Couldn't initiate roaming attempt");
    }

exit:
    if (m_OpportunisticRoamTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        m_pAdapter->GetEventQueue()->DeregisterTimeoutCallback(m_OpportunisticRoamTimerToken);
        m_OpportunisticRoamTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    TraceExit(ndisStatus);
}
// ---------------------------------------------------------------------

NDIS_STATUS CPort::SendANQPQueryResponse(
    _In_ DOT11_ACTION_FRAME_PUBLIC_ACTION_TYPE ActionType,
    _In_ UCHAR DialogToken,
    _In_ UCHAR AdvertisementProtocolID,
    _In_ USHORT StatusCode,                 // For Initial and Comeback Responses
    _In_ DOT11_ANQP_FRAGMENT_ID FragmentID, // For Comeback Response
    _In_ USHORT ComebackDelay,              // For Initial and Comeback Responses
    _In_ WDI_MAC_ADDRESS *pSourceAddress,
    _In_ UINT32 BandId,
    _In_ UINT32 ChannelNumber,
    _In_ USHORT QueryResponseLength,
    _In_bytecount_(QueryResponseLength) PUCHAR ucQueryBuffer
    )
{
    NDIS_STATUS                     ndisStatus = NDIS_STATUS_SUCCESS;
    ANQP_ACTION_FRAME_PARAMETERS    ANQPParams = {0};
    CSendActionFrameRequestJob      *pSendANQPResponseJob = NULL;

    TraceEntry();

    pSendANQPResponseJob = new CSendActionFrameRequestJob(IActivityId::get_NextActivityId());
    if (pSendANQPResponseJob == NULL)
    {
        ndisStatus = NDIS_STATUS_RESOURCES;
        WFCPortError("Couldn't create new pSendANQPResponseJob.");
        goto exit;
    }

    ANQPParams.AdvertisementProtocolID = AdvertisementProtocolID;
    ANQPParams.StatusCode = StatusCode;
    ANQPParams.FragmentID = FragmentID;
    ANQPParams.ComebackDelay = ComebackDelay;

    ndisStatus = pSendANQPResponseJob->InitializePublicActionFrame(
        m_pAdapter,
        m_NdisPortNumber,
        ActionType,
        DialogToken,
        pSourceAddress,
        BandId,
        ChannelNumber,
        QueryResponseLength,
        ucQueryBuffer,
        &ANQPParams,
        FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Couldn't initialize pSendANQPResponseJob.");
        goto exit;
    }

    ndisStatus = m_pAdapter->GetSerializedJobList()->QueueSerializedJob(
        pSendANQPResponseJob,
        this);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Couldn't queue new pSendANQPResponseJob to job serializer.");
        goto exit;
    }

    m_QueuedJobCount++;

exit:

    if ((ndisStatus != NDIS_STATUS_SUCCESS) && (pSendANQPResponseJob != NULL))
    {
        delete pSendANQPResponseJob;
    }

    TraceExit(ndisStatus);
    return(ndisStatus);
}


void CPort::SendResponseToServiceQueryComebackRequest(
    _In_ CServicesManager *pServicesManager,
    _In_ WDI_MAC_ADDRESS *pSourceAddress,
    _In_ WDI_CHANNEL_INFO_CONTAINER *pChannelInfo,
    _In_ ULONG PublicActionFrameBlobSize,
    _In_reads_bytes_(PublicActionFrameBlobSize) PDOT11_ANQP_ACTION_FRAME pANQPFrame)
{
    NDIS_STATUS status;
    UCHAR uAdvertisementProtocolID;
    USHORT uQueryResponseLength;
    PBYTE pQueryResponse = NULL;
    CServiceComebackResponseContext *pServiceComebackResponseContext = NULL;
    DOT11_ANQP_FRAGMENT_ID FragmentID = {0};

    UNREFERENCED_PARAMETER(PublicActionFrameBlobSize);

    //
    // Cache the response for the subsequent Comeback requests
    //
    pServiceComebackResponseContext = pServicesManager->FindComebackResponseContext(
        pSourceAddress,
        pANQPFrame->DialogToken);
    if (!pServiceComebackResponseContext)
    {
        status = NDIS_STATUS_RESOURCES;
        WFCError("FindComebackResponseContext FAILed, status=0x%x.", status);

        status = SendANQPQueryResponse(
            DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_COMEBACK_RESPONSE,
            pANQPFrame->DialogToken,
            0,                                      // We do not know the AdvertisementProtocolID for this request
            60,                                     // StatusCode = NO_OUTSTANDING_REQUEST
            FragmentID,                             // FragmentID
            0,                                      // ComebackDelay
            pSourceAddress,
            pChannelInfo->BandId,
            pChannelInfo->ChannelNumber,
            0,
            NULL);

        goto Exit;
    }

    uAdvertisementProtocolID = 0;
    uQueryResponseLength = 0;
    pQueryResponse = NULL;
    status = pServiceComebackResponseContext->GetNextFragmentToSend(
        &uAdvertisementProtocolID,
        &FragmentID,
        &uQueryResponseLength,
        &pQueryResponse);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("GetNextFragmentToSend FAIled, status=0x%x.", status);
        goto Exit;
    }

    if ((!uQueryResponseLength) ||
        (!pQueryResponse))
    {
        status = NDIS_STATUS_RESOURCES;
        WFCError("GetNextFragmentToSend FAILed %!MACADDR!, DialogToken=%d, FragmentID=0x%x, ResponseLength=%d, pResponse=%p.",
            pSourceAddress->Address, pANQPFrame->DialogToken, FragmentID.Value, uQueryResponseLength, pQueryResponse);
        goto Exit;
    }

    status = SendANQPQueryResponse(
        DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_COMEBACK_RESPONSE,
        pANQPFrame->DialogToken,
        uAdvertisementProtocolID,
        0,                                      // StatusCode = dot11_ANQP_query_result_success
        FragmentID,                             // FragmentID
        0,                                      // Comeback Delay
        pSourceAddress,
        pChannelInfo->BandId,
        pChannelInfo->ChannelNumber,
        uQueryResponseLength,
        pQueryResponse);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("SendANQPQueryResponse FAIled, Dest=%!MACADDR!, DialogToken=%d, status=0x%x.",
            pSourceAddress->Address, pANQPFrame->DialogToken, status);
        goto Exit;
    }


    //
    // If this is the last fragment, remove the context
    //
    if ((!FragmentID.MoreFragments) ||
        (0 == uQueryResponseLength))
    {
        pServicesManager->RemoveComebackResponseContext( pServiceComebackResponseContext );
    }

Exit:
    TraceExit(status);
}


void CPort::SendResponseToServiceQueryInitialRequest(
    _In_ CServicesManager *pServicesManager,
    _In_ WDI_MAC_ADDRESS *pSourceAddress,
    _In_ WDI_CHANNEL_INFO_CONTAINER *pChannelInfo,
    _In_ ULONG PublicActionFrameBlobSize,
    _In_reads_bytes_(PublicActionFrameBlobSize) PDOT11_ANQP_ACTION_FRAME pANQPFrame)
{
    ULONG uBytes, uRequestedLength, uQueryRequestLength, uQueryResponseLength;
    USHORT uANQPResponseLength;
    NDIS_STATUS status;
    DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST *pDot11FullAdvList = NULL;
    PBYTE pQueryResponse = NULL;
    CServiceComebackResponseContext *pServiceComebackResponseContext = NULL;
    WFC_PORT_ID PortId;
    DOT11_ANQP_FRAGMENT_ID FragmentID = {0};

    TraceEntry();

    if (0 != pANQPFrame->InitialRequest.AdvertisementProtocolElement.AdvertisementProtocol[0].AdvertisementProtocolID)
    {
        WFCError("ERROR: Invalid QueryRequest ElementID=<%d>, ProtocolID=<%d>\n",
            pANQPFrame->InitialRequest.AdvertisementProtocolElement.ElementID,
            pANQPFrame->InitialRequest.AdvertisementProtocolElement.AdvertisementProtocol[0].AdvertisementProtocolID);

        status = SendANQPQueryResponse(
            DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_INITIAL_RESPONSE,
            pANQPFrame->DialogToken,
            pANQPFrame->InitialRequest.AdvertisementProtocolElement.AdvertisementProtocol[0].AdvertisementProtocolID,
            59,                                     // StatusCode = ADVERTISEMENT_PROTOCOL_NOT_SUPPORTED
            FragmentID,                             // FragmentID
            0,                                      // ComebackDelay
            pSourceAddress,
            pChannelInfo->BandId,
            pChannelInfo->ChannelNumber,
            0,
            NULL);

        goto Exit;
    }

    uQueryRequestLength = Dot11ToHostShort (pANQPFrame->InitialRequest.QueryRequestLength);
    if (PublicActionFrameBlobSize < (FIELD_OFFSET(DOT11_ANQP_ACTION_FRAME, InitialRequest.InfoElements) +
                                     uQueryRequestLength))
    {
        WFCError("ERROR: Invalid QueryRequest - Size=<%d < %d>\n",
            PublicActionFrameBlobSize,
            (FIELD_OFFSET(DOT11_ANQP_ACTION_FRAME, InitialRequest.InfoElements) + uQueryRequestLength));
        status = NDIS_STATUS_INVALID_DATA;
        goto Exit;
    }

    //
    // For WFD, if the request came on a Role port, get the advertised list from the Device port
    //
    PortId = GetPortId();
    if (GetPortType() == WfcPortTypeWFDRole)
    {
        status = m_pAdapter->GetPortIdForPortType(WfcPortTypeWFDDevice, &PortId);
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCError("FAIled to get PortId for Device port, status=0x%x.", status);
            goto Exit;
        }
    }

    uBytes = 0;
    status = m_pAdapter->GetPortPropertyCache(PortId)->GetPropertyBuffer(
        WfcPortPropertyP2PAdvertisementList,
        &uBytes,
        (PBYTE *) &pDot11FullAdvList);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("FAIled to find WfcPortPropertyP2PAdvertisementList in port Property Cache, status=0x%x.", status);
        goto Exit;
    }

    if ((!uBytes) ||
        (uBytes < sizeof(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST)) ||
        (uBytes < (sizeof(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST) +
                        pDot11FullAdvList->uAdvertisedServicesLength +
                        pDot11FullAdvList->uAdvertisedServicePrefixesLength)))
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCError("Invalid AdvertisementList in property cache, uBytes=%d.", uBytes);
        goto Exit;
    }

    // Get Size
    uRequestedLength = 0;
    status = WFDSvcEncodeANQPResponse(
        uQueryRequestLength,
        (PBYTE) &pANQPFrame->InitialRequest.InfoElements,
        uBytes,
        pDot11FullAdvList,
        &uRequestedLength,
        NULL
        );
    if ((status != STATUS_BUFFER_TOO_SMALL) ||
        (!uRequestedLength))
    {
        WFCError("WFDSvcEncodeANQPResponse[1] FAIled, status=0x%x, Length=%d.", status, uRequestedLength);
        goto Exit;
    }

    if (uRequestedLength > 65535)
    {
        WFCError("ERROR: Query Response Length is too large -- %d > %d\n", uRequestedLength, 65535);

        status = SendANQPQueryResponse(
            DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_INITIAL_RESPONSE,
            pANQPFrame->DialogToken,
            pANQPFrame->InitialRequest.AdvertisementProtocolElement.AdvertisementProtocol[0].AdvertisementProtocolID,
            63,                                     // StatusCode = QUERY_RESPONSE_TOO_LARGE
            FragmentID,                             // FragmentID
            0,                                      // ComebackDelay
            pSourceAddress,
            pChannelInfo->BandId,
            pChannelInfo->ChannelNumber,
            0,
            NULL);

        goto Exit;
    }

    pQueryResponse = new UCHAR[uRequestedLength];
    if (!pQueryResponse)
    {
        WFCError("Couldn't allocate %d bytes for Query Response buffer.", uRequestedLength);
        status = NDIS_STATUS_RESOURCES;
        goto Exit;
    }

    uQueryResponseLength = uRequestedLength;
    status = WFDSvcEncodeANQPResponse(
        uQueryRequestLength,
        (PBYTE) &pANQPFrame->InitialRequest.InfoElements,
        uBytes,
        pDot11FullAdvList,
        &uQueryResponseLength,
        pQueryResponse
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("WFDSvcEncodeANQPResponse[2] FAIled, status=0x%x, RequestedLength=%d, Returned=%d.",
            status, uRequestedLength, uQueryResponseLength);
        goto Exit;
    }

    //
    // If only 1 MPDU is needed, send it out and be done
    //
    uANQPResponseLength = (USHORT) uQueryResponseLength;
    if (uQueryResponseLength < pServicesManager->GetMaxResponseFragmentSize())
    {
        status = SendANQPQueryResponse(
            DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_INITIAL_RESPONSE,
            pANQPFrame->DialogToken,
            pANQPFrame->InitialRequest.AdvertisementProtocolElement.AdvertisementProtocol[0].AdvertisementProtocolID,
            0,                                      // StatusCode = dot11_ANQP_query_result_success
            FragmentID,                             // FragmentID
            0,                                      // Comeback Delay
            pSourceAddress,
            pChannelInfo->BandId,
            pChannelInfo->ChannelNumber,
            uANQPResponseLength,
            pQueryResponse);

        goto Exit;
    }

    //
    // Cache the response for the subsequent Comeback requests
    //
    pServiceComebackResponseContext = pServicesManager->AddComebackResponseContext(
        pSourceAddress,
        pANQPFrame->DialogToken,
        &pANQPFrame->InitialRequest.AdvertisementProtocolElement,
        uANQPResponseLength,
        pQueryResponse);
    if (!pServiceComebackResponseContext)
    {
        status = NDIS_STATUS_RESOURCES;
        WFCError("WFDSvcEncodeANQPResponse FAILed, status=0x%x.", status);
        goto Exit;
    }
    pQueryResponse = NULL;      // Set to NULL since it has been cached in the ComebackResponseContext

    //
    // Sending the Initial Response for multiple MPDUs
    //
    status = SendANQPQueryResponse(
        DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_INITIAL_RESPONSE,
        pANQPFrame->DialogToken,
        pANQPFrame->InitialRequest.AdvertisementProtocolElement.AdvertisementProtocol[0].AdvertisementProtocolID,
        0,                                      // StatusCode = dot11_ANQP_query_result_success
        FragmentID,                             // FragmentID
        1,                                      // Comeback Delay indicating that a Comeback Request is expected
        pSourceAddress,
        pChannelInfo->BandId,
        pChannelInfo->ChannelNumber,
        0,
        NULL);

Exit:
    if (pQueryResponse)
    {
        delete [] pQueryResponse;
    }

    TraceExit(status);
}


void CPort::OnPublicActionFrameReceived(
    _In_ WDI_MAC_ADDRESS *pSourceAddress,
    _In_ WDI_CHANNEL_INFO_CONTAINER *pChannelInfo,
    _In_ ULONG PublicActionFrameBlobSize,
    _In_reads_bytes_(PublicActionFrameBlobSize) BYTE* pGASRequestBlob)
{
    PDOT11_ANQP_ACTION_FRAME pANQPFrame = (PDOT11_ANQP_ACTION_FRAME) pGASRequestBlob;
    CServicesManager *pServicesManager = m_pAdapter->GetServicesManager();

    switch (pANQPFrame->Action)
    {
        //
        // We will only handle ServiceQueryRequests here.  ServiceQueryResponse frames will be handled
        // by the tasks which sent the corresponding request
        //
        case (DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_INITIAL_REQUEST):
        {
            //
            // Validate the Frame contents - check for:
            //  Frame length
            //  ANQP Protocol ID (= 0)
            //
            if ((PublicActionFrameBlobSize < FIELD_OFFSET(DOT11_ANQP_ACTION_FRAME, InitialRequest.InfoElements)) ||
                (DOT11_INFO_ELEMENT_ID_ADVERTISEMENT_PROTOCOL !=
                pANQPFrame->InitialRequest.AdvertisementProtocolElement.ElementID))
            {
                WFCError("ERROR: Invalid QueryRequest Size=<%d>, ElementID=<%d>, ProtocolID=<%d>\n",
                    PublicActionFrameBlobSize, pANQPFrame->InitialRequest.AdvertisementProtocolElement.ElementID,
                    pANQPFrame->InitialRequest.AdvertisementProtocolElement.AdvertisementProtocol[0].AdvertisementProtocolID);
                break;
            }

            SendResponseToServiceQueryInitialRequest(
                pServicesManager,
                pSourceAddress,
                pChannelInfo,
                PublicActionFrameBlobSize,
                (PDOT11_ANQP_ACTION_FRAME) pGASRequestBlob);
            break;
        }

        case (DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_COMEBACK_REQUEST):
        {
            //
            // Validate the Frame contents - check for:
            //  Frame length
            //  ANQP Protocol ID (= 0)
            //
            if (PublicActionFrameBlobSize < FIELD_OFFSET(DOT11_ANQP_ACTION_FRAME, ComebackResponse.StatusCode))
            {
                WFCError("ERROR: Invalid ComebackRequest Size=<%d>\n", PublicActionFrameBlobSize);
                break;
            }

            SendResponseToServiceQueryComebackRequest(
                pServicesManager,
                pSourceAddress,
                pChannelInfo,
                PublicActionFrameBlobSize,
                (PDOT11_ANQP_ACTION_FRAME) pGASRequestBlob);
            break;
        }

        default:
        {
            break;
        }
    }
}

void CPort::OnBssTransitionManagementRequestReceived(
    _In_ ULONG RequestBlobSize,
    _In_reads_bytes_(RequestBlobSize) BYTE* pRequestBlob)
{
    PDOT11_BSS_TRANSITION_MANAGEMENT_REQUEST_FRAME pRequestFrame = NULL;
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_ROAM_CONNECT_TRIGGER trigger = WfcRoamConnectTriggerBssTransitionRequest;

    if (RequestBlobSize < sizeof(DOT11_BSS_TRANSITION_MANAGEMENT_REQUEST_FRAME))
    {
        return;
    }

    pRequestFrame = (PDOT11_BSS_TRANSITION_MANAGEMENT_REQUEST_FRAME)pRequestBlob;


    /* Start roam */
    if (pRequestFrame->RequestMode.PreferredCandidateListIncluded)
    {
        OnNeighborReportReceived(
            RequestBlobSize - sizeof(DOT11_BSS_TRANSITION_MANAGEMENT_REQUEST_FRAME),
            pRequestBlob + sizeof(DOT11_BSS_TRANSITION_MANAGEMENT_REQUEST_FRAME),
            true
            );
    }

    if (((m_pRoamJob != NULL) || (m_pReconnectJob != NULL) || (m_pDisconnectJob != NULL)) &&
        m_BSSTransitionRequestFrameReceived)
    {
        WFCPortInfo("[Roam][BetterAP] Already roaming/reconnecting/disconnecting, and a BSS Transition request was received already. Ignoring this");
        goto Exit;
    }

    m_BSSTransitionRequestDialogToken = pRequestFrame->DialogToken;
    m_BSSTransitionRequestFrameReceived = true;

    WFCInfo("Received a BSS Transition Management Request from the AP. Triggering a Roam");

    trigger = (pRequestFrame->RequestMode.DisassociationImminent || pRequestFrame->RequestMode.BSSTerminationIncluded) ? 
        WfcRoamConnectTriggerBssTransitionRequestCritical : WfcRoamConnectTriggerBssTransitionRequest;

    status = TriggerWdiInitiatedRoam(trigger,
                                     WDI_ASSOC_STATUS_ROAMING_BETTER_AP_FOUND,
                                     CConnectHelpers::GetRoamConfig(trigger));
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Couldn't initiate roaming attempt (Error: 0x%08x). Sending out reject in Action Frame response as we will stay in same AP now", 
             status);
        status = SendResponseBSSTransitionFrame(
            WDI_BSS_TRANSITION_REJECT_UNSPECIFIED_REASON, 
            NULL  //Target BSSID - NULL, as we are rejecting
            );
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCPortError("Failed to send Action frame -  BSS Transition response. Error = 0x%08x", status);
            goto Exit;
        }
    }

Exit:
    TraceExit(status);
}

NDIS_STATUS CPort::SendResponseBSSTransitionFrame(
    _In_ WDI_BSS_TRANSITION_RESPONSE_RESULT StatusCode,
    _In_opt_ PDOT11_MAC_ADDRESS TargetBSSId)
{
    CBssTransitionResponseJob *pResponseJob = NULL;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    if (!m_BSSTransitionRequestFrameReceived)
    {
        WFCPortError("Request frame hasn't been received yet or response already sent.");
        return NDIS_STATUS_FAILURE;
    }

    pResponseJob = new CBssTransitionResponseJob(IActivityId::get_NextActivityId());
    if (pResponseJob == NULL)
    {
        ndisStatus = NDIS_STATUS_RESOURCES;
        WFCPortError("Couldn't create new pResponseJob.");
        goto Exit;
    }

    pResponseJob->SetResponseFrameParameters(
        m_BSSTransitionRequestDialogToken,            //Use the dialog token from request frame
        StatusCode
        );

    if (StatusCode == WDI_BSS_TRANSITION_ACCEPT)
    {
        if (TargetBSSId != NULL)  //Optional field
        {
            pResponseJob->SetTargetBssId(TargetBSSId);
        }
        else
        {
            WFCInfo("[Roam][11v] Sending accept BSS transition frame without the optional Target BSSID");
        }
    }

    ndisStatus = pResponseJob->Initialize(
        m_pAdapter,
        m_NdisPortNumber
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Couldn't initialize pResponseJob.");
        goto Exit;
    }

    ndisStatus = m_pAdapter->GetSerializedJobList()->QueueSerializedJob(
        pResponseJob,
        this
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCPortError("Couldn't queue new pResponseJob to job serializer.");
        goto Exit;
    }

    m_QueuedJobCount++;
    //Reset the variable so that duplicate response frames arent sent for same request frame
    m_BSSTransitionRequestFrameReceived = false;
Exit:

    if ((ndisStatus != NDIS_STATUS_SUCCESS) && (pResponseJob != NULL))
    {
        delete pResponseJob;
    }

    TraceExit(ndisStatus);
    return ndisStatus;
}

void CPort::BlockSSID(DOT11_SSID blockedSSID,
    WFC_BLOCKED_REASON reason)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CNetworkHistory* pNetwork;

    status = m_pConnectHistory->UpdateOrAddNetworkToTable(&blockedSSID, &pNetwork);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError( "[BlockedList][11r] Failed to add SSID = %!DOT11SSID!  to connect history. Error = 0x%x", 
            &blockedSSID, status );
    }
    else
    {
        pNetwork->BlockedInfo.BlockedReason = reason;
        pNetwork->BlockedInfo.LastAssocFailTime = CSystem::get_CurrentTime();
        WFCTrace("[BlockedList][11r] Blocking SSID = %!DOT11SSID! for reason: 0x%08x", &blockedSSID, reason);
    }
}

void CPort::UnBlockSSID(DOT11_SSID blockedSSID,
    WFC_BLOCKED_REASON reason)
{
    CNetworkHistory* pNetwork;

    pNetwork = m_pConnectHistory->FindNetworkBySsid(&blockedSSID);
    if (pNetwork)
    {
        pNetwork->BlockedInfo.BlockedReason = WFC_BLOCKED_REASON_NONE;
        pNetwork->BlockedInfo.LastAssocFailTime = 0;
        WFCTrace("[BlockedList][11r] Unblocking SSID = %!DOT11SSID! for reason: 0x%08x", &blockedSSID, reason);
    }
}

bool CPort::IsValidSSIDForFT(DOT11_SSID SSID,
    ULONGLONG currentTime)
{
    CNetworkHistory* pNetwork;

    pNetwork = m_pConnectHistory->FindNetworkBySsid(&SSID);
    if (pNetwork)
    {
        if (pNetwork->BlockedInfo.BlockedReason != WFC_BLOCKED_REASON_NONE)
        {
            // Current time returned by the Get_CurrentTime() is in 100ns units
            return (currentTime > 
                (pNetwork->BlockedInfo.LastAssocFailTime 
                    + ((ULONGLONG)WFC_CONSTANT::FastTransitionConnectRoamMinBlockedTimeSeconds * 10000000))
                    );
        }
        else
        {
            return true;
        }
    }
    //We did not find the entry, so returning true
    return true;
}

NDIS_STATUS CPort::IncorporateBSSEntryList(
    _In_ ArrayOfElements<WDI_BSS_ENTRY_CONTAINER>* BSSEntryList,
    _Inout_ ULONG* pIncorporatedEntryCount,
    _Out_writes_opt_(*pIncorporatedEntryCount) CBSSEntry** ppIncorporatedBssEntryList
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONGLONG currentTime;
    CBSSEntry* pBssEntry = NULL;
    bool addBSSEntryToList = false;
    bool bOnSTAPort = (WfcPortTypeExtSTA == GetPortType() ? true : false);
    CBSSListManager* pBSSListManager = m_pAdapter->GetBSSListManager(
        bOnSTAPort ? WfcPortTypeExtSTA : WfcPortTypeWFDDevice);
    ULONG outEntriesCount = 0;
    ULONG inListCapacity = 0;
    UINT16 beaconBufferLength = 0, probeBufferLength = 0;
    bool bIsWFDDevice;
    DOT11_MAC_ADDRESS WfdDeviceAddress = { 0 };

    TraceEntry();

    if (pIncorporatedEntryCount)
    {
        inListCapacity = *pIncorporatedEntryCount;
    }

    currentTime = CSystem::get_CurrentTime();
    for (UINT32 i = 0; i < BSSEntryList->ElementCount; ++i)
    {
        WDI_BSS_ENTRY_CONTAINER* pParsedEntry = &(BSSEntryList->pElements[i]);
        DOT11_MAC_ADDRESS* CurrentBssID = (DOT11_MAC_ADDRESS*)pParsedEntry->BSSID.Address;

        addBSSEntryToList = false;
        beaconBufferLength = 0;
        probeBufferLength = 0;
        bIsWFDDevice = false;
        RtlZeroMemory(&WfdDeviceAddress, sizeof(WfdDeviceAddress));

        ndisStatus = Dot11GetDeviceAddressFromBSSEntry(
            pParsedEntry,
            &beaconBufferLength,
            &probeBufferLength,
            &WfdDeviceAddress,
            &bIsWFDDevice
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            break;
        }

        if (bOnSTAPort)
        {
            if (pBSSListManager->IsIHVMaintainedExtSTABSSListCache())
            {
                // Time stamp has to come from the device
                if (!pParsedEntry->Optional.EntryAgeInfo_IsPresent)
                {
                    WFCError("[BSSList] Failed to find age info in BSS entry");
                    ndisStatus = NDIS_STATUS_INVALID_DATA;
                    break;
                }

                currentTime = pParsedEntry->EntryAgeInfo.HostTimeStamp;
            }

#ifndef ALLOW_STA_ENTRIES_ON_11ad
            if (WDI_BAND_ID_60000 == pParsedEntry->ChannelInfo.BandId)
            {
                WFCTrace("IncorporateBSSEntryList: Dropping AP entry on 11ad band - MAC=%!MACADDR!, PortID=%d, PortType=%d\n",
                    (UCHAR*)CurrentBssID, GetPortId(), GetPortType());
                continue;
            }
#endif  // !ALLOW_STA_ENTRIES_ON_11ad
        }
        else if (!bIsWFDDevice)     // If this came on the device port, the P2P IE must be present
        {
            WFCTrace("IncorporateBSSEntryList: Ignoring AP on DevicePort - MAC=%!MACADDR!, PortID=%d, PortType=%d\n",
                (UCHAR*)CurrentBssID, GetPortId(), GetPortType());
            continue;
        }

        pBssEntry = pBSSListManager->FindBSSEntry(*CurrentBssID);
        if (pBssEntry == NULL)
        {
            pBssEntry = new CBSSEntry(*CurrentBssID);
            if (pBssEntry == NULL)
            {
                WFCError("Failed to allocate memory for BSS entry");
                ndisStatus = NDIS_STATUS_RESOURCES;
                break;
            }

            addBSSEntryToList = true;
        }

        pBssEntry->SetSignalInfo(
            pParsedEntry->SignalInfo.RSSI,
            pParsedEntry->SignalInfo.LinkQuality
            );

        if (pParsedEntry->Optional.ProbeResponseFrame_IsPresent)
        {
            ndisStatus = pBssEntry->SetBeaconOrProbeResponse(
                WFC_BSS_ENTRY_PROBE_RESPONSE_FRAME,
                currentTime,
                (BYTE*)pParsedEntry->ProbeResponseFrame.pElements,
                probeBufferLength,
                (bIsWFDDevice ? &WfdDeviceAddress : NULL)
                );
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                break;
            }
        }

        if (pParsedEntry->Optional.BeaconFrame_IsPresent)
        {
            ndisStatus = pBssEntry->SetBeaconOrProbeResponse(
                WFC_BSS_ENTRY_BEACON_FRAME,
                currentTime,
                (BYTE*)pParsedEntry->BeaconFrame.pElements,
                beaconBufferLength,
                (bIsWFDDevice ? &WfdDeviceAddress : NULL)
                );
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                break;
            }
        }

        if (pParsedEntry->Optional.DeviceSpecificContext_IsPresent)
        {
            ndisStatus = pBssEntry->SetDeviceContext(
                (BYTE*)pParsedEntry->DeviceSpecificContext.pElements,
                (UINT16)pParsedEntry->DeviceSpecificContext.ElementCount
                );
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                break;
            }
        }

        pBssEntry->SetChannelAndPhyID(
            bOnSTAPort,
            m_pAdapter->GetAdapterPropertyCache(),
            pParsedEntry->ChannelInfo.ChannelNumber,
            pParsedEntry->ChannelInfo.BandId
            );

        if (pBSSListManager->IsBackgroundDiscoveryEnabled())
        {
            ndisStatus = pBssEntry->UpdateBackgroundDeviceIndicatedByDriver( m_pAdapter );
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                break;
            }
        }

        pBssEntry->SetVisibility(true);

        if (addBSSEntryToList)
        {
            ndisStatus = pBSSListManager->AddBSSEntry(pBssEntry);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                break;
            }
        }
        else
        {
            // Move existing entries to the head so that search finds the freshest entries first
            pBSSListManager->MoveBSSEntryToHeadOfList(pBssEntry);
        }

        if (inListCapacity > outEntriesCount)
        {
            // Report the entry to the caller
            ppIncorporatedBssEntryList[outEntriesCount] = pBssEntry;
            outEntriesCount++;            
        }

        pBssEntry = NULL;
    }

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if ((pBssEntry) &&
            (addBSSEntryToList))
        {
            delete pBssEntry;
        }
        WFCError("[BSSList] Failed to parse BSS List (0x%x)", ndisStatus);
    }

    if (pIncorporatedEntryCount)
    {
        *pIncorporatedEntryCount = outEntriesCount;
    }
    
    TraceExit(ndisStatus);

    return ndisStatus;
}

void CPort::OnBSSEntryNotification(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WDI_INDICATION_BSS_ENTRY_LIST_PARAMETERS bssEntryList;

    TraceEntry();

    NT_ASSERT(EventBufferLength >= sizeof(WFC_MESSAGE_METADATA));
    if (EventBufferLength < sizeof(WFC_MESSAGE_METADATA))
    {
        WFCError("Invalid EventBufferLength %d < %d", EventBufferLength, sizeof(WFC_MESSAGE_METADATA));
        goto Exit;
    }

    ndisStatus = ParseWdiIndicationBssEntryList(
        (ULONG)(EventBufferLength - sizeof(WFC_MESSAGE_METADATA)),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &bssEntryList
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to parse BSS List (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = IncorporateBSSEntryList(&bssEntryList.DeviceDescriptor, 0, NULL);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[BSSList] Failed to parse BSS List (0x%x)", ndisStatus);
    }

Exit:
    TraceExit(ndisStatus);
}


void CPort::OnNloDiscovery(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    TraceEntry();
    WFCPortInfo("[NLO] Received Nlo discovery notification from device.");

    //
    // OS issued NLO discovery is revised to carry payload of BSS entry list. It must have at least 
    // one BSS entry that is discovered. This revision avoids the need to Query BSS list
    // before indicating the discovery to the OS. This streamlines the flow.
    // Most importantly, it avoid a race condition where Discovery may happens in D0 while
    // NDIS happens to be putting the NIC to Dx. The Query code could, if inDx, be dropped by 
    // command queue. Implement code to retrieve BSS list.
    //

    //
    // parse the list into BSS manager list; leverage the existing method
    //
    if ( EventBufferLength > sizeof( WFC_MESSAGE_METADATA ))
    {
        OnBSSEntryNotification( EventBufferLength, EventBuffer );
    }
    else
    {
        WFCError( "NLO Indication too short; maybe no payload" );
        ASSERT( 0 );
    }

    //
    // indicate NLO discovery
    //
    DOT11_OFFLOAD_NETWORK_STATUS_PARAMETERS nloDiscoveryParams;

    ASSIGN_NDIS_OBJECT_HEADER(
        nloDiscoveryParams.Header, 
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_OFFLOAD_NETWORK_STATUS_PARAMETERS_REVISION_1,
        DOT11_SIZEOF_OFFLOAD_NETWORK_STATUS_PARAMETERS_REVISION_1 );
        
    nloDiscoveryParams.Status = NDIS_STATUS_SUCCESS;

    m_pAdapter->IndicateStatus(
        NDIS_DEFAULT_PORT_NUMBER,
        NDIS_STATUS_DOT11_OFFLOAD_NETWORK_STATUS_CHANGED,
        NULL,
        &nloDiscoveryParams,
        sizeof(nloDiscoveryParams) );

    //
    // remember this time so when connection comes down soon ( 1s ) we scan for best ap
    //
    m_ullLastNloDiscoverTime = CSystem::get_CurrentTime();

    TraceExit(NDIS_STATUS_SUCCESS);
}

void CPort::OnWakeReason(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    WDI_INDICATION_WAKE_REASON_PARAMETERS parameter;
    NDIS_STATUS status;
    NTSTATUS ntstatus;

    ULONG cbTotalToAlloc = sizeof( NDIS_PM_WAKE_REASON );

    BYTE * pb; // @msg payload

    // to satisfy NDIS aligment requirements.
    const ULONG cbPad1 = ALIGN_UP_BY( sizeof(NDIS_PM_WAKE_REASON), 8) - sizeof(NDIS_PM_WAKE_REASON);
    const ULONG cbPad2 = ALIGN_UP_BY( sizeof(NDIS_PM_WAKE_PACKET), 8) - sizeof(NDIS_PM_WAKE_PACKET);

    TraceEntry();

    WFCPortInfo("Got wake reason notification from device.");

    //NDIS_STATUS_PM_WAKE_REASON
    PNDIS_PM_WAKE_REASON pNdisWakeReason;
    status = ParseWdiIndicationWakeReason( EventBufferLength - sizeof( WFC_MESSAGE_METADATA ),
        EventBuffer + sizeof( WFC_MESSAGE_METADATA ),
        m_pAdapter->GetTlvContext(),
        &parameter );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCPortError( "Failed to parse Wake Reason Indication. Error = 0x%08x", status );
        NT_ASSERTMSG( "Invalid WakeReason TLV parsed", FALSE );
        goto Exit;
    }

    //
    // check the type and allocate memory. additional memory if it is a packet wake
    //
    if (parameter.Optional.WakePacket_IsPresent)
    {
        const ULONG totalCbPadding = cbPad1 + cbPad2; //guaranteed to be less than 8 + 8, so we don't need to check for overflow here

        NT_ASSERTMSG( "Wake Packet must also have Wake Packet Pattern Id", parameter.Optional.WakePacketPatternId_IsPresent );

        ntstatus = RtlULongAdd(cbTotalToAlloc, (ULONG)sizeof(NDIS_PM_WAKE_PACKET), &cbTotalToAlloc);
        if (!NT_SUCCESS(ntstatus))
        {
            status = NTSTATUS2NDISSTATUS(ntstatus);
            WFCPortError(
                "Error while trying to add cbTotalToAlloc(0x%x) and NDIS_PM_WAKE_PACKET(0x%x) size. Status = 0x%08x",
                cbTotalToAlloc,
                (ULONG)sizeof(NDIS_PM_WAKE_PACKET),
                status
                );
            goto Exit;
        }

        ntstatus = RtlULongAdd(cbTotalToAlloc, totalCbPadding, &cbTotalToAlloc);
        if (!NT_SUCCESS(ntstatus))
        {
            status = NTSTATUS2NDISSTATUS(ntstatus);
            WFCPortError(
                "Error while trying to add cbTotalToAlloc(0x%x) and totalCbPadding(0x%x). Status = 0x%08x",
                cbTotalToAlloc,
                totalCbPadding,
                status);
            goto Exit;
        }

        //
        // add actual packet space
        //
        ntstatus = RtlULongAdd(cbTotalToAlloc, parameter.WakePacket.ElementCount, &cbTotalToAlloc);
        if (!NT_SUCCESS(ntstatus))
        {
            status = NTSTATUS2NDISSTATUS(ntstatus);
            WFCPortError(
                "Error while trying to add cbTotalToAlloc (0x%x) and the actual packet space (0x%x). Status = 0x%08x",
                cbTotalToAlloc,
                parameter.WakePacket.ElementCount,
                status
                );
            goto Exit;
        }
    }

    pb = (BYTE*) ExAllocatePoolWithTag(
                    NonPagedPoolNx,
                    cbTotalToAlloc,
                    'cFiW' );

    if ( pb == NULL )
    {
        WFCPortError( "Failed to allocate wake reason indciation memory" );
        status = NDIS_STATUS_RESOURCES;
        goto Exit;
    }

    RtlZeroMemory( pb, cbTotalToAlloc );
    pNdisWakeReason = (PNDIS_PM_WAKE_REASON) pb;
    
    pNdisWakeReason->Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    pNdisWakeReason->Header.Revision = NDIS_PM_WAKE_REASON_REVISION_1,
    pNdisWakeReason->Header.Size = NDIS_SIZEOF_PM_WAKE_REASON_REVISION_1;

    // Remap the wake reasons that dont correspond to something that the OS understands
    switch(parameter.WakeEventCode)
    {
        case WDI_WAKE_REASON_CODE_GTK_HANDSHAKE_REQUEST:
            WFCPortInfo("Mapping WDI_WAKE_REASON_CODE_GTK_HANDSHAKE_REQUEST to WDI_WAKE_REASON_GTK_HANDSHAKE_ERROR");            
            pNdisWakeReason->WakeReason = NdisWakeReasonWlanGTKHandshakeError;
            break;
            
        case WDI_WAKE_REASON_CODE_MEDIA_DISCONNECT:
            //
            // hackhack, disconn should be AP loss
            //
            WFCPortInfo("Mapping WDI_WAKE_REASON_MEDIA_DISCONNECT to WDI_WAKE_REASON_AP_ASSOCIATION_LOST");            
            pNdisWakeReason->WakeReason = NdisWakeReasonWlanAPAssociationLost;
            break;
            
        default:
            pNdisWakeReason->WakeReason = (NDIS_PM_WAKE_REASON_TYPE)parameter.WakeEventCode;
            break;
    }


    if ( parameter.Optional.WakePacket_IsPresent )
    {
        NT_ASSERT( cbTotalToAlloc - sizeof( NDIS_PM_WAKE_REASON ) >=
            parameter.WakePacket.ElementCount + sizeof( NDIS_PM_WAKE_PACKET ) + cbPad1 + cbPad2 );

        //
        // patch up upper
        //
        pNdisWakeReason->InfoBufferSize = sizeof( NDIS_PM_WAKE_PACKET ) + cbPad2 + parameter.WakePacket.ElementCount;
        pNdisWakeReason->InfoBufferOffset = sizeof( NDIS_PM_WAKE_REASON ) + cbPad1;
        
        //
        // packet pattern wake needs NDIS_PM_WAKE_PACKET
        //
        PNDIS_PM_WAKE_PACKET pWakePacket;

        // move over NDIS_PM_WAKE_REASON
        pb += sizeof( NDIS_PM_WAKE_REASON ) + cbPad1;

        pWakePacket = (PNDIS_PM_WAKE_PACKET)pb;

        pWakePacket->Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
        pWakePacket->Header.Revision = NDIS_PM_WAKE_PACKET_REVISION_1;
        pWakePacket->Header.Size = NDIS_SIZEOF_PM_WAKE_PACKET_REVISION_1;

        pWakePacket->PatternId = parameter.WakePacketPatternId;
        pWakePacket->OriginalPacketSize = parameter.WakePacket.ElementCount;
        pWakePacket->SavedPacketSize = parameter.WakePacket.ElementCount;
        pWakePacket->SavedPacketOffset = sizeof( NDIS_PM_WAKE_PACKET ) + cbPad2;

        if (parameter.WakePacket.ElementCount > 0)
        {
            // move over wake packet descriptor
            pb += sizeof( NDIS_PM_WAKE_PACKET ) + cbPad2;
            RtlCopyMemory( pb, parameter.WakePacket.pElements, parameter.WakePacket.ElementCount );
        }
    }

    if ( pNdisWakeReason->WakeReason == NdisWakeReasonWlan4WayHandshakeRequest )
    {
        // NDIS does not expect the wake packet in this case. Clear the fields
        pNdisWakeReason->InfoBufferSize = 0;
        pNdisWakeReason->InfoBufferOffset = 0;
    }

    WFCPortInfo( "Indicating Wake Reason code 0x%x", pNdisWakeReason->WakeReason );
    m_pAdapter->IndicateStatus(
                m_NdisPortNumber,
                NDIS_STATUS_PM_WAKE_REASON,
                NULL,
                pNdisWakeReason,
                cbTotalToAlloc );

    ExFreePool( pNdisWakeReason );

Exit:

    TraceExit(status);
}


void CPort::OnTkipMicFailure(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS ndisStatus;
    WDI_INDICATION_TKIP_MIC_FAILURE_PARAMETERS Params;
    DOT11_TKIPMIC_FAILURE_PARAMETERS TkipMicFailureParams = { 0 };
    CConnectedPeer *pPeer = NULL;

    TraceEntry();

    WFCPortInfo("Got TKIP MIC failure from device.");

    ndisStatus = ParseWdiIndicationTkipMicFailure(
        EventBufferLength - sizeof( WFC_MESSAGE_METADATA ),
        EventBuffer + sizeof( WFC_MESSAGE_METADATA ),
        m_pAdapter->GetTlvContext( ),
        &Params);

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't retrieve parsed TKIP MIC failure parameters.");
        goto Exit;
    }    

    pPeer = m_pPeerList->FindPeerByAddress(Params.FailureInfo.PeerMacAddress.Address);
    if ((pPeer == NULL) ||
        (pPeer->GetAssociationState() != dot11_assoc_state_auth_assoc))
    {
        WFCPortInfo("Not currently connected to %!MACADDR!, ignoring TKIP MIC failure indication",
            Params.FailureInfo.PeerMacAddress.Address );
        goto Exit;
    }

    ASSIGN_NDIS_OBJECT_HEADER(
        TkipMicFailureParams.Header, 
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_TKIPMIC_FAILURE_PARAMETERS_REVISION_1,
        sizeof(DOT11_TKIPMIC_FAILURE_PARAMETERS) );

    TkipMicFailureParams.bDefaultKeyFailure = Params.FailureInfo.DefaultKeyFailure;
    TkipMicFailureParams.uKeyIndex = Params.FailureInfo.KeyIndex;
    RtlCopyMemory( &TkipMicFailureParams.PeerMac, &Params.FailureInfo.PeerMacAddress.Address, sizeof(DOT11_MAC_ADDRESS) );

    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_DOT11_TKIPMIC_FAILURE,
        NULL,
        &TkipMicFailureParams,
        sizeof(DOT11_TKIPMIC_FAILURE_PARAMETERS) );

Exit:
    TraceExit(NDIS_STATUS_SUCCESS);
}


void CPort::OnIncomingAssociationRequestReceived(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NTSTATUS ntstatus;
    WDI_INDICATION_AP_ASSOCIATION_REQUEST_PARAMETERS assocRequestParams;
    BYTE* pIndicationBuffer = NULL;
    DOT11_INCOMING_ASSOC_REQUEST_RECEIVED_PARAMETERS* pIncomingAssocIndication;
    ULONG indicationSize = sizeof(*pIncomingAssocIndication);
    DOT11_INCOMING_ASSOC_STARTED_PARAMETERS AssocStartIndication;
    CConnectedPeer *pPeer = NULL;

    TraceEntry();

    status = ParseWdiIndicationApAssociationRequestReceived(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &assocRequestParams );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't parse incoming association request parameters. 0x%x", status);
        goto exit;
    }

    //
    // We could receive a new association request, but not have received a disassociation from a previous
    // association request from this peer.  If so, we need to simulate a disassociation for the upper layers
    //
    pPeer = m_pPeerList->FindPeerByAddress(assocRequestParams.IncomingRequestInfo.AssocRequestParams.PeerMacAddress.Address);
    if (pPeer) 
    {
        WFCPortTrace("Assoc request received for existing peer = %!MACADDR! - Simulating disassociation", 
            (assocRequestParams.IncomingRequestInfo.AssocRequestParams.PeerMacAddress.Address));

        this->IndicateSinglePeerDisassociated(
            pPeer,
            DOT11_ASSOC_STATUS_DISASSOCIATED_BY_OS,
            WDI_ASSOC_STATUS_PEER_DISASSOCIATED );

        //reset FIPS mode - queried on first Rx packet of next association
        m_FIPSMode = WDI_FIPS_MODE_UNKNOWN;

        m_pPeerList->DeletePeer(assocRequestParams.IncomingRequestInfo.AssocRequestParams.PeerMacAddress.Address);

        // If someone is registered, notify them about the disassociation
        if (m_pDisassociationCompleteCallback)
        {
            m_pDisassociationCompleteCallback->OnDisassociationProcessed();
        }
    }

    ntstatus = RtlULongAdd(indicationSize, assocRequestParams.IncomingRequestInfo.AssocRequestFrame.ElementCount, &indicationSize);
    if (!NT_SUCCESS(ntstatus))
    {
        status = NTSTATUS2NDISSTATUS(ntstatus);
        WFCPortError(
            "Integer Overflow while adding AssocRequestFrame (0x%x) size to indicationSize (0x%x). Error = 0x%08x", 
            assocRequestParams.IncomingRequestInfo.AssocRequestFrame.ElementCount,
            indicationSize,
            status
            );
        goto exit;
    }

    pIndicationBuffer = new BYTE[indicationSize];

    if ( pIndicationBuffer == NULL )
    {
        WFCPortError("Couldn't allocate space for incoming association request indication.");
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    ASSIGN_NDIS_OBJECT_HEADER(
        AssocStartIndication.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_INCOMING_ASSOC_STARTED_PARAMETERS_REVISION_1,
        sizeof(DOT11_INCOMING_ASSOC_STARTED_PARAMETERS) );

    RtlCopyMemory(
        &AssocStartIndication.PeerMacAddr,
        &assocRequestParams.IncomingRequestInfo.AssocRequestParams.PeerMacAddress,
        sizeof(AssocStartIndication.PeerMacAddr));

    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_DOT11_INCOMING_ASSOC_STARTED,
        NULL,
        &AssocStartIndication,
        sizeof(AssocStartIndication) );

    pIncomingAssocIndication = (DOT11_INCOMING_ASSOC_REQUEST_RECEIVED_PARAMETERS*)pIndicationBuffer;

    ASSIGN_NDIS_OBJECT_HEADER(
        pIncomingAssocIndication->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_INCOMING_ASSOC_REQUEST_RECEIVED_PARAMETERS_REVISION_1,
        sizeof(DOT11_INCOMING_ASSOC_REQUEST_RECEIVED_PARAMETERS) );

    RtlCopyMemory(
        &pIncomingAssocIndication->PeerMacAddr,
        &assocRequestParams.IncomingRequestInfo.AssocRequestParams.PeerMacAddress,
        sizeof(pIncomingAssocIndication->PeerMacAddr));

    pIncomingAssocIndication->bReAssocReq =
        assocRequestParams.IncomingRequestInfo.AssocRequestParams.IsReassociationRequest;

    pIncomingAssocIndication->uAssocReqOffset = sizeof(*pIncomingAssocIndication);
    pIncomingAssocIndication->uAssocReqSize =
        assocRequestParams.IncomingRequestInfo.AssocRequestFrame.ElementCount;

    RtlCopyMemory(
        pIndicationBuffer + pIncomingAssocIndication->uAssocReqOffset,
        assocRequestParams.IncomingRequestInfo.AssocRequestFrame.pElements,
        pIncomingAssocIndication->uAssocReqSize );

    if ( m_pAdapter->GetAdapterPropertyCache()->SetPropertyBuffer(
             WfcAdapterProperty_LastAssociationRequestEventBuffer,
             EventBufferLength,
             EventBuffer ) != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't store last association request event buffer. Association will fail.");
        NT_ASSERT(FALSE);
    }

    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_DOT11_INCOMING_ASSOC_REQUEST_RECEIVED,
        NULL,
        pIndicationBuffer,
        indicationSize );

exit:

    if ( pIndicationBuffer != NULL )
    {
        delete [] pIndicationBuffer;
    }

    TraceExit(status);
}


void CPort::OnIncomingP2PActionFrameReceived(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NTSTATUS ntstatus;
    WDI_INDICATION_P2P_ACTION_FRAME_RECEIVED_PARAMETERS actionFrameParams;
    BYTE* pIndicationBuffer = NULL;
    ULONG indicationSize;
    NDIS_STATUS statusToIndicate;
    WFD_OTA_CHANNEL p2pChannelInfo;
    DOT11_WFD_GROUP_ID p2pGroupID = { {0}, { 7, "DIRECT-" } };
    DOT11_MAC_ADDRESS p2pBSSID = {0};
    bool channelHintAvailable = false;

    TraceEntry();

    status = ParseWdiIndicationP2pActionFrameReceived(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &actionFrameParams );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't parse incoming p2p action frame parameters. 0x%x", status);
        goto exit;
    }


    //
    // Fill in channel hint structures
    //
    RtlCopyMemory(
        &p2pGroupID.DeviceAddress,
        &actionFrameParams.FrameInfo.FrameParams.PeerDeviceAddress,
        sizeof(p2pGroupID.DeviceAddress) );

    RtlCopyMemory(
        &p2pBSSID,
        &actionFrameParams.FrameInfo.FrameParams.PeerDeviceAddress,
        sizeof(p2pBSSID) );

        
    switch(actionFrameParams.FrameInfo.FrameParams.ActionFrameType)
    {
    case WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_REQUEST:
        statusToIndicate = NDIS_STATUS_DOT11_WFD_RECEIVED_GO_NEGOTIATION_REQUEST;
        indicationSize = sizeof(DOT11_RECEIVED_GO_NEGOTIATION_REQUEST_PARAMETERS);
        break;
    
    case WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_RESPONSE:
        statusToIndicate = NDIS_STATUS_DOT11_WFD_RECEIVED_GO_NEGOTIATION_RESPONSE;
        indicationSize = sizeof(DOT11_RECEIVED_GO_NEGOTIATION_RESPONSE_PARAMETERS);
        break;
    
    case WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_CONFIRM:
        statusToIndicate = NDIS_STATUS_DOT11_WFD_RECEIVED_GO_NEGOTIATION_CONFIRMATION;
        indicationSize = sizeof(DOT11_RECEIVED_GO_NEGOTIATION_CONFIRMATION_PARAMETERS);
        break;
    
    case WDI_P2P_ACTION_FRAME_INVITATION_REQUEST:
        statusToIndicate = NDIS_STATUS_DOT11_WFD_RECEIVED_INVITATION_REQUEST;
        indicationSize = sizeof(DOT11_RECEIVED_INVITATION_REQUEST_PARAMETERS);
        break;
    
    case WDI_P2P_ACTION_FRAME_INVITATION_RESPONSE:
        statusToIndicate = NDIS_STATUS_DOT11_WFD_RECEIVED_INVITATION_RESPONSE;
        indicationSize = sizeof(DOT11_RECEIVED_INVITATION_RESPONSE_PARAMETERS);
        break;
    
    case WDI_P2P_ACTION_FRAME_PROVISION_DISCOVERY_REQUEST:
        statusToIndicate = NDIS_STATUS_DOT11_WFD_RECEIVED_PROVISION_DISCOVERY_REQUEST;
        indicationSize = sizeof(DOT11_RECEIVED_PROVISION_DISCOVERY_REQUEST_PARAMETERS);
        break;
    
    case WDI_P2P_ACTION_FRAME_PROVISION_DISCOVERY_RESPONSE:
        statusToIndicate = NDIS_STATUS_DOT11_WFD_RECEIVED_PROVISION_DISCOVERY_RESPONSE;
        indicationSize = sizeof(DOT11_RECEIVED_PROVISION_DISCOVERY_RESPONSE_PARAMETERS);
        break;

    default:
        WFCPortError("Unknown P2P action frame type provided.");
        NT_ASSERT(FALSE);
        goto exit;
    }

    ntstatus = RtlULongAdd(indicationSize, actionFrameParams.FrameInfo.FrameIEs.ElementCount, &indicationSize);
    if (!NT_SUCCESS(ntstatus))
    {
        status = NTSTATUS2NDISSTATUS(ntstatus);
        WFCPortError("Error: Integer Overflow while adding FrameIEs(0x%x) size to indicationSize(0x%x). Status = 0x%08x",
            actionFrameParams.FrameInfo.FrameIEs.ElementCount,
            indicationSize,
            status
            );
        goto exit;
    }

    pIndicationBuffer = new BYTE[indicationSize];

    if ( pIndicationBuffer == NULL )
    {
        WFCPortError("Couldn't allocate space for incoming association request indication.");
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    switch(actionFrameParams.FrameInfo.FrameParams.ActionFrameType)
    {
    case WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_REQUEST:
    {
        DOT11_RECEIVED_GO_NEGOTIATION_REQUEST_PARAMETERS* pIndication =
            (DOT11_RECEIVED_GO_NEGOTIATION_REQUEST_PARAMETERS*)pIndicationBuffer;

        ASSIGN_NDIS_OBJECT_HEADER(
            pIndication->Header,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_RECEIVED_GO_NEGOTIATION_REQUEST_PARAMETERS_REVISION_1,
            DOT11_SIZEOF_RECEIVED_GO_NEGOTIATION_REQUEST_PARAMETERS_REVISION_1 );

        pIndication->DialogToken =
            actionFrameParams.FrameInfo.FrameParams.DialogToken;

        RtlCopyMemory(
            pIndication->PeerDeviceAddress,
            &actionFrameParams.FrameInfo.FrameParams.PeerDeviceAddress,
            sizeof(pIndication->PeerDeviceAddress));

        pIndication->uIEsOffset = sizeof(*pIndication);
        pIndication->uIEsLength = actionFrameParams.FrameInfo.FrameIEs.ElementCount;

        RtlCopyMemory(
            pIndicationBuffer + pIndication->uIEsOffset,
            actionFrameParams.FrameInfo.FrameIEs.pElements,
            pIndication->uIEsLength );

        if ( NT_SUCCESS(
                 WFDGetListenChannel(
                     actionFrameParams.FrameInfo.FrameIEs.pElements,
                     actionFrameParams.FrameInfo.FrameIEs.ElementCount,
                     &p2pChannelInfo ) ) )
        {
            channelHintAvailable = true;
        }

    }
    break;
        
    case WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_RESPONSE:
    {
        DOT11_RECEIVED_GO_NEGOTIATION_RESPONSE_PARAMETERS* pIndication =
            (DOT11_RECEIVED_GO_NEGOTIATION_RESPONSE_PARAMETERS*)pIndicationBuffer;

        ASSIGN_NDIS_OBJECT_HEADER(
            pIndication->Header,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_RECEIVED_GO_NEGOTIATION_RESPONSE_PARAMETERS_REVISION_1,
            DOT11_SIZEOF_RECEIVED_GO_NEGOTIATION_RESPONSE_PARAMETERS_REVISION_1 );

        pIndication->DialogToken =
            actionFrameParams.FrameInfo.FrameParams.DialogToken;

        RtlCopyMemory(
            pIndication->PeerDeviceAddress,
            &actionFrameParams.FrameInfo.FrameParams.PeerDeviceAddress,
            sizeof(pIndication->PeerDeviceAddress));

        pIndication->uIEsOffset = sizeof(*pIndication);
        pIndication->uIEsLength = actionFrameParams.FrameInfo.FrameIEs.ElementCount;

        RtlCopyMemory(
            pIndicationBuffer + pIndication->uIEsOffset,
            actionFrameParams.FrameInfo.FrameIEs.pElements,
            pIndication->uIEsLength );
    }
    break;
    
    case WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_CONFIRM:
    {
        DOT11_RECEIVED_GO_NEGOTIATION_CONFIRMATION_PARAMETERS* pIndication =
            (DOT11_RECEIVED_GO_NEGOTIATION_CONFIRMATION_PARAMETERS*)pIndicationBuffer;

        ASSIGN_NDIS_OBJECT_HEADER(
            pIndication->Header,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_RECEIVED_GO_NEGOTIATION_CONFIRMATION_PARAMETERS_REVISION_1,
            DOT11_SIZEOF_RECEIVED_GO_NEGOTIATION_CONFIRMATION_PARAMETERS_REVISION_1 );

        pIndication->DialogToken =
            actionFrameParams.FrameInfo.FrameParams.DialogToken;

        RtlCopyMemory(
            pIndication->PeerDeviceAddress,
            &actionFrameParams.FrameInfo.FrameParams.PeerDeviceAddress,
            sizeof(pIndication->PeerDeviceAddress));

        pIndication->uIEsOffset = sizeof(*pIndication);
        pIndication->uIEsLength = actionFrameParams.FrameInfo.FrameIEs.ElementCount;

        RtlCopyMemory(
            pIndicationBuffer + pIndication->uIEsOffset,
            actionFrameParams.FrameInfo.FrameIEs.pElements,
            pIndication->uIEsLength );
    }
    break;
    
    case WDI_P2P_ACTION_FRAME_INVITATION_REQUEST:
    {
        WFD_OTA_GROUP_ID groupID;
        ULONG groupIDNameLength;

        DOT11_RECEIVED_INVITATION_REQUEST_PARAMETERS* pIndication =
            (DOT11_RECEIVED_INVITATION_REQUEST_PARAMETERS*)pIndicationBuffer;

        ASSIGN_NDIS_OBJECT_HEADER(
            pIndication->Header,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_RECEIVED_INVITATION_REQUEST_PARAMETERS_REVISION_1,
            DOT11_SIZEOF_RECEIVED_INVITATION_REQUEST_PARAMETERS_REVISION_1 );

        pIndication->DialogToken =
            actionFrameParams.FrameInfo.FrameParams.DialogToken;

        RtlCopyMemory(
            pIndication->TransmitterDeviceAddress,
            &actionFrameParams.FrameInfo.FrameParams.PeerDeviceAddress,
            sizeof(pIndication->TransmitterDeviceAddress));

        //
        // TODO FILL IN BSSID - WABI SPEC UPDATE
        //
        
        pIndication->uIEsOffset = sizeof(*pIndication);
        pIndication->uIEsLength = actionFrameParams.FrameInfo.FrameIEs.ElementCount;

        RtlCopyMemory(
            pIndicationBuffer + pIndication->uIEsOffset,
            actionFrameParams.FrameInfo.FrameIEs.pElements,
            pIndication->uIEsLength );

        if ( NT_SUCCESS(
                 WFDGetOperatingChannel(
                     actionFrameParams.FrameInfo.FrameIEs.pElements,
                     actionFrameParams.FrameInfo.FrameIEs.ElementCount,
                     &p2pChannelInfo ) ) &&
             NT_SUCCESS(
                 WFDGetGroupID(
                     actionFrameParams.FrameInfo.FrameIEs.pElements,
                     actionFrameParams.FrameInfo.FrameIEs.ElementCount,
                     &groupIDNameLength,
                     &groupID ) ) &&
             NT_SUCCESS(
                 WFDConvertGroupIDFromOTAtoDot11(
                     &groupID,
                     groupIDNameLength,
                     &p2pGroupID ) ) &&
             NT_SUCCESS(
                 WFDGetBSSID(
                     actionFrameParams.FrameInfo.FrameIEs.pElements,
                     actionFrameParams.FrameInfo.FrameIEs.ElementCount,
                     &p2pBSSID ) ) )
        {
            channelHintAvailable = true;
        }
    }
    break;
    
    case WDI_P2P_ACTION_FRAME_INVITATION_RESPONSE:
    {
        DOT11_RECEIVED_INVITATION_RESPONSE_PARAMETERS* pIndication =
            (DOT11_RECEIVED_INVITATION_RESPONSE_PARAMETERS*)pIndicationBuffer;

        ASSIGN_NDIS_OBJECT_HEADER(
            pIndication->Header,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_RECEIVED_INVITATION_RESPONSE_PARAMETERS_REVISION_1,
            DOT11_SIZEOF_RECEIVED_INVITATION_RESPONSE_PARAMETERS_REVISION_1 );

        pIndication->DialogToken =
            actionFrameParams.FrameInfo.FrameParams.DialogToken;

        RtlCopyMemory(
            pIndication->TransmitterDeviceAddress,
            &actionFrameParams.FrameInfo.FrameParams.PeerDeviceAddress,
            sizeof(pIndication->TransmitterDeviceAddress));

        //
        // TODO FILL IN BSSID - WABI SPEC UPDATE
        //
        
        pIndication->uIEsOffset = sizeof(*pIndication);
        pIndication->uIEsLength = actionFrameParams.FrameInfo.FrameIEs.ElementCount;

        RtlCopyMemory(
            pIndicationBuffer + pIndication->uIEsOffset,
            actionFrameParams.FrameInfo.FrameIEs.pElements,
            pIndication->uIEsLength );

    }
    break;
    
    case WDI_P2P_ACTION_FRAME_PROVISION_DISCOVERY_REQUEST:
    {
        DOT11_RECEIVED_PROVISION_DISCOVERY_REQUEST_PARAMETERS* pIndication =
            (DOT11_RECEIVED_PROVISION_DISCOVERY_REQUEST_PARAMETERS*)pIndicationBuffer;

        ASSIGN_NDIS_OBJECT_HEADER(
            pIndication->Header,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_RECEIVED_PROVISION_DISCOVERY_REQUEST_PARAMETERS_REVISION_1,
            DOT11_SIZEOF_RECEIVED_PROVISION_DISCOVERY_REQUEST_PARAMETERS_REVISION_1 );

        pIndication->DialogToken =
            actionFrameParams.FrameInfo.FrameParams.DialogToken;

        RtlCopyMemory(
            pIndication->TransmitterDeviceAddress,
            &actionFrameParams.FrameInfo.FrameParams.PeerDeviceAddress,
            sizeof(pIndication->TransmitterDeviceAddress));

        //
        // TODO FILL IN BSSID - WABI SPEC UPDATE
        //
        
        pIndication->uIEsOffset = sizeof(*pIndication);
        pIndication->uIEsLength = actionFrameParams.FrameInfo.FrameIEs.ElementCount;

        RtlCopyMemory(
            pIndicationBuffer + pIndication->uIEsOffset,
            actionFrameParams.FrameInfo.FrameIEs.pElements,
            pIndication->uIEsLength );
    }
    break;
    
    case WDI_P2P_ACTION_FRAME_PROVISION_DISCOVERY_RESPONSE:
    {
        DOT11_RECEIVED_PROVISION_DISCOVERY_RESPONSE_PARAMETERS* pIndication =
            (DOT11_RECEIVED_PROVISION_DISCOVERY_RESPONSE_PARAMETERS*)pIndicationBuffer;

        ASSIGN_NDIS_OBJECT_HEADER(
            pIndication->Header,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_RECEIVED_PROVISION_DISCOVERY_RESPONSE_PARAMETERS_REVISION_1,
            DOT11_SIZEOF_RECEIVED_PROVISION_DISCOVERY_RESPONSE_PARAMETERS_REVISION_1 );

        pIndication->DialogToken =
            actionFrameParams.FrameInfo.FrameParams.DialogToken;

        RtlCopyMemory(
            pIndication->TransmitterDeviceAddress,
            &actionFrameParams.FrameInfo.FrameParams.PeerDeviceAddress,
            sizeof(pIndication->TransmitterDeviceAddress));

        //
        // TODO FILL IN BSSID - WABI SPEC UPDATE
        //
        
        pIndication->uIEsOffset = sizeof(*pIndication);
        pIndication->uIEsLength = actionFrameParams.FrameInfo.FrameIEs.ElementCount;

        RtlCopyMemory(
            pIndicationBuffer + pIndication->uIEsOffset,
            actionFrameParams.FrameInfo.FrameIEs.pElements,
            pIndication->uIEsLength );
    }
    break;

    default:
        WFCPortError("Unknown P2P action frame type provided.");
        NT_ASSERT(FALSE);
        goto exit;
    }

    //
    // TODO: Figure out a more robust store/recall method for last p2p
    // action frame.
    //
    if ( m_pAdapter->GetAdapterPropertyCache()->SetPropertyBuffer(
             WfcAdapterProperty_LastActionFrameIndicationEventBuffer,
             EventBufferLength,
             EventBuffer ) != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't store last action frame event buffer. P2P action frame send will fail.");
        NT_ASSERT(FALSE);
    }

    if ( channelHintAvailable )
    {
        // TODO: Move this block into a helper function
        CBSSListManager* pBSSListManager = m_pAdapter->GetBSSListManager(WfcPortTypeWFDDevice);
        CBSSEntry* pDeviceInfo = pBSSListManager->FindBSSEntryByGroupID(
                p2pGroupID,
                true );         // Match DeviceAddress in addition to SSID

        if ( pDeviceInfo == NULL )
        {
            pDeviceInfo = new CBSSEntry(p2pBSSID);

            if ( pDeviceInfo != NULL )
            {
                pDeviceInfo->SetGroupID( &p2pGroupID ); 
                if ( pBSSListManager->AddBSSEntry(
                             pDeviceInfo ) != NDIS_STATUS_SUCCESS )
                {
                    delete pDeviceInfo;
                    pDeviceInfo = NULL;
                }
            }                    
        }

        if ( pDeviceInfo != NULL )
        {
            UINT32 bandID, channelNumber;

            if ( ConvertP2PChannelToBandChannel(
                     m_pAdapter->GetAdapterPropertyCache(),
                     &p2pChannelInfo,
                     &bandID,
                     &channelNumber ) == NDIS_STATUS_SUCCESS )
            {
                pDeviceInfo->SetChannelAndPhyID(
                    false,
                    m_pAdapter->GetAdapterPropertyCache(),
                    channelNumber,
                    bandID
                    );
            }

            if (pBSSListManager->IsBackgroundDiscoveryEnabled())
            {
                pDeviceInfo->UpdateBackgroundDeviceIndicatedByDriver( m_pAdapter );
            }
        }
    }

    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        statusToIndicate,
        NULL,
        pIndicationBuffer,
        indicationSize );

exit:

    if ( pIndicationBuffer != NULL )
    {
        delete [] pIndicationBuffer;
    }

    TraceExit(status);
}


void CPort::OnOperatingChannelIndication(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_INDICATION_P2P_GROUP_OPERATING_CHANNEL_PARAMETERS opChannelInfo;
    DOT11_WFD_GROUP_OPERATING_CHANNEL_UPDATE_PARAMETERS ChannelUpdate = { 0 };

    TraceEntry();

    status = ParseWdiIndicationP2pGroupOperatingChannel(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &opChannelInfo );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't parse group operating channel parameters. 0x%x", status);
        goto exit;
    }

#ifdef OLDER_OS_COMPAT
    {
        DOT11_WFD_CHANNEL OpChannel = { 0 };

        //
        // Older OS'es do not handle the NDIS_STATUS_DOT11_WFD_GROUP_OPERATING_CHANNEL_UPDATE
        // notification yet, so we have to send the prior notification as well
        //
        OpChannel.ChannelNumber = (UCHAR)opChannelInfo.Channel.ChannelNumber;
        memcpy(
            OpChannel.CountryRegionString,
            opChannelInfo.Channel.CountryOrRegionString,
            sizeof( DOT11_COUNTRY_OR_REGION_STRING ) );
        OpChannel.OperatingClass = opChannelInfo.Channel.OperatingClass;

        m_pAdapter->IndicateStatus(
            m_NdisPortNumber,
            NDIS_STATUS_DOT11_WFD_GROUP_OPERATING_CHANNEL,
            NULL,
            (BYTE*)&OpChannel,
            sizeof(OpChannel) );
    }
#endif  // OLDER_OS_COMPAT

    ChannelUpdate.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    ChannelUpdate.Header.Revision = DOT11_WFD_GROUP_OPERATING_CHANNEL_UPDATE_PARAMETERS_REVISION_1;
    ChannelUpdate.Header.Size = DOT11_SIZEOF_WFD_GROUP_OPERATING_CHANNEL_UPDATE_PARAMETERS_REVISION_1;

    // WFD uses UCHAR instead of UINT32, like everybody else
    ChannelUpdate.WFDChannel.ChannelNumber = (UCHAR) opChannelInfo.Channel.ChannelNumber;
    memcpy( ChannelUpdate.WFDChannel.CountryRegionString, opChannelInfo.Channel.CountryOrRegionString, sizeof( DOT11_COUNTRY_OR_REGION_STRING ) );
    ChannelUpdate.WFDChannel.OperatingClass = opChannelInfo.Channel.OperatingClass;

    ChannelUpdate.Reason = CWabiToDot11Converter::MapP2PChannelIndicateReason( opChannelInfo.IndicateReason );

    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_DOT11_WFD_GROUP_OPERATING_CHANNEL_UPDATE,
        NULL,
        (BYTE*)&ChannelUpdate,
        sizeof(ChannelUpdate) );

exit:

    TraceExit(status);
}


void CPort::OnOperatingChannelAttributesIndication(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NTSTATUS ntstatus;
    DWORD i, j, uBufferLength, dwLength = 0;

    BYTE* pIndicationBuffer = NULL;
    WDI_INDICATION_P2P_OPERATING_CHANNEL_ATTRIBUTES_PARAMETERS WdiChannelAttributes;
    WDI_P2P_CHANNEL_ENTRIES_CONTAINER  *pWdiChannelEntry;
    DOT11_WFD_OPERATING_CHANNEL_ATTRIBUTES *pDot11ChannelAttributes;
    DOT11_WFD_CHANNEL_ENTRY *pDot11ChannelEntry;
    PBYTE   pByte;

    TraceEntry();

    WFCPortInfo("\t=====> Got OperatingChannelAttributes on Port <%d> <=====", m_NdisPortNumber);

    status = ParseWdiIndicationP2pOperatingChannelAttributes(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &WdiChannelAttributes );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't parse operating channel attributes parameters. 0x%x", status);
        goto exit;
    }

    pByte = (PBYTE) WdiChannelAttributes.PreferredOperatingChannel.CountryOrRegionString;
    WFCPortInfo("\tOperatingChannel:\n");
    WFCPortInfo("\t\tPreferred Country=<0x%02x:0x%02x:0x%02x>, Class:Channel=<0x%02x:%d>",
        pByte[0], pByte[1], pByte[2], WdiChannelAttributes.PreferredOperatingChannel.OperatingClass,
        WdiChannelAttributes.PreferredOperatingChannel.ChannelNumber);

    //
    // This is a variable length struct, so determine the length before copying the data
    //
    pByte = (PBYTE) &WdiChannelAttributes.ChannelList.CountryRegionList.pElements[0];
    WFCPortInfo("\tChannelList:\n");
    WFCPortInfo("\t\tAttributes Country=<0x%02x:0x%02x:0x%02x>, %d Channels",
        pByte[0], pByte[1], pByte[2], WdiChannelAttributes.ChannelList.ChannelEntries.ElementCount);

    uBufferLength = 0;
    for (i=0; i<WdiChannelAttributes.ChannelList.ChannelEntries.ElementCount; i++)
    {
        pWdiChannelEntry = &WdiChannelAttributes.ChannelList.ChannelEntries.pElements[i];

        //Add: uBufferLength += FIELD_OFFSET(DOT11_WFD_CHANNEL_ENTRY,ChannelNumbers)
        ntstatus = RtlULongAdd(
            uBufferLength,
            FIELD_OFFSET(DOT11_WFD_CHANNEL_ENTRY, ChannelNumbers),
            &uBufferLength
            );
        if (!NT_SUCCESS(ntstatus))
        {
            status = NTSTATUS2NDISSTATUS(ntstatus);
            WFCPortError(
                "Error: Integer Overflow while adding ChannelNumbers (0x%x) to uBufferLength (0x%x), Error = 0x%08x",
                FIELD_OFFSET(DOT11_WFD_CHANNEL_ENTRY, ChannelNumbers),
                uBufferLength,
                status
                );
            goto exit;
        }

        C_ASSERT(sizeof(UCHAR) == 1);

        ntstatus = RtlULongAdd(
            uBufferLength,
            (ULONG)(pWdiChannelEntry->ChannelList.ElementCount * sizeof(UCHAR)),
            &uBufferLength
            );

        if (!NT_SUCCESS(ntstatus))
        {
            status = NTSTATUS2NDISSTATUS(ntstatus);
            WFCPortError(
                "Error: Integer Overflow while adding ChannelNumbers (0x%x) to uBufferLength (0x%x)",
                (pWdiChannelEntry->ChannelList.ElementCount * sizeof(UCHAR)),
                uBufferLength
                );
            goto exit;
        }
    }

    ntstatus = RtlULongAdd( FIELD_OFFSET(DOT11_WFD_OPERATING_CHANNEL_ATTRIBUTES, ChannelList.ChannelEntries),
                            uBufferLength,
                            &dwLength );
    if (!NT_SUCCESS(ntstatus))
    {
        status = NTSTATUS2NDISSTATUS(ntstatus);
        WFCPortError(
            "Error: Integer Overflow while adding uBufferLength (0x%x) to ChannelAttributes (0x%x)",
            uBufferLength, sizeof(DOT11_WFD_OPERATING_CHANNEL_ATTRIBUTES));
        goto exit;
    }
    pIndicationBuffer = new BYTE[dwLength];
    if ( pIndicationBuffer == NULL )
    {
        WFCPortError("Couldn't allocate space for incoming channel attributes indication.");
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    pDot11ChannelAttributes = (DOT11_WFD_OPERATING_CHANNEL_ATTRIBUTES *) pIndicationBuffer;
    pDot11ChannelAttributes->Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    pDot11ChannelAttributes->Header.Revision = DOT11_WFD_OPERATING_CHANNEL_ATTRIBUTES_UPDATED_REVISION_1;
    pDot11ChannelAttributes->Header.Size = DOT11_SIZEOF_WFD_OPERATING_CHANNEL_ATTRIBUTES_UPDATED_REVISION_1;

    RtlCopyMemory (&pDot11ChannelAttributes->PreferredOperatingChannel.CountryRegionString,
                   WdiChannelAttributes.PreferredOperatingChannel.CountryOrRegionString,
                   sizeof(DOT11_COUNTRY_OR_REGION_STRING));
    pDot11ChannelAttributes->PreferredOperatingChannel.OperatingClass = WdiChannelAttributes.PreferredOperatingChannel.OperatingClass;
    pDot11ChannelAttributes->PreferredOperatingChannel.ChannelNumber = (UCHAR) WdiChannelAttributes.PreferredOperatingChannel.ChannelNumber;

    RtlCopyMemory(&pDot11ChannelAttributes->PreferredListenChannel.CountryRegionString,
        WdiChannelAttributes.PreferredListenChannel.CountryOrRegionString,
        sizeof(DOT11_COUNTRY_OR_REGION_STRING));
    pDot11ChannelAttributes->PreferredListenChannel.OperatingClass = WdiChannelAttributes.PreferredListenChannel.OperatingClass;
    pDot11ChannelAttributes->PreferredListenChannel.ChannelNumber = (UCHAR)WdiChannelAttributes.PreferredListenChannel.ChannelNumber;

    RtlCopyMemory (&pDot11ChannelAttributes->ChannelList.CountryRegionString,
                   &WdiChannelAttributes.ChannelList.CountryRegionList.pElements[0],
                   sizeof(DOT11_COUNTRY_OR_REGION_STRING));

    pDot11ChannelEntry = pDot11ChannelAttributes->ChannelList.ChannelEntries;
    for (i=0; i<WdiChannelAttributes.ChannelList.ChannelEntries.ElementCount; i++)
    {
        pWdiChannelEntry = &WdiChannelAttributes.ChannelList.ChannelEntries.pElements[i];

        pDot11ChannelEntry->OperatingClass = pWdiChannelEntry->OperatingClass;
        pDot11ChannelEntry->uNumChannels = pWdiChannelEntry->ChannelList.ElementCount;
        for (j=0; j<pDot11ChannelEntry->uNumChannels; j++)
        {
            pDot11ChannelEntry->ChannelNumbers[j] = (UCHAR) pWdiChannelEntry->ChannelList.pElements[j];
        }

        pByte = (PBYTE) &WdiChannelAttributes.ChannelList.ChannelEntries.pElements[i];
        WFCPortInfo("\t\t\t[%d] OperatingClass=<0x%02x>, NumChannels=<%d>, <%d %d %d ...>",
            i, pDot11ChannelEntry->OperatingClass, pDot11ChannelEntry->uNumChannels,
            (pDot11ChannelEntry->uNumChannels > 0 ? pDot11ChannelEntry->ChannelNumbers[0] : 0),
            (pDot11ChannelEntry->uNumChannels > 1 ? pDot11ChannelEntry->ChannelNumbers[1] : 0),
            (pDot11ChannelEntry->uNumChannels > 2 ? pDot11ChannelEntry->ChannelNumbers[2] : 0));

        //
        // Set the next pDot11ChannelEntry
        //
        pDot11ChannelEntry = (PDOT11_WFD_CHANNEL_ENTRY) (((PBYTE) pDot11ChannelEntry) +
                                                         FIELD_OFFSET(DOT11_WFD_CHANNEL_ENTRY,ChannelNumbers) +
                                                         (pWdiChannelEntry->ChannelList.ElementCount * sizeof(UCHAR)));
    }

    pDot11ChannelAttributes->ChannelList.uNumChannelEntries = WdiChannelAttributes.ChannelList.ChannelEntries.ElementCount;
    pDot11ChannelAttributes->ChannelList.uBufferLength = uBufferLength;

    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_DOT11_WFD_OPERATING_CHANNEL_ATTRIBUTES_UPDATED,
        NULL,
        (BYTE*) pDot11ChannelAttributes,
        dwLength);

exit:

    if ( pIndicationBuffer != NULL )
    {
        delete [] pIndicationBuffer;
    }

    TraceExit(status);
}


void CPort::OnStopAPReceived(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS                                 status = NDIS_STATUS_SUCCESS;
    WDI_INDICATION_STOP_AP_PARAMETERS           WdiStopParams;
    DOT11_STOP_AP_PARAMETERS                    Dot11StopParams = {0};

    TraceEntry();

    status = ParseWdiIndicationStopAp(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &WdiStopParams );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't parse StopAP indication parameters. 0x%x", status);
        goto exit;
    }

    Dot11StopParams.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    Dot11StopParams.Header.Revision = DOT11_STOP_AP_PARAMETERS_REVISION_1;
    Dot11StopParams.Header.Size = sizeof(DOT11_STOP_AP_PARAMETERS);
    Dot11StopParams.ulReason = WdiStopParams.Reason;

    WFCPortInfo("StopAP reason=<0x%x>\n", Dot11StopParams.ulReason);

    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_DOT11_STOP_AP,
        NULL,
        (BYTE*)&Dot11StopParams,
        sizeof(Dot11StopParams) );

exit:

    TraceExit(status);
}


void CPort::OnCanSustainAPReceived(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS                                 status = NDIS_STATUS_SUCCESS;
    WDI_INDICATION_CAN_SUSTAIN_AP_PARAMETERS    WdiSustainParams;
    DOT11_CAN_SUSTAIN_AP_PARAMETERS             Dot11SustainParams = {0};

    TraceEntry();

    status = ParseWdiIndicationCanSustainAp(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &WdiSustainParams );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("Couldn't parse SustainAP indication parameters. 0x%x", status);
        goto exit;
    }

    Dot11SustainParams.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    Dot11SustainParams.Header.Revision = DOT11_CAN_SUSTAIN_AP_PARAMETERS_REVISION_1;
    Dot11SustainParams.Header.Size = sizeof(DOT11_CAN_SUSTAIN_AP_PARAMETERS);
    Dot11SustainParams.ulReason = WdiSustainParams.Reason;

    WFCPortInfo("SustainAP reason=<0x%x>\n", Dot11SustainParams.ulReason);

    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_DOT11_CAN_SUSTAIN_AP,
        NULL,
        (BYTE*)&Dot11SustainParams,
        sizeof(Dot11SustainParams) );

exit:

    TraceExit(status);
}

bool CPort::PerformPMKIDRequest(
    _In_ ULONG CandidatesCount,
    _In_reads_(CandidatesCount) CBSSEntry** ppBssCandidatesList
    )
{
    ULONGLONG rightNow;
    PDOT11_PMKID_CANDIDATE_LIST_PARAMETERS pPMKIDParam = NULL;
    PDOT11_BSSID_CANDIDATE PMKIDCandidateList;
    bool bPMKIDRequested = false;
    ULONG candidatesBufferSize = 0;

    CBSSEntry* pCurrentEntry;
    
    TraceEntry();

    if ( (GetDot11State() == WfcPortDot11StateInit) || (CandidatesCount == 0))
    {
        WFCPortTrace("Not connected (%d)/No candidates (%d). Ignoring PMKID candidate list check.", 
            GetDot11State(), CandidatesCount);
        goto exit;
    }

    // Check if we have recently done a PMKID request. OS has its own throttling mechanism
    // and we dont want to wait if its not going to happen
    rightNow = CSystem::get_CurrentTime();
    if (rightNow < (m_LastPMKIDRequest + WFC_CONSTANT::PMKIDReportingInterval))
    {
        WFCPortTrace("Skipping early PMKID candidate list update. Last = %llu, Now = %llu", m_LastPMKIDRequest, rightNow);
        goto exit;
    }

    // We are going to attempt PMKID request indication
    if (CandidatesCount > WFC_CONSTANT::PMKIDCacheSize)
    {
        // This is the max we have claimed to the OS
        CandidatesCount  = WFC_CONSTANT::PMKIDCacheSize;
    }

    candidatesBufferSize = sizeof(DOT11_PMKID_CANDIDATE_LIST_PARAMETERS) +   
        CandidatesCount * sizeof(DOT11_BSSID_CANDIDATE);

    pPMKIDParam = (PDOT11_PMKID_CANDIDATE_LIST_PARAMETERS)new UCHAR[candidatesBufferSize];
    if (pPMKIDParam == NULL)
    {
        WFCPortError("Failed to allocate memory for the PMKID Candidates buffer");
        goto exit;
    }

    ASSIGN_NDIS_OBJECT_HEADER(
        pPMKIDParam->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_PMKID_CANDIDATE_LIST_PARAMETERS_REVISION_1,
        sizeof(DOT11_PMKID_CANDIDATE_LIST_PARAMETERS));

    pPMKIDParam->uCandidateListOffset = sizeof(DOT11_PMKID_CANDIDATE_LIST_PARAMETERS);
    pPMKIDParam->uCandidateListSize = CandidatesCount;
    PMKIDCandidateList = (PDOT11_BSSID_CANDIDATE)(((PUCHAR)pPMKIDParam) + pPMKIDParam->uCandidateListOffset);

    for (ULONG i = 0; i < CandidatesCount; i++)
    {
        pCurrentEntry = ppBssCandidatesList[i];
        
        WFCTrace("PMKID Candidate %!MACADDR!", (UCHAR*)pCurrentEntry->GetBssId());

        RtlCopyMemory(
            PMKIDCandidateList[i].BSSID,
            pCurrentEntry->GetBssId(),
            sizeof(PMKIDCandidateList->BSSID));

        PMKIDCandidateList[i].uFlags = pCurrentEntry->GetPMKIDCandidateFlags();
    }
    
    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_DOT11_PMKID_CANDIDATE_LIST,
        NULL,
        pPMKIDParam,
        candidatesBufferSize);

    // Set state that we are waiting for a PMKID update
    WFCPortInfo("PMKID Candidate list indicated (Count: %d)", CandidatesCount);
    bPMKIDRequested = true;
    m_LastPMKIDRequest = rightNow;

    m_PortPropertyCache.SetPropertyBoolean(WfcPortPropertyPMKIDListUpdatePending, TRUE);

exit:
    if (pPMKIDParam)
    {
        delete[] pPMKIDParam;
    }
    
    TraceExit(NDIS_STATUS_SUCCESS);
    return bPMKIDRequested;
}

void CPort::OnBSSListChanged(
    _In_ CBSSListManager* pBSSListManager )
{
    UNREFERENCED_PARAMETER(pBSSListManager);
}

void CPort::OnAssociationParametersRequestReceived(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WDI_INDICATION_ASSOCIATION_PARAMS_REQUEST_PARAMETERS associationParametersRequest;
    bool bAllocatedJob = false;
    bool bRequestPMKID = false;
    CBSSEntry** ppTempBSSList = NULL;
    ULONG tempBssListSize = 0;

    TraceEntry();

    ndisStatus = ParseWdiIndicationAssociationParametersRequest(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &associationParametersRequest );

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCPortError("[AssocParam] Couldn't parse association parameters request. 0x%x", ndisStatus);
        goto exit;
    }

    // We will like to get the Incorporated list back
    tempBssListSize = associationParametersRequest.BSSEntryList.ElementCount;
    ppTempBSSList = new CBSSEntry*[tempBssListSize];
    if (ppTempBSSList == NULL)
    {
        ndisStatus = NDIS_STATUS_RESOURCES;
        WFCError("[AssocParam] Unable to allocate memory for BSS list. Error = 0x%08x", ndisStatus);
        goto exit;
    }
    
    // Incorporate the BSS entries into our BSS list
    ndisStatus = IncorporateBSSEntryList(&associationParametersRequest.BSSEntryList,
        &tempBssListSize,
        ppTempBSSList
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[AssocParam] Failed to parse association parameter's request BSS List (0x%x)", ndisStatus);
        goto exit;
    }


    // If there is a PMKID request, request it
    for (ULONG i = 0; i < associationParametersRequest.AssociationParameterTypes.ElementCount; i++)
    {
        if (associationParametersRequest.AssociationParameterTypes.pElements[i] == WDI_TLV_PMKID)
        {
            bRequestPMKID = true;
            break;
        }
    }
    
    if (bRequestPMKID)
    {

        bRequestPMKID = false;
        if (tempBssListSize)
        {
            WFCInfo("[AssocParam] Requesting Association Parameters PMKID request for %d entries", tempBssListSize);
        
            bRequestPMKID = PerformPMKIDRequest(tempBssListSize, 
                ppTempBSSList);
        }                

        if (!bRequestPMKID)
        {
            WFCInfo("[AssocParam] PMKID needed but not requested for Association Parameters request");
        }
    }


    // If we already have a job queued, it will run on its own and do the update (may not be 
    // what the driver asked for but would be with whatever we have)
    if (m_pAssociationParametersRequestResponderJob == NULL)
    {
        // We need to queue a job to wait for the response to be ready
        m_pAssociationParametersRequestResponderJob = new CAssociationParametersRequestResponderJob(
                IActivityId::get_NextActivityId());
        if (m_pAssociationParametersRequestResponderJob == NULL)
        {
            ndisStatus = NDIS_STATUS_RESOURCES;
            WFCError("[AssocParam] Unable to allocate association parameters response job. Error = 0x%08x", ndisStatus);
            goto exit;
        }
        bAllocatedJob = true;

        // Call a serialized initialization function
        ndisStatus = m_pAssociationParametersRequestResponderJob->InitializeFromAssociationParametersRequest(
            m_pAdapter,
            m_NdisPortNumber,
            m_WfcPortId,
            &associationParametersRequest);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("[AssocParam] Failed to initialize association parameters response job. Error = 0x%08x", ndisStatus);
            goto exit;
        }

        // Queue the non-serialized job
        ndisStatus = m_pAdapter->GetActiveJobsList()->StartAddNewJob(
            m_pAssociationParametersRequestResponderJob,
            this
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("[AssocParam] Failed to queue non-serialized association parameters response job. Error = 0x08%x",
                ndisStatus
                );
            goto exit;
        }

        m_QueuedJobCount++;
        // Job is queued, dont free it
        bAllocatedJob = false;
    }   
    else
    {
        WFCInfo("[AssocParam] Association parameter's request job already pending. Not queueing a new one");
    }

exit:
    if (ppTempBSSList)
    {
        delete[] ppTempBSSList;
    }
    
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (bAllocatedJob)
        {
            delete m_pAssociationParametersRequestResponderJob;
            m_pAssociationParametersRequestResponderJob = NULL;
        }
    }

    TraceExit(ndisStatus);
}


void CPort::OnIhvTaskRequestReceived(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    bool bAllocatedJob = false;
    
    CIhvTaskRequestJob *pIhvTaskJob = new CIhvTaskRequestJob(
            IActivityId::get_NextActivityId());
    if (pIhvTaskJob == NULL)
    {
        ndisStatus = NDIS_STATUS_RESOURCES;
        WFCError("[IHVTask] Unable to allocate IHV task request job. Error = 0x%08x", ndisStatus);
        goto exit;
    }
    bAllocatedJob = true;

    // Call a serialized initialization function
    ndisStatus = pIhvTaskJob->InitializeFromJob(
        m_pAdapter,
        m_NdisPortNumber,
        m_WfcPortId,
        EventBufferLength- sizeof( WFC_MESSAGE_METADATA),
        EventBuffer + sizeof( WFC_MESSAGE_METADATA )
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[IHVTask] Failed to initialize IHV task request job. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    // Queue the serialized job
    ndisStatus = m_pAdapter->GetSerializedJobList()->QueueSerializedJob(
        pIhvTaskJob,
        this
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[IHVTask] Failed to queue serialized IHV task request job. Error = 0x08%x",
            ndisStatus
            );
        goto exit;
    }

    m_QueuedJobCount++;
    // Job is queued, dont free it
    bAllocatedJob = false;

    // It will fire on its own later
exit:
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (bAllocatedJob)
        {
            delete pIhvTaskJob;
            pIhvTaskJob = NULL;
        }
    }

    TraceExit(ndisStatus);
}

// The Set NLO job store the last NLO command sent to the driver in TLV format.
// Several jobs (Power, RAndomMacForScan) need to replumb the NLO parameters after a reset.
// Before they are replumbed, the TLV need to be updated to reflect elapsed times.
//
// Returns NDIS_STATUS_SUCCESS if everything is fine, NDIS_STATUS_SOFT_ERRORS if there is no need for NLO, other errors otherwise.

NDIS_STATUS CPort::UpdateStoredNLOParameterBuffer()
{
    NDIS_STATUS status = NDIS_STATUS_SOFT_ERRORS; // can this is a better error.
    ULONGLONG ullElaspseMs; // elapse time in ms since NLO is plumb
    ULONG ulElapseS;
    ULONG ulElapsePartialS;
    LARGE_INTEGER liCurrentTime;
    CConnectedPeer *pPeer = NULL;
    WDI_NETWORK_LIST_OFFLOAD_PARAMETERS nloParameters;
    const PLACEMENT_NEW_ALLOCATION_CONTEXT pnac = { m_cbNloCommand, m_pbNloCommand };
    TLV_CONTEXT tlvCtx = *m_pAdapter->GetTlvContext();
    BYTE * pb = NULL;
    ULONG cb = 0;

    //
    // check to see if we need to re-plumb NLO
    //
    if (m_pbNloCommand == NULL )
    {
        // no standing NLO
        WFCInfo( "No outstanding NLO to replumb" );
        goto Exit;
    }

    pPeer = GetConnectedPeerList()->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);
    if (pPeer != NULL)
    {
        WFCInfo( "Link is active, no needs for NLO replumb" );
        goto Exit;
    }

    // must be greater
    NT_ASSERT( m_cbNloCommand > sizeof(WFC_MESSAGE_METADATA));

    //
    // adjust NLO scan times
    //
    KeQuerySystemTime( &liCurrentTime );
    NT_ASSERT( m_liNloStart.QuadPart != 0);
    ullElaspseMs = (liCurrentTime.QuadPart - m_liNloStart.QuadPart )/10000;
    ulElapseS = (ULONG)ullElaspseMs /1000;

    status = ParseWdiSetNetworkListOffloadToIhv(
        m_cbNloCommand - sizeof( WFC_MESSAGE_METADATA ),
        m_pbNloCommand + sizeof( WFC_MESSAGE_METADATA ),
        m_pAdapter->GetTlvContext(), &nloParameters );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError( "Somehow can't parse NLO message we sent? 0x%x", status );
        NT_ASSERTMSG( "Somehow can't parse what we generated!?!?", FALSE );
        goto Exit;
    }

    LONG fastIterations;    // how many fast scan has passed

    ulElapsePartialS = ulElapseS % nloParameters.NetworkListOffload.NetworkListOffloadConfig.FastScanPeriodinSeconds;
    fastIterations = ulElapseS / nloParameters.NetworkListOffload.NetworkListOffloadConfig.FastScanPeriodinSeconds;

    //
    // repeat enough times
    //
    if ( m_nNloWakes > c_MaxNloRepeatWakes )
    {
        WFCInfo( "Nlo replumb max reached %d", m_nNloWakes );
        status = NDIS_STATUS_SOFT_ERRORS;
        goto Exit;
    }

    if ( fastIterations < m_FastScanIterations )
    {
        nloParameters.NetworkListOffload.NetworkListOffloadConfig.FastScanIterations = m_FastScanIterations - fastIterations;
        nloParameters.NetworkListOffload.NetworkListOffloadConfig.DelayStartInSeconds = nloParameters.NetworkListOffload.NetworkListOffloadConfig.FastScanPeriodinSeconds - ulElapsePartialS;
        WFCInfo( "Replumb NLO reduce iters=%d delay=%d", 
            fastIterations,
            nloParameters.NetworkListOffload.NetworkListOffloadConfig.DelayStartInSeconds );
    }
    else // fast scan have exhausted into slow scan
    {
        nloParameters.NetworkListOffload.NetworkListOffloadConfig.FastScanIterations = 0;
        ulElapseS -= m_FastScanIterations * nloParameters.NetworkListOffload.NetworkListOffloadConfig.FastScanPeriodinSeconds;
        ulElapsePartialS = ulElapseS % nloParameters.NetworkListOffload.NetworkListOffloadConfig.SlowScanPeriodinSeconds;
        nloParameters.NetworkListOffload.NetworkListOffloadConfig.DelayStartInSeconds = nloParameters.NetworkListOffload.NetworkListOffloadConfig.SlowScanPeriodinSeconds - ulElapsePartialS;
    }

    WFCInfo ("Replumb NLO Dlay=%d FastScanIters=%d",
        nloParameters.NetworkListOffload.NetworkListOffloadConfig.DelayStartInSeconds,
        nloParameters.NetworkListOffload.NetworkListOffloadConfig.FastScanIterations );

    tlvCtx.AllocationContext = (ULONG_PTR)(&pnac);
    status = GenerateWdiSetNetworkListOffloadToIhv( &nloParameters, sizeof( WFC_MESSAGE_METADATA ), &tlvCtx, &cb, &pb );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError( "Somehow can't regenerate NLO message? 0x%x", status );
        goto Exit;
    }
    NT_ASSERT( pb == m_pbNloCommand );
    NT_ASSERT( cb == m_cbNloCommand );

Exit:
    return status;
}

NDIS_STATUS CPort::SaveNetworkCandidatesHistory(
    _In_ PDOT11_SSID pSsid,
    _In_ WFC_CONNECT_SCAN_TYPE CompletedScanType, 
    _In_ ULONG CandidatesCount,
    _In_reads_(CandidatesCount) CBSSEntry** ppBssCandidatesList
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CNetworkHistory* pNetwork;
    CBSSEntry *pBssEntry;
    ULONG bssIndex = 0;
    ULONG chanIndex = 0;
    ULONG bandId = 0;
    ULONG channelNumber = 0;
    
    TraceEntry();

    if (CandidatesCount > 1)
    {
        // Set property that this is a multiple BSSID network
        (void)GetPropertyCache()->SetPropertyBoolean(
            WfcPortPropertyMultipleBSSIDNetwork,
            TRUE);
    }
    else
    {
        // For single entries, dont do anything
        goto exit;
    }
    
    status = m_pConnectHistory->UpdateOrAddNetworkToTable(pSsid, &pNetwork);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError( "[ChannelHint] Failed to add network to candidates history. Error = 0x%x", status );
        goto exit;
    }

    // Go through the candidates and see if this AP is found on 5GHz
    if (!pNetwork->FoundOutside24GHz)
    {
        for (bssIndex = 0; bssIndex < CandidatesCount; bssIndex++)
        {
            pBssEntry = ppBssCandidatesList[bssIndex];
            if (pBssEntry->GetChannelInfo()->BandId != WDI_BAND_ID_2400)
            {
                // Seen on something other than 2.4GHz
                pNetwork->FoundOutside24GHz = true;
                WFCTrace( "[ChannelHint] SSID = %!DOT11SSID! has been found on non-2.4GHz channel B:%d C:%d", 
                    pSsid,
                    pBssEntry->GetChannelInfo()->BandId,
                    pBssEntry->GetChannelInfo()->Channel);

                // Clear the 2.4GHz connect usage so that we dont put higher weight
                // on a 2.4GHz scans
                for (chanIndex = 0; chanIndex < WFC_CONSTANT::MaxNetworkChannelHistorySize; chanIndex++)
                {
                    if (pNetwork->ChannelHistory[chanIndex].BandId == WDI_BAND_ID_2400)
                    {
                        WFCTrace( "[ChannelHint] Clearing 2.4GHz scan channel hint for channel B:%d C:%d", 
                            pNetwork->ChannelHistory[chanIndex].BandId,
                            pNetwork->ChannelHistory[chanIndex].ChannelNumber
                            );                
                        pNetwork->ChannelHistory[chanIndex].ConnectUsageCount = 0;
                    }                    
                }
                break;
            }
        }
    }

    // Check if every channel we have cached had an entry on this candidates list. Note that
    // we may have only completed a partial scan. But the goal is to keep track that we still 
    // found the AP. Note also that we dont add channel entries based on scans
    if (CompletedScanType > WfcConnectScanTypeNone)
    {
        for (chanIndex = 0; chanIndex < WFC_CONSTANT::MaxNetworkChannelHistorySize; chanIndex++)
        {
            pNetwork->ChannelHistory[chanIndex].FoundInCandidateList = false;
            if (pNetwork->ChannelHistory[chanIndex].IsValid())
            {
                bandId = pNetwork->ChannelHistory[chanIndex].BandId;
                channelNumber = pNetwork->ChannelHistory[chanIndex].ChannelNumber;
                
                for (bssIndex = 0; bssIndex < CandidatesCount; bssIndex++)
                {
                    pBssEntry = ppBssCandidatesList[bssIndex];
                    if ((pBssEntry->GetChannelInfo()->Channel == channelNumber) &&
                        (pBssEntry->GetChannelInfo()->BandId == bandId))
                    {
                        // Found on the scan. So even if we dont connect to it, its OK to keep it in
                        pNetwork->ChannelHistory[chanIndex].FoundInCandidateList = true;
                        WFCTrace( "[ChannelHint] Candidate AP found at index %d on channel B:%d C:%d", 
                            bssIndex,
                            bandId,
                            channelNumber
                            );
                        
                        break;
                    }
                }
            }
        }
    }
    
exit:
    TraceExit(status);
    return status;    
}
NDIS_STATUS CPort::SaveNetworkConnectHistory(
    _In_ PDOT11_SSID pSsid,
    _In_ CBSSEntry*  pBssEntry
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CNetworkHistory* pNetwork;
    UINT32 bandId;
    UINT32 channelNumber;

    TraceEntry();

    // For single BSSID entries, dont do anything
    if (!GetPropertyCache()->GetPropertyBooleanOrDefault(WfcPortPropertyMultipleBSSIDNetwork, FALSE))
    {
        goto exit;
    }

    status = m_pConnectHistory->UpdateOrAddNetworkToTable(pSsid, &pNetwork);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError( "[ChannelHint] Failed to add network to connect history. Error = 0x%x", status );
        goto exit;
    }

    // If the BSSID is the same as last attempt, then we dont update
    // the BSSID
    if (ARE_MAC_ADDRESS_EQUAL(pNetwork->LastBssid, pBssEntry->GetBssId()))
    {
        WFCTrace( "[ChannelHint] Not updating channel for reconnect to previous BSSID");    
        goto exit;
    }

    channelNumber = pBssEntry->GetChannelInfo()->Channel;
    bandId = pBssEntry->GetChannelInfo()->BandId;

    RtlCopyMemory(pNetwork->LastBssid, pBssEntry->GetBssId(), sizeof(DOT11_MAC_ADDRESS));

    WFCTrace("[ChannelHint] Connect channel entry B:%d C:%d. Link Quality %d Rank %d", 
        bandId, channelNumber, pBssEntry->GetSignalInfo()->LinkQuality, pBssEntry->GetCachedRank());

    pNetwork->FindOrAddChannelEntry(
        bandId,
        channelNumber
        );

exit:
    TraceExit(status);
    return status;    
}

int __cdecl ChannelUsageCompare(const void* a, const void* b)
{
    // By default qsort sorts in increasing order. We want the highest candidate to be at the top of the list
    return (((WFC_NETWORK_CHANNEL_ENTRY*)b)->ConnectUsageCount - ((WFC_NETWORK_CHANNEL_ENTRY*)a)->ConnectUsageCount);
}

NDIS_STATUS CPort::GetChannelListsFromChannelHistory(
    _In_ PDOT11_SSID pSsid,
    _In_ CChannelListManager *pChannelListManager)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CNetworkHistory* pNetwork;
    ULONG numChannelHintEntries = 0;

    TraceEntry();

    if (pChannelListManager == NULL)
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    numChannelHintEntries = m_pAdapter->GetAdapterPropertyCache()->GetPropertyULongOrDefault(
        WfcAdapterPropertyRegScanOptChannelCount,
        0
        );
    if ((numChannelHintEntries == 0) || (numChannelHintEntries > WFC_CONSTANT::MaxNetworkChannelHistorySize))
    {
        WFCTrace( "[ChannelHint] Scan channel hint optimization disable via registry");
        status = NDIS_STATUS_SUCCESS;
        goto exit;
    }        

    pNetwork = m_pConnectHistory->FindNetworkBySsid(pSsid);
    if (!pNetwork)
    {
        WFCTrace( "[ChannelHint] Didnt find the network in the connect history");
        // Not an error
        status = NDIS_STATUS_SUCCESS;
        goto exit;
    }
    
    // Sort the channel list based on usage
    qsort(&(pNetwork->ChannelHistory), 
        WFC_CONSTANT::MaxNetworkChannelHistorySize, 
        sizeof(WFC_NETWORK_CHANNEL_ENTRY), 
        ChannelUsageCompare);

    // Pick the first numChannelHintEntries entries
    for (ULONG i = 0; i < numChannelHintEntries; i++)
    {
        if (!pNetwork->ChannelHistory[i].IsValid())
        {
            break;
        }
        
        status = pChannelListManager->AddChannel(pNetwork->ChannelHistory[i].ChannelNumber, 
                        pNetwork->ChannelHistory[i].BandId);
        if (status != NDIS_STATUS_SUCCESS)
        {
            goto exit;
        }
        else
        {
            WFCTrace( "[ChannelHint] Reporting Scan Channel Hint for B:%d C:%d (Usage %d)",
                pNetwork->ChannelHistory[i].BandId,
                pNetwork->ChannelHistory[i].ChannelNumber,
                pNetwork->ChannelHistory[i].ConnectUsageCount
                );
        }
    }

exit:
    TraceExit(status);
    return status;
}

void CPort::TraceLoggingRoamFinished(
    _In_ NDIS_STATUS OperationStatus
    )
{
#ifndef WDI_WIN81_COMPLIANT

    if (m_pRoamTraceLoggingData == NULL)
    {
        return;
    }

    if (g_RegisteredWithTraceLogging)
    {
        if (OperationStatus != STATUS_SUCCESS)
        {
            m_pRoamTraceLoggingData->setCipherKeysTime = 0;
        }

        BOOLEAN FTConnection;
        m_PortPropertyCache.GetPropertyBoolean((WFC_PROPERTY_NAME)WfcPortPropertyCurrentConnectionFT, &FTConnection);

        // Fire the event
        TraceLoggingWrite(
            g_hProvider,
            "WdiRoamCompleted",
            TraceLoggingGuid(m_pAdapter->GetNetGuid(), "interfaceGuid"),
            TraceLoggingUInt32(m_pRoamTraceLoggingData->connectTrigger, "roamTrigger"),
            TraceLoggingUInt32(m_pRoamTraceLoggingData->roamWabiReason, "roamReason"),
            TraceLoggingHexInt32(OperationStatus, "errorCode"),
            TraceLoggingBool(m_pRoamTraceLoggingData->isCancelled, "isCancelled"),
            TraceLoggingHexInt32(m_pRoamTraceLoggingData->associationStatusCode, "associationStatusCode"),
            TraceLoggingBool(FTConnection, "is11rFastTransitionRoam"),
            TraceLoggingUInt8(m_pRoamTraceLoggingData->lastScanType, "lastScanType"),
            TraceLoggingBool(m_pRoamTraceLoggingData->isNeighborReportSupported, "isNeighborReportSupported"),
            TraceLoggingBool(m_pRoamTraceLoggingData->isAPChannelReportSupported, "isAPChannelReportSupported"),
            TraceLoggingUInt32(m_pRoamTraceLoggingData->bssCandidateCount, "bssCandidateCount"),
            TraceLoggingUInt8(m_pRoamTraceLoggingData->linkQualityPreviousConnect, "linkQualityPreviousConnect"),
            TraceLoggingUInt8(m_pRoamTraceLoggingData->linkQualityDuringRoam, "linkQualityDuringRoam"),
            TraceLoggingUInt8(m_pRoamTraceLoggingData->linkQualityCurrent, "linkQualityCurrent"),
            TraceLoggingUInt32(m_pRoamTraceLoggingData->channel, "channel"),
            TraceLoggingUInt32(m_pRoamTraceLoggingData->bandID, "bandID"),
            TraceLoggingUInt32(m_pRoamTraceLoggingData->roamAPRank, "roamAPRank"),
            TraceLoggingUInt32(m_pRoamTraceLoggingData->assocFailureCount, "assocFailureCount"),
            TraceLoggingUInt64(m_pRoamTraceLoggingData->connectJobStartTime, "connectJobStartTime"),
            TraceLoggingUInt64(m_pRoamTraceLoggingData->connectJobEndTime, "connectJobEndTime"),
            TraceLoggingUInt64(m_pRoamTraceLoggingData->connectRoamTaskStartTime, "connectRoamTaskStartTime"),
            TraceLoggingUInt64(m_pRoamTraceLoggingData->connectRoamTaskEndTime, "connectRoamTaskEndTime"),
            TraceLoggingUInt64(m_pRoamTraceLoggingData->disassociationIndicationTime, "disassociationIndicationTime"),
            TraceLoggingUInt64(m_pRoamTraceLoggingData->setCipherKeysTime, "setCipherKeysTime"),
            TraceLoggingUInt64(m_pRoamTraceLoggingData->timeToFinishPrivacyExemptionListSetMS, "privacyExemptionListSetDoneMS"),
            TraceLoggingUInt64(m_pRoamTraceLoggingData->timeToFinishBSSListUpdateMS, "bssListUpdateDoneMS"),
            TraceLoggingUInt64(m_pRoamTraceLoggingData->timeToFinishScanJobCompletionMS, "scanJobCompletionDoneMS"),
            TraceLoggingUInt64(m_pRoamTraceLoggingData->timeToFinishPMKIDUpdateMS, "pmkidUpdateDoneMS"),
            TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES)
            );
    }
    m_pRoamTraceLoggingData->linkQualityPreviousConnect = m_pRoamTraceLoggingData->linkQualityCurrent;
    m_pRoamTraceLoggingData->roamOccured = false;
#else
    UNREFERENCED_PARAMETER(OperationStatus);
#endif
}

void CSetNeighborReportEntriesJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    TraceEntry();
    UNREFERENCED_PARAMETER(SenderObject);
    CompleteJob(OperationStatus);
    TraceExit(OperationStatus);
}

void CSetNeighborReportEntriesJob::OnJobCancelled()
{
    TraceEntry();
    CancelJob();
    TraceExit(NDIS_STATUS_SUCCESS);
}

NDIS_STATUS CSetNeighborReportEntriesJob::InitializeFromNeighborReportEntries(
    _In_ CAdapter* pAdapter,
    _In_ NDIS_PORT_NUMBER PortNumber,
    _In_ WFC_PORT_ID portId,
    _In_ PWDI_SET_NEIGHBOR_REPORT_ENTRIES_PARAMETERS pNeighborReportEntries
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    m_pAdapter = pAdapter;
    m_NdisPortNumber = PortNumber;
    m_PortId = portId;

    // Initialize the base
    ndisStatus = CJobBase::Initialize(
        m_pAdapter->GetEventQueue(),
        m_pAdapter->GetDeviceCommandScheduler(),
        m_pAdapter->GetActiveJobsList(),
        m_pAdapter->GetSerializedJobList(),
        WiFiJobSetNeighborReportEntries,
        true,
        false,
        WiFiPriorityScopePort,
        WiFiSerializedJobPriority_10,
        // If queued as child of non-serialized job, then its used
        (IPropertyCacheDirectory *)m_pAdapter,
        (INdisConversion *)pAdapter,
        PortNumber
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[NeighborReport] Failed to initialize base job for CSetNeighborReportEntriesJob (0x%x)", ndisStatus);
        goto exit;
    }

    ndisStatus = GenerateWdiSetNeighborReportEntries(
        pNeighborReportEntries,
        sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext(),
        &m_CommandSize,
        &m_pCommandBuffer
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[NeighborReport] Failed to generate Neighbor report entries tlv blob (0x%x)", ndisStatus);
        goto exit;
    }

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

void CSetNeighborReportEntriesJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    // Initialize the device command with this TLV data
    ndisStatus = m_DeviceCommand.Initialize(
        m_PortId,
        WDI_SET_NEIGHBOR_REPORT_ENTRIES,
        m_CommandSize,
        m_pCommandBuffer
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[NeighborReport] Failed to initialize device command for Neighbor report entries. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    // Queue the command
    ndisStatus = QueueDeviceCommand(&m_DeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[NeighborReport] Failed to queue device command for Neighbor report entries (0x%x)", ndisStatus);
        goto exit;
    }

exit:
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        // If we arent waiting, move on to the next step
        OnJobStepCompleted(ndisStatus, this);
    }
}

#ifdef WDI_FUTURE_COMPLIANT_FASTROAM_CACHE
void CPort::ProcessAssociationCompletion(
    _In_ PDOT11_SSID pSsid,
    _In_ PDOT11_MAC_ADDRESS pBssid,
    _In_ PWFC_BSS_ENTRY_CHANNEL_INFO pChannelInfo)
{
    WFC_TRANSITION_INFO TransitionInfo = { 0 };
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceEntry();

    WFCPortInfo("[TransitionCache] Association completion SSID = %!DOT11SSID!, BSSID = %!MACADDR!, ChannelNum = %u, BandId = %u",
        pSsid, *pBssid, pChannelInfo->Channel, pChannelInfo->BandId);

    // Break if SSID doesn't match
    if (m_LastSsid.uSSIDLength != pSsid->uSSIDLength ||
        memcmp(
        m_LastSsid.ucSSID,
        pSsid->ucSSID,
        m_LastSsid.uSSIDLength) != 0)
    {
        goto exit;
    }

    if (ARE_MAC_ADDRESS_EQUAL(*pBssid, m_LastBssid))
    {
        goto exit;
    }

    RtlCopyMemory(TransitionInfo.FromBssid, m_LastBssid, sizeof(DOT11_MAC_ADDRESS));
    TransitionInfo.FromChannelInfo.Channel = m_LastChannelInfo.Channel;
    TransitionInfo.FromChannelInfo.BandId = m_LastChannelInfo.BandId;

    RtlCopyMemory(TransitionInfo.ToBssid, *pBssid, sizeof(DOT11_MAC_ADDRESS));
    TransitionInfo.ToChannelInfo.Channel = pChannelInfo->Channel;
    TransitionInfo.ToChannelInfo.BandId = pChannelInfo->BandId;

    ProcessRoamTransition(&TransitionInfo);
exit:
    // Update last values
    RtlCopyMemory(&m_LastSsid, pSsid, sizeof(DOT11_SSID));
    RtlCopyMemory(m_LastBssid, *pBssid, sizeof(DOT11_MAC_ADDRESS));
    m_LastChannelInfo.Channel = pChannelInfo->Channel;
    m_LastChannelInfo.BandId = pChannelInfo->BandId;

    TraceExit(status);
}

NDIS_STATUS CPort::GetChannelListsForNeighborScan(
    _Inout_ ULONG* pChannelList24GHzCount,
    _Out_writes_(*pChannelList24GHzCount) WDI_CHANNEL_NUMBER* pChannelList24GHz,
    _Inout_ ULONG* pChannelList5GHzCount,
    _Out_writes_(*pChannelList5GHzCount) WDI_CHANNEL_NUMBER* pChannelList5GHz)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG matching24GHzEntryCount = 0;
    ULONG matching5GHzEntryCount = 0;
    
    WFCPortInfo("[TransitionCache] Finding most likely channels for [%!MACADDR!]", m_LastBssid);

    if (pChannelList24GHzCount == NULL || pChannelList5GHzCount == NULL ||
        pChannelList24GHz == NULL || pChannelList5GHz == NULL)
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    // Find the transition entry for the current BSSID
    PWFC_TRANSITION_ENTRY pTransitionEntry = FindTransitionEntry(m_LastBssid);
    if (pTransitionEntry == NULL)
    {
        WFCPortInfo("[TransitionCache] BSSID not in cache");
        goto exit;
    }
    
    *pChannelList24GHzCount = matching24GHzEntryCount;
    *pChannelList5GHzCount = matching5GHzEntryCount;
exit:
    TraceExit(status);
    return status;
}

void CPort::ProcessRoamTransition(
    _In_ PWFC_TRANSITION_INFO pTransitionInfo)
{
    PWFC_TRANSITION_ENTRY pTransitionEntry = NULL;
    PWFC_DEST_ENTRY pDestinationEntry = NULL;
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceEntry();

    WFCPortInfo("[TransitionCache] Processing transition[%!MACADDR!:%u:%u] -> [%!MACADDR!:%u:%u]",
        pTransitionInfo->FromBssid,
        pTransitionInfo->FromChannelInfo.Channel,
        pTransitionInfo->FromChannelInfo.BandId,
        pTransitionInfo->ToBssid,
        pTransitionInfo->ToChannelInfo.Channel,
        pTransitionInfo->ToChannelInfo.BandId);

    pTransitionEntry = GetTransitionEntry(pTransitionInfo->FromBssid);
    if (pTransitionEntry == NULL)
    {
        // Not important enough to make it on the list
        WFCPortInfo("[TransitionCache] No entry found for [%!MACADDR!]", pTransitionInfo->FromBssid);
        goto exit;
    }

    // Update the entry
    RtlCopyMemory(pTransitionEntry->Bssid, pTransitionInfo->FromBssid, sizeof(DOT11_MAC_ADDRESS));
    RtlCopyMemory(&pTransitionEntry->ChannelInfo, &pTransitionInfo->FromChannelInfo, sizeof(WFC_BSS_ENTRY_CHANNEL_INFO));
    // TODO should the hits be updated if the destination bssid didn't make the cut???
    pTransitionEntry->Hits++;

    pDestinationEntry = GetDestinationEntry(pTransitionEntry, pTransitionInfo->ToBssid);
    if (pDestinationEntry == NULL)
    {
        WFCPortInfo("[TransitionCache] No dest entry found for [%!MACADDR!]", pTransitionInfo->ToBssid);
        //Not important enough to make it on the list
        goto exit;
    }

    // Update the destination entry
    RtlCopyMemory(pDestinationEntry->Bssid, pTransitionInfo->ToBssid, sizeof(DOT11_MAC_ADDRESS));
    RtlCopyMemory(&pDestinationEntry->ChannelInfo, &pTransitionInfo->ToChannelInfo, sizeof(WFC_BSS_ENTRY_CHANNEL_INFO));
    pDestinationEntry->Hits++;

    WFCPortInfo("[TransitionCache] [%!MACADDR!] -> [%!MACADDR!] recorded in transition cache", pTransitionInfo->FromBssid, pTransitionInfo->ToBssid);

exit:
    TraceExit(status);
    return;
}

PWFC_TRANSITION_ENTRY CPort::FindTransitionEntry(
    _In_ DOT11_MAC_ADDRESS Bssid)
{
    PWFC_TRANSITION_ENTRY pTransitionEntry = NULL;
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceEntry();

    for (UINT32 i = 0; i < m_TransitionInfoCache.NumberTransitionEntries; i++)
    {
        if (ARE_MAC_ADDRESS_EQUAL(Bssid, m_TransitionInfoCache.TransitionList[i].Bssid) &&
            m_TransitionInfoCache.TransitionList[i].Hits != 0)
        {
            WFCPortInfo("[TransitionCache] Source %!MACADDR! in cache entry %u", Bssid, i);
            pTransitionEntry = &(m_TransitionInfoCache.TransitionList[i]);
            break;
        }
    }

    TraceExit(status);
    return pTransitionEntry;
}

PWFC_DEST_ENTRY CPort::FindDestinationEntry(
    _In_ PWFC_TRANSITION_ENTRY pTransitionEntry,
    _In_ DOT11_MAC_ADDRESS Bssid)
{
    PWFC_DEST_ENTRY pDestinationEntry = NULL;
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceEntry();

    for (UINT32 i = 0; i < m_TransitionInfoCache.NumberDestinationEntries; i++)
    {
        if (ARE_MAC_ADDRESS_EQUAL(Bssid, pTransitionEntry->DestinationList[i].Bssid) &&
            pTransitionEntry->DestinationList[i].Hits != 0)
        {
            WFCPortInfo("[TransitionCache] Dest %!MACADDR! in cache dest entry %u", Bssid, i);
            pDestinationEntry = &(pTransitionEntry->DestinationList[i]);
            break;
        }
    }

    TraceExit(status);
    return pDestinationEntry;
}

PWFC_TRANSITION_ENTRY CPort::GetTransitionEntry(
    _In_ DOT11_MAC_ADDRESS Bssid)
{
    PWFC_TRANSITION_ENTRY pTransitionEntry = NULL;
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceEntry();

    // Is it in the list?
    pTransitionEntry = FindTransitionEntry(Bssid);

    // TODO think about keeping the transition list sorted also so I can easily spot zero hits in the list
    if (pTransitionEntry != NULL)
    {
        WFCPortInfo("[TransitionCache] Found current entry [%!MACADDR!] in cache", pTransitionEntry->Bssid);
        goto exit;
    }

    for (UINT32 i = 0; i < m_TransitionInfoCache.NumberTransitionEntries; i++)
    {
        if (m_TransitionInfoCache.TransitionList[i].Hits == 0)
        {
            WFCPortInfo("[TransitionCache] Clearing entry %u:[%!MACADDR!] in cache", i, m_TransitionInfoCache.TransitionList[i].Bssid);
            RtlZeroMemory(&(m_TransitionInfoCache.TransitionList[i]), sizeof(WFC_TRANSITION_ENTRY));
            pTransitionEntry = &(m_TransitionInfoCache.TransitionList[i]);
            goto exit;
        }
    }

    // No free transition entries available
    // TODO age out one of the entries, or decrement all entries by 1?
    WFCPortInfo("[TransitionCache] Source [%!MACADDR!] not in cache and cache has no free entries. Decrementing all hit counts", Bssid);
    for (UINT32 i = 0; i < m_TransitionInfoCache.NumberTransitionEntries; i++)
    {
        m_TransitionInfoCache.TransitionList[i].Hits--;
    }

exit:
    TraceExit(status);
    return pTransitionEntry;
}

PWFC_DEST_ENTRY CPort::GetDestinationEntry(
    _In_ PWFC_TRANSITION_ENTRY pTransitionEntry,
    _In_ DOT11_MAC_ADDRESS Bssid)
{
    PWFC_DEST_ENTRY pDestinationEntry = NULL;
    UINT32 i;
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceEntry();

    pDestinationEntry = FindDestinationEntry(pTransitionEntry, Bssid);

    if (pDestinationEntry != NULL)
    {
        WFCPortInfo("[TransitionCache] Found current dest entry [%!MACADDR!] in cache", pDestinationEntry->Bssid);
        goto exit;
    }

    for (i = 0; i < m_TransitionInfoCache.NumberDestinationEntries; i++)
    {
        if (pTransitionEntry->DestinationList[i].Hits == 0)
        {
            //TODO should I zero this out before returning it?  If it was 0 then it wasn't in use anyways?
            WFCPortInfo("[TransitionCache] Clearing dest entry %u:[%!MACADDR!] in cache", i, pTransitionEntry->DestinationList[i].Bssid);
            RtlZeroMemory(&(pTransitionEntry->DestinationList[i]), sizeof(WFC_DEST_ENTRY));
            pDestinationEntry = &(pTransitionEntry->DestinationList[i]);
            goto exit;
        }
    }

    // If we made it here then pDestinationEntry is NULL and ALL the destination hits are > 0; safe to decrement.
    // TODO think about ways to optimize this from a O(2N) to a O(N) (Easy way is to sort the list so I know if there are free entries
    // though this is just getting the hits ready for the next call.
    WFCPortInfo("[TransitionCache] Dest [%!MACADDR!] not in dest list and dest list has no free entries. Decrementing all hit counts", Bssid);
    for (i = 0; i < m_TransitionInfoCache.NumberDestinationEntries; i++)
    {
        pTransitionEntry->DestinationList[i].Hits--;
    }

exit:
    TraceExit(status);
    return pDestinationEntry;
}

UINT16 CPort::GetNumberBandIds(
    _In_ PWFC_TRANSITION_ENTRY pTransitionEntry)
{
    UINT16 numBandIds = 0;
    BOOL isUnique;
    if (pTransitionEntry == NULL)
    {
        goto exit;
    }

    for (UINT32 i = 0; i < m_TransitionInfoCache.NumberDestinationEntries; i++)
    {
        isUnique = TRUE;

        if (pTransitionEntry->DestinationList[i].Hits == 0)
            continue;

        for (UINT32 j = i + 1; j < m_TransitionInfoCache.NumberDestinationEntries; j++)
        {
            if (pTransitionEntry->DestinationList[j].Hits == 0)
                continue;

            if (pTransitionEntry->DestinationList[j].ChannelInfo.BandId ==
                pTransitionEntry->DestinationList[i].ChannelInfo.BandId)
            {
                isUnique = FALSE;
                break;
            }
        }

        if (isUnique)
            numBandIds++;
    }

exit:
    return numBandIds;
}

UINT16 CPort::GetNumberChannels(
    _In_ PWFC_TRANSITION_ENTRY pTransitionEntry,
    _In_ UINT32 BandId)
{
    UINT16 numChannels = 0;
    BOOL isUnique;

    if (pTransitionEntry == NULL)
    {
        goto exit;
    }

    for (UINT32 i = 0; i < m_TransitionInfoCache.NumberDestinationEntries; i++)
    {
        isUnique = TRUE;

        if (pTransitionEntry->DestinationList[i].Hits == 0 ||
            pTransitionEntry->DestinationList[i].ChannelInfo.BandId != BandId)
            continue;

        for (UINT32 j = i + 1; j < m_TransitionInfoCache.NumberDestinationEntries; j++)
        {
            if (pTransitionEntry->DestinationList[j].Hits == 0 ||
                pTransitionEntry->DestinationList[j].ChannelInfo.BandId != BandId)
                continue;

            if (pTransitionEntry->DestinationList[j].ChannelInfo.Channel ==
                pTransitionEntry->DestinationList[i].ChannelInfo.Channel)
            {
                isUnique = FALSE;
                break;
            }
        }

        if (isUnique)
            numChannels++;
    }

exit:
    return numChannels;
}
#endif
