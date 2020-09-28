//////////////////////////////////////////////////////////////////////////
//  Framework.cpp
//  created:	2004/09/22
//
//  purpose: Implements the framework core.
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#include "pch.h"

#include "CommandLine.h"
#include "Framework.h"
#include "sfidx.h"
#include <wttsdk.h>

#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>

#include <initguid.h>
#include <ISimpleLogController.h>


/////////////////////////////////////////////////////////////////
//
// Pragmas
//
/////////////////////////////////////////////////////////////////

#pragma warning(push)
#pragma warning(disable:4068)
#pragma prefast(disable: 321, "We want relative path when loading DLLs for testing purposes.") 
#pragma warning(pop)


/////////////////////////////////////////////////////////////////
//
// Macros
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// String manipulation macros
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Macro:  CONCATENATE
//
// Purpose:
// Concatenates two tokens into a single token.
///////////////////////////////////////////////////////
#if !defined(CONCATENATE2)
#define CONCATENATE2(x, y)  x ## y
#endif

#if !defined(CONCATENATE)
#define CONCATENATE(x, y)   CONCATENATE2(x, y)
#endif


///////////////////////////////////////////////////////
// Macro:  TEXTIFY
//
// Purpose:
// Converts a sequence of characters into a string.
///////////////////////////////////////////////////////
#if !defined(TEXTIFY)
#define TEXTIFY(x) #x
#endif


///////////////////////////////////////////////////////
// Macro:  UNICODIFY
//
// Purpose:
// Converts a string into a wide-character string by
// prepending it with L.
//
// Example:
// UNICODIFY("Foo") produces L"Foo"
///////////////////////////////////////////////////////
#if !defined(UNICODIFY)
#define UNICODIFY(x) CONCATENATE(L, x)
#endif


///////////////////////////////////////////////////////
// Macro:  __FILEW__
//
// Purpose:
// Unicode version of the __FILE__ macro.
///////////////////////////////////////////////////////
#if !defined(__FILEW__)
#define __FILEW__ UNICODIFY(__FILE__)
#endif


#define WORK_BUFFER_SIZE  1024

CWGFTestFramework::CWGFExceptionTrace CWGFTestFramework::s_exceptionTrace;

/////////////////////////////////////////////////////////////////
//
// Helper functions
//
/////////////////////////////////////////////////////////////////

DWORD GetWTTResultTypeForFrameworkResultType
(
	RESULT_CODE ResultCode
)
{
	switch ( ResultCode )
	{
		case RESULT_PASS:
		{
			return WTT_TESTCASE_RESULT_PASS;
		} break;

		case RESULT_FAIL:
		{
			return WTT_TESTCASE_RESULT_FAIL;
		} break;

		case RESULT_SKIP:
		{
			return WTT_TESTCASE_RESULT_SKIPPED;
		} break;

		case RESULT_ABORT:
		case RESULT_BLOCKED:
		{
			return WTT_TESTCASE_RESULT_BLOCKED;
		} break;

		case RESULT_PENDING:
		{
			return WTT_TESTCASE_RESULT_BLOCKED;
		} break;

		// This function shouldn't ever be called with RETRY
		case RESULT_RETRY:
		default:
		{
			assert( FALSE && "Unhandled result code." );
			return WTT_TESTCASE_RESULT_FAIL;
		} break;
	};
};


HRESULT PulseScreensaverTimeout()
{
	//
	// Get the screensaver timeout.
	//

	UINT ScreensaverTimeout = 0;
	BOOL bGetScreensaverTimeoutResult = SystemParametersInfo
	(
		SPI_GETSCREENSAVETIMEOUT,
		0,
		&ScreensaverTimeout,
		0
	);

	if ( bGetScreensaverTimeoutResult != TRUE )
	{
		DWORD GetScreensaverTimeoutError = GetLastError();

		if ( GetScreensaverTimeoutError == ERROR_SUCCESS )
		{
			return E_FAIL;
		};

		return HRESULT_FROM_WIN32( GetScreensaverTimeoutError );
	};


	//
	// Set a default if needed.
	//

	if ( ScreensaverTimeout == 0 )
	{
		// Default to 10 minutes.
		ScreensaverTimeout = 10 * 60;
	};


	//
	// Set the screensaver timeout to the same value.
	// This has the effect of resetting the timer.
	//

	BOOL bSetScreensaverTimeoutResult = SystemParametersInfo
	(
		SPI_SETSCREENSAVETIMEOUT,
		ScreensaverTimeout,
		NULL,
		0
	);

	if ( bSetScreensaverTimeoutResult == TRUE )
	{
		return S_OK;
	};


	return HRESULT_FROM_WIN32( GetLastError() );
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


CWGFTestFramework *CWGFTestFramework::m_pThis = NULL;

CWGFTestFramework * WGFTESTAPI GetFramework()
{
	CWGFTestFramework *pFramework = CWGFTestFramework::Instance();
	
	if ( !pFramework )
	{
		// BUGBUG - TODO: Allocate a framework instance.
	}
	
	return pFramework;
}


CWGFTestFramework::CWGFTestFramework() : 
	m_pLogWrapper( NULL ),
	m_pShell( NULL ), 
	m_bMuteLog( false ),	
	m_bNoLog( false ),
	m_bCoverage( false ),
	m_bShowDesktopAtStartup( false ),
	m_bAllowGhosting( false ),
	m_bAllowIdleDetection( false ),
	m_bHelp( false ),
	m_bLogClean( false ),
	m_bTextLog( false ),
	m_bLogMessages( false ),
	m_bLogToWindow( false ),
	m_bLogToWindowCached( false ),
	m_bLogToDebugger( false ),
	m_bNoCache( false ),
	m_bDBForceLog( false ),
	m_bDBKeepLocalLog( false ),
	m_LastTestCount( 0 ),
	m_RandPoolTemp( 0.0f ),
	m_priorityLevel( -1 ),
	m_priorityProfile( _T("") ),
	m_bSession0( false ),
	m_Retries( 0 ),
	m_RetryCurrent( 0 ),
	m_bInsideRetrySection( false ),
	m_bGenerateCSVOnly(false),
	m_pCSVFile(NULL),
	m_variationCoreXmlBuilder(NULL),

	m_TestMode( WGFTEST_MODE_UNDEFINED ),

	m_bDoNotCatchExceptions( false ),
	m_breakOnAllocID( 0 ),

	m_MaxGroupPasses( DEFAULT_MAX_GROUP_PASSES ),
	m_MaxTotalPasses( DEFAULT_MAX_TOTAL_PASSES ),
	m_MaxGroupSkips( DEFAULT_MAX_GROUP_SKIPS ),
	m_MaxTotalSkips( DEFAULT_MAX_TOTAL_SKIPS ),
	m_MaxGroupFailures( DEFAULT_MAX_GROUP_FAILURES ),
	m_MaxTotalFailures( DEFAULT_MAX_TOTAL_FAILURES ),
	m_MaxGroupAborts( DEFAULT_MAX_GROUP_ABORTS ),
	m_MaxTotalAborts( DEFAULT_MAX_TOTAL_ABORTS ),
	m_MaxGroupWarnings( DEFAULT_MAX_GROUP_WARNINGS ),
	m_MaxTotalWarnings( DEFAULT_MAX_TOTAL_WARNINGS ),

	m_fRandPercent( 1.0f ),
	m_uSeed( 1 ),

	m_bLockOptionVariables( true ),
	m_pTest( NULL ),
	m_bIsNewGroup( false ),
	m_LastResult( RESULT_PENDING ),
	m_fLastTestConformance( -1.f ),
	m_Status( WGFTEST_STATUS_NOT_FOUND ),

	m_bTestCaseInitialized( false ),

	m_nPassCount( 0 ),
	m_nSkipCount( 0 ),
	m_nFailCount( 0 ),
	m_nAbortCount( 0 ),

	m_hCovDLL( NULL ),
	CoverageInitialize( NULL ),
	CoverageReset( NULL ),
	CoverageSave( NULL ),
	CoverageUninitialize( NULL ),

	m_hMSCTF( NULL ),
	m_dwLangBarState( 0 ),

	m_dpfLevel( 1 )
{
	m_plit = m_pl.End();
	memset( &m_LogInfo, 0, sizeof( DXGLOG_INFO ) );
	memset( &m_CurrentDeviceInfo, 0, sizeof( DXGLOG_DEVICEINFO ) );

	m_pThis = this;

	m_DXGLog.SetDebugOutLevel( 1 );
}


CWGFTestFramework::~CWGFTestFramework()
{
#if !defined(_ARM_) && !defined(_ARM64_)
	if( m_sfi.IsInitialized() )
	{
		m_sfi.UnInit();
	}
#endif

	SAFEDELETE(m_variationCoreXmlBuilder);

	// Release the log info strings.
	SAFEDELETEA(m_LogInfo.sTestName);
	SAFEDELETEA(m_LogInfo.sExeName);
	SAFEDELETEA(m_LogInfo.sCmdLine);
	SAFEDELETEA(m_LogInfo.sXMLFileNameBase);
	SAFEDELETEA(m_LogInfo.sXMLFileExtension);
	SAFEDELETEA(m_LogInfo.sStyleSheet);
	SAFEDELETEA(m_LogInfo.sPlainFileNameBase);
	SAFEDELETEA(m_LogInfo.sPlainFileExtension);
	SAFEDELETEA(m_LogInfo.sLocalDirectory);
	SAFEDELETEA(m_LogInfo.sDBDirectory);
	SAFEDELETEA(m_LogInfo.sSourceFile);
	SAFEDELETEA(m_LogInfo.sDestinationFile);
	SAFEDELETEA(m_LogInfo.sCacheDirectory);
	SAFEDELETEA(m_LogInfo.sDBDirectoryPath);
	SAFEDELETEA(m_LogInfo.sTagName);
	SAFEDELETEA(m_LogInfo.sFriendlyChipsetName);
	SAFEDELETEA(m_LogInfo.sTypeName);

	// Release the device info strings.
	SAFEDELETEA(m_CurrentDeviceInfo.sDisplay);
	SAFEDELETEA(m_CurrentDeviceInfo.sDriver);
	SAFEDELETEA(m_CurrentDeviceInfo.sDriverVersion);
	SAFEDELETEA(m_CurrentDeviceInfo.sPnPID);
	SAFEDELETEA(m_CurrentDeviceInfo.sResolution);
	SAFEDELETEA(m_CurrentDeviceInfo.sDevice);

	if ( m_pLogWrapper != NULL )
	{
		m_pLogWrapper->Release();
		m_pLogWrapper = NULL;
	}
}


bool CWGFTestFramework::IsStatusContinuable()
{
	switch ( m_Status )
	{
		case WGFTEST_STATUS_ABORT_TEST_USERINITIATED:
		case WGFTEST_STATUS_EXCEPTION_CAUGHT:
		{
			return false;
		} break;


		default:
		{
			return true;
		} break;
	};
};


bool CWGFTestFramework::GetWTTDimensionValues(
	__in_ecount(numDimensions) const tstring * pStrDimensionNameList,
	__out_ecount(numDimensions) tstring * pStrDimensionValueList,
	UINT numDimensions)
{
	//
	// Initialize locals.
	//

	bool bResult = true; 

	class IWTTDiag;

	// WTTSDKInitialize
	typedef HRESULT (*PF_WTTSDKInitialize)(
		IWTTDiag *pDiag
		);
	PF_WTTSDKInitialize pWTTSDKInitialize = nullptr;

	typedef void (*PF_WTTSDKCleanup)();
	PF_WTTSDKCleanup pWTTSDKCleanup = nullptr;

	typedef void (*PF_WTTFreeMemory)( 
		LPVOID pMemory
		);
	PF_WTTFreeMemory pWTTFreeMemory = nullptr;

	typedef LPWSTR (*PF_GetParameterValue)
	(
		LPCWSTR pcszParamName, 
		LPWSTR *ppszErrorXML
	);
	PF_GetParameterValue pGetParameterValue = nullptr;


	//
	// Load WTTSDK.dll.
	//

	HMODULE hWttSDKDll = LoadLibraryExW(
		L"WTTSDK.dll",
		NULL,
		0);

	if ( hWttSDKDll == NULL )
	{
		bResult = false;
		goto Cleanup;
	}


	//
	// Get entry points.
	//

	pWTTSDKInitialize = reinterpret_cast< PF_WTTSDKInitialize >(
		GetProcAddress(
			hWttSDKDll,
			"WTTSDKInitialize"));

	if ( pWTTSDKInitialize == nullptr )
	{
		bResult = false;
		goto Cleanup;
	}


	pWTTSDKCleanup = reinterpret_cast< PF_WTTSDKCleanup >(
		GetProcAddress(
			hWttSDKDll,
			"WTTSDKCleanup"));

	if ( pWTTSDKCleanup == nullptr )
	{
		bResult = false;
		goto Cleanup;
	}


	pGetParameterValue = reinterpret_cast< PF_GetParameterValue >(
		GetProcAddress(
			hWttSDKDll,
			"GetParameterValue"));

	if ( pGetParameterValue == nullptr )
	{
		bResult = false;
		goto Cleanup;
	}


	pWTTFreeMemory = reinterpret_cast< PF_WTTFreeMemory >(
		GetProcAddress(
			hWttSDKDll,
			"WTTFreeMemory"));

	if ( pWTTFreeMemory == nullptr )
	{
		bResult = false;
		goto Cleanup;
	}


	//
	// Call WTTSDKInitialize.
	//

	HRESULT hInitializeWTTSDKResult = (*pWTTSDKInitialize)(
		nullptr);

	if ( FAILED( hInitializeWTTSDKResult ) )
	{
		bResult = false;
		goto Cleanup;
	}


	//
	// Retrieve dimension values.
	//

	for ( UINT i = 0; i < numDimensions; i++ )
	{
		tstring tstrDimensionName = pStrDimensionNameList[i];
		const wstring wstrDimensionName = tstr2wstr( tstrDimensionName );
		LPCWSTR lpcwDimensionName = wstrDimensionName.c_str();
		LPWSTR lpwDimensionValue = NULL;
		LPWSTR lpwErrorXML = NULL;

		lpwDimensionValue = (*pGetParameterValue)(
			lpcwDimensionName,
			&lpwErrorXML);

		if ( !lpwDimensionValue )
		{
			WriteToLogF(_T("GetWTTDimensionValues(): GetParameterValue() failed for %s."), tstrDimensionName.c_str() );
			bResult = false;
		}
		else
		{
			wstring wstrDimensionValue( lpwDimensionValue );
			tstring tstrDimensionValue = wstr2tstr( wstrDimensionValue );
			pStrDimensionValueList[i].assign (tstrDimensionValue.c_str()); 
		}

		if (lpwErrorXML && pWTTFreeMemory)
		{
			(*pWTTFreeMemory)( lpwErrorXML );
			lpwErrorXML = NULL;
		}

		if (lpwDimensionValue && pWTTFreeMemory)
		{
			(*pWTTFreeMemory)( lpwDimensionValue );
			lpcwDimensionName = NULL;
		}

		if ( bResult == false )
		{
			break;
		}
	}


Cleanup:
	//
	// Call WTTSDKCleanup.
	//

	if ( pWTTSDKCleanup != nullptr )
	{
		(*pWTTSDKCleanup)();
		pWTTSDKCleanup = nullptr;
	}


	//
	// Unload WTTSDK.dll.
	//

	if ( hWttSDKDll != nullptr )
	{
		FreeLibrary( hWttSDKDll );
		hWttSDKDll = nullptr;
	}

	return bResult;
}


void CWGFTestFramework::SetSymbolPath()
{
	// An environment variable has a max size limit of 32767 characters, including the null-terminating character.
	const size_t pathLen = SHRT_MAX; 
	
	tstring tstrDimensionNameList[] = {
		_T("WTT\\OSBinRoot"),
		_T("WTT\\TestBinRoot"),
		_T("WTT\\WoW64OsBinRoot"),
		_T("WTT\\WoW64TestBinRoot"),
	};
	tstring tstrDimensionValueList[4];

	UINT numDimensions = ARRAYSIZE(tstrDimensionNameList);

	bool bDimensionValueResult = GetWTTDimensionValues(
		tstrDimensionNameList,
		tstrDimensionValueList,
		numDimensions);

	if ( !bDimensionValueResult )
	{
		return;
	}

	// removing the ending "\bin"
	tstring tstrSubBin( _T("\\bin") );
	for (UINT i = 0; i < 4; i++)
	{
		tstring::size_type index = tstrDimensionValueList[i].find(tstrSubBin);
		if ( index == string::npos )
		{
			return;
		}

		tstrDimensionValueList[i].erase(index);
	}

	// adding the symbol dir to the end
	tstring tstrOSSymDir( _T("\\symbols.pri\\retail") );
	tstring tstrTestSymDir( _T("\\symbols.pri\\NTTEST\\WINDOWSTEST\\Graphics") );
	tstrDimensionValueList[0].append( tstrOSSymDir );		// OSBinRoot
	tstrDimensionValueList[1].append( tstrTestSymDir );		// TestBinRoot
	tstrDimensionValueList[2].append( tstrOSSymDir );		// WoW64OsBinRoot
	tstrDimensionValueList[3].append( tstrTestSymDir );		// WoW64TestBinRoot

	tstring tstrFinalSymPath;
	tstrFinalSymPath.append(tstrDimensionValueList[0]);
	tstrFinalSymPath.append(_T(";"));
	tstrFinalSymPath.append(tstrDimensionValueList[1]);
	tstrFinalSymPath.append(_T(";"));
	tstrFinalSymPath.append(tstrDimensionValueList[2]);
	tstrFinalSymPath.append(_T(";"));
	tstrFinalSymPath.append(tstrDimensionValueList[3]);

	// set symbol path string to _NT_SYMBOL_PATH Environment Variable
	TCHAR envSymPath[pathLen];
	TCHAR* pEnvName = _T("_NT_SYMBOL_PATH");
	size_t envSize = GetEnvironmentVariable(pEnvName, envSymPath, pathLen); 
	tstring tstrEnvSymPath( envSymPath );
	tstrEnvSymPath.append( tstrFinalSymPath );
	if ( !SetEnvironmentVariable( pEnvName, tstrEnvSymPath.c_str() ) )
	{
	}

/*
	// for debugging use
	TCHAR dbTmpEV[pathLen];
	TCHAR dbPrintEV[pathLen + 128];
	size_t dbSize = GetEnvironmentVariable(pEnvName, dbTmpEV, pathLen); 
	_stprintf_s(dbPrintEV, pathLen + 128, _T("%s: size:%d: %s"), pEnvName, dbSize, dbTmpEV );
	OutputDebugString(dbPrintEV);
*/
}


int CWGFTestFramework::Execute( CShellBase *pShell )
{
	//
	// Initialize locals.
	//

	int OverallResult = 1;


	//
	// Initialize members.
	//

	m_pShell = pShell;
	
	if ( !Initialize() )
	{
		return OverallResult;
	}


	//
	// Set _NT_ALTERNATE_SYMBOL_PATH environment variable.
	//
	SetSymbolPath();


	//
	// Enter the main message loop.
	//

	for( ; ; )
	{	
		if ( !Update() )
		{
			// If exiting due to a user interruption, force an abort to be logged.
			if
			(
				m_Status == WGFTEST_STATUS_ABORT_TEST_USERINITIATED &&
				m_bNoLog == false
			)
			{
				if (!m_DXGLog.InTestCase())
				{
					m_DXGLog.BeginTestCase( _T( "User aborted test." ), 0xFFFFFFFF );
				}

				m_DXGLog.Trace(1, _T("***  Test aborted by user.\n"));
				m_DXGLog.AbortTest();
				m_DXGLog.EndTestCase();
			}

			break;
		}
	}


	//
	// Clean up.
	//

	Uninitialize();


	OverallResult = 0;

	
	return OverallResult;
}


///////////////////////////////////////////////////////
// Function:  GetSDKBuild
//
// Purpose:
// Reads the SDK build number from the registry.
///////////////////////////////////////////////////////
float CWGFTestFramework::GetSDKBuild()
{
	float fSDKBuild = 0.f;
	HKEY hRegKey = NULL;


	//
	// Open the DirectX registry key.
	//

	LONG OpenRegistryKeyResult = RegOpenKeyEx
	(
		HKEY_LOCAL_MACHINE,
		_T("Software\\Microsoft\\DirectX"),
		NULL,
		KEY_QUERY_VALUE,
		&hRegKey
	);

	if ( OpenRegistryKeyResult != ERROR_SUCCESS )
	{
		WriteToLog(_T("CWGFTestFramework::GetSDKBuild:  Failed to open DirectX registry key.\n" ) );

		goto Cleanup;
	};


	//
	// Read the SDK version from the registry.
	//

	DWORD nStrSize = sizeof(TCHAR) * 24;
	DWORD nValType = REG_SZ;
	TCHAR tcsVersion[ 24 + 1 ] = _T( "" );
	LONG ReadSDKVersionResult = RegQueryValueEx
	(
		hRegKey,
		_T("SDKVersion"),
		NULL,
		&nValType,
		(BYTE *) tcsVersion,
		&nStrSize
	);

	if ( ReadSDKVersionResult == ERROR_SUCCESS )
	{
		LPTSTR ptcBld = _tcschr(tcsVersion, _T('.'));
		if ( ptcBld != NULL )
		{
			ptcBld = _tcschr(ptcBld + 1, _T('.'));
			if ( ptcBld != NULL )
			{
				++ptcBld;

				LPTSTR ptcStop = NULL;
				fSDKBuild = (float)_tcstod(ptcBld, &ptcStop);
			};
		};
	};


Cleanup:
	//
	// Close the registry key.
	//

	if ( hRegKey != NULL )
	{
		RegCloseKey( hRegKey );
		hRegKey = NULL;
	};


	return fSDKBuild;
};

///////////////////////////////////////////////////////
// Function:  GetRetryPhase
//
// Purpose:
// Determines the current retry phase of the framework.
// A retry indicates that a test case has failed and the framework 
// will automatically retry that test case to see if it is consistent.
///////////////////////////////////////////////////////
RETRY_PHASE CWGFTestFramework::GetRetryPhase() const
{
	// Did the test indicate it should ever attempt a retry?
	// Are we on the first attempt of a test case?
	if( m_Retries <= 0 ||
		m_RetryCurrent == 0 )
	{
		return RETRY_PHASE_INITIAL;
	}

	//
	//  Determine what phase we are in
	//

	if( m_RetryCurrent > 1 )
	{
		// This will also cause the framework to call the test's Setup() and Cleanup() functions
		// Example: For graphics, recycle the device(s) between retries
		return RETRY_PHASE_RESET_AND_RETRY;
	}

	// Otherwise, just do a basic retry (SetupTestCase(), ExecuteTestCase(), CleanupTestCase())
	return RETRY_PHASE_RETRY;
}

void CWGFTestFramework::BeginSection( const WCHAR * szSectionName )
{
	m_DXGLog.BeginSection( szSectionName );
}


void CWGFTestFramework::EndSection( const bool bCommit )
{
	m_DXGLog.EndSection( bCommit == true ? TRUE : FALSE );
}


void CWGFTestFramework::LogAssertion
(
	const BOOL bCondition,
	const LPCWSTR FileName,
	const UINT FileLineNumber,
	const LPCWSTR Assertion
)
{
	m_DXGLog.LogAssertion
	(
		bCondition,
		FileName,
		FileLineNumber,
		Assertion
	);
}


void CWGFTestFramework::LogError
(
	const LPCWSTR FileName,
	const UINT FileLineNumber,
	const LPCWSTR ErrorDescription,
	const DWORD ErrorCode,
	const ERRORCODE_TYPE ErrorType
)
{
	m_DXGLog.LogError
	(
		FileName,
		FileLineNumber,
		ErrorDescription,
		ErrorCode,
		ErrorType
	);
}


void CWGFTestFramework::LogError
(
	const LPCWSTR FileName,
	const UINT FileLineNumber,
	const LPCWSTR ErrorDescription,
	const DWORD ErrorCode,
	const ERRORCODE_TYPE ErrorType,
	const LPCWSTR FailureCategory
)
{
	m_DXGLog.LogError
	(
		FileName,
		FileLineNumber,
		ErrorDescription,
		ErrorCode,
		ErrorType,
		FailureCategory
	);
}


void CWGFTestFramework::LogFile
(
	const LPCWSTR FileName
)
{
	m_DXGLog.LogFile
	(
		FileName
	);
}


void CWGFTestFramework::LogImage
(
	const LPCWSTR FileName
)
{
	m_DXGLog.LogImage
	(
		FileName
	);
}

void CWGFTestFramework::LogException(
	const CWGFExceptionTrace& exception )
{
	SetStatus( WGFTEST_STATUS_EXCEPTION_CAUGHT );

	WriteToLogF( "### %s", __FUNCTION__ );
	WriteToLogF( "### %s", exception.GetExceptionDescString().c_str() );

	for( size_t i = 0; i < exception.GetTraceDepth(); ++i )
	{
		WriteToLogF( "### %s", exception.GetTrace( i ).c_str() );
	}
}

void CWGFTestFramework::WriteMessageF( const CHAR *szFormat, ... )
{
	CHAR szBuffer[ 4096 ] = "";
	va_list args;
	va_start( args, szFormat );
	StringCchVPrintfA
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		args
	);
	va_end( args );

	WriteMessage( szBuffer );
}


void CWGFTestFramework::WriteMessageF( const WCHAR *szFormat, ... )
{
	WCHAR szBuffer[ 4096 ] = L"";
	va_list args;
	va_start( args, szFormat );
	StringCchVPrintfW
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		args
	);
	va_end( args );

	WriteMessage( szBuffer );
}


void CWGFTestFramework::WriteMessage( const CHAR *szText )
{
	if ( GetShell() )
	{
		GetShell()->MessageText( szText );
	}
	
	if ( m_bLogMessages )
		WriteToLog( szText );
}


void CWGFTestFramework::WriteMessage( const WCHAR *szText )
{
	if ( GetShell() )
	{
		GetShell()->MessageText( szText );
	}
	
	if ( m_bLogMessages )
		WriteToLog( szText );
}


void CWGFTestFramework::WriteToLog( UINT uLogLevel, const CHAR *szText )
{
	if ( !m_bNoLog && !m_bMuteLog )
	{
		m_DXGLog.TraceWhole( uLogLevel, szText );
	}
}


void CWGFTestFramework::WriteToLog( UINT uLogLevel, const WCHAR *szText )
{
	if ( !m_bNoLog && !m_bMuteLog )
	{
		m_DXGLog.TraceWhole( uLogLevel, szText );
	}
}


void CWGFTestFramework::WriteToLogF( UINT uLogLevel, const CHAR *szFormat, ... )
{
	CHAR szBuffer[ 4096 ] = "";

	va_list args;
	va_start( args, szFormat );
	StringCchVPrintfA
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		args
	);
	va_end( args );

	WriteToLog( uLogLevel, szBuffer );
}


void CWGFTestFramework::WriteToLogF( UINT uLogLevel, const WCHAR *szFormat, ... )
{
	WCHAR szBuffer[ 4096 ] = L"";

	va_list args;
	va_start( args, szFormat );
	StringCchVPrintfW
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		args
	);
	va_end( args );

	WriteToLog( uLogLevel, szBuffer );
}


void CWGFTestFramework::WriteToLogF( const CHAR *szFormat, ... )
{
	CHAR szBuffer[ 4096 ] = "";

	va_list args;
	va_start( args, szFormat );
	StringCchVPrintfA
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		args
	);
	va_end( args );

	WriteToLog( 1, szBuffer );
}


void CWGFTestFramework::WriteToLogF( const WCHAR *szFormat, ... )
{
	WCHAR szBuffer[ 4096 ] = L"";

	va_list args;
	va_start( args, szFormat );
	StringCchVPrintfW
	(
		szBuffer,
		ARRAYSIZE( szBuffer ),
		szFormat,
		args
	);
	va_end( args );

	WriteToLog( 1, szBuffer );
}


void CWGFTestFramework::WriteToLog( const CHAR *szText )
{
	WriteToLog( 1, szText );
}


void CWGFTestFramework::WriteToLog( const WCHAR *szText )
{
	WriteToLog( 1, szText );
}

void CWGFTestFramework::UnRegisterOptionVariable( LPCTSTR szName )
{
	if ( m_bLockOptionVariables )
	{
		_DPF( "Attempting to unregister option variable '%s' outside of appropriate InitOptionVariables() callback", szName );
		return;
	}

	m_optionsManager.UnregisterProperty(szName);
}


int CWGFTestFramework::AddOptionCategory( LPCTSTR szName )
{
	tstring strName( szName );
	
	InvOptionCategoryMapType::iterator it = m_InvOptionCategoryMap.find( strName );
	if ( it != m_InvOptionCategoryMap.end() )
		return it->second;
		
	int i = m_OptionCategories.size();
	m_OptionCategories.push_back( strName );
	m_InvOptionCategoryMap.insert( InvOptionCategoryMapType::value_type( strName, i ) );
	
	return i;
}

bool CWGFTestFramework::GetOptionValueAsString( const tstring &strName, tstring &strValue ) const
{
	CProperty *pProperty = m_optionsManager.FindProperty(strName);

	if(pProperty)
	{
		return pProperty->GetValueString(strValue);
	}

	return false;
}

bool CWGFTestFramework::SetOptionValueFromString( const tstring &strName, const tstring &strValue )
{
	CProperty *pProperty = m_optionsManager.FindProperty(strName);

	if(pProperty)
	{
		return pProperty->SetValueString(strValue);
	}

	return false;
}


bool CWGFTestFramework::Initialize()
{
	//
	// Create a log wrapper.
	//

	HRESULT hCreateLogWrapperResult = CDXGLog::CreateInstance
	(
		&m_DXGLog,
		IID_ISimpleLogController,
		(void **) &m_pLogWrapper
	);

	if ( FAILED( hCreateLogWrapperResult ) )
	{
		LogError
		(
			__FILEW__,
			__LINE__,
			L"CDXGLog::CreateInstance failed.",
			hCreateLogWrapperResult,
			ERRORCODE_TYPE_HRESULT
		);

		return false;
	};


	//
	// Register command-line arguments.
	//

	// Get OS version information
	m_osVersionInfo.dwOSVersionInfoSize = sizeof(m_osVersionInfo);
	if(!GetVersionEx((LPOSVERSIONINFO) &m_osVersionInfo))
	{
		WriteError( _T( "Failure getting OS version information." ) );
		return false;
	}

	// Allow registration of option variables.
	m_bLockOptionVariables = false;


	// Register framework core command-line arguments.
	int nGeneral = AddOptionCategory( _T( "General" ) ); // Always register this first 
	RegisterOptionVariable( _T( "?" ), _T( "Show this help." ), &m_bHelp, false, OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "WHQL" ), _T( "Run in WHQL mode." ), gOptionCallback_SetTestModeWHQL, tstring( _T("default") ), OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "BVT" ), _T( "Run in BVT mode. Runs a reduced set of test cases." ), gOptionCallback_SetTestModeBVT, tstring( _T("default") ), OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "TestMode" ), _T( "Set the test mode.  Many tests are designed to target multiple modes suited towards a testing goal." ), &m_TestMode, WGFTEST_MODE_UNDEFINED, OPTION_VALUE_CONSTANT, nGeneral, _T( "-testmode:[BVT|RUNTIME|WHQL|STRESS]" ) );
	RegisterOptionVariable( _T( "Random" ), _T( "Set random percentage of test cases to run. Use -seed to set seed." ), &m_fRandPercent, (float)1.0, OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "Seed" ), _T( "Set seed for random percentage of test cases to run.  Use -random to set percentage." ), &m_uSeed, (UINT)0, OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "Coverage" ), _T( "Enables code coverage integration." ), &m_bCoverage, false, OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "ShowDesktop" ), _T( "Shows the desktop on test startup." ), &m_bShowDesktopAtStartup, false, OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "AllowGhosting" ), _T( "Allow OS to ghost unresponsive windows." ), &m_bAllowGhosting, false, OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "AllowIdleDetection" ), _T( "Allow OS to perform idle tasks such as screensavers while test is running." ), &m_bAllowIdleDetection, false, OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "Priority" ), _T( "Limits test case execution to cases with equal or higher priority level (0 being highest)." ), &m_priorityLevel, -1, OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "PriorityProfile" ), _T( "Selects a priority profile potentially changing which test cases are run when '-priority' is used.  Leave out to use default profile." ), &m_priorityProfile, tstring( _T("")), OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "GenerateCSV" ), _T( "Generate CSV files containing all test cases in each group and exit" ), &m_bGenerateCSVOnly, false, OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "ExportVCXml" ), _T( "Exports variation core XML file" ), gOptionCallback_ExportVCXML, tstring(_T("")), OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "ImportVCXml" ), _T( "Imports variation core XML file" ), gOptionCallback_ImportVCXML, tstring(_T("")), OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "SFI" ), _T( "Enable Sequential Fault Injection. Format: -SFI[:[n[-|,halt]]]" ),gOptionCallback_SFI, tstring( _T( "default" ) ), OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "Session0" ), _T( "Run in Session 0." ), &m_bSession0, false, OPTION_VALUE_CONSTANT, nGeneral );
	RegisterOptionVariable( _T( "Retry" ), _T( "Retry failed test cases N number of times." ), gOptionCallback_Retry, tstring( _T( "default" ) ), OPTION_VALUE_CONSTANT, nGeneral, _T( "-retry:N" ) );

	// Register framework logging command-line arguments.
	int nLogging = AddOptionCategory( _T( "Logging" ) );
	RegisterOptionVariable( _T( "NoLog" ), _T( "Disables logging." ), &m_bNoLog, false, OPTION_VALUE_CONSTANT, nLogging );  
	RegisterOptionVariable( _T( "TextLog" ), _T( "Create a text log instead of XML." ), &m_bTextLog, false, OPTION_VALUE_CONSTANT, nLogging );
	RegisterOptionVariable( _T( "LogClean" ), _T( "Delete existing log." ), &m_bLogClean, false, OPTION_VALUE_CONSTANT, nLogging );

	RegisterOptionVariable( _T( "LogAll" ), _T( "Log all test case results." ), gOptionCallback_LogAll, tstring( _T( "default") ), OPTION_VALUE_CONSTANT, nLogging );

	RegisterOptionVariable( _T( "LogVerbose" ), _T( "Log detailed messages for each result." ), gOptionCallback_LogVerbose, tstring( _T( "default" ) ), OPTION_VALUE_CONSTANT, nLogging );
	RegisterOptionVariable( _T( "LogAllFailures" ), _T( "Log all failures (disabling failure count limit)." ), gOptionCallback_LogAllFailures, tstring( _T( "default" ) ), OPTION_VALUE_CONSTANT, nLogging );
	RegisterOptionVariable( _T( "MaxFailures" ), _T( "Set maximum failures logged per group." ), gOptionCallback_MaxFailures, tstring( _T( "default" ) ), OPTION_VALUE_CONSTANT, nLogging );

	RegisterOptionVariable( _T( "LogSkips" ), _T( "Log skips." ), gOptionCallback_LogSkips, tstring( _T( "default" ) ), OPTION_VALUE_CONSTANT, nLogging );
	RegisterOptionVariable( _T( "LogAllSkips" ), _T( "Log all skips (disabling failure count limit)." ), gOptionCallback_LogAllSkips, tstring( _T( "default" ) ), OPTION_VALUE_CONSTANT, nLogging );
	RegisterOptionVariable( _T( "MaxSkips" ), _T( "Set maximum skips logged per group." ), gOptionCallback_MaxSkips, tstring( _T( "default" ) ), OPTION_VALUE_CONSTANT, nLogging );

	RegisterOptionVariable( _T( "LogMessages" ), _T( "Dumps message text to the log." ), &m_bLogMessages, false, OPTION_VALUE_CONSTANT, nLogging );

	RegisterOptionVariable( _T( "LogWindow" ), _T( "Logs to the Log tab in the test shell window." ), &m_bLogToWindow, false, OPTION_VALUE_CONSTANT, nLogging );
	RegisterOptionVariable( _T( "LogWindowCaching" ), _T( "Uses log caching when logging to the window, if enabled for logfiles." ), &m_bLogToWindowCached, GetShell()->IsConsole(), OPTION_VALUE_CONSTANT, nLogging );

	if ( IsDebuggerPresent() == TRUE )
	{
		RegisterOptionVariable( _T( "Debug" ), _T( "Logs to the debugger." ), &m_bLogToDebugger, true, OPTION_VALUE_CONSTANT, nLogging );
	}
	else
	{
		RegisterOptionVariable( _T( "Debug" ), _T( "Logs to the debugger." ), &m_bLogToDebugger, false, OPTION_VALUE_CONSTANT, nLogging );
	};

	RegisterOptionVariable( _T( "NoCache" ), _T( "Enable no cache logging." ), &m_bNoCache, false, OPTION_VALUE_CONSTANT, nLogging );
	RegisterOptionVariable( _T( "FailureSummaryLevel" ), _T( "Log Level for failure summary. 0=none, 1=single values, 2=failure value pairs" ), &m_nFailureSummaryLevel, 1, OPTION_VALUE_CONSTANT, nLogging );
	RegisterOptionVariable( _T( "ForceLog" ), _T( "Forces logging of a duplicate device group to the database." ), &m_bDBForceLog, false, OPTION_VALUE_CONSTANT, nLogging );
	RegisterOptionVariable( _T( "KeepLog" ), _T( "Keeps the local log file when logging to the database." ), &m_bDBKeepLocalLog, false, OPTION_VALUE_CONSTANT, nLogging );
	RegisterOptionVariable( _T( "LogPath" ), _T( "Set the log file directory." ), &m_LogPath, tstring( _T("") ), OPTION_VALUE_CONSTANT, nLogging );
	RegisterOptionVariable( _T( "LogFileName" ), _T( "Set the name of the log file.  File extension will be added by the framework." ), &m_LogFileName, tstring( _T("") ), OPTION_VALUE_CONSTANT, nLogging );

	// Register framework debugging arguments.
	int nDebug = AddOptionCategory( _T( "Debugging" ) );
	//The default dpf level is 0 for Retail and 1 for Debug
#ifdef NDEBUG
	RegisterOptionVariable<DPFLevel>( _T( "dpf" ), _T( "Specifies the debug spew level." ), &m_dpfLevel, 0, 0, nDebug );
#else
	RegisterOptionVariable<DPFLevel>( _T( "dpf" ), _T( "Specifies the debug spew level." ), &m_dpfLevel, 1, 0, nDebug );
#endif

	RegisterOptionVariable( _T( "DoNotCatchExceptions" ), _T( "Do not catch exceptions from ExecuteTestCase." ), &m_bDoNotCatchExceptions, false, 0, nDebug );
#ifndef NDEBUG
	RegisterOptionVariable<size_t>( _T( "BreakOnAllocID" ), _T( "Causes a debug break in the memory manager for the given allocation id." ), &m_breakOnAllocID, 0, OPTION_VALUE_CONSTANT, nDebug );
#endif

	RegisterOptionVariable( _T( "ParamDefaultInit" ), _T( "Specifies how a parameter's default value should be initialized." ), gOptionCallback_ParamDefaultInit, tstring( _T( "none" ) ), 0, nDebug, _T("[zero|bitpattern|warn]. Zero = The default value should be Zero'ed out using ZeroMemory(). Bitpattern = Init with non-zero, repeating bit pattern. Warn = If the default value gets used, but wasn't set a debug break or assert will occur.") );

	// Initialize option variables.
	LocalInitOptionVariables();


	// Calll the shell to register option variables.
	if ( m_pShell )
	{
		m_pShell->InitOptionVariables();
	};


	//
	// Process command-line arguments.
	//

	for( size_t n = 0; n < m_CmdLine.GetNumArguments(); ++n )
	{
		tstring strArg( m_CmdLine.GetArgument( n ) );
		LocalProcessCommandLineArgument( strArg );
	};

	// Force no logging if just generating CSV file
	if (IsGenerateCSVOnly() || IsExportVCXml())
		m_bNoLog = true;

	// If exporting Variation Core XML then create the XmlBuilder
	if ( IsExportVCXml() )
	{
		m_variationCoreXmlBuilder = new CVariationCoreXmlBuilder();
	}

	CMemoryManager::Instance().BreakOnAllocationID( m_breakOnAllocID );

	SetRuntimeVersion();


	//
	// Register test groups.
	//

	SetLockTestRegistration( false );
	
	if ( !InitTestGroups() )
	{
		WriteError( _T( "Failure setting up test groups." ) );
		return false;
	}
	
	SetLockTestRegistration( true );

	// Verify that at least one test group has been added.
	CTestGroupPath Path;
	if ( !FindFirstTestGroup( Path ) )
	{
		WriteError( _T( "No test groups found.\nAt least one test group must be added in the application's InitTestGroups() method.\n" ) );
		return false;
	}

	//
	// Load play lists and filters.
	//
	
	// Add play list entries and filter strings from command line.
	for( size_t n = 0; n < m_CmdLine.GetNumArguments(); ++n )
	{
		size_t nOffset;
		tstring strArgName;
		tstring strArgValue;
		tstring strArg( m_CmdLine.GetArgument( n ) );
		
		nOffset = strArg.find_first_of( _T( "=<>" ) );
		if ( nOffset != tstring::npos )
		{
			// This is a filter string.
			// Add it to the filter list
			m_filterStrings.push_back( strArg );
		}
		else
		if ( !strArg.empty() && strArg[0] != '-' && strArg[0] != '/' && strArg[0] != '!' )
		{	
			nOffset = strArg.find_first_of( _T( ':' ) );
			
			if ( nOffset != tstring::npos )
			{
				strArgName = strArg.substr( 0, nOffset );
				strArgValue = strArg.substr( nOffset + 1 );
			}
			else
			{
				strArgName = strArg.substr( 0 );
				strArgValue = _T( "" );
			}
							
			CTestGroupPath path( strArgName );
			const CTestGroupNode *pNode = GetTestGroupNode( path );
			if ( pNode )
			{
				if ( pNode->IsTestNode() )
					m_pl.AddTestCases( (CTestGroupLeafNode *) pNode, strArgValue );
				else
					m_pl.AddTestBranch( pNode );			
			}
			else
			{
				_DPF( "Invalid Play List Entry - %s\n", strArg.c_str() );
			}
		}
	}


	// If no play list entries are given then compose a complete play list		
	if ( m_pl.Begin() == m_pl.End() )
	{
		m_pl.AddTestBranch( _T( "" ) );
	}


	// Process play list exclusions from command line
	for( size_t n = 0; n < m_CmdLine.GetNumArguments(); ++n )
	{
		size_t nOffset;
		tstring strArgName;
		tstring strArgValue;
		tstring strArg( m_CmdLine.GetArgument( n ) );
		
		nOffset = strArg.find_first_of( _T( "=<>" ) );
		if ( nOffset != tstring::npos )
			continue;
			
		if ( !strArg.empty() && strArg[0] == _T( '!' ) )
		{	
			// Make sure this is not a filter argument
			nOffset = strArg.find_first_of( _T( ':' ) );

			if ( nOffset != tstring::npos )
			{
				strArgName = strArg.substr( 1, nOffset - 1 );
				strArgValue = strArg.substr( nOffset + 1 );
			}
			else
			{
				strArgName = strArg.substr( 1 );
				strArgValue = _T( "" );
			}
							
			CTestGroupPath path( strArgName );
			const CTestGroupNode *pNode = GetTestGroupNode( path );
			if ( pNode )
			{
				if ( pNode->IsTestNode() )
					m_pl.RemoveTestCases( pNode->GetPath(), strArgValue );
				else
					m_pl.RemoveTestBranch( pNode );
			}
			else
			{
				_DPF( "Invalid Play List Exclusion - %s\n", strArg.c_str() );
			}
		}
	}


	//
	// Minimize everything else on the desktop using the
	// Win-M (show desktop) hotkey.
	//
	// N.B. [SEdmison]:
	// I know that this may seem a bit draconian, but it
	// should help to alleviate problems that result from
	// other windows' being above our test UI in the z-order.
	//

	if ( m_bShowDesktopAtStartup == true )
	{
		INPUT Inputs[ 4 ];
		ZeroMemory( Inputs, sizeof( Inputs ) );

		Inputs[ 0 ].type = INPUT_KEYBOARD;
		Inputs[ 0 ].ki.wVk = VK_LWIN;

		Inputs[ 1 ].type = INPUT_KEYBOARD;
		Inputs[ 1 ].ki.wVk = 0x4D;

		Inputs[ 2 ].type = INPUT_KEYBOARD;
		Inputs[ 2 ].ki.wVk = 0x4D;
		Inputs[ 2 ].ki.dwFlags = KEYEVENTF_KEYUP;

		Inputs[ 3 ].type = INPUT_KEYBOARD;
		Inputs[ 3 ].ki.wVk = VK_LWIN;
		Inputs[ 3 ].ki.dwFlags = KEYEVENTF_KEYUP;

		SendInput
		(
			ARRAYSIZE( Inputs ),
			Inputs,
			sizeof( Inputs[ 0 ] )
		);
	};


	//
	// Prevent the OS from deciding that our test app is not
	// responding and displaying "ghost" windows over our UI.
	//

	if ( m_bAllowGhosting == false )
	{
		HMODULE hUser32 = GetModuleHandle( _T( "user32.dll" ) );
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


	//
	// Setup random number pool based on input seed
	// Note: pool is used to not interrupt rand or srand calls between test cases
	//

	if (m_fRandPercent < 1.0f)
	{
		const UINT uSize = 1000;
		m_vRandPool.resize(uSize);
		srand(m_uSeed);
		for (UINT i = 0; i < uSize; ++i)
			m_vRandPool[i] = rand()/(float)RAND_MAX;

		
		m_RandPoolTemp = m_vRandPool[1];
		// reset to default seed
		srand(1);
	}


	//
	// Setup the shell.
	//

	if ( m_pShell )
	{
		if ( !m_pShell->Initialize() )
			return false;
	}


	//
	// Initialize the code coverage tools.
	//

	if ( IsCodeCoverage())
	{
		int	nResult = 0;

		m_hCovDLL = LoadLibrary( _T("wincover.dll") );
				
		if (NULL == m_hCovDLL)
		{
			WriteError(_T("CWGFTestFramework::Initialize() - WinCover.dll not detected.  Code Coverage setup failed.\n"));
			return false;
		}

		CoverageInitialize = (COVERAGEINITIALIZE) GetProcAddress( m_hCovDLL, "_CoverageInitialize@4" );
		CoverageReset = (COVERAGERESET) GetProcAddress( m_hCovDLL, "_CoverageResetA@4" );
		CoverageSave = (COVERAGESAVE) GetProcAddress( m_hCovDLL, "_CoverageSaveA@8" );
		CoverageUninitialize = (COVERAGEUNINITIALIZE) GetProcAddress( m_hCovDLL, "_CoverageUninitialize@0" );

		if (NULL == CoverageInitialize || NULL == CoverageReset ||
			NULL == CoverageSave || NULL == CoverageUninitialize)
		{
			_DPF(_T("CD3DWindowFramework::Initialize() - Code Coverage setup failed.\n"));
			FreeLibrary( m_hCovDLL );

			return false;
		}

		CoverageInitialize(NULL);
	}

	//
	// Disable IME
	// 
	if (	GetSystemMetrics( SM_IMMENABLED ) != 0 )
	{
		HRESULT hr = E_FAIL;
		m_hMSCTF = LoadLibrary( _T("msctf.dll") );

		if (m_hMSCTF != NULL)
		{
			PTF_CREATELANGBARMGR pfnCreateLangBarMgr;
			pfnCreateLangBarMgr = (PTF_CREATELANGBARMGR)GetProcAddress(m_hMSCTF, "TF_CreateLangBarMgr");

			if (pfnCreateLangBarMgr)
			{
				ITfLangBarMgr *pLangBarMgr;
				if (SUCCEEDED((*pfnCreateLangBarMgr)(&pLangBarMgr)))
				{
					if (  pLangBarMgr->GetShowFloatingStatus(&m_dwLangBarState) != S_OK ) // get setting
						m_dwLangBarState = (DWORD) 0 ;

					// Still try to minimize even if GetShowFloatingStatus fails
					hr = pLangBarMgr->ShowFloating(TF_SFT_MINIMIZED) ;
					pLangBarMgr->Release();
				} 

			}
		}
		if (hr != S_OK)
			_DPF(_T("CD3DWindowFramework::Initialize() - Unable to minimize language bar.\n"));

	}

	//
	// Open the log.
	//
	
	if ( !m_bNoLog )
	{
		// Open the log
		if ( !OpenLogFile() )
			return false;
	}

	//
	// Perform test initialization.
	//

	if ( !m_bNoLog )
	{
		// Log the beginning of test initialization.
		m_DXGLog.BeginTestInitialization();
	}	

	// Call test-defined Setup.
	bool bSetupResult = Setup();

	// Handle Setup failures.
	if ( bSetupResult != true )
	{
		if ( !m_bNoLog )
		{
			// Log the end of test initialization.
			m_DXGLog.EndTestInitialization( WTT_TESTCASE_RESULT_FAIL );


			m_DXGLog.BeginTestCase( _T( "Setup method failed." ), 0xFFFFFFFF );

			m_DXGLog.LogError
			(
				__FILEW__,
				__LINE__,
				L"Setup failed.",
				false,
				ERRORCODE_TYPE_BOOL
			);

			m_DXGLog.SetTestCaseResult( WTT_TESTCASE_RESULT_FAIL );

			m_DXGLog.EndTestCase();
		}

		return false;
	}

	if ( !m_bNoLog )
	{
		// Log the end of test initialization.
		m_DXGLog.EndTestInitialization( WTT_TESTCASE_RESULT_PASS );
	}


	//
	// Begin the first device group.
	//

	if ( !StartNewDeviceGroup() )
	{
		// This case doesn't produce evidence, but that's less of a
		// problem than the above Setup failure case because we're
		// not calling test-defined code; failures here should be
		// exceedingly rare.
		return false;
	}

	// Prevent the registration of option variables from here on out.
	m_bLockOptionVariables = true;

	return true;
}


void CWGFTestFramework::OptionCallback_SFI( const tstring &Value )
{
	m_SFIOptions = Value;

#if !defined(_ARM_) && !defined(_ARM64_)
	//
	// Only initialize SFI when we get a non-default callback
	// (the framework calls each callback twice, once with default value
	// once with actual value from the command line)
	//
	if( m_SFIOptions.compare( _T( "default" ) ) )
	{
		m_sfi.AddInjectionEnumerator( SFIDXInjectionPointEnumerator );
		m_sfi.Init( m_SFIOptions.c_str() );
	}
#endif
}

void CWGFTestFramework::OptionCallback_Retry( const tstring &Value )
{
	if ( Value.empty() ) // "-retry"
	{
		m_Retries = 2;
	}
	else if ( ToUpper(Value) == _T( "DEFAULT" ) )
	{
		// Do nothing.
	}
	else
	{
		m_Retries = (UINT) _ttol( Value.c_str() );
	};
};

void CWGFTestFramework::OptionCallback_LogAll( const tstring &Value )
{
	if ( Value == _T( "default" ) )
	{
		// Do nothing.
	}
	else if
	(
		ToLower(Value) == _T( "true" ) ||
		Value == _T( "" ) ||
		Value == _T( "1" )
	)
	{
		m_MaxGroupPasses = DXGLOG_MAX_TRACES;
		m_MaxTotalPasses = DXGLOG_MAX_TRACES;

		m_MaxGroupSkips = DXGLOG_MAX_TRACES;
		m_MaxTotalSkips = DXGLOG_MAX_TRACES;

		m_MaxGroupFailures = DXGLOG_MAX_TRACES;
		m_MaxTotalFailures = DXGLOG_MAX_TRACES;

		m_MaxGroupAborts = DXGLOG_MAX_TRACES;
		m_MaxTotalAborts = DXGLOG_MAX_TRACES;

		m_MaxGroupWarnings = DXGLOG_MAX_TRACES;
		m_MaxTotalWarnings = DXGLOG_MAX_TRACES;
	}
	else
	{
	};
};


void CWGFTestFramework::OptionCallback_LogAllSkips( const tstring &Value )
{
	if ( Value == _T( "default" ) )
	{
		// Do nothing.
	}
	else if
	(
		ToLower(Value) == _T( "true" ) ||
		Value == _T( "" ) ||
		Value == _T( "1" )
	)
	{
		m_MaxGroupSkips = DXGLOG_MAX_TRACES;
		m_MaxTotalPasses = DXGLOG_MAX_TRACES;
	}
	else if
	(
		ToLower(Value) == _T( "false" ) ||
		Value == _T( "0" )
	)
	{
		m_MaxGroupSkips = DEFAULT_MAX_GROUP_SKIPS;
		m_MaxTotalSkips = DEFAULT_MAX_TOTAL_SKIPS;
	}
	else
	{
	};
};


void CWGFTestFramework::OptionCallback_LogSkips( const tstring &Value )
{
	if ( Value == _T( "default" ) )
	{
		// Do nothing.
	}
	else if
	(
		ToLower(Value) == _T( "true" ) ||
		Value == _T( "" ) ||
		Value == _T( "1" )
	)
	{
		m_MaxGroupSkips = DXGLOG_MAX_TRACES;
		m_MaxTotalPasses = DXGLOG_MAX_TRACES;
	}
	else if
	(
		ToLower(Value) == _T( "false" ) ||
		Value == _T( "0" )
	)
	{
		m_MaxGroupSkips = 0;
		m_MaxTotalSkips = 0;
	}
	else
	{
	};
}


void CWGFTestFramework::OptionCallback_MaxSkips( const tstring &Value )
{
	if ( Value == _T( "default" ) )
	{
		// Do nothing.
	}
	else if
	(
		ToLower(Value) == _T( "all" ) ||
		Value == _T( "0" )
	)
	{
		m_MaxGroupSkips = DXGLOG_MAX_TRACES;
		m_MaxTotalSkips = DXGLOG_MAX_TRACES;
	}
	else
	{
		m_MaxGroupSkips = (UINT) _ttol( Value.c_str() );
	};
}


void CWGFTestFramework::OptionCallback_LogVerbose( const tstring &Value )
{
	if
	(
		ToLower(Value) == _T( "true" ) ||
		Value == _T( "" ) ||
		Value == _T( "1" )
	)
	{
		m_bLogVerbose = true;
	}
	else
	{
		m_bLogVerbose = false;
	}
}


void CWGFTestFramework::OptionCallback_LogAllFailures( const tstring &Value )
{
	if ( Value == _T( "default" ) )
	{
		// Do nothing.
	}
	else if
	(
		ToLower(Value) == _T( "true" ) ||
		Value == _T( "" ) ||
		Value == _T( "1" )
	)
	{
		m_MaxGroupFailures = DXGLOG_MAX_TRACES;
		m_MaxTotalFailures = DXGLOG_MAX_TRACES;
	}
	else if
	(
		ToLower(Value) == _T( "false" ) ||
		Value == _T( "0" )
	)
	{
		m_MaxGroupFailures = DEFAULT_MAX_GROUP_FAILURES;
		m_MaxTotalFailures = DEFAULT_MAX_TOTAL_FAILURES;
	}
	else
	{
	};
}


void CWGFTestFramework::OptionCallback_MaxFailures( const tstring &Value )
{
	if ( Value == _T( "default" ) )
	{
		// Do nothing.
	}
	else if
	(
		ToLower(Value) == _T( "all" ) ||
		Value == _T( "0" )
	)
	{
		m_MaxGroupFailures = DXGLOG_MAX_TRACES;
		m_MaxTotalFailures = DXGLOG_MAX_TRACES;
	}
	else
	{
		m_MaxGroupFailures = (UINT) _ttol( Value.c_str() );
	};
}

void CWGFTestFramework::OptionCallback_SetTestModeWHQL( const tstring &Value )
{
	if
	(
		ToLower(Value) == _T( "true" ) ||
		Value == _T( "" ) ||
		Value == _T( "1" )
	)
	{
		m_TestMode = WGFTEST_MODE_WHQL;
	}
};

void CWGFTestFramework::OptionCallback_SetTestModeBVT( const tstring &Value )
{
	if
	(
		ToLower(Value) == _T( "true" ) ||
		Value == _T( "" ) ||
		Value == _T( "1" )
	)
	{
		m_TestMode = WGFTEST_MODE_BVT;
	}
};

void CWGFTestFramework::gOptionCallback_Retry( const tstring &Value )
{
	// foward to local 
	GetFramework()->OptionCallback_Retry( Value );
}


void CWGFTestFramework::gOptionCallback_LogAll( const tstring &Value )
{
	// foward to local 
	GetFramework()->OptionCallback_LogAll( Value );
}


void CWGFTestFramework::gOptionCallback_LogVerbose( const tstring &Value )
{
	// foward to local 
	GetFramework()->OptionCallback_LogVerbose( Value );
}

void CWGFTestFramework::gOptionCallback_LogAllFailures( const tstring &Value )
{
	// foward to local 
	GetFramework()->OptionCallback_LogAllFailures( Value );
}


void CWGFTestFramework::gOptionCallback_MaxFailures( const tstring &Value )
{
	// foward to local 
	GetFramework()->OptionCallback_MaxFailures( Value );
}


void CWGFTestFramework::gOptionCallback_LogAllSkips( const tstring &Value )
{
	// forward to local
	GetFramework()->OptionCallback_LogAllSkips( Value );
}


void CWGFTestFramework::gOptionCallback_LogSkips( const tstring &Value )
{
	// foward to local 
	GetFramework()->OptionCallback_LogSkips( Value );
}


void CWGFTestFramework::gOptionCallback_MaxSkips( const tstring &Value )
{
	// foward to local 
	GetFramework()->OptionCallback_MaxSkips( Value );
}


void CWGFTestFramework::gOptionCallback_TestMode( const WGFTEST_MODE &mode )
{
	// foward to local 
	GetFramework()->OptionCallback_TestMode(mode);
}


void CWGFTestFramework::gOptionCallback_SetTestModeWHQL( const tstring &Value )
{
	// foward to local 
	GetFramework()->OptionCallback_SetTestModeWHQL( Value );
}


void CWGFTestFramework::gOptionCallback_SetTestModeBVT( const tstring &Value )
{
	// foward to local 
	GetFramework()->OptionCallback_SetTestModeBVT( Value );
}


void CWGFTestFramework::gOptionCallback_SFI( const tstring &Value )
{
	// foward to local 
	GetFramework()->OptionCallback_SFI(Value);
}


void CWGFTestFramework::gOptionCallback_ImportVCXML( const tstring &path )
{
	if(!path.empty())
	{
		GetFramework()->m_variationCoreXmlImportPath = path;
	}
}


void CWGFTestFramework::gOptionCallback_ExportVCXML( const tstring &path )
{
	if(!path.empty())
	{
		GetFramework()->m_variationCoreXmlExportPath = path;
	}
}

void CWGFTestFramework::gOptionCallback_ParamDefaultInit( const tstring &value )
{
	// Parse the value
	if( ToLower(value) == _T("zero") )
	{
		CVariationGenerator::s_parameterDefaultValueInitialization = PARAMETER_DEFAULT_VALUE_INITIALIZATION_ZERO;
	}
	else if( (ToLower(value) == _T("flood")) || (ToLower(value) == _T("bitpattern")) )
	{
		CVariationGenerator::s_parameterDefaultValueInitialization = PARAMETER_DEFAULT_VALUE_INITIALIZATION_FLOOD;
	}
	else if( (ToLower(value) == _T("warn")) )
	{
		CVariationGenerator::s_parameterDefaultValueInitialization = PARAMETER_DEFAULT_VALUE_INITIALIZATION_WARN;
	}
	else
	{
		CVariationGenerator::s_parameterDefaultValueInitialization = PARAMETER_DEFAULT_VALUE_INITIALIZATION_NONE;
	}
}


bool CWGFTestFramework::Update()
{
	// Update the shell
	if ( m_pShell )
	{
		if ( !m_pShell->Update() )
			return false;
	}
	
	return true;
};


void CWGFTestFramework::Uninitialize()
{
	if ( m_pTest )
	{
		if (IsExportVCXml() && m_variationCoreXmlBuilder)
		{
			tstring errorString;

			// If Xml path is empty then set to test name
			if (m_variationCoreXmlExportPath.empty())
			{
				m_variationCoreXmlExportPath = GetName() + tstring(_T(".xml"));
			}

			if (FAILED(m_variationCoreXmlBuilder->SaveXml(m_variationCoreXmlExportPath, errorString)))
			{
				WriteError(errorString.c_str());
			}
		}
		else if (IsGenerateCSVOnly())
		{
			EndCSV();
		}
		else if (!m_needTestSetup)
		{
			if ( m_LastResult == RESULT_PENDING && m_bTestCaseInitialized )
			{
				CleanupTestCase();
			}

			CleanupTest();

			PostCleanupTest();
		}
	}

	
	// Call test-wide cleanup unless we're recovering from an
	// exception (in which case the cleanup code that the test
	// might want to execute could call into components under
	// test that are in a bad state).
	if ( m_Status != WGFTEST_STATUS_EXCEPTION_CAUGHT )
	{
		Cleanup();
	}


	// Close the log file but first end the current device group
	if ( !m_bNoLog )
	{
		if (m_DXGLog.InDeviceGroup())
		{
			m_DXGLog.EndDeviceGroup();  
		}

		m_DXGLog.Close();
	}


	// Release the coverage object
	if (IsCodeCoverage() && m_hCovDLL)
	{
		if ( CoverageUninitialize )
		{
			CoverageUninitialize();
		}

		FreeLibrary(m_hCovDLL);
		m_hCovDLL = NULL;
	}


	// Destroy the shell
	if ( m_pShell )
	{
		m_pShell->Destroy();
		m_pShell = NULL;
	}


	//
	// Restore the desktop using the Shift-Win-M hotkey.
	//

	if ( m_bShowDesktopAtStartup == true )
	{
		INPUT Inputs[ 6 ];
		ZeroMemory( Inputs, sizeof( Inputs ) );

		Inputs[ 0 ].type = INPUT_KEYBOARD;
		Inputs[ 0 ].ki.wVk = VK_SHIFT;

		Inputs[ 1 ].type = INPUT_KEYBOARD;
		Inputs[ 1 ].ki.wVk = VK_LWIN;

		Inputs[ 2 ].type = INPUT_KEYBOARD;
		Inputs[ 2 ].ki.wVk = 0x4D;

		Inputs[ 3 ].type = INPUT_KEYBOARD;
		Inputs[ 3 ].ki.wVk = 0x4D;
		Inputs[ 3 ].ki.dwFlags = KEYEVENTF_KEYUP;

		Inputs[ 4 ].type = INPUT_KEYBOARD;
		Inputs[ 4 ].ki.wVk = VK_LWIN;
		Inputs[ 4 ].ki.dwFlags = KEYEVENTF_KEYUP;

		Inputs[ 5 ].type = INPUT_KEYBOARD;
		Inputs[ 5 ].ki.wVk = VK_SHIFT;
		Inputs[ 5 ].ki.dwFlags = KEYEVENTF_KEYUP;

		SendInput
		(
			ARRAYSIZE( Inputs ),
			Inputs,
			sizeof( Inputs[ 0 ] )
		);
	};


	if ( m_hMSCTF != NULL )
	{
		if ( m_dwLangBarState != 0x0 )
		{
			PTF_CREATELANGBARMGR pfnCreateLangBarMgr;

			pfnCreateLangBarMgr = (PTF_CREATELANGBARMGR)GetProcAddress(m_hMSCTF, "TF_CreateLangBarMgr");

			if (pfnCreateLangBarMgr)
			{
				ITfLangBarMgr *pLangBarMgr;

				if (SUCCEEDED((*pfnCreateLangBarMgr)(&pLangBarMgr)))
					pLangBarMgr->ShowFloating(m_dwLangBarState);

				pLangBarMgr->Release();
			}
		}

		FreeLibrary(m_hMSCTF);
		m_hMSCTF = NULL;
	}
};


bool CWGFTestFramework::StartNewDeviceGroup()
{
	if ( !m_bNoLog )
	{
		//end the old device group
		if (m_DXGLog.InDeviceGroup())
			m_DXGLog.EndDeviceGroup();

		//start a new device group
		m_DXGLog.BeginDeviceGroup();
		
		//if the device info has been set, log it, otherwise just log system info
		bool bUpdateCurrentDeviceInfoResult = UpdateCurrentDeviceInfo(&m_CurrentDeviceInfo);
		m_CurrentDeviceInfo.fBuild = GetSDKBuild();
		if (bUpdateCurrentDeviceInfoResult)
		{
			if ( !LogDeviceInfo() )
			{
				return false;
			}
		}
	}
	
	return true;
}


bool CWGFTestFramework::OpenLogFile()
{
	TCHAR tcsBuffer[WORK_BUFFER_SIZE] = {0};

	// Fill out the DXGLOG_INFO struct
	COPYSTRING (m_LogInfo.sExeName, GetExeName( false, tcsBuffer ) );

	if (_sntprintf( tcsBuffer, WORK_BUFFER_SIZE-1, _T("%s %s"), m_LogInfo.sExeName, m_CmdLine.GetArgumentString() ) < 0 )
		tcsBuffer[WORK_BUFFER_SIZE-1] = _T('\0');

	COPYSTRING( m_LogInfo.sCmdLine, tcsBuffer);

	// set the log file name
	if (m_LogFileName.length() > 0)
	{
		COPYSTRING( m_LogInfo.sTestName, m_LogFileName.c_str() );
	}
	else
	{
		COPYSTRING( m_LogInfo.sTestName, GetName() );
	}

//	m_LogInfo.hWnd = m_pShell ? m_pShell->GetMainHWnd() : NULL;
	m_LogInfo.hWnd = NULL;
	m_LogInfo.TestType = GetTestType();

	// set the log path
	if (m_LogPath.length() > 0)
	{
		COPYSTRING (m_LogInfo.sLocalDirectory,m_LogPath.c_str());
	}
	
	// Set default options
	m_LogInfo.dwOptions |= DXGLOG_OPTION_OUTPUT_XML | DXGLOG_OPTION_UNICODE;

	// Determine log endpoints.
	if ( m_bLogToDebugger == true )
	{
		m_LogInfo.dwOptions |= DXGLOG_OPTION_OUTPUT_DEBUGGER;
	};

	if ( m_bLogToWindow == true )
	{
		m_LogInfo.dwOptions |= DXGLOG_OPTION_OUTPUT_WINDOW;
	};

	if ( m_bLogToWindowCached == true )
	{
		m_LogInfo.dwOptions |= DXGLOG_OPTION_OUTPUT_WINDOW_CACHED;
	};

	if (m_bTextLog)
	{
		m_LogInfo.dwOptions &= ~DXGLOG_OPTION_OUTPUT_XML;
		m_LogInfo.dwOptions |= DXGLOG_OPTION_OUTPUT_PLAINTEXT;
	}

	// Determine result types to log.
	if ( m_TestMode == WGFTEST_MODE_WHQL )
	{
		if
		(
			m_MaxGroupSkips == DEFAULT_MAX_GROUP_SKIPS &&
			m_MaxTotalSkips == DEFAULT_MAX_TOTAL_SKIPS
		)
		{
			m_MaxGroupSkips = 0;
			m_MaxTotalSkips = 0;
		};

		m_MaxGroupFailures = DXGLOG_MAX_TRACES;
		m_MaxTotalFailures = DXGLOG_MAX_TRACES;

		m_MaxGroupAborts = DXGLOG_MAX_TRACES;
		m_MaxTotalAborts = DXGLOG_MAX_TRACES;

		m_MaxGroupWarnings = DXGLOG_MAX_TRACES;
		m_MaxTotalWarnings = DXGLOG_MAX_TRACES;
	};

	m_LogInfo.uMaxGroupPasses = m_MaxGroupPasses;
	m_LogInfo.uMaxLogFilePasses = m_MaxTotalPasses;

	m_LogInfo.uMaxGroupFailures = m_MaxGroupFailures;
	m_LogInfo.uMaxLogFileFailures = m_MaxTotalFailures;

	m_LogInfo.uMaxGroupSkips = m_MaxGroupSkips;
	m_LogInfo.uMaxLogFileSkips = m_MaxTotalSkips;

	m_LogInfo.uMaxGroupAborts = m_MaxGroupAborts;
	m_LogInfo.uMaxLogFileAborts = m_MaxTotalAborts;

	m_LogInfo.uMaxGroupWarnings = m_MaxGroupWarnings;
	m_LogInfo.uMaxLogFileWarnings = m_MaxTotalWarnings;

	// Setup a few various log options
	if (m_bLogClean)
	{
		m_LogInfo.dwOptions |= DXGLOG_OPTION_OVERWRITE;
	};

	if (m_bNoCache)
	{
		m_LogInfo.dwOptions |= DXGLOG_OPTION_NOCACHE;
	};


	// Check allocations
	if
	(
		NULL == m_LogInfo.sTestName ||
		NULL == m_LogInfo.sExeName ||
		NULL == m_LogInfo.sCmdLine ||
		(m_LogPath.length() > 0 && m_LogInfo.sLocalDirectory == NULL)
	)
	{
		_DPF( _T("CD3DWindowFramework::OpenLogFile:  Out of memory.\n") );

		return false;
	}

	//  Open the log
	if (FAILED(m_DXGLog.Open(&m_LogInfo)))
	{
		return false;
	}

	return true;
}


// This function is called when we are logging full device info as opposed to 
// only system info.  The dxglogger would initialize  any unset strings to "N/A",
// But we set them here to "Unknown" to indicate that they are being logged and 
// should be set by the user overloading UpdateCurrentDeviceInfo().

bool CWGFTestFramework::LogDeviceInfo()
{
	TCHAR * sUnknown = _T("Unknown");

	if (NULL == m_CurrentDeviceInfo.sDisplay)
		COPYSTRING(m_CurrentDeviceInfo.sDisplay, sUnknown);
	if (NULL == m_CurrentDeviceInfo.sDriver)
		COPYSTRING(m_CurrentDeviceInfo.sDriver, sUnknown);
	if (NULL == m_CurrentDeviceInfo.sDriverVersion)
		COPYSTRING(m_CurrentDeviceInfo.sDriverVersion, sUnknown);
	if (NULL == m_CurrentDeviceInfo.sPnPID)
		COPYSTRING(m_CurrentDeviceInfo.sPnPID, sUnknown);
	if (NULL == m_CurrentDeviceInfo.sResolution)
		COPYSTRING(m_CurrentDeviceInfo.sResolution, sUnknown);
	if (NULL == m_CurrentDeviceInfo.sDevice)
		COPYSTRING(m_CurrentDeviceInfo.sDevice, sUnknown);

	if (FAILED(m_DXGLog.LogDeviceInfo(&m_CurrentDeviceInfo)))
	{
		_DPF(_T("CD3DWindowFramework::LogDeviceInfo() - DXGLogger::LogDeviceInfo() failed.\n"));
		return false;
	}

	return true;
}


void CWGFTestFramework::LocalProcessCommandLineArgument( const tstring &strArg )
{
	tstring strArgName;
	tstring strArgValue;
	size_t nOffset;
	
	if ( strArg.empty() )
		return;
		
	// Allows the shell to process the argument.
	if ( m_pShell )
	{
		if ( m_pShell->ProcessCommandLineArgument( strArg ) )
			return;
	}
			
	// Allow the framework overload to process the argument.
	if ( ProcessCommandLineArgument( strArg ) )
		return;

	if ( strArg[0] == '/' || strArg[0] == '-' )
	{
		// Find the first occurrence of ':'
		nOffset = strArg.find_first_of( _T( ':' ) );
		
		if ( nOffset != tstring::npos )
		{
			strArgName = strArg.substr( 1, nOffset - 1 );
			strArgValue = strArg.substr( nOffset + 1 );
		}
		else
		{
			strArgName = strArg.substr( 1 );
			strArgValue = _T( "" );
		}

		// Is this a registered option variable?
		CProperty *pProperty = m_optionsManager.FindProperty(strArgName.c_str());
		if(pProperty)
		{
            pProperty->SetValueString(strArgValue.c_str());
			return;
		}
	}
	
	// Play list entries must be handled later since
	// the test group tree has not yet been initialized.
}


const TCHAR *CWGFTestFramework::GetName() const
{
	static TCHAR szName[1024] = {0};
	GetExeName( true, szName ); // BUGBUG - I think this will break if this library is built as a DLL.
	return szName;
}


void CWGFTestFramework::LocalInitOptionVariables()
{
	InitOptionVariables();
	
	RegisterOptionVariable( _T( "DebugOnFail" ), _T( "Break into a debugger for test case failures" ), &TEST_RESULT::s_bDebugOnFail, false );
}


bool CWGFTestFramework::ProcessCommandLineArgument( const tstring &strArgument )
{
	return false;
}


WGFTEST_STATUS CWGFTestFramework::GoToFirstTestCase()
{
	// Perform test case cleanup if needed.
	if ( m_pTest && m_bTestCaseInitialized )
	{
		CleanupTestCase();
	}


	// Avoid running any more tests cases if the user has aborted
	// the test or we've recovered from an exception.
	if ( IsStatusContinuable() != true )
	{
		return m_Status;
	}


	CPlayList::Iterator plit = m_pl.Begin();

	// Check whether the play list is empty.
	if ( plit == m_pl.End() )
	{
		SetStatus( WGFTEST_STATUS_NOT_FOUND );
		return m_Status;
	}


	// Find the first unfiltered test case.
	for( ; plit != m_pl.End(); plit.NextCase() )
	{
		m_Status = LocalGoToTestCase( plit.GetNode(), plit.GetTestCaseNumber() );

		switch ( m_Status )
		{
			case WGFTEST_STATUS_OK:
			{
				m_plit = plit;
				return m_Status;
			} break;


			case WGFTEST_STATUS_SKIP_TEST:
			case WGFTEST_STATUS_ABORT_TEST:
			case WGFTEST_STATUS_ABORT_TEST_USERINITIATED:
			case WGFTEST_STATUS_EXCEPTION_CAUGHT:
			{
				plit.NullCase();
				m_plit = plit;
				return m_Status;
			} break;
		};
	}


	SetStatus( WGFTEST_STATUS_NOT_FOUND );
	return m_Status;
}


WGFTEST_STATUS CWGFTestFramework::GoToLastTestCase()
{
	// Perform test case cleanup if needed.
	if ( m_pTest && m_bTestCaseInitialized )
	{
		CleanupTestCase();
	}


	// Avoid running any more tests cases if the user has aborted
	// the test or we've recovered from an exception.
	if ( IsStatusContinuable() != true )
	{
		return m_Status;
	}


	size_t nCaseRangeIndex = 0;

	// Find the last unfiltered test case.
	CPlayList::Iterator plit = m_pl.End();

	if ( plit == m_pl.Begin() )
	{
		SetStatus( WGFTEST_STATUS_NOT_FOUND );
		return WGFTEST_STATUS_NOT_FOUND; // No tests in playlist.
	}

	for( ; plit != m_pl.Begin(); plit.PrevCase() )
	{
		m_Status = LocalGoToTestCase( plit.GetNode(), plit.GetTestCaseNumber() );

		switch ( m_Status )
		{
			case WGFTEST_STATUS_OK:
			{
				m_plit = plit;
				return m_Status;
			} break;


			case WGFTEST_STATUS_SKIP_TEST:
			case WGFTEST_STATUS_ABORT_TEST:
			case WGFTEST_STATUS_ABORT_TEST_USERINITIATED:
			case WGFTEST_STATUS_EXCEPTION_CAUGHT:
			{
				plit.NullCase();
				m_plit = plit;
				return m_Status;
			} break;
		}
	}


	SetStatus( WGFTEST_STATUS_NOT_FOUND );
	return m_Status;
}


WGFTEST_STATUS CWGFTestFramework::GoToNextTestCase()
{
	// Perform test case cleanup if needed.
	if ( m_pTest && m_bTestCaseInitialized )
	{
		CleanupTestCase();
	}


	// Avoid running any more tests cases if the user has aborted
	// the test or we've recovered from an exception.
	if ( IsStatusContinuable() != true )
	{
		return m_Status;
	}


	if ( m_plit == m_pl.End() )
	{
		SetStatus( WGFTEST_STATUS_NOT_FOUND );
		return m_Status;
	}

	// Reset our counter for number of times we will retry a failed test case, if the -retry parameter is used
	m_RetryCurrent = 0;

	CPlayList::Iterator plit = m_plit;
	
	for( plit.NextCase(); plit != m_pl.End(); plit.NextCase() )
	{
		m_Status = LocalGoToTestCase( plit.GetNode(), plit.GetTestCaseNumber() );

		switch ( m_Status )
		{
			case WGFTEST_STATUS_OK:
			{
				m_plit = plit;
				return m_Status;
			} break;


			case WGFTEST_STATUS_SKIP_TEST:
			case WGFTEST_STATUS_ABORT_TEST:
			case WGFTEST_STATUS_ABORT_TEST_USERINITIATED:
			case WGFTEST_STATUS_EXCEPTION_CAUGHT:
			{
				plit.NullCase();
				m_plit = plit;
				return m_Status;
			} break;
		};
	}


	SetStatus( WGFTEST_STATUS_NOT_FOUND );
	return m_Status;
}


WGFTEST_STATUS CWGFTestFramework::GoToPrevTestCase()
{
	// Perform test case cleanup if needed.
	if ( m_pTest && m_bTestCaseInitialized )
	{
		CleanupTestCase();
	}


	// Avoid running any more tests cases if the user has aborted
	// the test or we've recovered from an exception.
	if ( IsStatusContinuable() != true )
	{
		return m_Status;
	}


	if ( m_plit == m_pl.Begin() )
	{
		SetStatus( WGFTEST_STATUS_NOT_FOUND );
		return m_Status;
	}


	CPlayList::Iterator plit = m_plit;
	
	for ( plit.PrevCase(); plit != m_pl.Begin(); plit.PrevCase() )
	{
		m_Status = LocalGoToTestCase( plit.GetNode(), plit.GetTestCaseNumber() );

		switch ( m_Status )
		{
			case WGFTEST_STATUS_OK:
			{
				m_plit = plit;
				return m_Status;
			} break;


			case WGFTEST_STATUS_SKIP_TEST:
			case WGFTEST_STATUS_ABORT_TEST:
			case WGFTEST_STATUS_ABORT_TEST_USERINITIATED:
			case WGFTEST_STATUS_EXCEPTION_CAUGHT:
			{
				plit.NullCase();
				m_plit = plit;
				return m_Status;
			} break;
		};
	}

	
	SetStatus( WGFTEST_STATUS_NOT_FOUND );
	return m_Status;
}


WGFTEST_STATUS CWGFTestFramework::LocalGoToTestCase( CTestGroupLeafNode *pNode, size_t nNumber )
{
	m_bIsNewGroup = false;
	assert( m_pTest == NULL || !m_bTestCaseInitialized );
	
	if ( !nNumber )
	{
		return WGFTEST_STATUS_NOT_FOUND;
	}

	if (!pNode->IsActive())
	{
		// This is a new test group so 
		if ( m_pTest )
		{
			if (IsGenerateCSVOnly())
			{
				EndCSV();
			}
			else if (!m_needTestSetup)
			{
				CleanupTest();

				PostCleanupTest();
			}

			// Avoid running any more tests cases if the user has
			// aborted the test or we've recovered from an exception.
			if ( IsStatusContinuable() != true )
			{
				return m_Status;
			}
		}

		m_bIsNewGroup = true;
		PreSetupTest();
		m_pTest = pNode->NewTest();

		// Initialize test parameters
		m_pTest->BeginTestParameters();
		m_pTest->InitTestParameters();
		m_pTest->InitOutputParameters();
		m_pTest->EndTestParameters();
	}
	else
	{
		m_pTest = pNode->GetTest();
	}

	if ( !m_pTest )
	{
		return WGFTEST_STATUS_NOT_FOUND;
	}

	if ( m_bIsNewGroup )
	{
		if ( IsImportVCXml())
		{
			tstring errorString;
			CVariationCoreXmlLoader variationCoreXmlLoader;

			// Clear existing priorityprofiles
			m_pTest->ClearPriorityProfiles();

			// Create xml reader
			if (FAILED(variationCoreXmlLoader.CreateXmlFileForRead(m_variationCoreXmlImportPath, errorString)))
			{
				WriteError(errorString.c_str());
				return WGFTEST_STATUS_SKIP_TEST;
			}

			// Read priority profiles
			tstring groupPath = m_pTest->GetPath().ToString();
			if (FAILED(variationCoreXmlLoader.LoadVariationGroupXml(m_pTest, groupPath, errorString)))
			{
				WriteError(errorString.c_str());
				return WGFTEST_STATUS_SKIP_TEST;
			}
		}

		if ( IsExportVCXml())
		{
			// Iterate through priority profiles and add them to the Xml Builder
			for(size_t i = 0; i < m_pTest->GetNumPriorityProfiles(); ++i)
			{
				// By default use the full test group path name as the VariationGroup name
				tstring path = m_pTest->GetPath().ToString();
				CVariationGenerator::CPriorityProfile *priorityProfile = m_pTest->GetPriorityProfile(i);
				m_variationCoreXmlBuilder->AddPriorityProfile(m_pTest, path, priorityProfile);
			}

			// We can skip the rest of the test
			m_needTestSetup = false;
			return WGFTEST_STATUS_SKIP_TEST;
		}

		RefreshFilter( m_pTest );
		m_needTestSetup = true;

		// Fixup the playlist by removing any test case numbers beyond the
		// last test case number.
		size_t numCases = m_pTest->GetNumVariations();

		m_pl.RemoveTestCases( m_pTest->GetPath(), numCases + 1 );

		if ( numCases == 0 )
		{
			return WGFTEST_STATUS_NOT_FOUND;
		}

		LogBeginTestGroup( m_pTest->GetPath().ToString().c_str() );
	}

	if ( nNumber > m_pTest->GetNumVariations() )
	{
		if (IsGenerateCSVOnly())
		{
			EndCSV();
		}
		else if (!m_needTestSetup)
		{
			CleanupTest();

			PostCleanupTest();
		}


		// Avoid running any more tests cases if the user has
		// aborted the test or we've recovered from an exception.
		if ( IsStatusContinuable() != true )
		{
			return m_Status;
		}


		return WGFTEST_STATUS_NOT_FOUND;
	}

		
	// Avoid running any more tests cases if the user has aborted
	// the test or we've recovered from an exception.
	if ( IsStatusContinuable() != true )
	{
		return m_Status;
	}


	// Generate test case	
	PreGenerateTestCase();
	try
	{
		if ( !m_pTest->GenerateVariation( nNumber ) )
		{
			return WGFTEST_STATUS_NOT_FOUND;
		}
	}
	catch(std::exception &e)
	{
		WriteError(str2tstr(std::string(e.what())).c_str());

		// Rethrow the exception
		throw;
	}

	// If test cases are prioritized then set the factor index to priority index
	CVariationGenerator::CPriorityProfile *priorityProfile = m_pTest->GetActivePriorityProfile();
	if ( priorityProfile && m_priorityLevel >= 0 )
	{
		int maxPriorityLevel = priorityProfile->GetMaxDeclaredPriorityLevel();

		if ( m_priorityLevel <= maxPriorityLevel)
		{
			if ( m_pTest->IsPrioritySorted() )
			{
				int testCasePriorityLevel = m_pTest->GetVariationPriorityLevel( nNumber );
				if ( testCasePriorityLevel < 0 || testCasePriorityLevel > m_priorityLevel )
				{
					// Since test variations are sorted the rest of the test can be skipped
					return WGFTEST_STATUS_SKIP_TEST;
				}
			}
			else
			{
				float priorityValue = m_pTest->CalcGeneratedVariationPriorityValue();
				int testCasePriorityLevel = priorityProfile->GetPriorityLevel( priorityValue );
				if ( testCasePriorityLevel < 0 || testCasePriorityLevel > m_priorityLevel )
				{
					// Skip this test variation
					return WGFTEST_STATUS_SKIP_TEST_CASE;
				}
			}
		}
	}

	// Compare to filters
	CFilterHandle filter = m_pTest->GetSkipMatchingFilter();
	if ( filter.IsValid() && filter->Match() )
	{
		return WGFTEST_STATUS_SKIP_TEST_CASE;
	}

	filter = m_pTest->GetRootFilter();
	if ( filter.IsValid() && !filter->Match() )
	{
		return WGFTEST_STATUS_NOT_FOUND;
	}
	
	//
	// Make sure there is at least one test per group.
	// Don't need to do again if new group has same number of cases 
	// as the last group.
	//
	if (m_vRandPool.size() && (m_LastTestCount != m_pTest->GetNumVariations()))
	{		
		//
		//  Restore the orginal value
		//
		m_vRandPool[1] = m_RandPoolTemp;
		
		bool foundAtLeastOneTestCase = false;
		const size_t size = min ( m_pTest->GetNumVariations(), m_vRandPool.size() );
		for (size_t i = 1; i < size; ++i)
		{
			if (m_fRandPercent >= m_vRandPool[i])
			{
				foundAtLeastOneTestCase = true;
				break;
			}
		}

		if (foundAtLeastOneTestCase == false)
		{
			//
			//  Rig the m_vRandPool so one case is added. 
			// 
			m_vRandPool[1] = 0.0f;	
		}
	}

	m_LastTestCount = m_pTest->GetNumVariations();

	// Rand percentage skip
	if (m_vRandPool.size() && m_fRandPercent < m_vRandPool[nNumber%m_vRandPool.size()])
	{
		return WGFTEST_STATUS_SKIP_TEST_CASE;
	}

	// Compose test case name
	m_testCaseName = m_pTest->GenerateTestCaseName();
	
	if ( m_needTestSetup )
	{
		if ( IsGenerateCSVOnly() )
		{
			//
			// If generating CSV file then update the file here...
			//
			BeginCSV();
			m_needTestSetup = false;
		}
		else
		{
			// Test group changed.  Begin a new test
			TEST_RESULT tr;

			// seed random number so any setup functions behave consistently
			srand(1);

			m_AllParamValues.clear();

			m_PairAllParamValues.clear();

			if (m_bDoNotCatchExceptions)
			{
				tr = SetupTest();
			}
			else
			{		
				TrySetupTest(tr);
			}

			m_needTestSetup = false;	


			// Any failure to initialize indicates that we should
			// abort the entire group.		
			if
			(
				tr == RESULT_FAIL ||
				tr == RESULT_BLOCKED
			)
			{
				tr = RESULT_ABORT;
			}


			switch ( tr )
			{
				case RESULT_SKIP:
				{
					if ( m_bNoLog == false )
					{
						assert( m_DXGLog.InTestCase() == false );
						m_DXGLog.BeginTestCase( _T( "Test Initialization" ), 0 );
						m_DXGLog.SkipTest();
					}

					m_LastResult = tr;
					
					return WGFTEST_STATUS_SKIP_TEST;
				} break;


				case RESULT_ABORT:
				{
					if ( m_bNoLog == false )
					{
						assert( m_DXGLog.InTestCase() == false );
						m_DXGLog.BeginTestCase( _T( "Test Initialization" ), 0 );
						m_DXGLog.AbortTest();
					}

					m_LastResult = tr;

					return WGFTEST_STATUS_ABORT_TEST;
				} break;
			}


			// Avoid running any more tests cases if the user has
			// aborted the test or we've recovered from an
			// exception.
			if ( IsStatusContinuable() != true )
			{
				return m_Status;
			}
		}
	}
	
	if ( IsGenerateCSVOnly() )
	{
		//
		// If generating CSV file then update the file here...
		//
		WriteVariationCSV(nNumber);
		return WGFTEST_STATUS_OK;
	}

	m_LastResult = RESULT_PENDING;
	return WGFTEST_STATUS_OK;
}


void CWGFTestFramework::RefreshFilter( CTest *pTest )
{
	// Iterate through filter strings.  'Or' together separate filter expressions.
	CFilterHandle filter;
	CFilterHandle filterRoot = pTest->GetRootFilter();
	
	for( std::vector< tstring >::iterator it = m_filterStrings.begin(); it != m_filterStrings.end(); ++it )
	{
		const tstring &filterExpr = *it;
		filter = CreateFilter( pTest, filterExpr );
		if ( filterRoot.IsValid() )
			filterRoot = filterRoot || filter;
		else
			filterRoot = filter;
	}
	
	pTest->SetRootFilter( filterRoot );
}

int CWGFTestFramework::CWGFExceptionTrace::FilterProc( 
	EXCEPTION_POINTERS *ep )
{
	//
	// Validate parameters.
	//

	if ( ep == NULL )
	{
		return EXCEPTION_CONTINUE_SEARCH;
	};

	if ( ep->ExceptionRecord == NULL )
	{
		return EXCEPTION_CONTINUE_SEARCH;
	};


	//
	// Grab the exception code.
	//

	m_exceptionCode = ep->ExceptionRecord->ExceptionCode;


	//
	// Decide what to do with the exception.
	//

	switch ( m_exceptionCode )
	{
		//
		// Let the debugger handle these.
		//

		case EXCEPTION_BREAKPOINT:
		case EXCEPTION_SINGLE_STEP:
		{
			return EXCEPTION_CONTINUE_SEARCH;
		} break;


		//
		// These exceptions might represent a logic
		// error in the test or the component under
		// test.  We should log a failure, along with
		// as much information as possible as to the
		// state of the machine at the time of the
		// failure.
		//

		case EXCEPTION_ACCESS_VIOLATION:
		case EXCEPTION_FLT_DENORMAL_OPERAND:
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		case EXCEPTION_FLT_INEXACT_RESULT:
		case EXCEPTION_FLT_INVALID_OPERATION:
		case EXCEPTION_FLT_OVERFLOW:
		case EXCEPTION_FLT_STACK_CHECK:
		case EXCEPTION_FLT_UNDERFLOW:
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
		case EXCEPTION_INT_OVERFLOW:
		{
			return EXCEPTION_EXECUTE_HANDLER;
		} break;


		//
		// We don't try to handle any other exceptions,
		// because they may require arbitrarily complex
		// cleanup and recovery, or may not even be
		// recoverable at all.  In these cases, the
		// test is in a completely indeterminate state,
		// and is very likely making matters worse than
		// just exiting.
		//

		default:
		{
			// if it's a continuable exception, we can
			// probably handle it safely
			if (ep->ExceptionRecord->ExceptionFlags == 0)
			{
				return EXCEPTION_EXECUTE_HANDLER;
			}

			return EXCEPTION_CONTINUE_SEARCH;
		} break;
	};
}

void CWGFTestFramework::CleanupTest()
{
	LogRecordedParamValues();


	if (m_bDoNotCatchExceptions)
	{
		m_pTest->LocalCleanup();
	}
	else
	{		
		__try
		{
			if ( m_Status != WGFTEST_STATUS_EXCEPTION_CAUGHT )
			{
				m_pTest->LocalCleanup();
			}
		}
		__except( ExceptionTraceFilter( GetExceptionInformation(), &s_exceptionTrace ) )
		{
			SetStatus( WGFTEST_STATUS_EXCEPTION_CAUGHT );

			if ( !m_bNoLog )
			{
				if (!m_DXGLog.InTestCase())
				{
					m_DXGLog.BeginTestCase( _T( "Exception caught." ), 0xFFFFFFFE );
				}

				WriteToLogF( "### %s", __FUNCTION__ );
				WriteToLogF( "### %s", s_exceptionTrace.GetExceptionDescString().c_str() );

				for( size_t i = 0; i < s_exceptionTrace.GetTraceDepth(); ++i )
				{
					WriteToLogF( "### %s", s_exceptionTrace.GetTrace( i ).c_str() );
				}

				m_DXGLog.AbortTest();
				m_DXGLog.EndTestCase();
			};
		}
	}	


	//
	// Log a placeholder failure for WTT if necessary.
	//

	if
	(
		// Only the HCK version of WTT still has this problem.
		(
			m_TestMode == WGFTEST_MODE_WHQL ||
			RunningUnderWLK() == true
		) &&
		m_bNoLog == false &&
		m_DXGLog.GetGroupAbort() > 0 &&
		m_DXGLog.GetGroupFail() == 0 
	)
	{
		// We have a group that should be investigated but will not show up in WTT
		// We can make WTT take notice by logging a failing test case
		assert( m_DXGLog.InTestCase() == false );
		m_DXGLog.BeginTestCase( _T( "Test group had aborts." ), 0xFFFFFFFF );
		m_DXGLog.Trace(1, _T("***  Test group had aborts but no failures. This test case is a place-holder failure for WTT.\n"));
		DWORD dwWTTResult = GetWTTResultTypeForFrameworkResultType( RESULT_FAIL );
		m_DXGLog.SetTestCaseResult( dwWTTResult );
		m_DXGLog.EndTestCase();
	}


	LogEndTestGroup();


	ClearTest( m_pTest->GetPath() ); // This will delete the cached test object
	m_pTest = NULL;
}


WGFTEST_STATUS CWGFTestFramework::GoToTestCase( CTestGroupLeafNode *pNode, size_t nNumber )
{
	// Avoid running any more tests cases if the user has aborted
	// the test or we've recovered from an exception.
	if ( IsStatusContinuable() != true )
	{
		return m_Status;
	}



	if (pNode == nullptr)
	{
		return WGFTEST_STATUS_NOT_FOUND;
	}

	bool bInScript = true;

	// Is the test case in the current playlist?
	CPlayList::Iterator it = m_pl.Find( pNode->GetPath(), nNumber );
	CPlayList::Iterator Oldplit = m_plit;
	if ( it == m_pl.End() )
	{
		// BUGBUG - Consider falling back to a default playlist
		_DPF( _T( "Test case not in current playlist.\n" ) );
		m_pl.Clear();
		m_pl.AddTestBranch( GetTestGroupNode( _T( "" ) ) );
		it = m_pl.Find( pNode->GetPath(), nNumber );
		Oldplit = m_pl.End();
	}
	
	m_Status = LocalGoToTestCase( it.GetNode(), it.GetTestCaseNumber() );
	
	if ( m_Status != WGFTEST_STATUS_OK )
	{
		switch ( m_Status )
		{
			case WGFTEST_STATUS_SKIP_TEST:
			case WGFTEST_STATUS_ABORT_TEST:
			case WGFTEST_STATUS_ABORT_TEST_USERINITIATED:
			case WGFTEST_STATUS_EXCEPTION_CAUGHT:
			{
				it.NullCase();
				m_plit = it;
				return m_Status;
			}
		}


		m_plit = Oldplit;

		return m_Status;
	}
	
	// Update the playlist iterator.
	m_plit = it;
				
	return m_Status;
}


/*
void CWGFTestFramework::PlayListReset()
{
	CTestGroupPath Path;
	size_t nCaseNumber = 0;
	
	// Record the current test case.
	if ( m_plit != m_pl.End() )
	{
		Path = m_plit.GetPath();
		nCaseNumber = m_plit.GetTestCaseNumber();
	}
	
	m_pl.Clear();
	m_pl.AddTestBranch( CTestGroupPath() );
	
	if ( nCaseNumber )
	{
		// Reset the current test case.
		m_plit = m_pl.Find( Path, nCaseNumber );
	}
	else
	{
		m_plit = m_pl.End();
	}
}
*/


void CWGFTestFramework::PlayListClear()
{
	m_pl.Clear();
	m_plit = m_pl.End();
}


void CWGFTestFramework::PlayListAddBranch( const CTestGroupPath &Path )
{
	m_pl.AddTestBranch( Path );

	if ( m_plit == m_pl.End() )
	{
		m_plit = m_pl.Begin();
	}
}


void CWGFTestFramework::PlayListAddCaseRange( const CTestGroupPath &Path, size_t nFirst /* = 1 */, size_t nLast /* = CPlayList::RANGE_END  */)
{
	const CTestGroupNode *pNode = GetTestGroupNode( Path );
	assert( pNode->IsTestNode() );
	m_pl.AddTestCases( (CTestGroupLeafNode *) pNode, nFirst, nLast );

	if ( m_plit == m_pl.End() )
	{
		m_plit = m_pl.Begin();
	}
}


WGFTEST_STATUS CWGFTestFramework::ResetTestCase()
{
	if ( m_bTestCaseInitialized )
	{
		CleanupTestCase();
	}


	// Avoid running any more tests cases if the user has aborted
	// the test or we've recovered from an exception.
	if ( IsStatusContinuable() != true )
	{
		return m_Status;
	}


	if ( m_plit == m_pl.End() )
	{
		SetStatus( WGFTEST_STATUS_NOT_FOUND );
		return m_Status;
	}
			
	assert( m_pTest == m_plit.GetNode()->GetTest() );
	if ( !m_pTest->GenerateVariation( m_plit.GetTestCaseNumber() ) )
	{
		m_plit = m_pl.End();
		return WGFTEST_STATUS_NOT_FOUND;
	}


	m_LastResult = RESULT_PENDING;
	return m_Status = WGFTEST_STATUS_OK;
}


WGFTEST_STATUS CWGFTestFramework::SkipToNextTestGroup()
{
	if ( m_pTest && m_bTestCaseInitialized )
	{
		CleanupTestCase();
	}


	// Avoid running any more tests cases if the user has aborted
	// the test or we've recovered from an exception.
	if ( IsStatusContinuable() != true )
	{
		return m_Status;
	}


	CPlayList::Iterator plit = m_plit;
	
	// Find the first case in the next group.
	for( plit.NextTest(); plit != m_pl.End(); plit.NextTest() )
	{
		for( ; plit != m_pl.End(); plit.NextCase() )
		{
			m_Status = LocalGoToTestCase( plit.GetNode(), plit.GetTestCaseNumber() );

			switch ( m_Status )
			{
				case WGFTEST_STATUS_OK:
				{
					m_plit = plit;
					return m_Status;
				} break;


				case WGFTEST_STATUS_SKIP_TEST:
				case WGFTEST_STATUS_ABORT_TEST:
				case WGFTEST_STATUS_ABORT_TEST_USERINITIATED:
				case WGFTEST_STATUS_EXCEPTION_CAUGHT:
				{
					plit.NullCase();
					m_plit = plit;
					return m_Status;
				} break;
			}
		}
		SetStatus( WGFTEST_STATUS_NOT_FOUND );
		return m_Status;
	}

	SetStatus( WGFTEST_STATUS_NOT_FOUND );
	return m_Status;
}


WGFTEST_STATUS CWGFTestFramework::SkipToPrevTestGroup()
{
	if ( m_pTest && m_bTestCaseInitialized )
	{
		CleanupTestCase();
	}


	// Avoid running any more tests cases if the user has aborted
	// the test or we've recovered from an exception.
	if ( IsStatusContinuable() != true )
	{
		return m_Status;
	}


	// make sure that the current iterator is a valid test
	if ( m_plit == m_pl.End() )
	{
		if ( m_pl.Begin() == m_pl.End() )
		{
			m_Status = WGFTEST_STATUS_NOT_FOUND;
		}
		else
		{
			m_plit.PrevTest();
			m_Status = WGFTEST_STATUS_OK;
		}

		return m_Status;
	}
	
	CPlayList::Iterator plit = m_plit;
	// First attempt to go the first unfiltered test case in the current group
	// if the test case is lower than the current.
	plit.BeginCases();
	
	if ( plit.GetTestCaseNumber() < m_plit.GetTestCaseNumber() )
	{
		m_plit = plit;
		return m_Status = WGFTEST_STATUS_OK;
	}

	if ( plit != m_pl.Begin() )
	{	
		for( bool bContinue = true; bContinue; )
		{
			plit.PrevTest();

			if ( plit == m_pl.Begin() )
				bContinue = false;

			CTestGroupLeafNode *pNode = plit.GetNode();
			
			for( ; plit.GetNode() == pNode; plit.NextCase() )
			{
				m_Status = LocalGoToTestCase( plit.GetNode(), plit.GetTestCaseNumber() );

				switch ( m_Status )
				{
					case WGFTEST_STATUS_OK:
					{
						m_plit = plit;
						return m_Status;
					} break;


					case WGFTEST_STATUS_SKIP_TEST:
					case WGFTEST_STATUS_ABORT_TEST:
					case WGFTEST_STATUS_ABORT_TEST_USERINITIATED:
					case WGFTEST_STATUS_EXCEPTION_CAUGHT:
					{
						plit.NullCase();
						m_plit = plit;
						return m_Status;
					} break;
				}
			}
		}
	}

		
	SetStatus( WGFTEST_STATUS_NOT_FOUND );
	return m_Status;
}


const CTestGroupNode *CWGFTestFramework::GetTestGroupNode( const CTestGroupPath &Path )
{
	if ( Path.Empty() )
	{
		return &m_Root;
	}


	return m_Root.GetSubGroupNode( Path );
}


WGFTEST_STATUS CWGFTestFramework::OnBeyondLastTestCase()
{
	// Save off this code coverage info
	if (IsCodeCoverage())
	{
		TCHAR tcsDesc[256] = {0};
		TCHAR sConfig[256];

		if (GetTestType() == TESTTYPE_CONF)
		{
			_stprintf(sConfig, _T(""));
		}
		else if (GetTestType() == TESTTYPE_PSGP)
		{
			_stprintf(sConfig, _T(""));

			/* BUGBUG -stubbing device info requirement
			_stprintf(sConfig, _T("PSGP-%s,"), m_DeviceInfo.bPSGP ? LogGetSysInfo().ProcInf.sPipeline : _T("MS"));
			*/
		}
		else
		{
			_stprintf(sConfig, _T(""));
		}
		
		_stprintf(tcsDesc, _T("Multimedia-DirectX-Direct3D IM"));
		/*
		_stprintf(tcsDesc, _T("Multimedia-DirectX-Direct3D IM-%s:%s%s"), m_LogInfo.sTestName, sConfig, 
					m_DeviceInfo.sDevice);
					*/
		CoverageSave(tcsDesc, NULL);
	}

	// Reset the code coverage info
	if (IsCodeCoverage())
	{
		CoverageReset(NULL);
	}

	
	return WGFTEST_STATUS_NOT_FOUND;
}


WGFTEST_STATUS CWGFTestFramework::OnBeyondFirstTestCase()
{
	// Save off this code coverage info
	if (IsCodeCoverage())
	{
		TCHAR sConfig[256];
		TCHAR tcsDesc[256] = {0};

		if (GetTestType() == TESTTYPE_CONF)
		{
			_stprintf(sConfig, _T(""));
		}
		else if (GetTestType() == TESTTYPE_PSGP)
		{
			_stprintf(sConfig, _T(""));

			/* BUGBUG -stubbing device info requirement
			_stprintf(sConfig, _T("PSGP-%s,"), m_DeviceInfo.bPSGP ? LogGetSysInfo().ProcInf.sPipeline : _T("MS"));
			*/
		}
		else
		{
			_stprintf(sConfig, _T(""));
		}

		_stprintf(tcsDesc, _T("Multimedia-DirectX-Direct3D IM"));
		/*
		_stprintf(tcsDesc, _T("Multimedia-DirectX-Direct3D IM-%s:%s%s"), m_LogInfo.sTestName, sConfig, 
					m_DeviceInfo.sDevice);
					*/
		CoverageSave(tcsDesc, NULL);
	}

	// Reset the code coverage info
	if (IsCodeCoverage())
	{
		CoverageReset(NULL);
	}


	return WGFTEST_STATUS_OK;
}


bool CWGFTestFramework::UpdateCurrentDeviceInfo(DXGLOG_DEVICEINFO * pDeviceInfo)
{
	return false;
}


TEST_RESULT CWGFTestFramework::LocalPreExecuteTestCase()
{
	TEST_RESULT tr = RESULT_NONE;

	// Prevent screensavers from kicking in on our test.
	if ( m_bAllowIdleDetection != true )
	{
		PulseScreensaverTimeout();
	};

	if ( m_pTest )
	{
		tr = RESULT_PASS;
		
		if ( !m_bTestCaseInitialized )
		{
			// Increment the retry count if necessary and get the current RETRY phase (based on the count)
			if ( RESULT_RETRY == m_LastResult )
			{
				m_RetryCurrent++;
			}

			// We're clearing the log of RETRY results unless we've requested to log all
			// Any other result status indicates we are starting a new test case
			if ( RESULT_RETRY == m_LastResult && !m_bInsideRetrySection )
			{
				wstring wsRetryPhase = L"";
				wstring wsRetryCurrent = L"";
				if ( !FromString( &wsRetryPhase, ToString(GetRetryPhase()) ) )
				{
					WriteToLogF( _T("FromString() failed for the retry phase's name.") );
					m_LastResult = RESULT_FAIL;
					return m_LastResult;
				}
				if ( !FromString( &wsRetryCurrent, ToString(m_RetryCurrent) ) )
				{
					WriteToLogF( _T("FromString() failed for the retry phase's number.") );
					m_LastResult = RESULT_FAIL;
					return m_LastResult;
				}

				std::wstring sectionName = L"Retry #" + wsRetryCurrent + L" - " + wsRetryPhase;
				BeginSection( sectionName.c_str() );
				m_bInsideRetrySection = true;
			}
			else
			{
				LogBeginTestCase( m_testCaseName.c_str(), (UINT) m_plit.GetTestCaseNumber() );
			}

			// See if we need to reset the test
			if ( RESULT_RETRY == m_LastResult && GetRetryPhase() == RETRY_PHASE_RESET_AND_RETRY )
			{
				// Call the test's cleanup
				if ( m_pTest )
				{
					m_pTest->Cleanup();
				}

				TEST_RESULT setupTestResult = RESULT_FAIL;
				TrySetupTest( setupTestResult );	// Will call refresh graphics and the test's Setup()
				if ( RESULT_PASS != setupTestResult )
				{
					WriteToLogF( _T("During a retry attempt, RefreshGraphics() and/or the test's Setup() failed.") );
					m_LastResult = RESULT_FAIL;
				}
			}

			// Dump parameter values to the log
			for ( int n = 0; n < m_pTest->GetNumParameters(); ++n )
			{
				CTestCaseParameterBase * pParameter = m_pTest->GetTestCaseParameter( n );
				if ( pParameter->IsActive() || pParameter->DefaultValueWasSet() )
				{
					const tstring & strName = pParameter->GetName();
					tstring strValue = pParameter->GetValueAsString();

					if ( m_TestMode == WGFTEST_MODE_WHQL )
					{
						WriteToLogF
						(
							1,
							_T( "%s = %s" ),
							strName.c_str(),
							strValue.c_str()
						);
					};

					m_DXGLog.LogTestParameter
					(
						strName.c_str(),
						strValue.c_str()
					);
				};
			};


			tr = PreSetupTestCase();
			
			if ( tr == RESULT_PASS )
			{
				if (m_bDoNotCatchExceptions)
				{
					tr = m_pTest->SetupTestCase();
				}
				else
				{
					TrySetupTestCase(m_pTest, tr);
				}


				switch ( tr )
				{
					case RESULT_ABORT:
					case RESULT_BLOCKED:
					case RESULT_FAIL:
					{
						assert( m_DXGLog.InTestCase() == true );
						WriteToLog( _T( "SetupTestCase failed." ) );
					} break;
				}


				if ( tr != RESULT_PASS )
				{
					CleanupTestCase();
				}
				else
				{				
					m_bTestCaseInitialized = true;				
				}


				// Avoid running any more tests cases if the user
				// has aborted the test or we've recovered from
				// an exception.
				if ( IsStatusContinuable() != true )
				{
					tr = RESULT_ABORT;
					goto Cleanup;
				}
			}
		}
	}


Cleanup:	
	return tr;
}


void CWGFTestFramework::CleanupTestCase()
{
	if ( m_pTest )
	{
		if (m_bDoNotCatchExceptions)
		{
			m_pTest->CleanupTestCase();

			m_bTestCaseInitialized = false;				
			PostCleanupTestCase();
		}
		else
		{		
			__try
			{
				if ( m_Status != WGFTEST_STATUS_EXCEPTION_CAUGHT )
				{
					m_pTest->CleanupTestCase();
				}

				m_bTestCaseInitialized = false;				
				if ( m_Status != WGFTEST_STATUS_EXCEPTION_CAUGHT )
				{
					PostCleanupTestCase();
				}
			}
			__except( ExceptionTraceFilter( GetExceptionInformation(), &s_exceptionTrace ) )
			{
				LogException(s_exceptionTrace);
				m_LastResult = RESULT_ABORT;
			}

			m_bTestCaseInitialized = false;				
		}	
	}
}

void CWGFTestFramework::TrySetupTest(TEST_RESULT& tr)
{
	__try
	{
		if ( m_Status != WGFTEST_STATUS_EXCEPTION_CAUGHT )
		{
			tr = SetupTest();
			if( RESULT_RETRY == tr )
			{
				WriteToLogF( _T("It is invalid for a test to return RESULT_RETRY. This is a reserved framework status.") );
				tr = RESULT_FAIL;
			}
		}
		else
		{
			tr = RESULT_ABORT;
		}
	}
	__except( ExceptionTraceFilter( GetExceptionInformation(), &s_exceptionTrace ) )
	{
		SetStatus( WGFTEST_STATUS_EXCEPTION_CAUGHT );

		WriteToLogF( "### %s", __FUNCTION__ );
		WriteToLogF( "### %s", s_exceptionTrace.GetExceptionDescString().c_str() );

		for( size_t i = 0; i < s_exceptionTrace.GetTraceDepth(); ++i )
		{
			WriteToLogF( "### %s", s_exceptionTrace.GetTrace( i ).c_str() );
		}

		tr = RESULT_ABORT;
	}
}


void CWGFTestFramework::TrySetupTestCase(CTest* pTest, TEST_RESULT& tr)
{
	__try
	{
		if ( m_Status != WGFTEST_STATUS_EXCEPTION_CAUGHT )
		{
			tr = pTest->SetupTestCase();
		}
		else
		{
			tr = RESULT_ABORT;
		}
	}
	__except( ExceptionTraceFilter( GetExceptionInformation(), &s_exceptionTrace ) )
	{
		SetStatus( WGFTEST_STATUS_EXCEPTION_CAUGHT );

		WriteToLogF( "### %s", __FUNCTION__ );
		WriteToLogF( "### %s", s_exceptionTrace.GetExceptionDescString().c_str() );

		for( size_t i = 0; i < s_exceptionTrace.GetTraceDepth(); ++i )
		{
			WriteToLogF( "### %s", s_exceptionTrace.GetTrace( i ).c_str() );
		}

		tr = RESULT_ABORT;
	}
}


TEST_RESULT CWGFTestFramework::IterateTestCase()
{
	TEST_RESULT LastResult;
#if !defined(_ARM_) && !defined(_ARM64_)
	if ( m_sfi.IsInitialized() )
	{
		
		//once to cause any per-process, per-DLL-load code to be executed before counting SFI cases
		LastResult = m_pTest->ExecuteTestCase();

		m_sfi.SetCountingMode();

		{
			m_pTest->ExecuteTestCase();
		}

		LONG CaseCount = m_sfi.CaseCount();
		for( LONG i = 0; i < CaseCount; ++i )
		{
			m_sfi.SetCase(i);
			{
				LastResult = m_pTest->ExecuteTestCase();
			}
		}
		m_sfi.SetCountingMode();

	} else
#endif //_ARM_
	{
		LastResult = m_pTest->ExecuteTestCase();
	}
	return LastResult;
}


TEST_RESULT CWGFTestFramework::ExecuteTestCase()
{
	//
	// Avoid running any more tests cases if the user has aborted
	// the test or we've recovered from an exception.
	//

	if ( IsStatusContinuable() != true )
	{
		return RESULT_ABORT;
	}


	//
	// Don't actually run the test if we're just building a test
	// replay file.
	//

	if ( IsGenerateCSVOnly() )
	{
		m_LastResult = RESULT_PASS;
		return RESULT_PASS;
	}


	//
	// Seed the random number generator based on the test case name
	// and number, which should provide reproducible results.
	//

	srand( m_plit.GetTestCaseNumber() + m_testCaseName.size() );


	//
	// Save off floating-point unit state.
	//

	m_fpuStateCheck.InitFPUStateCheck();


	//
	// Run pre-ExecuteTestCase code including the SetupTestCase method.
	//

	m_LastResult = LocalPreExecuteTestCase();


	//
	// Run the ExecuteTestCase method.
	//

	if ( RESULT_PASS == m_LastResult )
	{
		m_fLastTestConformance = -1.f;
		if ( m_bDoNotCatchExceptions )
		{
			m_LastResult = IterateTestCase(); 
		}
		else
		{		
			__try
			{
				if ( m_Status != WGFTEST_STATUS_EXCEPTION_CAUGHT )
				{
					m_LastResult = IterateTestCase(); 
				}
				else
				{
					m_LastResult = RESULT_ABORT;
				}
			}
			__except( ExceptionTraceFilter( GetExceptionInformation(), &s_exceptionTrace ) )
			{
				SetStatus( WGFTEST_STATUS_EXCEPTION_CAUGHT );

				WriteToLogF( "### %s", __FUNCTION__ );
				WriteToLogF( "### %s", s_exceptionTrace.GetExceptionDescString().c_str() );

				for( size_t i = 0; i < s_exceptionTrace.GetTraceDepth(); ++i )
				{
					WriteToLogF( "### %s", s_exceptionTrace.GetTrace( i ).c_str() );
				}

				m_LastResult = RESULT_ABORT;
			}
		}			
	}

	if ( RESULT_PASS == m_LastResult )
	{
		m_LastResult = m_pTest->PostExecuteTestCase();
	}


	//
	// Check for floating-point unit state corruption.
	//

	UINT uiFPUCtrlWord = 0;
	UINT uiFPUCtrlReg = 0;
	if ( !m_fpuStateCheck.IsFPUStateRight( uiFPUCtrlWord, uiFPUCtrlReg ) )
	{
		m_LastResult = RESULT_FAIL;

		// these 3 statements can be used by filters.
		WriteToLogF( "Error: FPU state changed during testing. The first 3 Error messages could be used by filters." );
		WriteToLogF( "FPU Control Word - changed bits:  0x%08x(0x00000000 expected).", uiFPUCtrlWord ^ m_fpuStateCheck.PreFPUCtrlWord() );
		WriteToLogF( "FPU Control Register - changed bits:  0x%08x(0x00000000 expected).", uiFPUCtrlReg ^ m_fpuStateCheck.PreFPUCtrlReg() );

		WriteToLogF( "FPU Control Word before testing: 0x%08x", m_fpuStateCheck.PreFPUCtrlWord());
		WriteToLogF( "FPU Control Word after testing:  0x%08x", uiFPUCtrlWord);
		WriteToLogF( "FPU Control Register before testing: 0x%08x.", m_fpuStateCheck.PreFPUCtrlReg());
		WriteToLogF( "FPU Control Register after testing:  0x%08x.", uiFPUCtrlReg);
	}


	//
	// Run the CleanupTestCase method.
	//

	if ( IsStatusContinuable() == true )
	{
		CleanupTestCase();
	}

	if ( RESULT_RETRY == m_LastResult )
	{
		WriteToLogF( _T("It is invalid for a test to return RESULT_RETRY. This is a reserved framework status.") );
		m_LastResult = RESULT_FAIL;
	}
	else if ( RESULT_FAIL == m_LastResult && m_RetryCurrent < m_Retries )
	{
		// Retry test case if failed, up to the specified number of times
		m_LastResult = RESULT_RETRY;
	}

	//
	// Log the test case result and the end of the test case.
	// If this is a RETRY result, we only want to log in combination with a -LogAll or -LogAllFailures option

	if ( ( m_DXGLog.InTestCase() == true || m_bNoLog == true ) && ( RESULT_RETRY != m_LastResult ) )
	{
		LogTestCaseResult();
		LogEndTestCase();
	}
	else if ( RESULT_RETRY == m_LastResult )
	{
		// Discard previous section?
		WriteToLogF( _T("") );	// A visual space between sections in the log
		if( m_bInsideRetrySection )
		{
			EndSection( LogAllFailures() == true ); // Only keep the RETRY sections if LogAll is requested
			m_bInsideRetrySection = false;
		}
	}

	//
	// Update the framework's status.
	//
	
	switch( m_LastResult )
	{
		case RESULT_PASS:
		case RESULT_PENDING:
		case RESULT_RETRY:
		{
			SetStatus( WGFTEST_STATUS_OK );
		} break;


		case RESULT_ABORT:
		{
			SetStatus( WGFTEST_STATUS_ABORT_TEST );
		} break;


		case RESULT_SKIP:
		{
			SetStatus( WGFTEST_STATUS_SKIP_TEST_CASE );
		} break;			
	}


	//	
	// BUGBUG - TODO:  Have the framework return a distinct
	// exit code based on the status.
	//

	return m_LastResult;
}	


void CWGFTestFramework::RecordParamValues( RESULT_CODE result )
{
	// For each registered test parameter index...
	for( int n = -1 * m_pTest->GetNumPrivateParameters(); n < m_pTest->GetNumParameters(); ++n )
	{
		// ...get the parameter...
		CTestCaseParameterBase * pParameter = m_pTest->GetTestCaseParameter( n );

		// ...and if the parameter is inactive and not set to a default value...
		if ( !pParameter->IsActive() && !pParameter->DefaultValueWasSet())
		{
			// ...then skip it.
			continue;
		};

		// ...or if the parameter is private and not an output parameter
		if ( pParameter->IsPrivate() && !pParameter->IsOutputParameter() )
		{
			// ...then also skip it.
			continue;
		}

		// Get the parameter name and value.
		tstring strName = pParameter->GetName();

		if(pParameter->IsOutputParameter())
			strName = tstring(_T("[Output] ")) + strName;

		tstring strValue = pParameter->GetValueAsString();


		// Record statistics for this test parameter.
		ParameterValueToResultToCountMap::iterator allParamValuesIterator = m_AllParamValues[ tstr2str(strName) ].find( tstr2str(strValue) );
		if ( allParamValuesIterator != m_AllParamValues[ tstr2str(strName) ].end() )
		{
			ResultToCountMap RollupMap = allParamValuesIterator->second;
			ResultToCountMap::iterator rollupIt = RollupMap.find( GetWTTResultTypeForFrameworkResultType( result ) );
			if ( rollupIt != RollupMap.end() )
			{
				rollupIt->second += 1;
			}
			else
			{
				RollupMap[ GetWTTResultTypeForFrameworkResultType( result ) ] = 1;
			};
			allParamValuesIterator->second = RollupMap;
		}
		else
		{
			ResultToCountMap RollupMap;
			RollupMap[ GetWTTResultTypeForFrameworkResultType( result ) ] = 1;
			m_AllParamValues[ tstr2str(strName) ][ tstr2str(strValue) ] = RollupMap;
		};


		// For each subsequent registered test parameter index...
		for
		(
			int k = n + 1;
			k < m_pTest->GetNumParameters();
			++k
		)
		{
			// ...get the test parameter.
			CTestCaseParameterBase * pParameter2 = m_pTest->GetTestCaseParameter( k );

			// ...and if the parameter is inactive...
			if ( !pParameter2->IsActive() )
			{
				// ...then skip it.
				continue;
			};

			// Get the parameter name and value.
			const tstring & strName2 = pParameter2->GetName();
			tstring strValue2 = pParameter2->GetValueAsString();
			tstring indexStr2 = strName2 + _T("=") + strValue2;
			tstring indexStr1 = strName + _T("=") + strValue;

			ParameterNameToValueToResultToCountMap & paramMap = m_PairAllParamValues;

			for
			(
				int x = 0;
				x < 2;
				++x
			)
			{
				tstring & index1 = (0 == x) ? indexStr1 : indexStr2;
				tstring & index2 = (0 == x) ? indexStr2 : indexStr1;

				ParameterValueToResultToCountMap & valuesMap = paramMap[ tstr2str(index1) ];
				ParameterValueToResultToCountMap::iterator paramIt = valuesMap.find( tstr2str(index2) );
				if ( paramIt != valuesMap.end() )
				{
					ResultToCountMap RollupMap = paramIt->second;
					ResultToCountMap::iterator rollupIt = RollupMap.find( GetWTTResultTypeForFrameworkResultType( result ) );
					if ( rollupIt != RollupMap.end() )
					{
						rollupIt->second += 1;
					}
					else
					{
						RollupMap[ GetWTTResultTypeForFrameworkResultType( result ) ] = 1;
					};
					paramIt->second = RollupMap;
				}
				else
				{
					ResultToCountMap RollupMap;
					RollupMap[ GetWTTResultTypeForFrameworkResultType( result ) ] = 1;
					valuesMap[ tstr2str(index2) ] = RollupMap;
				};
			};
		};
	};
};


void CWGFTestFramework::LogRecordedParamValues()
{
	m_DXGLog.LogTestGroupFailureBreakdown
	(
		(DXGLOG_FAILURE_BREAKDOWN_TYPE) m_nFailureSummaryLevel,
		m_AllParamValues,
		m_PairAllParamValues
	);
};


void CWGFTestFramework::LogTestCaseResult()
{
	RESULT_CODE result = m_LastResult;

	// Dump output parameter values to the log (private parameter indices are negative)
	for ( int n = 0; n < m_pTest->GetNumPrivateParameters(); ++n )
	{
		// Get private parameter at index n
		CTestCaseParameterBase * pParameter = m_pTest->GetPrivateTestCaseParameter( n );

		if (pParameter->IsOutputParameter() && pParameter->IsActive() )
		{
			tstring strName = tstring(_T("[Output] ")) + pParameter->GetName();
			tstring strValue = pParameter->GetValueAsString();

			if ( m_TestMode == WGFTEST_MODE_WHQL )
			{
				WriteToLogF
				(
					1,
					_T( "%s = %s" ),
					strName.c_str(),
					strValue.c_str()
				);
			};

			m_DXGLog.LogTestParameter
			(
				strName.c_str(),
				strValue.c_str()
			);
		};
	};



	RecordParamValues( result );

	switch ( result )
	{
		case RESULT_PASS:
		{
			++m_nPassCount;
		} break;

		case RESULT_ABORT:
		case RESULT_BLOCKED:
		{
			++m_nAbortCount;
		} break;

		case RESULT_FAIL:
		{
			++m_nFailCount;
		} break;

		case RESULT_SKIP:
		{
			++m_nSkipCount;
		} break;
	};

	if ( !m_bNoLog )
	{
		DWORD dwWTTResult = GetWTTResultTypeForFrameworkResultType( result );
		m_DXGLog.SetTestCaseResult( dwWTTResult );
	};
};


int CWGFTestFramework::BeginCSV()
{
	assert(m_pTest);

	const CTestGroupPath &currentTest = GetCurrentTestPath();

	// Make sure there is no open CSV file
	assert( m_pCSVFile == NULL );

	// name file after test group, with underscores instead of backslashes
	tstring szTest = currentTest.ToString();
	for( UINT i = 0; i < szTest.length(); ++i )
	{
		if ( szTest[i] == '\\' )
		{
			szTest[i] = '_';
		}
	}

	szTest = tstring(szTest + _T(".csv"));
	m_pCSVFile = NULL;

	if ( _tfopen_s(&m_pCSVFile, szTest.c_str(), _T( "w" ) ) != 0 )
		return -1;

	// get test group and create header row
	tstring szRow = _T("Test Number, ");
	UINT numParameters = m_pTest->GetNumParameters();
	for( UINT i = 0; i < numParameters; ++i )
	{
		szRow = tstring(szRow + m_pTest->GetTestCaseParameter(i)->GetName() + _T(", "));
	}
	szRow = tstring(szRow + _T("Priority Value, Priority Level\n"));

	fwrite(szRow.c_str(), sizeof(TCHAR), szRow.length(), m_pCSVFile);

	return 0;
}


void CWGFTestFramework::WriteVariationCSV(size_t nNumber)
{
	assert(m_pCSVFile);
	assert(m_pTest);

	UINT numParameters = m_pTest->GetNumParameters();
	tstring szRow = ToString(nNumber) + _T(", ");

	for( UINT param = 0; param < numParameters; ++param )
	{
		szRow = tstring(szRow + m_pTest->GetTestCaseParameter(param)->GetValueAsString() + _T(", "));
	}


	float priorityValue = m_pTest->GetVariationPriorityValue(nNumber);
	int priorityLevel = m_pTest->GetVariationPriorityLevel(nNumber);
	szRow = tstring(szRow + ToString(priorityValue) + _T(", ") + ToString(priorityLevel) + _T("\n"));
	fwrite(szRow.c_str(), sizeof(TCHAR), szRow.length(), m_pCSVFile);
}


void CWGFTestFramework::EndCSV()
{
	if (m_pCSVFile)
	{
		fclose(m_pCSVFile);
		m_pCSVFile = NULL;
	}
}


#if 0
//////////////////////////////////////////////////////////////////////////
// CTestCaseExecuteSet
//////////////////////////////////////////////////////////////////////////
const size_t CTestCaseExecuteSet::RangeLast = (size_t) 0 - 1;

void CTestCaseExecuteSet::SetCasesString( const tstring &strCases )
{
	m_Ranges.clear();
	
	if ( strCases.empty() )
		return;
	
	for( size_t nOffset = 0; nOffset != tstring::npos; )
	{
		CaseRange range;
		
		// Get range description string.
		size_t nStart = strCases.find_first_of( _T( ',' ), nOffset );
		size_t nSize = nStart == tstring::npos ? 0 : nStart - nOffset;
		tstring strRange = strCases.substr( nOffset, nStart - nOffset );
		nOffset = nStart == tstring::npos ? tstring::npos : nStart + 1;
		
		// Skip leading whitespace.
		nStart = strRange.find_first_not_of( _T( " \t" ) );
		
		if ( nStart == tstring::npos )
			continue;
			
		// Get the minimum range value.
		size_t nEnd = strRange.find_first_not_of( _T( "xX0123456789abcdefABCDEF" ), nStart );
		if ( nEnd == nStart )
			continue;

		nSize = nEnd == tstring::npos ? tstring::npos : nEnd - nStart;			
		tstring strNumber = strRange.substr( nStart, nSize );
		if ( !FromString( &range.nMin, strNumber ) )
			continue;
			
		range.nMax = range.nMin; // Copy nMin to nMax by default.
		
		if ( nEnd != tstring::npos )
		{
			// Skip more whitespace
			nStart = strRange.find_first_not_of( _T( " \t" ), nEnd );
			
			if ( nStart != tstring::npos )
			{
				// Find the '-' symbol
				if ( strRange[nStart] != _T( '-' ) )
					continue;
				
				// And past the whitespace
				nStart = strRange.find_first_not_of( _T( " \t" ), ++nStart );
				
				// Get the maximum range value.
				nEnd = strRange.find_first_not_of( _T( "xX0123456789abcdefABCDEF" ), nStart );
				if ( nEnd == nStart )
					continue;
					
				nSize = nEnd == tstring::npos ? tstring::npos : nEnd - nStart;			
				strNumber = strRange.substr( nStart, nSize );
				if ( !FromString( &range.nMax, strNumber ) )
					continue;
				
				// Make sure there is nothing else in the range string.
				nStart = strRange.find_first_not_of( _T( " \t" ), nEnd );
				if ( nStart != tstring::npos )
					continue; // Why don't they learn?
			}
		}
		
		AddCaseRange( range );
	}
}


void CTestCaseExecuteSet::AddCaseRange( const CaseRange &range )
{
	CaseRange rCopy = range;
	
	if ( rCopy.nMin > rCopy.nMax )
	{
		// Swap range values.
		size_t nTemp = rCopy.nMin;
		rCopy.nMin = rCopy.nMax;
		rCopy.nMax = nTemp;
	}
	
	// Remove and replace overlapping and adjacent ranges.
	for( RangesType::iterator it = m_Ranges.begin(); it != m_Ranges.end(); )
	{
		CaseRange &ref = *it;
		
		if ( ref.nMax + 1 >= rCopy.nMin && ref.nMin - 1 <= rCopy.nMax )
		{
			rCopy.nMin = min( ref.nMin, rCopy.nMin );
			rCopy.nMax = max( ref.nMax, rCopy.nMax );
			it = m_Ranges.erase( it );
		}
		else
		{
			++it;
		}
	}
	
	// Add the range to the range set
	m_Ranges.insert( rCopy );
};


void CTestCaseExecuteSet::GetCasesString( tstring &strOut ) const
{
	strOut = _T( "" );
	
	for( RangesType::const_iterator it = m_Ranges.begin(); it != m_Ranges.end(); ++it )
	{
		if ( !strOut.empty() )
			strOut += _T( ',' );
			
		const CaseRange &range = *it;
		if ( range.nMin == range.nMax )
		{
			strOut += ToString( range.nMin );
		}
		else
		{
			strOut = strOut + ToString( range.nMin ) + _T( '-' ) + ToString( range.nMax );
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// CTestPlayList
//////////////////////////////////////////////////////////////////////////
CTestPlayList::~CTestPlayList()
{
	Clear();
}


CTestCaseExecuteSet *CTestPlayList::AddTestCaseExecuteSet( const CTestGroupPath &Path, const tstring &strCases )
{
	CTestCaseExecuteSet *pxs = new CTestCaseExecuteSet( Path );
	pxs->SetCasesString( strCases );
	m_ExecuteSets.push_back( pxs );
	
	return pxs;
}


void CTestPlayList::Clear()
{
	for( size_t n = 0; n < m_ExecuteSets.size(); ++n )
		delete( m_ExecuteSets[n] );
		
	m_ExecuteSets.clear();
}
#endif


//////////////////////////////////////////////////////////////////////////
// CPlayList
//////////////////////////////////////////////////////////////////////////
const size_t CPlayList::RANGE_END = size_t( -1 );

void CPlayList::CaseRangeSet::AddRange( size_t nFirst, size_t nLast )
{
	bool bIntersect = false;
	
	// Iterate through the array of ranges and insert the specified range
	// in order.
	size_t nIndex;
	RangesType::iterator it;
	for( it = Ranges.begin(); it != Ranges.end(); ++it )
	{
		CaseRange &range = *it;
		
		if ( nFirst >= range.nFirst )
		{
			if ( nFirst <= range.nLast + 1 || range.nLast == RANGE_END )
			{
				// Ranges intersect.  Update range in array.
				if ( nLast > range.nLast )
					range.nLast = nLast;
				
				bIntersect = true;
				break;
			}
		}
		else
		if ( nLast >= range.nFirst - 1 )
		{
			// Ranges intersect.  Update range in array.
			range.nFirst = nFirst;
			
			if ( nLast > range.nLast )
				range.nLast = nLast;
			
			bIntersect = true;
			break;
		}
	}
	
	if ( bIntersect )
	{
		CaseRange &range = *it;
		
		// See if we intersect the next range.
		RangesType::iterator nit = ++it;
		for( ; nit != Ranges.end(); ++nit )
		{
			CaseRange &next = *nit;
			
			if ( range.nLast < next.nFirst - 1 )
				break;
				
			if ( range.nLast < next.nLast )
				range.nLast = next.nLast;
		}
		
		Ranges.erase( it, nit );
	}
	else
	{	
		// Insert range into the array.
		for( ; it != Ranges.end(); ++it )
		{
			CaseRange r = *it;
			*it = CaseRange( nFirst, nLast );
			nFirst = r.nFirst;
			nLast = r.nLast;
		}
		
		Ranges.push_back( CaseRange( nFirst, nLast ) );
	}
}


void CPlayList::CaseRangeSet::RemoveRange( size_t nFirst, size_t nLast )
{
	bool bIntersect = false;
	
	if ( nFirst > nLast )
	{
		size_t nTemp = nLast;
		nLast = nFirst;
		nFirst = nTemp;
	}
	
	// Iterate through the array of ranges and remove the specified range
	size_t nIndex;
	RangesType::iterator it;
	
	// Remove ranges intersecting [nFirst,nLast]
	for( it = Ranges.begin(); it != Ranges.end(); )
	{
		CaseRange &range = *it;
		
		if ( nFirst <= range.nLast && nLast >= range.nFirst )
		{
			// Ranges intersect
			if ( nFirst <= range.nFirst )
			{
				if ( nLast >= range.nLast )
				{
					// Remove entire range from list
					it = Ranges.erase( it );
				}
				else
				{
					// Crop the range start
					range.nFirst = nLast + 1;
				}
			}
			else
			if ( nLast >= range.nLast )
			{
				// Crop the range end
				range.nLast = nFirst - 1;
				return;
			}
			else
			{
				// Split the range in two
				size_t nTemp = nFirst;
				nFirst = nLast == RANGE_END ? RANGE_END : nLast + 1;
				nLast = range.nLast;
				range.nLast = nTemp - 1;

				// Insert new range into the array.
				for( ++it; it != Ranges.end(); ++it )
				{
					CaseRange r = *it;
					*it = CaseRange( nFirst, nLast );
					nFirst = r.nFirst;
					nLast = r.nLast;
				}
				
				Ranges.push_back( CaseRange( nFirst, nLast ) );
				return;
			}
		}
		else
		{
			++it;
		}
	}
}


size_t CPlayList::CaseRangeSet::GetCaseRangeFirst( size_t nIndex ) const
{
	if ( Ranges.size() == 0 )
		return 0;

	return Ranges[nIndex].nFirst;
}


size_t CPlayList::CaseRangeSet::GetCaseRangeLast( size_t nIndex ) const
{
	if ( Ranges.size() == 0 )
		return 0;
		
	return Ranges[nIndex].nLast;
}


void CPlayList::Iterator::BeginCases()
{
	m_nCaseRangeIndex = 0;
	m_nCaseIndex = 0;
}


void CPlayList::Iterator::EndCases()
{
	m_nCaseRangeIndex = GetNumCaseRanges() - 1;
	m_nCaseIndex = GetCaseRangeLast( m_nCaseRangeIndex ) - GetCaseRangeFirst( m_nCaseRangeIndex );
}


void CPlayList::Iterator::NullCase()
{
	m_nCaseRangeIndex = (size_t) -1;
	m_nCaseIndex = (size_t) -1;
}


void CPlayList::Iterator::NextCase()
{
	if ( m_nCaseIndex == size_t( -1 ) )
	{
		++m_it;
		m_nCaseRangeIndex = 0;
		m_nCaseIndex = 0;
		return;
	}
	
	size_t nMax = GetCaseRangeLast( m_nCaseRangeIndex ) - GetCaseRangeFirst( m_nCaseRangeIndex );
			
	if ( ++m_nCaseIndex > nMax )
	{
		if ( ++m_nCaseRangeIndex >= GetNumCaseRanges() )
		{
			m_nCaseRangeIndex = 0;
				++m_it;
		}

		m_nCaseIndex = 0;
	}
}


void CPlayList::Iterator::PrevCase()
{
	if ( m_nCaseIndex == size_t( -1 ) )
	{
		--m_it;
		m_nCaseRangeIndex = GetNumCaseRanges() - 1;
		m_nCaseIndex = GetCaseRangeLast( m_nCaseRangeIndex ) - GetCaseRangeFirst( m_nCaseRangeIndex );
		return;
	}
	
	if ( m_nCaseIndex == 0 )
	{
		if ( m_nCaseRangeIndex == 0 )
		{
			--m_it;
			m_nCaseRangeIndex = GetNumCaseRanges() - 1;
		}
		else
		{
			--m_nCaseRangeIndex;
		}

		m_nCaseIndex = GetCaseRangeLast( m_nCaseRangeIndex ) - GetCaseRangeFirst( m_nCaseRangeIndex );
	}
	else
	{
		--m_nCaseIndex;
	}
}


void CPlayList::Iterator::NextTest()
{
	++m_it;
	m_nCaseRangeIndex = 0;
	m_nCaseIndex = 0;
}


void CPlayList::Iterator::PrevTest()
{
	--m_it;
	m_nCaseIndex = 0;
	m_nCaseRangeIndex = 0;
}


size_t CPlayList::Iterator::GetNumCaseRanges() const
{
	const TestPathCases &tc = *m_it;
	return tc.Cases.GetNumCaseRanges();
}


size_t CPlayList::Iterator::GetCaseRangeFirst( size_t nIndex ) const
{
	const TestPathCases &tc = *m_it;
	return tc.Cases.GetCaseRangeFirst( nIndex );
}


size_t CPlayList::Iterator::GetCaseRangeLast( size_t nIndex ) const
{
	const TestPathCases &tc = *m_it;
	return tc.Cases.GetCaseRangeLast( nIndex );
}


CPlayList::Iterator CPlayList::Find( const CTestGroupPath &Path, size_t nCaseNumber ) const
{
	return Find( Path.ToString(), nCaseNumber );
}


CPlayList::Iterator CPlayList::Find( const tstring &strPath, size_t nCaseNumber ) const
{
	TestCasesType::const_iterator it = FindTestPath( strPath );

	if ( it != m_TestCases.end() )
	{
		const CaseRangeSet &crs = (*it).Cases;
		
		size_t n = crs.GetNumCaseRanges();
		for( size_t i = 0; i < n; ++i )
		{
			if ( nCaseNumber >= crs.GetCaseRangeFirst( i ) && nCaseNumber <= crs.GetCaseRangeLast( i ) )
				return Iterator( it, i, nCaseNumber - crs.GetCaseRangeFirst( i ) );
		}
	}
	
	return End();
}


CPlayList::TestCasesType::iterator CPlayList::FindTestPath( const CTestGroupPath &Path )
{
	return FindTestPath( Path.ToString() );
}


CPlayList::TestCasesType::iterator CPlayList::FindTestPath( const tstring &strPath )
{
	// Search for the specified path in the list
	TestCasesMapType::iterator mit = m_TestCasesMap.find( strPath );
	if ( mit != m_TestCasesMap.end() )
		return (*mit).second;
	
	return m_TestCases.end();
}


CPlayList::TestCasesType::const_iterator CPlayList::FindTestPath( const CTestGroupPath &Path ) const
{
	return FindTestPath( Path.ToString() );
}


CPlayList::TestCasesType::const_iterator CPlayList::FindTestPath( const tstring &strPath ) const
{
	// Search for the specified path in the list
	TestCasesMapType::const_iterator mit = m_TestCasesMap.find( strPath );
	if ( mit != m_TestCasesMap.end() )
		return (*mit).second;
	
	return m_TestCases.end();
}


CPlayList::TestCasesType::iterator CPlayList::AddTest( CTestGroupLeafNode *pNode )
{
	assert( FindTestPath( pNode->GetPath() ) == m_TestCases.end() );
	
	// Add the path to the end of the list
	TestCasesType::iterator tcit;
	tcit = m_TestCases.insert( m_TestCases.end(), TestPathCases( pNode ) );
	m_TestCasesMap.insert( TestCasesMapType::value_type( pNode->GetPath().ToString(), tcit ) );
	
	return tcit;
}


CPlayList::Iterator CPlayList::Begin() const
{
	TestCasesType::const_iterator it = m_TestCases.begin();
	
	if ( it != m_TestCases.end() )
	{
		const TestPathCases &tc = *it;
		if ( tc.Cases.GetNumCaseRanges() )
		{
			return Iterator( it, 0, 0 );
		}
	}
	
	return End();
}


CPlayList::Iterator CPlayList::End() const
{
	return Iterator( m_TestCases.end(), 0, 0 );
}


void CPlayList::BuildCaseRangeSet( CaseRangeSet &set, const tstring &strCases )
{
	set.Clear();
	
	for( size_t nOffset = 0; nOffset != tstring::npos; )
	{
		CaseRange range;
		
		// Get range description string.
		size_t nStart = strCases.find_first_of( _T( ',' ), nOffset );
		// warning: unused nSize = size_t nSize = nStart == tstring::npos ? 0 : nStart - nOffset;
		size_t nSize = nStart == tstring::npos ? tstring::npos : nStart - nOffset;
		tstring strRange = strCases.substr( nOffset, nSize );
		nOffset = nStart == tstring::npos ? tstring::npos : nStart + 1;
		
		// Skip leading whitespace.
		nStart = strRange.find_first_not_of( _T( " \t" ) );
		
		if ( nStart == tstring::npos )
			continue;
			
		// Get the minimum range value.
		size_t nEnd = strRange.find_first_not_of( _T( "xX0123456789abcdefABCDEF" ), nStart );
		if ( nEnd == nStart )
			continue;

		nSize = nEnd == tstring::npos ? tstring::npos : nEnd - nStart;			
		tstring strNumber = strRange.substr( nStart, nSize );
		if ( !FromString( &range.nFirst, strNumber ) )
			continue;
			
		range.nLast = range.nFirst; // Copy nMin to nMax by default.
		
		if ( nEnd != tstring::npos )
		{
			// Skip more whitespace
			nStart = strRange.find_first_not_of( _T( " \t" ), nEnd );
			
			if ( nStart != tstring::npos )
			{
				// Find the '-' symbol
				if ( strRange[nStart] != _T( '-' ) )
					continue;
				
				// And past the whitespace
				nStart = strRange.find_first_not_of( _T( " \t" ), ++nStart );
				
				// If the last character is the '-' symbol then
				// the range indicates "to the last test case in the test".
				if ( nStart == tstring::npos )
				{
					range.nLast = CPlayList::RANGE_END;
				}
				else
				{
					// Get the maximum range value.
					nEnd = strRange.find_first_not_of( _T( "xX0123456789abcdefABCDEF" ), nStart );
					if ( nEnd == nStart )
						continue;
						
					nSize = nEnd == tstring::npos ? tstring::npos : nEnd - nStart;			
					strNumber = strRange.substr( nStart, nSize );
					if ( !FromString( &range.nLast, strNumber ) )
						continue;
					
					// Make sure there is nothing else in the range string.
					nStart = strRange.find_first_not_of( _T( " \t" ), nEnd );
					if ( nStart != tstring::npos )
						continue; // Why don't they learn?
				}
			}
		}
		
		set.AddRange( range.nFirst, range.nLast );
	}
	
	if ( set.GetNumCaseRanges() == 0 )
		set.AddRange( 1, CPlayList::RANGE_END );
}


void CPlayList::RemoveTestCases( const CTestGroupPath &Path, size_t nFirst, size_t nLast )
{
	TestCasesMapType::iterator mit = m_TestCasesMap.find( Path.ToString() );
	TestCasesType::iterator it = mit == m_TestCasesMap.end() ? m_TestCases.end() : (*mit).second;

	if ( it != m_TestCases.end() )
	{
		(*it).Cases.RemoveRange( nFirst, nLast );
	}
}


void CPlayList::RemoveTestCases( const CTestGroupPath &Path, const tstring &strCases )
{
	TestCasesMapType::iterator mit = m_TestCasesMap.find( Path.ToString() );
	TestCasesType::iterator it = mit == m_TestCasesMap.end() ? m_TestCases.end() : (*mit).second;
	
	if ( it != m_TestCases.end() )
	{
		CaseRangeSet set;
		BuildCaseRangeSet( set, strCases );

		for( size_t n = set.GetNumCaseRanges() - 1; n != (size_t) -1; --n )
		{
			(*it).Cases.RemoveRange( set.GetCaseRangeFirst( n ), set.GetCaseRangeLast( n ) );
			
			if ( (*it).Cases.GetNumCaseRanges() == 0 )
			{
				m_TestCases.erase( it );
				m_TestCasesMap.erase( mit );
			}
		}
	}
}


void CPlayList::AddTestCases( CTestGroupLeafNode *pNode, size_t nFirst, size_t nLast )
{
	TestCasesType::iterator it = FindTestPath( pNode->GetPath() );
	
	if ( it == m_TestCases.end() )
		it = AddTest( pNode );
		
	(*it).Cases.AddRange( nFirst, nLast );
}


void CPlayList::AddTestCases( CTestGroupLeafNode *pNode, const tstring &strCases )
{
	TestCasesType::iterator it = FindTestPath( pNode->GetPath() );
	
	if ( it == m_TestCases.end() )
		it = AddTest( pNode );		

	(*it).Cases.Clear();
	
	BuildCaseRangeSet( (*it).Cases, strCases );
}


void CPlayList::RemoveTestBranch( const CTestGroupPath &Branch )
{
	// Iterate through the subpaths of the specified path and remove each
	// test from the list.
	const CTestGroupNode *pNode = GetFramework()->GetTestGroupNode( Branch );
	
	if ( pNode )
		RemoveTestBranch( pNode );
}


void CPlayList::RemoveTestBranch( const CTestGroupNode *pNode )
{
	if ( pNode->IsTestNode() )
		RemoveTestCases( pNode->GetPath() );

	size_t n = pNode->GetNumChildGroupNodes();
	for( size_t i = 0; i < n; ++i )
	{
		const CTestGroupNode *pChild = pNode->GetChildGroupNode( i );
		if ( pChild )
			RemoveTestBranch( pChild );
	}

	// Remove the branch from the map and list of tests
	TestCasesMapType::iterator mit = m_TestCasesMap.find( pNode->GetPath().ToString() );
	if ( mit != m_TestCasesMap.end() ){
		m_TestCases.erase( (*mit).second );
		m_TestCasesMap.erase( mit );
	}
}


void CPlayList::AddTestBranch( const CTestGroupPath &Branch )
{
	// Iterate through the subpaths of the specified path and add each
	// test to the list.
	const CTestGroupNode *pNode = GetFramework()->GetTestGroupNode( Branch );
	
	if ( pNode )
		AddTestBranch( pNode );
}


void CPlayList::AddTestBranch( const CTestGroupNode *pNode )
{
	if ( pNode->IsTestNode() )
		AddTestCases( ( CTestGroupLeafNode *) pNode );

	size_t n = pNode->GetNumChildGroupNodes();
	for( size_t i = 0; i < n; ++i )
	{
		const CTestGroupNode *pChild = pNode->GetChildGroupNode( i );
		if ( pChild )
				AddTestBranch( pChild );
	}
}


void CPlayList::Clear()
{
	m_TestCases.clear();
	m_TestCasesMap.clear();
}


tstring ToString( const CWGFTestFramework::DPFLevel &level )
{
	return ToString( level.iLevel ); 
}


bool FromString( CWGFTestFramework::DPFLevel *plevel, const tstring &str ) 
{
	int iLevel;
	if ( !FromString( &iLevel, str ) )
		return false;
	*plevel = CWGFTestFramework::DPFLevel( iLevel );
	return true;
}


bool FromString( WGFTEST_MODE *pMode, const tstring &str ) 
{
	// N.B. [SEdmison]:
	// LOCALE_INVARIANT didn't come along until Windows XP;
	// here's the way to keep our tests working on Windows
	// 2000 (in case we care).
	LCID InvariantLocale = MAKELCID
	(
		MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ),
		SORT_DEFAULT
	);

	if (CSTR_EQUAL == CompareString(InvariantLocale, NORM_IGNORECASE, str.c_str(), -1, _T("WHQL"), -1))
	{
		*pMode = WGFTEST_MODE_WHQL;
	}
	else if (CSTR_EQUAL == CompareString(InvariantLocale, NORM_IGNORECASE, str.c_str(), -1, _T("BVT"), -1))
	{
		*pMode = WGFTEST_MODE_BVT;
	}
	else if (CSTR_EQUAL == CompareString(InvariantLocale, NORM_IGNORECASE, str.c_str(), -1, _T("STRESS"), -1))
	{
		*pMode = WGFTEST_MODE_STRESS;
	}
	else if (CSTR_EQUAL == CompareString(InvariantLocale, NORM_IGNORECASE, str.c_str(), -1, _T("RUNTIME"), -1))
	{
		*pMode = WGFTEST_MODE_RUNTIME;
	}
	else if (CSTR_EQUAL == CompareString(InvariantLocale, NORM_IGNORECASE, str.c_str(), -1, _T("UNDEFINED"), -1))
	{
		*pMode = WGFTEST_MODE_UNDEFINED;
	}
	else
	{
		*pMode = WGFTEST_MODE_UNDEFINED;
		return false;
	};

	return true;
}
