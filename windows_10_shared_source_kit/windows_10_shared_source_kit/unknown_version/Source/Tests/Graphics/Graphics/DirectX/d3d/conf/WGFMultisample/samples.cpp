//////////////////////////////////////////////////////////////////////////
//  Samples.cpp
//  created:	2005/07/01
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "samples.h"
#include "D3DMSHelper.h"
#include "FormatDesc.hpp"

// 4 bit grid
POINT SamplePositions1[1] = { { 8, 8 } };
POINT SamplePositions2[2] = { { 12, 12 },	
							  { 4, 4 } };
POINT SamplePositions4[4] = { { 6, 2 },	
							  { 14, 6 },	
							  { 2, 10 },	
							  { 10, 14 } };
POINT SamplePositions8[8] = { { 9, 5 },	
							  { 7, 11 },	
							  { 13, 9 },	
							  { 5, 3 },	
							  { 3, 13 },	
							  { 1, 7 },	
							  { 11, 15 },	
							  { 15, 1 } };
POINT SamplePositions16[16] = { { 9, 9 },	
							    { 7, 5 },	
							    { 5, 10 },	
							    { 12, 7 },	
							    { 3, 6 },	
							    { 10, 13 },	
								{ 13, 11 },
							    { 11, 3 },	
							    { 6, 14 },	
							    { 8, 1 },	
							    { 4, 2 },	
							    { 2, 12 },	
							    { 0, 8 },	
							    { 15, 4 },	
							    { 14, 15 },	
							    { 1, 0 } };
POINT SamplePositions32[32] = { { 8, 7 },	
							    { 10, 9 },	
							    { 7, 10 },	
							    { 5, 8 },	
							    { 11, 6 },	
							    { 6, 5 },	
								{ 9, 12 },
							    { 9, 4 },	
							    { 13, 8 },	
							    { 12, 11 },	
							    { 4, 11 },	
							    { 6, 13 },	
							    { 3, 6 },	
							    { 2, 9 },	
							    { 7, 2 },	
							    { 12, 3 },	
							    { 4, 3 },	
							    { 11, 14 },	
							    { 14, 5 },	
							    { 8, 15 },	
							    { 15, 10 },	
							    { 10, 1 },	
							    { 14, 13 },	
							    { 3, 14 },	
							    { 1, 12 },	
							    { 1, 4 },	
							    { 0, 7 },	
							    { 5, 0 },	
							    { 15, 2 },	
							    { 2, 1 },	
							    { 13, 0 },	
								{ 0, 15 } };

POINT *Palette[] = {&SamplePositions1[0], 
					&SamplePositions2[0],
					&SamplePositions4[0],
					&SamplePositions8[0],
					&SamplePositions16[0],
					&SamplePositions32[0]};

void CCountTest::InitTestParameters()
{
	InitMultisampleParameters( );
	SetRootTestFactor ( m_rfSampleDesc * ( ( m_rfRTFormatTypeless + m_rfRTFormatTypedAll ) % m_rfPixPos )  );
}

TEST_RESULT CCountTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	tRes = SetFormatAndQualityLevel( false );
	if ( tRes != RESULT_PASS)
	{
		return tRes;
	}

	try 
	{
		SetupD3D();
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Samples::SetupTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}
	
	return tRes;
}

void CCountTest::CleanupTestCase()
{
	CMultisampleTest::CleanupD3D();
}

TEST_RESULT CCountTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	try 
	{
		if( RESULT_FAIL == m_Finder.FindSamples() )
			throw TRException("FindSamples failed.");	

		// verify sample positions
		if( m_uSampleQuality == D3D11_STANDARD_MULTISAMPLE_PATTERN )
		{
			if( RESULT_FAIL == m_Finder.OrderSamples() )
				throw TRException("OrderSamples failed.");	

			// convert 
			for (UINT uSample = 0; uSample < m_uSampleCount; ++uSample)
			{
				UINT uCount = m_uSampleCount;
				UINT uIndex = 0;
				while (uCount = uCount >> 1) ++uIndex;

				//sanity check
				if (((1 << uIndex) != m_uSampleCount) ||
					(m_uSampleCount > 32))
				{
					WriteToLog("SampleCount = %d", m_uSampleCount);
					throw TRException("Sample count is invalid.");
				}

				POINT pActual = m_Finder.GetLoc(uSample);
				POINT pExpected = Palette[uIndex][uSample];

				// convert palette entry from 4 bit to 16 bit
				pExpected.x *= 16;
				pExpected.y *= 16;

				if ((pActual.x != pExpected.x) || (pActual.y != pExpected.y))
				{
					WriteToLog("Sample = %d", uSample);
					WriteToLog("pExpected = [%d, %d]", pExpected.x, pExpected.y);
					WriteToLog("PActual = [%d, %d]", pActual.x, pActual.y);
					tRes = RESULT_FAIL;					
				}
			}
			if (tRes == RESULT_FAIL)
			{
				WriteToLog("(Reporting sample locations on 8 bit grid, origin top left corner of pixel)");
				WriteToLog("SampleCount = %d", m_uSampleCount);
				throw TRException("Sample locations do not match standard multisample pattern.");
			}
		}


		ResetStates();
		Present();
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Samples::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}


//////////////////////////////////////////////////////////////////////////
// CSupportTest
//////////////////////////////////////////////////////////////////////////

extern DXGI_FORMAT DFormats[]; 
extern UINT DFormatsSize;
extern UINT StandardSampleCounts[];
extern UINT StandardSampleCountsSize;


void CSupportTest::InitTestParameters()
{
	InitMultisampleParameters();
	testfactor::RFactor rfDFormats = AddParameterValueSet( m_pRTFormatParam,
		new CTableValueSet<DXGI_FORMAT>( DFormats, sizeof( DXGI_FORMAT ), DFormatsSize ) ) ;
	
	SetRootTestFactor ( rfDFormats + m_rfRTFormatTypedAll );
}

TEST_RESULT CSupportTest::SetupTestCase()
{
	// checking render target support
	UINT caps;
	HRESULT hr = GetDevice()->CheckFormatSupport( m_RTFormat, &caps );
	if( FAILED( hr ) )
	{
		// If the driver doesn't support the 11.1 DDI then it might fail for some formats
		if( !GetFramework()->Is11_1DDISupported( GetDevice() ) &&		// Check to see if the driver supports the 11.1 DDI
			CD3D10FormatHelper_Test::Is16BitBGRFormat( m_RTFormat ) )	// The 16 bit formats (B5G6R5, etc...) are only required to pass CheckFormatSupport on the 11.1 DDI
		{
			WriteToLog( _T( "CSupportTest::SetupTestCase(): CheckFormatSupport() failed. However the driver doesn't support the D3D11.1 DDI, so this is OK for the 16-bit formats. " ) );
			return RESULT_SKIP;
		}

		WriteToLog( _T( "CSupportTest::SetupTestCase(): CheckFormatSupport() failed." ) );
		return RESULT_FAIL;
	}

	if ( !(caps & ( D3D11_FORMAT_SUPPORT_RENDER_TARGET | D3D11_FORMAT_SUPPORT_DEPTH_STENCIL )) )
	{
		WriteToLog( _T( "CSupportTest::SetupTestCase(): Format does not support RENDER_TARGET or DEPTH_STENCIL." ) );
		return RESULT_SKIP;
	}
	return RESULT_PASS;
}

TEST_RESULT CSupport4xTest::ExecuteTestCase()
{
	HRESULT hr;
	UINT uLevels;
	const bool bFL10_1OrLess = GetFramework()->m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_1;
	const bool bFL10_1OrGreater = GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1;
	
	if( FAILED( hr = GetDevice()->CheckMultisampleQualityLevels( m_RTFormat, 4, &uLevels ) ) )
	{
		WriteToLog("CheckMultisampleQualityLevels failed. hr = (%s)", D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}
	if (uLevels == 0)
	{
		// This will intentionally fall through to the WriteToLog unless a break is encountered
		// ONLY fall through if the Format/FeatureLevel combination is OPTIONAL
		switch(m_RTFormat)
		{
		case DXGI_FORMAT_B5G6R5_UNORM:
			if( bFL10_1OrGreater )
			{
				// DXGI_FORMAT_B5G6R5_UNORM is required for 4x on FL10.1+
				break;
			}

		case DXGI_FORMAT_B4G4R4A4_UNORM:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
			WriteToLog(_T("%s is optional for 4x MSAA on Feature Level %s"), ToString(m_RTFormat).c_str(), ToString(GetFramework()->m_D3DOptions.FeatureLevel).c_str()); 
			return RESULT_SKIP;

		default:
			// Don't do anything here
			break;
		}

		if ( bFL10_1OrLess
			&& GetBitsPerElement(m_RTFormat) >= 64 )
		{
			WriteToLog(_T("Formats with 64 bits per sample or more are optional to support 4xMSAA for FL 10.1")); 
			return RESULT_SKIP;		
		}
		else
		{
			WriteToLog("Format %s does not support 4x MSAA.", ToString(m_RTFormat).c_str());
			return RESULT_FAIL;
		}
	}
	return RESULT_PASS;
}

TEST_RESULT CSupport8xTest::ExecuteTestCase()
{
	HRESULT hr;
	UINT uLevels;
	const bool bFL11_1OrLess = GetFramework()->m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_11_1;
	const bool bFL11_0OrGreater = GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0;
	
	if( FAILED( hr = GetDevice()->CheckMultisampleQualityLevels( m_RTFormat, 8, &uLevels ) ) )
	{
		WriteToLog("CheckMultisampleQualityLevels failed. hr = (%s)", D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}
	if (uLevels == 0)
	{
		// This will intentionally fall through to the WriteToLog unless a break is encountered
		// ONLY fall through if the Format/FeatureLevel combination is OPTIONAL
		switch(m_RTFormat)
		{
		case DXGI_FORMAT_B5G6R5_UNORM:
			if( bFL11_0OrGreater )
			{
				// DXGI_FORMAT_B5G6R5_UNORM is required for 8x on FL11.0+
				break;
			}

		case DXGI_FORMAT_B4G4R4A4_UNORM:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
			WriteToLog(_T("%s is optional for 8x MSAA on Feature Level %s"), ToString(m_RTFormat).c_str(), ToString(GetFramework()->m_D3DOptions.FeatureLevel).c_str()); 
			return RESULT_SKIP;

		default:
			// Don't do anything here
			break;
		}

		if ( bFL11_1OrLess 
			&& GetBitsPerElement(m_RTFormat) >= 128 )
		{
			WriteToLog(_T("Formats with 128 bits per sample or more are optional to support 8xMSAA for FL 11.x")); 
			return RESULT_SKIP;
		}
		else
		{
			WriteToLog("Format %s does not support 8x MSAA.", ToString(m_RTFormat).c_str());
			return RESULT_FAIL;
		}
	}
	return RESULT_PASS;
}


//////////////////////////////////////////////////////////////////////////
// CSampleTest
//////////////////////////////////////////////////////////////////////////
extern UINT CompactMultisampleFormatsSize;
extern DXGI_FORMAT CompactMultisampleFormats[];

void CSampleTest::InitTestParameters()
{
	InitMultisampleParameters();

	// topology
	CTestCaseParameter< D3D11_PRIMITIVE_TOPOLOGY > *pTopParam = AddParameter( _T("Topology") , &m_Topology );
	CUserValueSet< D3D11_PRIMITIVE_TOPOLOGY > *pTopSet = new CUserValueSet< D3D11_PRIMITIVE_TOPOLOGY >();
	
	pTopSet->AddValue( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	pTopSet->AddValue( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	testfactor::RFactor rfTopTriangle = AddParameterValueSet< D3D11_PRIMITIVE_TOPOLOGY >  ( pTopParam, pTopSet );
	
	pTopSet->AddValue( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );
	pTopSet->AddValue( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
	testfactor::RFactor rfTop = AddParameterValueSet< D3D11_PRIMITIVE_TOPOLOGY >  ( pTopParam, pTopSet );

	
	testfactor::RFactor rfSampleCountStandard = AddParameterValueSet<UINT>( m_pSampleCountParam, 
		new CTableValueSet<UINT>( StandardSampleCounts, sizeof(UINT), StandardSampleCountsSize ) );
	testfactor::RFactor rfSampleCountOne = AddParameterValue<UINT>( m_pSampleCountParam, 1 );
	testfactor::RFactor rfSampleCount32 = AddParameterValue<UINT>( m_pSampleCountParam, 32 );

	testfactor::RFactor rfQualityLevelZero = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_ZERO );
	testfactor::RFactor rfQualityLevelMaxSupported = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_MAX_SUPPORTED );
	testfactor::RFactor rfQualityLevelStandard = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_STANDARD );
	testfactor::RFactor rfQualityLevelCenter = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_CENTER );

	
	testfactor::RFactor rfSampleDescTop = 
		rfSampleCountOne * rfQualityLevelZero * rfTop + 
		rfSampleCount32 * ( rfQualityLevelZero + rfQualityLevelMaxSupported ) * rfTop + 
		// only sample counts 1(trival), 2, 4, 8 and 16 support standard and center sample patterns
		rfSampleCountStandard * ( rfQualityLevelZero + rfQualityLevelMaxSupported + rfQualityLevelStandard ) * rfTop +
		rfSampleCountStandard * ( rfQualityLevelCenter ) * rfTopTriangle;

	
	CUserValueSet<DXGI_FORMAT> *pRTFormatValues = new CUserValueSet<DXGI_FORMAT>;
	for (UINT i = 0; i < CompactMultisampleFormatsSize; i++)
	{
		UINT uFlags = GetFlags( CompactMultisampleFormats[i] );
		if ( !( (uFlags & FF_UINT) || (uFlags & FF_SINT) ) )
			pRTFormatValues->AddValue( CompactMultisampleFormats[i] );
	}
	testfactor::RFactor rfRTFormatSampleFreq = AddParameterValueSet<DXGI_FORMAT>(m_pRTFormatParam, pRTFormatValues);
	
	SetRootTestFactor ( rfRTFormatSampleFreq * rfSampleDescTop );
}

TEST_RESULT CSampleTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	tRes = SetFormatAndQualityLevel( false );
	if ( tRes != RESULT_PASS )
	{
		return tRes;
	}

	ID3D10Blob *pBlob(NULL), *pErrorBlob(NULL);
	pPSLinear = NULL;
	pPSSample = NULL;

	try 
	{
		HRESULT hr;
		SetupD3D();
	
		// create shaders
		char szPSTemplate[] = 
		"struct PSIn																\n"
		"{																			\n"
		"	float4 pos : SV_Position;												\n"
		"	%s float2 data : tex;													\n"
		"};																			\n"
		"struct PSOut																\n"
		"{																			\n"
		"	float4 Color : SV_Target;												\n"
		"};																			\n"
		"PSOut main(PSIn input)														\n"
		"{																			\n"
		"	PSOut output;															\n"
		"	output.Color = float4( input.data, 0.0, 1.0 );							\n"
		"	return output;															\n"
		"}";
		
		// compile and create pixel shaders
		char szPS[ARRAY_SIZE(szPSTemplate) + 100];

		char *sShaderModel;
		if (GetFramework()->m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_1)
			sShaderModel = "ps_4_1";
		else if (GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
			sShaderModel = "ps_5_0";
		else
			sShaderModel = "ps_4_0";

		_snprintf( szPS, ARRAY_SIZE(szPSTemplate) + 99, szPSTemplate, "linear");
		szPS[ARRAY_SIZE(szPSTemplate) + 99] = 0;
		if ( FAILED( hr = D3DX10CompileFromMemory( szPS, strlen(szPS), NULL, NULL, NULL, "main", sShaderModel, 0, 0, NULL, &pBlob, &pErrorBlob, NULL )) )
		{
			WriteToLog("%s", pErrorBlob->GetBufferPointer());
			throw TRException("Failed to compile PS.", hr);
		}
		if ( FAILED( hr = GetDevice()->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPSLinear )) )
			throw TRException("Failed to create PS.", hr);
		
		SAFE_RELEASE( pBlob );
		SAFE_RELEASE( pErrorBlob );

		// Sample frequency interpolation and MSAA lines rasterization are undefined 
		// when using the center sample patterns (D3D10_CENTER_MULTISAMPLE_PATTERN)
		// that have more than one sample overlapping at the center of the pixel.
		if ( m_uSampleCount > 1 && m_QualityLevelScenario == QUALITY_LEVEL_CENTER)
			m_bTestSampleFreq = false;
		else
			m_bTestSampleFreq = true;


		if ( m_bTestSampleFreq )
		{
			_snprintf( szPS, ARRAY_SIZE(szPSTemplate) + 99, szPSTemplate, "sample");
			szPS[ARRAY_SIZE(szPSTemplate) + 99] = 0;
			if ( FAILED( hr = D3DX10CompileFromMemory( szPS, strlen(szPS), NULL, NULL, NULL, "main", sShaderModel, 0, 0, NULL, &pBlob, &pErrorBlob, NULL )) )
			{
				WriteToLog("%s", pErrorBlob->GetBufferPointer());
				throw TRException("Failed to compile PS.", hr);
			}

			if ( FAILED( hr = GetDevice()->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPSSample )) )
			{
				throw TRException("Failed to create PS.", hr);
			}

			SAFE_RELEASE( pBlob );
			SAFE_RELEASE( pErrorBlob );
		}

		// interpolation data
		m_vStart = D3DXVECTOR2(0, 100.f);
		m_vEnd = D3DXVECTOR2(1.f, 1.f);

		// fill vertex buffers
		VERTEX *pVerts;
		D3D11_MAPPED_SUBRESOURCE mappedRes;
		if( FAILED( hr = GetEffectiveContext()->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
			throw TRException("[CMultisampleTest::CoverPixel] Map on m_pVertexBuffer failed.", hr, RESULT_FAIL);
		pVerts = (VERTEX*) mappedRes.pData;

		if ((m_Topology == D3D11_PRIMITIVE_TOPOLOGY_LINELIST) || (m_Topology == D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP))
		{
			m_uVertexCount = 2;

			pVerts->pos = D3DXVECTOR2(0,0.5f);
			pVerts->tex = m_vStart;
			pVerts++;

			pVerts->pos = D3DXVECTOR2(4.0f, 0.5f);
			pVerts->tex = m_vEnd;
		}
		else if ( m_Topology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST )
		{
			m_uVertexCount = 6;

			// tri 1
			pVerts->pos = D3DXVECTOR2(0,0);
			pVerts->tex = m_vStart;
			pVerts++;

			pVerts->pos = D3DXVECTOR2(0,1);
			pVerts->tex = m_vStart;
			pVerts++;

			pVerts->pos = D3DXVECTOR2(4,0);
			pVerts->tex = m_vEnd;
			pVerts++;

			// tri 2
			pVerts->pos = D3DXVECTOR2(0,1);
			pVerts->tex = m_vStart;
			pVerts++;

			pVerts->pos = D3DXVECTOR2(4,1);
			pVerts->tex = m_vEnd;
			pVerts++;

			pVerts->pos = D3DXVECTOR2(4,0);
			pVerts->tex = m_vEnd;
			pVerts++;
		}
		else
		{
			m_uVertexCount = 4;

			// tri 1
			pVerts->pos = D3DXVECTOR2(0,0);
			pVerts->tex = m_vStart;
			pVerts++;

			pVerts->pos = D3DXVECTOR2(0,1);
			pVerts->tex = m_vStart;
			pVerts++;

			pVerts->pos = D3DXVECTOR2(4,0);
			pVerts->tex = m_vEnd;
			pVerts++;

			// tri 2 
			pVerts->pos = D3DXVECTOR2(4,1);
			pVerts->tex = m_vEnd;
			pVerts++;
		}
		GetEffectiveContext()->Unmap(m_pVertexBuffer,0);
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Samples::SetupTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}

void CSampleTest::CleanupTestCase()
{
	CMultisampleTest::CleanupD3D();
	SAFE_RELEASE( pPSLinear );
	SAFE_RELEASE( pPSSample );
}

TEST_RESULT CSampleTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	try 
	{
		HRESULT hr;
		if( RESULT_FAIL == m_Finder.FindSamples() )
			throw TRException("FindSamples failed.");	

		// setup
		ResetStates();
		GetEffectiveContext()->IASetPrimitiveTopology( m_Topology );

		// Pass 1: linear
		FLOAT ClearColor[4] = { 0, 0, 0, 0 };
		GetEffectiveContext()->ClearRenderTargetView( m_pRTView, ClearColor );
		GetEffectiveContext()->PSSetShader(pPSLinear,NULL,0);
		GetEffectiveContext()->Draw(m_uVertexCount, 0);

		if ( FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

		// store results
		vector<D3DXVECTOR4> vPixelFreq(m_RTWidth * m_RTHeight * m_uSampleCount);
		MSHelper::Read(GetFramework(), GetFramework()->GetRTTexture(), (FLOAT*)&vPixelFreq[0], m_RTVFormat);

		vector<D3DXVECTOR4> vSampleFreq(m_RTWidth * m_RTHeight * m_uSampleCount);

		if ( m_bTestSampleFreq )
		{
			// Pass 2: sample
			ResetStates();
			GetEffectiveContext()->IASetPrimitiveTopology( m_Topology );
			GetEffectiveContext()->ClearRenderTargetView( m_pRTView, ClearColor );
			GetEffectiveContext()->VSSetShader(m_pVS,NULL,0);
			GetEffectiveContext()->PSSetShader(pPSSample,NULL,0);
			GetEffectiveContext()->Draw(m_uVertexCount, 0);

			if ( FAILED( hr = ExecuteEffectiveContext() ) )
				throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

			// store results
			MSHelper::Read(GetFramework(), GetFramework()->GetRTTexture(), (FLOAT*)&vSampleFreq[0], m_RTVFormat);
		}

		// convert data 
		
		// get format info
		bool bGComp = GetBitsPerComponent(1, m_RTFormat) > 0;
		UINT uFlags = GetFlags(m_RTVFormat);
		bool bTestG = bGComp && !((uFlags & FF_UNORM) || (uFlags & FF_SNORM));
		
		// verify linear data is linear
		UINT uSlice = m_RTWidth * m_RTHeight;
		for (UINT uX = 0; uX < 4; ++uX)
		{
			// verify all the samples in a given pixel are the same
			D3DXVECTOR4 &vLinearSample0 = vPixelFreq[uX];
			for (UINT uSample = 1; uSample < m_uSampleCount; ++uSample)
			{
				D3DXVECTOR4 &vSample = vPixelFreq[uSlice*uSample + uX];
				if (vSample != vPixelFreq[uX])
				{
					WriteToLog("Pixel [%d,0]", uX);
					WriteToLog("Sample 0: float4(%f,%f,0,1.0f)", vPixelFreq[uX].x, vPixelFreq[uX].y);
					WriteToLog("Sample %d: float4(%f,%f,0,1.0f)", uSample, vSample.x, vSample.y);
					throw TRException("ERROR: all samples not the same with linear interpolation on MS render target.");
				}
			}
			
			// verfiy data is in between adjacent pixels
			D3DXVECTOR4 vLeft, vRight;
			if (uX == 0) vLeft = D3DXVECTOR4(m_vStart.x, m_vStart.y, 0, 1.f);
			else vLeft = vPixelFreq[uX-1];
			if (uX == 3) vRight = D3DXVECTOR4(m_vEnd.x, m_vEnd.y, 0, 1.f);
			else vRight = vPixelFreq[uX+1];
			bool bFailed = false;

			// assume greater value is on the right
			if (m_vEnd.x < m_vStart.x)
				swap(vLeft.x, vRight.x);
			if (m_vEnd.y < m_vStart.y)
				swap(vLeft.y, vRight.y);

			// test x
			if ((vPixelFreq[uX].x >= vRight.x) || (vPixelFreq[uX].x <= vLeft.x))
				bFailed = true;

			// test y
			if (bTestG && ((vPixelFreq[uX].y >= vRight.y) || (vPixelFreq[uX].y <= vLeft.y)))
				bFailed = true;
			
			if (bFailed)
			{
				for (UINT i = 0; i < 4; ++i)
					WriteToLog("Pixel [%d,0] Sample [0]: (%f,%f,0,1.0f)", i, vPixelFreq[i].x, vPixelFreq[i].y);
				WriteToLog("Start data: (%f,%f,0,1.0f)", m_vStart.x, m_vStart.y);
				WriteToLog("End data: (%f,%f,0,1.0f)", m_vEnd.x, m_vEnd.y);
				throw TRException("ERROR: linear interpolation failed.");
			}
		}

		if ( m_bTestSampleFreq )
		{
			// verify sample frequency data
			for (UINT uX = 0; uX < 4; ++uX)
			{
				for (UINT uSample = 0; uSample < m_uSampleCount; ++uSample)
				{
					D3DXVECTOR4 &vSample = vSampleFreq[uSlice*uSample + uX];
					POINT pLoc = m_Finder.GetLoc(uSample);
					// if its in the center, it should match the sample frequency data
					if ((pLoc.x == 128) && (pLoc.y == 128) && (vSample != vPixelFreq[uX]))
					{
						WriteToLog("Sample #%d", uSample);
						WriteToLog("Pixel frequency: (%f, %f, 0, 1.0f)", vPixelFreq[uX].x, vPixelFreq[uX].y);
						WriteToLog("Sample frequency: (%f, %f, 0, 1.0f)", vSample.x, vSample.y);
						throw TRException("Pixel and Sample frequency should be the same if the sample is on the pixel center");
					}

					float fT = (uX + pLoc.x/256.f)/4.f;
					D3DXVECTOR2 vExpected = m_vStart + fT * (m_vEnd - m_vStart);
					D3DXVECTOR2 vUpperBound = m_vStart + (fT+0.1f) * (m_vEnd - m_vStart);
					D3DXVECTOR2 vLowerBound = m_vStart + (fT-0.1f) * (m_vEnd - m_vStart);
					
					// assume greater value is on the right
					if (m_vEnd.x < m_vStart.x)
						swap(vUpperBound.x, vLowerBound.x);
					if (m_vEnd.y < m_vStart.y)
						swap(vUpperBound.y, vLowerBound.y);
					
					bool bFailed = false;
					if ((vSample.x > vUpperBound.x) || (vSample.x < vLowerBound.x))
						bFailed = true;

					if (bTestG && ((vSample.y > vUpperBound.y) || (vSample.y < vLowerBound.y)))
						bFailed = true;

					if (bFailed)
					{
						WriteToLog("Sample[%d] location: [%d, %d]", uSample, (pLoc.x/32 - 16), (pLoc.y/32 - 16));
						WriteToLog("Expected: (%f,%f,0,1.0f)", vExpected.x, vExpected.y);
						WriteToLog("Actual: (%f,%f,0,1.0f)", vSample.x, vSample.y);
						WriteToLog("Start data: (%f,%f,0,1.0f)", m_vStart.x, m_vStart.y);
						WriteToLog("End data: (%f,%f,0,1.0f)", m_vEnd.x, m_vEnd.y);
						throw TRException("Sample frequency interpolated data is way off expected value (no specific precision is required so using +/-10% between start point and end point)");

					}
				}
			}
		}
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Resolve::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}