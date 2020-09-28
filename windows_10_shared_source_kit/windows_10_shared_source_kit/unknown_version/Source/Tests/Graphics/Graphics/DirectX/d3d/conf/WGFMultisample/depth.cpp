//////////////////////////////////////////////////////////////////////////
//  Depth.cpp
//  created:	2006/08/03
//
//  purpose:    test multisample depth
//////////////////////////////////////////////////////////////////////////

#include "depth.h"


TEST_TYPE TestType[ ] =
{
	NORMAL,
	COMBO_MSOFF
	//COMBO_ODEPTH  // this part doesn't work yet
};



//////////////////////////////////////////////////////////////////////////
// CMultisampleTest_Depth
//////////////////////////////////////////////////////////////////////////

CMultisampleTest_Depth::CMultisampleTest_Depth() 
: CMultisampleTest(),
  m_pPSDepth(NULL),
  m_pDSTex2D(NULL),
  m_pDSView(NULL),
  m_pDSState(NULL),
  m_pDSCompState(NULL),
  m_pRSMSOff(NULL)
{}

extern DXGI_FORMAT DFormats[]; 
extern UINT DFormatsSize;
void CMultisampleTest_Depth::InitTestParameters()
{
	// common multisample test parameters
	m_bDepthTest = true;
	InitMultisampleParameters();

	CTestCaseParameter< DXGI_FORMAT > * pDSFormatParam = 
		AddParameter<DXGI_FORMAT>( _T( "DS_format" ), &m_DSFormat );

	testfactor::RFactor rfDSFormats = AddParameterValueSet( pDSFormatParam,
		new CTableValueSet<DXGI_FORMAT>( DFormats, sizeof( DXGI_FORMAT ), DFormatsSize ) );
	
	SetRootTestFactor ( ( rfDSFormats % m_rfPixPos ) * m_rfSampleDesc );

	//TODO: the depth test currently does not apply to Center pattern. Skip for now
	CFilterHandle filter = FilterEqual<QUALITY_LEVEL_SCENARIO>(m_pQualityLevelParam, QUALITY_LEVEL_CENTER);
	SkipMatching( filter );
}

TEST_RESULT CMultisampleTest_Depth::SetupTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	tRes = SetFormatAndQualityLevel( true );
	if ( tRes != RESULT_PASS )
	{
		return tRes;
	}

	m_TestType = NORMAL;

	try 
	{
		// test length is here in case the test cases take too long
		if ( m_uSampleCount == 32 )
			m_TestLength = MEDIUM;
		else
			m_TestLength = LARGE;

		m_pDSTex2D = g_TestApp.GetDSTexture();
		m_pDSView = g_TestApp.GetDSView();
		
		SetupD3D();
		if ( UpdateFormatOrSampleDesc() )
		{
			if( FAILED( hr = m_Finder.AddDepth( m_pDSTex2D, m_pDSView ) ) )
				throw TRException("AddDepth failed.", hr, RESULT_FAIL);

			//Might only happen once for this test.
			if( FAILED( hr = m_Finder.FindSamples() ) )
				throw TRException("FindSamples failed.", hr, RESULT_FAIL);

			ResetStates();
		}	

		//
		// Create Rasterizer state with MS disabled
		//
		D3D11_RASTERIZER_DESC rastDesc;
		rastDesc.CullMode = D3D11_CULL_NONE;
		rastDesc.FrontCounterClockwise = true;
		rastDesc.DepthBias = false;
		rastDesc.DepthBiasClamp = 0;
		rastDesc.SlopeScaledDepthBias = 0;
		rastDesc.DepthClipEnable = true;
		rastDesc.ScissorEnable = false;
		rastDesc.MultisampleEnable = false;
		rastDesc.AntialiasedLineEnable = false;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		if( FAILED( hr = m_pDevice->CreateRasterizerState( &rastDesc, &m_pRSMSOff ) ) )
			throw TRException("CreateRasterizerState for m_pRSMSOff failed.", hr, RESULT_FAIL);

		//
		// Create DepthStencil state
		//
		bool bStencilEnable = (GetFlags(m_DSFormat) & FF_STENCIL) > 0;
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		memset(&dsDesc, 0, sizeof(dsDesc));
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsDesc.StencilEnable = bStencilEnable;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;
		dsDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilFunc		= D3D11_COMPARISON_ALWAYS;
		dsDesc.BackFace.StencilFailOp		= D3D11_STENCIL_OP_INCR;
		dsDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_INCR;
		dsDesc.BackFace.StencilPassOp		= D3D11_STENCIL_OP_INCR;
		dsDesc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;
		if( FAILED( hr = GetDevice()->CreateDepthStencilState( &dsDesc, &m_pDSState ) ) )
			throw TRException("CreateDepthStencilState() failed.", hr, RESULT_FAIL);
		
		dsDesc.DepthEnable = FALSE;
		dsDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc		= D3D11_COMPARISON_EQUAL;
		dsDesc.BackFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc			= D3D11_COMPARISON_EQUAL;
		if( FAILED( hr = GetDevice()->CreateDepthStencilState( &dsDesc, &m_pDSCompState ) ) )
			throw TRException("CreateDepthStencilState() failed.", hr, RESULT_FAIL);

		//
		// Create Shaders
		//
		ID3D10Blob *pPSBuf = NULL;
		if ( FAILED( hr = D3DX10CompileFromResource(NULL, "ps_odepth.psh", NULL, NULL, NULL, "main", "ps_4_0", 0, 0, NULL, &pPSBuf, NULL, NULL ) ) )
			throw TRException("CompileShaderFromResource Failed.", hr, RESULT_FAIL);
		if ( FAILED( hr = m_pDevice->CreatePixelShader( (DWORD*) pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &m_pPSDepth ) ) )
			throw TRException("CreatePixelShader Failed.", hr, RESULT_FAIL);
		SAFE_RELEASE( pPSBuf );

	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Depth::SetupTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}
	
	return tRes;
}

void CMultisampleTest_Depth::CleanupTestCase()
{
	CMultisampleTest::CleanupD3D();

	m_pDeviceContext->OMSetDepthStencilState( NULL, 0 );

	SAFE_RELEASE( m_pPSDepth );
	SAFE_RELEASE( m_pDSState );
	SAFE_RELEASE( m_pDSCompState );
	SAFE_RELEASE( m_pRSMSOff );
}

TEST_RESULT CMultisampleTest_Depth::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	try
	{
		// clear
		FLOAT ClearColor[4] = { 0, 0, 0, 0 };
		m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, m_pDSView );
		m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
		m_pDeviceContext->ClearDepthStencilView( m_pDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		m_pDeviceContext->OMSetDepthStencilState( m_pDSState, 0 );


		bool bStencilEnable = (GetFlags(m_DSFormat) & FF_STENCIL) > 0;
		for (UINT uSampleA = 0; uSampleA < m_uSampleCount; ++uSampleA)
		{
			//
			// 1st Test: Make sure sample A can pass and fail depth
			//

			// Step 1: cover sample A with average depth
			m_uVertexCount = 0;
			SetSample( uSampleA );
			SetDepth( 0.5f );
			m_pDeviceContext->Draw(m_uVertexCount, 0);
			if( IsPixelBlack( ) ) // pixel should not be black
				throw TRException("Initial sample coverage failed.");

			// Step 2: non-normal tests toggle states and write to all samples
			if (m_TestType != NORMAL)
			{
				if (m_TestType == COMBO_MSOFF)
				{
					// disable multisampling
					m_pDeviceContext->RSSetState( m_pRSMSOff );
					
					// write depth to all samples
					// Note: depth test should still be performed per sample
					m_uVertexCount = 0;
					CoverPixel();
					SetDepth( 0.8f );
					m_pDeviceContext->Draw(m_uVertexCount, 0);

					// enable multisampling
					m_pDeviceContext->RSSetState( m_pRSEnabled );
				}
				else if (m_TestType == COMBO_ODEPTH)
				{
					// switch to shader that writes depth
					m_pDeviceContext->PSSetShader( m_pPSDepth, NULL, 0 );

					// cover a different sample
					// Note: odepth should broadcast to all samples for depth test
					UINT sample = (uSampleA+1)%m_uSampleCount;
					m_uVertexCount = 0;
					SetSample( sample );
					SetDepth( 0.8f );
					m_pDeviceContext->Draw(m_uVertexCount, 0);

					// switch back to regular shader
					m_pDeviceContext->PSSetShader( m_pPS, NULL, 0 );
				}

				// reset vertex buffer
				m_uVertexCount = 0;
				SetSample( uSampleA );
			}

			// Step 3: cover sample A with greater depth (should fail depth test)
			m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
			SetDepth( 0.6f );
			m_pDeviceContext->Draw(m_uVertexCount, 0);
			if( !IsPixelBlack( ) ) // pixel should be black
				throw TRException("Same sample should have failed depth test.");	

			// Step 4: cover sample A with lesser depth (should pass depth test)
			m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
			SetDepth( 0.4f );
			m_pDeviceContext->Draw(m_uVertexCount, 0);
			if( IsPixelBlack( ) )
				throw TRException("Same sample should have passed depth test.");	

			//Step 5: test A's stencil value
			if (bStencilEnable)
			{
				m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
				m_pDeviceContext->OMSetDepthStencilState( m_pDSCompState, (m_TestType == NORMAL) ? 3 : 4 );
				m_pDeviceContext->Draw(m_uVertexCount, 0);
				if( IsPixelBlack( ) ) // pixel should have passed stencil
					throw TRException("Sample stencil value is incorrect");	
				m_pDeviceContext->OMSetDepthStencilState( m_pDSState, 0 );
			}



			//
			// 2nd Test: Make sure sample B can pass depth 
			//
			UINT uSampleB;
			for (UINT uSampleB = 0; uSampleB < m_uSampleCount; ++uSampleB)
			{
				if (uSampleB == uSampleA) continue;

				// cover sample B
				m_uVertexCount = 0;
				SetSample( uSampleB );

				// Step 1: non-normal tests should fail on all samples with greater than 0.8 depth
				if (m_TestType != NORMAL)
				{
					m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
					SetDepth( 0.9f );
					m_pDeviceContext->Draw(m_uVertexCount, 0);
					if ( !IsPixelBlack() )
						throw TRException("The second sample should have failed depth test.");
				}

				//Step 2: test B's stencil value
				if (bStencilEnable)
				{
					m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
					m_pDeviceContext->OMSetDepthStencilState( m_pDSCompState, (m_TestType == NORMAL) ? 0 : 2 );
					m_pDeviceContext->Draw(m_uVertexCount, 0);
					if( IsPixelBlack( ) ) // pixel should have passed stencil
						throw TRException("Second sample stencil value is incorrect");	
					m_pDeviceContext->OMSetDepthStencilState( m_pDSState, 0 );
				}

				// Step 3: cover sample B with greater depth than sample A but less than 0.8
				m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
				SetDepth( 0.7f );
				m_pDeviceContext->Draw(m_uVertexCount, 0);
				if( IsPixelBlack( ) ) 
					throw TRException("The second sample should have passed depth test.");

				// only do full SampleB loop if LARGE test
				if (m_TestLength < LARGE) break;
			}

			// only do full SampleA loop if MEDIUM or LARGE test
			if (m_TestLength < MEDIUM) break;

			// clear render target and depth/stencil
			m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
			m_pDeviceContext->ClearDepthStencilView( m_pDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Depth::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}

	try
	{
		Present();
	}
	catch (TRException& exc)
	{
		exc.Prepend("[CMultisampleTest_Depth::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}

	return tRes;	
}

BOOL CMultisampleTest_Depth::IsPixelBlack()
{
	HRESULT hr;
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
		throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

	BOOL bIsBlack;
	if( RESULT_PASS != m_Finder.TestPixelNonZero(bIsBlack, true) )
		throw TRException("TestPixelNonZero failed.");

	// reset state
	m_pDeviceContext->OMSetDepthStencilState( m_pDSState, 0 );
	m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, m_pDSView );

	// reset vertex buffer
	m_uVertexCount = 0;
	CoverSample( m_uCurrentSample );

	return !bIsBlack;
}

void CMultisampleTest_Depth::SetSample(UINT index)
{
	m_uCurrentSample = index;
	CoverSample(index);
}

void CMultisampleTest_Depth::SetDepth(float depth)
{
	HRESULT hr;
	// Set Depth value
	float *bufData;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if( FAILED( hr = GetEffectiveContext()->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
		throw TRException("m_pConstBuffer->Map() Failed.", hr, RESULT_FAIL);
	bufData = (float*) mappedRes.pData;
	// Inverse viewport scale.
	bufData[2] = 2 / (float)g_TestApp.m_RenderTargetOptions.SwapChainDesc.Width;
	bufData[3] = -2 / (float)g_TestApp.m_RenderTargetOptions.SwapChainDesc.Height;
	// Prescaled inverse viewport translation.
	bufData[0] = -1.0f;
	bufData[1] = 1.0f;
	//alpha value;
	bufData[4] = 1.0f;
	bufData[5] = depth;  
	//pixel position
	bufData[6] = (float)m_vPixPos.x;
	bufData[7] = (float)m_vPixPos.y;

	GetEffectiveContext()->Unmap(m_pConstBuffer,0);
}

//////////////////////////////////////////////////////////////////////////
// CMultisampleTest_Depth
//////////////////////////////////////////////////////////////////////////


tstring __stdcall ToString( TEST_TYPE type )
{
	switch( type )
	{
		case NORMAL:
			return tstring( _T( "NORMAL" ) );
			break;
		case COMBO_MSOFF:
			return tstring( _T( "COMBO_MSOFF" ) );
			break;
		case COMBO_ODEPTH:
			return tstring( _T( "COMBO_ODEPTH" ) );
			break;
		default:
			return tstring( _T( "" ) );
			break;
	}
}

bool __stdcall FromString( TEST_TYPE *pType, const tstring &str )
{
	if( tcistring( str ) == _T( "NORMAL" ) )
	{
		*pType = NORMAL;
		return true;
	}
	
	if( tcistring( str ) == _T( "COMBO_MSOFF" ) )
	{
		*pType = COMBO_MSOFF;
		return true;
	}
	
	if( tcistring( str ) == _T( "COMBO_ODEPTH" ) )
	{
		*pType = COMBO_ODEPTH;
		return true;
	}
	
	return false;
}


//////////////////////////////////////////////////////////////////////////
// CMultisampleTest_DepthHelper
//
// Examples for using DepthHelper
//////////////////////////////////////////////////////////////////////////
extern DXGI_FORMAT DSFormats[];
extern UINT DSFormatsSize;
CMultisampleTest_DepthHelper::CMultisampleTest_DepthHelper()
{

	m_uSampleCount = 1;
}


void CMultisampleTest_DepthHelper::InitTestParameters()
{
	// common multisample test parameters
	InitMultisampleParameters();
	m_bDepthTest = true;

	CTableValueSet< DXGI_FORMAT > *pDSFormat = new CTableValueSet< DXGI_FORMAT >( DSFormats, sizeof(DXGI_FORMAT), DSFormatsSize );
	CTestCaseParameter< DXGI_FORMAT > *pDSFormatParam = AddParameter( _T( "Depth_Format" ), &m_DSFormat );
	testfactor::RFactor rfFormat = AddParameterValueSet< DXGI_FORMAT > ( pDSFormatParam, pDSFormat );

	SetRootTestFactor ( rfFormat );
}

void CMultisampleTest_DepthHelper::CreateDSTex()
{
	HRESULT hr;

	// params
	m_uArraySize = 4;
	m_uFirstSlice = 1;

	// cleanup
	SAFE_RELEASE( m_pDSTex2D );
	SAFE_RELEASE( m_pDSView );

	//
	// Create DepthStencil texture
	//
	D3D11_TEXTURE2D_DESC dsTexDesc;
	dsTexDesc.Width = g_TestApp.m_RenderTargetOptions.SwapChainDesc.Width;
	dsTexDesc.Height = g_TestApp.m_RenderTargetOptions.SwapChainDesc.Height;
	dsTexDesc.MipLevels = 1;
	dsTexDesc.ArraySize = m_uArraySize;
	dsTexDesc.Format = m_DSFormat;
	dsTexDesc.SampleDesc.Count = m_uSampleCount;
	dsTexDesc.SampleDesc.Quality = m_uSampleQuality;
	dsTexDesc.Usage = D3D11_USAGE_DEFAULT;
	dsTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsTexDesc.CPUAccessFlags = 0;
	dsTexDesc.MiscFlags = 0;
	if( FAILED( hr = GetFramework()->CreateTexture2DTiledOptional(&dsTexDesc, NULL, &m_pDSTex2D) ) )
		throw TRException( "CreateTexture2D() for m_pDSTex2D failed.", hr, RESULT_FAIL );

	D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
	dsViewDesc.Format = m_DSFormat;
	dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
	dsViewDesc.Texture2DMSArray.FirstArraySlice = m_uFirstSlice;
	dsViewDesc.Texture2DMSArray.ArraySize = m_uArraySize - m_uFirstSlice;
	if( FAILED( hr = m_pDevice->CreateDepthStencilView(m_pDSTex2D, &dsViewDesc, &m_pDSView) ) )
		throw TRException( "CreateDepthStencilView() failed.", hr, RESULT_FAIL );
}

TEST_RESULT CMultisampleTest_DepthHelper::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		D3D11_TEXTURE2D_DESC rtdesc;
		m_pRTTexture2D->GetDesc( &rtdesc );
		CreateDSTex();

		// generate depth data
		srand(0);
		UINT size = rtdesc.SampleDesc.Count * rtdesc.Width * rtdesc.Height * (m_uArraySize - m_uFirstSlice);
		std::vector<float> pData(size);
		for (UINT i = 0; i < size; ++i)
			pData[i] = rand()/(float)RAND_MAX;

		//
		// DepthHelper
		//
		DepthHelper helper;
		helper.Setup(GetFramework(), m_pDSView);
		helper.Fill(&pData[0], 4, 4, m_uSampleCount);

		// get sample locations
		if ( UpdateFormatOrSampleDesc() )
		{
			if( FAILED( hr = m_Finder.FindSamples() ) )
				throw TRException("FindSamples failed.", hr, RESULT_FAIL);
			if( FAILED( hr = m_Finder.OrderSamples() ) )
				throw TRException("OrderSamples failed.", hr, RESULT_FAIL);
			ResetStates();
		}
		
		UINT index = 0;
		for (UINT uSlice = m_uFirstSlice; uSlice < m_uArraySize; ++uSlice)
		{
			ID3D11DepthStencilView *pDSV;
			D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
			dsViewDesc.Format = m_DSFormat;
			dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
			dsViewDesc.Texture2DMSArray.FirstArraySlice = uSlice;
			dsViewDesc.Texture2DMSArray.ArraySize = 1;
			if( FAILED( hr = m_pDevice->CreateDepthStencilView(m_pDSTex2D, &dsViewDesc, &pDSV) ) )
				throw TRException( "CreateDepthStencilView() failed.", hr, RESULT_FAIL );

			// clear
			FLOAT ClearColor[4] = { 0, 0, 0, 0 };
			m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, pDSV );
			m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
			m_pDeviceContext->OMSetDepthStencilState( m_pDSState, 0 );
		
			for (UINT y = 0; y < rtdesc.Height; y++)
				for (UINT x = 0; x < rtdesc.Width; x++)
					for (m_uCurrentSample = 0; m_uCurrentSample < rtdesc.SampleDesc.Count; ++m_uCurrentSample)
					{
						// cover only this sample
						BOOL bIsWhite;
						m_uVertexCount = 0;
						CoverSample( x, y, m_uCurrentSample );
						float depth = pData[index];

						m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
						SetDepth( min(1.0f, depth + 0.01f) );
						BeginQuery();
						m_pDeviceContext->Draw(m_uVertexCount, 0);
						if( EndQuery() )
						{
							WriteToLog("Slice:%d, Sample:%d, x:%d, y:%d", uSlice, m_uCurrentSample, x, y);
							tRes = RESULT_FAIL;
						}

						m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
						SetDepth( max(0.f, depth - 0.01f) );
						BeginQuery();
						m_pDeviceContext->Draw(m_uVertexCount, 0);
						if( !EndQuery() )
						{
							WriteToLog("Slice:%d, Sample:%d, x:%d, y:%d", uSlice, m_uCurrentSample, x, y);
							tRes = RESULT_FAIL;
						}

						++index;
					}

			m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, NULL );
			SAFE_RELEASE( pDSV );
		}
		if (tRes != RESULT_PASS)
			throw TRException("", tRes);
	}

	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_DepthHelper::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}
	return tRes;
}

void CMultisampleTest_DepthHelper::BeginQuery()
{
	HRESULT hr;
	// setup query and predicate
	D3D11_QUERY_DESC desc;
	desc.Query = D3D11_QUERY_OCCLUSION;
	desc.MiscFlags = 0;
	if ( FAILED( hr = m_pDevice->CreateQuery( &desc, &m_pQuery ) ) ) 
		throw TRException("CreateQuery failed.", hr, RESULT_FAIL);

	m_pDeviceContext->Begin(m_pQuery);
}

UINT64 CMultisampleTest_DepthHelper::EndQuery()
{
	m_pDeviceContext->End(m_pQuery);
	UINT64 uQResult;
	HRESULT hr;

	// wait for results
	hr = GetFramework()->GetDataWithTimeout( m_pDeviceContext, m_pQuery, (void*)&uQResult, sizeof(UINT64), 0 );

	// bad result
	if (hr != S_OK)
	{
		WriteToLog("Query HRESULT = %s", D3DHResultToString(hr).c_str());
		throw TRException("Query failed or timed out.");
	}

	// cleanup
	SAFE_RELEASE( m_pQuery );

	return uQResult;
}	
