//////////////////////////////////////////////////////////////////////////
//  Fill.cpp
//  created:	2005/07/18
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "fill.h"
#include <sstream>
using namespace std;

struct VERTEXFILL {	int x, y; };
struct TRIANGLE { VERTEXFILL a, b, c; };
struct FILL_POINT { float x, y; };
 
TRIANGLE TriangleList[] = 
{
									// DESCRIPTION						SAMPLE LIT
									// ------------------------------	----------
	{{-1,-1 }, {-1, 1 }, { 1, 0 }}, // Sample inside triangle			YES
	
	{{-1, 0 }, { 1, 0 }, {-1,-1 }}, // Top edge on sample				YES
	{{ 0, 1 }, { 0,-1 }, { 1,-1 }}, // Left edge on sample				YES
	{{-1, 1 }, { 1,-1 }, {-1,-1 }}, // Other edge on sample				NO

	{{ 0, 0 }, { 0,-1 }, { 1, 0 }}, // Top + Left corner on sample		YES
	{{-1, 0 }, { 0, 0 }, {-1,-1 }}, // Top + Other corner on sample		NO
	{{ 0, 0 }, { 0, 1 }, { 1, 0 }}, // Left + Other corner on sample	NO
	{{ 0, 0 }, { 1, 1 }, { 1, -1}}, // Left + Left corner on sample		YES
	{{-1, 1 }, { 0, 0 }, {-1, -1}}, // Other + Other corner on sample	NO

	{{ 0, 0 }, { 0, 0 }, { 0,-1 }}, // Degenerate triangle line			NO
	{{ 0, 0 }, { 0, 0 }, { 0, 0 }}, // Degenerate triangle point		NO
};

BOOL TriangleResultList[] =
{
									// DESCRIPTION						SAMPLE LIT
									// ------------------------------	----------
	true,							// Sample inside triangle			YES

	true,							// Top edge on sample				YES
	true,							// Left edge on sample				YES
	false,							// Other edge on sample				NO

	true,							// Top + Left corner on sample		YES
	false,							// Top + Other corner on sample		NO
	false,							// Left + Other corner on sample	NO
	true,							// Left + Left corner on sample		YES
	false,							// Other + Other corner on sample	NO

	false,							// Degenerate triangle line			NO
	false,							// Degenerate triangle point		NO
};

//Offset from sampling point
FILL_POINT PointList[] =
{
	{ 0.0f, 0.0f }, // Sample inside point

	{ 0.0f, 0.5f }, // Top edge on sample
	{ 0.5f, 0.0f }, // Left edge on sample
	{ 0.0f,-0.5f }, // Bottom edge on sample
	{-0.5f, 0.0f }, // Right edge on sample

	{ 0.5f, 0.5f }, // Top + Left corner on sample
	{-0.5f, 0.5f }, // Top + Right corner on sample
	{ 0.5f,-0.5f }, // Bottom + Left corner on sample
	{-0.5f,-0.0f }, // Bottom + Right corner on sample

	{ 0.5f + 1.0f/256.0f, 0.0f }, // Just off right edge
	{ 0.0f, -0.5f - 1.0f/256.0f }, // Just off top edge
};

BOOL PointResultList[] =
{
		true,

		true,
		true,
		false,
		false,

		true,
		false,
		false,
		false,

		false,
		false,
};

TEST_RESULT CMultisampleTest_Fill::SetupSampleFinder()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;
	try 
	{
		m_pDevice = GetDevice1();
		SetupD3D();

		if ( UpdateFormatOrSampleDesc() )
		{
			if (RESULT_PASS != (tRes = m_Finder.FindSamples())) 
				throw TRException("FindSamples failed.", tRes);
			if (m_bOrder)
				if( FAILED( hr = m_Finder.OrderSamples() ) )
					throw TRException("FindSamples failed.", hr, RESULT_FAIL);
		}
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Fill::SetupSampleFinder()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}
	return tRes;
}

TEST_RESULT CMultisampleTest_Fill::SetupTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	tRes = SetFormatAndQualityLevel( false );
	if ( tRes != RESULT_PASS )
	{
		return tRes;
	}
	tRes = SetupSampleFinder( );
	if ( tRes != RESULT_PASS )
	{
		return tRes;
	}

	try 
	{
		//
		// pixel pos changes 
		//
		ResolvePixPos();	

		// setup const data
		float *bufData;
		D3D11_MAPPED_SUBRESOURCE mappedRes;
		if( FAILED( hr = GetEffectiveContext()->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
			throw TRException("Map on m_pConstBuffer Failed", hr, RESULT_FAIL);
		bufData = (float*) mappedRes.pData;
		// Inverse viewport scale.
		bufData[2] = 2 / (float)m_RTWidth;
		bufData[3] = -2 / (float)m_RTHeight;
		// Prescaled inverse viewport translation.
		bufData[0] = -1.0f;
		bufData[1] = 1.0f;
		//alpha value;
		bufData[4] = 1.0f;
		//depth
		bufData[5] = 1.0f;
		//pixel position
		bufData[6] = (float)m_vPixPos.x;
		bufData[7] = (float)m_vPixPos.y;
		GetEffectiveContext()->Unmap(m_pConstBuffer,0);

		//
		// update sample finder constant buffer
		//
		POINT pix = {m_PixBox.left, m_PixBox.top};
		m_Finder.Update( pix );

		ResetStates();
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Fill::SetupTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}
	
	return tRes;
}

void CMultisampleTest_Fill::CleanupTestCase()
{
	CMultisampleTest::CleanupD3D();
}

TEST_RESULT CMultisampleTest_Fill::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;
	D3D11_MAPPED_SUBRESOURCE texmap;

	try
	{
		// setup geometry
		m_uVertexCount = 0;
		// convert subpixel coordinates to screen coordinates
		if (m_uSampleIndex >= m_uSampleCount)
			throw TRException("uSampleIndex invalid.");
		POINT pos = m_Finder.GetLoc(m_uSampleIndex);
		float x = m_vPixPos.x + pos.x/256.0f;
		float y = m_vPixPos.y + pos.y/256.0f;

		// clear
		FLOAT ClearColor[4] = { 0, 0, 0, 0 };
		m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );

		Draw( x, y );

		if ( FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

		// check resulting pixel
		BOOL bResult;
		if( RESULT_PASS != m_Finder.TestPixelNonZero(bResult) )
			throw TRException("TestPixelNonZero failed.");
		Validate( bResult );
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Fill::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}

	try
	{
		Present();
	}
	catch (TRException& exc)
	{
		exc.Prepend("[CMultisampleTest_Fill::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}
	
	if ( RESULT_FAIL == tRes )
	{
		POINT pos = m_Finder.GetLoc(m_uSampleIndex);
		WriteToLog( _T("Sample point at x:%d y:%d "), pos.x, pos.y  );
		LogInfo();
	}
	return tRes;	
}
	
void CMultisampleTest_FillTriangle::InitTestParameters()
{
	InitMultisampleParameters();
	CTestCaseParameter< UINT > *pSIndex = AddParameter( _T( "SampleIndex" ), &m_uSampleIndex );
	pSIndex->SetDefaultValue( 0 );

	const numSampleCounts = 6;
	testfactor::RFactor rfSampleCount[numSampleCounts];
	testfactor::RFactor rfSindex[numSampleCounts];
	for( size_t i = 0; i < numSampleCounts; i++ )
	{
		rfSampleCount[i] = AddParameterValue<UINT>( m_pSampleCountParam, (UINT)pow((float)2, (float)i) );
		rfSindex[i] = AddParameterValueSet<UINT>( pSIndex, 
			new CRangeValueSet< UINT >(0, (UINT)pow((float)2, (float)i)-1 , 1) );
	}

	testfactor::RFactor rfQualityLevelZero = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_ZERO );
	testfactor::RFactor rfQualityLevelMaxSupported = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_MAX_SUPPORTED );
	testfactor::RFactor rfQualityLevelStandard = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_STANDARD );
	testfactor::RFactor rfQualityLevelCenter = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_CENTER );

	testfactor::RFactor rfAllQualityLevels = 
		rfQualityLevelZero + rfQualityLevelMaxSupported + rfQualityLevelStandard + rfQualityLevelCenter;

	// here the order of the test cases has impact on the performance, since we'll skip FindSmaples() 
	// the the format/sampleCount/QualityLevel combination is the same as the previous test case.
	// So make sure to run the test cases with the same format/sampleCount/QualityLevel together.
	testfactor::RFactor rfSampleDescIndex = 
		 rfSampleCount[0] * rfQualityLevelZero * rfSindex[0] + 
		 rfSampleCount[5] * ( rfQualityLevelZero + rfQualityLevelMaxSupported ) * rfSindex[5] + 
		// only sample counts 1(trival), 2, 4, 8 and 16 support standard and center sample patterns
		 rfSampleCount[1] * rfAllQualityLevels * rfSindex[1] + 
		 rfSampleCount[2] * rfAllQualityLevels * rfSindex[2] + 
		 rfSampleCount[3] * rfAllQualityLevels * rfSindex[3] + 
		 rfSampleCount[4] * rfAllQualityLevels * rfSindex[4] ; 
		 
	testfactor::RFactor rfSampleDescIndex2 = rfSampleCount[1] * rfSindex[1] * 
		( rfQualityLevelZero + rfQualityLevelMaxSupported + rfQualityLevelStandard + rfQualityLevelCenter );

	testfactor::RFactor rfSampleDescIndexformat = m_rfRTFormatTypedSubset * rfSampleDescIndex + 
		( m_rfRTFormatTypedAll + m_rfRTFormatTypeless ) * rfSampleDescIndex2;

	testfactor::RFactor triangle = AddParameter( _T( "Triangle" ), &m_TriangleID, new CRangeValueSet< UINT > (0, ARRAY_SIZE(TriangleList)-1, 1) );
	
	SetRootTestFactor ( ( rfSampleDescIndexformat % m_rfPixPos ) * triangle );
	
	m_bOrder = false;
}

void CMultisampleTest_FillTriangle::Draw( float x, float y )
{
	HRESULT hr;
	float size = 1.0f/256;

	// fill vertex buffer
	VERTEX *pDestVerts;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if( FAILED( hr = GetEffectiveContext()->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
		throw TRException("m_pVertexBuffer->Map() failed.", hr, RESULT_FAIL);
	pDestVerts = (VERTEX*) mappedRes.pData;

	pDestVerts[0].pos = D3DXVECTOR2(x + TriangleList[m_TriangleID].a.x * size, y - TriangleList[m_TriangleID].a.y * size);
	pDestVerts[1].pos = D3DXVECTOR2(x + TriangleList[m_TriangleID].b.x * size, y - TriangleList[m_TriangleID].b.y * size);
	pDestVerts[2].pos = D3DXVECTOR2(x + TriangleList[m_TriangleID].c.x * size, y - TriangleList[m_TriangleID].c.y * size);

	GetEffectiveContext()->Unmap(m_pVertexBuffer,0);

	//If we're on the top edge, these need to be adjusted to avoid producing degenerate triangles
	if ( 0.0f == y && (PP_TOP_RIGHT == m_PixPosID || PP_TOP_LEFT == m_PixPosID) )
	{
		switch (m_TriangleID)
		{
		case 2:
			//This case would be a duplicate of triangle 4

			throw TRException("Redundant test.  Clipped triangle is duplicate of triangle 4.", TEST_RESULT(RESULT_SKIP));
			break;
		}
	}

	//If we're on the left edge, these need to be adjusted to avoid producing degenerate triangles
	if ( 0.0f == x && (PP_BOTTOM_LEFT == m_PixPosID || PP_TOP_LEFT == m_PixPosID) )
	{
		switch (m_TriangleID)
		{
		case 0:
			//This case would be a duplicate of triangle 2

			throw TRException("Redundant test.  Clipped triangle is duplicate of triangle 2.", TEST_RESULT(RESULT_SKIP));
			break;

		case 1:
			//This case would be a duplicate of triangle 4

			throw TRException("Redundant test.  Clipped triangle is duplicate of triangle 4.", TEST_RESULT(RESULT_SKIP));
			break;
		}
	}
	
	// render
	m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	m_pDeviceContext->Draw(3, 0);
}
void CMultisampleTest_FillTriangle::LogInfo()
{
	WriteToLog( _T("Triangle point 0: x:%d, y:%d"), TriangleList[m_TriangleID].a.x, TriangleList[m_TriangleID].a.y );
	WriteToLog( _T("Triangle point 1: x:%d, y:%d"), TriangleList[m_TriangleID].b.x, TriangleList[m_TriangleID].b.y );
	WriteToLog( _T("Triangle point 2: x:%d, y:%d"), TriangleList[m_TriangleID].c.x, TriangleList[m_TriangleID].c.y );
}

TEST_RESULT CMultisampleTest_FillTriangle::Validate( BOOL bResult )
{
	if ( bResult && !TriangleResultList[m_TriangleID])
		throw TRException("Sample lit when it shouldn't be.\n");
	if ( !bResult && TriangleResultList[m_TriangleID])
		throw TRException("Sample not lit when it should be.\n");

	return RESULT_PASS;
}
	
void CMultisampleTest_FillPoint::InitTestParameters()
{
	InitMultisampleParameters();
	CTestCaseParameter< UINT > *pSIndex = AddParameter( _T( "SampleIndex" ), &m_uSampleIndex );
	pSIndex->SetDefaultValue( 0 );

	const numSampleCounts = 6;
	testfactor::RFactor rfSampleCount[numSampleCounts];
	testfactor::RFactor rfSindex[numSampleCounts];
	for( size_t i = 0; i < numSampleCounts; i++ )
	{
		rfSampleCount[i] = AddParameterValue<UINT>( m_pSampleCountParam, (UINT)pow((float)2, (float)i) );
		rfSindex[i] = AddParameterValueSet<UINT>( pSIndex, 
			new CRangeValueSet< UINT >(0, (UINT)pow((float)2, (float)i)-1 , 1) );
	}

	testfactor::RFactor rfQualityLevelZero = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_ZERO );
	testfactor::RFactor rfQualityLevelMaxSupported = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_MAX_SUPPORTED );
	testfactor::RFactor rfQualityLevelStandard = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_STANDARD );
	testfactor::RFactor rfQualityLevelCenter = AddParameterValue<QUALITY_LEVEL_SCENARIO>( m_pQualityLevelParam,
											QUALITY_LEVEL_CENTER );

	testfactor::RFactor rfAllQualityLevels = 
		rfQualityLevelZero + rfQualityLevelMaxSupported + rfQualityLevelStandard + rfQualityLevelCenter;

	// here the order of the test cases has impact on the performance, since we'll skip FindSmaples() 
	// the the format/sampleCount/QualityLevel combination is the same as the previous test case.
	// So make sure to run the test cases with the same format/sampleCount/QualityLevel together.

	testfactor::RFactor rfSampleDescIndex = 
		 rfSampleCount[0] * rfQualityLevelZero * rfSindex[0] + 
		 rfSampleCount[5] * ( rfQualityLevelZero + rfQualityLevelMaxSupported ) * rfSindex[5] + 
		// only sample counts 1(trival), 2, 4, 8 and 16 support standard and center sample patterns
		 rfSampleCount[1] * rfAllQualityLevels * rfSindex[1] + 
		 rfSampleCount[2] * rfAllQualityLevels * rfSindex[2] + 
		 rfSampleCount[3] * rfAllQualityLevels * rfSindex[3] + 
		 rfSampleCount[4] * rfAllQualityLevels * rfSindex[4] ; 

	testfactor::RFactor rfSampleDescIndex2 = rfSampleCount[1] * rfSindex[1] * 
		( rfQualityLevelZero + rfQualityLevelMaxSupported + rfQualityLevelStandard + rfQualityLevelCenter );

	testfactor::RFactor rfSampleDescIndexformat = m_rfRTFormatTypedSubset * rfSampleDescIndex + 
		( m_rfRTFormatTypedAll + m_rfRTFormatTypeless ) * rfSampleDescIndex2;

	testfactor::RFactor point = AddParameter( _T( "Point" ), &m_PointID, new CRangeValueSet< UINT > (0, ARRAY_SIZE(PointList)-1, 1) );
	
	SetRootTestFactor ( ( rfSampleDescIndexformat % m_rfPixPos ) * point  );
	
	m_bOrder = true;
}

void CMultisampleTest_FillPoint::Draw( float x, float y )
{
	HRESULT hr;
	// mask all other samples
	m_pDeviceContext->OMSetBlendState( m_pBS, NULL, 1 << m_uSampleIndex );
	
	// fill vertex buffer
	VERTEX *pDestVerts;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if( FAILED( hr = GetEffectiveContext()->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
		throw TRException("m_pVertexBuffer->Map() failed.", hr, RESULT_FAIL);
	pDestVerts = (VERTEX*) mappedRes.pData;
	pDestVerts[0].pos = D3DXVECTOR2(x + PointList[m_PointID].x, y + PointList[m_PointID].y);
	GetEffectiveContext()->Unmap(m_pVertexBuffer,0);

	// render
	m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	m_pDeviceContext->Draw(1, 0);
}

void CMultisampleTest_FillPoint::LogInfo()
{
	WriteToLog( _T("Point x: %d, y: %d"), PointList[m_PointID].x, PointList[m_PointID].y );
}
TEST_RESULT CMultisampleTest_FillPoint::Validate( BOOL bResult )
{
	POINT pos = m_Finder.GetLoc(m_uSampleIndex);
	const float x = m_vPixPos.x + pos.x/256.0f + PointList[m_PointID].x;
	const float y = m_vPixPos.y + pos.y/256.0f + PointList[m_PointID].y;

	const bool bSkippable = ( x < 0 || y < 0 || x >= m_RTWidth || y >= m_RTHeight );

	if ( bResult && !PointResultList[m_PointID])
		throw TRException("Sample lit when it shouldn't be.", TEST_RESULT(bSkippable ? RESULT_SKIP : RESULT_FAIL) );
	if ( !bResult && PointResultList[m_PointID])
		throw TRException("Sample not lit when it should be.", TEST_RESULT(bSkippable ? RESULT_SKIP : RESULT_FAIL) );

	return RESULT_PASS;
}
