// ColorConverting.h: interface for the CColorConverting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORCONVERTING_H__1C4DDF5E_50FD_4C9E_8E75_75B8344F7335__INCLUDED_)
#define AFX_COLORCONVERTING_H__1C4DDF5E_50FD_4C9E_8E75_75B8344F7335__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PresentII.h"

class CColorConverting : public CPresentII
{
public:
	CColorConverting();
	virtual ~CColorConverting();

protected:
	// virtual methods
	virtual char*	Name( char* szName );				// return current test name
	virtual void	InitPresentParameters();			// build the vector of presentparameters to test
	virtual void	InitTextures();						// build the vector of textures to test
	virtual void	InitPresentSourceRect();			// build the vector of present source rects to test
	virtual void	InitPresentDestRect();				// build the vector of present dest rects to test
};

#endif // !defined(AFX_COLORCONVERTING_H__1C4DDF5E_50FD_4C9E_8E75_75B8344F7335__INCLUDED_)
