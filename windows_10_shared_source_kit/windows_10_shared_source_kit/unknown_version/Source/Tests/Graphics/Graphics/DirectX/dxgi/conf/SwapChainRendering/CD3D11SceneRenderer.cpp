//////////////////////////////////////////////////////////////////////
// File:  CD3D11SceneRenderer.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a Direct3D scene management object.
//
// History:
// 12 Dec 2013  SEdmison  Imported from
//                        windowstest\DWM\Common\DXScene\DXScene.cpp.
// 12 Aug 2015  SEdmison  Factored into D3D 11 and 12 versions.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Build flags
//
/////////////////////////////////////////////////////////////////

#define CREATE_DEVICE_CONTEXT_SEPARATELY 1


/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "CD3D11SceneRenderer.hpp"
#include "CD3D11ClearRenderingTechnique.hpp"
#include "CD3D11ColorSwatchPatternRenderingTechnique.hpp"
#include "CD3D11ShadedCubeRenderingTechnique.hpp"
#include "CD3D11TexturedPlaneRenderingTechnique.hpp"


/////////////////////////////////////////////////////////////////
//
// CD3D11SceneRenderer implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Static initializers
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class static:  s_D3DDriverTypePreferences
//
// Purpose:
// Maintains a table of Direct3D driver types to be
// considered when instances of the class create
// Direct3D devices.  To use it, a caller need only
// set the bConsider field to true for each driver type
// that he/she wants considered, and set bConsider to
// false for all other entries in the table.
///////////////////////////////////////////////////////
D3D_DRIVER_TYPE_PREFERENCE CD3D11SceneRenderer::s_D3DDriverTypePreferences[] =
{
	{ D3D_DRIVER_TYPE_HARDWARE, true },
	{ D3D_DRIVER_TYPE_WARP, true },
	{ D3D_DRIVER_TYPE_REFERENCE, false },
	{ D3D_DRIVER_TYPE_SOFTWARE, true },
	{ D3D_DRIVER_TYPE_UNKNOWN, false },
	{ D3D_DRIVER_TYPE_NULL, false },
};


///////////////////////////////////////////////////////
// Class static:  s_SwapChainClearColors
//
// Purpose:
// An array of colors with which to clear the render
// target for each swap chain slot.
///////////////////////////////////////////////////////
FLOAT_COLOR CD3D11SceneRenderer::s_SwapChainClearColors[] =
{
	{ 0.f, 0.f, 0.f, 1.f }, // black
	{ 0.f, 0.f, 0.f, 0.f }, // transparent
};


////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D11SceneRenderer
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CD3D11SceneRenderer::CD3D11SceneRenderer()
:
	m_pLogger( nullptr ),
	m_hWindow( nullptr ),
	m_pWindow( nullptr ),
	m_hCompositionSurface( nullptr ),
	m_SceneWidth( 0 ),
	m_SceneHeight( 0 ),
	m_SelectedD3DDriverType( D3D_DRIVER_TYPE_UNKNOWN ),
	m_PresentFlags( 0 ),
	m_bAnimating( false ),
	m_pDXGIDebug_IDXGIDebug1( nullptr ),
	m_pFactory_IDXGIFactory4( nullptr ),
	m_pFactory_IDXGIFactoryMedia( nullptr ),
	m_pAdapter_IDXGIAdapter1( nullptr ),
	m_pOutput_IDXGIOutput1( nullptr ),
	m_pDevice_ID3D11Device( nullptr ),
	m_pDevice_IDXGIDevice1( nullptr ),
	m_pDevice_ID3D11VideoDevice( nullptr ),
	m_pImmediateContext_ID3D11DeviceContext1( nullptr ),
	m_pImmediateContext_ID3D11VideoContext( nullptr ),
	m_pDepthStencilView( nullptr ),
	m_pVertexShaderConstantBuffer( nullptr )
{
	ZeroMemory( m_SceneTypes, sizeof( m_SceneTypes ) );

	ZeroMemory( &m_SwapChainTypes, sizeof( m_SwapChainTypes ) );

	ZeroMemory( &m_SwapChainBufferFormats, sizeof( m_SwapChainBufferFormats ) );

	ZeroMemory( &m_WorldMatrix, sizeof( m_WorldMatrix ) );
	ZeroMemory( &m_ViewMatrix, sizeof( m_ViewMatrix ) );
	ZeroMemory( &m_ProjectionMatrix, sizeof( m_ProjectionMatrix ) );

	InitializeCriticalSection( &m_RenderingGuard );

	ZeroMemory( m_cSwapChainBufferTextureArraySlices, sizeof( m_cSwapChainBufferTextureArraySlices ) );
	ZeroMemory( m_ppSwapChainBufferTextureArrays, sizeof( m_ppSwapChainBufferTextureArrays ) );

	ZeroMemory( m_LastSwapChainDescriptors, sizeof( m_LastSwapChainDescriptors ) );
	ZeroMemory( m_LastDecodeSwapChainDescriptors, sizeof( m_LastDecodeSwapChainDescriptors ) );
	ZeroMemory( &m_LastSwapChainFullscreenDescriptor, sizeof( m_LastSwapChainFullscreenDescriptor ) );

	ZeroMemory( m_ppSwapChains_IDXGISwapChain1, sizeof( m_ppSwapChains_IDXGISwapChain1 ) );
	ZeroMemory( m_ppSwapChains_IDXGIDecodeSwapChain, sizeof( m_ppSwapChains_IDXGIDecodeSwapChain ) );
	ZeroMemory( m_ppSwapChains_IDXGISwapChainMedia, sizeof( m_ppSwapChains_IDXGISwapChainMedia ) );

	ZeroMemory( m_ppRenderingTechniques, sizeof( m_ppRenderingTechniques ) );
};


///////////////////////////////////////////////////////
// Function:  ~CD3D11SceneRenderer
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CD3D11SceneRenderer::~CD3D11SceneRenderer()
{
	for
	(
		SWAPCHAIN_SLOT CurrentSwapChainSlot = SWAPCHAIN_SLOT_FIRST;
		CurrentSwapChainSlot < SWAPCHAIN_SLOT_COUNT;
		CurrentSwapChainSlot = (SWAPCHAIN_SLOT) ( CurrentSwapChainSlot + 1 )
	)
	{
		assert( m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ] == nullptr );
	};

	assert( m_pDepthStencilView == nullptr );

	assert( m_pVertexShaderConstantBuffer == nullptr );

	assert( m_pImmediateContext_ID3D11VideoContext == nullptr );
	assert( m_pImmediateContext_ID3D11DeviceContext1 == nullptr );

	assert( m_pDevice_IDXGIDevice1 == nullptr );
	assert( m_pDevice_ID3D11Device == nullptr );

	assert( m_pOutput_IDXGIOutput1 == nullptr );

	assert( m_pAdapter_IDXGIAdapter1 == nullptr );

	assert( m_pFactory_IDXGIFactoryMedia == nullptr );
	assert( m_pFactory_IDXGIFactory4 == nullptr );

	assert( m_pDXGIDebug_IDXGIDebug1 == nullptr );

	assert( m_hCompositionSurface == nullptr );

	assert( m_hWindow == nullptr );
	assert( m_pWindow == nullptr );


	SetLogger( nullptr );
};


////////////////////////////////////////////////////////////
//
// Initialization/cleanup
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  Initialize
//
// Purpose:
// Initializes the renderer.
///////////////////////////////////////////////////////

// Intialize overload for windowed swapchains
HRESULT CD3D11SceneRenderer::Initialize
(
	SCENE_TYPE SceneType,
	SWAPCHAIN_TYPE SwapChainType,
	_In_ HWND hWindow,
	bool bRestrictToOutput0,
	UINT BufferCount,
	DXGI_SWAP_EFFECT SwapEffect,
	UINT MaximumFrameLatency
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	RECT WindowClientRect;
	ZeroMemory( &WindowClientRect, sizeof( WindowClientRect ) );


	//
	// Validate parameters.
	//

	assert( hWindow != nullptr );
	if ( hWindow == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"hWindow is NULL."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};

	assert( IsWindow( hWindow ) == TRUE );


	assert
	(
		SwapChainType == SWAPCHAIN_TYPE_CREATESWAPCHAIN ||
		SwapChainType == SWAPCHAIN_TYPE_CREATESWAPCHAINFORHWND
	);

	if
	(
		SwapChainType != SWAPCHAIN_TYPE_CREATESWAPCHAIN &&
		SwapChainType != SWAPCHAIN_TYPE_CREATESWAPCHAINFORHWND
	)
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"Incorrect swap chain type for this specialization of Initialize."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Use the window size as the size of the swapchain.
	//

	BOOL bGetWindowRectResult = GetWindowRect
	(
		hWindow,
		&WindowClientRect
	);

	if ( bGetWindowRectResult != TRUE)
	{
		DWORD GetWindowRectError = GetLastError();

		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			GetWindowRectError,
			L"GetWindowRect failed."
		);

		hOverallResult = HRESULT_FROM_WIN32( GetWindowRectError );
		goto Cleanup;
	};


	//
	// Use the specified window as the swap chain window.
	//

	m_hWindow = hWindow;


	//
	// Finish initialization based on the client rect size.
	//

	SCENE_TYPE SceneTypes[] =
	{
		SceneType,
		SCENE_TYPE_NONE
	};

	SWAPCHAIN_TYPE SwapChainTypes[] =
	{
		SwapChainType,
		SWAPCHAIN_TYPE_NONE
	};

	hOverallResult = Initialize
	(
		SceneTypes,
		SwapChainTypes,
		WindowClientRect.right - WindowClientRect.left,
		WindowClientRect.bottom - WindowClientRect.top,
		bRestrictToOutput0,
		BufferCount,
		SwapEffect,
		MaximumFrameLatency
	);


Cleanup:
	return hOverallResult;
};


// Intialize overload for immersive window
HRESULT CD3D11SceneRenderer::Initialize
(
	_In_reads_( SWAPCHAIN_SLOT_COUNT ) SCENE_TYPE SceneTypes[],
	_In_ IUnknown * pWindow,
	int Width,
	int Height,
	bool bRestrictToOutput0,
	UINT BufferCount,
	DXGI_SWAP_EFFECT SwapEffect,
	UINT MaximumFrameLatency
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Validate parameters.
	//

	if ( pWindow == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"pWindow is NULL."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Use the specified window as the swap chain window.
	//

	m_pWindow = pWindow;
	m_pWindow->AddRef();


	//
	// Finish initialization based on the client rect size.
	//

	SWAPCHAIN_TYPE SwapChainTypes[] =
	{
		SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW,
		SWAPCHAIN_TYPE_NONE
	};

	if ( SceneTypes[ SWAPCHAIN_SLOT_FOREGROUND ] != SCENE_TYPE_NONE )
	{
		SwapChainTypes[ SWAPCHAIN_SLOT_FOREGROUND ] = SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW;
	};


	hOverallResult = Initialize
	(
		SceneTypes,
		SwapChainTypes,
		Width,
		Height,
		bRestrictToOutput0,
		BufferCount,
		SwapEffect,
		MaximumFrameLatency
	);


Cleanup:
	return hOverallResult;
};


// Intialize overload for windowless (composition) swap chains
HRESULT CD3D11SceneRenderer::Initialize
(
	SCENE_TYPE SceneType,
	SWAPCHAIN_TYPE SwapChainType,
	_In_ HANDLE hCompositionSurface,
	int Width,
	int Height,
	bool bRestrictToOutput0,
	UINT BufferCount,
	DXGI_SWAP_EFFECT SwapEffect,
	UINT MaximumFrameLatency
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Use the specified composition surface.
	//

	assert( hCompositionSurface != nullptr );
	if ( hCompositionSurface == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"hCompositionSurface is NULL."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	assert
	(
		SwapChainType == SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE ||
		SwapChainType == SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOMPOSITIONSURFACEHANDLE
	);

	if
	(
		SwapChainType != SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE &&
		SwapChainType != SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOMPOSITIONSURFACEHANDLE
	)
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"Incorrect swap chain type for this specialization of Initialize."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Use the specified composition surface.
	//

	m_hCompositionSurface = hCompositionSurface;


	//
	// Finish initialization with the specified size.
	//

	SCENE_TYPE SceneTypes[] =
	{
		SceneType,
		SCENE_TYPE_NONE
	};

	SWAPCHAIN_TYPE SwapChainTypes[] =
	{
		SwapChainType,
		SWAPCHAIN_TYPE_NONE
	};

	hOverallResult = Initialize
	(
		SceneTypes,
		SwapChainTypes,
		Width,
		Height,
		bRestrictToOutput0,
		BufferCount,
		SwapEffect,
		MaximumFrameLatency
	);


Cleanup:
	return hOverallResult;
};


// Overload that finishes the initialization.
HRESULT CD3D11SceneRenderer::Initialize
(
	_In_reads_( SWAPCHAIN_SLOT_COUNT ) SCENE_TYPE SceneTypes[],
	_In_reads_( SWAPCHAIN_SLOT_COUNT ) SWAPCHAIN_TYPE SwapChainTypes[],
	int Width,
	int Height,
	bool bRestrictToOutput0,
	UINT BufferCount,
	DXGI_SWAP_EFFECT SwapEffect,
	UINT MaximumFrameLatency
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Validate parameters.
	//

	assert( SceneTypes != nullptr );
	if ( SceneTypes == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"SceneTypes is NULL."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	assert( SwapChainTypes != nullptr );
	if ( SwapChainTypes == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"SwapChainTypes is NULL."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	assert
	(
		SwapChainTypes[ SWAPCHAIN_SLOT_FOREGROUND ] == SWAPCHAIN_TYPE_NONE ||
		SwapChainTypes[ SWAPCHAIN_SLOT_FOREGROUND ] == SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW
	);

	if
	(
		SwapChainTypes[ SWAPCHAIN_SLOT_FOREGROUND ] != SWAPCHAIN_TYPE_NONE &&
		SwapChainTypes[ SWAPCHAIN_SLOT_FOREGROUND ] != SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW
	)
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"Invalid foreground swap chain type."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	if ( SwapChainTypes[ SWAPCHAIN_SLOT_FOREGROUND ] != SWAPCHAIN_TYPE_NONE )
	{
		assert( SwapChainTypes[ SWAPCHAIN_SLOT_DEFAULT ] == SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW );
		if ( SwapChainTypes[ SWAPCHAIN_SLOT_DEFAULT ] != SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				E_INVALIDARG,
				L"Invalid foreground swap chain type."
			);

			hOverallResult = E_INVALIDARG;
			goto Cleanup;
		};
	};


	//
	// Use the specified scene dimensions.
	//

	m_SceneWidth = Width;
	m_SceneHeight = Height;


	//
	// Use the specified scene types.
	//

	CopyMemory
	(
		m_SceneTypes,
		SceneTypes,
		sizeof( m_SceneTypes )
	);


	//
	// Use the specified swap chain types.
	//

	CopyMemory
	(
		m_SwapChainTypes,
		SwapChainTypes,
		sizeof( m_SwapChainTypes )
	);


	//
	// Get a DXGI debug object.
	//

	HRESULT hGetDebugResult = DXGIGetDebugInterface1
	(
		0,
		IID_IDXGIDebug1,
		(void **) &m_pDXGIDebug_IDXGIDebug1
	);

	if ( FAILED( hGetDebugResult ) )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"DXGIGetDebugInterface1 failed.  This likely indicates that the DXGI debug layer is not installed, or does not match the version of DXGI in the build."
		);
	};

	if ( m_pDXGIDebug_IDXGIDebug1 != nullptr )
	{
		// Enable leak tracking for this thread.
		m_pDXGIDebug_IDXGIDebug1->EnableLeakTrackingForThread();
	};


	//
	// Create a DXGI factory.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CD3D11SceneRenderer::Initialize:  Creating DXGI factory."
	);

	UINT fFactoryCreation = 0;

#if defined(DEBUG)
	fFactoryCreation |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	if ( IsDebuggerPresent() == TRUE )
	{
		fFactoryCreation |= DXGI_CREATE_FACTORY_DEBUG;
	};

	assert( m_pFactory_IDXGIFactory4 == nullptr );
	HRESULT hCreateFactoryResult = CreateDXGIFactory2
	(
		fFactoryCreation,
		IID_IDXGIFactory4,
		(void **) &m_pFactory_IDXGIFactory4
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
				(void **) &m_pFactory_IDXGIFactory4
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
		L"CD3D11SceneRenderer::Initialize:  Enumerating adapter 0."
	);

	assert( m_pAdapter_IDXGIAdapter1 == nullptr );
	HRESULT hEnumerateAdapter0Result = m_pFactory_IDXGIFactory4->EnumAdapters1
	(
		0,
		&m_pAdapter_IDXGIAdapter1
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

	IDXGIOutput * pOutput = nullptr;
	HRESULT hEnumerateOutput0Result = m_pAdapter_IDXGIAdapter1->EnumOutputs
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

	assert( m_pOutput_IDXGIOutput1 == nullptr );
	HRESULT hQueryForIDXGIOutput1Result = pOutput->QueryInterface
	(
		IID_IDXGIOutput1,
		(void **) &m_pOutput_IDXGIOutput1
	);

	SAFE_RELEASE( pOutput );

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
	HRESULT hGetDefaultOutputDescriptorResult = m_pOutput_IDXGIOutput1->GetDesc( &DefaultOutputDescriptor );
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
	// Set the scene width and height to match the display
	// resolution if needed.
	//

	if ( m_SceneWidth == 0 )
	{
		m_SceneWidth = DefaultOutputDescriptor.DesktopCoordinates.right - DefaultOutputDescriptor.DesktopCoordinates.left;
	};

	if ( m_SceneHeight == 0 )
	{
		m_SceneHeight = DefaultOutputDescriptor.DesktopCoordinates.bottom - DefaultOutputDescriptor.DesktopCoordinates.top;
	};

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"CD3D11SceneRenderer::Initialize:  Setting scene dimensions to %u x %u.",
			m_SceneWidth,
			m_SceneHeight
		)
	);


	//
	// Enable the Direct3D debug layer if needed.
	//

	UINT fD3DDeviceCreation = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG)
	fD3DDeviceCreation |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	if ( IsDebuggerPresent() == TRUE )
	{
		fD3DDeviceCreation |= D3D11_CREATE_DEVICE_DEBUG;
	};


	//
	// Initialize the array of feature levels to request.
	//

	D3D_FEATURE_LEVEL RequestedFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	UINT cRequestedFeatureLevels = ARRAYSIZE( RequestedFeatureLevels );


	//
	// Create a Direct3D device.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CD3D11SceneRenderer::Initialize:  Creating Direct3D device."
	);

	HRESULT hCreateD3DDeviceResult = E_FAIL;
	D3D_FEATURE_LEVEL ActualFeatureLevel = D3D_FEATURE_LEVEL_11_1;
	for
	(
		UINT iCurrentDriverType = 0;
		iCurrentDriverType < ARRAYSIZE( s_D3DDriverTypePreferences );
		++iCurrentDriverType
	)
	{
		//
		// Skip driver types that have been eliminated.
		//

		if ( s_D3DDriverTypePreferences[ iCurrentDriverType ].bConsider != true )
		{
			continue;
		};


		//
		// Provisionally select the next driver type in the list.
		//

		m_SelectedD3DDriverType = s_D3DDriverTypePreferences[ iCurrentDriverType ].DriverType;

		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"- Considering driver type %s.",
				ToString( m_SelectedD3DDriverType ).c_str()
			)
		);


		//
		// Try creating a device with the specified driver type.
		//

		assert( m_pDevice_ID3D11Device == nullptr );
		hCreateD3DDeviceResult = D3D11CreateDevice
		(
			nullptr,
			m_SelectedD3DDriverType,
			nullptr,
			fD3DDeviceCreation,
			RequestedFeatureLevels,
			cRequestedFeatureLevels,
			D3D11_SDK_VERSION,
			&m_pDevice_ID3D11Device,
			&ActualFeatureLevel,
			nullptr
		);

		if
		(
			FAILED( hCreateD3DDeviceResult ) &&
			( fD3DDeviceCreation & D3D11_CREATE_DEVICE_DEBUG ) != 0
		)
		{
			assert( m_pDevice_ID3D11Device == nullptr );
			hCreateD3DDeviceResult = D3D11CreateDevice
			(
				nullptr,
				m_SelectedD3DDriverType,
				nullptr,
				( fD3DDeviceCreation & ~D3D11_CREATE_DEVICE_DEBUG ),
				RequestedFeatureLevels,
				cRequestedFeatureLevels,
				D3D11_SDK_VERSION,
				&m_pDevice_ID3D11Device,
				&ActualFeatureLevel,
				nullptr
			);
		};

		if ( SUCCEEDED( hCreateD3DDeviceResult ) )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"CD3D11SceneRenderer::Initialize:  Created Direct3D device."
			);

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				WEX::Common::String().Format
				(
					L"- Driver type:  %s.",
					ToString( m_SelectedD3DDriverType ).c_str()
				)
			);

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				WEX::Common::String().Format
				(
					L"- Feature level:  %s.",
					ToString( ActualFeatureLevel ).c_str()
				)
			);


			// Query the device for its IDXGIDevice1 interface.
			HRESULT hQueryForIDXGIDevice1Result = m_pDevice_ID3D11Device->QueryInterface
			(
				IID_IDXGIDevice1,
				(void **) &m_pDevice_IDXGIDevice1
			);

			if ( FAILED( hQueryForIDXGIDevice1Result ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hQueryForIDXGIDevice1Result,
					L"QueryInterface on Direct3D device for IDXGIDevice1 failed."
				);

				hOverallResult = hQueryForIDXGIDevice1Result;
				goto Cleanup;
			};


			if ( m_pDevice_ID3D11Device == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_BOOL,
					false,
					L"D3D11CreateDevice succeeded but returned NULL device pointer."
				);

				goto Cleanup;
			};


			// Query the device for its ID3D11VideoDevice interface.
			assert( m_pDevice_ID3D11VideoDevice == nullptr );
			HRESULT hQueryForID3D11VideoDeviceResult = m_pDevice_ID3D11Device->QueryInterface
			(
				IID_ID3D11VideoDevice,
				(void **) &m_pDevice_ID3D11VideoDevice
			);

			if ( FAILED( hQueryForID3D11VideoDeviceResult ) )
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					L"QueryInterface on Direct3D device for ID3D11VideoDevice failed."
				);

				// WARP and REF are the only exceptions; they are allowed
				// NOT to support the ID3D11VideoDevice interface.
				if
				(
					m_SwapChainTypes[ SWAPCHAIN_SLOT_FOREGROUND ] == SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE ||
					m_SwapChainTypes[ SWAPCHAIN_SLOT_BACKGROUND ] == SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE
				)
				{
					if		
					(
						m_SelectedD3DDriverType == D3D_DRIVER_TYPE_REFERENCE ||
						m_SelectedD3DDriverType == D3D_DRIVER_TYPE_WARP
					)
					{
						LOG_MESSAGE
						(
							MESSAGE_TYPE_INFORMATIONAL,
							L"Reference and WARP Direct3D devices do not support the ID3D11VideoDevice interface, which is required for this test."
						);

						continue;
					};

					LOG_ERROR
					(
						ERROR_TYPE_BOOL,
						false,
						L"Hardware Direct3D device did not support ID3D11VideoDevice."
					);

					goto Cleanup;
				};
			};

			if ( m_pDevice_ID3D11VideoDevice == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_BOOL,
					false,
					L"QueryInterface on Direct3D device for ID3D11VideoDevice succeeded but returned NULL video device pointer."
				);

				goto Cleanup;
			};


			//
			// Query the device for its ID3D11Device1 interface.
			//

			ID3D11Device1 * pDevice_ID3D11Device1 = nullptr;
			HRESULT hQueryForID3D11Device1Result = m_pDevice_ID3D11Device->QueryInterface
			(
				IID_ID3D11Device1,
				(void **) &pDevice_ID3D11Device1
			);

			if ( FAILED( hQueryForID3D11Device1Result ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hQueryForID3D11Device1Result,
					L"QueryInterface on Direct3D device for ID3D11Device1 failed."
				);

				hOverallResult = hQueryForID3D11Device1Result;
				goto Cleanup;
			};


			//
			// Get the immediate context.
			//

			assert( m_pImmediateContext_ID3D11DeviceContext1 == nullptr );
			pDevice_ID3D11Device1->GetImmediateContext1
			(
				&m_pImmediateContext_ID3D11DeviceContext1
			);

			SAFE_RELEASE( pDevice_ID3D11Device1 );

			assert( m_pImmediateContext_ID3D11DeviceContext1 != nullptr );


			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"CD3D11SceneRenderer::Initialize:  Direct3D device was created successfully."
			);

			break;
		}
		else
		{
			assert( m_pDevice_ID3D11Device == nullptr );
			SAFE_RELEASE( m_pDevice_ID3D11Device );

			assert( m_pDevice_ID3D11VideoDevice == nullptr );
			SAFE_RELEASE( m_pDevice_ID3D11VideoDevice );

			assert( m_pImmediateContext_ID3D11VideoContext == nullptr );
			SAFE_RELEASE( m_pImmediateContext_ID3D11VideoContext );
		};
	};


	//
	// Handle the fall-through case.
	//

	if ( FAILED( hCreateD3DDeviceResult ) )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_WARNING,
			L"Could not find a suitable device type and feature level."
		);

		hOverallResult = S_FALSE;
		goto Cleanup;
	};


	//
	// Query the immediate context for its ID3D11VideoContext interface.
	//

	assert( m_pDevice_ID3D11VideoDevice != nullptr );
	assert( m_pImmediateContext_ID3D11VideoContext == nullptr );
	HRESULT hQueryForID3D11VideoContextResult = m_pImmediateContext_ID3D11DeviceContext1->QueryInterface
	(
		IID_ID3D11VideoContext,
		(void **) &m_pImmediateContext_ID3D11VideoContext
	);

	if ( FAILED( hQueryForID3D11VideoContextResult ) )
	{
		// We got a video device but can't get a video context?!
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hQueryForID3D11VideoContextResult,
			L"QueryInterface on immediate context for ID3D11VideoContext failed."
		);

		hOverallResult = hQueryForID3D11VideoContextResult;
		goto Cleanup;
	};

	if ( m_pImmediateContext_ID3D11VideoContext == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"QueryInterface on immediate context for ID3D11VideoContext succeeded but returned NULL video context pointer."
		);

		goto Cleanup;
	};


	//
	// Set the maximum frame latency if needed.
	//

	if ( MaximumFrameLatency > 0 )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"CD3D11SceneRenderer::Initialize:  Setting maximum frame latency to %u.",
				MaximumFrameLatency
			)
		);


		HRESULT hSetMaximumFrameLatencyResult = m_pDevice_IDXGIDevice1->SetMaximumFrameLatency( MaximumFrameLatency );

		if ( FAILED( hSetMaximumFrameLatencyResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hSetMaximumFrameLatencyResult,
				L"SetMaximumFrameLatency failed."
			);

			hOverallResult = hSetMaximumFrameLatencyResult;
			goto Cleanup;
		};
	};


	//
	// Find suitable swap chain buffer formats.
	//

	for
	(
		SWAPCHAIN_SLOT CurrentSwapChainSlot = SWAPCHAIN_SLOT_FIRST;
		CurrentSwapChainSlot < SWAPCHAIN_SLOT_COUNT;
		CurrentSwapChainSlot = (SWAPCHAIN_SLOT) ( CurrentSwapChainSlot + 1 )
	)
	{
		// Skip empty swap chain slots.
		if ( m_SwapChainTypes[ CurrentSwapChainSlot ] == SWAPCHAIN_TYPE_NONE )
		{
			continue;
		};


		HRESULT hFindSuitableSwapChainBufferFormatResult = FindSwapChainBufferFormat
		(
			CurrentSwapChainSlot,
			&m_SwapChainBufferFormats[ CurrentSwapChainSlot ]
		);

		if ( FAILED( hFindSuitableSwapChainBufferFormatResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hFindSuitableSwapChainBufferFormatResult,
				L"FindSwapChainBufferFormat failed."
			);

			hOverallResult = hFindSuitableSwapChainBufferFormatResult;
			goto Cleanup;
		};

		if ( hFindSuitableSwapChainBufferFormatResult == S_FALSE )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_WARNING,
				L"No suitable swap chain buffer format was found."
			);

			hOverallResult = S_FALSE;
			goto Cleanup;
		};

#if defined(UNICODE)
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"CD3D11SceneRenderer::Initialize:  Selected format %s for %s swap chain.",
				ToString( m_SwapChainBufferFormats[ CurrentSwapChainSlot ] ).c_str(),
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? L"foreground" : L"background"
			)
		);
#else
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"CD3D11SceneRenderer::Initialize:  Selected format %S for %s swap chain.",
				ToString( m_SwapChainBufferFormats[ CurrentSwapChainSlot ] ).c_str(),
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? L"foreground" : L"background"
			)
		);
#endif
	};


	//
	// Create rendering techniques.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::Initialize:  Allocating rendering techniques."
	);


	assert( m_ppRenderingTechniques[ SCENE_TYPE_CUBE_SHADED ] == nullptr );
	m_ppRenderingTechniques[ SCENE_TYPE_CUBE_SHADED ] = new( std::nothrow ) CD3D11ShadedCubeRenderingTechnique( this );
	if ( m_ppRenderingTechniques[ SCENE_TYPE_CUBE_SHADED ] == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Allocation of shaded-cube rendering technique failed."
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	assert( m_ppRenderingTechniques[ SCENE_TYPE_PLANE_TEXTURED ] == nullptr );
	m_ppRenderingTechniques[ SCENE_TYPE_PLANE_TEXTURED ] = new( std::nothrow ) CD3D11TexturedPlaneRenderingTechnique( this );
	if ( m_ppRenderingTechniques[ SCENE_TYPE_PLANE_TEXTURED ] == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Allocation of textured-plane rendering technique failed."
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	assert( m_ppRenderingTechniques[ SCENE_TYPE_SWATCH_FIXEDCOLOR ] == nullptr );
	m_ppRenderingTechniques[ SCENE_TYPE_SWATCH_FIXEDCOLOR ] = new( std::nothrow ) CD3D11ClearRenderingTechnique( this );
	if ( m_ppRenderingTechniques[ SCENE_TYPE_SWATCH_FIXEDCOLOR ] == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Allocation of fixed-color-swatch rendering technique failed."
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	assert( m_ppRenderingTechniques[ SCENE_TYPE_SWATCH_RANDOMCOLOR ] == nullptr );
	m_ppRenderingTechniques[ SCENE_TYPE_SWATCH_RANDOMCOLOR ] = new( std::nothrow ) CD3D11ClearRenderingTechnique( this );
	if ( m_ppRenderingTechniques[ SCENE_TYPE_SWATCH_RANDOMCOLOR ] == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Allocation of random-color-swatch rendering technique failed."
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	assert( m_ppRenderingTechniques[ SCENE_TYPE_SWATCH_TRANSPARENT ] == nullptr );
	m_ppRenderingTechniques[ SCENE_TYPE_SWATCH_TRANSPARENT ] = new( std::nothrow ) CD3D11ClearRenderingTechnique( this );
	if ( m_ppRenderingTechniques[ SCENE_TYPE_SWATCH_TRANSPARENT ] == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Allocation of shaded-cube rendering technique failed."
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	assert( m_ppRenderingTechniques[ SCENE_TYPE_PATTERN ] == nullptr );
	m_ppRenderingTechniques[ SCENE_TYPE_PATTERN ] = new( std::nothrow ) CD3D11ColorSwatchPatternRenderingTechnique( this );
	if ( m_ppRenderingTechniques[ SCENE_TYPE_PATTERN ] == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"Allocation of color-swatch-pattern rendering technique failed."
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};


	//
	// Create scene objects.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CD3D11SceneRenderer::Initialize:  Creating scene objects."
	);

	for
	(
		SWAPCHAIN_SLOT CurrentSwapChainSlot = SWAPCHAIN_SLOT_FIRST;
		CurrentSwapChainSlot < SWAPCHAIN_SLOT_COUNT;
		CurrentSwapChainSlot = (SWAPCHAIN_SLOT) ( CurrentSwapChainSlot + 1 )
	)
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			WEX::Common::String().Format
			(
				L"CD3D11SceneRenderer::Initialize:  - Processing %s swap chain slot.",
				CurrentSwapChainSlot == SWAPCHAIN_SLOT_DEFAULT ? L"default" : L"foreground"
			)
		);


		//
		// Skip empty swap chain slots.
		//

		if ( m_SwapChainTypes[ CurrentSwapChainSlot ] == SWAPCHAIN_TYPE_NONE )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_DEBUG,
				L"CD3D11SceneRenderer::Initialize:  - Skipping empty swap chain slot."
			);

			continue;
		};


		//
		// Create the appropriate resources for the scene.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D11SceneRenderer::Initialize:  Calling CreateResources on rendering technique."
		);


		HRESULT hCreateTechniqueResourcesResult = m_ppRenderingTechniques[ m_SceneTypes[ CurrentSwapChainSlot ] ]->CreateResources
		(
			CurrentSwapChainSlot
		);

		if ( FAILED( hCreateTechniqueResourcesResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateTechniqueResourcesResult,
				L"CreateResources failed."
			);

			hOverallResult = hCreateTechniqueResourcesResult;
			goto Cleanup;
		};
	};


	//
	// Create the vertex shader constant buffer.
	//

	D3D11_BUFFER_DESC VertexShaderConstantBufferDescriptor;
	ZeroMemory( &VertexShaderConstantBufferDescriptor, sizeof( VertexShaderConstantBufferDescriptor ) );
	VertexShaderConstantBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
	VertexShaderConstantBufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	VertexShaderConstantBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VertexShaderConstantBufferDescriptor.MiscFlags = 0;
	VertexShaderConstantBufferDescriptor.ByteWidth = sizeof( VertexShaderMatrices );

	assert( m_pVertexShaderConstantBuffer == nullptr );
	HRESULT hCreateVertexShaderConstantBufferResult = m_pDevice_ID3D11Device->CreateBuffer
	(
		&VertexShaderConstantBufferDescriptor,
		nullptr,
		&m_pVertexShaderConstantBuffer
	);

	if ( FAILED( hCreateVertexShaderConstantBufferResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateVertexShaderConstantBufferResult,
			L"CreateBuffer on vertex shader constant buffer failed."
		);

		hOverallResult = hCreateVertexShaderConstantBufferResult;
		goto Cleanup;
	};


	//
	// Create size-dependent resources.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CD3D11SceneRenderer::Initialize:  - Creating size-dependent resources."
	);

	DXGI_SWAP_CHAIN_DESC1 SwapChainDescriptors[ SWAPCHAIN_SLOT_COUNT ];
	ZeroMemory( SwapChainDescriptors, sizeof( SwapChainDescriptors ) );

	SwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].Width = m_SceneWidth;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].Height = m_SceneHeight;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].Format = m_SwapChainBufferFormats[ SWAPCHAIN_SLOT_DEFAULT ];
	SwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].Stereo = FALSE;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].SampleDesc.Count = 1;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].SampleDesc.Quality = 0;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].BufferCount = BufferCount;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].Scaling = DXGI_SCALING_STRETCH;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].SwapEffect = SwapEffect;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	SwapChainDescriptors[ SWAPCHAIN_SLOT_FOREGROUND ].Width = m_SceneWidth;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_FOREGROUND ].Height = m_SceneHeight;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_FOREGROUND ].Format = m_SwapChainBufferFormats[ SWAPCHAIN_SLOT_FOREGROUND ];
	SwapChainDescriptors[ SWAPCHAIN_SLOT_FOREGROUND ].Stereo = FALSE;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_FOREGROUND ].SampleDesc.Count = 1;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_FOREGROUND ].SampleDesc.Quality = 0;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_FOREGROUND ].BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_FOREGROUND ].BufferCount = BufferCount;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_FOREGROUND ].Scaling = DXGI_SCALING_NONE;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_FOREGROUND ].SwapEffect = SwapEffect;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_FOREGROUND ].AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
	SwapChainDescriptors[ SWAPCHAIN_SLOT_FOREGROUND ].Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_FOREGROUND_LAYER;


	DXGI_DECODE_SWAP_CHAIN_DESC DecodeSwapChainDescriptors[ SWAPCHAIN_SLOT_COUNT ];
	ZeroMemory( DecodeSwapChainDescriptors, sizeof( DecodeSwapChainDescriptors ) );


	DXGI_SWAP_CHAIN_FULLSCREEN_DESC SwapChainFullscreenDescriptor;
	ZeroMemory( &SwapChainFullscreenDescriptor, sizeof( SwapChainFullscreenDescriptor ) );

	SwapChainFullscreenDescriptor.RefreshRate.Numerator = 60;
	SwapChainFullscreenDescriptor.RefreshRate.Denominator = 1;
	SwapChainFullscreenDescriptor.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainFullscreenDescriptor.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainFullscreenDescriptor.Windowed = TRUE;

	hOverallResult = RecreateSizeDependentResources
	(
		SwapChainDescriptors,
		DecodeSwapChainDescriptors,
		nullptr, // &SwapChainFullscreenDescriptor
		bRestrictToOutput0
	);

	if ( FAILED( hOverallResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hOverallResult,
			L"RecreateSizeDependentResources failed."
		);
	};


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  SetLogger
//
// Purpose:
// Assigns a logger to the renderer.
///////////////////////////////////////////////////////
void CD3D11SceneRenderer::SetLogger
(
	_In_ ISimpleLog * pLogger
)
{
	SAFE_RELEASE( m_pLogger );

	m_pLogger = pLogger;

	if ( m_pLogger != nullptr )
	{
		m_pLogger->AddRef();
	};
};


///////////////////////////////////////////////////////
// Function:  Uninitialize
//
// Purpose:
// Destroys resources created during Initialize and
// gets the object ready to be released or for another
// call to Initialize.
///////////////////////////////////////////////////////
HRESULT CD3D11SceneRenderer::Uninitialize()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Delete the rendering guard critical section.
	//

	DeleteCriticalSection( &m_RenderingGuard );


	//
	// For each rendering technique, have it destroy its resources,
	// then release it.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::Uninitialize:  Releasing rendering techniques."
	);

	for
	(
		SCENE_TYPE CurrentTechnique = SCENE_TYPE_FIRST;
		CurrentTechnique <= SCENE_TYPE_PATTERN;
		CurrentTechnique = (SCENE_TYPE) ( CurrentTechnique + 1 )
	)
	{
		if ( m_ppRenderingTechniques[ CurrentTechnique ] == nullptr )
		{
			continue;
		};

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			WEX::Common::String().Format
			(
				L"CD3D11SceneRenderer::Uninitialize:  - Calling DestroyResources on technique %u.",
				CurrentTechnique
			)
		);

		m_ppRenderingTechniques[ CurrentTechnique ]->DestroyResources();


		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			WEX::Common::String().Format
			(
				L"CD3D11SceneRenderer::Uninitialize:  - Deleting technique %u.",
				CurrentTechnique
			)
		);

		delete m_ppRenderingTechniques[ CurrentTechnique ];
		m_ppRenderingTechniques[ CurrentTechnique ] = nullptr;
	};


	//
	// Release depth stencil.
	//

	ReleaseDepthStencil();


	//
	// Release swap chain state.
	//

	ReleaseSwapChains();


	//
	// Release swap chain buffers.
	//

	ReleaseSwapChainBuffers();


	//
	// Release other Direct3D and DXGI objects.
	//

	if ( m_pImmediateContext_ID3D11DeviceContext1 != nullptr )
	{
		m_pImmediateContext_ID3D11DeviceContext1->ClearState();
	};


	SAFE_RELEASE( m_pVertexShaderConstantBuffer );

	SAFE_RELEASE( m_pImmediateContext_ID3D11VideoContext );
	SAFE_RELEASE( m_pImmediateContext_ID3D11DeviceContext1 );

	SAFE_RELEASE( m_pDevice_ID3D11VideoDevice );
	SAFE_RELEASE( m_pDevice_IDXGIDevice1 );
	SAFE_RELEASE( m_pDevice_ID3D11Device );

	SAFE_RELEASE( m_pOutput_IDXGIOutput1 );

	SAFE_RELEASE( m_pAdapter_IDXGIAdapter1 );

#pragma warning( suppress:  28922 )
	if ( m_pFactory_IDXGIFactory4 != nullptr )
	{
		HWND hwndCurrentFocus = nullptr;

		m_pFactory_IDXGIFactory4->GetWindowAssociation( &hwndCurrentFocus );

#pragma warning( suppress:  28922 )
		if ( hwndCurrentFocus != nullptr )
		{
			m_pFactory_IDXGIFactory4->MakeWindowAssociation
			(
				nullptr,
				0
			);
		};

		m_pFactory_IDXGIFactory4->Release();
		m_pFactory_IDXGIFactory4 = nullptr;
	};


	//
	// Report leaked DXGI and Direct3D objects.
	//

	if ( m_pDXGIDebug_IDXGIDebug1 != nullptr )
	{
		m_pDXGIDebug_IDXGIDebug1->ReportLiveObjects
		(
			DXGI_DEBUG_ALL,
			(DXGI_DEBUG_RLO_FLAGS) ( DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL )
		);

		m_pDXGIDebug_IDXGIDebug1->DisableLeakTrackingForThread();

		SAFE_RELEASE( m_pDXGIDebug_IDXGIDebug1 );
	};


	//
	// Forget the DirectComposition surface.
	//

	m_hCompositionSurface = nullptr;


	//
	// Release the window.
	//

	SAFE_RELEASE( m_pWindow );

	m_hWindow = nullptr;


	return S_OK;
};


////////////////////////////////////////////////////////////
//
// Accessor methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  GetSwapChain
//
// Purpose:
// Swap chain accessor method.
//
// N.B. [SEdmison]:
// I wanted to avoid having to expose the swap chains
// out of the scene renderer, but I was forced to allow
// access to the swap chain so that it could be passed
// to SetSourceSwapChain in the DComp isolation layer,
// to have the scene renderer know about (or befriend)
// the DCompIsolation layer, or to have the DComp
// isolation layer know about (or befriend) the scene
// renderer.  This seemed the least of those evils.
///////////////////////////////////////////////////////
IDXGISwapChain * CD3D11SceneRenderer::GetSwapChain
(
	SWAPCHAIN_SLOT CurrentSwapChainSlot
)
{
	return m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ];
};


////////////////////////////////////////////////////////////
//
// Resource management methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CreateDepthStencil
//
// Purpose:
// Creates render target and depth stencil surfaces and
// views.
///////////////////////////////////////////////////////
HRESULT CD3D11SceneRenderer::CreateDepthStencil()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	ID3D11Texture2D * pDepthStencilBuffer = nullptr;


	//
	// Create a depth-stencil texture.
	//

	D3D11_TEXTURE2D_DESC DepthStencilDescriptor;
	DepthStencilDescriptor.Width = m_SceneWidth;
	DepthStencilDescriptor.Height = m_SceneHeight;
	DepthStencilDescriptor.MipLevels = 1;
	DepthStencilDescriptor.ArraySize = 1;
	DepthStencilDescriptor.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilDescriptor.SampleDesc.Count = 1;
	DepthStencilDescriptor.SampleDesc.Quality = 0;
	DepthStencilDescriptor.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilDescriptor.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilDescriptor.CPUAccessFlags = 0;
	DepthStencilDescriptor.MiscFlags = 0;

#pragma warning( suppress:  28922 )
	assert( pDepthStencilBuffer == nullptr );
	HRESULT hCreateDepthStencilTextureResult = m_pDevice_ID3D11Device->CreateTexture2D
	(
		&DepthStencilDescriptor,
		nullptr,
		&pDepthStencilBuffer
	);

	if ( FAILED( hCreateDepthStencilTextureResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateDepthStencilTextureResult,
			L"CreateTexture2D to create depth stencil texture failed."
		);

		hOverallResult = hCreateDepthStencilTextureResult;
		goto Cleanup;
	};


	//
	// Create a depth-stencil view.
	//

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDescriptor;
	ZeroMemory( &DepthStencilViewDescriptor, sizeof( DepthStencilViewDescriptor ) );
	DepthStencilViewDescriptor.Format = DepthStencilDescriptor.Format;
	DepthStencilViewDescriptor.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDescriptor.Texture2D.MipSlice = 0;

	assert( m_pDepthStencilView == nullptr );
	HRESULT hCreateDepthStencilViewResult = m_pDevice_ID3D11Device->CreateDepthStencilView
	(
		pDepthStencilBuffer,
		&DepthStencilViewDescriptor,
		&m_pDepthStencilView
	);

	if ( FAILED( hCreateDepthStencilViewResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateDepthStencilViewResult,
			L"CreateDepthStencilView failed."
		);

		hOverallResult = hCreateDepthStencilViewResult;
		goto Cleanup;
	};


	hOverallResult = S_OK;


Cleanup:
	SAFE_RELEASE( pDepthStencilBuffer );


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  CreateSwapChainBuffers
//
// Purpose:
// Creates a texture 2D array with array slices that
// are used as swap chain buffers.
///////////////////////////////////////////////////////
HRESULT CD3D11SceneRenderer::CreateSwapChainBuffers
(
	SWAPCHAIN_SLOT CurrentSwapChainSlot,
	DXGI_FORMAT SwapChainBufferFormat,
	UINT Width,
	UINT Height
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Create a texture array of swap chain buffers.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"Creating texture array of swap chain buffers for %s swap chain.",
			( CurrentSwapChainSlot == SWAPCHAIN_SLOT_DEFAULT ) ? L"default" : L"foreground"
		)
	);

	D3D11_TEXTURE2D_DESC SwapChainBufferArrayDescriptor;
	ZeroMemory( &SwapChainBufferArrayDescriptor, sizeof( SwapChainBufferArrayDescriptor ) );
	SwapChainBufferArrayDescriptor.Width = Width;
	SwapChainBufferArrayDescriptor.Height = Height;
	SwapChainBufferArrayDescriptor.MipLevels = 1;
	SwapChainBufferArrayDescriptor.ArraySize = m_cSwapChainBufferTextureArraySlices[ CurrentSwapChainSlot ];
	SwapChainBufferArrayDescriptor.Format = SwapChainBufferFormat;
	SwapChainBufferArrayDescriptor.SampleDesc.Count = 1;
	SwapChainBufferArrayDescriptor.SampleDesc.Quality = 0;
	SwapChainBufferArrayDescriptor.Usage = D3D11_USAGE_DEFAULT;
	SwapChainBufferArrayDescriptor.BindFlags = D3D11_BIND_DECODER;
	SwapChainBufferArrayDescriptor.CPUAccessFlags = 0;
	SwapChainBufferArrayDescriptor.MiscFlags = 0;

	assert( m_ppSwapChainBufferTextureArrays[ CurrentSwapChainSlot ] == nullptr );
	HRESULT hCreateSwapChainBufferArrayResult = m_pDevice_ID3D11Device->CreateTexture2D
	(
		&SwapChainBufferArrayDescriptor,
		nullptr,
		&m_ppSwapChainBufferTextureArrays[ CurrentSwapChainSlot ]
	);

	if
	(
		hCreateSwapChainBufferArrayResult == E_INVALIDARG &&
		m_pDevice_ID3D11Device->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 &&
		(
			SwapChainBufferFormat == DXGI_FORMAT_AYUV ||
			SwapChainBufferFormat == DXGI_FORMAT_YUY2 ||
			SwapChainBufferFormat == DXGI_FORMAT_NV11 ||
			SwapChainBufferFormat == DXGI_FORMAT_NV12
		)
	)
	{
#if defined(UNICODE)
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"CreateTexture2D on swap chain buffer array returned E_INVALIDARG because format %s is not supported by feature level %s.",
				ToString( SwapChainBufferFormat ).c_str(),
				ToString( m_pDevice_ID3D11Device->GetFeatureLevel() ).c_str()
			)
		);
#else
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"CreateTexture2D on swap chain buffer array returned E_INVALIDARG because format %S is not supported by feature level %S.",
				ToString( m_ForegroundSwapChainBufferFormat ).c_str(),
				ToString( m_pDevice_ID3D11Device->GetFeatureLevel() ).c_str()
			)
		);
#endif

		hOverallResult = hCreateSwapChainBufferArrayResult;
		goto Cleanup;
	};

	if ( FAILED( hCreateSwapChainBufferArrayResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateSwapChainBufferArrayResult,
			L"CreateTexture2D on swap chain buffer array failed."
		);

		hOverallResult = hCreateSwapChainBufferArrayResult;
		goto Cleanup;
	};

	if ( m_ppSwapChainBufferTextureArrays[ CurrentSwapChainSlot ] == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_POINTER,
			L"CreateTexture2D on swap chain buffer array succeeded but returned NULL texture array pointer."
		);

		goto Cleanup;
	};


	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"Swap chain buffer array was created successfully."
	);

	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  CreateSwapChains
//
// Purpose:
// Creates (or re-creates) the swap chain(s).
///////////////////////////////////////////////////////
HRESULT CD3D11SceneRenderer::CreateSwapChains
(
	_In_reads_( SWAPCHAIN_SLOT_COUNT ) DXGI_SWAP_CHAIN_DESC1 SwapChainDescriptors[],
	_In_reads_( SWAPCHAIN_SLOT_COUNT ) DXGI_DECODE_SWAP_CHAIN_DESC DecodeSwapChainDescriptors[],
	_In_ DXGI_SWAP_CHAIN_FULLSCREEN_DESC * pFullscreenDescriptor,
	bool bRestrictToOutput0
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Validate parameters.
	//

	if
	(
		m_SwapChainTypes[ SWAPCHAIN_SLOT_FOREGROUND ] != SWAPCHAIN_TYPE_NONE &&
		m_SwapChainTypes[ SWAPCHAIN_SLOT_FOREGROUND ] != m_SwapChainTypes[ SWAPCHAIN_SLOT_BACKGROUND ]
	)
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"Swap chain types for foreground and background swap chains must match."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	if
	(
		m_SwapChainTypes[ SWAPCHAIN_SLOT_FOREGROUND ] != SWAPCHAIN_TYPE_NONE &&
		m_SwapChainTypes[ SWAPCHAIN_SLOT_FOREGROUND ] != SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW
	)
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"Foreground swap chains can only be created with CreateSwapChainForCoreWindow."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	if
	(
		m_SwapChainTypes[ SWAPCHAIN_SLOT_DEFAULT ] == SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE &&
		DecodeSwapChainDescriptors == nullptr ||

		SwapChainDescriptors == nullptr
	)
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"Swap chain descriptors must be provided for all swap chain slots."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Release existing swap chain state (if any).
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::CreateSwapChains:  Calling ReleaseSwapChains."
	);

	ReleaseSwapChains();


	//
	// Release existing render targets (if any).
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::CreateSwapChains:  Calling ReleaseDepthStencil."
	);

	ReleaseDepthStencil();


	//
	// Capture the incoming swap chain descriptor(s).
	//

	if ( DecodeSwapChainDescriptors != nullptr )
	{
		CopyMemory
		(
			&m_LastDecodeSwapChainDescriptors,
			DecodeSwapChainDescriptors,
			sizeof( m_LastDecodeSwapChainDescriptors )
		);
	};

	if ( SwapChainDescriptors != nullptr )
	{
		CopyMemory
		(
			&m_LastSwapChainDescriptors,
			SwapChainDescriptors,
			sizeof( m_LastSwapChainDescriptors )
		);
	};


	//
	// Copy the incoming full-screen mode descriptor.
	//

	if ( pFullscreenDescriptor != nullptr )
	{
		CopyMemory
		(
			&m_LastSwapChainFullscreenDescriptor,
			pFullscreenDescriptor,
			sizeof( m_LastSwapChainFullscreenDescriptor )
		);
	};


	//
	// Create the swap chain(s).
	//

	for
	(
		SWAPCHAIN_SLOT CurrentSwapChainSlot = SWAPCHAIN_SLOT_FIRST;
		CurrentSwapChainSlot < SWAPCHAIN_SLOT_COUNT;
		CurrentSwapChainSlot = (SWAPCHAIN_SLOT) ( CurrentSwapChainSlot + 1 )
	)
	{
		//
		// Skip empty swap chain slots.
		//

		if ( m_SwapChainTypes[ CurrentSwapChainSlot ] == SWAPCHAIN_TYPE_NONE )
		{
			continue;
		};


		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"CD3D11SceneRenderer::CreateSwapChains:  Creating %s swap chain.",
				CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ? L"foreground" : L"default"
			)
		);


		//
		// Determine the scaling mode to be applied to the swap chain.
		//

		if ( ( SwapChainDescriptors[ CurrentSwapChainSlot ].Scaling == DXGI_SCALING_STRETCH ) != 0 )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"- Scaling mode:  DXGI_SCALING_STRETCH."
			);
		}
		else
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"- Scaling mode:  DXGI_SCALING_NONE."
			);
		};


		//
		// Determine whether to restrict the swap chain to output 0.
		//

		IDXGIOutput1 * pOutput = nullptr;

		if ( bRestrictToOutput0 == true )
		{
			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				L"- Restricting to output 0."
			);

			m_PresentFlags |= DXGI_PRESENT_RESTRICT_TO_OUTPUT;

			// Pass the output to CreateSwapChain1
			pOutput = m_pOutput_IDXGIOutput1;
		};


		//
		// Use the appropriate swap chain creation method to create
		// the swap chain.
		//

		switch ( m_SwapChainTypes[ CurrentSwapChainSlot ] )
		{
			case SWAPCHAIN_TYPE_NONE:
			{
				assert( false && "This case should have been skipped at the top of the loop." );
			} break;


			case SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					L"- Using CreateDecodeSwapChainForCompositionSurfaceHandle."
				);


				assert( CurrentSwapChainSlot == SWAPCHAIN_SLOT_DEFAULT );


				//
				// Create swap chain buffers.
				//

				HRESULT hCreateSwapChainBuffersResult = CreateSwapChainBuffers
				(
					CurrentSwapChainSlot,
					m_SwapChainBufferFormats[ CurrentSwapChainSlot ],
					m_SceneWidth,
					m_SceneHeight
				);

				if ( FAILED( hCreateSwapChainBuffersResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hCreateSwapChainBuffersResult,
						L"CreateSwapChainBuffers failed."
					);

					hOverallResult = hCreateSwapChainBuffersResult;
					goto Cleanup;
				};


				//
				// Validate parameters.
				//

				assert( ( m_LastDecodeSwapChainDescriptors[ CurrentSwapChainSlot ].Flags & DXGI_SWAP_CHAIN_FLAG_FULLSCREEN_VIDEO ) != 0 );
				m_LastDecodeSwapChainDescriptors[ CurrentSwapChainSlot ].Flags |= DXGI_SWAP_CHAIN_FLAG_FULLSCREEN_VIDEO;


				//
				// Query the swap chain buffer texture array for
				// its IDXGIResource interface.
				//

				IDXGIResource * pSwapChainBufferTextureArray_IDXGIResource = nullptr;
				HRESULT hQueryForIDXGIResourceResult = m_ppSwapChainBufferTextureArrays[ CurrentSwapChainSlot ]->QueryInterface
				(
					IID_IDXGIResource,
					(void **) &pSwapChainBufferTextureArray_IDXGIResource
				);

				if ( FAILED( hQueryForIDXGIResourceResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hQueryForIDXGIResourceResult,
						L"QueryInterface for IDXGIResource on swap chain buffer texture array failed."
					);

					hOverallResult = hQueryForIDXGIResourceResult;
					goto Cleanup;
				};


				//
				// Create a YUV decoding swap chain.
				//

				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					L"Creating foreground swap chain with CreateDecodeSwapChainForCompositionSurfaceHandle."
				);

				assert( m_pFactory_IDXGIFactoryMedia != nullptr );
				assert( m_ppSwapChains_IDXGIDecodeSwapChain[ CurrentSwapChainSlot ] == nullptr );
				HRESULT hCreateSwapChainResult = m_pFactory_IDXGIFactoryMedia->CreateDecodeSwapChainForCompositionSurfaceHandle
				(
					m_pDevice_ID3D11Device,
					m_hCompositionSurface,
					&m_LastDecodeSwapChainDescriptors[ CurrentSwapChainSlot ],
					pSwapChainBufferTextureArray_IDXGIResource,
					nullptr,
					&m_ppSwapChains_IDXGIDecodeSwapChain[ CurrentSwapChainSlot ]
				);

				SAFE_RELEASE( pSwapChainBufferTextureArray_IDXGIResource );

				if ( FAILED( hCreateSwapChainResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hCreateSwapChainResult,
						L"CreateDecodeSwapChainForCompositionSurfaceHandle failed."
					);

					hOverallResult = hCreateSwapChainResult;
					goto Cleanup;
				};

				if ( m_ppSwapChains_IDXGIDecodeSwapChain[ CurrentSwapChainSlot ] == nullptr )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						E_POINTER,
						L"CreateDecodeSwapChainForCompositionSurfaceHandle succeeded but returned NULL swap chain pointer."
					);

					hOverallResult = E_POINTER;
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

				HRESULT hQuerySwapChainForIDXGISwapChain1Result = m_ppSwapChains_IDXGIDecodeSwapChain[ CurrentSwapChainSlot ]->QueryInterface
				(
					IID_IDXGISwapChain1,
					(void **) &m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ]
				);

				if ( FAILED( hQuerySwapChainForIDXGISwapChain1Result ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hQuerySwapChainForIDXGISwapChain1Result,
						L"QueryInterface for IDXGISwapChain1 failed."
					);

					hOverallResult = hQuerySwapChainForIDXGISwapChain1Result;
					goto Cleanup;
				};

				if ( m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ] == nullptr )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						E_POINTER,
						L"QueryInterface for IDXGISwapChain1 succeeded but returned NULL swap chain pointer."
					);

					hOverallResult = E_POINTER;
					goto Cleanup;
				};


				//
				// Query the swap chain for its IDXGISwapChainMedia interface.
				//

				HRESULT hQuerySwapChainForIDXGISwapChainMediaResult = m_ppSwapChains_IDXGIDecodeSwapChain[ CurrentSwapChainSlot ]->QueryInterface
				(
					__uuidof( IDXGISwapChainMedia ),
					(void **) &m_ppSwapChains_IDXGISwapChainMedia[ CurrentSwapChainSlot ]
				);

				if ( FAILED( hQuerySwapChainForIDXGISwapChainMediaResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hQuerySwapChainForIDXGISwapChainMediaResult,
						L"QueryInterface for IDXGISwapChainMedia failed."
					);

					hOverallResult = hQuerySwapChainForIDXGISwapChainMediaResult;
					goto Cleanup;
				};

				if ( m_ppSwapChains_IDXGISwapChainMedia[ CurrentSwapChainSlot ] == nullptr )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						E_POINTER,
						L"QueryInterface for IDXGISwapChainMedia succeeded but returned NULL swap chain pointer."
					);

					hOverallResult = E_POINTER;
					goto Cleanup;
				};
			} break;


			case SWAPCHAIN_TYPE_CREATESWAPCHAIN:
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					L"- Using CreateSwapChain."
				);


				assert( CurrentSwapChainSlot == SWAPCHAIN_SLOT_DEFAULT );


				//
				// Validate members and state.
				//

				assert( m_hWindow != nullptr );
				if ( m_hWindow == nullptr )
				{
					LOG_ERROR
					(
						ERROR_TYPE_BOOL,
						false,
						L"CreateSwapChain requires a valid window handle."
					);

					goto Cleanup;
				};


				//
				// Create a swap chain.
				//

				assert( m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ] == nullptr );

				DXGI_SWAP_CHAIN_DESC TempSwapChainDescriptor;
				ZeroMemory( &TempSwapChainDescriptor, sizeof( TempSwapChainDescriptor ) );
				TempSwapChainDescriptor.BufferDesc.Width = SwapChainDescriptors[ CurrentSwapChainSlot ].Width;
				TempSwapChainDescriptor.BufferDesc.Height = SwapChainDescriptors[ CurrentSwapChainSlot ].Height;
				TempSwapChainDescriptor.BufferDesc.RefreshRate.Numerator = 60;
				TempSwapChainDescriptor.BufferDesc.RefreshRate.Denominator = 1;
				TempSwapChainDescriptor.BufferDesc.Format = m_SwapChainBufferFormats[ CurrentSwapChainSlot ];
				TempSwapChainDescriptor.SampleDesc = SwapChainDescriptors[ CurrentSwapChainSlot ].SampleDesc;
				TempSwapChainDescriptor.BufferUsage = SwapChainDescriptors[ CurrentSwapChainSlot ].BufferUsage;
				TempSwapChainDescriptor.BufferCount = SwapChainDescriptors[ CurrentSwapChainSlot ].BufferCount;
				TempSwapChainDescriptor.OutputWindow = m_hWindow;
				TempSwapChainDescriptor.Windowed = TRUE;
				TempSwapChainDescriptor.SwapEffect = SwapChainDescriptors[ CurrentSwapChainSlot ].SwapEffect;
				TempSwapChainDescriptor.Flags = SwapChainDescriptors[ CurrentSwapChainSlot ].Flags;

				IDXGISwapChain * pTempSwapChain = nullptr;
				HRESULT hCreateSwapChainResult = m_pFactory_IDXGIFactory4->CreateSwapChain
				(
					m_pDevice_ID3D11Device,
					&TempSwapChainDescriptor,
					&pTempSwapChain
				);

				if ( FAILED( hCreateSwapChainResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hCreateSwapChainResult,
						L"CreateSwapChain failed."
					);

					hOverallResult = hCreateSwapChainResult;
					goto Cleanup;
				};


				//
				// Query the swap chain for its IDXGISwapChain1 interface.
				//

				HRESULT hQueryForIDXGISwapChain1Result = pTempSwapChain->QueryInterface
				(
					IID_IDXGISwapChain1,
					(void **) &m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ]
				);

				SAFE_RELEASE( pTempSwapChain );

				if ( FAILED( hQueryForIDXGISwapChain1Result ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hQueryForIDXGISwapChain1Result,
						L"QueryInterface on swap chain for IDXGISwapChain1 interface failed."
					);

					hOverallResult = hCreateSwapChainResult;
					goto Cleanup;
				};


				//
				// Hook up keypress monitoring for the window.
				//

				HRESULT hMakeWindowAssociationResult = m_pFactory_IDXGIFactory4->MakeWindowAssociation
				(
					m_hWindow,
					0
				);

				if ( FAILED( hMakeWindowAssociationResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hMakeWindowAssociationResult,
						L"MakeWindowAssociation failed."
					);
				};
			} break;


			case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					L"- Using CreateSwapChainForCompositionSurfaceHandle."
				);


				assert( CurrentSwapChainSlot == SWAPCHAIN_SLOT_DEFAULT );


				//
				// Validate members and state.
				//

				assert( m_hCompositionSurface != nullptr );
				if ( m_hCompositionSurface == nullptr )
				{
					LOG_ERROR
					(
						ERROR_TYPE_BOOL,
						false,
						L"CreateSwapChainForCompositionSurfaceHandle requires a valid composition surface handle."
					);

					goto Cleanup;
				};


				//
				// Query the DXGI factory for its IDXGIFactoryMedia interface.
				//

				IDXGIFactoryMedia * pFactory_IDXGIFactoryMedia = NULL;
				HRESULT hQueryForIDXGIFactory4Result = m_pFactory_IDXGIFactory4->QueryInterface
				(
					__uuidof( *pFactory_IDXGIFactoryMedia ),
					reinterpret_cast<void **>( &pFactory_IDXGIFactoryMedia )
				);

				if ( FAILED( hQueryForIDXGIFactory4Result ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hQueryForIDXGIFactory4Result,
						L"QueryInterface on factory for IDXGIFactoryMedia interface failed."
					);

					hOverallResult = hQueryForIDXGIFactory4Result;
					goto Cleanup;
				};


				//
				// Create a windowless swap chain for the composition handle.
				//

				assert( m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ] == nullptr );
				HRESULT hCreateSwapChainResult = pFactory_IDXGIFactoryMedia->CreateSwapChainForCompositionSurfaceHandle
				(
					m_pDevice_ID3D11Device,
					m_hCompositionSurface,
					&SwapChainDescriptors[ CurrentSwapChainSlot ],
					pOutput,
					&m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ]
				);

				SAFE_RELEASE( pFactory_IDXGIFactoryMedia );

				if ( FAILED( hCreateSwapChainResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hCreateSwapChainResult,
						L"CreateSwapChainForCompositionSurfaceHandle failed."
					);

					hOverallResult = hCreateSwapChainResult;
					goto Cleanup;
				};
			} break;


			case SWAPCHAIN_TYPE_CREATESWAPCHAINFORHWND:
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					L"- Using CreateSwapChainForHwnd."
				);


				assert( CurrentSwapChainSlot == SWAPCHAIN_SLOT_DEFAULT );


				//
				// Validate members and state.
				//

				assert( m_hWindow != nullptr );
				if ( m_hWindow == nullptr )
				{
					LOG_ERROR
					(
						ERROR_TYPE_BOOL,
						false,
						L"CreateSwapChainForHwnd requires a valid window handle."
					);

					goto Cleanup;
				};


				//
				// Create a swap chain for the window.
				//

				assert( m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ] == nullptr );
				HRESULT hCreateSwapChainResult = m_pFactory_IDXGIFactory4->CreateSwapChainForHwnd
				(
					m_pDevice_ID3D11Device,
					m_hWindow,
					&SwapChainDescriptors[ CurrentSwapChainSlot ],
					pFullscreenDescriptor,
					pOutput,
					&m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ]
				);

				if ( FAILED( hCreateSwapChainResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hCreateSwapChainResult,
						L"CreateSwapChainForHwnd failed."
					);

					hOverallResult = hCreateSwapChainResult;
					goto Cleanup;
				};
			} break;


			case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW:
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_INFORMATIONAL,
					L"- Using CreateSwapChainForCoreWindow."
				);


				//
				// Validate parameters.
				//

				if ( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND )
				{
					assert( ( SwapChainDescriptors[ CurrentSwapChainSlot ].Flags & DXGI_SWAP_CHAIN_FLAG_FOREGROUND_LAYER ) != 0 );
					SwapChainDescriptors[ CurrentSwapChainSlot ].Flags |= DXGI_SWAP_CHAIN_FLAG_FOREGROUND_LAYER;
				};


				//
				// Validate members and state.
				//

				assert( m_pWindow != nullptr );
				if ( m_pWindow == nullptr )
				{
					LOG_ERROR
					(
						ERROR_TYPE_BOOL,
						false,
						L"CreateSwapChainForCoreWindow requires a valid core window pointer."
					);

					goto Cleanup;
				};


				//
				// Create a swap chain for a core window.
				//

				assert( m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ] == nullptr );
				HRESULT hCreateSwapChainResult = m_pFactory_IDXGIFactory4->CreateSwapChainForCoreWindow
				(
					m_pDevice_ID3D11Device,
					m_pWindow,
					&SwapChainDescriptors[ CurrentSwapChainSlot ],
					pOutput,
					&m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ]
				);

				if ( FAILED( hCreateSwapChainResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hCreateSwapChainResult,
						L"CreateSwapChainForCoreWindow failed."
					);

					hOverallResult = hCreateSwapChainResult;
					goto Cleanup;
				};
			} break;


			default:
			{
				assert( false && "Unhandled swap chain type." );
			} break;
		};
	};


	//
	// Create a depth stencil if needed.
	//

	hOverallResult = CreateDepthStencil();


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  FindSwapChainBufferFormat
//
// Purpose:
// Finds a buffer format that can be used as a swap
// chain buffer for the specified swap chain slot.
///////////////////////////////////////////////////////
HRESULT CD3D11SceneRenderer::FindSwapChainBufferFormat
(
	SWAPCHAIN_SLOT CurrentSwapChainSlot,
	_Out_ DXGI_FORMAT * pSwapChainBufferFormat
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	bool bSuitableSwapChainFormatFound = false;


	//
	// Validate and initialize parameters.
	//

	if ( pSwapChainBufferFormat == nullptr )
	{
		hOverallResult = E_POINTER;
		goto Cleanup;
	};

	*pSwapChainBufferFormat = DXGI_FORMAT_UNKNOWN;


	assert( m_SwapChainTypes[ CurrentSwapChainSlot ] != SWAPCHAIN_TYPE_NONE );
	if ( m_SwapChainTypes[ CurrentSwapChainSlot ] == SWAPCHAIN_TYPE_NONE )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"There is no swap chain for the specified swap chain slot."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Validate members and state.
	//

	assert( m_pDevice_ID3D11Device != nullptr );
	if ( m_pDevice_ID3D11Device == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"m_pDevice_ID3D11Device is NULL."
		);

		goto Cleanup;
	};


	//
	// Search for a swap chain format that will meet our needs.
	//

	switch ( m_SwapChainTypes[ CurrentSwapChainSlot ] )
	{
		case SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
		{
			for
			(
				DXGI_FORMAT TempFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
				TempFormat <= DXGI_FORMAT_B4G4R4A4_UNORM;
				TempFormat = (DXGI_FORMAT) ( (UINT) TempFormat + 1 )
			)
			{
				UINT SupportForTempBufferFormat = 0;
				m_pDevice_ID3D11Device->CheckFormatSupport
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

					*pSwapChainBufferFormat = TempFormat;

					bSuitableSwapChainFormatFound = true;

					break;
				};
			};
		} break;


		case SWAPCHAIN_TYPE_CREATESWAPCHAIN:
		case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
		case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW:
		case SWAPCHAIN_TYPE_CREATESWAPCHAINFORHWND:
		{
			*pSwapChainBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;

			bSuitableSwapChainFormatFound = true;
		} break;


		default:
		{
			assert( false && L"Unhandled swap chain type." );

			hOverallResult = E_NOTIMPL;
			goto Cleanup;
		} break;
	};


	//
	// Error out if we couldn't find a good format.
	//

	if ( bSuitableSwapChainFormatFound != true )
	{
		hOverallResult = S_FALSE;
		goto Cleanup;
	};


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  RecreateSizeDependentResources
//
// Purpose:
// Creates (or re-creates) resources that are tied to
// the size of the swap chain buffers, such as the
// viewport, the matrices, and the swap chain itself.
///////////////////////////////////////////////////////
HRESULT CD3D11SceneRenderer::RecreateSizeDependentResources
(
	bool bRestrictToOutput0, // = false
	UINT BufferCount // = 0
)
{
	return RecreateSizeDependentResources
	(
		m_SceneWidth,
		m_SceneHeight,
		bRestrictToOutput0,
		BufferCount
	);
};


HRESULT CD3D11SceneRenderer::RecreateSizeDependentResources
(
	UINT NewWidth,
	UINT NewHeight,
	bool bRestrictToOutput0, // = false
	UINT BufferCount // = 0
)
{
	//
	// Create a default swap chain descriptor.
	//

	if ( BufferCount == 0 )
	{
		BufferCount = m_LastSwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].BufferCount;
	};


	//
	// Override the width and height that we captured at swap
	// chain creation in case they've changed.
	//

	m_LastSwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].Width = NewWidth;
	m_LastSwapChainDescriptors[ SWAPCHAIN_SLOT_DEFAULT ].Height = NewHeight;


	//
	// Update the scene width and height to reflect the new
	// dimensions.
	//

	m_SceneWidth = NewWidth;
	m_SceneHeight = NewHeight;


	//
	// Hand off to another overload to finish up.
	//

	return CD3D11SceneRenderer::RecreateSizeDependentResources
	(
		m_LastSwapChainDescriptors,
		m_LastDecodeSwapChainDescriptors,
		nullptr,
		bRestrictToOutput0
	);
};


HRESULT CD3D11SceneRenderer::RecreateSizeDependentResources
(
	_In_reads_( SWAPCHAIN_SLOT_COUNT ) DXGI_SWAP_CHAIN_DESC1 SwapChainDescriptors[],
	_In_reads_( SWAPCHAIN_SLOT_COUNT ) DXGI_DECODE_SWAP_CHAIN_DESC DecodeSwapChainDescriptors[],
	_In_ DXGI_SWAP_CHAIN_FULLSCREEN_DESC * pFullscreenDescriptor,
	bool bRestrictToOutput0 // = false
)
{
	//
	// Enter the rendering guard critical section so that the
	// rendering thread does not render during the mode change.
	//

	EnterCriticalSection( &m_RenderingGuard );


	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Initialize the world, view, and projection matrices.
	//

	// Initialize the world matrix.
	D3DXMatrixIdentity( &m_WorldMatrix );

	// Initialize the view matrix.
	D3DXVECTOR3 PointToLookAt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 UpVector( 0.0f, 1.0f, 0.0f );

	D3DXVECTOR3 EyePosition( 2.0f, 2.0f, -5.0f );
	D3DXMatrixLookAtLH
	(
		&m_ViewMatrix,
		&EyePosition,
		&PointToLookAt,
		&UpVector
	);

	// Initialize the projection matrix.
	D3DXMatrixPerspectiveFovLH
	(
		&m_ProjectionMatrix,
		(float) D3DX_PI * 0.5f,
		(float) m_SceneWidth / (float) m_SceneHeight,
		0.1f,
		1000.0f
	);


	//
	// Validate parameters.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::RecreateSizeDependentResources:  Validating parameters."
	);

	if
	(
		m_SwapChainTypes[ SWAPCHAIN_SLOT_DEFAULT ] == SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE &&
		DecodeSwapChainDescriptors == nullptr
	)
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"DecodeSwapChainDescriptors is NULL."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	if
	(
		m_SwapChainTypes[ SWAPCHAIN_SLOT_DEFAULT ] != SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE &&
		SwapChainDescriptors == nullptr
	)
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			E_INVALIDARG,
			L"SwapChainDescriptors is NULL."
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::RecreateSizeDependentResources:  Start."
	);


	//
	// Set the world, view, and projection matrices as constants
	// to our shaders.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::RecreateSizeDependentResources:  Setting shader constants."
	);

	HRESULT hSetShaderConstantsResult = SetShaderConstants();

	if ( FAILED( hSetShaderConstantsResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hSetShaderConstantsResult,
			L"SetShaderConstants failed."
		);

		hOverallResult = hSetShaderConstantsResult;
		goto Cleanup;
	};


	//
	// Set the viewport.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CD3D11SceneRenderer::RecreateSizeDependentResources:  Setting viewport."
	);

	D3D11_VIEWPORT Viewport;
	Viewport.Width = (float) m_SceneWidth;
	Viewport.Height = (float) m_SceneHeight;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;

	m_pImmediateContext_ID3D11DeviceContext1->RSSetViewports
	(
		1,
		&Viewport
	);


	//
	// Create (or re-create) the swap chain(s).
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CD3D11SceneRenderer::RecreateSizeDependentResources:  Creating swap chain(s)."
	);

	HRESULT hCreateSwapChainsResult = CreateSwapChains
	(
		SwapChainDescriptors,
		DecodeSwapChainDescriptors,
		pFullscreenDescriptor,
		bRestrictToOutput0
	);

	if ( FAILED( hCreateSwapChainsResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateSwapChainsResult,
			L"CreateSwapChains failed."
		);

		hOverallResult = hCreateSwapChainsResult;
		goto Cleanup;
	};


	hOverallResult = S_OK;


Cleanup:
	//
	// Leave the rendering guard critical section.
	//

	LeaveCriticalSection( &m_RenderingGuard );


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  ReleaseDepthStencil
//
// Purpose:
// Releases the depth stencil texture and view.
///////////////////////////////////////////////////////
void CD3D11SceneRenderer::ReleaseDepthStencil()
{
	if ( m_pImmediateContext_ID3D11DeviceContext1 != nullptr )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CD3D11SceneRenderer::ReleaseDepthStencil:  Unbinding render target and depth stencil views."
		);

		//
		// Unbind render target and depth stencil views.
		//

		m_pImmediateContext_ID3D11DeviceContext1->OMSetRenderTargets
		(
			0,
			nullptr,
			nullptr
		);
	};


	//
	// Release the depth stencil view.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CD3D11SceneRenderer::ReleaseDepthStencil:  Releasing depth stencil view."
	);

	SAFE_RELEASE( m_pDepthStencilView );
};


///////////////////////////////////////////////////////
// Function:  ReleaseSwapChainBuffers
//
// Purpose:
// Releases swap chain buffer texture arrays created
// for use with media swap chains.
///////////////////////////////////////////////////////
void CD3D11SceneRenderer::ReleaseSwapChainBuffers()
{
	//
	// Release the swap chain buffer texture arrays.
	//

	for
	(
		SWAPCHAIN_SLOT CurrentSwapChainSlot = SWAPCHAIN_SLOT_FIRST;
		CurrentSwapChainSlot < SWAPCHAIN_SLOT_COUNT;
		CurrentSwapChainSlot = (SWAPCHAIN_SLOT) ( CurrentSwapChainSlot + 1 )
	)
	{
		SAFE_RELEASE( m_ppSwapChainBufferTextureArrays[ CurrentSwapChainSlot ] );
		m_cSwapChainBufferTextureArraySlices[ CurrentSwapChainSlot ] = 0;
	};
};


///////////////////////////////////////////////////////
// Function:  ReleaseSwapChains
//
// Purpose:
// Releases the swap chain(s).
///////////////////////////////////////////////////////
void CD3D11SceneRenderer::ReleaseSwapChains()
{
	//
	// Release the swap chain(s).
	//

	for
	(
		SWAPCHAIN_SLOT CurrentSwapChainSlot = SWAPCHAIN_SLOT_FIRST;
		CurrentSwapChainSlot < SWAPCHAIN_SLOT_COUNT;
		CurrentSwapChainSlot = (SWAPCHAIN_SLOT) ( CurrentSwapChainSlot + 1 )
	)
	{
		if ( m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ] != nullptr )
		{
			switch ( m_SwapChainTypes[ CurrentSwapChainSlot ] )
			{
				case SWAPCHAIN_TYPE_NONE:
				{
				} break;


				case SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
				case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW:
				case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
				case SWAPCHAIN_TYPE_CREATESWAPCHAINFORHWND:
				{
				} break;


				case SWAPCHAIN_TYPE_CREATESWAPCHAIN:
				default:
				{
					m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ]->SetFullscreenState
					(
						FALSE,
						nullptr
					);
				} break;
			};
		};

		SAFE_RELEASE( m_ppSwapChains_IDXGISwapChainMedia[ CurrentSwapChainSlot ] );
		SAFE_RELEASE( m_ppSwapChains_IDXGIDecodeSwapChain[ CurrentSwapChainSlot ] );
		SAFE_RELEASE( m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ] );
	};
};


////////////////////////////////////////////////////////////
//
// Rendering and presentation methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  ClearRenderTarget
//
// Purpose:
// Clears the render target view for the specified swap
// chain slot.
///////////////////////////////////////////////////////
HRESULT CD3D11SceneRenderer::ClearRenderTarget
(
	SWAPCHAIN_SLOT CurrentSwapChainSlot
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	ID3D11Texture2D * pBuffer0_ID3D11Texture2D = nullptr;

	ID3D11RenderTargetView * pRenderTargetView = nullptr;


	//
	// Validate members and state.
	//

	switch ( m_SwapChainTypes[ CurrentSwapChainSlot ] )
	{
		case SWAPCHAIN_TYPE_NONE:
		{
			hOverallResult = S_FALSE;
			goto Cleanup;
		} break;


		case SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
		{
			// For a decode swap chain, the decode buffers
			// are owned and maintained separately from the
			// swap chain, so the expectation is that they
			// have already been initialized, and will just
			// be passed directly to PresentBuffer when the
			// time comes to present the scene.  Thus, this
			// step is a no-op.

			hOverallResult = S_FALSE;
			goto Cleanup;
		} break;


		case SWAPCHAIN_TYPE_CREATESWAPCHAIN:
		case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
		case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW:
		case SWAPCHAIN_TYPE_CREATESWAPCHAINFORHWND:
		{
			//
			// Validate members and state.
			//

			assert( m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ] != nullptr );
			if ( m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ] == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_BOOL,
					false,
					L"Swap chain pointer is null."
				);

				hOverallResult = E_INVALIDARG;
				goto Cleanup;
			};
		} break;


		default:
		{
			assert( false && "Unhandled swap chain type." );
		} break;
	};


	//
	// Get buffer 0 from the swap chain.
	//

	assert( pBuffer0_ID3D11Texture2D == nullptr );
	HRESULT hGetBuffer0Result = m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ]->GetBuffer
	(
		0,
		IID_ID3D11Texture2D,
		(void **) &pBuffer0_ID3D11Texture2D
	);

	if ( FAILED( hGetBuffer0Result ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hGetBuffer0Result,
			L"GetBuffer on swap chain for buffer 0 failed."
		);

		goto Cleanup;
	};

	assert( pBuffer0_ID3D11Texture2D != nullptr );


	//
	// Create a render target view for buffer 0.
	//

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = m_SwapChainBufferFormats[ CurrentSwapChainSlot ];
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2DArray.MipSlice = 0;
	rtvDesc.Texture2DArray.FirstArraySlice = 0;
	rtvDesc.Texture2DArray.ArraySize = 1;

	assert( pRenderTargetView == nullptr );
	HRESULT hCreateRenderTargetViewResult = m_pDevice_ID3D11Device->CreateRenderTargetView
	(
		pBuffer0_ID3D11Texture2D,
		&rtvDesc,
		&pRenderTargetView
	);

	if ( FAILED( hCreateRenderTargetViewResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateRenderTargetViewResult,
			L"CreateRenderTargetView for buffer 0 failed."
		);

		goto Cleanup;
	};

	assert( pRenderTargetView != nullptr );


	//
	// Bind the render target view.
	//

	m_pImmediateContext_ID3D11DeviceContext1->OMSetRenderTargets
	(
		1,
		&pRenderTargetView,
		m_pDepthStencilView
	);


	//
	// Determine the clear color.
	// This will be a fixed color or a random color, depending on
	// the scene type.
	//

	FLOAT_COLOR BackgroundColor;
	ZeroMemory( &BackgroundColor, sizeof( BackgroundColor ) );
	switch ( m_SceneTypes[ CurrentSwapChainSlot ] )
	{
		case SCENE_TYPE_SWATCH_TRANSPARENT:
		{
			// Set the clear color to transparent.
			BackgroundColor.r = 0.f;
			BackgroundColor.g = 0.f;
			BackgroundColor.b = 0.f;
			BackgroundColor.a = 0.f;
		} break;


		case SCENE_TYPE_SWATCH_FIXEDCOLOR:
		{
			// Set the clear color to azure.
			BackgroundColor.r = 0.f;
			BackgroundColor.g = 0.5f;
			BackgroundColor.b = 1.f;
			BackgroundColor.a = 1.f;
		} break;


		case SCENE_TYPE_SWATCH_RANDOMCOLOR:
		{
			// Randomize the clear color.
			BackgroundColor.r = rand() / (float) RAND_MAX;
			BackgroundColor.g = rand() / (float) RAND_MAX;
			BackgroundColor.b = rand() / (float) RAND_MAX;
			BackgroundColor.a = 1.f;
		} break;


		default:
		{
			// Use the default clear color for the swap chain slot.
			BackgroundColor.r = s_SwapChainClearColors[ CurrentSwapChainSlot ].r;
			BackgroundColor.g = s_SwapChainClearColors[ CurrentSwapChainSlot ].g;
			BackgroundColor.b = s_SwapChainClearColors[ CurrentSwapChainSlot ].b;
			BackgroundColor.a = s_SwapChainClearColors[ CurrentSwapChainSlot ].a;
		} break;
	};


	//
	// Convert normalized RGBA buffer colors to a colors that
	// can be passed to ClearView.
	//

	float BackgroundClearViewColor[ 4 ] = { 0.f, 0.f, 0.f, 0.f };
	HRESULT hInitializeBackgroundClearViewColorResult = InitializeClearViewColor
	(
		m_SwapChainBufferFormats[ CurrentSwapChainSlot ],
		( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? DXGI_ALPHA_MODE_PREMULTIPLIED : DXGI_ALPHA_MODE_STRAIGHT,
		BackgroundColor,
		BackgroundClearViewColor
	);

	if ( FAILED( hInitializeBackgroundClearViewColorResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hInitializeBackgroundClearViewColorResult,
			L"InitializeClearViewColor with background color failed."
		);

		goto Cleanup;
	};


	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"  - Converted background color from normalized RGB color (%f, %f, %f, %f) to clear color (%f, %f, %f, %f).",
			BackgroundColor.r,
			BackgroundColor.g,
			BackgroundColor.b,
			BackgroundColor.a,
			BackgroundClearViewColor[ 0 ],
			BackgroundClearViewColor[ 1 ],
			BackgroundClearViewColor[ 2 ],
			BackgroundClearViewColor[ 3 ]
		)
	);


	//
	// Clear the entire buffer to the background color.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"  - Clearing buffer 0 to (%f, %f, %f, %f).",
			BackgroundClearViewColor[ 0 ],
			BackgroundClearViewColor[ 1 ],
			BackgroundClearViewColor[ 2 ],
			BackgroundClearViewColor[ 3 ]
		)
	);

	assert( m_pImmediateContext_ID3D11DeviceContext1 != nullptr );
	m_pImmediateContext_ID3D11DeviceContext1->ClearView
	(
		pRenderTargetView,
		BackgroundClearViewColor,
		nullptr,
		0
	);


	hOverallResult = S_OK;


Cleanup:
	//
	// Release the reference to swap chain buffer 0.
	//

	SAFE_RELEASE( pBuffer0_ID3D11Texture2D );


	//
	// Release the render target view.
	//

	SAFE_RELEASE( pRenderTargetView );


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  RenderAndPresentOnce
//
// Purpose:
// Renders a single frame.
///////////////////////////////////////////////////////
HRESULT CD3D11SceneRenderer::RenderAndPresentOnce()
{
	return CD3D11SceneRenderer::RenderAndPresentOnce
	(
		1,
		m_PresentFlags
	);
};


HRESULT CD3D11SceneRenderer::RenderAndPresentOnce
(
	UINT SyncInterval,
	UINT PresentFlags
)
{
	//
	// Build default presentation parameters.
	//

	DXGI_PRESENT_PARAMETERS PresentParameters;
	PresentParameters.DirtyRectsCount = 0;
	PresentParameters.pDirtyRects = NULL;
	PresentParameters.pScrollRect = NULL;
	PresentParameters.pScrollOffset = NULL;

	return CD3D11SceneRenderer::RenderAndPresentOnce
	(
		SyncInterval,
		PresentFlags,
		&PresentParameters
	);
};


HRESULT CD3D11SceneRenderer::RenderAndPresentOnce
(
	UINT SyncInterval,
	UINT PresentFlags,
	_In_ DXGI_PRESENT_PARAMETERS * pPresentParameters
)
{
	//
	// Enter the rendering guard critical section so that the main
	// thread does not change the mode during rendering.
	//

	EnterCriticalSection( &m_RenderingGuard );


	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Animate the world matrix if needed.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::RenderAndPresentOnce:  Animating world matrix."
	);

	static UINT64 StartTime = 0;

	if ( m_bAnimating == true )
	{
		UINT64 CurrentTime = GetTickCount64();
		if ( StartTime == 0 )
		{
			StartTime = CurrentTime;
		};

		float ElapsedTime = ( CurrentTime - StartTime ) / 1000.0f;

		D3DXMatrixRotationY( &m_WorldMatrix, ElapsedTime );
	};


	//
	// Render to all render targets and present to all swap chains.
	//

	for
	(
		SWAPCHAIN_SLOT CurrentSwapChainSlot = SWAPCHAIN_SLOT_FIRST;
		CurrentSwapChainSlot < SWAPCHAIN_SLOT_COUNT;
		CurrentSwapChainSlot = (SWAPCHAIN_SLOT) ( CurrentSwapChainSlot + 1 )
	)
	{
		//
		// Skip empty swap chain slots.
		//

		if ( m_SwapChainTypes[ CurrentSwapChainSlot ] == SWAPCHAIN_TYPE_NONE )
		{
			continue;
		};


		//
		// Clear the render target for this swap chain.
		//

		HRESULT hClearRenderTargetResult = ClearRenderTarget
		(
			CurrentSwapChainSlot
		);

		if ( FAILED( hClearRenderTargetResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hClearRenderTargetResult,
				L"ClearRenderTarget failed."
			);

			hOverallResult = hClearRenderTargetResult;
			goto Cleanup;
		};


		//
		// Clear the depth stencil view.
		//

		m_pImmediateContext_ID3D11DeviceContext1->ClearDepthStencilView
		(
			m_pDepthStencilView,
			D3D11_CLEAR_DEPTH,
			1.0,
			0
		);


		//
		// Render scene-dependent content.
		//

		assert( m_ppRenderingTechniques[ m_SceneTypes[ CurrentSwapChainSlot ] ] != nullptr );
		HRESULT hRenderSceneResult = m_ppRenderingTechniques[ m_SceneTypes[ CurrentSwapChainSlot ] ]->RenderScene
		(
			CurrentSwapChainSlot
		);

		if ( FAILED( hRenderSceneResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hRenderSceneResult,
				L"RenderScene failed."
			);

			hOverallResult = hRenderSceneResult;
			goto Cleanup;
		};


		//
		// Present to the current swap chain.
		//

		switch ( m_SwapChainTypes[ CurrentSwapChainSlot ] )
		{
			case SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_DEBUG,
					L"CD3D11SceneRenderer::RenderAndPresentOnce:  Calling PresentBuffer."
				);

				// @TODO:  Determine the next buffer in the sequence.
				UINT BufferToPresent = 0; // iCurrentFrame % m_cSwapChainBufferTextureArraySlices[ CurrentSwapChainSlot ];

				// Present the buffer.
				assert( m_ppSwapChains_IDXGIDecodeSwapChain[ CurrentSwapChainSlot ] != nullptr );
				HRESULT hPresentResult = m_ppSwapChains_IDXGIDecodeSwapChain[ CurrentSwapChainSlot ]->PresentBuffer
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

					hOverallResult = hPresentResult;
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


			case SWAPCHAIN_TYPE_CREATESWAPCHAIN:
			case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_DEBUG,
					L"CD3D11SceneRenderer::RenderAndPresentOnce:  Calling Present."
				);

				// Present to the swap chain.
				assert( m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ] != nullptr );
				HRESULT hPresentResult = m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ]->Present
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
						L"Present failed."
					);

					hOverallResult = hPresentResult;
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


			case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW:
			case SWAPCHAIN_TYPE_CREATESWAPCHAINFORHWND:
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_DEBUG,
					L"CD3D11SceneRenderer::RenderAndPresentOnce:  Calling Present1."
				);

				// Present the frame.
				DXGI_PRESENT_PARAMETERS PresentParameters;
				ZeroMemory( &PresentParameters, sizeof( PresentParameters ) );

				assert( m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ] != nullptr );
				HRESULT hPresentResult = m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ]->Present1
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
						L"Present1 failed."
					);

					hOverallResult = hPresentResult;
					goto Cleanup;
				};

				if ( hPresentResult != S_OK )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"Present1 returned unexpected HRESULT."
					);

					goto Cleanup;
				};
			} break;


			default:
			{
				LOG_ERROR
				(
					ERROR_TYPE_BOOL,
					false,
					L"CD3D11SceneRenderer::RenderAndPresentOnce:  Unhandled swap chain type."
				);

				assert( false && L"Unhandled swap chain type." );

				hOverallResult = E_NOTIMPL;
				goto Cleanup;
			} break;
		};
	};


	hOverallResult = S_OK;

Cleanup:
	//
	// Leave the rendering guard critical section.
	//

	LeaveCriticalSection( &m_RenderingGuard );


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  SetShaderConstants
//
// Purpose:
// Sets the world, projection, and view matrices as
// constants in a constant buffer so that they will be
// available to the vertex shader.
///////////////////////////////////////////////////////
HRESULT CD3D11SceneRenderer::SetShaderConstants()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Map the vertex shader constant buffer.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::SetShaderConstants:  Mapping vertex shader constant buffer."
	);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hMapVertexShaderConstantBufferResult = m_pImmediateContext_ID3D11DeviceContext1->Map
	(
		m_pVertexShaderConstantBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&MappedResource
	);

	if ( FAILED( hMapVertexShaderConstantBufferResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hMapVertexShaderConstantBufferResult,
			L"Map on vertex shader constant buffer failed."
		);

		hOverallResult = hMapVertexShaderConstantBufferResult;
		goto Cleanup;
	};


	//
	// Set vertex shader constants.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::SetShaderConstants:  Filling vertex shader constant buffer."
	);

	VertexShaderMatrices * pVertexShaderMatrices = (VertexShaderMatrices *) MappedResource.pData;
	D3DXMatrixTranspose( &pVertexShaderMatrices->m_World, &m_WorldMatrix );
	D3DXMatrixTranspose( &pVertexShaderMatrices->m_View, &m_ViewMatrix );
	D3DXMatrixTranspose( &pVertexShaderMatrices->m_Projection, &m_ProjectionMatrix );


	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::SetShaderConstants:  Stored world matrix:"
	);

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]",
			pVertexShaderMatrices->m_World._11,
			pVertexShaderMatrices->m_World._12,
			pVertexShaderMatrices->m_World._13,
			pVertexShaderMatrices->m_World._14,
			pVertexShaderMatrices->m_World._21,
			pVertexShaderMatrices->m_World._22,
			pVertexShaderMatrices->m_World._23,
			pVertexShaderMatrices->m_World._24,
			pVertexShaderMatrices->m_World._31,
			pVertexShaderMatrices->m_World._32,
			pVertexShaderMatrices->m_World._33,
			pVertexShaderMatrices->m_World._34,
			pVertexShaderMatrices->m_World._41,
			pVertexShaderMatrices->m_World._42,
			pVertexShaderMatrices->m_World._43,
			pVertexShaderMatrices->m_World._44
		)
	);


	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::SetShaderConstants:  Stored view matrix:"
	);

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]",
			pVertexShaderMatrices->m_View._11,
			pVertexShaderMatrices->m_View._12,
			pVertexShaderMatrices->m_View._13,
			pVertexShaderMatrices->m_View._14,
			pVertexShaderMatrices->m_View._21,
			pVertexShaderMatrices->m_View._22,
			pVertexShaderMatrices->m_View._23,
			pVertexShaderMatrices->m_View._24,
			pVertexShaderMatrices->m_View._31,
			pVertexShaderMatrices->m_View._32,
			pVertexShaderMatrices->m_View._33,
			pVertexShaderMatrices->m_View._34,
			pVertexShaderMatrices->m_View._41,
			pVertexShaderMatrices->m_View._42,
			pVertexShaderMatrices->m_View._43,
			pVertexShaderMatrices->m_View._44
		)
	);


	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::SetShaderConstants:  Stored projection matrix:"
	);

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]",
			pVertexShaderMatrices->m_Projection._11,
			pVertexShaderMatrices->m_Projection._12,
			pVertexShaderMatrices->m_Projection._13,
			pVertexShaderMatrices->m_Projection._14,
			pVertexShaderMatrices->m_Projection._21,
			pVertexShaderMatrices->m_Projection._22,
			pVertexShaderMatrices->m_Projection._23,
			pVertexShaderMatrices->m_Projection._24,
			pVertexShaderMatrices->m_Projection._31,
			pVertexShaderMatrices->m_Projection._32,
			pVertexShaderMatrices->m_Projection._33,
			pVertexShaderMatrices->m_Projection._34,
			pVertexShaderMatrices->m_Projection._41,
			pVertexShaderMatrices->m_Projection._42,
			pVertexShaderMatrices->m_Projection._43,
			pVertexShaderMatrices->m_Projection._44
		)
	);


	//
	// Unmap the vertex shader constant buffer.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::SetShaderConstants:  Unmapping vertex shader constant buffer."
	);

	m_pImmediateContext_ID3D11DeviceContext1->Unmap( m_pVertexShaderConstantBuffer, 0 );


	//
	// Set the constant buffer on the vertex shader pipeline.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11SceneRenderer::SetShaderConstants:  Binding vertex shader constant buffer."
	);

	m_pImmediateContext_ID3D11DeviceContext1->VSSetConstantBuffers
	(
		0,
		1,
		&m_pVertexShaderConstantBuffer
	);


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  StartAnimation
//
// Purpose:
// Indicates that the world matrix should be rotated as
// a function of elapsed (wall clock) time.
///////////////////////////////////////////////////////
void CD3D11SceneRenderer::StartAnimation()
{
	m_bAnimating = true;
};


///////////////////////////////////////////////////////
// Function:  StopAnimation
//
// Purpose:
// Indicates that the world matrix should not be
// rotated as a function of elapsed (wall clock) time.
///////////////////////////////////////////////////////
void CD3D11SceneRenderer::StopAnimation()
{
	m_bAnimating = false;
};
