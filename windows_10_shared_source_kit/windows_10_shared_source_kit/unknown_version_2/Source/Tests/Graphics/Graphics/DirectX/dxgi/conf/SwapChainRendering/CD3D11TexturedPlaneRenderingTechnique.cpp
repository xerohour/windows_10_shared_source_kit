//////////////////////////////////////////////////////////////////////
// File:  CD3D11TexturedPlaneRenderingTechnique.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a technique to render a textured plane.
//
// History:
// 19 Aug 2015  SEdmison  Factored out of CD3D11SceneRenderer.cpp.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "CD3D11TexturedPlaneRenderingTechnique.hpp"
#include "CD3D11SceneRenderer.hpp"

// Pre-compiled shader blobs
#include "TexturedObjectVertexShader_4_0_level_9_1.h"
#include "TexturedObjectPixelShader_4_0_level_9_1.h"


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
// CD3D11TexturedPlaneRenderingTechnique implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D11TexturedPlaneRenderingTechnique
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CD3D11TexturedPlaneRenderingTechnique::CD3D11TexturedPlaneRenderingTechnique
(
	_In_ CD3D11SceneRenderer * pParentRenderer
)
:
	CD3D11RenderingTechnique( pParentRenderer ),
	m_pTexturedPlaneVertexBuffer( nullptr ),
	m_pTexture( nullptr ),
	m_pTextureResourceView( nullptr ),
	m_pTexturedObjectVertexShader( nullptr ),
	m_pTexturedObjectPixelShader( nullptr ),
	m_pTexturedObjectInputLayout( nullptr ),
	m_pTextureSamplerState( nullptr )
{
};


///////////////////////////////////////////////////////
// Function:  ~CD3D11TexturedPlaneRenderingTechnique
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CD3D11TexturedPlaneRenderingTechnique::~CD3D11TexturedPlaneRenderingTechnique()
{
	assert( m_pTexturedObjectVertexShader == nullptr );
	assert( m_pTexturedObjectPixelShader == nullptr );
	assert( m_pTexturedObjectInputLayout == nullptr );

	assert( m_pTextureSamplerState == nullptr );

	assert( m_pTextureResourceView == nullptr );
	assert( m_pTexture == nullptr );

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
HRESULT CD3D11TexturedPlaneRenderingTechnique::CreateResources
(
	SWAPCHAIN_SLOT /* CurrentSwapChain */
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Create the textured-object vertex shader if needed.
	//

	if ( m_pTexturedObjectVertexShader == nullptr )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CD3D11TexturedPlaneRenderingTechnique::CreateResources:  Creating textured-object vertex shader."
		);

		assert( m_pTexturedObjectVertexShader == nullptr );
		HRESULT hCreateTexturedObjectVertexShaderResult = m_pParentRenderer->m_pDevice_ID3D11Device->CreateVertexShader
		(
			g_TexturedObjectVertexShader,
			ARRAYSIZE( g_TexturedObjectVertexShader ),
			nullptr,
			&m_pTexturedObjectVertexShader
		);

		if ( FAILED( hCreateTexturedObjectVertexShaderResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateTexturedObjectVertexShaderResult,
				L"CreateVertexShader on textured-object vertex shader failed."
			);

			hOverallResult = hCreateTexturedObjectVertexShaderResult;
			goto Cleanup;
		};
	};


	//
	// Create the textured-object pixel shader if needed.
	//

	if ( m_pTexturedObjectPixelShader == nullptr )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CD3D11TexturedPlaneRenderingTechnique::CreateResources:  Creating textured-object pixel shader."
		);

		assert( m_pTexturedObjectPixelShader == nullptr );
		HRESULT hCreateTexturedObjectPixelShaderResult = m_pParentRenderer->m_pDevice_ID3D11Device->CreatePixelShader
		(
			g_TexturedObjectPixelShader,
			ARRAYSIZE( g_TexturedObjectPixelShader ),
			nullptr,
			&m_pTexturedObjectPixelShader
		);

		if ( FAILED( hCreateTexturedObjectPixelShaderResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateTexturedObjectPixelShaderResult,
				L"CreatePixelShader on textured-object pixel shader failed."
			);

			hOverallResult = hCreateTexturedObjectPixelShaderResult;
			goto Cleanup;
		};
	};


	//
	// Create the textured-object input layout if needed.
	//

	if ( m_pTexturedObjectInputLayout == nullptr )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CD3D11TexturedPlaneRenderingTechnique::CreateResources:  Creating textured-object input layout."
		);

		D3D11_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXTURECOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		assert( m_pTexturedObjectInputLayout == nullptr );
		HRESULT hCreateTexturedObjectInputLayoutResult = m_pParentRenderer->m_pDevice_ID3D11Device->CreateInputLayout
		(
			inputLayout,
			ARRAYSIZE( inputLayout ),
			g_TexturedObjectVertexShader,
			ARRAYSIZE( g_TexturedObjectVertexShader ),
			&m_pTexturedObjectInputLayout
		);

		if ( FAILED( hCreateTexturedObjectInputLayoutResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateTexturedObjectInputLayoutResult,
				L"CreateInputLayout on textured-object input layout failed."
			);

			hOverallResult = hCreateTexturedObjectInputLayoutResult;
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
			L"CD3D11TexturedPlaneRenderingTechnique::CreateResources:  Creating plane vertex buffer."
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

		D3D11_BUFFER_DESC PlaneVertexBufferDescriptor;
		ZeroMemory( &PlaneVertexBufferDescriptor, sizeof( PlaneVertexBufferDescriptor ) );
		PlaneVertexBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
		PlaneVertexBufferDescriptor.ByteWidth = sizeof( TexturedPlaneVertices );
		PlaneVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		PlaneVertexBufferDescriptor.CPUAccessFlags = 0;
		PlaneVertexBufferDescriptor.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA PlaneVertexBufferSubresourceData;
		ZeroMemory( &PlaneVertexBufferSubresourceData, sizeof( PlaneVertexBufferSubresourceData ) );
		PlaneVertexBufferSubresourceData.pSysMem = TexturedPlaneVertices;
		PlaneVertexBufferSubresourceData.SysMemPitch = 0;
		PlaneVertexBufferSubresourceData.SysMemSlicePitch = 0;

		assert( m_pTexturedPlaneVertexBuffer == nullptr );
		HRESULT hCreateTexturedPlaneVertexBufferResult = m_pParentRenderer->m_pDevice_ID3D11Device->CreateBuffer
		(
			&PlaneVertexBufferDescriptor,
			&PlaneVertexBufferSubresourceData,
			&m_pTexturedPlaneVertexBuffer
		);

		if ( FAILED( hCreateTexturedPlaneVertexBufferResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateTexturedPlaneVertexBufferResult,
				L"CreateBuffer on textured-plane vertex buffer failed."
			);

			hOverallResult = hCreateTexturedPlaneVertexBufferResult;
			goto Cleanup;
		};
	};


	//
	// Create the texture resource view if needed.
	//

	if ( m_pTextureResourceView == nullptr )
	{
#if USE_RAINBOW_TEXTURE
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CD3D11TexturedPlaneRenderingTechnique::CreateResources:  Creating resource view of texture Granger_Rainbow.png."
		);

		assert( m_pTextureResourceView == nullptr );
		HRESULT hCreateTextureResourceViewResult = D3DX11CreateShaderResourceViewFromFileW
		(
			m_pDevice_ID3D11Device,
			L"Granger_Rainbow.png",
			nullptr,
			nullptr,
			&m_pTextureResourceView,
			nullptr
		);

		if ( FAILED( hCreateTextureResourceViewResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateTextureResourceViewResult,
				L"D3DX11CreateShaderResourceViewFromFile on Granger_Rainbow.png failed."
			);

			hOverallResult = hCreateTextureResourceViewResult;
			goto Cleanup;
		};
#else
		//
		// Create a texture with a 10x10 blue-green gradient.
		//

		const UINT64 TEXTURE_WIDTH = 10;
		const UINT TEXTURE_HEIGHT = 10;

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


		D3D11_TEXTURE2D_DESC TextureDescriptor;
		ZeroMemory( &TextureDescriptor, sizeof( TextureDescriptor ) );
		TextureDescriptor.Width = TEXTURE_WIDTH;
		TextureDescriptor.Height = TEXTURE_HEIGHT;
		TextureDescriptor.MipLevels = 1;
		TextureDescriptor.ArraySize = 1;
		TextureDescriptor.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // @REVIEW
		TextureDescriptor.SampleDesc.Count = 1;
		TextureDescriptor.SampleDesc.Quality = 0;
		TextureDescriptor.Usage = D3D11_USAGE_DEFAULT;
		TextureDescriptor.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TextureDescriptor.CPUAccessFlags = 0;
		TextureDescriptor.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA TextureData;
		ZeroMemory( &TextureData, sizeof( TextureData ) );
		TextureData.pSysMem = TextureColorValues;
		TextureData.SysMemPitch = TEXTURE_WIDTH * sizeof( DWORD );
		TextureData.SysMemSlicePitch = sizeof( TextureData );


		HRESULT hCreateTextureResult = m_pParentRenderer->m_pDevice_ID3D11Device->CreateTexture2D
		(
			&TextureDescriptor,
			&TextureData,
			&m_pTexture
		);

		if ( FAILED( hCreateTextureResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateTextureResult,
				L"CreateTexture2D failed."
			);

			hOverallResult = hCreateTextureResult;
			goto Cleanup;
		};


		//
		// Create a shader resource view for the texture.
		//

		D3D11_SHADER_RESOURCE_VIEW_DESC TextureShaderResourceViewDescriptor;
		ZeroMemory( &TextureShaderResourceViewDescriptor, sizeof( TextureShaderResourceViewDescriptor ) );
		TextureShaderResourceViewDescriptor.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // @REVIEW
		TextureShaderResourceViewDescriptor.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		TextureShaderResourceViewDescriptor.Texture2D.MostDetailedMip = 0;
		TextureShaderResourceViewDescriptor.Texture2D.MipLevels = 1;

		HRESULT hCreateTextureShaderResourceViewResult = m_pParentRenderer->m_pDevice_ID3D11Device->CreateShaderResourceView
		(
			m_pTexture,
			&TextureShaderResourceViewDescriptor,
			&m_pTextureResourceView
		);

		if ( FAILED( hCreateTextureShaderResourceViewResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateTextureShaderResourceViewResult,
				L"CreateShaderResourceView failed."
			);

			hOverallResult = hCreateTextureShaderResourceViewResult;
			goto Cleanup;
		};
#endif
	};


	//
	// Create the texture sampler state if needed.
	//

	if ( m_pTextureSamplerState == nullptr )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CD3D11TexturedPlaneRenderingTechnique::CreateResources:  Creating texture sampler state."
		);

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory( &sampDesc, sizeof(sampDesc) );
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		assert( m_pTextureSamplerState == nullptr );
		HRESULT hCreateTextureSamplerStateResult = m_pParentRenderer->m_pDevice_ID3D11Device->CreateSamplerState
		(
			&sampDesc,
			&m_pTextureSamplerState
		);

		if ( FAILED( hCreateTextureSamplerStateResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateTextureSamplerStateResult,
				L"CreateSamplerState to create texture sampler state failed."
			);

			hOverallResult = hCreateTextureSamplerStateResult;
			goto Cleanup;
		};
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
void CD3D11TexturedPlaneRenderingTechnique::DestroyResources()
{
	if ( m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1 != nullptr )
	{
		//
		// Unbind vertex shaders.
		//

		m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->VSSetShader
		(
			nullptr,
			nullptr,
			0
		);


		//
		// Unbind pixel shaders.
		//

		m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->PSSetShader
		(
			nullptr,
			nullptr,
			0
		);


		//
		// Unbind the input layout.
		//

		m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->IASetInputLayout
		(
			nullptr
		);


		//
		// Unbind the vertex buffer.
		//

		m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->IASetVertexBuffers
		(
			0,
			0,
			nullptr,
			nullptr,
			nullptr
		);


		//
		// Unbind pixel shader resources.
		//

		ID3D11ShaderResourceView * NullShaderResources[] =
		{
			nullptr
		};

		m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->PSSetShaderResources
		(
			0,
			1,
			NullShaderResources
		);


		//
		// Unbind pixel shader samplers.
		//

		m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->PSSetSamplers
		(
			0,
			0,
			nullptr
		);
	};

	SAFE_RELEASE( m_pTexturedObjectVertexShader );
	SAFE_RELEASE( m_pTexturedObjectPixelShader );
	SAFE_RELEASE( m_pTexturedObjectInputLayout );

	SAFE_RELEASE( m_pTextureResourceView );
	SAFE_RELEASE( m_pTextureSamplerState );

	SAFE_RELEASE( m_pTexture );

	SAFE_RELEASE( m_pTexturedPlaneVertexBuffer );
};


///////////////////////////////////////////////////////
// Function:  RenderScene
//
// Purpose:
// Renders a textured plane.
///////////////////////////////////////////////////////
HRESULT CD3D11TexturedPlaneRenderingTechnique::RenderScene
(
	SWAPCHAIN_SLOT /* CurrentSwapChainSlot */
)
{
	//
	// Set the textured-object input layout.
	//

	assert( m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1 != nullptr );
	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->IASetInputLayout
	(
		m_pTexturedObjectInputLayout
	);


	//
	// Set the textured-object shaders.
	//

	assert( m_pTexturedObjectVertexShader != nullptr );
	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->VSSetShader
	(
		m_pTexturedObjectVertexShader,
		nullptr,
		0
	);

	assert( m_pTexturedObjectPixelShader != nullptr );
	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->PSSetShader
	(
		m_pTexturedObjectPixelShader,
		nullptr,
		0
	);


	//
	// Set the plane vertex buffer.
	//

	UINT VertexBufferStrides[] = { sizeof( TexturedVertex ) };
	UINT VertexBufferOffsets[] = { 0 };
	assert( m_pTexturedPlaneVertexBuffer != nullptr );
	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->IASetVertexBuffers
	(
		0,
		1,
		&m_pTexturedPlaneVertexBuffer,
		VertexBufferStrides,
		VertexBufferOffsets
	);


	//
	// Set the texture resource view as an input to the pixel shader.
	//

	assert( m_pTextureResourceView != nullptr );
	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->PSSetShaderResources
	(
		0,
		1,
		&m_pTextureResourceView
	);


	//
	// Set the texture sampler as a sampler input to the pixel shader.
	//

	assert( m_pTextureSamplerState != nullptr );
	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->PSSetSamplers
	(
		0,
		1,
		&m_pTextureSamplerState
	);


	//
	// Set the plane primitive topology.
	//

	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->IASetPrimitiveTopology
	(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);


	//
	// Draw the plane.
	//

	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->Draw( 6, 0 );


	//
	// Unbind the texture sampler.
	//

	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->PSSetSamplers
	(
		0,
		0,
		nullptr
	);


	//
	// Unbind pixel shader resources.
	//

	ID3D11ShaderResourceView * NullShaderResources[] =
	{
		nullptr
	};

	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->PSSetShaderResources
	(
		0,
		1,
		NullShaderResources
	);


	return S_OK;
};
