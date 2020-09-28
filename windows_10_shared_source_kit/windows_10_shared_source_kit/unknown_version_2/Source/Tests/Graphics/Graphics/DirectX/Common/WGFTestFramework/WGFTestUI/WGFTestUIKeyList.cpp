/********************************************************************
	created:	2005/10/19
	filename: 	WGFTestUIKeyList.cpp
	author:		billkris
	
	purpose:	
*********************************************************************/

#include "pch.h"
#include "WGFTestUIKeyList.h"

LRESULT CWGFTestUIKeyListElementEditControl::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_GETDLGCODE:
		{
			// Let the parent handle VK_RETURN, VK_TAB, AND VK_ESCAPE
			if( GetParent() )
			{
				if( lParam )
				{
					MSG *pMsg = (MSG *) lParam;
					if( pMsg->message == WM_KEYDOWN && ( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_TAB || pMsg->wParam == VK_ESCAPE ) )
						return GetParent()->SendMessage( uMsg, wParam, lParam );
				}
			}
			
			break;
		}
	}
			
	return CWGFTestUIWindow::WindowProc( uMsg, wParam, lParam );
}

LRESULT CWGFTestUIKeyList::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static TCHAR buffer[4096];
	
	switch( uMsg )
	{
		case WM_CREATE:
			Init();
			break;
			
		case WM_GETDLGCODE:
		{
			if( lParam )
			{
				MSG *pMsg = (MSG *) lParam;
				
				switch( pMsg->message )
				{
					case WM_KEYDOWN:
					{
						switch( pMsg->wParam )
						{
							case VK_TAB:
							case VK_RETURN:
							{
								if( m_Edit.IsWindowVisible() )
								{
									static TCHAR buffer[4096];
									HWND hList = GetHWnd();
									int i = ListView_GetNextItem( hList, -1, LVNI_SELECTED );
									m_Edit.GetWindowText( buffer, sizeof( buffer) );
									
									LVITEM item;
									item.mask = LVIF_TEXT;
									item.pszText = buffer;
									item.iItem = i;
									item.iSubItem = 1;
									ListView_SetItem( hList, &item );
									
									m_Edit.ShowWindow( SW_HIDE );
									
									if( wParam == VK_RETURN )
										return DLGC_WANTMESSAGE;

									break;
								}
								else
								if( wParam == VK_TAB )
								{
									EditSelectionValue();
									return DLGC_WANTMESSAGE;
								}
								
								break;
							}
								
							case VK_ESCAPE:
							{
								if( m_Edit.IsWindowVisible() )
								{
									m_Edit.ShowWindow( SW_HIDE );
									return DLGC_WANTMESSAGE;
								}
								break;
							}
						}
					}
					
					break;
				}		
			}
			
			break;
		}			
		case WM_LBUTTONDBLCLK:
		{
			EditSelectionValue();
			break;
		}
	};
	
	return CWGFTestUIWindow::WindowProc( uMsg, wParam, lParam );
}


void CWGFTestUIKeyList::Init()
{
	// Initialize columns
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = _T( "Name" );
	lvc.iSubItem = 0;
	lvc.iOrder = 0;
	ListView_InsertColumn( GetHWnd(), 0, &lvc );
	
	lvc.pszText = _T( "Value" );
	lvc.iSubItem = 1;
	lvc.iOrder = 1;
	ListView_InsertColumn( GetHWnd(), 1, &lvc );

	// Turn on gridlines.
	ListView_SetExtendedListViewStyle( GetHWnd(), LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
}


void CWGFTestUIKeyList::EditSelectionValue()
{
	static TCHAR buffer[4096];
	int i = ListView_GetNextItem( GetHWnd(), -1, LVNI_SELECTED );
	
	if( i >= 0 )
	{
		RECT rc;
		ListView_GetSubItemRect( GetHWnd(), i, 1, LVIR_BOUNDS, &rc );
		ListView_GetItemText( GetHWnd(), i, 1, buffer, sizeof( buffer ) );
		
		if( m_Edit.GetHWnd() == NULL )
			m_Edit.CreateEx( 0, "Edit", NULL, WS_CHILD, 0, 0, 1, 1, this, NULL, 0 );
			
		// Copy the font used by the list control to the edit control
		HFONT hFont = (HFONT) SendMessage( WM_GETFONT, 0, 0 );
		m_Edit.SendMessage( WM_SETFONT, (WPARAM) hFont, 0 );
		
		m_Edit.SetWindowText( buffer );
		m_Edit.SetWindowPos( HWND_TOP, rc.left + 3, rc.top, rc.right - rc.left - 1, rc.bottom - rc.top - 1, SWP_SHOWWINDOW );
		m_Edit.SendMessage( EM_SETSEL, (WPARAM) 0, (LPARAM) -1 );
		SetFocus( m_Edit.GetHWnd() );
	}
}

