#include "WindowFrameworkPCH.h"

#define NTSTATUS LONG // To get around ntdef.h/ntstatus.h inclusion issues
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

// Base headers
#include <d3dkmthk.h>

// C Runtime headers
#include <crtdbg.h>
#include <math.h>

// Win32 headers
#include <RichEdit.h>
#include <powrprof.h>

// Project headers
#include "TestUtils.h"

// StrSafe.h has to go last.
#include <strsafe.h>


#define DEVMODE_EQUALS(w,h,mode) \
	(mode.dmPelsHeight == h && mode.dmPelsWidth == w) ? true : false

// Create a list of all possible device types
DWORD	g_dwDevTypes[] = { DEVICETYPE_HAL, DEVICETYPE_REF, DEVICETYPE_SW, DEVICETYPE_NULLREF };
int		g_nDeviceTypes = (int)(sizeof(g_dwDevTypes) / sizeof(DWORD));
float	g_pVersions[] = { 9.1f, 9.f, 8.1f, 8.f, 7.f, 6.f };
int		g_nVersions = 	sizeof(g_pVersions) / sizeof(float);

// Local function / types
typedef BOOL (WINAPI *MONITORENUMPROC)(HMONITOR, HDC, LPRECT, LPARAM);
typedef BOOL (WINAPI *ENUMMONITORPROC)(HDC, LPCRECT, MONITORENUMPROC, LPARAM);
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
typedef BOOL (WINAPI * DEBUGFUNC) (void);

#if defined(_PREFAST_)
#pragma prefast(disable: 12110)
#pragma prefast(disable: 38030)
#endif

// Code Coverage
COVERAGEINITIALIZE		CoverageInitialize;
COVERAGERESET			CoverageReset;
COVERAGESAVE			CoverageSave;
COVERAGEUNINITIALIZE	CoverageUninitialize;


// --------------------------------------------------------------------------------------

UINT GetBitCountForFormat
(
	FMT Format
)
{
	DWORD dwBPP = 0;

	switch ( Format )
	{
	case FMT_R5G6B5:
		{
			dwBPP = 16;
		} break;


	case FMT_X8R8G8B8:
	case FMT_A2R10G10B10:
	case FMT_A2B10G10R10_XR_BIAS:
		{
			dwBPP = 32;
		} break;


	default:
		{
		} break;
	};

	return dwBPP;
};


// N.B. [SEdmison]:
// This function is an enormous hack to tell us whether we're
// running under the HCK version of WTT, because the HCK version
// and the ATLAS version have different handling of blocked test
// cases (and in particular whether blocked results with no
// failed results should cause the job result status to be set
// to Investigate or Cancelled).  This hack should be removed
// when the HCK team updates their version of WTT to include the
// fixed result status logic from the ATLAS version.
bool RunningUnderWLK()
{
	TCHAR CurrentDirectory[ MAX_PATH ] = _T( "" );
	DWORD nCurrentDirectory = GetCurrentDirectory
	(
		ARRAYSIZE( CurrentDirectory ),
		CurrentDirectory
	);

	if ( nCurrentDirectory == 0 )
	{
		return false;
	};

	TCHAR FullCurrentDirectory[ MAX_PATH ] = _T( "" );
	DWORD nFullCurrentDirectory = GetFullPathName
	(
		CurrentDirectory,
		ARRAYSIZE( FullCurrentDirectory ),
		FullCurrentDirectory,
		NULL
	);

	if ( nFullCurrentDirectory == 0 )
	{
		return false;
	};

	if ( _tcsstr( FullCurrentDirectory, _T( "\\WLK\\" ) ) != NULL )
	{
		return true;
	};

	return false;
};


// --------------------------------------------------------------------------------------

CD3DWindowFramework::CD3DWindowFramework()
:
	m_bMuteLog( false ),
	m_dwWindowWidth( 512 ),
	m_dwWindowHeight( 384 ),
	m_dwWindowedStyle( WS_OVERLAPPED | WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_CLIPCHILDREN | WS_MAXIMIZEBOX | WS_MINIMIZEBOX ),
	m_dwFullscreenStyle( WS_POPUP ),
	m_nAdapter( 1 ),
	m_pDeviceMode( NULL ),
	m_nDeviceMode( 0 ),
	m_pCurrentDevMode( NULL ),
	m_dwCmdDevTypes( 0 ),
	m_dwCmdBehaviors( 0 ),
	m_dwExcludeDevTypes( 0 ),
	m_dwExcludeBehaviors( 0 ),
	m_bShowCompareWindow( false ),
	m_bExceptionHandling( true ),
	m_hInstance( NULL ),
	m_pSimpleLog( NULL ),
	m_pD3D( NULL ),
	m_bStressMode( false ),
	m_bWHQL( false ),
	m_pDisplayWindow( NULL ),
	m_pLogWindow( NULL ),
	m_pStatWindow( NULL ),
	m_pMsgWindow( NULL ),
	m_pDetailsWindow( NULL ),
	m_pTabWindow( NULL ),
	m_pRebar( NULL ),
	m_pStatusBar( NULL ),
	m_pHelpWindow( NULL ),
	m_nCurrentAdapter( 0 ),
	m_dwCurrentDispWidth( 0 ),
	m_dwCurrentDispHeight( 0 ),
	m_bCurrentDispWindowed( false ),
	m_hMenu( NULL ),
	m_hAccel( NULL ),
	m_hRichEdit( NULL ),
	m_bActive( true ),
	m_bMaximizeDisplay( false ),
	m_bForceLog( false ),
	m_nAdapters( 0 ),
	m_pOrigModes( NULL ),
	m_pCurModes( NULL ),
	m_pDispModes( NULL ),
	m_bCoverage( false ),
	m_hCovDLL( NULL ),
	m_bDebug( false ),
	m_nStressDuration( 0 ),
	m_uExitCode( EXITCLASS_STANDARD | EXITCLASS_LOG ),
	m_pGetProcessIdOfThread( NULL ),
	m_hPSAPI( NULL ),
	m_pGetProcessImageFileName( NULL ),
	m_pTraceListenerThread( NULL ),
	m_hOriginalThreadDesktop( NULL ),
	m_hOriginalInputDesktop( NULL ),
	m_hCustomDesktop( NULL ),
	m_bReenableDWM( false ),
	m_uHybridOverride( FORCEINTEGRATED )
{
	m_tStartTime = time(NULL);


	//
	// Initialize device information.
	//

	memset(&m_DeviceInfo, 0, sizeof(DXGLOG_DEVICEINFO));


	//
	// Initialize logger options.
	//

	memset(&m_LogInfo, 0, sizeof(DXGLOG_INFO));
	m_LogInfo.dwOptions = DXGLOG_OPTION_OUTPUT_XML | DXGLOG_OPTION_UNICODE;
	m_LogInfo.uMaxGroupPasses = 0;
	m_LogInfo.uMaxLogFilePasses = 0;
	m_LogInfo.uMaxGroupSkips = 0;
	m_LogInfo.uMaxLogFileSkips = 0;
	m_LogInfo.uMaxGroupFailures = 100;
	m_LogInfo.uMaxLogFileFailures = DXGLOG_MAX_TRACES;
	m_LogInfo.uMaxGroupAborts = 100;
	m_LogInfo.uMaxLogFileAborts = 200;
	m_LogInfo.uMaxGroupWarnings = 100;
	m_LogInfo.uMaxLogFileWarnings = 200;


	//
	// Set the global framework object pointer.
	//

	DeclareFramework(this);


	//
	// Look for GetProcessIdOfThread in kernel32.  We do the
	// GetProcAddress because XP didn't include this API.
	//

	HMODULE hKernel32 = GetModuleHandle( "kernel32.dll" );
	if ( hKernel32 != NULL )
	{
		m_pGetProcessIdOfThread = (PGETPROCESSIDOFTHREAD) GetProcAddress
		(
			hKernel32,
			"GetProcessIdOfThread"
		);

		hKernel32 = NULL;
	};


	//
	// Load PSAPI.dll.
	//

	m_hPSAPI = LoadLibraryExW( L"psapi.dll", NULL, 0 );
	if ( m_hPSAPI != NULL )
	{
		m_pGetProcessImageFileName = (PGETPROCESSIMAGEFILENAME) GetProcAddress
		(
			m_hPSAPI,
			GETPROCESSIMAGEFILENAME
		);
	};


#ifdef _DEBUG
	int tmp = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmp |= _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF;
	_CrtSetDbgFlag(tmp);
#endif
}

// --------------------------------------------------------------------------------------

CD3DWindowFramework::~CD3DWindowFramework()
{
	//
	// Unregister the global framework object.
	//

	DeclareFramework( NULL );


	//
	// Free RichEd20.dll.
	//

	if ( NULL != m_hRichEdit )
	{
		FreeLibrary( m_hRichEdit );
		m_hRichEdit = NULL;
	};


	//
	// Free PSAPI.dll.
	//

	if ( m_hPSAPI != NULL )
	{
		FreeLibrary( m_hPSAPI );
		m_hPSAPI = NULL;
	};
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::SetLogLevel(UINT uLevel)
{
	m_uLogLevel = uLevel;

	DebugOut.SetLevel(m_uLogLevel);
	m_pStatWindow->SetLogLevel(m_uLogLevel);
	m_pLogWindow->SetLogLevel(m_uLogLevel);
	m_DXGLog.SetLogLevel(m_uLogLevel);
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::DebugPrint(UINT uLevel, LPCTSTR ptcsFmt, ...)
{
	if(m_bMuteLog)
		return;

	if(ptcsFmt)
	{
		va_list	argList;

		va_start(argList, ptcsFmt);
		TCHAR* pcsOut = GetVarArgString(ptcsFmt, argList);
		va_end(argList);

		if(pcsOut)
		{
			m_DXGLog.TraceWhole(uLevel, pcsOut);
			DebugOut.WriteWhole(uLevel, pcsOut);

			SAFEDELETEA(pcsOut);
		}
	}
}


///////////////////////////////////////////////////////
// Function:  EnablePrivilege
//
// Purpose:
// Asserts the specified security privilege for the
// process.
///////////////////////////////////////////////////////
bool CD3DWindowFramework::EnablePrivilege
(
	__in LPCTSTR PrivilegeName
)
{
	//
	// Open the process token.
	//

	HANDLE hToken = NULL;
	BOOL bOpenTokenResult = OpenProcessToken
	(
		GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken
	);

	if
	(
		bOpenTokenResult != TRUE ||
		hToken == NULL ||
		hToken == INVALID_HANDLE_VALUE
	)
	{
		return false;
	};


	//
	// Initialize the token privilege structure.
	//

	TOKEN_PRIVILEGES tkp;
	ZeroMemory( &tkp, sizeof( tkp ) );


	//
	// Lookup the specified privilege.
	//

	LookupPrivilegeValue
	(
		NULL,
		PrivilegeName,
		&tkp.Privileges[0].Luid
	); 


	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;   


	//
	// Enable the privlege for the process.
	//

	BOOL bAdjustPrivilegesResult = AdjustTokenPrivileges
	(
		hToken,
		FALSE,
		&tkp,
		sizeof( tkp ),
		(PTOKEN_PRIVILEGES) NULL,
		0
	);


	//
	// Close the token handle.
	//

	CloseHandle( hToken );
	hToken = NULL;


	if ( bAdjustPrivilegesResult != TRUE )
	{
		DWORD AdjustPrivilegesError = GetLastError();

		if ( AdjustPrivilegesError != ERROR_SUCCESS )
		{
			return false;
		};
	};


	return true;
};


// --------------------------------------------------------------------------------------

int CD3DWindowFramework::MsgBox(UINT uType, LPCTSTR ptcsMsg, ...)
{
	int nRet = 0;
	if(ptcsMsg)
	{
		va_list	argList;

		va_start(argList, ptcsMsg);
		TCHAR* pcsOut = GetVarArgString(ptcsMsg, argList);
		va_end(argList);

		if(pcsOut)
		{
			nRet = MessageBox(m_hWindow, pcsOut, _T("D3D Framework"), uType);
			SAFEDELETEA(pcsOut);
		}
	}

	return nRet;
}

// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::LogDeviceInfo(CDevice *pDevice)
{
	//
	// Initialize locals.
	//

	PADAPTER pAdapter = NULL;
	TCHAR tcsBuffer[256];
	TCHAR tcsFmt[32];
	TCHAR * sUnknown = _T("Unknown");
	TCHAR* pcsVPEmu = _T("");
	TCHAR* pcsPPEmu = _T("");

	WriteStatusBar(0,"Updating log with D3D info...");

	//
	// If we have a device, get its adapter.
	//

	if ( pDevice != NULL )
	{
		pAdapter = pDevice->GetAdapter();
	};


	//
	// Set device info fields.
	//

	COPYSTRING(m_DeviceInfo.sDisplay, NULL == pDevice ? sUnknown : pDevice->GetAdapter()->sAdapterName);
	COPYSTRING(m_DeviceInfo.sDriver, NULL == pDevice ? sUnknown : pDevice->GetAdapter()->sDriver);
	COPYSTRING(m_DeviceInfo.sDriverVersion, NULL == pDevice ? sUnknown : pDevice->GetAdapter()->sVersion);
	m_DeviceInfo.dwVendorID	= NULL == pDevice ? 0 : pDevice->GetAdapter()->dwVendorID;
	m_DeviceInfo.dwDeviceID	= NULL == pDevice ? 0 : pDevice->GetAdapter()->dwChipID;
	COPYSTRING(m_DeviceInfo.sPnPID, NULL == pDevice ? sUnknown : pDevice->GetAdapter()->sPnPID);
	m_DeviceInfo.nDDI 			= NULL == pDevice ? 0 : pDevice->GetDDI();
	m_DeviceInfo.fVersion		= NULL == pDevice ? 0.f : pDevice->GetD3D()->GetMaxVersion();
	m_DeviceInfo.fBuild 		= NULL == pDevice ? 0.f : pDevice->GetD3D()->GetBuild();
	m_DeviceInfo.bDebug		= NULL == pDevice ? false : pDevice->GetD3D()->GetDebug();
	m_DeviceInfo.bPSGP		= NULL == pDevice ? false : pDevice->IsPSGPEnabled();
	m_DeviceInfo.fInterface	= NULL == pDevice ? 0.f : pDevice->GetInterface();
	m_DeviceInfo.dwDispMode	= NULL == pDevice ? 0 : (DWORD)pAdapter->DesktopMode.Format.d3dfFormat;
	m_DeviceInfo.wDesktopBPP= NULL == pDevice ? 0 : (WORD)pAdapter->DesktopMode.Format.ddpfPixelFormat.dwRGBBitCount;

	if ( NULL == pDevice ? false : pDevice->IsVPEmuEnabled() )
	{
		pcsVPEmu = _T(", VP Emu");
	}
	if ( NULL == pDevice ? false : pDevice->IsPPEmuEnabled() )
	{
		pcsPPEmu = _T(", PP Emu");
	}

	StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("%s%s%s"), NULL == pDevice ? sUnknown : pDevice->GetDescription(), pcsVPEmu, pcsPPEmu);

	COPYSTRING(m_DeviceInfo.sDevice, tcsBuffer);


	if ( pAdapter != NULL )
	{
		FmtToString
		(
			FORMAT_TEXTURE,
			&pAdapter->DesktopMode.Format,
			tcsFmt
		);

		StringCchPrintf
		(
			tcsBuffer,
			ARRAYSIZE(tcsBuffer),
			_T("%d x %d x %s @ %d Hz"),
			pAdapter->DesktopMode.dwWidth,
			pAdapter->DesktopMode.dwHeight, 
			tcsFmt,
			pAdapter->DesktopMode.dwRefreshRate
		);

		COPYSTRING(m_DeviceInfo.sResolution, tcsBuffer);
	}
	else
	{
		COPYSTRING(m_DeviceInfo.sResolution, sUnknown);
	};


	//
	// Log the device information.
	//

	if ( FAILED( m_DXGLog.LogDeviceInfo( &m_DeviceInfo ) ) )
	{
		DPF
		(
			1,
			_T("CD3DWindowFramework::LogDeviceInfo:  LogDeviceInfo failed.\n")
		);

		return false;	
	};


	return true;
};


// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::CreateTabControl()
{
	//
	// Initialize locals.
	//

	bool bOverallResult = false;

	CTabWindow * pTabWindow = NULL;
	CLogWindow * pStatWindow = NULL;
	CLogWindow * pLogWindow = NULL;
	CLogWindow * pDetailsWindow = NULL;
	CLogWindow * pMsgWindow = NULL;


	//
	// Create the tab control.
	//

	pTabWindow = new CTabWindow();
	if ( pTabWindow == NULL )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateTabControl:  Failed to allocate tab window.\n" )
		);

		goto Cleanup;
	};


	if
	(
		!pTabWindow->InitializeWindow
		(
			0,
			WC_TABCONTROL,
			"",
			(HMENU) WND_TAB,
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
			0,
			0,
			200,
			200,
			m_hWindow
		)
	)
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateTabControl:  InitializeWindow on tab control failed.\n" )
		);

		goto Cleanup;
	};


	if
	(
		!AddChild(_T("Tab"), pTabWindow)
	)
	{
		DPF(1, _T("CD3DWindowFramework::CreateTabControl:  AddChild on tab control failed.\n"));

		goto Cleanup;
	};


	// Now that we've hooked up the tab window to the hierarchy,
	// we can relinquish our reference to it.
	m_pTabWindow = pTabWindow;
	pTabWindow = NULL;


	//
	// Create the status window.
	//

	pStatWindow = new CLogWindow();
	if ( pStatWindow == NULL )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateTabControl:  Failed to allocate status window.\n" )
		);

		goto Cleanup;
	};


	if
	(
		!pStatWindow->InitializeWindow
		(
			WS_EX_CLIENTEDGE,
			RICHEDIT_CLASS,
			NULL,
			(HMENU) WND_STATUS,
			NULL,
			WS_VSCROLL | WS_CHILD | ES_AUTOVSCROLL | ES_READONLY | ES_AUTOHSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS,
			0,
			0,
			100,
			100,
			m_pTabWindow->m_hWindow
		)
	)
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateTabControl:  InitializeWindow on status window failed.  Falling back to RichEdit 1.0 window class.\n" )
		);

		if
		(
			!pStatWindow->InitializeWindow
			(
				WS_EX_CLIENTEDGE,
				_T( "RICHEDIT" ),
				NULL,
				(HMENU) WND_STATUS,
				NULL,
				WS_VSCROLL | WS_CHILD | ES_AUTOVSCROLL | ES_READONLY | ES_AUTOHSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS,
				0,
				0,
				100,
				100,
				m_pTabWindow->m_hWindow
			)
		)
		{
			DPF
			(
				1,
				_T( "CD3DWindowFramework::CreateTabControl:  InitializeWindow on status window failed with RichEdit window class.\n" )
			);

			goto Cleanup;
		};
	};


	if
	(
		!m_pTabWindow->AddChild(_T("Status"), pStatWindow)
	)
	{
		DPF(1, _T("CD3DWindowFramework::CreateTabControl:  AddChild on status window failed.\n"));

		goto Cleanup;
	};


	// Now that we've hooked up the status window to the hierarchy,
	// we can relinquish our reference to it.
	m_pStatWindow = pStatWindow;
	pStatWindow = NULL;


	//
	// Create the log window.
	//

	pLogWindow = new CLogWindow();
	if ( pLogWindow == NULL )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateTabControl:  Failed to allocate log window.\n" )
		);

		goto Cleanup;
	};


	if
	(
		!pLogWindow->InitializeWindow
		(
			WS_EX_CLIENTEDGE,
			RICHEDIT_CLASS,
			NULL,
			(HMENU) WND_LOG,
			NULL,
			WS_VSCROLL | WS_CHILD | ES_AUTOVSCROLL | ES_READONLY | ES_AUTOHSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS,
			0,
			0,
			100,
			100,
			m_pTabWindow->m_hWindow
		)
	)
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateTabControl:  InitializeWindow on log window failed.  Falling back to RichEdit 1.0 window class.\n" )
		);

		if
		(
			!pLogWindow->InitializeWindow
			(
				WS_EX_CLIENTEDGE,
				_T( "RICHEDIT" ),
				NULL,
				(HMENU) WND_LOG,
				NULL,
				WS_VSCROLL | WS_CHILD | ES_AUTOVSCROLL | ES_READONLY | ES_AUTOHSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS,
				0,
				0,
				100,
				100,
				m_pTabWindow->m_hWindow
			)
		)
		{
			DPF
			(
				1,
				_T( "CD3DWindowFramework::CreateTabControl:  InitializeWindow on log window failed with RichEdit window class.\n" )
			);

			goto Cleanup;
		};
	};

	if
	(
		!m_pTabWindow->AddChild(_T("Log"), pLogWindow)
	)
	{
		DPF(1, _T("CD3DWindowFramework::CreateTabControl:  AddChild on log window failed.\n"));

		goto Cleanup;
	};


	// Now that we've hooked up the log window to the hierarchy,
	// we can relinquish our reference to it.
	m_pLogWindow = pLogWindow;
	pLogWindow = NULL;


	//
	// Create the Details window.
	//

	pDetailsWindow = new CLogWindow();
	if ( pDetailsWindow == NULL )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateTabControl:  Failed to allocate details window.\n" )
		);

		goto Cleanup;
	};


	if
	(
		!pDetailsWindow->InitializeWindow
		(
			WS_EX_CLIENTEDGE,
			RICHEDIT_CLASS,
			NULL,
			(HMENU) WND_LOG,
			NULL,
			WS_VSCROLL | WS_CHILD | ES_AUTOVSCROLL | ES_READONLY | ES_AUTOHSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS,
			0,
			0,
			100,
			100,
			m_pTabWindow->m_hWindow
		)
	)
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateTabControl:  InitializeWindow on details window failed.  Falling back to RichEdit 1.0 window class.\n" )
		);

		if
		(
			!pDetailsWindow->InitializeWindow
			(
				WS_EX_CLIENTEDGE,
				_T( "RICHEDIT" ),
				NULL,
				(HMENU) WND_LOG,
				NULL,
				WS_VSCROLL | WS_CHILD | ES_AUTOVSCROLL | ES_READONLY | ES_AUTOHSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS,
				0,
				0,
				100,
				100,
				m_pTabWindow->m_hWindow
			)
		)
		{
			DPF
			(
				1,
				_T( "CD3DWindowFramework::CreateTabControl:  InitializeWindow on details window failed with RichEdit window class.\n" )
			);

			goto Cleanup;
		};
	};


	if
	(
		!m_pTabWindow->AddChild(_T("Details"), pDetailsWindow)
	)
	{
		DPF(1, _T("CD3DWindowFramework::CreateTabControl:  AddChild on details window failed.\n"));

		goto Cleanup;
	};


	// Now that we've hooked up the details window to the hierarchy,
	// we can relinquish our reference to it.
	m_pDetailsWindow = pDetailsWindow;
	pDetailsWindow = NULL;


	//
	// Create the Message window.
	//

	pMsgWindow = new CLogWindow();
	if ( pMsgWindow == NULL )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateTabControl:  Failed to allocate message window.\n" )
		);

		goto Cleanup;
	};


	if
	(
		!pMsgWindow->InitializeWindow
		(
			WS_EX_CLIENTEDGE,
			RICHEDIT_CLASS,
			NULL,
			(HMENU) WND_MSG,
			NULL,
			WS_VSCROLL | WS_CHILD | ES_AUTOVSCROLL | ES_READONLY | ES_AUTOHSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS,
			0,
			0,
			100,
			100,
			m_pTabWindow->m_hWindow
		)
	)
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateTabControl:  InitializeWindow on message window failed.  Falling back to RichEdit 1.0 window class.\n" )
		);

		if
		(
			!pMsgWindow->InitializeWindow
			(
				WS_EX_CLIENTEDGE,
				_T( "RICHEDIT" ),
				NULL,
				(HMENU) WND_MSG,
				NULL,
				WS_VSCROLL | WS_CHILD | ES_AUTOVSCROLL | ES_READONLY | ES_AUTOHSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS,
				0,
				0,
				100,
				100,
				m_pTabWindow->m_hWindow
			)
		)
		{
			DPF
			(
				1,
				_T( "CD3DWindowFramework::CreateTabControl:  InitializeWindow on message window failed with RichEdit window class.\n" )
			);

			goto Cleanup;
		};
	};


	if
	(
		!m_pTabWindow->AddChild(_T("Message"), pMsgWindow)
	)
	{
		DPF
		(
			1,
			_T("CD3DWindowFramework::CreateTabControl:  AddChild on message window failed.\n")
		);

		return false;
	};


	// Now that we've hooked up the message window to the hierarchy,
	// we can relinquish our reference to it.
	m_pMsgWindow = pMsgWindow;
	pMsgWindow = NULL;


	//
	// Set dimensions.
	//

	ASSERT( m_pStatWindow != NULL );
	m_pStatWindow->SetDimensions(1.f, 1.f);

	ASSERT( m_pLogWindow != NULL );
	m_pLogWindow->SetDimensions(1.f, 1.f);

	ASSERT( m_pDetailsWindow != NULL );
	m_pDetailsWindow->SetDimensions(1.f, 1.f);

	ASSERT( m_pMsgWindow != NULL );
	m_pMsgWindow->SetDimensions(1.f, 1.f);


	//
	// Select the first tab.
	//

	m_pTabWindow->SelectTab(0);


	bOverallResult = true;


Cleanup:
	if ( bOverallResult != true )
	{
		if ( m_pTabWindow != NULL )
		{
			m_pTabWindow->Close();

			delete m_pTabWindow;
			m_pTabWindow = NULL;
		};

		SAFEDELETE(pMsgWindow);
		SAFEDELETE(pDetailsWindow);
		SAFEDELETE(pLogWindow);
		SAFEDELETE(pStatWindow);
		SAFEDELETE(pTabWindow);
	};

	return bOverallResult;
}

// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::CreateRebarControl()
{
	//
	// Initialize locals.
	//

	bool bOverallResult = false;

	HWND hWnd = NULL;
	CRebarWindow * pRebarWindow = NULL;
	CWindow * pWindow = NULL;


	//
	// Create the rebar control.
	//

	pRebarWindow = new CRebarWindow();
	if ( pRebarWindow == NULL )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateRebarControl:  Failed to allocate rebar window.\n" )
		);

		goto Cleanup;
	};


	if
	(
		!pRebarWindow->InitializeWindow
		(
			WS_EX_TOOLWINDOW,
			REBARCLASSNAME,
			"Rebar",
			(HMENU) WND_REBAR,
			NULL,
			WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | RBS_VARHEIGHT | RBS_BANDBORDERS | CCS_NODIVIDER | CCS_NOPARENTALIGN,
			0,
			0,
			100,
			100,
			m_hWindow
		)
	)
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateRebarControl:  InitializeWindow on rebar window failed.\n" )
		);

		goto Cleanup;
	};


	if
	(
		!AddChild(_T("Rebar"), pRebarWindow)
	)
	{
		DPF
		(
			1,
			_T("CD3DWindowFramework::CreateRebarControl:  AddChild on rebar control failed.\n")
		);

		goto Cleanup;
	};


	// Now that we've hooked up the rebar window to the hierarchy,
	// we can relinquish our reference to it.
	m_pRebar = pRebarWindow;
	pRebarWindow = NULL;


	m_pRebar->SetFixed(true);


	//
	// Create an edit box.
	//

	hWnd = CreateWindowEx
	(
		WS_EX_CLIENTEDGE,
		RICHEDIT_CLASS,
		"",
		WS_CHILD | WS_VISIBLE | ES_RIGHT | CCS_NOPARENTALIGN,
		0,
		0,
		60,
		20,
		m_pRebar->m_hWindow,
		(HMENU) WND_TESTNUM,
		m_hInstance,
		NULL
	);

	if ( hWnd == NULL )
	{
		DWORD CreateEditBoxError = ::GetLastError();

		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateRebarControl:  CreateWindowEx failed with error %u.  Falling back to RichEdit 1.0 window class.\n" ),
			CreateEditBoxError
		);

		hWnd = CreateWindowEx
		(
			WS_EX_CLIENTEDGE,
			_T( "RichEdit" ),
			"",
			WS_CHILD | WS_VISIBLE | ES_RIGHT | CCS_NOPARENTALIGN,
			0,
			0,
			60,
			20,
			m_pRebar->m_hWindow,
			(HMENU) WND_TESTNUM,
			m_hInstance,
			NULL
		);

		if ( hWnd == NULL )
		{
			CreateEditBoxError = ::GetLastError();

			DPF
			(
				1,
				_T( "CD3DWindowFramework::CreateRebarControl:  CreateWindowEx failed with error %u on RichEdit window class.\n" ),
				CreateEditBoxError
			);

			goto Cleanup;
		};
	};


	pWindow = new CWindow(WND_TESTNUM, hWnd);
	if ( pWindow == NULL )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateRebarControl:  Failed to allocate edit box window.\n" )
		);

		DestroyWindow( hWnd );
		hWnd = NULL;

		goto Cleanup;
	};


	if
	(
		!m_pRebar->AddChild(_T("Test Number"), pWindow)
	)
	{
		DPF
		(
			1,
			_T("CD3DWindowFramework::CreateRebarControl:  AddChild on edit box window failed.\n")
		);

		goto Cleanup;
	};


	// Request keyboard event notification
	SendMessage(pWindow->m_hWindow, EM_SETEVENTMASK, 0, ENM_KEYEVENTS);

	// Add the control to the Rebar
	m_pRebar->AddBand(_T("Test:"), RBBS_CHILDEDGE, -1, hWnd, 0);


	// Now that we've hooked up the band to the rebar,
	// we can relinquish our reference to it (and its window).
	pWindow = NULL;
	hWnd = NULL;


	//
	// Create the toolbar.
	//

	const UINT nButtons = 6;
	const UINT nButtonXY = 16;
	TBBUTTON tb[nButtons];
	ZeroMemory(tb, sizeof(tb));
	tb[0].iBitmap = 0;tb[0].iString = 0;
	tb[0].fsState = TBSTATE_ENABLED;
	tb[0].fsStyle = TBSTYLE_BUTTON;
	tb[0].idCommand = IDM_EXECUTION_PREVIOUS;
	tb[1].iBitmap = 1;tb[1].iString = 1;
	tb[1].fsState = TBSTATE_ENABLED | TBSTATE_PRESSED;
	tb[1].fsStyle = TBSTYLE_BUTTON;
	tb[1].idCommand = IDM_EXECUTION_RUN;
	tb[2].iBitmap = 2; tb[2].iString = 2;
	tb[2].fsState = TBSTATE_ENABLED;
	tb[2].fsStyle = TBSTYLE_BUTTON;
	tb[2].idCommand = IDM_EXECUTION_PAUSE;
	tb[3].iBitmap = 3; tb[3].iString = 3;
	tb[3].fsState = TBSTATE_ENABLED;
	tb[3].fsStyle = TBSTYLE_BUTTON;
	tb[3].idCommand = IDM_EXECUTION_STEP;
	tb[4].iBitmap = 4; tb[4].iString = 4;
	tb[4].fsState = TBSTATE_ENABLED;
	tb[4].fsStyle = TBSTYLE_BUTTON;
	tb[4].idCommand = IDM_EXECUTION_REPLAY;
	tb[5].iBitmap = 5; tb[5].iString = 5;
	tb[5].fsState = TBSTATE_ENABLED;
	tb[5].fsStyle = TBSTYLE_BUTTON;
	tb[5].idCommand = IDM_EXECUTION_NEXT;


	hWnd = CreateToolbarEx
	(
		m_pRebar->m_hWindow,
		WS_CHILD | WS_VISIBLE | CCS_NORESIZE | CCS_NOPARENTALIGN | TBSTYLE_TOOLTIPS, WND_TOOLBAR,
		nButtons,
		m_hInstance,
		IDB_TOOLBAR,
		tb,
		nButtons,
		nButtonXY,
		nButtonXY,
		nButtonXY,
		nButtonXY,
		sizeof(TBBUTTON)
	);

	if ( hWnd == NULL )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateRebarControl:  CreateToolbarEx failed.\n" )
		);

		goto Cleanup;
	};


	pWindow = new CWindow(WND_TOOLBAR, hWnd);
	if ( pWindow == NULL )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::CreateRebarControl:  Failed to allocate toolbar window.\n" )
		);

		DestroyWindow( hWnd );
		hWnd = NULL;

		goto Cleanup;
	};


	if
	(
		!m_pRebar->AddChild(_T("Toolbar"), pWindow)
	)
	{
		DPF
		(
			1,
			_T("CD3DWindowFramework::CreateRebarControl:  AddChild on toolbar window failed.\n")
		);

		goto Cleanup;
	};


	SendMessage(hWnd, TB_SETMAXTEXTROWS, 0, 0);
	SendMessage(hWnd, TB_ADDSTRING, 0, (LPARAM)_T("Previous Group\0Run\0Pause\0Step\0Replay\0Next Group\0\0"));

	// Add the control to the Rebar
	m_pRebar->AddBand(_T(""), RBBS_CHILDEDGE, -1, hWnd, 1);


	// Now that we've hooked up the toolbar to the rebar,
	// we can relinquish our reference to it (and its window).
	pWindow = NULL;
	hWnd = NULL;


	//
	// Update the UI.
	//

	// Recalculate the dimensions.
	m_pRebar->CalculateSize();

	// Show the contructed rebar.
	m_pRebar->SetVisible(true);


	bOverallResult = true;


Cleanup:
	if ( bOverallResult != true )
	{
		SAFEDELETE( pRebarWindow );

		SAFEDELETE( pWindow );
		pWindow = NULL;
	};

	return bOverallResult;
}

// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::WindowCreate()
{
	bool bOverallResult = false;

	INITCOMMONCONTROLSEX icc;
	HWND hWnd = NULL;
	CWindow * pDisplayWindow = NULL;
	CWindow * pWindow = NULL;


	//
	// Initialize shell common controls.
	//

	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = ICC_BAR_CLASSES | ICC_COOL_CLASSES | ICC_TAB_CLASSES;

	BOOL bInitCommonControlsResult = InitCommonControlsEx(&icc);
	if ( bInitCommonControlsResult != TRUE )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::WindowCreate:  InitCommonControls failed.\n" )
		);
	};


	//
	// Load RichEd20.dll.
	//

	m_hRichEdit = LoadLibraryExW( L"RichEd20.Dll", NULL, 0 );
	if ( m_hRichEdit == NULL )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::WindowCreate:  Failed to load RichEd20.dll.\n" )
		);
	};


	//
	// Load accelerators.
	//

	m_hAccel = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));
	if ( m_hAccel == NULL )
	{
		DWORD LoadAcceleratorsError = ::GetLastError();

		DPF
		(
			1,
			_T( "CD3DWindowFramework::WindowCreate:  LoadAccelerators failed with error %u.\n" ),
			LoadAcceleratorsError
		);
	};


	//
	// Load the menu.
	//

	m_hMenu = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_MENU));
	if ( m_hMenu == NULL )
	{
		DWORD LoadMenuError = ::GetLastError();

		DPF
		(
			1,
			_T( "CD3DWindowFramework::WindowCreate:  LoadMenu failed with error %u.\n" ),
			LoadMenuError
		);

		goto Cleanup;
	};


	//
	// Initialize the top level window.
	//

	if
	(
		!InitializeWindow
		(
			0,
			_T("FMWK-MAIN"),
			_T("D3D Test Framework"),
			m_hMenu,
			LoadIcon(m_hInstance,MAKEINTRESOURCE(IDI_D3DICON)),
			m_dwWindowedStyle,
			0,
			0,
			0,
			0,
			NULL
		)
	)
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::WindowCreate:  InitializeWindow failed.\n" )
		);

		goto Cleanup;
	};


	//
	// Initialize the UI.
	//
	// N.B.:
	// Order is important here.  These need to be created in the
	// top-down order they should appear in the client window.
	//

	pDisplayWindow = new CWindow();
	if ( pDisplayWindow == NULL )
	{
		DPF
		(
			0,
			_T( "CD3DWindowFramework::WindowCreate:  Failed to allocate display window.\n" )
		);

		goto Cleanup;
	};


	if
	(
		!pDisplayWindow->InitializeWindow
		(
			WS_EX_CLIENTEDGE,
			_T("FMWK-DISPLAY"),
			_T(""),
			(HMENU) WND_DISPLAY,
			NULL,
			WS_BORDER | WS_CHILD | WS_VISIBLE,
			0,
			0,
			MODEOPT_BACKBUFFERWIDTH,
			MODEOPT_BACKBUFFERHEIGHT,
			m_hWindow
		)
	)
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::WindowCreate:  InitializeWindow on display window failed.\n" )
		);

		goto Cleanup;
	};


	if
	(
		!AddChild
		(
			_T("Display"),
			pDisplayWindow
		)
	)
	{
		DPF(1, _T("CD3DWindowFramework::WindowCreate:  AddChild on display window failed.\n"));

		goto Cleanup;
	};


	// Now that we've hooked up the display window to the hierarchy,
	// we can relinquish our reference to it.
	m_pDisplayWindow = pDisplayWindow;
	pDisplayWindow = NULL;


	//	
	// Initialize the rebar control.
	//

	if(!CreateRebarControl())
	{
		DPF(1, _T("CD3DWindowFramework::WindowCreate:  CreateRebarControl failed.\n"));

		goto Cleanup;
	};


	//
	// Initialize the tab control.
	//

	if(!CreateTabControl())
	{
		DPF(1, _T("CD3DWindowFramework::WindowCreate:  CreateTabControl failed.\n"));

		goto Cleanup;
	};


	//
	// Create the status bar.
	//

	hWnd = CreateStatusWindow
	(
		WS_CHILD | WS_VISIBLE,
		"",
		m_hWindow,
		0
	);

	if ( NULL == hWnd )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::WindowCreate:  CreateStatusBarWindow failed.\n" )
		);

		goto Cleanup;
	};


	pWindow = new CWindow(WND_STATUSBAR, hWnd);
	if ( NULL == pWindow )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::WindowCreate:  Failed to allocate status bar window.\n" )
		);

		DestroyWindow( hWnd );
		hWnd = NULL;

		goto Cleanup;
	};


	if
	(
		!AddChild(_T("Status Bar"), pWindow)
	)
	{
		DPF(1, _T("CD3DWindowFramework::WindowCreate:  AddChild on status bar window failed.\n"));

		goto Cleanup;
	};


	// Now that we've hooked up the status bar window to the
	// hierarchy, we can relinquish our reference to it.
	m_pStatusBar = pWindow;
	pWindow = NULL;


	//
	// Configure the status bar.
	//

	m_pStatusBar->SetFixed(true);

	SendMessage
	(
		m_pStatusBar->m_hWindow,
		SB_SIMPLE,
		0,
		0
	);


	//
	// Set the default display size.
	//

	SetDisplaySize
	(
		MODEOPT_BACKBUFFERWIDTH,
		MODEOPT_BACKBUFFERHEIGHT,
		true
	);


	bOverallResult = true;


Cleanup:
	if ( bOverallResult != true )
	{
		SAFEDELETE(pWindow);
		SAFEDELETE(pDisplayWindow);
	};

	return bOverallResult;
};


// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::ToggleSizeable(bool bSize)
{
	// For some bizarre reason, the SIZEGRIP won't toggle on/off when I
	// change the style.  So, we have to destroy and recreate the window
	// if necessary when we toggle.
	if(bSize)
	{
		m_dwStyle |= WS_SIZEBOX;
		if(!(m_pStatusBar->m_dwStyle & SBARS_SIZEGRIP))
		{
			SendMessage(m_pStatusBar->m_hWindow, WM_DESTROY, 0, 0);
			m_pStatusBar->m_dwStyle = WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP;
			m_pStatusBar->m_hWindow = CreateStatusWindow(m_pStatusBar->m_dwStyle, "", m_hWindow,0);
		}
	}
	else
	{
		m_dwStyle &= ~WS_SIZEBOX;
		if(m_pStatusBar->m_dwStyle & SBARS_SIZEGRIP)
		{
			SendMessage(m_pStatusBar->m_hWindow, WM_DESTROY, 0, 0);
			m_pStatusBar->m_dwStyle = WS_CHILD | WS_VISIBLE;
			m_pStatusBar->m_hWindow = CreateStatusWindow(m_pStatusBar->m_dwStyle, "", m_hWindow,0);
		}
	}

	SetStyle(m_dwStyle);

	return true;
}

// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::ToggleFullscreen(bool bWindowed)
{
	if(bWindowed)
		SetStyle(m_dwWindowedStyle | WS_VISIBLE);
	else
		SetStyle(m_dwFullscreenStyle | WS_VISIBLE);

	// Toggle the visibility of all windows	
	for(UINT i=0; i<m_nChildren; i++)
		m_pChildren[i]->SetVisible(bWindowed);

	return true;
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::ParseCmdline()
{
	int nLevel, nTemp=0, nMinutes = 0;
	float fTemp;
	TCHAR tcsBuffer[MAX_PATH];

	WriteStatusBar(0,_T("Parsing command line..."));

	if(!m_CmdLine.KeySet(_T("!DISABLEGHOSTING")))
	{
		// Prevent the OS from deciding that our test app is not
		// responding and displaying "ghost" windows over our UI.
		HMODULE hUser32 = GetModuleHandle( "user32.dll" );
		if ( hUser32 != NULL )
		{
			typedef void (WINAPI *PDISABLEPROCESSWINDOWSGHOSTING)();
			PDISABLEPROCESSWINDOWSGHOSTING pfnNoGhost = (PDISABLEPROCESSWINDOWSGHOSTING) GetProcAddress
			(
				hUser32,
				"DisableProcessWindowsGhosting"
			);

			if ( pfnNoGhost != NULL )
			{
				(*pfnNoGhost)();
				pfnNoGhost = NULL;
			}

			hUser32 = NULL;
		}
	}

	// Check if we just want to Lose the device and exit
	if (m_bLoseDeviceThenExit = m_CmdLine.ReadInteger(_T("LoseDevice"), &nLevel))
	{
		m_uAdapterToLoseDevice = (nLevel < 0) ? 0xffffffff : (UINT)nLevel;
		return;
	}

	// Determine whether to use the D3D9 hybrid enumeration (Force DGPU in a Hybrid system)
	if(m_CmdLine.ReadString(_T("HYBRID"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
		if(_tcsstr(tcsBuffer, _T("ForceDiscrete")))
			m_uHybridOverride = FORCEDISCRETE;
		else if(_tcsstr(tcsBuffer, _T("None")))
			m_uHybridOverride = NONE;
		else if(_tcsstr(tcsBuffer, _T("ForceIntegrated")))
			m_uHybridOverride = FORCEINTEGRATED;
	}
	else if (m_CmdLine.KeySet(_T("HYBRID")))
	{
		m_uHybridOverride = FORCEDISCRETE;
	}
	else
	{
		m_uHybridOverride = FORCEINTEGRATED;
	}

	// Determine whether to stop DWM.
	if (m_CmdLine.KeySet(_T("STOPDWM")))
	{
		CDWMController DWMController( m_pSimpleLog );
		HRESULT hStopDWMResult = DWMController.ToggleDWM( false );

		if
		(
			FAILED( hStopDWMResult ) &&
			hStopDWMResult != HRESULT_FROM_WIN32( ERROR_SERVICE_DOES_NOT_EXIST )
		)
		{
			DPF
			(
				1,
				_T( "CD3DWindowFramework::ParseCmdLine:  ToggleDWM failed with error 0x%x.\n" ),
				hStopDWMResult
			);
		}
		else if ( hStopDWMResult == S_OK )
		{
			DPF
			(
				3,
				_T( "Successfully stopped DWM.\n" )
			);

			m_bReenableDWM = true;
		};
	};


	// Get the log params
	if(m_CmdLine.KeySet(_T("stress")) || m_CmdLine.ReadInteger(_T("stress"), &nMinutes))
	{
		m_bStressMode = true;
		m_nStressDuration = nMinutes;
		m_nAdapter = 0;
		m_uExitCode &= ~EXITCLASS_LOG;
		SuggestExitCode(EXITCLASS_STRESS);

		if(!m_CmdLine.KeySet(_T("WHQL")))
			m_LogInfo.dwOptions &= ~(DXGLOG_OPTION_OUTPUT_PLAINTEXT | DXGLOG_OPTION_OUTPUT_XML | DXGLOG_OPTION_OUTPUT_DEBUGGER);
	}

	if(m_CmdLine.KeySet(_T("WHQL")))
	{
		m_bWHQL = true;
		m_LogInfo.uMaxGroupFailures = DXGLOG_MAX_TRACES;
		m_LogInfo.uMaxLogFileFailures = DXGLOG_MAX_TRACES;
	}


	if(m_CmdLine.KeySet(_T("COVERAGE")))
		m_bCoverage = true;

	if(m_CmdLine.KeySet(_T("FORCELOG")))
		m_bForceLog = true;

	if(m_CmdLine.KeySet(_T("NOLOG")))
		m_LogInfo.dwOptions &= ~(DXGLOG_OPTION_OUTPUT_PLAINTEXT | DXGLOG_OPTION_OUTPUT_XML | DXGLOG_OPTION_OUTPUT_DEBUGGER);

	if(m_CmdLine.KeySet(_T("TEXTLOG")))
	{
		m_LogInfo.dwOptions &= ~DXGLOG_OPTION_OUTPUT_XML;
		m_LogInfo.dwOptions |= DXGLOG_OPTION_OUTPUT_PLAINTEXT;
	}

	if(m_CmdLine.KeySet(_T("DEBUG")))
	{
		m_LogInfo.dwOptions |= DXGLOG_OPTION_OUTPUT_DEBUGGER;
		m_bExceptionHandling = false;
		m_bDebug = true;
	}

	if(m_CmdLine.KeySet(_T("LOGWINDOW")))
	{
		m_LogInfo.dwOptions |= DXGLOG_OPTION_OUTPUT_WINDOW;
	}

	if(m_CmdLine.KeySet(_T("DONOTCATCHEXCEPTIONS")))
	{
		m_bExceptionHandling = false;
	}

	if(m_CmdLine.KeySet(_T("LOGCLEAN")))
	{
		m_LogInfo.dwOptions |= DXGLOG_OPTION_OVERWRITE;
	}

	if(m_CmdLine.KeySet(_T("LOGALL")))
	{
		m_LogInfo.uMaxGroupPasses = DXGLOG_MAX_TRACES;
		m_LogInfo.uMaxLogFilePasses = DXGLOG_MAX_TRACES;

		m_LogInfo.uMaxGroupSkips = DXGLOG_MAX_TRACES;
		m_LogInfo.uMaxLogFileSkips = DXGLOG_MAX_TRACES;

		m_LogInfo.uMaxGroupFailures = DXGLOG_MAX_TRACES;
		m_LogInfo.uMaxLogFileFailures = DXGLOG_MAX_TRACES;

		m_LogInfo.uMaxGroupAborts = DXGLOG_MAX_TRACES;
		m_LogInfo.uMaxLogFileAborts = DXGLOG_MAX_TRACES;

		m_LogInfo.uMaxGroupWarnings = DXGLOG_MAX_TRACES;
		m_LogInfo.uMaxLogFileWarnings = DXGLOG_MAX_TRACES;
	}

	if(m_CmdLine.KeySet(_T("LOGSKIPS")))
	{
		m_LogInfo.uMaxGroupSkips = DXGLOG_MAX_TRACES;
		m_LogInfo.uMaxLogFileSkips = DXGLOG_MAX_TRACES;
	}

	if( m_CmdLine.KeySet( _T("NOCACHE") ) )
	{
		m_LogInfo.dwOptions |= DXGLOG_OPTION_NOCACHE;
	}

	if(m_CmdLine.ReadInteger(_T("MAXFAILURES"), &nTemp))
	{
		m_LogInfo.uMaxGroupFailures = nTemp;

		if ( m_LogInfo.uMaxLogFileFailures < nTemp )
		{
			m_LogInfo.uMaxLogFileFailures = nTemp;
		}
	}

	if(m_CmdLine.ReadInteger(_T("MAXLOGFILEFAILURES"), &nTemp))
	{
		m_LogInfo.uMaxLogFileFailures = nTemp;
	}

	if(m_CmdLine.KeySet(_T("LOGALLFAILURES")))
	{
		m_LogInfo.uMaxGroupFailures = DXGLOG_MAX_TRACES;
		m_LogInfo.uMaxLogFileFailures = DXGLOG_MAX_TRACES;
	}

	if(m_CmdLine.ReadString(_T("STYLESHEET"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
		COPYSTRING(m_LogInfo.sStyleSheet, tcsBuffer);
	}

	m_CmdLine.ReadInteger(_T("M"), &m_nAdapter);

	// Check for compare overrides
	if(m_CmdLine.ReadFloat(_T("PassRequired"), &fTemp))
	{
		m_ImageCmp.SetPassRequired(fTemp);
	}

	if(m_CmdLine.ReadFloat(_T("PixChanTol"), &fTemp))
	{
		m_ImageCmp.SetTolPerPixelChannel(fTemp);
	}

	if(m_CmdLine.ReadFloat(_T("PixTol"), &fTemp))
	{
		m_ImageCmp.SetTolPerPixel(fTemp);
	}

	if(m_CmdLine.ReadFloat(_T("ImgDeltaWght"), &fTemp))
	{
		m_ImageCmp.SetImgDeltaWght(fTemp);
	}

	// Get the initial state for test execution
	if(m_CmdLine.KeySet(_T("STEP")))
	{
		Step();
		UpdateToolbar();
	}

	// Put memory leak breakpoint
#ifdef _DEBUG
	int iLeak = 0;
	if (m_CmdLine.ReadInteger(_T("LEAK"), &iLeak))
	{
		_CrtSetBreakAlloc(iLeak);
	}

	if (m_CmdLine.KeySet(_T("MEMCHECK")))
	{
		int tmp = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		tmp |= _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF;
		_CrtSetDbgFlag(tmp);
	}
#endif

	// Set the logging levels
	m_CmdLine.ReadInteger(_T("LL"),1,(int*)&nLevel);
	SetLogLevel(nLevel);


	// Check for device overrides
	if(m_CmdLine.ReadString(_T("SRC"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
		// Convert to uppercase
		_tcsupr(tcsBuffer);

		// Get the device type
		if(_tcsstr(tcsBuffer, _T("HAL")))
			m_dwCmdDevTypes 	= DEVICETYPE_HAL;
		else if(_tcsstr(tcsBuffer, _T("NULLREF")))
			m_dwCmdDevTypes 	= DEVICETYPE_NULLREF;
		else if(_tcsstr(tcsBuffer, _T("REF")))
			m_dwCmdDevTypes 	= DEVICETYPE_REF;
		else if(_tcsstr(tcsBuffer, _T("SW")))
			m_dwCmdDevTypes 	= DEVICETYPE_SW;

		if(_tcsstr(tcsBuffer, _T("SVP")))
			m_dwCmdBehaviors 	= DEVICECREATE_SWVP;
		else if(_tcsstr(tcsBuffer, _T("HVP")))
			m_dwCmdBehaviors 	= DEVICECREATE_HWVP;

		if(_tcsstr(tcsBuffer, _T("MX")))
			m_dwCmdBehaviors	= DEVICECREATE_MIXED | ((m_dwCmdBehaviors & DEVICECREATE_HWVP) ? DEVICECREATE_HWVP : DEVICECREATE_SWVP);
		// MANAGED DEVICES PULLED FROM DX9
		// else if(_tcsstr(tcsBuffer, _T("MAN")))
		//	m_dwCmdBehaviors	|= DEVICECREATE_MANAGED;
		else if(_tcsstr(tcsBuffer, _T("PURE")))
			m_dwCmdBehaviors 	= DEVICECREATE_HWVP | DEVICECREATE_PURE;
	}

	// Cmdline exclusions
	if(m_CmdLine.KeySet(_T("!HAL")))
		m_dwExcludeDevTypes |= DEVICETYPE_HAL;
	if(m_CmdLine.KeySet(_T("!NULLREF")))
		m_dwExcludeDevTypes |= DEVICETYPE_NULLREF;
	if(m_CmdLine.KeySet(_T("!REF")))
		m_dwExcludeDevTypes |= DEVICETYPE_REF;
	if(m_CmdLine.KeySet(_T("!SW")))
		m_dwExcludeDevTypes |= DEVICETYPE_SW;
	if(m_CmdLine.KeySet(_T("!PURE")))
		m_dwExcludeBehaviors |= DEVICECREATE_PURE;
	if(m_CmdLine.KeySet(_T("!MIXED")))
		m_dwExcludeBehaviors |= DEVICECREATE_MIXED;
	if(m_CmdLine.KeySet(_T("!SVP")))
		m_dwExcludeBehaviors |= DEVICECREATE_SWVP;
	if(m_CmdLine.KeySet(_T("!HVP")))
		m_dwExcludeBehaviors |= DEVICECREATE_HWVP;
	// MANAGED DEVICES PULLED FROM DX9
	//if(m_CmdLine.KeySet(_T("!MAN")))
	//	m_dwExcludeBehaviors |= DEVICECREATE_MANAGED;

	// Check for test run %
	int iPerc = 0;
	m_CmdLine.ReadInteger( _T("TestPerc"), &iPerc, true);
	if ( 0 != iPerc && 0 < iPerc && 100 > iPerc )
	{
		m_fRunPerc = iPerc / 100.0f;
	}
	else
	{
		m_fRunPerc = 0.0f;
	}

	// Prevent the OS from deciding that our test app is not
	// responding and displaying "ghost" windows over our UI.
	if(!m_CmdLine.KeySet(_T("!DISABLEGHOSTING")))
	{
		HMODULE hUser32 = GetModuleHandle( "user32.dll" );
		if ( hUser32 != NULL )
		{
			typedef void (WINAPI *PDISABLEPROCESSWINDOWSGHOSTING)();
			PDISABLEPROCESSWINDOWSGHOSTING pfnNoGhost = (PDISABLEPROCESSWINDOWSGHOSTING) GetProcAddress
			(
				hUser32,
				"DisableProcessWindowsGhosting"
			);

			if ( pfnNoGhost != NULL )
			{
				(*pfnNoGhost)();
				pfnNoGhost = NULL;
			}

			hUser32 = NULL;
		}
	}
}

void CD3DWindowFramework::LoadRegistrySettings()
{
	LRESULT lr = 0;
	HKEY hRegKey = 0;
	DWORD dwSize = 256;
	TCHAR tcsBuffer[256];
	ULONG uValType = REG_SZ;
	DWORD dwVal;

	// Attempt to open the test key
	lr = RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\DXGTest"),NULL,KEY_QUERY_VALUE,&hRegKey);
	if(lr == ERROR_SUCCESS)
	{
		// Query for each value
		dwSize = 256;
		lr = RegQueryValueEx(hRegKey,_T("DBLogDirectory"),NULL,&uValType,(BYTE *)tcsBuffer,(LPDWORD)&dwSize);
		if(lr == ERROR_SUCCESS)
		{
			COPYSTRING(m_LogInfo.sDBDirectory, tcsBuffer);
		}

		dwSize = 256;
		lr = RegQueryValueEx(hRegKey,_T("LocalLogDirectory"),NULL,&uValType,(BYTE *)tcsBuffer,(LPDWORD)&dwSize);
		if(lr == ERROR_SUCCESS)
		{
			COPYSTRING(m_LogInfo.sLocalDirectory, tcsBuffer);
		}

		//
		// N.B. [OlanH]:
		// Look for "BreakOnCorruption" regkey.  This is an
		// alternative to the command-line switch for enabling
		// WDDM aperture corruption detection.  This will
		// cause the framework to trigger a DebugBreak when
		// WDDM aperture corruption is detected, and is used
		// during test automation to tie up a machine and thus
		// prevent other tests from running on it.  Because
		// aperture corruption is typically permanent on a
		// given piece of hardware, the detection and break
		// behavior prevents additional tests from running
		// and failing in bizarre ways.
		// 
		// Aperture corruption detection occurs as a post-
		// process (not real time).  Therefore it is not
		// intended or designed to improve a tester's
		// ability to debug the cause of the corruption in
		// such random-looking failures.
		//
		// If either the command-line switch or the registry
		// value is set, we will enable corruption detection.
		//
		// However, both the registry value and the command-
		// line switch are overridden by the
		// "DisableCorruption" command-line switch.
		//

		uValType = REG_DWORD;
		dwSize = sizeof(dwVal);
		lr = RegQueryValueEx(hRegKey, _T("BreakOnCorruption"), NULL, &uValType,(BYTE *)&dwVal, &dwSize);
		if(lr == ERROR_SUCCESS)
		{
			// Only turn *on* BreakOnCorruptionDetection, don't turn it off since it might have been turned on by
			// another means.
			if (dwVal)
				((CD3DTest*)m_pCurrentTest->pTest)->m_Options.TestOptions.bBreakOnCorruptionDetection = (dwVal ? true : false);
		}
	}
}
// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::CheckD3DVersion()
{
	WriteStatusBar(0, _T("Checking the D3D version..."));

	// Alloc the D3D object
	m_pD3D = new CD3D();
	if(m_pD3D == NULL)
	{
		DebugPrint(1, _T("CD3DWindowFramework::CheckD3DVersion:  Failed to allocate m_pD3D.\n"));
		return false;
	};


	// Initialize the D3D object
	if(!m_pD3D->Initialize())
	{
		return false;
	};

	return true;
}

// --------------------------------------------------------------------------------------

HRESULT CD3DWindowFramework::LoadDevice
(
	__in DEVICEMODE * pMode,
	__out CDevice ** pSrcDev
)
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;

	CD3DTest * pTest = NULL;

	UINT nModeID = 0;
	UINT nAttached = 0;

	DISPLAY_DEVICE DispDev;
	ZeroMemory( &DispDev, sizeof( DispDev ) );
	DispDev.cb = sizeof( DispDev );

	DEVMODE OptimalMode;
	ZeroMemory( &OptimalMode, sizeof( OptimalMode ) );
	OptimalMode.dmSize = sizeof( DEVMODE );

	DEVMODE DevMode;
	ZeroMemory( &DevMode, sizeof( DevMode ) );
	DevMode.dmSize = sizeof( DevMode );


	WriteStatusBar
	(
		0,
		_T("Loading DX%.1f device"),
		pMode->fVersion
	);


	//
	// Validate parameters.
	//

	if ( pSrcDev == NULL )
	{
		DPF
		(
			1,
			_T("CD3DWindowFramework::LoadDevice:  pSrcDev is NULL.")
		);

		hr = E_INVALIDARG;
		goto Exit;
	};


	//
	// Setup the D3D object for the new version.
	//

	hr = m_pD3D->SetupD3D(pMode->fVersion);
	if ( FAILED( hr ) )
	{
		DPF
		(
			2,
			_T("CD3DWindowFramework::LoadDevice:  SetupD3D failed with error 0x%x.\n"),
			hr
		);

		goto Exit;
	};


	//
	// Decode mode flags.
	//

	// If the test asked to run in the current desktop mode...
	if
	(
		pMode->pDispMode->dwWidth == MODEOPT_CURRENT &&
		pMode->pDispMode->dwHeight == MODEOPT_CURRENT
	)
	{
		// ...then fill in the mode from the modes that we saved at startup.
		pMode->pDispMode->dwWidth = m_pOrigModes[ pMode->nAdapter ].dmPelsWidth;
		pMode->pDispMode->dwHeight = m_pOrigModes[ pMode->nAdapter ].dmPelsHeight;
		pMode->pDispMode->dwRefreshRate = m_pOrigModes[ pMode->nAdapter ].dmDisplayFrequency;
		pMode->pDispMode->dwBPP = m_pOrigModes[ pMode->nAdapter ].dmBitsPerPel;
	}
	// ...otherwise, if we're running under WHQL...
	else if
	(
		pMode->pDispMode->dwWidth == MODEOPT_WHQL &&
		pMode->pDispMode->dwHeight == MODEOPT_WHQL
	)
	{
		// We're looking for the highest supported mode from our list of acceptable
		// WHQL desktop display modes
		for
		(
			int i = 0;
			EnumDisplayDevices
			(
				NULL,
				i,
				&DispDev,
				0
			);
			++i
		)
		{
			// Skip devices not attached to the desktop.
			if ( ( DispDev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0 )
			{
				continue;
			};

			// Skip devices other than the one we want to test.
			if ( pMode->nAdapter != nAttached++ )
			{
				continue;
			};


			// Scan the mode list, keeping track of the mode that
			// we prefer in OptimalMode.
			nModeID = 0;
			while
			(
				EnumDisplaySettings
				(
					DispDev.DeviceName,
					nModeID++,
					&DevMode
				)
			)
			{
				if
				(
					(
						DEVMODE_EQUALS(1024,768, DevMode) ||
						DEVMODE_EQUALS(800,600, DevMode) ||
						DEVMODE_EQUALS(640,480, DevMode) ||
						DEVMODE_EQUALS(768,1024, DevMode) ||
						DEVMODE_EQUALS(600,800, DevMode) ||
						DEVMODE_EQUALS(480,640, DevMode)
					) &&
					(
						DevMode.dmPelsWidth > OptimalMode.dmPelsWidth &&
						DevMode.dmPelsHeight > OptimalMode.dmPelsHeight
					)
				)
				{
					memcpy
					(
						&OptimalMode,
						&DevMode,
						sizeof( OptimalMode )
					);
				};
			};
		};


		// If none of the blessed modes match...
		if
		(
			OptimalMode.dmPelsWidth == 0 ||
			OptimalMode.dmPelsHeight == 0

		)
		{
			// ...then fill in the optimal mode from the modes that we saved at startup.
			OptimalMode.dmPelsWidth = m_pOrigModes[ pMode->nAdapter ].dmPelsWidth;
			OptimalMode.dmPelsHeight = m_pOrigModes[ pMode->nAdapter ].dmPelsHeight;
			OptimalMode.dmDisplayFrequency = m_pOrigModes[ pMode->nAdapter ].dmDisplayFrequency;
			OptimalMode.dmBitsPerPel = m_pOrigModes[ pMode->nAdapter ].dmBitsPerPel;
		};

		pMode->pDispMode->dwWidth = OptimalMode.dmPelsWidth;
		pMode->pDispMode->dwHeight = OptimalMode.dmPelsHeight;
		pMode->pDispMode->dwRefreshRate = OptimalMode.dmDisplayFrequency;
		pMode->pDispMode->dwBPP = OptimalMode.dmBitsPerPel;
	};


	//
	// Sanity-check the refresh rate.
	//
	// N.B. [SEdmison]:
	// Now that we're setting the refresh rate in the cases
	// above, I don't think that we should hit this code.
	// I put it in just for troubleshooting purposes.  Still,
	// I don't want to risk having the DebugBreak pop during
	// a lab run, so I've commented it out.
	//

	// If it looks too low...
	if ( pMode->pDispMode->dwRefreshRate < 20 )
	{
		// ...then break, ...
		//DebugBreak();

		// ...and use the desktop refresh rate.
		pMode->pDispMode->dwRefreshRate = m_pOrigModes[ pMode->nAdapter ].dmDisplayFrequency;
	};


	//
	// Get a pointer to the current test.
	//

	pTest = (CD3DTest *) GetCurrentTest();
	if ( pTest == NULL )
	{
		DPF
		(
			1,
			_T("CD3DWindowFramework::LoadDevice:  No test loaded.\n")
		);

		hr = E_FAIL;
		goto Exit;
	};


	//
	// Create test resources.
	//

	hr = LoadNextTest( pMode );

	if ( FAILED( hr ) )
	{
		DPF
		(
			2,
			_T( "CD3DWindowFrame::LoadDevice:  LoadNextTest failed with error 0x%x."),
			hr
		);
	};


	//
	// Get a pointer to the created device.
	//

	*pSrcDev = pTest->GetDevice( 0 );

	if ( *pSrcDev == NULL )
	{
		DPF
		(
			2,
			_T("CD3DWindowFramework::LoadDevice:  GetDevice failed.\n")
		);
	};


	//
	// Set the ImageCompare mode.  We're disabling Alpha compare on DX6 due to a REF bug.
	//

	m_ImageCmp.SetCompareAlpha( pMode->fVersion > 6.f );


	//
	// If we made it this far, then we're good to go.
	//

	hr = S_OK;


Exit:
	return hr;
};


// --------------------------------------------------------------------------------------

int CD3DWindowFramework::Execute(int nShow)
{	
	HMODULE hKernel32 = GetModuleHandle("KERNEL32.DLL");
	ASSERT(hKernel32 != NULL);
	DEBUGFUNC Debugger = (DEBUGFUNC) GetProcAddress(hKernel32,"IsDebuggerPresent");
	HRESULT hr = S_OK;

	int iLoseDeviceExitCode = 0;

	// Get the cmdline
	m_CmdLine.Refresh();

	// if running on Vista, inform the OS that the application is High-DPI aware
	if( m_CmdLine.KeySet( _T("HIGHDPI" ) ) )
	{
		HMODULE hUser32 = GetModuleHandle( "user32.dll" );
		if ( hUser32 != NULL )
		{
			typedef BOOL (WINAPI *PSETPROCESSDPIAWARE)(VOID);
			PSETPROCESSDPIAWARE pSetProcessDPIAware = (PSETPROCESSDPIAWARE)GetProcAddress( hUser32, "SetProcessDPIAware" );

			if ( pSetProcessDPIAware != NULL )
			{
				(*pSetProcessDPIAware)();
				pSetProcessDPIAware = NULL;
			}

			hUser32 = NULL;
		}
	}

	// If the user has asked to run on a custom desktop...
	TCHAR tcsDesktop[ MAX_PATH ] = _T( "" );
	if(m_CmdLine.ReadString(_T("DESKTOP"), tcsDesktop, ARRAYSIZE(tcsDesktop)))
	{
		// ...then let's try to switch to that desktop...
		HRESULT hSwitchToDesktopResult = SwitchToDesktop( tcsDesktop );

		if ( FAILED( hSwitchToDesktopResult ) )
		{
			DPF( 1, _T( "CD3DWindowFramework::Execute:  SwitchToDesktop failed.\n" ) );
		};
	}
	else if ( m_CmdLine.KeySet(_T("DESKTOP")))
	{
		// ...then away we go.
		HRESULT hSwitchToDesktopResult = SwitchToDesktop( NULL );

		if ( FAILED( hSwitchToDesktopResult ) )
		{
			DPF( 1, _T( "CD3DWindowFramework::Execute:  SwitchToDesktop failed.\n" ) );
		};
	};


	// Create the windows
	if(!PreWindowCreate())
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::Execute:  PreWindowCreate failed.\n" )
		);

		goto Exit;
	};

	if(!WindowCreate())
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::Execute:  WindowCreate failed.\n" )
		);

		goto Exit;
	};

	if(!PostWindowCreate())
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::Execute:  PostWindowCreate failed.\n" )
		);

		goto Exit;
	};

	// Show help if requested from cmdline
	if(m_CmdLine.KeySet(_T("?")))
	{
		ShowHelp();
		goto Exit;
	}

	// Check if the user has requested the configuration UI
	if(m_CmdLine.KeySet(_T("configure"), true))
	{
		if(!DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_CONFIGURE), m_hWindow, (DLGPROC)ConfigureProc, (LPARAM)&m_CmdLine))
		{
			DPF(1, _T("Test configuration aborted.  Exiting.\n"));
			goto Exit;
		}
	}

	// Show the windows
	if(m_hWindow != 0)
	{
		ShowWindow(m_hWindow,nShow);
		UpdateWindow(m_hWindow);
	}

	// Parse the commandline
	ParseCmdline();

	// Load registry settings
	LoadRegistrySettings();

	// If we want to Lose the Device and exit, do it!
	if ( m_bLoseDeviceThenExit )
	{
		if (m_uAdapterToLoseDevice == 0xffffffff)
		{
			DebugPrint(0, _T("CD3DWindowFramework::Execute:  Invalid adapter number passed to -LoseDevice.\n"));

			iLoseDeviceExitCode = EXITCODE_ABORT;
			goto Exit;
		}

		bool bLoseDeviceResult = LoseDeviceDefault(m_uAdapterToLoseDevice);

		if ( bLoseDeviceResult == true )
		{
			iLoseDeviceExitCode = EXITCODE_SUCCESS;
		}
		else
		{
			iLoseDeviceExitCode = EXITCODE_ABORT;
		};

		goto Exit;
	};


	// Open the log file.
	if(!OpenLogFile())
	{
		goto Exit;
	};


	// Determine whether to spawn a thread to listen for debug spew.
	BOOL bCaptureDebugSpew = FALSE;
	BOOL bCaptureCurrentProcessOnly = TRUE;
	TCHAR tcsBuffer[32] = _T( "" );
	if
	(
		m_CmdLine.ReadString(_T("DEBUGCAPTURE"), tcsBuffer, ARRAYSIZE(tcsBuffer)) &&
		Debugger != NULL &&
		(*Debugger)() == FALSE
	)
	{
		bCaptureDebugSpew = TRUE;
		if ( _tcsicmp(tcsBuffer, _T("ALL")) == 0 )
		{
			bCaptureCurrentProcessOnly = FALSE;
		};
	}
	else if
	(
		m_CmdLine.KeySet(_T("DEBUGCAPTURE")) &&
		Debugger != NULL &&
		(*Debugger)() == FALSE
	)
	{
		bCaptureDebugSpew = TRUE;
		bCaptureCurrentProcessOnly = TRUE;
	};

	// Prevent infinite loops of logging to the debugger,
	// then capturing the debug spew and logging it.
	if ( ( m_LogInfo.dwOptions & DXGLOG_OPTION_OUTPUT_DEBUGGER ) != 0 )
	{
		bCaptureDebugSpew = FALSE;
	};

	if (bCaptureDebugSpew == TRUE)
	{
		m_pTraceListenerThread = new CTraceListenerThread();
		if ( m_pTraceListenerThread == NULL )
		{
			DPF( 1, _T( "CD3DWindowFramework::Execute:  Failed to allocate trace listener thread object.\n" ) );
		}
		else
		{
			HRESULT hInitializeTraceListenerThreadResult = m_pTraceListenerThread->Initialize
			(
				m_pSimpleLog,
				bCaptureCurrentProcessOnly
			);

			if ( FAILED( hInitializeTraceListenerThreadResult ) )
			{
				DPF( 1, _T( "CD3DWindowFramework::Execute:  Failed to initialize trace listener thread.\n" ) );
			};

			HRESULT hStartTraceListenerThreadResult = m_pTraceListenerThread->Start();
			if ( FAILED( hStartTraceListenerThreadResult ) )
			{
				DPF( 1, _T( "CD3DWindowFramework::Execute:  Failed to start trace listener thread.\n" ) );
			};
		};
	};


	// Begin tracing with log
	m_DXGLog.BeginTestInitialization();

	// Check D3D version info
	if(!CheckD3DVersion())
	{
		m_DXGLog.EndTestInitialization(WTT_TESTCASE_RESULT_BLOCKED);
		goto Exit;
	}

	// Initialize the application
	if(FAILED(hr = Initialize()))
	{
		m_DXGLog.EndTestInitialization(hr == E_UNSUPPORTED ? WTT_TESTCASE_RESULT_SKIPPED : WTT_TESTCASE_RESULT_BLOCKED);

		m_DXGLog.LogJustSystemInfo();

		goto Exit;
	}

	// End the initialization tag
	m_DXGLog.EndTestInitialization(WTT_TESTCASE_RESULT_PASS);

	// Execute all tests
	ExecuteTests();

Exit:
	// Re-enable DWM if necessary.
	if ( m_bReenableDWM == true )
	{
		DPF
		(
			1,
			_T( "Re-enabling DWM.\n" )
		);

		CDWMController DWMController( m_pSimpleLog );
		HRESULT hRestartDWMResult = DWMController.ToggleDWM( true );

		if ( FAILED( hRestartDWMResult ) )
		{
			DPF
			(
				1,
				_T( "CD3DWindowFramework::Execute:  ToggleDWM failed with error 0x%x.\n" ),
				hRestartDWMResult
			);
		};
	};

	// Shut down the trace listener thread.
	if ( m_pTraceListenerThread != NULL )
	{
		m_pTraceListenerThread->Stop();

		m_pTraceListenerThread->Wait( INFINITE );

		m_pTraceListenerThread->Cleanup();

		delete m_pTraceListenerThread;
		m_pTraceListenerThread = NULL;
	};

	// Log the test finish time.
	SYSTEMTIME LocalTime;
	GetLocalTime( &LocalTime );
	WriteToLog
	(
		1,
		_T( "Finished at %2.2d:%2.2d:%2.2d %2.2d/%2.2d/%4.4d." ),
		LocalTime.wHour,
		LocalTime.wMinute,
		LocalTime.wSecond,
		LocalTime.wDay,
		LocalTime.wMonth,
		LocalTime.wYear
	);

	// Release the log adapter.
	if ( m_pSimpleLog != NULL )
	{
		m_pSimpleLog->Release();
		m_pSimpleLog = NULL;
	};

	// Wait for other processes sharing this log to exit.
	HRESULT hWaitForOtherProcessesResult = m_DXGLog.WaitForBroadcastingThreadsToFinish
	(
		10 * 1000,
		60 * 1000
	);

	if ( FAILED( hWaitForOtherProcessesResult ) )
	{
		WriteToLog
		(
			1,
			_T( "WaitForBroadcastingProcessesToFinish failed with error 0x%x." )
			_T( "This may mean that the log is incomplete."),
			hWaitForOtherProcessesResult
		);
	};


	// Shutdown the app.
	Shutdown();


	// Revert to the original desktop.
	if ( m_hCustomDesktop != NULL )
	{
		ASSERT( m_hOriginalThreadDesktop != NULL );
		ASSERT( m_hOriginalInputDesktop != NULL );

		RevertToOriginalDesktop();
	};


	// Close the log.
	m_DXGLog.Close();


	// Make sure the exit code doesn't get changed by the framework.
	if ( m_bLoseDeviceThenExit )
	{
		m_uExitCode = iLoseDeviceExitCode;
	};

	// Return the suggested exit code
	return m_uExitCode;
}


// --------------------------------------------------------------------------------------
// ApertureCorruptionDebugBreak - Spew and trigger a DebugBreak().   
//
// This function is called from ExecuteTest() below.  It is only triggered when WDDM
// aperture corruption is detected, and the "BreakOnCorruption" reg key is set.
//
// We put this in a separate function as an aid to future developers trying to quickly 
// triage why the test triggered a debug break.  Often debug spew is lost, but a stack
// trace is almost always available.
// --------------------------------------------------------------------------------------
void CD3DWindowFramework::ApertureCorruptionDebugBreak()
{
	TCHAR tszExeName[MAX_PATH+1];
	DPF(1, "*******************************************************************************\n");
	DPF(1, "%s detected WDDM aperture corruption!\n", GetExeName(false, tszExeName) );
	DPF(1, "Triggering DebugBreak() to halt further test execution.\n");
	DPF(1, "*******************************************************************************\n");

	DebugBreak();

}

// --------------------------------------------------------------------------------------
bool CD3DWindowFramework::ExecuteTests()
{
	HRESULT hr = S_OK;
	DEVICEMODE *pMode = m_pDeviceMode;
	CDevice *pDevice = NULL;
	bool bDuplicate = false;
	bool bTestRun = false;

	if(m_pDeviceMode == NULL)
	{
		DPF(1, _T("CD3DWindowFramework::ExecuteTests:  No device modes available.\n"));
		return false;
	}

	// Execute all Device modes in the device list
	do
	{
		// Mark the new Device group
		m_DXGLog.BeginDeviceGroup();

		// Reset the code coverage info
		if(m_bCoverage)
			CoverageReset(NULL);

		// Set the very first test in the list
		SetCurrentTest(GetActiveTest(0));

		// Attempt to load the device.
		pDevice = NULL;
		hr = LoadDevice(pMode, &pDevice);

		// Update the log with D3D Info
		LogDeviceInfo(pDevice);

		// If the device didn't load, don't execute the tests
		if(SUCCEEDED(hr))
		{
			WriteStatusBar(0,"Executing tests...");
			bTestRun = false;

			// Switch us into the new mode.  It's possible that hardware doesn't support this mode,
			// but we'll silently fail here and let CDevice validate it later.  This is a better
			// failure path that makes logs and behavior consistent
			//
			// 01/22/2010 - Added a warning in response to this failing. 
			hr = ChangeDesktopMode(pMode);
			if (FAILED(hr))
			{
				WriteToLog(1, "WARNING! CD3DWindowFramework::ExecuteTests - ChangeDesktopMode failed.  We'll continue testing, but we're not in the display mode that the test expects!");
				// 01/22/2010 - Ideally we should abort, but such a framework change was deemed too risky.
			}

			// Execute all test modes generated for this device
			do
			{
				if ( ( (CD3DTest*) m_pCurrentTest->pTest )->ModeSupported(pMode) )
				{
					// Reset the reverse flag once we successfully initialize a group
					m_bReverseGroup = false;

					// Set the flag to indicate that a test has actually been run
					bTestRun = true;

					// Initialize the test group.
					if(BeginTestExecution())
					{
						// Have the test group execute its test cases.
						((CD3DTest*)m_pCurrentTest->pTest)->ExecuteTests();
					}

					// LDDM Aperture Corruption?
					if(!(((CD3DTest*)m_pCurrentTest->pTest)->m_Options.TestOptions.bDisableCorruptionDetection) && LDDMApertureCorruption())
					{
						if(!m_DXGLog.InTestCase())
						{
							((CD3DTest*)m_pCurrentTest->pTest)->BeginTestCase
							(
								_T("WDDM Aperture Corruption Detected."),
								((CD3DTest *) m_pCurrentTest->pTest)->m_uTestNumber
							);
						}

						m_DXGLog.SetTestCaseResult(WTT_TESTCASE_RESULT_FAIL);
						m_DXGLog.EndTestCase();

						// Should we trigger a DebugBreak()?
						if ( ((CD3DTest*)m_pCurrentTest->pTest)->m_Options.TestOptions.bBreakOnCorruptionDetection)
						{
							// Spew and DebugBreak.
							ApertureCorruptionDebugBreak();
						}
					}

					// Clean up the test group.
					EndTestExecution();
				}


				// Process any windows messages
				if(!IsTestFinished() && !g_pFramework->ProcessMessages())
				{
					if(!m_DXGLog.InTestCase())
					{
						((CD3DTest*)m_pCurrentTest->pTest)->BeginTestCase
						(
							_T("User aborted test."),
							0xFFFFFFFF
						);
					}

					m_DXGLog.Trace(1, _T("***  Test aborted by user.\n"));

					m_DXGLog.SetTestCaseResult(WTT_TESTCASE_RESULT_BLOCKED);
					m_DXGLog.EndTestCase();


					m_dwExecuteState = EXECUTE_TERMINATE;
					SuggestExitCode(EXITCODE_ABORT);
				}
			}
			while(!IsTestFinished() && SUCCEEDED( hr = NextTest(pMode)) && !HasStressTimedOut());

			if( !bTestRun && hr == E_UNSUPPORTED )
			{
				DebugPrint(1, _T("No test groups execute against the current device mode - skipping.\n"));
				m_DXGLog.SkipTest();
			}
		}
		else if(hr == E_UNSUPPORTED)
		{
			if(!m_DXGLog.InTestCase())
			{
				m_DXGLog.BeginTestCase
				(
					_T("ExecuteTests"),
					0
				);
			}

			m_DXGLog.SkipTest();
		}
		else
		{
			if(!m_DXGLog.InTestCase())
			{
				m_DXGLog.BeginTestCase
				(
					_T("ExecuteTests"),
					0
				);
			}

			m_DXGLog.AbortTest();

			if(hr != E_ABORT)
				m_dwExecuteState = EXECUTE_TERMINATE;
		}

		// Mark the end of the Device group
		m_DXGLog.EndDeviceGroup();

		// Save off this code coverage info
		if(m_bCoverage)
		{
			TCHAR sConfig[256] = {0};
			if (m_LogInfo.TestType == TESTTYPE_CONF)
			{
				TCHAR sChipset[128] = {0};
				StringCchPrintf(sChipset, ARRAYSIZE(sChipset), _T("Unknown/Invalid"));

				StringCchPrintf(sConfig, ARRAYSIZE(sConfig), _T("CONF-%s-"), sChipset);
			}
			else if (m_LogInfo.TestType == TESTTYPE_PSGP)
			{
				StringCchPrintf(sConfig, ARRAYSIZE(sConfig), _T("PSGP-%s-"), m_DeviceInfo.bPSGP ? m_DXGLog.GetSysInfo()->ProcInf.sPipeline : _T("MS"));
			}
			else
			{
				StringCchPrintf(sConfig, ARRAYSIZE(sConfig), _T(""));
			}


			TCHAR tcsDesc[256] = {0};
			if (m_DXGLog.GetSysInfo()->OSInf.fVersion >= 6)
			{
				StringCchPrintf
				(
					tcsDesc,
					ARRAYSIZE(tcsDesc),
					_T("Desktop-Graphics-Runtime-%s-%s%s"),
					m_LogInfo.sTestName,
					sConfig,
					m_DeviceInfo.sDevice
				);
			}
			else
			{
				StringCchPrintf
				(
					tcsDesc,
					ARRAYSIZE(tcsDesc),
					_T("Multimedia-DirectX-Direct3D IM-%s-%s%s"),
					m_LogInfo.sTestName,
					sConfig,
					m_DeviceInfo.sDevice
				);
			}


			//
			// N.B. [SEdmison]:
			// Replace commas with dashes, since the code coverage
			// tools don't like trace names with commas or colons.
			// (The colons were getting introduced in the _stprintf calls above,
			// and should now be gone; unfortunately, some of the commas are
			// built into strings like our device name, and thus need to be
			// forcibly removed.)
			//

			for
			(
				LPTSTR pCurrentComma = _tcsstr( tcsDesc, _T( "," ) );
				pCurrentComma != NULL;
				pCurrentComma = _tcsstr( pCurrentComma, _T( "," ) )
			)
			{
				pCurrentComma[ 0 ] = _T( '-' );
			};


			//
			// Prepend the magic trace name prefix to the trace name.
			//
			// N.B. [SEdmison]:
			// For more information on what this is and what it's doing
			// here, consult AaronEd.  I just work here.
			//

			TCHAR tcsCodeCoverageTraceName[512] = {0};
			StringCchPrintf
			(
				tcsCodeCoverageTraceName,
				ARRAYSIZE(tcsCodeCoverageTraceName),
				_T( "%s%s" ),
				_T( "$M$NonWTT$\\LH\\Desktop Technologies\\Graphics Infrastructure\\Runtime$1342:" ),
				tcsDesc
			);

			CoverageSave(tcsCodeCoverageTraceName, NULL);
			DebugPrint(0, _T( "Saving coverage to trace %s." ), tcsCodeCoverageTraceName);
		}
	}
	while(!IsTestFinished() && (pMode = GetNextDevice()) && !HasStressTimedOut());

	return true;
}

// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::HasStressTimedOut()
{
	time_t tTime = 0;

	if(!m_bStressMode || m_nStressDuration == 0)
		return false;

	tTime = time(NULL);

	return ((UINT)((tTime - m_tStartTime) / 60)) >= m_nStressDuration;
}

// --------------------------------------------------------------------------------------

DEVICEMODE* CD3DWindowFramework::GetNextDevice()
{
	m_pCurrentDevMode = (NULL == m_pCurrentDevMode) ? m_pDeviceMode : m_pCurrentDevMode->pNext;

	// If we're in stress mode, we'll loop
	if(m_bStressMode && (NULL == m_pCurrentDevMode))
		m_pCurrentDevMode = m_pDeviceMode;

	return m_pCurrentDevMode;
}

//--------------------------------------------------------------------------------------

void CD3DWindowFramework::UpdateToolbar()
{
	CWindow *pWin = GetWindow(WND_TOOLBAR);
	LRESULT lrState;

	if(NULL == pWin)
		return;

	if(EXECUTE_RUN == m_dwExecuteState)
	{
		lrState = SendMessage(pWin->m_hWindow, TB_GETSTATE, (WPARAM)IDM_EXECUTION_PAUSE, 0);
		lrState &= ~TBSTATE_PRESSED;
		SendMessage(pWin->m_hWindow, TB_SETSTATE, IDM_EXECUTION_PAUSE, lrState);

		lrState = SendMessage(pWin->m_hWindow, TB_GETSTATE, (WPARAM)IDM_EXECUTION_RUN, 0);
		lrState |= TBSTATE_PRESSED;
		SendMessage(pWin->m_hWindow, TB_SETSTATE, IDM_EXECUTION_RUN, lrState);
	}
	else if(EXECUTE_PAUSE == m_dwExecuteState || EXECUTE_STEP == m_dwExecuteState || EXECUTE_REPLAY == m_dwExecuteState)
	{
		lrState = SendMessage(pWin->m_hWindow, TB_GETSTATE, (WPARAM)IDM_EXECUTION_RUN, 0);
		lrState &= ~TBSTATE_PRESSED;
		SendMessage(pWin->m_hWindow, TB_SETSTATE, IDM_EXECUTION_RUN, lrState);

		lrState = SendMessage(pWin->m_hWindow, TB_GETSTATE, (WPARAM)IDM_EXECUTION_PAUSE, 0);
		lrState |= TBSTATE_PRESSED;
		SendMessage(pWin->m_hWindow, TB_SETSTATE, IDM_EXECUTION_PAUSE, lrState);
	}
}

//--------------------------------------------------------------------------------------

void CD3DWindowFramework::OnSize(WPARAM wParam, LPARAM lParam)
{
	static bWasMaximized = false;
	HRESULT hr = S_OK;

	// Check if we're getting minimized
	if(SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam)
	{
		m_bActive = false;
	}
	else
	{
		m_bActive = true;

		// Toggle the maximized display window
		if(SIZE_MAXIMIZED == wParam)
			m_bMaximizeDisplay = true;
		else if(bWasMaximized)
			m_bMaximizeDisplay = false;

		// Do the default resizing
		CWindow::OnSize(wParam, lParam);

		// Update all windows based on new size
		ResizeWindows(LOWORD(lParam), HIWORD(lParam));

		// Update the status bar
		if(NULL != m_pStatusBar)
			SendMessage(m_pStatusBar->m_hWindow, WM_SIZE, 0, 0);

		// Resize the device if we've been maximized/restored
		if(SIZE_MAXIMIZED == wParam || bWasMaximized)
		{
			hr = ( (CD3DTest *) GetCurrentTest() )->Resize
			(
				m_pDisplayWindow->m_nWidth,
				m_pDisplayWindow->m_nHeight
			);

			if ( FAILED( hr ) )
			{
				m_dwExecuteState = EXECUTE_TERMINATE;
				SendMessage(m_hWindow, WM_CLOSE, 0, 0);
			};

			bWasMaximized = !bWasMaximized;
		};
	};
};


//--------------------------------------------------------------------------------------

void CD3DWindowFramework::ShowHelp()
{
	if(NULL == m_pHelpWindow)
	{
		// Create the help window - this window doesn't draw anything.  We use it for RTF/log management
		m_pHelpWindow = new CLogWindow();
		if(NULL == m_pHelpWindow || 
			!m_pHelpWindow->InitializeWindow(0,_T("FMWK-HELP"),NULL,(HMENU)WND_HELP,NULL,WS_CHILD,0,0,0,0,m_hWindow) ||
			!AddChild(_T("Help"), m_pHelpWindow))
		{
			DPF(1, _T("CD3DWindowFramework::CreateTabControl:  Failed to create Help window.\n"));
			SAFEDELETE(m_pHelpWindow);
		}
		else
		{
			// Generate the help log
			CommandLineHelp();
		}
	}
	if(NULL != m_pHelpWindow)
		DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_HELP), m_hWindow, (DLGPROC)HelpProc, (LPARAM)m_pHelpWindow->GetLog());
}

//--------------------------------------------------------------------------------------

LRESULT CD3DWindowFramework::OnCommand(WPARAM wParam, LPARAM lParam)
{
	CD3DTest *pTest = (CD3DTest*)GetCurrentTest();

	// Let the test handle the command
	if(NULL != pTest && pTest->OnCommand(wParam, lParam))
		return TRUE;

	switch(LOWORD(wParam))
	{
	case IDM_CLOSE:
		SuggestExitCode(EXITCODE_ABORT);
		PostQuitMessage(m_uExitCode);
		break;
	case IDM_IMAGECOMPARE:
		{	
			if(NULL != pTest && pTest->m_pCurrentMode->bWindowed && !m_bMaximizeDisplay)
			{
				m_bShowCompareWindow = !m_bShowCompareWindow;
				pTest->ToggleCompareWindow();
			}
		}
		break;
	case IDM_CMPIMGMODE:
		{
			m_ImageCmp.NextCmpImgMode();
			SendMessage(m_hWindow, WM_COMMAND, (WPARAM)IDM_PROCESSFRAME, 0L);
		}
		break;
	case IDM_SIZETODEVICE:
		{	
			if(NULL != pTest && pTest->m_pCurrentMode->bWindowed && !m_bMaximizeDisplay)
			{
				m_bMaximizeDisplay = false;
				SetDisplaySize(pTest->m_Options.D3DOptions.nBackBufferWidth * 
					(pTest->m_uDeviceList + (m_bShowCompareWindow ? 1 : 0)),
					pTest->m_Options.D3DOptions.nBackBufferHeight + pTest->m_nTagHeight, true, true);
			}
		}
		break;
	case IDM_EXECUTION_RUN:
		Run();
		UpdateToolbar();
		break;
	case  IDM_EXECUTION_PAUSE:
		Pause();
		UpdateToolbar();
		break;
	case  IDM_EXECUTION_STEP:
		Step();
		UpdateToolbar();
		break;
	case IDM_EXECUTION_PREVIOUS:
		PreviousGroup();
		UpdateToolbar();
		break;
	case IDM_EXECUTION_NEXT:
		NextGroup();
		UpdateToolbar();
		break;
	case IDM_EXECUTION_REPLAY:
		if(NULL != pTest && !(pTest->m_Options.TestOptions.dwCaps & TESTCAPS_SERIALEXECUTION))
		{
			Replay();
			UpdateToolbar();
		}
		break;
	case IDM_SAVEDEVICE0:
	case IDM_SAVEDEVICE1:
	case IDM_SAVEIMGCMP:
		if(NULL != pTest)
		{
			pTest->DeviceCapture(LOWORD(wParam) == IDM_SAVEIMGCMP ? CD3D_IMAGECMP : LOWORD(wParam) - IDM_SAVEDEVICE0 + 1);
		}
		break;
	case IDM_SAVEALL:
		if(NULL != pTest)
		{
			DWORD dwFlags = 0;
			for ( UINT i = 0; i < pTest->m_uDeviceList; i++)
			{
				dwFlags += ( 1 << i );
			}

			pTest->DeviceCapture( CD3D_IMAGECMP || dwFlags );
		}
		break;
	case WND_TESTNUM:
		if(EN_SETFOCUS == HIWORD(wParam))
		{
			SendMessage(m_hWindow, WM_COMMAND, IDM_EXECUTION_PAUSE, 0);
		}
		break;
	case IDHELP:
		ShowHelp();
		break;
	case ID_OPTIONS_DEVICE:
	case ID_OPTIONS_IMAGECOMPARE:
		{
			if(NULL != pTest && pTest->m_pCurrentMode->bWindowed)
			{
				switch(wParam)
				{
				case ID_OPTIONS_IMAGECOMPARE:
					DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_COMPAREOPTIONS), m_hWindow, (DLGPROC)ImageCompareProc, (LPARAM)&m_ImageCmp);
					break;
				case ID_OPTIONS_DEVICE:
					DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_DEVICEOPTIONS), m_hWindow, (DLGPROC)DevOptionsProc, (LPARAM)pTest);
					break;
				default:
					break;
				}
			}
		}
		break;
	default:
		return CWindow::OnCommand(wParam, lParam);
	}
	return TRUE;
}

//--------------------------------------------------------------------------------------

void CD3DWindowFramework::OnNotify(WPARAM wParam, LPARAM lParam)
{
	MSGFILTER *pFilter = (MSGFILTER*)lParam;
	NMHDR *nm = &(pFilter->nmhdr);
	CWindow *pWindow = NULL;    

	switch (LOWORD(wParam))
	{
		case WND_TESTNUM:
			if(EN_MSGFILTER == nm->code)
			{
				if(pFilter->msg == WM_KEYDOWN)
				{
					if((int)pFilter->wParam == VK_RETURN)
					{
						TCHAR tcsBuffer[16] = _T( "" );
						UINT uTestNum = 0;

						SendMessage(nm->hwndFrom, WM_GETTEXT, 16, (UINT_PTR)tcsBuffer);

						uTestNum = _ttoi(tcsBuffer);

						if(GetCurrentTest()->SetCurrentTest(uTestNum))
							SendMessage(m_hWindow, WM_COMMAND, IDM_EXECUTION_REPLAY, 0);
					}
				}
			}
		break;

		default:
			// Let the window handle it's WM_NOTIFY messages
			pWindow = GetWindowPointer(nm->hwndFrom);
			if(pWindow)
				pWindow->OnNotify(wParam, lParam);
			else
				CWindow::OnNotify(wParam, lParam);
		break;
	}
}

//--------------------------------------------------------------------------------------

LRESULT CD3DWindowFramework::OnMouse(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CTest *pTest = GetCurrentTest();

	// Let the test handle the mouse input
	if(pTest && pTest->OnMouse(uMsg, wParam, lParam))
		return TRUE;

	return CWindow::OnMouse(uMsg, wParam, lParam);
}

//--------------------------------------------------------------------------------------

LRESULT CD3DWindowFramework::OnKeydown(WPARAM wParam, LPARAM lParam)
{
	CTest *pTest = GetCurrentTest();

	// Let the test handle the keyboard input
	if(pTest && pTest->OnKeydown(wParam, lParam))
		return TRUE;

	return CWindow::OnKeydown(wParam, lParam);
}

//--------------------------------------------------------------------------------------

LRESULT CD3DWindowFramework::OnKeyup(WPARAM wParam, LPARAM lParam)
{
	CTest *pTest = GetCurrentTest();

	// Let the test handle the keyboard input
	if(pTest && pTest->OnKeyup(wParam, lParam))
		return TRUE;

	return CWindow::OnKeyup(wParam, lParam);
}

//--------------------------------------------------------------------------------------

LRESULT CD3DWindowFramework::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	CTest *pTest = GetCurrentTest();

	// Let the test handle the keyboard input
	if(pTest && pTest->OnSetCursor(wParam, lParam))
		return TRUE;

	return CWindow::OnSetCursor(wParam, lParam);
}

//--------------------------------------------------------------------------------------

LRESULT CD3DWindowFramework::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;
	static RECT rOriginal, rNew;
	LCID InvariantLocale = MAKELCID( MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ), SORT_DEFAULT );

	if(m_bWHQL)
	{
		// Disable input if running in WHQL mode
		if(WM_COMMAND == uMsg || WM_KEYDOWN == uMsg)
			return 0;
	}

	switch(uMsg)
	{
	case WM_ACTIVATEAPP:
		{
			//
			// Did the test ask for app activation logging?
			//

			// If not...
			CD3DTest * pTest = (CD3DTest *) GetCurrentTest();
			if
			(
				NULL == pTest ||
				pTest->m_Options.TestOptions.bLogAppActivation == false
			)
			{
				// ...then skip it.
				break;
			};


			// Determine whether we're receiving or losing the focus.
			if (wParam == 0)
			{
				//
				// We lost focus.  Now we need to determine whether
				// we gave it up or had it stolen.
				//

				//
				// First, grab the thread ID from the notification.
				//

				DWORD NotifyingThreadID = (DWORD) lParam;

				// If there's no thread in the notification...
				if ( NotifyingThreadID == 0 )
				{
					// ...then there's not much to do here.
					break;
				};


				//
				// Next, figure out the new foreground window.
				//

				HWND hForegroundWindow = GetForegroundWindow();

				// If there's no foreground window to tattle on...
				if ( hForegroundWindow == NULL )
				{
					// ...then there's not much to do here.
					break;
				};


				//
				// Get the window caption.
				//

				TCHAR pForegroundWindowText[128] = _T( "" );
				int nForegroundWindowText = GetWindowText
				(
					hForegroundWindow,
					pForegroundWindowText,
					ARRAYSIZE( pForegroundWindowText )
				);


				//
				// Get the window class name.
				//

				TCHAR pForegroundWindowClass[128] = _T( "" );
				int nForegroundWindowClass = GetClassName
				(
					hForegroundWindow,
					pForegroundWindowClass,
					ARRAYSIZE( pForegroundWindowClass )
				);


				//
				// Let's not tattle on our own test.
				//

				if
				(
					nForegroundWindowClass > 4 &&
					CompareString
					(
						InvariantLocale,
						NORM_IGNORECASE,
						pForegroundWindowClass,
						5,
						_T( "FMWK-" ),
						5
					) == CSTR_EQUAL
				)
				{
					break;
				};


				//
				// Now figure out what process and thread created
				// that window.
				//

				DWORD ForegroundProcessID = 0;
				DWORD ForegroundThreadID = GetWindowThreadProcessId
				(
					hForegroundWindow,
					&ForegroundProcessID
				);


				//
				// Prior to Longhorn, we don't have the tools we
				// need to dig any further, so log the info that we
				// have, and call it a day.
				//

				if ( m_pGetProcessIdOfThread == NULL )
				{
					DPF
					(
						1,
						_T( "CD3DWindowFramework::WindowProc:  Lost focus to thread %u.\n" ),
						NotifyingThreadID
					);
				}
				else
				{
					//
					// Attempt to assert the debug privilege, which we need
					// in order to do OpenProcess or OpenThread on certain
					// system services and other protected processes.
					//

					bool bEnableDebugPrivilegeResult = EnablePrivilege( SE_DEBUG_NAME );
					if ( bEnableDebugPrivilegeResult != true )
					{
						DebugOut.Write
						(
							_T( "CD3DWindowFramework::WindowProc:  Failed to enable SeDebugPrivilege.\n" )
						);
					};


					//
					// Open the thread passed to us in the window message.
					//

					HANDLE hNotifyingThread = OpenThread
					(
						THREAD_QUERY_INFORMATION,
						FALSE,
						NotifyingThreadID
					);

					if
					(
						hNotifyingThread == NULL
					)
					{
						DPF
						(
							1,
							_T( "CD3DWindowFramework::WindowProc:  Lost focus to thread %u, but can't discover process ID because OpenThread failed.\n" ),
							NotifyingThreadID
						);
					}
					else
					{
						//
						// Find the process who owns the offending thread.
						//

						DWORD NotifyingProcessID = (*m_pGetProcessIdOfThread)
						(
							hNotifyingThread
						);

						if ( NotifyingProcessID == 0 )
						{
							DPF
							(
								1,
								_T( "CD3DWindowFramework::WindowProc:  Lost focus to thread %u, but can't discover process ID because GetProcessIdOfThread failed.\n" ),
								NotifyingThreadID
							);
						}
						else if ( NotifyingProcessID == GetCurrentProcessId() )
						{
							// We have seen the enemy, and it is we!
							// No worries; we must have set focus to another window.
						}
						else
						{
							if ( m_pGetProcessImageFileName == NULL )
							{
								DPF
								(
									1,
									_T( "CD3DWindowFramework::WindowProc:  Lost focus to thread %u, process %u.\n" ),
									NotifyingThreadID,
									NotifyingProcessID
								);
							}
							else
							{
								HANDLE hNotifyingProcess = OpenProcess
								(
									PROCESS_QUERY_INFORMATION,
									FALSE,
									NotifyingProcessID
								);

								if ( hNotifyingProcess == NULL )
								{
									DPF
									(
										1,
										_T( "CD3DWindowFramework::WindowProc:  Lost focus to thread %u, process %u, but can't discover process name because OpenProcess failed.\n" ),
										NotifyingThreadID,
										NotifyingProcessID
									);
								}
								else
								{
									TCHAR pNotifyingProcessName[ 256 ] = _T( "" );
									DWORD nNotifyingProcessName = (*m_pGetProcessImageFileName)
									(
										hNotifyingProcess,
										pNotifyingProcessName,
										ARRAYSIZE( pNotifyingProcessName )
									);

									if ( nNotifyingProcessName == 0 )
									{
										DPF
										(
											1,
											_T( "CD3DWindowFramework::WindowProc:  Lost focus to thread %u, process %u, but can't discover process name because GetProcessImageFileName failed.\n" ),
											NotifyingThreadID,
											NotifyingProcessID
										);
									}
									else
									{
										DPF
										(
											1,
											_T( "CD3DWindowFramework::WindowProc:  Lost focus to thread %u, process %u (%s).\n" ),
											NotifyingThreadID,
											NotifyingProcessID,
											pNotifyingProcessName
										);
									};

									CloseHandle(hNotifyingProcess);
									hNotifyingProcess = NULL;
								};
							};
						};

						CloseHandle(hNotifyingThread);
						hNotifyingThread = NULL;
					};
				};


				//
				// Log information about the foreground window,
				// caption, thread, and process.
				//

				DPF
				(
					1,
					_T( "CD3DWindowFramework::WindowProc:  Foreground window is 0x%x of class %s, caption %s, created by thread %u of process %u.\n" ),
					hForegroundWindow,
					nForegroundWindowClass > 0 ? pForegroundWindowClass : _T( "[Unknown class]" ),
					nForegroundWindowText > 0 ? pForegroundWindowText : _T( "[No caption]" ),
					ForegroundThreadID,
					ForegroundProcessID
				);
			}
		} break;


	case WM_ENTERSIZEMOVE:
		{
			GetClientRect( m_pDisplayWindow->m_hWindow, &rOriginal );
		} break;


	case WM_EXITSIZEMOVE:
		{
			GetClientRect( m_pDisplayWindow->m_hWindow, &rNew );
			// Resize the devices to the new display size
			if( (rOriginal.right - rOriginal.left) != (rNew.right - rNew.left) ||
				(rOriginal.bottom - rOriginal.top) != (rNew.bottom - rNew.top))
			{
				hr = ( (CD3DTest *) GetCurrentTest() )->Resize
				(
					m_pDisplayWindow->m_nWidth,
					m_pDisplayWindow->m_nHeight
				);

				if ( FAILED( hr ) )
				{
					m_dwExecuteState = EXECUTE_TERMINATE;
					SendMessage(m_hWindow, WM_CLOSE, 0, 0);
				};
			};
		} break;


	case WM_CLOSE:
		{
			// Repost this message through our internal handler - do not pass to DefWindowProc
			return SendMessage(m_hWindow, WM_COMMAND, (WPARAM)IDM_CLOSE, 0L);
		} break;


	default:
		{
		} break;
	};


	return CWindow::WindowProc(uMsg,wParam,lParam);
}

//--------------------------------------------------------------------------------------

bool CD3DWindowFramework::ProcessMessages()
{
	MSG				msg;
	HWND			hActive;

	msg.message = WM_NULL;
	hActive		= GetActiveWindow();

	while (m_bActive ? PeekMessage(&msg,NULL,0,0,PM_REMOVE) : 
		GetMessage(&msg,NULL,0,0))
	{
		// Only translate accelerator for the main window
		if( (hActive == m_hWindow || !IsDialogMessage(hActive,&msg)) &&
			(hActive != m_hWindow || !TranslateAccelerator(m_hWindow, m_hAccel, &msg)))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If this is a quit message, re-post it.
		if (WM_QUIT == msg.message)
		{
			PostQuitMessage((int)msg.wParam);
			return false;
		}
	}

	// Ensure that all GDI commands complete
	GdiFlush();

	return true;
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::OnPaint(WPARAM wParam, LPARAM lParam)
{
	CD3DTest * pTest;

	// Do default processing
	CWindow::OnPaint(wParam, lParam);

	// Let the test paint the display window
	pTest=(CD3DTest*)GetCurrentTest();

	if(NULL != pTest)
		pTest->OnPaint();
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::CommandLineHelp(void)
{
	CTest *pTest = NULL;

	// Test specific help
	WriteCommandLineHelp(_T("$gTests available:\n"));

	UINT uMax = GetNumTests();
	for (UINT uCount = 0; uCount < uMax; uCount++)
	{
		pTest = GetTest(uCount);
		if (NULL == pTest)
			return;
		WriteCommandLineHelp(_T("$y-%s: %s\n"), pTest->m_szCommandKey, pTest->m_szTestName);
		((CD3DTest*)pTest)->CommandLineHelp();
	}

	if (NULL == pTest)
		return;


	// Test selection help
	WriteCommandLineHelp(_T("\n$gTest selection options:\n"));
	WriteCommandLineHelp(_T("$y\tTest execution control is available via the command line"));
	WriteCommandLineHelp(_T("$y\tby specifying individual test numbers, or test ranges.\n"));
	WriteCommandLineHelp(_T("$y\tFor example:\n"));
	WriteCommandLineHelp(_T("$w\t\t-%s:-5,10,15,20-30,100-\n"),pTest->m_szCommandKey);
	WriteCommandLineHelp(_T("$y\tThis command line will execute tests 1 thru 5, 10, 15, 20 thru 30,"));
	WriteCommandLineHelp(_T("$y\tand then 100 until completion. By ommitting a test range, or individual"));
	WriteCommandLineHelp(_T("$y\ttest numbers, the entire range will execute. For example:\n"));
	WriteCommandLineHelp(_T("$w\t\t-%s\n"),pTest->m_szCommandKey);
	WriteCommandLineHelp(_T("$y\tThis command line will execute the entire range of %s"),pTest->m_szTestName);


	// Test execution help
	WriteCommandLineHelp(_T("\n$gTest execution options:\n"));
	WriteCommandLineHelp(_T("$y\t-?\t\t\t$wThis help window"));
	WriteCommandLineHelp(_T("$y\t-Debug\t\t\t$wVerbose debug output, no exception handling"));
	WriteCommandLineHelp(_T("$y\t-Desktop\t\t$wRun on custom desktop"));
	WriteCommandLineHelp(_T("$y\t-Desktop:$rname$w\t\tRun on desktop $rname$w"));
	WriteCommandLineHelp(_T("$y\t-DisableCorruption\t$wDisable WDDM Aperture Corruption detection"));
	WriteCommandLineHelp(_T("$y\t-BreakOnCorruption\t$wDebugBreak on WDDM Aperture Corruption detection"));
	WriteCommandLineHelp(_T("$y\t-HighDPI\t\t$wMark process as high-DPI-aware"));
	WriteCommandLineHelp(_T("$y\t-Stress\t\t$wStress mode (no log, runs forever)"));
	WriteCommandLineHelp(_T("$y\t-Stress:<$r#$y>\t\t$wStress mode that terminates in # minutes"));
	WriteCommandLineHelp(_T("$w\t\t\t\tKeyboard input is disabled"));
	WriteCommandLineHelp(_T("$w\t\t\t\tEarly termination returns failure"));
	WriteCommandLineHelp(_T("$y\t-WHQL\t\t\t$wExecute as a WHQL conformance test"));


	// Format selection help
	WriteCommandLineHelp(_T("\n$gDirectX, format, and mode options:\n"));
	WriteCommandLineHelp(_T("$y\t-BB:$rW$wx$rH$wx$rF\t\t$wSpecify backbuffer format. Case sensitive."));
	WriteCommandLineHelp(_T("$w\t\t\t\tW - width (default=320)"));
	WriteCommandLineHelp(_T("$w\t\t\t\tH - height (default=280)"));
	WriteCommandLineHelp(_T("$w\t\t\t\tF - Format (default=DisplayMode, ALL and ANY also supported)"));
	WriteCommandLineHelp(_T("$w\t\t\t\tUse $r? $wto specify default, eg. -BB:160x140x?"));
	WriteCommandLineHelp(_T("$y\t-Cube:$r<fmt>\t\t$wRequest specific cube format($rA8R8G8B8$w, $rP8$w, etc.)"));
	WriteCommandLineHelp(_T("$y\t-!CycleAPI\t\t$wDo not perform API cycling"));
	WriteCommandLineHelp(_T("$y\t-DisplayMode:$rW$wx$rH$wx$rF\t$wSpecify display mode. Case sensitive."));
	WriteCommandLineHelp(_T("$w\t\t\t\tW - width (default=Current)"));
	WriteCommandLineHelp(_T("$w\t\t\t\tH - height (default=Current)"));
	WriteCommandLineHelp(_T("$w\t\t\t\tF - Format (default=Highest, ALL and ANY also supported)"));
	WriteCommandLineHelp(_T("$w\t\t\t\tUse $r? $wto specify default, eg. -DisplayMode:?x?xX8R8G8B8"));
	WriteCommandLineHelp(_T("$y\t-DX$r#$y\t\t\t$wUse Dx version if available(ex. -dx9, -dx8)"));
	WriteCommandLineHelp(_T("$y\t-!DX$r#$y\t\t\t$wExclude Dx version from run"));
	WriteCommandLineHelp(_T("$y\t-FBCompare\t\t$wEnable front buffer image compare"));
	WriteCommandLineHelp(_T("$y\t-FFEMU\t\t$wFixed function emulation mode."));
	WriteCommandLineHelp(_T("$y\t-ImgDeltaWght:$r<float>\t$wSets the weight for total image delta deduction"));
	WriteCommandLineHelp(_T("$y\t-M:$r#\t\t\t$wSelect adapter to run against"));
	WriteCommandLineHelp(_T("$w\t\t\t\t(default=$r1$w, primary=$r1$w, secondary=$r2$w, etc.)"));
	WriteCommandLineHelp(_T("$y\t-MS:$r#\t\t\t$wRequest specific multisample format($r2$w, $r4$w, etc.)"));
	WriteCommandLineHelp(_T("$y\t-PassRequired:$r<float>\t$wSets the conformance pass percentage"));
	WriteCommandLineHelp(_T("$y\t-PixChanTol:$r<float>\t$wSets the tolerance per pixel per channel"));
	WriteCommandLineHelp(_T("$y\t-PixTol:$r<float>\t\t$wSets the tolerance per pixel"));
	WriteCommandLineHelp(_T("$y\t-RefD3D:<$rstr$y>\t\t$wFull path to D3D runtime DLL used with ref device"));
	WriteCommandLineHelp(_T("$y\t-SrcD3D:<$rstr$y>\t\t$wFull path to D3D runtime DLL used with src device"));
	WriteCommandLineHelp(_T("$y\t-!Stretch$y\t\t$wDo not stretch from render target to window target"));
	WriteCommandLineHelp(_T("$y\t-SWDLL:<$rstr$y>\t\t$wSoftware rasterizer dll"));
	WriteCommandLineHelp(_T("$w\t\t\t\t(default=$rRGB$w)"));
	WriteCommandLineHelp(_T("$y\t-SWDLLEntry:<$rstr$y>\t$wSoftware rasterizer dll entry"));
	WriteCommandLineHelp(_T("$y\t-TestPerc:$r#$y\t\t$wPercentage of tests to run"));
	WriteCommandLineHelp(_T("$y\t-Tex:$r<fmt>\t\t$wRequest specific texture format($rA8R8G8B8$w, $rP8$w, etc.)"));
	WriteCommandLineHelp(_T("$y\t-Vol:$r<fmt>\t\t$wRequest specific volume format($rA8R8G8B8$w, $rP8$w, etc.)"));
	WriteCommandLineHelp(_T("$y\t-WinMode: <$rstr$y>\t$wRendering mode ($rWindow$w, $rFullscreen$w, $rAll$w)"));
	WriteCommandLineHelp(_T("$y\t-ZBuf:$r<fmt>\t\t$wRequest specific ZBuffer format($rD16$w, $rD32$w, etc.)"));


	// Device help
	WriteCommandLineHelp(_T("\n$gDevice selection options (any reasonable combination):\n"));
	WriteCommandLineHelp(_T("$y\t-SRC:<$rdevice$y>\t$wSpecify device for source (specify options from below)"));
	WriteCommandLineHelp(_T("$y\t-REF:<$rdevice$y>\t$wSpecify device for reference (specify options from below)"));
	WriteCommandLineHelp(_T("$y\t-!<$rdevice$y>\t$wDo not execute on specified device (specify options from below)"));
	WriteCommandLineHelp(_T("$r\tSW\t\t$wSoftware device"));
	WriteCommandLineHelp(_T("$r\tREF\t\t$wReference device"));
	WriteCommandLineHelp(_T("$r\tHAL\t\t$wHardware device"));
	WriteCommandLineHelp(_T("$r\tPURE\t\t$wPure device"));
	WriteCommandLineHelp(_T("$r\tMX\t\t$wMixed vertex processing"));
	// MANAGED DEVICES PULLED FROM DX9
	//WriteCommandLineHelp(_T("$r\tMAN\t\t$wManaged device"));
	WriteCommandLineHelp(_T("$r\tSVP\t\t$wSoftware vertex processing"));
	WriteCommandLineHelp(_T("$r\tHVP\t\t$wHardware vertex processing"));
	WriteCommandLineHelp(_T("\n$gDevice behaviors:\n"));
	WriteCommandLineHelp(_T("$y\t-MultiThreaded\t$wSet DEVICECREATE_MULTITHREADED in device creation"));
	WriteCommandLineHelp(_T("$y\t-!FPUPRESERVE\t$wDisable FPU_PRESERVE (default is enabled)"));
	WriteCommandLineHelp(_T("$y\t-!FPUCHECK\t$wDisable FPU state checks (default is enabled)"));

	// Logging help
	WriteCommandLineHelp(_T("\n$gLogging options:\n"));
	WriteCommandLineHelp(_T("$y\t-DBLOG\t\t\t$wLog to the D3D test database"));
	WriteCommandLineHelp(_T("$y\t-DEBUGCAPTURE\t$wCapture debug output to the log file"));
	WriteCommandLineHelp(_T("$y\t-NTLOG\t\t\t$wGenerate NT log (BVT) (eg. testname_nt.log)"));
	WriteCommandLineHelp(_T("$y\t-ALTLOG\t\t$wGenerate NT log (eg. testname.alt.log)"));
	WriteCommandLineHelp(_T("$y\t-FORCELOG\t\t$wForce logging (overwrite log if it exists)"));
	WriteCommandLineHelp(_T("$y\t-NOLOG\t\t\t$wDon't generate any log file"));
	WriteCommandLineHelp(_T("$y\t-LOGCLEAN\t\t$wOverwrite any existing log"));
	WriteCommandLineHelp(_T("$y\t-LOGALL\t\t$wLog all results(even if test passes)"));
	WriteCommandLineHelp(_T("$y\t-LOGSKIPS\t\t$wLog skips"));
	WriteCommandLineHelp(_T("$y\t-LOGALLFAILURES\t$wLog all failures(disable failure limit)"));
	WriteCommandLineHelp(_T("$y\t-MAXFAILURES:<$r#$y>\t$wSet maximum failures logged per group"));
	WriteCommandLineHelp(_T("$y\t-STYLESHEET:<$rfull path$y>\t$wSet log file stylesheet"));
	WriteCommandLineHelp(_T("$y\t-LL:<$r#$y>\t\t\t$wSet the logging level(1-4, higher = more spew)"));

	// API help
	WriteCommandLineHelp(_T("\n$gDrawPrimitive options:\n"));
	WriteCommandLineHelp(_T("$y\t-API:<$rstr$y>\t$wSpecify rendering style"));
	WriteCommandLineHelp(_T("\t$rBE \t\t$wBegin/Vertex/End"));
	WriteCommandLineHelp(_T("\t$rBIE \t\t$wBeginIndexed/Index/End"));
	WriteCommandLineHelp(_T("\t$rDP \t\t$wDrawPrimitive"));
	WriteCommandLineHelp(_T("\t$rDPS \t\t$wDrawPrimitiveStrided"));
	WriteCommandLineHelp(_T("\t$rDPVB \t\t$wDrawPrimitiveVB"));
	WriteCommandLineHelp(_T("\t$rDPVBO \t\t$wDrawPrimitiveVB (opt VB)"));
	WriteCommandLineHelp(_T("\t$rDPVBPV \t$wDrawPrimitiveVB ProcessVertices"));
	WriteCommandLineHelp(_T("\t$rDPVBOPV \t$wDrawPrimitiveVB ProcessVertices (opt VB)")); 
	WriteCommandLineHelp(_T("\t$rDPVBPVS \t$wDrawPrimitiveVB ProcessVerticesStrided"));
	WriteCommandLineHelp(_T("\t$rDIP \t\t$wDrawIndexedPrimitive"));
	WriteCommandLineHelp(_T("\t$rDIPS \t\t$wDrawIndexedPrimitveStrided"));
	WriteCommandLineHelp(_T("\t$rDIPVB \t\t$wDrawIndexedPrimitiveVB"));
	WriteCommandLineHelp(_T("\t$rDIPVBO \t$wDrawIndexedPrimitiveVB (opt VB)")); 
	WriteCommandLineHelp(_T("\t$rDIPVBPV \t$wDrawIndexedPrimitiveVB ProcessVertices"));
	WriteCommandLineHelp(_T("\t$rDIPVBOPV \t$wDrawIndexedPrimitiveVB ProcessVertices (opt VB)"));
	WriteCommandLineHelp(_T("\t$rDIPVBPVS \t$wDrawIndexedPrimitiveVB ProcessVerticesStrided"));

	// Test control help (static)
	WriteCommandLineHelp(_T("\n$gTest control options:\n"));
	WriteCommandLineHelp(_T("$y\t-INV\t\t$wExecute invalid parameter tests"));
	WriteCommandLineHelp(_T("$y\t-HALTONFAIL\t$wPause test execution when test fails"));

	// Test control help (dynamic)
	WriteCommandLineHelp(_T("\n$gHot keys:\n"));
	WriteCommandLineHelp(_T("$y\tS\t\t$wPauses, Single step test execution"));
	WriteCommandLineHelp(_T("$y\tG\t\t$wRestarts automated test execution"));
	WriteCommandLineHelp(_T("$y\tR\t\t$wReplay current test"));
	WriteCommandLineHelp(_T("$y\tPgDn\t\t$wSkip to next group or mode"));
	WriteCommandLineHelp(_T("$y\tPgUp\t\t$wSkip to previous group or mode"));
	WriteCommandLineHelp(_T("$y\tF1\t\t$wSave Device0 results to %%userprofile%%\\dxlogs"));
	WriteCommandLineHelp(_T("$y\tF2\t\t$wSave Device1 results to %%userprofile%%\\dxlogs"));
	WriteCommandLineHelp(_T("$y\tF3\t\t$wSave ImageCompare results to %%userprofile%%\\dxlogs"));
	WriteCommandLineHelp(_T("$y\tF4\t\t$wSave all devices and ImageCompare results to %%userprofile%%\\dxlogs"));
	WriteCommandLineHelp(_T("$y\tF7\t\t$wResize window to fit device(no stretching)"));
	WriteCommandLineHelp(_T("$y\tF8\t\t$wShow results of SRC v REF comparison"));
	WriteCommandLineHelp(_T("$y\tF9\t\t$wToggle rendering mode for comparison window"));

	// Notes
	WriteCommandLineHelp(_T("\n$gNotes:\n"));
	WriteCommandLineHelp(_T("\t$wCommand line options are NOT case sensitive (except where noted)\n"));
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::UpdateWindows()
{
	UINT nHeight=0;
	UINT nXBdr, nYBdr;

	// Adjust the positions for all child windows
	for(UINT i=0; i<m_nChildren; i++)
	{
		m_pChildren[i]->GetBorders(nXBdr, nYBdr);
		m_pChildren[i]->Move(0, nHeight);

		nHeight += m_pChildren[i]->m_nHeight + nYBdr;
	}
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::ResizeWindows(UINT nWidth, UINT nHeight)
{
	//
	// Initialize locals.
	//

	// Available client area height in pixels.
	UINT nHeightLeft = nHeight;

	// Available client area height fraction.
	float fLeft = 1.f;


	//
	// Calculate the height of the client area that's available
	// after we account for all fixed-size windows.
	//

	if (!m_bMaximizeDisplay)
	{
		// For each child window...
		for(UINT i=0; i<m_nChildren; i++)
		{
			// ...if it's fixed-size...
			if(m_pChildren[i]->IsFixed())
			{
				// ...and if its height is expressed as a
				// fraction of the remaining height, and if we
				// have room for it...
				if ( fLeft > m_pChildren[ i ]->m_fHeight )
				{
					// ...then deduct its height from the
					// fractional height remaining...
					fLeft -= m_pChildren[i]->m_fHeight;
				}
				else
				{
					// ...otherwise, the window is too small,
					// so don't bother re-arranging child
					// windows...
					return;
				}


				// ...or, if its height is expressed in pixels
				// and we have room for it...
				if ( nHeightLeft > m_pChildren[ i ]->m_nHeight )
				{
					// ...then deduct its height from the
					// pixel height remaining...
					nHeightLeft -= m_pChildren[i]->m_nHeight;
				}
				else
				{
					// ...otherwise, the window is too small,
					// so don't bother re-arranging child
					// windows.
					return;
				}
			}
		}
	}


	//
	// Now adjust the heights of variable-height child windows
	// based on the remaining fractional and pixel heights
	// calculated above.
	//

	for(UINT i=0; i<m_nChildren; i++)
	{
		if (m_bMaximizeDisplay)
		{
			if(m_pChildren[i] == m_pDisplayWindow)
			{
				m_pChildren[i]->Resize(nWidth, nHeight);
				m_pChildren[i]->SetVisible(true);
			}
			else
			{
				m_pChildren[i]->SetVisible(false);
			}
		}
		else
		{
			m_pChildren[i]->Resize
			(
				nWidth,
				m_pChildren[i]->IsFixed() ?
					m_pChildren[i]->m_nHeight :
					(UINT) floor
					(
						(
							(float) nHeightLeft * (m_pChildren[i]->m_fHeight / fLeft)
						) + .5f
					)
			);
			m_pChildren[i]->SetVisible(true);
		}
	}


	// Update all the window positions
	UpdateWindows();


	// Repaint the status window if it exists
	if ( m_pStatusBar != NULL )
	{
		InvalidateRect
		(
			m_pStatusBar->m_hWindow,
			NULL,
			FALSE
		);
	}
}


// --------------------------------------------------------------------------------------

void CD3DWindowFramework::SetDisplaySize(int nWidth, int nHeight, bool bWindowed, bool bForce)
{
	UINT nTotalHeight = 0, nTotalWidth = 0;
	RECT r;

	// Early out if we're not actually changing the size
	if(!bForce && (nWidth == m_dwCurrentDispWidth && nHeight == m_dwCurrentDispHeight && bWindowed == m_bCurrentDispWindowed))
		return;
	else
	{
		m_dwCurrentDispWidth = nWidth;
		m_dwCurrentDispHeight = nHeight;
		m_bCurrentDispWindowed = bWindowed;
	}

	// For fullscreen, we set the window size = to the mode requested
	if(!bWindowed)
	{
		Resize(nWidth, nHeight);
	}
	else
	{
		// Resize the display window to the target size
		r.left = r.top = 0;
		r.right = nWidth;
		r.bottom = nHeight;
		AdjustWindowRectEx(&r, m_pDisplayWindow->m_dwStyle, false, m_pDisplayWindow->m_dwStyleEx);

		// The new width will be the display window size
		nTotalWidth = r.right - r.left;

		// Resize the windows and recalculate the coverage percentages
		m_pDisplayWindow->Resize(r.right - r.left, r.bottom - r.top);
		m_pTabWindow->Resize(r.right - r.left, 175);
		UpdateChildStats();

		// Calc the new window height
		for(UINT i=0; i<m_nChildren; i++)
		{
			r.left = r.top = 0;
			r.right = m_pChildren[i]->m_nWidth;
			r.bottom = m_pChildren[i]->m_nHeight;

			AdjustWindowRectEx(&r, m_pChildren[i]->m_dwStyle, false, m_pChildren[i]->m_dwStyleEx);
			nTotalHeight += r.bottom - r.top;
		}

		// Get the total size for the given client size
		r.left = r.top = 0;
		r.right = nTotalWidth;
		r.bottom = nTotalHeight;
		AdjustWindowRectEx(&r, m_dwStyle, true, m_dwStyleEx);

		// Set the new size
		Resize(r.right - r.left, r.bottom - r.top);
	}
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::SetAdapter(UINT nAdapter, PADAPTER pAdapter, bool bForce)
{
	if(!bForce && (NULL == pAdapter || nAdapter == m_nCurrentAdapter))
		return;
	else
	{
		Move(pAdapter->rMonitorRect.left, pAdapter->rMonitorRect.top);
		m_nCurrentAdapter = nAdapter;
	}
}

// --------------------------------------------------------------------------------------

HRESULT CD3DWindowFramework::Initialize(void)
{
	HRESULT			hr = S_OK;
	DISPLAY_DEVICE	DispDev;
	HMODULE hKernel32 = GetModuleHandle("KERNEL32.DLL");
	ASSERT(hKernel32 != NULL);
	DEBUGFUNC Debugger = (DEBUGFUNC) GetProcAddress(hKernel32,"IsDebuggerPresent");
	int nAttached = 0;
	int nResult = 0;

	// Disable exception handling if a debugger is attached
	if (m_bExceptionHandling && (Debugger && (*Debugger)()))
		m_bExceptionHandling = false;

	// If -STRESS is specified (and DebugLevel isn't set in the registry), turn off debug output!
	if(m_bStressMode && !DebugOut.bLevelSetByRegistry)
	{
#if !defined(DBG) && !defined(_DEBUG)
		DebugOut.Off();
#else
		m_LogInfo.dwOptions |= DXGLOG_OPTION_OUTPUT_DEBUGGER | DXGLOG_OPTION_OUTPUT_XML;
		m_bExceptionHandling = false;
#endif
	}

	// Initialize the code coverage tools
	if(m_bCoverage)
	{
		do
		{
			m_hCovDLL = LoadLibraryExW( L"wincover.dll", NULL, 0 );

			// Give the user a chance to add the DLL to the system if load failed
			if(m_hCovDLL == NULL)
				nResult = MsgBox(MB_ICONSTOP|MB_RETRYCANCEL|MB_APPLMODAL, _T("WinCover.dll not detected.\n"));			
		}while(nResult == IDRETRY && NULL == m_hCovDLL);

		if(NULL == m_hCovDLL)
		{
			DebugPrint(1, _T("CD3DWindowFramework::Initialize:  WinCover.dll not detected.  Code Coverage setup failed.\n"));
			hr = E_FAIL;
			goto Exit;
		}

		CoverageInitialize = (COVERAGEINITIALIZE)GetProcAddress(m_hCovDLL, "_CoverageInitialize@4");
		CoverageReset = (COVERAGERESET)GetProcAddress(m_hCovDLL, _T("_CoverageResetA@4"));
		CoverageSave = (COVERAGESAVE)GetProcAddress(m_hCovDLL, _T("_CoverageSaveA@8"));
		CoverageUninitialize = (COVERAGEUNINITIALIZE)GetProcAddress(m_hCovDLL, _T("_CoverageUninitialize@0"));

		if(NULL == CoverageInitialize || NULL == CoverageReset ||
			NULL == CoverageSave || NULL == CoverageUninitialize)
		{
			DebugPrint(1, _T("CD3DWindowFramework::Initialize:  Code Coverage setup failed.\n"));
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		CoverageInitialize(NULL);
	}

	// Get the current modes for all monitors
	m_nAdapters = GetAdapterCount();
	if(m_nAdapters)
	{
		m_pOrigModes = new DEVMODE[m_nAdapters];
		m_pCurModes = new DISPLAYMODE[m_nAdapters];

		if(NULL == m_pOrigModes || NULL == m_pCurModes)
		{
			DebugPrint(1, _T("CD3DWindowFramework::Initialize:  Out of memory.\n"));
			SAFEDELETEA(m_pOrigModes);
			SAFEDELETEA(m_pCurModes);
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		memset(m_pCurModes, 0, sizeof(DISPLAYMODE) * m_nAdapters);
		memset(m_pOrigModes, 0, sizeof(DEVMODE) * m_nAdapters);

		DispDev.cb = sizeof(DISPLAY_DEVICE);
		for(int i=0; EnumDisplayDevices(NULL, i, &DispDev, 0) && (nAttached < m_nAdapters);i++)
		{
			if (DispDev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
			{
				// Get the current mode for each adapter
				m_pOrigModes[ nAttached ].dmSize = sizeof( m_pOrigModes[ 0 ] );
				m_pOrigModes[ nAttached ].dmSpecVersion = DM_SPECVERSION;

				BOOL bGetCurrentModeResult = EnumDisplaySettings
				(
					DispDev.DeviceName,
					ENUM_CURRENT_SETTINGS,
					&m_pOrigModes[ nAttached ]
				);

				if ( bGetCurrentModeResult != TRUE )
				{
					DPF
					(
						1,
						_T( "CD3DWindowFramework::Initialize:  EnumDisplaySettings failed.\n" )
					);

					SAFEDELETEA(m_pOrigModes);
					SAFEDELETEA(m_pCurModes);

					hr = E_FAIL;
					goto Exit;
				}

				// Set the current mode info
				m_pCurModes[nAttached].dwWidth = m_pOrigModes[nAttached].dmPelsWidth;
				m_pCurModes[nAttached].dwHeight = m_pOrigModes[nAttached].dmPelsHeight;
				m_pCurModes[nAttached].dwBPP = m_pOrigModes[nAttached].dmBitsPerPel;
				m_pCurModes[nAttached].dwRefreshRate = m_pOrigModes[nAttached].dmDisplayFrequency;

				switch ( m_pOrigModes[nAttached].dmBitsPerPel )
				{
				case 16:
					m_pCurModes[nAttached].Format.d3dfFormat = (D3DFORMAT)FMT_R5G6B5;
					break;

				case 32:
					m_pCurModes[nAttached].Format.d3dfFormat = (D3DFORMAT)FMT_X8R8G8B8;
					break;

				default:
					m_pCurModes[nAttached].Format.d3dfFormat = (D3DFORMAT)FMT_UNKNOWN;
					break;
				};


				// Make sure the display mode is at least 16bpp.  Initializing in a palettized mode
				// can cause problems in DX6/DX7 so we'll find a better mode and temporarily change here.
				if(m_pOrigModes[nAttached].dmBitsPerPel < 16)
				{
					UINT nModeID = 0;
					DEVMODE DevMode;
					ZeroMemory( &DevMode, sizeof( DevMode ) );
					DevMode.dmSize = sizeof( DevMode );

					// Get the current mode for each adapter
					while(EnumDisplaySettings(DispDev.DeviceName, nModeID++, &DevMode))
					{
						if(DevMode.dmBitsPerPel >= 16)
						{
							LONG ChangeDisplayModeResult = ChangeDisplaySettingsEx
							(
								DispDev.DeviceName,
								&DevMode,
								NULL,
								CDS_FULLSCREEN,
								NULL
							);

							if ( DISP_CHANGE_SUCCESSFUL != ChangeDisplayModeResult )
							{
								DPF
								(
									1,
									_T("CD3DWindowFramework::Initialize:  ChangeDisplaySettingsEx failed with error %d.\n"),
									ChangeDisplayModeResult
								);

								goto Exit;
							}

							break;
						}
					}
				}

				++nAttached;
			}
		}
	}


	// Add and initialize the global tests
	hr = InitializeTests();
	if ( FAILED( hr ) )
	{
		/*
		DPF
		(
			0,
			_T( "InitializeTests failed.\n" )
		);
		*/

		goto Exit;
	};


#ifdef _X86_ // The fpstate.dll utility only works on X86 platforms
	// Load FPU state checking library if necessary
	if(!m_CmdLine.KeySet(_T("!FPUCHECK")))
	{
		bool bRequired = m_CmdLine.KeySet(_T("WHQL"));
		g_hDCTDLL = LoadLibraryExW( L"fpstate.dll", NULL, 0 );
		if (NULL == g_hDCTDLL)
		{
			DPF(bRequired ? 1 : 2, _T("CD3DWindowFramework::Initialize:  Unable to load fpstate.dll.\n"));
			if(bRequired)
			{
				hr = E_FAIL;
				goto Exit;
			}
		}
		else
		{
			if( (NULL == (g_DCTGet = (GETFUNC) GetProcAddress(g_hDCTDLL,"Get"))) ||
				(NULL == (g_DCTCompare = (COMPAREFUNC) GetProcAddress(g_hDCTDLL,"Compare"))))
			{
				g_DCTGet = NULL;
				g_DCTCompare = NULL;
				FreeLibrary(g_hDCTDLL);
				g_hDCTDLL = NULL;

				DPF(bRequired ? 1 : 2, _T("CD3DWindowFramework::Initialize:  Unable to load functions from fpstate.dll!\n"));
				if(bRequired)
				{
					hr = E_FAIL;
					goto Exit;
				}
			}
		}
	}

	// Create the fonts
	HDC hDC = GetDC(m_pDisplayWindow->m_hWindow);
	if(hDC)
	{
		m_hSmallFont = MakeFont(hDC, 8, 0, 0, 0);
		m_hLargeFont = MakeFont(hDC, 10, FW_BOLD, 0, 0);
		ReleaseDC(m_pDisplayWindow->m_hWindow, hDC);
	}

	if(!m_hSmallFont || !m_hLargeFont)
	{
		DebugPrint(1, _T("CD3DWindowFramework::Initialize:  Failed to create fonts.\n"));
		hr = E_FAIL;
		goto Exit;
	}
#endif // _X86_

Exit:
	return hr;
}


// --------------------------------------------------------------------------------------

void CD3DWindowFramework::WriteStringToLog(UINT uLevel, LPCTSTR pcsMsg)
{
	if(m_bMuteLog)
		return;

	m_DXGLog.TraceWhole(uLevel, pcsMsg);
};


// --------------------------------------------------------------------------------------

void CD3DWindowFramework::WriteToLog(UINT uLogLevel, LPCTSTR ptcsFmt, ...)
{
	TCHAR *sTemp = NULL;
	va_list	argList;

	va_start(argList, ptcsFmt);
	sTemp = GetVarArgString(ptcsFmt, argList);
	va_end(argList);

	// write to the framework
	if (!m_bMuteLog)
	{
		m_DXGLog.TraceWhole(uLogLevel, sTemp);
	};

	SAFEDELETEA(sTemp);
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::WriteMessage(UINT nLevel, LPCTSTR pcsMsg)
{
	m_pMsgWindow->WriteToLogWhole(nLevel, pcsMsg);
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::WriteDetails(UINT nLevel, LPCTSTR pcsMsg)
{
	m_pDetailsWindow->WriteToLogWhole(nLevel, pcsMsg);
}

// --------------------------------------------------------------------------------------

HRESULT CD3DWindowFramework::InitializeTests()
{
	HRESULT hr = S_OK;


	//
	// Determine what OS we're running on.
	//

	OSVERSIONINFOEX OSVersionInfo;
	ZeroMemory( &OSVersionInfo, sizeof( OSVersionInfo ) );
	OSVersionInfo.dwOSVersionInfoSize = sizeof( OSVersionInfo );
	BOOL bGetOSVersionResult = GetVersionEx( (OSVERSIONINFO *) &OSVersionInfo );
	if ( bGetOSVersionResult != TRUE )
	{
		DPF
		(
			1,
			_T( "GetVersionEx failed.\n" )
		);

		hr = E_FAIL;
		goto Exit;
	};


	//
	// Walk the list of selected test groups, and for each one,
	// determine what device modes that it wants, and add them to
	// the master mode list.
	//

	// Determine how many test groups we have.
	UINT uMax = GetNumTests();

	// For each test group...
	for (UINT uCount = 0; uCount < uMax; uCount++)
	{
		// ...get a pointer to the test group object...
		CD3DTest * pTest = (CD3DTest *) GetTest(uCount);

		// ...and make sure that we found a test group...
		if ( pTest == NULL )
		{
			continue;
		};

		// ...and that the test group is selected.
		if ( !TestSelected( pTest ) )
		{
			continue;
		};


		// Initialize the test group.
		pTest->Initialize(m_pD3D, m_bExceptionHandling);


		// Check that the test group set its test type.
		if ( (DWORD) m_LogInfo.TestType == 0 )
		{
			DPF
			(
				1,
				_T("CD3DWindowFramework::InitializeTests:  Test group %u (\"%s\") never called SetTestType.  Exiting.\n"),
				uCount,
				pTest->m_szTestName
			);

			hr = E_INVALIDARG;
			goto Exit;
		};


		// Get the set of requested device types from the test.
		DWORD dwDevTypes =
			( m_dwCmdDevTypes != 0 )
			?
			m_dwCmdDevTypes
			:
		pTest->m_Options.D3DOptions.dwDevTypes;

		// Get the set of requested behavior flags from the test.
		DWORD dwBehaviors = pTest->m_Options.D3DOptions.dwBehaviors;

		DWORD dwPSGPRegChanges = pTest->m_Options.D3DOptions.dwPSGPRegChange;


		// Get the requested D3D version range from the test.
		if
		(
			m_DeviceInfo.fMaxDXVersion == 0.f ||
			pTest->m_Options.D3DOptions.fMaxDXVersion > m_DeviceInfo.fMaxDXVersion
		)
		{
			m_DeviceInfo.fMaxDXVersion = pTest->m_Options.D3DOptions.fMaxDXVersion;
		};

		if
		(
			m_DeviceInfo.fMinDXVersion == 0.f ||
			pTest->m_Options.D3DOptions.fMinDXVersion < m_DeviceInfo.fMinDXVersion
		)
		{
			m_DeviceInfo.fMinDXVersion = pTest->m_Options.D3DOptions.fMinDXVersion;
		};


		// Skip unsupported runtime versions.
		if
		(
			m_pD3D->GetMinVersion() > m_DeviceInfo.fMaxDXVersion
		)
		{
			DPF
			(
				1,
				_T( "CD3DWindowFramework::InitializeTests:  Test group %u (\"%s\") tests Direct3D %2.1f to %2.1f, but the minimum version for this OS/architecture is %2.1f.\n" ),
				uCount,
				pTest->m_szTestName,
				m_DeviceInfo.fMinDXVersion,
				m_DeviceInfo.fMaxDXVersion,
				m_pD3D->GetMinVersion()
			);

			continue;
		};

		if
		(
			m_pD3D->GetMaxVersion() < m_DeviceInfo.fMinDXVersion
		)
		{
			DPF
			(
				1,
				_T( "CD3DWindowFramework::InitializeTests:  Test group %u (\"%s\") tests Direct3D %2.1f to %2.1f, but the maximum version for this OS/architecture is %2.1f.\n" ),
				uCount,
				pTest->m_szTestName,
				m_DeviceInfo.fMinDXVersion,
				m_DeviceInfo.fMaxDXVersion,
				m_pD3D->GetMaxVersion()
			);

			continue;
		};


		// Get the requested version mask from the test.
		DWORD dwIncVersions = pTest->m_Options.D3DOptions.dwIncVersions;

		// Resolve ambiguous version masks.  (The code to make
		// x_OR_y different than x_AND_y is below.)
		if ( (dwIncVersions & DXVERSION_9_OR_91) != 0)
		{
			dwIncVersions |= DXVERSION_9;
			dwIncVersions |= DXVERSION_91;
		};

		if ( (dwIncVersions & DXVERSION_8_OR_81) != 0)
		{
			dwIncVersions |= DXVERSION_8;
			dwIncVersions |= DXVERSION_81;
		};


		// Merge command-line behavior flags and test behavior flags.
		if ( m_dwCmdBehaviors != 0 )
		{
			// If the cmdline behaviors don't set everything, use the test's defaults
			if(!(m_dwCmdBehaviors & 0xFF))	// VP modes
			{
				m_dwCmdBehaviors = m_dwCmdBehaviors | (dwBehaviors & 0xFF);
			};

			if(!(m_dwCmdBehaviors & 0xFF00)) // Device behaviors(Pure, managed, etc)
			{
				m_dwCmdBehaviors = m_dwCmdBehaviors | (dwBehaviors & 0xFF00);
			};

			dwBehaviors = m_dwCmdBehaviors;
		};


		// Mask out excluded device types and behaviors.
		dwDevTypes &= ~m_dwExcludeDevTypes;
		dwBehaviors &= ~m_dwExcludeBehaviors;


		// If we're in terminal services mode and the test asked
		// to cycle over formats...
		if
		(
			m_CmdLine.KeySet(_T("ts")) &&
			(FMT) pTest->m_Options.D3DOptions.DisplayMode.Format.d3dfFormat == FMT_ALL
		)
		{
			// ...then just use the current format instead, since
			// we can't change modes anyway.
			pTest->m_Options.D3DOptions.DisplayMode.Format = FMT_ANY;
		};


		// Add the tests's requested display mode to the display mode list.
		if (!AddDisplayMode(&pTest->m_Options.D3DOptions.DisplayMode))
		{
			DPF(1, _T("CD3DWindowFramework::InitializeTests:  AddDisplayMode failed.\n"));
			hr = E_FAIL;
			goto Exit;
		};


		//
		// Iterate over combinations of device parameters
		// in order to select device modes.
		//

		for (DISPLAYMODELIST *pMode = m_pDispModes; pMode != NULL; pMode = pMode->pNext)
		{
			for (int nVersion = 0; nVersion < g_nVersions; nVersion++)
			{
				// Look up the floating-point version number.
				float fVersion = g_pVersions[nVersion];


				// Skip 9.1 interfaces on XPDM.
				if ( (FLOAT_EQUAL(fVersion, 9.1f) && m_pD3D->GetDriverModel() == DRIVERMODEL_XPDM) )
				{
					continue;
				};


				// Skip versions that the test doesn't want.
				if ( !(dwIncVersions & GetDXVersionFlag(fVersion)) )
				{
					continue;
				};

				if ( fVersion < pTest->m_Options.D3DOptions.fMinDXVersion )
				{
					continue;
				};

				if ( fVersion > pTest->m_Options.D3DOptions.fMaxDXVersion )
				{
					continue;
				};


				// Skip crippled 64-bit DX versions.
#if defined(_AMD64_)
				// Pre-DX9 versions are crippled on AMD64 native.
				if
				(
					fVersion < 9.0f
				)
				{
					DPF
					(
						1,
						_T( "CD3DWindowFramework::InitializeTests:  Test group %u (\"%s\") is being skipped for Direct3D version %2.1f, which is not supported on AMD64.\n" ),
						uCount,
						pTest->m_szTestName,
						fVersion
					);

					continue;
				};
#elif defined(_IA64_)
				// All DX versions are crippled on IA64 as of Vista.
				if
				(
					OSVersionInfo.dwMajorVersion >= 6
				)
				{
					DPF
					(
						1,
						_T( "CD3DWindowFramework::InitializeTests:  Test group %u (\"%s\") is being skipped, because Direct3D is not supported on IA64 builds of this operating system.\n" ),
						uCount,
						pTest->m_szTestName
					);

					continue;
				};
#endif


				// Initialize our D3D helper object for this version so we can use it.
				if (FAILED(m_pD3D->SetupD3D(fVersion)))
				{
					DPF
					(
						1,
						_T( "CD3DWindowFramework::InitializeTests: Failed to create device, exiting.\n" )
					);
					hr = E_FAIL;
					goto Exit;
				};


				for (UINT nAdapter = 0; nAdapter < m_nAdapters; nAdapter++)
				{
					// Skip unneeded adapters.
					if ( (m_nAdapter && (m_nAdapter -1) != nAdapter) )
					{
						continue;
					};


					for (int nDevType = 0; nDevType < g_nDeviceTypes; nDevType++)
					{
						// Skip unneeded device types.
						if ( !(g_dwDevTypes[nDevType] & dwDevTypes) )
						{
							continue;
						};


						// Resolve ambiguous device subtype and
						// behavior flags like BESTAVAILABLE and
						// SW_OR_HW_VP.
						DWORD ResolvedBehaviors = dwBehaviors;
						if
						(
							!m_pD3D->ResolveBehaviors
							(
								nAdapter,
								g_dwDevTypes[nDevType],
								dwBehaviors,
								&ResolvedBehaviors
							)
						)
						{
							DPF(1,"CD3DWindowFramework::InitializeTests:  ResolveBehaviors failed.\n");

							continue;
						};

						// Verify that no ambiguous behavior flags remain.
						ASSERT( (ResolvedBehaviors & DEVICECREATE_SW_OR_HW_VP) == 0 );
						ASSERT( (ResolvedBehaviors & DEVICECREATE_HWVP_OR_PURE) == 0 );
						ASSERT( (ResolvedBehaviors & DEVICECREATE_BESTAVAILABLE) == 0 );


						// DX 7 can only do HWVP on HAL.
						if
						(
							fVersion < 8.0f &&
							(dwDevTypes & DEVICETYPE_HAL) == 0
						)
						{
							ResolvedBehaviors &= ~DEVICECREATE_HWVP;

							if ( (ResolvedBehaviors & DEVICECREATE_SWVP) == 0 )
								continue;
						};


						// DX 6 can't do HWVP at all.
						if (fVersion == 6.f)
						{
							ResolvedBehaviors &= ~DEVICECREATE_HWVP;

							if ( (ResolvedBehaviors & DEVICECREATE_SWVP) == 0 )
								continue;
						};


						//
						// Resolve mutually exclusive flags that
						// the test has OR'd together to mean "do
						// each of the above" into multiple calls
						// to AddDeviceMode.
						//
						// Note:  AddDeviceMode itself will prevent
						// us from adding duplicate modes.
						//

						if ( (ResolvedBehaviors & DEVICECREATE_HWVP) != 0 )
						{
							if ( (ResolvedBehaviors & DEVICECREATE_PURE) != 0 )
							{
								DebugOut.Write
								(
									_T( "CD3DWindowFramework::InitializeTests:  Test group \"%s\" requested adapter %u version %2.2f device type %u, behaviors 0x%x, dwPSGPRegChange %u, mode %dx%dx%d@%d fmt 0x%x.\n" ),
									pTest->m_szTestName,
									nAdapter,
									fVersion,
									g_dwDevTypes[nDevType],
									(((ResolvedBehaviors & ~DEVICECREATE_SWVP) & ~DEVICECREATE_MIXED) & ~DEVICECREATE_STANDARD),
									dwPSGPRegChanges,
									pMode->Mode.dwWidth,
									pMode->Mode.dwHeight,
									pMode->Mode.dwBPP,
									pMode->Mode.dwRefreshRate,
									pMode->Mode.Format.d3dfFormat
								);

								AddDeviceMode
								(
									&pMode->Mode,
									nAdapter,
									fVersion,
									g_dwDevTypes[nDevType],
									(((ResolvedBehaviors & ~DEVICECREATE_SWVP) & ~DEVICECREATE_MIXED) & ~DEVICECREATE_STANDARD),
									dwPSGPRegChanges
								);
							};


							if ( (ResolvedBehaviors & DEVICECREATE_STANDARD) != 0 )
							{
								DebugOut.Write
								(
									_T( "CD3DWindowFramework::InitializeTests:  Test \"%s\" requested adapter %u version %2.2f device type %u, behaviors 0x%x, dwPSGPRegChange %u, mode %dx%dx%d@%d fmt 0x%x.\n" ),
									pTest->m_szTestName,
									nAdapter,
									fVersion,
									g_dwDevTypes[nDevType],
									((ResolvedBehaviors & ~DEVICECREATE_SWVP) & ~DEVICECREATE_PURE),
									dwPSGPRegChanges,
									pMode->Mode.dwWidth,
									pMode->Mode.dwHeight,
									pMode->Mode.dwBPP,
									pMode->Mode.dwRefreshRate,
									pMode->Mode.Format.d3dfFormat
								);

								AddDeviceMode
								(
									&pMode->Mode,
									nAdapter,
									fVersion,
									g_dwDevTypes[nDevType],
									((ResolvedBehaviors & ~DEVICECREATE_SWVP) & ~DEVICECREATE_PURE),
									dwPSGPRegChanges
								);
							};


							if
							(
								(ResolvedBehaviors & DEVICECREATE_PURE) == 0 &&
								(ResolvedBehaviors & DEVICECREATE_STANDARD) == 0
							)
							{
								DebugOut.Write
								(
									_T( "CD3DWindowFramework::InitializeTests:  Test \"%s\" requested adapter %u version %2.2f device type %u, behaviors 0x%x, dwPSGPRegChange %u, mode %dx%dx%d@%d fmt 0x%x.\n" ),
									pTest->m_szTestName,
									nAdapter,
									fVersion,
									g_dwDevTypes[nDevType],
									(ResolvedBehaviors & ~DEVICECREATE_SWVP),
									dwPSGPRegChanges,
									pMode->Mode.dwWidth,
									pMode->Mode.dwHeight,
									pMode->Mode.dwBPP,
									pMode->Mode.dwRefreshRate,
									pMode->Mode.Format.d3dfFormat
								);

								AddDeviceMode
								(
									&pMode->Mode,
									nAdapter,
									fVersion,
									g_dwDevTypes[nDevType],
									(ResolvedBehaviors & ~DEVICECREATE_SWVP),
									dwPSGPRegChanges
								);
							};


							// Unselect 9.0 if we're doing 9.1 and the test
							// requested one or the other but not both.
							if
							(
								(GetDXVersionFlag(fVersion) & DXVERSION_91) != 0 &&
								(dwIncVersions & DXVERSION_9_OR_91) != 0
							)
							{
								dwIncVersions &= ~DXVERSION_9;
							};


							// Unselect 9.1 if we're doing 9.0 and the test
							// requested one or the other but not both.
							if
							(
								(GetDXVersionFlag(fVersion) & DXVERSION_9) != 0 &&
								(dwIncVersions & DXVERSION_9_OR_91) != 0
							)
							{
								dwIncVersions &= ~DXVERSION_91;
							};


							// Unselect 8.0 if we're doing 8.1 and the test
							// requested one or the other but not both.
							if
							(
								(GetDXVersionFlag(fVersion) & DXVERSION_81) != 0 &&
								(dwIncVersions & DXVERSION_8_OR_81) != 0
							)
							{
								dwIncVersions &= ~DXVERSION_8;
							};


							// Unselect 8.1 if we're doing 8.0 and the test
							// requested one or the other but not both.
							if
							(
								(GetDXVersionFlag(fVersion) & DXVERSION_8) != 0 &&
								(dwIncVersions & DXVERSION_8_OR_81) != 0
							)
							{
								dwIncVersions &= ~DXVERSION_81;
							};
						};


						if ( (ResolvedBehaviors & DEVICECREATE_SWVP) != 0 )
						{
							// For SWVP, PURE is invalid, so treat requests for
							// PURE or HAL as just HAL.
							if
							(
								(ResolvedBehaviors & DEVICECREATE_PURE) != 0 ||
								(ResolvedBehaviors & DEVICECREATE_STANDARD) != 0
							)
							{
								DebugOut.Write
								(
									_T( "CD3DWindowFramework::InitializeTests:  Test \"%s\" requested adapter %u version %2.2f device type %u, behaviors 0x%x, dwPSGPRegChange %u, mode %dx%dx%d@%d fmt 0x%x.\n" ),
									pTest->m_szTestName,
									nAdapter,
									fVersion,
									g_dwDevTypes[nDevType],
									((ResolvedBehaviors & ~DEVICECREATE_HWVP) & ~DEVICECREATE_PURE),
									dwPSGPRegChanges,
									pMode->Mode.dwWidth,
									pMode->Mode.dwHeight,
									pMode->Mode.dwBPP,
									pMode->Mode.dwRefreshRate,
									pMode->Mode.Format.d3dfFormat
								);

								AddDeviceMode
								(
									&pMode->Mode,
									nAdapter,
									fVersion,
									g_dwDevTypes[nDevType],
									((ResolvedBehaviors & ~DEVICECREATE_HWVP) & ~DEVICECREATE_PURE),
									dwPSGPRegChanges
								);
							}
							else
							{
								DebugOut.Write
								(
									_T( "CD3DWindowFramework::InitializeTests:  Test \"%s\" requested adapter %u version %2.2f device type %u, behaviors 0x%x, dwPSGPRegChange %u, mode %dx%dx%d@%d fmt 0x%x.\n" ),
									pTest->m_szTestName,
									nAdapter,
									fVersion,
									g_dwDevTypes[nDevType],
									(ResolvedBehaviors & ~DEVICECREATE_HWVP),
									dwPSGPRegChanges,
									pMode->Mode.dwWidth,
									pMode->Mode.dwHeight,
									pMode->Mode.dwBPP,
									pMode->Mode.dwRefreshRate,
									pMode->Mode.Format.d3dfFormat
								);

								AddDeviceMode
								(
									&pMode->Mode,
									nAdapter,
									fVersion,
									g_dwDevTypes[nDevType],
									(ResolvedBehaviors & ~DEVICECREATE_HWVP),
									dwPSGPRegChanges
								);
							};


							// Unselect 9.0 if we're doing 9.1 and the test
							// requested one or the other but not both.
							if
							(
								(GetDXVersionFlag(fVersion) & DXVERSION_91) != 0 &&
								(dwIncVersions & DXVERSION_9_OR_91) != 0
							)
							{
								dwIncVersions &= ~DXVERSION_9;
							};


							// Unselect 9.1 if we're doing 9.0 and the test
							// requested one or the other but not both.
							if
							(
								(GetDXVersionFlag(fVersion) & DXVERSION_9) != 0 &&
								(dwIncVersions & DXVERSION_9_OR_91) != 0
							)
							{
								dwIncVersions &= ~DXVERSION_91;
							};


							// Unselect 8.0 if we're doing 8.1 and the test
							// requested one or the other but not both.
							if
							(
								(GetDXVersionFlag(fVersion) & DXVERSION_81) != 0 &&
								(dwIncVersions & DXVERSION_8_OR_81) != 0
							)
							{
								dwIncVersions &= ~DXVERSION_8;
							};


							// Unselect 8.1 if we're doing 8.0 and the test
							// requested one or the other but not both.
							if
							(
								(GetDXVersionFlag(fVersion) & DXVERSION_8) != 0 &&
								(dwIncVersions & DXVERSION_8_OR_81) != 0
							)
							{
								dwIncVersions &= ~DXVERSION_81;
							};
						};
					};
				};
			};
		};
	};


	//
	// Verify that we added at least one device mode.
	//

	if ( m_pDeviceMode == NULL )
	{
		if ( m_DXGLog.InTestCase() )
		{
			m_DXGLog.EndTestCase();
		};

		((CD3DTest*)m_pCurrentTest->pTest)->BeginTestCase
		(
			_T("InitializeTests" ),
			((CD3DTest *) m_pCurrentTest->pTest)->m_uTestNumber
		);

		DebugPrint(1, _T("This test does not run against the D3D version/devices installed.\n"));

		m_DXGLog.SetTestCaseResult(WTT_TESTCASE_RESULT_SKIPPED);

		m_DXGLog.EndTestCase();

		SuggestExitCode(EXITCODE_SKIP);

		hr = E_UNSUPPORTED;
		goto Exit;
	};


	//
	// Set our current device mode to the first in the linked list.
	//

	m_pCurrentDevMode = m_pDeviceMode;


Exit:
	return hr;
};


// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::AddDisplayMode(PDISPLAYMODE pMode)
{
	bool bRes= true;
	DISPLAYMODELIST **ppDispList = &m_pDispModes;
	FMT AllDisplayFormatsPreWin8[] = {FMT_R5G6B5, FMT_X8R8G8B8};
	FMT AllDisplayFormatsPostWin8[] = {FMT_X8R8G8B8};
	UINT nAllDisplayFormatsPreWin8 = sizeof(AllDisplayFormatsPreWin8) / sizeof(FMT);
	UINT nAllDisplayFormatsPostWin8 = sizeof(AllDisplayFormatsPostWin8) / sizeof(FMT);
	FMT *AllDisplayFormats = NULL;
	UINT nAllDisplayFormats = 0;
	UINT nModeCount = 0;
	DISPLAYMODELIST **ppAnyFmt = NULL;
	DISPLAYMODE DispMode;

	// Get the Windows version
	OSVERSIONINFO windowsVersionInfo;
	windowsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&windowsVersionInfo);

	// Display modes less than 32 bpp are no longer supported on Windows 8 and later 
	if(windowsVersionInfo.dwMajorVersion > 6 || (windowsVersionInfo.dwMajorVersion == 6 && windowsVersionInfo.dwMinorVersion >= 2))
	{
		AllDisplayFormats = AllDisplayFormatsPostWin8;
		nAllDisplayFormats = nAllDisplayFormatsPostWin8;
	}
	else
	{
		AllDisplayFormats = AllDisplayFormatsPreWin8;
		nAllDisplayFormats = nAllDisplayFormatsPreWin8;
	}

	//
	// Validate parameters.
	//

	ASSERT(pMode);
	if ( pMode == NULL )
	{
		return false;
	};


	FMT fmt = (FMT)pMode->Format.d3dfFormat;

	// If we've requested ALL, add all allowable modes
	if(fmt == FMT_ALL)
	{
		memcpy(&DispMode, pMode, sizeof(DISPLAYMODE));
		for(UINT i=0; i<nAllDisplayFormats; i++)
		{
			DispMode.Format.d3dfFormat = (D3DFORMAT)AllDisplayFormats[i];
			bRes &= AddDisplayMode(&DispMode);
		}
		return bRes;
	}

	// Validate that the requested FMT is in the ALL list
	for(UINT i=0; i<nAllDisplayFormats; i++)
	{
		if(fmt == FMT_ANY)
			break;
		if(fmt == AllDisplayFormats[i])
			break;
		if( (i == nAllDisplayFormats + 1) && fmt != AllDisplayFormats[i])
		{
			DPF(1, _T("CD3DWindowFramework::AddDisplayMode:  Format not supported by FMWK.  Please update.\n"));
			return false;
		}
	}

	// Find the tail or duplicate and exit
	while(*ppDispList)
	{
		nModeCount++;

		// If this mode has already been added, exit
		if
		(
			(*ppDispList)->Mode.Format.d3dfFormat == pMode->Format.d3dfFormat &&
			(*ppDispList)->Mode.dwWidth == pMode->dwWidth &&
			(*ppDispList)->Mode.dwHeight == pMode->dwHeight &&
			(*ppDispList)->Mode.dwBPP == pMode->dwBPP
		)
		{
			return true;
		};

		// Check for ANY index
		if((FMT)(*ppDispList)->Mode.Format.d3dfFormat == FMT_ANY)
		{
			ppAnyFmt = ppDispList;
		};

		ppDispList = &((*ppDispList)->pNext);
	};

	// Allocate the new mode list.
	*ppDispList = new DISPLAYMODELIST;
	if ( NULL == *ppDispList )
	{
		DebugPrint(1, _T("CD3DWindowFramework::AddDisplayMode:  Failed to allocate display mode list.\n"));
		return false;
	};

	memset(*ppDispList, 0, sizeof(DISPLAYMODELIST));
	memcpy(&(*ppDispList)->Mode, pMode, sizeof(DISPLAYMODE));
	d3df2ddpf((*ppDispList)->Mode.Format.d3dfFormat, &(*ppDispList)->Mode.Format.ddpfPixelFormat);
	nModeCount++;
	if(fmt == FMT_ANY) ppAnyFmt = ppDispList;

	// Remove redundant state.  If FMT_ANY is in the list, we want to remove it if
	// all of the display modes are present in the list.  Otherwise, we end up running
	// one extra mode.  We need to make sure that all modes are in the list otherwise
	// there is no way to guarrantee that each group will run(as in the case of a single group
	// that needs to run in A2R10G10B10, but the rest have ANY).
	if(nModeCount > nAllDisplayFormats && NULL != ppAnyFmt)
	{
		DISPLAYMODELIST *pTemp = *ppAnyFmt;
		*ppAnyFmt = (*ppAnyFmt)->pNext;
		SAFEDELETE(pTemp);
	}

	return true;
}
// --------------------------------------------------------------------------------------

void CD3DWindowFramework::ReleaseTests()
{
	UINT uMax = GetNumTests();
	for (UINT uCount = 0; uCount < uMax; uCount++)
	{
		CD3DTest * pTest = (CD3DTest*)GetTest(uCount);

		if(pTest)
		{
			pTest->ReleaseAll();
		}
	}
}

// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::ValidateDeviceMode
(
	int nAdapter,
	float fVersion,
	DWORD dwDevType,
	DWORD dwBehaviors,
	DWORD dwPSGPRegChange
)
{
	// Filter out some common/known exceptions
	if( (fVersion < 8.f && (dwBehaviors & DEVICECREATE_HWVP && !(dwDevType & DEVICETYPE_HAL))) ||
		(fVersion == 6.f && (dwBehaviors & DEVICECREATE_HWVP)) )
		return false;

	return true;
}

// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::AddDeviceMode
(
	DISPLAYMODE * pDispMode,
	int nAdapter,
	float fVersion,
	DWORD dwDevType,
	DWORD dwBehaviors,
	DWORD dwPSGPRegChange
)
{
	DEVICEMODE ** ppCurrentDeviceMode = &m_pDeviceMode;

	//
	// Validate parameters.
	//

	// Validate the display mode pointer.
	ASSERT(pDispMode != NULL);
	if ( pDispMode == NULL )
	{
		DPF( 1, _T( "CD3DWindowFramework::AddDeviceMode:  pDispMode is NULL.\n" ) );

		return false;
	};


	// Validate device sub-type flags.
	ASSERT
	(
		!(
			(dwBehaviors & DEVICECREATE_PURE) != 0 &&
			(dwBehaviors & DEVICECREATE_STANDARD) != 0
		)
	);
	if ( (dwBehaviors & DEVICECREATE_PURE) != 0 && (dwBehaviors & DEVICECREATE_STANDARD) != 0 )
	{
		DPF( 1, _T( "CD3DWindowFramework::AddDeviceMode:  PURE and STANDARD requested in the same device mode.\n" ) );

		return false;
	};


	// Validate vertex processing flags.
	ASSERT
	(
		!(
			(dwBehaviors & DEVICECREATE_HWVP) != 0 &&
			(dwBehaviors & DEVICECREATE_SWVP) != 0
		)
	);
	if ( (dwBehaviors & DEVICECREATE_HWVP) != 0 && (dwBehaviors & DEVICECREATE_SWVP) != 0 )
	{
		DPF( 1, _T( "CD3DWindowFramework::AddDeviceMode:  HWVP and SWVP requested in same device mode.\n" ) );

		return false;
	};


	// Validate the adapter, DX version, device type, behaviors, and PSGP settings.
	if(!ValidateDeviceMode(nAdapter, fVersion, dwDevType, dwBehaviors, dwPSGPRegChange))
	{
		return false;
	};


	//
	// Walk the mode list, and bail if the requested mode is already in the list.
	//

	while ( *ppCurrentDeviceMode != NULL )
	{
		if
		(
			(*ppCurrentDeviceMode)->nAdapter == nAdapter &&
			(*ppCurrentDeviceMode)->fVersion == fVersion &&
			(*ppCurrentDeviceMode)->dwDevType == dwDevType &&
			(*ppCurrentDeviceMode)->dwBehaviors == dwBehaviors &&
			(*ppCurrentDeviceMode)->dwPSGPRegChange == dwPSGPRegChange &&
			memcmp( &((*ppCurrentDeviceMode)->pDispMode->Format.ddpfPixelFormat), &(pDispMode->Format.ddpfPixelFormat), sizeof(PIXELFORMAT)) == 0 &&
			memcmp( &((*ppCurrentDeviceMode)->pDispMode->Format.d3dfFormat), &(pDispMode->Format.d3dfFormat), sizeof(D3DFORMAT)) == 0
		)
		{
			DebugOut.Write
			(
				_T( "CD3DWindowFramework::AddDeviceMode:  Skipping redundant mode:  Adapter %u version %2.2f device type %u, behaviors 0x%x, dwPSGPRegChange %u, mode %dx%dx%d@%d fmt 0x%x.\n" ),
				nAdapter,
				fVersion,
				dwDevType,
				dwBehaviors,
				dwPSGPRegChange,
				pDispMode->dwWidth,
				pDispMode->dwHeight,
				pDispMode->dwBPP,
				pDispMode->dwRefreshRate,
				pDispMode->Format.d3dfFormat
			);

			return false;
		};

		ppCurrentDeviceMode = &((*ppCurrentDeviceMode)->pNext);
	};


	//
	// We got to the end of the list without finding the requested mode, so we need to add it.
	//

	// Allocate a new device mode
	*ppCurrentDeviceMode = new DEVICEMODE;
	if ( *ppCurrentDeviceMode == NULL )
	{
		DebugPrint(1, _T("CD3DWindowFramework::AddDeviceMode:  Failed to allocate device mode.\n"));
		return false;
	}

	// Zero out the device mode struct.
	ZeroMemory(*ppCurrentDeviceMode, sizeof(DEVICEMODE));


	// Spew to the debugger the mode that we're adding.
	DebugOut.Write
	(
		_T( "CD3DWindowFramework::AddDeviceMode:  Adding device mode:  Adapter %u version %2.2f device type %u, behaviors 0x%x, dwPSGPRegChange %u, mode %dx%dx%d@%d fmt 0x%x.\n" ),
		nAdapter,
		fVersion,
		dwDevType,
		dwBehaviors,
		dwPSGPRegChange,
		pDispMode->dwWidth,
		pDispMode->dwHeight,
		pDispMode->dwBPP,
		pDispMode->dwRefreshRate,
		pDispMode->Format.d3dfFormat
	);


	// Set the data for this mode.
	(*ppCurrentDeviceMode)->nAdapter = nAdapter;
	(*ppCurrentDeviceMode)->fVersion = fVersion;
	(*ppCurrentDeviceMode)->dwDevType = dwDevType;
	(*ppCurrentDeviceMode)->dwBehaviors = dwBehaviors;
	(*ppCurrentDeviceMode)->dwPSGPRegChange = dwPSGPRegChange;
	(*ppCurrentDeviceMode)->pDispMode = new DISPLAYMODE;
	if(NULL == (*ppCurrentDeviceMode)->pDispMode)
	{
		DebugPrint(1, _T("CD3DWindowFramework::AddDeviceMode:  Failed to allocate display mode.\n"));
		return false;
	}

	ZeroMemory((*ppCurrentDeviceMode)->pDispMode, sizeof(DISPLAYMODE));
	(*ppCurrentDeviceMode)->pDispMode->dwWidth = pDispMode->dwWidth;
	(*ppCurrentDeviceMode)->pDispMode->dwHeight = pDispMode->dwHeight;
	(*ppCurrentDeviceMode)->pDispMode->dwBPP = pDispMode->dwBPP;
	(*ppCurrentDeviceMode)->pDispMode->dwRefreshRate = pDispMode->dwRefreshRate;
	(*ppCurrentDeviceMode)->pDispMode->Format.ddpfPixelFormat = pDispMode->Format.ddpfPixelFormat;
	(*ppCurrentDeviceMode)->pDispMode->Format.d3dfFormat = pDispMode->Format.d3dfFormat;
	//(*ppCurrentDeviceMode)->pDispMode->Format.pNext = pDispMode->Format.pNext;


	return true;
}

// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::PreWindowCreate(void)
{
	return true;
}

// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::PostWindowCreate(void)
{
	return true;
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::Shutdown(void)
{
	DISPLAY_DEVICE DispDev;
	int nAttached = 0;

	// Release tests.
	ReleaseTests();

	// Release framework objects.
	ReleaseAll();

	if(m_hSmallFont)
	{
		DeleteObject(m_hSmallFont);
		m_hSmallFont = NULL;
	}

	if(m_hLargeFont)
	{
		DeleteObject(m_hLargeFont);
		m_hLargeFont = NULL;
	}

	// Reset the desktop modes.
	if(NULL != m_pOrigModes)
	{
		DispDev.cb = sizeof(DISPLAY_DEVICE);
		for(int i=0; EnumDisplayDevices(NULL, i, &DispDev, 0);i++)
		{
			if (DispDev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
			{
				DPF
				(
					1,
					_T( "CD3DWindowFramework::Shutdown:  Restoring adapter %u (%s) to mode %u x %u x %u @ %u Hz.\n" ),
					nAttached,
					DispDev.DeviceName,
					m_pOrigModes[ nAttached ].dmPelsWidth,
					m_pOrigModes[ nAttached ].dmPelsHeight,
					m_pOrigModes[ nAttached ].dmBitsPerPel,
					m_pOrigModes[ nAttached ].dmDisplayFrequency
				);

				m_pOrigModes[ nAttached ].dmSize = sizeof( m_pOrigModes[ 0 ] );
				m_pOrigModes[ nAttached ].dmSpecVersion = DM_SPECVERSION;

				LONG ChangeDisplayModeResult = ChangeDisplaySettingsEx
				(
					DispDev.DeviceName,
					&m_pOrigModes[ nAttached ],
					NULL,
					CDS_UPDATEREGISTRY,
					NULL
				);

				if ( DISP_CHANGE_SUCCESSFUL != ChangeDisplayModeResult )
				{
					DebugPrint
					(
						1,
						_T( "CD3DWindowFramework::Shutdown:  ChangeDisplaySettingsEx failed with error %d.\n"),
						ChangeDisplayModeResult
					);
				};

				++nAttached;
			};
		};

		SAFEDELETEA(m_pOrigModes);
	};


	// Release the coverage object.
	if(m_bCoverage)
	{
		CoverageUninitialize();

		FreeLibrary(m_hCovDLL);
		m_hCovDLL = NULL;
	};


	// Release all the test management info.
	CTestManager::ReleaseAll();


	// Release the menu.
	DestroyMenu(m_hMenu);
	m_hMenu = NULL;


	// Release child windows.
	Close();
}


// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::TestSelected(CTest *pTest)
{
	TCHAR szBuffer[MAX_PATH * 2];
	bool bCheckCmdkey = false;

	StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T("!%s"), (TCHAR *)pTest->m_szCommandKey);

	// !CmdKey was set, so skip this group
	if(m_CmdLine.KeySet(szBuffer))
	{
		return false;
	}

	// -CmdKey was set, return true
	if(m_CmdLine.KeySet((TCHAR *)pTest->m_szCommandKey))
	{
		return true;
	}

	// First, we need to check if specific tests have been selected
	UINT uMax = GetNumTests();
	for (UINT uCount = 0; uCount < uMax; uCount++)
	{
		CD3DTest * pTempTest = (CD3DTest*)GetTest(uCount);
		if(QueryTestRange(pTempTest))
		{
			bCheckCmdkey = true;
		}
	}

	return (!bCheckCmdkey || QueryTestRange(pTest));
}

// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::QueryTestRange(CTest * pTest)
{
	TCHAR   szBuffer[MAX_PATH * 2];
	TCHAR * szString = szBuffer;

	if(m_CmdLine.KeySet((TCHAR *)pTest->m_szCommandKey))
	{
		// Add the whole range (the default range).

		return true;
	}

	if(m_CmdLine.ReadString((TCHAR *)pTest->m_szCommandKey, szString, ARRAYSIZE(szBuffer)))
	{
		// If the test requires serial execution, add the whole range
		if(((CD3DTest*)pTest)->m_Options.TestOptions.dwCaps & TESTCAPS_SERIALEXECUTION)
			return true;

		// Parse the string to determine the test ranges to execute.

		while (lstrlen(szString) > 0)
		{
			UINT    uStart=1;
			UINT    uStop=0xFFFFFFFF;
			TCHAR * szBuf = szString;

			// Look for -A case, as in 1-A

			if('-' == *szString)
			{
				int     nAdd;

				szString++;
				szBuf = szString;

				while ('0' <= *szBuf && '9' >=*szBuf)
					szBuf++;

				nAdd = (0 == *szBuf) ? 0:1;

				*szBuf=0;

				m_CmdLine.ATOI(szString,(PINT)&uStop);

				szString = szBuf + nAdd;
			}
			else
			{
				// Look for A-B or A- (A-0xFFFFFFFF), or A.

				int     nAdd;
				bool    fEnd = false;

				while ('0' <= *szBuf && '9' >=*szBuf)
					szBuf++;

				nAdd = (0 == *szBuf) ? 0:1;

				if('-' == *szBuf)
					fEnd = true;

				*szBuf=0;

				m_CmdLine.ATOI(szString,(PINT)&uStart);

				if(fEnd)
				{
					// Determine end test

					szBuf++;
					szString = szBuf;

					while ('0' <= *szBuf && '9' >=*szBuf)
						szBuf++;

					// End of string?

					nAdd = (0 == *szBuf) ? 0:1;
					*szBuf = 0;

					// If there was an ending limit, determine what it was.

					if(szString != szBuf)
						m_CmdLine.ATOI(szString,(PINT) &uStop);
				}
				else
					uStop = uStart;     // Single number case.

				// Move to next range.

				szString = szBuf + nAdd;
			}

			pTest->AddTestRange(uStart,uStop,true);
		}

		return true;
	}

	return false;
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::WriteStatusBar(int nPart, LPCTSTR szFormat, ...)
{
	TCHAR   szBuffer[MAX_PATH];
	va_list list;

	if(NULL != m_pStatusBar)
	{
		va_start(list,szFormat);
		StringCchVPrintf(szBuffer, ARRAYSIZE(szBuffer), szFormat, list);

		SendMessage(m_pStatusBar->m_hWindow,SB_SETTEXT,nPart,(LPARAM)szBuffer);
	}
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::WriteCommandLineHelp(LPCTSTR szFormat, ...)
{
	TCHAR szBuffer[1024] = _T( "" );
	va_list list;

	if (NULL != m_pHelpWindow)
	{
		va_start(list, szFormat);
		StringCchVPrintf(szBuffer, ARRAYSIZE(szBuffer), szFormat, list);
		StringCchCat(szBuffer, ARRAYSIZE(szBuffer), "\n");

		m_pHelpWindow->WriteToLogWhole(1, szBuffer);
	}
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::SuggestExitCode(UINT uCode)
{
	m_uExitCode |= (uCode & 0xFFFF0000);

	// Record failures only.

	if(EXITCODE_SUCCESS != (uCode & 0x0000FFFF))
		m_uExitCode |= (uCode & 0x0000FFFF);

	// Filter all but exit codes for BVT (remove exit class).
	// BVT tools interpret zero as pass, non-zero as fail.

	if(m_CmdLine.KeySet("BVT"))
		m_uExitCode &= (EXITCODE_ABORT | EXITCODE_SKIP | EXITCODE_FAIL);
}

// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::KeyPress(WPARAM wKey, bool bOnce)
{
	if(bOnce)
	{
		if(m_bKey[wKey] == 2)
		{
			m_bKey[wKey] = 1;
			return true;
		}

		return false;
	}

	return m_bKey[wKey] > 0;
}               

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::ReleaseAll()
{
	DEVICEMODE * pDevice = m_pDeviceMode;
	DEVICEMODE * pTemp = NULL;
	DISPLAYMODELIST * pDisplayModes = m_pDispModes;
	DISPLAYMODELIST * pTempMode = NULL;

	while(pDevice)
	{
		pTemp = pDevice->pNext;
		SAFEDELETE(pDevice->pDispMode);
		SAFEDELETE(pDevice);
		pDevice = pTemp;
	}
	m_pDeviceMode = NULL;

	if(m_pD3D)
	{
		m_pD3D->ReleaseD3D();			// Release all D3D object
		UINT uRef = m_pD3D->Release();	// Release the wrapper
		if(uRef)
		{
			DebugPrint(1, _T("CD3DWindowFramework::ReleaseAll:  m_pD3D ref count = %d after final Release()\n"), uRef);
		}
	}

	while(pDisplayModes)
	{
		pTempMode = pDisplayModes->pNext;
		SAFEDELETE(pDisplayModes);
		pDisplayModes = pTempMode;
	}

	SAFEDELETEA(m_pCurModes);

	// These windows get destoyed when we call Close() on the toplevel parent window
	m_pDisplayWindow = NULL;
	m_pLogWindow = m_pStatWindow = m_pMsgWindow = m_pDetailsWindow = NULL;
	m_pTabWindow = NULL;
	m_pRebar = NULL;
	m_pStatusBar = NULL;
	m_pHelpWindow = NULL;

	// Release the logging vars
	SAFEDELETEA(m_LogInfo.sTestName);
	SAFEDELETEA(m_LogInfo.sExeName);
	SAFEDELETEA(m_LogInfo.sLocalDirectory);
	SAFEDELETEA(m_LogInfo.sDBDirectory);
	SAFEDELETEA(m_LogInfo.sCmdLine);
	SAFEDELETEA(m_LogInfo.sStyleSheet);
	memset(&m_LogInfo, 0, sizeof(DXGLOG_INFO));
	SAFEDELETEA(m_DeviceInfo.sDisplay);
	SAFEDELETEA(m_DeviceInfo.sDriver);
	SAFEDELETEA(m_DeviceInfo.sDriverVersion);
	SAFEDELETEA(m_DeviceInfo.sPnPID);
	SAFEDELETEA(m_DeviceInfo.sResolution);
	SAFEDELETEA(m_DeviceInfo.sDevice);
	memset(&m_DeviceInfo, 0, sizeof(DXGLOG_DEVICEINFO));
}

// --------------------------------------------------------------------------------------

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	// Increment the counter
	(*((int*)dwData))++;

	return TRUE;
}

// --------------------------------------------------------------------------------------

int CD3DWindowFramework::GetAdapterCount()
{
	HMODULE hUser32 = GetModuleHandle("USER32.DLL");
	ASSERT( hUser32 != NULL );
	ENUMMONITORPROC EnumMonitorProc = (ENUMMONITORPROC) GetProcAddress(hUser32, "EnumDisplayMonitors");
	if (NULL != EnumMonitorProc)
	{
		int nAdapters = 0;

		if(EnumMonitorProc(NULL, NULL, MonitorEnumProc, (LPARAM)&nAdapters))
			return nAdapters;
		else
		{
			// Enumeration failed, assume only 1 monitor
			return 1;
		}
	}
	else
	{
		return 1;
	}
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::MuteLog()
{
	m_bMuteLog = true;
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::UnMuteLog()
{
	m_bMuteLog = false;
}

// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::BeginTestExecution()
{
	//
	// Validate state.
	//

	// Check to see if a test has been specified to execute   
	if (NULL == m_pCurrentTest)
		return false;


	//
	// Reset the test execution state (start, stop, test number, etc.).
	//

	m_pCurrentTest->pTest->Reset();


	//
	// Disable DWM if needed.
	//

	if ( ((CD3DTest*) m_pCurrentTest->pTest)->m_Options.TestOptions.bDisableDWM == true )
	{
		// Stop DWM.
		CDWMController DWMController( m_pSimpleLog );
		HRESULT hStopDWMResult = DWMController.ToggleDWM( false );

		// If the service...
		if
		(
			// ...was already stopped...
			hStopDWMResult == S_FALSE ||

			// ...or doesn't exist...
			hStopDWMResult == HRESULT_FROM_WIN32( ERROR_SERVICE_DOES_NOT_EXIST )
		)
		{
			DPF
			(
				3,
				_T( "DWM was already stopped.\n" )
			);

			// ...then we don't need to do anything now, or later...
			((CD3DTest*) m_pCurrentTest->pTest)->m_Options.TestOptions.bDisableDWM = false;
		}
		// ...otherwise, if we hit an error...
		else if
		(
			FAILED( hStopDWMResult )
		)
		{
			// ...then note the error...
			DPF
			(
				1,
				_T( "CD3DWindowFramework::InitializeTests:  ToggleDWM failed with error 0x%x.\n" ),
				hStopDWMResult
			);

			// ...and note that we didn't actually touch DWM,
			// so there's nothing to undo later.
			((CD3DTest*) m_pCurrentTest->pTest)->m_Options.TestOptions.bDisableDWM = false;
		}
		else if ( hStopDWMResult == S_OK )
		{
			DPF
			(
				3,
				_T( "Successfully stopped DWM for this test group.\n" )
			);
		};
	};


	// Call the test group's SetupTest method.
	bool bSetupTestResult = ((CD3DTest*)(m_pCurrentTest->pTest))->SetupTest();

	return bSetupTestResult;
}

// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::EndTestExecution()
{
	//
	// Validate members.
	//

	// Check to see whether the current test is valid.
	if (NULL == m_pCurrentTest)
	{
		return false;
	};


	//
	// Initialize locals.
	//

	CD3DTest * pTest = (CD3DTest *) m_pCurrentTest->pTest;


	//
	// Log the result of CleanupTest as a separate test case in the log.
	//

	// If we were still inside a BeginTestCase/EndTestCase pair...
	if ( m_DXGLog.InTestCase() )
	{
		// ...then close it.
		m_DXGLog.EndTestCase();
	};


	// Begin a new test case.
	pTest->BeginTestCase
	(
		_T("CleanupTest"),
		pTest->m_uTotalTests + 1
	);


	// Call the test group's CleanupTest method.
	bool bCleanupTestResult = pTest->CleanupTest();

	// Log the result of CleanupTest.
	if ( bCleanupTestResult == true )
	{
		m_DXGLog.SetTestCaseResult(WTT_TESTCASE_RESULT_PASS);
		pTest->m_fPassPercentage = 1.f;
	}
	else
	{
		m_DXGLog.SetTestCaseResult(WTT_TESTCASE_RESULT_FAIL);
	};

	// End the test case.
	pTest->EndTestCase();


	//
	// Add repro information for this group.
	//

	if ( m_DXGLog.InTestGroup() == true )
	{
		//
		// WTT workaround:
		// Log a placeholder failure to make WTT take notice of a
		// result with aborted test cases.
		//

		if
		(
			// Only the HCK version of WTT still has this problem.
			(
				m_bWHQL == true ||
				RunningUnderWLK() == true
			) &&
			m_DXGLog.GetGroupAbort() > 0 &&
			m_DXGLog.GetGroupFail() == 0
		)
		{
			m_DXGLog.BeginTestCase( _T( "Test group had aborts." ), 0xFFFFFFFF );

			m_DXGLog.Trace(1, _T("***  Test group had aborts but no failures.  This test case is a place-holder failure for WTT.\n"));

			m_DXGLog.SetTestCaseResult( WTT_TESTCASE_RESULT_FAIL );

			m_DXGLog.EndTestCase();
		};


		if ( m_DXGLog.IsGroupFail() )
		{
			WriteToLog
			(
				1,
				_T("\nTo execute just this group, append \"-%s\" to the cmdline.\n"),
				pTest->m_szCommandKey
			);
		};


		//
		// End this test group.
		//

		m_DXGLog.EndTestGroup();
	};


	//
	// Re-enable DWM if necessary.
	//
	// N.B. [SEdmison]:
	// We only do this if we successfully disabled DWM for
	// a particular group AND we didn't already disable it
	// globally due to the -stopdwm command line argument.
	// (If it's disabled globally, it will get re-enabled
	// elsewhere.)
	//

	if
	(
		pTest->m_Options.TestOptions.bDisableDWM == true &&
		m_bReenableDWM == false
	)
	{
		// Start DWM.
		CDWMController DWMController( m_pSimpleLog );
		HRESULT hStartDWMResult = DWMController.ToggleDWM( true );

		if ( FAILED( hStartDWMResult ) )
		{
			DPF
			(
				1,
				_T( "CD3DWindowFramework::EndTestExecution:  ToggleDWM failed with error 0x%x after test group.\n" ),
				hStartDWMResult
			);
		}
		else
		{
			DPF
			(
				3,
				_T( "Successfully restarted DWM after test group.\n" )
			);
		};
	};


	return bCleanupTestResult;
}

// --------------------------------------------------------------------------------------

HRESULT CD3DWindowFramework::LoadNextTest(DEVICEMODE *pMode)
{
	HRESULT hr = S_OK;

	if(NULL == m_pCurrentTest)
	{
		DebugOut.Write
		(
			_T( "CD3DWindowFramework::LoadNextTest:  m_pCurrentTest == NULL.\n" )
		);

		return E_FAIL;
	};


	// Initialize the next test with the current device mode.
	DebugOut.Write
	(
		_T( "CD3DWindowFramework::LoadNextTest:  Initializing test \"%s\", adapter %u version %2.2f device type %u, behaviors 0x%x, mode %dx%dx%d@%d fmt 0x%x.\n"),
		m_pCurrentTest->pTest->m_szTestName,
		pMode->nAdapter,
		pMode->fVersion,
		pMode->dwDevType,
		pMode->dwBehaviors,
		pMode->pDispMode->dwWidth,
		pMode->pDispMode->dwHeight,
		pMode->pDispMode->dwBPP,
		pMode->pDispMode->dwRefreshRate,
		pMode->pDispMode->Format.d3dfFormat
	);

	hr = ((CD3DTest*)m_pCurrentTest->pTest)->SetupDeviceMode(pMode);
	if(FAILED(hr))
	{
		DebugPrint(2, _T("CD3DWindowFramework::LoadNextTest:  SetupDeviceMode failed.\n"));
		return hr;
	}

	return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT CD3DWindowFramework::NextTest(DEVICEMODE *pMode)
{
	HRESULT hr = S_OK;

	if (NULL == m_pCurrentTest)
		return E_FAIL;

	// If we're supposed to be reversing the group, don't check for more modes
	// in the current group.
	if (m_bReverseGroup || ((CD3DTest*)m_pCurrentTest->pTest)->NextTestMode())
	{
		do
		{
			m_pCurrentTest = m_bReverseGroup ? m_pCurrentTest->pPrev : m_pCurrentTest->pNext;
			m_uTestNumber +=  m_bReverseGroup ? -1 : 1;
		} while
		(
			NULL != m_pCurrentTest &&
			(
				( (CD3DTest *) m_pCurrentTest->pTest )->IsSelected() == false ||
				( (hr = LoadNextTest(pMode) ) == E_UNSUPPORTED )
			)
		);

		hr =  m_pCurrentTest == NULL ? E_UNSUPPORTED : hr;
	}

	return hr;
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::UpdateControls(CD3DTest *pTest)
{
	TCHAR szCaption[MAX_D3D_STRING];
	CWindow *pWin = NULL;

	if(pTest->m_pCurrentMode->bWindowed)
	{
		StringCchPrintf(szCaption, ARRAYSIZE(szCaption), _T("%s - \0"), pTest->m_szTestName);

		// Set the test caption
		pTest->ConstructTestCaption(szCaption, ARRAYSIZE(szCaption));
		SetWindowText(m_hWindow, szCaption);
		// Update the status bar
		WriteStatusBar
		(
			0,
			_T("Executing Test(%d/%d), Mode(%d/%d), Group(%d/%d)"), 
			pTest->m_uTestNumber,
			pTest->m_uTotalTests,
			pTest->m_uCurrentMode,
			pTest->m_uTotalModes,
			m_uTestNumber,
			m_uTotalTests
		);

		// Update the testnum edit box
		_itot(pTest->m_uTestNumber, (TCHAR*)szCaption, 10);
		pWin = GetWindow(WND_TESTNUM);
		SendMessage(pWin->m_hWindow, WM_SETTEXT, 0, (LPARAM)szCaption);

		// Update the log windows
		pWin = m_pTabWindow->GetSelectedTab();
		if(pWin == m_pStatWindow)
			m_pStatWindow->Update();
		else if(pWin == m_pLogWindow)
			m_pLogWindow->Update();
		else if(pWin == m_pDetailsWindow)
			m_pDetailsWindow->Update();
		else if(pWin == m_pMsgWindow)
			m_pMsgWindow->Update();
	}
}

// --------------------------------------------------------------------------------------

void CD3DWindowFramework::ClearControls()
{
	m_pStatWindow->ClearEntries();
	m_pDetailsWindow->ClearEntries();
	m_pMsgWindow->ClearEntries();
}

// --------------------------------------------------------------------------------------

INT_PTR CALLBACK CD3DWindowFramework::HelpProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hCtrl = NULL;

	switch (nMsg) 
	{ 
	case WM_INITDIALOG:
		hCtrl = GetDlgItem(hWnd, IDC_HELPEDIT);
		SendMessage(hCtrl, EM_SETBKGNDCOLOR, 0, RGB(0,0,0));
		SendMessage(hCtrl, WM_SETTEXT, 0, lParam);
		return TRUE; 
	case WM_COMMAND: 
		switch (LOWORD(wParam)) 
		{ 
		case IDOK: 
			EndDialog(hWnd, TRUE);
			return TRUE; 
		case IDCANCEL: 
			EndDialog(hWnd, TRUE);
			return TRUE; 
		} 
	} 
	return FALSE; 
}

// --------------------------------------------------------------------------------------

INT_PTR CALLBACK CD3DWindowFramework::ImageCompareProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	static CImageCompare *pIC = NULL;
	static const UINT nBuffer = 16;
	static TCHAR tcsBuffer[nBuffer];
	static int nDec, nSign;

	switch (nMsg) 
	{ 
	case WM_INITDIALOG:
		if(0 == lParam)
			break;
		pIC = (CImageCompare*)lParam;
		StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("%.3f\0"), pIC->GetPassRequired());
		SetDlgItemText(hWnd, IDC_CONFREQ, tcsBuffer);
		StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("%.3f\0"), pIC->GetTolPerPixelChannel());
		SetDlgItemText(hWnd, IDC_TOLPERPIXELCHANNEL, tcsBuffer);
		StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("%.3f\0"), pIC->GetTolPerPixel());
		SetDlgItemText(hWnd, IDC_TOLPERPIXEL, tcsBuffer);
		StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("%.3f\0"), pIC->GetImgDeltaWght());
		SetDlgItemText(hWnd, IDC_TOLPERCHANNEL, tcsBuffer);
		SetDlgItemText(hWnd, IDC_MINPIXELS, _ultot(pIC->GetMinPixels(), tcsBuffer,10));
		SetDlgItemText(hWnd, IDC_MAXPIXELS, _ultot(pIC->GetMaxPixels(), tcsBuffer,10));
		SendMessage(GetDlgItem(hWnd, IDC_CMPIMGMODE), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("RGB"));
		SendMessage(GetDlgItem(hWnd, IDC_CMPIMGMODE), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("Red"));
		SendMessage(GetDlgItem(hWnd, IDC_CMPIMGMODE), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("Green"));
		SendMessage(GetDlgItem(hWnd, IDC_CMPIMGMODE), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("Blue"));
		SendMessage(GetDlgItem(hWnd, IDC_CMPIMGMODE), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("Alpha"));
		SendMessage(GetDlgItem(hWnd, IDC_CMPIMGMODE), CB_SETCURSEL, (WPARAM)0, (LPARAM)pIC->GetCmpImgMode());

		if(pIC->GetGain() == -1.f)
		{
			StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("%.3f\0"), 5.0f);
			SetDlgItemText(hWnd, IDC_GAIN, tcsBuffer);
			SendMessage(GetDlgItem(hWnd, IDC_ADAPTIVEGAIN), BM_SETCHECK,  (WPARAM)BST_CHECKED, 0);
			SendMessage(GetDlgItem(hWnd, IDC_GAIN), WM_ENABLE,  (WPARAM)FALSE, 0);
		}
		else
		{
			StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("%.3f\0"), pIC->GetGain());
			SetDlgItemText(hWnd, IDC_GAIN, tcsBuffer);
			SendMessage(GetDlgItem(hWnd, IDC_ADAPTIVEGAIN), BM_SETCHECK,  (WPARAM)BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hWnd, IDC_GAIN), WM_ENABLE,  (WPARAM)TRUE, 0);
		}
		SendMessage(GetDlgItem(hWnd, IDC_FAILONLY), BM_SETCHECK, (WPARAM)(pIC->GetFailOnly() ? TRUE : FALSE), 0);
		return TRUE; 
	case WM_COMMAND: 
		if(NULL == pIC)
			break;
		switch (LOWORD(wParam)) 
		{ 
		case IDOK:
		case IDAPPLY:
			GetDlgItemText(hWnd, IDC_CONFREQ, tcsBuffer, nBuffer);
			pIC->SetPassRequired((float)atof(tcsBuffer));
			GetDlgItemText(hWnd, IDC_TOLPERPIXELCHANNEL, tcsBuffer, nBuffer);
			pIC->SetTolPerPixelChannel((float)atof(tcsBuffer));
			GetDlgItemText(hWnd, IDC_TOLPERPIXEL, tcsBuffer, nBuffer);
			pIC->SetTolPerPixel((float)atof(tcsBuffer));
			GetDlgItemText(hWnd, IDC_TOLPERCHANNEL, tcsBuffer, nBuffer);
			pIC->SetImgDeltaWght((float)atof(tcsBuffer));
			GetDlgItemText(hWnd, IDC_MINPIXELS, tcsBuffer, nBuffer);
			pIC->SetMinPixels((UINT)_ttol(tcsBuffer));
			GetDlgItemText(hWnd, IDC_MAXPIXELS, tcsBuffer, nBuffer);
			pIC->SetMaxPixels((UINT)_ttol(tcsBuffer));
			pIC->SetCmpImgMode((CMPIMGMODE)SendMessage(GetDlgItem(hWnd, IDC_CMPIMGMODE), CB_GETCURSEL, 0, 0));

			if(BST_CHECKED & SendMessage(GetDlgItem(hWnd, IDC_FAILONLY), BM_GETSTATE, 0, 0))
				pIC->SetFailOnly(true);
			else
				pIC->SetFailOnly(false);

			if(BST_CHECKED & SendMessage(GetDlgItem(hWnd, IDC_ADAPTIVEGAIN), BM_GETSTATE, 0, 0))
				pIC->SetGain(-1.f);
			else
			{
				GetDlgItemText(hWnd, IDC_GAIN, tcsBuffer, nBuffer);
				pIC->SetGain((float)atof(tcsBuffer));
			}
			SendMessage(::GetParent(hWnd), WM_COMMAND, (WPARAM)IDM_PROCESSFRAME, 0L);

			if(LOWORD(wParam) == IDOK)
			{
				pIC = NULL; 
				EndDialog(hWnd, TRUE);
			}
			return TRUE; 
		case IDCANCEL: 
			pIC = NULL;
			EndDialog(hWnd, TRUE);
			return TRUE;
		case IDC_ADAPTIVEGAIN:
			if(BST_CHECKED & SendMessage(GetDlgItem(hWnd, IDC_ADAPTIVEGAIN), BM_GETSTATE, 0, 0))
			{
				SendMessage(GetDlgItem(hWnd, IDC_GAIN), WM_ENABLE,  (WPARAM)FALSE, 0);
			}
			else
			{
				SendMessage(GetDlgItem(hWnd, IDC_GAIN), WM_ENABLE,  (WPARAM)TRUE, 0);
			}
			return TRUE;
		} 
	} 
	return FALSE; 
}

// --------------------------------------------------------------------------------------

INT_PTR CALLBACK CD3DWindowFramework::ConfigureProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	static CCommandLine *pCmdline = NULL;
	static const UINT nBuffer = 64, nBuffer2 = 16;
	static TCHAR tcsBuffer[nBuffer] = {0}, tcsBuffer2[nBuffer2] = {0};
	static LRESULT lrIndex = 0;

	switch (nMsg) 
	{ 
	case WM_INITDIALOG:
		if(0 == lParam)
			break;
		pCmdline = (CCommandLine*)lParam;
		for(int i=0; i<g_nVersions; i++)
		{
			StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("DX%.1f\0"), g_pVersions[i]);
			SendMessage(GetDlgItem(hWnd, IDC_CFG_INTERFACE), LB_ADDSTRING, (WPARAM)0, (LPARAM)tcsBuffer);
		}
		SendMessage(GetDlgItem(hWnd, IDC_CFG_DEVTYPE), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T(""));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_DEVTYPE), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("HAL"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_DEVTYPE), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("REF"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_DEVTYPE), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("SW"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_DEVTYPE), CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
		SendMessage(GetDlgItem(hWnd, IDC_CFG_VP), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T(""));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_VP), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("SW"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_VP), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("HW"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_VP), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		SendMessage(GetDlgItem(hWnd, IDC_CFG_BEHAVIORS), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T(""));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_BEHAVIORS), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("PURE"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_BEHAVIORS), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		SendMessage(GetDlgItem(hWnd, IDC_CFG_DESKTOP), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("ANY"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_DESKTOP), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("X8R8G8B8"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_DESKTOP), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("R5G6B5"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_DESKTOP), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("A2R10G10B10"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_DESKTOP), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("A2B10G10R10_XR_BIAS"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_DESKTOP), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("ALL"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_DESKTOP), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		SendMessage(GetDlgItem(hWnd, IDC_CFG_ADAPTERID), WM_SETTEXT, (WPARAM)0, (LPARAM)_T("1"));
		if(hWnd != 0)
		{
			SetDlgItemText(hWnd, IDC_DISPWIDTH, _T("Current"));
			SetDlgItemText(hWnd, IDC_DISPHEIGHT, _T("Current"));
		}
		SendMessage(GetDlgItem(hWnd, IDC_CFG_WINMODE), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T(""));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_WINMODE), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("Window"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_WINMODE), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("Fullscreen"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_WINMODE), CB_ADDSTRING, (WPARAM)0, (LPARAM)_T("All"));
		SendMessage(GetDlgItem(hWnd, IDC_CFG_WINMODE), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		return TRUE; 
	case WM_COMMAND: 
		if(NULL == pCmdline)
			break;
		switch (LOWORD(wParam)) 
		{ 
		case IDOK:
			if(LOWORD(wParam) == IDOK)
			{
				// Get the interfaces
				for(LRESULT lr=SendMessage(GetDlgItem(hWnd, IDC_CFG_INTERFACE), LB_GETCOUNT, 0,0)-1; lr >= 0; lr--)
				{
					if(SendMessage(GetDlgItem(hWnd, IDC_CFG_INTERFACE), LB_GETSEL, lr,0))
					{
						StringCchCopy(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("-"));
						SendMessage(GetDlgItem(hWnd, IDC_CFG_INTERFACE), LB_GETTEXT, lr, (LPARAM)tcsBuffer+_tcslen(tcsBuffer));
						pCmdline->AddKeys(tcsBuffer);
					}
				}
				// Get the device modifier
				StringCchCopy(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("-SRC:"));
				lrIndex = SendMessage(GetDlgItem(hWnd, IDC_CFG_BEHAVIORS), CB_GETCURSEL, 0, 0);
				SendMessage(GetDlgItem(hWnd, IDC_CFG_BEHAVIORS), CB_GETLBTEXT, lrIndex, (LPARAM)&tcsBuffer2);
				if(0 == _tcsicmp(tcsBuffer2, _T("PURE")))
					StringCchCat(tcsBuffer, ARRAYSIZE(tcsBuffer), tcsBuffer2);
				else
				{
					lrIndex = SendMessage(GetDlgItem(hWnd, IDC_CFG_VP), CB_GETCURSEL, 0, 0);
					SendMessage(GetDlgItem(hWnd, IDC_CFG_VP), CB_GETLBTEXT, lrIndex, (LPARAM)&tcsBuffer2);
					if(0 == _tcsicmp(tcsBuffer2, _T("HW")))
						StringCchCat(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("HVP"));
					else if(0 == _tcsicmp(tcsBuffer2, _T("SW")))
						StringCchCat(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("SVP"));
				}
				lrIndex = SendMessage(GetDlgItem(hWnd, IDC_CFG_DEVTYPE), CB_GETCURSEL, 0,0);
				SendMessage(GetDlgItem(hWnd, IDC_CFG_DEVTYPE), CB_GETLBTEXT, lrIndex, (LPARAM)tcsBuffer+_tcslen(tcsBuffer));
				if(_tcslen(tcsBuffer) > 5)
					pCmdline->AddKeys(tcsBuffer);

				// Get the displaymode modifier
				StringCchCopy(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("-DisplayMode:"));
				GetDlgItemText(hWnd, IDC_DISPWIDTH, tcsBuffer2, nBuffer2);
				if(atoi(tcsBuffer2) == 0)
					StringCchCat(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("?x"));
				else
				{
					StringCchCat(tcsBuffer, ARRAYSIZE(tcsBuffer), tcsBuffer2);
					StringCchCat(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("x"));
				}
				GetDlgItemText(hWnd, IDC_DISPHEIGHT, tcsBuffer2, nBuffer2);
				if(atoi(tcsBuffer2) == 0)
					StringCchCat(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("?x"));
				else
				{
					StringCchCat(tcsBuffer, ARRAYSIZE(tcsBuffer), tcsBuffer2);
					StringCchCat(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("x"));
				}

				lrIndex = SendMessage(GetDlgItem(hWnd, IDC_CFG_DESKTOP), CB_GETCURSEL, 0,0);
				SendMessage(GetDlgItem(hWnd, IDC_CFG_DESKTOP), CB_GETLBTEXT, lrIndex, (LPARAM)tcsBuffer2);
				if(_tcsicmp(tcsBuffer2, _T("ANY")) == 0)
					StringCchCat(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("?"));
				else
					StringCchCat(tcsBuffer, ARRAYSIZE(tcsBuffer), tcsBuffer2);

				if(_tcsicmp(tcsBuffer, _T("-DisplayMode:?x?x?")) != 0)
					pCmdline->AddKeys(tcsBuffer);

				// Get the window mode
				lrIndex = SendMessage(GetDlgItem(hWnd, IDC_CFG_WINMODE), CB_GETCURSEL, 0, 0);
				SendMessage(GetDlgItem(hWnd, IDC_CFG_WINMODE), CB_GETLBTEXT, lrIndex, (LPARAM)&tcsBuffer2);
				if(0 != _tcsicmp(tcsBuffer2, _T("")))
				{
					StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("-Winmode:%s"), tcsBuffer2);
					pCmdline->AddKeys(tcsBuffer);
				}

				// Get the adapter ID
				StringCchCopy(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("-M:"));
				SendMessage(GetDlgItem(hWnd, IDC_CFG_ADAPTERID), WM_GETTEXT, (WPARAM)64-_tcslen(tcsBuffer), (LPARAM)tcsBuffer+_tcslen(tcsBuffer));
				if(_tcslen(tcsBuffer) > 3)
					pCmdline->AddKeys(tcsBuffer);

				EndDialog(hWnd, TRUE);
				pCmdline = NULL;
			}
			return TRUE; 
		case IDCANCEL: 
			pCmdline = NULL;
			EndDialog(hWnd, FALSE);
			return TRUE;
		case IDC_CFG_PURE:
			if(BST_CHECKED & SendMessage(GetDlgItem(hWnd, IDC_CFG_PURE), BM_GETSTATE, 0, 0))
			{
				SendMessage(GetDlgItem(hWnd, IDC_CFG_VP), WM_ENABLE,  (WPARAM)FALSE, 0);
			}
			else
			{
				SendMessage(GetDlgItem(hWnd, IDC_CFG_VP), WM_ENABLE,  (WPARAM)TRUE, 0);
			}
			return TRUE;
		} 
	} 
	return FALSE; 
}

// --------------------------------------------------------------------------------------

INT_PTR CALLBACK CD3DWindowFramework::DevOptionsProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	static CD3DTest *pTest = NULL;
	static HWND hCtrl;
	static const UINT nBuffer = 16;
	static TCHAR tcsBuffer[nBuffer];

	switch (nMsg) 
	{ 
	case WM_INITDIALOG:
		{	pTest = (CD3DTest*)lParam;
		hCtrl = GetDlgItem(hWnd, IDC_SELDEVICE);

		// Add the devices
		for(UINT i=0; i<pTest->m_uDeviceList; i++)
		{
			TCHAR *sTemp;

			if(i==0)
				sTemp = _T("SRC");
			else if(i==1)
				sTemp = _T("REF");
			else
			{
				StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("DEVICE%d"), i);
				sTemp = tcsBuffer;
			}
			SendMessage(hCtrl, CB_ADDSTRING, 0, (LPARAM)sTemp);
		}
		SendMessage(hCtrl, CB_SETCURSEL, 0, 0);

		// Add the adapters
		return TRUE; 
		}
	case WM_COMMAND: 
		switch (LOWORD(wParam)) 
		{ 
		case IDOK: 
			EndDialog(hWnd, TRUE);
			return TRUE; 
		case IDCANCEL: 
			EndDialog(hWnd, TRUE);
			return TRUE; 
		} 
	} 
	return FALSE; 
}

//-----------------------------------------------------------------------------

#pragma prefast( suppress:  6054 )
bool CD3DWindowFramework::GetDisplayString( UINT n, __out_ecount(nDisplayMaxPath) LPTSTR szDisplayString, UINT nDisplayMaxPath )
{
	//
	// Initialize locals.
	//

	DWORD nAdapter = 0;

	DISPLAY_DEVICE dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.cb = sizeof(dd);


	//
	// Initialize parameters.
	//

	ASSERT( szDisplayString != NULL );
	ASSERT( nDisplayMaxPath != 0 );
	ZeroMemory( szDisplayString, nDisplayMaxPath * sizeof( szDisplayString[ 0 ] ) );


	// Always make the first entry reflect the primary device.
	if ( n == 0 )
	{
		for
		(
			DWORD i = 0;
			EnumDisplayDevices
			(
				NULL,
				i,
				&dd,
			0
			) == TRUE;
			++i
		)
		{
			// Skip devices other than the primary.
			if ( (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0 )
			{
				StringCchCopy( szDisplayString, nDisplayMaxPath, dd.DeviceName );
				return true;
			};
		};

		WriteToLog(0, _T("CD3DWindowFramework::GetDisplayString could not locate primary device.") );
		return false;
	};


	// Now get the info for the attached secondary devices.
	nAdapter = 1;
	for
	(
		DWORD i = 0;
		EnumDisplayDevices
		(
			NULL,
			i,
			&dd,
			0
		) == TRUE;
		++i
	)
	{
		// Skip drivers that are not hardware devices.
		if ( (dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) != 0 )
		{
			continue;
		};

		// Skip the primary, since it's already accounted for.
		if ( (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) != 0 )
		{
			continue;
		};

		if ( n == nAdapter )
		{
			StringCchCopy( szDisplayString, nDisplayMaxPath, dd.DeviceName );
			return true;
		};

		++nAdapter;
	};


	WriteToLog(0, _T("CD3DWindowFramework::GetDisplayString could not locate device.") );

	return false;
};


// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::ChangeDeviceMode
(
	__in_opt LPCTSTR szDeviceName,
	__out_opt DEVMODE * pOriginalMode
)
{
	// Initialize locals.
	DEVMODE OriginalMode;
	ZeroMemory( &OriginalMode, sizeof( OriginalMode ) );
	OriginalMode.dmSize = sizeof( OriginalMode );

	DEVMODE NewMode;
	ZeroMemory( &NewMode, sizeof( NewMode ) );
	NewMode.dmSize = sizeof( NewMode );

	bool bResult = false;

	long ChangeDisplaySettingsResult = DISP_CHANGE_FAILED;

	// Get the device's current display mode.
	if
	(
		!EnumDisplaySettings
		(
			szDeviceName,
			ENUM_CURRENT_SETTINGS,
			&OriginalMode
		)
	)
	{
		DPF
		(
			1,
			TEXT("CD3DWindowFramework::ChangeDeviceMode:  EnumDisplaySettings on current mode failed.\n")
		);

		goto Cleanup;
	};


	DPF
	(
		1,
		TEXT("CD3DWindowFramework::ChangeDeviceMode:  Current mode %d x %d @ %d....\n"),
		OriginalMode.dmPelsWidth,
		OriginalMode.dmPelsHeight,
		OriginalMode.dmBitsPerPel
	);


	ZeroMemory( &NewMode, sizeof( NewMode ) );
	NewMode.dmSize = sizeof( NewMode );

	DWORD dwEnumFlags=0;
	//
	// Find a mode that differs from the current mode to momentarily switch into.  This will be enough to
	// trigger the device lost condition we are after.
	//
	do  // We'll try this loop twice, the first time without raw and rotated modes, the second time with.
	{
		// We need to find a mode that differs by more than just the bits-per-pixel, since 8 and 16bpp support is emulated.
		// If we were to choose a mode that only differed by bits-per-pixel, it would not cause a system wide mode change,
		// and would not trigger the DEVICELOST condition that this function is primarily used for.
		for
		(
			int iCurrentMode = 0;
			EnumDisplaySettingsEx
			(
				szDeviceName,
				iCurrentMode,
				&NewMode,
				dwEnumFlags
			);
			++iCurrentMode
		)
		{
			// If the potential mode's...
			if
			(
				(
					// ...resolution is different...
					NewMode.dmPelsWidth != OriginalMode.dmPelsWidth || 
					NewMode.dmPelsHeight != OriginalMode.dmPelsHeight
				) &&

				// ...and its bit depth is not 8 (which apparently caused leaks at some point)...
				NewMode.dmBitsPerPel > 8 

				// ...and its height is a multiple of 8 (which was apparently a requirement on Win9x)...
				//*************************************************************************************
				// Removing this restriction.  In the Windows 8 era (circa 2012), there are a number of 
				// LCD panels with heights or widths that are not a multiple of 8.  With Desktop 
				// rotation any dimension can become the height.
				///*************************************************************************************
				//&& NewMode.dmPelsHeight % 8 == 0

			)			
			{
				DPF
				(
					1,
					TEXT("CD3DWindowFramework::ChangeDeviceMode:  Calling ChangeDisplaySettingsEx to mode %d x %d @ %d....\n"),
					NewMode.dmPelsWidth,
					NewMode.dmPelsHeight,
					NewMode.dmBitsPerPel
				);

				// ...then try to change to the new mode.
				ChangeDisplaySettingsResult = ChangeDisplaySettingsEx
				(
					szDeviceName,
					&NewMode,
					NULL,
					0,
					NULL
				);

				if ( ChangeDisplaySettingsResult == DISP_CHANGE_SUCCESSFUL )
				{
					break;
				};

				DPF
				(
					1,
					TEXT("CD3DWindowFramework::ChangeDeviceMode:  ChangeDisplaySettingsEx to mode %d x %d @ %d failed.\n"),
					NewMode.dmPelsWidth,
					NewMode.dmPelsHeight,
					NewMode.dmBitsPerPel
				);
			}
		}; // End mode enumeration loop.

		//
		// On some environments, there is only one mode enumerated, the current desktop mode.
		// In such situations we can usually get at least a few additional modes by looking
		// for rotated versions of the desktop mode.   We'll try the above loop twice and include
		// rotated modes.  We'll throw in RAW modes as well, since we're desperate, and we don't 
		// really care if our monitor can't handle the mode.
		//
		if (ChangeDisplaySettingsResult != DISP_CHANGE_SUCCESSFUL)  // Even if we never called ChangeDisplaySettings in the above for loop, this is still set to a failure code from our earlier attempt.
		{
			if (dwEnumFlags  == 0) dwEnumFlags = EDS_ROTATEDMODE | EDS_RAWMODE;
			else
				break;
		}
	}
	while(ChangeDisplaySettingsResult != DISP_CHANGE_SUCCESSFUL); // We'll actually only try a maximum of two times.

	// If the mode could not be changed, then the system is in a bad state.
	if ( ChangeDisplaySettingsResult != DISP_CHANGE_SUCCESSFUL )
	{
		DPF
		(
			1,
			TEXT("CD3DWindowFramework::ChangeDeviceMode:  Failed to change modes.\n")
		);

		goto Cleanup;
	};

	DPF
	(
		1,
		TEXT("CD3DWindowFramework::ChangeDeviceMode:  mode change successfull.  Restoring orginal mode.\n")
	);

	// Restore the original mode.
	ChangeDisplaySettingsResult = ChangeDisplaySettingsEx
	(
		szDeviceName,
		&OriginalMode,
		NULL,
		0,
		NULL
	);

	if ( ChangeDisplaySettingsResult == DISP_CHANGE_SUCCESSFUL )
	{
		bResult = true;
		goto Cleanup;
	};


	// Wait, and retry, just in case this might have been a timing issue.
	Sleep( 2 * 1000 );
	ChangeDisplaySettingsResult = ChangeDisplaySettingsEx
	(
		szDeviceName,
		&OriginalMode,
		NULL,
		0,
		NULL
	);

	if ( ChangeDisplaySettingsResult == DISP_CHANGE_SUCCESSFUL )
	{
		bResult = true;
		goto Cleanup;
	};


	DPF
	(
		1,
		TEXT("CD3DWindowFramework::ChangeDeviceMode:  Failed to restore original display mode.\n")
	);


Cleanup:
	if ( pOriginalMode != NULL )
	{
		memcpy( pOriginalMode, &OriginalMode, sizeof( OriginalMode ) );
	};

	return bResult;
};


// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::LoseDeviceDefault(UINT uAdapterToLoseDevice)
{
	bool bResult = false;


	// For each device...
	DISPLAY_DEVICE CurrentDevice;
	ZeroMemory( &CurrentDevice, sizeof( CurrentDevice ) );
	CurrentDevice.cb = sizeof( CurrentDevice );
	for
	(
		int iCurrentDevice = 0;
		EnumDisplayDevices
		(
			NULL,
			iCurrentDevice,
			&CurrentDevice,
			0
		) == TRUE;
		++iCurrentDevice
	)
	{
		// ...if the device is connected to the desktop...
		if ( ( CurrentDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) != 0 )
		{
			DPF
			(
				1,
				"CD3DWindowFramework::LoseDeviceDefault:  Performing mode change on device %s.\n",
				CurrentDevice.DeviceName
			);

			// ...then attempt a mode change on it.
			ChangeDeviceMode
			(
				CurrentDevice.DeviceName,
				NULL
			);
		};
	};


	DPF
	(
		1,
		"CD3DWindowFramework::LoseDeviceDefault:  Done.\n"
	);

	bResult = true;

	//Cleanup:
	return bResult;
};


// --------------------------------------------------------------------------------------
bool CD3DWindowFramework::LoseDeviceByOtherProcess(UINT uAdapterToLose)
{
	// Initialize locals.
	PROCESS_INFORMATION ChildProcessInfo;
	ZeroMemory( &ChildProcessInfo, sizeof( ChildProcessInfo ) );

	STARTUPINFO ChildProcessStartupInfo;
	ZeroMemory( &ChildProcessStartupInfo, sizeof( ChildProcessStartupInfo ) );
	ChildProcessStartupInfo.cb = sizeof( ChildProcessStartupInfo );
	if ( m_hCustomDesktop != NULL )
	{
		ChildProcessStartupInfo.lpDesktop = m_pCustomDesktopName;
	};

	TCHAR ParentProcessName[ MAX_PATH * 2 ] = _T( "" );
	TCHAR ChildProcessCommandLine[ MAX_PATH ] = _T( "" );

	bool bResult = false;


	// Get the name of the running app (since this function is
	// just going to launch a second instance of it).
	if
	(
		!GetModuleFileName
		(
			NULL,
			ParentProcessName,
			ARRAYSIZE( ParentProcessName ) - 1
		)
	)
	{
		DPF
		(
			0,
			_T("CD3DWindowFramework::LoseDeviceByOtherProcess:  GetModuleFileName failed.\n")
		);

		goto Cleanup;
	};


	// Append the command-line switch to have the app trigger
	// device loss.
	StringCchPrintf
	(
		ChildProcessCommandLine,
		ARRAYSIZE( ChildProcessCommandLine ),
		"%s -LoseDevice:%u",
		ParentProcessName,
		uAdapterToLose
	);


	__try
	{
		// Start the child process. 
		if
		(
			!CreateProcess
			(
				NULL,
				ChildProcessCommandLine,
				NULL,
				NULL,
				FALSE,
				0,
				NULL,
				NULL,
				&ChildProcessStartupInfo,
				&ChildProcessInfo
			)
		)
		{
			DWORD CreateProcessError = GetLastError();

			DPF
			(
				0,
				_T("CD3DWindowFramework::LoseDeviceByOtherProcess:  CreateProcess failed with error %u.\n"),
				CreateProcessError
			);

			goto Cleanup;
		};


		// Pump messages as we wait for the child process
		// to exit, to make sure that we don't stall the
		// child process as it tries to change the display
		// mode (since changing modes sends sizing messages).
		DWORD WaitForChildProcessResult = WAIT_TIMEOUT;
		for
		(
			UINT iCurrentAttempt = 0;
			iCurrentAttempt < 300;
			++iCurrentAttempt
		)
		{
			WaitForChildProcessResult = WaitForSingleObject
			(
				ChildProcessInfo.hProcess,
				0
			);

			if ( WaitForChildProcessResult == WAIT_OBJECT_0 )
			{
				break;
			};

			Sleep( 100 );

			ProcessMessages();
		};

		// If the child process didn't complete in a timely manner...
		if ( WaitForChildProcessResult != WAIT_OBJECT_0 )
		{
			// ...then complain...
			DPF
			(
				0,
				_T( "CD3DWindowFramework::LoseDeviceByOtherProcess:  Wait on child process timed out.  Killing it.\n" )
			);

			// ...attempt to assert the debug privilege, which we may need...
			bool bEnableDebugPrivilegeResult = EnablePrivilege( SE_DEBUG_NAME );
			if ( bEnableDebugPrivilegeResult != true )
			{
				DPF
				(
					1,
					_T( "CD3DWindowFramework::LoseDeviceByOtherProcess:  Failed to enable SeDebugPrivilege.\n" )
				);
			};

			// ...and kill the process...
			BOOL bTerminateChildProcessResult = TerminateProcess
			(
				ChildProcessInfo.hProcess,
				666
			);

			// ...and if we couldn't even kill it...
			if ( bTerminateChildProcessResult != TRUE )
			{
				// ...then complain some more.
				DWORD TerminateProcessError = GetLastError();

				DPF
				(
					0,
					_T( "CD3DWindowFramework::LoseDeviceByOtherProcess:  TerminateProcess on child process failed with error %u.\n" ),
					TerminateProcessError
				);
			};

			goto Cleanup;
		};


		// Get the process return code.
		DWORD ChildProcessExitCode = 0;
		if
		(
			!GetExitCodeProcess
			(
				ChildProcessInfo.hProcess,
				&ChildProcessExitCode
			)
		)
		{
			DPF
			(
				0,
				_T("CD3DWindowFramework::LoseDeviceByOtherProcess:  GetExitCodeProcess failed.\n")
			);

			goto Cleanup;
		};


		// Validate the return code.
		if ( ChildProcessExitCode != EXITCODE_SUCCESS )
		{
			DPF
			(
				0,
				_T( "CD3DWindowFramework::LoseDeviceByOtherProcess:  Child process returned exit code 0x%x.\n" ),
				ChildProcessExitCode
			);

			goto Cleanup;
		};
	}
	__except
	(
		ExceptionFilter
		(
			GetExceptionCode(),
			GetExceptionInformation()
		)
	)
	{
		DPF
		(
			0,
			_T("CD3DWindowFramework::LoseDeviceByOtherProcess:  Exception encountered launching child process.\n")
		);

		goto Cleanup;
	};


	// If we made it this far, then we're good to go.
	bResult = true;


Cleanup:
	//
	// Clean up.
	//

	if ( ChildProcessInfo.hProcess != NULL )
	{
		CloseHandle( ChildProcessInfo.hProcess );
		ChildProcessInfo.hProcess = NULL;
	};

	if ( ChildProcessInfo.hThread != NULL )
	{
		CloseHandle( ChildProcessInfo.hThread );
		ChildProcessInfo.hThread = NULL;
	};


	return bResult;
};


// --------------------------------------------------------------------------------------
bool CD3DWindowFramework::LoseDeviceByTDR(UINT uAdapterToLoseDevice)
{
	HRESULT hr = S_OK;
	LPTSTR szLibrary = NULL;
	TCHAR szDeviceName[32];
	TCHAR szSystemDir[MAX_PATH];
	DWORD dwSize;
	bool bResult = false;
	D3DKMT_TDRDBGCTRL_ESCAPE escapeData = {};

	HDC					hdc = NULL;
	HINSTANCE			hInstThunk = NULL;
	D3DKMT_ESCAPE		sEsc;
	D3DKMT_HANDLE		hAdapter = NULL;
	D3DKMT_OPENADAPTERFROMHDC		OpenAdapterData;
	PFND3DKMT_OPENADAPTERFROMHDC	pfnKTOpenAdapterFromHdc = NULL;
	PFND3DKMT_ESCAPE	pfnKTEscape = NULL;

	// Get entry points needed
	dwSize = GetSystemDirectory(szSystemDir, ARRAYSIZE(szSystemDir));
	if(dwSize > ARRAYSIZE(szSystemDir))
	{
		DPF( 0, TEXT("CD3DWindowFramework::LoseDeviceByTDR:  System directory name too long.\n"));
		goto CLEANUP;
	}

	dwSize += (_tcslen(TEXT("gdi32.dll")) + 2);
	szLibrary = new TCHAR[dwSize];
	if(!szLibrary)
	{
		DPF( 0, TEXT("CD3DWindowFramework::LoseDeviceByTDR:  Out of memory.\n"));
		goto CLEANUP;
	}

	StringCchPrintf(szLibrary, dwSize, TEXT("%s\\%s"), szSystemDir, TEXT("gdi32.dll"));
	szLibrary[dwSize-1] = _T('\0');

	hInstThunk = LoadLibraryEx( szLibrary, NULL, 0 );
	if (hInstThunk == NULL)
	{
		DPF( 0, TEXT("CD3DWindowFramework::LoseDeviceByTDR:  Could not load thunk DLL.\n"));
		goto CLEANUP;
	}

	pfnKTOpenAdapterFromHdc = (PFND3DKMT_OPENADAPTERFROMHDC)GetProcAddress((HMODULE)hInstThunk, "D3DKMTOpenAdapterFromHdc" );
	if (pfnKTOpenAdapterFromHdc == NULL)
	{
		DPF( 0, TEXT("CD3DWindowFramework::LoseDeviceByTDR:  Could not find D3DKMTOpenAdapterFromHdc.\n"));
		goto CLEANUP;
	}

	pfnKTEscape = (PFND3DKMT_ESCAPE)GetProcAddress((HMODULE)hInstThunk, "D3DKMTEscape" );
	if (pfnKTEscape == NULL)
	{
		DPF( 0, TEXT("CD3DWindowFramework::LoseDeviceByTDR:  Could not find D3DKMTEscape.\n"));
		goto CLEANUP;
	}

	//Get adapter handle
	if( !GetDisplayString( uAdapterToLoseDevice, szDeviceName, 32 ) )
	{
		DPF( 0, TEXT("CD3DWindowFramework::LoseDeviceByTDR:  GetDisplayString failed.\n") );
		goto CLEANUP;
	}

	hdc = CreateDC (NULL, szDeviceName, NULL, NULL);
	if (hdc == NULL)
	{
		DPF( 0, TEXT("CD3DWindowFramework::LoseDeviceByTDR:  CreateDC failed.\n") );
		goto CLEANUP;
	}

	OpenAdapterData.hDc = hdc;
	if (! NT_SUCCESS((*pfnKTOpenAdapterFromHdc)(&OpenAdapterData)))
	{
		DPF( 0, TEXT("CD3DWindowFramework::LoseDeviceByTDR:  OpenAdapterFromHdc failed.\n") );
		goto CLEANUP;
	}
	hAdapter = OpenAdapterData.hAdapter;

	if (hr != S_OK)
	{
		DPF( 0, TEXT("CD3DWindowFramework::LoseDeviceByTDR:  Failed to get primary display adapter handle.\n"));
		goto CLEANUP;
	}

	escapeData.TdrControl = D3DKMT_TDRDBGCTRLTYPE_FORCETDR;

	sEsc.hAdapter = hAdapter;		
	sEsc.hDevice = (D3DKMT_HANDLE)NULL;
	sEsc.Type = D3DKMT_ESCAPE_TDRDBGCTRL;
	sEsc.pPrivateDriverData = &escapeData;
	sEsc.PrivateDriverDataSize = sizeof(escapeData);
	hr = pfnKTEscape(&sEsc);

	if (hr == S_OK)
	{
		bResult = true;
	}

CLEANUP:
	if (hdc != NULL)
		DeleteDC(hdc);

	if (!bResult)
	{
		DPF( 0, TEXT("CD3DWindowFramework::LoseDeviceByTDR:  Failed.\n") );
		//DebugBreak();
	}
	return bResult;
}


// --------------------------------------------------------------------------------------
bool CD3DWindowFramework::LoseDeviceByFUS()
{
	return false;
}


// --------------------------------------------------------------------------------------
bool CD3DWindowFramework::LoseDeviceByTerminalServer()
{
	return false;
}


//////////////////////////////////////////////////////////////////////
// Function:  SuspendSystem
//
// Purpose:
// Creates a waitable timer, puts the machine into standby or
// hibernate, and then waits for the timer to be signalled.
//
// Called by:
// CD3DTest::LoseTheDevice(D3DLOSEDEV_BYSTANDBY)
// CD3DTest::LoseTheDevice(D3DLOSEDEV_BYHIBERNATE)
//////////////////////////////////////////////////////////////////////
bool CD3DWindowFramework::SuspendSystem
(
	IN bool bHibernate
)
{
	//
	// Assume by default that we've failed until we determine otherwise.
	//

	bool bOverallResult = false;


	//
	// Create a waitable timer to wake us.
	//

	HANDLE hTimer = CreateWaitableTimer
	(
		NULL,
		FALSE,
		NULL
	);

	if
	(
		hTimer == INVALID_HANDLE_VALUE ||
		hTimer == NULL
	)
	{
		DPF
		(
			0,
			"CD3DWindowFramework::SuspendSystem:  Could not create timer.\n"
		);

		goto CLEANUP;
	};


	//
	// Wake the system in Interval number of seconds.
	//

	LONG Interval = 20;
	LARGE_INTEGER WakeTime;
	WakeTime.QuadPart = Interval * -10000000;


	//
	// Set the timer to wake the system.
	//

	BOOL bSetTimerResult = SetWaitableTimer
	(
		hTimer,
		&WakeTime,
		500, // half-second interval, in case we miss it
		NULL,
		NULL,
		TRUE
	);

	if ( bSetTimerResult != TRUE )
	{
		DPF
		(
			0,
			"CD3DWindowFramework::SuspendSystem:  Could not set waitable timer.\n"
		);

		goto CLEANUP;
	};


	//
	// Suspend the system.
	//

	BOOLEAN bForceCritical = 0;
	BOOLEAN bSuspendResult = SetSuspendState
	(
		(BOOLEAN) bHibernate,
		bForceCritical,
		0
	);

	if ( bSuspendResult == 0 )
	{
		DPF
		(
			0,
			"CD3DWindowFramework::SuspendSystem:  Could not suspend.\n"
		);

		goto CLEANUP;
	};


	//
	// Wait for the timer to be signalled or our timeout to elapse.
	//

	DWORD WaitResult = WaitForSingleObject
	(
		hTimer,
		Interval * 2 * 1000
	);

	if ( WaitResult != WAIT_OBJECT_0 )
	{
		DPF
		(
			0,
			"CD3DWindowFramework::SuspendSystem:  Wait for wake-up timer timed out.\n"
		);

		goto CLEANUP;
	};


	//
	// If we got this far, then we should be good to go.
	//

	bOverallResult = true;


CLEANUP:
	if
	(
		hTimer != INVALID_HANDLE_VALUE &&
		hTimer != NULL
	)
	{
		//
		// Cancel the timer.
		//

		CancelWaitableTimer( hTimer );


		//
		// Release our timer.
		//

		CloseHandle( hTimer );
		hTimer = NULL;
	};


	//
	// Reset the input-idle counters to make sure that the system
	// and monitor(s) come back on.
	//

	SetThreadExecutionState( ES_SYSTEM_REQUIRED );
	SetThreadExecutionState( ES_DISPLAY_REQUIRED );
	SetThreadExecutionState( ES_USER_PRESENT );


	if ( bOverallResult != true )
	{
		DPF( 0, TEXT("CD3DWindowFramework::SuspendSystem() failed.\n") );
		//DebugBreak();
	};


	return bOverallResult;
}


// --------------------------------------------------------------------------------------
HRESULT CD3DWindowFramework::ChangeDesktopMode
(
	DEVICEMODE * pMode
)
{
	//
	// Initialize locals.
	//

	HRESULT hr = S_OK;
	BOOL bRes = FALSE;
	UINT i = 0;
	UINT nAttached = 0;
	DWORD dwBPP = 0;
	TCHAR szFormat[64] = {0};

	DISPLAY_DEVICE DispDev;
	ZeroMemory( &DispDev, sizeof( DispDev ) );
	DispDev.cb = sizeof( DispDev );

	DEVMODE DevMode;
	ZeroMemory( &DevMode, sizeof( DevMode ) );
	DevMode.dmSize = sizeof( DevMode );


	//
	// Validate parameters.
	//

	ASSERT( NULL != pMode );
	if ( pMode == NULL )
	{
		DPF
		(
			1,
			_T("CD3DWindowFramework::ChangeDesktopMode:  pMode is NULL.\n")
		);

		hr = E_INVALIDARG;
		goto Exit;
	};

	if ( pMode->pDispMode == NULL )
	{
		DPF
		(
			1,
			_T("CD3DWindowFramework::ChangeDesktopMode:  pMode->pDispMode is NULL.\n")
		);

		hr = E_INVALIDARG;
		goto Exit;
	};


	if
	(
		pMode->pDispMode->dwWidth == 0 ||
		pMode->pDispMode->dwHeight == 0 ||
		pMode->pDispMode->dwBPP == 0
	)
	{
		DebugOut.Write
		(
			_T( "CD3DWindowFramework::ChangeDesktopMode:  Invalid mode requested:  %d x %d x %d.\n" ),
			pMode->pDispMode->dwWidth,
			pMode->pDispMode->dwHeight,
			pMode->pDispMode->dwBPP
		);

		hr = S_FALSE;
		goto Exit;
	};


	//
	// Validate members and state.
	//

	if ( m_pCurModes == NULL )
	{
		DPF
		(
			1,
			_T("CD3DWindowFramework::ChangeDesktopMode:  m_pCurModes is NULL.\n")
		);

		hr = E_FAIL;
		goto Exit;
	};


	//
	// Determine the bit depth for the requested mode.
	//

	dwBPP = GetBitCountForFormat( (FMT) pMode->pDispMode->Format.d3dfFormat );
	if ( dwBPP == 0 )
	{
		FmtToString
		(
			FORMAT_TEXTURE,
			&pMode->pDispMode->Format,
			szFormat
		);

		DPF
		(
			1,
			_T("CD3DWindowFramework::ChangeDesktopMode:  GetBitCountForFormat failed for format %s.\n"),
			szFormat
		);

		return E_FAIL;
	};


	//
	// Iterate over adapters, setting them to the requested mode.
	//

	int nActualAdapter = -1;
	int nSkipAdapter = 0;

	DispDev.cb = sizeof( DispDev );
	for
	(
		i = 0;
		EnumDisplayDevices
		(
			NULL,
			i,
			&DispDev,
			0
		);
		++i
	)
	{
		if ( ( DispDev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) != 0 )
		{
			// If the current adapter is already in the requested
			// mode...
			if
			(
				m_pCurModes[ nAttached ].Format.d3dfFormat == pMode->pDispMode->Format.d3dfFormat &&
				m_pCurModes[ nAttached ].dwWidth == pMode->pDispMode->dwWidth &&
				m_pCurModes[ nAttached ].dwHeight == pMode->pDispMode->dwHeight &&
				m_pCurModes[ nAttached ].dwRefreshRate == pMode->pDispMode->dwRefreshRate
			)
			{
				// ...then we don't need to adjust its mode.
				// Move on.

				DPF
				(
					3,
					_T( "CD3DWindowFramework::ChangeDesktopMode:  Adapter %u (%s) is already in target mode %u x %u x %u @ %u Hz.\n" ),
					nAttached,
					DispDev.DeviceName,
					DevMode.dmPelsWidth,
					DevMode.dmPelsHeight,
					DevMode.dmBitsPerPel,
					DevMode.dmDisplayFrequency
				);

				++nAttached;
				continue;
			};


			// Enumerate the adapter's modes in search of the
			// requested mode.
			ZeroMemory( &DevMode, sizeof( DevMode ) );
			DevMode.dmSize = sizeof( DevMode );
			for
			(
				UINT j = 0;
				bRes = EnumDisplaySettings
				(
					DispDev.DeviceName,
					j,
					&DevMode
				);
				++j
			)
			{
				if ( bRes != TRUE )
				{
					DPF
					(
						2,
						_T("CD3DWindowFramework::ChangeDesktopMode:  EnumDisplaySettings failed.\n")
					);

					break;
				};

				// If we've found the target mode in the list...
				if
				(
					DevMode.dmBitsPerPel == dwBPP &&
					DevMode.dmPelsWidth == pMode->pDispMode->dwWidth &&
					DevMode.dmPelsHeight == pMode->pDispMode->dwHeight &&
					DevMode.dmDisplayFrequency == pMode->pDispMode->dwRefreshRate 					
					// 
					// At one time we had an additional check to filter out
					// display modes whose height was not divisible by 8.
					// The reason for this check was not recorded, but it was
					// likely there to protect against limitations in early 
					// D3D hardware.   We are not aware of any modern HW with
					// this limitation, and display mode heights such as 1050
					// and 900 pixels are becoming common.   We want to support
					// and test these modes.
					//
					// Code removed on 1/22/2010
					//

				)
				{
					// ...then stop scanning.
					break;
				};
			};

			if ( bRes != TRUE )
			{
				// If we didn't find any match, then fail.
				WriteToLog
				(
					1,
					_T("CD3DWindowFramework::ChangeDesktopMode:  Failed to find requested mode (%u x %u x %u @ %u Hz).\n"), 
					pMode->pDispMode->dwWidth,
					pMode->pDispMode->dwHeight,
					dwBPP,
					pMode->pDispMode->dwRefreshRate
				);

				hr =  E_FAIL;
				goto Exit;
			}


			DPF
			(
				1,
				_T( "CD3DWindowFramework::ChangeDesktopMode:  Changing adapter %u (%s) to mode %u x %u x %u @ %u Hz.\n" ),
				nAttached,
				DispDev.DeviceName,
				DevMode.dmPelsWidth,
				DevMode.dmPelsHeight,
				DevMode.dmBitsPerPel,
				DevMode.dmDisplayFrequency
			);

			// Switch the adapter to the requested mode.
			LONG ChangeDisplayModeResult = ChangeDisplaySettingsEx
			(
				DispDev.DeviceName,
				&DevMode,
				NULL,
				CDS_UPDATEREGISTRY,
				NULL
			);

			if(DISP_CHANGE_SUCCESSFUL != ChangeDisplayModeResult)
			{
				WriteToLog
				(
					1,
					_T("CD3DWindowFramework::ChangeDesktopMode:  ChangeDisplaySettingsEx failed with error %d.\n"),
					ChangeDisplayModeResult
				);

				hr = E_FAIL;
				goto Exit;
			};


			// Note what resolution we settled on.
			//
			// N.B. [SEdmison]:
			// Now we're validating the mode against the refresh
			// rate before we select it, so we know that these
			// are already equal.
			//pMode->pDispMode->dwRefreshRate = DevMode.dmDisplayFrequency;


			// Note what bit depth we settled on.
			pMode->pDispMode->dwBPP = dwBPP;

			// Note what format we settled on.
			d3df2ddpf
			(
				pMode->pDispMode->Format.d3dfFormat,
				&pMode->pDispMode->Format.ddpfPixelFormat
			);


			// Make a note of this adapter's now-current mode.
			memcpy
			(
				&m_pCurModes[ nAttached ],
				pMode->pDispMode,
				sizeof( DISPLAYMODE )
			);

			// Note what format we settled on.
			d3df2ddpf
			(
				pMode->pDispMode->Format.d3dfFormat,
				&pMode->pDispMode->Format.ddpfPixelFormat
			);


			// Make a note of this adapter's now-current mode.
			memcpy
			(
				&m_pCurModes[ nAttached ],
				pMode->pDispMode,
				sizeof( DISPLAYMODE )
			);

			++nAttached;
		};
	};


	//
	// @BUGBUG [SEdmison]:
	// This delay is here in order to give the DWM a chance to
	// start up or shut down in response to the mode change.
	//
	// This should be replaced with a call to a DWM API in
	// order to know when they're done, rather than a blind
	// sleep.
	//

	Sleep( 3000 );


	//
	// Update D3D adapter information.
	//

	m_pD3D->UpdateAdapterInfo();


Exit:
	return hr;
};


// --------------------------------------------------------------------------------------

bool CD3DWindowFramework::OpenLogFile()
{
	//
	// Initialize locals.
	//

	TCHAR tcsBuffer[ MAX_PATH ] = _T( "" );


	//	
	// Get the first test group.
	//

	CD3DTest * pTest = (CD3DTest *) GetTest( 0 );
	if ( pTest == NULL )
	{
		DebugPrint(1, _T("CD3DWindowFramework::OpenLogFile:  No CD3DTest objects defined.\n"));
		return false;
	};


	//
	// Fill out the DXGLOG_INFO struct.
	//

	COPYSTRING(m_LogInfo.sExeName, GetExeName(false, tcsBuffer));
	if ( m_LogInfo.sExeName == NULL )
	{
		DebugPrint( 1, _T( "CD3DWindowFramework::OpenLogFile:  Failed to get test executable name.\n" ) );
		return false;
	};

	StringCchPrintf(tcsBuffer, ARRAYSIZE(tcsBuffer), _T("%s %s"), m_LogInfo.sExeName, m_CmdLine.GetParam());

	COPYSTRING(m_LogInfo.sCmdLine, tcsBuffer);
	if ( m_LogInfo.sCmdLine == NULL )
	{
		DebugPrint( 1, _T( "CD3DWindowFramework::OpenLogFile:  Failed to get test command line.\n" ) );
		return false;
	};

	COPYSTRING(m_LogInfo.sTestName, pTest->m_szLogName == NULL ? GetExeName(true, tcsBuffer) : pTest->m_szLogName);
	if ( m_LogInfo.sTestName == NULL )
	{
		DebugPrint( 1, _T( "CD3DWindowFramework::OpenLogFile:  Failed to get test name.\n" ) );
		return false;
	};

	m_LogInfo.hWnd = m_hWindow;
	m_LogInfo.TestType = pTest->m_Options.TestOptions.Type;

	m_DXGLog.pLogWindow = m_pLogWindow;


	//
	// Open the log.
	//

	HRESULT hOpenLogResult = m_DXGLog.Open( &m_LogInfo );
	if ( FAILED( hOpenLogResult ) )
	{
		DebugPrint
		(
			1,
			_T("CD3DWindowFramework::OpenLogFile:  Open on DXG logger failed with error 0x%x.\n"),
			hOpenLogResult
		);

		return false;
	};


	//
	// Create a log adapter.
	//

	HRESULT hCreateDXGLogAdapterResult = CDXGLog::CreateInstance
	(
		&m_DXGLog,
		IID_ISimpleLog,
		(void **) &m_pSimpleLog
	);

	if ( FAILED( hCreateDXGLogAdapterResult ) )
	{
		DebugPrint
		(
			1,
			_T("CD3DWindowFramework::OpenLogFile:  CreateInstance DXG log adapter failed with error 0x%x.\n"),
			hCreateDXGLogAdapterResult
		);

		return false;
	};


	return true;
}


///////////////////////////////////////////////////////
// Function:  ExceptionFilter
//
// Purpose:
// Determines which types of exceptions are eligible
// by handling in the application's __except block.
// Currently the only exception that will be caught is
// an access violation.
//
// Called by:
// __except block of various pieces of code.
//
// Returns:
// EXCEPTION_EXECUTE_HANDLER     Go to the body of the
//                                __except block.
// EXCEPTION_CONTINUE_EXECUTION  Ignore the exception
//                               and continue.
// EXCEPTION_CONTINUE_SEARCH     Bubble the exception
//                               up the call stack.
///////////////////////////////////////////////////////
int CD3DWindowFramework::ExceptionFilter
(
	unsigned int ExceptionCode,
	_EXCEPTION_POINTERS * pExceptionDetails
)
{
	switch ( ExceptionCode )
	{
	case EXCEPTION_ACCESS_VIOLATION:
		{
			//OutputDebugString( "Caught access violation.\n" );
			return EXCEPTION_EXECUTE_HANDLER;
		};

	default:
		{
			//OutputDebugString( "Caught other exception.\n" );
			return EXCEPTION_CONTINUE_SEARCH;
		};
	};
};


///////////////////////////////////////////////////////
// Function:  SwitchToDesktop
//
// Purpose:
// Creates and switches to a custom desktop.
///////////////////////////////////////////////////////
HRESULT CD3DWindowFramework::SwitchToDesktop
(
	__in_opt LPCTSTR pDesktopName
)
{
	HRESULT hOverallResult = E_FAIL;


	//
	// Determine the desktop with which our UI is currently associated.
	//

	m_hOriginalThreadDesktop = GetThreadDesktop
	(
		GetCurrentThreadId()
	);

	if ( m_hOriginalThreadDesktop == NULL )
	{
		DWORD GetOriginalThreadDesktopError = GetLastError();

		DPF
		(
			1,
			_T( "CD3DWindowFramework::SwitchToDesktop:  GetThreadDesktop failed with error %u.\n" ),
			GetOriginalThreadDesktopError
		);

		hOverallResult = HRESULT_FROM_WIN32( GetOriginalThreadDesktopError );
		goto Cleanup;
	};


	//
	// Spew the name of the original thread desktop.
	//

	DWORD nOriginalThreadDesktopName = 0;
	BOOL bGetOriginalThreadDesktopInformation = GetUserObjectInformation
	(
		(HANDLE) m_hOriginalThreadDesktop,
		UOI_NAME,
		m_pOriginalThreadDesktopName,
		sizeof( m_pOriginalThreadDesktopName ),
		&nOriginalThreadDesktopName
	);

	if ( bGetOriginalThreadDesktopInformation != TRUE )
	{
		DWORD GetUserObjectInformationError = GetLastError();

		DPF
		(
			1,
			_T( "CD3DWindowFramework::SwitchToDesktop:  GetUserObjectInformation failed with error %u.\n" ),
			GetUserObjectInformationError
		);

		hOverallResult = HRESULT_FROM_WIN32( GetUserObjectInformationError );
		goto Cleanup;
	};

	DPF
	(
		1,
		_T( "CD3DWindowFramework::SwitchToDesktop:  Original thread desktop:  0x%x (%s).\n" ),
		m_hOriginalThreadDesktop,
		m_pOriginalThreadDesktopName
	);


	//
	// Determine the currently active UI desktop.
	//

	m_hOriginalInputDesktop = OpenInputDesktop
	(
		0,
		FALSE,
		DESKTOP_SWITCHDESKTOP
	);

	if ( m_hOriginalInputDesktop == NULL )
	{
		DWORD GetOriginalInputDesktopError = GetLastError();

		DPF
		(
			1,
			_T( "CD3DWindowFramework::SwitchToDesktop:  OpenInputDesktop failed with error %u.\n" ),
			GetOriginalInputDesktopError
		);

		hOverallResult = HRESULT_FROM_WIN32( GetOriginalInputDesktopError );
		goto Cleanup;
	};


	//
	// Spew the name of the original input desktop.
	//

	DWORD nOriginalInputDesktopName = 0;
	BOOL bGetOriginalInputDesktopInformation = GetUserObjectInformation
	(
		(HANDLE) m_hOriginalInputDesktop,
		UOI_NAME,
		m_pOriginalInputDesktopName,
		sizeof( m_pOriginalInputDesktopName ),
		&nOriginalInputDesktopName
	);

	if ( bGetOriginalInputDesktopInformation != TRUE )
	{
		DWORD GetUserObjectInformationError = GetLastError();

		DPF
		(
			1,
			_T( "CD3DWindowFramework::SwitchToDesktop:  GetUserObjectInformation failed with error %u.\n" ),
			GetUserObjectInformationError
		);

		hOverallResult = HRESULT_FROM_WIN32( GetUserObjectInformationError );
		goto Cleanup;
	};

	DPF
	(
		1,
		_T( "CD3DWindowFramework::SwitchToDesktop:  Original input desktop:  0x%x (%s).\n" ),
		m_hOriginalInputDesktop,
		m_pOriginalInputDesktopName
	);


	//
	// Check whether we're already on the right desktop.
	//

	if
	(
		_tcsicmp( m_pCustomDesktopName, m_pOriginalThreadDesktopName ) == 0 &&
		_tcsicmp( m_pCustomDesktopName, m_pOriginalInputDesktopName ) == 0
	)
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::SwitchToDesktop:  We're already on the correct desktop (%s).\n" ),
			m_pCustomDesktopName
		);

		hOverallResult = S_FALSE;
		goto Cleanup;
	};


	//
	// Create or open the desktop to which we plan to switch.
	//

	if ( pDesktopName == NULL )
	{
		StringCchCopy
		(
			m_pCustomDesktopName,
			ARRAYSIZE( m_pCustomDesktopName ),
			_T( "D3D9FrameworkDesktop" )
		);
	}
	else
	{
		StringCchCopy
		(
			m_pCustomDesktopName,
			ARRAYSIZE( m_pCustomDesktopName ),
			pDesktopName
		);
	};


	//
	// Attempt to create the specified desktop.
	//

	DPF
	(
		1,
		_T( "CD3DWindowFramework::SwitchToDesktop:  Creating desktop %s.\n" ),
		m_pCustomDesktopName
	);

	m_hCustomDesktop = CreateDesktop
	(
		m_pCustomDesktopName,
		NULL,
		NULL,
		0,
		GENERIC_ALL,
		NULL
	);

	// If we couldn't create it...
	if ( m_hCustomDesktop == NULL )
	{
		// ...then note the failure...
		DWORD CreateDesktopError = GetLastError();
		DPF
		(
			1,
			_T( "CD3DWindowFramework::SwitchToDesktop:  CreateDesktop failed with error %u.\n" ),
			CreateDesktopError
		);


		// ...and attempt to open it instead...
		DPF
		(
			1,
			_T( "CD3DWindowFramework::SwitchToDesktop:  Opening desktop %s.\n" ),
			m_pCustomDesktopName
		);

		m_hCustomDesktop = OpenDesktop
		(
			m_pCustomDesktopName,
			0,
			FALSE,
			GENERIC_ALL
		);

		// ...and if we can't open it either...
		if ( m_hCustomDesktop == NULL )
		{
			// ...then we have a problem.
			DWORD OpenDesktopError = GetLastError();

			DPF
			(
				1,
				_T( "CD3DWindowFramework::SwitchToDesktop:  OpenDesktop on desktop %s failed with error %u.\n" ),
				m_pCustomDesktopName,
				OpenDesktopError
			);

			hOverallResult = HRESULT_FROM_WIN32( OpenDesktopError );
			goto Cleanup;
		};
	};


	//
	// Associate our UI with the new desktop.
	//

	DPF
	(
		1,
		_T( "CD3DWindowFramework::SwitchToDesktop:  Setting thread desktop to 0x%x.\n" ),
		m_hCustomDesktop
	);

	BOOL bSetThreadDesktopResult = SetThreadDesktop( m_hCustomDesktop );

	if ( bSetThreadDesktopResult != TRUE )
	{
		DWORD SetThreadDesktopError = GetLastError();

		DPF
		(
			1,
			_T( "CD3DWindowFramework::SwitchToDesktop:  SetThreadDesktop failed with error %u.\n" ),
			SetThreadDesktopError
		);

		hOverallResult = HRESULT_FROM_WIN32( SetThreadDesktopError );
		goto Cleanup;
	};


	//
	// Make the new desktop active.
	//

	DPF
	(
		1,
		_T( "CD3DWindowFramework::SwitchToDesktop:  Switching to desktop to 0x%x.\n" ),
		m_hCustomDesktop
	);

	BOOL bSwitchDesktopResult = SwitchDesktop( m_hCustomDesktop );

	if ( bSwitchDesktopResult != TRUE )
	{
		DWORD SwitchDesktopError = GetLastError();

		DPF
		(
			1,
			_T( "CD3DWindowFramework::SwitchToDesktop:  SwitchDesktop failed with error %u.\n" ),
			SwitchDesktopError
		);


		SetThreadDesktop( m_hOriginalThreadDesktop );

		hOverallResult = HRESULT_FROM_WIN32( SwitchDesktopError );
		goto Cleanup;
	};


	//
	// If we made it this far, then we're good to go.
	//

	hOverallResult = S_OK;


Cleanup:
	if ( hOverallResult != S_OK )
	{
		m_hOriginalThreadDesktop = NULL;
		m_hOriginalInputDesktop = NULL;

		if ( m_hCustomDesktop != NULL )
		{
			CloseDesktop( m_hCustomDesktop );
			m_hCustomDesktop = NULL;
		};
	};

	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  RevertToOriginalDesktop
//
// Purpose:
// Switches back to the original desktop.
///////////////////////////////////////////////////////
HRESULT CD3DWindowFramework::RevertToOriginalDesktop()
{
	HRESULT hOverallResult = S_OK;


	// If we never switched desktops to begin with...
	if ( m_hCustomDesktop == NULL )
	{
		// ...then there is nothing to do here.
		ASSERT( m_hOriginalThreadDesktop == NULL );
		ASSERT( m_hOriginalInputDesktop == NULL );

		hOverallResult = S_FALSE;
		goto Cleanup;
	};


	//
	// Restore the original thread desktop.
	//

	BOOL bSetThreadDesktopResult = FALSE;
	DWORD SetThreadDesktopError = ERROR_SUCCESS;
	if ( m_hOriginalThreadDesktop != NULL )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::RevertToOriginalDesktop:  Setting thread desktop to original thread desktop 0x%x.\n" ),
			m_hOriginalThreadDesktop
		);

		bSetThreadDesktopResult = SetThreadDesktop( m_hOriginalThreadDesktop );

		if ( bSetThreadDesktopResult != TRUE )
		{
			SetThreadDesktopError = GetLastError();

			DPF
			(
				1,
				_T( "CD3DWindowFramework::RevertToOriginalDesktop:  SetThreadDesktop failed with error %u.\n" ),
				SetThreadDesktopError
			);
		};
	};

	if
	(
		m_hOriginalThreadDesktop == NULL ||
		bSetThreadDesktopResult != TRUE
	)
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::RevertToOriginalDesktop:  Opening Default desktop.\n" )
		);

		HDESK hThreadDesktopToRestore = OpenDesktop
		(
			_T( "Default" ),
			0,
			FALSE,
			GENERIC_READ | GENERIC_EXECUTE
		);

		if ( hThreadDesktopToRestore == NULL )
		{
			DWORD OpenDesktopError = GetLastError();

			DPF
			(
				1,
				_T( "CD3DWindowFramework::RevertToOriginalDesktop:  OpenDesktop on Default desktop failed with error %u.\n" ),
				OpenDesktopError
			);

			hOverallResult = HRESULT_FROM_WIN32( OpenDesktopError );
		}
		else
		{
			DPF
			(
				1,
				_T( "CD3DWindowFramework::RevertToOriginalDesktop:  Setting thread desktop to Default.\n" )
			);

			BOOL bSetThreadDesktopResult = SetThreadDesktop( hThreadDesktopToRestore );

			if ( bSetThreadDesktopResult != TRUE )
			{
				DWORD SetThreadDesktopError = GetLastError();

				DPF
				(
					1,
					_T( "SetThreadDesktop on Default desktop failed with error %u.\n" ),
					SetThreadDesktopError
				);

				hOverallResult = HRESULT_FROM_WIN32( SetThreadDesktopError );
			};

			CloseDesktop( hThreadDesktopToRestore );
			hThreadDesktopToRestore = NULL;
		};
	};


	//
	// Restore the original input desktop.
	//

	BOOL bSwitchDesktopResult = FALSE;
	DWORD SwitchDesktopError = ERROR_SUCCESS;
	if ( m_hOriginalInputDesktop != NULL )
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::RevertToOriginalDesktop:  Switching to original input desktop 0x%x.\n" ),
			m_hOriginalInputDesktop
		);

		bSwitchDesktopResult = SwitchDesktop( m_hOriginalInputDesktop );

		if ( bSwitchDesktopResult != TRUE )
		{
			SwitchDesktopError = GetLastError();

			DPF
			(
				1,
				_T( "CD3DWindowFramework::RevertToOriginalDesktop:  SwitchDesktop failed with error %u.\n" ),
				SwitchDesktopError
			);
		};
	};

	if
	(
		m_hOriginalInputDesktop == NULL ||
		bSwitchDesktopResult != TRUE
	)
	{
		DPF
		(
			1,
			_T( "CD3DWindowFramework::RevertToOriginalDesktop:  Opening Default desktop.\n" )
		);

		HDESK hInputDesktopToRestore = OpenDesktop
		(
			_T( "Default" ),
			0,
			FALSE,
			GENERIC_READ | GENERIC_EXECUTE
		);

		if ( hInputDesktopToRestore == NULL )
		{
			DWORD OpenDesktopError = GetLastError();

			DPF
			(
				1,
				_T( "CD3DWindowFramework::RevertToOriginalDesktop:  OpenDesktop on Default desktop failed with error %u.\n" ),
				OpenDesktopError
			);

			hOverallResult = HRESULT_FROM_WIN32( OpenDesktopError );
		}
		else
		{
			BOOL bSwitchDesktopResult = SwitchDesktop( hInputDesktopToRestore );

			if ( bSwitchDesktopResult != TRUE )
			{
				DWORD SwitchDesktopError = GetLastError();

				DPF
				(
					1,
					_T( "CD3DWindowFramework::RevertToOriginalDesktop:  SwitchDesktop on Default desktop failed with error %u.\n" ),
					SwitchDesktopError
				);

				hOverallResult = HRESULT_FROM_WIN32( SwitchDesktopError );
			};

			CloseDesktop( hInputDesktopToRestore );
			hInputDesktopToRestore = NULL;
		};
	};


Cleanup:
	return hOverallResult;
};
