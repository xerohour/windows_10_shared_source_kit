#include "pch.h"

#include "WGFTestUIShell.h"
#include "mmsystem.h"
#include "resource.h"
#include <HelperFunctions.h>

#include <strsafe.h>

#define CONSOLE_PANE_WIDTH_DEFAULT	640
#define CONSOLE_PANE_HEIGHT_DEFAULT 240

enum STATUS_BAR_PARTS
{
	PART_TEST_PATH = 0,
	PART_TEST_NUMBER = 1,
	PART_TEST_CASE = 2,
	PART_TEST_RESULT = 3,
};

CWGFTestUIShell::CWGFTestUIShell() :
	m_bLaunchInStepMode( false ),
	m_bHaltOnFail( false ),
	m_bStartup( true ),
	m_bMobile( false ),
	m_LastResult( RESULT_NONE ),
	m_State( STATE_PAUSE ),
	m_hAccelerators( NULL ),
	m_bLockDevWindowSize( false ),
	m_bLockDevWindowAspect( false ),
	m_nNumDeviceWindows( 0 ),
	m_iRenderWindowWidth( DEFAULT_RENDER_WINDOW_WIDTH ),
	m_iRenderWindowHeight( DEFAULT_RENDER_WINDOW_HEIGHT ),
	m_bUpdateStatusBar( false ),
	m_bUpdateStatusBarPath( false ),
	m_nLastTestCase( 0 ),
	m_iStatusTabIndex( -1 ),
	m_iGroupsTabIndex( -1 ),
	m_iCasesTabIndex( -1 ),
	m_iLogTabIndex( -1 ),
	m_iMessageTabIndex( -1 ),
	m_nDelay( 0 ),
	m_bExiting( false ),
	m_hU32OverCUI( nullptr )
{
	memset( &m_hDevWndBmp, 0, sizeof( m_hDevWndBmp ) );
}

void CWGFTestUIShell::InitOptionVariables()
{
	// Register command line arguments
	int n = AddOptionCategory( _T( "Execution" ) );
	RegisterOptionVariable( _T( "Step" ), _T( "Pause execution after running first test case." ), &m_bLaunchInStepMode, false, OPTION_VALUE_CONSTANT, n );
	RegisterOptionVariable( _T( "Haltonfail" ), _T( "Pause execution whenever a failure occurs." ), &m_bHaltOnFail, false, 0, n );
	RegisterOptionVariable( _T( "HaltOnFinish" ), _T( "Pause after finishing the last test case" ), &m_bHaltOnFinish, false, 0, n );
	RegisterOptionVariable( _T( "Mobile" ), _T( "Run test on mobile" ), &m_bMobile, false, 0, n );
}

bool CWGFTestUIShell::AllowDebugOnFail()
{
	CTest *pTest = GetFramework()->GetCurrentTest();
	if( pTest  )
	{		
		TCHAR szBuffer[ 4096 ] = _T( "" );
		StringCchPrintf
		(
			szBuffer,
			ARRAYSIZE( szBuffer ),
			_T( "Test case failure in %s:%u\n\nSelect Yes to break, No to ignore, and Cancel to stop debug-on-fail handling." ),
			pTest->GetPath().ToString().c_str(),
			GetFramework()->GetCurrentTestCase()
		);

		int result = MessageBox
		(
			GetHWnd(),
			szBuffer,
			_T( "Debug On Fail" ),
			MB_YESNOCANCEL
		);
		
		switch( result )
		{
			case IDYES:
				return true;
			
			case IDNO:
				return false;
				
			case IDCANCEL:
				GetFramework()->SetOptionValueFromString( tstring( _T( "DebugOnFail" ) ), tstring( _T( "0" ) ) );
				return false;
		}
	}
		
	return false;
}

CWGFTestUIShell::~CWGFTestUIShell()
{
}

//////////////////////////////////////////////////////////////////////////
// CWGFTestUIShell
//////////////////////////////////////////////////////////////////////////

bool CWGFTestUIShell::Initialize()
{
	bool bLockRestore = m_bLockDevWindowSize;

	if (m_bMobile)
	{
		m_hU32OverCUI = LoadLibrary("U32OverCUI.dll");
		if (m_hU32OverCUI == nullptr)
		{
			WriteError(_T("U32OverCUI.dll not found."));
			return false;
		}

		U32OverCUI_GetDefaultWindowOptions pU32OverCUI_GetDefaultWindowOptions;
		pU32OverCUI_GetDefaultWindowOptions = (U32OverCUI_GetDefaultWindowOptions)GetProcAddress(m_hU32OverCUI, "U32OverCUI_GetDefaultWindowOptions");
		if (pU32OverCUI_GetDefaultWindowOptions == nullptr)
		{
			WriteError(_T("U32OverCUI_GetDefaultWindowOptions - Function not found in U32OverCUI.dll"));
			return false;
		}

		U32OverCUI_CreateWindow pU32OverCUI_CreateWindow;
		pU32OverCUI_CreateWindow = (U32OverCUI_CreateWindow)GetProcAddress(m_hU32OverCUI, "U32OverCUI_CreateWindow");
		if (pU32OverCUI_CreateWindow == nullptr)
		{
			WriteError(_T("U32OverCUI_CreateWindow - Function not found in U32OverCUI.dll"));
			return false;
		}

		U32OverCUI_WindowOptions windowOptions = pU32OverCUI_GetDefaultWindowOptions();
		windowOptions.bDoInitialPresent = false;
		windowOptions.bFullscreen = true;
		HWND wnd = pU32OverCUI_CreateWindow(windowOptions);

		if (wnd == nullptr)
		{
			WriteError(_T("Failed to create window"));
			return false;
		}

		m_hWnd = wnd;
	}
	else
	{
		static ATOM reg = 0;
		if( reg == 0 )
		{
			reg = WGFTestUIRegisterWndClass( _T( "WGFTestUIMainWindow" ), 0, LoadIcon( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDI_DIRECTX ) ), LoadCursor( NULL, IDC_ARROW ), GetSysColorBrush( COLOR_BTNFACE ) );
		}

		DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
		DWORD dwExStyle = WS_EX_APPWINDOW;
		
		m_bLockDevWindowSize = true;

		if( !CreateEx( dwExStyle,
			_T( "WGFTestUIMainWindow" ),
			GetFramework()->GetName(),
			dwStyle,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			NULL,
			LoadMenu( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDR_MAIN_MENU ) ),
					0 ) )
			return false;

		// Create a status bar			
		m_statusbar.Create( IDC_STATUS_BAR, this, 0, NULL );

		// Create a toolbar control with progress controls.
		int bids[] =
		{
			ID_EXECUTION_PLAY,
			ID_EXECUTION_PAUSE,
			ID_EXECUTION_REPLAY,
			ID_EXECUTION_STEPBACK,
			ID_EXECUTION_STEPFORWARD,
			ID_EXECUTION_PREVGROUP,
			ID_EXECUTION_NEXTGROUP,
			-1,
			0 // Zero terminated list of button IDs.
		};
		m_toolbar.CreateSimpleBitmapButtons( ID_TOOLBAR, this, bids, CCS_NORESIZE | CCS_NOPARENTALIGN );
		m_toolbar.SetIndent( 6 );
		m_toolbar.ShowWindow( SW_SHOW );
		m_toolbar.HideButton( ID_EXECUTION_PLAY, true );

		RECT rcLast;
		m_toolbar.GetRect( ID_EXECUTION_NEXTGROUP, &rcLast );

		//
		// Tabbed control windows.
		//								

		// Create tab control.
		m_tabControl.Create( ID_TABCONTROL, this, WS_CHILD, 0, 0, 1, 1 );

		// Create the tabbed windows.
		TCITEM item;
		item.mask = TCIF_TEXT | TCIF_PARAM;
		int iTabIndex = 0;

		// Initially set this as the parent so all notification messages
		// are sent to the main window.
		m_StatusWindow.Create( 0, this, 0, 0, 64, 32 );
		m_StatusWindow.SetParent( &m_tabControl );

		m_GroupsWindow.Create( ID_TESTGROUPS, this );
		m_GroupsWindow.SetParent( &m_tabControl );
		m_TestCasesWindow.Create( ID_TESTCASES, this, 0, 0, 1, 1 );
		m_TestCasesWindow.SetParent( &m_tabControl );
		m_LogWindow.Create( ID_LOG, this );
		m_LogWindow.SetParent( &m_tabControl );
		m_MessageWindow.Create( ID_LOG, this );
		m_MessageWindow.SetParent( &m_tabControl );

		item.pszText = _T( "Status" );
		item.lParam = (LPARAM) &m_StatusWindow;
		m_iStatusTabIndex = iTabIndex;
		m_tabControl.InsertItem( iTabIndex++, &item );

		// Don't initialize these tabs when running under whql
		if(GetFramework()->GetTestMode() != WGFTEST_MODE_WHQL)
		{
			item.pszText = _T( "Test Groups" );
			item.lParam = (LPARAM) &m_GroupsWindow;
			m_iGroupsTabIndex = iTabIndex;
			m_tabControl.InsertItem( iTabIndex++, &item );

			item.pszText = _T( "Test Cases" );
			item.lParam = (LPARAM) &m_TestCasesWindow;
			m_iCasesTabIndex = iTabIndex;
			m_tabControl.InsertItem( iTabIndex++, &item );
		}

		item.pszText = _T( "Log" );
		item.lParam = (LPARAM) &m_LogWindow;
		m_iLogTabIndex = iTabIndex;
		m_tabControl.InsertItem( iTabIndex++, &item );

		item.pszText = _T( "Message" );
		item.lParam = (LPARAM) &m_MessageWindow;
		m_iMessageTabIndex = iTabIndex;
		m_tabControl.InsertItem( iTabIndex++, &item );

		m_tabControl.SetCurSel( 0 );
		AdjustTabbedControls();
		m_tabControl.ShowWindow( SW_SHOW );

		// Create splitter control
		m_splitter.Create( ID_SPLITTER, this, 0, 0 );
		m_splitter.ShowWindow( SW_SHOW );

		// Show the status bar
		m_statusbar.ShowWindow( SW_SHOW );

		RecalcLayout( RECALC_TO_FIT_DEVICE_WINDOWS );

		// Load the accelerators.
		m_hAccelerators = LoadAccelerators( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDR_ACCELERATORS ) );
	}
	
	// Initialize the shell state
	SetState( m_bLaunchInStepMode ? STATE_STEPFORWARD : STATE_PLAY );
	
	if ( !m_bMobile )
	{
		m_bLockDevWindowSize = bLockRestore;

		if ( GetFramework()->IsHelpOnly() )
		{
			m_DlgHelp.Execute( this );
			return false;
		}

		ShowWindow( SW_SHOW );
	}

	return true;
}

void CWGFTestUIShell::Destroy()
{ 
	if (m_bMobile)
	{
		if (m_hU32OverCUI)
		{
			U32OverCUI_DestroyWindow pU32OverCUI_DestroyWindow;
			pU32OverCUI_DestroyWindow = (U32OverCUI_DestroyWindow)GetProcAddress(m_hU32OverCUI, "DestroyWindow");
			if (pU32OverCUI_DestroyWindow == nullptr)
			{
				WriteError(_T("DestroyWindow - Function not found in U32OverCUI.dll"));
				return;
			}
			else
			{
				pU32OverCUI_DestroyWindow(m_hWnd);
			}

			m_hWnd = nullptr;

			FreeLibrary(m_hU32OverCUI);
			m_hU32OverCUI = nullptr;
		}
	}
	else
	{
		if (m_hWnd)
		{
			DestroyWindow();
		}
	}
}

typedef struct _DW_ARRANGEMENT
{
	WORD nx;
	WORD ny;
} DW_ARRANGEMENT;

static const DW_ARRANGEMENT DevWinArrangements[] =
{
	{ 0, 0 }, // 0: 1x1
	{ 1, 1 }, // 1: 1x1
	{ 2, 1 }, // 2: 2x2
	{ 3, 1 }, // 3: 3x3
	{ 2, 2 }, // 4: 2x2
	{ 3, 2 }, // 5: 3x3
	{ 3, 2 }, // 6: 3x3
	{ 4, 2 }, // 7: 3x3
	{ 4, 2 }, // 8: 4x4
};

static DW_ARRANGEMENT GetDevWindowArrangement( int nNumWindows ) { return DevWinArrangements[nNumWindows]; }

bool CWGFTestUIShell::SetNumDeviceWindows( int n )
{
	if ( m_bMobile && n > 1 )
	{
		// If we are running on mobile, and the test is trying to use multiple windows, debugbreak since this isn't valid.
		DebugBreak();
	}

	if (m_bMobile)
	{
		return true;
	}

	int i;
	bool bRecalcLayout = false;
	
	bool bLockRestore = m_bLockDevWindowSize;
	m_bLockDevWindowSize = true;

	
	if( n > MAX_DEVICE_WINDOWS )
		return false;
		
	// Destroy any device windows beyond the desired count.
	for( i = n; i < m_nNumDeviceWindows; ++i )
		m_DeviceWindows[i].DestroyWindow();
	
	// Create any new windows needed.	
	for( i = m_nNumDeviceWindows; i < n; ++i )
	{
		if( !m_DeviceWindows[i].Create( m_iRenderWindowWidth, m_iRenderWindowHeight, this, 0 ) )
		{
			// Get rid of all device windows and return false.
			SetNumDeviceWindows( 0 );
			return false;
		}
		
		m_DeviceWindows[i].ShowWindow( SW_SHOW );
	}
	
	if( m_nNumDeviceWindows != n )
		bRecalcLayout = true;
		
	m_nNumDeviceWindows = n;

	if( m_nNumDeviceWindows )
	{
		RECT rcdw;
		m_DeviceWindows[0].GetWindowRect( &rcdw );
		int w = rcdw.right - rcdw.left;
		int h = rcdw.bottom - rcdw.top;
		m_iDeviceWindowAspect = (w==0)?0:1024 * h / w;
	}
	
	InitDevWindowPositions( false );
	
	if( bRecalcLayout )
		RecalcLayout( RECALC_TO_FIT_DEVICE_WINDOWS );
	
	m_bLockDevWindowSize = bLockRestore;
		
	return true;
}

HWND CWGFTestUIShell::GetDeviceRenderWindow( int nIndex )
{
	if ( m_bMobile )
	{
		return m_hWnd;
	}

	HWND hWnd = NULL;
	
	if( nIndex >= 0 && nIndex < m_nNumDeviceWindows )
	{
		hWnd = m_DeviceWindows[nIndex].m_RenderWindow.GetHWnd();
	}
	
	return hWnd;
}

HBITMAP CWGFTestUIShell::SetDeviceWindowBitmap( int nIndex, HBITMAP hBmp )
{
	if ( m_bMobile )
	{
		return NULL;
	}

	HBITMAP result = NULL;
	
	if( nIndex < m_nNumDeviceWindows )
		result = m_DeviceWindows[nIndex].SetBitmap( hBmp );
		
	return result;
}

void CWGFTestUIShell::SetDeviceTitle( int nIndex, LPCTSTR szTitle, LPCTSTR szDesc )
{
	if ( m_bMobile )
	{
		return;
	}

	if( nIndex >= 0 && nIndex < m_nNumDeviceWindows )
	{
		m_DeviceWindows[nIndex].SetDeviceTitle( szTitle, szDesc );
	}
}

void CWGFTestUIShell::SetDeviceWindowsResizeable( bool bResizeable )
{
	// BUGBUG - TODO...
}

void CWGFTestUIShell::InitDevWindowPositions( bool bResize )
{
	if ( m_bMobile )
	{
		return;
	}

	if( m_nNumDeviceWindows )
	{
		int x = 0, y = 0, w = 0, h = 0;
		RECT rcdw;
		DW_ARRANGEMENT dwa = GetDevWindowArrangement( m_nNumDeviceWindows );
		
		if( bResize )
		{
			RECT rcClient;
			GetClientRect( &rcClient );
			h = ( rcClient.bottom - m_iSplitterHeight ) / dwa.ny;
			
			if( m_bLockDevWindowAspect )
				w = 1024 * h / m_iDeviceWindowAspect;
			else
				w = ( rcClient.right - rcClient.left );
			w /= dwa.nx;
		}
		else
		{
			m_DeviceWindows[0].GetWindowRect( &rcdw );
			h = rcdw.bottom - rcdw.top;
			w = rcdw.right - rcdw.left;
		}
		
		// Position the device window(s) (center if the client is wider than the device window grid).
		int n = 0;
		for( int j = 0; n < m_nNumDeviceWindows && j < dwa.ny; ++j )
		{
			for( int i = 0; n < m_nNumDeviceWindows && i < dwa.nx; ++i )
			{
				m_DeviceWindows[n].SetWindowPos( HWND_TOP, x, y, w, h, ( bResize ? 0 : SWP_NOSIZE ) );
				++n;
				x += w;
			}
			x = 0;
			y += h;
		}
	}
}

void CWGFTestUIShell::AdjustTabbedControls()
{
	if ( m_bMobile )
	{
		return;
	}

	CWGFTestUIWindow *pCurTabbedWindow = GetTabbedWindow();
	if( pCurTabbedWindow )
	{
		RECT rc;
		m_tabControl.GetWindowRect( &rc );
		m_tabControl.ScreenToClient( &rc );
		m_tabControl.AdjustRect( false, &rc );
		pCurTabbedWindow->MoveWindow( rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, true );
		pCurTabbedWindow->ShowWindow( SW_SHOW );
	}
}

void CWGFTestUIShell::RecalcLayout( RecalcMethod rm )
{
	if ( m_bMobile )
	{
		return;
	}

	RECT rcClient;
	RECT rcStatus;
	int x = 0, y = 0, w = 0, h = 0;
	
	DW_ARRANGEMENT dwa = GetDevWindowArrangement( m_nNumDeviceWindows );

	if( m_nNumDeviceWindows )
	{
		if( rm != RECALC_TO_FIT_DEVICE_WINDOWS )
		{
			InitDevWindowPositions( !m_bLockDevWindowSize );
		}

		RECT rcdw;
		m_DeviceWindows[m_nNumDeviceWindows - 1].GetWindowRect( &rcdw );
		ScreenToClient( &rcdw );
		y = rcdw.bottom;
		m_DeviceWindows[dwa.nx - 1].GetWindowRect( &rcdw );
		ScreenToClient( &rcdw );
		w = rcdw.right;
	}
	else
	{
		w = CONSOLE_PANE_WIDTH_DEFAULT;
	}
	
	if( rm != RECALC_FROM_USER_ADJUSTMENT )
	{
		RECT rc;
		rc.top = 0;
		rc.left = 0;
		rc.bottom = y + CONSOLE_PANE_HEIGHT_DEFAULT;
		m_iSplitterHeight = rc.bottom - y;
			
		rc.right = w;
		DWORD dwStyle = (DWORD) GetWindowLong( GetHWnd(), GWL_STYLE );
		DWORD dwExStyle = (DWORD) GetWindowLong( GetHWnd(), GWL_EXSTYLE );
		AdjustWindowRectEx( &rc, dwStyle, true, dwExStyle );
		bool bLockRestore = m_bLockDevWindowSize;
		m_bLockDevWindowSize = true;
		w = rc.right - rc.left;
		h = rc.bottom - rc.top;
		SetWindowPos( HWND_TOP, 0, 0, w, h, SWP_NOMOVE );
		m_bLockDevWindowSize = bLockRestore;
		GetClientRect( &rcClient );
	}
	else
	{
		GetClientRect( &rcClient );
		
		if( m_nNumDeviceWindows > 0 )
		{
			m_splitter.ShowWindow( SW_SHOW );
			y = rcClient.bottom - m_iSplitterHeight;
		}
		else
		{
			m_splitter.ShowWindow( SW_HIDE );
			m_iSplitterHeight = rcClient.bottom - rcClient.top;
			y = 0;
		}
	}
	
	w = rcClient.right - rcClient.left;

	if( m_nNumDeviceWindows > 0 )
	{
		h = 4;
		m_splitter.SetWindowPos( HWND_TOP, x, y, w, h, 0 );
		m_splitter.InvalidateRect( NULL, true );
		y += h;
	}
	
	m_statusbar.GetWindowRect( &rcStatus );
	ScreenToClient( &rcStatus );

	// Position the play controls toolbar
	h = 28;
	m_toolbar.SetWindowPos( HWND_TOP, x, y, w, h, 0 );
	m_toolbar.InvalidateRect( NULL, false );

	y += h;					
	// Position the tab window
	h = rcStatus.top - y;
	m_tabControl.SetWindowPos( HWND_TOP, x, y, w, h, 0 );
	AdjustTabbedControls();

	// Adjust the status bar "parts"
	int parts[4];
	parts[3] = rcClient.right;
	parts[2] = parts[3] - 64;
	parts[0] = parts[2] / 2;
	parts[1] = parts[0] + parts[0] / 2;
	m_statusbar.SetParts( ARRAY_SIZE( parts ), parts );
}


bool CWGFTestUIShell::Update()
{
	if ( GetFramework()->IsStatusContinuable() != true )
	{
		if(m_bHaltOnFinish)
		{
			SetState(STATE_PAUSE);
			return true;
		}

		SetState( STATE_FINISHED );

		// Automatically exit the test application.
		if ( !m_bMobile )
		{
			DestroyWindow();
		}
		m_bExiting = true;

		return false;
	}


	if( m_bStartup )
	{
		m_bStartup = false;

		if ( !m_bMobile )
		{
			m_MessageWindow.Clear();
		}

		switch( GetFramework()->GoToFirstTestCase() )
		{
			case WGFTEST_STATUS_NOT_FOUND:
			{
				if( m_State == STATE_STEPFORWARD )
				{
					SetState( STATE_PAUSE );
				}
			} break;


			case WGFTEST_STATUS_OK:
			case WGFTEST_STATUS_ABORT_TEST:
			case WGFTEST_STATUS_SKIP_TEST:
			{
				if( m_State == STATE_STEPFORWARD )
				{
					SetState( STATE_PLAY_ONCE );
				}
			} break;


			case WGFTEST_STATUS_ABORT_TEST_USERINITIATED:
			case WGFTEST_STATUS_EXCEPTION_CAUGHT:
			default:
			{
				if(m_bHaltOnFinish)
				{
					SetState(STATE_PAUSE);
				}
				else
				{
					SetState( STATE_FINISHED );

					// Automatically exit the test application.
					if ( !m_bMobile )
					{
						DestroyWindow();
					}
					m_bExiting = true;

				}
			} break;
		}
		
		if ( !m_bMobile )
		{
			m_TestCasesWindow.Invalidate();
		}

		m_bUpdateStatusBar = true;
		m_bUpdateStatusBarPath = true;
	}
	
	CTest *pTest = GetFramework()->GetCurrentTest();
	
	if ( m_State != STATE_FINISHED )
	{
		TEST_RESULT result = RESULT_NONE;

		if ( !m_bMobile )
		{
			tstring strPath = GetFramework()->GetCurrentTestPath().ToString();

			if( m_bUpdateStatusBar || m_bUpdateStatusBarPath )
			{
				m_bUpdateStatusBar = false;

				if( m_bUpdateStatusBarPath )
				{
					m_bUpdateStatusBarPath = false;
					m_statusbar.SetText( PART_TEST_PATH | SBT_OWNERDRAW, NULL );
					m_statusbar.SetText( PART_TEST_NUMBER | SBT_OWNERDRAW, NULL );

					//update Test Cases List View in Test Cases tab when entering a new test
					m_TestCasesWindow.Invalidate();
				}

				if( pTest )
				{
					size_t nCaseNumber = GetFramework()->GetCurrentTestCase();

					m_statusbar.SetText( PART_TEST_CASE | SBT_OWNERDRAW, NULL );
					m_TestCasesWindow.SetCurrentTestCase();

					// Flush paint messages to update UI elements.
					FlushPaintMessages();
				}
			}
		}
		
		// Update status control			
		if ( !m_bMobile && pTest )
		{
			// Initialize standard status items
			m_StatusWindow.ClearItems( 3 );

			TCHAR szBuffer[ 1024 ] = _T( "" );
			StringCchPrintf
			(
				szBuffer,
				ARRAYSIZE( szBuffer ),
				_T( "$C%s" ),
				GetFramework()->GetCurrentTestPath().ToString().c_str()
			);

			m_StatusWindow.SetItem( _T( "Test" ), szBuffer );


			// Update standard status items
			const TCHAR *szResult;
			
			switch( m_LastResult )
			{
				case RESULT_PENDING:
					szResult = _T( "$BPending" );
					break;

				case RESULT_RETRY:
					szResult = _T( "$BRetry" );
					break;
					
				case RESULT_SKIP:
					szResult = _T( "$YSkip" );
					break;
					
				case RESULT_PASS:
					szResult = _T( "$GPass" );
					break;
					
				case RESULT_FAIL:
					szResult = _T( "$RFail" );
					break;
					
				case RESULT_ABORT:
					szResult = _T( "$RAbort" );
					break;
					
				default:
					szResult = _T( "" );
					break;
			}

		    m_StatusWindow.SetItem( _T( "Result" ), szResult );

			StringCchPrintf
			(
				szBuffer,
				ARRAYSIZE( szBuffer ),
				_T( "$CPass $x%i, $CFail $x%i, $CSkip $x%i, $CAbort $x%i" ),
				GetFramework()->GetPassCount(), 
				GetFramework()->GetFailCount(),
				GetFramework()->GetSkipCount(),
				GetFramework()->GetAbortCount()
			);


			m_StatusWindow.SetItem( _T( "Overall Results" ), szBuffer );
										
			switch( GetFramework()->GetStatus() )
			{
				case WGFTEST_STATUS_OK:
				case WGFTEST_STATUS_SKIP_TEST_CASE:
				{
					for( int n = 0; n < pTest->GetNumParameters(); ++n )
					{
						CTestCaseParameterBase *pParameter = pTest->GetTestCaseParameter( n );
						if( pParameter->IsActive() || pParameter->DefaultValueWasSet())
						{
							const tstring &strName = tstring( _T( "$Y" ) ) + pParameter->GetName();
							tstring strValue = pParameter->GetValueAsString();
							m_StatusWindow.SetItem( strName.c_str(),strValue.c_str());
						}
					}
				} break;


				case WGFTEST_STATUS_ABORT_TEST:
				{
					m_StatusWindow.SetItem( _T( "Status" ), _T( "$RTEST ABORTED" ) );
				} break;


				case WGFTEST_STATUS_SKIP_TEST:
				{
					m_StatusWindow.SetItem( _T( "Status" ), _T( "$YTEST SKIPPED" ) );
				} break;


				case WGFTEST_STATUS_ABORT_TEST_USERINITIATED:
				case WGFTEST_STATUS_EXCEPTION_CAUGHT:
				{
					if(m_bHaltOnFinish)
					{
						SetState(STATE_PAUSE);
						return true;
					}

					SetState( STATE_FINISHED );

					// Automatically exit the test application.
					if ( !m_bMobile )
					{
						DestroyWindow();
					}
					m_bExiting = true;

					return false;
				} break;
			}

			m_StatusWindow.Refresh();
		}

		if
		(
			m_nDelay == 0 &&
			( m_State == STATE_PLAY || m_State == STATE_PLAY_ONCE ) &&
			GetFramework()->GetStatus() == WGFTEST_STATUS_OK
		)
		{
			if ( !m_bMobile )
			{
				FlushPaintMessages();
			}
			
			result = GetFramework()->ExecuteTestCase();
			
			if( m_bHaltOnFail && ( result == RESULT_FAIL || result == RESULT_ABORT ) )
			{
				SetState( STATE_PAUSE );
			}

			if( m_State == STATE_PLAY_ONCE )
			{
				SetState( STATE_PAUSE );
			}
		}

		bool bUpdateStatus = pTest && GetFramework()->GetLastTestResult() != m_LastResult;
		
		m_nLastTestCase = GetFramework()->GetCurrentTestCase();
		m_LastResult = GetFramework()->GetLastTestResult();

		if( !m_bMobile && bUpdateStatus )
		{
			m_statusbar.SetText( PART_TEST_RESULT | SBT_OWNERDRAW, (LPCTSTR) &m_LastResult );
		}
	}
		
	DWORD tc = GetTickCount();
	static DWORD tcLast = 0;
	int dt;
	if( tcLast )
		dt = tc - tcLast;
	else
		dt = 0;
	tcLast = tc;
	if( m_State == STATE_PLAY )
	{
		// Major performance enhancement: Don't process messages more often
		// than every 100ms.
		if( m_nDelay )
			m_nDelay = dt >= m_nDelay ? 0 : m_nDelay - dt;

		static int tcFlush = 0;
		if( tc - tcFlush > 100 )
		{
			if ( !m_bMobile && FlushMessages() )
			{
				return false;
			}

			if ( m_bExiting )
			{
				return false;
			}

			tcFlush = tc;
		}
	}
	else
	{
		if( m_State == STATE_PAUSE )
		{
			WaitMessage();
		}

		if ( !m_bMobile && FlushMessages() )
		{
			return false;
		}

		if ( m_bExiting )
		{
			return false;
		}
	}
		

	//
	// Update the shell state.
	//

	switch( m_State )
	{
		case STATE_INITFAIL:
		{
			SetState( STATE_PAUSE );
		} break;

	
		case STATE_PLAY:
		{
			if( m_nDelay > 0 )
			{
				break;
			};

			WGFTEST_STATUS FrameworkStatus = GetFramework()->GetStatus();

			if
			(
				FrameworkStatus == WGFTEST_STATUS_ABORT_TEST ||
				FrameworkStatus == WGFTEST_STATUS_ABORT_TEST_USERINITIATED ||
				FrameworkStatus == WGFTEST_STATUS_EXCEPTION_CAUGHT
			)
			{
				if(m_bHaltOnFinish)
				{
					SetState(STATE_PAUSE);
				}
				else
				{
					SetState( STATE_FINISHED );

					if ( !m_bMobile )
					{
						// Automatically exit the test application.
						DestroyWindow();
					}
					m_bExiting = true;

					break;
				}
			}
			else if ( FrameworkStatus == WGFTEST_STATUS_SKIP_TEST )
			{
				if( GetFramework()->SkipToNextTestGroup() != WGFTEST_STATUS_NOT_FOUND )
				{
					m_bUpdateStatusBar = true;
					if ( !m_bMobile )
					{
						m_MessageWindow.Clear();
					}
					break;
				}
			}
			else if( FrameworkStatus == WGFTEST_STATUS_NOT_FOUND )
			{
				// Only happens if there are zero tests to run.
				// Just fall thru to later code that will exit.
			}
			else if( GetFramework()->GetTestResultPending() || GetFramework()->GetTestResultRetry() )
			{
				break;
			}
			else
			{
				if( GetFramework()->GoToNextTestCase() != WGFTEST_STATUS_NOT_FOUND )
				{
					m_bUpdateStatusBar = true;
					if ( !m_bMobile )
					{
						m_MessageWindow.Clear();
					}
					break;
				}
			}
			
			// In normal play mode the application automatically
			// exits after executing the last test case
			// (unless executing a user selected branch).
			if( m_GroupsWindow.HasTempScript() )
			{
				m_GroupsWindow.DeleteTempPlayList();
				SetState( STATE_PAUSE );
			}
			else
			if( m_TestCasesWindow.HasTempScript() )
			{
				m_TestCasesWindow.DeleteTempPlayList();
				SetState( STATE_PAUSE );
			}
			else
			{
				if(m_bHaltOnFinish)
				{
					SetState(STATE_PAUSE);
				}
				else
				{
					SetState( STATE_FINISHED );
				
					// Automatically exit the test application.
					if ( !m_bMobile )
					{
						DestroyWindow();
					}
					m_bExiting = true;

				}
			}
		} break;

			
		case STATE_REPLAY:
		{
			if( GetFramework()->ResetTestCase() == WGFTEST_STATUS_OK )
			{
				SetState( STATE_PLAY_ONCE );
				if ( !m_bMobile )
				{
					m_MessageWindow.Clear();
				}
			}
			else
			{
				SetState( STATE_PAUSE );
			}

			m_bUpdateStatusBar = true;
		} break;

			
		case STATE_STEPFORWARD:
		{
			bool bFound = true;

			WGFTEST_STATUS FrameworkStatus = GetFramework()->GetStatus();
			if
			(
				FrameworkStatus == WGFTEST_STATUS_SKIP_TEST ||
				FrameworkStatus == WGFTEST_STATUS_ABORT_TEST
			)
			{
				if( GetFramework()->SkipToNextTestGroup() == WGFTEST_STATUS_NOT_FOUND )
				{
					SetState( STATE_PAUSE );
					bFound = false;
					break;
				}
			}
			else
			{
				if( GetFramework()->GoToNextTestCase() == WGFTEST_STATUS_NOT_FOUND )
				{
					SetState( STATE_PAUSE );
					bFound = false;
					break;
				}
			}

			if( bFound )
			{
				m_MessageWindow.Clear();
				m_bUpdateStatusBar = true;
				SetState( STATE_PLAY_ONCE );
			}
		} break;

		
		case STATE_STEPBACK:
		{
			bool bFound = true;

			WGFTEST_STATUS FrameworkStatus = GetFramework()->GetStatus();

			if
			(
				FrameworkStatus == WGFTEST_STATUS_SKIP_TEST ||
				FrameworkStatus == WGFTEST_STATUS_ABORT_TEST
			)
			{
				if( GetFramework()->SkipToPrevTestGroup() == WGFTEST_STATUS_NOT_FOUND )
				{
					SetState( STATE_PAUSE );
					bFound = false;
					break;
				}
			}
			else
			{
				if( GetFramework()->GoToPrevTestCase() == WGFTEST_STATUS_NOT_FOUND )
				{
					SetState( STATE_PAUSE );
					bFound = false;
					break;
				}
			}
			
			if( bFound )
			{
				if ( !m_bMobile )
				{
					m_MessageWindow.Clear();
				}
				m_bUpdateStatusBar = true;
				SetState( STATE_PLAY_ONCE );
			}
		} break;

			
		case STATE_SKIPFORWARD:
		{
			bool bFound = true;
			
			if( GetFramework()->SkipToNextTestGroup() == WGFTEST_STATUS_NOT_FOUND )
			{
				SetState( STATE_PAUSE );
				bFound = false;
				break;
			}
			
			if( bFound )
			{
				if ( !m_bMobile )
				{
					m_MessageWindow.Clear();
				}
				m_bUpdateStatusBar = true;
				SetState( STATE_PLAY_ONCE );
			}
		} break;

			
		case STATE_SKIPBACK:
		{
			bool bFound = true;
			
			if( GetFramework()->SkipToPrevTestGroup() == WGFTEST_STATUS_NOT_FOUND )
			{
				SetState( STATE_PAUSE );
				bFound = false;
				break;
			}
			
			if( bFound )
			{
				if ( !m_bMobile )
				{
					m_MessageWindow.Clear();
				}
				m_bUpdateStatusBar = true;
				SetState( STATE_PLAY_ONCE );
			}
		} break;


		case STATE_FINISHED:
		{
		} break;
	}

	if( GetFramework()->IsNewGroup() && (STATE_PAUSE != m_State))
	{
		m_bUpdateStatusBarPath = true;
	}

	return true;
}


void CWGFTestUIShell::FlushPaintMessages()
{
	MSG msg;
	while( PeekMessage( &msg, NULL, WM_PAINT, WM_PAINT, PM_REMOVE ) )
	{
		DispatchMessage( &msg );
	}
}


bool CWGFTestUIShell::FlushMessages()
{
	MSG msg;
	bool bQuit = false;
	for( ; PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ); )
	{
		if( !TranslateAccelerator( GetHWnd(), m_hAccelerators, &msg ) )
			TranslateMessage( &msg );
		DispatchMessage( &msg );

		if( msg.message == WM_QUIT )
			bQuit = true;
	}
	
	return bQuit;
}

HWND CWGFTestUIShell::GetMainHWnd() const
{
	return GetHWnd();
}

void CWGFTestUIShell::SetTitleText( const TCHAR *szText )
{
	SetWindowText( szText );
}

void CWGFTestUIShell::StatusText( const TCHAR *szKey, const TCHAR *szText )
{
	m_StatusWindow.SetItem( szKey, szText );
}

void CWGFTestUIShell::DetailsText( const TCHAR * szHeader, const TCHAR * szKey, const TCHAR *szText )
{
	// BUGBUG - TODO...
}

void CWGFTestUIShell::LogText( const CHAR *szText )
{
	m_LogWindow.WriteText( szText );
}

void CWGFTestUIShell::LogText( const WCHAR *szText )
{
	LPSTR pText = NULL;
	COPYSTRING( pText, szText );
	m_LogWindow.WriteText( pText );
	SAFEDELETEA( pText );
	pText = NULL;
}

void CWGFTestUIShell::MessageText( const CHAR *szText )
{
	m_MessageWindow.WriteText( szText );
}

void CWGFTestUIShell::MessageText( const WCHAR *szText )
{
	LPSTR pText = NULL;
	COPYSTRING( pText, szText );
	m_MessageWindow.WriteText( pText );
	SAFEDELETEA( pText );
	pText = NULL;
}

void CWGFTestUIShell::ErrorText( const CHAR *szText )
{
	if( m_errorDialog.GetHWnd() == NULL )
	{
		m_errorDialog.Execute( this, szText, _T( "ERROR" ) );
		m_errorDialog.ShowWindow( SW_SHOW );
	}
	else
	{
		m_errorDialog.SetMessageText( szText );
	}

	tstring text = tstring("{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1033 {\\colortbl ;\\red255\\green0\\blue0;}" ) + tstring( "\\cf1ERROR: " ) + tstring( szText );
	m_MessageWindow.SetWindowText( text.c_str() );

	// Write the error to the log
	tstring logText( tstring( "ERROR: " ) + tstring( szText ) );
	LogText( logText.c_str() );

	SetFocus( m_errorDialog.GetHWnd() );
}

void CWGFTestUIShell::ErrorText( const WCHAR *szText )
{
	LPSTR pText = NULL;
	COPYSTRING( pText, szText );
	ErrorText( pText );
	SAFEDELETEA( pText );
	pText = NULL;
}

void CWGFTestUIShell::SetState( WGFTESTUI_STATE state )
{
	m_State = state;

	if ( !m_bMobile )
	{
		if ( state == STATE_PLAY )
		{
			m_toolbar.HideButton( ID_EXECUTION_PLAY, true );
			m_toolbar.HideButton( ID_EXECUTION_PAUSE, false );
		}
		else
		{
			m_toolbar.HideButton( ID_EXECUTION_PLAY, false );
			m_toolbar.HideButton( ID_EXECUTION_PAUSE, true );
		}
	}
}

CWGFTestUIWindow *CWGFTestUIShell::GetTabbedWindow()
{
	int iItem = m_tabControl.GetCurFocus();
	TCITEM tcitem;
	ZeroMemory(&tcitem, sizeof(tcitem));
	tcitem.mask = TCIF_PARAM;
	
	CWGFTestUIWindow * pTabbedWindow = NULL;
	
	if( m_tabControl.GetItem( iItem, &tcitem ) )
	{
		pTabbedWindow = ( CWGFTestUIWindow * )tcitem.lParam;
	}
	
	return pTabbedWindow;
}
	
LRESULT CWGFTestUIShell::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( m_bMobile )
	{
		return 0;
	}

	// Disable input if running in WHQL mode
	if(GetFramework()->GetTestMode() == WGFTEST_MODE_WHQL)
	{
		if(WM_COMMAND == uMsg || WM_KEYDOWN == uMsg)
			return 0;
	}


	switch( uMsg )
	{
		case WM_CLOSE:
		{
			if ( GetFramework()->GetStatus() != WGFTEST_STATUS_NOT_FOUND )
			{
				GetFramework()->SetStatus( WGFTEST_STATUS_ABORT_TEST_USERINITIATED );
			}

			return CWGFTestUIWindow::WindowProc( uMsg, wParam, lParam );
		} break;


		case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			return 0;
		} break;


		case WM_NOTIFY:
		{
			NMHDR *pnmh = (NMHDR *) lParam;
			
			switch( pnmh->idFrom )
			{
				case ID_SPLITTER:
					if( pnmh->code == 1 )
					{
						POINT pt;
						pt.x = 0;
						pt.y = m_splitter.GetDropPos();
						ScreenToClient( &pt );
						RECT rcClient;
						GetClientRect( &rcClient );
						m_iSplitterHeight = rcClient.bottom - pt.y;
						RecalcLayout( RECALC_FROM_USER_ADJUSTMENT );
					}
					break;
					
				case ID_TABCONTROL:
					switch( pnmh->code )
					{
						case TCN_SELCHANGING:
						{
							// Hide the current tabbed control.
							CWGFTestUIWindow *pWindow = GetTabbedWindow();
							pWindow->ShowWindow( SW_HIDE );
							break;
						}	
						case TCN_SELCHANGE:
						{
							// Show the new tabbed control.
							AdjustTabbedControls();
							break;
						}
					}
					break;
					
				case ID_TESTGROUPS:
					break;
				case ID_TESTCASES:
					break;
			}
			
			break;
		}
		
		case WM_COMMAND:
		{
			WORD wCode = HIWORD( wParam );
			WORD wID = LOWORD( wParam );

			// Kill playlist if user interferes with execution.
			switch( wID )
			{
				case ID_EXECUTION_PAUSE:
				case ID_EXECUTION_STEPBACK:
				case ID_EXECUTION_STEPFORWARD:
				case ID_EXECUTION_NEXTGROUP:
				case ID_EXECUTION_PREVGROUP:
				case ID_EXECUTION_REPLAY:
				case ID_EXECUTION_GOTOTESTCASE:
					m_GroupsWindow.DeleteTempPlayList();
					m_TestCasesWindow.DeleteTempPlayList();
					break;
			}
						
			switch( wID )
			{
				//
				// Execution
				//
				case ID_EXECUTION_PLAY:
					SetState( STATE_PLAY );
					break;
					
				case ID_EXECUTION_PAUSE:
					SetState( STATE_PAUSE );
					break;
					
				case ID_EXECUTION_STEPBACK:
					SetState( STATE_STEPBACK );
					break;
					
				case ID_EXECUTION_STEPFORWARD:
					SetState( STATE_STEPFORWARD );
					break;
					
				case ID_EXECUTION_NEXTGROUP:
					SetState( STATE_SKIPFORWARD );
					break;
					
				case ID_EXECUTION_PREVGROUP:
					SetState( STATE_SKIPBACK );
					break;
					
				case ID_EXECUTION_REPLAY:
				{
					SetState( STATE_REPLAY );
					break;
				}
				
				case ID_EXECUTION_GOTOTESTCASE:
				{
					m_DlgGoTo.m_nCaseNumber = GetFramework()->GetCurrentTestCase();
					m_DlgGoTo.m_Path = GetFramework()->GetCurrentTestPath();
				
					if( m_DlgGoTo.Execute( this ) == IDOK )
					{
						GetFramework()->GoToTestCase( (CTestGroupLeafNode *) GetFramework()->GetTestGroupNode( m_DlgGoTo.m_Path ), m_DlgGoTo.m_nCaseNumber );
						SetState( STATE_PLAY_ONCE );
					}
					break;
				}
				
				case ID_OPTIONS_OPTIONVARS:
				{
					const CPropertyManager *pOptionsManager = GetFramework()->GetOptionsPropertyManager();

					UINT optionsCount = pOptionsManager->GetPropertyCount();

					CProperty **pOptionsArray = new CProperty *[optionsCount];

					pOptionsManager->GetProperties(pOptionsArray, optionsCount);

					for(UINT index = 0; index < optionsCount; ++index)
					{
						const CProperty *pOptionDesc = pOptionsArray[index];
						const tstring &isHiddenString = pOptionDesc->GetAttribute(_T("IsHidden"));
						const tstring &isConstantString = pOptionDesc->GetAttribute(_T("IsConstant"));
						bool isHidden = ToLower(isHiddenString) == tstring(_T("yes"));
						bool isConstant = ToLower(isConstantString) == tstring(_T("yes"));
						if(!isHidden && !isConstant)
						{
							const tstring &strName = pOptionDesc->GetName();
							tstring strValue;
							if(pOptionDesc->GetValueString(strValue))
							{
								m_DlgOptions.m_Keys.SetKey( strName, strValue );
							}
						}
					}

					delete[](pOptionsArray);
							
					bool bRepeat = true;
					
					for( ; bRepeat && m_DlgOptions.Execute( this ) == IDOK; )
					{
						CStringKeys::Iterator it;
						bRepeat = false;
						for( it = m_DlgOptions.m_Keys.Begin(); it != m_DlgOptions.m_Keys.End(); ++it )
						{
							const tstring &strName = m_DlgOptions.m_Keys.GetIteratorKeyName( it );
							const tstring &strValue = m_DlgOptions.m_Keys.GetIteratorKeyValue( it );
							if( !GetFramework()->SetOptionValueFromString( strName, strValue ) )
							{
								bRepeat = true;
								WriteError( _T( "Option '%s' not found or value '%s' invalid." ), strName.c_str(), strValue.c_str() );
							}
						}
					}
					
					SetState( STATE_PAUSE );
					
					break;
				}

				case ID_OPTIONS_PARAMETERS:
				{
					CTest *pTest = GetFramework()->GetCurrentTest();
					if( pTest )
					{
						INT_PTR result;

						m_DlgParams.Init( pTest );

						bool bRepeat = true;
						
						for( ; bRepeat && ( result = m_DlgParams.Execute( this ) ) == IDOK; )
						{
							CStringKeys::Iterator it;
							bRepeat = false;
							for( it = m_DlgParams.m_Keys.Begin(); it != m_DlgParams.m_Keys.End(); ++it )
							{
								const tstring &strName = m_DlgParams.m_Keys.GetIteratorKeyName( it );
								const tstring &strValue = m_DlgParams.m_Keys.GetIteratorKeyValue( it );
								CTestCaseParameterBase *pParam = pTest->GetTestCaseParameter( strName );
								if( !pParam->SetValueFromString( strValue ) )
								{
									bRepeat = true;
									WriteError( _T( "Parameter '%s' could not be set to value '%s'." ), strName.c_str(), strValue.c_str() );
								}
							}	
                        }

						if( result == IDOK && GetFramework()->ResetTestCase() == WGFTEST_STATUS_OK)
						{
							SetState( STATE_PLAY_ONCE );
						}
						else
						{
							SetState( STATE_PAUSE );
						}
					}

					break;
				}

				case ID_HELP_COMMANDLINE:
				{
					m_DlgHelp.Execute( this );
				}
			}
			
			break;
		}
		
		case WM_SIZE:
		{
			m_statusbar.SendMessage( WM_SIZE, 0, 0 );
			m_toolbar.SendMessage( WM_SIZE, 0, 0 );
			
			RecalcLayout( RECALC_FROM_USER_ADJUSTMENT );
			break;
		}
	}

	return CWGFTestUIWindow::WindowProc( uMsg, wParam, lParam );
}


//////////////////////////////////////////////////////////////////////////
// CWGFTestUISplitter
//////////////////////////////////////////////////////////////////////////
bool CWGFTestUISplitter::Create( UINT uID, CWGFTestUIWindow *pParent, int nMarginTop, int nMarginBottom )
{
	static ATOM atom = 0;
	
	if( !atom )
		atom = WGFTestUIRegisterWndClass( _T( "WGFTestUISplitter" ), 0, NULL, LoadCursor( GetModuleHandle( NULL ), MAKEINTRESOURCE( ID_HSPLITTER ) ), GetSysColorBrush( COLOR_BTNFACE ) );
	
	if( !pParent )
		return false;
		
	RECT rcParent;
	pParent->GetClientRect( &rcParent );
	
	if( !CreateEx( 0, _T( "WGFTestUISplitter" ), NULL, WS_CHILD, 0, 0, rcParent.right, 6, pParent, (HMENU) (LONG_PTR) uID, 0 ) )
		return false;
	
	m_hbmp = LoadBitmap( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDB_SPLITTER ) );	
	m_hbrPattern = CreatePatternBrush( m_hbmp );

	m_nMarginTop = nMarginTop;
	m_nMarginBottom = nMarginBottom;

	return true;
}

LRESULT CWGFTestUISplitter::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_DESTROY:
			DeleteObject( m_hbmp );
			DeleteObject( m_hbrPattern );
			break;
			
		case WM_LBUTTONDOWN:
		{
			SetCapture( GetHWnd() );
			POINT pt;
			GetCursorPos( &pt );
			m_iCursorY = pt.y;
			GetWindowRect( &m_rcDrag );
			DrawMoveRect( false );
			break;
		}	
		case WM_MOUSEMOVE:
			if( GetCapture() == GetHWnd() )
			{
				// Draw move rectangle below cursor.
				POINT pt;
				GetCursorPos( &pt );
				
				if( pt.y != m_iCursorY )
				{
					m_iCursorY = pt.y;
					DrawMoveRect( true );
				}
			}
			break;
			
		case WM_CAPTURECHANGED:
			EraseMoveRect();
			break;
		
		case WM_LBUTTONUP:
			if( GetCapture() == GetHWnd() )
			{
				ReleaseCapture();
				
				// Notify the main window of the change
				NMHDR nmhdr;
				nmhdr.idFrom = (WPARAM) GetWindowLong( GetHWnd(), GWL_ID );
				nmhdr.hwndFrom = GetHWnd();
				nmhdr.code = 1;
				GetParent()->SendMessage( WM_NOTIFY, nmhdr.idFrom, (LPARAM) &nmhdr );
			}
			break;
	}
	
	return CWGFTestUIWindow::WindowProc( uMsg, wParam, lParam );
}

void CWGFTestUISplitter::EraseMoveRect()
{
	HDC hdc = ::GetDC( NULL );
	HGDIOBJ hold = SelectObject( hdc, m_hbrPattern );
	PatBlt( hdc, m_rcDrag.left, m_rcDrag.top, m_rcDrag.right - m_rcDrag.left, m_rcDrag.bottom - m_rcDrag.top, PATINVERT );
	SelectObject( hdc, hold );
	ReleaseDC( hdc );
}

void CWGFTestUISplitter::DrawMoveRect( bool bEraseOld )
{
	HDC hdc = ::GetDC( NULL );
	HGDIOBJ hold = SelectObject( hdc, m_hbrPattern );

	// Erase the old rect
	if( bEraseOld )	
		PatBlt( hdc, m_rcDrag.left, m_rcDrag.top, m_rcDrag.right - m_rcDrag.left, m_rcDrag.bottom - m_rcDrag.top, PATINVERT );

	RECT rcParent;
	GetParent()->GetClientRect( &rcParent );
	GetParent()->ClientToScreen( &rcParent );

	GetWindowRect( &m_rcDrag );
	int h = m_rcDrag.bottom - m_rcDrag.top;
	m_rcDrag.top = m_iCursorY - h / 2;
	
	if( m_rcDrag.top < rcParent.top + m_nMarginTop )
		m_rcDrag.top = rcParent.top + m_nMarginTop;
		
	if( m_rcDrag.top > rcParent.bottom - m_nMarginBottom )
		m_rcDrag.bottom = rcParent.bottom - m_nMarginBottom;
		
	m_rcDrag.bottom = m_rcDrag.top + h;
	
	// Paint the new rect
	PatBlt( hdc, m_rcDrag.left, m_rcDrag.top, m_rcDrag.right - m_rcDrag.left, m_rcDrag.bottom - m_rcDrag.top, PATINVERT );
	SelectObject( hdc, hold );
	ReleaseDC( hdc );
}


bool CWGFTestUIMainStatusBar::DrawItemCallback( CWGFTestUIWindow *pParent, LPDRAWITEMSTRUCT lpdis )
{
	HDC hDC = lpdis->hDC;
	LPCTSTR szText = (LPCTSTR) lpdis->itemData;
	CWGFTestUIWindow *pStatusBar = CWGFTestUIWindow::FromHWnd( lpdis->hwndItem );
	RECT rcText;
	rcText.left = lpdis->rcItem.left + 1;
	rcText.top = lpdis->rcItem.top + 1;
	rcText.right = lpdis->rcItem.right - 1;
	rcText.bottom = lpdis->rcItem.bottom - 1;
	
	int bm = SetBkMode( hDC, TRANSPARENT );
	
	switch( lpdis->itemID )
	{
		case PART_TEST_CASE:
		{
			CTest *pTest = GetFramework()->GetCurrentTest();
			
			if( pTest )
			{
				CHAR szBuffer[ 256 ] = "";
				StringCchPrintfA
				(
					szBuffer,
					ARRAYSIZE( szBuffer ),
					"Case %i:%i",
					GetFramework()->GetCurrentTestCase(),
					pTest->GetNumVariations()
				);

				DrawProgress
				(
					hDC,
					szBuffer,
					RGB( 0, 0, 192 ),
					&lpdis->rcItem,
					GetFramework()->GetCurrentTestCase(),
					pTest->GetNumVariations()
				);
			}
			break;
		}
		
		case PART_TEST_NUMBER:
		{
			CTest *pTest = GetFramework()->GetCurrentTest();
			
			if( pTest )
			{
				size_t nGroupNumber;
				
				if( GetFramework()->GetGroupNumber( GetFramework()->GetCurrentTestPath(), nGroupNumber ) )
				{
					CHAR szBuffer[ 256 ] = "";
					StringCchPrintf
					(
						szBuffer,
						ARRAYSIZE( szBuffer ),
						"Test %i:%i",
						nGroupNumber,
						GetFramework()->GetNumTests()
					);

					DrawProgress
					(
						hDC,
						szBuffer,
						RGB( 192, 0, 0 ),
						&lpdis->rcItem,
						nGroupNumber,
						GetFramework()->GetNumTests()
					);
				}
			}
			break;
		}	
		case PART_TEST_RESULT:
		{
			TEST_RESULT *ptr = (TEST_RESULT *) lpdis->itemData;
			switch( ptr->GetResultCode() )
			{
				case RESULT_PASS:
					szText = _T( "$gPASS" );
					break;
				case RESULT_FAIL:
					szText = _T( "$rFAIL" );
					break;
				case RESULT_ABORT:
					szText = _T( "$RABORT" );
					break;
				case RESULT_SKIP:
					szText = _T( "$bSKIP" );
					break;
				case RESULT_PENDING:
					szText = _T( "$cPENDING" );
					break;
				case RESULT_RETRY:
					szText = _T( "$cRETRY" );
					break;

				default:
					szText = _T( "" );
					break;
			}
					
			// Color the text
			WGFTestUIDrawText( hDC, szText, -1, &rcText, 0 );							
			break;
		}
		
		case PART_TEST_PATH:
		{
			tstring strPath = GetFramework()->GetCurrentTestPath().ToString();
			TCHAR szBuffer[MAX_PATH];
			if( FAILED( StringCbCopy( szBuffer, sizeof( szBuffer ), strPath.c_str() ) ) )
				szBuffer[0] = 0;
			// Draw the path string with ellipses if needed
			PathCompactPath( hDC, szBuffer, rcText.right - rcText.left );
			COLORREF cr = SetTextColor( hDC, RGB( 0, 0, 127 ) );
			WGFTestUIDrawText( hDC, szBuffer, -1, &rcText, 0 );
			SetTextColor( hDC, cr );
			break;
		}
	}

	SetBkMode( hDC, bm );
	
	return true;
}


void CWGFTestUIMainStatusBar::DrawProgress( HDC hdc, LPCTSTR szText, COLORREF cr, LPRECT prc, size_t iValue, size_t iMax )
{
	if( iMax == 0 )
		return;
	
	if( iValue > iMax )
		iValue = iMax;
			
	size_t nPct = ( 1024 * iValue ) / iMax;

	// Draw the progress rect
	RECT rcProgress;
	HRGN hRgn;
	rcProgress.left = prc->left;
	rcProgress.top = prc->top;
	rcProgress.bottom = prc->bottom;
	rcProgress.right = rcProgress.left + (int) ( ( prc->right - prc->left ) * nPct ) / 1024;
	HBRUSH hbr = CreateSolidBrush( cr );
	
	FillRect( hdc, &rcProgress, hbr );
	
	// Draw the progress text (left half)
	hRgn = CreateRectRgn( rcProgress.left, prc->top, rcProgress.right, prc->bottom );
	SelectClipRgn( hdc, hRgn );
	SetTextColor( hdc, RGB( 255, 255, 255 ) );
	DrawText( hdc, szText, -1, prc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	DeleteObject( hRgn );
	
	// Draw the progress text (right half)
	hRgn = CreateRectRgn( rcProgress.right, prc->top, prc->right, prc->bottom );
	SelectClipRgn( hdc, hRgn );
	SetTextColor( hdc, RGB( 0, 0, 0 ) );
	DrawText( hdc, szText, -1, prc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	DeleteObject( hRgn );
	DeleteObject( hbr );
}


INT_PTR CWGFTestUIMessageDialog::Execute( CWGFTestUIWindow *pParent, const tstring &messageText, const tstring &messageTitle  )
{
	m_messageTitle = messageTitle;
	m_messageText = messageText;

	if( Create( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDD_DIALOG_MESSAGE ), pParent ) == false )
		return -1;
	SetWindowText( m_messageTitle.c_str() );
	return 0;
}

void CWGFTestUIMessageDialog::SetMessageText( const tstring &messageText )
{
	m_messageText = messageText;
	SetDlgItemText( IDC_EDIT_MESSAGE, m_messageText.c_str() );			
}

INT_PTR CWGFTestUIMessageDialog::DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
        case WM_INITDIALOG:
        {
			SetDlgItemText( IDC_EDIT_MESSAGE, m_messageText.c_str() );			
            return TRUE;
		}
		
		case WM_COMMAND:
		{
			WORD wCode = HIWORD( wParam );
			WORD wID = LOWORD( wParam );
			switch( wID )
			{
				case IDOK:
				case IDCANCEL:
					DestroyWindow();
					break;
			}
			break;
		}
	}
	
	return CWGFTestUIDialog::DialogProc( uMsg, wParam, lParam );
}

