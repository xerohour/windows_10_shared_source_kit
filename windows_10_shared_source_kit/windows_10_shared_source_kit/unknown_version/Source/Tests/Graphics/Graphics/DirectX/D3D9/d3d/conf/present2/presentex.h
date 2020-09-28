// PresentEx.h: interface for the CPresentEx class. LH extensions to Present
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "PresentII.h"

class CPresentEx : public CPresentII
{
public:
	CPresentEx();
	virtual ~CPresentEx();

	virtual bool CapsCheck();

protected:
	// virtual methods
	virtual char*	Name( char* szName );			// return current test name
	virtual void	InitPresentWindows();			// build the vector of Present windows to test
	virtual void	InitTextures();					// build the vector of textures to test
	virtual void	InitPresentParameters();		// build the vector of present parameters to test
	virtual void	InitPresentSourceRect();		// build the vector of present source rects to test
	virtual void	InitPresentDestRect();			// build the vector of present dest rects to test
	virtual void	InitFlagsEx();					// LONGHORN ONLY: build the vector of Ex flags to test
	virtual void	Present();						// implementation of Present (either using the device's shortcut or swapchain's method)
	virtual bool	ClearBackBuffers();				// true if the backbuffer(s) should be cleared
	virtual bool	UpdateBackBuffer();				// true if the backbuffer contents should be updated in the REF image
};

