/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Reset.cpp

Abstract:

    Implementation for reset handler

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "Reset.tmh"

//==============================================================================================

CDot11ResetJob::CDot11ResetJob(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
    COidJobBase(ActivityId),
    m_pPort(NULL),
    m_APResetJob(ActivityId),
    m_DisconnectJob(ActivityId),
    m_Dot11ResetTask(ActivityId),    
    m_JobState(ResetPortJobStateNotStarted)
{
    RtlZeroMemory(&m_Request, sizeof(m_Request));
}

CDot11ResetJob::~CDot11ResetJob ()
{
    if (m_CommandBuffer)
    {
        delete[] m_CommandBuffer;
        m_CommandBuffer = NULL;
    }
}

NDIS_STATUS CDot11ResetJob::InitializeFromJob(
    _In_ CAdapter *pAdapter,
    _In_ NDIS_PORT_NUMBER PortNumber,
    _In_opt_ PDOT11_MAC_ADDRESS pMacAddress)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    POID_HANDLER_ENTRY pHandlerInfo;
    CPort *pPort;
    PDOT11_MAC_ADDRESS pResetMacAddress = NULL;

    TraceActEntry();

    // Some jobs reuse without deleting and cleaning up
    if (m_CommandBuffer)
    {
        delete[] m_CommandBuffer;
        m_CommandBuffer = NULL;
    }

    // To simplify things, use the adapter's table to find the priority, etc
    pHandlerInfo = COidJobBase::s_FindOidHandlerEntry(OID_DOT11_RESET_REQUEST);
    if (pHandlerInfo == NULL)
    {
        WFCActError("Failed to find handler entry for OID_DOT11_RESET_REQUEST");
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
        WFCActError("Failed to initialize OID job base (0x%0x)", ndisStatus);
        goto Exit;
    }

    // Set the default reset options we will be using
    m_Request.dot11ResetType = dot11_reset_type_phy_and_mac;
    m_Request.bSetDefaultMIB = TRUE;

    if (pMacAddress == NULL)
    {
        pPort = m_pAdapter->GetPortFromNdisPortNumber(PortNumber);
        pResetMacAddress = pPort->GetMacAddressForScan();
        WFCActInfo("Reset job received with null MAC. Will use  %!MACADDR!", *pResetMacAddress);
    }
    else
    {
        pResetMacAddress = pMacAddress;
        WFCActInfo("Reset job received MAC:  %!MACADDR!", *pResetMacAddress);
    }

    RtlCopyMemory(
        m_Request.dot11MacAddress, 
        pResetMacAddress,
        sizeof(DOT11_MAC_ADDRESS));

Exit:

    TraceActExit(ndisStatus);

    return ndisStatus;
}


NDIS_STATUS CDot11ResetJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_RESET_REQUEST dot11ResetRequest = 
        (PDOT11_RESET_REQUEST)pOidRequest->DATA.METHOD_INFORMATION.InformationBuffer;
    
    TraceActEntry();
    
    //
    // First make sure the input buffer is large enough to
    // hold a RESET_CONFIRM
    //
    if (pOidRequest->DATA.METHOD_INFORMATION.OutputBufferLength < sizeof(DOT11_STATUS_INDICATION)) 
    {
        pOidRequest->DATA.METHOD_INFORMATION.BytesNeeded = sizeof(DOT11_STATUS_INDICATION);
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        goto Exit;
    }
    
    //
    // Validate the buffer length
    //
    if (pOidRequest->DATA.METHOD_INFORMATION.InputBufferLength < sizeof(DOT11_RESET_REQUEST)) 
    {
        pOidRequest->DATA.METHOD_INFORMATION.BytesNeeded = sizeof(DOT11_RESET_REQUEST);
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        goto Exit;
    }
    
    //
    // Validate the buffer
    //
    switch (dot11ResetRequest->dot11ResetType) {
        case dot11_reset_type_phy:
        case dot11_reset_type_mac:
        case dot11_reset_type_phy_and_mac:
            break;

        default:
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            goto Exit;
        }
    }
    
    pOidRequest->DATA.METHOD_INFORMATION.BytesRead = sizeof(DOT11_RESET_REQUEST);

    RtlCopyMemory(
        &m_Request, 
        pOidRequest->DATA.METHOD_INFORMATION.InformationBuffer,
        sizeof(DOT11_RESET_REQUEST));

    WFCActInfo("Reset OID received with MAC:  %!MACADDR!", m_Request.dot11MacAddress);

    // if MAC Address in request is set to all zeroes, keep the current port adapter
    if (m_Request.dot11MacAddress[0] == 0 &&
        m_Request.dot11MacAddress[1] == 0 &&
        m_Request.dot11MacAddress[2] == 0 &&
        m_Request.dot11MacAddress[3] == 0 &&
        m_Request.dot11MacAddress[4] == 0 &&
        m_Request.dot11MacAddress[5] == 0)
    {
        // m_pPort is not yet cached since we are not yet serialized
        CPort *pPort;        
        pPort = m_pAdapter->GetPortFromNdisPortNumber(m_NdisPortNumber);

        // This should never happen since the OID came down on this port
        NT_ASSERTMSG("OID_DOT11_RESET_REQUEST on NdisPort that is not currently present", pPort);
        
        if (pPort)
        {
            RtlCopyMemory( &m_Request.dot11MacAddress, 
                            pPort->GetMacAddress(),
                            sizeof( DOT11_MAC_ADDRESS));
            WFCActTrace("[MacRandom] Using current MAC address for Reset:  %!MACADDR!", m_Request.dot11MacAddress);
        }
        else
        {
            WFCActError("Reset OID received on invalid Ndis Port number %d", m_NdisPortNumber);
            ndisStatus = NDIS_STATUS_INVALID_PORT;
            goto Exit;
        }
    }
    
Exit:
    TraceActExit(ndisStatus);
    
    return ndisStatus;
}


VOID CDot11ResetJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    PDOT11_STATUS_INDICATION    dot11StatusIndication 
        = (PDOT11_STATUS_INDICATION) METHOD_OID_BUFFER(m_pOidRequest);

    TraceActEntry();

    //
    // If the address has changed, update the port object.
    // This will trigger the notification of address change to NDIS if required.
    //
    m_pPort->SetMacAddress((DOT11_MAC_ADDRESS*)&m_Request.dot11MacAddress);
    
    //
    // Complete the reset request with appropriate status to NDIS
    //
    dot11StatusIndication->uStatusType = DOT11_STATUS_RESET_CONFIRM;
    dot11StatusIndication->ndisStatus = NdisStatus;
    METHOD_OID_BYTES_WRITTEN(m_pOidRequest) = sizeof(DOT11_STATUS_INDICATION);

    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit(NdisStatus);
}


NDIS_STATUS CDot11ResetJob::StartPortDataPathCleanup()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    m_JobState = ResetPortJobStateWaitingForDataPathCleanup;

    WFCActInfo("Cleaning up Port Data Path");

    ndisStatus = m_pPort->Dot11Reset(this);
    if ( ndisStatus == NDIS_STATUS_PENDING )
    {
        SetPending();
    }

    if (ndisStatus != NDIS_STATUS_PENDING) 
    {
        // Since the model is always sync, call
        // OnJobStepCompleted when we are doing a sync complete
        CDot11ResetJob::OnJobStepCompleted(
            ndisStatus,
            this);
    }
    else
    {
        // This is success for the start
        ndisStatus = NDIS_STATUS_SUCCESS;
    }

    TraceActExit(ndisStatus);

    return ndisStatus;
}

NDIS_STATUS CDot11ResetJob::CompletePortDataPathCleanup(
    _In_ NDIS_STATUS OperationStatus)
{
    // Job returned the final code
    WFCActInfo("Port Data Path cleanup completed (0x%x)", OperationStatus);

    return OperationStatus;    
}


NDIS_STATUS CDot11ResetJob::StartPortActiveJobsCompletion()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    m_JobState = ResetPortJobStateWaitingForActiveJobsCompletion;

    WFCActInfo("Cleaning up active jobs");

    ndisStatus = m_pAdapter->GetActiveJobsList()->StartResetForPort(
        m_NdisPortNumber,
        this
        );
    if ( ndisStatus == NDIS_STATUS_PENDING )
    {
        SetPending();
    }

    if (ndisStatus != NDIS_STATUS_PENDING) 
    {
        // Since the model is always sync, call
        // OnJobStepCompleted when we are doing a sync complete
        CDot11ResetJob::OnJobStepCompleted(
            ndisStatus,
            this);
    }
    else
    {
        // This is success for the start
        ndisStatus = NDIS_STATUS_SUCCESS;
    }

    TraceActExit(ndisStatus);

    return ndisStatus;
}

NDIS_STATUS CDot11ResetJob::CompletePortActiveJobsCompletion(
    _In_ NDIS_STATUS OperationStatus)
{
    // Job returned the final code
    WFCActInfo("Active jobs cleanup completed (0x%x)", OperationStatus);

    return OperationStatus;    
}


NDIS_STATUS CDot11ResetJob::StartPortDisconnect()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN completeStep = FALSE;
    ULONG currentOpMode;
    
    TraceActEntry();

    ndisStatus = GetPortPropertyCache()->GetPropertyULong(
        WfcPortCurrentOperationalMode,
        &currentOpMode );

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Could not retrieve current opmode from property cache.");
        NT_ASSERT(FALSE);
        goto Exit;
    }

    // Save the start state so that when complete happens, we know where we are
    m_JobState = ResetPortJobStateWaitingForDisconnectCompletion;
    
    if ( m_pPort->GetPortType() == WfcPortTypeExtAP ||
         ( m_pPort->GetPortType() == WfcPortTypeWFDRole &&
           ( m_pPort->GetAPStarted() ||
             ( currentOpMode & WDI_OPERATION_MODE_P2P_GO ) == WDI_OPERATION_MODE_P2P_GO ) ) )
    {
        if ( m_pPort->GetDot11State() == WfcPortDot11StateInit)
        {
            // Do nothing here, just complete step and move on 
            WFCActInfo("Skipping Stop AP job. It is in Init state.");
            ndisStatus = NDIS_STATUS_SUCCESS;
            completeStep = TRUE;
            goto Exit;
        }

        ndisStatus = m_APResetJob.InitializeFromJob(
            m_pAdapter,
            m_NdisPortNumber );
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCActError("Failed to initialize reset AP Job (0x%x)", ndisStatus);
            goto Exit;
        }

        ndisStatus = StartChildJob( &m_APResetJob );
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCActError("Failed to start AP Reset job (0x%x)", ndisStatus);
            goto Exit;
        }
    }
    else
    if ( m_pPort->GetPortType() == WfcPortTypeExtSTA ||
              ( m_pPort->GetPortType() == WfcPortTypeWFDRole &&
                ( currentOpMode & WDI_OPERATION_MODE_P2P_CLIENT ) == WDI_OPERATION_MODE_P2P_CLIENT ) )
    {
        if ( m_pPort->GetDot11State() == WfcPortDot11StateInit)
        {
            // Do nothing here, just complete step and move on 
            WFCActInfo("Skipping disconnect job, it is in init state.");
            ndisStatus = NDIS_STATUS_SUCCESS;
            completeStep = TRUE;
            goto Exit;
        }
    
        // Launch the child job to cause disconnect on the port
        ndisStatus = m_DisconnectJob.InitializeFromJob(
            m_pAdapter,
            m_NdisPortNumber,
            false,
            WDI_ASSOC_STATUS_DISASSOCIATED_BY_HOST,
            DOT11_MGMT_REASON_DEAUTH_LEAVE_SS
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("Failed to initialize Disconnect/Stop Job (0x%x)", ndisStatus);
            goto Exit;
        }

        ndisStatus = StartChildJob(&m_DisconnectJob);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("Failed to start Disconnect/Stop Job (0x%x)", ndisStatus);
            goto Exit;
        }
    }
    else
    {
        // Do nothing here, just complete step and move on 
        ndisStatus = NDIS_STATUS_SUCCESS;
        completeStep = TRUE;
        goto Exit;
    }

Exit:
    if (completeStep)
    {
        // Since the model is always sync, call
        // OnJobStepCompleted when we are doing a sync complete
        CDot11ResetJob::OnJobStepCompleted(
            ndisStatus,
            this);
    }
    
    TraceActExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CDot11ResetJob::CompletePortDisconnect(
    _In_ NDIS_STATUS OperationStatus)
{
    // Job returned the final code
    WFCActInfo("Port Disconnect/Stop Job completed (0x%x)", OperationStatus);
   
    return OperationStatus;
}



NDIS_STATUS CDot11ResetJob::StartPortSwitchToINITState()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;    

    WDI_TASK_DOT11_RESET_PARAMETERS resetParameters;
    RtlZeroMemory(&resetParameters, sizeof(WDI_TASK_DOT11_RESET_PARAMETERS));

    TraceActEntry();

    // Save the start state so that when complete happens, we know where we are
    m_JobState = ResetPortJobStateWaitingForPortINITState;
    
    ndisStatus = m_Dot11ResetTask.Initialize(
        m_pAdapter->GetNotificationManager(),
        m_pAdapter->GetDeviceCommandScheduler(),
        m_pAdapter->GetEventQueue() );

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Failed to initialize reset task (0x%x)", ndisStatus);
        goto exit;
    }

    DeviceCommand* pTaskDeviceCommand = NULL;

    ndisStatus = m_Dot11ResetTask.get_TaskDeviceCommand(
        &pTaskDeviceCommand );

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't get task device command (0x%x)", ndisStatus);
        goto exit;
    }

    ULONG CommandLength;
    resetParameters.Dot11ResetParameters.SetDefaultMIB = m_Request.bSetDefaultMIB;

    // If the request specifies a mac address different from the current one, need to include the parameter.
    if (RtlEqualMemory(m_pPort->GetMacAddress(), &m_Request.dot11MacAddress, sizeof(DOT11_MAC_ADDRESS)))
    {
        resetParameters.Optional.ResetMACAddress_IsPresent = 0;
        WFCActInfo("[MACRandom] Port Dot11 Reset will not change the MAC Address");
    }
    else
    {
        resetParameters.Optional.ResetMACAddress_IsPresent = 1;
        RtlCopyMemory(
            &resetParameters.ResetMACAddress.Address,
            &m_Request.dot11MacAddress, sizeof(DOT11_MAC_ADDRESS));
        WFCActInfo("[MACRandom] Port Dot11 Reset will change the MAC Address to %!MACADDR!", resetParameters.ResetMACAddress.Address);
    }

    ndisStatus = GenerateWdiTaskDot11Reset(
        &resetParameters,
        (ULONG)sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &CommandLength,
        (UINT8**)&m_CommandBuffer
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to generate TLV for WDI_TASK_DOT11_RESET (0x%x)", ndisStatus);
        goto exit;
    }

    ndisStatus = pTaskDeviceCommand->Initialize(
        get_PortId(),
        WDI_TASK_DOT11_RESET,
        CommandLength,
        m_CommandBuffer );

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Failed to initialize Dot11 Reset task device command (0x%x)", ndisStatus );
        goto exit;
    }

    ndisStatus = StartTask(
        &m_Dot11ResetTask );

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Failed to start Dot11 Reset task (0x%x)", ndisStatus );
        goto exit;
    }

exit:

    TraceActExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CDot11ResetJob::CompletePortSwitchToINITState(
    _In_ NDIS_STATUS OperationStatus)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_STATUS commandStatus;

    // Job returned the final code
    WFCActInfo("Port Dot11 Reset Task completed (0x%x)", OperationStatus);

    TraceActEntry();

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        status = OperationStatus;
        WFCActErrorStatus("Dot11 Reset operation failed.");
        // We still proceed
    }
    else
    {
        status = CMessageHelper::GetStatusFromTaskOutput(
            &m_Dot11ResetTask,
            &commandStatus );

        if ( commandStatus != NDIS_STATUS_SUCCESS )
        {
            status = commandStatus;
            WFCActErrorStatus("Dot11 Reset WDI command failed.");
        }
        else
        {
            // Remember the new MAC Address
            m_pPort->SetMacAddress(&m_Request.dot11MacAddress);
        }

        OperationStatus = commandStatus;
    }
    
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        // TODO: WDI_DISABLED
        // TODO: workaround: ignore the error and assign some PortID
        WFCActError("WORKAROUND: Ignoring failure from DOT11_RESET task");
        OperationStatus = NDIS_STATUS_SUCCESS;

    }

    // We are now in init state
    m_pPort->SetDot11State(WfcPortDot11StateInit);

    // Flush any neighbor reports
    CBSSListManager *pBSSListManager = m_pAdapter->GetBSSListManager(WfcPortTypeExtSTA);
    pBSSListManager->FlushAllAssociationState();

    m_pPort->GetConnectedPeerList()->FlushPeerList();

    //Cancel opportunistic roam timer (if running)
    m_pPort->CancelOpportunisticRoam();

    // Reset media streaming refcount if necessary
    {
        BOOLEAN mediaStreamingEnabled;
        ULONG mediaStreamingRefCount;

        if ( m_pPort->GetPropertyCache()->GetPropertyBoolean(
                 WfcPortPropertyMediaStreamingEnabled,
                 &mediaStreamingEnabled ) == NDIS_STATUS_SUCCESS &&
             m_pAdapter->GetAdapterPropertyCache()->GetPropertyULong(
                 WfcAdapterPropertyMediaStreamingRefcount,
                 &mediaStreamingRefCount ) == NDIS_STATUS_SUCCESS &&
             mediaStreamingEnabled &&
             mediaStreamingRefCount > 0 )
        {
            WFCActInfo(
                "Media streaming was on during reset operation. Resetting media streaming setting and decrementing ref-count." );
            mediaStreamingEnabled = FALSE;
            mediaStreamingRefCount--;

            m_pAdapter->GetAdapterPropertyCache()->SetPropertyULong(
                WfcAdapterPropertyMediaStreamingRefcount,
                mediaStreamingRefCount );

            m_pPort->GetPropertyCache()->SetPropertyBoolean(
                WfcPortPropertyMediaStreamingEnabled,
                mediaStreamingEnabled );
        }
    }

    // Reset the state
    m_pPort->GetPropertyCache()->Dot11Reset (m_Request.bSetDefaultMIB);

    TraceActExit(OperationStatus);    
    return OperationStatus;
}

void CDot11ResetJob::FinishJob(
    _In_ NDIS_STATUS JobCompleteStatus)
{
    CompleteJob(JobCompleteStatus);
}

void CDot11ResetJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    //
    // get the port object
    //
    m_pPort = m_pAdapter->GetPortFromNdisPortNumber(m_NdisPortNumber);
    ASSERT(m_pPort);

    // Trigger Disconnect
    ndisStatus = StartPortDisconnect();
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to disconnect port (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:
    // If anything failed, we will complete the job right now
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        FinishJob(ndisStatus);
    }

    TraceActExit(ndisStatus);    
}

void CDot11ResetJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{

    BOOLEAN completeJob = FALSE;
    
    TraceActEntry();

    UNREFERENCED_PARAMETER(SenderObject);

    switch(m_JobState)
    {
        case ResetPortJobStateWaitingForDisconnectCompletion:
            {
                OperationStatus = CompletePortDisconnect(OperationStatus);

                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    OperationStatus = StartPortActiveJobsCompletion();
                }
            }
            break;

        case ResetPortJobStateWaitingForActiveJobsCompletion:
            {
                OperationStatus = CompletePortActiveJobsCompletion(OperationStatus);

                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    OperationStatus = StartPortDataPathCleanup();
                }
            }
            break;

        case ResetPortJobStateWaitingForDataPathCleanup:
            {
                OperationStatus = CompletePortDataPathCleanup(OperationStatus);

                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    OperationStatus = StartPortSwitchToINITState();
                }
            }
            break;

        case ResetPortJobStateWaitingForPortINITState:
            {
                OperationStatus = CompletePortSwitchToINITState(OperationStatus);

                completeJob = TRUE;
            }
            break;
        default:
            NT_ASSERT(FALSE);
            break;
    }

    if ((OperationStatus != NDIS_STATUS_SUCCESS) ||
        (completeJob == TRUE))
    {
        FinishJob(OperationStatus);
    }
    
    TraceActExit(OperationStatus);
    
}

void CDot11ResetJob::OnJobCancelled()
{
    // We do nothing yet
}
