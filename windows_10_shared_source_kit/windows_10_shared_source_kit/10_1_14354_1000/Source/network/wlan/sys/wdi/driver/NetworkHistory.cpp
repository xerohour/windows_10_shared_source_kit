/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    NetworkHistory.cpp

Abstract:

    Implementation for maintaining history of networks on a port

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "NetworkHistory.tmh"


//==============================================================================================
void CNetworkHistory::FindOrAddChannelEntry(
    _In_ UINT32 BandId,
    _In_ UINT32 Channel
    )
{
    ULONG leastUsage = (ULONG)-1;
    ULONG replaceIndex = 0;
    ULONG i;
    bool  bNeedsToBeSaved = true;

    // If already seen outside 2.4GHz, only update if this is not 2.4GHz. This way we are using
    // the scan optimization to find 5GHz APs instead of spending time on 2.4GHz
    // aps
    if (FoundOutside24GHz && (BandId == WDI_BAND_ID_2400))
    {
        // Dont save, but "reduce" others
        bNeedsToBeSaved = false;
    }

    for (i = 0; i < WFC_CONSTANT::MaxNetworkChannelHistorySize; i++)
    {
        if ((ChannelHistory[i].BandId == BandId) &&
            (ChannelHistory[i].ChannelNumber == Channel) &&
            (ChannelHistory[i].IsValid())
            )            
        {        
            // Dont increment so much that we have to deal with this never going away.
            // We limit to "MaxConnectBSSChannelUsage + ConnectBSSChannelHistoryBoost".
            if (bNeedsToBeSaved &&                      // Would be false for 2.4GHz when 5GHz is found
                (ChannelHistory[i].ConnectUsageCount < WFC_CONSTANT::MaxConnectBSSChannelUsage))
            {
                ChannelHistory[i].ConnectUsageCount += WFC_CONSTANT::ConnectBSSChannelHistoryBoost;
              
                WFCTrace("[ChannelHint] Incrementing channel hint entry B:%d C:%d to %d", BandId, Channel, 
                    ChannelHistory[i].ConnectUsageCount); 
            }

            // Dont break since we want to reduce everyone else
            bNeedsToBeSaved = false;
        }
        else if (leastUsage > ChannelHistory[i].ConnectUsageCount)
        {
            if ((ChannelHistory[i].ConnectUsageCount > 0) &&
                !ChannelHistory[i].FoundInCandidateList)
            {
                // Not connected to and was not found in candidates list
                ChannelHistory[i].ConnectUsageCount--;
            }
            
            // This would be a candidate to replace
            leastUsage = ChannelHistory[i].ConnectUsageCount;
            replaceIndex = i;
        }

        // No longer considered found in scan
        ChannelHistory[i].FoundInCandidateList = false;
   }

    if (bNeedsToBeSaved && (i >= WFC_CONSTANT::MaxNetworkChannelHistorySize))
    {   
        WFCTrace("[ChannelHint] Saved channel entry B:%d C:%d (replacing B:%d C:%d)", 
            BandId, Channel,
            ChannelHistory[replaceIndex].BandId, ChannelHistory[replaceIndex].ChannelNumber); 
        // Didnt find the entry. Replace an existing one
        ChannelHistory[replaceIndex].Initialize(BandId, Channel);        
        ChannelHistory[replaceIndex].ConnectUsageCount = WFC_CONSTANT::ConnectBSSChannelHistoryBoost;
    }
}

//==============================================================================================
NDIS_STATUS CNetworkHistoryList::Initialize(
    _In_ CAdapter* pAdapter,
    _In_ UINT16 WfcPortId,
    _In_ ULONG MaxEntryCount
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceEntry();

    m_pAdapter = pAdapter;
    m_WfcPortId = WfcPortId;
    m_MaxEntryCount = MaxEntryCount;
    m_pTable = new CNetworkHistory[m_MaxEntryCount];
    if (m_pTable == NULL)
    {
        WFCError("Failed to allocate table for %d Networks", m_MaxEntryCount);  
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }
exit:    
    TraceExit(ndisStatus);
    return ndisStatus;
}


CNetworkHistory* CNetworkHistoryList::FindNetworkBySsid(
    _In_ PDOT11_SSID pSsid
    )
{
    CNetworkHistory *pNetwork = NULL;

    for (ULONG i = 0; i < m_MaxEntryCount; i++)
    {
        if (m_pTable[i].IsValid() &&
            (m_pTable[i].NetworkSsid.uSSIDLength == pSsid->uSSIDLength) &&
            (memcmp(m_pTable[i].NetworkSsid.ucSSID, pSsid->ucSSID,
                pSsid->uSSIDLength) == 0))
        {
            pNetwork = &m_pTable[i];
        }
    }        
    
    return pNetwork;
}

 __success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CNetworkHistoryList::UpdateOrAddNetworkToTable(
    _In_ PDOT11_SSID pSsid,
    _Out_ CNetworkHistory **ppOutNetwork
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CNetworkHistory *pNetwork = NULL;
    
    pNetwork = FindNetworkBySsid(pSsid);

    // Didnt find, add
    if (!pNetwork)
    {
        ULONGLONG oldestTime = (ULONGLONG)-1;
        ULONG indexToReplace = 0;
        
        // Try to find a place to add or an entry to replace
        for (ULONG i = 0; i < m_MaxEntryCount; i++)
        {
            if (!m_pTable[i].IsValid())
            {
                pNetwork = &m_pTable[i];
                break;
            }
            else
            {
                // Check if this is the oldest entry (for replacing)
                if (oldestTime > m_pTable[i].LastUpdateTime)
                {
                    // This is the oldest entry so far
                    oldestTime = m_pTable[i].LastUpdateTime;
                    indexToReplace = i;
                }
            }
        }        

        if (pNetwork == NULL)
        {
            // Replace index 0 or whichever was found as the oldest entry
            pNetwork = &m_pTable[indexToReplace];
        }
        
        if (pNetwork)
        {
            // Set this up
            pNetwork->Initialize(pSsid);
        }
        else
        {        
            ndisStatus = NDIS_STATUS_RESOURCES;
        }
    }

    if (pNetwork)
    {
        pNetwork->LastUpdateTime = CSystem::get_CurrentTime();
    }
    
    *ppOutNetwork = pNetwork;
    
    return ndisStatus;
}


VOID CNetworkHistoryList::CleanupNetworkObject(
    _In_ CNetworkHistory* pPeer )
{
    pPeer->Cleanup();
}


// Enumerate interfaces
CNetworkHistory* CNetworkHistoryList::GetFirst(
    _Out_ NETWORK_HISTORY_ENUM_CONTEXT *pEnumContext
    )
{
    *pEnumContext = 0;
    return GetNext(pEnumContext);
}

CNetworkHistory* CNetworkHistoryList::GetNext(
    _Inout_ NETWORK_HISTORY_ENUM_CONTEXT *pEnumContext
    )
{
    for (ULONG i = *pEnumContext; i < m_MaxEntryCount; i++)
    {
        if (m_pTable[i].IsValid())
        {
            *pEnumContext = i + 1;
            return &m_pTable[i];
        }
    }

    return NULL;
}

void CNetworkHistoryList::FlushNetworkHistory()
{
    for(ULONG i = 0; i < m_MaxEntryCount; i++)
    {
        if (m_pTable[i].IsValid())
        {
            CleanupNetworkObject(&m_pTable[i]);
        }
    }
}
