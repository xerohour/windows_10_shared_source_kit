//////////////////////////////////////////////////////////////////////
// File:  PresentDurationTest.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of DXGI presentation duration test group class.
//
// History:
// 23 Mar 2013  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "PresentDurationTest.hpp"


/////////////////////////////////////////////////////////////////
//
// Macros
//
/////////////////////////////////////////////////////////////////

#define DO_ETW_TRACING 1
#define EXPECT_CUSTOM_PRESENT_DURATION_SUPPORT 1

#define VERIFY_PER_FRAME_PRECISION 0
#define VERIFY_AVERAGE_PRECISION 1
#define VERIFY_GLITCH_COUNT 1


// How close does a duration need to be in order to be deemed
// deemed as suitable?  Let's say +/- 0.5%.
// 0.5% = 0.5 / 100 = 5 / 1000
#define CUSTOM_PRESENT_DURATION_SELECTION_TOLERANCE 5 / 1000

// How close does the timing of each frame need to be in order
// to be considered passing?  If we assume that we're attempting
// to hold IHVs to a threshold of +/- 0.5%, and we further
// assume that some of our computations will introduce error,
// let's give a frame-to-frame expectation of +/- 2% and an
// average expected tolerance of +/- 0.5%
#define CUSTOM_PRESENT_DURATION_PRECISION_PER_FRAME_TOLERANCE 20 / 1000
#define CUSTOM_PRESENT_DURATION_PRECISION_AVERAGE_TOLERANCE 5 / 1000


/////////////////////////////////////////////////////////////////
//
// Constants
//
/////////////////////////////////////////////////////////////////

const UINT MaxAcceptableGlitchCount = 0;


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  IsIntegratedVideoOutputType
//
// Purpose:
// Determines whether the given output type indicates
// an integrated panel or an external monitor.
///////////////////////////////////////////////////////
__inline BOOL _Check_return_ IsIntegratedVideoOutputType
(
	const DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY VideoOutputTechnologyType
)
{
	switch ( VideoOutputTechnologyType )
	{
		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL:
		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED:
		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED:
		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS:
		{
			return TRUE;
		} break;

		default:
		{
			return FALSE;
		} break;
	};
};


///////////////////////////////////////////////////////
// Function:  ToString( DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY )
//
// Purpose:
// Provides a string representation for a display
// output technology type.
///////////////////////////////////////////////////////
tstring ToString( const DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY OutputTechnology )
{
	switch ( OutputTechnology )
	{
		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_OTHER:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_OTHER" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HD15:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HD15" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SVIDEO:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SVIDEO" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPOSITE_VIDEO:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPOSITE_VIDEO" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPONENT_VIDEO:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPONENT_VIDEO" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DVI:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DVI" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HDMI:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HDMI" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_D_JPN:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_D_JPN" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDI:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDI" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EXTERNAL:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EXTERNAL" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EXTERNAL:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EXTERNAL" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDTVDONGLE:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDTVDONGLE" ) );
		} break;

		case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL:
		{
			return tstring( _T( "DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL" ) );
		} break;

		default:
		{
			return ToString( (UINT) OutputTechnology );
		} break;
	};
};


///////////////////////////////////////////////////////
// Function:  ToString( DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAGS )
//
// Purpose:
// Provides a string representation for a set of YCbCr
// flags.
///////////////////////////////////////////////////////
tstring ToString( const DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAGS & ColorSpace )
{
	DWORD ColorSpaceCopy = (DWORD) ColorSpace;
	tstring OutputString( _T( "" ) );

	if ( ( ColorSpaceCopy & DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_xvYCC ) != 0 )
	{
		OutputString += tstring( _T( "DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_xvYCC" ) );
		ColorSpaceCopy &= ~DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_xvYCC;
	};

	if ( ( ColorSpaceCopy & DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_BT709 ) != 0 )
	{
		if ( OutputString != _T( "" ) )
		{
			OutputString += tstring( _T( "|" ) );
		};

		OutputString += tstring( _T( "DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_BT709" ) );
		ColorSpaceCopy &= ~DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_BT709;
	};

	if ( ( ColorSpaceCopy & DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_NOMINAL_RANGE ) != 0 )
	{
		if ( OutputString != _T( "" ) )
		{
			OutputString += tstring( _T( "|" ) );
		};

		OutputString += tstring( _T( "DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_NOMINAL_RANGE" ) );
		ColorSpaceCopy &= ~DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_NOMINAL_RANGE;
	};

	if ( OutputString == _T( "" ) || ColorSpaceCopy != 0 )
	{
		TCHAR Buffer[ 256 ];
		_itot( ColorSpaceCopy, Buffer, 10 );

		if ( OutputString != _T( "" ) )
		{
			OutputString += tstring( _T( "|" ) );
		};

		OutputString += tstring( Buffer );
		ColorSpaceCopy = 0;
	};


	return OutputString;
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
	switch( FeatureLevel )
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
// CPresentDurationTest implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CPresentDurationTest
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CPresentDurationTest::CPresentDurationTest()
:
	CGraphicsTAEFTest(),
	m_hWindow( nullptr ),
    m_pFactory( nullptr ),
    m_pFactoryMedia( nullptr ),
	//m_SwapChainCreationMethod( SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORHWND ),
	//m_SwapChainCreationMethod( SWAPCHAIN_CREATION_METHOD_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACE ),
	m_SwapChainCreationMethod( SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORCOMPOSITIONSURFACE ),
	m_pSwapChain_IDXGISwapChain1( nullptr ),
	m_pSwapChain_IDXGIDecodeSwapChain( nullptr ),
	m_pSwapChain_IDXGISwapChainMedia( nullptr ),
	m_pDevice( nullptr ),
	m_pImmediateContext( nullptr ),
	m_pYUVDecodingBufferArray( nullptr ),
	m_SwapChainBufferWidth( 0 ),
	m_SwapChainBufferHeight( 0 ),
	m_SwapChainBufferFormat( DXGI_FORMAT_UNKNOWN ),
	m_SwapChainBufferArraySize( 2 ),
	m_SwapChainBufferBindFlags( D3D11_BIND_DECODER ),
	m_SwapChainBufferMiscFlags( 0 ),
	m_hCompositionSurface( nullptr ),
	m_EventProcessingThread( nullptr ),
	m_hEventTracingSession( 0 )
{
	m_DCompIsolation.SetLogger( m_pLogger );

	m_EventProcessingThread.SetLogger( m_pLogger );
};


///////////////////////////////////////////////////////
// Function:  ~CPresentDurationTest
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CPresentDurationTest::~CPresentDurationTest()
{
	//
	// Check for leaked objects.
	//

	assert( m_hCompositionSurface == nullptr );

	assert( m_pYUVDecodingBufferArray == nullptr );

	assert( m_pImmediateContext == nullptr );

	assert( m_pDevice == nullptr );

	assert( m_pSwapChain_IDXGISwapChain1 == nullptr );

	assert( m_pSwapChain_IDXGIDecodeSwapChain == nullptr );

	assert( m_pSwapChain_IDXGISwapChainMedia == nullptr );

    assert( m_pFactory == nullptr );
    assert( m_pFactoryMedia == nullptr );

	assert( m_hWindow == nullptr );

	assert( m_hEventTracingSession == 0 );
};


///////////////////////////////////////////////////////
// Function:  SetupTestGroup
//
// Purpose:
// Handles test-group-wide setup.
///////////////////////////////////////////////////////
bool CPresentDurationTest::SetupTestGroup()
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
		L"Present Duration Conformance SetupTestGroup",
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
bool CPresentDurationTest::CleanupTestGroup()
{
	if ( m_pLogger != nullptr )
	{
		//
		// Begin a test group in the log.
		//

		m_pLogger->BeginTestGroup
		(
			L"Present Duration Conformance CleanupTestGroup",
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
bool CPresentDurationTest::SetupTestCase()
{
	//
	// Initialize locals.
	//

	bool bErrorsWereDetected = true;
	bool bContinueToTestExecution = false;

	IDXGIAdapter * pDefaultAdapter = nullptr;
	IDXGIOutput * pDefaultOutput = nullptr;

	IDXGIResource * pYUVDecodingBufferArrayResource = nullptr;


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
		L"Present Duration Conformance SetupTestCase",
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
	// Create a DXGI factory.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Creating DXGI factory."
	);

	HRESULT hCreateFactoryResult = CreateDXGIFactory
	(
		IID_IDXGIFactory3,
		(void **) &m_pFactory
	);

	if ( FAILED( hCreateFactoryResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateFactoryResult,
			L"CreateDXGIFactory failed."
		);

		goto Cleanup;
	};

	if ( m_pFactory == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_POINTER,
			L"CreateDXGIFactory succeeded but returned a NULL factory pointer."
		);

		goto Cleanup;
	};


    //
    // Query the factory for its IDXGIFactoryMedia interface.
    //

    LOG_MESSAGE
    (
        MESSAGE_TYPE_INFORMATIONAL,
        L"Querying for IDXGIFactoryMedia interface."
    );

    HRESULT hQueryForIDXGIFactoryMediaResult = m_pFactory->QueryInterface
    (
        __uuidof(m_pFactoryMedia),
        (void **) &m_pFactoryMedia
    );

    if ( FAILED( hQueryForIDXGIFactoryMediaResult ) )
    {
        LOG_ERROR
        (
            ERROR_TYPE_HRESULT,
            hQueryForIDXGIFactoryMediaResult,
            L"QueryInterface on factory for IDXGIFactoryMedia interface failed."
        );

        goto Cleanup;
    };

    if ( m_pFactoryMedia == nullptr )
    {
        LOG_ERROR
        (
            ERROR_TYPE_HRESULT,
            E_POINTER,
            L"QueryInterface on factory for IDXGIFactoryMedia succeeded but returned NULL pointer."
        );

        goto Cleanup;
    };

	//
	// Fill out our window class struct.
	//

	WNDCLASSEXW WindowClass;
	ZeroMemory( &WindowClass, sizeof( WindowClass ) );
	WindowClass.cbSize = sizeof( WindowClass );
	WindowClass.style = CS_DBLCLKS;
	WindowClass.lpfnWndProc = StaticMsgProc;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = sizeof( PVOID );
	WindowClass.hInstance = GetModuleHandle( nullptr );
	WindowClass.hbrBackground = (HBRUSH) COLOR_WINDOW + 1;
	WindowClass.lpszClassName = L"PresentDuration";


	//
	// Register our window class.
	//

	m_WindowClassAtom = RegisterClassExW( &WindowClass );

	if ( m_WindowClassAtom == 0 )
	{
		DWORD RegisterClassError = GetLastError();

		if ( RegisterClassError != ERROR_CLASS_ALREADY_EXISTS )
		{
			LOG_ERROR
			(
				ERROR_TYPE_WIN32,
				RegisterClassError,
				L"RegisterClassEx failed."
			);

			goto Cleanup;
		};
	};


	//
	// Enumerate the default adapter.
	//

	HRESULT hGetDefaultAdapterResult = m_pFactory->EnumAdapters
	(
		0,
		&pDefaultAdapter
	);

	if ( FAILED( hGetDefaultAdapterResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hGetDefaultAdapterResult,
			L"EnumAdapters on adapter 0 failed."
		);

		goto Cleanup;
	};


	//
	// Enumerate the default output.
	//

	HRESULT hGetDefaultOutputResult = pDefaultAdapter->EnumOutputs
	(
		0,
		&pDefaultOutput
	);

	if ( FAILED( hGetDefaultOutputResult ) )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"EnumOutputs on output 0 failed."
		);

		bErrorsWereDetected = false;
		goto Cleanup;
	};


	//
	// Determine the current mode for the default output.
	//

	DXGI_OUTPUT_DESC DefaultOutputDescriptor;
	ZeroMemory( &DefaultOutputDescriptor, sizeof( DefaultOutputDescriptor ) );
	HRESULT hGetDefaultOutputDescriptorResult = pDefaultOutput->GetDesc( &DefaultOutputDescriptor );
	if ( FAILED( hGetDefaultOutputDescriptorResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hGetDefaultOutputDescriptorResult,
			L"GetDesc on default output failed."
		);

		goto Cleanup;
	};


	//
	// Create a window that spans the default output.
	//

	DWORD ExtendedStyles = IsDebuggerPresent() == TRUE ? 0 : WS_EX_TOPMOST;

	WCHAR WindowCaption[ 256 ] = L"Present duration test";
	m_hWindow = CreateWindowExW
	(
		ExtendedStyles,
		L"PresentDuration",
		WindowCaption,
		WS_POPUP | WS_VISIBLE,
		DefaultOutputDescriptor.DesktopCoordinates.left,
		DefaultOutputDescriptor.DesktopCoordinates.top,
		DefaultOutputDescriptor.DesktopCoordinates.right - DefaultOutputDescriptor.DesktopCoordinates.left,
		DefaultOutputDescriptor.DesktopCoordinates.bottom - DefaultOutputDescriptor.DesktopCoordinates.top,
		nullptr,
		nullptr,
		WindowClass.hInstance,
		(void *) this
	);

	if ( m_hWindow == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"CreateWindowEx failed."
		);

		goto Cleanup;
	};


	//
	// Verify the window's size and position.
	//

	RECT WindowRect;
	GetWindowRect( m_hWindow, &WindowRect );
	if
	(
		WindowRect.left != DefaultOutputDescriptor.DesktopCoordinates.left ||
		WindowRect.top != DefaultOutputDescriptor.DesktopCoordinates.top ||
		WindowRect.right != DefaultOutputDescriptor.DesktopCoordinates.right ||
		WindowRect.bottom != DefaultOutputDescriptor.DesktopCoordinates.bottom
	)
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"SetupTestCase:  Window rectangle (%d, %d) to (%d, %d) differs from expected rectangle (%d, %d) to (%d, %d).",
				WindowRect.left,
				WindowRect.top,
				WindowRect.right,
				WindowRect.bottom,
				DefaultOutputDescriptor.DesktopCoordinates.left,
				DefaultOutputDescriptor.DesktopCoordinates.top,
				DefaultOutputDescriptor.DesktopCoordinates.right,
				DefaultOutputDescriptor.DesktopCoordinates.bottom
			)
		);
	};


	//
	// Set the swap chain width and height to match the display
	// resolution.
	//

	m_SwapChainBufferWidth = DefaultOutputDescriptor.DesktopCoordinates.right - DefaultOutputDescriptor.DesktopCoordinates.left;
	m_SwapChainBufferHeight = DefaultOutputDescriptor.DesktopCoordinates.bottom - DefaultOutputDescriptor.DesktopCoordinates.top;

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"SetupTestCase:  Setting swap chain dimensions to %u x %u.",
			m_SwapChainBufferWidth,
			m_SwapChainBufferHeight
		)
	);


	//
	// Create a Direct3D 11 device.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Creating Direct3D 11 device."
	);

	D3D_FEATURE_LEVEL FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	D3D_FEATURE_LEVEL ActualFeatureLevel = D3D_FEATURE_LEVEL_9_1;
	HRESULT hCreateDeviceResult = D3D11CreateDevice
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT | ( IsDebuggerPresent() == TRUE ? D3D11_CREATE_DEVICE_DEBUG : 0 ),
		FeatureLevels,
		ARRAYSIZE( FeatureLevels ),
		D3D11_SDK_VERSION,
		&m_pDevice,
		&ActualFeatureLevel,
		&m_pImmediateContext
	);

	if ( FAILED( hCreateDeviceResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateDeviceResult,
			L"D3D11CreateDevice failed."
		);

		goto Cleanup;
	};

	if ( m_pDevice == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_POINTER,
			L"D3D11CreateDevice succeeded but returned NULL device pointer."
		);

		goto Cleanup;
	};

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Device was created successfully."
	);


	//
	// Search for a format that will meet our needs.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Looking for a suitable buffer format."
	);

	bool bSuitableFormatFound = false;
	switch ( m_SwapChainCreationMethod )
	{
		case SWAPCHAIN_CREATION_METHOD_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACE:
		{
			for
			(
				DXGI_FORMAT TempFormat = DXGI_FORMAT_R8G8B8A8_UNORM; // DXGI_FORMAT_R32G32B32A32_TYPELESS;
				TempFormat <= DXGI_FORMAT_B4G4R4A4_UNORM;
				TempFormat = (DXGI_FORMAT) ( (UINT) TempFormat + 1 )
			)
			{
				UINT SupportForTempBufferFormat = 0;
				m_pDevice->CheckFormatSupport
				(
					TempFormat,
					&SupportForTempBufferFormat
				);

				if
				(
					( SupportForTempBufferFormat & D3D11_FORMAT_SUPPORT_BUFFER ) != 0 &&
					( SupportForTempBufferFormat & D3D11_FORMAT_SUPPORT_TEXTURE2D ) != 0 &&
					( SupportForTempBufferFormat & D3D11_FORMAT_SUPPORT_DECODER_OUTPUT ) != 0
				)
				{
					LOG_MESSAGE
					(
						MESSAGE_TYPE_INFORMATIONAL,
						WEX::Common::String().Format
						(
							L"Using format %u.",
							(UINT) TempFormat
						)
					);

					m_SwapChainBufferFormat = TempFormat;

					m_SwapChainBufferBindFlags = D3D11_BIND_DECODER;
					m_SwapChainBufferMiscFlags = 0;

					bSuitableFormatFound = true;

					break;
				};
			};
		} break;


		case SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORCOMPOSITIONSURFACE:
		case SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORHWND:
		{
			m_SwapChainBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

			m_SwapChainBufferBindFlags = 0;
			m_SwapChainBufferMiscFlags = 0;

			bSuitableFormatFound = true;
		} break;


		default:
		{
			assert( false && L"Unhandled swap chain type." );
		} break;
	};


	//
	// Error out if we couldn't find a good format.
	//

	if ( bSuitableFormatFound != true )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"No suitable format was found."
		);

		bErrorsWereDetected = false;
		goto Cleanup;
	};


	//
	// Create the swap chain.
	//

	switch ( m_SwapChainCreationMethod )
	{
		case SWAPCHAIN_CREATION_METHOD_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACE:
		{
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
				L"Composition surface handle was created successfully."
			);


			//
			// Create a texture array of YUV decode buffers.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Creating texture array of YUV decoding swap chain buffers."
			);

			D3D11_TEXTURE2D_DESC YUVDecodingBufferArrayDescriptor;
			ZeroMemory( &YUVDecodingBufferArrayDescriptor, sizeof( YUVDecodingBufferArrayDescriptor ) );
			YUVDecodingBufferArrayDescriptor.Width = m_SwapChainBufferWidth;
			YUVDecodingBufferArrayDescriptor.Height = m_SwapChainBufferHeight;
			YUVDecodingBufferArrayDescriptor.MipLevels = 1;
			YUVDecodingBufferArrayDescriptor.ArraySize = m_SwapChainBufferArraySize;
			YUVDecodingBufferArrayDescriptor.Format = m_SwapChainBufferFormat;
			YUVDecodingBufferArrayDescriptor.SampleDesc.Count = 1;
			YUVDecodingBufferArrayDescriptor.SampleDesc.Quality = 0;
			YUVDecodingBufferArrayDescriptor.Usage = D3D11_USAGE_DEFAULT;
			YUVDecodingBufferArrayDescriptor.BindFlags = m_SwapChainBufferBindFlags;
			YUVDecodingBufferArrayDescriptor.CPUAccessFlags = 0;
			YUVDecodingBufferArrayDescriptor.MiscFlags = m_SwapChainBufferMiscFlags;

			HRESULT hCreateYUVDecodingBufferArrayResult = m_pDevice->CreateTexture2D
			(
				&YUVDecodingBufferArrayDescriptor,
				nullptr,
				&m_pYUVDecodingBufferArray
			);

			if
			(
				hCreateYUVDecodingBufferArrayResult == E_INVALIDARG &&
				m_pDevice->GetFeatureLevel() <= D3D_FEATURE_LEVEL_11_0 &&
				m_SwapChainBufferArraySize > 1 &&
				( m_SwapChainBufferMiscFlags & D3D11_RESOURCE_MISC_SHARED_NTHANDLE ) != 0
			)
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					L"CreateTexture2D on YUV decoding buffer array returned E_INVALIDARG because for feature levels 11_0 and below, D3D11_RESOURCE_MISC_SHARED_NTHANDLE is incompatible with texture arrays with more than 1 element."
				);

				bErrorsWereDetected = false;
				goto Cleanup;
			};

			if
			(
				hCreateYUVDecodingBufferArrayResult == E_INVALIDARG &&
				m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 &&
				(
					m_SwapChainBufferFormat == DXGI_FORMAT_AYUV ||
					m_SwapChainBufferFormat == DXGI_FORMAT_YUY2 ||
					m_SwapChainBufferFormat == DXGI_FORMAT_NV11 ||
					m_SwapChainBufferFormat == DXGI_FORMAT_NV12
				)
			)
			{
#if defined(UNICODE)
				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					WEX::Common::String().Format
					(
						L"CreateTexture2D on YUV decoding buffer array returned E_INVALIDARG because format %s is not supported by feature level %s.",
						ToString( m_SwapChainBufferFormat ).c_str(),
						ToString( m_pDevice->GetFeatureLevel() ).c_str()
					)
				);
#else
				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					WEX::Common::String().Format
					(
						L"CreateTexture2D on YUV decoding buffer array returned E_INVALIDARG because format %S is not supported by feature level %S.",
						ToString( m_SwapChainBufferFormat ).c_str(),
						ToString( m_pDevice->GetFeatureLevel() ).c_str()
					)
				);
#endif

				bErrorsWereDetected = false;
				goto Cleanup;
			};

			if ( FAILED( hCreateYUVDecodingBufferArrayResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hCreateYUVDecodingBufferArrayResult,
					L"CreateTexture2D on YUV decoding buffer array failed."
				);

				goto Cleanup;
			};

			if ( m_pYUVDecodingBufferArray == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					E_POINTER,
					L"CreateTexture2D on YUV decoding buffer array succeeded but returned NULL texture array pointer."
				);

				goto Cleanup;
			};

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"YUV decoding buffer array was created successfully."
			);


			//
			// Query the texture array for its IDXGIResource interface.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Querying for IDXGIResource interface."
			);

			HRESULT hQueryForIDXGIResourceResult = m_pYUVDecodingBufferArray->QueryInterface
			(
				IID_IDXGIResource,
				(void **) &pYUVDecodingBufferArrayResource
			);

			if ( FAILED( hQueryForIDXGIResourceResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hQueryForIDXGIResourceResult,
					L"QueryInterface on YUV decoding buffer array for IDXGIResource interface failed."
				);

				goto Cleanup;
			};

			if ( pYUVDecodingBufferArrayResource == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					E_POINTER,
					L"QueryInterface on YUV decoding buffer array for IDXGIResource succeeded but returned NULL resource pointer."
				);

				goto Cleanup;
			};


			//
			// Create a YUV decoding swap chain.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Creating swap chain with CreateDecodeSwapChainForCompositionSurfaceHandle."
			);

			DXGI_DECODE_SWAP_CHAIN_DESC SwapChainDescriptor;
			ZeroMemory( &SwapChainDescriptor, sizeof( SwapChainDescriptor ) );
			SwapChainDescriptor.Flags = DXGI_SWAP_CHAIN_FLAG_FULLSCREEN_VIDEO; // @REVIEW

			assert( m_pSwapChain_IDXGIDecodeSwapChain == nullptr );
			HRESULT hCreateSwapChainResult = m_pFactoryMedia->CreateDecodeSwapChainForCompositionSurfaceHandle
			(
				m_pDevice,
				m_hCompositionSurface,
				&SwapChainDescriptor,
				pYUVDecodingBufferArrayResource,
				nullptr,
				&m_pSwapChain_IDXGIDecodeSwapChain
			);

			if ( FAILED( hCreateSwapChainResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hCreateSwapChainResult,
					L"CreateDecodeSwapChainForCompositionSurfaceHandle failed."
				);

				goto Cleanup;
			};

			if ( m_pSwapChain_IDXGIDecodeSwapChain == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					E_POINTER,
					L"CreateDecodeSwapChainForCompositionSurfaceHandle succeeded but returned NULL swap chain pointer."
				);

				goto Cleanup;
			};

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"YUV decoding swap chain was created successfully."
			);


			//
			// Query the swap chain for its IDXGISwapChain1 interface.
			//

			HRESULT hQuerySwapChainForIDXGISwapChain1Result = m_pSwapChain_IDXGIDecodeSwapChain->QueryInterface
			(
				IID_IDXGISwapChain1,
				(void **) &m_pSwapChain_IDXGISwapChain1
			);

			if ( FAILED( hQuerySwapChainForIDXGISwapChain1Result ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hQuerySwapChainForIDXGISwapChain1Result,
					L"QueryInterface for IDXGISwapChain1 failed."
				);

				goto Cleanup;
			};

			if ( m_pSwapChain_IDXGISwapChain1 == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					E_POINTER,
					L"QueryInterface for IDXGISwapChain1 succeeded but returned NULL interface pointer."
				);

				goto Cleanup;
			};


			//
			// Query the swap chain for its IDXGISwapChainMedia interface.
			//

			HRESULT hQuerySwapChainForIDXGISwapChainMediaResult = m_pSwapChain_IDXGIDecodeSwapChain->QueryInterface
			(
				__uuidof( IDXGISwapChainMedia ),
				(void **) &m_pSwapChain_IDXGISwapChainMedia
			);

			if ( FAILED( hQuerySwapChainForIDXGISwapChainMediaResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hQuerySwapChainForIDXGISwapChainMediaResult,
					L"QueryInterface for IDXGISwapChainMedia failed."
				);

				goto Cleanup;
			};

			if ( m_pSwapChain_IDXGISwapChainMedia == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					E_POINTER,
					L"QueryInterface for IDXGISwapChainMedia succeeded but returned NULL interface pointer."
				);

				goto Cleanup;
			};
		} break;


		case SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORCOMPOSITIONSURFACE:
		{
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
				L"Composition surface handle was created successfully."
			);


			//
			// Create a swap chain.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Creating swap chain with CreateSwapChainForCompositionSurfaceHandle."
			);

			DXGI_SWAP_CHAIN_DESC1 SwapChainDescriptor;
			ZeroMemory( &SwapChainDescriptor, sizeof( SwapChainDescriptor ) );
			SwapChainDescriptor.Width = m_SwapChainBufferWidth;
			SwapChainDescriptor.Height = m_SwapChainBufferHeight;
			SwapChainDescriptor.Format = m_SwapChainBufferFormat;
			SwapChainDescriptor.Stereo = FALSE;
			SwapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			SwapChainDescriptor.SampleDesc.Count = 1;
			SwapChainDescriptor.SampleDesc.Quality = 0;
			SwapChainDescriptor.Scaling = DXGI_SCALING_STRETCH;
			SwapChainDescriptor.BufferCount = 3;
			SwapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			SwapChainDescriptor.Flags = 0;

			assert( m_pFactoryMedia != nullptr );
			assert( m_hWindow != nullptr );
			HRESULT hCreateSwapChainResult = m_pFactoryMedia->CreateSwapChainForCompositionSurfaceHandle
			(
				m_pDevice,
				m_hCompositionSurface,
				&SwapChainDescriptor,
				nullptr,
				&m_pSwapChain_IDXGISwapChain1
			);

			if ( FAILED( hCreateSwapChainResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hCreateSwapChainResult,
					L"CreateSwapChainForCompositionSurfaceHandle failed."
				);

				goto Cleanup;
			};


			//
			// Query the swap chain for its IDXGISwapChainMedia interface.
			//

			HRESULT hQuerySwapChainForIDXGISwapChainMediaResult = m_pSwapChain_IDXGISwapChain1->QueryInterface
			(
				__uuidof( IDXGISwapChainMedia ),
				(void **) &m_pSwapChain_IDXGISwapChainMedia
			);

			if ( FAILED( hQuerySwapChainForIDXGISwapChainMediaResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hQuerySwapChainForIDXGISwapChainMediaResult,
					L"QueryInterface for IDXGISwapChainMedia failed."
				);

				goto Cleanup;
			};

			if ( m_pSwapChain_IDXGISwapChainMedia == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					E_POINTER,
					L"QueryInterface for IDXGISwapChainMedia succeeded but returned NULL interface pointer."
				);

				goto Cleanup;
			};
		} break;


		case SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORHWND:
		{
			//
			// Create a swap chain.
			//

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"Creating swap chain with CreateSwapChainForHwnd."
			);

			DXGI_SWAP_CHAIN_DESC1 SwapChainDescriptor;
			ZeroMemory( &SwapChainDescriptor, sizeof( SwapChainDescriptor ) );
			SwapChainDescriptor.Width = m_SwapChainBufferWidth;
			SwapChainDescriptor.Height = m_SwapChainBufferHeight;
			SwapChainDescriptor.Format = m_SwapChainBufferFormat;
			SwapChainDescriptor.Stereo = FALSE;
			SwapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			SwapChainDescriptor.SampleDesc.Count = 1;
			SwapChainDescriptor.SampleDesc.Quality = 0;
			SwapChainDescriptor.Scaling = DXGI_SCALING_STRETCH;
			SwapChainDescriptor.BufferCount = 3;
			SwapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			SwapChainDescriptor.Flags = 0;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC SwapChainFullscreenDescriptor;
			ZeroMemory( &SwapChainFullscreenDescriptor, sizeof( SwapChainFullscreenDescriptor ) );
			SwapChainFullscreenDescriptor.RefreshRate.Numerator = 60;
			SwapChainFullscreenDescriptor.RefreshRate.Denominator = 1;
			SwapChainFullscreenDescriptor.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			SwapChainFullscreenDescriptor.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			SwapChainFullscreenDescriptor.Windowed = TRUE;

			assert( m_pFactory != nullptr );
			assert( m_hWindow != nullptr );
			HRESULT hCreateSwapChainResult = m_pFactory->CreateSwapChainForHwnd
			(
				m_pDevice,
				m_hWindow,
				&SwapChainDescriptor,
				&SwapChainFullscreenDescriptor, // nullptr,
				nullptr,
				&m_pSwapChain_IDXGISwapChain1
			);

			if ( FAILED( hCreateSwapChainResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hCreateSwapChainResult,
					L"CreateSwapChainForHwnd failed."
				);

				goto Cleanup;
			};


			//
			// Query the swap chain for its IDXGISwapChainMedia interface.
			//

			HRESULT hQuerySwapChainForIDXGISwapChainMediaResult = m_pSwapChain_IDXGISwapChain1->QueryInterface
			(
				__uuidof( IDXGISwapChainMedia ),
				(void **) &m_pSwapChain_IDXGISwapChainMedia
			);

			if ( FAILED( hQuerySwapChainForIDXGISwapChainMediaResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hQuerySwapChainForIDXGISwapChainMediaResult,
					L"QueryInterface for IDXGISwapChainMedia failed."
				);

				goto Cleanup;
			};

			if ( m_pSwapChain_IDXGISwapChainMedia == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					E_POINTER,
					L"QueryInterface for IDXGISwapChainMedia succeeded but returned NULL interface pointer."
				);

				goto Cleanup;
			};
		} break;


		default:
		{
			assert( false && L"Unhandled swap chain type." );
		} break;
	};

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Swap chain was created successfully."
	);


	bContinueToTestExecution = true;
	bErrorsWereDetected = false;


Cleanup:
	if ( pYUVDecodingBufferArrayResource != nullptr )
	{
		pYUVDecodingBufferArrayResource->Release();
		pYUVDecodingBufferArrayResource = nullptr;
	};


	if ( pDefaultOutput != nullptr )
	{
		pDefaultOutput->Release();
		pDefaultOutput = nullptr;
	};

	if ( pDefaultAdapter != nullptr )
	{
		pDefaultAdapter->Release();
		pDefaultAdapter = nullptr;
	};


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
bool CPresentDurationTest::CleanupTestCase()
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
			L"Present Duration Conformance CleanupTestCase",
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


	if ( m_hCompositionSurface != nullptr )
	{
		CloseHandle( m_hCompositionSurface );
		m_hCompositionSurface = nullptr;
	};


	if ( m_pYUVDecodingBufferArray != nullptr )
	{
		m_pYUVDecodingBufferArray->Release();
		m_pYUVDecodingBufferArray = nullptr;
	};


	if ( m_pImmediateContext != nullptr )
	{
		m_pImmediateContext->Release();
		m_pImmediateContext = nullptr;
	};


	if ( m_pDevice != nullptr )
	{
		m_pDevice->Release();
		m_pDevice = nullptr;
	};


	if ( m_pSwapChain_IDXGISwapChain1 != nullptr )
	{
		m_pSwapChain_IDXGISwapChain1->SetFullscreenState( FALSE, nullptr );

		m_pSwapChain_IDXGISwapChain1->Release();
		m_pSwapChain_IDXGISwapChain1 = nullptr;
	};


	if ( m_pSwapChain_IDXGIDecodeSwapChain != nullptr )
	{
		m_pSwapChain_IDXGIDecodeSwapChain->Release();
		m_pSwapChain_IDXGIDecodeSwapChain = nullptr;
	};


	if ( m_pSwapChain_IDXGISwapChainMedia != nullptr )
	{
		m_pSwapChain_IDXGISwapChainMedia->Release();
		m_pSwapChain_IDXGISwapChainMedia = nullptr;
	};

	if ( m_pFactory != nullptr )
	{
		HWND hwndCurrentFocus = nullptr;

		m_pFactory->GetWindowAssociation( &hwndCurrentFocus );
		if ( hwndCurrentFocus != nullptr )
		{
			m_pFactory->MakeWindowAssociation( nullptr, 0 );
		}

		m_pFactory->Release();
		m_pFactory = nullptr;
	};

    if ( m_pFactoryMedia != nullptr )
    {
        m_pFactoryMedia->Release();
        m_pFactoryMedia = nullptr;
    };

	//
	// Destroy our window.
	//

	if ( m_hWindow != nullptr )
	{
		DestroyWindow( m_hWindow );
		m_hWindow = nullptr;
	};


	//
	// Unregister the window class.
	//

	BOOL bUnregisterClassResult = UnregisterClassW
	(
		L"PresentDuration",
		GetModuleHandle( nullptr )
	);

	if ( bUnregisterClassResult != TRUE )
	{
		DWORD UnregisterClassError = GetLastError();

		if ( UnregisterClassError != ERROR_CLASS_DOES_NOT_EXIST )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"CleanupTestCase:  UnregisterClass failed."
			);
		};
	};


	//
	// Clean up DirectComposition state.
	//

	m_DCompIsolation.Cleanup();


	//
	// Call the base class implementation.
	//

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
// Function:  LogTestCaseParameters
//
// Purpose:
// Logs parameters for this test variation.
///////////////////////////////////////////////////////
void CPresentDurationTest::LogTestCaseParameters()
{
};


///////////////////////////////////////////////////////
// Function:  TestPresentDurationPrecision_WoW64
//
// Purpose:
// This method is just a wrapper that gives us a place
// to hang metadata for creating the WoW64 version of
// the job.
///////////////////////////////////////////////////////
void CPresentDurationTest::TestPresentDurationPrecision_WoW64()
{
	TestPresentDurationPrecision( true );
};


///////////////////////////////////////////////////////
// Function:  TestPresentDurationPrecision_Native
//
// Purpose:
// This method is just a wrapper that gives us a place
// to hang metadata for creating the native version of
// the job.
///////////////////////////////////////////////////////
void CPresentDurationTest::TestPresentDurationPrecision_Native()
{
	TestPresentDurationPrecision( false );
};


///////////////////////////////////////////////////////
// Function:  TestPresentDurationPrecision
//
// Purpose:
// Tests whether the device implements custom present
// durations to within a specified tolerance.
///////////////////////////////////////////////////////
void CPresentDurationTest::TestPresentDurationPrecision
(
	bool bRunUnderWoW64
)
{
	//
	// Initialize locals.
	//

	bool bOverallResult = false;
	bool bVerificationsPerformed = false;

	IDXGISwapChain * pSwapChain = nullptr;


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
		bRunUnderWoW64 == true ? L"Present Duration Conformance:  Custom Duration Precision Test (WoW64)" : L"Present Duration Conformance:  Custom Duration Precision Test (Native)",
		3,
		TESTCASE_RESULT_SKIPPED
	);


	//
	// Begin a test case in the log.
	//

	m_pLogger->BeginTestCase
	(
		L"TestPresentDurationPrecision",
		0xFFFFFFFF,
		nullptr
	);


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


	//
	// Initialize DirectComposition objects if necessary.
	//

	switch( m_SwapChainCreationMethod )
	{
		case SWAPCHAIN_CREATION_METHOD_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACE:
		{
			//
			// Set the window as the DirectComposition target.
			//

			HRESULT hSetTargetWindowResult = m_DCompIsolation.SetTargetWindow
			(
				m_hWindow
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
			// Query the swap chain for an IDXGISwapChain interface pointer.
			//

			HRESULT hQueryForIDXGISwapChainResult = m_pSwapChain_IDXGIDecodeSwapChain->QueryInterface
			(
				IID_IDXGISwapChain,
				(void **) &pSwapChain
			);

			if ( FAILED( hQueryForIDXGISwapChainResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hQueryForIDXGISwapChainResult,
					L"QueryInterface for IDXGISwapChain failed."
				);

				goto Cleanup;
			};


			//
			// Associate the source swap chain with the DirectComposition
			// visual tree.
			//

			HRESULT hSetSourceSwapChainResult = m_DCompIsolation.SetSourceSwapChain
			(
				pSwapChain,
				false // @REVIEW
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
		} break;


		case SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORCOMPOSITIONSURFACE:
		{
			//
			// Set the window as the DirectComposition target.
			//

			HRESULT hSetTargetWindowResult = m_DCompIsolation.SetTargetWindow
			(
				m_hWindow
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
			// Query the swap chain for an IDXGISwapChain interface pointer.
			//

			HRESULT hQueryForIDXGISwapChainResult = m_pSwapChain_IDXGISwapChainMedia->QueryInterface
			(
				IID_IDXGISwapChain,
				(void **) &pSwapChain
			);

			if ( FAILED( hQueryForIDXGISwapChainResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hQueryForIDXGISwapChainResult,
					L"QueryInterface for IDXGISwapChain failed."
				);

				goto Cleanup;
			};


			//
			// Associate the source swap chain with the DirectComposition
			// visual tree.
			//

			HRESULT hSetSourceSwapChainResult = m_DCompIsolation.SetSourceSwapChain
			(
				pSwapChain,
				false // @REVIEW
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
		};


		case SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORHWND:
		{
		} break;


		default:
		{
			assert( false && L"Unhandled swap chain type." );
		} break;
	};


#if DO_ETW_TRACING
	//
	// Begin tracing and processing ETW events.
	//

	HRESULT hBeginETWTraceResult = BeginETWTrace();

	if ( FAILED( hBeginETWTraceResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hBeginETWTraceResult,
			L"BeginETWTrace failed."
		);

		goto Cleanup;
	};
#endif


	//
	// Verify support for a number of interesting custom
	// presentation durations.
	//

	UINT CustomPresentDurationsToTest[] =
	{
		// 50 Hz
		10 * 1000 * 1000 / 50,

		// 48 Hz
		10 * 1000 * 1000 / 48,

		// 47.95 Hz
		208542,

		// 30 Hz
		10 * 1000 * 1000 / 30,

		// 25 Hz
		10 * 1000 * 1000 / 25,

		// 24 Hz
		10 * 1000 * 1000 / 24,

		// 23.97 Hz
		417083
	};

	for
	(
		UINT iCurrentCustomPresentDuration = 0;
		iCurrentCustomPresentDuration < ARRAYSIZE( CustomPresentDurationsToTest );
		++iCurrentCustomPresentDuration
	)
	{
		//
		// Determine whether the graphics adapter supports the
		// specified custom presentation duration.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"**** Querying for support for custom presentation duration %u. ****",
				CustomPresentDurationsToTest[ iCurrentCustomPresentDuration ]
			)
		);

		UINT ClosestSmallerPresentDuration = 0;
		UINT ClosestLargerPresentDuration = 0;
#if EXPECT_CUSTOM_PRESENT_DURATION_SUPPORT
		HRESULT hCheckPresentDurationResult = m_pSwapChain_IDXGISwapChainMedia->CheckPresentDurationSupport
		(
			CustomPresentDurationsToTest[ iCurrentCustomPresentDuration ],
			&ClosestSmallerPresentDuration,
			&ClosestLargerPresentDuration
		);
#else
		// Temporary workaround:  Pretend that 48 Hz is supported.
		if ( CustomPresentDurationsToTest[ iCurrentCustomPresentDuration ] == 208333 )
		{
			ClosestSmallerPresentDuration = 208333;
			ClosestLargerPresentDuration = 208333;
		};
#endif

		if
		(
			ClosestSmallerPresentDuration == 0 &&
			ClosestLargerPresentDuration == 0
		)
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				WEX::Common::String().Format
				(
					L"This device does not support presentation duration %u.",
					CustomPresentDurationsToTest[ iCurrentCustomPresentDuration ]
				)
			);

			continue;
		};


		UINT DeltaAbove = ClosestLargerPresentDuration - CustomPresentDurationsToTest[ iCurrentCustomPresentDuration ];
		UINT DeltaBelow = CustomPresentDurationsToTest[ iCurrentCustomPresentDuration ] - ClosestSmallerPresentDuration;

		UINT AcceptableDelta = CustomPresentDurationsToTest[ iCurrentCustomPresentDuration ] * CUSTOM_PRESENT_DURATION_SELECTION_TOLERANCE;

		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"CheckPresentDurationSupport reports bracketing durations %u and %u for duration %u.",
				ClosestSmallerPresentDuration,
				ClosestLargerPresentDuration,
				CustomPresentDurationsToTest[ iCurrentCustomPresentDuration ]
			)
		);

		if
		(
			DeltaAbove > AcceptableDelta &&
			DeltaBelow > AcceptableDelta
		)
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				WEX::Common::String().Format
				(
					L"Bracketing durations of %u and %u are not close enough to requested duration of %u for testing.",
					ClosestSmallerPresentDuration,
					ClosestLargerPresentDuration,
					CustomPresentDurationsToTest[ iCurrentCustomPresentDuration ]
				)
			);

			continue;
		};


		UINT CustomPresentDurationToRequest = 0;
		if ( DeltaAbove < DeltaBelow )
		{
			CustomPresentDurationToRequest = ClosestLargerPresentDuration;
		}
		else
		{
			CustomPresentDurationToRequest = ClosestSmallerPresentDuration;
		};


		//
		// Report the duration that we're actually going to verify.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"Verifying precision for custom presentation duration %u.",
				CustomPresentDurationToRequest
			)
		);


		//
		// Verify the precision of the device's support for this
		// custom presentation duration.
		//

		bool bCustomPresentDurationWasSupportedPrecisely = VerifySupportForCustomPresentDuration
		(
			CustomPresentDurationToRequest
		);

		bVerificationsPerformed = true;

		if ( bCustomPresentDurationWasSupportedPrecisely != true )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				WEX::Common::String().Format
				(
					L"Verification of custom presentation duration %u failed.",
					CustomPresentDurationToRequest
				)
			);

			goto Cleanup;
		};


		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"Verification of custom presentation duration %u passed.",
				CustomPresentDurationToRequest
			)
		);
	};


	bOverallResult = true;


Cleanup:
	if ( pSwapChain != nullptr )
	{
		pSwapChain->Release();
		pSwapChain = nullptr;
	};


	//
	// Log how many events the ETW event processing thread
	// processed.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"ETW event processing thread processed %u events.",
			m_EventProcessingThread.m_cEventsProcessed
		)
	);


#if DO_ETW_TRACING
	EndETWTrace();
#else
	m_EventProcessingThread.Cleanup();
#endif


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


///////////////////////////////////////////////////////
// Function:  TestPresentDurationSupport_WoW64
//
// Purpose:
// This method is just a wrapper that gives us a place
// to hang metadata for creating the WoW64 version of
// the job.
///////////////////////////////////////////////////////
void CPresentDurationTest::TestPresentDurationSupport_WoW64()
{
	TestPresentDurationSupport( true );
};


///////////////////////////////////////////////////////
// Function:  TestPresentDurationSupport_Native
//
// Purpose:
// This method is just a wrapper that gives us a place
// to hang metadata for creating the native version of
// the job.
///////////////////////////////////////////////////////
void CPresentDurationTest::TestPresentDurationSupport_Native()
{
	TestPresentDurationSupport( false );
};


///////////////////////////////////////////////////////
// Function:  TestPresentDurationSupport
//
// Purpose:
// Tests whether the display reports support for custom
// presentation durations and verifies that against the
// type of the display and our expectations for that
// type of display.
///////////////////////////////////////////////////////
void CPresentDurationTest::TestPresentDurationSupport
(
	bool bRunUnderWoW64
)
{
	//
	// Initialize locals.
	//

	bool bOverallResult = false;
	bool bVerificationsPerformed = false;


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
		bRunUnderWoW64 == true ? L"Present Duration Conformance:  Custom Duration Support Test (WoW64)" : L"Present Duration Conformance:  Custom Duration Support Test (Native)",
		3,
		TESTCASE_RESULT_SKIPPED
	);


	//
	// Begin a test case in the log.
	//

	m_pLogger->BeginTestCase
	(
		L"TestPresentDurationSupport",
		0xFFFFFFFF,
		nullptr
	);


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


	//
	// Determine the display topology.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Analyzing display topology."
	);

	DISPLAYCONFIG_PATH_INFO PathArrayElements[ 32 ];
	UINT32 cPathArrayElements = ARRAYSIZE( PathArrayElements );
	DISPLAYCONFIG_MODE_INFO ModeArrayElements[ 32 ];
	UINT32 cModeArrayElements = ARRAYSIZE( ModeArrayElements );
	DISPLAYCONFIG_TOPOLOGY_ID CurrentTopologyID;
	LONG QueryDisplayConfigValue = QueryDisplayConfig
	(
		QDC_DATABASE_CURRENT,
		&cPathArrayElements,
		PathArrayElements,
		&cModeArrayElements,
		ModeArrayElements,
		&CurrentTopologyID
	);

	if ( QueryDisplayConfigValue == ERROR_NOT_SUPPORTED )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"QueryDisplayConfigValue returned ERROR_NOT_SUPPORTED, indicating that the system is not running on a WDDM driver."
		);

		bOverallResult = true;
		goto Cleanup;
	};

	if ( QueryDisplayConfigValue != ERROR_SUCCESS )
	{
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			QueryDisplayConfigValue,
			L"QueryDisplayConfig failed."
		);

		goto Cleanup;
	};

	bool bDisplayTopologyIncludesExternalDevices = false;
	for
	(
		UINT32 iCurrentPathArrayElement = 0;
		iCurrentPathArrayElement < cPathArrayElements;
		++iCurrentPathArrayElement
	)
	{
		DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY PathArrayOutputTechnology = PathArrayElements[ iCurrentPathArrayElement ].targetInfo.outputTechnology;
#if defined(UNICODE)
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"Display path %u output technology:  %s.",
				iCurrentPathArrayElement,
				ToString( PathArrayOutputTechnology ).c_str()
			)
		);
#else
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"Display path %u output technology:  %S.",
				iCurrentPathArrayElement,
				ToString( PathArrayOutputTechnology ).c_str()
			)
		);
#endif

		if ( IsIntegratedVideoOutputType( PathArrayOutputTechnology ) != TRUE )
		{
			bDisplayTopologyIncludesExternalDevices = true;
		};
	};


	//
	// Determine whether the display reports support for custom
	// presentation durations.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Querying support for custom presentation duration."
	);

	UINT DesiredPresentDuration = ( 10 * 1000 * 1000 ) / 48;
	UINT ClosestSmallerPresentDuration = 0;
	UINT ClosestLargerPresentDuration = 0;

	assert( m_pSwapChain_IDXGISwapChainMedia != nullptr );
	HRESULT hCheckPresentDurationSupportResult = m_pSwapChain_IDXGISwapChainMedia->CheckPresentDurationSupport
	(
		DesiredPresentDuration,
		&ClosestSmallerPresentDuration,
		&ClosestLargerPresentDuration
	);

	if ( FAILED( hCheckPresentDurationSupportResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCheckPresentDurationSupportResult,
			L"CheckPresentDurationSupport failed."
		);

		goto Cleanup;
	};


	if
	(
		ClosestSmallerPresentDuration != 0 ||
		ClosestLargerPresentDuration != 0
	)
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"CheckPresentDurationSupport reports bracketing durations %u and %u for duration %u.",
				ClosestSmallerPresentDuration,
				ClosestLargerPresentDuration,
				DesiredPresentDuration
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
				L"CheckPresentDurationSupport reports no support for duration %u.",
				DesiredPresentDuration
			)
		);
	};


	//
	// Check whether an external monitor reports support for
	// custom presentation durations.  If any external monitors
	// are connected, we require that the driver/panel NOT
	// report support for custom presentation durations.
	//

	bVerificationsPerformed = true;

	if
	(
		bDisplayTopologyIncludesExternalDevices == true &&
		(
			ClosestSmallerPresentDuration != 0 ||
			ClosestLargerPresentDuration != 0
		)
	)
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Display topology including one or more external devices reports support for custom presentation durations."
		);

		bOverallResult = false;
		goto Cleanup;
	};


/*
	WDDM 1.3 drivers are required to support this feature, but
	display panels are not.  Thus we CANNOT make the following
	assertion:


	//
	// If there are no external devices present and the display
	// driver is a WDDM 1.3 driver, then we require that the
	// driver report support for custom presentation durations.
	//

	if
	(
		m_SysInfo.GetGraphicsDriverModel() >= GRAPHICS_DRIVER_MODEL_WDDM_1_3 &&
		bDisplayTopologyIncludesExternalDevices != true &&
		(
			ClosestSmallerPresentDuration == 0 ||
			ClosestLargerPresentDuration == 0
		)
	)
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Display topology including only integrated devices on WDDM 1.3 does not report support for custom presentation durations."
		);

		bOverallResult = false;
		goto Cleanup;
	};
*/


	bOverallResult = true;


Cleanup:
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


///////////////////////////////////////////////////////
// Function:  MsgProc
//
// Purpose:
// Message handler routine.
//
// Called by:
// StaticMsgProc
///////////////////////////////////////////////////////
LRESULT CPresentDurationTest::MsgProc
(
	__in HWND hWindow,
	UINT Message,
	WPARAM wParam,
	LPARAM lParam
)
{
	//
	// Handle expected messages.
	//

	switch ( Message )
	{
		case WM_CLOSE:
		{
			return 0;
		} break;
	};


	//
	// Do default handling of unrecognized messages.
	//

	return DefWindowProc
	(
		hWindow,
		Message,
		wParam,
		lParam
	);
};


///////////////////////////////////////////////////////
// Function:  StaticMsgProc
//
// Purpose:
// Static message handler routine.
//
// Called by:
// [System]
///////////////////////////////////////////////////////
LRESULT WINAPI CPresentDurationTest::StaticMsgProc
(
	__in HWND hWindow,
	UINT Message,
	WPARAM wParam,
	LPARAM lParam
)
{
	//
	// Stash away our this pointer during handling of
	// the WM_NCCREATE message.
	//

	if ( Message == WM_NCCREATE )
	{
		SetWindowLongPtr
		(
			hWindow,
			GWLP_USERDATA,
			(LONG_PTR) ( (LPCREATESTRUCT) lParam )->lpCreateParams
		);

		return 1;
	};


	//
	// Retrieve our this pointer.
	//

	LONG_PTR pContext = GetWindowLongPtr
	(
		hWindow,
		GWLP_USERDATA
	);


	//
	// Do default handling in the absence of a this pointer.
	//

	if ( pContext == 0 )
	{
		return DefWindowProc
		(
			hWindow,
			Message,
			wParam,
			lParam
		);
	};


	//
	// Cast the pointer that we stashed away previously.
	//

	CPresentDurationTest * pThis = (CPresentDurationTest *) pContext;
	assert( pThis != nullptr );


	//
	// Hand off to the member method.
	//

	return pThis->MsgProc
	(
		hWindow,
		Message,
		wParam,
		lParam
	);
};


///////////////////////////////////////////////////////
// Function:  VerifySupportForCustomPresentDuration
//
// Purpose:
// Exercises the specified custom presentation duration
// and verifies that vertical blank events occur at a
// precise and regular interval.
///////////////////////////////////////////////////////
bool CPresentDurationTest::VerifySupportForCustomPresentDuration
(
	UINT RequestedCustomPresentDuration
)
{
	//
	// Initialize locals.
	//

	bool bOverallResult = false;

	struct EXPECTED_FRAME_RETIREMENT
	{
		UINT ExpectedRetirementCount;
		LARGE_INTEGER ActualRetirementTime;
	};

	typedef std::map< UINT, EXPECTED_FRAME_RETIREMENT > EXPECTED_FRAME_RETIREMENT_MAP;
	EXPECTED_FRAME_RETIREMENT_MAP PresentedFrameMap;

	LONGLONG CumulativeTimeDeltaInQPCTicks = 0;

	ULONG cGlitchesDetectedForCustomDuration = 0;


#if EXPECT_CUSTOM_PRESENT_DURATION_SUPPORT
	//
	// Set the custom presentation duration.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"Calling SetPresentDuration( %u ).",
			RequestedCustomPresentDuration
		)
	);

	HRESULT hSetCustomPresentDurationResult = m_pSwapChain_IDXGISwapChainMedia->SetPresentDuration
	(
		RequestedCustomPresentDuration
	);

	if ( FAILED( hSetCustomPresentDurationResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hSetCustomPresentDurationResult,
			L"SetPresentDuration failed."
		);

		goto Cleanup;
	};
#endif


	//
	// Render and present using the system refresh rate
	// until we get back present statistics suggesting
	// that it's okay to begin using the custom presentation
	// duration.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Rendering and presenting until custom presentation duration is approved."
	);

	bool bCustomPresentDurationWasApproved = false;
	bool bUsedOverlays = false;

	DWORD StartTickCount = GetTickCount();
	UINT iCurrentFrame = 0;

	for
	(
		DWORD CurrentTickCount = StartTickCount;
		CurrentTickCount <= StartTickCount + 5 * 1000;
		CurrentTickCount = GetTickCount(),
			++iCurrentFrame
	)
	{
		//
		// Render and present the frame.
		//

		switch ( m_SwapChainCreationMethod )
		{
			case SWAPCHAIN_CREATION_METHOD_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACE:
			{
				// Determine the next buffer to render to and present.
				UINT BufferToPresent = iCurrentFrame % m_SwapChainBufferArraySize;


				// Clear the buffer.
				// @TODO
				float FrameColors[ 2 ][ 4 ] =
				{
					{ 1.f, 0.f, 0.f, 1.f },
					{ 0.f, 0.f, 1.f, 1.f }
				};


				// Present the frame.
				HRESULT hPresentResult = m_pSwapChain_IDXGIDecodeSwapChain->PresentBuffer
				(
					BufferToPresent,
					1, // @REVIEW:  Sync interval
					0  // @REVIEW:  Flags
				);

				if ( FAILED( hPresentResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"PresentBuffer failed."
					);

					goto Cleanup;
				};

				if ( hPresentResult != S_OK )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"PresentBuffer returned unexpected HRESULT."
					);

					goto Cleanup;
				};
			} break;


			case SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORCOMPOSITIONSURFACE:
			{
				// Determine the next buffer to render to and present.
				UINT BufferToPresent = iCurrentFrame % m_SwapChainBufferArraySize;


				// Clear the buffer.
				// @TODO
				float FrameColors[ 2 ][ 4 ] =
				{
					{ 1.f, 0.f, 0.f, 1.f },
					{ 0.f, 0.f, 1.f, 1.f }
				};


				// Present the frame.
				HRESULT hPresentResult = m_pSwapChain_IDXGISwapChain1->Present
				(
					1, // @REVIEW:  Sync interval
					0  // @REVIEW:  Flags
				);

				if ( FAILED( hPresentResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"PresentEx failed."
					);

					goto Cleanup;
				};

				if ( hPresentResult != S_OK )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"PresentEx returned unexpected HRESULT."
					);

					goto Cleanup;
				};
			} break;


			case SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORHWND:
			{
				// Present the frame.
				DXGI_PRESENT_PARAMETERS PresentParameters;
				ZeroMemory( &PresentParameters, sizeof( PresentParameters ) );

				HRESULT hPresentResult = m_pSwapChain_IDXGISwapChain1->Present1
				(
					1, // @REVIEW:  Sync interval
					0, // @REVIEW:  Flags
					&PresentParameters
				);

				if ( FAILED( hPresentResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"Present failed."
					);

					goto Cleanup;
				};

				if ( hPresentResult != S_OK )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"Present returned unexpected HRESULT."
					);

					goto Cleanup;
				};
			} break;


			default:
			{
				assert( false && L"Unhandled swap chain type." );
			} break;
		};


		//
		// Get frame statistics.
		//

		DXGI_FRAME_STATISTICS_MEDIA FrameStatistics1;
		ZeroMemory( &FrameStatistics1, sizeof( FrameStatistics1 ) );
		HRESULT hGetFrameStatisticsMediaResult = m_pSwapChain_IDXGISwapChainMedia->GetFrameStatisticsMedia
		(
			&FrameStatistics1
		);

		if ( iCurrentFrame == 0 && hGetFrameStatisticsMediaResult == DXGI_ERROR_FRAME_STATISTICS_DISJOINT )
		{
			// This is expected for the first frame in a sequence.
		}
		else if ( FAILED( hGetFrameStatisticsMediaResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hGetFrameStatisticsMediaResult,
				L"GetFrameStatisticsMedia failed."
			);

			goto Cleanup;
		};


		//
		// Note whether we used an overlay.
		//

		if ( FrameStatistics1.CompositionMode == DXGI_FRAME_PRESENTATION_MODE_OVERLAY )
		{
			bUsedOverlays = true;
		};


#if EXPECT_CUSTOM_PRESENT_DURATION_SUPPORT
#else
		// Workaround:  Pretend that 48 Hz is supported.
		FrameStatistics1.ApprovedPresentDuration = 208333;
#endif


		//
		// Check whether the custom presentation duration would
		// have been honored.
		//

		if ( FrameStatistics1.ApprovedPresentDuration == 0 )
		{
			continue;
		};

		if ( FrameStatistics1.ApprovedPresentDuration == RequestedCustomPresentDuration )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				WEX::Common::String().Format
				(
					L"Frame statistics indicate exact support for custom presentation duration %u.",
					RequestedCustomPresentDuration
				)
			);

			bCustomPresentDurationWasApproved = true;
			break;
		};

		if ( iCurrentFrame < 5 )
		{
			continue;
		}

		UINT Delta = 0;
		if ( FrameStatistics1.ApprovedPresentDuration > RequestedCustomPresentDuration )
		{
			Delta = FrameStatistics1.ApprovedPresentDuration - RequestedCustomPresentDuration;
		}
		else
		{
			Delta = RequestedCustomPresentDuration - FrameStatistics1.ApprovedPresentDuration;
		};

		UINT MaximumAcceptableDelta = RequestedCustomPresentDuration * CUSTOM_PRESENT_DURATION_SELECTION_TOLERANCE;
		if ( Delta > MaximumAcceptableDelta )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				WEX::Common::String().Format
				(
					L"Frame statistics indicate support for custom presentation duration %u, which does not match requested duration %u.",
					FrameStatistics1.ApprovedPresentDuration,
					RequestedCustomPresentDuration
				)
			);

			continue;
		};

		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"Frame statistics indicate support for custom present duration %u, which is close enough to requested duration %u.",
				FrameStatistics1.ApprovedPresentDuration,
				RequestedCustomPresentDuration
			)
		);

		bCustomPresentDurationWasApproved = true;
		break;
	};


#if EXPECT_CUSTOM_PRESENT_DURATION_SUPPORT
	//
	// Error out if we never received approval to use the custom
	// presentation duration.
	//

	if ( bCustomPresentDurationWasApproved != true )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			WEX::Common::String().Format
			(
				L"After %u frames, custom presentation duration was never approved.",
				iCurrentFrame - 1
			)
		);

		goto Cleanup;
	};
#endif


	//
	// Determine the frame latency for this device.
	//

	ULONG MaxFrameLatency = 0;
	switch ( m_SwapChainCreationMethod )
	{
		case SWAPCHAIN_CREATION_METHOD_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACE:
		{
			MaxFrameLatency = 1;
		} break;


		case SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORCOMPOSITIONSURFACE:
		case SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORHWND:
		{
			MaxFrameLatency = 3;
		} break;


		default:
		{
			assert( false && L"Unhandled swap chain type." );
		} break;
	};


	//
	// Query initial presentation state.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Querying initial swap chain presentation counter and performance counter values."
	);


	// Determine the current present frame ID.
	UINT MostRecentPresentCount = 0;
	HRESULT hGetLastPresentCountResult = m_pSwapChain_IDXGISwapChain1->GetLastPresentCount
	(
		&MostRecentPresentCount
	);

	if ( FAILED( hGetLastPresentCountResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hGetLastPresentCountResult,
			L"GetLastPresentCount failed."
		);

		goto Cleanup;
	};


	// Get frame statistics.
	DXGI_FRAME_STATISTICS_MEDIA FrameStatistics1;
	ZeroMemory( &FrameStatistics1, sizeof( FrameStatistics1 ) );
	HRESULT hGetInitialFrameStatisticsResult = m_pSwapChain_IDXGISwapChainMedia->GetFrameStatisticsMedia
	(
		&FrameStatistics1
	);

	if ( hGetInitialFrameStatisticsResult == DXGI_ERROR_FRAME_STATISTICS_DISJOINT )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"Frame statistics were reset for frame %u.",
				iCurrentFrame
			)
		);
	}
	else if ( FAILED( hGetInitialFrameStatisticsResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hGetInitialFrameStatisticsResult,
			L"GetFrameStatisticsMedia failed."
		);
	};


	// Determine the vsync at which the most recent frame was
	// retired.
	UINT MostRecentPresentRefreshCount = FrameStatistics1.PresentRefreshCount;


	// Determine the performance counter tick count at which the
	// most recent frame was retired.
	LARGE_INTEGER MostRecentlyRetiredPresentPerformanceCounter;
	MostRecentlyRetiredPresentPerformanceCounter.QuadPart = FrameStatistics1.SyncQPCTime.QuadPart;


	// Query the performance counter tick frequency.
	LARGE_INTEGER PerformanceCounterFrequency;
	BOOL bQueryPerformanceFrequencyResult = QueryPerformanceFrequency( &PerformanceCounterFrequency );

	if ( bQueryPerformanceFrequencyResult != TRUE )
	{
		DWORD QueryPerformanceFrequencyError = GetLastError();

		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			QueryPerformanceFrequencyError,
			L"QueryPerformanceFrequency failed."
		);

		goto Cleanup;
	};


	// Start out assuming that no glitches have been detected.
	UINT GlitchRecoveryFrames = 0;


	bOverallResult = true;


	//
	// Render and present with the custom presentation duration
	// for several seconds or until we lose approval to use the
	// custom duration.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Rendering with custom presentation duration."
	);


	DWORD TimeToPresent = 10;
	StartTickCount = GetTickCount();
	iCurrentFrame = 0;
	UINT cFramesPresented = 0;
	UINT cFramesProcessed = 0;
	for
	(
		DWORD CurrentTickCount = StartTickCount;
		CurrentTickCount <= StartTickCount + ( TimeToPresent + 2 ) * 1000;
		CurrentTickCount = GetTickCount(),
			++iCurrentFrame
	)
	{
		//
		// Get the current QPC tick.
		//

		LARGE_INTEGER CurrentQPCTick;
		BOOL bQueryPerformanceCounterResult = QueryPerformanceCounter
		(
			&CurrentQPCTick
		);

		if ( bQueryPerformanceCounterResult != TRUE )
		{
			LOG_ERROR
			(
				ERROR_TYPE_WIN32,
				GetLastError(),
				L"QueryPerformanceCounter failed."
			);

			bOverallResult = false;
			goto Cleanup;
		};


		//
		// Render and present the frame.
		//

		switch ( m_SwapChainCreationMethod )
		{
			case SWAPCHAIN_CREATION_METHOD_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACE:
			{
				// Determine the next buffer to render to and present.
				UINT BufferToPresent = iCurrentFrame % m_SwapChainBufferArraySize;


				// Clear the buffer.
				// @TODO
				float FrameColors[ 2 ][ 4 ] =
				{
					{ 1.f, 0.f, 0.f, 1.f },
					{ 0.f, 0.f, 1.f, 1.f }
				};


				// Present the frame.
				HRESULT hPresentResult = E_FAIL;
				if ( GlitchRecoveryFrames > 0 )
				{
					LOG_MESSAGE
					(
						MESSAGE_TYPE_INFORMATIONAL,
						WEX::Common::String().Format
						(
							L"Doing glitch recovery on frame %u (ID %u).",
							iCurrentFrame,
							MostRecentPresentCount
						)
					);

					hPresentResult = m_pSwapChain_IDXGIDecodeSwapChain->PresentBuffer
					(
						BufferToPresent,
						0, // @REVIEW:  Sync interval
						DXGI_PRESENT_USE_DURATION // @REVIEW:  Flags
					);

					--GlitchRecoveryFrames;
				}
				else
				{
					hPresentResult = m_pSwapChain_IDXGIDecodeSwapChain->PresentBuffer
					(
						BufferToPresent,
						1, // @REVIEW:  Sync interval
						DXGI_PRESENT_USE_DURATION // @REVIEW:  Flags
					);
				};

				if ( FAILED( hPresentResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"PresentBuffer failed."
					);

					bOverallResult = false;
					goto Cleanup;
				};

				if ( hPresentResult != S_OK )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"PresentEx returned unexpected HRESULT."
					);

					bOverallResult = false;
					goto Cleanup;
				};
			} break;


			case SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORCOMPOSITIONSURFACE:
			{
				// Determine the next buffer to render to and present.
				UINT BufferToPresent = iCurrentFrame % m_SwapChainBufferArraySize;


				// Clear the buffer.
				// @TODO
				float FrameColors[ 2 ][ 4 ] =
				{
					{ 1.f, 0.f, 0.f, 1.f },
					{ 0.f, 0.f, 1.f, 1.f }
				};


				// Present the frame.
				HRESULT hPresentResult = m_pSwapChain_IDXGISwapChain1->Present
				(
					1, // @REVIEW:  Sync interval
					DXGI_PRESENT_USE_DURATION // @REVIEW:  Flags
				);

				if ( FAILED( hPresentResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"PresentEx failed."
					);

					bOverallResult = false;
					goto Cleanup;
				};

				if ( hPresentResult != S_OK )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"PresentEx returned unexpected HRESULT."
					);

					bOverallResult = false;
					goto Cleanup;
				};
			} break;


			case SWAPCHAIN_CREATION_METHOD_CREATESWAPCHAINFORHWND:
			{
				// Present the frame.
				HRESULT hPresentResult = E_FAIL;
				if ( GlitchRecoveryFrames > 0 )
				{
					LOG_MESSAGE
					(
						MESSAGE_TYPE_INFORMATIONAL,
						WEX::Common::String().Format
						(
							L"Doing glitch recovery on frame %u (ID %u).",
							iCurrentFrame,
							MostRecentPresentCount
						)
					);

					hPresentResult = m_pSwapChain_IDXGISwapChain1->Present
					(
						0, // @REVIEW:  Sync interval
						0 // @REVIEW:  Flags
					);

					--GlitchRecoveryFrames;
				}
				else
				{
					hPresentResult = m_pSwapChain_IDXGISwapChain1->Present
					(
						1, // @REVIEW:  Sync interval
						0 // @REVIEW:  Flags
					);
				};

				if ( FAILED( hPresentResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"Present failed."
					);

					bOverallResult = false;
					goto Cleanup;
				};

				if ( hPresentResult != S_OK )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"Present returned unexpected HRESULT."
					);

					bOverallResult = false;
					goto Cleanup;
				};
			} break;


			default:
			{
				assert( false && L"Unhandled swap chain type." );
			} break;
		};


		//
		// Add a presented frame entry to our map.
		//

		// If we're in the main part of our loop...
		if
		(
			GetTickCount() <= StartTickCount + TimeToPresent * 1000
		)
		{
			++cFramesPresented;

			// ...then add a presented frame entry for this frame to
			// our presented frame entry map.
#if EXPECT_CUSTOM_PRESENT_DURATION_SUPPORT
			LONGLONG CustomPresentDurationInQPCTicks = (LONGLONG) RequestedCustomPresentDuration * PerformanceCounterFrequency.QuadPart / ( 10 * 1000 * 1000 );
#else
			LONGLONG CustomPresentDurationInQPCTicks = (LONGLONG) 166666 * PerformanceCounterFrequency.QuadPart / ( 10 * 1000 * 1000 );
#endif

			EXPECTED_FRAME_RETIREMENT ExpectedFrameRetirement;
			ExpectedFrameRetirement.ExpectedRetirementCount = MostRecentPresentRefreshCount + MaxFrameLatency + 1;

			PresentedFrameMap[ MostRecentPresentCount ] = ExpectedFrameRetirement;
		};


		//
		// Get frame statistics.
		//

		ZeroMemory( &FrameStatistics1, sizeof( FrameStatistics1 ) );
		HRESULT hGetFrameStatisticsMediaResult = m_pSwapChain_IDXGISwapChainMedia->GetFrameStatisticsMedia
		(
			&FrameStatistics1
		);

		if ( hGetFrameStatisticsMediaResult == DXGI_ERROR_FRAME_STATISTICS_DISJOINT )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				WEX::Common::String().Format
				(
					L"Frame statistics were reset for frame %u.",
					iCurrentFrame
				)
			);
		}
		else if ( FAILED( hGetFrameStatisticsMediaResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hGetFrameStatisticsMediaResult,
				L"GetFrameStatisticsMedia failed."
			);

			bOverallResult = false;
			goto Cleanup;
		};


#if EXPECT_CUSTOM_PRESENT_DURATION_SUPPORT
		// Check whether the custom presentation duration was
		// honored.
		if ( FrameStatistics1.ApprovedPresentDuration == 0 )
		{
			LOG_ERROR
			(
				ERROR_TYPE_BOOL,
				false,
				WEX::Common::String().Format
				(
					L"On frame %u, %u seconds into rendering, frame statistics indicate that the custom presentation duration was not honored.",
					iCurrentFrame,
					( GetTickCount() - StartTickCount ) / 1000
				)
			);

			bOverallResult = false;
			goto Cleanup;
		};

		if ( FrameStatistics1.ApprovedPresentDuration != RequestedCustomPresentDuration )
		{
			UINT Delta = 0;
			if ( FrameStatistics1.ApprovedPresentDuration > RequestedCustomPresentDuration )
			{
				Delta = FrameStatistics1.ApprovedPresentDuration - RequestedCustomPresentDuration;
			}
			else
			{
				Delta = RequestedCustomPresentDuration - FrameStatistics1.ApprovedPresentDuration;
			};

			UINT MaximumAcceptableDelta = RequestedCustomPresentDuration * CUSTOM_PRESENT_DURATION_SELECTION_TOLERANCE;
			if ( Delta > MaximumAcceptableDelta )
			{
				LOG_ERROR
				(
					ERROR_TYPE_BOOL,
					false,
					WEX::Common::String().Format
					(
						L"On frame %u, %u seconds into rendering, frame statistics indicate support for custom presentation duration %u, which does not match requested duration %u.",
						iCurrentFrame,
						( GetTickCount() - StartTickCount ) / 1000,
						FrameStatistics1.ApprovedPresentDuration,
						RequestedCustomPresentDuration
					)
				);

				bOverallResult = false;
				goto Cleanup;
			};


			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				WEX::Common::String().Format
				(
					L"On frame %u, %u seconds into rendering, frame statistics indicate support for custom present duration %u, which is close enough to requested duration %u.",
					iCurrentFrame,
					( GetTickCount() - StartTickCount ) / 1000,
					FrameStatistics1.ApprovedPresentDuration,
					RequestedCustomPresentDuration
				)
			);
		};
#endif

		// Update the vsync at which the most recent frame was
		// retired.
		MostRecentPresentRefreshCount = FrameStatistics1.PresentRefreshCount;


		// Update the performance counter tick count at which the
		// most recent frame was retired.
		MostRecentlyRetiredPresentPerformanceCounter.QuadPart = FrameStatistics1.SyncQPCTime.QuadPart;


		// Update the current Present frame ID.
		hGetLastPresentCountResult = m_pSwapChain_IDXGISwapChain1->GetLastPresentCount
		(
			&MostRecentPresentCount
		);

		if ( FAILED( hGetLastPresentCountResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hGetLastPresentCountResult,
				L"GetLastPresentCount failed."
			);

			bOverallResult = false;
			goto Cleanup;
		};


		// Look for Present frame retirement expectations in our map.
		EXPECTED_FRAME_RETIREMENT_MAP::iterator CurrentFrameIterator = PresentedFrameMap.find
		(
			FrameStatistics1.PresentCount
		);

		if ( CurrentFrameIterator != PresentedFrameMap.end() )
		{
			// Update the actual retirement time for this frame.
			CurrentFrameIterator->second.ActualRetirementTime = FrameStatistics1.SyncQPCTime;

			// Check for glitches.
			ULONG ExpectedRetirementCount = CurrentFrameIterator->second.ExpectedRetirementCount;

			if ( FrameStatistics1.PresentRefreshCount > ExpectedRetirementCount )
			{
				GlitchRecoveryFrames += FrameStatistics1.PresentRefreshCount - ExpectedRetirementCount;

				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					WEX::Common::String().Format
					(
						L"Glitch detected.  Presented frame with ordinal %u was expected to be retired at vsync %u but was actually retired at vsync %u.",
						FrameStatistics1.PresentCount,
						ExpectedRetirementCount,
						FrameStatistics1.PresentRefreshCount
					)
				);

				++cGlitchesDetectedForCustomDuration;
			};


			EXPECTED_FRAME_RETIREMENT_MAP::iterator PreviousFrameIterator = PresentedFrameMap.find
			(
				FrameStatistics1.PresentCount - 1
			);

			if ( PreviousFrameIterator != PresentedFrameMap.end() )
			{
#if EXPECT_CUSTOM_PRESENT_DURATION_SUPPORT
				LONGLONG CustomPresentDurationInQPCTicks = (LONGLONG) RequestedCustomPresentDuration * PerformanceCounterFrequency.QuadPart / ( 10 * 1000 * 1000 );
#else
				LONGLONG CustomPresentDurationInQPCTicks = (LONGLONG) 166666 * PerformanceCounterFrequency.QuadPart / ( 10 * 1000 * 1000 );
#endif

				LARGE_INTEGER CurrentFrameExpectedRetirementTime;
				CurrentFrameExpectedRetirementTime.QuadPart = PreviousFrameIterator->second.ActualRetirementTime.QuadPart + CustomPresentDurationInQPCTicks;

				PresentedFrameMap.erase( PreviousFrameIterator );

				// Check for per-frame timing irregularities.
				LONGLONG TimeDeltaInQPCTicks = FrameStatistics1.SyncQPCTime.QuadPart - CurrentFrameExpectedRetirementTime.QuadPart;

				CumulativeTimeDeltaInQPCTicks += TimeDeltaInQPCTicks;

				LONGLONG MaxAcceptableTimeDeltaInQPCTicks = CustomPresentDurationInQPCTicks * CUSTOM_PRESENT_DURATION_PRECISION_PER_FRAME_TOLERANCE + 1;

#if VERIFY_PER_FRAME_PRECISION
				if
				(
					TimeDeltaInQPCTicks > MaxAcceptableTimeDeltaInQPCTicks ||
					(-1) * TimeDeltaInQPCTicks > MaxAcceptableTimeDeltaInQPCTicks
				)
				{
					LOG_ERROR
					(
						ERROR_TYPE_BOOL,
						false,
						L"Timing irregularity detected."
					);

					LOG_MESSAGE
					(
						MESSAGE_TYPE_INFORMATIONAL,
						WEX::Common::String().Format
						(
							L"Presented frame with ordinal %u was expected to be retired at performance counter tick %I64d but was actually retired at tick %I64d, a difference of %I64d ticks at %I64d ticks per second.",
							FrameStatistics1.PresentCount,
							CurrentFrameExpectedRetirementTime.QuadPart,
							FrameStatistics1.SyncQPCTime.QuadPart,
							TimeDeltaInQPCTicks,
							PerformanceCounterFrequency.QuadPart
						)
					);

					bOverallResult = false;
					//goto Cleanup;
				}
				else
				{
					LOG_MESSAGE
					(
						MESSAGE_TYPE_INFORMATIONAL,
						WEX::Common::String().Format
						(
							L"Presented frame with ordinal %u was expected to be retired at performance counter tick %I64d and was actually retired at tick %I64d, a difference of %I64d ticks at %I64d ticks per second.",
							FrameStatistics1.PresentCount,
							CurrentFrameExpectedRetirementTime.QuadPart,
							FrameStatistics1.SyncQPCTime.QuadPart,
							TimeDeltaInQPCTicks,
							PerformanceCounterFrequency.QuadPart
						)
					);
				};
#endif

				++cFramesProcessed;
			};
		};
	};


#if VERIFY_GLITCH_COUNT
	//
	// Verify that there were no more than an acceptable number
	// of detected glitches.
	//

	if ( cGlitchesDetectedForCustomDuration > MaxAcceptableGlitchCount )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Unacceptable number of glitches detected."
		);

		bOverallResult = false;
	};
#endif


	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"Detected %u glitch(es) for custom presentation duration %u.",
			cGlitchesDetectedForCustomDuration,
			RequestedCustomPresentDuration
		)
	);


	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"Verification of custom presentation duration %u is complete.",
			RequestedCustomPresentDuration
		)
	);


	//
	// Make note of any frames left in the map.  We expect that
	// there will be one frame left at the end, because the
	// algorithm uses the actual retirement time from the prior
	// frame to calculate the expected retirement time for the
	// frame being processed, and only removes the prior frame.
	// Thus there is no frame after the last one to come along
	// and remove the last frame from the map.
	//

	EXPECTED_FRAME_RETIREMENT_MAP::size_type cMissingFrames = PresentedFrameMap.size();
	if ( cMissingFrames > 1 )
	{
		for
		(
			EXPECTED_FRAME_RETIREMENT_MAP::iterator iterator = PresentedFrameMap.begin();
			iterator != PresentedFrameMap.end();
			++iterator
		)
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				WEX::Common::String().Format
				(
					L"Frame statistics were left over for frame with ordinal %u.",
					iterator->first
				)
			);
		};
	};

	PresentedFrameMap.clear();


#if VERIFY_AVERAGE_PRECISION
	//
	// Analyze the cumulative error.
	//

	if ( iCurrentFrame > cMissingFrames )
	{
		double AverageTimeDeltaInQPCTicks = CumulativeTimeDeltaInQPCTicks / (double) ( cFramesProcessed );

#if EXPECT_CUSTOM_PRESENT_DURATION_SUPPORT
		LONGLONG CustomPresentDurationInQPCTicks = (LONGLONG) RequestedCustomPresentDuration * PerformanceCounterFrequency.QuadPart / ( 10 * 1000 * 1000 );
#else
		LONGLONG CustomPresentDurationInQPCTicks = (LONGLONG) 166666 * PerformanceCounterFrequency.QuadPart / ( 10 * 1000 * 1000 );
#endif

		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"Custom present duration is equivalent to %I64i QPC ticks.",
				CustomPresentDurationInQPCTicks
			)
		);

		LONGLONG MaxAcceptableTimeDeltaInQPCTicks = CustomPresentDurationInQPCTicks * CUSTOM_PRESENT_DURATION_PRECISION_AVERAGE_TOLERANCE + 1;

		if
		(
			AverageTimeDeltaInQPCTicks > MaxAcceptableTimeDeltaInQPCTicks ||
			AverageTimeDeltaInQPCTicks * (-1.) > MaxAcceptableTimeDeltaInQPCTicks
		)
		{
			LOG_ERROR
			(
				ERROR_TYPE_BOOL,
				false,
				L"Average frame timing was not within acceptable tolerance."
			);

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				WEX::Common::String().Format
				(
					L"On average over %u seconds (with %u frames presented and %u frames' worth of statistics analyzed), frames were retired %2.2f performance counter ticks %s when they were expected (at a performance counter interval of %I64d ticks per second), which is outside of the maximum tolerance of %I64d ticks per frame.",
					TimeToPresent,
					cFramesPresented,
					cFramesProcessed,
					( AverageTimeDeltaInQPCTicks < 0. ) ? ( AverageTimeDeltaInQPCTicks * -1. ) : AverageTimeDeltaInQPCTicks,
					( AverageTimeDeltaInQPCTicks < 0. ) ? L"before" : L"after",
					PerformanceCounterFrequency.QuadPart,
					MaxAcceptableTimeDeltaInQPCTicks
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
					L"On average over %u seconds (with %u frames presented and %u frames' worth of statistics analyzed), frames were retired %2.2f performance counter ticks %s when they were expected (at a performance counter interval of %I64d ticks per second), which is within the maximum tolerance of %I64d ticks per frame.",
					TimeToPresent,
					cFramesPresented,
					cFramesProcessed,
					( AverageTimeDeltaInQPCTicks < 0. ) ? ( AverageTimeDeltaInQPCTicks * -1. ) : AverageTimeDeltaInQPCTicks,
					( AverageTimeDeltaInQPCTicks < 0. ) ? L"before" : L"after",
					PerformanceCounterFrequency.QuadPart,
					MaxAcceptableTimeDeltaInQPCTicks
				)
			);
		};
	};
#endif


Cleanup:
	//
	// Log whether we used overlays.
	//

	if
	(
		bUsedOverlays == true
	)
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"The test used overlays."
		);
	}
	else
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"The test did not use overlays."
		);
	};


#if DO_ETW_TRACING
	//
	// Log whether we achieved independent flip.
	//

	if ( m_EventProcessingThread.m_bIndependentFlipWasAchieved == true )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"The test achieved independent flip."
		);
	}
	else
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"The test did not achieve independent flip."
		);
	};
#endif


	return bOverallResult;
};


///////////////////////////////////////////////////////
// Function:  BeginETWTrace
//
// Purpose:
// Begins an ETW trace for DXGI events.
///////////////////////////////////////////////////////
HRESULT CPresentDurationTest::BeginETWTrace()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	size_t nEventTracingProperties = sizeof( EVENT_TRACE_PROPERTIES ) + 2048;
	EVENT_TRACE_PROPERTIES * pEventTracingProperties = nullptr;

	EVENT_TRACE_LOGFILE EventTraceLogFile;
	ZeroMemory( &EventTraceLogFile, sizeof( EventTraceLogFile ) );


	//
	// Delete the ETW trace log file if it exists.
	//

	BOOL bDeleteETWFileResult = DeleteFileW( m_EventProcessingThread.m_LogFileName );
	if ( bDeleteETWFileResult != TRUE )
	{
		DWORD DeleteETWFileError = GetLastError();

		if ( DeleteETWFileError != ERROR_FILE_NOT_FOUND )
		{
			LOG_ERROR
			(
				ERROR_TYPE_WIN32,
				DeleteETWFileError,
				WEX::Common::String().Format
				(
					L"ETW trace file %s could not be deleted.",
					m_EventProcessingThread.m_LogFileName
				)
			);

			hOverallResult = HRESULT_FROM_WIN32( DeleteETWFileError );
			goto Cleanup;
		};
	};


	//
	// Allocate event tracing properties.
	//

	pEventTracingProperties = reinterpret_cast< EVENT_TRACE_PROPERTIES * > ( new ( std::nothrow ) BYTE[ nEventTracingProperties ] );
	if ( pEventTracingProperties == nullptr )
	{
		hOverallResult = E_OUTOFMEMORY;

		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hOverallResult,
			L"Allocation of buffer for EVENT_TRACE_PROPERTIES failed."
		);

		goto Cleanup;
	};


	//
	// Initialize event tracing properties for a real-time session.
	//

	ZeroMemory( pEventTracingProperties, nEventTracingProperties );

	pEventTracingProperties->Wnode.BufferSize = (ULONG) nEventTracingProperties;
	//pEventTracingProperties->Wnode.Guid = SystemTraceControlGuid;
	pEventTracingProperties->Wnode.ClientContext = 2; // Retrieve event times from TimerResolution
	pEventTracingProperties->Wnode.Flags = WNODE_FLAG_TRACED_GUID;
	pEventTracingProperties->BufferSize = 1024;
	pEventTracingProperties->MinimumBuffers = 200;
	pEventTracingProperties->MaximumBuffers = 220;
	pEventTracingProperties->MaximumFileSize = 128;
	pEventTracingProperties->LogFileMode = EVENT_TRACE_REAL_TIME_MODE | EVENT_TRACE_FILE_MODE_CIRCULAR;
	pEventTracingProperties->LogFileNameOffset = sizeof( EVENT_TRACE_PROPERTIES ) + ARRAYSIZE( m_EventProcessingThread.m_SessionName );
	pEventTracingProperties->FlushTimer = 1;
	pEventTracingProperties->EnableFlags = 0;
	pEventTracingProperties->LoggerNameOffset = sizeof( EVENT_TRACE_PROPERTIES );

	StringCchCopyW
	(
		reinterpret_cast< PWSTR >( reinterpret_cast< BYTE * >( pEventTracingProperties ) + pEventTracingProperties->LoggerNameOffset ),
		ARRAYSIZE( m_EventProcessingThread.m_SessionName ),
		m_EventProcessingThread.m_SessionName
	);

	StringCchCopyW
	(
		reinterpret_cast< PWSTR >( reinterpret_cast< BYTE * >( pEventTracingProperties ) + pEventTracingProperties->LogFileNameOffset ),
		ARRAYSIZE( m_EventProcessingThread.m_LogFileName ),
		m_EventProcessingThread.m_LogFileName
	);


	//
	// Start the ETW trace.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Starting ETW trace."
	);

	ULONG StartTraceResult = StartTraceW
	(
		&m_hEventTracingSession,
		m_EventProcessingThread.m_SessionName,
		pEventTracingProperties
	);

	if ( StartTraceResult != ERROR_SUCCESS )
	{
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			StartTraceResult,
			L"StartTrace failed."
		);

		hOverallResult = HRESULT_FROM_WIN32( StartTraceResult );
		goto Cleanup;
	};

	assert( m_hEventTracingSession != 0 );


	//
	// Initialize the ETW event processing thread.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Initializaing ETW event processing thread."
	);

	HRESULT hInitializeEventProcessingThreadResult = m_EventProcessingThread.Initialize();

	if ( FAILED( hInitializeEventProcessingThreadResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hInitializeEventProcessingThreadResult,
			L"Initialize on ETW event processing thread failed."
		);

		hOverallResult = hInitializeEventProcessingThreadResult;
		goto Cleanup;
	};


	//
	// Enable the providers of interest.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Enabling DXGI ETW event provider."
	);

	ULONG EnableDXGITracingResult = EnableTrace
	(
		TRUE,
		0,
		TRACE_LEVEL_INFORMATION,
		&DXGIEtwProviderGuid,
		m_hEventTracingSession
	);

	if ( EnableDXGITracingResult != ERROR_SUCCESS )
	{
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			EnableDXGITracingResult,
			L"EnableTrace on DXGI ETW provider GUID failed."
		);

		hOverallResult = HRESULT_FROM_WIN32( EnableDXGITracingResult );
		goto Cleanup;
	};


	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Enabling DXGKrnl ETW event provider."
	);

	ULONG EnableDXGKTracingResult = EnableTrace
	(
		TRUE,
		0,
		TRACE_LEVEL_INFORMATION,
		&DxgkControlGuid,
		m_hEventTracingSession
	);

	if ( EnableDXGKTracingResult != ERROR_SUCCESS )
	{
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			EnableDXGKTracingResult,
			L"EnableTrace on DXGKrnl ETW provider GUID failed."
		);

		hOverallResult = HRESULT_FROM_WIN32( EnableDXGKTracingResult );
		goto Cleanup;
	};


/*
	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Enabling DirectComposition ETW event provider."
	);

	ULONG EnableDirectCompositionTracingResult = EnableTrace
	(
		TRUE,
		0,
		TRACE_LEVEL_INFORMATION,
		&Microsoft_Windows_DirectComposition_Provider,
		m_hEventTracingSession
	);

	if ( EnableDirectCompositionTracingResult != ERROR_SUCCESS )
	{
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			EnableDirectCompositionTracingResult,
			L"EnableTrace on DirectComposition provider GUID failed."
		);

		hOverallResult = HRESULT_FROM_WIN32( EnableDirectCompositionTracingResult );
		goto Cleanup;
	};
*/


	//
	// Wait for the ETW trace to get up and running.
	//

	Sleep( 1000 );


	//
	// Have the ETW event processing thread start processing events.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Launching ETW event processing thread."
	);

	DWORD RunEventCountingThreadResult = m_EventProcessingThread.RunOnOriginalThread
	(
		&CCustomPresentDurationEventProcessingThread::ThreadProc
	);

	if ( RunEventCountingThreadResult != 0 )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"RunOnOriginalThread failed."
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};


	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"ETW tracing/processing started successfully."
	);

	hOverallResult = S_OK;


Cleanup:
	//
	// Delete buffers.
	//

	if ( pEventTracingProperties != nullptr )
	{
		delete [] reinterpret_cast< BYTE * >( pEventTracingProperties );
		pEventTracingProperties = nullptr;
	};


	if ( FAILED( hOverallResult ) )
	{
		EndETWTrace();
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  EndETWTrace
//
// Purpose:
// Ends the current ETW trace.
///////////////////////////////////////////////////////
HRESULT CPresentDurationTest::EndETWTrace()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	size_t nEventTracingProperties = sizeof( EVENT_TRACE_PROPERTIES ) + 2048;
	EVENT_TRACE_PROPERTIES * pEventTracingProperties = nullptr;

	EVENT_TRACE_LOGFILE EventTraceLogFile;
	ZeroMemory( &EventTraceLogFile, sizeof( EventTraceLogFile ) );


	//
	// Allocate event tracing properties.
	//

	pEventTracingProperties = reinterpret_cast< EVENT_TRACE_PROPERTIES * > ( new ( std::nothrow ) BYTE[ nEventTracingProperties ] );
	if ( pEventTracingProperties == nullptr )
	{
		hOverallResult = E_OUTOFMEMORY;

		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hOverallResult,
			L"Allocation of buffer for EVENT_TRACE_PROPERTIES failed."
		);

		goto Cleanup;
	};


	//
	// Initialize event tracing properties for a real-time session.
	//

	ZeroMemory( pEventTracingProperties, nEventTracingProperties );

	pEventTracingProperties->Wnode.BufferSize = (ULONG) nEventTracingProperties;
	//pEventTracingProperties->Wnode.Guid = SystemTraceControlGuid;
	pEventTracingProperties->Wnode.ClientContext = 2; // Retrieve event times from TimerResolution
	pEventTracingProperties->Wnode.Flags = WNODE_FLAG_TRACED_GUID;
	pEventTracingProperties->BufferSize = 1024;
	pEventTracingProperties->MinimumBuffers = 200;
	pEventTracingProperties->MaximumBuffers = 220;
	pEventTracingProperties->MaximumFileSize = 128;
	pEventTracingProperties->LogFileMode = EVENT_TRACE_REAL_TIME_MODE | EVENT_TRACE_FILE_MODE_CIRCULAR;
	pEventTracingProperties->LogFileNameOffset = sizeof( EVENT_TRACE_PROPERTIES ) + ARRAYSIZE( m_EventProcessingThread.m_SessionName );
	pEventTracingProperties->FlushTimer = 1;
	pEventTracingProperties->EnableFlags = 0;
	pEventTracingProperties->LoggerNameOffset = sizeof( EVENT_TRACE_PROPERTIES );

	StringCchCopyW
	(
		reinterpret_cast< PWSTR >( reinterpret_cast< BYTE * >( pEventTracingProperties ) + pEventTracingProperties->LoggerNameOffset ),
		ARRAYSIZE( m_EventProcessingThread.m_SessionName ),
		m_EventProcessingThread.m_SessionName
	);

	StringCchCopyW
	(
		reinterpret_cast< PWSTR >( reinterpret_cast< BYTE * >( pEventTracingProperties ) + pEventTracingProperties->LogFileNameOffset ),
		ARRAYSIZE( m_EventProcessingThread.m_LogFileName ),
		m_EventProcessingThread.m_LogFileName
	);


	//
	// Stop the ETW trace.
	//

	if ( m_hEventTracingSession != 0 )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"Stopping ETW event trace."
		);

		ULONG StopTraceResult = ControlTraceW
		(
			m_hEventTracingSession,
			m_EventProcessingThread.m_SessionName,
			pEventTracingProperties,
			EVENT_TRACE_CONTROL_STOP
		);

		if
		(
			StopTraceResult != ERROR_SUCCESS &&
			StopTraceResult != ERROR_MORE_DATA
		)
		{
			LOG_ERROR
			(
				ERROR_TYPE_WIN32,
				StopTraceResult,
				L"StopTrace failed."
			);

			hOverallResult = HRESULT_FROM_WIN32( StopTraceResult );
			goto Cleanup;
		};
	};


	//
	// Delete the ETW trace log file if it exists.
	//

	BOOL bDeleteETWFileResult = DeleteFileW( m_EventProcessingThread.m_LogFileName );
	if ( bDeleteETWFileResult != TRUE )
	{
		DWORD DeleteETWFileError = GetLastError();

		if ( DeleteETWFileError != ERROR_FILE_NOT_FOUND )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				WEX::Common::String().Format
				(
					L"ETW trace file %s could not be deleted.",
					m_EventProcessingThread.m_LogFileName
				)
			);
		};
	};


	hOverallResult = S_OK;


Cleanup:
	m_hEventTracingSession = 0;


	//
	// Clean up the ETW event counting thread.
	//

	m_EventProcessingThread.Cleanup();


	//
	// Delete buffers.
	//

	if ( pEventTracingProperties != nullptr )
	{
		delete [] reinterpret_cast< BYTE * >( pEventTracingProperties );
		pEventTracingProperties = nullptr;
	};


	return hOverallResult;
};
