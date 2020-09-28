/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    rxmgr.hpp

Abstract:

    Header for the WDI RxMgr.

Environment:

    Kernel mode

Author:


--*/

#pragma once

#define SafeDelete( ppo ) if ( *(ppo) != NULL ) { delete *(ppo); *(ppo) = NULL; }

__inline void WdfSafeDelete( 
    _In_ WDFOBJECT * ppo )
{
    if ( *(ppo) != NULL ) 
    { 
        WdfObjectDelete(*ppo); 
        *(ppo) = NULL; 
    }
}


#define MAX_DEFRAGMENT_BUFFERS 4 // 11spec requires at least 3 (per port?)

typedef enum _WFC_RX_PEER_STATE {
    RxPeerStateInvalid                  = 0,
    RxPeerStateStopped                  = 1,
    RxPeerStatePausedForAssoc           = 2,
    RxPeerStatePausedForAssocWithData   = 3,
    RxPeerStateStarted                  = 4,
} WFC_RX_PEER_STATE;

typedef enum _WFC_RX_STOP_REASON {
    RxStopReasonPause       = 1,
    RxStopReasonDot11Reset  = 2,
    RxStopReasonHalt        =3
} WFC_RX_STOP_REASON;


typedef struct _WDI_RX_PEER_ENTRY {
    WDI_PORT_ID      PortId;
    WDI_PEER_ID      PeerId;

    WFC_RX_PEER_STATE RxPeerState;

    WDI_MAC_ADDRESS  PeerAddr;
} WDI_RX_PEER_ENTRY, *PWDI_RX_PEER_ENTRY;

#define RXMGR_PEER_ARRAY_SIZE 32

class CRxPeerTable
{
    // 
    // Except GO, the table is very small, less than low teen, maybe lcopules
    // A linear search is used. When the table grows big, will switch to hash table.
    //
public:
    CRxPeerTable():
    m_TableSize( RXMGR_PEER_ARRAY_SIZE )
    {
    };

    ~CRxPeerTable()
    {
        SafeDelete( &m_LockRxPeerTable );
    }

    NDIS_STATUS Initialize(
        _In_ CAdapter * pAdapter,
        _In_ UINT8 MaxNumPeers);

    NDIS_STATUS RxPeerEntryAdd(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId,
        _In_ PCWDI_MAC_ADDRESS PeerAddr);

    NDIS_STATUS RxPeerEntryRemove(
        _In_ WDI_PEER_ID PeerId);

    NDIS_STATUS RxPeerGetPortIdState( 
        _In_ WDI_PEER_ID PeerId,
        _Out_ WDI_PORT_ID* pPortId,
        _Out_ WFC_RX_PEER_STATE * pRxPeerState );

    NDIS_STATUS RxPeersForPortSetState( 
        _In_ WDI_PORT_ID PortId,
        _In_ WFC_RX_PEER_STATE RxPeerState );

    NDIS_STATUS RxPeerUnpause(
        _In_ WDI_PEER_ID PeerId );

    NDIS_STATUS RxPeerPauseWithData(
        _In_ WDI_PEER_ID PeerId );

    NDIS_STATUS RxPeerGetPeerId(
        _In_ PCWDI_MAC_ADDRESS PeerAddr,
        _Out_ WDI_PEER_ID *pPeerId);

private:
    CNdisSpinLock *     m_LockRxPeerTable;
    CAdapter *          m_pAdapter;
    UINT                m_MaxNumPeers;
    //static const UINT  m_PeerEntryArraySize = 32; // turn to hash table if grows over this.
    UINT                m_TableSize;
    WDI_RX_PEER_ENTRY   m_PeerEntryArray[RXMGR_PEER_ARRAY_SIZE];
    WDI_RX_PEER_ENTRY*  m_pPeerEntryTable;   // used when m_TbleSize > m_InitialTableSize
};

class NblChain
{
public:
    NblChain() : m_pNblHead( NULL ), m_ppNblTail( &m_pNblHead ), m_NblCnt( 0 ) {}
    PNET_BUFFER_LIST GetHead() { return m_pNblHead; }

    PNET_BUFFER_LIST RemoveHead()
    {
        PNET_BUFFER_LIST pOldHead = m_pNblHead;

        if (pOldHead)
        {
            m_pNblHead = NET_BUFFER_LIST_NEXT_NBL(pOldHead);
            NET_BUFFER_LIST_NEXT_NBL(pOldHead) = NULL;
            m_NblCnt--;
        }

        //fix tail in case list is now empty
        if (m_NblCnt == 0)
        {
            m_ppNblTail = &m_pNblHead;
        }

        return pOldHead;
    }

    void AddHead(PNET_BUFFER_LIST pNbl)
    {
        NT_ASSERT(NULL == NET_BUFFER_LIST_NEXT_NBL(pNbl));
        if (m_NblCnt == 0)
        {
            AddTail(pNbl);
        }
        else
        {
            NET_BUFFER_LIST_NEXT_NBL(pNbl) = m_pNblHead;
            m_pNblHead = pNbl;
            m_NblCnt++;
        }
    }

    void AddTail( PNET_BUFFER_LIST pNbl )
    {
        *m_ppNblTail = pNbl;
        NT_ASSERT( NULL == NET_BUFFER_LIST_NEXT_NBL( pNbl ) );
        m_ppNblTail = &(NET_BUFFER_LIST_NEXT_NBL( *m_ppNblTail ));
        m_NblCnt++;
    }

    void AddChainTail( PNET_BUFFER_LIST pNblChain )
    {
        *m_ppNblTail = pNblChain;
        while (NULL != NET_BUFFER_LIST_NEXT_NBL( pNblChain ))
        {
            pNblChain = NET_BUFFER_LIST_NEXT_NBL( pNblChain );
            m_NblCnt++;
        }
        m_ppNblTail = &(NET_BUFFER_LIST_NEXT_NBL( pNblChain ));
        m_NblCnt++;
    }

    ULONG GetCount() { return m_NblCnt;  }

    void Reset()
    {
        m_pNblHead = NULL;
        m_ppNblTail = &m_pNblHead;
        m_NblCnt = 0;
    }

private:
    PNET_BUFFER_LIST   m_pNblHead;
    PNET_BUFFER_LIST * m_ppNblTail;
    ULONG               m_NblCnt;
};

struct SmoothedThrottleLevel
{
    SmoothedThrottleLevel( ) : m_value( 0 ) {}

    bool IncreaseThrottle();
    bool DecreaseThrottle();

    operator WDI_RX_THROTTLE_LEVEL() const
    {
        return ToLevel(m_value);
    }

    ULONG GetLogValue( ) const
    {
        return m_value;
    }

private:

    static WDI_RX_THROTTLE_LEVEL ToLevel( ULONG value )
    {
        return (WDI_RX_THROTTLE_LEVEL)(value >> ShiftAmount);
    }

    static const int ShiftAmount = 24;
    static const ULONG Range = ((1 << ShiftAmount) - 1);
    static const ULONG CountIncrements = 200; // Smooth to about 200 'events'
    static const ULONG Delta = Range / CountIncrements;
    ULONG m_value;
};

class CPort;

class CRxMgr {

public:
    CRxMgr( ) :
        m_pRxDefragTable( NULL ),
        m_RxState( rmsWaitingForData ),
        m_pRxResumeThread( NULL ),
        m_PortIdWaitStopFlush( WDI_PORT_ANY ),
        m_DpcNblCount( 0 ),
        m_DpcNblLimit( ULONG_MAX ),
        m_hRxWorkItem( NULL ),
        m_PendingNbls( NULL ),
        m_fDataIndLowResources( false ),
        m_fPreviousDataIndWasPaused( false )
    {
    };

    ~CRxMgr()
    {
        SafeDelete( &m_pRxDefragTable );
        NT_ASSERT( m_hRxWorkItem == NULL );
        NT_ASSERT( m_PendingNbls == NULL );
    };

    NDIS_STATUS Initialize(
        _In_ CAdapter * pAdapter,
        _In_ PWFC_DATAPATH_CAPABILITIES pWfcDataPathCapabilities,
        _In_ PNDIS_MINIPORT_WDI_DATA_HANDLERS pMpWdiDataHandlers 
        );

    void Deinitialize();

    void RxInOrderDataInd(
            _In_ WDI_RX_INDICATION_LEVEL IndicationLevel,
            _In_ WDI_PEER_ID PeerId,
            _In_ WDI_EXTENDED_TID ExTid,
            _In_ PNDIS_RECEIVE_THROTTLE_PARAMETERS pRxThrottleParams,
            _Out_ NDIS_STATUS * pWifiStatus
            );

    void RxStopConfirm();
    void RxFlushConfirm();

    void
    OnNdisReturnNetBufferLists(
            _In_  PNET_BUFFER_LIST    NetBufferLists,
            _In_  ULONG               ReturnFlags
            );

    NDIS_STATUS AddPeerId(
            _In_ WDI_PORT_ID PortId,
            _In_ WDI_PEER_ID PeerId,
            _In_ PCWDI_MAC_ADDRESS PeerAddr
            )
    {
        NDIS_STATUS ndisStatus;

        ndisStatus = m_RxPeerTable.RxPeerEntryAdd( PortId, PeerId, PeerAddr );

        return ndisStatus;
    }

    NDIS_STATUS RemovePeerId(
        _In_ WDI_PEER_ID PeerId
        );

    void OnAssociationComplete(
        _In_ WDI_PEER_ID PeerId
        );

    NDIS_STATUS
    RxNblAddRecvContext( 
        _In_ PWFC_FRAME pWfcFrame,
        _In_ UINT8 Rssi
        );

    void RxIndicatePortFrames(
        _In_ WFC_PORT_ID PortId,
        _In_ PNET_BUFFER_LIST pNblChain,
        _In_ UINT numNbls,
        _In_ ULONG ReceiveFlags);

    void RxReturnFrames(PNET_BUFFER_LIST pNBL);

    VOID WorkItemRoutine();
    /*++

        Use to resume Data Indication. There will be only one RxMgr thread running.
        It is either one from TAL idicating Rx, or us resuming Rx.

    --*/

    NDIS_STATUS RxPortStop(
        _In_ WFC_PORT_ID PortId,
        _In_ WFC_RX_STOP_REASON RxStopReason );

    NDIS_STATUS RxPortRestart(
        _In_ WFC_PORT_ID PortId );

private:

    volatile enum RxMgrState : LONG
    {
        rmsWaitingForData           = 0x00,
        rmsWithData                 = 0x01,
        rmsWaitingForStop           = 0x02,
        rmsWaitingForStopWithData   = 0x03,
        rmsWaitingForFlush          = 0x04,
        rmsWaitingForFlushWithData  = 0x05,

        rmsWaitingForDrain          = 0x09,

        rmsStopFlushDrainMask       = 0x0E,
    }                       m_RxState;

    NDIS_HANDLE             m_hTalTxRx;
    CAdapter *              m_pAdapter;
    WFC_PORT_ID             m_PortIdWaitStopFlush;
    PKTHREAD                m_pRxResumeThread;
    PNET_BUFFER_LIST        m_PendingNbls;
    NDIS_HANDLE             m_hRxWorkItem;
    ULONG                   m_DpcNblLimit, m_DpcNblCount;
    SmoothedThrottleLevel   m_RxThrottleLevel;
    bool                    m_fPreviousDataIndWasPaused;
    bool                    m_fDataIndLowResources;
    PWFC_DATAPATH_CAPABILITIES m_pWfcDataPathCapabilities;    // assume CApter keeps the actual content
    PNDIS_MINIPORT_WDI_DATA_HANDLERS m_pMpWdiDataHandlers;  // ditto
    CRxPeerTable            m_RxPeerTable;

    // Touched on both data and control paths, with lazy init, so tread lightly
    CRxDefragTable *        m_pRxDefragTable;

    void RxStateChangeData()
    {
        NT_ASSERTMSG( "Illegal state to data indication while still processing previous", !RxStateHasData() );
        InterlockedOr( (volatile LONG*)&m_RxState, rmsWithData );
    }
    void RxStateChangeStop( )
    {
        NT_ASSERTMSG( "Illegal state to stop from", (m_RxState & ~rmsWithData) == 0 );
        InterlockedOr( (volatile LONG*)&m_RxState, rmsWaitingForStop ); // Set waiting for stop
    }
    void RxStateChangeStopConfirm()
    {
        NT_ASSERTMSG( "Illegal stop confirm without stop", (m_RxState & rmsStopFlushDrainMask) == rmsWaitingForStop );
        // Clear the stop bit and set the flush bit, leaving us with either rmsWaitingForFlush or rmsWaitingForFlushWithData
        InterlockedXor( (volatile LONG*)&m_RxState, rmsWaitingForFlush | rmsWaitingForStop );
    }
    void RxStateChangeFlushConfirm( );
    void RxStateChangeDrained()
    {
        NT_ASSERTMSG( "Illegal state to drain with no previous data indication", RxStateHasData( ) );
        if (rmsWaitingForDrain == InterlockedAnd( (volatile LONG*)&m_RxState, ~rmsWaitingForDrain ))
        {
            RxIndicateStopFlushComplete();
        }
    }
  
    // State query, remember this is stale before it even return!
    bool RxStateHasData()
    {
        return (0 != (m_RxState & rmsWithData));
    }

    void RxIndicateStopFlushComplete();

    NDIS_STATUS RxClassify(
        _Inout_ WDI_PEER_ID *pPeerId,
        _Inout_ WDI_EXTENDED_TID *pExTid,
        _In_ PNET_BUFFER_LIST pNbl);

    PNET_BUFFER_LIST RxDefragmentMSDU(
        _In_ PWDI_FRAME_METADATA pWiFiFrameMetaData,
        _In_ WDI_PORT_ID portId,
        _In_ WDI_PEER_ID peerId);

    NDIS_STATUS RxProcessAndIndicateNblChain(
        _In_ PNET_BUFFER_LIST pPendingNBLs,
        _In_ WDI_RX_INDICATION_LEVEL IndicationLevel );

    void RequeuePendingRxNbls(
        _In_ PNET_BUFFER_LIST pNblChain,
        _In_ WDI_PEER_ID     WaitForAssociationPeerId
        );


    void RxSendIncreaseThrottle( );
    void RxSendDecreaseThrottle( );

    ULONG GetReceiveFlags( WDI_RX_INDICATION_LEVEL IndicationLevel )
    {
        ULONG flags = 0;
        // 
        // Don't set NDIS_RECEIVE_FLAGS_DISPATCH_LEVEL unless we know
        // already because the LE told us.  We know for 3 out of the 4
        // indication levels, but for RX_RESUME_FRAMES, we don't know.
        // Although our code is running at passive, the LE might have
        // raised the IRQL before calling back, so here play it safe
        // and don't set the flag.
        //
        if ((IndicationLevel == WDI_RX_INDICATION_DISPATCH_GENERAL) ||
            (IndicationLevel == WDI_RX_INDICATION_DISPATCH_FIRST_OF_DPC))
        {
            flags |= NDIS_RECEIVE_FLAGS_DISPATCH_LEVEL;
        }
        else
        {
            NT_ASSERTMSG_ASSUME( "Illegal IndicationLevel passed to GetReceiveFlags", 
                (IndicationLevel == WDI_RX_INDICATION_PASSIVE) || (IndicationLevel == WDI_RX_INDICATION_FROM_RX_RESUME_FRAMES));
        }
        if (m_fDataIndLowResources)
        {
            flags |= NDIS_RECEIVE_FLAGS_RESOURCES;
        }
        return flags;
    }
};

