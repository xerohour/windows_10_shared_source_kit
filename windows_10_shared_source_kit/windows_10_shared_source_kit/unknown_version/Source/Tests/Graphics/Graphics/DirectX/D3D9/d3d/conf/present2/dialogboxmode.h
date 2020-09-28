// DialogBoxMode.h: interface for the CDialogBoxMode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOGBOXMODE_H__2C4D5EBB_C60A_4B5F_B6BE_62B9B7E08570__INCLUDED_)
#define AFX_DIALOGBOXMODE_H__2C4D5EBB_C60A_4B5F_B6BE_62B9B7E08570__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PresentII.h"

class CDialogBoxMode : public CPresentII
{
    PVOID m_oldDropShadowState;

public:
	CDialogBoxMode();
	virtual ~CDialogBoxMode();

protected:
	// virtual methods
	virtual char*	Name( char* szName );		// return current test name
	virtual void	InitPresentParameters();	// build the vector of present parameters to test
	virtual void	InitPrivateData();			// build the vector of special data
	virtual void	InitPresentWindows();		// build the vector of Present windows to test
	virtual void	InitPresentSourceRect();	// build the vector of present source rects to test
	virtual void	InitPresentDestRect();		// build the vector of present dest rects to test
	virtual void	InitWindows();				// build the vector of dialog boxes to test
	virtual UINT	TestInitialize();			// Init the testcase
	virtual bool	PixelInsideDialogs( POINT Pixel );		// true if the pixel is inside Dialogs
	virtual void	PrivateDataProcessing();	// process private data case before rendering

	// helper functions
};

#endif // !defined(AFX_DIALOGBOXMODE_H__2C4D5EBB_C60A_4B5F_B6BE_62B9B7E08570__INCLUDED_)
