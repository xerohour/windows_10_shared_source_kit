//////////////////////////////////////////////////////////////////////
// File:  CD3D12SceneRenderer.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a Direct3D 12 scene renderer.
//
// History:
// 12 Dec 2013  SEdmison  Imported from
//                        windowstest\DWM\Common\DXScene\DXScene.cpp.
// 12 Aug 2015  SEdmison  Factored into D3D 11 and 12 versions.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "CD3D12SceneRenderer.hpp"


/////////////////////////////////////////////////////////////////
//
// Helper function implementation
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  InitializeClearViewColor
//
// Purpose:
// Initializes an array of 4 floating-point color
// components to be used as a render-target clear
// color.
///////////////////////////////////////////////////////
HRESULT InitializeClearViewColor
(
	DXGI_FORMAT Format,
	DXGI_ALPHA_MODE AlphaMode,
	_In_ const FLOAT_COLOR & NormalizedColorWithStraightAlpha,
	_Out_writes_( 4 ) float * ClearColor
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Validate and initialize parameters.
	//

	if ( ClearColor == nullptr )
	{
		hOverallResult = E_POINTER;
		goto Cleanup;
	};

	ZeroMemory( ClearColor, 4 * sizeof( float ) );


	//
	// Determine how to proceed by format.
	//

	switch ( Format )
	{
		//
		// RGB/RGBA normalized formats
		//

		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B4G4R4A4_UNORM:
		case DXGI_FORMAT_B5G6R5_UNORM:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
		{
			ClearColor[ 0 ] = NormalizedColorWithStraightAlpha.r;
			ClearColor[ 1 ] = NormalizedColorWithStraightAlpha.g;
			ClearColor[ 2 ] = NormalizedColorWithStraightAlpha.b;
			ClearColor[ 3 ] = NormalizedColorWithStraightAlpha.a;
		} break;


		//
		// RGB/RGBA integer formats
		//

		case DXGI_FORMAT_R10G10B10A2_UINT:
		{
			ClearColor[ 0 ] = NormalizedColorWithStraightAlpha.r * (float) ( ( 2 << 10 ) - 1 );
			ClearColor[ 1 ] = NormalizedColorWithStraightAlpha.g * (float) ( ( 2 << 10 ) - 1 );
			ClearColor[ 2 ] = NormalizedColorWithStraightAlpha.b * (float) ( ( 2 << 10 ) - 1 );
			ClearColor[ 3 ] = NormalizedColorWithStraightAlpha.a * (float) ( ( 2 << 2 ) - 1 );
		} break;

		case DXGI_FORMAT_R8G8B8A8_UINT:
		{
			ClearColor[ 0 ] = NormalizedColorWithStraightAlpha.r * (float) ( ( 2 << 8 ) - 1 );
			ClearColor[ 1 ] = NormalizedColorWithStraightAlpha.g * (float) ( ( 2 << 8 ) - 1 );
			ClearColor[ 2 ] = NormalizedColorWithStraightAlpha.b * (float) ( ( 2 << 8 ) - 1 );
			ClearColor[ 3 ] = NormalizedColorWithStraightAlpha.a * (float) ( ( 2 << 8 ) - 1 );
		} break;


		//
		// YUV formats
		//

		case DXGI_FORMAT_AYUV:
		case DXGI_FORMAT_Y410:
		case DXGI_FORMAT_Y416:
		case DXGI_FORMAT_NV12:
		case DXGI_FORMAT_P010:
		case DXGI_FORMAT_P016:
		case DXGI_FORMAT_420_OPAQUE:
		case DXGI_FORMAT_YUY2:
		case DXGI_FORMAT_Y210:
		case DXGI_FORMAT_Y216:
		case DXGI_FORMAT_NV11:
		case DXGI_FORMAT_AI44:
		case DXGI_FORMAT_IA44:
		{
			ClearColor[ 0 ] =
				(
					( 0.257f * 255.f * NormalizedColorWithStraightAlpha.r ) +
					( 0.504f * 255.f * NormalizedColorWithStraightAlpha.g ) +
					( 0.098f * 255.f * NormalizedColorWithStraightAlpha.b ) +
					16.f
				);

			ClearColor[ 1 ] =
				(
					- ( 0.148f * 255.f * NormalizedColorWithStraightAlpha.r ) -
					  ( 0.291f * 255.f * NormalizedColorWithStraightAlpha.g ) +
					  ( 0.439f * 255.f * NormalizedColorWithStraightAlpha.b ) +
					  128.f
				);

			ClearColor[ 2 ] =
				(
					( 0.439f * 255.f * NormalizedColorWithStraightAlpha.r ) -
					( 0.368f * 255.f * NormalizedColorWithStraightAlpha.g ) -
					( 0.071f * 255.f * NormalizedColorWithStraightAlpha.b ) +
					128.f
				);

			ClearColor[ 3 ] = NormalizedColorWithStraightAlpha.a;
		} break;


		default:
		{
			assert( false && L"Unhandled DXGI format" );

			hOverallResult = E_NOTIMPL;
			goto Cleanup;
		} break;
	};


	//
	// Multiply by alpha if necessary.
	//

	switch ( Format )
	{
		//
		// RGB/RGBA formats
		//

		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B4G4R4A4_UNORM:
		case DXGI_FORMAT_B5G6R5_UNORM:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		{
			if ( AlphaMode == DXGI_ALPHA_MODE_PREMULTIPLIED )
			{
				ClearColor[ 0 ] *= ClearColor[ 3 ];
				ClearColor[ 1 ] *= ClearColor[ 3 ];
				ClearColor[ 2 ] *= ClearColor[ 3 ];
			};
		} break;
	};


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


/////////////////////////////////////////////////////////////////
//
// CD3D12SceneRenderer implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Static initializers
//
////////////////////////////////////////////////////////////

FLOAT_COLOR CD3D12SceneRenderer::s_SwapChainClearColors[] =
{
	//{ 0.7f, 0.7f, 0.4f, 1.f }, // gold

	{ 0.f, 0.f, 0.f, 1.f }, // black
	{ 0.f, 0.f, 0.f, 0.f }, // transparent
};


////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D12SceneRenderer
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CD3D12SceneRenderer::CD3D12SceneRenderer()
:
	m_pLogger( nullptr ),
	m_hWindow( nullptr ),
	m_pWindow( nullptr ),
	m_hCompositionSurface( nullptr ),
	m_SceneWidth( 0 ),
	m_SceneHeight( 0 ),
	m_PresentFlags( 0 ),
	m_bAnimating( false ),
	m_pDXGIDebug_IDXGIDebug1( nullptr ),
	m_pFactory_IDXGIFactory4( nullptr ),
	m_pFactory_IDXGIFactoryMedia( nullptr ),
	m_pAdapter_IDXGIAdapter1( nullptr ),
	m_pOutput_IDXGIOutput1( nullptr ),
	m_pDevice_ID3D12Device( nullptr ),
	m_pRootSignature( nullptr ),
	m_pConstantBufferViewDescriptorHeap( nullptr ),
	m_pRenderTargetViewDescriptorHeap( nullptr ),
	m_pConstantBufferUploadHeap( nullptr ),
	m_pQueue_ID3D12CommandQueue( nullptr ),
	m_pVertexShaderMatrices( nullptr ),
#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
	m_pDevice_ID3D11VideoDevice( nullptr ),
	m_pQueue_ID3D11VideoContext( nullptr ),
#endif
	m_pCurrentFrameFence( nullptr ),
	m_CurrentFrame( 0 )
{
	ZeroMemory( m_SceneTypes, sizeof( m_SceneTypes ) );

	ZeroMemory( &m_SwapChainTypes, sizeof( m_SwapChainTypes ) );

	ZeroMemory( &m_SwapChainBufferFormats, sizeof( m_SwapChainBufferFormats ) );

	ZeroMemory( &m_WorldMatrix, sizeof( m_WorldMatrix ) );
	ZeroMemory( &m_ViewMatrix, sizeof( m_ViewMatrix ) );
	ZeroMemory( &m_ProjectionMatrix, sizeof( m_ProjectionMatrix ) );

	InitializeCriticalSection( &m_RenderingGuard );

#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
	ZeroMemory( m_cSwapChainBufferTextureArraySlices, sizeof( m_cSwapChainBufferTextureArraySlices ) );
	ZeroMemory( m_ppSwapChainBufferTextureArrays, sizeof( m_ppSwapChainBufferTextureArrays ) );
#endif

	ZeroMemory( m_LastSwapChainDescriptors, sizeof( m_LastSwapChainDescriptors ) );
	ZeroMemory( m_LastDecodeSwapChainDescriptors, sizeof( m_LastDecodeSwapChainDescriptors ) );
	ZeroMemory( &m_LastSwapChainFullscreenDescriptor, sizeof( m_LastSwapChainFullscreenDescriptor ) );

	ZeroMemory( m_ppSwapChains_IDXGISwapChain3, sizeof( m_ppSwapChains_IDXGISwapChain3 ) );
	ZeroMemory( m_ppSwapChains_IDXGIDecodeSwapChain, sizeof( m_ppSwapChains_IDXGIDecodeSwapChain ) );
	ZeroMemory( m_ppSwapChains_IDXGISwapChainMedia, sizeof( m_ppSwapChains_IDXGISwapChainMedia ) );

	ZeroMemory( m_ppCommandLists, sizeof( m_ppCommandLists ) );
	ZeroMemory( m_ppCommandAllocators, sizeof( m_ppCommandAllocators ) );

	ZeroMemory( m_RenderTargetViews, sizeof( m_RenderTargetViews ) );

	ZeroMemory( m_ppRenderingTechniques, sizeof( m_ppRenderingTechniques ) );
};


///////////////////////////////////////////////////////
// Function:  ~CD3D12SceneRenderer
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CD3D12SceneRenderer::~CD3D12SceneRenderer()
{
	for
	(
		SCENE_TYPE CurrentTechnique = SCENE_TYPE_FIRST;
		CurrentTechnique <= SCENE_TYPE_PATTERN;
		CurrentTechnique = (SCENE_TYPE) ( CurrentTechnique + 1 )
	)
	{
		assert( m_ppRenderingTechniques[ CurrentTechnique ] == nullptr );
	};

	assert( m_pCurrentFrameFence == nullptr );

	for
	(
		SWAPCHAIN_SLOT CurrentSwapChainSlot = SWAPCHAIN_SLOT_FIRST;
		CurrentSwapChainSlot < SWAPCHAIN_SLOT_COUNT;
		CurrentSwapChainSlot = (SWAPCHAIN_SLOT) ( CurrentSwapChainSlot + 1 )
	)
	{
		assert( m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ] == nullptr );
		assert( m_ppSwapChains_IDXGIDecodeSwapChain[ CurrentSwapChainSlot ] == nullptr );
		assert( m_ppSwapChains_IDXGISwapChainMedia[ CurrentSwapChainSlot ] == nullptr );

		assert( m_ppCommandLists[ CurrentSwapChainSlot ] == nullptr );
		assert( m_ppCommandAllocators[ CurrentSwapChainSlot ] == nullptr );
	};

	assert( m_pVertexShaderMatrices == nullptr );

	assert( m_pQueue_ID3D12CommandQueue == nullptr );

	assert( m_pConstantBufferUploadHeap == nullptr );

	assert( m_pRenderTargetViewDescriptorHeap == nullptr );
	assert( m_pConstantBufferViewDescriptorHeap == nullptr );

	assert( m_pRootSignature == nullptr );

	assert( m_pDevice_ID3D12Device == nullptr );

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
HRESULT CD3D12SceneRenderer::Initialize
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
HRESULT CD3D12SceneRenderer::Initialize
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
HRESULT CD3D12SceneRenderer::Initialize
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
HRESULT CD3D12SceneRenderer::Initialize
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
		L"CD3D12SceneRenderer::Initialize:  Creating DXGI factory."
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
		L"CD3D12SceneRenderer::Initialize:  Enumerating adapter 0."
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
			L"CD3D12SceneRenderer::Initialize:  Setting scene dimensions to %u x %u.",
			m_SceneWidth,
			m_SceneHeight
		)
	);


	//
	// Enable the Direct3D debug layer if needed.
	//

	bool bEnableDebugLayer = false;

#if defined(DEBUG)
	bEnableDebugLayer = true;
#endif

	if ( IsDebuggerPresent() == TRUE )
	{
		bEnableDebugLayer = true;
	};

	if ( bEnableDebugLayer == true )
	{
		ID3D12Debug * pDebugController_ID3D12Debug = nullptr;
		HRESULT hGetDebugInterfaceResult = D3D12GetDebugInterface
		(
			IID_ID3D12Debug,
			(void **) &pDebugController_ID3D12Debug
		);

		if ( SUCCEEDED( hGetDebugInterfaceResult ) )
		{
			pDebugController_ID3D12Debug->EnableDebugLayer();

			pDebugController_ID3D12Debug->Release();
			pDebugController_ID3D12Debug = nullptr;
		};
	};


	//
	// Create a Direct3D device.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CD3D12SceneRenderer::Initialize:  Creating Direct3D device."
	);

	HRESULT hCreateD3DDeviceResult = E_FAIL;
	D3D_FEATURE_LEVEL ActualFeatureLevel = D3D_FEATURE_LEVEL_11_1;


	// Attempt to create a device, ...
	assert( m_pDevice_ID3D12Device == nullptr );
	hCreateD3DDeviceResult = D3D12CreateDevice
	(
		m_pAdapter_IDXGIAdapter1,
		D3D_FEATURE_LEVEL_11_0,
		IID_ID3D12Device,
		(void **) &m_pDevice_ID3D12Device
	);


	// ...and if we can't find suitable hardware, try again with WARP.
	if ( FAILED( hCreateD3DDeviceResult ) )
	{
		m_pAdapter_IDXGIAdapter1->Release();
		m_pAdapter_IDXGIAdapter1 = nullptr;

		HRESULT hEnumerateWarpAdapterResult = m_pFactory_IDXGIFactory4->EnumWarpAdapter
		(
			IID_IDXGIAdapter1,
			(void **) &m_pAdapter_IDXGIAdapter1
		);

		if ( FAILED( hEnumerateWarpAdapterResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hEnumerateWarpAdapterResult,
				L"EnumWarpAdapter failed."
			);

			hOverallResult = hEnumerateWarpAdapterResult;
			goto Cleanup;
		};

		assert( m_pDevice_ID3D12Device == nullptr );
		hCreateD3DDeviceResult = D3D12CreateDevice
		(
			m_pAdapter_IDXGIAdapter1,
			D3D_FEATURE_LEVEL_11_0,
			IID_ID3D12Device,
			(void **) &m_pDevice_ID3D12Device
		);
	};


	//
	// Handle device creation failure cases.
	//

	if ( FAILED( hCreateD3DDeviceResult ) )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_WARNING,
			L"Could not find a suitable device type and feature level with which to create a Direct3D 12 device."
		);

		hOverallResult = S_FALSE;
		goto Cleanup;
	};

	if ( m_pDevice_ID3D12Device == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"D3D12CreateDevice succeeded but returned NULL device pointer."
		);

		goto Cleanup;
	};

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CD3D12SceneRenderer::Initialize:  Created Direct3D 12 device."
	);


	//
	// Query and log feature level support.
	//

	D3D_FEATURE_LEVEL FeatureLevelsRequested[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS FeatureLevelSupport;
	ZeroMemory( &FeatureLevelSupport, sizeof( FeatureLevelSupport ) );
	FeatureLevelSupport.NumFeatureLevels = ARRAYSIZE( FeatureLevelsRequested );
	FeatureLevelSupport.pFeatureLevelsRequested = FeatureLevelsRequested;

	HRESULT hCheckFeatureLevelSupportResult = m_pDevice_ID3D12Device->CheckFeatureSupport
	(
		D3D12_FEATURE_FEATURE_LEVELS,
		(void *) &FeatureLevelSupport,
		sizeof( FeatureLevelSupport )
	);

	if ( FAILED( hCheckFeatureLevelSupportResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCheckFeatureLevelSupportResult,
			L"CheckFeatureSupport for feature levels failed."
		);

		hOverallResult = hCheckFeatureLevelSupportResult;
		goto Cleanup;
	};


	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"- Maximum supported feature level:  %s.",
			ToString( FeatureLevelSupport.MaxSupportedFeatureLevel ).c_str()
		)
	);


	//
	// Create a command queue.
	//

	D3D12_COMMAND_QUEUE_DESC CommandQueueDescriptor;
	ZeroMemory( &CommandQueueDescriptor, sizeof( CommandQueueDescriptor ) );
	CommandQueueDescriptor.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	CommandQueueDescriptor.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	CommandQueueDescriptor.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	assert( m_pQueue_ID3D12CommandQueue == nullptr );
	HRESULT hCreateCommandQueueResult = m_pDevice_ID3D12Device->CreateCommandQueue
	(
		&CommandQueueDescriptor,
		IID_ID3D12CommandQueue,
		(void **) &m_pQueue_ID3D12CommandQueue
	);

	if ( FAILED( hCreateCommandQueueResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateCommandQueueResult,
			L"CreateCommandQueue failed."
		);

		hOverallResult = hCreateCommandQueueResult;
		goto Cleanup;
	};

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CD3D12SceneRenderer::Initialize:  Direct3D device and related resources were created successfully."
	);


#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
	//
	// Query the command queue for its ID3D11VideoContext interface.
	//

	assert( m_pDevice_ID3D11VideoDevice != nullptr );
	assert( m_pQueue_ID3D11VideoContext == nullptr );
	HRESULT hQueryForID3D11VideoContextResult = m_pQueue_ID3D12CommandQueue->QueryInterface
	(
		IID_ID3D11VideoContext,
		(void **) &m_pQueue_ID3D11VideoContext
	);

	if ( FAILED( hQueryForID3D11VideoContextResult ) )
	{
		// We got a video device but can't get a video context?!
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hQueryForID3D11VideoContextResult,
			L"QueryInterface on command queue for ID3D11VideoContext failed."
		);

		hOverallResult = hQueryForID3D11VideoContextResult;
		goto Cleanup;
	};

	if ( m_pQueue_ID3D11VideoContext == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"QueryInterface on command queue for ID3D11VideoContext succeeded but returned NULL video context pointer."
		);

		goto Cleanup;
	};
#endif


	//
	// Create a fence.
	//

	assert( m_pCurrentFrameFence == nullptr );
	HRESULT hCreateFenceResult = m_pDevice_ID3D12Device->CreateFence
	(
		m_CurrentFrame,
		D3D12_FENCE_FLAG_NONE,
		IID_ID3D12Fence,
		(void **) &m_pCurrentFrameFence
	);

	if ( FAILED( hCreateFenceResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateFenceResult,
			L"CreateFence failed."
		);

		hOverallResult = hCreateFenceResult;
		goto Cleanup;
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
				L"CD3D12SceneRenderer::Initialize:  Selected format %s for %s swap chain.",
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
				L"CD3D12SceneRenderer::Initialize:  Selected format %S for %s swap chain.",
				ToString( m_SwapChainBufferFormats[ CurrentSwapChainSlot ] ).c_str(),
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? L"foreground" : L"background"
			)
		);
#endif
	};


	//
	// Create Direct3D device-dependent resources like the root signature.
	//

	HRESULT hCreateDeviceResourcesResult = CreateDeviceResources();

	if ( FAILED( hCreateDeviceResourcesResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateDeviceResourcesResult,
			L"CreateDeviceResources failed."
		);

		hOverallResult = hCreateDeviceResourcesResult;
		goto Cleanup;
	};


	//
	// Create rendering techniques.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::Initialize:  Allocating rendering techniques."
	);


	assert( m_ppRenderingTechniques[ SCENE_TYPE_CUBE_SHADED ] == nullptr );
	m_ppRenderingTechniques[ SCENE_TYPE_CUBE_SHADED ] = new( std::nothrow ) CD3D12ShadedCubeRenderingTechnique( this );
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
	m_ppRenderingTechniques[ SCENE_TYPE_PLANE_TEXTURED ] = new( std::nothrow ) CD3D12TexturedPlaneRenderingTechnique( this );
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
	m_ppRenderingTechniques[ SCENE_TYPE_SWATCH_FIXEDCOLOR ] = new( std::nothrow ) CD3D12ClearRenderingTechnique( this );
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
	m_ppRenderingTechniques[ SCENE_TYPE_SWATCH_RANDOMCOLOR ] = new( std::nothrow ) CD3D12ClearRenderingTechnique( this );
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
	m_ppRenderingTechniques[ SCENE_TYPE_SWATCH_TRANSPARENT ] = new( std::nothrow ) CD3D12ClearRenderingTechnique( this );
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
	m_ppRenderingTechniques[ SCENE_TYPE_PATTERN ] = new( std::nothrow ) CD3D12ColorSwatchPatternRenderingTechnique( this );
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
		L"CD3D12SceneRenderer::Initialize:  Creating scene objects."
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
				L"CD3D12SceneRenderer::Initialize:  - Processing %s swap chain slot.",
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
				L"CD3D12SceneRenderer::Initialize:  - Skipping empty swap chain slot."
			);

			continue;
		};


		//
		// Create the appropriate resources for the scene.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12SceneRenderer::Initialize:  Calling CreateResources on rendering technique."
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
	// Create size-dependent resources.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CD3D12SceneRenderer::Initialize:  - Creating size-dependent resources."
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
// Function:  CreateDeviceResources
//
// Purpose:
// Creates scene-independent Direct3D device resources
// like the root signature.
///////////////////////////////////////////////////////
HRESULT CD3D12SceneRenderer::CreateDeviceResources()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	ID3DBlob * pOutputBlob = nullptr;
	ID3DBlob * pErrorBlob = nullptr;


	//
	// Validate members and state.
	//

	if ( m_pRootSignature != nullptr )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CD3D12SceneRenderer::CreateDeviceResources:  Device resources have already been created."
		);

		hOverallResult = S_FALSE;
		goto Cleanup;
	};


	//
	// Create a root parameter with a slots for two descriptor
	// ranges, one consisting of a single constant buffer descriptor,
	// and the other consisting of a single shader resource view
	// descriptor.
	//

	D3D12_DESCRIPTOR_RANGE ConstantBufferViewDescriptorRange;
	ZeroMemory( &ConstantBufferViewDescriptorRange, sizeof( ConstantBufferViewDescriptorRange ) );
	ConstantBufferViewDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	ConstantBufferViewDescriptorRange.NumDescriptors = 1;
	ConstantBufferViewDescriptorRange.BaseShaderRegister = 0;
	ConstantBufferViewDescriptorRange.RegisterSpace = 0;
	ConstantBufferViewDescriptorRange.OffsetInDescriptorsFromTableStart = 0;

	D3D12_DESCRIPTOR_RANGE ShaderResourceViewDescriptorRange;
	ZeroMemory( &ShaderResourceViewDescriptorRange, sizeof( ShaderResourceViewDescriptorRange ) );
	ShaderResourceViewDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	ShaderResourceViewDescriptorRange.NumDescriptors = 1;
	ShaderResourceViewDescriptorRange.BaseShaderRegister = 0;
	ShaderResourceViewDescriptorRange.RegisterSpace = 0;
	ShaderResourceViewDescriptorRange.OffsetInDescriptorsFromTableStart = 1;

	D3D12_DESCRIPTOR_RANGE DescriptorRanges[] =
	{
		ConstantBufferViewDescriptorRange,
		ShaderResourceViewDescriptorRange,
	};

	D3D12_ROOT_PARAMETER RootParameter;
	ZeroMemory( &RootParameter, sizeof( RootParameter ) );
	RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameter.DescriptorTable.NumDescriptorRanges = ARRAYSIZE( DescriptorRanges );
	RootParameter.DescriptorTable.pDescriptorRanges = DescriptorRanges;
	RootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	//
	// Specify a static sampler.
	//

	D3D12_STATIC_SAMPLER_DESC StaticSamplerDescriptor;
	ZeroMemory( &StaticSamplerDescriptor, sizeof( StaticSamplerDescriptor ) );
	StaticSamplerDescriptor.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	StaticSamplerDescriptor.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	StaticSamplerDescriptor.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	StaticSamplerDescriptor.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	StaticSamplerDescriptor.MipLODBias = 0.f;
	StaticSamplerDescriptor.MaxAnisotropy = 1;
	StaticSamplerDescriptor.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	StaticSamplerDescriptor.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	StaticSamplerDescriptor.MinLOD = 0.f;
	StaticSamplerDescriptor.MaxLOD = D3D12_FLOAT32_MAX;
	StaticSamplerDescriptor.ShaderRegister = 0;
	StaticSamplerDescriptor.RegisterSpace = 0;
	StaticSamplerDescriptor.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	//
	// Create a root signature.
	//

	D3D12_ROOT_SIGNATURE_DESC RootSignatureDescriptor;
	ZeroMemory( &RootSignatureDescriptor, sizeof( RootSignatureDescriptor ) );
	RootSignatureDescriptor.NumParameters = 1;
	RootSignatureDescriptor.pParameters = &RootParameter;
	RootSignatureDescriptor.NumStaticSamplers = 1;
	RootSignatureDescriptor.pStaticSamplers = &StaticSamplerDescriptor;
	RootSignatureDescriptor.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;


	// Serialize the root signature.
	HRESULT hSerializeRootSignatureResult = D3D12SerializeRootSignature
	(
		&RootSignatureDescriptor,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&pOutputBlob,
		&pErrorBlob
	);

	if ( FAILED( hSerializeRootSignatureResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hSerializeRootSignatureResult,
			L"D3D12SerializeRootSignature failed."
		);

		hOverallResult = hSerializeRootSignatureResult;
		goto Cleanup;
	};


	// Create the root signature.
	assert( m_pRootSignature == nullptr );
	HRESULT hCreateRootSignatureResult = m_pDevice_ID3D12Device->CreateRootSignature
	(
		0,
		pOutputBlob->GetBufferPointer(),
		pOutputBlob->GetBufferSize(),
		IID_ID3D12RootSignature,
		(void **) &m_pRootSignature
	);

	if ( FAILED( hCreateRootSignatureResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateRootSignatureResult,
			L"CreateRootSignature failed."
		);

		hOverallResult = hCreateRootSignatureResult;
		goto Cleanup;
	};


	//
	// Create a descriptor heap for constant buffer view descriptors
	// and shader resource view descriptors.
	//

	D3D12_DESCRIPTOR_HEAP_DESC ConstantBufferViewDescriptorHeapDescriptor;
	ZeroMemory( &ConstantBufferViewDescriptorHeapDescriptor, sizeof( ConstantBufferViewDescriptorHeapDescriptor ) );
	ConstantBufferViewDescriptorHeapDescriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	ConstantBufferViewDescriptorHeapDescriptor.NumDescriptors = 2;
	ConstantBufferViewDescriptorHeapDescriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	assert( m_pConstantBufferViewDescriptorHeap == nullptr );
	HRESULT hCreateConstantBufferViewDescriptorHeapResult = m_pDevice_ID3D12Device->CreateDescriptorHeap
	(
		&ConstantBufferViewDescriptorHeapDescriptor,
		IID_ID3D12DescriptorHeap,
		(void **) &m_pConstantBufferViewDescriptorHeap
	);

	if ( FAILED( hCreateConstantBufferViewDescriptorHeapResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateConstantBufferViewDescriptorHeapResult,
			L"CreateDescriptorHeap failed."
		);

		hOverallResult = hCreateConstantBufferViewDescriptorHeapResult;
		goto Cleanup;
	};


	//
	// Create a descriptor heap for render target view descriptors.
	//

	D3D12_DESCRIPTOR_HEAP_DESC RenderTargetViewDescriptorHeapDescriptor;
	ZeroMemory( &RenderTargetViewDescriptorHeapDescriptor, sizeof( RenderTargetViewDescriptorHeapDescriptor ) );
	RenderTargetViewDescriptorHeapDescriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RenderTargetViewDescriptorHeapDescriptor.NumDescriptors = (UINT) SWAPCHAIN_SLOT_COUNT;
	RenderTargetViewDescriptorHeapDescriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	assert( m_pRenderTargetViewDescriptorHeap == nullptr );
	HRESULT hCreateRenderTargetViewDescriptorHeapResult = m_pDevice_ID3D12Device->CreateDescriptorHeap
	(
		&RenderTargetViewDescriptorHeapDescriptor,
		IID_ID3D12DescriptorHeap,
		(void **) &m_pRenderTargetViewDescriptorHeap
	);

	if ( FAILED( hCreateRenderTargetViewDescriptorHeapResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateRenderTargetViewDescriptorHeapResult,
			L"CreateDescriptorHeap failed."
		);

		hOverallResult = hCreateRenderTargetViewDescriptorHeapResult;
		goto Cleanup;
	};


	//
	// Create an upload heap for the constant buffer view.
	//

	D3D12_HEAP_PROPERTIES ConstantBufferViewUploadHeapProperties;
	ZeroMemory( &ConstantBufferViewUploadHeapProperties, sizeof( ConstantBufferViewUploadHeapProperties ) );
	ConstantBufferViewUploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	ConstantBufferViewUploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	ConstantBufferViewUploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	ConstantBufferViewUploadHeapProperties.CreationNodeMask = 0;
	ConstantBufferViewUploadHeapProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC ConstantBufferResourceDescriptor;
	ZeroMemory( &ConstantBufferResourceDescriptor, sizeof( ConstantBufferResourceDescriptor ) );
	ConstantBufferResourceDescriptor.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ConstantBufferResourceDescriptor.Alignment = 0;
	ConstantBufferResourceDescriptor.Width = ( ( 3 * sizeof( D3DXMATRIX ) ) + 0xFF ) & ~0xFF; // Aligned to 256 bytes.
	ConstantBufferResourceDescriptor.Height = 1;
	ConstantBufferResourceDescriptor.DepthOrArraySize = 1;
	ConstantBufferResourceDescriptor.MipLevels = 1;
	ConstantBufferResourceDescriptor.Format = DXGI_FORMAT_UNKNOWN;
	ConstantBufferResourceDescriptor.SampleDesc.Count = 1;
	ConstantBufferResourceDescriptor.SampleDesc.Quality = 0;
	ConstantBufferResourceDescriptor.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ConstantBufferResourceDescriptor.Flags = D3D12_RESOURCE_FLAG_NONE;

	assert( m_pConstantBufferUploadHeap == nullptr );
	HRESULT hCreateConstantBufferViewUploadHeapResult = m_pDevice_ID3D12Device->CreateCommittedResource
	(
		&ConstantBufferViewUploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ConstantBufferResourceDescriptor,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_ID3D12Resource,
		(void **) &m_pConstantBufferUploadHeap
	);

	if ( FAILED( hCreateConstantBufferViewUploadHeapResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateConstantBufferViewUploadHeapResult,
			L"CreateCommittedResource failed."
		);

		hOverallResult = hCreateConstantBufferViewUploadHeapResult;
		goto Cleanup;
	};


	//
	// Create a constant buffer view.
	//

	D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferViewDescriptor;
	ZeroMemory( &ConstantBufferViewDescriptor, sizeof( ConstantBufferViewDescriptor ) );

	ConstantBufferViewDescriptor.BufferLocation = m_pConstantBufferUploadHeap->GetGPUVirtualAddress();
	ConstantBufferViewDescriptor.SizeInBytes = ( ( 3 * sizeof( D3DXMATRIX ) ) + 0xFF ) & ~0xFF; // Constant buffer size is required to be 256-byte aligned.

	assert( m_pConstantBufferViewDescriptorHeap != nullptr );
	m_pDevice_ID3D12Device->CreateConstantBufferView
	(
		&ConstantBufferViewDescriptor,
		m_pConstantBufferViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
	);


	//
	// Map the constant buffer upload heap.
	//

	m_pConstantBufferUploadHeap->Map
	(
		0,
		nullptr,
		(void **) &m_pVertexShaderMatrices
	);


	hOverallResult = S_OK;


Cleanup:
	SAFE_RELEASE( pOutputBlob );
	SAFE_RELEASE( pErrorBlob );

	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  SetLogger
//
// Purpose:
// Assigns a logger to the renderer.
///////////////////////////////////////////////////////
void CD3D12SceneRenderer::SetLogger
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
HRESULT CD3D12SceneRenderer::Uninitialize()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Delete the rendering guard critical section.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::Uninitialize:  Deleting rendering guard critical section."
	);

	DeleteCriticalSection( &m_RenderingGuard );


	//
	// For each rendering technique, have it destroy its resources,
	// then release it.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::Uninitialize:  Releasing rendering techniques."
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
				L"CD3D12SceneRenderer::Uninitialize:  - Calling DestroyResources on technique %u.",
				CurrentTechnique
			)
		);

		m_ppRenderingTechniques[ CurrentTechnique ]->DestroyResources();


		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			WEX::Common::String().Format
			(
				L"CD3D12SceneRenderer::Uninitialize:  - Deleting technique %u.",
				CurrentTechnique
			)
		);

		delete m_ppRenderingTechniques[ CurrentTechnique ];
		m_ppRenderingTechniques[ CurrentTechnique ] = nullptr;
	};


	//
	// Release command lists.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::Uninitialize:  Releasing command lists."
	);

	ReleaseCommandLists();


	//
	// Release depth stencil.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::Uninitialize:  Releasing depth stencil."
	);

	ReleaseDepthStencil();


	//
	// Release swap chain state.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::Uninitialize:  Releasing swap chains."
	);

	ReleaseSwapChains();


	//
	// Release swap chain buffers.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::Uninitialize:  Releasing swap chain buffers."
	);

	ReleaseSwapChainBuffers();


	//
	// Release Direct3D device resources.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::Uninitialize:  Releasing device resources."
	);

	ReleaseDeviceResources();


#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
	//
	// Release other Direct3D and DXGI objects.
	//

	if ( m_pQueue_ID3D11DeviceContext1 != nullptr )
	{
		m_pQueue_ID3D11DeviceContext1->ClearState();
	};

	SAFE_RELEASE( m_pQueue_ID3D11VideoContext );

	SAFE_RELEASE( m_pDevice_ID3D11VideoDevice );
#endif


	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::Uninitialize:  Releasing Direct3D 12 device."
	);

	SAFE_RELEASE( m_pDevice_ID3D12Device );


	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::Uninitialize:  Releasing DXGI output."
	);

	SAFE_RELEASE( m_pOutput_IDXGIOutput1 );

	SAFE_RELEASE( m_pAdapter_IDXGIAdapter1 );


	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::Uninitialize:  Releasing DXGI factory."
	);

	SAFE_RELEASE( m_pFactory_IDXGIFactoryMedia );

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

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::Uninitialize:  Reporting live objects from Direct3D and DXGI debug layer."
	);

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

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::Uninitialize:  Releasing immersive window wrapper."
	);

	SAFE_RELEASE( m_pWindow );

	m_hWindow = nullptr;


	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::Uninitialize:  Done."
	);

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
IDXGISwapChain * CD3D12SceneRenderer::GetSwapChain
(
	SWAPCHAIN_SLOT CurrentSwapChainSlot
)
{
	return m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ];
};


////////////////////////////////////////////////////////////
//
// Resource management methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CreateCommandList
//
// Purpose:
// Creates a command list for the specified swap chain
// slot using the specified graphics pipeline state.
///////////////////////////////////////////////////////
HRESULT CD3D12SceneRenderer::CreateCommandList
(
	SWAPCHAIN_SLOT CurrentSwapChainSlot,
	_In_ ID3D12PipelineState * pGraphicsPipelineState
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Validate members and state.
	//

	// Skip empty swap chain slots.
	if ( m_SwapChainTypes[ CurrentSwapChainSlot] == SWAPCHAIN_TYPE_NONE )
	{
		hOverallResult = S_FALSE;
		goto Cleanup;
	};

	if ( pGraphicsPipelineState == nullptr )
	{
		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Create a command allocator.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::CreateCommandList:  Creating command allocator."
	);

	assert( m_ppCommandAllocators[ CurrentSwapChainSlot ] == nullptr );
	HRESULT hCreateCommandAllocatorResult = m_pDevice_ID3D12Device->CreateCommandAllocator
	(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_ID3D12CommandAllocator,
		(void **) &m_ppCommandAllocators[ CurrentSwapChainSlot ]
	);

	if ( FAILED( hCreateCommandAllocatorResult) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateCommandAllocatorResult,
			L"CreateCommandAllocator failed."
		);

		hOverallResult = hCreateCommandAllocatorResult;
		goto Cleanup;
	};


	//
	// Create a command list.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::CreateCommandList:  Creating command list."
	);

	assert( m_ppCommandLists[ CurrentSwapChainSlot ] == nullptr );
	HRESULT hCreateCommandListResult = m_pDevice_ID3D12Device->CreateCommandList
	(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_ppCommandAllocators[ CurrentSwapChainSlot ],
		pGraphicsPipelineState,
		IID_ID3D12CommandList,
		(void **) &m_ppCommandLists[ CurrentSwapChainSlot ]
	);

	if ( FAILED( hCreateCommandListResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateCommandListResult,
			L"CreateCommandList failed."
		);

		hOverallResult = hCreateCommandListResult;
		goto Cleanup;
	};


	//
	// Close the command list.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::CreateCommandList:  Closing command list."
	);

	m_ppCommandLists[ CurrentSwapChainSlot ]->Close();


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  ReleaseCommandLists
//
// Purpose:
// Releases all command lists and command allocators.
///////////////////////////////////////////////////////
void CD3D12SceneRenderer::ReleaseCommandLists()
{
	//
	// Release command lists and command allocators.
	//

	for
	(
		SWAPCHAIN_SLOT CurrentSwapChainSlot = SWAPCHAIN_SLOT_FIRST;
		CurrentSwapChainSlot < SWAPCHAIN_SLOT_COUNT;
		CurrentSwapChainSlot = (SWAPCHAIN_SLOT) ( CurrentSwapChainSlot + 1 )
	)
	{
		SAFE_RELEASE( m_ppCommandLists[ CurrentSwapChainSlot ] );
		SAFE_RELEASE( m_ppCommandAllocators[ CurrentSwapChainSlot ] );
	};
};


///////////////////////////////////////////////////////
// Function:  CreateDepthStencil
//
// Purpose:
// Creates render target and depth stencil surfaces and
// views.
///////////////////////////////////////////////////////
HRESULT CD3D12SceneRenderer::CreateDepthStencil()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


#if USE_DEPTH_STENCIL
	//
	// @TODO:
	// Create a depth stencil surface and related resources.
	//


	hOverallResult = S_OK;


Cleanup:
#else
	hOverallResult = S_FALSE;
#endif


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  CreateSwapChainBuffers
//
// Purpose:
// Creates a texture 2D array with array slices that
// are used as swap chain buffers.
///////////////////////////////////////////////////////
HRESULT CD3D12SceneRenderer::CreateSwapChainBuffers
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


#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
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
#else
	hOverallResult = S_FALSE;
#endif


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  CreateSwapChains
//
// Purpose:
// Creates (or re-creates) the swap chain(s).
///////////////////////////////////////////////////////
HRESULT CD3D12SceneRenderer::CreateSwapChains
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
		L"CD3D12SceneRenderer::CreateSwapChains:  Calling ReleaseSwapChains."
	);

	ReleaseSwapChains();


	//
	// Release existing render targets (if any).
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::CreateSwapChains:  Calling ReleaseDepthStencil."
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
				L"CD3D12SceneRenderer::CreateSwapChains:  Creating %s swap chain.",
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


#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
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
					m_pDevice_ID3D12Device,
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
				// Query the swap chain for its IDXGISwapChain3 interface.
				//

				HRESULT hQuerySwapChainForIDXGISwapChain3Result = m_ppSwapChains_IDXGIDecodeSwapChain[ CurrentSwapChainSlot ]->QueryInterface
				(
					IID_IDXGISwapChain3,
					(void **) &m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ]
				);

				if ( FAILED( hQuerySwapChainForIDXGISwapChain3Result ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hQuerySwapChainForIDXGISwapChain3Result,
						L"QueryInterface for IDXGISwapChain3 failed."
					);

					hOverallResult = hQuerySwapChainForIDXGISwapChain3Result;
					goto Cleanup;
				};

				if ( m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ] == nullptr )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						E_POINTER,
						L"QueryInterface for IDXGISwapChain3 succeeded but returned NULL swap chain pointer."
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
#endif


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

				assert( m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ] == nullptr );

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
					m_pQueue_ID3D12CommandQueue,
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

				if ( pTempSwapChain == nullptr )
				{
					hOverallResult = E_FAIL;

					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hOverallResult,
						L"CreateSwapChain succeeded but returned NULL swap chain pointer."
					);

					goto Cleanup;
				};


				//
				// Query the swap chain for its IDXGISwapChain3 interface.
				//

				HRESULT hQueryForIDXGISwapChain3Result = pTempSwapChain->QueryInterface
				(
					IID_IDXGISwapChain3,
					(void **) &m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ]
				);

				SAFE_RELEASE( pTempSwapChain );

				if ( FAILED( hQueryForIDXGISwapChain3Result ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hQueryForIDXGISwapChain3Result,
						L"QueryInterface on swap chain for IDXGISwapChain3 interface failed."
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

				IDXGIFactoryMedia * pFactory_IDXGIFactoryMedia = nullptr;
				HRESULT hQueryForIDXGIFactoryMediaResult = m_pFactory_IDXGIFactory4->QueryInterface
				(
					__uuidof( *pFactory_IDXGIFactoryMedia ),
					reinterpret_cast<void **>( &pFactory_IDXGIFactoryMedia )
				);

				if ( FAILED( hQueryForIDXGIFactoryMediaResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hQueryForIDXGIFactoryMediaResult,
						L"QueryInterface on factory for IDXGIFactoryMedia interface failed."
					);

					hOverallResult = hQueryForIDXGIFactoryMediaResult;
					goto Cleanup;
				};


				//
				// Create a windowless swap chain for the composition handle.
				//

				assert( m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ] == nullptr );
				IDXGISwapChain1 * pTempSwapChain = nullptr;
				HRESULT hCreateSwapChainResult = pFactory_IDXGIFactoryMedia->CreateSwapChainForCompositionSurfaceHandle
				(
					m_pQueue_ID3D12CommandQueue,
					m_hCompositionSurface,
					&SwapChainDescriptors[ CurrentSwapChainSlot ],
					pOutput,
					&pTempSwapChain
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

				if ( pTempSwapChain == nullptr )
				{
					hOverallResult = E_FAIL;

					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hOverallResult,
						L"CreateSwapChainForHwnd succeeded but returned NULL swap chain pointer."
					);

					goto Cleanup;
				};


				//
				// Query the swap chain for its IDXGISwapChain3 interface.
				//

				HRESULT hQueryForIDXGISwapChain3Result = pTempSwapChain->QueryInterface
				(
					IID_IDXGISwapChain3,
					(void **) &m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ]
				);

				SAFE_RELEASE( pTempSwapChain );

				if ( FAILED( hQueryForIDXGISwapChain3Result ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hQueryForIDXGISwapChain3Result,
						L"QueryInterface on swap chain for its IDXGISwapChain3 interface failed."
					);

					hOverallResult = hQueryForIDXGISwapChain3Result;
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

				IDXGISwapChain1 * pTempSwapChain = nullptr;
				HRESULT hCreateSwapChainResult = m_pFactory_IDXGIFactory4->CreateSwapChainForHwnd
				(
					m_pQueue_ID3D12CommandQueue,
					m_hWindow,
					&SwapChainDescriptors[ CurrentSwapChainSlot ],
					pFullscreenDescriptor,
					pOutput,
					&pTempSwapChain
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

				if ( pTempSwapChain == nullptr )
				{
					hOverallResult = E_FAIL;

					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hOverallResult,
						L"CreateSwapChainForHwnd succeeded but returned NULL swap chain pointer."
					);

					goto Cleanup;
				};


				//
				// Query the swap chain for its IDXGISwapChain3 interface.
				//

				HRESULT hQueryForIDXGISwapChain3Result = pTempSwapChain->QueryInterface
				(
					IID_IDXGISwapChain3,
					(void **) &m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ]
				);

				SAFE_RELEASE( pTempSwapChain );

				if ( FAILED( hQueryForIDXGISwapChain3Result ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hQueryForIDXGISwapChain3Result,
						L"QueryInterface on swap chain for its IDXGISwapChain3 interface failed."
					);

					hOverallResult = hQueryForIDXGISwapChain3Result;
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

				IDXGISwapChain1 * pTempSwapChain = nullptr;
				HRESULT hCreateSwapChainResult = m_pFactory_IDXGIFactory4->CreateSwapChainForCoreWindow
				(
					m_pQueue_ID3D12CommandQueue,
					m_pWindow,
					&SwapChainDescriptors[ CurrentSwapChainSlot ],
					pOutput,
					&pTempSwapChain
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


				if ( pTempSwapChain == nullptr )
				{
					hOverallResult = E_FAIL;

					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hOverallResult,
						L"CreateSwapChainCoreWindow succeeded but returned NULL swap chain pointer."
					);

					goto Cleanup;
				};


				//
				// Query the swap chain for its IDXGISwapChain3 interface.
				//

				HRESULT hQueryForIDXGISwapChain3Result = pTempSwapChain->QueryInterface
				(
					IID_IDXGISwapChain3,
					(void **) &m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ]
				);

				SAFE_RELEASE( pTempSwapChain );

				if ( FAILED( hQueryForIDXGISwapChain3Result ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hQueryForIDXGISwapChain3Result,
						L"QueryInterface on swap chain for its IDXGISwapChain3 interface failed."
					);

					hOverallResult = hQueryForIDXGISwapChain3Result;
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
HRESULT CD3D12SceneRenderer::FindSwapChainBufferFormat
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

	assert( m_pDevice_ID3D12Device != nullptr );
	if ( m_pDevice_ID3D12Device == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"m_pDevice_ID3D12Device is NULL."
		);

		goto Cleanup;
	};


	//
	// Search for a swap chain format that will meet our needs.
	//

	switch ( m_SwapChainTypes[ CurrentSwapChainSlot ] )
	{
#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
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
				m_pDevice_ID3D12Device->CheckFormatSupport
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
#endif


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
HRESULT CD3D12SceneRenderer::RecreateSizeDependentResources
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


HRESULT CD3D12SceneRenderer::RecreateSizeDependentResources
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

	return CD3D12SceneRenderer::RecreateSizeDependentResources
	(
		m_LastSwapChainDescriptors,
		m_LastDecodeSwapChainDescriptors,
		nullptr,
		bRestrictToOutput0
	);
};


HRESULT CD3D12SceneRenderer::RecreateSizeDependentResources
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
	// Validate parameters.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::RecreateSizeDependentResources:  Validating parameters."
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
		L"CD3D12SceneRenderer::RecreateSizeDependentResources:  Start."
	);


	//
	// Set the world, view, and projection matrices as constants
	// to our shaders.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::RecreateSizeDependentResources:  Setting shader constants."
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
	// Create (or re-create) the swap chain(s).
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CD3D12SceneRenderer::RecreateSizeDependentResources:  Creating swap chain(s)."
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
void CD3D12SceneRenderer::ReleaseDepthStencil()
{
#if USE_DEPTH_STENCIL
	//
	// @TODO:
	// Release depth stencil resources.
	//
#endif
};


///////////////////////////////////////////////////////
// Function:  ReleaseSwapChainBuffers
//
// Purpose:
// Releases swap chain buffer texture arrays created
// for use with media swap chains.
///////////////////////////////////////////////////////
void CD3D12SceneRenderer::ReleaseSwapChainBuffers()
{
#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
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
#endif
};


///////////////////////////////////////////////////////
// Function:  ReleaseSwapChains
//
// Purpose:
// Releases the swap chain(s).
///////////////////////////////////////////////////////
void CD3D12SceneRenderer::ReleaseSwapChains()
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
		if ( m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ] != nullptr )
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
					m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ]->SetFullscreenState
					(
						FALSE,
						nullptr
					);
				} break;
			};
		};

		SAFE_RELEASE( m_ppSwapChains_IDXGISwapChainMedia[ CurrentSwapChainSlot ] );
		SAFE_RELEASE( m_ppSwapChains_IDXGIDecodeSwapChain[ CurrentSwapChainSlot ] );
		SAFE_RELEASE( m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ] );
	};
};


///////////////////////////////////////////////////////
// Function:  ReleaseDeviceResources
//
// Purpose:
// Releases scene-independent Direct3D graphics
// resources like the root signature.
///////////////////////////////////////////////////////
void CD3D12SceneRenderer::ReleaseDeviceResources()
{
	//
	// Unmap and release the constant buffer upload heap.
	//

	m_pVertexShaderMatrices = nullptr;
	if ( m_pConstantBufferUploadHeap != nullptr )
	{
		m_pConstantBufferUploadHeap->Unmap
		(
			0,
			nullptr
		);

		m_pConstantBufferUploadHeap->Release();
		m_pConstantBufferUploadHeap = nullptr;
	};


	//
	// Release device-tied resources.
	//

	SAFE_RELEASE( m_pCurrentFrameFence );

	SAFE_RELEASE( m_pQueue_ID3D12CommandQueue );

	SAFE_RELEASE( m_pRenderTargetViewDescriptorHeap );
	SAFE_RELEASE( m_pConstantBufferViewDescriptorHeap );

	SAFE_RELEASE( m_pRootSignature );
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
HRESULT CD3D12SceneRenderer::ClearRenderTarget
(
	SWAPCHAIN_SLOT CurrentSwapChainSlot
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


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

			assert( m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ] != nullptr );
			if ( m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ] == nullptr )
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


	assert( m_ppCommandLists[ CurrentSwapChainSlot ] != nullptr );
	if ( m_ppCommandLists[ CurrentSwapChainSlot ] == nullptr )
	{
		goto Cleanup;
	};


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
			L"CD3D12SceneRenderer::ClearRenderTarget:  Clearing render target view to (%f, %f, %f, %f).",
			BackgroundClearViewColor[ 0 ],
			BackgroundClearViewColor[ 1 ],
			BackgroundClearViewColor[ 2 ],
			BackgroundClearViewColor[ 3 ]
		)
	);

	m_ppCommandLists[ CurrentSwapChainSlot ]->ClearRenderTargetView
	(
		m_RenderTargetViews[ CurrentSwapChainSlot ],
		BackgroundClearViewColor,
		0,
		nullptr
	);


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  WaitForGPU
//
// Purpose:
// Waits for the GPU to catch up to work that has been
// issued.
///////////////////////////////////////////////////////
HRESULT CD3D12SceneRenderer::WaitForGPU()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Create the fence-completion event.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::WaitForGPU:  Creating fence-completion event."
	);

	HANDLE hFenceCompletionEvent = CreateEventEx
	(
		nullptr,
		FALSE,
		FALSE,
		EVENT_ALL_ACCESS
	);

	if ( hFenceCompletionEvent == nullptr )
	{
		DWORD CreateEventError = GetLastError();
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			CreateEventError,
			L"CreateEventEx failed."
		);

		hOverallResult = HRESULT_FROM_WIN32( CreateEventError );
		goto Cleanup;
	};


	//
	// Signal the fence with the current fence value.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"CD3D12SceneRenderer::WaitForGPU:  Signalling fence with value %I64u.",
			m_CurrentFrame
		)
	);

	// Signal the fence.
	const UINT64 CurrentFrameFenceValue = m_CurrentFrame;
	HRESULT hSignalFenceResult = m_pQueue_ID3D12CommandQueue->Signal
	(
		m_pCurrentFrameFence,
		CurrentFrameFenceValue
	);

	if ( FAILED( hSignalFenceResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hSignalFenceResult,
			L"Signal on command queue failed."
		);

		hOverallResult = hSignalFenceResult;
		goto Cleanup;
	};


	//
	// Increment the current fence value.
	//

	++m_CurrentFrame;


	//
	// Wait until the GPU reaches the fence.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::WaitForGPU:  Waiting for fence."
	);

	if ( m_pCurrentFrameFence->GetCompletedValue() >= CurrentFrameFenceValue )
	{
		// The GPU passed the fence before we got here, so we don't
		// need to wait.
		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12SceneRenderer::WaitForGPU:  The GPU's completion value has already passed the fence."
		);
	}
	else
	{
		// Have the fence signal the fence-completion event upon
		// completion of the specified fence value.
		HRESULT hSetCompletionEventResult = m_pCurrentFrameFence->SetEventOnCompletion
		(
			CurrentFrameFenceValue,
			hFenceCompletionEvent
		);

		if ( FAILED( hSetCompletionEventResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hSetCompletionEventResult,
				L"SetEventOnCompletion failed."
			);

			hOverallResult = hSetCompletionEventResult;
			goto Cleanup;
		};


		// Wait for the fence-completion event to be signalled.
		DWORD WaitResult = WaitForSingleObject
		(
			hFenceCompletionEvent,
			IsDebuggerPresent() == TRUE ? INFINITE : 30 * 1000
		);

		switch ( WaitResult )
		{
			case WAIT_OBJECT_0:
			{
				// The event was signalled as expected.
				LOG_MESSAGE
				(
					MESSAGE_TYPE_DEBUG,
					L"CD3D12SceneRenderer::WaitForGPU:  Fence-completion event was signalled."
				);
			} break;

			case WAIT_TIMEOUT:
			{
				hOverallResult = E_FAIL;
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hOverallResult,
					L"A single frame has taken longer than 30 seconds to render.  Something is desperately wrong."
				);

				goto Cleanup;
			} break;

			case WAIT_ABANDONED:
			case WAIT_FAILED:
			{
				hOverallResult = E_FAIL;
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hOverallResult,
					L"WaitForSingleObject returned an unexpected value."
				);

				goto Cleanup;
			} break;
		};
	};


	hOverallResult = S_OK;


Cleanup:
	if ( hFenceCompletionEvent != nullptr )
	{
		CloseHandle( hFenceCompletionEvent );
		hFenceCompletionEvent = nullptr;
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  RenderAndPresentOnce
//
// Purpose:
// Renders a single frame.
///////////////////////////////////////////////////////
HRESULT CD3D12SceneRenderer::RenderAndPresentOnce()
{
	return CD3D12SceneRenderer::RenderAndPresentOnce
	(
		1,
		m_PresentFlags
	);
};


HRESULT CD3D12SceneRenderer::RenderAndPresentOnce
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
	PresentParameters.pDirtyRects = nullptr;
	PresentParameters.pScrollRect = nullptr;
	PresentParameters.pScrollOffset = nullptr;

	return CD3D12SceneRenderer::RenderAndPresentOnce
	(
		SyncInterval,
		PresentFlags,
		&PresentParameters
	);
};


HRESULT CD3D12SceneRenderer::RenderAndPresentOnce
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

	ID3D12Resource * pCurrentSwapChainBuffer = nullptr;


	//
	// Animate the world matrix if needed.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::RenderAndPresentOnce:  Animating world matrix."
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
		D3DXMatrixTranspose( &m_pVertexShaderMatrices->m_World, &m_WorldMatrix );
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
		// Get a pointer to the current swap chain buffer.
		//

		HRESULT hGetCurrentBufferResult = m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ]->GetBuffer
		(
			m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ]->GetCurrentBackBufferIndex(),
			IID_ID3D12Resource,
			(void **) &pCurrentSwapChainBuffer
		);

		if ( FAILED( hGetCurrentBufferResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hGetCurrentBufferResult,
				L"GetBuffer on current buffer failed."
			);

			hOverallResult = hGetCurrentBufferResult;
			goto Cleanup;
		};


		//
		// Render scene-dependent content.
		//

		assert( m_ppRenderingTechniques[ m_SceneTypes[ CurrentSwapChainSlot ] ] != nullptr );
		HRESULT hRenderSceneResult = m_ppRenderingTechniques[ m_SceneTypes[ CurrentSwapChainSlot ] ]->RenderScene
		(
			CurrentSwapChainSlot,
			pCurrentSwapChainBuffer
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
		// Indicate that the current swap chain buffer will now be used to present.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12SceneRenderer::RenderAndPresentOnce:  Marking back buffer for presentation."
		);

		D3D12_RESOURCE_BARRIER RenderTargetPresentationResourceBarrier;
		ZeroMemory( &RenderTargetPresentationResourceBarrier, sizeof( RenderTargetPresentationResourceBarrier ) );
		RenderTargetPresentationResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		RenderTargetPresentationResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		RenderTargetPresentationResourceBarrier.Transition.pResource = pCurrentSwapChainBuffer;
		RenderTargetPresentationResourceBarrier.Transition.Subresource = 0;
		RenderTargetPresentationResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		RenderTargetPresentationResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

		m_ppCommandLists[ CurrentSwapChainSlot ]->ResourceBarrier
		(
			1,
			&RenderTargetPresentationResourceBarrier
		);


		//
		// Close the command list.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12SceneRenderer::RenderAndPresentOnce:  Closing command list."
		);

		HRESULT hCloseShadedCubeCommandListResult = m_ppCommandLists[ CurrentSwapChainSlot ]->Close();

		if ( FAILED( hCloseShadedCubeCommandListResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCloseShadedCubeCommandListResult,
				L"Close on command list failed."
			);

			hOverallResult = hCloseShadedCubeCommandListResult;
			goto Cleanup;
		};


		//
		// Execute the command list.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12SceneRenderer::RenderAndPresentOnce:  Executing command list."
		);

		ID3D12CommandList * CommandListsToExecute[] =
		{
			m_ppCommandLists[ CurrentSwapChainSlot ]
		};

		m_pQueue_ID3D12CommandQueue->ExecuteCommandLists
		(
			ARRAYSIZE( CommandListsToExecute ),
			CommandListsToExecute
		);


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
					L"CD3D12SceneRenderer::RenderAndPresentOnce:  Calling PresentBuffer."
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
					L"CD3D12SceneRenderer::RenderAndPresentOnce:  Calling Present."
				);

				// Present to the swap chain.
				assert( m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ] != nullptr );
				HRESULT hPresentResult = m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ]->Present
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
					L"CD3D12SceneRenderer::RenderAndPresentOnce:  Calling Present1."
				);

				// Present the frame.
				DXGI_PRESENT_PARAMETERS PresentParameters;
				ZeroMemory( &PresentParameters, sizeof( PresentParameters ) );

				assert( m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ] != nullptr );
				HRESULT hPresentResult = m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ]->Present1
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
					L"CD3D12SceneRenderer::RenderAndPresentOnce:  Unhandled swap chain type."
				);

				assert( false && L"Unhandled swap chain type." );

				hOverallResult = E_NOTIMPL;
				goto Cleanup;
			} break;
		};


		//
		// Wait for the frame to have been presented.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12SceneRenderer::RenderAndPresentOnce:  Waiting for frame presentation."
		);

		HRESULT hWaitForGPUResult = WaitForGPU();

		if ( FAILED( hWaitForGPUResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hWaitForGPUResult,
				L"WaitForGPU failed."
			);

			hOverallResult = hWaitForGPUResult;
			goto Cleanup;
		};


		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12SceneRenderer::RenderAndPresentOnce:  Presentation complete."
		);


		//
		// Release the current swap chain buffer.
		//

		SAFE_RELEASE( pCurrentSwapChainBuffer );
	};


	hOverallResult = S_OK;


Cleanup:
	//
	// Release the swap chain buffer.
	//

	SAFE_RELEASE( pCurrentSwapChainBuffer );


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
HRESULT CD3D12SceneRenderer::SetShaderConstants()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Initialize our matrices.
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
		MIN_DEPTH,
		MAX_DEPTH
	);


	//
	// Validate members and state.
	//

	// Check that the constant buffer upload heap is mapped.
	assert( m_pVertexShaderMatrices != nullptr );
	if ( m_pVertexShaderMatrices == nullptr )
	{
		hOverallResult = E_FAIL;

		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hOverallResult,
			L"Constant-buffer upload heap is not mapped."
		);

		goto Cleanup;
	};


	//
	// Copy constants into the constant buffer.
	//

	D3DXMatrixTranspose( &m_pVertexShaderMatrices->m_World, &m_WorldMatrix );
	D3DXMatrixTranspose( &m_pVertexShaderMatrices->m_View, &m_ViewMatrix );
	D3DXMatrixTranspose( &m_pVertexShaderMatrices->m_Projection, &m_ProjectionMatrix );

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::SetShaderConstants:  Stored world matrix:"
	);

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]",
			m_pVertexShaderMatrices->m_World._11,
			m_pVertexShaderMatrices->m_World._12,
			m_pVertexShaderMatrices->m_World._13,
			m_pVertexShaderMatrices->m_World._14,
			m_pVertexShaderMatrices->m_World._21,
			m_pVertexShaderMatrices->m_World._22,
			m_pVertexShaderMatrices->m_World._23,
			m_pVertexShaderMatrices->m_World._24,
			m_pVertexShaderMatrices->m_World._31,
			m_pVertexShaderMatrices->m_World._32,
			m_pVertexShaderMatrices->m_World._33,
			m_pVertexShaderMatrices->m_World._34,
			m_pVertexShaderMatrices->m_World._41,
			m_pVertexShaderMatrices->m_World._42,
			m_pVertexShaderMatrices->m_World._43,
			m_pVertexShaderMatrices->m_World._44
		)
	);


	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::SetShaderConstants:  Stored view matrix:"
	);

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]",
			m_pVertexShaderMatrices->m_View._11,
			m_pVertexShaderMatrices->m_View._12,
			m_pVertexShaderMatrices->m_View._13,
			m_pVertexShaderMatrices->m_View._14,
			m_pVertexShaderMatrices->m_View._21,
			m_pVertexShaderMatrices->m_View._22,
			m_pVertexShaderMatrices->m_View._23,
			m_pVertexShaderMatrices->m_View._24,
			m_pVertexShaderMatrices->m_View._31,
			m_pVertexShaderMatrices->m_View._32,
			m_pVertexShaderMatrices->m_View._33,
			m_pVertexShaderMatrices->m_View._34,
			m_pVertexShaderMatrices->m_View._41,
			m_pVertexShaderMatrices->m_View._42,
			m_pVertexShaderMatrices->m_View._43,
			m_pVertexShaderMatrices->m_View._44
		)
	);


	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::SetShaderConstants:  Stored projection matrix:"
	);

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]",
			m_pVertexShaderMatrices->m_Projection._11,
			m_pVertexShaderMatrices->m_Projection._12,
			m_pVertexShaderMatrices->m_Projection._13,
			m_pVertexShaderMatrices->m_Projection._14,
			m_pVertexShaderMatrices->m_Projection._21,
			m_pVertexShaderMatrices->m_Projection._22,
			m_pVertexShaderMatrices->m_Projection._23,
			m_pVertexShaderMatrices->m_Projection._24,
			m_pVertexShaderMatrices->m_Projection._31,
			m_pVertexShaderMatrices->m_Projection._32,
			m_pVertexShaderMatrices->m_Projection._33,
			m_pVertexShaderMatrices->m_Projection._34,
			m_pVertexShaderMatrices->m_Projection._41,
			m_pVertexShaderMatrices->m_Projection._42,
			m_pVertexShaderMatrices->m_Projection._43,
			m_pVertexShaderMatrices->m_Projection._44
		)
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
void CD3D12SceneRenderer::StartAnimation()
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
void CD3D12SceneRenderer::StopAnimation()
{
	m_bAnimating = false;
};
