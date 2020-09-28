/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    PeerList.hpp

Abstract:

    Header file for the per-port peer table

Environment:

    Kernel mode

Revision History:

--*/
#pragma once

//==============================================================================================

typedef struct _PEER_ROAM_HISTORY
{
    VOID Reset()
    {
        RoamAttemptCount = 0;     // No attempts
        RoamAttemptTime = CSystem::get_CurrentTime();
        LinkQualityHistory = 0;
    }

    // Roam history
    ULONGLONG         RoamAttemptTime;
    ULONG             RoamAttemptCount;
    UCHAR             LinkQualityHistory;
    
} PEER_ROAM_HISTORY, *PPEER_ROAM_HISTORY;


class CConnectedPeer
{
public:
    CConnectedPeer(); 

    ~CConnectedPeer();

    VOID Cleanup();

    VOID Initialize(
        DOT11_MAC_ADDRESS MacAddress
        );

    VOID SetAssociationState(
        DOT11_ASSOCIATION_STATE AssociationState
        )
    {
        m_AssociationState = AssociationState;
        if (AssociationState == dot11_assoc_state_auth_assoc)
        {
            // Save the association time as a roam attempt time
            m_RoamHistory.RoamAttemptTime = CSystem::get_CurrentTime();
        }
    }

    VOID SetLinkQuality(
        UCHAR LinkQuality
        )
    {
        m_LinkQuality = LinkQuality;
    }

    UCHAR GetLinkQuality()
    {
        return m_LinkQuality;
    }
    
    DOT11_ASSOCIATION_STATE GetAssociationState()
    {
        return m_AssociationState;
    }

    PDOT11_MAC_ADDRESS GetMacAddress()
    {
        return &m_MacAddress;
    }

    PPEER_ROAM_HISTORY GetRoamHistory()
    {
        return &m_RoamHistory;
    }

    BOOLEAN IsValid()
    {
        return m_IsValid;
    }
    
private:
    BOOLEAN           m_IsValid;
    
    DOT11_MAC_ADDRESS m_MacAddress;

    DOT11_ASSOCIATION_STATE m_AssociationState;   // dot11_assoc_state_zero = Not in use
    UCHAR             m_LinkQuality;

    PEER_ROAM_HISTORY m_RoamHistory;
};
//==============================================================================================


class CConnectedPeerList : public IConnectedPeerList
{
public:
    CConnectedPeerList(
        )
    {

    }

    ~CConnectedPeerList()
    {
        if (m_pPeerTable)
        {
            delete []m_pPeerTable;
            m_pPeerTable = NULL;
        }
    }

    NDIS_STATUS Initialize(
        _In_ CAdapter* pAdapter,
        _In_ UINT16 WfcPortId,
        _In_ ULONG MaxPeerCount
        );
    

    // Enumerate interfaces
    CConnectedPeer* GetFirst(
        _Out_ PEERLIST_ENUM_CONTEXT *pEnumContext
        );

    CConnectedPeer* GetNext(
        _Inout_ PEERLIST_ENUM_CONTEXT *pEnumContext
        );

public:
    // IConnectedPeerList
    NDIS_STATUS AddPeer(
        _In_bytecount_(6) DOT11_MAC_ADDRESS MacAddress,
        _In_ DOT11_ASSOCIATION_STATE AssociationState,
        _In_ ULONG LinkQuality        
        );

    NDIS_STATUS DeletePeer(
        _In_ DOT11_MAC_ADDRESS MacAddress        
        );

    CConnectedPeer *FindPeerByAddress(
        _In_ DOT11_MAC_ADDRESS MacAddress
        );

    CConnectedPeer* FindPeerByAssociationState(
        _In_ DOT11_ASSOCIATION_STATE AssociationState
        );

    void FlushPeerList();

private:
    CConnectedPeer* FindPeerInTable(
        _In_ DOT11_MAC_ADDRESS MacAddress
        );

    NDIS_STATUS FindOrAddPeerToTable(
        _In_ DOT11_MAC_ADDRESS MacAddress,
        _Out_ CConnectedPeer **ppOutPeer
        );

    NDIS_STATUS CleanupPeerObject(
        _In_ CConnectedPeer* pPeer );

private:
    CAdapter* m_pAdapter;
    UINT16 m_WfcPortId;
    
    ULONG m_MaxPeerCount;

    // For simplicity we are using a static table
    CConnectedPeer *m_pPeerTable;
    
};

