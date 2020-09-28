/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

Request.cpp

Abstract:

Implementation for generic request handler

Environment:

Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "ANQPQuery.tmh"


// -----------------------------------------------------------

CANQPQueryJob::CANQPQueryJob(
    _In_ WFC_ACTIVITY_ID ActivityId) :
    COidJobBase(ActivityId),
    m_PortType(WfcPortTypeUnspecified),
    m_DiscoverJob(ActivityId),
    m_bDiscoverJobPending(FALSE),
    m_ScanJob(ActivityId),
    m_bScanJobPending(FALSE),
    m_pSendActionFrameJob(NULL),
    m_bActionFrameTypeIsRequest(TRUE),
    m_pGetTokenDeviceCommand(NULL),
    m_bGetTokenCommandPending(FALSE),
    m_GetTokenBuffer(),
    m_ANQPQueryInfo(NULL),
    m_uComebackDelay(0),
    m_pEndDwellDeviceCommand(NULL),
    m_SetEndDwellBuffer(),
    m_ANQPFragmentsBytesCached(0),
    m_QueryBufferLength(0),
    m_pANQPQueryCompleteParams(NULL),
    m_ResponseBufferLength(0),
    m_ActionFrameReceivedRegistrationToken(WFC_REGISTRATION_TOKEN_INVALID),
    m_OidTimerToken(WFC_TIMER_REGISTRATION_TOKEN_INVALID),
    m_AwaitingANQPResponseTimerToken(WFC_TIMER_REGISTRATION_TOKEN_INVALID),
    m_AwaitingANQPResponseTimerPending(FALSE),
    m_ComebackTimerToken(WFC_TIMER_REGISTRATION_TOKEN_INVALID),
    m_ComebackTimerPending(FALSE),
    m_QueryResult(dot11_ANQP_query_result_failure),
    m_JobStatus(NDIS_STATUS_SUCCESS),
    m_IsCancelled(FALSE)
{
    TraceActEntry();

    InitializeListHead(&m_ANQPFragmentsList);
    m_ScanJobParameters.InitializeDefaults();

    TraceActExit(ActivityId);
}

CANQPQueryJob::~CANQPQueryJob()
{
    LIST_ENTRY              *pEntry;
    ANQP_RESPONSE_FRAGMENT  *pANQPFragment;

    TraceActEntry();

    if (m_pGetTokenDeviceCommand)
    {
        delete m_pGetTokenDeviceCommand;
        m_pGetTokenDeviceCommand = NULL;
    }

    if (m_pEndDwellDeviceCommand)
    {
        delete m_pEndDwellDeviceCommand;
        m_pEndDwellDeviceCommand = NULL;
    }

    while (!IsListEmpty(&m_ANQPFragmentsList))
    {
        pEntry = RemoveHeadList(&m_ANQPFragmentsList);
        pANQPFragment = CONTAINING_RECORD(pEntry, ANQP_RESPONSE_FRAGMENT, Linkage);

        delete[] pANQPFragment;
    }

    NT_ASSERT (NULL == m_pSendActionFrameJob);
    if (m_pSendActionFrameJob)
    {
        delete m_pSendActionFrameJob;
    }

    if (m_pANQPQueryCompleteParams)
    {
        delete[] m_pANQPQueryCompleteParams;
    }

    if (m_ANQPQueryInfo)
    {
        delete[] m_ANQPQueryInfo;
    }

    TraceActExit(0);
}


// -----------------------------------------------------------

VOID CANQPQueryJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus)
{
    TraceActEntry();

    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit(NdisStatus);
}

void
CANQPQueryJob::CleanupAndCompleteJob(
    _In_ NDIS_STATUS JobCompleteStatus)
{
    USHORT BufferLength;
    DOT11_ANQP_QUERY_COMPLETE_PARAMETERS anqpQueryCompleteParams = { 0 };
    DOT11_ANQP_QUERY_COMPLETE_PARAMETERS *pANQPQueryCompleteParams;

    TraceActEntry();

    if (m_ActionFrameReceivedRegistrationToken != WFC_REGISTRATION_TOKEN_INVALID)
    {
        m_pAdapter->GetNotificationManager()->DeregisterNotificationHandler(m_ActionFrameReceivedRegistrationToken);
        m_ActionFrameReceivedRegistrationToken = WFC_REGISTRATION_TOKEN_INVALID;
    }

    if (m_ComebackTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        (VOID)m_pEventQueue->DeregisterTimeoutCallback(
            m_ComebackTimerToken );
        m_ComebackTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    if (m_OidTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        (VOID)m_pEventQueue->DeregisterTimeoutCallback(
            m_OidTimerToken );
        m_OidTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    if (m_AwaitingANQPResponseTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        NT_ASSERT(m_AwaitingANQPResponseTimerPending);
        (VOID)m_pEventQueue->DeregisterTimeoutCallback(
            m_AwaitingANQPResponseTimerToken );
        m_AwaitingANQPResponseTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    if (dot11_ANQP_query_result_success == m_QueryResult)
    {
        pANQPQueryCompleteParams = m_pANQPQueryCompleteParams;
        BufferLength = m_ResponseBufferLength;
    }
    else
    {
        pANQPQueryCompleteParams = &anqpQueryCompleteParams;
        BufferLength = sizeof(anqpQueryCompleteParams);

        pANQPQueryCompleteParams->uResponseLength = 0;
    }

    //
    // Avoid a double completion in the service. Do not send the indication if we had already sent it earlier
    //
    if (m_ANQPQueryInfo->hContext)
    {
        pANQPQueryCompleteParams->Status = m_QueryResult;
        pANQPQueryCompleteParams->hContext = m_ANQPQueryInfo->hContext;

        ASSIGN_NDIS_OBJECT_HEADER(
            pANQPQueryCompleteParams->Header,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_ANQP_QUERY_COMPLETE_PARAMETERS_REVISION_1,
            BufferLength);

        WFCActInfo ("CANQPQueryJob::CleanupAndCompleteJob -- Context=%p, Status=%d, ResponseLength=<%d / %d>\n",
            pANQPQueryCompleteParams->hContext, pANQPQueryCompleteParams->Status,
            pANQPQueryCompleteParams->uResponseLength, BufferLength);

        m_ANQPQueryInfo->hContext = NULL;

        // Send the complete indication to OS
        m_pAdapter->IndicateStatus(
            m_OidPortNumber,
            NDIS_STATUS_DOT11_ANQP_QUERY_COMPLETE,
            m_OidRequestId,
            pANQPQueryCompleteParams,
            BufferLength);
    }
    else
    {
        WFCActError("Context is NULL - Query was completed earlier\n");
        NT_ASSERT(m_ANQPQueryInfo->hContext);
    }

    CompleteJob(JobCompleteStatus);

    TraceActExit(JobCompleteStatus);
}

// -----------------------------------------------------------

NDIS_STATUS CANQPQueryJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    DWORD                   dwRequestLength;
    NDIS_STATUS             ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_ANQP_QUERY_INFO  dot11ANQPQueryInfo = (PDOT11_ANQP_QUERY_INFO) SET_OID_BUFFER(pOidRequest);
    BOOLEAN                 ActionFramesSupported = FALSE;

    UNREFERENCED_PARAMETER(pOidRequest);

    TraceActEntry();

    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;

    ndisStatus = m_pAdapter->GetAdapterPropertyCache()->GetPropertyBoolean(WfcAdapterPropertyActionFramesSupported, &ActionFramesSupported);
    if (NDIS_STATUS_SUCCESS != ndisStatus)
    {
        WFCActError("Unable to query property ActionFramesSupported, %d\n", ndisStatus);
        goto Exit;
    }

    WFCActInfo("Found ActionFramesSupported Setting = %x\n", ActionFramesSupported);
    m_ActionFramesSupported = ActionFramesSupported;

    if (!m_ActionFramesSupported)
    {
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        WFCActError("Action Frames is not supported.  Cannot do ANQP queries");
        goto Exit;
    }

    dwRequestLength = SET_OID_BUFFER_LENGTH(pOidRequest);
    m_QueryBufferLength = dwRequestLength >= FIELD_OFFSET(DOT11_ANQP_QUERY_INFO, ucQueryBuffer) ?
                     dot11ANQPQueryInfo->uQueryLength : 0;
    if (!m_QueryBufferLength)
    {
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        WFCActError("Invalid Buffer length <%d> < <%d + %d>",
            dwRequestLength, FIELD_OFFSET(DOT11_ANQP_QUERY_INFO, ucQueryBuffer), m_QueryBufferLength);
        goto Exit;
    }
    pOidRequest->DATA.SET_INFORMATION.BytesRead = dwRequestLength;

    //
    // Validate that the Context is non-NULL
    //
    if (!dot11ANQPQueryInfo->hContext)
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCActError("Invalid Context value\n");
        goto Exit;
    }

    m_ANQPQueryInfo = (PDOT11_ANQP_QUERY_INFO) new UCHAR[dwRequestLength];
    if (NULL == m_ANQPQueryInfo)
    {
        WFCActError("Failed to allocate memory for ANQP Query");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto Exit;
    }

    RtlCopyMemory(m_ANQPQueryInfo, dot11ANQPQueryInfo, dwRequestLength);

    m_OidQueryTimeoutInMs = min(dot11ANQPQueryInfo->uQueryFailureTimeoutMs, DOT11_ANQP_MAX_QUERY_TIMEOUT_MS);
    m_JobExpireTime = CSystem::get_CurrentTime() + MSECS_TO_CURRENT_TIME_UNITS(m_OidQueryTimeoutInMs);

    RtlCopyMemory(&m_Dot11BSSID, &dot11ANQPQueryInfo->dot11BSSID, sizeof(DOT11_MAC_ADDRESS));
    RtlCopyMemory(&m_ScanJobParameters.WdiScanParameters.BSSID,
                   &dot11ANQPQueryInfo->dot11BSSID,
                   sizeof(DOT11_MAC_ADDRESS));

Exit:
    TraceActExit(ndisStatus);
    return ndisStatus;
}


CBSSEntry* CANQPQueryJob::FindCachedBSSEntry(
    )
{
    CBSSEntry               *pMatchedBssEntry;
    ULONGLONG               OldestValidTimeOfEntry, LastUpdateTime;
    ULONGLONG               maxAgeOfEntries100nS = WFC_CONSTANT::ConnectBssEntryExpiryTime;

    pMatchedBssEntry = m_pAdapter->GetBSSListManager(m_PortType)->FindBSSEntry(m_ANQPQueryInfo->dot11BSSID);
    if (pMatchedBssEntry)
    {
        //
        // Check the age of the entry
        //
        OldestValidTimeOfEntry = CSystem::get_CurrentTime();
        if (OldestValidTimeOfEntry >= maxAgeOfEntries100nS)
        {
            OldestValidTimeOfEntry -= maxAgeOfEntries100nS;
        }

        LastUpdateTime = pMatchedBssEntry->GetUpdateTime();
        if (LastUpdateTime >= OldestValidTimeOfEntry)
        {
            WFCActInfo("Found Cached entry for BSSID = %!MACADDR!\n", m_ANQPQueryInfo->dot11BSSID);
        }
        else
        {
            WFCActInfo("Cached entry for BSSID = %!MACADDR! is too old - ignoring\n", m_ANQPQueryInfo->dot11BSSID);
            pMatchedBssEntry = NULL;
        }
    }

    return pMatchedBssEntry;
}


void CANQPQueryJob::OnJobStarted()
{
    NDIS_STATUS             status = NDIS_STATUS_SUCCESS;
    DOT11_SSID              broadcastSSID;
    CBSSEntry               *pMatchedBssEntry = NULL;

    TraceActEntry();

    //
    // Tell NDIS to complete the OID if necessary
    //
    NotifyStarted(NDIS_STATUS_SUCCESS);

    status = m_pAdapter->GetPortTypeFromPortId(get_PortId(), &m_PortType);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("FAILed to get Port type from NdisPortNumber=%d, PortId=%d failed <0x%08x>",
            m_NdisPortNumber, get_PortId(), status);
        goto Exit;
    }

    WFCInfo("CANQPQueryJob::OnJobStarted: Got Port type = %d from NdisPortNumber=%d, PortId=%d",
        m_PortType, m_NdisPortNumber, get_PortId());

    WFCActInfo("registering callback timer.");

    status = m_pEventQueue->RegisterTimeoutCallbackWithLevelAndReuse(
        get_ActivityId(),
        this,
        m_OidQueryTimeoutInMs,
        &m_OidTimerToken,                               // WFC_TIMER_CONTEXT
        WDF_EXECUTION_LEVEL::WdfExecutionLevelPassive,
        false,
        NULL,
        &m_OidTimerToken );

    if (NDIS_STATUS_SUCCESS != status)
    {
        WFCActErrorStatus("Couldn't register for ANQP request complete callback.");
        goto Exit;
    }

    pMatchedBssEntry = FindCachedBSSEntry();
    if (pMatchedBssEntry)
    {
        status = StartANQPQueryTask(pMatchedBssEntry);
        goto Exit;
    }

    if (WfcPortTypeExtSTA == m_PortType)
    {
        // Use broadcast SSID for scan
        RtlZeroMemory(&broadcastSSID, sizeof(DOT11_SSID));
        broadcastSSID.uSSIDLength = 0;
        m_ScanJobParameters.pSSIDList = &broadcastSSID;
        m_ScanJobParameters.SSIDListCount = 1;
        m_ScanJobParameters.WdiScanParameters.ScanModeParameters.ScanTrigger = WDI_SCAN_TRIGGER_ANQP_QUERY;

        status = m_ScanJob.InitializeFromScanParams(m_pAdapter, m_NdisPortNumber, &m_ScanJobParameters);
        if (NDIS_STATUS_SUCCESS != status)
        {
            WFCActErrorStatus("Couldn't initialize scan job from scan parameters.");
            goto Exit;
        }

        m_bScanJobPending = TRUE;
        status = StartChildJob(&m_ScanJob);
        if (NDIS_STATUS_SUCCESS != status)
        {
            WFCActErrorStatus("Couldn't start scan job.");
            m_bScanJobPending = FALSE;
            goto Exit;
        }
    }
    else
    {
        DOT11_WFD_GROUP_ID targetGroupId = { {0}, { 7, "DIRECT-" } };

        status = m_DiscoverJob.InitializeDiscoverByGroupID(
            m_pAdapter,
            m_NdisPortNumber,
            targetGroupId,
            false, /* RequireSelectedRegistrar */
            false, /* RequireGroupFormationBit */
            false, /* AllowBeaconMatch */
            false, /* P2PRoamingReconnect */
            NULL,
            NULL );
        if (NDIS_STATUS_SUCCESS != status)
        {
            WFCActErrorStatus("Couldn't initialize on-demand discover.");
            goto Exit;
        }

        m_bDiscoverJobPending = TRUE;
        status = StartChildJob(&m_DiscoverJob);
        if (NDIS_STATUS_SUCCESS != status)
        {
            WFCActErrorStatus("Couldn't start discover job.");
            m_bDiscoverJobPending = FALSE;
            goto Exit;
        }
    }

    m_State = WiFiANQPQueryJobStateWaitingForScanJobCompletion;

Exit:

    // Keep OACR happy
    _Analysis_assume_( m_ScanJobParameters.pSSIDList->uSSIDLength <= 32 );
    // Destructor would free the m_ScanParameters structure

    if (NDIS_STATUS_SUCCESS != status)
    {
        CleanupAndCompleteJob(status);
    }

    TraceActExit(status);
}


VOID CANQPQueryJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceActEntry();

    TraceActExit(NdisStatus);
}


void CANQPQueryJob::OnJobCancelled()
{
    TraceActEntry();

    if (!m_IsCancelled)
    {
        m_IsCancelled = true;

        // Cancel the Scan Job
        if (m_bScanJobPending)
        {
            m_pAdapter->GetSerializedJobList()->CancelJob(
                m_NdisPortNumber,
                WiFiJobScan,
                &m_ScanJob
                );
        }

        // Cancel the Discover Job
        if (m_bDiscoverJobPending)
        {
            m_pAdapter->GetSerializedJobList()->CancelJob(
                m_NdisPortNumber,
                WiFiJobP2PDiscover,
                &m_DiscoverJob
                );
        }

        // Cancel the Send Action Frame job if active
        if (m_pSendActionFrameJob)
        {
            // Cancel the serialized ActionFrame job
            m_pAdapter->GetSerializedJobList()->CancelJob(
                m_NdisPortNumber,
                WiFiJobSendActionFrameRequest,
                m_pSendActionFrameJob
                );
        }
    }

    TraceActExit(0);
}


void CANQPQueryJob::OnTimerCallback(
    _In_opt_ WFC_TIMER_CONTEXT pvContext
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if (pvContext == &m_OidTimerToken)
    {
        WFCActInfo("ANQPQuery Timer Expiry: Job time limit expired. Completing the job");

        m_QueryResult = dot11_ANQP_query_result_timed_out;
        m_JobStatus = NDIS_ERROR_CODE_TIMEOUT;

        //
        // If we are currently waiting on some tasks or commands to complete, cancel and wait for completion
        //
        if (CheckAndMarkPendingJobs (m_JobStatus, NULL))
        {
            OnJobCancelled();
        }
        else
        {
            CleanupAndCompleteJob(m_JobStatus);
        }
    }
    else if (pvContext == &m_AwaitingANQPResponseTimerToken)
    {
        WFCActInfo("ANQPQuery Timer Expiry: AwaitingANQPResponse time limit expired Pending=<%x>. Completing the job",
            m_AwaitingANQPResponseTimerPending);
        NT_ASSERT(m_AwaitingANQPResponseTimerPending);

        m_QueryResult = dot11_ANQP_query_result_timed_out;
        m_JobStatus = NDIS_ERROR_CODE_TIMEOUT;

        //
        // If we are currently waiting on some tasks or commands to complete, cancel and wait for completion
        //
        if (CheckAndMarkPendingJobs (m_JobStatus, NULL))
        {
            OnJobCancelled();
        }
        else
        {
            CleanupAndCompleteJob(m_JobStatus);
        }
    }
    else if (pvContext == &m_ComebackTimerToken)
    {
        WFCActInfo("ANQPQuery Timer Expiry: Comeback timer fired, Pending=<%x>", m_ComebackTimerPending);
        NT_ASSERT(m_ComebackTimerPending);
        if (m_ComebackTimerPending)
        {
            m_ComebackTimerPending = FALSE;
            OnJobStepCompleted (status, this);
        }
    }
    else
    {
        NT_ASSERT(0);  // Unknown time context
    }

    TraceActExit(0);
}


void CANQPQueryJob::OnDeviceIndicationArrived(
    _In_ WDI_INDICATION_TYPE                 IndicationType,
    _In_ WFC_COMMAND_TOKEN                   CommandToken,
    _In_ WFC_PORT_ID                         PortId,
    _In_ ULONG                               EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer)
{
    UNREFERENCED_PARAMETER(CommandToken);
    UNREFERENCED_PARAMETER(PortId);

    TraceActEntry();
    switch (IndicationType)
    {
        case WiFiIndicationActionFrameReceived:
        {
            OnActionFrameReceived(EventBufferLength, EventBuffer);
            break;
        }

        default:
        {
            WFCActError("Ignoring unknown IndicationType <%d>\n", IndicationType);
            break;
        }
    }

    TraceActExit(IndicationType);
}

// -----------------------------------------------------------
// ANQP via Action frames
// -----------------------------------------------------------

DOT11_ANQP_QUERY_RESULT
CANQPQueryJob::MapStatusCodeToQueryResult(
    _In_    USHORT  uStatusCode
    )
{
    DOT11_ANQP_QUERY_RESULT     QueryResult = dot11_ANQP_query_result_failure;

    switch (uStatusCode)
    {
        case (0):
        {
            QueryResult = dot11_ANQP_query_result_success;
            break;
        }

        case (59):                      // GAS_ADVERTISEMENT_PROTOCOL_NOT_SUPPORTED
        {
            QueryResult = dot11_ANQP_query_result_advertisement_protocol_not_supported_on_remote;
            break;
        }

        case (60):                      // NO_OUTSTANDING_GAS_REQUEST
        case (63):                      // GAS_QUERY_RESPONSE_TOO_LARGE
        case (79):                      // TRANSMISSION_FAILURE
        {
            QueryResult = dot11_ANQP_query_result_gas_protocol_failure;
            break;
        }

        case (61):                      // GAS_RESPONSE_NOT_RECEIVED_FROM_SERVER
        case (62):                      // GAS_QUERY_TIMEOUT
        case (65):                      // SERVER_UNREACHABLE
        {
            QueryResult = dot11_ANQP_query_result_advertisement_server_not_responding;
            break;
        }

        case (64):                      // REJECTED_HOME_WITH_SUGGESTED_CHANGES
        case (67):                      // REJECTED_FOR_SSP_PERMISSIONS
        case (68):                      // AP does not support unauthenticated access
        {
            QueryResult = dot11_ANQP_query_result_access_issues;
            break;
        }

        default:
        {
            QueryResult = dot11_ANQP_query_result_failure;
            break;
        }
    }

    return (QueryResult);
}


NDIS_STATUS CANQPQueryJob::CacheANQPFragment(
    _In_    UCHAR                           uFragmentID,
    _In_    USHORT                          uFragmentSize,
    _In_reads_bytes_(uFragmentSize) UCHAR   *pucFragment
    )
{
    NDIS_STATUS             status = NDIS_STATUS_SUCCESS;
    USHORT                  uBytes = 0, uANQPFragmentsBytesCached = 0;
    LIST_ENTRY              *pEntry;
    ANQP_RESPONSE_FRAGMENT  *pANQPFragment, *pPreviousANQPFragment;

    TraceActEntry();

    if (!uFragmentSize)
    {
        WFCActInfo("Ignoring Data -- 0 size associated with FragmentID <%d>\n", uFragmentID);
        goto Exit;
    }

    status = RtlUShortAdd( sizeof(ANQP_RESPONSE_FRAGMENT), uFragmentSize, &uBytes );
    if (NDIS_STATUS_SUCCESS != status)
    {
        WFCActErrorStatus("Addition overflow in appending FragmentSize to struct Header -- %u + %u\n",
            sizeof(ANQP_RESPONSE_FRAGMENT), uFragmentSize);
        goto Exit;
    }

    status = RtlUShortAdd( m_ANQPFragmentsBytesCached, uFragmentSize, &uANQPFragmentsBytesCached );
    if (NDIS_STATUS_SUCCESS != status)
    {
        WFCActErrorStatus("Addition overflow in appending FragmentSize to ANQPFragmentsBytesCached -- %u + %u\n",
            m_ANQPFragmentsBytesCached, uFragmentSize);
        goto Exit;
    }

    //
    // Allocate and Initialize the fragment entry
    //
    pANQPFragment = (PANQP_RESPONSE_FRAGMENT) new UCHAR[uBytes];
    if (!pANQPFragment)
    {
        status = NDIS_STATUS_RESOURCES;
        WFCActErrorStatus("FAILed to allocate %d bytes for ANQP fragment", uBytes);
        m_QueryResult = dot11_ANQP_query_result_resources;
        goto Exit;
    }

    InitializeListHead(&pANQPFragment->Linkage);
    pANQPFragment->uFragmentID = uFragmentID;
    pANQPFragment->uFragmentSize = uFragmentSize;
    RtlCopyMemory(&pANQPFragment->ucFragment, pucFragment, uFragmentSize);

    //
    // Find an entry to insert in front of.
    // Search backwards from the end of the list since the fragments are expected to be in order
    //
    pPreviousANQPFragment = NULL;
    for (pEntry = m_ANQPFragmentsList.Blink; pEntry != &m_ANQPFragmentsList; )
    {
        pPreviousANQPFragment = CONTAINING_RECORD(pEntry, ANQP_RESPONSE_FRAGMENT, Linkage);
        pEntry = pEntry->Blink;

        //
        // If the new fragment should be before this one, skip over it
        //
        if (pPreviousANQPFragment->uFragmentID > uFragmentID)
        {
            pPreviousANQPFragment = NULL;   // Set to NULL, so that we will Insert at the head below if the list ends
            continue;
        }

        //
        // If this is a duplicate fragment, drop it - we will keep the first one received
        //
        if (pPreviousANQPFragment->uFragmentID == uFragmentID)
        {
            WFCActErrorStatus("DROPPING Duplicate Fragment, ID=%d\n", uFragmentID);
            delete[] pANQPFragment;
            pANQPFragment = NULL;
            break;
        }

        pANQPFragment->Linkage.Flink = pPreviousANQPFragment->Linkage.Flink;
        pANQPFragment->Linkage.Blink = &pPreviousANQPFragment->Linkage;
        pPreviousANQPFragment->Linkage.Flink->Blink = &pANQPFragment->Linkage;
        pPreviousANQPFragment->Linkage.Flink = &pANQPFragment->Linkage;
        
        m_ANQPFragmentsBytesCached = uANQPFragmentsBytesCached;
        break;
    }

    if (!pPreviousANQPFragment)
    {
        InsertHeadList(&m_ANQPFragmentsList, &pANQPFragment->Linkage);
        m_ANQPFragmentsBytesCached = uANQPFragmentsBytesCached;
    }

    WFCActInfo("CANQPQueryJob::CacheANQPFragment: Cached FragmentID=<%d>, Length=<%d>\n", uFragmentID, uFragmentSize);

Exit:
    TraceActExit(status);
    return (status);
}


NDIS_STATUS CANQPQueryJob::HandleFinalANQPResponseReceived(
    )
{
    NDIS_STATUS             status = NDIS_STATUS_SUCCESS;
    ULONG                   uBytesCopied;
    UCHAR                   uNextFragmentID;
    LIST_ENTRY              *pEntry;
    ANQP_RESPONSE_FRAGMENT  *pANQPFragment;
    PUCHAR                  pDestBuffer;

    TraceActEntry();

    status = RtlUShortAdd(
        sizeof(DOT11_ANQP_QUERY_COMPLETE_PARAMETERS),
        m_ANQPFragmentsBytesCached,
        &m_ResponseBufferLength);
    if (NDIS_STATUS_SUCCESS != status)
    {
        WFCActErrorStatus("Addition overflow in appending ANQPFragmentBytesCached -- %u + %u\n",
            sizeof(DOT11_ANQP_QUERY_COMPLETE_PARAMETERS), m_ANQPFragmentsBytesCached);
        goto Exit;
    }

    m_pANQPQueryCompleteParams = (PDOT11_ANQP_QUERY_COMPLETE_PARAMETERS) new UCHAR[m_ResponseBufferLength];
    if (!m_pANQPQueryCompleteParams)
    {
        WFCActErrorStatus("FAILed to allocate %d bytes for ANQP response", m_ResponseBufferLength);
        m_ResponseBufferLength = 0;
        m_QueryResult = dot11_ANQP_query_result_resources;
        status = NDIS_STATUS_RESOURCES;
        goto Exit;
    }

    uNextFragmentID = 0;
    uBytesCopied = 0;
    pDestBuffer = ((PUCHAR) m_pANQPQueryCompleteParams) + sizeof(DOT11_ANQP_QUERY_COMPLETE_PARAMETERS);
    for (pEntry = m_ANQPFragmentsList.Flink; pEntry != &m_ANQPFragmentsList; pEntry = pEntry->Flink)
    {
        pANQPFragment = CONTAINING_RECORD(pEntry, ANQP_RESPONSE_FRAGMENT, Linkage);

        if ((pANQPFragment->uFragmentID > uNextFragmentID) ||
            ((pANQPFragment->uFragmentSize + uBytesCopied) > m_ANQPFragmentsBytesCached))
        {
            WFCActErrorStatus("ERROR - All fragments not received, or bad byte count ID=<%d vs %d>, Bytes=<%d > %d>\n",
                pANQPFragment->uFragmentID, uNextFragmentID,
                (pANQPFragment->uFragmentSize + uBytesCopied), m_ANQPFragmentsBytesCached);

            delete [] m_pANQPQueryCompleteParams;
            m_pANQPQueryCompleteParams = NULL;

            m_QueryResult = dot11_ANQP_query_result_gas_protocol_failure;
            
            status = NDIS_STATUS_RESOURCES;
            goto Exit;
        }

        RtlCopyMemory(&pDestBuffer[uBytesCopied], pANQPFragment->ucFragment, pANQPFragment->uFragmentSize);

        uNextFragmentID++;
        uBytesCopied += pANQPFragment->uFragmentSize;
    }

    m_pANQPQueryCompleteParams->uResponseLength = uBytesCopied;

    WFCActInfo("CANQPQueryJob::HandleFinalANQPResponseReceived: TOTAL %d bytes received\n", uBytesCopied);

    m_QueryResult = dot11_ANQP_query_result_success;

Exit:
    TraceActExit(status);
    return (status);
}


void CANQPQueryJob::OnActionFrameReceived(
    _In_ ULONG  EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_INDICATION_ACTION_FRAME_RECEIVED_PARAMETERS  actionFrameReceived;
    PDOT11_ANQP_ACTION_FRAME pANQPFrame;
    UCHAR uFragmentID = 0, uAdvertisementProtocolID = 0, uAdvertisementElementID = 0;
    USHORT uResponseLength = 0, uStatusCode = 0, usComebackDelay = 0, minActionFrameLength;
    ULONG  ulComebackDelayMS, ulJobTimeRemainingMS;
    UCHAR *pucQueryResponseBuffer = NULL;
    BOOLEAN bSendComebackRequest = FALSE;
    DOT11_ANQP_QUERY_RESULT QueryResult;
    ULONGLONG   ullCurrentTime;

    TraceActEntry();

    if (WiFiANQPQueryJobStateWaitingForGasResponse != m_State)
    {
        WFCActErrorStatus("Dropping Frame since we are not expecting a GAS response - state = %d", m_State);
        goto DropFrame;
    }

    status = ParseWdiIndicationActionFrameReceived(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &actionFrameReceived );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Dropping Frame: LE Error - Couldn't parse TLVs for received action frame.");
        goto Exit;
    }

    if ((sizeof(WDI_MAC_ADDRESS)) != RtlCompareMemory(&actionFrameReceived.SourceAddress,
                                                       &m_Dot11BSSID,
                                                       sizeof(WDI_MAC_ADDRESS)))
    {
        WFCActErrorStatus("Dropping Frame - Src Mac address of Action frame does not match - Expecting=%!MACADDR!, Got=%!MACADDR!\n",
            m_Dot11BSSID, actionFrameReceived.SourceAddress.Address);

        goto DropFrame;
    }

    minActionFrameLength = FIELD_OFFSET(DOT11_ANQP_ACTION_FRAME, InitialResponse.InfoElements);
    if (actionFrameReceived.ActionFrameBody.ElementCount < minActionFrameLength)
    {
        WFCActErrorStatus("Dropping Frame: Bad length for received action frame %d < %d",
            actionFrameReceived.ActionFrameBody.ElementCount, minActionFrameLength);
        goto DropFrame;
    }

    pANQPFrame = (PDOT11_ANQP_ACTION_FRAME) actionFrameReceived.ActionFrameBody.pElements;
    switch (pANQPFrame->Action)
    {
        case (DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_INITIAL_RESPONSE):
        {
            uStatusCode = Dot11ToHostShort (pANQPFrame->InitialResponse.StatusCode);
            usComebackDelay = Dot11ToHostShort (pANQPFrame->InitialResponse.ComebackDelay);
            //
            // If the ComebackDelay is 0, then a QueryResponse must have been provided
            //
            if (usComebackDelay)
            {
                bSendComebackRequest = TRUE;
            }
            uResponseLength = Dot11ToHostShort (pANQPFrame->InitialResponse.QueryResponseLength);
            uAdvertisementElementID = pANQPFrame->InitialResponse.AdvertisementProtocolElement.ElementID;
            uAdvertisementProtocolID = pANQPFrame->InitialResponse.AdvertisementProtocolElement.AdvertisementProtocol[0].AdvertisementProtocolID;
            pucQueryResponseBuffer = (((PUCHAR) pANQPFrame) +
                                      FIELD_OFFSET(DOT11_ANQP_ACTION_FRAME, InitialResponse.InfoElements));
            break;
        }

        case (DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_COMEBACK_RESPONSE):
        {
            minActionFrameLength = FIELD_OFFSET(DOT11_ANQP_ACTION_FRAME, ComebackResponse.InfoElements);
            if (actionFrameReceived.ActionFrameBody.ElementCount < minActionFrameLength)
            {
                WFCActError("Dropping Frame - Bad TLV length for received ComebackResponse frame %d <= %d",
                    EventBufferLength, minActionFrameLength);
                goto Exit;
            }

            uStatusCode = Dot11ToHostShort (pANQPFrame->ComebackResponse.StatusCode);
            usComebackDelay = Dot11ToHostShort (pANQPFrame->ComebackResponse.ComebackDelay);
            uResponseLength = Dot11ToHostShort (pANQPFrame->ComebackResponse.QueryResponseLength);
            uFragmentID = pANQPFrame->ComebackResponse.QueryResponseFragmentID.FragmentID;
            //
            // Send a ComebackRequest if the More-Fragments bit is set.
            // Also send a Comeback request if AP is still waiting for the BackEnd, i.e. FragmentID is 0
            //
            if ((pANQPFrame->ComebackResponse.QueryResponseFragmentID.MoreFragments) ||
                (usComebackDelay))
            {
                bSendComebackRequest = TRUE;
            }
            uAdvertisementElementID = pANQPFrame->ComebackResponse.AdvertisementProtocolElement.ElementID;
            uAdvertisementProtocolID = pANQPFrame->ComebackResponse.AdvertisementProtocolElement.AdvertisementProtocol[0].AdvertisementProtocolID;
            pucQueryResponseBuffer = (((PUCHAR) pANQPFrame) +
                                      FIELD_OFFSET(DOT11_ANQP_ACTION_FRAME, ComebackResponse.InfoElements));
            break;
        }

        default:
        {
            WFCActErrorStatus("Dropping Frame - Unknown Action frame type <%d>\n", pANQPFrame->Action);
            goto DropFrame;
        }
    }

    //
    // Verify that the Action frame data is good
    //
    if ((WDI_ACTION_FRAME_CATEGORY_PUBLIC != pANQPFrame->Category) ||
        (pANQPFrame->DialogToken != m_DialogToken) ||
        (DOT11_INFO_ELEMENT_ID_ADVERTISEMENT_PROTOCOL != uAdvertisementElementID) ||
        (0 != uAdvertisementProtocolID))                // ANQP protocol ID
    {
        //
        // We will just drop this packet for now in case this was a DoS attack
        //
        WFCActErrorStatus("Dropping Frame: Action=<%d>, In:Expected Category=<%d:%d>, Token=<%d:%d>, AdvProtocol=<%d:%d>, ProtID=<%d:%d>",
            pANQPFrame->Action, pANQPFrame->Category, WDI_ACTION_FRAME_CATEGORY_PUBLIC,
            pANQPFrame->DialogToken, m_DialogToken,
            uAdvertisementElementID, DOT11_INFO_ELEMENT_ID_ADVERTISEMENT_PROTOCOL,
            uAdvertisementProtocolID, 0);
        goto DropFrame;
    }

    //
    // If there is no need to send a Comeback request, then verify that the frame contains some data
    //
    if (((!bSendComebackRequest) && (0 == uResponseLength)) ||
        (actionFrameReceived.ActionFrameBody.ElementCount < (UINT32) (minActionFrameLength+uResponseLength)))
    {
        //
        // We will just drop this packet for now in case this was a DoS attack
        //
        m_QueryResult = dot11_ANQP_query_result_gas_protocol_failure;
        WFCActError("[Action=%d] Bad ANQP frame - SendComeback=<%x>, length=%d < %d + %d ?",
            pANQPFrame->Action, bSendComebackRequest,
            actionFrameReceived.ActionFrameBody.ElementCount, minActionFrameLength, uResponseLength);
        goto DropFrame;
    }

    //
    // Stop the Awaiting response timer if it is still running
    //
    if (m_AwaitingANQPResponseTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        NT_ASSERT(m_AwaitingANQPResponseTimerPending);
        m_AwaitingANQPResponseTimerPending = FALSE;

        (VOID)m_pEventQueue->DeregisterTimeoutCallback(
            m_AwaitingANQPResponseTimerToken );
        m_AwaitingANQPResponseTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    QueryResult = MapStatusCodeToQueryResult(uStatusCode);
    if (dot11_ANQP_query_result_success != QueryResult)
    {
        m_QueryResult = QueryResult;
        WFCActErrorStatus("ERROR: Remote Device returned failure -- Status=<%d> -> <%d>\n", uStatusCode, QueryResult);
        m_State= WiFiANQPQueryJobStateWaitingForEndDwellTimeToCompleteJob;
        goto Exit;
    }

    //
    // The frame is good.  Now, act on it, and set the next state
    //
    status = CacheANQPFragment(uFragmentID, uResponseLength, pucQueryResponseBuffer);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't cache received action frame.");
        goto Exit;
    }

    //
    // If a comeback delay has been specified, start the timer, and send the EndDwell command
    //
    if (usComebackDelay > 1)
    {
        // Convert from TU to milliseconds (1 TU = 1024 microseconds)
        ulComebackDelayMS = (((ULONG) usComebackDelay * 1024)/1000);

        //
        // Check if the Comeback delay exceeds the JobExpireTime
        //
        ullCurrentTime = CSystem::get_CurrentTime();
        ulJobTimeRemainingMS = ullCurrentTime > m_JobExpireTime ?
            0 : CURRENT_TIME_UNITS_TO_MSECS (m_JobExpireTime - ullCurrentTime); 
        if (ulJobTimeRemainingMS < ulComebackDelayMS)
        {
            //
            // The ComebackDelay will exceed our Job timeout, so we will make a best effort
            // to send the Comeback request with the minimum time before the Job expires
            //
            if (ulJobTimeRemainingMS < WFC_CONSTANT::DefaultANQPResponseFromAPTimeoutMs)
            {
                //
                // Not enough time to send the Comeback request, so no need to wait any more
                // Complete the job with failure
                //
                WFCActError("Not enough time to schedule Comeback - %d->%d msecs, JobTimeLeft=%d MSecs",
                    usComebackDelay, ulComebackDelayMS, ulJobTimeRemainingMS);
                m_QueryResult = dot11_ANQP_query_result_timed_out;
                m_JobStatus = NDIS_ERROR_CODE_TIMEOUT;
                status = NDIS_STATUS_FAILURE;

                goto Exit;
            }

            WFCActInfo("Modifying Comeback time from %d->%d since JobTimeLeft=%d MSecs",
                ulComebackDelayMS, (ulJobTimeRemainingMS-WFC_CONSTANT::DefaultANQPResponseFromAPTimeoutMs), ulJobTimeRemainingMS);
            ulComebackDelayMS = ulJobTimeRemainingMS - WFC_CONSTANT::DefaultANQPResponseFromAPTimeoutMs;
        }

        m_State = WiFiANQPQueryJobStateWaitingForEndDwellTimeToSendComebackRequest;
        NT_ASSERT((0 == uResponseLength) && (0 == uFragmentID));

        //
        // Next, set the next state so that random threads do not cause havoc
        //
        m_ComebackTimerPending = TRUE;
        m_uComebackDelay = ulComebackDelayMS;
        status = m_pEventQueue->RegisterTimeoutCallbackWithLevelAndReuse(
            get_ActivityId(),
            this,
            ulComebackDelayMS,
            &m_ComebackTimerToken,                  // WFC_TIMER_CONTEXT
            WDF_EXECUTION_LEVEL::WdfExecutionLevelPassive,
            false,
            NULL,
            &m_ComebackTimerToken );
        if (NDIS_STATUS_SUCCESS != status)
        {
            m_ComebackTimerPending = FALSE;
            WFCActErrorStatus("ERROR - Couldn't register for Comeback Delay timer.");
            m_QueryResult = dot11_ANQP_query_result_resources;

            goto Exit;
        }

        WFCActInfo("Started Comeback timer <%p> for %d->%d msecs, send EndDwell command ...",
            &m_ComebackTimerToken, usComebackDelay, ulComebackDelayMS);
    }
    else if (bSendComebackRequest)
    {
        //
        // Set the state and call the StepCompleted Handler to send the next request if there is no completion pending
        //
        m_State = WiFiANQPQueryJobStateWaitingToSendComebackRequest;
        WFCActInfo("SendComeback response ...");
    }
    else
    {
        m_State= WiFiANQPQueryJobStateWaitingForEndDwellTimeToCompleteJob;
        WFCActInfo("Handle Final Response frame ...");
        HandleFinalANQPResponseReceived();
    }

Exit:
    //
    // Call the State machine handler to transition to the next state
    //
    OnJobStepCompleted (status, this);

DropFrame:
    TraceActExit(status);
}

NDIS_STATUS CANQPQueryJob::BuildAndSendANQPRequestActionFrame(
    _In_    DOT11_ACTION_FRAME_PUBLIC_ACTION_TYPE   ActionType,
    _In_    USHORT                                  QueryRequestLength,
    _In_bytecount_(QueryRequestLength) PUCHAR       ucQueryBuffer,
    _In_    BOOLEAN                                 bRequestTask
    )
{
    NDIS_STATUS                     status = NDIS_STATUS_SUCCESS;
    ANQP_ACTION_FRAME_PARAMETERS    ANQPParams = {0};       // All fields in params are 0 for ANQP request frame

    TraceActEntry();

    //
    // Send the frame
    //
    NT_ASSERT (NULL == m_pSendActionFrameJob);

    if (m_pSendActionFrameJob)
    {
        delete m_pSendActionFrameJob;
    }

    m_pSendActionFrameJob = new CSendActionFrameRequestJob(get_ActivityId());
    if (NULL == m_pSendActionFrameJob)
    {
        WFCActError("Failed to allocate send action frame job\n");
        status = NDIS_STATUS_RESOURCES;
        goto Exit;
    }
    m_bActionFrameTypeIsRequest = bRequestTask;

    status = m_pSendActionFrameJob->InitializePublicActionFrame(
        m_pAdapter,
        m_OidPortNumber,
        ActionType,
        m_DialogToken,
        (WDI_MAC_ADDRESS *) &m_Dot11BSSID,
        m_BandId,
        m_ChannelNumber,
        QueryRequestLength,
        ucQueryBuffer,
        &ANQPParams,
        m_bActionFrameTypeIsRequest);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("InitializePublicActionFrame failed %d", status);
        goto Exit;
    }

    // Queue the serialized SendActionFrame job
    status = StartChildJob(m_pSendActionFrameJob);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("StartChildJob for SendActionFrame Job failed %d", status);
        goto Exit;
    }

Exit:
    TraceActExit(status);
    return (status);
}


NDIS_STATUS CANQPQueryJob::StartANQPQueryTask(
    _In_    CBSSEntry               *pMatchedBssEntry
    )
{
    NDIS_STATUS         status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    m_BandId = pMatchedBssEntry->GetChannelInfo()->BandId;
    m_ChannelNumber = pMatchedBssEntry->GetChannelInfo()->Channel;

    if (m_ActionFrameReceivedRegistrationToken == WFC_REGISTRATION_TOKEN_INVALID)
    {
        status = m_pAdapter->GetNotificationManager()->RegisterForNotifications(
            WiFiIndicationActionFrameReceived,
            WFC_COMMAND_TOKEN_INVALID,
            get_PortId(),
            (INotifyDeviceIndicationCallback*)this,
            &m_ActionFrameReceivedRegistrationToken);

        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Couldn't register for Action Frame notifications.");
            goto Exit;
        }
    }

    //
    // First, get the Dialog Token
    //
    m_pGetTokenDeviceCommand = new DeviceCommand( get_ActivityId() );
    if ( NULL == m_pGetTokenDeviceCommand )
    {
        status = NDIS_STATUS_RESOURCES;
        WFCActError("Failed to Allocate GetDialogToken Device Command 0x%x", status);
        goto Exit;
    }

    status = m_pGetTokenDeviceCommand->Initialize(
        get_PortId(),
        WDI_GET_NEXT_ACTION_FRAME_DIALOG_TOKEN,
        sizeof(m_GetTokenBuffer),
        (BYTE*)&m_GetTokenBuffer);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize GetDialogToken Device Command 0x%x", status);
        goto Exit;
    }

    m_State = WiFiANQPQueryJobStateWaitingForDialogToken;
    m_bGetTokenCommandPending = TRUE;
    status = QueueDeviceCommand(m_pGetTokenDeviceCommand);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to queue GetDialogToken (0x%x)", status);
        m_bGetTokenCommandPending = FALSE;
        goto Exit;
    }

Exit:
    TraceActExit(status);
    return (status);
}


// -----------------------------------------------------------
// ANQP Query states
// -----------------------------------------------------------


BOOLEAN CANQPQueryJob::CheckAndMarkPendingJobs(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    //
    // SenderObject would be NULL if we just wanted to check if any Jobs/Tasks/Commands are pending
    //
    if (!SenderObject)
    {
    }
    else if (SenderObject == &m_DiscoverJob)
    {
        WFCActInfo("Discover Job=<%p> Completed ... Status = 0x%08x",
            &m_DiscoverJob, OperationStatus);

        m_bDiscoverJobPending = FALSE;
    }
    else if (SenderObject == &m_ScanJob)
    {
        WFCActInfo("Scan Job=<%p> Completed ... Status = 0x%08x",
            &m_ScanJob, OperationStatus);

        m_bScanJobPending = FALSE;
    }
    else if (SenderObject == m_pGetTokenDeviceCommand)
    {
        WFCActInfo("GetToken DeviceCommand=<%p> Completed ... Status = 0x%08x",
            m_pGetTokenDeviceCommand, OperationStatus);

        m_bGetTokenCommandPending = FALSE;
    }
    else if (SenderObject == m_pSendActionFrameJob)
    {
        WFCActInfo("SendActionFrame Job=<%p> Completed ... Status = 0x%08x",
            m_pSendActionFrameJob, OperationStatus);

        delete m_pSendActionFrameJob;
        m_pSendActionFrameJob = NULL;
    }
    else if (SenderObject == m_pEndDwellDeviceCommand)
    {
        WFCActInfo("EndDwell DeviceCommand=<%p> Completed ... Ignore the Status = 0x%08x",
            m_pEndDwellDeviceCommand, OperationStatus);
        OperationStatus = NDIS_STATUS_SUCCESS;

        delete m_pEndDwellDeviceCommand;
        m_pEndDwellDeviceCommand = NULL;
    }

    if ((m_bDiscoverJobPending) ||
        (m_bScanJobPending) ||
        (m_bGetTokenCommandPending) ||
        (m_pSendActionFrameJob) ||
        (m_pEndDwellDeviceCommand))
    {
        return (TRUE);
    }

    return (FALSE);
}

void CANQPQueryJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    NDIS_STATUS                                         commandStatus, status = NDIS_STATUS_SUCCESS;
    CBSSEntry                                           *pMatchedBssEntry = NULL;
    WDI_GET_NEXT_ACTION_FRAME_DIALOG_TOKEN_PARAMETERS   getNextDialogToken;
    USHORT                                              uComebackDelay;

    UNREFERENCED_PARAMETER(SenderObject);

    TraceActEntry();

    CheckAndMarkPendingJobs (OperationStatus, SenderObject);

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        status = OperationStatus;
        WFCActErrorStatus("Previous Job Failed.");
        goto Exit;
    }

    if (m_IsCancelled)
    {
        status = NDIS_STATUS_REQUEST_ABORTED;
        WFCActErrorStatus("ANQP Query job got cancelled.");
        goto Exit;
    }

    switch (m_State)
    {
        case (WiFiANQPQueryJobStateWaitingForScanJobCompletion):
        {
            NT_ASSERT((SenderObject == &m_DiscoverJob) || (SenderObject == &m_ScanJob));

            if ((SenderObject != &m_DiscoverJob) && (SenderObject != &m_ScanJob))
            {
                WFCActError("[WaitingForScanJobCompletion] ERROR: Invalid SenderObject=<%p>, Scan=<%p>, Discover=<%p>",
                    SenderObject, &m_ScanJob, &m_DiscoverJob);
                break;
            }

            pMatchedBssEntry = FindCachedBSSEntry();
            if (pMatchedBssEntry)
            {
                status = StartANQPQueryTask(pMatchedBssEntry);
            }
            else
            {
                status = NDIS_STATUS_FILE_NOT_FOUND;
            }

            break;
        }

        case (WiFiANQPQueryJobStateWaitingForDialogToken):
        {
            ULONG   cbOutput;
            BYTE    *pbOutput;

            NT_ASSERT(SenderObject == m_pGetTokenDeviceCommand);

            if (SenderObject != m_pGetTokenDeviceCommand)
            {
                WFCActError("[WaitingForDialogToken] ERROR: Invalid SenderObject <%p> != <%p>",
                    SenderObject, m_pGetTokenDeviceCommand);
                break;
            }

            status = CMessageHelper::GetStatusFromCommandResult(m_pGetTokenDeviceCommand, &commandStatus);
            if (status == NDIS_STATUS_SUCCESS)
            {
                status = commandStatus;
            }

            if (status != NDIS_STATUS_SUCCESS)
            {
                WFCActErrorStatus("[WaitingForDialogToken] ERROR - GetStatusFromCommandResult FAILed");
                goto Exit;
            }

            //
            // successful return, cache the Dialog Token
            //
            status = m_pGetTokenDeviceCommand->get_CommandResult(&cbOutput, &pbOutput);
            if ((status != NDIS_STATUS_SUCCESS) ||
                (cbOutput < (sizeof(WFC_MESSAGE_METADATA)+sizeof(UINT8))))
            {
                WFCActErrorStatus("[WaitingForDialogToken] ERROR - get_CommandResult FAILed");
                goto Exit;
            }

            pbOutput += sizeof(WFC_MESSAGE_METADATA);
            cbOutput -= sizeof(WFC_MESSAGE_METADATA);

            status = ParseWdiGetNextActionFrameDialogToken(
                cbOutput,
                pbOutput,
                m_pAdapter->GetTlvContext(),
                &getNextDialogToken);
            if (status != NDIS_STATUS_SUCCESS)
            {
                WFCActErrorStatus("[WaitingForDialogToken] ERROR - ParseWifiGetNextDialogToken FAILed");
                goto Exit;
            }

            delete m_pGetTokenDeviceCommand;
            m_pGetTokenDeviceCommand = NULL;

            m_DialogToken = getNextDialogToken.NextDialogToken;
            WFCActInfo("GetNextDialogToken command returned Token = %d", (ULONG) getNextDialogToken.NextDialogToken);

            m_AwaitingANQPResponseTimerPending = TRUE;
            status = m_pEventQueue->RegisterTimeoutCallbackWithLevelAndReuse(
                get_ActivityId(),
                this,
                (WfcPortTypeExtSTA == m_PortType) ?
                    WFC_CONSTANT::DefaultANQPResponseFromAPTimeoutMs :
                    WFC_CONSTANT::DefaultANQPResponseFromP2PTimeoutMs,
                &m_AwaitingANQPResponseTimerToken,                      // WFC_TIMER_CONTEXT
                WDF_EXECUTION_LEVEL::WdfExecutionLevelPassive,
                false,
                NULL,
                &m_AwaitingANQPResponseTimerToken );
            if (NDIS_STATUS_SUCCESS != status)
            {
                WFCActErrorStatus("Couldn't register for Awaiting ANQP Response timeout.");
                goto Exit;
            }

            m_State = WiFiANQPQueryJobStateWaitingForGasResponse;
            status = BuildAndSendANQPRequestActionFrame(DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_INITIAL_REQUEST,
                                                        m_QueryBufferLength,
                                                        m_ANQPQueryInfo->ucQueryBuffer,
                                                        TRUE);

            if (status != NDIS_STATUS_SUCCESS)
            {
                WFCActErrorStatus("[WaitingForDialogToken]: ERROR - BuildAndSendANQPRequestActionFrame[INITIAL_REQUEST]  FAILed");
                goto Exit;
            }

            break;
        }

        case (WiFiANQPQueryJobStateWaitingForEndDwellTimeToSendComebackRequest):
        case (WiFiANQPQueryJobStateWaitingForEndDwellTimeToCompleteJob):
        {
            if (m_pSendActionFrameJob)
            {
                WFCActInfo("[WaitingForEndDwellTime]: SenderObject=<%p>, Still Waiting on SendCompletion Event=<%p>",
                    SenderObject, m_pSendActionFrameJob);
                break;
            }

            //
            // Set the next state first
            //
            if (WiFiANQPQueryJobStateWaitingForEndDwellTimeToSendComebackRequest == m_State)
            {
                m_State = WiFiANQPQueryJobStateWaitingToSendComebackRequest;
            }
            else
            {
                m_State = WiFiANQPQueryJobStateGasProtocolComplete;
            }

            NT_ASSERT (m_pEndDwellDeviceCommand == NULL);
            m_pEndDwellDeviceCommand = new DeviceCommand( get_ActivityId() );
            if (m_pEndDwellDeviceCommand)
            {
                status = m_pEndDwellDeviceCommand->Initialize(
                    get_PortId(),
                    WDI_SET_END_DWELL_TIME,
                    sizeof(m_SetEndDwellBuffer),
                    (BYTE*)&m_SetEndDwellBuffer);
                if (status == NDIS_STATUS_SUCCESS)
                {
                    status = QueueDeviceCommand(m_pEndDwellDeviceCommand);
                    if (status == NDIS_STATUS_SUCCESS)
                    {
                        WFCActInfo("Queued EndDwell DeviceCommand <%p>\n", m_pEndDwellDeviceCommand);
                    }
                    else
                    {
                        WFCActError("ERROR - Failed to queue EndDwellDeviceCommand (0x%x)", status);
                    }
                }
                else
                {
                    WFCActError("ERROR - Failed to initialize EndDwell DeviceCommand 0x%x", status);
                }
            }
            else
            {
                status = NDIS_STATUS_RESOURCES;
                WFCActError("ERROR - Failed to Allocate Device Command for EndDwell Command 0x%x", status);
            }

            //
            // In case of failure, must NULL the EndDwell command so that subsequent requests do not expect it
            //
            if ((status != NDIS_STATUS_SUCCESS) &&
                (m_pEndDwellDeviceCommand))
            {
                delete m_pEndDwellDeviceCommand;
                m_pEndDwellDeviceCommand = NULL;
            }

            break;
        }

        case (WiFiANQPQueryJobStateWaitingToSendComebackRequest):
        {
            if ((m_pSendActionFrameJob) || (m_pEndDwellDeviceCommand) || (m_ComebackTimerPending))
            {
                WFCActInfo("[WaitingToSendComebackRequest]: SenderObject=<%p>, Still Waiting on Event(s): m_pSendActionFrameJob=<%p>, m_pEndDwellDeviceCommand=<%p>, m_ComebackTimerPending=<%x>",
                    SenderObject, m_pSendActionFrameJob, m_pEndDwellDeviceCommand, m_ComebackTimerPending);
                break;
            }

            m_AwaitingANQPResponseTimerPending = TRUE;
            status = m_pEventQueue->RegisterTimeoutCallbackWithLevelAndReuse(
                get_ActivityId(),
                this,
                (WfcPortTypeExtSTA == m_PortType) ?
                    WFC_CONSTANT::DefaultANQPResponseFromAPTimeoutMs :
                    WFC_CONSTANT::DefaultANQPResponseFromP2PTimeoutMs,
                &m_AwaitingANQPResponseTimerToken,                      // WFC_TIMER_CONTEXT
                WDF_EXECUTION_LEVEL::WdfExecutionLevelPassive,
                false,
                NULL,
                &m_AwaitingANQPResponseTimerToken );
            if (NDIS_STATUS_SUCCESS != status)
            {
                WFCActErrorStatus("Couldn't register for Awaiting ANQP Response timeout.");
                goto Exit;
            }

            //
            // If we are sending after a ComebackDelay, send as a response frame, otherwise send as a request frame
            //
            uComebackDelay = m_uComebackDelay;
            m_uComebackDelay = 0;
            m_State = WiFiANQPQueryJobStateWaitingForGasResponse;
            status = BuildAndSendANQPRequestActionFrame(DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_COMEBACK_REQUEST,
                                                        0,
                                                        NULL,
                                                        (uComebackDelay ? TRUE : FALSE));
            if (NDIS_STATUS_SUCCESS != status)
            {
                m_QueryResult = dot11_ANQP_query_result_resources;
                WFCActErrorStatus("[WaitingToSendComebackRequest]: ERROR - BuildAndSendANQPRequestActionFrame [COMEBACK_REQUEST] FAILed");
            }

            break;
        }

        //
        // The following states indicate success or failure of the Job
        //
        case (WiFiANQPQueryJobStateGasProtocolComplete):
        {
            //
            // If we have a Send task or EndDwell command pending, wait for them to complete
            //
            if ((m_pSendActionFrameJob) || (m_pEndDwellDeviceCommand))
            {
                WFCActInfo("[Complete]: SenderObject=<%p>, Still Waiting on Event(s): m_pSendActionFrameJob=<%p>, m_pSendActionFrameJob=<%p>",
                    SenderObject, m_pSendActionFrameJob, m_pEndDwellDeviceCommand);
                break;
            }

            CleanupAndCompleteJob(status);
            break;
        }

        //
        // No action required for following cases
        //
        case (WiFiANQPQueryJobStateWaitingForGasResponse):
        {
            break;
        }

        default:
        {
            NT_ASSERT(FALSE);
        }
    }

Exit:
    if (NDIS_STATUS_SUCCESS != status)
    {
        //
        // Save the Job status in case we need to wait for some pending task and complete the job later
        //
        if (m_JobStatus == NDIS_STATUS_SUCCESS)
        {
            m_JobStatus = status;
        }

        //
        // If we are currently waiting on some tasks or commands to complete, try to cancel and continue waiting
        //
        if (CheckAndMarkPendingJobs (0, NULL))
        {
            OnJobCancelled();
        }
        else
        {
            CleanupAndCompleteJob(m_JobStatus);
        }
    }

    TraceActExit(status);
}


