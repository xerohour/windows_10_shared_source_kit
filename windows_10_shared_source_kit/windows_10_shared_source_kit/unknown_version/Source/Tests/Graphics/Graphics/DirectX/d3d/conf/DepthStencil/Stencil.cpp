#include "Stencil.h"

struct VERTEX 
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 col;
};

///////////////////////////////////////////////////////////////////////////////
tstring ToString( const StencilBitMask &type )
{
	char szStr[9];
	szStr[8] = '\0';

	for ( UINT i = 0; i < 8; ++i )
	{
		if ( type.Mask & (0x80 >> i) )
			szStr[i] = '1';
		else
			szStr[i] = '0';
	}

	return tstring(szStr);
}
///////////////////////////////////////////////////////////////////////////////
bool FromString( StencilBitMask *pPos, const tstring &str )
{
	pPos->Mask = 0;

	unsigned int strLen = str.length();
	const char *szStr = str.c_str();

	for ( UINT i = 0; i < strLen; ++i )
	{
		switch (szStr[i])
		{
		case '0':
			continue;

		case '1':
			pPos->Mask += 1 << ( strLen - i - 1 );
			
			break;

		default:
			return false;
		}
	}

	return true;
}
	
///////////////////////////////////////////////////////////////////////////////
CStencilTest::CStencilTest() : 
	m_pDSState(NULL),
	m_pRTCopy(NULL),
	m_pConstBuffer(NULL),
	m_pVertexBuffer(NULL),
	m_pVertexLayout(NULL),
	m_pRS(NULL),
	m_pBS(NULL),
	m_pVS(NULL),
	m_pPS(NULL),
	m_RTHeight(10),
	m_RTWidth(10)
{
    // Cache framework settings changed by this test
    m_origRenderTargetOptions.SwapChainDesc.Height  = g_TestApp.m_RenderTargetOptions.SwapChainDesc.Height;
    m_origRenderTargetOptions.SwapChainDesc.Width   = g_TestApp.m_RenderTargetOptions.SwapChainDesc.Width;
    m_origRenderTargetOptions.NoDepthStencils       = g_TestApp.m_RenderTargetOptions.NoDepthStencils;
    m_origRenderTargetOptions.NoRenderTargets       = g_TestApp.m_RenderTargetOptions.NoRenderTargets;
    m_origRenderTargetOptions.NoSwapChains          = g_TestApp.m_RenderTargetOptions.NoSwapChains;

    g_TestApp.m_RenderTargetOptions.SwapChainDesc.Height = m_RTHeight;
	g_TestApp.m_RenderTargetOptions.SwapChainDesc.Width = m_RTWidth;
	// TODO is DepthStencil really allowed to be diff size from RT? D3D complains with error,
	// but sets it anyway.  Leaving code as is for now.
	//g_TestApp.m_RenderTargetOptions.DepthTextureDesc.Height = m_RTHeight;
	//g_TestApp.m_RenderTargetOptions.DepthTextureDesc.Width = m_RTWidth;
	g_TestApp.m_RenderTargetOptions.NoDepthStencils = false;
    g_TestApp.m_RenderTargetOptions.NoRenderTargets = false;
    g_TestApp.m_RenderTargetOptions.NoSwapChains = false;
}

CStencilTest::~CStencilTest()
{
    // Restore framework settings changed by this test
    g_TestApp.m_RenderTargetOptions.SwapChainDesc.Height  = m_origRenderTargetOptions.SwapChainDesc.Height;
    g_TestApp.m_RenderTargetOptions.SwapChainDesc.Width   = m_origRenderTargetOptions.SwapChainDesc.Width;
    g_TestApp.m_RenderTargetOptions.NoDepthStencils       = m_origRenderTargetOptions.NoDepthStencils;
    g_TestApp.m_RenderTargetOptions.NoRenderTargets       = m_origRenderTargetOptions.NoRenderTargets;
    g_TestApp.m_RenderTargetOptions.NoSwapChains          = m_origRenderTargetOptions.NoSwapChains;
}

///////////////////////////////////////////////////////////////////////////////
TEST_RESULT	CStencilTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	ID3D11Device *pDevice = g_TestApp.GetDevice();
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	if ( FAILED( pDevice->CreateDepthStencilState( &m_dsDesc, &m_pDSState ) ) )
	{
		WriteToLog( _T("Failed to create test DepthStencilState.") );
		tRes = RESULT_FAIL;
		goto FAIL;
	}
	
	memcpy( &dsDesc, &m_dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC) );
	dsDesc.StencilEnable = true;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	if ( FAILED( pDevice->CreateDepthStencilState( &dsDesc, &m_pDSVerify ) ) )
	{
		WriteToLog( _T("Failed to create verification DepthStencilState.") );
		tRes = RESULT_FAIL;
	}
	
FAIL:
	return tRes;
}

///////////////////////////////////////////////////////////////////////////////
void		CStencilTest::CleanupTestCase()
{
	ID3D11Device *pDevice = NULL;

	pDevice = g_TestApp.GetDevice();
	if ( pDevice ) //otherwise, we failed to create the device in the first place
	{
		GetEffectiveContext()->OMSetDepthStencilState( NULL, 0 );
		SAFE_RELEASE( m_pDSState );
		SAFE_RELEASE( m_pDSVerify );
	}
}

///////////////////////////////////////////////////////////////////////////////
TEST_RESULT	CStencilTest::Setup()
{
	ID3D11Device *pDevice = g_TestApp.GetDevice();
	ID3D10Blob *pVSBuf = NULL, *pPSBuf = NULL, *pErrBuf = NULL;

	Cleanup();

	//
	// Setup the default viewport
	//
	D3D11_VIEWPORT vp;
	vp.Height = (float) m_RTHeight;
	vp.Width = (float) m_RTWidth;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	GetEffectiveContext()->RSSetViewports( 1, &vp );

	//
	// Set Rasterizer state
	//
	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = TRUE;
	rastDesc.DepthBias = FALSE;
	rastDesc.DepthBiasClamp = 0;
	rastDesc.SlopeScaledDepthBias = 0;
	rastDesc.DepthClipEnable = TRUE;
	rastDesc.ScissorEnable = FALSE;
	rastDesc.MultisampleEnable = FALSE;
	rastDesc.AntialiasedLineEnable = FALSE;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	if( FAILED( pDevice->CreateRasterizerState( &rastDesc, &m_pRS ) ) )
	{
		WriteToLog( _T("CreateRasterizerState for m_pRS failed.") );
		return RESULT_FAIL;
	}

	GetEffectiveContext()->RSSetState( m_pRS );
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	//
	// Set Blend state
	//
	D3D11_BLEND_DESC blendDesc;	
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[1].BlendEnable = FALSE;
	blendDesc.RenderTarget[2].BlendEnable = FALSE;
	blendDesc.RenderTarget[3].BlendEnable = FALSE;
	blendDesc.RenderTarget[4].BlendEnable = FALSE;
	blendDesc.RenderTarget[5].BlendEnable = FALSE;
	blendDesc.RenderTarget[6].BlendEnable = FALSE;
	blendDesc.RenderTarget[7].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0xf;
	blendDesc.RenderTarget[1].RenderTargetWriteMask = 0xf;
	blendDesc.RenderTarget[2].RenderTargetWriteMask = 0xf;
	blendDesc.RenderTarget[3].RenderTargetWriteMask = 0xf;
	blendDesc.RenderTarget[4].RenderTargetWriteMask = 0xf;
	blendDesc.RenderTarget[5].RenderTargetWriteMask = 0xf;
	blendDesc.RenderTarget[6].RenderTargetWriteMask = 0xf;
	blendDesc.RenderTarget[7].RenderTargetWriteMask = 0xf;
	if( FAILED( pDevice->CreateBlendState( &blendDesc, &m_pBS ) ) )
	{
		WriteToLog( _T("CreateBlendState failed.") );
		return RESULT_FAIL;
	}

	GetEffectiveContext()->OMSetBlendState( m_pBS, 0, D3D10_DEFAULT_SAMPLE_MASK );

	//
	// Create the constant buffer
	//
	D3D11_BUFFER_DESC bufdesc;
	bufdesc.ByteWidth = 32;
	bufdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufdesc.MiscFlags = 0;
	if( FAILED( pDevice->CreateBuffer( &bufdesc, NULL, &m_pConstBuffer ) ) )
	{
		WriteToLog( _T("CreateBuffer for m_pConstBuffer failed.") );
		return RESULT_FAIL;
	}

	GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );

	//
	// Create Shaders
	//

	if ( FAILED( D3DX10CompileFromResource(NULL, "stencilvs.vsh", NULL, NULL, NULL, "main", "vs_4_0", 0, 0, NULL, &pVSBuf, &pErrBuf, NULL ) ) )
	{
		WriteToLog( _T("CompileShaderFromResource Failed.") );

		if ( pErrBuf )
		{
			LPCSTR szErr = (LPCSTR)pErrBuf->GetBufferPointer();		
			WriteToLog(szErr);
		}
		SAFE_RELEASE(pErrBuf);
		return RESULT_FAIL;
	}

	if ( FAILED( pDevice->CreateVertexShader( (DWORD*) pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), NULL, &m_pVS ) ) )
	{
		WriteToLog( _T("CreateVertexShader Failed.") );
		return RESULT_FAIL;
	}

	if ( FAILED( D3DX10CompileFromResource(NULL, "stencilps.psh", NULL, NULL, NULL, "main", "ps_4_0", 0, 0, NULL, &pPSBuf, &pErrBuf, NULL) ) )
	{
		WriteToLog( _T("CompileShaderFromResource Failed.") );

		if ( pErrBuf )
		{
			LPCSTR szErr = (LPCSTR)pErrBuf->GetBufferPointer();		
			WriteToLog(szErr);
		}		
		SAFE_RELEASE(pErrBuf);
		return RESULT_FAIL;
	}

	if ( FAILED( pDevice->CreatePixelShader( (DWORD*) pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &m_pPS ) ) )
	{
		WriteToLog( _T("CreatePixelShader Failed.") );
		return RESULT_FAIL;
	}

	SAFE_RELEASE( pPSBuf );

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );

	// Define our vertex data layout
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "pos", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "col", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if(	FAILED(	pDevice->CreateInputLayout( layout, 2, pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), &m_pVertexLayout ) ) )
	{
		WriteToLog( _T("CreateInputLayout for m_pVertexLayout Failed.") );
		return RESULT_FAIL;
	}

	SAFE_RELEASE( pVSBuf );

	GetEffectiveContext()->IASetInputLayout( m_pVertexLayout );

	//
	// Create vertex buffer
	//
	D3D11_BUFFER_DESC bufferDesc =
	{
		100 * sizeof( VERTEX ),
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_VERTEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE,
			0
	};
	if( FAILED( pDevice->CreateBuffer( &bufferDesc, NULL, &m_pVertexBuffer ) ) )
	{
		WriteToLog( _T("CreateBuffer for m_pVertexBuffer Failed.") );
		return RESULT_FAIL;
	}

	UINT strides[] = { sizeof( VERTEX ) };
	UINT offsets[] = { 0 };
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, strides, offsets );

	HRESULT hr;
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Execute Deferred Context Failed.") );
		return RESULT_FAIL;
	}

	//
	// setup const data
	//
	float *bufData;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if( FAILED( GetEffectiveContext()->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
	{
		WriteToLog( _T("Map on m_pConstBuffer Failed") );
		return RESULT_FAIL;
	}
	bufData = (float*) mappedRes.pData;

	// Inverse viewport scale.
	bufData[2] = 2;
	bufData[3] = -2;
	// Prescaled inverse viewport translation.
	bufData[0] = -1.0f;
	bufData[1] = 1.0f;

	GetEffectiveContext()->Unmap(m_pConstBuffer,0);

	D3D11_TEXTURE2D_DESC texDesc;

	texDesc.ArraySize = 1;
	texDesc.BindFlags = 0;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	texDesc.Format = g_TestApp.m_RenderTargetOptions.SwapChainDesc.Format;
	texDesc.Height = m_RTHeight;
	texDesc.Width = m_RTWidth;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;

	if ( FAILED(pDevice->CreateTexture2D( &texDesc, NULL, &m_pRTCopy )) )
	{
		WriteToLog( _T("Failed to create staging texture.") );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}
///////////////////////////////////////////////////////////////////////////////
void		CStencilTest::Cleanup()
{
	ID3D11Device *pDevice = NULL;
	ID3D11Buffer* pBufferNull[] = { NULL };
	ID3D11Buffer *pNullBuffers[] = { NULL };
	UINT offsets[] = { 0 };
	
	pDevice = g_TestApp.GetDevice();
	if ( pDevice ) //otherwise, we failed to create the device in the first place
	{
		GetEffectiveContext()->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );
		GetEffectiveContext()->IASetInputLayout( NULL );
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, pNullBuffers, offsets, offsets );
		GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->VSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->RSSetState( NULL );

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to Execute Deferred Context Cleanup.") );
		}

		SAFE_RELEASE( m_pVS );
		SAFE_RELEASE( m_pPS );

		SAFE_RELEASE( m_pConstBuffer );

		SAFE_RELEASE( m_pRS );
		SAFE_RELEASE( m_pBS );

		SAFE_RELEASE( m_pVertexLayout );
		SAFE_RELEASE( m_pVertexBuffer );
		SAFE_RELEASE( m_pRTCopy );
	}
}
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT	CStencilTest::DrawFront( FLOAT depth )
{
	VERTEX *pDestVerts;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	ID3D11Device *pDevice = g_TestApp.GetDevice();

	//Fill vertex buffer
	if( FAILED( GetEffectiveContext()->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
	{
		WriteToLog( _T("Map on m_pVertexBuffer failed." ));
		return RESULT_FAIL;
	}
	pDestVerts = (VERTEX*) mappedRes.pData;
	
	pDestVerts[0].pos = D3DXVECTOR3(0, 0, depth);
	pDestVerts[0].col = D3DXVECTOR3(1, 0, 0);

	pDestVerts[1].pos = D3DXVECTOR3(0, 0.5, depth);
	pDestVerts[1].col = D3DXVECTOR3(1, 0, 0);

	pDestVerts[2].pos = D3DXVECTOR3(1, 0, depth);
	pDestVerts[2].col = D3DXVECTOR3(1, 0, 0);

	pDestVerts[3].pos = D3DXVECTOR3(1, 0.5, depth);
	pDestVerts[3].col = D3DXVECTOR3(1, 0, 0);

	GetEffectiveContext()->Unmap(m_pVertexBuffer,0);
	GetEffectiveContext()->Draw( 4, 0 );

	HRESULT hr;
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to Execute Deferred Context.") );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT	CStencilTest::DrawBack( FLOAT depth )
{
	VERTEX *pDestVerts;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	ID3D11Device *pDevice = g_TestApp.GetDevice();

	//Fill vertex buffer
	if( FAILED( GetEffectiveContext()->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
	{
		WriteToLog( _T("Map on m_pVertexBuffer failed.") );
		return RESULT_FAIL;
	}
	pDestVerts = (VERTEX*) mappedRes.pData;

	pDestVerts[0].pos = D3DXVECTOR3(0, 0.5, depth);
	pDestVerts[0].col = D3DXVECTOR3(0, 1, 0);

	pDestVerts[1].pos = D3DXVECTOR3(1, 0.5, depth);
	pDestVerts[1].col = D3DXVECTOR3(0, 1, 0);

	pDestVerts[2].pos = D3DXVECTOR3(0, 1, depth);
	pDestVerts[2].col = D3DXVECTOR3(0, 1, 0);	

	pDestVerts[3].pos = D3DXVECTOR3(1, 1, depth);
	pDestVerts[3].col = D3DXVECTOR3(0, 1, 0);

	GetEffectiveContext()->Unmap(m_pVertexBuffer,0);
	GetEffectiveContext()->Draw( 4, 0 );

	HRESULT hr;
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to Execute Deferred Context.") );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT CStencilTest::Verify( VERIFYRANGE range, bool expectBlack )
{
	TEST_RESULT tRes = RESULT_PASS;
	ID3D11Device		*pDevice = g_TestApp.GetDevice();
	ID3D11Texture2D	*pRT = g_TestApp.GetRTTexture();
	
	GetEffectiveContext()->CopyResource( m_pRTCopy, pRT );

	HRESULT hr;
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to Execute Deferred Context.") );
		return RESULT_FAIL;
	}

	D3D11_MAPPED_SUBRESOURCE tMap;
	if ( FAILED(GetImmediateContext()->Map( m_pRTCopy, 0, D3D11_MAP_READ, 0, &tMap )) )
	{
		WriteToLog( _T("Failed to map staging texture.") );
		return RESULT_FAIL;
	}	
	
	
	UINT start, end;
	switch( range )
	{
	case TOP:
		start = 0;
		end = m_RTHeight/2;
		break;

	case BOTTOM:
		start = m_RTHeight/2;
		end = m_RTHeight;
		break;

	case BOTH:
		start = 0;
		end = m_RTHeight;
		break;

	default:
		WriteToLog( _T("Invalid range.") );
		return RESULT_FAIL;
	}

	UINT8 *pData;
	for ( UINT y = 0; y < m_RTHeight; ++y )
	{
		pData = (UINT8*)tMap.pData + y * tMap.RowPitch / sizeof(UINT8); //Should be div by 1.  Here in case format changes

		for ( UINT x = 0; x < m_RTWidth; ++x )
		{
			if ( expectBlack || y < start || y >= end )
			{
				for ( UINT i = 0; i < 4; ++i )
				{
					if ( pData[i] != 0 )
					{
						WriteToLog( _T("Color found where black should be.") );
						tRes = RESULT_FAIL;
						goto QUIT;
					}
				}
			}
			else
			{
				if ( y < m_RTHeight/2 ) //If we're on the top/front half
				{
					if ( pData[0] != 0xFF )
					{
						WriteToLog( _T("Wrong value found in red channel for front face.") );
						tRes = RESULT_FAIL;
						goto QUIT;
					}
					for ( UINT i = 1; i < 3; ++i )
					{
						if ( pData[i] != 0x00 )
						{
							WriteToLog( _T("Color found where black should be for front face.") );
							tRes = RESULT_FAIL;
							goto QUIT;
						}
					}
					if ( pData[3] != 0xFF )
					{
						WriteToLog( _T("Wrong value found in alpha channel for front face.") );
						tRes = RESULT_FAIL;
						goto QUIT;
					}
				}
				else
				{
					for ( UINT i = 0; i < 3; ++i )
					{
						if ( 1 == i ) continue;

						if ( pData[i] != 0x00 )
						{
							WriteToLog( _T("Color found where black should be for back face.") );
							tRes = RESULT_FAIL;
							goto QUIT;
						}

					}
					if ( pData[1] != 0xFF )
					{
						WriteToLog( _T("Wrong value found in green channel for front face.") );
						tRes = RESULT_FAIL;
						goto QUIT;
					}
					if ( pData[3] != 0xFF )
					{
						WriteToLog( _T("Wrong value found in alpha channel for back face.") );
						tRes = RESULT_FAIL;
						goto QUIT;
					}
				}
			}

			pData += 4;
		}
	}

QUIT:
	GetImmediateContext()->Unmap( m_pRTCopy, 0 );

	return tRes;
}
///////////////////////////////////////////////////////////////////////////////
HRESULT CStencilTest::ClearDepthStencil( FLOAT depthVal, UINT8 stencilVal )
{
	// create a separate view just to clear, because g_TestApp.GetDSView() might be read-only
	ID3D11DepthStencilView *tempDsv = NULL;
	HRESULT hr = g_TestApp.GetDevice()->CreateDepthStencilView( g_TestApp.GetDSTexture(), NULL, &tempDsv );
	if( FAILED( hr ) )
	{
		WriteToLog( _T("Failed to CreateDepthStencilView.") );
		goto END;
	}
	GetEffectiveContext()->ClearDepthStencilView( tempDsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthVal, stencilVal );
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to Execute Deferred Context.") );
		goto END;
	}

END:
	SAFE_RELEASE( tempDsv );
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
CStencilOps::CStencilOps()
{
	m_dsDesc.DepthEnable = FALSE;
	m_dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	m_dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	m_dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	m_dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	
	m_dsDesc.StencilReadMask = 0xFF;
	m_dsDesc.StencilWriteMask = 0xFF;

	m_dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER;
	m_dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_LESS;

	g_TestApp.m_RenderTargetOptions.UseDefaultDepthStencilView = false;
	g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
}
///////////////////////////////////////////////////////////////////////////////
void		CStencilOps::InitTestParameters()
{
	CUserValueSet<D3D11_STENCIL_OP> *pStencilOps = new CUserValueSet<D3D11_STENCIL_OP>();
	pStencilOps->SetFromNamedValues();

	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T("Format"), &g_TestApp.m_RenderTargetOptions.DepthTextureDesc.Format );
	testfactor::RFactor rfFormat = AddParameterValueSet( pFormatParam, FormatValueSet( FF_S, 0 ) );

	CTestCaseParameter<UINT> *pViewFlag = AddParameter( _T("DepthStencilViewDesc_Flags"), &g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.Flags );
	pViewFlag->SetDefaultValue( 0 );
	CUserValueSet<UINT> *pViewFlagValueSet = new CUserValueSet<UINT>();
	pViewFlagValueSet->AddValue( 0 );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ) // not available for 10_*
	{
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_STENCIL );
	}
	testfactor::RFactor rfViewFlag = AddParameterValueSet( pViewFlag, pViewFlagValueSet );

	CTestCaseParameter<BOOL> *pStencilEnable = AddParameter( _T("StencilEnable"), &m_dsDesc.StencilEnable );
	testfactor::RFactor rfStencilOff = AddParameterValue( pStencilEnable, 0 );
	testfactor::RFactor rfStencilOn = AddParameterValue( pStencilEnable, 1 );

	CTestCaseParameter<D3D11_STENCIL_OP> *pFrontPassOp, *pBackPassOp;

	pFrontPassOp = AddParameter( _T("FrontFace_StencilPassOp"), &m_dsDesc.FrontFace.StencilPassOp );
	pFrontPassOp->SetDefaultValue( D3D11_STENCIL_OP_INCR );
	testfactor::RFactor rfFrontStencilPass = AddParameterValueSet( pFrontPassOp, pStencilOps );

	pBackPassOp = AddParameter( _T("BackFace_StencilPassOp"), &m_dsDesc.BackFace.StencilPassOp );
	pBackPassOp->SetDefaultValue( D3D11_STENCIL_OP_DECR );
	testfactor::RFactor rfBackStencilPass = AddParameterValueSet( pBackPassOp, pStencilOps );

	CTestCaseParameter<D3D11_STENCIL_OP> *pFrontFailOp, *pBackFailOp;
	
	pFrontFailOp = AddParameter( _T("FrontFace_StencilFailOp"), &m_dsDesc.FrontFace.StencilFailOp );
	pFrontFailOp->SetDefaultValue( D3D11_STENCIL_OP_DECR );
	testfactor::RFactor rfFrontStencilFail = AddParameterValueSet( pFrontFailOp, pStencilOps );
	
	pBackFailOp = AddParameter( _T("BackFace_StencilFailOp"), &m_dsDesc.BackFace.StencilFailOp );
	pBackFailOp->SetDefaultValue( D3D11_STENCIL_OP_INCR );
	testfactor::RFactor rfBackStencilFail = AddParameterValueSet( pBackFailOp, pStencilOps );
	
	
	
	SetRootTestFactor( rfFormat * ( rfStencilOff + rfStencilOn * 
		rfFrontStencilPass * rfBackStencilPass * rfFrontStencilFail * rfBackStencilFail ) * rfViewFlag );

	AddPriorityPatternFilter(FilterEqual<BOOL>(pStencilEnable, 1), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<D3D11_STENCIL_OP>(pFrontPassOp, D3D11_STENCIL_OP_INCR), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<D3D11_STENCIL_OP>(pBackPassOp, D3D11_STENCIL_OP_DECR), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<D3D11_STENCIL_OP>(pFrontFailOp, D3D11_STENCIL_OP_DECR), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<D3D11_STENCIL_OP>(pBackFailOp, D3D11_STENCIL_OP_INCR), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 3.375f );
	DeclarePriorityLevelByPriorityValue( 2, 2.25f );

}
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT	CStencilOps::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	ID3D11Device *pDevice = g_TestApp.GetDevice();
	ID3D11RenderTargetView *rtView = g_TestApp.GetRTView();
	ID3D11DepthStencilView *dsView = g_TestApp.GetDSView();
	
	FLOAT clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	
	if ( !m_dsDesc.StencilEnable )
	{
		/************************************************************************/
		/*
		m_dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		m_dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_DECR;
		m_dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER;

		m_dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
		m_dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_INCR;
		m_dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_LESS;
		*/
		/************************************************************************/

		/*
		With stencil cleared to 4, and stencil ref of 4, both front and back should fail
		But with stencil off, should pass anyway.  Then check to ensure that the stencil
		value hasn't changed by turning stencil on.
		*/

		GetEffectiveContext()->OMSetDepthStencilState( m_pDSState, 4 );
		HRESULT hr;
		if( FAILED( hr = ClearDepthStencil( 1.0f, 4 ) ) )
		{
			WriteToLog( _T("Failed to clear depth stencil.") );
			return RESULT_FAIL;
		}
        GetEffectiveContext()->ClearRenderTargetView(rtView, clearColor);

		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to Execute Deferred Context.") );
			return RESULT_FAIL;
		}

		if ( RESULT_FAIL == DrawFront() )
			return RESULT_FAIL;		

		if ( RESULT_FAIL == DrawBack() )
			return RESULT_FAIL;

		if ( RESULT_FAIL == Verify() )
		{
			WriteToLog( _T("Stencil test still applied.") );
			tRes = RESULT_FAIL;
			goto END;
		}

		//Now if the stencil value has changed, this should fail stencil
		GetEffectiveContext()->OMSetDepthStencilState( m_pDSVerify, 4 );
		GetEffectiveContext()->ClearRenderTargetView( rtView, clearColor );

		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to Execute Deferred Context.") );
			return RESULT_FAIL;
		}

		if ( RESULT_FAIL == DrawFront() )
		{
			tRes = RESULT_FAIL;
			goto END;
		}
		GetEffectiveContext()->Draw( 4, 0 );

		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to Execute Deferred Context.") );
			return RESULT_FAIL;
		}

		if ( RESULT_FAIL == DrawBack() )
		{
			tRes = RESULT_FAIL;
			goto END;
		}
		GetEffectiveContext()->Draw( 4, 0 );

		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to Execute Deferred Context.") );
			return RESULT_FAIL;
		}

		if ( RESULT_FAIL == Verify() )
		{
			WriteToLog( _T("Stencil value changed when stencil disabled.") );
			tRes = RESULT_FAIL;
		}

	}
	else
	{
		bool bFront = true, bPass = true;
		D3D11_STENCIL_OP currentOp;

		UINT refVal, expectedVal;
		UINT8 clearVal;
		TCHAR *szFrontBack, *szPassFail;

		for ( UINT i = 0; i < 4; ++i ) //Cycle through front-back pass-fail
		{
			bool bFirstDone = false;
			szFrontBack = bFront?_T("frontface"):_T("backface");
			szPassFail = bPass?_T("passing"):_T("failing");

			if ( bFront )
			{
				if ( bPass )
				{
					currentOp = m_dsDesc.FrontFace.StencilPassOp;
				}
				else
				{
					currentOp = m_dsDesc.FrontFace.StencilFailOp;
				}
			}
			else
			{
				if ( bPass )
				{
					currentOp = m_dsDesc.BackFace.StencilPassOp;
				}
				else
				{
					currentOp = m_dsDesc.BackFace.StencilFailOp;
				}
			}

REPEAT:
			
			switch ( currentOp )
			{
			case D3D11_STENCIL_OP_KEEP:
				refVal = bFront?(bPass?5:3):(bPass?3:5);
				clearVal = 4;
				expectedVal = 4;
				
				break;

			case D3D11_STENCIL_OP_ZERO:
				refVal = bFront?(bPass?5:3):(bPass?3:5);
				clearVal = 4;
				expectedVal = 0;

				break;

			case D3D11_STENCIL_OP_REPLACE:
				refVal = bFront?(bPass?5:3):(bPass?3:5);
				clearVal = 4;
				expectedVal = bFront?(bPass?5:3):(bPass?3:5);

				break;

			case D3D11_STENCIL_OP_INCR_SAT:
				//First test.  Check for incr.  Special case below for clamp
				if ( !bFirstDone )
				{
					refVal = bFront?(bPass?5:3):(bPass?3:5);
					clearVal = 4;
					expectedVal = 5;
				}
				else
				{
					refVal = bPass?0xFF:0xFE;
					clearVal = 0xFF;
					expectedVal = 0xFF;
				}

				break;

			case D3D11_STENCIL_OP_DECR_SAT:
				//First test.  Check for decr.  Special case below for clamp
				if ( !bFirstDone )
				{
					refVal = bFront?(bPass?5:3):(bPass?3:5);
					clearVal = 4;
					expectedVal = 3;
				}
				else
				{
					refVal = bPass?0:1;
					clearVal = 0;
					expectedVal = 0;
				}

				break;

			case D3D11_STENCIL_OP_INVERT:
				refVal = bFront?(bPass?5:3):(bPass?3:5);
				clearVal = 4;
				expectedVal = 0xFF ^ 4;

				break;

			case D3D11_STENCIL_OP_INCR:
				//First test.  Check for incr.  Special case below for wrap
				if ( !bFirstDone )
				{
					refVal = bFront?(bPass?5:3):(bPass?3:5);
					clearVal = 4;
					expectedVal = 5;
				}
				else
				{
					refVal = bPass?0xFF:0xFE;
					clearVal = 0xFF;
					expectedVal = 0;
				}

				break;

			case D3D11_STENCIL_OP_DECR:
				//First test.  Check for decr.  Special case below for wrap
				if ( !bFirstDone )
				{
					refVal = bFront?(bPass?5:3):(bPass?3:5);
					clearVal = 4;
					expectedVal = 3;
				}
				else
				{
					refVal = bPass?0:1;
					clearVal = 0;
					expectedVal = 0xFF;
				}
				
				break;

			default:
				WriteToLog( _T("Unrecognized stencil op") );
				return RESULT_FAIL;
			}

			if ( g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.Flags & D3D11_DSV_READ_ONLY_STENCIL ) // read-only view
			{
				expectedVal = clearVal; // read-only - should still be what we cleared it to
			}

			GetEffectiveContext()->ClearRenderTargetView( rtView, clearColor );
			GetEffectiveContext()->OMSetDepthStencilState( bFirstDone?m_pDSVerify:m_pDSState, refVal );
			HRESULT hr;
			if( FAILED( hr = ClearDepthStencil( 1.0f, clearVal ) ) )
			{
				WriteToLog( _T("Failed to clear depth stencil.") );
				return RESULT_FAIL;
			}

			if( FAILED( hr = ExecuteEffectiveContext() ) )
			{
				WriteToLog( _T("Failed to Execute Deferred Context.") );
				return RESULT_FAIL;
			}

			if ( RESULT_FAIL == (bFront?DrawFront():DrawBack()) )
				return RESULT_FAIL;

			//Base verification
			if ( RESULT_FAIL == Verify(bFront?TOP:BOTTOM, bPass?false:true) )
			{
				WriteToLog( _T("Failed when verifying %s %s."), szPassFail, szFrontBack );
				tRes = RESULT_FAIL;
				goto END;
			}
	
			//Now if the stencil value is not as expected, this should fail stencil
			GetEffectiveContext()->OMSetDepthStencilState( m_pDSVerify, expectedVal );
			GetEffectiveContext()->ClearRenderTargetView( rtView, clearColor );

			if( FAILED( hr = ExecuteEffectiveContext() ) )
			{
				WriteToLog( _T("Failed to Execute Deferred Context.") );
				return RESULT_FAIL;
			}

			if ( RESULT_FAIL == (bFront?DrawFront():DrawBack()) )
			{
				tRes = RESULT_FAIL;
				goto END;
			}

			if ( RESULT_FAIL == Verify(bFront?TOP:BOTTOM) )
			{
				WriteToLog( _T("Unexpected stencil value.") );
				WriteToLog( _T("Failed when verifying %s %s."), szPassFail, szFrontBack );
				switch( currentOp ) 
				{
				case D3D11_STENCIL_OP_INCR_SAT:
				case D3D11_STENCIL_OP_DECR_SAT:
					if ( bFirstDone ) 
					{
						WriteToLog(_T("Clamp verification."));
					}

					break;
				case D3D11_STENCIL_OP_INCR:
				case D3D11_STENCIL_OP_DECR:
					if ( bFirstDone ) 
					{
						WriteToLog(_T("Wrap verification."));
					}

					break;
				}
				tRes = RESULT_FAIL;
				goto END;
			}

			switch( currentOp )
			{
			case D3D11_STENCIL_OP_INCR_SAT:
			case D3D11_STENCIL_OP_DECR_SAT:
			case D3D11_STENCIL_OP_INCR:
			case D3D11_STENCIL_OP_DECR:
				if ( !bFirstDone ) 
				{
					bFirstDone = true;
					goto REPEAT;
				}

				break;
			}

			g_TestApp.Present();

			if ( bPass )
			{
				bPass = false;
				continue;
			}else if ( bFront )
			{
				bFront = false;
				bPass = true;
				continue;
			}
		}
	}

	return tRes;

END:
	g_TestApp.Present();

	return tRes;
}

///////////////////////////////////////////////////////////////////////////////
void		CStencilOpsDepthRO::InitTestParameters()
{
	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T("Format"), &g_TestApp.m_RenderTargetOptions.DepthTextureDesc.Format );
	testfactor::RFactor rfFormat = AddParameterValueSet( pFormatParam, FormatValueSet( FF_S, 0 ) );

	CTestCaseParameter<BOOL> *pStencilEnable = AddParameter( _T("StencilEnable"), &m_dsDesc.StencilEnable );
	testfactor::RFactor rfStencilOff = AddParameterValue( pStencilEnable, 0 );
	testfactor::RFactor rfStencilOn = AddParameterValue( pStencilEnable, 1 );

	CTestCaseParameter<UINT> *pViewFlag = AddParameter( _T("DepthStencilViewDesc_Flags"), &g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.Flags );
	CUserValueSet<UINT> *pViewFlagValueSet = new CUserValueSet<UINT>();
	pViewFlagValueSet->AddValue( 0 );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ) // not available for 10_*
	{
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH );
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH );
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL );
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL );
	}
	testfactor::RFactor rfViewFlag = AddParameterValueSet( pViewFlag, pViewFlagValueSet );

	CTestCaseParameter<D3D11_DEPTH_WRITE_MASK> *pDepthWriteMask = AddParameter( _T( "DepthStencilDesc_DepthWriteMask" ), &m_dsDesc.DepthWriteMask );
	CUserValueSet<D3D11_DEPTH_WRITE_MASK> *pDepthWriteMaskValueSet = new CUserValueSet<D3D11_DEPTH_WRITE_MASK>();
	pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ZERO );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ALL );
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ZERO );
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ALL );
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ZERO );
	}
	testfactor::RFactor rfDepthWriteMask = AddParameterValueSet( pDepthWriteMask, pDepthWriteMaskValueSet );

	CTestCaseParameter<D3D11_STENCIL_OP> *pFrontPassOp, *pBackPassOp;

	// We don't need every combination....just a small sparse set would do
	pFrontPassOp = AddParameter( _T("FrontFace_StencilPassOp"), &m_dsDesc.FrontFace.StencilPassOp );
	pFrontPassOp->SetDefaultValue( D3D11_STENCIL_OP_INCR );
	CUserValueSet<D3D11_STENCIL_OP> *pFrontPassOpValueSet = new CUserValueSet<D3D11_STENCIL_OP>();
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_KEEP );
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_ZERO );
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_REPLACE );
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_INCR_SAT );
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_DECR_SAT );
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_INVERT );
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_INCR );
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_DECR );

	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_KEEP );
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_ZERO );
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_REPLACE );
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_INCR_SAT );
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_DECR_SAT );
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_INVERT );
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_INCR );
	pFrontPassOpValueSet->AddValue( D3D11_STENCIL_OP_DECR );

	testfactor::RFactor rfFrontStencilPass = AddParameterValueSet( pFrontPassOp, pFrontPassOpValueSet );

	pBackPassOp = AddParameter( _T("BackFace_StencilPassOp"), &m_dsDesc.BackFace.StencilPassOp );
	pBackPassOp->SetDefaultValue( D3D11_STENCIL_OP_DECR );
	CUserValueSet<D3D11_STENCIL_OP> *pBackPassOpValueSet = new CUserValueSet<D3D11_STENCIL_OP>();
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_KEEP );
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_ZERO );
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_REPLACE );
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_INCR_SAT );
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_DECR_SAT );
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_INVERT );
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_INCR );
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_DECR );

	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_INCR );
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_DECR );
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_KEEP );
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_ZERO );
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_REPLACE );
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_INCR_SAT );
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_DECR_SAT );
	pBackPassOpValueSet->AddValue( D3D11_STENCIL_OP_INVERT );
	testfactor::RFactor rfBackStencilPass = AddParameterValueSet( pBackPassOp, pBackPassOpValueSet );

	CTestCaseParameter<D3D11_STENCIL_OP> *pFrontFailOp, *pBackFailOp;
	
	pFrontFailOp = AddParameter( _T("FrontFace_StencilFailOp"), &m_dsDesc.FrontFace.StencilFailOp );
	pFrontFailOp->SetDefaultValue( D3D11_STENCIL_OP_DECR );
	CUserValueSet<D3D11_STENCIL_OP> *pFrontFailOpValueSet = new CUserValueSet<D3D11_STENCIL_OP>();
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_KEEP );
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_ZERO );
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_REPLACE );
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_INCR_SAT );
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_DECR_SAT );
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_INVERT );
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_INCR );
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_DECR );

	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_DECR_SAT );
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_INVERT );
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_INCR );
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_DECR );
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_KEEP );
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_ZERO );
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_REPLACE );
	pFrontFailOpValueSet->AddValue( D3D11_STENCIL_OP_INCR_SAT );
	testfactor::RFactor rfFrontStencilFail = AddParameterValueSet( pFrontFailOp, pFrontFailOpValueSet );
	
	pBackFailOp = AddParameter( _T("BackFace_StencilFailOp"), &m_dsDesc.BackFace.StencilFailOp );
	pBackFailOp->SetDefaultValue( D3D11_STENCIL_OP_INCR );
	CUserValueSet<D3D11_STENCIL_OP> *pBackFailOpValueSet = new CUserValueSet<D3D11_STENCIL_OP>();
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_KEEP );
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_ZERO );
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_REPLACE );
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_INCR_SAT );
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_DECR_SAT );
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_INVERT );
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_INCR );
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_DECR );

	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_REPLACE );
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_INCR_SAT );
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_DECR_SAT );
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_INVERT );
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_INCR );
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_DECR );
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_KEEP );
	pBackFailOpValueSet->AddValue( D3D11_STENCIL_OP_ZERO );
	testfactor::RFactor rfBackStencilFail = AddParameterValueSet( pBackFailOp, pBackFailOpValueSet );

	
	SetRootTestFactor( ( rfViewFlag % rfDepthWriteMask ) * rfFormat * ( rfStencilOff + rfStencilOn * 
		( rfFrontStencilPass % rfBackStencilPass % rfFrontStencilFail % rfBackStencilFail ) ) );
}
///////////////////////////////////////////////////////////////////////////////
//TODO: Test order of operations.  Stencil pass vs. stencildepthfail
CStencilDepthOp::CStencilDepthOp()
{
	m_dsDesc.StencilEnable = TRUE;
	m_dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	m_dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	m_dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;
	m_dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_LESS_EQUAL;
	m_dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	m_dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	m_dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	m_dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;	

	m_dsDesc.StencilReadMask = 0xFF;
	m_dsDesc.StencilWriteMask = 0xFF;

	g_TestApp.m_RenderTargetOptions.UseDefaultDepthStencilView = false;
	g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
}
///////////////////////////////////////////////////////////////////////////////
void		CStencilDepthOp::InitTestParameters()
{
	CUserValueSet<D3D11_STENCIL_OP> *pStencilOps = new CUserValueSet<D3D11_STENCIL_OP>();
	pStencilOps->SetFromNamedValues();
	
	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T("Format"), &g_TestApp.m_RenderTargetOptions.DepthTextureDesc.Format );
	testfactor::RFactor rfFormat = AddParameterValueSet( pFormatParam, FormatValueSet( FF_S, 0 ) );

	CTestCaseParameter<UINT> *pViewFlag = AddParameter( _T("DepthStencilViewDesc_Flags"), &g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.Flags );
	pViewFlag->SetDefaultValue( 0 );
	CUserValueSet<UINT> *pViewFlagValueSet = new CUserValueSet<UINT>();
	pViewFlagValueSet->AddValue( 0 );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ) // not available for 10_*
	{
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_STENCIL );
	}
	testfactor::RFactor rfViewFlag = AddParameterValueSet( pViewFlag, pViewFlagValueSet );

	CTestCaseParameter<BOOL> *pDepth = AddParameter<BOOL>( _T("DepthEnable"), &m_dsDesc.DepthEnable );
	testfactor::RFactor rfDepthOff = AddParameterValue( pDepth, 0 );
	testfactor::RFactor rfDepthOn = AddParameterValue( pDepth, 1 );

	testfactor::RFactor rfFrontDepth = AddParameter( _T("FrontFace_StencilDepthFailOp"), &m_dsDesc.FrontFace.StencilDepthFailOp, pStencilOps );
	testfactor::RFactor rfBackDepth = AddParameter( _T("BackFace_StencilDepthFailOp"), &m_dsDesc.BackFace.StencilDepthFailOp, pStencilOps );

	testfactor::RFactor rfFrontDepthOff = AddParameterValue( _T("FrontFace_StencilDepthFailOp"), D3D11_STENCIL_OP_INCR );
	testfactor::RFactor rfBackDepthOff = AddParameterValue( _T("BackFace_StencilDepthFailOp"), D3D11_STENCIL_OP_DECR );

	SetRootTestFactor( rfFormat * (rfDepthOff * rfFrontDepthOff * rfBackDepthOff + rfDepthOn * rfFrontDepth * rfBackDepth ) * rfViewFlag );
}
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT	CStencilDepthOp::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	ID3D11Device *pDevice = g_TestApp.GetDevice();
	ID3D11RenderTargetView *rtView = g_TestApp.GetRTView();
	ID3D11DepthStencilView *dsView = g_TestApp.GetDSView();

	FLOAT clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	if ( !m_dsDesc.DepthEnable )
	{
		/************************************************************************/
		/*
		m_dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		m_dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		*/
		/************************************************************************/

		/*
		With stencil cleared to 4, and stencil ref of 4, both front and back should pass
		But with depth off, any depth val should pass depth.  Ensure that front and back face
		value hasn't changed.
		*/

		GetEffectiveContext()->OMSetDepthStencilState( m_pDSState, 4 );

		HRESULT hr;
		if( FAILED( hr = ClearDepthStencil( 0.5f, 4 ) ) )
		{
			WriteToLog( _T("Failed to clear depth stencil.") );
			return RESULT_FAIL;
		}

		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to Execute Deferred Context.") );
			return RESULT_FAIL;
		}

		if ( RESULT_FAIL == DrawFront( 0.6f ) )
			return RESULT_FAIL;		

		if ( RESULT_FAIL == DrawBack( 0.6f ) )
			return RESULT_FAIL;

		if ( RESULT_FAIL == Verify() )
		{
			WriteToLog( _T("Depth test still applied.") );
			tRes = RESULT_FAIL;
			goto END;
		}

		//Now if the stencil value has changed, this should fail stencil
		GetEffectiveContext()->ClearRenderTargetView( rtView, clearColor );

		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to Execute Deferred Context.") );
			return RESULT_FAIL;
		}

		if ( RESULT_FAIL == DrawFront() )
		{
			tRes = RESULT_FAIL;
			goto END;
		}
		GetEffectiveContext()->Draw( 4, 0 );

		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to Execute Deferred Context.") );
			return RESULT_FAIL;
		}

		if ( RESULT_FAIL == DrawBack() )
		{
			tRes = RESULT_FAIL;
			goto END;
		}
		GetEffectiveContext()->Draw( 4, 0 );

		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to Execute Deferred Context.") );
			return RESULT_FAIL;
		}

		if ( RESULT_FAIL == Verify() )
		{
			WriteToLog( _T("Stencil value changed when depth test disabled.") );
			tRes = RESULT_FAIL;
		}

		g_TestApp.Present();
	}
	else
	{
		bool bFront = true;
		D3D11_STENCIL_OP currentOp;

		UINT refVal, expectedVal;
		UINT8 clearVal;
		
		TCHAR *szFrontBack;

		for ( UINT i = 0; i < 2; ++i ) //Cycle through front-back
		{
			bool bFirstDone = false;
			szFrontBack = bFront?_T("frontface"):_T("backface");

			if ( bFront )
			{
				currentOp = m_dsDesc.FrontFace.StencilDepthFailOp;
			}
			else
			{
				currentOp = m_dsDesc.BackFace.StencilDepthFailOp;
			}

REPEAT:
			switch ( currentOp )
			{
			case D3D11_STENCIL_OP_KEEP:
				clearVal = refVal = expectedVal = 4;

				break;

			case D3D11_STENCIL_OP_ZERO:
				clearVal = refVal = 4;
				expectedVal = 0;

				break;

			case D3D11_STENCIL_OP_REPLACE:
				refVal = bFront?5:3;
				clearVal = 4;
				expectedVal = bFront?5:3;

				break;

			case D3D11_STENCIL_OP_INCR_SAT:
				//First test.  Check for incr.  Special case below for clamp
				if ( !bFirstDone )
				{
					refVal = clearVal = 4;
					expectedVal = 5;
				}
				else
				{
					refVal = clearVal = 0xFF;
					expectedVal = 0xFF;
				}

				break;

			case D3D11_STENCIL_OP_DECR_SAT:
				//First test.  Check for decr.  Special case below for clamp
				if ( !bFirstDone )
				{
					refVal = clearVal = 4;
					expectedVal = 3;
				}
				else
				{
					refVal = clearVal = 0;
					expectedVal = 0;
				}

				break;

			case D3D11_STENCIL_OP_INVERT:
				refVal = clearVal = 4;
				expectedVal = 0xFF ^ 4;

				break;

			case D3D11_STENCIL_OP_INCR:
				//First test.  Check for incr.  Special case below for wrap
				if ( !bFirstDone )
				{
					refVal = clearVal = 4;
					expectedVal = 5;
				}
				else
				{
					refVal = clearVal = 0xFF;
					expectedVal = 0;
				}

				break;

			case D3D11_STENCIL_OP_DECR:
				//First test.  Check for decr.  Special case below for wrap
				if ( !bFirstDone )
				{
					refVal = clearVal = 4;
					expectedVal = 3;
				}
				else
				{
					refVal = clearVal = 0;
					expectedVal = 0xFF;
				}

				break;

			default:
				WriteToLog( _T("Unrecognized stencil op") );
				return RESULT_FAIL;
			}

			if ( g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.Flags & D3D11_DSV_READ_ONLY_STENCIL ) // read-only view
			{
				expectedVal = clearVal; // read-only - should still be what we cleared it to
			}

			GetEffectiveContext()->ClearRenderTargetView( rtView, clearColor );
			GetEffectiveContext()->OMSetDepthStencilState( bFirstDone?m_pDSVerify:m_pDSState, refVal );
			HRESULT hr;
			if( FAILED( hr = ClearDepthStencil( 0.5f, clearVal ) ) )
			{
				WriteToLog( _T("Failed to clear depth stencil.") );
				return RESULT_FAIL;
			}

			if( FAILED( hr = ExecuteEffectiveContext() ) )
			{
				WriteToLog( _T("Failed to Execute Deferred Context.") );
				return RESULT_FAIL;
			}

			if ( RESULT_FAIL == (bFront?DrawFront( 0.6f ):DrawBack( 0.6f )) )
				return RESULT_FAIL;

			//Base verification
			if ( RESULT_FAIL == Verify( bFront?TOP:BOTTOM, true ) ) //Always expect black, since we're failing depth
			{
				WriteToLog( _T("Failed when verifying %s."), szFrontBack );
				tRes = RESULT_FAIL;
				goto END;
			}

			//Now if the stencil value is not as expected, this should fail stencil
			GetEffectiveContext()->OMSetDepthStencilState( m_pDSVerify, expectedVal );
			GetEffectiveContext()->ClearRenderTargetView( rtView, clearColor );

			if( FAILED( hr = ExecuteEffectiveContext() ) )
			{
				WriteToLog( _T("Failed to Execute Deferred Context.") );
				return RESULT_FAIL;
			}

			if ( RESULT_FAIL == (bFront?DrawFront():DrawBack()) )
			{
				tRes = RESULT_FAIL;
				goto END;
			}

			if ( RESULT_FAIL == Verify(bFront?TOP:BOTTOM) )
			{
				WriteToLog( _T("Unexpected stencil value.") );
				WriteToLog( _T("Failed when verifying %s."), szFrontBack );
				switch( currentOp ) 
				{
				case D3D11_STENCIL_OP_INCR_SAT:
				case D3D11_STENCIL_OP_DECR_SAT:
					if ( bFirstDone ) 
					{
						WriteToLog(_T("Clamp verification."));
					}

					break;
				case D3D11_STENCIL_OP_INCR:
				case D3D11_STENCIL_OP_DECR:
					if ( bFirstDone ) 
					{
						WriteToLog(_T("Wrap verification."));
					}

					break;
				}
				tRes = RESULT_FAIL;
				goto END;
			}

			switch( currentOp )
			{
			case D3D11_STENCIL_OP_INCR_SAT:
			case D3D11_STENCIL_OP_DECR_SAT:
			case D3D11_STENCIL_OP_INCR:
			case D3D11_STENCIL_OP_DECR:
				if ( !bFirstDone ) 
				{
					bFirstDone = true;
					goto REPEAT;
				}

				break;
			}

			g_TestApp.Present();

			if ( bFront )
			{
				bFront = false;
				continue;
			}
		}
	}

	return tRes;

END:
	g_TestApp.Present();

	return tRes;
}

///////////////////////////////////////////////////////////////////////////////
void		CStencilDepthOpDepthRO::InitTestParameters()
{
	CUserValueSet<D3D11_STENCIL_OP> *pStencilOps = new CUserValueSet<D3D11_STENCIL_OP>();
	pStencilOps->SetFromNamedValues();
	
	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T("Format"), &g_TestApp.m_RenderTargetOptions.DepthTextureDesc.Format );
	testfactor::RFactor rfFormat = AddParameterValueSet( pFormatParam, FormatValueSet( FF_S, 0 ) );

	CTestCaseParameter<BOOL> *pDepth = AddParameter<BOOL>( _T("DepthEnable"), &m_dsDesc.DepthEnable );
	testfactor::RFactor rfDepthOff = AddParameterValue( pDepth, 0 );
	testfactor::RFactor rfDepthOn = AddParameterValue( pDepth, 1 );

	CTestCaseParameter<UINT> *pViewFlag = AddParameter( _T("DepthStencilViewDesc_Flags"), &g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.Flags );
	CUserValueSet<UINT> *pViewFlagValueSet = new CUserValueSet<UINT>();
	pViewFlagValueSet->AddValue( 0 );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ) // not available for 10_*
	{
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH );
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH );
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL );
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL );
	}
	testfactor::RFactor rfViewFlag = AddParameterValueSet( pViewFlag, pViewFlagValueSet );

	CTestCaseParameter<D3D11_DEPTH_WRITE_MASK> *pDepthWriteMask = AddParameter( _T( "DepthStencilDesc_DepthWriteMask" ), &m_dsDesc.DepthWriteMask );
	CUserValueSet<D3D11_DEPTH_WRITE_MASK> *pDepthWriteMaskValueSet = new CUserValueSet<D3D11_DEPTH_WRITE_MASK>();
	pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ZERO );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ALL );
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ZERO );
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ALL );
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ZERO );
	}
	testfactor::RFactor rfDepthWriteMask = AddParameterValueSet( pDepthWriteMask, pDepthWriteMaskValueSet );

	testfactor::RFactor rfFrontDepth = AddParameter( _T("FrontFace_StencilDepthFailOp"), &m_dsDesc.FrontFace.StencilDepthFailOp, pStencilOps );
	testfactor::RFactor rfBackDepth = AddParameter( _T("BackFace_StencilDepthFailOp"), &m_dsDesc.BackFace.StencilDepthFailOp, pStencilOps );

	testfactor::RFactor rfFrontDepthOff = AddParameterValue( _T("FrontFace_StencilDepthFailOp"), D3D11_STENCIL_OP_INCR );
	testfactor::RFactor rfBackDepthOff = AddParameterValue( _T("BackFace_StencilDepthFailOp"), D3D11_STENCIL_OP_DECR );

	SetRootTestFactor( ( rfViewFlag % rfDepthWriteMask ) * rfFormat * (rfDepthOff * rfFrontDepthOff * rfBackDepthOff + rfDepthOn * rfFrontDepth * rfBackDepth) );
}
///////////////////////////////////////////////////////////////////////////////
CStencilFuncs::CStencilFuncs()
{
	m_dsDesc.StencilEnable = TRUE;
	m_dsDesc.DepthEnable = FALSE;
	m_dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	m_dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	m_dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	m_dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	m_dsDesc.StencilReadMask = 0xFF;
	m_dsDesc.StencilWriteMask = 0xFF;

	m_dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	m_dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
	m_dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	m_dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;

	g_TestApp.m_RenderTargetOptions.UseDefaultDepthStencilView = false;
	g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
}
///////////////////////////////////////////////////////////////////////////////
void		CStencilFuncs::InitTestParameters()
{
	CUserValueSet<D3D11_COMPARISON_FUNC> *pStencilFuncs = new CUserValueSet<D3D11_COMPARISON_FUNC>();
	pStencilFuncs->SetFromNamedValues();

	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T("Format"), &g_TestApp.m_RenderTargetOptions.DepthTextureDesc.Format );
	testfactor::RFactor rfFormat = AddParameterValueSet( pFormatParam, FormatValueSet( FF_S, 0 ) );

	CTestCaseParameter<UINT> *pViewFlag = AddParameter( _T("DepthStencilViewDesc_Flags"), &g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.Flags );
	pViewFlag->SetDefaultValue( 0 );
	CUserValueSet<UINT> *pViewFlagValueSet = new CUserValueSet<UINT>();
	pViewFlagValueSet->AddValue( 0 );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ) // not available for 10_*
	{
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_STENCIL );
	}
	testfactor::RFactor rfViewFlag = AddParameterValueSet( pViewFlag, pViewFlagValueSet );

	testfactor::RFactor rfFrontStencilFunc = AddParameter( _T("FrontFace_StencilFunc"), &m_dsDesc.FrontFace.StencilFunc, pStencilFuncs );
	testfactor::RFactor rfBackStencilFunc = AddParameter( _T("BackFace_StencilFunc"), &m_dsDesc.BackFace.StencilFunc, pStencilFuncs );

	SetRootTestFactor( rfFormat * rfFrontStencilFunc * rfBackStencilFunc * rfViewFlag );
}
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT	CStencilFuncs::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	ID3D11Device *pDevice = g_TestApp.GetDevice();
	ID3D11RenderTargetView *rtView = g_TestApp.GetRTView();
	ID3D11DepthStencilView *dsView = g_TestApp.GetDSView();

	UINT8 clearVal = 4;
	bool bFront = true;
	FLOAT clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	for ( UINT i = 0; i < 2; ++i )
	{
		D3D11_COMPARISON_FUNC currentFunc;		
		TCHAR * szFrontBack;

		szFrontBack = bFront?_T("frontface"):_T("backface");

		if ( bFront )
		{
			currentFunc = m_dsDesc.FrontFace.StencilFunc;
		}
		else
		{
			currentFunc = m_dsDesc.BackFace.StencilFunc;
		}
	
		for ( int relativePos = -1; relativePos < 2; ++relativePos )
		{
			bool bPass;

			switch( currentFunc )
			{
			case D3D11_COMPARISON_NEVER:
				switch ( relativePos )
				{
				case -1:
				case 0:
				case 1:
					bPass = false;

					break;
				}

				break;
			case D3D11_COMPARISON_LESS:
				switch ( relativePos )
				{
				case -1:
					bPass = true;

					break;

				case 0:
				case 1:
					bPass = false;

					break;
				}

				break;
			case D3D11_COMPARISON_EQUAL:
				switch ( relativePos )
				{
				case 0:
					bPass = true;

					break;

				case -1:
				case 1:
					bPass = false;

					break;
				}

				break;
			case D3D11_COMPARISON_LESS_EQUAL:
				switch ( relativePos )
				{
				case -1:
				case 0:
					bPass = true;

					break;

				case 1:
					bPass = false;

					break;
				}

				break;
			case D3D11_COMPARISON_GREATER:
				switch ( relativePos )
				{
				case -1:
				case 0:
					bPass = false;

					break;

				case 1:
					bPass = true;
					
					break;
				}

				break;
			case D3D11_COMPARISON_NOT_EQUAL:
				switch ( relativePos )
				{
				case -1:
				case 1:
					bPass = true;

					break;

				case 0:
					bPass = false;

					break;
				}

				break;
			case D3D11_COMPARISON_GREATER_EQUAL:
				switch ( relativePos )
				{
				case -1:
					bPass = false;

					break;

				case 0:
				case 1:
					bPass = true;

					break;
				}

				break;
			case D3D11_COMPARISON_ALWAYS:
				switch ( relativePos )
				{
				case -1:
				case 0:
				case 1:
					bPass = true;

					break;
				}

				break;
			default:
				WriteToLog( _T("Unrecognized stencil func.") );
				return RESULT_FAIL;
			}

			UINT8 expectedVal = 0; // all stencil ops set to OP_ZERO in constructor
			if ( g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.Flags & D3D11_DSV_READ_ONLY_STENCIL ) // read-only view
			{
				expectedVal = clearVal; // read-only - should still be what we cleared it to
			}
	
			GetEffectiveContext()->OMSetDepthStencilState( m_pDSState, clearVal + relativePos );
			GetEffectiveContext()->ClearRenderTargetView( rtView, clearColor );
			HRESULT hr;
			if( FAILED( hr = ClearDepthStencil( 1.0f, clearVal ) ) )
			{
				WriteToLog( _T("Failed to clear depth stencil.") );
				return RESULT_FAIL;
			}

			if( FAILED( hr = ExecuteEffectiveContext() ) )
			{
				WriteToLog( _T("Failed to Execute Deferred Context.") );
				return RESULT_FAIL;
			}

			if ( RESULT_FAIL == (bFront?DrawFront():DrawBack()) )
				return RESULT_FAIL;

			//Base verification
			if ( RESULT_FAIL == Verify( bFront?TOP:BOTTOM, !bPass ) )
			{
				WriteToLog( _T("Failed when verifying %s."), szFrontBack );
				WriteToLog( _T("Stencil ref value: %d"), clearVal + relativePos );
				WriteToLog( _T("Stencil was cleared to: %d"), clearVal );

				tRes = RESULT_FAIL;
			}
	
			//Now if the stencil value is not as expected, this should fail stencil
			GetEffectiveContext()->OMSetDepthStencilState( m_pDSVerify, expectedVal );
			GetEffectiveContext()->ClearRenderTargetView( rtView, clearColor );

			if( FAILED( hr = ExecuteEffectiveContext() ) )
			{
				WriteToLog( _T("Failed to Execute Deferred Context.") );
				return RESULT_FAIL;
			}

			if ( RESULT_FAIL == (bFront?DrawFront():DrawBack()) )
			{
				return RESULT_FAIL;
			}

			if ( RESULT_FAIL == Verify(bFront?TOP:BOTTOM) )
			{
				WriteToLog( _T("Failed when verifying %s."), szFrontBack );
				WriteToLog( _T("Stencil expected value: %d"), expectedVal );
				tRes = RESULT_FAIL;
			}

			g_TestApp.Present();
		}
	
		if ( bFront ) bFront = false;
	}

	return tRes;
}

///////////////////////////////////////////////////////////////////////////////
void		CStencilFuncsDepthRO::InitTestParameters()
{
	CUserValueSet<D3D11_COMPARISON_FUNC> *pStencilFuncs = new CUserValueSet<D3D11_COMPARISON_FUNC>();
	pStencilFuncs->SetFromNamedValues();

	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T("Format"), &g_TestApp.m_RenderTargetOptions.DepthTextureDesc.Format );
	testfactor::RFactor rfFormat = AddParameterValueSet( pFormatParam, FormatValueSet( FF_S, 0 ) );

	CTestCaseParameter<UINT> *pViewFlag = AddParameter( _T("DepthStencilViewDesc_Flags"), &g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.Flags );
	CUserValueSet<UINT> *pViewFlagValueSet = new CUserValueSet<UINT>();
	pViewFlagValueSet->AddValue( 0 );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ) // not available for 10_*
	{
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH );
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH );
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL );
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL );
	}
	testfactor::RFactor rfViewFlag = AddParameterValueSet( pViewFlag, pViewFlagValueSet );

	CTestCaseParameter<D3D11_DEPTH_WRITE_MASK> *pDepthWriteMask = AddParameter( _T( "DepthStencilDesc_DepthWriteMask" ), &m_dsDesc.DepthWriteMask );
	CUserValueSet<D3D11_DEPTH_WRITE_MASK> *pDepthWriteMaskValueSet = new CUserValueSet<D3D11_DEPTH_WRITE_MASK>();
	pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ZERO );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ALL );
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ZERO );
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ALL );
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ZERO );
	}
	testfactor::RFactor rfDepthWriteMask = AddParameterValueSet( pDepthWriteMask, pDepthWriteMaskValueSet );

	testfactor::RFactor rfFrontStencilFunc = AddParameter( _T("FrontFace_StencilFunc"), &m_dsDesc.FrontFace.StencilFunc, pStencilFuncs );
	testfactor::RFactor rfBackStencilFunc = AddParameter( _T("BackFace_StencilFunc"), &m_dsDesc.BackFace.StencilFunc, pStencilFuncs );

	SetRootTestFactor( rfFormat * rfFrontStencilFunc * rfBackStencilFunc * ( rfViewFlag % rfDepthWriteMask ) );
}

///////////////////////////////////////////////////////////////////////////////
CStencilMask::CStencilMask()
{
	m_dsDesc.StencilEnable = TRUE;
	m_dsDesc.DepthEnable = FALSE;
	m_dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	m_dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	m_dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	m_dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	m_dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	m_dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	m_dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	m_dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	m_dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INVERT;
	m_dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INVERT;

	g_TestApp.m_RenderTargetOptions.UseDefaultDepthStencilView = false;
	g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
}
///////////////////////////////////////////////////////////////////////////////
void		CStencilMask::InitTestParameters()
{
	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T("Format"), &g_TestApp.m_RenderTargetOptions.DepthTextureDesc.Format );
	testfactor::RFactor rfFormat = AddParameterValueSet( pFormatParam, FormatValueSet( FF_S, 0 ) );

	CTestCaseParameter<UINT> *pViewFlag = AddParameter( _T("DepthStencilViewDesc_Flags"), &g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.Flags );
	pViewFlag->SetDefaultValue( 0 );
	CUserValueSet<UINT> *pViewFlagValueSet = new CUserValueSet<UINT>();
	pViewFlagValueSet->AddValue( 0 );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ) // not available for 10_*
	{
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_STENCIL );
	}
	testfactor::RFactor rfViewFlag = AddParameterValueSet( pViewFlag, pViewFlagValueSet );

	CUserValueSet<StencilBitMask> *pMasks = new CUserValueSet<StencilBitMask>();
	pMasks->AddValue(StencilBitMask(0x00));
	pMasks->AddValue(StencilBitMask(0xF0));
	pMasks->AddValue(StencilBitMask(0x0F));

	testfactor::RFactor rfWriteMasks = AddParameter( "StencilWriteMask", &WriteMask, pMasks );
	testfactor::RFactor rfReadMasks = AddParameter( "StencilReadMask", &ReadMask, pMasks );

	SetRootTestFactor( rfFormat * rfWriteMasks * rfReadMasks * rfViewFlag );
}

///////////////////////////////////////////////////////////////////////////////
TEST_RESULT	CStencilMask::SetupTestCase()
{
	m_dsDesc.StencilReadMask = ReadMask;
	m_dsDesc.StencilWriteMask = WriteMask;

	return CStencilTest::SetupTestCase();
}
///////////////////////////////////////////////////////////////////////////////
TEST_RESULT	CStencilMask::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	ID3D11Device *pDevice = g_TestApp.GetDevice();
	ID3D11RenderTargetView *rtView = g_TestApp.GetRTView();
	ID3D11DepthStencilView *dsView = g_TestApp.GetDSView();

	UINT8 clearVal = 0x44, refVal, expectedVal;
	FLOAT clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	refVal = expectedVal = clearVal;
	refVal |= 0xFF & (~m_dsDesc.StencilReadMask);
	expectedVal ^= 0xFF;
	expectedVal = (expectedVal & m_dsDesc.StencilWriteMask) | (clearVal & (~m_dsDesc.StencilWriteMask));

	if ( g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.Flags & D3D11_DSV_READ_ONLY_STENCIL ) // read-only view
	{
		expectedVal = clearVal; // read-only - should still be what we cleared it to
	}

	GetEffectiveContext()->OMSetDepthStencilState( m_pDSState, refVal );
	GetEffectiveContext()->ClearRenderTargetView( rtView, clearColor );
	HRESULT hr;
	if( FAILED( hr = ClearDepthStencil( 1.0f, clearVal ) ) )
	{
		WriteToLog( _T("Failed to clear depth stencil.") );
		return RESULT_FAIL;
	}

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to Execute Deferred Context.") );
		return RESULT_FAIL;
	}

	if ( RESULT_FAIL == DrawFront() )
		return RESULT_FAIL;

	if ( RESULT_FAIL == DrawBack() )
		return RESULT_FAIL;

	if ( RESULT_FAIL == Verify( BOTH ) )
	{
		WriteToLog( _T("Unexpectedly failed stencil.") );
		tRes = RESULT_FAIL;
	}

	g_TestApp.Present();

	GetEffectiveContext()->OMSetDepthStencilState( m_pDSVerify, expectedVal );
	GetEffectiveContext()->ClearRenderTargetView( rtView, clearColor );

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to Execute Deferred Context.") );
		return RESULT_FAIL;
	}

	if ( RESULT_FAIL == DrawFront() )
		return RESULT_FAIL;

	if ( RESULT_FAIL == DrawBack() )
		return RESULT_FAIL;

	if ( RESULT_FAIL == Verify( BOTH ) )
	{
		WriteToLog( _T("Incorrect final stencil value.") );
		WriteToLog( _T("Cleared to: %d Expected: %d"), clearVal, expectedVal );
		tRes = RESULT_FAIL;
	}


	return tRes;
}

///////////////////////////////////////////////////////////////////////////////
void		CStencilMaskDepthRO::InitTestParameters()
{
	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T("Format"), &g_TestApp.m_RenderTargetOptions.DepthTextureDesc.Format );
	testfactor::RFactor rfFormat = AddParameterValueSet( pFormatParam, FormatValueSet( FF_S, 0 ) );

	CTestCaseParameter<UINT> *pViewFlag = AddParameter( _T("DepthStencilViewDesc_Flags"), &g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.Flags );
	CUserValueSet<UINT> *pViewFlagValueSet = new CUserValueSet<UINT>();
	pViewFlagValueSet->AddValue( 0 );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ) // not available for 10_*
	{
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH );
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH );
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL );
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL );
	}
	testfactor::RFactor rfViewFlag = AddParameterValueSet( pViewFlag, pViewFlagValueSet );

	CTestCaseParameter<D3D11_DEPTH_WRITE_MASK> *pDepthWriteMask = AddParameter( _T( "DepthStencilDesc_DepthWriteMask" ), &m_dsDesc.DepthWriteMask );
	CUserValueSet<D3D11_DEPTH_WRITE_MASK> *pDepthWriteMaskValueSet = new CUserValueSet<D3D11_DEPTH_WRITE_MASK>();
	pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ZERO );
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ALL );
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ZERO );
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ALL );
		pDepthWriteMaskValueSet->AddValue( D3D11_DEPTH_WRITE_MASK_ZERO );
	}
	testfactor::RFactor rfDepthWriteMask = AddParameterValueSet( pDepthWriteMask, pDepthWriteMaskValueSet );

	CUserValueSet<StencilBitMask> *pMasks = new CUserValueSet<StencilBitMask>();
	pMasks->AddValue(StencilBitMask(0x00));
	pMasks->AddValue(StencilBitMask(0xF0));
	pMasks->AddValue(StencilBitMask(0x0F));

	testfactor::RFactor rfWriteMasks = AddParameter( "StencilWriteMask", &WriteMask, pMasks );
	testfactor::RFactor rfReadMasks = AddParameter( "StencilReadMask", &ReadMask, pMasks );

	SetRootTestFactor( ( rfViewFlag % rfDepthWriteMask ) * rfFormat * rfWriteMasks * rfReadMasks );
}
