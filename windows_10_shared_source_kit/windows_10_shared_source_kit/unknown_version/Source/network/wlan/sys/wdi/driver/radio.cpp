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
#include "radio.tmh"

//
//++ CSetRadioStateOffJob : a special job to set radio off at Adapter init
//      if persistent Software radio is off
//
NDIS_STATUS 
CSetRadioStateJob::InitializeFromJob(
    _In_ CAdapter *pAdapter,
    _In_ NDIS_PORT_NUMBER PortNumber,
    _In_ BOOLEAN RadioState
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    POID_HANDLER_ENTRY pHandlerInfo;

    TraceActEntry();

    // Some jobs reuse without deleting and cleaning up
    if (m_pTlvBuffer)
    {
        delete[] m_pTlvBuffer;
        m_pTlvBuffer = NULL;
    }

    // To simplify things, use the adapter's table to find the priority, etc
    pHandlerInfo = COidJobBase::s_FindOidHandlerEntry(OID_DOT11_NIC_POWER_STATE);
    if (pHandlerInfo == NULL)
    {
        WFCActError("[Radio] Failed to find handler entry for OID_DOT11_NIC_POWER_STATE");
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto Exit;

    }

    // Initialize the base
    ndisStatus = COidJobBase::InitializeWithoutOid(
                    pHandlerInfo->SetMethodJobType,
                    pAdapter,
                    pHandlerInfo->IsPreemptable,
                    pHandlerInfo->CanPreempt,
                    pHandlerInfo->PriorityScope,
                    pHandlerInfo->Priority,
                    PortNumber
                    );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Radio] Failed to initialize OID job base (0x%0x)", ndisStatus);
        goto Exit;
    }

    m_OidRadioState = RadioState;

Exit:

    TraceActExit(ndisStatus);
    return ndisStatus;
}


//
//++ CSetRadioStateJob   implementation =====
//
NDIS_STATUS CSetRadioStateJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();
    
    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;

    m_OidRadioState = *(BOOLEAN*)
            pOidRequest->DATA.SET_INFORMATION.InformationBuffer;

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID CSetRadioStateJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceEntry();
    TraceExit(NdisStatus);
}

VOID CSetRadioStateJob::OnOidJobCompletedEvent(
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

void CSetRadioStateJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN bCurrSwRadioOff = FALSE;
    ULONG currPhyId;
    CPort *pPort;

    TraceActEntry();

    m_RadioOffResetLoop = 0;
    m_pPort = GetAdapter()->GetPortFromNdisPortNumber( m_OidPortNumber );
    NT_ASSERT( m_pPort );

    //
    // check if the status has not changed.
    //
    CAdapterPropertyCache* adapterPropertyCache = m_pAdapter->GetAdapterPropertyCache();

    adapterPropertyCache->GetPropertyBoolean(
        WfcAdapterSoftwareRadioOff,
        &bCurrSwRadioOff);

    currPhyId = GetPortPropertyCache()->GetPropertyULongOrDefault(
        WfcPortOSConfiguredPhyID,
        DOT11_PHY_ID_ANY);

    DtPrint( "SetRadio PhyId=%d state curr=%d to %d\n", currPhyId, !bCurrSwRadioOff, m_OidRadioState );
    WFCInfo( "[Radio] Set Software Radio State on NDIS Port %d for PhyId=%d from Current=%d to New=%d ", m_OidPortNumber, currPhyId, !bCurrSwRadioOff, m_OidRadioState);

    if ( (!bCurrSwRadioOff) == m_OidRadioState )
    {
        BOOLEAN bCurrHwRadioOff;
        DOT11_PHY_STATE_PARAMETERS phyStateParams;

        //
        // no change, indicate up and complete
        //
        WFCTrace( "[Radio] Set Software Radio to the same state %d", m_OidRadioState);

        adapterPropertyCache->GetPropertyBoolean(
            WfcAdapterHardwareRadioOff,
            &bCurrHwRadioOff);

        phyStateParams.uPhyId = DOT11_PHY_ID_ANY;
        phyStateParams.bHardwarePhyState = (!bCurrHwRadioOff);
        phyStateParams.bSoftwarePhyState = m_OidRadioState;

        ASSIGN_NDIS_OBJECT_HEADER(
            phyStateParams.Header, 
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_PHY_STATE_PARAMETERS_REVISION_1,
            sizeof(DOT11_PHY_STATE_PARAMETERS) );

        WFCInfo( "[Radio] Synchronously Indicating Radio State on NDIS Port %d Hardware=%d Software=%d",
                    m_OidPortNumber,
                    phyStateParams.bHardwarePhyState,
                    phyStateParams.bSoftwarePhyState);

        m_pAdapter->IndicateStatus(
            m_OidPortNumber,
            NDIS_STATUS_DOT11_PHY_STATE_CHANGED,
            NULL,
            &phyStateParams,
            sizeof(phyStateParams) );

        CompleteJob( ndisStatus );
        goto exit;
    }

    // We are going to change the radio state. Save it 
    // as the target radio off state (if request = TRUE, save RadioOff = FALSE)
    ndisStatus = adapterPropertyCache->SetPropertyBoolean(
        WfcAdapterDesiredSoftwareRadioOff,
        m_OidRadioState ? FALSE : TRUE      // Setting: ON, Save: FALSE
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError( "[Radio] Failed to remember target Software Radio Off state (%x)", ndisStatus );
        // continue
        ndisStatus = NDIS_STATUS_SUCCESS;        
    }
        

    if ( !m_OidRadioState ) 
    {
        //
        // turning off radio, reset all ports first
        //
        pPort = GetNextPortToReset();

        NT_ASSERT( pPort ); // at least one port ourself.
        
        ndisStatus = StartDot11ResetSubJob( pPort );
        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            m_JobState = JobStateAwaitResetComplete;
            goto exit;
        }
        else
        {
            WFCError( "[Radio] StartDot11ResetSubJob failed (%x)", ndisStatus );
            CompleteJob( ndisStatus );
        }
    }
    else 
    {
        //
        // turn on radio, go stright to set it up
        ndisStatus = StartSetRadioStateCommand();
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            CompleteJob( ndisStatus );
        }
        else
        {
            m_JobState = JobStateAwaitSetCommandComplete;
        }
    }

exit:
    TraceActExit( ndisStatus );
}

NDIS_STATUS
CSetRadioStateJob::StartDot11ResetSubJob(
    _In_ CPort * pPort )
/*++

    Private function to send dot11reset job before we set radio off.

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
        WFCError( "[Radio] SetRadio Initialize Reset job failed (%x)", ndisStatus );
        goto Exit;
    }

    ndisStatus = StartChildJob( &m_CDot11ResetJob );
    
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError( "[Radio] StartChildJob failed (%x)", ndisStatus );
        goto Exit;
    }

    Exit: 
    {
        TraceActExit( ndisStatus );
        return ndisStatus;
    }
}



void CSetRadioStateJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    NDIS_STATUS status=NDIS_STATUS_SUCCESS;
    CPort * pPort;
    
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);
    TraceActEntry();

    switch ( m_JobState )
    {
        case JobStateAwaitResetComplete:

            pPort = GetNextPortToReset();
            if ( pPort ) 
            {
                //
                // more port to reset, reamin in the same job state
                //
                status = StartDot11ResetSubJob( pPort );
                if (status == NDIS_STATUS_SUCCESS)
                {
                    m_JobState = JobStateAwaitResetComplete;
                    goto exit;
                }
                else
                {
                    WFCError( "[Radio] StartDot11ResetSubJob failed (%x)", status );
                    CompleteJob( status );
                }
            }
            else 
            {
                // Flush the scan and WFD device 
                CBSSListManager *pListManager;

                pListManager = m_pAdapter->GetBSSListManager(WfcPortTypeExtSTA);

                WFCActTrace( "[Radio] Flushing the station bss list");
                pListManager->FlushAllEntries(true);

                pListManager = m_pAdapter->GetBSSListManager(WfcPortTypeWFDDevice);
                WFCActTrace( "[Radio] Flushing the WFD device list");
                pListManager->FlushAllEntries(true);
            
                //
                // no more port to reset, send the radio off command
                //
                status = StartSetRadioStateCommand();
                if ( status != NDIS_STATUS_SUCCESS )
                {
                    CompleteJob( status );
                }
                else
                {
                    m_JobState = JobStateAwaitSetCommandComplete;
                }
            }
            
            break;
            
        case JobStateAwaitSetCommandComplete:

            if ( OperationStatus != NDIS_STATUS_SUCCESS )
            {
                WFCError("[Radio] SetRadioCommand failed");
                status = OperationStatus;
                CompleteJob(OperationStatus);
                goto exit;
            }

            OperationStatus = CMessageHelper::GetStatusFromTaskOutput(
                &m_Task,
                &status);

            if (OperationStatus != NDIS_STATUS_SUCCESS)                // If status != SUCCESS, OperationStatus != SUCCESS
            {
                WFCError("[Radio] Radio state change task failed (%x)", OperationStatus);
                CompleteJob(OperationStatus);
                goto exit;
            }

            //
            // This is a Task command, Indication path will update our status
            //
            CompleteJob(OperationStatus);
        }

exit:

    TraceActExit( OperationStatus );
}

void CSetRadioStateJob::OnJobCancelled()
{
    // We are not cancellable

    TraceActEntry();
    TraceActExit( STATUS_SUCCESS );
}


//-- CSetRadioStateJob   implementation =====

CPort * CSetRadioStateJob::GetNextPortToReset()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CPort * pPort = NULL;
    
    for ( ;m_RadioOffResetLoop < m_pAdapter->c_MaxPorts; m_RadioOffResetLoop++ )
    {
        ULONG uOpmodeMask = 0;

        if ( m_pAdapter->m_pPortList[m_RadioOffResetLoop] == NULL )
        {
            continue;
        }

        //
        // If this is a P2P Device port, do not Reset it
        //
        pPort = m_pAdapter->m_pPortList[m_RadioOffResetLoop];
        status = pPort->GetPropertyCache()->GetPropertyULong(WfcPortCurrentOperationalMode, &uOpmodeMask);
        if ((status == NDIS_STATUS_SUCCESS) &&
            (uOpmodeMask & WDI_OPERATION_MODE_P2P_DEVICE))
        {
            WFCActInfo( "[Radio] Not Resetting P2P Device port on power state change");
            pPort = NULL;
            continue;
        }

        m_RadioOffResetLoop++;
        break;
    }
    return pPort;
}


NDIS_STATUS
CSetRadioStateJob::StartSetRadioStateCommand(
    )
/*++

    Private function to send SetReceiveCoalescing command

--*/
{
    NDIS_STATUS status = STATUS_SUCCESS;
    DeviceCommand * pDeviceCommand;
    WDI_SET_RADIO_STATE_PARAMETERS    setParameters;
    //UINT32 cbRead;

    TraceActEntry();

    //
    // initialize task
    //
    status = m_Task.Initialize(
                m_pAdapter->GetNotificationManager(),
                m_pAdapter->GetDeviceCommandScheduler(),
                m_pAdapter->GetEventQueue());

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("[Radio] Failed to initialize SetRadioState task.");
        goto Exit;
    }

    //
    // init task and  command
    //
    status = m_Task.get_TaskDeviceCommand( &pDeviceCommand );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("[Radio] Couldn't get device command for SetRadioState task.");
        goto Exit;
    }

    RtlZeroMemory(&setParameters, sizeof(WDI_SET_RADIO_STATE_PARAMETERS));
    setParameters.SoftwareRadioState = m_OidRadioState;
    status = GenerateWdiTaskSetRadioState(&setParameters,
        (ULONG)sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &m_TlvLength,
        &m_pTlvBuffer);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("[Radio] Failed to generate TLV for SetRadioState task.");
        goto Exit;
    }    

    status = pDeviceCommand->Initialize(
            WDI_PORT_ID_ADAPTER,  // m_pPort-> GetPortId(), use adapter port
            WDI_TASK_SET_RADIO_STATE,
            m_TlvLength,
            m_pTlvBuffer );
            
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Radio] Failed to initialize Device Command for SetRadioState 0x%x", status);
        goto Exit;
    }

    //
    // rock and roll
    //
    status = StartTask( &m_Task );
    
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("[Radio] Couldn't start SetRadio task.");
        goto Exit;
    }
    
    //need states ? m_State 

    Exit:
    {
        if ( status != NDIS_STATUS_SUCCESS )
        {
            CompleteJob( status );
        }
        TraceActExit( status );
        return status;
    }
}

//++    Hanlde GET OID_DOT11_NIC_POWER_STATE or OID_DOT11_HARDWARE_PHY_STATE ++ from cache

NDIS_STATUS 
CGetRadioStateJob::QueryData(
        _In_ PVOID  pInformationBuffer,
        _In_ UINT   InformationBufferLength,
        _In_ PUINT  pBytesWritten,
        _In_ PUINT  pBytesNeeded )
/*++

--*/
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CAdapterPropertyCache * pAdapterPropertyCache;
    BOOLEAN cachedRadioOffState;

    TraceActEntry();

    if ( InformationBufferLength < sizeof(BOOLEAN) )
    {
        *pBytesNeeded = sizeof(BOOLEAN);
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto Exit;
    }

    // Radio states are simplied to be in sync at adapter layer.
    //otherwies :
    //1. Get current PhyId
    //2. Find the PhyType
    //3. Look up the type in BandInfo (curr property cache get the first match only->ToFix)
    // AND the hardware and software state
    //

    pAdapterPropertyCache = m_pAdapter->GetAdapterPropertyCache();
    status = pAdapterPropertyCache->GetPropertyBoolean(
                m_bHardwareQuery ? WfcAdapterHardwareRadioOff : WfcAdapterSoftwareRadioOff,
                &cachedRadioOffState );
                
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("[Radio] Unable to find current PhyId in Property Cache. Error = 0x%08x", status);
        goto Exit;
    }

    *(BOOLEAN*)pInformationBuffer = (!( cachedRadioOffState ));

    WFCActTrace( "[Radio] Reporting %s Radio State on NDIS Port %d as %d ", 
        m_bHardwareQuery ? "Hardware" : "Software",
        GetNdisPortNumber(),
        *(BOOLEAN*)pInformationBuffer
        );
    
    *pBytesWritten = sizeof(BOOLEAN);

Exit:
    TraceExit(status);
    return status;
}


