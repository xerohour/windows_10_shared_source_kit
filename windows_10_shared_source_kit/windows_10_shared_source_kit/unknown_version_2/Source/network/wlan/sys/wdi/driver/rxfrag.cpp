/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    rxfrag.cpp

Abstract:

    Contains code for the WDI RxMgr's MSDU defragment helper routines.

Environment:

    Kernel mode

Author:


--*/
#include "precomp.hpp"
#include "rxfrag.tmh"

CRxDefragTable::CRxDefragTable( _In_ CAdapter * pAdapter ) : m_pRxMgr( &pAdapter->m_RxMgr ), m_pAdapter( pAdapter )
{
    RtlZeroMemory( m_MSDUDefragArray, sizeof(m_MSDUDefragArray) );
}

NDIS_STATUS
CRxDefragTable::Initialize()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry( );
    ndisStatus = CNdisSpinLock::CreateInstance( &m_LockRxDefragTable );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError( "CRxDefragTable failed to create Lock" );
        goto exit;
    }

exit:
    TraceExit( ndisStatus );
    return ndisStatus;
}

void
_WDI_RX_DEFRAG_ENTRY::InsertFragment(USHORT FragNo, PNET_BUFFER_LIST pNbl)
{
    ASSERT(pNbl->Next == NULL);

    PNET_BUFFER_LIST * ppNblNext = &pNblFragmentChain;
    for (USHORT index = 0; index < FragNo; index++)
    {
        if ((1 << index) & FragmentsReceivedMask)
        {
            ppNblNext = &((*ppNblNext)->Next);
        }
    }

    pNbl->Next = *ppNblNext;
    *ppNblNext = pNbl;
    FragmentsReceivedMask |= (1 << FragNo);
}

PNET_BUFFER_LIST 
CRxDefragTable::FlushFragmentList(UCHAR index)
{
    PNET_BUFFER_LIST pResult = NULL;
    if (m_MSDUDefragArray[index].pNblFragmentChain)
    {
        pResult = m_MSDUDefragArray[index].pNblFragmentChain;
        RtlZeroMemory(&m_MSDUDefragArray[index], sizeof(m_MSDUDefragArray[index]));
    }
    else if (m_MSDUDefragArray[index].IsInUse())
    {
        RtlZeroMemory( &m_MSDUDefragArray[index], sizeof(m_MSDUDefragArray[index]) );
    }
    else
    {
        ASSERT(m_MSDUDefragArray[index].PortId == 0);
        ASSERT(m_MSDUDefragArray[index].PeerId == 0);
        ASSERT(m_MSDUDefragArray[index].SequenceNumber == 0);
        ASSERT(m_MSDUDefragArray[index].FragmentsReceivedMask == 0);
        ASSERT(m_MSDUDefragArray[index].StartTime == 0LL);
    }
    return pResult;
}

void
CRxDefragTable::RemovePeerFragments( WDI_PEER_ID PeerId )
{
    NblChain chainToReturn;

    TraceEntry();

    {
        CAutoLock lock( m_LockRxDefragTable );

        for (UCHAR index = 0; index < MAX_CONCURRENT_MSDU; index++)
        {
            if (m_MSDUDefragArray[index].IsInUse() && (m_MSDUDefragArray[index].PeerId == PeerId))
            {
                chainToReturn.AddChainTail( FlushFragmentList( index ) );
            }
        }
    }

    // Return the NBLs outside the lock
    if (chainToReturn.GetCount() > 0)
    {
        WFCInfo( "Returning %u MSDU fragments from peer id %u", chainToReturn.GetCount(), PeerId );
        m_pRxMgr->OnNdisReturnNetBufferLists( chainToReturn.GetHead(), 0 );
    }

    TraceExit( NDIS_STATUS_SUCCESS );
}

void
CRxDefragTable::RemovePortFragments( WDI_PORT_ID PortId )
{
    NblChain chainToReturn;

    TraceEntry( );

    {
        CAutoLock lock( m_LockRxDefragTable );

        for (UCHAR index = 0; index < MAX_CONCURRENT_MSDU; index++)
        {
            if (m_MSDUDefragArray[index].IsInUse() && (m_MSDUDefragArray[index].PortId == PortId))
            {
                chainToReturn.AddChainTail( FlushFragmentList( index ) );
            }
        }
    }

    // Return the NBLs outside the lock
    if (chainToReturn.GetCount( ) > 0)
    {
        WFCInfo( "Returning %u MSDU fragments from port id %u", chainToReturn.GetCount( ), PortId );
        m_pRxMgr->OnNdisReturnNetBufferLists( chainToReturn.GetHead( ), 0 );
    }

    TraceExit( NDIS_STATUS_SUCCESS );
}

NDIS_STATUS
CRxDefragTable::AddFragmentToEntry(
    _In_  PNET_BUFFER_LIST       pNbl,
    _In_  UCHAR                  index,
    _In_  USHORT                 FragmentNumber,
    _In_  BOOLEAN                bMoreFrag,
    _In_  BOOLEAN                bCopyIfKeep,
    _Out_ PNET_BUFFER_LIST     * pCompleteMSDUNblChain )
/*++
    Add pNbl to m_MSDUDefragArray[index] after doing basic checks
    for duplicates.  If !bMoreFrag and this completes the MSDU, the set
    pCompleteMSDUNblChain, and clear out the FragmentEntry so it is ready
    to be re-used.
--*/
{
    NDIS_STATUS ndisStatus;

    TraceEntry();

    *pCompleteMSDUNblChain = NULL;

    // check for a duplicate MPDU fragment
    if ((1 << FragmentNumber) & m_MSDUDefragArray[index].FragmentsReceivedMask)
    {
        WFCInfo("Duplicate Fragment Number %u, discarding second.", FragmentNumber);
        ndisStatus = NDIS_STATUS_NOT_ACCEPTED;
    }
    else
    {
        if (bMoreFrag)
        {
            if (bCopyIfKeep)
            {
                PNET_BUFFER_LIST pNblCopy = NULL;
                NT_ASSERT( NET_BUFFER_LIST_NEXT_NBL( pNbl ) == NULL );
                ndisStatus = m_pAdapter->CopyNetBufferLists( pNbl, &pNblCopy, 0, true);
                if (ndisStatus == NDIS_STATUS_SUCCESS)
                {
                    WFC_FRAME_FROM_WDI_FRAME( GetWiFiFrameMetadataFromNbl( pNblCopy ) )->u.Rx.Cloned = true;
                    pNbl = pNblCopy;
                    ndisStatus = NDIS_STATUS_PENDING;
                }
                else
                {
                    ndisStatus = NDIS_STATUS_NOT_COPIED;
                }
            }
            else
            {
                // More fragments remaining, so just save for later
                ndisStatus = NDIS_STATUS_NOT_COPIED;
            }
        }
        else
        {
            // Guilty until proven innocent
            ndisStatus = NDIS_STATUS_INVALID_PACKET;
        }

        // Insert the fragment into the chain (in the correct order)
        m_MSDUDefragArray[index].InsertFragment(FragmentNumber, pNbl);

        if (!bMoreFrag)
        {
            // The sender marked this as the last packet, so no matter what we're release all the fragments
            *pCompleteMSDUNblChain = FlushFragmentList( index );

            if (0 == ((m_MSDUDefragArray[index].FragmentsReceivedMask + 1) & m_MSDUDefragArray[index].FragmentsReceivedMask))
            {
                // If we've received all the previous fragments
                // Then it is time to indicate it up.
                ndisStatus = NDIS_STATUS_SUCCESS;
            }
            else
            {
                // If we got the last fragment before some of the others, drop everything
                WFCInfo( "Last Fragment Number %u, while missing previous (ReceivedMask: 0x%04x), discarding MSDU.", FragmentNumber, m_MSDUDefragArray[index].FragmentsReceivedMask );
                ndisStatus = NDIS_STATUS_INVALID_PACKET;
            }
        }
    }

    TraceExit(ndisStatus);
    return ndisStatus;
}

UCHAR
CRxDefragTable::ForceFreeFragmentEntry(
    _In_  WDI_PORT_ID           PortId,
    _Out_ PNET_BUFFER_LIST     * ppNblChain )
/*++
    Find the port that is using the most fragment entries.  At least 1 port must be using
    more than 3 fragment entries for us to be out of fragment entries.
    Force flush the oldest fragment entry for that port on the assumption that it will
    likely timeout anyway.

    Returns the index of the now free fragment entry.
--*/
{
    WDI_PORT_ID rgPorts[WFC_CONSTANT::MaximumNumberOfPorts] = { 0 };
    UCHAR rgCntFragments[WFC_CONSTANT::MaximumNumberOfPorts] = { 0 };
    UCHAR rgOldestFragIndex[WFC_CONSTANT::MaximumNumberOfPorts] = { 0 };
    ULONG maxPort = 0, maxPortIdx = 0;

    TraceEntry();

    // Build a histogram of fragments used by each port, and which
    // fragment is the oldest on that port, and which port has the
    // most fragments
    for (UCHAR index = 0; index < MAX_CONCURRENT_MSDU; index++)
    {
        ASSERT( m_MSDUDefragArray[index].IsInUse() );
        ULONG portIdx;
        bool fFoundPortIdx = false;
        for (portIdx = 0; portIdx < maxPort; portIdx++)
        {
            if (rgPorts[portIdx] == m_MSDUDefragArray[index].PortId)
            {
                fFoundPortIdx = true;
                break;
            }
        }
        if (!fFoundPortIdx)
        {
            ASSERT( maxPort < WFC_CONSTANT::MaximumNumberOfPorts );
            if (maxPort >= WFC_CONSTANT::MaximumNumberOfPorts)
            {
                // Don't buffer overrun just in case my logic is wrong
                // Instead just fail to free the 'best' fragments
                continue;
            }
            maxPort++;
            rgPorts[portIdx] = m_MSDUDefragArray[index].PortId;
            rgOldestFragIndex[portIdx] = index;
        }
        rgCntFragments[portIdx]++;
        if (rgCntFragments[portIdx] > rgCntFragments[maxPortIdx])
        {
            maxPortIdx = portIdx;
        }
        if (m_MSDUDefragArray[rgOldestFragIndex[portIdx]].StartTime > m_MSDUDefragArray[index].StartTime)
        {
            rgOldestFragIndex[portIdx] = index;
        }
    }

    WFCInfo( "Port %u has insufficient fragment entries, Port %u has %u, flushing oldest.",
        PortId, rgPorts[maxPortIdx], rgCntFragments[maxPortIdx] );

    ASSERT( rgPorts[maxPortIdx] != PortId );
    ASSERT( rgCntFragments[maxPortIdx] > 3 );
    *ppNblChain = FlushFragmentList( rgOldestFragIndex[maxPortIdx] );

    TraceExit( NDIS_STATUS_SUCCESS );
    return rgOldestFragIndex[maxPortIdx];
}

NDIS_STATUS
CRxDefragTable::AddFragment(
    _In_  PNET_BUFFER_LIST       pNbl,
    _In_  WDI_PORT_ID           PortId,
    _In_  WDI_PEER_ID           PeerId,
    _In_  DOT11_SEQUENCE_CONTROL SeqInfo,
    _In_  BOOLEAN                bMoreFrag,
    _In_  BOOLEAN                bCopyIfKeep,
    _Out_ PNET_BUFFER_LIST     * pCompleteMSDUNblChain)
/*++

    Search the defrag table.
    If a matching sequence number is found,
        add this fragment
        if this fragment completes the MSDU return it
    Flush any expired fragments.
    If no match is found, search for a free entry
    If no free entry is found and this port has 3 or more
        in progress MSDU fragments, then just disccard
        this new fragment
    If no free entry is found and this port has fewer than
        3 MSDUs that it's tracking, flush another port's
        excessive fragments (FIFO - assuming the older
        ones are most likely to be flushed due to timeout
        anyway).
    Add fragment to new free entry (making it no longer free)

    Returns a non-NULL pCompleteMSDUNblChain when a fragment is complete
    A returns value of NDIS_STATUS_SUCCESS means a fragment is complete
    A return value of NDIS_STATUS_PENDING means we saved the MPDU as part
    of an MSDU fragment, all other cases are errors.

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_FAILURE;
    BOOLEAN fFoundFree = FALSE, fFoundMatch = FALSE;
    UCHAR firstFreeIndex = 0, countThisPort = 0;
    const ULONGLONG CurrentTime = CSystem::get_CurrentTime();
    const ULONGLONG OldestUnexpiredTime = CurrentTime - MSDU_FRAGMENT_TIMEOUT;
    NblChain chainToReturn;

    TraceEntry( );

    *pCompleteMSDUNblChain = NULL;

    // Fragment 0 should never be the 'last' fragment
    ASSERT( bMoreFrag || (SeqInfo.FragmentNumber != 0) );

    {
        CAutoLock lock( m_LockRxDefragTable );

        for (UCHAR index = 0; index < MAX_CONCURRENT_MSDU; index++)
        {
            if (m_MSDUDefragArray[index].IsInUse() &&
                m_MSDUDefragArray[index].PortId == PortId &&
                m_MSDUDefragArray[index].PeerId == PeerId &&
                m_MSDUDefragArray[index].SequenceNumber == SeqInfo.SequenceNumber)
            {
                // There should only ever be one match
                NT_ASSERT( !fFoundMatch );

                // We found a matching MSDU set
                fFoundMatch = TRUE;

                // Don't early terminate this loop, so we can flush any expired MSDU fragments
                ndisStatus = AddFragmentToEntry( pNbl, index, SeqInfo.FragmentNumber, bMoreFrag, bCopyIfKeep, pCompleteMSDUNblChain );
            }
            // Flush any expired fragments
            else if (m_MSDUDefragArray[index].IsInUse() &&
                (m_MSDUDefragArray[index].StartTime < OldestUnexpiredTime))
            {
                chainToReturn.AddChainTail( FlushFragmentList( index ) );
            }

            // Keep track of the first free one we find, and how many this port is using
            if (!m_MSDUDefragArray[index].IsInUse())
            {
                if (!fFoundFree)
                {
                    fFoundFree = TRUE;
                    firstFreeIndex = index;
                }
            }
            else if (m_MSDUDefragArray[index].PortId == PortId)
            {
                countThisPort++;
            }
        }

        if (!fFoundMatch)
        {
            if (!fFoundFree && (countThisPort >= 3))
            {
                // Nothing free and this port is already using the required
                // minimum of 3, so reject this pack until something frees up
                WFCInfo( "Excessive MSDU Fragments (%u) and no free, discarding new fragment (Port = %u, SeqNo = %u).",
                    countThisPort, PortId, SeqInfo.SequenceNumber );
                ndisStatus = NDIS_STATUS_DATA_NOT_ACCEPTED;
            }
            else
            {
                if (!fFoundFree)
                {
                    PNET_BUFFER_LIST pNblChain = NULL;
                    // No match and no free slots, and some other port
                    // is hogging all the MSDU fragment buffers
                    // so go steal one back
                    firstFreeIndex = ForceFreeFragmentEntry( PortId, &pNblChain );
                    fFoundFree = TRUE;
                    chainToReturn.AddChainTail( pNblChain );
                }

                // We have a free fragment entry.  Set it up and add the fragment
                m_MSDUDefragArray[firstFreeIndex].PortId = PortId;
                m_MSDUDefragArray[firstFreeIndex].PeerId = PeerId;
                m_MSDUDefragArray[firstFreeIndex].SequenceNumber = SeqInfo.SequenceNumber;
                m_MSDUDefragArray[firstFreeIndex].StartTime = CurrentTime;
                ndisStatus = AddFragmentToEntry( pNbl, firstFreeIndex, SeqInfo.FragmentNumber, bMoreFrag, bCopyIfKeep, pCompleteMSDUNblChain );
            }
        }

        // The NBL was copied or was a duplicate fragment, so we can return the NBL back to LE
        if ((ndisStatus == NDIS_STATUS_PENDING) || (ndisStatus == NDIS_STATUS_NOT_ACCEPTED))
        {
            //  Release the frame back to LE outside of the lock
            chainToReturn.AddTail( pNbl );
        }
        // The fragment caused the whole set of fragments to get flushed
        else if (ndisStatus == NDIS_STATUS_INVALID_PACKET)
        {
            chainToReturn.AddChainTail( *pCompleteMSDUNblChain );
            *pCompleteMSDUNblChain = NULL;
        }
        // For fragments that are not copied, tell the caller it was pended
        else if (ndisStatus == NDIS_STATUS_NOT_COPIED)
        {
            ndisStatus = NDIS_STATUS_PENDING;
        }
    }

    // Return frames outside the lock
    if (chainToReturn.GetCount() > 0)
    {
        m_pRxMgr->OnNdisReturnNetBufferLists( chainToReturn.GetHead(), 0 );
    }

    TraceExit(ndisStatus);
    return ndisStatus;
}

