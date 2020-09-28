//////////////////////////////////////////////////////////////////////
// File:  CD3D11ShadedCubeRenderingTechnique.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a Direct3D 11 rendering technique that renders a
// wrap-shaded cube.
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

#include "CD3D11ShadedCubeRenderingTechnique.hpp"
#include "CD3D11SceneRenderer.hpp"

// Pre-compiled shader blobs
#include "ShadedObjectVertexShader_4_0_level_9_1.h"
#include "ShadedObjectPixelShader_4_0_level_9_1.h"


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
// CD3D11ShadedCubeRenderingTechnique implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D11ShadedCubeRenderingTechnique
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CD3D11ShadedCubeRenderingTechnique::CD3D11ShadedCubeRenderingTechnique
(
	_In_ CD3D11SceneRenderer * pParentRenderer
)
:
	CD3D11RenderingTechnique( pParentRenderer ),
	m_pShadedCubeVertexBuffer( nullptr ),
	m_pShadedObjectVertexShader( nullptr ),
	m_pShadedObjectPixelShader( nullptr ),
	m_pShadedObjectInputLayout( nullptr )
{
};


///////////////////////////////////////////////////////
// Function:  ~CD3D11ShadedCubeRenderingTechnique
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CD3D11ShadedCubeRenderingTechnique::~CD3D11ShadedCubeRenderingTechnique()
{
	assert( m_pShadedObjectVertexShader == nullptr );
	assert( m_pShadedObjectPixelShader == nullptr );
	assert( m_pShadedObjectInputLayout == nullptr );

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
HRESULT CD3D11ShadedCubeRenderingTechnique::CreateResources
(
	SWAPCHAIN_SLOT /* CurrentSwapChainSlot */
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Create the shaded-object vertex shader if needed.
	//

	if ( m_pShadedObjectVertexShader == nullptr )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CD3D11ShadedCubeRenderingTechnique::CreateResources:  Creating shaded-object vertex shader."
		);

		assert( m_pShadedObjectVertexShader == nullptr );
		HRESULT hCreateShadedObjectVertexShaderResult = m_pParentRenderer->m_pDevice_ID3D11Device->CreateVertexShader
		(
			g_ShadedObjectVertexShader,
			ARRAYSIZE( g_ShadedObjectVertexShader ),
			nullptr,
			&m_pShadedObjectVertexShader
		);

		if ( FAILED( hCreateShadedObjectVertexShaderResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateShadedObjectVertexShaderResult,
				L"CreateVertexShader on shaded-object vertex shader failed."
			);

			hOverallResult = hCreateShadedObjectVertexShaderResult;
			goto Cleanup;
		};
	};


	//
	// Create the shaded-object pixel shader if needed.
	//

	if ( m_pShadedObjectPixelShader == nullptr )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CD3D11ShadedCubeRenderingTechnique::CreateResources:  Creating shaded-object pixel shader."
		);

		assert( m_pShadedObjectPixelShader == nullptr );
		HRESULT hCreateShadedObjectPixelShaderResult = m_pParentRenderer->m_pDevice_ID3D11Device->CreatePixelShader
		(
			g_ShadedObjectPixelShader,
			ARRAYSIZE( g_ShadedObjectPixelShader ),
			nullptr,
			&m_pShadedObjectPixelShader
		);

		if ( FAILED( hCreateShadedObjectPixelShaderResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateShadedObjectPixelShaderResult,
				L"CreatePixelShader on shaded-object pixel shader failed."
			);

			hOverallResult = hCreateShadedObjectPixelShaderResult;
			goto Cleanup;
		};
	};


	//
	// Create the shaded-object input layout if needed.
	//

	if ( m_pShadedObjectInputLayout == nullptr )
	{
		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			L"CD3D11ShadedCubeRenderingTechnique::CreateResources:  Creating shaded-object input shader."
		);

		D3D11_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		assert( m_pShadedObjectInputLayout == nullptr );
		HRESULT hCreateShadedObjectInputLayoutResult = m_pParentRenderer->m_pDevice_ID3D11Device->CreateInputLayout
		(
			inputLayout,
			ARRAYSIZE( inputLayout ),
			g_ShadedObjectVertexShader,
			ARRAYSIZE( g_ShadedObjectVertexShader ),
			&m_pShadedObjectInputLayout
		);

		if ( FAILED( hCreateShadedObjectInputLayoutResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateShadedObjectInputLayoutResult,
				L"CreateInputLayout on shaded-object input layout failed."
			);

			hOverallResult = hCreateShadedObjectInputLayoutResult;
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
			L"CD3D11ShadedCubeRenderingTechnique::CreateResources:  Creating cube vertex buffer."
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


		D3D11_BUFFER_DESC ShadedCubeVertexBufferDescriptor;
		ZeroMemory( &ShadedCubeVertexBufferDescriptor, sizeof( ShadedCubeVertexBufferDescriptor ) );
		ShadedCubeVertexBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
		ShadedCubeVertexBufferDescriptor.ByteWidth = sizeof( ShadedCubeVertices );
		ShadedCubeVertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		ShadedCubeVertexBufferDescriptor.CPUAccessFlags = 0;
		ShadedCubeVertexBufferDescriptor.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA ShadedCubeVertexBufferSubresourceData;
		ZeroMemory( &ShadedCubeVertexBufferSubresourceData, sizeof( ShadedCubeVertexBufferSubresourceData ) );
		ShadedCubeVertexBufferSubresourceData.pSysMem = ShadedCubeVertices;
		ShadedCubeVertexBufferSubresourceData.SysMemPitch = 0;
		ShadedCubeVertexBufferSubresourceData.SysMemSlicePitch = 0;

		assert( m_pShadedCubeVertexBuffer == nullptr );
		HRESULT hCreateShadedCubeVertexBufferResult = m_pParentRenderer->m_pDevice_ID3D11Device->CreateBuffer
		(
			&ShadedCubeVertexBufferDescriptor,
			&ShadedCubeVertexBufferSubresourceData,
			&m_pShadedCubeVertexBuffer
		);

		if ( FAILED( hCreateShadedCubeVertexBufferResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateShadedCubeVertexBufferResult,
				L"CreateBuffer on cube vertex buffer failed."
			);

			hOverallResult = hCreateShadedCubeVertexBufferResult;
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
// Releases resources associated with the shaded-cube
// scene.
///////////////////////////////////////////////////////
void CD3D11ShadedCubeRenderingTechnique::DestroyResources()
{
	if ( m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1 != nullptr )
	{
		m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->VSSetShader
		(
			nullptr,
			nullptr,
			0
		);

		m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->PSSetShader
		(
			nullptr,
			nullptr,
			0
		);

		m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->IASetInputLayout
		(
			nullptr
		);

		m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->IASetVertexBuffers
		(
			0,
			0,
			nullptr,
			nullptr,
			nullptr
		);
	};

	SAFE_RELEASE( m_pShadedObjectVertexShader );
	SAFE_RELEASE( m_pShadedObjectPixelShader );
	SAFE_RELEASE( m_pShadedObjectInputLayout );

	SAFE_RELEASE( m_pShadedCubeVertexBuffer );
};


///////////////////////////////////////////////////////
// Function:  RenderScene
//
// Purpose:
// Renders a wrap-shaded cube.
///////////////////////////////////////////////////////
HRESULT CD3D11ShadedCubeRenderingTechnique::RenderScene
(
	SWAPCHAIN_SLOT /* CurrentSwapChainSlot */
)
{
	//
	// Set the shaded-object input layout.
	//

	assert( m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1 != nullptr );
	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->IASetInputLayout( m_pShadedObjectInputLayout );


	//
	// Set the shaded-object shaders.
	//

	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->VSSetShader( m_pShadedObjectVertexShader, nullptr, 0 );
	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->PSSetShader( m_pShadedObjectPixelShader, nullptr, 0 );


	//
	// Set the cube vertex buffer.
	//

	UINT VertexBufferStrides[] = { sizeof( ShadedVertex ) };
	UINT VertexBufferOffsets[] = { 0 };
	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->IASetVertexBuffers
	(
		0,
		1,
		&m_pShadedCubeVertexBuffer,
		VertexBufferStrides,
		VertexBufferOffsets
	);


	//
	// Set the cube primitive topology.
	//

	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );


	//
	// Draw the cube.
	//

	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->Draw( 36, 0 );


	return S_OK;
};
