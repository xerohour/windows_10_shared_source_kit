/*  
    Base definition of MIDI Transform Filter object 

    Copyright (c) Microsoft.  All rights reserved.






*/

#ifndef __AllocatorMXF_H__
#define __AllocatorMXF_H__

#include "MXF.h"


#define kMXFBufferSize  240
//  WDMAud currently sends down 50 capture IRPs (12 bytes each), DMusic sends down 32 (of 20).


#define kNumPtrsPerPage     (PAGE_SIZE / sizeof(PVOID))
#define kNumEvtsPerPage     (PAGE_SIZE / sizeof(DMUS_KERNEL_EVENT))

class CAllocatorMXF 
:   public CMXF,
    public IAllocatorMXF,
    public CUnknown
{
public:
    DECLARE_STD_UNKNOWN();
    IMP_IAllocatorMXF;

    CAllocatorMXF(PPOSITIONNOTIFY BytePositionNotify);
    ~CAllocatorMXF(void);
    
private:
    ULONG               m_NumFreeEvents;
    ULONG               m_NumPages;
    PVOID               m_pPages;
    PDMUS_KERNEL_EVENT  m_pEventList;
    KSPIN_LOCK          m_EventLock;            // protects the free list
    PPOSITIONNOTIFY     m_BytePositionNotify;

    void     CheckEventLowWaterMark(void);
    void     CheckEventHighWaterMark(void);
    BOOL     AddPage(PVOID *pPool, PVOID pPage);
    void     DestructorFreeBuffers(void);
    void     DestroyPages(PVOID pPages);
    NTSTATUS FreeBuffers(PDMUS_KERNEL_EVENT  pDMKEvt);
    void     MakeNewEvents(void);
};

#endif  //  __AllocatorMXF_H__
