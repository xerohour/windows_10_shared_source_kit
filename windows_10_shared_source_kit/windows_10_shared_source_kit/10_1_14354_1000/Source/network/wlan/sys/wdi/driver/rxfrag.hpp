/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    rxfrag.hpp

Abstract:

    Header for the WDI RxMgr's MSDU defragment helper structures.

Environment:

    Kernel mode

Author:


--*/

#pragma once

#define MIN_PER_PORT_DEFRAGMENT_BUFFERS  3 // 11spec requires at least 3 
#define MAX_CONCURRENT_MSDU             32 // 3 per port * 5 ports == 15 minimum
#define MSDU_FRAGMENT_TIMEOUT     10000000 // 1 second in units of 0.1us

typedef struct _WDI_RX_DEFRAG_ENTRY {
    WDI_PORT_ID      PortId;
    WDI_PEER_ID      PeerId;
    USHORT            SequenceNumber;
    USHORT            FragmentsReceivedMask;
    ULONGLONG         StartTime;

    PNET_BUFFER_LIST  pNblFragmentChain;

    BOOLEAN           IsInUse() { return (FragmentsReceivedMask != 0); }
    void              InsertFragment(USHORT FragNo, PNET_BUFFER_LIST pNbl);

} WDI_RX_DEFRAG_ENTRY, *PWDI_RX_DEFRAG_ENTRY;

class CRxMgr;
class CAdapter;

class CRxDefragTable
{
public:
    CRxDefragTable( _In_ CAdapter * pAdapter );

    ~CRxDefragTable()
    {
        if (m_LockRxDefragTable != NULL)
        {
            delete m_LockRxDefragTable;
            m_LockRxDefragTable = NULL;
        }
    }

    NDIS_STATUS Initialize();

    NDIS_STATUS AddFragment(
        _In_  PNET_BUFFER_LIST       pNBL,
        _In_  WDI_PORT_ID           PortId,
        _In_  WDI_PEER_ID           PeerId,
        _In_  DOT11_SEQUENCE_CONTROL SeqInfo,
        _In_  BOOLEAN                bMoreFrag,
        _In_  BOOLEAN                bCopyIfKeep,
        _Out_ PNET_BUFFER_LIST     * pCompleteMSDUNblChain );

    void RemovePeerFragments( WDI_PEER_ID PeerId );

    void RemovePortFragments( WDI_PORT_ID PordId );

private:
    PNET_BUFFER_LIST FlushFragmentList( UCHAR index );

    NDIS_STATUS AddFragmentToEntry(
        _In_  PNET_BUFFER_LIST       pNbl,
        _In_  UCHAR                  index,
        _In_  USHORT                 FragmentNumber,
        _In_  BOOLEAN                bMoreFrag,
        _In_  BOOLEAN                bCopyIfKeep,
        _Out_ PNET_BUFFER_LIST     * pCompleteMSDUNblChain );

    UCHAR ForceFreeFragmentEntry(
        _In_  WDI_PORT_ID           PortId,
        _Out_ PNET_BUFFER_LIST     * ppNblChain);

    // Defragmenting may collide with control-path stop/flush
    // So we lock. Performance isn't a big deal because we're using
    // fragmented MSDU's so this isn't high-throughput scenario.
    CNdisSpinLock          * m_LockRxDefragTable;

    WDI_RX_DEFRAG_ENTRY     m_MSDUDefragArray[MAX_CONCURRENT_MSDU];
    CRxMgr                * m_pRxMgr;
    CAdapter              * m_pAdapter;
};

