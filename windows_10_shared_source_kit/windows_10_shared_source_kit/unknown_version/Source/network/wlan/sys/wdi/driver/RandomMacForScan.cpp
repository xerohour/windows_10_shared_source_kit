
#include "precomp.hpp"
#include "RandomMacForScan.tmh"


CDot11RandomMacForScan::CDot11RandomMacForScan(
    _In_ WFC_ACTIVITY_ID ActivityId) : COidJobBase(ActivityId),
    m_JobState(RandomMacForScanNotStarted),
    m_pPort(NULL),
    m_ResetJob(ActivityId),
    m_bOidAutoPowerSave(FALSE),
    m_ConnectionQualityJob(ActivityId),
    m_DeviceCommand(ActivityId)
{
    RtlZeroMemory(&m_MacAddress, sizeof(m_MacAddress));
}

CDot11RandomMacForScan::~CDot11RandomMacForScan()
{
}

// COidJobBase
NDIS_STATUS CDot11RandomMacForScan::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    //
    // Validate the buffer length
    //
    if (pOidRequest->DATA.METHOD_INFORMATION.InputBufferLength < sizeof(DOT11_MAC_ADDRESS))
    {
        pOidRequest->DATA.METHOD_INFORMATION.BytesNeeded = sizeof(DOT11_MAC_ADDRESS);
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        goto Exit;
    }

    pOidRequest->DATA.METHOD_INFORMATION.BytesRead = sizeof(DOT11_MAC_ADDRESS);

    RtlCopyMemory(
        &m_MacAddress,
        pOidRequest->DATA.METHOD_INFORMATION.InformationBuffer,
        sizeof(DOT11_MAC_ADDRESS));

    WFCActInfo("[MacRandom] Dot11 Set Random Mac For Scan to: %!MACADDR!", m_MacAddress);


Exit:
    TraceActExit(ndisStatus);

    return ndisStatus;
}

VOID CDot11RandomMacForScan::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceActEntry();

    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit(NdisStatus);
}


NDIS_STATUS CDot11RandomMacForScan::StartResetJob()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // Immediately set the new job state
    m_JobState = RandomMacForScanWaitingForResetJob;

    // Launch the child job to create the default port
    ndisStatus = m_ResetJob.InitializeFromJob(
        m_pAdapter,
        m_pPort->GetNdisPortNumber(),
        &m_MacAddress
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Dot11 Reset Port Job (0x%x)", ndisStatus);
        goto Exit;
    }

    // Save the start state so that when complete happens, we know where we are
    m_JobState = RandomMacForScanWaitingForResetJob;

    ndisStatus = StartChildJob(&m_ResetJob);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Dot11 Reset Port Job (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:
    TraceActExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CDot11RandomMacForScan::CompleteResetJob(
    _In_ NDIS_STATUS OperationStatus)
{
    // Job returned the final code
    WFCActInfo("Dot11 Reset Port Job completed (0x%x)", OperationStatus);

    return OperationStatus;
}

NDIS_STATUS CDot11RandomMacForScan::StartPowerParameterRestoreJob()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // Immediately set the new job state
    m_JobState = RandomMacForScanWaitingPowerStateJob;

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

NDIS_STATUS CDot11RandomMacForScan::CompletePowerParameterRestoreJob(
    _In_ NDIS_STATUS OperationStatus)
{
    // Job returned the final code
    WFCActInfo("Dot11 Power Parameter Restore Job completed (0x%x)", OperationStatus);
    return OperationStatus;
}

NDIS_STATUS CDot11RandomMacForScan::StartNLOParameterResetJob()
{
    NDIS_STATUS status = NDIS_STATUS_SOFT_ERRORS; // can this is a better error.

    m_JobState = RandomMacForScanWaitingNLOJob;

    //
    // check to see if we need to re-plumb NLO
    // Update NLO buffer command returns 
    //
    status = m_pPort->UpdateStoredNLOParameterBuffer();
    if (status == NDIS_STATUS_SOFT_ERRORS)
    {
        WFCInfo("No need to replumb NLO");
    }
    else if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Could not update the NLO parameters 0x%x", status);
    }
    else
    {
        status = m_DeviceCommand.Initialize(
            m_PortId,
            WDI_SET_NETWORK_LIST_OFFLOAD,
            m_pPort->m_cbNloCommand,
            m_pPort->m_pbNloCommand);
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActError("Failed to initialize Device Command for SetNlo 0x%x", status);
        }
        else
        {
            // StartTask ->  Use use m_DeviceCommand and QueueDeviceCommand
            status = QueueDeviceCommand(&m_DeviceCommand);
            if (status != NDIS_STATUS_SUCCESS)
            {
                WFCActError("Failed to start SetPower Task 0x%x", status);
            }
        }
    }

    return status;
}

NDIS_STATUS CDot11RandomMacForScan::CompleteNLOParameterResetJob(
    _In_ NDIS_STATUS OperationStatus)
{
    // Job returned the final code
    WFCActInfo("Dot11 NLO Parameter Restore Job completed (0x%x)", OperationStatus);
    return OperationStatus;
}

void CDot11RandomMacForScan::FinishJob(
    _In_ NDIS_STATUS JobCompleteStatus)
{
    CompleteJob(JobCompleteStatus);
}

void CDot11RandomMacForScan::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOL completeJob = FALSE;

    TraceActEntry();

    //
    // get the port object
    //
    m_pPort = m_pAdapter->GetPortFromNdisPortNumber(m_NdisPortNumber);
    ASSERT(m_pPort);

    // Compare the value of the new MAC address to the hardware address and to the configured address for scan.
    // If ((new == hardware MAC && configured != hardwareMAC) || ((new != hardware MAC && configured == hardwareMAC)
    // then need to reset the adapter to the new value
    // else just need to change the property and wait for the next atural reset
    DOT11_MAC_ADDRESS hwMacAddress;
    ndisStatus = m_pAdapter->GetAdapterPropertyCache()->GetPermanentMacAddress(&hwMacAddress);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActInfo("[MacRandom] Couldn't read the Permanent MAC Address");
        goto Exit;
    }

    // if MAC Address in request is set to all zeroes, there is no need to do anything.
    if (m_MacAddress[0] == 0 &&
        m_MacAddress[1] == 0 &&
        m_MacAddress[2] == 0 &&
        m_MacAddress[3] == 0 &&
        m_MacAddress[4] == 0 &&
        m_MacAddress[5] == 0)
    {
        // New and current are the same. This will happen if randomization is turned off, or if the NWIFI driver somehow issues a reset before WDI.
        WFCActInfo("[MacRandom] Setting the random MAC to null is a no op.");
        completeJob = TRUE;
    }
    else if (RtlCompareMemory(m_pPort->GetMacAddress(), m_MacAddress, sizeof(DOT11_MAC_ADDRESS)) == sizeof(DOT11_MAC_ADDRESS))
    {
        // New and current are the same. This will happen if randomization is turned off, or if the NWIFI driver somehow issues a reset before WDI.
        WFCActInfo("[MacRandom] Current address and MAC for Scan Address are the same, no need for Reset");

        m_pPort->SetMacAddressForScan(&m_MacAddress);
        completeJob = TRUE;
    }
    else if (m_pPort->GetDot11State() == WfcPortDot11StateOp)
    {
        // If we are in Op State, we should just copy the address and wait for the next natuural reset to apply it.
        WFCActInfo("[MacRandom] Driver is in OP state, will not send Reset, wait for next one");

        m_pPort->SetMacAddressForScan(&m_MacAddress);
        completeJob = TRUE;
    }
    else
    {
        bool oldIsSameAsCurrent = (RtlCompareMemory(m_pPort->GetMacAddress(), m_pPort->GetMacAddressForScan(), sizeof(DOT11_MAC_ADDRESS)) == sizeof(DOT11_MAC_ADDRESS));
        bool oldIsSameAsHW = (RtlCompareMemory(m_pPort->GetMacAddressForScan(), &hwMacAddress, sizeof(DOT11_MAC_ADDRESS)) == sizeof(DOT11_MAC_ADDRESS));
        bool newIsSameAsHW = (RtlCompareMemory(m_MacAddress, &hwMacAddress, sizeof(DOT11_MAC_ADDRESS)) == sizeof(DOT11_MAC_ADDRESS));

        m_pPort->SetMacAddressForScan(&m_MacAddress);

        if (oldIsSameAsCurrent && 
            ((oldIsSameAsHW && newIsSameAsHW) || (!oldIsSameAsHW && !newIsSameAsHW))
            )
        {
            // Old and new state are matching. No need to execute further tasks. Address for scan will be naturally set in next RESET.
            WFCActInfo("[MacRandom] Current address and MAC for Scan Address are in same category, wait for next Reset");
            completeJob = TRUE;
        }
        else
        {
            // Read the value of the power save setting before the RESET, so it can be restored.
            if (GetPortPropertyCache()->GetPropertyBoolean(
                WfcPortPropertyAutoPsm,
                &m_bOidAutoPowerSave
                ) != NDIS_STATUS_SUCCESS)
            {
                WFCActInfo("Couldn't read AutoPsm in property cache");
            }

            // We assume that the "stored NLO command" parameter will not be affected by the RESET. 
            // If that turns out not to be true, we will need logic here to save the state

            // Start the first job. 
            ndisStatus = StartResetJob();
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCActError("Failed to start reset job (0x%x)", ndisStatus);
                goto Exit;
            }
        }
    }

Exit:
    // If anything failed, we will complete the job right now
    if (ndisStatus != NDIS_STATUS_SUCCESS || completeJob == TRUE)
    {
        FinishJob(ndisStatus);
    }

    TraceActExit(ndisStatus);
}

void CDot11RandomMacForScan::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{

    BOOLEAN completeJob = FALSE;

    TraceActEntry();

    UNREFERENCED_PARAMETER(SenderObject);

    switch (m_JobState)
    {
    case RandomMacForScanWaitingForResetJob:
        {
            OperationStatus = CompleteResetJob(OperationStatus);

            if (OperationStatus == NDIS_STATUS_SUCCESS)
            {
                OperationStatus = StartPowerParameterRestoreJob();
            }
        }
        break;

    case RandomMacForScanWaitingPowerStateJob:
        {
            OperationStatus = CompletePowerParameterRestoreJob(OperationStatus);

            if (OperationStatus == NDIS_STATUS_SUCCESS)
            {
                OperationStatus = StartNLOParameterResetJob();
                if (OperationStatus == NDIS_STATUS_SOFT_ERRORS)
                {
                    // If there is no need to replumb NLO, complete job immediately.
                    OperationStatus = NDIS_STATUS_SUCCESS;
                    completeJob = TRUE;
                }
            }
        }
        break;

    case RandomMacForScanWaitingNLOJob:
        {
            OperationStatus = CompleteNLOParameterResetJob(OperationStatus);

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

void CDot11RandomMacForScan::OnJobCancelled()
{
    // We do nothing yet
}
