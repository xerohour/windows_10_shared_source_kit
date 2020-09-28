/************************************************************
 *
 *   File:      DrawD3D10Scene.cpp
 *   Class definitions for drawing a 2D checker pattern in 3D space.
 *   Copyright (c) Microsoft Corporation, 2007
 *
 *   Program:   GetDispSurfData - API Test for IDXGIOutput::GetDisplaySurfaceData() 
 *   Created:   7/07/07
 *   Author:    RichGr
 *
 *   Classes:   D3D10Scene
 *              D3D10VidSurface
 *
 ************************************************************/
#include "DrawD3D10Scene.hpp"

#include <windows.h>
#include <dxgi.h>
#include <StrSafe.h>
#include "Common.hpp"

//--------------------------------------------------------------------------------------
//  Consts
//--------------------------------------------------------------------------------------

const D3D10_INPUT_ELEMENT_DESC  c_inputElemDescForCheckeredSquare[] =
{ 
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
};
 
const UINT       c_nNumElements = ARRAYSIZE( c_inputElemDescForCheckeredSquare );

VERTEX  g_verticesForCheckeredSquare[] = 
{
	{ D3DXVECTOR3( -1.0f,  0.0f,  0.5f ), D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f ) },
	{ D3DXVECTOR3( -1.0f,  1.0f,  0.5f ), D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f ) },
	{ D3DXVECTOR3(  0.0f,  0.0f,  0.5f ), D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f ) },

	{ D3DXVECTOR3(  0.0f,  0.0f,  0.5f ), D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f ) },
	{ D3DXVECTOR3( -1.0f,  1.0f,  0.5f ), D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f ) },
	{ D3DXVECTOR3(  0.0f,  1.0f,  0.5f ), D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f ) },

	{ D3DXVECTOR3(  0.0f,  0.0f,  0.5f ), D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 1.0f ) },
	{ D3DXVECTOR3(  0.0f,  1.0f,  0.5f ), D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 1.0f ) },
	{ D3DXVECTOR3(  1.0f,  0.0f,  0.5f ), D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 1.0f ) },

	{ D3DXVECTOR3(  1.0f,  0.0f,  0.5f ), D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 1.0f ) },
	{ D3DXVECTOR3(  0.0f,  1.0f,  0.5f ), D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 1.0f ) },
	{ D3DXVECTOR3(  1.0f,  1.0f,  0.5f ), D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 1.0f ) },

	{ D3DXVECTOR3( -1.0f, -1.0f,  0.5f ), D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3( -1.0f,  0.0f,  0.5f ), D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3(  0.0f, -1.0f,  0.5f ), D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ) },

	{ D3DXVECTOR3(  0.0f, -1.0f,  0.5f ), D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3( -1.0f,  0.0f,  0.5f ), D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3(  0.0f,  0.0f,  0.5f ), D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ) },

	{ D3DXVECTOR3(  0.0f, -1.0f,  0.5f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3(  0.0f,  0.0f,  0.5f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3(  1.0f, -1.0f,  0.5f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },

	{ D3DXVECTOR3(  1.0f, -1.0f,  0.5f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3(  0.0f,  0.0f,  0.5f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3(  1.0f,  0.0f,  0.5f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
};

const UINT  c_nNumVerticesForCheckeredSquare = ARRAYSIZE(g_verticesForCheckeredSquare);

//--------------------------------------------------------------------------------------
//
// D3D10Scene Implementation
//
//--------------------------------------------------------------------------------------
D3D10Scene::D3D10Scene() :
	m_pSwapChain( NULL ),
	m_pRenderTargetView( NULL ),
	m_pInputLayout( NULL ),
	m_pEffect( NULL ),
	m_pTechnique( NULL ),
	m_pVertexBuffer( NULL ),
	m_pWrappedD3DDevice( NULL )
{
	memset( &m_swapChainDesc, 0, sizeof(m_swapChainDesc) );

	return;
}

//
D3D10Scene::~D3D10Scene()
{
	if ( m_pEffect != NULL )
	{
		m_pEffect->Release();
		m_pEffect = NULL;
	}

	return;
}

//
HRESULT
D3D10Scene::Initialize(
	__in    D3DWrappedDevice*  pWrappedD3DDevice,
	__in    IDXGISwapChain*  pSwapChain,
	__in    DXGI_SWAP_CHAIN_DESC&  swapChainDesc
	)
{
	static const char   c_szEffectFileName[] = "GetDispSurfData.fx";

	HRESULT                 hr = S_OK;
	char                    szMsg[c_nMSG_LEN] = {0};    
	DWORD                   dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG;


	m_pWrappedD3DDevice = pWrappedD3DDevice;
	m_pSwapChain = pSwapChain;
	m_swapChainDesc = swapChainDesc;


	// Load the effect file
	if ( FAILED( hr = pWrappedD3DDevice->CreateEffectFromFile( c_szEffectFileName,
												  NULL,
												  NULL,
												  "fx_4_0",
												  dwShaderFlags,
												  0,
												  m_pWrappedD3DDevice->GetD3D10DevicePtr(),
												  NULL,
												  NULL,
												  &m_pEffect,
												  NULL,
												  NULL
												) ) )
	{
		StringCchPrintf
		(
			szMsg,
			ARRAYSIZE( szMsg ),
			"%s in %s at line %d.  FAILED: D3DWrappedDevice::CreateEffectFromFile failed on file %s - hr = 0x%08x\n",
			__FUNCTION__,
			__FILE__,
			__LINE__,
			c_szEffectFileName,
			hr
		);

		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
	}

	return hr;
}

//
void  D3D10Scene::Uninitialize()
{

	if ( m_pRenderTargetView != NULL )
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = NULL;
	}

	if ( m_pInputLayout != NULL )
	{
		m_pInputLayout->Release();
		m_pInputLayout = NULL;
	}

	if ( m_pVertexBuffer != NULL )
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}

	return;
}

//
void  D3D10Scene::ReleaseViews()
{

	if ( m_pRenderTargetView != NULL )
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = NULL;
	}

	return;
}

//
HRESULT
D3D10Scene::Create3DState(
	__in    float       flColorIntensity
	)
{
	HRESULT         hr = E_FAIL;


	if ( FAILED( hr = SetupRenderTarget() ) ) 
	{
		goto Exit;
	}

	if ( FAILED( hr = SetupInputLayout( flColorIntensity ) ) )
	{
		goto Exit;
	}

	hr = S_OK;

Exit: 
   return hr;
}

//--------------------------------------------------------------------------------------
// Create a render target view 
//--------------------------------------------------------------------------------------
HRESULT  D3D10Scene::SetupRenderTarget()
{
	HRESULT                         hr = E_FAIL;
	ULONG                           ulRefCt = 0;
	char                            szMsg[c_nMSG_LEN] = {0};    
	ID3D10Texture2D                *pBackBuffer = NULL;
	D3D10_TEXTURE2D_DESC            BBDesc = {0};
	D3D10_RENDER_TARGET_VIEW_DESC   RTVDesc;
	D3D10_VIEWPORT                  viewPort = {0};


	// Get the backbuffer
	if ( FAILED( hr = m_pSwapChain->GetBuffer( 0, __uuidof(ID3D10Texture2D), (void**)&pBackBuffer ) ) )
	{
		StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: IDXGISwapChain::GetBuffer failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
		goto Exit;
	}

	// Get the backbuffer desc
	pBackBuffer->GetDesc( &BBDesc );

	// Create the render target view
	memset( &RTVDesc, 0, sizeof(RTVDesc) );

    // The bug #553152 DXGIGetDispSurfData(D3D11on10.0 - CreateRenderTargetView returned E_INVALIDARG( 0x80070057) 
    // with color format R10G10B10_XR_BIAS_A2_UNORM. Not supported at this time. @jianchen 03/05/2009
    if (BBDesc.Format == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM)
    {
        RTVDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
    }
    else
    {
	    RTVDesc.Format = BBDesc.Format;
    }
	RTVDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;

	if ( FAILED( hr = m_pWrappedD3DDevice->GetD3D10DevicePtr()->CreateRenderTargetView( pBackBuffer, &RTVDesc, &m_pRenderTargetView ) ) )
	{
		StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: ID3D10Device::CreateRenderTargetView failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
		goto Exit;
	}

	if ( ( ulRefCt = pBackBuffer->Release() ) != 0 ) 
	{
		StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: ID3D10Texture2D::Release() left RefCount = %d\n", __FUNCTION__, __FILE__, __LINE__, ulRefCt );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
		goto Exit;
	}

	pBackBuffer = NULL;

	// Set the render target
	//
	m_pWrappedD3DDevice->OMSetRenderTargets( 1, &m_pRenderTargetView, NULL );

	// Set Viewport state
	//
	viewPort.Width = m_swapChainDesc.BufferDesc.Width;
	viewPort.Height = m_swapChainDesc.BufferDesc.Height;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;

	m_pWrappedD3DDevice->RSSetViewports( 1, &viewPort );

	hr = S_OK;

Exit:
	return hr;
}

//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT
D3D10Scene::SetupInputLayout(
	__in    float       flColorIntensity
	)
{
	HRESULT                 hr = E_FAIL;
	char                    szMsg[c_nMSG_LEN] = {0};    

	switch( m_pWrappedD3DDevice->GetFeatureLevel() )
	{
	case D3D_FEATURE_LEVEL_9_1:
	case D3D_FEATURE_LEVEL_9_2:
	case D3D_FEATURE_LEVEL_9_3:
		m_pWrappedD3DDevice->GetTechniqueByName( "Render_fl9", &m_pEffect, &m_pTechnique );
		break;
	default:
		m_pWrappedD3DDevice->GetTechniqueByName( "Render", &m_pEffect, &m_pTechnique );
		break;
	}

	// Create the Input Layout
	//
	D3D10_PASS_DESC     passDesc;

	memset( &passDesc, 0, sizeof(passDesc) );

	if ( FAILED( hr = m_pWrappedD3DDevice->Technique_GetPassDescByIndex( &m_pTechnique, 0, &passDesc ) ) )
	{
		StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DWrappedDevice::Technique_GetPassDescByIndex failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
		goto Exit;
	}

	// Set the color intensity using the flColorIntensity input parameter.
	const int   c_nSquares = 4;
	const int   c_nVerticesPerSquare = c_nNumVerticesForCheckeredSquare / c_nSquares;

	for ( int i = 0; i < c_nSquares; i++ )
	{
		for ( int j = 0; j < c_nVerticesPerSquare; j++ )
		{
			switch ( i )
			{
				case 0:
					g_verticesForCheckeredSquare[i * c_nVerticesPerSquare + j].color.x = flColorIntensity;      // Red 
					break;

				case 1:
					g_verticesForCheckeredSquare[i * c_nVerticesPerSquare + j].color.y = flColorIntensity;      // Green 
					break;

				case 2:
					g_verticesForCheckeredSquare[i * c_nVerticesPerSquare + j].color.z = flColorIntensity;      // Blue
					break;

				case 3:
					g_verticesForCheckeredSquare[i * c_nVerticesPerSquare + j].color.x = flColorIntensity;      // Grey/White 
					g_verticesForCheckeredSquare[i * c_nVerticesPerSquare + j].color.y = flColorIntensity;      
					g_verticesForCheckeredSquare[i * c_nVerticesPerSquare + j].color.z = flColorIntensity;      
					break;

				default:
					break;
			}
		}
	}

	if ( FAILED( hr = m_pWrappedD3DDevice->GetD3D10DevicePtr()->CreateInputLayout( &c_inputElemDescForCheckeredSquare[0],
														 c_nNumElements,
														 passDesc.pIAInputSignature,
														 passDesc.IAInputSignatureSize,
														 &m_pInputLayout
													   ) ) )
	{
		StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: ID3D10Device::CreateInputLayout() failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
		goto Exit;
	}

	m_pWrappedD3DDevice->IASetInputLayout( m_pInputLayout );

	// Create the Vertex Buffer
	//
	D3D10_BUFFER_DESC       vertBuffDesc;
	D3D10_SUBRESOURCE_DATA  subResourceData;
	UINT                    nStride = sizeof(VERTEX);
	UINT                    nOffset = 0;

	memset( &vertBuffDesc, 0, sizeof(vertBuffDesc) );
	memset( &subResourceData, 0, sizeof(subResourceData) );
	vertBuffDesc.Usage = D3D10_USAGE_DEFAULT;
	vertBuffDesc.ByteWidth = sizeof(g_verticesForCheckeredSquare);
	vertBuffDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertBuffDesc.CPUAccessFlags = 0;
	vertBuffDesc.MiscFlags = 0;
	subResourceData.pSysMem = g_verticesForCheckeredSquare;

	if ( FAILED( hr = m_pWrappedD3DDevice->GetD3D10DevicePtr()->CreateBuffer( &vertBuffDesc, &subResourceData, &m_pVertexBuffer ) ) )
	{
		StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: ID3D10Device::CreateBuffer() failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
		goto Exit;
	}

	m_pWrappedD3DDevice->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &nStride, &nOffset );

	// Set Primitive Topology
	m_pWrappedD3DDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	hr = S_OK;

Exit:        
	return hr;
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
HRESULT
D3D10Scene::RenderScene()
{
	HRESULT                 hr = E_FAIL;
	char                    szMsg[c_nMSG_LEN] = {0};    
	D3D10_TECHNIQUE_DESC    techDesc = {0};


	// Clear the background to black
	float  clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red, green, blue, alpha

	m_pWrappedD3DDevice->ClearRenderTargetView( m_pRenderTargetView, clearColor );

	if ( FAILED( hr = m_pWrappedD3DDevice->Technique_GetDesc( &m_pTechnique, &techDesc ) ) )
	{
		StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: ID3D10EffectTechnique::GetDesc failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
		goto Exit;
	}

	for ( UINT p = 0; p < techDesc.Passes; p++ )
	{
		if ( FAILED( hr = m_pWrappedD3DDevice->Technique_ApplyPassByIndex( &m_pTechnique, p ) )  )
		{
			StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: ID3D10EffectTechnique::GetPassByIndex failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
			CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
			goto Exit;
		}

		m_pWrappedD3DDevice->Draw( c_nNumVerticesForCheckeredSquare, 0 );
	}

	hr = S_OK;

Exit:
	return hr;
}


//--------------------------------------------------------------------------------------
// Present a frame
//--------------------------------------------------------------------------------------
HRESULT
D3D10Scene::PresentScene()
{
	HRESULT                 hr = S_OK;
	char                    szMsg[c_nMSG_LEN] = {0};    


	if ( FAILED( hr = m_pSwapChain->Present( 0, 0 ) ) )
	{
		StringCchPrintfA( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: IDXGISwapChain::Present failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
	}

	return hr;
}


//--------------------------------------------------------------------------------------
//
// D3D10VidSurface Implementation
//
//--------------------------------------------------------------------------------------
D3D10VidSurface::D3D10VidSurface() :
	m_pSwapChain( NULL ),
	m_pVidMemTex( NULL ),
	m_pRenderTargetView( NULL ),
	m_pInputLayout( NULL ),
	m_pEffect( NULL ),
	m_pTechnique( NULL ),
	m_pVertexBuffer( NULL ),
	m_pWrappedD3DDevice( NULL )
{

	memset( &m_swapChainDesc, 0, sizeof(m_swapChainDesc) );

	return;
}

//
D3D10VidSurface::~D3D10VidSurface()
{

	if ( m_pEffect != NULL )
	{
		m_pEffect->Release();
		m_pEffect = NULL;
	}

	return;
}

//
HRESULT
D3D10VidSurface::Initialize(
	__in    D3DWrappedDevice*  pWrappedD3DDevice,
	__in    IDXGISwapChain*  pSwapChain,
	__in    DXGI_SWAP_CHAIN_DESC&  swapChainDesc
	)
{
	static const char   c_szEffectFileName[] = "GetDispSurfData.fx";

	HRESULT                 hr = S_OK;
	char                    szMsg[c_nMSG_LEN] = {0};    
	DWORD                   dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG;

	m_pSwapChain = pSwapChain;
	m_swapChainDesc = swapChainDesc;
	m_pWrappedD3DDevice = pWrappedD3DDevice;


	// Load the effect file
	if ( FAILED( hr = pWrappedD3DDevice->CreateEffectFromFile( c_szEffectFileName,
												  NULL,
												  NULL,
												  "fx_4_0",
												  dwShaderFlags,
												  0,
												  m_pWrappedD3DDevice->GetD3D10DevicePtr(),
												  NULL,
												  NULL,
												  &m_pEffect,
												  NULL,
												  NULL
												) ) )
	{
		StringCchPrintf
		(
			szMsg,
			ARRAYSIZE(szMsg),
			"%s in %s at line %d.  FAILED: D3DWrappedDevice::CreateEffectFromFile failed on file %s - hr = 0x%08x\n",
			__FUNCTION__,
			__FILE__,
			__LINE__,
			c_szEffectFileName,
			hr
		);

		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
	}

   return hr;
}

//
void  D3D10VidSurface::Uninitialize()
{

	if ( m_pVidMemTex != NULL )
	{
		m_pVidMemTex->Release();
		m_pVidMemTex = NULL;
	}

	if ( m_pRenderTargetView != NULL )
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = NULL;
	}

	if ( m_pInputLayout != NULL )
	{
		m_pInputLayout->Release();
		m_pInputLayout = NULL;
	}

	if ( m_pVertexBuffer != NULL )
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}

	return;
}

//
void  D3D10VidSurface::ReleaseViews()
{

	if ( m_pRenderTargetView != NULL )
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = NULL;
	}

	return;
}

//
HRESULT
D3D10VidSurface::Create3DState(
	__in    UINT        width,
	__in    UINT        height,
	__in    DXGI_FORMAT nFormat,
	__in    float       flColorIntensity
	)
{
	HRESULT         hr = E_FAIL;


	if ( FAILED( hr = SetupRenderTarget( width, height, nFormat ) ) ) 
	{
		goto Exit;
	}

	if ( FAILED( hr = SetupInputLayout( flColorIntensity ) ) )
	{
		goto Exit;
	}

	hr = S_OK;

Exit: 
   return hr;
}

//--------------------------------------------------------------------------------------
// Create a render target view 
//--------------------------------------------------------------------------------------
HRESULT
D3D10VidSurface::SetupRenderTarget(
	__in    UINT        width,
	__in    UINT        height,
	__in    DXGI_FORMAT nFormat
	)
{
	HRESULT                         hr = E_FAIL;
	ULONG                           ulRefCt = 0;
	char                            szMsg[c_nMSG_LEN] = {0};    
	D3D10_TEXTURE2D_DESC            tex2DVidDesc;
	D3D10_RENDER_TARGET_VIEW_DESC   rtvDesc;
	D3D10_VIEWPORT                  viewPort = {0};


	memset( &tex2DVidDesc, 0, sizeof(tex2DVidDesc) );
	tex2DVidDesc.Width = width;
	tex2DVidDesc.Height = height;
	tex2DVidDesc.MipLevels = 1;
	tex2DVidDesc.ArraySize = 1;
	tex2DVidDesc.Format = nFormat;
	tex2DVidDesc.SampleDesc.Count = 1;
	tex2DVidDesc.Usage = D3D10_USAGE_DEFAULT;
	tex2DVidDesc.BindFlags = D3D10_BIND_RENDER_TARGET;
	tex2DVidDesc.CPUAccessFlags = 0;

	if ( FAILED( hr = m_pWrappedD3DDevice->GetD3D10DevicePtr()->CreateTexture2D( &tex2DVidDesc, NULL, &m_pVidMemTex ) ) )
	{
		StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: ID3D10Device::CreateTexture2D failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
		goto Exit;
	}
    
	// Create the render target view
	memset( &rtvDesc, 0, sizeof(rtvDesc) );
	rtvDesc.Format = nFormat;
	rtvDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	if ( FAILED( hr = m_pWrappedD3DDevice->GetD3D10DevicePtr()->CreateRenderTargetView( m_pVidMemTex, &rtvDesc, &m_pRenderTargetView ) ) )
	{
		StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: ID3D10Device::CreateRenderTargetView failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
		goto Exit;
	}

	// Set the render target
	//
	m_pWrappedD3DDevice->OMSetRenderTargets( 1, &m_pRenderTargetView, NULL );

	// Set Viewport state
	//
	viewPort.Width = width;
	viewPort.Height = height;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;

	m_pWrappedD3DDevice->RSSetViewports( 1, &viewPort );

	hr = S_OK;

Exit:
	return hr;
}

//--------------------------------------------------------------------------------------
// Create remaining D3D10 resources
//--------------------------------------------------------------------------------------
HRESULT
D3D10VidSurface::SetupInputLayout(
	__in    float       flColorIntensity
	)
{
	HRESULT                 hr = E_FAIL;
	char                    szMsg[c_nMSG_LEN] = {0};    


	switch( m_pWrappedD3DDevice->GetFeatureLevel() )
	{
	case D3D_FEATURE_LEVEL_9_1:
	case D3D_FEATURE_LEVEL_9_2:
	case D3D_FEATURE_LEVEL_9_3:
		m_pWrappedD3DDevice->GetTechniqueByName( "Render_fl9", &m_pEffect, &m_pTechnique );
		break;
	default:
		m_pWrappedD3DDevice->GetTechniqueByName( "Render", &m_pEffect, &m_pTechnique );
		break;
	}

	// Create the Input Layout
	//
	D3D10_PASS_DESC     passDesc;

	memset( &passDesc, 0, sizeof(passDesc) );

	if ( FAILED( hr = m_pWrappedD3DDevice->Technique_GetPassDescByIndex( &m_pTechnique, 0, &passDesc ) ) )
	{
		StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DWrappedDevice::Technique_GetPassDescByIndex failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
		goto Exit;
	}

	// Set the color intensity using the flColorIntensity input parameter.
	const int   c_nSquares = 4;
	const int   c_nVerticesPerSquare = c_nNumVerticesForCheckeredSquare / c_nSquares;

	for ( int i = 0; i < c_nSquares; i++ )
	{
		for ( int j = 0; j < c_nVerticesPerSquare; j++ )
		{
			switch ( i )
			{
				case 0:
					g_verticesForCheckeredSquare[i * c_nVerticesPerSquare + j].color.x = flColorIntensity;      // Red 
					break;

				case 1:
					g_verticesForCheckeredSquare[i * c_nVerticesPerSquare + j].color.y = flColorIntensity;      // Green 
					break;

				case 2:
					g_verticesForCheckeredSquare[i * c_nVerticesPerSquare + j].color.z = flColorIntensity;      // Blue
					break;

				case 3:
					g_verticesForCheckeredSquare[i * c_nVerticesPerSquare + j].color.x = flColorIntensity;      // Grey/White 
					g_verticesForCheckeredSquare[i * c_nVerticesPerSquare + j].color.y = flColorIntensity;      
					g_verticesForCheckeredSquare[i * c_nVerticesPerSquare + j].color.z = flColorIntensity;      
					break;

				default:
					break;
			}
		}
	}

	if ( FAILED( hr = m_pWrappedD3DDevice->GetD3D10DevicePtr()->CreateInputLayout( &c_inputElemDescForCheckeredSquare[0],
														 c_nNumElements,
														 passDesc.pIAInputSignature,
														 passDesc.IAInputSignatureSize,
														 &m_pInputLayout
													   ) ) )
	{
		StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: ID3D10Device::CreateInputLayout() failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
		goto Exit;
	}

	m_pWrappedD3DDevice->IASetInputLayout( m_pInputLayout );

	// Create the Vertex Buffer
	//
	D3D10_BUFFER_DESC       vertBuffDesc;
	D3D10_SUBRESOURCE_DATA  subResourceData;
	UINT                    nStride = sizeof(VERTEX);
	UINT                    nOffset = 0;

	memset( &vertBuffDesc, 0, sizeof(vertBuffDesc) );
	memset( &subResourceData, 0, sizeof(subResourceData) );
	vertBuffDesc.Usage = D3D10_USAGE_DEFAULT;
	vertBuffDesc.ByteWidth = sizeof(g_verticesForCheckeredSquare);
	vertBuffDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertBuffDesc.CPUAccessFlags = 0;
	vertBuffDesc.MiscFlags = 0;
	subResourceData.pSysMem = g_verticesForCheckeredSquare;

	if ( FAILED( hr = m_pWrappedD3DDevice->GetD3D10DevicePtr()->CreateBuffer( &vertBuffDesc, &subResourceData, &m_pVertexBuffer ) ) )
	{
		StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: ID3D10Device::CreateBuffer() failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
		goto Exit;
	}

	m_pWrappedD3DDevice->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &nStride, &nOffset );

	// Set Primitive Topology
	m_pWrappedD3DDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	hr = S_OK;

Exit:        
	return hr;
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
HRESULT
D3D10VidSurface::RenderScene(
	__in    ID3D10Texture2D   **ppVidMemTex
	)
{
	HRESULT                 hr = E_FAIL;
	char                    szMsg[c_nMSG_LEN] = {0};    
	D3D10_TECHNIQUE_DESC    techDesc = {0};


	if ( ppVidMemTex == NULL )
	{
		StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: ID3D10Texture2D** is NULL\n", __FUNCTION__, __FILE__, __LINE__ );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
		goto Exit;
	}

	// Clear the background to black
	float  clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red, green, blue, alpha

	m_pWrappedD3DDevice->ClearRenderTargetView( m_pRenderTargetView, clearColor );

	if ( FAILED( hr = m_pWrappedD3DDevice->Technique_GetDesc( &m_pTechnique, &techDesc ) ) )
	{
		StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: D3DWrappedDevice::TechniqueGetDesc failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
		CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
		goto Exit;
	}

	for ( UINT p = 0; p < techDesc.Passes; p++ )
	{
		if ( FAILED( hr = m_pWrappedD3DDevice->Technique_ApplyPassByIndex( &m_pTechnique, p ) ) )
		{
			StringCchPrintf( szMsg, ARRAYSIZE(szMsg), "%s in %s at line %d.  FAILED: ID3D10EffectTechnique::GetPassByIndex failed - hr = 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, hr );
			CCommon::WriteMsgs( szMsg, ARRAYSIZE(szMsg) );
			goto Exit;
		}

		m_pWrappedD3DDevice->Draw( c_nNumVerticesForCheckeredSquare, 0 );
	}

	// Pass back a pointer to the VidMem surface that has been created
	//
	*ppVidMemTex = m_pVidMemTex;
	((ID3D10Texture2D*)*ppVidMemTex)->AddRef();

	hr = S_OK;

Exit:
	return hr;
}
