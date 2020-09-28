// Present.h: interface for the CPresent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRESENT_H__B559714E_BC95_4AAA_8F96_86B0D9D58292__INCLUDED_)
#define AFX_PRESENT_H__B559714E_BC95_4AAA_8F96_86B0D9D58292__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PresentII.h"

class CPresent : public CPresentII
{
public:
	CPresent();
	virtual ~CPresent();

protected:
	// virtual methods
	virtual char*	Name( char* szName );			// return current test name
	virtual void	InitPresentParameters();		// build the vector of presentparameters to test
	virtual void	InitPresentWindows();			// build the vector of Present windows to test
	virtual void	InitTextures();					// build the vector of textures to test
};

// Splitting up testing of different present rects and presenting to different types of windows when doing FAST_TEST variations
#ifdef FAST_TEST
class CWindowPresent : public CPresentII
{
public:
	CWindowPresent();
	virtual ~CWindowPresent();

protected:
	// virtual methods
	virtual char*	Name(char* szName);			// return current test name
	virtual void	InitPresentParameters();		// build the vector of presentparameters to test
	virtual void	InitPresentWindows();			// build the vector of Present windows to test
	virtual void	InitTextures();					// build the vector of textures to test
	virtual void    InitPresentSourceRect();         // build the vector of present source rects to test
	virtual void    InitPresentDestRect();           // build the vector of present dest rects to test
};
#endif FAST_TEST


#endif // !defined(AFX_PRESENT_H__B559714E_BC95_4AAA_8F96_86B0D9D58292__INCLUDED_)
