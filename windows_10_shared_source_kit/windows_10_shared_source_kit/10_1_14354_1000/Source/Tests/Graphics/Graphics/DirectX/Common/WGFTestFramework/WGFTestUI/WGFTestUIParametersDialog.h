/********************************************************************
	created:	2005/10/20
	filename: 	WGFTestUIParametersDialog.h
	author:		billkris
	
	purpose:	
*********************************************************************/

#pragma once

#include "WGFTestUIKeyList.h"

class CWGFTestUIParametersDialog : public CWGFTestUIDialog
{
public:
	void Init( CTest *pTest );
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


