//////////////////////////////////////////////////////////////////////////
//  WGFTestUIOptionsDialog.h
//  created:	2005/01/05
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "WGFTestUIKeyList.h"

class CWGFTestUIOptionsDialog : public CWGFTestUIDialog
{
public:
	INT_PTR Execute( CWGFTestUIWindow *pParent );
	
private:
	bool OnInitDialog();
	void UpdateKeys();
	INT_PTR DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	CStringKeys m_Keys;

private:
	CWGFTestUIKeyList m_List;
};


