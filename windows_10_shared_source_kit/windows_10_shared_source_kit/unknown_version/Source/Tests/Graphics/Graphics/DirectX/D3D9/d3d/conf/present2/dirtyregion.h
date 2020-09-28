// DirtyRegion.h: interface for the CDirtyRegion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRTYREGION_H__695EB08A_7A9C_4B3F_8C6B_D7E8ECF177D4__INCLUDED_)
#define AFX_DIRTYREGION_H__695EB08A_7A9C_4B3F_8C6B_D7E8ECF177D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PresentII.h"

class CDirtyRegion : public CPresentII
{
public:
	CDirtyRegion();
	virtual ~CDirtyRegion();

protected:
	// virtual methods
	virtual char*	Name( char* szName );		// return current test name
	virtual void	InitPresentParameters();	// build the vector of present parameters to test
	virtual void	InitPresentDestRect();		// build the vector of present dest rects to test
	virtual void	InitDirtyRegions();			// build the vector of regions to test
	// helper functions

#ifdef FAST_TEST
	virtual void    InitPresentSourceRect();         // build the vector of present source rects to test
#endif
};

#endif // !defined(AFX_DIRTYREGION_H__695EB08A_7A9C_4B3F_8C6B_D7E8ECF177D4__INCLUDED_)
