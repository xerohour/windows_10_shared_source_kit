#include <cmath>
#include "Clipping.h"

CClippingConfApp  g_App;

///////////////////////////////////////////////////////////////////////////////
//	Clipping Conformance testing
///////////////////////////////////////////////////////////////////////////////

#define ZeroStructMem(x) ZeroMemory(&x, sizeof(x))

TEST_RESULT
CClippingConfTest::SetupDirect3D( )
{
	TEST_RESULT tRes;
	if( (tRes = SetupRenderTarget( )) != RESULT_PASS )
		return tRes;

	if( (tRes = SetupShaders( )) != RESULT_PASS )
		return tRes;

	if( (tRes = SetupGeometry( )) != RESULT_PASS )
		return tRes;

	// create and set viewport
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float) m_nPrimitiveW;
	vp.Height   = (float) m_nPrimitiveH;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	GetEffectiveContext()->RSSetViewports(1, &vp);

	// create and set rasterizer state
	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.CullMode = D3D11_CULL_FRONT;
	rastDesc.FrontCounterClockwise = true;
	rastDesc.DepthBias = false;
	rastDesc.DepthBiasClamp = 0;
	rastDesc.SlopeScaledDepthBias = 0;
	rastDesc.DepthClipEnable = true;
	rastDesc.ScissorEnable = false;
	rastDesc.MultisampleEnable = false;
	rastDesc.AntialiasedLineEnable = false;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSEnabled );

	GetEffectiveContext()->RSSetState( m_pRSEnabled );

	/*
	// create and set sampler
	m_SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	m_SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.MipMapLODBias = 0;
	m_SamplerDesc.MaxAnisotropy = 1;
	m_SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	m_SamplerDesc.BorderColorR = 0.5f;
	m_SamplerDesc.BorderColorG = 0.5f;
	m_SamplerDesc.BorderColorB = 0.5f;
	m_SamplerDesc.BorderColorA = 1.0f;
	m_SamplerDesc.MinLOD = 0;
	m_SamplerDesc.MaxLOD = 5;

	HRESULT hr;
	if( FAILED( hr = GetDevice()->CreateSampler(&m_SamplerDesc, &m_pSampler ) ))
	{
	WriteToLog( _T("CClippingConfTest::SetupD3D() - CreateSampler() failed. hr = 0x%x"), hr );
	return RESULT_FAIL;
	}

	GetDevice()->VSSetSamplers(0, 1, &m_pSampler);
	GetDevice()->GSSetSamplers(0, 1, &m_pSampler);
	GetDevice()->PSSetSamplers(0, 1, &m_pSampler);
	*/

	return RESULT_PASS;
}

//---------------------------------------------------------------

TEST_RESULT
CClippingConfTest::SetupGeometry( )
{
	HRESULT hr;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA resUP;

	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);

	// create vertex buffer
	// Z stores vertex number
	TVERTEX vertices[] = 
	{
		{0,						0,						0},
		{(float)m_nPrimitiveW,	(float)m_nPrimitiveH,	2},
		{0,						(float)m_nPrimitiveH,	3},
		{(float)m_nPrimitiveW,	0,						1},
	};
	ZeroStructMem(bufferDesc);
	ZeroStructMem(resUP);
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	resUP.pSysMem = vertices;
	resUP.SysMemPitch = 0;
	resUP.SysMemSlicePitch = 0;
	hr = GetDevice()->CreateBuffer(&bufferDesc, &resUP, &m_pVertexBuffer);
	if (FAILED(hr))
		return RESULT_FAIL;

	// create index buffer
	UINT16 indices[] = 
	{	0,3,1, 1,2,0, // juggled for line ordering of X shape
	};
	m_nIndexCount = sizeof(indices)/sizeof(*indices);
	ZeroStructMem(bufferDesc);
	ZeroStructMem(resUP);
	bufferDesc.ByteWidth = sizeof(indices);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	resUP.pSysMem = indices;
	resUP.SysMemPitch = 0;
	resUP.SysMemSlicePitch = 0;
	hr = GetDevice()->CreateBuffer(&bufferDesc, &resUP, &m_pIndexBuffer);
	if (FAILED(hr))
		return RESULT_FAIL;

	return RESULT_PASS;
}

//---------------------------------------------------------------

TEST_RESULT
CClippingConfTest::SetupRenderTarget( )
{
	SAFE_RELEASE(m_pTextureRT);
	SAFE_RELEASE(m_pTextureRTCopy);
	SAFE_RELEASE(m_pTextureRTResolve);
	SAFE_RELEASE(m_pRTView);

	HRESULT hr = E_FAIL;

	D3D11_TEXTURE2D_DESC tex2ddesc;
	tex2ddesc.ArraySize = 1;
	tex2ddesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tex2ddesc.Format = m_FormatRT;
	tex2ddesc.Height = m_nPrimitiveH;
	tex2ddesc.Width = m_nPrimitiveW;
	tex2ddesc.CPUAccessFlags = 0;
	tex2ddesc.MipLevels = 1;
	tex2ddesc.MiscFlags = 0;
	tex2ddesc.SampleDesc.Count = m_nMultisamples;
	tex2ddesc.SampleDesc.Quality = 0;
	tex2ddesc.Usage = D3D11_USAGE_DEFAULT;

	hr = GetDevice()->CreateTexture2D( &tex2ddesc, NULL, &m_pTextureRT );
	if (FAILED(hr))
		return RESULT_FAIL;

	tex2ddesc.BindFlags = 0;
	tex2ddesc.Usage = D3D11_USAGE_DEFAULT;
	tex2ddesc.CPUAccessFlags = 0;
//	tex2ddesc.Width = 1;
//	tex2ddesc.Height = 1;
	tex2ddesc.SampleDesc.Count = 1;
	tex2ddesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex2ddesc.MiscFlags = 0;

	hr = GetDevice()->CreateTexture2D( &tex2ddesc, NULL, &m_pTextureRTResolve );
	if (FAILED(hr))
		return RESULT_FAIL;

	tex2ddesc.BindFlags = 0;
	tex2ddesc.Usage = D3D11_USAGE_STAGING;
	tex2ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
//	tex2ddesc.Width = 1;
//	tex2ddesc.Height = 1;
	tex2ddesc.SampleDesc.Count = 1;
	tex2ddesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex2ddesc.MiscFlags = 0;

	hr = GetDevice()->CreateTexture2D( &tex2ddesc, NULL, &m_pTextureRTCopy );
	if (FAILED(hr))
		return RESULT_FAIL;

	D3D11_RENDER_TARGET_VIEW_DESC rtviewdesc;
	rtviewdesc.Texture2D.MipSlice = 0;
	rtviewdesc.Format = m_FormatRT;
	if (m_nMultisamples  > 1)
		rtviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	else
		rtviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = GetDevice()->CreateRenderTargetView( m_pTextureRT, &rtviewdesc, &m_pRTView );
	if (FAILED(hr))
		return RESULT_FAIL;
	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, NULL );

	FLOAT colors[4] = { 0.1f, 0.2f, 0.3f, 0.4f };
	GetEffectiveContext()->ClearRenderTargetView( m_pRTView, colors );

	m_nPrevSamples = m_nMultisamples;

	return RESULT_PASS;
}

//---------------------------------------------------------------

TEST_RESULT
CClippingConfTest::SetupShaders( )
{
	TEST_RESULT tRes = RESULT_PASS;
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	HRESULT hr = E_FAIL;

	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pConstBuffers[0]);
	SAFE_RELEASE(m_pConstBuffers[0]);

	if (m_szPixelShaderName.size() > 0)
	{
		hr = D3DX10CompileFromResource(NULL, m_szPixelShaderName.c_str(), NULL, NULL, NULL, "main", "ps_4_0", D3D10_SHADER_DEBUG, 0, NULL, &pShaderBuf, &pErrorBuf, NULL );
		if( FAILED( hr ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "CClippingConfTest::SetupShaders - CompileShaderFromFile() failed assembling PS: %s\n" ), m_szPixelShaderName.c_str() );
			if (pErrorBuf)
			{
				LPVOID pBuf = pErrorBuf->GetBufferPointer();
				size_t bufSize = pErrorBuf->GetBufferSize();
				char* szError = new char[bufSize+1];
				memcpy(szError, pBuf, bufSize);
				szError[bufSize] = 0;
				WriteToLog( _T( "Compiler errors: %s\n" ), szError );
				delete[] szError;
			}
			goto setup_fail;
		}
		if( FAILED( hr = GetDevice( )->CreatePixelShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, &m_pPS ) ) )
		{
			WriteToLog( _T( "CClippingConfTest::SetupShaders - CreatePixelShader() failed. hr=0x%x\n" ), hr );
			tRes = RESULT_FAIL;
			goto setup_fail;
		}
		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
	}
	else if (GetFramework()->GetCurrentTestCase() > 0)
	{
		WriteToLog( _T( "CClippingConfTest::SetupShaders - No pixel shader named\n" ) );
		//tRes = RESULT_FAIL;
		//goto setup_fail;
	}


	if (m_szVertexShaderName.size() > 0)
	{
		hr = D3DX10CompileFromResource(NULL, m_szVertexShaderName.c_str(), NULL, NULL, NULL, "main", "vs_4_0", D3D10_SHADER_DEBUG, 0, NULL, &pShaderBuf, &pErrorBuf, NULL );
		if( FAILED( hr ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "CClippingConfTest::SetupShaders - CompileShaderFromFile() failed assembling VS: %s\n" ), m_szVertexShaderName.c_str() );
			if (pErrorBuf)
			{
				LPVOID pBuf = pErrorBuf->GetBufferPointer();
				size_t bufSize = pErrorBuf->GetBufferSize();
				char* szError = new char[bufSize+1];
				memcpy(szError, pBuf, bufSize);
				szError[bufSize] = 0;
				WriteToLog( _T( "Compiler errors: %s\n" ), szError );
				delete[] szError;
			}
			goto setup_fail;
		}
		if( FAILED( hr = GetDevice( )->CreateVertexShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, &m_pVS ) ) )
		{
			WriteToLog( _T( "CClippingConfTest::SetupShaders - CreateVertexShader() failed. hr=0x%x\n" ), hr );
			tRes = RESULT_FAIL;
			goto setup_fail;
		}


		D3D11_INPUT_ELEMENT_DESC DeclTL[] =
		{
			{"pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},  //position
			{"vertNum", 0, DXGI_FORMAT_R32_UINT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},  //clip distances
		};

		if(	FAILED(	hr = GetDevice()->CreateInputLayout( DeclTL, sizeof(DeclTL)/sizeof(*DeclTL), pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pLayout ) ) )
		{
			WriteToLog(_T("CClippingConfTest::SetupShaders - CreateInputLayout() failed. hr=0x%x\n"), hr);
			tRes = RESULT_FAIL;
			goto setup_fail;
		}
		GetEffectiveContext()->IASetInputLayout( m_pLayout );
		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
	}
	else if (GetFramework()->GetCurrentTestCase() > 0)
	{
		WriteToLog( _T( "CClippingConfTest::SetupShaders - No vertex shader named\n" ) );
		//tRes = RESULT_FAIL;
		//goto setup_fail;
	}

	D3D11_BUFFER_DESC bufdesc;
	bufdesc.ByteWidth = 512;
	bufdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufdesc.MiscFlags = 0;
	GetDevice()->CreateBuffer( &bufdesc, NULL, &m_pConstBuffers[0] );
	bufdesc.ByteWidth = 8*4*sizeof(float);
	bufdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufdesc.MiscFlags = 0;
	GetDevice()->CreateBuffer( &bufdesc, NULL, &m_pConstBuffers[1] );

	GetEffectiveContext()->VSSetConstantBuffers( 0, 2, m_pConstBuffers );
	GetEffectiveContext()->GSSetConstantBuffers( 0, 2, m_pConstBuffers );
	GetEffectiveContext()->PSSetConstantBuffers( 0, 2, m_pConstBuffers );

	{
		float* pMatrix = NULL;
		D3D11_MAPPED_SUBRESOURCE mappedRes;
		HRESULT hr = GetEffectiveContext()->Map( m_pConstBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
		if (SUCCEEDED(hr))
		{
			pMatrix = (float*) mappedRes.pData;
			float transx = (float) m_nPrimitiveW / 2.f;
			float transy = (float) m_nPrimitiveH / 2.f;

			// Inverse viewport scale.
			pMatrix[2] = 1 / transx;
			pMatrix[3] = -1 / transy;

			// Prescaled inverse viewport translation.
			pMatrix[0] = -transx * pMatrix[2];
			pMatrix[1] = -transy * pMatrix[3];

			GetEffectiveContext()->Unmap(m_pConstBuffers[0],0);
		}
	}

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );

	return tRes;

setup_fail:
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	return tRes;
}

TEST_RESULT
CClippingConfTest::Setup()
{
	if ( g_App.m_D3DOptions.Debug )
		m_pDebug = g_App.GetDebug();
	else
		m_pDebug = NULL;

	m_nPrevSamples = 0;

	//TEST_RESULT tRes = SetupDirect3D();
	//if( tRes != RESULT_PASS )
	//	return tRes;

	return RESULT_PASS;
}

TEST_RESULT
CClippingConfTest::SetupTestCase()
{
	assert (   (m_bEnableClipDist && !(m_bEnableCullDist || m_bEnableZClip || m_bEnableXYClip))
			|| (m_bEnableCullDist && !(m_bEnableClipDist || m_bEnableZClip || m_bEnableXYClip))
			|| (m_bEnableXYClip && !(m_bEnableClipDist || m_bEnableZClip || m_bEnableCullDist))
			|| (m_bEnableZClip && !(m_bEnableCullDist || m_bEnableClipDist || m_bEnableXYClip)));

	UINT numQualityLevels = 0;
	GetDevice()->CheckMultisampleQualityLevels(m_FormatRT, m_nMultisamples, &numQualityLevels);
	if (numQualityLevels == 0)
	{
		WriteToLog( _T("Device does not support multisample count of %d\n"), m_nMultisamples );
		return RESULT_SKIP;
	}

	if (m_bEnableClipDist)
	{
		m_szVertexShaderName = _T("vs_clipcullmix.vsh");
		//m_szVertexShaderName = _T("vs_clipdist.vsh");
	}
	else if (m_bEnableCullDist)
		m_szVertexShaderName = _T("vs_culldist.vsh");
	else if (m_bEnableZClip)
		m_szVertexShaderName = _T("vs_depth_clip.vsh");
	else if (m_bEnableXYClip)
		m_szVertexShaderName = _T("vs_w_clip.vsh");
	else
		m_szVertexShaderName = _T("vs_pos_only.vsh");
	m_szPixelShaderName = _T("ps.psh");

	return UpdateResources(); // Modifies the context
}

void
CClippingConfTest::CleanupDirect3D( )
{
	if (GetDevice() != NULL)
	{
		ID3D11Buffer* pNullConstBuffers[2] = {NULL, NULL};
		GetEffectiveContext()->VSSetConstantBuffers( 0, 2, pNullConstBuffers );
		GetEffectiveContext()->GSSetConstantBuffers( 0, 2, pNullConstBuffers );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 2, pNullConstBuffers );
		GetEffectiveContext()->VSSetShader(NULL, NULL, 0);
		GetEffectiveContext()->GSSetShader(NULL, NULL, 0);
		GetEffectiveContext()->PSSetShader(NULL, NULL, 0);

		HRESULT hr;
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context: %s"), D3DHResultToString(hr).c_str() );
		}
	}

	SAFE_RELEASE( m_pConstBuffers[0] );
	SAFE_RELEASE( m_pConstBuffers[1] );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pRSEnabled );
	SAFE_RELEASE( m_pRTView );
	SAFE_RELEASE( m_pTextureRT );
	SAFE_RELEASE( m_pTextureRTCopy );
	SAFE_RELEASE( m_pTextureRTResolve );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pIndexBuffer );
	SAFE_RELEASE( m_pLayout );
}

void
CClippingConfTest::Cleanup( )
{
	CleanupDirect3D( );
}

TEST_RESULT
CClippingConfTest::UpdateResources()
{
	TEST_RESULT tRes = RESULT_PASS;

	bool needsUpdate = false;
	if (m_nPrevSamples != m_nMultisamples)
	{
		m_nPrevSamples = m_nMultisamples;
		needsUpdate = true;
	}
	if (m_szPrevPSName != m_szPixelShaderName)
	{
		m_szPrevPSName = m_szPixelShaderName;
		needsUpdate = true;
	}
	if (m_szPrevVSName != m_szVertexShaderName)
	{
		m_szPrevVSName = m_szVertexShaderName;
		needsUpdate = true;
	}

	if (needsUpdate)
	{
		CleanupDirect3D();
		tRes = SetupDirect3D();
		if (RESULT_PASS != tRes)
		{
			m_nPrevSamples = 0;
			goto update_end;
		}
	}

	GetEffectiveContext()->VSSetConstantBuffers( 0, 2, m_pConstBuffers );
	GetEffectiveContext()->GSSetConstantBuffers( 0, 2, m_pConstBuffers );
	GetEffectiveContext()->PSSetConstantBuffers( 0, 2, m_pConstBuffers );

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );

	GetEffectiveContext()->IASetInputLayout( m_pLayout );

	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, NULL );

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float) m_nPrimitiveW;
	vp.Height   = (float) m_nPrimitiveH;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	GetEffectiveContext()->RSSetViewports(1, &vp);

	GetEffectiveContext()->RSSetState(m_pRSEnabled);

update_end:
	return tRes;
}

HRESULT
CClippingConfTest::DrawQuad(const clipvec4 clipdists[4][2])
{
	HRESULT hr = S_OK;

	if (!m_pConstBuffers[1])
		return E_FAIL;

	float* pConstData = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	hr = GetEffectiveContext()->Map( m_pConstBuffers[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
	pConstData = (float*) mappedRes.pData;

	memcpy(pConstData, clipdists, 8*4*sizeof(float));
	if (m_bEnableZClip)
	{
		pConstData[0] /= 4.0f;
		pConstData[8] /= 4.0f;
		pConstData[16] /= 4.0f;
		pConstData[24] /= 4.0f;
	}

	GetEffectiveContext()->Unmap(m_pConstBuffers[1],0);

	UINT stride = sizeof(TVERTEX);
	UINT offset = 0;
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	GetEffectiveContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	GetEffectiveContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	GetEffectiveContext()->DrawIndexed(m_nIndexCount, 0, 0);

	return hr;
}

HRESULT
CClippingConfTest::DrawLine(const float clipdists[4])
{
	HRESULT hr = S_OK;

	if (!m_pConstBuffers[1])
		return E_FAIL;

	float* pConstData = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	GetEffectiveContext()->Map( m_pConstBuffers[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes);
	pConstData = (float*) mappedRes.pData;

	ZeroMemory(pConstData, 8*4*sizeof(float));

	pConstData[0] = clipdists[0];
	pConstData[8] = clipdists[1];
	pConstData[16] = clipdists[2];
	pConstData[24] = clipdists[3];
	if (m_bEnableZClip)
	{
		pConstData[0] /= 4.0f;
		pConstData[8] /= 4.0f;
		pConstData[16] /= 4.0f;
		pConstData[24] /= 4.0f;
	}

	GetEffectiveContext()->Unmap(m_pConstBuffers[1],0);

	UINT stride = sizeof(TVERTEX);
	UINT offset = 0;
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
	GetEffectiveContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	GetEffectiveContext()->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	GetEffectiveContext()->Draw(4, 0);

	return hr;
}

static
void
grabD3DColor(DXGI_FORMAT format, const D3D11_MAPPED_SUBRESOURCE *pTexMap, int x, int y, TEST_COLOR* pColor)
{
	assert(format == DXGI_FORMAT_R32G32B32A32_FLOAT);

	float* pData = (float*)((BYTE*)(pTexMap->pData) + sizeof(float)*4*x + y*pTexMap->RowPitch);

	pColor->r = pData[0];
	pColor->g = pData[1];
	pColor->b = pData[2];
	pColor->a = pData[3];
}

HRESULT
CClippingConfTest::ReadBackBufferColor(int x, int y, TEST_COLOR* pColor)
{
	if (m_nMultisamples > 1)
	{
		GetEffectiveContext()->ResolveSubresource( m_pTextureRTResolve, 0, m_pTextureRT, 0, DXGI_FORMAT_R32G32B32A32_FLOAT );
		GetEffectiveContext()->CopyResource( m_pTextureRTCopy, m_pTextureRTResolve );
	}
	else
		GetEffectiveContext()->CopyResource( m_pTextureRTCopy, m_pTextureRT );

	HRESULT hr;
	if( FAILED(hr = ExecuteEffectiveContext()) )
	{
		WriteToLog( _T("Failed to execute deferred context: %s"), D3DHResultToString(hr).c_str() );
		return hr;
	}

	D3D11_MAPPED_SUBRESOURCE texMap;
	hr = GetImmediateContext()->Map( m_pTextureRTCopy, 0, D3D11_MAP_READ, 0, &texMap );
	if (SUCCEEDED(hr))
	{
		grabD3DColor(DXGI_FORMAT_R32G32B32A32_FLOAT, &texMap, x, y, pColor);
	}
	GetImmediateContext()->Unmap( m_pTextureRTCopy, 0 );

	return hr;
}

bool
CClippingConfTest::TestColorsSame(const TEST_COLOR& c1, const TEST_COLOR& c2, double delta)
{
	if (   fabs(c1.r - c2.r) > delta
		|| fabs(c1.g - c2.g) > delta
		|| fabs(c1.b - c2.b) > delta
		|| fabs(c1.a - c2.a) > delta)
	{
		return false;
	}
	return true;
}

TEST_RESULT
CClippingConfTest::CheckQuadPattern(const pixelLit_t* lit)
{
	TEST_RESULT tRes = RESULT_PASS;

	int x,y;
	for (y = 0; y < m_nPrimitiveH; ++y)
	{
		for (x = 0; x < m_nPrimitiveW; ++x)
		{
			TEST_COLOR bufferColor, testColor;
			double delta;
			if (FAILED(ReadBackBufferColor(x,y,&bufferColor)))
			{
				if (tRes != RESULT_FAIL)
					tRes = RESULT_FAIL;
				WriteToLog( _T("Failed to read backbuffer at x=%d y=%d.\n"), x, y );
			}
			switch(lit[y*m_nPrimitiveW+x])
			{
			default:
				tRes = RESULT_FAIL;
				break;
			case pixel_lit:
				testColor.a = testColor.r = testColor.g = testColor.b = 1;
				delta = 1.0/1024;
				break;
			case pixel_unlit:
				testColor.a = testColor.r = testColor.g = testColor.b = 0;
				delta = 1.0/1024;
				break;
			case pixel_partial_low:
				testColor.a = testColor.r = testColor.g = testColor.b = 0.375;
				delta = 0.375-1.0/1024;
				break;
			case pixel_partial_high:
				testColor.a = testColor.r = testColor.g = testColor.b = 0.625;
				delta = 0.375-1.0/1024;
				break;
			case pixel_edgecase:
				testColor.a = testColor.r = testColor.g = testColor.b = 0.5;
				delta = 0.5;
				break;
			}
			if (!TestColorsSame(bufferColor, testColor, delta))
			{
				if (tRes != RESULT_FAIL)
					tRes = RESULT_FAIL;
				WriteToLog( _T("Color mismatch at (x,y)=(%d,%d). Expected %g (+/-%g), found %g\n"), x, y, testColor.r, delta, bufferColor.r );
			}
		}
	}

	return tRes;
}

float
f_lerp(float t, float t0, float t1)
{
	return t0 + t*(t1-t0);
}

void
vec3_sub(float out[3], float head[3], float tail[3])
{
	out[0] = head[0] - tail[0];
	out[1] = head[1] - tail[1];
	out[2] = head[2] - tail[2];
}

float
vec3_dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void
vec3_cross(float out[3], float v1[3], float v2[3])
{
	float temp[3];
	temp[0] = v1[1]*v2[2] - v1[2]*v2[1];
	temp[1] = v1[2]*v2[0] - v1[0]*v2[2];
	temp[2] = v1[0]*v2[1] - v1[1]*v2[0];
	out[0] = temp[0];
	out[1] = temp[1];
	out[2] = temp[2];
}

void
vec3_normalize(float v[3])
{
	float len = sqrtf(vec3_dot(v, v));
	float invlen = 1.0f/len;
	v[0] *= invlen;
	v[1] *= invlen;
	v[2] *= invlen;
}

void
make_plane(float plane[4], float v0[3], float v1[3], float v2[3])
{
	float d1[3], d2[3];
	vec3_sub(d1, v1, v0);
	vec3_sub(d2, v2, v0);
	vec3_cross(plane, d1, d2);
	vec3_normalize(plane);
	plane[3] = -vec3_dot(plane, v0);
}

float
plane_intersect(float plane[4], float planept[3], float p0[3], float p1[3])
{
	float d1[3], d2[3];
	vec3_sub(d1, planept, p0);
	vec3_sub(d2, p1, p0);
	return vec3_dot(plane, d1) / vec3_dot(plane, d2);
}

float
test_point(float plane[4], float planePt[3], float x, float y)
{
	float testPt[2][3] = {
		{x, y, 0},
		{x, y, 1}
	};
	return plane_intersect(plane, planePt, testPt[0], testPt[1]);
}

float
f_clamp(float in, float epsilon)
{
	if (fabsf(in) < epsilon)
		return 0;
	return in;
}

int
f_sign(double in)
{
	if (in >= 0)
		return 1;
	return -1;
}

float
f_getNaN()
{
	UINT32 dwNaNVal = 0x7f800001; // QNaN
	return *(float*)(&dwNaNVal);
}

bool
isNaN(float in)
{
	UINT32 dwInVal = *(UINT32*)(&in);
	if ((dwInVal & 0x7f800000) != 0x7f800000)
		return false;
	if ((dwInVal & 0x007fffff) == 0)
		return false; // +infinity
	return true;
}

bool
isINF(float in)
{
	UINT32 dwInVal = *(UINT32*)(&in);
	if ((dwInVal & 0x7fffffff) != 0x7f800000)
		return false;
	return true;
}

bool
test_range(double test, double lower, double upper)
{
	assert(lower < upper);
	if (isINF((float)test) || isNaN((float)test))
		return false;
	if (test < lower || upper < test)
		return false;
	return true;
}

double
f_quantize(double in, int bits)
{
	if (isNaN(float(in)) || isINF(float(in)))
		return in;
	INT64 iVal = (INT64)((in - (int)in) * ((INT64)1 << bits) + 0.5*f_sign(in));
	return (int)in + ((double)iVal)/((INT64)1 << bits);
}

double
f_get_parameter(double in, double start, double stop)
{
	return (in-start)/(stop-start);
}

double
f_interpolate(double param, double start, double stop)
{
	//assert(test_range(param, 0.0, 1.0));
	return start + param*(stop-start);
}

float maxNoNaN(float a, float b)
{
	bool aNaN = isNaN(a);
	bool bNaN = isNaN(b);
	if (aNaN || bNaN)
	{
		if (aNaN && bNaN)
			return a;
		if (aNaN)
			return b;
		return a;
	}
	return (a > b) ? a : b;
}

void
CClippingConfApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();
	UnRegisterOptionVariable("srconly");
	m_D3DOptions.SrcOnly = true;
}

bool
CClippingConfApp::InitTestGroups()
{
	// Register test classes with the group manager.
	if( !ADD_TEST( _T( "Clipping\\Line\\ZClip" ), CLineClipTest ) ) return false;
	if( !ADD_TEST( _T( "Clipping\\Line\\ClipDist" ), CLineClipTest ) ) return false;
	if( !ADD_TEST( _T( "Clipping\\Line\\CullDist" ), CLineClipTest ) ) return false;

	//if( !ADD_TEST( _T( "Clipping\\Triangle" ), CTriangleClipTest ) ) return false;
	if( !ADD_TEST( _T( "Clipping\\Triangle\\ZClip" ), CTriangleClipTest ) ) return false;
	if( !ADD_TEST( _T( "Clipping\\Triangle\\ClipDist" ), CTriangleClipTest ) ) return false;
	if( !ADD_TEST( _T( "Clipping\\Triangle\\CullDist" ), CTriangleClipTest ) ) return false;

	return true;
}

