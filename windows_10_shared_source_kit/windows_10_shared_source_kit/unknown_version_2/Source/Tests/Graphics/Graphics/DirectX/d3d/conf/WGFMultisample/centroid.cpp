//////////////////////////////////////////////////////////////////////////
//  Centroid.cpp
//  created:	2005/07/18
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "centroid.h"

void CMultisampleTest_Centroid::InitTestParameters()
{
	InitMultisampleParameters();

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

	// Forced sample count (part of the rasterizer desc)
	// With a forced sample count/pattern selected at the rasterizer (ForcedSampleCount > 0), 
	// pixels are candidates for shader invocation based on the selected sample pattern, independent of the RTV ("output") sample count
	m_pForcedSampleCountParam = AddParameter( _T( "Forced_Sample_Count" ), &m_uForcedSampleCount );
	testfactor::RFactor rfForcedSampleCount0 = AddParameterValue<UINT>( m_pForcedSampleCountParam, 0);	// 0 means the feature is off and rendering is 'normal'
	testfactor::RFactor rfForcedSampleCount1 = AddParameterValue<UINT>( m_pForcedSampleCountParam, 1);
	testfactor::RFactor rfForcedSampleCount4 = AddParameterValue<UINT>( m_pForcedSampleCountParam, 4);
	testfactor::RFactor rfForcedSampleCount8 = AddParameterValue<UINT>( m_pForcedSampleCountParam, 8);
	testfactor::RFactor rfForcedSampleCount16 = AddParameterValue<UINT>( m_pForcedSampleCountParam, 16);

	CTestCaseParameter< UINT > *pSIndex = AddParameter( _T( "SampleIndex" ), &m_uSampleIndex );
	pSIndex->SetDefaultValue( 0 );
	testfactor::RFactor sindex1 = AddParameterValue<UINT>( pSIndex, 0);
	testfactor::RFactor sindex2 = AddParameterValueSet<UINT>( pSIndex, new CRangeValueSet< UINT > (0, 1, 1) );
	testfactor::RFactor sindex4 = AddParameterValueSet<UINT>( pSIndex, new CRangeValueSet< UINT > (0, 3, 1) );
	testfactor::RFactor sindex8 = AddParameterValueSet<UINT>( pSIndex, new CRangeValueSet< UINT > (0, 7, 1) );
	testfactor::RFactor sindex16 = AddParameterValueSet<UINT>( pSIndex, new CRangeValueSet< UINT > (0, 15, 1) );
	testfactor::RFactor sindex32 = AddParameterValueSet<UINT>( pSIndex, new CRangeValueSet< UINT > (0, 31, 1) );
	testfactor::RFactor bcentroid = AddParameter( _T( "CentroidEnable" ), &m_bCentroid, new CBoolValueSet() );

	testfactor::RFactor rfSampleIndexAndForcedSampleCount =
		sindex1 * rfForcedSampleCount0 + 
		sindex1 * rfForcedSampleCount1 + 
		sindex4 * rfForcedSampleCount4 + 
		sindex8 * rfForcedSampleCount8 + 
		sindex16 * rfForcedSampleCount16;

	testfactor::RFactor rfSampleDescIndex = 
		rfSampleCount1 * rfQualityLevelZero * rfSampleIndexAndForcedSampleCount + 
		// only sample counts 1(trival), 2, 4, 8 and 16 support standard and center sample patterns
		rfSampleCount2 * rfQualityLevelAll * sindex2 * rfForcedSampleCount0 +
		rfSampleCount2 * rfQualityLevelAll * sindex1 * rfForcedSampleCount1 + // If ForcedSampleCount is 1, RTVs that are bound can be any sample count

		rfSampleCount4 * rfQualityLevelAll * sindex4 * rfForcedSampleCount0 +
		rfSampleCount4 * rfQualityLevelAll * sindex1 * rfForcedSampleCount1 + 

		rfSampleCount8 * rfQualityLevelAll * sindex8 * rfForcedSampleCount0 +
		rfSampleCount8 * rfQualityLevelAll * sindex1 * rfForcedSampleCount1 + 

		rfSampleCount16 * rfQualityLevelAll * sindex16 * rfForcedSampleCount0 +
		rfSampleCount16 * rfQualityLevelAll * sindex1 * rfForcedSampleCount1 + 

		rfSampleCount32 * ( rfQualityLevelZero + rfQualityLevelMaxSupported ) * sindex32 * rfForcedSampleCount0 +
		rfSampleCount32 * ( rfQualityLevelZero + rfQualityLevelMaxSupported ) * sindex1 * rfForcedSampleCount1;

	SetRootTestFactor ( ( m_rfRTFormatTypedSubset % m_rfPixPos ) * rfSampleDescIndex * bcentroid  );
}

TEST_RESULT CMultisampleTest_Centroid::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	ID3D10Blob *pBuf = NULL;

	tRes = SetFormatAndQualityLevel( false );
	if ( tRes != RESULT_PASS )
	{
		return tRes;
	}

	try
	{
		tRes = SetupD3D();
		if( RESULT_PASS != tRes )
		{
			return tRes;
		}
		
		D3D11_TEXTURE2D_DESC tex2ddesc;
		tex2ddesc.ArraySize = 1;
		tex2ddesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tex2ddesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tex2ddesc.Height = 1;
		tex2ddesc.Width = 1;
		tex2ddesc.CPUAccessFlags = 0;
		tex2ddesc.MipLevels = 1;
		tex2ddesc.MiscFlags = 0;
		tex2ddesc.SampleDesc.Count = 1;
		tex2ddesc.SampleDesc.Quality = 0;
		tex2ddesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA pData;
		const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(DXGI_FORMAT_R8G8B8A8_UNORM);
		pData.pSysMem = new UINT(0xFFFFFFFF);
		pData.SysMemPitch = pTexFormatInfo->uBitsPerElement/8;
		pData.SysMemSlicePitch = pTexFormatInfo->uBitsPerElement/8;

		GetFramework()->CreateTexture2DTiledOptional( &tex2ddesc, &pData, &m_pTexture );

		delete pData.pSysMem;

		D3D11_SHADER_RESOURCE_VIEW_DESC	svDesc;
		svDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		svDesc.Texture2D.MipLevels = 1;
		svDesc.Texture2D.MostDetailedMip = 0;
		svDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		m_pDevice->CreateShaderResourceView( m_pTexture, &svDesc, &m_pTexView );

		// create sampler
		D3D11_SAMPLER_DESC desc;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.BorderColor[0] = 0.0f;
		desc.BorderColor[1] = 0.0f;
		desc.BorderColor[2] = 0.0f;
		desc.BorderColor[3] = 0.0f;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;

		if( FAILED( m_pDevice->CreateSamplerState(&desc, &m_pSampler) ) )
			throw TRException("CreateSamplerState Failed.");

		// Create the centroid test pixel shader
		tstring filename("ps_centroid_on.psh");
		if (!m_bCentroid) filename = "ps_centroid_off.psh";
		if ( FAILED( D3DX10CompileFromResource( NULL, filename.c_str(), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, NULL, &pBuf, NULL, NULL) ) )
			throw TRException("D3D10CompileShaderFromResource failed.");

		if ( FAILED( m_pDevice->CreatePixelShader( (DWORD*) pBuf->GetBufferPointer(), pBuf->GetBufferSize(), NULL, &m_pPSCentroid ) ) )
			throw TRException("CreatePixelShader failed.");

		//Update Sample Locations
		if ( UpdateFormatOrSampleDesc() )
		{
			if( FAILED( m_Finder.FindSamples() ) )
				throw TRException("FindSamples failed.");
			ResetStates();
		}
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Centroid::SetupTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}
	
	SAFE_RELEASE( pBuf );
	return tRes;
}

TEST_RESULT CMultisampleTest_Centroid::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	try
	{
		m_pDeviceContext->PSSetShaderResources( 0, 1, &m_pTexView );
		m_pDeviceContext->PSSetSamplers( 0, 1, &m_pSampler);
		
		// setup geometry
		m_uVertexCount = 0;
		CoverSample( m_uSampleIndex );

		// clear
		FLOAT ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
		m_pDeviceContext->PSSetShader( m_pPSCentroid , NULL, 0);

		// disable depth if needed
		if( m_uForcedSampleCount > 0 )
		{
			// Depth/Stencil Views must not be bound, depth testing must be disabled, and the shader must not output depth while rendering with ForcedSampleCount 1 or greater, otherwise rendering behavior is undefined
			m_pDeviceContext->OMSetDepthStencilState( m_pDepthStateDisabled, 0 );
		}
		else
		{
			m_pDeviceContext->OMSetDepthStencilState( m_pDepthStateDefault, 0 );
		}

		// render
		m_pDeviceContext->Draw(m_uVertexCount, 0);

		if ( FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

		// check resulting pixel
		BOOL bResult;
		if( RESULT_PASS != m_Finder.TestPixelNonZero(bResult) )
			throw TRException("TestPixelNonZero failed.");
		Validate(bResult);
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Centroid::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}
	
	try
	{
		Present();
	}
	catch (TRException& exc)
	{
		exc.Prepend("[CMultisampleTest_Centroid::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}	

	return tRes;	
}

BOOL CMultisampleTest_Centroid::Validate(BOOL bResult)
{
	//Special case for covering the pixel center
	if ((m_Finder.GetLoc(m_uSampleIndex).x == 128) &&
		(m_Finder.GetLoc(m_uSampleIndex).y == 128))
	{
		if ( !bResult )
			throw TRException("[CMultisampleTest_Centroid::Validate] Pixel center is covered but the pixel is not lit.");
		else
			return true;
	}

	if ( bResult )
	{
		if ( !m_bCentroid )
			throw TRException("[CMultisampleTest_Centroid::Validate] Centroid disabled and sampling within the gamut.");
	}
	else
	{
		if ( m_bCentroid )
			throw TRException("[CMultisampleTest_Centroid::Validate] Centroid enabled and not sampling within the gamut.");
	}
	
	return true;	
}

void CMultisampleTest_Centroid::CleanupTestCase() 
{
	ID3D11SamplerState *pNullState[] = { NULL, NULL, NULL, NULL };
	ID3D11ShaderResourceView *pSRVNull[] = { NULL, NULL, NULL, NULL };
	
	m_pDeviceContext->PSSetShader(NULL, NULL, 0);
	m_pDeviceContext->PSSetShaderResources(0, 1, pSRVNull);
	m_pDeviceContext->PSSetSamplers(0, 1, pNullState);
	SAFE_RELEASE( m_pPSCentroid );
	SAFE_RELEASE( m_pTexture );
	SAFE_RELEASE( m_pTexView );
	SAFE_RELEASE( m_pSampler );
	CMultisampleTest::CleanupD3D();
}

//////////////////////////////////////////////////////////////////////////
//
// CMultisampleTest_Centroid11
//
//////////////////////////////////////////////////////////////////////////

BEGIN_NAMED_VALUES(CentroidInterpolationMode)
	NAMED_VALUE("Linear", CentroidInterpolationMode_Linear)
	NAMED_VALUE("Linear_NoPerspective", CentroidInterpolationMode_Linear_NoPerspective)
END_NAMED_VALUES(CentroidInterpolationMode)

BEGIN_NAMED_VALUES(TriangleOffsetAxis)
	NAMED_VALUE("X", TriangleOffsetAxis_X)
	NAMED_VALUE("Y", TriangleOffsetAxis_Y)
END_NAMED_VALUES(TriangleOffsetAxis)

const INT8 FullyCovered = -1;

const float STDMS1Offsets[1 * 2] =
{ .5f, .5f, };
const UINT16 STDMS1XMaskCovered[2] =
{ 0x0, 0x1 };
const UINT16 STDMS1YMaskCovered[2] =
{ 0x0, 0x1 };

const float STDMS2Offsets[2 * 2] =
{
	.75f, .75f,
	.25f, .25f,
};
const UINT16 STDMS2XMaskCovered[3] =
{ 0x0, 0x2, 0x3 };
const UINT16 STDMS2YMaskCovered[3] =
{ 0x0, 0x2, 0x3 };

const float STDMS4Offsets[4 * 2] =
{
	.375f, .125f,
	.875f, .375f,
	.125f, .625f,
	.625f, .875f,
};
const UINT16 STDMS4XMaskCovered[5] =
{ 0x0, 0x4, 0x5, 0xd, 0xf };
const UINT16 STDMS4YMaskCovered[5] =
{ 0x0, 0x1, 0x3, 0x7, 0xf };

const float STDMS8Offsets[8 * 2] =
{
	.5625f, .3125f,
	.4375f, .6875f,
	.8125f, .5625f,
	.3125f, .1875f,
	.1875f, .8125f,
	.0625f, .4375f,
	.6875f, .9375f,
	.9375f, .0625f,
};
const UINT16 STDMS8XMaskCovered[9] =
{ 0x0, 0x20, 0x30, 0x38, 0x3a, 0x3b, 0x7b, 0x7f, 0xff };
const UINT16 STDMS8YMaskCovered[9] =
{ 0x0, 0x80, 0x88, 0x89, 0xa9, 0xad, 0xaf, 0xbf, 0xff };

const float STDMS16Offsets[16 * 2] =
{
	.5625f, .5625f,
	.4375f, .3125f,
	.3125f, .625f,
	.75f, .4375f,
	.1875f, .375f,
	.625f, .8125f,
	.8125f, .6875f,
	.6875f, .1875f,
	.375f, .875f,
	.5f, .0625f,
	.25f, .125f,
	.125f, .75f,
	.0f, .5f,
	.9375f, .25f,
	.875f, .9375f,
	.0625f, .0f,
};
const UINT16 STDMS16XMaskCovered[17] =
{ 0x0, 0x1000, 0x9000, 0x9800, 0x9810, 0x9c10, 0x9c14, 0x9d14, 0x9d16, 0x9f16, 0x9f17, 0x9f37, 0x9fb7, 0x9fbf, 0x9fff, 0xdfff, 0xffff };
const UINT16 STDMS16YMaskCovered[17] =
{ 0x0, 0x8000, 0x8200, 0x8600, 0x8680, 0xa680, 0xa682, 0xa692, 0xa69a, 0xb69a, 0xb69b, 0xb69f, 0xb6df, 0xbedf, 0xbeff, 0xbfff, 0xffff };

void CMultisampleTest_Centroid11::InitTestParameters()
{
	InitMultisampleParameters();

	m_PixPosID = PP_TOP_LEFT;
	m_ReadBack = MS_LOAD;

	// Sample counts and quality levels
	// only sample counts 1(trival), 2, 4, 8 and 16 support standard and center sample patterns
	// Centroid definition only applies to standard and center sample patterns
	// SampleMask is relevant to the output sample pattern only, not the TIR ForcedSampleCount pattern
	const UINT numSampleCounts = 5;
	CTestCaseParameter< INT > *pMinSIndex = AddParameter( _T( "MinCoveredSampleFromMask" ), &m_MinCoveredSampleFromMask );
	pMinSIndex->SetDefaultValue( FullyCovered );
	testfactor::RFactor rfSampleCount[numSampleCounts];
	testfactor::RFactor rfSindex[numSampleCounts];
	for( UINT i = 0; i < numSampleCounts; i++ )
	{
		rfSampleCount[i] = AddParameterValue<UINT>( m_pSampleCountParam, (UINT)pow((float)2, (float)i) );
		rfSindex[i] = AddParameterValueSet<INT>( pMinSIndex, 
			new CRangeValueSet< INT >(FullyCovered, (UINT)pow((float)2, (float)i), 1) );
	}
	
	// Forced sample count (part of the rasterizer desc)
	// With a forced sample count/pattern selected at the rasterizer (ForcedSampleCount > 0), 
	// pixels are candidates for shader invocation based on the selected sample pattern, independent of the RTV ("output") sample count
	m_pForcedSampleCountParam = AddParameter( _T( "Forced_Sample_Count" ), &m_uForcedSampleCount );
	testfactor::RFactor rfForcedSampleCount0 = AddParameterValue<UINT>( m_pForcedSampleCountParam, 0);	// 0 means the feature is off and rendering is 'normal'
	testfactor::RFactor rfForcedSampleCount1 = AddParameterValue<UINT>( m_pForcedSampleCountParam, 1);
	testfactor::RFactor rfForcedSampleCount4 = AddParameterValue<UINT>( m_pForcedSampleCountParam, 4);
	testfactor::RFactor rfForcedSampleCount8 = AddParameterValue<UINT>( m_pForcedSampleCountParam, 8);
	testfactor::RFactor rfForcedSampleCount16 = AddParameterValue<UINT>( m_pForcedSampleCountParam, 16);

	testfactor::RFactor rfQualityLevelStandard = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_STANDARD );
	testfactor::RFactor rfQualityLevelCenter = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_CENTER );
		
	CTestCaseParameter<CentroidInterpolationMode>* pCentroidInterpolationModeParam =
		AddParameter<CentroidInterpolationMode>( _T("InterpolationMode"), &m_CentroidInterpolationMode );
	pCentroidInterpolationModeParam->SetDefaultValue( CentroidInterpolationMode_Linear);
	testfactor::RFactor rInterp_Linear =
		AddParameterValue<CentroidInterpolationMode>( pCentroidInterpolationModeParam, CentroidInterpolationMode_Linear );
	testfactor::RFactor rInterp_Linear_NoPerspective =
		AddParameterValue<CentroidInterpolationMode>( pCentroidInterpolationModeParam, CentroidInterpolationMode_Linear_NoPerspective );

	CTestCaseParameter<TriangleOffsetAxis>* pTriangleOffsetAxisParam =
		AddParameter<TriangleOffsetAxis>( _T("TriangleOffsetAxis"), &m_TriangleOffsetAxis );
	pTriangleOffsetAxisParam->SetDefaultValue( TriangleOffsetAxis_X );
	testfactor::RFactor rOffset_X =
		AddParameterValue<TriangleOffsetAxis>( pTriangleOffsetAxisParam, TriangleOffsetAxis_X );
	testfactor::RFactor rOffset_Y =
		AddParameterValue<TriangleOffsetAxis>( pTriangleOffsetAxisParam, TriangleOffsetAxis_Y );

	CTestCaseParameter< INT > *pTriOffsetParam = AddParameter( _T( "TriangleOffset" ), &m_TriangleOffset );
	pTriOffsetParam->SetDefaultValue( 0 );
	testfactor::RFactor rfTriOffsetStandard[numSampleCounts];
	testfactor::RFactor rfTriOffsetCenter[numSampleCounts];
	for(size_t i = 0; i < numSampleCounts; i++)
	{
		rfTriOffsetCenter[i] =
			AddParameterValueSet( pTriOffsetParam, new CRangeValueSet< INT >( 0, 
													(UINT)pow((float)2, (float)i), 
													(UINT)pow((float)2,(float) i) ) );
		
		// Add special case for 16 samples, to catch sample index 10 as min geometry covered sample.
		rfTriOffsetStandard[i] = AddParameterValueSet( pTriOffsetParam,
			new CRangeValueSet< INT >( -INT( (UINT)pow((float)2, (float)i) ) + (UINT)(pow((float)2, (float)i) == 16 ? 0 : 1), 
			(UINT)pow((float)2, (float)i), 
			1 ) );
	}

	testfactor::RFactor rfForcedSampleCountsOffset = 
		rfForcedSampleCount0 * rfTriOffsetStandard[0] + 
		rfForcedSampleCount1 * rfTriOffsetStandard[0] +
		rfForcedSampleCount4 * rfTriOffsetStandard[2] + 
		rfForcedSampleCount8 * rfTriOffsetStandard[3]+ 
		rfForcedSampleCount16 * rfTriOffsetStandard[4];

	testfactor::RFactor rfSampleDescIndexOffset = 
			rfQualityLevelStandard * rfSampleCount[0] * rfSindex[0] * rfForcedSampleCountsOffset  + 
			rfQualityLevelCenter * rfSampleCount[0] * rfSindex[0] * rfForcedSampleCountsOffset;
	for(size_t i = 1; i < numSampleCounts; i++)
	{
		rfSampleDescIndexOffset = rfSampleDescIndexOffset + 
			rfQualityLevelStandard * rfSampleCount[i] * rfSindex[i] * rfForcedSampleCount0 * rfTriOffsetStandard[i]  +
			rfQualityLevelStandard * rfSampleCount[i] * rfSindex[0] * rfForcedSampleCount1 * rfTriOffsetStandard[0]  + 
			rfQualityLevelCenter * rfSampleCount[i] * rfSindex[i] * rfForcedSampleCount0 * rfTriOffsetCenter[i];
	}

	testfactor::RFactor rfRTFormat = AddParameterValue<DXGI_FORMAT>( m_pRTFormatParam, DXGI_FORMAT_R8G8_UNORM);

	SetRootTestFactor( rfRTFormat * rfSampleDescIndexOffset *
		(rInterp_Linear + rInterp_Linear_NoPerspective) *
		(rOffset_X + rOffset_Y)  );

	// Sample-frequency Pixel Shader execution is undefined when sample pattern is CENTER and 
	// more than one sample overlapping at the center of the pixel. 
	CFilterHandle skipMatchingFilter = ( 
		FilterGreater<UINT>( _T("Sample_Count"), 1 ) 
		&& FilterEqual<UINT> ( _T("Forced_Sample_Count"), 0 ) 
		&& FilterEqual<QUALITY_LEVEL_SCENARIO> ( _T("Quality_Level"), QUALITY_LEVEL_CENTER ) );
	SkipMatching( skipMatchingFilter );
}

TEST_RESULT CMultisampleTest_Centroid11::SetupPipeline()
{
	try
	{
		const bool bTIRIsEnabled = m_uForcedSampleCount > 0;
		TEST_RESULT tRes = SetupD3D();
		if (tRes != RESULT_PASS)
			throw TRException( "CMultisampleTest::SetupD3D failed.", tRes );

		m_FullyCoveredSampleMask = (0x1 << m_uEffectiveSampleCount) - 1;
		switch (m_uEffectiveSampleCount)
		{
		case (1):
			m_pSamplePixelOffsets = STDMS1Offsets;
			m_pSamplesCoveredByTriangleXOffset = STDMS1XMaskCovered;
			m_pSamplesCoveredByTriangleYOffset = STDMS1YMaskCovered;
			break;
			
		case (2):
			m_pSamplePixelOffsets = STDMS2Offsets;
			m_pSamplesCoveredByTriangleXOffset = STDMS2XMaskCovered;
			m_pSamplesCoveredByTriangleYOffset = STDMS2YMaskCovered;
			break;
			
		case (4):
			m_pSamplePixelOffsets = STDMS4Offsets;
			m_pSamplesCoveredByTriangleXOffset = STDMS4XMaskCovered;
			m_pSamplesCoveredByTriangleYOffset = STDMS4YMaskCovered;
			break;
			
		case (8):
			m_pSamplePixelOffsets = STDMS8Offsets;
			m_pSamplesCoveredByTriangleXOffset = STDMS8XMaskCovered;
			m_pSamplesCoveredByTriangleYOffset = STDMS8YMaskCovered;
			break;
			
		case (16):
			m_pSamplePixelOffsets = STDMS16Offsets;
			m_pSamplesCoveredByTriangleXOffset = STDMS16XMaskCovered;
			m_pSamplesCoveredByTriangleYOffset = STDMS16YMaskCovered;
			break;

		default: assert( false );
		}

		// Render Test:
		m_pDeviceContext->RSSetState( m_pRSEnabled );
		m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		m_pDeviceContext->OMSetBlendState( m_pBS, 0, D3D11_DEFAULT_SAMPLE_MASK );
		m_pDeviceContext->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		m_pDeviceContext->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		m_pDeviceContext->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		m_pDeviceContext->VSSetShader( m_pVS, NULL, 0 );
		m_pDeviceContext->GSSetShader( NULL, NULL, 0 );
		m_pDeviceContext->PSSetShader( m_pPS, NULL, 0 );
		m_pDeviceContext->IASetInputLayout( m_pVertexLayout );

		// Disable depth if needed
		if( bTIRIsEnabled )
		{
			// Depth/Stencil Views must not be bound, depth testing must be disabled, and the shader must not output depth while rendering with ForcedSampleCount 1 or greater, otherwise rendering behavior is undefined
			m_pDeviceContext->OMSetDepthStencilState( m_pDepthStateDisabled, 0 );
		}
		else
		{
			m_pDeviceContext->OMSetDepthStencilState( m_pDepthStateDefault, 0 );
		}

		UINT strides[] = { sizeof( VERTEX ) };
		UINT offsets[] = { 0 };
		m_pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, strides, offsets );

		CD3D11_VIEWPORT vp = CD3D11_VIEWPORT( 0.f, 0.f, (float)m_RTWidth, (float)m_RTHeight );
		m_pDeviceContext->RSSetViewports( 1, &vp );
		m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, NULL );

		float *bufData;
		D3D11_MAPPED_SUBRESOURCE mappedRes;
		if( FAILED( m_pDeviceContext->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
			throw TRException("Map on m_pConstBuffer Failed");
		bufData = (float*)mappedRes.pData;
		// Prescaled inverse viewport translation.
		bufData[0] = -1.f;
		bufData[1] = 1.f;
		// Inverse viewport scale.
		bufData[2] = 2.f / (float)m_RTWidth;
		bufData[3] = -2.f / (float)m_RTHeight;
		// float alpha
		bufData[4] = 0.f;
		// float depth
		bufData[5] = 0.f;
		m_pDeviceContext->Unmap(m_pConstBuffer,0);

		CD3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc =
			CD3D11_SHADER_RESOURCE_VIEW_DESC( m_pRTTexture2D, D3D11_SRV_DIMENSION_TEXTURE2DMS );
		if ( FAILED(m_pDevice->CreateShaderResourceView( m_pRTTexture2D, (m_uSampleCount == 1 ? &SRVDesc : NULL), &m_pSRView )) )
			throw TRException("CreateShaderResourceView failed for m_pSRView.");

		const UINT MAX_STR = 4000;
		char szShader[ MAX_STR ];

		for (CentroidInterpolationMode m = CentroidInterpolationMode( 0 ); m < CentroidInterpolationMode_Max; ++m)
		{
			// Centroid Attribute Evaluation PS
			// Forces super-sampling with usage of SV_SampleIndex (per-sample invocation); but
			// alpha should be 0, no-op'ing the affect on the expression.
			// Stuffs centroid-evaluted tex into color.
			char szPS[] = 
				"struct PSIn																\n"
				"{																			\n"
				"	float4 pos : SV_Position;												\n"
				"	centroid %sfloat2 tex : tex;											\n"
				"	%s																		\n"
				"};																			\n"
				"struct PSOut																\n"
				"{																			\n"
				"	float4 Color : SV_Target;												\n"
				"};																			\n"
				"cbuffer cb0 : register(b0)													\n"
				"{																			\n"
				"	float4 inverseViewport;													\n"
				"	float alpha;															\n"
				"};																			\n"
				"PSOut main(PSIn input)														\n"
				"{																			\n"
				"	PSOut output;															\n"
				"	output.Color = float4(0,0,0,1);											\n"
				"	%s																		\n"
				"	return output;															\n"
				"}";	

			ID3D10Blob *pPSBuf = NULL;
			ID3D10Blob *pErrorBuf = NULL;
			// construct PS, requires output type info
			_snprintf( szShader, MAX_STR-1, szPS,
				(m == CentroidInterpolationMode_Linear_NoPerspective ? "noperspective " : ""),
				(!bTIRIsEnabled ? "uint si : SV_SampleIndex;" : "" ),	// If TIR is enabled, Sample-frequency shader invocation cannot be requested, otherwise rendering results are undefined
				(!bTIRIsEnabled ? "output.Color.xy = input.tex.xy + alpha * input.si;" : "output.Color.xy = input.tex.xy;" ) );
			szShader[MAX_STR-1] = 0;
			if ( FAILED(D3DX11CompileFromMemory( szShader, strlen(szShader), NULL, NULL, NULL, "main",
				GetFramework()->GetShaderProfile( D3D_SHADER_STAGE_PIXEL, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0),
				D3D10_SHADER_DEBUG, 0, NULL, &pPSBuf, &pErrorBuf, NULL )) )
				throw TRException( tstring("Failed to compile PS.") + (pErrorBuf ? (char*)pErrorBuf->GetBufferPointer() : "") );
			SAFE_RELEASE( pErrorBuf );
			if ( FAILED(m_pDevice->CreatePixelShader( pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &m_pPSCentroid[ m ] )) )
			{
				SAFE_RELEASE( pPSBuf );
				throw TRException("Failed to create PS.");
			}
			SAFE_RELEASE( pPSBuf );
		}

		// Sample Readback with Load:
		// New Render Target & Staging pair
		D3D11_TEXTURE2D_DESC rtdesc;
		m_pRTTexture2D->GetDesc( &rtdesc );

		rtdesc.Width *= rtdesc.SampleDesc.Count;
		rtdesc.SampleDesc.Count = 1;
		rtdesc.SampleDesc.Quality = 0;
		if ( FAILED(GetFramework()->CreateTexture2DTiledOptional( &rtdesc, NULL, &m_pRTTexture2DSS )) )
			throw TRException("CreateTexture2D failed for m_pRTTexture2DSS.");

		if ( FAILED(m_pDevice->CreateRenderTargetView( m_pRTTexture2DSS, NULL, &m_pSSRTView )) )
			throw TRException("CreateRenderTargetView failed for m_pSSRTView.");

		rtdesc.Usage = D3D11_USAGE_STAGING;
		rtdesc.BindFlags = 0;
		rtdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		if ( FAILED(GetFramework()->CreateTexture2DTiledOptional( &rtdesc, NULL, &m_pSTexture2DSS )) )
			throw TRException("CreateTexture2D failed for m_pSTexture2DSS.");

		// New PS
		char szMSPS[] =
			"Texture2DMS< float2, %d > MyTex;											\n"
			"struct PSIn																\n"
			"{																			\n"
			"	float4 pos : SV_Position;												\n"
			"	float2 tex : tex;														\n"
			"};																			\n"
			"struct PSOut																\n"
			"{																			\n"
			"	float4 Color : SV_Target;												\n"
			"};																			\n"
			"cbuffer cb0 : register(b0)													\n"
			"{																			\n"
			"	float4 inverseViewport;													\n"
			"	float alpha;															\n"
			"};																			\n"
			"PSOut main(PSIn input)														\n"
			"{																			\n"
			"	uint iX = input.pos.x;													\n"
			"	uint iS = %d;															\n"
			"	PSOut output;															\n"
			"	output.Color = float4(0,0,0,1);											\n"
			"	output.Color.xy = MyTex.Load( int3(iX/iS,input.pos.y,0), iX%%iS ).xy;	\n"
			"	return output;															\n"
			"}";

		// construct PS, requires output type info
		ID3D10Blob *pPSBuf = NULL;
		ID3D10Blob *pErrorBuf = NULL;
		_snprintf( szShader, MAX_STR-1, szMSPS, m_uSampleCount, m_uSampleCount );
		szShader[MAX_STR-1] = 0;
		if ( FAILED(D3DX11CompileFromMemory( szShader, strlen(szShader), NULL, NULL, NULL, "main", "ps_4_1", D3D10_SHADER_DEBUG, 0, NULL, &pPSBuf, &pErrorBuf, NULL )) )
			throw TRException( tstring("Failed to compile PS.") + (pErrorBuf ? (char*)pErrorBuf->GetBufferPointer() : "") );
		SAFE_RELEASE( pErrorBuf );
		if ( FAILED(m_pDevice->CreatePixelShader( pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &m_pPSConvertMSToSS )) )
		{
			SAFE_RELEASE( pPSBuf );
			throw TRException("Failed to create PS.");
		}
		SAFE_RELEASE( pPSBuf );

		return tRes;
	}
	catch (TRException& exc)
	{
		exc.Prepend("[CMultisampleTest_Centroid11::SetupPipeline()] ");
		if (exc.GetResult() == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
		return exc.GetResult();
	}
}

TEST_RESULT CMultisampleTest_Centroid11::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	tRes = SetFormatAndQualityLevel( false );
	if ( tRes != RESULT_PASS )
	{
		return tRes;
	}

	// Determine the effective sample count (if TIR is ON or OFF)
	m_uEffectiveSampleCount = ( m_uForcedSampleCount > 0 ) ? m_uForcedSampleCount : m_uSampleCount;

	tRes = SetupPipeline();
	if (tRes != RESULT_PASS)
	{
		WriteToLog(_T("CMultisampleTest_Centroid11::SetupTestCase: SetupPipeline() failed."));
		return tRes;
	}

	const UINT16 *pSamplesCoveredByTriangleOffset = (m_TriangleOffsetAxis == TriangleOffsetAxis_X ?
		m_pSamplesCoveredByTriangleXOffset : m_pSamplesCoveredByTriangleYOffset);
	if (m_TriangleOffset >= 0)
	{
		m_PixelX = 1;
		m_PixelY = 1;

		// Must cover whole pixel with a single unclipped triangle, that can slide in +X & +Y
		m_TriangleVertex[ 0 ].pos = D3DXVECTOR2(1.f, 3.f);
		m_TriangleVertex[ 0 ].tex = D3DXVECTOR2(0.f, 2.f);
		m_TriangleVertex[ 1 ].pos = D3DXVECTOR2(3.f, 1.f);
		m_TriangleVertex[ 1 ].tex = D3DXVECTOR2(2.f, 0.f);
		m_TriangleVertex[ 2 ].pos = D3DXVECTOR2(1.f, 1.f);
		m_TriangleVertex[ 2 ].tex = D3DXVECTOR2(0.f, 0.f);

		m_SamplesCoveredByTriangleOffset =
			~pSamplesCoveredByTriangleOffset[ m_TriangleOffset ];
	}
	else
	{
		m_PixelX = 2;
		m_PixelY = 2;

		// Must cover whole pixel with a single unclipped triangle, that can slide in -X & -Y
		m_TriangleVertex[ 0 ].pos = D3DXVECTOR2(3.f, 1.f);
		m_TriangleVertex[ 0 ].tex = D3DXVECTOR2(1.f, -1.f);
		m_TriangleVertex[ 1 ].pos = D3DXVECTOR2(1.f, 3.f);
		m_TriangleVertex[ 1 ].tex = D3DXVECTOR2(-1.f, 1.f);
		m_TriangleVertex[ 2 ].pos = D3DXVECTOR2(3.f, 3.f);
		m_TriangleVertex[ 2 ].tex = D3DXVECTOR2(1.f, 1.f);

		if (m_uEffectiveSampleCount == 16 &&
			m_TriangleOffset == -16)
		{
			// Special case for 16 samples, to catch sample index 10 as min geometry covered sample.
			// 15 and 10:
			m_SamplesCoveredByTriangleOffset = 0x8400;
		}
		else
		{
			m_SamplesCoveredByTriangleOffset =
				pSamplesCoveredByTriangleOffset[ (m_TriangleOffset + m_uEffectiveSampleCount) % m_uEffectiveSampleCount ];
		}
	}

	return tRes;
}

TEST_RESULT CMultisampleTest_Centroid11::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;
	const bool bTIRIsEnabled = m_uForcedSampleCount > 0;

	try
	{
		// clear
		FLOAT ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );

		CD3D11_VIEWPORT vp = CD3D11_VIEWPORT( 0.f, 0.f, (float)m_RTWidth, (float)m_RTHeight );
		m_pDeviceContext->RSSetViewports( 1, &vp );
		m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, NULL );
		m_pDeviceContext->PSSetShader( m_pPSCentroid[ m_CentroidInterpolationMode ], NULL, 0 );
		UINT32 uSampleMask = D3D11_DEFAULT_SAMPLE_MASK;
		if (m_MinCoveredSampleFromMask >= 0)
		{
			uSampleMask = (0xFFFFFFFDu << m_MinCoveredSampleFromMask);
		}
		m_pDeviceContext->OMSetBlendState( m_pBS, 0, uSampleMask );
		WriteToLog( _T("Blend state sample mask set to %#X."), uSampleMask );

		// The centroid interpolation algorithm doesn't take into account the SampleMask Rasterizer State, 
		// since the SampleMask is relevant to the output sample pattern only, not the ForcedSampleCount pattern.
		// We need to check special cases to ensure that the sample mask doesn't mask off the writes
		if( bTIRIsEnabled &&
			(m_uSampleCount == 1 &&
			(uSampleMask & 0x1) != 0) )
		{
			uSampleMask = D3D11_DEFAULT_SAMPLE_MASK;
		}

		// setup geometry
		m_uVertexCount = 0;
		VERTEX *pDestVerts;
		D3D11_MAPPED_SUBRESOURCE mappedRes;

		//Fill vertex buffer
		if( FAILED( hr = GetEffectiveContext()->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
			throw TRException("Map on m_pVertexBuffer failed.", hr, RESULT_FAIL);
		pDestVerts = (VERTEX*) mappedRes.pData;

		float XOffset = (m_TriangleOffsetAxis == TriangleOffsetAxis_X ? m_TriangleOffset / (float)m_uEffectiveSampleCount : 0.f);
		float YOffset = (m_TriangleOffsetAxis == TriangleOffsetAxis_Y ? m_TriangleOffset / (float)m_uEffectiveSampleCount : 0.f);
		if (m_uEffectiveSampleCount == 16 &&
			m_TriangleOffset == -16)
		{
			// Special case for 16 samples, to catch sample index 10 as min geometry covered sample.
			// 15 and 10:
			XOffset = -21.f/ 32.f;
			YOffset = -25.f/ 32.f;
		}
		pDestVerts[m_uVertexCount].pos =
			D3DXVECTOR2(m_TriangleVertex[ 0 ].pos.x + XOffset, m_TriangleVertex[ 0 ].pos.y + YOffset);
		pDestVerts[m_uVertexCount++].tex =
			D3DXVECTOR2(m_TriangleVertex[ 0 ].tex.x + XOffset, m_TriangleVertex[ 0 ].tex.y + YOffset);
		pDestVerts[m_uVertexCount].pos =
			D3DXVECTOR2(m_TriangleVertex[ 1 ].pos.x + XOffset, m_TriangleVertex[ 1 ].pos.y + YOffset);
		pDestVerts[m_uVertexCount++].tex =
			D3DXVECTOR2(m_TriangleVertex[ 1 ].tex.x + XOffset, m_TriangleVertex[ 1 ].tex.y + YOffset);
		pDestVerts[m_uVertexCount].pos =
			D3DXVECTOR2(m_TriangleVertex[ 2 ].pos.x + XOffset, m_TriangleVertex[ 2 ].pos.y + YOffset);
		pDestVerts[m_uVertexCount++].tex =
			D3DXVECTOR2(m_TriangleVertex[ 2 ].tex.x + XOffset, m_TriangleVertex[ 2 ].tex.y + YOffset);

		GetEffectiveContext()->Unmap(m_pVertexBuffer,0);
		m_pDeviceContext->Draw(m_uVertexCount, 0);

#if 0
		ClearColor[0] = 0.1f;
		ClearColor[1] = 0.1f;
		m_pDeviceContext->ClearRenderTargetView( m_pSSRTView, ClearColor );
#endif
		
		float width = (float)m_RTWidth * m_uSampleCount;
		float height = (float)m_RTHeight;

		m_pDeviceContext->OMSetRenderTargets( 1, &m_pSSRTView, NULL );
		vp = CD3D11_VIEWPORT( 0.f, 0.f, width, height );
		m_pDeviceContext->RSSetViewports( 1, &vp );
		m_pDeviceContext->PSSetShader( m_pPSConvertMSToSS, NULL, 0 );
		m_pDeviceContext->PSSetShaderResources( 0, 1, &m_pSRView );
		m_pDeviceContext->OMSetBlendState( m_pBS, 0, D3D11_DEFAULT_SAMPLE_MASK );

		// render
		m_uVertexCount = 0;

		//Fill vertex buffer
		if( FAILED( hr = m_pDeviceContext->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
			throw TRException("Map on m_pVertexBuffer failed.", hr, RESULT_FAIL);
		pDestVerts = (VERTEX*) mappedRes.pData;

		pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(0, 0);
		pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(0, 0);
		pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(0, height);
		pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(0, 1);
		pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(width, 0);
		pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(1, 0);
		pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(width, height);
		pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(1, 1);

		m_pDeviceContext->Unmap(m_pVertexBuffer,0);
		m_pDeviceContext->Draw(m_uVertexCount, 0);
		ID3D11ShaderResourceView* pNULL = NULL;
		m_pDeviceContext->PSSetShaderResources( 0, 1, &pNULL ); // Mute warning debug spew.
		m_pDeviceContext->CopyResource(m_pSTexture2DSS, m_pRTTexture2DSS);

		if ( FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

		UINT CoveredSample = uSampleMask & m_SamplesCoveredByTriangleOffset;
		float fTriangleTolerance = 1.f;
		float fExpectedTriangleR, fExpectedTriangleG;
		const bool bCenterPatternIsUsed = !bTIRIsEnabled && m_uSampleQuality == D3D11_CENTER_MULTISAMPLE_PATTERN;	// TIR defines a standard sample pattern, regardless of the quality
		if (bCenterPatternIsUsed ||
			(((bTIRIsEnabled || (m_MinCoveredSampleFromMask == FullyCovered)) &&
			(m_SamplesCoveredByTriangleOffset & m_FullyCoveredSampleMask) == m_FullyCoveredSampleMask)))
		{
			fExpectedTriangleR = fExpectedTriangleG = 128.f;
		}
		else if (CoveredSample)
		{
			UINT8 MinCoveredSample = 0;
			while (!((1u << MinCoveredSample) & CoveredSample))
			{
				++MinCoveredSample;
			}

			if( bTIRIsEnabled &&
				MinCoveredSample > m_uEffectiveSampleCount )
			{
				fExpectedTriangleR = fExpectedTriangleG = 0.f;
			}
			else
			{
				const float* pOffsets = &m_pSamplePixelOffsets[ MinCoveredSample * 2 ];
				fExpectedTriangleR = pOffsets[ 0 ] * 256.0f;
				fExpectedTriangleG = pOffsets[ 1 ] * 256.0f;
			}
		}

		if( FAILED( hr = GetImmediateContext()->Map( m_pSTexture2DSS, 0, D3D11_MAP_READ, 0, &mappedRes ) ) )
			throw TRException("Map on m_pSTexture2DSS failed.", hr, RESULT_FAIL);

		float fTolerance, fExpectedR, fExpectedG;
		UINT8* pRow = static_cast< UINT8* >( mappedRes.pData ) + m_PixelY * mappedRes.RowPitch;
		UINT8* pPixel = pRow + m_PixelX * m_uSampleCount * 2; // 2 UINT8 components
		UINT8* pSample = pPixel;
		for (UINT s = 0; s < m_uSampleCount; ++s, pSample += 2)
		{
			if( m_uSampleCount == 1 &&
				m_uForcedSampleCount > 1 && // If m_uSampleCount == 1 and m_uForcedSampleCount == 1, no changes are needed. If m_uForcedSampleCount == 0 (TIR off), no changes are needed
				((1u << s) & uSampleMask) )	// If the current sample isn't in the sample mask, no changes are needed
			{
				// If TIR is on, and the output is only single sample, the output of that pixel would be the centroid interpolation of the attribute with the forced sample count's standard MSAA pattern.
				// EG. Output is single sample, forced sample count is 4x. The triangle X offset is 0. The triangle Y offset is .25. 
				// This means the first sample at (-2, -6) is NOT covered. Thus the first covered sample is (6, 2). - See D3D11 spec and D3D11_MSAAPatterns for 4x.
				// We need to update "s" to be the sample index that was evaluated

				// See if all samples are covered
				if( (m_SamplesCoveredByTriangleOffset & m_FullyCoveredSampleMask) == m_FullyCoveredSampleMask )
				{
					// Sample location should be the pixel center, even if the sample pattern does not happen to have a sample location there
				}
				else
				{
					// Not all sample locations are covered, find the first covered sample, in increasing order of sample index,
					// where sample coverage is after ANDing the coverage with the SampleMask Rasterizer State.
					// For TIR, the centroid interpolation algorithm doesn't take into account the SampleMask Rasterizer State, 
					// since the SampleMask is relevant to the output sample pattern only, not the ForcedSampleCount pattern. Thus we assume a full sample mask for this case
					UINT8 MinCoveredSample = 0;
					UINT TIRCoveredSample = D3D11_DEFAULT_SAMPLE_MASK & m_SamplesCoveredByTriangleOffset;
					while (!((1u << MinCoveredSample) & TIRCoveredSample))
					{
						++MinCoveredSample;
					}

					// Force s to be the lowest covered sample
					WriteToLog( _T("Not all sample locations are covered.") );
					WriteToLog( _T("TIR is on (Forced sample count = %d), and the output is single sample. The output should be based on sample index %d because it's the min covered sample."), m_uForcedSampleCount, MinCoveredSample );
					s = MinCoveredSample;
				}
			}

			if ((1u << s) & CoveredSample)
			{
				fExpectedR = fExpectedTriangleR;
				fExpectedG = fExpectedTriangleG;
				fTolerance = fTriangleTolerance;
			}
			else
			{
				// From Clear operation
				fExpectedR = fExpectedG = 0.f;
				fTolerance = 0.f;
			}
			
			float fR = pSample[ 0 ];
			float fG = pSample[ 1 ];
			
			if (!(fExpectedR - fTolerance <= fR && fR <= fExpectedR + fTolerance &&
				fExpectedG - fTolerance <= fG && fG <= fExpectedG + fTolerance))
			{
				WriteToLog( "Pixel (%u, %u), Sample %u (Values = R: %f, G: %f) "
					"outside of range R: [ %f, %f ], G: [ %f, %f ].",
					int( m_PixelX ),
					int( m_PixelY ),
					int( s ),
					fR,
					fG,
					fExpectedR - fTolerance,
					fExpectedR + fTolerance,
					fExpectedG - fTolerance,
					fExpectedG + fTolerance );
				tRes = RESULT_FAIL;
			}
		}

		GetImmediateContext()->Unmap( m_pSTexture2DSS, 0 );

		return tRes;
	}
	catch (TRException& exc)
	{
		exc.Prepend("[CMultisampleTest_Centroid11::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
		return exc.GetResult();
	}

	#if 0
	try
	{
		Present();
	}
	catch (TRException& exc)
	{
		exc.Prepend("[CMultisampleTest_Centroid11::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}	
	#endif
}

void CMultisampleTest_Centroid11::CleanupTestCase() 
{
	if (m_pDevice)
	{
		m_pDeviceContext->ClearState();
	}

	for (CentroidInterpolationMode m = CentroidInterpolationMode( 0 ); m < CentroidInterpolationMode_Max; ++m)
	{
		SAFE_RELEASE( m_pPSCentroid[ m ] );
	}
	SAFE_RELEASE( m_pSRView );
	SAFE_RELEASE( m_pRTTexture2DSS );
	SAFE_RELEASE( m_pSSRTView );
	SAFE_RELEASE( m_pSTexture2DSS );
	SAFE_RELEASE( m_pPSConvertMSToSS );

	CMultisampleTest::CleanupD3D();
}

//////////////////////////////////////////////////////////////////////////
//
// CMultisampleTest_Centroid11Uncovered
//
//////////////////////////////////////////////////////////////////////////

void CMultisampleTest_Centroid11Uncovered::InitTestParameters()
{
	InitMultisampleParameters();

	m_PixPosID = PP_TOP_LEFT;
	m_ReadBack = MS_LOAD;

	// Forced sample count (part of the rasterizer desc)
	// With a forced sample count/pattern selected at the rasterizer (ForcedSampleCount > 0), 
	// pixels are candidates for shader invocation based on the selected sample pattern, independent of the RTV ("output") sample count
	m_pForcedSampleCountParam = AddParameter( _T( "Forced_Sample_Count" ), &m_uForcedSampleCount );
	testfactor::RFactor rfForcedSampleCount0 = AddParameterValue<UINT>( m_pForcedSampleCountParam, 0);	// 0 means the feature is off and rendering is 'normal'
	testfactor::RFactor rfForcedSampleCount1 = AddParameterValue<UINT>( m_pForcedSampleCountParam, 1);
	// Note we dont cover ForcedSampleCounts > 1 because this test uses derivatives. Incorrect derivatives will often result when using centroid unless sample frequency is used. Sample frequency is undefined for TIR.
	testfactor::RFactor rfForcedSampleCounts = rfForcedSampleCount0 + rfForcedSampleCount1;

	// Sample counts and quality levels
	// only sample counts 1(trival), 2, 4, 8 and 16 support standard and center sample patterns
	// Centroid definition only applies to standard and center sample patterns
	const numSampleCounts = 5;
	CTestCaseParameter< INT > *pMinSIndex = AddParameter( _T( "MinCoveredSampleFromMask" ), &m_MinCoveredSampleFromMask );
	pMinSIndex->SetDefaultValue( FullyCovered );
	testfactor::RFactor rfSampleCount[numSampleCounts];
	testfactor::RFactor rfSindex[numSampleCounts];
	testfactor::RFactor rfSampleDescIndex;

	rfSampleCount[0] = AddParameterValue<UINT>( m_pSampleCountParam, (UINT)pow((float)2, (float)0) );
		rfSindex[0] = AddParameterValueSet<INT>( pMinSIndex, 
			new CRangeValueSet< INT >(FullyCovered, (UINT)pow((float)2, (float)0) - 1, 1) );
	rfSampleDescIndex = rfSampleCount[0] * rfSindex[0] * rfForcedSampleCounts;
	for( size_t i = 1; i < numSampleCounts; i++ )
	{
		rfSampleCount[i] = AddParameterValue<UINT>( m_pSampleCountParam, (UINT)pow((float)2, (float)i) );
		rfSindex[i] = AddParameterValueSet<INT>( pMinSIndex, 
			new CRangeValueSet< INT >(FullyCovered, (UINT)pow((float)2, (float)i) - 1, 1) );

		rfSampleDescIndex = 
			rfSampleDescIndex + rfSampleCount[i] * rfSindex[i] * rfForcedSampleCount0 +
			rfSampleDescIndex + rfSampleCount[i] * rfSindex[0] * rfForcedSampleCount1;
	}

	testfactor::RFactor rfQualityLevelStandard = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_STANDARD );
	testfactor::RFactor rfQualityLevelCenter = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_CENTER );

	rfSampleDescIndex = rfSampleDescIndex * ( rfQualityLevelStandard + rfQualityLevelCenter);

	CTestCaseParameter<CentroidInterpolationMode>* pCentroidInterpolationModeParam =
		AddParameter<CentroidInterpolationMode>( _T("InterpolationMode"), &m_CentroidInterpolationMode );
	pCentroidInterpolationModeParam->SetDefaultValue( CentroidInterpolationMode_Linear);
	testfactor::RFactor rInterp_Linear =
		AddParameterValue<CentroidInterpolationMode>( pCentroidInterpolationModeParam, CentroidInterpolationMode_Linear );
	testfactor::RFactor rInterp_Linear_NoPerspective =
		AddParameterValue<CentroidInterpolationMode>( pCentroidInterpolationModeParam, CentroidInterpolationMode_Linear_NoPerspective );

	testfactor::RFactor rfRTFormat = AddParameterValue<DXGI_FORMAT>( m_pRTFormatParam, DXGI_FORMAT_R8G8_UNORM);
	SetRootTestFactor( rfSampleDescIndex * rfRTFormat *
		(rInterp_Linear + rInterp_Linear_NoPerspective) );
	
	// Sample-frequency Pixel Shader execution is undefined when sample pattern is CENTER and 
	// more than one sample overlapping at the center of the pixel. 
	CFilterHandle skipMatchingFilter = (
		FilterGreater<UINT>( _T("Sample_Count"), 1 ) 
		&& FilterEqual<UINT> ( _T("Forced_Sample_Count"), 0 ) 
		&& FilterEqual<QUALITY_LEVEL_SCENARIO> ( _T("Quality_Level"), QUALITY_LEVEL_CENTER ) );
	SkipMatching( skipMatchingFilter );
}

TEST_RESULT CMultisampleTest_Centroid11Uncovered::SetupPipeline()
{
	try
	{
		const bool bTIRIsEnabled = m_uForcedSampleCount > 0;
		TEST_RESULT tRes = SetupD3D();
		if (tRes != RESULT_PASS)
			throw TRException( "CMultisampleTest::SetupD3D failed.", tRes );

		m_FullyCoveredSampleMask = (0x1 << m_uEffectiveSampleCount) - 1;
		switch (m_uEffectiveSampleCount)
		{
		case (1):
			m_pSamplePixelOffsets = STDMS1Offsets;
			break;
			
		case (2):
			m_pSamplePixelOffsets = STDMS2Offsets;
			break;
			
		case (4):
			m_pSamplePixelOffsets = STDMS4Offsets;
			break;
			
		case (8):
			m_pSamplePixelOffsets = STDMS8Offsets;
			break;
			
		case (16):
			m_pSamplePixelOffsets = STDMS16Offsets;
			break;

		default: assert( false );
		}

		// Render Test:
		m_pDeviceContext->RSSetState( m_pRSEnabled );
		m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		m_pDeviceContext->OMSetBlendState( m_pBS, 0, D3D11_DEFAULT_SAMPLE_MASK );
		m_pDeviceContext->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		m_pDeviceContext->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		m_pDeviceContext->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		m_pDeviceContext->VSSetShader( m_pVS, NULL, 0 );
		m_pDeviceContext->GSSetShader( NULL, NULL, 0 );
		m_pDeviceContext->PSSetShader( m_pPS, NULL, 0 );
		m_pDeviceContext->IASetInputLayout( m_pVertexLayout );

		// Disable depth if needed
		if( bTIRIsEnabled )
		{
			// Depth/Stencil Views must not be bound, depth testing must be disabled, and the shader must not output depth while rendering with ForcedSampleCount 1 or greater, otherwise rendering behavior is undefined
			m_pDeviceContext->OMSetDepthStencilState( m_pDepthStateDisabled, 0 );
		}
		else
		{
			m_pDeviceContext->OMSetDepthStencilState( m_pDepthStateDefault, 0 );
		}

		UINT strides[] = { sizeof( VERTEX ) };
		UINT offsets[] = { 0 };
		m_pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, strides, offsets );

		CD3D11_VIEWPORT vp = CD3D11_VIEWPORT( 0.f, 0.f, (float)m_RTWidth, (float)m_RTHeight );
		m_pDeviceContext->RSSetViewports( 1, &vp );
		m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, NULL );

		float *bufData;
		D3D11_MAPPED_SUBRESOURCE mappedRes;
		if( FAILED( m_pDeviceContext->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
			throw TRException("Map on m_pConstBuffer Failed");
		bufData = (float*)mappedRes.pData;
		// Prescaled inverse viewport translation.
		bufData[0] = -1.f;
		bufData[1] = 1.f;
		// Inverse viewport scale.
		bufData[2] = 2.f / (float)m_RTWidth;
		bufData[3] = -2.f / (float)m_RTHeight;
		// float alpha
		bufData[4] = 0.f;
		// float depth
		bufData[5] = 0.f;
		m_pDeviceContext->Unmap(m_pConstBuffer,0);

		CD3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc =
			CD3D11_SHADER_RESOURCE_VIEW_DESC( m_pRTTexture2D, D3D11_SRV_DIMENSION_TEXTURE2DMS );
		if ( FAILED(m_pDevice->CreateShaderResourceView( m_pRTTexture2D, (m_uSampleCount == 1 ? &SRVDesc : NULL), &m_pSRView )) )
			throw TRException("CreateShaderResourceView failed for m_pSRView.");

		const UINT MAX_STR = 4000;
		char szShader[ MAX_STR ];

		for (CentroidInterpolationMode m = CentroidInterpolationMode( 0 ); m < CentroidInterpolationMode_Max; ++m)
		{
			// Centroid Attribute Evaluation PS
			// Forces super-sampling with usage of SV_SampleIndex (per-sample invocation); but
			// alpha should be 0, no-op'ing the affect on the expression.
			// Stuffs centroid-evaluted tex into color.
			char szPS[] = 
				"struct PSIn																\n"
				"{																			\n"
				"	float4 pos : SV_Position;												\n"
				"	centroid %sfloat2 tex : tex;											\n"
				"	%s																		\n"
				"};																			\n"
				"struct PSOut																\n"
				"{																			\n"
				"	float4 Color : SV_Target;												\n"
				"};																			\n"
				"cbuffer cb0 : register(b0)													\n"
				"{																			\n"
				"	float4 inverseViewport;													\n"
				"	float alpha;															\n"
				"};																			\n"
				"PSOut main(PSIn input)														\n"
				"{																			\n"
				"	PSOut output;															\n"
				"	output.Color = float4(0,0,0,1);											\n"
				"	%s																		\n"
				"	output.Color.xy = output.Color.zw;										\n"
				"	return output;															\n"
				"}";	

			ID3D10Blob *pPSBuf = NULL;
			ID3D10Blob *pErrorBuf = NULL;
			// construct PS, requires output type info
			_snprintf( szShader, MAX_STR-1, szPS,
				(m == CentroidInterpolationMode_Linear_NoPerspective ? "noperspective " : ""),
				(!bTIRIsEnabled ? "uint si : SV_SampleIndex;" : "" ),	// If TIR is enabled, Sample-frequency shader invocation cannot be requested, otherwise rendering results are undefined
				(!bTIRIsEnabled ? "output.Color.zw = input.tex.xy + alpha * input.si;" : "output.Color.zw = input.tex.xy;" ) );
			szShader[MAX_STR-1] = 0;
			if ( FAILED(D3DX11CompileFromMemory( szShader, strlen(szShader), NULL, NULL, NULL, "main",
				GetFramework()->GetShaderProfile( D3D_SHADER_STAGE_PIXEL, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0),
				D3D10_SHADER_DEBUG, 0, NULL, &pPSBuf, &pErrorBuf, NULL )) )
				throw TRException( tstring("Failed to compile PS.") + (pErrorBuf ? (char*)pErrorBuf->GetBufferPointer() : "") );
			SAFE_RELEASE( pErrorBuf );
			if ( FAILED(m_pDevice->CreatePixelShader( pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &m_pPSCentroid[ m ] )) )
			{
				SAFE_RELEASE( pPSBuf );
				throw TRException("Failed to create PS.");
			}
			SAFE_RELEASE( pPSBuf );

			// Centroid Attribute Evaluation PS
			// Forces super-sampling with usage of SV_SampleIndex (per-sample invocation); but
			// alpha should be 0, no-op'ing the affect on the expression.
			// Stuffs centroid-evaluted tex into color.
			char szPSUncovered[] = 
				"struct PSIn																\n"
				"{																			\n"
				"	float4 pos : SV_Position;												\n"
				"	centroid %sfloat2 tex : tex;											\n"
				"	%s																		\n"
				"};																			\n"
				"struct PSOut																\n"
				"{																			\n"
				"	float4 Color : SV_Target;												\n"
				"};																			\n"
				"cbuffer cb0 : register(b0)													\n"
				"{																			\n"
				"	float4 inverseViewport;													\n"
				"	float alpha;															\n"
				"};																			\n"
				"PSOut main(PSIn input)														\n"
				"{																			\n"
				"	PSOut output;															\n"
				"	output.Color = float4(0,0,0,1);											\n"
				"	%s																		\n"
				"	output.Color.x = ddx_fine( input.tex.x );								\n"
				"	output.Color.y = ddy_fine( input.tex.y );								\n"
				"	return output;															\n"
				"}";	

			// construct PS, requires output type info
			_snprintf( szShader, MAX_STR-1, szPSUncovered,
				(m == CentroidInterpolationMode_Linear_NoPerspective ? "noperspective " : ""),
				(!bTIRIsEnabled ? "uint si : SV_SampleIndex;" : "" ),	// If TIR is enabled, Sample-frequency shader invocation cannot be requested, otherwise rendering results are undefined
				(!bTIRIsEnabled ? "output.Color.zw = input.tex.xy + alpha * input.si;" : "output.Color.zw = input.tex.xy;" ) );
			szShader[MAX_STR-1] = 0;
			if ( FAILED(D3DX11CompileFromMemory( szShader, strlen(szShader), NULL, NULL, NULL, "main", 
				GetFramework()->GetShaderProfile( D3D_SHADER_STAGE_PIXEL, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_0),
				D3D10_SHADER_DEBUG, 0, NULL, &pPSBuf, &pErrorBuf, NULL )) )
				throw TRException( tstring("Failed to compile PS.") + (pErrorBuf ? (char*)pErrorBuf->GetBufferPointer() : "") );
			SAFE_RELEASE( pErrorBuf );
			if ( FAILED(m_pDevice->CreatePixelShader( pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &m_pPSCentroidUncovered[ m ] )) )
			{
				SAFE_RELEASE( pPSBuf );
				throw TRException("Failed to create PS.");
			}
			SAFE_RELEASE( pPSBuf );
		}

		// Sample Readback with Load:
		// New Render Target & Staging pair
		D3D11_TEXTURE2D_DESC rtdesc;
		m_pRTTexture2D->GetDesc( &rtdesc );

		rtdesc.Width *= rtdesc.SampleDesc.Count;
		rtdesc.SampleDesc.Count = 1;
		rtdesc.SampleDesc.Quality = 0;
		if ( FAILED(GetFramework()->CreateTexture2DTiledOptional( &rtdesc, NULL, &m_pRTTexture2DSS )) )
			throw TRException("CreateTexture2D failed for m_pRTTexture2DSS.");

		if ( FAILED(m_pDevice->CreateRenderTargetView( m_pRTTexture2DSS, NULL, &m_pSSRTView )) )
			throw TRException("CreateRenderTargetView failed for m_pSSRTView.");

		rtdesc.Usage = D3D11_USAGE_STAGING;
		rtdesc.BindFlags = 0;
		rtdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		for (int i = 0; i < ARRAYSIZE( m_apSTexture2DSS ); ++i)
		{
			if ( FAILED(GetFramework()->CreateTexture2DTiledOptional( &rtdesc, NULL, &m_apSTexture2DSS[ i ] )) )
				throw TRException("CreateTexture2D failed for m_apSTexture2DSS.");
		}

		// New PS
		char szMSPS[] =
			"Texture2DMS< float2, %d > MyTex;											\n"
			"struct PSIn																\n"
			"{																			\n"
			"	float4 pos : SV_Position;												\n"
			"	float2 tex : tex;														\n"
			"};																			\n"
			"struct PSOut																\n"
			"{																			\n"
			"	float4 Color : SV_Target;												\n"
			"};																			\n"
			"cbuffer cb0 : register(b0)													\n"
			"{																			\n"
			"	float4 inverseViewport;													\n"
			"	float alpha;															\n"
			"};																			\n"
			"PSOut main(PSIn input)														\n"
			"{																			\n"
			"	uint iX = input.pos.x;													\n"
			"	uint iS = %d;															\n"
			"	PSOut output;															\n"
			"	output.Color = float4(0,0,0,1);											\n"
			"	output.Color.xy = MyTex.Load( int3(iX/iS,input.pos.y,0), iX%%iS ).xy;	\n"
			"	return output;															\n"
			"}";

		// construct PS, requires output type info
		ID3D10Blob *pPSBuf = NULL;
		ID3D10Blob *pErrorBuf = NULL;
		_snprintf( szShader, MAX_STR-1, szMSPS, m_uSampleCount, m_uSampleCount );
		szShader[MAX_STR-1] = 0;
		if ( FAILED(D3DX11CompileFromMemory( szShader, strlen(szShader), NULL, NULL, NULL, "main", "ps_4_1", D3D10_SHADER_DEBUG, 0, NULL, &pPSBuf, &pErrorBuf, NULL )) )
			throw TRException( tstring("Failed to compile PS.") + (pErrorBuf ? (char*)pErrorBuf->GetBufferPointer() : "") );
		SAFE_RELEASE( pErrorBuf );
		if ( FAILED(m_pDevice->CreatePixelShader( pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &m_pPSConvertMSToSS )) )
		{
			SAFE_RELEASE( pPSBuf );
			throw TRException("Failed to create PS.");
		}
		SAFE_RELEASE( pPSBuf );

		return tRes;
	}
	catch (TRException& exc)
	{
		exc.Prepend("[CMultisampleTest_Centroid11Uncovered::Setup()] ");
		if (exc.GetResult() == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
		return exc.GetResult();
	}
}

TEST_RESULT CMultisampleTest_Centroid11Uncovered::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	tRes = SetFormatAndQualityLevel( false );
	if ( tRes != RESULT_PASS )
	{
		return tRes;
	}

	// Determine the effective sample count (if TIR is ON or OFF)
	m_uEffectiveSampleCount = ( m_uForcedSampleCount > 0 ) ? m_uForcedSampleCount : m_uSampleCount;

	tRes = SetupPipeline();
	if (tRes != RESULT_PASS)
	{
		WriteToLog(_T("CMultisampleTest_Centroid11::SetupTestCase: SetupPipeline() failed."));
		return tRes;
	}

	return tRes;
}

TEST_RESULT CMultisampleTest_Centroid11Uncovered::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;
	const bool bTIRIsEnabled = m_uForcedSampleCount > 0;

	try
	{
		UINT32 uSampleMask = D3D11_DEFAULT_SAMPLE_MASK;
		if (m_MinCoveredSampleFromMask >= 0)
		{
			uSampleMask = (0xFFFFFFFDu << m_MinCoveredSampleFromMask);
		}
		WriteToLog( _T("Blend state sample mask will be set to %#X."), uSampleMask );

		for (int i = 0; i < ARRAYSIZE( m_apSTexture2DSS ); ++i)
		{
			ID3D11PixelShader* pPSCentroid = (i ? m_pPSCentroidUncovered[ m_CentroidInterpolationMode ] :
				m_pPSCentroid[ m_CentroidInterpolationMode ]);
				
			// clear
			FLOAT ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );

			CD3D11_VIEWPORT vp = CD3D11_VIEWPORT( 0.f, 0.f, (float)m_RTWidth, (float)m_RTHeight );
			m_pDeviceContext->RSSetViewports( 1, &vp );
			m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, NULL );
			m_pDeviceContext->PSSetShader( pPSCentroid, NULL, 0 );
			m_pDeviceContext->OMSetBlendState( m_pBS, 0, uSampleMask );
			
			// setup geometry
			m_uVertexCount = 0;
			VERTEX *pDestVerts;
			D3D11_MAPPED_SUBRESOURCE mappedRes;

			//Fill vertex buffer
			if( FAILED( hr = GetEffectiveContext()->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
				throw TRException("Map on m_pVertexBuffer failed.", hr, RESULT_FAIL);
			pDestVerts = (VERTEX*) mappedRes.pData;

			pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(1, 1);
			pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(.5f, .5f);
			pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(1, 2);
			pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(.5f, 1.f);
			pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(2, 1);
			pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(1.f, .5f);
			pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(2, 2);
			pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(1.f, 1.f);

			GetEffectiveContext()->Unmap(m_pVertexBuffer,0);
			m_pDeviceContext->Draw(m_uVertexCount, 0);

			float width = (float)m_RTWidth * m_uSampleCount;
			float height = (float)m_RTHeight;

			m_pDeviceContext->OMSetRenderTargets( 1, &m_pSSRTView, NULL );
			vp = CD3D11_VIEWPORT( 0.f, 0.f, width, height );
			m_pDeviceContext->RSSetViewports( 1, &vp );
			m_pDeviceContext->PSSetShader( m_pPSConvertMSToSS, NULL, 0 );
			m_pDeviceContext->PSSetShaderResources( 0, 1, &m_pSRView );
			m_pDeviceContext->OMSetBlendState( m_pBS, 0, D3D11_DEFAULT_SAMPLE_MASK );

			// render
			m_uVertexCount = 0;

			//Fill vertex buffer
			if( FAILED( hr = m_pDeviceContext->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
				throw TRException("Map on m_pVertexBuffer failed.", hr, RESULT_FAIL);
			pDestVerts = (VERTEX*) mappedRes.pData;

			pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(0, 0);
			pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(0, 0);
			pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(0, height);
			pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(0, 1);
			pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(width, 0);
			pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(1, 0);
			pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(width, height);
			pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(1, 1);

			m_pDeviceContext->Unmap(m_pVertexBuffer,0);
			m_pDeviceContext->Draw(m_uVertexCount, 0);
			ID3D11ShaderResourceView* pNULL = NULL;
			m_pDeviceContext->PSSetShaderResources( 0, 1, &pNULL ); // Mute warning debug spew.
			m_pDeviceContext->CopyResource(m_apSTexture2DSS[ i ], m_pRTTexture2DSS);
		}

		if ( FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

		const float PixelCenterOffsets[ 2 ] = { .5f, .5f };
		const float* pUncoveredEvaluationOffsets;
		const bool bCenterPatternIsUsed = !bTIRIsEnabled && m_uSampleQuality == D3D11_CENTER_MULTISAMPLE_PATTERN;	// TIR defines a standard sample pattern, regardless of the quality
		if (bCenterPatternIsUsed ||
			m_MinCoveredSampleFromMask == FullyCovered)
		{
			pUncoveredEvaluationOffsets = PixelCenterOffsets;
		}
		else
		{
			pUncoveredEvaluationOffsets = &m_pSamplePixelOffsets[ m_MinCoveredSampleFromMask * 2 ];
		}
		const float UncoveredEvaluatedTexCoordDXSrc_X[ 2 ] =
		{ pUncoveredEvaluationOffsets[ 0 ] / 2.f, 1.f + pUncoveredEvaluationOffsets[ 0 ] / 2.f };
		const float UncoveredEvaluatedTexCoordDXSrc_Y = pUncoveredEvaluationOffsets[ 1 ] / 2.f + .5f;
		const float UncoveredEvaluatedTexCoordDYSrc_X = pUncoveredEvaluationOffsets[ 0 ] / 2.f + .5f;
		const float UncoveredEvaluatedTexCoordDYSrc_Y[ 2 ] =
		{ pUncoveredEvaluationOffsets[ 1 ] / 2.f, 1.f + pUncoveredEvaluationOffsets[ 1 ] / 2.f };

		D3D11_MAPPED_SUBRESOURCE mappedRes1, mappedRes2;
		if( FAILED( hr = GetImmediateContext()->Map( m_apSTexture2DSS[ 0 ], 0, D3D11_MAP_READ, 0, &mappedRes1 ) ) )
			throw TRException("Map on m_pSTexture2DSS[ 0 ] failed.", hr, RESULT_FAIL);
		if( FAILED( hr = GetImmediateContext()->Map( m_apSTexture2DSS[ 1 ], 0, D3D11_MAP_READ, 0, &mappedRes2 ) ) )
			throw TRException("Map on m_pSTexture2DSS[ 1 ] failed.", hr, RESULT_FAIL);

			float fTolerance;
		UINT8* pRow1 = static_cast< UINT8* >( mappedRes1.pData ) + 1 * mappedRes1.RowPitch;
		UINT8* pPixel1 = pRow1 + 1 * m_uSampleCount * 2; // 2 UINT8 components
		UINT8* pSample1 = pPixel1;
		UINT8* pRow2 = static_cast< UINT8* >( mappedRes2.pData ) + 1 * mappedRes2.RowPitch;
		UINT8* pPixel2 = pRow2 + 1 * m_uSampleCount * 2; // 2 UINT8 components
		UINT8* pSample2 = pPixel2;
		for (UINT s = 0; s < m_uSampleCount; ++s, pSample1 += 2, pSample2 += 2)
		{
			if ((1u << s) & uSampleMask)
			{
				const float fEvaulatedSampleTexCoord[ 2 ] =
				{ pSample1[ 0 ] / 256.f, pSample1[ 1 ] / 256.f };
				const float fPossibleR[ 2 ] =
				{
					(fEvaulatedSampleTexCoord[ 0 ] - UncoveredEvaluatedTexCoordDXSrc_X[ 0 ]) * 256.f,
					(UncoveredEvaluatedTexCoordDXSrc_X[ 1 ] - fEvaulatedSampleTexCoord[ 0 ]) * 256.f,
				};
				const float fPossibleG[ 2 ] =
				{
					(fEvaulatedSampleTexCoord[ 1 ] - UncoveredEvaluatedTexCoordDYSrc_Y[ 0 ]) * 256.f,
					(UncoveredEvaluatedTexCoordDYSrc_Y[ 1 ] - fEvaulatedSampleTexCoord[ 1 ]) * 256.f,
				};
				fTolerance = 2.f;

				float fR = pSample2[ 0 ], fG = pSample2[ 1 ];
				if (!((fPossibleR[ 0 ] - fTolerance <= fR && fR <= fPossibleR[ 0 ] + fTolerance) ||
					(fPossibleR[ 1 ] - fTolerance <= fR && fR <= fPossibleR[ 1 ] + fTolerance)) ||
					!((fPossibleG[ 0 ] - fTolerance <= fG && fG <= fPossibleG[ 0 ] + fTolerance) ||
					(fPossibleG[ 1 ] - fTolerance <= fG && fG <= fPossibleG[ 1 ] + fTolerance)))
				{
					WriteToLog( "Pixel (%u, %u), Sample %u (Values = R: %f, G: %f) "
						"outside of range R: [ %f, %f ] or [ %f, %f ], "
						"G: [ %f, %f ] or [ %f, %f ].",
						int( 1 ),
						int( 1 ),
						int( s ),
						fR,
						fG,
						fPossibleR[ 0 ] - fTolerance,
						fPossibleR[ 0 ] + fTolerance,
						fPossibleR[ 1 ] - fTolerance,
						fPossibleR[ 1 ] + fTolerance,
						fPossibleG[ 0 ] - fTolerance,
						fPossibleG[ 0 ] + fTolerance,
						fPossibleG[ 1 ] - fTolerance,
						fPossibleG[ 1 ] + fTolerance );
					tRes = RESULT_FAIL;
				}
			}
			else
			{
				// From Clear operation
				float fExpectedR = 0.f, fExpectedG = 0.f;
				fTolerance = 0.f;

				float fR = pSample2[ 0 ], fG = pSample2[ 1 ];
				if (!(fExpectedR - fTolerance <= fR && fR <= fExpectedR + fTolerance &&
					fExpectedG - fTolerance <= fG && fG <= fExpectedG + fTolerance))
				{
					WriteToLog( "Pixel (%u, %u), Sample %u (Values = R: %f, G: %f) "
						"outside of range R: [ %f, %f ], G: [ %f, %f ].",
						int( 1 ),
						int( 1 ),
						int( s ),
						fR,
						fG,
						fExpectedR - fTolerance,
						fExpectedR + fTolerance,
						fExpectedG - fTolerance,
						fExpectedG + fTolerance );
					tRes = RESULT_FAIL;
				}
			}
		}

		GetImmediateContext()->Unmap( m_apSTexture2DSS[ 0 ], 0 );
		GetImmediateContext()->Unmap( m_apSTexture2DSS[ 1 ], 0 );

		return tRes;
	}
	catch (TRException& exc)
	{
		exc.Prepend("[CMultisampleTest_Centroid11Uncovered::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
		return exc.GetResult();
	}

	#if 0
	try
	{
		Present();
	}
	catch (TRException& exc)
	{
		exc.Prepend("[CMultisampleTest_Centroid11::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}	
	#endif
}

void CMultisampleTest_Centroid11Uncovered::CleanupTestCase() 
{
	if (m_pDevice)
	{
		m_pDeviceContext->ClearState();
	}

	for (CentroidInterpolationMode m = CentroidInterpolationMode( 0 ); m < CentroidInterpolationMode_Max; ++m)
	{
		SAFE_RELEASE( m_pPSCentroid[ m ] );
		SAFE_RELEASE( m_pPSCentroidUncovered[ m ] );
	}
	SAFE_RELEASE( m_pSRView );
	SAFE_RELEASE( m_pRTTexture2DSS );
	SAFE_RELEASE( m_pSSRTView );
	for (int i = 0; i < ARRAYSIZE( m_apSTexture2DSS ); ++i)
	{
		SAFE_RELEASE( m_apSTexture2DSS[ i ] );
	}
	SAFE_RELEASE( m_pPSConvertMSToSS );

	CMultisampleTest::CleanupD3D();
}


