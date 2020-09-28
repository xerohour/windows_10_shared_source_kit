// DirtyRegionFlipEx.h: interface for the CDirtyRegionFlipEx class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "DirtyRegion.h"

class CDirtyRegionFlipEx : public CDirtyRegion
{
public:
	CDirtyRegionFlipEx();
	virtual ~CDirtyRegionFlipEx();

protected:
    // virtual methods
    virtual bool    CapsCheck();
	virtual void	InitPresentParameters();	// build the vector of present parameters to test
    virtual void    InitPresentSourceRect();            // build the vector of present source rects to test
    virtual void    InitPresentDestRect();              // build the vector of present dest rects to test
    virtual void    InitPresentWindows();               // build the vector of present windowsto test
    virtual void    InitDirtyRegions();


};

