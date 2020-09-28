/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

BSSList.cpp

Abstract:

Implementation for BSS list manager

Environment:

Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "wcnsimplewfdparser.h"
#include "BSSList.tmh"


void tmpTraceWfdIE_DbgInstrument(
    _In_reads_bytes_(lengthTmp) BYTE *traceTMP,
    ULONG lengthTmp,
    _In_z_ const char *BaconOrProbe)
{
    for (; lengthTmp > 0;)
    {
#define WFD_IE_SIZE_OF_TYPE   ( sizeof(UCHAR) )
#define WFD_IE_SIZE_OF_LENGTH ( sizeof(UCHAR) )
#define WFD_IE_SIZE_OF_OUI    ( 4 * sizeof(UCHAR) )

        if (lengthTmp < (WFD_IE_SIZE_OF_TYPE + WFD_IE_SIZE_OF_LENGTH))
        {
            WFCInfo("[CBSSEntry][device][%s][ie] Invalid WFD IE %02x\n", BaconOrProbe, traceTMP[0]);
            break;
        }

        UCHAR ieType = traceTMP[0];
        UCHAR ieLength = traceTMP[1];

        if (ieLength >(lengthTmp - (WFD_IE_SIZE_OF_TYPE + WFD_IE_SIZE_OF_LENGTH)))
        {
            WFCInfo("[CBSSEntry][device][%s][ie] Invalid WFD IE length %02x\n", BaconOrProbe, ieLength);
            break;
        }

        WFCInfo("[CBSSEntry][device][%s][ie] ElemID %02x\n", BaconOrProbe, ieType);
        WFCInfo("[CBSSEntry][device][%s] Length %02x\n", BaconOrProbe, ieLength);
        if (ieLength >= WFD_IE_SIZE_OF_OUI)
        {
            WFCInfo("[CBSSEntry][device][%s] OUI %02x%02x%02x%02x\n", BaconOrProbe, traceTMP[2], traceTMP[3], traceTMP[4], traceTMP[5]);

            BYTE *IEAttrb = traceTMP + 6;
            WFCInfo("[CBSSEntry][device][%s] IEAttrb (Length - 4 = %u)\n", BaconOrProbe, ieLength - 4);
            for (int i = ieLength - 4; i > 0;)
            {
                if (i >= 8)
                {
                    WFCInfo("[CBSSEntry][device][%s] OUI %02x %02x %02x %02x %02x %02x %02x %02x\n", BaconOrProbe,
                        IEAttrb[0], IEAttrb[1], IEAttrb[2], IEAttrb[3], IEAttrb[4], IEAttrb[5], IEAttrb[6], IEAttrb[7]);
                    i -= 8;
                    IEAttrb += 8;
                }
                else
                {
                    switch (i)
                    {
                    case 7:
                        WFCInfo("[CBSSEntry][device][%s] OUI %02x %02x %02x %02x %02x %02x %02x", BaconOrProbe,
                            IEAttrb[0], IEAttrb[1], IEAttrb[2], IEAttrb[3], IEAttrb[4], IEAttrb[5], IEAttrb[6]);
                        break;
                    case 6:
                        WFCInfo("[CBSSEntry][device][%s] OUI %02x %02x %02x %02x %02x %02x", BaconOrProbe,
                            IEAttrb[0], IEAttrb[1], IEAttrb[2], IEAttrb[3], IEAttrb[4], IEAttrb[5]);
                        break;
                    case 5:
                        WFCInfo("[CBSSEntry][device][%s] OUI %02x %02x %02x %02x %02x", BaconOrProbe,
                            IEAttrb[0], IEAttrb[1], IEAttrb[2], IEAttrb[3], IEAttrb[4]);
                        break;
                    case 4:
                        WFCInfo("[CBSSEntry][device][%s] OUI %02x %02x %02x %02x", BaconOrProbe,
                            IEAttrb[0], IEAttrb[1], IEAttrb[2], IEAttrb[3]);
                        break;
                    case 3:
                        WFCInfo("[CBSSEntry][device][%s] OUI %02x %02x %02x", BaconOrProbe,
                            IEAttrb[0], IEAttrb[1], IEAttrb[2]);
                        break;
                    case 2:
                        WFCInfo("[CBSSEntry][device][%s] OUI %02x %02x", BaconOrProbe,
                            IEAttrb[0], IEAttrb[1]);
                        break;
                    case 1:
                        WFCInfo("[CBSSEntry][device][%s] OUI %02x", BaconOrProbe,
                            IEAttrb[0]);
                        break;
                    }
                    break;
                }
            }
        }

        ULONG uOffsetToNextIE = ieLength + WFD_IE_SIZE_OF_TYPE + WFD_IE_SIZE_OF_LENGTH;
        if (lengthTmp == uOffsetToNextIE)
        {
            lengthTmp = 0;
            uOffsetToNextIE = 0;
        }
        else
        {
            traceTMP = traceTMP + uOffsetToNextIE;
            lengthTmp = lengthTmp - uOffsetToNextIE;
        }
    }
}

CBSSEntry::CBSSEntry(
    _In_ const DOT11_MAC_ADDRESS& bssId) :
    m_ProbeResponseTime(0),
    m_BeaconTime(0),
    m_IsHESSIDPresent(false),
    m_pBeaconWfdServiceNames(NULL),
    m_pProbeWfdServiceNames(NULL),
    m_BSSEntryDiscoveryContext(0),
    m_IsWFDServiceUpdated(false),
    m_IsNeighborReportSupported(false),
    m_IsAPChannelReportSupported(false),
    m_IsSuggestedByAPBSSTransitionRequest(false),
    m_AdvertisesCorrectFTAKMSuites(false),
    m_AdvertisesOnlyFTAKMSuite(false)
    /*++
    Routine Description:

    Arguments:

    Return Value:
    --*/
{
    RtlZeroMemory(&m_SignalInfo, sizeof(m_SignalInfo));
    RtlZeroMemory(&m_ProbeResponseFrameBody, sizeof(m_ProbeResponseFrameBody));
    RtlZeroMemory(&m_BeaconFrameBody, sizeof(m_BeaconFrameBody));
    RtlZeroMemory(&m_DeviceSpecificContext, sizeof(m_DeviceSpecificContext));
    RtlZeroMemory(&m_BssChannelInfo, sizeof(m_BssChannelInfo));
    RtlZeroMemory(&m_MobilityDomainIEBody, sizeof(m_MobilityDomainIEBody));
    RtlZeroMemory(&m_HESSID, sizeof(m_HESSID));

    RtlCopyMemory(m_BssID, bssId, sizeof(DOT11_MAC_ADDRESS));
    m_MaxValidityTime = (ULONG)-1;

    m_pNeighborReportInfoElement = NULL;

    m_CachedChannelNumber = 0;
    m_CachedBandID = WDI_BAND_ID_ANY;

    m_CachedDot11PhyID = DOT11_PHY_ID_ANY;
    m_CachedChannelCenterFrequency = 0;
    m_CachedRank = 0;
    m_CachedRawRank = 0;

    RtlZeroMemory(&m_GroupId, sizeof(m_GroupId));
    RtlZeroMemory(&m_GroupCaps, sizeof(m_GroupCaps));
    m_IsSelectedRegistrar = false;

    m_BlockedInfo.Initialize();

    InitializeCppListEntry(
        this,
        &m_BssListEntry);
}

CBSSEntry::~CBSSEntry()
/*++
Routine Description:

Arguments:

Return Value:
--*/
{
    if (m_ProbeResponseFrameBody.pBuffer != NULL)
    {
        delete[] m_ProbeResponseFrameBody.pBuffer;
    }
    if (m_BeaconFrameBody.pBuffer != NULL)
    {
        delete[] m_BeaconFrameBody.pBuffer;
    }
    if (m_DeviceSpecificContext.pBuffer != NULL)
    {
        delete[] m_DeviceSpecificContext.pBuffer;
    }
    if (m_pNeighborReportInfoElement)
    {
        delete m_pNeighborReportInfoElement;
    }
    if (m_pBeaconWfdServiceNames)
    {
        delete[] m_pBeaconWfdServiceNames;
    }
    if (m_pProbeWfdServiceNames)
    {
        delete[] m_pProbeWfdServiceNames;
    }
    if (m_MobilityDomainIEBody.pBuffer)
    {
        delete[] m_MobilityDomainIEBody.pBuffer;
    }
}

PLIST_ENTRY CBSSEntry::GetListEntry()
/*++
Routine Description:

Arguments:

Return Value:
--*/
{
    return &m_BssListEntry.ListEntry;
}

CBSSEntry *CBSSEntry::GetBSSEntryFromListEntry(
    _In_ LIST_ENTRY *pBssListEntry)
    /*++
    Routine Description:

    Arguments:

    Return Value:
    --*/
{
    return (CBSSEntry*)GetCppObjectFromListEntry(
        pBssListEntry);
}

BOOLEAN CBSSEntry::IsBssIdEqual(
    _In_ const DOT11_MAC_ADDRESS& BssId)
    /*++
    Routine Description:

    Arguments:

    Return Value:
    --*/
{
    return 0 == memcmp(BssId, m_BssID, sizeof(DOT11_MAC_ADDRESS));
}

BOOLEAN CBSSEntry::IsMatchingBssEntryForConnect(
    _In_ BSS_ENTRY_CONNECT_MATCHING_CRITERIA* pMatchingCriteria,
    _In_ BOOLEAN bIsFTConnection)
{
    BOOLEAN matches = TRUE;
    DOT11_SSID mySsid;
    ULONG i;

    if (pMatchingCriteria == NULL ||
        pMatchingCriteria->pConnectionParameters == NULL)
    {
        matches = FALSE;
        goto exit;
    }

    // Reset the selection flags. This can be modified from now
    // at any time
    m_BssSelectionFlags = 0;

    //
    // Parse out necessary data
    //
    GetSSID(mySsid);
    WFCTrace("Evaluating Candidate BSSID = %!MACADDR! SSID = %!DOT11SSID! RSSI = %d LinkQuality = %d Channel = %d",
        m_BssID, &mySsid, m_SignalInfo.Rssi, m_SignalInfo.LinkQuality, m_BssChannelInfo.Channel);

    if (!pMatchingCriteria->bIncludeCurrentPeer && GetAssociatedFlag())
    {
        WFCInfo("Currently associated BSS entry with BSSID %!MACADDR!, is not included as a candidate", m_BssID);
        matches = FALSE;
        goto exit;
    }

    if ((m_ProbeResponseTime < pMatchingCriteria->MaxAgeOfEntry) &&
        (m_BeaconTime < pMatchingCriteria->MaxAgeOfEntry))
    {
        // Skip old entry unless its something we have been asked to include
        if ((pMatchingCriteria->bIncludeCurrentPeer) &&
            GetAssociatedFlag())
        {
            WFCInfo("Currently associated BSS entry too old BSSID %!MACADDR!, but including as candidate", m_BssID);
            // This is an expired AP.  The penalty is applied in calculate rank.
        }
        else
        {
            WFCTrace(
                "Entry too old BSSID %!MACADDR!, ignoring candidate.", m_BssID);                
            matches = FALSE;
            goto exit;
        }            
    }

    //
    // Data is parsed. From now on this isn't a match unless we say it
    // is.
    //
    matches = FALSE;

    WFC_CONNECTION_PROFILE_PARAMETERS* pConnectParams =
        pMatchingCriteria->pConnectionParameters;

    for (i = 0; i < pConnectParams->SSIDList.ElementCount; ++i)
    {
        PDOT11_SSID pCurrentSSID = &pConnectParams->SSIDList.pBuffer[i];
        if (pCurrentSSID->uSSIDLength == mySsid.uSSIDLength &&
            memcmp(
            pCurrentSSID->ucSSID,
            mySsid.ucSSID,
            mySsid.uSSIDLength) == 0)
        {
            matches = TRUE;
            break;
        }
    }

    if (!matches)
    {
        goto exit;
    }

    //
    // If the connection requires an HESSID match, search for it
    //
    if (pConnectParams->HESSIDList.ElementCount)
    {
        matches = false;
        for (i = 0; i < pConnectParams->HESSIDList.ElementCount; ++i)
        {
            PDOT11_HESSID pHESSID = &pConnectParams->HESSIDList.pBuffer[i];
            if (0 == memcmp (&m_HESSID, pHESSID, sizeof(DOT11_HESSID)))
            {
                matches = true;
                break;
            }
        }
    }

    if (!matches)
    {
        goto exit;
    }

    //
    // BUGBUG: OS does not provide a correct group ID during connect
    // in OID_DOT11_WFD_DESIRED_GROUP_ID. Ignoring device address
    // allows match to happen.
    //
    if (pMatchingCriteria->pGroupIdMatch != NULL)
    {
        matches = IsGroupIdMatch(
            *(pMatchingCriteria->pGroupIdMatch),
            false,                                  // Do not match DeviceAddress
            true);                                  // Match SSID only
    }

    if (!matches)
    {
        goto exit;
    }

    if (pMatchingCriteria->pRequireSelectedRegistrar &&
        *pMatchingCriteria->pRequireSelectedRegistrar)
    {
        matches = GetSelectedRegistrarState();

        WFCTrace("Selected registrar matches? %d", matches);
    }

    if (!matches)
    {
        goto exit;
    }

    if (pMatchingCriteria->pRequireGroupFormation &&
        *pMatchingCriteria->pRequireGroupFormation)
    {
        matches = GetGroupCapability()->bGroupFormationEnabled ? true : false;

        WFCTrace("Group Formation matches? %d", matches);
    }

    if (!matches)
    {
        goto exit;
    }


    //
    // TODO: Match auth-cipher, rate
    //

    //
    // Now it's a match. Need to check policies to exclude this entry
    // if necessary.
    //

    // Check the excluded MAC address list
    if (pConnectParams->IgnoreAllMACAddresses)
    {
        WFCInfo(
            "All MAC addresses excluded. Ignoring candidate BSSID %!MACADDR!", m_BssID);
        matches = FALSE;
        goto exit;
    }
    else
    {
        PDOT11_MAC_ADDRESS pMacAddressList;

        matches = TRUE;     // Reject if found in list     

        // Walk through the excluded MAC address list
        pMacAddressList = pConnectParams->ExcludedMacAddressList.pBuffer;

        for (i = 0; i < pConnectParams->ExcludedMacAddressList.ElementCount; i++)
        {
            if (IsBssIdEqual(pMacAddressList[i]) == TRUE)
            {
                matches = FALSE;
                break;
            }
        }

        if (matches == FALSE)
        {
            WFCInfo(
                "MAC addresses excluded. Ignoring candidate BSSID %!MACADDR!", m_BssID);
            goto exit;
        }
    }

    // Check the desired BSSID list
    if (!pConnectParams->AcceptAnyBSSID)
    {
        PDOT11_MAC_ADDRESS pMacAddressList;

        matches = FALSE; // Only accept if found in list

        // Walk through the desired BSSID list
        pMacAddressList = pConnectParams->BSSIDList.pBuffer;

        for (i = 0; i < pConnectParams->BSSIDList.ElementCount; i++)
        {
            if (IsBssIdEqual(pMacAddressList[i]) == TRUE)
            {
                matches = TRUE;
                break;
            }
        }

        if (matches == FALSE)
        {
            WFCInfo(
                "MAC addresses not in BSSID list. Ignoring candidate BSSID %!MACADDR!", m_BssID);
            goto exit;
        }
    }

    if (bIsFTConnection)
    {
        if (!IsFastBSSTransitionSupported())
        {
            WFCInfo("[11r] Candidate does not advertise MDIE. Not including it.");
            matches = FALSE;
        }
        if (!IsCorrectFTAKMSuitesAdvertised())
        {
            WFCInfo("[11r] Candidate does not advertise Correct FT AKM suites. Not including it.");
            matches = FALSE;
        }
    }

exit:
    return matches;
}



NDIS_STATUS CBSSEntry::UpdateVariableBuffers(
    _In_reads_bytes_opt_(bufferLength) BYTE* pBuffer,
    _In_ UINT16 bufferLength,
    _Inout_ PVARIABLE_LENGTH_BUFFER_UINT8 pVariableLengthBuffer)
    /*++
    Routine Description:
    if the exisiting buffer is small then realocates variable sized buffers in bss entry.
    otherwise just copy new data into the existing buffer and updates elemnt count parameter
    Arguments:

    Return Value:

    --*/
{
    NDIS_STATUS  ndisStatus = NDIS_STATUS_SUCCESS;
    UINT8 *pTempBuffer;

    TraceEntry();


    if (bufferLength == 0)
    {
        pVariableLengthBuffer->dataLength = 0;
        goto exit;
    }

    if (pVariableLengthBuffer->pBuffer == NULL ||
        pVariableLengthBuffer->bufferCapacity < bufferLength)
    {
        pTempBuffer = new UINT8[bufferLength];
        if (pTempBuffer == NULL)
        {
            ndisStatus = NDIS_STATUS_RESOURCES;
            goto exit;
        }

        if (pVariableLengthBuffer->pBuffer != NULL)
        {
            delete[] pVariableLengthBuffer->pBuffer;
        }

        pVariableLengthBuffer->pBuffer = pTempBuffer;
        pVariableLengthBuffer->bufferCapacity = bufferLength;
    }

    RtlCopyMemory(pVariableLengthBuffer->pBuffer, pBuffer, bufferLength);

    pVariableLengthBuffer->dataLength = bufferLength;

exit:

    TraceExit(ndisStatus);

    return ndisStatus;
}

NDIS_STATUS CBSSEntry::SaveBeaconOrProbeResponse(
    _In_reads_bytes_opt_(bufferLength) BYTE* pBuffer,
    _In_ UINT16 bufferLength,
    _Inout_ PVARIABLE_LENGTH_BUFFER_UINT8 pVariableLengthBuffer
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG               ieBlobSize = 0;

    if (bufferLength < FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements))
    {
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        goto Exit;
    }

    // Validate that IE blob is good. We dont save it unless its good.
    // As part of this process, we also get the real length of IE blob
    ndisStatus = Dot11GetIEsSizeFromIEBlob(
        pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements),
        bufferLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements),
        &ieBlobSize
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        goto Exit;
    }

    ndisStatus = UpdateVariableBuffers(
        pBuffer,
        (UINT16)(ieBlobSize + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements)),
        pVariableLengthBuffer);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        goto Exit;
    }

Exit:
    return ndisStatus;
}

void CBSSEntry::Block(WFC_BLOCKED_REASON reason)
{
    // Set the blocked reason flag
    m_BlockedInfo.BlockedReasons |= reason;
    WFCInfo("[BlockedList] Blocking BSSID = %!MACADDR! due to reason 0x%x. NewReasons = 0x%x", 
        m_BssID,reason, m_BlockedInfo.BlockedReasons);

    switch (reason)
    {
        case WFC_BLOCKED_REASON_ASSOC_REJECTED_OR_PEER_DISCONNECTED:
            m_BlockedInfo.LastAssocFailTime = CSystem::get_CurrentTime();
            break;
        case WFC_BLOCKED_REASON_ASSOC_OR_DISASSOC_BSS_UNREACHABLE:
            m_BlockedInfo.LastAssocFailLinkQuality = m_SignalInfo.LinkQuality;
            break;
    }
}

void CBSSEntry::UnBlock(ULONG blockedReasons)
{
    // Clears the reasons for the blocked
    m_BlockedInfo.BlockedReasons &= ~(blockedReasons);
    WFCInfo("[BlockedList] Unblocking BSSID = %!MACADDR! for reasons 0x%x. NewReasons = 0x%x", 
        m_BssID, blockedReasons, m_BlockedInfo.BlockedReasons);

    if (blockedReasons & WFC_BLOCKED_REASON_ASSOC_REJECTED_OR_PEER_DISCONNECTED)
    {
        m_BlockedInfo.LastAssocFailTime = 0;
    }
    if (blockedReasons & WFC_BLOCKED_REASON_ASSOC_OR_DISASSOC_BSS_UNREACHABLE)
    {
        m_BlockedInfo.LastAssocFailLinkQuality = 0;
    }
}

ULONG CBSSEntry::CalculateRank(
    _In_ BSS_ENTRY_CONNECT_MATCHING_CRITERIA* pMatchingCriteria,
    _In_ PROAM_CONTROL_PARAMETERS pRoamControl
    )
{
    ULONG rank = m_SignalInfo.LinkQuality; // Initially just link quality
    ULONG rankAdjustFactorTimes1000 = 1000; // This is always divided by 1000

    //only boost 5GHz APs with link quality >= FiveGHzBoostLinkQualityStart
    if (m_BssChannelInfo.BandId == WDI_BAND_ID_5000 && m_SignalInfo.LinkQuality >= pRoamControl->FiveGHzBoostLinkQualityStart)
    {
        // Boost the rank for 5GHz AP

        // The boost needs to be bigger when signal quality is good (so that we prefer 5GHz AP more often) and
        // lower when signal quality is poor. For this logic to work, we depend on the currently reported
        // link quality to determine what the boost needs to be. The boost is determined by linear interpolation between
        // a boost of boost_low at lq_low and a boost of 100-lq_high at lq_high (making lq_high the point at which a 5GHz AP
        // will be either ranked the same as or better than all 2.4GHz APs.
        UINT32 lq_low = pRoamControl->FiveGHzBoostLinkQualityStart;
        UINT32 lq_high = pRoamControl->FiveGHzBoostLinkQualityEnd;


        UINT32 boost_high = 100 - lq_high;
        UINT32 boost_low = pRoamControl->FiveGHzLowBoost;

        LONG boost = ((boost_high - boost_low) * m_SignalInfo.LinkQuality + (lq_high * boost_low - lq_low * boost_high)) / (lq_high - lq_low);

        WFCTrace("[Roam][Boost] BSSID = %!MACADDR! has link quality %lu is boosted by %ld.", m_BssID, m_SignalInfo.LinkQuality, boost);

        rank += boost;
    }

    //Cache the raw rank.
    // this is before any reductions/penalties are applied.
    m_CachedRawRank = rank;

    NT_ASSERT(pRoamControl);
    
    // Check blocked state to see if we should fix rank of this AP
    if (TestRoamConfig(pMatchingCriteria->roamConfigFlags, RC_CONNECTED_ROAM))
    {
        // For better AP roaming, we are more careful about picking new APs        
        if (m_BlockedInfo.BlockedReasons & WFC_BLOCKED_REASON_ASSOC_REJECTED_OR_PEER_DISCONNECTED)
        {
            ULONGLONG minUnblockedTime;
            
            minUnblockedTime = CSystem::get_CurrentTime() -
                (((ULONGLONG)pRoamControl->BetterAPRoamReattemptFailedBlockTimeSeconds) * 10000000);

            // If last association failure was recent lower rank significantly, else consider it OK
            if (minUnblockedTime < m_BlockedInfo.LastAssocFailTime)
            {
                // Has recently been blocked, dont consider him for a while
                WFCTrace("[Roam][BetterAP][BlockedList] BSSID = %!MACADDR! was blocked recently due to disconnect", m_BssID);
                rankAdjustFactorTimes1000 = MIN(rankAdjustFactorTimes1000, WFC_CONSTANT::RankAdjustment_BetterAP_RemoteDisconnect_Recent);
                
                SET_FLAG(m_BssSelectionFlags, WDI_BSS_SELECTION_RECENT_ASSOCIATION_ERROR);
            }
            else
            {
                // If its been a while, only do a partial hit to this AP
                WFCTrace("[Roam][BetterAP][BlockedList] BSSID = %!MACADDR! was blocked earlier due to disconnect", m_BssID);
                rankAdjustFactorTimes1000 = MIN(rankAdjustFactorTimes1000, WFC_CONSTANT::RankAdjustment_BetterAP_RemoteDisconnect_Old);
            }
        }

        if (m_BlockedInfo.BlockedReasons & WFC_BLOCKED_REASON_ASSOC_OR_DISASSOC_BSS_UNREACHABLE)
        {
            if (m_SignalInfo.LinkQuality <
                        (m_BlockedInfo.LastAssocFailLinkQuality + pRoamControl->APLinkQualitySignificantChangeBar))
            {
                // Signal strength hasn't increased since we last tried, dont consider this a great candidate
                rankAdjustFactorTimes1000 = MIN(rankAdjustFactorTimes1000, WFC_CONSTANT::RankAdjustment_BetterAP_Unreachable_NoLQChange);
                WFCTrace("[Connect][BlockedList] BSSID = %!MACADDR! signal strength has NOT increased since blocking", m_BssID);

                SET_FLAG(m_BssSelectionFlags, WDI_BSS_SELECTION_RECENT_ASSOCIATION_ERROR);                
            }
        }
    }
    else // reconnect or connect (association loss/resume from DX or first time connect)
    {
        if (m_BlockedInfo.BlockedReasons & WFC_BLOCKED_REASON_ASSOC_REJECTED_OR_PEER_DISCONNECTED)
        {
            ULONGLONG minUnblockedTime;
            
            minUnblockedTime = CSystem::get_CurrentTime() -
                (((ULONGLONG)pRoamControl->BetterAPRoamReattemptFailedBlockTimeSeconds) * 10000000);

            // If last association failure was recent lower rank slightly, else consider it OK
            if (minUnblockedTime < m_BlockedInfo.LastAssocFailTime)
            {
                // Has recently been blocked, dont give this too much preference
                WFCTrace("[Connect][BlockedList] BSSID = %!MACADDR! was blocked recently due to disconnect", m_BssID);
                rankAdjustFactorTimes1000 = MIN(rankAdjustFactorTimes1000, WFC_CONSTANT::RankAdjustment_Connect_RemoteDisconnect_Recent);

                SET_FLAG(m_BssSelectionFlags, WDI_BSS_SELECTION_RECENT_ASSOCIATION_ERROR);                
            }
        }        
    }

    // Apply the adjustment factor to get real rank
    rank = (rank * rankAdjustFactorTimes1000) / 1000 ;

    // Set this flag for every that has a non-zero rank. Caller will clear any it doesnt like
    if (rank > 0)
    {
        SET_FLAG(m_BssSelectionFlags, WDI_BSS_SELECTION_HOST_PREFERRED);
    }

    if (m_IsSuggestedByAPBSSTransitionRequest)
    {
        SET_FLAG(m_BssSelectionFlags, WDI_BSS_SELECTION_FLAGS_AP_REQUESTED_TRANSITION);
    }

    if ((GetAssociatedFlag())                                    &&
        (m_ProbeResponseTime < pMatchingCriteria->MaxAgeOfEntry) &&
        (m_BeaconTime < pMatchingCriteria->MaxAgeOfEntry)           )
    {
        //If the currently associated AP is too old give a rank of 0
        // this does not affect the raw rank cached earlier.
        rank = 0;
    }
    // Cache the rank
    m_CachedRank = rank;
    
    return rank;
}


//
// This function inspects beacon or probe response when they change,
// and sets appropriate P2P metadata about the device (if it exists).
//
void CBSSEntry::UpdateP2PFields(
    _In_ WFC_BSS_ENTRY_FRAME_TYPE sourceType,
    _In_ ULONG IEsLength,
    _In_bytecount_(IEsLength) BYTE* pIEBlob,
    _In_opt_ DOT11_MAC_ADDRESS *pWfdDeviceAddress)
{
    DOT11_WFD_GROUP_ID newGroupID = { 0 };
    bool bReset = true;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    WCN_PARSED_MSG selectedRegistrarInfo = { 0 };
    USHORT usOTAServiceInfoLength = 0;
    USHORT usServiceInfoCount = 0;
    DWORD dwBufferLength;
    PBSS_ENTRY_WFD_SERVICES_INFO pWfdServicesInfo = NULL;
    PBSS_ENTRY_WFD_SERVICES_INFO pOldWfdServiceNames = NULL;

    if ((!pIEBlob) ||
        (!IEsLength) ||
        (!pWfdDeviceAddress))
    {
        goto exit;
    }

    RtlCopyMemory( &newGroupID.DeviceAddress, pWfdDeviceAddress, sizeof(newGroupID.DeviceAddress) );

    if (WfdParseWpsIe(
        pIEBlob,
        IEsLength,
        MESSAGE_TYPE_PROBE_RESPONSE,
        &selectedRegistrarInfo) &&
        (selectedRegistrarInfo.AvailableFieldsMask &
        WCN_FIELD_SELECTED_REGISTRAR) == WCN_FIELD_SELECTED_REGISTRAR)
    {
        WFCInfo("Found selected registrar bit set on AP.");
        m_IsSelectedRegistrar = selectedRegistrarInfo.Message.ProbeResponse.SelectedRegistrar ? true : false;
    }
    else
    {
        m_IsSelectedRegistrar = false;
    }

    ntStatus = WFDGetAdvertisedServiceInfoSizeAndCount(
                    WFA_P2P_OUI,
                    pIEBlob,
                    IEsLength,
                    &usOTAServiceInfoLength,
                    &usServiceInfoCount
                    );
    if ((NT_SUCCESS(ntStatus)) &&
        (usServiceInfoCount))
    {
        WFCInfo("[CBSSEntry][device] Found %d WFDS Services, TotalLength = %d\n",
            usServiceInfoCount, usOTAServiceInfoLength);

        dwBufferLength = FIELD_OFFSET(BSS_ENTRY_WFD_SERVICES_INFO,ServiceInfo) +
                         (usServiceInfoCount * WFD_DEVICE_DESCRIPTOR_GET_SERVICE_INFO_FIXED_SIZE) +
                         usOTAServiceInfoLength;

        pWfdServicesInfo = (PBSS_ENTRY_WFD_SERVICES_INFO) new UINT8[dwBufferLength];
        if (pWfdServicesInfo)
        {
            PWFD_OTA_ADVERTISED_SERVICE_INFO pServiceInfo;

            RtlZeroMemory (pWfdServicesInfo, dwBufferLength);
            pWfdServicesInfo->usServicesDescriptorOffset = FIELD_OFFSET(BSS_ENTRY_WFD_SERVICES_INFO,ServiceInfo);
                                                           (usServiceInfoCount * sizeof(DOT11_WFD_SERVICE_HASH));
            pServiceInfo = (PWFD_OTA_ADVERTISED_SERVICE_INFO) (((PBYTE) pWfdServicesInfo) +
                                                               pWfdServicesInfo->usServicesDescriptorOffset);

            ntStatus = WFDGetAdvertisedServiceInfo(
                            WFA_P2P_OUI,
                            pIEBlob,
                            IEsLength,
                            &usOTAServiceInfoLength,
                            pServiceInfo);
            if ( NT_SUCCESS(ntStatus) )
            {
                USHORT usNumServiceNames = 0, usEntryLength, usBytesRemaining = usOTAServiceInfoLength;
                PWFD_OTA_ADVERTISED_SERVICE_DESCRIPTOR pOTAService = pServiceInfo->AdvertisedServiceDescriptors;

                //
                // Validate the number of entries
                //
                while ((usNumServiceNames < usServiceInfoCount) &&
                       (usBytesRemaining > WFD_OTA_ADVERTISED_SERVICE_DESCRIPTOR_FIXED_SIZE))
                {
                    usEntryLength = WFD_OTA_ADVERTISED_SERVICE_DESCRIPTOR_FIXED_SIZE + pOTAService->ServiceNameLength;
                    if ((pOTAService->ServiceNameLength > DOT11_WFD_SERVICE_NAME_MAX_LENGTH) ||
                        (usEntryLength > usBytesRemaining))
                    {
                        WFCInfo("\t[%d] ERROR - ServiceNameLength=%d, EntryLength=%d > usBytesRemaining=%d\n",
                            usNumServiceNames, pOTAService->ServiceNameLength, usEntryLength, usBytesRemaining);
                        break;
                    }

                    usNumServiceNames++;
                    WFCTrace("\t[%d] AdvertisementID=0x%x, ConfigMethods=0x%x, ServiceNameLength=<%d>, Name=<%s>\n",
                        usNumServiceNames, pOTAService->AdvertisementID,
                        pOTAService->ConfigMethods, pOTAService->ServiceNameLength, (PCHAR) pOTAService->ServiceName);

                    // Go to next OTA descriptor
                    pOTAService = (PWFD_OTA_ADVERTISED_SERVICE_DESCRIPTOR) (((PBYTE)pOTAService) + usEntryLength);
                    usBytesRemaining -= usEntryLength;
                }

                NT_ASSERT (usServiceInfoCount == usNumServiceNames);
                pWfdServicesInfo->usNumServices = usNumServiceNames;    // Use usNumServiceNames instead of usServiceInfoCount for safety
                pWfdServicesInfo->usServiceNamesDescriptorLength = usOTAServiceInfoLength;
            }
            else
            {
                WFCInfo ("ERROR - WFDGetAdvertisedServiceInfo FAILed <0x%x>, Length:Count=<%d:%d>\n",
                    ntStatus, usOTAServiceInfoLength, usServiceInfoCount);

                delete[] pWfdServicesInfo;
                pWfdServicesInfo = NULL;
            }
        }
        else
        {
            WFCInfo ("FAILed to allocate <%d * %d> + %d = %d bytes\n",
                usServiceInfoCount, WFD_DEVICE_DESCRIPTOR_GET_SERVICE_INFO_FIXED_SIZE, usOTAServiceInfoLength,
                dwBufferLength);
        }
    }

    if (pWfdServicesInfo)
    {
        if (sourceType == WFC_BSS_ENTRY_BEACON_FRAME)
        {
            pOldWfdServiceNames = m_pBeaconWfdServiceNames;
            m_pBeaconWfdServiceNames = pWfdServicesInfo;
        }
        else
        {
            pOldWfdServiceNames = m_pProbeWfdServiceNames;
            m_pProbeWfdServiceNames = pWfdServicesInfo;
        }
    }

    if (!m_IsWFDServiceUpdated)
    {
        BOOL fMatched = TRUE;

        if (pWfdServicesInfo == pOldWfdServiceNames)        // Either both NULL, or same pointer
        {
            fMatched = TRUE;
        }
        else if ((!pWfdServicesInfo) ||                     // Any one is NULL
                 (!pOldWfdServiceNames))
        {
            fMatched = FALSE;
        }
        else
        {
            ntStatus = WFDCompareAdvertisedServiceInfo(
                    pWfdServicesInfo->usServiceNamesDescriptorLength,
                    (PWFD_OTA_ADVERTISED_SERVICE_INFO) (((PBYTE) pWfdServicesInfo) +
                                                        pWfdServicesInfo->usServicesDescriptorOffset),
                    pOldWfdServiceNames->usServiceNamesDescriptorLength,
                    (PWFD_OTA_ADVERTISED_SERVICE_INFO) (((PBYTE) pOldWfdServiceNames) +
                                                        pOldWfdServiceNames->usServicesDescriptorOffset),
                    &fMatched);
        }

        if ((STATUS_SUCCESS == ntStatus) &&
            (!fMatched))
        {
            m_IsWFDServiceUpdated = true;
        }
    }
    pWfdServicesInfo = NULL;            // So that it does not get deleted below

    if (Dot11CopySSIDFromIEBlob(
        pIEBlob,
        IEsLength,
        &newGroupID.SSID) != NDIS_STATUS_SUCCESS)
    {
        goto exit;
    }

    if (!NT_SUCCESS(
        WFDGetCapability(
        pIEBlob,
        IEsLength,
        NULL,
        &m_GroupCaps)))
    {
        goto exit;
    }

    bReset = false;

exit:
    if (bReset)
    {
        RtlZeroMemory(&newGroupID, sizeof(newGroupID));
    }
    if (pWfdServicesInfo)
    {
        delete[] pWfdServicesInfo;
    }
    if (pOldWfdServiceNames)
    {
        delete[] pOldWfdServiceNames;
    }

    SetGroupID(&newGroupID);
}

void CBSSEntry::GetIEBlobFromBeaconOrProbeReponse(
    _In_ WFC_BSS_ENTRY_FRAME_TYPE sourceType,
    _Outptr_result_buffer_(*pBufferLength)PBYTE *pIEBuffer,
    _Out_ ULONG *pBufferLength
    )
{
    if (sourceType == WFC_BSS_ENTRY_BEACON_FRAME &&
        m_BeaconFrameBody.dataLength > FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements))
    {
        *pIEBuffer = m_BeaconFrameBody.pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
        *pBufferLength = m_BeaconFrameBody.dataLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
    }
    else if (sourceType == WFC_BSS_ENTRY_PROBE_RESPONSE_FRAME &&
        m_ProbeResponseFrameBody.dataLength > FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements))
    {
        *pIEBuffer = m_ProbeResponseFrameBody.pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
        *pBufferLength = m_ProbeResponseFrameBody.dataLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
    }
    else
    {
        *pIEBuffer = NULL;
        *pBufferLength = 0;
    }
}

void CBSSEntry::UpdateFastBSSTransitionCapabilities(
    WFC_BSS_ENTRY_FRAME_TYPE sourceType)
{
    BYTE *pIEBlob = NULL;
    ULONG IEsLength = 0;
    MD_IE_INFO MDIEInfo;
    UCHAR MDIELength;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    GetIEBlobFromBeaconOrProbeReponse(sourceType, &pIEBlob, &IEsLength);
    if (pIEBlob == NULL || IEsLength == 0)
    {
        goto exit;
    }

    ndisStatus = Dot11GetInfoElementFromIEBlob(
        pIEBlob,
        IEsLength,
        DOT11_INFO_ELEMENT_ID_MOBILITY_DOMAIN,
        &MDIELength,
        (PVOID*)&pIEBlob);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        goto exit;
    }

    ndisStatus = Dot11ParseMDIE(
        pIEBlob,
        MDIELength,
        &MDIEInfo);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCTrace("Found MD IE but couldn't parse it.");
        goto exit;
    }

    //
    // Save full IE blob. 
    // This will be passed back to the AP if 802.11r fast BSS transition is supported.
    //
    ndisStatus = UpdateVariableBuffers(
        (BYTE*)&MDIEInfo,
        MDIELength,
        &m_MobilityDomainIEBody);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        goto exit;
    }

exit:
    TraceExit(ndisStatus);
}

void CBSSEntry::UpdateRMEnabledCapabilities(
    WFC_BSS_ENTRY_FRAME_TYPE sourceType)
{
    BYTE *pIEBlob = NULL;
    ULONG IEsLength = 0;
    RM_IE_INFO RMIEInfo;
    UCHAR RMIELength;
    bool bIsNeighborReportSupported = false;
    bool bIsAPChannelReportSupported = false;

    GetIEBlobFromBeaconOrProbeReponse(sourceType, &pIEBlob, &IEsLength);
    if (pIEBlob == NULL || IEsLength == 0)
    {
        goto exit;
    }

    if (Dot11GetInfoElementFromIEBlob(
        pIEBlob,
        IEsLength,
        DOT11_INFO_ELEMENT_ID_RM,
        &RMIELength,
        (PVOID*)&pIEBlob) != NDIS_STATUS_SUCCESS)
    {
        goto exit;
    }

    if (Dot11ParseRMIE(
        pIEBlob,
        RMIELength,
        &RMIEInfo) != NDIS_STATUS_SUCCESS)
    {
        WFCTrace("Found RM IE but couldn't parse it.");
        goto exit;
    }

    bIsNeighborReportSupported = RMIEInfo.NeighborReport;
    bIsAPChannelReportSupported = RMIEInfo.APChannelReport;

exit:
    m_IsNeighborReportSupported = bIsNeighborReportSupported;
    m_IsAPChannelReportSupported = bIsAPChannelReportSupported;
}

void CBSSEntry::UpdateRSNAInfo(
    _In_ WFC_BSS_ENTRY_FRAME_TYPE sourceType)
{
    BYTE* pIEBlob = NULL;
    ULONG IEsLength = 0;
    RSN_IE_INFO RSNIEInfo;
    UCHAR rsnIELength;
    ULONG rsnaAuthAlgosSupported = 0;

    GetIEBlobFromBeaconOrProbeReponse(sourceType, &pIEBlob, &IEsLength);
    if (pIEBlob == NULL || IEsLength == 0)
    {
        goto exit;
    }

    if (Dot11GetInfoElementFromIEBlob(
        pIEBlob,
        IEsLength,
        DOT11_INFO_ELEMENT_ID_RSN,
        &rsnIELength,
        (PVOID*)&pIEBlob) != NDIS_STATUS_SUCCESS)
    {
        goto exit;
    }

    if (Dot11ParseRSNIE(
        pIEBlob,
        RSNA_OUI_PREFIX,
        rsnIELength,
        &RSNIEInfo) != NDIS_STATUS_SUCCESS)
    {
        WFCTrace("Found RSN IE but couldn't parse it.");
        goto exit;
    }

    m_PMKIDCandidateFlags = 0;
    bool bFT1xSupported = false;
    bool bFTPSKSupported = false;
    bool bRSNA1xSupported = false;
    bool bRSNAPSKSupported = false;
    for (USHORT i = 0; i < RSNIEInfo.AKMSuiteCount; ++i)
    {
        DOT11_AUTH_ALGORITHM authAlgo = Dot11GetAKMSuiteFromRSNIEInfo(&RSNIEInfo, i);

        if (authAlgo == DOT11_AUTH_ALGO_RSNA)
        {
            bRSNA1xSupported = true;
            rsnaAuthAlgosSupported |= (1 << DOT11_AUTH_ALGO_RSNA);

            // Check the preauth capability
            m_PMKIDCandidateFlags = (RSNIEInfo.Capability & RSNA_CAPABILITY_PRE_AUTH) ? DOT11_PMKID_CANDIDATE_PREAUTH_ENABLED : 0;
        }
        else if (authAlgo == DOT11_AUTH_ALGO_RSNA_PSK)
        {
            bRSNAPSKSupported = true;
            rsnaAuthAlgosSupported |= (1 << DOT11_AUTH_ALGO_RSNA_PSK);
        }
        else
        {
            ULONG uFTAKMSuite = Dot11GetFTAKMSuite(&RSNIEInfo, i);

            if (uFTAKMSuite == RSNA_AKM_RSNA_FT)
            {
                bFT1xSupported = true;
            }
            else if (uFTAKMSuite == RSNA_AKM_RSNA_FT_PSK)
            {
                bFTPSKSupported = true;
            }
        }
    }

    if (bFTPSKSupported || bFT1xSupported)
    {
        m_AdvertisesCorrectFTAKMSuites = true;
    }
    else
    {
        m_AdvertisesCorrectFTAKMSuites = false;
    }

    if ((bFTPSKSupported && !bRSNAPSKSupported) ||
        (bFT1xSupported && !bRSNA1xSupported))
    {
        m_AdvertisesOnlyFTAKMSuite = true;
    }

exit:
    // Update global auth algos
    m_RsnaAuthAlgosSupported = rsnaAuthAlgosSupported;
}

void CBSSEntry::UpdateHESSIDInfo(
    WFC_BSS_ENTRY_FRAME_TYPE sourceType)
{
    BYTE *pIEBlob = NULL;
    ULONG IEsLength = 0;
    UCHAR HESSIDLength = 0;

    GetIEBlobFromBeaconOrProbeReponse(sourceType, &pIEBlob, &IEsLength);
    if (pIEBlob == NULL || IEsLength == 0)
    {
        return;
    }

    if (Dot11GetInfoElementFromIEBlob(
        pIEBlob,
        IEsLength,
        DOT11_INFO_ELEMENT_ID_INTERWORKING,
        &HESSIDLength,
        (PVOID*)&pIEBlob) != NDIS_STATUS_SUCCESS)
    {
        return;
    }

    if (HESSIDLength == (1 + sizeof(DOT11_HESSID)))
    {
        RtlCopyMemory( m_HESSID, &pIEBlob[1], sizeof(DOT11_HESSID) );
    }
    else if (HESSIDLength == (3 + sizeof(DOT11_HESSID)))
    {
        RtlCopyMemory( m_HESSID, &pIEBlob[3], sizeof(DOT11_HESSID) );
    }
    else
    {
        return;
    }

    m_IsHESSIDPresent = true;
}

NDIS_STATUS CBSSEntry::SetBeaconOrProbeResponse(
    _In_ WFC_BSS_ENTRY_FRAME_TYPE sourceType,
    _In_ ULONGLONG HostTimeStamp,
    _In_reads_bytes_opt_(bufferLength) BYTE* pBuffer,
    _In_ UINT16 bufferLength,
    _In_opt_ DOT11_MAC_ADDRESS *pWfdDeviceAddress
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG oldRsnaAuthAlgosSupported = 0;
    BYTE* pIEBlob = NULL;
    ULONG IEsLength = 0;

    if (sourceType == WFC_BSS_ENTRY_BEACON_FRAME)
    {
        // Update refresh timestamp
        m_BeaconTime = HostTimeStamp;

        ndisStatus = UpdateVariableBuffers(
            pBuffer,
            bufferLength,
            &m_BeaconFrameBody
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            goto exit;
        }

        if (m_BeaconFrameBody.dataLength > FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements))
        {
            pIEBlob = m_BeaconFrameBody.pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
            IEsLength = m_BeaconFrameBody.dataLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
        }
    }
    else if (sourceType == WFC_BSS_ENTRY_PROBE_RESPONSE_FRAME)
    {
        // Update refresh timestamp
        m_ProbeResponseTime = HostTimeStamp;

        ndisStatus = UpdateVariableBuffers(
            pBuffer,
            bufferLength,
            &m_ProbeResponseFrameBody
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            goto exit;
        }

        if (m_ProbeResponseFrameBody.dataLength > FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements))
        {
            pIEBlob = m_ProbeResponseFrameBody.pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
            IEsLength = m_ProbeResponseFrameBody.dataLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
        }
    }
    else
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    UpdateP2PFields(sourceType, IEsLength, pIEBlob, pWfdDeviceAddress);

    // Cache the old RSNA auth algos before we do the update
    oldRsnaAuthAlgosSupported = m_RsnaAuthAlgosSupported;
    UpdateRSNAInfo(sourceType);

    // Check if RSNA auth algo change
    if (oldRsnaAuthAlgosSupported  != m_RsnaAuthAlgosSupported)
    {
        // The above check can happen the first time we see a network
        if (sourceType == WFC_BSS_ENTRY_BEACON_FRAME)
        {
            if (m_ProbeResponseTime)
            {
                // RSNA auth algos changed on this AP, clear the "other" source
                WFCTrace("RSNA Auth Algorithms for BSSID %!MACADDR! changed from %d to %d in Beacon. Clearing cached probe response",             
                    m_BssID, oldRsnaAuthAlgosSupported, m_RsnaAuthAlgosSupported);

                // Clear cached probe response's timestamp
                m_ProbeResponseTime = 0;
            }                
        }
        else 
        {
            if (m_BeaconTime)
            {
                // RSNA auth algos changed on this AP, clear the "other" source
                WFCTrace("RSNA Auth Algorithms for BSSID %!MACADDR! changed from %d to %d in Probe Response. Clearing cached beacon",
                    m_BssID, oldRsnaAuthAlgosSupported, m_RsnaAuthAlgosSupported);

                // Clear cached beacon's timestamp
                m_BeaconTime = 0;
            }
        }
    }
    
    UpdateRMEnabledCapabilities(sourceType);
    UpdateFastBSSTransitionCapabilities(sourceType);
    UpdateHESSIDInfo(sourceType);

exit:
    return ndisStatus;
}

NDIS_STATUS CBSSEntry::SetDeviceContext(
    _In_reads_bytes_opt_(bufferLength) BYTE* pBuffer,
    _In_ UINT16 bufferLength
    )
{
    return UpdateVariableBuffers(
        pBuffer,
        bufferLength,
        &m_DeviceSpecificContext
        );
}

void CBSSEntry::SetSignalInfo(
    _In_ INT32 RSSI,
    _In_ UINT32 LinkQuality
    )
{
    m_SignalInfo.Rssi = RSSI;
    m_SignalInfo.LinkQuality = LinkQuality;
}

void CBSSEntry::SetChannelInfo(
    _In_ UINT32 ChannelNumber,
    _In_ UINT32 BandId
    )
{
    m_BssChannelInfo.Channel = ChannelNumber;
    m_BssChannelInfo.BandId = BandId;
}

WFC_BSS_ENTRY_FRAME_TYPE CBSSEntry::GetBeaconOrProbeFrame(
    _Out_ PDOT11_BEACON_FRAME       *ppBeacon,
    _Out_ UINT16                    *pBeaconLength
    )
{
    WFC_BSS_ENTRY_FRAME_TYPE  sourceType = WFC_BSS_ENTRY_TYPE_UNKNOWN;

    //
    // If we have both the probe response and beacons, we use the most recent IE blob
    // for reporting and also include some IEs from the other blob in the reported data
    //
    if ((m_ProbeResponseFrameBody.dataLength != 0) &&
        (m_BeaconFrameBody.dataLength != 0))
    {
        // Both are present, use the newest one as our primary
        if (m_ProbeResponseTime > m_BeaconTime)
        {
            sourceType = WFC_BSS_ENTRY_PROBE_RESPONSE_FRAME;
        }
        else
        {
            sourceType = WFC_BSS_ENTRY_BEACON_FRAME;
        }
    }
    else
    {
        // Only one is available
        if (m_ProbeResponseFrameBody.dataLength != 0)
        {
            sourceType = WFC_BSS_ENTRY_PROBE_RESPONSE_FRAME;
        }
        else if (m_BeaconFrameBody.dataLength != 0)
        {
            sourceType = WFC_BSS_ENTRY_BEACON_FRAME;
        }
        else
        {
            // Exit without setting remaining fields
            goto Exit;
        }
    }

    if ((ppBeacon) &&
        (pBeaconLength))
    {
        if (sourceType == WFC_BSS_ENTRY_BEACON_FRAME)
        {
            *ppBeacon = (PDOT11_BEACON_FRAME)(m_BeaconFrameBody.pBuffer);
            *pBeaconLength = m_BeaconFrameBody.dataLength;
        }
        else
        {
            *ppBeacon = (PDOT11_BEACON_FRAME)(m_ProbeResponseFrameBody.pBuffer);
            *pBeaconLength = m_ProbeResponseFrameBody.dataLength;
        }
    }

Exit:
    return sourceType;
}

void CBSSEntry::SetChannelAndPhyID(
    _In_ bool   bOnSTAPort,
    _In_ CAdapterPropertyCache *pAdapterCache,
    _In_ UINT32 ChannelNumber,
    _In_ UINT32 BandId
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_BEACON_FRAME pBeacon = NULL;
    UINT16              beaconLength;
    CBandCacheHelper*   pBandCacheHelper = NULL;

    m_BssChannelInfo.Channel = ChannelNumber;
    m_BssChannelInfo.BandId = BandId;

    // If we have already done work to get Phy ID & channel, lets not do it again
    if ((m_CachedChannelNumber == m_BssChannelInfo.Channel) &&
        (m_CachedBandID == m_BssChannelInfo.BandId))
    {
        // If we have valid Channel information, set the channel information to be visible
        if ((m_CachedChannelNumber) &&
            (m_CachedChannelCenterFrequency))
        {
            WFCTrace("[DeviceList] Same Band:Channel <%d:%d> and PhyID:CenterFrequency <%d:%d> visiblility=<%x->1>",
                m_CachedBandID, m_CachedChannelNumber, m_CachedDot11PhyID, m_CachedChannelCenterFrequency, m_CachedChannelInfoAvailable);

            m_CachedChannelInfoAvailable = true;
        }
        else
        {
            WFCError("[DeviceList] Invalid cached Band:Channel <%d:%d> and PhyID:CenterFrequency <%d:%d>",
                m_CachedBandID, m_CachedChannelNumber, m_CachedDot11PhyID, m_CachedChannelCenterFrequency);
        }

        goto exit;
    }

    GetBeaconOrProbeFrame (&pBeacon, &beaconLength);
    if ((!pBeacon) || (!beaconLength))
    {
        WFCError("[DeviceList] No Beacon or Probe response!");
        goto exit;
    }

    // For APs, figure out if we need to patch the channel
    if ((bOnSTAPort) &&
        (IS_FEATURE_ENABLED(TEST_FEATUREMASK_UPDATE_HARDWARE_CHANNEL)) &&
        (m_BssChannelInfo.Channel < 14))
    {
        UCHAR ucLength = 0;
        BYTE* pDSChannel = NULL;

        // For 2.4GHz, determine channel from DS attributes
        ndisStatus = Dot11GetInfoElementFromIEBlob(
            (PUCHAR)pBeacon + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements),
            beaconLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements),
            DOT11_INFO_ELEMENT_ID_DS_PARAM_SET,
            &ucLength,
            (PVOID*)&pDSChannel
            );
        if ((ndisStatus == NDIS_STATUS_SUCCESS) && ((NULL != pDSChannel) && (ucLength == 1)))
        {
            // Copy the channel from the DS IE
            if (m_BssChannelInfo.Channel != *pDSChannel)
            {
                WFCTrace("WORKAROUND: Patching Channel from %d to %d", m_BssChannelInfo.Channel, *pDSChannel);
                m_BssChannelInfo.Channel = *pDSChannel;
            }
        }

        // Ignore error in parsing
        ndisStatus = NDIS_STATUS_SUCCESS;
    }

    // Determine the cached channel/frequency for the OS
    ndisStatus = pAdapterCache->CreateBandCacheHelperForBandID(
        m_BssChannelInfo.BandId,
        &pBandCacheHelper);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to create Band cache helper for Band ID. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    ndisStatus = pBandCacheHelper->DetectPhyID(
        m_BssChannelInfo.Channel,
        pBeacon,
        beaconLength,
        &m_CachedDot11PhyID);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to get PHY ID for . Error = 0x%08x", ndisStatus);
        goto exit;
    }

    // Convert the channel number to channel frequency
    ndisStatus = pBandCacheHelper->ConvertChannelNumberToFrequency(
        m_BssChannelInfo.Channel,
        &m_CachedChannelCenterFrequency);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to convert channel number to frequency. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    WFCTrace("[DeviceList] Setting Band=<%d->%d>, Channel=<%d->%d> and PhyID:CenterFrequency=<%d:%d>",
        m_CachedBandID, m_BssChannelInfo.BandId, m_CachedChannelNumber, m_BssChannelInfo.Channel, m_CachedDot11PhyID, m_CachedChannelCenterFrequency);

    // These are the values we saved
    SetChannelHint (m_BssChannelInfo.BandId, m_BssChannelInfo.Channel);

exit:
    if (pBandCacheHelper)
    {
        delete pBandCacheHelper;
    }

    return;
}

PVARIABLE_LENGTH_BUFFER_UINT8 CBSSEntry::GetBeaconOrProbeResponse()
{
    // Prefer the probe response since that has most likelihood of having hidden network SSID.
    // If its new enough, we will use it. There is a small chance where AP's SSID was just changed
    // after we received the problem response AND we received beacon after the SSID change where
    // we would end up returning the wrong SSID. But thats a small margin that we have to deal with
    // to work with hidden networks.
    if ((m_ProbeResponseFrameBody.dataLength != 0)              // Probe response available
        &&
        (
        (m_BeaconFrameBody.dataLength == 0) ||                 // Beacon not available
        (m_ProbeResponseTime + m_MaxValidityTime >= m_BeaconTime)   // Probe response is new enough
        )
        )
    {
        // return the probe response
        return &m_ProbeResponseFrameBody;
    }
    else if (m_BeaconFrameBody.dataLength != 0)
    {
        // return the beacon
        return &m_BeaconFrameBody;
    }

    return NULL;
}

NDIS_STATUS CBSSEntry::GetSSID(
    _Out_ DOT11_SSID& outSSID)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
    PUCHAR  pIEBlob = NULL;
    ULONG   sizeOfIEBlob = 0;
    DOT11_SSID frameSsid;
    PVARIABLE_LENGTH_BUFFER_UINT8 pIEBuffer = NULL;

    // Find the SSID in the probe response/beacon
    outSSID.uSSIDLength = 0;
    RtlZeroMemory( outSSID.ucSSID, DOT11_SSID_MAX_LENGTH );

    pIEBuffer = GetBeaconOrProbeResponse();
    if (pIEBuffer != NULL)
    {
        sizeOfIEBlob = pIEBuffer->dataLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
        pIEBlob = pIEBuffer->pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);

        //
        // Get the SSID IE
        //
        ndisStatus = Dot11CopySSIDFromIEBlob(
            pIEBlob,
            sizeOfIEBlob,
            &frameSsid
            );
        //_Analysis_assume_( frameSsid.uSSIDLength <= 32 );
        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            // SSID saved
            outSSID.uSSIDLength = frameSsid.uSSIDLength;
            RtlCopyMemory( outSSID.ucSSID, frameSsid.ucSSID, frameSsid.uSSIDLength );
        }
    }

    return ndisStatus;
}

NDIS_STATUS
CBSSEntry::FillIEs(
_In_ WFC_BSS_ENTRY_FRAME_TYPE sourceType,
_Out_writes_bytes_opt_(DestinationLength) PUCHAR pDestBuffer,
_In_ ULONG      DestinationLength,
_Out_ PULONG    pIEsSize
)
{
    PUCHAR  primaryIEBlob = NULL;
    ULONG   sizeOfPrimaryIEBlob = 0;
    PUCHAR  secondaryIEBlob = NULL;
    ULONG   sizeOfSecondaryIEBlob = 0;
    ULONG   IEEntrySize = 0;
    PDOT11_INFO_ELEMENT pInfoElemHdr = NULL, pTempIEHdr;
    PUCHAR  pVendorIEData = NULL;
    BOOLEAN copyIEEntry = FALSE;
    BOOLEAN ssidFound = FALSE;
    DOT11_SSID      probeResponseSSID;
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    *pIEsSize = 0;

    probeResponseSSID.uSSIDLength = 0;

    //
    // If beacon blob does not have the SSID, we will copy the SSID from 
    // the probe response blob before copying rest of data from the beacon. At the end, we will 
    // copy the WCN IEs from the second IE blob into the return list. 
    //

    if (sourceType == WFC_BSS_ENTRY_BEACON_FRAME)
    {
        NT_ASSERT(m_BeaconFrameBody.pBuffer != NULL);

        // Length is already validated
        sizeOfPrimaryIEBlob = m_BeaconFrameBody.dataLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
        primaryIEBlob = m_BeaconFrameBody.pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);

        if (m_ProbeResponseFrameBody.dataLength != 0)
        {
            sizeOfSecondaryIEBlob = m_ProbeResponseFrameBody.dataLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
            secondaryIEBlob = m_ProbeResponseFrameBody.pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);

            // For hidden SSIDs, we would only have SSID in probe
            // response. Pick it up now

            //
            // Get the SSID IE from the probe response for use with hidden networks
            //
            ndisStatus = Dot11CopySSIDFromIEBlob(
                secondaryIEBlob,
                sizeOfSecondaryIEBlob,
                &probeResponseSSID
                );
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                // Couldnt find SSID IE in the probe response
                probeResponseSSID.uSSIDLength = 0;
                ndisStatus = NDIS_STATUS_SUCCESS;
            }
        }
    }
    else
    {
        NT_ASSERT(m_ProbeResponseFrameBody.pBuffer != NULL);

        // Length is already validated
        sizeOfPrimaryIEBlob = m_ProbeResponseFrameBody.dataLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
        primaryIEBlob = m_ProbeResponseFrameBody.pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);

        if (m_BeaconFrameBody.dataLength != 0)
        {
            sizeOfSecondaryIEBlob = m_BeaconFrameBody.dataLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
            secondaryIEBlob = m_BeaconFrameBody.pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
        }
    }

    // First walk the primary IE blob & copy all possible IEs
    while (sizeOfPrimaryIEBlob > 0)
    {
        pInfoElemHdr = (PDOT11_INFO_ELEMENT)primaryIEBlob;
        if ((sizeOfPrimaryIEBlob < sizeof(DOT11_INFO_ELEMENT)) ||
            ((pInfoElemHdr->Length + sizeof(DOT11_INFO_ELEMENT)) > sizeOfPrimaryIEBlob))
        {
            // Shouldnt happen. The IE's must already be verified
            break;
        }

        // Copy the entry from beacon IE blob
        copyIEEntry = TRUE;

        switch (pInfoElemHdr->ElementID)
        {
        case DOT11_INFO_ELEMENT_ID_SSID:
        {
            //
            // For SSID, we copy cached Probe SSID if we do not have a SSID
            // in the Beacon. This is to handle hidden SSID. For hidden SSID, 
            // the OS could first do a scan and expect us to indicate the 
            // found SSID so that it can do a connect. We copy the probe 
            // response SSID instead of the hidden one from the beacon
            //
            ssidFound = TRUE;

            if (Dot11IsHiddenSSID((((PUCHAR)pInfoElemHdr) + sizeof(DOT11_INFO_ELEMENT)), pInfoElemHdr->Length))
            {
                if (probeResponseSSID.uSSIDLength != 0)
                {
                    // Check if this was new enough
                    if ((m_ProbeResponseTime + m_MaxValidityTime) < CSystem::get_CurrentTime())
                    {
                        m_ProbeResponseFrameBody.dataLength = 0;
                        WFCTrace("Hidden SSID Probe response too old for %02X-%02X-%02X-%02X-%02X-%02X",
                            m_BssID[0], m_BssID[1], m_BssID[2],
                            m_BssID[3], m_BssID[4], m_BssID[5]);

                    }
                    else
                    {
                        // Will use probe response SSID and not copy source SSID IE
                        copyIEEntry = FALSE;

                        IEEntrySize = sizeof(DOT11_INFO_ELEMENT)+
                            probeResponseSSID.uSSIDLength;
                        *pIEsSize += IEEntrySize;

                        WFCTrace("Will use Probe SSID for %02X-%02X-%02X-%02X-%02X-%02X",
                            m_BssID[0], m_BssID[1], m_BssID[2],
                            m_BssID[3], m_BssID[4], m_BssID[5]);

                        if (IEEntrySize > DestinationLength)
                        {
                            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;

                            // Stop copying, but continue going through the IEs
                            DestinationLength = 0;
                        }
                        else
                        {
                            pTempIEHdr = (PDOT11_INFO_ELEMENT)pDestBuffer;

                            pTempIEHdr->ElementID = DOT11_INFO_ELEMENT_ID_SSID;
                            pTempIEHdr->Length = (UCHAR)probeResponseSSID.uSSIDLength;

                            RtlCopyMemory(pDestBuffer + sizeof(DOT11_INFO_ELEMENT),
                                probeResponseSSID.ucSSID,
                                probeResponseSSID.uSSIDLength
                                );

                            DestinationLength -= IEEntrySize;
                            pDestBuffer += IEEntrySize;
                        }
                    }
                }
            }
        }
        break;

        default:
            //
            // For everything else we copy what is in the
            // primary IE blob
            //

            //
            // The SSID IE should be first. If it has not yet been found
            // copy the SSID from the probe (if available) before copying the
            // other IEs
            //
            if (ssidFound == FALSE)
            {
                ssidFound = TRUE;
                if (probeResponseSSID.uSSIDLength != 0)
                {
                    IEEntrySize = sizeof(DOT11_INFO_ELEMENT)+
                        probeResponseSSID.uSSIDLength;
                    *pIEsSize += IEEntrySize;

                    WFCTrace("Will use Probe SSID for %02X-%02X-%02X-%02X-%02X-%02X",
                        m_BssID[0], m_BssID[1], m_BssID[2],
                        m_BssID[3], m_BssID[4], m_BssID[5]);

                    if (IEEntrySize > DestinationLength)
                    {
                        ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
                        // Stop copying, but continue going through the IEs
                        DestinationLength = 0;
                    }
                    else
                    {
                        pTempIEHdr = (PDOT11_INFO_ELEMENT)pDestBuffer;

                        pTempIEHdr->ElementID = DOT11_INFO_ELEMENT_ID_SSID;
                        pTempIEHdr->Length = (UCHAR)probeResponseSSID.uSSIDLength;

                        RtlCopyMemory(pDestBuffer + sizeof(DOT11_INFO_ELEMENT),
                            probeResponseSSID.ucSSID,
                            probeResponseSSID.uSSIDLength
                            );

                        DestinationLength -= IEEntrySize;
                        pDestBuffer += IEEntrySize;

                        // We still copy the original IE
                    }
                }
            }

            break;
        }

        //
        // Copy/skip past IE in original buffer
        // 
        IEEntrySize = sizeof(DOT11_INFO_ELEMENT)+
            pInfoElemHdr->Length;

        if (copyIEEntry)
        {
            *pIEsSize += IEEntrySize;

            if (IEEntrySize > DestinationLength)
            {
                ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
                DestinationLength = 0;
            }
            else
            {
                //
                // Copy IE from source buffer to destination
                //
                NdisMoveMemory(pDestBuffer, primaryIEBlob, IEEntrySize);

                DestinationLength -= IEEntrySize;
                pDestBuffer += IEEntrySize;
            }
        }

        // Move forward in the beacon IE
        sizeOfPrimaryIEBlob -= IEEntrySize;
        primaryIEBlob += IEEntrySize;
    }


    //
    // Now walk the secondary IEs and copy the WCN IEs. We dont 
    // attempt to merge these, but just place them at the end
    //
    while (sizeOfSecondaryIEBlob > 0)
    {
        pInfoElemHdr = (PDOT11_INFO_ELEMENT)secondaryIEBlob;
        if ((sizeOfSecondaryIEBlob < sizeof(DOT11_INFO_ELEMENT)) ||
            ((pInfoElemHdr->Length + sizeof(DOT11_INFO_ELEMENT)) > sizeOfSecondaryIEBlob))
        {
            // Shouldnt happen. The IE's must already be verified
            break;
        }

        // Dont copy all the entries from IE blob
        copyIEEntry = FALSE;

        switch (pInfoElemHdr->ElementID)
        {
        case DOT11_INFO_ELEMENT_ID_VENDOR_SPECIFIC:
        {
            //
            // Check the vendor specific IE type
            // WCN IE contains 4 bytes WCN_IE_TAG (0x04f25000) at the very beginning of the IE data
            // If we don't find the tag, it's not a WCN IE & we skip this IE
            //
            if (pInfoElemHdr->Length >= 4)
            {
                pVendorIEData = secondaryIEBlob + sizeof(DOT11_INFO_ELEMENT);
                if (*((ULONG UNALIGNED *)pVendorIEData) == WCN_IE_TAG)
                {
                    copyIEEntry = TRUE;
                }
            }
        }
        break;

        default:
            //
            // For everything else we dont copy what is in the
            // probe response
            //
            copyIEEntry = FALSE;
            break;
        }

        //
        // Copy/skip past IE in original buffer
        // 
        IEEntrySize = sizeof(DOT11_INFO_ELEMENT)+
            pInfoElemHdr->Length;

        if (copyIEEntry)
        {
            *pIEsSize += IEEntrySize;

            if (IEEntrySize > DestinationLength)
            {
                ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
                DestinationLength = 0;
            }
            else
            {
                //
                // Copy IE from source buffer to destination
                //
                NdisMoveMemory(pDestBuffer, secondaryIEBlob, IEEntrySize);

                DestinationLength -= IEEntrySize;
                pDestBuffer += IEEntrySize;
            }
        }

        // Move forward in the IE blob
        sizeOfSecondaryIEBlob -= IEEntrySize;
        secondaryIEBlob += IEEntrySize;
    }

    return ndisStatus;
}


NDIS_STATUS
CBSSEntry::FillDot11BSSEntry(
_In_ WFC_BSS_ENTRY_FRAME_TYPE sourceType,
_Out_writes_bytes_opt_(DestinationLength) PDOT11_BSS_ENTRY pDestBSSEntry,
_In_ ULONG      DestinationLength,
_Out_ PULONG    pBSSEntrySize
)
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_BEACON_FRAME pBeacon = NULL;
    UINT16          beaconLength;

    // Fill the header
    *pBSSEntrySize = FIELD_OFFSET(DOT11_BSS_ENTRY, ucBuffer);
    if (DestinationLength >= (*pBSSEntrySize))
    {
        if (sourceType == WFC_BSS_ENTRY_BEACON_FRAME)
        {
            pBeacon = (PDOT11_BEACON_FRAME)(m_BeaconFrameBody.pBuffer);
            beaconLength = m_BeaconFrameBody.dataLength;
            pDestBSSEntry->ullHostTimestamp = m_BeaconTime;
        }
        else
        {
            pBeacon = (PDOT11_BEACON_FRAME)(m_ProbeResponseFrameBody.pBuffer);
            beaconLength = m_ProbeResponseFrameBody.dataLength;
            pDestBSSEntry->ullHostTimestamp = m_ProbeResponseTime;
        }

        pDestBSSEntry->usBeaconPeriod = pBeacon->BeaconInterval;
        pDestBSSEntry->ullTimestamp = pBeacon->Timestamp;

        if (pBeacon->Capability.IBSS)
        {
            pDestBSSEntry->dot11BSSType = dot11_BSS_type_independent;
        }
        else
        {
            pDestBSSEntry->dot11BSSType = dot11_BSS_type_infrastructure;
        }

        pDestBSSEntry->usCapabilityInformation = pBeacon->Capability.usValue;

        pDestBSSEntry->lRSSI = m_SignalInfo.Rssi;
        pDestBSSEntry->uLinkQuality = m_SignalInfo.LinkQuality;

        // Use the saved values
        pDestBSSEntry->uPhyId = m_CachedDot11PhyID;
        pDestBSSEntry->PhySpecificInfo.uChCenterFrequency = m_CachedChannelCenterFrequency;

        //
        // NOTE: We assume that we are always in regulatory domain
        //
        pDestBSSEntry->bInRegDomain = TRUE;

        RtlCopyMemory(
            pDestBSSEntry->dot11BSSID,
            m_BssID,
            sizeof(DOT11_MAC_ADDRESS)
            );

        WFCTrace("[BSSList] Reporting BSSID = %!MACADDR!", m_BssID);
    }
    else
    {
        ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
    }

    return ndisStatus;
}

NDIS_STATUS
CBSSEntry::FillDot11WfdDeviceEntry(
_In_ WFC_BSS_ENTRY_FRAME_TYPE sourceType,
_Out_writes_bytes_opt_(DestinationLength) PDOT11_WFD_DEVICE_ENTRY pDestBSSEntry,
_In_ ULONG      DestinationLength,
_Out_ PULONG    pBSSEntrySize)
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_BEACON_FRAME pBeacon = NULL;
    ULONG currentOffset = sizeof(*pDestBSSEntry);
    ULONG calculatedLength = sizeof(*pDestBSSEntry);

    RtlZeroMemory( pDestBSSEntry, DestinationLength );

    // Fill the header
    if (m_BeaconFrameBody.dataLength > FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements))
    {
        if (!NT_SUCCESS(
            RtlULongAdd(
            calculatedLength,
            m_BeaconFrameBody.dataLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements),
            &calculatedLength)))
        {
            WFCError("Couldn't calculate WFD Device Entry Size.");
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            NT_ASSERT(FALSE);
            goto exit;
        }
    }

    if (m_ProbeResponseFrameBody.dataLength > FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements))
    {
        if (!NT_SUCCESS(
            RtlULongAdd(
            calculatedLength,
            m_ProbeResponseFrameBody.dataLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements),
            &calculatedLength)))
        {
            WFCError("Couldn't calculate WFD Device Entry Size.");
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            NT_ASSERT(FALSE);
            goto exit;
        }
    }

    *pBSSEntrySize = calculatedLength;

    if (DestinationLength >= calculatedLength)
    {
        if (sourceType == WFC_BSS_ENTRY_BEACON_FRAME)
        {
            pBeacon = (PDOT11_BEACON_FRAME)(m_BeaconFrameBody.pBuffer);
        }
        else
        {
            pBeacon = (PDOT11_BEACON_FRAME)(m_ProbeResponseFrameBody.pBuffer);
        }

        // Use the saved values
        pDestBSSEntry->uPhyId = m_CachedDot11PhyID;
        pDestBSSEntry->PhySpecificInfo.uChCenterFrequency = m_CachedChannelCenterFrequency;

        RtlCopyMemory(
            pDestBSSEntry->dot11BSSID,
            m_BssID,
            sizeof(DOT11_MAC_ADDRESS));

        if (pBeacon->Capability.IBSS)
        {
            pDestBSSEntry->dot11BSSType = dot11_BSS_type_independent;
        }
        else
        {
            pDestBSSEntry->dot11BSSType = dot11_BSS_type_infrastructure;
        }

        //
        // In device mode, TransmitterAddress and BSSID match
        //
        RtlCopyMemory(
            pDestBSSEntry->TransmitterAddress,
            m_BssID,
            sizeof(DOT11_MAC_ADDRESS));

        WFCTrace("[DeviceList] Reporting Device = %!MACADDR!", m_BssID);

        pDestBSSEntry->lRSSI = m_SignalInfo.Rssi;
        pDestBSSEntry->uLinkQuality = m_SignalInfo.LinkQuality;

        pDestBSSEntry->usBeaconPeriod = pBeacon->BeaconInterval;
        pDestBSSEntry->ullTimestamp = pBeacon->Timestamp;

        pDestBSSEntry->usCapabilityInformation = pBeacon->Capability.usValue;

        if (m_BeaconFrameBody.dataLength > FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements) &&
            m_BeaconFrameBody.pBuffer != NULL)
        {
            pDestBSSEntry->ullBeaconHostTimestamp = m_BeaconTime;
            pDestBSSEntry->uBeaconIEsOffset = currentOffset;
            pDestBSSEntry->uBeaconIEsLength = m_BeaconFrameBody.dataLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
            RtlCopyMemory(
                (BYTE*)pDestBSSEntry + currentOffset,
                m_BeaconFrameBody.pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements),
                pDestBSSEntry->uBeaconIEsLength);
#if 0
            tmpTraceWfdIE_DbgInstrument((BYTE*)pDestBSSEntry + currentOffset,
                pDestBSSEntry->uBeaconIEsLength,
                "beacon");
#endif
            currentOffset += pDestBSSEntry->uBeaconIEsLength;
        }
        else
        {
            pDestBSSEntry->ullBeaconHostTimestamp = 0;
            pDestBSSEntry->uBeaconIEsOffset = 0;
            pDestBSSEntry->uBeaconIEsLength = 0;
        }

        WFCTrace("[CBSSEntry][device][probe] m_ProbeResponseFrameBody.dataLength  %u\n", m_ProbeResponseFrameBody.dataLength);

        if (m_ProbeResponseFrameBody.dataLength > FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements))
        {
            pDestBSSEntry->ullProbeResponseHostTimestamp = m_ProbeResponseTime;
            pDestBSSEntry->uProbeResponseIEsOffset = currentOffset;
            pDestBSSEntry->uProbeResponseIEsLength = m_ProbeResponseFrameBody.dataLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements);
            RtlCopyMemory(
                (BYTE*)pDestBSSEntry + currentOffset,
                m_ProbeResponseFrameBody.pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements),
                pDestBSSEntry->uProbeResponseIEsLength);

#if 0
            tmpTraceWfdIE_DbgInstrument((BYTE*)pDestBSSEntry + currentOffset,
                pDestBSSEntry->uProbeResponseIEsLength,
                "probe");
#endif

            currentOffset += pDestBSSEntry->uProbeResponseIEsLength;
        }
        else
        {
            pDestBSSEntry->ullProbeResponseHostTimestamp = 0;
            pDestBSSEntry->uProbeResponseIEsOffset = 0;
            pDestBSSEntry->uProbeResponseIEsLength = 0;
        }
    }
    else
    {
        ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
    }

exit:
    return ndisStatus;
}



NDIS_STATUS
CBSSEntry::CopyDot11BSSEntry(
_Out_writes_bytes_opt_(DestinationLength) PUCHAR pDestBuffer,
_In_ ULONG      DestinationLength,
_Out_ PULONG    pEntrySize
)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_STATUS completeStatus = NDIS_STATUS_SUCCESS;
    WFC_BSS_ENTRY_FRAME_TYPE sourceType;
    ULONG   bssEntrySize = 0;
    ULONG   iesSize = 0;
    PDOT11_BSS_ENTRY bssEntry = (PDOT11_BSS_ENTRY)pDestBuffer;
    *pEntrySize = 0;

    sourceType = GetBeaconOrProbeFrame (NULL, NULL);
    if (sourceType == WFC_BSS_ENTRY_TYPE_UNKNOWN)
    {
        // Exit without reporting this entry
        completeStatus = ndisStatus;
        WFCTrace ("CopyDot11BSSEntry: NOT Reporting BSS without beacon or probe [%!MACADDR!]\n", m_BssID); 
        goto Exit;
    }

    //
    // One additional check is if beacon blob does not have the SSID, we will copy the SSID from 
    // the probe response blob before copying rest of data from the beacon. At the end, we will 
    // copy the WCN IEs from the second IE blob into the return list. 
    //
    ndisStatus = FillDot11BSSEntry(
        sourceType,
        bssEntry,
        DestinationLength,
        &bssEntrySize
        );
    if (ndisStatus == NDIS_STATUS_BUFFER_OVERFLOW)
    {
        // Not enough space. Force truncate size since 
        // we have to figure out IEs length
        DestinationLength = 0;
        completeStatus = ndisStatus;
        bssEntry = NULL;    // Dont use this anymore        
    }
    else if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        // Adjust available space
        pDestBuffer += bssEntrySize;
        DestinationLength -= bssEntrySize;
    }
    else
    {
        // Something went wrong
        completeStatus = ndisStatus;
        goto Exit;
    }

    *pEntrySize += bssEntrySize;

    // Determine IEs length
    ndisStatus = FillIEs(
        sourceType,
        pDestBuffer,
        DestinationLength,
        &iesSize
        );
    if (ndisStatus == NDIS_STATUS_BUFFER_OVERFLOW)
    {
        // Not enough space. Force truncate size since 
        // we have to figure out IEs length
        DestinationLength = 0;
        completeStatus = ndisStatus;
    }
    else if ((ndisStatus == NDIS_STATUS_SUCCESS) &&
        (bssEntry != NULL))        // Is implied because DestinationLength > 0, but being explicit about this
    {
        // Adjust available space
        pDestBuffer += iesSize;
        DestinationLength -= iesSize;
        bssEntry->uBufferLength = iesSize;
    }
    else
    {
        // Something went wrong
        completeStatus = ndisStatus;
        goto Exit;
    }
    *pEntrySize += iesSize;


Exit:
    return completeStatus;
}

NDIS_STATUS
CBSSEntry::CopyDot11WfdBSSEntry(
_Out_writes_bytes_opt_(DestinationLength) PUCHAR pDestBuffer,
_In_ ULONG      DestinationLength,
_Out_ PULONG    pEntrySize
)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_STATUS completeStatus = NDIS_STATUS_SUCCESS;
    WFC_BSS_ENTRY_FRAME_TYPE sourceType;
    ULONG   bssEntrySize = 0;
    PDOT11_WFD_DEVICE_ENTRY bssEntry = (PDOT11_WFD_DEVICE_ENTRY)pDestBuffer;
    *pEntrySize = 0;

    sourceType = GetBeaconOrProbeFrame (NULL, NULL);
    if (sourceType == WFC_BSS_ENTRY_TYPE_UNKNOWN)
    {
        // Exit without reporting this entry
        WFCTrace ("CopyDot11WfdBSSEntry: NOT Reporting Device without beacon or probe [%!DOT11SSID! : %!MACADDR!]\n",
            &m_GroupId.SSID, m_BssID);
        goto Exit;
    }

    ndisStatus = FillDot11WfdDeviceEntry(
        sourceType,
        bssEntry,
        DestinationLength,
        &bssEntrySize);
    if (ndisStatus == NDIS_STATUS_BUFFER_OVERFLOW)
    {
        // Not enough space. Force truncate size since 
        // we have to figure out IEs length
        DestinationLength = 0;
        completeStatus = ndisStatus;
        bssEntry = NULL;    // Dont use this anymore        
    }
    else if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        // Adjust available space
        pDestBuffer += bssEntrySize;
        DestinationLength -= bssEntrySize;
    }
    else
    {
        // Something went wrong
        completeStatus = ndisStatus;
        goto Exit;
    }

    *pEntrySize += bssEntrySize;

Exit:
    return completeStatus;
}

NDIS_STATUS CBSSEntry::UpdateBackgroundDeviceIndicatedByDriver(
    _In_ CAdapter *pAdapter
    )
{
    return pAdapter->GetServicesManager()->UpdateBackgroundDeviceIndicatedByDriver(
        &m_BSSEntryDiscoveryContext,
        &m_IsWFDServiceUpdated,
        &m_BssID,
        m_pBeaconWfdServiceNames,
        m_pProbeWfdServiceNames);
}

VOID CBSSEntry::UpdateBackgroundDeviceLastQueriedTime(
    _In_ CAdapter *pAdapter
    )
{
    pAdapter->GetServicesManager()->MarkP2PBackgroundContextsQueried(
        &m_BssID,
        m_pBeaconWfdServiceNames,
        m_pProbeWfdServiceNames);
}

VOID CBSSEntry::UpdateBackgroundDeviceExpired(
    _In_ CAdapter *pAdapter
    )
{
    pAdapter->GetServicesManager()->UpdateBackgroundDeviceExpired(
        &m_BssID,
        m_BSSEntryDiscoveryContext,
        m_pBeaconWfdServiceNames,
        m_pProbeWfdServiceNames);

    //
    // Set the device and state to that of a new device so that it will reported again as a new device if seen
    //
    m_BSSEntryDiscoveryContext = 0;
    m_IsWFDServiceUpdated = false;

    if (m_pBeaconWfdServiceNames)
    {
        delete[] m_pBeaconWfdServiceNames;
        m_pBeaconWfdServiceNames = NULL;
    }
    if (m_pProbeWfdServiceNames)
    {
        delete[] m_pProbeWfdServiceNames;
        m_pProbeWfdServiceNames = NULL;
    }
}

VOID CBSSEntry::OnLinkQualityUpdate(
    _In_ UCHAR  LinkQuality
    )
{
    m_SignalInfo.LinkQuality = LinkQuality;
    if (LinkQuality > WFC_CONSTANT::BssStillVisibleLinkQualityLowerLimit)
    {
        // If this is a good enough signal strength, then update the
        // time
        if (m_BeaconFrameBody.dataLength > 0)
        {
            m_BeaconTime = CSystem::get_CurrentTime();
        }
        else if (m_ProbeResponseFrameBody.dataLength > 0)
        {
            m_ProbeResponseTime = CSystem::get_CurrentTime();
        }
    }
}


//==============================================================================================

CBSSListManager::CBSSListManager(
    ) : m_CurrentBSSEntryCount(0),
    m_MaxBSSEntryCount(512),
    m_IHVMaintainedExtSTABSSListCache(false),
    m_ScanInProgress(false),
    m_BackgroundDiscoveryEnabled(false),
    m_LastExpiredEntriesCheckTime(0),
    m_PortType(WfcPortTypeExtSTA),
    m_BssEntryExpiryTime(WFC_CONSTANT::DefaultBssEntryExpiryTime),
    m_BSSListUpdateTime(0)
{
    InitializeListHead(&m_BSSEntryList);
}

CBSSListManager::~CBSSListManager()
{
    FlushAllEntries(true);
}

NDIS_STATUS CBSSListManager::Initialize(
    _In_ CAdapter *pAdapter,
    _In_ CAdapterPropertyCache *pAdapterCache,
    _In_ WFC_PORT_TYPE PortType
    )
{
    m_pAdapter = pAdapter;
    m_pAdapterCache = pAdapterCache;
    m_PortType = PortType;

    if (WfcPortTypeWFDDevice == PortType)
    {
        m_BssEntryExpiryTime = WFC_CONSTANT::WFDDeviceBssEntryExpiryTime;
    }
    else
    {
        NT_ASSERT (WfcPortTypeExtSTA == PortType);
    }

    // Save the maximum number of entries we will remember
    pAdapterCache->GetPropertyULong(
        WfcAdapterPropertyRegMaxBssEntryCount,
        &m_MaxBSSEntryCount
        );

    return NDIS_STATUS_SUCCESS;
}

void  CBSSListManager::SetBSSListCacheMode(
    _In_ bool IHVMaintainedExtSTABSSListCache
    )
{
    m_IHVMaintainedExtSTABSSListCache = IHVMaintainedExtSTABSSListCache;
    
    if (IHVMaintainedExtSTABSSListCache)
    {
        // IHV maintains scan list, we start with the assumption that bss list is not current
        m_BSSListUpdateTime = 0;
    };

}

bool CBSSListManager::IsIHVMaintainedExtSTABSSListCache()
{
    return m_IHVMaintainedExtSTABSSListCache;
}

void  CBSSListManager::OnScanStart(
    _In_ WDI_SCAN_TRIGGER ScanTrigger
    )
{
    m_ScanInProgress = true;
    m_ScanInProgressTrigger = ScanTrigger;
    
    if (m_IHVMaintainedExtSTABSSListCache)
    {
        // Scan is started and IHV maintains scan list. Assume scan list is not updated
        m_BSSListUpdateTime = 0;
    };

}

void  CBSSListManager::OnScanComplete()
{
    m_ScanInProgress = false;
    // We dont touch the BSS list state
}

bool CBSSListManager::IsScanInProgress(
    _Out_ WDI_SCAN_TRIGGER  *pScanInProgressTrigger
    )
{    
    if (m_ScanInProgress)
    {
        *pScanInProgressTrigger = m_ScanInProgressTrigger;
    }

    return m_ScanInProgress;
}

void CBSSListManager::OnBSSListUpdateComplete(
    _In_ NDIS_STATUS CompletionStatus
    )
{
    if (m_IHVMaintainedExtSTABSSListCache && 
        (CompletionStatus == NDIS_STATUS_SUCCESS) && 
        !m_ScanInProgress
        )
    {
        // IHV maintains scan list. We just got a completed update
        // Mark the BSS list as updated
        m_BSSListUpdateTime = CSystem::get_CurrentTime();
    }
}

bool CBSSListManager::DoesBSSListNeedUpdate(
    _In_ ULONGLONG MaximumAgeOfIhvCachedList
    )
{
    if (!m_IHVMaintainedExtSTABSSListCache)
    {
        // BSS list maintained by us. We never need an update
        return false;
    }        
    else
    {
        // BSS list is maintained by IHV. 
        
        if (m_ScanInProgress)
        {
            // Scan is in progress
            return true;
        }

        // Check when we updated the BSS list and return based on that
        ULONGLONG bsslistAge = CSystem::get_CurrentTime() - m_BSSListUpdateTime;
        
        if (bsslistAge < MaximumAgeOfIhvCachedList)
        {
            // List is new enough
            return false;
        }

    }
    return true;
}

void CBSSListManager::FlushAllEntries(
    _In_ bool bFullFlush
    )
/*++
Routine Description:
frees all bss entries

Arguments:

Return Value:
--*/
{
    PLIST_ENTRY pCurrentEntry;
    PLIST_ENTRY pNextEntry;
    LIST_ENTRY  PersistedEntries;

    TraceEntry();

    if (IsListEmpty(&m_BSSEntryList))
    {
        return;
    }

    InitializeListHead(&PersistedEntries);

    pCurrentEntry = m_BSSEntryList.Flink;

    while (pCurrentEntry != &m_BSSEntryList)
    {
        CBSSEntry* pBSSEntry = CBSSEntry::GetBSSEntryFromListEntry(pCurrentEntry);

        pNextEntry = pCurrentEntry->Flink;
        RemoveEntryList(pCurrentEntry);

        pCurrentEntry = pNextEntry;
           
        NT_ASSERT(m_CurrentBSSEntryCount > 0);
        m_CurrentBSSEntryCount--;

        if (!bFullFlush)
        {
            if ((pBSSEntry->GetAssociatedFlag())||
                (pBSSEntry->GetBlockedInfo()->BlockedReasons != WFC_BLOCKED_REASON_NONE))
            {
                WFCTrace("[BSSList][Flush] Not flushing BSSID %!MACADDR! (Blocked Reasons = 0x%x)", 
                    *(pBSSEntry->GetBssId()), pBSSEntry->GetBlockedInfo()->BlockedReasons);
                // If this is an AP we are currently associated to
                // or something we have blocked dont delete it 
                // but just mark it as not being visible
                pBSSEntry->SetVisibility(false);

                // Put this into our persisted entries list (same insert
                // as AddBSSEntry)
                InsertTailList(&PersistedEntries, pBSSEntry->GetListEntry());
                continue;
            }
        }

        if (IsBackgroundDiscoveryEnabled())
        {
            pBSSEntry->UpdateBackgroundDeviceExpired ( m_pAdapter );
        }

        // If we dont need to persist it, delete it
        delete pBSSEntry;
    }

    // Add the persisted entries back in. Since the entries are added Head-first,
    // start from the end of the local list so that the entries are added back as before
    pCurrentEntry = PersistedEntries.Blink;
    while (pCurrentEntry != &PersistedEntries)
    {
        pNextEntry = pCurrentEntry->Blink;
        RemoveEntryList(pCurrentEntry);

        AddBSSEntry(CBSSEntry::GetBSSEntryFromListEntry(pCurrentEntry));        
        pCurrentEntry = pNextEntry;
    }

    TraceExit(NDIS_STATUS_SUCCESS);

    return;
}

void CBSSListManager::FlushAllNeighborReports()
/*++
Routine Description:
Frees all neighbor reports

Arguments:

Return Value:
--*/
{
    PLIST_ENTRY pCurrentEntry;

    TraceEntry();

    if (IsListEmpty(&m_BSSEntryList))
    {
        return;
    }

    pCurrentEntry = m_BSSEntryList.Flink;

    while (pCurrentEntry != &m_BSSEntryList)
    {
        CBSSEntry* pBSSEntry = CBSSEntry::GetBSSEntryFromListEntry(pCurrentEntry);
        pBSSEntry->FlushNeighborReport();
        pCurrentEntry = pCurrentEntry->Flink;
    }

    TraceExit(NDIS_STATUS_SUCCESS);

    return;
}

void CBSSListManager::FlushAllAssociationState()
/*++
Routine Description:
Frees all neighbor reports

Arguments:

Return Value:
--*/
{
    PLIST_ENTRY pCurrentEntry;

    TraceEntry();

    if (IsListEmpty(&m_BSSEntryList))
    {
        return;
    }

    pCurrentEntry = m_BSSEntryList.Flink;

    while (pCurrentEntry != &m_BSSEntryList)
    {
        CBSSEntry* pBSSEntry = CBSSEntry::GetBSSEntryFromListEntry(pCurrentEntry);
        pBSSEntry->FlushNeighborReport();
        pBSSEntry->SetAssociatedFlag(false);
        pCurrentEntry = pCurrentEntry->Flink;
    }

    TraceExit(NDIS_STATUS_SUCCESS);

    return;
}


CBSSEntry* CBSSListManager::FindBSSEntry(
    _In_ const DOT11_MAC_ADDRESS& BssId)
    /*++
    Routine Description:
    find bss entry for bssId
    Arguments:

    Return Value:
    --*/
{
    PLIST_ENTRY pCurrentEntry;

    TraceEntry();

    if (IsListEmpty(&m_BSSEntryList))
    {
        return NULL;
    }

    pCurrentEntry = m_BSSEntryList.Flink;

    while (pCurrentEntry != &m_BSSEntryList)
    {
        CBSSEntry* pBSSEntry = CBSSEntry::GetBSSEntryFromListEntry(pCurrentEntry);
        if (pBSSEntry->IsBssIdEqual(BssId))
        {
            return pBSSEntry;
        }

        pCurrentEntry = pCurrentEntry->Flink;
    }

    TraceExit(NDIS_STATUS_SUCCESS);

    return NULL;
}


CBSSEntry* CBSSListManager::FindBSSEntryByGroupID(
    _In_ const DOT11_WFD_GROUP_ID& GroupID,
    _In_ bool MatchDeviceAddress,
    _In_ bool MatchSSID,
    _In_ bool MatchAll)
{
    PLIST_ENTRY pCurrentEntry;

    TraceEntry();

    // If you ignore both any entry will be returned
    NT_ASSERT(MatchDeviceAddress || MatchSSID);

    if (IsListEmpty(&m_BSSEntryList))
    {
        WFCInfo ("FindBSSEntryByGroupID: BSSEntryList is empty");
        return NULL;
    }

    pCurrentEntry = m_BSSEntryList.Flink;

    while (pCurrentEntry != &m_BSSEntryList)
    {
        CBSSEntry* pBSSEntry = CBSSEntry::GetBSSEntryFromListEntry(pCurrentEntry);
        if (pBSSEntry->IsGroupIdMatch(GroupID, MatchDeviceAddress, MatchSSID, MatchAll))
        {
            WFCInfo ("FindBSSEntryByGroupID: SUCCESS - MatchDevice:SSID:All=<%x:%x:%x> [ %!DOT11SSID! : %!MACADDR! ]\n",
                MatchDeviceAddress, MatchSSID, MatchAll, (DOT11_SSID *) &GroupID.SSID, (PUCHAR) GroupID.DeviceAddress);

            return pBSSEntry;
        }

        pCurrentEntry = pCurrentEntry->Flink;
    }

    WFCInfo ("FindBSSEntryByGroupID: BSSEntry was NOT found");
    return NULL;
}


void CBSSListManager::RemoveBSSEntry(
    _In_ const DOT11_MAC_ADDRESS& BssId)
    /*++
    Routine Description:
    find and frees bss entry for bssId
    Arguments:

    Return Value:
    --*/
{
    PLIST_ENTRY pCurrentEntry;
    PLIST_ENTRY pNextEntry;

    TraceEntry();

    if (IsListEmpty(&m_BSSEntryList))
    {
        return;
    }

    pCurrentEntry = m_BSSEntryList.Flink;

    while (pCurrentEntry != &m_BSSEntryList)
    {
        CBSSEntry* pBSSEntry = CBSSEntry::GetBSSEntryFromListEntry(pCurrentEntry);

        pNextEntry = pCurrentEntry->Flink;

        if (pBSSEntry->IsBssIdEqual(BssId))
        {
            RemoveEntryList(pCurrentEntry);

            if (IsBackgroundDiscoveryEnabled())
            {
                pBSSEntry->UpdateBackgroundDeviceExpired ( m_pAdapter );
            }
            delete pBSSEntry;
            NT_ASSERT(m_CurrentBSSEntryCount > 0);
            m_CurrentBSSEntryCount--;

            return;
        }

        pCurrentEntry = pNextEntry;
    }

    TraceExit(NDIS_STATUS_SUCCESS);

    return;
}


bool CBSSListManager::CheckForExpiredEntries(
    _Out_ bool *pDeleteExpiredEntries
    )
{
    *pDeleteExpiredEntries = false;

    //
    // If there are no entries, no need to check
    //
    if (IsListEmpty(&m_BSSEntryList))
    {
        return false;
    }

    //
    // If the number of entries have exceeded, go ahead with the check
    //
    if (m_CurrentBSSEntryCount > m_MaxBSSEntryCount)
    {
        *pDeleteExpiredEntries = true;
        return true;
    }

    //
    // See if we need to do the check for the background services
    //
    if ((m_BackgroundDiscoveryEnabled) &&
        (CSystem::get_CurrentTime() > (m_LastExpiredEntriesCheckTime + WFC_CONSTANT::MinimumBssEntryExpiryCheckTime)))
    {
        return true;
    }

    return false;
}


void CBSSListManager::ExpireOldEntries(
    )
{
    PLIST_ENTRY pCurrentEntry;
    PLIST_ENTRY pNextEntry;
    ULONGLONG   oldestAllowedEntry;
    bool        DeleteExpiredEntries = false;

    if (!CheckForExpiredEntries (&DeleteExpiredEntries))
    {
        return;
    }

    TraceEntry();

    m_LastExpiredEntriesCheckTime = CSystem::get_CurrentTime();

    //
    // Determine time to use for expiring AP's
    //
    oldestAllowedEntry = CSystem::get_CurrentTime();
    if (m_BssEntryExpiryTime <= oldestAllowedEntry)
        oldestAllowedEntry -= m_BssEntryExpiryTime;

    pCurrentEntry = m_BSSEntryList.Flink;

    while (pCurrentEntry != &m_BSSEntryList)
    {
        CBSSEntry* pBSSEntry = CBSSEntry::GetBSSEntryFromListEntry(pCurrentEntry);

        pNextEntry = pCurrentEntry->Flink;

        if (pBSSEntry->GetUpdateTime() < oldestAllowedEntry)
        {
            if (IsBackgroundDiscoveryEnabled())
            {
                pBSSEntry->UpdateBackgroundDeviceExpired ( m_pAdapter );
            }

            if (DeleteExpiredEntries)
            {
                RemoveEntryList(pCurrentEntry);
                delete pBSSEntry;
                NT_ASSERT(m_CurrentBSSEntryCount > 0);

                m_CurrentBSSEntryCount--;
            }
        }

        pCurrentEntry = pNextEntry;
    }

    TraceExit(NDIS_STATUS_SUCCESS);
    return;
}

NDIS_STATUS CBSSListManager::AddBSSEntry(
    _In_ CBSSEntry* bssEntry)
{
    // Set the max validity time for a BSS entry
    bssEntry->SetMaxValidityTime(m_BssEntryExpiryTime);

    ExpireOldEntries();

    // If we didnt manage to remove something deal with it
    NT_ASSERTMSG("[BSSList] BSS list has grown too big", m_CurrentBSSEntryCount < m_MaxBSSEntryCount);

    // This is same 
    InsertHeadList(
        &m_BSSEntryList,
        bssEntry->GetListEntry());
    m_CurrentBSSEntryCount++;

    return NDIS_STATUS_SUCCESS;
}

void CBSSListManager::MoveBSSEntryToHeadOfList(
    _In_ CBSSEntry* bssEntry)
{
    // Remove and then reinsert the entry at the head of the list
    RemoveEntryList( bssEntry->GetListEntry() ); 
    InsertHeadList( &m_BSSEntryList, bssEntry->GetListEntry());
}

int __cdecl BSSIDRankCompare(const void* a, const void* b)
{
    // By default qsort sorts in increasing order. We want the best candidate to be at the top of the list
    return ((*((CBSSEntry**)b))->GetCachedRank()) - ((*((CBSSEntry**)a))->GetCachedRank());
}

NDIS_STATUS CBSSListManager::FindMatchingBSSEntriesForConnect(
    _In_ BSS_ENTRY_CONNECT_MATCHING_CRITERIA* pMatchingCriteria,
    _In_ ULONG MaxAgeOfEntries100nS,
    _Inout_ ULONG* pMatchingEntryCount,
    _Out_writes_opt_(*pMatchingEntryCount) CBSSEntry** ppBssEntryList)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    PLIST_ENTRY pCurrentEntry = m_BSSEntryList.Flink;
    ULONG matchingEntryCount = 0;
    bool bCountOnly = (NULL == ppBssEntryList);
    ULONG restrictedBSSChannel = 0;
    PROAM_CONTROL_PARAMETERS pRoamControl = CConnectHelpers::GetRoamControlParameters(
        m_pAdapterCache);

    TraceEntry();

    if (pMatchingCriteria == NULL ||
        pMatchingEntryCount == NULL)
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

#if DBG    
    status = m_pAdapterCache->GetPropertyULong(
        WfcAdapterPropertyRegTest_BSSChannel,
        &restrictedBSSChannel
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        status = NDIS_STATUS_SUCCESS;
        restrictedBSSChannel = 0;
    }
#else
    // No BSS restriction support on free builds
    restrictedBSSChannel = 0;
#endif

    // Set maximum age of entries
    pMatchingCriteria->MaxAgeOfEntry = CSystem::get_CurrentTime();
    if (MaxAgeOfEntries100nS <= pMatchingCriteria->MaxAgeOfEntry)
    {
        pMatchingCriteria->MaxAgeOfEntry -= MaxAgeOfEntries100nS;
    }

    BOOLEAN bIsFTConnection;
    WFC_PORT_ID portID;
    m_pAdapter->GetPortIdForPortType(
        m_PortType,
        &portID
        );
    bIsFTConnection = m_pAdapter->GetPortPropertyCache(portID)->GetPropertyBooleanOrDefault(
        (WFC_PROPERTY_NAME)WfcPortPropertyCurrentConnectionFT,
        false
        );

    while (pCurrentEntry != &m_BSSEntryList &&
        (bCountOnly || matchingEntryCount < *pMatchingEntryCount))
    {
        CBSSEntry* pBSSEntry = CBSSEntry::GetBSSEntryFromListEntry(
            pCurrentEntry);

        if (pBSSEntry->IsMatchingBssEntryForConnect(
            pMatchingCriteria,
            bIsFTConnection))
        {
            if ((restrictedBSSChannel == 0) ||
                (pBSSEntry->GetChannelInfo()->Channel == restrictedBSSChannel))
            {
                if (!bCountOnly)
                {
                    ppBssEntryList[matchingEntryCount] = pBSSEntry;
                }

                matchingEntryCount++;
            }
            else
            {
                WFCInfo(
                    "Entry not on target channel. BSSID %!MACADDR!, ignoring candidate.", *(pBSSEntry->GetBssId()));
            }
        }

        pCurrentEntry = pCurrentEntry->Flink;
    }

    *pMatchingEntryCount = matchingEntryCount;

    if (matchingEntryCount == 0 || bCountOnly)
    {
        goto exit;
    }

    //
    // Rank candidates. This is done after we have done any matching of the APs. The APs
    // cannot be removed at this point, they will just get prioritized lower if we dont
    // like them
    //
    for (ULONG i = 0; i < matchingEntryCount; ++i)
    {        
        // Calculate the rank of the AP's taking into account the roaming parameters
        // and the trigger for the call
        ppBssEntryList[i]->CalculateRank(pMatchingCriteria, pRoamControl);
    }

    // 
    // Sort the APs
    //
    qsort((ppBssEntryList), matchingEntryCount, sizeof(CBSSEntry *), BSSIDRankCompare);

    for (ULONG i = 0; i < matchingEntryCount; ++i)
    {
        CBSSEntry* pTmp = ppBssEntryList[i];

        WFCTrace("[Connect] Selected BSSID = %!MACADDR!, Rank = %lu, Link quality = %lu, Channel = %lu",
            *(pTmp->GetBssId()), pTmp->GetCachedRank(), pTmp->GetSignalInfo()->LinkQuality, pTmp->GetChannelInfo()->Channel);

        if (pMatchingCriteria->bPreferToStayWithCurrentPeer)
        {
            // During roam, APs whose rank is below our current AP's rank, clear the Preferred flag
            if (pTmp->GetAssociatedFlag())
            {
                WFCTrace("[Roam][BetterAP] Currently connected AP is %d of %d candidates. Clearing preferred bit for lower ranked entries",
                    i+1, matchingEntryCount);
            
                for (ULONG j = i + 1; j < matchingEntryCount; j++)
                {
                    ULONG bssFlags = ppBssEntryList[j]->GetBssSelectionFlags();
                    CLEAR_FLAG(bssFlags, WDI_BSS_SELECTION_HOST_PREFERRED);
                    
                    ppBssEntryList[j]->SetBssSelectionFlags(bssFlags);
                }
            }
        }            
    }
exit:
    TraceExit(status);
    return status;
}


NDIS_STATUS CBSSListManager::FindMatchingBSSEntriesForNlo(
    _In_ WFC_BSS_ENTRY_NLO_MATCHING_CRITERIA* pMatchingCriteria,
    _In_ ULONG MaxAgeOfEntries100nS,
    _Inout_ bool* MatchFound)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    PLIST_ENTRY pCurrentEntry = m_BSSEntryList.Flink;
    DOT11_SSID bssSSID;
    ULONGLONG oldestTimeOfEntry;

    TraceEntry();

    *MatchFound = false;
    
    // Set maximum age of entries
    oldestTimeOfEntry = CSystem::get_CurrentTime();
    if (MaxAgeOfEntries100nS <= oldestTimeOfEntry)
    {
        oldestTimeOfEntry -= MaxAgeOfEntries100nS;
    }

    while (pCurrentEntry != &m_BSSEntryList)
    {
        CBSSEntry* pBSSEntry = CBSSEntry::GetBSSEntryFromListEntry(
            pCurrentEntry);

        if (pBSSEntry->GetUpdateTime() > oldestTimeOfEntry)
        {
            if (pBSSEntry->GetSSID(bssSSID) == NDIS_STATUS_SUCCESS)
            {
                for (ULONG i = 0; i < pMatchingCriteria->SSIDList.ElementCount; ++i)
                {
                    PDOT11_SSID pNloSSID = &pMatchingCriteria->SSIDList.pBuffer[i];
                    if ((pNloSSID->uSSIDLength == bssSSID.uSSIDLength) &&
                        (memcmp(
                            pNloSSID->ucSSID,
                            bssSSID.ucSSID,
                            bssSSID.uSSIDLength) == 0))
                    {
                        WFCInfo(
                            "[NLO] Matching NLO entry found BSSID %!MACADDR! SSID = %!DOT11SSID!", *(pBSSEntry->GetBssId()), pNloSSID);
                        *MatchFound = true;
                        goto exit;
                    }
                }            
            }
        }

        pCurrentEntry = pCurrentEntry->Flink;
    }

exit:
    TraceExit(status);
    return status;
}


NDIS_STATUS CBSSListManager::GetChannelListsForNeighborScan(
    _In_ CChannelListManager *pChannelListManager)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    PLIST_ENTRY pCurrentEntry = m_BSSEntryList.Flink;

    TraceEntry();

    if (pChannelListManager == NULL)
    {
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    while (pCurrentEntry != &m_BSSEntryList)
    {
        CBSSEntry* pBSSEntry = CBSSEntry::GetBSSEntryFromListEntry(
            pCurrentEntry);

        // Check if this BSS Entry was included in the neighbor report
        PDOT11_NEIGHBOR_REPORT_IE pNeighborReport = pBSSEntry->GetNeighborReport();
        if (pNeighborReport != NULL)
        {
            PWFC_BSS_ENTRY_CHANNEL_INFO pChannelInfo = pBSSEntry->GetChannelInfo();

            status = pChannelListManager->AddChannel(pChannelInfo->Channel, pChannelInfo->BandId);
            if (status != NDIS_STATUS_SUCCESS)
            {
                goto exit;
            }
        }

        pCurrentEntry = pCurrentEntry->Flink;
    }

exit:
    TraceExit(status);
    return status;
}


bool
CBSSListManager::HasNewerBssEntries(
    _In_  ULONGLONG oldestAllowedEntry
    )
{
    PLIST_ENTRY         pCurrentEntry = NULL;

    pCurrentEntry = (m_BSSEntryList.Flink);
    while (pCurrentEntry != &m_BSSEntryList)
    {
        CBSSEntry* pBSSEntry = CBSSEntry::GetBSSEntryFromListEntry(pCurrentEntry);
        pCurrentEntry = pCurrentEntry->Flink;

        //
        // Ignore stale entries
        //
        if (pBSSEntry->GetUpdateTime() < oldestAllowedEntry)
        {
            continue;
        }

        // Valid BSS entry found
        return true;
    }

    return false;
}

NDIS_STATUS
CBSSListManager::GetDot11BSSEntryList(
    _In_  ULONGLONG oldestAllowedEntry,
    _Inout_updates_bytes_(TotalLength) PDOT11_BYTE_ARRAY Dot11ByteArray,
    _In_  ULONG TotalLength
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PLIST_ENTRY         pCurrentEntry = NULL;
    ULONG               bssEntrySize = 0;
    PUCHAR              pCurrPtr;
    ULONG               unreportedEntryCount = 0, reportedEntryCount = 0;

    if (TotalLength < FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer))
    {
        WFCError("Invalid length %d provided for GetDot11BSSEntryList", TotalLength);
        return NDIS_STATUS_INVALID_LENGTH;
    }

    ASSIGN_NDIS_OBJECT_HEADER(Dot11ByteArray->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_BSS_ENTRY_BYTE_ARRAY_REVISION_1,
        sizeof(DOT11_BYTE_ARRAY));

    Dot11ByteArray->uNumOfBytes = 0;
    Dot11ByteArray->uTotalNumOfBytes = 0;

    TotalLength = TotalLength
        - FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer);
    pCurrPtr = Dot11ByteArray->ucBuffer;

    pCurrentEntry = (m_BSSEntryList.Flink);
    while (pCurrentEntry != &m_BSSEntryList)
    {
        CBSSEntry* pBSSEntry = CBSSEntry::GetBSSEntryFromListEntry(pCurrentEntry);
        pCurrentEntry = pCurrentEntry->Flink;

        //
        // Ignore stale entries
        //
        if (pBSSEntry->GetUpdateTime() < oldestAllowedEntry)
        {
            continue;
        }

        //
        // Copy the BSS entry
        //
        ndisStatus = pBSSEntry->CopyDot11BSSEntry(
            pCurrPtr,
            TotalLength,
            &bssEntrySize
            );
        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            // Move pointer
            pCurrPtr += bssEntrySize;
            TotalLength -= bssEntrySize;
            Dot11ByteArray->uNumOfBytes += bssEntrySize;
            reportedEntryCount++;
        }
        else if (ndisStatus == NDIS_STATUS_BUFFER_OVERFLOW)
        {
            TotalLength = 0;
            //
            // We continue walking through the list to determine the total
            // space required for this OID
            //
            unreportedEntryCount++;
        }
        else
        {
            // Something went wrong
            WFCError("Failed to copy BSS entry (0x%x)", ndisStatus);
            break;
        }

        // Update total number of bytes needed to report to the OS
        Dot11ByteArray->uTotalNumOfBytes += bssEntrySize;

    }

    WFCInfo("[BSSList] Found %d reportable entries. Reported %d of those", reportedEntryCount + unreportedEntryCount, reportedEntryCount);

    return ndisStatus;
}


NDIS_STATUS
CBSSListManager::CopyWfdDevicesList(
    _Inout_updates_bytes_(TotalLength) PBYTE pDot11Buffer,
    _In_  ULONG TotalLength,
    _Out_ ULONG *pNumEntries,
    _Out_ ULONG *pTotalListEntries,
    _Out_ ULONG *pBytesCopied,
    _Out_ ULONG *pTotalListLength
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    NTSTATUS            ntStatus;
    PLIST_ENTRY         pCurrentEntry = NULL;
    ULONG               bssEntrySize = 0;
    ULONG               uNumEntries, uBytesCopied, uTotalListLength, uTotalListEntries;
    PUCHAR              pCurrPtr;
    ULONGLONG           oldestAllowedEntry;

    if (IsBackgroundDiscoveryEnabled())
    {
        //
        // Clear the Last notified state for WFD services as we will reset it below
        //
        m_pAdapter->GetServicesManager()->InitializeForP2PDeviceQuery ();
    }

    //
    // Determine time to use for expiring AP's
    //
    oldestAllowedEntry = CSystem::get_CurrentTime();
    if (m_BssEntryExpiryTime <= oldestAllowedEntry)
    {
        oldestAllowedEntry -= m_BssEntryExpiryTime;
    }

    uBytesCopied = 0;
    uNumEntries = 0;
    uTotalListLength = 0;
    uTotalListEntries = 0;
    pCurrPtr = pDot11Buffer;
    pCurrentEntry = (m_BSSEntryList.Flink);
    while (pCurrentEntry != &m_BSSEntryList)
    {
        CBSSEntry* pBSSEntry = CBSSEntry::GetBSSEntryFromListEntry(pCurrentEntry);
        pCurrentEntry = pCurrentEntry->Flink;

        //
        // Ignore stale entries, or entries which don't have a probe
        // request and response (channel hint entries).
        //
        if (pBSSEntry->GetUpdateTime() < oldestAllowedEntry ||
            !pBSSEntry->IsWFDFilledIn(true))
        {
            continue;
        }

        if (uNumEntries >= DOT11_WFD_DISCOVER_COMPLETE_MAX_LIST_SIZE)
        {
            break;
        }

        //
        // Copy the BSS entry
        //
        ndisStatus = pBSSEntry->CopyDot11WfdBSSEntry(
            pCurrPtr,
            TotalLength,
            &bssEntrySize
            );
        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            //
            // If there was no beacon or probe respone in the BSSEntry, then nothing would have been copied
            // So, just skip over those entries
            //
            if (bssEntrySize)
            {
                // Move pointer
                pCurrPtr += bssEntrySize;
                TotalLength -= bssEntrySize;

                uBytesCopied += bssEntrySize;
                uNumEntries++;

                if (IsBackgroundDiscoveryEnabled())
                {
                    pBSSEntry->UpdateBackgroundDeviceLastQueriedTime( m_pAdapter );
                }
            }
        }
        else if (ndisStatus == NDIS_STATUS_BUFFER_OVERFLOW)
        {
            TotalLength = 0;
            //
            // We continue walking through the list to determine the total
            // space required for this OID
            //
        }
        else
        {
            // Something went wrong
            WFCError("Failed to copy BSS entry (0x%x)", ndisStatus);
            break;
        }

        ntStatus = RtlULongAdd( uTotalListLength, bssEntrySize, &uTotalListLength );
        if (!NT_SUCCESS(ntStatus))
        {
            ndisStatus = NTSTATUS2NDISSTATUS(ntStatus);
            WFCError("Size exceeded MaxSize available for uTotalListLength: %d + %d",
                uTotalListLength, bssEntrySize);
            break;
        }

        uTotalListEntries++;
    }

    if (IsBackgroundDiscoveryEnabled())
    {
        //
        // Start/Reset the 5-minute timer for Devices expiring
        //
        m_pAdapter->GetServicesManager()->StartDeviceExpiryNotificationTimer ();
    }

    if (pNumEntries)
    {
        *pNumEntries = uNumEntries;
    }
    if (pBytesCopied)
    {
        *pBytesCopied = uBytesCopied;
    }
    if (pTotalListEntries)
    {
        *pTotalListEntries = uTotalListEntries;
    }
    if (pTotalListLength)
    {
        *pTotalListLength = uTotalListLength;
    }

    return ndisStatus;

}

NDIS_STATUS
CBSSListManager::GetDot11WfdDeviceEntryList(
    _Inout_updates_bytes_(TotalLength) PDOT11_BYTE_ARRAY Dot11ByteArray,
    _In_  ULONG TotalLength
    )
{
    if (TotalLength < FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer))
    {
        WFCError("Invalid length %d provided for %!FUNC!", TotalLength);
        return NDIS_STATUS_INVALID_LENGTH;
    }

    ASSIGN_NDIS_OBJECT_HEADER(Dot11ByteArray->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_DEVICE_ENTRY_BYTE_ARRAY_REVISION_1,
        sizeof(DOT11_BYTE_ARRAY));

    Dot11ByteArray->uNumOfBytes = 0;
    Dot11ByteArray->uTotalNumOfBytes = 0;

    return CopyWfdDevicesList(
        Dot11ByteArray->ucBuffer,
        (TotalLength - FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer)),
        NULL,
        NULL,
        &Dot11ByteArray->uNumOfBytes,
        &Dot11ByteArray->uTotalNumOfBytes
        );
}


NDIS_STATUS
CBSSListManager::GetWfdDiscoverRequestResultList(
_Out_writes_bytes_(TotalLength)
PDOT11_WFD_DISCOVER_COMPLETE_PARAMETERS pDiscoverCompleteParams,
_Inout_ ULONG TotalLength)
{
    if (TotalLength < sizeof(*pDiscoverCompleteParams))
    {
        WFCError("Invalid length %d provided for %!FUNC!", TotalLength);
        return NDIS_STATUS_INVALID_LENGTH;
    }

    ASSIGN_NDIS_OBJECT_HEADER(
        pDiscoverCompleteParams->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_WFD_DISCOVER_COMPLETE_PARAMETERS_REVISION_1,
        sizeof(*pDiscoverCompleteParams));

    pDiscoverCompleteParams->uNumOfEntries = 0;
    pDiscoverCompleteParams->uTotalNumOfEntries = 0;
    pDiscoverCompleteParams->uListOffset = sizeof(*pDiscoverCompleteParams);
    pDiscoverCompleteParams->uListLength = 0;

    return CopyWfdDevicesList(
        ((BYTE*)pDiscoverCompleteParams) + sizeof(*pDiscoverCompleteParams),
        (TotalLength - sizeof(*pDiscoverCompleteParams)),
        &pDiscoverCompleteParams->uNumOfEntries,
        &pDiscoverCompleteParams->uTotalNumOfEntries,
        NULL,
        &pDiscoverCompleteParams->uListLength
        );
}

//==============================================================================================

CChannelListManager::CChannelListManager() :
    m_ChannelList24GHzCount(0),
    m_ChannelList5GHzCount(0)
{
    RtlZeroMemory(m_ChannelList24GHz, sizeof(m_ChannelList24GHz));
    RtlZeroMemory(m_ChannelList5GHz, sizeof(m_ChannelList5GHz));
}

CChannelListManager::~CChannelListManager()
{

}

ULONG CChannelListManager::GetNumberOfChannels(UINT32 band)
{
    ULONG channelCount = 0;

    if (band == WDI_BAND_ID_2400)
    {
        channelCount = m_ChannelList24GHzCount;
    }
    else if (band == WDI_BAND_ID_5000)
    {
        channelCount = m_ChannelList5GHzCount;
    }

    return channelCount;
}

WDI_CHANNEL_NUMBER * CChannelListManager::GetChannelList(UINT32 band)
{
    WDI_CHANNEL_NUMBER * channelList = NULL;

    if (band == WDI_BAND_ID_2400)
    {
        channelList = m_ChannelList24GHz;
    }
    else if (band == WDI_BAND_ID_5000)
    {
        channelList = m_ChannelList5GHz;
    }

    return channelList;
}


NDIS_STATUS CChannelListManager::AddChannel(UINT32 channel, UINT32 band)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_CHANNEL_NUMBER * pChannelList = NULL;
    PULONG pChannelListCount = NULL;

    TraceEntry();

    if (band == WDI_BAND_ID_2400)
    {
        pChannelList = m_ChannelList24GHz;
        pChannelListCount = &m_ChannelList24GHzCount;
    }
    else if (band == WDI_BAND_ID_5000)
    {
        pChannelList = m_ChannelList5GHz;
        pChannelListCount = &m_ChannelList5GHzCount;
    }
    else
    {
        // We dont support this band, so dont add this in        
        WFCTrace("[ChannelHint][NeighborReport] Not using non 2.4/5GHz B:%d C:%d", band, channel);
        goto exit;
    }
    
    // Check if channel already added to list
    for (UINT32 i = 0; i < *pChannelListCount; i++)
    {
        if (pChannelList[i] == channel)
        {
            goto exit;
        }
    }

    // Add channel if channel list isn't full
    if (*pChannelListCount < c_MaxNeighborScanChannelListCount)
    {
        pChannelList[(*pChannelListCount)++] = channel;
    }

exit:
    TraceExit(status);
    return status;
}

//==============================================================================================
// Services Manager - Handle background discovery and responses to Service advertisements
//==============================================================================================

//
// Comeback Response Context
//
CServiceComebackResponseContext::CServiceComebackResponseContext(
    _In_ const USHORT MaxResponseFragmentSize,
    _In_ const WDI_MAC_ADDRESS *pSourceAddress,
    _In_ const UCHAR DialogToken,
    _In_ const DOT11_AVERTISEMENT_PROTOCOL_ELEMENT *pAdvertisementProtocol,
    _In_ USHORT uQueryResponseLength,
    _In_reads_bytes_(uQueryResponseLength) PBYTE pQueryResponse
    ) :
    m_uNextFragmentID(0)
{
    m_MaxResponseFragmentSize = MaxResponseFragmentSize;
    RtlCopyMemory(&m_SourceAddress, pSourceAddress, sizeof(WDI_MAC_ADDRESS));
    m_DialogToken = DialogToken;
    m_AdvertisementProtocol = *pAdvertisementProtocol;
    m_uQueryResponseLength = uQueryResponseLength;
    m_pQueryResponse = pQueryResponse;

    m_BytesLeft = uQueryResponseLength;
    m_pNextQueryResponse = pQueryResponse;

    InitializeCppListEntry(this, &m_ComebackResponseListEntry);
    SetLastTouchedTime ();
}

CServiceComebackResponseContext::~CServiceComebackResponseContext()
{
    if (m_pQueryResponse)
    {
        delete [] m_pQueryResponse;
        m_pQueryResponse = NULL;
    }
}

void
CServiceComebackResponseContext::UpdateContext(
    _In_ DOT11_AVERTISEMENT_PROTOCOL_ELEMENT *pAdvertisementProtocol,
    _In_ USHORT uQueryResponseLength,
    _In_reads_bytes_(uQueryResponseLength) PBYTE pQueryResponse)
{
    if (m_pQueryResponse)
    {
        delete [] m_pQueryResponse;
    }
    m_AdvertisementProtocol = *pAdvertisementProtocol;
    m_pQueryResponse = pQueryResponse;
    m_uQueryResponseLength = uQueryResponseLength;
}


NDIS_STATUS
CServiceComebackResponseContext::GetNextFragmentToSend(
    _Out_ UCHAR *puAdvertisementProtocolID,
    _Out_ DOT11_ANQP_FRAGMENT_ID *pFragmentID,
    _Out_ USHORT *puQueryResponseLength,
    _Outptr_result_bytebuffer_(*puQueryResponseLength) BYTE** ppQueryResponse)
{
    USHORT uQueryResponseLength;
    DOT11_ANQP_FRAGMENT_ID FragmentID = {0};

    if (!m_BytesLeft)
    {
        return (NDIS_STATUS_INVALID_DATA);
    }

    FragmentID.FragmentID = (m_uNextFragmentID & 0x7F);
    if (m_BytesLeft > m_MaxResponseFragmentSize)
    {
        uQueryResponseLength = m_MaxResponseFragmentSize;
        FragmentID.MoreFragments = 1;
    }
    else
    {
        uQueryResponseLength = m_BytesLeft;
        FragmentID.MoreFragments = 0;
    }

    WFCTrace("GetNextFragmentToSend: [TrackANQP] - FragmentID=<0x%x - 0x%x>, uQueryResponseLength=%d\n",
        m_uNextFragmentID, FragmentID.Value, uQueryResponseLength);

    *puAdvertisementProtocolID = m_AdvertisementProtocol.AdvertisementProtocol[0].AdvertisementProtocolID;
    *pFragmentID = FragmentID;
    *puQueryResponseLength = uQueryResponseLength;
    *ppQueryResponse = m_pNextQueryResponse;

    m_uNextFragmentID++;
    m_BytesLeft -= uQueryResponseLength;
    m_pNextQueryResponse += uQueryResponseLength;

    return (NDIS_STATUS_SUCCESS);
}


//==============================================================================================

CServicesManager::CServicesManager(
    ) :
    m_MaxResponseFragmentSize(1400),
    m_P2PBackgroundNotificationsEnabled(false),
    m_pAdapter(NULL),
    m_pAdapterCache(NULL),
    m_ExpiredDevicesCheckTimerToken(WFC_TIMER_REGISTRATION_TOKEN_INVALID),
    m_DeviceUpdateNotificationTimerToken(WFC_TIMER_REGISTRATION_TOKEN_INVALID),
    m_DeviceAndServiceUpdateLock(NULL),
    m_NdisDevicePortNumber(0),
    m_NotifyAllDevicesAndServices(false),
    m_DeviceUpdateNotificationTimerRunning(false),
    m_DeviceExpiryNotificationTimerRunning(false),
    m_WaitingForP2PDeviceQuery(false),
    m_CurrentDiscoveryContext(0),
    m_AllCurrentDevices(0),
    m_AllNewDevices(0),
    m_AllExpiredDevices(0),
    m_AllUpdatedServices(0),
    m_VisibleFilteredDevices(0),
    m_VisibleFilteredServices(0),
    m_TrackedP2PDevicesCount(0),
    m_TrackedP2PServicesCount(0),
    m_PendingComebackResponseCount(0)
{
    C_ASSERT (WFC_CONSTANT::DefaultWfdDeviceNotVisibleTimeoutNSecs >= WFC_CONSTANT::DefaultBssEntryExpiryTime);

    m_WfdDeviceExpiryTimeNSecs = WFC_CONSTANT::DefaultWfdDeviceNotVisibleTimeoutNSecs;
    m_WfdDeviceExpiryCheckTimeoutMSecs = WFC_CONSTANT::DefaultWfdDeviceNotVisibleCheckIntervalMSecs +
                                         WFC_CONSTANT::DefaultWfdDeviceNotVisibleDelayIntervalMSecs;
    m_WfdDeviceUpdateNotificationDelayTimeoutMSecs = WFC_CONSTANT::DefaultWfdDeviceOrServiceUpdateDelayMSecs;

    InitializeListHead(&m_TrackedP2PDevicesList);
    InitializeListHead(&m_TrackedP2PServicesList);
    InitializeListHead(&m_PendingComebackResponseList);
}

CServicesManager::~CServicesManager()
{
    FlushComebackResponseContexts ();
    FlushDiscoveryContexts ();

    if (m_DeviceUpdateNotificationTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        m_pAdapter->GetEventQueue()->DeregisterTimeoutCallback(
            m_DeviceUpdateNotificationTimerToken );
        m_DeviceUpdateNotificationTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    if (m_ExpiredDevicesCheckTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        m_pAdapter->GetEventQueue()->DeregisterTimeoutCallback(
            m_ExpiredDevicesCheckTimerToken );
        m_ExpiredDevicesCheckTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    if (m_DeviceAndServiceUpdateLock)
    {
        delete(m_DeviceAndServiceUpdateLock);
    }
}

NDIS_STATUS
CServicesManager::Initialize(
    _In_ CAdapter* pAdapter,
    _In_ CAdapterPropertyCache *pAdapterCache
    )
{
    m_pAdapter = pAdapter;
    m_pAdapterCache = pAdapterCache;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    ndisStatus = CNdisSpinLock::CreateInstance(&m_DeviceAndServiceUpdateLock);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("CServicesManager::Initialize: Unable to initialize DeviceAndServiceUpdateLock");
        goto Exit;
    }

Exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

//
// Comeback Response context
//
void
CServicesManager::ExpireOldComebackEntries()
{
    PLIST_ENTRY pCurrentEntry;
    PLIST_ENTRY pNextEntry;
    ULONGLONG   oldestAllowedEntry;

    TraceEntry();

    //
    // Determine time to use for expiring AP's
    //
    oldestAllowedEntry = CSystem::get_CurrentTime();
    if (WFC_CONSTANT::DefaultServiceComebackEntryExpiryTime <= oldestAllowedEntry)
    {
        oldestAllowedEntry -= WFC_CONSTANT::DefaultServiceComebackEntryExpiryTime;
    }

    pCurrentEntry = m_PendingComebackResponseList.Flink;
    while (pCurrentEntry != &m_PendingComebackResponseList)
    {
        CServiceComebackResponseContext* pServiceComebackResponseContext =
            CServiceComebackResponseContext::GetComebackResponseContextFromListEntry(pCurrentEntry);
        pNextEntry = pCurrentEntry->Flink;

        if (pServiceComebackResponseContext->IsEntryExpired(oldestAllowedEntry))
        {
            RemoveEntryList(pCurrentEntry);
            delete pServiceComebackResponseContext;

            NT_ASSERT(m_PendingComebackResponseCount > 0);
            m_PendingComebackResponseCount--;
        }

        pCurrentEntry = pNextEntry;
    }

    TraceExit(NDIS_STATUS_SUCCESS);

    return;
}


void
CServicesManager::FlushComebackResponseContexts()
{
    PLIST_ENTRY pCurrentEntry;

    TraceEntry();

    while (!IsListEmpty (&m_PendingComebackResponseList))
    {
        pCurrentEntry = RemoveHeadList (&m_PendingComebackResponseList);
        CServiceComebackResponseContext* pServiceComebackResponseContext =
            CServiceComebackResponseContext::GetComebackResponseContextFromListEntry(pCurrentEntry);

        delete pServiceComebackResponseContext;

        NT_ASSERT(m_PendingComebackResponseCount > 0);
        m_PendingComebackResponseCount--;
    }

    TraceExit(NDIS_STATUS_SUCCESS);

    return;
}

CServiceComebackResponseContext*
CServicesManager::FindComebackResponseContext(
    _In_ WDI_MAC_ADDRESS *pSourceAddress,
    _In_ UCHAR DialogToken)
{
    PLIST_ENTRY pCurrentEntry;

    TraceEntry();

    pCurrentEntry = m_PendingComebackResponseList.Flink;
    while (pCurrentEntry != &m_PendingComebackResponseList)
    {
        CServiceComebackResponseContext* pServiceComebackResponseContext =
            CServiceComebackResponseContext::GetComebackResponseContextFromListEntry(pCurrentEntry);
        if (pServiceComebackResponseContext->IsComebackResponseMatch(pSourceAddress, DialogToken))
        {
            return pServiceComebackResponseContext;
        }

        pCurrentEntry = pCurrentEntry->Flink;
    }

    TraceExit(NDIS_STATUS_SUCCESS);
    return NULL;
}


void
CServicesManager::RemoveComebackResponseContext(
     _In_ CServiceComebackResponseContext* pServiceComebackResponseContext)
{
    TraceEntry();

    RemoveEntryList (pServiceComebackResponseContext->GetListEntry());
    delete pServiceComebackResponseContext;

    NT_ASSERT(m_PendingComebackResponseCount > 0);
    m_PendingComebackResponseCount--;

    TraceExit(NDIS_STATUS_SUCCESS);

    return;
}


CServiceComebackResponseContext*
CServicesManager::AddComebackResponseContext(
    _In_ WDI_MAC_ADDRESS *pSourceAddress,
    _In_ UCHAR DialogToken,
    _In_ DOT11_AVERTISEMENT_PROTOCOL_ELEMENT *pAdvertisementProtocol,
    _In_ USHORT uQueryResponseLength,
    _In_reads_bytes_(uQueryResponseLength) PBYTE pQueryResponse)
{
    CServiceComebackResponseContext *pServiceComebackResponseContext;

    //
    // See if there is already an existing entry
    //
    pServiceComebackResponseContext = FindComebackResponseContext( pSourceAddress, DialogToken );
    if (pServiceComebackResponseContext)
    {
        pServiceComebackResponseContext->UpdateContext (pAdvertisementProtocol, uQueryResponseLength, pQueryResponse);
    }
    else
    {
        pServiceComebackResponseContext = new CServiceComebackResponseContext(
            m_MaxResponseFragmentSize,
            pSourceAddress,
            DialogToken,
            pAdvertisementProtocol,
            uQueryResponseLength,
            pQueryResponse);
        if (!pServiceComebackResponseContext)
        {
            return NULL;
        }

        InsertTailList( &m_PendingComebackResponseList, pServiceComebackResponseContext->GetListEntry());
        m_PendingComebackResponseCount++;

        ExpireOldComebackEntries();
    }

    return pServiceComebackResponseContext;
}

//
// P2P Background Discovery Contexts
//
void
CServicesManager::FlushDiscoveryContexts()
{
    PLIST_ENTRY pCurrentEntry;

    TraceEntry();

    //
    // Stop the Timer if it is running
    //
    if (WFC_TIMER_REGISTRATION_TOKEN_INVALID != m_DeviceUpdateNotificationTimerToken)
    {
        WFCTrace("CServicesManager::FlushDiscoveryContexts: STOPPING DeviceUpdateNotification Timer ...");

        m_pAdapter->GetEventQueue()->DeregisterTimeoutCallback( m_DeviceUpdateNotificationTimerToken );
        m_DeviceUpdateNotificationTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    if (WFC_TIMER_REGISTRATION_TOKEN_INVALID != m_ExpiredDevicesCheckTimerToken)
    {
        WFCTrace("CServicesManager::FlushDiscoveryContexts: STOPPING ExpiredDevicesCheck Timer ...");

        m_pAdapter->GetEventQueue()->DeregisterTimeoutCallback( m_ExpiredDevicesCheckTimerToken );
        m_ExpiredDevicesCheckTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
    }

    while (!IsListEmpty (&m_TrackedP2PDevicesList))
    {
        pCurrentEntry = RemoveHeadList (&m_TrackedP2PDevicesList);
        CP2PBackgroundDeviceContext* pP2PDeviceContext = (CP2PBackgroundDeviceContext*)
            CP2PBackgroundDeviceContext::GetP2PBackgroundContextFromListEntry (pCurrentEntry);

        delete pP2PDeviceContext;

        NT_ASSERT(m_TrackedP2PDevicesCount > 0);
        m_TrackedP2PDevicesCount--;
    }

    while (!IsListEmpty (&m_TrackedP2PServicesList))
    {
        pCurrentEntry = RemoveHeadList (&m_TrackedP2PServicesList);
        CP2PBackgroundServiceContext* pP2PServiceContext = (CP2PBackgroundServiceContext*)
            CP2PBackgroundServiceContext::GetP2PBackgroundContextFromListEntry (pCurrentEntry);

        delete pP2PServiceContext;

        NT_ASSERT(m_TrackedP2PServicesCount > 0);
        m_TrackedP2PServicesCount--;
    }

    NT_ASSERT ((0 == m_TrackedP2PDevicesCount) &&
               (0 == m_TrackedP2PServicesCount));

    m_CurrentDiscoveryContext++;
    if (!m_CurrentDiscoveryContext)
    {
        //
        // Skip over 0 in case we wrap around
        m_CurrentDiscoveryContext++;
    }
    m_AllCurrentDevices = 0;
    m_AllNewDevices = 0;
    m_AllExpiredDevices = 0;
    m_AllUpdatedServices = 0;

    m_TrackedP2PDevicesCount = 0;
    m_TrackedP2PServicesCount = 0;
    m_VisibleFilteredDevices = 0;
    m_VisibleFilteredServices = 0;

    TraceExit(NDIS_STATUS_SUCCESS);

    return;
}


NDIS_STATUS
CServicesManager::SetNewBackgroundDiscoveryContexts(
    _In_ BOOLEAN NotifyAllDevicesAndServices,
    _In_ ULONG DiscoveryBufferLength,
    _In_reads_bytes_(DiscoveryBufferLength) PBYTE pDiscoveryBuffer)
/*++
    Routine Description:
        This routine is called when P2P plumbs the Background Discovery List.
        On being called, the old list is flushed and new list configured

    Arguments:

    Return Value:
--*/
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG   i, uOffset, uBytesLeft;
    DOT11_MAC_ADDRESS *pDeviceAddress;
    PDOT11_WFD_SERVICE_NAME pServiceName;
    DOT11_WFD_BACKGROUND_DISCOVERY_REQUEST *pDot11WFDDiscovery =
                                           (DOT11_WFD_BACKGROUND_DISCOVERY_REQUEST *) pDiscoveryBuffer;

    TraceEntry();

    //
    // First, flush the existing contexts
    //
    FlushDiscoveryContexts ();

    status = m_pAdapter->GetNdisPortNumberForPortType(WfcPortTypeWFDDevice, &m_NdisDevicePortNumber);
    if (NDIS_STATUS_SUCCESS != status)
    {
        WFCError("Couldn't find Device port, Status=0x%x.", status);
        goto Exit;
    }

    WFCTrace("SetNewBackgroundDiscoveryContexts: %d Device filters specified\n", pDot11WFDDiscovery->uNumDeviceFilters);
    uOffset = pDot11WFDDiscovery->uDeviceFilterListOffset;
    for (i=0; i<pDot11WFDDiscovery->uNumDeviceFilters; i++)
    {
        pDeviceAddress = (PDOT11_MAC_ADDRESS) (((PBYTE) pDot11WFDDiscovery) + uOffset);

        CP2PBackgroundDeviceContext *pP2PDeviceContext = new CP2PBackgroundDeviceContext(pDeviceAddress);
        if (!pP2PDeviceContext)
        {
            break;
        }

        WFCTrace("\t[%d] <%!MACADDR!>", m_TrackedP2PDevicesCount, *pP2PDeviceContext->GetContext());

        InsertTailList( &m_TrackedP2PDevicesList, pP2PDeviceContext->GetListEntry());
        m_TrackedP2PDevicesCount++;

        uOffset += sizeof(DOT11_MAC_ADDRESS);
    }

    if ((pDot11WFDDiscovery->uNumServiceNames) &&
        (DiscoveryBufferLength > pDot11WFDDiscovery->uServiceNamesListOffset))
    {
        uBytesLeft = DiscoveryBufferLength - pDot11WFDDiscovery->uServiceNamesListOffset;
        uOffset = pDot11WFDDiscovery->uServiceNamesListOffset;

        WFCTrace("SetNewBackgroundDiscoveryContexts: %d Service Names specified\n",
            pDot11WFDDiscovery->uNumServiceNames);
        pServiceName = (PDOT11_WFD_SERVICE_NAME) (((PBYTE) pDot11WFDDiscovery) + uOffset);
        while ((m_TrackedP2PServicesCount < pDot11WFDDiscovery->uNumServiceNames) &&
               (uBytesLeft >= sizeof(DOT11_WFD_SERVICE_NAME)) &&
               (uBytesLeft >= ((FIELD_OFFSET(DOT11_WFD_SERVICE_NAME,strUtf8ServiceName) + (ULONG) pServiceName->uUtf8ServiceNameLength))))
        {
            CP2PBackgroundServiceContext *pP2PServiceContext = new CP2PBackgroundServiceContext(
                pServiceName->uUtf8ServiceNameLength,
                pServiceName->strUtf8ServiceName);
            if (!pP2PServiceContext)
            {
                break;
            }

            WFCTrace("\t[%d] <%d:%s>\n",
                m_TrackedP2PServicesCount, pServiceName->uUtf8ServiceNameLength,
                (PCHAR) pP2PServiceContext->GetContext());

            InsertTailList( &m_TrackedP2PServicesList, pP2PServiceContext->GetListEntry());
            m_TrackedP2PServicesCount++;

            uOffset += FIELD_OFFSET(DOT11_WFD_SERVICE_NAME,strUtf8ServiceName) + pServiceName->uUtf8ServiceNameLength;
            uBytesLeft -= (FIELD_OFFSET(DOT11_WFD_SERVICE_NAME,strUtf8ServiceName) +
                           pServiceName->uUtf8ServiceNameLength);
            pServiceName = (PDOT11_WFD_SERVICE_NAME) (((PBYTE) pDot11WFDDiscovery) + uOffset);
        }
    }

    NT_ASSERT (pDot11WFDDiscovery->uNumServiceNames == m_TrackedP2PServicesCount);

    m_NotifyAllDevicesAndServices = NotifyAllDevicesAndServices;
    if ((m_NotifyAllDevicesAndServices) ||
        (m_TrackedP2PDevicesCount) ||
        (m_TrackedP2PServicesCount))
    {
        m_P2PBackgroundNotificationsEnabled = true;
    }
    else
    {
        m_P2PBackgroundNotificationsEnabled = false;
    }

    m_pAdapter->GetBSSListManager(WfcPortTypeWFDDevice)->SetBackgroundDiscoveryEnabled (m_P2PBackgroundNotificationsEnabled);

Exit:
    TraceExit(status);
    return (status);
}


ULONG
CServicesManager::CheckForExpiredDevicesAndServices(
    _In_ ULONGLONG currentTime
    )
{
    PLIST_ENTRY pCurrentEntry;
    ULONG uNumExpiredDevices = 0, uNumExpiredServices = 0;

    pCurrentEntry = m_TrackedP2PDevicesList.Flink;
    while (pCurrentEntry != &m_TrackedP2PDevicesList)
    {
        CP2PBackgroundDeviceContext* pP2PDeviceContext = (CP2PBackgroundDeviceContext*)
            CP2PBackgroundDeviceContext::GetP2PBackgroundContextFromListEntry (pCurrentEntry);

        pCurrentEntry = pCurrentEntry->Flink;

        if (pP2PDeviceContext->CheckForNotVisibleStateChange (currentTime, m_WfdDeviceExpiryTimeNSecs))
        {
            uNumExpiredDevices++;
            m_VisibleFilteredDevices--;
            WFCTrace("\t----- [BkD - Check] Device <%!MACADDR!> is now NO longer visible -----\n",
                *pP2PDeviceContext->GetContext());
        }
    }

    pCurrentEntry = m_TrackedP2PServicesList.Flink;
    while (pCurrentEntry != &m_TrackedP2PServicesList)
    {
        CP2PBackgroundServiceContext* pP2PServiceContext = (CP2PBackgroundServiceContext*)
            CP2PBackgroundServiceContext::GetP2PBackgroundContextFromListEntry (pCurrentEntry);

        pCurrentEntry = pCurrentEntry->Flink;

        if (pP2PServiceContext->CheckForNotVisibleStateChange (currentTime, m_WfdDeviceExpiryTimeNSecs))
        {
            uNumExpiredServices++;
            m_VisibleFilteredServices--;
            WFCTrace("\t----- [BkD - Check] Service <%s> is now NO longer visible -----\n",
                (PCHAR) pP2PServiceContext->GetContext());
        }
    }

    WFCTrace("\tCheckForExpiredDevicesAndServices: Found %d+%d expired Devices+Services",
        uNumExpiredDevices, uNumExpiredServices);

    return (uNumExpiredDevices + uNumExpiredServices);
}

void
CServicesManager::OnTimerCallback(
    _In_opt_ WFC_TIMER_CONTEXT pvContext
    )
{
    BOOLEAN bSendIndication = FALSE;
    BOOLEAN bRestartDeviceExpiryTimer = FALSE;
    ULONG uNumExpiredEntries;
    DOT11_WFD_VISIBLE_ENTRIES VisibleEntries;

    TraceEntry();

    {   // m_DeviceAndServiceUpdateLock
        CAutoLock lock(m_DeviceAndServiceUpdateLock);

        NT_ASSERT (!m_WaitingForP2PDeviceQuery);

        if (&m_DeviceUpdateNotificationTimerToken == pvContext)
        {
            NT_ASSERT (!m_DeviceExpiryNotificationTimerRunning);

            m_DeviceUpdateNotificationTimerRunning = false;
            bSendIndication = TRUE;
            WFCTrace("--> CServicesManager::OnTimerCallback: INDICATING DeviceUpdate ...");
        }
        else if (&m_ExpiredDevicesCheckTimerToken == pvContext)
        {
            NT_ASSERT (!m_DeviceUpdateNotificationTimerRunning);
            m_DeviceExpiryNotificationTimerRunning = false;

            if (m_AllExpiredDevices)
            {
                if (m_NotifyAllDevicesAndServices)
                {
                    bSendIndication = TRUE;
                    WFCTrace("--> CServicesManager::OnTimerCallback: INDICATING DeviceExpiry - NotifyAll, Expired=%d",
                        m_AllExpiredDevices);
                }
                else
                {
                    uNumExpiredEntries = CheckForExpiredDevicesAndServices( CSystem::get_CurrentTime() );
                    if (uNumExpiredEntries)
                    {
                        bSendIndication = TRUE;
                        WFCTrace("--> CServicesManager::OnTimerCallback: INDICATING DeviceExpiry - ExpiredFiltered=%d",
                            m_AllExpiredDevices);
                    }
                    else if ((m_VisibleFilteredDevices) ||
                             (m_VisibleFilteredServices))
                    {
                        bRestartDeviceExpiryTimer = TRUE;
                        WFCTrace("--> CServicesManager::OnTimerCallback: NOT indicating  DeviceExpiry - RESTARTing timer as NumVisible=<%d+%d>", m_VisibleFilteredDevices, m_VisibleFilteredServices);
                    }
                    else
                    {
                        WFCTrace("--> CServicesManager::OnTimerCallback: NOT indicating DeviceExpiry as AllExpired and VisibleFiltered=0");
                    }
                }
            }
            else if (m_AllCurrentDevices)
            {
                bRestartDeviceExpiryTimer = TRUE;
                WFCTrace("--> CServicesManager::OnTimerCallback: NOT indicating  DeviceExpiry - RESTARTing timer as AllCurrentDevices=<%d>", m_AllCurrentDevices);
            }
            else
            {
                WFCTrace("--> CServicesManager::OnTimerCallback: NOT indicating DeviceExpiry as AllExpired and AllCurrent=0");
            }
        }
        else
        {
            NT_ASSERT(0);  // Unknown timer context
        }

        if (bSendIndication)
        {
            m_WaitingForP2PDeviceQuery = true;

            VisibleEntries.uNumAllCurrentDevices = m_AllCurrentDevices;
            VisibleEntries.uNumAllNewDevices = m_AllNewDevices;
            VisibleEntries.uNumAllExpiredDevices = m_AllExpiredDevices;
            VisibleEntries.uNumAllUpdatedServices = m_AllUpdatedServices;

            VisibleEntries.uNumFilteredVisibleDevices = m_VisibleFilteredDevices;
            VisibleEntries.uNumFilteredVisibleServices = m_VisibleFilteredServices;

            WFCTrace("--> CServicesManager::OnTimerCallback[Notify]: Sending notification ...");
            m_pAdapter->IndicateStatus(
                m_NdisDevicePortNumber,
                NDIS_STATUS_DOT11_WFD_VISIBLE_DEVICE_LIST_UPDATED,
                NULL,
                &VisibleEntries,
                sizeof(VisibleEntries));
        }
    }

    if (bRestartDeviceExpiryTimer)
    {
        StartDeviceExpiryNotificationTimer ();
    }

    TraceExit(0);
}


ULONG
CServicesManager::CheckForNewDevices(
    _In_ ULONGLONG currentTime,
    _In_ DOT11_MAC_ADDRESS *pDeviceAddress
    )
/*++
    Routine Description:
        This routine checks if the device address matches any of the configured
        entries for background discovery, and if so updates the visible time, and also returns
        TRUE if the status changed from not-visible to visible

    Arguments:

    Return Value:
--*/
{
    PLIST_ENTRY pCurrentEntry;
    ULONG uNumMatched = 0;

    pCurrentEntry = m_TrackedP2PDevicesList.Flink;
    while (pCurrentEntry != &m_TrackedP2PDevicesList)
    {
        CP2PBackgroundDeviceContext* pP2PDeviceContext = (CP2PBackgroundDeviceContext*)
            CP2PBackgroundDeviceContext::GetP2PBackgroundContextFromListEntry (pCurrentEntry);

        pCurrentEntry = pCurrentEntry->Flink;

        if (pP2PDeviceContext->IsContextMatch(pDeviceAddress))
        {
            if (pP2PDeviceContext->CheckForStateChangeOnDriverIndication (currentTime))
            {
                uNumMatched++;
                m_VisibleFilteredDevices++;
                WFCTrace("\t+++++ [BkD - New] Device <%!MACADDR!> is now VISIBLE +++++\n",
                    *pP2PDeviceContext->GetContext());
            }
            break;
        }
    }

    return (uNumMatched);
}


ULONG
CServicesManager::CheckForNewServices(
    _In_ ULONGLONG currentTime,
    _In_ PBSS_ENTRY_WFD_SERVICES_INFO pWfdServiceNames
    )
/*++
    Routine Description:
        This routine checks if the service names match any of the configured
        entries for background discovery, and if so updates the visible time, and also returns
        TRUE if the status changed from not-visible to visible

    Arguments:

    Return Value:
--*/
{
    PLIST_ENTRY pCurrentEntry;
    ULONG uNumMatched = 0;
    PWFD_OTA_ADVERTISED_SERVICE_INFO pServiceInfo;
    PWFD_OTA_ADVERTISED_SERVICE_DESCRIPTOR pOTAService;

    if (!pWfdServiceNames)
    {
        return (0);
    }

    pServiceInfo = pWfdServiceNames->ServiceInfo;
    pOTAService = pServiceInfo->AdvertisedServiceDescriptors;
    pWfdServiceNames->usNumMatchedServices = 0;
    for (ULONG i=0; i< pWfdServiceNames->usNumServices; i++)
    {
        BOOLEAN bServiceMatched = FALSE;

        pCurrentEntry = m_TrackedP2PServicesList.Flink;
        while (pCurrentEntry != &m_TrackedP2PServicesList)
        {
            CP2PBackgroundServiceContext* pP2PServiceContext = (CP2PBackgroundServiceContext*)
                CP2PBackgroundServiceContext::GetP2PBackgroundContextFromListEntry (pCurrentEntry);

            pCurrentEntry = pCurrentEntry->Flink;

            if (pP2PServiceContext->IsContextMatch(pOTAService->ServiceNameLength, pOTAService->ServiceName))
            {
                bServiceMatched = TRUE;
                pWfdServiceNames->usNumMatchedServices++;

                if (pP2PServiceContext->CheckForStateChangeOnDriverIndication (currentTime))
                {
                    uNumMatched++;
                    m_VisibleFilteredServices++;
                    WFCTrace("\t+++++ [BkD - New] Service <%s> is now VISIBLE +++++\n",
                        (PCHAR) pP2PServiceContext->GetContext());
                }
                break;
            }
        }

        //
        // If there was no match, set the ConfigMethods to 0 so that we avoid another search later on
        //
        if (!bServiceMatched)
        {
            pOTAService->ConfigMethods = 0;
        }

        // Go to next OTA descriptor
        pOTAService = (PWFD_OTA_ADVERTISED_SERVICE_DESCRIPTOR) (
            ((PBYTE) pOTAService) +
            WFD_OTA_ADVERTISED_SERVICE_DESCRIPTOR_FIXED_SIZE +
            pOTAService->ServiceNameLength);
    }

    return (uNumMatched);
}


VOID
CServicesManager::CheckStartDeviceUpdateNotificationTimer(
    )
{
    NDIS_STATUS ndisStatus;

    if (m_P2PBackgroundNotificationsEnabled)
    {
        // m_DeviceAndServiceUpdateLock
        CAutoLock lock(m_DeviceAndServiceUpdateLock);

        if (m_WaitingForP2PDeviceQuery)
        {
            WFCTrace("CServicesManager::CheckStartDeviceUpdateNotificationTimer: Notification was already sent, waiting for Query ...");
        }
        else if (!m_DeviceUpdateNotificationTimerRunning)
        {
            //
            // Start the DeviceOrServiceUpdateTimer timer
            //
            m_DeviceUpdateNotificationTimerRunning = true;

            ndisStatus = m_pAdapter->GetEventQueue()->RegisterTimeoutCallbackWithLevelAndReuse(
                IActivityId::get_NextActivityId(),
                this,
                m_WfdDeviceUpdateNotificationDelayTimeoutMSecs,
                &m_DeviceUpdateNotificationTimerToken,                                   // WFC_TIMER_CONTEXT
                WDF_EXECUTION_LEVEL::WdfExecutionLevelPassive,
                false,
                NULL,
                &m_DeviceUpdateNotificationTimerToken );
            if (NDIS_STATUS_SUCCESS == ndisStatus)
            {
                if (m_DeviceExpiryNotificationTimerRunning)
                {
                    //
                    // If the Expired Devices timer is currently running, stop the timer from firing
                    //
                    WFCTrace("CheckStartDeviceUpdateNotificationTimer: STOPping ExpiredDevices Timer ...");
                    m_pAdapter->GetEventQueue()->DeregisterTimeoutCallback( m_ExpiredDevicesCheckTimerToken );
                    m_ExpiredDevicesCheckTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;

                    m_DeviceExpiryNotificationTimerRunning = FALSE;
                }

                WFCTrace("CheckStartDeviceUpdateNotificationTimer: STARTed DeviceOrServiceUpdate Timer");
            }
            else
            {
                WFCTrace("CheckStartDeviceUpdateNotificationTimer: FAILed to start DeviceOrServiceUpdate Timer");
                m_DeviceUpdateNotificationTimerRunning = false;
            }
        }
    }   // m_DeviceAndServiceUpdateLock
}


NDIS_STATUS
CServicesManager::UpdateBackgroundDeviceExpired(
    _In_ DOT11_MAC_ADDRESS *pDeviceAddress,
    _In_ ULONG BSSEntryDiscoveryContext,
    _In_ PBSS_ENTRY_WFD_SERVICES_INFO pBeaconWfdServiceNames,
    _In_ PBSS_ENTRY_WFD_SERVICES_INFO pProbeWfdServiceNames
    )
/*++
    Routine Description:
        This routine is called when the cache timeout logic removes a device from the cache

    Arguments:

    Return Value:
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    UNREFERENCED_PARAMETER (pDeviceAddress);
    UNREFERENCED_PARAMETER (pBeaconWfdServiceNames);
    UNREFERENCED_PARAMETER (pProbeWfdServiceNames);

    NT_ASSERT (m_AllCurrentDevices);

    if (BSSEntryDiscoveryContext == m_CurrentDiscoveryContext)
    {
        m_AllCurrentDevices--;
        m_AllExpiredDevices++;
    }

    if (m_P2PBackgroundNotificationsEnabled)
    {
        WFCTrace("UpdateBackgroundDeviceExpired: Device=%!MACADDR!, BSS:CurrentContext=%d:%d, AllCurrent:New:Expired:Updated=%d:%d:%d:%d, Filtered=%d+%d",
            *pDeviceAddress, BSSEntryDiscoveryContext, m_CurrentDiscoveryContext, m_AllCurrentDevices,
            m_AllNewDevices, m_AllExpiredDevices, m_AllUpdatedServices,
            m_VisibleFilteredDevices, m_VisibleFilteredServices);

        if (!(m_NotifyAllDevicesAndServices))
        {
            CheckForExpiredDevicesAndServices( CSystem::get_CurrentTime() );
        }

        CheckStartDeviceUpdateNotificationTimer ();
    }

    return ndisStatus;
}

NDIS_STATUS
CServicesManager::UpdateBackgroundDeviceIndicatedByDriver(
    _Inout_ ULONG *pBSSEntryDiscoveryContext,
    _Inout_ BOOLEAN *pIsWFDServiceUpdated,
    _In_ DOT11_MAC_ADDRESS *pDeviceAddress,
    _In_ PBSS_ENTRY_WFD_SERVICES_INFO pBeaconWfdServiceNames,
    _In_ PBSS_ENTRY_WFD_SERVICES_INFO pProbeWfdServiceNames
    )
/*++
    Routine Description:
        This routine is called when the driver indicates a discovered Device entry
        On being called, the device address and services indicated are matched with
        the any that the OS is supposed to be listening for.
        Also, if the ExpiryCheck time has passed, it checks if any devices and services are not longer visible

    Arguments:

    Return Value:
--*/
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG BSSEntryDiscoveryContext = *pBSSEntryDiscoveryContext;
    BOOLEAN IsWFDServiceUpdated = *pIsWFDServiceUpdated;
    ULONG uNumNewFilteredDevices = 0, uNumNewFilteredServices = 0, uNumExpiredEntries = 0;
    ULONGLONG currentTime = CSystem::get_CurrentTime();         // Use one value of currentTime for consistency

    TraceEntry();

    if (!m_P2PBackgroundNotificationsEnabled)
    {
        goto Exit;
    }

    WFCTrace("UpdateBackgroundDeviceIndicatedByDriver: Device=%!MACADDR! NotificationsEnabled=%x, BSSEntry:CurrentContext=%d:%d, SvcUpdated=%x, AllCurrent:New:Expired:Updated=%d:%d:%d:%d, Filtered=%d+%d\n",
        *pDeviceAddress, m_P2PBackgroundNotificationsEnabled, BSSEntryDiscoveryContext, m_CurrentDiscoveryContext,
        IsWFDServiceUpdated, m_AllCurrentDevices, m_AllNewDevices, m_AllExpiredDevices, m_AllUpdatedServices,
        m_VisibleFilteredDevices, m_VisibleFilteredServices);

    if ((BSSEntryDiscoveryContext != m_CurrentDiscoveryContext) || (IsWFDServiceUpdated))
    {
        if (BSSEntryDiscoveryContext != m_CurrentDiscoveryContext)
        {
            m_AllCurrentDevices++;
            m_AllNewDevices++;
            *pBSSEntryDiscoveryContext = m_CurrentDiscoveryContext;
        }
        if (IsWFDServiceUpdated)
        {
            m_AllUpdatedServices++;
            *pIsWFDServiceUpdated = false;
        }

        if (!m_NotifyAllDevicesAndServices)
        {
            uNumNewFilteredDevices = CheckForNewDevices( currentTime, pDeviceAddress );
            uNumNewFilteredServices += CheckForNewServices( currentTime, pBeaconWfdServiceNames );
            uNumNewFilteredServices += CheckForNewServices( currentTime, pProbeWfdServiceNames );
        }
    }
    else if (!m_NotifyAllDevicesAndServices)
    {
        //
        // Nothing changed -- probably just the same device becoming visible again
        // If the UpdateNotification timer is not running, check for expired entries
        //
        if (!m_DeviceUpdateNotificationTimerRunning)
        {
            uNumExpiredEntries = CheckForExpiredDevicesAndServices( currentTime );
        }
    }

    //
    // See if we need to send the Indication up the stack
    //
    if ((BSSEntryDiscoveryContext != m_CurrentDiscoveryContext) || (IsWFDServiceUpdated) || (uNumExpiredEntries))
    {
        CheckStartDeviceUpdateNotificationTimer ();
    }

Exit:
    TraceExit(uNumNewFilteredDevices + uNumNewFilteredServices + uNumExpiredEntries);
    return (status);
}

//
// Now, handle the case when the upper layers query the Device info
//
VOID
CServicesManager::InitializeForP2PDeviceQuery(
    )
{
    PLIST_ENTRY pCurrentEntry;

    TraceEntry();

    if (!m_P2PBackgroundNotificationsEnabled)
    {
        goto Exit;
    }

    m_WaitingForP2PDeviceQuery = false;

    m_AllNewDevices = 0;
    m_AllExpiredDevices = 0;
    m_AllUpdatedServices = 0;

    pCurrentEntry = m_TrackedP2PDevicesList.Flink;
    while (pCurrentEntry != &m_TrackedP2PDevicesList)
    {
        CP2PBackgroundDeviceContext* pP2PDeviceContext = (CP2PBackgroundDeviceContext*)
            CP2PBackgroundDeviceContext::GetP2PBackgroundContextFromListEntry (pCurrentEntry);

        pCurrentEntry = pCurrentEntry->Flink;

        if (pP2PDeviceContext->ClearVisibleStateAtQueryTime())
        {
            //
            // The device was marked as visible earlier
            //
            WFCTrace("\t----- [BkD - Query] Setting NOT visible state for Device <%!MACADDR!> -----\n",
                *pP2PDeviceContext->GetContext());
        }
    }

    //
    // Set the state for the Service entries
    //
    pCurrentEntry = m_TrackedP2PServicesList.Flink;
    while (pCurrentEntry != &m_TrackedP2PServicesList)
    {
        CP2PBackgroundServiceContext* pP2PServiceContext = (CP2PBackgroundServiceContext*)
            CP2PBackgroundServiceContext::GetP2PBackgroundContextFromListEntry (pCurrentEntry);

        pCurrentEntry = pCurrentEntry->Flink;

        if (pP2PServiceContext->ClearVisibleStateAtQueryTime())
        {
            //
            // The service was marked as visible earlier
            //
            WFCTrace("\t----- [BkD - Query] Setting NOT visible state for Service <%s> -----\n",
                (PCHAR) pP2PServiceContext->GetContext());
        }
    }

Exit:
    TraceExit(0);
}

VOID
CServicesManager::StartDeviceExpiryNotificationTimer(
    )
{
    NDIS_STATUS status;

    TraceEntry();

    if (m_P2PBackgroundNotificationsEnabled)
    {
        // m_DeviceAndServiceUpdateLock
        CAutoLock lock(m_DeviceAndServiceUpdateLock);

        if ((m_DeviceUpdateNotificationTimerRunning) ||
            (m_DeviceExpiryNotificationTimerRunning))
        {
            //
            // If any of the timers is already running, do not start a new timer
            //
            WFCTrace("CServicesManager::StartDeviceExpiryNotificationTimer: Timer is already running ...");
        }
        else if (m_WaitingForP2PDeviceQuery)
        {
            WFCTrace("CServicesManager::StartDeviceExpiryNotificationTimer: Notification was already sent, waiting for Quey ...");
        }
        else if (((m_NotifyAllDevicesAndServices) && (m_AllCurrentDevices)) ||
                 ((!m_NotifyAllDevicesAndServices) &&
                  ((m_VisibleFilteredDevices) || (m_VisibleFilteredServices))))
        {
            //
            // At least one device of interest is currently cached
            //
            m_DeviceExpiryNotificationTimerRunning = true;

            status = m_pAdapter->GetEventQueue()->RegisterTimeoutCallbackWithLevelAndReuse(
                IActivityId::get_NextActivityId(),
                this,
                m_WfdDeviceExpiryCheckTimeoutMSecs,
                &m_ExpiredDevicesCheckTimerToken,                                   // WFC_TIMER_CONTEXT
                WDF_EXECUTION_LEVEL::WdfExecutionLevelPassive,
                false,
                NULL,
                &m_ExpiredDevicesCheckTimerToken );
            if (NDIS_STATUS_SUCCESS == status)
            {
                WFCTrace("\tStartDeviceExpiryNotificationTimer: STARTed Timer for All=%d, Filtered=%d+%d",
                    m_AllCurrentDevices, m_VisibleFilteredDevices, m_VisibleFilteredServices);
            }
            else
            {
                m_DeviceExpiryNotificationTimerRunning = false;
                WFCError("\tStartDeviceExpiryNotificationTimer: FAILed to start Timer status=%x", status);
            }
        }
        else if (WFC_TIMER_REGISTRATION_TOKEN_INVALID != m_ExpiredDevicesCheckTimerToken)
        {
            //
            // If the timer is currently running, stop the timer from firing
            //
            WFCTrace("CServicesManager::SetDeviceNotVisibleTimer: No Devices or Services - STOPPING Timer ...");

            m_pAdapter->GetEventQueue()->DeregisterTimeoutCallback( m_ExpiredDevicesCheckTimerToken );
            m_ExpiredDevicesCheckTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
        }
    }

    TraceExit(0);
}

ULONG
CServicesManager::MarkP2PBackgroundServicesQueried(
    _In_opt_ PBSS_ENTRY_WFD_SERVICES_INFO pWfdServiceNames
    )
{
    ULONG uNewVisibleServices = 0;
    PLIST_ENTRY pCurrentEntry;
    PWFD_OTA_ADVERTISED_SERVICE_INFO pServiceInfo;
    PWFD_OTA_ADVERTISED_SERVICE_DESCRIPTOR pOTAService;

    if (!pWfdServiceNames)
    {
        return 0;
    }

    pServiceInfo = pWfdServiceNames->ServiceInfo;
    pOTAService = pServiceInfo->AdvertisedServiceDescriptors;
    for (ULONG i=0; i< pWfdServiceNames->usNumServices; i++)
    {
        //
        // ConfigMethods would have been zero'ed earlier if there was no matching service
        //
        if (pOTAService->ConfigMethods)
        {
            pCurrentEntry = m_TrackedP2PServicesList.Flink;
            while (pCurrentEntry != &m_TrackedP2PServicesList)
            {
                CP2PBackgroundServiceContext* pP2PServiceContext = (CP2PBackgroundServiceContext*)
                    CP2PBackgroundServiceContext::GetP2PBackgroundContextFromListEntry (pCurrentEntry);

                pCurrentEntry = pCurrentEntry->Flink;

                if (pP2PServiceContext->IsContextMatch(pOTAService->ServiceNameLength, pOTAService->ServiceName))
                {
                    if (!pP2PServiceContext->SetVisibleStateAtQueryTime())
                    {
                        uNewVisibleServices++;
                        m_VisibleFilteredServices++;
                        WFCTrace("\t+++++ [BkD - Query] Setting VISIBLE state for Service <%s> +++++\n",
                            (PCHAR) pP2PServiceContext->GetContext());
                    }
                    break;
                }
            }
        }

        // Go to next OTA descriptor
        pOTAService = (PWFD_OTA_ADVERTISED_SERVICE_DESCRIPTOR) (
            ((PBYTE) pOTAService) +
            WFD_OTA_ADVERTISED_SERVICE_DESCRIPTOR_FIXED_SIZE +
            pOTAService->ServiceNameLength);
    }

    return (uNewVisibleServices);
}

VOID
CServicesManager::MarkP2PBackgroundContextsQueried(
    _In_ DOT11_MAC_ADDRESS *pDeviceAddress,
    _In_opt_ PBSS_ENTRY_WFD_SERVICES_INFO pBeaconWfdServiceNames,
    _In_opt_ PBSS_ENTRY_WFD_SERVICES_INFO pProbeWfdServiceNames
    )
{
    ULONG uNewVisibleDevices = 0, uNewVisibleServices = 0;
    PLIST_ENTRY pCurrentEntry;

    TraceEntry();

    if (!m_P2PBackgroundNotificationsEnabled)
    {
        goto Exit;
    }

    //
    // Set the state for the Device entry
    //
    pCurrentEntry = m_TrackedP2PDevicesList.Flink;
    while (pCurrentEntry != &m_TrackedP2PDevicesList)
    {
        CP2PBackgroundDeviceContext* pP2PDeviceContext = (CP2PBackgroundDeviceContext*)
            CP2PBackgroundDeviceContext::GetP2PBackgroundContextFromListEntry (pCurrentEntry);

        pCurrentEntry = pCurrentEntry->Flink;

        if (pP2PDeviceContext->IsContextMatch(pDeviceAddress))
        {
            if (!pP2PDeviceContext->SetVisibleStateAtQueryTime())
            {
                uNewVisibleDevices++;
                m_VisibleFilteredDevices++;
                WFCTrace("\t+++++ [BkD - Query] Setting VISIBLE state for Device <%!MACADDR!> +++++\n",
                    *pP2PDeviceContext->GetContext());
            }
            break;
        }
    }

    //
    // Set the state for the Services entries
    //
    uNewVisibleServices += MarkP2PBackgroundServicesQueried( pBeaconWfdServiceNames );
    uNewVisibleServices += MarkP2PBackgroundServicesQueried( pProbeWfdServiceNames );

    WFCTrace("\tMarkP2PBackgroundContextsQueried: Device %!MACADDR! has %d+%d new Devices+Services",
        *pDeviceAddress, uNewVisibleDevices, uNewVisibleServices);

Exit:
    TraceExit(uNewVisibleDevices+uNewVisibleServices);
}


