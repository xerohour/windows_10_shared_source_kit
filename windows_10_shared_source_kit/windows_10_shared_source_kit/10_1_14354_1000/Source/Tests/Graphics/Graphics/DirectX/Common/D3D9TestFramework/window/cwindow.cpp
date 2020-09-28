#include "WindowPCH.h"
#include "Framework.h"
#include <strsafe.h>

// --------------------------------------------------------------------------------------

CWindow::CWindow()
{
	m_hWindow   = NULL;
	m_hFont 	= NULL;
	m_fWidth = m_fHeight = 0.f;
	m_pParent = NULL;
	m_nChildren = 0;
	m_nID = 0;
	m_bFixed = false;
	memset(&m_WC, 0, sizeof(WNDCLASS));
}

// --------------------------------------------------------------------------------------

CWindow::CWindow(UINT_PTR nID, HWND hWnd)
{
	m_hWindow   = NULL;
	m_hFont 	= NULL;
	m_fWidth = m_fHeight = 0.f;
	m_pParent = NULL;
	m_nChildren = 0;
	m_nID = nID;
	memset(&m_WC, 0, sizeof(WNDCLASS));

	m_dwStyle = (DWORD)GetWindowLongPtr(hWnd, GWL_STYLE);
	m_dwStyleEx = (DWORD)GetWindowLongPtr(hWnd, GWL_EXSTYLE);
	m_hWindow = hWnd;
	m_pParent = GetParent();

	Init();
}

// --------------------------------------------------------------------------------------

CWindow::~CWindow()
{
}

// --------------------------------------------------------------------------------------

void CWindow::SetStyle(DWORD dwStyle)
{
	m_dwStyle = dwStyle;
	SetWindowLong(m_hWindow, GWL_STYLE, m_dwStyle);
}

// --------------------------------------------------------------------------------------

void CWindow::SetStyleEx(DWORD dwStyleEx)
{
	m_dwStyleEx = dwStyleEx;
	SetWindowLong(m_hWindow, GWL_EXSTYLE, m_dwStyleEx);
}

// --------------------------------------------------------------------------------------

CWindow* CWindow::GetWindowPointer(HWND hWnd)
{
	if (NULL != hWnd)
		return (CWindow *) GetWindowLongPtr(hWnd,GWLP_USERDATA);

	return NULL;
}

// --------------------------------------------------------------------------------------

bool CWindow::InitializeWindow
(
	DWORD dwStyleEx,
	LPTSTR pClassName,
	LPTSTR pName,
	HMENU hMenu,
	HICON hIcon,
	DWORD dwStyle,
	int nPosX,
	int nPosY,
	int nWidth,
	int nHeight,
	HWND hParent
)
{
	TCHAR tcsBuffer[ 128 ] = _T( "" );
	StringCchCopy
	(
		tcsBuffer,
		ARRAYSIZE( tcsBuffer ),
		pClassName
	);

	_tcslwr(tcsBuffer);
    
	// Register any new window classes
	if(_tcsstr(tcsBuffer, "fmwk"))
	{
		m_WC.style            = 0;
		m_WC.lpfnWndProc      = (WNDPROC) StdWndProc;
		m_WC.cbClsExtra       = 0;
		m_WC.cbWndExtra       = sizeof(LPVOID);
		m_WC.hInstance        = GetModuleHandle(NULL);
		m_WC.hIcon            = hIcon;
		m_WC.hCursor          = NULL;
		m_WC.hbrBackground    = (HBRUSH) GetStockObject(BLACK_BRUSH);
		m_WC.lpszMenuName     = NULL;
		m_WC.lpszClassName    = pClassName;

		// Register the window class
		RegisterClass(&m_WC);
	}

	m_hWindow = CreateWindowEx
	(
		dwStyleEx,
		pClassName,
		pName,
		dwStyle,
		nPosX,
		nPosY,
		nWidth,
		nHeight,
		hParent,
		hMenu,
		GetModuleHandle(NULL),
		(void *) this
	);

	if (NULL == m_hWindow)
	{
		DWORD CreateWindowError = ::GetLastError();

		DPF
		(
			1,
			_T("CWindow::InitializeWindow:  CreateWindowEx failed with error %u.\n"),
			CreateWindowError
		);

		return false;
	};

	m_dwStyle = dwStyle;
	m_dwStyleEx = dwStyleEx;
	m_nID = (UINT_PTR)hMenu;
	m_pParent = GetParent();

	Init();

	return true;
}

// --------------------------------------------------------------------------------------

void CWindow::CalculateSize()
{
	RECT r;

	if(NULL == m_hWindow)
		return;

	// Get the window/height
	GetWindowRect(m_hWindow, &r);
	m_nWidth = r.right - r.left;
	m_nHeight = r.bottom - r.top;
}

// --------------------------------------------------------------------------------------

void CWindow::Init()
{
	// Set the window long again in case the class is not owned by us.
	SetWindowLongPtr(m_hWindow,GWLP_USERDATA,(LONG_PTR)this);

	// Get the window/height
	CalculateSize();

	// Create and set a font
	HDC hDC = GetDC(m_hWindow);
	if(hDC)
	{
		m_hFont = MakeFont(hDC, 8, 0, 0, 0);
		ReleaseDC(m_hWindow, hDC);
	}
	SendMessage(m_hWindow, WM_SETFONT, (WPARAM)m_hFont, TRUE);
}

// --------------------------------------------------------------------------------------

void CWindow::UpdateChildStats()
{
	UINT nHeight = 0;
	UINT nX, nY;
		int i;

		for(i=0; i<m_nChildren; i++)
	{
		m_pChildren[i]->GetBorders(nX, nY);
		nHeight += 	m_pChildren[i]->m_nHeight + nY;
	}
	   
	for(i=0; i<m_nChildren; i++)
	{
		m_pChildren[i]->GetBorders(nX, nY);
		m_pChildren[i]->m_fHeight = (float)(m_pChildren[i]->m_nHeight + nY) / (float)nHeight;
		m_pChildren[i]->m_fWidth = 1.f;
	}
}
// --------------------------------------------------------------------------------------

bool CWindow::AddChild(TCHAR *pcsName, CWindow *pChild)
{

	if(m_nChildren >= MAX_CHILDREN || NULL == pChild)
		return false;

	m_pChildren[m_nChildren++] = pChild;

	UpdateChildStats();

	return true;
}

// --------------------------------------------------------------------------------------

void CWindow::Move(UINT nX, UINT nY, UINT nWidth, UINT nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	::MoveWindow(m_hWindow, nX, nY, nWidth, nHeight, TRUE);
}

// --------------------------------------------------------------------------------------

void CWindow::Move(UINT nX, UINT nY)
{
	HWND insertAfter = HWND_NOTOPMOST;

	if( m_dwStyleEx & WS_EX_TOPMOST )
		insertAfter = HWND_TOPMOST;

	::SetWindowPos(m_hWindow, insertAfter, nX,nY,0,0, SWP_NOZORDER | SWP_NOSIZE);
}

// --------------------------------------------------------------------------------------

void CWindow::Update(void)
{
	// Base class noop
}

// --------------------------------------------------------------------------------------

void CWindow::SetVisible(bool bVisible)
{
	if(bVisible)
		m_dwStyle |= WS_VISIBLE;
	else
		m_dwStyle &= (~WS_VISIBLE);

	SetWindowLongPtr(m_hWindow, GWL_STYLE, m_dwStyle);
}

// --------------------------------------------------------------------------------------

void CWindow::SetEnable(bool bEnable)
{
	SendMessage(m_hWindow, WM_ENABLE, (BOOL)bEnable, 0);
}

// --------------------------------------------------------------------------------------

HFONT CWindow::MakeFont(HDC hDC, int nPoint, DWORD dwWeight, DWORD dwItalic, DWORD dwUnderline)
{
	int nHeight = -MulDiv(nPoint, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	int nWidth = 0;

	return CreateFont(
				nHeight,   		// height of font
				nWidth,         // average character width
				0, 				// angle of escapement
				0,          	// base-line orientation angle
				dwWeight,       // font weight
				dwItalic,       // italic attribute option
				dwUnderline,    // underline attribute option
				0,        		// strikeout attribute option
				ANSI_CHARSET,          	// character set identifier
				OUT_DEFAULT_PRECIS,  	// output precision
				CLIP_DEFAULT_PRECIS,    // clipping precision
				DEFAULT_QUALITY,        // output quality
				DEFAULT_PITCH | FF_DONTCARE,   // pitch and family
				NULL			// typeface name
	);
}

// --------------------------------------------------------------------------------------

void CWindow::Close()
{	
	//
	// Free child window objects.
	//

	for ( int i = 0; i < m_nChildren; ++i )
	{
		if ( m_pChildren[ i ] != NULL )
		{
			m_pChildren[ i ]->Close();
			delete m_pChildren[ i ];
			m_pChildren[ i ] = NULL;
		};
	};

	m_nChildren = 0;


	//
	// Release our font object.
	//

	if(m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	};


	//
	// Unregister our window class.
	//

	if(NULL != m_WC.lpszClassName)
	{
		UnregisterClass(m_WC.lpszClassName, m_WC.hInstance);
	};


	//
	// Destroy our window.
	//

	if(m_hWindow)
	{
		SetWindowLongPtr(m_hWindow,GWLP_USERDATA,(LONG_PTR)NULL);
		DestroyWindow(m_hWindow);
		m_hWindow = NULL;
	};
}

// --------------------------------------------------------------------------------------

void CWindow::GetBorders(UINT &nWidth, UINT &nHeight)
{
	RECT rRect, rRect2;
	GetClientRect(m_hWindow,&rRect);
	GetWindowRect(m_hWindow,&rRect2);
	rRect2.right -= rRect2.left;
	rRect2.bottom -= rRect2.top;

	nWidth = rRect2.right-rRect.right;
	nHeight = rRect2.bottom-rRect.bottom;
}

// --------------------------------------------------------------------------------------

void CWindow::Resize(UINT nWidth, UINT nHeight)
{
	UINT nX, nY;
	GetBorders(nX, nY);
	m_nWidth = nWidth-nX;
	m_nHeight = nHeight-nY;

	HWND insertAfter = HWND_NOTOPMOST;

	if( m_dwStyleEx & WS_EX_TOPMOST )
		insertAfter = HWND_TOPMOST;

	SetWindowPos(m_hWindow, insertAfter, 0,0,nWidth,nHeight, SWP_NOZORDER | SWP_NOMOVE);
}

// --------------------------------------------------------------------------------------

void CWindow::OnPaint(WPARAM wParam, LPARAM lParam)
{
	DefWindowProc(m_hWindow,WM_PAINT,wParam,lParam);
}

// --------------------------------------------------------------------------------------

CWindow* CWindow::GetParent(void)
{
	return GetWindowPointer(::GetParent(m_hWindow));
}

// --------------------------------------------------------------------------------------

CWindow* CWindow::GetWindow(UINT_PTR nID)
{
	CWindow *pWindow = NULL;

	if(m_nID == nID)
		return this;

	for(int i=0; i<m_nChildren; i++)
	{
		pWindow = m_pChildren[i]->GetWindow(nID);
		if(NULL != pWindow)
			return pWindow;
	}

	return NULL;
}

// --------------------------------------------------------------------------------------

LRESULT CWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(m_hWindow,WM_COMMAND,wParam,lParam);
}

// --------------------------------------------------------------------------------------

void CWindow::OnNotify(WPARAM wParam, LPARAM lParam)
{
	DefWindowProc(m_hWindow,WM_NOTIFY,wParam,lParam);
}

// --------------------------------------------------------------------------------------

void CWindow::OnSize(WPARAM wParam, LPARAM lParam)
{
	m_nWidth = LOWORD(lParam);
	m_nHeight = HIWORD(lParam);

	DefWindowProc(m_hWindow,WM_SIZE,wParam,lParam);
}

// --------------------------------------------------------------------------------------

LRESULT CWindow::OnKeydown(WPARAM wParam, LPARAM lParam)
{
	if(m_pParent && TRUE == m_pParent->OnKeydown(wParam, lParam))
		return TRUE;
	else
		return DefWindowProc(m_hWindow,WM_KEYDOWN,wParam,lParam);
}

// --------------------------------------------------------------------------------------

LRESULT CWindow::OnKeyup(WPARAM wParam, LPARAM lParam)
{
	if(m_pParent && TRUE == m_pParent->OnKeyup(wParam, lParam))
		return TRUE;
	else
		return DefWindowProc(m_hWindow,WM_KEYUP,wParam,lParam);
}

// --------------------------------------------------------------------------------------

LRESULT CWindow::OnMouse(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_pParent && TRUE == m_pParent->OnMouse(uMsg, wParam, lParam))
		return TRUE;
	else
		return DefWindowProc(m_hWindow,WM_KEYDOWN,wParam,lParam);	
}

// --------------------------------------------------------------------------------------

LRESULT CWindow::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	if(m_pParent && TRUE == m_pParent->OnSetCursor(wParam, lParam))
		return TRUE;
	else
		return DefWindowProc(m_hWindow,WM_SETCURSOR,wParam,lParam);	
}

// --------------------------------------------------------------------------------------

LRESULT CWindow::WindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	switch(uMessage)
	{
		case WM_SIZE:
			OnSize(wParam, lParam);
			break;
		case WM_NOTIFY:
			OnNotify(wParam, lParam);
			break;
		case WM_PAINT:
			BeginPaint(m_hWindow, &ps);
			OnPaint(wParam, lParam);
			EndPaint(m_hWindow, &ps);
			break;
		case WM_COMMAND:
			return OnCommand(wParam, lParam);
		case WM_KEYDOWN:
			return OnKeydown(wParam, lParam);
		case WM_KEYUP:
			return OnKeyup(wParam, lParam);
		case WM_SETCURSOR:
			return OnSetCursor(wParam, lParam);
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			SetFocus(m_hWindow);
		case WM_MOUSEMOVE:
			return OnMouse(uMessage,wParam,lParam);
		default:
			return DefWindowProc(m_hWindow,uMessage,wParam,lParam);
	}

	return 0;   
}

// --------------------------------------------------------------------------------------

LRESULT WINAPI CWindow::StdWndProc
(
	HWND hWnd,
	UINT uMessage,
	WPARAM wParam,
	LPARAM lParam
)
{
	//
	// Validate parameters.
	//

	if ( hWnd == NULL )
	{
		return 0;
	};

	CWindow * pThis = NULL;

	switch ( uMessage )
	{
		case WM_NCCREATE:
		{
			// Grab the pointer from our create struct.
			CREATESTRUCT * pStruct = (CREATESTRUCT *) lParam;

			ASSERT( pStruct != NULL );
			if ( pStruct == NULL )
			{
				return 0;
			};


			// Cast it to a this pointer.
			pThis = (CWindow *)pStruct->lpCreateParams;

			ASSERT( pThis != NULL );
			if ( pThis == NULL )
			{
				return 0;
			};


			// Stash it into the window bytes for later use.
			SetWindowLongPtr
			(
				hWnd,
				GWLP_USERDATA,
				(LONG_PTR) pThis
			);

			// Assign our window handle.
			HWND * phWnd = const_cast<HWND *>( &pThis->m_hWindow );
			*phWnd = hWnd;

			return 1;
		} break;


		case WM_DESTROY:
		{
			SetWindowLongPtr
			(
				hWnd,
				GWLP_USERDATA,
				0
			);

			return 0;
		} break;
	};


	//
	// Retrieve our this pointer.
	//

	pThis = GetWindowPointer( hWnd );

	if ( NULL == pThis )
	{
		return DefWindowProc
		(
			hWnd,
			uMessage,
			wParam,
			lParam
		);
	}

	return pThis->WindowProc
	(
		uMessage,
		wParam,
		lParam
	);
}

 

