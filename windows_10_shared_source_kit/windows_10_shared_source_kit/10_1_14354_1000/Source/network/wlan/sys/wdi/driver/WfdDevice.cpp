#include "Precomp.hpp"
#include "WfdDevice.hpp"
#include "WfdDevice.tmh"

NDIS_STATUS CP2PDiscoverJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_OBJECT_HEADER *pNdisHeader = (NDIS_OBJECT_HEADER *) SET_OID_BUFFER(pOidRequest);
    DWORD i, dwOffset, InformationBufferLength;
    BOOLEAN bForceScanLegacyNetworks;
    ULONG uIEsOffset;
    ULONG uIEsLength;
    ULONG uDeviceFilterListOffset;
    ULONG uNumDeviceFilters;

    TraceActEntry();

    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;
    InformationBufferLength = SET_OID_BUFFER_LENGTH(pOidRequest);

    switch (pNdisHeader->Revision)
    {
        case (DOT11_WFD_DISCOVER_REQUEST_REVISION_1):
        {
            DOT11_WFD_DISCOVER_REQUEST* pDiscoverRequest = (DOT11_WFD_DISCOVER_REQUEST*)SET_OID_BUFFER(pOidRequest);

            pOidRequest->DATA.SET_INFORMATION.BytesRead = sizeof(*pDiscoverRequest); 

            bForceScanLegacyNetworks = pDiscoverRequest->bForceScanLegacyNetworks;
            uIEsOffset = pDiscoverRequest->uIEsOffset;
            uIEsLength = pDiscoverRequest->uIEsLength;
            uNumDeviceFilters = pDiscoverRequest->uNumDeviceFilters;
            uDeviceFilterListOffset = pDiscoverRequest->uDeviceFilterListOffset;

            m_DiscoverTimeoutInMs = pDiscoverRequest->uDiscoverTimeout;
            m_DiscoverParameters.DiscoverMode.DiscoveryType =
                CDot11ToWabiConverter::MapP2PDiscoverType(
                    ((DOT11_WFD_DISCOVER_TYPE) (pDiscoverRequest->DiscoverType & ~dot11_wfd_discover_type_forced)));
            m_DiscoverParameters.DiscoverMode.ForcedDiscovery = 
                (pDiscoverRequest->DiscoverType &
                dot11_wfd_discover_type_forced) ? 1 : 0;
            m_DiscoverParameters.DiscoverMode.ScanType =
                CDot11ToWabiConverter::MapP2PScanType(pDiscoverRequest->ScanType);

            break;
        }

        case (DOT11_WFD_DISCOVER_REQUEST_REVISION_2):
        {
            DOT11_WFD_DISCOVER_REQUEST_V2* pDiscoverRequest =
                (DOT11_WFD_DISCOVER_REQUEST_V2*)SET_OID_BUFFER(pOidRequest);

            pOidRequest->DATA.SET_INFORMATION.BytesRead = sizeof(*pDiscoverRequest); 

            bForceScanLegacyNetworks = pDiscoverRequest->bForceScanLegacyNetworks;
            uIEsOffset = pDiscoverRequest->uIEsOffset;
            uIEsLength = pDiscoverRequest->uIEsLength;
            uNumDeviceFilters = pDiscoverRequest->uNumDeviceFilters;
            uDeviceFilterListOffset = pDiscoverRequest->uDeviceFilterListOffset;

            m_DiscoverTimeoutInMs = pDiscoverRequest->uDiscoverTimeout;
            m_DiscoverParameters.DiscoverMode.DiscoveryType = 
                CDot11ToWabiConverter::MapP2PDiscoverType(
                    ((DOT11_WFD_DISCOVER_TYPE) (pDiscoverRequest->DiscoverType & ~dot11_wfd_discover_type_forced)));
            m_DiscoverParameters.DiscoverMode.ForcedDiscovery = 
                (pDiscoverRequest->DiscoverType &
                dot11_wfd_discover_type_forced) ? 1 : 0;
            m_DiscoverParameters.DiscoverMode.ScanType =
                CDot11ToWabiConverter::MapP2PScanType(pDiscoverRequest->ScanType);

            //
            // Set the V2 only parameters
            //
            m_DiscoverParameters.DiscoverMode.ServiceDiscoveryType =
                CDot11ToWabiConverter::MapServiceDiscoveryType( pDiscoverRequest->ServiceDiscoveryType );

            if (pDiscoverRequest->uNumChannelInfoElements)
            {
                if (InformationBufferLength < pDiscoverRequest->uChannelInfoListOffset +
                                              (pDiscoverRequest->uNumChannelInfoElements *
                                               sizeof(DOT11_WFD_REMOTE_CHANNEL_AVAILABILITY_INFO)))
                {
                    status = NDIS_STATUS_INVALID_DATA;
                    WFCActErrorStatus("Invalid ChannelInfoListLength count = %d in pDiscoverRequest %d < <%d+%d>",
                        pDiscoverRequest->uNumDeviceFilters, InformationBufferLength,
                        pDiscoverRequest->uChannelInfoListOffset,
                        pDiscoverRequest->uNumChannelInfoElements * sizeof(DOT11_WFD_REMOTE_CHANNEL_AVAILABILITY_INFO));

                    goto exit;
                }

                status = m_DiscoverParameters.DiscoveryChannelSettings.AllocateElements(
                    pDiscoverRequest->uNumChannelInfoElements, 0);
                if (status != NDIS_STATUS_SUCCESS)
                {
                    WFCActError("Failed to allocate memory for %d DiscoveryChannelSettings. Error = 0x%08x",
                        pDiscoverRequest->uNumChannelInfoElements, status);
                    goto exit;
                }

                m_DiscoverParameters.Optional.DiscoveryChannelSettings_IsPresent = 1;
                dwOffset = pDiscoverRequest->uChannelInfoListOffset;
                for (i=0; i<pDiscoverRequest->uNumChannelInfoElements; i++)
                {
                    status = CDot11ToWabiConverter::MapWFDChannelInfo(
                        (PBYTE) pDiscoverRequest,
                        InformationBufferLength,
                        &dwOffset,
                        (PDOT11_WFD_REMOTE_CHANNEL_AVAILABILITY_INFO) (((PBYTE) pDiscoverRequest) + dwOffset),
                        &m_DiscoverParameters.DiscoveryChannelSettings.pElements[i]);
                    if (status != NDIS_STATUS_SUCCESS)
                    {
                        WFCActError("MapWFDChannelInfo FAILed. Error = 0x%08x", status);
                        goto exit;
                    }
                }
            }

            if (pDiscoverRequest->uNumServices)
            {
                if (m_DiscoverParameters.DiscoverMode.ServiceDiscoveryType == WDI_P2P_SERVICE_DISCOVERY_TYPE_SERVICE_NAME_ONLY)
                {
                    C_ASSERT(sizeof(WDI_P2P_SERVICE_NAME_HASH_CONTAINER) == sizeof(DOT11_WFD_SERVICE_HASH));
                    if ((pDiscoverRequest->uServiceHashListLength != (pDiscoverRequest->uNumServices * sizeof(DOT11_WFD_SERVICE_HASH))) ||
                        (InformationBufferLength < (pDiscoverRequest->uServiceHashListOffset + pDiscoverRequest->uServiceHashListLength)))
                    {
                        status = NDIS_STATUS_INVALID_DATA;
                        WFCActErrorStatus("Invalid ServiceHashListLength count = %d->%d in pDiscoverRequest %d < %d+%d",
                            pDiscoverRequest->uNumServices, (pDiscoverRequest->uNumServices*sizeof(DOT11_WFD_SERVICE_HASH)),
                            InformationBufferLength, 
                            pDiscoverRequest->uServiceHashListOffset, pDiscoverRequest->uServiceHashListLength);

                        goto exit;
                    }

                    m_DiscoverParameters.ServiceNameHash.SimpleAssign(
                        (WDI_P2P_SERVICE_NAME_HASH_CONTAINER*) (((PBYTE) pDiscoverRequest) +
                                                                pDiscoverRequest->uServiceHashListOffset),
                        pDiscoverRequest->uNumServices);

                    m_DiscoverParameters.Optional.ServiceNameHash_IsPresent = 1;
                }
                else if (m_DiscoverParameters.DiscoverMode.ServiceDiscoveryType == WDI_P2P_SERVICE_DISCOVERY_TYPE_SERVICE_INFORMATION)
                {
                    if ((pDiscoverRequest->uServiceInfoListLength != (pDiscoverRequest->uNumServices * sizeof(DOT11_WFD_SERVICE_INFORMATION_DISCOVERY_ENTRY))) ||
                        (InformationBufferLength < (pDiscoverRequest->uServiceInfoListOffset + pDiscoverRequest->uServiceInfoListLength)) ||
                        (pDiscoverRequest->uNumServices == 0))
                    {
                        status = NDIS_STATUS_INVALID_DATA;
                        WFCActErrorStatus("Invalid ServiceInfoListLength count = %d->%d in pDiscoverRequest %d < %d+%d, uNumServices = %u",
                            pDiscoverRequest->uNumServices, (pDiscoverRequest->uNumServices*sizeof(DOT11_WFD_SERVICE_INFORMATION_DISCOVERY_ENTRY)),
                            InformationBufferLength,
                            pDiscoverRequest->uServiceInfoListOffset, pDiscoverRequest->uServiceInfoListLength,
                            pDiscoverRequest->uNumServices);

                        goto exit;
                    }

                    status = m_DiscoverParameters.ServiceInformationDiscoveryEntry.AllocateElements(
                        pDiscoverRequest->uNumServices,
                        0);
                    if (status != NDIS_STATUS_SUCCESS)
                    {
                        WFCActErrorStatus("Couldn't allocate %d ServiceInformationDiscoveryEntry elements",
                            pDiscoverRequest->uNumServices);
                        goto exit;
                    }

                    m_DiscoverParameters.Optional.ServiceInformationDiscoveryEntry_IsPresent = 1;

                    DOT11_WFD_SERVICE_INFORMATION_DISCOVERY_ENTRY *pDot11SvcEntry = (PDOT11_WFD_SERVICE_INFORMATION_DISCOVERY_ENTRY)(((PBYTE)pDiscoverRequest) + pDiscoverRequest->uServiceInfoListOffset);
                    for (i = 0; i < pDiscoverRequest->uNumServices; i++)
                    {
                        WDI_P2P_SERVICE_INFORMATION_DISCOVERY_ENTRY_CONTAINER *pWdiSvcContainer = &m_DiscoverParameters.ServiceInformationDiscoveryEntry.pElements[i];

                        if (((pDot11SvcEntry->uServiceNameOffset + pDot11SvcEntry->uServiceNameLength) > InformationBufferLength) ||
                            ((pDot11SvcEntry->uServiceInformationQueryOffset + pDot11SvcEntry->uServiceInformationQueryLength) > InformationBufferLength))
                        {
                            WFCActErrorStatus("[%d] BAD Entry NameOffset+Length=<%d+%d>, InfoOffset+Length=<%d+%d>, MaxLength=<%d>",
                                i, pDot11SvcEntry->uServiceNameOffset, pDot11SvcEntry->uServiceNameLength,
                                pDot11SvcEntry->uServiceInformationQueryOffset, pDot11SvcEntry->uServiceInformationQueryLength,
                                InformationBufferLength);
                            goto exit;
                        }

                        RtlCopyMemory(&pWdiSvcContainer->ServiceNameHash,
                            &pDot11SvcEntry->ServiceHash,
                            sizeof(pWdiSvcContainer->ServiceNameHash));
                        pWdiSvcContainer->ServiceName.ElementCount = pDot11SvcEntry->uServiceNameLength;
                        pWdiSvcContainer->ServiceName.pElements = ((PBYTE)pDiscoverRequest) + pDot11SvcEntry->uServiceNameOffset;

                        if (pDot11SvcEntry->uServiceInformationQueryLength)
                        {
                            status = pWdiSvcContainer->ServiceInformation.SimpleSet(
                                ((PBYTE)pDiscoverRequest) + pDot11SvcEntry->uServiceInformationQueryOffset,
                                pDot11SvcEntry->uServiceInformationQueryLength, 
                                NULL);
                            if (status != NDIS_STATUS_SUCCESS)
                            {
                                WFCActErrorStatus("[%d] Could not allocate %d bytes for ServiceInformation",
                                    i, pDot11SvcEntry->uServiceInformationQueryLength);
                                goto exit;
                            }

                            pWdiSvcContainer->Optional.ServiceInformation_IsPresent = TRUE;

                            pWdiSvcContainer->ServiceUpdateIndicator = pDot11SvcEntry->uUpdateIndicator;
                            pWdiSvcContainer->Optional.ServiceUpdateIndicator_IsPresent = TRUE;

                            pWdiSvcContainer->ServiceTransactionId = pDot11SvcEntry->ucTransactionId;
                            pWdiSvcContainer->Optional.ServiceTransactionId_IsPresent = TRUE;
                        }

                        pDot11SvcEntry++;
                    }
                }
            }

            break;
        }

        default:
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("Invalid Discover Request version %d.", pNdisHeader->Revision);
            goto exit;
        }
    }

    m_DiscoverParameters.DiscoverMode.ScanRepeatCount = 1;
    m_DiscoverParameters.DwellTime.ActiveChannelDwellTime = WFC_CONSTANT::DefaultWFDActiveChannelDwellTime;
    m_DiscoverParameters.DwellTime.PassiveChannelDwellTime = WFC_CONSTANT::DefaultWFDPassiveChannelDwellTime;
    m_DiscoverParameters.DwellTime.MaximumScanTime = m_DiscoverTimeoutInMs;
    m_RemainingTimeoutInMs = m_DiscoverTimeoutInMs;

    //
    // OS Expects a fast discovery. Tweak parameters accordingly.
    //
    if ( m_DiscoverTimeoutInMs < 8000 )
    {
        m_DiscoverParameters.DiscoverMode.TimeBetweenScans = 50;
        m_DiscoverParameters.DiscoverMode.ScanRepeatCount = 0;
    }

    m_DiscoverParameters.Optional.DiscoveryChannelSettings_IsPresent = 0;

    m_DiscoverParameters.Optional.SSIDList_IsPresent = 1;
    m_DiscoverParameters.SSIDList.pElements = (WDI_SSID*)&m_DefaultDiscoverSSIDs;
    m_DiscoverParameters.SSIDList.ElementCount = bForceScanLegacyNetworks ? 2 : 1;

    //
    // Additional IEs
    //
    if ( uIEsLength > 0 )
    {
        ULONG bufferLengthCheck;

        if ( !NT_SUCCESS(
                 RtlULongAdd(
                     uIEsLength,
                     uIEsOffset,
                     &bufferLengthCheck ) ) ||
             bufferLengthCheck > SET_OID_BUFFER_LENGTH(pOidRequest) )
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("Bad IEs information in buffer provided from OS.");
            goto exit;
        }

        status = m_DiscoverParameters.VendorIEs.SimpleSet(
            ((PBYTE)pNdisHeader) + uIEsOffset,
            uIEsLength,
            NULL);
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Could not allocate %d bytes for Vendor IEs.", uIEsLength);
            goto exit;
        }

        m_DiscoverParameters.Optional.VendorIEs_IsPresent = 1;
    }

    if ( uNumDeviceFilters > 0 )
    {
        ULONG bufferLengthCheck;

        if ( !NT_SUCCESS( RtlULongMult(
                 uNumDeviceFilters,
                 sizeof(DOT11_WFD_DISCOVER_DEVICE_FILTER),
                 &bufferLengthCheck ) ) ||
             !NT_SUCCESS( RtlULongAdd(
                 bufferLengthCheck,
                 uDeviceFilterListOffset,
                 &bufferLengthCheck ) ) ||
             bufferLengthCheck > SET_OID_BUFFER_LENGTH(pOidRequest) )
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("Bad IEs information in buffer provided from OS.");
            goto exit;
        }

        //
        // Targeted scan is 'repeat until cancelled', a timer will be
        // set in to cancel discover at the appropriate time.
        //
        m_DiscoverParameters.DiscoverMode.ScanRepeatCount = 0;
        m_DiscoverParameters.DiscoverMode.TimeBetweenScans = 50;

        //
        // Tweak dwell times for targeted scan
        //
        m_DiscoverParameters.DwellTime.ActiveChannelDwellTime = WFC_CONSTANT::DefaultWFDActiveChannelDwellTime;
        m_DiscoverParameters.DwellTime.PassiveChannelDwellTime = WFC_CONSTANT::DefaultWFDPassiveChannelDwellTime;
        m_DiscoverParameters.DwellTime.MaximumScanTime = m_DiscoverTimeoutInMs;

        m_DeviceFilterCount = uNumDeviceFilters;

        m_pDeviceFilterList = new DOT11_WFD_DISCOVER_DEVICE_FILTER[m_DeviceFilterCount];

        if ( m_pDeviceFilterList == NULL )
        {
            status = NDIS_STATUS_RESOURCES;
            WFCActErrorStatus("Couldn't allocate copy of device filter list.");
            goto exit;
        }

        RtlCopyMemory(
            m_pDeviceFilterList,
            (BYTE*)pNdisHeader + uDeviceFilterListOffset,
            m_DeviceFilterCount * sizeof(*m_pDeviceFilterList) );

        // OACR can't seem to figure this out.
        __analysis_assume( m_pDeviceFilterList->GroupSSID.uSSIDLength <= 32 );

        {
            //
            // If OS is issuing a "DIRECT-" discover, overwrite scan
            // type to social channel only. OS does not do this on its
            // own.
            //
            DOT11_SSID wildcardSSID = {7,"DIRECT-"};
            if ( m_DeviceFilterCount == 1 &&
                 m_pDeviceFilterList[0].GroupSSID.uSSIDLength == wildcardSSID.uSSIDLength &&
                 memcmp(
                     m_pDeviceFilterList[0].GroupSSID.ucSSID,
                     wildcardSSID.ucSSID,
                     wildcardSSID.uSSIDLength ) == 0 )
            {
                WFCActInfo("Overwriting DiscoveryType to social channels only since DIRECT- directed discover was specified.");
                m_DiscoverParameters.DiscoverMode.DiscoveryType = WDI_P2P_DISCOVER_TYPE_SCAN_SOCIAL_CHANNELS;
                m_DiscoverParameters.DiscoverMode.ScanType = WDI_P2P_SCAN_TYPE_ACTIVE;
            }
        }
    }

exit:
    TraceActExit(status);
    return status;
}

VOID CP2PDiscoverJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS ndisStatus
    )
{
    TraceActEntry();

    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        ndisStatus );

    TraceActExit(ndisStatus);
}

void CP2PDiscoverJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS ndisStatus )
{
    NDIS_STATUS status = ndisStatus;
    TraceEntry();

    if ( ndisStatus == NDIS_STATUS_SUCCESS &&
         m_pIndicationBuffer == NULL )
    {
        WFCActErrorStatus("Claimed success but indication buffer was NULL.");
        status = NDIS_STATUS_FAILURE;
    }

    if ( status != NDIS_STATUS_SUCCESS)
    {
        DOT11_WFD_DISCOVER_COMPLETE_PARAMETERS failIndication = {0};

        ASSIGN_NDIS_OBJECT_HEADER(
            failIndication.Header, 
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_WFD_DISCOVER_COMPLETE_PARAMETERS_REVISION_1,
            sizeof(failIndication));
        
        failIndication.Status = status;

        m_pAdapter->IndicateStatus(
            m_NdisPortNumber,
            NDIS_STATUS_DOT11_WFD_DISCOVER_COMPLETE,
            m_OidRequestId,
            &failIndication,
            sizeof(failIndication) );       
    }
    else
    {
        m_pAdapter->IndicateStatus(
            m_NdisPortNumber,
            NDIS_STATUS_DOT11_WFD_DISCOVER_COMPLETE,
            m_OidRequestId,
            m_pIndicationBuffer,
            m_IndicationBufferLength );       
    }

    TraceExit(status);
}


NDIS_STATUS CP2PDiscoverJob::TriggerDiscover()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG DiscoverTimeoutInMs = m_RemainingTimeoutInMs;

    TraceActEntry();

    if ( m_pDiscoverTlvBuffer != NULL )
    {
        delete [] m_pDiscoverTlvBuffer;
        m_pDiscoverTlvBuffer = NULL;
        m_DiscoverTlvBufferLength = 0;
    }

    //
    // If AllowFallbackDiscover was set earlier, it means targeted discovery has been tried and failed,
    // and we are now entering FallbackDiscover, hence should not set channel hint
    //
    if ( m_ChannelHintAvailable &&
         !m_AllowFallbackDiscover )
    {
        WFCActInfo(
            "Adding channel hint to internal P2P discover BandID = 0x%x, Channel Number = 0x%x.",
            m_ChannelHintBandInfo.BandID,
            m_ChannelHintChannelNumber );

        m_DiscoverParameters.DiscoverMode.DiscoveryType = WDI_P2P_DISCOVER_TYPE_SCAN_ONLY;

        m_DiscoverParameters.Optional.DiscoveryChannelSettings_IsPresent = 1;

        RtlZeroMemory (&m_DiscoveryChannelSettings, sizeof(m_DiscoveryChannelSettings));
        m_DiscoveryChannelSettings.BandChannelList.pElements = &m_ChannelHintBandInfo;
        m_DiscoveryChannelSettings.BandChannelList.ElementCount = 1;

        m_DiscoverParameters.DiscoveryChannelSettings.pElements = &m_DiscoveryChannelSettings;
        m_DiscoverParameters.DiscoveryChannelSettings.ElementCount = 1;

        m_DiscoverParameters.DiscoverMode.ScanType = WDI_P2P_SCAN_TYPE_AUTO;

        m_ChannelHintBandInfo.ChannelList.pElements = &m_ChannelHintChannelNumber;
        m_ChannelHintBandInfo.ChannelList.ElementCount = 1;

        m_ChannelHintAvailable = false;

        if ( WFC_CONSTANT::TargetedDiscoveryTimeoutMs < (m_RemainingTimeoutInMs/3) )
        {
            m_AllowFallbackDiscover = true;
            DiscoverTimeoutInMs = WFC_CONSTANT::TargetedDiscoveryTimeoutMs;
        }
    }
    else
    {
        m_DiscoverParameters.Optional.DiscoveryChannelSettings_IsPresent = 0;
    }
    m_RemainingTimeoutInMs -= DiscoverTimeoutInMs;

    if ( m_RevertListenState )
    {
        WDI_P2P_LISTEN_STATE latestListenState = WDI_P2P_LISTEN_STATE_OFF;

        //
        // If the device is in the Listen state, the driver needs to include the information in the probe frames
        //
        if ( m_pAdapter->GetAdapterPropertyCache()->GetPropertyT(
                WfcAdapterPropertyCurrentListenState,
                &latestListenState ) == NDIS_STATUS_SUCCESS )
        {
            m_DiscoverParameters.bIncludeListenChannel = (WDI_P2P_LISTEN_STATE_OFF != latestListenState);
            m_DiscoverParameters.Optional.bIncludeListenChannel_IsPresent = 1;
        }
    }

    if ( m_TimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID )
    {
        m_pEventQueue->DeregisterTimeoutCallback(
            m_TimerToken );
        m_TimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    status = GenerateWdiTaskP2pDiscover(
        &m_DiscoverParameters,
        sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &m_DiscoverTlvBufferLength,
        &m_pDiscoverTlvBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    WFCActInfo("registering callback timer.");

    status = m_pEventQueue->RegisterTimeoutCallbackWithLevelAndReuse(
        get_ActivityId(),
        this,
        DiscoverTimeoutInMs,
        NULL,
        WDF_EXECUTION_LEVEL::WdfExecutionLevelPassive,
        false,
        NULL,
        &m_TimerToken );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't register for targeted discover callback.");
        goto exit;
    }

    m_DiscoverStartTime = CSystem::get_CurrentTime();

    //
    // Hang detection should be longer than the maximum expected discover time
    //
    ULONG taskTimeout = DiscoverTimeoutInMs + 2000;

    WFCActInfo ("***** Starting Discover task: Timeout=<%d>, m_DiscoverStartTime=<%I64d>, Maxtime=<%d .. %d> *****\n",
        DiscoverTimeoutInMs, m_DiscoverStartTime, m_DiscoverParameters.DwellTime.MaximumScanTime, m_DiscoverTimeoutInMs);

    status = InitializeAndStartTask(
        &m_DiscoverTask,
        WDI_TASK_P2P_DISCOVER,
        m_DiscoverTlvBufferLength,
        m_pDiscoverTlvBuffer,
        m_pAdapter->GetNotificationManager(),
        &taskTimeout );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Failed to initialize and start discover task.");
        goto exit;
    }

exit:
    TraceActExit(status);
    return status;
}


void CP2PDiscoverJob::GetDiscoverySettingsFromBitmask(
    _In_ UCHAR ucBitmask,
    _Out_ bool *pMatchDeviceAddress,
    _Out_ bool *pMatchSSID,
    _Out_ bool *pMatchAll
    )
{
    bool matchDeviceAddress = true;
    bool matchSSID = true;
    bool matchAll = true;

    if ((ucBitmask & DISCOVERY_FILTER_BITMASK_ANY) == DISCOVERY_FILTER_BITMASK_ANY)
    {
        //
        // From MSDN: The local P2P device needs only to discover the P2P device or any group owner
        // hosted by the P2P device.
        //
        matchSSID = false;
        matchDeviceAddress = true;
        matchAll = true;
    }
    else if (ucBitmask == DISCOVERY_FILTER_BITMASK_GO)
    {
        //
        // From MSDN: The local P2P device discovers a group with P2P group ID matching
        // the specified DeviceID and GroupSSID.
        //
        matchSSID = true;
        matchDeviceAddress = false;     // BugBug - P2P Discovery fails during connect if set to true
        matchAll = true;
    }
    else if (ucBitmask == DISCOVERY_FILTER_BITMASK_DEVICE)
    {
        //
        // From MSDN: The local P2P device discovers the P2P device with the desired DeviceID.
        //
        matchSSID = false;
        matchDeviceAddress = true;
        matchAll = true;
    }
    else if (ucBitmask == (DISCOVERY_FILTER_BITMASK_DEVICE | DISCOVERY_FILTER_BITMASK_GO ))
    {
        //
        // From MSDN: The local P2P device should discover only one instance of the P2P device
        // - either the P2P device or a GO with the desired P2P DeviceID and GroupSSID.
        //
        matchSSID = true;
        matchDeviceAddress = true;
        matchAll = false;
    }
    else
    {
        NT_ASSERT(FALSE);
    }

    *pMatchDeviceAddress = matchDeviceAddress;
    *pMatchSSID = matchSSID;
    *pMatchAll = matchAll;
}

    
void CP2PDiscoverJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CBSSListManager* pDiscoveryCache = m_pAdapter->GetBSSListManager(WfcPortTypeWFDDevice);
    CBSSEntry* pDeviceInfo;
    WDI_P2P_LISTEN_STATE currentListenState;

    TraceActEntry();

    NotifyStarted(NDIS_STATUS_INDICATION_REQUIRED);

    status = m_pAdapter->GetNotificationManager()->RegisterForNotifications(
        WiFiIndicationBssEntryList,
        WFC_COMMAND_TOKEN_INVALID,
        get_PortId(),
        (INotifyDeviceIndicationCallback*)this,
        &m_ScanResultRegistration );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't register for scan result notifications.");
        goto exit;
    }

    WFCActInfo("IsInternal=%x, ChannelHint=%x, FilterCount=%d\n",
        m_IsInternalDiscover, m_ChannelHintAvailable, m_DeviceFilterCount);

    if ( !m_ChannelHintAvailable &&
         m_DeviceFilterCount == 1 )
    {
        bool matchDeviceAddress = true;
        bool matchSSID = true;
        bool matchAll = true;

        WFCActInfo("Checking for channel hint availability. Single device filter provided [ %!MACADDR! ] and channel hint doesn't already exist.",
            m_pDeviceFilterList[0].DeviceID);

        GetDiscoverySettingsFromBitmask( m_pDeviceFilterList->ucBitmask, &matchDeviceAddress, &matchSSID, &matchAll );

        RtlCopyMemory(
            &m_TargetGroupID.DeviceAddress,
            &m_pDeviceFilterList[0].DeviceID,
            sizeof(m_TargetGroupID.DeviceAddress) );

        RtlCopyMemory(
            &m_TargetGroupID.SSID,
            &m_pDeviceFilterList[0].GroupSSID,
            sizeof(m_TargetGroupID.SSID) );

        //
        // Check for channel hints
        //
        pDeviceInfo = pDiscoveryCache->FindBSSEntryByGroupID(
            m_TargetGroupID,
            matchDeviceAddress,
            matchSSID,
            matchAll);

        if ( pDeviceInfo != NULL )
        {
            if ( pDeviceInfo->GetChannelHint(
                     &m_ChannelHintBandInfo.BandID,
                     &m_ChannelHintChannelNumber ) )
            {
                m_ChannelHintAvailable = true;
            }
        }
    }

    //
    // If there are any filters, make sure to clear out any existing
    // cached data so we only get fresh information.
    //
    for(ULONG i = 0; i < m_DeviceFilterCount; ++i)
    {
        bool matchDeviceAddress = true;
        bool matchSSID = true;
        DOT11_WFD_GROUP_ID targetGroupID = { {0}, m_pDeviceFilterList[i].GroupSSID };

        //
        // If the SSID length is 0, do not match the SSID
        //
        if ( targetGroupID.SSID.uSSIDLength == 0 )
        {
            matchSSID = false;
        }
        //
        // If the SSID was provided, and is not the Wildcard SSID, match the SSID only
        //
        else if ( targetGroupID.SSID.uSSIDLength != 7 ||
                  RtlCompareMemory( targetGroupID.SSID.ucSSID, "DIRECT-", 7 ) != 7 )
        {
            matchDeviceAddress = false;
        }

        WFCInfo("\t[%d] Filter Bitmask= 0x%x, GroupSSID= %!DOT11SSID!, DeviceID= %!MACADDR!, Match SSID:Device= %x:%x",
            i, m_pDeviceFilterList[i].ucBitmask, &m_pDeviceFilterList[i].GroupSSID, m_pDeviceFilterList[i].DeviceID,
            matchSSID, matchDeviceAddress);

        RtlCopyMemory(
            &targetGroupID.DeviceAddress,
            &m_pDeviceFilterList[i].DeviceID,
            sizeof(targetGroupID.DeviceAddress) );

        CBSSEntry* pExistingDeviceInfo = m_pAdapter->GetBSSListManager(
            WfcPortTypeWFDDevice )->FindBSSEntryByGroupID(
                targetGroupID,
                matchDeviceAddress,
                matchSSID );

        if ( pExistingDeviceInfo != NULL )
        {
            // Don't block, just set as not visible
            pExistingDeviceInfo->SetVisibility(false);
        }
    }

    status = m_pAdapter->GetAdapterPropertyCache()->GetPropertyT(
        WfcAdapterPropertyCurrentListenState,
        &currentListenState );

    if ( status == NDIS_STATUS_SUCCESS &&
         currentListenState != WDI_P2P_LISTEN_STATE_OFF &&
         !m_IsInternalDiscover )
    {
        status = m_ListenStateJob.Initialize(
            m_pAdapter,
            get_NdisPortNumber(),
            WDI_P2P_LISTEN_STATE_OFF );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't initialize listen state toggle job.");
            goto exit;
        }

        status = StartChildJob(
            &m_ListenStateJob );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't start listen state toggle job.");
            goto exit;
        }

        WFCActInfo("Waiting to turn off listen state.");

        m_DiscoverState = DiscoverStateWaitingForToggleListenStateCompletion;
    }
    else
    {
        status = TriggerDiscover();

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't trigger discover attempt.");
            goto exit;
        }

        m_DiscoverState = DiscoverStateWaitingForDiscoverCompletion;
    }

exit:
    if ( status != NDIS_STATUS_SUCCESS )
    {
        CompleteJob(status);
    }

    TraceActExit(status);
}

NDIS_STATUS CP2PDiscoverJob::PrepareCompletionIndication()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CBSSListManager* pDiscoveryStore = m_pAdapter->GetBSSListManager(WfcPortTypeWFDDevice);
    BYTE* pIndicationBuffer = NULL;
    ULONG indicationBufferSize = sizeof(DOT11_WFD_DISCOVER_COMPLETE_PARAMETERS);
    DOT11_WFD_DISCOVER_COMPLETE_PARAMETERS staticInfo = {0};
    DOT11_WFD_DISCOVER_COMPLETE_PARAMETERS* pIndication;

    TraceActEntry();

    //
    // completing with cached results
    //
    status = pDiscoveryStore->GetWfdDiscoverRequestResultList(
        &staticInfo,
        indicationBufferSize );

    if ( status != NDIS_STATUS_SUCCESS && 
         status != NDIS_STATUS_BUFFER_OVERFLOW )
    {
        WFCActErrorStatus("Couldn't get discovery request result list.");
        goto exit;
    }

    indicationBufferSize += staticInfo.uListLength;

    pIndicationBuffer = new BYTE[indicationBufferSize];
    pIndication = (DOT11_WFD_DISCOVER_COMPLETE_PARAMETERS*)pIndicationBuffer;
    if ( pIndicationBuffer == NULL )
    {
        WFCActErrorStatus("Couldn't allocate output indication buffer size");
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    //
    // completing with cached results
    //
    status = pDiscoveryStore->GetWfdDiscoverRequestResultList(
        pIndication,
        indicationBufferSize );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't get discover result list after allocating memory.");
        NT_ASSERT(FALSE);
        goto exit;
    }

    WFCActInfo("CP2PDiscoverJob::PrepareCompletionIndication: Num:TotalEntries=<%d:%d>, ListOffset:Length=%d:%d>",
        pIndication->uNumOfEntries, pIndication->uTotalNumOfEntries,
        pIndication->uListOffset, pIndication->uListLength);

    m_pIndicationBuffer = pIndicationBuffer;
    m_IndicationBufferLength = indicationBufferSize;
    pIndicationBuffer = NULL;

exit:
    if ( pIndicationBuffer != NULL )
    {
        delete [] pIndicationBuffer;
    }

    TraceActExit(status);
    return status;
}

void CP2PDiscoverJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    bool TriggerJobCompletion = true;

    UNREFERENCED_PARAMETER(SenderObject);

    TraceActEntry();

    NT_ASSERT(
        SenderObject == &m_DiscoverTask ||
        SenderObject == &m_ListenStateJob );

    switch( m_DiscoverState )
    {
    case DiscoverStateWaitingForDiscoverCompletion:

        if ( m_TriggerFallbackDiscover )
        {
            TriggerJobCompletion = false;
            m_TriggerFallbackDiscover = false;          // Set to false so that we don't loop

            WFCActInfo(
                "Discover completed with 0x%x, ignoring error and triggering fallback discover attempt.",
                OperationStatus );

            m_DiscoverParameters.DiscoverMode.ScanRepeatCount = 1;
            m_DiscoverParameters.Optional.DiscoveryChannelSettings_IsPresent = 0;

            status = TriggerDiscover();

            if ( status != NDIS_STATUS_SUCCESS )
            {
                WFCActErrorStatus(
                    "Couldn't trigger fallback discover attempt.");
                goto exit;
            }

            m_DiscoverState = DiscoverStateWaitingForFallbackDiscoverCompletion;
            goto exit;
        }

        break;

    case DiscoverStateWaitingForToggleListenStateCompletion:

        WFCActInfo("Listen state turned off. Triggering discovery.");
        m_RevertListenState = true;

        status = TriggerDiscover();

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't trigger discover.");
            goto exit;
        }

        m_DiscoverState = DiscoverStateWaitingForDiscoverCompletion;
        TriggerJobCompletion = false;
        goto exit;

    case DiscoverStateWaitingForFallbackDiscoverCompletion:
        WFCActInfo("Fallback discover completed.");
        break;

    case DiscoverStateWaitingForRevertListenStateCompletion:
        WFCActInfo("Listen state reverted. Completing Job.");
        goto exit;
    }

    if ( m_IsInternalCancel )
    {
        WFCActInfo(
            "Discover completed with 0x%x, ignoring error code because this was an internal cancellation, and completing with cached results.",
            OperationStatus );
        status = NDIS_STATUS_SUCCESS;
        goto exit;
    }

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        status = OperationStatus;
        WFCActErrorStatus("Discovery failed with 0x%x", 
                          OperationStatus);
        goto exit;
    }
    
    {
        NDIS_STATUS wifiStatus;
    
        status = CMessageHelper::GetStatusFromTaskOutput(
            &m_DiscoverTask,
            &wifiStatus );
    
        if ( status == NDIS_STATUS_SUCCESS )
        {
            status = wifiStatus;
        }
    }

    WFCActInfo(
        "Discover completed with 0x%x",
        status ); 

exit:

    if ( m_RevertListenState &&
         ( status != NDIS_STATUS_SUCCESS ||
           TriggerJobCompletion ) )
    {
        WDI_P2P_LISTEN_STATE latestListenState = WDI_P2P_LISTEN_STATE_OFF;

        //
        // This code path gets one try to reset listen state to its
        // previous value.
        //
        m_RevertListenState = false;

        if ( m_pAdapter->GetAdapterPropertyCache()->GetPropertyT(
                WfcAdapterPropertyCurrentListenState,
                &latestListenState ) == NDIS_STATUS_SUCCESS &&
             m_ListenStateJob.Initialize(
                 m_pAdapter,
                 get_NdisPortNumber(),
                 latestListenState ) == NDIS_STATUS_SUCCESS &&
             StartChildJob(
                 &m_ListenStateJob ) == NDIS_STATUS_SUCCESS )
        {
            status = NDIS_STATUS_SUCCESS;
            TriggerJobCompletion = false;
            WFCActInfo("Reverting listen state before completing job.");
            m_DiscoverState = DiscoverStateWaitingForRevertListenStateCompletion;
        }
        else
        {
            //
            // If this assert is loud it can be removed. Asserting
            // here because it's a source of potential bugs (losing
            // listen state setting), but in strange failure/shutdown
            // paths it may be OK.
            //
            WFCActInfo("Couldn't revert listen state to stored value. Need to investigate why.");
            NT_ASSERT(FALSE);
            status = NDIS_STATUS_FAILURE;
        }
    }

    if ( status != NDIS_STATUS_SUCCESS ||
         TriggerJobCompletion )
    {
        WFCActInfo("Completing discover job with status 0x%x", status);
        m_DiscoverState = DiscoverStateDone;

        if ( m_ScanResultRegistration != WFC_REGISTRATION_TOKEN_INVALID )
        {
            m_pAdapter->GetNotificationManager()->DeregisterNotificationHandler(
                m_ScanResultRegistration );
            m_ScanResultRegistration = WFC_REGISTRATION_TOKEN_INVALID;
        }

        PrepareCompletionIndication();

        CompleteJob(status);
    }

    TraceActExit(status);
}

void CP2PDiscoverJob::OnJobCancelled()
{
    TraceActEntry();

    if ( m_IsCancelled )
    {
        goto exit;
    }

    m_IsCancelled = true;

    if ( m_DiscoverState != DiscoverStateInit &&
         m_DiscoverState != DiscoverStateDone )
    {
        CancelActivePendingCommand();
    }

exit:
    TraceActExit(NDIS_STATUS_SUCCESS);
}

bool CP2PDiscoverJob::IsDeviceFilterSatisfied()
{
    CBSSListManager* pDiscoveryCache = m_pAdapter->GetBSSListManager(WfcPortTypeWFDDevice);
    bool bSatisfied = true;

    TraceActEntry();

    if ( m_DeviceFilterCount == 0 )
    {
        // If there's no filter we don't want to break out early from the discovery.
        bSatisfied = false;
        goto exit;
    }

    for( ULONG i = 0; i < m_DeviceFilterCount; ++i )
    {
        DOT11_WFD_DISCOVER_DEVICE_FILTER* pCurrentFilter = &m_pDeviceFilterList[i];
        DOT11_WFD_GROUP_ID targetGroupId = { {0}, { 7, "DIRECT-" } };
        CBSSEntry* pDeviceInfo;
        bool matchDeviceAddress = true;
        bool matchSSID = true;
        bool matchAll = true;

        GetDiscoverySettingsFromBitmask( pCurrentFilter->ucBitmask, &matchDeviceAddress, &matchSSID, &matchAll );

        RtlCopyMemory(
            &targetGroupId.DeviceAddress,
            pCurrentFilter->DeviceID,
            sizeof(targetGroupId.DeviceAddress) );

        if ( pCurrentFilter->ucBitmask & DISCOVERY_FILTER_BITMASK_GO )
        {
            RtlCopyMemory(
                &targetGroupId.SSID,
                &pCurrentFilter->GroupSSID,
                sizeof(targetGroupId.SSID) );
        }

        pDeviceInfo = pDiscoveryCache->FindBSSEntryByGroupID(
            targetGroupId,
            matchDeviceAddress,
            matchSSID,
            matchAll);

        if ( pDeviceInfo != NULL )
        {
            //
            // Device Info was found. Does it have any other required settings?
            //
            if ( !pDeviceInfo->IsWFDFilledIn(m_AllowBeaconMatch) ||
                 pDeviceInfo->GetUpdateTime() <= m_DiscoverStartTime ||
                 ( m_RequireSelectedRegistrar &&
                   !pDeviceInfo->GetSelectedRegistrarState() ) ||
                 ( m_RequireGroupFormationBit &&
                   !pDeviceInfo->GetGroupCapability()->bGroupFormationEnabled ) )
            {
                WFCActInfo("\t[%d] Match FAILed - WFDFilledIn:AllowBeacon=<%x : %x>, UpdateTime<=DiscoverTime=<%x = %I64d ? %I64d>, SelRegistrar=<%x ? %x>, RequireGroup=<%x ? %x>\n",
                    i, pDeviceInfo->IsWFDFilledIn(m_AllowBeaconMatch), m_AllowBeaconMatch,
                    (pDeviceInfo->GetUpdateTime() <= m_DiscoverStartTime), pDeviceInfo->GetUpdateTime(), m_DiscoverStartTime,
                    m_RequireSelectedRegistrar, pDeviceInfo->GetSelectedRegistrarState(),
                    m_RequireGroupFormationBit, pDeviceInfo->GetGroupCapability()->bGroupFormationEnabled);

                bSatisfied = false;
                goto exit;
            }

            WFCActInfo("\t[%d] Match SUCCEEDed - WFDFilledIn:AllowBeacon=<%x : %x>, UpdateTime<=DiscoverTime=<%x>, SelRegistrar=<%x ? %x>, RequireGroup=<%x ? %x>\n",
                i, pDeviceInfo->IsWFDFilledIn(m_AllowBeaconMatch), m_AllowBeaconMatch,
                (pDeviceInfo->GetUpdateTime() <= m_DiscoverStartTime),
                m_RequireSelectedRegistrar, pDeviceInfo->GetSelectedRegistrarState(),
                m_RequireGroupFormationBit, pDeviceInfo->GetGroupCapability()->bGroupFormationEnabled);
        }
        else
        {
            bSatisfied = false;
            goto exit;
        }
    }

exit:
    TraceActExit(bSatisfied);
    return bSatisfied;
}

void CP2PDiscoverJob::OnDeviceIndicationArrived(
    _In_ WDI_INDICATION_TYPE                 IndicationType,
    _In_ WFC_COMMAND_TOKEN                   CommandToken,
    _In_ WFC_PORT_ID                         PortId,
    _In_ ULONG                               EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    TraceActEntry();
    
    UNREFERENCED_PARAMETER(IndicationType);
    UNREFERENCED_PARAMETER(CommandToken);
    UNREFERENCED_PARAMETER(PortId);
    UNREFERENCED_PARAMETER(EventBufferLength);
    UNREFERENCED_PARAMETER(EventBuffer);

    //
    // The caching of the P2P Entry will be done in IncorporateBSSEntryList
    // Over here, if any filters were specified, just check that the filter was satisfied
    //
    if ( !m_IsInternalCancel &&
         m_DeviceFilterCount > 0 &&
         IsDeviceFilterSatisfied() )
    {
        WFCActInfo("Device filter was satisfied, triggering discover task cancellation.");
        m_IsInternalCancel = true;
        this->CancelJob();
    }

    TraceActExit(NDIS_STATUS_SUCCESS);
}

NDIS_STATUS CP2PDiscoverJob::InitializeDiscoverByGroupID(
    _In_ CAdapter* pAdapter,
    _In_ NDIS_PORT_NUMBER PortNumber,
    _In_ const DOT11_WFD_GROUP_ID& groupID,
    _In_ bool SelectedRegistrar,
    _In_ bool GroupFormation,
    _In_ bool AllowBeaconMatch,
    _In_ bool P2PRoamingReconnect,
    _In_opt_ DOT11_WFD_CHANNEL* pChannelHint,
    _In_opt_ CBSSEntry* pBSSEntryHint )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    //
    // The initialization could be called multiple times to rediscover devices, so cleanup any previous allocations
    //
    if ( m_pIndicationBuffer != NULL )
    {
        delete [] m_pIndicationBuffer;
        m_pIndicationBuffer = NULL;
    }

    if ( m_pDeviceFilterList != NULL )
    {
        delete [] m_pDeviceFilterList;
        m_pDeviceFilterList = NULL;
    }

    status = InitializeWithoutOid(
        WiFiJobP2PDiscover,
        pAdapter,
        true,
        false,
        WiFiPriorityScopeAdapter,
        WiFiSerializedJobPriority_Lowest,   // Parent is already running
        PortNumber );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't initialize COidJobBase");
        goto exit;
    }

    WFCActInfo(
        "Initializing internal discover for GroupID[ SSID = %!DOT11SSID!, DevAddr = %!MACADDR! ], SR = %d, GF = %d, Channel Hint Given? %d",
        (DOT11_SSID*)&groupID.SSID,
        (UCHAR*)groupID.DeviceAddress,
        SelectedRegistrar,
        GroupFormation,
        pChannelHint == NULL ? 0 : 1 );

    m_IsInternalDiscover = true;

    RtlCopyMemory(
        &m_TargetGroupID,
        &groupID,
        sizeof(m_TargetGroupID) );

    if ( !GroupFormation && SelectedRegistrar )
    {
        //
        // Join existing GO for pairing requires discover large
        // timeout (2 minutes) for user input on remote side.
        //
        m_DiscoverTimeoutInMs = 120000;
    }
    else if ( P2PRoamingReconnect )
    {
        //
        // P2P Roaming Reconnect should give up after 5 seconds
        //
        m_DiscoverTimeoutInMs = 5000;
    }
    else
    {
        m_DiscoverTimeoutInMs = 15000;
    }

    m_DiscoverParameters.DiscoverMode.DiscoveryType = WDI_P2P_DISCOVER_TYPE_SCAN_SOCIAL_CHANNELS;
    m_DiscoverParameters.DiscoverMode.ScanType = WDI_P2P_SCAN_TYPE_AUTO;
    m_DiscoverParameters.DiscoverMode.ForcedDiscovery = 1;

    m_DiscoverParameters.DiscoverMode.ScanRepeatCount = 0;
    m_DiscoverParameters.DiscoverMode.TimeBetweenScans = 50;

    m_DiscoverParameters.DwellTime.ActiveChannelDwellTime = WFC_CONSTANT::DefaultWFDActiveChannelDwellTime;
    m_DiscoverParameters.DwellTime.PassiveChannelDwellTime = WFC_CONSTANT::DefaultWFDPassiveChannelDwellTime;
    m_DiscoverParameters.DwellTime.MaximumScanTime = m_DiscoverTimeoutInMs;
    m_RemainingTimeoutInMs = m_DiscoverTimeoutInMs;

    m_DefaultDiscoverSSIDs[0].pElements = (UINT8*)m_TargetGroupID.SSID.ucSSID;
    m_DefaultDiscoverSSIDs[0].ElementCount = m_TargetGroupID.SSID.uSSIDLength;

    m_DiscoverParameters.Optional.SSIDList_IsPresent = 1;
    m_DiscoverParameters.SSIDList.pElements = (WDI_SSID*)&m_DefaultDiscoverSSIDs;
    m_DiscoverParameters.SSIDList.ElementCount = 1;

    m_DeviceFilterCount = 1;
    m_pDeviceFilterList = new DOT11_WFD_DISCOVER_DEVICE_FILTER[1];

    if ( m_pDeviceFilterList == NULL )
    {
        status = NDIS_STATUS_RESOURCES;
        WFCActErrorStatus("Couldn't allocate single device filter.");
        goto exit;
    }

    RtlCopyMemory(
        &m_pDeviceFilterList->DeviceID,
        &groupID.DeviceAddress,
        sizeof(m_pDeviceFilterList->DeviceID) );

    RtlCopyMemory(
        &m_pDeviceFilterList->GroupSSID,
        &groupID.SSID,
        sizeof(m_pDeviceFilterList->GroupSSID) );
    _Analysis_assume_( m_pDeviceFilterList->GroupSSID.uSSIDLength <= 32 );

    m_pDeviceFilterList->ucBitmask = DISCOVERY_FILTER_BITMASK_DEVICE;
    if ( groupID.SSID.uSSIDLength != 7 ||
         RtlCompareMemory(
             groupID.SSID.ucSSID,
             "DIRECT-",
             7 ) != 7 )
    {
        m_DiscoverParameters.DiscoverMode.DiscoveryType = WDI_P2P_DISCOVER_TYPE_SCAN_ONLY;
        m_pDeviceFilterList->ucBitmask = DISCOVERY_FILTER_BITMASK_GO;
    }

    m_RequireGroupFormationBit = GroupFormation;
    m_RequireSelectedRegistrar = SelectedRegistrar;
    
    if ( pChannelHint != NULL )
    {
        C_ASSERT(sizeof(WFD_OTA_CHANNEL) == sizeof(DOT11_WFD_CHANNEL));

        if ( ConvertP2PChannelToBandChannel(
                 m_pAdapter->GetAdapterPropertyCache(),
                 (WFD_OTA_CHANNEL*)pChannelHint,
                 &m_ChannelHintBandInfo.BandID,
                 &m_ChannelHintChannelNumber ) == NDIS_STATUS_SUCCESS )
        {
            WFCActInfo("Using Channel Hint from P2P Frame Information");
            m_ChannelHintAvailable = true;
        }
    }

    if ( !m_ChannelHintAvailable &&
         pBSSEntryHint != NULL &&
         pBSSEntryHint->GetChannelHint(
             &m_ChannelHintBandInfo.BandID,
             &m_ChannelHintChannelNumber ) == true )
    {
        WFCActInfo("Using Channel Hint from BSS Entry.");
        m_ChannelHintAvailable = true;
    }

    WFCActInfo("Allow beacon match? %u", AllowBeaconMatch);
    m_AllowBeaconMatch = AllowBeaconMatch;

exit:
    TraceActExit(status);
    return status;
}


void CP2PDiscoverJob::OnTimerCallback(
    _In_opt_ WFC_TIMER_CONTEXT )
{
    TraceActEntry();
    if ( m_DiscoverState == DiscoverStateWaitingForDiscoverCompletion ||
         m_DiscoverState == DiscoverStateWaitingForFallbackDiscoverCompletion )
    {
        WFCActInfo("Discover Timeout expired, cancelling discover task AllowFallback=<%x>, TriggerFallback=<%x>",
            m_AllowFallbackDiscover, m_TriggerFallbackDiscover);
        if ( m_AllowFallbackDiscover )
        {
            m_TriggerFallbackDiscover = true;
        }
        m_DiscoverTask.CancelTask();
    }
    TraceActExit(0);
}



NDIS_STATUS CP2PSendActionFrameRequestJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;

    switch(SET_OID_VALUE(pOidRequest))
    {
    case OID_DOT11_WFD_SEND_GO_NEGOTIATION_REQUEST:
    {
        status = PopulateGONegotiationRequestParameters(
            SET_OID_BUFFER_LENGTH(pOidRequest),
            (DOT11_SEND_GO_NEGOTIATION_REQUEST_PARAMETERS*)SET_OID_BUFFER(pOidRequest) );

        m_SendCompleteIndicationType = NDIS_STATUS_DOT11_WFD_GO_NEGOTIATION_REQUEST_SEND_COMPLETE;
    }
    break;
    case OID_DOT11_WFD_SEND_INVITATION_REQUEST:
    {
        status = PopulateInvitationRequestParameters(
            SET_OID_BUFFER_LENGTH(pOidRequest),
            (DOT11_SEND_INVITATION_REQUEST_PARAMETERS*)SET_OID_BUFFER(pOidRequest) );        

        m_SendCompleteIndicationType = NDIS_STATUS_DOT11_WFD_INVITATION_REQUEST_SEND_COMPLETE;
    }
    break;
    case OID_DOT11_WFD_SEND_PROVISION_DISCOVERY_REQUEST:
    {
        status = PopulateProvisionDiscoveryRequestParameters(
            SET_OID_BUFFER_LENGTH(pOidRequest),
            (NDIS_OBJECT_HEADER*)SET_OID_BUFFER(pOidRequest) );

        m_SendCompleteIndicationType = NDIS_STATUS_DOT11_WFD_PROVISION_DISCOVERY_REQUEST_SEND_COMPLETE;
    }
    break;
    default:
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_NOT_SUPPORTED;
        goto exit;
    }

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't populate action request frame parameters.");
        goto exit;
    }

exit:
    TraceActExit(status);
    return status;
}

void CP2PSendActionFrameRequestJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS ndisStatus )
{
    TraceActEntry();

    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        ndisStatus );

    TraceActExit(ndisStatus);
}

void CP2PSendActionFrameRequestJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS ndisStatus )
{
    NDIS_STATUS status = ndisStatus;

    TraceActEntry();

    if ( ndisStatus == NDIS_STATUS_SUCCESS &&
         m_pSendCompleteIndication == NULL )
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCActErrorStatus("Send action frame request job claimed success but no send complete indication provided.");
        NT_ASSERT(FALSE);
    }

    WFCActInfo("Indicating send action frame complete to NDIS with status 0x%x", ndisStatus);

    if ( status == NDIS_STATUS_SUCCESS )
    {
        m_pAdapter->IndicateStatus(
            m_NdisPortNumber,
            m_SendCompleteIndicationType,
            m_OidRequestId,
            m_pSendCompleteIndication,
            m_SendCompleteIndicationLength );
    }
    else
    {
        BYTE* pIndicationBuffer = NULL;
        ULONG indicationLength = 0;

        switch(m_RequestParameters.RequestParams.RequestFrameType)
        {
        case WDI_P2P_ACTION_FRAME_PROVISION_DISCOVERY_REQUEST:
            pIndicationBuffer = (BYTE*)&m_ProvDiscRequestSendCompleteParams;
            indicationLength = sizeof(m_ProvDiscRequestSendCompleteParams);
            m_ProvDiscRequestSendCompleteParams.Status = status;
            break;

        case WDI_P2P_ACTION_FRAME_INVITATION_REQUEST:
            pIndicationBuffer = (BYTE*)&m_InvitationRequestSendCompleteParams;
            indicationLength = sizeof(m_InvitationRequestSendCompleteParams);
            m_InvitationRequestSendCompleteParams.Status = status;
            break;

        case WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_REQUEST:
            pIndicationBuffer = (BYTE*)&m_GONRequestSendCompleteParams;
            indicationLength = sizeof(m_GONRequestSendCompleteParams);
            m_GONRequestSendCompleteParams.Status = status;
            break;
        default:
            NT_ASSERT(FALSE);
            break;
        };

        m_pAdapter->IndicateStatus(
            m_NdisPortNumber,
            m_SendCompleteIndicationType,
            m_OidRequestId,
            pIndicationBuffer,
            indicationLength );
    }

    TraceActExit(status);
}

void CP2PSendActionFrameRequestJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceEntry();

    NotifyStarted(NDIS_STATUS_INDICATION_REQUIRED);

    WFCActInfo("Sending request action frame. Blocking background scan for 15 seconds.");
    m_pAdapter->SetNextAllowedScanTime(15000);

    status = TrySendActionRequestFrame();

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't start process to send action frame request.");
        goto exit;
    }

exit:
    if ( status != NDIS_STATUS_SUCCESS )
    {
        CompleteJob(status);
    }

    TraceExit(status);
}

NDIS_STATUS CP2PSendActionFrameRequestJob::PopulateBSSEntry(
    _In_ CBSSEntry* pBSSEntry,
    _Out_ WDI_BSS_ENTRY_CONTAINER* pBSSEntryContainer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONGLONG currentTime = CSystem::get_CurrentTime();

    TraceEntry();

    NT_ASSERT(pBSSEntry && pBSSEntryContainer);

    RtlCopyMemory(
        &pBSSEntryContainer->BSSID,
        pBSSEntry->GetBssId(),
        sizeof(pBSSEntryContainer->BSSID) );

    C_ASSERT(sizeof(pBSSEntryContainer->SignalInfo) == sizeof(WFC_BSS_ENTRY_SIGNAL_INFO));
    RtlCopyMemory(
        &pBSSEntryContainer->SignalInfo,
        pBSSEntry->GetSignalInfo(),
        sizeof(pBSSEntryContainer->SignalInfo) );

    C_ASSERT(sizeof(pBSSEntryContainer->ChannelInfo) == sizeof(WFC_BSS_ENTRY_CHANNEL_INFO));
    RtlCopyMemory(
        &pBSSEntryContainer->ChannelInfo,
        pBSSEntry->GetChannelInfo(),
        sizeof(pBSSEntryContainer->ChannelInfo) );

    PVARIABLE_LENGTH_BUFFER_UINT8 pByteBlob = pBSSEntry->GetBeacon(currentTime);

    if ( pByteBlob != NULL )
    {
        pBSSEntryContainer->Optional.BeaconFrame_IsPresent = 1;
        pBSSEntryContainer->BeaconFrame.ElementCount = pByteBlob->dataLength;
        pBSSEntryContainer->BeaconFrame.pElements = pByteBlob->pBuffer;
    }

    pByteBlob = pBSSEntry->GetProbeResponse(currentTime);
    if ( pByteBlob != NULL )
    {
        pBSSEntryContainer->Optional.ProbeResponseFrame_IsPresent = 1;
        pBSSEntryContainer->ProbeResponseFrame.ElementCount = pByteBlob->dataLength;
        pBSSEntryContainer->ProbeResponseFrame.pElements = pByteBlob->pBuffer;
    }

    pByteBlob = pBSSEntry->GetDeviceContext();
    if ( pByteBlob->dataLength != 0 )
    {
        pBSSEntryContainer->Optional.DeviceSpecificContext_IsPresent = 1;
        pBSSEntryContainer->DeviceSpecificContext.ElementCount = pByteBlob->dataLength;
        pBSSEntryContainer->DeviceSpecificContext.pElements = pByteBlob->pBuffer;
    }

    TraceExit(status);
    return status;
}

NDIS_STATUS CP2PSendActionFrameRequestJob::TrySendActionRequestFrame()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    CBSSListManager* pDiscoveryCache =
        m_pAdapter->GetBSSListManager(WfcPortTypeWFDDevice);

    CBSSEntry* pBSSEntry = NULL;

    //
    // By default search for the other guy in device mode
    //
    DOT11_WFD_GROUP_ID targetGroupId = { {0}, { 7, "DIRECT-" } };
    RtlCopyMemory(
        &targetGroupId.DeviceAddress,
        &m_RequestParameters.RequestParams.PeerDeviceAddress,
        sizeof(targetGroupId.DeviceAddress) );

    //
    // Special case: Outgoing Provision Discovery Request to an
    // existing GO must be sent to the GO on its operating channel.
    //
    // NOTE: for WFD Services, this is no longer true as the P2P Group ID attribute has a different meaning
    //
    if ( m_RequestParameters.RequestParams.RequestFrameType ==
         WDI_P2P_ACTION_FRAME_PROVISION_DISCOVERY_REQUEST &&
         m_RequestParameters.ProvisionDiscoveryRequestInfo.Optional.GroupID_IsPresent &&
         !m_RequestParameters.ProvisionDiscoveryRequestInfo.Optional.ServiceAttributes_IsPresent )
    {
        if ( m_RequestParameters.ProvisionDiscoveryRequestInfo.GroupID.GroupSSID.ElementCount > sizeof(targetGroupId.SSID.ucSSID) )
        {
            WFCActError(
                "There were too many bytes stored in GroupID array. (%d bytes, expected <= 32)",
                m_RequestParameters.ProvisionDiscoveryRequestInfo.GroupID.GroupSSID.ElementCount );
            NT_ASSERT(FALSE);
            goto exit;
        }

        RtlCopyMemory(
            &targetGroupId.SSID.ucSSID,
            m_RequestParameters.ProvisionDiscoveryRequestInfo.GroupID.GroupSSID.pElements,
            m_RequestParameters.ProvisionDiscoveryRequestInfo.GroupID.GroupSSID.ElementCount );
        targetGroupId.SSID.uSSIDLength = m_RequestParameters.ProvisionDiscoveryRequestInfo.GroupID.GroupSSID.ElementCount;
    }

    pBSSEntry = pDiscoveryCache->FindBSSEntryByGroupID(
        targetGroupId,
        true );             // Match DeviceAddress in addition to SSID

    if ( pBSSEntry != NULL &&
         pBSSEntry->IsWFDFilledIn() )
    {   
        status = PopulateBSSEntry(
            pBSSEntry,
            &m_RequestParameters.DeviceDescriptor );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't populate BSS Entry.");
            goto exit;
        }

        if ( m_pTlvBuffer != NULL )
        {
            delete [] m_pTlvBuffer;
            m_pTlvBuffer = NULL;
        }

        status = GenerateWdiTaskP2pSendRequestActionFrame(
            &m_RequestParameters,
            sizeof(WFC_MESSAGE_METADATA),
            m_pAdapter->GetTlvContext( ),
            &m_TlvBufferLength,
            &m_pTlvBuffer );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't send P2P request action frame.");
            goto exit;
        }

        status = InitializeAndStartTask(
            &m_TaskCommand,
            WDI_TASK_P2P_SEND_REQUEST_ACTION_FRAME,
            m_TlvBufferLength,
            m_pTlvBuffer,
            m_pAdapter->GetNotificationManager() );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't start send action frame request.");
            goto exit;
        }

        m_SendFrameState = P2PSendRequestStateWaitingForSendCompletion;
        WFCActInfo("Waiting for send action frame completion.");
    }
    else
    {
        if ( m_AlreadyDiscovered )
        {
            status = NDIS_STATUS_FILE_NOT_FOUND;
            WFCActErrorStatus("Target device was not in bss cache after targeted discovery. Probably disappeared. Bailing out.");
            goto exit;
        }

        WFCActInfo("Didn't find peer device in scan cache. Doing on-demand discovery to find it.");

        status = m_DiscoverJob.InitializeDiscoverByGroupID(
            m_pAdapter,
            get_NdisPortNumber(),
            targetGroupId,
            false, /* RequireSelectedRegistrar */
            false, /* RequireGroupFormationBit */
            false, /* AllowBeaconMatch */
            false, /* P2PRoamingReconnect */
            NULL,
            pBSSEntry );
            
        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't initialize on-demand discover by group ID.");
            goto exit;
        }

        status = StartChildJob(&m_DiscoverJob);

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't start on-demand discover job.");
            goto exit;
        }

        m_AlreadyDiscovered = true;
        m_SendFrameState = P2PSendRequestStateFindingDevice;
    }

exit:
    TraceExit(status);
    return status;
}

NDIS_STATUS CP2PSendActionFrameRequestJob::ConstructCompletionIndication()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_INDICATION_P2P_SEND_REQUEST_ACTION_FRAME_COMPLETE_PARAMETERS completionParameters;
    ULONG requiredIndicationSize;
    ULONG offsetToIes;
    BYTE* allocatedSpace = NULL;
    BYTE* pIndicationBuffer;
    BYTE* pCommandCompletionBuffer;
    ULONG CommandCompletionBufferLength;
    NDIS_STATUS taskStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    status = m_TaskCommand.get_OutputBuffer(
        &CommandCompletionBufferLength,
        &pCommandCompletionBuffer );

    if ( status != NDIS_STATUS_SUCCESS ||
         CommandCompletionBufferLength < sizeof(WFC_MESSAGE_METADATA) )
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCActErrorStatus("Couldn't retrieve M4 contents to parse send action frame complete parameters.");
        goto exit;
    }

    status = CMessageHelper::GetStatusFromTaskOutput(
        &m_TaskCommand,
        &taskStatus );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't get WDI_STATUS from task command.");
        goto exit;
    }

    if ( taskStatus != NDIS_STATUS_SUCCESS )
    {
        status = taskStatus;
        WFCActErrorStatus(
            "Send action frame request task failed with 0x%x",
            taskStatus );
        goto exit;
    }

    status = ParseWdiIndicationP2pSendRequestActionFrameComplete(
        CommandCompletionBufferLength - sizeof(WFC_MESSAGE_METADATA),
        pCommandCompletionBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &completionParameters );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't parse send complete parameters.");
        goto exit;
    }

    switch(m_RequestParameters.RequestParams.RequestFrameType)
    {

    case WDI_P2P_ACTION_FRAME_PROVISION_DISCOVERY_REQUEST:
        pIndicationBuffer = (BYTE*)&m_ProvDiscRequestSendCompleteParams;
        requiredIndicationSize = sizeof(m_ProvDiscRequestSendCompleteParams);
        offsetToIes = requiredIndicationSize;

        // TODO: Fill in ReceiverAddress

        m_ProvDiscRequestSendCompleteParams.Status = status;
        m_ProvDiscRequestSendCompleteParams.uIEsOffset = offsetToIes;
        m_ProvDiscRequestSendCompleteParams.uIEsLength = completionParameters.SendActionFrameResult.FrameIEs.ElementCount;
        break;

    case WDI_P2P_ACTION_FRAME_INVITATION_REQUEST:
        pIndicationBuffer = (BYTE*)&m_InvitationRequestSendCompleteParams;
        requiredIndicationSize = sizeof(m_InvitationRequestSendCompleteParams);
        offsetToIes = requiredIndicationSize;

        // TODO: Fill in ReceiverAddress

        m_InvitationRequestSendCompleteParams.Status = status;
        m_InvitationRequestSendCompleteParams.uIEsOffset = offsetToIes;
        m_InvitationRequestSendCompleteParams.uIEsLength = completionParameters.SendActionFrameResult.FrameIEs.ElementCount;
        break;

    case WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_REQUEST:
        pIndicationBuffer = (BYTE*)&m_GONRequestSendCompleteParams;
        requiredIndicationSize = sizeof(m_GONRequestSendCompleteParams);
        offsetToIes = requiredIndicationSize;
        m_GONRequestSendCompleteParams.Status = status;
        m_GONRequestSendCompleteParams.uIEsOffset = offsetToIes;
        m_GONRequestSendCompleteParams.uIEsLength = completionParameters.SendActionFrameResult.FrameIEs.ElementCount;
        break;

    default:
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_NOT_SUPPORTED;
        goto exit;
    };

    if ( !NT_SUCCESS(
             RtlULongAdd(
                 requiredIndicationSize,
                 completionParameters.SendActionFrameResult.FrameIEs.ElementCount,
                 &requiredIndicationSize) ) ) 
    {
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        WFCActErrorStatus("Overflowed when attempting to calculate required indication size.");
        goto exit;
    }
    
    allocatedSpace = new BYTE[requiredIndicationSize];

    if ( allocatedSpace  == NULL )
    {
        status = NDIS_STATUS_RESOURCES;
        WFCActErrorStatus("Couldn't allocate completion parameters structure.");
        goto exit;
    }

    RtlCopyMemory(
        allocatedSpace,
        pIndicationBuffer,
        offsetToIes );

    if ( completionParameters.SendActionFrameResult.FrameIEs.ElementCount > 0 )
    {
        RtlCopyMemory(
            allocatedSpace + offsetToIes,
            completionParameters.SendActionFrameResult.FrameIEs.pElements,
            completionParameters.SendActionFrameResult.FrameIEs.ElementCount );
    }

    m_pSendCompleteIndication = allocatedSpace;
    m_SendCompleteIndicationLength = requiredIndicationSize;
    allocatedSpace = NULL;

exit:
    if ( allocatedSpace != NULL )
    {
        delete [] allocatedSpace;
    }

    TraceActExit(status);
    return status;
}

void CP2PSendActionFrameRequestJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    bool bCompleteJob = false;

    UNREFERENCED_PARAMETER(SenderObject);

    TraceActEntry();

    switch(m_SendFrameState)
    {
    case P2PSendRequestStateFindingDevice:

        WFCActInfo("On-demand send action frame discovery completed with 0x%x. (Masking if there's an error)", OperationStatus);
        // May have been cancelled. Ignore status code.

        OperationStatus = TrySendActionRequestFrame();

        break;

    case P2PSendRequestStateWaitingForSendCompletion:
        NT_ASSERT(SenderObject == &m_TaskCommand);

        WFCActInfo("Send Action Frame Request Task Completed with 0x%x", OperationStatus);

        OperationStatus = ConstructCompletionIndication();

        bCompleteJob = true;

        break;

    default:
        NT_ASSERT(FALSE);
        OperationStatus = NDIS_STATUS_NOT_SUPPORTED;
        break;
    }

    if ( OperationStatus != NDIS_STATUS_SUCCESS ||
         bCompleteJob )
    {
        CompleteJob(OperationStatus);
    }

    TraceActExit(OperationStatus);
}

void CP2PSendActionFrameRequestJob::OnJobCancelled()
{
    TraceActEntry();

    if ( m_IsCancelled )
    {
        goto exit;
    }

    m_IsCancelled = true;

    if ( m_SendFrameState != P2PSendRequestStateInit &&
         m_SendFrameState != P2PSendRequestStateDone )
    {
        CancelActivePendingCommand();
    }
    else
    {
        //
        // Cancel called when there was nothing pending.
        //
        NT_ASSERT(FALSE);
    }

exit:
    TraceActExit(NDIS_STATUS_SUCCESS);
}


NDIS_STATUS CP2PSendActionFrameRequestJob::CopyExtraIEsToByteBlob(
    _In_ WDI_BYTE_BLOB* pTarget,
    _In_ ULONG DataLength,
    _In_reads_bytes_(DataLength) BYTE* pExtraIEs )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    //
    // OID Request will disappear once InitializeFromOidRequest
    // returns, so we need to make a copy of the vendor IEs to send down.
    //
    // This needs to happen here because code cannot fill in the
    // entire request right now. The structure is only partially
    // filled when the request comes in, and then device descriptor is
    // filled in later.
    //
    pTarget->pElements = new BYTE[DataLength];

    if ( pTarget->pElements == NULL )
    {
        WFCActErrorStatus(
            "Couldn't allocate buffer to copy additional IEs to local storage. (%d bytes requested)",
            DataLength );
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    //
    // Set this flag for automatic clean-up when the object is destroyed
    //
    pTarget->MemoryInternallyAllocated = true;
    pTarget->ElementCount = DataLength;

    RtlCopyMemory(
        pTarget->pElements,
        pExtraIEs,
        DataLength );

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CP2PSendActionFrameRequestJob::PopulateGONegotiationRequestParameters(
    _In_ ULONG RequestLength,
    _In_reads_bytes_(RequestLength) DOT11_SEND_GO_NEGOTIATION_REQUEST_PARAMETERS* pRequestParams )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    if ( RequestLength < sizeof(*pRequestParams) )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    //
    // Fill in static parameters
    //
    m_RequestParameters.RequestParams.RequestFrameType = WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_REQUEST;

    RtlCopyMemory(
        &m_RequestParameters.RequestParams.PeerDeviceAddress,
        pRequestParams->PeerDeviceAddress,
        sizeof(m_RequestParameters.RequestParams.PeerDeviceAddress));

    m_RequestParameters.RequestParams.DialogToken =
        pRequestParams->DialogToken;

    m_RequestParameters.RequestParams.SendTimeout =
        pRequestParams->uSendTimeout;
    
    // TODO: Define constant for post ACK dwell time
    m_RequestParameters.RequestParams.PostACKDwellTime = 125;

    //
    // Fill in Specific Request Parameters
    //
    m_RequestParameters.Optional.GONegotiationRequestInfo_IsPresent = 1;

    m_RequestParameters.GONegotiationRequestInfo.RequestParams.GOIntent =
        pRequestParams->GroupOwnerIntent.Intent;

    m_RequestParameters.GONegotiationRequestInfo.RequestParams.TieBreaker =
        pRequestParams->GroupOwnerIntent.TieBreaker ? 1 : 0;

    m_RequestParameters.GONegotiationRequestInfo.RequestParams.GOConfigTimeout =
        pRequestParams->MinimumConfigTimeout.GOTimeout;

    m_RequestParameters.GONegotiationRequestInfo.RequestParams.ClientConfigTimeout =
        pRequestParams->MinimumConfigTimeout.ClientTimeout;

    RtlCopyMemory(
        &m_RequestParameters.GONegotiationRequestInfo.RequestParams.IntendedInterfaceAddress,
        pRequestParams->IntendedInterfaceAddress,
        sizeof(m_RequestParameters.GONegotiationRequestInfo.RequestParams.IntendedInterfaceAddress));

    m_RequestParameters.GONegotiationRequestInfo.RequestParams.GroupCapability =
        (UINT8)pRequestParams->GroupCapability;
    m_RequestParameters.GONegotiationRequestInfo.RequestParams.OSSetGroupCapabilityBitmask =
        (DOT11_WFD_GROUP_CAPABILITY_PERSISTENT_GROUP |
         DOT11_WFD_GROUP_CAPABILITY_EAPOL_KEY_IP_ADDRESS_ALLOCATION_SUPPORTED);

    WFCActInfo("[GONegotiationRequest]: Group:OSSetCapabilities = <0x%08x : 0x%08x>",
        m_RequestParameters.GONegotiationRequestInfo.RequestParams.GroupCapability,
        m_RequestParameters.GONegotiationRequestInfo.RequestParams.OSSetGroupCapabilityBitmask);

    //
    // Fill in vendor IEs
    //
    if ( pRequestParams->uIEsLength > 0 )
    {
        ULONG bufferLengthCheck;

        if ( !NT_SUCCESS(
                 RtlULongAdd(
                     pRequestParams->uIEsLength,
                     pRequestParams->uIEsOffset,
                     &bufferLengthCheck ) ) ||
             bufferLengthCheck > RequestLength )
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("Bad IEs information in buffer provided from OS.");
            goto exit;
        }

        m_RequestParameters.Optional.VendorIEs_IsPresent = 1;
        status = CopyExtraIEsToByteBlob(
            &m_RequestParameters.VendorIEs,
            pRequestParams->uIEsLength,
            (BYTE*)pRequestParams + pRequestParams->uIEsOffset );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            goto exit;
        }
    }

    if (pRequestParams->Header.Revision == DOT11_SEND_GO_NEGOTIATION_REQUEST_PARAMETERS_REVISION_2 &&
        RequestLength >= DOT11_SIZEOF_SEND_GO_NEGOTIATION_REQUEST_PARAMETERS_REVISION_2)
    {
        PDOT11_SEND_GO_NEGOTIATION_REQUEST_PARAMETERS_V2 pV2RequestParams = (PDOT11_SEND_GO_NEGOTIATION_REQUEST_PARAMETERS_V2)pRequestParams;
        if (pV2RequestParams->bUseSpecifiedRemoteListenChannel)
        {
            m_RequestParameters.GONegotiationRequestInfo.Optional.RemoteListenChannel_IsPresent = TRUE;

            RtlCopyMemory(m_RequestParameters.GONegotiationRequestInfo.RemoteListenChannel.CountryOrRegionString,
                &pV2RequestParams->RemoteListenChannel.CountryRegionString,
                sizeof(DOT11_COUNTRY_OR_REGION_STRING));
            m_RequestParameters.GONegotiationRequestInfo.RemoteListenChannel.OperatingClass = pV2RequestParams->RemoteListenChannel.OperatingClass;
            m_RequestParameters.GONegotiationRequestInfo.RemoteListenChannel.ChannelNumber = pV2RequestParams->RemoteListenChannel.ChannelNumber;
        }
    }

    //
    // Fill in default completion parameters
    //
    ASSIGN_NDIS_OBJECT_HEADER(
        m_GONRequestSendCompleteParams.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_GO_NEGOTIATION_REQUEST_SEND_COMPLETE_PARAMETERS_REVISION_1,
        DOT11_SIZEOF_GO_NEGOTIATION_REQUEST_SEND_COMPLETE_PARAMETERS_REVISION_1 );

    RtlCopyMemory(
        &m_GONRequestSendCompleteParams.PeerDeviceAddress,
        pRequestParams->PeerDeviceAddress,
        sizeof(m_GONRequestSendCompleteParams.PeerDeviceAddress) );

    m_GONRequestSendCompleteParams.DialogToken =
        pRequestParams->DialogToken;


exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CP2PSendActionFrameRequestJob::PopulateInvitationRequestParameters(
    _In_ ULONG RequestLength,
    _In_reads_bytes_(RequestLength) DOT11_SEND_INVITATION_REQUEST_PARAMETERS* pRequestParams )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    if ( RequestLength < sizeof(*pRequestParams) )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    //
    // Fill in static parameters
    //
    m_RequestParameters.RequestParams.RequestFrameType = WDI_P2P_ACTION_FRAME_INVITATION_REQUEST;

    RtlCopyMemory(
        &m_RequestParameters.RequestParams.PeerDeviceAddress,
        pRequestParams->PeerDeviceAddress,
        sizeof(m_RequestParameters.RequestParams.PeerDeviceAddress));

    m_RequestParameters.RequestParams.DialogToken =
        pRequestParams->DialogToken;

    m_RequestParameters.RequestParams.SendTimeout =
        pRequestParams->uSendTimeout;
    
    // TODO: Define constant for post ACK dwell time
    m_RequestParameters.RequestParams.PostACKDwellTime = 125;

    //
    // Fill in Specific Request Parameters
    //
    m_RequestParameters.Optional.InvitationRequestInfo_IsPresent = 1;

    m_RequestParameters.InvitationRequestInfo.RequestParams.GOConfigTimeout =
        pRequestParams->MinimumConfigTimeout.GOTimeout;
        
    m_RequestParameters.InvitationRequestInfo.RequestParams.ClientConfigTimeout = 
        pRequestParams->MinimumConfigTimeout.ClientTimeout;

    // BUGBUG: Not sure if this is shifted correctly
    m_RequestParameters.InvitationRequestInfo.RequestParams.InvitationFlags =
        pRequestParams->InvitationFlags.InvitationType ? 1 : 0;
    
    m_RequestParameters.InvitationRequestInfo.RequestParams.IsLocalGO =
        pRequestParams->bLocalGO ? 1 : 0;

    RtlCopyMemory(
        &m_SSIDStorage,
        &pRequestParams->GroupID.SSID,
        sizeof(m_SSIDStorage) );
    m_RequestParameters.InvitationRequestInfo.GroupID.GroupSSID.pElements = m_SSIDStorage.ucSSID;
    m_RequestParameters.InvitationRequestInfo.GroupID.GroupSSID.ElementCount = m_SSIDStorage.uSSIDLength;
    RtlCopyMemory(
        &m_RequestParameters.InvitationRequestInfo.GroupID.DeviceAddress,
        pRequestParams->GroupID.DeviceAddress,
        sizeof(m_RequestParameters.InvitationRequestInfo.GroupID.DeviceAddress));

    if ( pRequestParams->bUseGroupBSSID )
    {
        m_RequestParameters.InvitationRequestInfo.Optional.GroupBSSID_IsPresent = 1;
        RtlCopyMemory(
            &m_RequestParameters.InvitationRequestInfo.GroupBSSID,
            pRequestParams->GroupBSSID,
            sizeof(m_RequestParameters.InvitationRequestInfo.GroupBSSID) );
    }

    if ( pRequestParams->bUseSpecifiedOperatingChannel )
    {
        m_RequestParameters.InvitationRequestInfo.Optional.OperatingChannel_IsPresent = 1;
        m_RequestParameters.InvitationRequestInfo.OperatingChannel.OperatingClass = pRequestParams->OperatingChannel.OperatingClass;
        m_RequestParameters.InvitationRequestInfo.OperatingChannel.ChannelNumber = pRequestParams->OperatingChannel.ChannelNumber;
        C_ASSERT( sizeof( m_RequestParameters.InvitationRequestInfo.OperatingChannel.CountryOrRegionString ) == sizeof( pRequestParams->OperatingChannel.CountryRegionString ) );
        RtlCopyMemory(
            m_RequestParameters.InvitationRequestInfo.OperatingChannel.CountryOrRegionString,
            pRequestParams->OperatingChannel.CountryRegionString,
            sizeof( pRequestParams->OperatingChannel.CountryRegionString ) );
    }

    //
    // Fill in vendor IEs
    //
    if ( pRequestParams->uIEsLength > 0 )
    {
        ULONG bufferLengthCheck;

        if ( !NT_SUCCESS(
                 RtlULongAdd(
                     pRequestParams->uIEsLength,
                     pRequestParams->uIEsOffset,
                     &bufferLengthCheck ) ) ||
             bufferLengthCheck > RequestLength )
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("Bad IEs information in buffer provided from OS.");
            goto exit;
        }

        m_RequestParameters.Optional.VendorIEs_IsPresent = 1;
        status = CopyExtraIEsToByteBlob(
            &m_RequestParameters.VendorIEs,
            pRequestParams->uIEsLength,
            (BYTE*)pRequestParams + pRequestParams->uIEsOffset );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            goto exit;
        }
    }

    //
    // Fill in default completion parameters
    //
    ASSIGN_NDIS_OBJECT_HEADER(
        m_InvitationRequestSendCompleteParams.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_INVITATION_REQUEST_SEND_COMPLETE_PARAMETERS_REVISION_1,
        DOT11_SIZEOF_INVITATION_REQUEST_SEND_COMPLETE_PARAMETERS_REVISION_1 );

    RtlCopyMemory(
        &m_InvitationRequestSendCompleteParams.PeerDeviceAddress,
        pRequestParams->PeerDeviceAddress,
        sizeof(m_InvitationRequestSendCompleteParams.PeerDeviceAddress) );

    m_InvitationRequestSendCompleteParams.DialogToken =
        pRequestParams->DialogToken;

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CP2PSendActionFrameRequestJob::PopulateProvisionDiscoveryRequestParameters(
    _In_ ULONG RequestLength,
    _In_reads_bytes_(RequestLength) NDIS_OBJECT_HEADER *pNdisHeader )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    DOT11_WFD_GROUP_ID *pGroupID;
    UINT8 GroupCapability;
    ULONG uIEsOffset;
    ULONG uIEsLength;

    TraceActEntry();

    if ( RequestLength < sizeof(DOT11_SEND_PROVISION_DISCOVERY_REQUEST_PARAMETERS) )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    switch (pNdisHeader->Revision)
    {
        case (DOT11_SEND_PROVISION_DISCOVERY_REQUEST_PARAMETERS_REVISION_1):
        {
            DOT11_SEND_PROVISION_DISCOVERY_REQUEST_PARAMETERS* pRequestParams =
                (DOT11_SEND_PROVISION_DISCOVERY_REQUEST_PARAMETERS*) pNdisHeader;

            RtlCopyMemory(
                &m_RequestParameters.RequestParams.PeerDeviceAddress,
                pRequestParams->PeerDeviceAddress,
                sizeof(m_RequestParameters.RequestParams.PeerDeviceAddress)); 
            m_RequestParameters.RequestParams.DialogToken = pRequestParams->DialogToken; 
            m_RequestParameters.RequestParams.SendTimeout = pRequestParams->uSendTimeout;
            pGroupID = pRequestParams->bUseGroupID ? &pRequestParams->GroupID : NULL;
            GroupCapability = (UINT8)pRequestParams->GroupCapability;
            uIEsLength = pRequestParams->uIEsLength;
            uIEsOffset = pRequestParams->uIEsOffset;
    
            break;
        }

        case (DOT11_SEND_PROVISION_DISCOVERY_REQUEST_PARAMETERS_REVISION_2):
        {
            DOT11_SEND_PROVISION_DISCOVERY_REQUEST_PARAMETERS_V2* pRequestParams =
                (DOT11_SEND_PROVISION_DISCOVERY_REQUEST_PARAMETERS_V2*) pNdisHeader;

            RtlCopyMemory(
                &m_RequestParameters.RequestParams.PeerDeviceAddress,
                pRequestParams->PeerDeviceAddress,
                sizeof(m_RequestParameters.RequestParams.PeerDeviceAddress)); 
            m_RequestParameters.RequestParams.DialogToken = pRequestParams->DialogToken; 
            m_RequestParameters.RequestParams.SendTimeout = pRequestParams->uSendTimeout;
            pGroupID = pRequestParams->bUseGroupID ? &pRequestParams->GroupID : NULL;
            GroupCapability = (UINT8)pRequestParams->GroupCapability;
            uIEsLength = pRequestParams->uIEsLength;
            uIEsOffset = pRequestParams->uIEsOffset;
    
            //
            // If the request did not specify any ServiceAttributes, we are done
            //
            if (!pRequestParams->bEncodeServiceAttributes)
            {
                break;
            }

            m_RequestParameters.ProvisionDiscoveryRequestInfo.Optional.ServiceAttributes_IsPresent = 1;

            m_RequestParameters.ProvisionDiscoveryRequestInfo.ServiceAttributes.StatusCode =
                (UINT8)pRequestParams->Status;

            RtlCopyMemory(
                &m_RequestParameters.ProvisionDiscoveryRequestInfo.ServiceAttributes.IntendedInterfaceAddress,
                &pRequestParams->IntendedInterfaceAddress,
                sizeof(WDI_MAC_ADDRESS));

            m_RequestParameters.ProvisionDiscoveryRequestInfo.ServiceAttributes.ConnectionCapability =
                pRequestParams->ConnectionCapability;
            m_RequestParameters.ProvisionDiscoveryRequestInfo.ServiceAttributes.FeatureCapability =
                pRequestParams->FeatureCapability;

            m_RequestParameters.ProvisionDiscoveryRequestInfo.ServiceAttributes.AdvertisementID =
                pRequestParams->AdvertisementID.AdvertisementID;

            RtlCopyMemory(
                &m_RequestParameters.ProvisionDiscoveryRequestInfo.ServiceAttributes.ServiceAddress,
                &pRequestParams->AdvertisementID.ServiceAddress,
                sizeof(WDI_MAC_ADDRESS));

            m_RequestParameters.ProvisionDiscoveryRequestInfo.ServiceAttributes.SessionID =
                pRequestParams->SessionID.SessionID;

            RtlCopyMemory(
                &m_RequestParameters.ProvisionDiscoveryRequestInfo.ServiceAttributes.SessionAddress,
                &pRequestParams->SessionID.SessionAddress,
                sizeof(WDI_MAC_ADDRESS));

            m_RequestParameters.ProvisionDiscoveryRequestInfo.ServiceAttributes.GOConfigTimeout =
                pRequestParams->ConfigurationTimeout.GOTimeout;

            m_RequestParameters.ProvisionDiscoveryRequestInfo.ServiceAttributes.ClientConfigTimeout =
                pRequestParams->ConfigurationTimeout.ClientTimeout;

            if ( pRequestParams->bUsePersistentGroup )
            {
                m_RequestParameters.ProvisionDiscoveryRequestInfo.Optional.PersistentGroupID_IsPresent = 1;
                m_RequestParameters.ProvisionDiscoveryRequestInfo.ServiceAttributes.UsePersistentGroup = 1;

                RtlCopyMemory(
                    &m_PersistedGroupSSID,
                    &pRequestParams->PersistentGroupID.SSID,
                    sizeof(m_PersistedGroupSSID) );
                m_RequestParameters.ProvisionDiscoveryRequestInfo.PersistentGroupID.GroupSSID.pElements = m_PersistedGroupSSID.ucSSID;
                m_RequestParameters.ProvisionDiscoveryRequestInfo.PersistentGroupID.GroupSSID.ElementCount = m_PersistedGroupSSID.uSSIDLength;
                RtlCopyMemory(
                    &m_RequestParameters.ProvisionDiscoveryRequestInfo.PersistentGroupID.DeviceAddress,
                    pRequestParams->PersistentGroupID.DeviceAddress,
                    sizeof(m_RequestParameters.ProvisionDiscoveryRequestInfo.PersistentGroupID.DeviceAddress));        
            }

            if ( pRequestParams->ServiceSessionInfo.uSessionInfoLength )
            {
                m_RequestParameters.ProvisionDiscoveryRequestInfo.Optional.ServiceSessionInfo_IsPresent = 1;

                m_RequestParameters.ProvisionDiscoveryRequestInfo.ServiceSessionInfo.SimpleAssign(
                    pRequestParams->ServiceSessionInfo.ucSessionInfo,
                    pRequestParams->ServiceSessionInfo.uSessionInfoLength);
            }

            if ( pRequestParams->bFollowOnProvisionDiscovery )
            {
                m_RequestParameters.ProvisionDiscoveryRequestInfo.ServiceAttributes.FollowOnProvisionDiscovery = 1;
            }

            break;
        }

        default:
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("Invalid Provision Discovery Request version %d.", pNdisHeader->Revision);
            goto exit;
        }
    }

    //
    // Fill in static parameters
    //
    m_RequestParameters.RequestParams.RequestFrameType = WDI_P2P_ACTION_FRAME_PROVISION_DISCOVERY_REQUEST;

    // TODO: Define constant for post ACK dwell time
    m_RequestParameters.RequestParams.PostACKDwellTime = 125;

    //
    // Fill in Specific Request Parameters
    //
    m_RequestParameters.Optional.ProvisionDiscoveryRequestInfo_IsPresent = 1;
    m_RequestParameters.ProvisionDiscoveryRequestInfo.RequestParams.GroupCapability = GroupCapability;
    m_RequestParameters.ProvisionDiscoveryRequestInfo.RequestParams.OSSetGroupCapabilityBitmask =
                    (DOT11_WFD_GROUP_CAPABILITY_PERSISTENT_GROUP |
                    DOT11_WFD_GROUP_CAPABILITY_EAPOL_KEY_IP_ADDRESS_ALLOCATION_SUPPORTED);

    WFCActInfo("[ProvisionDiscoveryRequest]: Group:OSSetCapabilities = <0x%08x : 0x%08x>",
        m_RequestParameters.ProvisionDiscoveryRequestInfo.RequestParams.GroupCapability,
        m_RequestParameters.ProvisionDiscoveryRequestInfo.RequestParams.OSSetGroupCapabilityBitmask);

    if ( pGroupID )
    {
        m_RequestParameters.ProvisionDiscoveryRequestInfo.Optional.GroupID_IsPresent = 1;

        RtlCopyMemory(
            &m_SSIDStorage,
            &pGroupID->SSID,
            sizeof(m_SSIDStorage) );
        m_RequestParameters.ProvisionDiscoveryRequestInfo.GroupID.GroupSSID.pElements = m_SSIDStorage.ucSSID;
        m_RequestParameters.ProvisionDiscoveryRequestInfo.GroupID.GroupSSID.ElementCount = m_SSIDStorage.uSSIDLength;
        RtlCopyMemory(
            &m_RequestParameters.ProvisionDiscoveryRequestInfo.GroupID.DeviceAddress,
            pGroupID->DeviceAddress,
            sizeof(m_RequestParameters.ProvisionDiscoveryRequestInfo.GroupID.DeviceAddress));        
    }
        
    //
    // Fill in vendor IEs
    //
    if ( uIEsLength > 0 )
    {
        ULONG bufferLengthCheck;

        if ( !NT_SUCCESS(
                 RtlULongAdd(
                     uIEsLength,
                     uIEsOffset,
                     &bufferLengthCheck ) ) ||
             bufferLengthCheck > RequestLength )
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("Bad IEs information in buffer provided from OS.");
            goto exit;
        }

        m_RequestParameters.Optional.VendorIEs_IsPresent = 1;
        status = CopyExtraIEsToByteBlob(
            &m_RequestParameters.VendorIEs,
            uIEsLength,
            ((BYTE*) pNdisHeader) + uIEsOffset );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            goto exit;
        }
    }

    //
    // Fill in default completion parameters
    //
    ASSIGN_NDIS_OBJECT_HEADER(
        m_ProvDiscRequestSendCompleteParams.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_PROVISION_DISCOVERY_REQUEST_SEND_COMPLETE_PARAMETERS_REVISION_1,
        DOT11_SIZEOF_PROVISION_DISCOVERY_REQUEST_SEND_COMPLETE_PARAMETERS_REVISION_1 );

    RtlCopyMemory(
        &m_ProvDiscRequestSendCompleteParams.PeerDeviceAddress,
        &m_RequestParameters.RequestParams.PeerDeviceAddress,
        sizeof(m_ProvDiscRequestSendCompleteParams.PeerDeviceAddress) );

    m_ProvDiscRequestSendCompleteParams.DialogToken = m_RequestParameters.RequestParams.DialogToken;

exit:
    TraceActExit(status);
    return status;
}


NDIS_STATUS CP2PSendActionFrameResponseJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();
    ULONG eventBufferLength;
    BYTE* pEventBuffer;

    TraceActEntry();

    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;

    switch(SET_OID_VALUE(pOidRequest))
    {
    case OID_DOT11_WFD_SEND_GO_NEGOTIATION_RESPONSE:
    {
        status = PopulateGONegotiationResponseParameters(
            SET_OID_BUFFER_LENGTH(pOidRequest),
            (DOT11_SEND_GO_NEGOTIATION_RESPONSE_PARAMETERS*)SET_OID_BUFFER(pOidRequest) );

        m_SendCompleteIndicationType = NDIS_STATUS_DOT11_WFD_GO_NEGOTIATION_RESPONSE_SEND_COMPLETE;
    }
    break;
    case OID_DOT11_WFD_SEND_GO_NEGOTIATION_CONFIRMATION:
    {
        status = PopulateGONegotiationConfirmationParameters(
            SET_OID_BUFFER_LENGTH(pOidRequest),
            (DOT11_SEND_GO_NEGOTIATION_CONFIRMATION_PARAMETERS*)SET_OID_BUFFER(pOidRequest) );

        m_SendCompleteIndicationType = NDIS_STATUS_DOT11_WFD_GO_NEGOTIATION_CONFIRMATION_SEND_COMPLETE;
    }
    break;
    case OID_DOT11_WFD_SEND_INVITATION_RESPONSE:
    {
        status = PopulateInvitationResponseParameters(
            SET_OID_BUFFER_LENGTH(pOidRequest),
            (DOT11_SEND_INVITATION_RESPONSE_PARAMETERS*)SET_OID_BUFFER(pOidRequest) );        

        m_SendCompleteIndicationType = NDIS_STATUS_DOT11_WFD_INVITATION_RESPONSE_SEND_COMPLETE;
    }
    break;
    case OID_DOT11_WFD_SEND_PROVISION_DISCOVERY_RESPONSE:
    {
        status = PopulateProvisionDiscoveryResponseParameters(
            SET_OID_BUFFER_LENGTH(pOidRequest),
            (NDIS_OBJECT_HEADER*)SET_OID_BUFFER(pOidRequest) );

        m_SendCompleteIndicationType = NDIS_STATUS_DOT11_WFD_PROVISION_DISCOVERY_RESPONSE_SEND_COMPLETE;
    }
    break;
    default:
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_NOT_SUPPORTED;
        goto exit;
    }

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't populate action response frame parameters.");
        goto exit;
    }

    status = m_pAdapter->GetAdapterPropertyCache()->GetPropertyBuffer(
        WfcAdapterProperty_LastActionFrameIndicationEventBuffer,
        &eventBufferLength,
        &pEventBuffer );

    if ( status != NDIS_STATUS_SUCCESS ||
         pEventBuffer == NULL ||
         eventBufferLength == 0 )
    {
        WFCActErrorStatus("Could not retrieve information about last action frame indication.");
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    if ( m_pAdapter->GetAdapterPropertyCache()->SetPropertyBuffer(
             WfcAdapterProperty_LastActionFrameIndicationEventBuffer,
             0,
             NULL ) != NDIS_STATUS_SUCCESS )
    {
        WFCActInfo("Could not reset last action frame indication property.");
        status = NDIS_STATUS_INVALID_STATE;
        NT_ASSERT(FALSE);
        goto exit;
    }

    {
        WDI_INDICATION_P2P_ACTION_FRAME_RECEIVED_PARAMETERS incomingFrameInfo;

        status = ParseWdiIndicationP2pActionFrameReceived(
            eventBufferLength - sizeof(WFC_MESSAGE_METADATA),
            pEventBuffer + sizeof(WFC_MESSAGE_METADATA),
            m_pAdapter->GetTlvContext( ),
            &incomingFrameInfo );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't parse previous incoming frame info.");
            NT_ASSERT(FALSE);
            goto exit;
        }

        // Take ownership of all data in parsed FrameInfo structure
        RtlCopyMemory(
            &m_ResponseParameters.PrecedingFrameInformation,
            &incomingFrameInfo.FrameInfo,
            sizeof(m_ResponseParameters.PrecedingFrameInformation) );

        // Zero out memory so incomingFrameInfo doesn't cause double-free
        RtlZeroMemory(
            &incomingFrameInfo.FrameInfo,
            sizeof(incomingFrameInfo.FrameInfo ) );
    }


exit:
    TraceActExit(status);
    return status;
}

void CP2PSendActionFrameResponseJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS ndisStatus )
{
    NDIS_STATUS status = ndisStatus;

    WFCActInfo("Indicating send action frame complete to NDIS with status 0x%x", ndisStatus);

    if ( m_pSendCompleteIndication != NULL )
    {
        m_pAdapter->IndicateStatus(
            m_NdisPortNumber,
            m_SendCompleteIndicationType,
            m_OidRequestId,
            m_pSendCompleteIndication,
            m_SendCompleteIndicationLength );
    }
    else
    {
        BYTE* pIndicationBuffer = NULL;
        ULONG indicationLength = 0;

        switch(m_ResponseParameters.ResponseParams.ResponseFrameType)
        {
        case WDI_P2P_ACTION_FRAME_PROVISION_DISCOVERY_RESPONSE:
            pIndicationBuffer = (BYTE*)&m_ProvDiscResponseSendCompleteParams;
            indicationLength = sizeof(m_ProvDiscResponseSendCompleteParams);
            m_ProvDiscResponseSendCompleteParams.Status = status;
            break;

        case WDI_P2P_ACTION_FRAME_INVITATION_RESPONSE:
            pIndicationBuffer = (BYTE*)&m_InvitationResponseSendCompleteParams;
            indicationLength = sizeof(m_InvitationResponseSendCompleteParams);
            m_InvitationResponseSendCompleteParams.Status = status;
            break;

        case WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_RESPONSE:
            pIndicationBuffer = (BYTE*)&m_GONResponseSendCompleteParams;
            indicationLength = sizeof(m_GONResponseSendCompleteParams);
            m_GONResponseSendCompleteParams.Status = status;
            break;

        case WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_CONFIRM:
            pIndicationBuffer = (BYTE*)&m_GONConfirmationSendCompleteParams;
            indicationLength = sizeof(m_GONConfirmationSendCompleteParams);
            m_GONConfirmationSendCompleteParams.Status = status;
            break;

        default:
            NT_ASSERT(FALSE);
            break;
        };

        m_pAdapter->IndicateStatus(
            m_NdisPortNumber,
            m_SendCompleteIndicationType,
            m_OidRequestId,
            pIndicationBuffer,
            indicationLength );
    }
}

NDIS_STATUS CP2PSendActionFrameResponseJob::QueryNextCommandInfo(
    _In_ ULONG ReservedHeaderLength,
    _Inout_ WFC_DEVICE_COMMAND_TOKEN* pCommandToken,
    _Out_ WFC_DEVICE_COMMAND_TYPE* pCommandType,
    _Out_ UINT32* pMessageID,
    _Out_ ULONG* pCommandBufferLength,
    _Outptr_result_buffer_(*pCommandBufferLength) BYTE** ppCommandBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    *pCommandType = WfcDeviceCommandTypeInvalid;
    *pMessageID = 0;

    //
    // Optimization for "Join Existing GO" Accept vs "GO Negotiation"
    // Provision Discovery
    //
    // If there was a group ID in the previous provision discovery
    // request, it means we are probably the GO. This optimization is
    // here to reduce task completion delay so we can continue pairing ASAP.
    //
    // If there is a group ID, then there is some chance of a GON
    // Request arriving immediately after this frame is acknowledged,
    // so waiting around can reduce time to convergence for the GON
    // Request frame.
    //
    if ( m_ResponseParameters.ResponseParams.ResponseFrameType == 
         WDI_P2P_ACTION_FRAME_PROVISION_DISCOVERY_RESPONSE )
    {
        ULONG groupNameLength;
        WFD_OTA_GROUP_ID groupID;

        if ( NT_SUCCESS(
                 WFDGetGroupID(
                     m_ResponseParameters.PrecedingFrameInformation.FrameIEs.pElements,
                     m_ResponseParameters.PrecedingFrameInformation.FrameIEs.ElementCount,
                     &groupNameLength,
                     &groupID ) ) )
        {
            m_ResponseParameters.ResponseParams.PostACKDwellTime = 0;
        }
        else
        {
            m_ResponseParameters.ResponseParams.PostACKDwellTime = 300;
        }
    }

    status = GenerateWdiTaskP2pSendResponseActionFrame(
        &m_ResponseParameters,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        ppCommandBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't generate send response action frame settings.");
        goto exit;
    }

    *pCommandToken = NULL;
    *pCommandType = WfcDeviceCommandTypeTask;
    *pMessageID = WDI_TASK_P2P_SEND_RESPONSE_ACTION_FRAME;

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CP2PSendActionFrameResponseJob::HandleCommandCompletion(
    _In_ WFC_DEVICE_COMMAND_TOKEN,
    _In_ NDIS_STATUS OperationStatus,
    _In_ NDIS_STATUS WiFiStatus,
    _In_ ULONG CommandCompletionBufferLength,
    _In_reads_bytes_(CommandCompletionBufferLength) BYTE* pCommandCompletionBuffer,
    _Out_ bool* pQueryNextCommand )
{
    NDIS_STATUS status = OperationStatus;
    WDI_INDICATION_P2P_SEND_RESPONSE_ACTION_FRAME_COMPLETE_PARAMETERS completionParameters;
    ULONG requiredIndicationSize;
    BYTE* allocatedSpace = NULL;
    BYTE* pIndicationBuffer = NULL;
    ULONG offsetToIes;

    TraceActEntry();

    *pQueryNextCommand = false;

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Send action frame task failed - OperationStatus.");
        goto exit;
    }

    if ( WiFiStatus != NDIS_STATUS_SUCCESS )
    {
        status = WiFiStatus;
        WFCActErrorStatus("Send action frame task failed - WiFiStatus.");
        goto exit;
    }

    status = ParseWdiIndicationP2pSendResponseActionFrameComplete(
        CommandCompletionBufferLength - sizeof(WFC_MESSAGE_METADATA),
        pCommandCompletionBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &completionParameters );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't parse send complete parameters.");
        goto exit;
    }

    switch(m_ResponseParameters.ResponseParams.ResponseFrameType)
    {
    case WDI_P2P_ACTION_FRAME_PROVISION_DISCOVERY_RESPONSE:
        pIndicationBuffer = (BYTE*)&m_ProvDiscResponseSendCompleteParams;
        requiredIndicationSize = sizeof(m_ProvDiscResponseSendCompleteParams);
        offsetToIes = requiredIndicationSize;
        m_ProvDiscResponseSendCompleteParams.Status = NDIS_STATUS_SUCCESS;
        m_ProvDiscResponseSendCompleteParams.uIEsOffset = offsetToIes;
        m_ProvDiscResponseSendCompleteParams.uIEsLength = completionParameters.SendActionFrameResult.FrameIEs.ElementCount;
        break;

    case WDI_P2P_ACTION_FRAME_INVITATION_RESPONSE:
        pIndicationBuffer = (BYTE*)&m_InvitationResponseSendCompleteParams;
        requiredIndicationSize = sizeof(m_InvitationResponseSendCompleteParams);
        offsetToIes = requiredIndicationSize;
        m_InvitationResponseSendCompleteParams.Status = NDIS_STATUS_SUCCESS;
        m_InvitationResponseSendCompleteParams.uIEsOffset = offsetToIes;
        m_InvitationResponseSendCompleteParams.uIEsLength = completionParameters.SendActionFrameResult.FrameIEs.ElementCount;
        break;

    case WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_RESPONSE:
        pIndicationBuffer = (BYTE*)&m_GONResponseSendCompleteParams;
        requiredIndicationSize = sizeof(m_GONResponseSendCompleteParams);
        offsetToIes = requiredIndicationSize;
        m_GONResponseSendCompleteParams.Status = NDIS_STATUS_SUCCESS;
        m_GONResponseSendCompleteParams.uIEsOffset = offsetToIes;
        m_GONResponseSendCompleteParams.uIEsLength = completionParameters.SendActionFrameResult.FrameIEs.ElementCount;
        break;

    case WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_CONFIRM:
        pIndicationBuffer = (BYTE*)&m_GONConfirmationSendCompleteParams;
        requiredIndicationSize = sizeof(m_GONConfirmationSendCompleteParams);
        offsetToIes = requiredIndicationSize;
        m_GONConfirmationSendCompleteParams.Status = NDIS_STATUS_SUCCESS;
        m_GONConfirmationSendCompleteParams.uIEsOffset = offsetToIes;
        m_GONConfirmationSendCompleteParams.uIEsLength = completionParameters.SendActionFrameResult.FrameIEs.ElementCount;
        break;

    default:
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_NOT_SUPPORTED;
        goto exit;
    };

    if ( !NT_SUCCESS(
             RtlULongAdd(
                 requiredIndicationSize,
                 completionParameters.SendActionFrameResult.FrameIEs.ElementCount,
                 &requiredIndicationSize) ) ) 
    {
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        WFCActErrorStatus("Overflowed when attempting to calculate required indication size.");
        goto exit;
    }
    
    allocatedSpace = new BYTE[requiredIndicationSize];

    if ( allocatedSpace  == NULL )
    {
        status = NDIS_STATUS_RESOURCES;
        WFCActErrorStatus("Couldn't allocate completion parameters structure.");
        goto exit;
    }

    RtlCopyMemory(
        allocatedSpace,
        pIndicationBuffer,
        offsetToIes );

    if ( completionParameters.SendActionFrameResult.FrameIEs.ElementCount > 0 )
    {
        RtlCopyMemory(
            allocatedSpace + offsetToIes,
            completionParameters.SendActionFrameResult.FrameIEs.pElements,
            completionParameters.SendActionFrameResult.FrameIEs.ElementCount );
    }

    m_pSendCompleteIndication = allocatedSpace;
    m_SendCompleteIndicationLength = requiredIndicationSize;
    allocatedSpace = NULL;

exit:
    if ( allocatedSpace != NULL )
    {
        delete [] allocatedSpace;
    }

    TraceActExit(status);
    return status;
}


void CP2PSendActionFrameResponseJob::OnNotifyJobCompletion(
    _In_ WFC_DEVICE_COMMAND_TOKEN,
    _In_ NDIS_STATUS OperationStatus )
{
    WFCActInfo("Send action frame job completed with 0x%x", OperationStatus);
}

NDIS_STATUS CP2PSendActionFrameResponseJob::CopyExtraIEsToByteBlob(
    _In_ WDI_BYTE_BLOB* pTarget,
    _In_ ULONG DataLength,
    _In_reads_bytes_(DataLength) BYTE* pExtraIEs )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    //
    // OID Request will disappear once InitializeFromOidRequest
    // returns, so we need to make a copy of the vendor IEs to send down.
    //
    // This needs to happen here because code cannot fill in the
    // entire request right now. The structure is only partially
    // filled when the request comes in, and then device descriptor is
    // filled in later.
    //
    pTarget->pElements = new BYTE[DataLength];

    if ( pTarget->pElements == NULL )
    {
        WFCActErrorStatus(
            "Couldn't allocate buffer to copy additional IEs to local storage. (%d bytes requested)",
            DataLength );
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    //
    // Set this flag for automatic clean-up when the object is destroyed
    //
    pTarget->MemoryInternallyAllocated = true;
    pTarget->ElementCount = DataLength;

    RtlCopyMemory(
        pTarget->pElements,
        pExtraIEs,
        DataLength );

exit:
    TraceActExit(status);
    return status;
}


NDIS_STATUS CP2PSendActionFrameResponseJob::PopulateGONegotiationResponseParameters(
    _In_ ULONG RequestLength,
    _In_reads_bytes_(RequestLength) DOT11_SEND_GO_NEGOTIATION_RESPONSE_PARAMETERS* pResponseParams )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    if ( RequestLength < sizeof(*pResponseParams) )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    //
    // Fill in static parameters
    //
    m_ResponseParameters.ResponseParams.ResponseFrameType = WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_RESPONSE;

    RtlCopyMemory(
        &m_ResponseParameters.ResponseParams.PeerDeviceAddress,
        pResponseParams->PeerDeviceAddress,
        sizeof(m_ResponseParameters.ResponseParams.PeerDeviceAddress));

    m_ResponseParameters.ResponseParams.DialogToken =
        pResponseParams->DialogToken;

    m_ResponseParameters.ResponseParams.SendTimeout =
        pResponseParams->uSendTimeout;
    
    //
    // If status is success, we should wait around a bit for the
    // confirmation frame, otherwise we should go back to what we were
    // doing before ASAP.
    //
    if ( pResponseParams->Status == 0 )
    {
        m_ResponseParameters.ResponseParams.PostACKDwellTime = 125;
    }
    else
    {
        m_ResponseParameters.ResponseParams.PostACKDwellTime = 0;
    }

    //
    // Fill in Specific Request Parameters
    //
    m_ResponseParameters.Optional.GONegotiationResponseInfo_IsPresent = 1;

    m_ResponseParameters.GONegotiationResponseInfo.ResponseParams.StatusCode =
        (UINT8)pResponseParams->Status;

    m_ResponseParameters.GONegotiationResponseInfo.ResponseParams.GOIntent =
        pResponseParams->GroupOwnerIntent.Intent;

    m_ResponseParameters.GONegotiationResponseInfo.ResponseParams.TieBreaker =
        pResponseParams->GroupOwnerIntent.TieBreaker ? 1 : 0;

    m_ResponseParameters.GONegotiationResponseInfo.ResponseParams.GOConfigTimeout =
        pResponseParams->MinimumConfigTimeout.GOTimeout;

    m_ResponseParameters.GONegotiationResponseInfo.ResponseParams.ClientConfigTimeout =
        pResponseParams->MinimumConfigTimeout.ClientTimeout;

    RtlCopyMemory(
        &m_ResponseParameters.GONegotiationResponseInfo.ResponseParams.IntendedInterfaceAddress,
        &pResponseParams->IntendedInterfaceAddress,
        sizeof(m_ResponseParameters.GONegotiationResponseInfo.ResponseParams.IntendedInterfaceAddress));

    m_ResponseParameters.GONegotiationResponseInfo.ResponseParams.GroupCapability =
        (UINT8)pResponseParams->GroupCapability;
    m_ResponseParameters.GONegotiationResponseInfo.ResponseParams.OSSetGroupCapabilityBitmask =
        (DOT11_WFD_GROUP_CAPABILITY_PERSISTENT_GROUP |
         DOT11_WFD_GROUP_CAPABILITY_EAPOL_KEY_IP_ADDRESS_ALLOCATION_SUPPORTED);

    WFCActInfo("[GONegotiationResponse]: Group:OSSetCapabilities = <0x%08x : 0x%08x>",
        m_ResponseParameters.GONegotiationResponseInfo.ResponseParams.GroupCapability,
        m_ResponseParameters.GONegotiationResponseInfo.ResponseParams.OSSetGroupCapabilityBitmask);

    if ( pResponseParams->bUseGroupID )
    {
        m_ResponseParameters.GONegotiationResponseInfo.Optional.GroupID_IsPresent = 1;

        RtlCopyMemory(
            &m_SSIDStorage,
            &pResponseParams->GroupID.SSID,
            sizeof(m_SSIDStorage) );
        m_ResponseParameters.GONegotiationResponseInfo.GroupID.GroupSSID.pElements = m_SSIDStorage.ucSSID;
        m_ResponseParameters.GONegotiationResponseInfo.GroupID.GroupSSID.ElementCount = m_SSIDStorage.uSSIDLength;
        RtlCopyMemory(
            &m_ResponseParameters.GONegotiationResponseInfo.GroupID.DeviceAddress,
            pResponseParams->GroupID.DeviceAddress,
            sizeof(m_ResponseParameters.GONegotiationResponseInfo.GroupID.DeviceAddress));        
    }


    //
    // Fill in vendor IEs
    //
    if ( pResponseParams->uIEsLength > 0 )
    {
        ULONG bufferLengthCheck;

        if ( !NT_SUCCESS(
                 RtlULongAdd(
                     pResponseParams->uIEsLength,
                     pResponseParams->uIEsOffset,
                     &bufferLengthCheck ) ) ||
             bufferLengthCheck > RequestLength )
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("Bad IEs information in buffer provided from OS.");
            goto exit;
        }

        m_ResponseParameters.Optional.VendorIEs_IsPresent = 1;
        status = CopyExtraIEsToByteBlob(
            &m_ResponseParameters.VendorIEs,
            pResponseParams->uIEsLength,
            (BYTE*)pResponseParams + pResponseParams->uIEsOffset );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            goto exit;
        }
    }

    //
    // Fill in default completion parameters
    //
    ASSIGN_NDIS_OBJECT_HEADER(
        m_GONResponseSendCompleteParams.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_GO_NEGOTIATION_RESPONSE_SEND_COMPLETE_PARAMETERS_REVISION_1,
        DOT11_SIZEOF_GO_NEGOTIATION_RESPONSE_SEND_COMPLETE_PARAMETERS_REVISION_1 );

    RtlCopyMemory(
        &m_GONResponseSendCompleteParams.PeerDeviceAddress,
        pResponseParams->PeerDeviceAddress,
        sizeof(m_GONResponseSendCompleteParams.PeerDeviceAddress) );

    m_GONResponseSendCompleteParams.DialogToken =
        pResponseParams->DialogToken;

exit:
    TraceActExit(status);
    return status;
}


NDIS_STATUS CP2PSendActionFrameResponseJob::PopulateGONegotiationConfirmationParameters(
    _In_ ULONG RequestLength,
    _In_reads_bytes_(RequestLength) DOT11_SEND_GO_NEGOTIATION_CONFIRMATION_PARAMETERS* pResponseParams )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    if ( RequestLength < sizeof(*pResponseParams) )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    //
    // Fill in static parameters
    //
    m_ResponseParameters.ResponseParams.ResponseFrameType = WDI_P2P_ACTION_FRAME_GO_NEGOTIATION_CONFIRM;

    RtlCopyMemory(
        &m_ResponseParameters.ResponseParams.PeerDeviceAddress,
        pResponseParams->PeerDeviceAddress,
        sizeof(m_ResponseParameters.ResponseParams.PeerDeviceAddress));

    m_ResponseParameters.ResponseParams.DialogToken =
        pResponseParams->DialogToken;

    m_ResponseParameters.ResponseParams.SendTimeout =
        pResponseParams->uSendTimeout;
    
    m_ResponseParameters.ResponseParams.PostACKDwellTime = 0;

    //
    // Fill in Specific Request Parameters
    //
    m_ResponseParameters.Optional.GONegotiationConfirmationInfo_IsPresent = 1;

    m_ResponseParameters.GONegotiationConfirmationInfo.ConfirmationParams.StatusCode =
        (UINT8)pResponseParams->Status;

    m_ResponseParameters.GONegotiationConfirmationInfo.ConfirmationParams.GroupCapability =
        (UINT8)pResponseParams->GroupCapability;
    m_ResponseParameters.GONegotiationConfirmationInfo.ConfirmationParams.OSSetGroupCapabilityBitmask =
        (DOT11_WFD_GROUP_CAPABILITY_PERSISTENT_GROUP |
         DOT11_WFD_GROUP_CAPABILITY_EAPOL_KEY_IP_ADDRESS_ALLOCATION_SUPPORTED);

    WFCActInfo("[GONegotiationConfirmation]: Group:OSSetCapabilities = <0x%08x : 0x%08x>",
        m_ResponseParameters.GONegotiationConfirmationInfo.ConfirmationParams.GroupCapability,
        m_ResponseParameters.GONegotiationConfirmationInfo.ConfirmationParams.OSSetGroupCapabilityBitmask);

    if ( pResponseParams->bUseGroupID )
    {
        m_ResponseParameters.GONegotiationConfirmationInfo.Optional.GroupID_IsPresent = 1;

        RtlCopyMemory(
            &m_SSIDStorage,
            &pResponseParams->GroupID.SSID,
            sizeof(m_SSIDStorage) );
        m_ResponseParameters.GONegotiationConfirmationInfo.GroupID.GroupSSID.pElements = m_SSIDStorage.ucSSID;
        m_ResponseParameters.GONegotiationConfirmationInfo.GroupID.GroupSSID.ElementCount = m_SSIDStorage.uSSIDLength;
        RtlCopyMemory(
            &m_ResponseParameters.GONegotiationConfirmationInfo.GroupID.DeviceAddress,
            pResponseParams->GroupID.DeviceAddress,
            sizeof(m_ResponseParameters.GONegotiationConfirmationInfo.GroupID.DeviceAddress));        
    }


    //
    // Fill in vendor IEs
    //
    if ( pResponseParams->uIEsLength > 0 )
    {
        ULONG bufferLengthCheck;

        if ( !NT_SUCCESS(
                 RtlULongAdd(
                     pResponseParams->uIEsLength,
                     pResponseParams->uIEsOffset,
                     &bufferLengthCheck ) ) ||
             bufferLengthCheck > RequestLength )
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("Bad IEs information in buffer provided from OS.");
            goto exit;
        }

        m_ResponseParameters.Optional.VendorIEs_IsPresent = 1;
        status = CopyExtraIEsToByteBlob(
            &m_ResponseParameters.VendorIEs,
            pResponseParams->uIEsLength,
            (BYTE*)pResponseParams + pResponseParams->uIEsOffset );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            goto exit;
        }
    }

    if (pResponseParams->Header.Revision == DOT11_SEND_GO_NEGOTIATION_CONFIRMATION_PARAMETERS_REVISION_2 &&
        RequestLength >= DOT11_SIZEOF_SEND_GO_NEGOTIATION_CONFIRMATION_PARAMETERS_REVISION_2)
    {
        PDOT11_SEND_GO_NEGOTIATION_CONFIRMATION_PARAMETERS_V2 pV2ResponseParams = (PDOT11_SEND_GO_NEGOTIATION_CONFIRMATION_PARAMETERS_V2)pResponseParams;
        if (pV2ResponseParams->bUseSpecifiedRemoteListenChannel)
        {
            m_ResponseParameters.GONegotiationConfirmationInfo.Optional.RemoteListenChannel_IsPresent = TRUE;

            RtlCopyMemory(m_ResponseParameters.GONegotiationConfirmationInfo.RemoteListenChannel.CountryOrRegionString,
                &pV2ResponseParams->RemoteListenChannel.CountryRegionString,
                sizeof(DOT11_COUNTRY_OR_REGION_STRING));
            m_ResponseParameters.GONegotiationConfirmationInfo.RemoteListenChannel.OperatingClass = pV2ResponseParams->RemoteListenChannel.OperatingClass;
            m_ResponseParameters.GONegotiationConfirmationInfo.RemoteListenChannel.ChannelNumber = pV2ResponseParams->RemoteListenChannel.ChannelNumber;
        }
    }

    //
    // Fill in default completion parameters
    //
    ASSIGN_NDIS_OBJECT_HEADER(
        m_GONConfirmationSendCompleteParams.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_GO_NEGOTIATION_CONFIRMATION_SEND_COMPLETE_PARAMETERS_REVISION_1,
        DOT11_SIZEOF_GO_NEGOTIATION_CONFIRMATION_SEND_COMPLETE_PARAMETERS_REVISION_1 );

    RtlCopyMemory(
        &m_GONConfirmationSendCompleteParams.PeerDeviceAddress,
        pResponseParams->PeerDeviceAddress,
        sizeof(m_GONConfirmationSendCompleteParams.PeerDeviceAddress) );

    m_GONConfirmationSendCompleteParams.DialogToken =
        pResponseParams->DialogToken;

exit:
    TraceActExit(status);
    return status;
}


NDIS_STATUS CP2PSendActionFrameResponseJob::PopulateInvitationResponseParameters(
    _In_ ULONG RequestLength,
    _In_reads_bytes_(RequestLength) DOT11_SEND_INVITATION_RESPONSE_PARAMETERS* pResponseParams )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    if ( RequestLength < sizeof(*pResponseParams) )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    //
    // Fill in static parameters
    //
    m_ResponseParameters.ResponseParams.ResponseFrameType = WDI_P2P_ACTION_FRAME_INVITATION_RESPONSE;

    RtlCopyMemory(
        &m_ResponseParameters.ResponseParams.PeerDeviceAddress,
        pResponseParams->ReceiverDeviceAddress,
        sizeof(m_ResponseParameters.ResponseParams.PeerDeviceAddress));

    m_ResponseParameters.ResponseParams.DialogToken =
        pResponseParams->DialogToken;

    m_ResponseParameters.ResponseParams.SendTimeout =
        pResponseParams->uSendTimeout;
    
    // TODO: Define constant for post ACK dwell time
    m_ResponseParameters.ResponseParams.PostACKDwellTime = 0;

    //
    // Fill in Specific Request Parameters
    //
    m_ResponseParameters.Optional.InvitationResponseInfo_IsPresent = 1;

    m_ResponseParameters.InvitationResponseInfo.ResponseParams.StatusCode =
        (UINT8)pResponseParams->Status;

    m_ResponseParameters.InvitationResponseInfo.ResponseParams.GOConfigTimeout =
        pResponseParams->MinimumConfigTimeout.GOTimeout;

    m_ResponseParameters.InvitationResponseInfo.ResponseParams.ClientConfigTimeout =
        pResponseParams->MinimumConfigTimeout.ClientTimeout;


    if ( pResponseParams->bUseGroupBSSID )
    {
        m_ResponseParameters.InvitationResponseInfo.Optional.GroupBSSID_IsPresent = 1;
        RtlCopyMemory(
            &m_ResponseParameters.InvitationResponseInfo.GroupBSSID,
            pResponseParams->GroupBSSID,
            sizeof(m_ResponseParameters.InvitationResponseInfo.GroupBSSID) );
    }

    if ( pResponseParams->bUseSpecifiedOperatingChannel )
    {
        m_ResponseParameters.InvitationResponseInfo.Optional.OperatingChannel_IsPresent = 1;
        m_ResponseParameters.InvitationResponseInfo.Optional.OperatingChannel_IsPresent = 1;
        m_ResponseParameters.InvitationResponseInfo.OperatingChannel.OperatingClass = pResponseParams->OperatingChannel.OperatingClass;
        m_ResponseParameters.InvitationResponseInfo.OperatingChannel.ChannelNumber = pResponseParams->OperatingChannel.ChannelNumber;
        C_ASSERT( sizeof( m_ResponseParameters.InvitationResponseInfo.OperatingChannel.CountryOrRegionString ) == sizeof( pResponseParams->OperatingChannel.CountryRegionString ) );
        RtlCopyMemory(
            m_ResponseParameters.InvitationResponseInfo.OperatingChannel.CountryOrRegionString,
            pResponseParams->OperatingChannel.CountryRegionString,
            sizeof( pResponseParams->OperatingChannel.CountryRegionString ) );
    }

    //
    // Fill in vendor IEs
    //
    if ( pResponseParams->uIEsLength > 0 )
    {
        ULONG bufferLengthCheck;

        if ( !NT_SUCCESS(
                 RtlULongAdd(
                     pResponseParams->uIEsLength,
                     pResponseParams->uIEsOffset,
                     &bufferLengthCheck ) ) ||
             bufferLengthCheck > RequestLength )
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("Bad IEs information in buffer provided from OS.");
            goto exit;
        }

        m_ResponseParameters.Optional.VendorIEs_IsPresent = 1;
        status = CopyExtraIEsToByteBlob(
            &m_ResponseParameters.VendorIEs,
            pResponseParams->uIEsLength,
            (BYTE*)pResponseParams + pResponseParams->uIEsOffset );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            goto exit;
        }
    }

    //
    // Fill in default completion parameters
    //
    ASSIGN_NDIS_OBJECT_HEADER(
        m_InvitationResponseSendCompleteParams.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_INVITATION_RESPONSE_SEND_COMPLETE_PARAMETERS_REVISION_1,
        DOT11_SIZEOF_INVITATION_RESPONSE_SEND_COMPLETE_PARAMETERS_REVISION_1 );

    RtlCopyMemory(
        &m_InvitationResponseSendCompleteParams.ReceiverDeviceAddress,
        pResponseParams->ReceiverDeviceAddress,
        sizeof(m_InvitationResponseSendCompleteParams.ReceiverDeviceAddress) );

    m_InvitationResponseSendCompleteParams.DialogToken =
        pResponseParams->DialogToken;

exit:
    TraceActExit(status);
    return status;
}


NDIS_STATUS CP2PSendActionFrameResponseJob::PopulateProvisionDiscoveryResponseParameters(
    _In_ ULONG RequestLength,
    _In_reads_bytes_(RequestLength) NDIS_OBJECT_HEADER *pNdisHeader )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    DOT11_MAC_ADDRESS *pReceiverDeviceAddress;
    DOT11_DIALOG_TOKEN DialogToken;
    ULONG uIEsOffset;
    ULONG uIEsLength;

    TraceActEntry();

    if ( RequestLength < sizeof(DOT11_SEND_PROVISION_DISCOVERY_RESPONSE_PARAMETERS) )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    switch (pNdisHeader->Revision)
    {
        case (DOT11_SEND_PROVISION_DISCOVERY_RESPONSE_PARAMETERS_REVISION_1):
        {
            DOT11_SEND_PROVISION_DISCOVERY_RESPONSE_PARAMETERS* pResponseParams =
                (DOT11_SEND_PROVISION_DISCOVERY_RESPONSE_PARAMETERS*) pNdisHeader;

            RtlCopyMemory(
                &m_ResponseParameters.ResponseParams.PeerDeviceAddress,
                pResponseParams->ReceiverDeviceAddress,
                sizeof(m_ResponseParameters.ResponseParams.PeerDeviceAddress));
            m_ResponseParameters.ResponseParams.DialogToken = pResponseParams->DialogToken;
            m_ResponseParameters.ResponseParams.SendTimeout = pResponseParams->uSendTimeout;
            DialogToken = pResponseParams->DialogToken;
            uIEsLength = pResponseParams->uIEsLength;
            uIEsOffset = pResponseParams->uIEsOffset;
            pReceiverDeviceAddress = &pResponseParams->ReceiverDeviceAddress;

            break;
        }

        case (DOT11_SEND_PROVISION_DISCOVERY_RESPONSE_PARAMETERS_REVISION_2):
        {
            DOT11_SEND_PROVISION_DISCOVERY_RESPONSE_PARAMETERS_V2* pResponseParams =
                (DOT11_SEND_PROVISION_DISCOVERY_RESPONSE_PARAMETERS_V2*) pNdisHeader;

            RtlCopyMemory(
                &m_ResponseParameters.ResponseParams.PeerDeviceAddress,
                pResponseParams->ReceiverDeviceAddress,
                sizeof(m_ResponseParameters.ResponseParams.PeerDeviceAddress));
            m_ResponseParameters.ResponseParams.DialogToken = pResponseParams->DialogToken;
            m_ResponseParameters.ResponseParams.SendTimeout = pResponseParams->uSendTimeout;
            DialogToken = pResponseParams->DialogToken;
            uIEsLength = pResponseParams->uIEsLength;
            uIEsOffset = pResponseParams->uIEsOffset;
            pReceiverDeviceAddress = &pResponseParams->ReceiverDeviceAddress;

            //
            // If the request did not specify any ServiceAttributes, we are done
            //
            if (!pResponseParams->bEncodeServiceAttributes)
            {
                break;
            }

            m_ResponseParameters.Optional.ProvisionDiscoveryResponseInfo_IsPresent = 1;

            m_ResponseParameters.ProvisionDiscoveryResponseInfo.Optional.ServiceAttributes_IsPresent = 1;

            m_ResponseParameters.ProvisionDiscoveryResponseInfo.ServiceAttributes.StatusCode =
                (UINT8)pResponseParams->Status;

            RtlCopyMemory(
                &m_ResponseParameters.ProvisionDiscoveryResponseInfo.ServiceAttributes.IntendedInterfaceAddress,
                &pResponseParams->IntendedInterfaceAddress,
                sizeof(WDI_MAC_ADDRESS));

            m_ResponseParameters.ProvisionDiscoveryResponseInfo.ServiceAttributes.ConnectionCapability =
                pResponseParams->ConnectionCapability;
            m_ResponseParameters.ProvisionDiscoveryResponseInfo.ServiceAttributes.FeatureCapability =
                pResponseParams->FeatureCapability;

            m_ResponseParameters.ProvisionDiscoveryResponseInfo.ServiceAttributes.AdvertisementID =
                pResponseParams->AdvertisementID.AdvertisementID;

            RtlCopyMemory(
                &m_ResponseParameters.ProvisionDiscoveryResponseInfo.ServiceAttributes.ServiceAddress,
                &pResponseParams->AdvertisementID.ServiceAddress,
                sizeof(WDI_MAC_ADDRESS));

            m_ResponseParameters.ProvisionDiscoveryResponseInfo.ServiceAttributes.SessionID =
                pResponseParams->SessionID.SessionID;

            RtlCopyMemory(
                &m_ResponseParameters.ProvisionDiscoveryResponseInfo.ServiceAttributes.SessionAddress,
                &pResponseParams->SessionID.SessionAddress,
                sizeof(WDI_MAC_ADDRESS));

            m_ResponseParameters.ProvisionDiscoveryResponseInfo.ServiceAttributes.GOConfigTimeout =
                pResponseParams->ConfigurationTimeout.GOTimeout;

            m_ResponseParameters.ProvisionDiscoveryResponseInfo.ServiceAttributes.ClientConfigTimeout =
                pResponseParams->ConfigurationTimeout.ClientTimeout;

#ifdef OLDER_OS_COMPAT
            if ((pNdisHeader->Size >=
                 RTL_SIZEOF_THROUGH_FIELD(DOT11_SEND_PROVISION_DISCOVERY_RESPONSE_PARAMETERS_V2, GroupID)) &&
                (pResponseParams->bUseGroupID))
#else
            if ( pResponseParams->bUseGroupID )
#endif  // OLDER_OS_COMPAT
            {
                m_ResponseParameters.ProvisionDiscoveryResponseInfo.Optional.GroupID_IsPresent = 1;

                m_ResponseParameters.ProvisionDiscoveryResponseInfo.GroupID.GroupSSID.pElements = pResponseParams->GroupID.SSID.ucSSID;
                m_ResponseParameters.ProvisionDiscoveryResponseInfo.GroupID.GroupSSID.ElementCount = pResponseParams->GroupID.SSID.uSSIDLength;
                RtlCopyMemory(
                    &m_ResponseParameters.ProvisionDiscoveryResponseInfo.GroupID.DeviceAddress,
                    pResponseParams->GroupID.DeviceAddress,
                    sizeof(m_ResponseParameters.ProvisionDiscoveryResponseInfo.GroupID.DeviceAddress));        
            }

            if ( pResponseParams->bUsePersistentGroup )
            {
                m_ResponseParameters.ProvisionDiscoveryResponseInfo.Optional.PersistentGroupID_IsPresent = 1;
                m_ResponseParameters.ProvisionDiscoveryResponseInfo.ServiceAttributes.UsePersistentGroup = 1;

                RtlCopyMemory(
                    &m_PersistedGroupSSID,
                    &pResponseParams->PersistentGroupID.SSID,
                    sizeof(m_PersistedGroupSSID));
                m_ResponseParameters.ProvisionDiscoveryResponseInfo.PersistentGroupID.GroupSSID.pElements = m_PersistedGroupSSID.ucSSID;
                m_ResponseParameters.ProvisionDiscoveryResponseInfo.PersistentGroupID.GroupSSID.ElementCount = m_PersistedGroupSSID.uSSIDLength;
                RtlCopyMemory(
                    &m_ResponseParameters.ProvisionDiscoveryResponseInfo.PersistentGroupID.DeviceAddress,
                    pResponseParams->PersistentGroupID.DeviceAddress,
                    sizeof(m_ResponseParameters.ProvisionDiscoveryResponseInfo.PersistentGroupID.DeviceAddress));        
            }

            if ( pResponseParams->ServiceSessionInfo.uSessionInfoLength )
            {
                m_ResponseParameters.ProvisionDiscoveryResponseInfo.Optional.ServiceSessionInfo_IsPresent = 1;

                m_ResponseParameters.ProvisionDiscoveryResponseInfo.ServiceSessionInfo.SimpleAssign(
                    pResponseParams->ServiceSessionInfo.ucSessionInfo,
                    pResponseParams->ServiceSessionInfo.uSessionInfoLength);
            }

            if ( pResponseParams->bFollowOnProvisionDiscovery )
            {
                m_ResponseParameters.ProvisionDiscoveryResponseInfo.ServiceAttributes.FollowOnProvisionDiscovery = 1;
            }

#ifdef OLDER_OS_COMPAT
            if ((pNdisHeader->Size >=
                RTL_SIZEOF_THROUGH_FIELD(DOT11_SEND_PROVISION_DISCOVERY_RESPONSE_PARAMETERS_V2, GroupCapability)))
#endif  // OLDER_OS_COMPAT
            {
                m_ResponseParameters.ProvisionDiscoveryResponseInfo.ResponseParams.GroupCapability =
                    (UINT8)pResponseParams->GroupCapability;
                m_ResponseParameters.ProvisionDiscoveryResponseInfo.ResponseParams.OSSetGroupCapabilityBitmask =
                    (DOT11_WFD_GROUP_CAPABILITY_PERSISTENT_GROUP |
                    DOT11_WFD_GROUP_CAPABILITY_EAPOL_KEY_IP_ADDRESS_ALLOCATION_SUPPORTED);

                WFCActInfo("[ProvisionDiscoveryResponse]: Group:OSSetCapabilities = <0x%08x : 0x%08x>",
                    m_ResponseParameters.ProvisionDiscoveryResponseInfo.ResponseParams.GroupCapability,
                    m_ResponseParameters.ProvisionDiscoveryResponseInfo.ResponseParams.OSSetGroupCapabilityBitmask);
            }
            break;
        }

        default:
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("Invalid Provision Discovery Request version %d.", pNdisHeader->Revision);
            goto exit;
        }
    }


    //
    // Fill in static parameters
    //
    m_ResponseParameters.ResponseParams.ResponseFrameType = WDI_P2P_ACTION_FRAME_PROVISION_DISCOVERY_RESPONSE;

    //
    // NOTE: There is an optimization for this in
    // CP2PSendActionFrameResponseJob::QueryNextCommandInfo
    //
    m_ResponseParameters.ResponseParams.PostACKDwellTime = 125;

    //
    // Fill in vendor IEs
    //
    if ( uIEsLength > 0 )
    {
        ULONG bufferLengthCheck;

        if ( !NT_SUCCESS( RtlULongAdd( uIEsLength, uIEsOffset, &bufferLengthCheck ) ) ||
             bufferLengthCheck > RequestLength )
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("Bad IEs information in buffer provided from OS.");
            goto exit;
        }

        m_ResponseParameters.Optional.VendorIEs_IsPresent = 1;
        status = CopyExtraIEsToByteBlob(
            &m_ResponseParameters.VendorIEs,
            uIEsLength,
            ((BYTE*)pNdisHeader) + uIEsOffset );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            goto exit;
        }
    }

    //
    // Fill in default completion parameters
    //
    ASSIGN_NDIS_OBJECT_HEADER(
        m_ProvDiscResponseSendCompleteParams.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_PROVISION_DISCOVERY_RESPONSE_SEND_COMPLETE_PARAMETERS_REVISION_1,
        DOT11_SIZEOF_PROVISION_DISCOVERY_RESPONSE_SEND_COMPLETE_PARAMETERS_REVISION_1 );

    RtlCopyMemory(
        &m_ProvDiscResponseSendCompleteParams.ReceiverDeviceAddress,
        pReceiverDeviceAddress,
        sizeof(m_ProvDiscResponseSendCompleteParams.ReceiverDeviceAddress) );

    m_ProvDiscResponseSendCompleteParams.DialogToken = DialogToken;

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CWfdChangeListenStateJob::Initialize(
    _In_ CAdapter* pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _In_ WDI_P2P_LISTEN_STATE NewListenState )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_SET_P2P_LISTEN_STATE_PARAMETERS listenStateParams;

    TraceActEntry();

    status = CJobBase::Initialize(
        pAdapter->GetEventQueue(),
        pAdapter->GetDeviceCommandScheduler(),
        pAdapter->GetActiveJobsList(),
        pAdapter->GetSerializedJobList(),
        WiFiJobP2PSetListenState,
        false,
        false,
        WiFiPriorityScopeAdapter,
        WiFiSerializedJobPriority_Lowest,
        pAdapter,
        pAdapter,
        NdisPortNumber );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't initialize CJobBase.");
        goto exit;
    }

    m_NewListenState = NewListenState;

    listenStateParams.ListenState = NewListenState;

    if ( m_pTlvBuffer != NULL )
    {
        delete [] m_pTlvBuffer;
        m_pTlvBuffer = NULL;
    }

    m_pAdapter = pAdapter;

    status = GenerateWdiSetP2pListenState(
        &listenStateParams,
        sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &m_TlvBufferLength,
        &m_pTlvBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't generate p2p listen state TLV buffer.");
        goto exit;
    }

exit:
    TraceActExit(status);
    return status;
}

void CWfdChangeListenStateJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    status = m_DeviceCommand.Initialize(
        get_PortId(),
        WDI_SET_P2P_LISTEN_STATE,
        m_TlvBufferLength,
        m_pTlvBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't initialize device command to send down.");
        goto exit;
    }

    status = QueueDeviceCommand(
        &m_DeviceCommand );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't queue device command to send down.");
        goto exit;
    }

exit:
    if ( status != NDIS_STATUS_SUCCESS )
    {
        CompleteJob(status);
    }

    TraceActExit(status);
}

void CWfdChangeListenStateJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;;
    NDIS_STATUS wifiStatus = NDIS_STATUS_FAILURE;

    TraceActEntry();

    NT_ASSERT(SenderObject == &m_DeviceCommand);

    UNREFERENCED_PARAMETER(SenderObject);

    if ( OperationStatus != NDIS_STATUS_SUCCESS ||
         CMessageHelper::GetStatusFromCommandResult(
             &m_DeviceCommand,
             &wifiStatus ) != NDIS_STATUS_SUCCESS )
    {
        status = NDIS_STATUS_FAILURE;
    }

    WFCActInfo(
        "Set listen state completed with 0x%x (wifiStatus 0x%x) -> Failure Code 0x%x",
        OperationStatus,
        wifiStatus,
        status );

    CompleteJob(status);
    TraceActExit(OperationStatus);
}

//
// GetNextDialogToken implementation
//

NDIS_STATUS CGetNextDialogTokenJob::GetDeviceCommandId(
    _Out_ ULONG* pCommandId
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    *pCommandId = WDI_GET_NEXT_ACTION_FRAME_DIALOG_TOKEN;

    TraceActExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CGetNextDialogTokenJob::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG CommandBufferLength,
    _In_reads_bytes_(CommandBufferLength) BYTE* pCommandBuffer,
    _In_ PVOID pInformationBuffer,
    _In_ UINT InformationBufferLength,
    _Inout_ PUINT pBytesWritten,
    _Inout_ PUINT pBytesNeeded)
{
    WDI_GET_NEXT_ACTION_FRAME_DIALOG_TOKEN_PARAMETERS  getNextDialogToken;
    DOT11_DIALOG_TOKEN *pDialogToken;

    TraceActEntry();

    NT_ASSERT(pInformationBuffer);

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        goto Exit;
    }

    // make sure that buffer is large enough.
    *pBytesNeeded = sizeof(DOT11_DIALOG_TOKEN);
    if (InformationBufferLength < *pBytesNeeded)
    {
        OperationStatus = NDIS_STATUS_BUFFER_OVERFLOW;
        goto Exit;
    }

    OperationStatus = ParseWdiGetNextActionFrameDialogToken(
        CommandBufferLength-sizeof(WFC_MESSAGE_METADATA),
        pCommandBuffer+sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &getNextDialogToken);
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("ParseWdiGetNextActionFrameDialogToken failed %d", OperationStatus);
        goto Exit;
    }

    pDialogToken = (DOT11_DIALOG_TOKEN *) pInformationBuffer;
    *pDialogToken = (DOT11_DIALOG_TOKEN) getNextDialogToken.NextDialogToken;

    *pBytesWritten = *pBytesNeeded;

Exit:
    TraceActExit(OperationStatus);
    return OperationStatus;
}

