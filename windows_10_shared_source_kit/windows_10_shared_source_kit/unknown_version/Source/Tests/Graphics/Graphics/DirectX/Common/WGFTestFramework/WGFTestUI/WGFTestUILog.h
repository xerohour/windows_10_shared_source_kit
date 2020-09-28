#pragma once

#include "WGFTestUIWindow.h"


class CWGFTestUILogWindow : public CWGFTestUIWindow
{
	public:
		CWGFTestUILogWindow()
		{
		};

		bool Create( UINT uID, CWGFTestUIWindow *pParent );
		LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
		
		void WriteText( const TCHAR *szText );
		void Clear();
};


