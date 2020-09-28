//////////////////////////////////////////////////////////////////////////
//  clamp.cpp
//  created:	2006/11/10
//
//  purpose:    conformance for depth clamp in the viewport struct
//////////////////////////////////////////////////////////////////////////

#include "clamp.h"

// string converstions for custom types
tstring __stdcall ToString( DEPTH_VALUE value );
bool __stdcall FromString( DEPTH_VALUE *value, const tstring &str );

// data for value sets
// use partition equivalent and boundary value methodologies
float MinMaxDepth[][2] = 
{
	{ 0.0f, 1.0f },		// default
	
	{ 0.25f, 0.75f },	// various valid values

	{ 0.0f, 0.0f },		// same values
	{ 1.0f, 1.0f },     // same value
};

DEPTH_VALUE DepthValues[] =
{
	MIN_DEPTH,
	MAX_DEPTH,
	MID_DEPTH,
	NEAR_MIN_DEPTH,
	NEAR_MAX_DEPTH,
	PAST_MIN_DEPTH,
	PAST_MAX_DEPTH
};

CClampTest::CClampTest()
: CDepthTest()
{

}

void CClampTest::InitTestParameters()
{
	//
	// Min and Max Depth 
	//
	CTestCaseParameter<FLOAT> *pMinDepthParam = AddParameter( _T( "MinDepth" ), &m_VP.MinDepth );
	CTestCaseParameter<FLOAT> *pMaxDepthParam = AddParameter( _T( "MaxDepth" ), &m_VP.MaxDepth );
	CUserValueSet<FLOAT> *pMinDepthSet = new CUserValueSet<FLOAT>();
	CUserValueSet<FLOAT> *pMaxDepthSet = new CUserValueSet<FLOAT>();
	for (UINT i = 0; i < ARRAY_SIZE(MinMaxDepth); ++i)
	{
		pMinDepthSet->AddValue(MinMaxDepth[i][0]);
		pMaxDepthSet->AddValue(MinMaxDepth[i][1]);
	}
	testfactor::RFactor rfMinDepth = AddParameterValueSet( pMinDepthParam, pMinDepthSet );
	testfactor::RFactor rfMaxDepth = AddParameterValueSet( pMaxDepthParam, pMaxDepthSet );

	//
	// Depth test values
	//
	CTestCaseParameter<DEPTH_VALUE> *pDepthParam = AddParameter( _T( "DepthValue" ), &m_DepthValue );
	CTableValueSet<DEPTH_VALUE> *pDepthSet = new CTableValueSet< DEPTH_VALUE >( DepthValues, sizeof(DEPTH_VALUE), ARRAY_SIZE( DepthValues ) );
	testfactor::RFactor rfDepthValue = AddParameterValueSet( pDepthParam, pDepthSet );

	InitDepthTestParameters();
	SetRootTestFactor(m_rfAll * (rfMinDepth % rfMaxDepth) * rfDepthValue);

	AddPriorityPatternFilter(FilterEqual<D3D11_DSV_DIMENSION>(_T("Dimension"), D3D11_DSV_DIMENSION_TEXTURE2D), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Width"), 128), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Height"), 256), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

TEST_RESULT CClampTest::Setup()
{
	TEST_RESULT tRes = RESULT_PASS;

	try
	{
		SetupCommon();
	}
	catch(TRException &exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CClampTest::Setup()] ");
		if (tRes != RESULT_PASS)
			WriteToLog( exc.GetError().c_str() );
	}
	return tRes;
}

TEST_RESULT CClampTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		SetupTestCaseCommon();

		//
		// Setup the default viewport
		//
		m_VP.Height = (float) m_uSubResHeight;
		m_VP.Width = (float) m_uSubResWidth;
		m_VP.TopLeftX = 0;
		m_VP.TopLeftY = 0;
		GetEffectiveContext()->RSSetViewports( 1, &m_VP );

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
		exc.Prepend("[CClampTest::SetupTestCase()] ");
		if (tRes != RESULT_PASS)
			WriteToLog( exc.GetError().c_str() );
	}
	return tRes;
}

TEST_RESULT CClampTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		//
		// Init Depth Texture
		//

		// determine input depth value
		float fDepthIn;
		switch (m_DepthValue)
		{
			case MIN_DEPTH: fDepthIn = m_VP.MinDepth; break;
			case MAX_DEPTH: fDepthIn = m_VP.MaxDepth; break;
			case MID_DEPTH: fDepthIn = 0.5f*(m_VP.MaxDepth*m_VP.MinDepth); break;
			case NEAR_MIN_DEPTH: fDepthIn = m_VP.MinDepth + 0.00001f; break;
			case NEAR_MAX_DEPTH: fDepthIn = m_VP.MaxDepth - 0.00001f; break;
			case PAST_MIN_DEPTH: fDepthIn = m_VP.MinDepth - 0.00001f; break;
			case PAST_MAX_DEPTH: fDepthIn = m_VP.MaxDepth + 0.00001f; break;
		}
		
		// create depth buffer
		UINT uSize = m_uSubResWidth*m_uSubResHeight;
		std::vector<float> vDepthIn(uSize, fDepthIn);
		FillVB(vDepthIn);

		//
		// Render
		//
		float fClear = 0.25f;
		GetEffectiveContext()->OMSetDepthStencilState( m_pDSS, 0 );
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTV, m_pDSV );
		GetEffectiveContext()->ClearDepthStencilView( m_pDSV, D3D11_CLEAR_DEPTH, fClear, 0 );
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
		
		// get depth
		std::vector<float> vDepthOut(uSize);
		GetDepth(vDepthOut);

		// compute expected result
		//  -cull pixel if out of range  
		//	-viewport scale
		//  -viewport clamp
		float fExpected = fDepthIn;
		if (m_PSType != ODEPTH)
			fExpected = m_VP.MinDepth + fExpected*(m_VP.MaxDepth - m_VP.MinDepth);
		fExpected = min(m_VP.MaxDepth, max(m_VP.MinDepth, fExpected));
		if ((m_PSType != ODEPTH) && ((fDepthIn < 0) || (fDepthIn > 1)) )
			fExpected = fClear;

		// compare
		const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(m_DSTex2DDesc.Format);
		for (UINT i = 0; i < vDepthIn.size(); ++i)
		{
			float fUlpDiff;
			if(!MinMaxRange::TestFloatsSame(fExpected, vDepthOut[0], pTexFormatInfo, 0, 1.f, &fUlpDiff) )
			{
				WriteToLog("Failure Info:");
				WriteToLog("Pixel x: %d, y: %d", i%m_uSubResWidth, i/m_uSubResWidth);
				WriteToLog("Input Depth: \t%f", fDepthIn);
				WriteToLog("Expected value: \t%f", fExpected);
				WriteToLog("Actual value: \t%f", vDepthOut[i]);

				if (fUlpDiff < 50) // spew ulpdiff only if it appears to be a precision issue
					WriteToLog("Ulp difference: \t%+ f", fUlpDiff);
				throw TRException("Depth buffer contains invalid results.");
			}
		}
	}
	catch(TRException &exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CClampTest::ExecuteTestCase()] ");
		if (tRes != RESULT_PASS)
			WriteToLog( exc.GetError().c_str() );
	}
	return tRes;
}

void CClampTest::CleanupTestCase()
{
	CleanupTestCaseCommon();
}

void CClampTest::Cleanup()
{
	CleanupCommon();
}

tstring __stdcall ToString( DEPTH_VALUE value )
{
	switch( value )
	{
		case MIN_DEPTH:
			return tstring( _T( "MIN_DEPTH" ) );
			break;
		case MAX_DEPTH:
			return tstring( _T( "MAX_DEPTH" ) );
			break;
		case MID_DEPTH:
			return tstring( _T( "MID_DEPTH" ) );
			break;
		case NEAR_MIN_DEPTH:
			return tstring( _T( "NEAR_MIN_DEPTH" ) );
			break;
		case NEAR_MAX_DEPTH:
			return tstring( _T( "NEAR_MAX_DEPTH" ) );
			break;
		case PAST_MIN_DEPTH:
			return tstring( _T( "PAST_MIN_DEPTH" ) );
			break;
		case PAST_MAX_DEPTH:
			return tstring( _T( "PAST_MAX_DEPTH" ) );
			break;
		default:
			return tstring( _T( "" ) );
			break;
	}
}

bool __stdcall FromString( DEPTH_VALUE *value, const tstring &str )
{
	if( tcistring( str ) == _T( "MIN_DEPTH" ) )
	{
		*value = MIN_DEPTH;
		return true;
	}
	if( tcistring( str ) == _T( "MAX_DEPTH" ) )
	{
		*value = MAX_DEPTH;
		return true;
	}
	if( tcistring( str ) == _T( "MID_DEPTH" ) )
	{
		*value = MID_DEPTH;
		return true;
	}
	if( tcistring( str ) == _T( "NEAR_MIN_DEPTH" ) )
	{
		*value = NEAR_MIN_DEPTH;
		return true;
	}
	if( tcistring( str ) == _T( "NEAR_MAX_DEPTH" ) )
	{
		*value = NEAR_MAX_DEPTH;
		return true;
	}
	if( tcistring( str ) == _T( "PAST_MIN_DEPTH" ) )
	{
		*value = PAST_MIN_DEPTH;
		return true;
	}
	if( tcistring( str ) == _T( "PAST_MAX_DEPTH" ) )
	{
		*value = PAST_MAX_DEPTH;
		return true;
	}
	return false;
}
