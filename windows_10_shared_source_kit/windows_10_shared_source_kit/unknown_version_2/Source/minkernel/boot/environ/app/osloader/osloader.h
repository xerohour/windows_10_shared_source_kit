r.RasterizerState.DepthClipEnable = FALSE;
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
/******************************Module*Header*******************************
* Module Name: main.cpp
*
* Implementation of a test app that invokes the VidMm
* memory segment CPU access suspension.
*
* Author: Steve Pronovost [spronovo]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#include "basevid.h"
#include "Sleepnode.h"

void Go();

class CSuspendCpuAccessTest : public CGenericI3DComponent
{
public:
    CSuspendCpuAccessTest();
    
protected:
    virtual void RunTest();
    virtual void EndTest();
};


void __cdecl main()
{
    try
    {
        Go();
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void Go()
{
    CTestRunner Runner;
    bool bBVT = false;

    //Set assertion guid
    Runner.SetTestGUID(DISPLAY_GENERIC_GUID);

    Runner.SetRootComponentDesc(L"This test invokes the VidMm CPU access suspension functionnality");
    
    Runner.SetTestName(L"LDDM Suspend CPU Access");
    
    Runner.DescribeCommandLineUsage(L"BVT", L"Set to run in BVT suite");
    
    if(Runner.IsCmdLineSet(L"BVT"))
    {
        bBVT = true;
    }
 
    Runner.AddNode<CI3DDeviceComponent>(L"Device");
    SetupDeviceCommandLine(Runner, L"Device");
    Runner.SetParamTestDomainSingleValue<bool>(L"Device", L"Windowed", true);

    UINT TimesToExecute = 60;

    if(bBVT)
    {
        TimesToExecute = 10;
    }

    Runner.AddNode<CLoop>                (L"Device/Loop");
    Runner.SetParamTestDomainRange< UINT >(L"Device/Loop", L"Index", 0, TimesToExecute, 1 ); //execute the loop 100 times

    Runner.AddNode<CSleepNode>            (L"Device/Loop/Sleep");
    Runner.SetParamTestDomainSingleValue<UINT>(L"Device/Loop/Sleep", L"SleepTime", 3000 );  // 3 sec

    // Test runs for nLoop*nSleepTime msec. Under the current settnings the component runs for 3 min.

    Runner.AddNode<CSuspendCpuAccessTest>(L"Device/Loop/SuspendCpuAccess");
    
    Runner.SetParamTestDomainSingleValue<bool>(L"Device/Loop/SuspendCpuAccess", L"BVT", bBVT);

    Runner.Run();
}

CSuspendCpuAccessTest::CSuspendCpuAccessTest()
{
    SetDescription(L"This test invokes the VidMm CPU access suspension functionnality");
    AddParam<bool>(L"BVT", L"Will run in bvt mode");
}

void CSuspendCpuAccessTest::RunTest()
{
    D3DKMT_VIDMM_ESCAPE VidMMEscape;
    memset(&VidMMEscape, 0, sizeof(VidMMEscape));
    VidMMEscape.Type = D3DKMT_VIDMMESCAPETYPE_SUSPEND_CPU_ACCESS_TEST;
    
    D3DKMT_ESCAPETYPE EscapeType = D3DKMT_ESCAPE_VIDMM;
    void* pPrivateDriverData = &VidMMEscape;
    UINT nPrivateDriverDataSize = sizeof(VidMMEscape);

    D3DDDI_ESCAPEFLAGS Flags;
    Flags.Value = 0;
    Flags.HardwareAccess = 1;
    
    HRESULT hr = m_pDevice->Escape(EscapeType,
                                   Flags,
                                   pPrivateDriverData,
                                   nPrivateDriverDataSize);

    if(FAILED(hr))
    {
        if(hr != D3DDDIERR_WASSTILLDRAWING)
        {
            std::wstring strError = L"Suspend CPU Access test failed hr = ";
            strError += ToString(hr);
            m_pLog->Fail(strError.c_str());
        }
    }
}


void CSuspendCpuAccessTest::EndTest()
{
}

/*++

Copyright (c) Microsoft Corporation

Module Name:

    osloader.h

Abstract:

    Header file for the Windows OS Loader.

Environment:

    Boot

--*/

#pragma once

//
// ------------------------------------------------------------------- Includes
//

#include <bootlib.h>
#include "baerror.h"
#include "msg.h"
#include "bootstatus.h"
#include <ntacpi.h>
#include <ntverp.h>
#include <ntiodump.h>
#include <hdlsblk.h>
#include <resminfo.h>
#include <wmdinfo.h>
#include <sqminfo.h>
#include "inbv.h"
#include "bgfx.h"
#include <sha.h>
#include <ntstrsafe.h>
#include <symcrypt.h>
#include <esrt.h>
#include <secureboot.h>
#include <apisetp.h>
#include <mfgmode.h>
#include "bltracelogging.h"
#include <blblobsealing.h>
#include "hypercall.h"

#undef ROUND_UP            // hive.h redefines ROUND_UP.
#include "cmp.h"

// VSM IDK headers.
#include <bcrypt.h>
#include <vsmidk.h>
#include <BlVsmKeys.h>
#include <blsvn.h>
#include <vsml.h>

//
// ---------------------------------------------------------------- Definitions
//

#define KERNEL_DEFAULT_NAME L"ntoskrnl.exe"
#define KERNEL_PAE_NAME     KERNEL_DEFAULT_NAME
#define HAL_DEFAULT_NAME    L"hal.dll"
#define KD_DEFAULT_NAME     L"kd.dll"
#define KD_SERIAL_NAME      L"kdcom.dll"
#define KD_1394_NAME        L"kd1394.dll"
#define KD_USB_NAME         L"kdusb.dll"
#define KD_NET_NAME         L"kdnet.dll"
#define KD_LOCAL_NAME       L"kd.dll"
#define KD_FRIENDLY_NAME    KD_SERIAL_NAME
#define KD_EXTENSION_FRIENDLY_NAME KD_EXTENSION_DEFAULT_NAME
#define KD_HV_COM_NAME      L"kdhvcom.dll"
#define KD_HV_1394_NAME     L"kdhv1394.dll"
#define UPDATE_DEFAULT_NAME L"mcupdate.dll"
#define UPDATE_NAME_PREFIX  L"mcupdate_"
#define DRVMAIN_NAME        L"drvmain.sdb"
#define ACPI_TABLE_NAME     L"acpitabl.dat"
#define SYSTEM_PATH         L"system32"
#define DRVMAIN_PATH        L"AppPatch"
#define INF_PATH            L"inf"
#define SYSTEM_HIVE_PATH    L"system32\\config\\"
#define SYSTEM_HIVE_NAME    L"system"
#define SYSTEM_HIVE_LOGNAME1 L"system.log1"
#define SYSTEM_HIVE_LOGNAME2 L"system.log2"
#define FPSWA_NAME          L"Drivers\\fpswa.efi"
#define HIVE_LOG1_EXTENSION L".LOG1"
#define HIVE_LOG2_EXTEN