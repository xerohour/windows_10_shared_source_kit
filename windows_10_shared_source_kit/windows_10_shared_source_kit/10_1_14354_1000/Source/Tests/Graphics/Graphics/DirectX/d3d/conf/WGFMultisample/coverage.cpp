//////////////////////////////////////////////////////////////////////////
//  Coverage.cpp
//  created:	2005/07/18
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "Coverage.h"
#include <formathelper.h>
#include <d3dmshelper.h>

BEGIN_NAMED_VALUES( SAMPLE_MASK_PATTERN )
	NAMED_VALUE( "MASK_ALL_ON", MASK_ALL_ON )
	NAMED_VALUE( "MASK_ALL_OFF", MASK_ALL_OFF )
	NAMED_VALUE( "MASK_LOWER_HALF_ON_HIGHER_HALF_OFF", MASK_LOWER_HALF_ON_HIGHER_HALF_OFF )
	NAMED_VALUE( "MASK_LOWER_HALF_OFF_HIGHER_HALF_ON", MASK_LOWER_HALF_OFF_HIGHER_HALF_ON )
	NAMED_VALUE( "MASK_PATTERN_OFF_OFF_OFF_ON", MASK_PATTERN_OFF_OFF_OFF_ON )
	NAMED_VALUE( "MASK_PATTERN_ON_ON_ON_OFF", MASK_PATTERN_ON_ON_ON_OFF )
END_NAMED_VALUES( SAMPLE_MASK_PATTERN )

CCoverageTest::CCoverageTest() 
: CMultisampleTest(),
  m_pTexture(NULL),
  m_pSRV(NULL),
  m_pPSMask(NULL)
{}


void CCoverageTest::InitTestParameters()
{
	InitMultisampleParameters();

	// shader frequency value set
	CUserValueSet<SHADER_FREQUENCY> *pFrequencySet = new CUserValueSet<SHADER_FREQUENCY>();
	pFrequencySet->AddValue(FREQUENCY_PIXEL);
	pFrequencySet->AddValue(FREQUENCY_SAMPLE);

	// mask value set
	CTestCaseParameter< SAMPLE_MASK_PATTERN > *pMaskParam = AddParameter( _T( "SampleMask" ), &m_MaskPattern );
	CUserValueSet<SAMPLE_MASK_PATTERN> *pMaskSet = new CUserValueSet<SAMPLE_MASK_PATTERN>();
	
	pMaskSet->AddValue( MASK_ALL_OFF );
	pMaskSet->AddValue( MASK_ALL_ON );
	testfactor::RFactor rfMaskBasic = AddParameterValueSet( pMaskParam, pMaskSet);

	// These two only applies to 2 and more sample counts
	pMaskSet->AddValue( MASK_LOWER_HALF_ON_HIGHER_HALF_OFF );
	pMaskSet->AddValue( MASK_LOWER_HALF_OFF_HIGHER_HALF_ON );
	testfactor::RFactor rfMaskFor2AndMoreSamples = AddParameterValueSet( pMaskParam, pMaskSet);

	// These two only applies to 4 and more sample counts
	pMaskSet->AddValue( MASK_PATTERN_OFF_OFF_OFF_ON );
	pMaskSet->AddValue( MASK_PATTERN_ON_ON_ON_OFF );
	testfactor::RFactor rfMaskFor4AndMoreSamples = AddParameterValueSet( pMaskParam, pMaskSet);
	
	// Sample counts and quality levels
	testfactor::RFactor rfSampleCount1 = AddParameterValue<UINT>( m_pSampleCountParam, 1 );
	testfactor::RFactor rfSampleCount2 = AddParameterValue<UINT>( m_pSampleCountParam, 2 );
	testfactor::RFactor rfSampleCount4 = AddParameterValue<UINT>( m_pSampleCountParam, 4 );
	testfactor::RFactor rfSampleCount8 = AddParameterValue<UINT>( m_pSampleCountParam, 8 );
	testfactor::RFactor rfSampleCount16 = AddParameterValue<UINT>( m_pSampleCountParam, 16 );
	testfactor::RFactor rfSampleCount32 = AddParameterValue<UINT>( m_pSampleCountParam, 32 );

	testfactor::RFactor rfQualityLevelZero = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_ZERO );
	testfactor::RFactor rfQualityLevelMaxSupported = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_MAX_SUPPORTED );
	testfactor::RFactor rfQualityLevelStandard = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_STANDARD );
	testfactor::RFactor rfQualityLevelCenter = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_CENTER );

	testfactor::RFactor rfQualityLevelAll = rfQualityLevelZero + rfQualityLevelMaxSupported + rfQualityLevelStandard + rfQualityLevelCenter;

	testfactor::RFactor rfSampleDescMask = 
		rfSampleCount1 * rfQualityLevelZero * rfMaskBasic + 
		// only sample counts 1(trival), 2, 4, 8 and 16 support standard and center sample patterns
		rfSampleCount2 * rfQualityLevelAll * rfMaskBasic + 
		rfSampleCount4 * rfQualityLevelAll * rfMaskFor2AndMoreSamples + 
		( rfSampleCount8 + rfSampleCount16 ) * rfQualityLevelAll * rfMaskFor4AndMoreSamples + 
		rfSampleCount32 * ( rfQualityLevelZero + rfQualityLevelMaxSupported ) * rfMaskFor4AndMoreSamples;

	testfactor::RFactor rfSampleDescMaskBasic =  
		(
			rfSampleCount1 * rfQualityLevelZero + 
			// only sample counts 1(trival), 2, 4, 8 and 16 support standard and center sample patterns
			( rfSampleCount2 + rfSampleCount4 + rfSampleCount8 + rfSampleCount16 ) * rfQualityLevelAll + 
			rfSampleCount32 * ( rfQualityLevelZero + rfQualityLevelMaxSupported )
		) * rfMaskBasic;

	// parameter
	CTestCaseParameter< bool > *pCoverageParam = AddParameter( _T( "SV_Coverage" ), &m_bSVCoverage );
	

	// factors
	testfactor::RFactor rfCoverageOn  = AddParameterValue( _T( "SV_Coverage" ), true );
	testfactor::RFactor rfCoverageOff = AddParameterValue( _T( "SV_Coverage" ), false );
	testfactor::RFactor rfA2C = AddParameter( _T( "A2CEnable" ), &m_bA2C, new CBoolValueSet() );
	testfactor::RFactor alpha = AddParameter( _T( "Alpha" ), &m_fAlpha, new CGradientValueSet<float>(0.0, 1.0, 2));
	testfactor::RFactor freq = AddParameter( _T( "ShaderFrequency" ), &m_sf, pFrequencySet );
	
	// root
	testfactor::RFactor rfCoverageMask = rfCoverageOff * rfSampleDescMaskBasic + rfCoverageOn * rfSampleDescMask;
	SetRootTestFactor ( m_rfRTFormatTypedSubset * rfCoverageMask * alpha * freq * rfA2C );

	
	CFilterHandle skipMatchingFilter;
	// filter out dx10.1 tests
	if (GetFramework()->m_D3DOptions.FeatureLevel == D3D10_FEATURE_LEVEL_10_0)
	{
		// SV_Coverage and sample frequency shaders don't exist in 10.0
		skipMatchingFilter = ( FilterEqual<bool>( _T("SV_Coverage"), true ) ||
					  FilterEqual<SHADER_FREQUENCY> ( _T("ShaderFrequency"), FREQUENCY_SAMPLE ) );
	}

	// Sample-frequency Pixel Shader execution is undefined when sample pattern is CENTER and 
	// more than one sample overlapping at the center of the pixel. 
	skipMatchingFilter = skipMatchingFilter || ( 
		FilterGreater<UINT>( _T("Sample_Count"), 1 ) 
		&& FilterEqual<SHADER_FREQUENCY> ( _T("ShaderFrequency"), FREQUENCY_SAMPLE ) 
		&& FilterEqual<QUALITY_LEVEL_SCENARIO> ( _T("Quality_Level"), QUALITY_LEVEL_CENTER ) );
	SkipMatching( skipMatchingFilter );
}

TEST_RESULT CCoverageTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	tRes = SetFormatAndQualityLevel( false );
	if ( tRes != RESULT_PASS )
	{
		return tRes;
	}

	switch(m_MaskPattern)
	{
	case MASK_ALL_ON:
		m_uMask = 0xffffffff; 
		break;
	case MASK_ALL_OFF:
		m_uMask = 0x0; 
		break;
	case MASK_LOWER_HALF_ON_HIGHER_HALF_OFF:
		m_uMask = 0xffffffff >> ( 32 - m_uSampleCount/2 ); 
		break;
	case MASK_LOWER_HALF_OFF_HIGHER_HALF_ON:
		m_uMask = 0xffffffff << ( m_uSampleCount/2 ); 
		break;
	case MASK_PATTERN_OFF_OFF_OFF_ON:
		m_uMask = 0xaaaaaaaa; 
		break;
	case MASK_PATTERN_ON_ON_ON_OFF:
		m_uMask = 0x55555555; 
		break;
	default:
		WriteToLog(_T("Unexpected Mask Pattern Value!"));
		return RESULT_FAIL;
	}

	try 
	{
		SetupD3D();

		srand(0);
		// create float test data
		UINT uHeight = m_RTHeight;
		UINT uWidth = (m_sf == FREQUENCY_PIXEL) ? m_RTWidth : m_RTWidth*m_uSampleCount;
		m_vCoverage.resize(uWidth*uHeight);
		const UINT randRangeMax = (UINT)0xFFFFFFFF >> ( 32 - m_uSampleCount ); 
		for (UINT i = 0; i < m_vCoverage.size(); i++)
			m_vCoverage[i] = (UINT)( (double)rand() / RAND_MAX * randRangeMax );

		// data texture initial data
		D3D11_SUBRESOURCE_DATA srData;
		srData.pSysMem = &m_vCoverage[0];
		srData.SysMemPitch = uWidth * 4;
		srData.SysMemSlicePitch = uWidth * uHeight * 4;

		// data texture description
		D3D11_TEXTURE2D_DESC tex2DDesc;
		tex2DDesc.ArraySize = 1;
		tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tex2DDesc.CPUAccessFlags = 0;
		tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
		tex2DDesc.Format = DXGI_FORMAT_R32_UINT;
		tex2DDesc.SampleDesc.Count = 1;
		tex2DDesc.SampleDesc.Quality = 0;
		tex2DDesc.MipLevels = 1;
		tex2DDesc.MiscFlags = 0;
		tex2DDesc.Height = uHeight;
		tex2DDesc.Width = uWidth;
		if( FAILED( hr = GetFramework()->CreateTexture2DTiledOptional( &tex2DDesc, &srData, &m_pTexture ) ) )
			throw TRException( "Failed CreateTexture2D.", hr, RESULT_FAIL );

		// create shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = tex2DDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pTexture, &srvDesc, &m_pSRV ) ) )
			throw TRException( "Failed CreateShaderResourceView", hr, RESULT_FAIL );

		// create shader
		if (m_bSVCoverage)
		{
			char *szFunctionNameList[] = { "MainPixelMask", "MainSampleMask" };
			char *szFunctionName = szFunctionNameList[(m_sf == FREQUENCY_PIXEL) ? 0:1];

			ID3D10Blob *pBuf(NULL);
			ID3D10Blob *pError(NULL);
			if ( FAILED( hr = D3DX10CompileFromResource(NULL, "ps_coverage.psh", NULL, NULL, NULL, szFunctionName, "ps_4_1", 0, 0, NULL, &pBuf, &pError, NULL) ) )
			{
				if (pError) WriteToLog("%s", pError->GetBufferPointer());
				SAFE_RELEASE(pBuf);
				SAFE_RELEASE(pError);		
				throw TRException("CompileShaderFromResource Failed.", hr, RESULT_FAIL);
			}
			if ( FAILED( hr = m_pDevice->CreatePixelShader( (DWORD*) pBuf->GetBufferPointer(), pBuf->GetBufferSize(), NULL, &m_pPSMask ) ) )
			{
				SAFE_RELEASE(pBuf);
				SAFE_RELEASE(pError);	
				throw TRException("CreatePixelShader Failed.", hr, RESULT_FAIL);
			}
			SAFE_RELEASE(pBuf);
			SAFE_RELEASE(pError);
		}

		ResetStates();

	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_A2C::SetupTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}
	
	return tRes;
}

void CCoverageTest::CleanupTestCase()
{
	CMultisampleTest::CleanupD3D();

	SAFE_RELEASE( m_pTexture );
	SAFE_RELEASE( m_pSRV );
	SAFE_RELEASE( m_pPSMask );
	m_vCoverage.clear();
}

TEST_RESULT CCoverageTest::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	try
	{
		// Set A2C value
		float *bufData;
		D3D11_MAPPED_SUBRESOURCE mappedRes;
		if( FAILED( hr = GetEffectiveContext()->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
			throw TRException("m_pConstBuffer->Map() Failed.", hr, RESULT_FAIL);
		bufData = (float*) mappedRes.pData;
		//// Inverse viewport scale.
		bufData[2] = 2 / (float)GetFramework()->m_RenderTargetOptions.SwapChainDesc.Width;
		bufData[3] = -2 / (float)GetFramework()->m_RenderTargetOptions.SwapChainDesc.Height;
		// Prescaled inverse viewport translation.
		bufData[0] = -1.0f;
		bufData[1] = 1.0f;
		bufData[4] = m_fAlpha;  
		bufData[5] = 1.0f;
		GetEffectiveContext()->Unmap(m_pConstBuffer,0);

		// setup geometry
		m_uVertexCount = 0;
		CoverBuffer();

		// clear
		FLOAT ClearColor[4] = { 0, 0, 0, 0 };
		GetEffectiveContext()->ClearRenderTargetView( m_pRTView, ClearColor );
		GetEffectiveContext()->PSSetShaderResources(0, 1, &m_pSRV);
		if (m_bSVCoverage)
			GetEffectiveContext()->PSSetShader(m_pPSMask, NULL, 0);		
		GetEffectiveContext()->OMSetBlendState( m_pBS, 0, m_uMask );

		// render
		GetEffectiveContext()->Draw(m_uVertexCount, 0);



		if (m_bSVCoverage)
			ValidateSVC();
		else
			ValidateA2C();			

		
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_A2C::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}

	try
	{
		Present();
	}
	catch (TRException& exc)
	{
		exc.Prepend("[CMultisampleTest_A2C::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}

	return tRes;	
}

void CCoverageTest::ValidateA2C()
{
	HRESULT hr;
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
		throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

	vector<D3DXVECTOR4> vBuffer(m_RTWidth * m_RTHeight * m_uSampleCount);
	MSHelper::Read(GetFramework(), GetFramework()->GetRTTexture(), (FLOAT*)&vBuffer[0], m_RTVFormat);

	
	// determine if pixel (0,0) has a non-zero sample
	bool bResult = false;
	for (UINT uSample = 0; uSample < m_uSampleCount; ++uSample)
	{
		// get sample x, pixel (0,0)
		D3DXVECTOR4 vColor = vBuffer[uSample*m_RTWidth*m_RTHeight];
		if (vColor.x != 0) bResult = true;
	}

	if (m_uMask == 0)
	{
		if (bResult)
			throw TRException("SampleMask is 0 but pixel (0,0) color is non-zero");
	}
	else
	{
		if (!m_bA2C)
		{
			if(!bResult)
				throw TRException("A2C is disabled and SampleMask is non-zero but pixel (0,0) color is zero");
		}
		else
		{
			if (!bResult && m_fAlpha)
				throw TRException("A2C is enabled; Alpha is 1.0f but pixel (0,0) color is zero");
			if (bResult && !m_fAlpha)
				throw TRException("A2C is enabled; Alpha is 0.0f but pixel (0,0) color is non-zero");
		}
	}
}

void CCoverageTest::ValidateSVC()
{
	HRESULT hr;
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
		throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

	vector<D3DXVECTOR4> vBuffer(m_RTWidth * m_RTHeight * m_uSampleCount);
	MSHelper::Read(GetFramework(), GetFramework()->GetRTTexture(), (FLOAT*)&vBuffer[0], m_RTVFormat);

	UINT uHeight = m_RTHeight;
	UINT uWidth = m_RTWidth;
	for (UINT uSample = 0; uSample < m_uSampleCount; ++uSample)
	{
		m_uSampleMask = 1 << uSample;
		for (UINT uY = 0; uY < uHeight; ++uY)
			for (UINT uX = 0; uX < uWidth; ++uX)
			{
				// get coverage mask
				UINT uMaskIndex = uX + m_RTWidth * uY;
				if (m_sf == FREQUENCY_SAMPLE)
					uMaskIndex = uSample + m_uSampleCount * uX + m_RTWidth * m_uSampleCount * uY;
				// only concerned with current sample
				UINT uSampleBit = (m_vCoverage[uMaskIndex] & m_uSampleMask & m_uMask);

				// get samples color 
				D3DXVECTOR4 vColor = vBuffer[uX + uY*m_RTWidth + uSample*m_RTWidth*m_RTHeight];

				// determine is sample wasn't incorrectly masked
				bool bFailed = false;
				if (uSampleBit && (vColor.x == 0))
						bFailed = true;
				if (!uSampleBit && (vColor.x != 0.f))
						bFailed = true;

				// log error
				if (bFailed)
				{
					WriteToLog("Pixel = x:%d, y:%d", uX, uY);
					WriteToLog("Sample = %d", uSample);
					WriteToLog("Color = {%f, %f, %f, %f}", vColor.x, vColor.y, vColor.z, vColor.w);
					WriteToLog("SV_Coverage = %x, Sample[%d] bit = %d", m_vCoverage[uMaskIndex], uSample, (UINT)((m_vCoverage[uMaskIndex] & m_uSampleMask) > 0));
					throw TRException("SV_Coverage incorrectly masked the sample.");
				}
			}
	}
}


////////////////////////////////////////
CInputCoverageTest::CInputCoverageTest()
{
	m_pCoveragePS = NULL;
}

void CInputCoverageTest::InitTestParameters()
{
	InitMultisampleParameters();

	testfactor::RFactor freq = AddParameter( _T( "ShaderFrequency" ), &m_sf, new CUserValueSet<SHADER_FREQUENCY>( FREQUENCY_PIXEL, FREQUENCY_SAMPLE ) );
	testfactor::RFactor targetX = AddParameter( _T( "TargetPixelX" ), &m_TargetPixelX, new CRangeValueSet< UINT > (0, 1, 1) );
	testfactor::RFactor targetY = AddParameter( _T( "TargetPixelY" ), &m_TargetPixelY, new CRangeValueSet< UINT > (0, 1, 1) );
	testfactor::RFactor sampleMask = AddParameter( _T( "SampleMask" ), &m_BlendStateSampleMask, new CUserValueSet< UINT > (0, UINT_MAX, 0xAAAAAAAA) );

	// Sample-frequency Pixel Shader execution is undefined when sample pattern is CENTER and 
	// more than one sample overlapping at the center of the pixel. 
	// Set skip ranges BEFORE creating CrossFactor, as instructed in ctestfactor.h
	SkipMatching( 
		FilterGreater<UINT>( _T("Sample_Count"), 1 ) 
		&& FilterEqual<SHADER_FREQUENCY> ( _T("ShaderFrequency"), FREQUENCY_SAMPLE ) 
		&& FilterEqual<QUALITY_LEVEL_SCENARIO> ( _T("Quality_Level"), QUALITY_LEVEL_CENTER ) );

	testfactor::XFACTOR pairwiseTable[] = 
	{
		{freq, 1, NULL, 0},
		{targetX, 1, NULL, 0},
		{targetY, 1, NULL, 0},
		{sampleMask, 1, NULL, 0},
	};
	size_t groupOrders [] = { XGROUPORDER(1, 2) };

	testfactor::RFactor rfCoverage = NewCrossFactor(
		pairwiseTable, 
		_countof(pairwiseTable),
		_T("CoverageFactors"),
		groupOrders,
		_countof(groupOrders) );

	SetRootTestFactor ( ( m_rfRTFormatTypedUINT % m_rfPixPos ) * m_rfSampleDesc * rfCoverage  );

	
}

TEST_RESULT CInputCoverageTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	tRes = SetFormatAndQualityLevel( false );
	if (tRes != RESULT_PASS )
	{
		return tRes;
	}

	ID3D10Blob* pBuffer = NULL;
	ID3D10Blob* pError = NULL;

	try
	{
		SetupD3D();

		// Compile PS
		const char* PSName = (m_sf == FREQUENCY_PIXEL) ? "MainPixelCoverage" : "MainSampleCoverage";

		hr = D3DX10CompileFromResource(NULL, "ps_input_coverage.psh", NULL, NULL, NULL, PSName, "ps_5_0", 0, 0, NULL, &pBuffer, &pError, NULL);
		if(FAILED(hr))
		{
			if (pError) 
			{
				WriteToLog((const char*)pError->GetBufferPointer());
			}

			throw TRException("CompileFromResource failed", hr, RESULT_FAIL);
		}

		hr = m_pDevice->CreatePixelShader( (DWORD*)pBuffer->GetBufferPointer(), pBuffer->GetBufferSize(), NULL, &m_pCoveragePS );
		if(FAILED(hr))
		{
			throw TRException("CreatePixelShader failed");
		}

		// Set state back to CMultisampleTest default
		ResetStates();
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();

		exc.Prepend("[CInputCoverageTest::SetupTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}

	SAFE_RELEASE(pBuffer);
	SAFE_RELEASE(pError);

	return tRes;
}

void CInputCoverageTest::CleanupTestCase()
{
	SAFE_RELEASE(m_pCoveragePS);

	CMultisampleTest::CleanupD3D();
}

TEST_RESULT CInputCoverageTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	ID3D11BlendState* pBlendState = NULL;

	try
	{
		// clear
		FLOAT ClearColor[4] = { 0, 0, 0, 0 };
		GetEffectiveContext()->ClearRenderTargetView( m_pRTView, ClearColor );
		GetEffectiveContext()->PSSetShader(m_pCoveragePS, NULL, 0);		

		// set blend state sample mask to m_BlendStateSampleMask
		FLOAT BlendFactor[4];
		UINT SavedSampleMask = 0;
		GetEffectiveContext()->OMGetBlendState(&pBlendState, BlendFactor, &SavedSampleMask);
		GetEffectiveContext()->OMSetBlendState(pBlendState, BlendFactor, m_BlendStateSampleMask);

		// render
		m_uVertexCount = 0;
		CoverHalfPixel( m_TargetPixelX, m_TargetPixelY );
		GetEffectiveContext()->Draw(m_uVertexCount, 0);

		// restore blend state sample mask to original value
		GetEffectiveContext()->OMSetBlendState(pBlendState, BlendFactor, SavedSampleMask);

		// verify results
		Validate();
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CInputCoverageTest::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}
	
	SAFE_RELEASE(pBlendState);

	return tRes;
}

UINT GetRedChannel( DXGI_FORMAT Format, CONST VOID* Src )
{
	UINT Result = 0;

	switch( Format )
	{
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32_UINT:
		Result = ((UINT*)Src)[0];
		break;

	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16_UINT:
		Result = ((WORD*)Src)[0];
		break;

	case DXGI_FORMAT_R10G10B10A2_UINT:
		{
			UINT u = ((UINT*)Src)[0];
			Result = u & 0x3ff;
		}
		break;

	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8_UINT:
		Result = ((BYTE*)Src)[0];
		break;

	default:
		assert( false );
	}

	return Result;
}

UINT GetRedChannelMask( DXGI_FORMAT Format )
{
	UINT Result = 0;

	switch( Format )
	{
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32_UINT:
		Result = UINT_MAX;
		break;

	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16_UINT:
		Result = 0xffff;
		break;

	case DXGI_FORMAT_R10G10B10A2_UINT:
		Result = 0x3ff;
		break;

	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8_UINT:
		Result = 0xff;
		break;

	default:
		assert( false );
	}

	return Result;
}

void CInputCoverageTest::Validate()
{
	HRESULT hr;
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
		throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

	UINT BytesPerPixel = GetBitsPerElement( m_RTVFormat ) / 8;
	UINT RowPitch = BytesPerPixel * m_RTWidth;
	UINT SlicePitch = RowPitch * m_RTHeight;

	vector<BYTE> vBuffer( m_RTWidth * m_RTHeight * m_uSampleCount * BytesPerPixel );
	if( !MSHelper::ReadWithoutConversion( GetFramework(), GetFramework()->GetRTTexture(), &vBuffer[0], m_RTVFormat ) )
	{
		throw TRException("MSHelper::ReadWithoutConversion failed");
	}

	CONST BYTE* BufferStart = &vBuffer[0];

	// Determine which samples were lit
	UINT ExpectedCoverageMask = 0;

	for( UINT s = 0; s < m_uSampleCount; s++)
	{
		CONST BYTE* Sample = BufferStart + 
							 (m_TargetPixelX * BytesPerPixel) + 
							 (m_TargetPixelY * RowPitch) + 
							 (s * SlicePitch);

		UINT SampleContents = GetRedChannel( m_RTVFormat, Sample );

		if( SampleContents )
		{
			if( m_BlendStateSampleMask & (1 << s) )
			{
				ExpectedCoverageMask |= (1 << s);
			}
			else
			{
				WriteToLog( "x = %u, y = %u, sample = %u\n", m_TargetPixelX, m_TargetPixelY, s );
				throw TRException("A sample was lit that was not in the blend state sample mask");
			}
		}
	}

	// Saturate the expected mask in case the red channel of the render target format
	// doesn't have enough bits to contain the entire mask
	UINT MaskValue = GetRedChannelMask( m_RTVFormat );
	ExpectedCoverageMask = min(ExpectedCoverageMask, MaskValue);

	CONST BYTE* Curr = BufferStart;

	for( UINT s = 0; s < m_uSampleCount; s++)
	{
		for( UINT y = 0; y < (UINT)m_RTHeight; y++ )
		{
			for( UINT x = 0; x < (UINT)m_RTWidth; x++ )
			{
				UINT ExpectedCoverage = 0;

				UINT Result = GetRedChannel( m_RTVFormat, Curr );

				// If the sample is in the covered pixel and it was lit
				// then the red channel should contain ExpectedCoverageMask
				// Otherwise, the red channel should contain 0 (it should not have been lit)
				if( (y == m_TargetPixelY) && (x == m_TargetPixelX) )
				{
					if( Result )
					{
						ExpectedCoverage = ExpectedCoverageMask;
					}
				}

				if(Result != ExpectedCoverage)
				{
					WriteToLog( "x = %u, y = %u, Sample = %u\n", x, y, s );
					WriteToLog( "Expected SV_COVERAGE = %u, Actual SV_COVERAGE = %u", ExpectedCoverage, Result );
					throw TRException("SV_COVERAGE was generated incorrectly");
				}

				Curr += BytesPerPixel;                
			}
		}
	}
}


////////////////////////////////////////
CInputCoverageWithDepthTest::CInputCoverageWithDepthTest()
{
	m_pCoveragePS = NULL;
	m_pDepthBuffer = NULL;
	m_pDSV = NULL;
	m_pDepthStencilState = NULL;
}

void CInputCoverageWithDepthTest::InitTestParameters()
{
	InitMultisampleParameters();

	vector< DXGI_FORMAT > DepthFormats;
	DepthFormats.push_back( DXGI_FORMAT_D16_UNORM );
	DepthFormats.push_back( DXGI_FORMAT_D32_FLOAT );
	DepthFormats.push_back( DXGI_FORMAT_D24_UNORM_S8_UINT );
	DepthFormats.push_back( DXGI_FORMAT_D32_FLOAT_S8X24_UINT );

	testfactor::RFactor freq = AddParameter( _T( "ShaderFrequency" ), &m_sf, new CUserValueSet<SHADER_FREQUENCY>( FREQUENCY_PIXEL, FREQUENCY_SAMPLE ) );
	testfactor::RFactor targetX = AddParameter( _T( "TargetPixelX" ), &m_TargetPixelX, new CRangeValueSet< UINT > (0, 1, 1) );
	testfactor::RFactor targetY = AddParameter( _T( "TargetPixelY" ), &m_TargetPixelY, new CRangeValueSet< UINT > (0, 1, 1) );
	testfactor::RFactor depthMask = AddParameter( _T( "DepthMask" ), &m_DepthMask, new CUserValueSet< UINT > (0xAAAAAAAA, 0xCCCCCCCC) );
	testfactor::RFactor depthFormat = AddParameter( _T( "DepthFormat" ), &m_DepthBufferFormat, new CUserValueSet< DXGI_FORMAT >(DepthFormats) );

	// Sample-frequency Pixel Shader execution is undefined when sample pattern is CENTER and 
	// more than one sample overlapping at the center of the pixel. 
	// Set skip ranges BEFORE creating CrossFactor, as instructed in ctestfactor.h
	SkipMatching( 
		FilterGreater<UINT>( _T("Sample_Count"), 1 ) 
		&& FilterEqual<SHADER_FREQUENCY> ( _T("ShaderFrequency"), FREQUENCY_SAMPLE ) 
		&& FilterEqual<QUALITY_LEVEL_SCENARIO> ( _T("Quality_Level"), QUALITY_LEVEL_CENTER ) );

	testfactor::XFACTOR pairwiseTable[] = 
	{
		{freq, 1, NULL, 0},
		{targetX, 1, NULL, 0},
		{targetY, 1, NULL, 0},
		{depthMask, 1, NULL, 0},
		{depthFormat, 1, NULL, 0},
	};
	size_t groupOrders [] = { XGROUPORDER(1, 2) };

	testfactor::RFactor rfCoverage = NewCrossFactor(
		pairwiseTable, 
		_countof(pairwiseTable),
		_T("CoverageFactors"),
		groupOrders,
		_countof(groupOrders) );

	SetRootTestFactor ( ( rfCoverage % m_rfRTFormatTypedUINT ) * m_rfSampleDesc );

	
}

TEST_RESULT CInputCoverageWithDepthTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	tRes = SetFormatAndQualityLevel( false );
	if ( tRes != RESULT_PASS )
	{
		return tRes;
	}

	ID3D10Blob* pBuffer = NULL;
	ID3D10Blob* pError = NULL;

	try
	{
		SetupD3D();

		// Compile PS
		const char* PSName = (m_sf == FREQUENCY_PIXEL) ? "MainPixelCoverage" : "MainSampleCoverage";

		hr = D3DX10CompileFromResource(NULL, "ps_input_coverage.psh", NULL, NULL, NULL, PSName, "ps_5_0", 0, 0, NULL, &pBuffer, &pError, NULL);
		if(FAILED(hr))
		{
			if (pError) 
			{
				WriteToLog((const char*)pError->GetBufferPointer());
			}

			throw TRException("CompileFromResource failed", hr, RESULT_FAIL);
		}

		hr = m_pDevice->CreatePixelShader( (DWORD*)pBuffer->GetBufferPointer(), pBuffer->GetBufferSize(), NULL, &m_pCoveragePS );
		if(FAILED(hr))
		{
			throw TRException("CreatePixelShader failed", hr, RESULT_FAIL);
		}

		
		D3D11_TEXTURE2D_DESC DepthBufferDesc;
		m_pRTTexture2D->GetDesc(&DepthBufferDesc);

		// Check to see if depth buffer format is supported
		if( (DepthBufferDesc.SampleDesc.Quality != D3D11_STANDARD_MULTISAMPLE_PATTERN) &&
			(DepthBufferDesc.SampleDesc.Quality != D3D11_CENTER_MULTISAMPLE_PATTERN) )
		{
			UINT NumQualityLevelsSupported = 0;

			hr = m_pDevice->CheckMultisampleQualityLevels( m_DepthBufferFormat, DepthBufferDesc.SampleDesc.Count, &NumQualityLevelsSupported );
			
			if( FAILED(hr) )
			{
				NumQualityLevelsSupported = 0;
			}

			if( DepthBufferDesc.SampleDesc.Quality >= NumQualityLevelsSupported  )
			{
				throw TRException("Depth buffer format not supported", RESULT_SKIP);
			}
		}


		// Create depth buffer
		DepthBufferDesc.Format    = m_DepthBufferFormat;
		DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		DepthBufferDesc.MiscFlags = 0;

		hr = GetFramework()->CreateTexture2DTiledOptional( &DepthBufferDesc, NULL, &m_pDepthBuffer );
		if( FAILED(hr) )
		{
			throw TRException("CreateTexture2D failed", hr, RESULT_FAIL);
		}

		// Create depth/stencil view
		hr = m_pDevice->CreateDepthStencilView( m_pDepthBuffer, NULL, &m_pDSV );
		if( FAILED(hr) )
		{
			throw TRException("CreateDepthStencilView failed", hr, RESULT_FAIL);
		}

		// Create depth/stencil state
		D3D11_DEPTH_STENCIL_DESC DepthStencilStateDesc = CD3D11_DEPTH_STENCIL_DESC( D3D11_DEFAULT );
		DepthStencilStateDesc.DepthEnable = TRUE;
		DepthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DepthStencilStateDesc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
		DepthStencilStateDesc.StencilEnable = FALSE;

		hr = m_pDevice->CreateDepthStencilState( &DepthStencilStateDesc, &m_pDepthStencilState );
		if(FAILED(hr))
		{
			throw TRException("CreateDepthStencilState failed", hr, RESULT_FAIL);
		}

		//Update Sample Locations
		if ( UpdateFormatOrSampleDesc() )
		{
			if( FAILED( m_Finder.FindSamples() ) )
				throw TRException("FindSamples failed.");
			// Set state back to CMultisampleTest default (FindSamples changed the state)
			ResetStates();
		}
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();

		exc.Prepend("[CInputCoverageWithDepthTest::SetupTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}

	SAFE_RELEASE(pBuffer);
	SAFE_RELEASE(pError);

	return tRes;
}

void CInputCoverageWithDepthTest::CleanupTestCase()
{
	SAFE_RELEASE(m_pCoveragePS);
	SAFE_RELEASE(m_pDepthBuffer);
	SAFE_RELEASE(m_pDSV);
	SAFE_RELEASE(m_pDepthStencilState);

	CMultisampleTest::CleanupD3D();
}

TEST_RESULT CInputCoverageWithDepthTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	try
	{
		// Fill the depth buffer with values such only that the samples in m_DepthMask will pass the depth test
		FillDepthBuffer();

		UINT CoverageValue[2] = {0, 0};

		// Render twice
		// In the first pass, do not bind the depth buffer
		// In the second pass, do bind the depth buffer
		// Save the value of SV_COVERAGE PS input on each pass
		for(UINT i = 0; i < 2; i++)
		{
			FLOAT ClearColor[4] = { 0, 0, 0, 0 };
			GetEffectiveContext()->ClearRenderTargetView( m_pRTView, ClearColor );
			GetEffectiveContext()->PSSetShader( m_pCoveragePS, NULL, 0 );
			GetEffectiveContext()->OMSetDepthStencilState( m_pDepthStencilState, 0 );
			GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, (i == 1) ? m_pDSV : NULL );

			m_uVertexCount = 0;
			CoverHalfPixel( m_TargetPixelX, m_TargetPixelY );
			GetEffectiveContext()->Draw( m_uVertexCount, 0 );

			CoverageValue[i] = ReadCoverageValue();

			// ReadCoverageValue changes device state
			ResetStates();
		}

		// The value of SV_COVERAGE should be the same for both cases
		if( CoverageValue[1] && (CoverageValue[0] != CoverageValue[1]) )
		{
			WriteToLog( "SV_COVERAGE without depth test = %u, SV_COVERAGE with depth test = %u\n", CoverageValue[0], CoverageValue[1] );
			throw TRException( "Depth test results affected SV_COVERAGE PS input" );
		}
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CInputCoverageWithDepthTest::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}
	
	return tRes;
}

UINT CInputCoverageWithDepthTest::ReadCoverageValue()
{
	// Executed deferred context
	HRESULT hr;
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
		throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

	// Read contents of render target
	UINT BytesPerPixel = GetBitsPerElement( m_RTVFormat ) / 8;
	UINT RowPitch = BytesPerPixel * m_RTWidth;
	UINT SlicePitch = RowPitch * m_RTHeight;

	vector<BYTE> vBuffer( m_RTWidth * m_RTHeight * m_uSampleCount * BytesPerPixel );
	if ( !MSHelper::ReadWithoutConversion(GetFramework(), GetFramework()->GetRTTexture(), &vBuffer[0], m_RTVFormat) )
	{
		throw TRException("MSHelper::ReadWithoutConversion failed");
	}

	CONST BYTE* BufferStart = &vBuffer[0];

	CONST BYTE* Curr = BufferStart;

	UINT Result = 0;

	// SV_COVERAGE input to PS must be the same for all samples that were lit
	for( UINT s = 0; s < m_uSampleCount; s++)
	{
		for( UINT y = 0; y < (UINT)m_RTHeight; y++ )
		{
			for( UINT x = 0; x < (UINT)m_RTWidth; x++ )
			{
				UINT RedChannel = GetRedChannel( m_RTVFormat, Curr );

				if( Result && RedChannel )
				{
					if( Result != RedChannel )
					{
						WriteToLog( "SV_COVERAGE[0] = %u, SV_COVERAGE[1] = %u\n",
									Result,
									RedChannel );
						throw TRException("2 samples in the same primitive contained different values of SV_COVERAGE");
					}
				}

				if( RedChannel )
				{
					Result = RedChannel;
				}

				Curr += BytesPerPixel;                
			}
		}
	}

	return Result;
}

// Sets the values in the depth buffer such that only samples with the bit set in m_DepthMask will pass the depth test
void CInputCoverageWithDepthTest::FillDepthBuffer()
{
	GetEffectiveContext()->OMSetRenderTargets( 0, NULL, m_pDSV);
	GetEffectiveContext()->OMSetDepthStencilState( m_pDepthStencilState, 0 );
	GetEffectiveContext()->ClearDepthStencilView(m_pDSV, D3D10_CLEAR_DEPTH, 0.5f, 0); // don't clear to 1.0f, because that would cause all samples to fail the depth test

	for( UINT s = 0; s < m_uSampleCount; s++ )
	{
		if( 0 == (m_DepthMask & (1 << s)) )
		{
			m_uVertexCount = 0;
			CoverSample( m_TargetPixelX, m_TargetPixelY, s );
			GetEffectiveContext()->Draw( m_uVertexCount, 0 );
		}
	}
}
