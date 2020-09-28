// DeviceClip.h: interface for the CDeviceClip class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "DeviceClip.h"

class CDeviceClipFlipEx : public CDeviceClip
{
public:
	CDeviceClipFlipEx();
	virtual ~CDeviceClipFlipEx();

protected:
    // virtual methods
    virtual bool    CapsCheck();
	virtual void	InitPresentParameters();	// build the vector of present parameters to test
    virtual void    InitPresentSourceRect();    // build the vector of present source rects to test
    virtual void    InitPresentDestRect();      // build the vector of present dest rects to test
    virtual void	InitDirtyRegions();			// build the vector of regions to test
    virtual void    InitPresentWindows();

};
