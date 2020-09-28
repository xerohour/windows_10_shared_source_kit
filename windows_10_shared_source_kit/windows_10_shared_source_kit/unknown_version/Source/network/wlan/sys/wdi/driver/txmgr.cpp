/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    txmgr.cpp

Abstract:

    Contains code for the WDI TxMgr.

Environment:

    Kernel mode

Author:


--*/
#include "precomp.hpp"
#include "txmgr.tmh"

NDIS_IO_WORKITEM_FUNCTION TxMgrProcessPendingFrames;

NDIS_IO_WORKITEM_FUNCTION TxMgrDeferSendCompletion;

CTxEntryTable::CTxEntryTable() :
    m_NumEntries(0),
    m_TableSize(0),
    m_pTxEntryTable(NULL),
    m_QueueingMode(PEER_QUEUEING)
/*++
--*/
{
}

CTxEntryTable::~CTxEntryTable()
/*++
--*/
{
    if (m_pTxEntryTable)
    {
        for (UINT i = 0; i < m_TableSize; i++)
        {
            if (m_pTxEntryTable[i].ppTxQueue)
            {
                //in case IHV did not delete all peers cleanup any remaining TxQueues
                for (UINT j = 0; j < m_pTxEntryTable[i].MaxQueueCount; j++)
                {
                    PWFC_TX_QUEUE pTxQueue = (m_pTxEntryTable[i].ppTxQueue)[j];
                    if (pTxQueue)
                    {
                        delete pTxQueue;
                        pTxQueue = NULL;
                        (m_pTxEntryTable[i].ppTxQueue)[j] = NULL;
                    }
                }
                delete[] m_pTxEntryTable[i].ppTxQueue;
                m_pTxEntryTable[i].ppTxQueue = NULL;
            }
        }
        delete [] m_pTxEntryTable;
        m_pTxEntryTable = NULL;
    }
}

NDIS_STATUS CTxEntryTable::Initialize(
    _In_ CAdapter * pAdapter,
    _In_ UINT8 MaxNumberOfEntries,
    _In_ WFC_TX_QUEUEING_MODE QueueingMode)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN bCleanupNeeded = FALSE;

    TraceEntry();

    m_pAdapter = pAdapter;
    m_QueueingMode = QueueingMode;

    m_TableSize = MaxNumberOfEntries;

    m_pTxEntryTable = new WFC_TX_ENTRY[m_TableSize];

    if (m_pTxEntryTable == NULL)
    {
        m_TableSize = 0;
        ndisStatus = NDIS_STATUS_RESOURCES;
        WFCError("[txmgr]Unable to initialize TxEntryTable with %d max entries", MaxNumberOfEntries);
        goto exit;
    }

    for (UINT i = 0; i < m_TableSize; i++)
    {
        if (QueueingMode == PORT_QUEUEING)
        {
            m_pTxEntryTable[i].ppTxQueue = new WFC_TX_QUEUE*[WFC_DATAPATH_CONSTANT::NumPortQueues];
            m_pTxEntryTable[i].MaxQueueCount = WFC_DATAPATH_CONSTANT::NumPortQueues;
        }
        else
        {
            m_pTxEntryTable[i].ppTxQueue = new WFC_TX_QUEUE*[WFC_DATAPATH_CONSTANT::NumExtendedTids];
            m_pTxEntryTable[i].MaxQueueCount = WFC_DATAPATH_CONSTANT::NumExtendedTids;
        }
        if (m_pTxEntryTable[i].ppTxQueue == NULL)
        {
            bCleanupNeeded = TRUE;
            ndisStatus = NDIS_STATUS_RESOURCES;
            WFCError("[txmgr]Unable to initialize TxEntryTable with %d max entries", MaxNumberOfEntries);
            goto exit;
        }
    }

exit:
    if (bCleanupNeeded)
    {
        for (UINT i = 0; i < m_TableSize; i++)
        {
            if (m_pTxEntryTable[i].ppTxQueue)
            {
                delete[] m_pTxEntryTable[i].ppTxQueue;
                m_pTxEntryTable[i].ppTxQueue = NULL;
            }
        }
        m_TableSize = 0;
        delete[] m_pTxEntryTable;
        m_pTxEntryTable = NULL;
    }
    TraceExit(ndisStatus);
    return ndisStatus;
}

PWFC_TX_ENTRY CTxEntryTable::AssignTxEntry(
    _In_ WFC_TX_ID TxId)
/*++
--*/
{
    PWFC_TX_ENTRY pTxEntry = NULL;

    int startIndex = 0;
    int curIndex = 0;

    TraceEntry();

    if (m_NumEntries == m_TableSize)
    {
        WFCError("[txmgr]TxEntryTable is full");
        goto exit;
    }

    if (TxId == WFC_TX_ID_ANY)
    {
        WFCError("[txmgr]Invalid Parameter");
        goto exit;
    }

    if (m_pTxEntryTable == NULL)
    {
        WFCError("[txmgr]Error: TxEntryTable not initialized properly");
        goto exit;
    }

    startIndex = hash_func(TxId);
    curIndex = startIndex;

    do
    {
        ASSERT(m_pTxEntryTable[curIndex].bInUse == FALSE ||
               m_pTxEntryTable[curIndex].TxId != TxId);

        if (m_pTxEntryTable[curIndex].bInUse == TRUE &&
            m_pTxEntryTable[curIndex].TxId == TxId      )
        {
            WFCError("[txmgr]Error:[TxId:%x] already in TxEntryTable",TxId);
            goto exit;
        }

        if (m_pTxEntryTable[curIndex].bInUse == FALSE)
        {
            pTxEntry = &m_pTxEntryTable[curIndex];
            m_pTxEntryTable[curIndex].bInUse = TRUE;
            m_pTxEntryTable[curIndex].TxId = TxId;
            m_NumEntries++; //TODO SHOULD THIS BE IN INITIALIZE? avoiding a --++ in the remove case.

            WFCInfo("[txmgr][TxId:%x] Assigned to index %d",TxId,curIndex);

            goto exit;
        }

        curIndex = (curIndex + 1) % m_TableSize;
    } while (curIndex != startIndex);

exit:
    TraceExit(pTxEntry ? NDIS_STATUS_SUCCESS : NDIS_STATUS_FAILURE);
    return pTxEntry;
}

NDIS_STATUS CTxEntryTable::InitializeTxEntry(
    _In_ WFC_TX_ID TxId,
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ PWDI_MAC_ADDRESS pMacAddr)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    PWFC_TX_ENTRY pTxEntry = NULL;

    WDI_MAC_ADDRESS zeroMac = {0};

    WDI_TX_PAUSE_REASON initialPauseState = WDI_TX_PAUSE_REASON_NULL;

    int i;

    TraceEntry();

    if (pMacAddr == NULL                       ||
        TxId == WFC_TX_ID_ANY                  ||
        PortId == WDI_PORT_ANY                 ||
        AreMacAddressEqual(pMacAddr,&zeroMac)     )
    {
        WFCError("[txmgr]Invalid Parameter");
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    if (m_pTxEntryTable == NULL)
    {
        WFCError("[txmgr]Error: TxEntryTable not initialized properly");
        goto exit;
    }

    pTxEntry = AssignTxEntry(TxId);

    if (pTxEntry == NULL)
    {
        WFCError("[txmgr]Unable to assign slot for TxId:%x [Port:%x][Peer:%x][Addr:%!MACADDR!]",
            TxId,
            PortId,
            PeerId,
            pMacAddr->Address);
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    pTxEntry->PeerId = PeerId;
    pTxEntry->PortId = PortId;
    RtlCopyMemory(&pTxEntry->MacAddr,
                  pMacAddr,
                  sizeof(WDI_MAC_ADDRESS));

    pTxEntry->InOrderNotifPendingBitmask = 0;

    //determine initial TidPauseState
    // start in a paused state (PEER_CREATE) if version > .90 and peer_queueing mode
    if (m_QueueingMode == PORT_QUEUEING)
    {
        initialPauseState = WDI_TX_PAUSE_REASON_NULL;
    }
    else
    {
        initialPauseState = WDI_TX_PAUSE_REASON_PEER_CREATE;
    }
    //intialize TidPauseState array
    for (i = 0; i < WFC_DATAPATH_CONSTANT::NumExtendedTids; i++)
    {
        pTxEntry->TidPauseState[i] = initialPauseState;
    }

    pTxEntry->TxEntryState = TX_ENTRY_STARTED;

    WFCInfo("[txmgr]Initialized TxEntry for [TxId:%x][Port:%x][Peer:%x][Addr:%!MACADDR!]",
        TxId,
        PortId,
        PeerId,
        pMacAddr->Address);

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

PWFC_TX_ENTRY CTxEntryTable::GetTxEntry(
    _In_ WFC_TX_ID TxId)
/*++
--*/
{
    PWFC_TX_ENTRY pTxEntry = NULL;

    int startIndex = 0;
    int curIndex  = 0;

    if (TxId == WFC_TX_ID_ANY)
    {
        WFCError("[txmgr]Invalid Parameter");
        goto exit;
    }

    if (m_pTxEntryTable == NULL)
    {
        WFCError("[txmgr]Error: TxEntryTable not initialized properly");
        goto exit;
    }

    startIndex = hash_func(TxId);
    curIndex = startIndex;

    do
    {
        if (m_pTxEntryTable[curIndex].bInUse == FALSE)
        {
            WFCError("[txmgr][TxId:%x] not found in TxEntryTable", TxId);
            goto exit;
        }
        if (m_pTxEntryTable[curIndex].TxId == TxId)
        {
            WFCTrace("[txmgr][TxId:%x] found in TxEntryTable", TxId);
            pTxEntry = &m_pTxEntryTable[curIndex];
            goto exit;
        }
        curIndex = (curIndex + 1) % m_TableSize;
    } while (curIndex != startIndex);

exit:
    TraceExit(pTxEntry ? NDIS_STATUS_SUCCESS : NDIS_STATUS_FAILURE);
    return pTxEntry;
}

PWFC_TX_ENTRY CTxEntryTable::GetNextPortPeerEntry(
        _In_ WDI_PORT_ID PortId,
        _Inout_ UINT *pIndex)
/*++
    Only valid for PeerQueueing
    linearly searches through table for valid TxEntry which matches the portId.
    uses pIndex to keep track of starting place.  *pIndex should be set to 0 initially
--*/
{
    PWFC_TX_ENTRY pTxPeerEntry = NULL;

    UINT i;

    TraceEntry();

    if (PortId == WDI_PORT_ANY ||
        *pIndex >= m_TableSize    )
    {
        WFCError("[txmgr]Invalid Parameter");
        goto exit;
    }

    if (m_QueueingMode == PORT_QUEUEING)
    {
        WFCError("[txmgr]GetNextPortPeerEntry not permitted in PortQueueing mode");
        goto exit;
    }

    for (i = *pIndex; i < m_TableSize; i++)
    {
        if (m_pTxEntryTable[i].bInUse == TRUE &&
            m_pTxEntryTable[i].PortId == PortId)
        {
            WFCTrace("[txmgr][TxId:%x] is next in table for [Port:%x]",
                m_pTxEntryTable[i].TxId,
                PortId);

            pTxPeerEntry = &m_pTxEntryTable[i];
            //start at next slot next time
            *pIndex = i;
            (*pIndex)++;
            break;
        }
    }

exit:
    TraceExit(NDIS_STATUS_SUCCESS);
    return pTxPeerEntry;
}


PWFC_TX_ENTRY CTxEntryTable::GetNextTxEntry(
    _Inout_ UINT *pIndex)
/*++
    linearly searches through table for valid TxEntry.
    uses pIndex to keep track of starting place.  *pIndex should be set to 0 initially
--*/
{
    PWFC_TX_ENTRY pTxEntry = NULL;

    UINT i;

    TraceEntry();

    if (*pIndex >= m_TableSize    )
    {
        WFCError("[txmgr]Invalid Parameter");
        goto exit;
    }

    for(i = *pIndex; i < m_TableSize; i++)
    {
        if (m_pTxEntryTable[i].bInUse == TRUE)
        {
            WFCTrace("[txmgr][TxId:%x] is next in table",
                m_pTxEntryTable[i].TxId);

            pTxEntry = &m_pTxEntryTable[i];
            //start at next slot next time
            *pIndex = i;
            (*pIndex)++;
            break;
        }
    }

exit:
    TraceExit(NDIS_STATUS_SUCCESS);
    return pTxEntry;
}

NDIS_STATUS CTxEntryTable::RemoveTxEntry(
    _In_ WFC_TX_ID TxId)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    PWFC_TX_ENTRY pTxEntry = NULL;
    WFC_TX_ENTRY tempTxEntry;

    int startIndex = 0;
    int curIndex = 0;

    BOOLEAN bFound = FALSE;

    TraceEntry();

    if (TxId == WFC_TX_ID_ANY)
    {
        WFCError("[txmgr]Invalid Parameter");
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    if (m_pTxEntryTable == NULL)
    {
        WFCError("[txmgr]Error: TxEntryTable not initialized properly");
        goto exit;
    }

    startIndex = hash_func(TxId);
    curIndex = startIndex;

    do
    {
        //TxEntry not in table
        if (m_pTxEntryTable[curIndex].bInUse == FALSE)
        {
            WFCError("[txmgr][TxId:%x] not found in TxEntryTable",TxId);
            ndisStatus = NDIS_STATUS_FAILURE;
            goto exit;
        }
        if (m_pTxEntryTable[curIndex].TxId == TxId)
        {
            WFCInfo("[txmgr][TxId:%x] found in TxEntryTable, slot %d", TxId, curIndex);
            bFound = TRUE;
            //do not zero ppTxQueue or MaxQueueCount
            RtlZeroMemory(&m_pTxEntryTable[curIndex], RTL_SIZEOF_THROUGH_FIELD(WFC_TX_ENTRY, TidPauseState));
            m_NumEntries--;
        }

        curIndex = (curIndex + 1) % m_TableSize;
    } while ((curIndex != startIndex) && (!bFound));

    //reinsert remaining TxEntries
    while (m_pTxEntryTable[curIndex].bInUse == TRUE)
    {
        //reinsert and swap pointers to pTxQueue array
        RtlCopyMemory(&tempTxEntry,
                     &m_pTxEntryTable[curIndex],
                     sizeof(WFC_TX_ENTRY));
        RtlZeroMemory(&m_pTxEntryTable[curIndex],
                      sizeof(WFC_TX_ENTRY));

        m_NumEntries--;

        pTxEntry = AssignTxEntry(tempTxEntry.TxId);

        //this should never fail
        ASSERT(pTxEntry);
        if(pTxEntry == NULL)
        {
            WFCError("[txmgr]ERROR reinsertion of TxEntry failed");
            ndisStatus = NDIS_STATUS_FAILURE;
            goto exit;
        }

        //copy ppTxQueue from new slot to old slot
        m_pTxEntryTable[curIndex].ppTxQueue = pTxEntry->ppTxQueue;
        RtlCopyMemory(pTxEntry,
                      &tempTxEntry,
                      sizeof(WFC_TX_ENTRY));

        curIndex = (curIndex + 1) % m_TableSize;
    }

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

CACList::CACList()
{
    TraceEntry();

    //initialize ACEntries
    for (UINT i = 0; i < c_NumAcEntries; i++)
    {
        WFC_ACCESS_CATEGORY ac = (WFC_ACCESS_CATEGORY)i;
        UINT16 pri = MapAccessCategoryToPriority(ac);

        m_ACList[pri].Ac = ac;
        m_ACList[pri].Priority = pri;
        m_ACList[pri].TxQueueCount = 0;
        InitializeListHead(&m_ACList[pri].m_TxQueueList);
    }

    m_TopIndex = 0;

    TraceExit(NDIS_STATUS_SUCCESS);
}

//Clean up AC lists
CACList::~CACList()
{
    TraceEntry();

    RemoveAllEntries();

    TraceExit(NDIS_STATUS_SUCCESS);
}

void CACList::RemoveAllEntries()
/*++
    removes all entries from all ACEntries
--*/
{
    PLIST_ENTRY pCurrentEntry = NULL;
    UINT i = 0;

    TraceEntry();

    for (i = 0; i < c_NumAcEntries; i++)
    {
        while (!IsListEmpty(&m_ACList[i].m_TxQueueList))
        {
            pCurrentEntry = RemoveHeadList(&m_ACList[i].m_TxQueueList);

            InitializeListHead(pCurrentEntry);
        }
    }

    TraceExit(NDIS_STATUS_SUCCESS);
}

NDIS_STATUS CACList::AddTxQueue(
    _In_ PWFC_TX_QUEUE pTxQueue)
/*++
    Adds TxQueue to the tail of the appropriate AC entry
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    UINT16 index = 0;

    TraceEntry();

    if (pTxQueue == NULL)
    {
        WFCError("[txmgr]Invalid parameter");
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if (!IsListEmpty(&pTxQueue->ACListEntry))
    {
        NT_ASSERTMSG("Queue already in ACList.", FALSE);
        WFCError("[txmgr]TxQueue already in ACList");
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    index = MapAccessCategoryToPriority(pTxQueue->Ac);

    NT_ASSERT(m_ACList[index].Ac == pTxQueue->Ac);
    
    InsertTailList(&m_ACList[index].m_TxQueueList, &pTxQueue->ACListEntry);
    m_ACList[index].TxQueueCount++;

    if (index > m_TopIndex)
    {
        m_TopIndex = index;
    }

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CACList::RemoveTxQueue(
    _In_ PWFC_TX_QUEUE pTxQueue)
/*++
    Removes a TxQueue from the appropriate AC entry
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    UINT16 index = 0;
    TraceEntry();

    if (pTxQueue == NULL)
    {
        WFCError("[txmgr]Invalid parameter");
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    //if TxQueue ACListEntry is not in a list
    if (IsListEmpty(&pTxQueue->ACListEntry))
    {
        WFCInfo("[txmgr]TxQueue was not in the AcListEntry");
        ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
        goto exit;
    }

    RemoveEntryList(&pTxQueue->ACListEntry);
    InitializeListHead(&pTxQueue->ACListEntry);

    index = MapAccessCategoryToPriority(pTxQueue->Ac);

    NT_ASSERT(m_ACList[index].Ac == pTxQueue->Ac);

    m_ACList[index].TxQueueCount--;

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

PWFC_TX_QUEUE CACList::GetTxQueueFromMaxAC()
{
    PWFC_TX_QUEUE pTxQueue = NULL;
    PLIST_ENTRY pFirstEntry = NULL;

    UINT i;

    TraceEntry();

    //priority order(last entry to first)
    i = m_TopIndex;
    while (i != 0)
    {
        if (!IsListEmpty(&m_ACList[i].m_TxQueueList))
        {
            pFirstEntry = RemoveHeadList(&m_ACList[i].m_TxQueueList);
            pTxQueue = CONTAINING_RECORD(pFirstEntry, WFC_TX_QUEUE, ACListEntry);
            InsertTailList(&m_ACList[i].m_TxQueueList, &pTxQueue->ACListEntry);

            break;
        }
        i--;
    }

    m_TopIndex = i;

    TraceExit(pTxQueue ? NDIS_STATUS_SUCCESS : NDIS_STATUS_FILE_NOT_FOUND);
    return pTxQueue;

}

CGlobalTxQueueList::CGlobalTxQueueList() :
    m_TxQueueCount(0)
{
    TraceEntry();

    InitializeListHead(&m_TxQueueList);

    TraceExit(NDIS_STATUS_SUCCESS);
}

CGlobalTxQueueList::~CGlobalTxQueueList()
/*++
--*/
{

    TraceEntry();

    RemoveAllEntries();

    TraceExit(NDIS_STATUS_SUCCESS);
}

void CGlobalTxQueueList::RemoveAllEntries()
/*++
    removes all entries from the GlobalTxQueueList
--*/
{
    PLIST_ENTRY pCurrentEntry;

    TraceEntry();

    while (!IsListEmpty(&m_TxQueueList))
    {
        pCurrentEntry = RemoveHeadList(&m_TxQueueList);

        InitializeListHead(pCurrentEntry);
    }

    TraceExit(NDIS_STATUS_SUCCESS);
    return;
}

NDIS_STATUS CGlobalTxQueueList::AddTxQueue(
    _In_ PWFC_TX_QUEUE pTxQueue)
/*++
    Adds TxQueue to the tail of the GlobalTxQueueList
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    if (pTxQueue == NULL)
    {
        WFCError("[txmgr]Invalid parameter");
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    if (!IsListEmpty(&pTxQueue->GlobalTxQueueListEntry))
    {
        NT_ASSERTMSG("Queue already in ACList.", FALSE);
        WFCError("[txmgr]TxQueue already in GlobalTxQueueList");
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    InsertTailList(&m_TxQueueList, &pTxQueue->GlobalTxQueueListEntry);
    m_TxQueueCount++;
exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CGlobalTxQueueList::RemoveTxQueue(
    _In_ PWFC_TX_QUEUE pTxQueue)
/*++
    Removes a TxQueue from the GlobalTxQueueList
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    if (pTxQueue == NULL)
    {
        WFCError("[txmgr]Invalid parameter");
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    //if TxQueue GlobalPeeTIDListEntry is not in a list
    if (IsListEmpty(&pTxQueue->GlobalTxQueueListEntry))
    {
        WFCInfo("[txmgr]TxQueue was not in the GlobalTxQueueList");
        ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
        goto exit;
    }

    RemoveEntryList(&pTxQueue->GlobalTxQueueListEntry);
    InitializeListHead(&pTxQueue->GlobalTxQueueListEntry);

    m_TxQueueCount--;

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

PWFC_TX_QUEUE CGlobalTxQueueList::GetFirstTxQueue(
    )
/*++
    Returns the TxQueue at the head of the list and moves it to the tail.
    Returns null if the list is empty
--*/
{
    PWFC_TX_QUEUE pTxQueue = NULL;
    PLIST_ENTRY pFirstEntry = NULL;
    TraceEntry();

    if (IsListEmpty(&m_TxQueueList))
    {
        WFCTrace("[txmgr]GlobalTxQueueList is empty");
        goto exit;
    }

    pFirstEntry = RemoveHeadList(&m_TxQueueList);

    pTxQueue = CONTAINING_RECORD(pFirstEntry, WFC_TX_QUEUE, GlobalTxQueueListEntry);

    InsertTailList(&m_TxQueueList,&pTxQueue->GlobalTxQueueListEntry);

    WFCTrace("[txmgr][Port:%x][Peer:%x][Tid:%u] at head of GlobalTxQueueList. Moving to tail",
        pTxQueue->PortId,
        pTxQueue->PeerId,
        pTxQueue->Tid);

exit:
    TraceExit(NDIS_STATUS_SUCCESS);
    return pTxQueue;
}

NDIS_STATUS CGlobalTxQueueList::MoveTxQueueToTail(
    _In_ PWFC_TX_QUEUE pTxQueue)
/*++
    Moves the TxQueue to the tail of the list.  Called by TxMgr when priority scheduling to move
    serviced queues to tail so global DRR will service older queues first
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    if (pTxQueue == NULL)
    {
        WFCError("[txmgr]Invalid parameter");
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    //if TxQueue GlobalTxQueueListEntry is not in a list
    if (IsListEmpty(&pTxQueue->GlobalTxQueueListEntry))
    {
        WFCInfo("[txmgr]TxQueue was not in the GlobalTxQueueList");
        ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
        goto exit;
    }

    RemoveEntryList(&pTxQueue->GlobalTxQueueListEntry);
    InsertTailList(&m_TxQueueList, &pTxQueue->GlobalTxQueueListEntry);

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

//TODO add a state for initialization??
CTxMgr::CTxMgr() :
    m_pAdapter(NULL),
    m_MaxOutstandingTransfers(0),
    m_SendThreadActive(0),
    m_SchedulingRound(0),
    m_ServiceCounter(0),
    m_CurActiveBacklog(0),
    m_CurOutstandingTransfers(0),
    m_CurWifiFrameNumber(0),
    m_PrevWifiFrameNumber(0),
    m_TxMgrLock(NULL),
    m_TxTargetDescLock(NULL),
    m_TxTargetDescDeinitLock(NULL),
    m_TxPrequeueLock(NULL),
    m_TotalPrequeueCount(0),
    m_CGlobalTxQueueList(),
    m_ProcessPendingFramesWorkitemScheduled(0),
    m_hSendCompleteWorkItem(NULL),
    m_LastProcessedPortIndex(0),
    m_BackFillSize(0),
    m_OldFrameDetectionTimerToken(WFC_TIMER_REGISTRATION_TOKEN_INVALID),
    m_pOldFrameDetectionTimerRegistrationContext(NULL),
    m_bAllowOldFrameTimerQueueing(true),
    m_bIsOldFrameDetected(false)
/*++
--*/
{
    InitializeListHead(&m_TxAbortRequestQueue);
}

CTxMgr::~CTxMgr()
/*++
--*/
{
    PLIST_ENTRY pListEntry = NULL;
    PWFC_TX_ABORT_REQUEST pTxAbortRequest = NULL;

    if (m_pACList)
    {
        delete(m_pACList);
    }

    if (m_hSendCompleteWorkItem != NULL)
    {
        NdisFreeIoWorkItem(m_hSendCompleteWorkItem);
        m_hSendCompleteWorkItem = NULL;
    }

    if (m_TxMgrLock)
    {
        delete(m_TxMgrLock);
    }

    if (m_TxTargetDescLock)
    {
        delete(m_TxTargetDescLock);
    }

    if (m_TxTargetDescDeinitLock)
    {
        delete(m_TxTargetDescDeinitLock);
    }

    if (m_TxPrequeueLock)
    {
        delete(m_TxPrequeueLock);
    }

    //The timer should have been deregistered before this point
    if (m_OldFrameDetectionTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        NT_ASSERT(FALSE);
        WFCError("[txmgr]The old frame detection timer was not cleaned up correctly.");
    }

    while (!IsListEmpty(&m_TxAbortRequestQueue))
    {
        pListEntry = RemoveHeadList(&m_TxAbortRequestQueue);
        pTxAbortRequest = (PWFC_TX_ABORT_REQUEST)CONTAINING_RECORD(pListEntry, WFC_TX_ABORT_REQUEST, AbortListEntry);

        WFCInfo("[txmgr]Abort request for [PortId:0x%x][PeerId:0x%x] still in queue",
            pTxAbortRequest->PortId,
            pTxAbortRequest->PeerId);
        delete pTxAbortRequest;
        pTxAbortRequest = NULL;
    }
}

NDIS_STATUS CTxMgr::Initialize(
    _In_ CAdapter* pAdapter,
    _In_ PWFC_DATAPATH_CAPABILITIES pDatapathCapabilities,
    _In_ UINT8 MaxNumPorts)
/*++
    Initializes TxMgr.

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    ASSERT(pAdapter);
    ASSERT(pDatapathCapabilities);
    ASSERT(pDatapathCapabilities->MaxNumPeers > 0);

    if (pAdapter == NULL                        ||
        pDatapathCapabilities == NULL           ||
        pDatapathCapabilities->MaxNumPeers == 0 ||
        MaxNumPorts == 0                           )
    {
        WFCError("[txmgr]Invalid Parameters");
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    m_pAdapter = pAdapter;

    m_pDatapathCapabilities = pDatapathCapabilities;
    m_MaxNumPorts = MaxNumPorts;

    m_QueueingMode = (m_pDatapathCapabilities->TxTargetPriorityQueueing) ? PORT_QUEUEING : PEER_QUEUEING;

    WFCInfo("[txmgr]Queueing Mode = %x", m_QueueingMode);
    if (m_QueueingMode == PORT_QUEUEING)
    {
        ndisStatus = m_TxEntryTable.Initialize(
            m_pAdapter,
            m_MaxNumPorts,
            m_QueueingMode);
    }
    else
    {
        ndisStatus = m_TxEntryTable.Initialize(
            m_pAdapter,
            pDatapathCapabilities->MaxNumPeers,
            m_QueueingMode);
    }

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[txmgr]Unable to initialize TxPeerEntryTable");
        goto exit;
    }

    if (m_QueueingMode == PEER_QUEUEING)
    {
        m_pACList = new CACList();
        if (m_pACList == NULL)
        {
            WFCError("[txmgr]Unable to allocate CACList");
            ndisStatus = NDIS_STATUS_RESOURCES;
            goto exit;
        }
    }
    //Set backfill size correctly.
    ndisStatus = m_pAdapter->GetAdapterPropertyCache()->GetPropertyULong(
        WfcAdapterPropertyBackFillSize,
        &m_BackFillSize);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[txmgr]Could not retreive BackFillSize from property cache.");
        NT_ASSERT(FALSE);
        goto exit;
    }

    ndisStatus = CNdisSpinLock::CreateInstance(&m_TxMgrLock);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[txmgr]Unable to initialize TxMgrLock");
        goto exit;
    }

    ndisStatus = CNdisSpinLock::CreateInstance(&m_TxTargetDescLock);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[txmgr]Unable to initialize TxTargetDescLock");
        goto exit;
    }

    ndisStatus = CNdisSpinLock::CreateInstance(&m_TxTargetDescDeinitLock);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[txmgr]Unable to initialize TxTargetDescDeinitLock");
        goto exit;
    }

    ndisStatus = CNdisSpinLock::CreateInstance(&m_TxPrequeueLock);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[txmgr]Unable to initialize TxPrequeueLock");
        goto exit;
    }

    m_hSendCompleteWorkItem = NdisAllocateIoWorkItem(m_pAdapter->GetMiniportAdapterHandle());
    if (m_hSendCompleteWorkItem == NULL)
    {
        WFCError("[txmgr]Unable to allocate work item for send complete work.");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    //Initialize is called by the event queue so it is safe to register here.
    ndisStatus = m_pAdapter->GetEventQueue()->RegisterTimeoutCallbackWithLevelAndReuse(
        IActivityId::get_NextActivityId(),
        this,
        WFC_DATAPATH_CONSTANT::OldFrameDetectionPeriodMs,
        NULL,
        WdfExecutionLevelPassive,
        true,
        &m_pOldFrameDetectionTimerRegistrationContext,
        &m_OldFrameDetectionTimerToken);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[txmgr]Unable to register for old frame detection timer callback (0x%x)", ndisStatus);
        goto exit;
    }

    //remaining initialization
exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTxMgr::InitializeTxWfcFrame(
    _In_ PNET_BUFFER_LIST pNbl,
    _In_ WDI_PORT_ID PortId,
    _In_ BOOLEAN IsKeyConfigured)
    /*++
        Allocates WfcFrame and initializes, if successful then pNbl->MiniportReserved[0]
        will point to WDI_FRAME_METADATA otherwise it will be NULL
        --*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_EXTSTA_SEND_CONTEXT pDot11ExtStaSendContext = NULL;
    PWFC_FRAME pWfcFrame = NULL;

    PNET_BUFFER pNb = NULL;
    PMDL pMdlCurrent;
    ULONG currentDataOffset;

    PNDIS_NET_BUFFER_LIST_8021Q_INFO pIeee8021Q;

    PDOT11_MAC_HEADER pDot11MacHeader;
    BYTE *mdlBuffer;
    ULONG mdlBytesAvailable;

    IEEE_8022_LLC_SNAP LlcSnap;

    WDI_PEER_ID PeerId = WDI_PEER_ANY;

    ULONG uSize = 0;

    TraceEntry();

    pNb = NET_BUFFER_LIST_FIRST_NB(pNbl);

    pMdlCurrent = NET_BUFFER_CURRENT_MDL(pNb);

    currentDataOffset = NET_BUFFER_CURRENT_MDL_OFFSET(pNb);

    //AllocateWfcFrame zeros the frame
    pWfcFrame = m_pAdapter->AllocateWfcFrame();
    if (pWfcFrame == NULL)
    {
        WFCError("[txmgr]Unable to allocate WfcFrame");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    pWfcFrame->FrameType = TX_NDIS_FRAME;

    //get 802.11 header
    // the assumption is that nwifi will provide continuous buffer
    NdisQueryMdl(pMdlCurrent, &mdlBuffer, &mdlBytesAvailable, NormalPagePriority);
    if ((mdlBytesAvailable - currentDataOffset) < sizeof(DOT11_MAC_HEADER))
    {
        WFCError("[txmgr]Mdl not large enough to include DOT11_MAC_HEADER");
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        goto exit;
    }
    pDot11MacHeader = (PDOT11_MAC_HEADER)(mdlBuffer + currentDataOffset);

    //only do peer classification if operating in PeerQueueing mode
    if (m_QueueingMode == PEER_QUEUEING)
    {
        PeerId = m_pAdapter->m_PeerTable.GetPeerId((PWDI_MAC_ADDRESS)&pDot11MacHeader->Address1, PortId);
        if (PeerId == WDI_PEER_ANY)
        {
            WFCError("[txmgr]Unable to classify NBL %!MACADDR!",
                pDot11MacHeader->Address1);
            ndisStatus = NDIS_STATUS_INVALID_ADDRESS;
            goto exit;
        }
    }
    pWfcFrame->WiFiFrameMetadata.u.txMetaData.PeerID = PeerId;

    pIeee8021Q = (PNDIS_NET_BUFFER_LIST_8021Q_INFO)&NET_BUFFER_LIST_INFO(pNbl, Ieee8021QNetBufferListInfo);

    pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExTID = pIeee8021Q->WLanTagHeader.WMMInfo;

    InitializeListHead(&pWfcFrame->WiFiFrameMetadata.Linkage);

    pWfcFrame->WiFiFrameMetadata.pNBL = pNbl;
    pWfcFrame->WiFiFrameMetadata.u.txMetaData.PortID = PortId;

    if (pDot11MacHeader->FrameControl.FromDS && pDot11MacHeader->FrameControl.ToDS)
    {
        uSize = DOT11_DATA_LONG_HEADER_SIZE;
    }
    else
    {
        uSize = DOT11_DATA_SHORT_HEADER_SIZE;
    }

    if (pDot11MacHeader->FrameControl.Subtype & DOT11_DATA_SUBTYPE_QOS_FLAG)
    {
        uSize += DOT11_QOS_CONTROL_HEADER_SIZE;
    }

    pWfcFrame->WiFiFrameMetadata.u.txMetaData.IsUnicast = !ETH_IS_MULTICAST(pDot11MacHeader->Address1);

    if (NET_BUFFER_DATA_LENGTH(pNb) < (uSize + IEEE_8022_LLC_SNAP_SIZE))
    {
        WFCError("[txmgr]NET_BUFFER not large enough to contain LLC/SNAP");
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        goto exit;
    }

    NdisAdvanceNetBufferListDataStart(pNbl,
                                      uSize,
                                      FALSE,
                                      NULL);


    //check LLC SNAP
    PIEEE_8022_LLC_SNAP pLlcSnap = 
        reinterpret_cast<PIEEE_8022_LLC_SNAP>(NdisGetDataBuffer(pNbl->FirstNetBuffer, IEEE_8022_LLC_SNAP_SIZE, &LlcSnap, 1, 0));

    //NdisGetDataBuffer can still return null so in that case reject the packet
    if (pLlcSnap == NULL)
    {
        WFCError("[txmgr]Unable to obtain LLC/SNAP");

        //retreat is undoing advance above so the NBL already has the space needed.
        ndisStatus = NdisRetreatNetBufferListDataStart(pNbl, uSize, 0, NULL, NULL);
        NT_ASSERT(ndisStatus == NDIS_STATUS_SUCCESS);

        ndisStatus = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    //fill in Ethertype
    pWfcFrame->WiFiFrameMetadata.u.txMetaData.Ethertype = pLlcSnap->sh_etype;

    //retreat is undoing advance above so the NBL already has the space needed.
    ndisStatus = NdisRetreatNetBufferListDataStart(pNbl, uSize, 0, NULL, NULL);
    NT_ASSERT(ndisStatus == NDIS_STATUS_SUCCESS);

    pDot11ExtStaSendContext = (PDOT11_EXTSTA_SEND_CONTEXT)NET_BUFFER_LIST_INFO(pNbl, MediaSpecificInformation);

    pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExemptionAction =
        CDot11ToWabiConverter::MapExemptionActionType(pDot11ExtStaSendContext->usExemptionActionType);

    // if the m_KeyConfigured flag is false (by default), overwrite WDI_EXEMPT_ON_KEY_MAPPING_KEY_UNAVAILABLE
    // with WDI_EXEMPT_ALWAYS
    if ((pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExemptionAction == WDI_EXEMPT_ON_KEY_MAPPING_KEY_UNAVAILABLE) &&
        (IsKeyConfigured == FALSE)                                                                                   )
    {
        pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExemptionAction = WDI_EXEMPT_ALWAYS;
    }

    pNbl->MiniportReserved[0] = WDI_FRAME_FROM_WFC_FRAME(pWfcFrame);
    ndisStatus = m_pAdapter->m_FrameIdTable.AssignAndMapFrameId(pWfcFrame);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[txmgr]Unable to assign frameId (0x%x)", ndisStatus);
        goto exit;
    }
    WFCTrace("[txmgr]%p - [Port:0x%x][Peer:0x%x][Tid:0x%x][Ethertype:0x%x][Exemption:%d][FrameId:0x%x]",
        pNbl,
        pWfcFrame->WiFiFrameMetadata.u.txMetaData.PortID,
        pWfcFrame->WiFiFrameMetadata.u.txMetaData.PeerID,
        pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExTID,
        pWfcFrame->WiFiFrameMetadata.u.txMetaData.Ethertype,
        pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExemptionAction,
        pWfcFrame->WiFiFrameMetadata.FrameID);

exit:
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        //free frame and set MiniportReserved[0] to null
        if (pWfcFrame)
        {
            WFCError("[txmgr]Freeing WfcFrame");
            m_pAdapter->FreeWfcFrame(pWfcFrame);
        }
        pNbl->MiniportReserved[0] = NULL;
    }

    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTxMgr::RestartSend(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    PWFC_TX_ENTRY pTxEntry = NULL;

    TraceEntry();

    WFCInfo("[txmgr]RestartSend [PortId:%d][PeerId:%d]",
        PortId,
        PeerId);

    //PortId must be valid,  PeerId may be wild.
    if (PortId == WDI_PORT_ANY)
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("[txmgr]Wildcard PortId not valid");
        goto exit;
    }
    //if PortQueueing then PeerId must be wild
    if (m_QueueingMode == PORT_QUEUEING &&
        PeerId != WDI_PEER_ANY            )
    {
        WFCInfo("[txmgr]PortQueueing enabled. Ignoring peer Tx restart");
        goto exit;
    }

    {
        CAutoLock lock(m_TxMgrLock);

        if(PeerId != WDI_PEER_ANY)
        {
            pTxEntry = m_TxEntryTable.GetTxEntry(PeerId);
            if (pTxEntry == NULL)
            {
                WFCError("[txmgr]No Peer TxEntry found for [Port:%x][Peer:%x]",
                    PortId,
                    PeerId);
                ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
                goto exit;
            }
            ndisStatus = RestartSendTxEntry(pTxEntry);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCError("[txmgr]Error restarting [TxId:%x]", pTxEntry->TxId);
                goto exit;
            }
        }
        else
        {
            if (m_QueueingMode == PORT_QUEUEING)
            {
                pTxEntry = m_TxEntryTable.GetTxEntry(PortId);
                if (pTxEntry == NULL)
                {
                    WFCError("[txmgr]No TxEntry found for [Port:%x]",
                        PortId);
                    ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
                    goto exit;
                }
                ndisStatus = RestartSendTxEntry(pTxEntry);
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    WFCError("[txmgr]Error restarting [TxId:%x]", pTxEntry->PeerId);
                    goto exit;
                }
            }
            else
            {
                UINT Index = 0;
                pTxEntry = m_TxEntryTable.GetNextPortPeerEntry(PortId, &Index);
                while (pTxEntry != NULL)
                {
                    ndisStatus = RestartSendTxEntry(pTxEntry);
                    if (ndisStatus != NDIS_STATUS_SUCCESS)
                    {
                        WFCError("[txmgr]Error restarting [TxId:%x]", pTxEntry->TxId);
                        //TODO continue?
                    }

                    pTxEntry = m_TxEntryTable.GetNextPortPeerEntry(PortId, &Index);
                }
            }

        }
    }

    //take oppportunity to service queues
    ServiceQueues();

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTxMgr::RestartSendTxEntry(
        _In_ PWFC_TX_ENTRY pTxEntry)
/*++
    called with txmgr lock held
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    PWFC_TX_QUEUE pTxQueue = NULL;

    int i;

    WFCInfo("[txmgr]RestartSendTxEntry [TxId:%x] is in state %x",
        pTxEntry->TxId,
        pTxEntry->TxEntryState);

    //The entry typically will be stopped however due to IHV frames there is a possibility
    // that it will be stopping.  If it is stopping then the outstanding frames are not NDIS frames
    // since the port would not be restarting unless there were no outstanding port frames.
    // in this case it is okay to restart (stopping -> started) unless TX_ENTRY_PEER_DELETE_PENDING is set
    if ((TEST_FLAG(pTxEntry->TxEntryState,TX_ENTRY_PEER_DELETE_PENDING)) ||
        (!TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPING) &&
         !TEST_FLAG(pTxEntry->TxEntryState,TX_ENTRY_STOPPED)           )    )
    {
        WFCError("[txmgr][TxId:%x] in invalid state %x",
            pTxEntry->TxId,
            pTxEntry->TxEntryState);
        ndisStatus = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    for (i = 0; IsValidId((WFC_TX_QUEUE_ID)i); i++)
    {
        pTxQueue = GetTxQueue(pTxEntry, (WFC_TX_QUEUE_ID)i);

        if (pTxQueue != NULL)
        {
            CLEAR_FLAG(pTxQueue->TxQueueState, TX_QUEUE_STOPPING);
            CLEAR_FLAG(pTxQueue->TxQueueState, TX_QUEUE_STOPPED);

            if (pTxQueue->TxQueueState == TX_QUEUE_STARTED &&
                pTxQueue->BacklogCount > 0)
            {
                //Add to appropriate scheduling structures
                if (m_CGlobalTxQueueList.AddTxQueue(pTxQueue) == NDIS_STATUS_SUCCESS)
                {
                    m_CurActiveBacklog += pTxQueue->BacklogCount;
                }
                //no need to check return code as the return code checked above is sufficient to update the
                // backlog count.
                AddTxQueueToAcEntry(pTxQueue);

            }
        }
    }

    CLEAR_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPING);
    CLEAR_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPED);

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}


NDIS_STATUS CTxMgr::TxAbortRequest(
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId)
/*++
    allocates a TxAbortRequest and inserts into tail of list.
    if the list was previously empty then calls tx abort.

    returns NDIS_STATUS_SUCCESS if TxAbort succeeded.
            NDIS_STATUS_PENDING if TxAbort either returned pending or 
                there is another abort request in progress
            or an error if the allocation failed
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_TX_ABORT_REQUEST pTxAbortRequest = NULL;
    BOOLEAN bIsListEmpty = FALSE;

    TraceEntry();

    WFCInfo("[txmgr]TxAbortRequest [PortId:0x%x][PeerId:0x%x]",
        PortId,
        PeerId);

    pTxAbortRequest = new WFC_TX_ABORT_REQUEST;
    if (pTxAbortRequest == NULL)
    {
        WFCError("[txmgr]Unable to allocate TxAbortRequest");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    InitializeListHead(&pTxAbortRequest->AbortListEntry);
    pTxAbortRequest->PortId = PortId;
    pTxAbortRequest->PeerId = PeerId;

    { // m_TxMgrLock
        CAutoLock lock(m_TxMgrLock);

        bIsListEmpty = IsListEmpty(&m_TxAbortRequestQueue);

        InsertTailList(&m_TxAbortRequestQueue, &pTxAbortRequest->AbortListEntry);

    } // m_TxMgrLock

    if (bIsListEmpty)
    {
        m_pAdapter->m_MiniportDataHandlers.TxAbortHandler(m_pAdapter->GetMiniportTalTxRxContext(),
            PortId,
            PeerId,
            &ndisStatus);
        WFCInfo("[txmgr]Abort returned status (0x%x)", ndisStatus);
        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            //don't notify this port/peer since this completed synchronously (other requests should be notified)
            //calling TxAbortConfirm to remove the request and process other requests
            TxAbortConfirm(FALSE);
        }
        else
        {
            ndisStatus = NDIS_STATUS_PENDING;
        }
    }
    else
    {
        WFCInfo("[txmgr]Another abort request already in queue.  Added abort request to tail");
        ndisStatus = NDIS_STATUS_PENDING;
    }
exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

void CTxMgr::TxAbortConfirm(
    _In_ BOOLEAN NotifyInitialConfirmation)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_TX_ABORT_REQUEST pTxAbortRequest = NULL;
    PLIST_ENTRY pListEntry = NULL;

    WDI_PORT_ID lastPortId = WDI_PORT_ANY;
    WDI_PEER_ID lastPeerId = WDI_PEER_ANY;

    WDI_PORT_ID nextPortId = WDI_PORT_ANY;
    WDI_PEER_ID nextPeerId = WDI_PEER_ANY;

    BOOLEAN bIsListEmpty = FALSE;
    BOOLEAN bIsAbortReqNeeded = FALSE;
    BOOLEAN bCurNotifyConfirmation = NotifyInitialConfirmation;
    CPort* pPort = NULL;


    TraceEntry();

    do
    {
        bIsListEmpty = TRUE;
        bIsAbortReqNeeded = FALSE;
        { //m_TxMgrLock
            CAutoLock lock(m_TxMgrLock);

            //check for invalid abort confirm
            if (IsListEmpty(&m_TxAbortRequestQueue))
            {
                WFCError("[txmgr]Invalid TxAbortConfirm.  No request in queue");
                goto exit;
            }

            //remove the request being confirmed
            pListEntry = RemoveHeadList(&m_TxAbortRequestQueue);
            pTxAbortRequest = (PWFC_TX_ABORT_REQUEST)CONTAINING_RECORD(pListEntry, WFC_TX_ABORT_REQUEST, AbortListEntry);

            lastPortId = pTxAbortRequest->PortId;
            lastPeerId = pTxAbortRequest->PeerId;

            WFCInfo("[txmgr]Removed abort request [PortId:0x%x][PeerId:0x%x]",
                lastPortId,
                lastPeerId);

            delete pTxAbortRequest;
            pTxAbortRequest = NULL;

            //if there are remaining requests in list process first one
            if (!IsListEmpty(&m_TxAbortRequestQueue))
            {
                bIsAbortReqNeeded = TRUE;
                bIsListEmpty = FALSE;
                WFCInfo("[txmgr]There are remaining requests in queue.");

                pListEntry = m_TxAbortRequestQueue.Flink;
                pTxAbortRequest = (PWFC_TX_ABORT_REQUEST)CONTAINING_RECORD(pListEntry, WFC_TX_ABORT_REQUEST, AbortListEntry);

                nextPortId = pTxAbortRequest->PortId;
                nextPeerId = pTxAbortRequest->PeerId;

                WFCInfo("[txmgr] Next abort request [PortId:0x%x][PeerId:0x%x]",
                    nextPortId,
                    nextPeerId);
            }
        } //m_TxMgrLock
        if (bCurNotifyConfirmation)
        {
            WFCInfo("[txmgr]Notifying Abort confirm for [PortId:0x%x][PeerId:0x%x]",
                lastPortId,
                lastPeerId);
            if (lastPeerId == WDI_PEER_ANY)
            {
                //abort was for port
                pPort = m_pAdapter->GetPortFromPortId(lastPortId);
                if (pPort != NULL)
                {
                    pPort->OnTxAbortConfirm();
                }
                else
                {
                    WFCError("[txmgr]Invalid TxAbortConfirm.  [PortId:0x%x] not found",
                        lastPortId);
                    ndisStatus = NDIS_STATUS_INVALID_DEVICE_REQUEST;
                }
            }
            else
            {
                PeerAbortConfirm(lastPortId, lastPeerId);
            }
        }
        if (bIsAbortReqNeeded)
        {
            //NotifyInitialConfirmation only applies to the first confirm, if there are other requests in the queue
            // then those should be notified as TxAbortRequest would have returned pending for those requests.
            bCurNotifyConfirmation = TRUE;
            m_pAdapter->m_MiniportDataHandlers.TxAbortHandler(m_pAdapter->GetMiniportTalTxRxContext(),
                nextPortId,
                nextPeerId,
                &ndisStatus);
            WFCInfo("[txmgr]Abort returned status (0x%x)", ndisStatus);
            //if return is success then the loop will take care of the notification 
            // and processing next pending request (if any)
        }
    } while (!bIsListEmpty && ndisStatus == NDIS_STATUS_SUCCESS);

exit:
    TraceExit(ndisStatus);
}

NDIS_STATUS CTxMgr::PeerAbortConfirm(
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId)
/*++
    Clears abort confirm pending and calls PeerDeleteConfirm if Peer delete pending
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_TX_ENTRY pTxEntry = NULL;
    BOOLEAN bPeerDeleteConfirm = FALSE;

    TraceEntry();

    WFCInfo("[txmgr][PortId:%x][PeerId:%x]",
        PortId,
        PeerId);

    //this should never even be called for PortQueuing mode
    if (m_QueueingMode == PORT_QUEUEING)
    {
        WFCError("[txmgr]Not operating in PeerQueueing mode. Ignoring PeerAbortConfirm");
        goto exit;
    }

    { // m_TxMgrLock
        CAutoLock lock(m_TxMgrLock);

        pTxEntry = m_TxEntryTable.GetTxEntry(PeerId);
        if (pTxEntry == NULL)
        {
            ndisStatus = NDIS_STATUS_FAILURE;
            WFCError("[txmgr]TxPeerEntry not found");
            goto exit;
        }

        WFCInfo("[txmgr]TxEntry [TxId:0x%x][PortId:0x%x][PeerId:0x%x][State:0x%x]",
            pTxEntry->TxId,
            pTxEntry->PortId,
            pTxEntry->PeerId,
            pTxEntry->TxEntryState);

        CLEAR_FLAG(pTxEntry->TxEntryState, TX_ENTRY_PEER_ABORT_CONFIRM_PENDING);

        if (TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_PEER_DELETE_PENDING))
        {
            bPeerDeleteConfirm = TRUE;
        }

    } // m_TxMgrLock

    if (bPeerDeleteConfirm)
    {
        WFCInfo("[txmgr]Calling PeerDeleteConfirm");
        PeerDeleteConfirm(PortId, PeerId);
    }

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTxMgr::StopCancelSend(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId,
        _In_ WFC_TX_STOP_REASON TxStopReason)
/*++
    TxMgr will mark the TxQueues to the stopping state and dequeue all NBLs in the backlogQueue and 
     chain the NBLs and call Adapter::SendComplete().  A TxEntry will go to the stopped state when there
     are no packets in the backlog queues and the transfer/tx pending counts are both zero.
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    PNET_BUFFER_LIST pNdisCompleteNbl = NULL;
    PNET_BUFFER_LIST pIhvCompleteNbl = NULL;

    PWFC_TX_ENTRY pTxEntry = NULL;

    TraceEntry();

    WFCInfo("[txmgr]StopCancelSend [PortId:%x][PeerId:%x][TxStopReason:%d]",
        PortId,
        PeerId,
        TxStopReason);

    //PortId must be valid,  PeerId may be wild.
    if (PortId == WDI_PORT_ANY)
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("[txmgr]Wildcard PortId not valid");
        goto exit;
    }

    //if PortQueueing then PeerId must be wild
    if (m_QueueingMode == PORT_QUEUEING &&
        PeerId != WDI_PEER_ANY            )
    {
        WFCInfo("[txmgr]PortQueueing enabled.  Ignoring peer Tx stop");
        goto exit;
    }

    { // m_TxMgrLock
        CAutoLock lock(m_TxMgrLock);

        if(PeerId != WDI_PEER_ANY)
        {
            pTxEntry = m_TxEntryTable.GetTxEntry(PeerId);
            if (pTxEntry == NULL)
            {
                WFCError("[txmgr]No TxEntry found for [Port:%x][Peer:%x]",
                    PortId,
                    PeerId);
                ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
                goto exit;
            }
            ndisStatus = StopCancelSendTxEntry(pTxEntry,
                                               TxStopReason,
                                               &pNdisCompleteNbl,
                                               &pIhvCompleteNbl);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCError("[txmgr]Error stopping [TxId:%x]", pTxEntry->TxId);
                goto exit;
            }
        }
        else
        {
            if (m_QueueingMode == PORT_QUEUEING)
            {
                pTxEntry = m_TxEntryTable.GetTxEntry(PortId);
                if (pTxEntry == NULL)
                {
                    WFCError("[txmgr]No TxEntry found for [Port:%x][Peer:%x]",
                        PortId,
                        PeerId);
                    ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
                    goto exit;
                }
                ndisStatus = StopCancelSendTxEntry(pTxEntry,
                                                   TxStopReason,
                                                   &pNdisCompleteNbl,
                                                   &pIhvCompleteNbl);
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    WFCError("[txmgr]Error stopping [TxId:%x]", pTxEntry->TxId);
                    goto exit;
                }
            }
            else
            {
                UINT Index = 0;

                pTxEntry = m_TxEntryTable.GetNextPortPeerEntry(PortId, &Index);
                while (pTxEntry != NULL)
                {
                    ndisStatus = StopCancelSendTxEntry(pTxEntry,
                                                       TxStopReason,
                                                       &pNdisCompleteNbl,
                                                       &pIhvCompleteNbl);
                    if (ndisStatus != NDIS_STATUS_SUCCESS)
                    {
                        WFCError("[txmgr]Error stopping [TxId:%x]", pTxEntry->TxId);
                        //TODO continue?
                    }

                    pTxEntry = m_TxEntryTable.GetNextPortPeerEntry(PortId, &Index);
                }
            }
        }
    } // m_TxMgrLock

    //complete the canceled NBLs
    if (pIhvCompleteNbl)
    {
        CompleteIhvNbl(pIhvCompleteNbl);
    }
    if (pNdisCompleteNbl)
    {
        //we want to defer completion of these NBLs when we are at dispatch
        // in case there are many queued and because there is little impact to also
        // defering if we are at passive we currently defer in that case as well to 
        // have a more simple flow
        CompleteNdisNbl(pNdisCompleteNbl, TRUE);
    }
    //In both the case of a Control path stop or PeerDelete stop issue abort
    if (TxStopReason == TxStopReasonControlPath || 
        TxStopReason == TxStopReasonPeerDelete     )
    {
        ndisStatus = TxAbortRequest(PortId, PeerId);
        WFCInfo("[txmgr]Abort request returned status (0x%x).", ndisStatus);

        if (ndisStatus == NDIS_STATUS_PENDING)
        {
            WFCInfo("[txmgr]Waiting for TxAbortConfirm");
        }
        else
        {
            //if there was an error sending the txabort continue as if it were successful.
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
    }

exit:

    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTxMgr::StopCancelSendTxEntry(
        _In_ PWFC_TX_ENTRY pTxEntry,
        _In_ WFC_TX_STOP_REASON TxStopReason,
        _Inout_ PNET_BUFFER_LIST *ppNdisCompleteNbl,
        _Inout_ PNET_BUFFER_LIST *ppIhvCompleteNbl)
/*++
    called with txmgr lock held
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_TX_QUEUE pTxQueue = NULL;
    PLIST_ENTRY pListEntry = NULL;
    PWDI_FRAME_METADATA pCurFrameMeta = NULL;
    PWFC_FRAME pWfcFrame = NULL;
    PNET_BUFFER_LIST pCurNbl = NULL;

    BOOLEAN bInjected = FALSE;

    int i;

    TraceEntry();

    //Clear stopped and set stopping
    CLEAR_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPED);
    SET_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPING);

    if (TxStopReason == TxStopReasonPeerDelete)
    {
        SET_FLAG(pTxEntry->TxEntryState, TX_ENTRY_PEER_DELETE_PENDING);
    }

    for (i = 0; IsValidId((WFC_TX_QUEUE_ID)i); i++)
    {
        //PeerDelete will cause IHV TIDs to be stopped and backlog flushed.
        //other stop reasons will skip IHV TIDs
        if (IsIhvId((WFC_TX_QUEUE_ID)i)            &&
            TxStopReason != TxStopReasonPeerDelete    )
        {
            continue;
        }

        pTxQueue = GetTxQueue(pTxEntry, (WFC_TX_QUEUE_ID)i);
        
        if (pTxQueue != NULL)
        {
            CLEAR_FLAG(pTxQueue->TxQueueState, TX_QUEUE_STOPPED);
            SET_FLAG(pTxQueue->TxQueueState, TX_QUEUE_STOPPING);

            if (m_CGlobalTxQueueList.RemoveTxQueue(pTxQueue) == NDIS_STATUS_SUCCESS)
            {
                //subtract queue backlog from total active backlog if the queue was schedulable
                m_CurActiveBacklog -= pTxQueue->BacklogCount;
            }
            RemoveTxQueueFromAcEntry(pTxQueue);

            while (!IsListEmpty(&pTxQueue->BacklogQueue))
            {
                pListEntry = RemoveHeadList(&pTxQueue->BacklogQueue);

                pCurFrameMeta = (PWDI_FRAME_METADATA)CONTAINING_RECORD(pListEntry, WDI_FRAME_METADATA, Linkage);
                pCurNbl = pCurFrameMeta->pNBL;
                pWfcFrame = (PWFC_FRAME)WFC_FRAME_FROM_WDI_FRAME(pCurFrameMeta);

                bInjected = (pWfcFrame->FrameType == TX_INJECTED_FRAME) ? TRUE : FALSE;

                pTxQueue->BacklogCount--;

                pTxEntry->BacklogCount--;

                if (bInjected == TRUE)
                {
                    NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = *ppIhvCompleteNbl;
                    *ppIhvCompleteNbl = pCurNbl;
                }
                else
                {
                    NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = *ppNdisCompleteNbl;
                    *ppNdisCompleteNbl = pCurNbl;
                }
            }

            if (pTxQueue->TransferPendingCount == 0 && 
                pTxQueue->TxPendingCount == 0          )
            {
                CLEAR_FLAG(pTxQueue->TxQueueState, TX_QUEUE_STOPPING);
                SET_FLAG(pTxQueue->TxQueueState, TX_QUEUE_STOPPED);
            }
        }
    }

    if (pTxEntry->TransferPendingCount == 0 &&
        pTxEntry->TxPendingCount == 0          )
    {
        CLEAR_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPING);
        SET_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPED);
    }

    WFCTrace("[txmgr]StopCancelTxEntry [TxId:%x][State:%x]",
        pTxEntry->TxId,
        pTxEntry->TxEntryState);

    TraceExit(ndisStatus);
    return ndisStatus;
}

void CTxMgr::SetMaxOutstandingTransfers(
        _In_ UINT16 MaxOutstandingTransfers)
/*++
--*/
{
    m_MaxOutstandingTransfers = MaxOutstandingTransfers;
    WFCInfo("[txmgr]MaxOutstandingTransfers = %d", m_MaxOutstandingTransfers);
}

NDIS_STATUS CTxMgr::AddPeerId(
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ PWDI_MAC_ADDRESS pPeerAddr)
/*++
    Called by Adapter as part of PEER_CREATE_IND.
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    WFCInfo("[txmgr][Port:%x][Peer:%x][Addr:%!MACADDR!]",
            PortId,PeerId,pPeerAddr->Address);

    if (m_QueueingMode == PORT_QUEUEING)
    {
        WFCInfo("[txmgr]Not operating in PeerQueueing mode. Ignoring AddPeerId");
        goto exit;
    }

    {
        CAutoLock lock(m_TxMgrLock);

        ndisStatus = m_TxEntryTable.InitializeTxEntry(PeerId,
                                                      PortId,
                                                      PeerId,
                                                      pPeerAddr);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("[txmgr]Unable to initialize TxEntry (0x%x)",ndisStatus);
            goto exit;
        }
        //TODO OTHER OPERATIONS MAY OCCUR (such as trying to preallocate/assign PeerTIDQueues).
    }
exit: 
    TraceExit(ndisStatus);
    return ndisStatus;
}

//Todo cleanup
NDIS_STATUS CTxMgr::PeerDeleteBegin(
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId)
/*++
    Called by Adapter as part of PEER_DELETE_IND.
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_TX_ENTRY pTxEntry = NULL;
    int i;

    PNET_BUFFER_LIST pIhvCompleteNbl = NULL;
    PNET_BUFFER_LIST pNdisCompleteNbl = NULL;
    PLIST_ENTRY pListEntry = NULL;
    PWDI_FRAME_METADATA pCurFrameMeta = NULL;
    PWFC_FRAME pWfcFrame = NULL;
    PNET_BUFFER_LIST pCurNbl = NULL;

    TraceEntry();

    WFCInfo("[txmgr][Port:%x][Peer:%x]",
            PortId,PeerId);

    if (m_QueueingMode == PORT_QUEUEING)
    {
        WFCInfo("[txmgr]Not operating in PeerQueueing mode. Ignoring PeerDeleteBegin");
        goto exit;
    }

    ndisStatus = StopCancelSend(PortId, PeerId, TxStopReasonPeerDelete);

    { // m_TxMgrLock
        CAutoLock lock(m_TxMgrLock);

        pTxEntry = m_TxEntryTable.GetTxEntry(PeerId);
        if (pTxEntry == NULL)
        {
            ndisStatus = NDIS_STATUS_FAILURE;
            WFCError("[txmgr]TxPeerEntry not found");
            goto exit;
        }

        //ndisStatus came from stopcancelsend above.  success means TX_ABORT not pending
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            SET_FLAG(pTxEntry->TxEntryState, TX_ENTRY_PEER_ABORT_CONFIRM_PENDING);
        }

        //if the peer is stopped and abort has been confirmed
        if (TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPED)                     &&
            !TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_PEER_ABORT_CONFIRM_PENDING)    )
        {
            for (i = 0; IsValidId((WFC_TX_QUEUE_ID)i); i++)
            {
                PWFC_TX_QUEUE pTempTxQueue = GetTxQueue(pTxEntry, (WFC_TX_QUEUE_ID)i);
                if (pTempTxQueue)
                {
                    WFCInfo("[txmgr][Port:%x][Peer:%x][Tid:%u][BacklogCount:%u][TransferPendingCount:%u][TxPendingCount:%u]",
                        pTempTxQueue->PortId,
                        pTempTxQueue->PeerId,
                        pTempTxQueue->Tid,
                        pTempTxQueue->BacklogCount,
                        pTempTxQueue->TransferPendingCount,
                        pTempTxQueue->TxPendingCount);

                    if (m_CGlobalTxQueueList.RemoveTxQueue(pTempTxQueue) == NDIS_STATUS_SUCCESS)
                    {
                        //subtract queue backlog from total active backlog if the queue was schedulable
                        m_CurActiveBacklog -= pTempTxQueue->BacklogCount;
                    }
                    RemoveTxQueueFromAcEntry(pTempTxQueue);

                    //TODO if we are connected and disable adapter we can get in this state.
                    //there shouldnt be remaining NBLs at this point but clean them up anyways
                    //these should only be IHV NBLs
                    while (!IsListEmpty(&pTempTxQueue->BacklogQueue))
                    {
                        pListEntry = RemoveHeadList(&pTempTxQueue->BacklogQueue);

                        pCurFrameMeta = (PWDI_FRAME_METADATA)CONTAINING_RECORD(pListEntry, WDI_FRAME_METADATA, Linkage);
                        pCurNbl = pCurFrameMeta->pNBL;
                        pWfcFrame = (PWFC_FRAME)WFC_FRAME_FROM_WDI_FRAME(pCurFrameMeta);

                        pTempTxQueue->BacklogCount--;
                        pTxEntry->BacklogCount--;

                        ASSERT(pWfcFrame->FrameType == TX_INJECTED_FRAME);

                        if (pWfcFrame->FrameType == TX_INJECTED_FRAME)
                        {
                            NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = pIhvCompleteNbl;
                            pIhvCompleteNbl = pCurNbl;
                        }
                        else
                        {
                            NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = pNdisCompleteNbl;
                            pNdisCompleteNbl = pCurNbl;
                        }
                    }

                    delete pTempTxQueue;
                    pTempTxQueue = NULL;
                    (pTxEntry->ppTxQueue)[i] = NULL;
                }
            }
            ndisStatus = m_TxEntryTable.RemoveTxEntry(PeerId);
            //at this point the peer has been deleted so return success so IHV
            //peer delete completes synchronously

            //Todo other operations may be needed
        }
        else
        {
            ndisStatus = NDIS_STATUS_PENDING;
        }
    } // m_TxMgrLock
    if (pIhvCompleteNbl)
    {
        CompleteIhvNbl(pIhvCompleteNbl);
    }
    if (pNdisCompleteNbl)
    {
        //we want to defer completion of these NBLs when we are at dispatch
        // in case there are many queued and because there is little impact to also
        // defering if we are at passive we currently defer in that case as well to 
        // have a more simple flow
        CompleteNdisNbl(pNdisCompleteNbl, TRUE);
    }

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTxMgr::PeerDeleteConfirm(
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId)
/*++
    Called when a Peer is in a state to be deleted
    Acquires/releases txmgr lock
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_TX_ENTRY pTxEntry = NULL;
    int i;

    PNET_BUFFER_LIST pIhvCompleteNbl = NULL;
    PNET_BUFFER_LIST pNdisCompleteNbl = NULL;
    PLIST_ENTRY pListEntry = NULL;
    PWDI_FRAME_METADATA pCurFrameMeta = NULL;
    PWFC_FRAME pWfcFrame = NULL;
    PNET_BUFFER_LIST pCurNbl = NULL;

    BOOLEAN bCallDeletePeer = FALSE;

    TraceEntry();

    WFCInfo("[txmgr][Port:%x][Peer:%x]",
        PortId, PeerId);

    if (m_QueueingMode == PORT_QUEUEING)
    {
        WFCInfo("[txmgr]Not operating in PeerQueueing mode. Ignoring PeerDeleteConfirm");
        goto exit;
    }

    {
        CAutoLock lock(m_TxMgrLock);

        pTxEntry = m_TxEntryTable.GetTxEntry(PeerId);
        if (pTxEntry == NULL)
        {
            ndisStatus = NDIS_STATUS_FAILURE;
            WFCError("[txmgr]TxPeerEntry not found");
            goto exit;
        }

        //confirm that the peer is stopped 
        if (TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPED)                     &&
            TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_PEER_DELETE_PENDING)         &&
            !TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_PEER_ABORT_CONFIRM_PENDING)    )
        {
            bCallDeletePeer = TRUE;
            for (i = 0; IsValidId((WFC_TX_QUEUE_ID)i); i++)
            {
                PWFC_TX_QUEUE pTempTxQueue = GetTxQueue(pTxEntry, (WFC_TX_QUEUE_ID)i);
                if (pTempTxQueue)
                {
                    WFCInfo("[txmgr][Port:%x][Peer:%x][Tid:%u][BacklogCount:%u][TransferPendingCount:%u][TxPendingCount:%u]",
                        pTempTxQueue->PortId,
                        pTempTxQueue->PeerId,
                        pTempTxQueue->Tid,
                        pTempTxQueue->BacklogCount,
                        pTempTxQueue->TransferPendingCount,
                        pTempTxQueue->TxPendingCount);

                    if (m_CGlobalTxQueueList.RemoveTxQueue(pTempTxQueue) == NDIS_STATUS_SUCCESS)
                    {
                        //subtract queue backlog from total active backlog if the queue was schedulable
                        m_CurActiveBacklog -= pTempTxQueue->BacklogCount;
                    }
                    RemoveTxQueueFromAcEntry(pTempTxQueue);

                    //TODO if we are connected and disable adapter we can get in this state.
                    //there shouldnt be remaining NBLs at this point but clean them up anyways
                    //these should only be IHV NBLs
                    while (!IsListEmpty(&pTempTxQueue->BacklogQueue))
                    {
                        pListEntry = RemoveHeadList(&pTempTxQueue->BacklogQueue);

                        pCurFrameMeta = (PWDI_FRAME_METADATA)CONTAINING_RECORD(pListEntry, WDI_FRAME_METADATA, Linkage);
                        pCurNbl = pCurFrameMeta->pNBL;
                        pWfcFrame = (PWFC_FRAME)WFC_FRAME_FROM_WDI_FRAME(pCurFrameMeta);

                        pTempTxQueue->BacklogCount--;
                        pTxEntry->BacklogCount--;

                        ASSERT(pWfcFrame->FrameType == TX_INJECTED_FRAME);

                        if (pWfcFrame->FrameType == TX_INJECTED_FRAME)
                        {
                            NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = pIhvCompleteNbl;
                            pIhvCompleteNbl = pCurNbl;
                        }
                        else
                        {
                            NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = pNdisCompleteNbl;
                            pNdisCompleteNbl = pCurNbl;
                        }
                    }

                    delete pTempTxQueue;
                    pTempTxQueue = NULL;
                    (pTxEntry->ppTxQueue)[i] = NULL;
                }
            }
            ndisStatus = m_TxEntryTable.RemoveTxEntry(PeerId);
            //Todo other operations may be needed
        }
    }

    if (pIhvCompleteNbl)
    {
        CompleteIhvNbl(pIhvCompleteNbl);
    }
    if (pNdisCompleteNbl)
    {
        //we want to defer completion of these NBLs when we are at dispatch
        // in case there are many queued and because there is little impact to also
        // defering if we are at passive we currently defer in that case as well to 
        // have a more simple flow
        CompleteNdisNbl(pNdisCompleteNbl, TRUE);
    }
    if (bCallDeletePeer)
    {
        WFCInfo("[txmgr]Calling IHV PeerDeleteConfirm");
        m_pAdapter->m_MiniportDataHandlers.TalTxRxPeerDeleteConfirmHandler(m_pAdapter->GetMiniportTalTxRxContext(),
            PortId,
            PeerId);
    }

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTxMgr::AddPortId(
    _In_ WDI_PORT_ID PortId)
/*++
    Called when port is created.  Adds TxEntry if in port queueing mode
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WDI_MAC_ADDRESS groupAddr = { 0xff,0xff,0xff,0xff,0xff,0xff };
    TraceEntry();

    WFCInfo("[txmgr][Port:%x]", PortId);

    if (m_QueueingMode == PEER_QUEUEING)
    {
        WFCInfo("[txmgr]Not operating in PortQueueing mode. Ignoring AddPortId");
        goto exit;
    }
    {
        CAutoLock lock(m_TxMgrLock);

        ndisStatus = m_TxEntryTable.InitializeTxEntry(PortId,
                                                      PortId,
                                                      WDI_PEER_ANY,
                                                      &groupAddr);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("[txmgr]Unable to initialize TxEntry (0x%x)", ndisStatus);
            goto exit;
        }
    }
exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTxMgr::RemovePortId(
    _In_ WDI_PORT_ID PortId)
/*++
    Called when port is deleted.  Removes TxEntry if in port queueing mode
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_TX_ENTRY pTxEntry = NULL;
    int i;

    PNET_BUFFER_LIST pIhvCompleteNbl = NULL;
    PNET_BUFFER_LIST pNdisCompleteNbl = NULL;
    PLIST_ENTRY pListEntry = NULL;
    PWDI_FRAME_METADATA pCurFrameMeta = NULL;
    PWFC_FRAME pWfcFrame = NULL;
    PNET_BUFFER_LIST pCurNbl = NULL;

    TraceEntry();

    WFCInfo("[txmgr][Port:%x]", PortId);

    if (m_QueueingMode == PEER_QUEUEING)
    {
        WFCInfo("[txmgr]Not operating in PortQueueing mode. Ignoring RemovePortId");
        goto exit;
    }

    {
        CAutoLock lock(m_TxMgrLock);

        pTxEntry = m_TxEntryTable.GetTxEntry(PortId);
        if (pTxEntry == NULL)
        {
            ndisStatus = NDIS_STATUS_FAILURE;
            WFCError("[txmgr]TxEntry nto found");
            goto exit;
        }

        CLEAR_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPED);
        SET_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPING);

        for (i = 0; IsValidId((WFC_TX_QUEUE_ID)i); i++)
        {
            PWFC_TX_QUEUE pTempTxQueue = GetTxQueue(pTxEntry, (WFC_TX_QUEUE_ID)i);
            if (pTempTxQueue)
            {
                WFCInfo("[txmgr][Port:%x][Peer:%x][Tid:%u][BacklogCount:%u][TransferPendingCount:%u][TxPendingCount:%u]",
                    pTempTxQueue->PortId,
                    pTempTxQueue->PeerId,
                    pTempTxQueue->Tid,
                    pTempTxQueue->BacklogCount,
                    pTempTxQueue->TransferPendingCount,
                    pTempTxQueue->TxPendingCount);

                if (m_CGlobalTxQueueList.RemoveTxQueue(pTempTxQueue) == NDIS_STATUS_SUCCESS)
                {
                    //subtract queue backlog from total active backlog if the queue was schedulable
                    m_CurActiveBacklog -= pTempTxQueue->BacklogCount;
                }
                RemoveTxQueueFromAcEntry(pTempTxQueue);

                //TODO if we are connected and disable adapter we can get in this state.
                //there shouldnt be remaining NBLs at this point but clean them up anyways
                //these should only be IHV NBLs
                while (!IsListEmpty(&pTempTxQueue->BacklogQueue))
                {
                    pListEntry = RemoveHeadList(&pTempTxQueue->BacklogQueue);

                    pCurFrameMeta = (PWDI_FRAME_METADATA)CONTAINING_RECORD(pListEntry, WDI_FRAME_METADATA, Linkage);
                    pCurNbl = pCurFrameMeta->pNBL;
                    pWfcFrame = (PWFC_FRAME)WFC_FRAME_FROM_WDI_FRAME(pCurFrameMeta);

                    pTempTxQueue->BacklogCount--;
                    pTxEntry->BacklogCount--;

                    ASSERT(pWfcFrame->FrameType == TX_INJECTED_FRAME);

                    if (pWfcFrame->FrameType == TX_INJECTED_FRAME)
                    {
                        NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = pIhvCompleteNbl;
                        pIhvCompleteNbl = pCurNbl;
                    }
                    else
                    {
                        NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = pNdisCompleteNbl;
                        pNdisCompleteNbl = pCurNbl;
                    }
                }

                delete pTempTxQueue;
                pTempTxQueue = NULL;
                (pTxEntry->ppTxQueue)[i] = NULL;
            }
        }
        //Todo other operations may be needed
        ndisStatus = m_TxEntryTable.RemoveTxEntry(PortId);
    }
    if (pIhvCompleteNbl)
    {
        CompleteIhvNbl(pIhvCompleteNbl);
    }
    if (pNdisCompleteNbl)
    {
        //we want to defer completion of these NBLs when we are at dispatch
        // in case there are many queued and because there is little impact to also
        // defering if we are at passive we currently defer in that case as well to 
        // have a more simple flow
        CompleteNdisNbl(pNdisCompleteNbl, TRUE);
    }
exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

//TODO
void CTxMgr::TxInjectIhvFrame(
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ WDI_EXTENDED_TID ExTid,
    _In_ PNET_BUFFER_LIST pNBL,
    _In_ BOOLEAN bIsUnicast,
    _In_ BOOLEAN bUseLegacyRates,
    _In_ UINT16 Ethertype,
    _In_ WDI_EXEMPTION_ACTION_TYPE ExemptionAction)
/*++
    Handles the injection of frames from the TxEngine
    for each NBL
       -- CreateTxWifiFrameMetadata
       --Fill in PeerID,PortID,ExtendedTID and bUseLegacyRates
         and mark the packet source appropriately
       --Call TX_TARGET_DESC_INIT
       --ProcessNBL
--*/
{
    UNREFERENCED_PARAMETER(bUseLegacyRates);

    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    BOOLEAN bTargetDescResources = FALSE;

    PNET_BUFFER_LIST pFailedNBLs = NULL;
    PNET_BUFFER_LIST pCurNetBufferList = NULL;
    PNET_BUFFER_LIST pNextNetBufferList = NULL;

    PWFC_FRAME pWfcFrame = NULL;

    TraceEntry();
    //TODO verify that PeerId == ANY when PortQueueing
    if ((PortId == WDI_PORT_ANY)                                    ||
        (PeerId == WDI_PEER_ANY && m_QueueingMode == PEER_QUEUEING) ||
        (!IsValidTid(ExTid))                                            )
    {
        ndisStatus = NDIS_STATUS_FAILURE;
        WFCError("[txmgr]Invalid parameter(s), adding to list to return to IHV");
        pFailedNBLs = pNBL;
        goto exit;
    }

    if (m_QueueingMode == PORT_QUEUEING)
    {
        PeerId = WDI_PEER_ANY;
    }

    //process each NBL individually
    pCurNetBufferList = pNBL;

    while (pCurNetBufferList != NULL)
    {
        pNextNetBufferList = NET_BUFFER_LIST_NEXT_NBL(pCurNetBufferList);
        NET_BUFFER_LIST_NEXT_NBL(pCurNetBufferList) = NULL;

        do
        {
            //Call CreateTxWifiFrameMetadata
            pWfcFrame = m_pAdapter->AllocateWfcFrame();
            if (pWfcFrame == NULL)
            {
                WFCError("[txmgr]Unable to allocate WFC FRAME for NBL, adding to list to return to IHV");
                NET_BUFFER_LIST_NEXT_NBL(pCurNetBufferList) = pFailedNBLs;
                pFailedNBLs = pCurNetBufferList;
                break;
            }

            pCurNetBufferList->MiniportReserved[0] = WDI_FRAME_FROM_WFC_FRAME(pWfcFrame);
            //Fill in appropriate fields. mark frame as from the IHV
            ndisStatus = InitializeIhvWfcFrame(pWfcFrame,
                                  pCurNetBufferList,
                                  PortId,
                                  PeerId,
                                  ExTid,
                                  bIsUnicast,
                                  bUseLegacyRates,
                                  Ethertype,
                                  ExemptionAction);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCError("[txmgr]Unable to initialize WfcFrame (0x%x), adding to list to return to IHV",
                    ndisStatus);

                pCurNetBufferList->MiniportReserved[0] = NULL;

                m_pAdapter->FreeWfcFrame(pWfcFrame);
                pWfcFrame = NULL;
                
                NET_BUFFER_LIST_NEXT_NBL(pCurNetBufferList) = pFailedNBLs;
                pFailedNBLs = pCurNetBufferList;
                break;
            }

            //TODO should this be single or multiple?
            //For injected frames we do not plan on queueing if the TxTargetDescInit failed for reason resources.
            // The IHV can either use a separate pool or dynamically allocate (lookaside list or the like)
            ndisStatus = ProcessNBL(pCurNetBufferList, &bTargetDescResources);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCError("[txmgr]ProcessNbl failed (0x%x) for frameId:%x, adding to list to return to IHV",
                    ndisStatus,
                    pWfcFrame->WiFiFrameMetadata.FrameID);

                m_pAdapter->m_FrameIdTable.FreeAndUnmapFrameId(pWfcFrame);

                pCurNetBufferList->MiniportReserved[0] = NULL;
                m_pAdapter->FreeWfcFrame(pWfcFrame);
                pWfcFrame = NULL;

                NET_BUFFER_LIST_NEXT_NBL(pCurNetBufferList) = pFailedNBLs;
                pFailedNBLs = pCurNetBufferList;
                break;
            }

        } while (0,0);
        pCurNetBufferList = pNextNetBufferList;
    }

exit:
    ServiceQueues();

    if (pFailedNBLs != NULL)
    {
        m_pAdapter->m_MiniportDataHandlers.TxTalSendCompleteHandler(m_pAdapter->GetMiniportTalTxRxContext(),
                                                                    pFailedNBLs,
                                                                    WDI_TxFrameStatus_TransferFailed);
    }

    TraceExit(ndisStatus);
}

void CTxMgr::ServiceQueues()
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_TX_QUEUE pCurTxQueue = NULL;

    int i;

    UINT32 scheduledCurActiveBacklog;

    TraceEntry();

    if (InterlockedIncrement(&m_SendThreadActive) > 1)
    {
        //active send thread already running
        goto exit;
    }

    //TODO limit run time.
    //TODO update AC list tokens
    m_SchedulingRound++;

    for (i = 0; i < WFC_DATAPATH_CONSTANT::MaxNumSendsPerServicing; i++)
    {
        pCurTxQueue = NULL;
        //Scheduler
        {
            CAutoLock lock(m_TxMgrLock);

            if (m_pACList                                                                                                   &&
                ((m_ServiceCounter % WFC_DATAPATH_CONSTANT::IterationPeriod) < WFC_DATAPATH_CONSTANT::NumPriorityIterations)   )
            {
                pCurTxQueue = m_pACList->GetTxQueueFromMaxAC();
            }

            if (pCurTxQueue == NULL)
            {
                pCurTxQueue = m_CGlobalTxQueueList.GetFirstTxQueue();
            }
            else
            {
                //TxQueue was selected by priority scheduling
                //move to tail of global list so global scheduling
                //will operate over older queues first
                m_CGlobalTxQueueList.MoveTxQueueToTail(pCurTxQueue);
            }

            if (pCurTxQueue == NULL)
            {
                WFCTrace("[txmgr]No active queue found");
                goto exit;
            }
            NT_ASSERT(pCurTxQueue->TxQueueState == TX_QUEUE_STARTED);
            NT_ASSERT(pCurTxQueue->BacklogCount > 0);

            m_ScheduledPortId = pCurTxQueue->PortId;
            m_ScheduledPeerId = pCurTxQueue->PeerId;
            m_ScheduledExtTid = pCurTxQueue->Tid;
            m_ScheduledQueueLength = pCurTxQueue->BacklogCount;
            scheduledCurActiveBacklog = m_CurActiveBacklog;
            //TODO figure out way to easily set this
            m_ScheduledRobustnessFlag = IsIhvTid(m_ScheduledExtTid);

            m_ServiceCounter++;
        }

        WFCTrace("[txmgr]Scheduling queue [PortId:0x%x][PeerId:0x%x][TID:0x%x][QueueLength:%u][TotalActiveCount:%llu]",
            m_ScheduledPortId,
            m_ScheduledPeerId,
            m_ScheduledExtTid,
            m_ScheduledQueueLength,
            scheduledCurActiveBacklog);

        if (IsIhvTid(m_ScheduledExtTid))
        {
            m_pAdapter->m_MiniportDataHandlers.TxTalSendHandler(m_pAdapter->GetMiniportTalTxRxContext(),
                                                                m_ScheduledPortId,
                                                                (m_QueueingMode == PORT_QUEUEING) ? WDI_PEER_ANY : m_ScheduledPeerId,
                                                                (m_QueueingMode == PORT_QUEUEING) ? WDI_EXT_TID_UNKNOWN : m_ScheduledExtTid,
                                                                m_ScheduledQueueLength,
                                                                scheduledCurActiveBacklog,
                                                                m_ScheduledRobustnessFlag);
        }
        else
        {
            m_pAdapter->m_MiniportDataHandlers.TxDataSendHandler(m_pAdapter->GetMiniportTalTxRxContext(),
                                                                 m_ScheduledPortId,
                                                                 (m_QueueingMode == PORT_QUEUEING) ? WDI_PEER_ANY : m_ScheduledPeerId,
                                                                 (m_QueueingMode == PORT_QUEUEING) ? WDI_EXT_TID_UNKNOWN : m_ScheduledExtTid,
                                                                 m_ScheduledQueueLength,
                                                                 scheduledCurActiveBacklog,
                                                                 m_ScheduledRobustnessFlag);
        }
    }

exit:
    InterlockedDecrement(&m_SendThreadActive);

    //TODO schedule servicing (DPC or workitem)
    TraceExit(ndisStatus);
}

//TODO should this be a single NBL or a chain?
NDIS_STATUS CTxMgr::AddNBLToTxQueue(
    _In_ PNET_BUFFER_LIST pNBL,
    _Out_ PBOOLEAN pbPeerBacklogNotifNeeded)
/*++
    Adds a single NBL to TxQueue, called with TxMgrlock held
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_FRAME pWfcFrame = NULL;
    PWDI_FRAME_METADATA pWifiFrameMeta = NULL;
    PWFC_TX_QUEUE pTxQueue = NULL;
    PWFC_TX_ENTRY pTxEntry = NULL;
    UINT64 prevBacklogCount = 0;
    UINT64 prevPeerBacklogCount = 0;

    TraceEntry();

    if (pbPeerBacklogNotifNeeded == NULL)
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("[txmgr]pbPeerBacklogNotifNeeded is null");
        goto exit;
    }

    *pbPeerBacklogNotifNeeded = FALSE;

    pWifiFrameMeta = GetWiFiFrameMetadataFromNbl(pNBL);
    if (pWifiFrameMeta == NULL)
    {
        ndisStatus = NDIS_STATUS_FAILURE;
        WFCError("[txmgr]Nbl doesn't have WDI_FRAME_METADATA");
        goto exit;
    }

    pWfcFrame = WFC_FRAME_FROM_WDI_FRAME(pWifiFrameMeta);

    pTxEntry = m_TxEntryTable.GetTxEntry(GetTxId(pWfcFrame));

    if (pTxEntry == NULL)
    {
        ndisStatus = NDIS_STATUS_FAILURE;
        WFCError("[txmgr]No Tx entry for [TxId:%x][Port:%x][Peer:%x]",
            GetTxId(pWfcFrame),
            pWfcFrame->WiFiFrameMetadata.u.txMetaData.PortID,
            pWfcFrame->WiFiFrameMetadata.u.txMetaData.PeerID);

        goto exit;
    }

    //if NDIS frame and TxEntry is stopping or stopped drop the frame
    //(we're either deleting the port, peer, or stopping because of request from port so we don't want the frame queued)
    if ((TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPED) || TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPING)) &&
        !IsIhvTid(pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExTID)                                                       )
    {
        ndisStatus = NDIS_STATUS_FAILURE;
        WFCInfo("[txmgr]Tx entry [TxId:%x][Port:%x][Peer:%x] in state %x.  Failing NBL.",
            pTxEntry->TxId,
            pTxEntry->PortId,
            pTxEntry->PeerId,
            pTxEntry->TxEntryState);
        goto exit;
    }

    //if there is a delete pending then fail the NBL
    if (TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_PEER_DELETE_PENDING))
    {
        ndisStatus = NDIS_STATUS_FAILURE;
        WFCInfo("[txmgr]Tx entry [TxId:%x][Port:%x][Peer:%x] being removed.  Failing NBL.",
            pTxEntry->TxId,
            pTxEntry->PortId,
            pTxEntry->PeerId);
        goto exit;
    }

    pTxQueue = GetTxQueue(pTxEntry, ExTidToId(pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExTID));

    if (pTxQueue == NULL)
    {
        pTxQueue = new WFC_TX_QUEUE;
        if (pTxQueue == NULL)
        {
            ndisStatus = NDIS_STATUS_RESOURCES;
            WFCError("[txmgr]Unable to allocate TxQueue for [Port:%x][Peer:%x][Tid:%u]",
                    pWfcFrame->WiFiFrameMetadata.u.txMetaData.PortID,
                    pWfcFrame->WiFiFrameMetadata.u.txMetaData.PeerID,
                    pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExTID);
            goto exit;
        }
        (pTxEntry->ppTxQueue)[ExTidToId(pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExTID)] = pTxQueue;
        InitializeTxQueue(pTxQueue, pTxEntry, pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExTID);
    }

    prevPeerBacklogCount = pTxEntry->BacklogCount;
    prevBacklogCount = pTxQueue->BacklogCount;

    InsertTailList(&pTxQueue->BacklogQueue, &pWifiFrameMeta->Linkage);
    pTxEntry->BacklogCount++;
    pTxQueue->BacklogCount++;

    //if state == STARTED add to scheduling structures
    if (pTxQueue->TxQueueState == TX_QUEUE_STARTED)
    {
        //backlog state changed
        if (prevBacklogCount == 0)
        {
            m_CGlobalTxQueueList.AddTxQueue(pTxQueue);
            AddTxQueueToAcEntry(pTxQueue);
        }
        m_CurActiveBacklog++;
    }

    if (m_QueueingMode == PEER_QUEUEING &&
        prevPeerBacklogCount == 0       &&
        IsPeerPaused(pTxEntry)             )
    {
        WFCTrace("[txmgr]PeerBacklog notification needed");
        *pbPeerBacklogNotifNeeded = TRUE;
    }

    //TODO Add to TxMgr stats.

    WFCTrace("[txmgr][Port:%x][Peer:%x][Tid:%u][BacklogCount:%u][TransferPendingCount:%u][TxPendingCount:%u]",
        pTxQueue->PortId,
        pTxQueue->PeerId,
        pTxQueue->Tid,
        pTxQueue->BacklogCount,
        pTxQueue->TransferPendingCount,
        pTxQueue->TxPendingCount);
exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

//TODO In future may optimize path to avoid queueing if no backlogged queues exist.
//TODO process chain of NBLs or single NBL?
NDIS_STATUS CTxMgr::ProcessNBL(
    _In_ PNET_BUFFER_LIST pNBL,
    _Out_ PBOOLEAN pbTargetDescResouces)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    ULONG numPages = 0;
    BOOLEAN bIsDescInit = FALSE;
    BOOLEAN bIsPeerBacklogNotifNeeded = FALSE;

    PWFC_FRAME pWfcFrame = NULL;
    PWDI_FRAME_METADATA pWifiFrameMeta = NULL;

    TraceEntry();

    *pbTargetDescResouces = FALSE;

    pWifiFrameMeta = GetWiFiFrameMetadataFromNbl(pNBL);
    if (pWifiFrameMeta == NULL)
    {
        ndisStatus = NDIS_STATUS_FAILURE;
        WFCError("[txmgr]Nbl doesn't have WDI_FRAME_METADATA");
        goto exit;
    }

    pWfcFrame = WFC_FRAME_FROM_WDI_FRAME(pWifiFrameMeta);

    numPages = CheckNumPages(pNBL);
    if (numPages > m_pDatapathCapabilities->TxMaxScatterGatherElementsPerFrame)
    {
        WFCTrace("[txmgr]NBL has too many pages (%u), trying to reallocate to fit within (%u)", numPages, m_pDatapathCapabilities->TxMaxScatterGatherElementsPerFrame);

        PNET_BUFFER_LIST pOrigNbl = pNBL;
        PNET_BUFFER_LIST pCloneNbl = NULL;
        //preserve backfill but no need to allocate and copy WFC_FRAME
        ndisStatus = m_pAdapter->CopyNetBufferLists(pOrigNbl, &pCloneNbl, m_BackFillSize, false);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("[txmgr]Unable to clone NBL (0x%x)", ndisStatus);
            goto exit;
        }
        //check number of pages in the clone before proceeding
        numPages = CheckNumPages(pCloneNbl);
        if (numPages > m_pDatapathCapabilities->TxMaxScatterGatherElementsPerFrame)
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            WFCError("[txmgr]The reallocated NBL has %u pages. Unable to send NBL", numPages);
            m_pAdapter->FreeCopiedNetBufferLists(pCloneNbl);
            goto exit;
        }
        //set the original wfcFrame to point to the cloned NBL and swap the NB chains
        //pCloneNbl being non-NULL indicates that the NBs have been swapped.
        pWfcFrame->u.Tx.pCloneNbl = pCloneNbl;

        PNET_BUFFER pOrigNb = NET_BUFFER_LIST_FIRST_NB(pOrigNbl);
        PNET_BUFFER pCloneNb = NET_BUFFER_LIST_FIRST_NB(pCloneNbl);

        NET_BUFFER_LIST_FIRST_NB(pOrigNbl) = pCloneNb;
        NET_BUFFER_LIST_FIRST_NB(pCloneNbl) = pOrigNb;
    }

    ndisStatus = TxTargetDescInit(pNBL);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        //if TxTargetDescInit failed for resources set the out param to true so caller can queue if desired.
        if (ndisStatus == NDIS_STATUS_RESOURCES)
        {
            *pbTargetDescResouces = TRUE;
        }
        WFCError("[txmgr]TxDescInit failed (0x%x) for frameId:%x",
            ndisStatus,
            pWfcFrame->WiFiFrameMetadata.FrameID);
        goto exit;
    }

    bIsDescInit = TRUE;

    {
        CAutoLock lock(m_TxMgrLock);

        pWfcFrame->FrameNum = GetFrameNum();

        ndisStatus = AddNBLToTxQueue(pNBL, &bIsPeerBacklogNotifNeeded);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("[txmgr]Unable to add NBL to TxQueue(0x%x)",ndisStatus);
            goto exit;
        }
        else if (m_bAllowOldFrameTimerQueueing)
        {
            //if the NBL was added and the timer is allowed to be queued then queue the timer
            QueueOldFrameDetectionTimer();
            m_bAllowOldFrameTimerQueueing = false;
        }
    }

    if (bIsPeerBacklogNotifNeeded)
    {
        m_pAdapter->m_MiniportDataHandlers.TxPeerBacklogHandler(
            m_pAdapter->GetMiniportTalTxRxContext(),
            pWfcFrame->WiFiFrameMetadata.u.txMetaData.PortID,
            pWfcFrame->WiFiFrameMetadata.u.txMetaData.PeerID,
            TRUE);
    }

exit: 
    if (ndisStatus != NDIS_STATUS_SUCCESS && bIsDescInit)
    {
        TxTargetDescDeinit(pNBL);
    }
    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTxMgr::TxTargetDescInit(
    _In_ PNET_BUFFER_LIST pNBL)
/*++
    Serializes calls to TX_TARGET_DESC_INIT using
    the m_TxTargetDescLock.
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    {
        CAutoLock lock(m_TxTargetDescLock);

        m_pAdapter->m_MiniportDataHandlers.TxTargetDescInitHandler(m_pAdapter->GetMiniportTalTxRxContext(),
                                                                   pNBL,
                                                                   &ndisStatus);

        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            //previously the same lock was used in calls to TxTargetDescInit and deinit.  Now that
            // the they are separate locks this operation needs to be interlocked to prevent races with
            // the modification or reading of m_CurInitedTxDescriptors
            InterlockedIncrement(&m_CurInitedTxDescriptors);
        }
    }

    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTxMgr::TxTargetDescDeinit(
    _In_ PNET_BUFFER_LIST pNBL)
/*++
    Serializes calls to TX_TARGET_DESC_DEINIT using
    the m_TxTargetDescDeinitLock.
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    {
        CAutoLock lock(m_TxTargetDescDeinitLock);

        m_pAdapter->m_MiniportDataHandlers.TxTargetDescDeInitHandler(m_pAdapter->GetMiniportTalTxRxContext(),
                                                                     pNBL);

        //Decrement the count of currently initted TxDescriptors
        PNET_BUFFER_LIST pTempNbl;
        for (pTempNbl = pNBL; pTempNbl != NULL; pTempNbl = NET_BUFFER_LIST_NEXT_NBL(pTempNbl))
        {
            //previously the same lock was used in calls to TxTargetDescInit and deinit.  Now that
            // the they are separate locks this operation needs to be interlocked to prevent races with
            // the modification or reading of m_CurInitedTxDescriptors
            InterlockedDecrement(&m_CurInitedTxDescriptors);
        }
    }

    TraceExit(ndisStatus);
    return ndisStatus;
}

void CTxMgr::TxDequeueInd(
    _In_ UINT32 Quantum,
    _In_ UINT8 MaxFrameCount,
    _In_ UINT16 Credit,
    _Out_ PNET_BUFFER_LIST *ppNBL)
/*++

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    NblChain chainToDequeue;

    PNET_BUFFER_LIST pCurNbl = NULL;

    PNET_BUFFER pCurNb = NULL;
    PWFC_TX_ENTRY pScheduledTxEntry = NULL;
    PWFC_TX_QUEUE pScheduledTxQueue = NULL;

    PWFC_FRAME pCurWfcFrame = NULL;
    PWDI_FRAME_METADATA pCurFrameMeta = NULL;
    PLIST_ENTRY pListEntry = NULL;

    UINT32 curDequeuedBytes = 0;
    UINT16 curDequeuedCost = 0;
    UINT16 lastDequeuedSeqNo = 0;
    UINT32 effectiveSize = 0;

    BOOLEAN bCountCredits = (Credit == WDI_TX_CREDIT_INVALID) ? FALSE : TRUE;
    BOOLEAN bCountFrames = (MaxFrameCount == WDI_TX_MAX_FRAME_COUNT_INVALID) ? FALSE : TRUE;
    BOOLEAN bCountSize = (Quantum == WDI_QUANTUM_INVALID) ? FALSE : TRUE;
    BOOLEAN bCheckReplaySeqNo = FALSE;
    //bIgnoreLimit is used to bypass the limit check in the event of replayed frames with 
    // the same SeqCtl (since they need to be dequeued together)
    BOOLEAN bIgnoreLimit = FALSE;

    TraceEntry();

    WFCTrace("[txmgr]Dequeue Indication [Quantum=0x%x][MaxFrameCount=0x%x][Credit=0x%x]",
            Quantum,
            MaxFrameCount,
            Credit);

    {
        CAutoLock lock(m_TxMgrLock);

        pScheduledTxEntry = m_TxEntryTable.GetTxEntry(GetTxId(m_ScheduledPortId, m_ScheduledPeerId));
        if (pScheduledTxEntry == NULL)
        {
            WFCInfo("[txmgr][TxId:%x][Port:%x][PeerId:%x] no longer in TxPeerTable",
                GetTxId(m_ScheduledPortId,m_ScheduledPeerId),
                m_ScheduledPortId,
                m_ScheduledPeerId);
            goto exit;
        }

        //The TxEntryState does not store TID pauses so this
        // check is for stopping/peer deleting etc
        if (pScheduledTxEntry->TxEntryState != TX_ENTRY_STARTED &&
            !IsIhvTid(m_ScheduledExtTid)                           )
        {
            WFCInfo("[txmgr][TxId:0x%x] not in active state. TxEntry state = %x",
                pScheduledTxEntry->TxId,
                pScheduledTxEntry->TxEntryState);
            goto exit;
        }

        pScheduledTxQueue = GetTxQueue(pScheduledTxEntry, ExTidToId(m_ScheduledExtTid));
        if (pScheduledTxQueue == NULL)
        {
            WFCInfo("[txmgr]TxQueue:[Port:%x][PeerId:%x][TID:%u] not present",
                m_ScheduledPortId,
                m_ScheduledPeerId,
                m_ScheduledExtTid);
            goto exit;
        }
        if (pScheduledTxQueue->BacklogCount == 0                ||
            pScheduledTxQueue->TxQueueState != TX_QUEUE_STARTED ||
            IsListEmpty(&pScheduledTxQueue->BacklogQueue)         )
        {
            WFCInfo("[txmgr]TxQueue:[PortId:%x][PeerId:%x][TID:%u] not active. Queue state = %x, num frames = %x",
                m_ScheduledPortId,
                m_ScheduledPeerId,
                m_ScheduledExtTid,
                pScheduledTxQueue->TxQueueState,
                pScheduledTxQueue->BacklogCount);
            goto exit;
        }
        //TODO handle replay case

        WFCTrace("[txmgr]Dequeuing from TxQueue:[PortId:%x][PeerId:0x%x][TID:%u][BacklogCount:%u]",
            m_ScheduledPortId,
            m_ScheduledPeerId,
            m_ScheduledExtTid,
            pScheduledTxQueue->BacklogCount);

        do
        {
            pListEntry = RemoveHeadList(&pScheduledTxQueue->BacklogQueue);

            pCurFrameMeta = (PWDI_FRAME_METADATA)CONTAINING_RECORD(pListEntry, WDI_FRAME_METADATA, Linkage);
            pCurWfcFrame = WFC_FRAME_FROM_WDI_FRAME(pCurFrameMeta);
            pCurNbl = pCurFrameMeta->pNBL;

            ASSERT(pCurNbl);

            //compute effective size;
            effectiveSize = 0;
            if (bCountSize)
            {
                pCurNb = NET_BUFFER_LIST_FIRST_NB(pCurNbl);

                ASSERT(pCurNb);

                while (pCurNb != NULL)
                {
                    effectiveSize += NET_BUFFER_DATA_LENGTH(pCurNb);
                    pCurNb = NET_BUFFER_NEXT_NB(pCurNb);
                }
                effectiveSize = (effectiveSize < m_pDatapathCapabilities->TxMinEffectiveFrameSize) ?
                    m_pDatapathCapabilities->TxMinEffectiveFrameSize :
                    effectiveSize;

                effectiveSize = (effectiveSize + m_pDatapathCapabilities->TxFrameSizeGranularity - 1) & ~(m_pDatapathCapabilities->TxFrameSizeGranularity - 1);

                WFCTrace("[txmgr]HOL frame:[Size:%u][Cost:%x]", effectiveSize, pCurFrameMeta->u.txMetaData.TxCost);
            }
            //check if both last frame and cur frame were replayed
            // and SeqCtl matches
            // if the replay data was not set then dequeue occurs normally.
            if (bCheckReplaySeqNo                                         &&
                pCurWfcFrame->u.Tx.bIsReplayDataSet                       &&
                (lastDequeuedSeqNo == pCurFrameMeta->u.txMetaData.SeqCtl)    )
            {
                bIgnoreLimit = TRUE;
                WFCInfo("[txmgr]Replayed frame with same SeqCtl as previous dequeued frame.  Ignoring dequeue limits for this frame.");
            }
            else
            {
                bIgnoreLimit = FALSE;
            }

            if (((bCountCredits) && (!bIgnoreLimit) && (curDequeuedCost + pCurFrameMeta->u.txMetaData.TxCost > Credit)) ||
                ((bCountFrames)  && (!bIgnoreLimit) && (chainToDequeue.GetCount() + 1 > MaxFrameCount)                ) ||
                ((bCountSize)    && (!bIgnoreLimit) && (curDequeuedBytes + effectiveSize > Quantum)                   )    )
            {
                WFCTrace("[txmgr]Unable to dequeue another frame.");
                InsertHeadList(&pScheduledTxQueue->BacklogQueue, pListEntry);
                break;
            }
            else
            {
                //add to list
                chainToDequeue.AddTail(pCurNbl);

                //check for replayed
                if (pCurWfcFrame->u.Tx.bIsReplayDataSet)
                {
                    lastDequeuedSeqNo = pCurFrameMeta->u.txMetaData.SeqCtl;
                    bCheckReplaySeqNo = TRUE;
                }
                else
                {
                    bCheckReplaySeqNo = FALSE;
                }

                curDequeuedCost += pCurFrameMeta->u.txMetaData.TxCost;
                curDequeuedBytes += effectiveSize;

                pScheduledTxQueue->BacklogCount--;
                pScheduledTxQueue->TransferPendingCount++;

                pScheduledTxEntry->BacklogCount--;
                pScheduledTxEntry->TransferPendingCount++;

                m_CurActiveBacklog--;
                m_CurOutstandingTransfers++;
            }

        } while (!IsListEmpty(&pScheduledTxQueue->BacklogQueue));

        WFCTrace("[txmgr]Dequeued %d frames from [PortId:0x%x][PeerId:0x%x][TID:%u]:[BacklogCount=%u]",
            chainToDequeue.GetCount(),
            m_ScheduledPortId,
            m_ScheduledPeerId,
            m_ScheduledExtTid,
            pScheduledTxQueue->BacklogCount
            );

        //TODO update AC tokens

        if (pScheduledTxQueue->BacklogCount == 0)
        {
            //no longer backlogged remove from scheduling structures
            m_CGlobalTxQueueList.RemoveTxQueue(pScheduledTxQueue);
            RemoveTxQueueFromAcEntry(pScheduledTxQueue);
        }
    }
exit:
    *ppNBL = chainToDequeue.GetHead();
    TraceExit(ndisStatus);
}

void CTxMgr::TxTransferCompleteInd(
    _In_ WDI_TX_FRAME_STATUS TxFrameStatus,
    _In_ PNET_BUFFER_LIST pNBL)
/*++
TODO
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PNET_BUFFER_LIST pCurNbl = NULL;
    PNET_BUFFER_LIST pNextNbl = NULL;

    BOOLEAN bNblNeedTxComplete = FALSE;
    BOOLEAN bCanSendComplete = TRUE;
    BOOLEAN bIsTxEntryValid = TRUE;
    BOOLEAN bPeerDeleteConfirm = FALSE;
    BOOLEAN bQueueInOrder = FALSE;
    BOOLEAN bPeerBacklogNotifNeeded = FALSE;

    WDI_PORT_ID tempPortId = WDI_PORT_ANY;
    WDI_PEER_ID tempPeerId = WDI_PEER_ANY;
    WDI_EXTENDED_TID tempTid = WDI_EXT_TID_UNKNOWN;

    PNET_BUFFER_LIST pNdisCompleteNbl = NULL;
    PNET_BUFFER_LIST pIhvCompleteNbl = NULL;

    PWFC_TX_ENTRY pTxEntry = NULL;
    PWFC_TX_QUEUE pTxQueue = NULL;

    PWDI_FRAME_METADATA pWifiFrameMeta = NULL;
    PWFC_FRAME pWfcFrame = NULL;

    TraceEntry();

    WFCTrace("[txmgr]Transfer complete status = %d", TxFrameStatus);

    NT_ASSERT(pNBL != NULL);

    pCurNbl = pNBL;

    while (pCurNbl != NULL)
    {
        pNextNbl = NET_BUFFER_LIST_NEXT_NBL(pCurNbl);
        NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = NULL;

        NT_ASSERT(pCurNbl->MiniportReserved[0] != NULL);

        WFCTrace("[txmgr]Transfer complete NBL = %p", pCurNbl);

        pWifiFrameMeta = GetWiFiFrameMetadataFromNbl(pCurNbl);
        pWfcFrame = (PWFC_FRAME)WFC_FRAME_FROM_WDI_FRAME(pWifiFrameMeta);

        { // m_TxMgrLock
            CAutoLock lock(m_TxMgrLock);

            pTxQueue = NULL;
            bNblNeedTxComplete = FALSE;
            bCanSendComplete = TRUE;
            bPeerDeleteConfirm = FALSE;
            bQueueInOrder = FALSE;
            bPeerBacklogNotifNeeded = FALSE;

            //TODO optimize this path to not get TxEntry every time if last NBL had same peerid/tid.
            pTxEntry = m_TxEntryTable.GetTxEntry(GetTxId(pWfcFrame));

            bIsTxEntryValid = (pTxEntry != NULL) ? TRUE : FALSE;

            if (!bIsTxEntryValid)
            {
                WFCInfo("[txmgr][TxId:%x][Port:%x][Peer:%x] not found. Frame will be completed but Peer counts may be corrupt",
                    GetTxId(pWfcFrame),
                    pWfcFrame->WiFiFrameMetadata.u.txMetaData.PortID,
                    pWfcFrame->WiFiFrameMetadata.u.txMetaData.PeerID);
            }

            if (bIsTxEntryValid)
            {
                pTxQueue = GetTxQueue(pTxEntry, ExTidToId(pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExTID));
                ASSERT(pTxQueue != NULL);
            }

            m_CurOutstandingTransfers--;

            if (bIsTxEntryValid)
            {
                pTxEntry->TransferPendingCount--;
                pTxQueue->TransferPendingCount--;
            }

            pWfcFrame->u.Tx.bIsTransferStatusSet = TRUE;
            pWfcFrame->u.Tx.TransferStatus = TxFrameStatus;

            if ((TxFrameStatus == WDI_TxFrameStatus_Ok && pWifiFrameMeta->u.txMetaData.bTxCompleteRequired == TRUE)            ||
                (TxFrameStatus == WDI_TxFrameStatus_Ok && m_pDatapathCapabilities->TxExplicitSendCompleteFlagRequired == FALSE)   )
            {
                bNblNeedTxComplete = TRUE;
            }

            //check if already send completed
            if (bNblNeedTxComplete && !pWfcFrame->u.Tx.bIsSendStatusSet)
            {
                if (bIsTxEntryValid)
                {
                    pTxQueue->TxPendingCount++;
                    pTxEntry->TxPendingCount++;
                }
                bCanSendComplete = FALSE;
            }

            // if there is no TxEntry then can't replay frame
            if (bIsTxEntryValid                                                &&
                bNblNeedTxComplete                                             &&
                pWfcFrame->u.Tx.bIsSendStatusSet                               &&
                pWfcFrame->u.Tx.SendStatus == WDI_TxFrameStatus_SendPostponed    )
            {
                NDIS_STATUS requeueStatus = RequeueNbl(pTxEntry,
                    pTxQueue,
                    pWfcFrame,
                    &bPeerBacklogNotifNeeded);
                //if requeue did not succeed then frame can be completed
                if (requeueStatus == NDIS_STATUS_SUCCESS)
                {
                    bCanSendComplete = FALSE;
                }
                tempPortId = pTxEntry->PortId;
                tempPeerId = pTxEntry->PeerId;
            }

            if (bCanSendComplete)
            {
                if (pWfcFrame->FrameType == TX_INJECTED_FRAME)
                {
                    NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = pIhvCompleteNbl;
                    pIhvCompleteNbl = pCurNbl;
                }
                else
                {
                    NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = pNdisCompleteNbl;
                    pNdisCompleteNbl = pCurNbl;
                }
            }

            if (bIsTxEntryValid                                      &&
                TEST_FLAG(pTxQueue->TxQueueState, TX_QUEUE_STOPPING) &&
                pTxQueue->TransferPendingCount == 0                  &&
                pTxQueue->TxPendingCount == 0                           )
            {
                CLEAR_FLAG(pTxQueue->TxQueueState, TX_QUEUE_STOPPING);
                SET_FLAG(pTxQueue->TxQueueState, TX_QUEUE_STOPPED);

                WFCInfo("[txmgr][Port:%x][Peer:%x][Tid:%u] is now in stopped state (%x)",
                    pTxQueue->PortId,
                    pTxQueue->PeerId,
                    pTxQueue->Tid,
                    pTxQueue->TxQueueState);
            }

            if (bIsTxEntryValid                                       &&
                TEST_FLAG(pTxQueue->TxQueueState, TX_QUEUE_PS_PAUSED) &&
                pTxQueue->TransferPendingCount == 0                   &&
                pTxQueue->TxPendingCount == 0                            )
            {
                WFCInfo("[txmgr][Port:%x][Peer:%x][Tid:%u] is now in order",
                    pTxQueue->PortId,
                    pTxQueue->PeerId,
                    pTxQueue->Tid);

                bQueueInOrder = TRUE;
                tempPortId = pTxQueue->PortId;
                tempPeerId = pTxQueue->PeerId;
                tempTid = pTxQueue->Tid;
            }

            if (bIsTxEntryValid                                      &&
                TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPING) &&
                pTxEntry->TransferPendingCount == 0                  &&
                pTxEntry->TxPendingCount == 0                           )
            {
                CLEAR_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPING);
                SET_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPED);

                WFCInfo("[txmgr][TxId:%x] is now in stopped state (%x)",
                    pTxEntry->TxId,
                    pTxEntry->TxEntryState);

                if (TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_PEER_DELETE_PENDING))
                {
                    bPeerDeleteConfirm = TRUE;
                    tempPortId = pTxEntry->PortId;
                    tempPeerId = pTxEntry->PeerId;
                }
            }
        } // m_TxMgrLock

        if (bPeerDeleteConfirm)
        {
            WFCInfo("[txmgr] Calling PeerDeleteConfirm for [PortId:%x][PeerId:%x]",
                tempPortId,
                tempPeerId);
            PeerDeleteConfirm(tempPortId, tempPeerId);
        }

        if (bPeerBacklogNotifNeeded)
        {
            NT_ASSERT(m_QueueingMode == PEER_QUEUEING);

            WFCInfo("[txmgr] Calling TxPeerBacklog for [PortId:%x][PeerId:%x]",
                tempPortId,
                tempPeerId);

            m_pAdapter->m_MiniportDataHandlers.TxPeerBacklogHandler(
                m_pAdapter->GetMiniportTalTxRxContext(),
                tempPortId,
                tempPeerId,
                TRUE);
        }

        if (bQueueInOrder)
        {
            NT_ASSERT(m_QueueingMode == PEER_QUEUEING);

            WFCInfo("[txmgr] Calling TxQueueInOrder for [PortId:%x][PeerId:%x][Tid:%x]",
                tempPortId,
                tempPeerId,
                tempTid);

            m_pAdapter->m_MiniportDataHandlers.TxTalQueueInOrderHandler(m_pAdapter->GetMiniportTalTxRxContext(),
                tempPeerId,
                (1 << tempTid));
        }
        pCurNbl = pNextNbl;
    }

    if (pIhvCompleteNbl)
    {
        CompleteIhvNbl(pIhvCompleteNbl);
    }
    if (pNdisCompleteNbl)
    {
        //This is a common path.  Do not defer completion.
        CompleteNdisNbl(pNdisCompleteNbl, FALSE);
    }
    TraceExit(ndisStatus);
}

void CTxMgr::TxSendCompleteInd(
    _In_ WDI_TX_FRAME_STATUS TxFrameStatus,
    _In_ UINT16 NumCompletedSends,
    _In_reads_(NumCompletedSends) WDI_FRAME_ID* TxFrameIdList,
    _In_reads_opt_(NumCompletedSends) WDI_TX_COMPLETE_DATA* TxCompleteDataList)
/*++
TODO
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    PNET_BUFFER_LIST pCurNbl = NULL;

    PNET_BUFFER_LIST pNdisCompleteNbl = NULL;
    PNET_BUFFER_LIST pIhvCompleteNbl = NULL;

    PWFC_TX_ENTRY pTxEntry = NULL;
    PWFC_TX_QUEUE pTxQueue = NULL;

    PWDI_FRAME_METADATA pWifiFrameMeta = NULL;
    PWFC_FRAME pWfcFrame = NULL;

    WDI_FRAME_ID curFrameId = (WDI_FRAME_ID)-1;

    BOOLEAN bTxCompleteDataValid = FALSE;
    BOOLEAN bIsTxEntryValid = TRUE;
    BOOLEAN bPeerDeleteConfirm = FALSE;
    BOOLEAN bQueueInOrder = FALSE;
    BOOLEAN bPeerBacklogNotifNeeded = FALSE;

    WDI_PORT_ID tempPortId = WDI_PORT_ANY;
    WDI_PEER_ID tempPeerId = WDI_PEER_ANY;
    WDI_EXTENDED_TID tempTid = WDI_EXT_TID_UNKNOWN;

    WDI_TX_FRAME_STATUS frameStatus = TxFrameStatus;

    int i;

    TraceEntry();

    WFCTrace("[txmgr]Send complete status = %d", TxFrameStatus);

    NT_ASSERT(TxFrameIdList != NULL);

    if (TxCompleteDataList)
    {
        bTxCompleteDataValid = TRUE;
        WFCTrace("[txmgr]TxComplete data is valid");
    }


    for (i = 0; i < NumCompletedSends; i++)
    {
        pCurNbl = NULL;
        pTxQueue = NULL;

        curFrameId = TxFrameIdList[i];
        bPeerDeleteConfirm = FALSE;
        bQueueInOrder = FALSE;
        frameStatus = TxFrameStatus;
        bPeerBacklogNotifNeeded = FALSE;

        pCurNbl = m_pAdapter->m_FrameIdTable.FindNblByFrameId(curFrameId);

        if (pCurNbl == NULL)
        {
            WFCError("[txmgr]ERROR could not find frame with id %x in frameId table",
                curFrameId);
            continue;
        }

        WFCTrace("[txmgr]FrameId:%x points to NBL:%p",
            curFrameId,
            pCurNbl);

        pWifiFrameMeta = GetWiFiFrameMetadataFromNbl(pCurNbl);

        NT_ASSERT(pWifiFrameMeta);

        pWfcFrame = WFC_FRAME_FROM_WDI_FRAME(pWifiFrameMeta);

        { // m_TxMgrLock
            CAutoLock lock(m_TxMgrLock);

            if (pWfcFrame->u.Tx.bIsSendStatusSet == TRUE)
            {
                WFCError("[txmgr]ERROR duplicate SendComplete for frameId:%x",
                    curFrameId);
                continue;
            }

            pWfcFrame->u.Tx.bIsSendStatusSet = TRUE;
            pWfcFrame->u.Tx.SendStatus = frameStatus;

            //TODO optimize this path to not get TxEntry every time if last NBL had same (port|peer)id/tid.
            pTxEntry = m_TxEntryTable.GetTxEntry(GetTxId(pWfcFrame));

            bIsTxEntryValid = (pTxEntry != NULL) ? TRUE : FALSE;

            if (!bIsTxEntryValid)
            {
                WFCInfo("[txmgr][TxId:%x] not found. Frame will be completed but TxEntry counts may be corrupt",
                    GetTxId(pWfcFrame));
            }

            if (bIsTxEntryValid)
            {
                pTxQueue = GetTxQueue(pTxEntry, ExTidToId(pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExTID));
                ASSERT(pTxQueue != NULL);
            }

            NT_ASSERT(pWfcFrame->u.Tx.bIsTransferStatusSet == FALSE           ||
                      pWfcFrame->u.Tx.TransferStatus == WDI_TxFrameStatus_Ok    );

            //if sendpostponed then copy replay information
            //pause peer for ps
            if (frameStatus == WDI_TxFrameStatus_SendPostponed &&
                bIsTxEntryValid                                    )
            {
                NT_ASSERT(m_QueueingMode == PEER_QUEUEING);

                if (bTxCompleteDataValid)
                {
                    pWifiFrameMeta->u.txMetaData.SeqCtl = TxCompleteDataList[i].SeqCtl;
                    pWifiFrameMeta->u.txMetaData.PnLength = TxCompleteDataList[i].PnLength;
                    pWifiFrameMeta->u.txMetaData.MpduPn = TxCompleteDataList[i].MpduPN;
                    pWifiFrameMeta->u.txMetaData.ReplayIHVReserved0 = TxCompleteDataList[i].ReplayIHVReserved0;
                    pWifiFrameMeta->u.txMetaData.ReplayIHVReserved1 = TxCompleteDataList[i].ReplayIHVReserved1;
                    pWfcFrame->u.Tx.bIsReplayDataSet = TRUE;
                }
                PausePeerEntry(pTxEntry,
                    EXT_TID_OS_MASK | EXT_TID_IHV_MASK,
                    WDI_TX_PAUSE_REASON_PS);
            }


            if (pWfcFrame->u.Tx.bIsTransferStatusSet &&
                bIsTxEntryValid                         )
            {
                pTxQueue->TxPendingCount--;
                pTxEntry->TxPendingCount--;
            }

            if (pWfcFrame->u.Tx.bIsTransferStatusSet              &&
                frameStatus == WDI_TxFrameStatus_SendPostponed   &&
                bIsTxEntryValid                                      )
            {
                NDIS_STATUS requeueStatus = RequeueNbl(pTxEntry,
                    pTxQueue,
                    pWfcFrame,
                    &bPeerBacklogNotifNeeded);

                //if requeue failed then complete frame
                if (requeueStatus != NDIS_STATUS_SUCCESS)
                {
                    frameStatus = WDI_TxFrameStatus_Ok;
                }
                tempPortId = pTxEntry->PortId;
                tempPeerId = pTxEntry->PeerId;
            }

            //if TxEntry invalid then replay won't be attempted.  complete frame
            //if the requeue failed then complete frame.
            if (pWfcFrame->u.Tx.bIsTransferStatusSet                                    &&
                (frameStatus != WDI_TxFrameStatus_SendPostponed || !bIsTxEntryValid)      )
            {
                if (pWfcFrame->FrameType == TX_INJECTED_FRAME)
                {
                    NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = pIhvCompleteNbl;
                    pIhvCompleteNbl = pCurNbl;
                }
                else
                {
                    NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = pNdisCompleteNbl;
                    pNdisCompleteNbl = pCurNbl;
                }
            }

            if (bIsTxEntryValid                                      &&
                TEST_FLAG(pTxQueue->TxQueueState, TX_QUEUE_STOPPING) &&
                pTxQueue->TransferPendingCount == 0                  &&
                pTxQueue->TxPendingCount == 0                           )
            {
                CLEAR_FLAG(pTxQueue->TxQueueState, TX_QUEUE_STOPPING);
                SET_FLAG(pTxQueue->TxQueueState, TX_QUEUE_STOPPED);

                WFCInfo("[txmgr][Port:%x][Peer:%x][Tid:%u] is now in stopped state (%x)",
                    pTxQueue->PortId,
                    pTxQueue->PeerId,
                    pTxQueue->Tid,
                    pTxQueue->TxQueueState);
            }

            if (bIsTxEntryValid                                       &&
                TEST_FLAG(pTxQueue->TxQueueState, TX_QUEUE_PS_PAUSED) &&
                pTxQueue->TransferPendingCount == 0                   &&
                pTxQueue->TxPendingCount == 0                            )
            {
                WFCInfo("[txmgr][Port:%x][Peer:%x][Tid:%u] is now in order",
                    pTxQueue->PortId,
                    pTxQueue->PeerId,
                    pTxQueue->Tid);

                bQueueInOrder = TRUE;
                tempPortId = pTxQueue->PortId;
                tempPeerId = pTxQueue->PeerId;
                tempTid = pTxQueue->Tid;
            }

            if (bIsTxEntryValid                                      &&
                TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPING) &&
                pTxEntry->TransferPendingCount == 0                  &&
                pTxEntry->TxPendingCount == 0                           )
            {
                CLEAR_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPING);
                SET_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPED);

                WFCInfo("[txmgr][TxId:%x] is now in stopped state (%x)",
                    pTxEntry->TxId,
                    pTxEntry->TxEntryState);

                if (TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_PEER_DELETE_PENDING))
                {
                    bPeerDeleteConfirm = TRUE;
                    tempPortId = pTxEntry->PortId;
                    tempPeerId = pTxEntry->PeerId;
                }
            }
        } // m_TxMgrLock

        if (bPeerDeleteConfirm)
        {
            WFCInfo("[txmgr] Calling PeerDeleteConfirm for [PortId:%x][PeerId:%x]",
                tempPortId,
                tempPeerId);
            PeerDeleteConfirm(tempPortId, tempPeerId);
        }

        if (bPeerBacklogNotifNeeded)
        {
            NT_ASSERT(m_QueueingMode == PEER_QUEUEING);

            WFCInfo("[txmgr] Calling TxPeerBacklog for [PortId:%x][PeerId:%x]",
                tempPortId,
                tempPeerId);

            m_pAdapter->m_MiniportDataHandlers.TxPeerBacklogHandler(
                m_pAdapter->GetMiniportTalTxRxContext(),
                tempPortId,
                tempPeerId,
                TRUE);
        }

        if (bQueueInOrder)
        {
            NT_ASSERT(m_QueueingMode == PEER_QUEUEING);

            WFCInfo("[txmgr] Calling TxQueueInOrder for [PortId:%x][PeerId:%x][Tid:%x]",
                tempPortId,
                tempPeerId,
                tempTid);

            m_pAdapter->m_MiniportDataHandlers.TxTalQueueInOrderHandler(m_pAdapter->GetMiniportTalTxRxContext(),
                tempPeerId,
                (1 << tempTid));
        }
    }

    if (pIhvCompleteNbl)
    {
        CompleteIhvNbl(pIhvCompleteNbl);
    }
    if (pNdisCompleteNbl)
    {
        //This is a common path.  Do not defer completion.
        CompleteNdisNbl(pNdisCompleteNbl, FALSE);
    }
    TraceExit(ndisStatus);
    return;
}

NDIS_STATUS CTxMgr::RequeueNbl(
    _In_ PWFC_TX_ENTRY pTxEntry,
    _In_ PWFC_TX_QUEUE pTxQueue,
    _In_ PWFC_FRAME pWfcFrame,
    _Out_ PBOOLEAN pbPeerBacklogNotifNeeded)
/*++
    Called with txmgr lock held
    resets set fields (such as tx/transfer completion status and set bits)
    requeues in proper place using the frame number to sort
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PLIST_ENTRY pListEntry = NULL;
    PWDI_FRAME_METADATA pCurFrameMeta = NULL;
    PWFC_FRAME pCurWfcFrame = NULL;
    
    UINT64 prevPeerBacklogCount = 0;

    TraceEntry();

    if (pbPeerBacklogNotifNeeded == NULL)
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("[txmgr]pbPeerBacklogNotifNeeded is null");
        goto exit;
    }

    *pbPeerBacklogNotifNeeded = FALSE;

    //reset transfer/tx fields
    pWfcFrame->u.Tx.bIsTransferStatusSet = FALSE;
    pWfcFrame->u.Tx.TransferStatus = WDI_TxFrameStatus_Ok;
    pWfcFrame->u.Tx.bIsSendStatusSet = FALSE;
    pWfcFrame->u.Tx.SendStatus = WDI_TxFrameStatus_Ok;

    //requeue unless we are trying to stop/delete the peer
    if (TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_PEER_DELETE_PENDING) ||
        TEST_FLAG(pTxEntry->TxEntryState, TX_ENTRY_STOPPING)               )
    {
        ndisStatus = NDIS_STATUS_FAILURE;
        WFCInfo("[txmgr]Tx entry [TxId:%x][Port:%x][Peer:%x] being removed.  Failing NBL.",
            pTxEntry->TxId,
            pTxEntry->PortId,
            pTxEntry->PeerId);
        goto exit;
    }

    if (IsListEmpty(&pTxQueue->BacklogQueue))
    {
        InsertTailList(&pTxQueue->BacklogQueue, &pWfcFrame->WiFiFrameMetadata.Linkage);
    }
    else
    {
        pListEntry = pTxQueue->BacklogQueue.Flink;

        while (pListEntry != &pTxQueue->BacklogQueue)
        {
            pCurFrameMeta = (PWDI_FRAME_METADATA)CONTAINING_RECORD(pListEntry, WDI_FRAME_METADATA, Linkage);
            pCurWfcFrame = WFC_FRAME_FROM_WDI_FRAME(pCurFrameMeta);

            if (pWfcFrame->FrameNum < pCurWfcFrame->FrameNum)
            {
                WFCInfo("[txmgr]Found correct entry for requeued frame. Inserting FrameNum:%llu before FrameNum:%llu.",
                    pWfcFrame->FrameNum,
                    pCurWfcFrame->FrameNum);
                break;
            }

            pListEntry = pListEntry->Flink;
        }
        InsertHeadList(pListEntry, &pWfcFrame->WiFiFrameMetadata.Linkage);

    }

    prevPeerBacklogCount = pTxEntry->BacklogCount;

    pTxEntry->BacklogCount++;
    pTxQueue->BacklogCount++;

    //The queue should be paused since requeueing when the queue isn't pspaused is disallowed
    NT_ASSERT(pTxQueue->TxQueueState != TX_QUEUE_STARTED);

    if (prevPeerBacklogCount == 0)
    {
        WFCTrace("[txmgr]PeerBacklog notification needed");
        *pbPeerBacklogNotifNeeded = TRUE;
    }

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

void CTxMgr::TxQueryRaTidStateInd(
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ WDI_EXTENDED_TID ExTid,
    _Out_ NDIS_STATUS* pQueueStatus,
    _Out_ UINT16* pQueueLength)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    PWFC_TX_ENTRY pCurTxEntry = NULL;
    PWFC_TX_QUEUE pCurTxQueue = NULL;

    UINT16 queueLength = 0;

    TraceEntry();

    WFCTrace("[txmgr]QueryRaTidStateInd [PortId:%x][PeerId:%x][ExTid:%x]",
        PortId,
        PeerId,
        ExTid);

    if (PortId == WDI_PORT_ANY)
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("[txmgr]Wildcard PortId not allowed.");
        goto exit;
    }

    if (m_QueueingMode == PEER_QUEUEING &&
        PeerId == WDI_PEER_ANY             )
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("[txmgr]Wildcard PeerId not allowed in PeerQueueing mode.");
        goto exit;
    }

    if (m_QueueingMode == PORT_QUEUEING &&
        PeerId != WDI_PEER_ANY             )
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("[txmgr]Specific PeerID not allowed in PortQueueing mode.");
        goto exit;
    }

    if (!IsValidTid(ExTid))
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("[txmgr]Invalid extended TID.");
        goto exit;
    }

    { //m_TxMgrLock
        CAutoLock lock(m_TxMgrLock);

        pCurTxEntry = m_TxEntryTable.GetTxEntry(GetTxId(PortId, PeerId));
        if (pCurTxEntry == NULL)
        {
            WFCInfo("[txmgr][TxId:0x%x][PortId:0x%x][PeerId:0x%x] not found in peer table",
                GetTxId(PortId, PeerId),
                PortId,
                PeerId);
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            goto exit;
        }

        pCurTxQueue = GetTxQueue(pCurTxEntry, ExTidToId(ExTid));
        if (pCurTxQueue == NULL)
        {
            WFCTrace("[txmgr]TxQueue:[PortId:0x%x][PeerId:0x%x][TID:%u] not present.  Backlog is 0.",
                PortId,
                PeerId,
                ExTid);
            queueLength = 0;
            goto exit;
        }
        else
        {
            WFCTrace("[txmgr]TxQueue:[PortId:0x%x][PeerId:0x%x][TID:%u] queue length %u.",
                PortId,
                PeerId,
                ExTid,
                pCurTxQueue->BacklogCount);
            queueLength = pCurTxQueue->BacklogCount;
        }
    } //m_TxMgrLock

exit:
    TraceExit(ndisStatus);

    *pQueueStatus = ndisStatus;
    *pQueueLength = queueLength;
}

NDIS_STATUS CTxMgr::PausePeerEntry(
    _In_ PWFC_TX_ENTRY pTxEntry,
    _In_ UINT32 ExTidBitmask,
    _In_ WDI_TX_PAUSE_REASON TxPauseReason)
/*++
    called with txmgr lock held
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_TX_QUEUE pTxQueue = NULL;
    UINT32 prevPauseState = 0;
    int i;

    TraceEntry();

    WFCInfo("[txmgr]Pausing [TxId:%x][PortId:%x][PeerId:%x][TidBitmask:%x][PauseReasonBitmask:%x] Current state:%x",
        pTxEntry->TxId,
        pTxEntry->PortId,
        pTxEntry->PeerId,
        ExTidBitmask,
        TxPauseReason,
        pTxEntry->TxEntryState);

    //A TxEntry TxEntryState will not store the pause state.

    for (i = 0; IsValidTid((WDI_EXTENDED_TID)i); i++)
    {
        //If this tid is in the bitmask
        if(ExTidBitmask & (1 << i))
        {
            prevPauseState = pTxEntry->TidPauseState[i];

            pTxEntry->TidPauseState[i] = (WDI_TX_PAUSE_REASON)(prevPauseState | TxPauseReason);
            pTxQueue = GetTxQueue(pTxEntry, (WDI_EXTENDED_TID)i);

            if (pTxQueue != NULL)
            {
                //this is safe because the TxPauseReason was checked to make sure that it had at least
                // one bit set.  This may double set the TX_QUEUE_PAUSED flag but that isn't an issue
                // as a double remove is prevented below.
                if (TxPauseReason & WDI_TX_PAUSE_REASON_PS)
                {
                    SET_FLAG(pTxQueue->TxQueueState, TX_QUEUE_PS_PAUSED);
                }
                //TX_QUEUE_PS_PAUSED is a subset of TX_QUEUE_PAUSED
                SET_FLAG(pTxQueue->TxQueueState, TX_QUEUE_PAUSED);

                //Don't remove from scheduling structures unless TID was previously unpaused
                if (prevPauseState == 0)
                {
                    //remove from scheduling structure
                    if (m_CGlobalTxQueueList.RemoveTxQueue(pTxQueue) == NDIS_STATUS_SUCCESS)
                    {
                        m_CurActiveBacklog -= pTxQueue->BacklogCount;
                    }
                    else
                    {
                        WFCInfo("[txmgr]TxQueue wasn't removed from the global queue list");
                    }
                    RemoveTxQueueFromAcEntry(pTxQueue);
                }
            }
        }
        else
        {
            WFCTrace("[txmgr]ExTid:%x not in ExTidBitmask", i);
        }
    }

    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTxMgr::PausePortEntry(
    _In_ PWFC_TX_ENTRY pTxEntry,
    _In_ UINT32 IdBitmask,
    _In_ WDI_TX_PAUSE_REASON TxPauseReason)
    /*++
    called with txmgr lock held
    --*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_TX_QUEUE pTxQueue = NULL;
    UINT32 prevPauseState = 0;
    int i;

    TraceEntry();

    WFCInfo("[txmgr]Pausing [TxId:%x][PortId:%x][PeerId:%x][IdBitmask:%x][PauseReason:%x] Current state:%x",
        pTxEntry->TxId,
        pTxEntry->PortId,
        pTxEntry->PeerId,
        IdBitmask,
        TxPauseReason,
        pTxEntry->TxEntryState);

    for (i = 0; IsValidId((WFC_TX_QUEUE_ID)i); i++)
    {
        //If this ID is in the bitmask
        if (IdBitmask & (1 << i))
        {
            prevPauseState = pTxEntry->TidPauseState[i];

            pTxEntry->TidPauseState[i] = (WDI_TX_PAUSE_REASON)(prevPauseState | TxPauseReason);
            pTxQueue = GetTxQueue(pTxEntry, (WFC_TX_QUEUE_ID)i);

            if (pTxQueue != NULL)
            {
                SET_FLAG(pTxQueue->TxQueueState, TX_QUEUE_PAUSED);

                //Don't remove from scheduling structures unless queue was previously unpaused
                if (prevPauseState == 0)
                {
                    //remove from scheduling structure
                    if (m_CGlobalTxQueueList.RemoveTxQueue(pTxQueue) == NDIS_STATUS_SUCCESS)
                    {
                        m_CurActiveBacklog -= pTxQueue->BacklogCount;
                    }
                    else
                    {
                        WFCInfo("[txmgr]TxQueue wasn't removed from the global queue list");
                    }
                    RemoveTxQueueFromAcEntry(pTxQueue);
                }
            }
        }
        else
        {
            WFCTrace("[txmgr]Id:%x not in IdBitmask", i);
        }
    }

    TraceExit(ndisStatus);
    return ndisStatus;
}

void CTxMgr::TxSendPauseInd(
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ UINT32 ExTidBitmask,
    _In_ WDI_TX_PAUSE_REASON TxPauseReason)
/*++
    TxMgr will pause the appropriate TxEntries and TxQueues
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    PWFC_TX_ENTRY pTxEntry = NULL;

    PWFC_TX_PEER_BACKLOG_NOTIFICATION pTempPeerBacklogNotif = NULL;

    LIST_ENTRY peerBacklogListHead;
    InitializeListHead(&peerBacklogListHead);

    PLIST_ENTRY pTempEntry = NULL;

    TraceEntry();

    WFCInfo("[txmgr]SendPauseInd [PortId:%x][PeerId:%x][TidBitmask:%x][PauseReason:%x]",
        PortId,
        PeerId,
        ExTidBitmask,
        TxPauseReason);

    if (!IsValidPauseReason(TxPauseReason))
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("[txmgr]Invalid PauseReason");
        goto exit;
    }

    if (m_QueueingMode == PORT_QUEUEING &&
        PeerId != WDI_PEER_ANY            )
    {
        WFCInfo("[txmgr]Ignoring peer pause due to PortQueueing mode");
        goto exit;
    }

    { //m_TxMgrLock
        CAutoLock lock(m_TxMgrLock);

        //specific peer
        if(PortId != WDI_PORT_ANY &&
           PeerId != WDI_PEER_ANY    )
        {
            //specific port and specific peer
            pTxEntry = m_TxEntryTable.GetTxEntry(PeerId);
            if (pTxEntry == NULL)
            {
                WFCError("[txmgr][PeerId:%x] not found",
                    PeerId);
                ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
                goto exit;
            }
            ndisStatus = PausePeerEntry(pTxEntry, ExTidBitmask, TxPauseReason);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCError("[txmgr]Error pausing [PeerId:%x]", pTxEntry->PeerId);
                goto exit;
            }

            pTempPeerBacklogNotif = new WFC_TX_PEER_BACKLOG_NOTIFICATION;
            if (pTempPeerBacklogNotif)
            {
                pTempPeerBacklogNotif->PortId = pTxEntry->PortId;
                pTempPeerBacklogNotif->PeerId = pTxEntry->PeerId;
                pTempPeerBacklogNotif->Backlogged = (pTxEntry->BacklogCount == 0) ? FALSE : TRUE;

                WFCInfo("[txmgr]Adding PeerBacklogNotification [Port:%x][Peer:%x][Backlog:%x]",
                    pTempPeerBacklogNotif->PortId,
                    pTempPeerBacklogNotif->PeerId,
                    pTempPeerBacklogNotif->Backlogged);

                InsertTailList(&peerBacklogListHead, &pTempPeerBacklogNotif->ListEntry);
            }
            pTempPeerBacklogNotif = NULL;
        }
        else if (PortId != WDI_PORT_ANY &&
                 PeerId == WDI_PEER_ANY    )
        {
            if (m_QueueingMode == PORT_QUEUEING)
            {
                //specific port
                pTxEntry = m_TxEntryTable.GetTxEntry(PortId);
                if (pTxEntry == NULL)
                {
                    WFCError("[txmgr][PortId:%x] not found",
                        PortId);
                    ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
                    goto exit;
                }
                ndisStatus = PausePortEntry(pTxEntry, ExTidBitmaskToTxQueueIdBitmask(ExTidBitmask), TxPauseReason);
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    WFCError("[txmgr]Error pausing [PortId:%x]", pTxEntry->PortId);
                    goto exit;
                }
            }
            else
            {
                //all peers on specific port
                UINT Index = 0;

                pTxEntry = m_TxEntryTable.GetNextPortPeerEntry(PortId, &Index);
                while (pTxEntry != NULL)
                {
                    ndisStatus = PausePeerEntry(pTxEntry, ExTidBitmask, TxPauseReason);
                    if (ndisStatus != NDIS_STATUS_SUCCESS)
                    {
                        WFCError("[txmgr]Error pausing [PeerId:%x]", pTxEntry->PeerId);
                        //TODO continue?
                    }

                    pTempPeerBacklogNotif = new WFC_TX_PEER_BACKLOG_NOTIFICATION;
                    if (pTempPeerBacklogNotif)
                    {
                        pTempPeerBacklogNotif->PortId = pTxEntry->PortId;
                        pTempPeerBacklogNotif->PeerId = pTxEntry->PeerId;
                        pTempPeerBacklogNotif->Backlogged = (pTxEntry->BacklogCount == 0) ? FALSE : TRUE;

                        WFCInfo("[txmgr]Adding PeerBacklogNotification [Port:%x][Peer:%x][Backlog:%x]",
                            pTempPeerBacklogNotif->PortId,
                            pTempPeerBacklogNotif->PeerId,
                            pTempPeerBacklogNotif->Backlogged);

                        InsertTailList(&peerBacklogListHead, &pTempPeerBacklogNotif->ListEntry);
                    }
                    pTempPeerBacklogNotif = NULL;

                    pTxEntry = m_TxEntryTable.GetNextPortPeerEntry(PortId, &Index);
                }
            }
            
        }
        else if (PortId == WDI_PORT_ANY &&
                 PeerId == WDI_PEER_ANY    )
        {
            //all entries on all ports
            UINT Index = 0;

            pTxEntry = m_TxEntryTable.GetNextTxEntry(&Index);
            while (pTxEntry != NULL)
            {
                if (m_QueueingMode == PORT_QUEUEING)
                {
                    ndisStatus = PausePortEntry(pTxEntry, ExTidBitmaskToTxQueueIdBitmask(ExTidBitmask), TxPauseReason);
                }
                else
                {
                    ndisStatus = PausePeerEntry(pTxEntry, ExTidBitmask, TxPauseReason);

                    pTempPeerBacklogNotif = new WFC_TX_PEER_BACKLOG_NOTIFICATION;
                    if (pTempPeerBacklogNotif)
                    {
                        pTempPeerBacklogNotif->PortId = pTxEntry->PortId;
                        pTempPeerBacklogNotif->PeerId = pTxEntry->PeerId;
                        pTempPeerBacklogNotif->Backlogged = (pTxEntry->BacklogCount == 0) ? FALSE : TRUE;

                        WFCInfo("[txmgr]Adding PeerBacklogNotification [Port:%x][Peer:%x][Backlog:%x]",
                            pTempPeerBacklogNotif->PortId,
                            pTempPeerBacklogNotif->PeerId,
                            pTempPeerBacklogNotif->Backlogged);

                        InsertTailList(&peerBacklogListHead, &pTempPeerBacklogNotif->ListEntry);
                    }
                    pTempPeerBacklogNotif = NULL;
                }

                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    WFCError("[txmgr]Error pausing [TxId:%x]", pTxEntry->TxId);
                    //TODO continue?
                }

                pTxEntry = m_TxEntryTable.GetNextTxEntry(&Index);
            }
        }
    } //m_TxMgrLock

exit:
    //Issue PeerBacklog indications
    while (!IsListEmpty(&peerBacklogListHead))
    {
        pTempEntry = RemoveHeadList(&peerBacklogListHead);

        pTempPeerBacklogNotif = CONTAINING_RECORD(pTempEntry, WFC_TX_PEER_BACKLOG_NOTIFICATION, ListEntry);

        WFCTrace("[txmgr]Calling TxPeerBacklog for [PortId:%x][PeerId:%x][Backlog:%x]",
            pTempPeerBacklogNotif->PortId,
            pTempPeerBacklogNotif->PeerId,
            pTempPeerBacklogNotif->Backlogged);

        m_pAdapter->m_MiniportDataHandlers.TxPeerBacklogHandler(
            m_pAdapter->GetMiniportTalTxRxContext(),
            pTempPeerBacklogNotif->PortId,
            pTempPeerBacklogNotif->PeerId,
            pTempPeerBacklogNotif->Backlogged);

        delete pTempPeerBacklogNotif;
        pTempPeerBacklogNotif = NULL;
    }

    TraceExit(ndisStatus);
    return;
}

NDIS_STATUS CTxMgr::ResumePeerEntry(
    _In_ PWFC_TX_ENTRY pTxEntry,
    _In_ UINT32 ExTidBitmask,
    _In_ WDI_TX_PAUSE_REASON TxRestartReason)
    /*++
    called with txmgr lock held
    --*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_TX_QUEUE pPeerTidQueue = NULL;
    UINT32 prevPauseState = 0;
    int i;

    TraceEntry();

    WFCInfo("[txmgr]Resuming [PeerId:%x][TidBitmask:%x][RestartReason:%x] Current state:%x",
        pTxEntry->PeerId,
        ExTidBitmask,
        TxRestartReason,
        pTxEntry->TxEntryState);


    for (i = 0; IsValidTid((WDI_EXTENDED_TID)i); i++)
    {
        //If this tid is in the bitmask
        if (ExTidBitmask & (1 << i))
        {
            prevPauseState = pTxEntry->TidPauseState[i];
            pTxEntry->TidPauseState[i] = (WDI_TX_PAUSE_REASON)(prevPauseState & ~TxRestartReason);

            pPeerTidQueue = GetTxQueue(pTxEntry, (WDI_EXTENDED_TID)i);

            if (pPeerTidQueue != NULL)
            {
                //only clear the flag if the restart reason contained PS
                if (TxRestartReason & WDI_TX_PAUSE_REASON_PS)
                {
                    CLEAR_FLAG(pPeerTidQueue->TxQueueState, TX_QUEUE_PS_PAUSED);
                }
                //to unpause the TidPauseState must not have any bit set
                if (pTxEntry->TidPauseState[i] == 0)
                {
                    CLEAR_FLAG(pPeerTidQueue->TxQueueState, TX_QUEUE_PAUSED);
                }

                //to be added to scheduling structures the queue must be in a started state,
                // have packets to send, and have been previously paused
                if (pPeerTidQueue->TxQueueState == TX_QUEUE_STARTED &&
                    pPeerTidQueue->BacklogCount > 0                 &&
                    prevPauseState                                     )
                {
                    //add to scheduling structure
                    if (m_CGlobalTxQueueList.AddTxQueue(pPeerTidQueue) == NDIS_STATUS_SUCCESS)
                    {
                        m_CurActiveBacklog += pPeerTidQueue->BacklogCount;
                    }
                    else
                    {
                        WFCInfo("[txmgr]TxQueue wasn't added to the global queue list");
                    }
                    //no need to check return code as the return code checked above is sufficient to update the
                    // backlog count.
                    AddTxQueueToAcEntry(pPeerTidQueue);
                }
            }
        }
        else
        {
            WFCTrace("[txmgr]ExTid:%x not in ExTidBitmask", i);
        }
    }

    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CTxMgr::ResumePortEntry(
    _In_ PWFC_TX_ENTRY pTxEntry,
    _In_ UINT32 IdBitmask,
    _In_ WDI_TX_PAUSE_REASON TxRestartReason)
    /*++
    called with txmgr lock held
    --*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_TX_QUEUE pTxQueue = NULL;
    UINT32 prevPauseState = 0;
    int i;

    TraceEntry();

    WFCInfo("[txmgr]Resuming [PortId:%x][IdBitmask:%x][RestartReason:%x] Current state:%x",
        pTxEntry->PortId,
        IdBitmask,
        TxRestartReason,
        pTxEntry->TxEntryState);

    for (i = 0; IsValidId((WFC_TX_QUEUE_ID)i); i++)
    {
        //If this tid is in the bitmask
        if (IdBitmask & (1 << i))
        {
            prevPauseState = pTxEntry->TidPauseState[i];
            pTxEntry->TidPauseState[i] = (WDI_TX_PAUSE_REASON)(prevPauseState & ~TxRestartReason);

            pTxQueue = GetTxQueue(pTxEntry, (WFC_TX_QUEUE_ID)i);

            if (pTxQueue != NULL)
            {
                //to unpause the TidPauseState must not have any bit set
                if (pTxEntry->TidPauseState[i] == 0)
                {
                    CLEAR_FLAG(pTxQueue->TxQueueState, TX_QUEUE_PAUSED);
                }

                //to be added to scheduling structures the queue must be in a started state,
                // have packets to send, and have been previously paused
                if (pTxQueue->TxQueueState == TX_QUEUE_STARTED &&
                    pTxQueue->BacklogCount > 0                 &&
                    prevPauseState                                )
                {
                    //add to scheduling structure
                    if (m_CGlobalTxQueueList.AddTxQueue(pTxQueue) == NDIS_STATUS_SUCCESS)
                    {
                        m_CurActiveBacklog += pTxQueue->BacklogCount;
                    }
                    else
                    {
                        WFCInfo("[txmgr]TxQueue wasn't added to the global queue list");
                    }
                    //no need to check return code as the return code checked above is sufficient to update the
                    // backlog count.
                    AddTxQueueToAcEntry(pTxQueue);
                }
            }
        }
        else
        {
            WFCTrace("[txmgr]Id:%x not in IdBitmask", i);
        }
    }

    TraceExit(ndisStatus);
    return ndisStatus;
}

void CTxMgr::TxSendRestartInd(
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ UINT32 ExTidBitmask,
    _In_ WDI_TX_PAUSE_REASON TxRestartReason)
/*++
    TxMgr will resume appropriate TxQueues and TxEntries
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    PWFC_TX_ENTRY pTxEntry = NULL;

    TraceEntry();

    WFCInfo("[txmgr]SendRestartInd [PortId:%x][PeerId:%x][TidBitmask:%x][RestartReason:%x]",
        PortId,
        PeerId,
        ExTidBitmask,
        TxRestartReason);

    if (!IsValidPauseReason(TxRestartReason))
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("[txmgr]Invalid RestartReason");
        goto exit;
    }

    if (m_QueueingMode == PORT_QUEUEING &&
        PeerId != WDI_PEER_ANY            )
    {
        WFCInfo("[txmgr]Ignoring peer restart due to PortQueueing mode");
        goto exit;
    }

    {
        CAutoLock lock(m_TxMgrLock);

        //specific peer
        if (PortId != WDI_PORT_ANY &&
            PeerId != WDI_PEER_ANY    )
        {
            //specific port and specific peer
            pTxEntry = m_TxEntryTable.GetTxEntry(PeerId);
            if (pTxEntry == NULL)
            {
                WFCError("[txmgr][PeerId:%x] not found",
                    PeerId);
                ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
                goto exit;
            }
            ndisStatus = ResumePeerEntry(pTxEntry, ExTidBitmask, TxRestartReason);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCError("[txmgr]Error resuming [PeerId:%x]", pTxEntry->PeerId);
                goto exit;
            }
        }
        else if (PortId != WDI_PORT_ANY &&
                 PeerId == WDI_PEER_ANY    )
        {
            if (m_QueueingMode == PORT_QUEUEING)
            {
                //specific port
                pTxEntry = m_TxEntryTable.GetTxEntry(PortId);
                if (pTxEntry == NULL)
                {
                    WFCError("[txmgr][PortId:%x] not found",
                        PortId);
                    ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
                    goto exit;
                }

                ndisStatus = ResumePortEntry(pTxEntry,
                    ExTidBitmaskToTxQueueIdBitmask(ExTidBitmask),
                    TxRestartReason);

                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    WFCError("[txmgr]Error resuming [PortId:%x]", pTxEntry->PortId);
                    goto exit;
                }
            }
            else
            {
                //all peers on specific port
                UINT Index = 0;

                pTxEntry = m_TxEntryTable.GetNextPortPeerEntry(PortId, &Index);
                while (pTxEntry != NULL)
                {
                    ndisStatus = ResumePeerEntry(pTxEntry, ExTidBitmask, TxRestartReason);
                    if (ndisStatus != NDIS_STATUS_SUCCESS)
                    {
                        WFCError("[txmgr]Error resuming [PeerId:%x]", pTxEntry->PeerId);
                        //TODO continue?
                    }

                    pTxEntry = m_TxEntryTable.GetNextPortPeerEntry(PortId, &Index);
                }
            }
        }
        else if (PortId == WDI_PORT_ANY &&
                 PeerId == WDI_PEER_ANY    )
        {
            //all peers on all ports
            UINT Index = 0;

            pTxEntry = m_TxEntryTable.GetNextTxEntry(&Index);
            while (pTxEntry != NULL)
            {
                if (m_QueueingMode == PORT_QUEUEING)
                {
                    ndisStatus = ResumePortEntry(pTxEntry,
                        ExTidBitmaskToTxQueueIdBitmask(ExTidBitmask),
                        TxRestartReason);
                }
                else
                {
                    ndisStatus = ResumePeerEntry(pTxEntry, ExTidBitmask, TxRestartReason);
                }
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    WFCError("[txmgr]Error resuming [TxId:%x]", pTxEntry->TxId);
                    //TODO continue?
                }

                pTxEntry = m_TxEntryTable.GetNextTxEntry(&Index);
            }
        }

        //subtract a pseudorandom number in interval [0,IterationPeriod)
        if (m_ServiceCounter >= WFC_DATAPATH_CONSTANT::IterationPeriod)
        {
            m_ServiceCounter -= CSystem::get_CurrentTime() % WFC_DATAPATH_CONSTANT::IterationPeriod;
        }
    }

    //take oppportunity to service queues
    ServiceQueues();
exit:
    TraceExit(ndisStatus);
}

void CTxMgr::TxReleaseFramesInd(
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ UINT32 ExTidBitmask,
    _In_ UINT8 MaxFrameCount,
    _In_ UINT16 Credit,
    _Out_ PNET_BUFFER_LIST *ppNBL)
/*++
    TxReleaseFrames is only permitted when peer_queueing.
    wildcards for PortId or PeerId are invalid
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    NblChain chainToRelease;
    PNET_BUFFER_LIST pCurNbl = NULL;

    PWFC_TX_ENTRY pCurTxEntry = NULL;
    PWFC_TX_QUEUE pCurTxQueue = NULL;

    PWFC_FRAME pCurWfcFrame = NULL;
    PWDI_FRAME_METADATA pCurFrameMeta = NULL;
    PLIST_ENTRY pListEntry = NULL;

    UINT16 curReleasedCost = 0;
    UINT16 lastDequeuedSeqNo = 0;

    UINT16 prevPeerBacklogCount = 0;

    BOOLEAN bIsPeerBacklogNotifNeeded = FALSE;

    BOOLEAN bCountCredits = (Credit == WDI_TX_CREDIT_INVALID) ? FALSE : TRUE;
    BOOLEAN bCountFrames = (MaxFrameCount == WDI_TX_MAX_FRAME_COUNT_INVALID) ? FALSE : TRUE;
    BOOLEAN bCheckReplaySeqNo = FALSE;
    //bIgnoreLimit is used to bypass the limit check in the event of replayed frames with 
    // the same SeqCtl (since they need to be be released together)
    BOOLEAN bIgnoreLimit = FALSE;

    UINT16 i = 0;

    TraceEntry();

    WFCInfo("[txmgr]TxReleaseFramesInd [PortId:0x%x][PeerId:0x%x][TidBitmask:0x%x][MaxFrameCount=0x%x][Credit=0x%x]",
        PortId,
        PeerId,
        ExTidBitmask,
        MaxFrameCount,
        Credit);

    if (ppNBL == NULL)
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("[txmgr]TxReleaseFramesInd exiting: ppNBL is null");
        goto exit;
    }

    //Only permitted in peer queueing
    if (m_QueueingMode == PORT_QUEUEING)
    {
        ndisStatus = NDIS_STATUS_INVALID_PORT_STATE;
        WFCError("[txmgr]TxReleaseFramesInd exiting: Indication not valid in PORT_QUEUEING mode");
        goto exit;
    }

    if (PortId == WDI_PORT_ANY ||
        PeerId == WDI_PEER_ANY    )
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("[txmgr]TxReleaseFramesInd exiting: Both PortId and PeerId must not be wildcards");
        goto exit;
    }

    if (ExTidBitmask == 0)
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("[txmgr]TxReleaseFramesInd exiting: ExTidBitmask must not be zero");
        goto exit;
    }
    //Now get the peer.  then get the queues.(in priority order?)
    { //m_TxMgrLock
        CAutoLock lock(m_TxMgrLock);

        //find the peer.
        pCurTxEntry = m_TxEntryTable.GetTxEntry(GetTxId(PortId, PeerId));
        if (pCurTxEntry == NULL)
        {
            WFCInfo("[txmgr][TxId:0x%x][PortId:0x%x][PeerId:0x%x] not found in peer table",
                GetTxId(PortId, PeerId),
                PortId,
                PeerId);
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            goto exit;
        }

        prevPeerBacklogCount = pCurTxEntry->BacklogCount;

        //loop through the TIDs in priority order
        //Stop if it's clear that no more frames can be dequeued
        for (i = 0; (!bCountFrames || chainToRelease.GetCount() < MaxFrameCount) && (!bCountCredits || curReleasedCost < Credit) && IsValidId(MapInversePriToTid(i)); i++)
        {
            bIgnoreLimit = FALSE;
            bCheckReplaySeqNo = FALSE;
            lastDequeuedSeqNo = 0;
            
            WDI_EXTENDED_TID curTid = MapInversePriToTid(i);
            //is this TID in the bitmask?
            if (~ExTidBitmask & (1 << curTid))
            {
                continue;
            }

            if (pCurTxEntry->TxEntryState != TX_ENTRY_STARTED &&
                !IsIhvTid(curTid)                                )
            {
                WFCInfo("[txmgr][TxId:0x%x] not in active state. TxEntry state = %x",
                    pCurTxEntry->TxId,
                    pCurTxEntry->TxEntryState);
                continue;
            }

            pCurTxQueue = GetTxQueue(pCurTxEntry, ExTidToId(curTid));

            if (pCurTxQueue == NULL)
            {
                WFCInfo("[txmgr]TxQueue:[PortId:0x%x][PeerId:0x%x][TID:%u] not present",
                    PortId,
                    PeerId,
                    curTid);
                continue;
            }
            //must be paused
            if (!TEST_FLAG(pCurTxQueue->TxQueueState, TX_QUEUE_PAUSED))
            {
                WFCInfo("[txmgr]TxQueue:[PortId:0x%x][PeerId:0x%x][TID:%u] not paused",
                    PortId,
                    PeerId,
                    curTid);
                continue;
            }
            //if ps paused must the frames must be in order
            if (TEST_FLAG(pCurTxQueue->TxQueueState, TX_QUEUE_PS_PAUSED)                     &&
                (pCurTxQueue->TransferPendingCount != 0 || pCurTxQueue->TxPendingCount != 0)    )
            {
                WFCInfo("[txmgr]TxQueue:[PortId:0x%x][PeerId:0x%x][TID:%u] not in order",
                    PortId,
                    PeerId,
                    curTid);
                continue;
            }
            //must have backlog
            if (pCurTxQueue->BacklogCount == 0          ||
                IsListEmpty(&pCurTxQueue->BacklogQueue)    )
            {
                WFCInfo("[txmgr]TxQueue:[PortId:0x%x][PeerId:0x%x][TID:%u] does not have backlog.",
                    PortId,
                    PeerId,
                    curTid);
                continue;
            }

            WFCInfo("[txmgr]Releasing frames from TxQueue:[PortId:0x%x][PeerId:0x%x][TID:%u][BacklogCount:%u]",
                PortId,
                PeerId,
                curTid,
                pCurTxQueue->BacklogCount);

            //release frames
            do
            {
                pListEntry = RemoveHeadList(&pCurTxQueue->BacklogQueue);

                pCurFrameMeta = (PWDI_FRAME_METADATA)CONTAINING_RECORD(pListEntry, WDI_FRAME_METADATA, Linkage);
                pCurWfcFrame = WFC_FRAME_FROM_WDI_FRAME(pCurFrameMeta);
                pCurNbl = pCurFrameMeta->pNBL;

                ASSERT(pCurNbl);

                WFCTrace("[txmgr]HOL frame:[Cost:%x]", pCurFrameMeta->u.txMetaData.TxCost);

                //check if both last frame and cur frame were replayed
                // and SeqCtl matches
                // if the replay data was not set then dequeue occurs normally.
                if (bCheckReplaySeqNo                                         &&
                    pCurWfcFrame->u.Tx.bIsReplayDataSet                       &&
                    (lastDequeuedSeqNo == pCurFrameMeta->u.txMetaData.SeqCtl)    )
                {
                    bIgnoreLimit = TRUE;
                    WFCInfo("[txmgr]Replayed frame with same SeqCtl as previous released frame.  Ignoring release limits for this frame.");
                }
                else
                {
                    bIgnoreLimit = FALSE;
                }

                if (((bCountCredits) && (!bIgnoreLimit) && (curReleasedCost + pCurFrameMeta->u.txMetaData.TxCost > Credit)) ||
                    ((bCountFrames) && (!bIgnoreLimit) && (chainToRelease.GetCount() + 1 > MaxFrameCount))                     )
                {
                    WFCInfo("[txmgr]Unable to release another frame from this queue.");
                    InsertHeadList(&pCurTxQueue->BacklogQueue, pListEntry);
                    break;
                }
                else
                {
                    //add to list
                    chainToRelease.AddTail(pCurNbl);

                    //check for replayed
                    if (pCurWfcFrame->u.Tx.bIsReplayDataSet)
                    {
                        lastDequeuedSeqNo = pCurFrameMeta->u.txMetaData.SeqCtl;
                        bCheckReplaySeqNo = TRUE;
                    }
                    else
                    {
                        bCheckReplaySeqNo = FALSE;
                    }

                    curReleasedCost += pCurFrameMeta->u.txMetaData.TxCost;

                    pCurTxQueue->BacklogCount--;
                    pCurTxQueue->TransferPendingCount++;

                    pCurTxEntry->BacklogCount--;
                    pCurTxEntry->TransferPendingCount++;
                    //To reach this point the queue must not have been schedulable.  Do NOT decrement m_CurActiveBacklog

                    m_CurOutstandingTransfers++;
                }

            } while (!IsListEmpty(&pCurTxQueue->BacklogQueue));
        }

        //if the TxEntry is paused and the backlog went from non-zero to zero
        if (prevPeerBacklogCount > 0       &&
            pCurTxEntry->BacklogCount == 0 &&
            IsPeerPaused(pCurTxEntry)         )
        {
            bIsPeerBacklogNotifNeeded = TRUE;
        }

    } //m_TxMgrLock

    WFCTrace("[txmgr]Released %d frames from [PortId:0x%x][PeerId:0x%x]",
        chainToRelease.GetCount(),
        PortId,
        PeerId);

    if (bIsPeerBacklogNotifNeeded)
    {
        WFCTrace("[txmgr]Calling TxPeerBacklog for [PortId:0x%x][PeerId:0x%x][Backlog:%x]",
            PortId,
            PeerId,
            FALSE);

        m_pAdapter->m_MiniportDataHandlers.TxPeerBacklogHandler(
            m_pAdapter->GetMiniportTalTxRxContext(),
            PortId,
            PeerId,
            FALSE);
    }

exit:
    if (ppNBL)
    {
        *ppNBL = chainToRelease.GetHead();
    }
    TraceExit(ndisStatus);
}

void CTxMgr::CompleteIhvNbl(
    _In_ PNET_BUFFER_LIST pNBL)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    PNET_BUFFER_LIST pCurNbl = NULL;
    PNET_BUFFER_LIST pNextNbl = NULL;

    PWDI_FRAME_METADATA pWifiFrameMeta = NULL;
    PWFC_FRAME pWfcFrame = NULL;

    WDI_TX_FRAME_STATUS tempFrameStatus = WDI_TxFrameStatus_Ok;

    TraceEntry();

    pCurNbl = pNBL;

    TxTargetDescDeinit(pCurNbl);

    while (pCurNbl != NULL)
    {
        pNextNbl = NET_BUFFER_LIST_NEXT_NBL(pCurNbl);
        NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = NULL;

        pWifiFrameMeta = GetWiFiFrameMetadataFromNbl(pCurNbl);

        ASSERT(pWifiFrameMeta != NULL);

        pWfcFrame = (PWFC_FRAME)WFC_FRAME_FROM_WDI_FRAME(pWifiFrameMeta);

        ASSERT(pWfcFrame);

        //if necessary swap clone and orig NBs and free clonedNBL
        if (pWfcFrame->u.Tx.pCloneNbl)
        {
            WFCTrace("[txmgr]Cleaning up cloned NBL");
            CleanupCloneNbl(pCurNbl, pWfcFrame->u.Tx.pCloneNbl, pWfcFrame);
        }

        //TODO fix the status
        tempFrameStatus = (pWfcFrame->u.Tx.bIsSendStatusSet) ? 
            pWfcFrame->u.Tx.SendStatus : pWfcFrame->u.Tx.TransferStatus;

        //free frame id
        m_pAdapter->m_FrameIdTable.FreeAndUnmapFrameId(pWfcFrame);
        //Free metadata
        m_pAdapter->FreeWfcFrame(pWfcFrame);

        pCurNbl->MiniportReserved[0] = NULL;

        //TODO batch calls to TalSendComplete
        //TODO fix frame status.
        m_pAdapter->m_MiniportDataHandlers.TxTalSendCompleteHandler(m_pAdapter->GetMiniportTalTxRxContext(),
                                                                    pCurNbl,
                                                                    tempFrameStatus);

        pCurNbl = pNextNbl;
    }

    TraceExit(ndisStatus);
}

//Allow deferring send completion to a workitem to prevent operations like miniport pause
// and peer delete to cause a huge number of queued frames to be send completed at dispatch.
void CTxMgr::CompleteNdisNbl(
    _In_ PNET_BUFFER_LIST pNBL,
    _In_ BOOLEAN bDeferSendCompletion)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_HANDLE hWorkitem = NULL;
    PWFC_TX_DEFERRED_SEND_COMPLETION_CONTEXT pDeferSendCompleteContext = NULL;
    BOOLEAN bWasCompletionDeferred = FALSE;
    TraceEntry();

    TxTargetDescDeinit(pNBL);

    if (bDeferSendCompletion)
    {
        pDeferSendCompleteContext = new WFC_TX_DEFERRED_SEND_COMPLETION_CONTEXT;
        if (pDeferSendCompleteContext == NULL)
        {
            WFCError("[txmgr]Unable to allocate deferred send complete context");
            ndisStatus = NDIS_STATUS_RESOURCES;
            goto exit;
        }
        pDeferSendCompleteContext->pAdapter = m_pAdapter;
        pDeferSendCompleteContext->pNbl = pNBL;

        hWorkitem = NdisAllocateIoWorkItem(m_pAdapter->GetMiniportAdapterHandle());
        if (hWorkitem == NULL)
        {
            WFCError("[txmgr]Unable to allocate work item for send complete deferral.");
            ndisStatus = NDIS_STATUS_RESOURCES;
            goto exit;
        }

        NdisQueueIoWorkItem(hWorkitem,
                            TxMgrDeferSendCompletion,
                            pDeferSendCompleteContext);

        bWasCompletionDeferred = TRUE;
        WFCInfo("[txmgr]Send completion deferred.");
    }

exit:
    if (!bWasCompletionDeferred)
    {
        m_pAdapter->SendCompleteNbl(pNBL);

        //perform any cleanup on deferral handles/allocations
        if (pDeferSendCompleteContext)
        {
            delete pDeferSendCompleteContext;
            pDeferSendCompleteContext = NULL;
        }
        if (hWorkitem)
        {
            NdisFreeIoWorkItem(hWorkitem);
            hWorkitem = NULL;
        }
    }

    //if the workitem is needed schedule the workitem
    if (m_TotalPrequeueCount > 0)
    {
        SchedulePrequeueWorkitem();
    }

    TraceExit(ndisStatus);
}

__inline NDIS_STATUS CTxMgr::InitializeIhvWfcFrame(
    _In_ PWFC_FRAME pWfcFrame,
    _In_ PNET_BUFFER_LIST pNBL,
    _In_ WDI_PORT_ID PortId,
    _In_ WDI_PEER_ID PeerId,
    _In_ WDI_EXTENDED_TID ExTid,
    _In_ BOOLEAN bIsUnicast,
    _In_ BOOLEAN bUseLegacyRates,
    _In_ UINT16 Ethertype,
    _In_ WDI_EXEMPTION_ACTION_TYPE ExemptionAction)
/*++
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    pWfcFrame->FrameType = TX_INJECTED_FRAME;
    pWfcFrame->u.Tx.pCloneNbl = NULL;
    pWfcFrame->u.Tx.bIsTransferStatusSet = FALSE;
    pWfcFrame->u.Tx.bIsSendStatusSet = FALSE;
    pWfcFrame->u.Tx.bIsReplayDataSet = FALSE;
    pWfcFrame->u.Tx.TransferStatus = WDI_TxFrameStatus_TransferCancelled;

    RtlZeroMemory(&pWfcFrame->WiFiFrameMetadata.u.txMetaData,sizeof(WDI_TX_METADATA));

    InitializeListHead(&pWfcFrame->WiFiFrameMetadata.Linkage);
    pWfcFrame->WiFiFrameMetadata.pNBL = pNBL;

    ndisStatus = m_pAdapter->m_FrameIdTable.AssignAndMapFrameId(pWfcFrame);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[txmgr]Unable to assign FrameId (0x%x)",ndisStatus);
        goto exit;
    }

    pWfcFrame->WiFiFrameMetadata.u.txMetaData.PortID = PortId;
    pWfcFrame->WiFiFrameMetadata.u.txMetaData.PeerID = PeerId;
    pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExTID = ExTid;
    pWfcFrame->WiFiFrameMetadata.u.txMetaData.IsUnicast = bIsUnicast;
    pWfcFrame->WiFiFrameMetadata.u.txMetaData.bAllowLegacyRates = bUseLegacyRates;
    pWfcFrame->WiFiFrameMetadata.u.txMetaData.Ethertype = Ethertype;

    //TODO update this based on port and capabilities
    pWfcFrame->WiFiFrameMetadata.u.txMetaData.bTxCompleteRequired = FALSE;
    pWfcFrame->WiFiFrameMetadata.u.txMetaData.ExemptionAction = ExemptionAction;

exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

void 
CTxMgr::SchedulePrequeueWorkitem()
{
    TraceEntry();
    if (InterlockedIncrement(&m_ProcessPendingFramesWorkitemScheduled) == 1)
    {
        WFCTrace("[txmgr]Queueing work item to process port queue");
        NdisQueueIoWorkItem(m_hSendCompleteWorkItem, TxMgrProcessPendingFrames, this);
    }

    TraceExit(0);
}

void
CTxMgr::ProcessPortQueuesWorkitemRoutine()
/*++
    WorkItem callback to try to process Port queue frames.
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    CPort* pPort = NULL;
    ULONG startIndex = 0;
    ULONG curIndex = 0;
    UINT i = 0;
    LONG curVal = 0;

    BOOLEAN bIsPrequeueFlushNeeded = FALSE;
    TraceEntry();

    do
    {
        curVal = m_ProcessPendingFramesWorkitemScheduled;
        NT_ASSERT(curVal > 0);

        WFCTrace("[txmgr]%ld triggers for the workitem occured", curVal);

        //start at the port after the last processed port
        startIndex = (m_LastProcessedPortIndex + 1) % m_pAdapter->c_MaxPorts;
        curIndex = startIndex;

        do
        {
            if (m_pAdapter->m_PortLockList[curIndex].Enter())
            {
                pPort = m_pAdapter->m_pPortList[curIndex];
                if (pPort != NULL)
                {
                    m_LastProcessedPortIndex = curIndex;
                    WFCInfo("[txmgr]Trying to dequeue port queued frames from PortId:0x%x.",
                        pPort->GetPortId());

                    ndisStatus = pPort->SendQueuedNetBufferLists();

                    if (ndisStatus != NDIS_STATUS_SUCCESS)
                    {
                        WFCInfo("[txmgr]Unable to empty port queue.");
                        m_pAdapter->m_PortLockList[curIndex].Leave();
                        break; //break out of inner do-while loop but not out of outer do-while loop
                    }
                }
                m_pAdapter->m_PortLockList[curIndex].Leave();
            }
            curIndex = (curIndex + 1) % m_pAdapter->c_MaxPorts;
        } while (curIndex != startIndex);

        //repeat the operation if more SendCompletes came during processing, freed TxTargetDescs 
        // but didn't schedule the workitem because it was already scheduled/running
        curVal = InterlockedAdd(&m_ProcessPendingFramesWorkitemScheduled, -curVal);

        if (curVal > 0)
        {
            //Sleep for 1 ms to allow other threads to get work done
            // the sleep will likely be longer than 1 ms due to thread quantum which is acceptable.
            NdisMSleep(1 * 1000);
        }
    } while (curVal != 0);

    WFCTrace("[txmgr]Finished handling send completions that occured during workitem processing.  Checking if forward progress can be made on remaining prequeued frames.");

    if (m_CurInitedTxDescriptors == 0 &&
        m_TotalPrequeueCount > 0         )
    { // TxPrequeueLock
        CAutoLock lock(GetPrequeueLock());

        { // m_TxTargetDescLock
            CAutoLock lock(m_TxTargetDescLock);
            { // m_TxTargetDescDeinitLock
                CAutoLock lock(m_TxTargetDescDeinitLock);

                if (m_CurInitedTxDescriptors == 0 &&
                    m_TotalPrequeueCount > 0         )
                {
                    //There are frames which are prequeued but the IHV doesn't have any TxTargetDescs in use
                    // So complete everything in the port prequeues since forward progress will not be made
                    bIsPrequeueFlushNeeded = TRUE;
                    WFCError("[txmgr] There are %ld frames prequeued but there are 0 oustanding TxTargetDescriptors.  Flushing prequeues.",
                        m_TotalPrequeueCount);
                }

            } // m_TxTargetDescDeinitLock

        } // m_TxTargetDescLock
    } // TxPrequeueLock

    if (bIsPrequeueFlushNeeded)
    {
        for (i = 0; i < m_pAdapter->c_MaxPorts; i++)
        {
            if (m_pAdapter->m_PortLockList[i].Enter())
            {
                pPort = m_pAdapter->m_pPortList[i];
                if (pPort != NULL)
                {
                    pPort = m_pAdapter->m_pPortList[i];

                    WFCInfo("[txmgr]Instructing PortId:0x%x to complete all frames in pending queue.",
                        pPort->GetPortId());

                    pPort->CompleteTxPortQueue();
                }
                m_pAdapter->m_PortLockList[i].Leave();
            }
        }
    }

    ServiceQueues();

    TraceExit(ndisStatus);
}

_Use_decl_annotations_
VOID TxMgrProcessPendingFrames(
    PVOID Context,
    NDIS_HANDLE NdisIoWorkItemHandle)
/*++
--*/
{
    CTxMgr * pTxMgr = reinterpret_cast<CTxMgr*>(Context);

    UNREFERENCED_PARAMETER(NdisIoWorkItemHandle);

    pTxMgr->ProcessPortQueuesWorkitemRoutine();
}

__inline void
CTxMgr::CleanupCloneNbl(
    _In_ PNET_BUFFER_LIST pOrigNbl,
    _In_ PNET_BUFFER_LIST pCloneNbl,
    _In_ PWFC_FRAME pWfcFrame)
{
    //original NB stored in clone NBL and clone NB stored in orig NBL
    PNET_BUFFER pOrigNb = NET_BUFFER_LIST_FIRST_NB(pCloneNbl);
    PNET_BUFFER pCloneNb = NET_BUFFER_LIST_FIRST_NB(pOrigNbl);

    //swap back to restore original state
    NET_BUFFER_LIST_FIRST_NB(pOrigNbl) = pOrigNb;
    NET_BUFFER_LIST_FIRST_NB(pCloneNbl) = pCloneNb;
    //free cloned NBL
    m_pAdapter->FreeCopiedNetBufferLists(pCloneNbl);

    pWfcFrame->u.Tx.pCloneNbl = NULL;
}

_Use_decl_annotations_
VOID TxMgrDeferSendCompletion(
    PVOID Context,
    NDIS_HANDLE NdisIoWorkItemHandle)
/*++
    Performs deferred 
--*/
{
    PWFC_TX_DEFERRED_SEND_COMPLETION_CONTEXT pSendCompleteContext = 
        reinterpret_cast<PWFC_TX_DEFERRED_SEND_COMPLETION_CONTEXT>(Context);
    NT_ASSERT(pSendCompleteContext);

    PNET_BUFFER_LIST pNbl = pSendCompleteContext->pNbl;
    CAdapter *pAdapter = pSendCompleteContext->pAdapter;

    NT_ASSERT(pNbl);
    NT_ASSERT(pAdapter);

    TraceEntry();

    WFCInfo("[txmgr]TxMgrDeferSendCompletion");

    pAdapter->SendCompleteNbl(pNbl);

    //clean up handle and allocation
    delete pSendCompleteContext;
    NdisFreeIoWorkItem(NdisIoWorkItemHandle);

    TraceExit(NDIS_STATUS_SUCCESS);
}

void CTxMgr::OnTimerCallback(
    _In_opt_ WFC_TIMER_CONTEXT TimerContext)
{
    UNREFERENCED_PARAMETER(TimerContext);

    CheckForOldFrames();
}

void CTxMgr::CheckForOldFrames()
/*++
    Called about every 60 seconds to check for frames which have been outstanding for
    more than 60 seconds (unless there is no activity on datapath and no inited txdescriptors)
    This is serialized by the event queue so the callback cannot occur at the same time as
    StopOldFrameDetectionTimer
--*/
{
    UINT32 numOldFramesFound = 0;
    bool bRequeueTimer = false;

    TraceEntry();

    //Call into the frame table with previous FrameNum
    numOldFramesFound = m_pAdapter->m_FrameIdTable.LogFramesWithOldFrameNum(m_PrevWifiFrameNumber);

    { //m_TxMgrLock
        CAutoLock lock(m_TxMgrLock);

        if (numOldFramesFound == 0)
        {
            if (m_bIsOldFrameDetected)
            {
                WFCInfo("[txmgr]Old frame(s) no longer outstanding.");
                LogOldFrameStateChangeTelemetryEvent(false);
            }
            m_bIsOldFrameDetected = false;
        }
        else if (!m_bIsOldFrameDetected)
        {
            LogOldFrameStateChangeTelemetryEvent(true);
            m_bIsOldFrameDetected = true;
        }

        //do not queue the timer if there have been no new frames (prev = cur)
        // and there are no inited txdescriptors
        if (m_PrevWifiFrameNumber == m_CurWifiFrameNumber &&
            m_CurInitedTxDescriptors == 0                    )
        {
            WFCTrace("[txmgr]Datapath quiet, not requeueing old frame detection timer.");
            m_bAllowOldFrameTimerQueueing = true;
        }
        else
        {
            bRequeueTimer = true;
        }

        WFCInfo("[txmgr][FramesSinceLastCheck:%llu][OldFramesFound:%lu][CurActiveBacklog:%lu][CurOutstandingTransfers:%llu][CurInitedTxDescriptors:%lu][LastScheduled[PortId:0x%x][PeerId:0x%x][TID:0x%x][QueueLength:%u]]",
            m_CurWifiFrameNumber-m_PrevWifiFrameNumber,
            numOldFramesFound,
            m_CurActiveBacklog,
            m_CurOutstandingTransfers,
            m_CurInitedTxDescriptors,
            m_ScheduledPortId,
            m_ScheduledPeerId,
            m_ScheduledExtTid,
            m_ScheduledQueueLength);

        m_PrevWifiFrameNumber = m_CurWifiFrameNumber;
    } //m_TxMgrLock

    if (bRequeueTimer)
    {
        QueueOldFrameDetectionTimer();
    }

    TraceExit(NDIS_STATUS_SUCCESS);
}

void CTxMgr::QueueOldFrameDetectionTimer()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    TraceEntry();

    if (m_pOldFrameDetectionTimerRegistrationContext)
    {
        ndisStatus = m_pAdapter->GetEventQueue()->StartTimer(
            m_pOldFrameDetectionTimerRegistrationContext,
            WFC_DATAPATH_CONSTANT::OldFrameDetectionPeriodMs);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            //this error is not returned to the caller since we want to ignore and continue
            //if this failure happens when called by the callback then m_bAllowOldFrameTimerQueueing
            // will already be false and queueing will not be attempted later.
            //if this failure happens when called by ProcessNBL then m_bAllowOldFrameTimerQueueing
            // will be set to false immediately after and queueing will not be attempted later.
            WFCError("[txmgr]Unable to queue old frame detection timer.");
        }
    }
    else
    {
        //see comment above for why error is masked to caller
        ndisStatus = NDIS_STATUS_FAILURE;
        WFCError("[txmgr]m_pOldFrameDetectionTimerRegistrationContext is NULL. Unable to queue timer.");
    }
    TraceExit(ndisStatus);
}

//This is serialized by the event queue
void CTxMgr::StopOldFrameDetectionTimer()
{
    TraceEntry();

    { // m_TxMgrLock
        CAutoLock lock(m_TxMgrLock);
        //prevent the timer from being queued after this point.
        //because this is serialized by the event queue the callback cannot be happening
        // at this time.
        m_bAllowOldFrameTimerQueueing = false;
    } // m_TxMgrLock

    if (m_OldFrameDetectionTimerToken != WFC_TIMER_REGISTRATION_TOKEN_INVALID)
    {
        m_pAdapter->GetEventQueue()->DeregisterTimeoutCallback(m_OldFrameDetectionTimerToken);
        m_OldFrameDetectionTimerToken = WFC_TIMER_REGISTRATION_TOKEN_INVALID;
        m_pOldFrameDetectionTimerRegistrationContext = NULL;
    }

    TraceExit(NDIS_STATUS_SUCCESS);
}

//Called inside m_TxMgrLock
void CTxMgr::LogOldFrameStateChangeTelemetryEvent(
    _In_ bool bOldFrameDetected)
{
    if (g_RegisteredWithTraceLogging)
    {
        TraceLoggingWrite(
            g_hProvider,
            "WdiOldFrameStateChange",
            TraceLoggingGuid(m_pAdapter->GetNetGuid(), "interfaceGuid"),
            TraceLoggingBool(bOldFrameDetected, "isOldFrameDetected"),
            TraceLoggingKeyword(MICROSOFT_KEYWORD_TELEMETRY));
    }
}

