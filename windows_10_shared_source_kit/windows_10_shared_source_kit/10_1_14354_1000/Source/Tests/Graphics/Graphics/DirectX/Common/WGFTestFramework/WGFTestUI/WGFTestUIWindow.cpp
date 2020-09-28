#include "pch.h"
#include "resource.h"

#include "WGFTestUIWindow.h"

ATOM __stdcall WGFTestUIRegisterWndClass( LPCTSTR szClassName, UINT uStyle, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground )
{
	WNDCLASSEX wc;
	memset( &wc, 0, sizeof( wc ) );
	wc.cbSize = sizeof( wc );
	wc.style = uStyle;
	wc.hbrBackground = hbrBackground;
	wc.hCursor = hCursor;
	wc.hIcon = hIcon;
	wc.hInstance = GetModuleHandle( NULL );
	wc.lpfnWndProc = DefWindowProc;
	wc.lpszClassName = szClassName;
	
	return RegisterClassEx( &wc );
}

LRESULT CALLBACK CWGFTestUIWindow::ProxyWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CWGFTestUIWindow *pWnd = reinterpret_cast< CWGFTestUIWindow *>( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
	
	if( pWnd )
	{
		LRESULT rval =  pWnd->WindowProc( uMsg, wParam, lParam );
		
		if( uMsg == WM_DESTROY )
		{
			pWnd->m_hWnd = NULL;
		}
		
		return rval;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

CWGFTestUIWindow *CWGFTestUIWindow::FromHWnd( HWND hWnd )
{
	if( hWnd == NULL )
		return NULL;
		
	static CWGFTestUIWindow temp;
	CWGFTestUIWindow *pWindow = (CWGFTestUIWindow *) GetWindowLongPtr( hWnd, GWLP_USERDATA );
	
	if( pWindow == NULL )
	{
		// Create a temporary CWGFTestUIWindow object
		temp.m_hWnd = hWnd;
		pWindow = &temp;
	}
	
	return pWindow;
}


CWGFTestUIWindow::CWGFTestUIWindow() : 
	m_hWnd( NULL ),
	m_pDefaultWndProc( NULL )
{
}

CWGFTestUIWindow::~CWGFTestUIWindow()
{
	Detach();
}

bool CWGFTestUIWindow::Attach( HWND hWnd )
{
	// Detach from current window
	Detach();
	
	if( GetWindowLongPtr( hWnd, GWLP_USERDATA ) != 0 )
		return false; // User data already taken.
		
	// Set the user data to point to this.
	SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR) this );
	
	// Override the window proc (subclass the window).
	m_pDefaultWndProc = (WNDPROC) SetWindowLongPtr( hWnd, GWLP_WNDPROC, (LONG_PTR) CWGFTestUIWindow::ProxyWindowProc );
	
	m_hWnd = hWnd;
	
	return true;
}

HWND CWGFTestUIWindow::Detach()
{
	HWND hWnd = m_hWnd;
	
	if( hWnd )
	{
		// Set the user data to NULL
		SetWindowLongPtr( m_hWnd, GWLP_USERDATA, 0 );
		
		// Restore the window proc
		if( m_pDefaultWndProc )
			SetWindowLongPtr( m_hWnd, GWLP_WNDPROC, (LONG_PTR) m_pDefaultWndProc );
		m_hWnd = NULL;
		m_pDefaultWndProc = NULL;
	}
	
	return hWnd;
}

// A CallWndProc hook function is used to guarantee that the wrapper class pointer is
// stored in the window's USERDATA value prior to WM_CREATE.  This allow wrapping of 
// standard window classes such as buttons and toolbars.
HHOOK CWGFTestUIWindow::s_hhook = NULL;
CWGFTestUIWindow *CWGFTestUIWindow::s_pHookWindow = NULL;
LRESULT CALLBACK CWGFTestUIWindow::WndProcHookFn( int nCode, WPARAM wParam, LPARAM lParam )
{
	if( nCode == HC_ACTION )
	{
		CWPSTRUCT *pcwps = (CWPSTRUCT *) lParam;
		
		if( s_pHookWindow && pcwps->hwnd && IsWindow( pcwps->hwnd ) )
		{
			s_pHookWindow->Attach( pcwps->hwnd );
			
			UnhookWindowsHookEx( s_hhook );
			s_hhook = NULL;
			s_pHookWindow = NULL;
		}
	}
	
	return CallNextHookEx( s_hhook, nCode, wParam, lParam );
}

CWGFTestUIWindow *CWGFTestUIWindow::GetParent()
{
	CWGFTestUIWindow *pParent = NULL;
	
	HWND hWndParent = ::GetParent( m_hWnd );
	
	if( hWndParent )
	{
		pParent = FromHWnd( hWndParent );	
	}
	
	return pParent;
}

CWGFTestUIWindow *CWGFTestUIWindow::GetAncestor( UINT gaFlags )
{
	CWGFTestUIWindow *pAncestor = NULL;
	
	HWND hWndAncestor = ::GetAncestor( m_hWnd, gaFlags );
	
	if( hWndAncestor )
	{
		pAncestor = FromHWnd( hWndAncestor );	
	}
	
	return pAncestor;
}

bool CWGFTestUIWindow::CreateEx(	DWORD dwExStyle, 
								LPCTSTR lpszClassName, 
								LPCTSTR lpszWindowName,
								DWORD dwStyle,
								int x,
								int y, 
								int nWidth,
								int nHeight, 
								CWGFTestUIWindow *pParent,
								HMENU hMenu,
								LPVOID lParam
								)
{
	HWND hWndParent = pParent ? pParent->GetHWnd() : NULL;
	
	// BUGBUG - TEST WINDOW PROC HOOK
	s_hhook = SetWindowsHookEx( WH_CALLWNDPROC, CWGFTestUIWindow::WndProcHookFn, NULL, GetCurrentThreadId() );
	s_pHookWindow = this;
	
	HWND hWnd = CreateWindowEx( dwExStyle, lpszClassName, lpszWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, GetModuleHandle( NULL ), lParam );
	if( hWnd )
	{
		m_hWnd = hWnd;
		return true;
	}
	
	return false;
}

void CWGFTestUIWindow::DestroyWindow()
{
	if( m_hWnd )
	{
		::DestroyWindow( m_hWnd );
		m_hWnd = NULL;
	}
}

LRESULT CWGFTestUIWindow::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
			
			if( lpdis->hwndItem )
			{
				CWGFTestUIWindow *pChild = FromHWnd( lpdis->hwndItem );
				if( pChild )
				{
					// Give child windows an opportunity to draw themselves if they are owner drawn.
					
					if( pChild->DrawItemCallback( this, lpdis ) )
						return 0;
				}
			}
			
			break;
		}
		
		case WM_NOTIFY:
		{
			LPNMHDR pnmh = (LPNMHDR) lParam;
			int idCtrl = (int) wParam;
			
			if( pnmh->hwndFrom )
			{
				CWGFTestUIWindow *pChild = FromHWnd( pnmh->hwndFrom );
				if( pChild )
				{
					if( pChild->NotifyCallback( idCtrl, pnmh ) )
						return 0;
				}
			}
			
			break;
		}
	}
	
	return m_pDefaultWndProc ? CallWindowProc( m_pDefaultWndProc, m_hWnd, uMsg, wParam, lParam ) : DefWindowProc( m_hWnd, uMsg, wParam, lParam );
}

int __stdcall WGFTestUIDrawText( HDC hDC, LPCTSTR szText, int nCount, LPRECT lpRect, UINT uFormat )
{
	int rVal = 0;
	int n = nCount;
	TCHAR *c = NULL;
	COLORREF crRestore = GetTextColor( hDC );
	COLORREF cr = crRestore;
	//int bm = SetBkMode( hDC, TRANSPARENT );
	bool bColor = false;
	bool bRestore = false;
	RECT rc;

	rc.left = lpRect->left;
	rc.top = lpRect->top;
	rc.right = lpRect->right;
	rc.bottom = lpRect->bottom;
	
	if( szText )
	{	
		// Find the first occurance of '$'
		for( ; *szText; )
		{
			c = _tcschr( szText, '$' );
			
			n = c ? int( ptrdiff_t( c - szText ) ) + ( c[1] == '$' ? 1 : 0 ) : -1;
			
			// Set the text color.
			SetTextColor( hDC, cr );
			
			// Draw the text and update the rectangle.
			rVal = DrawText( hDC, szText, n, &rc, uFormat );
				
			bColor = false;

			if( !c )
				break;
				
			SIZE size;
			GetTextExtentPoint( hDC, szText, n, &size );
			rc.left += size.cx;
			if( rc.left > rc.right )
				break;
					
			// Parse the color descriptor
			++c;
			
			bColor = true;
			bRestore = true;
			
			switch( *c )
			{
				// BUGBUG - TODO: Special case custom RGB value
				//case '('
				//	break;
					
				case 'R':
					cr = RGB( 255, 0, 0 );
					++c;
					break;
					
				case 'G':
					cr = RGB( 0, 255, 0 );
					++c;
					break;
					
				case 'B':
					cr = RGB( 0, 0, 255 );
					++c;
					break;
					
				case 'r':
					cr = RGB( 127, 0, 0 );
					++c;
					break;
					
				case 'g':
					cr = RGB( 0, 127, 0 );
					++c;
					break;
					
				case 'b':
					cr = RGB( 0, 0, 127 );
					++c;
					break;
					
				case 'C':
					cr = RGB( 0, 255, 255 );
					++c;
					break;
					
				case 'M':
					cr = RGB( 255, 0, 255 );
					++c;
					break;
					
				case 'Y':
					cr = RGB( 255, 255, 0 );
					++c;
					break;
				
				case 'c':
					cr = RGB( 0, 127, 127 );
					++c;
					break;
					
				case 'm':
					cr = RGB( 127, 0, 127 );
					++c;
					break;
					
				case 'y':
					cr = RGB( 127, 127, 0 );
					++c;
					break;
				
				case 'W':
					cr = RGB( 255, 255, 255 );
					++c;
					break;
					
				case 'w':
					cr = RGB( 192, 192, 192 );
					++c;
					break;
					
				case 'K':
				case 'k':
					cr = RGB( 0, 0, 0 );
					++c;
					break;
					
				case 'L':
				case 'l':
					cr = RGB( 127, 127, 127 );
					++c;
					break;
					
				case 'D':
				case 'd':
					cr = RGB( 95, 95, 95 );
					++c;
					break;
					
				case 'x':					
					++c;

				default:
					bRestore = false;
					cr = crRestore;
					break;									
			}
						
			// Point to the next character.
			szText = c;
		}
	}
	
	if( bRestore )
		SetTextColor( hDC, crRestore );
		
	//SetBkMode( hDC, bm );
		
	return rVal;
}



static CWGFTestUIDialog *s_pInitDlg = NULL;

INT_PTR CALLBACK CWGFTestUIDialog::ProxyDialogProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CWGFTestUIDialog *pWnd = (CWGFTestUIDialog *) GetWindowLongPtr( hWndDlg, GWLP_USERDATA );
	
	switch( uMsg )
	{
		case WM_INITDIALOG:
			pWnd = s_pInitDlg;
			s_pInitDlg = NULL;
			pWnd->Attach( hWndDlg );
			break;
	}
	
	if( pWnd )
		return pWnd->DialogProc( uMsg, wParam, lParam );
		
	return false;
}

bool CWGFTestUIDialog::Create( HINSTANCE hInstance, LPCTSTR lpTemplate, CWGFTestUIWindow *pParent )
{
	s_pInitDlg = this;
	HWND hWnd = CreateDialog( hInstance, lpTemplate, pParent ? pParent->GetHWnd() : NULL, CWGFTestUIDialog::ProxyDialogProc );
	
	return hWnd != NULL;
}

bool CWGFTestUIDialog::CreateParam( HINSTANCE hInstance, LPCTSTR lpTemplate, CWGFTestUIWindow *pParent, LPARAM lParam )
{
	s_pInitDlg = this;
	HWND hWnd = CreateDialogParam( hInstance, lpTemplate, pParent ? pParent->GetHWnd() : NULL, CWGFTestUIDialog::ProxyDialogProc, lParam );
	
	return hWnd != NULL;
}

INT_PTR CWGFTestUIDialog::DoModal( HINSTANCE hInstance, LPCTSTR lpTemplate, CWGFTestUIWindow *pParent )
{
	s_pInitDlg = this;
	return DialogBox( hInstance, lpTemplate, pParent ? pParent->GetHWnd() : NULL, CWGFTestUIDialog::ProxyDialogProc );
}

INT_PTR CWGFTestUIDialog::DoModalParam( HINSTANCE hInstance, LPCTSTR lpTemplate, CWGFTestUIWindow *pParent, LPARAM lParam )
{
	s_pInitDlg = this;
	return DialogBoxParam( hInstance, lpTemplate, pParent ? pParent->GetHWnd() : NULL, CWGFTestUIDialog::ProxyDialogProc, lParam );
}

bool CWGFTestUIToolbar::Create( UINT uID, CWGFTestUIWindow *pParent, DWORD dwTBStyle )
{
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof( icex );
	icex.dwICC = ICC_BAR_CLASSES;
	InitCommonControlsEx( &icex );
	
	if( !CWGFTestUIWindow::CreateEx( 0, TOOLBARCLASSNAME, NULL, WS_CHILD | CCS_ADJUSTABLE | dwTBStyle, 0, 0, 0, 0, pParent, (HMENU) (INT_PTR) uID, 0 ) )
		return false;
	
	SendMessage( TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof( TBBUTTON ), 0 );
	
	return true;
}

bool CWGFTestUIToolbar::CreateSimpleBitmapButtons( UINT uID, CWGFTestUIWindow *pParent, int *pButtonIDs, DWORD dwTBStyle )
{
	int nButtons;
	TBBUTTON *pButtons = NULL;
	HINSTANCE hInstance = GetModuleHandle( NULL );
	
	// Count the number of buttons
	for( nButtons = 0; pButtonIDs[nButtons]; ++nButtons )
	{
		// BUGBUG - Should we have a maximum button count to handle
		// forgotten NULL terminator?
	}
	
	if( !Create( uID, pParent, dwTBStyle | TBSTYLE_LIST | TBSTYLE_TOOLTIPS ) )
		goto fail;
	
	TBADDBITMAP tbab;
	tbab.hInst = hInstance;
	tbab.nID = (UINT_PTR) uID; // Use the id of this toolbar to load the bitmap.
		
	if( -1 == AddBitmap( nButtons, &tbab ) )
		goto fail;
		
	SetExtendedStyle( TBSTYLE_EX_MIXEDBUTTONS );
	
	pButtons = new TBBUTTON[nButtons];
	if( !pButtons ) // Should never happen if using thrownew.obj
		return false;
		
	memset( pButtons, 0, nButtons * sizeof( TBBUTTON ) );
/*	
	// Add a string resource using the same id as this toolbar.	
	TCHAR buffer[256];
	int nChars = LoadString( hInstance, uID, buffer, sizeof( buffer ) - 1 );
	buffer[nChars + 1] = 0; // Double NULL terminate.
	
	int iFirstString = AddStrings( buffer );
*/
	int index = 0;
	
	for( int i = 0; i < nButtons; ++i )
	{
		if( pButtonIDs[i] == -1 )
		{
			pButtons[i].fsStyle = TBSTYLE_SEP;
		}
		else
		{
			static TCHAR buffer[256];
			LoadString( hInstance, pButtonIDs[i], buffer, sizeof( buffer ) );
			buffer[sizeof(buffer) - 1] = 0;
			int iString = AddString( buffer );
			pButtons[i].iBitmap = index;
			pButtons[i].idCommand = pButtonIDs[i];
			pButtons[i].fsState = TBSTATE_ENABLED;
			pButtons[i].fsStyle = BTNS_BUTTON;
			pButtons[i].dwData = 0;
			pButtons[i].iString = iString;
			
			++index;
		}
	}
		
	if( !AddButtons( nButtons, pButtons ) )
		goto fail;
		
	delete[]( pButtons );
		
	return true;
	
fail:
	delete[]( pButtons );
	
	DestroyWindow();
	
	return false;
}


//////////////////////////////////////////////////////////////////////////
// CWGFTestUIStatusBar
//////////////////////////////////////////////////////////////////////////
bool CWGFTestUIStatusBar::Create( UINT uID, CWGFTestUIWindow *pParent, DWORD dwStyle, LPCTSTR lpszText )
{
	Detach();
	
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof( icc );
	icc.dwICC = ICC_BAR_CLASSES;
	
	InitCommonControlsEx( &icc );
	
	if( !CreateEx( 0, STATUSCLASSNAME, lpszText, WS_CHILD | dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, pParent, (HMENU) (INT_PTR) uID, 0 ) )
		goto fail;
	
	return true;
		
fail:
	DestroyWindow();
	
	return false;
}

//////////////////////////////////////////////////////////////////////////
// CWGFTestUITabControl
//////////////////////////////////////////////////////////////////////////
bool CWGFTestUITabControl::Create( UINT uID, CWGFTestUIWindow *pParent, DWORD dwStyle , int x, int y, int width, int height )
{
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof( iccx );
	iccx.dwICC = ICC_TAB_CLASSES;
	InitCommonControlsEx( &iccx );
	
	if( !CreateEx( 0, WC_TABCONTROL, NULL, dwStyle, x, y, width, height, pParent, (HMENU) (INT_PTR) uID, 0 ) )
		return false;
		
	m_hFont = CreateFont(	15, 
							0, 
							0, 
							0, 
							FW_NORMAL, 
							false, 
							false, 
							false, 
							DEFAULT_CHARSET, 
							OUT_DEFAULT_PRECIS, 
							CLIP_DEFAULT_PRECIS, 
							DEFAULT_QUALITY, 
							DEFAULT_PITCH, 
							"MS Sans Serif" );
							
	if( !m_hFont )
		return false;
		
	SendMessage( WM_SETFONT, (WPARAM) m_hFont, false );
	
	return true;
}

LRESULT CWGFTestUITabControl::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_DESTROY:
			DeleteObject( m_hFont );
			break;
	}
	
	return CWGFTestUIWindow::WindowProc( uMsg, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////////
// CWGFTestUIRichEditControl
//////////////////////////////////////////////////////////////////////////
bool CWGFTestUIRichEditControl::Create( HWND hWnd )
{
	Attach( hWnd );

	return true;
}
bool CWGFTestUIRichEditControl::Create( UINT uID, CWGFTestUIWindow *pParent, DWORD dwStyle , int x, int y, int width, int height )
{	
	UINT l = GetSystemDirectory( NULL, 0 );
	l += sizeof( _T( "riched32.dll" ) ) + 1;
	TCHAR *szPath = new TCHAR[ l ];
	if( !szPath )
		return false;
	UINT n = GetSystemDirectory( szPath, l );
	_tcscpy( szPath + n, _T( "\\riched32.dll" ) );
	szPath[l - 1] = 0;
	LoadLibrary( szPath );
	delete[] szPath;

	if( !CreateEx( 0, RICHEDIT_CLASS, NULL, dwStyle, x, y, width, height, pParent, (HMENU) (INT_PTR) uID, 0 ) )
		return false;
		
	return true;
}

LRESULT CWGFTestUIRichEditControl::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_DESTROY:
			break;
	}
	
	return CWGFTestUIWindow::WindowProc( uMsg, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////////
// CWGFTestUIGroupTreeControl
//////////////////////////////////////////////////////////////////////////
CWGFTestUIGroupTreeControl::CWGFTestUIGroupTreeControl() :
	m_hImageList( NULL )
{
}

bool CWGFTestUIGroupTreeControl::Create( HWND hWnd )
{
	Attach( hWnd );
	Initialize();

	return true;
}

bool CWGFTestUIGroupTreeControl::Create( UINT uID, CWGFTestUIWindow *pParent )
{
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof( icc );
	icc.dwICC = ICC_TREEVIEW_CLASSES;
	InitCommonControlsEx( &icc );
	
	if( !CreateEx( WS_EX_CLIENTEDGE, WC_TREEVIEW, _T( "Groups" ), WS_CHILD | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS, 0, 0, 1, 1, pParent, (HMENU) (INT_PTR) uID, 0 ) )
		return false;

	Initialize();

	return true;
}

LRESULT CWGFTestUIGroupTreeControl::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_DESTROY:
			ImageList_Destroy( m_hImageList );
			m_hImageList = NULL;
			break;
	}
	
	return CWGFTestUIWindow::WindowProc( uMsg, wParam, lParam );
}

void CWGFTestUIGroupTreeControl::Initialize( )
{
	AddGroups( TVI_ROOT );

	m_hImageList = ImageList_LoadBitmap( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDB_GROUPS_ICONS ), 16, 0, CLR_NONE );	
	TreeView_SetImageList( m_hWnd, m_hImageList, TVSIL_NORMAL );
};

void CWGFTestUIGroupTreeControl::AddGroups( HTREEITEM hParent )
{
	// Walk the test group tree.
	const CTestGroupNode *pRoot = GetFramework()->GetTestGroupNode( CTestGroupPath( _T( "" ) ) );
	
	for( size_t i = 0; i < pRoot->GetNumChildGroupNodes(); ++i )
	{
		const CTestGroupNode *pNode = pRoot->GetChildGroupNode( i );
		
		AddBranch( hParent, pNode );
	}
}

void CWGFTestUIGroupTreeControl::AddBranch( HTREEITEM hParent, const CTestGroupNode *pNode )
{
	TVINSERTSTRUCT tvis;
	tvis.hParent = hParent;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	tvis.item.pszText = const_cast< LPTSTR >( pNode->GetName().c_str() );
	tvis.item.lParam = (LPARAM) pNode;
	tvis.item.iImage = tvis.item.iSelectedImage = pNode->IsTestNode() ? 1 : 0;
	
	if ( 0 < pNode->GetNumChildGroupNodes() )
	{
		tvis.item.mask |= TVIF_STATE;
		tvis.item.state = TVIS_BOLD;
		tvis.item.stateMask = TVIS_BOLD;
	}

	HTREEITEM hItem = (HTREEITEM) SendMessage( TVM_INSERTITEM, 0, (LPARAM) &tvis );

	for( size_t i = 0; i < pNode->GetNumChildGroupNodes(); ++i )
	{
		AddBranch( hItem, pNode->GetChildGroupNode( i ) );
	}
}

const CTestGroupPath &CWGFTestUIGroupTreeControl::PathFromCurSel()
{
	static CTestGroupPath nullPath( "" );
	HTREEITEM hItem = TreeView_GetSelection( m_hWnd );
	
	if( hItem )
		return PathFromItem( hItem );
	
	return nullPath;
}

const CTestGroupPath &CWGFTestUIGroupTreeControl::PathFromItem( HTREEITEM hItem )
{
	TVITEM item;
	item.mask = TVIF_PARAM | TVIF_HANDLE;
	item.hItem = hItem;
	TreeView_GetItem( m_hWnd, &item );
	
	const CTestGroupNode *pNode = (const CTestGroupNode *) item.lParam;
	
	return pNode->GetPath();
}