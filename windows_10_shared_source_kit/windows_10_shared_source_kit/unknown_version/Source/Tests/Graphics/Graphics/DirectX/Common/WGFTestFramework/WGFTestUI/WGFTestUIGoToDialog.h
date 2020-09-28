#pragma once

#include "WGFTestUIWindow.h"

class CWGFTestUIGoToDialog : public CWGFTestUIDialog
{
public:
	CWGFTestUIGoToDialog();
	INT_PTR Execute( CWGFTestUIWindow *pParent );
	
	INT_PTR DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	
	size_t m_nCaseNumber;
	CTestGroupPath m_Path;
};