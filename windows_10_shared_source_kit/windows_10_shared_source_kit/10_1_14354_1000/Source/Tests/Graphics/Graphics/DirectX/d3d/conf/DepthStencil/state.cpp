//////////////////////////////////////////////////////////////////////////
//  state.cpp
//  created:	2006/11/10
//
//  purpose:    conformance for depth parameters in the depth/stencil state
//////////////////////////////////////////////////////////////////////////

#include "state.h"
#include "formathelper.h"

CStateTest::CStateTest()
: CDepthTest(),
  m_pDSSAlways(NULL),
  m_ulpLimits(false)
{

}

void CStateTest::InitTestParameters()
{
	InitDepthTestParameters();

	//
	// DepthEnable
	//
	CTestCaseParameter<BOOL> *pDepthEnableParam = AddParameter( _T("DepthEnable"), &m_DSDesc.DepthEnable );
	CUserValueSet<BOOL> *pDepthEnableSet = new CUserValueSet<BOOL>();
	pDepthEnableSet->AddValue(TRUE);
	pDepthEnableSet->AddValue(FALSE);
	testfactor::RFactor rfDepthEnable = AddParameterValueSet( pDepthEnableParam, pDepthEnableSet );

	testfactor::RFactor rfDepthEnableTrue = AddParameterValue<BOOL>( pDepthEnableParam, TRUE );

	//
	// DepthWriteMask
	//
	CTestCaseParameter<D3D11_DEPTH_WRITE_MASK> *pDepthWriteMaskParam = AddParameter( _T("DepthWriteMask"), &m_DSDesc.DepthWriteMask );
	CUserValueSet<D3D11_DEPTH_WRITE_MASK> *pDepthWriteValues = new CUserValueSet<D3D11_DEPTH_WRITE_MASK>( );
	pDepthWriteValues->AddValue( D3D11_DEPTH_WRITE_MASK_ZERO );
	pDepthWriteValues->AddValue( D3D11_DEPTH_WRITE_MASK_ALL  );
	testfactor::RFactor rfDepthWriteMask = AddParameterValueSet( pDepthWriteMaskParam, pDepthWriteValues );

	testfactor::RFactor rfDepthWriteMaskAll = AddParameterValue<D3D11_DEPTH_WRITE_MASK>( pDepthWriteMaskParam, D3D11_DEPTH_WRITE_MASK_ALL );

	//
	// DepthStencilView flag
	//
	CTestCaseParameter<UINT> *pViewFlag = AddParameter( _T("DepthStencilViewDesc_Flags"), &m_DSVDesc.Flags );
	pViewFlag->SetDefaultValue( 0 );
	CUserValueSet<UINT> *pViewFlagValueSet = new CUserValueSet<UINT>();
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ) // not available for 10_*
	{
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH );
	}
	pViewFlagValueSet->AddValue( 0 );
	testfactor::RFactor rfViewFlag = AddParameterValueSet( pViewFlag, pViewFlagValueSet );

	//
	// DepthFunc
	//
	CTestCaseParameter<D3D11_COMPARISON_FUNC> *pDepthFuncParam = AddParameter( _T("DepthFunc"), &m_DSDesc.DepthFunc );
	CUserValueSet<D3D11_COMPARISON_FUNC> *pDepthFuncValues = new CUserValueSet<D3D11_COMPARISON_FUNC>( );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_NEVER );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_LESS );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_EQUAL );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_LESS_EQUAL );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_GREATER );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_NOT_EQUAL );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_GREATER_EQUAL );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_ALWAYS );
	testfactor::RFactor rfDepthFunc = AddParameterValueSet( pDepthFuncParam, pDepthFuncValues );

	//
	// ULP Limit testing
	//
	CTestCaseParameter<bool> *pULPLimitParam = AddParameter( _T("ULPLimits"), &m_ulpLimits );

	testfactor::RFactor rfULPLimitTrue = AddParameterValue<bool>( pULPLimitParam, true );

	testfactor::RFactor rfULPLimitFalse = AddParameterValue<bool>( pULPLimitParam, false );

	//
	// Root
	//
	testfactor::RFactor noULPLimitTests = m_rfAll * rfDepthFunc * rfULPLimitFalse * rfDepthWriteMask * rfViewFlag * rfDepthEnable;
	testfactor::RFactor ulpLimitTests = (m_rfReduced * rfDepthFunc * rfULPLimitTrue) * rfDepthWriteMaskAll * rfDepthEnableTrue;

	SetRootTestFactor(ulpLimitTests + noULPLimitTests);

	AddPriorityPatternFilter(FilterEqual<D3D11_DSV_DIMENSION>(_T("Dimension"), D3D11_DSV_DIMENSION_TEXTURE2D), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Width"), 128), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Height"), 256), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

TEST_RESULT CStateTest::Setup()
{
	TEST_RESULT tRes = RESULT_PASS;

	try
	{
		SetupCommon();
		
		//
		// DepthStencil state
		//

		// dss state that always passes
		D3D11_DEPTH_STENCIL_DESC dsDesc; 
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		dsDesc.StencilEnable = false;
		if( FAILED( GetDevice()->CreateDepthStencilState( &dsDesc, &m_pDSSAlways ) ) )
			throw TRException("CreateDepthStencilState() failed.");

	}
	catch(TRException &exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CStateTest::Setup()] ");
		if (tRes != RESULT_PASS)
			WriteToLog( exc.GetError().c_str() );
	}
	return tRes;
}

TEST_RESULT CStateTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		SetupTestCaseCommon();
	
		//
		// DepthStencil state
		//
	
		// release if it exists
		GetEffectiveContext()->OMSetDepthStencilState( NULL, 0 );
		SAFE_RELEASE( m_pDSS );

		// dss state with test params
		if( FAILED( GetDevice()->CreateDepthStencilState( &m_DSDesc, &m_pDSS ) ) )
			throw TRException("CreateDepthStencilState() failed.");
		GetEffectiveContext()->OMSetDepthStencilState( m_pDSS, 0 );
			
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
		exc.Prepend("[CStateTest::SetupTestCase()] ");
		if (tRes != RESULT_PASS)
			WriteToLog( exc.GetError().c_str() );
	}
	return tRes;
}

TEST_RESULT CStateTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	bool bFailCase = false;
	UINT failureCount = 0;
	UINT uSize = m_uSubResWidth*m_uSubResHeight;
	std::vector<float> vDepth0(uSize);
	std::vector<float> vDepth1(uSize);
	std::vector<float> vDepth0_quantized(uSize);
	std::vector<float> vDepth1_quantized(uSize);
	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(m_DSTex2DDesc.Format);

	// Temp buffers
	FLOAT_COLOR* pTempColorData = new FLOAT_COLOR[uSize];
	BYTE* pTempDSFormatData = new BYTE[uSize * 4 * 4];

	try
	{
		//
		// Init Depth Texture
		//

		// generate values and fill vertex buffer
		srand(1);
		for (UINT i = 0; i < uSize; ++i)
			vDepth0[i] = rand()/(float)RAND_MAX;

		// We want to fill the first couple pixels with intersting values based on the format used
		int numDepthBits = 0;
		float maxValue = 0;
		float stepSize = 0;

		switch( m_DSTex2DDesc.Format )
		{
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			numDepthBits = 32;
			break;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			numDepthBits = 24;
			break;
		case DXGI_FORMAT_D16_UNORM:
			numDepthBits = 16;
			break;
		default:
			throw TRException("Unknown format. Please make sure to add any new formats to the list.");
		}

		// Calculate the step size from the number of bits
		maxValue = pow(2.0f, numDepthBits) - 1;
		stepSize = 1.0f / maxValue;

		// Special values (if we are doing limit testing)
		if( m_ulpLimits && uSize >= 10)
		{
			// Start off loose
			vDepth0[0] = stepSize * (1.3f);
			vDepth0[1] = stepSize * (1.39f);
			
			vDepth0[2] = stepSize * (1.61f);
			vDepth0[3] = stepSize * (1.7f);

			vDepth0[4] = stepSize * (1.99f);
			vDepth0[5] = stepSize * (2.01f);
			
			// Getting a little tighter
			vDepth0[6] = stepSize * (1.399f);
			vDepth0[7] = stepSize * (0.601f);	// .6 ULP limit - to 2

			vDepth0[8] = stepSize * (1.25f);
			vDepth0[9] = stepSize * (1.601f);
		}

		FillVB(vDepth0);

		// render vertex buffer
		HRESULT hr;
		GetEffectiveContext()->OMSetDepthStencilState( m_pDSSAlways, 0 );
		GetEffectiveContext()->ClearDepthStencilView( m_pDSVWriteable, D3D11_CLEAR_DEPTH, 0.25f, 0 );
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTV, m_pDSVWriteable );

		GetEffectiveContext()->Draw( uSize*3, 0 );

		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context.") );
			return RESULT_FAIL;
		}
			
		//
		//	Depth Test
		//
		
		// generate values and fill vertex buffer
		srand(2);
		for (UINT i = 0; i < uSize; ++i)
			vDepth1[i] = rand()/(float)RAND_MAX;

		// Special values (if we are doing limit testing) - Based on the stepSize used for vDepth0
		if( m_ulpLimits && uSize >= 10 )
		{
			// Start off loose
			vDepth1[0] = stepSize * (1.7f);
			vDepth1[1] = stepSize * (1.61f);
			
			vDepth1[2] = stepSize * (1.39f);
			vDepth1[3] = stepSize * (1.3f);

			vDepth1[4] = stepSize * (2.01f);
			vDepth1[5] = stepSize * (1.99f);
			
			// Getting a little tighter
			vDepth1[6] = stepSize * (0.601f);
			vDepth1[7] = stepSize * (1.399f);	// .6 ULP limit - to 1

			vDepth1[8] = stepSize * (1.25f);
			vDepth1[9] = stepSize * (1.601f);
		}

		FillVB(vDepth1);

		// render vertex buffer
		GetEffectiveContext()->OMSetDepthStencilState( m_pDSS, 0 );
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTV, m_pDSV ); // m_pDSV might be read-only
		GetEffectiveContext()->Draw( uSize*3, 0 );

		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context.") );
			return RESULT_FAIL;
		}
			
		//
		// Quantize the two buffers 
		// Process: (FLOAT32 -> FLOAT_COLOR (R32G32B32A32_FLOAT -> *depth buffer format* -> FLOAT_COLOR (R32G32B32A32_FLOAT -> FLOAT32)
		//

		// Convert vDepth0
		if( !CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, DXGI_FORMAT_R32_FLOAT, pTempColorData, 1, uSize, &(vDepth0[0]) ))
			throw TRException("Failed to convert vDepth0 to FLOAT_COLOR.");
		if( !CFormatHelper::ConvertPixels( CONV_TO_FORMAT, m_DSTex2DDesc.Format, pTempColorData, 1, uSize, pTempDSFormatData ))	// Quantization happens here
			throw TRException("Failed to convert vDepth0 to m_DSTex2DDesc.Format.");

		// The format helper will quantize the data for us, according to the spec. Now lets convert back.

		if( !CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, m_DSTex2DDesc.Format, pTempColorData, 1, uSize, pTempDSFormatData ))
			throw TRException("Failed to convert vDepth0 to FLOAT_COLOR.");
		if( !CFormatHelper::ConvertPixels( CONV_TO_FORMAT, DXGI_FORMAT_R32_FLOAT, pTempColorData, 1, uSize, &(vDepth0_quantized[0]) ))
			throw TRException("Failed to convert vDepth0 to DXGI_FORMAT_R32_FLOAT.");

		// Convert vDepth1
		if( !CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, DXGI_FORMAT_R32_FLOAT, pTempColorData, 1, uSize, &(vDepth1[0]) ))
			throw TRException("Failed to convert vDepth1 to FLOAT_COLOR.");
		if( !CFormatHelper::ConvertPixels( CONV_TO_FORMAT, m_DSTex2DDesc.Format, pTempColorData, 1, uSize, pTempDSFormatData ))	// Quantization happens here
			throw TRException("Failed to convert vDepth1 to m_DSTex2DDesc.Format.");

		// The format helper will quantize the data for us, according to the spec. Now lets convert back.

		if( !CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, m_DSTex2DDesc.Format, pTempColorData, 1, uSize, pTempDSFormatData ))
			throw TRException("Failed to convert vDepth1 to FLOAT_COLOR.");
		if( !CFormatHelper::ConvertPixels( CONV_TO_FORMAT, DXGI_FORMAT_R32_FLOAT, pTempColorData, 1, uSize, &(vDepth1_quantized[0]) ))
			throw TRException("Failed to convert vDepth1 to DXGI_FORMAT_R32_FLOAT.");

		//
		// Verify
		// 
		std::vector<float> vDepth(uSize);
		GetDepth(vDepth);
		for (UINT i = 0; i < vDepth.size(); ++i)
		{
			// expected value
			float fExpected;
			float fExpected_quantized;
			switch(m_DSDesc.DepthFunc)
			{
				case D3D11_COMPARISON_NEVER: 
					fExpected = vDepth0[i];
					fExpected_quantized  = vDepth0_quantized[i];
					break;
				case D3D11_COMPARISON_LESS:  
					fExpected = (vDepth1[i] < vDepth0[i]) ? vDepth1[i] : vDepth0[i];
					fExpected_quantized = (vDepth1_quantized[i] < vDepth0_quantized[i]) ? vDepth1_quantized[i] : vDepth0_quantized[i];
					break;
				case D3D11_COMPARISON_EQUAL:
					fExpected = (vDepth1[i] == vDepth0[i]) ? vDepth1[i] : vDepth0[i];
					fExpected_quantized = (vDepth1_quantized[i] == vDepth0_quantized[i]) ? vDepth1_quantized[i] : vDepth0_quantized[i];
					break;
				case D3D11_COMPARISON_LESS_EQUAL:
					fExpected = (vDepth1[i] <= vDepth0[i]) ? vDepth1[i] : vDepth0[i];
					fExpected_quantized = (vDepth1_quantized[i] <= vDepth0_quantized[i]) ? vDepth1_quantized[i] : vDepth0_quantized[i];
					break;
				case D3D11_COMPARISON_GREATER:
					fExpected = (vDepth1[i] > vDepth0[i]) ? vDepth1[i] : vDepth0[i];
					fExpected_quantized = (vDepth1_quantized[i] > vDepth0_quantized[i]) ? vDepth1_quantized[i] : vDepth0_quantized[i];
					break;
				case D3D11_COMPARISON_NOT_EQUAL:
					fExpected = (vDepth1[i] != vDepth0[i]) ? vDepth1[i] : vDepth0[i];
					fExpected_quantized = (vDepth1_quantized[i] != vDepth0_quantized[i]) ? vDepth1_quantized[i] : vDepth0_quantized[i];
					break;
				case D3D11_COMPARISON_GREATER_EQUAL:
					fExpected = (vDepth1[i] >= vDepth0[i]) ? vDepth1[i] : vDepth0[i];
					fExpected_quantized = (vDepth1_quantized[i] >= vDepth0_quantized[i]) ? vDepth1_quantized[i] : vDepth0_quantized[i];
					break;
				case D3D11_COMPARISON_ALWAYS:
					fExpected = vDepth1[i];
					fExpected_quantized = vDepth1_quantized[i];
					break;
			}

			// determine if depth was written
			if( !m_DSDesc.DepthEnable || (m_DSDesc.DepthWriteMask == D3D11_DEPTH_WRITE_MASK_ZERO) || (m_DSVDesc.Flags & D3D11_DSV_READ_ONLY_DEPTH) )
			{
				fExpected = vDepth0[i];
				fExpected_quantized = vDepth0_quantized[i];
			}

			// compare expected results to actual results
			float fUlpDiff;

			MinMaxRange::SetD3DFeatureLevel(GetFramework()->m_D3DOptions.FeatureLevel);
			float allowedULP = 0.5f;
			
			if( pTexFormatInfo->dwFlags & FF_FLOAT )
			{
				allowedULP = MinMaxRange::GetFloatULPVariance( 23 );
			}
			else
			if( pTexFormatInfo->dwFlags & FF_UNORM )
			{
                if(24 == numDepthBits)
                {
                    // 03.27.2015 - billkris: Changing to 1.0.  There is not enough precision
                    // in 32-bit float to accurrately measure 0.6 ULP for D24_UNORM (float has 
                    // only 23-bits of fractional precision while D24_UNORM has 24-bits of 
                    // fractional precision).
				    allowedULP = 1.0f;
                }
                else
                {
				    // Direct3D 10+ allows 0.6 ULP for FLOAT->UNORM conversion (see functional spec - 3.2.2.6)
				    // and this conversion is taking place in this test when writing into the depth buffer.
                    allowedULP = 0.6f;
                }
			}
			else
			{
				allowedULP = MinMaxRange::GetFixedFloatULPVariance();
			}

			if(!MinMaxRange::TestFloatsSame(fExpected, vDepth[i], pTexFormatInfo, 0, allowedULP, &fUlpDiff) )
			{
				// Increment the failure count
				failureCount++;

                if (failureCount < 1)
                {
				    UINT pixelX = i % m_uSubResWidth;
				    UINT pixelY = i / m_uSubResWidth;

    				WriteToLog("=================");
	    			WriteToLog("Failure #%d Info:", failureCount);
		    		WriteToLog("Pixel (%d, %d) Old value: %g (%g/%g)", pixelX, pixelY, vDepth0[i], vDepth0[i]/stepSize, maxValue);
			    	WriteToLog("Pixel (%d, %d) Old value (Quantized): %g (%g/%g)", pixelX, pixelY, vDepth0_quantized[i], vDepth0_quantized[i]/stepSize, maxValue);
				    WriteToLog("Pixel (%d, %d) New value: %g (%g/%g)", pixelX, pixelY, vDepth1[i], vDepth1[i]/stepSize, maxValue);
				    WriteToLog("Pixel (%d, %d) New value (Quantized): %g (%g/%g)", pixelX, pixelY, vDepth1_quantized[i], vDepth1_quantized[i]/stepSize, maxValue);
				    WriteToLog("Pixel (%d, %d) Expected value: %g (%g/%g)", pixelX, pixelY, fExpected, fExpected/stepSize, maxValue);
				    WriteToLog("Pixel (%d, %d) Actual value: %g (%g/%g)", pixelX, pixelY, vDepth[i], vDepth[i]/stepSize, maxValue);

    				if (fUlpDiff < 50) // spew ulpdiff only if it appears to be a precision issue
	    				WriteToLog("Pixel (%d, %d)Ulp difference: %+ f", pixelX, pixelY, fUlpDiff);
                }

				// Set the flag indicating we had a failure
				bFailCase = true;
			}

			// If we hit a lot of errors, exit out early to avoid creating a massive log.
			if( failureCount > 5 )
				throw TRException("Depth buffer contains invalid results. Exiting early as there are too many errors.");
		}

		// Check to see if we had any pixels fail
		if( bFailCase )
			throw TRException("Depth buffer contains invalid results.");
	}
	catch(TRException &exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CStateTest::ExecuteTestCase()] ");
		if (tRes != RESULT_PASS)
			WriteToLog( exc.GetError().c_str() );
	}

	// Clean up the temps
	if( pTempColorData )
		delete [] pTempColorData;

	if( pTempDSFormatData )
		delete [] pTempDSFormatData;

	pTempColorData = NULL;
	pTempDSFormatData = NULL;

	return tRes;
}

void CStateTest::CleanupTestCase()
{
	if (GetDevice())
	{
		GetEffectiveContext()->OMSetDepthStencilState( NULL, 0 );

		HRESULT hr;
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context.") );
		}
	}
			
	SAFE_RELEASE( m_pDSS );
	CleanupTestCaseCommon();
}

void CStateTest::Cleanup()
{
	SAFE_RELEASE( m_pDSSAlways );
	CleanupCommon();
}

void CStateStencilROTest::InitTestParameters()
{
	InitDepthTestParameters();

	//
	// StencilWriteMask
	//
	CTestCaseParameter<UINT8> *pStencilWriteMaskParam = AddParameter( _T("StencilWriteMask"), &m_DSDesc.StencilWriteMask );
	pStencilWriteMaskParam->SetDefaultValue( D3D11_DEFAULT_STENCIL_WRITE_MASK );
	CUserValueSet<UINT8> *pStencilWriteValues = new CUserValueSet<UINT8>( );
	pStencilWriteValues->AddValue( 0 );
	pStencilWriteValues->AddValue( 0xf );
	pStencilWriteValues->AddValue( D3D11_DEFAULT_STENCIL_WRITE_MASK );
	testfactor::RFactor rfStencilWriteMask = AddParameterValueSet( pStencilWriteMaskParam, pStencilWriteValues );

	//
	// DepthStencilView flag
	//
	CTestCaseParameter<UINT> *pViewFlag = AddParameter( _T("DepthStencilViewDesc_Flags"), &m_DSVDesc.Flags );
	pViewFlag->SetDefaultValue( 0 );
	CUserValueSet<UINT> *pViewFlagValueSet = new CUserValueSet<UINT>();
	if ( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ) // not available for 10_*
	{
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH );
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_STENCIL );
		pViewFlagValueSet->AddValue( D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL );
	}
	pViewFlagValueSet->AddValue( 0 );
	testfactor::RFactor rfViewFlag = AddParameterValueSet( pViewFlag, pViewFlagValueSet );

	//
	// DepthEnable
	//
	CTestCaseParameter<BOOL> *pDepthEnableParam = AddParameter( _T("DepthEnable"), &m_DSDesc.DepthEnable );
	CUserValueSet<BOOL> *pDepthEnableSet = new CUserValueSet<BOOL>();
	pDepthEnableSet->AddValue(TRUE);
	pDepthEnableSet->AddValue(FALSE);
	testfactor::RFactor rfDepthEnable = AddParameterValueSet( pDepthEnableParam, pDepthEnableSet );

	//
	// DepthWriteMask
	//
	CTestCaseParameter<D3D11_DEPTH_WRITE_MASK> *pDepthWriteMaskParam = AddParameter( _T("DepthWriteMask"), &m_DSDesc.DepthWriteMask );
	CUserValueSet<D3D11_DEPTH_WRITE_MASK> *pDepthWriteValues = new CUserValueSet<D3D11_DEPTH_WRITE_MASK>( );
	pDepthWriteValues->AddValue( D3D11_DEPTH_WRITE_MASK_ZERO );
	pDepthWriteValues->AddValue( D3D11_DEPTH_WRITE_MASK_ALL  );
	testfactor::RFactor rfDepthWriteMask = AddParameterValueSet( pDepthWriteMaskParam, pDepthWriteValues );

	//
	// DepthFunc
	//
	CTestCaseParameter<D3D11_COMPARISON_FUNC> *pDepthFuncParam = AddParameter( _T("DepthFunc"), &m_DSDesc.DepthFunc );
	CUserValueSet<D3D11_COMPARISON_FUNC> *pDepthFuncValues = new CUserValueSet<D3D11_COMPARISON_FUNC>( );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_NEVER );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_LESS );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_EQUAL );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_LESS_EQUAL );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_GREATER );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_NOT_EQUAL );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_GREATER_EQUAL );
	pDepthFuncValues->AddValue( D3D11_COMPARISON_ALWAYS );
	testfactor::RFactor rfDepthFunc = AddParameterValueSet( pDepthFuncParam, pDepthFuncValues );

	//
	// ULP Limit testing
	//
	CTestCaseParameter<bool> *pULPLimitParam = AddParameter( _T("ULPLimits"), &m_ulpLimits );
	testfactor::RFactor rfULPLimitFalse = AddParameterValue<bool>( pULPLimitParam, false );

	//
	// Root
	//
	SetRootTestFactor( m_rfReducedWithStencil * rfDepthFunc * rfULPLimitFalse * rfDepthWriteMask * rfViewFlag * rfDepthEnable * rfStencilWriteMask );
}


#define D3D_REQ_DEPTH_STENCIL_OBJECT_COUNT_PER_CONTEXT (D3D10_REQ_DEPTH_STENCIL_OBJECT_COUNT_PER_CONTEXT)
#define MAX_DEVICES (32)

static const c_ValidD3DCreateFlags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_SWITCH_TO_REF | D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;

static HRESULT CreateTempDevice(CD3D11TestFramework *pFramework, ID3D11Device* pExisting, ID3D11Device** pNewDevice)
{
	HRESULT hr = E_UNEXPECTED;
	*pNewDevice = NULL;
	IDXGIDevice* pDXGI = NULL;
	IDXGIAdapter* pAdapter = NULL;

	hr = pExisting->QueryInterface(__uuidof(IDXGIDevice), (void**)(&pDXGI));
	if (FAILED(hr) || pDXGI == NULL)
		goto func_end;
	hr = pDXGI->GetAdapter(&pAdapter);
	const UINT createFlags = c_ValidD3DCreateFlags & pExisting->GetCreationFlags();
	ID3D11DeviceContext *pDeviceContext = NULL;
	hr = pFramework->CreateDevice(pAdapter, pFramework->m_D3DOptions.DriverType, createFlags, pFramework->m_D3DOptions.FeatureLevel, pNewDevice, &pDeviceContext);
	SAFE_RELEASE(pDeviceContext);

func_end:
	SAFE_RELEASE(pDXGI);
	SAFE_RELEASE(pAdapter);
	return hr;
}

TEST_RESULT CStateLimitsTest::Setup()
{
	m_pDevice = GetFramework()->GetDevice();
	return RESULT_PASS;
}

void CStateLimitsTest::Cleanup()
{

}

void CStateLimitsTest::InitTestParameters()
{
	int nPrimitiveW = 16;
	int nPrimitiveH = 16;

	GetFramework()->m_RenderTargetOptions.SwapChainDesc.Width = nPrimitiveW*2;
	GetFramework()->m_RenderTargetOptions.SwapChainDesc.Height = nPrimitiveH;
	GetFramework()->m_RenderTargetOptions.DepthTextureDesc.Width = nPrimitiveW*2;
	GetFramework()->m_RenderTargetOptions.DepthTextureDesc.Height = nPrimitiveH;

	AddParameter<UINT>( _T("NumDevices"), &m_numDevices );
	testfactor::RFactor rNumDevices = AddParameterValue( _T("NumDevices"), (UINT)8 );

	SetRootTestFactor(rNumDevices);
}

TEST_RESULT CStateLimitsTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = E_UNEXPECTED;

	assert(m_numDevices <= MAX_DEVICES);

	std::vector< std::vector<ID3D11DepthStencilState*> > vStates(MAX_DEVICES);
	std::vector< ID3D11Device* > vDevices(MAX_DEVICES);

	D3D11_DEPTH_STENCIL_DESC defaultDesc, stateDesc;
	// defaults first
	defaultDesc.DepthEnable						= TRUE;
	defaultDesc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ALL;
	defaultDesc.DepthFunc						= D3D11_COMPARISON_LESS;
	defaultDesc.StencilEnable					= FALSE;
	defaultDesc.StencilReadMask					= 0xff;
	defaultDesc.StencilWriteMask				= 0xff;
	defaultDesc.FrontFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
	defaultDesc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
	defaultDesc.FrontFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
	defaultDesc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;
	defaultDesc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
	defaultDesc.BackFace.StencilDepthFailOp		= D3D11_STENCIL_OP_KEEP;
	defaultDesc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
	defaultDesc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;
	// then incremental values
	stateDesc = defaultDesc;

	for (UINT uDevice = 0; uDevice < m_numDevices; ++uDevice)
	{
		// create device
		if( FAILED( hr = CreateTempDevice(GetFramework(), m_pDevice, &vDevices[uDevice]) ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T("Failed to create another device. device#=%d, hr=%s\n"), uDevice+1, D3DHResultToString(hr).c_str());
			goto testDone;
		}
		
		// allocate room for 4096 states for this device
		vStates[uDevice].resize(D3D_REQ_DEPTH_STENCIL_OBJECT_COUNT_PER_CONTEXT+1);

		// create default state
		if( FAILED( hr = vDevices[uDevice]->CreateDepthStencilState(&defaultDesc, &vStates[uDevice][0]) ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T("Failed on call CreateDepthStencilState when creating default state. device#=%d, hr=%s\n"), uDevice+1, D3DHResultToString(hr).c_str());
			goto testDone;
		}

		for (UINT n = 1; n < D3D_REQ_DEPTH_STENCIL_OBJECT_COUNT_PER_CONTEXT; ++n)
		{
			// create a unique state
			stateDesc = defaultDesc;
			stateDesc.StencilReadMask = n%256;
			stateDesc.StencilReadMask = n/256;

			if( FAILED( hr = vDevices[uDevice]->CreateDepthStencilState(&stateDesc, &vStates[uDevice][n]) ) )
			{
				if (n < D3D_REQ_DEPTH_STENCIL_OBJECT_COUNT_PER_CONTEXT 
					|| hr != D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS)
				{
					tRes = RESULT_FAIL;
					WriteToLog( _T("Failed on call to CreateDepthStencilState.\n"));
					WriteToLog( _T("Device#=%d, StateNum=%d, TotalStates=%d, hr=%s\n"), uDevice+1, n, uDevice*4096+n, D3DHResultToString(hr).c_str() );
					goto testDone;
				}
			}
			else if (n >= D3D_REQ_DEPTH_STENCIL_OBJECT_COUNT_PER_CONTEXT)
			{
				tRes = RESULT_FAIL;
				WriteToLog( _T("Too many unique objects were created without failure.\n"));
				WriteToLog( _T("Device#=%d, StateNum=%d, TotalStates=%d, hr=%s\n"), uDevice+1, n, uDevice*4096+n, D3DHResultToString(hr).c_str() );
				goto testDone;
			}
		}
	}

testDone:
	for (UINT uDevice = 0; uDevice < m_numDevices; ++uDevice)
	{
		if (vStates[uDevice].size() > 0)
		{
			for (UINT n = 0; n <= D3D_REQ_DEPTH_STENCIL_OBJECT_COUNT_PER_CONTEXT; ++n)
			{
				SAFE_RELEASE(vStates[uDevice][n]);
			}
		}
		SAFE_RELEASE(vDevices[uDevice]);
	}

	return tRes;
}
