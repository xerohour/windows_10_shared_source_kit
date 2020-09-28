#include "pch.h"

using namespace std;

#include "WGFTestUIStatus.h"

const size_t CWGFTestUIStatusWindow::NPOS = (size_t) -1;

CWGFTestUIStatusWindow::CWGFTestUIStatusWindow() :
	m_hFont( NULL ),
	m_nLineHeight( 0 ),
	m_nMarginLeft( 2 ),
	m_nMarginTop( 2 ),
	m_nNameColWidth( 128 ),
	m_hBrush( NULL ),
	m_hbmOffScreen( NULL )
{
}

bool CWGFTestUIStatusWindow::Create( UINT uID, CWGFTestUIWindow *pParent, int x, int y, int w, int h )
{
	static ATOM atom = 0;
	
	if( !atom )
		atom = WGFTestUIRegisterWndClass( "WGFStatusWindow", 0, 0, LoadCursor( NULL, IDC_ARROW ), NULL );
		
	if( !CreateEx( WS_EX_CLIENTEDGE, "WGFStatusWindow", NULL, WS_CHILD | WS_VSCROLL | WS_VISIBLE, x, y, w, h, pParent, (HMENU) (INT_PTR) uID, 0 ) )
		return false;
	
	m_hBrush = CreateSolidBrush( RGB( 0, 0, 0 ) );
		
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
							"Arial" );
	
	TEXTMETRIC tm;
	HDC hdc = GetDC();
	HFONT hOldFont = (HFONT) SelectObject( hdc, m_hFont );
	int nOldMM = SetMapMode( hdc, MM_TEXT );
	GetTextMetrics( hdc, &tm );
	m_nLineHeight = tm.tmHeight;
	SetMapMode( hdc, nOldMM );
	SelectObject( hdc, hOldFont );
	ReleaseDC( hdc );

	m_nNameColWidth = tm.tmAveCharWidth * 24;
		
	ShowWindow( SW_SHOW );
		
	return true;
}

LRESULT CWGFTestUIStatusWindow::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_DESTROY:
			if( m_hBrush )
				DeleteObject( m_hBrush );
			m_hBrush = NULL;
			
			if( m_hFont )
				DeleteObject( m_hFont );
			m_hFont = NULL;
			
			if( m_hbmOffScreen )
				DeleteObject( m_hbmOffScreen );
			m_hbmOffScreen = NULL;
			
			break;

		case WM_SIZE:
		{
			UpdateScrollRange();
			
			// Create a compatible bitmap the size of this window
			HDC hDC = GetDC();
			if( m_hbmOffScreen )
				DeleteObject( m_hbmOffScreen );	
			m_hbmOffScreen = CreateCompatibleBitmap( hDC, LOWORD( lParam ), HIWORD( lParam ) );
			ReleaseDC( hDC );
			
			RECT rcControl;
			GetWindowRect( &rcControl );
			
			m_nNameColWidth = max( 128, ( rcControl.right - rcControl.left ) / 3 );
			
			break;
		}
			
		case WM_VSCROLL:
		{
			SCROLLINFO si;
			int yPos;
			
			// Get all the vertial scroll bar information
			si.cbSize = sizeof (si);
			si.fMask  = SIF_ALL;
			GetScrollInfo (m_hWnd, SB_VERT, &si);
			// Save the position for comparison later on
			yPos = si.nPos;
			switch (LOWORD (wParam))
			{
				// user clicked the HOME keyboard key
			case SB_TOP:
				si.nPos = si.nMin;
				break;

				// user clicked the END keyboard key
			case SB_BOTTOM:
				si.nPos = si.nMax;
				break;

				// user clicked the top arrow
			case SB_LINEUP:
				si.nPos -= 1;
				break;

				// user clicked the bottom arrow
			case SB_LINEDOWN:
				si.nPos += 1;
				break;

				// user clicked the shaft above the scroll box
			case SB_PAGEUP:
				si.nPos -= si.nPage;
				break;

				// user clicked the shaft below the scroll box
			case SB_PAGEDOWN:
				si.nPos += si.nPage;
				break;

				// user dragged the scroll box
			case SB_THUMBTRACK:
				si.nPos = si.nTrackPos;
				break;

			default:
				break; 
			}
			// Set the position and then retrieve it.  Due to adjustments
			//   by Windows it may not be the same as the value set.
			si.fMask = SIF_POS;
			SetScrollInfo (m_hWnd, SB_VERT, &si, TRUE);
			GetScrollInfo (m_hWnd, SB_VERT, &si);
			// If the position has changed, scroll window and update it
			if (si.nPos != yPos)
			{                    
				ScrollWindow(m_hWnd, 0, m_nLineHeight * (yPos - si.nPos), NULL, NULL);
				InvalidateRect( NULL, false );
			}
			return 0;
		}
		
		case WM_PAINT:
		{
			RECT rcUpdate;
			
			if( GetUpdateRect( &rcUpdate, false ) )
			{
				PAINTSTRUCT ps;
				BeginPaint( &ps );
				
				RECT rcClient;
				GetClientRect( &rcClient );
				
				HDC hdcOffScreen = CreateCompatibleDC( ps.hdc );
				
				HGDIOBJ hbmOld = SelectObject( hdcOffScreen, m_hbmOffScreen );

				// Fill the update rect with the background brush
				FillRect( hdcOffScreen, &rcUpdate, m_hBrush );
				
				int mmOld = SetMapMode( hdcOffScreen, MM_TEXT );
				HFONT hfontOld = (HFONT) SelectObject( hdcOffScreen, m_hFont );
				int modeOld = SetBkMode( hdcOffScreen, TRANSPARENT );
				COLORREF crOld = SetTextColor( hdcOffScreen, RGB( 192, 192, 192 ) );
				
				// Find the first item associated with the rectangle.
				int nPos = GetScrollPos( m_hWnd, SB_VERT );
				int nFirst = nPos + ( rcUpdate.top - m_nMarginTop ) / m_nLineHeight;
				int nLast = min( (int) m_Items.size(), 1 + nPos + (rcUpdate.bottom - m_nMarginTop ) / m_nLineHeight );
				
				RECT rcName;
				RECT rcValue;
				
				rcName.left = m_nMarginLeft;
				rcName.top = m_nMarginTop + ( nFirst - nPos ) * m_nLineHeight;
				rcName.right = m_nNameColWidth;
				rcName.bottom = rcName.top + m_nLineHeight;
				
				rcValue.left = m_nMarginLeft + m_nNameColWidth;
				rcValue.top = rcName.top;
				rcValue.right = rcClient.right;
				rcValue.bottom = rcValue.top + m_nLineHeight;
				
				for( int n = nFirst; n < nLast; ++n )
				{
					RECT rcDraw;
					if( IntersectRect( &rcDraw, &rcName, &rcUpdate ) )
					{
						WGFTestUIDrawText( hdcOffScreen, m_Items[n].strName.c_str(), -1, &rcName, DT_LEFT );
					}
					
					if( IntersectRect( &rcDraw, &rcValue, &rcUpdate ) )
					{
						WGFTestUIDrawText( hdcOffScreen, m_Items[n].strValue.c_str(), -1, &rcValue, DT_LEFT );
					}
					
					rcName.top += m_nLineHeight;
					rcName.bottom += m_nLineHeight;
					rcValue.top += m_nLineHeight;
					rcValue.bottom += m_nLineHeight;
				}
				
				BitBlt( ps.hdc, rcUpdate.left, rcUpdate.top, rcUpdate.right - rcUpdate.left, rcUpdate.bottom - rcUpdate.top, hdcOffScreen, rcUpdate.left, rcUpdate.top, SRCCOPY );

				SetMapMode( hdcOffScreen, mmOld );
				SelectObject( hdcOffScreen, hfontOld );
				SetBkMode( hdcOffScreen, modeOld );
				SetTextColor( hdcOffScreen, crOld );
				SelectObject( hdcOffScreen, hbmOld );
				
				DeleteDC( hdcOffScreen );

				EndPaint( &ps );
			}
			
			return 0;
			
			break;
		}
	}
	
	return CWGFTestUIWindow::WindowProc( uMsg, wParam, lParam );
}

void CWGFTestUIStatusWindow::InvalidateItemRect( size_t nIndex, bool bName, bool bValue )
{
	if( bName || bValue )
	{
		RECT rc;
		RECT rcClient;
		int pos = GetScrollPos( m_hWnd, SB_VERT );
		int relIndex = (int) ( nIndex - pos );
		
		GetClientRect( &rcClient );
		
		rc.top = m_nMarginTop + relIndex * m_nLineHeight;
		rc.bottom = m_nLineHeight + rc.top;
		rc.left = m_nMarginLeft + ( bName ? 0 : m_nNameColWidth );
		rc.right = bValue ? rcClient.right : m_nNameColWidth;
		
		InvalidateRect( &rc, false );
	}
}

void CWGFTestUIStatusWindow::UpdateScrollRange()
{
	RECT rcClient;
	
	GetClientRect( &rcClient );
	SCROLLINFO si;
	si.cbSize = sizeof( si );
	si.fMask = SIF_PAGE | SIF_RANGE;
	si.nPage = m_nLineHeight ? ( rcClient.bottom - m_nMarginTop ) / m_nLineHeight : 0;
	si.nMin = 0;
	si.nMax = (int) m_Items.size() - 1;
	SetScrollInfo( m_hWnd, SB_VERT, &si, false );
}

size_t CWGFTestUIStatusWindow::SetItem( LPCTSTR szName, LPCTSTR szValue )
{
	int nIndex; 
	
	ItemIndexMapType::iterator it = m_ItemIndexMap.find( szName );
	if( it != m_ItemIndexMap.end() )
	{
		nIndex = (int) it->second;
		Item &item = m_Items[ nIndex ];
		item.dwFlags = 0;
		if( item.strValue != string( szValue ) )
		{
			item.dwFlags = DIRTY_VALUE;
			item.strValue = szValue;
			InvalidateItemRect( nIndex, false, true );
		}
	}
	else
	{
		nIndex = (int) m_Items.size();
		Item item( szName, szValue );
		m_ItemIndexMap.insert( ItemIndexMapType::value_type( item.strName, nIndex ) );
		m_Items.push_back( item );
		InvalidateItemRect( nIndex, true, true );
		UpdateScrollRange();
	}
	
	return nIndex;
}

void CWGFTestUIStatusWindow::ClearItems( size_t nStart, size_t nEnd )
{
	size_t nLast = m_Items.size();
	size_t nFirst;
	
	// Remove the items
	for( nFirst = nStart; nFirst <= nEnd && nFirst < nLast; ++nFirst )
	{
		Item &item = m_Items[ nFirst ];
		item.dwFlags = INVALID;
	}
}

void CWGFTestUIStatusWindow::RemoveItem( LPCTSTR szName )
{
	ItemIndexMapType::iterator it = m_ItemIndexMap.find( szName );

	if( it != m_ItemIndexMap.end() )
		ClearItems( it->second, it->second + 1 );
}

void CWGFTestUIStatusWindow::Refresh()
{
	size_t n;
	bool bForceInvalidateRect = false;
	
	// Invalidate appropriate rects
	for( n = 0; n < m_Items.size(); ++n )
	{
		Item &item = m_Items[ n ];
		
		if( item.dwFlags & INVALID )
			bForceInvalidateRect = true;
			
		InvalidateItemRect( n, bForceInvalidateRect || ( item.dwFlags & DIRTY_NAME ), bForceInvalidateRect || ( item.dwFlags & DIRTY_VALUE ) );
	}
	
	// Clear out invalid data
	size_t dest = 0;
	for( n = 0; n < m_Items.size(); ++n )
	{
		Item &item = m_Items[ n ];
		
		if( item.dwFlags & INVALID )
		{
			// search for the next valid item
			m_ItemIndexMap.erase( item.strName );
		}
		else
		{
			if( dest < n )
			{
				ItemIndexMapType::iterator it = m_ItemIndexMap.find( item.strName );
				it->second = dest;
				m_Items[dest] = item;
			}
			++dest;
		}
	}
	
	if( dest < m_Items.size() )
		m_Items.resize( dest );
}





//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


bool CWGFTestUIPropertyWindow::Create( UINT uID, CWGFTestUIWindow *pParent, int x, int y, int w, int h )
{
	DWORD dwListViewExStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
	DWORD dwListViewStyle = LVS_NOLABELWRAP | LVS_REPORT;
	
	if( !CreateEx( WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, WS_CHILD | dwListViewStyle, x, y, w, h, pParent, (HMENU) (INT_PTR) uID, 0 ) )
		return false;
	
	SendMessage( LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM) dwListViewExStyle, (LPARAM) dwListViewExStyle );
		
	// Add columns
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvc.pszText = _T( "Key" );
	lvc.iSubItem = 0;
	lvc.cx = 128;
	SendMessage( LVM_INSERTCOLUMN, lvc.iSubItem, (LPARAM) &lvc );
	
	lvc.pszText = _T( "Value" );
	lvc.iSubItem = 1;
	lvc.cx = 512;
	SendMessage( LVM_INSERTCOLUMN, lvc.iSubItem, (LPARAM) &lvc );
	
	return true;
}
