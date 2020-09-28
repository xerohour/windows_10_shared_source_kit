//////////////////////////////////////////////////////////////////////
// File:  CD3D12ClearRenderingTechnique.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a Direct3D 12 rendering technique that clears the
// render target to a solid color.
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

#include "CD3D12ClearRenderingTechnique.hpp"
#include "CD3D12SceneRenderer.hpp"

// Pre-compiled shader blobs
#include "ShadedObjectVertexShader_5_1.h"
#include "ShadedObjectPixelShader_5_1.h"


/////////////////////////////////////////////////////////////////
//
// CD3D12ClearRenderingTechnique implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D12ClearRenderingTechnique
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CD3D12ClearRenderingTechnique::CD3D12ClearRenderingTechnique
(
	_In_ CD3D12SceneRenderer * pParentRenderer
)
:
	CD3D12RenderingTechnique( pParentRenderer ),
	m_pColorSwatchGraphicsPipelineState( nullptr )
{
};


///////////////////////////////////////////////////////
// Function:  ~CD3D12ClearRenderingTechnique
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CD3D12ClearRenderingTechnique::~CD3D12ClearRenderingTechnique()
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
// Creates and initializes resources for the rendering
// technique.
///////////////////////////////////////////////////////
HRESULT CD3D12ClearRenderingTechnique::CreateResources
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
	if ( m_pParentRenderer->m_SwapChainTypes[ CurrentSwapChainSlot ] == SWAPCHAIN_TYPE_NONE )
	{
		hOverallResult = S_FALSE;
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
		L"CD3D12ClearRenderingTechnique::CreateResources:  Creating command list."
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
void CD3D12ClearRenderingTechnique::DestroyResources()
{
	SAFE_RELEASE( m_pColorSwatchGraphicsPipelineState );
};


///////////////////////////////////////////////////////
// Function:  RenderScene
//
// Purpose:
// Renders a single solid color to the swap chain's
// buffer via a single call to ClearRenderTargetView.
///////////////////////////////////////////////////////
HRESULT CD3D12ClearRenderingTechnique::RenderScene
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
		L"CD3D12ClearRenderingTechnique::RenderScene:  Creating render target view."
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
		L"CD3D12RenderingTechnique::RenderScene:  Populating command list."
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
		L"CD3D12ClearRenderingTechnique::RenderScene:  Setting back buffer as render target."
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
		L"CD3D12ClearRenderingTechnique::RenderScene:  Clearing render target."
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
		m_pParentRenderer->m_DepthStencilViewDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH,
		1.f,
		0,
		0,
		nullptr
	);
#endif


	// If we made it this far, then we're good to go.
	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};
