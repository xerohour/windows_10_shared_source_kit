#include <cmath>
#include "AsyncConf.h" 
#include "InfoQueueHelper.h"

CAsyncConfApp  g_App;

// TODO: enable the over-allocation test
const bool TestOverAlloc = true;

///////////////////////////////////////////////////////////////////////////////
//	Asynchronous Conformance testing, based off Rasterization API tests
///////////////////////////////////////////////////////////////////////////////

#define ZeroStructMem(x) ZeroMemory(&x, sizeof(x))
#define HRSTRCASE(x) case (x): return (L#x)

static
const wchar_t*
DeviceRemovedReasonString(HRESULT hrReason)
{
	switch (hrReason)
	{
	HRSTRCASE(S_OK);
	HRSTRCASE(DXGI_ERROR_DEVICE_REMOVED);
	HRSTRCASE(DXGI_ERROR_DEVICE_HUNG);
	HRSTRCASE(DXGI_ERROR_DEVICE_RESET);
	HRSTRCASE(DXGI_ERROR_INVALID_CALL);
	HRSTRCASE(DXGI_ERROR_DRIVER_INTERNAL_ERROR);
	default:
		return L"unrecognized HRESULT value";
	}
}

TEST_RESULT
CAsyncConfTest::SetupDirect3D( )
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;
	ID3D11Device *pNewDevice = NULL;
	ID3D11Device2 *pD3DDevice2 = NULL;
	ID3D11DeviceTest *pDevTest = NULL;
	IDXGIAdapter1* pAdapter1 = nullptr;
	DevX::GRFX::AdapterDescriptorCore* pAdapterDescriptor = nullptr;
	D3DKMT_DRIVERVERSION kmtDriverVersion = KMT_DRIVERVERSION_WDDM_1_1;
	bool supportsFL10;

	// these setups aren't needed (and aren't supported) for our FL9 tests (which only covers counters and certain query tests)
	// if we are running FL9, then further evaluation is needed to determine what to run
	if (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_0) 
	{
		if( (tRes = SetupRenderTarget( false )) != RESULT_PASS )
			goto EndSetup;

		if( (tRes = SetupShaders( false )) != RESULT_PASS )
			goto EndSetup;

		if( (tRes = SetupGeometry( false )) != RESULT_PASS )
			goto EndSetup;
	}
	else
	{
		// make sure we are on a WDDM 1.3 9-only device, or a 10+ device that supports level9 perf improvements, otherwise skip
		
		// check if device supports FL10+
		D3D_FEATURE_LEVEL featureLevels[] = 
				{
					D3D_FEATURE_LEVEL_11_1,
					D3D_FEATURE_LEVEL_11_0,
					D3D_FEATURE_LEVEL_10_1,
					D3D_FEATURE_LEVEL_10_0
				};

		hr = D3D11CreateDevice(
						NULL, 
						g_App.m_D3DOptions.DriverType,
						NULL,
						GetDevice()->GetCreationFlags(),
						featureLevels,
						ARRAYSIZE(featureLevels),
						D3D11_SDK_VERSION,
						&pNewDevice,
						NULL,
						NULL );

		if (SUCCEEDED(hr))
		{
			supportsFL10 = true;
		}
		else
		{
			supportsFL10 = false;
		}

		// Check driver model
		HRESULT hr = GetFramework()->GetDXGIAdapter()->QueryInterface( __uuidof(IDXGIAdapter1), (void**)&pAdapter1 );
		if( FAILED( hr ) || pAdapter1 == nullptr)
		{
			WriteToLog( "CAsyncConfTest::SetupDirect3D() - QueryInterface failed for IDXGIAdapter1. hr = %s", D3DHResultToString(hr).c_str());
			tRes = RESULT_FAIL;
			goto EndSetup;
		}

		pAdapterDescriptor = DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor( pAdapter1 );
		if (!pAdapterDescriptor)
		{
			WriteToLog("CAsyncConfTest::SetupDirect3D() - DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor failed.");
			tRes = RESULT_FAIL;
			goto EndSetup;
		}

		pAdapterDescriptor->GetDisplayDriverModel( kmtDriverVersion );

		// maintain backwards compatibility, gracefully fail (by skipping the test group)
		if(SUCCEEDED(GetDevice()->QueryInterface(__uuidof( ID3D11Device2 ), (void **)&pD3DDevice2)) && SUCCEEDED(GetDevice()->QueryInterface(__uuidof( ID3D11DeviceTest ), (void**)&pDevTest)))
		{	
			if (kmtDriverVersion < KMT_DRIVERVERSION_WDDM_1_3 || (pDevTest->SupportsLevel9PerfFeatures() == FALSE && supportsFL10 == true))
			{
				// we are on a pre-WDDM 1.3 driver
				// or we are on a 10+ driver that doesn't support the optional perf features
				WriteToLog( _T( "This is either a WDDM 1.2 or older driver, or this driver has chosen to not support this optional FL9 feature. Skipping test group." ) );
				tRes = RESULT_SKIP;
				goto EndSetup;
			}
		}
		else
		{
			WriteToLog( _T( "This is not supported on this OS version. Skipping test group." ) );
			tRes = RESULT_SKIP;
			goto EndSetup;
		}

		if( (tRes = SetupRenderTarget( true )) != RESULT_PASS )
			goto EndSetup;

		if( (tRes = SetupShaders( true )) != RESULT_PASS )
			goto EndSetup;

		if( (tRes = SetupGeometry( true )) != RESULT_PASS )
			goto EndSetup;

	}

	// create and set viewport
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float) m_nPrimitiveSize;
	vp.Height   = (float) m_nPrimitiveSize;
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

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = true;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.StencilEnable = false;
	GetDevice()->CreateDepthStencilState( &dsDesc, &m_pDSSEnabled );

	GetEffectiveContext()->RSSetState( m_pRSEnabled );
	GetEffectiveContext()->OMSetDepthStencilState( m_pDSSEnabled, 0 );

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
		WriteToLog( _T("CAsyncConfTest::SetupD3D() - CreateSampler() failed. hr = 0x%x"), hr );
		tRes = RESULT_FAIL;
		goto EndSetup;
	}

	GetDevice()->VSSetSamplers(0, 1, &m_pSampler);
	GetDevice()->GSSetSamplers(0, 1, &m_pSampler);
	GetDevice()->PSSetSamplers(0, 1, &m_pSampler);
	*/

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		tRes = RESULT_FAIL;
		goto EndSetup;
	}

EndSetup:
	SAFE_RELEASE(pAdapter1);
	SAFE_RELEASE(pNewDevice);
	SAFE_RELEASE(pDevTest);
	SAFE_RELEASE(pD3DDevice2);

	return tRes;	
}

//---------------------------------------------------------------

TEST_RESULT
CAsyncConfTest::SetupGeometry( bool bIsLevel9 )
{
	HRESULT hr;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA resUP;

	m_bGpuJobSetup = false;

	// create vertex buffer
	TVERTEX vertices[] = 
	{
		{ {0,							0,							0}, 0xFFFFFFFF },
		{ {(float)m_nPrimitiveSize,	    0,							0}, 0xFFFFFFFF },
		{ {(float)m_nPrimitiveSize,	    (float)m_nPrimitiveSize,	0}, 0xFFFFFFFF },
		{ {0,							(float)m_nPrimitiveSize,	0}, 0xFFFFFFFF },
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
	{	0,1,2, 2,3,0, // 1
		0,1,2, 2,3,0, // 2
		0,1,2, 2,3,0, // 3
		0,1,2, 2,3,0, // 4
		0,1,2, 2,3,0, // 5
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0, // 10
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0, // 15
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0, // 20
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0, // 25
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0, // 30
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0, // 35
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0, // 40
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0, // 45
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0,
		0,1,2, 2,3,0, // 50
	};
	m_nIndexCount = sizeof(indices)/sizeof(*indices);
	if (GetFramework()->m_D3DOptions.DriverType == D3D_DRIVER_TYPE_REFERENCE)
		m_nIndexCount = 12; // 2 quads
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

	if (!bIsLevel9)
	{
		D3D11_BUFFER_DESC soBufDesc;
		soBufDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
		soBufDesc.ByteWidth = (m_nIndexCount * sizeof(SOVERTEX));
		soBufDesc.CPUAccessFlags = 0;
		soBufDesc.MiscFlags = 0;
		soBufDesc.Usage = D3D11_USAGE_DEFAULT;

		hr = GetDevice()->CreateBuffer(&soBufDesc, NULL, &m_pSOBuffer);
		if (FAILED(hr))
			return RESULT_FAIL;
	}

	return RESULT_PASS;
}

//---------------------------------------------------------------

TEST_RESULT
CAsyncConfTest::SetupRenderTarget( bool bIsLevel9 )
{
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC tex2ddesc;
	tex2ddesc.ArraySize = 1;
	tex2ddesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	if (bIsLevel9)
	{
		tex2ddesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	else
	{
		tex2ddesc.Format = m_FormatRT;
	}
	tex2ddesc.Height = m_nPrimitiveSize;
	tex2ddesc.Width = m_nPrimitiveSize;
	tex2ddesc.CPUAccessFlags = 0;
	tex2ddesc.MipLevels = 1;
	tex2ddesc.MiscFlags = 0;
	tex2ddesc.SampleDesc.Count = 1;
	tex2ddesc.SampleDesc.Quality = 0;
	tex2ddesc.Usage = D3D11_USAGE_DEFAULT;

	hr = GetDevice()->CreateTexture2D( &tex2ddesc, NULL, &m_pTextureRT );
	if (FAILED(hr))
		return RESULT_FAIL;

	if (bIsLevel9)
	{
		tex2ddesc.Format = DXGI_FORMAT_D16_UNORM;
	}
	else
	{
		tex2ddesc.Format = DXGI_FORMAT_D32_FLOAT;
	}
	tex2ddesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = GetDevice()->CreateTexture2D( &tex2ddesc, NULL, &m_pTextureDepth );
	if (FAILED(hr))
		return RESULT_FAIL;

	tex2ddesc.BindFlags = 0;
	tex2ddesc.Usage = D3D11_USAGE_STAGING;
	tex2ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	tex2ddesc.Width = 1;
	tex2ddesc.Height = 1;
	if (bIsLevel9)
	{
		tex2ddesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	else
	{
		tex2ddesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	}

	hr = GetDevice()->CreateTexture2D( &tex2ddesc, NULL, &m_pTextureRTCopy );
	if (FAILED(hr))
		return RESULT_FAIL;

	/*
	tex2ddesc.BindFlags = 0;
	tex2ddesc.Usage = D3D11_USAGE_STAGING;
	tex2ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	GetDevice()->CreateTexture2D( &tex2ddesc, NULL, &m_pTextureRTCopy );
	*/

	D3D11_RENDER_TARGET_VIEW_DESC rtviewdesc;
	rtviewdesc.Texture2D.MipSlice = 0;
	if (bIsLevel9)
	{
		rtviewdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	else
	{
		rtviewdesc.Format = m_FormatRT;
	}
	rtviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = GetDevice()->CreateRenderTargetView( m_pTextureRT, &rtviewdesc, &m_pRTView );
	if (FAILED(hr))
		return RESULT_FAIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsviewdesc;
	if (bIsLevel9)
	{
		dsviewdesc.Format = DXGI_FORMAT_D16_UNORM;
	}
	else
	{
		dsviewdesc.Format = DXGI_FORMAT_D32_FLOAT;
	}
	dsviewdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsviewdesc.Texture2D.MipSlice = 0;
	dsviewdesc.Flags = 0;
	hr = GetDevice()->CreateDepthStencilView( m_pTextureDepth, &dsviewdesc, &m_pDSView );
	if (FAILED(hr))
		return RESULT_FAIL;

	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, m_pDSView );

	FLOAT colors[4] = { 0.1f, 0.2f, 0.3f, 0.4f };
	GetEffectiveContext()->ClearRenderTargetView( m_pRTView, colors );

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

//---------------------------------------------------------------

TEST_RESULT
CAsyncConfTest::SetupShaders( bool bIsLevel9 )
{
	TEST_RESULT tRes = RESULT_PASS;
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	HRESULT hr;

	D3D11_SO_DECLARATION_ENTRY declSO[] = 
	{
		{ 0, "SV_POSITION", 0, 0, 4, 0 }	// position
		//	{0, "tex", 0, 0, 2, 0 },         	//texture coordinates
	};

	UINT strideSO = sizeof(SOVERTEX);

	if( FAILED(hr = GetFramework()->CompileShaderFromResource(NULL, _T("async.sh"), "PSMain", D3D_SHADER_STAGE_PIXEL, D3D_FEATURE_LEVEL_9_1, D3D_FEATURE_LEVEL_11_1, D3D10_SHADER_DEBUG, &pShaderBuf, &pErrorBuf)) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "CAsyncConfTest::SetupShaders - CompileShaderFromResource() failed compiling PS: %s\n" ), D3DHResultToString(hr).c_str() );
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
		goto shader_done;
	}
	
	if( FAILED( hr = GetDevice()->CreatePixelShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, &m_pPS ) ) )
	{
		WriteToLog( _T( "CAsyncConfTest::SetupShaders - CreatePixelShader() failed. hr=%s\n" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto shader_done;
	}
	SAFE_RELEASE(pShaderBuf);
	SAFE_RELEASE(pErrorBuf);

	if( FAILED(hr = GetFramework()->CompileShaderFromResource(NULL, _T("async.sh"), "VSMain", D3D_SHADER_STAGE_VERTEX, D3D_FEATURE_LEVEL_9_1, D3D_FEATURE_LEVEL_11_1, D3D10_SHADER_DEBUG, &pShaderBuf, &pErrorBuf)) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "CAsyncConfTest::SetupShaders - CompileShaderFromResource() failed compiling VS: %s\n" ), D3DHResultToString(hr).c_str() );
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
		goto shader_done;
	}

	if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, &m_pVS ) ) )
	{
		WriteToLog( _T( "CAsyncConfTest::SetupShaders - CreateVertexShader() failed. hr=0x%x\n" ), hr );
		tRes = RESULT_FAIL;
		goto shader_done;
	}

	D3D11_INPUT_ELEMENT_DESC DeclTL[] =
	{
		{"pos",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},	//position
		{"color", 0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},   //color
	};

	if(	FAILED(	hr = GetDevice()->CreateInputLayout( DeclTL, _countof(DeclTL), pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pLayout ) ) )
	{
		WriteToLog(_T("CAsyncConfTest::SetupShaders - CreateInputLayout() failed. hr=%s\n" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto shader_done;
	}
	GetEffectiveContext()->IASetInputLayout( m_pLayout );

	if( !bIsLevel9 )
	{
		if( FAILED( hr = GetDevice()->CreateGeometryShaderWithStreamOutput(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), 
			declSO, sizeof(declSO)/sizeof(*declSO), &strideSO, 1, 0, NULL, &m_pGS)))
		{
			WriteToLog( _T( "CAsyncConfTest::SetupShaders - CreateGeometryShaderWithStreamOutput() failed. hr=%s\n" ), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto shader_done;
		}
	}

	SAFE_RELEASE(pShaderBuf);
	SAFE_RELEASE(pErrorBuf);

	{
		D3D11_BUFFER_DESC bufdesc;
		bufdesc.ByteWidth = 512;
		bufdesc.Usage = D3D11_USAGE_DYNAMIC;
		bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufdesc.MiscFlags = 0;
		GetDevice()->CreateBuffer( &bufdesc, NULL, &m_pConstBuffer );

		GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		if( !bIsLevel9 )
		{
			GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		}
		GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );

		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context.") );
			return RESULT_FAIL;
		}

		{
			float* pMatrix = NULL;
			D3D11_MAPPED_SUBRESOURCE mappedRes;
			HRESULT hr = GetImmediateContext()->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
			pMatrix = (float*) mappedRes.pData;
			if (SUCCEEDED(hr))
			{
				float transx = (float) m_nPrimitiveSize / 2.f;
				float transy = (float) m_nPrimitiveSize / 2.f;

				// Inverse viewport scale.
				pMatrix[2] = 1 / transx;
				pMatrix[3] = -1 / transy;

				// Prescaled inverse viewport translation.
				pMatrix[0] = -transx * pMatrix[2];
				pMatrix[1] = -transy * pMatrix[3];

				GetImmediateContext()->Unmap(m_pConstBuffer,0);
			}
		}
	}

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	if( !bIsLevel9 )
	{
		GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	}
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		return RESULT_FAIL;
	}
	
shader_done:
	/*
	if (tRes == RESULT_FAIL && pInfoQueue != NULL)
	{
		CInfoQueueHelper IQHelp;
		IQHelp.SetInfoQueue(pInfoQueue);
		IQHelp.FindCoveredMessages();
		IQHelp.LogUncoveredMessages(this);
	}
	*/
	SAFE_RELEASE(pShaderBuf);
	SAFE_RELEASE(pErrorBuf);
	return tRes;
}

TEST_RESULT
CAsyncConfTest::Setup()
{
	if ( g_App.m_D3DOptions.Debug )
		m_pDebug = g_App.GetDebug();
	else
		m_pDebug = NULL;

	TEST_RESULT tRes = SetupDirect3D();
	if( tRes != RESULT_PASS )
		return tRes;
	
	return RESULT_PASS;
}

void
CAsyncConfTest::CleanupDirect3D( )
{
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pRSEnabled );
	SAFE_RELEASE( m_pDSSEnabled );
	SAFE_RELEASE( m_pRTView );
	SAFE_RELEASE( m_pDSView );
	SAFE_RELEASE( m_pTextureRT );
	SAFE_RELEASE( m_pTextureRTCopy );
	SAFE_RELEASE( m_pTextureDepth );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pIndexBuffer );
	SAFE_RELEASE( m_pConstBuffer );
	SAFE_RELEASE( m_pSOBuffer );
	SAFE_RELEASE( m_pLayout );
}

TEST_RESULT
CAsyncConfTest::SetupTestCase()
{
	UINT offset = 0;
	GetEffectiveContext()->SOSetTargets(1, &m_pSOBuffer, &offset);

	HRESULT hr;
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

void
CAsyncConfTest::Cleanup( )
{
	CleanupDirect3D( );
}

HRESULT
CAsyncConfTest::CpuBoundJob()
{
	// TODO: implement CPU-bound job
	return E_FAIL;
}

HRESULT
CAsyncConfTest::SetupGpuJob()
{
	UINT stride = sizeof(TVERTEX);
	UINT offset = 0;
	// Caller will have to call ExecuteEffectiveContext()
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	GetEffectiveContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	GetEffectiveContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	m_bGpuJobSetup = true;

	return S_OK;
}

HRESULT
CAsyncConfTest::GpuBoundJob(bool bExtraGPUWork)
{
	HRESULT hr = S_OK;

	assert(m_bGpuJobSetup);
	if (!m_bGpuJobSetup)
		return E_FAIL;

	if (bExtraGPUWork)
	{
		BYTE OneByte = 0xfe;
		D3D11_BOX Box = {0, 0, 0, 1, 1, 1};
		GetEffectiveContext()->UpdateSubresource(m_pTextureRT, 0, &Box, &OneByte, sizeof(OneByte), 0);
	}

	// Caller will have to call ExecuteEffectiveContext()
	GetEffectiveContext()->DrawIndexed(m_nIndexCount, 0, 0);

	return hr;
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
CAsyncConfTest::ReadBackBufferColor(int x, int y, TEST_COLOR* pColor)
{
	D3D11_BOX pixbox;
	pixbox.left = x;
	pixbox.right = x+1;
	pixbox.top = y;
	pixbox.bottom = y+1;
	pixbox.front = 0;
	pixbox.back = 1;
	D3D11_MAPPED_SUBRESOURCE texMap;
	GetEffectiveContext()->CopySubresourceRegion( m_pTextureRTCopy, 0, 0, 0, 0, m_pTextureRT, 0, &pixbox );

	HRESULT hr;
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		return hr;
	}

	hr = GetImmediateContext()->Map( m_pTextureRTCopy, 0, D3D11_MAP_READ, 0, &texMap );
	if (SUCCEEDED(hr))
	{
		makeD3DColor(DXGI_FORMAT_R32G32B32A32_FLOAT, &texMap, pColor);
	}
	GetImmediateContext()->Unmap( m_pTextureRTCopy, 0 );

	return hr;
}

bool
CAsyncConfTest::TestColorsSame(const TEST_COLOR& c1, const TEST_COLOR& c2, double delta)
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

void
COverAllocTest::InitTestParameters()
{
	testfactor::RFactor rF1, rF2, rI1a, rI1b, rI2, rOverAllocPart;

	m_bOverAlloc = false;

	CUserValueSet<D3D11_QUERY> *pQueryValues = new CUserValueSet< D3D11_QUERY >();
	pQueryValues->AddValue( D3D11_QUERY_EVENT );
	pQueryValues->AddValue( D3D11_QUERY_TIMESTAMP );
	pQueryValues->AddValue( D3D11_QUERY_OCCLUSION );
	pQueryValues->AddValue( D3D11_QUERY_TIMESTAMP_DISJOINT );
	pQueryValues->AddValue( D3D11_QUERY_PIPELINE_STATISTICS );
	pQueryValues->AddValue( D3D11_QUERY_SO_STATISTICS );
	pQueryValues->AddValue( D3D11_QUERY_SO_OVERFLOW_PREDICATE );
	pQueryValues->AddValue( D3D11_QUERY_OCCLUSION_PREDICATE );

	rF1 = AddParameter<D3D11_QUERY>( _T( "Query_Name" ), &m_QueryDesc.Query, pQueryValues );
	rF2 = AddParameter<UINT32>( _T( "Create_Flags" ), &m_QueryDesc.MiscFlags, (UINT32)0 );

	CTestCaseParameter<bool> *pOverAlloc = AddParameter( _T( "OverAlloc" ), &m_bOverAlloc );
	rI1a = AddParameterValue(pOverAlloc, false);
	rI1b = AddParameterValue(pOverAlloc, true);

	rI2 = AddParameter<bool>( _T( "OverAlloc_Issue" ), &m_bOverAllocIssue, BoolValueSet() );

	rOverAllocPart = rI1a;
	if (TestOverAlloc)
		rOverAllocPart = rOverAllocPart + (rI1b * rI2);

	SetRootTestFactor( rF1 * rF2 * rOverAllocPart );
}

TEST_RESULT
COverAllocTest::ExecuteTestCase()
{
	HRESULT hr = S_OK, hr2 = S_OK;
	TEST_RESULT tRes = RESULT_PASS;
	ID3D11Query* pQuery = NULL;

	// TODO: insert test code here
	if (m_bOverAlloc)
	{
		// TODO: evaluate usefulness of this on 64-bit OS
		// allocate some unused memory to fill up address space
		std::list<char*> allocList;
		while (1)
		{
			char* bigChunk = NULL;
			try
			{
				bigChunk = new char[1024*1024*128];
			}
			catch (std::bad_alloc e_ba)
			{
				break;
			}
			allocList.push_back(bigChunk);
		}

		std::list<ID3D11Query*> queryList;
		do {
			hr = GetDevice()->CreateQuery(&m_QueryDesc, &pQuery);
			if (m_bOverAllocIssue && SUCCEEDED(hr) && pQuery != NULL)
			{
				GetEffectiveContext()->End( pQuery );
				if( FAILED(hr = ExecuteEffectiveContext() ) )
				{
					WriteToLog( _T("Failed to execute deferred context.") );
					tRes = RESULT_FAIL;
					goto testDone;
				}
			}
			try
			{
				queryList.push_back(pQuery);
			}
			catch (std::bad_alloc e_ba)
			{
				// try once more, we should have no memory left...
				if (SUCCEEDED(hr))
				{
					hr = GetDevice()->CreateQuery(&m_QueryDesc, &pQuery);
					SAFE_RELEASE(pQuery);
				}
				break;
			}
		} while(SUCCEEDED(hr) && pQuery != NULL);
		if (hr != E_OUTOFMEMORY)
		{
			WriteToLog( _T("CreateQuery failed with code other than E_OUTOFMEMORY. hr=%s\n"), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
		}
		else if (queryList.size() < 10)
		{
			WriteToLog( _T("CreateQuery got E_OUTOFMEMORY with under 10 allocations. hr=%s\n"), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
		}
		std::list<ID3D11Query*>::iterator it;
		for (it = queryList.begin(); it != queryList.end(); ++it)
		{
			SAFE_RELEASE((*it));
		}
		queryList.clear();
		std::list<char*>::iterator allocIt;
		for (allocIt = allocList.begin(); allocIt != allocList.end(); ++allocIt)
		{
			delete[] *allocIt;
		}
		allocList.clear();
		goto testDone;
	}

	// TODO: more over-allocation cases

	goto testDone;

testDone:
	// cleanup
	SAFE_RELEASE(pQuery);

	return tRes;
}

void
CAsyncConfApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();

	// TODO: REF only is a temporary measure while no HAL capable hardware available
	UnRegisterOptionVariable( _T( "SrcOnly" ) );
	m_D3DOptions.SrcOnly = true;
	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;
}

bool
CAsyncConfApp::InitTestGroups()
{
	// Register test classes with the group manager.
	if( !ADD_TEST( _T( "Query" ), CQueryConfTest ) ) return false;

	if( !ADD_TEST( _T( "Predicate\\ClearRTV" ),                 CClearRTVPredicateConfTest ) ) return false;
	if( !ADD_TEST( _T( "Predicate\\ClearDSV" ),                 CClearDSVPredicateConfTest ) ) return false;
	if( !ADD_TEST( _T( "Predicate\\CopyResource" ),             CCopyResourcePredicateConfTest ) ) return false;
	if( !ADD_TEST( _T( "Predicate\\CopySubresourceRegion" ),    CCopySubresourceRegionPredicateConfTest ) ) return false;
	if( !ADD_TEST( _T( "Predicate\\UpdateSubresource" ),        CUpdateSubresourcePredicateConfTest ) ) return false;
	if( !ADD_TEST( _T( "Predicate\\ResolveSubresource" ),       CResolveSubresourcePredicateConfTest ) ) return false;
	if( !ADD_TEST( _T( "Predicate\\GenerateMips" ),             CGenerateMipsPredicateConfTest ) ) return false;
	if( !ADD_TEST( _T( "Predicate\\Draw" ),                     CDrawPredicateConfTest ) ) return false;
	if( !ADD_TEST( _T( "Predicate\\DrawInstanced" ),            CDrawInstancedPredicateConfTest ) ) return false;
	if( !ADD_TEST( _T( "Predicate\\DrawIndexed" ),              CDrawIndexedPredicateConfTest ) ) return false;
	if( !ADD_TEST( _T( "Predicate\\DrawIndexedInstanced" ),     CDrawIndexedInstancedPredicateConfTest ) ) return false;
	if( !ADD_TEST( _T( "Predicate\\DrawAuto" ),                 CDrawAutoPredicateConfTest ) ) return false;

	if( !ADD_TEST( _T( "Counter" ), CCounterConfTest ) ) return false;

// TODO: re-enable this test for final
//	if( !ADD_TEST( _T( "OverAlloc" ), COverAllocTest ) ) return false;

	return true;
}

