/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    datapath.h

Abstract:

    Contains common type definitions for the datapath driver private
    This header should be included after the public wdi header.

Environment:

    Kernel mode

Author:


--*/

#pragma once

#define WDI_TXRX_POOL_TAG (ULONG) 'DidW'
#define WDI_TX_POOL_TAG   (ULONG) 'TidW'
#define WDI_RX_POOL_TAG   (ULONG) 'RidW'

#define SET_FLAG(_Flags, _F)         ((_Flags) |= (_F))   
#define CLEAR_FLAG(_Flags, _F)       ((_Flags) &= ~(_F))
#define TEST_FLAG(_Flags, _F)        (((_Flags) & (_F)) != 0)

#define WDI_EXT_TID_NON_QOS ((WDI_EXTENDED_TID)16)

#define EXT_TID_0_BIT  0x00000001
#define EXT_TID_1_BIT  0x00000002
#define EXT_TID_2_BIT  0x00000004
#define EXT_TID_3_BIT  0x00000008
#define EXT_TID_4_BIT  0x00000010
#define EXT_TID_5_BIT  0x00000020
#define EXT_TID_6_BIT  0x00000040
#define EXT_TID_7_BIT  0x00000080
#define EXT_TID_8_BIT  0x00000100
#define EXT_TID_9_BIT  0x00000200
#define EXT_TID_10_BIT 0x00000400
#define EXT_TID_11_BIT 0x00000800
#define EXT_TID_12_BIT 0x00001000
#define EXT_TID_13_BIT 0x00002000
#define EXT_TID_14_BIT 0x00004000
#define EXT_TID_15_BIT 0x00008000
#define EXT_TID_16_BIT 0x00010000
#define EXT_TID_17_BIT 0x00020000
#define EXT_TID_18_BIT 0x00040000
#define EXT_TID_19_BIT 0x00080000
#define EXT_TID_20_BIT 0x00100000
#define EXT_TID_21_BIT 0x00200000
#define EXT_TID_22_BIT 0x00400000
#define EXT_TID_23_BIT 0x00800000
#define EXT_TID_24_BIT 0x01000000

#define EXT_TID_OS_MASK ( \
    EXT_TID_0_BIT  | EXT_TID_1_BIT  | EXT_TID_2_BIT  | EXT_TID_3_BIT  | \
    EXT_TID_4_BIT  | EXT_TID_5_BIT  | EXT_TID_6_BIT  | EXT_TID_7_BIT  | \
    EXT_TID_8_BIT  | EXT_TID_9_BIT  | EXT_TID_10_BIT | EXT_TID_11_BIT | \
    EXT_TID_12_BIT | EXT_TID_13_BIT | EXT_TID_14_BIT | EXT_TID_15_BIT | \
    EXT_TID_16_BIT )

#define EXT_TID_IHV_MASK ( \
    EXT_TID_17_BIT | EXT_TID_18_BIT | EXT_TID_19_BIT | EXT_TID_20_BIT | \
    EXT_TID_21_BIT | EXT_TID_22_BIT | EXT_TID_23_BIT | EXT_TID_24_BIT )


#include <packon.h>
typedef struct NWF_802_LLC_SNAP {
    BYTE sh_dsap;
    BYTE sh_ssap;
    BYTE sh_ctl;
    BYTE sh_protid[3];
} NWF_802_LLC_SNAP, * PNWF_802_LLC_SNAP;

typedef struct {
    NWF_802_LLC_SNAP LLC;
    unsigned short  sh_etype;
} IEEE_8022_LLC_SNAP, *PIEEE_8022_LLC_SNAP;
#include <packoff.h>

#define IEEE_8022_LLC_SNAP_SIZE     sizeof(IEEE_8022_LLC_SNAP)

typedef enum _WFC_FRAME_TYPE {
    WFC_FRAME_TYPE_UNINITIALIZED,
    TX_INJECTED_FRAME,
    TX_NDIS_FRAME,
    RX_FRAME_TYPE_IHV,
    RX_FRAME_TYPE_DERIVED,
    RX_FRAME_TYPE_DEFRAGMENTED_MSDU,
    RX_FRAME_TYPE_FIPS_DEFRAGMENTED_MSDU,
    RX_FRAME_TYPE_MSDU,
    RX_FRAME_TYPE_AMSDU,
    WFC_FRAME_TYPE_MAX
} WFC_FRAME_TYPE;

typedef UINT64 WFC_FRAME_NUM;

typedef struct _WFC_TX_FRAME
{
    LIST_ENTRY     FrameTableLink;
    //If this is set then pCloneNbl points to a cloned
    // NBL with the *original* NB chain.  The original NBL
    // will have the copied NB chain.  Before returning these
    // will need to be swapped and the cloned NBL will need to be freed
    PNET_BUFFER_LIST pCloneNbl;

    WDI_TX_FRAME_STATUS TransferStatus;
    WDI_TX_FRAME_STATUS SendStatus;
    BOOLEAN bIsTransferStatusSet;
    BOOLEAN bIsSendStatusSet;
    BOOLEAN bIsReplayDataSet;
    BOOLEAN bIsPortPending;
} WFC_TX_FRAME;


typedef struct _WFC_RX_FRAME
{
    WFC_PORT_ID       PortId;
    WDI_PEER_ID      PeerId;
    WDI_EXTENDED_TID ExTid;

    PNET_BUFFER_LIST MSDUFragmentNblChain;  // frag list for freeing on return
    ULONG            cbMSDUFragmentMPDUHeaderSize; // Size of MPDU header for fragmented MSDU
    
    PDOT11_EXTSTA_RECV_CONTEXT pDot11RecvContext;

    UINT8            Rssi;
    BOOL             Cloned;

} WFC_RX_FRAME;


typedef struct _WFC_FRAME {
    //
    // Common for TxRx
    //
    ULONG          ulSizeOfThis;
    WFC_FRAME_TYPE FrameType;
    UINT64         FrameNum;

    // align previous fields to 128 bits for 64 bit OS
    __declspec(align(16)) union {
        WFC_TX_FRAME Tx;
        WFC_RX_FRAME Rx;
    } u;

    // Add internal fields here as needed before the next public field.
    WDI_FRAME_METADATA     WiFiFrameMetadata; // public field for MP
    // if we allow MP to specify extra size for work space, start here.
} WFC_FRAME, *PWFC_FRAME;

#define WDI_FRAME_FROM_WFC_FRAME( _p ) (&(_p)->WiFiFrameMetadata )
#define WFC_FRAME_FROM_WDI_FRAME( _p ) CONTAINING_RECORD( _p, WFC_FRAME, WiFiFrameMetadata )

_inline 
PWDI_FRAME_METADATA
GetWiFiFrameMetadataFromNbl(
    _In_ PNET_BUFFER_LIST pNbl )
{
    return ((PWDI_FRAME_METADATA)pNbl->MiniportReserved[0]);
}

typedef class _WFC_DATAPATH_CONSTANT
{
public:
    static const UINT NumExtendedTids = 25; //0-15:802.11, 16:non-QoS, 17-24:IHV TIDs
    static const UINT NumPortQueues = 2; //0:NDIS frames, 1:injected frames
    static const UINT MaximumNumberTxPeerEntries = 255;
    static const UINT NumFrameIds = (1<<16); //2^16 frame ids
    static const UINT NumLookupEntries = (1<<6); //number of list heads in the frame lookup table.  Must be a power of 2.  incurs a cost of n*sizeof(LIST_ENTRY)
    static const UINT FrameLookupShift = 3; //this means that the index into the lookup table ignores the last 3 bits of the FrameId (0-7 will map to same index, 8-15 will map to next index)
    static const UINT MaxNumSendsPerServicing = 1024;
    static const UINT NumPriorityIterations = 8;
    static const UINT NumGlobalIterations = 2;
    static const UINT IterationPeriod = NumPriorityIterations + NumGlobalIterations;
    static const UINT FrameTableTimerOverscan = 16;
    static const UINT OldFrameDetectionPeriodMs = 60 * 1000; //60s old frame detection period
} WFC_DATAPATH_CONSTANT;


__inline BOOLEAN AreMacAddressEqual(
    _In_ PCWDI_MAC_ADDRESS A,
    _In_ PCWDI_MAC_ADDRESS B)
/*++
--*/
{
    return (RtlCompareMemory(A,B,sizeof(WDI_MAC_ADDRESS)) == sizeof(WDI_MAC_ADDRESS));
}


NDIS_WDI_TX_DEQUEUE_IND                 AdapterTxDequeueInd;
NDIS_WDI_TX_TRANSFER_COMPLETE_IND       AdapterTxTransferCompleteInd;
NDIS_WDI_TX_SEND_COMPLETE_IND           AdapterTxSendCompleteInd;
NDIS_WDI_TX_QUERY_RA_TID_STATE          AdapterTxQueryRaTidState;
NDIS_WDI_TX_SEND_PAUSE_IND              AdapterTxSendPauseInd;
NDIS_WDI_TX_SEND_RESTART_IND            AdapterTxSendRestartInd;
NDIS_WDI_TX_RELEASE_FRAMES_IND          AdapterTxReleaseFramesInd;
NDIS_WDI_TX_INJECT_FRAME_IND            AdapterTxInjectFrameInd;
NDIS_WDI_TX_ABORT_CONFIRM               AdapterTxAbortConfirm;

NDIS_WDI_RX_INORDER_DATA_IND            AdapterRxInorderDataInd;
NDIS_WDI_RX_STOP_CONFIRM                AdapterRxStopConfirm;
NDIS_WDI_RX_FLUSH_CONFIRM               AdapterRxFlushConfirm;

NDIS_WDI_PEER_CREATE_IND                AdapterPeerCreateInd;
NDIS_WDI_PEER_DELETE_IND                AdapterPeerDeleteInd;

NDIS_WDI_ALLOCATE_WDI_FRAME_METADATA   AdapterAllocateWifiFrameMetadata;
NDIS_WDI_FREE_WDI_FRAME_METADATA       AdapterFreeWifiFrameMetadata;

//This stub is used to prevent an AV.  It is only called when the IHV misbehaves and will cause a verifier assert when called
MINIPORT_WDI_TX_PEER_BACKLOG           TxPeerBacklogStub;

typedef struct _WFC_PEER_TABLE_ENTRY {
    WDI_PORT_ID       PortId;
    WDI_PEER_ID       PeerId;
    WDI_MAC_ADDRESS   PeerAddr;
    BOOLEAN            bInUse;
} WFC_PEER_TABLE_ENTRY, *PWFC_PEER_TABLE_ENTRY;

class CPeerTable
{
public:
    CPeerTable();

    ~CPeerTable();

    NDIS_STATUS Initialize(
        _In_ UINT8 MaxNumberOfPeers);

    NDIS_STATUS AddPeer(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId,
        _In_ PWDI_MAC_ADDRESS pPeerAddr);

    NDIS_STATUS RemovePeer(
        _In_ WDI_PEER_ID PeerId);

    WDI_PEER_ID GetPeerId(
        _In_ PWDI_MAC_ADDRESS pPeerAddr,
        _In_ WDI_PORT_ID PortId);

private:
    CNdisSpinLock* m_PeerTableLock;

    UINT m_TableSize;

    UINT m_NumEntries;

    WFC_PEER_TABLE_ENTRY* m_pPeerTable;

    PWFC_PEER_TABLE_ENTRY CPeerTable::AssignPeerEntry(
        _In_ WDI_PORT_ID PortId,
        _In_ WDI_PEER_ID PeerId,
        _In_ PWDI_MAC_ADDRESS pPeerAddr);

    __inline UINT hash_func(
        _In_ WDI_PORT_ID PortId,
        _In_ PWDI_MAC_ADDRESS pPeerAddr)
    {
        UINT val = 0;
        if(ETH_IS_MULTICAST(pPeerAddr))
        {
            return PortId % m_TableSize;
        }
        else
        {
            val = pPeerAddr->Address[0] ^ pPeerAddr->Address[1] ^
                  pPeerAddr->Address[2] ^ pPeerAddr->Address[3] ^
                  pPeerAddr->Address[4] ^ pPeerAddr->Address[5];
            return val % m_TableSize;
        }
    }

    __inline BOOLEAN DoesPeerMatch(
        _In_ PWFC_PEER_TABLE_ENTRY pPeerTableEntry,
        _In_ WDI_PORT_ID PortId,
        _In_ PWDI_MAC_ADDRESS pPeerAddr)
    {
        BOOLEAN bResult = FALSE;
        if (ETH_IS_MULTICAST(pPeerAddr))
        {
            //from the spec the IHV miniport adds a broadcast peer for use
            // with all group traffic(multicast and broadcast) so match
            // all group addresses to a broadcast address
            if (ETH_IS_BROADCAST(&pPeerTableEntry->PeerAddr) &&
                pPeerTableEntry->PortId == PortId               )
            {
                bResult = TRUE;
            }
            else
            {
                bResult = FALSE;
            }
        }
        else
        {
            bResult = AreMacAddressEqual(&pPeerTableEntry->PeerAddr, pPeerAddr);
        }
        return bResult;
    }
};

//TODO lookup table for frame IDs initially use hash table with chaining.
class CFrameIdLookupTable
{
public:
    CFrameIdLookupTable();

    ~CFrameIdLookupTable();

    NDIS_STATUS Initialize();

    NDIS_STATUS AssignAndMapFrameId(
        _In_ PWFC_FRAME pWfcFrame);

    NDIS_STATUS FreeAndUnmapFrameId(
        _In_ PWFC_FRAME pWfcFrame);

    PNET_BUFFER_LIST FindNblByFrameId(
        _In_ WDI_FRAME_ID FrameId);

    //Log outstanding frames (frames with framenums < MinFrameNum)
    //returns number of old frames detected
    UINT32 LogFramesWithOldFrameNum(
        _In_ UINT64 MinFrameNum);

private:
    CNdisSpinLock* m_FrameIdLock;
    BOOLEAN m_Initialized;

    //m_FreeFrameIdStartIndex is not the index for a free frameId rather it is the index
    // where the lowest free frameID *COULD* be (rather all frameIds in indexes lower are in use)
    // it is used to improve searching for frameId.
    UINT32 m_FreeFrameIdStartIndex;
    UINT32 m_NumFrameIdAssigned;

    static const UINT32 c_NumLookupEntries = WFC_DATAPATH_CONSTANT::NumLookupEntries;
    static const UINT32 c_FrameLookupShift = WFC_DATAPATH_CONSTANT::FrameLookupShift;
    static const UINT32 c_NumFrameIds = WFC_DATAPATH_CONSTANT::NumFrameIds;
    static const UINT32 c_BitsInUint32 = sizeof(UINT32) * 8;
    static const UINT32 c_NumEntries = (c_NumFrameIds + ((c_BitsInUint32 - c_NumFrameIds) % c_BitsInUint32) % (c_BitsInUint32)) / (c_BitsInUint32);

    LIST_ENTRY m_FrameListHeadArray[c_NumLookupEntries];

    UINT32 m_FrameIdBitArray[c_NumEntries];

    __forceinline WDI_FRAME_ID GetNextFrameId();

    __forceinline void FreeFrameId(
        _In_ WDI_FRAME_ID FrameId);
};

