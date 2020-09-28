//////////////////////////////////////////////////////////////////////
// File:  CSwapChainsTest.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of DXGI swap chain conformance test group class.
//
// History:
// 07 Nov 2013  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "CSwapChainsTest.hpp"


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Helper function implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  FromString( D3D_FEATURE_LEVEL )
//
// Purpose:
// Converts a string to a Direct3D feature level.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	_Out_ D3D_FEATURE_LEVEL * pFeatureLevel,
	const tstring & FeatureLevelString
)
{
	if ( pFeatureLevel == nullptr )
	{
		return false;
	};

	*pFeatureLevel = D3D_FEATURE_LEVEL_9_1;


	if ( FeatureLevelString == _T("D3D_FEATURE_LEVEL_9_1") )
	{
		*pFeatureLevel = D3D_FEATURE_LEVEL_9_1;
		return true;
	};

	if ( FeatureLevelString == _T("D3D_FEATURE_LEVEL_9_2") )
	{
		*pFeatureLevel = D3D_FEATURE_LEVEL_9_2;
		return true;
	};

	if ( FeatureLevelString == _T("D3D_FEATURE_LEVEL_9_3") )
	{
		*pFeatureLevel = D3D_FEATURE_LEVEL_9_3;
		return true;
	};

	if ( FeatureLevelString == _T("D3D_FEATURE_LEVEL_10_0") )
	{
		*pFeatureLevel = D3D_FEATURE_LEVEL_10_0;
		return true;
	};

	if ( FeatureLevelString == _T("D3D_FEATURE_LEVEL_10_1") )
	{
		*pFeatureLevel = D3D_FEATURE_LEVEL_10_1;
		return true;
	};

	if ( FeatureLevelString == _T("D3D_FEATURE_LEVEL_11_0") )
	{
		*pFeatureLevel = D3D_FEATURE_LEVEL_11_0;
		return true;
	};

	if ( FeatureLevelString == _T("D3D_FEATURE_LEVEL_11_1") )
	{
		*pFeatureLevel = D3D_FEATURE_LEVEL_11_1;
		return true;
	};

	return false;
};


///////////////////////////////////////////////////////
// Function:  FromString( SCENE_TYPE )
//
// Purpose:
// Converts a scene type string to a scene type.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	_Out_ SCENE_TYPE * pSceneType,
	const std::wstring & SceneTypeString
)
{
	if ( pSceneType == nullptr )
	{
		return false;
	};


	*pSceneType = SCENE_TYPE_NONE;


	if ( SceneTypeString == L"SCENE_TYPE_CUBE_SHADED" )
	{
		*pSceneType = SCENE_TYPE_CUBE_SHADED;
		return true;
	};

	if ( SceneTypeString == L"SCENE_TYPE_PLANE_TEXTURED" )
	{
		*pSceneType = SCENE_TYPE_PLANE_TEXTURED;
		return true;
	};

	if ( SceneTypeString == L"SCENE_TYPE_SWATCH_FIXEDCOLOR" )
	{
		*pSceneType = SCENE_TYPE_SWATCH_FIXEDCOLOR;
		return true;
	};

	if ( SceneTypeString == L"SCENE_TYPE_SWATCH_RANDOMCOLOR" )
	{
		*pSceneType = SCENE_TYPE_SWATCH_RANDOMCOLOR;
		return true;
	};

	if ( SceneTypeString == L"SCENE_TYPE_SWATCH_TRANSPARENT" )
	{
		*pSceneType = SCENE_TYPE_SWATCH_TRANSPARENT;
		return true;
	};

	if ( SceneTypeString == L"SCENE_TYPE_PATTERN" )
	{
		*pSceneType = SCENE_TYPE_PATTERN;
		return true;
	};

	if ( SceneTypeString == L"SCENE_TYPE_NONE" )
	{
		*pSceneType = SCENE_TYPE_NONE;
		return true;
	};


	return false;
};


///////////////////////////////////////////////////////
// Function:  FromString( TEST_API )
//
// Purpose:
// Converts an API name into an API-under-test
// enumerant.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	_Out_ TEST_API * pTestAPI,
	const std::wstring & TestAPIString
)
{
	if ( pTestAPI == nullptr )
	{
		return false;
	};


	// @REVIEW:  Add a TEST_API_NONE?
	*pTestAPI = TEST_API_FIRST;


	if ( TestAPIString == L"TEST_API_D3D11" )
	{
		*pTestAPI = TEST_API_D3D11;
		return true;
	};

	if ( TestAPIString == L"TEST_API_D3D12" )
	{
		*pTestAPI = TEST_API_D3D12;
		return true;
	};


	return false;
};


///////////////////////////////////////////////////////
// Function:  ToString( D3D_DRIVER_TYPE )
//
// Purpose:
// Provides a string representation for a Direct3D
// driver type.
///////////////////////////////////////////////////////
tstring ToString( const D3D_DRIVER_TYPE & DriverType )
{
	switch ( DriverType )
	{
		case D3D_DRIVER_TYPE_UNKNOWN:
		{
			return tstring( _T( "D3D_DRIVER_TYPE_UNKNOWN" ) );
		} break;

		case D3D_DRIVER_TYPE_HARDWARE:
		{
			return tstring( _T( "D3D_DRIVER_TYPE_HARDWARE" ) );
		} break;

		case D3D_DRIVER_TYPE_REFERENCE:
		{
			return tstring( _T( "D3D_DRIVER_TYPE_REFERENCE" ) );
		} break;

		case D3D_DRIVER_TYPE_NULL:
		{
			return tstring( _T( "D3D_DRIVER_TYPE_NULL" ) );
		} break;

		case D3D_DRIVER_TYPE_SOFTWARE:
		{
			return tstring( _T( "D3D_DRIVER_TYPE_SOFTWARE" ) );
		} break;

		case D3D_DRIVER_TYPE_WARP:
		{
			return tstring( _T( "D3D_DRIVER_TYPE_WARP" ) );
		} break;

		default:
		{
			return ToString( (UINT) DriverType );
		} break;
	};
};


///////////////////////////////////////////////////////
// Function:  ToString( D3D_FEATURE_LEVEL )
//
// Purpose:
// Provides a string representation for a Direct3D
// feature level.
///////////////////////////////////////////////////////
tstring ToString( const D3D_FEATURE_LEVEL & FeatureLevel )
{
	switch ( FeatureLevel )
	{
		case D3D_FEATURE_LEVEL_9_1:
		{
			return tstring( _T( "D3D_FEATURE_LEVEL_9_1" ) );
		} break;

		case D3D_FEATURE_LEVEL_9_2:
		{
			return tstring( _T( "D3D_FEATURE_LEVEL_9_2" ) );
		} break;

		case D3D_FEATURE_LEVEL_9_3:
		{
			return tstring( _T( "D3D_FEATURE_LEVEL_9_3" ) );
		} break;

		case D3D_FEATURE_LEVEL_10_0:
		{
			return tstring( _T( "D3D_FEATURE_LEVEL_10_0" ) );
		} break;

		case D3D_FEATURE_LEVEL_10_1:
		{
			return tstring( _T( "D3D_FEATURE_LEVEL_10_1" ) );
		} break;

		case D3D_FEATURE_LEVEL_11_0:
		{
			return tstring( _T( "D3D_FEATURE_LEVEL_11_0" ) );
		} break;

		case D3D_FEATURE_LEVEL_11_1:
		{
			return tstring( _T( "D3D_FEATURE_LEVEL_11_1" ) );
		} break;

		default:
		{
			return ToString( (UINT) FeatureLevel );
		} break;
	};
};


////////////////////////////////////////////////////////////
//
// CSwapChainsTest implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CSwapChainsTest
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CSwapChainsTest::CSwapChainsTest()
:
	CGraphicsTAEFTest(),
	m_pCoreWindowWrapper( nullptr ),
	m_pSystemWindowWrapper( nullptr ),
	m_hCompositionSurface( nullptr )
#if defined(USE_ADVANCED_DCOMPISOLATION_MODEL)
	,
	m_pDCompDeviceWrapper( nullptr ),
	m_pDCompTargetWrapper( nullptr )
#endif
#if USE_DWMSLICER
	,
	m_pDWMSlicer( nullptr ),
	m_cCapturedFrames( 0 )
#endif
#if USE_SCREENCAPTURE
	,
	m_pScreenCapture( nullptr )
#endif
{
	ZeroMemory( m_ppSceneRenderers, sizeof( m_ppSceneRenderers ) );


	//
	// Assign the DirectComposition wrapper a logger.
	//

	m_DCompIsolation.SetLogger( m_pLogger );
};


///////////////////////////////////////////////////////
// Function:  ~CSwapChainsTest
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CSwapChainsTest::~CSwapChainsTest()
{
	//
	// Check for leaked objects.
	//

#if defined(USE_ADVANCED_DCOMPISOLATION_MODEL)
	assert( m_pDCompTargetWrapper == nullptr );

	assert( m_pDCompDeviceWrapper == nullptr );
#endif

	assert( m_hCompositionSurface == nullptr );

	assert( m_pCoreWindowWrapper == nullptr );

	assert( m_pSystemWindowWrapper == nullptr );

	for
	(
		TEST_API CurrentRenderer = TEST_API_FIRST;
		CurrentRenderer < TEST_API_COUNT;
		CurrentRenderer = (TEST_API) ( CurrentRenderer + 1 )
	)
	{
		assert( m_ppSceneRenderers[ CurrentRenderer ] == nullptr );
	};
};


///////////////////////////////////////////////////////
// Function:  SetupTestGroup
//
// Purpose:
// Handles test-group-wide setup.
///////////////////////////////////////////////////////
bool CSwapChainsTest::SetupTestGroup()
{
	//
	// Initialize locals.
	//

	bool bOverallResult = false;


	//
	// Validate members and state.
	//

	if ( m_pLogger == nullptr )
	{
		goto Cleanup;
	};


	//
	// Start a test group in the log.
	//

	m_pLogger->BeginTestGroup
	(
		L"Swap Chain Rendering SetupTestGroup",
		1,
		TESTCASE_RESULT_SKIPPED
	);


	//
	// Begin a test case in the log.
	//

	m_pLogger->BeginTestCase
	(
		L"SetupTestGroup",
		0xFFFFFFFF,
		nullptr
	);


	//
	// Call the base class implementation.
	//

	if ( __super::SetupTestGroup() == false )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Base class SetupTestGroup failed."
		);

		goto Cleanup;
	};


#if USE_DWMSLICER
	//
	// Initalize the RAD image pipeline.
	//

	try
	{
		ImagePipeline::InitializeRad();
	}
	catch ( ImagePipeline::DxException e )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			e.GetHRESULT(),
			WEX::Common::String().Format
			(
				L"InitializeRad threw exception 0x%x:  %s.",
				e.GetHRESULT(),
				e.GetMessage().c_str()
			)
		);

		goto Cleanup;
	};


	//
	// Create a DWM slicer.
	//

	m_pDWMSlicer = new( std::nothrow ) ImagePipeline::DwmSlicer();
	if ( m_pDWMSlicer == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Failed to allocate DWM slicer."
		);

		goto Cleanup;
	};
#endif


#if USE_SCREENCAPTURE
	//
	// Create a screen capture helper.
	//

	m_pScreenCapture = new( std::nothrow ) CScreenCapture();

	if ( m_pScreenCapture == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Failed to allocate screen capture helper."
		);

		goto Cleanup;
	};


	//
	// Initialize the screen capture helper.
	//

	HRESULT hInitializeScreenCaptureHelperResult = m_pScreenCapture->Initialize();

	if ( FAILED( hInitializeScreenCaptureHelperResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hInitializeScreenCaptureHelperResult,
			L"Initialize on screen capture helper failed."
		);

		goto Cleanup;
	};
#endif


	//
	// Build the matrix of expected results.
	//

	SetExpectedResults();


	//
	// If we made it this far, we're good to go.
	//

	bOverallResult = true;


Cleanup:
	if ( bOverallResult != true )
	{
		CleanupTestGroup();
	};


	TESTCASE_RESULT TestGroupOverallResult = TESTCASE_RESULT_FAILED;
	if ( m_pLogger != nullptr )
	{
		//
		// Set the test case result.
		//

		if ( bOverallResult == true )
		{
			m_pLogger->SetTestCaseResult
			(
				TESTCASE_RESULT_PASSED,
				L"All setup tasks completed successfully."
			);
		}
		else
		{
			m_pLogger->SetTestCaseResult
			(
				TESTCASE_RESULT_ABORTED,
				L"One or more setup tasks did not complete successfully."
			);
		};


		//
		// End the test case in the log.
		//

		m_pLogger->EndTestCase();


		//
		// Get the test group overall result.
		//

		TestGroupOverallResult = m_pLogger->GetTestGroupOverallResult();


		//
		// End the test group in the log.
		//

		m_pLogger->EndTestGroup();


		//
		// Log the test group overall result.
		//

		LogTestGroupOverallResult( TestGroupOverallResult );
	};


	return bOverallResult;
};


///////////////////////////////////////////////////////
// Function:  CleanupTestGroup
//
// Purpose:
// Handles test-group-wide cleanup.
///////////////////////////////////////////////////////
bool CSwapChainsTest::CleanupTestGroup()
{
	if ( m_pLogger != nullptr )
	{
		//
		// Begin a test group in the log.
		//

		m_pLogger->BeginTestGroup
		(
			L"Swap Chain Rendering CleanupTestGroup",
			5,
			TESTCASE_RESULT_SKIPPED
		);


		//
		// Begin a test case in the log.
		//

		m_pLogger->BeginTestCase
		(
			L"CleanupTestGroup",
			0xFFFFFFFF,
			nullptr
		);
	};


#if USE_DWMSLICER
	//
	// Have DWM Slicer clean up.
	//

	if ( m_pDWMSlicer != nullptr )
	{
		delete m_pDWMSlicer;
		m_pDWMSlicer = nullptr;

		ImagePipeline::UninitializeRad();
	};
#endif

#if USE_SCREENCAPTURE
	//
	// Have the screen capture helper clean up.
	//

	if ( m_pScreenCapture != nullptr )
	{
		m_pScreenCapture->Uninitialize();

		delete m_pScreenCapture;
		m_pScreenCapture = nullptr;
	};
#endif


	//
	// Call the base class implementation.
	//

	bool bOverallResult = __super::CleanupTestGroup();


	TESTCASE_RESULT TestGroupOverallResult = TESTCASE_RESULT_FAILED;
	if ( m_pLogger != nullptr )
	{
		//
		// Set the test case result.
		//

		if ( bOverallResult == true )
		{
			m_pLogger->SetTestCaseResult
			(
				TESTCASE_RESULT_PASSED,
				L"All cleanup tasks completed successfully."
			);
		}
		else
		{
			m_pLogger->SetTestCaseResult
			(
				TESTCASE_RESULT_FAILED,
				L"One or more cleanup tasks failed."
			);
		};


		//
		// End the test case in the log.
		//

		m_pLogger->EndTestCase();


		//
		// Get the test group overall result.
		//

		TestGroupOverallResult = m_pLogger->GetTestGroupOverallResult();


		//
		// End the test group in the log.
		//

		m_pLogger->EndTestGroup();


		//
		// Log the test group overall result.
		//

		LogTestGroupOverallResult( TestGroupOverallResult );
	};


	return bOverallResult;
};


///////////////////////////////////////////////////////
// Function:  SetupTestCase
//
// Purpose:
// Handles per-test-case setup.
///////////////////////////////////////////////////////
bool CSwapChainsTest::SetupTestCase()
{
	//
	// Initialize locals.
	//

	bool bErrorsWereDetected = true;
	bool bContinueToTestExecution = false;


	//
	// Validate members and state.
	//

	if ( m_pLogger == nullptr )
	{
		goto Cleanup;
	};


	//
	// Begin a test group in the log.
	//

	m_pLogger->BeginTestGroup
	(
		L"Swap Chain Rendering SetupTestCase",
		2,
		TESTCASE_RESULT_SKIPPED
	);


	//
	// Begin a test case in the log.
	//

	m_pLogger->BeginTestCase
	(
		L"SetupTestCase",
		0xFFFFFFFF,
		nullptr
	);


	//
	// Call the base class implementation.
	//

	if ( __super::SetupTestCase() != true )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Base class SetupTestCase failed."
		);

		goto Cleanup;
	};


	//
	// Initialize the DirectComposition wrapper object.
	//

	HRESULT hInitializeDCompIsolationResult = m_DCompIsolation.Initialize();

	if ( FAILED( hInitializeDCompIsolationResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hInitializeDCompIsolationResult,
			L"Initialize on DirectComposition wrapper object failed."
		);

		goto Cleanup;
	};


	//
	// If we made it this far, then we're good to go.
	//

	bContinueToTestExecution = true;
	bErrorsWereDetected = false;


Cleanup:
	TESTCASE_RESULT TestGroupOverallResult = TESTCASE_RESULT_FAILED;
	if ( m_pLogger != nullptr )
	{
		//
		// Set the test case result.
		//

		if ( bErrorsWereDetected == false )
		{
			if ( bContinueToTestExecution == true )
			{
				m_pLogger->SetTestCaseResult
				(
					TESTCASE_RESULT_PASSED,
					L"All setup tasks completed successfully."
				);
			}
			else
			{
				m_pLogger->SetTestCaseResult
				(
					TESTCASE_RESULT_SKIPPED,
					L"A capability required by this test case was not found."
				);
			};
		}
		else
		{
			m_pLogger->SetTestCaseResult
			(
				TESTCASE_RESULT_ABORTED,
				L"One or more setup tasks failed."
			);
		};


		//
		// End the test case in the log.
		//

		m_pLogger->EndTestCase();


		//
		// Get the test group overall result.
		//

		TestGroupOverallResult = m_pLogger->GetTestGroupOverallResult();


		//
		// End the test group in the log.
		//

		m_pLogger->EndTestGroup();


		//
		// Log the test group overall result.
		//

		LogTestGroupOverallResult( TestGroupOverallResult );
	};


	if
	(
		bErrorsWereDetected == true ||
		bContinueToTestExecution != true
	)
	{
		CleanupTestCase();
	};


	return bErrorsWereDetected != true;
};


///////////////////////////////////////////////////////
// Function:  CleanupTestCase
//
// Purpose:
// Handles per-test-case cleanup.
///////////////////////////////////////////////////////
bool CSwapChainsTest::CleanupTestCase()
{
	//
	// Initialize locals.
	//

	bool bOverallResult = false;


	if ( m_pLogger != nullptr )
	{
		//
		// Begin a test group in the log.
		//

		m_pLogger->BeginTestGroup
		(
			L"Swap Chain Rendering CleanupTestCase",
			4,
			TESTCASE_RESULT_SKIPPED
		);


		//
		// Begin a test case in the log.
		//

		m_pLogger->BeginTestCase
		(
			L"CleanupTestCase",
			0xFFFFFFFF,
			nullptr
		);
	};


	//
	// Destroy the system window wrapper.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"Destroying system window wrapper."
	);

	if ( m_pSystemWindowWrapper != nullptr )
	{
		HRESULT hStopThreadResult = m_pSystemWindowWrapper->StopSystemWindowThread();

		if ( FAILED( hStopThreadResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hStopThreadResult,
				L"StopSystemWindowThread failed."
			);
		};

		delete m_pSystemWindowWrapper;
		m_pSystemWindowWrapper = nullptr;
	};


	//
	// Destroy the core window wrapper.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"Destroying immersive window wrapper."
	);

	if ( m_pCoreWindowWrapper != nullptr )
	{
		HRESULT hStopThreadResult = m_pCoreWindowWrapper->StopImmersiveWindowThread();

		if ( FAILED( hStopThreadResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hStopThreadResult,
				L"StopImmersiveWindowThread failed."
			);
		};

		delete m_pCoreWindowWrapper;
		m_pCoreWindowWrapper = nullptr;
	};


	//
	// Destroy scene renderers.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"Destroying scene renderers."
	);

	for
	(
		TEST_API CurrentRenderer = TEST_API_FIRST;
		CurrentRenderer < TEST_API_COUNT;
		CurrentRenderer = (TEST_API) ( CurrentRenderer + 1 )
	)
	{
		if ( m_ppSceneRenderers[ CurrentRenderer ] != nullptr )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_DEBUG,
				WEX::Common::String().Format
				(
					L"- Destroying scene renderer %u.",
					CurrentRenderer
				)
			);

			m_ppSceneRenderers[ CurrentRenderer ]->Uninitialize();

			delete m_ppSceneRenderers[ CurrentRenderer ];
			m_ppSceneRenderers[ CurrentRenderer ] = nullptr;
		};
	};


	//
	// Clean up DirectComposition state.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"Destroying DirectComposition objects."
	);

#if defined(USE_ADVANCED_DCOMPISOLATION_MODEL)
	if ( m_pDCompTargetWrapper != nullptr )
	{
		m_pDCompTargetWrapper->Cleanup();

		delete m_pDCompTargetWrapper;
		m_pDCompTargetWrapper = nullptr;
	};

	if ( m_pDCompDeviceWrapper != nullptr )
	{
		m_pDCompDeviceWrapper->Cleanup();

		delete m_pDCompDeviceWrapper;
		m_pDCompDeviceWrapper = nullptr;
	};
#endif

	if ( m_hCompositionSurface != nullptr )
	{
		CloseHandle( m_hCompositionSurface );
		m_hCompositionSurface = nullptr;
	};


	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"Destroying DirectComposition wrapper."
	);

	m_DCompIsolation.Cleanup();


	//
	// Call the base class implementation.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"Calling base class implementation of CleanupTestCase."
	);

	bOverallResult = __super::CleanupTestCase();


	TESTCASE_RESULT TestGroupOverallResult = TESTCASE_RESULT_FAILED;
	if ( m_pLogger != nullptr )
	{
		//
		// Set the test case result.
		//

		if ( bOverallResult == true )
		{
			m_pLogger->SetTestCaseResult
			(
				TESTCASE_RESULT_PASSED,
				L"All cleanup tasks completed successfully."
			);
		}
		else
		{
			m_pLogger->SetTestCaseResult
			(
				TESTCASE_RESULT_FAILED,
				L"One or more cleanup tasks failed."
			);
		};


		//
		// End the test case in the log.
		//

		m_pLogger->EndTestCase();


		//
		// Get the test group overall result.
		//

		TestGroupOverallResult = m_pLogger->GetTestGroupOverallResult();


		//
		// End the test group in the log.
		//

		m_pLogger->EndTestGroup();


		//
		// Log the test group overall result.
		//

		LogTestGroupOverallResult( TestGroupOverallResult );
	};

	return bOverallResult;
};


///////////////////////////////////////////////////////
// Function:  GetDesktopCoordinatesForDefaultOutput
//
// Purpose:
// Retrieves the desktop coordinates for output 0 on
// adapter 0.
///////////////////////////////////////////////////////
HRESULT CSwapChainsTest::GetDesktopCoordinatesForDefaultOutput
(
	_Out_ LONG * pDefaultOutputLeft,
	_Out_ LONG * pDefaultOutputTop,
	_Out_ LONG * pDefaultOutputRight,
	_Out_ LONG * pDefaultOutputBottom
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	IDXGIFactory4 * pFactory = nullptr;
	IDXGIAdapter1 * pAdapter = nullptr;
	IDXGIOutput * pOutput = nullptr;
	IDXGIOutput1 * pOutput_IDXGIOutput1 = nullptr;


	//
	// Validate parameters.
	//

	if ( pDefaultOutputLeft == nullptr )
	{
		hOverallResult = E_POINTER;
		goto Cleanup;
	};

	*pDefaultOutputLeft = 0;

	if ( pDefaultOutputTop == nullptr )
	{
		hOverallResult = E_POINTER;
		goto Cleanup;
	};

	*pDefaultOutputTop = 0;

	if ( pDefaultOutputRight == nullptr )
	{
		hOverallResult = E_POINTER;
		goto Cleanup;
	};

	*pDefaultOutputRight = 0;

	if ( pDefaultOutputBottom == nullptr )
	{
		hOverallResult = E_POINTER;
		goto Cleanup;
	};

	*pDefaultOutputRight = 0;


	//
	// Create a DXGI factory.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CSwapChainsTest::GetDesktopCoordinatesForDefaultOutput:  Creating DXGI factory."
	);

	UINT fFactoryCreation = 0;

#if defined(DEBUG)
	fFactoryCreation |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	if ( IsDebuggerPresent() == TRUE )
	{
		fFactoryCreation |= DXGI_CREATE_FACTORY_DEBUG;
	};

	HRESULT hCreateFactoryResult = CreateDXGIFactory2
	(
		fFactoryCreation,
		IID_IDXGIFactory4,
		(void **) &pFactory
	);

	if ( FAILED( hCreateFactoryResult ) )
	{
		if ( ( fFactoryCreation & DXGI_CREATE_FACTORY_DEBUG ) != 0 )
		{
			fFactoryCreation &= ~DXGI_CREATE_FACTORY_DEBUG;

			hCreateFactoryResult = CreateDXGIFactory2
			(
				fFactoryCreation,
				IID_IDXGIFactory4,
				(void **) &pFactory
			);
		};
	};

	if ( FAILED( hCreateFactoryResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateFactoryResult,
			L"CreateDXGIFactory2 failed."
		);

		hOverallResult = hCreateFactoryResult;
		goto Cleanup;
	};


	//
	// Enumerate adapter 0.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CSwapChainsTest::GetDesktopCoordinatesForDefaultOutput:  Enumerating adapter 0."
	);

	HRESULT hEnumerateAdapter0Result = pFactory->EnumAdapters1
	(
		0,
		&pAdapter
	);

	if ( FAILED( hEnumerateAdapter0Result ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hEnumerateAdapter0Result,
			L"EnumAdapters1 failed."
		);

		hOverallResult = hEnumerateAdapter0Result;
		goto Cleanup;
	};


	//
	// Enumerate output 0.
	//

	HRESULT hEnumerateOutput0Result = pAdapter->EnumOutputs
	(
		0,
		&pOutput
	);

	if ( FAILED( hEnumerateOutput0Result ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hEnumerateOutput0Result,
			L"EnumOutputs failed."
		);

		hOverallResult = hEnumerateOutput0Result;
		goto Cleanup;
	};


	//
	// Query output 0 for its IDXGIOutput1 interface.
	//

	HRESULT hQueryForIDXGIOutput1Result = pOutput->QueryInterface
	(
		IID_IDXGIOutput1,
		(void **) &pOutput_IDXGIOutput1
	);

	pOutput->Release();
	pOutput = nullptr;

	if ( FAILED( hQueryForIDXGIOutput1Result ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hQueryForIDXGIOutput1Result,
			L"QueryInterface for IDXGIOutput1 on output 0 failed."
		);

		hOverallResult = hQueryForIDXGIOutput1Result;
		goto Cleanup;
	};


	//
	// Determine the current mode for the default output.
	//

	DXGI_OUTPUT_DESC DefaultOutputDescriptor;
	ZeroMemory( &DefaultOutputDescriptor, sizeof( DefaultOutputDescriptor ) );
	HRESULT hGetDefaultOutputDescriptorResult = pOutput_IDXGIOutput1->GetDesc( &DefaultOutputDescriptor );
	if ( FAILED( hGetDefaultOutputDescriptorResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hGetDefaultOutputDescriptorResult,
			L"GetDesc on default output failed."
		);

		hOverallResult = hGetDefaultOutputDescriptorResult;
		goto Cleanup;
	};


	//
	// Return the desktop coordinates for the default output.
	//

	*pDefaultOutputLeft = DefaultOutputDescriptor.DesktopCoordinates.left;
	*pDefaultOutputTop = DefaultOutputDescriptor.DesktopCoordinates.top;
	*pDefaultOutputRight = DefaultOutputDescriptor.DesktopCoordinates.right;
	*pDefaultOutputBottom = DefaultOutputDescriptor.DesktopCoordinates.bottom;


	hOverallResult = S_OK;


Cleanup:
	SAFE_RELEASE( pOutput_IDXGIOutput1 );

#pragma warning( suppress:  28922 )
	SAFE_RELEASE( pOutput );

	SAFE_RELEASE( pAdapter );
	SAFE_RELEASE( pFactory );

	return hOverallResult;
};


#if USE_DWMSLICER
///////////////////////////////////////////////////////
// Function: GetTexel
//
// Retrieves a Texel value from a mapped sub resource
// and returns a FLOAT_COLOR.
//
// Called by ValidateTexel.
///////////////////////////////////////////////////////
bool CSwapChainsTest::GetTexel
(
	UINT X,
	UINT Y,
	DXGI_FORMAT Format,
	_In_ const D3D11_MAPPED_SUBRESOURCE & MappedSubresource,	
	_Out_ FLOAT_COLOR * pFloatColor
)
{
	//
	// Validate and initialize parameters.
	//

	assert ( pFloatColor != nullptr );
	if ( pFloatColor == nullptr )
	{
		return false;
	};

	ZeroMemory( pFloatColor, sizeof( FLOAT_COLOR ) );


	//
	// Initialize locals.
	//

	const UINT FrameElementSize = CD3D10FormatHelper_Test::GetBitsPerElement( Format ) / 8;
	BYTE * pFrameData = reinterpret_cast< BYTE * >( MappedSubresource.pData );


	// Get the current texel data.
	BYTE * pCurrentTexel = pFrameData + 
		( Y * MappedSubresource.RowPitch ) + 
		( X * FrameElementSize );



	//
	// Convert the current texel to FLOAT_COLOR.
	//

	if
	(
		!CFormatHelper::ConvertPixels
		(
			CONV_FROM_FORMAT,
			Format,
			pFloatColor,
			1,
			1,
			1,
			pCurrentTexel
		)
	)
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			WEX::Common::String().Format
			(
				L"ConvertPixels failed for texel (%u, %u). Row pitch = %u.",
				X,
				Y,
				MappedSubresource.RowPitch
			)
		);

		return false;
	};

	return true;
};


bool CSwapChainsTest::GetTexel
(
	UINT iCurrentCapturedFrame,
	UINT FrameWidth,
	UINT FrameHeight,
	UINT X,
	UINT Y,
	_Out_ FLOAT_COLOR * pFloatColor
)
{
	//
	// Initialize locals.
	//

	bool bOverallResult = false;

	SmartPtr< ImagePipeline::Texture > CurrentCapturedFrame;

	ATL::CComPtr< ID3D11Device > pSlicerDevice = nullptr;
	ATL::CComPtr< ID3D11DeviceContext > pSlicerImmediateContext = nullptr;

	UINT FrameElementSize = 0;


	//
	// Validate and initialize parameters.
	//

	if ( pFloatColor == nullptr )
	{
		goto Cleanup;
	};

	ZeroMemory( pFloatColor, sizeof( FLOAT_COLOR ) );


	//
	// Get the specified frame out of the atlas.
	//

	// Create a frame to hold it.
	CurrentCapturedFrame = m_CapturedFrameAtlas->CreateCompatibleFrame
	(
		FrameWidth,
		FrameHeight,
		true
	);

	// Copy the frame out of the atlas.
	if
	(
		!m_CapturedFrameAtlas->CopySubtexture
		(
			CurrentCapturedFrame,
			iCurrentCapturedFrame
		)
	)
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			WEX::Common::String().Format
			(
				L"CopySubtexture failed for frame %u of %u.",
				iCurrentCapturedFrame,
				m_cCapturedFrames
			)
		);

		goto Cleanup;
	};


	if ( nullptr == CurrentCapturedFrame->GetResource() )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"CopySubtexture succeeded but returned a null resource pointer."
		);

		goto Cleanup;
	};


	// Get the frame's device and device context.
	CurrentCapturedFrame->GetTexture()->GetDevice( &pSlicerDevice );

	pSlicerDevice->GetImmediateContext( &pSlicerImmediateContext );

	// Get the frame's texture descriptor.
	D3D11_TEXTURE2D_DESC CurrentCapturedFrameTextureDescriptor;
	CurrentCapturedFrame->GetTexture()->GetDesc( &CurrentCapturedFrameTextureDescriptor );

	FrameElementSize = CD3D10FormatHelper_Test::GetBitsPerElement( CurrentCapturedFrameTextureDescriptor.Format ) / 8;
	assert( FrameWidth == CurrentCapturedFrameTextureDescriptor.Width );
	assert( FrameHeight == CurrentCapturedFrameTextureDescriptor.Height );

	// Map the frame texture.
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	ZeroMemory( &MappedSubresource, sizeof( MappedSubresource ) );
	HRESULT hMapResult = pSlicerImmediateContext->Map
	(
		CurrentCapturedFrame->GetTexture(),
		0,
		D3D11_MAP_READ,
		0,
		&MappedSubresource
	);

	if ( FAILED( hMapResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hMapResult,
			L"Map on captured frame texture failed."
		);

		goto Cleanup;
	};

	// Hand off to the other overload of this method.
	bOverallResult = GetTexel
	(
		X,
		Y,
		CurrentCapturedFrameTextureDescriptor.Format,
		MappedSubresource,
		pFloatColor
	);

	// Unmap the frame.
	pSlicerImmediateContext->Unmap
	(
		CurrentCapturedFrame->GetTexture(),
		0
	);


Cleanup:
	return bOverallResult;
};


///////////////////////////////////////////////////////
// Function:  IsAtlasNonEmpty
//
// Purpose:
// Returns S_FALSE if every pixel in the captured atlas
// is transparent, S_OK if there are any opaque pixels,
// and a failed HRESULT in the event of failure.
///////////////////////////////////////////////////////
HRESULT CSwapChainsTest::IsAtlasNonEmpty
(
	UINT FrameWidth,
	UINT FrameHeight
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = S_FALSE;

	UINT FrameElementSize = 0;


	//
	// Go through each captured frame...
	//

	for
	(
		UINT iCurrentCapturedFrame = 0;
		iCurrentCapturedFrame < m_cCapturedFrames;
		++iCurrentCapturedFrame
	)
	{
		// Get the current frame out of the atlas.
		SmartPtr< ImagePipeline::Texture > CurrentCapturedFrame = m_CapturedFrameAtlas->CreateCompatibleFrame
		(
			FrameWidth,
			FrameHeight,
			true
		);

		if
		(
			!m_CapturedFrameAtlas->CopySubtexture
			(
				CurrentCapturedFrame,
				iCurrentCapturedFrame
			)
		)
		{
			LOG_ERROR
			(
				ERROR_TYPE_BOOL,
				false,
				WEX::Common::String().Format
				(
					L"CopySubtexture failed for frame %u of %u.",
					iCurrentCapturedFrame,
					m_cCapturedFrames
				)
			);

			goto Cleanup;
		};


		if ( nullptr == CurrentCapturedFrame->GetResource() )
		{
			LOG_ERROR
			(
				ERROR_TYPE_BOOL,
				false,
				L"CopySubtexture succeeded but returned a null resource pointer."
			);

			goto Cleanup;
		};


		// Get the frame's device and device context.
		ATL::CComPtr< ID3D11Device > pSlicerDevice = nullptr;
		CurrentCapturedFrame->GetTexture()->GetDevice( &pSlicerDevice );

		ATL::CComPtr< ID3D11DeviceContext > pSlicerImmediateContext = nullptr;
		pSlicerDevice->GetImmediateContext( &pSlicerImmediateContext );

		// Get the frame's texture descriptor.
		D3D11_TEXTURE2D_DESC CurrentCapturedFrameTextureDescriptor;
		CurrentCapturedFrame->GetTexture()->GetDesc( &CurrentCapturedFrameTextureDescriptor );

		FrameElementSize = CD3D10FormatHelper_Test::GetBitsPerElement( CurrentCapturedFrameTextureDescriptor.Format ) / 8;
		assert( FrameWidth == CurrentCapturedFrameTextureDescriptor.Width );
		assert( FrameHeight == CurrentCapturedFrameTextureDescriptor.Height );

		// Map the current frame.
		D3D11_MAPPED_SUBRESOURCE MappedSubresource;
		ZeroMemory( &MappedSubresource, sizeof( MappedSubresource ) );
		HRESULT hMapResult = pSlicerImmediateContext->Map
		(
			CurrentCapturedFrame->GetTexture(),
			0,
			D3D11_MAP_READ,
			0,
			&MappedSubresource
		);

		if ( FAILED( hMapResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hMapResult,
				L"Map on captured frame texture failed."
			);

			goto Cleanup;
		};

		// Scan the current frame.
		HRESULT hIsFrameNonEmptyResult = IsFrameNonEmpty
		(
			iCurrentCapturedFrame,
			FrameWidth,
			FrameHeight,
			CurrentCapturedFrameTextureDescriptor.Format,
			MappedSubresource
		);

		// Unmap the current frame.
		pSlicerImmediateContext->Unmap
		(
			CurrentCapturedFrame->GetTexture(),
			0
		);

		if ( hIsFrameNonEmptyResult != S_FALSE )
		{
			hOverallResult = hIsFrameNonEmptyResult;
			goto Cleanup;
		};
	};

	
	// If we got here, the atlas was all transparency.
	hOverallResult = S_OK;

Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  IsFrameNonEmpty
//
// Purpose:
// Returns S_OK if the entire frame is transparency,
// S_FALSE if there are any opaque pixels, and a failed
// HRESULT in the event of an error.
///////////////////////////////////////////////////////
HRESULT CSwapChainsTest::IsFrameNonEmpty
(
	UINT iCurrentFrame,
	UINT FrameWidth,
	UINT FrameHeight,
	DXGI_FORMAT FrameFormat,
	const D3D11_MAPPED_SUBRESOURCE & MappedSubresource
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;
	const UINT FrameElementSize = CD3D10FormatHelper_Test::GetBitsPerElement( FrameFormat ) / 8;
	BYTE * pFrameData = reinterpret_cast< BYTE * >( MappedSubresource.pData );


	//
	// Scan each pixel in the frame.
	//

	for
	(
		UINT CurrentRow = 0;
		CurrentRow < FrameHeight;
		++CurrentRow
	)
	{
		for
		(
			UINT CurrentColumn = 0;
			CurrentColumn < FrameWidth;
			++CurrentColumn
		)
		{
			// Get the current texel data.
			BYTE * pCurrentTexel = pFrameData + 
				( CurrentRow * MappedSubresource.RowPitch ) + 
				( CurrentColumn * FrameElementSize );

			// Convert the current texel to FLOAT_COLOR.
			FLOAT_COLOR CurrentTexelFloatColor;
			bool bConvertPixelsResult = CFormatHelper::ConvertPixels
			(
				CONV_FROM_FORMAT,
				FrameFormat,
				&CurrentTexelFloatColor,
				1,
				1,
				1,
				pCurrentTexel
			);

			if ( bConvertPixelsResult != true )
			{
				LOG_ERROR
				(
					ERROR_TYPE_BOOL,
					false,
					WEX::Common::String().Format
					(
						L"ConvertPixels failed for texel (%u, %u).  Row pitch = %u.",
						CurrentColumn,
						CurrentRow,
						MappedSubresource.RowPitch
					)
				);

				goto Cleanup;
			};

			if
			(
				CurrentTexelFloatColor.r != 0.f ||
				CurrentTexelFloatColor.g != 0.f ||
				CurrentTexelFloatColor.b != 0.f ||
				CurrentTexelFloatColor.a != 0.f
			)
			{
				hOverallResult = S_OK;
				goto Cleanup;
			};
		};
	};


	// If we got here, the frame is all transparency.
	hOverallResult = S_FALSE;

Cleanup:
	return hOverallResult;
};
#endif


///////////////////////////////////////////////////////
// Function:  LogTestCaseParameters
//
// Purpose:
// Logs parameters for this test variation.
///////////////////////////////////////////////////////
void CSwapChainsTest::LogTestCaseParameters()
{
};


///////////////////////////////////////////////////////
// Function:  SetExpectedResults
//
// Purpose:
// Builds the expected-results data structure.
///////////////////////////////////////////////////////
void CSwapChainsTest::SetExpectedResults()
{
	for
	(
		TEST_API CurrentAPI = TEST_API_FIRST;
		CurrentAPI < TEST_API_COUNT;
		CurrentAPI = (TEST_API) ( CurrentAPI + 1 )
	)
	{
		for
		(
			SWAPCHAIN_TYPE CurrentSwapChainType = SWAPCHAIN_TYPE_FIRST;
			CurrentSwapChainType < SWAPCHAIN_TYPE_COUNT;
			CurrentSwapChainType = (SWAPCHAIN_TYPE) ( CurrentSwapChainType + 1 )
		)
		{
#if !defined(CORE_WINDOWS_CONSISTENTLY_SPAN_MONITOR)
			//
			// Add no expected results for variations we can't currently verify.
			//

			if ( CurrentSwapChainType == SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW )
			{
				continue;
			};
#endif


#if !defined(TEST_DECODE_SWAP_CHAINS_UNDER_D3D12)
			//
			// Add no expected results for variations we can't currently exercise.
			//

			if
			(
				CurrentAPI == TEST_API_D3D12 &&
				CurrentSwapChainType == SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE
			)
			{
				continue;
			};
#endif


			//
			// Expected results for the shaded-cube rendering technique.
			//

			// Test a point on the left-front face of the cube.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_CUBE_SHADED ].push_back
			(
				ExpectedTexelColor
				(
					.5f,
					.5f,
					.670588f,
					.670588f,
					.670588f,
					1.f
				)
			);

			// Test a point to the left of and above the cube.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_CUBE_SHADED ].push_back
			(
				ExpectedTexelColor
				(
					.1f,
					.1f,
					0.f,
					0.f,
					0.f,
					1.f
				)
			);

			// Test a point to the right of and below the cube.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_CUBE_SHADED ].push_back
			(
				ExpectedTexelColor
				(
					1.f,
					1.f,
					0.f,
					0.f,
					0.f,
					1.f
				)
			);

			// Test a point on the front-right face of the cube.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_CUBE_SHADED ].push_back
			(
				ExpectedTexelColor
				(
					.5484375f,
					.5f,
					.42353f,
					.42353f,
					.42353f,
					1.f
				)
			);

			// Test a point on the top face of the cube.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_CUBE_SHADED ].push_back
			(
				ExpectedTexelColor
				(
					.5f,
					.41667f,
					0.75294f,
					0.75294f,
					0.75294f,
					1.f
				)
			);


			//
			// Expected results for the textured-plane rendering technique.
			//

			// Test a point sampled from the upper-left pixel of the texture.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_PLANE_TEXTURED ].push_back
			(
				ExpectedTexelColor
				(
					.05f,
					.05f,
					0.f,
					1.f,
					1.f,
					1.f
				)
			);

			// Test a point sampled from the upper-right pixel of the texture.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_PLANE_TEXTURED ].push_back
			(
				ExpectedTexelColor
				(
					.95f,
					.05f,
					0.f,
					1.f,
					0.11765f,
					1.f
				)
			);

			// Test a point sampled from the lower-left pixel of the texture.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_PLANE_TEXTURED ].push_back
			(
				ExpectedTexelColor
				(
					.05f,
					.95f,
					0.f,
					0.11765f,
					1.f,
					1.f
				)
			);

			// Test a point sampled from the lower-right pixel of the texture.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_PLANE_TEXTURED ].push_back
			(
				ExpectedTexelColor
				(
					.95f,
					.95f,
					0.f,
					0.11765f,
					0.11765f,
					1.f
				)
			);


			//
			// Expected results for the fixed-color-swatch rendering technique
			//

			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_SWATCH_FIXEDCOLOR ].push_back
			(
				ExpectedTexelColor
				(
					0.f,
					0.f,
					0.f,
					.5f,
					1.f,
					1.f
				)
			);

			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_SWATCH_FIXEDCOLOR ].push_back
			(
				ExpectedTexelColor
				(
					.1f,
					.1f,
					0.f,
					.5f,
					1.f,
					1.f
				)
			);

			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_SWATCH_FIXEDCOLOR ].push_back
			(
				ExpectedTexelColor
				(
					.5f,
					.5f,
					0.f,
					.5f,
					1.f,
					1.f
				)
			);

			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_SWATCH_FIXEDCOLOR ].push_back
			(
				ExpectedTexelColor
				(
					0.f,
					1.f,
					0.f,
					.5f,
					1.f,
					1.f
				)
			);


			//
			// @TODO
			//
			// We can't very well generate expected results a priori
			// for the random-color-swatch expected results.  They
			// could be generated at the point the random color is
			// generated.
			//


			//
			// @TODO
			// Expected results for the transparent-swatch rendering technique
			//


			//
			// Expected results for the color-swatch-pattern rendering technique
			//

			// Verify the white swatch from the background swap chain.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_PATTERN ].push_back
			(
				ExpectedTexelColor
				(
					.05f,
					.05f,
					1.f,
					1.f,
					1.f,
					1.f
				)
			);

			// Verify the light gray swatch from the background swap chain.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_PATTERN ].push_back
			(
				ExpectedTexelColor
				(
					.15f,
					.05f,
					.75f,
					.75f,
					.75f,
					1.f
				)
			);

			// Verify the medium gray swatch from the background swap chain.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_PATTERN ].push_back
			(
				ExpectedTexelColor
				(
					.25f,
					.05f,
					.5f,
					.5f,
					.5f,
					1.f
				)
			);

			// Verify the dark gray swatch from the background swap chain.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_PATTERN ].push_back
			(
				ExpectedTexelColor
				(
					.35f,
					.05f,
					.25f,
					.25f,
					.25f,
					1.f
				)
			);

			// Verify the red gray swatch from the background swap chain.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_PATTERN ].push_back
			(
				ExpectedTexelColor
				(
					.05f,
					.25f,
					.5f,
					0.f,
					0.f,
					1.f
				)
			);

			// Verify the green swatch from the background swap chain.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_PATTERN ].push_back
			(
				ExpectedTexelColor
				(
					.15f,
					.25f,
					0.f,
					.5f,
					0.f,
					1.f
				)
			);

			// Verify the blue swatch from the background swap chain.
			m_ExpectedResults[ CurrentAPI ][ CurrentSwapChainType ][ SCENE_TYPE_PATTERN ].push_back
			(
				ExpectedTexelColor
				(
					.25f,
					.25f,
					0.f,
					0.f,
					.5f,
					1.f
				)
			);
		};
	};
};


///////////////////////////////////////////////////////
// Function:  TestCreateDecodeSwapChainForCompositionSurfaceHandle_WoW64
//
// Purpose:
// This method is just a wrapper that gives us a place
// to hang metadata for creating the WoW64 version of
// the job.
///////////////////////////////////////////////////////
void CSwapChainsTest::TestCreateDecodeSwapChainForCompositionSurfaceHandle_WoW64()
{
	TestSwapChainRendering
	(
		true,
		SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE
	);
};


///////////////////////////////////////////////////////
// Function:  TestCreateDecodeSwapChainForCompositionSurfaceHandle_Native
//
// Purpose:
// This method is just a wrapper that gives us a place
// to hang metadata for creating the native version of
// the job.
///////////////////////////////////////////////////////
void CSwapChainsTest::TestCreateDecodeSwapChainForCompositionSurfaceHandle_Native()
{
	TestSwapChainRendering
	(
		false,
		SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE
	);
};


///////////////////////////////////////////////////////
// Function:  TestCreateSwapChain_WoW64
//
// Purpose:
// This method is just a wrapper that gives us a place
// to hang metadata for creating the WoW64 version of
// the job.
///////////////////////////////////////////////////////
void CSwapChainsTest::TestCreateSwapChain_WoW64()
{
	TestSwapChainRendering
	(
		true,
		SWAPCHAIN_TYPE_CREATESWAPCHAIN
	);
};


///////////////////////////////////////////////////////
// Function:  TestCreateSwapChain_Native
//
// Purpose:
// This method is just a wrapper that gives us a place
// to hang metadata for creating the native version of
// the job.
///////////////////////////////////////////////////////
void CSwapChainsTest::TestCreateSwapChain_Native()
{
	TestSwapChainRendering
	(
		false,
		SWAPCHAIN_TYPE_CREATESWAPCHAIN
	);
};


///////////////////////////////////////////////////////
// Function:  TestCreateSwapChainForCompositionSurfaceHandle_WoW64
//
// Purpose:
// This method is just a wrapper that gives us a place
// to hang metadata for creating the WoW64 version of
// the job.
///////////////////////////////////////////////////////
void CSwapChainsTest::TestCreateSwapChainForCompositionSurfaceHandle_WoW64()
{
	TestSwapChainRendering
	(
		true,
		SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOMPOSITIONSURFACEHANDLE
	);
};


///////////////////////////////////////////////////////
// Function:  TestCreateSwapChainForCompositionSurfaceHandle_Native
//
// Purpose:
// This method is just a wrapper that gives us a place
// to hang metadata for creating the native version of
// the job.
///////////////////////////////////////////////////////
void CSwapChainsTest::TestCreateSwapChainForCompositionSurfaceHandle_Native()
{
	TestSwapChainRendering
	(
		false,
		SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOMPOSITIONSURFACEHANDLE
	);
};


///////////////////////////////////////////////////////
// Function:  TestCreateSwapChainForHwnd_WoW64
//
// Purpose:
// This method is just a wrapper that gives us a place
// to hang metadata for creating the WoW64 version of
// the job.
///////////////////////////////////////////////////////
void CSwapChainsTest::TestCreateSwapChainForHwnd_WoW64()
{
	TestSwapChainRendering
	(
		true,
		SWAPCHAIN_TYPE_CREATESWAPCHAINFORHWND
	);
};


///////////////////////////////////////////////////////
// Function:  TestCreateSwapChainForHwnd_Native
//
// Purpose:
// This method is just a wrapper that gives us a place
// to hang metadata for creating the native version of
// the job.
///////////////////////////////////////////////////////
void CSwapChainsTest::TestCreateSwapChainForHwnd_Native()
{
	TestSwapChainRendering
	(
		false,
		SWAPCHAIN_TYPE_CREATESWAPCHAINFORHWND
	);
};


///////////////////////////////////////////////////////
// Function:  TestCreateSwapChainForCoreWindow_Overlapped_WoW64
//
// Purpose:
// This method is just a wrapper that gives us a place
// to hang metadata for creating the WoW64 version of
// the job.
///////////////////////////////////////////////////////
void CSwapChainsTest::TestCreateSwapChainForCoreWindow_Overlapped_WoW64()
{
	TestSwapChainRendering
	(
		true,
		SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW
	);
};


///////////////////////////////////////////////////////
// Function:  TestCreateSwapChainForCoreWindow_Overlapped_Native
//
// Purpose:
// This method is just a wrapper that gives us a place
// to hang metadata for creating the native version of
// the job.
///////////////////////////////////////////////////////
void CSwapChainsTest::TestCreateSwapChainForCoreWindow_Overlapped_Native()
{
	TestSwapChainRendering
	(
		false,
		SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW
	);
};


///////////////////////////////////////////////////////
// Function:  TestSwapChainRendering
//
// Purpose:
// Tests whether output rendered and presented with
// various types of swap chains is correct.
///////////////////////////////////////////////////////
void CSwapChainsTest::TestSwapChainRendering
(
	bool bRunUnderWoW64,
	SWAPCHAIN_TYPE SwapChainType
)
{
	//
	// Initialize locals.
	//

	bool bOverallResult = false;
	bool bVerificationsPerformed = false;

	LONG DefaultOutputLeft = 0;
	LONG DefaultOutputTop = 0;
	LONG DefaultOutputRight = 1920;
	LONG DefaultOutputBottom = 1080;

	IDXGISwapChain * pBackgroundSwapChain = nullptr;
	IDXGISwapChain * pForegroundSwapChain = nullptr;

	WEX::Common::String RenderingAPIName;
	TEST_API RenderingAPI = TEST_API_D3D12;

	WEX::Common::String SceneTypeName;
	SCENE_TYPE SceneType = SCENE_TYPE_PATTERN;

#if USE_DWMSLICER
	WEX::Common::String CurrentAtlasFilename;
#endif

#if USE_SCREENCAPTURE
	WEX::Common::String CurrentCaptureFilename;
#endif


	//
	// Validate members and state.
	//

	if ( m_pLogger == nullptr )
	{
		goto Cleanup;
	};


	//
	// Begin a test group in the log.
	//

	m_pLogger->BeginTestGroup
	(
		bRunUnderWoW64 == true ? L"Swap Chain Rendering (WoW64)" : L"Swap Chain Rendering (Native)",
		3,
		TESTCASE_RESULT_SKIPPED
	);


	//
	// Begin a test case in the log.
	//

	m_pLogger->BeginTestCase
	(
		L"TestSwapChainRendering",
		0xFFFFFFFF,
		nullptr
	);


	//
	// Parse the rendering API.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Parsing rendering API."
	);

	HRESULT hParseRenderingAPINameResult = WEX::TestExecution::TestData::TryGetValue( L"RenderingAPI", RenderingAPIName );
	if ( FAILED( hParseRenderingAPINameResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hParseRenderingAPINameResult,
			L"TryGetValue on RenderingAPI failed."
		);

		goto Cleanup;
	};

	bool bConvertRenderingAPIResult = FromString
	(
		&RenderingAPI,
		std::wstring( RenderingAPIName )
	);

	if ( bConvertRenderingAPIResult != true )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			bConvertRenderingAPIResult,
			L"FromString on rendering API name failed."
		);

		goto Cleanup;
	};


	//
	// Parse the scene type.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Parsing scene type."
	);

	HRESULT hParseSceneTypeNameResult = WEX::TestExecution::TestData::TryGetValue( L"SceneType", SceneTypeName );
	if ( FAILED( hParseSceneTypeNameResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hParseSceneTypeNameResult,
			L"TryGetValue on SceneType failed."
		);

		goto Cleanup;
	};

	bool bConvertSceneTypeResult = FromString
	(
		&SceneType,
		std::wstring( SceneTypeName )
	);

	if ( bConvertSceneTypeResult != true )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			bConvertSceneTypeResult,
			L"FromString on scene type name failed."
		);

		goto Cleanup;
	};


	//
	// Log test case parameters.
	//

	LogTestCaseParameters();


	//
	// Determine whether we're running under WoW64.
	//

	BOOL bIsWoW64Process = FALSE;
	BOOL bCheckForWoWResult = IsWow64Process
	(
		GetCurrentProcess(),
		&bIsWoW64Process
	);

	if ( bIsWoW64Process == TRUE && bRunUnderWoW64 != true )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"This variation does not run under WoW64."
		);

		bOverallResult = true;
		goto Cleanup;
	}
	else if ( bIsWoW64Process != TRUE && bRunUnderWoW64 == true )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"This variation runs only under WoW64."
		);

		bOverallResult = true;
		goto Cleanup;
	};


#if 0
	//
	// Skip this test if the SetupTestCase method skipped.
	//

	if ( m_pFactory == nullptr )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"SetupTestCase skipped, but yet TAEF is running our test method!"
		);

		bOverallResult = true;
		goto Cleanup;
	};
#endif


	//
	// Determine the desktop coordinates for the default
	// DXGI output.
	//

	HRESULT hGetDesktopCoordinatesResult = GetDesktopCoordinatesForDefaultOutput
	(
		&DefaultOutputLeft,
		&DefaultOutputTop,
		&DefaultOutputRight,
		&DefaultOutputBottom
	);

	if ( FAILED( hGetDesktopCoordinatesResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hGetDesktopCoordinatesResult,
			L"GetDesktopCoordinatesForDefaultOutput failed."
		);

		goto Cleanup;
	};

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"Desktop coordinates for default output:  (%li, %li) to (%li, %li).",
			DefaultOutputLeft,
			DefaultOutputTop,
			DefaultOutputRight,
			DefaultOutputBottom
		)
	);


	//
	// Allocate scene renderers and connect them to the logger.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Creating scene renderers."
	);


	assert( m_ppSceneRenderers[ TEST_API_D3D11 ] == nullptr );
	m_ppSceneRenderers[ TEST_API_D3D11 ] = new( std::nothrow ) CD3D11SceneRenderer;
	if ( m_ppSceneRenderers[ TEST_API_D3D11 ] == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Failed to allocate Direct3D 11 scene renderer."
		);

		goto Cleanup;
	};

	( (CD3D11SceneRenderer *) m_ppSceneRenderers[ TEST_API_D3D11 ] )->SetLogger( m_pLogger );


	assert( m_ppSceneRenderers[ TEST_API_D3D12 ] == nullptr );
	m_ppSceneRenderers[ TEST_API_D3D12 ] = new( std::nothrow ) CD3D12SceneRenderer;
	if ( m_ppSceneRenderers[ TEST_API_D3D12 ] == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Failed to allocate Direct3D 12 scene renderer."
		);

		goto Cleanup;
	};

	( (CD3D12SceneRenderer *) m_ppSceneRenderers[ TEST_API_D3D12 ] )->SetLogger( m_pLogger );


	//
	// Create and initialize DirectComposition objects, a swap
	// chain window, and a scene renderer.
	//

	switch ( SwapChainType )
	{
		case SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
		case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
		{
			//
			// Create a system window wrapper object.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"SCreating CSystemWindowWrapper object."
			);

			m_pSystemWindowWrapper = new (std::nothrow) CSystemWindowWrapper
			(
				DefaultOutputLeft,
				DefaultOutputTop,
				DefaultOutputRight,
				DefaultOutputBottom
			);

			if ( m_pSystemWindowWrapper == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					E_OUTOFMEMORY,
					L"Failed to allocate new CSystemWindowWrapper object."
				);

				goto Cleanup;
			};


			//
			// Assign the system window wrapper a logger.
			//

			m_pSystemWindowWrapper->SetLogger( m_pLogger );


			//
			// Have the wrapper create a window.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Calling StartSystemWindowThread on CSystemWindowWrapper object."
			);

			HRESULT hStartResult = m_pSystemWindowWrapper->StartSystemWindowThread();

			if ( FAILED( hStartResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hStartResult,
					L"StartSystemWindowThread on CSystemWindowWrapper object failed."
				);

				goto Cleanup;
			};


			//
			// Create a composition surface.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Creating DirectComposition surface."
			);

			HRESULT hCreateCompositionSurfaceResult = m_DCompIsolation.CreateSurfaceHandle
			(
				COMPOSITIONOBJECT_ALL_ACCESS,
				nullptr,
				&m_hCompositionSurface
			);

			if ( FAILED( hCreateCompositionSurfaceResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hCreateCompositionSurfaceResult,
					L"DCompositionCreateSurfaceHandle failed."
				);

				goto Cleanup;
			};

			if ( m_hCompositionSurface == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					E_POINTER,
					L"DCompositionCreateSurfaceHandle succeeded but returned NULL surface handle."
				);

				goto Cleanup;
			};

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Composition surface was created successfully."
			);


			//
			// Initialize the scene renderer.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Calling Initialize on scene renderer."
			);

#pragma warning( suppress:  26001 )
			assert( m_ppSceneRenderers[ RenderingAPI ] != nullptr );

#pragma warning( suppress:  26001 )
			HRESULT hInitializeSceneRendererResult = m_ppSceneRenderers[ RenderingAPI ]->Initialize
			(
				SceneType,
				SwapChainType,
				m_hCompositionSurface,
				0,
				0,
				false,
				2,
				DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
				0
			);

			if ( hInitializeSceneRendererResult == S_FALSE )
			{
				bOverallResult = true;
				goto Cleanup;
			}

			if ( FAILED( hInitializeSceneRendererResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hInitializeSceneRendererResult,
					L"Initialize on scene renderer failed."
				);

				goto Cleanup;
			};


			//
			// Associate the scene renderer with the window wrapper.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Calling SetSceneRenderer on system window wrapper object."
			);

#pragma warning( suppress:  26001 )
			assert( m_ppSceneRenderers[ RenderingAPI ] != nullptr );

#pragma warning( suppress:  26001 )
			m_pSystemWindowWrapper->SetSceneRenderer( m_ppSceneRenderers[ RenderingAPI ] );


#if defined(USE_ADVANCED_DCOMPISOLATION_MODEL)
			//
			// Create a DirectComposition device wrapper if needed.
			//

			if ( m_pDCompDeviceWrapper == nullptr )
			{
				HRESULT hCreateDCompDeviceResult = m_DCompIsolation.CreateDevice
				(
					&m_pDCompDeviceWrapper
				);

				if ( FAILED( hCreateDCompDeviceResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hCreateDCompDeviceResult,
						L"CreateDevice on DirectComposition wrapper object failed."
					);

					goto Cleanup;
				};
			};


			//
			// Create a DirectComposition target wrapper.
			//

			assert( m_pDCompTargetWrapper == nullptr );
			assert( m_pSystemWindowWrapper != nullptr );
			assert( m_pSystemWindowWrapper->GetSystemWindow() != nullptr );
			HRESULT hCreateTargetResult = m_pDCompDeviceWrapper->CreateTargetForWindow
			(
				m_pSystemWindowWrapper->GetSystemWindow(),
				TRUE,
				&m_pDCompTargetWrapper
			);

			if ( FAILED( hCreateTargetResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hCreateTargetResult,
					L"CreateTargetForWindow failed."
				);

				goto Cleanup;
			};


			//
			// Set the swap chain as the target's contents.
			//

#pragma warning( suppress:  26001 )
			HRESULT hSetSourceSwapChainResult = m_pDCompTargetWrapper->SetSourceSwapChain
			(
				m_ppSceneRenderers[ RenderingAPI ]->GetSwapChain( SWAPCHAIN_SLOT_DEFAULT ),
				true
			);

			if ( FAILED( hSetSourceSwapChainResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hSetSourceSwapChainResult,
					L"SetSourceSwapChain failed."
				);

				goto Cleanup;
			};
#else
			//
			// Have the composition surface target the window.
			//

			HRESULT hSetTargetWindowResult = m_DCompIsolation.SetTargetWindow
			(
				m_pSystemWindowWrapper->GetSystemWindow()
			);

			if ( FAILED( hSetTargetWindowResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hSetTargetWindowResult,
					L"SetTargetWindow failed."
				);

				goto Cleanup;
			};


			//
			// Set the swap chain as the composition surface contents.
			//

#pragma warning( suppress:  26001 )
			HRESULT hSetSourceSwapChainResult = m_DCompIsolation.SetSourceSwapChain
			(
				m_ppSceneRenderers[ RenderingAPI ]->GetSwapChain( SWAPCHAIN_SLOT_DEFAULT ),
				true
			);

			if ( FAILED( hSetSourceSwapChainResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hSetSourceSwapChainResult,
					L"SetSourceSwapChain failed."
				);

				goto Cleanup;
			};
#endif
		} break;


		case SWAPCHAIN_TYPE_CREATESWAPCHAIN:
		case SWAPCHAIN_TYPE_CREATESWAPCHAINFORHWND:
		{
			//
			// Create a system window wrapper object.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Creating CSystemWindowWrapper object."
			);

			m_pSystemWindowWrapper = new (std::nothrow) CSystemWindowWrapper
			(
				DefaultOutputLeft,
				DefaultOutputTop,
				DefaultOutputRight,
				DefaultOutputBottom
			);

			if ( m_pSystemWindowWrapper == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					E_OUTOFMEMORY,
					L"Failed to allocate new CSystemWindowWrapper object."
				);

				goto Cleanup;
			};


			//
			// Assign the system window wrapper a logger.
			//

			m_pSystemWindowWrapper->SetLogger( m_pLogger );


			//
			// Have the wrapper create a window.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Calling StartSystemWindowThread on CSystemWindowWrapper object."
			);

			HRESULT hStartResult = m_pSystemWindowWrapper->StartSystemWindowThread();

			if ( FAILED( hStartResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hStartResult,
					L"StartSystemWindowThread on CSystemWindowWrapper object failed."
				);

				goto Cleanup;
			};


			//
			// Initialize the scene renderer.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Calling Initialize on scene renderer."
			);

#pragma warning( suppress:  26001 )
			assert( m_ppSceneRenderers[ RenderingAPI ] != nullptr );

#pragma warning( suppress:  26001 )
			HRESULT hInitializeSceneRendererResult = m_ppSceneRenderers[ RenderingAPI ]->Initialize
			(
				SceneType,
				SwapChainType,
				m_pSystemWindowWrapper->GetSystemWindow(),
				false,
				2,
				DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
				0
			);

			if ( hInitializeSceneRendererResult == S_FALSE )
			{
				bOverallResult = true;
				goto Cleanup;
			};

			if ( FAILED( hInitializeSceneRendererResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hInitializeSceneRendererResult,
					L"Initialize on scene renderer failed."
				);

				goto Cleanup;
			};


			//
			// Associate the scene renderer with the window wrapper.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Calling SetSceneRenderer on system window wrapper object."
			);

#pragma warning( suppress:  26001 )
			assert( m_ppSceneRenderers[ RenderingAPI ] != nullptr );

#pragma warning( suppress:  26001 )
			m_pSystemWindowWrapper->SetSceneRenderer( m_ppSceneRenderers[ RenderingAPI ] );
		} break;


		case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW:
		{
			//
			// Create an immersive window wrapper object.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Creating CImmersiveWindowWrapper object."
			);

			m_pCoreWindowWrapper = new (std::nothrow) CImmersiveWindowWrapper
			(
				(int) DefaultOutputLeft,
				(int) DefaultOutputTop,
				(UINT32) ( DefaultOutputRight - DefaultOutputLeft ),
				(UINT32) ( DefaultOutputBottom - DefaultOutputTop )
			);

			if ( m_pCoreWindowWrapper == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					E_OUTOFMEMORY,
					L"Failed to allocate new CImmersiveWindowWrapper object."
				);

				goto Cleanup;
			};


			//
			// Assign the immersive window wrapper a logger.
			//

			m_pCoreWindowWrapper->SetLogger( m_pLogger );


			//
			// Have the wrapper create an immersive window.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Calling StartImmersiveWindowThread on CImmersiveWindowWrapper object."
			);

			HRESULT hStartResult = m_pCoreWindowWrapper->StartImmersiveWindowThread();

			if ( FAILED( hStartResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hStartResult,
					L"StartImmersiveWindowThread on CImmersiveWindowWrapper object failed."
				);

				goto Cleanup;
			};


			//
			// Initialize the scene renderer.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Calling Initialize on scene renderer."
			);

			SCENE_TYPE SceneTypes[] =
			{
				SceneType,
				SceneType
			};

#pragma warning( suppress:  26001 )
			assert( m_ppSceneRenderers[ RenderingAPI ] != nullptr );

#pragma warning( suppress:  26001 )
			HRESULT hInitializeSceneRendererResult = m_ppSceneRenderers[ RenderingAPI ]->Initialize
			(
				SceneTypes,
				m_pCoreWindowWrapper->GetImmersiveWindow(),
				0,
				0,
				false,
				2,
				DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
				2
			);

			if ( hInitializeSceneRendererResult == S_FALSE )
			{
				bOverallResult = true;
				goto Cleanup;
			};

			if ( FAILED( hInitializeSceneRendererResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hInitializeSceneRendererResult,
					L"Initialize on scene renderer failed."
				);

				goto Cleanup;
			};


			//
			// Associate the immersive window wrapper with the scene.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Calling SetSceneRenderer on core window wrapper object."
			);

#pragma warning( suppress:  26001 )
			assert( m_ppSceneRenderers[ RenderingAPI ] != nullptr );

#pragma warning( suppress:  26001 )
			m_pCoreWindowWrapper->SetSceneRenderer( m_ppSceneRenderers[ RenderingAPI ] );
		} break;


		default:
		{
			assert( false && "Unhandled swap chain type.");
		} break;
	};


#if USE_DWMSLICER
	//
	// Start capturing frames using the DWM Slicer.
	// Note: This only works in Windowed mode, but we are always windowed for this test.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Starting DWM slicer frame capture."
	);

	RECT CaptureRect;
	CaptureRect.left = DefaultOutputLeft;
	CaptureRect.top = DefaultOutputTop;
	CaptureRect.right = DefaultOutputRight;
	CaptureRect.bottom = DefaultOutputBottom;

	m_pDWMSlicer->Begin
	(
		CaptureRect,
		30,	// Max frames to capture
		0,	// Frames to skip between captures
		ImagePipeline::SlicerMode::Mono,
		nullptr
	);
#endif


	//
	// Render and present.
	//

	DWORD SleepDuration = RENDER_TIME;
	if ( IsDebuggerPresent() == TRUE )
	{
		SleepDuration = RENDER_TIME_UNDER_DEBUGGER;
	};

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"Sleeping for %u milliseconds.",
			SleepDuration
		)
	);

	Sleep( SleepDuration );


#if USE_SCREENCAPTURE
	//
	// Capture the screen.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Capturing the desktop."
	);

	assert( m_pScreenCapture != nullptr );
	HRESULT hCaptureScreenResult = m_pScreenCapture->Capture();

	if ( FAILED( hCaptureScreenResult ) )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_WARNING,
			L"Desktop capture failed."
		);

		bVerificationsPerformed = false;
		bOverallResult = true;
		goto Cleanup;
	};


	//
	// Create a filename with which to save the captured frame.
	//

	static UINT iCurrentCapture = 1;
	CurrentCaptureFilename = WEX::Common::String().Format
	(
		L"TestSwapChainRendering_%3.3u.bmp",
		iCurrentCapture
	);

	++iCurrentCapture;


	//
	// Verify that the last captured frame contains the expected
	// texels color values.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"Looking for expected results."
	);

	if ( m_ExpectedResults.find( RenderingAPI ) == m_ExpectedResults.end() )
	{
		bVerificationsPerformed = false;
		bOverallResult = true;
		goto Cleanup;
	};


	if ( m_ExpectedResults[ RenderingAPI ].find( SwapChainType ) == m_ExpectedResults[ RenderingAPI ].end() )
	{
		bVerificationsPerformed = false;
		bOverallResult = true;
		goto Cleanup;
	};

	if ( m_ExpectedResults[ RenderingAPI ][ SwapChainType ].find( SceneType ) == m_ExpectedResults[ RenderingAPI ][ SwapChainType ].end() )
	{
		bVerificationsPerformed = false;
		bOverallResult = true;
		goto Cleanup;
	};


	if ( m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ].size() == 0 )
	{
		bVerificationsPerformed = false;
		bOverallResult = true;
		goto Cleanup;
	};


#if 0
	RECT DefaultOutputVirtualRectangle;
	ZeroMemory( &DefaultOutputVirtualRectangle, sizeof( DefaultOutputVirtualRectangle ) );
	BOOL bGetDefaultOutputVirtualRectangleResult = SystemParametersInfo
	(
		SPI_GETWORKAREA,
		0,
		(PVOID) &DefaultOutputVirtualRectangle,
		0
	);

	if ( bGetDefaultOutputVirtualRectangleResult != TRUE )
	{
		DWORD GetDefaultOutputVirtualRectangleError = GetLastError();
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			GetDefaultOutputVirtualRectangleError,
			L"SystemParametersInfo failed to retrieve SPI_GETWORKAREA."
		);

		bVerificationsPerformed = false;
		bOverallResult = false;
		goto Cleanup;
	};
#else
	//
	// Enumerate monitors in search of the leftmost and topmost extents.
	//

	RECT MonitorExtents;
	ZeroMemory( &MonitorExtents, sizeof( MonitorExtents ) );

	BOOL bEnumerateMonitorsResult = EnumDisplayMonitors
	(
		nullptr,
		nullptr,
		CSwapChainsTest::MonitorEnumProc,
		(LPARAM) &MonitorExtents
	);

	if ( bEnumerateMonitorsResult != TRUE )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"EnumDisplayMonitors failed."
		);

		bVerificationsPerformed = false;
		bOverallResult = false;
		goto Cleanup;
	};
#endif


	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Verifying pixels in screen capture."
	);

	bVerificationsPerformed = true;
	for
	(
		UINT iCurrentTexel = 0;
		iCurrentTexel < m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ].size();
		++iCurrentTexel
	)
	{
		LONG ScaledX = (LONG) ( ( (float) DefaultOutputRight - (float) DefaultOutputLeft - 1.f ) * m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_RelativeX + .5f );
		if ( ScaledX > (LONG) DefaultOutputRight - (LONG) DefaultOutputLeft - 1 )
		{
			ScaledX = (LONG) DefaultOutputRight - (LONG) DefaultOutputLeft - 1;
		};

		LONG ScaledY = (LONG) ( ( (float) DefaultOutputBottom - (float) DefaultOutputTop - 1.f ) * m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_RelativeY + .5f );
		if ( ScaledY > (LONG) DefaultOutputBottom - (LONG) DefaultOutputTop - 1 )
		{
			ScaledY = (LONG) DefaultOutputBottom - (LONG) DefaultOutputTop - 1;
		};

		COLORREF ActualPixelColor;
		assert( m_pScreenCapture != nullptr );
		HRESULT hGetPixelResult = m_pScreenCapture->GetPixel
		(
			ScaledX,
			ScaledY,
			&ActualPixelColor
		);

		if ( FAILED( hGetPixelResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hGetPixelResult,
				WEX::Common::String().Format
				(
					L"GetPixel on pixel (%i, %i) failed.",
					ScaledX,
					ScaledY
				)
			);

			bOverallResult = false;
			goto Cleanup;
		};


		FLOAT_COLOR ActualTexelColor;
		ActualTexelColor.r = (float) LOBYTE( LOWORD( ActualPixelColor ) ) / 255.f;
		ActualTexelColor.g = (float) HIBYTE( LOWORD( ActualPixelColor ) ) / 255.f;
		ActualTexelColor.b = (float) LOBYTE( HIWORD( ActualPixelColor ) ) / 255.f;
		ActualTexelColor.a = 1.f;

		if
		(
			ActualTexelColor.r > m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.r + .01f ||
			ActualTexelColor.r < m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.r - .01f ||

			ActualTexelColor.g > m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.g + .01f ||
			ActualTexelColor.g < m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.g - .01f ||

			ActualTexelColor.b > m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.b + .01f ||
			ActualTexelColor.b < m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.b - .01f
		)
		{
			LOG_ERROR
			(
				ERROR_TYPE_BOOL,
				false,
				WEX::Common::String().Format
				(
					L"Texel color (%f, %f, %f) at position (%u, %u) in captured frame differs from expected color (%f, %f, %f).",
					ActualTexelColor.r,
					ActualTexelColor.g,
					ActualTexelColor.b,
					ScaledX,
					ScaledY,
					m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.r,
					m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.g,
					m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.b
				)
			);


			//
			// Save the screen capture.
			//

			HRESULT hSaveScreenCaptureResult = m_pScreenCapture->Save
			(
				(PCWSTR) CurrentCaptureFilename,
				IMAGE_CONTAINER_FORMAT_BMP
			);

			// If our first attempt failed due to permissions...
			if ( hSaveScreenCaptureResult == E_ACCESSDENIED )
			{
				// ...then we'll try again under the temporary directory.
				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					L"Save on screen capture in local directory failed.  Trying to save to temporary directory."
				);

				// Get the temporary directory.
				WCHAR TempPath[ MAX_PATH ] = L"";
				DWORD nTempPath = GetTempPathW
				(
					ARRAYSIZE( TempPath ),
					TempPath
				);

				// If we couldn't get the temporary directory...
				if ( nTempPath == 0 )
				{
					// ...then log an error...
					DWORD GetTempPathError = GetLastError();
					LOG_ERROR
					(
						ERROR_TYPE_WIN32,
						GetTempPathError,
						L"GetTempPath failed."
					);
				}
				else
				{
					// ...otherwise, concatenate the filename and path...
					StringCchCatW
					(
						TempPath,
						ARRAYSIZE( TempPath ),
						(PCWSTR) CurrentCaptureFilename
					);

					// ...and retry the save.
					hSaveScreenCaptureResult = m_pScreenCapture->Save
					(
						TempPath,
						IMAGE_CONTAINER_FORMAT_BMP
					);

					if ( FAILED( hSaveScreenCaptureResult ) )
					{
						LOG_ERROR
						(
							ERROR_TYPE_HRESULT,
							hSaveScreenCaptureResult,
							WEX::Common::String().Format
							(
								L"Save on screen capture in temporary directory %s failed.",
								TempPath
							)
						);
					}
					else
					{
						LOG_MESSAGE
						(
							MESSAGE_TYPE_INFORMATIONAL,
							WEX::Common::String().Format
							(
								L"Screen capture saved to file %s.",
								TempPath
							)
						);
					};
				};
			}
			else if ( FAILED( hSaveScreenCaptureResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hSaveScreenCaptureResult,
					L"Save on screen capture failed."
				);
			}
			else
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					WEX::Common::String().Format
					(
						L"Screen capture saved to file %s.",
						(PCWSTR) CurrentCaptureFilename
					)
				);
			};

			bOverallResult = false;
			goto Cleanup;
		};
	};


	//
	// The captured frames look good.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Texels in captured frames were as expected."
	);


	bOverallResult = true;
#endif


#if USE_DWMSLICER
	//
	// End Slicer frame capture.
	//

	m_CapturedFrameAtlas = m_pDWMSlicer->End( m_cCapturedFrames );
	if ( 0 == m_cCapturedFrames )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_WARNING,
			L"DWMSlicer failed to capture any frames.  This may indicate that the content was placed into an overlay plane on the hardware, and is thus not an error condition."
		);

		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"The test will thus skip this variation, since it cannot be sure whether the buffers were presented in the expected order."
		);

		bVerificationsPerformed = false;
		bOverallResult = true;
		goto Cleanup;
	};

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"Captured %u frames of size %u x %u.",
			m_cCapturedFrames,
			DefaultOutputRight - DefaultOutputLeft,
			DefaultOutputBottom - DefaultOutputTop
		)
	);


	//
	// Create a filename with which to save the atlas.
	//

	static UINT iCurrentAtlas = 1;
	CurrentAtlasFilename = WEX::Common::String().Format
	(
		L"TestSwapChainRendering_%3.3u.bmp",
		iCurrentAtlas
	);

	++iCurrentAtlas;


	//
	// Save the atlas.
	//

	m_CapturedFrameAtlas->Save( (PCWSTR) CurrentAtlasFilename );


	//
	// Only proceed if the atlas contains presented frames.
	//

	HRESULT hIsAtlasNonEmptyResult = IsAtlasNonEmpty
	(
		(UINT) ( DefaultOutputRight - DefaultOutputLeft ),
		(UINT) ( DefaultOutputBottom - DefaultOutputTop )
	);

	if ( FAILED( hIsAtlasNonEmptyResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hIsAtlasNonEmptyResult,
			L"IsAtlasNonEmpty failed."
		);

		bOverallResult = false;
		goto Cleanup;
	};

	if ( hIsAtlasNonEmptyResult == S_FALSE )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"Atlas contained no opaque pixels.  This may indicate that the content was placed into an overlay plane on the hardware, and is thus not an error condition."
		);

		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"The test will thus skip this variation, since it cannot be sure whether the buffers were presented in the expected order."
		);

		bOverallResult = true;
		bVerificationsPerformed = false;
		goto Cleanup;
	};


	//
	// Verify that the last captured frame contains the expected
	// texels color values.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"Looking for expected results."
	);

	if
	(
		m_ExpectedResults.find( RenderingAPI ) != m_ExpectedResults.end() &&
		m_ExpectedResults[ RenderingAPI ].find( SwapChainType ) != m_ExpectedResults[ RenderingAPI ].end() &&
		m_ExpectedResults[ RenderingAPI ][ SwapChainType ].find( SceneType ) != m_ExpectedResults[ RenderingAPI ][ SwapChainType ].end() &&
		m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ].size() > 0
	)
	{
		bVerificationsPerformed = true;
		for
		(
			UINT iCurrentTexel = 0;
			iCurrentTexel < m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ].size();
			++iCurrentTexel
		)
		{
			LONG ScaledX = (LONG) ( ( (float) DefaultOutputRight - (float) DefaultOutputLeft - 1.f ) * m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_RelativeX + .5f );
			if ( ScaledX > (LONG) DefaultOutputRight - (LONG) DefaultOutputLeft - 1 )
			{
				ScaledX = (LONG) DefaultOutputRight - (LONG) DefaultOutputLeft - 1;
			};

			LONG ScaledY = (LONG) ( ( (float) DefaultOutputBottom - (float) DefaultOutputTop - 1.f ) * m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_RelativeY + .5f );
			if ( ScaledY > (LONG) DefaultOutputBottom - (LONG) DefaultOutputTop - 1 )
			{
				ScaledY = (LONG) DefaultOutputBottom - (LONG) DefaultOutputTop - 1;
			};

			FLOAT_COLOR ActualTexelColor;
			bool bGetTexelResult = GetTexel
			(
				m_cCapturedFrames - 1,
				DefaultOutputRight - DefaultOutputLeft,
				DefaultOutputBottom - DefaultOutputTop,
				ScaledX,
				ScaledY,
				&ActualTexelColor
			);

			if
			(
				ActualTexelColor.r > m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.r + .01f ||
				ActualTexelColor.r < m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.r - .01f ||

				ActualTexelColor.g > m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.g + .01f ||
				ActualTexelColor.g < m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.g - .01f ||

				ActualTexelColor.b > m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.b + .01f ||
				ActualTexelColor.b < m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.b - .01f ||

				ActualTexelColor.a > m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.a + .01f ||
				ActualTexelColor.a < m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.a - .01f
			)
			{
				LOG_ERROR
				(
					ERROR_TYPE_BOOL,
					false,
					WEX::Common::String().Format
					(
						L"Texel color (%f, %f, %f, %f) at position (%u, %u) in captured frame %u differs from expected color (%f, %f, %f, %f).",
						ActualTexelColor.r,
						ActualTexelColor.g,
						ActualTexelColor.b,
						ActualTexelColor.a,
						ScaledX,
						ScaledY,
						m_cCapturedFrames - 1,
						m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.r,
						m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.g,
						m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.b,
						m_ExpectedResults[ RenderingAPI ][ SwapChainType ][ SceneType ][ iCurrentTexel ].m_ExpectedColor.a
					)
				);

				bOverallResult = false;
				goto Cleanup;
			};
		};


		//
		// The captured frames look good.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"Texels in captured frames were as expected."
		);


		//
		// The test case passed, so there is no reason to keep the saved
		// atlas file.
		//

		BOOL bDeleteAtlasFileResult = DeleteFileW( CurrentAtlasFilename );

		if ( bDeleteAtlasFileResult != TRUE )
		{
			DWORD DeleteAtlasFileError = GetLastError();

			LOG_ERROR
			(
				ERROR_TYPE_WIN32,
				DeleteAtlasFileError,
				L"DeleteFile on atlas file failed."
			);
		};


		bOverallResult = true;
	};
#endif


Cleanup:
#pragma warning( suppress:  28922 )
	if ( pBackgroundSwapChain != nullptr )
	{
		pBackgroundSwapChain->Release();
		pBackgroundSwapChain = nullptr;
	};


#pragma warning( suppress:  28922 )
	if ( pForegroundSwapChain != nullptr )
	{
		pForegroundSwapChain->Release();
		pForegroundSwapChain = nullptr;
	};


	TESTCASE_RESULT TestGroupOverallResult = TESTCASE_RESULT_FAILED;
	if ( m_pLogger != nullptr )
	{
		//
		// Set the test case result.
		//

		if ( bOverallResult == true )
		{
			if ( bVerificationsPerformed == true )
			{
				m_pLogger->SetTestCaseResult
				(
					TESTCASE_RESULT_PASSED,
					L"All verifications passed."
				);
			}
			else
			{
				m_pLogger->SetTestCaseResult
				(
					TESTCASE_RESULT_SKIPPED,
					L"Verifications were skipped."
				);
			};
		}
		else
		{
			m_pLogger->SetTestCaseResult
			(
				TESTCASE_RESULT_FAILED,
				L"One or more variations failed."
			);
		};


		//
		// End the test case in the log.
		//

		m_pLogger->EndTestCase();


		//
		// Get the test group overall result.
		//

		TestGroupOverallResult = m_pLogger->GetTestGroupOverallResult();


		//
		// End the test group in the log.
		//

		m_pLogger->EndTestGroup();


		//
		// Log the test group overall result.
		//

		LogTestGroupOverallResult( TestGroupOverallResult );
	};


	CHECK_CONDITION( bOverallResult == true );
};


BOOL CALLBACK CSwapChainsTest::MonitorEnumProc
(
	_In_ HMONITOR /* hMonitor */,
	_In_ HDC /* hMonitorDC */,
	_In_ LPRECT pMonitorRectangle,
	_In_ LPARAM dwData
)
{
	//
	// Fish out the rectangle pointer.
	//

	RECT * pMonitorExtents = (RECT *) dwData;
	if ( pMonitorExtents == nullptr )
	{
		return FALSE;
	};


	//
	// Check this monitor's extents against the most extreme
	// extents that we've seen so far.
	//

	if ( pMonitorRectangle->left < pMonitorExtents->left )
	{
		pMonitorExtents->left = pMonitorRectangle->left;
	};

	if ( pMonitorRectangle->top < pMonitorExtents->top )
	{
		pMonitorExtents->top = pMonitorRectangle->top;
	};

	if ( pMonitorRectangle->right > pMonitorExtents->right )
	{
		pMonitorExtents->right = pMonitorRectangle->right;
	};

	if ( pMonitorRectangle->bottom > pMonitorExtents->bottom )
	{
		pMonitorExtents->bottom = pMonitorRectangle->bottom;
	};


	return TRUE;
};
