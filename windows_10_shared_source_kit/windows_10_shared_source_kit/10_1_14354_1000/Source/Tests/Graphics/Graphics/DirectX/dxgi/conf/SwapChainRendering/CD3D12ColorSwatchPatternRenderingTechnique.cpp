//////////////////////////////////////////////////////////////////////
// File:  CD3D12ColorSwatchPatternRenderingTechnique.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a technique to render a pattern of colored
// swatches.
//
// History:
// 12 Aug 2015  SEdmison  Factored out of CD3D12SceneRenderer.cpp.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "CD3D12ColorSwatchPatternRenderingTechnique.hpp"
#include "CD3D12SceneRenderer.hpp"

// Pre-compiled shader blobs
#include "ShadedObjectVertexShader_5_1.h"
#include "ShadedObjectPixelShader_5_1.h"


/////////////////////////////////////////////////////////////////
//
// CD3D12ColorSwatchPatternRenderingTechnique implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Static initializers
//
////////////////////////////////////////////////////////////

COLOR_SWATCH CD3D12ColorSwatchPatternRenderingTechnique::s_BackgroundSwapChainColorSwatches[] =
{
	// White swatch in the upper-left 10% rown and column of the render target
	{
		{ 0.f, 0.f, .1f, .1f },
		{ 1.f, 1.f, 1.f, 1.f },
	},

	// Light gray swatch in the upper row, second column
	{
		{ .1f, 0.f, .2f, .1f },
		{ .75f, .75f, .75f, 1.f },
	},

	// Medium gray swatch in the upper row, third column
	{
		{ .2f, 0.f, .3f, .1f },
		{ .5f, .5f, .5f, 1.f },
	},

	// Dark gray swatch in the upper row, fourth column
	{
		{ .3f, 0.f, .4f, .1f },
		{ .25f, .25f, .25f, 1.f },
	},

	// Medium red swatch in the third row, first column
	{
		{ 0.f, .2f, .1f, .3f },
		{ .5f, 0.f, 0.f, 1.f },
	},

	// Medium green swatch in the third row, second column
	{
		{ .1f, .2f, .2f, .3f },
		{ 0.f, .5f, 0.f, 1.f },
	},

	// Medium blue swatch in the third row, fourth column
	{
		{ .2f, .2f, .3f, .3f },
		{ 0.f, 0.f, 0.5f, 1.f },
	},
};


COLOR_SWATCH CD3D12ColorSwatchPatternRenderingTechnique::s_ForegroundSwapChainColorSwatches[] =
{
	// White swatch in the second row, first column
	{
		{ 0.f, .1f, .1f, .2f },
		{ 1.f, 1.f, 1.f, 1.f },
	},

	// Light gray swatch in the second row, second column
	{
		{ .1f, .1f, .2f, .2f },
		{ .75f, .75f, .75f, 1.f },
	},

	// Medium gray swatch in the second row, third column
	{
		{ .2f, .1f, .3f, .2f },
		{ .5f, .5f, .5f, 1.f },
	},

	// Dark gray swatch in the second row, fourth column
	{
		{ .3f, .1f, .4f, .2f },
		{ .25f, .25f, .25f, 1.f },
	},

	// Medium red swatch in the fourth row, first column
	{
		{ 0.f, .3f, .1f, .4f },
		{ .5f, 0.f, 0.f, 1.f },
	},

	// Medium green swatch in the fourth row, second column
	{
		{ .1f, .3f, .2f, .4f },
		{ 0.f, .5f, 0.f, 1.f },
	},

	// Medium blue swatch in the fourth row, third column
	{
		{ .2f, .3f, .3f, .4f },
		{ 0.f, 0.f, 0.5f, 1.f },
	},

	// Medium red, half-opaque swatch in the firth row, first column
	{
		{ 0.f, .4f, .1f, .5f },
		{ .5f, 0.f, 0.f, .5f },
	},

	// Medium green, half-opaque swatch in the fifth row, second column
	{
		{ .1f, .4f, .2f, .5f },
		{ 0.f, .5f, 0.f, .5f },
	},

	// Medium blue, half-opaque swatch in the firth row, third column
	{
		{ .2f, .4f, .3f, .5f },
		{ 0.f, 0.f, 0.5f, .5f },
	},
};


////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D12ColorSwatchPatternRenderingTechnique
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CD3D12ColorSwatchPatternRenderingTechnique::CD3D12ColorSwatchPatternRenderingTechnique
(
	_In_ CD3D12SceneRenderer * pParentRenderer
)
:
	CD3D12RenderingTechnique( pParentRenderer ),
	m_pColorSwatchGraphicsPipelineState( nullptr )
{
};


///////////////////////////////////////////////////////
// Function:  ~CD3D12ColorSwatchPatternRenderingTechnique
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CD3D12ColorSwatchPatternRenderingTechnique::~CD3D12ColorSwatchPatternRenderingTechnique()
{
	assert( m_pColorSwatchGraphicsPipelineState == nullptr );
};


////////////////////////////////////////////////////////////
//
// Public method implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CreateResources
//
// Purpose:
// Creates and initializes resources for the color
// swatch pattern scene, including initializing the
// swap chain buffers with the color swatches.
///////////////////////////////////////////////////////
HRESULT CD3D12ColorSwatchPatternRenderingTechnique::CreateResources
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

	// Skip empty swap chain slots.
	if ( m_pParentRenderer->m_SwapChainTypes[ CurrentSwapChainSlot] == SWAPCHAIN_TYPE_NONE )
	{
		hOverallResult = S_FALSE;
		goto Cleanup;
	};


	//
	// Initialize swap chain buffers.
	//

	// We need already to have determined the swap chain buffer format.
	assert( m_pParentRenderer->m_SwapChainBufferFormats[ CurrentSwapChainSlot ] != DXGI_FORMAT_UNKNOWN );

	HRESULT hInitializeSwapChainBuffersResult = InitializeSwapChainBuffers
	(
		CurrentSwapChainSlot,
		m_pParentRenderer->m_SwapChainBufferFormats[ CurrentSwapChainSlot ],
		( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND )
			? DXGI_ALPHA_MODE_PREMULTIPLIED
			: DXGI_ALPHA_MODE_STRAIGHT
	);

	if ( FAILED( hInitializeSwapChainBuffersResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hInitializeSwapChainBuffersResult,
			L"InitializeSwapChainBuffers failed."
		);

		hOverallResult = hInitializeSwapChainBuffersResult;
		goto Cleanup;
	};


	//
	// Create the graphics pipeline state.
	//

	if ( m_pColorSwatchGraphicsPipelineState == nullptr )
	{
		//
		// Create the textured-object input layout.
		//

		D3D12_INPUT_ELEMENT_DESC ColorSwatchInputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};


		//
		// Create a color-swatch graphics pipeline state object.
		//

		D3D12_GRAPHICS_PIPELINE_STATE_DESC ColorSwatchGraphicsPipelineStateDescriptor;
		ZeroMemory( &ColorSwatchGraphicsPipelineStateDescriptor, sizeof( ColorSwatchGraphicsPipelineStateDescriptor ) );

		ColorSwatchGraphicsPipelineStateDescriptor.InputLayout = { ColorSwatchInputLayout, ARRAYSIZE( ColorSwatchInputLayout ) };
		ColorSwatchGraphicsPipelineStateDescriptor.pRootSignature = m_pParentRenderer->m_pRootSignature;
		ColorSwatchGraphicsPipelineStateDescriptor.VS = { g_ShadedObjectVertexShader, sizeof( g_ShadedObjectVertexShader ) };
		ColorSwatchGraphicsPipelineStateDescriptor.PS = { g_ShadedObjectPixelShader, sizeof( g_ShadedObjectPixelShader ) };

		ColorSwatchGraphicsPipelineStateDescriptor.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		ColorSwatchGraphicsPipelineStateDescriptor.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
		ColorSwatchGraphicsPipelineStateDescriptor.RasterizerState.FrontCounterClockwise = TRUE;
		ColorSwatchGraphicsPipelineStateDescriptor.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		ColorSwatchGraphicsPipelineStateDescriptor.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		ColorSwatchGraphicsPipelineStateDescriptor.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		ColorSwatchGraphicsPipelineStateDescriptor.RasterizerState.DepthClipEnable = FALSE;
		ColorSwatchGraphicsPipelineStateDescriptor.RasterizerState.MultisampleEnable = FALSE;
		ColorSwatchGraphicsPipelineStateDescriptor.RasterizerState.AntialiasedLineEnable = FALSE;
		ColorSwatchGraphicsPipelineStateDescriptor.RasterizerState.ForcedSampleCount = 0;
		ColorSwatchGraphicsPipelineStateDescriptor.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		ColorSwatchGraphicsPipelineStateDescriptor.BlendState.AlphaToCoverageEnable = FALSE;
		ColorSwatchGraphicsPipelineStateDescriptor.BlendState.IndependentBlendEnable = FALSE;
        for ( UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i )
		{
			ColorSwatchGraphicsPipelineStateDescriptor.BlendState.RenderTarget[ i ] = DEFAULT_RENDER_TARGET_BLEND_DESCRIPTOR;
		};

		ColorSwatchGraphicsPipelineStateDescriptor.DepthStencilState.DepthEnable = FALSE;
		ColorSwatchGraphicsPipelineStateDescriptor.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		ColorSwatchGraphicsPipelineStateDescriptor.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		ColorSwatchGraphicsPipelineStateDescriptor.DepthStencilState.StencilEnable = FALSE;
		ColorSwatchGraphicsPipelineStateDescriptor.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		ColorSwatchGraphicsPipelineStateDescriptor.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		ColorSwatchGraphicsPipelineStateDescriptor.DepthStencilState.FrontFace = DEFAULT_STENCIL_OPERATION;
		ColorSwatchGraphicsPipelineStateDescriptor.DepthStencilState.BackFace = DEFAULT_STENCIL_OPERATION;

		ColorSwatchGraphicsPipelineStateDescriptor.SampleMask = UINT_MAX;
		ColorSwatchGraphicsPipelineStateDescriptor.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		ColorSwatchGraphicsPipelineStateDescriptor.NumRenderTargets = 1;
		//ColorSwatchGraphicsPipelineStateDescriptor.RTVFormats[ 0 ] = DXGI_FORMAT_R8G8B8A8_UNORM; // @REVIEW
		ColorSwatchGraphicsPipelineStateDescriptor.RTVFormats[ 0 ] = DXGI_FORMAT_B8G8R8A8_UNORM; // @REVIEW
		ColorSwatchGraphicsPipelineStateDescriptor.SampleDesc.Count = 1;

		assert( m_pColorSwatchGraphicsPipelineState == nullptr );
		HRESULT hCreateColorSwatchGraphicsPipelineStateResult = m_pParentRenderer->m_pDevice_ID3D12Device->CreateGraphicsPipelineState
		(
			&ColorSwatchGraphicsPipelineStateDescriptor,
			IID_ID3D12PipelineState,
			(void **) &m_pColorSwatchGraphicsPipelineState
		);

		if ( FAILED( hCreateColorSwatchGraphicsPipelineStateResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateColorSwatchGraphicsPipelineStateResult,
				L"CreateGraphicsPipelineState failed."
			);

			hOverallResult = hCreateColorSwatchGraphicsPipelineStateResult;
			goto Cleanup;
		};
	};


	//
	// Create a command list.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		L"CD3D12ColorSwatchPatternRenderingTechnique::CreateResources:  Creating command list."
	);

	HRESULT hCreateCommandListResult = m_pParentRenderer->CreateCommandList
	(
		CurrentSwapChainSlot,
		m_pColorSwatchGraphicsPipelineState
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


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  DestroyResources
//
// Purpose:
///////////////////////////////////////////////////////
void CD3D12ColorSwatchPatternRenderingTechnique::DestroyResources()
{
	SAFE_RELEASE( m_pColorSwatchGraphicsPipelineState );
};


///////////////////////////////////////////////////////
// Function:  RenderScene
//
// Purpose:
// Renders color swatches onto the swap chain's buffer
// by calling ClearView on a render target view of the
// buffer with various rectangles and colors.
///////////////////////////////////////////////////////
HRESULT CD3D12ColorSwatchPatternRenderingTechnique::RenderScene
(
	SWAPCHAIN_SLOT CurrentSwapChainSlot,
	_In_ ID3D12Resource * pCurrentSwapChainBuffer
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	RECT WindowClientRectangle = { 0, 0, 0, 0 };


	//
	// Validate members and state.
	//

	assert( m_pParentRenderer->m_pQueue_ID3D12CommandQueue != nullptr );
	if ( m_pParentRenderer->m_pQueue_ID3D12CommandQueue == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"m_pQueue_ID3D12CommandQueue is NULL."
		);

		goto Cleanup;
	};


	//
	// Reset the command list.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12ClearRenderingTechnique::RenderScene:  Resetting command list."
	);

	HRESULT hResetCommandListResult = m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->Reset
	(
		m_pParentRenderer->m_ppCommandAllocators[ CurrentSwapChainSlot ],
		m_pColorSwatchGraphicsPipelineState
	);

	if ( FAILED( hResetCommandListResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hResetCommandListResult,
			L"Reset on command list failed."
		);

		//goto Cleanup;
	};


	switch ( m_pParentRenderer->m_SwapChainTypes[ CurrentSwapChainSlot ] )
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

			assert( m_pParentRenderer->m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ] != nullptr );
			if ( m_pParentRenderer->m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ] == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_BOOL,
					false,
					L"Swap chain pointer is NULL."
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
	// Create a render target view of the swap chain's current buffer.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12ColorSwatchPatternRenderingTechnique::RenderScene:  Creating render target view."
	);

	m_pParentRenderer->m_RenderTargetViews[ CurrentSwapChainSlot ] = m_pParentRenderer->m_pRenderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pParentRenderer->m_RenderTargetViews[ CurrentSwapChainSlot ].ptr += ( (size_t) CurrentSwapChainSlot ) * (size_t) m_pParentRenderer->m_pDevice_ID3D12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );

	m_pParentRenderer->m_pDevice_ID3D12Device->CreateRenderTargetView
	(
		pCurrentSwapChainBuffer,
		nullptr,
		m_pParentRenderer->m_RenderTargetViews[ CurrentSwapChainSlot ]
	);


	//
	// Add scene-independent commands to the command list.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12ColorSwatchPatternRenderingTechnique::RenderScene:  Populating command list."
	);

	// Set descriptor heaps.
	ID3D12DescriptorHeap * DescriptorHeaps[] =
	{
		m_pParentRenderer->m_pConstantBufferViewDescriptorHeap
	};

	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->SetDescriptorHeaps
	(
		ARRAYSIZE( DescriptorHeaps ),
		DescriptorHeaps
	);


	// Set the graphics root signature.
	assert( m_pParentRenderer->m_pRootSignature != nullptr );
	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->SetGraphicsRootSignature( m_pParentRenderer->m_pRootSignature );


	// Set the root descriptor table.
	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->SetGraphicsRootDescriptorTable
	(
		0,
		m_pParentRenderer->m_pConstantBufferViewDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);


	// Set the viewport.
	D3D12_VIEWPORT Viewport;
	ZeroMemory( &Viewport, sizeof( Viewport ) );
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.Width = static_cast< float >( m_pParentRenderer->m_SceneWidth );
	Viewport.Height = static_cast< float >( m_pParentRenderer->m_SceneHeight );
	Viewport.MinDepth = MIN_DEPTH;
	Viewport.MaxDepth = MAX_DEPTH;

	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->RSSetViewports
	(
		1,
		&Viewport
	);


	// Set the scissor rectangle.
	RECT ScissorRectangle;
	ScissorRectangle.left = 0;
	ScissorRectangle.top = 0;
	ScissorRectangle.right = static_cast< LONG >( m_pParentRenderer->m_SceneWidth );
	ScissorRectangle.bottom = static_cast< LONG >( m_pParentRenderer->m_SceneHeight );

	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->RSSetScissorRects
	(
		1,
		&ScissorRectangle
	);


	//
	// Indicate that the back buffer will be used as a render target.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12ColorSwatchPatternRenderingTechnique::RenderScene:  Setting back buffer as render target."
	);

	D3D12_RESOURCE_BARRIER RenderTargetRenderingResourceBarrier;
	ZeroMemory( &RenderTargetRenderingResourceBarrier, sizeof( RenderTargetRenderingResourceBarrier ) );
	RenderTargetRenderingResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	RenderTargetRenderingResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	RenderTargetRenderingResourceBarrier.Transition.pResource = pCurrentSwapChainBuffer;
	RenderTargetRenderingResourceBarrier.Transition.Subresource = 0;
	RenderTargetRenderingResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	RenderTargetRenderingResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->ResourceBarrier
	(
		1,
		&RenderTargetRenderingResourceBarrier
	);


	//
	// Set the render target view.
	//

	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->OMSetRenderTargets
	(
		1,
		&m_pParentRenderer->m_RenderTargetViews[ CurrentSwapChainSlot ],
		FALSE,
		nullptr
	);


	//
	// Clear the render target for this swap chain.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12ColorSwatchPatternRenderingTechnique::RenderScene:  Clearing render target."
	);

	HRESULT hClearRenderTargetResult = m_pParentRenderer->ClearRenderTarget
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


#if USE_DEPTH_STENCIL
	//
	// Clear the depth stencil view.
	//

	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->ClearDepthStencilView
	(
		m_DepthStencilViewDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH,
		1.f,
		0,
		0,
		nullptr
	);
#endif


	//
	// Convert normalized RGBA buffer colors to a colors that
	// can be passed to ClearView.
	//

	float BackgroundClearViewColor[ 4 ] = { 0.f, 0.f, 0.f, 1.f };
	HRESULT hInitializeBackgroundClearViewColorResult = InitializeClearViewColor
	(
		m_pParentRenderer->m_SwapChainBufferFormats[ CurrentSwapChainSlot ],
		( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? DXGI_ALPHA_MODE_PREMULTIPLIED : DXGI_ALPHA_MODE_STRAIGHT,
		CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ],
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
			CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].r,
			CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].g,
			CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].b,
			CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].a,
			BackgroundClearViewColor[ 0 ],
			BackgroundClearViewColor[ 1 ],
			BackgroundClearViewColor[ 2 ],
			BackgroundClearViewColor[ 3 ]
		)
	);


	//
	// Clear the entire render target to the background color.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"  - Clearing render target to (%f, %f, %f, %f).",
			BackgroundClearViewColor[ 0 ],
			BackgroundClearViewColor[ 1 ],
			BackgroundClearViewColor[ 2 ],
			BackgroundClearViewColor[ 3 ]
		)
	);

	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->ClearRenderTargetView
	(
		m_pParentRenderer->m_RenderTargetViews[ CurrentSwapChainSlot ],
		BackgroundClearViewColor,
		0,
		nullptr
	);


	//
	// Fill color swatches in the buffer.
	//

	UINT SwatchCount = 0;
	if ( CurrentSwapChainSlot == SWAPCHAIN_SLOT_BACKGROUND )
	{
		SwatchCount = ARRAYSIZE( s_BackgroundSwapChainColorSwatches );
	}
	else
	{
		SwatchCount = ARRAYSIZE( s_ForegroundSwapChainColorSwatches );
	};

	for
	(
		UINT iCurrentSwatch = 0;
		iCurrentSwatch < SwatchCount;
		++iCurrentSwatch
	)
	{
		//
		// Convert the color if needed.
		//

		float SwatchClearViewColor[ 4 ] = { 0.f, 0.f, 0.f, 1.f };

		assert
		(
			CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND && iCurrentSwatch < ARRAYSIZE( s_ForegroundSwapChainColorSwatches ) ||
			CurrentSwapChainSlot == SWAPCHAIN_SLOT_BACKGROUND && iCurrentSwatch < ARRAYSIZE( s_BackgroundSwapChainColorSwatches )
		);

#pragma warning( suppress:  26014 )
		HRESULT hInitializeSwatchClearViewColorResult = InitializeClearViewColor
		(
			m_pParentRenderer->m_SwapChainBufferFormats[ CurrentSwapChainSlot ],
			( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? DXGI_ALPHA_MODE_PREMULTIPLIED : DXGI_ALPHA_MODE_STRAIGHT,
			( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].Color : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color,
			SwatchClearViewColor
		);

		if ( FAILED( hInitializeSwatchClearViewColorResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hInitializeSwatchClearViewColorResult,
				L"InitializeClearViewColor on swatch color failed."
			);

			goto Cleanup;
		};


#pragma warning( suppress:  26014 )
		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			WEX::Common::String().Format
			(
				L"  - Converted swatch %u color for %s swap chain from normalized RGB color (%f, %f, %f, %f) to clear color (%f, %f, %f, %f).",
				iCurrentSwatch,
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? L"foreground" : L"background",
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].Color.r : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.r,
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].Color.g : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.g,
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].Color.b : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.b,
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].Color.a : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.a,
				SwatchClearViewColor[ 0 ],
				SwatchClearViewColor[ 1 ],
				SwatchClearViewColor[ 2 ],
				SwatchClearViewColor[ 3 ]
			)
		);


		//
		// Clear a rectangle in the buffer to the swatch color.
		//

		RECT ClearViewRectangle;
		ZeroMemory( &ClearViewRectangle, sizeof( ClearViewRectangle ) );

#pragma warning( suppress:  26014 )
		ClearViewRectangle.left = (LONG) ( ( ( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.left : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.left ) * m_pParentRenderer->m_SceneWidth );

#pragma warning( suppress:  26014 )
		ClearViewRectangle.top = (LONG) ( ( ( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.top : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.top ) * m_pParentRenderer->m_SceneHeight );

#pragma warning( suppress:  26014 )
		ClearViewRectangle.right = (LONG) ( ( ( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.right : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.right ) * m_pParentRenderer->m_SceneWidth );

#pragma warning( suppress:  26014 )
		ClearViewRectangle.bottom = (LONG) ( ( ( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.bottom : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.bottom ) * m_pParentRenderer->m_SceneHeight );

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			WEX::Common::String().Format
			(
				L"  - Clearing swatch (%li, %li) to (%li, %li) within %s render target with color (%f, %f, %f, %f).",
				ClearViewRectangle.left,
				ClearViewRectangle.top,
				ClearViewRectangle.right,
				ClearViewRectangle.bottom,
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? L"foreground" : L"background",
				SwatchClearViewColor[ 0 ],
				SwatchClearViewColor[ 1 ],
				SwatchClearViewColor[ 2 ],
				SwatchClearViewColor[ 3 ]
			)
		);

		m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->ClearRenderTargetView
		(
			m_pParentRenderer->m_RenderTargetViews[ CurrentSwapChainSlot ],
			SwatchClearViewColor,
			1,
			&ClearViewRectangle
		);
	};


	// If we made it this far, then we're good to go.
	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


////////////////////////////////////////////////////////////
//
// Helper method implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  InitializeSwapChainBuffers
//
// Purpose:
// Initializes swap chain buffer texture array slices.
///////////////////////////////////////////////////////
HRESULT CD3D12ColorSwatchPatternRenderingTechnique::InitializeSwapChainBuffers
(
	SWAPCHAIN_SLOT CurrentSwapChainSlot,
	DXGI_FORMAT SwapChainBufferFormat,
	DXGI_ALPHA_MODE AlphaMode
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	IDXGIResource * pSwapChainBufferBufferArray_IDXGIResource = nullptr;

	GUID VideoDecoderProfileID;
	ZeroMemory( &VideoDecoderProfileID, sizeof( VideoDecoderProfileID ) );

	BOOL bFoundSupportedVideoDecoderProfile = FALSE;

#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
	ID3D11VideoDecoderOutputView * pVideoDecoderOutputView = nullptr;
#endif


	//
	// Validate members and state.
	//

	// This work is only needed for decode swap chains.
	if ( m_pParentRenderer->m_SwapChainTypes[ CurrentSwapChainSlot ] != SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE )
	{
		hOverallResult = S_FALSE;
		goto Cleanup;
	};


#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
	//
	// Get the video decoder profile count.
	//

	assert( m_pDevice_ID3D11VideoDevice != nullptr );
	const UINT VideoDecoderProfileCount = m_pParentRenderer->m_pDevice_ID3D11VideoDevice->GetVideoDecoderProfileCount();


	//
	// Find a decoder profile to use.
	//

	// If the count is 0, and we call GetVideoDecoderProfile, the
	// runtime will crash, so we'll fail instead.
	if ( 0 == VideoDecoderProfileCount )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"GetVideoDecoderProfileCount returned 0."
		);

		goto Cleanup;
	};

	for
	(
		UINT iCurrentVideoDecoderProfile = 0;
		iCurrentVideoDecoderProfile < VideoDecoderProfileCount &&
			bFoundSupportedVideoDecoderProfile != TRUE;
		++iCurrentVideoDecoderProfile
	)
	{
		HRESULT hGetVideoDecoderProfileResult = m_pParentRenderer->m_pDevice_ID3D11VideoDevice->GetVideoDecoderProfile
		(
			iCurrentVideoDecoderProfile,
			&VideoDecoderProfileID
		);

		if ( FAILED( hGetVideoDecoderProfileResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hGetVideoDecoderProfileResult,
				L"GetVideoDecoderProfile failed."
			);

			goto Cleanup;
		};

		HRESULT hCheckVideoDecoderFormatResult = m_pParentRenderer->m_pDevice_ID3D11VideoDevice->CheckVideoDecoderFormat
		(
			&VideoDecoderProfileID,
			SwapChainBufferFormat,
			&bFoundSupportedVideoDecoderProfile
		);

		if ( FAILED( hCheckVideoDecoderFormatResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCheckVideoDecoderFormatResult,
				L"CheckVideoDecoderFormat failed."
			);

			goto Cleanup;
		};
	};


	// If we got here and we still haven't found a supported
	// video decoder profile, something is very wrong, because
	// CheckFormatSupport indicated DECODER_OUTPUT support.
	if ( bFoundSupportedVideoDecoderProfile != TRUE )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"CheckFormatSupport indicated decoder output support for this format, but no supported video decoder profile was found."
		);

		goto Cleanup;
	};


	//
	// Log the decoder profile that we selected.
	//
	// N.B. [SEdmison]:  These IDs are defined in dxva2api.h.
	//

	WCHAR GUIDFormattingBuffer[ 64 ] = L"";
	int nGUIDFormattingBuffer = StringFromGUID2
	(
		VideoDecoderProfileID,
		GUIDFormattingBuffer,
		ARRAYSIZE( GUIDFormattingBuffer )
	);

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"- Using decoder profile %s.",
			GUIDFormattingBuffer
		)
	);


	//
	// Create video decoder views on the swap chain decode buffers.
	//

	for
	(
		UINT iCurrentSwapChainBuffer = 0;
		iCurrentSwapChainBuffer < m_pParentRenderer->m_cSwapChainBufferTextureArraySlices[ CurrentSwapChainSlot ];
		++iCurrentSwapChainBuffer
	)
	{
		//
		// Convert normalized RGBA buffer colors to a colors that
		// can be passed to ClearView.
		//

		float BackgroundClearViewColor[ 4 ] = { 0.f, 0.f, 0.f, 1.f };
		HRESULT hInitializeBackgroundClearViewColorResult = InitializeClearViewColor
		(
			SwapChainBufferFormat,
			AlphaMode,
			CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ],
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
				CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].r,
				CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].g,
				CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].b,
				CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].a,
				BackgroundClearViewColor[ 0 ],
				BackgroundClearViewColor[ 1 ],
				BackgroundClearViewColor[ 2 ],
				BackgroundClearViewColor[ 3 ]
			)
		);


		//
		// Create a video output view for this array slice.
		//

		D3D11_VIDEO_DECODER_OUTPUT_VIEW_DESC VideoOutputViewDescriptor;
		ZeroMemory( &VideoOutputViewDescriptor, sizeof( VideoOutputViewDescriptor ) );
		VideoOutputViewDescriptor.DecodeProfile = VideoDecoderProfileID;
		VideoOutputViewDescriptor.ViewDimension = D3D11_VDOV_DIMENSION_TEXTURE2D;
		VideoOutputViewDescriptor.Texture2D.ArraySlice = iCurrentSwapChainBuffer;


		assert( m_pDevice_ID3D11VideoDevice != nullptr );
		assert( pVideoDecoderOutputView == nullptr );
		HRESULT hCreateVideoDecoderOutputViewResult = m_pParentRenderer->m_pDevice_ID3D11VideoDevice->CreateVideoDecoderOutputView
		(
			m_ppSwapChainBufferTextureArrays[ CurrentSwapChainSlot ],
			&VideoOutputViewDescriptor,
			&pVideoDecoderOutputView
		);

		if ( FAILED( hCreateVideoDecoderOutputViewResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateVideoDecoderOutputViewResult,
				L"CreateVideoDecoderOutputView failed."
			);

			goto Cleanup;
		};

		if ( pVideoDecoderOutputView == nullptr )
		{
			LOG_ERROR
			(
				ERROR_TYPE_BOOL,
				false,
				L"CreateVideoDecoderOutputView succeeded but returned NULL video decoder output view pointer."
			);

			goto Cleanup;
		};


		//
		// Clear the entire buffer to the background color.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"  - Clearing buffer %u to (%f, %f, %f, %f).",
				iCurrentSwapChainBuffer,
				BackgroundClearViewColor[ 0 ],
				BackgroundClearViewColor[ 1 ],
				BackgroundClearViewColor[ 2 ],
				BackgroundClearViewColor[ 3 ]
			)
		);

		assert( m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1 != nullptr );
		m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->ClearView
		(
			pVideoDecoderOutputView,
			BackgroundClearViewColor,
			nullptr,
			0
		);


		//
		// Fill color swatches in the buffer.
		//

		for
		(
			UINT iCurrentSwatch = 0;
			iCurrentSwatch < ARRAYSIZE( s_BackgroundSwapChainColorSwatches );
			++iCurrentSwatch
		)
		{
			//
			// Convert the color if needed.
			//

			float SwatchClearViewColor[ 4 ] = { 0.f, 0.f, 0.f, 1.f };
			HRESULT hInitializeSwatchClearViewColorResult = InitializeClearViewColor
			(
				SwapChainBufferFormat,
				AlphaMode,
				s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color,
				SwatchClearViewColor
			);

			if ( FAILED( hInitializeSwatchClearViewColorResult ) )
			{
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hInitializeSwatchClearViewColorResult,
					L"InitializeClearViewColor on swatch color failed."
				);

				goto Cleanup;
			};


			LOG_MESSAGE
			(
				MESSAGE_TYPE_DEBUG,
				WEX::Common::String().Format
				(
					L"  - Converted swatch %u color for %s swap chain buffer %u from normalized RGB color (%f, %f, %f, %f) to clear color (%f, %f, %f, %f).",
					iCurrentSwatch,
					( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? L"foreground" : L"background",
					iCurrentSwapChainBuffer,
					s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.r,
					s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.g,
					s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.b,
					s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.a,
					SwatchClearViewColor[ 0 ],
					SwatchClearViewColor[ 1 ],
					SwatchClearViewColor[ 2 ],
					SwatchClearViewColor[ 3 ]
				)
			);


			//
			// Clear a rectangle in the buffer to the swatch color.
			//

			RECT ClearViewRectangle;
			ZeroMemory( &ClearViewRectangle, sizeof( ClearViewRectangle ) );
			ClearViewRectangle.left = (LONG) ( s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.left * m_pParentRenderer->m_SceneWidth );
			ClearViewRectangle.top = (LONG) ( s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.top * m_pParentRenderer->m_SceneHeight );
			ClearViewRectangle.right = (LONG) ( s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.right * m_pParentRenderer->m_SceneWidth );
			ClearViewRectangle.bottom = (LONG) ( s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.bottom * m_pParentRenderer->m_SceneHeight );

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				WEX::Common::String().Format
				(
					L"  - Clearing swatch (%li, %li) to (%li, %li) within swap chain buffer %u to (%f, %f, %f, %f).",
					ClearViewRectangle.left,
					ClearViewRectangle.top,
					ClearViewRectangle.right,
					ClearViewRectangle.bottom,
					iCurrentSwapChainBuffer,
					SwatchClearViewColor[ 0 ],
					SwatchClearViewColor[ 1 ],
					SwatchClearViewColor[ 2 ],
					SwatchClearViewColor[ 3 ]
				)
			);

			assert( m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1 != nullptr );
			m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->ClearView
			(
				pVideoDecoderOutputView,
				SwatchClearViewColor,
				&ClearViewRectangle,
				1
			);
		};


		SAFE_RELEASE( pVideoDecoderOutputView );
	};


	hOverallResult = S_OK;
#endif


Cleanup:
#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
	SAFE_RELEASE( pVideoDecoderOutputView );
#endif

	return hOverallResult;
};
