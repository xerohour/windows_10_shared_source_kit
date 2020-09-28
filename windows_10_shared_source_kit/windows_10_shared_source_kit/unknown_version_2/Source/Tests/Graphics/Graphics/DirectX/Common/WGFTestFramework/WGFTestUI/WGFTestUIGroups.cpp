#include "pch.h"

#include "resource.h"
#include "WGFTestUIGroups.h"

CWGFTestUIGroupsWindow::CWGFTestUIGroupsWindow() :
	m_bTempScript( false ),
	m_hGroupsMenu( NULL )
{
}


void CWGFTestUIGroupsWindow::Initialize()
{
	CWGFTestUIGroupTreeControl::Initialize();
	
	// Load menu for groups.
	m_hGroupsMenu = LoadMenu( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDR_MENU_GROUPS ) );
}


bool CWGFTestUIGroupsWindow::NotifyCallback( int idCtrl, LPNMHDR pnmhdr )
{
	switch( pnmhdr->code )
	{
		case TVN_SELCHANGED:
		{
			const CTestGroupPath &Path = PathFromCurSel();
			tstring str = Path.ToString();
			break;
		}
		
		case NM_RCLICK:
		{
			TVHITTESTINFO hti;
			POINT pt;
			GetCursorPos( &pt );
			hti.pt.x = pt.x;
			hti.pt.y = pt.y;
			ScreenToClient( &hti.pt );
			hti.flags = TVHT_ONITEM;
			HTREEITEM hItem = TreeView_HitTest( GetHWnd(), &hti );
			
			if( hItem && m_hGroupsMenu )
			{
				HMENU hMenu = GetSubMenu( m_hGroupsMenu, 0 );
				
				if( hMenu )
				{
					int id = (int) TrackPopupMenu( hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, m_hWnd, NULL );
					
					switch( id )
					{
						case ID_TESTGROUPS_EXECUTEBRANCH:
						{
							GetFramework()->PlayListClear();
							GetFramework()->PlayListAddBranch( PathFromItem( hItem ) );
							GetFramework()->GoToFirstTestCase();
							m_bTempScript = true;
							::SendMessage( GetShell()->GetMainHWnd(), WM_COMMAND, ID_EXECUTION_PLAY, 0 );
							break;
						}
					}
				}
			}
			break;
		}
	}
	
	return true; // Indicate that the notification was processed.
}

