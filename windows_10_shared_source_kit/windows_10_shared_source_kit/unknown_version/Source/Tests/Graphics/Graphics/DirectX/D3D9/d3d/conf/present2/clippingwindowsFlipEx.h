// ClippingWindows.h: interface for the CClippingWindows class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "ClippingWindows.h"

class CClippingWindowsFlipEx : public CClippingWindows
{
public:
	CClippingWindowsFlipEx();
	virtual ~CClippingWindowsFlipEx();

protected:
	// virtual methods
    virtual bool    CapsCheck();
	virtual void	InitPresentParameters();	        // build the vector of present parameters to test
    virtual void    InitPresentSourceRect();            // build the vector of present source rects to test
    virtual void    InitPresentDestRect();              // build the vector of present dest rects to test
    virtual void    InitPresentWindows();               // build the vector of present windowsto test
    virtual void	InitDirtyRegions();			// build the vector of regions to test

};
