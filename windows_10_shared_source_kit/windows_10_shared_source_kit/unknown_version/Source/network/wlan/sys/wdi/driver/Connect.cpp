#include "Precomp.hpp"
#include "Connect.hpp"
#include "Connect.tmh"

ROAM_CONTROL_PARAMETERS g_RoamControlParametersTable[] =
{
    // 0 = Never
    {
        0,          // BetterAPRoamLinkQualityThreshold
        false,      // BetterAPRoamOnNonEnterpriseNetworks
        0,          // BetterAPRoamReattemptCount
        100,        // APLinkQualitySignificantChangeBar
        6000,       // BetterAPRoamReattemptTimeGapSeconds
        6000,       // BetterAPRoamReattemptFailedBlockTimeSeconds
        false,      // ScanCancelOnGoodAPFoundPermitted.  Don't cancel scans.
        5,          // FiveGHzLowBoost              At FiveGHzBoostLinkQualityStart 5GHz APs will be boosted by 5
        50,         // FiveGHzBoostLinkQualityStart 5GHz boosting will occur at link quality 50
        80          // FiveGHzBoostLinkQualityEnd.  At 80 5GHz APs will be boosted by 100-80 (20)

    },
    // 1 = Media streaming mode roam
    {
        40,         // BetterAPRoamLinkQualityThreshold
        false,      // BetterAPRoamOnNonEnterpriseNetworks
        1,          // BetterAPRoamReattemptCount
        20,         // APLinkQualitySignificantChangeBar. Desired 80% drop, but this is approximate
        300,        // BetterAPRoamReattemptTimeGapSeconds
        1800,       // BetterAPRoamReattemptFailedBlockTimeSeconds
        true,       // ScanCancelOnGoodAPFoundPermitted.  Permit cancelling of scans.
        5,          // FiveGHzLowBoost              At FiveGHzBoostLinkQualityStart 5GHz APs will be boosted by 5
        50,         // FiveGHzBoostLinkQualityStart 5GHz boosting will occur at link quality 50
        80          // FiveGHzBoostLinkQualityEnd.  At 80 5GHz APs will be boosted by 100-80 (20)
    },
    // 2 = Normal roam
    {
        50,         // BetterAPRoamLinkQualityThreshold
        false,      // BetterAPRoamOnNonEnterpriseNetworks
        1,          // BetterAPRoamReattemptCount
        10,         // APLinkQualitySignificantChangeBar
        120,        // BetterAPRoamReattemptTimeGapSeconds
        1800,       // BetterAPRoamReattemptFailedBlockTimeSeconds
        true,       // ScanCancelOnGoodAPFoundPermitted.  Permit cancelling of scans.
        5,          // FiveGHzLowBoost              At FiveGHzBoostLinkQualityStart 5GHz APs will be boosted by 5
        50,         // FiveGHzBoostLinkQualityStart 5GHz boosting will occur at link quality 50
        80          // FiveGHzBoostLinkQualityEnd.  At 80 5GHz APs will be boosted by 100-80 (20)
    },
    // 3 = Super active
    {
        50,         // BetterAPRoamLinkQualityThreshold
        true,       // BetterAPRoamOnNonEnterpriseNetworks
        3,          // BetterAPRoamReattemptCount
        10,         // APLinkQualitySignificantChangeBar
        120,        // BetterAPRoamReattemptTimeGapSeconds
        6000,       // BetterAPRoamReattemptFailedBlockTimeSeconds
        false,      // ScanCancelOnGoodAPFoundPermitted.  Don't cancel scans.
        5,          // FiveGHzLowBoost              At FiveGHzBoostLinkQualityStart 5GHz APs will be boosted by 5
        50,         // FiveGHzBoostLinkQualityStart 5GHz boosting will occur at link quality 50
        80          // FiveGHzBoostLinkQualityEnd.  At 80 5GHz APs will be boosted by 100-80 (20)
    },
    // 4 = Manually Controlled parameters
    {
        0,          // BetterAPRoamLinkQualityThreshold
        false,      // BetterAPRoamOnNonEnterpriseNetworks
        1,          // BetterAPRoamReattemptCount
        100,        // APLinkQualitySignificantChangeBar 
        6000,       // BetterAPRoamReattemptTimeGapSeconds
        1800,       // BetterAPRoamReattemptFailedBlockTimeSeconds
        false,      // ScanCancelOnGoodAPFoundPermitted.  Don't cancel scans.
        5,          // FiveGHzLowBoost              At FiveGHzBoostLinkQualityStart 5GHz APs will be boosted by 5
        50,         // FiveGHzBoostLinkQualityStart 5GHz boosting will occur at link quality 50
        80          // FiveGHzBoostLinkQualityEnd.  At 80 5GHz APs will be boosted by 100-80 (20)
    }
};
NDIS_STATUS CConnectHelpers::PopulateConnectionParameters(
    _In_ CPortPropertyCache* pPortPropertyCache,
    _In_ WFC_CONNECTION_PROFILE_PARAMETERS* pConnectParameters
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    PDOT11_HESSID_LIST pHESSIDList = NULL;
    ULONG bufferSize;

    TraceEntry();

    status = pPortPropertyCache->GetPropertyList(WfcPortPropertySSID,
        &bufferSize,
        &pConnectParameters->SSIDList.ElementCount,
        (BYTE **)&pConnectParameters->SSIDList.pBuffer);
    if (status != NDIS_STATUS_SUCCESS)
    {
        goto exit;
    }

    status = pPortPropertyCache->GetPropertyBuffer(
        WfcPortPropertyHESSID,
        &bufferSize,
        (PBYTE *) &pHESSIDList);
    if (status != NDIS_STATUS_SUCCESS)
    {
        goto exit;
    }
    pConnectParameters->AccessNetworkType = pHESSIDList->AccessNetworkType;
    pConnectParameters->HESSIDList.ElementCount = (UINT16) pHESSIDList->NumberOfHESSIDs;
    pConnectParameters->HESSIDList.pBuffer = &pHESSIDList->HESSIDs[0];
    pConnectParameters->HotspotIndicationElement.ElementCount = (UINT16) pHESSIDList->HSIndicationElementLength;
    pConnectParameters->HotspotIndicationElement.pBuffer = ((PBYTE) pHESSIDList)+pHESSIDList->HSIndicationElementOffset;

    status = pPortPropertyCache->GetPropertyBoolean(
        WfcPortPropertySafeModeEnabled,
        &pConnectParameters->HostFIPSModeEnabled);
    if (status != NDIS_STATUS_SUCCESS)
    {
        goto exit;
    }

    status = pPortPropertyCache->GetPropertyList(WfcPortPropertyAuthAlgoList,
        &bufferSize,
        &pConnectParameters->AuthAlgoList.ElementCount,
        (BYTE **)&pConnectParameters->AuthAlgoList.pBuffer);
    if (status != NDIS_STATUS_SUCCESS)
        goto exit;

    status = pPortPropertyCache->GetPropertyList(WfcPortPropertyMulticastCipherAlgoList,
        &bufferSize,
        &pConnectParameters->MulticastCipherAlgoList.ElementCount,
        (BYTE **)&pConnectParameters->MulticastCipherAlgoList.pBuffer);
    if (status != NDIS_STATUS_SUCCESS)
        goto exit;

    status = pPortPropertyCache->GetPropertyList(WfcPortPropertyUnicastCipherAlgoList,
        &bufferSize,
        &pConnectParameters->UnicastCipherAlgoList.ElementCount,
        (BYTE **)&pConnectParameters->UnicastCipherAlgoList.pBuffer);
    if (status != NDIS_STATUS_SUCCESS)
        goto exit;

    if (!pConnectParameters->HostFIPSModeEnabled)
    {
        status = pPortPropertyCache->GetPropertyBoolean(WfcPortPropertyExcludeUnencrypted,
            &pConnectParameters->ExcludeUnencrypted);
        if (status != NDIS_STATUS_SUCCESS)
            goto exit;
    }
    else
    {
        pConnectParameters->ExcludeUnencrypted = FALSE;
    }

    status = pPortPropertyCache->GetPropertyBoolean(WfcPortPropertyHiddenNetwork,
        &pConnectParameters->HiddenNetwork);
    if (status != NDIS_STATUS_SUCCESS)
        goto exit;

    status = pPortPropertyCache->GetPropertyBoolean(WfcPortPropertyAnyBSSIDAllowed,
        &pConnectParameters->AcceptAnyBSSID);
    if (status != NDIS_STATUS_SUCCESS)
        goto exit;

    status = pPortPropertyCache->GetPropertyList(WfcPortPropertyDesiredBSSIDList,
        &bufferSize,
        &pConnectParameters->BSSIDList.ElementCount,
        (BYTE **)&pConnectParameters->BSSIDList.pBuffer);
    if (status != NDIS_STATUS_SUCCESS)
        goto exit;

    if (!pConnectParameters->HostFIPSModeEnabled)
    {
        // Apply MFP setting
        pConnectParameters->MFPEnabled = pPortPropertyCache->GetPropertyBooleanOrDefault(WfcPortPropertyMFPAllowed, TRUE);
    }

    status = pPortPropertyCache->GetPropertyBoolean(WfcPortPropertyIgnoreAllMacAddresses,
        &pConnectParameters->IgnoreAllMACAddresses);
    if (status != NDIS_STATUS_SUCCESS)
        goto exit;

    status = pPortPropertyCache->GetPropertyList(WfcPortPropertyExcludedMacAddressList,
        &bufferSize,
        &pConnectParameters->ExcludedMacAddressList.ElementCount,
        (BYTE **)&pConnectParameters->ExcludedMacAddressList.pBuffer);
    if (status != NDIS_STATUS_SUCCESS)
        goto exit;

exit:
    TraceExit(status);

    return status;
}

bool CConnectHelpers::IsFastTransitionAllowed(
    _In_ CPort* pPort,
    _In_ CAdapter* pAdapter,
    _In_ WFC_CONNECTION_PROFILE_PARAMETERS* pConnectionParameters
    )
{
    /*
    There are four cases where Fast Transition is not supported/not allowed by the OS
    1: If the SSID is blocked for 802.11r
    2: If the network is not RSN (Authentication Algorithm is not DOT11_AUTH_ALGO_RSNA)
    3: If the connection is FIPS
    4: If the adapter does not support 802.11r
    */
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    bool isFTAllowed = true;

    isFTAllowed = pPort->IsValidSSIDForFT(pConnectionParameters->SSIDList.pBuffer[0],
        CSystem::get_CurrentTime());
    if (!isFTAllowed)
    {
        goto exit;
    }

    bool bIsNonRSN = true;

    for (UINT32 i = 0; i < pConnectionParameters->AuthAlgoList.ElementCount; i++)
    {
        if (pConnectionParameters->AuthAlgoList.pBuffer[i] == DOT11_AUTH_ALGO_RSNA)
        {
            bIsNonRSN = false;
        }
    }

    if (bIsNonRSN)
    {
        isFTAllowed = false;
        goto exit;
    }

    PDOT11_ADAPTER_CAPABILITIES pAdapterCapabilities = NULL;
    ndisStatus = CConnectHelpers::GetAdapterCapabilitiesFromPropertyCache(pAdapter, &pAdapterCapabilities);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to get adapter capabilities from propertye cache");
        isFTAllowed = false;
        goto exit;
    }

    isFTAllowed = pAdapterCapabilities->FastTransitionSupported == dot11_tri_state_true ? true : false;

    if (!isFTAllowed)
    {
        goto exit;
    }

    if (pConnectionParameters->HostFIPSModeEnabled)
    {
        isFTAllowed = false;
        goto exit;
    }

exit:
    return isFTAllowed;
}

PROAM_CONTROL_PARAMETERS CConnectHelpers::GetRoamControlParameters(
    _In_ CAdapterPropertyCache* pAdapterPropertyCache
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG roamControl = ROAM_CONTROL_LEVEL::WfcNeverRoam;

    ndisStatus = pAdapterPropertyCache->GetPropertyULong(WfcAdapterPropertyRegRoamControlParametersIndex,
        &roamControl);
    if ((ndisStatus != NDIS_STATUS_SUCCESS))
    {
        WFCError("Didn't find Roam Control in propertye cache");
        // Don't roam
        roamControl = ROAM_CONTROL_LEVEL::WfcNeverRoam;
    }

    // If we are in media streaming mode and are on NormalRoaming, switch to
    // media streaming mode roaming
    if ((pAdapterPropertyCache->GetPropertyULongOrDefault(
            WfcAdapterPropertyMediaStreamingRefcount, 0) > 0) &&
        (roamControl == ROAM_CONTROL_LEVEL::WfcNormalRoam))
    {
        WFCTrace("[Roam] Using Media Streaming mode Roam Control (instead of Normal)");
        roamControl = ROAM_CONTROL_LEVEL::WfcMediaStreamingRoam;
    }

    if (roamControl == ROAM_CONTROL_LEVEL::WfcCustomRoam)
    {
        // Load the fields from the registry
        (VOID)pAdapterPropertyCache->GetPropertyULong(WfcAdapterPropertyRegRoamLQThreshold,
            &(g_RoamControlParametersTable[roamControl].BetterAPRoamLinkQualityThreshold));

        (VOID)pAdapterPropertyCache->GetPropertyULong(WfcAdapterPropertyRegRoamLQChangeBar,
            &(g_RoamControlParametersTable[roamControl].APLinkQualitySignificantChangeBar));

        (VOID)pAdapterPropertyCache->GetPropertyULong(WfcAdapterPropertyRegRoamTimeGap,
            &(g_RoamControlParametersTable[roamControl].BetterAPRoamReattemptTimeGapSeconds));

        (VOID)pAdapterPropertyCache->GetPropertyBoolean(WfcAdapterPropertyRegRoamAllowScanCancel,
            &(g_RoamControlParametersTable[roamControl].ScanCancelOnGoodAPFoundPermitted));

        ULONG tempFiveGHzParams = 0;
        (VOID)pAdapterPropertyCache->GetPropertyULong(WfcAdapterPropertyRegRoamFiveGhzParams,
            &tempFiveGHzParams);

        //regkey is read like: 0x00503205  
        //low byte is FiveGHzLowBoost (0x5), 
        //second low byte is FiveGHzBoostLinkQualityStart (0x32 (50)),
        //third low byte isFiveGHzBoostLinkQualityEnd (0x50 (80))
        ULONG tempLowLQBoost = tempFiveGHzParams & 0x0000FF;
        ULONG tempLowLQ = (tempFiveGHzParams & 0x00FF00) >> 8;
        ULONG tempHighLQ = (tempFiveGHzParams & 0xFF0000) >> 16;

        g_RoamControlParametersTable[roamControl].FiveGHzLowBoost = tempLowLQBoost;
        g_RoamControlParametersTable[roamControl].FiveGHzBoostLinkQualityStart = tempLowLQ;
        g_RoamControlParametersTable[roamControl].FiveGHzBoostLinkQualityEnd = tempHighLQ;

        WFCTrace("[Roam]Custom 5GHz boost parameters: LowLQBoost=%d,LowLQ=%d,HighLQ=%d", tempLowLQBoost, tempLowLQ, tempHighLQ);
    }
    else
    {
        NT_ASSERT(roamControl <= ROAM_CONTROL_LEVEL::WfcMaxRoamControlLevel);
    }
    return &g_RoamControlParametersTable[roamControl];
}

//
// WDI Roam behavior is determined by a set of flags
// These flags are currently set by the roam/connect trigger however 
// this is not required in any way. There is no reason that the roam/connect can't have behavior 
// separate from the trigger however current triggers lended themselves to a mapping function (i.e. NOT one-to-many)
//
// The behavior of the roam/connect job should never depend on the trigger but rather the WDI_ROAM_CONFIGURATION_FLAGS.
// This allows new triggers to be added without polluting the conditionals in the roam/connect code.
//
// Currently the behavior is sufficiently boolean based that flags were sufficient and simple.  Future behaviors
// may require a more complex configuration structure (rather than the current flag based enum).
//
WDI_ROAM_CONFIGURATION_FLAGS CConnectHelpers::GetRoamConfig(WFC_ROAM_CONNECT_TRIGGER trigger)
{
    WDI_ROAM_CONFIGURATION_FLAGS flags = RC_MAX;
    switch (trigger)
    {
        case (WfcRoamConnectTriggerFirstTimeConnect) :
            flags = RC_CONNECT | RC_BYPASS_SCAN_THROTTLING;
            break;
        case (WfcRoamConnectTriggerAssociationLoss) :
            flags = RC_RECONNECT | RC_BYPASS_SCAN_THROTTLING | RC_USE_CHANNEL_HISTORY | RC_INCLUDE_SSID | RC_USE_ROAM_EXPIRY_TIME;
            break;
        case (WfcRoamConnectTriggerResumeFromDx) :
            //same as association loss except add RC_RESUME_FROM_DX and remove RC_INCLUDE_SSID (for privacy considerations)
            flags = RC_RECONNECT | RC_RESUME_FROM_DX | RC_BYPASS_SCAN_THROTTLING | RC_USE_CHANNEL_HISTORY | RC_USE_ROAM_EXPIRY_TIME;
            break;
        case (WfcRoamConnectTriggerToFindBetterAP_BestEffort) :
            __fallthrough; //opportunistic and besteffort have same behavior
        case (WfcRoamConnectTriggerToFindBetterAP_Opportunistic) :
            flags = RC_CONNECTED_ROAM | RC_BEST_EFFORT | RC_CHECK_GOOD_ENOUGH_AP | RC_USE_CHANNEL_HISTORY | RC_INCLUDE_SSID | RC_USE_ROAM_EXPIRY_TIME | RC_DEFERRABLE_ROAM;
            break;
        case (WfcRoamConnectTriggerToFindBetterAP_Critical) :
            flags = RC_CONNECTED_ROAM | RC_BYPASS_SCAN_THROTTLING | RC_CHECK_GOOD_ENOUGH_AP | RC_USE_CHANNEL_HISTORY | RC_INCLUDE_SSID | RC_USE_ROAM_EXPIRY_TIME | RC_DEFERRABLE_ROAM;
            break;
        case (WfcRoamConnectTriggerBssTransitionRequest) :
            flags = RC_CONNECTED_ROAM | RC_BSS_TRANSITION_MANAGEMENT_REQUEST | RC_BEST_EFFORT | RC_CHECK_GOOD_ENOUGH_AP | RC_USE_CHANNEL_HISTORY | RC_INCLUDE_SSID | RC_USE_ROAM_EXPIRY_TIME | RC_DEFERRABLE_ROAM;
            break;
        case (WfcRoamConnectTriggerBssTransitionRequestCritical) :
            flags = RC_CONNECTED_ROAM | RC_BSS_TRANSITION_MANAGEMENT_REQUEST | RC_BSS_TRANSITION_MANAGEMENT_REQUEST_CRITICAL | RC_BYPASS_SCAN_THROTTLING | RC_CHECK_GOOD_ENOUGH_AP | RC_USE_CHANNEL_HISTORY | RC_INCLUDE_SSID | RC_USE_ROAM_EXPIRY_TIME;
            break;
        default:
            NT_ASSERTMSG("Invalid trigger", 0);
            flags = RC_MAX;
            break;
    }
    return flags;
}

NDIS_STATUS CConnectHelpers::FillConnectBSSEntryTLV(
    _In_ CPort* pPort,
    _In_ bool bFTAllowed,
    _In_ ULONG  BSSCandidateCount,
    _In_opt_count_(BSSCandidateCount)CBSSEntry** pBSSCandidateList,
    _In_ ArrayOfElements<WDI_CONNECT_BSS_ENTRY_CONTAINER> *pWDIBSSEntryList,
    _In_ bool bRoamRequest
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONGLONG currentTime;
    UINT16 pmkidEntryCount;
    PDOT11_PMKID_ENTRY pPmkidEntries;
    ULONG bufferSize;
    CPortPropertyCache* pPortPropertyCache = pPort->GetPropertyCache();
    BYTE* pSNonce = NULL;

    if (BSSCandidateCount > 0)
    {
        // Get the PMKID list so that we can use it below
        ndisStatus = pPortPropertyCache->GetPropertyList(WfcPortPropertyPMKIDList,
            &bufferSize,
            &pmkidEntryCount,
            (BYTE **)&pPmkidEntries);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("Failed to retrieve PMKID list from property cache. Error = 0x%08x", ndisStatus);    
            goto exit;
        }   

        ndisStatus = pWDIBSSEntryList->AllocateElements(BSSCandidateCount, 0);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("Failed to allocate memory for PreferredBSSEntryList algorithms list. Error = 0x%08x", ndisStatus);
            goto exit;
        }

        currentTime = CSystem::get_CurrentTime();
        for (UINT32 i = 0; i < pWDIBSSEntryList->ElementCount; i++)
        {
            PWDI_CONNECT_BSS_ENTRY_CONTAINER pBSSContainer = &pWDIBSSEntryList->pElements[i];
            PVARIABLE_LENGTH_BUFFER_UINT8 pBeaconProbeResponse = NULL;

            RtlCopyMemory(pBSSContainer->BSSID.Address,
                pBSSCandidateList[i]->GetBssId(),
                sizeof(DOT11_MAC_ADDRESS));

            pBSSContainer->ChannelInfo.BandId = pBSSCandidateList[i]->GetChannelInfo()->BandId;
            pBSSContainer->ChannelInfo.ChannelNumber = pBSSCandidateList[i]->GetChannelInfo()->Channel;
        
            pBSSContainer->SignalInfo.RSSI = pBSSCandidateList[i]->GetSignalInfo()->Rssi;
            pBSSContainer->SignalInfo.LinkQuality = pBSSCandidateList[i]->GetSignalInfo()->LinkQuality;
        
            // Optional: Probe Response, Beacon, Device Context, PMKID
            pBeaconProbeResponse = pBSSCandidateList[i]->GetProbeResponse(currentTime);        
            if ( pBeaconProbeResponse != NULL )
            {
                pBSSContainer->ProbeResponseFrame.SimpleAssign(pBeaconProbeResponse->pBuffer,
                    pBeaconProbeResponse->dataLength
                    );

                pBSSContainer->Optional.ProbeResponseFrame_IsPresent = 1;           
            }

            pBeaconProbeResponse = pBSSCandidateList[i]->GetBeacon(currentTime);        
            if ( pBeaconProbeResponse != NULL )
            {
                pBSSContainer->BeaconFrame.SimpleAssign(pBeaconProbeResponse->pBuffer,
                    pBeaconProbeResponse->dataLength
                    );

                pBSSContainer->Optional.BeaconFrame_IsPresent = 1;           
            }

            if ( pBSSCandidateList[i]->GetDeviceContext()->dataLength > 0 )
            {
                pBSSContainer->DeviceSpecificContext.SimpleAssign(pBSSCandidateList[i]->GetDeviceContext()->pBuffer,
                    pBSSCandidateList[i]->GetDeviceContext()->dataLength
                    
                    );

                pBSSContainer->Optional.DeviceSpecificContext_IsPresent = 1;           
            }

            // Find and set the PMKID
            DOT11_PMKID_VALUE* pOutPmkidValue = NULL;
            
            for (UINT16 j = 0; j < pmkidEntryCount; j++)
            {
                if (ARE_MAC_ADDRESS_EQUAL(pBSSContainer->BSSID.Address, pPmkidEntries[j].BSSID))
                {
                    pOutPmkidValue = &(pPmkidEntries[j].PMKID);
                }
            }

            if (pOutPmkidValue)
            {
                pBSSContainer->PMKID.SimpleAssign((UINT8 *)pOutPmkidValue,
                    sizeof(DOT11_PMKID_VALUE)
                    );

                pBSSContainer->Optional.PMKID_IsPresent = 1;           
            }

            // Optional: BSS Selection flags
            pBSSContainer->Optional.BSSSelectionParameters_IsPresent = 1;
            pBSSContainer->BSSSelectionParameters.SelectionFlags = pBSSCandidateList[i]->GetBssSelectionFlags();

            /* 
            Set Fast BSS Transition IEs if supported
            1: If this is a first time connect (non-roam), then check if the SSID is blocked for FT (bFtAllowed) and populate the FT parameters
            2: If this is a roam, check if Initial Mobility Domain Associaiton was done, and bFTAllowed. In this case, populate the FT parameters
            */
            if (pBSSCandidateList[i]->IsFastBSSTransitionSupported())
            {
                PVARIABLE_LENGTH_BUFFER_UINT8 pMDEBody = pBSSCandidateList[i]->GetMobilityDomainIEBody();
                ArrayOfElements<BYTE> MobilityDomainIE;
                PDOT11_INFO_ELEMENT MDEInfo;
                ULONG MDELength = sizeof(DOT11_INFO_ELEMENT)+pMDEBody->dataLength;

                ndisStatus = MobilityDomainIE.AllocateElements(MDELength, 0);
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    WFCError("[11r] Failed to allocate memory for MDE. Status = 0x%08x", ndisStatus);
                    goto exit;
                }

                RtlZeroMemory(MobilityDomainIE.pElements, MDELength);

                MDEInfo = (PDOT11_INFO_ELEMENT)MobilityDomainIE.pElements;
                MDEInfo->ElementID = DOT11_INFO_ELEMENT_ID_MOBILITY_DOMAIN;
                MDEInfo->Length = (UCHAR)pMDEBody->dataLength; //Fixed - 3;

                RtlCopyMemory((MobilityDomainIE.pElements + sizeof(DOT11_INFO_ELEMENT)),
                    (BYTE*)pMDEBody->pBuffer,
                    pMDEBody->dataLength);

                if (!bFTAllowed)
                {
                    WFCInfo("[11r] Fast Transition is not allowed currently. Not filling up 11r FT parameters");
                    continue;
                }

                if (!bRoamRequest)
                {
                    if (pBSSCandidateList[i]->IsCorrectFTAKMSuitesAdvertised())
                    {
                        pBSSContainer->FTInitialAssocParameters.MDE.FullMove(&MobilityDomainIE);
                        pBSSContainer->Optional.FTInitialAssocParameters_IsPresent = 1;
                    }
                    else
                    {
                        WFCInfo("[11r] Candidate does not advertise correct FT AKM suites. Not filling up 11r parameters. Need to advertise FT-PSK or FT-1x");
                        continue;
                    }
                }
                else
                {
                    UCHAR R0KHIDSubElementID = 3;
                    ULONG cbToAlloc = sizeof(WDI_STRUCT_FTE);
                    ULONG CurrentOffset = sizeof(WDI_STRUCT_FTE);

                    PWDI_STRUCT_FTE pFastTransitionElement;

                    BOOLEAN FTConnection;

                    pPortPropertyCache->GetPropertyBoolean((WFC_PROPERTY_NAME)WfcPortPropertyCurrentConnectionFT, &FTConnection);
                    if (!FTConnection)
                    {
                        WFCInfo("Not a Fast transition connect. So can't do FT roam, not filling up params for roam");
                        continue;
                    }

                    BYTE* pR0KHID;
                    ULONG R0KHIDSize;
                    ndisStatus = pPortPropertyCache->GetPropertyBuffer((WFC_PROPERTY_NAME)WfcPortPropertyR0KHID, &R0KHIDSize,
                        &pR0KHID);
                    if (ndisStatus != NDIS_STATUS_SUCCESS)
                    {
                        WFCError("Error: Unable to retrieve R0KH-ID from property cache. Status = 0x%08x", ndisStatus);
                        ndisStatus = NDIS_STATUS_FAILURE;
                        goto exit;
                    }
                    if (pR0KHID == NULL)
                    {
                        WFCError("[11r] This is a FT connect and R0KH-ID is NULL. Hence cannot do fast roaming.");
                        ndisStatus = NDIS_STATUS_FAILURE;
                        goto exit;
                    }

                    cbToAlloc += R0KHIDSize + 2; //1 byte for Sub-element ID and 1 byte for Length
                    
                    ArrayOfElements<BYTE> buffer;
                    ndisStatus = buffer.AllocateElements(cbToAlloc, 0);
                    if (ndisStatus != NDIS_STATUS_SUCCESS)
                    {
                        WFCError("[11r] Failed to allocate memory for FTE. Status = 0x%08x", ndisStatus); 
                        goto exit;
                    }

                    RtlZeroMemory(buffer.pElements, cbToAlloc);

                    pFastTransitionElement = (PWDI_STRUCT_FTE)buffer.pElements;
                    pFastTransitionElement->ElementId = DOT11_INFO_ELEMENT_ID_FAST_BSS_TRANSITION;
                    pFastTransitionElement->Length = (UCHAR)cbToAlloc - 2;      // 2 bytes are for the Type+Length
                    
                    RtlCopyMemory(
                        (buffer.pElements + CurrentOffset),
                        (BYTE*)&R0KHIDSubElementID,
                        sizeof(BYTE)
                        );
                    
                    CurrentOffset += sizeof(BYTE);

                    RtlCopyMemory(
                        (buffer.pElements + CurrentOffset),
                        (BYTE*)&R0KHIDSize,
                        sizeof(BYTE)
                        );
                    
                    CurrentOffset += sizeof(BYTE);
                    RtlCopyMemory(
                        (buffer.pElements + CurrentOffset),
                        pR0KHID,
                        R0KHIDSize
                        );

                    BYTE* pPMKR0Name;
                    ULONG PMKR0NameSize;
                    ndisStatus = pPortPropertyCache->GetPropertyBuffer((WFC_PROPERTY_NAME)WfcPortPropertyPMKR0Name, &PMKR0NameSize,
                        &pPMKR0Name);
                    if (ndisStatus != NDIS_STATUS_SUCCESS)
                    {
                        WFCError("Error: Unable to retrieve PMKR0Name from property cache. Status = 0x%08x", ndisStatus);
                        ndisStatus = NDIS_STATUS_FAILURE;
                        goto exit;
                    }

                    if (pPMKR0Name == NULL)
                    {
                        WFCError("[11r] This is a FT connect and PMKR0Name is NULL. Hence cannot do fast roaming.");
                        ndisStatus = NDIS_STATUS_FAILURE;
                        goto exit;
                    }

                    RtlCopyMemory(pBSSContainer->FTReAssocParameters.PMKR0Name.pmkname.Name, pPMKR0Name, PMKR0NameSize);

                    //
                    // If an SNonce has not been generated yet, generate it and save it
                    //
                    if (!pSNonce)
                    {
                        pSNonce = new BYTE[SIZEOF_NONCE];
                        if (pSNonce == NULL)
                        {
                            WFCError("Error: Unable to allocate memory for SNonce");
                            ndisStatus = NDIS_STATUS_RESOURCES;
                            goto exit;
                        }

                        pBSSCandidateList[i]->GenerateSNonce(pSNonce, SIZEOF_NONCE);
                        ndisStatus = pPortPropertyCache->SetPropertyBuffer((WFC_PROPERTY_NAME)WfcPortPropertySNonce,
                            SIZEOF_NONCE,
                            pSNonce);
                        if (ndisStatus != NDIS_STATUS_SUCCESS)
                        {
                            WFCError("Error: Unable to set SNonce in property cache. Status = 0x%08x", ndisStatus);
                            ndisStatus = NDIS_STATUS_FAILURE;
                            goto exit;
                        }
                    }

                    RtlCopyMemory(pFastTransitionElement->SNonce, pSNonce, SIZEOF_NONCE);

                    pBSSContainer->FTReAssocParameters.FTE.FullMove(&buffer);
                    pBSSContainer->FTReAssocParameters.MDE.FullMove(&MobilityDomainIE);
                    pBSSContainer->Optional.FTReAssocParameters_IsPresent = 1;
                }
            }
        }
    }

exit:
    if (pSNonce)
    {
        delete [] pSNonce;
    }

    return ndisStatus;
}    

//==============================================================================================
CConnectJob::CConnectJob(
    _In_ WFC_ACTIVITY_ID ActivityId) :
    m_IsCancelled(false),
    m_LastScanType(WfcConnectScanTypeNone),
    m_RoamWabiReason(WDI_ASSOC_STATUS_FAILURE),
    m_ConnectTrigger(WfcRoamConnectTriggerFirstTimeConnect),
    m_pSetConnectionParametersBuffer(NULL),
    m_SetConnectionParametersBufferLength(0),
    m_ConnectTask(ActivityId),
    m_AssocNotificationRegistrationToken(WFC_REGISTRATION_TOKEN_INVALID),
    m_PMKIDNotificationRegistrationToken(WFC_REGISTRATION_TOKEN_INVALID),
    m_BssEntryListNotificationRegistrationToken(WFC_REGISTRATION_TOKEN_INVALID),
    m_pScanJob(NULL),
    m_pResponseJob(NULL),
    COidJobBase(ActivityId),
    m_ConnectAttemptCount(0),
    m_NotifiedConnectionStarted(false),
    m_IsP2PConnect(false),
    m_NotifiedSuccessfulAssociation(false),
    m_PrivacyExemptCommand(ActivityId),
    m_pPrivacyCommandDataBuffer(NULL),
    m_PrivacyCommandBufferLength(0),
    m_PMKIDTimerToken(WFC_TIMER_REGISTRATION_TOKEN_INVALID),
    m_BSSListUpdateJob(ActivityId),
    m_DiscoverJob(ActivityId),
    m_ConnectCompletionStatus(WDI_ASSOC_STATUS_FAILURE),
    m_IsRoam(false),
    m_bIsScanCancelIntended(false),
    m_RoamConfigFlags(CConnectHelpers::GetRoamConfig(m_ConnectTrigger)),
    m_IsReconnectNeededDueTo11r(false),
    m_IsBlockSSIDFor11rNeeded(false),
    m_IsCriticalRoamToLEAlreadyAttempted(false)
{
}

CConnectJob::~CConnectJob()
{
    if (m_pConnectParametersTlvBuffer != NULL)
    {
        delete[] m_pConnectParametersTlvBuffer;
        m_pConnectParametersTlvBuffer = NULL;
    }

    if (m_pPrivacyCommandDataBuffer != NULL)
    {
        FreeGenerated(m_pPrivacyCommandDataBuffer);
        m_pPrivacyCommandDataBuffer = NULL;
    }

    if (m_PMKIDTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        (VOID)m_pEventQueue->DeregisterTimeoutCallback(
            m_PMKIDTimerToken);
        m_PMKIDTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    if (m_NloMatchingCriteria.SSIDList.pBuffer)
    {
        delete[] m_NloMatchingCriteria.SSIDList.pBuffer;
        m_NloMatchingCriteria.SSIDList.pBuffer = NULL;
    }
}

NDIS_STATUS CConnectJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceEntry();

    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;
    pOidRequest->DATA.SET_INFORMATION.BytesRead = 0;
    m_ConnectTrigger = WfcRoamConnectTriggerFirstTimeConnect;
    m_RoamConfigFlags = CConnectHelpers::GetRoamConfig(WfcRoamConnectTriggerFirstTimeConnect);

    m_IsP2PConnect = SET_OID_VALUE(pOidRequest) == OID_DOT11_WFD_CONNECT_TO_GROUP_REQUEST ?
        1 : 0;

    TraceExit(status);

    return status;
}


NDIS_STATUS CConnectJob::InitializeForRoamReconnectRequest(
    _In_ CAdapter* pAdapter,
    _In_ NDIS_PORT_NUMBER PortNumber,
    _In_ bool IsP2PClient,
    _In_ WFC_ROAM_CONNECT_TRIGGER ConnectTrigger,
    _In_ WDI_ASSOC_STATUS RoamWabiReason,
    _In_ WDI_ROAM_CONFIGURATION_FLAGS RoamConfigFlags,
    _In_ WFC_CONNECT_SCAN_TYPE InitialScanType
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    bool bIsPreemptable = true; // default is true
    bool bIsRoamDeferrable = false;
   
    if (TestRoamConfig(RoamConfigFlags, RC_RESUME_FROM_DX))
    {
        bIsPreemptable = false;
    }

    if (TestRoamConfig(RoamConfigFlags, RC_DEFERRABLE_ROAM))
    {
        bIsRoamDeferrable = true;
    }

    status = InitializeWithoutOid(
        WiFiJobConnectRequest,
        pAdapter,
        bIsPreemptable,
        false,
        WiFiPriorityScopeAdapter,
        WiFiSerializedJobPriority_5, // Connect for roam is lower priority than first-time connect
        PortNumber,
        bIsRoamDeferrable);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't initialize COidJobBase");
        goto exit;
    }

    m_OidRequestId = 0;
    m_OidPortNumber = PortNumber;
    m_pOidRequest = NULL;

    m_ConnectTrigger = ConnectTrigger;
    m_RoamWabiReason = RoamWabiReason;
    m_IsP2PConnect = IsP2PClient;
    m_RoamConfigFlags = RoamConfigFlags;
    m_IsRoam = true;
    m_LastScanType = InitialScanType;

exit:
    TraceActExit(status);

    return status;
}

void CConnectJob::NotifyConnectionStartIfNecessary()
{
    // Only for a first time connect do we send the connection start 
    // and completion notifications
    if (TestRoamConfig(m_RoamConfigFlags, RC_CONNECT))
    {    
        DOT11_CONNECTION_START_PARAMETERS connectionStartParameters = { 0 };

        WFCTrace("[Connect] Connect Start Indication");

        ASSIGN_NDIS_OBJECT_HEADER(
            connectionStartParameters.Header,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_CONNECTION_START_PARAMETERS_REVISION_1,
            sizeof(DOT11_CONNECTION_START_PARAMETERS));

        //
        // There is no adhoc support
        //
        connectionStartParameters.BSSType = dot11_BSS_type_infrastructure;

        m_pAdapter->IndicateStatus(
            m_OidPortNumber,
            NDIS_STATUS_DOT11_CONNECTION_START,
            m_OidRequestId,
            &connectionStartParameters,
            sizeof(connectionStartParameters));

        m_NotifiedConnectionStarted = true;                    
    }

    return;
}


void CConnectJob::NotifyConnectionCompletionIfNecessary(
    _In_ NDIS_STATUS OperationStatus)
{
    DOT11_CONNECTION_COMPLETION_PARAMETERS connectCompletionParams = { 0 };
    WDI_ASSOC_STATUS associationCompletionStatus = m_ConnectCompletionStatus;

    if (!m_NotifiedConnectionStarted)
    {
        WFCInfo(
            "Didn't notify connection started. Skipping connection completed notifications.");
        return;
    }

    WFCTrace("[Connect] Connect Completion Indication with status 0x%08x", OperationStatus);

    NT_ASSERT(TestRoamConfig(m_RoamConfigFlags, RC_CONNECT));

    ASSIGN_NDIS_OBJECT_HEADER(
        connectCompletionParams.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_CONNECTION_COMPLETION_PARAMETERS_REVISION_1,
        sizeof(DOT11_CONNECTION_COMPLETION_PARAMETERS));

    if (OperationStatus != NDIS_STATUS_SUCCESS &&
        associationCompletionStatus == WDI_ASSOC_STATUS_SUCCESS)
    {
        WFCActInfo(
            "Assoc status not set and job failed. Setting generic failure string.");
        associationCompletionStatus = WDI_ASSOC_STATUS_FAILURE;
    }

    connectCompletionParams.uStatus = CWabiToDot11Converter::MapAssocStatus(
                                            associationCompletionStatus,
                                            0);

    // Send the complete indication to OS
    m_pAdapter->IndicateStatus(
        m_OidPortNumber,
        NDIS_STATUS_DOT11_CONNECTION_COMPLETION,
        m_OidRequestId,
        &connectCompletionParams,
        sizeof(connectCompletionParams));
}

void CConnectJob::NotifyRoamingAttemptExhausted()
{
    DOT11_ROAMING_ATTEMPTS_EXHAUSTED_PARAMETERS roamingAttemptExhaustedParameters = { 0 };

    CConnectedPeer* pPeer = m_pAdapter->GetConnectedPeerList(get_PortId())
        ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);
    if (pPeer)
    {
        NT_ASSERTMSG("Roaming Attempt Exhausted Indication request when we are actually associated", FALSE);
        
        WFCTrace("[Roam] Roaming Attempt Exhausted Indication requested when we are actually associated");
        return;
    }

    if (m_NdisPortNumber != NDIS_DEFAULT_PORT_NUMBER)
    {
        WFCTrace("[Roam] Not sending Roaming Attempt Exhausted Indication on non-default port");
        return;
    }    
    
    WFCTrace("[Roam] Roaming Attempt Exhausted Indication");

    ASSIGN_NDIS_OBJECT_HEADER(
        roamingAttemptExhaustedParameters.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_ROAMING_ATTEMPTS_EXHAUSTED_PARAMETERS_REVISION_1,
        DOT11_SIZEOF_ROAMING_ATTEMPTS_EXHAUSTED_PARAMETERS_REVISION_1);

    m_pAdapter->IndicateStatus(
        m_OidPortNumber,
        NDIS_STATUS_DOT11_ROAMING_ATTEMPTS_EXHAUSTED,
        m_OidRequestId,
        &roamingAttemptExhaustedParameters,
        DOT11_SIZEOF_ROAMING_ATTEMPTS_EXHAUSTED_PARAMETERS_REVISION_1);

    return;
}

ULONG CConnectJob::NotifyAssociationStartAndCompletionIfNecessary(
    _In_ CBSSEntry* pBssEntry,
    _In_ WDI_ASSOCIATION_RESULT_CONTAINER* pAssociationResult
    )
{
    ULONG internalStatus = DOT11_ASSOC_STATUS_SUCCESS;
    ULONG completionStatus = DOT11_ASSOC_STATUS_SUCCESS;
    ULONG completionParamsSize = 0;
    DOT11_ASSOCIATION_COMPLETION_PARAMETERS* pCompletionParams = NULL;
    bool completionAllocated = false;
    TraceEntry();

    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());
    if (pPort->m_pRoamTraceLoggingData != NULL)
    {
        pPort->m_pRoamTraceLoggingData->connectRoamTaskEndTime = CSystem::get_CurrentTime();
    }

    // Fill association start parameters
    RtlCopyMemory(
        &m_AssociationStartParameters.MacAddr,
        pAssociationResult->BSSID.Address,
        sizeof(DOT11_MAC_ADDRESS));

    RtlCopyMemory(
        &m_AssociationStartParameters.SSID,
        m_ConnectionProfile.SSIDList.pBuffer,
        sizeof(DOT11_SSID));

    WFCTrace("[Connect] Association Completion BSSID = %!MACADDR!", m_AssociationStartParameters.MacAddr);

    // Fill the association completion parameters            

    // Figure out the completion size
    completionParamsSize = sizeof(DOT11_ASSOCIATION_COMPLETION_PARAMETERS);

    // We will be storing offsets, etc into the private structure
    RtlZeroMemory(&m_StaticAssociationCompletionParameters, sizeof(DOT11_ASSOCIATION_COMPLETION_PARAMETERS));

    if (pAssociationResult->Optional.AssociationRequestFrame_IsPresent)
    {
        m_StaticAssociationCompletionParameters.uAssocReqOffset = completionParamsSize;
        m_StaticAssociationCompletionParameters.uAssocReqSize = pAssociationResult->AssociationRequestFrame.ElementCount;            
        if (!NT_SUCCESS(RtlULongAdd(
            completionParamsSize,
            m_StaticAssociationCompletionParameters.uAssocReqSize,
            &completionParamsSize)))
        {
            WFCActError("Integer overflow populating uAssocReqSize %u += %u", completionParamsSize,
                m_StaticAssociationCompletionParameters.uAssocReqSize);
            completionParamsSize = 0;
            internalStatus = DOT11_ASSOC_STATUS_FAILURE;
            // No point doing extra work below, jump ahead
            goto SkipAssocResultProcessing;
        }        
    }            

    if (pAssociationResult->Optional.AssociationResponseFrame_IsPresent)
    {
        m_StaticAssociationCompletionParameters.uAssocRespOffset = completionParamsSize;
        m_StaticAssociationCompletionParameters.uAssocRespSize = pAssociationResult->AssociationResponseFrame.ElementCount;            
        if (!NT_SUCCESS(RtlULongAdd(
            completionParamsSize,
            m_StaticAssociationCompletionParameters.uAssocRespSize,
            &completionParamsSize)))
        {
            WFCActError("Integer overflow populating uAssocRespSize %u += %u", completionParamsSize,
                m_StaticAssociationCompletionParameters.uAssocRespSize);
            completionParamsSize = 0;
            internalStatus = DOT11_ASSOC_STATUS_FAILURE;
            // No point doing extra work below, jump ahead
            goto SkipAssocResultProcessing;
        }        
    }            

    if (pAssociationResult->Optional.BeaconProbeResponse_IsPresent)
    {
        m_StaticAssociationCompletionParameters.uBeaconOffset = completionParamsSize;
        m_StaticAssociationCompletionParameters.uBeaconSize = pAssociationResult->BeaconProbeResponse.ElementCount;            
        
        if (!NT_SUCCESS(RtlULongAdd(
            completionParamsSize,
            m_StaticAssociationCompletionParameters.uBeaconSize,
            &completionParamsSize)))
        {
            WFCActError("Integer overflow populating uBeaconSize %u += %u", completionParamsSize,
                m_StaticAssociationCompletionParameters.uBeaconSize);
            completionParamsSize = 0;
            internalStatus = DOT11_ASSOC_STATUS_FAILURE;
            // No point doing extra work below, jump ahead
            goto SkipAssocResultProcessing;
        }        
        
    }            
    else
    {
        PVARIABLE_LENGTH_BUFFER_UINT8 pBeaconFrameBuffer;

        pBeaconFrameBuffer = pBssEntry->GetBeaconOrProbeResponse();
        if (pBeaconFrameBuffer == NULL)
        {
            WFCActError("Couldn't find cached beacon/probe response blob.");
            internalStatus = DOT11_ASSOC_STATUS_FAILURE;
            // No point doing extra work below, jump ahead
            goto SkipAssocResultProcessing;
        }
        else
        {
            m_StaticAssociationCompletionParameters.uBeaconOffset = completionParamsSize;
            m_StaticAssociationCompletionParameters.uBeaconSize = pBeaconFrameBuffer->dataLength;            
            
            if (!NT_SUCCESS(RtlULongAdd(
                completionParamsSize,
                m_StaticAssociationCompletionParameters.uBeaconSize,
                &completionParamsSize)))
            {
                WFCActError("Integer overflow populating uBeaconSize %u += %u", completionParamsSize,
                    m_StaticAssociationCompletionParameters.uBeaconSize);
                completionParamsSize = 0;                
                internalStatus = DOT11_ASSOC_STATUS_FAILURE;
                // No point doing extra work below, jump ahead
                goto SkipAssocResultProcessing;
            }        
        }        
    }
        
        
    if (pAssociationResult->Optional.EthertypeEncapTable_IsPresent)
    {
        m_StaticAssociationCompletionParameters.uEncapTableOffset = completionParamsSize;
        m_StaticAssociationCompletionParameters.uEncapTableSize = pAssociationResult->EthertypeEncapTable.ElementCount;            
    
        if (!NT_SUCCESS(RtlULongAdd(
            completionParamsSize,
            m_StaticAssociationCompletionParameters.uEncapTableSize,
            &completionParamsSize)))
        {
            WFCActError("Integer overflow populating uEncapTableSize %u += %u", completionParamsSize,
                m_StaticAssociationCompletionParameters.uEncapTableSize);
            completionParamsSize = 0;            
            internalStatus = DOT11_ASSOC_STATUS_FAILURE;
            // No point doing extra work below, jump ahead
            goto SkipAssocResultProcessing;
        }        
        
    }            
        
    if (pAssociationResult->Optional.ActivePhyTypeList_IsPresent)
    {
        m_StaticAssociationCompletionParameters.uActivePhyListOffset = completionParamsSize;
        m_StaticAssociationCompletionParameters.uActivePhyListSize = pAssociationResult->ActivePhyTypeList.ElementCount * sizeof(ULONG);            
        
        if (!NT_SUCCESS(RtlULongAdd(
            completionParamsSize,
            m_StaticAssociationCompletionParameters.uActivePhyListSize,
            &completionParamsSize)))
        {
            WFCActError("Integer overflow populating uActivePhyListSize %u += %u", completionParamsSize,
                m_StaticAssociationCompletionParameters.uActivePhyListSize);
            completionParamsSize = 0;            
            internalStatus = DOT11_ASSOC_STATUS_FAILURE;
            // No point doing extra work below, jump ahead
            goto SkipAssocResultProcessing;
        }        
    }

SkipAssocResultProcessing:
    if (internalStatus != DOT11_ASSOC_STATUS_SUCCESS)
    {
        // Use the static buffer to fill the result
        pCompletionParams = &m_StaticAssociationCompletionParameters;

        // Zero the Size & Offset's out of the buffer since we won't be filling them in anymore
        RtlZeroMemory(&m_StaticAssociationCompletionParameters, sizeof(DOT11_ASSOCIATION_COMPLETION_PARAMETERS));
    }
    else
    {
        // Only on success, will we use new buffer
        pCompletionParams = (DOT11_ASSOCIATION_COMPLETION_PARAMETERS *)new UCHAR[completionParamsSize];

        if (pCompletionParams == NULL)
        {
            WFCActError("Failed to allocate completion parameters blob.");

            // Use the static buffer to fill the result
            pCompletionParams = &m_StaticAssociationCompletionParameters;

            // Zero the Size & Offset's out of the buffer since we won't be filling them in anymore
            RtlZeroMemory(&m_StaticAssociationCompletionParameters, sizeof(DOT11_ASSOCIATION_COMPLETION_PARAMETERS));
            
            internalStatus = DOT11_ASSOC_STATUS_SYSTEM_ERROR;
        }
        else
        {    
            completionAllocated = true;
            
            // Copy the completion offsets over
            RtlCopyMemory(pCompletionParams, 
                &m_StaticAssociationCompletionParameters, 
                sizeof(DOT11_ASSOCIATION_COMPLETION_PARAMETERS));
        }
    }
    
    // Start filling reset of the completion indication
    RtlCopyMemory(
        &pCompletionParams->MacAddr,
        pAssociationResult->BSSID.Address,
        sizeof(DOT11_MAC_ADDRESS));

    pCompletionParams->bReAssocReq = pAssociationResult->AssociationResultParameters.ReAssociation;
    pCompletionParams->bReAssocResp = pAssociationResult->AssociationResultParameters.ReAssociation;

    pCompletionParams->AuthAlgo = CWabiToDot11Converter::MapAuthAlgorithm(
        pAssociationResult->AssociationResultParameters.AuthAlgorithm);
    
    pCompletionParams->UnicastCipher = CWabiToDot11Converter::MapCipherAlgorithm(
        pAssociationResult->AssociationResultParameters.UnicastCipherAlgorithm);
        
    pCompletionParams->MulticastCipher = CWabiToDot11Converter::MapCipherAlgorithm(
        pAssociationResult->AssociationResultParameters.MulticastDataCipherAlgorithm);
        
    pCompletionParams->bFourAddressSupported = 
        pAssociationResult->AssociationResultParameters.FourAddressSupported;
        
    pCompletionParams->bPortAuthorized =
        pAssociationResult->AssociationResultParameters.PortAuthorized;
    pCompletionParams->ucActiveQoSProtocol =
        (pAssociationResult->AssociationResultParameters.WMMQoSEnabled ? DOT11_QOS_PROTOCOL_FLAG_WMM : 0);
    pCompletionParams->DSInfo = CWabiToDot11Converter::MapDsInfo(
        pAssociationResult->AssociationResultParameters.DSInfo);

    pCompletionParams->MulticastMgmtCipher = CWabiToDot11Converter::MapCipherAlgorithm(
        pAssociationResult->AssociationResultParameters.MulticastMgmtCipherAlgorithm);

    pCompletionParams->uAssocComebackTime = pAssociationResult->AssociationResultParameters.AssociationComebackTime;

    // If we are forcing a failure. We dont fill the details below in
    if (internalStatus == DOT11_ASSOC_STATUS_SUCCESS)
    {        
        if (pAssociationResult->Optional.AssociationRequestFrame_IsPresent)
        {
            RtlCopyMemory(
                (PUCHAR)pCompletionParams + m_StaticAssociationCompletionParameters.uAssocReqOffset,
                pAssociationResult->AssociationRequestFrame.pElements,
                m_StaticAssociationCompletionParameters.uAssocReqSize
                );
        }            

        if (pAssociationResult->Optional.AssociationResponseFrame_IsPresent)
        {
            RtlCopyMemory(
                (PUCHAR)pCompletionParams + m_StaticAssociationCompletionParameters.uAssocRespOffset,
                pAssociationResult->AssociationResponseFrame.pElements,
                m_StaticAssociationCompletionParameters.uAssocRespSize
                );
        }            

        if (pAssociationResult->Optional.BeaconProbeResponse_IsPresent)
        {
            RtlCopyMemory(
                (PUCHAR)pCompletionParams + m_StaticAssociationCompletionParameters.uBeaconOffset,
                pAssociationResult->BeaconProbeResponse.pElements,
                m_StaticAssociationCompletionParameters.uBeaconSize
                );
        }            
        else
        {
            PVARIABLE_LENGTH_BUFFER_UINT8 pBeaconFrameBuffer;

            pBeaconFrameBuffer = pBssEntry->GetBeaconOrProbeResponse();
            if (pBeaconFrameBuffer == NULL)
            {
                WFCActError("Couldn't find cached beacon/probe response blob.");
                internalStatus = DOT11_ASSOC_STATUS_FAILURE;
            }
            else if (pBeaconFrameBuffer->dataLength <= m_StaticAssociationCompletionParameters.uBeaconSize)
            {
                RtlCopyMemory(
                    (PUCHAR)pCompletionParams + m_StaticAssociationCompletionParameters.uBeaconOffset,
                    pBeaconFrameBuffer->pBuffer,
                    m_StaticAssociationCompletionParameters.uBeaconSize
                    );                
            }
            else
            {
                internalStatus = DOT11_ASSOC_STATUS_FAILURE;
            }
        }
            
            
        if (pAssociationResult->Optional.EthertypeEncapTable_IsPresent)
        {
            RtlCopyMemory(
                (PUCHAR)pCompletionParams + m_StaticAssociationCompletionParameters.uEncapTableOffset,
                pAssociationResult->EthertypeEncapTable.pElements,
                m_StaticAssociationCompletionParameters.uEncapTableSize
                );
        }
            
        if (pAssociationResult->Optional.ActivePhyTypeList_IsPresent)
        {                
            RtlCopyMemory(
                (PUCHAR)pCompletionParams + m_StaticAssociationCompletionParameters.uActivePhyListOffset,
                pAssociationResult->ActivePhyTypeList.pElements,
                m_StaticAssociationCompletionParameters.uActivePhyListSize
                );
        }            

        // Not populating
        // pCompletionParams->uIHVDataOffset;
        // pCompletionParams->uIHVDataSize;
        
    }
    else
    {
        WFCActError("Not populating all Association_Completion fields because of internal failure (%d)", internalStatus);
    }

    if ((pAssociationResult->AssociationResultParameters.AssociationStatus == WDI_ASSOC_STATUS_SUCCESS) && 
        (internalStatus != DOT11_ASSOC_STATUS_SUCCESS))
    {
        // We had an internal failure
        WFCActError("Changing Association Completion status from SUCCESS to %d due to internal failure", internalStatus);
        completionStatus = internalStatus;
    }
    else
    {
        // Use the real completion status
        completionStatus = CWabiToDot11Converter::MapAssocStatus(
            pAssociationResult->AssociationResultParameters.AssociationStatus,
            (UINT16)pAssociationResult->AssociationResultParameters.StatusCode,
            false); // Allow new status codes
    }
    pCompletionParams->uStatus = completionStatus;

    
    WFCTrace("[Assoc] Completion BSSID = %!MACADDR! Status = %d", m_AssociationStartParameters.MacAddr, pCompletionParams->uStatus);
    WFCTrace("[Assoc] Completion Auth Algo = %d", pCompletionParams->AuthAlgo);
    WFCTrace("[Assoc] Completion Unicast Cipher = %d", pCompletionParams->UnicastCipher);
    WFCTrace("[Assoc] Completion Multicast Data Cipher = %d", pCompletionParams->MulticastCipher);
    WFCTrace("[Assoc] Completion Multicast Mgmt Cipher = %d", pCompletionParams->MulticastMgmtCipher);



    ASSIGN_NDIS_OBJECT_HEADER(
        m_AssociationStartParameters.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_ASSOCIATION_START_PARAMETERS_REVISION_1,
        sizeof(DOT11_ASSOCIATION_START_PARAMETERS));
    
    //
    // Indicate both inline.
    //
    m_pAdapter->IndicateStatus(
        m_OidPortNumber,
        NDIS_STATUS_DOT11_ASSOCIATION_START,
        m_OidRequestId,
        &m_AssociationStartParameters,
        sizeof(m_AssociationStartParameters));

    ASSIGN_NDIS_OBJECT_HEADER(
        pCompletionParams->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_ASSOCIATION_COMPLETION_PARAMETERS_REVISION_2,
        sizeof(DOT11_ASSOCIATION_COMPLETION_PARAMETERS));

    BOOLEAN FTConnection;
    GetPortPropertyCache()->GetPropertyBoolean((WFC_PROPERTY_NAME)WfcPortPropertyCurrentConnectionFT, &FTConnection);

    if (m_IsRoam && FTConnection)
    {
        m_pAdapter->IndicateStatus(
            m_OidPortNumber,
            NDIS_STATUS_DOT11_FT_ASSOCIATION_COMPLETION,
            m_OidRequestId,
            pCompletionParams,
            completionParamsSize);
    }
    else
    {
        m_pAdapter->IndicateStatus(
            m_OidPortNumber,
            NDIS_STATUS_DOT11_ASSOCIATION_COMPLETION,
            m_OidRequestId,
            pCompletionParams,
            completionParamsSize);
    }
    if (completionAllocated)
    {
        delete[] pCompletionParams;
    }

    TraceExit(completionStatus);

    return completionStatus;
}


void CConnectJob::FinishJob(
    _In_ NDIS_STATUS JobCompleteStatus)
{
    TraceActEntry();
    //
    // TODO: Further cleanup if necessary.
    // Maybe schedule dot11 reset job.
    //

    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());
    if (pPort->m_pRoamTraceLoggingData != NULL)
    {
        pPort->m_pRoamTraceLoggingData->connectJobEndTime = CSystem::get_CurrentTime();
    }

    NotifyConnectionCompletionIfNecessary(
        JobCompleteStatus);

    if (m_AssocNotificationRegistrationToken != WFC_REGISTRATION_TOKEN_INVALID)
    {
        m_pAdapter->GetNotificationManager()->DeregisterNotificationHandler(
            m_AssocNotificationRegistrationToken);
        m_AssocNotificationRegistrationToken = WFC_REGISTRATION_TOKEN_INVALID;
    }

    StopWaitForPMKIDList();

    m_ConnectState = WiFiConnectJobStateCompleted;

    WFCActInfo("[Connect][Link] Connect Job completed (0x%x)", JobCompleteStatus);

    CompleteJob(
        JobCompleteStatus);

    // For Critical 802.11v roam, we might have sent a roam task with no candidates
    // For those cases, we need to send the reject response frame here, if the roam wasn't successful (IHV couldnt find any candidates)
    // and if we are currently still connected
    if (m_IsRoam && TestRoamConfig(m_RoamConfigFlags, RC_BSS_TRANSITION_MANAGEMENT_REQUEST_CRITICAL))
    {
        if (JobCompleteStatus != NDIS_STATUS_SUCCESS)
        {
            CConnectedPeer* pPeer = m_pAdapter->GetConnectedPeerList(get_PortId())
                ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);
            if (pPeer)
            {
                NDIS_STATUS status = pPort->SendResponseBSSTransitionFrame(
                    WDI_BSS_TRANSITION_REJECT_NO_SUITABLE_BSS_CANDIDATES,
                    NULL
                    );
                if (status != NDIS_STATUS_SUCCESS)
                {
                    WFCActError("[11v] Failed to send BSS Transition reject response frame for critical roam. Status = 0x%08x", status);
                }
            }
        }
    }

    if (m_IsRoam && 
        pPort->m_pRoamTraceLoggingData!=NULL && 
        pPort->m_pRoamTraceLoggingData->roamOccured)
    {
        // Trigger the trace logging only if we did not succeed in association with any AP
        // For successful associaiton, we trigger the trace logging when we set the cipher keys
        if (pPort->m_pRoamTraceLoggingData->associationStatusCode != WDI_ASSOC_STATUS_SUCCESS)
        {
            pPort->TraceLoggingRoamFinished(NDIS_STATUS_FAILURE);
        }
    }

    TraceActExit(JobCompleteStatus);
}


VOID CConnectJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus)
{
    TraceActEntry();

    NT_ASSERT(TestRoamConfig(m_RoamConfigFlags, RC_CONNECT));

    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit(NdisStatus);
}

VOID CConnectJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus)
{
    TraceActEntry();

    //
    // No-op. There is no required NDIS interaction when the job is completed for an
    // OID_DOT11_CONNECT_REQUEST.
    //
    // See NotifyConnectCompletionIfNecessary for NDIS notifications
    // that are common to connect and roam functionality.
    //

    TraceActExit(NdisStatus);
}
void CConnectJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    bool bPrivacyExemptionSent = false;
    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    TraceActEntry();

    WFCActInfo("[Connect][Link] Connect Job started. Trigger = %d. Config = 0x%08x", m_ConnectTrigger, m_RoamConfigFlags);
    
    if (pPort->m_pRoamTraceLoggingData == NULL && 
        g_RegisteredWithTraceLogging &&
        !m_IsP2PConnect)
    {
        pPort->m_pRoamTraceLoggingData = new ROAM_TRACELOGGING_DATA;
        if (pPort->m_pRoamTraceLoggingData == NULL)
        {
            WFCActError("[connect][trace] Failed to Allocate memory for trace logging");
        }
    }

    if (pPort->m_pRoamTraceLoggingData != NULL)
    {
        UCHAR uTempLinkQuality = pPort->m_pRoamTraceLoggingData->linkQualityPreviousConnect;
        ULONGLONG uTempDisassociationTime = pPort->m_pRoamTraceLoggingData->disassociationIndicationTime;

        RtlZeroMemory(pPort->m_pRoamTraceLoggingData, sizeof(ROAM_TRACELOGGING_DATA));

        pPort->m_pRoamTraceLoggingData->roamAPRank = 0xFFFFFFFF; //Max
        pPort->m_pRoamTraceLoggingData->connectJobStartTime = CSystem::get_CurrentTime();
        pPort->m_pRoamTraceLoggingData->linkQualityPreviousConnect = uTempLinkQuality;
        pPort->m_pRoamTraceLoggingData->disassociationIndicationTime = uTempDisassociationTime;
    }

    //
    // Tell NDIS to complete the OID if necessary or roam job to send roam started indication
    //
    NotifyStarted(
        NDIS_STATUS_SUCCESS);

    //
    // Notify connection/roam started
    //
    NotifyConnectionStartIfNecessary();

    // If we got triggered due to disconnect but by the time this job
    // ran we have already connected, then don't run the job.
    if (TestRoamConfig(m_RoamConfigFlags, RC_RECONNECT))
    {
        CConnectedPeer* pPeer = pPort->GetConnectedPeerList()
            ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);

        if (pPeer != NULL)
        {
            WFCActInfo("[Roam] Skipping connect job since we are already connected");
            // The parent roam job suppresses this error (NDIS_STATUS_ALREADY_MAPPED) when connect job completes
            // since it is not really an error condition.
            status = NDIS_STATUS_ALREADY_MAPPED;
            goto exit;
        }
    }

    if (TestRoamConfig(m_RoamConfigFlags, RC_CONNECT))
    {
        pPort->SetDot11State(WfcPortDot11StateOp);
    }        

    // Set some locals up
    m_MediaStreamingMode = (m_pAdapter->GetAdapterPropertyCache()->GetPropertyULongOrDefault(
                                    WfcAdapterPropertyMediaStreamingRefcount, 0) > 0) ? true:  false;

    m_PerformFullPassiveScan = (m_pAdapter->GetAdapterPropertyCache()->GetPropertyULongOrDefault(
                    WfcAdapterPropertyRegRoamFullPassiveScan, 0) > 0) ? true : false;
                    
    status = FillConnectionProfileParameters(GetPortPropertyCache(), &m_ConnectionProfile);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Failed to create connection parameters from property cache.");
        goto exit;
    }
    
    if (TestRoamConfig(m_RoamConfigFlags, RC_CONNECT) && !m_IsP2PConnect)
    {
        // Test does not always configure the privacy exemption list. 
        // Check if we should do it
        status = StartSetPrivacyExemptionIfNecessary(&bPrivacyExemptionSent);
        if (status != NDIS_STATUS_SUCCESS)
        {
            // Since this code is only a workaround for a test, we 
            // ignore failures in this
            WFCActErrorStatus("Ignoring failure to determine if privacy exemption needs to be set");
            status = NDIS_STATUS_SUCCESS;
            bPrivacyExemptionSent = false;
        }
    }

    if (!bPrivacyExemptionSent)
    {
        status = StartBssListUpdate();
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Failed to start BSS list update for connect");
            goto exit;
        }
    }

exit:
    if (status != NDIS_STATUS_SUCCESS)
    {
        FinishJob(
            status);
    }
}


void CConnectJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    bool completeJob = false;
    bool bAttemptConnect = false;
    bool bAttemptScan = false;
    WDI_ASSOC_STATUS nextStepStatus = WDI_ASSOC_STATUS_CANDIDATE_LIST_EXHAUSTED;
    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    UNREFERENCED_PARAMETER(SenderObject);

    TraceActEntry();
    WFCActTrace("[Connect] Step %d completed with 0x%08x", m_ConnectState, OperationStatus);

    switch (m_ConnectState)
    {
        case WiFiConnectJobStateWaitingForPrivacyExemptionListSet:
            {
                WFCActTrace("[Connect] Privacy exemption set completed with status 0x%08x", OperationStatus);
                if (pPort->m_pRoamTraceLoggingData != NULL)
                {
                    pPort->m_pRoamTraceLoggingData->timeToFinishPrivacyExemptionListSetMS = (CSystem::get_CurrentTime() - pPort->m_pRoamTraceLoggingData->connectJobStartTime) / 10000;
                }

                // Status is ignore
                OperationStatus = StartBssListUpdate();
                if (OperationStatus != NDIS_STATUS_SUCCESS)
                {
                    WFCActError("Failed to start BSS list update for connect. Error = 0x%08x", OperationStatus);
                    
                    // We will give up on the connect attempt
                }
            }
            break;

        case WiFiConnectJobStateWaitingForBSSListUpdate:
            {
                WFCActTrace("[Connect] BSS list update completed with status 0x%08x", OperationStatus);
                if (pPort->m_pRoamTraceLoggingData != NULL)
                {
                    pPort->m_pRoamTraceLoggingData->timeToFinishBSSListUpdateMS = (CSystem::get_CurrentTime() - pPort->m_pRoamTraceLoggingData->connectJobStartTime) / 10000;
                }

                // Even if BSS list update fails, we will continue scan/connect
                // Unless we are RR
                if ( m_pAdapter->m_lResetRecovery ) 
                {
                    WFCActError( "Give up connect for BSSListUpdate hang");
                    // BSS list update may finish internally without going to control plane (which would have given a failure)
                    if (OperationStatus == NDIS_STATUS_SUCCESS)
                        OperationStatus = NDIS_STATUS_DEVICE_FAILED;
                }
                else
                {
                    if (m_IsP2PConnect && m_IsRoam)
                    {
                        WFCActInfo("[Connect][Roam] P2P Roam trying scan first");
                    }
                    else
                    {
                        bAttemptConnect = true;
                    }

                    bAttemptScan = true;
                }
            }
            break;

        case WiFiConnectJobStateWaitingForScanJobCompletion:
            {
                WFCActInfo("[Connect][Roam] Scan job completed with status 0x%08x", OperationStatus);
                if (pPort->m_pRoamTraceLoggingData != NULL)
                {
                    pPort->m_pRoamTraceLoggingData->timeToFinishScanJobCompletionMS = (CSystem::get_CurrentTime() - pPort->m_pRoamTraceLoggingData->connectJobStartTime) / 10000;

                }

                OperationStatus = CompleteScanJob(OperationStatus);
                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    //If the scan completed then cancel the opportunistic roam timer (if running)
                    pPort->CancelOpportunisticRoam();

                    bAttemptConnect = true;

                    if (m_IsP2PConnect && m_IsRoam)
                    {
                        WFCActInfo("[Connect][Roam] Not attempting any additional scans for P2P Roam");
                    }
                    else
                    {
                        bAttemptScan = true;
                    }
                }
                else
                {
                    WFCActError("[Connect] Scan complete failed. Error = 0x%08x", OperationStatus);
                }
            }
            break;

        case WiFiConnectJobStateWaitingForActionFrameComplete:
        {
            CompleteSendResponseActionFrameJob(OperationStatus);

            // Done with sending BSS transition Management Response frame (11v).
            // Ignore errors if any in sending the frame and proceed to PMKID update step.
            WFCActTrace("[Connect] Found %d usable candidates for connect", m_TempBSSCandidateCount);
            bool bPMKIDPending = CheckAndRequestPMKIDUpdate(
                m_TempBSSCandidateCount,
                m_TempBSSCandidateList
                );
            if (bPMKIDPending)
            {
                // PMKID list is pending, wait for it to get configured
                WFCActInfo("PMKID update is needed but not yet completed. Waiting for it before continuing connect");

                OperationStatus = StartWaitForPMKIDList();
                if (OperationStatus != NDIS_STATUS_SUCCESS)
                {
                    WFCActError("Failed to start waiting for PMKID list. Error = 0x%08x", OperationStatus);
                    OperationStatus = NDIS_STATUS_SUCCESS;
                    // Continue with the call
                }
                else
                {
                    // We are waiting for the PMKID list update to happen
                    m_ConnectState = WiFiConnectJobStateWaitingForPMKIDUpdate;
                    SetPending();
                    break;
                }
            }
            else
            {
                WFCActTrace("PMKID already configured or NOT needed. Proceeding with the connect");
            }

            OperationStatus = StartConnectRoamTask(
                START_CONNECT_ROAM_CANDIDATE_LIST_AVAILABLE,
                START_CONNECT_ROAM_STOP_IF_NO_CANDIDATES_FOUND,
                &nextStepStatus);

            if (OperationStatus == NDIS_STATUS_SUCCESS)
            {
                if (nextStepStatus == WDI_ASSOC_STATUS_SUCCESS)
                {
                    // Started connect, we are done for now
                    WFCActTrace("Initiated attempt to connect");
                }
                else if (OperationStatus != NDIS_STATUS_DEVICE_FAILED) // if !FW hung
                {
                    // Didnt start connect. We can see if a scan helps
                    bAttemptScan = true;
                }
            }
            else
            {
                WFCActError("[Connect] Connect start failed. Error = 0x%08x", OperationStatus);
            }
            break;
        }

        case WiFiConnectJobStateWaitingForPMKIDUpdate:
            {                
                WFCActTrace("[Connect] PMKID update process completed with status 0x%08x", OperationStatus);

                if (pPort->m_pRoamTraceLoggingData != NULL)
                {
                    pPort->m_pRoamTraceLoggingData->timeToFinishPMKIDUpdateMS = (CSystem::get_CurrentTime() - pPort->m_pRoamTraceLoggingData->connectJobStartTime) / 10000;

                }

                OperationStatus = StartConnectRoamTask(
                                        START_CONNECT_ROAM_RECREATE_CANDIDATE_LIST,
                                        START_CONNECT_ROAM_STOP_IF_NO_CANDIDATES_FOUND,
                                        &nextStepStatus);

                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    if (nextStepStatus == WDI_ASSOC_STATUS_SUCCESS)
                    {
                        // Started connect, we are done for now
                        WFCActTrace("Initiated attempt to connect");
                    }
                    else if ( OperationStatus != NDIS_STATUS_DEVICE_FAILED ) // if !FW hung
                    {
                        // Didnt start connect. We can see if a scan helps
                        bAttemptScan = true;
                    }
                }
                else
                {
                    WFCActError("[Connect] Connect start failed. Error = 0x%08x", OperationStatus);
                }
                
            }
            break;

        case WiFiConnectJobStateWaitingForConnectCompletion:
            {
                WFCActInfo("[Connect][Roam] Connect Task Completed with status 0x%08x", OperationStatus);

                OperationStatus = CompleteConnectTask(OperationStatus);
                
                if (m_NotifiedSuccessfulAssociation)
                {
                    //
                    // Connect is done. Complete the job.
                    //
                    completeJob = true;
                }
                else 
                {
                    if (m_IsReconnectNeededDueTo11r)
                    {
                        if (OperationStatus != NDIS_STATUS_DEVICE_FAILED)
                        {
                            WFCActTrace("[11r] Triggering reconnect due to failed 11r association");
                            if (m_IsBlockSSIDFor11rNeeded)
                            {
                                WFCActTrace("[11r] Blocking SSID for 11r due to failure");
                                pPort->BlockSSID(
                                    m_SSID,
                                    WFC_BLOCKED_REASON_ASSOC_REJECTED_OR_PEER_DISCONNECTED
                                    );
                                pPort->ClearFTPropertyCache();
                            }

                            completeJob = true;

                            pPort->TriggerReconnect(
                                WDI_ASSOC_STATUS_ROAMING_ASSOCIATION_LOST,
                                WfcRoamConnectTriggerAssociationLoss,
                                CConnectHelpers::GetRoamConfig(WfcRoamConnectTriggerAssociationLoss));
                        }
                    }
                    else
                    {
                        WFCActTrace(
                            "[Connect] Connect command failed/successful association was not indicated. Trying to reconnect best-effort. (commandStatus = %x)",
                            OperationStatus);

                        // We havent yet finished. We can see if a scan helps
                        if (OperationStatus != NDIS_STATUS_DEVICE_FAILED)
                        {
                            bAttemptScan = true;
                        }
                    }
                }
            }

            break;
            
        default:
            NT_ASSERT(FALSE);
    }

    // If we are allowed, we attempt scan or connect irrespective of 
    // OperationStatus above since steps above could be saying failure
    if (!m_IsCancelled && bAttemptConnect)
    {        
        OperationStatus = CheckAndStartConnectProcess(&nextStepStatus);
        if (OperationStatus == NDIS_STATUS_SUCCESS)
        {
            // We were able to check, determine what happened in the check
            if (nextStepStatus == WDI_ASSOC_STATUS_SUCCESS)
            {
                // Started connect, we are done for now
                WFCActTrace("[Connect] Initiated connect process");

                // We wont attempt the scan
                bAttemptScan = false;
            }
            else
            {
                WFCActTrace("[Connect] Connect process didnt launch due to status %d", nextStepStatus);
                // Connect didnt finish, continue using the 
                // bAttemptScan set above to determine if we should 
                // should scan
            }
        }
        else
        {
            WFCActInfo("[Connect] Attempt to determine if we can continue with connect completed with Status = 0x%08x", OperationStatus);

            // If we failed, we are giving up
            bAttemptScan = false;
        }
    }

    if (!m_IsCancelled && bAttemptScan)
    {
        OperationStatus = CheckAndStartScanProcess(&nextStepStatus);
        if (OperationStatus == NDIS_STATUS_SUCCESS)
        {
            // We were able to check, determine what happened in the check
            if (nextStepStatus == WDI_ASSOC_STATUS_SUCCESS)
            {
                // Started scan, we are done for now
                WFCActTrace("[Connect] Initiated attempt to scan");
            }
            else
            {
                WFCActTrace("[Connect] Scan process didnt launch due to status %d", nextStepStatus);
            }
        }
        else
        {
            WFCActInfo("[Connect] Attempt to determine if we can continue with scan completed with Status = 0x%08x", OperationStatus);

            // On resume from DX roam (Hibernate/Standby) if we have given 
            // up on scanning tell the OS we are done
            // Status codes being checked here are describe below. 
            if ((OperationStatus == NDIS_STATUS_NETWORK_UNREACHABLE) &&
                (nextStepStatus == WDI_ASSOC_STATUS_CANDIDATE_LIST_EXHAUSTED) &&
                (TestRoamConfig(m_RoamConfigFlags, RC_RESUME_FROM_DX)))
            {
                // This can be done for non-resume cases also, but for now we are only doing it for resume
                WFCActInfo("[Roam] Notify OS that we are no longer attempting roam on resume");
                NotifyRoamingAttemptExhausted();
            }
        }

    }

    // If we havent started a roam because we didnt find any "good" candidates,
    // we do additional processing for Ihv triggered roam
    if (bAttemptConnect || bAttemptScan)
    {
        // Written this way to make debugging easier

        // SelfTrigger is also here to support upgrade. The WfcPortPropertyIhvRoamRequested
        // would only be set if its IHV triggered
        if (TestRoamConfig(m_RoamConfigFlags, RC_CONNECTED_ROAM))
        {
            // NDIS_STATUS_SUCCESS = We didnt have any problem that requires us to stop
            // NDIS_STATUS_NETWORK_UNREACHABLE = Already tried too many times = Give it one more last try
            // Anything else we had a problem = Dont do anything
            if ((OperationStatus == NDIS_STATUS_SUCCESS) ||
                (OperationStatus == NDIS_STATUS_NETWORK_UNREACHABLE))
            {
                // WDI_ASSOC_STATUS_SUCCESS = We are still working on it
                // WDI_ASSOC_STATUS_CANDIDATE_LIST_EXHAUSTED = We are about to give up
                // Anything else we had a problem = Dont do anything
                if (nextStepStatus == WDI_ASSOC_STATUS_CANDIDATE_LIST_EXHAUSTED)
                {
                    // For 11v roams with Disassociation Imminent bit set, continue trying to roam even if there are no suitable candidates
                    // Also check if we have attempted this roam task with START_CONNECT_ROAM_CONTINUE_IF_NO_CANDIDATE_FOUND before. If so, do not start the task.
                    // If the job has been deferred, don't attempt the roam task just yet. The job will be requeued again.
                    if ((GetPortPropertyCache()->GetPropertyBooleanOrDefault(WfcPortPropertyIhvRoamRequested, FALSE) ||
                         TestRoamConfig(m_RoamConfigFlags, RC_BSS_TRANSITION_MANAGEMENT_REQUEST_CRITICAL)               ) &&
                        (!m_IsCriticalRoamToLEAlreadyAttempted)                                                           &&
                        (!m_IsDeferred)                                                                                      )
                    {
                        WFCActTrace("[Roam][IHV][BetterAP] Don't have any better APs. Allowing IHV to determine if they want to roam");
                        m_IsCriticalRoamToLEAlreadyAttempted = true;
                        OperationStatus = StartConnectRoamTask(
                                                START_CONNECT_ROAM_RECREATE_CANDIDATE_LIST,
                                                START_CONNECT_ROAM_CONTINUE_IF_NO_CANDIDATE_FOUND,
                                                &nextStepStatus);   // Continue roam even if we dont have any candidates
                        if (OperationStatus != NDIS_STATUS_SUCCESS)
                        {
                            WFCActError("[Roam][IHV][BetterAP] Failed to continue Better AP roam. Error = 0x%08x", OperationStatus);
                        }
                    }
                }
                else if (nextStepStatus != WDI_ASSOC_STATUS_SUCCESS)
                {
                    WFCActInfo("[Roam][BetterAP] Failed in starting step for IHV triggered better AP roam. Next Step Status = %u", nextStepStatus);
                }
            }
            else
            {
                WFCActInfo("[Roam][BetterAP] Failed preparing for IHV triggered better AP roam. Error = 0x%08x", OperationStatus);
                // Continue below
            }

        }
    }

    // Catch all to determine when to fail
    if ((bAttemptConnect || bAttemptScan) && 
        (OperationStatus == NDIS_STATUS_SUCCESS) &&
        (nextStepStatus != WDI_ASSOC_STATUS_SUCCESS))
    {
        WFCActInfo("[Connect] Failed to proceed with scan or connect. Stopping connect");
        OperationStatus = NDIS_STATUS_FAILURE;
    }
    

    if ((OperationStatus != NDIS_STATUS_SUCCESS) ||
        (completeJob))
    {
        FinishJob(OperationStatus);
    }

    TraceActExit(OperationStatus);
}

void CConnectJob::OnJobCancelled()
{
    TraceActEntry();

    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    if (m_IsCancelled)
    {
        goto exit;
    }

    m_IsCancelled = true;

    //
    // The PortId is set in CJobBase::OnStartJobEventHandler, so if the job was cancelled before
    // CJobBase::OnStartJobEventHandler ran, then pPort can be NULL
    // so check for that
    //
    if (!pPort)
    {
        goto exit;
    }

    if (m_IsRoam && pPort->m_pRoamTraceLoggingData != NULL && 
        pPort->m_pRoamTraceLoggingData->roamOccured)
    {
        pPort->m_pRoamTraceLoggingData->isCancelled = true;
        pPort->m_pRoamTraceLoggingData->connectJobEndTime = CSystem::get_CurrentTime();
        pPort->TraceLoggingRoamFinished(STATUS_CANCELLED);
    }

    if (m_ConnectState != WiFiConnectJobStateInit &&
        m_ConnectState != WiFiConnectJobStateWaitingForPMKIDUpdate &&
        m_ConnectState != WiFiConnectJobStateCompleted)
    {
        CancelActivePendingCommand();
    }
    else
    {
        //
        // Cancel called when there was nothing pending.
        //
    }

exit:
    TraceActExit(NDIS_STATUS_SUCCESS);
}

void CConnectJob::OnDeviceIndicationArrived(
    _In_ WDI_INDICATION_TYPE                 IndicationType,
    _In_ WFC_COMMAND_TOKEN                   CommandToken,
    _In_ WFC_PORT_ID                         PortId,
    _In_ ULONG                               EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer)
{
    UNREFERENCED_PARAMETER(CommandToken);
    UNREFERENCED_PARAMETER(PortId);

    switch (IndicationType)
    {
    case WiFiIndicationAssociationResult:
        {
            OnAssociationNotificationArrived(
                EventBufferLength,
                EventBuffer);
            break;
        }
    case WiFiIndicationPmkIdCandidateListUpdate:
        {
           OnPMKIDListUpdateNotificationArrived(
               EventBufferLength,
               EventBuffer);
           break;
        }
    case WiFiIndicationBssEntryList:
        {
            OnBssEntryListNotification();
            break;
        }
    }

}

// Returns failure only if there is a processing error. Is success
// if entries were found
NDIS_STATUS CConnectJob::CheckAndUpdateCandidates()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    CPortPropertyCache *pPropertyCache = GetPortPropertyCache();

    PROAM_CONTROL_PARAMETERS pRoamControl = CConnectHelpers::GetRoamControlParameters(
        m_pAdapter->GetAdapterPropertyCache());

    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    CConnectedPeer* pPeer = pPort->GetConnectedPeerList()
        ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);
    ULONG APLinkQualityBarAdjustment = 0;

    TraceActEntry();

    // Step 1 - Determine if we have any candidates

    //In the case that we are calling CheckAndUpdateCandidates in order to determine
    // if a scan should be canceled the scan would be directed so it is acceptable 
    // to call PickCandidates when the relevant AP BSS Entries come in.
    status = PickCandidates(&m_ConnectionProfile);
    if (status != NDIS_STATUS_SUCCESS)
    {
        // This is a fatal error in selecting candidate. The function would have returned success 
        // even if 0 candidates were found
        WFCActErrorStatus("Couldn't pick candidates.");
        goto exit;
    }

    if (m_TempBSSCandidateCount == 0)
    {
        // No candidates found. We will just bail out of here
        WFCActTrace("[Connect] No candidates found for connect");

        // [11v] If Scan has finished, send response BSS Transition frame to reject the request, 
        // as we do not have candidates for connect (We wait for a full scan to be completed to conclude that
        // we do not have any candidates for connect)
        // An exception to this is 11v critical roams, where we send roam task even if there arent any candidates found.
        // Logic in FinishJob handles sending of reject frame for this case
        if ((TestRoamConfig(m_RoamConfigFlags, RC_BSS_TRANSITION_MANAGEMENT_REQUEST))           &&
            (!TestRoamConfig(m_RoamConfigFlags, RC_BSS_TRANSITION_MANAGEMENT_REQUEST_CRITICAL)) &&
            (m_LastScanType >= WfcConnectScanTypeFull)                                          &&
            (m_pScanJob == NULL)                                                                   )
        {
            status = pPort->SendResponseBSSTransitionFrame(
                WDI_BSS_TRANSITION_REJECT_NO_SUITABLE_BSS_CANDIDATES,
                NULL
                );
            if (status != NDIS_STATUS_SUCCESS)
            {
                WFCActTrace("[Roam][BetterAP][11v] Failed to send response action frame to reject the request. Error: 0x%08x.", status);
            }
        }
        goto exit;
    }
    WFCActTrace("[Connect] Found %d potential candidates for connect", m_TempBSSCandidateCount);

    NT_ASSERT(m_TempBSSCandidateCount > 0);

    //[11v] For critical roam (AP had set the disassociation imminent bit), we need to roam even if the signal quality does not meet the bar.
    // Wait till scan finishes before we ignore signal quality
    bool bIgnoreSignalQuality = 
        (TestRoamConfig(m_RoamConfigFlags, RC_BSS_TRANSITION_MANAGEMENT_REQUEST_CRITICAL)) && (m_pScanJob == NULL); 

    // 
    // Step 2 - Determine if the candidates are good enough
    //

    // Determine the adjustment factor for LinkQuality checks below. This factor is higher
    // in earlier scan and lower later (so we dont stop too early from a partial results)
    APLinkQualityBarAdjustment = 0;
    if (m_LastScanType == WfcConnectScanTypeNone)
    {
        // This is 0 because we dont understand what may have happened recently
        // We may have done a scan recently or gotten a BSS list update from IHV
        APLinkQualityBarAdjustment =  0;
    }
    else if (m_LastScanType <= WfcConnectScanTypeChannelHint)
    {
        // High bar. We dont want to stop too early from our partial scans
        APLinkQualityBarAdjustment =  WFC_CONSTANT::RoamPartialScanLinkQualityAdjustment;
    }
    else if (m_LastScanType < WfcConnectScanTypeFull)
    {
        if (m_pScanJob)
        {
            // Scan still in progress. Use a high adjustment so we dont stop too early
            APLinkQualityBarAdjustment =  WFC_CONSTANT::RoamPartialScanLinkQualityAdjustment;
        }
    }

    // If we havent yet done a full scan, check if things are good
    if ((m_LastScanType < WfcConnectScanTypeFull))
    {
        if (TestRoamConfig(m_RoamConfigFlags, RC_CONNECT))
        {
            // First time connect. Determine if we have ever done a full
            // scan. This is to avoid connecting to an AP from an aborted bootup scan
            BYTE *goodScanTime;
            ULONG scanTimeBufferLength;

            ULONGLONG now = CSystem::get_CurrentTime();

            //
            // For CS NLO discovery triggered connect, we may have to do a scan to get best ap.
            // We assume its NLO discovery if its within a second of NLO indication from device.
            // For non-CS Sleep (legacy Standby and Hibernate), the NLO scan happened
            // after wakeup, so assume that AP is good enough to get the user connected
            // quickly and later our opportunistic better AP roam may move us. The only
            // way we know it was a non-CS sleep is based on whether we were armed for wake
            // or not
            //
            if ((m_NdisPortNumber == NDIS_DEFAULT_PORT_NUMBER) &&          // STA port
                ((now - pPort->m_ullLastNloDiscoverTime) < 10000000) &&    // One second
                pPropertyCache->GetPropertyBooleanOrDefault(WfcPortPropertyLastSleepWasWakeEnabled, TRUE))    // Wake Enabled Sleep
            {
                //
                // trigger a full scan
                //
                WFCActInfo("[Connect] NLO discovery connect; force a full scan");
                m_TempBSSCandidateCount = 0;
            }
            else if (pPropertyCache->GetPropertyBuffer(WfcPortPropertyGoodScanStartTime,
                                                       &scanTimeBufferLength,
                                                       &goodScanTime) == NDIS_STATUS_INVALID_STATE)
            {
                // This is a connect. Check if we have finished a good scan ever. We just check
                // the property cache. If we have finished a scan, this value would be populated
                WFCActInfo("[Connect] Not finished a good scan recently. Will need to scan");
                m_TempBSSCandidateCount = 0;
            }
        }
        else
        {
            // If this is a roam & we have only done a fast roam scan, determine if we have a
            // a good enough AP. If not, we will ignore the list we found and wait for a more complete
            // scan . This logic uses a higher bar for earlier (partial) scans and a lower bar for later scans

            // [11v] If the AP had indicated that it was going to send out the dissociation frame soon (Through BSS Transition Request Action Frame),
            // then roam irrespective of signal quality of the most preferred candidate list
            if (!bIgnoreSignalQuality)
            {
                if (m_TempBSSCandidateList[0]->GetSignalInfo()->LinkQuality <
                        (pRoamControl->BetterAPRoamLinkQualityThreshold + APLinkQualityBarAdjustment)
                    )
                {
                    WFCActTrace("[Roam] Roam candidate from non-full scan (%d) has low signal strength (%d) (Adjustment = %d). Wait for a full scan.",
                        m_LastScanType,
                        m_TempBSSCandidateList[0]->GetSignalInfo()->LinkQuality,
                        APLinkQualityBarAdjustment);
                    m_TempBSSCandidateCount = 0;
                }
            }
        }
    }

    if ((m_TempBSSCandidateCount > 0)                               &&
        (TestRoamConfig(m_RoamConfigFlags, RC_CHECK_GOOD_ENOUGH_AP))   )
    {
        // For moving to a better AP, we determine if this is really a better AP
        // Determine if there is a better AP
        if (pPeer != NULL)
        {
            ULONG curPeerIndex = 0;
            for (curPeerIndex = 0; curPeerIndex < m_TempBSSCandidateCount; curPeerIndex++)
            {
                if (m_TempBSSCandidateList[curPeerIndex]->IsBssIdEqual(*(pPeer->GetMacAddress())))
                {
                    break;
                }
            }
            // If the most preferred candidate is the first entry, then we are done
            if (curPeerIndex == 0)
            {
                    WFCActTrace("[Roam][BetterAP] Not roaming yet as Most preferred AP is the one we are connected to");
                    // Better AP is not found
                    m_TempBSSCandidateCount = 0;
                    goto exit;
            }
            
            // [11v] If the AP had indicated that it was going to send out the dissociation frame soon (Through BSS Transition Request Action Frame),
            // then roam irrespective of signal quality of the most preferred candidate list
            if (!bIgnoreSignalQuality)
            {
                // validate that peer is found in list
                if (curPeerIndex < m_TempBSSCandidateCount)
                {
                    // If the most preferred candidate does not have significantly better link quality, do not roam
                    // the rank of the best candidate is compared to the raw rank (before any penalties) of the peer to
                    // disregard if the peer was found in the scan or not
                    if (m_TempBSSCandidateList[0]->GetCachedRank() <
                        (m_TempBSSCandidateList[curPeerIndex]->GetCachedRawRank() + pRoamControl->APLinkQualitySignificantChangeBar))
                    {
                        WFCActTrace("[Roam][BetterAP] Ignoring candidate as rank of more preferred AP is not significantly higher (Current: %d, Preferred: %d)",
                            m_TempBSSCandidateList[curPeerIndex]->GetCachedRawRank(),
                            m_TempBSSCandidateList[0]->GetCachedRank());
                        m_TempBSSCandidateCount = 0;
                        goto exit;
                    }
                }
                else
                {
                    WFCActError("[Roam][BetterAP]Current peer not in candidate list.");
                }
            }
        }
        else
        {
            // Already disconnected from the old AP
            WFCActTrace("[Roam] Roaming as old association is already gone");
        }
    }

exit:
    TraceActExit(status);
    return status;
}
NDIS_STATUS CConnectJob::CheckAndStartConnectProcess(
    _Out_ WDI_ASSOC_STATUS* pConnectStartStatus)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());
        
    CConnectedPeer* pPeer = pPort->GetConnectedPeerList()
        ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);
    
    TraceActEntry();

    // Assume we dont proceed because we didnt find any entries
    *pConnectStartStatus = WDI_ASSOC_STATUS_CANDIDATE_LIST_EXHAUSTED;

    if (m_ConnectAttemptCount > (WfcConnectScanTypeMaximumAllowed + 1))
    {
        WFCActTrace("[Connect][Roam] Exhausted attempts to connect");
        status = NDIS_STATUS_NETWORK_UNREACHABLE;
        goto exit;
    }

    if (pPeer)
    {
        NT_ASSERT(!TestRoamConfig(m_RoamConfigFlags, RC_CONNECT));

        if (!TestRoamConfig(m_RoamConfigFlags,RC_CONNECTED_ROAM))
        {
            // We are not doing a better AP roam and are already connected, bail out
            WFCActTrace("[Roam] Already connected not proceeding with roam connect");
            status = NDIS_STATUS_ALREADY_MAPPED;   // This is a bad status, but failure stops
                                                   // us from continuing
            goto exit;
        }
    }

    //
    // Step 1 check and update candidate list
    //
    status = CheckAndUpdateCandidates();
    if (status != NDIS_STATUS_SUCCESS)
    {
        // This will give failure only if its couldnt do the update. Even if 0 were found
        // this would be success
        WFCActErrorStatus("Couldn't update candidates.");
        goto exit;
    }

    // Step 2 - If this is a 11v roam, make sure the action frame response is received by the AP before proceeding with the connect
    if (m_TempBSSCandidateCount > 0)
    {
        bool bActionFrameNotificationPending = false;
        bool bContinueConnect = true;
        bool bPMKIDPending = false;
        bool condition = false;

        *pConnectStartStatus = WDI_ASSOC_STATUS_SUCCESS;
        m_ConnectAttemptCount++;

        do
        {
            if (TestRoamConfig(m_RoamConfigFlags, RC_BSS_TRANSITION_MANAGEMENT_REQUEST))
            {
                if (!pPort->GetBSSTransitionFrameRequestReceived())
                {
                    WFCActTrace("[11v]Request frame hasn't been received yet or response already sent. Skipping to next step (PMKID update)");
                    break;
                }

                m_pResponseJob = new CBssTransitionResponseJob(get_ActivityId());
                if (m_pResponseJob == NULL)
                {
                    status = NDIS_STATUS_RESOURCES;
                    WFCActTrace("[11v]Couldn't create new pResponseJob. Skipping to next step (PMKID update)");
                    break;
                }

                m_pResponseJob->SetResponseFrameParameters(
                    pPort->GetBSSTransitionRequestDialogToken(),            //Use the dialog token from request frame
                    WDI_BSS_TRANSITION_ACCEPT
                    );

                //This is a WDI_BSS_TRANSITION_ACCEPT. So fill in the Target BSSID
                m_pResponseJob->SetTargetBssId(m_TempBSSCandidateList[0]->GetBssId());

                status = m_pResponseJob->Initialize(
                    m_pAdapter,
                    m_NdisPortNumber
                    );
                if (status != NDIS_STATUS_SUCCESS)
                {
                    WFCActTrace("[11v] Couldn't initialize pResponseJob. Skipping to next step (PMKID update)");
                    break;
                }

                WFCActTrace("[Roam][BetterAP][11v] Sending BSS Transition response frame - Accept");

                //Start the BSS Transition response job
                status = StartChildJob(m_pResponseJob);
                if (status != NDIS_STATUS_SUCCESS)
                {
                    WFCActError("[11v]Failed to start child job to send response Action Frame. Error = 0x%08x", status);
                    break;
                }

                //Wait for the action frame to be sent before sending the roam task
                //If that fails, we still will proceed to roam, as the AP has asked us to Roam with BSS Transition Management frame
                m_ConnectState = WiFiConnectJobStateWaitingForActionFrameComplete;
                bContinueConnect = false;
                bActionFrameNotificationPending = true;

                //Reset the variable so that duplicate response frames arent sent for same request frame
                pPort->SetBSSTransitionFrameRequestReceived(false);
            }
        } while (condition);

        //
        // Step 3 - Determine if we should be doing PMKIDs or Connect
        //

        // if something has gone wrong in sending action frame response (11v), clean up before proceeding.
        if (status != NDIS_STATUS_SUCCESS)
        {
            if (m_pResponseJob)
            {
                delete m_pResponseJob;
                m_pResponseJob = NULL;
            }
        }

        if (!bActionFrameNotificationPending)
        {
            // We are going to proceed with connect or PMKID set
            WFCActTrace("[Connect] Found %d usable candidates for connect", m_TempBSSCandidateCount);

            bPMKIDPending = CheckAndRequestPMKIDUpdate(
                m_TempBSSCandidateCount,
                m_TempBSSCandidateList
                );
            if (bPMKIDPending)
            {
                // PMKID list is pending, wait for it to get configured
                WFCActInfo("PMKID update is needed but not yet completed. Waiting for it before continuing connect");

                status = StartWaitForPMKIDList();
                if (status != NDIS_STATUS_SUCCESS)
                {
                    WFCActErrorStatus("Failed to start waiting for PMKID list");
                    status = NDIS_STATUS_SUCCESS;
                    // Continue with the call
                }
                else
                {
                    // We are waiting for the PMKID list update to happen
                    m_ConnectState = WiFiConnectJobStateWaitingForPMKIDUpdate;
                    SetPending();
                    bContinueConnect = false;
                    // Dont break out yet, we are still doing some work below
                }
            }
            else
            {
                WFCActTrace("PMKID already configured or NOT needed. Proceeding with the connect");
                bContinueConnect = true;
            }
        }

        // Update history with the list of candidates we found
        pPort->SaveNetworkCandidatesHistory(
            m_ConnectionProfile.SSIDList.pBuffer,
            m_LastScanType,
            m_TempBSSCandidateCount,
            m_TempBSSCandidateList
            );

        if (bContinueConnect)
        {
            if (pPeer != NULL)
            {
                pPort->m_pRoamTraceLoggingData->linkQualityDuringRoam = pPeer->GetLinkQuality();
            }

            // Continue the connect with the candidates we have found so far
            status = StartConnectRoamTask(
                START_CONNECT_ROAM_CANDIDATE_LIST_AVAILABLE,
                START_CONNECT_ROAM_STOP_IF_NO_CANDIDATES_FOUND,
                pConnectStartStatus);
        }
    }

exit:
    TraceActExit(status);
    return status;    
}

NDIS_STATUS CConnectJob::CheckAndStartScanProcess(
    _Out_ WDI_ASSOC_STATUS* pScanStartStatus)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    DOT11_SSID broadcastSSID;
    WFC_CONNECT_SCAN_TYPE nextScanType = WfcConnectScanTypeNone;
    WFC_SCAN_JOB_PARAMETERS scanJobParameters;
    CChannelListManager channelListManager;

    TraceActEntry();
    
    // Determine if we have scanned too often
    *pScanStartStatus = WDI_ASSOC_STATUS_CANDIDATE_LIST_EXHAUSTED;

    if (m_LastScanType >= WfcConnectScanTypeFull)
    {
        WFCActTrace("[Roam][Roam] Exhausted attempts to scan");
        status = NDIS_STATUS_NETWORK_UNREACHABLE;
        goto exit;
    }

    // We havent scanned too often, determine the next scan type
    //scanJobParameters.InitializeDefaults() will set the channel counts to 0 and
    // null the channel list pointers so code below only needs to update if those
    // defaults are not appropriate.
    scanJobParameters.InitializeDefaults();

    // Set next scan type
    if (TestRoamConfig(m_RoamConfigFlags, RC_CONNECT))
    {
        // For connect, we only do a full scan
        nextScanType = WfcConnectScanTypeFull;
    }
    else
    {
        // Try the next scan type
        nextScanType = (WFC_CONNECT_SCAN_TYPE)(m_LastScanType + 1);
    }

    // If the job has been asked to defer itself, and if next ScanType is one of the later ones,
    // defer (finish) this job. Roam will requeue it again.
    // Do this only if the roam is deferrable, else let this continue.

    if (TestRoamConfig(m_RoamConfigFlags, RC_DEFERRABLE_ROAM) &&
        (CJobBase::get_DeferHint() == true) &&
        (nextScanType > WfcConnectScanTypeChannelHint))
    {
        WFCActInfo("[Roam][Roam] Deferring roam - later scans will not run");
        m_IsDeferred = true;
        status = NDIS_STATUS_NETWORK_UNREACHABLE;
        goto exit;
    }

    //set forced scan
    if (TestRoamConfig(m_RoamConfigFlags, RC_BYPASS_SCAN_THROTTLING))
    {
        scanJobParameters.BypassScanThrottling = true;
    }
    else
    {
        scanJobParameters.BypassScanThrottling = false;
    }
    //set scan trigger
    if (TestRoamConfig(m_RoamConfigFlags, RC_CONNECTED_ROAM))
    {
        scanJobParameters.WdiScanParameters.ScanModeParameters.ScanTrigger = WDI_SCAN_TRIGGER_ROAM;
    }
    else
    {
        scanJobParameters.WdiScanParameters.ScanModeParameters.ScanTrigger = WDI_SCAN_TRIGGER_CONNECT;
    }

    //
    // Determine if we will do a partial scan for neighbor or instant connect or channel hints or move forward
    //
    if (nextScanType == WfcConnectScanTypeNeighborOrInstantConnect)
    {
        ULONG channelList24GHzCount =0;
        ULONG channelList5GHzCount =0;
        
        if (TestRoamConfig(m_RoamConfigFlags, RC_RESUME_FROM_DX))
        {
            //
            // roam for resume Dx, scan type is replaced with scanning previous AP channel
            //
            ASSERT( m_OidPortNumber == NDIS_DEFAULT_PORT_NUMBER );
            ULONG ulChannel, ulBandId=0;
            //
            // first time InstConn scan; default to start ActiveScan unless we dont have the last AP channel
            //
            ///use Cache Sta last connected
            status = m_pAdapter->GetAdapterPropertyCache()->GetPropertyULong(
                    (WFC_PROPERTY_NAME)WfcAdapterPropertyStaLastConnectedChannel,
                    &ulChannel);

            if ( status == NDIS_STATUS_SUCCESS) 
            {
                status = m_pAdapter->GetAdapterPropertyCache()->GetPropertyULong(
                    WfcAdapterPropertyStaLastConnectedBandId,
                    &ulBandId );
            }

            if ( ulBandId != 0 ) 
            {
                channelListManager.AddChannel(ulChannel, ulBandId);

                WFCActTrace("[Connect] Instant Connect: Previous AP channel:%lu,bandID:%lu",
                    ulChannel,
                    ulBandId);
            }
        }
        else
        {
            //
            // the original neighbor scan
            //

            status = m_pAdapter->GetBSSListManager(WfcPortTypeExtSTA)->GetChannelListsForNeighborScan(
                &channelListManager);
            if (status != NDIS_STATUS_SUCCESS)
            {
                WFCActInfo("[NeighborReport] Failed to create channel list for quick scan. Error = 0x%08x", status);
                // Failure would cause us to skip the neighbor scan
            }
        }

        // Check if either of the above were able to find a channel list to scan
        if (status == NDIS_STATUS_SUCCESS)
        {
            channelList24GHzCount = channelListManager.GetNumberOfChannels(WDI_BAND_ID_2400);
            channelList5GHzCount = channelListManager.GetNumberOfChannels(WDI_BAND_ID_5000);
            WFCTrace( "[NeighborReport] Received %d + %d Neighbor Scan Channels",
                channelList24GHzCount,
                channelList5GHzCount
                );
        }
        
        if ((channelList24GHzCount == 0) && 
            (channelList5GHzCount == 0))
        {
            // This might be a ChannelHint based scan
            nextScanType = (WFC_CONNECT_SCAN_TYPE)(nextScanType + 1);
                
            if (TestRoamConfig(m_RoamConfigFlags, RC_USE_CHANNEL_HISTORY))
            {
                // If neighbor report didnt give us anything and we are not doing a first time connect
                // check channel history for this network
                WFCActTrace("[ChannelHint] No channels available from neighbor reports. Checking channel history");
                status = m_pAdapter->GetPortFromPortId(get_PortId())->GetChannelListsFromChannelHistory(
                    m_ConnectionProfile.SSIDList.pBuffer, 
                    &channelListManager);
                if (status != NDIS_STATUS_SUCCESS)
                {
                    WFCActInfo("[ChannelHint] Failed to create channel list hint for quick scan. Error = 0x%08x. Skipping channel hint", status);

                    // Dont do the channel hint scan, not even with partial lists
                    // Both channelList24GHzCount & channelList5GHzCount are still at 0
                }
                else
                {                    
                    channelList24GHzCount = channelListManager.GetNumberOfChannels(WDI_BAND_ID_2400);
                    channelList5GHzCount = channelListManager.GetNumberOfChannels(WDI_BAND_ID_5000);
                }
                WFCTrace( "[ChannelHint] Received %d + %d Scan Channel Hints",
                    channelList24GHzCount,
                    channelList5GHzCount
                    );

                // Below we will check if this is a good list or should we just try the next scan
            }
        }
        else
        {
            // Will be a neighbor or instant connect scan
            WFCActTrace("[Connect] Searching BSSList for channels to use in neighbor scan resulted in %lu 2.4 GHz channels and %lu 5 GHz channels.",
                channelList24GHzCount,
                channelList5GHzCount);

        }
        
        if ((channelList24GHzCount == 0) && (channelList5GHzCount == 0))
        {
            // No channel from any source, try next scan (we should currently be on WfcConnectScanTypeChannelHint)
            nextScanType = (WFC_CONNECT_SCAN_TYPE)(nextScanType + 1);
        }
        else
        {
            if (channelList24GHzCount > 0)
            {
                scanJobParameters.ChannelList24GHzCount = channelList24GHzCount;
                scanJobParameters.pChannelList24GHz = channelListManager.GetChannelList(WDI_BAND_ID_2400);
            }

            if (channelList5GHzCount > 0)
            {
                scanJobParameters.ChannelList5GHzCount = channelList5GHzCount;
                scanJobParameters.pChannelList5GHz = channelListManager.GetChannelList(WDI_BAND_ID_5000);
            }
        }
    }
    else if (nextScanType == WfcConnectScanTypeChannelHint)
    {
        ASSERT(m_LastScanType == WfcConnectScanTypeNeighborOrInstantConnect);
        // We just did a neighbor or instant connect scan, skip the channel hint scan

        nextScanType = (WFC_CONNECT_SCAN_TYPE)(nextScanType + 1);
    }
    
    switch (nextScanType)
    {
        case WfcConnectScanTypeNeighborOrInstantConnect:
            if (m_MediaStreamingMode)
            {
                // Someone is in media streaming mode use shorter active channel dwell time
                scanJobParameters.WdiScanParameters.DwellTime.ActiveChannelDwellTime = 
                        WFC_CONSTANT::MediaStreamingOffChannelTimeLimit;

                if (TestRoamConfig(m_RoamConfigFlags, RC_RECONNECT))
                {
                    // Lost the connection, use longer scan time
                    scanJobParameters.WdiScanParameters.DwellTime.PassiveChannelDwellTime = 
                            WFC_CONSTANT::RoamScanLongPassiveChannelDwellTime;
                }
                else
                {
                    // Honor media streaming requirements. We may have
                    // lost association from the time of the trigger, but we are being
                    // conservative here
                    scanJobParameters.WdiScanParameters.DwellTime.PassiveChannelDwellTime =
                        WFC_CONSTANT::MediaStreamingOffChannelTimeLimit;                    
                }
            }
            else
            {
                // Since we have a good reason to believe the AP is there, use
                // longer dwell time
                scanJobParameters.WdiScanParameters.DwellTime.ActiveChannelDwellTime =
                    WFC_CONSTANT::RoamScanLongActiveChannelDwellTime;
                    
                scanJobParameters.WdiScanParameters.DwellTime.PassiveChannelDwellTime =
                    WFC_CONSTANT::RoamScanLongPassiveChannelDwellTime;                    
            }
            
            scanJobParameters.WdiScanParameters.ScanModeParameters.ScanTrigger = 
                WDI_SCAN_TRIGGER_FAST_ROAM;
            WFCActTrace("[Connect] Next scan type will be Neighbor/Instant Connect");
            break;

        case WfcConnectScanTypeChannelHint:

            if (m_MediaStreamingMode)
            {
                // Someone is in media streaming mode use shorter active channel dwell time
                scanJobParameters.WdiScanParameters.DwellTime.ActiveChannelDwellTime = 
                        WFC_CONSTANT::MediaStreamingOffChannelTimeLimit;         

                // Short passive channels too (irrespective of DFS preference)
                scanJobParameters.WdiScanParameters.DwellTime.ActiveChannelDwellTime = 
                        WFC_CONSTANT::MediaStreamingOffChannelTimeLimit;
            }
            else
            {
                // Best effort, but quick time is good enough for active scan
                scanJobParameters.WdiScanParameters.DwellTime.ActiveChannelDwellTime =
                    WFC_CONSTANT::RoamScanShortActiveChannelDwellTime;

                // Longer dwell time for passive channels because we have a channel hint
                scanJobParameters.WdiScanParameters.DwellTime.PassiveChannelDwellTime =
                    WFC_CONSTANT::RoamScanLongPassiveChannelDwellTime;
            }
            WFCActTrace("[Connect] Next scan type will be ChannelHint");
            break;
            

        case WfcConnectScanTypeActiveOnly:
            if (m_MediaStreamingMode)
            {
                // Someone is in media streaming mode use shorter active channel dwell time
                scanJobParameters.WdiScanParameters.DwellTime.ActiveChannelDwellTime = 
                        WFC_CONSTANT::MediaStreamingOffChannelTimeLimit;            

                // Short passive channels too (irrespective of DFS preference)
                scanJobParameters.WdiScanParameters.DwellTime.ActiveChannelDwellTime = 
                        WFC_CONSTANT::MediaStreamingOffChannelTimeLimit;
            }
            else
            {
                // Quick time is good enough for active scan
                scanJobParameters.WdiScanParameters.DwellTime.ActiveChannelDwellTime =
                    WFC_CONSTANT::RoamScanShortActiveChannelDwellTime;
                    
                // If DFS channels are to be searched for, use longer dwell time, else
                // finish quickly. We want to avoid doing this all the time since it 
                // will increase our total scan time
                if (m_PerformFullPassiveScan)
                    scanJobParameters.WdiScanParameters.DwellTime.PassiveChannelDwellTime =
                        WFC_CONSTANT::RoamScanLongPassiveChannelDwellTime;
                else
                    scanJobParameters.WdiScanParameters.DwellTime.PassiveChannelDwellTime =
                        WFC_CONSTANT::RoamScanShortPassiveChannelDwellTime;
            }

            WFCActTrace("[Connect] Next scan type will be Active Only");
            break;

        case WfcConnectScanTypeFull:
            if ((!TestRoamConfig(m_RoamConfigFlags, RC_CONNECT)) ||
                (m_MediaStreamingMode)                              )
            {
                // Use roam dwell times (shorter than default) for roam connects and 
                // for media streaming mode
                scanJobParameters.WdiScanParameters.DwellTime.ActiveChannelDwellTime =
                    WFC_CONSTANT::RoamScanLongActiveChannelDwellTime;
                    
                scanJobParameters.WdiScanParameters.DwellTime.PassiveChannelDwellTime =
                    WFC_CONSTANT::RoamScanLongPassiveChannelDwellTime;
            }
            else
            {
                // Use regular scan timers
                scanJobParameters.WdiScanParameters.DwellTime.ActiveChannelDwellTime =
                    WFC_CONSTANT::DefaultActiveChannelDwellTime;
                    
                scanJobParameters.WdiScanParameters.DwellTime.PassiveChannelDwellTime =
                    WFC_CONSTANT::DefaultPassiveChannelDwellTime;

            }

            WFCActTrace("[Connect] Next scan type will be Full");
            break;        
    }
    //Include SSID if this is not a first time connect and this is NOT a resume from sleep/hibernate (privacy reasons)
    // or if this is for a hidden network
    if (TestRoamConfig(m_RoamConfigFlags, RC_INCLUDE_SSID) || m_ConnectionProfile.HiddenNetwork)
    {
        // Use SSID for scan
        scanJobParameters.pSSIDList = m_ConnectionProfile.SSIDList.pBuffer;
        scanJobParameters.SSIDListCount = m_ConnectionProfile.SSIDList.ElementCount;
    }
    else
    {
        // Use broadcast SSID for scan
        RtlZeroMemory(&broadcastSSID, sizeof(DOT11_SSID));

        broadcastSSID.uSSIDLength = 0;

        scanJobParameters.pSSIDList = &broadcastSSID;
        scanJobParameters.SSIDListCount = 1;
    }

    m_LastScanType = nextScanType;

    // Set this scan to do only passive if instructed by upper layers (wlansvc)
    BOOLEAN bIsPassiveOnly = false;
    bIsPassiveOnly = m_pAdapter->GetAdapterPropertyCache()->GetPropertyBooleanOrDefault(WfcAdapterPropertyScanModePassiveOnly, FALSE);

    if (bIsPassiveOnly &&
        scanJobParameters.WdiScanParameters.ScanModeParameters.ScanTrigger != WDI_SCAN_TRIGGER_MANUAL)
    {
        scanJobParameters.WdiScanParameters.ScanModeParameters.AllowedScanType = WDI_SCAN_TYPE_PASSIVE_ONLY;
    }

    status = StartScanJob(&scanJobParameters);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't start scan job.");
        goto exit;
    }
    // Scan has started
    *pScanStartStatus = WDI_ASSOC_STATUS_SUCCESS;

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CConnectJob::FillConnectionProfileParameters(
    _In_ CPortPropertyCache* pPortPropertyCache,
    _In_ WFC_CONNECTION_PROFILE_PARAMETERS* pConnectParameters
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    TraceActEntry();

    status = CConnectHelpers::PopulateConnectionParameters(
        pPortPropertyCache,
        pConnectParameters
        );

    if (status != NDIS_STATUS_SUCCESS)
        goto exit;


    if (pConnectParameters->HostFIPSModeEnabled)
    {
        pPort->UpdateTaskOffloadCurrentConfigForFIPS(true);
        status = pPort->GetPropertyCache()->SetPropertyBoolean(
            WfcPortPropertyTaskOffloadsDisabledForFIPS,
            true
            );
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Failed to set property cache for WfcAdapterPropertyTaskOffloadsDisabledForFIPS");
        }
    }
    else
    {
        BOOLEAN bTaskOffloadDisabled = false;
        status = pPort->GetPropertyCache()->GetPropertyBoolean(
            WfcPortPropertyTaskOffloadsDisabledForFIPS,
            &bTaskOffloadDisabled
            );
        if (status != NDIS_STATUS_SUCCESS ||
            bTaskOffloadDisabled)
        {
            WFCInfo("Setting the TCP offload capabilities to default");
            pPort->UpdateTaskOffloadCurrentConfigForFIPS(false);

            //Reset the property cache
            status = pPort->GetPropertyCache()->SetPropertyBoolean(
                WfcPortPropertyTaskOffloadsDisabledForFIPS,
                FALSE
                );
            if (status != NDIS_STATUS_SUCCESS)
            {
                WFCActErrorStatus("Failed to set property cache for WfcAdapterPropertyTaskOffloadsDisabledForFIPS");
            }
        }
    }

    if (m_IsP2PConnect)
    {
        DOT11_WFD_GROUP_ID* pGroupId = NULL;
        DOT11_WFD_GROUP_JOIN_PARAMETERS* pGroupJoinParameters;
        ULONG bufferLength;

        pConnectParameters->SSIDList.ElementCount = 0;
        pConnectParameters->SSIDList.pBuffer = FALSE;

        pConnectParameters->HostFIPSModeEnabled = FALSE;

        status = pPortPropertyCache->GetPropertyBuffer(
            WfcPortPropertyP2PDesiredGroupID,
            &bufferLength,
            (BYTE**)&pGroupId);

        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Couldn't get desired group ID from property cache.");
            goto exit;
        }

        NT_ASSERT(bufferLength >= sizeof(*pGroupId));

        RtlCopyMemory(
            &m_GroupIdStorage,
            pGroupId,
            sizeof(m_GroupIdStorage));

        pConnectParameters->SSIDList.ElementCount = 1;
        pConnectParameters->SSIDList.pBuffer = &m_GroupIdStorage.SSID;

        status = pPortPropertyCache->GetPropertyBuffer(
            WfcPortPropertyP2PGroupJoinParameters,
            &bufferLength,
            (BYTE**)&pGroupJoinParameters);

        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Couldn't get desired group ID from property cache.");
            goto exit;
        }

        NT_ASSERT(bufferLength >= sizeof(*pGroupJoinParameters));

        m_RequireSelectedRegistrar = pGroupJoinParameters->bWaitForWPSReady ? true : false;
        m_RequireGroupFormationBit = pGroupJoinParameters->bInGroupFormation ? true : false;

        if (TestRoamConfig(m_RoamConfigFlags, RC_CONNECT))
        {
            RtlCopyMemory(
                &m_GOChannelHint,
                &pGroupJoinParameters->GOOperatingChannel,
                sizeof(m_GOChannelHint));
        }
    }

    // Determine if the network support PMKIDs (default = FALSE)
    for( UINT16 i = 0; i < pConnectParameters->AuthAlgoList.ElementCount; ++i )
    {
        if ( pConnectParameters->AuthAlgoList.pBuffer[i] == DOT11_AUTH_ALGO_RSNA )
        {
            status = pPortPropertyCache->SetPropertyBoolean(WfcPortPropertyPMKIDSupported, TRUE);
            if (status != NDIS_STATUS_SUCCESS)
            {
                WFCActErrorStatus("Failed to save state about PMKID support");
            }
            break;
        }
    }    

    WFCInfo("[Connect] SSID = %!DOT11SSID!", pConnectParameters->SSIDList.pBuffer);
    WFCInfo("[Connect] Auth Algo = %!HEXDUMP!", LOG_HEXDUMP(pConnectParameters->AuthAlgoList.ElementCount * sizeof(DOT11_AUTH_ALGORITHM), pConnectParameters->AuthAlgoList.pBuffer));
    WFCInfo("[Connect] Multicast Cipher Algo = %!HEXDUMP!", LOG_HEXDUMP(pConnectParameters->MulticastCipherAlgoList.ElementCount * sizeof(DOT11_CIPHER_ALGORITHM), pConnectParameters->MulticastCipherAlgoList.pBuffer));
    WFCInfo("[Connect] Unicast Cipher Algo = %!HEXDUMP!", LOG_HEXDUMP(pConnectParameters->UnicastCipherAlgoList.ElementCount * sizeof(DOT11_CIPHER_ALGORITHM), pConnectParameters->UnicastCipherAlgoList.pBuffer));


exit:
    TraceActExit(status);

    return status;
}


NDIS_STATUS CConnectJob::PickCandidates(
    _In_ WFC_CONNECTION_PROFILE_PARAMETERS  *pConnectionParameters
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    BSS_ENTRY_CONNECT_MATCHING_CRITERIA matchingCriteria = { 0 };
    WFC_PORT_TYPE scanCacheSource = WfcPortTypeExtSTA;
    ULONG maxAgeOfEntries100nS = WFC_CONSTANT::ConnectBssEntryExpiryTime;

    // Determine what the max age would be for AP's
    if (TestRoamConfig(m_RoamConfigFlags, RC_USE_ROAM_EXPIRY_TIME))
    {
        if (m_LastScanType < WfcConnectScanTypeFull)
        {
            // For short scans, use partial scan bss entry expiry time
            maxAgeOfEntries100nS = WFC_CONSTANT::PartialScanBssEntryExpiryTime;
        }
        else
        {
            // When roaming, only allow the newest entries to be used
            // 5 seconds (a scan is normally 4 seconds, add some buffer)
            maxAgeOfEntries100nS = WFC_CONSTANT::RoamBssEntryExpiryTime;
        }
    }
    else
    {
        // Allow old entries to be used
        maxAgeOfEntries100nS = WFC_CONSTANT::ConnectBssEntryExpiryTime;
    }

    if (TestRoamConfig(m_RoamConfigFlags, RC_CONNECTED_ROAM))
    {
        // For Better AP roam, even if we dont see the current AP,
        // include it in the list
        CPort* pPort = m_pAdapter->GetPortFromPortId(
            get_PortId());

        CConnectedPeer* pPeer = pPort->GetConnectedPeerList()
            ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);

        if (pPeer)
        {
            if (!TestRoamConfig(m_RoamConfigFlags,RC_BSS_TRANSITION_MANAGEMENT_REQUEST))
            {
                 matchingCriteria.bIncludeCurrentPeer = true;
            }
            else
            {
                matchingCriteria.bIncludeCurrentPeer = false;
            }

            //[11v] For critical roam (AP had set the disassociation imminent bit), 
            // we need to roam even if the signal quality does not meet the bar.
            if (TestRoamConfig(m_RoamConfigFlags, RC_BSS_TRANSITION_MANAGEMENT_REQUEST_CRITICAL))
            {
                matchingCriteria.bPreferToStayWithCurrentPeer = false;
            }
            else
            {
                matchingCriteria.bPreferToStayWithCurrentPeer = true;
            }
        }
    }

    if (m_IsP2PConnect)
    {
        scanCacheSource = WfcPortTypeWFDDevice;
        matchingCriteria.pGroupIdMatch = &m_GroupIdStorage;
        matchingCriteria.pRequireSelectedRegistrar = &m_RequireSelectedRegistrar;
        matchingCriteria.pRequireGroupFormation = &m_RequireGroupFormationBit;
    }

    matchingCriteria.pConnectionParameters = pConnectionParameters;

    m_TempBSSCandidateCount = celems(m_TempBSSCandidateList);

    status = m_pAdapter->GetBSSListManager(scanCacheSource)->FindMatchingBSSEntriesForConnect(
        &matchingCriteria,
        maxAgeOfEntries100nS,
        &m_TempBSSCandidateCount,
        m_TempBSSCandidateList);


    WFCTrace("[Connect] Selected %d BSSIDs", m_TempBSSCandidateCount);
    return status;
}

NDIS_STATUS CConnectJob::StartSetPrivacyExemptionIfNecessary(
    _Out_ bool *pPrivacyExemptionSent
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG bufferSize;
    UINT16 authAlgoCount;
    DOT11_AUTH_ALGORITHM* pAuthAlgos;
    UINT16 privacyListCount;
    DOT11_PRIVACY_EXEMPTION* pPrivacyExemptTypes;
    bool needsEAP = false;
    CPortPropertyCache *pPropertyCache = GetPortPropertyCache();
    WDI_SET_PRIVACY_EXEMPTION_LIST_PARAMETERS privacyExemptionListParameters;

    RtlZeroMemory(&privacyExemptionListParameters, sizeof(privacyExemptionListParameters));

    TraceEntry();

    *pPrivacyExemptionSent = false;

    // If OS has already set this, then dont care
    status = pPropertyCache->GetPropertyList(
        WfcPortPropertyNamePrivacyExemptionList,
        &bufferSize,
        &privacyListCount,
        (BYTE **)&pPrivacyExemptTypes);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't recall privacy exemption list.");
        goto exit;
    }

    if (privacyListCount > 0)
    {
        WFCActTrace("Privacy exemption list already configured with %d entries", privacyListCount);
        goto exit;
    }

    // No privacy exemption list configured. Determine if its needed
    status = pPropertyCache->GetPropertyList(
        WfcPortPropertyAuthAlgoList,
        &bufferSize,
        &authAlgoCount,
        (BYTE **)&pAuthAlgos);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't recall auth algorithm list.");
        goto exit;
    }

    for (UINT16 i = 0; i < authAlgoCount; ++i)
    {
        if ((pAuthAlgos[i] == DOT11_AUTH_ALGO_RSNA) ||
            (pAuthAlgos[i] == DOT11_AUTH_ALGO_RSNA_PSK) ||
            (pAuthAlgos[i] == DOT11_AUTH_ALGO_WPA) ||
            (pAuthAlgos[i] == DOT11_AUTH_ALGO_WPA_PSK))
        {
            needsEAP = true;
            break;
        }
    }

    if (!needsEAP)
    {
        WFCActTrace("Not a connection that needs EAP authentication");
        goto exit;
    }

    // This is a setup that needs EAP packets to be exempt and its not
    // configured for that. Send it now
    privacyExemptionListParameters.PrivacyExemptionEntry.ElementCount = 1;
    privacyExemptionListParameters.Optional.PrivacyExemptionEntry_IsPresent = true;
    privacyExemptionListParameters.PrivacyExemptionEntry.AllocateElements(
        privacyExemptionListParameters.PrivacyExemptionEntry.ElementCount,
        0);

    privacyExemptionListParameters.PrivacyExemptionEntry.pElements[0].EtherType = 0x8E88;
    privacyExemptionListParameters.PrivacyExemptionEntry.pElements[0].ExemptionActionType = WDI_EXEMPT_ALWAYS;
    privacyExemptionListParameters.PrivacyExemptionEntry.pElements[0].ExemptionPacketType = WDI_EXEMPT_PACKET_TYPE_UNICAST;

    status = GenerateWdiSetPrivacyExemptionList(
        &privacyExemptionListParameters,
        sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &m_PrivacyCommandBufferLength,
        &m_pPrivacyCommandDataBuffer
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Failed to generate privacy exemption params tlv");
        goto exit;
    }

    // Send the command down
    status = m_PrivacyExemptCommand.Initialize(
        get_PortId(),
        WDI_SET_PRIVACY_EXEMPTION_LIST,
        m_PrivacyCommandBufferLength,
        m_pPrivacyCommandDataBuffer);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't initialize set privacy exemption command");
        goto exit;
    }

    status = QueueDeviceCommand(
        &m_PrivacyExemptCommand);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Queue device command for set privacy exemption list failed.");
        goto exit;
    }

    WFCActTrace("[Connect] Initiated wait for PMKID list update");

    // Tell caller that command has been sent
    *pPrivacyExemptionSent = true;
    m_ConnectState = WiFiConnectJobStateWaitingForPrivacyExemptionListSet;

exit:
    TraceExit(status);
    return status;
}

NDIS_STATUS CConnectJob::StartBssListUpdate()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WFC_PORT_TYPE scanCacheSource = WfcPortTypeExtSTA;

    WFCActTrace("[Connect] Querying for BSS List from device");

    if (m_IsP2PConnect)
    {
        scanCacheSource = WfcPortTypeWFDDevice;
    }
    
    ndisStatus = m_BSSListUpdateJob.Initialize(
        m_pAdapter,
        m_NdisPortNumber,
        m_pAdapter->GetBSSListManager(scanCacheSource)
        );

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Failed to initialize job to update BSS list. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    ndisStatus = StartChildJob(&m_BSSListUpdateJob);
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Failed to start child job to update BSS list. Error = 0x%08x", ndisStatus);
        goto exit;
    }
    m_ConnectState = WiFiConnectJobStateWaitingForBSSListUpdate;

exit:

    return ndisStatus;
}



NDIS_STATUS CConnectJob::StartScanJob(
    _In_ WFC_SCAN_JOB_PARAMETERS* pScanJobParameters
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_PORT_NUMBER NdisPortNumber = INVALID_NDIS_PORT_NUMBER;

    TraceActEntry();
    WFCActInfo("[Connect][Roam] Performing scan job %d", m_LastScanType);

    //
    // TODO: Switch on m_IsP2PConnect -> Discover if true, normal scan if false
    // TODO: Provide channel hint to discover from scan cache if possible
    //

    if (m_IsP2PConnect)
    {
        status = m_pAdapter->GetNdisPortNumberForPortType(WfcPortTypeWFDDevice, &NdisPortNumber);
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Couldn't find Device port.");
            goto exit;
        }

        status = m_DiscoverJob.InitializeDiscoverByGroupID(
            m_pAdapter,
            NdisPortNumber,
            m_GroupIdStorage,
            m_RequireSelectedRegistrar,
            m_RequireGroupFormationBit,
            true, /* AllowBeaconMatch */
            m_IsRoam, /* P2PRoamingReconnect */
            m_GOChannelHint.ChannelNumber == 0 ? NULL : &m_GOChannelHint,
            NULL);

        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Couldn't initialize internal discover request.");
            goto exit;
        }

        status = StartChildJob(&m_DiscoverJob);

        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Couldn't start child discover job.");
            goto exit;
        }
    }
    else
    {
        m_pScanJob = new CScanJob(get_ActivityId());
        if (m_pScanJob == NULL)
        {
            status = NDIS_STATUS_RESOURCES;
            WFCActErrorStatus("Unable to allocate scan job");
            goto exit;
        }
    
        // We need the bss list cache to also get updated
        pScanJobParameters->UpdateBSSListCache = true;

        status = m_pScanJob->InitializeFromScanParams(
            m_pAdapter,
            m_NdisPortNumber,
            pScanJobParameters);

        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Couldn't initialize scan job from scan parameters.");
            goto exit;
        }

        if (m_BssEntryListNotificationRegistrationToken == WFC_REGISTRATION_TOKEN_INVALID)
        {
            status = m_pAdapter->GetNotificationManager()->RegisterForNotifications(
                WiFiIndicationBssEntryList,
                WFC_COMMAND_TOKEN_INVALID,
                get_PortId(),
                (INotifyDeviceIndicationCallback*)this,
                &m_BssEntryListNotificationRegistrationToken);
        }
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Couldn't register for BSS Entry List notifications.");
            goto exit;
        }

        // If we are going to be doing a scan on resume, check if we have to do NLO matching
        if (TestRoamConfig(m_RoamConfigFlags, RC_RESUME_FROM_DX))
        {
            // Check if we have to do an NLO match
            CPort* pPort = m_pAdapter->GetPortFromPortId(m_PortId);

            PDOT11_OFFLOAD_NETWORK offloadNetworkList = NULL;
            ULONG bufferSize;
            UINT16 numOffloadNetworks = 0;

            status = pPort->GetPropertyCache()->GetPropertyList(WfcPortPropertyResumeNLOList,
                &bufferSize,
                &numOffloadNetworks,
                (BYTE **)&offloadNetworkList);
            if (status != NDIS_STATUS_SUCCESS)
            {
                // Not an error
                status = NDIS_STATUS_SUCCESS;
                goto exit;
            }

            // Check if we are supposed to doing a match
            if ((numOffloadNetworks > 0) && (offloadNetworkList != NULL))
            {
                // We have an NLO list, save it
                if (m_NloMatchingCriteria.SSIDList.pBuffer)
                {
                    // Second time through this path, free previous memory
                    delete[] m_NloMatchingCriteria.SSIDList.pBuffer;
                    m_NloMatchingCriteria.SSIDList.pBuffer = NULL;
                }
                
                m_NloMatchingCriteria.SSIDList.pBuffer = new DOT11_SSID[numOffloadNetworks];
                if (m_NloMatchingCriteria.SSIDList.pBuffer == NULL)
                {
                    WFCActError("[NLO] Failed to allocate memory for NLO SSID list maching (%d entries)", numOffloadNetworks);
                }
                else
                {
                    PDOT11_SSID pNloSsid = NULL;
                    PDOT11_SSID pConnectSsid = m_ConnectionProfile.SSIDList.pBuffer;    // Since SSID only
                    
                    m_NloMatchingCriteria.SSIDList.ElementCount = 0;
                    
                    // Save in a simple SSID match list
                    for (UINT16 i = 0; i < numOffloadNetworks; i++)
                    {
                        pNloSsid = &(offloadNetworkList[i].Ssid);
                        
                        if (pNloSsid->uSSIDLength  > 0)
                        {
                            // Do not add our current profile's SSID in the NLO match. If we found it we should
                            // connect & not do NLO. Doing NLO would actually delay the reconnect                            
                            if (!(
                                    (pNloSsid->uSSIDLength == pConnectSsid->uSSIDLength) &&
                                    (memcmp(pNloSsid->ucSSID, pConnectSsid->ucSSID, pConnectSsid->uSSIDLength) == 0)
                                  )
                                )
                            {
                                RtlCopyMemory(&m_NloMatchingCriteria.SSIDList.pBuffer[i], 
                                    &offloadNetworkList[i].Ssid,
                                    sizeof(DOT11_SSID));
                                m_NloMatchingCriteria.SSIDList.ElementCount++;
                            }
                        }
                    }
                    WFCActTrace("[NLO] Selected %d of %d entries for performing NLO match", m_NloMatchingCriteria.SSIDList.ElementCount,
                        numOffloadNetworks);
                }
            }
        }
        

        status = StartChildJob(
            m_pScanJob);

        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Couldn't start scan job.");
            goto exit;
        }
    }        

    m_ConnectState = WiFiConnectJobStateWaitingForScanJobCompletion;
exit:
    if (status != NDIS_STATUS_SUCCESS)
    {
        if (m_pScanJob != NULL)
        {
            delete m_pScanJob;
            m_pScanJob = NULL;
        }
        if (m_BssEntryListNotificationRegistrationToken != WFC_REGISTRATION_TOKEN_INVALID)
        {
            m_pAdapter->GetNotificationManager()->DeregisterNotificationHandler(
                m_BssEntryListNotificationRegistrationToken);
            m_BssEntryListNotificationRegistrationToken = WFC_REGISTRATION_TOKEN_INVALID;
        }
    }
    TraceActExit(status);

    return status;
}

NDIS_STATUS CConnectJob::CompleteScanJob(
    _In_ NDIS_STATUS OperationStatus
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    if (m_IsCancelled)
    {
        status = NDIS_STATUS_REQUEST_ABORTED;
        WFCActErrorStatus("Scan job got cancelled.");
        goto exit;
    }

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        if (m_bIsScanCancelIntended)
        {
            WFCActInfo("Scan Child Job cancel expected");
            status = NDIS_STATUS_SUCCESS;
        }
        else
        {
            status = OperationStatus;
            WFCActErrorStatus("Scan Child Job Failed.");
        }
        goto exit;
    }

exit:

    if (status == NDIS_STATUS_DOT11_MEDIA_IN_USE)
    {
        // Scan didnt happen
        if (TestRoamConfig(m_RoamConfigFlags, RC_BEST_EFFORT))
        {
            CPort* pPort = m_pAdapter->GetPortFromPortId(
                get_PortId());

            // Determine if there is a better AP
            CConnectedPeer* pPeer = pPort->GetConnectedPeerList()
                ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);
            if (pPeer)
            {
                PPEER_ROAM_HISTORY pCurrentAPHistory = pPeer->GetRoamHistory();
                WFCActTrace("[Roam][BetterAP] Undoing roam attempt info because scan type %d didnt execute", m_LastScanType);

                if (pCurrentAPHistory->RoamAttemptCount)
                {
                    pCurrentAPHistory->RoamAttemptCount--;
                    pCurrentAPHistory->RoamAttemptTime = 0;
                }
            }
        }
        else if (TestRoamConfig(m_RoamConfigFlags, RC_CONNECTED_ROAM))
        {
            // We will continue with the connect even if scan didnt finish
            WFCActTrace("[Roam][BetterAP] Masking error for IHV better AP roam");
            status = NDIS_STATUS_SUCCESS;
        }
    }

    delete m_pScanJob;
    m_pScanJob = NULL;
    m_bIsScanCancelIntended = false;

    if (m_BssEntryListNotificationRegistrationToken != WFC_REGISTRATION_TOKEN_INVALID)
    {
        m_pAdapter->GetNotificationManager()->DeregisterNotificationHandler(
            m_BssEntryListNotificationRegistrationToken);
        m_BssEntryListNotificationRegistrationToken = WFC_REGISTRATION_TOKEN_INVALID;
    }

    TraceActExit(status);

    return status;
}

void CConnectJob::CompleteSendResponseActionFrameJob(
    _In_ NDIS_STATUS OperationStatus
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[11v] Send Action frame response job Failed. Error: 0x%08x", OperationStatus);
    }

    if (m_pResponseJob)
    {
        delete m_pResponseJob;
        m_pResponseJob = NULL;
    }

    TraceActExit(status);
}

bool CConnectJob::CheckAndRequestPMKIDUpdate(
    _In_ ULONG               BSSCandidateCount,
    _In_opt_count_(BSSCandidateCount)CBSSEntry** pBSSCandidateList
    )
{
    bool bPMKIDPending = false;

    TraceActEntry();

    if (TestRoamConfig(m_RoamConfigFlags, RC_CONNECT))
    {
        // PMKID updates not required for first time connect
        goto exit;
    }

    if ((BSSCandidateCount == 0) || (m_IsP2PConnect))
    {
        // Not required for WFD
        goto exit;
    }

    // Check if this is a network that support PMKIDs 
    if (!GetPortPropertyCache()->GetPropertyBooleanOrDefault(WfcPortPropertyPMKIDSupported, FALSE))
    {
        goto exit;
    }

    // Ask the port to perform a PMKID request if its appropriate
    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());
        
    bPMKIDPending = pPort->PerformPMKIDRequest(
        BSSCandidateCount, 
        pBSSCandidateList
        );
    
exit:
    TraceActExit(NDIS_STATUS_SUCCESS);    

    return bPMKIDPending;
}

NDIS_STATUS CConnectJob::StartWaitForPMKIDList()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    // Queue the timer for the indication
    status = m_pEventQueue->RegisterTimeoutCallbackWithLevelAndReuse(
        get_ActivityId(),
        this,
        WFC_CONSTANT::PMKIDUpdateWaitTimeMs,
        NULL,
        WDF_EXECUTION_LEVEL::WdfExecutionLevelPassive,
        false,
        NULL,
        &m_PMKIDTimerToken);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to register for PMKID list timer callback (0x%x)", status);
        goto Exit;
    }

    // Start listening for the PMKID list indication
    status = m_pAdapter->GetNotificationManager()->RegisterForNotifications(
        WiFiIndicationPmkIdCandidateListUpdate,
        WFC_COMMAND_TOKEN_INVALID,
        get_PortId(),
        (INotifyDeviceIndicationCallback*)this,
        &m_PMKIDNotificationRegistrationToken);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to register for PMKID list update indication (0x%x)", status);
        goto Exit;
    }

Exit:

    if (status != NDIS_STATUS_SUCCESS)
    {
        StopWaitForPMKIDList();
    }
    return status;
}


void CConnectJob::OnPMKIDListUpdateNotificationArrived(
    _In_ ULONG  BufferLength,
    _In_reads_bytes_(BufferLength) BYTE* Buffer)
{
    UNREFERENCED_PARAMETER(BufferLength);
    UNREFERENCED_PARAMETER(Buffer);

    WFCActTrace("[Connect] PMKID List Update arrived. Continuing the connect");
    StopWaitForPMKIDList();
    OnOperationCompleted(NDIS_STATUS_SUCCESS, this);
}


void CConnectJob::OnTimerCallback(
    _In_opt_ WFC_TIMER_CONTEXT)
{
    WFCActTrace("[Connect] PMKID List Update time limit expired. Continuing the connect");

    StopWaitForPMKIDList();
    OnOperationCompleted(NDIS_STATUS_SUCCESS, this);
}

VOID CConnectJob::StopWaitForPMKIDList()
{
    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    CPortPropertyCache *pPropertyCache = pPort->GetPropertyCache();

    // Clear waiting for PMKID list updates
    pPropertyCache->SetPropertyBoolean(
        WfcPortPropertyPMKIDListUpdatePending,
        FALSE);

    if (m_PMKIDTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        (VOID)m_pEventQueue->DeregisterTimeoutCallback(
            m_PMKIDTimerToken);
        m_PMKIDTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    if (m_PMKIDNotificationRegistrationToken != WFC_REGISTRATION_TOKEN_INVALID)
    {
        m_pAdapter->GetNotificationManager()->DeregisterNotificationHandler(
            m_PMKIDNotificationRegistrationToken);
        m_PMKIDNotificationRegistrationToken = WFC_REGISTRATION_TOKEN_INVALID;
    }
}

NDIS_STATUS CConnectJob::FillConnectRoamTaskParameters(
    _In_ PWDI_CONNECT_PARAMETERS_CONTAINER pConnectParameters,
    _In_ ArrayOfElements<WDI_CONNECT_BSS_ENTRY_CONTAINER> *pPreferredBSSEntryList,
    _In_ bool RoamRequest
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());
    ULONG bufferSize;
    PUCHAR pTempBuffer;
    CPortPropertyCache *pPropertyCache = pPort->GetPropertyCache();
    PDOT11_ADAPTER_CAPABILITIES pAdapterCapabilities = NULL;

    TraceEntry();

    memset( pConnectParameters, 0, sizeof( *pConnectParameters ) );
    memset( pPreferredBSSEntryList, 0, sizeof( *pPreferredBSSEntryList ) );

    // ConnectionSettings
    pConnectParameters->ConnectionSettings.HiddenNetwork =
                m_ConnectionProfile.HiddenNetwork;
    pConnectParameters->ConnectionSettings.ExcludeUnencrypted =
                m_ConnectionProfile.ExcludeUnencrypted;
    pConnectParameters->ConnectionSettings.MFPEnabled =
                m_ConnectionProfile.MFPEnabled;
    pConnectParameters->ConnectionSettings.HostFIPSModeEnabled =
                m_ConnectionProfile.HostFIPSModeEnabled;

    ndisStatus = CConnectHelpers::GetAdapterCapabilitiesFromPropertyCache(m_pAdapter, &pAdapterCapabilities);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[11r] Failed to get adapter capability for BSS transition support. Error = 0x%08x", ndisStatus);
        goto exit;
    }
    pConnectParameters->ConnectionSettings.BSSTransitionSupported = (pAdapterCapabilities->BSSTransitionSupported == dot11_tri_state_true) ? true : false;

    if (TestRoamConfig(m_RoamConfigFlags, RC_BSS_TRANSITION_MANAGEMENT_REQUEST_CRITICAL))
    {
        pConnectParameters->ConnectionSettings.RoamTrigger = WDI_ROAM_TRIGGER_CRITICAL_BSS_TRANSITION_REQUEST;
    }

    // SSID        
    ndisStatus = pConnectParameters->SSIDList.AllocateElements(
        m_ConnectionProfile.SSIDList.ElementCount,
        0
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to allocate memory for SSID list. Error = 0x%08x", ndisStatus);
        goto exit;
    }
    // Copy each of the elements individually
    __analysis_assume(pConnectParameters->SSIDList.ElementCount < 
        m_ConnectionProfile.SSIDList.ElementCount);
    for (UINT32 i = 0; i < pConnectParameters->SSIDList.ElementCount; i++)
    {
        ndisStatus = pConnectParameters->SSIDList.pElements[i].SimpleSet(
            m_ConnectionProfile.SSIDList.pBuffer[i].ucSSID,
            m_ConnectionProfile.SSIDList.pBuffer[i].uSSIDLength,
            0
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("Failed to set SSIDs list. Error = 0x%08x", ndisStatus);
            goto exit;
        }
    }                

    // HESSIDList
    if (m_ConnectionProfile.HESSIDList.ElementCount > 0)
    {
        pConnectParameters->HESSIDInfo.HESSIDList.SimpleAssign(
                (WDI_MAC_ADDRESS*)m_ConnectionProfile.HESSIDList.pBuffer,
                m_ConnectionProfile.HESSIDList.ElementCount
                );
        pConnectParameters->HESSIDInfo.HSIndicationElement.SimpleAssign(
                m_ConnectionProfile.HotspotIndicationElement.pBuffer,
                m_ConnectionProfile.HotspotIndicationElement.ElementCount
                );
        pConnectParameters->HESSIDInfo.AccessNetworkType = m_ConnectionProfile.AccessNetworkType;

        pConnectParameters->Optional.HESSIDInfo_IsPresent = 1;           

        WFCActInfo ("FillConnectRoamTaskParameters: Set %d HESSIDs, ANT=%d, IELength=%d\n",
            pConnectParameters->HESSIDInfo.HESSIDList.ElementCount, pConnectParameters->HESSIDInfo.AccessNetworkType,
            pConnectParameters->HESSIDInfo.HSIndicationElement.ElementCount);
    }

    // AuthenticationAlgorithms
    ndisStatus = pConnectParameters->AuthenticationAlgorithms.AllocateElements(
        m_ConnectionProfile.AuthAlgoList.ElementCount,
        0
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to allocate memory for AuthenticationAlgorithms algorithms list. Error = 0x%08x", ndisStatus);
        goto exit;
    }
    __analysis_assume(pConnectParameters->AuthenticationAlgorithms.ElementCount < 
        m_ConnectionProfile.AuthAlgoList.ElementCount);
    for (UINT32 i = 0; i < pConnectParameters->AuthenticationAlgorithms.ElementCount; i++)
    {
        pConnectParameters->AuthenticationAlgorithms.pElements[i] = CDot11ToWabiConverter::MapAuthAlgorithm(
            m_ConnectionProfile.AuthAlgoList.pBuffer[i]
            );
    }

    // MulticastCipherAlgorithms
    ndisStatus = pConnectParameters->MulticastCipherAlgorithms.AllocateElements(
        m_ConnectionProfile.MulticastCipherAlgoList.ElementCount,
        0
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to allocate memory for MulticastCipherAlgorithms algorithms list. Error = 0x%08x", ndisStatus);
        goto exit;
    }
    __analysis_assume(pConnectParameters->MulticastCipherAlgorithms.ElementCount < 
        m_ConnectionProfile.MulticastCipherAlgoList.ElementCount);
    for (UINT32 i = 0; i < pConnectParameters->MulticastCipherAlgorithms.ElementCount; i++)
    {
        pConnectParameters->MulticastCipherAlgorithms.pElements[i] = CDot11ToWabiConverter::MapCipherAlgorithm(
            m_ConnectionProfile.MulticastCipherAlgoList.pBuffer[i]
            );
    }

    // UnicastCipherAlgorithms
    ndisStatus = pConnectParameters->UnicastCipherAlgorithms.AllocateElements(
        m_ConnectionProfile.UnicastCipherAlgoList.ElementCount,
        0
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to allocate memory for UnicastCipherAlgorithms algorithms list. Error = 0x%08x", ndisStatus);
        goto exit;
    }
    __analysis_assume(pConnectParameters->UnicastCipherAlgorithms.ElementCount < 
        m_ConnectionProfile.UnicastCipherAlgoList.ElementCount);
    for (UINT32 i = 0; i < pConnectParameters->UnicastCipherAlgorithms.ElementCount; i++)
    {
        pConnectParameters->UnicastCipherAlgorithms.pElements[i] = CDot11ToWabiConverter::MapCipherAlgorithm(
            m_ConnectionProfile.UnicastCipherAlgoList.pBuffer[i]
            );
    }

    // ExtraAssociationRequestIEs
    ndisStatus = pPropertyCache->GetPropertyBuffer(WfcPortPropertyExtraAssociationRequestIEs,
        &bufferSize,
        (BYTE **)&pTempBuffer);
    if ((ndisStatus == NDIS_STATUS_SUCCESS) && bufferSize > 0)
    {
        pConnectParameters->AssociationRequestVendorIE.SimpleAssign(
                pTempBuffer,
                bufferSize
                );
        pConnectParameters->Optional.AssociationRequestVendorIE_IsPresent = 1;           
    }


    // AllowedBSSIDs
    if (m_ConnectionProfile.BSSIDList.ElementCount > 0)
    {
        pConnectParameters->AllowedBSSIDs.SimpleAssign(
                (WDI_MAC_ADDRESS*)m_ConnectionProfile.BSSIDList.pBuffer,
                m_ConnectionProfile.BSSIDList.ElementCount
                );
        pConnectParameters->Optional.AllowedBSSIDs_IsPresent = 1;           
        
    }

    // DisallowedBSSIDs
    if (m_ConnectionProfile.ExcludedMacAddressList.ElementCount > 0)
    {
        pConnectParameters->DisallowedBSSIDs.SimpleAssign(
                (WDI_MAC_ADDRESS*)m_ConnectionProfile.ExcludedMacAddressList.pBuffer,
                m_ConnectionProfile.ExcludedMacAddressList.ElementCount
                );
        pConnectParameters->Optional.DisallowedBSSIDs_IsPresent = 1;                   
    }

    // ActivePhyTypeList
    pConnectParameters->Optional.ActivePhyTypeList_IsPresent = 0;

    bool isFTAllowed = CConnectHelpers::IsFastTransitionAllowed(pPort, m_pAdapter, &m_ConnectionProfile);

    if (RoamRequest)
    {
        ULONG uRoamWabiReason = m_RoamWabiReason;

        if (GetPortPropertyCache()->GetPropertyBooleanOrDefault(
            WfcPortPropertyIhvRoamRequested,
            FALSE
            ))
        {
            ndisStatus = GetPortPropertyCache()->GetPropertyULong(
                WfcPortPropertyIhvRoamReason,
                &uRoamWabiReason);
            if (ndisStatus != NDIS_STATUS_SUCCESS ||
                uRoamWabiReason == 0xFFFF)
            {
                WFCActError("[Roam][IHV][BetterAP] Failed to get roam reason property cache.Error = 0x%08x", ndisStatus);
                uRoamWabiReason = m_RoamWabiReason;
                // Continue
            }
        }
        pConnectParameters->ConnectionSettings.RoamNeededReason = uRoamWabiReason;
    }

    // Preferred BSS entry list
    ndisStatus = CConnectHelpers::FillConnectBSSEntryTLV(
        pPort,
        isFTAllowed,
        m_TempBSSCandidateCount,
        m_TempBSSCandidateList,
        pPreferredBSSEntryList,
        RoamRequest
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to fill Connect BSS Entry TLV. Error = 0x%08x", ndisStatus);
        goto exit;
    }
exit:

    TraceExit(ndisStatus);

    return ndisStatus;
}

_Success_(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CConnectJob::GenerateRoamTaskTlv(
    _Out_ ULONG* pBufferLength,
    _Outptr_result_buffer_(*pBufferLength) UINT8** ppBuffer 
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_TASK_ROAM_PARAMETERS taskParameters;    
    ULONG roamBufferLength;
    PUINT8 pRoamBuffer = NULL;

    *pBufferLength = 0;

    status = FillConnectRoamTaskParameters(
            &taskParameters.ConnectParameters,
            &taskParameters.PreferredBSSEntryList,
            TRUE
            );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Failed to fill TLV generator parameters for ROAM task.");
        goto exit;
    }
    taskParameters.ConnectParameters.ConnectionSettings.RoamRequest = TRUE;

    status = GenerateWdiTaskRoam(&taskParameters,
        (ULONG)sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &roamBufferLength,
        &pRoamBuffer);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Failed to generate TLV buffer for ROAM task.");
        goto exit;
    }

    // Prune this buffer to be under command size
    status = CMessageHelper::FitMessageToBufferSize(
        (PWFC_MESSAGE_METADATA)pRoamBuffer,
        roamBufferLength,
        get_AdapterPropertyCache()->GetPropertyULongOrDefault(WfcAdapterPropertyMaxCommandSize, MAXULONG),
        WDI_TLV_CONNECT_BSS_ENTRY,
        pBufferLength
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Failed to fit TLV buffer for ROAM task from %d to %d bytes.", roamBufferLength, 
            get_AdapterPropertyCache()->GetPropertyULongOrDefault(WfcAdapterPropertyMaxCommandSize, MAXULONG));
        goto exit;
    }

    // Transfer buffer ownership to caller. Length is also set above
    *ppBuffer = pRoamBuffer;
    pRoamBuffer = NULL;
        
exit:

    if (pRoamBuffer)
    {
        delete[] pRoamBuffer;
    }

    return status;  
}

_Success_(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CConnectJob::GenerateConnectTaskTlv(
    _In_ bool RoamRequest,
    _Out_ ULONG* pBufferLength,
    _Outptr_result_buffer_(*pBufferLength) UINT8** ppBuffer 
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_TASK_CONNECT_PARAMETERS taskParameters; 
    ULONG connectBufferLength;
    PUINT8 pConnectBuffer = NULL;

    *pBufferLength = 0;

    status = FillConnectRoamTaskParameters(
            &taskParameters.ConnectParameters,
            &taskParameters.PreferredBSSEntryList,
            RoamRequest
            );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Failed to fill TLV generator parameters for CONNECT task.");
        goto exit;
    }

    if (RoamRequest)
    {
        taskParameters.ConnectParameters.ConnectionSettings.RoamRequest = TRUE;
    }
    else
    {
        taskParameters.ConnectParameters.ConnectionSettings.RoamRequest = FALSE;
    }

    status = GenerateWdiTaskConnect(&taskParameters,
        (ULONG)sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &connectBufferLength,
        &pConnectBuffer);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Failed to generate TLV buffer for CONNECT task.");
        goto exit;
    }

    // Prune this buffer to be under command size
    status = CMessageHelper::FitMessageToBufferSize(
        (PWFC_MESSAGE_METADATA)pConnectBuffer,
        connectBufferLength,
        get_AdapterPropertyCache()->GetPropertyULongOrDefault(WfcAdapterPropertyMaxCommandSize, MAXULONG),
        WDI_TLV_CONNECT_BSS_ENTRY,
        pBufferLength
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Failed to fit TLV buffer for CONNECT task from %d to %d bytes.", connectBufferLength, 
            get_AdapterPropertyCache()->GetPropertyULongOrDefault(WfcAdapterPropertyMaxCommandSize, MAXULONG));
        goto exit;
    }

    // Transfer buffer ownership to caller. Length is also set above
    *ppBuffer = pConnectBuffer;
    pConnectBuffer = NULL;

exit:
    if (pConnectBuffer)
    {
        delete[] pConnectBuffer;
    }

    return status;  
}


NDIS_STATUS CConnectJob::StartConnectRoamTask(
    _In_  _START_CONNECT_ROAM_CANDIDATE_LIST_CREATION_OPTIONS CandidateGenerationOptions,
    _In_  _START_CONNECT_ROAM_NO_CANDIDATE_CONTINUE_OPTIONS NoCandidateOptions,
    _Out_ WDI_ASSOC_STATUS* pConnectStartStatus)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    DeviceCommand* pTaskDeviceCommand;
    bool bIsBetterAPRoam = false;
    bool Workaround_bDisconnectForRoam = false; // WDI_DISABLED
    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    TraceActEntry();
    
    if (pPort->m_pRoamTraceLoggingData != NULL)
    {
        pPort->m_pRoamTraceLoggingData->connectRoamTaskStartTime = CSystem::get_CurrentTime();
    }

    *pConnectStartStatus = WDI_ASSOC_STATUS_CANDIDATE_LIST_EXHAUSTED;
    if (m_IsCancelled)
    {
        status = NDIS_STATUS_FAILURE;
        WFCActErrorStatus("Connect job got cancelled.");
        goto exit;
    }

    // If radio is OFF, dont do anything
    if (m_pAdapter->GetAdapterPropertyCache()->GetPropertyBooleanOrDefault(WfcAdapterHardwareRadioOff, FALSE) ||
        m_pAdapter->GetAdapterPropertyCache()->GetPropertyBooleanOrDefault(WfcAdapterSoftwareRadioOff, FALSE))
    {
        status = NDIS_STATUS_DOT11_POWER_STATE_INVALID;
        WFCActErrorStatus("[Connect][Roam] Failed connect/roam because radio is OFF.");
        goto exit;
    }    

    if (m_AssocNotificationRegistrationToken == WFC_REGISTRATION_TOKEN_INVALID)
    {
        status = m_pAdapter->GetNotificationManager()->RegisterForNotifications(
            WiFiIndicationAssociationResult,
            WFC_COMMAND_TOKEN_INVALID,
            get_PortId(),
            (INotifyDeviceIndicationCallback*)this,
            &m_AssocNotificationRegistrationToken);
    }

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't register for association result notifications.");
        goto exit;
    }

    if (m_pConnectParametersTlvBuffer)
    {
        delete []m_pConnectParametersTlvBuffer;
        m_pConnectParametersTlvBuffer = NULL;
        m_ConnectParametersTlvLength = 0;
    }

    if (CandidateGenerationOptions == START_CONNECT_ROAM_RECREATE_CANDIDATE_LIST)
    {
        // Get list again since what we got earlier may be old (eg.
        // due to PMKID query wait)
        status = PickCandidates(&m_ConnectionProfile);
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Couldn't pick candidates to connect task.");
            goto exit;
        }
    }
    
    if (m_TempBSSCandidateCount > 0)
    {
        *pConnectStartStatus = WDI_ASSOC_STATUS_SUCCESS;
    }
    else
    {
        if (NoCandidateOptions == START_CONNECT_ROAM_CONTINUE_IF_NO_CANDIDATE_FOUND)
        {
            // We didnt find entries, but were asked to proceed            
            NT_ASSERT(TestRoamConfig(m_RoamConfigFlags, RC_CONNECTED_ROAM));
            *pConnectStartStatus = WDI_ASSOC_STATUS_SUCCESS;
        }
        else
        {
            *pConnectStartStatus = WDI_ASSOC_STATUS_CANDIDATE_LIST_EXHAUSTED;
            goto exit;
        }
    }

    if (TestRoamConfig(m_RoamConfigFlags, RC_CONNECTED_ROAM))
    {
        // Roam job
        bIsBetterAPRoam = true;

        CConnectedPeer* pPeer = m_pAdapter->GetConnectedPeerList(get_PortId())
            ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);
        if (pPeer == NULL)
        {
            WFCActInfo("Not current connected. Continuing with connect task");
            // Not a roam
            bIsBetterAPRoam = false;
        }
    }

    // Use the appropriate connect or roam job
    if (bIsBetterAPRoam)
    {                    
        status = GenerateRoamTaskTlv(
            &m_ConnectParametersTlvLength,
            &m_pConnectParametersTlvBuffer
            );
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Failed to generate TLV for roam task.");
            goto exit;
        }
    }
    else
    {
        status = GenerateConnectTaskTlv(
            !TestRoamConfig(m_RoamConfigFlags, RC_CONNECT),
            &m_ConnectParametersTlvLength,
            &m_pConnectParametersTlvBuffer
            );
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Failed to generate TLV for connect task.");
            goto exit;
        }
    }
    WFCActInfo("[Connect][Roam] Will be starting Connect/Roam Task");
    
    
    status = m_ConnectTask.Initialize(
        m_pAdapter->GetNotificationManager(),
        m_pAdapter->GetDeviceCommandScheduler(),
        m_pAdapter->GetEventQueue());

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Failed to initialize connect task.");
        goto exit;
    }

    status = m_ConnectTask.get_TaskDeviceCommand(
        &pTaskDeviceCommand);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't get device command for connect task.");
        goto exit;
    }
    
    status = pTaskDeviceCommand->Initialize(
        get_PortId(),
        (Workaround_bDisconnectForRoam ? WDI_TASK_DISCONNECT : (bIsBetterAPRoam ? WDI_TASK_ROAM : WDI_TASK_CONNECT)),
        m_ConnectParametersTlvLength,
        m_pConnectParametersTlvBuffer);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't attach generated tlvs to device command.");
        goto exit;
    }

    if (pPort->m_pRoamTraceLoggingData != NULL)
    {
        pPort->m_pRoamTraceLoggingData->roamOccured = true;
    }

    // Clear the pending IHV roam request flag since we have sent a roam or connect request down
    status = GetPortPropertyCache()->SetPropertyBoolean(
        WfcPortPropertyIhvRoamRequested, 
        FALSE
        );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't clear flag for IHV Roam requested. Error = 0x%08x", status);
        // Continue on
    }

    //Reset the IHV roam reason
    status = GetPortPropertyCache()->SetPropertyULong(
        WfcPortPropertyIhvRoamReason,
        0xFFFF);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Roam][IHV][BetterAP] Failed to set roam reason property cache. Error = 0x%08x.", status);
        // Continue
    }

    status = StartTask(
        &m_ConnectTask);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't start connect task.");
        goto exit;
    }
    WFCActInfo("[Connect][Roam] Started Connect/Roam Task");

    m_ConnectState = WiFiConnectJobStateWaitingForConnectCompletion;

exit:
    
    TraceActExit(status);

    return status;
}

_Ret_range_(WDI_PHY_TYPE_ANY, WDI_PHY_TYPE_DMG)
WDI_PHY_TYPE GetPhyTypeWithoutIHV( WDI_PHY_TYPE src )
{
    return (src > WDI_PHY_TYPE_DMG) ? WDI_PHY_TYPE_ANY : src;
}

int __cdecl PhyListCompare(const void* a, const void* b)
{
    static const ULONG PhyPriorities[] =
    {
        0,  //WDI_PHY_TYPE_any    (IHV + any)
        1,  //WDI_PHY_TYPE_fhss 
        2,  //WDI_PHY_TYPE_dsss
        3,  //WDI_PHY_TYPE_irbaseband
        6,  //WDI_PHY_TYPE_ofdm   (a)
        4,  //WDI_PHY_TYPE_hrdsss (b)
        5,  //WDI_PHY_TYPE_erp    (g)
        7,  //WDI_PHY_TYPE_ht     (n)
        8,  //WDI_PHY_TYPE_vht    (ac)
        9   //WDI_PHY_TYPE_dmg    (ad)
    };

    ULONG phyPriorityOfa = PhyPriorities[GetPhyTypeWithoutIHV(*(WDI_PHY_TYPE*)a)];
    ULONG phyPriorityOfb = PhyPriorities[GetPhyTypeWithoutIHV(*(WDI_PHY_TYPE*)b)];

    return phyPriorityOfb - phyPriorityOfa;
}

NDIS_STATUS CConnectJob::ReorderAssociationPhyTypesList(
    _In_     WDI_ASSOCIATION_RESULT_CONTAINER* pAssociationResult
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WDI_PHY_TYPE* pSourceEntries = pAssociationResult->ActivePhyTypeList.pElements;

    // higher priority will have lowest index in phy list (currently ac > n > a > g > b > other)
    // IHV will map to dot11_phy_type_any for priority purposes

    TraceEntry();

    qsort(pSourceEntries, pAssociationResult->ActivePhyTypeList.ElementCount, sizeof(WDI_PHY_TYPE), PhyListCompare);

    TraceExit(ndisStatus);
    return ndisStatus;
}

void CConnectJob::OnAssociationNotificationArrived(
    _In_ ULONG  EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_INDICATION_ASSOCIATION_RESULT_LIST assocResultList;
    WFC_PORT_TYPE scanCacheSource = WfcPortTypeExtSTA;
    WDI_ASSOCIATION_RESULT_CONTAINER* pAssociationResult;
    CPort* pPort = m_pAdapter->GetPortFromPortId(m_PortId);
    ULONG completionStatus = DOT11_ASSOC_STATUS_SUCCESS;
    bool bNotifyRxMgrIfNeeded = false;
    WDI_PEER_ID PeerId = WDI_PEER_ANY;

    TraceEntry();

    if (m_IsP2PConnect)
    {
        scanCacheSource = WfcPortTypeWFDDevice;
    }

    // Overall status is not interesting. We will be looking at each associations's status

    status = ParseWdiIndicationAssociationResult(
        EventBufferLength - sizeof(WFC_MESSAGE_METADATA),
        EventBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &assocResultList );

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't parse TLVs for association result.");
        goto exit;
    }

    for (ULONG i = 0; i < assocResultList.AssociationResults.ElementCount; ++i)
    {
        // We shouldn't have reported already reported a successful association
        ASSERT(m_NotifiedSuccessfulAssociation == false);

        pAssociationResult = &assocResultList.AssociationResults.pElements[i];

        WFCActTrace("[Connect] Received association notification from %!MACADDR!. Status %d", 
            pAssociationResult->BSSID.Address,
            pAssociationResult->AssociationResultParameters.AssociationStatus);

        
        CBSSEntry* pBssEntry = m_pAdapter->GetBSSListManager(
            scanCacheSource)->FindBSSEntry(
            pAssociationResult->BSSID.Address);

        if (pPort->m_pRoamTraceLoggingData != NULL)
        {
            // Determine if this was a Fast Roam
            pPort->m_pRoamTraceLoggingData->lastScanType = (UCHAR)m_LastScanType;
            pPort->m_pRoamTraceLoggingData->associationStatusCode = pAssociationResult->AssociationResultParameters.AssociationStatus;
            pPort->m_pRoamTraceLoggingData->bssCandidateCount = m_TempBSSCandidateCount;
            pPort->m_pRoamTraceLoggingData->roamWabiReason = m_RoamWabiReason;
            pPort->m_pRoamTraceLoggingData->connectTrigger = m_ConnectTrigger;
        }

        if (pBssEntry != NULL)
        {
            if (pPort->m_pRoamTraceLoggingData != NULL)
            {
                for (ULONG i = 0; i < m_TempBSSCandidateCount; i++)
                {
                    if (m_TempBSSCandidateList[i] == pBssEntry)
                    {
                        pPort->m_pRoamTraceLoggingData->roamAPRank = i;
                        break;
                    }
                }
            }
            pBssEntry->GetSSID(m_SSID);

            if (pAssociationResult->AssociationResultParameters.AssociationStatus != WDI_ASSOC_STATUS_SUCCESS)
            {
                // Not logging as ERROR since this maybe expected sometimes
                WFCActInfo("[Connect] Received association notification from %!MACADDR!. WDI Status = %d; 802.11 Status = %d; IHV Status = %d", 
                    pAssociationResult->BSSID.Address,
                    pAssociationResult->AssociationResultParameters.AssociationStatus,
                    pAssociationResult->AssociationResultParameters.StatusCode,
                    pAssociationResult->AssociationResultParameters.IHVAssociationStatus
                    );

                BOOLEAN FTConnection;
                GetPortPropertyCache()->GetPropertyBoolean((WFC_PROPERTY_NAME)WfcPortPropertyCurrentConnectionFT, &FTConnection);

                if (CConnectHelpers::IsFailedByPeerAction(pAssociationResult->AssociationResultParameters.AssociationStatus))
                {
                    WFCActTrace("[BlockedList] Blocking BSSID = %!MACADDR! due to rejected association ", 
                        pAssociationResult->BSSID.Address);
                    pBssEntry->Block(WFC_BLOCKED_REASON_ASSOC_REJECTED_OR_PEER_DISCONNECTED);

                    // [11r] Blocked if, this is a roam connect failure and Initial mobility domain association had been done (This failure is in FT)
                    // Or if this is a first time connect and we had tried 11r Initial mobility domain association
                    if ((((TestRoamConfig(m_RoamConfigFlags, RC_RECONNECT) && m_IsRoam) ||
                          (TestRoamConfig(m_RoamConfigFlags, RC_CONNECTED_ROAM)       )    ) &&
                         (FTConnection                                                     )    ) ||
                        (((TestRoamConfig(m_RoamConfigFlags, RC_CONNECT)               ) ||
                          (TestRoamConfig(m_RoamConfigFlags, RC_RECONNECT) && !m_IsRoam)    ) &&
                         (pBssEntry->IsFastBSSTransitionSupported()                         )    )   )
                    {
                        // Do not block the SSID for 11r if the AP advertises only FT AKM Suite. We will have to keep trying for 11r connect/roam in that case
                        if (!pBssEntry->IsOnlyFTAKMSuiteAdvertised())
                        {
                            m_IsBlockSSIDFor11rNeeded = true;
                        }
                        m_IsReconnectNeededDueTo11r = true;
                    }
                }
                else if (CConnectHelpers::IsPeerNotReachable(pAssociationResult->AssociationResultParameters.AssociationStatus))
                {
                    WFCActTrace("[BlockedList] Blocking BSSID = %!MACADDR! due to BSS unreachable", 
                        pAssociationResult->BSSID.Address);
                    pBssEntry->Block(WFC_BLOCKED_REASON_ASSOC_OR_DISASSOC_BSS_UNREACHABLE);
                }

                (VOID)NotifyAssociationStartAndCompletionIfNecessary(
                    pBssEntry,
                    pAssociationResult
                    );

                if (pPort->m_pRoamTraceLoggingData != NULL)
                {
                    pPort->m_pRoamTraceLoggingData->assocFailureCount += 1;
                }
            }
            else
            {
                IConnectedPeerList* pPeerList = m_pAdapter->GetConnectedPeerList(get_PortId());
                CPort* pPort = m_pAdapter->GetPortFromPortId(
                    get_PortId() );

                // Add to the peer table before calling associate
                pPeerList->AddPeer(
                    pAssociationResult->BSSID.Address,
                    dot11_assoc_state_auth_assoc,
                    pBssEntry->GetSignalInfo()->LinkQuality
                    );

                WDI_TXRX_PEER_CFG PeerConfig;
                BOOLEAN FTConnection;

                bNotifyRxMgrIfNeeded = true;
                PeerId = m_pAdapter->m_PeerTable.GetPeerId((PWDI_MAC_ADDRESS)&pAssociationResult->BSSID.Address,get_PortId());
                PeerConfig.PeerQoSConfig = (pAssociationResult->AssociationResultParameters.WMMQoSEnabled) ? WDI_TXRX_PeerCfgQosCapable : WDI_TXRX_PeerCfgQosNone;
                if (PeerId != WDI_PEER_ANY)
                {
                    WFCInfo("Calling TalTxRxPeerConfig for %!MACADDR!, QoS Protocol = %x",
                        pAssociationResult->BSSID.Address,
                        PeerConfig.PeerQoSConfig);
                    m_pAdapter->m_MiniportDataHandlers.TalTxRxPeerConfigHandler(m_pAdapter->GetMiniportTalTxRxContext(), m_PortId, PeerId, &PeerConfig);
                }
                else
                {
                    WFCError("Did not find %!MACADDR! in PeerTable",
                        pAssociationResult->BSSID.Address);
                }

                if (pAssociationResult->Optional.ActivePhyTypeList_IsPresent)
                {
                    // Reorder phy type list
                    ReorderAssociationPhyTypesList(pAssociationResult);

                    if (pAssociationResult->ActivePhyTypeList.ElementCount > 0)
                    {
                        ULONG phyIndex;

                        //replace the phy type (from the IHV) with the phy Id (for nwifi) inplace.
                        //since the phy types are not needed and the phy type and phy index have the same 
                        // size it is efficient to do the replacement inplace.
                        for (ULONG i = 0; i < pAssociationResult->ActivePhyTypeList.ElementCount; i++)
                        {
                            if (get_AdapterPropertyCache()->GetDot11PhyIDFromPhyType(pAssociationResult->ActivePhyTypeList.pElements[i], &phyIndex) != NDIS_STATUS_SUCCESS)
                            {
                                //the IHV miniport shouldn't indicate phys in an association 
                                //  completion that were not indicated during initialization.
                                WFCActError("Couldn't match the phy type (%x) to a phy index.", pAssociationResult->ActivePhyTypeList.pElements[i]);
                                NT_ASSERT(false);
                                //safest value to use is zero.
                                pAssociationResult->ActivePhyTypeList.pElements[i] = (WDI_PHY_TYPE)0;
                            }
                            else
                            {
                                pAssociationResult->ActivePhyTypeList.pElements[i] = (WDI_PHY_TYPE)phyIndex;
                            }
                        }
                        //save the most desired phy index here. Failure is ignored.
                        if (GetPortPropertyCache()->SetPropertyULong(
                            WfcPortPropertyMostDesiredPhyIndex,
                            pAssociationResult->ActivePhyTypeList.pElements[0]
                            ) != NDIS_STATUS_SUCCESS)
                        {
                            WFCActInfo("Couldn't save most desired Phy Index in property cache");
                        }
                    }
                }

                /*
                1: If this is a roam, then unblocked the SSID, as we have successfully connected to it.
                2: If this is a first time connect, then unblocked the SSID, if the association result params has MDID blob in it
                */
                if (pBssEntry->IsFastBSSTransitionSupported())
                {
                    DOT11_SSID ssid;
                    pBssEntry->GetSSID(ssid);

                    UCHAR MDIELength = 0;
                    BYTE* MDIE = NULL;
                    NDIS_STATUS ndisStatus = Dot11GetInfoElementFromIEBlob(
                        pAssociationResult->AssociationResponseFrame.pElements + sizeof(DOT11_ASSOC_RESPONSE_FRAME),
                        pAssociationResult->AssociationResponseFrame.ElementCount - sizeof(DOT11_ASSOC_RESPONSE_FRAME),
                        DOT11_INFO_ELEMENT_ID_MOBILITY_DOMAIN,
                        &MDIELength,
                        (PVOID*)&MDIE);

                    if ((TestRoamConfig(m_RoamConfigFlags, RC_RECONNECT) && m_IsRoam) ||
                        (TestRoamConfig(m_RoamConfigFlags, RC_CONNECTED_ROAM)       )    )
                    {
                        GetPortPropertyCache()->GetPropertyBoolean((WFC_PROPERTY_NAME)WfcPortPropertyCurrentConnectionFT, &FTConnection);
                        if (FTConnection)
                        {
                            if (ndisStatus != NDIS_STATUS_SUCCESS || MDIE == NULL)
                            {
                                WFCError("[11r] Error: The OS intended to do a FT roam, but the driver did a non-11r roam. Disconnecting");
                                pPort->TriggerDisconnectAndReconnect(
                                    WDI_ASSOC_STATUS_DISASSOCIATED_BY_HOST,
                                    DOT11_MGMT_REASON_AUTH_NOT_VALID);
                                goto exit;
                            }
                            WFCInfo("[11r]This is a successful association completion indication for a 11r roam");

                            m_IsBlockSSIDFor11rNeeded = false;
                            m_IsReconnectNeededDueTo11r = false;
                            pPort->UnBlockSSID(
                                ssid,
                                WFC_BLOCKED_REASON_ASSOC_REJECTED_OR_PEER_DISCONNECTED
                                );
                        }
                        else
                        {
                            WFCInfo("[11r]The AP is 11r supported, but OS did not do fast transition.");
                        }
                    }
                    else
                    {
                        // Initial Mobility Domain Association
                        if (ndisStatus == NDIS_STATUS_SUCCESS && MDIELength != 0 && MDIE != NULL)
                        {
                            m_IsBlockSSIDFor11rNeeded = false;
                            m_IsReconnectNeededDueTo11r = false;
                            WFCInfo("[11r] This is a successful association completion for Initial Mobility Domain Association");
                            pPort->UnBlockSSID(
                                ssid,
                                WFC_BLOCKED_REASON_ASSOC_REJECTED_OR_PEER_DISCONNECTED
                                );
                        }
                        else if (ndisStatus != NDIS_STATUS_SUCCESS || MDIELength == 0 || MDIE == NULL)
                        {
                            WFCInfo("[11r] This is a successful association completion for non 11r connect");
                            GetPortPropertyCache()->SetPropertyBoolean((WFC_PROPERTY_NAME)WfcPortPropertyCurrentConnectionFT, FALSE);
                            pPort->ClearFTPropertyCache();
                        }
                    }
                }
                else //Reset FT port property
                {
                    GetPortPropertyCache()->SetPropertyBoolean((WFC_PROPERTY_NAME)WfcPortPropertyCurrentConnectionFT, FALSE);
                    pPort->ClearFTPropertyCache();
                }

                completionStatus = NotifyAssociationStartAndCompletionIfNecessary(
                    pBssEntry,
                    pAssociationResult
                    );

                if (completionStatus == DOT11_ASSOC_STATUS_SUCCESS)
                {
                    PWFC_BSS_ENTRY_CHANNEL_INFO pChannelInfo;

                    WFCTrace("[LinkChangeTime]Association Complete");
                
                    if (!m_IsP2PConnect)
                    {
                        // Mark the BSS entry as something we are associated to so
                        // that we can remember it longer
                        pBssEntry->SetAssociatedFlag(true);
                    }

                    // Unblocked this AP
                    pBssEntry->UnBlock((WFC_BLOCKED_REASON_ASSOC_OR_DISASSOC_BSS_UNREACHABLE  | WFC_BLOCKED_REASON_ASSOC_REJECTED_OR_PEER_DISCONNECTED));                    
    
                    pChannelInfo = pBssEntry->GetChannelInfo();

                    // Save the current channel info. Failure is ignored here
                    if (GetPortPropertyCache()->SetPropertyULong(
                        WfcPortPropertyCurrentChannelNumber,
                        pChannelInfo->Channel
                        ) != NDIS_STATUS_SUCCESS)
                    {
                        WFCActInfo("Couldn't save Channel Number in property cache");
                    }

                    if (GetPortPropertyCache()->SetPropertyULong(
                        WfcPortPropertyCurrentBandID,
                        pChannelInfo->BandId
                        ) != NDIS_STATUS_SUCCESS)
                    {
                        WFCActInfo("Couldn't save Band ID in property cache");
                    }

                    //++ InstConn previous AP
                    //persist channel and bandid if default port
                    //
                    if ( m_NdisPortNumber == NDIS_DEFAULT_PORT_NUMBER )
                    {
                        WFCInfo( "Cache Sta last connected bandid=%d channel=%d",
                                    pChannelInfo->BandId,
                                    pChannelInfo->Channel );
                        //DbgPrintEx(0,0,"Cache Sta last connected bandid=%d channel=%d\n",
                        //            pChannelInfo->BandId, 
                        //            pChannelInfo->Channel );

                        // Save the band and channel in the adapter property cache. 
                        // We will reuse it later
                        m_pAdapter->GetAdapterPropertyCache()->SetPropertyULong(
                            WfcAdapterPropertyStaLastConnectedChannel,
                            pChannelInfo->Channel
                            );
                            
                        m_pAdapter->GetAdapterPropertyCache()->SetPropertyULong(
                            WfcAdapterPropertyStaLastConnectedBandId,
                            pChannelInfo->BandId
                            );
                    }            

                    // Save channel history
                    pPort->SaveNetworkConnectHistory(m_ConnectionProfile.SSIDList.pBuffer, pBssEntry);
                    
#ifdef WDI_FUTURE_COMPLIANT_FASTROAM_CACHE
                    DOT11_SSID temp_ssid = { 0 };
                    pBssEntry->GetSSID(temp_ssid);
                    pPort->ProcessAssociationCompletion(&temp_ssid, pBssEntry->GetBssId(), pBssEntry->GetChannelInfo());
#endif

                    WFCActInfo("[Connect][Link] Successful Association BSSID %!MACADDR! ; Band %d Channel %d ; Link Quality %d Rank %d", 
                        *(pBssEntry->GetBssId()), pChannelInfo->BandId, pChannelInfo->Channel, pBssEntry->GetSignalInfo()->LinkQuality, 
                        pBssEntry->GetCachedRank());

                }

                if (pPort->m_pRoamTraceLoggingData != NULL)
                {
                    pPort->m_pRoamTraceLoggingData->linkQualityCurrent = (UCHAR)(pBssEntry->GetSignalInfo()->LinkQuality);
                    pPort->m_pRoamTraceLoggingData->isNeighborReportSupported = pBssEntry->IsNeighborReportSupported();
                    pPort->m_pRoamTraceLoggingData->isAPChannelReportSupported = pBssEntry->IsAPChannelReportSupported();
                    pPort->m_pRoamTraceLoggingData->channel = pBssEntry->GetChannelInfo()->Channel;
                    pPort->m_pRoamTraceLoggingData->bandID = pBssEntry->GetChannelInfo()->BandId;
                }

                if (pAssociationResult->AssociationResultParameters.AuthAlgorithm == WDI_AUTH_ALGO_80211_OPEN)
                {
                    //
                    // Queue neighbor report request only if open network. On secure networks, we
                    // wait until keys have been exchanged before sending request.
                    //
                    pPort->QueueNeighborReportRequest();
                    // Queue opportunistic roam timer only if open network and this isn't P2P.
                    if (!m_IsP2PConnect)
                    {
                        pPort->QueueOpportunisticRoam();
                    }

                    // Association was successful and this is a open network. So we will do the trace logging here 
                    // as we will not be receiving cipher keys
                    if (m_IsRoam &&
                        pPort->m_pRoamTraceLoggingData != NULL &&
                        pPort->m_pRoamTraceLoggingData->roamOccured)
                    {
                        pPort->TraceLoggingRoamFinished(STATUS_SUCCESS);
                    }
                }

                WFCActTrace("[Connect] Association successful. Blocking background scan for 30 seconds");
                m_pAdapter->SetNextAllowedScanTime(30000);

                m_ConnectCompletionStatus = pAssociationResult->AssociationResultParameters.AssociationStatus;
                m_NotifiedSuccessfulAssociation = true;

                goto exit;
            }
        }
        else
        {
            WFCActError("No info about BSSID in scan cache to notify association start.");
        }
    }

exit:

    if (assocResultList.AssociationResults.pElements)
    {
        delete []assocResultList.AssociationResults.pElements;
        assocResultList.AssociationResults.MemoryInternallyAllocated = false;
    }

    //if there was a successful association and the port pauses Rx until assoc complete 
    // then notify the RxMgr (even if the association was not indicated to nwifi due to 
    // non-11r when 11r expected)
    // Not notifying the RxMgr results in the rx path being blocked until:
    //  - OnAssociationComplete
    //  - peer delete
    //  - pause/dot11rest for the port with that peer
    //There should never be a path which the IHV indicates a successful association and 
    // WDI does NOT call OnAssociationComplete if PortMustPauseRxUntilAssociateComplete 
    // is true as it can cause a a dot11reset or pause job to hang
    if (bNotifyRxMgrIfNeeded && pPort->PortMustPauseRxUntilAssociateComplete())
    {
        m_pAdapter->m_RxMgr.OnAssociationComplete(PeerId);
    }

    TraceActExit(status);
}

NDIS_STATUS CConnectJob::CompleteConnectTask(
    _In_ NDIS_STATUS OperationStatus)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_STATUS commandStatus;

    TraceActEntry();

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        status = OperationStatus;
        WFCActErrorStatus("Connect Task async operation failed.");
        goto exit;
    }

    if (m_IsCancelled)
    {
        status = NDIS_STATUS_FAILURE;
        WFCActErrorStatus("Connect job got cancelled.");
        goto exit;
    }

    status = CMessageHelper::GetStatusFromTaskOutput(
        &m_ConnectTask,
        &commandStatus);

    if ((status != NDIS_STATUS_SUCCESS) && (commandStatus == NDIS_STATUS_SUCCESS))
    {
        WFCActErrorStatus("Couldn't get status from command result.");
        goto exit;
    }


exit:

    TraceActExit(status);

    return status;
}

void CConnectJob::OnBssEntryListNotification()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    PROAM_CONTROL_PARAMETERS pRoamControl = CConnectHelpers::GetRoamControlParameters(
        m_pAdapter->GetAdapterPropertyCache());

    BOOLEAN bIsScanCancelPermitted = pRoamControl->ScanCancelOnGoodAPFoundPermitted;

    TraceActEntry();

    WFCActTrace("[Roam]ScanCancelPermittedState is %x", bIsScanCancelPermitted);
    //Only check the candidates if there is a scan in progress,
    // we are roaming, scan cancels are permitted for this 
    // roam control type, and we haven't already decided to cancel the scan
    if (m_pScanJob != NULL                           &&
        m_IsRoam == TRUE                             &&
        m_IsP2PConnect != TRUE                       &&
        m_bIsScanCancelIntended != TRUE)
    {
        status = CheckAndUpdateCandidates();
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("[Roam]Couldn't update candidates.");
            goto exit;
        }

        if (m_TempBSSCandidateCount > 0)
        {
            //We don't cancel scans if they are neighbor scans
            // neighbor - already a reduced set of channels so should complete scan to have better picture
            // Also if we need to perform full passive scans, then unless media streaming
            // dont allow scans to be cancelled
            if (bIsScanCancelPermitted == TRUE                                &&
                m_LastScanType != WfcConnectScanTypeNeighborOrInstantConnect  &&
                !(m_PerformFullPassiveScan && !m_MediaStreamingMode)
                )
            {
                WFCActInfo("[Roam] %lu acceptable candidates found during scan type %d.  Canceling scan job",
                    m_TempBSSCandidateCount,
                    m_LastScanType);
                m_pScanJob->CancelJob();
                m_bIsScanCancelIntended = true;
            }
        }
        else // no candidates found
        {
            // Check if this roam is deferrable. If so, cancel this scan and let CheckAndStartScanProcess handle
            // the actual deferring of job

            if ((CJobBase::get_DeferHint() == true)                     &&    // Deferral has been requested         
               (TestRoamConfig(m_RoamConfigFlags, RC_DEFERRABLE_ROAM))  &&    // Roam supports deferral
               (m_LastScanType > WfcConnectScanTypeChannelHint))              // One of the later scans              
            {
                //Cancel the scan job and allow it to be deferred 
                WFCActInfo("[Roam] No candidates found during scan type %d.  Canceling scan job in order to complete deferrable roam early",
                    m_LastScanType);
                m_pScanJob->CancelJob();
                m_bIsScanCancelIntended = true;
            }
        }
    }

    // If we didnt find anything check if we should do an NLO check
    if (!m_bIsScanCancelIntended                               &&
        (TestRoamConfig(m_RoamConfigFlags, RC_RESUME_FROM_DX)) &&           // Resume scenario
        (m_LastScanType > WfcConnectScanTypeChannelHint)       &&           // One of the later scans
        (m_NloMatchingCriteria.SSIDList.ElementCount > 0)         )
    {
        bool isMatchFound = false;
        
        // Check the BSS list
        status = m_pAdapter->GetBSSListManager(WfcPortTypeExtSTA)->FindMatchingBSSEntriesForNlo(
            &m_NloMatchingCriteria,
            WFC_CONSTANT::RoamBssEntryExpiryTime,
            &isMatchFound
            );
        if ((status == NDIS_STATUS_SUCCESS) && (isMatchFound))
        {
            // We are going to give up this roam attempt. Before indicating NLO, send this notification
            WFCActInfo("[Roam][NLO] Notify OS that we are no longer attempting roaming");
            NotifyRoamingAttemptExhausted();
        
            // Send up an NLO indication
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

            // Stop processing any more BSS entries
            m_NloMatchingCriteria.SSIDList.ElementCount = 0;

            // If we already have a low priority OID request pending (normally OID_*_MULTICAST_LIST), 
            // the OS wouldnt be able to trigger a disconnect and reconnect. Since we found an NLO match
            // and we have already tried instant connect, lets cancel the roam to help the OS
            WFCActInfo("[Roam][NLO] NLO candidates found. Cancelling resume from Dx Roam job");            
            OnJobCancelled();
        }
        
        status = NDIS_STATUS_SUCCESS;        
    }

exit:
    TraceActExit(status);
}

//==============================================================================================

CDisconnectJob::CDisconnectJob(
    _In_ WFC_ACTIVITY_ID ActivityId) :
    COidJobBase(ActivityId),
    m_DisconnectParamsTlvBufferLength(0),
    m_pDisconnectParamsTlvBuffer(NULL),
    m_DisconnectTask(ActivityId),
    m_OidRequestId(NULL),
    m_OidPortNumber(0),
    m_FullDisconnect(false)
{

}

CDisconnectJob::~CDisconnectJob()
{
    if (m_pDisconnectParamsTlvBuffer != NULL)
    {
        FreeGenerated(m_pDisconnectParamsTlvBuffer);
        m_pDisconnectParamsTlvBuffer = NULL;
    }
}
void CDisconnectJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    bool completedJob = false;
    
    TraceActEntry();

    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    NT_ASSERT(pPort != NULL);

    WFCActInfo("[Connect][Link] Disconnect Job started");

    if (pPort->GetDot11State() == WfcPortDot11StateInit)
    {
        status = NDIS_STATUS_INVALID_STATE;
        WFCActTrace("[Connect] Already disconnected. Bailing out of disconnect job");
        goto exit;
    }

    // Only if we are currently connected, we need to trigger a disconnect
    CConnectedPeer* pPeer = pPort->GetConnectedPeerList()
        ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);
    if (pPeer == NULL)
    {
        WFCActTrace("Not current connected. Ending the disconnect job.");
        completedJob = true;
        goto exit;
    }


    ASSIGN_NDIS_OBJECT_HEADER(
        m_NdisDisassocParameters.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_DISASSOCIATION_PARAMETERS_REVISION_1,
        sizeof(m_NdisDisassocParameters));

    //
    // TODO: In AP case, need to get MacAddr from elsewhere
    //
    RtlCopyMemory(
        &m_NdisDisassocParameters.MacAddr,
        pPeer->GetMacAddress(),
        sizeof(m_NdisDisassocParameters.MacAddr));

    //
    // TODO: In AP case, need to get MacAddr from elsewhere
    //
    RtlCopyMemory(
        &m_DisconnectParameters.DisconnectParameters.MacAddress,
        pPeer->GetMacAddress(),
        sizeof(m_DisconnectParameters.DisconnectParameters.MacAddress));

    if (m_pDisconnectParamsTlvBuffer != NULL)
    {
        FreeGenerated(m_pDisconnectParamsTlvBuffer);
        m_pDisconnectParamsTlvBuffer = NULL;
    }

    status = GenerateWdiTaskDisconnect(
        &m_DisconnectParameters,
        sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &m_DisconnectParamsTlvBufferLength,
        &m_pDisconnectParamsTlvBuffer
        );

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't make Disconnect TLVs");
        goto exit;
    }

    status = m_DisconnectTask.Initialize(
        m_pAdapter->GetNotificationManager(),
        m_pAdapter->GetDeviceCommandScheduler(),
        m_pAdapter->GetEventQueue());

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't initialize disconnect task.");
        goto exit;
    }

    DeviceCommand* pTaskDeviceCommand;

    status = m_DisconnectTask.get_TaskDeviceCommand(
        &pTaskDeviceCommand);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't get task device command.");
        goto exit;
    }

    status = pTaskDeviceCommand->Initialize(
        get_PortId(),
        WDI_TASK_DISCONNECT,
        m_DisconnectParamsTlvBufferLength,
        m_pDisconnectParamsTlvBuffer);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't initialize disconnect task device command.");
        goto exit;
    }

    status = StartTask(
        &m_DisconnectTask);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't start disconnect task.");
        goto exit;
    }

    // Regardless of the outcome of the task, link will be lost. However for full
    // disconnects, we get into init state to not trigger any other functionality
    if (m_FullDisconnect)
    {
        pPort->SetDot11State(WfcPortDot11StateInit);
    }
    else
    {
        // Stay in op state       
        pPort->SetDot11State(WfcPortDot11StateOp);
    }

exit:
    TraceActExit(status);

    if ((status != NDIS_STATUS_SUCCESS) || (completedJob == TRUE))
    {
        WFCActInfo("[Connect] Disconnect Job completed with status 0x%08x", status);
    
        CompleteJob(
            status);
    }
}

void CDisconnectJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_STATUS commandStatus;

    UNREFERENCED_PARAMETER(SenderObject);

    TraceActEntry();

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        status = OperationStatus;
        WFCActErrorStatus("Disconnect operation failed.");
        goto exit;
    }

    status = CMessageHelper::GetStatusFromTaskOutput(
        &m_DisconnectTask,
        &commandStatus);

    if ((status != NDIS_STATUS_SUCCESS) && (commandStatus == NDIS_STATUS_SUCCESS))
    {
        WFCActErrorStatus("Couldn't get status from task output.");
        goto exit;
    }

    if (commandStatus != NDIS_STATUS_SUCCESS)
    {
        status = commandStatus;
        WFCActErrorStatus("Disconnect WABI command failed.");
        goto exit;
    }

    if (m_FullDisconnect)
    {
        // If we are doing a full disconnect, we own the disassociation indication and would be
        // doing if after we completed the whole process
        WFCInfo("[Connect] Indicating disassociation to NDIS with status 0x%08x", m_NdisDisassocParameters.uReason);
        m_pAdapter->IndicateStatus(
            m_NdisPortNumber,
            NDIS_STATUS_DOT11_DISASSOCIATION,
            m_OidRequestId,
            &m_NdisDisassocParameters,
            sizeof(m_NdisDisassocParameters));
    }
exit:
    WFCActInfo("[Connect] Disconnect Job completed with status 0x%08x", status);

    TraceActExit(status);

    CompleteJob(status);
}

NDIS_STATUS CDisconnectJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;
    pOidRequest->DATA.SET_INFORMATION.BytesRead = 0;

    //
    // Disassociation Reason is copied to the disassociation
    // frame. WDI_ASSOC_STATUS is not the same as disassociation
    // reason, therefore we can't use this value in the WABI command
    // being sent down to the device.
    //
    // From 802.11 specification:
    // DISASSOCIATION_REASON_CODE_STATION_LEAVING_ESS = 3
    //
    m_DisconnectParameters.DisconnectParameters.Disassociation80211Reason = DOT11_MGMT_REASON_DEAUTH_LEAVE_SS;
    m_NdisDisassocParameters.uReason = DOT11_ASSOC_STATUS_DISASSOCIATED_BY_OS;

    // From OID
    m_FullDisconnect = true;

    TraceActExit(status);

    return status;
}

NDIS_STATUS CDisconnectJob::InitializeFromJob(
    _In_ CAdapter *pAdapter,
    _In_ NDIS_PORT_NUMBER PortNumber,
    _In_ bool FullDisconnect,
    _In_ WDI_ASSOC_STATUS DisconnectWabiReason,
    _In_ UINT16 Disassociation80211Reason
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    POID_HANDLER_ENTRY pHandlerInfo;

    TraceActEntry();

    // Some jobs reuse without deleting and cleaning up
    if (m_pDisconnectParamsTlvBuffer != NULL)
    {
        FreeGenerated(m_pDisconnectParamsTlvBuffer);
        m_pDisconnectParamsTlvBuffer = NULL;
    }

    // To simplify things, use the adapter's table to find stuff
    pHandlerInfo = COidJobBase::s_FindOidHandlerEntry(OID_DOT11_DISCONNECT_REQUEST);
    if (pHandlerInfo == NULL)
    {
        WFCActError("Failed to find handler entry for OID_DOT11_DISCONNECT_REQUEST");
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
        WiFiSerializedJobPriority_Lowest, // Parent is already running
        PortNumber
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize OID job base (0x%0x)", ndisStatus);
        goto Exit;
    }

    WFCInfo(
        "[Connect] Initializing disconnect job with disassociation reason %d",
        Disassociation80211Reason);

    m_DisconnectParameters.DisconnectParameters.Disassociation80211Reason = Disassociation80211Reason;
    m_NdisDisassocParameters.uReason = CWabiToDot11Converter::MapAssocStatus(
        DisconnectWabiReason,
        Disassociation80211Reason
        );

    m_FullDisconnect = FullDisconnect;
Exit:

    TraceActExit(ndisStatus);

    return ndisStatus;
}

VOID CDisconnectJob::OnOidJobCompletedEvent(
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

//==============================================================================================
CRoamReconnectJob::CRoamReconnectJob(
    _In_ WFC_ACTIVITY_ID ActivityId) :
    CJobBase(ActivityId),
    m_RoamState(RoamStateInit),
    m_IsCancelled(false),
    m_RoamTrigger(WfcRoamConnectTriggerAssociationLoss),
    m_pConnectJob(NULL),
    m_RoamStartIndicated(false),
    m_IsP2PClient(false),
    m_RoamDeferCount(0)
{
}

CRoamReconnectJob::~CRoamReconnectJob()
{
    if (m_pConnectJob)
    {
        delete m_pConnectJob;
        m_pConnectJob = NULL;
    }
}

NDIS_STATUS CRoamReconnectJob::Initialize(
    _In_ CAdapter* pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _In_ bool IsP2PClient,
    _In_ WFC_ROAM_CONNECT_TRIGGER RoamTrigger,
    _In_ WDI_ASSOC_STATUS RoamWabiReason,
    _In_ WDI_ROAM_CONFIGURATION_FLAGS RoamConfigFlags
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    if (pAdapter == NULL)
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        WFCActErrorStatus("Bad parameter to %!FUNC!");
        goto exit;
    }

    m_RoamTrigger = RoamTrigger;
    m_RoamWabiReason = RoamWabiReason;
    m_RoamConfigFlags = RoamConfigFlags;
    m_pAdapter = pAdapter;
    m_IsP2PClient = IsP2PClient;

    status = CJobBase::Initialize(
        pAdapter->GetEventQueue(),
        pAdapter->GetDeviceCommandScheduler(),
        pAdapter->GetActiveJobsList(),
        pAdapter->GetSerializedJobList(),
        WiFiJobRoam,
        true,
        false,
        WiFiPriorityScopeAdapter,
        WiFiSerializedJobPriority_5, // This is only used if this job is serialized.
        (IPropertyCacheDirectory*)pAdapter,
        (INdisConversion*)pAdapter,
        NdisPortNumber);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't initialize CJobBase.");
        goto exit;
    }

exit:

    TraceActExit(status);
    return status;
}

void CRoamReconnectJob::FinishJob(
    _In_ NDIS_STATUS OperationStatus)
{
    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    WFCActInfo("[Roam] Roam Job completed (0x%x)", OperationStatus);

    m_RoamState = RoamStateDone;
    pPort->RegisterForDisassociationNotifications(NULL);

    if (m_RoamStartIndicated)
    {
        // We shouldnt have reached here without the roaming completion indication
        NT_ASSERT(FALSE);
        
        NotifyRoamingCompletionIfNecessary(
            OperationStatus 
            );
        m_RoamStartIndicated = false;
    }

    CompleteJob(
        OperationStatus);
}

VOID CRoamReconnectJob::NotifyRoamingStart()
{
    DOT11_ROAMING_START_PARAMETERS roamingStartParameters = { 0 };
    TraceActEntry();

    WFCActTrace("[Roam] Roaming Start Indication");
    
    ASSIGN_NDIS_OBJECT_HEADER(
        roamingStartParameters.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_ROAMING_START_PARAMETERS_REVISION_1,
        sizeof(DOT11_ROAMING_START_PARAMETERS));

    roamingStartParameters.uRoamingReason 
        = CWabiToDot11Converter::MapAssocStatus(m_RoamWabiReason, 0);

    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_DOT11_ROAMING_START,
        NULL,
        &roamingStartParameters,
        sizeof(roamingStartParameters));
            
    m_RoamStartIndicated = true;

    TraceActExit(NDIS_STATUS_SUCCESS);
}

VOID CRoamReconnectJob::NotifyRoamingCompletionIfNecessary(
    _In_ NDIS_STATUS OperationStatus
    )
{
    DOT11_ROAMING_COMPLETION_PARAMETERS roamingCompletionParams = { 0 };
    WDI_ASSOC_STATUS associationCompletionStatus;
    
    TraceActEntry();

    if (!m_RoamStartIndicated || m_pConnectJob == NULL)
    {
        // In the case of roaming if we didnt do disassociation, we wouldnt have reported
        // roaming start, so completion isnt interesting
        // Also, in the case when connect job for reconnect completed because port was 
        // already connected, we wouldn't have reported roaming start
        WFCActTrace(
            "[Roam] No roaming start indication, so no need to indicate roaming completion");
        goto exit;
    }

    WFCActTrace("[Roam] Roaming Completion Indication with status 0x%08x", OperationStatus);
    
    ASSIGN_NDIS_OBJECT_HEADER(
        roamingCompletionParams.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_ROAMING_COMPLETION_PARAMETERS_REVISION_1,
        sizeof(DOT11_ROAMING_COMPLETION_PARAMETERS));

    associationCompletionStatus = m_pConnectJob->GetFinalAssociationStatus();
    if (OperationStatus != NDIS_STATUS_SUCCESS &&
        associationCompletionStatus == WDI_ASSOC_STATUS_SUCCESS)
    {
        WFCActInfo(
            "[Roam] Assoc status not set and job failed. Setting generic failure string.");
        associationCompletionStatus = WDI_ASSOC_STATUS_FAILURE;
    }

    roamingCompletionParams.uStatus = CWabiToDot11Converter::MapAssocStatus(
                                            associationCompletionStatus,
                                            0);

    // Send the complete indication to OS
    m_pAdapter->IndicateStatus(
        m_NdisPortNumber,
        NDIS_STATUS_DOT11_ROAMING_COMPLETION,
        NULL,
        &roamingCompletionParams,
        sizeof(roamingCompletionParams));

    m_RoamStartIndicated = false;

exit:
    TraceActExit(NDIS_STATUS_SUCCESS);
}

NDIS_STATUS CRoamReconnectJob::StartConnectJob(
    _In_opt_ WFC_CONNECT_SCAN_TYPE InitialScanType)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    TraceActEntry();

    WFCActInfo("[Roam] Connect for Roam Job started. Trigger = %d. Config = 0x%08x", m_RoamTrigger, m_RoamConfigFlags);

    if (pPort->GetDot11State() == WfcPortDot11StateInit)
    {
        status = NDIS_STATUS_INVALID_STATE;
        WFCActInfo("[Roam] Stopping Roam Job because we are not expected to be connected. Status = 0x%08x", status);
        goto exit;
    }

    NT_ASSERT(m_pConnectJob == NULL);

    // Allocate a new connect Job
    m_pConnectJob = new CConnectJob(get_ActivityId());
    if (m_pConnectJob == NULL)
    {
        WFCActError("[Roam] Failed to allocate connect Job");
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    status = m_pAdapter->ConvertPortIdToNdisPortNumber(
        m_PortId,
        &m_NdisPortNumber);

    if (status != NDIS_STATUS_SUCCESS)
    {
        NT_ASSERT(FALSE);
        WFCActError("[Roam] Couldn't convert port ID to NDIS port number. Error = 0x%o8x", status);
        goto exit;
    }

    // If roam is deferrable and it has already been deferred too many times,
    // change the config to non-deferrable in order to let it run to completion next time.
    // Note: If the maxRoamDeferCount registry is set to 0, it implies that the feature has been disabled.
    ULONG maxRoamDeferCount = WFC_CONSTANT::MaxRoamDeferCountDefault;
    
    maxRoamDeferCount = m_pAdapter->GetAdapterPropertyCache()->GetPropertyULongOrDefault(
        WfcAdapterPropertyRegMaxRoamDeferCount,
        WFC_CONSTANT::MaxRoamDeferCountDefault);

    if ( TestRoamConfig(m_RoamConfigFlags, RC_DEFERRABLE_ROAM) &&
         (m_RoamDeferCount >= maxRoamDeferCount ||
          m_IsP2PClient) // Currently not enabling deferrable roam for P2P
       )
    {
        WFCActInfo("[Roam] Already deferred roam %u times, making next roam non-deferrable", m_RoamDeferCount);
        m_RoamConfigFlags &= ~RC_DEFERRABLE_ROAM;
    }

    status = m_pConnectJob->InitializeForRoamReconnectRequest(
        m_pAdapter,
        m_NdisPortNumber,
        m_IsP2PClient,
        m_RoamTrigger,
        m_RoamWabiReason,
        m_RoamConfigFlags,
        InitialScanType
        );

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("[Roam] Couldn't initialize connect job for roam request.");
        goto exit;
    }

    // Sign up for the job started event. We will send roaming start indication to OS in there.
    m_pConnectJob->EventJobStarted = this;

    WFCActTrace("[Roam] Queueing serialized connect job");

    // Queue the serialized connect job
    status = CJobBase::QueueSerializedJob(
        m_pConnectJob
        );

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Roam] Failed to queue connect job (0x%x)", status);
        goto exit;
    }

    m_RoamState = RoamStateWaitingForConnectCompletion;

exit:
    if (status != NDIS_STATUS_SUCCESS)
    {
        if (m_pConnectJob)
        {
            delete m_pConnectJob;
            m_pConnectJob = NULL;
        }
    }

    TraceActExit(status);

    return status;
}

NDIS_STATUS CRoamReconnectJob::CompleteConnectJob(
    _In_ NDIS_STATUS OperationStatus,
    _Out_ bool* WasCancelled,
    _Out_ bool* WasDeferred,
    _Out_ WFC_CONNECT_SCAN_TYPE* LastScanType
    )
{
    NDIS_STATUS roamCompleteStatus = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    *WasDeferred  = m_pConnectJob->IsDeferred();
    *LastScanType = m_pConnectJob->get_LastScanType();
    *WasCancelled = m_pConnectJob->IsCancelled();

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        CPort* pPort = m_pAdapter->GetPortFromPortId(
            get_PortId());
            
        CConnectedPeer* pPeer = pPort->GetConnectedPeerList()
            ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);

        // Failure just means the connect job didnt proceed. But lets check
        // if it didnt proceed because we were already connected

        if (pPeer)
        {
            WFCActInfo("[Roam] Ignoring Connect job failure 0x%08x since we are already connected", OperationStatus);
            roamCompleteStatus = NDIS_STATUS_SUCCESS;

            // NDIS_STATUS_ALREADY_MAPPED is returned when Reconnect job bails because we were already
            // connected by the time it ran. Suppress the error only in that case since it is not a
            // real error. Return the original status to caller in all other cases.
            if (TestRoamConfig(m_RoamConfigFlags, RC_RECONNECT)    &&
                (OperationStatus == NDIS_STATUS_ALREADY_MAPPED))
            {
                OperationStatus = NDIS_STATUS_SUCCESS;
            }
        }
        else
        {
            WFCActError("[Roam] Connect job failed. Error = 0x%08x", OperationStatus);
            roamCompleteStatus = OperationStatus;
        }
        goto exit;
    }

exit:
    NotifyRoamingCompletionIfNecessary(
        roamCompleteStatus
        );

    // Delete the connect job
    delete m_pConnectJob;
    m_pConnectJob = NULL;

    TraceActExit(OperationStatus);

    return OperationStatus;
}

void CRoamReconnectJob::OnOperationStarted(
    _In_ NDIS_STATUS OperationStatus,
    _In_opt_ WFC_SENDER_OBJECT SenderObject)
{
    TraceActEntry();

    UNREFERENCED_PARAMETER(SenderObject);

    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    // Connect job has started
    // Send the roaming start indications if necessary
    // Depending on the flags, the indication logic would be different
    if (TestRoamConfig(m_RoamConfigFlags, RC_CONNECTED_ROAM))
    {
        // In this case we are not going to report roaming just yet. We will wait
        // to see if we lose connection and then report roaming. Setup for receiving the
        // roam completion indication
        pPort->RegisterForDisassociationNotifications((IDisassociationCompleteCallback*)this);
    }
    else
    {
        CConnectedPeer* pPeer = pPort->GetConnectedPeerList()
            ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);

        NT_ASSERT(TestRoamConfig(m_RoamConfigFlags, RC_RECONNECT));

        // Send the roaming start indication only if there isn't a connected peer.
        // If the connect job was triggered due to disconnect, but we are already connected 
        // by the time the child connect job started, do nothing. Connect job would finish early
        // in that case.
        if (pPeer == NULL)
        {
            // Lost connection, send the roaming start indication
            NotifyRoamingStart();
        }
    }

    TraceActExit(OperationStatus);
}

void CRoamReconnectJob::OnDisassociationProcessed()
{
    TraceActEntry();
    // We received the disassociation indication
    NT_ASSERT(TestRoamConfig(m_RoamConfigFlags, RC_CONNECTED_ROAM));

    if (!m_RoamStartIndicated)
    {
        // Once the disassociation indication is done, we are allowed to do the roaming start indication
        NotifyRoamingStart();
    }
    TraceActExit(NDIS_STATUS_SUCCESS);
}

void CRoamReconnectJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    WFCActInfo("[Roam] Roam Job started. Trigger = %d. Config = 0x%08x", m_RoamTrigger, m_RoamConfigFlags);
   
    status = StartConnectJob();
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("[Roam] Couldn't start connect job.");
        goto exit;
    }
    
exit:
    if (status != NDIS_STATUS_SUCCESS)
    {
        FinishJob(
            status);
    }

    TraceActExit(status);
}


NDIS_STATUS CRoamReconnectJob::CompleteAndRequeueConnectJobIfNecessary(
    _In_ NDIS_STATUS OperationStatus
    )
{
    bool bIsCancelled = false;
    bool bIsJobDeferred = false;
    WFC_CONNECT_SCAN_TYPE lastScanType;
    
    TraceActEntry();

    NT_ASSERT(m_pConnectJob != NULL);

    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    OperationStatus = CompleteConnectJob(OperationStatus,
        &bIsCancelled,
        &bIsJobDeferred,
        &lastScanType);

    if (!bIsCancelled                                        &&  // If job was cancelled, then don't requeue it
        bIsJobDeferred                                       &&  // Connect ended early because it was deferred
        (OperationStatus == NDIS_STATUS_NETWORK_UNREACHABLE)     // Connect did not complete successfully
       )
    {
        // Requeue the Connect Job;
        WFCActError("[Roam] Requeueing connect job since the last one was deferred - lastScanType %d", lastScanType);
        OperationStatus = StartConnectJob(lastScanType);

        if (OperationStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("[Roam] Connect requeue failed with 0x%08x", OperationStatus);
            goto exit;
        }
      
        m_RoamDeferCount++;
        // Unregister the diassociation notification. We'll re-register when the queued connect job starts if necessary.
        pPort->RegisterForDisassociationNotifications(NULL);
        // don't complete roam yet
        OperationStatus = NDIS_STATUS_PENDING;
    }
    else if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[Roam] Connect completion failed with 0x%08x", OperationStatus);
        goto exit;
    }
        
exit:
    TraceActExit(OperationStatus);

    return OperationStatus;
}

void CRoamReconnectJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    bool bCompleteJob = false;
    UNREFERENCED_PARAMETER(SenderObject);

    WFCActTrace("[Roam] Step %d completed with 0x%08x", m_RoamState, OperationStatus);

    switch (m_RoamState)
    {
    case RoamStateWaitingForConnectCompletion:
        OperationStatus = CompleteAndRequeueConnectJobIfNecessary(OperationStatus);

        if (OperationStatus == NDIS_STATUS_PENDING)
        {
            // Connect was requeued - don't complete roam job
            WFCActInfo("[Roam] Connect requeued - not completing roam job");
            OperationStatus = NDIS_STATUS_SUCCESS;
            goto exit;
        }
        else if (OperationStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("[Roam] Connect attempt failed with 0x%08x", 
                OperationStatus);
            goto exit;
        }
        // Done
        bCompleteJob = true;

        break;

    default:
        NT_ASSERT(FALSE);
        OperationStatus = NDIS_STATUS_FAILURE;
        break;
    };

exit:
    if ((OperationStatus != NDIS_STATUS_SUCCESS) || (bCompleteJob == true))
    {
        FinishJob(
            OperationStatus);
    }
}

void CRoamReconnectJob::OnJobCancelled()
{
    TraceActEntry();

    if (m_IsCancelled)
    {
        goto exit;
    }

    WFCActInfo("[Roam] Roam Job cancelled (Current State %d)", m_RoamState);

    m_IsCancelled = true;

    if (m_pConnectJob)
    {
        // Cancel the serialized scan job
        WFCActTrace("[Roam] Cancelling the Child connect Job");
        m_pAdapter->GetSerializedJobList()->CancelJob(
            m_NdisPortNumber,
            WiFiJobTypeConnect,
            m_pConnectJob
            );
    }
    else
    {
        //
        // Cancel called when there was nothing pending.
        //
    }

exit:
    TraceActExit(NDIS_STATUS_SUCCESS);
}



//==============================================================================================
CAssociationParametersRequestResponderJob::CAssociationParametersRequestResponderJob(
    _In_ WFC_ACTIVITY_ID ActivityId) :
    CJobBase(ActivityId),
    m_AssociationParametersJobState(AssociationParametersRequestStateInit),
    m_PMKIDNotificationRegistrationToken(WFC_REGISTRATION_TOKEN_INVALID),
    m_PMKIDTimerToken(WFC_TIMER_REGISTRATION_TOKEN_INVALID),
    m_IsCancelled(false),
    m_DeviceCommand(ActivityId)
{

}

CAssociationParametersRequestResponderJob::~CAssociationParametersRequestResponderJob()
{
    if (m_PMKIDTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        (VOID)m_pEventQueue->DeregisterTimeoutCallback(
            m_PMKIDTimerToken);
        m_PMKIDTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    if (m_pCommandBuffer)
    {
        delete[] m_pCommandBuffer;
        m_pCommandBuffer = NULL;
    }
}

NDIS_STATUS CAssociationParametersRequestResponderJob::InitializeFromAssociationParametersRequest(
    _In_ CAdapter* pAdapter,
    _In_ NDIS_PORT_NUMBER PortNumber,
    _In_ UINT16 WfcPortId,
    _In_ WDI_INDICATION_ASSOCIATION_PARAMS_REQUEST_PARAMETERS* pAssociationParametersRequest
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    m_pAdapter = pAdapter;
    m_NdisPortNumber = PortNumber;    
    m_PortId = WfcPortId;
    
    // Initialize the base
    ndisStatus = CJobBase::Initialize(
        m_pAdapter->GetEventQueue(),
        m_pAdapter->GetDeviceCommandScheduler(),
        m_pAdapter->GetActiveJobsList(),
        m_pAdapter->GetSerializedJobList(),
        WiFiJobAssociationParametersRequestResponder,
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
        WFCActError("[AssocParam] Failed to initialize base job for association parameters (0x%x)", ndisStatus);
        goto Exit;
    }

    // Check what all is being requested to be updated
    for (UINT32 requestCount = 0; 
            requestCount < pAssociationParametersRequest->AssociationParameterTypes.ElementCount;
            requestCount++)
    { 
        if (pAssociationParametersRequest->AssociationParameterTypes.pElements[requestCount] == WDI_TLV_PMKID)
        {
            WFCActTrace("[AssocParam] PMKID List Update requested");
        
            // Set global flag marking waiting for PMKID list updates for association parameters
            m_pAdapter->GetPortPropertyCache(m_PortId)->SetPropertyBoolean(
                WfcPortPropertyAssociationParameterPMKIDUpdatePending, 
                TRUE );
        }
    }

Exit:

    TraceActExit(ndisStatus);

    return ndisStatus;
}

void CAssociationParametersRequestResponderJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    bool bWaitingForData = false;

    TraceActEntry();

    // By default, we are waiting for response data
    m_AssociationParametersJobState = AssociationParametersRequestStateWaitingForData;
    
    // If we are still waiting for some properties, start the wait
    if (m_pAdapter->GetPortPropertyCache(m_PortId)->GetPropertyBooleanOrDefault(
            WfcPortPropertyAssociationParameterPMKIDUpdatePending, FALSE))
    {
        WFCActTrace("[AssocParam] PMKID List Update request pending");
        bWaitingForData = true;
        
        ndisStatus = StartWaitForPMKIDList();
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("[AssocParam] Failed to start wait for PMKID list update (0x%x)", ndisStatus);
        }
    }

    if (!bWaitingForData || (ndisStatus != NDIS_STATUS_SUCCESS))
    {
        SendAssociationParametersResponse(ndisStatus);
    }

    TraceActExit(ndisStatus);    
}

void CAssociationParametersRequestResponderJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    UNREFERENCED_PARAMETER(SenderObject);
    switch (m_AssociationParametersJobState)
    {
        case AssociationParametersRequestStateWaitingForData:
            {
                // We were waiting for data, go something
                WFCActTrace("[AssocParam] Completed wait for association parameters request data. Status = 0x%08x", 
                    OperationStatus);

                // We will send the response command
                SendAssociationParametersResponse(OperationStatus);
                
                // This will take care of next step
                OperationStatus = NDIS_STATUS_SUCCESS;
            }
            break;

        case AssociationParametersRequestStateWaitingForResponseCompletion:
            {
                WFCActTrace("[AssocParam] Completed command to set association parameters response. Status = 0x%08x", 
                    OperationStatus);
                if ( OperationStatus != NDIS_STATUS_SUCCESS )
                {
                    WFCActError("[AssocParam] Device command failed with status %x.", OperationStatus);
                }
                else
                {
                    NDIS_STATUS commandStatus;
                    OperationStatus = CMessageHelper::GetStatusFromCommandResult(
                        &m_DeviceCommand,
                        &commandStatus );

                    if ( OperationStatus != NDIS_STATUS_SUCCESS )
                    {
                        WFCActError("[AssocParam] Couldn't get WABI status from command result 0x%08x", OperationStatus);
                    }
                }            
                // Complete the job
                CompleteJob(OperationStatus);
            }
            break;

        default:
            NT_ASSERT(FALSE);
    }
}

void CAssociationParametersRequestResponderJob::OnJobCancelled()
{
    TraceActEntry();

    if (m_IsCancelled)
    {
        goto exit;
    }

    WFCActInfo("[AssocParam] Association parameters responder Job cancelled (Current State %d)", m_AssociationParametersJobState);

    m_IsCancelled = true;

    if (m_AssociationParametersJobState == AssociationParametersRequestStateWaitingForResponseCompletion)
    {
        CancelActivePendingCommand();
    }
    else if (m_AssociationParametersJobState == AssociationParametersRequestStateWaitingForData)
    {
        StopWaitForPMKIDList(NDIS_STATUS_REQUEST_ABORTED);
    }

exit:
    TraceActExit(NDIS_STATUS_SUCCESS);

}

NDIS_STATUS CAssociationParametersRequestResponderJob::StartWaitForPMKIDList()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    // Queue the timer for the indication
    status = m_pEventQueue->RegisterTimeoutCallbackWithLevelAndReuse(
        get_ActivityId(),
        this,
        WFC_CONSTANT::PMKIDUpdateWaitTimeMs,
        NULL,
        WDF_EXECUTION_LEVEL::WdfExecutionLevelPassive,
        false,
        NULL,
        &m_PMKIDTimerToken);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[AssocParam] Failed to register for PMKID list timer callback (0x%x)", status);
        goto Exit;
    }

    // Start listening for the PMKID list indication
    status = m_pAdapter->GetNotificationManager()->RegisterForNotifications(
        WiFiIndicationPmkIdCandidateListUpdate,
        WFC_COMMAND_TOKEN_INVALID,
        get_PortId(),
        (INotifyDeviceIndicationCallback*)this,
        &m_PMKIDNotificationRegistrationToken);

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[AssocParam] Failed to register for PMKID list update indication (0x%x)", status);
        goto Exit;
    }

    // Mark the job as pending
    SetPending();    
        
Exit:

    if (status != NDIS_STATUS_SUCCESS)
    {
        // This will stop the timer from firing anymore
        if (m_PMKIDTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
        {
            (VOID)m_pEventQueue->DeregisterTimeoutCallback(
                m_PMKIDTimerToken);
            m_PMKIDTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
        }

        // This will stop the event from coming in
        if (m_PMKIDNotificationRegistrationToken != WFC_REGISTRATION_TOKEN_INVALID)
        {
            m_pAdapter->GetNotificationManager()->DeregisterNotificationHandler(
                m_PMKIDNotificationRegistrationToken);
            m_PMKIDNotificationRegistrationToken = WFC_REGISTRATION_TOKEN_INVALID;
        }
    }
    return status;
}


VOID CAssociationParametersRequestResponderJob::StopWaitForPMKIDList(
    _In_ NDIS_STATUS StopStatus
    )
{
    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    CPortPropertyCache *pPropertyCache = pPort->GetPropertyCache();

    // Clear waiting for PMKID list updates
    pPropertyCache->SetPropertyBoolean(
        WfcPortPropertyAssociationParameterPMKIDUpdatePending,
        FALSE);

    // This will stop the timer from firing anymore
    if (m_PMKIDTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        (VOID)m_pEventQueue->DeregisterTimeoutCallback(
            m_PMKIDTimerToken);
        m_PMKIDTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    // This will stop the event from coming in
    if (m_PMKIDNotificationRegistrationToken != WFC_REGISTRATION_TOKEN_INVALID)
    {
        m_pAdapter->GetNotificationManager()->DeregisterNotificationHandler(
            m_PMKIDNotificationRegistrationToken);
        m_PMKIDNotificationRegistrationToken = WFC_REGISTRATION_TOKEN_INVALID;
    }

    // Since this is the only thing we are waiting for, mark it as done
    OnOperationCompleted(StopStatus, this);
    
}

void CAssociationParametersRequestResponderJob::OnDeviceIndicationArrived(
    _In_ WDI_INDICATION_TYPE                 IndicationType,
    _In_ WFC_COMMAND_TOKEN                   CommandToken,
    _In_ WFC_PORT_ID                         PortId,
    _In_ ULONG                               EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer)
{
    UNREFERENCED_PARAMETER(CommandToken);
    UNREFERENCED_PARAMETER(PortId);

    switch (IndicationType)
    {
    case WiFiIndicationPmkIdCandidateListUpdate:
        {
           OnPMKIDListUpdateNotificationArrived(
               EventBufferLength,
               EventBuffer);
           break;
        }

    }

}

void CAssociationParametersRequestResponderJob::OnPMKIDListUpdateNotificationArrived(
    _In_ ULONG  BufferLength,
    _In_reads_bytes_(BufferLength) BYTE* Buffer)
{
    UNREFERENCED_PARAMETER(BufferLength);
    UNREFERENCED_PARAMETER(Buffer);

    WFCActTrace("[AssocParam] PMKID List Update arrived. Continuing the association parameters response");
    StopWaitForPMKIDList(NDIS_STATUS_SUCCESS);
}


void CAssociationParametersRequestResponderJob::OnTimerCallback(
    _In_opt_ WFC_TIMER_CONTEXT)
{
    WFCActTrace("[AssocParam] PMKID List Update time limit expired. Continuing the association parameters response");
    StopWaitForPMKIDList(NDIS_STATUS_REQUEST_ABORTED);
}

VOID CAssociationParametersRequestResponderJob::SendAssociationParametersResponse(
    _In_ NDIS_STATUS DataAvailableStatus
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BSS_ENTRY_CONNECT_MATCHING_CRITERIA matchingCriteria = { 0 };
    CPortPropertyCache *pPropertyCache = m_pAdapter->GetPortPropertyCache(m_PortId);
    CPort* pPort = m_pAdapter->GetPortFromPortId(m_PortId);

    UNREFERENCED_PARAMETER(DataAvailableStatus);
    
    // We are now in the phase of sending the device a response
    m_AssociationParametersJobState = AssociationParametersRequestStateWaitingForResponseCompletion;

    // First we go through the BSS list and find the matching set of APs. We will send the association
    // parameters for those APs down
    ndisStatus = CConnectHelpers::PopulateConnectionParameters(
        pPropertyCache,
        &m_ConnectionParameters
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[AssocParam] Failed to populate connection parameters (0x%x)", ndisStatus);
        goto exit;
    }

    matchingCriteria.pConnectionParameters = &m_ConnectionParameters;

    // Do the match. Since this is not tied to connect, we will use the full 
    // set of live APs
    m_TempBSSCandidateCount = celems(m_TempBSSCandidateList);
    ndisStatus = m_pAdapter->GetBSSListManager(WfcPortTypeExtSTA)->FindMatchingBSSEntriesForConnect(
        &matchingCriteria,
        WFC_CONSTANT::ConnectBssEntryExpiryTime,
        &m_TempBSSCandidateCount,
        m_TempBSSCandidateList);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[AssocParam] Failed to find matching bss entries (0x%x)", ndisStatus);
        goto exit;
    }

    bool isFTAllowed = CConnectHelpers::IsFastTransitionAllowed(pPort, m_pAdapter, &m_ConnectionParameters);

    // Next we need to generate the TLV data blob for the command    
    // Copy the matching entries into the TLV input data structure
    ndisStatus = CConnectHelpers::FillConnectBSSEntryTLV(
        pPort,
        isFTAllowed,
        m_TempBSSCandidateCount,
        m_TempBSSCandidateList,
        &m_AssociationParameters.BSSEntryList,
        m_ConnectionParameters.RoamRequest?true:false
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[AssocParam] Failed to fill Connect BSS Entry TLV. Error = 0x%08x", ndisStatus);
        goto exit;
    }    

    // Generate the TLV data
    ndisStatus = GenerateWdiSetAssociationParameters(
        &m_AssociationParameters,
        sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &m_CommandSize,
        &m_pCommandBuffer
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[AssocParam] Failed to generate association parameters tlv blob (0x%x)", ndisStatus);
        goto exit;
    }



    // Initialize the device command with this TLV data
    ndisStatus = m_DeviceCommand.Initialize(
        m_PortId,
        WDI_SET_ASSOCIATION_PARAMETERS,
        m_CommandSize,
        m_pCommandBuffer        
        );
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("[AssocParam] Failed to initialize device command for association parameters. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    // Queue the command
    ndisStatus = QueueDeviceCommand(&m_DeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[AssocParam] Failed to queue device command for association parameters response (0x%x)", ndisStatus);
        goto exit;
    }

exit:
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        // If we arent waiting, move on to the next step
        OnJobStepCompleted(ndisStatus, this);
    }
}

//==============================================================================================

/*++
Queries HESSID Connection parameters
    WiFiJobQueryDesiredHESSIDList
--*/

NDIS_STATUS CQueryDesiredHESSIDList::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded)
{
    NDIS_STATUS         status;
    DOT11_HESSID_LIST   *pHESSIDList = NULL;
    ULONG               uBytes = 0;

    UNREFERENCED_PARAMETER(pBytesNeeded);
    UNREFERENCED_PARAMETER(InformationBufferLength);

    *pBytesWritten = 0;

    status = GetPortPropertyCache()->GetPropertyBuffer(
        WfcPortPropertyHESSID,
        &uBytes,
        (PBYTE *) &pHESSIDList);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("FAIled to find WfcPortPropertyHESSID in port Property Cache.");
        goto Exit;
    }

    if ((!uBytes) ||
        (uBytes < FIELD_OFFSET(DOT11_HESSID_LIST,HESSIDs)))
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCActErrorStatus("Invalid HESSID list in property cache.");
        goto Exit;
    }

    *pBytesNeeded = uBytes;
    if(InformationBufferLength < uBytes)
    {
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        WFCActError("CQueryDesiredHESSIDList: QueryData. Buffer too short <%d < %d>, Error = 0x%08x",
            InformationBufferLength, uBytes, status);
        goto Exit;
    }

    RtlZeroMemory(pInformationBuffer, InformationBufferLength);
    RtlCopyMemory(pInformationBuffer, pHESSIDList, uBytes);

    *pBytesWritten = *pBytesNeeded;

Exit:
    return (status);
}


NDIS_STATUS CSetDesiredHESSIDList::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status;
    DOT11_HESSID_LIST   *pHESSIDList;

    UNREFERENCED_PARAMETER(pInformationBuffer);
    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(pBytesRead);
    UNREFERENCED_PARAMETER(pBytesNeeded);

    //
    // Check InformationBufferLength
    //
    pHESSIDList = (DOT11_HESSID_LIST *) pInformationBuffer;
    if ((InformationBufferLength < FIELD_OFFSET(DOT11_HESSID_LIST, HESSIDs)) ||
        (InformationBufferLength < (FIELD_OFFSET(DOT11_HESSID_LIST, HESSIDs) +
                                    (pHESSIDList->NumberOfHESSIDs * sizeof(DOT11_HESSID)))) ||
        (InformationBufferLength < (pHESSIDList->HSIndicationElementOffset + pHESSIDList->HSIndicationElementLength)) ||
        (pHESSIDList->HSIndicationElementOffset < (FIELD_OFFSET(DOT11_HESSID_LIST, HESSIDs) +
                                                   (pHESSIDList->NumberOfHESSIDs * sizeof(DOT11_HESSID)))))
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCActErrorStatus("SetDesiredHESSIDList: INVALID data - BufferLength=%d, Hessids=%d, IEOffset:Length=%d:%d\n",
            InformationBufferLength, pHESSIDList->NumberOfHESSIDs,
            pHESSIDList->HSIndicationElementOffset, pHESSIDList->HSIndicationElementLength);
        goto Exit;
    }

    status = GetPortPropertyCache()->SetPropertyBuffer(
        (WFC_PROPERTY_NAME) WfcPortPropertyHESSID,
        InformationBufferLength,
        (PBYTE) pHESSIDList);

Exit:
    return status;
}


NDIS_STATUS CSetFTReassociationParametersJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    UINT  BytesNeeded, BufferLength;
    PDOT11_FT_REASSOCIATION_PARAMETERS pReassociationParams = (PDOT11_FT_REASSOCIATION_PARAMETERS) SET_OID_BUFFER(pOidRequest);

    TraceActEntry();

    if (!VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pReassociationParams->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_FT_REASSOCIATION_PARAMETERS_REVISION_1,
        SIZEOF_DOT11_FT_REASSOCIATION_PARAMETERS_REVISION_1))
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCActError("[11r] Invalid NDIS object header. Expected (T:%d R:%d S:%d) Received (T:%d R:%d S:%d)",
            NDIS_OBJECT_TYPE_DEFAULT, DOT11_FT_REASSOCIATION_PARAMETERS_REVISION_1,
            SIZEOF_DOT11_FT_REASSOCIATION_PARAMETERS_REVISION_1, pReassociationParams->Header.Type,
            pReassociationParams->Header.Revision, pReassociationParams->Header.Size
            );

        goto exit;
    }

    BufferLength = SET_OID_BUFFER_LENGTH(pOidRequest);

    BytesNeeded = FIELD_OFFSET(DOT11_FT_REASSOCIATION_PARAMETERS, uFTEIEOffset);
    BytesNeeded += pReassociationParams->uFTEIESize +
        pReassociationParams->uMDDIESize +
        pReassociationParams->uRSNIESize +
        pReassociationParams->uR0KHIDSize;

    if (BufferLength < BytesNeeded)
    {
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        WFCActError("CSetFTReassociationParametersJob::InitializeFromOidRequest: [11r] Insufficient data. %d < %d",
            BufferLength, BytesNeeded);
        goto exit;
    }

    m_pFTReassociationParameters = new BYTE [BufferLength];
    if (!m_pFTReassociationParameters)
    {
        status = NDIS_STATUS_RESOURCES;
        WFCActError("CSetFTReassociationParametersJob::InitializeFromOidRequest: [11r] Couldn't allocate %d bytes.",
            BufferLength);
        goto exit;
    }

    RtlCopyMemory(m_pFTReassociationParameters, pReassociationParams, BufferLength);

exit:
    TraceActExit(status);
    return status;
}


NDIS_STATUS CSetFTReassociationParametersJob::QueryNextCommandInfo(
    _In_ ULONG ReservedHeaderLength,
    _Inout_ WFC_DEVICE_COMMAND_TOKEN* pCommandToken,
    _Out_ WFC_DEVICE_COMMAND_TYPE* pCommandType,
    _Out_ UINT32* pMessageID,
    _Out_ ULONG* pCommandBufferLength,
    _Outptr_result_buffer_(*pCommandBufferLength) BYTE** ppCommandBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_SET_FAST_BSS_TRANSITION_PARAMETERS_COMMAND ftReassociationParams;
    PDOT11_FT_REASSOCIATION_PARAMETERS pReassociationParams = (PDOT11_FT_REASSOCIATION_PARAMETERS) m_pFTReassociationParameters;

    TraceActEntry();

    ftReassociationParams.status = pReassociationParams->uStatus;
    if (pReassociationParams->uStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError(
            "[11r] Computing reassociation request params failed. Status = 0x%08x. Not Filling up the FT parameters in TLV",
            pReassociationParams->uStatus
            );
    }
    else
    {
        ftReassociationParams.Optional.RSNIE_IsPresent = true;
        ftReassociationParams.RSNIE.SimpleAssign(
            (((PUCHAR) pReassociationParams) + pReassociationParams->uRSNIEOffset),
            pReassociationParams->uRSNIESize
            );

        ftReassociationParams.Optional.MDE_IsPresent = true;
        ftReassociationParams.MDE.SimpleAssign(
            (((PUCHAR)pReassociationParams) + pReassociationParams->uMDDIEOffset),
            pReassociationParams->uMDDIESize
            );

        ftReassociationParams.Optional.FTE_IsPresent = true;
        ftReassociationParams.FTE.SimpleAssign(
            (((PUCHAR)pReassociationParams) + pReassociationParams->uFTEIEOffset),
            pReassociationParams->uFTEIESize
            );

        status = GetPortPropertyCache()->SetPropertyBuffer((WFC_PROPERTY_NAME)WfcPortPropertyPMKR0Name, SIZEOF_PMKR0NAME,
            pReassociationParams->pmkr0name);
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActError("[11r] Set property cache for PMKR0Name failed. Error = 0x%08x", status);
            goto exit;
        }

        status = GetPortPropertyCache()->SetPropertyBuffer((WFC_PROPERTY_NAME)WfcPortPropertyR0KHID, pReassociationParams->uR0KHIDSize,
            (BYTE*)(((PUCHAR)pReassociationParams) + pReassociationParams->uR0KHIDOffset));
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActError("[11r] Set property cache for R0KH-ID failed. Error = 0x%08x", status);
            goto exit;
        }

        //Mark this as a FT Connection
        GetPortPropertyCache()->SetPropertyBoolean((WFC_PROPERTY_NAME)WfcPortPropertyCurrentConnectionFT, TRUE);
    }
    status = GenerateWdiSetFastBssTransitionParameters(
        &ftReassociationParams,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext(),
        pCommandBufferLength,
        ppCommandBuffer
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[11r] Failed to generate BSS fast transition params tlv. Error = 0x%08x", status);
        goto exit;
    }

    *pCommandToken = NULL;
    *pCommandType = WfcDeviceCommandTypeProperty;
    *pMessageID = WDI_SET_FAST_BSS_TRANSITION_PARAMETERS;

exit:
    TraceActExit(status);
    return status;
}

void CSetFTReassociationParametersJob::OnNotifyJobCompletion(
    _In_ WFC_DEVICE_COMMAND_TOKEN,
    _In_ NDIS_STATUS OperationStatus )
{
    NDIS_STATUS status = OperationStatus;

    TraceActEntry();

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Set FT Reassociation parameters FAILed.");
        goto exit;
    }

    WFCActInfo("FT Reassociation parameters completed Successfully.");

exit:
    TraceActExit(status);
}


void CSetFTReassociationParametersJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus )
{
    TraceActEntry();

    TraceActExit(NdisStatus);
}

