/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    PeerList.cpp

Abstract:

    Implementation for maintaining peers on a port

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "PeerList.tmh"


//==============================================================================================
CConnectedPeer::CConnectedPeer() :
    m_AssociationState(dot11_assoc_state_zero),
    m_IsValid(FALSE)
{
    RtlZeroMemory(m_MacAddress, sizeof(DOT11_MAC_ADDRESS));
}

CConnectedPeer::~CConnectedPeer()
{

}

VOID CConnectedPeer::Cleanup()
{
    RtlZeroMemory(&m_MacAddress, sizeof(DOT11_MAC_ADDRESS));
    m_AssociationState = dot11_assoc_state_zero;
    m_IsValid = FALSE;
    m_LinkQuality = (UCHAR)-1;    // Max value
    
}

VOID CConnectedPeer::Initialize(
    DOT11_MAC_ADDRESS MacAddress
    )
{
    RtlCopyMemory(&m_MacAddress, MacAddress, sizeof(DOT11_MAC_ADDRESS));
    m_IsValid = TRUE;
    m_LinkQuality = (UCHAR)-1;  // Max value
    m_RoamHistory.Reset();
}


//==============================================================================================
NDIS_STATUS CConnectedPeerList::Initialize(
    _In_ CAdapter* pAdapter,
    _In_ UINT16 WfcPortId,
    _In_ ULONG MaxPeerCount
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceEntry();

    m_pAdapter = pAdapter;
    m_WfcPortId = WfcPortId;
    m_MaxPeerCount = MaxPeerCount;
    m_pPeerTable = new CConnectedPeer[m_MaxPeerCount];
    if (m_pPeerTable == NULL)
    {
        WFCError("Failed to allocate table for %d Connected Peers", m_MaxPeerCount);  
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    for (ULONG i = 0; i < m_MaxPeerCount; i++)
    {
        m_pPeerTable[i].Cleanup();
    }

exit:    
    TraceExit(ndisStatus);
    return ndisStatus;
}


CConnectedPeer* CConnectedPeerList::FindPeerInTable(
    _In_ DOT11_MAC_ADDRESS MacAddress
    )
{
    CConnectedPeer *pPeer = NULL;

    for (ULONG i = 0; i < m_MaxPeerCount; i++)
    {
        if (ARE_MAC_ADDRESS_EQUAL(MacAddress,
                m_pPeerTable[i].GetMacAddress()))
        {
            pPeer = &m_pPeerTable[i];
        }
    }        
    
    return pPeer;
}

NDIS_STATUS CConnectedPeerList::FindOrAddPeerToTable(
    _In_ DOT11_MAC_ADDRESS MacAddress,
    _Out_ CConnectedPeer **ppOutPeer
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CConnectedPeer *pPeer = NULL;
    
    pPeer = FindPeerInTable(MacAddress);
    if (!pPeer)
    {
        // Try to add peer entry
        for (ULONG i = 0; i < m_MaxPeerCount; i++)
        {
            if (m_pPeerTable[i].IsValid() == FALSE)
            {
                pPeer = &m_pPeerTable[i];
            }
        }        
    }

    if (pPeer)
    {
        // Set this as Valid
        pPeer->Initialize(MacAddress);
    }
    else
    {        
        ndisStatus = NDIS_STATUS_RESOURCES;
    }
    
    *ppOutPeer = pPeer;
    
    return ndisStatus;
}

NDIS_STATUS CConnectedPeerList::AddPeer(
    _In_bytecount_(6) DOT11_MAC_ADDRESS MacAddress,
    _In_ DOT11_ASSOCIATION_STATE AssociationState,
    _In_ ULONG LinkQuality
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CConnectedPeer *pPeer = NULL;
    
    TraceEntry();

    ndisStatus = FindOrAddPeerToTable(MacAddress, &pPeer);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to find or add peer entry for %!MACADDR!. Error = 0x%08x", MacAddress, ndisStatus);
        goto exit;
    }

    // Reset the link speed & quality for this port so that we will indicate
    // when we finish association
    m_pAdapter->GetPortPropertyCache(m_WfcPortId)->SetPropertyULong(
        (WFC_PROPERTY_NAME)WfcPortPropertyNameTxLinkSpeed,
        0xFFFFFFFFul );

    m_pAdapter->GetPortPropertyCache(m_WfcPortId)->SetPropertyULong(
        (WFC_PROPERTY_NAME)WfcPortPropertyNameRxLinkSpeed,
        0xFFFFFFFFul );
    m_pAdapter->GetPortPropertyCache(m_WfcPortId)->SetPropertyUchar(
        (WFC_PROPERTY_NAME)WfcPortPropertyNameLinkQuality,
        0xFFu );

    // Set state
    pPeer->SetAssociationState(AssociationState);
    // Set the initial link quality
    pPeer->SetLinkQuality((UCHAR)LinkQuality);
    
    // Save the association time link quality
    pPeer->GetRoamHistory()->LinkQualityHistory = (UCHAR)LinkQuality;

#if WDI_DISABLED

    PEER_INFO peerInfo = {0};

    //notify lower edge
    peerInfo.PortID = m_WfcPortId;

    RtlCopyMemory(&peerInfo.MacAddress, MacAddress, sizeof(WDI_MAC_ADDRESS));

    m_pAdapter->GetPortPropertyCache(m_WfcPortId)->GetPropertyBoolean(
        (WFC_PROPERTY_NAME)WfcPortPropertySafeModeEnabled,
        &peerInfo.bHostFIPSMode);

    peerInfo.MaxSupportedAMSDUSize = (UINT16)(-1);

    ndisStatus = m_pAdapter->m_DataPathFunctions.NICAddPeer(
        m_pAdapter->m_NICContext, 
        &peerInfo);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to add peer to LE for %!MACADDR!. Error = 0x%08x", MacAddress, ndisStatus);
        goto exit;
    }
#endif WDI_DISABLED
exit:    

    // Add must always succeed
    NT_ASSERT(ndisStatus == NDIS_STATUS_SUCCESS);
    
    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CConnectedPeerList::DeletePeer(
    _In_ DOT11_MAC_ADDRESS MacAddress        
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CConnectedPeer *pPeer = NULL;
    
    TraceEntry();

    pPeer = FindPeerInTable(MacAddress);
    if (!pPeer)
    {
        // Caller can ignore this status
        ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
        goto exit;
    }

    ndisStatus = CleanupPeerObject(pPeer);

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}


NDIS_STATUS CConnectedPeerList::CleanupPeerObject(
    _In_ CConnectedPeer* pPeer )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    WFCTrace("Cleaning up peer object %!MACADDR!", *pPeer->GetMacAddress());

    // Reset to default state
    pPeer->Cleanup();

    TraceExit(ndisStatus);
    return ndisStatus;
}


CConnectedPeer* CConnectedPeerList::FindPeerByAssociationState(
    _In_ DOT11_ASSOCIATION_STATE AssociationState
    )
{
    for (ULONG i = 0; i < m_MaxPeerCount; i++)
    {
        if (m_pPeerTable[i].IsValid() && 
            (m_pPeerTable[i].GetAssociationState() == AssociationState))
        {
            return &m_pPeerTable[i];
        }
    }
    return NULL;
}

CConnectedPeer* CConnectedPeerList::FindPeerByAddress(
    _In_ DOT11_MAC_ADDRESS MacAddress
    )
{
    for (ULONG i = 0; i < m_MaxPeerCount; i++)
    {
        if (m_pPeerTable[i].IsValid() && 
            (ARE_MAC_ADDRESS_EQUAL(m_pPeerTable[i].GetMacAddress(), MacAddress)))
        {
            return &m_pPeerTable[i];
        }
    }
    return NULL;
}


// Enumerate interfaces
CConnectedPeer* CConnectedPeerList::GetFirst(
    _Out_ PEERLIST_ENUM_CONTEXT *pEnumContext
    )
{
    *pEnumContext = 0;
    return GetNext(pEnumContext);
}

CConnectedPeer* CConnectedPeerList::GetNext(
    _Inout_ PEERLIST_ENUM_CONTEXT *pEnumContext
    )
{
    for (ULONG i = *pEnumContext; i < m_MaxPeerCount; i++)
    {
        if (m_pPeerTable[i].IsValid() == TRUE)
        {
            *pEnumContext = i + 1;
            return &m_pPeerTable[i];
        }
    }

    return NULL;
}


void CConnectedPeerList::FlushPeerList()
{
    for(ULONG i = 0; i < m_MaxPeerCount; i++)
    {
        if ( m_pPeerTable[i].IsValid() == TRUE )
        {
            CleanupPeerObject(&m_pPeerTable[i]);
        }
    }
}
