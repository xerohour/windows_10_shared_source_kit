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
#include "Scan.tmh"


CScanJob::CScanJob(
    _In_ WFC_ACTIVITY_ID ActivityId) :
    CJobBase(ActivityId),
    m_ScanTask(ActivityId),
    m_IsCancelled(false),
    m_ScanParametersTlvLength(0),
    m_pScanParametersTlvBuffer(NULL),
    m_UpdateBSSListCache(false),
    m_UpdateBSSListCacheJob(ActivityId),
    m_State(WiFiScanJobStateInit)
{
    RtlZeroMemory(&m_ScanParameters, sizeof(m_ScanParameters));
}

CScanJob::~CScanJob()
{
    if (m_ScanParameters.SSIDList.pElements)
    {
        delete[] m_ScanParameters.SSIDList.pElements;
    }

    if (m_ScanParameters.BandChannelList.pElements)
    {
        for (UINT32 i = 0; i < m_ScanParameters.BandChannelList.ElementCount; i++)
        {
            if (m_ScanParameters.BandChannelList.pElements[i].ChannelList.pElements)
            {
                delete[] m_ScanParameters.BandChannelList.pElements[i].ChannelList.pElements;
                m_ScanParameters.BandChannelList.pElements[i].ChannelList.pElements = NULL;
            }
        }
        //delete[] m_ScanParameters.BandChannelList.pElements;
    }

    // m_ScanParameters.VendorIEs.pElements is auto-deleted 

    if (m_pScanParametersTlvBuffer)
    {
        FreeGenerated( m_pScanParametersTlvBuffer );
    }
}


LONG g_DbgEnableInstConn=0;

NDIS_STATUS CScanJob::InitializeFromScanParams(
    _In_ CAdapter* pAdapter,
    _In_ NDIS_PORT_NUMBER PortNumber,
    _In_ WFC_SCAN_JOB_PARAMETERS* pScanJobParams
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    POID_HANDLER_ENTRY pHandlerInfo;

    TraceActEntry();

    m_State = WiFiScanJobStateInit;

    if (pAdapter == NULL ||
        pScanJobParams == NULL)
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCActError("[Scan] Bad params to %!FUNC!");
        goto Exit;
    }

    // To simplify things, use the adapter's table to find the priority, etc
    pHandlerInfo = COidJobBase::s_FindOidHandlerEntry(OID_DOT11_SCAN_REQUEST);
    if (pHandlerInfo == NULL)
    {
        WFCActError("[Scan] Failed to find handler entry for OID_DOT11_SCAN_REQUEST");
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto Exit;
    }

    //
    // Shallow copy
    //
    RtlCopyMemory(
        &m_ScanParameters,
        &(pScanJobParams->WdiScanParameters),
        sizeof(WDI_SCAN_PARAMETERS));

    m_ScanParameters.SSIDList.pElements = NULL;
    m_ScanParameters.VendorIEs.pElements = NULL;
    m_ScanParameters.BandChannelList.pElements = NULL;

    //
    // Deep copy
    //
    if (pScanJobParams->SSIDListCount > 0)
    {
        m_ScanParameters.SSIDList.pElements =
            new WDI_SSID[pScanJobParams->SSIDListCount];

        if (m_ScanParameters.SSIDList.pElements == NULL)
        {
            WFCActError("[Scan] Failed to allocate memory for SSID list");
            ndisStatus = NDIS_STATUS_RESOURCES;
            goto Exit;
        }

        m_ScanParameters.SSIDList.ElementCount = pScanJobParams->SSIDListCount;

        // Copy each of the elements individually
        for (UINT32 i = 0; i < m_ScanParameters.SSIDList.ElementCount; i++)
        {
            ndisStatus = m_ScanParameters.SSIDList.pElements[i].SimpleSet(
                pScanJobParams->pSSIDList[i].ucSSID,
                pScanJobParams->pSSIDList[i].uSSIDLength,
                0
                );
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCActError("[Scan] Failed to allocate memory for SSIDs list");
                goto Exit;
            }
        }
    }

    if (pScanJobParams->VendorIELength > 0)
    {
        m_ScanParameters.Optional.VendorIEs_IsPresent = TRUE;
        ndisStatus = m_ScanParameters.VendorIEs.SimpleSet(
            pScanJobParams->pVendorIEs,
            pScanJobParams->VendorIELength,
            0
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("[Scan] Failed to allocate memory for VendorSpecificIE list");
            goto Exit;
        }
    }

    if (pScanJobParams->ChannelList24GHzCount > 0 || pScanJobParams->ChannelList5GHzCount > 0 )
    {
        m_ScanParameters.Optional.BandChannelList_IsPresent = TRUE;

        ULONG curIndex = 0;
        //There is at least one channel in either 2.4 or 5.  check to see if both bands needed.
        ULONG numBandChannelLists = 
            (pScanJobParams->ChannelList24GHzCount > 0 && pScanJobParams->ChannelList5GHzCount > 0) ? 2 : 1;

        ndisStatus = m_ScanParameters.BandChannelList.AllocateElements(numBandChannelLists, 0);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("[Scan] Failed to allocate memory for band channel list");
            goto Exit;
        }

        //If 2.4GHz needed
        if (pScanJobParams->ChannelList24GHzCount > 0)
        {
            m_ScanParameters.BandChannelList.pElements[curIndex].BandID = WDI_BAND_ID_2400;
            m_ScanParameters.BandChannelList.pElements[curIndex].ChannelList.pElements = NULL;

            ndisStatus = m_ScanParameters.BandChannelList.pElements[curIndex].ChannelList.SimpleSet(
                pScanJobParams->pChannelList24GHz,
                pScanJobParams->ChannelList24GHzCount,
                0
                );
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCActError("[Scan] Failed to allocate memory for 2.4GHz band channel info list");
                goto Exit;
            }

            curIndex++;
        }

        //If 5GHz needed
        if (pScanJobParams->ChannelList5GHzCount > 0)
        {
            m_ScanParameters.BandChannelList.pElements[curIndex].BandID = WDI_BAND_ID_5000;
            m_ScanParameters.BandChannelList.pElements[curIndex].ChannelList.pElements = NULL;

            ndisStatus = m_ScanParameters.BandChannelList.pElements[curIndex].ChannelList.SimpleSet(
                pScanJobParams->pChannelList5GHz,
                pScanJobParams->ChannelList5GHzCount,
                0
                );
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCActError("[Scan] Failed to allocate memory for 5GHz band channel info list");
                goto Exit;
            }

            curIndex++;
        }

        NT_ASSERT(curIndex == m_ScanParameters.BandChannelList.ElementCount);
    }

    m_pAdapter = pAdapter;
    m_NdisPortNumber = PortNumber;
    m_UpdateBSSListCache = pScanJobParams->UpdateBSSListCache;
    m_BypassScanThrottling = pScanJobParams->BypassScanThrottling;
    
    // Initialize the base
    ndisStatus = CJobBase::Initialize(
        m_pAdapter->GetEventQueue(),
        m_pAdapter->GetDeviceCommandScheduler(),
        m_pAdapter->GetActiveJobsList(),
        m_pAdapter->GetSerializedJobList(),
        WiFiJobScan,
        pHandlerInfo->IsPreemptable,
        pHandlerInfo->CanPreempt,
        pHandlerInfo->PriorityScope,
        pHandlerInfo->Priority,             // Use priority from table. If queued as child of serialized job, then its not used. 
        // If queued as child of non-serialized job, then its used
        (IPropertyCacheDirectory *)m_pAdapter,
        (INdisConversion *)m_pAdapter,
        PortNumber,
        true // Although scan job currently does not make use of the deferrable property, 
             // treating it as deferrable will ensure that roam job gets to run before scan does in case both are queued.
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Scan] Failed to initialize base job (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:
    // Destructor would free the m_ScanParameters structure

    TraceActExit(ndisStatus);

    return ndisStatus;
}

NDIS_STATUS CScanJob::StartScanTask()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    DeviceCommand* pDeviceCommand = NULL;

    TraceActEntry();

    // Send the scan job to the device
    ndisStatus = m_ScanTask.Initialize(
        m_pAdapter->GetNotificationManager(),
        m_pAdapter->GetDeviceCommandScheduler(),
        m_pAdapter->GetEventQueue()
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Scan] Failed to initialize Scan Task (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = m_ScanTask.get_TaskDeviceCommand(&pDeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Scan] Failed to get Device Command for Scan Task (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = GenerateWdiTaskScan(&m_ScanParameters,
        (ULONG)sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &m_ScanParametersTlvLength,
        &m_pScanParametersTlvBuffer);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Scan] Failed to generate TLV for Scan Parameters (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = pDeviceCommand->Initialize(
        get_PortId(),
        WDI_TASK_SCAN,
        m_ScanParametersTlvLength,
        m_pScanParametersTlvBuffer
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Scan] Failed to initialize Device Command for Scan Task (0x%x)", ndisStatus);
        goto Exit;
    }

    // Save the time when we started this scan. It will be persisted if 
    // we finish successfully
    m_ScanStartTime = CSystem::get_CurrentTime();

    ndisStatus = StartTask(&m_ScanTask);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Scan] Failed to start Create Port Task (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:

    TraceActExit(ndisStatus);

    return ndisStatus;
}

NDIS_STATUS CScanJob::CompleteScanTask(
    _In_ NDIS_STATUS OperationStatus)
{
    NDIS_STATUS taskStatus;

    TraceActEntry();

    WFCActInfo("[Scan] Scan Task completed (0x%x)", OperationStatus);

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Scan] Scan operation failed with status (0x%x)",
            OperationStatus);    
        goto Exit;
    }

    // Get the results from the TLV
    OperationStatus = CMessageHelper::GetStatusFromTaskOutput(
        &m_ScanTask,
        &taskStatus
        );
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Scan] Scan Task failed with status (0x%x) %d",
            OperationStatus, taskStatus);
        goto Exit;
    }

    if (get_NdisPortNumber() == NDIS_DEFAULT_PORT_NUMBER)
    {
        WFCActTrace("[Scan] Notifying that scan list has changed.");
        m_pAdapter->GetBSSListManager(WfcPortTypeExtSTA)->NotifyBSSListChanged();
    }

Exit:
    TraceActExit(OperationStatus);

    return OperationStatus;
}

void CScanJob::FinishJob(
    _In_ NDIS_STATUS JobCompleteStatus)
{
    // Tell the BSS List manager that the scan has finished
    WFC_PORT_TYPE scanCacheSource = WfcPortTypeExtSTA;
    if (get_NdisPortNumber() != NDIS_DEFAULT_PORT_NUMBER)
    {
        scanCacheSource = WfcPortTypeWFDDevice;
    }

    CBSSListManager* pBSSListManager = m_pAdapter->GetBSSListManager(scanCacheSource);    
    pBSSListManager->OnScanComplete();

    if (JobCompleteStatus == NDIS_STATUS_SUCCESS && !m_IsCancelled)
    {
        NDIS_STATUS saveStatus = NDIS_STATUS_SUCCESS;
        
        CPortPropertyCache* pPortPropertyCache;
    
        // Save completion state
        saveStatus = get_PortPropertyCache(&pPortPropertyCache);
        if (saveStatus == NDIS_STATUS_SUCCESS)
        {
            saveStatus = pPortPropertyCache->SetPropertyBuffer(
                WfcPortPropertyGoodScanStartTime, 
                sizeof(ULONGLONG),
                (BYTE *)&m_ScanStartTime
                );
        }
        if (saveStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("[Scan] Failed to save scan start time (0x%x)", saveStatus);
        }
        
    }

    CompleteJob(JobCompleteStatus);
}


void CScanJob::OnJobStarted()
{
    // Listen for notifications from the device
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // The parent job may have cancelled a child scan job before OnJobStarted
    // got a chance to run. In that case, don't proceed.
    if (m_IsCancelled)
    {
        WFCActInfo("[Scan] Job already cancelled. Not proceeding with scan.");
        ndisStatus = NDIS_STATUS_REQUEST_ABORTED;
        goto Exit;
    }

    if (!IsScanAllowed())
    {
        WFCActInfo("[Scan] Scan is not currently allowed. Failing with media in use.");
        ndisStatus = NDIS_STATUS_DOT11_MEDIA_IN_USE;
        goto Exit;
    }

    //
    // check if our radio is off, we dont start scan
    //
    CAdapterPropertyCache* propertyCache = m_pAdapter->GetAdapterPropertyCache();
    NDIS_STATUS status;
    BOOLEAN bCurHardwareRadioOff = FALSE;
    BOOLEAN bCurSoftwareRadioOff = FALSE;

    status = propertyCache->GetPropertyBoolean(
        WfcAdapterHardwareRadioOff,
        &bCurHardwareRadioOff);
    if (status != STATUS_SUCCESS)
    {
        NT_ASSERT(0);
        WFCError("[Scan] Failed to read cache for bCurHardwareRadioOff 0x%x", status);
    }
    status = propertyCache->GetPropertyBoolean(
        WfcAdapterSoftwareRadioOff,
        &bCurSoftwareRadioOff);
    if (status != STATUS_SUCCESS)
    {
        NT_ASSERT(0);
        WFCError("[Scan] Failed to read cache for bCurSoftwareRadioOff 0x%x", status);
    }

    if (bCurHardwareRadioOff || bCurSoftwareRadioOff)
    {
        //
        // no radio, dont start scan
        //
        WFCInfo("[Scan] Radio is off, Dont scan");
        ndisStatus = NTSTATUS2NDISSTATUS(STATUS_INVALID_DEVICE_STATE);
        goto Exit;
    }

    // Tell the BSS list manager about the scan state    
    WFC_PORT_TYPE scanCacheSource = WfcPortTypeExtSTA;
    if (get_NdisPortNumber() != NDIS_DEFAULT_PORT_NUMBER)
    {
        scanCacheSource = WfcPortTypeWFDDevice;
    }

    m_pBSSListManager = m_pAdapter->GetBSSListManager(scanCacheSource);    
    m_pBSSListManager->OnScanStart(m_ScanParameters.ScanModeParameters.ScanTrigger);

    if (!m_pBSSListManager->IsIHVMaintainedExtSTABSSListCache())
    {
        // Host maintained cache. Live updates are always needed
        m_ScanParameters.ScanModeParameters.LiveUpdatesNeeded = TRUE;

        // We get live updates so dont need to update the BSS list cache
        m_UpdateBSSListCache = false;
    }

    // Launch the scan task
    ndisStatus = StartScanTask();
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Scan] Failed to start Scan Task (0x%x)", ndisStatus);
        goto Exit;
    }
    
    m_State = WiFiScanJobStateWaitingForScanTaskCompletion;

Exit:
    // If anything failed, we will complete the job right now
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        FinishJob(ndisStatus);
    }

    TraceActExit(ndisStatus);
}

void CScanJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    BOOLEAN completeJob = FALSE;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    UNREFERENCED_PARAMETER(SenderObject);

    switch (m_State)
    {
        case WiFiScanJobStateWaitingForScanTaskCompletion:
            {
                ndisStatus = CompleteScanTask(OperationStatus);
                if (ndisStatus == NDIS_STATUS_SUCCESS)
                {
                    if (m_UpdateBSSListCache)
                    {
                        // BSS list update is needed. Trigger that job
                        ndisStatus = m_UpdateBSSListCacheJob.Initialize(
                            m_pAdapter,
                            m_NdisPortNumber,
                            m_pBSSListManager
                            );
                        if ( ndisStatus != NDIS_STATUS_SUCCESS )
                        {
                            WFCActError("[Scan] Failed to initialize job to update BSS list. Error = 0x%08x", ndisStatus);
                            goto Exit;
                        }

                        ndisStatus = StartChildJob(&m_UpdateBSSListCacheJob);
                        if ( ndisStatus != NDIS_STATUS_SUCCESS )
                        {
                            WFCActError("[Scan] Failed to start child job to update BSS list. Error = 0x%08x", ndisStatus);
                            goto Exit;
                        }
                        m_State = WiFiScanJobStateWaitingForBSSCacheUpdate;

                    }
                    else
                    {
                        // No need to update the bss cache
                        completeJob = TRUE;
                    }
                }
            }
            break;
        case WiFiScanJobStateWaitingForBSSCacheUpdate:
            {
                // BSS cache update finished
                if ( OperationStatus != NDIS_STATUS_SUCCESS )
                {
                    WFCActError("[Scan] BSS list update job command failed with status %x.", OperationStatus);
                }
                ndisStatus = OperationStatus;

                completeJob = TRUE;
            }
            break;
    };
    
Exit:
    if ((ndisStatus != NDIS_STATUS_SUCCESS) ||
        (completeJob == TRUE))
    {
        FinishJob(ndisStatus);
    }

    TraceActExit(ndisStatus);
}

void CScanJob::OnJobCancelled()
{
    m_IsCancelled = true;
    WFCActTrace("[Scan] Scan task is being cancelled.");

    // Cancel the scan task
    m_ScanTask.CancelTask();
}

bool CScanJob::IsScanAllowed()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    bool scanAllowed = true;
    ULONG currentListenState;
    ULONGLONG currentTime;
    CPort* pPort = m_pAdapter->GetPortFromPortId(get_PortId());
    ULONG mediaStreamingRefCount;

    TraceActEntry();

    if (m_BypassScanThrottling)
    {
        WFCActInfo("[Scan] Scan is manual or for connect.");
        goto exit;
    }

    status = m_pAdapter->GetAdapterPropertyCache()->GetPropertyULong(
        WfcAdapterPropertyMediaStreamingRefcount,
        &mediaStreamingRefCount);

    if (status == NDIS_STATUS_SUCCESS &&
        mediaStreamingRefCount > 0)
    {
        WFCActInfo(
            "[Scan] Background scan not allowed due to media streaming ref count being non-zero (%d)",
            mediaStreamingRefCount);

        scanAllowed = false;

        goto exit;
    }

    status = m_pAdapter->GetAdapterPropertyCache()->GetPropertyULong(
        WfcAdapterPropertyCurrentListenState,
        &currentListenState);

    if (status == NDIS_STATUS_SUCCESS &&
        currentListenState != WDI_P2P_LISTEN_STATE_OFF)
    {
        WFCActInfo("[Scan] Listen state is on.");
        scanAllowed = false;
    }

    currentTime = CSystem::get_CurrentTime();

    if (currentTime <= m_pAdapter->GetNextAllowedScanTime())
    {
        WFCActInfo(
            "[Scan] Trying to bg scan before the next allowed scan time (current = %llu, next Allowed = %llu).",
            currentTime,
            m_pAdapter->GetNextAllowedScanTime());
        scanAllowed = false;
    }

    if (!scanAllowed)
    {
        if (pPort->m_ScanSkipCount >
            m_pAdapter->GetAdapterPropertyCache()->GetPropertyULongOrDefault(WfcAdapterPropertyRegMaxBackgroundScanSkipCount, 1))
        {
            WFCActInfo(
                "[Scan] BG Scan is not allowed, but we have already skipped too many background scans (%u). Force allow.",
                pPort->m_ScanSkipCount);

            scanAllowed = true;
            pPort->m_ScanSkipCount = 0;
        }
        else
        {
            pPort->m_ScanSkipCount++;
        }
    }

exit:
    TraceActExit(scanAllowed);
    return scanAllowed;
}

//==============================================================================================

CScanOidJob::CScanOidJob(
    _In_ WFC_ACTIVITY_ID ActivityId) :
    COidJobBase(ActivityId),
    m_pScanJob(NULL),
    m_IsCancelled(false),
    m_TimerToken(WFC_TIMER_REGISTRATION_TOKEN_INVALID),
    m_IndicateCompletion(false),
    m_ScanOidSubmitTime(0)
{
    m_ScanJobParameters.InitializeDefaults();
}

CScanOidJob::~CScanOidJob()
{
    if (m_ScanJobParameters.pSSIDList)
    {
        delete[] m_ScanJobParameters.pSSIDList;
        m_ScanJobParameters.pSSIDList = NULL;
    }

    if (m_ScanJobParameters.pVendorIEs)
    {
        delete[] m_ScanJobParameters.pVendorIEs;
        m_ScanJobParameters.pVendorIEs = NULL;
    }
    
    if (m_pScanJob)
    {
        delete m_pScanJob;
        m_pScanJob = NULL;
    }

    if (m_TimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        (VOID)m_pEventQueue->DeregisterTimeoutCallback(
            m_TimerToken);
        m_TimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }
}

NDIS_STATUS CScanOidJob::ValidateScanRequest(
    _In_ PDOT11_SCAN_REQUEST_V2  Dot11ScanRequest,
    _In_ ULONG RequestLength
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG           i;
    PDOT11_SSID     dot11SSID;
    ULONG           requiredLength = 0;
    ULONG           bufferLength = 0;
    NTSTATUS        ntStatus;

    TraceActEntry();

    //
    // Perform some validation on the scan request.
    //    
    __analysis_assume(RequestLength >= FIELD_OFFSET(DOT11_SCAN_REQUEST_V2, ucBuffer));
    bufferLength = RequestLength - FIELD_OFFSET(DOT11_SCAN_REQUEST_V2, ucBuffer);

    // Check SSIDs offset/length        
    ntStatus = RtlULongMult(Dot11ScanRequest->uNumOfdot11SSIDs, sizeof(DOT11_SSID), &requiredLength);
    if (!NT_SUCCESS(ntStatus))
    {
        ndisStatus = NTSTATUS2NDISSTATUS(ntStatus);
        goto Exit;
    }

    ntStatus = RtlULongAdd(requiredLength, Dot11ScanRequest->udot11SSIDsOffset, &requiredLength);
    if (!NT_SUCCESS(ntStatus))
    {
        ndisStatus = NTSTATUS2NDISSTATUS(ntStatus);
        goto Exit;
    }

    if (requiredLength > bufferLength)
    {
        WFCActError("[Scan] Invalid Length for SSIDs in Scan Request. Expected %d. Received %d",
            requiredLength, bufferLength);
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        goto Exit;
    }

    // Check IEs offset/length
    ntStatus = RtlULongAdd(Dot11ScanRequest->uIEsOffset, Dot11ScanRequest->uIEsLength, &requiredLength);
    if (!NT_SUCCESS(ntStatus))
    {
        ndisStatus = NTSTATUS2NDISSTATUS(ntStatus);
        goto Exit;
    }

    if (requiredLength > bufferLength)
    {
        WFCActError("[Scan] Invalid Length for IEs in Scan Request. Expected %d. Received %d",
            requiredLength, bufferLength);
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        goto Exit;
    }


    if (Dot11ScanRequest->dot11BSSType != dot11_BSS_type_infrastructure &&
        Dot11ScanRequest->dot11BSSType != dot11_BSS_type_independent &&
        Dot11ScanRequest->dot11BSSType != dot11_BSS_type_any)
    {
        WFCActError("[Scan] BSS Type %d not supported", Dot11ScanRequest->dot11BSSType);
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto Exit;
    }

    if ((Dot11ScanRequest->dot11ScanType != (dot11_scan_type_active)) &&
        (Dot11ScanRequest->dot11ScanType != (dot11_scan_type_active | dot11_scan_type_forced)) &&
        (Dot11ScanRequest->dot11ScanType != (dot11_scan_type_passive)) &&
        (Dot11ScanRequest->dot11ScanType != (dot11_scan_type_passive | dot11_scan_type_forced)) &&
        (Dot11ScanRequest->dot11ScanType != (dot11_scan_type_auto)) &&
        (Dot11ScanRequest->dot11ScanType != (dot11_scan_type_auto | dot11_scan_type_forced)))
    {
        WFCActError("[Scan] Dot11 scan type %d not supported", Dot11ScanRequest->dot11ScanType);
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto Exit;
    }

    dot11SSID = (PDOT11_SSID)(Dot11ScanRequest->ucBuffer +
        Dot11ScanRequest->udot11SSIDsOffset);
    for (i = 0; i<Dot11ScanRequest->uNumOfdot11SSIDs; i++)
    {
        if (dot11SSID[i].uSSIDLength > DOT11_SSID_MAX_LENGTH)
        {
            WFCActError("[Scan] The SSID length provided (%d) is greater than max SSID length (%d)",
                dot11SSID[i].uSSIDLength,
                DOT11_SSID_MAX_LENGTH);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            goto Exit;
        }
    }


Exit:
    TraceActExit(ndisStatus);

    return ndisStatus;
}

NDIS_STATUS CScanOidJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_SCAN_REQUEST_V2  dot11ScanRequest =
        (PDOT11_SCAN_REQUEST_V2)SET_OID_BUFFER(pOidRequest);

    TraceActEntry();

    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;

    //
    // Perform some validation on the scan request.
    //
    ndisStatus = ValidateScanRequest(dot11ScanRequest,
        SET_OID_BUFFER_LENGTH(pOidRequest)
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Scan] Failed invalid scan OID request (0x%x)", ndisStatus);
        goto Exit;
    }

    pOidRequest->DATA.SET_INFORMATION.BytesRead = sizeof(DOT11_SCAN_REQUEST_V2);

    //Initialize default values
    m_ScanJobParameters.InitializeDefaults();

    RtlCopyMemory(
        &m_ScanJobParameters.WdiScanParameters.BSSID,
        &dot11ScanRequest->dot11BSSID,
        sizeof(DOT11_MAC_ADDRESS));

    // Save the OID buffer so that it can be reused for retries, etc
    if (dot11ScanRequest->uNumOfdot11SSIDs > 0)
    {
        m_ScanJobParameters.pSSIDList = new DOT11_SSID[dot11ScanRequest->uNumOfdot11SSIDs];
        if (m_ScanJobParameters.pSSIDList == NULL)
        {
            WFCActError("[Scan] Failed to allocate space for OID's SSID list");
            ndisStatus = NDIS_STATUS_RESOURCES;
            goto Exit;
        }
        RtlCopyMemory(m_ScanJobParameters.pSSIDList,
            (dot11ScanRequest->ucBuffer + dot11ScanRequest->udot11SSIDsOffset),
            sizeof(DOT11_SSID) * dot11ScanRequest->uNumOfdot11SSIDs);
    }    
    m_ScanJobParameters.SSIDListCount = dot11ScanRequest->uNumOfdot11SSIDs;

    // Save the OID buffer so that it can be reused for retries, etc
    if (dot11ScanRequest->uIEsLength > 0)
    {
        m_ScanJobParameters.pVendorIEs = new UCHAR[dot11ScanRequest->uIEsLength];
        if (m_ScanJobParameters.pVendorIEs == NULL)
        {
            WFCActError("[Scan] Failed to allocate space for OID's vendor IE list");
            ndisStatus = NDIS_STATUS_RESOURCES;
            goto Exit;
        }
        RtlCopyMemory(m_ScanJobParameters.pVendorIEs,
            (dot11ScanRequest->ucBuffer + dot11ScanRequest->uIEsOffset),
            dot11ScanRequest->uIEsLength);
    }
    m_ScanJobParameters.VendorIELength = dot11ScanRequest->uIEsLength;

    if (dot11ScanRequest->dot11ScanType & dot11_scan_type_forced)
    {
        m_ScanJobParameters.BypassScanThrottling = TRUE;
        m_ScanJobParameters.WdiScanParameters.ScanModeParameters.ScanTrigger = WDI_SCAN_TRIGGER_MANUAL;
    }

    // Set this scan to do passive only if instructed by policies in upper layers (wlansvc)
    BOOLEAN bIsPassiveOnly = false;
    bIsPassiveOnly = m_pAdapter->GetAdapterPropertyCache()->GetPropertyBooleanOrDefault(WfcAdapterPropertyScanModePassiveOnly, FALSE);

    if (bIsPassiveOnly &&
        m_ScanJobParameters.WdiScanParameters.ScanModeParameters.ScanTrigger != WDI_SCAN_TRIGGER_MANUAL)
    {
        m_ScanJobParameters.WdiScanParameters.ScanModeParameters.AllowedScanType = WDI_SCAN_TYPE_PASSIVE_ONLY;
    }
    else
    {
        m_ScanJobParameters.WdiScanParameters.ScanModeParameters.AllowedScanType =
            CDot11ToWabiConverter::MapScanType(dot11ScanRequest->dot11ScanType);
    }
Exit:
    // Keep OACR happy
    _Analysis_assume_( m_ScanJobParameters.pSSIDList->uSSIDLength <= 32 );
    // Destructor would free the m_ScanParameters structure

    TraceActExit(ndisStatus);

    return ndisStatus;
}

VOID CScanOidJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceActEntry();

    // Send the complete indication to OS
    if (m_IndicateCompletion)
    {
        m_pAdapter->IndicateStatus(
            m_OidPortNumber,
            NDIS_STATUS_DOT11_SCAN_CONFIRM,
            m_OidRequestId,
            &NdisStatus,
            sizeof(NDIS_STATUS));
    }
    TraceActExit(NdisStatus);
}

NDIS_STATUS CScanOidJob::StartScanJob()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // Allocate a new Scan Job
    m_pScanJob = new CScanJob(get_ActivityId());
    if (m_pScanJob == NULL)
    {
        WFCActError("[Scan] Failed to allocate Scan Job");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto Exit;
    }

    // If its not a forced scan and we are in media streaming mode,
    // do a shorter active scan dwell time
    if (!m_ScanJobParameters.BypassScanThrottling && 
         (m_pAdapter->GetAdapterPropertyCache()->GetPropertyULongOrDefault(
            WfcAdapterPropertyMediaStreamingRefcount, 0) > 0))
    {
        m_ScanJobParameters.WdiScanParameters.DwellTime.ActiveChannelDwellTime = 
                WFC_CONSTANT::MediaStreamingOffChannelTimeLimit;            
    }

    ndisStatus = m_pScanJob->InitializeFromScanParams(
        m_pAdapter,
        m_NdisPortNumber,
        &m_ScanJobParameters);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Scan] Failed to initialize scan job from OID scan parameters (0x%x)", ndisStatus);
        goto Exit;
    }

    // Queue the serialized scan job
    ndisStatus = CJobBase::QueueSerializedJob(
        m_pScanJob
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Scan] Failed to queue scan job (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (m_pScanJob)
        {
            delete m_pScanJob;
            m_pScanJob = NULL;
        }
    }

    TraceActExit(ndisStatus);

    return ndisStatus;
}

NDIS_STATUS CScanOidJob::CompleteScanJob(
    _In_ NDIS_STATUS OperationStatus,
    _Out_ bool* WasCancelled)
{
    TraceActEntry();

    WFCActInfo("[Scan] Child Scan Job completed (0x%x)", OperationStatus);

    if (m_pScanJob->IsCancelled())
    {
        *WasCancelled = true;
        WFCActTrace("[Scan] Child Job was Cancelled");
    }
    else
    {
        *WasCancelled = false;
        WFCActTrace("[Scan] Child Job was NOT Cancelled");
    }

    // Delete the scan job
    delete m_pScanJob;
    m_pScanJob = NULL;

    TraceActExit(OperationStatus);

    return OperationStatus;
}

void CScanOidJob::FinishJob(
    _In_ NDIS_STATUS JobCompleteStatus)
{
    CompleteJob(JobCompleteStatus);
}

void CScanOidJob::OnTimerCallback(
    _In_opt_ WFC_TIMER_CONTEXT)
{
    WFCActInfo("[Scan] Scan OID Job time limit expired. Completing the job");

    // Cancel 
    CancelJob();
}

void CScanOidJob::OnJobStarted()
{
    // Listen for notifications from the device
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN finishTheJob = FALSE;

    TraceActEntry();

    if (m_OidPortNumber != NDIS_DEFAULT_PORT_NUMBER)
    {
        WFCActError("[Scan] Scan on non default ndis port ( %u) is not supported", m_OidPortNumber);
        finishTheJob = TRUE;
        goto Exit;
    }

    // Save the time that we start the scan.
    m_ScanOidSubmitTime = CSystem::get_CurrentTime();
    ndisStatus = m_pEventQueue->RegisterTimeoutCallbackWithLevelAndReuse(
        get_ActivityId(),
        this,
        WFC_CONSTANT::MaxScanOidTimeInMs,
        NULL,
        WDF_EXECUTION_LEVEL::WdfExecutionLevelPassive,
        false,
        NULL,
        &m_TimerToken);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Scan] Failed to register for Scan complete timer callback (0x%x)", ndisStatus);
        goto Exit;
    }

    // Launch the scan job
    ndisStatus = StartScanJob();
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Scan] Failed to start Scan Job (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:
    // If we complete with success, we must indicate confirm indication
    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        m_IndicateCompletion = true;
    }

    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        ndisStatus);

    // If anything failed, we will complete the job right now
    if (ndisStatus != NDIS_STATUS_SUCCESS || finishTheJob)
    {
        FinishJob(ndisStatus);
    }

    TraceActExit(ndisStatus);
}

void CScanOidJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    bool completeJob = true;
    bool childScanJobCancelled = false;
    ULONGLONG currentTime;

    TraceActEntry();

    UNREFERENCED_PARAMETER(SenderObject);

    OperationStatus = CompleteScanJob(OperationStatus, &childScanJobCancelled);

    // If child scan job was cancelled & we didnt initiate it, then
    // see if we should requeue the scan job
    completeJob = true;

    if (childScanJobCancelled)
    {
        CJobBase* pActiveSerializedJob = m_pAdapter->GetSerializedJobList()->GetActiveSerializedJob();
        // Job was cancelled check if we should request
        if (m_IsCancelled)
        {
            // We cancelled it, so no need to request
            WFCActTrace("[Scan] Child scan job was cancelled by us");
            goto Exit;
        }

        //
        // If we have tried for a bit to get the scan to finish but kept
        // getting cancelled. Dont keep trying
        //
        currentTime = CSystem::get_CurrentTime();
        if ((currentTime - m_ScanOidSubmitTime) > WFC_CONSTANT::ScanRetryLimitTime)
        {
            WFCActInfo("[Scan] Scan OID hasnt managed to complete for a bit, bailing out of scan retry.");
            goto Exit;
        }
        
        //
        // P2P performance can get slowed down if scan retries
        // continue to happen. Heuristic here is that before
        // requeueing, if there is a discover procedure pending, we
        // just end the scan process so OS can move on.
        //
        if ((pActiveSerializedJob != NULL &&
            pActiveSerializedJob->get_JobType() == WiFiJobP2PDiscover) ||
            m_pAdapter->GetSerializedJobList()->FindQueuedSerializedJob(
            NDIS_DEFAULT_PORT_NUMBER,
            WiFiJobP2PDiscover,
            false /* Don't check port number */) != NULL)
        {
            WFCActInfo("[Scan] P2P Discover has been issued/is active, bailing out of scan retry.");
            goto Exit;
        }

        // Queue the scan job again
        completeJob = false;
        WFCActInfo("[Scan] Requeuing Scan Job because it got cancelled earlier than desired");
        OperationStatus = StartScanJob();
    }

Exit:
    if ((OperationStatus != NDIS_STATUS_SUCCESS) ||
        (completeJob == true))
    {
        FinishJob(OperationStatus);
    }

    TraceActExit(OperationStatus);
}

void CScanOidJob::OnJobCancelled()
{
    m_IsCancelled = true;

    WFCActTrace("[Scan] Scan OID job is being cancelled.");

    if (m_pScanJob)
    {
        // Cancel the serialized scan job
        WFCActTrace("[Scan] Cancelling the Child Scan Job");
        m_pAdapter->GetSerializedJobList()->CancelJob(
            m_NdisPortNumber,
            WiFiJobScan,
            m_pScanJob
            );
    }
    else
    {
        NT_ASSERT(FALSE);
    }
}

//==============================================================================================

CBSSListUpdateJob::CBSSListUpdateJob(
    _In_ WFC_ACTIVITY_ID ActivityId) :
    CJobBase(ActivityId),
    m_DeviceCommand(ActivityId)
{
}

CBSSListUpdateJob::~CBSSListUpdateJob()
{
}


NDIS_STATUS CBSSListUpdateJob::Initialize(
    _In_ CAdapter* pAdapter,
    _In_ NDIS_PORT_NUMBER PortNumber,    
    _In_ CBSSListManager *pListManager
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    m_pAdapter = pAdapter;
    m_NdisPortNumber = PortNumber;
    m_pListManager = pListManager;

    // Initialize the base
    ndisStatus = CJobBase::Initialize(
        m_pAdapter->GetEventQueue(),
        m_pAdapter->GetDeviceCommandScheduler(),
        m_pAdapter->GetActiveJobsList(),
        m_pAdapter->GetSerializedJobList(),
        WiFiJobUpdateBSSList,
        false,
        false,
        WiFiPriorityScopePort,
        WiFiSerializedJobPriority_Lowest,
        (IPropertyCacheDirectory *)m_pAdapter,
        (INdisConversion *)m_pAdapter,
        PortNumber
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize base job for BSS list update (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:
    TraceActExit(ndisStatus);
    
    return ndisStatus;
}

void CBSSListUpdateJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    bool bCompleteJob = true;

    TraceActEntry();

    // Check if we need to update the BSS list
    if (m_pListManager->DoesBSSListNeedUpdate(WFC_CONSTANT::IhvCachedEnumBssListMaxAge))
    {
        // Need to get BSS list update
        WFCActTrace("BSS list needs update");
        
        // We need to queue the job to perform the BSS list update        
        ndisStatus = m_DeviceCommand.Initialize(
            get_PortId(),
            WDI_GET_BSS_ENTRY_LIST,
            sizeof(WFC_MESSAGE_METADATA),
            (BYTE *)&m_UpdateCommandData
            );
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCActError("Failed to initialize device command for BSS list. Error = 0x%08x", ndisStatus);
            goto Exit;
        }

        ndisStatus = QueueDeviceCommand(
            &m_DeviceCommand );
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCActError("Failed to queue device command for BSS list. Error = 0x%08x", ndisStatus);
            goto Exit;
        }

        bCompleteJob = false;
    }
    else
    {
        WFCActTrace("BSS list is updated. No need to refresh");
    
        // BSS list is current, we dont need to do anything special
        bCompleteJob = true;
    }

Exit:
    if ((ndisStatus != NDIS_STATUS_SUCCESS) ||
        bCompleteJob)
    {
        // Complete the job
        CompleteJob(ndisStatus);
    }

    TraceActExit(ndisStatus);
}

void CBSSListUpdateJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    NDIS_STATUS commandStatus;
    UNREFERENCED_PARAMETER(SenderObject);

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Device command failed with status %x.", OperationStatus);
        goto exit;
    }

    OperationStatus = CMessageHelper::GetStatusFromCommandResult(
        &m_DeviceCommand,
        &commandStatus );

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't get WABI status from command result 0x%08x", OperationStatus);
        goto exit;
    }  

exit:
    // Tell the list manager that the BSS list update has complete
    m_pListManager->OnBSSListUpdateComplete(OperationStatus);

    CompleteJob(OperationStatus);        
}

void CBSSListUpdateJob::OnJobCancelled()
{
    // Do nothing
}


//==============================================================================================

CEnumBSSListJob::CEnumBSSListJob(
    _In_ WFC_ACTIVITY_ID ActivityId)
    : COidJobBase(ActivityId),
    m_pInformationBuffer(NULL),
    m_InputLength(0),
    m_OutputLength(0),
    m_pBytesWritten(NULL),
    m_pBytesNeeded(NULL),
    m_pBytesRead(NULL),
    m_BSSListUpdateJob(ActivityId)
{
}

CEnumBSSListJob::~CEnumBSSListJob()
{
}

// COidJobBase
NDIS_STATUS CEnumBSSListJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    if (pOidRequest->RequestType == NdisRequestMethod)
    {
        m_pInformationBuffer = METHOD_OID_BUFFER(pOidRequest);
        m_InputLength = METHOD_OID_INPUT_LENGTH(pOidRequest);
        m_OutputLength = METHOD_OID_OUTPUT_LENGTH(pOidRequest);
        m_pBytesWritten = &METHOD_OID_BYTES_WRITTEN(pOidRequest);
        m_pBytesNeeded = &METHOD_OID_BYTES_NEEDED(pOidRequest);
        m_pBytesRead = &METHOD_OID_BYTES_READ(pOidRequest);
    }
    else
    {
        m_pInformationBuffer = QUERY_OID_BUFFER(pOidRequest);
        m_InputLength = QUERY_OID_BUFFER_LENGTH(pOidRequest);
        m_OutputLength = QUERY_OID_BUFFER_LENGTH(pOidRequest);
        m_pBytesWritten = &QUERY_OID_BYTES_WRITTEN(pOidRequest);
        m_pBytesNeeded = &QUERY_OID_BYTES_NEEDED(pOidRequest);
        m_pBytesRead = &QUERY_OID_BYTES_WRITTEN(pOidRequest);
    }

    return NDIS_STATUS_SUCCESS;
}

VOID CEnumBSSListJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);
}

void CEnumBSSListJob::CompleteBSSListEnum(
    _In_ NDIS_STATUS NdisStatus
    )
{
    PDOT11_BYTE_ARRAY pByteArray = (PDOT11_BYTE_ARRAY)m_pInformationBuffer;

    TraceActEntry();
    
    if (NdisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Enum BSS list operation command failed with status %x.", NdisStatus);
        goto Exit;
    }
    
    *m_pBytesWritten = FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer);

    if (get_JobType() == WiFiJobEnumBSSList)
    {
        // Always on STA port only
        
        ULONGLONG oldestAllowedEntry;
        CPort* pPort = m_pAdapter->GetPortFromPortId(get_PortId());
                
        // Normally an AP that is older than WFC_CONSTANT::DefaultBssEntryExpiryTime 
        // will not be reported 
        oldestAllowedEntry = CSystem::get_CurrentTime();
        if (WFC_CONSTANT::DefaultBssEntryExpiryTime <= oldestAllowedEntry)
        {
            oldestAllowedEntry -= WFC_CONSTANT::DefaultBssEntryExpiryTime;
        }

        // However if there is a Miracast session up, we would end up
        // not doing scan (because of media streaming) and if we are also not connected
        // on station, we would end up with no entries to report. So for that case,
        // report entries seen around the last completed scan

        if ((pPort->GetDot11State() == WfcPortDot11StateInit) &&      // Not connected on STA port
            (m_pAdapter->GetAdapterPropertyCache()->GetPropertyULongOrDefault(  // Media streaming is ON
                WfcAdapterPropertyMediaStreamingRefcount, 0) > 0))
        {
            BYTE *goodScanTimeBuffer;
            ULONG scanTimeBufferLength;
            ULONGLONG goodScanStartTime;
            bool bUseScanTime = false;
            WDI_SCAN_TRIGGER currentScanTrigger;

            // Check if we can set the oldest allowed time to be off the time that we 
            // last completed a scan. We do no report old entries if we are in the middle
            // of a forced scan
            if (m_pListManager->IsScanInProgress(&currentScanTrigger) &&
                (currentScanTrigger == WDI_SCAN_TRIGGER_MANUAL))
            {
                // Manual Scan is in progress, dont report stale values
                WFCTrace("[BSSList] WFD in MediaStreaming mode, but manual scan in progress");
            }
            else if ((pPort->GetPropertyCache()->GetPropertyBuffer(
                    WfcPortPropertyGoodScanStartTime,
                    &scanTimeBufferLength,
                    &goodScanTimeBuffer) == NDIS_STATUS_SUCCESS) &&
                 (scanTimeBufferLength == sizeof(ULONGLONG)))
            {
                RtlCopyMemory(&goodScanStartTime, goodScanTimeBuffer, sizeof(ULONGLONG));                

                if ((goodScanStartTime < oldestAllowedEntry) &&
                    (!m_pListManager->IsScanInProgress(&currentScanTrigger)))
                {
                    // Scan was started earlier than oldest allowed entry. 
                    // We may not have any entries to report
                    if (m_pListManager->IsIHVMaintainedExtSTABSSListCache())
                    {
                        // For IHV maintained lists, we do not use the scan complete time if the 
                        // IHV has reported any recent entries (in which case we assume 
                        // that the IHV is doing background scan/slow BSS list aging and
                        // we dont need this special code)
                        if (!m_pListManager->HasNewerBssEntries(oldestAllowedEntry))
                        {
                            bUseScanTime = true;
                        }
                    }
                    else
                    {
                        // Use the scan complete time
                        bUseScanTime = true;
                    }

                    if (bUseScanTime)
                    {
                        WFCInfo("[BSSList] WFD in MediaStreaming mode. Enum BSS list reporting entries from last scan time");

                        // Allow all entries since the last scan
                        oldestAllowedEntry = goodScanStartTime;
                    }
                }
            }
        }
        
        NdisStatus = m_pListManager->GetDot11BSSEntryList(
            oldestAllowedEntry,
            pByteArray,
            m_OutputLength
            );

        *m_pBytesRead = sizeof(DOT11_COUNTRY_OR_REGION_STRING);
    }
    else if (get_JobType() == WiFiJobP2PEnumDeviceList)
    {
        NdisStatus = m_pListManager->GetDot11WfdDeviceEntryList(
            pByteArray,
            m_OutputLength
            );
    }
    else
    {
        NdisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto Exit;
    }

    if ((NdisStatus != NDIS_STATUS_SUCCESS) &&
        (NdisStatus != NDIS_STATUS_BUFFER_OVERFLOW))
    {
        WFCActError("Failed to enumerate BSS list (0x%x)", NdisStatus);
        goto Exit;
    }

    *m_pBytesWritten = pByteArray->uNumOfBytes +
        FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer);

    *m_pBytesNeeded = pByteArray->uTotalNumOfBytes +
        FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer);

Exit:
    CompleteJob(NdisStatus);

    TraceActExit(NdisStatus);
}

// CJobBase
void CEnumBSSListJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();
    if (get_JobType() == WiFiJobEnumBSSList)
    {
        m_pListManager = m_pAdapter->GetBSSListManager(WfcPortTypeExtSTA);
    }
    else if (get_JobType() == WiFiJobP2PEnumDeviceList)
    {
        m_pListManager = m_pAdapter->GetBSSListManager(WfcPortTypeWFDDevice);
    }
    else
    {
        WFCActError("Invalid job type for CEnumBSSListJob class");
        NT_ASSERT(FALSE);
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto Exit;
    }

    // Perform a BSS list refresh
    ndisStatus = m_BSSListUpdateJob.Initialize(
        m_pAdapter,
        m_NdisPortNumber,
        m_pListManager
        );
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Failed to initialize job to update BSS list. Error = 0x%08x", ndisStatus);
        goto Exit;
    }

    ndisStatus = StartChildJob(&m_BSSListUpdateJob);
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Failed to start child job to update BSS list. Error = 0x%08x", ndisStatus);
        goto Exit;
    }

Exit:

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        // Complete the job
        CompleteBSSListEnum(ndisStatus);
    }

    TraceActExit(ndisStatus);
}


void CEnumBSSListJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    UNREFERENCED_PARAMETER(SenderObject);

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("BSS list update job command failed with status %x.", OperationStatus);
    }

    // Complete the job
    CompleteBSSListEnum(OperationStatus);        
}

void CEnumBSSListJob::OnJobCancelled()
{
    // We do nothing yet
}

//==============================================================================================

CFlushBSSListJob::CFlushBSSListJob(
    _In_ WFC_ACTIVITY_ID ActivityId)
    : COidJobBase(ActivityId),
    m_DeviceCommand(ActivityId)
{
}

CFlushBSSListJob::~CFlushBSSListJob()
{
}

// COidJobBase
NDIS_STATUS CFlushBSSListJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    PUINT       pBytesNeeded;
    PUINT       pBytesRead;

    pBytesNeeded = &QUERY_OID_BYTES_NEEDED(pOidRequest);
    *pBytesNeeded  = 0;
    
    pBytesRead = &QUERY_OID_BYTES_WRITTEN(pOidRequest);
    *pBytesRead = 0;

    return NDIS_STATUS_SUCCESS;
}

VOID CFlushBSSListJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);
}

// CJobBase
void CFlushBSSListJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WFC_PORT_TYPE scanCacheSource = WfcPortTypeExtSTA;
    CBSSListManager *pListManager = NULL;
    bool bCompleteJob = false;
     
    TraceActEntry();

    if (get_JobType() == WiFiJobP2PFlushDeviceList)
    {
        scanCacheSource = WfcPortTypeWFDDevice;
    }

    pListManager = m_pAdapter->GetBSSListManager(scanCacheSource);

    // Flush our copy of the BSS list. Dont have to clear them
    // from our internal list though
    pListManager->FlushAllEntries(false);

    // Check if we need to flush the BSS list in the IHV code
    if (pListManager->IsIHVMaintainedExtSTABSSListCache())
    {
        // Need to flush IHV BSS list
        WFCActTrace("BSS list flush needs update");
        
        // We need to queue the job to perform the BSS list update        
        ndisStatus = m_DeviceCommand.Initialize(
            get_PortId(),
            WDI_SET_FLUSH_BSS_ENTRY,
            sizeof(WFC_MESSAGE_METADATA),
            (BYTE *)&m_UpdateCommandData
            );
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCActError("Failed to initialize device command for BSS flush list. Error = 0x%08x", ndisStatus);
            goto Exit;
        }

        ndisStatus = QueueDeviceCommand(
            &m_DeviceCommand );
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCActError("Failed to queue device command for BSS flush list. Error = 0x%08x", ndisStatus);
            goto Exit;
        }

        bCompleteJob = false;
    }
    else
    {
        // We are managing the BSS list
        bCompleteJob = true;
    }

Exit:

    if ((ndisStatus != NDIS_STATUS_SUCCESS) ||
        bCompleteJob)
    {
        // Complete the job
        CompleteJob(ndisStatus);
    }

    TraceActExit(ndisStatus);
}


void CFlushBSSListJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    NDIS_STATUS commandStatus;
    UNREFERENCED_PARAMETER(SenderObject);

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Device command failed with status %x.", OperationStatus);
        goto exit;
    }

    OperationStatus = CMessageHelper::GetStatusFromCommandResult(
        &m_DeviceCommand,
        &commandStatus );

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't get WABI status from command result 0x%08x", OperationStatus);
        goto exit;
    }  

exit:

    // Complete the job
    CompleteJob(OperationStatus);    
}

void CFlushBSSListJob::OnJobCancelled()
{
    // We do nothing yet
}

