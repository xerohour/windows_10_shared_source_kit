#pragma once

#include "WGFTestUIWindow.h"

class CWGFTestUIHelpTreeControl : public CWGFTestUIGroupTreeControl
{
public:
	void Initialize();
};

class CWGFTestUIHelpDialog : public CWGFTestUIDialog
{
private:
	CWGFTestUIHelpTreeControl m_Groups;
	CWGFTestUIRichEditControl m_RichEdit;

	void GeneralHelp();
	void TestParamHelp();
	
public:	
	INT_PTR Execute( CWGFTestUIWindow *pParent );
	
	INT_PTR DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
};