// FullScreenPresent.h: interface for the CFullScreenPresent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FULLSCREENPRESENT_H__A0904D31_F0C7_4B99_B8F9_F0F842EA2759__INCLUDED_)
#define AFX_FULLSCREENPRESENT_H__A0904D31_F0C7_4B99_B8F9_F0F842EA2759__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PresentII.h"

class CFullScreenPresent : public CPresentII  
{
public:
	CFullScreenPresent();
	virtual ~CFullScreenPresent();

protected:
	// virtual methods
	virtual char*	Name( char* szName );			// return current test name
	virtual void	InitPresentParameters();		// build the vector of presentparameters to test
	virtual void	InitTextures();					// build the vector of textures to test
	virtual void	InitPresentWindows();		// build the vector of Present windows to test
	virtual void	InitPresentSourceRect();	// build the vector of present source rects to test
	virtual void	InitPresentDestRect();		// build the vector of present dest rects to test
};

#endif // !defined(AFX_FULLSCREENPRESENT_H__A0904D31_F0C7_4B99_B8F9_F0F842EA2759__INCLUDED_)
