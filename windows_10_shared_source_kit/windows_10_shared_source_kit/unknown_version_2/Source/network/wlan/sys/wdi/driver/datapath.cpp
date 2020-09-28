/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    datapath.cpp

Abstract:

    Contains code for the WDI RxTx common functions.

Environment:

    Kernel mode

Author:


--*/
#include "precomp.hpp"
#include "datapath.tmh"


//
// Datapath interface to IHV component: datapath callbacks
//
void AdapterTxDequeueInd(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle,
    _In_ UINT32 Quantum,
    _In_ UINT8 MaxNumFrames,
    _In_ UINT16 Credit,
    _Out_ PNET_BUFFER_LIST *ppNBL)
{
    CAdapter *pAdapter = (CAdapter*)NdisMiniportDataPathHandle;

    ASSERT(pAdapter);

    pAdapter->m_TxMgr.TxDequeueInd(Quantum,
                                   MaxNumFrames,
                                   Credit,
                                   ppNBL);
}


void AdapterTxTransferCompleteInd(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle,
    _In_ WDI_TX_FRAME_STATUS WifiTxFrameStatus,
    _In_ PNET_BUFFER_LIST pNBL)
{
    CAdapter *pAdapter = (CAdapter*)NdisMiniportDataPathHandle;

    ASSERT(pAdapter);

    pAdapter->m_TxMgr.TxTransferCompleteInd(WifiTxFrameStatus,pNBL);
}

void AdapterTxSendCompleteInd(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle,
    _In_ WDI_TX_FRAME_STATUS WifiTxFrameStatus,
    _In_ UINT16 NumCompletedSends,
    _In_reads_(NumCompletedSends) WDI_FRAME_ID* WifiTxFrameIdList,
    _In_reads_opt_(NumCompletedSends) WDI_TX_COMPLETE_DATA* WifiTxCompleteList)
{
    CAdapter *pAdapter = (CAdapter*)NdisMiniportDataPathHandle;

    ASSERT(pAdapter);

    pAdapter->m_TxMgr.TxSendCompleteInd(WifiTxFrameStatus,
                                        NumCompletedSends,
                                        WifiTxFrameIdList,
                                        WifiTxCompleteList);
}

void AdapterTxQueryRaTidState(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle,
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ WDI_EXTENDED_TID ExTid,
    _Out_ PNDIS_STATUS pNdisStatus,
    _Out_ PUINT16 pQueueLength)
{
    CAdapter *pAdapter = (CAdapter*)NdisMiniportDataPathHandle;

    ASSERT(pAdapter);

    pAdapter->m_TxMgr.TxQueryRaTidStateInd(PortId,
        PeerId,
        ExTid,
        pNdisStatus,
        pQueueLength);
}

void AdapterTxSendPauseInd(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle,
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ UINT32 ExTidBitmask,
    _In_ WDI_TX_PAUSE_REASON TxPauseReason)
{
    CAdapter *pAdapter = (CAdapter*)NdisMiniportDataPathHandle;

    ASSERT(pAdapter);

    pAdapter->m_TxMgr.TxSendPauseInd(PortId,
        PeerId,
        ExTidBitmask,
        TxPauseReason);
}

void AdapterTxSendRestartInd(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle,
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ UINT32 ExTidBitmask,
    _In_ WDI_TX_PAUSE_REASON TxRestartReason)
{
    CAdapter *pAdapter = (CAdapter*)NdisMiniportDataPathHandle;

    ASSERT(pAdapter);

    pAdapter->m_TxMgr.TxSendRestartInd(PortId,
        PeerId,
        ExTidBitmask,
        TxRestartReason);
}

void AdapterTxReleaseFramesIndOld(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle,
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ UINT32 ExTidBitmask,
    _In_ UINT16 MaxNumFrames)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_NOT_SUPPORTED;

    TraceEntry();

    UNREFERENCED_PARAMETER(NdisMiniportDataPathHandle);
    UNREFERENCED_PARAMETER(PortId);
    UNREFERENCED_PARAMETER(PeerId);
    UNREFERENCED_PARAMETER(ExTidBitmask);
    UNREFERENCED_PARAMETER(MaxNumFrames);

    TraceExit(ndisStatus);
}

void AdapterTxReleaseFramesInd(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle,
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ UINT32 ExTidBitmask,
    _In_ UINT8 MaxNumFrames,
    _In_ UINT16 Credit,
    _Out_ PNET_BUFFER_LIST *ppNBL)
{
    CAdapter *pAdapter = (CAdapter*)NdisMiniportDataPathHandle;

    ASSERT(pAdapter);

    pAdapter->m_TxMgr.TxReleaseFramesInd(PortId,
        PeerId,
        ExTidBitmask,
        MaxNumFrames,
        Credit,
        ppNBL);
}

void AdapterTxInjectFrameInd(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle,
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ WDI_EXTENDED_TID ExTid,
    _In_ PNET_BUFFER_LIST pNBL,
    _In_ BOOLEAN bIsUnicast,
    _In_ BOOLEAN bUseLegacyRates,
    _In_ UINT16 Ethertype,
    _In_ WDI_EXEMPTION_ACTION_TYPE ExemptionAction)
{
    CAdapter *pAdapter = (CAdapter*)NdisMiniportDataPathHandle;

    ASSERT(pAdapter);

    pAdapter->m_TxMgr.TxInjectIhvFrame(PortId,
                                       PeerId,
                                       ExTid,
                                       pNBL,
                                       bIsUnicast,
                                       bUseLegacyRates,
                                       Ethertype,
                                       ExemptionAction);
}

void AdapterTxAbortConfirm(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle)
{
    CAdapter *pAdapter = (CAdapter*)NdisMiniportDataPathHandle;

    ASSERT(pAdapter);

    //Abort is from IHV miniport.  notify peer/port of confirmation
    pAdapter->m_TxMgr.TxAbortConfirm(TRUE);
}

void AdapterRxInorderDataInd(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle,
    _In_ WDI_RX_INDICATION_LEVEL IndicationLevel,
    _In_ WDI_PEER_ID PeerId,
    _In_ WDI_EXTENDED_TID ExTid,
    _In_ PNDIS_RECEIVE_THROTTLE_PARAMETERS pRxThrottleParams,
    _Out_ PNDIS_STATUS pNdisStatus)
{
    CAdapter *pAdapter = (CAdapter*)NdisMiniportDataPathHandle;

    TraceEntry();

    pAdapter->m_RxMgr.RxInOrderDataInd( IndicationLevel, PeerId, ExTid, pRxThrottleParams, pNdisStatus );

    TraceExit(*pNdisStatus);
}

void AdapterRxStopConfirm(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle)
{
    CAdapter *pAdapter = (CAdapter*)NdisMiniportDataPathHandle;

    pAdapter->m_RxMgr.RxStopConfirm();
}

void AdapterRxFlushConfirm(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle)
{
    CAdapter *pAdapter = (CAdapter*)NdisMiniportDataPathHandle;

    pAdapter->m_RxMgr.RxFlushConfirm();
}

void AdapterPeerCreateInd(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle,
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ WDI_MAC_ADDRESS PeerAddr)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    BOOLEAN bPeerAddedToTxMgr = FALSE;
    BOOLEAN bPeerAddedToRxMgr = FALSE;
    BOOLEAN bPeerAddedToPort = FALSE;
    BOOLEAN bPeerAddedToAdapter = FALSE;

    TraceEntry();

    CAdapter *pAdapter = (CAdapter*)NdisMiniportDataPathHandle;

    ASSERT(pAdapter);

    //Add to TxMgr
    ndisStatus = pAdapter->m_TxMgr.AddPeerId(PortId,PeerId,&PeerAddr);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("TxMgr AddPeerId failed (x%0x)",ndisStatus);
        goto exit;
    }
    bPeerAddedToTxMgr = TRUE;

    //Add to RxMgr
    ndisStatus = pAdapter->m_RxMgr.AddPeerId(PortId, PeerId, &PeerAddr);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("RxMgr AddPeer failed (x%0x)", ndisStatus);
        goto exit;
    }
    bPeerAddedToRxMgr = TRUE;

    //Add to Port
    //TODO
    bPeerAddedToPort = TRUE;

    //Add to Adapter PeerTable
    ndisStatus = pAdapter->m_PeerTable.AddPeer(PortId,PeerId, &PeerAddr);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Adapter AddPeer failed (x%0x)", ndisStatus);
        goto exit;
    }
    bPeerAddedToAdapter = TRUE;

exit:
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("PeerCreateInd failed (x%0x)",ndisStatus);
        if (bPeerAddedToTxMgr)
        {
            pAdapter->m_TxMgr.PeerDeleteBegin(PortId,PeerId);
        }
        if (bPeerAddedToRxMgr)
        {
            pAdapter->m_RxMgr.RemovePeerId(PeerId);
        }
        if (bPeerAddedToPort)
        {
        }
        if (bPeerAddedToAdapter)
        {
            pAdapter->m_PeerTable.RemovePeer(PeerId);
        }
    }
    TraceExit(ndisStatus);
}

void AdapterPeerDeleteInd(
    _In_ NDIS_HANDLE NdisMiniportDataPathHandle,
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _Out_ NDIS_STATUS * pWifiStatus)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    CAdapter *pAdapter = (CAdapter*)NdisMiniportDataPathHandle;

    NT_ASSERT(pWifiStatus);

    ASSERT(pAdapter);

    //Remove from PeerTable
    ndisStatus = pAdapter->m_PeerTable.RemovePeer(PeerId);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("PeerTable RemovePeer failed (0x%0x)", ndisStatus);
    }

    //Remove from RxMgr
    ndisStatus = pAdapter->m_RxMgr.RemovePeerId(PeerId);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("RxMgr RemovePeerId failed (x%0x)", ndisStatus);
    }

    //Begin deletion from TxMgr
    ndisStatus = pAdapter->m_TxMgr.PeerDeleteBegin(PortId,PeerId);
    if (ndisStatus != NDIS_STATUS_SUCCESS &&
        ndisStatus != NDIS_STATUS_PENDING    )
    {
        WFCError("TxMgr PeerDeleteBegin failed (x%0x)",ndisStatus);
    }

    *pWifiStatus = (ndisStatus == NDIS_STATUS_PENDING) ? NDIS_STATUS_PENDING : NDIS_STATUS_SUCCESS;
    TraceExit(ndisStatus);
}

PWDI_FRAME_METADATA AdapterAllocateWifiFrameMetadata(
    _In_  NDIS_HANDLE NdisMiniportDataPathHandle)
{
    PWFC_FRAME pWfcFrame=NULL;
    PWDI_FRAME_METADATA pWdiFrameMetadata=NULL;

    CAdapter* pAdapter = (CAdapter *) NdisMiniportDataPathHandle;
    pWfcFrame = pAdapter->AllocateWfcFrame();

    if ( pWfcFrame != NULL )
    {
        pWdiFrameMetadata = &pWfcFrame->WiFiFrameMetadata;
    }

    return pWdiFrameMetadata;
}

void AdapterFreeWifiFrameMetadata(
    _In_  NDIS_HANDLE NdisMiniportDataPathHandle,
    _In_ PWDI_FRAME_METADATA pWiFiFrameMetaData)
{
    PWFC_FRAME pWfcFrame;
    CAdapter* pAdapter = (CAdapter *) NdisMiniportDataPathHandle;

    if ( pWiFiFrameMetaData != NULL )
    {
        pWfcFrame = WFC_FRAME_FROM_WDI_FRAME( pWiFiFrameMetaData );
        pAdapter->FreeWfcFrame( pWfcFrame );
    } else {
        ASSERT( 0 ); // freeing NULL
    }
}

//
// This stub is used to prevent an AV.  It is only called when the IHV misbehaves and will cause a verifier assert when called.
//
void TxPeerBacklogStub(
    _In_ TAL_TXRX_HANDLE MiniportTalTxRxContext,
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ BOOLEAN bBacklogged)
{
    UNREFERENCED_PARAMETER(MiniportTalTxRxContext);
    UNREFERENCED_PARAMETER(PortId);
    UNREFERENCED_PARAMETER(PeerId);
    UNREFERENCED_PARAMETER(bBacklogged);

    WFCError("TxPeerBacklogStub should never be called with proper IHV miniport behavior. The presence of this trace indicates a misbehaving IHV miniport");

    CSystem::XdvStatelessAssert("IHV WDI miniport called datapath after datapath deinitialization.");
}

CPeerTable::CPeerTable() :
    m_NumEntries(0),
    m_TableSize(0),
    m_PeerTableLock(NULL),
    m_pPeerTable(NULL)
{
    TraceEntry();
}

CPeerTable::~CPeerTable()
{
    TraceEntry();

    if (m_PeerTableLock)
    {
        delete(m_PeerTableLock);
    }

    if (m_pPeerTable)
    {
        delete(m_pPeerTable);
    }
}

NDIS_STATUS CPeerTable::Initialize(
    _In_ UINT8 MaxNumberOfPeers)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    m_TableSize = MaxNumberOfPeers;

    m_pPeerTable = new WFC_PEER_TABLE_ENTRY[m_TableSize];

    if (m_pPeerTable == NULL)
    {
        m_TableSize = 0;
        ndisStatus = NDIS_STATUS_RESOURCES;
        WFCError("[PeerTable]Unable to initialize PeerTable with %d max peers",MaxNumberOfPeers);
        goto exit;
    }

    ndisStatus = CNdisSpinLock::CreateInstance(&m_PeerTableLock);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[PeerTable]Unable to initialize PeerTableLock");
        goto exit;
    }

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CPeerTable::AddPeer(
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ PWDI_MAC_ADDRESS pPeerAddr)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    PWFC_PEER_TABLE_ENTRY pPeerEntry = NULL;

    WDI_MAC_ADDRESS zeroMac = {0};

    TraceEntry();

    if (pPeerAddr == NULL ||
        PortId == WDI_PORT_ANY ||
        PeerId == WDI_PEER_ANY ||
        AreMacAddressEqual(pPeerAddr,&zeroMac))
    {
        WFCError("[PeerTable]Invalid Parameter");
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    if (ETH_IS_MULTICAST(pPeerAddr) && !ETH_IS_BROADCAST(pPeerAddr))
    {
        WFCError("[PeerTable]Group peers must use broadcast address.");
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    if (m_pPeerTable == NULL)
    {
        WFCError("[PeerTable]Error: PeerTable not initialized properly");
        ndisStatus = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }
    {
        CAutoLock lock(m_PeerTableLock);

        pPeerEntry = AssignPeerEntry(PortId, PeerId, pPeerAddr);
    }

    if (pPeerEntry == NULL)
    {
        WFCError("[PeerTable]Unable to assign slot for [Port:%x][Peer:%x][Addr:%!MACADDR!]",
            PortId,
            PeerId,
            pPeerAddr->Address);
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CPeerTable::RemovePeer(
    _In_ WDI_PEER_ID PeerId)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    PWFC_PEER_TABLE_ENTRY pPeerEntry = NULL;
    WFC_PEER_TABLE_ENTRY tempPeerEntry;

    int startIndex = 0;
    int curIndex = 0;

    BOOLEAN bFound = FALSE;

    TraceEntry();

    if (PeerId == WDI_PEER_ANY)
    {
        WFCError("[PeerTable]Invalid Parameter");
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    if (m_pPeerTable == NULL)
    {
        WFCError("[PeerTable]Error: PeerTable not initialized properly");
        ndisStatus = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    {
        CAutoLock lock(m_PeerTableLock);

        startIndex = 0;
        curIndex = 0;
        do
        {
            if (m_pPeerTable[curIndex].bInUse == TRUE &&
                m_pPeerTable[curIndex].PeerId == PeerId  )
            {
                WFCInfo("[PeerTable][PeerId:%x][%!MACADDR!] found in PeerTable, slot %d",
                    PeerId,
                    m_pPeerTable[curIndex].PeerAddr.Address,
                    curIndex);
                bFound = TRUE;
                RtlZeroMemory(&m_pPeerTable[curIndex],sizeof(WFC_PEER_TABLE_ENTRY));
                m_NumEntries--;
            }

            curIndex = (curIndex + 1) % m_TableSize;
        } while ((curIndex != startIndex) && (!bFound));

        //reinsert remaining peers
        while (bFound && m_pPeerTable[curIndex].bInUse)
        {
            //reinsert
            RtlCopyMemory(&tempPeerEntry,
                          &m_pPeerTable[curIndex],
                          sizeof(WFC_PEER_TABLE_ENTRY));
            RtlZeroMemory(&m_pPeerTable[curIndex],
                          sizeof(WFC_PEER_TABLE_ENTRY));

            m_NumEntries--;

            pPeerEntry = AssignPeerEntry(tempPeerEntry.PortId,
                                         tempPeerEntry.PeerId,
                                         &tempPeerEntry.PeerAddr);

            //this should never fail
            ASSERT(pPeerEntry);
            if(pPeerEntry == NULL)
            {
                WFCError("[PeerTable]ERROR reinsertion of PeerEntry failed");
                ndisStatus = NDIS_STATUS_FAILURE;
                goto exit;
            }

            curIndex = (curIndex + 1) % m_TableSize;
        }
    }
exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

WDI_PEER_ID CPeerTable::GetPeerId(
    _In_ PWDI_MAC_ADDRESS pPeerAddr,
    _In_ WDI_PORT_ID PortId)
/*++
--*/
{
    WDI_PEER_ID peerId = WDI_PEER_ANY;

    int startIndex = 0;
    int curIndex = 0;

    WDI_MAC_ADDRESS zeroMac = {0};

    TraceEntry();

    if (AreMacAddressEqual(pPeerAddr,&zeroMac) ||
        PortId == WDI_PORT_ANY)
    {
        WFCError("[PeerTable]Invalid Parameter");
        goto exit;
    }

    //remove? optimize out?
    if (m_pPeerTable == NULL)
    {
        WFCError("[PeerTable]Error: PeerTable not initialized properly");
        goto exit;
    }

    startIndex = hash_func(PortId,pPeerAddr);
    curIndex = startIndex;

    {
        CAutoLock lock(m_PeerTableLock);

        do
        {
            if(m_pPeerTable[curIndex].bInUse == FALSE)
            {
                WFCError("[%!MACADDR!] not found in PeerTable",pPeerAddr->Address);
                goto exit;
            }
            if (DoesPeerMatch(&m_pPeerTable[curIndex], PortId, pPeerAddr))
            {
                peerId = m_pPeerTable[curIndex].PeerId;
                goto exit;
            }
            curIndex = (curIndex + 1) % m_TableSize;
        } while (curIndex != startIndex);
    }

exit:
    TraceExit(peerId == WDI_PEER_ANY ? NDIS_STATUS_FAILURE : NDIS_STATUS_SUCCESS);
    return peerId;
}

PWFC_PEER_TABLE_ENTRY CPeerTable::AssignPeerEntry(
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ PWDI_MAC_ADDRESS pPeerAddr)
/*++
called with lock held
--*/
{
    PWFC_PEER_TABLE_ENTRY pPeerEntry = NULL;

    int startIndex = 0;
    int curIndex = 0;

    TraceEntry();

    if (m_NumEntries == m_TableSize)
    {
        WFCError("[PortId]PeerTable is full");
    }

    startIndex = hash_func(PortId,pPeerAddr);
    curIndex = startIndex;

    do
    {
        ASSERT(m_pPeerTable[curIndex].bInUse == FALSE ||
               m_pPeerTable[curIndex].PeerId != PeerId  );

        if (m_pPeerTable[curIndex].bInUse == TRUE                     &&
            DoesPeerMatch(&m_pPeerTable[curIndex], PortId, pPeerAddr)    )
        {
            WFCError("[PeerTable]Error:[%!MACADDR!] already in PeerTable", pPeerAddr->Address);
            goto exit;
        }

        if (m_pPeerTable[curIndex].bInUse == FALSE)
        {
            pPeerEntry = &m_pPeerTable[curIndex];

            m_pPeerTable[curIndex].bInUse = TRUE;
            m_pPeerTable[curIndex].PortId = PortId;
            m_pPeerTable[curIndex].PeerId = PeerId;
            RtlCopyMemory(&m_pPeerTable[curIndex].PeerAddr,
                          pPeerAddr,
                          sizeof(WDI_MAC_ADDRESS));

            m_NumEntries++;

            WFCInfo("[PeerTable][PortId:%x][PeerId:%x][%!MACADDR!] Assigned to index %d",
                PortId,
                PeerId,
                pPeerAddr->Address,
                curIndex);
            goto exit;
        }
        curIndex = (curIndex + 1) % m_TableSize;
    } while (curIndex != startIndex);

exit:
    TraceExit(pPeerEntry ? NDIS_STATUS_SUCCESS : NDIS_STATUS_FAILURE);
    return pPeerEntry;
}

CFrameIdLookupTable::CFrameIdLookupTable() :
    m_FreeFrameIdStartIndex(0),
    m_NumFrameIdAssigned(0),
    m_FrameIdLock(NULL),
    m_Initialized(FALSE)
{
    int i;

    TraceEntry();

    for (i = 0; i < c_NumLookupEntries; i++)
    {
        InitializeListHead(&m_FrameListHeadArray[i]);
    }

    RtlZeroMemory(&m_FrameIdBitArray,sizeof(m_FrameIdBitArray));
}

CFrameIdLookupTable::~CFrameIdLookupTable()
{
    TraceEntry();

    if (m_FrameIdLock)
    {
        delete(m_FrameIdLock);
    }
}

NDIS_STATUS CFrameIdLookupTable::Initialize()
/*++
    Initialize FrameIdLookupTable
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    ndisStatus = CNdisSpinLock::CreateInstance(&m_FrameIdLock);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[WDI]Unable to initialize FrameIdLookupTable");
        goto exit;
    }

    m_Initialized = TRUE;

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

UINT32 CFrameIdLookupTable::LogFramesWithOldFrameNum(
    _In_ UINT64 MinFrameNum)
/*++
    Find and log frames with FrameNum less than MinFrameNum
    Note that the FrameIdLookupTable uses FrameId which is different
    than FrameNum.  This can cause some corner cases where the logging might not
    catch everything (or catch everything older than one time period, it might require two
    time periods in some situations)
--*/
{
    UINT32 numOldFramesFound = 0;

    TraceEntry();

    { //m_FrameIdLock
        CAutoLock lock(m_FrameIdLock);

        for (UINT i = 0; i < c_NumLookupEntries; i++)
        {
            PLIST_ENTRY pHeadEntry = &m_FrameListHeadArray[i];

            if (!IsListEmpty(pHeadEntry))
            {
                PLIST_ENTRY pCurEntry = pHeadEntry->Flink;

                while (pCurEntry != pHeadEntry)
                {
                    PWFC_FRAME pWfcFrame = CONTAINING_RECORD(pCurEntry, WFC_FRAME, u.Tx.FrameTableLink);

                    if (pWfcFrame->FrameNum < MinFrameNum)
                    {
                        numOldFramesFound++;
                        WFCError("[WDI]Old frame detected, pNBL = %p [FrameNum:%llu][FrameId:%0x][IsTransferComplete:%x][IsSendComplete:%x][Port:0x%x][Peer:0x%x][Tid:0x%x]",
                            pWfcFrame->WiFiFrameMetadata.pNBL,
                            pWfcFrame->FrameNum,
                            pWfcFrame->WiFiFrameMetadata.FrameID,
                            pWfcFrame->u.Tx.bIsTransferStatusSet,
                            pWfcFrame->u.Tx.bIsSendStatusSet,
                            pWfcFrame->WiFiFrameMetadata.u.txMetaData.PortID,
                            pWfcFrame->WiFiFrameMetadata.u.txMetaData.PeerID,
                            pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExTID);
                    }
                    else if (pWfcFrame->FrameNum > MinFrameNum + WFC_DATAPATH_CONSTANT::FrameTableTimerOverscan)
                    {
                        //The order of FrameNums in this list may not always be increasing since
                        // threads can race with each other between the AssignAndMapFrameId and
                        // ProcessNBL however the design decision is to prioritize making this 
                        // low cost so this approximation is acceptable here.'
                        //A overscan is used to both limit the error as well as provide an upper bound
                        // on how many frames are searched.  After the timer fires again any missed
                        // frames (if they are still outstanding) would be flagged.
                        //So some frames older than the period of the timer might not be flagged
                        // but frames older than twice the period will definitely be flagged

                        //While extremely unlikely it is possible for the FrameNum to wrap around
                        //this could cause some frames to be logged as old which are not old
                        // (MinFrameNum is high and then FrameNum wraps)
                        // this will be limited to a single time period and does not have
                        // severe enough impact to warrant handling the case better
                        break;
                    }

                    pCurEntry = pCurEntry->Flink;
                }
            }
        }

    } //m_FrameIdLock

    TraceExit(NDIS_STATUS_SUCCESS);
    return numOldFramesFound;
}

//optimize for speed over size. at end of function reset optimizations to whatever /O compiler option was
//these 5 functions are short however they account for substantial cost on Tx path (especially when outstanding frame count is high)
//on some systems optimizing for speed helped and did not increase code size significantly (no increase on amd64)
#pragma optimize("t",on)
NDIS_STATUS CFrameIdLookupTable::AssignAndMapFrameId(
        _In_ PWFC_FRAME pWfcFrame)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WDI_FRAME_ID frameId = (WDI_FRAME_ID)-1;

    {
        CAutoLock lock(m_FrameIdLock);

        if (m_NumFrameIdAssigned >= c_NumFrameIds)
        {
            ndisStatus = NDIS_STATUS_RESOURCES;
            WFCError("[WDI]All frameIds in use");
            goto exit;
        }

        m_NumFrameIdAssigned++;
        frameId = GetNextFrameId();

        pWfcFrame->WiFiFrameMetadata.FrameID = frameId;

        InsertTailList(&m_FrameListHeadArray[(frameId >> c_FrameLookupShift) % c_NumLookupEntries], &pWfcFrame->u.Tx.FrameTableLink);
    }

    WFCTrace("[WDI]Assigned Frame ID %0x", frameId);

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}
#pragma optimize("",on)

#pragma optimize("t",on)
NDIS_STATUS CFrameIdLookupTable::FreeAndUnmapFrameId(
        _In_ PWFC_FRAME pWfcFrame)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WDI_FRAME_ID frameId = (WDI_FRAME_ID)-1;


    frameId = pWfcFrame->WiFiFrameMetadata.FrameID;

    {
        CAutoLock lock(m_FrameIdLock);

        RemoveEntryList(&pWfcFrame->u.Tx.FrameTableLink);

        //Free FrameID
        FreeFrameId(frameId);

        WFCTrace("[WDI]Free Frame ID %0x", frameId);

        m_NumFrameIdAssigned--;
    }

    return ndisStatus;
}
#pragma optimize("",on)

#pragma optimize("t",on)
PNET_BUFFER_LIST CFrameIdLookupTable::FindNblByFrameId(
        _In_ WDI_FRAME_ID FrameId)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
    PNET_BUFFER_LIST pNBL = NULL;

    PLIST_ENTRY pHeadEntry = &m_FrameListHeadArray[(FrameId >> c_FrameLookupShift) % c_NumLookupEntries];
    PLIST_ENTRY pCurEntry = NULL;

    PWFC_FRAME pWfcFrame = NULL;

    {
        CAutoLock lock(m_FrameIdLock);

        if (IsListEmpty(pHeadEntry))
        {
            WFCError("[WDI]FrameLookupTable list is empty, frameId %0x not found",FrameId);
            ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
            goto exit;
        }

        pCurEntry = pHeadEntry->Flink;

        while (pCurEntry != pHeadEntry)
        {
            pWfcFrame = CONTAINING_RECORD(pCurEntry, WFC_FRAME, u.Tx.FrameTableLink);

            if (pWfcFrame->WiFiFrameMetadata.FrameID == FrameId)
            {
                pNBL = pWfcFrame->WiFiFrameMetadata.pNBL;
                WFCTrace("[WDI]Found NBL (%p) with frameId %0x", pNBL, FrameId);
                ndisStatus = NDIS_STATUS_SUCCESS;
                goto exit;
            }

            pCurEntry = pCurEntry->Flink;
        }
    }

exit:
    TraceExit(ndisStatus);
    return pNBL;
}
#pragma optimize("",on)

#pragma optimize("t",on)
//A frame ID will exist.
__forceinline WDI_FRAME_ID CFrameIdLookupTable::GetNextFrameId()
{
    TraceEntry();

    ULONG i;
    ULONG index = c_NumEntries;
    ULONG offset = c_BitsInUint32;

    //start at m_FreeFrameIdStartIndex because there is guaranteed to be no free FrameId in a lower index
    for (i = m_FreeFrameIdStartIndex; i < c_NumEntries; i++)
    {
        //FrameIds in use are represented by 1's.  0's represent frees. store the inversion in a local variable here
        // as it is more convinient for the free FrameIds to be 1's in the function
        const UINT32 entry = ~m_FrameIdBitArray[i];

        //check if there are any free frameIds in this index
        if (entry)
        {
            //there is guaranteed to be at least one bit set (because the entry is non-zero)
            //find the first clear bit in the FrameIdBitArray at current index (i)
            _BitScanForward(&offset, entry);
            index = i;

            m_FrameIdBitArray[index] |= (1 << offset);

            //update m_FreeFrameIdStartIndex. simplest is to set to this index.
            // a more complex approach would be to set to the next index if this was the last frameId in this index
            // however the conditional might be more expensive than the extra compare.
            m_FreeFrameIdStartIndex = index;
            return (WDI_FRAME_ID)(index * c_BitsInUint32 + offset);
        }
    }

    return (WDI_FRAME_ID)-1;
}
#pragma optimize("",on)

#pragma optimize("t",on)
__forceinline void CFrameIdLookupTable::FreeFrameId(
    _In_ WDI_FRAME_ID FrameId)
{
    TraceEntry();

    UINT32 index = FrameId / c_BitsInUint32;
    UINT32 offset = FrameId % c_BitsInUint32;

    m_FrameIdBitArray[index] &= ~(1 << offset);

    //update m_FreeFrameIdStartIndex
    if (index < m_FreeFrameIdStartIndex)
    {
        m_FreeFrameIdStartIndex = index;
    }
}
#pragma optimize("",on)
