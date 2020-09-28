// ClippingWindows.h: interface for the CClippingWindows class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIPPINGWINDOWS_H__D165BFDF_7CCF_42C8_BD6E_07E6C8E7E60D__INCLUDED_)
#define AFX_CLIPPINGWINDOWS_H__D165BFDF_7CCF_42C8_BD6E_07E6C8E7E60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PresentII.h"

class CClippingWindows : public CPresentII
{
public:
	CClippingWindows();
	virtual ~CClippingWindows();

protected:
	// virtual methods
	virtual char*	Name( char* szName );		// return current test name
	virtual UINT TestInitialize();				// Init the testcase
	virtual void	InitPresentSourceRect();	// build the vector of present source rects to test
	virtual void	InitWindows();				// build the vector of clip windows
	virtual void	InitDirtyRegions();			// build the vector of regions to test

	// helper functions
#ifdef FAST_TEST
	virtual void	InitPresentDestRect();		// build the vector of present dest rects to test
#endif
};

#endif // !defined(AFX_CLIPPINGWINDOWS_H__D165BFDF_7CCF_42C8_BD6E_07E6C8E7E60D__INCLUDED_)
