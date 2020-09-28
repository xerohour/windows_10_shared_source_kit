#include "pch.h"

#include "WGFTestUIWindow.h"
#include "WGFTestUIDeviceWindow.h"

#include <strsafe.h>

CWGFTestUIDeviceWindow::CWGFTestUIDeviceWindow()
{
    m_dwFlags = 0;
	m_hTitleFont = 0;
	m_hDescFont = 0;
	m_hBackgroundBrush = 0;
}

CWGFTestUIDeviceWindow::~CWGFTestUIDeviceWindow()
{
	if (m_hTitleFont != 0)
		DeleteObject(m_hTitleFont);
	if (m_hDescFont != 0)
		DeleteObject(m_hDescFont);
	if (m_hBackgroundBrush != 0)
		DeleteObject(m_hBackgroundBrush);
}

bool CWGFTestUIDeviceWindow::Create(UINT uWidth, UINT uHeight, CWGFTestUIWindow *pParent, DWORD dwFlags )
{
	static ATOM awcrw = 0;
	static ATOM awcdw = 0;
	
	if( !awcdw )
		awcdw = WGFTestUIRegisterWndClass( _T( "WGFTestUIDeviceWindow" ), 0, 0, LoadCursor( NULL, IDC_ARROW ), NULL );
	
	if( !awcrw )
		awcrw = WGFTestUIRegisterWndClass( _T( "WGFTestUIRenderWindow" ), 0, 0, LoadCursor( NULL, IDC_ARROW ), CreateSolidBrush( RGB( 0, 0, 0 ) ) );
	
	DWORD dwStyle = WS_CHILD | WS_CLIPCHILDREN;
	DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_CLIENTEDGE;
	
    // Compute initial size
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = uWidth;	// Default client size
    rc.bottom = uHeight; // Default client size
    AdjustWindowRectEx( &rc, dwStyle, false, dwExStyle );
	
    bool bResult = CreateEx( dwExStyle, _T( "WGFTestUIDeviceWindow" ), NULL, dwStyle, 0, 0, rc.right - rc.left, rc.bottom - rc.top, pParent, NULL, 0 );
    
    if (!bResult)
        return false;

	// Create the render window        
    bResult = m_RenderWindow.CreateEx( 0, _T( "WGFTestUIRenderWindow" ), NULL, WS_CHILD | WS_VISIBLE, 0, 0, uWidth, uHeight, this, NULL, 0 );
    
    if (!bResult)
        return false;

    // Create the fonts to use.

    m_hTitleFont = CreateFont(18,0,0,0,0,FALSE,FALSE,FALSE,0,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,_T( "Arial Black" ));
    m_hDescFont = CreateFont(15,0,0,0,0,FALSE,FALSE,FALSE,0,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,_T( "Arial" ));

    m_hBackgroundBrush = CreateSolidBrush(RGB(50,50,150));

    // Set some default text values
    
    SetDeviceTitle( _T( "Unknown Device" ), _T( "No device description available" ) );

    // Determine font extents

    HDC     hDC = GetDC();
    HFONT   hOldFont = (HFONT) ::SelectObject(hDC,m_hTitleFont);
    SIZE    Size;
    
    GetTextExtentPoint32(hDC,_T( "Wq" ),2,&Size);
    
    m_nTitleSizeY = Size.cy;
    
    SelectObject(hDC,m_hDescFont);
    
    GetTextExtentPoint32(hDC,_T( "Wq" ),2,&Size);
    
    m_nDescSizeY = Size.cy;
    
    SelectObject(hDC,hOldFont);
    
    ReleaseDC(hDC);
    
    SetWindowPos( HWND_TOP, 0, 0, rc.right - rc.left, m_nTitleSizeY + m_nDescSizeY + rc.bottom - rc.top, SWP_NOMOVE );

    return true;
}

bool CWGFTestUIDeviceWindow::SetDeviceWindowFlags( DWORD dwFlags )
{
    m_dwFlags = dwFlags;
    
    return true;
}

DWORD CWGFTestUIDeviceWindow::GetDeviceWindowFlags()
{
    return m_dwFlags;
}

void CWGFTestUIDeviceWindow::SetDeviceTitle(LPCTSTR szTitle, LPCTSTR szDescription)
{
    if( FAILED( StringCbCopy( m_szTitle, MAX_TITLE, szTitle ) ) )
		m_szTitle[0] = 0;
		
    if( FAILED( StringCbCopy( m_szDescription, MAX_TITLE, szDescription ) ) )
		m_szDescription[0] = 0;
}

bool CWGFTestUIDeviceWindow::SetDeviceWindowSize(UINT uWidth, UINT uHeight)
{
	// BUGBUG - TODO...
	return true;
}

LRESULT CWGFTestUIDeviceWindow::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case    WM_ERASEBKGND:
                return FALSE;
                
        case WM_SIZE:
        {
			RECT rc;
			GetClientRect( &rc );
			int y = m_nDescSizeY + m_nTitleSizeY;
			rc.bottom -= y;
			rc.top += y;
			m_RenderWindow.SetWindowPos( HWND_TOP, 0, rc.top, rc.right, rc.bottom, 0 );
			break;
		}	
		
        case    WM_PAINT:
        {
            RECT        rRect;
            PAINTSTRUCT ps;
            HDC         hPaintDC = BeginPaint(&ps);
            HDC         hDC = CreateCompatibleDC(hPaintDC);
            
            GetClientRect(&rRect);
            
            HBITMAP     hBmp = CreateCompatibleBitmap(hPaintDC,rRect.right,rRect.bottom);
            HBITMAP     hOldBmp = (HBITMAP) SelectObject(hDC,hBmp);
            HFONT       hOldFont = (HFONT) ::SelectObject(hDC,m_hTitleFont);
            
            rRect.bottom = m_nDescSizeY + m_nTitleSizeY;
            
            FillRect(hDC,&rRect,m_hBackgroundBrush);
            
            COLORREF cr = SetTextColor( hDC, RGB(255,255,0) );
            RECT rc;
            rc.top = rRect.top;
            rc.left = rRect.left + 10;
            rc.bottom = rRect.bottom;
            rc.right = rRect.right;
            int bm = SetBkMode( hDC, TRANSPARENT );
            WGFTestUIDrawText( hDC, m_szTitle, - 1, &rc, 0 );
            rc.top += m_nTitleSizeY;
            
            SelectObject(hDC,m_hDescFont);
            
            WGFTestUIDrawText( hDC, m_szDescription, -1, &rc, 0 );
            
            SetBkMode( hDC, bm );
            
            BitBlt(hPaintDC,0,0,rRect.right,rRect.bottom,hDC,0,0,SRCCOPY);
            
            SelectObject(hDC,hOldFont);
            SelectObject(hDC,hOldBmp);
            
            DeleteDC(hDC);
            DeleteObject(hBmp);
            
            EndPaint(&ps);
        }
        break;
        case    WM_NCHITTEST:
                {
                    if( GetParent() == NULL )
                        return HTCAPTION;
                }
                break;
    }

    return CWGFTestUIWindow::WindowProc( uMsg, wParam, lParam);
}

LRESULT CRenderWindow::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_ERASEBKGND:
			if( m_hBitmap )
				return 0; // Do Nothing
			break;
			
		case WM_PAINT:
			if( m_hBitmap )
			{
				RECT rRect;
				PAINTSTRUCT ps;
				HDC hPaintDC = BeginPaint(&ps);
				HDC hDC = CreateCompatibleDC(hPaintDC);
	            
				GetClientRect(&rRect);
				
				HDC hdcSrc = CreateCompatibleDC( hDC );
				HBITMAP hbmOld = (HBITMAP) SelectObject( hdcSrc, m_hBitmap );
				
				BitBlt( hDC, 0, 0, rRect.right - rRect.left, rRect.bottom - rRect.top, hdcSrc, 0, 0, SRCCOPY );
				
				SelectObject( hdcSrc, hbmOld );
				DeleteDC( hdcSrc );

				EndPaint( &ps );
			}
			break;
	}
	
	return CWGFTestUIWindow::WindowProc( uMsg, wParam, lParam );
}
