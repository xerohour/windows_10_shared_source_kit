#include "Filter.h"
#include "D3DMSHelper.h"
#include "d3d11internaltest.h" // Internal interfaces for ID3D11DeviceTest
#include "CreateDevice.h"


CFilterConfApp  g_App;

///////////////////////////////////////////////////////////////////////////////
//	Filtering Conformance testing
///////////////////////////////////////////////////////////////////////////////

double f_quantize(double in, int bits);

int nextMultipleOfPow2(int in, int pow2)
{
	pow2 -= 1;
	return (in+pow2) & (~pow2);
}

int pow2roundup (int x)
{
	if (x <= 0)
		return 0;
	--x;
	int y = x;
	while( y > 1 )
	{
		y = y >> 1;
		x |= y;
	}
	return x+1;
}

int
CFilterConfTest::GetNumRenderWindows() const
{
	return (g_App.VisibleRender())?1:0;
}

TEST_RESULT
CFilterConfTest::SetupDirect3D( )
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
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = true;
	rastDesc.DepthBias = false;
	rastDesc.DepthBiasClamp = 0.0f;
	rastDesc.SlopeScaledDepthBias = 0.0f;
	rastDesc.DepthClipEnable = true;
	rastDesc.ScissorEnable = false;
	rastDesc.MultisampleEnable = false;
	rastDesc.AntialiasedLineEnable = false;
	m_pDevice->CreateRasterizerState( &rastDesc, &m_pRSEnabled );

	GetEffectiveContext()->RSSetState( m_pRSEnabled );

	if( FAILED(ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

//---------------------------------------------------------------

TEST_RESULT
CFilterConfTest::SetupGeometry( )
{
	TEST_RESULT tRes = RESULT_PASS;

	HRESULT hr;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA resUP;

	SAFE_RELEASE(m_pVertexBufferQuad);
	SAFE_RELEASE(m_pVertexBufferPoints);
	SAFE_RELEASE(m_pIndexBufferQuad);

	// create vertex buffer
	// Z stores vertex number
	TVERTEX vertices[] = 
	{	//x						y						vertNum
		{0,						0,						0},
		{(float)m_nPrimitiveW,	0,						1},
		{(float)m_nPrimitiveW,	(float)m_nPrimitiveH,	2},
		{0,						(float)m_nPrimitiveH,	3},
	};
	// 0--1
	// |\ |
	// | \|
	// 3--2
	ZeroStructMem(bufferDesc);
	ZeroStructMem(resUP);
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	resUP.pSysMem = vertices;
	resUP.SysMemPitch = 0;
	resUP.SysMemSlicePitch = 0;
	hr = GetFramework()->CreateBufferTiledOptional(&bufferDesc, &resUP, &m_pVertexBufferQuad);
	if (FAILED(hr))
	{
		WriteToLog( _T( "%s CreateBuffer() failed for vertex buffer Quad. hr=%s\n" ), LOG_HEADER, D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto setup_fail;
	}

	if (m_ShaderType == ShaderType_Vertex || m_ShaderType == ShaderType_Geometry)
	{
		// create a grid of vertices for VS/GS usage in lieu of pixel positions
		UINT numVerts = m_nPrimitiveW*m_nPrimitiveH;
		TVERTEX* pPointVerts = new TVERTEX[numVerts];
		bufferDesc.ByteWidth = numVerts * sizeof(TVERTEX);
		resUP.pSysMem = pPointVerts;

		for (int y = 0; y < m_nPrimitiveH; ++y)
		{
			for (int x = 0; x < m_nPrimitiveW; ++x)
			{
				int index = x + y*m_nPrimitiveW;
				pPointVerts[index].vPos[0] = (float)(x + 0.5f);
				pPointVerts[index].vPos[1] = (float)(y + 0.5f);
				pPointVerts[index].vNum = 0;
			}
		}

		hr = GetFramework()->CreateBufferTiledOptional(&bufferDesc, &resUP, &m_pVertexBufferPoints);
		delete[] pPointVerts;
		if (FAILED(hr))
		{
			WriteToLog( _T( "%s CreateBuffer() failed for vertex buffer Points. hr=%s\n" ), LOG_HEADER, D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto setup_fail;
		}
	}

	// create index buffer
	UINT16 indices[] = 
	{	0,1,2, 2,3,0,
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
	hr = GetFramework()->CreateBufferTiledOptional(&bufferDesc, &resUP, &m_pIndexBufferQuad);
	if (FAILED(hr))
	{
		WriteToLog( _T( "%s CreateBuffer() failed for index buffer. hr=%s\n" ), LOG_HEADER, D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto setup_fail;
	}

	return tRes;

setup_fail:
	SAFE_RELEASE(m_pVertexBufferQuad);
	SAFE_RELEASE(m_pVertexBufferPoints);
	SAFE_RELEASE(m_pIndexBufferQuad);

	return tRes;
}

//---------------------------------------------------------------

TEST_RESULT
CFilterConfTest::SetupRenderTarget( )
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	SAFE_RELEASE(m_pTextureRT);
	SAFE_RELEASE(m_pRTShaderView);
	SAFE_RELEASE(m_pRTView);

	{
		D3D11_TEXTURE2D_DESC tex2ddesc;
		tex2ddesc.ArraySize = 1;
		tex2ddesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tex2ddesc.CPUAccessFlags = 0;
		tex2ddesc.Format = m_FormatRT;
		tex2ddesc.Height = m_nPrimitiveH;
		tex2ddesc.Width = m_nPrimitiveW;
		tex2ddesc.MipLevels = 1;
		tex2ddesc.MiscFlags = 0;
		tex2ddesc.SampleDesc.Count = m_bMultisampleEnable?m_nMultiSamples:1;
		tex2ddesc.SampleDesc.Quality = 0;
		tex2ddesc.Usage = D3D11_USAGE_DEFAULT;

		hr = GetFramework()->CreateTexture2DTiledOptional( &tex2ddesc, NULL, &m_pTextureRT );
		if (FAILED(hr))
		{
			WriteToLog( _T( "%s CreateTexture2D() failed for rendertarget. hr=%s\n" ), LOG_HEADER, D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto setup_fail;
		}
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = m_bMultisampleEnable?D3D11_SRV_DIMENSION_TEXTURE2DMS:D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		hr = m_pDevice->CreateShaderResourceView( m_pTextureRT, &srvDesc, &m_pRTShaderView );
		if (FAILED(hr))
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "%s CreateShaderResourceView() failed for rendertarget %d. hr=%s\n" ), LOG_HEADER, 0, D3DHResultToString(hr).c_str() );
			goto setup_fail;
		}
	}

	{
		D3D11_RENDER_TARGET_VIEW_DESC rtviewdesc;
		rtviewdesc.Texture2D.MipSlice = 0;
		rtviewdesc.Format = m_FormatRT;
		rtviewdesc.ViewDimension = m_bMultisampleEnable?D3D11_RTV_DIMENSION_TEXTURE2DMS:D3D11_RTV_DIMENSION_TEXTURE2D;
		hr = m_pDevice->CreateRenderTargetView( m_pTextureRT, &rtviewdesc, &m_pRTView );
		if (FAILED(hr))
		{
			WriteToLog( _T( "%s CreateRenderTargetView() failed. hr=%s\n" ), LOG_HEADER, D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto setup_fail;
		}
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, NULL );

		FLOAT colors[4] = { 0.1f, 0.2f, 0.3f, 0.4f };
		GetEffectiveContext()->ClearRenderTargetView( m_pRTView, colors );
	}

	m_nPrevSamples = m_nMultiSamples;

	return tRes;

setup_fail:
	SAFE_RELEASE(m_pTextureRT);
	SAFE_RELEASE(m_pRTShaderView);
	SAFE_RELEASE(m_pRTView);

	return tRes;
}

//---------------------------------------------------------------

HRESULT CompileShader(CD3D11Test* pTest, tstring shaderName, LPCSTR entryPoint, D3D_SHADER_STAGE shaderStage, 
					  ID3D11VertexShader** pVS, ID3D11GeometryShader** pGS, ID3D11PixelShader** pPS,
					  ID3D10Blob** pShaderBufOut)
{
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	tstring shaderType;
	HRESULT hr;
	UINT flags = 0;

	if (!pTest->GetFramework()->IsMobile())
	{
		flags |= D3D10_SHADER_DEBUG;
	}

	hr = g_App.CompileShaderFromResource(NULL, shaderName.c_str(), entryPoint, shaderStage, D3D_FEATURE_LEVEL_9_1, D3D_FEATURE_LEVEL_11_0, flags, &pShaderBuf, &pErrorBuf );
	if( FAILED( hr ) )
	{
		pTest->WriteToLog( _T( "%s CompileShaderFromResource() failed on shader: %s : %s\n" ), LOG_HEADER, shaderName.c_str(), entryPoint );
		if (pErrorBuf)
		{
			LPVOID pBuf = pErrorBuf->GetBufferPointer();
			size_t bufSize = pErrorBuf->GetBufferSize();
			char* szError = new char[bufSize+1];
			memcpy(szError, pBuf, bufSize);
			szError[bufSize] = 0;
			pTest->WriteToLog( _T( "%s Compiler errors: %s\n" ), LOG_HEADER, szError );
			delete[] szError;
		}
		goto setup_fail;
	}
	if (pPS)
	{
		shaderType = "PixelShader";
		hr = pTest->GetDevice( )->CreatePixelShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, pPS );
	}
	else if (pVS)
	{
		shaderType = "VertexShader";
		hr = pTest->GetDevice( )->CreateVertexShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, pVS );
	}
	else if (pGS)
	{
		shaderType = "GeometryShader";
		hr = pTest->GetDevice( )->CreateGeometryShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, pGS );
	}
	if( FAILED(hr) )
	{
		pTest->WriteToLog( _T( "%s Create%s() failed. hr=%s\n" ), LOG_HEADER, shaderType.c_str(), D3DHResultToString(hr).c_str() );
		goto setup_fail;
	}
	if (pShaderBufOut)
	{
		if (pShaderBuf)
			pShaderBuf->AddRef();
		*pShaderBufOut = pShaderBuf;
	}
setup_fail:
	SAFE_RELEASE(pShaderBuf);
	SAFE_RELEASE(pErrorBuf);
	return hr;
}

static HRESULT CreateShaderFromResource(CD3D11Test* pTest, tstring shaderName, 
										ID3D11VertexShader** pVS, ID3D11GeometryShader** pGS, ID3D11PixelShader** pPS,
										ID3D10Blob** pShaderBufOut)
{
	ID3D10Blob *pShaderBuf = NULL;
	tstring shaderType;
	HRESULT hr = E_UNEXPECTED;
	void* pResourceData = NULL;
	SIZE_T resourceSize = 0;
	{
		HRSRC resourceInfo = FindResource(NULL, shaderName.c_str(), RT_RCDATA);
		if (resourceInfo != NULL)
		{
			resourceSize = SizeofResource(NULL, resourceInfo);
			HGLOBAL resourceHandle = LoadResource(NULL, resourceInfo);
			if (resourceHandle != NULL)
			{
				pResourceData = LockResource(resourceHandle);
			}
		}
		hr = HRESULT_FROM_WIN32(GetLastError());
		if (pResourceData == NULL)
		{
			pTest->WriteToLog( _T( "%s Failed to load resource '%s'. hr=%s.\n" ), LOG_HEADER, shaderName.c_str(), D3DHResultToString(hr).c_str() );
			goto setup_fail;
		}
	}
	{
		ID3D10Blob* p10Blob = NULL;
		hr = D3D10CreateBlob(resourceSize, &p10Blob);
		if (FAILED(hr) || p10Blob == NULL)
			goto setup_fail;
		pShaderBuf = p10Blob;
		void* pBlobData = pShaderBuf->GetBufferPointer();
		memcpy(pBlobData, pResourceData, resourceSize);
	}
	if (pPS)
	{
		shaderType = "PixelShader";
		hr = pTest->GetDevice( )->CreatePixelShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, pPS );
	}
	else if (pVS)
	{
		shaderType = "VertexShader";
		hr = pTest->GetDevice( )->CreateVertexShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, pVS );
	}
	else if (pGS)
	{
		shaderType = "GeometryShader";
		hr = pTest->GetDevice( )->CreateGeometryShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, pGS );
	}
	if( FAILED(hr) )
	{
		pTest->WriteToLog( _T( "%s Create%s() failed. hr=%s\n" ), LOG_HEADER, shaderType.c_str(), D3DHResultToString(hr).c_str() );
		goto setup_fail;
	}
	if (pShaderBufOut)
	{
		if (pShaderBuf)
			pShaderBuf->AddRef();
		*pShaderBufOut = pShaderBuf;
	}
setup_fail:
	SAFE_RELEASE(pShaderBuf);
	return hr;
}

TEST_RESULT
CFilterConfTest::SetupShaders( )
{
	if (GetPath() == "")
		return RESULT_NONE;

	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = E_FAIL;

	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pPSVisibleCopy);
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pVSDefault);
	SAFE_RELEASE(m_pGS);
	SAFE_RELEASE(m_pConstBuffers[0]);
	SAFE_RELEASE(m_pConstBuffers[1]);

	if (m_sPixelShaderName.size() > 0)
	{
		if (g_App.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0)
		{
			if ((m_nDimension == 2 && m_bSampleAOffImmI && m_sPixelShaderName == "ps_sample_cmp.psh"))
			{
				tstring shaderName = _T("ps_sample_cmp_");
				shaderName += m_sPSEntryPt;
				shaderName += _T(".bsh");
				hr = CreateShaderFromResource(this, shaderName, NULL, NULL, &m_pPS, NULL);
				goto pixelShader_Loaded;
			}
		}
		hr = CompileShader(this, m_sPixelShaderName, m_sPSEntryPt.c_str(), D3D_SHADER_STAGE_PIXEL, NULL, NULL, &m_pPS, NULL);
pixelShader_Loaded:
		if( FAILED(hr) )
			tRes = RESULT_FAIL;
	}

	if (g_App.VisibleRender())
	{
		ID3D10Blob *pShaderBuf = NULL;
		tstring sCopyShader = g_App.GetIsFL9Test()? _T("ps_copy_L9.psh"): _T("ps_copy.psh");
		hr = CompileShader(this, sCopyShader, ((m_bMultisampleEnable)?"ps_main_ms":"ps_main"), D3D_SHADER_STAGE_PIXEL, NULL, NULL, &m_pPSVisibleCopy, NULL);
		if( FAILED(hr) )
			tRes = RESULT_FAIL;
		hr = CompileShader(this, sCopyShader, "vs_main", D3D_SHADER_STAGE_VERTEX, &m_pVSDefault, NULL, NULL, &pShaderBuf);
		if( FAILED(hr) )
			tRes = RESULT_FAIL;

		DXGI_FORMAT indexFormat = g_App.GetIsFL9Test()? DXGI_FORMAT_R32_FLOAT: DXGI_FORMAT_R32_UINT;
		D3D11_INPUT_ELEMENT_DESC DeclTL[] =
		{
			{"pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},  //position
			{"vertNum", 0, indexFormat, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},  //index into cbuffer
		};

		if (pShaderBuf)
		{
			hr = m_pDevice->CreateInputLayout( DeclTL, sizeof(DeclTL)/sizeof(*DeclTL), pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pLayoutDefault );
			SAFE_RELEASE(pShaderBuf);
			if(	FAILED(hr) )
			{
				tRes = RESULT_FAIL;
				WriteToLog(_T("%s CreateInputLayout() failed. hr=%s\n" ), LOG_HEADER, D3DHResultToString(hr).c_str() );
			}
		}
	}


	if (m_sVertexShaderName.size() > 0)
	{
		ID3D10Blob *pShaderBuf = NULL;
		hr = CompileShader(this, m_sVertexShaderName, m_sVSEntryPt.c_str(), D3D_SHADER_STAGE_VERTEX, &m_pVS, NULL, NULL, &pShaderBuf);
		if( FAILED(hr) )
			tRes = RESULT_FAIL;

		DXGI_FORMAT indexFormat = g_App.GetIsFL9Test()? DXGI_FORMAT_R32_FLOAT: DXGI_FORMAT_R32_UINT;
		D3D11_INPUT_ELEMENT_DESC DeclTL[] =
		{
			{"pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},  //position
			{"vertNum", 0, indexFormat, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},  //index into cbuffer
		};

		if (pShaderBuf)
		{
			hr = m_pDevice->CreateInputLayout( DeclTL, sizeof(DeclTL)/sizeof(*DeclTL), pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pLayout );
			SAFE_RELEASE(pShaderBuf);
			if(	FAILED(hr) )
			{
				tRes = RESULT_FAIL;
				WriteToLog(_T("%s CreateInputLayout() failed. hr=%s\n" ), LOG_HEADER, D3DHResultToString(hr).c_str() );
			}
		}
	}

	if (m_sGeometryShaderName.size() > 0)
	{
		hr = CompileShader(this, m_sGeometryShaderName, m_sGSEntryPt.c_str(), D3D_SHADER_STAGE_GEOMETRY, NULL, &m_pGS, NULL, NULL);
		if( FAILED(hr) )
			tRes = RESULT_FAIL;
	}

	D3D11_BUFFER_DESC bufdesc;
	bufdesc.ByteWidth = 4*4*sizeof(float); // 4x4 matrix
	bufdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufdesc.MiscFlags = 0;
	hr = GetFramework()->CreateBufferTiledOptional( &bufdesc, NULL, &m_pConstBuffers[0] );
	if (FAILED(hr) )
	{
		tRes = RESULT_FAIL;
		WriteToLog(_T("%s CreateBuffer() failed for constant buffer 0. hr=%s\n" ), LOG_HEADER, D3DHResultToString(hr).c_str() );
		goto setup_fail;
	}
	bufdesc.ByteWidth = 8*4*sizeof(float);
	bufdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufdesc.MiscFlags = 0;
	hr = GetFramework()->CreateBufferTiledOptional( &bufdesc, NULL, &m_pConstBuffers[1] );
	if (FAILED(hr) )
	{
		tRes = RESULT_FAIL;
		WriteToLog(_T("%s CreateBuffer() failed for constant buffer 1. hr=%s\n" ), LOG_HEADER, D3DHResultToString(hr).c_str() );
		goto setup_fail;
	}

	GetEffectiveContext()->VSSetConstantBuffers( 0, 2, m_pConstBuffers );
	if( !g_App.GetIsFL9Test() )
		GetEffectiveContext()->GSSetConstantBuffers( 0, 2, m_pConstBuffers );
	GetEffectiveContext()->PSSetConstantBuffers( 0, 2, m_pConstBuffers );

	{
		float* pMatrix = NULL;
		D3D11_MAPPED_SUBRESOURCE mappedRes;
		HRESULT hr = GetImmediateContext()->Map( m_pConstBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
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

			GetImmediateContext()->Unmap(m_pConstBuffers[0],0);
		}
	}

	if (tRes != RESULT_PASS)
		goto setup_fail;

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );

	return tRes;

setup_fail:
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pPSVisibleCopy);
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pGS);

	return tRes;
}

TEST_RESULT
CFilterConfTest::Setup()
{
	m_pDevice = g_App.GetDevice();
	m_pDeviceContext = g_App.GetImmediateContext();

	if ( g_App.m_D3DOptions.Debug )
		m_pDebug = g_App.GetDebug();
	else
		m_pDebug = NULL;

	if( g_App.GetIsFL9Test() )
		m_FormatRT = FORMAT_RT_L9;

	TEST_RESULT tRes = SetupDirect3D();
	if( tRes != RESULT_PASS )
		return tRes;

	return RESULT_PASS;
}

void
CFilterConfTest::CleanupDirect3D( )
{
	if (GetEffectiveContext() != nullptr)
	{
		ID3D11Buffer* pNullConstBuffers[2] = {NULL, NULL};
		ID3D11RenderTargetView* pNullRTViews[8] = {NULL, };
		GetEffectiveContext()->VSSetConstantBuffers( 0, 2, pNullConstBuffers );
		if( !g_App.GetIsFL9Test() )
			GetEffectiveContext()->GSSetConstantBuffers( 0, 2, pNullConstBuffers );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 2, pNullConstBuffers );
		GetEffectiveContext()->VSSetShader(NULL, NULL, 0);
		GetEffectiveContext()->GSSetShader(NULL, NULL, 0);
		GetEffectiveContext()->PSSetShader(NULL, NULL, 0);
		GetEffectiveContext()->IASetInputLayout(NULL);
		GetEffectiveContext()->RSSetState(NULL);
		GetEffectiveContext()->OMSetRenderTargets( 8, pNullRTViews, NULL );
	}

	if( FAILED(ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
	}

	SAFE_RELEASE( m_pConstBuffers[0] );
	SAFE_RELEASE( m_pConstBuffers[1] );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pVSDefault );
	SAFE_RELEASE( m_pPSVisibleCopy );
	SAFE_RELEASE( m_pRSEnabled );
	SAFE_RELEASE( m_pRTView );
	SAFE_RELEASE( m_pTextureRT );
	SAFE_RELEASE( m_pRTShaderView );
	SAFE_RELEASE( m_pVertexBufferQuad );
	SAFE_RELEASE( m_pVertexBufferPoints );
	SAFE_RELEASE( m_pIndexBufferQuad );
	SAFE_RELEASE( m_pLayout );
	SAFE_RELEASE( m_pLayoutDefault );

	if (GetEffectiveContext() != nullptr)
	{
		GetEffectiveContext()->Flush();
	}
}

void
CFilterConfTest::Cleanup( )
{
	CleanupDirect3D( );

	if (m_pTexData)
	{
		for (int n = 0; n < 256; ++n)
			SAFE_DELETE_ARRAY(m_pTexData[n]);
		SAFE_DELETE_ARRAY(m_pTexData);
	}
}

void
CFilterConfTest::PreInitTestParams()
{
	m_nPrimitiveW = g_App.Width();
	m_nPrimitiveH = g_App.Height();

	g_App.m_RenderTargetOptions.SwapChainDesc.Width = m_nPrimitiveW*2;
	g_App.m_RenderTargetOptions.SwapChainDesc.Height = m_nPrimitiveH;
	g_App.m_RenderTargetOptions.DepthTextureDesc.Width = m_nPrimitiveW*2;
	g_App.m_RenderTargetOptions.DepthTextureDesc.Height = m_nPrimitiveH;
	g_App.m_RenderTargetOptions.NoDepthStencils = true;

	bool bIsLevel9Test = g_App.GetIsFL9Test();
	m_sVertexShaderName = bIsLevel9Test? "vs_L9.vsh": "vs.vsh";
	m_sGeometryShaderName = "";
	m_sPixelShaderName = bIsLevel9Test? "ps_basic_L9.psh": "ps_basic.psh";
	m_sVSEntryPt = _T("main");
	m_sGSEntryPt = _T("main");
	m_sPSEntryPt = _T("main");

	m_SamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	m_SamplerDesc.MaxAnisotropy = 1;

	m_SamplerDesc.MinLOD = -FLT_MAX;
	m_SamplerDesc.MaxLOD = FLT_MAX;

	m_nTextureSize[0] = 6;
	m_nTextureSize[1] = 4;
	m_nTextureSize[2] = 4;
	m_nArraySize = 0;

	// dimension
	{
		tstring sPathDimension = GetPath().ToString( GetPath().Depth() - 1 );
		int dimension = 0;
		if (sPathDimension == _T("1D"))
			dimension = 1;
		else if (sPathDimension == _T("2D"))
			dimension = 2;
		else if (sPathDimension == _T("3D"))
			dimension = 3;
		else if (sPathDimension == _T("Cube"))
			dimension = 4;
		assert(dimension >= 1 && dimension <= 4);
		m_nDimension = dimension;
	}
}

TEST_RESULT
CFilterConfTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	m_pTempMultisampleData = NULL;

	m_sPSEntryPt = _T("main_");
	m_sPSEntryPt += (TCHAR)('0' + m_nDimension);

	tstring sPathFilterType = GetPath().ToString( GetPath().Depth() - 2, 1 );
	if (sPathFilterType == "VS")
		m_ShaderType = ShaderType_Vertex;
	else if (sPathFilterType == "GS")
		m_ShaderType = ShaderType_Geometry;
	else if (sPathFilterType == "MipGen")
		m_ShaderType = ShaderType_MipGen;
	else
		m_ShaderType = ShaderType_Pixel;

	if (m_SampleFunction == SampleCmpLvlZero)
	{
		m_sPSEntryPt += _T("_L0");
	}

	if (m_bSampleAOffImmI)
	{
		if (m_nDimension == 4)
		{
			WriteToLog( _T("%s This configuration doesn't support sample*_aoffimmi"), LOG_HEADER );
			SkipMatching(GetSkipMatchingFilter() || (
				   FilterEqual(_T("Address_Offset"), true) ));
			return RESULT_SKIP;
		}
		m_sPSEntryPt += _T("_aoff");
	}

	if (m_bSampleFreqShader)
	{
		if (g_App.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_1)
		{
			WriteToLog( _T("%s This feature level doesn't support sample-frequency shaders"), LOG_HEADER );
			SkipMatching(GetSkipMatchingFilter() || (
				   FilterEqual(_T("SampleFreq_Shader"), true) ));
			return RESULT_SKIP;
		}
		m_sPSEntryPt += _T("_sampfreq");
	}

	if (m_bDefaultSampler)
		m_SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	if (m_ShaderType == ShaderType_Vertex || m_ShaderType == ShaderType_Geometry)
	{
		if (m_SampleFunction != SampleGrad && m_SampleFunction != SampleLevel && m_SampleFunction != SampleCmpLvlZero)
		{
			WriteToLog( _T("%s This configuration doesn't support Sample/SampleCmp with implicit derivatives"), LOG_HEADER );
			SkipMatching(GetSkipMatchingFilter() || 
				(  (!FilterEqual(_T("Sample_Func"), SampleGrad))
				&& (!FilterEqual(_T("Sample_Func"), SampleLevel))
				&& (!FilterEqual(_T("Sample_Func"), SampleCmpLvlZero))
				));
			return RESULT_SKIP;
		}
		m_sPixelShaderName = "ps_passthru.psh";
		if (m_ShaderType == ShaderType_Vertex)
			m_sVSEntryPt = m_sPSEntryPt;
		if (m_ShaderType == ShaderType_Geometry)
			m_sGSEntryPt = m_sPSEntryPt;
		m_sPSEntryPt = _T("main");
	}

	if (m_bMultisampleEnable)
	{
		if (m_ShaderType != ShaderType_Pixel || m_nDimension != 2)
		{
			WriteToLog( _T("%s This configuration doesn't support Multisample render target"), LOG_HEADER );
			SkipMatching(GetSkipMatchingFilter() 
				|| (FilterEqual(_T("Multisample"), true))
				);
			return RESULT_SKIP;
		}
		if (m_SampleFunction != Sample)
		{
			WriteToLog( _T("%s This configuration doesn't support Multisample render target"), LOG_HEADER );
			SkipMatching(GetSkipMatchingFilter() 
				|| (!FilterEqual(_T("Sample_Func"), Sample))
				);
			return RESULT_SKIP;
		}
		HRESULT hr = S_OK;
		UINT formatSupport;
		hr = m_pDevice->CheckFormatSupport(m_FormatRT, &formatSupport);
		if (FAILED(hr))
		{
			WriteToLog( _T("%s CheckFormatSupport(RT) failed with code %s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
			if (E_FAIL == hr)
				return RESULT_SKIP;
			else
				return RESULT_FAIL;
		}
		if (0 == (formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET))
		{
			WriteToLog( _T("%s Render target format doesn't support Multisample"), LOG_HEADER );
			SkipMatching(GetSkipMatchingFilter() 
				|| (FilterEqual(_T("Multisample"), true))
				);
			return RESULT_SKIP; // RT formats are author-time constant, but driver can choose support for MSAA
		}
		// try 4 and 2 MSAA
		for (m_nMultiSamples = 4; m_nMultiSamples > 0; m_nMultiSamples -= 2)
		{
			UINT qualityLevels = 0;
			if (FAILED(hr = m_pDevice->CheckMultisampleQualityLevels(m_FormatRT, m_nMultiSamples, &qualityLevels)))
			{
				WriteToLog( _T( "%s CheckMultisampleQualityLevels() failed for render target format=%s, samples=%d. hr=%s" ), 
					LOG_HEADER, ToString(m_FormatRT).c_str(), m_nMultiSamples, D3DHResultToString(hr).c_str() );
			}
			if (qualityLevels > 0)
				break;
		}
		if (m_nMultiSamples <= 0)
		{
			WriteToLog( _T("%s Render target format doesn't support any expected Multisample quality levels"), LOG_HEADER );
			SkipMatching(GetSkipMatchingFilter() 
				|| (FilterEqual(_T("Multisample"), true))
				);
			return RESULT_SKIP;
		}
	}
	else
		m_nMultiSamples = 1;

	bool bComparisonFilter = false;
	switch(m_SamplerDesc.Filter)
	{
	default:
		assert(!"Unexpected filter type");
		return RESULT_FAIL;
	case D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_ANISOTROPIC:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_ANISOTROPIC:
		{
			// The following tests case combinations aren't supported until the test can be restructured to better understand
			// sample weights equivalent to zero, given that sample weights are only required to be Q8.
			if (D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR == m_SamplerDesc.Filter ||
				D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR == m_SamplerDesc.Filter ||
				D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR == m_SamplerDesc.Filter ||
				D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR == m_SamplerDesc.Filter ||
				D3D11_FILTER_MINIMUM_ANISOTROPIC == m_SamplerDesc.Filter ||
				D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR == m_SamplerDesc.Filter ||
				D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR == m_SamplerDesc.Filter ||
				D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR == m_SamplerDesc.Filter ||
				D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR == m_SamplerDesc.Filter ||
				D3D11_FILTER_MAXIMUM_ANISOTROPIC == m_SamplerDesc.Filter ||
				((2 == m_nDimension || 3 == m_nDimension) &&
					(D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT == m_SamplerDesc.Filter ||
					D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT == m_SamplerDesc.Filter ||
					D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT == m_SamplerDesc.Filter ||
					D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT == m_SamplerDesc.Filter ||
					D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT == m_SamplerDesc.Filter ||
					D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT == m_SamplerDesc.Filter)))
			{
				return RESULT_SKIP;
			}

			HRESULT hr;
			D3D11_FEATURE_DATA_D3D11_OPTIONS1 d3d11Options1;

			if( g_App.GetDriverVersion() < KMT_DRIVERVERSION_WDDM_1_3 )
			{
				WriteToLog( _T("%s This test case is only relevant on WDDM 1.3+ drivers."), LOG_HEADER );
				return RESULT_SKIP;
			}

			if( FAILED( hr = m_pDevice->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS1, &d3d11Options1, sizeof(d3d11Options1) ) ) )
			{
				WriteToLog( _T( "%s CheckFeatureSupport() failed. HR = %s." ), LOG_HEADER, D3DHResultToString(hr).c_str());
				return RESULT_FAIL;
			}
			if( !d3d11Options1.MinMaxFiltering )
			{
				WriteToLog( _T("%s This device doesn't support min/max filtering."), LOG_HEADER );
				SkipMatching(GetSkipMatchingFilter() || (FilterEqual(_T("Filter_Type"), m_SamplerDesc.Filter)));
				return RESULT_SKIP;
			}
			if( (m_nDimension == 4) && // textureCube
				(m_TexCoordRotation != 0) )
			{
				// Skip cases with rotated texcoords since those are too finicky to test min/max filtering on cubes.
				return RESULT_SKIP;
			}
		}
		// falling through
	case D3D11_FILTER_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_ANISOTROPIC:
		{
			if (m_SampleFunction == SampleCmp || m_SampleFunction == SampleCmpLvlZero)
			{
				WriteToLog( _T("%s This configuration doesn't support sample_c_*"), LOG_HEADER );
				SkipMatching(GetSkipMatchingFilter() || 
					   (  (FilterEqual(_T("Sample_Func"), SampleCmp) || FilterEqual(_T("Sample_Func"), SampleCmpLvlZero))
					   && (FilterEqual(_T("Filter_Type"), m_SamplerDesc.Filter) || FilterEqual(_T("Default_Sampler"), true))
					   ));
				return RESULT_SKIP;
			}
			tstring sSampleFunc;
			bool bIsLevel9Test = g_App.GetIsFL9Test();
			switch (m_SampleFunction)
			{
			default:
				assert(m_ShaderType == ShaderType_Pixel || m_ShaderType == ShaderType_MipGen);
				sSampleFunc = bIsLevel9Test? _T("basic_L9"): _T("basic");
				break;
			case SampleBias:
				sSampleFunc = bIsLevel9Test? _T("sample_bias_L9"): _T("sample_bias");
				break;
			case SampleGrad:
				sSampleFunc = bIsLevel9Test? _T("sample_grad_L9"): _T("sample_grad");
				break;
			case SampleLevel:
				sSampleFunc = bIsLevel9Test? _T("sample_level_L9"): _T("sample_level");
				break;
			}
			switch (m_ShaderType)
			{
			case ShaderType_Vertex:
				m_sVertexShaderName = _T("vs_") + sSampleFunc + _T(".vsh");
				break;
			case ShaderType_Geometry:
				m_sGeometryShaderName = _T("gs_") + sSampleFunc + _T(".gsh");
				break;
			case ShaderType_Pixel:
			case ShaderType_MipGen:
				m_sPixelShaderName = _T("ps_") + sSampleFunc + _T(".psh");
				break;
			}
		}
		break;
	case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_ANISOTROPIC:
		{
			if (m_nDimension == 3)
			{
				WriteToLog( _T("%s This configuration doesn't support sample_c_*"), LOG_HEADER );
				SkipMatching(GetSkipMatchingFilter() || 
					(  FilterEqual(_T("Filter_Type"), m_SamplerDesc.Filter)
					|| FilterEqual(_T("Sample_Func"), SampleCmp)
					|| FilterEqual(_T("Sample_Func"), SampleCmpLvlZero)
					));
				return RESULT_SKIP;
			}
			if (m_SampleFunction != SampleCmp && m_SampleFunction != SampleCmpLvlZero)
			{
				WriteToLog( _T("%s This configuration doesn't support sample_c_*"), LOG_HEADER );
				SkipMatching(GetSkipMatchingFilter() || 
					(  FilterEqual(_T("Filter_Type"), m_SamplerDesc.Filter)
					&& (!FilterEqual(_T("Sample_Func"), SampleCmp))
					&& (!FilterEqual(_T("Sample_Func"), SampleCmpLvlZero))
					));
				return RESULT_SKIP;
			}
			if (g_App.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0)
			{
				/*
				// dx bug 106923: tex2d.SampleCmp(offset) does not compile
				if (   m_nDimension == 2
					&& m_bSampleAOffImmI 
					&& (m_SampleFunction == SampleCmp || m_SampleFunction == SampleCmpLvlZero))
				{
					WriteToLog( _T("%s Compiler bug with tex2d sample_c_aoffimmi"), LOG_HEADER );
					SkipMatching(GetSkipMatchingFilter() ||
						(  FilterEqual(_T("Address_Offset"), true) 
						&& (FilterEqual(_T("Sample_Func"), SampleCmp) || FilterEqual(_T("Sample_Func"), SampleCmpLvlZero)) ));
					return RESULT_SKIP;
				}
				*/
				/*
				// dx bug 1929604: VS & GS profiles do not compile with comparison sampler state
				if (m_ShaderType != ShaderType_Pixel)
				{
					WriteToLog( _T("%s Compiler bug with sample_c_* in VS and GS profiles"), LOG_HEADER );
					SkipMatching(GetSkipMatchingFilter() || 
						(  FilterEqual(_T("Sample_Func"), SampleCmp) 
						|| FilterEqual(_T("Sample_Func"), SampleCmpLvlZero)
						));
					return RESULT_SKIP;
				}
				*/
			}
			switch (m_ShaderType)
			{
			case ShaderType_Vertex:
				m_sVertexShaderName = _T("vs_sample_cmp.vsh");
				break;
			case ShaderType_Geometry:
				m_sGeometryShaderName = _T("gs_sample_cmp.gsh");
				break;
			case ShaderType_Pixel:
			case ShaderType_MipGen:
				m_sPixelShaderName = _T("ps_sample_cmp.psh");
				break;
			}
			bComparisonFilter = true;
		}
		break;
	}

	{
		HRESULT hr = S_OK;
		UINT formatSupport;
		hr = m_pDevice->CheckFormatSupport(m_eFormat, &formatSupport);
		if (FAILED(hr))
		{
			if (hr == E_FAIL)
				tRes = RESULT_SKIP;
			else
				tRes = RESULT_FAIL;
			WriteToLog( _T("%s CheckFormatSupport failed with code %s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
			goto setupDone;
		}

		UINT texDimensionFlag = 0;
		switch (m_nDimension)
		{
		default: assert(!"Unexpected dimension"); break;
		case 1: texDimensionFlag = D3D11_FORMAT_SUPPORT_TEXTURE1D; break;
		case 2: texDimensionFlag = D3D11_FORMAT_SUPPORT_TEXTURE2D; break;
		case 3: texDimensionFlag = D3D11_FORMAT_SUPPORT_TEXTURE3D; break;
		case 4: texDimensionFlag = D3D11_FORMAT_SUPPORT_TEXTURECUBE; break;
		}
		if (0 == (formatSupport & texDimensionFlag))
		{
			WriteToLog( _T("%s This format doesn't support textures of dimension %d"), LOG_HEADER, m_nDimension );
			tRes = RESULT_SKIP;
			SkipMatching( GetSkipMatchingFilter() || (
				FilterEqual(_T( "Format" ), m_eFormat) ));
			goto setupDone;
		}
		const tstring sTestType = GetPath().ToString( 1, 1 );
		if (sTestType == _T("Mip") || sTestType == _T("MipGen"))
		{
			if (0 == (formatSupport & D3D11_FORMAT_SUPPORT_MIP))
			{
				WriteToLog( _T("%s This format doesn't support mipmaps"), LOG_HEADER );
				tRes = RESULT_SKIP;
				SkipMatching( GetSkipMatchingFilter() || (FilterEqual(_T( "Format" ), m_eFormat) ));
				goto setupDone;
			}
		}
		if (sTestType == _T("MipGen"))
		{
			if (0 == (formatSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
			{
				WriteToLog( _T("%s This format doesn't support mipmap autogeneration"), LOG_HEADER );
				tRes = RESULT_SKIP;
				SkipMatching( GetSkipMatchingFilter() || (FilterEqual(_T( "Format" ), m_eFormat) ));
				goto setupDone;
			}
		}
		else
		{
			if (bComparisonFilter)
			{
				if (0 == (formatSupport & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON))
				{
					WriteToLog( _T("%s This format doesn't support sample_c"), LOG_HEADER );
					tRes = RESULT_SKIP;
					SkipMatching( GetSkipMatchingFilter() || (
						FilterEqual(_T( "Format" ), m_eFormat) && FilterEqual(_T("Sample_Func"), m_SampleFunction) ));
					goto setupDone;
				}
			}
			else
			{
				if (0 == (formatSupport & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE))
				{
					WriteToLog( _T("%s This format doesn't support sample"), LOG_HEADER );
					tRes = RESULT_SKIP;
					SkipMatching( GetSkipMatchingFilter() || (
						FilterEqual(_T( "Format" ), m_eFormat) && FilterEqual(_T("Sample_Func"), m_SampleFunction) ));
					goto setupDone;
				}
			}
		}
	}

	if (m_nDimension == 3)
	{
		// tex3d requires only 2048 texels in U/V/W
		m_nTextureSize[0] = min(2048, m_nTextureSize[0]);
		m_nTextureSize[1] = min(2048, m_nTextureSize[1]);
		m_nTextureSize[2] = min(2048, m_nTextureSize[2]);
	}

	switch (m_eFormat)
	{
	default:
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		break;
		// TODO BUGBUG FIXME: enable the above formats
		WriteToLog( _T("%s This format is currently not being tested"), LOG_HEADER );
		SkipMatching( GetSkipMatchingFilter() || (FilterEqual(_T( "Format" ), m_eFormat) ));
		tRes = RESULT_SKIP;
		break;
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		// x = multiple of 2
		m_nTextureSize[0] = nextMultipleOfPow2(m_nTextureSize[0], 2);
		break;
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		// 4x4 blocks per face
		m_nTextureSize[0] = nextMultipleOfPow2(m_nTextureSize[0], 4);
		m_nTextureSize[1] = nextMultipleOfPow2(m_nTextureSize[1], 4);
		break;
	case DXGI_FORMAT_R1_UNORM:
		// x = multiple of 8
		m_nTextureSize[0] = nextMultipleOfPow2(m_nTextureSize[0], 8);
		break;
	}

	if (m_nDimension == 4)
	{
		// texcube gets too big with large sizes
		m_nTextureSize[0] = min(1024, m_nTextureSize[0]);
		m_nTextureSize[1] = m_nTextureSize[0];
		m_nTextureSize[2] = 1;
	}

setupDone:
	return tRes;
}

void
CFilterConfTest::CleanupTestCase()
{
	delete[] m_pTempMultisampleData;
	m_pTempMultisampleData = NULL;
}

HRESULT
CFilterConfTest::UpdateResources()
{
	bool needsUpdate = false;
	if (m_nPrevSamples != m_nMultiSamples)
	{
		m_nPrevSamples = m_nMultiSamples;
		needsUpdate = true;
	}
	if (m_sPrevVSName != m_sVertexShaderName)
	{
		m_sPrevVSName = m_sVertexShaderName;
		needsUpdate = true;
	}
	if (m_sPrevGSName != m_sGeometryShaderName)
	{
		m_sPrevGSName = m_sGeometryShaderName;
		needsUpdate = true;
	}
	if (m_sPrevPSName != m_sPixelShaderName)
	{
		m_sPrevPSName = m_sPixelShaderName;
		needsUpdate = true;
	}
	if (m_sPrevVSEntryPt != m_sVSEntryPt)
	{
		m_sPrevVSEntryPt = m_sVSEntryPt;
		needsUpdate = true;
	}
	if (m_sPrevGSEntryPt != m_sGSEntryPt)
	{
		m_sPrevGSEntryPt = m_sGSEntryPt;
		needsUpdate = true;
	}
	if (m_sPrevPSEntryPt != m_sPSEntryPt)
	{
		m_sPrevPSEntryPt = m_sPSEntryPt;
		needsUpdate = true;
	}
	if (m_ePrevFormat != m_eFormat)
	{
		m_ePrevFormat = m_eFormat;
		needsUpdate = true;
	}

	if (needsUpdate)
	{
		CleanupDirect3D();
		if (RESULT_PASS != SetupDirect3D())
		{
			m_nPrevSamples = 0;
			m_sPrevVSName = _T("");
			m_sPrevGSName = _T("");
			m_sPrevPSName = _T("");
			m_sPrevVSEntryPt = _T("");
			m_sPrevGSEntryPt = _T("");
			m_sPrevPSEntryPt = _T("");
			m_ePrevFormat = DXGI_FORMAT_UNKNOWN;
			return E_FAIL;
		}
	}

	GetEffectiveContext()->VSSetConstantBuffers( 0, 2, m_pConstBuffers );
	if( !g_App.GetIsFL9Test() )
		GetEffectiveContext()->GSSetConstantBuffers( 0, 2, m_pConstBuffers );
	GetEffectiveContext()->PSSetConstantBuffers( 0, 2, m_pConstBuffers );

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );

	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, NULL );

	FLOAT colors[4] = { 0.1f, 0.2f, 0.3f, 0.4f };
	GetEffectiveContext()->ClearRenderTargetView( m_pRTView, colors );

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float) m_nPrimitiveW;
	vp.Height   = (float) m_nPrimitiveH;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	GetEffectiveContext()->RSSetViewports(1, &vp);

	GetEffectiveContext()->RSSetState( m_pRSEnabled );
	
	if( FAILED(ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
		return E_FAIL;
	}

	return S_OK;
}

void
CFilterConfTest::CalcMipSize(int texWHD[3], int mipLevel, int mipWHD[3], int nDimension)
{
	for (int i = 0; i < 3; ++i)
	{
		mipWHD[i] = texWHD[i] / (1<<mipLevel);
		mipWHD[i] = max(1, mipWHD[i]);
	}
	switch (nDimension)
	{
	case 1:
		mipWHD[1] = 1;
	case 2:
		mipWHD[2] = 1;
		break;
	case 3:
		break;
	case 4:
		mipWHD[1] = mipWHD[0];
		mipWHD[2] = 1;
		break;
	}
	if (nDimension == 4)
	{
	}
}

bool
CFilterConfTest::CreateTestMips(int mipLevels, int texWHD[3])
{
	assert(mipLevels < 256);
	if (!(0 < mipLevels && mipLevels < 256 ))
		return false;
	if (m_pTexData)
	{
		for (int n = 0; n < 256; ++n)
			SAFE_DELETE_ARRAY(m_pTexData[n]);
		SAFE_DELETE_ARRAY(m_pTexData);
	}
	m_pTexData = new TEST_COLOR*[256]; // max mips
	ZeroMemory(m_pTexData, 256*sizeof(TEST_COLOR*));
	for (int n = 0; n < mipLevels; ++n)
	{
		// big enough for a 3D, and a Cube
		int mipWHD[3];
		CalcMipSize(texWHD, n, mipWHD, m_nDimension);
		size_t texSize = mipWHD[0];
		if (m_nDimension > 1)
			texSize *= mipWHD[1];
		if (m_nDimension > 2)
			texSize *= mipWHD[2];
		if (m_nDimension == 4)
			texSize = mipWHD[0]*mipWHD[0]*6;
		if (texSize >= 0x08000000) // too big for 32-bit address space
			goto alloc_failure;
		try
		{
			m_pTexData[n] = new TEST_COLOR[texSize];
		}
		catch (std::bad_alloc e)
		{
			goto alloc_failure;
		}
		ZeroMemory(m_pTexData[n], texSize*sizeof(TEST_COLOR));
	}
	return true;
alloc_failure:
	for (int k = 0; k < 256; ++k)
		SAFE_DELETE_ARRAY(m_pTexData[k]);
	SAFE_DELETE_ARRAY(m_pTexData);
	return false;
}

static
void
makeD3DColor(DXGI_FORMAT format, const D3D11_MAPPED_SUBRESOURCE *pTexMap, TEST_COLOR* pColor)
{
	assert(format == DXGI_FORMAT_R32G32B32A32_FLOAT);

	pColor->r = *(((float*)pTexMap->pData) + 0);
	pColor->g = *(((float*)pTexMap->pData) + 1);
	pColor->b = *(((float*)pTexMap->pData) + 2);
	pColor->a = *(((float*)pTexMap->pData) + 3);
}

HRESULT
CFilterConfTest::ReadBackBufferColor(int x, int y, TEST_COLOR* pColor)
{
	HRESULT hr = S_OK;
	MSHelper helper;
	const UINT rtWidth = m_nPrimitiveW;
	const UINT rtHeight = m_nPrimitiveH;
	const size_t rtSizePixels = rtWidth * rtHeight;
	const size_t rtSizeFloats = rtSizePixels * 4;
	if (!m_pTempMultisampleData)
	{
		m_pTempMultisampleData = new FLOAT[rtSizeFloats * m_nMultiSamples];
		if (!helper.Read(&g_App, m_pTextureRT, m_pTempMultisampleData, m_FormatRT))
			hr = E_UNEXPECTED;
	}

	for (int n = 0; n < 4; ++n)
		pColor->f[n] = 0;
	for (int s = 0; s < m_nMultiSamples; ++s)
	{
		pColor->r += m_pTempMultisampleData[0 + (x + y*rtWidth)*4 + s*rtSizeFloats];
		pColor->g += m_pTempMultisampleData[1 + (x + y*rtWidth)*4 + s*rtSizeFloats];
		pColor->b += m_pTempMultisampleData[2 + (x + y*rtWidth)*4 + s*rtSizeFloats];
		pColor->a += m_pTempMultisampleData[3 + (x + y*rtWidth)*4 + s*rtSizeFloats];
	}
	for (int n = 0; n < 4; ++n)
		pColor->f[n] /= m_nMultiSamples;

	return hr;
}

void
CFilterConfTest::GenerateTextureCoords()
{
	memset(&m_vTexCoords, 0, sizeof(m_vTexCoords));
	if (m_bCustomTexCoords)
	{
		if (m_nDimension == 4)
		{
			if (m_nCustomTexCoordNumber >= 0 && m_nCustomTexCoordNumber < 6)
			{
				const int faceNum = m_nCustomTexCoordNumber - 0;
				assert(0 <= faceNum && faceNum < 6);
				float coeff = (float)f_quantize(pow(2.0, m_fMinMagCoeff), 16);
				float xVal=0, yVal=0, zVal=0;
				switch (faceNum/2)
				{
				case 0:
					xVal = (faceNum%2)*2-1.0f;
					m_vTexCoords[0].x = xVal;
					m_vTexCoords[0].y = +coeff;
					m_vTexCoords[0].z = +coeff;

					m_vTexCoords[1].x = xVal;
					m_vTexCoords[1].y = +coeff;
					m_vTexCoords[1].z = -coeff;

					m_vTexCoords[2].x = xVal;
					m_vTexCoords[2].y = -coeff;
					m_vTexCoords[2].z = -coeff;

					m_vTexCoords[3].x = xVal;
					m_vTexCoords[3].y = -coeff;
					m_vTexCoords[3].z = +coeff;
					break;
				case 1: 
					yVal = (faceNum%2)*2-1.0f;
					m_vTexCoords[0].x = -coeff;
					m_vTexCoords[0].y = yVal;
					m_vTexCoords[0].z = +coeff;

					m_vTexCoords[1].x = +coeff;
					m_vTexCoords[1].y = yVal;
					m_vTexCoords[1].z = +coeff;

					m_vTexCoords[2].x = +coeff;
					m_vTexCoords[2].y = yVal;
					m_vTexCoords[2].z = -coeff;

					m_vTexCoords[3].x = -coeff;
					m_vTexCoords[3].y = yVal;
					m_vTexCoords[3].z = -coeff;
					break;
				case 2: 
					zVal = (faceNum%2)*2-1.0f; 
					m_vTexCoords[0].x = -coeff;
					m_vTexCoords[0].y = +coeff;
					m_vTexCoords[0].z = zVal;

					m_vTexCoords[1].x = +coeff;
					m_vTexCoords[1].y = +coeff;
					m_vTexCoords[1].z = zVal;

					m_vTexCoords[2].x = +coeff;
					m_vTexCoords[2].y = -coeff;
					m_vTexCoords[2].z = zVal;

					m_vTexCoords[3].x = -coeff;
					m_vTexCoords[3].y = -coeff;
					m_vTexCoords[3].z = zVal;
					break;
				}
			}
			else if (m_nCustomTexCoordNumber >= 6 && m_nCustomTexCoordNumber < 14)
			{
				const int cornerNum = m_nCustomTexCoordNumber - 6;
				assert(0 <= cornerNum && cornerNum < 8);
				float xVal, yVal, zVal;
				if (cornerNum%2 == 0)
					xVal = -1;
				else
					xVal = 1;
				if (cornerNum/4 == 0)
					yVal = -1;
				else
					yVal = 1;
				if (cornerNum%4 < 2)
					zVal = -1;
				else
					zVal = 1;
				float coeff = (float)pow(2.0, m_fMinMagCoeff);
				coeff = min(1.875f, coeff);
				coeff = (float)f_quantize(coeff, 16);
				m_vTexCoords[0].x = (xVal<0)?xVal:xVal;
				m_vTexCoords[0].y = (yVal<0)?yVal+coeff:yVal;
				m_vTexCoords[0].z = (zVal<0)?zVal+coeff:zVal-coeff;
				m_vTexCoords[1].x = (xVal<0)?xVal+coeff:xVal-coeff;
				m_vTexCoords[1].y = (yVal<0)?yVal+coeff:yVal;
				m_vTexCoords[1].z = (zVal<0)?zVal:zVal;
				m_vTexCoords[2].x = (xVal<0)?xVal+coeff:xVal-coeff;
				m_vTexCoords[2].y = (yVal<0)?yVal:yVal-coeff;
				m_vTexCoords[2].z = (zVal<0)?zVal:zVal;
				m_vTexCoords[3].x = (xVal<0)?xVal:xVal;
				m_vTexCoords[3].y = (yVal<0)?yVal:yVal-coeff;
				m_vTexCoords[3].z = (zVal<0)?zVal+coeff:zVal-coeff;
			}
			else
			{
				assert(!"Unexpected custom texcoord number");
			}
		}
		else
		{
			// focus on specifc texels
			const float xRes = (float)(m_nTextureSize[0]);
			const float yRes = (float)((m_nDimension < 2)? 1 : m_nTextureSize[1]);
			const float zRes = (float)((m_nDimension < 3)? 1 : m_nTextureSize[2]);
			int desiredTexel = m_nCustomTexCoordNumber;
			float xCoord0 = (desiredTexel*0)/xRes;
			float xCoord1 = (desiredTexel*1)/xRes;
			float yCoord0 = (desiredTexel*0)/yRes;
			float yCoord1 = (desiredTexel*1)/yRes;
			float zCoord0 = (desiredTexel*0)/zRes;
			float zCoord1 = (desiredTexel*1)/zRes;
			m_vTexCoords[0].x = xCoord0;
			m_vTexCoords[0].y = yCoord0;
			m_vTexCoords[0].z = zCoord1;
			m_vTexCoords[1].x = xCoord1;
			m_vTexCoords[1].y = yCoord0;
			m_vTexCoords[1].z = zCoord1;
			m_vTexCoords[2].x = xCoord1;
			m_vTexCoords[2].y = yCoord1;
			m_vTexCoords[2].z = zCoord0;
			m_vTexCoords[3].x = xCoord0;
			m_vTexCoords[3].y = yCoord1;
			m_vTexCoords[3].z = zCoord0;
		}
	}
	else
	{
		// intent here is to have minmagcoeff == 0 give an exact (1:1) pixel:texel mapping
		const float baseCoordX = (m_nDimension==4)?1:((float)m_nPrimitiveW) / m_nTextureSize[0];
		const float baseCoordY = (m_nDimension==4)?1:((float)m_nPrimitiveH) / m_nTextureSize[1];
		// tweak texture coordinates using test parameters
		m_vTexCoords[0].z = 1;
		m_vTexCoords[1].x = baseCoordX;
		m_vTexCoords[1].z = 1;
		m_vTexCoords[2].x = baseCoordX;
		m_vTexCoords[2].y = baseCoordY;
		m_vTexCoords[3].y = baseCoordY;
		float coeff = (float)pow(2.0, m_fMinMagCoeff);
		coeff = (float)f_quantize(coeff, 16);
		for (int n = 0; n < 4; ++n)
		{
			// offset so origin is int center of render target
			m_vTexCoords[n].x -= baseCoordX/2;
			m_vTexCoords[n].y -= baseCoordY/2;
			m_vTexCoords[n] *= coeff;
		}
		tstring sPath = GetPath().ToString(GetPath().Depth() - 2, 1);
		//if (sPath == "Point")
		{
			for (int n = 0; n < 4; ++n)
			{
				(DWORD&)(m_vTexCoords[n].x) += 30;
				(DWORD&)(m_vTexCoords[n].y) += 30;
			}
		}
		if (m_nDimension == 4)
			for (int n = 0; n < 4; ++n)
			{
				m_vTexCoords[n].z = 1;
			}
	}

	if (m_TexCoordRotation != 0)
	{
		//A point <x,y> can be rotated around the origin <0,0> by running it through the following equations to get the new point <x',y'> :
		//x' = cos(theta)*x - sin(theta)*y 
		//y' = sin(theta)*x + cos(theta)*y
		const double Pi = 3.14159265358979323846264338327950288419716939937510;
		const double radianAngle = (m_TexCoordRotation/180.0f)*Pi;
		for (int n = 0; n < 4; ++n)
		{
			double xNew = cos(radianAngle)*m_vTexCoords[n].x - sin(radianAngle)*m_vTexCoords[n].y;
			double yNew = sin(radianAngle)*m_vTexCoords[n].x + cos(radianAngle)*m_vTexCoords[n].y;
			m_vTexCoords[n].x = (float)f_quantize(xNew, 16);
			m_vTexCoords[n].y = (float)f_quantize(yNew, 16);
		}
	}
}




void
CFilterConfTest::DoVisibleRender()
{
	HRESULT hr = E_FAIL;

	// texture coords
	TEST_VEC4F texCoords[4] = {
		{0,0,0,0},
		{1,0,0,0},
		{1,1,0,0},
		{0,1,0,0},
	};
	if (!m_pConstBuffers[1])
		return;
	float* pConstData = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	GetImmediateContext()->Map( m_pConstBuffers[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
	memcpy(mappedRes.pData, texCoords, 4*4*sizeof(float));
	GetImmediateContext()->Unmap(m_pConstBuffers[1],0);

	// create and set viewport
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float) (m_nPrimitiveW*2);
	vp.Height   = (float) m_nPrimitiveH;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	GetEffectiveContext()->RSSetViewports(1, &vp);
	RECT presentRect = {0,0,m_nPrimitiveW*2,m_nPrimitiveH};

	// set render target
	CD3D11TestFramework* pFW = GetFramework();
	ID3D11RenderTargetView* pRTView = pFW->GetRTView();
	GetEffectiveContext()->OMSetRenderTargets(1, &pRTView, NULL);

	// set shader
	GetEffectiveContext()->VSSetShader( m_pVSDefault, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPSVisibleCopy, NULL, 0 );

	// sampler
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.MaxLOD = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MipLODBias = 0;
	ID3D11SamplerState* pSampler;
	hr = m_pDevice->CreateSamplerState(&sampDesc, &pSampler);
	if (FAILED(hr))
		return;
	GetEffectiveContext()->PSSetSamplers(0, 1, &pSampler);

	// shader resource view
	GetEffectiveContext()->PSSetShaderResources(0, 1, &m_pRTShaderView);

	// draw
	UINT stride = sizeof(TVERTEX);
	UINT offset = 0;
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	GetEffectiveContext()->IASetVertexBuffers(0, 1, &m_pVertexBufferQuad, &stride, &offset);
	GetEffectiveContext()->IASetIndexBuffer(m_pIndexBufferQuad, DXGI_FORMAT_R16_UINT, 0);
	GetEffectiveContext()->IASetInputLayout(m_pLayoutDefault);
	GetEffectiveContext()->DrawIndexed(m_nIndexCount, 0, 0);

	if( FAILED(hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
	}

	// present
	bool stretch = g_App.Stretch();
	g_App.Present(&presentRect, stretch?NULL:&presentRect, NULL, NULL, 0, 0);

	// clean up
	ID3D11Buffer* nullBuffArr[1] = {NULL};
	GetEffectiveContext()->IASetVertexBuffers(0, 1, nullBuffArr, &stride, &offset);
	GetEffectiveContext()->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	ID3D11SamplerState* nullSampArr[1] = {NULL};
	GetEffectiveContext()->PSSetSamplers( 0, 1, nullSampArr);
	ID3D11ShaderResourceView* nullSRVArr[1] = {NULL};
	GetEffectiveContext()->PSSetShaderResources(0, 1, nullSRVArr);

	if( FAILED(hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
	}

	SAFE_RELEASE(pSampler);
}

void
CFilterConfTest::DoShowResources()
{
	ShowResource(L"Render Target", m_pTextureRT);
}

HRESULT
CFilterConfTest::DrawQuad()
{
	HRESULT hr = S_OK;

	if (!m_pConstBuffers[1])
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE mappedRes;
	float* pConstData = NULL;
	GetImmediateContext()->Map( m_pConstBuffers[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );

	memcpy(mappedRes.pData, m_vTexCoords, 4*4*sizeof(float));

	GetImmediateContext()->Unmap(m_pConstBuffers[1],0);

	UINT stride = sizeof(TVERTEX);
	UINT offset = 0;
	ID3D11SamplerState* pSamplerState = NULL;
	ID3D11SamplerState* nullSampArr[1] = {NULL};
	hr = m_pDevice->CreateSamplerState(&m_SamplerDesc, &pSamplerState);
	if (FAILED(hr))
	{
		WriteToLog( _T("%s CreateSamplerState failed. hr=%s\n"), LOG_HEADER, D3DHResultToString(hr).c_str() );
	}
	else
	{
		if (m_bDefaultSampler)
		{
			if( !g_App.GetIsFL9Test() )
			{
				GetEffectiveContext()->VSSetSamplers( 0, 1, nullSampArr);
				GetEffectiveContext()->GSSetSamplers( 0, 1, nullSampArr);
			}
			GetEffectiveContext()->PSSetSamplers( 0, 1, nullSampArr);
		}
		else
		{
			if( !g_App.GetIsFL9Test() )
			{
				GetEffectiveContext()->VSSetSamplers( 0, 1, &pSamplerState);
				GetEffectiveContext()->GSSetSamplers( 0, 1, &pSamplerState);
			}
			GetEffectiveContext()->PSSetSamplers( 0, 1, &pSamplerState);
		}

		GetEffectiveContext()->IASetInputLayout(m_pLayout);
		if (m_ShaderType == ShaderType_Pixel || m_ShaderType == ShaderType_MipGen)
		{
			GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
			GetEffectiveContext()->IASetVertexBuffers(0, 1, &m_pVertexBufferQuad, &stride, &offset);
			GetEffectiveContext()->IASetIndexBuffer(m_pIndexBufferQuad, DXGI_FORMAT_R16_UINT, 0);
			GetEffectiveContext()->DrawIndexed(m_nIndexCount, 0, 0);
		}
		else
		{
			GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
			GetEffectiveContext()->IASetVertexBuffers(0, 1, &m_pVertexBufferPoints, &stride, &offset);
			GetEffectiveContext()->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
			GetEffectiveContext()->Draw(m_nPrimitiveH*m_nPrimitiveW, 0);
		}
	}

	ID3D11Buffer* nullBuffArr[1] = {NULL};
	GetEffectiveContext()->IASetVertexBuffers(0, 1, nullBuffArr, &stride, &offset);
	GetEffectiveContext()->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);

	if( !g_App.GetIsFL9Test() )
	{
		GetEffectiveContext()->VSSetSamplers( 0, 1, nullSampArr);
		GetEffectiveContext()->GSSetSamplers( 0, 1, nullSampArr);
	}
	GetEffectiveContext()->PSSetSamplers( 0, 1, nullSampArr);

	if( FAILED(hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
	}

	SAFE_RELEASE(pSamplerState);

	return hr;
}




// some utility functions
TEST_VEC4F cross3f(const TEST_VEC4F& A, const TEST_VEC4F& B)
{
	TEST_VEC4F C;
	C.x = A.y*B.z - A.z*B.y;
	C.y = A.z*B.x - A.x*B.z;
	C.z = A.x*B.y - A.y*B.x;
	C.w = 0;
	return C;
}
float dot4f(const TEST_VEC4F& A, const TEST_VEC4F& B)
{
	return A.x*B.x + A.y*B.y + A.z*B.z + A.w*B.w;
}
void operator*=(TEST_VEC4F& lhs, float rhs)
{
	for (int n = 0; n < 4; ++n)
		lhs.f[n] *= rhs;
}
void operator*=(TEST_VEC4F& lhs, const TEST_VEC4F& rhs)
{
	for (int n = 0; n < 4; ++n)
		lhs.f[n] *= rhs.f[n];
}
TEST_VEC4F operator*(const TEST_VEC4F& lhs, float rhs)
{
	TEST_VEC4F retVec = lhs;
	retVec *= rhs;
	return retVec;
}
TEST_VEC4F operator*(const TEST_VEC4F& lhs, const TEST_VEC4F& rhs)
{
	TEST_VEC4F retVec = lhs;
	retVec *= rhs;
	return retVec;
}
TEST_VEC4F operator-(const TEST_VEC4F& lhs, const TEST_VEC4F& rhs)
{
	TEST_VEC4F retVec = lhs;
	for (int n = 0; n < 4; ++n)
		retVec.f[n] -= rhs.f[n];
	return retVec;
}
TEST_VEC4F operator+(const TEST_VEC4F& lhs, const TEST_VEC4F& rhs)
{
	TEST_VEC4F retVec = lhs;
	for (int n = 0; n < 4; ++n)
		retVec.f[n] += rhs.f[n];
	return retVec;
}


BEGIN_NAMED_VALUES( CFilterConfTest::eShaderType )
NAMED_VALUE( _T( "Vertex" ), CFilterConfTest::ShaderType_Vertex )
NAMED_VALUE( _T( "Geometry" ), CFilterConfTest::ShaderType_Geometry )
NAMED_VALUE( _T( "Pixel" ), CFilterConfTest::ShaderType_Pixel )
NAMED_VALUE( _T( "MipGen" ), CFilterConfTest::ShaderType_MipGen )
END_NAMED_VALUES( CFilterConfTest::eShaderType )

BEGIN_NAMED_VALUES( CFilterConfTest::eSampleFunction )
NAMED_VALUE( _T( "Sample" ), CFilterConfTest::Sample )
//NAMED_VALUE( _T( "SampleAOff" ), CFilterConfTest::SampleAOff )
NAMED_VALUE( _T( "SampleBias" ), CFilterConfTest::SampleBias )
//NAMED_VALUE( _T( "SampleBiasAOff" ), CFilterConfTest::SampleBiasAOff )
NAMED_VALUE( _T( "SampleLevel" ), CFilterConfTest::SampleLevel )
//NAMED_VALUE( _T( "SampleLevelAOff" ), CFilterConfTest::SampleLevelAOff )
NAMED_VALUE( _T( "SampleGrad" ), CFilterConfTest::SampleGrad )
//NAMED_VALUE( _T( "SampleGradAOff" ), CFilterConfTest::SampleGradAOff )
NAMED_VALUE( _T( "SampleCmp" ), CFilterConfTest::SampleCmp )
//NAMED_VALUE( _T( "SampleCmpAOff" ), CFilterConfTest::SampleCmpAOff )
NAMED_VALUE( _T( "SampleCmpLvlZero" ), CFilterConfTest::SampleCmpLvlZero )
//NAMED_VALUE( _T( "SampleCmpLvlZeroAOff" ), CFilterConfTest::SampleCmpLvlZeroAOff )
END_NAMED_VALUES( CFilterConfTest::eSampleFunction )


void
CFilterConfApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();
	UnRegisterOptionVariable("srconly");

	m_D3DOptions.SrcOnly = true;
	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;

	if (!GetShell()->IsConsole())
	{
		RegisterOptionVariable("Present", "Show render results in visible window", &m_bVisibleRender, false);
		RegisterOptionVariable("Stretch", "Stretch on present", &m_bStretch, true);
	}

	RegisterOptionVariable("Width", "Render target width", &m_Width, 16, OPTION_VALUE_CONSTANT);
	RegisterOptionVariable("Height", "Render target height", &m_Height, 16, OPTION_VALUE_CONSTANT);

	RegisterOptionVariable("AllPixels", "Log all pixel failures", &m_bLogAllPixelFailures, false);
}

bool
CFilterConfApp::InitTestGroups()
{
	if (GetFramework()->GetTestMode() == WGFTEST_MODE_WHQL)
	{
		m_bVisibleRender = false;
		m_Width = 16;
		m_Height = 16;
	}

	m_bFL9Test = false;
	TEST_RESULT tr = QueryDeviceProperties();
	if( tr != RESULT_PASS )
	{
		WriteToLogF( "CFilterConfApp::InitTestGroups(): QueryDeviceProperties failed.");
		return false;
	}

	// Register test classes with the group manager.
	// Only run the FL9 test cases on eligible devices
	if( m_kmtDriverVersion >= KMT_DRIVERVERSION_WDDM_1_3 && 
		( m_MaxHardwareFL < D3D_FEATURE_LEVEL_10_0 || ( m_bSupportL9PerfFeatures && m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0) ) )
	{
		m_bFL9Test = true;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Point\\2D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Point\\3D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Point\\Cube" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Linear\\2D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Linear\\3D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Linear\\Cube" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Mixed\\2D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Mixed\\3D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Mixed\\Cube" ), CBasicFilterTest ) ) return false;

		if( !ADD_TEST( _T( "Filter\\Mip\\Point\\2D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\Point\\3D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\Point\\Cube" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\Linear\\2D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\Linear\\3D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\Linear\\Cube" ), CMipFilterTest ) ) return false;
	}
	else if( m_MaxHardwareFL >= D3D_FEATURE_LEVEL_10_0 && m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_0 ) 
	{
		if( !ADD_TEST( _T( "Filter\\NoMip\\Point\\1D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Point\\2D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Point\\3D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Point\\Cube" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Linear\\1D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Linear\\2D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Linear\\3D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Linear\\Cube" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Mixed\\1D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Mixed\\2D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Mixed\\3D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Mixed\\Cube" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Depth\\1D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Depth\\2D" ), CBasicFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\NoMip\\Depth\\Cube" ), CBasicFilterTest ) ) return false;

		if( !ADD_TEST( _T( "Filter\\Mip\\Point\\1D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\Point\\2D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\Point\\3D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\Point\\Cube" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\Linear\\1D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\Linear\\2D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\Linear\\3D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\Linear\\Cube" ), CMipFilterTest ) ) return false;

		if( !ADD_TEST( _T( "Filter\\Mip\\VS\\1D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\VS\\2D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\VS\\3D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\VS\\Cube" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\GS\\1D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\GS\\2D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\GS\\3D" ), CMipFilterTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\Mip\\GS\\Cube" ), CMipFilterTest ) ) return false;

		if( !ADD_TEST( _T( "Filter\\Aniso\\2D" ), CAnisoFilterTest ) ) return false;
		// TODO: enable cube and 3D aniso tests if required in spec
		//if( !ADD_TEST( _T( "Filter\\Aniso\\3D" ), CMipFilterTest ) ) return false;
		//if( !ADD_TEST( _T( "Filter\\Aniso\\Cube" ), CMipFilterTest ) ) return false;

		if( !ADD_TEST( _T( "Filter\\MipGen\\1D" ), CMipGenTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\MipGen\\2D" ), CMipGenTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\MipGen\\3D" ), CMipGenTest ) ) return false;
		if( !ADD_TEST( _T( "Filter\\MipGen\\Cube" ), CMipGenTest ) ) return false;

		if( !ADD_TEST( _T( "Sampler\\States" ), ID3D11SamplerStateTest ) ) return false;

		if (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
		{
			if( !ADD_TEST( _T( "Filter\\MinLODScenario" ), CResourceMinLODScenario ) ) return false;
		}
	}
	else
	{
		if( !ADD_TEST( _T("NoGroups"), CDummyTest ) ) return false;
		WriteToLogF( "CFilterConfApp::InitTestGroups(): No eligible test groups for this hardware and driver." );
		return true;
	}

	return true;
}

TEST_RESULT CFilterConfApp::QueryDeviceProperties()
{
	ID3D11Device *pTempDevice = NULL;
	ID3D11Device *pTempDeviceL9 = NULL;
	ID3D11DeviceTest *pDevTest = NULL;
	IDXGIDevice* pDXGI = NULL;
	IDXGIAdapter* pAdapter = NULL;
	IDXGIAdapter1* pAdapter1 = NULL;
	DevX::GRFX::AdapterDescriptorCore* pAdapterDescriptor = NULL;

	TEST_RESULT tr = RESULT_PASS;

	// initializing the device property members
	m_MaxHardwareFL = D3D_FEATURE_LEVEL_10_0;
	m_bSupportL9PerfFeatures = false;
	m_kmtDriverVersion = KMT_DRIVERVERSION_WDDM_1_2;

	D3D_FEATURE_LEVEL featureLevels[] = 
			{
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3,
				D3D_FEATURE_LEVEL_9_2,
				D3D_FEATURE_LEVEL_9_1
			};

	bool bSupportsFL10 = true;
	HRESULT hr = D3D11CreateDevice_11or12(
		m_D3DOptions.Force11On12 ? UseD3D11On12_HighNodeMask : UseD3D11,
		NULL, 
		m_D3DOptions.DriverType,
		NULL,
		0,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&pTempDevice,
		NULL,
		NULL,
		NULL
		);
			
	if ( hr != S_OK)
	{
		WriteToLogF( "%s in %s at line %d: D3D11CreateDevice failed. hr = %s", __FUNCTION__, __FILE__, __LINE__ ,
			D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Exit;
	}
	// Get the Max HW feature level of the device
	m_MaxHardwareFL = pTempDevice->GetFeatureLevel();

	if( m_D3DOptions.DriverType == D3D_DRIVER_TYPE_REFERENCE )
	{
		// Ref doesn't support the GetDisplayDriverModel() call
		m_bSupportL9PerfFeatures = false;
		m_kmtDriverVersion = KMT_DRIVERVERSION_WDDM_1_2;
		tr = RESULT_PASS;
		goto Exit;
	}

	// Get the WDDM version
	hr = pTempDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)(&pDXGI));
	if (FAILED(hr) || pDXGI == NULL)
	{
		WriteToLogF( "%s in %s at line %d: QueryInterface failed for IDXGIDevice. hr = %s", __FUNCTION__, __FILE__, __LINE__ ,
			D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Exit;
	}
	hr = pDXGI->GetAdapter(&pAdapter);
	if (FAILED(hr) || pAdapter == NULL)
	{
		WriteToLogF( "%s in %s at line %d: GetAdapter failed. hr = %s", __FUNCTION__, __FILE__, __LINE__ ,
			D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Exit;
	}
	hr = pAdapter->QueryInterface( __uuidof(IDXGIAdapter1), (void**)&pAdapter1 );
	if( FAILED( hr ) || pAdapter1 == NULL)
	{
		WriteToLogF( "%s in %s at line %d: QueryInterface failed for IDXGIAdapter1. hr = %s", __FUNCTION__, __FILE__, __LINE__ ,
			D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Exit;
	}

	pAdapterDescriptor = DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor( pAdapter1 );
	if( !pAdapterDescriptor )
	{
		WriteToLogF( "%s in %s at line %d: DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor failed.",  
			__FUNCTION__, __FILE__, __LINE__ );
		tr = RESULT_FAIL;
		goto Exit;
	}

	hr = pAdapterDescriptor->GetDisplayDriverModel( m_kmtDriverVersion );
	if( FAILED( hr ) )
	{
		WriteToLogF( "%s in %s at line %d: GetDisplayDriverModel() failed. hr = %s", __FUNCTION__, __FILE__, __LINE__ ,
			D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Exit;
	}

	// Check SupportsLevel9PerfFeatures
	if ( m_kmtDriverVersion >= KMT_DRIVERVERSION_WDDM_1_3 )
	{
		// Create a FL9 device for SupportsLevel9PerfFeatures() query
		D3D_FEATURE_LEVEL featureLevels9[] = 
				{
					D3D_FEATURE_LEVEL_9_3,
					D3D_FEATURE_LEVEL_9_2,
					D3D_FEATURE_LEVEL_9_1
				};

		hr = D3D11CreateDevice_11or12(
						m_D3DOptions.Force11On12 ? UseD3D11On12_HighNodeMask : UseD3D11,
						NULL, 
						m_D3DOptions.DriverType,
						NULL,
						0,
						featureLevels9,
						ARRAYSIZE(featureLevels9),
						D3D11_SDK_VERSION,
						&pTempDeviceL9,
						NULL,
						NULL,
						NULL);
	
		if ( hr != S_OK)
		{
			WriteToLogF( "%s in %s at line %d: D3D11CreateDevice failed for FL9 device. hr = %s", __FUNCTION__, __FILE__, __LINE__ ,
				D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}

		hr = pTempDeviceL9->QueryInterface(__uuidof( ID3D11DeviceTest ), (void**)&pDevTest );
		if (FAILED(hr) || pDevTest == NULL)
		{
			WriteToLogF( "%s in %s at line %d: QI for ID3D11DeviceTest failed. hr = %s", __FUNCTION__, __FILE__, __LINE__ ,
				D3DHResultToString(hr).c_str());
			tr = RESULT_FAIL;
			goto Exit;
		}

		m_bSupportL9PerfFeatures = ( FALSE != pDevTest->SupportsLevel9PerfFeatures() );
	}

Exit:
	SAFE_RELEASE( pTempDevice );
	SAFE_RELEASE( pTempDeviceL9 );
	SAFE_RELEASE( pDevTest );
	SAFE_RELEASE(pDXGI);
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pAdapter1);
	if( pAdapterDescriptor )
	{
		delete pAdapterDescriptor;
		pAdapterDescriptor = NULL;
	}

	return tr;

}
