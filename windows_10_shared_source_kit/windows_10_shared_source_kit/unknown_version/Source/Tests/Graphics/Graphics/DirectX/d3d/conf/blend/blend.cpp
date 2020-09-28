#include "Blend.h"
#include "FormatDesc.hpp" // used for makeD3DColor()
#include "D3DMSHelper.h"
#include <strstream>

CBlendConfApp  g_App;

///////////////////////////////////////////////////////////////////////////////
//	Blending Conformance testing
///////////////////////////////////////////////////////////////////////////////

D3D11_BLEND_OP NextValue(D3D11_BLEND_OP input)
{
	if (input >= D3D11_BLEND_OP_ADD && input < D3D11_BLEND_OP_MAX)
		return ((D3D11_BLEND_OP)(input + 1));
	if (input == D3D11_BLEND_OP_MAX)
		return D3D11_BLEND_OP_ADD;
	assert(!"Unexpected value");
	return D3D11_BLEND_OP_ADD;
}
D3D11_BLEND NextValue(D3D11_BLEND input, bool bAlpha, bool bAllowDualSource)
{
	switch (input)
	{
	case D3D11_BLEND_ZERO:
		return D3D11_BLEND_ONE;
	case D3D11_BLEND_ONE:
		return bAlpha?D3D11_BLEND_SRC_ALPHA:D3D11_BLEND_SRC_COLOR;
	case D3D11_BLEND_SRC_COLOR:
		return bAlpha?D3D11_BLEND_SRC_ALPHA:D3D11_BLEND_INV_SRC_COLOR;
	case D3D11_BLEND_INV_SRC_COLOR:
		return D3D11_BLEND_SRC_ALPHA;
	case D3D11_BLEND_SRC_ALPHA:
		return D3D11_BLEND_INV_SRC_ALPHA;
	case D3D11_BLEND_INV_SRC_ALPHA:
		return D3D11_BLEND_DEST_ALPHA;
	case D3D11_BLEND_DEST_ALPHA:
		return D3D11_BLEND_INV_DEST_ALPHA;
	case D3D11_BLEND_INV_DEST_ALPHA:
		return bAlpha?D3D11_BLEND_SRC_ALPHA_SAT:D3D11_BLEND_DEST_COLOR;
	case D3D11_BLEND_DEST_COLOR:
		return bAlpha?D3D11_BLEND_SRC_ALPHA_SAT:D3D11_BLEND_INV_DEST_COLOR;
	case D3D11_BLEND_INV_DEST_COLOR:
		return D3D11_BLEND_SRC_ALPHA_SAT;
	case D3D11_BLEND_SRC_ALPHA_SAT:
		return D3D11_BLEND_BLEND_FACTOR;
	case D3D11_BLEND_BLEND_FACTOR:
		return D3D11_BLEND_INV_BLEND_FACTOR;
	case D3D11_BLEND_INV_BLEND_FACTOR:
		return bAllowDualSource?(bAlpha?D3D11_BLEND_SRC1_ALPHA:D3D11_BLEND_SRC1_COLOR):D3D11_BLEND_ZERO;
	case D3D11_BLEND_SRC1_COLOR:
		return bAllowDualSource?(bAlpha?D3D11_BLEND_SRC1_ALPHA:D3D11_BLEND_INV_SRC1_COLOR):D3D11_BLEND_ZERO;
	case D3D11_BLEND_INV_SRC1_COLOR:
		return bAllowDualSource?(D3D11_BLEND_SRC1_ALPHA):D3D11_BLEND_ZERO;
	case D3D11_BLEND_SRC1_ALPHA:
		return bAllowDualSource?(D3D11_BLEND_INV_SRC1_ALPHA):D3D11_BLEND_ZERO;
	case D3D11_BLEND_INV_SRC1_ALPHA:
		return D3D11_BLEND_ZERO;
	}
	assert(!"Unexpected value");
	return D3D11_BLEND_ZERO;
}
D3D11_COLOR_WRITE_ENABLE NextValue(D3D11_COLOR_WRITE_ENABLE input)
{
	UINT8 input_bits = (UINT8)input;
	if (input_bits > D3D11_COLOR_WRITE_ENABLE_ALL)
	{
		assert(!"Unexpected value");
		return D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	return (D3D11_COLOR_WRITE_ENABLE)((input_bits+1) % D3D11_COLOR_WRITE_ENABLE_ALL);
}

static
bool
isDualSource(D3D11_BLEND blend)
{
	switch(blend)
	{
	case D3D11_BLEND_SRC1_COLOR:
	case D3D11_BLEND_INV_SRC1_COLOR:
	case D3D11_BLEND_SRC1_ALPHA:
	case D3D11_BLEND_INV_SRC1_ALPHA:
		return true;
	default:
		return false;
	}
}

int
CBlendConfTest::GetNumRenderWindows() const
{
	return (g_App.VisibleRender())?1:0;
}


TEST_RESULT
CBlendConfTest::SetupDirect3D( )
{
	TEST_RESULT tRes;
	if( (tRes = SetupRenderTarget( )) != RESULT_PASS )
		return tRes;

	if( (tRes = SetupShaders( )) != RESULT_PASS )
		return tRes;

	if( (tRes = SetupGeometry( )) != RESULT_PASS )
		return tRes;

	// create and set rasterizer state
	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_FRONT;
	rastDesc.FrontCounterClockwise = true;
	rastDesc.DepthBias = false;
	rastDesc.DepthBiasClamp = 0.0f;
	rastDesc.SlopeScaledDepthBias = 0.0f;
	rastDesc.DepthClipEnable = true;
	rastDesc.ScissorEnable = false;
	rastDesc.MultisampleEnable = false;
	rastDesc.AntialiasedLineEnable = false;
	m_pDevice1->CreateRasterizerState( &rastDesc, &m_pRSEnabled );

	m_pDeviceContext1->RSSetState( m_pRSEnabled );

	return RESULT_PASS;
}

//---------------------------------------------------------------

TEST_RESULT
CBlendConfTest::SetupGeometry( )
{
	TEST_RESULT tRes = RESULT_PASS;

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
	hr = m_pDevice1->CreateBuffer(&bufferDesc, &resUP, &m_pVertexBuffer);
	if (FAILED(hr))
	{
		WriteToLog( _T( "CBlendConfTest::SetupGeometry - CreateBuffer() failed for vertex buffer. hr=%s\n" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto setup_fail;
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
	hr = m_pDevice1->CreateBuffer(&bufferDesc, &resUP, &m_pIndexBuffer);
	if (FAILED(hr))
	{
		WriteToLog( _T( "CBlendConfTest::SetupGeometry - CreateBuffer() failed for index buffer. hr=%s\n" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto setup_fail;
	}

	return tRes;

setup_fail:
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);

	return tRes;
}

//---------------------------------------------------------------

TEST_RESULT
CBlendConfTest::SetupRenderTarget( )
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	for (int rtNum = 0; rtNum < ARRAYSIZE(m_pRTViews); ++rtNum)
	{
		SAFE_RELEASE(m_pTextureRTs[rtNum]);
		SAFE_RELEASE(m_pRTViews[rtNum]);
		SAFE_RELEASE(m_pRTShaderViews[rtNum]);
	}

	for (int rtNum = 0; rtNum < m_NumRTs; ++rtNum)
	{
		switch (m_nDimension)
		{
		default:
			assert(!"Unexpected dimension value");
			break;
			{
				const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(m_eRTFormats[rtNum]);

				D3D11_BUFFER_DESC bufferdesc;
				bufferdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
				bufferdesc.CPUAccessFlags = 0;
				bufferdesc.MiscFlags = 0;
				bufferdesc.Usage = D3D11_USAGE_DEFAULT;
				bufferdesc.ByteWidth = (pTexFormatInfo->uBitsPerElement * m_nPrimitiveW * m_nPrimitiveH) / 8;

				ID3D11Buffer* pBuffer;
				hr = m_pDevice1->CreateBuffer( &bufferdesc, NULL, &pBuffer );
				if (FAILED(hr))
				{
					tRes = RESULT_FAIL;
					WriteToLog( _T( "CBlendConfTest::SetupRenderTarget - CreateTexture2D() failed for rendertarget %d. hr=%s\n" ), rtNum, D3DHResultToString(hr).c_str() );
					goto setup_fail;
				}
				m_pTextureRTs[rtNum] = dynamic_cast<ID3D11Texture2D*>(pBuffer);
			}
		case 2:
			{
				D3D11_TEXTURE2D_DESC tex2ddesc;
				tex2ddesc.ArraySize = 1;
				tex2ddesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
				tex2ddesc.Format = m_eRTFormats[rtNum];
				tex2ddesc.Height = m_nPrimitiveH;
				tex2ddesc.Width = m_nPrimitiveW;
				tex2ddesc.CPUAccessFlags = 0;
				tex2ddesc.MipLevels = 1;
				tex2ddesc.MiscFlags = 0;
				tex2ddesc.SampleDesc.Count = 1;
				tex2ddesc.SampleDesc.Quality = 0;
				tex2ddesc.Usage = D3D11_USAGE_DEFAULT;

				hr = m_pDevice1->CreateTexture2D( &tex2ddesc, NULL, &m_pTextureRTs[rtNum] );
				if (FAILED(hr))
				{
					tRes = RESULT_FAIL;
					WriteToLog( _T( "CBlendConfTest::SetupRenderTarget - CreateTexture2D() failed for rendertarget %d. hr=%s\n" ), rtNum, D3DHResultToString(hr).c_str() );
					goto setup_fail;
				}
			}
			break;
		}
	}

	
	for (int rtNum = 0; rtNum < m_NumRTs; ++rtNum)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = m_eRTFormatsEffectiveSRV[rtNum];
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		hr = m_pDevice1->CreateShaderResourceView( m_pTextureRTs[rtNum], &srvDesc, &m_pRTShaderViews[rtNum] );
		if (FAILED(hr))
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "CBlendConfTest::SetupRenderTarget - CreateShaderResourceView() failed for rendertarget %d. hr=%s\n" ), rtNum, D3DHResultToString(hr).c_str() );
			goto setup_fail;
		}
	}

	for (int rtNum = 0; rtNum < m_NumRTs; ++rtNum)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtviewdesc;
		rtviewdesc.Texture2D.MipSlice = 0;
		rtviewdesc.Format = m_eRTFormatsEffectiveRTV[rtNum];
		rtviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		hr = m_pDevice1->CreateRenderTargetView( m_pTextureRTs[rtNum], &rtviewdesc, &m_pRTViews[rtNum] );
		if (FAILED(hr))
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "CBlendConfTest::SetupRenderTarget - CreateRenderTargetView() failed. hr=%s\n" ), D3DHResultToString(hr).c_str() );
			goto setup_fail;
		}
	}

	return tRes;

setup_fail:
	for (int rtNum = 0; rtNum < ARRAYSIZE(m_pRTViews); ++rtNum)
	{
		SAFE_RELEASE(m_pTextureRTs[rtNum]);
		SAFE_RELEASE(m_pRTViews[rtNum]);
		SAFE_RELEASE(m_pRTShaderViews[rtNum]);
	}

	return tRes;
}

//---------------------------------------------------------------

TEST_RESULT
CBlendConfTest::SetupShaders( )
{
	TEST_RESULT tRes = RESULT_PASS;
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	HRESULT hr = E_FAIL;

	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pPSVisibleCopy);
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pConstBuffers[0]);
	SAFE_RELEASE(m_pConstBuffers[1]);

	if( m_bFL9X )
	{
		m_sPixelShaderName = m_sPixelShaderNameFL9X;
		m_sVertexShaderName = m_sVertexShaderNameFL9X;
	}

	if (m_sPixelShaderName.size() > 0)
	{
		hr = GetFramework()->CompileShaderFromResource( 
			nullptr,
			m_sPixelShaderName.c_str(),
			m_sPSEntryPt.c_str(), 
			D3D_SHADER_STAGE_PIXEL,
			D3D_FEATURE_LEVEL_9_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D10_SHADER_DEBUG,
			&pShaderBuf,
			&pErrorBuf );
		if( FAILED( hr ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "CBlendConfTest::SetupShaders - CompileShaderFromResource() failed on PS: %s\n" ), m_sPixelShaderName.c_str() );
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
			WriteToLog( _T( "CBlendConfTest::SetupShaders - CreatePixelShader() failed. hr=%s\n" ), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto setup_fail;
		}
		SAFE_RELEASE(pShaderBuf);
		SAFE_RELEASE(pErrorBuf);
	}
	else
	{
		//WriteToLog( _T( "CBlendConfTest::SetupShaders - No pixel shader named\n" ) );
		//tRes = RESULT_FAIL;
		//goto setup_fail;
	}

	if (g_App.VisibleRender())
	{
		tstring sCopyShader = _T("ps_copy.psh");
		hr = GetFramework()->CompileShaderFromResource( 
			nullptr,
			sCopyShader.c_str(),
			_T( "main" ), 
			D3D_SHADER_STAGE_PIXEL,
			D3D_FEATURE_LEVEL_9_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D10_SHADER_DEBUG,
			&pShaderBuf,
			&pErrorBuf );
		if( FAILED( hr ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "CBlendConfTest::SetupShaders - CompileShaderFromResource() failed on PS: %s\n" ), sCopyShader.c_str() );
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
		if( FAILED( hr = GetDevice( )->CreatePixelShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, &m_pPSVisibleCopy ) ) )
		{
			WriteToLog( _T( "CBlendConfTest::SetupShaders - CreatePixelShader() failed. hr=%s\n" ), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto setup_fail;
		}
		SAFE_RELEASE(pShaderBuf);
		SAFE_RELEASE(pErrorBuf);
	}


	if (m_sVertexShaderName.size() > 0)
	{
		hr = GetFramework()->CompileShaderFromResource( 
			nullptr,
			m_sVertexShaderName.c_str(),
			_T( "main" ), 
			D3D_SHADER_STAGE_VERTEX,
			D3D_FEATURE_LEVEL_9_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D10_SHADER_DEBUG,
			&pShaderBuf,
			&pErrorBuf );
		if( FAILED( hr ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "CBlendConfTest::SetupShaders - CompileShaderFromResource() failed on VS: %s\n" ), m_sVertexShaderName.c_str() );
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
			WriteToLog( _T( "CBlendConfTest::SetupShaders - CreateVertexShader() failed. hr=%s\n" ), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto setup_fail;
		}


		D3D11_INPUT_ELEMENT_DESC DeclTL[] =
		{
			{"pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},  //position
			{"vertNum", 0, m_bFL9X ? DXGI_FORMAT_R32_FLOAT : DXGI_FORMAT_R32_UINT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},  //index into cbuffer
		};

		if(	FAILED(	hr = m_pDevice1->CreateInputLayout( DeclTL, sizeof(DeclTL)/sizeof(*DeclTL), pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pLayout ) ) )
		{
			WriteToLog(_T("CBlendConfTest::SetupShaders - CreateInputLayout() failed. hr=%s\n" ), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto setup_fail;
		}
		m_pDeviceContext1->IASetInputLayout( m_pLayout );
		SAFE_RELEASE(pShaderBuf);
		SAFE_RELEASE(pErrorBuf);
	}
	else
	{
		//WriteToLog( _T( "CBlendConfTest::SetupShaders - No vertex shader named\n" ) );
		//tRes = RESULT_FAIL;
		//goto setup_fail;
	}

	D3D11_BUFFER_DESC bufdesc;
	bufdesc.ByteWidth = 4*4*sizeof(float); // 4x4 matrix
	bufdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufdesc.MiscFlags = 0;
	m_pDevice1->CreateBuffer( &bufdesc, NULL, &m_pConstBuffers[0] );
	bufdesc.ByteWidth = 8*4*sizeof(float);
	bufdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufdesc.MiscFlags = 0;
	m_pDevice1->CreateBuffer( &bufdesc, NULL, &m_pConstBuffers[1] );

	{
		float* pMatrix = NULL;
		D3D11_MAPPED_SUBRESOURCE mappedRes;
		HRESULT hr = GetEffectiveContext()->Map( m_pConstBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
		if (SUCCEEDED(hr))
		{
			pMatrix = (float*)mappedRes.pData;
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

	SAFE_RELEASE( pShaderBuf );
	return tRes;

setup_fail:
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pPSVisibleCopy);
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE( pShaderBuf );
	SAFE_RELEASE( pErrorBuf );

	return tRes;
}

TEST_RESULT
CBlendConfTest::Setup()
{
	HRESULT hr = E_FAIL;
	m_pDevice1 = g_App.GetDevice1();
	m_pDeviceContext1 = g_App.GetEffectiveContext1();

	tstring sPath = GetPath().ToString( GetPath().Depth() - 1 );
	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(sPath);
	if (!pTexFormatInfo)
	{
		pTexFormatInfo = CFormatInfoMap::Find(DXGI_FORMAT_R32G32B32A32_FLOAT);
		//WriteToLog(_T("Could not find corresponding format"));
		//return RESULT_FAIL;
	}
	UINT formatSupport;
	m_pDevice1->CheckFormatSupport(pTexFormatInfo->Format, &formatSupport);
	if (0 == (formatSupport & D3D11_FORMAT_SUPPORT_BLENDABLE))
	{
		WriteToLog( _T("[CBasicBlendTest::Setup] This format does not support blending.") );
		return RESULT_SKIP;
	}

	// Check what feature level we're running at
	m_bFL9X = m_pDevice1->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_0;

	//
	// Get the adapter descriptor and WDDM version.
	//

	m_pAdapterDescriptor = DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor( GetDevice() );
	if( nullptr == m_pAdapterDescriptor )
	{
		LogError( __FILEW__, __LINE__, L"GetAdapterDescriptor returned null.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCRequiredCapabilityNotSupported );
		return RESULT_FAIL;
	}

	// Get the WDDM version
	if ( GetFramework()->m_D3DOptions.DriverType == D3D_DRIVER_TYPE_REFERENCE )
	{
		// Ref doesn't support the GetDisplayDriverModel() call
		m_wddmDriverVersion = KMT_DRIVERVERSION_WDDM_1_2;
	}
	else
	{
		if( FAILED( hr = m_pAdapterDescriptor->GetDisplayDriverModel( m_wddmDriverVersion ) ) )
		{
			LogError( __FILEW__, __LINE__, L"GetDisplayDriverModel failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported );
			return RESULT_FAIL;
		}
	}

	// Not all formats are necessarily supported on FL9.x
	// See if we are on a FL9.x format
	if( m_bFL9X && !IsValidFL9Format( pTexFormatInfo->Format ) )
	{
		WriteToLog( _T("The test is running at feature level 9.x. Not all test formats are necessarily supported.") );
		return RESULT_SKIP;
	}

	if ( g_App.m_D3DOptions.Debug )
		m_pDebug = g_App.GetDebug();
	else
		m_pDebug = NULL;

	m_sPrevPSName = _T("");
	m_sPrevPSEntryPt = _T("");
	m_sPrevVSName = _T("");
	m_nPrevNumRTs = 0;

	return RESULT_PASS;
}

void
CBlendConfTest::CleanupDirect3D( )
{
	if (m_pDeviceContext1 != NULL)
	{
		ID3D11Buffer* pNullConstBuffers[2] = {NULL, NULL};
		m_pDeviceContext1->VSSetConstantBuffers( 0, 2, pNullConstBuffers );
		if( !m_bFL9X )
		{
			m_pDeviceContext1->GSSetConstantBuffers( 0, 2, pNullConstBuffers );
		}
		m_pDeviceContext1->PSSetConstantBuffers( 0, 2, pNullConstBuffers );
		m_pDeviceContext1->VSSetShader(NULL, NULL, 0 );
		m_pDeviceContext1->GSSetShader(NULL, NULL, 0 );
		m_pDeviceContext1->PSSetShader(NULL, NULL, 0 );
		m_pDeviceContext1->IASetInputLayout( NULL );
		m_pDeviceContext1->RSSetState( NULL );
		ID3D11RenderTargetView* pRTView = NULL;
		m_pDeviceContext1->OMSetRenderTargets(1, &pRTView, NULL);
	}

	SAFE_RELEASE( m_pConstBuffers[0] );
	SAFE_RELEASE( m_pConstBuffers[1] );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pPSVisibleCopy );
	SAFE_RELEASE( m_pRSEnabled );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pIndexBuffer );
	SAFE_RELEASE( m_pLayout );
	for (int rtNum = 0; rtNum < ARRAYSIZE(m_pRTViews); ++rtNum)
	{
		SAFE_RELEASE(m_pTextureRTs[rtNum]);
		SAFE_RELEASE(m_pTextureRTsStaging[rtNum]);
		SAFE_RELEASE(m_pRTViews[rtNum]);
		SAFE_RELEASE(m_pRTShaderViews[rtNum]);
	}
}

void
CBlendConfTest::Cleanup( )
{
	CleanupDirect3D( );

	if( nullptr != m_pAdapterDescriptor )
	{
		delete m_pAdapterDescriptor;
		m_pAdapterDescriptor = nullptr;
	}
}

TEST_RESULT
CBlendConfTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	ZeroArrayMem(m_pTempMultisampleData);

	return tRes;
}

void
CBlendConfTest::CleanupTestCase()
{
	for(int n = 0; n < ARRAYSIZE(m_pTempMultisampleData); ++n)
		delete[] m_pTempMultisampleData[n];
	ZeroArrayMem(m_pTempMultisampleData);
}

HRESULT
CBlendConfTest::UpdateResources()
{
	bool needsUpdate = false;
	if (m_sPrevPSName != m_sPixelShaderName)
	{
		m_sPrevPSName = m_sPixelShaderName;
		needsUpdate = true;
	}
	if (m_sPrevPSEntryPt != m_sPSEntryPt)
	{
		m_sPrevPSEntryPt = m_sPSEntryPt;
		needsUpdate = true;
	}
	if (m_sPrevVSName != m_sVertexShaderName)
	{
		m_sPrevVSName = m_sVertexShaderName;
		needsUpdate = true;
	}
	if (m_nPrevNumRTs != m_NumRTs)
	{
		m_nPrevNumRTs = m_NumRTs;
		needsUpdate = true;
	}

	if (needsUpdate)
	{
		CleanupDirect3D();
		if (RESULT_PASS != SetupDirect3D())
		{
			m_nPrevNumRTs = 0;
			return E_FAIL;
		}
	}

	m_pDeviceContext1->VSSetConstantBuffers( 0, 2, m_pConstBuffers );
	if( !m_bFL9X )
	{
		m_pDeviceContext1->GSSetConstantBuffers( 0, 2, m_pConstBuffers );
	}
	m_pDeviceContext1->PSSetConstantBuffers( 0, 2, m_pConstBuffers );

	m_pDeviceContext1->VSSetShader( m_pVS, NULL, 0 );
	m_pDeviceContext1->GSSetShader( NULL, NULL, 0 );
	m_pDeviceContext1->PSSetShader( m_pPS, NULL, 0 );

	return S_OK;
}

bool
CBlendConfTest::IsValidFL9Format( const DXGI_FORMAT &format ) const
{
	// Local variables
	bool bWDDM1_3OrHigher = false;
	bool bSwapbufferFormat = false;

	//
	// Common checks.
	//

	// Check the WDDM level
	if( m_wddmDriverVersion >= KMT_DRIVERVERSION_WDDM_1_3 )
	{
		bWDDM1_3OrHigher = true;
	}

	//
	// Swapbuffer test cases
	// Driver must be WDDM1.3 or higher
	// Format must be A8_UNORM, R8_UNORM, or R8G8_UNORM
	//

	// Check the current format
	switch( format )
	{
	case DXGI_FORMAT_A8_UNORM:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8G8_UNORM:
		bSwapbufferFormat = true;
		break;
	}

	if( bWDDM1_3OrHigher && bSwapbufferFormat )
	{
		return true;
	}

	return false;
}

static
float
unormVal(void* pData, int startBit, int numBits)
{
	assert(0 < numBits && numBits <= 32);
	UINT32 val = 0;
	for (int n = 0; n < numBits; ++n)
	{
		int offset = n+startBit;
		val |= ((*(((char*)pData)+((offset)/8))>>(offset%8))&0x1)<<n;
	}
	float retVal = (float)val;
	retVal /= (1<<numBits)-1;
	assert(0.0f <= retVal && retVal <= 1.0f );
	return retVal;
}
static
float
snormVal(void* pData, int startBit, int numBits)
{
	assert(1 < numBits && numBits <= 32);
	INT32 val = 0;
	for (int n = 0; n < numBits; ++n)
	{
		int offset = n+startBit;
		val |= ((*(((char*)pData)+((offset)/8))>>(offset%8))&0x1)<<n;
	}
	if (val == 1<<(numBits-1))
		return -1.0f;
	if (val == (1<<(numBits-1))-1)
		return 1.0f;
	if (val & 1<<(numBits-1))
		val |= 0-(1<<numBits);
	float retVal = (float)val;
	retVal /= (1<<(numBits-1))-1;
	assert(-1.0f <= retVal && retVal <= 1.0f );
	return retVal;
}

static
float
gammaToLinear( float a )
{
#define D3D10_SRGB_GAMMA	( 2.2f )
#define D3D10_SRGB_TO_FLOAT_DENOMINATOR_1	( 12.92f )
#define D3D10_SRGB_TO_FLOAT_DENOMINATOR_2	( 1.055f )
#define D3D10_SRGB_TO_FLOAT_EXPONENT	( 2.4f )
#define D3D10_SRGB_TO_FLOAT_OFFSET	( 0.055f )
#define D3D10_SRGB_TO_FLOAT_THRESHOLD	( 0.04045f )
#define D3D10_SRGB_TO_FLOAT_TOLERANCE_IN_ULP	( 0.5f )

	if (a < D3D10_SRGB_TO_FLOAT_THRESHOLD)
	{
		return a / D3D10_SRGB_TO_FLOAT_DENOMINATOR_1;
	}
	else
	{
		float temp = a + D3D10_SRGB_TO_FLOAT_OFFSET;
		temp = temp / D3D10_SRGB_TO_FLOAT_DENOMINATOR_2;
		return pow(temp, D3D10_SRGB_TO_FLOAT_EXPONENT);
	}
}

static
void
makeD3DColor(DXGI_FORMAT format, const D3D11_MAPPED_SUBRESOURCE *pTexMap, TEST_COLOR* pColor)
{
	pColor->r = 0.0;
	pColor->g = 0.0;
	pColor->b = 0.0;
	pColor->a = 1.0;

	R32G32B32A32_FLOAT* pFloat32 = (R32G32B32A32_FLOAT*)(pTexMap->pData);
	R16G16B16A16_FLOAT* pFloat16 = (R16G16B16A16_FLOAT*)(pTexMap->pData);
	R11G11B10_FLOAT* pFloat11 = (R11G11B10_FLOAT*)(pTexMap->pData);

	switch(format)
	{
	default:
		assert(!"Unrecognized format");
		return;
	// floats
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		pColor->a = pFloat32->A();
	case DXGI_FORMAT_R32G32B32_FLOAT:
		pColor->b = pFloat32->B();
	case DXGI_FORMAT_R32G32_FLOAT:
		pColor->g = pFloat32->G();
	case DXGI_FORMAT_R32_FLOAT:
		pColor->r = pFloat32->R();
		break;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		pColor->a = pFloat16->A();
		pColor->b = pFloat16->B();
	case DXGI_FORMAT_R16G16_FLOAT:
		pColor->g = pFloat16->G();
	case DXGI_FORMAT_R16_FLOAT:
		pColor->r = pFloat16->R();
		break;
	case DXGI_FORMAT_R11G11B10_FLOAT:
		pColor->b = pFloat11->B();
		pColor->g = pFloat11->G();
		pColor->r = pFloat11->R();
		break;
	// unorms
	case DXGI_FORMAT_R16G16B16A16_UNORM:
		pColor->a = unormVal(pTexMap->pData, 48, 16);
		pColor->b = unormVal(pTexMap->pData, 32, 16);
	case DXGI_FORMAT_R16G16_UNORM:
		pColor->g = unormVal(pTexMap->pData, 16, 16);
	case DXGI_FORMAT_R16_UNORM:
		pColor->r = unormVal(pTexMap->pData,  0, 16);
		break;
	case DXGI_FORMAT_R10G10B10A2_UNORM:
		pColor->a = unormVal(pTexMap->pData, 30,  2);
		pColor->b = unormVal(pTexMap->pData, 20, 10);
		pColor->g = unormVal(pTexMap->pData, 10, 10);
		pColor->r = unormVal(pTexMap->pData,  0, 10);
		break;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		pColor->a = unormVal(pTexMap->pData, 24, 8);
		pColor->b = unormVal(pTexMap->pData, 16, 8);
	case DXGI_FORMAT_R8G8_UNORM:
		pColor->g = unormVal(pTexMap->pData,  8, 8);
	case DXGI_FORMAT_R8_UNORM:
		pColor->r = unormVal(pTexMap->pData,  0, 8);
		break;
	case DXGI_FORMAT_A8_UNORM:
		pColor->a = unormVal(pTexMap->pData,  0, 8);
		break;
	// snorms
	case DXGI_FORMAT_R16G16B16A16_SNORM:
		pColor->a = snormVal(pTexMap->pData, 48, 16);
		pColor->b = snormVal(pTexMap->pData, 32, 16);
	case DXGI_FORMAT_R16G16_SNORM:
		pColor->g = snormVal(pTexMap->pData, 16, 16);
	case DXGI_FORMAT_R16_SNORM:
		pColor->r = snormVal(pTexMap->pData,  0, 16);
		break;
	case DXGI_FORMAT_R8G8B8A8_SNORM:
		pColor->a = snormVal(pTexMap->pData, 24, 8);
		pColor->b = snormVal(pTexMap->pData, 16, 8);
	case DXGI_FORMAT_R8G8_SNORM:
		pColor->g = snormVal(pTexMap->pData,  8, 8);
	case DXGI_FORMAT_R8_SNORM:
		pColor->r = snormVal(pTexMap->pData,  0, 8);
		break;
	}
	if (format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
	{
		for (int n = 0; n < 3; ++n) // no alpha conversion
		{
			float b = (float)pColor->f[n];
			pColor->f[n] = gammaToLinear( b );
		}
	}
}

HRESULT
CBlendConfTest::ReadBackBufferColor(int x, int y, int rtNum, TEST_COLOR* pColor)
{
	HRESULT hr = S_OK;
	MSHelper helper;
	const UINT rtWidth = m_nPrimitiveW;
	const UINT rtHeight = m_nPrimitiveH;
	const size_t rtSizePixels = rtWidth * rtHeight;
	const size_t rtSizeFloats = rtSizePixels * 4;
	const int numMultisamples = 1;
	if (!m_pTempMultisampleData[rtNum])
	{
		m_pTempMultisampleData[rtNum] = new FLOAT[rtSizeFloats * numMultisamples];
		if (!helper.Read(&g_App, m_pTextureRTs[rtNum], m_pTempMultisampleData[rtNum], m_eRTFormatsEffectiveRTV[rtNum]))
			hr = E_UNEXPECTED;
	}

	for (int n = 0; n < 4; ++n)
		pColor->f[n] = 0;
	for (int s = 0; s < numMultisamples; ++s)
	{
		pColor->r += m_pTempMultisampleData[rtNum][0 + (x + y*rtWidth)*4 + s*rtSizeFloats];
		pColor->g += m_pTempMultisampleData[rtNum][1 + (x + y*rtWidth)*4 + s*rtSizeFloats];
		pColor->b += m_pTempMultisampleData[rtNum][2 + (x + y*rtWidth)*4 + s*rtSizeFloats];
		pColor->a += m_pTempMultisampleData[rtNum][3 + (x + y*rtWidth)*4 + s*rtSizeFloats];
	}
	for (int n = 0; n < 4; ++n)
		pColor->f[n] /= numMultisamples;

	return hr;
}

void
CBlendConfTest::DoVisibleRender()
{
	HRESULT hr = E_FAIL;
	TEST_VEC4F texCoords[4] = {
		{0,0,0,0},
		{1,0,0,0},
		{1,1,0,0},
		{0,1,0,0},
	};

	// create and set viewport
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float) (m_nPrimitiveW*2);
	vp.Height   = (float) m_nPrimitiveH;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	m_pDeviceContext1->RSSetViewports(1, &vp);
	RECT presentRect = {0,0,m_nPrimitiveW*2,m_nPrimitiveH};

	// set render target
	CD3D11TestFramework* pFW = GetFramework();
	ID3D11RenderTargetView* pRTView = pFW->GetRTView();
	m_pDeviceContext1->OMSetRenderTargets(1, &pRTView, NULL);

	// set shader
	m_pDeviceContext1->PSSetShader( m_pPSVisibleCopy, NULL, 0 );

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
	hr = m_pDevice1->CreateSamplerState(&sampDesc, &pSampler);
	if (FAILED(hr))
		return;
	m_pDeviceContext1->PSSetSamplers(0, 1, &pSampler);

	for (int rtNum = 0; rtNum < m_NumRTs; ++rtNum)
	{
		if (0 == (m_RTEnableCombo & (1<<rtNum)))
			continue;
		// shader resource view
		m_pDeviceContext1->PSSetShaderResources(0, 1, &m_pRTShaderViews[rtNum]);

		// draw
		DrawQuad(texCoords, false, m_vBlendFactor, NULL);

		// present
		ExecuteEffectiveContext();
		bool stretch = g_App.Stretch();
		g_App.Present(&presentRect, stretch?NULL:&presentRect, NULL, NULL, 0, 0);
	}

	// clean up
	ID3D11SamplerState* pNullSampler = NULL;
	m_pDeviceContext1->PSSetSamplers(0, 1, &pNullSampler);
	SAFE_RELEASE(pSampler);
}

void
CBlendConfTest::DoShowResources()
{
	for (int rtNum = 0; rtNum < m_NumRTs; ++rtNum)
	{
		if (0 == (m_RTEnableCombo & (1<<rtNum)))
			continue;

		std::wstringstream filename;
		filename << L"Render Target " << rtNum;

		g_App.ShowResource(filename.str().c_str(), m_pTextureRTs[rtNum]);
	}
}

HRESULT
CBlendConfTest::DrawQuad(const TEST_VEC4F vertColors[4], 
						 bool isSrc, 
						 const TEST_VEC4F& blendFactor, 
						 const TEST_VEC4F extraColors[4])
{
	HRESULT hr = S_OK;

	if (!m_pConstBuffers[1])
		return E_FAIL;

	const D3D_FEATURE_LEVEL featureLevel = GetDevice()->GetFeatureLevel();
	float* pConstData = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	GetEffectiveContext()->Map( m_pConstBuffers[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
	pConstData = (float*) mappedRes.pData;

	assert(4*sizeof(float) == sizeof(TEST_VEC4F));
	memcpy(pConstData, vertColors, 4*4*sizeof(float));
	if (extraColors)
		memcpy(pConstData+4*4, extraColors, 4*4*sizeof(float));
	else
		memset(pConstData+4*4, 0, 4*4*sizeof(float));

	GetEffectiveContext()->Unmap(m_pConstBuffers[1],0);

	ID3D11BlendState* pBlendState = NULL;
	TEST_VEC4F defaultBlendFactor = {0,0,0,0};
	const TEST_VEC4F* pBlendFactor = &defaultBlendFactor;
	UINT sampleMask = 0xffffffff;
	if (isSrc)
	{
		if( m_bFL9X )
		{
			// 10L9 requires all targets are enabled or disabled
			m_BlendDesc_1.IndependentBlendEnable = FALSE;
			for (int n = 0; n < 8; ++n)
			{
				m_BlendDesc_1.RenderTarget[n].BlendEnable = (m_RTEnableCombo > 0);
			}
		}
		else
		{
			m_BlendDesc_1.IndependentBlendEnable = TRUE;
			for (int n = 0; n < 8; ++n)
			{
				m_BlendDesc_1.RenderTarget[n].BlendEnable = (m_RTEnableCombo & (1<<n))?TRUE:FALSE;
			}
		}
		tstring sPath = GetPath().ToString( GetPath().Depth() - 1 );
		for (int n = 1; n < 8; ++n)
		{
			m_BlendDesc_1.RenderTarget[n].BlendOp = NextValue(m_BlendDesc_1.RenderTarget[n-1].BlendOp);
			m_BlendDesc_1.RenderTarget[n].BlendOpAlpha = NextValue(m_BlendDesc_1.RenderTarget[n-1].BlendOpAlpha);
			m_BlendDesc_1.RenderTarget[n].SrcBlend = NextValue(m_BlendDesc_1.RenderTarget[n-1].SrcBlend, false, false);
			m_BlendDesc_1.RenderTarget[n].SrcBlendAlpha = NextValue(m_BlendDesc_1.RenderTarget[n-1].SrcBlendAlpha, true, false);
			m_BlendDesc_1.RenderTarget[n].DestBlend = NextValue(m_BlendDesc_1.RenderTarget[n-1].DestBlend, false, false);
			m_BlendDesc_1.RenderTarget[n].DestBlendAlpha = NextValue(m_BlendDesc_1.RenderTarget[n-1].DestBlendAlpha, true, false);
			if (sPath == _T("WriteMask") && (featureLevel >= D3D_FEATURE_LEVEL_9_3) ) // 9.1 & 9.2 require writemask be the same on each RT
			{
				m_BlendDesc_1.RenderTarget[n].RenderTargetWriteMask = NextValue((D3D11_COLOR_WRITE_ENABLE)m_BlendDesc_1.RenderTarget[n-1].RenderTargetWriteMask);
			}
			else
			{
				m_BlendDesc_1.RenderTarget[n].RenderTargetWriteMask = m_BlendDesc_1.RenderTarget[0].RenderTargetWriteMask;
			}
		}
		bool bDualSource_Src = isDualSource(m_BlendDesc_1.RenderTarget[0].SrcBlend);
		bool bDualSource_Dst = isDualSource(m_BlendDesc_1.RenderTarget[0].DestBlend);
		bool bDualSource = bDualSource_Src || bDualSource_Dst;

		bool bDualSourceAlpha_Src = isDualSource(m_BlendDesc_1.RenderTarget[0].SrcBlendAlpha);
		bool bDualSourceAlpha_Dst = isDualSource(m_BlendDesc_1.RenderTarget[0].DestBlendAlpha);
		bool bDualSourceAlpha = bDualSourceAlpha_Src || bDualSourceAlpha_Dst;

		if (bDualSource || bDualSourceAlpha)
		{
			for (int n = 1; n < 8; ++n)
				m_BlendDesc_1.RenderTarget[n].BlendEnable = FALSE;
		}
		if (g_App.m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_0)
		{
			D3D11_BLEND_DESC blendDesc_10_0;
			ZeroMemory(&blendDesc_10_0, sizeof(blendDesc_10_0));
			blendDesc_10_0.IndependentBlendEnable = TRUE;
			blendDesc_10_0.AlphaToCoverageEnable = m_BlendDesc_1.AlphaToCoverageEnable;
			for (int n = 0; n < 8; ++n)
			{
				// the 10.0 blend_desc can still enable/disable blending on each target independently
				blendDesc_10_0.RenderTarget[n].BlendEnable = m_BlendDesc_1.RenderTarget[n].BlendEnable;
				blendDesc_10_0.RenderTarget[n].RenderTargetWriteMask = m_BlendDesc_1.RenderTarget[n].RenderTargetWriteMask;
				// but each RT element must be the same
				blendDesc_10_0.RenderTarget[n].SrcBlend = m_BlendDesc_1.RenderTarget[0].SrcBlend;
				blendDesc_10_0.RenderTarget[n].SrcBlendAlpha = m_BlendDesc_1.RenderTarget[0].SrcBlendAlpha;
				blendDesc_10_0.RenderTarget[n].DestBlend = m_BlendDesc_1.RenderTarget[0].DestBlend;
				blendDesc_10_0.RenderTarget[n].DestBlendAlpha = m_BlendDesc_1.RenderTarget[0].DestBlendAlpha;
				blendDesc_10_0.RenderTarget[n].BlendOp = m_BlendDesc_1.RenderTarget[0].BlendOp;
				blendDesc_10_0.RenderTarget[n].BlendOpAlpha = m_BlendDesc_1.RenderTarget[0].BlendOpAlpha;
			}
			hr = m_pDevice1->CreateBlendState(&blendDesc_10_0, &pBlendState);
			if (FAILED(hr))
			{
				WriteToLog( _T("CreateBlendState() failed. hr=%s"), D3DHResultToString(hr).c_str() );
				return hr;
			}
		}
		else if (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1)
		{
			hr = m_pDevice1->CreateBlendState(&m_BlendDesc_1, &pBlendState);
			if (FAILED(hr))
			{
				WriteToLog( _T("CreateBlendState1() failed. hr=%s"), D3DHResultToString(hr).c_str() );
				return hr;
			}
		}
		pBlendFactor = &blendFactor;
	}
	m_pDeviceContext1->OMSetBlendState(pBlendState, pBlendFactor->f, sampleMask);

	const UINT stride = sizeof(TVERTEX);
	const UINT offset = 0;
	m_pDeviceContext1->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_pDeviceContext1->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pDeviceContext1->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	m_pDeviceContext1->DrawIndexed(m_nIndexCount, 0, 0);

	ID3D11Buffer* pNullBuf = NULL;
	m_pDeviceContext1->IASetVertexBuffers(0, 1, &pNullBuf, &stride, &offset);
	m_pDeviceContext1->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);

	m_pDeviceContext1->OMSetBlendState(NULL, defaultBlendFactor.f, sampleMask);
	SAFE_RELEASE(pBlendState);

	return hr;
}


// some utility functions
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
TEST_VEC4F operator+(const TEST_VEC4F& lhs, const TEST_VEC4F& rhs)
{
	TEST_VEC4F retVec = lhs;
	for (int n = 0; n < 4; ++n)
		retVec.f[n] += rhs.f[n];
	return retVec;
}
TEST_VEC4F operator-(const TEST_VEC4F& lhs, const TEST_VEC4F& rhs)
{
	TEST_VEC4F retVec = lhs;
	for (int n = 0; n < 4; ++n)
		retVec.f[n] -= rhs.f[n];
	return retVec;
}

TEST_COLOR vec2color(const TEST_VEC4F& vec)
{
	TEST_COLOR retCol;
	retCol.r = vec.a;
	retCol.g = vec.b;
	retCol.b = vec.c;
	retCol.a = vec.d;
	return retCol;
}


void
CBlendConfApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();

	// Indicate that this test runs FL9.X
	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;

	UnRegisterOptionVariable("srconly");
	m_D3DOptions.SrcOnly = true;

	if (!GetShell()->IsConsole())
	{
		RegisterOptionVariable("present", "show render results in visible window", &m_bVisibleRender, false);
		RegisterOptionVariable("stretch", "stretch on present", &m_bStretch, false);
	}

	RegisterOptionVariable("width", "render target width", &m_Width, 16);
	RegisterOptionVariable("height", "render target height", &m_Height, 16);

	RegisterOptionVariable("AllPixels", "Log all pixel failures", &m_bLogAllPixelFailures, false);
}

bool
CBlendConfApp::InitTestGroups()
{
	// Register test classes with the group manager.

	if (GetFramework()->GetTestMode() == WGFTEST_MODE_WHQL)
	{
		m_bVisibleRender = false;
		m_Width = 16;
		m_Height = 16;
	}

	CFormatValueSet* pFormats = FormatValueSet(FF_FLOAT|FF_UNORM|FF_SNORM|FF_SHAREDEXP|FF_STRIDED, 
		FF_D);
	size_t numFormats = pFormats->GetSize();
	for (size_t n = 0; n < numFormats; ++n)
	{
		tstring groupName = _T("Blend\\Format\\");
		DXGI_FORMAT format;
		pFormats->GetValue(n, &format);
		const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(format);
		if (pTexFormatInfo)
		{
			groupName += pTexFormatInfo->szDesc;
			if( !ADD_TEST( groupName, CBasicBlendTest ) ) return false;
		}
	}
	if( !ADD_TEST( _T("Blend\\WriteMask"), CBasicBlendTest ) ) return false;
	if( !ADD_TEST( _T("Blend\\States" ), CBlendStateTest ) ) return false;
	if( !ADD_TEST( _T("LogicOp" ), CLogicOpTest ) ) return false;

	delete pFormats;

	return true;
}

