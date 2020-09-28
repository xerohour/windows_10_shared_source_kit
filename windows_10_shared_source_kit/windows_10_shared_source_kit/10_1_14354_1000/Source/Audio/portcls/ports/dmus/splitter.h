/*  Base definition of MIDI Transform Filter object

    Copyright (c) Microsoft.  All rights reserved.
    



*/

#ifndef __SplitterMXF_H__
#define __SplitterMXF_H__

#include "MXF.h"
#include "Allocatr.h"


#define kNumSinkMXFs    32

class CSplitterMXF : public CMXF,
    public IMXF,
    public CUnknown
{
public:
    CSplitterMXF(CAllocatorMXF *allocatorMXF, PMASTERCLOCK clock);
    ~CSplitterMXF(void);

    DECLARE_STD_UNKNOWN();
    IMP_IMXF;
private:
    PDMUS_KERNEL_EVENT  MakeDMKEvtCopy(PDMUS_KERNEL_EVENT pDMKEvt);


    PMXF            m_SinkMXF[kNumSinkMXFs];
    DWORD           m_SinkMXFBitMap;
    PMASTERCLOCK    m_Clock;
};

#endif  //  __SplitterMXF_H__
