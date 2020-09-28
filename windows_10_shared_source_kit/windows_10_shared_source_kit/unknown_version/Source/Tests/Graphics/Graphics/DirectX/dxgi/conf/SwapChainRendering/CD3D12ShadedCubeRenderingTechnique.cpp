//////////////////////////////////////////////////////////////////////
// File:  CD3D12ShadedCubeRenderingTechnique.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a Direct3D 12 rendering technique that renders a
// wrap-shaded cube.
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

#include "CD3D12ShadedCubeRenderingTechnique.hpp"
#include "CD3D12SceneRenderer.hpp"

// Pre-compiled shader blobs
#include "ShadedObjectVertexShader_5_1.h"
#include "ShadedObjectPixelShader_5_1.h"


/////////////////////////////////////////////////////////////////
//
// Type definitions
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Structure:  ShadedVertex
//
// Purpose:
// Holds position information, a normal vector, and a
// color for a vertex.
///////////////////////////////////////////////////////
struct ShadedVertex
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR4 Color;
};


/////////////////////////////////////////////////////////////////
//
// CD3D12ShadedCubeRenderingTechnique implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D12ShadedCubeRenderingTechnique
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CD3D12ShadedCubeRenderingTechnique::CD3D12ShadedCubeRenderingTechnique
(
	_In_ CD3D12SceneRenderer * pParentRenderer
)
:
	CD3D12RenderingTechnique( pParentRenderer ),
	m_pShadedCubeVertexBuffer( nullptr ),
	m_pShadedObjectVertexShaderBlob( nullptr ),
	m_pShadedObjectPixelShaderBlob( nullptr ),
	m_pShadedCubeGraphicsPipelineState( nullptr )
{
};


///////////////////////////////////////////////////////
// Function:  ~CD3D12ShadedCubeRenderingTechnique
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CD3D12ShadedCubeRenderingTechnique::~CD3D12ShadedCubeRenderingTechnique()
{
	assert( m_pShadedCubeGraphicsPipelineState == nullptr );
	assert( m_pShadedObjectPixelShaderBlob == nullptr );
	assert( m_pShadedObjectVertexShaderBlob == nullptr );
	assert( m_pShadedCubeVertexBuffer == nullptr );
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
// Creates resources for the shaded cube scene,
// including the vertex buffer and shaders.
///////////////////////////////////////////////////////
HRESULT CD3D12ShadedCubeRenderingTechnique::CreateResources
(
	SWAPCHAIN_SLOT CurrentSwapChainSlot
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	ID3DBlob * pErrorBlob = nullptr;


	if ( m_pShadedCubeGraphicsPipelineState == nullptr )
	{
		//
		// Load and compile the shaded-object shaders.
		//

#if COMPILE_SHADERS_AT_RUNTIME
		// Load and compile shaders.
		assert( m_pShadedObjectVertexShaderBlob == nullptr );
		HRESULT hCompileShadedObjectVertexShaderResult = D3DCompileFromFile
		(
			L"ShadedObjectVertexShader.hlsl",
			nullptr,
			nullptr,
			"ShadedObjectVertexShader",
			"vs_5_1",
			0,
			0,
			&m_pShadedObjectVertexShaderBlob,
			&pErrorBlob
		);

		if ( FAILED( hCompileShadedObjectVertexShaderResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCompileShadedObjectVertexShaderResult,
				WEX::Common::String().Format
				(
					L"D3DCompileFile on ShadedObjectVertexShader.hlsl failed.  Error: %S.",
					pErrorBlob->GetBufferPointer()
				)
			);

			hOverallResult = hCompileShadedObjectVertexShaderResult;
			goto Cleanup;
		};


		assert( m_pShadedObjectPixelShaderBlob == nullptr );
		HRESULT hCompileShadedObjectPixelShaderResult = D3DCompileFromFile
		(
			L"ShadedObjectPixelShader.hlsl",
			nullptr,
			nullptr,
			"ShadedObjectPixelShader",
			"ps_5_1",
			0,
			0,
			&m_pShadedObjectPixelShaderBlob,
			&pErrorBlob
		);

		if ( FAILED( hCompileShadedObjectPixelShaderResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCompileShadedObjectPixelShaderResult,
				L"D3DCompileFile on ShadedObjectPixelShader.hlsl failed."
			);

			hOverallResult = hCompileShadedObjectPixelShaderResult;
			goto Cleanup;
		};
#endif


		//
		// Create the shaded-object input layout.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12ShadedCubeRenderingTechnique::CreateResources:  Defining input layout."
		);

		D3D12_INPUT_ELEMENT_DESC ShadedCubeInputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};


		//
		// Create a shaded-cube graphics pipeline state object.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12ShadedCubeRenderingTechnique::CreateResources:  Creating graphics pipeline state."
		);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC ShadedCubeGraphicsPipelineStateDescriptor;
		ZeroMemory( &ShadedCubeGraphicsPipelineStateDescriptor, sizeof( ShadedCubeGraphicsPipelineStateDescriptor ) );
		ShadedCubeGraphicsPipelineStateDescriptor.InputLayout = { ShadedCubeInputLayout, ARRAYSIZE( ShadedCubeInputLayout ) };
		ShadedCubeGraphicsPipelineStateDescriptor.pRootSignature = m_pParentRenderer->m_pRootSignature;
#if COMPILE_SHADERS_AT_RUNTIME
		ShadedCubeGraphicsPipelineStateDescriptor.VS = { reinterpret_cast< BYTE * >( m_pShadedObjectVertexShaderBlob->GetBufferPointer()), m_pShadedObjectVertexShaderBlob->GetBufferSize() };
		ShadedCubeGraphicsPipelineStateDescriptor.PS = { reinterpret_cast< BYTE * >( m_pShadedObjectPixelShaderBlob->GetBufferPointer()), m_pShadedObjectPixelShaderBlob->GetBufferSize() };
#else
		ShadedCubeGraphicsPipelineStateDescriptor.VS = { g_ShadedObjectVertexShader, sizeof( g_ShadedObjectVertexShader ) };
		ShadedCubeGraphicsPipelineStateDescriptor.PS = { g_ShadedObjectPixelShader, sizeof( g_ShadedObjectPixelShader ) };
#endif

		ShadedCubeGraphicsPipelineStateDescriptor.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		ShadedCubeGraphicsPipelineStateDescriptor.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
		ShadedCubeGraphicsPipelineStateDescriptor.RasterizerState.FrontCounterClockwise = TRUE;
		ShadedCubeGraphicsPipelineStateDescriptor.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		ShadedCubeGraphicsPipelineStateDescriptor.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		ShadedCubeGraphicsPipelineStateDescriptor.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		ShadedCubeGraphicsPipelineStateDescriptor.RasterizerState.DepthClipEnable = FALSE;
		ShadedCubeGraphicsPipelineStateDescriptor.RasterizerState.MultisampleEnable = FALSE;
		ShadedCubeGraphicsPipelineStateDescriptor.RasterizerState.AntialiasedLineEnable = FALSE;
		ShadedCubeGraphicsPipelineStateDescriptor.RasterizerState.ForcedSampleCount = 0;
		ShadedCubeGraphicsPipelineStateDescriptor.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		ShadedCubeGraphicsPipelineStateDescriptor.BlendState.AlphaToCoverageEnable = FALSE;
		ShadedCubeGraphicsPipelineStateDescriptor.BlendState.IndependentBlendEnable = FALSE;
        for ( UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i )
		{
			ShadedCubeGraphicsPipelineStateDescriptor.BlendState.RenderTarget[ i ] = DEFAULT_RENDER_TARGET_BLEND_DESCRIPTOR;
		};

		ShadedCubeGraphicsPipelineStateDescriptor.DepthStencilState.DepthEnable = FALSE;
		ShadedCubeGraphicsPipelineStateDescriptor.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		ShadedCubeGraphicsPipelineStateDescriptor.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		ShadedCubeGraphicsPipelineStateDescriptor.DepthStencilState.StencilEnable = FALSE;
		ShadedCubeGraphicsPipelineStateDescriptor.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		ShadedCubeGraphicsPipelineStateDescriptor.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		ShadedCubeGraphicsPipelineStateDescriptor.DepthStencilState.FrontFace = DEFAULT_STENCIL_OPERATION;
		ShadedCubeGraphicsPipelineStateDescriptor.DepthStencilState.BackFace = DEFAULT_STENCIL_OPERATION;

		ShadedCubeGraphicsPipelineStateDescriptor.SampleMask = UINT_MAX;
		ShadedCubeGraphicsPipelineStateDescriptor.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		ShadedCubeGraphicsPipelineStateDescriptor.NumRenderTargets = 1;
		//ShadedCubeGraphicsPipelineStateDescriptor.RTVFormats[ 0 ] = DXGI_FORMAT_R8G8B8A8_UNORM; // @REVIEW
		ShadedCubeGraphicsPipelineStateDescriptor.RTVFormats[ 0 ] = DXGI_FORMAT_B8G8R8A8_UNORM; // @REVIEW
		ShadedCubeGraphicsPipelineStateDescriptor.SampleDesc.Count = 1;

		assert( m_pShadedCubeGraphicsPipelineState == nullptr );
		HRESULT hCreateShadedCubeGraphicsPipelineStateResult = m_pParentRenderer->m_pDevice_ID3D12Device->CreateGraphicsPipelineState
		(
			&ShadedCubeGraphicsPipelineStateDescriptor,
			IID_ID3D12PipelineState,
			(void **) &m_pShadedCubeGraphicsPipelineState
		);

		if ( FAILED( hCreateShadedCubeGraphicsPipelineStateResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateShadedCubeGraphicsPipelineStateResult,
				L"CreateGraphicsPipelineState failed."
			);

			hOverallResult = hCreateShadedCubeGraphicsPipelineStateResult;
			goto Cleanup;
		};
	};


	//
	// Create the cube vertex buffer if needed.
	//

	if ( m_pShadedCubeVertexBuffer == nullptr )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CD3D12ShadedCubeRenderingTechnique::CreateResources:  Creating cube vertex buffer."
		);


		const float CUBE_HALF_WIDTH = 1.f;
		const float CUBE_ORIGIN_X = 0.f;
		const float CUBE_ORIGIN_Y = 0.f;
		const float CUBE_ORIGIN_Z = 0.f;

		ShadedVertex ShadedCubeVertices[] =
		{
			// Front face
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  0.0f, -1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  0.0f, -1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  0.0f, -1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },

			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  0.0f, -1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  0.0f, -1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  0.0f, -1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },

			// Right face
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  1.0f,  0.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  1.0f,  0.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  1.0f,  0.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },

			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  1.0f,  0.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  1.0f,  0.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  1.0f,  0.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },

			// Back face
			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  0.0f,  1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  0.0f,  1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  0.0f,  1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },

			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  0.0f,  1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  0.0f,  1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  0.0f,  1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },

			// Left face
			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3( -1.0f,  0.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3( -1.0f,  0.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3( -1.0f,  0.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },

			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3( -1.0f,  0.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3( -1.0f,  0.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3( -1.0f,  0.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },

			// Top face
			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  1.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  1.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  1.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },

			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  1.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  1.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f,  1.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },

			// Bottom face
			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f, -1.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f, -1.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f, -1.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },

			{ D3DXVECTOR3( -CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f, -1.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f, -1.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3(  CUBE_HALF_WIDTH + CUBE_ORIGIN_X, -CUBE_HALF_WIDTH + CUBE_ORIGIN_Y,  CUBE_HALF_WIDTH + CUBE_ORIGIN_Z ), D3DXVECTOR3(  0.0f, -1.0f,  0.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
		};


		D3D12_HEAP_PROPERTIES ShadedCubeVertexBufferHeapProperties;
		ZeroMemory( &ShadedCubeVertexBufferHeapProperties, sizeof( ShadedCubeVertexBufferHeapProperties ) );
		ShadedCubeVertexBufferHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		//ShadedCubeVertexBufferHeapProperties.CPUPageProperty = ;
		//ShadedCubeVertexBufferHeapProperties.MemoryPoolPreference = ;
		//ShadedCubeVertexBufferHeapProperties.CreationNodeMask = ;
		//ShadedCubeVertexBufferHeapProperties.VisibleNodeMask = ;

		D3D12_RESOURCE_DESC ShadedCubeVertexBufferResourceDescriptor;
		ZeroMemory( &ShadedCubeVertexBufferResourceDescriptor, sizeof( ShadedCubeVertexBufferResourceDescriptor ) );
		ShadedCubeVertexBufferResourceDescriptor.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ShadedCubeVertexBufferResourceDescriptor.Alignment = 0;
		ShadedCubeVertexBufferResourceDescriptor.Width = sizeof( ShadedCubeVertices );
		ShadedCubeVertexBufferResourceDescriptor.Height = 1;
		ShadedCubeVertexBufferResourceDescriptor.DepthOrArraySize = 1;
		ShadedCubeVertexBufferResourceDescriptor.MipLevels = 1;
		ShadedCubeVertexBufferResourceDescriptor.Format = DXGI_FORMAT_UNKNOWN;
		ShadedCubeVertexBufferResourceDescriptor.SampleDesc.Count = 1;
		ShadedCubeVertexBufferResourceDescriptor.SampleDesc.Quality = 0;
		ShadedCubeVertexBufferResourceDescriptor.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		ShadedCubeVertexBufferResourceDescriptor.Flags = D3D12_RESOURCE_FLAG_NONE;

		assert( m_pShadedCubeVertexBuffer == nullptr );
		HRESULT hCreateShadedCubeVertexBufferResult = m_pParentRenderer->m_pDevice_ID3D12Device->CreateCommittedResource
		(
			&ShadedCubeVertexBufferHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&ShadedCubeVertexBufferResourceDescriptor,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_ID3D12Resource,
			(void **) &m_pShadedCubeVertexBuffer
		);

		if ( FAILED( hCreateShadedCubeVertexBufferResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateShadedCubeVertexBufferResult,
				L"CreateCommittedResource failed."
			);

			hOverallResult = hCreateShadedCubeVertexBufferResult;
			goto Cleanup;
		};


		//
		// Initialize the shaded-cube vertex buffer.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12ShadedCubeRenderingTechnique::CreateResources:  Initializing vertex buffer."
		);

		ShadedVertex * pMappedShadedCubeVertexBuffer = nullptr;
		m_pShadedCubeVertexBuffer->Map
		(
			0,
			nullptr,
			(void **) &pMappedShadedCubeVertexBuffer
		);

		CopyMemory
		(
			pMappedShadedCubeVertexBuffer,
			ShadedCubeVertices,
			sizeof( ShadedCubeVertices )
		);

		m_pShadedCubeVertexBuffer->Unmap
		(
			0,
			nullptr
		);


		//
		// Create a shaded-cube vertex buffer view.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12ShadedCubeRenderingTechnique::CreateResources:  Creating vertex-buffer view."
		);

		ZeroMemory( &m_ShadedCubeVertexBufferView, sizeof( m_ShadedCubeVertexBufferView ) );
		m_ShadedCubeVertexBufferView.BufferLocation = m_pShadedCubeVertexBuffer->GetGPUVirtualAddress();
		m_ShadedCubeVertexBufferView.StrideInBytes = sizeof( ShadedVertex );
		m_ShadedCubeVertexBufferView.SizeInBytes = sizeof( ShadedCubeVertices );
	};


	//
	// Create a command list.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12ShadedCubeRenderingTechnique::CreateResources:  Creating command list."
	);

	HRESULT hCreateCommandListResult = m_pParentRenderer->CreateCommandList
	(
		CurrentSwapChainSlot,
		m_pShadedCubeGraphicsPipelineState
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
// Destroys and releases resources associated with the
// shaded-cube scene.
///////////////////////////////////////////////////////
void CD3D12ShadedCubeRenderingTechnique::DestroyResources()
{
	SAFE_RELEASE( m_pShadedObjectPixelShaderBlob );
	SAFE_RELEASE( m_pShadedObjectVertexShaderBlob );
	SAFE_RELEASE( m_pShadedCubeGraphicsPipelineState );
	SAFE_RELEASE( m_pShadedCubeVertexBuffer );
};


///////////////////////////////////////////////////////
// Function:  RenderScene
//
// Purpose:
// Renders a wrap-shaded cube.
///////////////////////////////////////////////////////
HRESULT CD3D12ShadedCubeRenderingTechnique::RenderScene
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
		L"CD3D12ShadedCubeRenderingTechnique::RenderScene:  Creating render target view."
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
		m_pShadedCubeGraphicsPipelineState
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
		L"CD3D12ShadedCubeRenderingTechnique::RenderScene:  Populating command list."
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
		L"CD3D12ShadedCubeRenderingTechnique::RenderScene:  Setting back buffer as render target."
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
		L"CD3D12ShadedCubeRenderingTechnique::RenderScene:  Clearing render target."
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
	// Record rendering commands.
	//

	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->IASetVertexBuffers
	(
		0,
		1,
		&m_ShadedCubeVertexBufferView
	);


	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	m_pParentRenderer->m_ppCommandLists[ CurrentSwapChainSlot ]->DrawInstanced( 36, 1, 0, 0 );


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};
