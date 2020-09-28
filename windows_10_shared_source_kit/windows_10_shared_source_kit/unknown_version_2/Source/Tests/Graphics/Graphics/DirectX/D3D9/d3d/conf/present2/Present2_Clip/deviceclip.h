// DeviceClip.h: interface for the CDeviceClip class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "PresentII.h"

class CDeviceClip : public CPresentII
{
public:
	CDeviceClip();
	virtual ~CDeviceClip();
	virtual bool CapsCheck();

protected:
	UINT m_nLastIndexPrivateData;
    bool m_IsTaskBarAlwaysOnTop;

	// virtual methods
	virtual UINT TestInitialize();				// Init the testcase
    virtual bool TestTerminate();
	virtual char*	Name( char* szName );		// return current test name
	virtual void	InitPresentParameters();	// build the vector of present parameters to test
	virtual void	InitPresentDestRect();		// build the vector of present dest rects to test
	virtual void	InitPresentSourceRect();	// build the vector of present source rects to test
	virtual void	InitDirtyRegions();			// build the vector of regions to test
	virtual void	InitTextures();				// build the vector of textures to test
	virtual void	InitPrivateData();			// build the vector of special data
	virtual void	PrivateDataProcessing();	// process private data case before rendering
	virtual void	Present();					// implementation of Present (either using the device's shortcut or swapchain's method)
	// helper functions
};
