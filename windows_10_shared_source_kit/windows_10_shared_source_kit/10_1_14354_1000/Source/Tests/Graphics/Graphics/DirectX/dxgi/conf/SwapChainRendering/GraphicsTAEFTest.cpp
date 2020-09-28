//////////////////////////////////////////////////////////////////////
// File:  GraphicsTAEFTest.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a base class for TAEF-based graphics tests.
//
// History:
// 23 Mar 2013  SEdmison  Created, based on DImageTestBase.cpp.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "GraphicsTAEFTest.hpp"


//
// C runtime headers
//

#include <eh.h>
#include <signal.h>


//
// STL headers
//

#include <iterator>


//
// Test infrastructure headers
//

#include <Detours.h>
#include <DXGLog.hpp>
#include <TAEFLog.hpp>


/////////////////////////////////////////////////////////////////
//
// Globals
//
/////////////////////////////////////////////////////////////////

// Quick reference to the top of the containing binary
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

CDXGLogger * g_pDXGLogger = nullptr;


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Helper method implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  FromString( MESSAGE_TYPE )
//
// Purpose:
// Converts a string to a message type.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	_Out_ MESSAGE_TYPE * pMessageType,
	const std::wstring & MessageTypeString
)
{
	if ( pMessageType == nullptr )
	{
		return false;
	}

	*pMessageType = MESSAGE_TYPE_INFORMATIONAL;


	if
	(
		MessageTypeString == L"MESSAGE_TYPE_DEBUG" ||
		MessageTypeString == L"DEBUG" ||
		MessageTypeString == L"Debug" ||
		MessageTypeString == L"debug"
	)
	{
		*pMessageType = MESSAGE_TYPE_DEBUG;
		return true;
	};


	if
	(
		MessageTypeString == L"MESSAGE_TYPE_INFORMATIONAL" ||
		MessageTypeString == L"INFORMATIONAL" ||
		MessageTypeString == L"Informational" ||
		MessageTypeString == L"informational" ||
		MessageTypeString == L"INFO" ||
		MessageTypeString == L"Info" ||
		MessageTypeString == L"info"
	)
	{
		*pMessageType = MESSAGE_TYPE_INFORMATIONAL;
		return true;
	};


	if
	(
		MessageTypeString == L"MESSAGE_TYPE_VERBOSE" ||
		MessageTypeString == L"VERBOSE" ||
		MessageTypeString == L"Verbose" ||
		MessageTypeString == L"verbose"
	)
	{
		*pMessageType = MESSAGE_TYPE_VERBOSE;
		return true;
	};


	if
	(
		MessageTypeString == L"MESSAGE_TYPE_WARNING" ||
		MessageTypeString == L"WARNING" ||
		MessageTypeString == L"Warning" ||
		MessageTypeString == L"warning"
	)
	{
		*pMessageType = MESSAGE_TYPE_WARNING;
		return true;
	};


	return false;
};


#if defined(UNICODE)

template< typename IteratorType >
unsigned TokenizeString
(
	IteratorType Iterator,
	_In_ PCWSTR SourceString,
	_In_ PCWSTR Delimiters
)
{
	unsigned uCount = 0;
	WCHAR * pNextToken = nullptr;
	std::auto_ptr< WCHAR > CopiedString( _wcsdup( SourceString ) );

	for
	(
		PWSTR pwcs = wcstok_s
		(
			CopiedString.get(),
			Delimiters,
			&pNextToken
		);
		pwcs;
		pwcs = wcstok_s
		(
			nullptr,
			Delimiters,
			&pNextToken
		),
		uCount++
	)
	{
		*Iterator++ = pwcs;
	};

	return uCount;
};

#else

template< typename IteratorType >
unsigned TokenizeString
(
	IteratorType Iterator,
	PCSTR SourceString,
	PCSTR Delimiters
)
{
	unsigned uCount = 0;
	PSTR pNextToken = nullptr;
	std::auto_ptr< CHAR > CopiedString( _strdup( SourceString ) );

	for
	(
		PSTR pwcs = strtok_s
		(
			CopiedString.get(),
			Delimiters,
			&pNextToken
		);
		pwcs;
		pwcs = strtok_s
		(
			nullptr,
			Delimiters,
			&pNextToken
		),
		uCount++
	)
	{
		*Iterator++ = pwcs;
	};

	return uCount;
};

#endif


////////////////////////////////////////////////////////////
//
// Detour method implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  Detour_EnableFeatureLevelUpgrade
//
// Purpose:
// Stub function to be used as a substitute for the
// Direct3D EnableFeatureLevelUpgrade routine.
///////////////////////////////////////////////////////
__declspec( noinline ) BOOL WINAPI Detour_EnableFeatureLevelUpgrade()
{
	static UINT iJustMakeThisFnBigEnoughForDetours = 0;
	iJustMakeThisFnBigEnoughForDetours++;

	return FALSE;
};


////////////////////////////////////////////////////////////
//
// CRT signal handler implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CRTSignalHandler
//
// Purpose:
// Handles C runtime signals.
///////////////////////////////////////////////////////
extern "C" void __cdecl CRTSignalHandler
(
	int CRTSignal
)
{
	switch ( CRTSignal )
	{
		case SIGABRT:
		{
			if ( IsDebuggerPresent() == TRUE )
			{
				DebugBreak();
			}
			else
			{
				exit( 3 );
			};
		};
	};
};


////////////////////////////////////////////////////////////
//
// Module properties
//
////////////////////////////////////////////////////////////

BEGIN_MODULE()
    MODULE_PROPERTY(L"Kits.DeploymentItem", L"nttest\\windowstest\\graphics\\d3d\\support\\d3dx10_test.dll")
    MODULE_PROPERTY(L"Kits.DeploymentItem", L"nttest\\windowstest\\graphics\\d3d\\support\\d3dx11_test.dll")
END_MODULE()

////////////////////////////////////////////////////////////
//
// Module setup and cleanup implementation
//
////////////////////////////////////////////////////////////

MODULE_SETUP( OpenLog );
MODULE_CLEANUP( CloseLog );


///////////////////////////////////////////////////////
// Function:  OpenLog
//
// Purpose:
// Creates the log file for the test if necessary and
// opens a logger object to write to the log.
///////////////////////////////////////////////////////
bool __stdcall OpenLog()
{
	//
	// Initialize locals.
	//

	bool bOverallResult = false;

	DXGLOG_INFO DXGLogOptions;
	ZeroMemory( &DXGLogOptions, sizeof( DXGLogOptions ) );

	WCHAR BinaryPathName[ MAX_PATH ] = L"";
	WCHAR * BinaryFileName = nullptr;

	CHAR LogFileName[ MAX_PATH ] = "";

	bool bUseDXGLogger = false;
	bool bAllowDXGLoggerCaching = true;
	bool bAllowLoggingToConsole = false;
#if defined( DEBUG )
	bool bAllowLoggingToUserModeDebugger = true;
#else
	bool bAllowLoggingToUserModeDebugger = false;
#endif
	bool bAppendToLog = false;

	UINT32 MaxFailures = 100;
	UINT32 MaxFailuresPerGroup = 10;


	//
	// Mark our process as DPI-aware so as to prevent the DWM
	// from lying to us about the desktop resolution.
	//

	SetProcessDpiAwareness( PROCESS_PER_MONITOR_DPI_AWARE );


	//
	// Register to handle CRT signals with a DebugBreak when under
	// the debugger (giving someone at the keyboard a chance to
	// look at the call stack to see what happened, such as a
	// failed assertion).
	//

	signal
	(
		SIGABRT,
		CRTSignalHandler
	);


	//
	// Initialize COM for the thread.
	//

	WEX::Common::String ThreadingModel( L"MTA" );
	/* HRESULT hGetThreadingModelName = */ WEX::TestExecution::TestData::TryGetValue
	(
		L"COMThreading",
		ThreadingModel
	);

	HRESULT hInitializeCOMResult = CoInitializeEx
	(
		nullptr,
		ThreadingModel == L"STA" ? COINIT_APARTMENTTHREADED : COINIT_MULTITHREADED
	);

	assert( hInitializeCOMResult == S_OK );
	if ( FAILED( hInitializeCOMResult ) )
	{
		WEX::Logging::Log::Error
		(
			L"CoInitializeEx failed."
		);

		return false;
	};


	//
	// Parse command-line parameters.
	//

	/* HRESULT hGetDXGLoggerParameterResult = */ WEX::TestExecution::RuntimeParameters::TryGetValue
	(
		L"EnableDXGLogging",
		bUseDXGLogger
	);

	/* HRESULT hGetDXGLoggerCachingParameterResult = */ WEX::TestExecution::RuntimeParameters::TryGetValue
	(
		L"LogCaching",
		bAllowDXGLoggerCaching
	);

	/* HRESULT hGetLogToConsoleParameterResult = */ WEX::TestExecution::RuntimeParameters::TryGetValue
	(
		L"LogToConsole",
		bAllowLoggingToConsole
	);

	/* HRESULT hGetLogToUserModeDebuggerParameterResult = */ WEX::TestExecution::RuntimeParameters::TryGetValue
	(
		L"LogToDebugger",
		bAllowLoggingToUserModeDebugger
	);

	/* HRESULT hGetMaxFailuresResult = */ WEX::TestExecution::RuntimeParameters::TryGetValue
	(
		L"MaxFailures",
		MaxFailures
	);

	/* HRESULT hGetMaxFailuresPerGroupResult = */ WEX::TestExecution::RuntimeParameters::TryGetValue
	(
		L"MaxFailuresPerGroup",
		MaxFailuresPerGroup
	);

	if ( MaxFailuresPerGroup > MaxFailures )
	{
		MaxFailures = MaxFailuresPerGroup;
	};


	//
	// Determine the type of logger to create.
	//

	if ( bUseDXGLogger == true )
	{
		//
		// Create a DXGLogger object.
		//

		g_pDXGLogger = new( std::nothrow ) CDXGLogger();

		if ( g_pDXGLogger == nullptr )
		{
			goto Cleanup;
		}


		//
		// Have the DXGLogger open the log.
		//

		DXGLogOptions.dwOptions =
			DXGLOG_OPTION_OUTPUT_XML |
			DXGLOG_OPTION_UNICODE;

		if ( bAllowLoggingToConsole == true )
		{
			DXGLogOptions.dwOptions |=
				DXGLOG_OPTION_OUTPUT_CONSOLE;
		};

		if ( bAllowLoggingToUserModeDebugger == true )
		{
			DXGLogOptions.dwOptions |=
				DXGLOG_OPTION_OUTPUT_DEBUGGER;
		};

		if ( bAllowDXGLoggerCaching == false )
		{
			DXGLogOptions.dwOptions |=
				DXGLOG_OPTION_NOCACHE;
		};

		if ( bAppendToLog == false )
		{
			DXGLogOptions.dwOptions |=
				DXGLOG_OPTION_OVERWRITE;
		};


		//
		// Get the name of the binary.
		//

		DWORD nBinaryPathName = GetModuleFileNameW
		(
			(HMODULE) &__ImageBase,
			BinaryPathName,
			ARRAYSIZE( BinaryPathName )
		);

		if ( nBinaryPathName < 1 )
		{
			goto Cleanup;
		};

		BinaryFileName = wcsrchr
		(
			BinaryPathName,
			L'\\'
		);

		if ( BinaryFileName == nullptr )
		{
			BinaryFileName = BinaryPathName;
		}
		else
		{
			++BinaryFileName;
		};


		//
		// Build the name of the log.
		//

		StringCchPrintfA
		(
			LogFileName,
			ARRAYSIZE( LogFileName ),
			"%S",
			BinaryFileName
		);


		//
		// Specify the test name, log name, and directory name.
		//

		DXGLogOptions.sTestName = _T( "DXGI Presentation Duration Conformance Tests" );
		DXGLogOptions.sXMLFileNameBase = LogFileName;
		DXGLogOptions.sXMLFileExtension = ".wtl";
		DXGLogOptions.sLocalDirectory = _T( "." );


		//
		// Specify thresholds for which results to log.
		//

		DXGLogOptions.uMaxGroupPasses = 0;
		DXGLogOptions.uMaxGroupSkips = 2;
		DXGLogOptions.uMaxGroupFailures = MaxFailuresPerGroup;
		DXGLogOptions.uMaxGroupAborts = 10;
		DXGLogOptions.uMaxGroupWarnings = 10;

		DXGLogOptions.uMaxLogFilePasses = 0;
		DXGLogOptions.uMaxLogFileFailures = MaxFailures;
		DXGLogOptions.uMaxLogFileSkips = UINT_MAX;
		DXGLogOptions.uMaxLogFileAborts = UINT_MAX;
		DXGLogOptions.uMaxLogFileWarnings = 100;


		//
		// Have the DXGLogger open the log.
		//

		HRESULT hOpenDXGLogResult = g_pDXGLogger->Open( &DXGLogOptions );

		if ( FAILED( hOpenDXGLogResult ) )
		{
			goto Cleanup;
		};


		//
		// Begin a device group.
		//

		HRESULT hBeginDeviceGroupResult = g_pDXGLogger->BeginDeviceGroup();

		if ( FAILED( hBeginDeviceGroupResult ) )
		{
			goto Cleanup;
		};
	};


	bOverallResult = true;


Cleanup:
	if ( hInitializeCOMResult == S_OK )
	{
		//
		// Uninitialize COM for the thread.
		//

		CoUninitialize();
	};


	return bOverallResult;
};


///////////////////////////////////////////////////////
// Function:  CloseLog
//
// Purpose:
// Closes the logger object and (if necessary) the log
// file.
///////////////////////////////////////////////////////
bool __stdcall CloseLog()
{
	//
	// Initialize locals.
	//

	bool bOverallResult = false;


	//
	// Close the DXGLogger if necessary.
	//

	if ( g_pDXGLogger != nullptr )
	{
		//
		// End the device group.
		//

		g_pDXGLogger->EndDeviceGroup();


		//
		// Close the DXGLogger object.
		//

		g_pDXGLogger->Close();


		//
		// Release the DXGLogger object.
		//

		delete g_pDXGLogger;
		g_pDXGLogger = nullptr;
	};


	bOverallResult = true;


// Cleanup:
	return bOverallResult;
};


////////////////////////////////////////////////////////////
//
// TAEF graphics test base class implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CGraphicsTAEFTest
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CGraphicsTAEFTest::CGraphicsTAEFTest()
:
	m_pLogger( nullptr ),
	m_bHaltOnFailure(false),
	m_VariationStartParam( 0 ),
	m_VariationEndParam( 0 ),
	m_VariationStartNumber( static_cast< size_t >( -1 ) ),
	m_VariationEndNumber( static_cast< size_t >( -1 ) ),
	m_VariationCount( 0 ),
	m_hDXGIDebugDLL( nullptr ),
	m_pDXGIDebug( nullptr ),
	m_pDXGIInfoQueue( nullptr ),
	m_bAllowFeatureLevelPromotion( true ),
	m_pD3D11_EnableFeatureLevelUpgrade( nullptr ),
	m_pD3D12_EnableFeatureLevelUpgrade( nullptr )
{
	//
	// Set the thread name.
	//

	SetThreadName( 0xFFFFFFFF, "Main test thread" );


	//
	// Create a log wrapper object.
	//

	if ( g_pDXGLogger != nullptr )
	{
		//
		// Create a DXG log wrapper.
		//

		/* HRESULT hOpenDXGLogWrapperResult = */ CDXGLog::CreateInstance
		(
			g_pDXGLogger,
			IID_ISimpleLogController,
			reinterpret_cast< void ** >( &m_pLogger )
		);
	}
	else
	{
		//
		// Create a TAEF log wrapper.
		//

		/* HRESULT hOpenTAEFLogWrapperResult = */ CTAEFLog::CreateInstance
		(
			IID_ISimpleLogController,
			reinterpret_cast< void ** >( &m_pLogger )
		);
	};


	//
	// Set the threshold for messages to be logged versus ignored.
	//

	WEX::Common::String LogMessageThresholdString;
	MESSAGE_TYPE LogMessageThreshold = MESSAGE_TYPE_INFORMATIONAL;

	HRESULT hGetLogMessageThresholdResult = WEX::TestExecution::RuntimeParameters::TryGetValue
	(
		L"LogThreshold",
		LogMessageThresholdString
	);

	if ( SUCCEEDED( hGetLogMessageThresholdResult ) )
	{
		bool bParseLogMessageThresholdResult = FromString
		(
			&LogMessageThreshold,
			std::wstring( LogMessageThresholdString )
		);

		if ( bParseLogMessageThresholdResult == true )
		{
			m_pLogger->SetMessageThreshold( LogMessageThreshold );
		};
	};


	//
	// Configure Direct3D feature level promotion.
	//

	/* HRESULT hGetAllowFeatureLevelPromotionResult = */ WEX::TestExecution::RuntimeParameters::TryGetValue
	(
		L"D3DAllowFeatureLevelPromotion",
		m_bAllowFeatureLevelPromotion
	);

	if ( m_bAllowFeatureLevelPromotion == false )
	{
		InstallFeatureLevelPromotionDetour( TEST_API_D3D11 );
		InstallFeatureLevelPromotionDetour( TEST_API_D3D12 );
	};
};


///////////////////////////////////////////////////////
// Function:  ~CGraphicsTAEFTest
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CGraphicsTAEFTest::~CGraphicsTAEFTest()
{
	if ( m_bAllowFeatureLevelPromotion == false )
	{
		UninstallFeatureLevelPromotionDetour( TEST_API_D3D11 );
		UninstallFeatureLevelPromotionDetour( TEST_API_D3D12 );
	};

	SAFE_RELEASE( m_pLogger );
};


///////////////////////////////////////////////////////
// Function:  InstallFeatureLevelPromotionDetour
//
// Purpose:
// Installs a detour the re-routes Direct3D's
// EnableFeatureLevelUpgrade to a substitute function.
///////////////////////////////////////////////////////
HRESULT CGraphicsTAEFTest::InstallFeatureLevelPromotionDetour
(
	TEST_API Direct3DVersion
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Validate parameters.
	//

	if ( Direct3DVersion != TEST_API_D3D11 && Direct3DVersion != TEST_API_D3D12 )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"CGraphicsTAEFTest::InstallFeatureLevelPromotionDetour:  Unsupported API version."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Disable Direct3D feature level promotion.
	//

	switch ( Direct3DVersion )
	{
		case TEST_API_D3D12:
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"CGraphicsTAEFTest::InstallFeatureLevelPromotionDetour:  Disabling Direct3D 12 feature level promotion."
			);


			HMODULE hD3D12 = GetModuleHandleA( "D3D12.dll" );
			if ( hD3D12 == nullptr )
			{
				hOverallResult = S_FALSE;
				goto Cleanup;
			};

			m_pD3D12_EnableFeatureLevelUpgrade = (PFNEnableFeatureLevelUpgrade) GetProcAddress
			(
				hD3D12,
				"EnableFeatureLevelUpgrade"
			);

			if ( m_pD3D12_EnableFeatureLevelUpgrade == nullptr )
			{
				DWORD GetProcAddressError = GetLastError();

				LOG_ERROR
				(
					ERROR_TYPE_WIN32,
					GetProcAddressError,
					L"EnableFeatureLevelUpgrade entry point not found in D3D12.dll."
				);

				hOverallResult = HRESULT_FROM_WIN32( GetProcAddressError );
				goto Cleanup;
			};
		} break;


		case TEST_API_D3D11:
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"CGraphicsTAEFTest::InstallFeatureLevelPromotionDetour:  Disabling Direct3D 11 feature level promotion."
			);


			HMODULE hD3D11 = GetModuleHandleA( "D3D11.dll" );
			if ( hD3D11 == nullptr )
			{
				hOverallResult = S_FALSE;
				goto Cleanup;
			};

			m_pD3D11_EnableFeatureLevelUpgrade = (PFNEnableFeatureLevelUpgrade) GetProcAddress
			(
				hD3D11,
				"EnableFeatureLevelUpgrade"
			);

//#define D3D11_EXPORT_ORDINAL_ENABLEFEATURELEVELUPGRADE 13
#if defined(D3D11_EXPORT_ORDINAL_ENABLEFEATURELEVELUPGRADE)
			// We can load it by ordinal only if this test is built 
			// specifically to know what export ordinal to look for.
			// The ordinal has changed in the various versions of
			// Windows since 7, and getting it wrong would mean that
			// we'd be calling some other function with a bogus stack
			// and would likely crash (or trigger other hard-to-debug
			// behavior).
			if ( m_pD3D11_EnableFeatureLevelUpgrade == nullptr )
			{
				m_pD3D11_EnableFeatureLevelUpgrade = (PFNEnableFeatureLevelUpgrade) GetProcAddress
				(
					hD3D11,
					(PCSTR) D3D11_EXPORT_ORDINAL_ENABLEFEATURELEVELUPGRADE
				);
			};
#endif

			if ( m_pD3D11_EnableFeatureLevelUpgrade == nullptr )
			{
				DWORD GetProcAddressError = GetLastError();

				LOG_ERROR
				(
					ERROR_TYPE_WIN32,
					GetProcAddressError,
					L"EnableFeatureLevelUpgrade entry point not found in D3D11.dll."
				);

				hOverallResult = HRESULT_FROM_WIN32( GetProcAddressError );
				goto Cleanup;
			};
		} break;
	};


	//
	// Detour the EnableFeatureLevelUpgrade routine to a substitute.
	//

	LONG BeginTransactionResult = DetourTransactionBegin();

	if ( BeginTransactionResult != ERROR_SUCCESS )
	{
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			BeginTransactionResult,
			L"DetourBeginTransaction failed."
		);

		goto Cleanup;
	};


	LONG UpdateThreadResult = DetourUpdateThread( GetCurrentThread() );

	if ( UpdateThreadResult != ERROR_SUCCESS )
	{
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			UpdateThreadResult,
			L"DetourUpdateThread failed."
		);

		DetourTransactionAbort();

		goto Cleanup;
	};


	LONG AttachDetourResult = DetourAttach
	(
		( Direct3DVersion == TEST_API_D3D12 )
			? reinterpret_cast< void ** >( &m_pD3D12_EnableFeatureLevelUpgrade )
			: reinterpret_cast< void ** >( &m_pD3D11_EnableFeatureLevelUpgrade ),
		Detour_EnableFeatureLevelUpgrade
	);

	if ( AttachDetourResult != ERROR_SUCCESS )
	{
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			AttachDetourResult,
			L"DetourAttach failed."
		);

		DetourTransactionAbort();

		goto Cleanup;
	};


	LONG CommitTransactionResult = DetourTransactionCommit();

	if ( CommitTransactionResult != ERROR_SUCCESS )
	{
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			CommitTransactionResult,
			L"DetourTransactionCommit failed."
		);

		DetourTransactionAbort();

		goto Cleanup;
	};


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  UninstallFeatureLevelPromotionDetour
//
// Purpose:
// Removes the detour set by the above method
// InstallFeatureLevelPromotionDetour.
///////////////////////////////////////////////////////
HRESULT CGraphicsTAEFTest::UninstallFeatureLevelPromotionDetour
(
	TEST_API Direct3DVersion
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Validate parameters.
	//

	if ( Direct3DVersion != TEST_API_D3D11 && Direct3DVersion != TEST_API_D3D12 )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"CGraphicsTAEFTest::UninstallFeatureLevelPromotionDetour:  Unsupported API version."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Validate members and state.
	//

	if
	(
		Direct3DVersion == TEST_API_D3D11 && m_pD3D11_EnableFeatureLevelUpgrade == nullptr ||
		Direct3DVersion == TEST_API_D3D12 && m_pD3D12_EnableFeatureLevelUpgrade == nullptr
	)
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"EnableFeatureLevelUpgrade function pointer is NULL."
		);

		hOverallResult = S_FALSE;
		goto Cleanup;
	};


	//
	// Uninstall the detour.
	//

	LONG BeginTransactionResult = DetourTransactionBegin();

	if ( BeginTransactionResult != ERROR_SUCCESS )
	{
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			BeginTransactionResult,
			L"DetourBeginTransaction failed."
		);

		hOverallResult = HRESULT_FROM_WIN32( BeginTransactionResult );
		goto Cleanup;
	};


	LONG UpdateThreadResult = DetourUpdateThread( GetCurrentThread() );

	if ( UpdateThreadResult != ERROR_SUCCESS )
	{
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			UpdateThreadResult,
			L"DetourUpdateThread failed."
		);

		DetourTransactionAbort();

		hOverallResult = HRESULT_FROM_WIN32( UpdateThreadResult );
		goto Cleanup;
	};


	LONG DetachDetourResult = DetourDetach
	(
		( Direct3DVersion == TEST_API_D3D12 )
			? reinterpret_cast< void ** >( &m_pD3D12_EnableFeatureLevelUpgrade )
			: reinterpret_cast< void ** >( &m_pD3D11_EnableFeatureLevelUpgrade ),
		Detour_EnableFeatureLevelUpgrade
	);

	if ( DetachDetourResult != ERROR_SUCCESS )
	{
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			DetachDetourResult,
			L"DetourDetach failed."
		);

		DetourTransactionAbort();

		hOverallResult = HRESULT_FROM_WIN32( DetachDetourResult );
		goto Cleanup;
	};


	LONG CommitTransactionResult = DetourTransactionCommit();

	if ( CommitTransactionResult != ERROR_SUCCESS )
	{
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			CommitTransactionResult,
			L"DetourCommitTransaction failed."
		);

		DetourTransactionAbort();

		hOverallResult = HRESULT_FROM_WIN32( CommitTransactionResult );
		goto Cleanup;
	};


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  SetupTestGroup
//
// Purpose:
// Handles class-wide initialization tasks.
///////////////////////////////////////////////////////
bool CGraphicsTAEFTest::SetupTestGroup()
{
	//
	// Create a DXGI debug interface object.
	//

	CreateDXGIDebugInterface();


	//
	// Set the symbol path.
	//

	HRESULT hSetSymbolPathResult = SetSymbolPath();

	if ( FAILED( hSetSymbolPathResult ) )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_WARNING,
			WEX::Common::String().Format
			(
				L"SetSymbolPath failed with error 0x%x.",
				hSetSymbolPathResult
			)
		);
	};


	//
	// Initialize COM for the thread.
	//

	WEX::Common::String ThreadingModel( L"MTA" );
	/* HRESULT hGetThreadingModelName = */ WEX::TestExecution::TestData::TryGetValue
	(
		L"COMThreading",
		ThreadingModel
	);

	HRESULT hInitializeCOMResult = CoInitializeEx
	(
		nullptr,
		ThreadingModel == L"STA" ? COINIT_APARTMENTTHREADED : COINIT_MULTITHREADED
	);

	assert( hInitializeCOMResult == S_OK) ;
	if ( FAILED( hInitializeCOMResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hInitializeCOMResult,
			L"CoInitializeEx failed."
		);

		return false;
	};


	//
	// Parse runtime options.
	//

	// Parse execution parameters.
	WEX::TestExecution::RuntimeParameters::TryGetValue< bool >( L"HaltOnFailure", m_bHaltOnFailure );
	WEX::TestExecution::RuntimeParameters::TryGetValue< bool >( L"StopOnFailure", m_bHaltOnFailure );


	// Parse variation parameters.
	GetVariations();


	return true;
};


///////////////////////////////////////////////////////
// Function:  CleanupTestGroup
//
// Purpose:
// Handles class-wide clean-up tasks.
///////////////////////////////////////////////////////
bool CGraphicsTAEFTest::CleanupTestGroup()
{
	//
	// Report live objects.
	//

	if ( m_pDXGIDebug != nullptr )
	{
		ReportLiveDXGIObjects();
	};


	//
	// Release the DXGI debug object.
	//

	SAFE_RELEASE( m_pDXGIDebug );


	//
	// Release the DXGI info queue object.
	//

	SAFE_RELEASE( m_pDXGIInfoQueue );


	//
	// Uninitialize COM for the thread.
	//

	CoUninitialize();


	//
	// Free DXGIDebug.dll.
	//

	if ( m_hDXGIDebugDLL != nullptr )
	{
		FreeLibrary( m_hDXGIDebugDLL );
		m_hDXGIDebugDLL = nullptr;
	};


	return true;
};


///////////////////////////////////////////////////////
// Function:  SetupTestCase
//
// Purpose:
// Runs before each test method invocation to set up
// per-variation state.
///////////////////////////////////////////////////////
bool CGraphicsTAEFTest::SetupTestCase()
{
	return true;
};


///////////////////////////////////////////////////////
// Function:  CleanupTestCase
//
// Purpose:
// Runs after each test method invocation to clean up
// per-variation state.
///////////////////////////////////////////////////////
bool CGraphicsTAEFTest::CleanupTestCase()
{
	return true;
};


///////////////////////////////////////////////////////
// Function:  CreateDXGIDebugInterface
//
// Purpose:
// Create a DXGI debug object.
///////////////////////////////////////////////////////
void CGraphicsTAEFTest::CreateDXGIDebugInterface()
{
	//
	// Create a DXGI debug object.
	//

	DXGIGetDebugInterface1
	(
        0,
		IID_IDXGIDebug,
		(void **) &m_pDXGIDebug
	);

	//
	// Create a DXGI info queue object.
	//

	DXGIGetDebugInterface1
	(
        0,
		IID_IDXGIInfoQueue,
		(void **) &m_pDXGIInfoQueue
	);
};


///////////////////////////////////////////////////////
// Function:  ReportLiveDXGIObjects
//
// Purpose:
// Have the DXGI debug object report DXGI and Direct3D
// objects that are still live at the end of the test
// (and presumably leaked).
///////////////////////////////////////////////////////
void CGraphicsTAEFTest::ReportLiveDXGIObjects()
{
	//
	// Initialize locals.
	//

	UINT64 QueuedMessages = 0;


	//
	// Validate members and state.
	//

	if ( m_pDXGIDebug == nullptr )
	{
		return;
	};

	if ( m_pDXGIInfoQueue == nullptr )
	{
		return;
	};


	//
	// Report live Direct3D 11 and DXGI objects.
	//

	m_pDXGIInfoQueue->ClearStoredMessages( DXGI_DEBUG_ALL );

	m_pDXGIDebug->ReportLiveObjects
	(
		DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_DETAIL
	);

	QueuedMessages = m_pDXGIInfoQueue->GetNumStoredMessages( DXGI_DEBUG_ALL );

	if ( QueuedMessages > 0 )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_WARNING,
			L"One or more Direct3D or DXGI objects are still alive at the end of the test (and thus have likely been leaked):"
		);
	};

	for
	(
		UINT64 iCurrentMessage = 0;
		iCurrentMessage < QueuedMessages;
		++iCurrentMessage
	)
	{
		// Get the message length.
		SIZE_T nCurrentMessage = 0;
		HRESULT hGetMessageLengthResult = m_pDXGIInfoQueue->GetMessage
		(
			DXGI_DEBUG_ALL,
			iCurrentMessage,
			nullptr,
			&nCurrentMessage
		);

		if ( FAILED( hGetMessageLengthResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hGetMessageLengthResult,
				WEX::Common::String().Format
				(
					L"GetMessage failed for message %I64u.",
					iCurrentMessage
				)
			);

			continue;
		};

		assert( nCurrentMessage > 0 );
		if ( nCurrentMessage > 0 )
		{
			// Allocate a buffer to hold the message.
			BYTE * pCurrentMessageBuffer = new( std::nothrow ) BYTE[ nCurrentMessage ];
			if ( pCurrentMessageBuffer == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					E_OUTOFMEMORY,
					L"Failed to allocated DXGI info queue message buffer."
				);

				return;
			};


			// Get the message.
			DXGI_INFO_QUEUE_MESSAGE * pCurrentMessage = reinterpret_cast< DXGI_INFO_QUEUE_MESSAGE * >( pCurrentMessageBuffer );

			HRESULT hGetMessageResult = m_pDXGIInfoQueue->GetMessage
			(
				DXGI_DEBUG_ALL,
				iCurrentMessage,
				pCurrentMessage,
				&nCurrentMessage
			);

			if ( FAILED( hGetMessageResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hGetMessageResult,
					WEX::Common::String().Format
					(
						L"GetMessage failed for message %I64u.",
						iCurrentMessage
					)
				);

				delete [] pCurrentMessageBuffer;
				pCurrentMessageBuffer = nullptr;

				continue;
			};


			// Log the message.
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				WEX::Common::String().Format
				(
					L"DXGI message (category %s, severity %s, ID %u):  %S.",
					ToString( pCurrentMessage->Category ).c_str(),
					ToString( pCurrentMessage->Severity ).c_str(),
					pCurrentMessage->ID,
					pCurrentMessage->pDescription
				)
			);


			// Delete the message buffer.
			pCurrentMessage = nullptr;

			delete [] pCurrentMessageBuffer;
			pCurrentMessageBuffer = nullptr;
		};
	};
};


///////////////////////////////////////////////////////
// Function:  SetSymbolPath
//
// Purpose:
// Set the NT symbol path to include the OSBinRoot and
// TestBinRoot paths to make finding debug symbols more
// likely in the event of a crash.
///////////////////////////////////////////////////////
HRESULT CGraphicsTAEFTest::SetSymbolPath()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	WCHAR WTTSDKPath[MAX_PATH] = L"";

	WCHAR NativeOSBinRoot[MAX_PATH] = L"";
	WCHAR WoW64OSBinRoot[MAX_PATH] = L"";

	WCHAR NativeTestBinRoot[MAX_PATH] = L"";
	WCHAR WoW64TestBinRoot[MAX_PATH] = L"";

	WCHAR SymbolPath[MAX_PATH * 4] = L"";


	PWSTR lpszErrorXML = nullptr;
	PWSTR lpszOSBinRoot = nullptr;
	PWSTR lpszWoW64OSBinRoot = nullptr;
	PWSTR lpszTestBinRoot = nullptr;
	PWSTR lpszWoW64TestBinRoot = nullptr;

	HMODULE hWTTSDK = nullptr;

	typedef PWSTR (*PF_GetParameterValue)
	(
		PCWSTR pcszParamName,
		PWSTR * ppszErrorXML
	);
	PF_GetParameterValue pWTTSDK_GetParameterValue = nullptr;

	typedef void (*PF_WTTFreeMemory)
	(
		PVOID pMemory
	);
	PF_WTTFreeMemory pWTTSDK_WTTFreeMemory = nullptr;

	typedef void (*PF_WTTSDKCleanup)();
	PF_WTTSDKCleanup pWTTSDK_WTTSDKCleanup = nullptr;


	//
	// Load WTTSDK.dll.
	//

	hWTTSDK = LoadLibraryExW
	(
		L"WTTSDK.dll",
		nullptr,
		0
	);

	if ( hWTTSDK == nullptr )
	{
		//
		// Get the WTT client directory.
		//

		DWORD nWTTSDKPath = GetEnvironmentVariableW
		(
			L"WTTBin",
			WTTSDKPath,
			ARRAYSIZE( WTTSDKPath )
		);

		if ( nWTTSDKPath == 0 )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				L"GetEnvironmentVariable on WTTBin failed."
			);
		}
		else
		{
			//
			// Concatenate the DLL name to the path.
			//

			StringCchCatW
			(
				WTTSDKPath,
				ARRAYSIZE(WTTSDKPath),
				L"\\WTTSDK.dll"
			);


			//
			// Try to load WTTSDK.dll again.
			//

			hWTTSDK = LoadLibraryExW
			(
				WTTSDKPath,
				nullptr,
				0
			);
		};
	};


	if ( hWTTSDK != nullptr )
	{
		//
		// Find the WTTSDK.dll entry points.
		//

		pWTTSDK_GetParameterValue = reinterpret_cast< PF_GetParameterValue >
		(
			GetProcAddress
			(
				hWTTSDK,
				"GetParameterValue"
			)
		);

		if ( pWTTSDK_GetParameterValue == nullptr )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				L"WTTSDK.dll was loaded, but does not seem to contain a GetParameterValue entry point."
			);
		};


		pWTTSDK_WTTFreeMemory = reinterpret_cast< PF_WTTFreeMemory >
		(
			GetProcAddress
			(
				hWTTSDK,
				"WTTFreeMemory"
			)
		);

		if ( pWTTSDK_WTTFreeMemory == nullptr )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				L"WTTSDK.dll was loaded, but does not seem to contain a WTTFreeMemory entry point."
			);

			pWTTSDK_GetParameterValue = nullptr;
		};


		pWTTSDK_WTTSDKCleanup = reinterpret_cast<PF_WTTSDKCleanup>
		(
			GetProcAddress
			(
				hWTTSDK,
				"WTTSDKCleanup"
			)
		);

		if ( pWTTSDK_WTTSDKCleanup == nullptr )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				L"WTTSDK.dll was loaded, but does not seem to contain a WTTSDKCleanup entry pointer."
			);

			pWTTSDK_GetParameterValue = nullptr;
			pWTTSDK_WTTFreeMemory = nullptr;
		};
	};


	//
	// Get the value of the WTT\OSBinRoot dimension.
	//

	if ( pWTTSDK_GetParameterValue != nullptr )
	{
		lpszOSBinRoot = (*pWTTSDK_GetParameterValue)
		(
			L"WTT\\OSBinRoot",
			&lpszErrorXML
		);

		if ( lpszOSBinRoot == nullptr )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				L"Could not find a value for WTT\\OSBinRoot."
			);
		}
		else
		{
			StringCchCopyW
			(
				NativeOSBinRoot,
				ARRAYSIZE(NativeOSBinRoot),
				lpszOSBinRoot
			);

			(*pWTTSDK_WTTFreeMemory)( lpszOSBinRoot );
			lpszOSBinRoot = nullptr;
		};

		if ( lpszErrorXML != nullptr )
		{
			(*pWTTSDK_WTTFreeMemory)( lpszErrorXML );
			lpszErrorXML = nullptr;
		};


		//
		// Get the value of the WTT\WoW64OSBinRoot dimension.
		//

		lpszWoW64OSBinRoot = (*pWTTSDK_GetParameterValue)
		(
			L"WTT\\WoW64OSBinRoot",
			&lpszErrorXML
		);

		if ( lpszWoW64OSBinRoot == nullptr )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				L"Could not find a value for WTT\\WoW64OSBinRoot."
			);
		}
		else
		{
			StringCchCopyW
			(
				WoW64OSBinRoot,
				ARRAYSIZE(WoW64OSBinRoot),
				lpszWoW64OSBinRoot
			);

			(*pWTTSDK_WTTFreeMemory)( lpszWoW64OSBinRoot );
			lpszWoW64OSBinRoot = nullptr;
		};

		if ( lpszErrorXML != nullptr )
		{
			(*pWTTSDK_WTTFreeMemory)( lpszErrorXML );
			lpszErrorXML = nullptr;
		};


		//
		// Get the value of the WTT\TestBinRoot dimension.
		//

		lpszTestBinRoot = (*pWTTSDK_GetParameterValue)
		(
			L"WTT\\TestBinRoot",
			&lpszErrorXML
		);

		if ( lpszTestBinRoot == nullptr )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				L"Could not find a value for WTT\\TestBinRoot."
			);
		}
		else
		{
			StringCchCopyW
			(
				NativeTestBinRoot,
				ARRAYSIZE( NativeTestBinRoot ),
				lpszTestBinRoot
			);

			(*pWTTSDK_WTTFreeMemory)( lpszTestBinRoot );
			lpszTestBinRoot = nullptr;
		}

		if ( lpszErrorXML != nullptr )
		{
			(*pWTTSDK_WTTFreeMemory)( lpszErrorXML );
			lpszErrorXML = nullptr;
		};


		//
		// Get the value of the WTT\WoW64TestBinRoot dimension.
		//

		lpszWoW64TestBinRoot = (*pWTTSDK_GetParameterValue)
		(
			L"WTT\\WoW64TestBinRoot",
			&lpszErrorXML
		);

		if ( lpszWoW64TestBinRoot == nullptr )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				L"Could not find a value for WTT\\WoW64TestBinRoot."
			);
		}
		else
		{
			StringCchCopyW
			(
				WoW64TestBinRoot,
				ARRAYSIZE( WoW64TestBinRoot ),
				lpszWoW64TestBinRoot
			);

			(*pWTTSDK_WTTFreeMemory)( lpszWoW64TestBinRoot );
			lpszWoW64TestBinRoot = nullptr;
		}

		if ( lpszErrorXML != nullptr )
		{
			(*pWTTSDK_WTTFreeMemory)( lpszErrorXML );
			lpszErrorXML = nullptr;
		}
	}
	else
	{
		//
		// Read the values from the command line.
		//
		// This provides an easy fallback in case the WTTSDK
		// routines can't be found (such as when a tester is
		// running the test locally rather than under WTT).
		// To use it, just set the environment variables
		// listed below:
		//

		/* DWORD nOSBinRoot = */ GetEnvironmentVariableW
		(
			L"OSBinRoot",
			NativeOSBinRoot,
			ARRAYSIZE( NativeOSBinRoot )
		);

		/* DWORD nTestBinRoot = */ GetEnvironmentVariableW
		(
			L"TestBinRoot",
			NativeTestBinRoot,
			ARRAYSIZE( NativeTestBinRoot )
		);

		/* DWORD nWoW64OSBinRoot = */ GetEnvironmentVariableW
		(
			L"WoW64OSBinRoot",
			WoW64OSBinRoot,
			ARRAYSIZE( WoW64OSBinRoot )
		);

		/* DWORD nWoW64TestBinRoot = */ GetEnvironmentVariableW
		(
			L"WoW64TestBinRoot",
			WoW64TestBinRoot,
			ARRAYSIZE( WoW64TestBinRoot )
		);
	};


	//
	// Build a symbol path from the OSBinRoot.
	//

	if ( wcslen( NativeOSBinRoot ) != 0 )
	{
		PWSTR pOSBinRootBinSubstring = wcsstr
		(
			NativeOSBinRoot,
			L"\\bin"
		);

		if ( pOSBinRootBinSubstring == nullptr )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				WEX::Common::String().Format
				(
					L"OSBinRoot value \"%s\" is not formatted as expected.",
					NativeOSBinRoot
				)
			);
		}
		else
		{
			pOSBinRootBinSubstring[ 0 ] = L'\0';

			StringCchCatW
			(
				NativeOSBinRoot,
				ARRAYSIZE( NativeOSBinRoot ),
				L"\\symbols.pri\\Indexes"
			);
		};
	};


	//
	// Build a symbol path from the TestBinRoot
	//

	if ( wcslen( NativeTestBinRoot ) != 0 )
	{
		PWSTR pTestBinRootSubstring = wcsstr
		(
			NativeTestBinRoot,
			L"\\bin"
		);

		if ( pTestBinRootSubstring == nullptr )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				WEX::Common::String().Format
				(
					L"TestBinRoot value \"%s\" is not formatted as expected.",
					NativeTestBinRoot
				)
			);
		}
		else
		{
			pTestBinRootSubstring[ 0 ] = L'\0';

			StringCchCatW
			(
				NativeTestBinRoot,
				ARRAYSIZE( NativeTestBinRoot ),
				L"\\symbols.pri\\Indexes"
			);
		};
	};


	//
	// Build a symbol path from the WoW64OSBinRoot.
	//

	if ( wcslen( WoW64OSBinRoot ) != 0 )
	{
		PWSTR pWoW64OSBinRootBinSubstring = wcsstr
		(
			WoW64OSBinRoot,
			L"\\bin"
		);

		if ( pWoW64OSBinRootBinSubstring == nullptr )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				WEX::Common::String().Format
				(
					L"WoW64OSBinRoot value \"%s\" is not formatted as expected.",
					WoW64OSBinRoot
				)
			);
		}
		else
		{
			pWoW64OSBinRootBinSubstring[ 0 ] = L'\0';

			StringCchCatW
			(
				WoW64OSBinRoot,
				ARRAYSIZE( WoW64OSBinRoot ),
				L"\\symbols.pri\\Indexes"
			);
		};
	};


	//
	// Build a symbol path from the WoW64TestBinRoot
	//

	if ( wcslen( WoW64TestBinRoot ) != 0 )
	{
		PWSTR pWoW64TestBinRootSubstring = wcsstr
		(
			WoW64TestBinRoot,
			L"\\bin"
		);

		if ( pWoW64TestBinRootSubstring == nullptr )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				WEX::Common::String().Format
				(
					L"WoW64TestBinRoot value \"%s\" is not formatted as expected.",
					WoW64TestBinRoot
				)
			);
		}
		else
		{
			pWoW64TestBinRootSubstring[ 0 ] = L'\0';

			StringCchCatW
			(
				WoW64TestBinRoot,
				ARRAYSIZE( WoW64TestBinRoot ),
				L"\\symbols.pri\\Indexes"
			);
		};
	};


	//
	// Get the current symbol path.
	//

	DWORD nSymbolPath = GetEnvironmentVariableW
	(
		L"_NT_SYMBOL_PATH",
		SymbolPath,
		ARRAYSIZE( SymbolPath )
	);


	//
	// Build the symbol path.
	//

	if ( nSymbolPath > 0 )
	{
		StringCchCatW
		(
			SymbolPath,
			ARRAYSIZE( SymbolPath ),
			L"; "
		);
	};

	if ( wcslen( NativeOSBinRoot ) > 0 )
	{
		StringCchCatW
		(
			SymbolPath,
			ARRAYSIZE( SymbolPath ),
			L"srv*"
		);

		StringCchCatW
		(
			SymbolPath,
			ARRAYSIZE( SymbolPath ),
			NativeOSBinRoot
		);
	};

	if ( wcslen( NativeTestBinRoot ) > 0 )
	{
		StringCchCatW
		(
			SymbolPath,
			ARRAYSIZE( SymbolPath ),
			L"; srv*"
		);

		StringCchCatW
		(
			SymbolPath,
			ARRAYSIZE( SymbolPath ),
			NativeTestBinRoot
		);
	};

	if ( wcslen( WoW64OSBinRoot ) > 0 )
	{
		StringCchCatW
		(
			SymbolPath,
			ARRAYSIZE( SymbolPath ),
			L"; srv*"
		);

		StringCchCatW
		(
			SymbolPath,
			ARRAYSIZE( SymbolPath ),
			WoW64OSBinRoot
		);
	};

	if ( wcslen( WoW64TestBinRoot ) > 0 )
	{
		StringCchCatW
		(
			SymbolPath,
			ARRAYSIZE( SymbolPath ),
			L"; srv*"
		);

		StringCchCatW
		(
			SymbolPath,
			ARRAYSIZE( SymbolPath ),
			WoW64TestBinRoot
		);
	};


	//
	// Set the symbol path.
	//

	if ( wcslen( SymbolPath ) > 0 )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"Setting _NT_SYMBOL_PATH to '%s'",
				SymbolPath
			)
		);

		BOOL bSetSymbolPathResult = SetEnvironmentVariableW
		(
			L"_NT_SYMBOL_PATH",
			SymbolPath
		);

		if ( bSetSymbolPathResult != TRUE )
		{
			DWORD SetEnvironmentVariableError = GetLastError();

			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				L"SetEnvironmentVariable on _NT_SYMBOL_PATH failed."
			);

			hOverallResult = HRESULT_FROM_WIN32( SetEnvironmentVariableError );
			goto Cleanup;
		};

		hOverallResult = S_OK;
	}
	else
	{
		hOverallResult = S_FALSE;
	};


Cleanup:
	if ( lpszErrorXML != nullptr )
	{
		assert( pWTTSDK_WTTFreeMemory != nullptr );
		(*pWTTSDK_WTTFreeMemory)( lpszErrorXML );
		lpszErrorXML = nullptr;
	};

	if ( lpszOSBinRoot != nullptr )
	{
		assert( pWTTSDK_WTTFreeMemory != nullptr );
		(*pWTTSDK_WTTFreeMemory)( lpszOSBinRoot );
		lpszOSBinRoot = nullptr;
	};

	if ( lpszTestBinRoot != nullptr )
	{
		assert( pWTTSDK_WTTFreeMemory != nullptr );
		(*pWTTSDK_WTTFreeMemory)( lpszTestBinRoot );
		lpszTestBinRoot = nullptr;
	};

	if ( lpszWoW64OSBinRoot != nullptr )
	{
		assert( pWTTSDK_WTTFreeMemory != nullptr );
		(pWTTSDK_WTTFreeMemory)( lpszWoW64OSBinRoot );
		lpszWoW64OSBinRoot = nullptr;
	};

	if ( lpszWoW64TestBinRoot != nullptr )
	{
		assert( pWTTSDK_WTTFreeMemory != nullptr );
		(pWTTSDK_WTTFreeMemory)( lpszWoW64TestBinRoot );
		lpszWoW64TestBinRoot = nullptr;
	};


	if ( pWTTSDK_WTTSDKCleanup != nullptr )
	{
		(*pWTTSDK_WTTSDKCleanup)();
	};


	pWTTSDK_GetParameterValue = nullptr;
	pWTTSDK_WTTFreeMemory = nullptr;
	pWTTSDK_WTTSDKCleanup = nullptr;


	if ( hWTTSDK != nullptr )
	{
		FreeLibrary( hWTTSDK );
		hWTTSDK = nullptr;
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  GetVariation
//
// Purpose:
// Parses the Variation command-line parameter to
// determine the set of variations to execute.
///////////////////////////////////////////////////////
void CGraphicsTAEFTest::GetVariations()
{
	WEX::Common::String VariationParameterValue;

	HRESULT hr = WEX::TestExecution::RuntimeParameters::TryGetValue
	(
		L"Variation",
		VariationParameterValue
	);

	if ( SUCCEEDED( hr ) )
	{
		typedef std::vector< tstring > StringVectorType;
		StringVectorType StringVariations;
		std::insert_iterator< StringVectorType > itVariation( StringVariations, StringVariations.end() );
		m_VariationCount = TokenizeString
		(
			itVariation,
			ToString( static_cast< std::wstring >( VariationParameterValue ) ).c_str(),
			_T( "- " )
		);

		if ( m_VariationCount >= 1 )
		{
			FromString
			(
				&m_VariationStartParam,
				StringVariations[ 0 ]
			);
		};

		if ( m_VariationCount == 2 )
		{
			FromString
			(
				&m_VariationEndParam,
				StringVariations[ 1 ]
			);
		};
	};
};



///////////////////////////////////////////////////////
// Function:  ComputeVariations
//
// Purpose:
// Calculates the number of variations to be run.
///////////////////////////////////////////////////////
HRESULT CGraphicsTAEFTest::ComputeVariations
(
	size_t numVariations
)
{
	HRESULT hr = S_OK;

	m_VariationStartNumber = 1;
	m_VariationEndNumber = numVariations + 1;

	if ( m_VariationCount > 0 )
	{
		size_t testCount = 1;
		if ( m_VariationCount == 2 )
		{
			if ( m_VariationEndParam > m_VariationStartParam )
			{
				testCount = ( m_VariationEndParam - m_VariationStartParam ) + 1;
			}
			else
			{
				hr = E_FAIL;
			};
		};

		m_VariationStartNumber =  min( max( m_VariationStartParam, m_VariationStartNumber ), m_VariationEndNumber - 1 );
		m_VariationEndNumber = min( m_VariationStartNumber + testCount, m_VariationEndNumber );
	};

	return hr;
};


///////////////////////////////////////////////////////
// Function:  IsOperatingSystemCheckedBuild
//
// Purpose:
// Determines whether the test is executing on a
// checked build of the operating system.
///////////////////////////////////////////////////////
bool CGraphicsTAEFTest::IsOperatingSystemCheckedBuild() const
{
	//
	// Determine whether we're on a checked OS build.
	//

	bool bIsCheckedBuild = ( GetSystemMetrics( SM_DEBUG ) != 0 );

	if ( bIsCheckedBuild == true )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CGraphicsTAEFTest::IsOperatingSystemCheckedBuild:  Checked build detected."
		);
	}
	else
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CGraphicsTAEFTest::IsOperatingSystemCheckedBuild:  Free build detected."
		);
	};


	return bIsCheckedBuild;
};


///////////////////////////////////////////////////////
// Function:  IsOperatingSystemCoverageBuild
//
// Purpose:
// Determines whether the test is executing on an
// instrumented build of the operating system.
///////////////////////////////////////////////////////
bool CGraphicsTAEFTest::IsOperatingSystemCoverageBuild() const
{
	//
	// Initialize locals.
	//

	bool bIsCoverageBuild = false;


	typedef BOOL (WINAPI * PENUMPROCESSES)
	(
		_Out_writes_bytes_(cb) DWORD * lpidProcess,
		_In_ DWORD cb,
		_Out_ LPDWORD lpcbNeeded
	);

	PENUMPROCESSES pKernel32_K32EnumProcesses = nullptr;


	typedef BOOL (WINAPI * PQUERYFULLPROCESSIMAGENAMEW)
	(
		_In_ HANDLE hProcess,
		_In_ DWORD dwFlags,
		_Out_writes_to_(*lpdwSize, *lpdwSize) LPWSTR lpExeName,
		_Inout_ PDWORD lpdwSize
	);

	PQUERYFULLPROCESSIMAGENAMEW pKernel32_QueryFullProcessImageNameW = nullptr;


	//
	// Find PSAPI entry points in Kernel32.dll.
	//

	HMODULE hKernel32 = LoadLibraryExW
	(
		L"Kernel32.dll",
		nullptr,
		0
	);

	if ( hKernel32 == nullptr )
	{
		goto Cleanup;
	};


	pKernel32_K32EnumProcesses = reinterpret_cast< PENUMPROCESSES >
	(
		GetProcAddress
		(
			hKernel32,
			"K32EnumProcesses"
		)
	);

	if ( pKernel32_K32EnumProcesses == nullptr )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_WARNING,
			L"GetProcAddress on K32EnumProcesses failed."
		);

		goto Cleanup;
	};


	pKernel32_QueryFullProcessImageNameW = reinterpret_cast< PQUERYFULLPROCESSIMAGENAMEW >
	(
		GetProcAddress
		(
			hKernel32,
			"QueryFullProcessImageNameW"
		)
	);

	if ( pKernel32_QueryFullProcessImageNameW == nullptr )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_WARNING,
			L"GetProcAddress on QueryFullProcessImageNameW failed."
		);

		goto Cleanup;
	};


	//
	// Enumerate processes.
	//

	DWORD ProcessIDs[ 1000 ] = { 0 };
	DWORD nProcessIDs = 0;
	BOOL bEnumProcessesResult = (*pKernel32_K32EnumProcesses)
	(
		ProcessIDs,
		sizeof( ProcessIDs ),
		&nProcessIDs
	);

	if ( bEnumProcessesResult != TRUE )
	{
		// If we can't enumerate processes, assume false.
		goto Cleanup;
	};


	//
	// Walk the list of process IDs, attempting to open
	// each one and get its name.
	//

	DWORD cProcesses = nProcessIDs / sizeof(DWORD);
	for
	(
		UINT iCurrentProcess = 0;
		iCurrentProcess < cProcesses;
		++iCurrentProcess
	)
	{
		// Attempt to open the process...
		HANDLE hCurrentProcess = OpenProcess
		(
			PROCESS_QUERY_LIMITED_INFORMATION,
			FALSE,
			ProcessIDs[ iCurrentProcess ]
		);

		// ...and if we can't...
		if ( hCurrentProcess == nullptr )
		{
			// ...then move on.
			continue;
		};


		// Attempt to get the process name...
		WCHAR CurrentProcessName[ MAX_PATH ] = L"";
		DWORD nCurrentProcessName = ARRAYSIZE( CurrentProcessName );
		BOOL bQueryProcessNameResult = (*pKernel32_QueryFullProcessImageNameW)
		(
			hCurrentProcess,
			0,
			CurrentProcessName,
			&nCurrentProcessName
		);

		// ...and if we can't...
		if ( bQueryProcessNameResult != TRUE )
		{
			// ...then close the process handle...
			CloseHandle( hCurrentProcess );
			hCurrentProcess = nullptr;

			// ...and move on.
			continue;
		};


		// Look for the coverage monitor process, which is
		// our clue that we're running on a coverage build.
		//
		// N.B. SEdmison:
		// Yeah, it's hacky to check for these two strings
		// rather than to do a true case-insensitive search,
		// but doing the work to convert the whole string to
		// upper case and then search it is essentially two
		// passes through the string, and this way is two
		// passes through the string with a lot less code
		// (and I don't imagine that someone's going to
		// name the binary cOVmON.exe or something anyway).
		// StrStrIW is a possibility in this case, if we're
		// willing to take a dependency on shlwapi.dll....
		PWSTR pCovMonSubstring = wcsstr
		(
			CurrentProcessName,
			L"\\CovMon.exe"
		);

		if ( pCovMonSubstring == nullptr )
		{
			pCovMonSubstring = wcsstr
			(
				CurrentProcessName,
				L"\\covmon.exe"
			);
		};

		CloseHandle( hCurrentProcess );
		hCurrentProcess = nullptr;

		// If we found the coverage monitor...
		if ( pCovMonSubstring != nullptr )
		{
			// ...then we're on a coverage build.
			bIsCoverageBuild = true;
			goto Cleanup;
		};
	};


Cleanup:
	//
	// Free Kernel32.dll.
	//

	pKernel32_K32EnumProcesses = nullptr;
	pKernel32_QueryFullProcessImageNameW = nullptr;

	if ( hKernel32 != nullptr )
	{
		FreeLibrary( hKernel32 );
		hKernel32 = nullptr;
	};


	if ( bIsCoverageBuild == true )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CGraphicsTAEFTest::IsOperatingSystemCoverageBuild:  Code coverage build detected."
		);
	};


	return bIsCoverageBuild;
};


///////////////////////////////////////////////////////
// Function:  LogTestGroupOverallResult
//
// Purpose:
// Logs the overallResult to the WEX logger
///////////////////////////////////////////////////////
void CGraphicsTAEFTest::LogTestGroupOverallResult
(
	TESTCASE_RESULT TestGroupOverallResult
) const
{
	//
	// Log a result for the test case that TAEF created for this
	// test method.
	//
	switch ( TestGroupOverallResult )
	{
		case TESTCASE_RESULT_PASSED:
		{
			WEX::Logging::Log::Result
			(
				WEX::Logging::TestResults::Passed,
				L"All test cases passed."
			);
		} break;

		case TESTCASE_RESULT_SKIPPED:
		{
			WEX::Logging::Log::Result
			(
				WEX::Logging::TestResults::Skipped,
				L"No test cases were run."
			);
		} break;

		case TESTCASE_RESULT_ABORTED:
		{
			WEX::Logging::Log::Result
			(
				WEX::Logging::TestResults::Blocked,
				L"One or more test cases were aborted."
			);
		} break;

		case TESTCASE_RESULT_FAILED:
		{
			WEX::Logging::Log::Result
			(
				WEX::Logging::TestResults::Failed,
				L"One or more test cases failed."
			);
		} break;

		default:
		{
			assert( false && L"Unhandled test case result type." );
			WEX::Logging::Log::Result
			(
				WEX::Logging::TestResults::Blocked,
				L"Test group returned an unrecognized result."
			);
		} break;
	};
};
