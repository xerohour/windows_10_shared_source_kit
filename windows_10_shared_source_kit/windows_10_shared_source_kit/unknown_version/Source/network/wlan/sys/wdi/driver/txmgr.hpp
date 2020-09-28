/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    txmgr.hpp

Abstract:

    Header for the WDI TxMgr.

Environment:

    Kernel mode

Author:


--*/

#pragma once

#define TX_QUEUE_STARTED                     0x00000000
#define TX_ENTRY_STARTED                     TX_QUEUE_STARTED
#define TX_QUEUE_STOPPING                    0x00000001
#define TX_ENTRY_STOPPING                    TX_QUEUE_STOPPING
#define TX_QUEUE_STOPPED                     0x00000002
#define TX_ENTRY_STOPPED                     TX_QUEUE_STOPPED
#define TX_QUEUE_PAUSED                      0x00000004
#define TX_QUEUE_PS_PAUSED                   0x00000008
#define TX_ENTRY_PEER_ABORT_CONFIRM_PENDING  0x00000010
#define TX_ENTRY_PEER_DELETE_PENDING         0x00000020
#define TX_QUEUE_PEER_DELETE_PENDING         TX_ENTRY_PEER_DELETE_PENDING


typedef WDI_EXTENDED_TID WFC_TX_QUEUE_ID;

#define TX_QUEUE_ID_OS_BIT  0x00000001
#define TX_QUEUE_ID_IHV_BIT 0x00000002

typedef WDI_PEER_ID WFC_TX_ID;

#define WFC_TX_ID_ANY ((WFC_TX_ID)0xFFFF)

typedef enum _WFC_TX_QUEUEING_MODE {
    PEER_QUEUEING = 0,
    PORT_QUEUEING = 1,
} WFC_TX_QUEUEING_MODE;

// should nonQos TID be it's own AC?
typedef enum _WFC_ACCESS_CATEGORY {
    AC_BE = 0,
    AC_BK = 1,
    AC_VI = 2,
    AC_VO = 3,
    AC_PR0 = 4,
    AC_PR1 = 5,
    AC_PR2 = 6,
    AC_PR3 = 7,
    AC_MAX = 8 //Unused
} WFC_ACCESS_CATEGORY;

//higher number -> higher priority
//all ACs have a unique priority
__inline UINT16 MapAccessCategoryToPriority(
    _In_ WFC_ACCESS_CATEGORY Ac)
{
    switch(Ac)
    {
        case AC_BK:
            return 0;
        case AC_BE:
            return 1;
        case AC_VI:
            return 2;
        case AC_VO:
            return 3;
        case AC_PR0:
            return 4;
        case AC_PR1:
            return 5;
        case AC_PR2:
            return 6;
        case AC_PR3:
            return 7;
        default:
            return 0;
    }
}

__inline WFC_ACCESS_CATEGORY MapTidToAccessCategory(
    _In_ WDI_EXTENDED_TID Tid)
{
    switch(Tid)
    {
        case 1:
        case 2:
        case 16: //non-QoS
        case 17: //IHV TID0
            return AC_BK;
        case 0:
        case 3:
        case 18: //IHV TID1
            return AC_BE;
        case 4:
        case 5:
        case 19: //IHV TID2
            return AC_VI;
        case 6:
        case 7:
        case 20: //IHV TID3
            return AC_VO;
        case 21: //IHV TID4
            return AC_PR0;
        case 22: //IHV TID5
            return AC_PR1;
        case 23: //IHV TID6
            return AC_PR2;
        case 24: //IHV TID7
            return AC_PR3;
        default:
            return AC_BK;
    }
}

//returns TIDs which decrease in priority.
// when multiple TIDs map to same priority 
// return higher value TID with exception that 16 is always lowest
// returns an invalid TID when i > #valid TIDs (17)
__inline WDI_EXTENDED_TID MapInversePriToTid(
    _In_ UINT16 i)
{
    switch (i)
    {
        case 0:
            return 24; //AC_PR3
        case 1:
            return 23; //AC_PR2
        case 2:
            return 22; //AC_PR1
        case 3:
            return 21; //AC_PR0
        case 4:
            return 20; //AC_VO
        case 5:
            return 7; //AC_V0
        case 6:
            return 6; //AC_V0
        case 7:
            return 19; //AC_VI
        case 8:
            return 5; //AC_VI
        case 9:
            return 4; //AC_VI
        case 10:
            return 18; //AC_BE
        case 11:
            return 3; //AC_BE
        case 12:
            return 0; //AC_BE
        case 13:
            return 17; //AC_BK
        case 14:
            return 2; //AC_BK
        case 15:
            return 1; //AC_BK
        case 16:
            return 16; //AC_BK
        default:
            return (WDI_EXTENDED_TID)-1;
    }
}

//returns true if TID is in valid range [0-24]
__inline BOOLEAN IsValidTid(
    _In_ WDI_EXTENDED_TID Tid)
{
    return (Tid <= 24) ? TRUE : FALSE;
}

__inline BOOLEAN IsIhvTid(
    _In_ WDI_EXTENDED_TID Tid)
{
    return (Tid > 16) ? TRUE : FALSE;
}
typedef struct _WFC_TX_QUEUE {

    LIST_ENTRY          BacklogQueue; //Queue of backlogged NBLs

    LIST_ENTRY          GlobalTxQueueListEntry;
    LIST_ENTRY          ACListEntry; // list entry for AC PeerTIDQueueList

    UINT16              BacklogCount; // #NBLs in backlog queue;

    //#NBLS dequeued but not TransferCompleted (for determining queue in order)
    UINT16              TransferPendingCount;
    //#NBLS that have been TransferCompleted but not TxCompleted (and req. TxCompleted)
    UINT16              TxPendingCount;

    BOOLEAN             bInUse; //false == free, true == in use.

    UINT32              TxQueueState;

    WDI_EXTENDED_TID   Tid;

    WDI_PORT_ID        PortId;
    WDI_PEER_ID        PeerId;

    WFC_ACCESS_CATEGORY Ac;

} WFC_TX_QUEUE, *PWFC_TX_QUEUE;

typedef struct _WFC_TX_ENTRY {
    BOOLEAN           bInUse; //false == free, true == in use

    WFC_TX_ID         TxId;  // equal PortId when PortQueueing, PeerId otherwise.

    WDI_PORT_ID      PortId;
    WDI_PEER_ID      PeerId;

    UINT32            TxEntryState;

    WDI_MAC_ADDRESS  MacAddr;

    //Bitmask of extended TIDs which are in order but need TX_QUEUE_IN_ORDER call
    UINT32            InOrderNotifPendingBitmask;

    UINT16            BacklogCount; // #NBLs in all PeerTid backlog queues
    UINT16            TransferPendingCount;
    UINT16            TxPendingCount;

    //Array of pause reasons per TID.  This must be the last of the WFC_TX_ENTRY that should be zeroed on TxEntry removal
    WDI_TX_PAUSE_REASON TidPauseState[WFC_DATAPATH_CONSTANT::NumExtendedTids];

    //MaxQueueCount and ppTxQueue must be the last fields in this struct
    UINT32            MaxQueueCount;
    _Field_size_(MaxQueueCount) PWFC_TX_QUEUE     *ppTxQueue;

} WFC_TX_ENTRY, *PWFC_TX_ENTRY;

typedef struct _WFC_AC_ENTRY {
    LIST_ENTRY m_TxQueueList; // list of unpaused and backlogged TxQueues in this AC

    UINT64 TxQueueCount; // used to maintain count of TxQueues in this list

    WFC_ACCESS_CATEGORY Ac;

    UINT16 Priority;

} WFC_AC_ENTRY, *PWFC_AC_ENTRY;


typedef struct _WFC_TX_ABORT_REQUEST {
    LIST_ENTRY   AbortListEntry;

    WDI_PORT_ID PortId;
    WDI_PEER_ID PeerId;
} WFC_TX_ABORT_REQUEST, *PWFC_TX_ABORT_REQUEST;

typedef struct _WFC_TX_PEER_BACKLOG_NOTIFICATION {
    LIST_ENTRY ListEntry;

    WDI_PORT_ID PortId;
    WDI_PEER_ID PeerId;
    BOOLEAN      Backlogged;
} WFC_TX_PEER_BACKLOG_NOTIFICATION, *PWFC_TX_PEER_BACKLOG_NOTIFICATION;

typedef struct _WFC_TX_DEFERRED_SEND_COMPLETION_CONTEXT {
    CAdapter *       pAdapter;
    PNET_BUFFER_LIST pNbl;
} WFC_TX_DEFERRED_SEND_COMPLETION_CONTEXT, *PWFC_TX_DEFERRED_SEND_COMPLETION_CONTEXT;

typedef enum _WFC_TX_STOP_REASON {
    TxStopReasonControlPath = 1,  //StopCancel only non IHV TIDs
    TxStopReasonPeerDelete = 2,   //StopCancel all TIDs and set TxEntry state to TX_ENTRY_PEER_DELETE_PENDING
} WFC_TX_STOP_REASON;

// hash table with linear probing of TxIds.
class CTxEntryTable
{
public:
    CTxEntryTable();

    ~CTxEntryTable();

    NDIS_STATUS Initialize(
        _In_ CAdapter * pAdapter,
        _In_ UINT8 MaxNumberOfEntries,
        _In_ WFC_TX_QUEUEING_MODE QueueingMode);

    NDIS_STATUS InitializeTxEntry(
        _In_ WFC_TX_ID TxId,
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId,
        _In_ PWDI_MAC_ADDRESS pMacAddr);

    NDIS_STATUS RemoveTxEntry(
        _In_ WFC_TX_ID TxId);

    PWFC_TX_ENTRY GetTxEntry(
        _In_ WFC_TX_ID TxId);

    PWFC_TX_ENTRY GetNextPortPeerEntry(
        _In_ WDI_PORT_ID PortId,
        _Inout_ UINT *pIndex);

    PWFC_TX_ENTRY GetNextTxEntry(
        _Inout_ UINT *pIndex);

private:
    UINT m_TableSize;
    
    UINT m_NumEntries;

    WFC_TX_ENTRY* m_pTxEntryTable;

    //sets bInUse and TxId and returns pointer to TxEntry
    PWFC_TX_ENTRY CTxEntryTable::AssignTxEntry(
        _In_ WFC_TX_ID TxId);
    //
    WFC_TX_QUEUEING_MODE m_QueueingMode;

    //needed for version check so peers can come up as paused if > WDI_VERSION_0_90
    CAdapter * m_pAdapter;

    //TODO find a better hash funtion.
    __inline UINT hash_func(
        _In_ WFC_TX_ID TxId)
    {
        return TxId % m_TableSize;
    }
};

//list of AC_Entries which is used for priority scheduling.
//only created if PeerQueueing
class CACList
{
public:
    CACList();

    ~CACList();

    NDIS_STATUS AddTxQueue(
        _In_ PWFC_TX_QUEUE pTxQueue);

    NDIS_STATUS RemoveTxQueue(
        _In_ PWFC_TX_QUEUE pTxQueue);

    //returns NULL if no ACEntries have TxQueues
    //returns pointer to first TXQueue in the list from the highest priority AC
    //the TxQueue will be moved from the head of the list to tail.
    PWFC_TX_QUEUE GetTxQueueFromMaxAC();

private:
    static const UINT32 c_NumAcEntries = AC_MAX;
    WFC_AC_ENTRY m_ACList[c_NumAcEntries]; //indexed by Priority

    UINT32 m_TopIndex;

    void RemoveAllEntries();
};

class CGlobalTxQueueList {

public:
    CGlobalTxQueueList();
    ~CGlobalTxQueueList();

    NDIS_STATUS AddTxQueue(
        _In_ PWFC_TX_QUEUE pTxQueue);

    NDIS_STATUS RemoveTxQueue(
        _In_ PWFC_TX_QUEUE pTxQueue);

    PWFC_TX_QUEUE GetFirstTxQueue();

    NDIS_STATUS MoveTxQueueToTail(
        _In_ PWFC_TX_QUEUE pTxQueue);

private:
    UINT64 m_TxQueueCount; // used to maintain count of TxQueues in this list
    LIST_ENTRY m_TxQueueList; // list of unpaused and backlogged TxQueues

    void RemoveAllEntries();
};

class CTxMgr : ITimerCallback {

public:
    CTxMgr();
    ~CTxMgr();

    NDIS_STATUS Initialize(
        _In_ CAdapter* pAdapter,
        _In_ PWFC_DATAPATH_CAPABILITIES pDatapathCapabilities,
        _In_ UINT8 MaxNumPorts);

    NDIS_STATUS Deinitialize(
        );

    NDIS_STATUS ProcessNBL(
        _In_ PNET_BUFFER_LIST pNBL,
        _Out_ PBOOLEAN pbTargetDescResouces);

    NDIS_STATUS StopCancelSend(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId,
        _In_ WFC_TX_STOP_REASON TxStopReason);

    NDIS_STATUS RestartSend(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId);

    NDIS_STATUS AddPeerId(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId,
        _In_ PWDI_MAC_ADDRESS pPeerAddr);

    NDIS_STATUS PeerDeleteBegin(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId);

    NDIS_STATUS AddPortId(
        _In_ WDI_PORT_ID PortId);

    NDIS_STATUS RemovePortId(
        _In_ WDI_PORT_ID PortId);

    NDIS_STATUS TxTargetDescInit(
        _In_ PNET_BUFFER_LIST pNBL);

    NDIS_STATUS TxTargetDescDeinit(
        _In_ PNET_BUFFER_LIST pNBL);

    NDIS_STATUS Reset();

    NDIS_STATUS InitializeTxWfcFrame(
        _In_ PNET_BUFFER_LIST pNbl,
        _In_ WDI_PORT_ID PortId,
        _In_ BOOLEAN IsKeyConfigured);

    // Indications

    void TxDequeueInd(
        _In_ UINT32 Quantum,
        _In_ UINT8 MaxFrameCount,
        _In_ UINT16 Credit,
        _Out_ PNET_BUFFER_LIST *ppNBL);

    void TxTransferCompleteInd(
        _In_ WDI_TX_FRAME_STATUS TxFrameStatus,
        _In_ PNET_BUFFER_LIST pNBL);

    void TxSendCompleteInd(
        _In_ WDI_TX_FRAME_STATUS TxFrameStatus,
        _In_ UINT16 NumCompletedSends,
        _In_reads_(NumCompletedSends) WDI_FRAME_ID* TxFrameIdList,
        _In_reads_opt_(NumCompletedSends) WDI_TX_COMPLETE_DATA* TxCompleteDataList);

    void TxQueryRaTidStateInd(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId,
        _In_ WDI_EXTENDED_TID ExTid,
        _Out_ NDIS_STATUS* pQueueStatus,
        _Out_ UINT16* pQueueLength);

    void TxSendPauseInd(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId,
        _In_ UINT32 ExTidBitmask,
        _In_ WDI_TX_PAUSE_REASON TxPauseReason);

    void TxSendRestartInd(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId,
        _In_ UINT32 ExTidBitmask,
        _In_ WDI_TX_PAUSE_REASON TxRestartReason);

    void TxReleaseFramesInd(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId,
        _In_ UINT32 ExTidBitmask,
        _In_ UINT8 MaxFrameCount,
        _In_ UINT16 Credit,
        _Out_ PNET_BUFFER_LIST *ppNBL);

    void TxAcParamsUpdateInd(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId,
        _In_ UINT8* WmmIe);

    void TxInjectIhvFrame(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId,
        _In_ WDI_EXTENDED_TID ExTid,
        _In_ PNET_BUFFER_LIST pNBL,
        _In_ BOOLEAN bIsUnicast,
        _In_ BOOLEAN bUseLegacyRates,
        _In_ UINT16 Ethertype,
        _In_ WDI_EXEMPTION_ACTION_TYPE ExemptionAction);

    void TxAbortConfirm(
        _In_ BOOLEAN NotifyInitialConfirmation);

    void SetMaxOutstandingTransfers(
        _In_ UINT16 MaxOutstandingTransfers);

    void ServiceQueues();

    void SchedulePrequeueWorkitem();

    void ProcessPortQueuesWorkitemRoutine();

    void OnTimerCallback(
        _In_opt_ WFC_TIMER_CONTEXT TimerContext);

    void StopOldFrameDetectionTimer();

    __inline void CleanupCloneNbl(
        _In_ PNET_BUFFER_LIST pOrigNbl,
        _In_ PNET_BUFFER_LIST pCloneNbl,
        _In_ PWFC_FRAME pWfcFrame);

    CNdisSpinLock* GetPrequeueLock()
    {
        return m_TxPrequeueLock;
    }

    volatile long m_TotalPrequeueCount; //sum of all frames in TxPortQueues.  only modified in m_TxPrequeueLock.
private:

    CAdapter* m_pAdapter;

    LONG     m_SendThreadActive; //use interlocked functions to check

    //No lock may be taken when holding m_TxMgrLock
    //No lock may be taken when holding m_TxTargetDescLock except for m_TxTargetDescDeinitLock
    //A TxTargetDescLock may be taken when holding m_TxPrequeueLock (ProcessPortQueuesWorkitemRoutine)
    CNdisSpinLock* m_TxMgrLock;
    CNdisSpinLock* m_TxTargetDescLock; // used to serialize calls to TX_TARGET_DESC_INIT
    //m_TxTargetDescDeinitLock MUST always be the innermost lock.
    CNdisSpinLock* m_TxTargetDescDeinitLock; // used to serialize calls to TX_TARGET_DESC_DEINIT
    CNdisSpinLock* m_TxPrequeueLock; // used to serialize use of TxPortQueues

    UINT16 m_MaxOutstandingTransfers;

    UINT64 m_SchedulingRound;
    //increments whenever a queue is serviced.  on a TxSendRestartInd 
    //txmgr will subtract a number in [0,IterationPeriod) assuming 
    //m_ServiceCounter >= IterationPeriod.  The purpose of the 'reseed'
    //is to break (prevent) interference between cyclic pause/restarts
    //and the priority/global scheduling cycle.
    UINT64 m_ServiceCounter;

    UINT32 m_CurActiveBacklog;
    UINT64 m_CurOutstandingTransfers;
    volatile LONG m_CurInitedTxDescriptors;

    UINT64 m_CurWifiFrameNumber;
    UINT64 m_PrevWifiFrameNumber;

    WFC_TX_QUEUEING_MODE m_QueueingMode;

    CACList* m_pACList;
    CGlobalTxQueueList m_CGlobalTxQueueList;

    //dequeue doesnt have context of TxQueue so TxMgr needs to save context of send.
    WDI_PORT_ID m_ScheduledPortId;
    WDI_PEER_ID m_ScheduledPeerId;
    WDI_EXTENDED_TID m_ScheduledExtTid;
    UINT16 m_ScheduledQueueLength;
    BOOLEAN m_ScheduledRobustnessFlag;

    CTxEntryTable m_TxEntryTable;

    //info from configuration m_TxConfiguration;
    PWFC_DATAPATH_CAPABILITIES m_pDatapathCapabilities;
    UINT8 m_MaxNumPorts;

    LIST_ENTRY m_TxAbortRequestQueue;

    volatile LONG m_ProcessPendingFramesWorkitemScheduled; //use interlocked calls >0 means work item is scheduled

    //Workitem to process NBLs which are queued at port layer
    NDIS_HANDLE m_hSendCompleteWorkItem;
    //index of the last port processed by the ProcessPortQueuesWorkitemRoutine
    ULONG m_LastProcessedPortIndex;

    //size of backfill requested by IHV.  Stored locally to prevent recurrent accesses to 
    // property cache during sends.
    ULONG m_BackFillSize;

    WFC_TIMER_REGISTRATION_TOKEN m_OldFrameDetectionTimerToken;
    TimerRegistrationContext* m_pOldFrameDetectionTimerRegistrationContext; //to prevent each queueing from allocating new timerregistrationcontext

    //check and modify under m_TxMgrLock
    // this will be true unless the timer is already queued OR it has been stopped (halting)
    bool m_bAllowOldFrameTimerQueueing;
    //modified under m_TxMgrLock. Set to true when at least one old frame has been detected
    // and stays true until there is not an old frame detected.
    // a telemetry event is fired when this transitions from false to true or from true to false
    bool m_bIsOldFrameDetected;

    NDIS_STATUS AddNBLToTxQueue(
        _In_ PNET_BUFFER_LIST pNBL,
        _Out_ PBOOLEAN pbPeerBacklogNotifNeeded);

    void CompleteIhvNbl(
        _In_ PNET_BUFFER_LIST pNBL);

    void CompleteNdisNbl(
        _In_ PNET_BUFFER_LIST pNBL,
        _In_ BOOLEAN bDeferSendCompletion);

    __inline NDIS_STATUS InitializeIhvWfcFrame(
        _In_ PWFC_FRAME pWfcFrame,
        _In_ PNET_BUFFER_LIST pNBL,
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId,
        _In_ WDI_EXTENDED_TID ExTid,
        _In_ BOOLEAN bIsUnicast,
        _In_ BOOLEAN bUseLegacyRates,
        _In_ UINT16 Ethertype,
        _In_ WDI_EXEMPTION_ACTION_TYPE ExemptionAction);

    //This is called under a lock so it doesn't need to be interlocked
    __inline UINT64 GetFrameNum()
    {
        return m_CurWifiFrameNumber++;
    }

    __inline void InitializeTxQueue(
        _Out_ PWFC_TX_QUEUE pTxQueue,
        _In_ PWFC_TX_ENTRY pTxEntry,
        _In_ WDI_EXTENDED_TID ExTid)
    {
        InitializeListHead(&pTxQueue->BacklogQueue);
        InitializeListHead(&pTxQueue->GlobalTxQueueListEntry);
        InitializeListHead(&pTxQueue->ACListEntry);
        pTxQueue->bInUse = TRUE;
        //non IHV TIDs will derive the TX_ENTRY_STOPPING, TX_ENTRY_STOPPED and TX_ENTRY_PEER_DELETE_PENDING states from the TxEntry.
        //IHV TIDs will only derive the TX_ENTRY_PEER_DELETE_PENDING state from the TxEntry.
        //This is because we only want to block IHV injected frames during a peer deletion.
        if (IsIhvTid(ExTid))
        {
            pTxQueue->TxQueueState = pTxEntry->TxEntryState & (TX_ENTRY_PEER_DELETE_PENDING);
        }
        else
        {
            pTxQueue->TxQueueState = pTxEntry->TxEntryState & (TX_ENTRY_STOPPING | TX_ENTRY_STOPPED | TX_ENTRY_PEER_DELETE_PENDING);
        }

        //set the paused state of the TxQueue

        if (pTxEntry->TidPauseState[ExTidToId(ExTid)] & WDI_TX_PAUSE_REASON_PS)
        {
            SET_FLAG(pTxQueue->TxQueueState, TX_QUEUE_PS_PAUSED);
        }
        //any bit will set the paused flag. (power save pause treated as subset of pause)
        if (pTxEntry->TidPauseState[ExTidToId(ExTid)])
        {
            SET_FLAG(pTxQueue->TxQueueState, TX_QUEUE_PAUSED);
        }
        pTxQueue->Tid = ExTid;
        pTxQueue->PortId = pTxEntry->PortId;
        pTxQueue->PeerId = pTxEntry->PeerId;
        pTxQueue->Ac = MapTidToAccessCategory(ExTid);
    }

    __inline ULONG CheckNumPages(
        _In_ PNET_BUFFER_LIST pNbl)
    {
        ULONG numPages = 0;
        PNET_BUFFER pCurNb = NULL;

        pCurNb = NET_BUFFER_LIST_FIRST_NB(pNbl);

        while (pCurNb != NULL)
        {
            numPages += NdisQueryNetBufferPhysicalCount(pCurNb);

            pCurNb = NET_BUFFER_NEXT_NB(pCurNb);
        }

        return numPages;
    }

    __inline WFC_TX_QUEUE_ID ExTidToId(
        _In_ WDI_EXTENDED_TID ExTid)
    {
        if (m_QueueingMode == PORT_QUEUEING)
        {
            return IsIhvTid(ExTid) ? 1 : 0;
        }
        else
        {
            return ExTid;
        }
    }

    __inline BOOLEAN IsValidId(
        _In_ WFC_TX_QUEUE_ID Id)
    {
        if (m_QueueingMode == PORT_QUEUEING)
        {
            return (Id < 2) ? TRUE : FALSE;
        }
        else
        {
            return IsValidTid(Id);
        }
    }

    __inline BOOLEAN IsIhvId(
        _In_ WFC_TX_QUEUE_ID Id)
    {
        if (m_QueueingMode == PORT_QUEUEING)
        {
            return (Id > 0) ? TRUE : FALSE;
        }
        else
        {
            return IsIhvTid(Id);
        }
    }

    __inline WFC_TX_ID GetTxId(
        _In_ PWFC_FRAME pWfcFrame)
    {
        return (m_QueueingMode == PORT_QUEUEING) ? pWfcFrame->WiFiFrameMetadata.u.txMetaData.PortID : pWfcFrame->WiFiFrameMetadata.u.txMetaData.PeerID;
    }

    __inline WFC_TX_ID GetTxId(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId)
    {
        return (m_QueueingMode == PORT_QUEUEING) ? PortId : PeerId;
    }

    __inline UINT32 ExTidBitmaskToTxQueueIdBitmask(
        _In_ UINT32 ExTidBitmask)
    {
        UINT32 IdBitmask = 0;
        if (ExTidBitmask & EXT_TID_OS_MASK)
        {
            IdBitmask |= TX_QUEUE_ID_OS_BIT;
        }
        if (ExTidBitmask & EXT_TID_IHV_MASK)
        {
            IdBitmask |= TX_QUEUE_ID_IHV_BIT;
        }
        return IdBitmask;
    }

    _Ret_maybenull_
    __inline  PWFC_TX_QUEUE GetTxQueue(
        _In_ PWFC_TX_ENTRY pTxEntry,
        _In_range_(0, pTxEntry->MaxQueueCount -1) WFC_TX_QUEUE_ID i)
    {
        if (IsValidId(i))
        {
            return (pTxEntry->ppTxQueue) ? (pTxEntry->ppTxQueue)[i] : NULL;
        }
        else
        {
            return NULL;
        }
    }

    //Peer Queueing :Adds a txqueue to the ACList and returns the status of the add
    //Port Queueing :m_pTxEntry will be null and success will be returned.  This essentially puts
    // the "is port queueing?" in this function rather than the caller.
    __inline NDIS_STATUS AddTxQueueToAcEntry(
        _In_ PWFC_TX_QUEUE pTxQueue)
    {
        NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
        if (m_pACList)
        {
            ndisStatus = m_pACList->AddTxQueue(pTxQueue);
        }
        return ndisStatus;
    }

    __inline NDIS_STATUS RemoveTxQueueFromAcEntry(
        _In_ PWFC_TX_QUEUE pTxQueue)
    {
        NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
        if (m_pACList)
        {
            ndisStatus = m_pACList->RemoveTxQueue(pTxQueue);
        }
        return ndisStatus;
    }

    //At least one bit must be set in the PauseReason and the PauseReason
    // may not contain a reason reserved for peers
    __inline BOOLEAN IsValidPauseReason(
        _In_ WDI_TX_PAUSE_REASON TxPauseReason)
    {
        if (TxPauseReason == 0)
        {
            return FALSE;
        }
        if (m_QueueingMode == PORT_QUEUEING)
        {
            //PS and PeerCreate are only peer specific reasons
            if (TxPauseReason & (WDI_TX_PAUSE_REASON_PS | WDI_TX_PAUSE_REASON_PEER_CREATE))
            {
                return FALSE;
            }
        }
        return TRUE;
    }

    //Returns true if 
    //  1) operating in PEER_QUEUEING mode
    //  2) all TIDs have a bit set in the pause bitmask array
    __inline BOOLEAN IsPeerPaused(
        _In_ PWFC_TX_ENTRY pTxEntry)
    {
        int i;

        if (m_QueueingMode == PORT_QUEUEING)
        {
            return FALSE;
        }

        for (i = 0; IsValidId((WFC_TX_QUEUE_ID)i); i++)
        {
            if (pTxEntry->TidPauseState[i] == 0)
            {
                return FALSE;
            }
        }
        return TRUE;
    }

    NDIS_STATUS StopCancelSendTxEntry(
        _In_ PWFC_TX_ENTRY pTxEntry,
        _In_ WFC_TX_STOP_REASON TxStopReason,
        _Inout_ PNET_BUFFER_LIST *ppNdisCompleteNbl,
        _Inout_ PNET_BUFFER_LIST *ppIhvCompleteNbl);

    NDIS_STATUS RestartSendTxEntry(
        _In_ PWFC_TX_ENTRY pTxEntry);

    NDIS_STATUS PausePeerEntry(
        _In_ PWFC_TX_ENTRY pTxEntry,
        _In_ UINT32 ExTidBitmask,
        _In_ WDI_TX_PAUSE_REASON TxPauseReason);

    NDIS_STATUS PausePortEntry(
        _In_ PWFC_TX_ENTRY pTxEntry,
        _In_ UINT32 IdBitmaks,
        _In_ WDI_TX_PAUSE_REASON TxPauseReason);

    NDIS_STATUS ResumePeerEntry(
        _In_ PWFC_TX_ENTRY pTxEntry,
        _In_ UINT32 ExTidBitmask,
        _In_ WDI_TX_PAUSE_REASON TxRestartReason);

    NDIS_STATUS ResumePortEntry(
        _In_ PWFC_TX_ENTRY pTxEntry,
        _In_ UINT32 IdBitmask,
        _In_ WDI_TX_PAUSE_REASON TxRestartReason);

    NDIS_STATUS PeerDeleteConfirm(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId);

    NDIS_STATUS PeerAbortConfirm(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId);

    NDIS_STATUS TxAbortRequest(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId);

    NDIS_STATUS RequeueNbl(
        _In_ PWFC_TX_ENTRY pTxEntry,
        _In_ PWFC_TX_QUEUE pTxQueue,
        _In_ PWFC_FRAME pWfcFrame,
        _Out_ PBOOLEAN pbPeerBacklogNotifNeeded);

    void CheckForOldFrames();

    void QueueOldFrameDetectionTimer();

    void LogOldFrameStateChangeTelemetryEvent(
        _In_ bool bOldFrameDetected);
};

