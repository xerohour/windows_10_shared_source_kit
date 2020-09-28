////////////////
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
	switch ( m_Scen