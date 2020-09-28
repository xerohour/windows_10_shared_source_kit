//////////////////////////////////////////////////////////////////////////
//  Interpolator.cpp
//  created:	2006/07/14
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////


#include <WGFTestCore.h>
#include <stdlib.h>

#include "rasterizer.h"
#include "interpolator.h"

#include "MinMaxRange.h"

#include "d3dmshelper.h"

//
//	structs
//
D3D_INTERPOLATOR FloatModeSet[] = {
	CONSTANT,
	LINEAR, 
	LINEAR_CENTROID,
	LINEAR_NOPERSPECTIVE,
	LINEAR_NOPERSPECTIVE_CENTROID,
	LINEAR_SAMPLE,
	LINEAR_NOPERSPECTIVE_SAMPLE,
};

D3D_INTERPOLATOR FloatModeSetNonSampleFreq[] = {
	CONSTANT,
	LINEAR, 
	LINEAR_CENTROID,
	LINEAR_NOPERSPECTIVE,
	LINEAR_NOPERSPECTIVE_CENTROID,
};

D3D_INTERPOLATOR FloatModeSetSampleFreq[] = {
	LINEAR_SAMPLE,
	LINEAR_NOPERSPECTIVE_SAMPLE,
};

D3D_INTERPOLATOR IntModeSet[] = {
	CONSTANT
};

DXGI_FORMAT FloatFormatSet[] = {
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_FLOAT
};

DXGI_FORMAT IntFormatSet[] = {
	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32_SINT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32_SINT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32_SINT,
	DXGI_FORMAT_R32G32B32A32_UINT,
	DXGI_FORMAT_R32G32B32A32_SINT
};

float FloatDataSet[4][5] = {
	{0.01f,		0.15f,	0.2456f,	377.0f,		985.f },
	{1.0f,		1.0f,	0.1f,		0.1f,		755.f },
	{377.0f,	0.1f,	1.0f,		0.9f,		837.3f },
	{15.0f,		0.5f,	0.5f,		0.01f,		739.345f }
};

// 
// foward declarations
//
tstring __stdcall ToString( D3D_INTERPOLATOR mode );
bool __stdcall FromString( D3D_INTERPOLATOR *pMode, const tstring &str );

//////////////////////////////////////////////////////////////////////////
// CInterpolator
//////////////////////////////////////////////////////////////////////////
CInterpolator::CInterpolator()
{
	m_RTFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
	m_uRTSampleCount = 1;
}

void CInterpolator::InitTestParameters()
{
	//	parameters
	CTestCaseParameter< D3D_INTERPOLATOR > *pModeParam = AddParameter( _T( "Mode" ), &m_Mode );
	CTestCaseParameter< DXGI_FORMAT > *pFormatParam = AddParameter( _T( "Format" ), &m_RTFormat );
	CTestCaseParameter< UINT > *pSampleCountParam = AddParameter( _T("SampleCount"), &m_uRTSampleCount);

	CTestCaseParameter< float > *pData0Param = AddParameter( _T("Data0") , &m_pData[0] );
	CTestCaseParameter< float > *pData1Param = AddParameter( _T("Data1") , &m_pData[1] );
	CTestCaseParameter< float > *pData2Param = AddParameter( _T("Data2") , &m_pData[2] );
	CTestCaseParameter< float > *pData3Param = AddParameter( _T("Data3") , &m_pData[3] );

	// fill mode factors
	testfactor::RFactor rfSampleCountOne     = AddParameterValueSet< UINT > (pSampleCountParam, new CRangeValueSet<UINT>( 1, 1, 1 ) );
	testfactor::RFactor rfSampleCountOneFour = AddParameterValueSet< UINT > (pSampleCountParam, new CRangeValueSet<UINT>( 1, 4, 3 ) );

	// Win7:688339
	// Since this test uses the D3D11_CENTER_MULTISAMPLE_PATTERN (tex2ddesc.SampleDesc.Quality = (m_uRTSampleCount > 1) ? D3D10_CENTER_MULTISAMPLE_PATTERN : 0;) we can only use a sample count of 1.
	// The 10.1 spec says: The standard center sample patterns (D3D10_CENTER_MULTISAMPLE_PATTERN) that have more than one sample
	// overlapping at the center of the pixel have a couple of usage restrictions:
	//   (1) Sample-frequency Pixel Shader execution is undefined.
	//   (2) MSAA lines (lines drawn as quadrilaterals / 2 triangles) are undefined.
	testfactor::RFactor rfFloatInterpolatorSampleFreq = AddParameterValueSet< D3D_INTERPOLATOR > ( pModeParam, 
		new CTableValueSet< D3D_INTERPOLATOR >( FloatModeSetSampleFreq, sizeof(D3D_INTERPOLATOR), ARRAY_SIZE(FloatModeSetSampleFreq) ) ); 
	testfactor::RFactor rfFloatInterpolatorNonSampleFreq = AddParameterValueSet< D3D_INTERPOLATOR > ( pModeParam, 
		new CTableValueSet< D3D_INTERPOLATOR >( FloatModeSetNonSampleFreq, sizeof(D3D_INTERPOLATOR), ARRAY_SIZE(FloatModeSetNonSampleFreq) ) ); 
	testfactor::RFactor rfIntInterpolator = AddParameterValueSet< D3D_INTERPOLATOR > ( pModeParam, 
		new CTableValueSet< D3D_INTERPOLATOR >( IntModeSet, sizeof(D3D_INTERPOLATOR), ARRAY_SIZE(IntModeSet) ) ); 

	testfactor::RFactor rfFloatFormat = AddParameterValueSet< DXGI_FORMAT > ( pFormatParam,
		new CTableValueSet< DXGI_FORMAT >( FloatFormatSet, sizeof(DXGI_FORMAT), ARRAY_SIZE(FloatFormatSet) ) ); 
	testfactor::RFactor rfIntFormat = AddParameterValueSet< DXGI_FORMAT > ( pFormatParam,
		new CTableValueSet< DXGI_FORMAT >( IntFormatSet, sizeof(DXGI_FORMAT), ARRAY_SIZE(IntFormatSet) ) ); 

	testfactor::RFactor rfFloatData = AddParameterValueSet< float > ( pData0Param, new CTableValueSet< float >( FloatDataSet[0], sizeof(float), ARRAY_SIZE(FloatDataSet[0]) ) ) %
									  AddParameterValueSet< float > ( pData1Param, new CTableValueSet< float >( FloatDataSet[1], sizeof(float), ARRAY_SIZE(FloatDataSet[1]) ) ) %
									  AddParameterValueSet< float > ( pData2Param, new CTableValueSet< float >( FloatDataSet[2], sizeof(float), ARRAY_SIZE(FloatDataSet[2]) ) ) %
									  AddParameterValueSet< float > ( pData3Param, new CTableValueSet< float >( FloatDataSet[3], sizeof(float), ARRAY_SIZE(FloatDataSet[3]) ) );


	// init common params
	InitRasterizerParameters();

	// float test
	testfactor::RFactor rfSampleFreqFloatTest = (rfFloatInterpolatorSampleFreq*rfFloatFormat*rfSampleCountOne*rfFloatData);
	testfactor::RFactor rfNonSampleFreqFloatTest = (rfFloatInterpolatorNonSampleFreq*rfFloatFormat*rfSampleCountOneFour*rfFloatData);
	testfactor::RFactor rfIntTest = (rfIntInterpolator*rfSampleCountOne*rfIntFormat);

	// set root
	SetRootTestFactor( m_rfFillModeSolid * 
					   m_rfCullModeNone * 
					   m_rfQuadSet * 
					  (rfSampleFreqFloatTest + rfNonSampleFreqFloatTest + rfIntTest) );
}

TEST_RESULT CInterpolator::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		// Sample-frequency execution is only supported on featurelevel 10.1 and above
		if(GetDevice()->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_1)
		{
			switch(m_Mode)
			{
			case LINEAR_SAMPLE:
			case LINEAR_NOPERSPECTIVE_SAMPLE:
				tRes = RESULT_SKIP;
				break;
			}
		}

		if(RESULT_PASS == tRes)
		{
			//
			// Type and Size are determined from format
			//
			DWORD flags = GetFlags( m_RTFormat );

			// get type
			if (flags & FF_FLOAT)
				m_Type = MODE_FLOAT;
			else if (flags & FF_UINT)
				m_Type = MODE_UINT;
			else
				m_Type = MODE_INT;

			// get size
			if (flags & FF_A)
				m_uSize = 4;
			else if (flags & FF_B)
				m_uSize = 3;
			else if (flags & FF_G)
				m_uSize = 2;
			else
				m_uSize = 1;

			// init d3d
			SetupD3D();
			CreateShaders();

			// create vb with new data values
			if (m_Type == MODE_FLOAT)
			{
				for (UINT i = 0; i < 4; ++i)
					m_VBData[4 + i].fData = m_pData[i];
				CreateVB();
			}

			// init common settings
			SetupRasterizerTestCase();

			// region of the buffer covered
			m_TexBox.back = 1;
			m_TexBox.front = 0;
			m_TexBox.left = 0;
			m_TexBox.right = 4;
			m_TexBox.top = 0;		
			m_TexBox.bottom = 4;
		}

	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CInterpolator::SetupTestCase()] ");
		if (tRes != RESULT_SKIP)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}

void CInterpolator::CreateShaders()
{
	HRESULT hr;
	// cleanup existing ps
	m_pDeviceContext->VSSetShader( NULL, NULL, 0 );
	m_pDeviceContext->PSSetShader( NULL, NULL, 0 );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pPS );

	char szVS[] = 
	"struct VSIn																\n"
	"{																			\n"
	"	float2 pos : pos;														\n"
	"	float fdata : fdata;													\n"
	"	uint  udata : udata;													\n"
	"	int   idata : idata;													\n"
	"};																			\n"
	"struct VSOut																\n"
	"{																			\n"
	"	float4 pos : SV_Position;												\n"
	"	%s%s data : data;														\n"
	"};																			\n"
	"cbuffer cb0 : register(b0)													\n"
	"{																			\n"
	"	float4 inverseViewport;													\n"
	"};																			\n"
	"VSOut main(VSIn input)														\n"
	"{																			\n"
	"	VSOut output;															\n"
	"	output.pos = float4( 0, 0, 0, 1.0 );									\n"
	"	output.pos.xy = input.pos * inverseViewport.zw + inverseViewport.xy;	\n"
	"	output.data = input.%s%s;												\n"

	"	return output;															\n"
	"}";

	char szPS[] = 
	"struct PSIn																\n"
	"{																			\n"
	"	float4 pos : SV_Position;												\n"
	"	%s %s%s data : data;													\n"
	"};																			\n"
	"struct PSOut																\n"
	"{																			\n"
	"	%s%s Color : SV_Target;													\n"
	"};																			\n"
	"cbuffer cb0 : register(b0)													\n"
	"{																			\n"
	"	float4 inverseViewport;													\n"
	"};																			\n"
	"PSOut main(PSIn input)														\n"
	"{																			\n"
	"	PSOut output;															\n"
	"	output.Color = input.data;												\n"
	"	return output;															\n"
	"}";	

	// mode, type, and size strings
	const char *szModeSet[]	 = {
		"nointerpolation",
		"linear",
		"centroid",
		"noperspective",
		"centroid noperspective",
		"sample",
		"sample noperspective",
	};

	// common replacement sets
	const char *szTypeSet[] = { "float", "uint", "int" };
	const char *szSizeSet[] = { "", "2", "3", "4" };
	// ps replacement set
	const char *szDataSet[] = { "data.xxxx", "data.xyyy", "data.xyzz", "data.xyzw" };
	// vs replacement sets
	const char *szVarSet[]  = { "fdata", "udata", "idata" };
	const char *szSwizSet[] = { ".x", ".xx", ".xxx", ".xxxx" };

	// select current mode and type string
	const char *szMode = szModeSet[m_Mode]; 
	const char *szType = szTypeSet[m_Type];
	const char *szSize = szSizeSet[m_uSize-1];
	const char *szData = szDataSet[m_uSize-1];
	const char *szVar  = szVarSet[m_Type];
	const char *szSwiz = szSwizSet[m_uSize-1];

	// add current mode string to shader
	char szVSFinal[2000];
	char szPSFinal[2000];
	_snprintf( szVSFinal, 1999, szVS, szType, szSize, szVar,  szSwiz );
	_snprintf( szPSFinal, 1999, szPS, szMode, szType, szSize, szType, szSize );
	szVSFinal[1999] = 0;
	szPSFinal[1999] = 0;

	const char* szPSProfile = "ps_4_0";
	const char* szVSProfile = "vs_4_0";
	switch(GetDevice()->GetFeatureLevel())
	{
	default:
		break;
	case D3D_FEATURE_LEVEL_10_1:
		szPSProfile = "ps_4_1";
		szVSProfile = "vs_4_1";
		break;
	case D3D_FEATURE_LEVEL_11_0:
	case D3D_FEATURE_LEVEL_11_1:
		szPSProfile = "ps_5_0";
		szVSProfile = "vs_5_0";
		break;
	}

	// compile and create
	ID3D10Blob *pBuf;
	if ( FAILED( hr = D3DX10CompileFromMemory( szVSFinal, strlen(szVSFinal), NULL, NULL, NULL, "main", szVSProfile, 0, 0, NULL, &pBuf, NULL, NULL )) )
		throw TRException("CompileShaderFromMemory Failed.", hr, RESULT_FAIL);
	if ( FAILED( hr = m_pDevice->CreateVertexShader( (DWORD*) pBuf->GetBufferPointer(), pBuf->GetBufferSize(), NULL, &m_pVS ) ) )
		throw TRException("CreateVertexShader Failed.", hr, RESULT_FAIL);
	SAFE_RELEASE( pBuf );

	if ( FAILED( hr = D3DX10CompileFromMemory( szPSFinal, strlen(szPSFinal), NULL, NULL, NULL, "main", szPSProfile, 0, 0, NULL, &pBuf, NULL, NULL )) )
		throw TRException("CompileShaderFromMemory Failed.", hr, RESULT_FAIL);
	if ( FAILED( hr = m_pDevice->CreatePixelShader( (DWORD*) pBuf->GetBufferPointer(), pBuf->GetBufferSize(), NULL, &m_pPS ) ) )
		throw TRException("CreatePixelShader Failed.", hr, RESULT_FAIL);
	SAFE_RELEASE( pBuf );


	m_pDeviceContext->VSSetShader( m_pVS, NULL, 0 );
	m_pDeviceContext->PSSetShader( m_pPS, NULL, 0 );
}

TEST_RESULT CInterpolator::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	try
	{
		// sanity validatation
		UINT64 occ = 0;
	
		// clear
		FLOAT ClearColor[4] = { 999.f, 999.f, 999.f, 999.f };
		m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, NULL );
		m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
		
		// draw primitives to m_pRTTexture2D
		BeginQuery();
		m_pDeviceContext->DrawIndexed(m_pBuffer->size, 0, 0);
		occ = EndQuery();

		// resolve if necessary, then copy to m_pTexA
		D3D11_TEXTURE2D_DESC rtDesc;
		m_pRTTexture2D->GetDesc(&rtDesc);

		if(rtDesc.SampleDesc.Count > 1)
		{
			ID3D11Texture2D* pResolved = NULL;

			D3D11_TEXTURE2D_DESC resolvedDesc = rtDesc;
			resolvedDesc.SampleDesc.Count   = 1;
			resolvedDesc.SampleDesc.Quality = 0;

			if ( FAILED( hr = m_pDevice->CreateTexture2D( &resolvedDesc, NULL, &pResolved ) ) )
				throw TRException("CreateTexture2D failed for pResolved.", hr, RESULT_FAIL);

			m_pDeviceContext->ResolveSubresource( pResolved, 0, m_pRTTexture2D, 0, m_RTFormat );

			m_pDeviceContext->CopySubresourceRegion( m_pTexA, 0, 0, 0, 0, pResolved, 0, &m_TexBox );

			pResolved->Release();
		}
		else
		{
			m_pDeviceContext->CopySubresourceRegion( m_pTexA, 0, 0, 0, 0, m_pRTTexture2D, 0, &m_TexBox );
		}

		// make sure something was rendered
		if (!occ) 
			throw TRException("Primitive did not render correctly.", RESULT_FAIL);

		if ( FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);
		
		// start texture read
		D3D11_MAPPED_SUBRESOURCE texmap;
		if( FAILED( hr = GetImmediateContext()->Map( m_pTexA, 0, D3D11_MAP_READ, 0, &texmap ) ) )
			throw TRException("m_pRTTexture2DCopy->Map() failed.", hr, RESULT_FAIL);

		// copy first 3 pixels of the first 3 rows
		std::vector<UINT> data(m_uSize*9);
		for (UINT row = 0; row < 3; ++row)
			memcpy(&data[row*3*m_uSize], (BYTE*)texmap.pData + row*texmap.RowPitch, m_uSize*3*sizeof(UINT));  
		
		// end texture read
		GetImmediateContext()->Unmap(m_pTexA,0);

		// verify data based on interpolation mode
		switch( m_Mode )
		{
		case CONSTANT:
			VerifyConstant( data );
			break;
		case LINEAR:
		case LINEAR_CENTROID:
		case LINEAR_NOPERSPECTIVE:
		case LINEAR_NOPERSPECTIVE_CENTROID:
		case LINEAR_SAMPLE:
		case LINEAR_NOPERSPECTIVE_SAMPLE:
			VerifyLinear( data );
			break;

		default:
			assert(false);
		}
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CInterpolator::ExecuteTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}

UINT CInterpolator::GetLeadingVertex(UINT x, UINT y)
{
	// is the pixel to the left of the diagnol of the first triangle?
	bool bPixelLeft;
	// is the corner vertex of the first triangle on the left of the quad?
	bool bCornerLeft;

	for (UINT i = 0; i < 3; ++i)
	{
		// quad diagnol edge either goes between vertices 5,6 or 4,7
		if (((m_pBuffer->data[i] == 5) && (m_pBuffer->data[(i+1)%3] == 6)) ||
			((m_pBuffer->data[i] == 6) && (m_pBuffer->data[(i+1)%3] == 5)))
		{
			bPixelLeft = 3-x > y;
			bCornerLeft = m_pBuffer->data[(i+2)%3] == 4;
			break;
		}
		else if (((m_pBuffer->data[i] == 4) && (m_pBuffer->data[(i+1)%3] == 7)) ||
				((m_pBuffer->data[i] == 7) && (m_pBuffer->data[(i+1)%3] == 4)))
		{
			bPixelLeft = x < y;
			bCornerLeft = m_pBuffer->data[(i+2)%3] == 6;
			break;
		}
	}

	// if the pixel and corner vert are on the same side, the pixel lies in the first triangle
	if (( bPixelLeft && bCornerLeft ) || ( !bPixelLeft && !bCornerLeft ))
		return m_pBuffer->data[0];
	else 
	{
		// determine the first vertex of the second triangle
		if (m_pBuffer->top == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			return m_pBuffer->data[3];
		else 
			return m_pBuffer->data[1];
	}
}

void CInterpolator::VerifyConstant(const std::vector<UINT> &vData)
{
	TEST_RESULT tRes = RESULT_PASS;
	
	UINT uVertex;
	// check all pixels
	for (UINT y = 0; y < 3; ++y)
		for (UINT x = 0; x < 3; ++x)	
		{
			// Find leading vertex
			Vertex &vLead = m_VBData[ GetLeadingVertex(x,y) ];
			for (UINT uComp = 0; uComp < m_uSize; ++uComp)
			{
				UINT uIndex = uComp + x*m_uSize + y*3*m_uSize;
				
				float f = GetFloat(vData, uIndex);
				
				// verify data in the pixel is the same as its leading vertex
				// play the casting game!
				if (m_Type == MODE_FLOAT) {
					if (GetFloat(vData, uIndex) != vLead.fData) tRes = RESULT_FAIL;
				}
				else  if (m_Type == MODE_INT) {
					if (GetInt(vData, uIndex) != vLead.iData) tRes = RESULT_FAIL;
				}
				else  if (m_Type == MODE_UINT) {
					if (GetUint(vData, uIndex) != vLead.uData) tRes = RESULT_FAIL;
				}
			}
		}

	if (tRes != RESULT_PASS)
		throw TRException("[CInterpolator::VerifyConstant()] Data did not match leading vertex data.", RESULT_FAIL);
}

int GetExp(float x)
{
	const UINT uExpMask = 0x7F800000;
	UINT uData = *(UINT*)&x;
	int uUnBiasExp = (uExpMask & uData) >> 23;
	return uUnBiasExp - 127;
}

void CInterpolator::VerifyLinear(const std::vector<UINT> &vData)
{
	TEST_RESULT tRes = RESULT_PASS;
	float fDelta = 0.00001f;

	//
	// ULP bounds
	//

	// get min and max data values
	float fMin, fMax;
	if (GetIBIndex() < 12)
	{
		fMin = min(min(m_VBData[4].fData, m_VBData[5].fData), m_VBData[6].fData);
		fMax = max(max(m_VBData[4].fData, m_VBData[5].fData), m_VBData[6].fData);
	}
	else
	{
		fMin = min(min(m_VBData[4].fData, m_VBData[5].fData), m_VBData[7].fData);
		fMax = max(max(m_VBData[4].fData, m_VBData[5].fData), m_VBData[7].fData);
	}

	// compute ULP bound
	float fMaxExp = (float)GetExp(fMax);
	float fMinExp = (float)GetExp(fMin);
	if (fMin == 0.f) fMinExp = -127;
	float fULPBound = 200.f * (fMaxExp - fMinExp);
	fULPBound = max(fULPBound, 4.0f);
	WriteToLog("MinExp: %f, MaxExp: %f, ULP Bound: %f", fMinExp, fMaxExp, fULPBound);

	//
	// verify corner
	//
	float fExpected = m_VBData[4].fData;

	for (UINT comp = 0; comp < m_uSize; ++comp)
	{
		//test actual vs. expected within ulp bounds
		const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(m_RTFormat);
		float fULPDiff;
		float fActual = GetFloat(vData, comp);

		// compare 
		if(	!MinMaxRange::TestFloatsSame(fActual, fExpected, pTexFormatInfo, comp, fULPBound, &fULPDiff) )
		{
			WriteVBToLog();
			WriteToLog("Pixel (0, 0)[%d]: %f", comp, fActual);
			WriteToLog("Expected: %f", fExpected);
			WriteToLog("ULP difference: %f", fULPDiff);
			throw TRException("[CInterpolator::VerifyLinear()] Value on corner vertex did not match value store in vertex.", RESULT_FAIL);
		}
	}

	float *data = (float*)&vData[0];
	// verify top row
	bool bDirection = m_VBData[5].fData > m_VBData[4].fData;
	for (UINT x = 1; x < 3; ++x)
		for (UINT comp = 0; comp < m_uSize; ++comp)
		{	
			// get values of this pixel and prev pixel in row
			float prev = GetFloat(vData, (x-1)*m_uSize + comp);
			float curr = GetFloat(vData, x*m_uSize + comp);

			// verify no two adjacent values are equal
			if (curr == prev)
			{
				WriteVBToLog();
				WriteToLog("Pixel (0, %d)[%d]: %f", x, comp, curr);
				WriteToLog("Pixel (0, %d)[%d]: %f", x-1, comp, prev);
				throw TRException("[CInterpolator::VerifyLinear()] Adjacent pixels should not be equal.", RESULT_FAIL);
			}

			// verify the values increase or decrease along edge
			bool bResult = curr > prev;
			if (bDirection != bResult)
			{
				WriteVBToLog();
				WriteToLog("Pixel (0, %d)[%d]: %f", x, comp, curr);
				WriteToLog("Pixel (0, %d)[%d]: %f", x-1, comp, prev);
				throw TRException("[CInterpolator::VerifyLinear()] Pixels along the edge should follow the gradient of the edge vertices.", RESULT_FAIL);
			}
		}
	
	// verify left column
	bDirection = m_VBData[6].fData > m_VBData[4].fData;
	for (UINT y = 1; y < 3; ++y)
		for (UINT comp = 0; comp < m_uSize; ++comp)
		{	
			// get values of this pixel and prev pixel in column
			float prev = GetFloat(vData, (y-1)*m_uSize*3 + comp);
			float curr = GetFloat(vData, y*m_uSize*3 + comp);

			// verify no two adjacent values are equal
			if (curr == prev)
			{
				WriteVBToLog();
				WriteToLog("Pixel (0, %d)[%d]: %f", y, comp, curr);
				WriteToLog("Pixel (0, %d)[%d]: %f", y-1, comp, prev);
				throw TRException("[CInterpolator::VerifyLinear()] Adjacent pixels should not be equal.", RESULT_FAIL);
			}

			// verify the values increase or decrease along edge
			bool bResult = curr > prev;
			if (bDirection != bResult)
			{
				WriteVBToLog();
				WriteToLog("Pixel (0, %d)[%d]: %f", y, comp, curr);
				WriteToLog("Pixel (0, %d)[%d]: %f", y-1, comp, prev);
				throw TRException("[CInterpolator::VerifyLinear()] Pixels along the edge should follow the gradient of the edge vertices.", RESULT_FAIL);
			}
		}


	// find max and min data
	float minimum = min(min(min(m_VBData[4].fData, m_VBData[5].fData), m_VBData[6].fData), m_VBData[7].fData);
	float maximum = max(max(max(m_VBData[4].fData, m_VBData[5].fData), m_VBData[6].fData), m_VBData[7].fData);

	// verify all pixels are in the quad data bounds
	for (UINT x = 0; x < 3; ++x)
		for (UINT y = 0; y < 3; ++y)
			for (UINT comp = 0; comp < m_uSize; ++comp)
			{
				float data = GetFloat(vData, comp + x*m_uSize + y*m_uSize*3);
				if ((data + fDelta) < minimum)
				{	
					WriteVBToLog();
					WriteToLog("VB Min: %f, VB Max: %f\n Pixel (%d, %d)[%d]: %f", minimum, maximum, x, y, comp, data); 
					throw TRException("[CInterpolator::VerifyLinear()] Pixels should fall within the vertices min and max.", RESULT_FAIL);
				}
			}
}

void CInterpolator::WriteVBToLog()
{
	if (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST == m_pBuffer->top)
		WriteToLog("TRIANGLELIST");
	else if (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP == m_pBuffer->top)
		WriteToLog("TRIANGLESTRIP");	

	for (UINT i = 0; i < m_pBuffer->size; ++i)
	{
		float x = m_VBData[m_pBuffer->data[i]].pos[0];
		float y = m_VBData[m_pBuffer->data[i]].pos[1];
		float data;
		if (m_Type == MODE_FLOAT) data =  m_VBData[m_pBuffer->data[i]].fData;
		else if (m_Type == MODE_INT) data = (float) m_VBData[m_pBuffer->data[i]].iData;
		else if (m_Type == MODE_UINT) data = (float) m_VBData[m_pBuffer->data[i]].uData;
		
		WriteToLog("[VB %d] x:%f, y:%f, data:%f", i, x, y, data);
	}
}

void CInterpolator::CleanupTestCase()
{
	CleanupRasterizerTestCase();
	CleanupD3D();
}

float CInterpolator::GetFloat(const std::vector<UINT> &data, UINT index)
{
	return *((float*)&data[0] + index);
}

UINT  CInterpolator::GetUint(const std::vector<UINT> &data, UINT index)
{
	return *((UINT*)&data[0] + index);
}

int   CInterpolator::GetInt(const std::vector<UINT> &data, UINT index)
{
	return data[index];
}


//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
tstring __stdcall ToString( D3D_INTERPOLATOR mode )
{
	if (mode == CONSTANT)								return tstring( _T( "CONSTANT" ) );
	else if (mode == LINEAR)							return tstring( _T( "LINEAR" ) );
	else if (mode == LINEAR_CENTROID)					return tstring( _T( "LINEAR_CENTROID" ) );
	else if (mode == LINEAR_NOPERSPECTIVE)				return tstring( _T( "LINEAR_NOPERSPECTIVE" ) );
	else if (mode == LINEAR_NOPERSPECTIVE_CENTROID)	    return tstring( _T( "LINEAR_NOPERSPECTIVE_CENTROID" ) );
	else if (mode == LINEAR_SAMPLE)					    return tstring( _T( "LINEAR_SAMPLE" ) );
	else if (mode == LINEAR_NOPERSPECTIVE_SAMPLE)		return tstring( _T( "LINEAR_NOPERSPECTIVE_SAMPLE" ) );
	else										        return tstring( _T( "Unknown" ) );
}

bool __stdcall FromString( D3D_INTERPOLATOR *pMode, const tstring &str )
{
	if( tcistring( str ) == _T( "CONSTANT" ) )
	{
		*pMode = CONSTANT;
		return true;
	}
	else if( tcistring( str ) == _T( "LINEAR" ) )
	{
		*pMode = LINEAR;
		return true;
	}
	else if( tcistring( str ) == _T( "LINEAR_CENTROID" ) )
	{
		*pMode = LINEAR_CENTROID;
		return true;
	}
	else if( tcistring( str ) == _T( "LINEAR_NOPERSPECTIVE" ) )
	{
		*pMode = LINEAR_NOPERSPECTIVE;
		return true;
	}
	else if( tcistring( str ) == _T( "LINEAR_NOPERSPECTIVE_CENTROID" ) )
	{
		*pMode = LINEAR_NOPERSPECTIVE_CENTROID;
		return true;
	}
	else if( tcistring( str ) == _T( "LINEAR_SAMPLE" ) )
	{
		*pMode = LINEAR_SAMPLE;
		return true;
	}
	else if( tcistring( str ) == _T( "LINEAR_NOPERSPECTIVE_SAMPLE" ) )
	{
		*pMode = LINEAR_NOPERSPECTIVE_SAMPLE;
		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
static const DXGI_FORMAT SampleFreqFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

static const char szSampleFreqVS[] = 
	"struct VSIn																\n"
	"{																			\n"
	"	float4 pos  : pos;														\n"
	"	float4 clr  : color;													\n"
	"};																			\n"
	"struct VSOut																\n"
	"{																			\n"
	"	float4 pos : SV_Position;												\n"
	"	nointerpolation        float4 clr0 : data0;    							\n"
	"	linear                 float4 clr1 : data1;    							\n"
	"	centroid               float4 clr2 : data2;    							\n"
	"	linear noperspective   float4 clr3 : data3;    							\n"
	"	centroid noperspective float4 clr4 : data4;    							\n"
	"	sample                 float4 clr5 : data5;    							\n"
	"	sample noperspective   float4 clr6 : data6;    							\n"
	"};																			\n"
	"VSOut main(VSIn input)														\n"
	"{																			\n"
	"	VSOut output;															\n"
	"	output.pos = input.pos;	                 								\n"
	"	output.clr0 = input.clr;												\n"
	"	output.clr1 = input.clr;												\n"
	"	output.clr2 = input.clr;												\n"
	"	output.clr3 = input.clr;												\n"
	"	output.clr4 = input.clr;												\n"
	"	output.clr5 = input.clr;												\n"
	"	output.clr6 = input.clr;												\n"
	"	return output;															\n"
	"}";

static const char szSampleFreqPS[] = 
	"struct PSIn																\n"
	"{																			\n"
	"	float4 pos : SV_Position;												\n"
	"	nointerpolation        float4 clr0 : data0;    							\n"
	"	linear                 float4 clr1 : data1;    							\n"
	"	centroid               float4 clr2 : data2;    							\n"
	"	linear noperspective   float4 clr3 : data3;    							\n"
	"	centroid noperspective float4 clr4 : data4;    							\n"
	"	sample                 float4 clr5 : data5;    							\n"
	"	sample noperspective   float4 clr6 : data6;    							\n"
	"};																			\n"
	"cbuffer cb0 : register(b0)													\n"
	"{																			\n"
	"	uint4 selector;	         												\n"
	"};																			\n"
	"float4 main(PSIn input) : SV_TARGET0   									\n"
	"{																			\n"
	"	float4 output = (float4)0;              								\n"
	"	switch(selector.x)       												\n"
	"   {                                                                       \n"
	"   case 0: output = input.clr0; break;                                     \n"
	"   case 1: output = input.clr1; break;                                     \n"
	"   case 2: output = input.clr2; break;                                     \n"
	"   case 3: output = input.clr3; break;                                     \n"
	"   case 4: output = input.clr4; break;                                     \n"
	"   case 5: output = input.clr5; break;                                     \n"
	"   case 6: output = input.clr6; break;                                     \n"
	"   }                                                                       \n"
	"	return output;															\n"
	"}";	

// The standard sample patterns are only defined for these sample counts
static const UINT StandardSampleCounts[] = { 1, 2, 4, 8, 16 };

struct SampleFreqVertex
{
	float pos[4];
	DWORD color;
};

CSampleFreq::CSampleFreq() : 
 m_Mode(CONSTANT),
 m_uRTSampleCount(1),
 m_pRenderTarget(NULL),
 m_pRTV(NULL),
 m_pCB(NULL),
 m_pVS(NULL),
 m_pPS(NULL),
 m_pIL(NULL),
 m_pVB(NULL),
 m_pRastState(NULL)
{
}

void CSampleFreq::InitTestParameters()
{
	//	parameters
	CTestCaseParameter< D3D_INTERPOLATOR > *pModeParam = AddParameter( _T( "Mode" ), &m_Mode );
	CTestCaseParameter< UINT > *pSampleCountParam = AddParameter( _T("SampleCount"), &m_uRTSampleCount);

	// fill mode factors
	testfactor::RFactor rfInterpolator = AddParameterValueSet< D3D_INTERPOLATOR > ( pModeParam, 
		new CTableValueSet< D3D_INTERPOLATOR >( FloatModeSet, sizeof(D3D_INTERPOLATOR), ARRAY_SIZE(FloatModeSet) ) ); 

	testfactor::RFactor rfSampleCount = AddParameterValueSet< UINT > (pSampleCountParam, 
		new CTableValueSet< UINT >( StandardSampleCounts, sizeof(UINT), ARRAY_SIZE(StandardSampleCounts) ) );

	// set root
	SetRootTestFactor( rfInterpolator * 
					   rfSampleCount );
}

TEST_RESULT CSampleFreq::Setup()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = S_OK;

	ID3D10Blob* pVSBuff    = NULL;
	ID3D10Blob* pVSErrMsgs = NULL;

	ID3D10Blob* pPSBuff    = NULL;
	ID3D10Blob* pPSErrMsgs = NULL;

	D3D11_BUFFER_DESC cbDesc;
	D3D11_BUFFER_DESC vbDesc;
	D3D11_RASTERIZER_DESC rastDesc;

	// Check caps
	if( GetDevice()->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_1 )
	{
		WriteToLog( _T("Sample-frequency tests are only supported on feature level > 10.0\n") );
		tRes = RESULT_SKIP;
	}

	const char* szVSProfile = "vs_4_1";
	const char* szPSProfile = "ps_4_1";
	switch(GetDevice()->GetFeatureLevel())
	{
	default:
		break;
	case D3D_FEATURE_LEVEL_11_0:
	case D3D_FEATURE_LEVEL_11_1:
		szVSProfile = "vs_5_0";
		szPSProfile = "ps_5_0";
		break;
	}

	if( RESULT_PASS != tRes )
		goto done;

	// Create PS constant buffer
	cbDesc = CD3D11_BUFFER_DESC(sizeof(UINT) * 4, D3D11_BIND_CONSTANT_BUFFER);
	if( FAILED( hr = GetDevice()->CreateBuffer( &cbDesc, NULL, &m_pCB ) ) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "CreateBuffer failed with %s\n" ), D3DHResultToString(hr).c_str() );
		goto done;
	}
	
	// Create shaders
	if ( FAILED( hr = D3DX10CompileFromMemory( szSampleFreqVS, strlen(szSampleFreqVS), NULL, NULL, NULL, "main", szVSProfile, 0, 0, NULL, &pVSBuff, &pVSErrMsgs, NULL )) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "D3DX10CompileFromMemory failed with %s\n" ), D3DHResultToString(hr).c_str() );

		if ( pVSErrMsgs )
		{
			OutputDebugStringA((const char*)pVSErrMsgs->GetBufferPointer());
			OutputDebugStringA("\n");
		}

		goto done;
	}

	if ( FAILED( hr = GetDevice()->CreateVertexShader( (DWORD*)pVSBuff->GetBufferPointer(), pVSBuff->GetBufferSize(), NULL, &m_pVS ) ) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "CreateVertexShader failed with %s\n" ), D3DHResultToString(hr).c_str() );
		goto done;
	}

	if ( FAILED( hr = D3DX10CompileFromMemory( szSampleFreqPS, strlen(szSampleFreqPS), NULL, NULL, NULL, "main", szPSProfile, 0, 0, NULL, &pPSBuff, &pPSErrMsgs, NULL )) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "D3DX10CompileFromMemory failed with %s\n" ), D3DHResultToString(hr).c_str() );

		if ( pPSErrMsgs )
		{
			OutputDebugStringA((const char*)pPSErrMsgs->GetBufferPointer());
			OutputDebugStringA("\n");
		}

		goto done;
	}

	if ( FAILED( hr = GetDevice()->CreatePixelShader( (DWORD*)pPSBuff->GetBufferPointer(), pPSBuff->GetBufferSize(), NULL, &m_pPS ) ) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "CreatePixelShader failed with %s\n" ), D3DHResultToString(hr).c_str() );
		goto done;
	}

	// Create input layout
	const D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "pos",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,                             D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "color", 0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if(	FAILED(	hr = GetDevice()->CreateInputLayout( inputLayout, ARRAY_SIZE(inputLayout), pVSBuff->GetBufferPointer(), pVSBuff->GetBufferSize(), &m_pIL ) ) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "CreateInputLayout failed with %s\n" ), D3DHResultToString(hr).c_str() );
		goto done;
	}

	// Create vertex buffer
	vbDesc = CD3D11_BUFFER_DESC(sizeof(SampleFreqVertex) * 4, D3D11_BIND_VERTEX_BUFFER);
	if( FAILED( hr = GetDevice()->CreateBuffer( &vbDesc, NULL, &m_pVB ) ) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "CreateBuffer failed with %s\n" ), D3DHResultToString(hr).c_str() );
		goto done;
	}

	// Create rasterizer state
	rastDesc = CD3D11_RASTERIZER_DESC(D3D11_FILL_SOLID,
									  D3D11_CULL_BACK,
									  FALSE,
									  D3D11_DEFAULT_DEPTH_BIAS,
									  D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
									  D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
									  TRUE,
									  FALSE,
									  TRUE, // MultisampleEnable,
									  FALSE);

	if( FAILED( hr = GetDevice()->CreateRasterizerState( &rastDesc, &m_pRastState ) ) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "CreateRasterizerState failed with %s\n" ), D3DHResultToString(hr).c_str() );
		goto done;
	}

done:
	SAFE_RELEASE(pVSBuff);
	SAFE_RELEASE(pVSErrMsgs);
	SAFE_RELEASE(pPSBuff);
	SAFE_RELEASE(pPSErrMsgs);

	return tRes;
}

void CSampleFreq::Cleanup()
{
	SAFE_RELEASE( m_pRastState );
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pIL );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pCB );
}

TEST_RESULT CSampleFreq::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_SKIP;
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC rtDesc;

	// Check caps
	UINT NumQualityLevels = 0;
	if(SUCCEEDED(GetDevice()->CheckMultisampleQualityLevels(SampleFreqFormat, m_uRTSampleCount, &NumQualityLevels)))
	{
		if(NumQualityLevels > 0)
			tRes = RESULT_PASS;
	}

	if( RESULT_PASS != tRes )
		goto done;

	// Create render target & view
	rtDesc = CD3D11_TEXTURE2D_DESC(SampleFreqFormat, 
								   1, 
								   1, 
								   1, 
								   1, 
								   D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, 
								   D3D11_USAGE_DEFAULT,
								   0, 
								   m_uRTSampleCount, 
								   D3D11_STANDARD_MULTISAMPLE_PATTERN);

	if( FAILED( hr = GetDevice()->CreateTexture2D( &rtDesc, NULL, &m_pRenderTarget ) ) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "CreateTexture2D failed with %s\n" ), D3DHResultToString(hr).c_str() );
		goto done;
	}

	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRenderTarget, NULL, &m_pRTV ) ) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T( "CreateRenderTargetView failed with %s\n" ), D3DHResultToString(hr).c_str() );
		goto done;
	}

done:
	return tRes;
}

void CSampleFreq::CleanupTestCase()
{
	SAFE_RELEASE( m_pRTV );
	SAFE_RELEASE( m_pRenderTarget );
}

TEST_RESULT CSampleFreq::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;
   
	D3D11_VIEWPORT vp = { 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
	std::vector<D3DXVECTOR4> sampleColors;

	// set viewport
	GetEffectiveContext()->RSSetViewports( 1, &vp );
	
	// bind render target
	GetEffectiveContext()->OMSetRenderTargets(1, &m_pRTV, NULL);
	
	// update & bind constant buffer (that determines which output to use)
	UINT CBData[4] = {0,0,0,0};
	switch(m_Mode)
	{
	case CONSTANT: CBData[0] = 0; break;
	case LINEAR: CBData[0] = 1; break;
	case LINEAR_CENTROID: CBData[0] = 2; break;
	case LINEAR_NOPERSPECTIVE: CBData[0] = 3; break;
	case LINEAR_NOPERSPECTIVE_CENTROID: CBData[0] = 4; break;
	case LINEAR_SAMPLE: CBData[0] = 5; break;
	case LINEAR_NOPERSPECTIVE_SAMPLE: CBData[0] = 6; break;
	default:
		assert(false);
	}

	GetEffectiveContext()->UpdateSubresource(m_pCB, 0, NULL, CBData, 0, 0);
	GetEffectiveContext()->PSSetConstantBuffers(0, 1, &m_pCB);
	
	// bind shaders
	GetEffectiveContext()->VSSetShader(m_pVS, NULL, 0);
	GetEffectiveContext()->PSSetShader(m_pPS, NULL, 0);

	// bind input layout
	GetEffectiveContext()->IASetInputLayout(m_pIL);

	// set input primitive topology
	GetEffectiveContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Bind rasterizer state
	GetEffectiveContext()->RSSetState(m_pRastState);

	// updated & bind vetex buffer
	SampleFreqVertex Vertices[4];

	// Red goes from 0 on the left to 1 on the right
	// Green goes from 0 on the top to 1 on the bottom
	// Blue is always 0
	// Alpha is always 1
	Vertices[0].pos[0] = -1.0f; Vertices[0].pos[1] = 1.0f;  Vertices[0].pos[2] = 0.5f; Vertices[0].pos[3] = 1.0f; Vertices[0].color = 0xFF000000;
	Vertices[1].pos[0] = 1.0f;  Vertices[1].pos[1] = 1.0f;  Vertices[1].pos[2] = 0.5f; Vertices[1].pos[3] = 1.0f; Vertices[1].color = 0xFF0000FF;
	Vertices[2].pos[0] = -1.0f; Vertices[2].pos[1] = -1.0f; Vertices[2].pos[2] = 0.5f; Vertices[2].pos[3] = 1.0f; Vertices[2].color = 0xFF00FF00;
	Vertices[3].pos[0] = 1.0f;  Vertices[3].pos[1] = -1.0f; Vertices[3].pos[2] = 0.5f; Vertices[3].pos[3] = 1.0f; Vertices[3].color = 0xFF00FFFF;

	GetEffectiveContext()->UpdateSubresource(m_pVB, 0, NULL, Vertices, 0, 0);

	UINT VBStride = sizeof(SampleFreqVertex);
	UINT VBOffset = 0;
	
	GetEffectiveContext()->IASetVertexBuffers(0, 1, &m_pVB, &VBStride, &VBOffset);
	
	// draw
	GetEffectiveContext()->Draw(4, 0);

	// read all samples from the render target
	sampleColors.resize(m_uRTSampleCount);

	if ( FAILED( hr = ExecuteEffectiveContext() ) )
		throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

	if( !MSHelper::Read(GetFramework(), m_pRenderTarget, (float*)&(sampleColors[0]), SampleFreqFormat) )
	{
		WriteToLog( _T( "MSHelper::Read failed\n") );
		tRes = RESULT_FAIL;
		goto done;
	}

	// No matter what interpolation mode is used
	// alpha must always be 1.0
	// and blue must alpha be 0.0

	for(UINT i = 0; i < m_uRTSampleCount; i++)
	{
		const D3DXVECTOR4& sampleColor = sampleColors[i];

		if(sampleColor.w != 1.0f)
		{
			WriteToLog( _T( "Sample %i alpha value was not 1.0\n"), i );
			tRes = RESULT_FAIL;
			goto done;
		}

		if(sampleColor.z != 0.0f)
		{
			WriteToLog( _T( "Sample %i blue value was not 0.0\n"), i );
			tRes = RESULT_FAIL;
			goto done;
		}
	}

	switch(m_Mode)
	{
	case CONSTANT:
	case LINEAR_CENTROID:
	case LINEAR_NOPERSPECTIVE_CENTROID:
		break;

	case LINEAR:
	case LINEAR_NOPERSPECTIVE:
		// All samples must have the same value
		for(UINT i = 1; i < m_uRTSampleCount; i++)
		{
			const D3DXVECTOR4& sampleColor = sampleColors[i];

			if(sampleColor != sampleColors[0])
			{
				WriteToLog( _T( "Sample %i did not match sample 0, expected: (%f, %f, %f, %f), found:  (%f, %f, %f, %f)\n"),  
							i,    
							sampleColors[0].x, sampleColors[0].y, sampleColors[0].z, sampleColors[0].w,
							sampleColor.x, sampleColor.y, sampleColor.z, sampleColor.w);
				tRes = RESULT_FAIL;
				goto done;
			}
		}
		break;

	case LINEAR_SAMPLE:
	case LINEAR_NOPERSPECTIVE_SAMPLE:
			// Red and green channels must be different for all samples
			for(UINT i = 0; i < m_uRTSampleCount; i++)
			{
				for(UINT j = 0; j < m_uRTSampleCount; j++)
				{
					if(i != j)
					{
						if((sampleColors[i].x == sampleColors[j].x) && 
						   (sampleColors[i].y == sampleColors[j].y))
						{
							WriteToLog( _T( "Sample %i red and green channels matched sample %i when they should not have, sample%i.rg = (%f,%f), sample%i.rg = (%f,%f)\n"),  
										i, j,
										i,
										sampleColors[i].x, sampleColors[i].y,
										j,
										sampleColors[j].x, sampleColors[j].y);
							tRes = RESULT_FAIL;
							goto done;
						}
					}
				}
			}
			break;
	default:
		assert(false);
	}

done:
	return tRes;
}
