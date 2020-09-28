// PresentRotate.h: interface for the CRotatePresent class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "PresentII.h"

class CRotatePresent : public CPresentII  
{
public:
	CRotatePresent();
	virtual ~CRotatePresent();

protected:
	vector< DEVMODE > m_vecDevModes;

	// virtual methods
	virtual char*	Name( char* szName );		// return current test name
	virtual void	InitPresentParameters();	// build the vector of presentparameters to test
	virtual void	InitTextures();				// build the vector of textures to test
	virtual void	InitPresentWindows();		// build the vector of Present windows to test
	virtual void	InitPresentSourceRect();	// build the vector of present source rects to test
	virtual void	InitPresentDestRect();		// build the vector of present dest rects to test
	virtual void	ChangeDisplaySettings();	// change the mode if necessary
};

