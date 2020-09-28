// MultiheadPresent.h: interface for the CMultiheadPresent class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "PresentII.h"

class CMultiheadPresent : public CPresentII  
{
public:
	CMultiheadPresent();
	virtual ~CMultiheadPresent();

protected:
	// virtual methods
	virtual char*	Name( char* szName );			// return current test name
	virtual void	InitPresentParameters();		// build the vector of presentparameters to test
	virtual void	InitTextures();					// build the vector of textures to test
	virtual void	InitPresentWindows();		// build the vector of Present windows to test
	virtual void	InitPresentSourceRect();	// build the vector of present source rects to test
	virtual void	InitPresentDestRect();		// build the vector of present dest rects to test
};
