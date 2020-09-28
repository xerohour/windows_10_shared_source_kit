/********************************************************************
	created:	2005/10/19
	filename: 	WGFTestUIKeyList.h
	author:		billkris
	
	purpose:	
*********************************************************************/
#pragma once

#include "WGFTestUIWindow.h"

class CWGFTestUIKeyListElementEditControl : public CWGFTestUIWindow
{
public:
	LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
};


class CWGFTestUIKeyList : public CWGFTestUIWindow
{
public:
	void Init();
	
	LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	
private:
	void EditSelectionValue();

private:
	CWGFTestUIKeyListElementEditControl m_Edit;
};



