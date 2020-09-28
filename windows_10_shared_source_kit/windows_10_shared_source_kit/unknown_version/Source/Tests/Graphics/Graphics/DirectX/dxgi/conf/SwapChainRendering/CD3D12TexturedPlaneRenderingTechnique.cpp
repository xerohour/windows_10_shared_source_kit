//////////////////////////////////////////////////////////////////////
// File:  CD3D12TexturedPlaneRenderingTechnique.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of technique to render a textured plane.
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

#include "CD3D12TexturedPlaneRenderingTechnique.hpp"
#include "CD3D12SceneRenderer.hpp"

// Pre-compiled shader blobs
#include "TexturedObjectVertexShader_5_1.h"
#include "TexturedObjectPixelShader_5_1.h"


/////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Structure:  TexturedVertex
//
// Purpose:
// Holds position information and (u, v) texture
// coordinates for a vertex.
///////////////////////////////////////////////////////
struct TexturedVertex
{
	D3DXVECTOR3 Position;
	D3DXVECTOR2 TextureCoordinate;
};


/////////////////////////////////////////////////////////////////
//
// CD3D12TexturedPlaneRenderingTechnique implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D12TexturedPlaneRenderingTechnique
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CD3D12TexturedPlaneRenderingTechnique::CD3D12TexturedPlaneRenderingTechnique
(
	_In_ CD3D12SceneRenderer * pParentRenderer
)
:
	CD3D12RenderingTechnique( pParentRenderer ),
	m_pTexturedPlaneVertexBuffer( nullptr ),
	m_pTexturedObjectVertexShaderBlob( nullptr ),
	m_pTexturedObjectPixelShaderBlob( nullptr ),
	m_pTexturedPlaneGraphicsPipelineState( nullptr ),
	m_pTexture2D( nullptr ),
	m_pTexture2DUploadBuffer( nullptr )
{
};


///////////////////////////////////////////////////////
// Function:  ~CD3D12TexturedPlaneRenderingTechnique
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CD3D12TexturedPlaneRenderingTechnique::~CD3D12TexturedPlaneRenderingTechnique()
{
	assert( m_pTexture2DUploadBuffer == nullptr );
	assert( m_pTexture2D == nullptr );
	assert( m_pTexturedPlaneGraphicsPipelineState == nullptr );
	assert( m_pTexturedObjectPixelShaderBlob == nullptr );
	assert( m_pTexturedObjectVertexShaderBlob == nullptr );
	assert( m_pTexturedPlaneVertexBuffer == nullptr );
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
// Creates resources for the textured-plane scene,
// including the vertex buffer, shaders, and texture
// sampler state.
///////////////////////////////////////////////////////
HRESULT CD3D12TexturedPlaneRenderingTechnique::CreateResources
(
	SWAPCHAIN_SLOT CurrentSwapChainSlot
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	ID3DBlob * pErrorBlob = nullptr;


	if ( m_pTexturedPlaneGraphicsPipelineState == nullptr )
	{
		//
		// Load and compile the textured-object shaders.
		//

#if COMPILE_SHADERS_AT_RUNTIME
		// Load and compile shaders.
		assert( m_pTexturedObjectVertexShaderBlob == nullptr );
		HRESULT hCompileTexturedObjectVertexShaderResult = D3DCompileFromFile
		(
			L"TexturedObjectVertexShader.hlsl",
			nullptr,
			nullptr,
			"TexturedObjectVertexShader",
			"vs_5_1",
			0,
			0,
			&m_pTexturedObjectVertexShaderBlob,
			&pErrorBlob
		);

		if ( FAILED( hCompileTexturedObjectVertexShaderResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCompileTexturedObjectVertexShaderResult,
				WEX::Common::String().Format
				(
					L"D3DCompileFile on TexturedObjectVertexShader.hlsl failed.  Error: %S.",
					pErrorBlob->GetBufferPointer()
				)
			);

			hOverallResult = hCompileTexturedObjectVertexShaderResult;
			goto Cleanup;
		};


		assert( m_pTexturedObjectPixelShaderBlob == nullptr );
		HRESULT hCompileTexturedObjectPixelShaderResult = D3DCompileFromFile
		(
			L"TexturedObjectPixelShader.hlsl",
			nullptr,
			nullptr,
			"TexturedObjectPixelShader",
			"ps_5_1",
			0,
			0,
			&m_pTexturedObjectPixelShaderBlob,
			&pErrorBlob
		);

		if ( FAILED( hCompileTexturedObjectPixelShaderResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCompileTexturedObjectPixelShaderResult,
				WEX::Common::String().Format
				(
					L"D3DCompileFile on TexturedObjectPixelShader.hlsl failed.  Error: %S.",
					pErrorBlob->GetBufferPointer()
				)
			);

			hOverallResult = hCompileTexturedObjectPixelShaderResult;
			goto Cleanup;
		};
#endif


		//
		// Create the textured-object input layout.
		//

		D3D12_INPUT_ELEMENT_DESC TexturedPlaneInputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXTURECOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};


		//
		// Create a textured-plane graphics pipeline state object.
		//

		D3D12_GRAPHICS_PIPELINE_STATE_DESC TexturedPlaneGraphicsPipelineStateDescriptor;
		ZeroMemory( &TexturedPlaneGraphicsPipelineStateDescriptor, sizeof( TexturedPlaneGraphicsPipelineStateDescriptor ) );
		TexturedPlaneGraphicsPipelineStateDescriptor.InputLayout = { TexturedPlaneInputLayout, ARRAYSIZE( TexturedPlaneInputLayout ) };
		TexturedPlaneGraphicsPipelineStateDescriptor.pRootSignature = m_pParentRenderer->m_pRootSignature;
#if COMPILE_SHADERS_AT_RUNTIME
		TexturedPlaneGraphicsPipelineStateDescriptor.VS = { reinterpret_cast< BYTE * >( m_pTexturedObjectVertexShaderBlob->GetBufferPointer()), m_pTexturedObjectVertexShaderBlob->GetBufferSize() };
		TexturedPlaneGraphicsPipelineStateDescriptor.PS = { reinterpret_cast< BYTE * >( m_pTexturedObjectPixelShaderBlob->GetBufferPointer()), m_pTexturedObjectPixelShaderBlob->GetBufferSize() };
#else
		TexturedPlaneGraphicsPipelineStateDescriptor.VS = { g_TexturedObjectVertexShader, sizeof( g_TexturedObjectVertexShader ) };
		TexturedPlaneGraphicsPipelineStateDescriptor.PS = { g_TexturedObjectPixelShader, sizeof( g_TexturedObjectPixelShader ) };
#endif

		TexturedPlaneGraphicsPipelineStateDescriptor.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		TexturedPlaneGraphicsPipelineStateDescriptor.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
		TexturedPlaneGraphicsPipelineStateDescriptor.RasterizerState.FrontCounterClockwise = TRUE;
		TexturedPlaneGraphicsPipelineStateDescriptor.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		TexturedPlaneGraphicsPipelineStateDescriptor.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		TexturedPlaneGraphicsPipelineStateDescriptor.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		TexturedPlaneGraphicsPipelineStateDescriptor.RasterizerState.DepthClipEnable = FALSE;
		TexturedPlaneGraphicsPipelineStateDescriptor.RasterizerState.MultisampleEnable = FALSE;
		TexturedPlaneGraphicsPipelineStateDescriptor.RasterizerState.AntialiasedLineEnable = FALSE;
		TexturedPlaneGraphicsPipelineStateDescriptor.RasterizerState.ForcedSampleCount = 0;
		TexturedPlaneGraphicsPipelineStateDescriptor.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		TexturedPlaneGraphicsPipelineStateDescriptor.BlendState.AlphaToCoverageEnable = FALSE;
		TexturedPlaneGraphicsPipelineStateDescriptor.BlendState.IndependentBlendEnable = FALSE;
        for ( UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i )
		{
			TexturedPlaneGraphicsPipelineStateDescriptor.BlendState.RenderTarget[ i ] = DEFAULT_RENDER_TARGET_BLEND_DESCRIPTOR;
		};

		TexturedPlaneGraphicsPipelineStateDescriptor.DepthStencilState.DepthEnable = FALSE;
		TexturedPlaneGraphicsPipelineStateDescriptor.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		TexturedPlaneGraphicsPipelineStateDescriptor.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		TexturedPlaneGraphicsPipelineStateDescriptor.DepthStencilState.StencilEnable = FALSE;
		TexturedPlaneGraphicsPipelineStateDescriptor.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		TexturedPlaneGraphicsPipelineStateDescriptor.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		TexturedPlaneGraphicsPipelineStateDescriptor.DepthStencilState.FrontFace = DEFAULT_STENCIL_OPERATION;
		TexturedPlaneGraphicsPipelineStateDescriptor.DepthStencilState.BackFace = DEFAULT_STENCIL_OPERATION;

		TexturedPlaneGraphicsPipelineStateDescriptor.SampleMask = UINT_MAX;
		TexturedPlaneGraphicsPipelineStateDescriptor.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		TexturedPlaneGraphicsPipelineStateDescriptor.NumRenderTargets = 1;
		//TexturedPlaneGraphicsPipelineStateDescriptor.RTVFormats[ 0 ] = DXGI_FORMAT_R8G8B8A8_UNORM; // @REVIEW
		TexturedPlaneGraphicsPipelineStateDescriptor.RTVFormats[ 0 ] = DXGI_FORMAT_B8G8R8A8_UNORM; // @REVIEW
		TexturedPlaneGraphicsPipelineStateDescriptor.SampleDesc.Count = 1;

		assert( m_pTexturedPlaneGraphicsPipelineState == nullptr );
		HRESULT hCreateTexturedPlaneGraphicsPipelineStateResult = m_pParentRenderer->m_pDevice_ID3D12Device->CreateGraphicsPipelineState
		(
			&TexturedPlaneGraphicsPipelineStateDescriptor,
			IID_ID3D12PipelineState,
			(void **) &m_pTexturedPlaneGraphicsPipelineState
		);

		if ( FAILED( hCreateTexturedPlaneGraphicsPipelineStateResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateTexturedPlaneGraphicsPipelineStateResult,
				L"CreateGraphicsPipelineState failed."
			);

			hOverallResult = hCreateTexturedPlaneGraphicsPipelineStateResult;
			goto Cleanup;
		};
	};


	//
	// Create the plane vertex buffer if needed.
	//

	if ( m_pTexturedPlaneVertexBuffer == nullptr )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CD3D12TexturedPlaneRenderingTechnique::CreateResources:  Creating plane vertex buffer."
		);


		const float PLANE_HALF_WIDTH = 1.f;
		const float PLANE_HALF_HEIGHT = 1.f;

		TexturedVertex TexturedPlaneVertices[] =
		{
			// Front face
			{ D3DXVECTOR3(  PLANE_HALF_WIDTH, -PLANE_HALF_HEIGHT, 0.0f ), D3DXVECTOR2( 1.0f, 1.0f ) },
			{ D3DXVECTOR3( -PLANE_HALF_WIDTH,  PLANE_HALF_HEIGHT, 0.0f ), D3DXVECTOR2( 0.0f, 0.0f ) },
			{ D3DXVECTOR3(  PLANE_HALF_WIDTH,  PLANE_HALF_HEIGHT, 0.0f ), D3DXVECTOR2( 1.0f, 0.0f ) },

			// Back face
			{ D3DXVECTOR3(  PLANE_HALF_WIDTH, -PLANE_HALF_HEIGHT, 0.0f ), D3DXVECTOR2( 1.0f, 1.0f ) },
			{ D3DXVECTOR3( -PLANE_HALF_WIDTH, -PLANE_HALF_HEIGHT, 0.0f ), D3DXVECTOR2( 0.0f, 1.0f ) },
			{ D3DXVECTOR3( -PLANE_HALF_WIDTH,  PLANE_HALF_HEIGHT, 0.0f ), D3DXVECTOR2( 0.0f, 0.0f ) }
		};


		D3D12_HEAP_PROPERTIES TexturedPlaneVertexBufferHeapProperties;
		ZeroMemory( &TexturedPlaneVertexBufferHeapProperties, sizeof( TexturedPlaneVertexBufferHeapProperties ) );
		TexturedPlaneVertexBufferHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		//TexturedPlaneVertexBufferHeapProperties.CPUPageProperty = ;
		//TexturedPlaneVertexBufferHeapProperties.MemoryPoolPreference = ;
		//TexturedPlaneVertexBufferHeapProperties.CreationNodeMask = ;
		//TexturedPlaneVertexBufferHeapProperties.VisibleNodeMask = ;

		D3D12_RESOURCE_DESC TexturedPlaneVertexBufferResourceDescriptor;
		ZeroMemory( &TexturedPlaneVertexBufferResourceDescriptor, sizeof( TexturedPlaneVertexBufferResourceDescriptor ) );
		TexturedPlaneVertexBufferResourceDescriptor.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		TexturedPlaneVertexBufferResourceDescriptor.Alignment = 0;
		TexturedPlaneVertexBufferResourceDescriptor.Width = sizeof( TexturedPlaneVertices );
		TexturedPlaneVertexBufferResourceDescriptor.Height = 1;
		TexturedPlaneVertexBufferResourceDescriptor.DepthOrArraySize = 1;
		TexturedPlaneVertexBufferResourceDescriptor.MipLevels = 1;
		TexturedPlaneVertexBufferResourceDescriptor.Format = DXGI_FORMAT_UNKNOWN;
		TexturedPlaneVertexBufferResourceDescriptor.SampleDesc.Count = 1;
		TexturedPlaneVertexBufferResourceDescriptor.SampleDesc.Quality = 0;
		TexturedPlaneVertexBufferResourceDescriptor.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		TexturedPlaneVertexBufferResourceDescriptor.Flags = D3D12_RESOURCE_FLAG_NONE;

		assert( m_pTexturedPlaneVertexBuffer == nullptr );
		HRESULT hCreateTexturedPlaneVertexBufferResult = m_pParentRenderer->m_pDevice_ID3D12Device->CreateCommittedResource
		(
			&TexturedPlaneVertexBufferHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&TexturedPlaneVertexBufferResourceDescriptor,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_ID3D12Resource,
			(void **) &m_pTexturedPlaneVertexBuffer
		);

		if ( FAILED( hCreateTexturedPlaneVertexBufferResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateTexturedPlaneVertexBufferResult,
				L"CreateCommittedResource on textured-plane vertex buffer failed."
			);

			hOverallResult = hCreateTexturedPlaneVertexBufferResult;
			goto Cleanup;
		};


		//
		// Initialize the textured-plane vertex buffer.
		//

		TexturedVertex * pMappedTexturedPlaneVertexBuffer = nullptr;

		assert( m_pTexturedPlaneVertexBuffer != nullptr );
		m_pTexturedPlaneVertexBuffer->Map
		(
			0,
			nullptr,
			(void **) &pMappedTexturedPlaneVertexBuffer
		);

		CopyMemory
		(
			pMappedTexturedPlaneVertexBuffer,
			TexturedPlaneVertices,
			sizeof( TexturedPlaneVertices )
		);

		m_pTexturedPlaneVertexBuffer->Unmap
		(
			0,
			nullptr
		);


		//
		// Initialize the textured-plane vertex buffer view.
		//

		ZeroMemory( &m_TexturedPlaneVertexBufferView, sizeof( m_TexturedPlaneVertexBufferView ) );
		m_TexturedPlaneVertexBufferView.BufferLocation = m_pTexturedPlaneVertexBuffer->GetGPUVirtualAddress();
		m_TexturedPlaneVertexBufferView.StrideInBytes = sizeof( TexturedVertex );
		m_TexturedPlaneVertexBufferView.SizeInBytes = sizeof( TexturedPlaneVertices );
	};


	//
	// Create a command list.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12TexturedPlaneRenderingTechnique::CreateResources:  Creating command list."
	);

	HRESULT hCreateCommandListResult = m_pParentRenderer->CreateCommandList
	(
		CurrentSwapChainSlot,
		m_pTexturedPlaneGraphicsPipelineState
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


	if ( m_pTexture2D == nullptr )
	{
		//
		// Create a texture.
		//

		D3D12_HEAP_PROPERTIES Texture2DHeapProperties;
		ZeroMemory( &Texture2DHeapProperties, sizeof( Texture2DHeapProperties ) );
		//Texture2DHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		Texture2DHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		//Texture2DHeapProperties.CPUPageProperty = ;
		//Texture2DHeapProperties.MemoryPoolPreference = ;
		//Texture2DHeapProperties.CreationNodeMask = ;
		//Texture2DHeapProperties.VisibleNodeMask = ;

		const UINT64 TEXTURE_WIDTH = 10;
		const UINT TEXTURE_HEIGHT = 10;

		D3D12_RESOURCE_DESC Texture2DResourceDescriptor;
		ZeroMemory( &Texture2DResourceDescriptor, sizeof( Texture2DResourceDescriptor ) );
		Texture2DResourceDescriptor.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		Texture2DResourceDescriptor.Alignment = 0;
		Texture2DResourceDescriptor.Width = TEXTURE_WIDTH;
		Texture2DResourceDescriptor.Height = TEXTURE_HEIGHT;
		Texture2DResourceDescriptor.DepthOrArraySize = 1;
		Texture2DResourceDescriptor.MipLevels = 1;
		//Texture2DResourceDescriptor.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // @REVIEW
		Texture2DResourceDescriptor.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // @REVIEW
		Texture2DResourceDescriptor.SampleDesc.Count = 1;
		Texture2DResourceDescriptor.SampleDesc.Quality = 0;
		Texture2DResourceDescriptor.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		Texture2DResourceDescriptor.Flags = D3D12_RESOURCE_FLAG_NONE;

		assert( m_pTexture2D == nullptr );
		HRESULT hCreateTextureResult = m_pParentRenderer->m_pDevice_ID3D12Device->CreateCommittedResource
		(
			&Texture2DHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&Texture2DResourceDescriptor,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_ID3D12Resource,
			(void **) &m_pTexture2D
		);

		if ( FAILED( hCreateTextureResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateTextureResult,
				L"CreateCommittedResource failed."
			);

			hOverallResult = hCreateTextureResult;
			goto Cleanup;
		};


		//
		// Create an upload buffer for the texture.
		//

		UINT64 UploadBufferSize = GetRequiredIntermediateSize
		(
			m_pTexture2D,
			0,
			1
		);

		D3D12_HEAP_PROPERTIES UploadBufferHeapProperties;
		ZeroMemory( &UploadBufferHeapProperties, sizeof( UploadBufferHeapProperties ) );
		UploadBufferHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		//UploadBufferHeapProperties.CPUPageProperty = ;
		//UploadBufferHeapProperties.MemoryPoolPreference = ;
		//UploadBufferHeapProperties.CreationNodeMask = ;
		//UploadBufferHeapProperties.VisibleNodeMask = ;

		D3D12_RESOURCE_DESC UploadBufferResourceDescriptor;
		ZeroMemory( &UploadBufferResourceDescriptor, sizeof( UploadBufferResourceDescriptor ) );
		UploadBufferResourceDescriptor.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		UploadBufferResourceDescriptor.Alignment = 0;
		UploadBufferResourceDescriptor.Width = UploadBufferSize;
		UploadBufferResourceDescriptor.Height = 1;
		UploadBufferResourceDescriptor.DepthOrArraySize = 1;
		UploadBufferResourceDescriptor.MipLevels = 1;
		UploadBufferResourceDescriptor.Format = DXGI_FORMAT_UNKNOWN;
		UploadBufferResourceDescriptor.SampleDesc.Count = 1;
		UploadBufferResourceDescriptor.SampleDesc.Quality = 0;
		UploadBufferResourceDescriptor.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		UploadBufferResourceDescriptor.Flags = D3D12_RESOURCE_FLAG_NONE;

		assert( m_pTexture2DUploadBuffer == nullptr );
		HRESULT hCreateUploadBufferResult = m_pParentRenderer->m_pDevice_ID3D12Device->CreateCommittedResource
		(
			&UploadBufferHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&UploadBufferResourceDescriptor,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_ID3D12Resource,
			(void **) &m_pTexture2DUploadBuffer
		);

		if ( FAILED( hCreateUploadBufferResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateUploadBufferResult,
				L"CreateCommittedResource failed."
			);

			hOverallResult = hCreateUploadBufferResult;
			goto Cleanup;
		};


		//
		// Initialize texture data with a 10x10 blue-green gradient.
		//

		DWORD TextureColorValues[ TEXTURE_WIDTH ][ TEXTURE_HEIGHT ] =
		{
			{
				0xFF00FFFF,
				0xFF00FFE7,
				0xFF00FFCE,
				0xFF00FFB5,
				0xFF00FF9C,
				0xFF00FF83,
				0xFF00FF6A,
				0xFF00FF51,
				0xFF00FF38,
				0xFF00FF1E
			},

			{
				0xFF00E7FF,
				0xFF00E7E7,
				0xFF00E7CE,
				0xFF00E7B5,
				0xFF00E79C,
				0xFF00E783,
				0xFF00E76A,
				0xFF00E751,
				0xFF00E738,
				0xFF00E71E
			},

			{
				0xFF00CEFF,
				0xFF00CEE7,
				0xFF00CECE,
				0xFF00CEB5,
				0xFF00CE9C,
				0xFF00CE83,
				0xFF00CE6A,
				0xFF00CE51,
				0xFF00CE38,
				0xFF00CE1E
			},

			{
				0xFF00B5FF,
				0xFF00B5E7,
				0xFF00B5CE,
				0xFF00B5B5,
				0xFF00B59C,
				0xFF00B583,
				0xFF00B56A,
				0xFF00B551,
				0xFF00B538,
				0xFF00B51E
			},

			{
				0xFF009CFF,
				0xFF009CE7,
				0xFF009CCE,
				0xFF009CB5,
				0xFF009C9C,
				0xFF009C83,
				0xFF009C6A,
				0xFF009C51,
				0xFF009C38,
				0xFF009C1E
			},

			{
				0xFF0083FF,
				0xFF0083E7,
				0xFF0083CE,
				0xFF0083B5,
				0xFF00839C,
				0xFF008383,
				0xFF00836A,
				0xFF008351,
				0xFF008338,
				0xFF00831E
			},

			{
				0xFF006AFF,
				0xFF006AE7,
				0xFF006ACE,
				0xFF006AB5,
				0xFF006A9C,
				0xFF006A83,
				0xFF006A6A,
				0xFF006A51,
				0xFF006A38,
				0xFF006A1E
			},

			{
				0xFF0051FF,
				0xFF0051E7,
				0xFF0051CE,
				0xFF0051B5,
				0xFF00519C,
				0xFF005183,
				0xFF00516A,
				0xFF005151,
				0xFF005138,
				0xFF00511E
			},

			{
				0xFF0038FF,
				0xFF0038E7,
				0xFF0038CE,
				0xFF0038B5,
				0xFF00389C,
				0xFF003883,
				0xFF00386A,
				0xFF003851,
				0xFF003838,
				0xFF00381E
			},

			{
				0xFF001EFF,
				0xFF001EE7,
				0xFF001ECE,
				0xFF001EB5,
				0xFF001E9C,
				0xFF001E83,
				0xFF001E6A,
				0xFF001E51,
				0xFF001E38,
				0xFF001E1E
			}
		};

		D3D12_SUBRESOURCE_DATA TextureData;
		ZeroMemory( &TextureData, sizeof( TextureData ) );
		TextureData.pData = TextureColorValues;
		TextureData.RowPitch = TEXTURE_WIDTH * sizeof( DWORD );
		TextureData.SlicePitch = sizeof( TextureData );


		//
		// Reset the command list.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12TexturedPlaneRenderingTechnique::CreateResources:  Resetting command list."
		);

		m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->Reset
		(
			m_pParentRenderer->m_ppCommandAllocators[ CurrentSwapChainSlot ],
			m_pTexturedPlaneGraphicsPipelineState
		);


		//
		// Copy data to the intermediate upload heap and then schedule
		// a copy from the upload heap to the texture.
		//

		UpdateSubresources
		(
			m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ],
			m_pTexture2D,
			m_pTexture2DUploadBuffer,
			0,
			0,
			1,
			&TextureData
		);


		//
		// Switch the texture into a state where it can be bound as a
		// pixel shader resource.
		//

		D3D12_RESOURCE_BARRIER Texture2DResourceBarrier;
		ZeroMemory( &Texture2DResourceBarrier, sizeof( Texture2DResourceBarrier ) );
		Texture2DResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Texture2DResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Texture2DResourceBarrier.Transition.pResource = m_pTexture2D;
		Texture2DResourceBarrier.Transition.Subresource = 0;
		Texture2DResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		Texture2DResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->ResourceBarrier
		(
			1,
			&Texture2DResourceBarrier
		);


		//
		// Close the command list.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12TexturedPlaneRenderingTechnique::CreateResources:  Closing command list."
		);

		HRESULT hCloseCommandListResult = m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->Close();

		if ( FAILED( hCloseCommandListResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCloseCommandListResult,
				L"Close on command list failed."
			);

			hOverallResult = hCloseCommandListResult;
			goto Cleanup;
		};


		//
		// Execute the command list to upload the texture data.
		//

		ID3D12CommandList * CommandListsToExecute[] =
		{
			m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]
		};

		m_pParentRenderer->m_pQueue_ID3D12CommandQueue->ExecuteCommandLists
		(
			ARRAYSIZE( CommandListsToExecute ),
			CommandListsToExecute
		);


		//
		// Wait for the GPU to catch up, meaning that the texture
		// data has been uploaded successfully.
		//

		HRESULT hWaitForGPUResult = m_pParentRenderer->WaitForGPU();

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


		//
		// Create a shader resource view for the texture.
		//

		D3D12_SHADER_RESOURCE_VIEW_DESC TextureShaderResourceViewDescriptor;
		ZeroMemory( &TextureShaderResourceViewDescriptor, sizeof( TextureShaderResourceViewDescriptor ) );
		//TextureShaderResourceViewDescriptor.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // @REVIEW
		TextureShaderResourceViewDescriptor.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // @REVIEW
		TextureShaderResourceViewDescriptor.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		TextureShaderResourceViewDescriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		TextureShaderResourceViewDescriptor.Texture2D.MostDetailedMip = 0;
		TextureShaderResourceViewDescriptor.Texture2D.MipLevels = 1;
		TextureShaderResourceViewDescriptor.Texture2D.PlaneSlice = 0;
		TextureShaderResourceViewDescriptor.Texture2D.ResourceMinLODClamp = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE TextureShaderResourceViewDestinationDescriptor;
		TextureShaderResourceViewDestinationDescriptor = m_pParentRenderer->m_pConstantBufferViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		TextureShaderResourceViewDestinationDescriptor.ptr += m_pParentRenderer->m_pDevice_ID3D12Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );

		m_pParentRenderer->m_pDevice_ID3D12Device->CreateShaderResourceView
		(
			m_pTexture2D,
			&TextureShaderResourceViewDescriptor,
			TextureShaderResourceViewDestinationDescriptor
		);
	};


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  DestroyResources
//
// Purpose:
// Destroys and releases resources associated with the
// textured-plane scene.
///////////////////////////////////////////////////////
void CD3D12TexturedPlaneRenderingTechnique::DestroyResources()
{
	SAFE_RELEASE( m_pTexture2DUploadBuffer );
	SAFE_RELEASE( m_pTexture2D );
	SAFE_RELEASE( m_pTexturedObjectPixelShaderBlob );
	SAFE_RELEASE( m_pTexturedObjectVertexShaderBlob );
	SAFE_RELEASE( m_pTexturedPlaneGraphicsPipelineState );
	SAFE_RELEASE( m_pTexturedPlaneVertexBuffer );
};


///////////////////////////////////////////////////////
// Function:  RenderScene
//
// Purpose:
// Renders a textured plane.
///////////////////////////////////////////////////////
HRESULT CD3D12TexturedPlaneRenderingTechnique::RenderScene
(
	SWAPCHAIN_SLOT CurrentSwapChainSlot,
	_In_ ID3D12Resource * pCurrentSwapChainBuffer
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Create a render target view of the swap chain's current buffer.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12TexturedPlaneRenderingTechnique::RenderScene:  Creating render target view."
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
		m_pTexturedPlaneGraphicsPipelineState
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


	//
	// Add scene-independent commands to the command list.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12TexturedPlaneRenderingTechnique::RenderScene:  Populating command list."
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
		L"CD3D12TexturedPlaneRenderingTechnique::RenderScene:  Setting back buffer as render target."
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
	// Set the render target.
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
		L"CD3D12TexturedPlaneRenderingTechnique::RenderScene:  Clearing render target."
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


	//
	// Record rendering commands.
	//

	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->IASetVertexBuffers
	(
		0,
		1,
		&m_TexturedPlaneVertexBufferView
	);


	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->DrawInstanced( 6, 1, 0, 0 );


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};
