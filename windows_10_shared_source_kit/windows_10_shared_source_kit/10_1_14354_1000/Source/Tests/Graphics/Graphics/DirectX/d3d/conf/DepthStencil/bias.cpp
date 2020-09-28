//////////////////////////////////////////////////////////////////////////
//  bias.cpp
//  created:	2006/11/10
//
//  purpose:    conformance for depth bias in rasterizer state
//////////////////////////////////////////////////////////////////////////

#include "bias.h"
#include "formathelper.h"

// use partition equivalent and boundary value methodologies
INT DepthBiasValues[] =
{
	0,
    1,
    16,
	0xff,

	-1,
};

float DepthBiasClampValues[] = 
{
	1.0e-8f,
    0.0f,
	-1.0e-3f
};

float SlopeScaledValues[] =
{
	0.f,
	159.2f,
	-0.3f
};

CBiasTest::CBiasTest()
: CDepthTest()
{
	m_RSDesc.CullMode = D3D11_CULL_NONE;
	m_RSDesc.FrontCounterClockwise = true;
	m_RSDesc.DepthClipEnable = true;
	m_RSDesc.ScissorEnable = false;
	m_RSDesc.MultisampleEnable = false;
	m_RSDesc.AntialiasedLineEnable = false;
	m_RSDesc.FillMode = D3D11_FILL_SOLID;

}

void CBiasTest::InitTestParameters()
{
	//
	// Depth Bias
	//
	CTestCaseParameter<INT> *pDepthBiasParam = AddParameter( _T( "DepthBias" ), &m_RSDesc.DepthBias );
	CTableValueSet<INT> *pDepthBiasSet = new CTableValueSet<INT>( DepthBiasValues, sizeof(INT), ARRAY_SIZE( DepthBiasValues ) );
	testfactor::RFactor rfDepthBias = AddParameterValueSet( pDepthBiasParam, pDepthBiasSet );

	//
	// Slope Scaled Depth Bias
	//
	CTestCaseParameter<float> *pSlopeParam = AddParameter( _T( "SlopeScaled" ), &m_RSDesc.SlopeScaledDepthBias );
	CTableValueSet<float> *pSlopeSet = new CTableValueSet<float>( SlopeScaledValues, sizeof(float), ARRAY_SIZE( SlopeScaledValues ) );
	testfactor::RFactor rfSlopeScaledBias = AddParameterValueSet( pSlopeParam, pSlopeSet );

	CTestCaseParameter<float> *pBiasClamp = AddParameter( _T( "BiasClamp" ), &m_RSDesc.DepthBiasClamp );
	CTableValueSet<float> *pBiasClampSet = new CTableValueSet<float>( DepthBiasClampValues, sizeof(float), ARRAY_SIZE( DepthBiasClampValues ) );
	testfactor::RFactor rfBiasClamp = AddParameterValueSet( pBiasClamp, pBiasClampSet );

	//
	// Set Root Factor
	//
	InitDepthTestParameters();
	SetRootTestFactor(m_rfAll * rfSlopeScaledBias * rfBiasClamp * rfDepthBias);

	AddPriorityPatternFilter(FilterEqual<D3D11_DSV_DIMENSION>(_T("Dimension"), D3D11_DSV_DIMENSION_TEXTURE2D), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Width"), 128), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Height"), 256), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

TEST_RESULT CBiasTest::Setup()
{
	TEST_RESULT tRes = RESULT_PASS;

	try
	{
		SetupCommon();
				
		// unbind and release
		GetEffectiveContext()->RSSetState( NULL );

		HRESULT hr;
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context.") );
			return RESULT_FAIL;
		}

		SAFE_RELEASE( m_pRS );

	}
	catch(TRException &exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CBiasTest::Setup()] ");
		if (tRes != RESULT_PASS)
			WriteToLog( exc.GetError().c_str() );
	}
	return tRes;
}

TEST_RESULT CBiasTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		SetupTestCaseCommon();

		//
		// Rasterizer State
		//

		// rasterizer state with test params
		if( FAILED( GetDevice()->CreateRasterizerState( &m_RSDesc, &m_pRS ) ) )
			throw TRException("CreateRasterizerState for m_pRSEnabled failed.");
		GetEffectiveContext()->RSSetState( m_pRS );

		HRESULT hr;
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context.") );
			return RESULT_FAIL;
		}

	}
	catch(TRException &exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CBiasTest::SetupTestCase()] ");
		if (tRes != RESULT_PASS)
			WriteToLog( exc.GetError().c_str() );
	}
	return tRes;
}

int exponent( float f ) { return (((*(UINT*)&f) >> 23) & 0xff) - 127; }

float ConvertToDepthFormat(float fDepth, DXGI_FORMAT format)
{
	float fOut;
	switch( format )
	{
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_D32_FLOAT:
		fOut = fDepth;
		break;
	case DXGI_FORMAT_D16_UNORM:
		fOut = (UINT)(fDepth * (double)(0x10000)) / (float)0x10000;
		break;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		fOut = (UINT)(fDepth * (double)(0x1000000)) / (float)0x1000000;
		break;
	}
	return fOut;
}

TEST_RESULT CBiasTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{

		UINT uSize = m_uSubResWidth*m_uSubResHeight;
		std::vector<float> vInitial(uSize);
		std::vector<float> vMaxDepthSlope(uSize, 0);
		const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(m_DSTex2DDesc.Format);

		//
		// Init Depth Texture
		//

		// generate values and fill vertex buffer
		srand(1);
		for (UINT i = 0; i < vInitial.size(); ++i)
			vInitial[i] = rand()/(float)RAND_MAX;
		FillVB(vInitial);
		// add this: //FillVBBias(vInitial, vMaxDepthSlope);

		// render vertex buffer
		GetEffectiveContext()->OMSetDepthStencilState( m_pDSS, 0 );
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTV, m_pDSV );
		GetEffectiveContext()->ClearDepthStencilView( m_pDSV, D3D11_CLEAR_DEPTH, 0.25f, 0 );
		GetEffectiveContext()->Draw( uSize*3, 0 );

		HRESULT hr;
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context.") );
			return RESULT_FAIL;
		}

		//
		// Verify
		// 
		std::vector<float> vActual(uSize);
		GetDepth(vActual);
		for (UINT i = 0; i < vActual.size(); ++i)
		{
			// From spec:
			// Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope

			// minimum representable value by format
			float fR;
			if( pTexFormatInfo->dwFlags & FF_UNORM )
				fR = 1.f/(float)(0xffffffff >> (32-pTexFormatInfo->pBitsPerComponent[0]));
			else 
				fR = pow(2.f, max(exponent(vInitial[i]) - 23, -127.f));

			// bias
			float fMaxDepthSlope = 0.f; // todo
			float fBias = (float)m_RSDesc.DepthBias * fR + m_RSDesc.SlopeScaledDepthBias * vMaxDepthSlope[i];

			// clamp
			if(m_RSDesc.DepthBiasClamp > 0)
				fBias = min(m_RSDesc.DepthBiasClamp, fBias);
			else if(m_RSDesc.DepthBiasClamp < 0)
				fBias = max(m_RSDesc.DepthBiasClamp, fBias);

			// compute expected
			float fExpected;
			if (m_PSType != ODEPTH)
				fExpected = min( max(vInitial[i] + fBias, 0), 1 );				
			else 
				fExpected = min( max(vInitial[i], 0), 1 );

			// convert to buffer format
			fExpected = ConvertToDepthFormat(fExpected, m_DSTex2DDesc.Format);

			// compare expected results to actual results
			float fUlpDiff;
			if(!MinMaxRange::TestFloatsSame(fExpected, vActual[i], pTexFormatInfo, 0, 3.f, &fUlpDiff) )
			{
				WriteToLog("Failure Info:");
				WriteToLog("Pixel x: %d, y: %d", i%m_uSubResWidth, i/m_uSubResWidth);
				WriteToLog("Initial value: \t\t%f", vInitial[i]);
				WriteToLog("Expected value: \t%f", fExpected);
				WriteToLog("Actual value: \t%f", vActual[i]);

				if (fUlpDiff < 50) // spew ulpdiff only if it appears to be a precision issue
					WriteToLog("Ulp difference: \t%+ f", fUlpDiff);
				throw TRException("Depth buffer contains invalid results.");
			}
		}
	}
	catch(TRException &exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CBiasTest::ExecuteTestCase()] ");
		if (tRes != RESULT_PASS) 
			WriteToLog( exc.GetError().c_str() );
	}
	return tRes;
}

void CBiasTest::FillVBBias(const std::vector<float> &vData, std::vector<float> &vMaxDepthSlope)
{
	assert(vData.size() == m_uSubResHeight*m_uSubResWidth);
	assert(vMaxDepthSlope.size() == m_uSubResHeight*m_uSubResWidth);

	HRESULT hr;
	VERTEX *pVert;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	// Fill vertex buffer
	if( FAILED( hr = GetEffectiveContext()->Map( m_pVB, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
	{
		WriteToLog( _T( "Map() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CDepthTest::FillDepth] Map on m_pVB failed.");
	}
	pVert = (VERTEX*) mappedRes.pData;

	srand(0);
	for (UINT y = 0; y < m_uSubResHeight; ++y)
		for (UINT x = 0; x < m_uSubResWidth; ++x)
		{	
			// create triangle
			float depth = vData[x + y * m_uSubResWidth];
			pVert[0].x = x + 0.5f;
			pVert[0].y = y + 0.5f;
			pVert[0].z = depth;

			// randomly change the 
			pVert[1].x = x + 0.6f;
			pVert[1].y = y + 0.5f;
			pVert[1].z = rand()/(float)RAND_MAX;

			pVert[2].x = x + 0.5f;
			pVert[2].y = y + 0.6f;
			pVert[2].z = rand()/(float)RAND_MAX;
			
			// max depth slope			
			float dzdx = fabsf( (pVert[1].z - pVert[0].z)/0.1f );
			float dzdy = fabsf( (pVert[2].z - pVert[0].z)/0.1f );
			vMaxDepthSlope[x + y * m_uSubResWidth] = max( dzdx, dzdy );

			pVert += 3;
		}

	GetEffectiveContext()->Unmap(m_pVB,0);

	// rebind
	UINT strides[] = { sizeof( VERTEX ) };
	UINT offsets[] = { 0 };
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVB, strides, offsets );
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
	}
}

void CBiasTest::CleanupTestCase()
{
	GetEffectiveContext()->RSSetState( NULL );
	HRESULT hr;
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
	}

	SAFE_RELEASE(m_pRS);
	CleanupTestCaseCommon();
}

void CBiasTest::Cleanup()
{
	CleanupCommon();
}

