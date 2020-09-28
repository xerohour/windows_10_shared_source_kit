//////////////////////////////////////////////////////////////////////////
//  Occlusion.cpp
//  created:	2005/07/18
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "occlusion.h"

DXGI_FORMAT DepthFormats[] =
{
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
	DXGI_FORMAT_D32_FLOAT,
	DXGI_FORMAT_D24_UNORM_S8_UINT,
	DXGI_FORMAT_D16_UNORM
};

void CMultisampleTest_Occ::InitTestParameters()
{
	InitMultisampleParameters();
	CUserValueSet< D3D11_COMPARISON_FUNC > *pCompFuncValues = new CUserValueSet< D3D11_COMPARISON_FUNC >();
	pCompFuncValues->AddValue( D3D11_COMPARISON_NEVER );
	pCompFuncValues->AddValue( D3D11_COMPARISON_ALWAYS );

	testfactor::RFactor ds = AddParameter( _T( "DepthStencil" ), &g_TestApp.m_RenderTargetOptions.DepthStencilDesc.DepthFunc, 
		pCompFuncValues );

	SetRootTestFactor ( m_rfSampleDesc * ( m_rfRTFormatTypedSubset % m_rfPixPos) * ds  );
}

TEST_RESULT CMultisampleTest_Occ::SetupDS()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	g_TestApp.m_RenderTargetOptions.NoDepthStencils = false;
	g_TestApp.m_RenderTargetOptions.UseDefaultDepthStencilView = false;

	try {
		// Choose first depth format that supports multisample
		UINT i = 0;
		DXGI_FORMAT format;
		for (; i < ARRAY_SIZE(DepthFormats); i++)
		{
			format = DepthFormats[i];

			UINT levels;
			if( FAILED( hr = m_pDevice->CheckMultisampleQualityLevels( DepthFormats[i], m_uSampleCount, &levels ) ) )
				throw TRException("CheckMultisampleQualityLevels failed.", hr, RESULT_FAIL);

			UINT caps;
			m_pDevice->CheckFormatSupport( DepthFormats[i], &caps );

			UINT uQuality = m_uSampleQuality == D3D11_STANDARD_MULTISAMPLE_PATTERN ? 0 : m_uSampleQuality;
			if ((caps & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET) && (levels > uQuality))
				break;
		}

		// abort
		if (i == ARRAY_SIZE(DepthFormats))
			throw TRException("No depth formats support multisample and current quality level.", TEST_RESULT(RESULT_SKIP));

		D3D11_TEXTURE2D_DESC &dsTexDesc = g_TestApp.m_RenderTargetOptions.DepthTextureDesc;
		dsTexDesc.Width = m_RTWidth;
		dsTexDesc.Height = m_RTHeight;
		dsTexDesc.MipLevels = 1;
		dsTexDesc.ArraySize = 1;
		dsTexDesc.Format = format;
		dsTexDesc.Usage = D3D11_USAGE_DEFAULT;
		dsTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsTexDesc.CPUAccessFlags = 0;
		dsTexDesc.MiscFlags = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC &dsViewDesc = g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc;
		dsViewDesc.Format = format;

		if ( m_uSampleCount > 1 )
			dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		else
			dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		g_TestApp.CleanupRenderTarget();
		TEST_RESULT ts = g_TestApp.SetupRenderTarget();
		if (ts == RESULT_FAIL)
			throw TRException("SetupRenderTargets() failed.", TEST_RESULT(RESULT_FAIL));	

	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Occ::Setup()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}
TEST_RESULT CMultisampleTest_Occ::SetupTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;
	
	tRes = SetFormatAndQualityLevel( false );
	if ( tRes != RESULT_PASS )
	{
		return tRes;
	}
	tRes = SetupDS( );
	if ( tRes != RESULT_PASS )
	{
		WriteToLog(_T( "CMultisampleTest_Occ::SetupTestCase(): SetupDS() failed."));
		return tRes;
	}

	m_bDS = g_TestApp.m_RenderTargetOptions.DepthStencilDesc.DepthFunc == D3D11_COMPARISON_ALWAYS;
	m_pDSView = g_TestApp.GetDSView();
	
	try {
		SetupD3D();

		D3D11_DEPTH_STENCIL_DESC &dsDesc = g_TestApp.m_RenderTargetOptions.DepthStencilDesc;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		if( FAILED( hr = m_pDevice->CreateDepthStencilState(&dsDesc, &m_pDSState) ) )
			throw TRException("CreateDepthStencilState() failed.", hr, RESULT_FAIL);

		//UpdateSampleLocations();
		if ( UpdateFormatOrSampleDesc() )
		{
			if( FAILED( hr = m_Finder.FindSamples() ) )
				throw TRException("FindSamples failed.", hr, RESULT_FAIL);

			ResetStates();
		}
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Occ::SetupTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}

void CMultisampleTest_Occ::CleanupTestCase()
{
	m_pDeviceContext->OMSetDepthStencilState( NULL, 0 );

	CMultisampleTest::CleanupD3D();

	SAFE_RELEASE( m_pDSState );
}

TEST_RESULT CMultisampleTest_Occ::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;
	D3D11_MAPPED_SUBRESOURCE texmap;

	try
	{
		Begin();

		// setup geometry
		m_uVertexCount = 0;
		CoverPixel();

		// clear
		FLOAT ClearColor[4] = { 0, 0, 0, 0 };
		m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, m_pDSView );
		m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
		m_pDeviceContext->OMSetDepthStencilState( m_pDSState, 0 );
			
		m_pDeviceContext->Draw(m_uVertexCount, 0);
		m_pDeviceContext->Draw(m_uVertexCount, 0);
		m_pDeviceContext->Draw(m_uVertexCount, 0);

		// validate
		if ( Validate() == RESULT_FAIL ) 
			throw TRException("Occlusion validation failed.\n");

		// copy to resolve texture (if format supports resolve)
		if (g_TestApp.m_bPresent)
		{
			BOOL bResult;
			if( RESULT_PASS != m_Finder.TestPixelNonZero(bResult) )
				throw TRException("TestPixelNonZero failed.");
		}
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Occ::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}

	try
	{
		Present();
	}
	catch (TRException& exc)
	{
		exc.Prepend("[CMultisampleTest_Occ::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}

	return tRes;	
}

TEST_RESULT CMultisampleTest_OccQuery::Begin()
{
	HRESULT hr;
	// setup query and predicate
	D3D11_QUERY_DESC desc;
	desc.Query = D3D11_QUERY_OCCLUSION;
	desc.MiscFlags = 0;
	if ( FAILED( hr = m_pDevice->CreateQuery( &desc, &m_pQuery ) ) ) 
		throw TRException("[CMultisampleTest_OccQuery::Begin()] CreateQuery failed.", hr, RESULT_FAIL);
		
	GetEffectiveContext()->Begin(m_pQuery);
	return RESULT_PASS;
}

TEST_RESULT CMultisampleTest_OccQuery::Validate()
{
	GetEffectiveContext()->End(m_pQuery);

	UINT64 uQResult;
	HRESULT hr;

	if ( FAILED( hr = ExecuteEffectiveContext() ) )
		throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

	// wait for results
	hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pQuery, (void*)&uQResult, sizeof(UINT64), 0 );

	// bad result
	if (hr != S_OK)
	{
		WriteToLog("Query HRESULT = %s", D3DHResultToString(hr).c_str());
		throw TRException("[CMultisampleTest_OccQuery::Validate()] Query failed or timed out.", hr, RESULT_FAIL);
	}

	// TODO: add validation
	UINT64 uExpected = m_uSampleCount * 3;
	if (!m_bDS) uExpected = 0;

	SAFE_RELEASE( m_pQuery );

	if (uQResult != uExpected)
	{
		WriteToLog("Query returned %d, expected %d", uQResult, uExpected);
		throw TRException("[CMultisampleTest_OccQuery::Validate()] Expected result does not match query result.");
	}

	return RESULT_PASS;
}

TEST_RESULT CMultisampleTest_OccPredicate::Begin()
{
	HRESULT hr;
	// setup query and predicate
	D3D11_QUERY_DESC desc;
	desc.Query = D3D11_QUERY_OCCLUSION_PREDICATE;
	desc.MiscFlags = 0;
	if ( FAILED( hr = m_pDevice->CreatePredicate( &desc, &m_pPredicate ) ) ) 
		throw TRException("[CMultisampleTest_OccPredicate::Begin()] CreatePredicate failed.", hr, RESULT_FAIL);

	GetEffectiveContext()->Begin(m_pPredicate);
	return RESULT_PASS;
}

TEST_RESULT CMultisampleTest_OccPredicate::Validate()
{
	HRESULT hr;
	GetEffectiveContext()->End(m_pPredicate);

	if ( FAILED( hr = ExecuteEffectiveContext() ) )
		throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

	BOOL bResult;
	// wait for results
	hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pPredicate, (void*)&bResult, sizeof(BOOL), 0 );

	// bad result
	if (hr != S_OK)
	{
		WriteToLog("Query HRESULT = %s", D3DHResultToString(hr).c_str());
		throw TRException("[CMultisampleTest_OccPredicate::Validate()] Query failed or timed out.", hr, RESULT_FAIL);
	}



	SAFE_RELEASE( m_pPredicate );
	
	if (bResult != (BOOL)m_bDS)
	{
		WriteToLog("Query returned: %d, expected: %d.", bResult, m_bDS);
		throw TRException("[CMultisampleTest_OccPredicate::Validate()] Expected result does not match predicate result.");
	}

	return RESULT_PASS;
}
