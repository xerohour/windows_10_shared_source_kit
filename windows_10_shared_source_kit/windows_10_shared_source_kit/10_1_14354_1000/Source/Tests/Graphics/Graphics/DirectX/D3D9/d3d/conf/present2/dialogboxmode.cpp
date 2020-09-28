// DialogBoxMode.cpp: implementation of the CDialogBoxMode class.
//
//////////////////////////////////////////////////////////////////////

#include "DialogBoxMode.h"

INT_PTR CALLBACK TestProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialogBoxMode::CDialogBoxMode()
{
	m_TestType		= TEST_SETDIALOGBOXMODE;
	m_szCommandKey	= _T("DialogBoxMode");
	m_szTestName	= _T("DialogBoxMode");

	m_Options.D3DOptions.DisplayMode.Format	= FMT_ALL;
    m_Options.D3DOptions.dwSwapEffect = SWAPEFFECT_DISCARD;

    // Disable window drop shadows for the duration of the test
    SystemParametersInfo(SPI_GETDROPSHADOW, 0, &m_oldDropShadowState, 0);
    SystemParametersInfo(SPI_SETDROPSHADOW, 0, FALSE, 0);
}

CDialogBoxMode::~CDialogBoxMode()
{
    // Restore drop shadows to previous setting
    SystemParametersInfo(SPI_SETDROPSHADOW, 0, (PVOID)m_oldDropShadowState, 0);
}


//////////////////////////////////////////////////////////////////////
// Name:	Name()
// Desc:	return current test name
//////////////////////////////////////////////////////////////////////
char* CDialogBoxMode::Name( char* szName )
{
	sprintf( szName, "%s (%d,%d)", m_szTestName, m_nIndexPresentParameters, m_nIndexDialogs );
	return szName;
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentParameters()
// Desc:	build the vector of present parameters to test
//////////////////////////////////////////////////////////////////////
void CDialogBoxMode::InitPresentParameters()
{
	PRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	GetFrontBufferRect();

	m_vecPresentParameters.clear();

	d3dpp.bWindowed						= false;
	d3dpp.uBackBufferCount				= 1;
	d3dpp.SwapEffect					= SWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow					= m_hWndReset;
	d3dpp.dwFlags						= PRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.uPresentInterval				= D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.uWidth						= m_vecAdapterMonitors[0].m_modeDisplay.dwWidth;
	d3dpp.uHeight						= m_vecAdapterMonitors[0].m_modeDisplay.dwHeight;
	d3dpp.BackBufferFormat				= m_fmtFrontBuffer;
	d3dpp.uFullscreenRefresh			= D3DPRESENT_RATE_DEFAULT;
	d3dpp.AutoDepthStencilFormat		= FMT_UNKNOWN;
	d3dpp.bAutoDepthStencil				= false;
	d3dpp.MultiSample					= MULTISAMPLE_NONE;

	d3dpp.bCachedBackBuffers			= false;
	d3dpp.DesktopFormat					= m_fmtFrontBuffer;

	m_vecPresentParameters.push_back( d3dpp );
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPrivateData()
// Desc:	build the vector of special data
//////////////////////////////////////////////////////////////////////
void CDialogBoxMode::InitPrivateData()
{
	m_vecPrivateData.clear();

	m_vecPrivateData.push_back(0);	// 0 - default cool behavior
	m_vecPrivateData.push_back(1);	// 1 - SetDialogBoxMode(false) after setting DialogBoxMode true
}


//////////////////////////////////////////////////////////////////////
// Name:	InitPresentWindows()
// Desc:	build the vector of Present windows to test
//////////////////////////////////////////////////////////////////////
void CDialogBoxMode::InitPresentWindows()
{
	m_vecPresentWindow.clear();
	m_vecPresentWindow.push_back( NULL );
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of present source rects to test
//////////////////////////////////////////////////////////////////////
void CDialogBoxMode::InitPresentSourceRect()
{
	m_vecRectPresentSource.clear();

	m_vecRectPresentSource.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDirtyRegions()
// Desc:	build the vector of present dest rects to test
//////////////////////////////////////////////////////////////////////
void CDialogBoxMode::InitPresentDestRect()
{
	m_vecRectPresentDest.clear();

	m_vecRectPresentDest.push_back( CreateRect( 0, 0, 0, 0 ) );		// NULL
}


//////////////////////////////////////////////////////////////////////
// Name:	InitDialogs()
// Desc:	build the vector of dialog boxes to test
//////////////////////////////////////////////////////////////////////
void CDialogBoxMode::InitWindows()
{
	DialogBoxDescription dlgDescription;
	VECINT vecDialogs;

	m_vecIntWindows.clear();
	m_vecWindowDescription.clear();

	// 1: half width, half height, top-left
	vecDialogs.clear();
	dlgDescription.hDlg = CreateDialogParam( m_pFramework->m_hInstance, MAKEINTRESOURCE(IDD_SETDIALOG1), m_hWndReset, (DLGPROC)TestProc, 0 );
	dlgDescription.bAbsolutePosition = false;
	dlgDescription.indexXScale = 2;
	dlgDescription.indexYScale = 2;
	dlgDescription.indexOffset = 1;
	m_vecWindowDescription.push_back( dlgDescription );

	// 2: half width, half height, centered
	vecDialogs.clear();
	dlgDescription.hDlg = CreateDialogParam( m_pFramework->m_hInstance, MAKEINTRESOURCE(IDD_SETDIALOG1), m_hWndReset, (DLGPROC)TestProc, 0 );
	dlgDescription.bAbsolutePosition = false;
	dlgDescription.indexXScale = 1;
	dlgDescription.indexYScale = 1;
	dlgDescription.indexOffset = 0;
	m_vecWindowDescription.push_back( dlgDescription );

	// 3: full width, full height, origin
	vecDialogs.clear();
	dlgDescription.hDlg = CreateDialogParam( m_pFramework->m_hInstance, MAKEINTRESOURCE(IDD_SETDIALOG1), m_hWndReset, (DLGPROC)TestProc, 0 );
	dlgDescription.bAbsolutePosition = false;
	dlgDescription.indexXScale = 1;
	dlgDescription.indexYScale = 1;
	dlgDescription.indexOffset = 1;
	m_vecWindowDescription.push_back( dlgDescription );

	// 4: full width, half height, origin
	vecDialogs.clear();
	dlgDescription.hDlg = CreateDialogParam( m_pFramework->m_hInstance, MAKEINTRESOURCE(IDD_SETDIALOG1), m_hWndReset, (DLGPROC)TestProc, 0 );
	dlgDescription.bAbsolutePosition = false;
	dlgDescription.indexXScale = 1;
	dlgDescription.indexYScale = 2;
	dlgDescription.indexOffset = 1;
	m_vecWindowDescription.push_back( dlgDescription );

	// 5: hald width, full height, origin
	vecDialogs.clear();
	dlgDescription.hDlg = CreateDialogParam( m_pFramework->m_hInstance, MAKEINTRESOURCE(IDD_SETDIALOG1), m_hWndReset, (DLGPROC)TestProc, 0 );
	dlgDescription.bAbsolutePosition = false;
	dlgDescription.indexXScale = 2;
	dlgDescription.indexYScale = 1;
	dlgDescription.indexOffset = 1;
	m_vecWindowDescription.push_back( dlgDescription );

	// 6: half width, third height, offset bottom right
	vecDialogs.clear();
	dlgDescription.hDlg = CreateDialogParam( m_pFramework->m_hInstance, MAKEINTRESOURCE(IDD_SETDIALOG1), m_hWndReset, (DLGPROC)TestProc, 0 );
	dlgDescription.bAbsolutePosition = false;
	dlgDescription.indexXScale = 2;
	dlgDescription.indexYScale = 4;
	dlgDescription.indexOffset = 9;
	m_vecWindowDescription.push_back( dlgDescription );

	// 1st dialog
	vecDialogs.clear();
	vecDialogs.push_back(0);
	m_vecIntWindows.push_back( vecDialogs );

	// 2nd dialog
	vecDialogs.clear();
	vecDialogs.push_back(1);
	m_vecIntWindows.push_back( vecDialogs );

	// 3rd dialogs
	vecDialogs.clear();
	vecDialogs.push_back(2);
	m_vecIntWindows.push_back( vecDialogs );

	// 4th dialogs
	vecDialogs.clear();
	vecDialogs.push_back(3);
	m_vecIntWindows.push_back( vecDialogs );

	// 5th dialogs
	vecDialogs.clear();
	vecDialogs.push_back(4);
	m_vecIntWindows.push_back( vecDialogs );

	// 6th dialogs
	vecDialogs.clear();
	vecDialogs.push_back(5);
	m_vecIntWindows.push_back( vecDialogs );

	// dialogs 1 & 2
	vecDialogs.clear();
	vecDialogs.push_back(0);
	vecDialogs.push_back(1);
	m_vecIntWindows.push_back( vecDialogs );
}


//////////////////////////////////////////////////////////////////////
// Name:	TestInitialize()
// Desc:	Init the testcase
//////////////////////////////////////////////////////////////////////
UINT CDialogBoxMode::TestInitialize()
{
	// test not supported in DCT
	return KeySet(_T("WHQL"))? D3DTESTINIT_SKIPALL:CPresentII::TestInitialize();
}


//////////////////////////////////////////////////////////////////////
// Name:	PixelInsideDialogs( POINT )
// Desc:	true if the pixel is inside Dialogs
//////////////////////////////////////////////////////////////////////
bool CDialogBoxMode::PixelInsideDialogs( POINT Pixel )
{
	if( m_vecPrivateData[m_nIndexPrivateData] > 0 )
		return false;	// dialogs not visible
	return CPresentII::PixelInsideDialogs( Pixel );
}


//////////////////////////////////////////////////////////////////////
// Name:	PrivateDataProcessing()
// Desc:	process private data case before rendering
//////////////////////////////////////////////////////////////////////
void CDialogBoxMode::PrivateDataProcessing()
{
	// the only special case is when Private Data is 1, disable dialogs
	Check_HRFail( m_pRenderingDevice->SetDialogBoxMode( m_vecPrivateData[m_nIndexPrivateData] == 0 ), "SetDialogBoxMode(TRUE/FALSE)" );
}


INT_PTR CALLBACK TestProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg) 
	{ 
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		if( LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL )
		{
			EndDialog( hWnd, LOWORD(wParam) );
			return TRUE;
		}
		break;
	} 
    return FALSE; 
}
