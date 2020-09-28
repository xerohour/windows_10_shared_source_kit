/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    NetworkHistory.hpp

Abstract:

    Header file for the per-port Networks table

Environment:

    Kernel mode

Revision History:

--*/
#pragma once

//==============================================================================================
// Used in the list CNetworkHistoryList
typedef struct _WFC_NETWORK_CHANNEL_ENTRY {
    UINT32 BandId;
    UINT32 ChannelNumber;
    UINT32 ConnectUsageCount;
    bool FoundInCandidateList;

    void Initialize(
        UINT32 bandId,
        UINT32 channelNumber
        )
    {
        BandId = bandId;
        ChannelNumber = channelNumber;
        ConnectUsageCount = 0;
        FoundInCandidateList = false;
    }

    bool IsValid()
    {
        return (ConnectUsageCount > 0);
    }
} WFC_NETWORK_CHANNEL_ENTRY, *PWFC_NETWORK_CHANNEL_ENTRY;

//==============================================================================================
typedef struct _WFC_NETWORK_BLOCKED_INFO
{
    void Initialize()
    {
        BlockedReason = WFC_BLOCKED_REASON_NONE;
        LastAssocFailTime = 0;
    }

    ULONG      BlockedReason;
    ULONGLONG  LastAssocFailTime;
} WFC_NETWORK_BLOCKED_INFO, *PWFC_NETWORK_BLOCKED_INFO;

//==============================================================================================

// Tracks history of connection attempts for a network
class CNetworkHistory
{
public:
    DOT11_SSID  NetworkSsid;
    ULONGLONG   LastUpdateTime;

    // Last BSSID we successfully connected to for this network
    DOT11_MAC_ADDRESS LastBssid;

    WFC_NETWORK_BLOCKED_INFO BlockedInfo;
    
    // List of channel we have successfully connected to for this network
    WFC_NETWORK_CHANNEL_ENTRY   ChannelHistory[WFC_CONSTANT::MaxNetworkChannelHistorySize];
    bool        FoundOutside24GHz;

public:
    CNetworkHistory()
    {
        Cleanup();
    }

    bool IsValid()
    {
        return ((NetworkSsid.uSSIDLength > 0) && (LastUpdateTime > 0));
    }

    void Initialize(
        _In_ PDOT11_SSID pSsid        
        )
    {
        Cleanup();
        
        RtlCopyMemory(&NetworkSsid, pSsid, sizeof(DOT11_SSID));
        LastUpdateTime = CSystem::get_CurrentTime();
    }

    void Cleanup()
    {
        LastUpdateTime = 0;
        FoundOutside24GHz = false;
        BlockedInfo.Initialize();        
        RtlZeroMemory(LastBssid, sizeof(DOT11_MAC_ADDRESS));
        RtlZeroMemory(ChannelHistory, sizeof(WFC_NETWORK_CHANNEL_ENTRY) * WFC_CONSTANT::MaxNetworkChannelHistorySize);    
   }

    void FindOrAddChannelEntry(
        _In_ UINT32 BandId,
        _In_ UINT32 Channel
        );

};

//==============================================================================================
typedef ULONG NETWORK_HISTORY_ENUM_CONTEXT;


class CNetworkHistoryList
{
public:
    CNetworkHistoryList(
        )
    {
        m_pTable = NULL;
    }

    ~CNetworkHistoryList()
    {
        if (m_pTable)
        {
            delete []m_pTable;
            m_pTable = NULL;
        }
    }

    NDIS_STATUS Initialize(
        _In_ CAdapter* pAdapter,
        _In_ UINT16 WfcPortId,
        _In_ ULONG MaxEntryCount
        );
    

    // Enumerate interfaces
    CNetworkHistory* GetFirst(
        _Out_ NETWORK_HISTORY_ENUM_CONTEXT *pEnumContext
        );

    CNetworkHistory* GetNext(
        _Inout_ NETWORK_HISTORY_ENUM_CONTEXT *pEnumContext
        );

public:
     __success(return == NDIS_STATUS_SUCCESS)
    NDIS_STATUS UpdateOrAddNetworkToTable(
        _In_ PDOT11_SSID pSsid,
        _Out_ CNetworkHistory **ppOutNetwork
        );
        
    CNetworkHistory *FindNetworkBySsid(
        _In_ PDOT11_SSID pSsid
        );

    void FlushNetworkHistory();
        
private:
    VOID CleanupNetworkObject(
        _In_ CNetworkHistory* pNetwork );

private:
    CAdapter* m_pAdapter;
    UINT16 m_WfcPortId;
    ULONG  m_MaxEntryCount;
    
    CNetworkHistory *m_pTable;
    
};

