// GammaPresent.h: interface for the CGammaPresent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMMAPRESENT_H__513A9AE4_14F1_49E2_B572_C378C00BBFC3__INCLUDED_)
#define AFX_GAMMAPRESENT_H__513A9AE4_14F1_49E2_B572_C378C00BBFC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PresentII.h"

class CGammaPresent : public CPresentII
{
public:
	CGammaPresent();
	virtual ~CGammaPresent();

protected:
	BOOL			m_bGammaSupported;					// true if the driver supports gamma correction
	FMT				m_lastBBFormat;						// last backbuffer format (they change with Reset)
	// virtual methods
	virtual char*	Name( char* szName );				// return current test name
	virtual void	InitPresentParameters();			// build the vector of present parameters to test
	virtual void	InitTextures();						// build the vector of textures to test
	virtual void	InitPresentSourceRect();			// build the vector of present source rects to test
	virtual void	InitPresentDestRect();				// build the vector of present dest rects to test
	virtual UINT	TestInitialize();					// Init the testcase
	virtual void	Present();							// implementation of Present (either using the device's shortcut or swapchain's method)
	virtual void	ProcessREFBuffer( UINT uSwapChain );// Do any postprocessing to the REF buffer (like gamma correct it)
};


#endif // !defined(AFX_GAMMAPRESENT_H__513A9AE4_14F1_49E2_B572_C378C00BBFC3__INCLUDED_)
