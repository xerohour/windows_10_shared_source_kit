and initializes resources for the color
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
		ColorSwatchGraphicsPipelineStateDescripto