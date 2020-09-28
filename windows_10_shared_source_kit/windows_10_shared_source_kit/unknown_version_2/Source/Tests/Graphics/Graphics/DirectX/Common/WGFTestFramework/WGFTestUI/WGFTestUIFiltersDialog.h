//////////////////////////////////////////////////////////////////////////
//  WGFTestUIFiltersDialog.h
//  created:	2005/01/05
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "WGFTestUIWindow.h"

class CWGFTestUIEditFilterDialog : public CWGFTestUIDialog
{
public:
	INT_PTR Execute( CWGFTestUIWindow *pParent, bool bNew );
	
public:
	tstring m_strName;
	tstring m_strValue;
	
private:
	bool OnInitDialog( WPARAM wParam, LPARAM lParam );
	INT_PTR DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
};


class CWGFTestUIFiltersDialog : public CWGFTestUIDialog
{
public:
	INT_PTR Execute( CWGFTestUIWindow *pParent );
	
private:
	bool OnInitDialog();
	void UpdateList();
	INT_PTR DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	CStringKeys m_Keys;

private:
	CWGFTestUIEditFilterDialog m_EditFilterDlg;	
};


