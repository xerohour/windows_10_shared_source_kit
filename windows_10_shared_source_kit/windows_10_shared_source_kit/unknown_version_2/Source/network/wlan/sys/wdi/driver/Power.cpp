/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    power.cpp

Abstract:

    Implementation for generic request handler

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "Power.tmh"


NDIS_STATUS CSetPowerJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();
    
    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;
    m_JobState = JobStateNotStarted;
    m_OidPowerState = *(PNDIS_DEVICE_POWER_STATE)pOidRequest->DATA.SET_INFORMATION.InformationBuffer;
    //
    // We receive OID D0 which means the underlying stack is ready in D0. Set it early
    // so we can start the flow of cmds.
    //
    if ( m_OidPowerState == NdisDeviceStateD0 )
    {
        m_pAdapter->SetAdapterPowerState( NdisDeviceStateD0 );
        m_pAdapter->GetSerializedJobList()->set_AllowNewJobs(TRUE);    // this includes ourselves
    }

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID CSetPowerJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
)
{
       TraceActEntry();
       TraceActExit(NdisStatus);
}


VOID CSetPowerJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
)
{
    TraceActEntry();

    //
    // Must not fail.
    //
    if ( NdisStatus != NDIS_STATUS_SUCCESS )
    {
        DtErrMsg("SetPower failed\n");
        NdisStatus = NDIS_STATUS_SUCCESS;
    }

    //
    // Complete the OID back to NDIS
    //
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceExit(NdisStatus);
}

CPort * CSetPowerJob::GetNextNon0Port()
{
    CPort * pPort = NULL;
    for ( ;m_D2Dot11ResetLoop < m_pAdapter->c_MaxPorts; m_D2Dot11ResetLoop++ )
    {
        if ( m_pAdapter->m_pPortList[m_D2Dot11ResetLoop] == NULL )
        {
            continue;
        }
        if ( m_pAdapter->m_pPortList[m_D2Dot11ResetLoop]->GetNdisPortNumber() 
            == NDIS_DEFAULT_PORT_NUMBER )
        {
            continue;
        }
        else
        {
            pPort = m_pAdapter->m_pPortList[m_D2Dot11ResetLoop];
            m_D2Dot11ResetLoop++;
            break;
        }
    }
    return pPort;
}


void CSetPowerJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus= NDIS_STATUS_SUCCESS;

    TraceActEntry();

    m_pCPort = GetAdapter()->GetPortFromNdisPortNumber( m_OidPortNumber );
    NT_ASSERT( m_pCPort );
    m_OldPortPowerState = m_pCPort->GetPortPowerState();

    //
    // if we are going down Dx, stop serialized jobs. We restart them when we receive D0 OID.
    //
    if ( m_OidPowerState != NdisDeviceStateD0 )
    {
        m_pAdapter->GetSerializedJobList()->set_AllowNewJobs(FALSE);    // don't allow
        m_pAdapter->GetSerializedJobList()->FlushQueuedJobs(INVALID_NDIS_PORT_NUMBER);          // flush as well

        // Save whether we had wake up enabled or not when we are going down
        (void)GetPortPropertyCache()->SetPropertyBoolean(
            WfcPortPropertyLastSleepWasWakeEnabled,
            m_pCPort->GetbValidPmParameters());        
    }

    switch ( m_OidPowerState )
    {
        case NdisDeviceStateD0 :
            m_pCPort->SetPortPowerState( NdisDeviceStateD0 );

            //
            // coming back up , send the setpower cmd w/o a reset child job like D2 or D3
            //
            m_JobState = JobStateAwaitSetPowerCommandComplete;
            ndisStatus = StartSetPowerCommand();
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCError( "StartSetPowerCommand failed (%x)", ndisStatus );
            }
            break;
    
        case NdisDeviceStateD2 :
            NT_ASSERT( m_OldPortPowerState == NdisDeviceStateD0 );
            m_pAdapter->SetAdapterPowerState( NdisDeviceStateD2 );
            //
            // any port except primary port receive a dot11reset before set to D2
            //
            if ( m_OidPortNumber == NDIS_DEFAULT_PORT_NUMBER )
            {
                //
                // 1. Stop more IO/cancel and wait inflight to complete
                // 2. Issue Dot11Reset to all other ports if any
                // 3. arm for wake etc before set power command
                // 4. set the power
                //
                //
                // Set to NotStarted and jump to StepCompleted to handle steps in one place
                //
                m_JobState = JobStateNotStarted;
                m_D2Dot11ResetLoop = 0;
                OnJobStepCompleted( STATUS_SUCCESS, this );
            }
            else 
            {
                //
                // none default port does not receive set D2, but if they do, just do the set
                //
                NT_ASSERT( 0 );
                ndisStatus = STATUS_ALREADY_COMMITTED;
            }
            break;

        case NdisDeviceStateD3 :
            //DtBreak();
            NT_ASSERT( m_OldPortPowerState == NdisDeviceStateD0 );

            if ( m_OidPortNumber == NDIS_DEFAULT_PORT_NUMBER )
            {
                //
                // Set to NotStarted and jump to StepCompleted to handle steps in one place
                //
                m_JobState = JobStateNotStarted;
                m_D2Dot11ResetLoop = 0;
                OnJobStepCompleted( STATUS_SUCCESS, this );
            }
            else
            {
                //
                // non-default port; dont expect to get this for non-default port
                //
                NT_ASSERT( 0 );
                ndisStatus = STATUS_NOT_SUPPORTED; // make it an error so it is completed
            }
            break;

        default:
            NT_ASSERT( 0 );
            WFCError( "SetPower unsupported state %d", m_OidPowerState );
            break;
    }

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        CompleteJob( NDIS_STATUS_SUCCESS ); // must not fail set power
    }
    TraceExit( ndisStatus );
    return;
}


NDIS_STATUS
CSetPowerJob::StartReplumbNloCmd( _In_ CPort * pPort)
/*++

    When we set NIC to Dx, check to see if we need to re-plumb NLO
    This solves a problem described below.

    When the OS moves close to an AP, at boarder line signal quality, NLO could wake 
    and the OS tries to connect. However, the connection often fails half way due to weak signal.
    We will check the connection state when we go to Dx and replumb NLO as needed.
    If we are not connected, we will try to re-plumb NLO up to pPort->c_MaxNloRepeatWakes 
    to give OS chances to retry.connection in the next wake.

    if we don't need to re-plumb, we dont return SUCCESS, so the caller proceed to the next step.

--*/
{
    NDIS_STATUS status = NDIS_STATUS_SOFT_ERRORS; // can this is a better error.

    //
    // check to see if we need to re-plumb NLO
    // Update NLO buffer command returns NDIS_STATUS_SOFT_ERRORS if no need for NLO re-plumb
    //
    status = pPort->UpdateStoredNLOParameterBuffer();
    if (status == NDIS_STATUS_SOFT_ERRORS)
    {
        WFCInfo("No need to replumb NLO");\
    }
    else if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError( "Could not update the NLO parameters 0x%x", status );
    }
    else
    {
        // StartTask ->  Use use m_DeviceCommand and QueueDeviceCommand
        status = m_DeviceCommand.Initialize(
            m_PortId,
            WDI_SET_NETWORK_LIST_OFFLOAD,
            pPort->m_cbNloCommand,
            pPort->m_pbNloCommand);
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActError("Failed to initialize Device Command for SetNlo 0x%x", status);
        }
        else
        {
            status = QueueDeviceCommand(&m_DeviceCommand);
            if (status != NDIS_STATUS_SUCCESS)
            {
                WFCActError("Failed to start SetPower Task 0x%x", status);
            }
        }
    }

    return status;
}


NDIS_STATUS
CSetPowerJob::StartPauseJobForD3()
/*++

    Private function to Pause all ports before going to power D3

--*/
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;

    TraceActEntry();

    ndisStatus = m_PauseJobForD3.Initialize( m_pAdapter, WiFiJobMiniportPause );
    NT_ASSERT( NT_SUCCESS( ndisStatus )); // set power is not failable. We should pre-alloc to guarantee
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError( "SetPowerD3 Initialize Pause job failed (%x)", ndisStatus );
        goto Exit;
    }

    ndisStatus = StartChildJob( &m_PauseJobForD3 );
    
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError( "StartChildJob failed (%x)", ndisStatus );
        goto Exit;
    }

    Exit: 
    {
        TraceActExit( ndisStatus );
        return ndisStatus;
    }
}


NDIS_STATUS
CSetPowerJob::StartDot11ResetSubJob(
    _In_ CPort * pPort )
/*++

    Private function to send dot11reset job before we go low power.
    D3 - for any port
    D2 - for non-Primary port

--*/
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;

    TraceActEntry();

    ndisStatus = m_CDot11ResetJob.InitializeFromJob(
            m_pAdapter,
            pPort->GetNdisPortNumber(),
            NULL);
    NT_ASSERT( NT_SUCCESS( ndisStatus )); // set power is not failable. We should pre-alloc to guarantee
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError( "SetPower Initialize Reset job failed (%x)", ndisStatus );
        goto Exit;
    }

    ndisStatus = StartChildJob( &m_CDot11ResetJob );
    
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError( "StartChildJob failed (%x)", ndisStatus );
        goto Exit;
    }

    Exit: 
    {
        TraceActExit( ndisStatus );
        return ndisStatus;
    }
}


NDIS_STATUS
CSetPowerJob::StartOpenCloseFirmwareTask( 
    _In_ BOOLEAN bToOpen )
/*++

    Modelled after Adapter StartCloseTask

--*/
{
    //
    // at SetPower OID for D3, we close the firmware before bus goes to D3.
    //
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    DeviceCommand* pDeviceCommand = NULL;

    TraceActEntry();

    //
    // Prepare the close task for the device
    //
    ndisStatus = m_TaskOpenCloseFirmware.Initialize(
            m_pAdapter->GetNotificationManager(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetEventQueue()
            );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize OpenClose Task (%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = m_TaskOpenCloseFirmware.get_TaskDeviceCommand(&pDeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to get Device Command for Close Task (%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = pDeviceCommand->Initialize(
        WDI_PORT_ID_ADAPTER,
        (bToOpen) ? WDI_TASK_OPEN : WDI_TASK_CLOSE,
        sizeof(WFC_MESSAGE_METADATA),
        (BYTE *)&m_CommandBufferOpenCloseFwForD3 );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize DevCmd for OpenClose Task (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = StartTask(&m_TaskOpenCloseFirmware);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start OpenClose Task (0x%x)", ndisStatus);
        goto Exit;
    }
    
Exit:
    TraceActExit(ndisStatus);

    return ndisStatus;
}


NDIS_STATUS
CSetPowerJob::StartSetPowerCommand()
/*++

    Private function to send SetPower command

--*/
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;
    WDI_SET_POWER_PARAMETERS parameter;
    const PLACEMENT_NEW_ALLOCATION_CONTEXT pnac = { sizeof( m_pCommandBuffer ), m_pCommandBuffer };
    TLV_CONTEXT tlvCtx = *m_pAdapter->GetTlvContext();
    ULONG commandSize;
    BYTE * pb;

    TraceActEntry();

    //
    // Set the TLV for the SET_POWER Parameters
    //
    parameter.PowerState = m_OidPowerState;

    //++ PCIe, Dx may be without wakes, e.g. for Hibernation. Set PM_PARAMETERS is use-once
    //      per NDIS ie. if it is set, NDIS arms us to wake and it is used once to Dx. if this is not set,
    //      we are not armed to wake. In the latter, we can go D3Cold, i.e. Power Off where we needs
    //      to re-init the firmware and bring the firmware context up to pre-Dx state when resumed to D0

    if ( m_OidPowerState != NdisDeviceStateD0 ) 
    {
        PNDIS_PM_PARAMETERS pPmParameters = m_pCPort->GetNdisPmParameters();

        if ( m_OidPowerState == NdisDeviceStateD2 && 
            WdiUsbIdleStateConfirm == m_pAdapter->GetUsbIdleState())
        {
            //
            // this is usb selective suspend, send D2 appropo
            //
            WFCInfo( "Set Power USB Selective Suspend" );
            parameter.Optional.SetPowerDxReason_IsPresent = true;
            parameter.SetPowerDxReason = WDI_SET_POWER_DX_REASON_SELETIVE_SUSPEND;
        }

        else if ( m_pCPort->GetbValidPmParameters() == FALSE )
        {
            //
            // Not to arm for wake since none is set.
            //
            WFCInfo( "Set power %d no arm to wake\n", m_OidPowerState );
        }
        else
        {
            //
            // size include NdisPmParameters
            //
            WFCInfo( "Set power %d arm to wake", m_OidPowerState );
            WFCInfo( "Wake settings 0x%08x 0x%08x 0x%08x 0x%08x",
                pPmParameters->EnabledWoLPacketPatterns,
                pPmParameters->EnabledProtocolOffloads,
                pPmParameters->WakeUpFlags,
                pPmParameters->MediaSpecificWakeUpEvents);

            parameter.Optional.WakeEventMask_IsPresent = true;
            parameter.WakeEventMask.EnabledWoLPacketPatterns = pPmParameters->EnabledWoLPacketPatterns;
            parameter.WakeEventMask.EnabledProtocolOffloads = pPmParameters->EnabledProtocolOffloads;
            parameter.WakeEventMask.WakeUpFlags = pPmParameters->WakeUpFlags;
            parameter.WakeEventMask.MediaSpecificWakeUpEvents = pPmParameters->MediaSpecificWakeUpEvents;
            //
            // PM_PARAMETERS used once; reset when resume to D0
            //
        }
    }

    tlvCtx.AllocationContext = (ULONG_PTR)(&pnac);
    ndisStatus = GenerateWdiSetPowerState( &parameter, sizeof( WFC_MESSAGE_METADATA ), &tlvCtx, &commandSize, &pb );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError( "Failed to generate tlv for SetPower (0x%x)", ndisStatus );
        goto Exit;
    }
    NT_ASSERT( pb == m_pCommandBuffer );

    ndisStatus = m_DeviceCommand.Initialize(
        WDI_PORT_ID_ADAPTER, // m_PortId, WDI spec says set power is on Adapter.
        WDI_SET_POWER_STATE,
        commandSize,
        m_pCommandBuffer );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for SetPower (0x%x)", ndisStatus);
        goto Exit;
    }

    // StartTask ->  Use use m_DeviceCommand and QueueDeviceCommand
    ndisStatus = QueueDeviceCommand(&m_DeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start SetPower Task (0x%x)", ndisStatus);
        goto Exit;
    }

    Exit:
    {
        TraceActExit( ndisStatus );
        return ndisStatus;
    }
}

void CSetPowerJob::D3DefaultPortArmedStepCompleted(
            _In_ NDIS_STATUS ndisStatus,
            _In_ WFC_SENDER_OBJECT SenderObject )
{
    SenderObject; // reference

    if ( m_JobState == JobStateAwaitPauseCompleteForDx ||
        m_JobState == JobStateAwaitDot11ResetJobComplete )
    {
        //
        // armed to wake, reset non-default ports and replumb NLO if needed
        //
        CPort * pPort = GetNextNon0Port();
        if ( pPort )
        {
            m_JobState = JobStateAwaitDot11ResetJobComplete;
            
            ndisStatus = StartDot11ResetSubJob( pPort );
            if (ndisStatus == NDIS_STATUS_SUCCESS)
            {
                goto Exit;
            }
            else
            {
                WFCError( "StartDot11ResetSubJob failed (%x)", ndisStatus );
                NT_ASSERT( 0 );
                //
                // fall thru to replumb NLO if necessary
                //
            }
        }
        m_JobState = JobStateDot11ResetJobCompleted;
    }

    if ( m_JobState == JobStateDot11ResetJobCompleted )
    {
        m_JobState = JobStateAwaitReplumbNloCmdComplete ;
        ndisStatus = StartReplumbNloCmd( m_pCPort );

        if ( ndisStatus == NDIS_STATUS_SUCCESS )
        {
            goto Exit;
        }
        else
        {
            WFCError( "StartDot11Replumb NLO failed (%x)", ndisStatus );
            NT_ASSERT( ndisStatus == NDIS_STATUS_SOFT_ERRORS );
            //
            // fall thru to replumb NLO if necessary
            //
        }
    }

    if ( m_JobState == JobStateAwaitReplumbNloCmdComplete )
    {
        //
        // This implies armed to wake 1st expression 
        // Not armed with 2nd expression.  Set D3
        //
        m_JobState = JobStateAwaitSetPowerCommandComplete;
        ndisStatus = StartSetPowerCommand();
        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            goto Exit;
        }
    }

    if ( m_JobState == JobStateAwaitSetPowerCommandComplete )
    {
        WFCInfo( "StartSetPowerCommand status (%x)", ndisStatus );
        m_pCPort->SetPortPowerState( NdisDeviceStateD3 );
        m_pAdapter->SetAdapterPowerState( NdisDeviceStateD3 );
        CompleteJob ( NDIS_STATUS_SUCCESS ); // must not fail
    }
    Exit:
    return;
}

void CSetPowerJob::D3DefaultPortNoArmStepCompleted(
        _In_ NDIS_STATUS ndisStatus,
        _In_ WFC_SENDER_OBJECT SenderObject )
{
    SenderObject; // reference

    //
    // Not armed to wake. Probably Hibernation or hiber-shutdown
    // Prepare for D3 and resume to D0 when we get it
    //
    // reset non-default ports 
    // Disconnect default port if connected
    //
    if ( m_JobState == JobStateAwaitPauseCompleteForDx ||
        m_JobState == JobStateAwaitDot11ResetJobComplete )
    {
        //
        // Reset all non-default ports
        //
        CPort * pPort = GetNextNon0Port();
        DtPrint( "D3Reset P2p Port %p\n", pPort);
        if ( pPort )
        {
            m_JobState = JobStateAwaitDot11ResetJobComplete;

            ndisStatus = StartDot11ResetSubJob( pPort );
            if (ndisStatus == NDIS_STATUS_SUCCESS)
            {
                goto Exit;
            }
            else
            {
                WFCError( "StartDot11ResetSubJob failed (%x)", ndisStatus );
                NT_ASSERT( 0 );
                //
                // fall thru to do next step
                //
            }
        }
        m_JobState = JobStateDot11ResetJobCompleted;
    }

    if ( m_JobState == JobStateDot11ResetJobCompleted )
    {
        //
        // not armed to wake; if default port is connected, disconnect it
        //
        CConnectedPeer* pPeer;
        m_JobState = JobStateAwaitDisconnectTaskComplete;
        pPeer = m_pCPort->GetConnectedPeerList()->
                    FindPeerByAssociationState(dot11_assoc_state_auth_assoc);

        DtPrint( "D3 STA peer is 0x%p\n", pPeer );
        if ( pPeer )
        {
            ndisStatus = GetPortPropertyCache()->SetPropertyBoolean(
                WfcPortPropertyRoamAtResumeD0,
                TRUE);
            if ( ndisStatus != STATUS_SUCCESS )
            {
                WFCError( "Set port property RoamAtResumeD0 failed 0x%x", ndisStatus );
            }

            // Launch the child job to cause disconnect on the port
            ndisStatus = m_DisconnectJob.InitializeFromJob(
                    m_pAdapter,
                    m_OidPortNumber,
                    false,
                    WDI_ASSOC_STATUS_DISASSOCIATED_BY_HOST,
                    DOT11_MGMT_REASON_DEAUTH_LEAVE_SS );
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                DtErrMsg( "D3Disconnect failed 0x%x\n", ndisStatus );
                WFCActError("Failed to initialize Disconnect/Stop Job (0x%x)", ndisStatus);
                goto Exit;
            }

            ndisStatus = StartChildJob(&m_DisconnectJob);
            DtPrint( "D3 Start disconn status 0x%x\n", ndisStatus );
            if ( ndisStatus == NDIS_STATUS_SUCCESS )
            {
                goto Exit;
            }
            else
            {
                DtErrMsg( "D3Start Disconn failed 0x%x\n", ndisStatus );
                WFCActError("Failed to start Disconnect/Stop Job (0x%x)", ndisStatus);
                // fall thru to the next step
            }

        }
        else
        {
            //
            // Not connected; no disconnect ; fall thru to do next step
            //
        }
    }

    if ( m_JobState == JobStateAwaitDisconnectTaskComplete )
    {
        m_JobState = JobStateAwaitSetPowerCommandComplete;
        ndisStatus = StartSetPowerCommand();
        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            goto Exit;
        }
        else
        {
            WFCError( "StartSetPower D3 failed (%x)", ndisStatus );
            NT_ASSERT( 0 );
            // fall thru to complete the step
        }
    }

    if ( m_JobState ==  JobStateAwaitSetPowerCommandComplete ) 
    {
        //
        // we are done
        //
        NT_ASSERT( ndisStatus == NDIS_STATUS_SUCCESS );
        m_pCPort->SetPortPowerState( NdisDeviceStateD3 );
        m_pAdapter->SetAdapterPowerState( NdisDeviceStateD3 );
        CompleteJob( NDIS_STATUS_SUCCESS );
    }

    Exit:

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        CompleteJob( ndisStatus );
    }
    return;
}

void CSetPowerJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;
    CPort *pPort=NULL;

    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);

    TraceActEntry();

    switch ( m_OidPowerState )
    {
        case NdisDeviceStateD2 :

            if ( m_JobState == JobStateNotStarted )
            {
                //
                // if we are idling USB - select suspend we dont do anything extra because
                // it is supposed to be transparent to clients.  Send D2 with note
                //
                WDI_USB_IDLE_STATE usbIdleState = m_pAdapter->GetUsbIdleState();

                if ( usbIdleState == WdiUsbIdleStateConfirm )
                {
                    m_JobState = JobStateAwaitSetPowerCommandComplete;
                    ndisStatus = StartSetPowerCommand();
                    if (ndisStatus == NDIS_STATUS_SUCCESS)
                    {
                        goto Exit;
                    }
                    else
                    {
                        WFCError( "StartSetPowerCommand failed (%x)", ndisStatus );
                        m_pCPort->SetPortPowerState( NdisDeviceStateD2 );
                        m_pAdapter->SetAdapterPowerState( NdisDeviceStateD2 );
                        CompleteJob ( NDIS_STATUS_SUCCESS ); // must not fail
                    }
                }
 
                //
                // Pause data and await completions
                //
                pPort = m_pAdapter->GetPortFromNdisPortNumber(NDIS_DEFAULT_PORT_NUMBER);

                m_JobState = JobStateAwaitDot11ResetJobComplete;

                // Set Power must *NOT* wait for all RX frames to be returned
                ndisStatus = pPort->Pause( this, CPort::NoWaitForRxReturn );

                if ( ndisStatus == NDIS_STATUS_PENDING )
                {
                    //
                    // wait for completion callback
                    //
                    SetPending();
                    goto Exit;
                }
                else
                {
                    //
                    // done sync'ly (success of failure). Continue to the next step
                    //
                }
            }

            if ( m_JobState ==  JobStateAwaitDot11ResetJobComplete )
            {
                //
                // if we are setting D2 on port 0, we may have more  non-0 port to reset
                //
                if ( m_OidPortNumber == NDIS_DEFAULT_PORT_NUMBER ) 
                {
                    pPort = GetNextNon0Port();
                    if ( pPort )
                    {
                        m_JobState = JobStateAwaitDot11ResetJobComplete;
                        
                        ndisStatus = StartDot11ResetSubJob( pPort );
                        if (ndisStatus == NDIS_STATUS_SUCCESS)
                        {
                            goto Exit;
                        }
                        else
                        {
                            WFCError( "StartDot11ResetSubJob failed (%x)", ndisStatus );
                            //
                            // fall thru to set power on the target, best effort
                            //
                        }
                    }

                    ndisStatus = StartReplumbNloCmd( m_pCPort );

                    if ( ndisStatus == NDIS_STATUS_SUCCESS )
                    {
                        m_JobState = JobStateAwaitReplumbNloCmdComplete;
                        goto Exit;
                    }

                    //
                    //  do arm for wake <todotodo> and set D2
                    //
                    m_JobState = JobStateAwaitSetPowerCommandComplete;
                    ndisStatus = StartSetPowerCommand();
                    if (ndisStatus == NDIS_STATUS_SUCCESS)
                    {
                        goto Exit;
                    }
                    else
                    {
                        WFCError( "StartSetPowerCommand failed (%x)", ndisStatus );
                        m_pCPort->SetPortPowerState( NdisDeviceStateD2 );
                        m_pAdapter->SetAdapterPowerState( NdisDeviceStateD2 );
                        CompleteJob ( NDIS_STATUS_SUCCESS ); // must not fail
                    }
                }
                else 
                {
                    //
                    // none default port does not receive set D2, but if they do, just do the set
                    //
                    NT_ASSERT( 0 );
                }
            }
            else if ( m_JobState == JobStateAwaitReplumbNloCmdComplete )
            {
                ndisStatus = StartSetPowerCommand();
                if (ndisStatus == NDIS_STATUS_SUCCESS)
                {
                    m_JobState = JobStateAwaitSetPowerCommandComplete;
                    goto Exit;
                }
                else
                {
                    WFCError( "StartSetPowerCommand failed (%x)", ndisStatus );
                    m_pCPort->SetPortPowerState( NdisDeviceStateD2 );
                    m_pAdapter->SetAdapterPowerState( NdisDeviceStateD2 );
                    CompleteJob ( NDIS_STATUS_SUCCESS ); // must not fail
                }
            }
            else if ( m_JobState ==  JobStateAwaitSetPowerCommandComplete ) 
            {
                //
                // we are done
                //
                NT_ASSERT( OperationStatus == NDIS_STATUS_SUCCESS );
                m_pCPort->SetPortPowerState( NdisDeviceStateD2 );
                m_pAdapter->SetAdapterPowerState( NdisDeviceStateD2 );
                CompleteJob( NDIS_STATUS_SUCCESS );
            }
            break;

        case NdisDeviceStateD0 :
            ///DtBreak();
            if ( m_JobState == JobStateAwaitSetPowerCommandComplete )
            {

                //
                //restart data; and reset ValidPmParameters
                //
                if ( m_OidPortNumber == NDIS_DEFAULT_PORT_NUMBER )
                {
                    (void) m_pCPort->Restart();
                    WFCInfo( "Resume D0 restarts data path" );
                    m_pCPort->SetbValidPmParameters( FALSE );
                }

                BOOLEAN bValidPmParameters = m_pCPort->SetbValidPmParameters(FALSE);
                NT_ASSERT( OperationStatus == NDIS_STATUS_SUCCESS );

                NDIS_STATUS wifiStatus;
                ndisStatus = CMessageHelper::GetStatusFromCommandResult( 
                                &m_DeviceCommand,
                                &wifiStatus );
                ULONG cbBuffer;
                PBYTE pbBuffer;
                WDI_SET_POWER_RESULTS results;

                m_JobState = JobStateAwaitD0ReinitComplete;
                //
                // check if WDI_STATUS_RESUME_FIRMWARE_REQUIRED
                //
                if ( OperationStatus == STATUS_SUCCESS && 
                    wifiStatus == NDIS_STATUS_SUCCESS &&
                    NDIS_STATUS_SUCCESS == m_DeviceCommand.get_CommandResult(&cbBuffer, &pbBuffer) &&
                    NDIS_STATUS_SUCCESS == ParseWdiSetPowerState(
                        cbBuffer - sizeof( WFC_MESSAGE_METADATA ),
                        pbBuffer + sizeof( WFC_MESSAGE_METADATA ),
                        m_pAdapter->GetTlvContext(), &results) &&
                    results.Optional.bResumeRequired_IsPresent &&
                    results.bResumeRequired )
                {
                    NT_ASSERT( m_OldPortPowerState == NdisDeviceStateD3 );
                    NT_ASSERT( m_NdisPortNumber == NDIS_DEFAULT_PORT_NUMBER );
                    //
                    //PM_POWER_PARAMETERS is use-once at going  into Dx;
                    // We always reset it at ebginning of this switch case.
                    // If we were armed to wake, the Firmware did not lose power which
                    // means no needs to re-init the FW and to restore its context
                    //
                    bValidPmParameters; // referenced
                    NT_ASSERT( !bValidPmParameters );

                    WFCInfo( "Firmware resume required" );
                    m_ResumeFirmwareJob.InitializeExplicit( m_pAdapter );

                    // Set a flag that we are in resume restore
                    m_pAdapter->SetResumeRestoreState(true);
                    
                    ndisStatus = StartChildJob(&m_ResumeFirmwareJob);
                    if ( ndisStatus == NDIS_STATUS_SUCCESS )
                    {
                        DtPrint( "ResumeFirmwareJob started\n" );
                        goto Exit;
                    }
                    else
                    {
                        m_pAdapter->SetResumeRestoreState(false);
                    
                        WFCActError("Failed to start ResumeFirmware Job (0x%x)", ndisStatus);
                        DtPrint( "ResumeFirmwareJobFailed 0x%x\n", ndisStatus );
                        // fall out to  complete job
                    }
                }
                else
                {
                    //
                    // no needs to resume firmware
                    //
                    // fall out to  continue
                }
            }

            if ( m_JobState == JobStateAwaitD0ReinitComplete )
            {
                // 
                //Roam if we forced a disconnect at Dx
                //
                NT_ASSERT( m_OldPortPowerState == NdisDeviceStateD3 );
                m_JobState = JobStateAwaitD0RoamComplete;

                m_pAdapter->SetResumeRestoreState(false);

                BOOLEAN bRoamAtResumeD0 = FALSE;
                ndisStatus = GetPortPropertyCache()->GetPropertyBoolean(
                    WfcPortPropertyRoamAtResumeD0,
                    &bRoamAtResumeD0 );
                if ( ndisStatus != STATUS_SUCCESS )
                {
                    bRoamAtResumeD0 = FALSE;
                    ndisStatus = STATUS_SUCCESS; // ok if the property is not set
                }

                DtPrint( "ResumeFirmwareCompleted bRaom=%d\n" , bRoamAtResumeD0);
                if ( bRoamAtResumeD0 )
                {
                    //
                    // We need to roam. Reset our flag before action
                    //
                    GetPortPropertyCache()->SetPropertyBoolean(
                            WfcPortPropertyRoamAtResumeD0,
                            FALSE );

                    //
                    // use port->TriggerReconnect so it is run after we complete D0
                    //
                    m_pCPort->TriggerReconnect(WDI_ASSOC_STATUS_DISASSOCIATED_BY_HOST,
                                               WfcRoamConnectTriggerResumeFromDx,
                                               CConnectHelpers::GetRoamConfig(WfcRoamConnectTriggerResumeFromDx));
                    if ( ndisStatus != NDIS_STATUS_SUCCESS )
                    {
                        WFCError("Couldn't trigger roaming reconnect %x", ndisStatus );
                        // continue regardless to next step
                        ndisStatus = NDIS_STATUS_SUCCESS;
                    }
                }
            }
            //
            //any cases except successfully launch roam child job comes here. complete this job.
            // Includes all failure cases and no need to roam which ndisStatus is a SUCCESS.
            //
            CompleteJob( ndisStatus );
            break;

        case NdisDeviceStateD3 :
            //
            // Supports for PCIe wake at D3. consider Morpheus only, i.e. AOAC only.
            // If armed to wake, the steps are similar to D2 armed to wake for SDBus
            //
            // If !bValidPmParameters (not armed to wake)
            //      Hibernation (and for resume)
            //      We disconnected the NIC, if it was connected. Resume at roaming.
            //
            // If bValidPmParameters (armed to wake)
            //  SET on non-default port - nop, but will be resume at default port resume
            //  SET on default port 
            //      1. Reset all non-default ports
            //      2. replumb NLO if needed
            //      3. set power D3
            //      4. Disconnect if connected && no arm to wake
            //

            //DtBreak();
            // For P2p ports we have completed it in JobStart. Here we deal default port only.
            NT_ASSERT( m_OidPortNumber == NDIS_DEFAULT_PORT_NUMBER );

            //
            // Regardless armed for wake or not, we pause data
            //
            if ( m_JobState == JobStateNotStarted )
            {
                //
                // Pause data and await completions
                //
                pPort = m_pAdapter->GetPortFromNdisPortNumber(NDIS_DEFAULT_PORT_NUMBER);

                m_JobState = JobStateAwaitPauseCompleteForDx;

                // Set Power must *NOT* wait for all RX frames to be returned
                ndisStatus = pPort->Pause( this, CPort::NoWaitForRxReturn );

                if ( ndisStatus == NDIS_STATUS_PENDING )
                {
                    //
                    // wait for completion callback
                    //
                    SetPending();
                    goto Exit;
                }
                else
                {
                    //
                    // done sync'ly (success of failure). Continue to the next step
                    //
                }
            }

            if ( m_pCPort->GetbValidPmParameters())
            {
                //
                // There is armed to wake, proceed differently vs not armed.
                //
                DtPrint( "D3 armed for wake\n" );
                D3DefaultPortArmedStepCompleted( OperationStatus, SenderObject );
            }
            else 
            {
                //
                // We are not armed to wake, we will disconnect if connected
                //
                DtPrint( "D3 NOT armed for wake\n" );
                D3DefaultPortNoArmStepCompleted( OperationStatus, SenderObject );
            }
        break;
    }

    Exit:
    {
        TraceActExit( ndisStatus );
        return;
    }
}

void CSetPowerJob::OnJobCancelled()
{
    // We are not cancellable
}


//
// Query Power OID implementation
//
NDIS_STATUS CQueryPowerJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;
    m_OidPowerState = *(PNDIS_DEVICE_POWER_STATE)pOidRequest->DATA.SET_INFORMATION.InformationBuffer;

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID CQueryPowerJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceEntry();

    TraceExit(NdisStatus);
}

VOID CQueryPowerJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceActEntry();

    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit( NdisStatus );
}

void CQueryPowerJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    m_pCPort = GetAdapter()->GetPortFromNdisPortNumber( m_OidPortNumber );
    NT_ASSERT( m_pCPort );
    m_OldPortPowerState = m_pCPort->GetPortPowerState();


    // Handle this ourselves; not send anything to LE
    //ndisStatus = StartQueryPowerCommand();

    //if ( ndisStatus != NDIS_STATUS_SUCCESS )
    //{
        CompleteJob( ndisStatus );
    //}
    TraceActExit( ndisStatus );
}

void CQueryPowerJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);
    TraceActEntry();
    CompleteJob(OperationStatus);
    TraceActExit( STATUS_SUCCESS );
}

void CQueryPowerJob::OnJobCancelled()
{
    // We are not cancellable

    TraceActEntry();
    TraceActExit( STATUS_SUCCESS );
}


NDIS_STATUS
CQueryPowerJob::StartQueryPowerCommand(
    )
/*++

    Private function to send QueryPower command

--*/
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;

    TraceActEntry();

    ndisStatus = m_DeviceCommand.Initialize(
        m_pCPort-> GetPortId(),
        0,
        c_CommandSize,
        m_pCommandBuffer );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for Query Power (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = QueueDeviceCommand(&m_DeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Query Power Task (0x%x)", ndisStatus);
        goto Exit;
    }

    Exit:
    {
        TraceActExit( ndisStatus );
        return ndisStatus;
    }
}


//
// CSetAutoPoweSaveJob  OID implementation
//
NDIS_STATUS CSetAutoPowerSaveJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_POWER_MGMT_AUTO_MODE_ENABLED_INFO pAutoPsmInfo;

    TraceEntry();
    
    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;
    pAutoPsmInfo = (PDOT11_POWER_MGMT_AUTO_MODE_ENABLED_INFO)
            pOidRequest->DATA.SET_INFORMATION.InformationBuffer;
    m_bOidAutoPowerSave = pAutoPsmInfo->bEnabled;

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID CSetAutoPowerSaveJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceEntry();

    TraceExit(NdisStatus);
}

VOID CSetAutoPowerSaveJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceActEntry();

    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit( NdisStatus );
}

void CSetAutoPowerSaveJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    m_pCPort = GetAdapter()->GetPortFromNdisPortNumber( m_OidPortNumber );
    NT_ASSERT( m_pCPort );

    ndisStatus = StartSetAutoPowerSaveCommand();

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        CompleteJob( ndisStatus );
    }

    TraceActExit( ndisStatus );
}

void CSetAutoPowerSaveJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);
    TraceActEntry();

    if ( OperationStatus == STATUS_SUCCESS )
    {
        if (GetPortPropertyCache()->SetPropertyBoolean(
            WfcPortPropertyAutoPsm,
            m_bOidAutoPowerSave
            ) != NDIS_STATUS_SUCCESS)
        {
            WFCActInfo("Couldn't save AutoPsm in property cache");
        }
    }
    CompleteJob(OperationStatus);
    TraceActExit( OperationStatus );
}

void CSetAutoPowerSaveJob::OnJobCancelled()
{
    // We are not cancellable

    TraceActEntry();
    TraceActExit( STATUS_SUCCESS );
}


NDIS_STATUS
CSetAutoPowerSaveJob::StartSetAutoPowerSaveCommand(
    )
/*++

    Private function to send SetAutoPowerSave command

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // Initialize the Connection quality Job
    ndisStatus = m_ConnectionQualityJob.InitializeFromJob(
        m_pAdapter,
        m_NdisPortNumber,
        m_pAdapter->IsResumeRestoreInProgress(),
        m_PortId,
        WDI_CONNECTION_QUALITY_AUTO_POWER_SAVE,
        m_bOidAutoPowerSave);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize connection quality job from auto-psm setting (0x%x)", ndisStatus);
        goto Exit;
    }

    // Queue the child job
    ndisStatus = CJobBase::StartChildJob(
        &m_ConnectionQualityJob
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to queue connection quality job (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:

    TraceActExit(ndisStatus);

    return ndisStatus;
}


//
// Get auto Power save OID implementation
//
NDIS_STATUS CGetAutoPowerSaveJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();
    
    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID CGetAutoPowerSaveJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceEntry();

    TraceExit(NdisStatus);
}

VOID CGetAutoPowerSaveJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
/*++

    When this function returns, the job is deleted.

--*/
{
    WDI_GET_AUTO_POWER_SAVE_RESULTS results;

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
        WFCActError("Get AutoPSM WiFiStatus2Ndis 0x%x", NdisStatus);
        goto Exit;
    }

    NdisStatus = ParseWdiGetAutoPowerSave( cb - sizeof( WFC_MESSAGE_METADATA ), pb + sizeof( WFC_MESSAGE_METADATA ), m_pAdapter->GetTlvContext(), &results );
    if (NdisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError( "Get AutoPSM Parse failed 0x%x", NdisStatus );
        goto Exit;
    }

    //
    // fill the answers if the target has the right size
    //
    DOT11_POWER_MGMT_MODE_STATUS_INFO * pAutoPsmInfo;

    if (QUERY_OID_BUFFER_LENGTH( m_pOidRequest ) < sizeof( pAutoPsmInfo ))
    {
        WFCActError("GetReceiveCoalescingMatchCount Bad output length %d\n", cb);
        QUERY_OID_BYTES_WRITTEN(m_pOidRequest) = 0;
        NdisStatus = NDIS_STATUS_BUFFER_TOO_SHORT;
        goto Exit;
    }

    pAutoPsmInfo = (DOT11_POWER_MGMT_MODE_STATUS_INFO*)QUERY_OID_BUFFER( m_pOidRequest );
    pAutoPsmInfo->Header.Type       = NDIS_OBJECT_TYPE_DEFAULT;
    pAutoPsmInfo->Header.Revision   = DOT11_POWER_MGMT_MODE_STATUS_INFO_REVISION_1;
    pAutoPsmInfo->Header.Size       = DOT11_SIZEOF_POWER_MGMT_MODE_STATUS_INFO_REVISION_1;

    pAutoPsmInfo->PowerSaveMode     = (DOT11_POWER_MODE)results.AutoPowerSaveParameters.EnableAutoPSM;
    pAutoPsmInfo->uPowerSaveLevel   = results.AutoPowerSaveParameters.PowerMode;
    pAutoPsmInfo->Reason            = (DOT11_POWER_MODE_REASON)results.AutoPowerSaveParameters.Reason;
    // Utilize these info of pAutoPsmTlv
    //UINT32 ListenIntervals;
    //UINT64 MillisecondsSinceStart;
    //UINT64 MillisecondsInPowerSave;
    //

    QUERY_OID_BYTES_WRITTEN(m_pOidRequest) = sizeof(DOT11_POWER_MGMT_MODE_STATUS_INFO);

Exit:

    QUERY_OID_BYTES_NEEDED(m_pOidRequest) = sizeof( DOT11_POWER_MGMT_MODE_STATUS_INFO );

    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit( NdisStatus );
}

void CGetAutoPowerSaveJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    m_pCPort = GetAdapter()->GetPortFromNdisPortNumber( m_OidPortNumber );
    NT_ASSERT( m_pCPort );

    ndisStatus = StartGetAutoPowerSaveCommand();

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        CompleteJob( ndisStatus );
    }

    TraceActExit( ndisStatus );
}

void CGetAutoPowerSaveJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);
    TraceActEntry();

    CompleteJob(OperationStatus);
    TraceActExit( STATUS_SUCCESS );
}

void CGetAutoPowerSaveJob::OnJobCancelled()
{
    // We are not cancellable

    TraceActEntry();
    TraceActExit( STATUS_SUCCESS );
}


NDIS_STATUS
CGetAutoPowerSaveJob::StartGetAutoPowerSaveCommand(
    )
/*++

    Private function to send QueryPower command

--*/
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;

    TraceActEntry();

    //
    // set up command message, no payload, simply use the member buffer
    //

    //
    // No TLV for the QUERY_POWER Parameters, empty payload.
    //
    ndisStatus = m_DeviceCommand.Initialize(
        m_pCPort-> GetPortId(),
        WDI_GET_AUTO_POWER_SAVE,
        c_CommandSize,
        m_pCommandBuffer );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for Get AutoPSM (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = QueueDeviceCommand(&m_DeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Get AutoPSM Task (0x%x)", ndisStatus);
        goto Exit;
    }

    Exit:
    {
        TraceActExit( ndisStatus );
        return ndisStatus;
    }
}


ULONG ChannelToFrequency( 
    _In_ ULONG ChannelNumber )
/*++
    Is this over-simpling?
--*/
{
    ULONG frequency=2437; // 2.4 channel 6

    if ( ChannelNumber <= 14 )
    {
        frequency = 2407+ ChannelNumber*5;
    }
    else if ( ChannelNumber <= 168 )
    {
        frequency = 5000+ ChannelNumber*5;
    }
    return frequency;
}


//
// CSetNloJob  OID_DOT11_OFFLOAD_NETWORK_LIST OID implementation
//
NDIS_STATUS CSetNloJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;
    m_pOffloadNetworkListInfo = (PDOT11_OFFLOAD_NETWORK_LIST_INFO)pOidRequest->
            DATA.SET_INFORMATION.InformationBuffer;

    TraceExit(ndisStatus);
    return ndisStatus;
}

VOID CSetNloJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceEntry();

    TraceExit(NdisStatus);
}

VOID CSetNloJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceActEntry();

    // Complete the OID back to NDIS
    NDIS_STATUS wiFiStatus;

    if ( NT_SUCCESS( NdisStatus ) ) 
    {
        if ( m_pOffloadNetworkListInfo->ulFlags == DOT11_NLO_FLAG_SCAN_ON_AOAC_PLATFORM ||
             m_pOffloadNetworkListInfo->ulFlags == DOT11_NLO_FLAG_STOP_NLO_INDICATION )
        {
            NdisStatus = CMessageHelper::GetStatusFromCommandResult( &m_DeviceCommand, &wiFiStatus );
            if ( NT_SUCCESS(NdisStatus)) 
            {
                NdisStatus = wiFiStatus;
                WFCInfo( "WiFiStatus=%x\n", wiFiStatus );
            }
        }
    }

    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit( NdisStatus );
}

void CSetNloJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    bool bCompleteJob = false;
    
    TraceActEntry();

    m_pCPort = GetAdapter()->GetPortFromNdisPortNumber( m_OidPortNumber );
    NT_ASSERT( m_pCPort );

    if ( m_NdisPortNumber != NDIS_DEFAULT_PORT_NUMBER )
    {
        ndisStatus = STATUS_INVALID_DEVICE_REQUEST;
    }
    else 
    {
        if ( m_pOffloadNetworkListInfo->ulFlags == DOT11_NLO_FLAG_SCAN_ON_AOAC_PLATFORM ||
            m_pOffloadNetworkListInfo->ulFlags == DOT11_NLO_FLAG_STOP_NLO_INDICATION )
        {
            //
            // CS NLO; go to work
            //
            ndisStatus = StartSetNloCommand();
        }
        else if (m_pOffloadNetworkListInfo->ulFlags == DOT11_NLO_FLAG_SCAN_AT_SYSTEM_RESUME)
        {
            ULONG listLength;
            UINT16 numOffloadEntriesToSave;

            //
            // Instant Connect NLO. We will remember this internally only
            //

            // This might truncate
            ndisStatus = RtlULongToUShort (m_pOffloadNetworkListInfo->uNumOfEntries, &numOffloadEntriesToSave);
            if (!NT_SUCCESS(ndisStatus))
            {
                ndisStatus = NTSTATUS2NDISSTATUS(ndisStatus);
                WFCError( "[NLO] Too many entries specified for NLO entries. Error = 0x%08x", ndisStatus );
                goto exit;
            }
            
            ndisStatus = RtlULongMult(numOffloadEntriesToSave, sizeof(DOT11_OFFLOAD_NETWORK), &listLength);
            if (!NT_SUCCESS(ndisStatus))
            {
                ndisStatus = NTSTATUS2NDISSTATUS(ndisStatus);
                WFCError( "[NLO] NLO entries are too big in size. Error = 0x%08x", ndisStatus );
                goto exit;
            }

            // Save the NLO list
            ndisStatus = m_pCPort->GetPropertyCache()->SetPropertyList(WfcPortPropertyResumeNLOList, 
                            listLength,
                            numOffloadEntriesToSave,
                            (BYTE *)&(m_pOffloadNetworkListInfo->offloadNetworkList));
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCError( "[NLO] Failed to save network offload list. Error = 0x%08x", ndisStatus );                
                goto exit;
            }

            bCompleteJob = true;
        }
        else
        {
            ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
            WFCInfo( "[NLO] Received unsupported Instant Connect NLO");            
        }
    }

exit:
    if ( (ndisStatus != NDIS_STATUS_SUCCESS ) || bCompleteJob)
    {
        CompleteJob( ndisStatus );
    }
    TraceActExit( ndisStatus );
}

void CSetNloJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);
    TraceActEntry();

    //
    // if non-0 NLO, get the start time.
    //
    if ( m_pOffloadNetworkListInfo->uNumOfEntries )
    {
        KeQuerySystemTime( &m_pCPort->m_liNloStart );
        if ( m_pCPort->m_pbNloCommand )
        {
            delete [] m_pCPort->m_pbNloCommand;
        }
        m_pCPort->m_FastScanIterations = m_pOffloadNetworkListInfo->FastScanIterations;
        m_pCPort->m_pbNloCommand = m_pbTlv;
        m_pCPort->m_cbNloCommand = m_cbTlv;
        InterlockedExchange( &m_pCPort->m_nNloWakes, 0 );
        m_pbTlv = NULL;
    }
    else 
    {
        // Nlo is stopping
        if ( m_pCPort->m_pbNloCommand )
        {
            delete [] m_pCPort->m_pbNloCommand;
            m_pCPort->m_pbNloCommand = NULL;
            m_pCPort->m_cbNloCommand = 0;
        }
    }
    CompleteJob(OperationStatus);

    TraceActExit( STATUS_SUCCESS );
}

void CSetNloJob::OnJobCancelled()
{
    // We are not cancellable

    TraceActEntry();
    TraceActExit( STATUS_SUCCESS );
}

NDIS_STATUS CSetNloJob::SetupInfoForTlvGenerator()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WDI_NETWORK_LIST_OFFLOAD_INFO nloParams;
    PDOT11_OFFLOAD_NETWORK pOffloadNetwork;
    PWDI_NETWORK_LIST_OFFLOAD_CONFIG pNloConfig = &nloParams.NetworkListOffloadConfig;
    ULONG nSsidOffloads = m_pOffloadNetworkListInfo->uNumOfEntries;

    TraceActEntry();
    //
    // prepare nloParams to feed tlv generator
    //
    RtlZeroMemory( &nloParams, sizeof(nloParams ));

    pNloConfig->ulFlags = m_pOffloadNetworkListInfo->ulFlags;
    pNloConfig->DelayStartInSeconds = 0;
    pNloConfig->FastScanPeriodinSeconds = m_pOffloadNetworkListInfo->FastScanPeriod;
    pNloConfig->FastScanIterations = m_pOffloadNetworkListInfo->FastScanIterations;
    pNloConfig->SlowScanPeriodinSeconds = m_pOffloadNetworkListInfo->SlowScanPeriod;

    if ( nSsidOffloads )
    {
        nloParams.Optional.SsidOffload_IsPresent = 1;

        ndisStatus = nloParams.SsidOffload.AllocateElements(nSsidOffloads, 0);
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError( "NLO SetupInfoForTlv alloc SsidOffload failed 0x%x", ndisStatus );
            goto exit;
        }
    }

    for ( ULONG i=0; i < nSsidOffloads; i++ )
    {
        PWDI_SSID_OFFLOAD_CONTAINER pSsidOffloadContainer;
        ULONG nChannelMaps;

        pOffloadNetwork = &m_pOffloadNetworkListInfo->offloadNetworkList[i];
        pSsidOffloadContainer = &nloParams.SsidOffload.pElements[i];

        //
        // Ssid
        //
        ULONG cbSsid;
        cbSsid = pOffloadNetwork->Ssid.uSSIDLength;

        pSsidOffloadContainer->SsidToScan.AllocateElements(cbSsid, 0);
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError( "NLO SetupInfoForTlv alloc Ssid failed 0x%x", ndisStatus );
            goto exit;
        }

        RtlCopyMemory( &pSsidOffloadContainer->SsidToScan.pElements[0],
                        &pOffloadNetwork->Ssid.ucSSID,
                        cbSsid);
        //
        // Auth cipher
        //
        ndisStatus = pSsidOffloadContainer->UnicastAlgorithms.AllocateElements(1, 0);
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError( "NLO SetupInfoForTlv alloc AuthCipher failed 0x%x", ndisStatus );
            goto exit;
        }
        pSsidOffloadContainer->UnicastAlgorithms.pElements[0].CipherAlgorithm = 
            CDot11ToWabiConverter::MapCipherAlgorithm(
                pOffloadNetwork->UnicastCipher);

        pSsidOffloadContainer->UnicastAlgorithms.pElements[0].AuthAlgorithm = 
            CDot11ToWabiConverter::MapAuthAlgorithm(
                pOffloadNetwork->AuthAlgo);

        //
        // Channel Maps; count first
        //
        for ( nChannelMaps=0; nChannelMaps < DOT11_MAX_CHANNEL_HINTS; nChannelMaps++ )
        {
            if ( pOffloadNetwork->Dot11ChannelHints[nChannelMaps].uChannelNumber == 
                    DOT11_INVALID_CHANNEL_NUMBER )
            {
                break;
            }
        }

        // alloc
        ASSERT( nChannelMaps > 0 );
        ndisStatus = pSsidOffloadContainer->ChannellHintList.AllocateElements(nChannelMaps, 0);
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError( "NLO SetupInfoForTlv alloc ChannleMaps failed 0x%x", ndisStatus );
            goto exit;
        }

        // fill up
        for ( ULONG j=0; j < nChannelMaps; j++ )
        {
            pSsidOffloadContainer->ChannellHintList.pElements[j].ChannelNumber =
                pOffloadNetwork->Dot11ChannelHints[j].uChannelNumber;
            pSsidOffloadContainer->ChannellHintList.pElements[j].ChannelCenterFrequency =
                ChannelToFrequency(pOffloadNetwork->Dot11ChannelHints[j].uChannelNumber);
        }
    }

    ndisStatus = GenerateWdiSetNetworkListOffload(
        (PWDI_NETWORK_LIST_OFFLOAD_PARAMETERS)&nloParams,
        (ULONG)sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &m_cbTlv,
        &m_pbTlv);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to generate TLV for SetNetworkListOffload 0x%x", ndisStatus);
        goto exit;
    }

exit:

    TraceActExit( ndisStatus );
    return ndisStatus;
}

NDIS_STATUS
CSetNloJob::StartSetNloCommand()
/*++

    Private function to send Set Nlo command

--*/
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;

    TraceActEntry();

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to generate TLV for SetAutoPowerSave 0x%x", ndisStatus);
        goto Exit;
    }

    //
    // use TLV generator
    //
    ndisStatus = SetupInfoForTlvGenerator();
    if ( ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to Generate NloTlv (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = m_DeviceCommand.Initialize(
        m_pCPort-> GetPortId(),
        WDI_SET_NETWORK_LIST_OFFLOAD,
        m_cbTlv,
        m_pbTlv );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for Set NLO (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = QueueDeviceCommand(&m_DeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Set NLO Task (0x%x)", ndisStatus);
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


//++ CSetAddWolPatternJob ++

NDIS_STATUS CSetAddWolPatternJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;
    m_pWolPattern = (PNDIS_PM_WOL_PATTERN)pOidRequest->
            DATA.SET_INFORMATION.InformationBuffer;

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID CSetAddWolPatternJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceEntry();

    TraceExit(NdisStatus);
}

VOID CSetAddWolPatternJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus )
{
    TraceActEntry();

    // Complete the OID back to NDIS
    NDIS_STATUS wiFiStatus;
    NDIS_STATUS ndisStatus;

    if ( NT_SUCCESS( NdisStatus ) ) 
    {
        //
        // if op status is good, further check the wifi status for error
        //
        NdisStatus = CMessageHelper::GetStatusFromCommandResult( 
                        &m_DeviceCommand, 
                        &wiFiStatus );

        //
        // WiFi status error is folded into NdisStatus
        //
        if ( !NT_SUCCESS(NdisStatus)) 
        {
            WFCInfo( "WiFiStatus=%x\n", wiFiStatus );
            // HCK only allows NDIS_STATUS_RESOURCES or 
            // NDIS_STATUS_PM_WOL_PATTERN_LIST_FULL 
            NdisStatus = NDIS_STATUS_RESOURCES;
        }
        else
        {
            ndisStatus = AddWolPatternToPropertyCache();
            UNREFERENCED_PARAMETER( ndisStatus );
            NT_ASSERT( ndisStatus == NDIS_STATUS_SUCCESS );
        }
    }

    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit( NdisStatus );
}

void CSetAddWolPatternJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    m_pCPort = GetAdapter()->GetPortFromNdisPortNumber( m_OidPortNumber );
    NT_ASSERT( m_pCPort );

    /*
    if (!VERIFY_NDIS_OBJECT_HEADER_DEFAULT(m_pWolPattern->Header, 
            NDIS_OBJECT_TYPE_DEFAULT,
            NDIS_PM_WOL_PATTERN_REVISION_2, // sometimes, we get version_1
            NDIS_SIZEOF_NDIS_PM_WOL_PATTERN_REVISION_2))
    {
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        WFCActError("Invalid NDIS object header");
        goto Exit;
    }
    */

    if ( m_NdisPortNumber != NDIS_DEFAULT_PORT_NUMBER )
    {
        ndisStatus = STATUS_INVALID_DEVICE_REQUEST;
    }
    else 
    {
        ndisStatus = StartSetAddWolPatternCommand();
    }

    //Exit:
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        CompleteJob( ndisStatus );
    }

    TraceActExit( ndisStatus );
}

void CSetAddWolPatternJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);
    TraceActEntry();
    CompleteJob(OperationStatus);
    TraceActExit( STATUS_SUCCESS );
}

void CSetAddWolPatternJob::OnJobCancelled()
{
    // We are not cancellable

    TraceActEntry();
    TraceActExit( STATUS_SUCCESS );
}

NDIS_STATUS
CSetAddWolPatternJob::AddWolPatternToPropertyCache()
{
    NDIS_STATUS status;

    CPort *pPort = m_pAdapter->GetPortFromNdisPortNumber( m_NdisPortNumber );
    CPropertyCache * pNewPropertyCache = new CPropertyCache;
    CPropertyCache * pPortPropertyCache;
    CPropertyCache * pFirstPropertyCache;

    if ( pNewPropertyCache == NULL )
    {
        WFCError( "AddWolPatternToPropertyCache failed alloc CPropertyCache" );
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
                WfcPortPropertyWolPattern,
                GetOidRequest()->DATA.SET_INFORMATION.InformationBufferLength,
                (BYTE*)GetOidRequest()->DATA.SET_INFORMATION.InformationBuffer);
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError("AddWolPatternToPropertyCache set new cache failed 0x%x", status);
        goto Exit;
    }

    pPortPropertyCache = pPort->GetPropertyCache();
    status = pPortPropertyCache->GetPropertyPropertyCacheList(
                    WfcPortPropertyWolPatternList,
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

    DtPrint( "Add to protperyCache WolPattern id=%d\n", m_pWolPattern->PatternId);
    status = pPortPropertyCache->SetPropertyPropertyCacheList(
                WfcPortPropertyWolPatternList,
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
CSetAddWolPatternJob::SetupInfoForTlvGenerator()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    //
    // count each type
    //
    UINT32 cWolBitMap   = 0;
    UINT32 cWolMagic    = 0;
    UINT32 cWolIpv4Sync = 0;
    UINT32 cWolIpv6Sync = 0;
    UINT32 cWolEaplId   = 0;

    PNDIS_PM_WOL_PATTERN pWolPatternTemp = m_pWolPattern;
    WDI_SET_ADD_WOL_PATTERN_PARAMETERS params;
    BYTE * pb;

    TraceActEntry();

    RtlZeroMemory( &params, sizeof( params ));
    //
    //get counts
    //
    do {
        switch ( pWolPatternTemp->WoLPacketType )
        {
            case NdisPMWoLPacketBitmapPattern:
                cWolBitMap++;
                params.Optional.WakePacketPattern_IsPresent = 1;
                break;
                
            case NdisPMWoLPacketMagicPacket:
                cWolMagic++;
                params.Optional.WakePacketMagicPacketPatternId_IsPresent = 1;
                break;

            case NdisPMWoLPacketIPv4TcpSyn:
                cWolIpv4Sync++;
                params.Optional.WakePacketIpv4TcpSync_IsPresent = 1;
                break;

            case NdisPMWoLPacketIPv6TcpSyn:
                params.Optional.WakePacketIpv6TcpSync_IsPresent = 1;
                cWolIpv6Sync++;
                break;
                
            case NdisPMWoLPacketEapolRequestIdMessage:
                params.Optional.WakePacketEapolRequestIdPatternId_IsPresent= 1;
                cWolEaplId++;
                break;
                
            default:
                NT_ASSERT( 0 );
                break;
        }
        if ( pWolPatternTemp->NextWoLPatternOffset == 0 ) {
            break;
        }
        else {
            pb = (BYTE*)pWolPatternTemp+ pWolPatternTemp->NextWoLPatternOffset;
            pWolPatternTemp = (PNDIS_PM_WOL_PATTERN)pb;
        }
    } while ( pWolPatternTemp );

    WFCActInfo( "AddWolPattern counts Bitmap=%d Magic=%d Ipv4=%d Ipv6=%d EapolId=%d\n",
                cWolBitMap,
                cWolMagic,
                cWolIpv4Sync,
                cWolIpv6Sync,
                cWolEaplId);

    //
    // fill up each type
    //
    UINT32 curWolBitMap   = 0;
    UINT32 curWolMagic    = 0;
    UINT32 curWolIpv4Sync = 0;
    UINT32 curWolIpv6Sync = 0;
    UINT32 curWolEaplId   = 0;

    //
    // allocate and create TLVs; WDI_SET_ADD_WOL_PATTERN_PARAMETERS
    //
    pWolPatternTemp = m_pWolPattern;

    do {
        switch ( pWolPatternTemp->WoLPacketType )
        {
            case NdisPMWoLPacketBitmapPattern:
            {
                if ( curWolBitMap == 0 )
                {
                    params.WakePacketPattern.AllocateElements( cWolBitMap, 0);
                }

                ASSERT( curWolBitMap < cWolBitMap );
                if ( curWolBitMap >= cWolBitMap )
                {
                    WFCError( "AddWolInternalError cWolBitMap %d too small", cWolBitMap );
                    ndisStatus = NDIS_STATUS_INVALID_PARAMETER; 
                    goto Exit;
                }

                WDI_PACKET_PATTERN_CONTAINER * pBitMap;
                pBitMap = &params.WakePacketPattern.pElements[curWolBitMap];
                //
                // id
                //
                pBitMap->WakePacketPatternId = pWolPatternTemp->PatternId;
                //
                // pattern
                //
                pBitMap->PacketPattern.SimpleAssign( 
                    (BYTE*)pWolPatternTemp + pWolPatternTemp->WoLPattern.WoLBitMapPattern.PatternOffset,
                    pWolPatternTemp->WoLPattern.WoLBitMapPattern.PatternSize );
                //
                // pattern mask
                //
                pBitMap->PacketPatternMask.SimpleAssign( 
                    (BYTE*)pWolPatternTemp + pWolPatternTemp->WoLPattern.WoLBitMapPattern.MaskOffset,
                    pWolPatternTemp->WoLPattern.WoLBitMapPattern.MaskSize );
            }
            curWolBitMap++;
            break;
                
            case NdisPMWoLPacketMagicPacket:
            {
                ASSERT( curWolMagic < cWolMagic );
                if ( curWolMagic >= cWolMagic )
                {
                    WFCError( "AddWolInternalError cWolMagic %d too small", cWolMagic );
                    ndisStatus = NDIS_STATUS_INVALID_PARAMETER; 
                    goto Exit;
                }

                //
                // id
                //
                params.WakePacketMagicPacketPatternId = pWolPatternTemp->PatternId;
            }
            curWolMagic++;
            break;

            case NdisPMWoLPacketIPv4TcpSyn:
            {
                if ( curWolIpv4Sync == 0 )
                {
                    params.WakePacketIpv4TcpSync.AllocateElements( cWolIpv4Sync, 0);
                }
            
                ASSERT( curWolIpv4Sync < cWolIpv4Sync );
                if ( curWolIpv4Sync >= cWolIpv4Sync )
                {
                    WFCError( "AddWolInternalError cWolIpv4Sync %d too small", cWolIpv4Sync );
                    ndisStatus = NDIS_STATUS_INVALID_PARAMETER; 
                    goto Exit;
                }
            
                WDI_IPv4_TCP_SYNC_CONTAINER * pIpv4;
                pIpv4 = &params.WakePacketIpv4TcpSync.pElements[curWolIpv4Sync];
                //
                // id
                //
                pIpv4->PatternId = pWolPatternTemp->PatternId;
                //
                // source addr
                //
                *(ULONG*)&pIpv4->IPv4SourceAddr = 
                    *(ULONG*)pWolPatternTemp->WoLPattern.IPv4TcpSynParameters.IPv4SourceAddress;
                //
                // dest addr
                //
                *(ULONG*)&pIpv4->IPv4DestAddr = 
                    *(ULONG*)pWolPatternTemp->WoLPattern.IPv4TcpSynParameters.IPv4DestAddress;
                //
                // TCPSourcePortNumber
                //
                pIpv4->TcpSourcePort =
                    pWolPatternTemp->WoLPattern.IPv4TcpSynParameters.TCPSourcePortNumber;
                //
                // TCPDestPortNumber
                //
                pIpv4->TcpDestPort =
                    pWolPatternTemp->WoLPattern.IPv4TcpSynParameters.TCPDestPortNumber;
            }
            curWolIpv4Sync++;
            break;

            case NdisPMWoLPacketIPv6TcpSyn:
            {
                if ( curWolIpv6Sync == 0 )
                {
                    params.WakePacketIpv6TcpSync.AllocateElements( cWolIpv6Sync, 0);
                }
            
                ASSERT( curWolIpv6Sync < cWolIpv6Sync );
                if ( curWolIpv6Sync >= cWolIpv6Sync )
                {
                    WFCError( "AddWolInternalError cWolIpv6Sync %d too small", cWolIpv6Sync );
                    ndisStatus = NDIS_STATUS_INVALID_PARAMETER; 
                    goto Exit;
                }
            
                WDI_IPv6TCP_SYNC_CONTAINER * pIpv6;
                pIpv6 = &params.WakePacketIpv6TcpSync.pElements[curWolIpv6Sync];
                //
                // id
                //
                pIpv6->PatternId = pWolPatternTemp->PatternId;
                //
                // source addr GUID just means 16 bytes
                //
                *(GUID*)&pIpv6->IPv6SourceAddr = 
                    *(GUID*)pWolPatternTemp->WoLPattern.IPv6TcpSynParameters.IPv6SourceAddress;
                //
                // dest addr
                //
                *(GUID*)&pIpv6->IPv6DestAddr = 
                    *(GUID*)pWolPatternTemp->WoLPattern.IPv6TcpSynParameters.IPv6DestAddress;
                //
                // TcpSourcePort
                //
                pIpv6->TcpSourcePort =
                    pWolPatternTemp->WoLPattern.IPv6TcpSynParameters.TCPSourcePortNumber;
                //
                // TcpDestPort
                //
                pIpv6->TcpDestPort =
                    pWolPatternTemp->WoLPattern.IPv6TcpSynParameters.TCPDestPortNumber;
            }
            curWolIpv4Sync++;
            break;

            case NdisPMWoLPacketEapolRequestIdMessage:
            {
                ASSERT( curWolEaplId < cWolEaplId );
                if ( curWolEaplId >= cWolEaplId )
                {
                    WFCError( "AddWolInternalError cWolEaplId %d too small", cWolEaplId );
                    ndisStatus = NDIS_STATUS_INVALID_PARAMETER; 
                    goto Exit;
                }
            
                //
                // id
                //
                params.WakePacketEapolRequestIdPatternId = pWolPatternTemp->PatternId;
            }
            curWolEaplId++;
            break;
                
            default:
                NT_ASSERT( 0 );
                break;
        }
        if ( pWolPatternTemp->NextWoLPatternOffset == 0 ) {
            break;
        }
        else {
            pb = (BYTE*)pWolPatternTemp+ pWolPatternTemp->NextWoLPatternOffset;
            pWolPatternTemp = (PNDIS_PM_WOL_PATTERN)pb;
        }
    } while (pWolPatternTemp);

    ndisStatus = GenerateWdiSetAddWolPattern( &params,
                    (ULONG)sizeof(WFC_MESSAGE_METADATA),
                    m_pAdapter->GetTlvContext( ),
                    &m_cbTlv,
                    &m_pbTlv);


Exit:
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError( "Generate SetAddWolPattern failed" );
    }
    TraceActExit( ndisStatus );
    return ndisStatus;
}


NDIS_STATUS
CSetAddWolPatternJob::StartSetAddWolPatternCommand()
/*++

    Private function to send Set Nlo command

--*/
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;

    TraceActEntry();

    // if this object is reused, make sure we free the buffer which was normally free in dtor
    if ( m_pbTlv )
    {
        delete [] m_pbTlv;
        m_pbTlv = NULL;
    }

    //
    // set up for TLV generator
    //
    ndisStatus = SetupInfoForTlvGenerator();
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to generate TLV for SetAddWolPatternCommand 0x%x", ndisStatus);
        goto Exit;
    }

    ndisStatus = m_DeviceCommand.Initialize(
                    m_pCPort-> GetPortId(),
                    WDI_SET_ADD_WOL_PATTERN,
                    m_cbTlv,
                    m_pbTlv );
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


//++ CSetRemoveWolPatternJob ++

NDIS_STATUS CSetRemoveWolPatternJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;
    m_PatternId = *(UINT32*)pOidRequest->DATA.SET_INFORMATION.InformationBuffer;

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID CSetRemoveWolPatternJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceEntry();

    TraceExit(NdisStatus);
}

VOID CSetRemoveWolPatternJob::OnOidJobCompletedEvent(
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
        if ( NdisStatus == NDIS_STATUS_SUCCESS )
        {
            ndisStatus = RemoveWolPatternFromPropertyCache();
            UNREFERENCED_PARAMETER( ndisStatus );
            NT_ASSERT( ndisStatus == NDIS_STATUS_SUCCESS );
        }
    }

    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit( NdisStatus );
}

void CSetRemoveWolPatternJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    m_pCPort = GetAdapter()->GetPortFromNdisPortNumber( m_OidPortNumber );
    NT_ASSERT( m_pCPort );

    if ( m_NdisPortNumber != NDIS_DEFAULT_PORT_NUMBER )
    {
        ndisStatus = STATUS_INVALID_DEVICE_REQUEST;
    }
    else 
    {
        ndisStatus = StartSetRemoveWolPatternCommand();
    }

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        CompleteJob( ndisStatus );
    }

    TraceActExit( ndisStatus );
}

void CSetRemoveWolPatternJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(SenderObject);
    TraceActEntry();
    CompleteJob(OperationStatus);
    TraceActExit( STATUS_SUCCESS );
}

void CSetRemoveWolPatternJob::OnJobCancelled()
{
    // We are not cancellable

    TraceActEntry();
    TraceActExit( STATUS_SUCCESS );
}

NDIS_STATUS
CSetRemoveWolPatternJob::RemoveWolPatternFromPropertyCache()
{
    NDIS_STATUS status=NDIS_STATUS_SUCCESS;

    CPort *pPort = m_pAdapter->GetPortFromNdisPortNumber( m_NdisPortNumber );
    CPropertyCache * pPortPropertyCache;
    CPropertyCache * pCurrPropertyCache;
    CPropertyCache *pPrevPropertyCache=NULL;
    CPropertyCache *pFirstPropertyCache=NULL;
    PNDIS_PM_WOL_PATTERN pWolPattern;
    ULONG ulSize;
    ULONG ulPatternId = *(ULONG*)(GetOidRequest()->DATA.SET_INFORMATION.InformationBuffer);

    pPortPropertyCache = pPort->GetPropertyCache();
    status = pPortPropertyCache->GetPropertyPropertyCacheList(
                        WfcPortPropertyWolPatternList,
                        &pFirstPropertyCache );
    if ( status != NDIS_STATUS_SUCCESS || pFirstPropertyCache == NULL )
    {
        WFCError("RemoveWolPatternFromPropertyCache list is empty");
        goto Exit;
    }

    pCurrPropertyCache = pFirstPropertyCache;

    DtPrint( "Remove from propertycache WolPattern =%d\n", ulPatternId );
    for ( ;pCurrPropertyCache!= NULL; pCurrPropertyCache = pCurrPropertyCache->Next )
    {
        status = pCurrPropertyCache->GetPropertyBuffer(
                    WfcPortPropertyWolPattern,
                    &ulSize,
                    (BYTE**)&pWolPattern);
        if ( status == STATUS_SUCCESS )
        {
            DtPrint( "Compare againset WolPattern id %d\n", pWolPattern->PatternId);
            if ( ulSize >= sizeof( NDIS_PM_WOL_PATTERN ) &&
                pWolPattern->PatternId == ulPatternId )
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
                        WfcPortPropertyWolPatternList,
                        pFirstPropertyCache );
        NT_ASSERT( status == NDIS_STATUS_SUCCESS );
    }
    else
    {
        WFCError("RemoveWolPatternFromPropertyCache id=%d does not exist", ulPatternId);
    }

    Exit:
    return status;

}



NDIS_STATUS
CSetRemoveWolPatternJob::StartSetRemoveWolPatternCommand()
/*++

    Private function to send Set Nlo command

--*/
{
    NDIS_STATUS ndisStatus = STATUS_SUCCESS;
    WDI_SET_REMOVE_WOL_PATTERN_PARAMETERS parameters;
    const PLACEMENT_NEW_ALLOCATION_CONTEXT pnac = { sizeof( m_pbCommandBuffer ), m_pbCommandBuffer };
    TLV_CONTEXT tlvCtx = *m_pAdapter->GetTlvContext();
    BYTE * pb = NULL;
    ULONG cb = 0;

    TraceActEntry();

    //
    // set up command message
    //
    tlvCtx.AllocationContext = (ULONG_PTR)(&pnac);
    parameters.PatternId = m_PatternId;
    ndisStatus = GenerateWdiSetRemoveWolPattern( &parameters, sizeof( WFC_MESSAGE_METADATA ), &tlvCtx, &cb, &pb );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError( "Failed to generate TLV for SetRemoveWolPattern (0x%x)", ndisStatus );
        goto Exit;
    }
    NT_ASSERT( pb == m_pbCommandBuffer );
    NT_ASSERT( cb == m_cbCommand );

    ndisStatus = m_DeviceCommand.Initialize(
        m_pCPort-> GetPortId(),
        WDI_SET_REMOVE_WOL_PATTERN,
        m_cbCommand,
        m_pbCommandBuffer );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for SetRemoveWolPattern (%x)", ndisStatus);
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


NDIS_STATUS CResumePrivacyExemptionJob::InitializeForResume(
    _In_ CAdapter * pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _In_ ULONG ulSize,
    _In_ UINT16 usEntries,
    _In_ PDOT11_PRIVACY_EXEMPTION pPrivacyExemptions)
/*++

    Intialize with a makeup OID so we can use simple OID job. The information
    for OID is retrieved from property cache.

    In this OID we dont include PDOT11_PRIVACY_EXEMPTION_LIST descriptor
    which is not needed. Besides, the descriptor requires list content in the array
    of the descriptor which would require a new allocation and copy. We avoid it.

--*/
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_OID oid = OID_DOT11_PRIVACY_EXEMPTION_LIST;
    POID_HANDLER_ENTRY pHandlerInfo;

    TraceActEntry();

    m_pAdapter = pAdapter;
    m_NdisPortNumber = NdisPortNumber;

    //
    // Make a dummy OID so we can reuse simple OID Job
    //
    usEntries; // reference it for free build
    ASSERT( ulSize == sizeof(DOT11_PRIVACY_EXEMPTION)*usEntries );

    RtlZeroMemory( &m_OidRequest, sizeof( NDIS_OID_REQUEST) );
    m_OidRequest.RequestType = NdisRequestSetInformation;
    m_OidRequest.PortNumber = NdisPortNumber;
    m_OidRequest.DATA.SET_INFORMATION.Oid = oid;
    m_OidRequest.DATA.SET_INFORMATION.InformationBufferLength = ulSize;
    m_OidRequest.DATA.SET_INFORMATION.InformationBuffer = (PVOID) pPrivacyExemptions;
    pHandlerInfo = COidJobBase::s_FindOidHandlerEntry( oid );
    NT_ASSERT( pHandlerInfo );

    status = COidJobBase::InitializeForOid(
                        pHandlerInfo->SetMethodJobType,
                        pAdapter,
                        pHandlerInfo,
                        &m_OidRequest);
    NT_ASSERT( NT_SUCCESS( status ) );

    TraceActExit( status );
    return status;
}


NDIS_STATUS CResumePrivacyExemptionJob::GetDeviceCommandData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _Inout_ PUINT  pBytesRead,
        _Inout_ PUINT  pBytesNeeded,
        _In_ UINT   ReservedHeaderLength,
        _Out_ ULONG* pCommandId,
        _Out_ ULONG* pCommandBufferLength,
        _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer )
/*++

    Replay Privacy exemption at resume hibernation.

    Note we use a make up OID whose information contains only privacyExemptionList.
           It does not include ExemptionListDescriptor. This has the following pros.
            1. The cached list is in propertyCache, there is no new mem allocation, i.e. no failure.
                PropertyCache dont include the descriptior but the list of privacy exemption.
            2. We dont go thru the minbuffer check, therefore, there is no size issue to fail.

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_PRIVACY_EXEMPTION   pExemptions; // array of exemptions
    ULONG i;
    ULONG uNumOfEntries;
    WDI_SET_PRIVACY_EXEMPTION_LIST_PARAMETERS privacyExemptionListParameters;

    TraceActEntry();

    //
    // prepare params  for TLV generator
    //
    RtlZeroMemory(&privacyExemptionListParameters, sizeof(privacyExemptionListParameters));

    *pBytesRead = 0;
    *pBytesNeeded = 0;

    //
    // Internal OID; dont check NDIS object header; also info buffer contains only list content
    //
    pExemptions = (PDOT11_PRIVACY_EXEMPTION)pInformationBuffer;
    uNumOfEntries = InformationBufferLength/sizeof( DOT11_PRIVACY_EXEMPTION );

    *pBytesRead = *pBytesNeeded;
    *pCommandId = WDI_SET_PRIVACY_EXEMPTION_LIST;

    if ( uNumOfEntries == 0 ) 
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCActError("0 entry for the WFC exemption list entries");
        goto exit;
    }

    privacyExemptionListParameters.PrivacyExemptionEntry.AllocateElements(
                            uNumOfEntries,
                            0);

    if ( privacyExemptionListParameters.PrivacyExemptionEntry.pElements == NULL )
    {
        ndisStatus = NDIS_STATUS_RESOURCES;
        WFCActError("Failed to allocate memory for the WFC exemption list entries");
        goto exit;
    }

    privacyExemptionListParameters.Optional.PrivacyExemptionEntry_IsPresent = true;

    for (i = 0; i < uNumOfEntries; i++)
    {
        privacyExemptionListParameters.PrivacyExemptionEntry.pElements[i].EtherType = 
            pExemptions[i].usEtherType;
        privacyExemptionListParameters.PrivacyExemptionEntry.pElements[i].ExemptionActionType =
            CDot11ToWabiConverter::MapExemptionActionType(
            pExemptions[i].usExemptionActionType );
        privacyExemptionListParameters.PrivacyExemptionEntry.pElements[i].ExemptionPacketType =
            CDot11ToWabiConverter::MapExemptionPacketType(
            pExemptions[i].usExemptionPacketType );
    }

    ndisStatus = GenerateWdiSetPrivacyExemptionList(
        &privacyExemptionListParameters,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        pCommandBuffer
        );

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to generate privacy exemption params tlv. Error = 0x%08x", ndisStatus);
        goto exit;
    }

exit:

    TraceActExit(ndisStatus);
    return ndisStatus;
}


NDIS_STATUS CResumePrivacyExemptionJob::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG MessageLength,
    _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader )
/*++

    Overwrite function so we don't re-add the list to property cache.

--*/
{
    OperationStatus, MessageLength, pMessageHeader;

    ASSERT( OperationStatus == NDIS_STATUS_SUCCESS );
    WFCInfo( "Resume PrivacyExempt compelte status=%x", OperationStatus );
    return OperationStatus;
}


NDIS_STATUS CResumeDefaultKeyJob::InitializeForResume(
        _In_ CAdapter * pAdapter,
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _In_ ULONG ulKeyIndex)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_OID oid = OID_DOT11_CIPHER_DEFAULT_KEY;
    POID_HANDLER_ENTRY pHandlerInfo;

    TraceActEntry();

    m_pAdapter = pAdapter;
    m_NdisPortNumber = NdisPortNumber;

    //
    // Make a dummy OID so we can reuse simple OID Job
    //
    ASSERT( pAdapter->m_DefaultKeyValues[ulKeyIndex] );
    RtlZeroMemory( &m_OidRequest, sizeof( NDIS_OID_REQUEST) );
    m_OidRequest.RequestType = NdisRequestSetInformation;
    m_OidRequest.PortNumber = NdisPortNumber;
    m_OidRequest.DATA.SET_INFORMATION.Oid = oid;
    m_OidRequest.DATA.SET_INFORMATION.InformationBufferLength = 
        pAdapter->m_DefaultKeyLengths[ulKeyIndex];
    m_OidRequest.DATA.SET_INFORMATION.InformationBuffer = 
        (PVOID) pAdapter->m_DefaultKeyValues[ulKeyIndex];
    pHandlerInfo = COidJobBase::s_FindOidHandlerEntry( oid );
    NT_ASSERT( pHandlerInfo );

    status = COidJobBase::InitializeForOid(
                        pHandlerInfo->SetMethodJobType,
                        pAdapter,
                        pHandlerInfo,
                        &m_OidRequest);
    NT_ASSERT( NT_SUCCESS( status ) );

    TraceActExit( status );
    return status;
}


ULONG CResumeDefaultKeyJob::GetNextKeyIndex(
    _In_ CAdapter * pAdapter )
/*++

    Return the keyindex of the next valid key value )pointer != null)
    If there is no more, return MAX_DEFAULT_KEYS .

--*/
{
    PDOT11_CIPHER_DEFAULT_KEY_VALUE pMyKeyValue = NULL;
    ULONG ulKey=MAX_DEFAULT_KEYS;

    for (;m_NextKeyIndex < MAX_DEFAULT_KEYS; m_NextKeyIndex++ )
    {
        pMyKeyValue = pAdapter->m_DefaultKeyValues[m_NextKeyIndex];
        if ( pMyKeyValue )
        {
            ulKey = m_NextKeyIndex;
            m_NextKeyIndex++;
            DtPrint( "Resuming Default Key[%d]\n", m_NextKeyIndex );
            WFCInfo( "Resuming Default Key[%d]", m_NextKeyIndex );
            break;
        }
    }
    return ulKey;
}


NDIS_STATUS CResumeDefaultKeyIdJob::InitializeForResume(
    _In_ CAdapter * pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber )
/*++

    Prepare the command job to set keyId that is cached in the adapter

--*/
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_OID oid = OID_DOT11_CIPHER_DEFAULT_KEY_ID;
    POID_HANDLER_ENTRY pHandlerInfo;

    TraceActEntry();

    m_pAdapter = pAdapter;
    m_NdisPortNumber = NdisPortNumber;

    if ( pAdapter->m_DefaultKeyId >= MAX_DEFAULT_KEYS ||
        pAdapter->m_DefaultKeyValues[pAdapter->m_DefaultKeyId] == NULL )
    {
        status = NDIS_STATUS_DATA_NOT_ACCEPTED;
        goto exit;
    }

    DtPrint( "Resume DefaultKeyId to [%d]\n", pAdapter->m_DefaultKeyId );
    WFCInfo( "Resume DefaultKeyId to [%d]", pAdapter->m_DefaultKeyId );

    //
    // Make a dummy OID so we can reuse simple OID Job
    //
    RtlZeroMemory( &m_OidRequest, sizeof( NDIS_OID_REQUEST) );
    m_OidRequest.RequestType = NdisRequestSetInformation;
    m_OidRequest.PortNumber = NdisPortNumber;
    m_OidRequest.DATA.SET_INFORMATION.Oid = oid;
    m_OidRequest.DATA.SET_INFORMATION.InformationBufferLength = sizeof(ULONG);
    m_OidRequest.DATA.SET_INFORMATION.InformationBuffer = (PVOID) &pAdapter->m_DefaultKeyId;
    pHandlerInfo = COidJobBase::s_FindOidHandlerEntry( oid );
    NT_ASSERT( pHandlerInfo );

    status = COidJobBase::InitializeForOid(
                        pHandlerInfo->SetMethodJobType,
                        pAdapter,
                        pHandlerInfo,
                        &m_OidRequest);
    NT_ASSERT( NT_SUCCESS( status ) );

exit:
    TraceActExit( status );
    return status;
}


