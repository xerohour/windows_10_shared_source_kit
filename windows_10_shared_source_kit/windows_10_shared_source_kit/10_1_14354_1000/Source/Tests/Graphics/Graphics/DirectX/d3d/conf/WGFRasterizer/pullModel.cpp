//////////////////////////////////////////////////////////////////////////
//  pullModel.cpp
//  created:	2009/04/21
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////


#include <WGFTestCore.h>
#include <stdlib.h>

#include "rasterizer.h"
#include "pullModel.h"

#include "MinMaxRange.h"

#include "d3dmshelper.h"
#include "FormatHelper.h"
#include "d3d11test.h"

BEGIN_NAMED_VALUES(PULL_EVAL_FUNCS)
	NAMED_VALUE("EvaluateAttributeSnapped", EVAL_SNAPPED)
	NAMED_VALUE("EvaluateAttributeAtSample", EVAL_SAMPLE)
	NAMED_VALUE("EvaluateAttributeAtCentroid", EVAL_CENTROID)
	NAMED_VALUE("UseAllEvalFuncs", USE_ALL_EVALS)
END_NAMED_VALUES(PULL_EVAL_FUNCS)

// Interpolation modes that can be used by pull-model
D3D_INTERPOLATOR g_pullInterModes[] = {
	LINEAR, 
	LINEAR_CENTROID,
	LINEAR_NOPERSPECTIVE,
	LINEAR_NOPERSPECTIVE_CENTROID,
	LINEAR_SAMPLE,
	LINEAR_NOPERSPECTIVE_SAMPLE,
};

// RT formats for 4 interpolated values
DXGI_FORMAT g_Value4Set[] = {
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	
};


// RT formats for 1 interpolated values
DXGI_FORMAT g_Value1Set[] = {
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
};

// Only standard multisample pattern will be tested for pull_model, which has quality level
// 0 or D3D11_STANDARD_MULTISAMPLE_PATTERN if the format and samplecount combination is supported 
UINT g_SampleCounts[] = 
{
	1,
	2,
	4,
	8,
	16
};


// 
// Defined in interpolator.cpp
//
tstring __stdcall ToString( D3D_INTERPOLATOR mode );
bool __stdcall FromString( D3D_INTERPOLATOR *pMode, const tstring &str );



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CPullModel : public CInterpolator
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------------
CPullModel::CPullModel()
{
	m_numInterValues = 0;

	// members used from CRasterizerTest
	m_pRTTexture2D = NULL;
	m_pDepthStateDefault = NULL;
	m_pDepthStateDisabled = NULL;
	m_pRastState = NULL;
	m_pRTView = NULL;
	m_uRTSampleCount = 0;
	m_RTFormat = DXGI_FORMAT_UNKNOWN;
	m_pVertexLayout = NULL;
	m_pVS = NULL;
	m_pVB = NULL;
	m_pPS = NULL;

}

//----------------------------------------------------------------------------------------------------------
void CPullModel::InitTestParameters()
{
	// interpolation mode
	CTestCaseParameter< D3D_INTERPOLATOR > *pModeParam = AddParameter( _T( "Mode" ), &m_Mode );
	testfactor::RFactor rfInterpolatorModes = AddParameterValueSet< D3D_INTERPOLATOR > ( pModeParam, 
		new CTableValueSet< D3D_INTERPOLATOR >( g_pullInterModes, sizeof(D3D_INTERPOLATOR), ARRAY_SIZE(g_pullInterModes) ) ); 
	testfactor::RFactor rfInterpolatorModeDef = AddParameterValue< D3D_INTERPOLATOR > ( pModeParam, LINEAR_NOPERSPECTIVE ); 


	// attibute evaluation intrinsics
	CTestCaseParameter< PULL_EVAL_FUNCS > *pEvalFuncParam = AddParameter( _T( "EvalFunc" ), &m_evalFunc );
	testfactor::RFactor rfEvalFuncSnap = AddParameterValue< PULL_EVAL_FUNCS >(pEvalFuncParam, EVAL_SNAPPED);
	testfactor::RFactor rfEvalFuncSamp = AddParameterValue< PULL_EVAL_FUNCS >(pEvalFuncParam, EVAL_SAMPLE);
	testfactor::RFactor rfEvalFuncCent = AddParameterValue< PULL_EVAL_FUNCS >(pEvalFuncParam, EVAL_CENTROID);
	testfactor::RFactor rfEvalFuncComb = AddParameterValue< PULL_EVAL_FUNCS >(pEvalFuncParam, USE_ALL_EVALS);

	// Number of interpolated values by pull-model
	testfactor::RFactor rfNumValues1 = AddParameter<UINT>(_T("NumOfInterpolatedValues"), &m_numInterValues, 1);
	testfactor::RFactor rfNumValues4 = AddParameterValue<UINT>(_T("NumOfInterpolatedValues"), 4);

	// RT formats
	CTestCaseParameter< DXGI_FORMAT > *pFormatParam = AddParameter( _T( "RTFormat" ), &m_RTFormat );
	testfactor::RFactor rfRTFormat1 = AddParameterValueSet(pFormatParam, 
		new CTableValueSet<DXGI_FORMAT>(g_Value1Set, sizeof(DXGI_FORMAT), ARRAY_SIZE(g_Value1Set)) );
	testfactor::RFactor rfRTFormat4 = AddParameterValueSet(pFormatParam, 
		new CTableValueSet<DXGI_FORMAT>(g_Value4Set, sizeof(DXGI_FORMAT), ARRAY_SIZE(g_Value4Set)) );

	// sample count
	CTestCaseParameter< UINT > *pSampleCountParam = AddParameter( _T( "RTSampleCount" ), &m_uRTSampleCount );
	testfactor::RFactor rfSampleCount1 = AddParameterValue<UINT>(pSampleCountParam, 1);
	testfactor::RFactor rfSampleCount2 = AddParameterValue<UINT>(pSampleCountParam, 2);
	testfactor::RFactor rfSampleCount4 = AddParameterValue<UINT>(pSampleCountParam, 4);
	testfactor::RFactor rfSampleCount8 = AddParameterValue<UINT>(pSampleCountParam, 8);
	testfactor::RFactor rfSampleCount16 = AddParameterValue<UINT>(pSampleCountParam, 16);

	// Forced sample count (part of the rasterizer desc)
	// With a forced sample count/pattern selected at the rasterizer (ForcedSampleCount > 0), 
	// pixels are candidates for shader invocation based on the selected sample pattern, independent of the RTV ("output") sample count
	CTestCaseParameter< UINT > *pForcedSampleCountParam = AddParameter( _T( "ForcedSampleCount" ), &m_uForcedSampleCount );
	testfactor::RFactor rfForcedSampleCount0 = AddParameterValue<UINT>(pForcedSampleCountParam, 0);	// 0 means the feature is off and rendering is 'normal'
	testfactor::RFactor rfForcedSampleCount1 = AddParameterValue<UINT>(pForcedSampleCountParam, 1);
	testfactor::RFactor rfForcedSampleCount4 = AddParameterValue<UINT>(pForcedSampleCountParam, 4);
	testfactor::RFactor rfForcedSampleCount8 = AddParameterValue<UINT>(pForcedSampleCountParam, 8);
	testfactor::RFactor rfForcedSampleCount16 = AddParameterValue<UINT>(pForcedSampleCountParam, 16);

	// Sample Index
	CTestCaseParameter< UINT > *pSampleIndexParam = AddParameter( _T( "SampleIndex" ), &m_SampleIndex );
	testfactor::RFactor rfSampleIndex1 = AddParameterValue<UINT>(pSampleIndexParam, 0);
	testfactor::RFactor rfSampleIndex2 = AddParameterValueSet<UINT>(pSampleIndexParam, new CRangeValueSet<UINT>(0, 1, 1));
	testfactor::RFactor rfSampleIndex4 = AddParameterValueSet<UINT>(pSampleIndexParam, new CRangeValueSet<UINT>(0, 3, 1));
	testfactor::RFactor rfSampleIndex8 = AddParameterValueSet<UINT>(pSampleIndexParam, new CRangeValueSet<UINT>(0, 7, 1));
	testfactor::RFactor rfSampleIndex16 = AddParameterValueSet<UINT>(pSampleIndexParam, new CRangeValueSet<UINT>(0, 15, 1));
	testfactor::RFactor rfSampleIndexDef = AddParameterValue<UINT>(pSampleIndexParam, 3);

	// sample position
	CUserValueSet< int > *pSamplePosValues = new CUserValueSet< int >;
	pSamplePosValues->AddValue(-8);
	pSamplePosValues->AddValue(-7);
	pSamplePosValues->AddValue(-2);
	pSamplePosValues->AddValue(0);
	pSamplePosValues->AddValue(4);
	pSamplePosValues->AddValue(5);
	pSamplePosValues->AddValue(7);
	CUserValueSet< int > *pSamplePosValues2 = new CUserValueSet< int >;
	pSamplePosValues2->AddValue(7);
	pSamplePosValues2->AddValue(-8);
	pSamplePosValues2->AddValue(-7);
	pSamplePosValues2->AddValue(-2);
	pSamplePosValues2->AddValue(0);
	pSamplePosValues2->AddValue(4);
	pSamplePosValues2->AddValue(5);

	testfactor::RFactor rfSamplePosX = AddParameter<int>(_T("SamplePositionX"), &(m_SamplePos.pos_x), pSamplePosValues);
	testfactor::RFactor rfSamplePosX2 = AddParameterValueSet<int>(_T("SamplePositionX"), pSamplePosValues);
	testfactor::RFactor rfSamplePosXDef = AddParameterValue<int>(_T("SamplePositionX"), -7);

	testfactor::RFactor rfSamplePosY = AddParameter<int>(_T("SamplePositionY"), &(m_SamplePos.pos_y), pSamplePosValues);
	// shuffled position values for posY
	testfactor::RFactor rfSamplePosY2 = AddParameterValueSet<int>(_T("SamplePositionY"), pSamplePosValues2);
	testfactor::RFactor rfSamplePosYDef = AddParameterValue<int>(_T("SamplePositionY"), 1);

	// combine some important factors
	testfactor::RFactor rfForcedSampleCounts = rfForcedSampleCount0 + rfForcedSampleCount1 + rfForcedSampleCount4 + rfForcedSampleCount8 + rfForcedSampleCount16;
	testfactor::RFactor rfSampleCounts = (rfSampleCount1 * rfForcedSampleCounts) + ((rfSampleCount2 + rfSampleCount4 + rfSampleCount8 + rfSampleCount16) * rfForcedSampleCount0);	// If ForcedSampleCount is greater than 1, any RTVs that are bound while rendering must only have a single sample, otherwise rendering behavior is undefined
	testfactor::RFactor rfSampleIndexAndForcedSampleCount =
		rfSampleIndex1 * rfForcedSampleCount0 + 
		rfSampleIndex1 * rfForcedSampleCount1 + 
		rfSampleIndex4 * rfForcedSampleCount4 + 
		rfSampleIndex8 * rfForcedSampleCount8 + 
		rfSampleIndex16 * rfForcedSampleCount16;

	// building root test factor
	testfactor::RFactor rfFuncSnap =  rfEvalFuncSnap * ( rfSamplePosX % rfSamplePosY + rfSamplePosX2 % rfSamplePosY2 )
		* rfSampleCounts;
	testfactor::RFactor rfFuncCent =  rfEvalFuncCent 
		* rfSampleCounts;
	testfactor::RFactor rfFuncSamp =  rfEvalFuncSamp * 
		( rfSampleCount1 * rfSampleIndexAndForcedSampleCount + rfSampleCount2 * rfSampleIndex2 * rfForcedSampleCount0 + rfSampleCount4 * rfSampleIndex4 * rfForcedSampleCount0
		  + rfSampleCount8 * rfSampleIndex8 * rfForcedSampleCount0 + rfSampleCount16 * rfSampleIndex16 * rfForcedSampleCount0);

	testfactor::RFactor rfRoot = 
		// single full-model func in the shader
		rfInterpolatorModes * ( rfNumValues1 * rfRTFormat1 + rfNumValues4 * rfRTFormat4) 
		* ( rfFuncSnap + rfFuncSamp + rfFuncCent )
		// mixed ful-model funcs in the shader
		+ rfInterpolatorModeDef * rfNumValues4 * rfRTFormat4 * rfEvalFuncComb *
		(	(rfSamplePosXDef % rfSamplePosYDef % (rfSampleCount4 * rfForcedSampleCount0) % rfSampleIndexDef) +
			(rfSamplePosXDef % rfSamplePosYDef % (rfSampleCount1 * rfForcedSampleCount4) % rfSampleIndexDef) );
		 
	SetRootTestFactor(rfRoot);

}

//----------------------------------------------------------------------------------------------------------
TEST_RESULT CPullModel::Setup() 
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	m_pDevice = GetDevice1();
	m_pDeviceContext = GetEffectiveContext1();

	if( m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 )
	{
		WriteToLog( _T("Skip Pull Model test group on feature levels < 11_0\n") );
		return RESULT_SKIP;
	}
	
	tr = SetupSampleTables();
	if (tr != RESULT_PASS)
	{
		WriteToLog( _T("CPullModel::SetupSampleTables failed") );
		return RESULT_FAIL;
	}

	// setup viewport: a 2x2-pixel area
	m_vp.Height = (float)2;
	m_vp.Width = (float)2;
	m_vp.MinDepth = 0;
	m_vp.MaxDepth = 1;
	m_vp.TopLeftX = 0;
	m_vp.TopLeftY = 0;

	// Create a depth stencil state to disable depth testing, and get the default state
	UINT stencilRef;
	m_pDeviceContext->OMGetDepthStencilState( &m_pDepthStateDefault, &stencilRef );

	D3D11_DEPTH_STENCILOP_DESC depthOpDesc;
	depthOpDesc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthOpDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthOpDesc.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthOpDesc.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	D3D11_DEPTH_STENCIL_DESC depthDesc;
	depthDesc.BackFace = depthOpDesc;
	depthDesc.DepthEnable = false;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.FrontFace = depthOpDesc;
	depthDesc.StencilEnable = false;
	depthDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	if( FAILED( hr = m_pDevice->CreateDepthStencilState(&depthDesc, &m_pDepthStateDisabled) ) )
	{
		WriteToLog( _T( "CPullModel::CreateDepthStencilState failed with %s while creating the depth disabled state." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	// calculate the expected interpolated values for the two different attribute data
	CalcExpectedValues( 0.0, 1.0, 0.0, m_SampleGridValuesSnorm );
	CalcExpectedValues( 16.0, 64.0, 0.0, m_SampleGridValuesFloat );

	return RESULT_PASS;
}

//----------------------------------------------------------------------------------------------------------
TEST_RESULT CPullModel::SetupSampleTables() 
{
	
	// fill the sampleposition tables according to D3D10.1 spec
	m_samplePosTable[0].push_back( SamplePosition(0, 0) );
	m_UpperLeftCentoid[0] = 0;

	m_samplePosTable[1].push_back( SamplePosition(4, 4) );
	m_samplePosTable[1].push_back( SamplePosition(-4, -4) );
	m_UpperLeftCentoid[1] = 1;

	m_samplePosTable[2].push_back( SamplePosition(-2, -6) );
	m_samplePosTable[2].push_back( SamplePosition(6, -2) );
	m_samplePosTable[2].push_back( SamplePosition(-6, 2) );
	m_samplePosTable[2].push_back( SamplePosition(2, 6) );
	m_UpperLeftCentoid[2] = 0;

	m_samplePosTable[3].push_back( SamplePosition(1, -3) );
	m_samplePosTable[3].push_back( SamplePosition(-1, 3) );
	m_samplePosTable[3].push_back( SamplePosition(5, 1) );
	m_samplePosTable[3].push_back( SamplePosition(-3, -5) );
	m_samplePosTable[3].push_back( SamplePosition(-5, 5) );
	m_samplePosTable[3].push_back( SamplePosition(-7, -1) );
	m_samplePosTable[3].push_back( SamplePosition(3, 7) );
	m_samplePosTable[3].push_back( SamplePosition(7, -7) );
	m_UpperLeftCentoid[3] = 0;

	m_samplePosTable[4].push_back( SamplePosition(1, 1) );
	m_samplePosTable[4].push_back( SamplePosition(-1, -3) );
	m_samplePosTable[4].push_back( SamplePosition(-3, 2) );
	m_samplePosTable[4].push_back( SamplePosition(4, -1) );
	m_samplePosTable[4].push_back( SamplePosition(-5, -2) );
	m_samplePosTable[4].push_back( SamplePosition(2, 5) );
	m_samplePosTable[4].push_back( SamplePosition(5, 3) );
	m_samplePosTable[4].push_back( SamplePosition(3, -5) );
	m_samplePosTable[4].push_back( SamplePosition(-2, 6) );
	m_samplePosTable[4].push_back( SamplePosition(0, -7) );
	m_samplePosTable[4].push_back( SamplePosition(-4, -6) );
	m_samplePosTable[4].push_back( SamplePosition(-6, 4) );
	m_samplePosTable[4].push_back( SamplePosition(-8, 0) );
	m_samplePosTable[4].push_back( SamplePosition(7, -4) );
	m_samplePosTable[4].push_back( SamplePosition(6, 7) );
	m_samplePosTable[4].push_back( SamplePosition(-7, -8) );
	m_UpperLeftCentoid[4] = 1;

	return RESULT_PASS;
}


//----------------------------------------------------------------------------------------------------------
// Since the triangle covers the upper left of the 2x2 pixel area, it is a 32x32 grid for sample positions
TEST_RESULT CPullModel::CalcExpectedValues( float valVert0, float valVert1, float valVert2, float valueTable[3][16][16] )
{
	float horizStep = (valVert2 - valVert0)/32;
	float vertiStep = (valVert1 - valVert0)/32;

	float startPix = valVert0;
	float startRow = startPix;
	for (UINT k = 0 ; k < 3; k++ )
	{
		if ( k == 1)
			startPix = horizStep * 16 + valVert0;
		else if ( k == 2)
			startPix = vertiStep * 16 + valVert0;
		for (UINT j = 0; j < 16; j++)
		{
			startRow = startPix + j * vertiStep;
			for (UINT i = 0; i < 16; i++)
			{
				valueTable[k][j][i] = startRow + horizStep * i;
			}
		}	
	}
	return RESULT_PASS;
}

//----------------------------------------------------------------------------------------------------------
void CPullModel::Cleanup() 
{
	SAFE_RELEASE(m_pDepthStateDefault);
	SAFE_RELEASE(m_pDepthStateDisabled);

	for ( int i = 0; i < 5; i++ )
		m_samplePosTable[i].clear();
}

//----------------------------------------------------------------------------------------------------------
TEST_RESULT CPullModel::SetupTestCase()
{
	// Local variables
	HRESULT hr = E_FAIL;

	//
	// If a forced sample count is being used, make sure the driver supports it
	//

	// Forced sample count > 0 is only supported on drivers that support the 11.1 DDI
	if( !GetFramework()->Is11_1DDISupported( GetDevice1() ) &&
		m_uForcedSampleCount > 0 )
	{
		WriteToLog( _T( "Skipping test case because forced sample count's > 0 are only supported on drivers that support the 11.1 DDI." ), D3DHResultToString(hr).c_str() );
		return RESULT_SKIP;
	}

	// FL11.1+ requires support for all sample counts. FL10.0-FL11.0 require support for forced sample count 1.
	if( GetDevice1()->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_1 &&
		m_uForcedSampleCount > 1 )
	{
		WriteToLog( _T( "Skipping test case because forced sample count's > 1 are only supported on FL11.1+ HW." ), D3DHResultToString(hr).c_str() );
		return RESULT_SKIP;
	}

	// Win8:293911 - Sample count 1 is only valid on D3D_FEATURE_LEVEL_11_0 or lower when MultisampleEnable is false
	const bool bMultisampleEnable = ((m_uForcedSampleCount == 1) && (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_1)) ? FALSE : TRUE;

	// Win8: WGF11Rasterizer: PullModel: Pixel Shader cannot be configured to execute at sample-frequency when ForcedSampleCount > 0
	// There are two ways to invoke sample frequency execution:
	// 1. Use a Pixel Shader Attribute Interpolation Mode: LinearSample or LinearNoPerspectiveSample
	// 2. If sampleIndex is declared for input into the Pixel Shader
	const bool bSampleIndexIsRequested = false; // Note, sampleIndex is not used in this test
	const bool bSampleFrequencyPSInvocation = (m_Mode == LINEAR_SAMPLE || m_Mode == LINEAR_NOPERSPECTIVE_SAMPLE || bSampleIndexIsRequested); 
	if( m_uForcedSampleCount > 0 && bSampleFrequencyPSInvocation )
	{
		WriteToLog( _T( "Skipping test case because forced sample count's > 0 are undefined for sample frequency pixel shader invocation." ) );
		return RESULT_SKIP;
	}

	// Create rasterizer state
	D3D11_RASTERIZER_DESC1 rastDesc = CD3D11_RASTERIZER_DESC1(D3D11_FILL_SOLID,
									  D3D11_CULL_NONE,
									  TRUE,
									  D3D11_DEFAULT_DEPTH_BIAS,
									  D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
									  D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
									  TRUE,
									  FALSE,
									  bMultisampleEnable, // MultisampleEnable,
									  FALSE,
									  m_uForcedSampleCount);

	if( FAILED( hr = GetDevice1()->CreateRasterizerState1( &rastDesc, &m_pRastState ) ) )
	{
		WriteToLog( _T( "CPullModel::CreateRasterizerState failed with %s\n" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	TEST_RESULT tr = SetupRenderTarget();
	if (tr != RESULT_PASS)
	{
		WriteToLog( _T("CPullModel::SetupRenderTarget()") );
		return tr;
	}
	
	tr = SetupVSStage();
	if (tr != RESULT_PASS)
	{
		WriteToLog( _T("CPullModel::SetupVSStage()") );
		return tr;
	}

	tr = BuildPixelShader();
	if (tr != RESULT_PASS)
	{
		WriteToLog( _T("CPullModel::BuildPixelShader()") );
		return tr;
	}

	return RESULT_PASS;
}

//----------------------------------------------------------------------------------------------------------
TEST_RESULT CPullModel::SetupRenderTarget()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;

	// Check support
	UINT NumQualityLevels = 0;
	hr = GetDevice()->CheckMultisampleQualityLevels(m_RTFormat, m_uRTSampleCount, &NumQualityLevels);
	if(SUCCEEDED(hr))
	{
		if(NumQualityLevels == 0)
		{
			WriteToLog(_T("Skipping test cases as the hardware does not support the format with the \
						  specified number of samples."));
			tr = RESULT_SKIP;
			goto Done;
		}
	}
	else
	{
		WriteToLog(_T("CPullModel::SetupRenderTarget(): CheckMultisampleQualityLevels() failed. HR = %s"), 
			D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Done;
	}

	// Win7:687504
	UINT formatSupport = 0;
	hr = GetDevice()->CheckFormatSupport( m_RTFormat, &formatSupport );

	if(SUCCEEDED(hr))
	{
		// Check if the format is supported on the Texture2D RT
		if( !(D3D11_FORMAT_SUPPORT_RENDER_TARGET & formatSupport) )
		{
			WriteToLog(_T("Skipping test cases as the hardware does not support the format with the \
						  current render target dimension: Texture2D."));
			tr = RESULT_SKIP;
			goto Done;
		}

		if( m_uRTSampleCount > 1 )
		{
			const bool bFormatIsRGBA32FLOAT = (m_RTFormat == DXGI_FORMAT_R32G32B32A32_FLOAT);
			const bool bFormatIsRGBA32UINT = (m_RTFormat == DXGI_FORMAT_R32G32B32A32_UINT);
			const bool bFormatIsRGBA32SINT = (m_RTFormat == DXGI_FORMAT_R32G32B32A32_SINT);

			// Some things are required, we won't check those
			if( 4 == m_uRTSampleCount )
			{
				// All 4x multisample render target are required, don't skip
			}
			else if(	8 == m_uRTSampleCount &&
						!bFormatIsRGBA32FLOAT &&
						!bFormatIsRGBA32UINT &&
						!bFormatIsRGBA32SINT )
			{
				/*
					Only these formats are optional for 8x multisample, so don't ask
					DXGI_FORMAT_R32G32B32A32_FLOAT
					DXGI_FORMAT_R32G32B32A32_UINT
					DXGI_FORMAT_R32G32B32A32_SINT
				*/
			}
			else
			{
				// Make sure multisampling is supported
				if( !(D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET & formatSupport) )
				{
					WriteToLog(_T("Skipping test cases as the hardware does not support multisampling with the current Render Target format."));
					tr = RESULT_SKIP;
					goto Done;
				}
			}
		}
	}
	else
	{
		WriteToLog(_T("CPullModel::SetupRenderTarget(): CheckFormatSupport() failed. HR = %s"), 
			D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto Done;
	}

	// Create render target & view
	D3D11_TEXTURE2D_DESC rtDesc;
	rtDesc.Width = 2;
    rtDesc.Height = 2;
    rtDesc.MipLevels = 1;
    rtDesc.ArraySize = 1;
    rtDesc.Format = m_RTFormat;
    rtDesc.SampleDesc.Count = m_uRTSampleCount;
	rtDesc.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
    rtDesc.Usage = D3D11_USAGE_DEFAULT;
    rtDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    rtDesc.CPUAccessFlags = 0;
    rtDesc.MiscFlags = 0;

	if( FAILED( hr = GetDevice()->CreateTexture2D( &rtDesc, NULL, &m_pRTTexture2D ) ) )
	{		
		WriteToLog( _T( "CPullModel::SetupRenderTarget(): CreateTexture2D failed. HR = %s\n" ), 
			D3DHResultToString(hr).c_str() );	
		tr = RESULT_FAIL;
		goto Done;
	}

	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRTTexture2D, NULL, &m_pRTView ) ) )
	{
		WriteToLog( _T( "CreateRenderTargetView failed. HR = %s\n" ), D3DHResultToString(hr).c_str() );	
		tr = RESULT_FAIL;
		goto Done;
	}

	return RESULT_PASS;

Done:
	SAFE_RELEASE(m_pRTTexture2D);
	SAFE_RELEASE(m_pRTView);

	return tr;

}

//----------------------------------------------------------------------------------------------------------
// Create vertex buffer, vertex shader, inputlayout
TEST_RESULT CPullModel::SetupVSStage()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;

	tstring VSPassthrough = 
"		struct VSIn \n" 
"		{ \n"
"			float3 pos : POSITION; \n"
"			DATA_DCLS \n"
"		}; \n"
" \n"
"		struct VSOut \n"
"		{ \n"
"			float4 pos : SV_POSITION; \n"
"			DATA_DCLS \n"
"		}; \n"
" \n"
"		VSOut VSMain(VSIn input) \n"
"		{ \n"
"			VSOut output; \n"
"			output.pos.xyz = input.pos.xyz; \n"
"			output.pos.w = 1; \n"
"			DATA_TO_OUTPUT \n"
"			\n"
"			return output; \n"
"		} \n";
	
	tstring dataDclsStr;  // to replace macro DATA_DCLS
	tstring dataOutStr;	  // to replace macro DATA_TO_OUTPUT

	// create vertex buffer data
	float * pVBData;
	if (!CreateVBValues(&pVBData))
	{
		WriteToLog(_T("CPullModel::SetupVSStage(): CreateVBValues failed."));
		return RESULT_FAIL;
	}
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pVBData;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;


	D3D11_BUFFER_DESC bufDesc;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;

	D3D11_INPUT_ELEMENT_DESC InputDescs1Value[2] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "DATA", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0  }
	};
	D3D11_INPUT_ELEMENT_DESC InputDescs4Value[3] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "DATA", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "DATA", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	D3D11_INPUT_ELEMENT_DESC *pInputElementDescs = NULL;
	UINT NumElements = 0;

	if (m_numInterValues == 1)
	{
		bufDesc.ByteWidth = sizeof( Vertex1Value ) * 3;
		pInputElementDescs = InputDescs1Value;
		NumElements = 2;

		dataDclsStr = "float data0 : DATA0; \n";
		dataOutStr = "output.data0 = input.data0; \n";	
	}
	else if (m_numInterValues == 4)
	{
		bufDesc.ByteWidth = sizeof( Vertex4Value ) * 3;
		pInputElementDescs = InputDescs4Value;
		NumElements = 3;

		dataDclsStr = "float data0 : DATA0; ";
		dataDclsStr += "float3 data1 : DATA1; \n";
		dataOutStr = "output.data0 = input.data0; ";
		dataOutStr += "output.data1 = input.data1; \n";
	}

	// create vertex buffer
	hr = m_pDevice->CreateBuffer( &bufDesc, &initData, &m_pVB );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CPullModel::SetupVSStage() - CreateBuffer failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto Done;
	}

	// create vertex shader
	D3D10_SHADER_MACRO macroSet[] =
	{
		{ "DATA_DCLS", dataDclsStr.c_str() },
		{ "DATA_TO_OUTPUT", dataOutStr.c_str() },
		NULL
	};

	hr = D3DX11CompileFromMemory(	
								VSPassthrough.c_str(), 
								strlen(VSPassthrough.c_str()),
								NULL,
								macroSet,
								NULL,
								_T("VSMain"),
								_T("vs_5_0"),
								0,
								0,
								NULL,
								&pShaderBuf,
								&pErrorBuf,
								NULL );

	if ( FAILED(hr) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T( "CPullModel::SetupVSStage(): Compiler errors:"));
		if (pErrorBuf)
		{
			LPVOID pBuf = pErrorBuf->GetBufferPointer();
			size_t bufSize = pErrorBuf->GetBufferSize();
			char* szError = new char[bufSize+1];
			memcpy(szError, pBuf, bufSize);
			szError[bufSize] = 0;
			WriteToLog( _T( "%s\n" ), szError );
			delete[] szError;
		}
		goto Done;
	}
	hr = m_pDevice->CreateVertexShader( 
		pShaderBuf->GetBufferPointer(), 
		pShaderBuf->GetBufferSize(), 
		NULL, 
		&m_pVS
	);

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CPullModel::SetupVSStage - Failed to create vertex shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto Done;
	}

	// create inputlayout
	hr = m_pDevice->CreateInputLayout( 
			pInputElementDescs, 
			NumElements, 
			pShaderBuf->GetBufferPointer(), 
			pShaderBuf->GetBufferSize(), 
			&m_pVertexLayout );
	if (FAILED(hr))
	{
		WriteToLog( _T( "CPullModel::SetupVSStage() - CreateInputLayout failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto Done;
	}

Done:
	delete[] pVBData;
	SAFE_RELEASE(pShaderBuf);
	SAFE_RELEASE(pErrorBuf);


	return tr;
}

//----------------------------------------------------------------------------------------------------------
TEST_RESULT CPullModel::CreateVBValues( float** ppRetVBValues)
{
	UINT bufSize = 0;
	if ( m_numInterValues == 1  )
		bufSize = sizeof( Vertex1Value ) * 3;
	else if ( m_numInterValues == 4 )
		bufSize = sizeof( Vertex4Value ) * 3;

	float* pVBValues = new float[bufSize];
	if (!pVBValues)
	{
		WriteToLog(_T("CPullModel::CreateVBValues: failed to allocate memory."));
		return RESULT_FAIL;
	}

	if ( m_numInterValues == 1 )
	{
		// positions for 3 verticis
		pVBValues[0] = -1.0; pVBValues[1] = 1.0; pVBValues[2] = 0.0;  //v1
		pVBValues[4] = -1.0; pVBValues[5] = -1.0; pVBValues[6] = 0.0;  //v2
		pVBValues[8] = 1.0; pVBValues[9] = 1.0; pVBValues[10] = 0.0;  //v3
		
		// data for 3 verticis
		pVBValues[3] = 0.0;  //v1
		pVBValues[7] = 1.0;  //v2
		pVBValues[11] = 0.0; //v3
	}
	else if ( m_numInterValues == 4 )
	{
		// positions for 3 verticis
		pVBValues[0] = -1.0; pVBValues[1] = 1.0; pVBValues[2] = 0.0;  //v1
		pVBValues[7] = -1.0; pVBValues[8] = -1.0; pVBValues[9] = 0.0;  //v2
		pVBValues[14] = 1.0; pVBValues[15] = 1.0; pVBValues[16] = 0.0;  //v3

		// data for 3 verticis: the last 3 values are the same for each vertex
		pVBValues[3] = 0.0;   pVBValues[4] = 16.0;  pVBValues[5] = 16.0;  pVBValues[6] = 16.0;  //v1
		pVBValues[10] = 1.0;  pVBValues[11] = 64.0; pVBValues[12] = 64.0; pVBValues[13] = 64.0;  //v2
		pVBValues[17] = 0.0;  pVBValues[18] = 0.0;  pVBValues[19] = 0.0;  pVBValues[20] = 0.0;  //v3
	}
	else
	{
		*ppRetVBValues = NULL;
		WriteToLog(_T("CPullModel::CreateVBValues: wrong number of values to be interpolated."));
		return RESULT_FAIL;
	}

	*ppRetVBValues = pVBValues;
	return RESULT_PASS;
}

//----------------------------------------------------------------------------------------------------------
TEST_RESULT CPullModel::BuildPixelShader()
{
	tstring pixelShaderStr = 
"	struct PSIn \n"
"   { \n"
"  		float4 pos : SV_POSITION; \n"
"		ATTRIB_DATA_DCLS \n"
"   }; \n"
"   \n"
"	float4 PSMain(PSIn input) : SV_TARGET \n"
"	{    \n"
"		float4 temp = float4(0.0, 0.0, 0.0, 0.0); \n"
"		ATTRIB_EVALS \n"
"		return (temp); \n"
"	} \n";
	
	// mode strings
	tstring ModeStrSet[]	 = {
		"nointerpolation",
		"linear",
		"centroid",
		"noperspective",
		"centroid noperspective",
		"sample",
		"sample noperspective",
	};
	tstring InterModeStr = ModeStrSet[m_Mode];

	tstring AttribDcls;
	tstring AttribEvals;
	
	AttribDcls += InterModeStr;
	AttribDcls += " float data0 : DATA0; ";

	if (m_numInterValues == 4)
	{
		AttribDcls += InterModeStr;
		AttribDcls += " float3 data1 : DATA1; \n";
	}

	if ( m_evalFunc == EVAL_SNAPPED)
	{
		AttribEvals += FormatString(_T("temp.x += EvaluateAttributeSnapped(input.data0, int2(%d, %d)); "), 
			m_SamplePos.pos_x, m_SamplePos.pos_y);
		if (m_numInterValues == 4)
		{
			AttribEvals += FormatString(_T("temp.yzw += EvaluateAttributeSnapped(input.data1, int2(%d, %d)); \n"), 
				m_SamplePos.pos_x, m_SamplePos.pos_y);
		}
	}
	else if ( m_evalFunc == EVAL_SAMPLE)
	{
		AttribEvals += FormatString(_T("temp.x += EvaluateAttributeAtSample(input.data0, %d); "), m_SampleIndex);
		if (m_numInterValues == 4)
		{
			AttribEvals += FormatString(_T("temp.yzw += EvaluateAttributeAtSample(input.data1, %d); \n"), m_SampleIndex);
		}
	}
	else if ( m_evalFunc == EVAL_CENTROID)
	{
		AttribEvals += "temp.x += EvaluateAttributeCentroid(input.data0); ";
		if (m_numInterValues == 4)
		{
			AttribEvals += "temp.yzw += EvaluateAttributeCentroid(input.data1); \n";
		}
	}
	else if ( m_evalFunc == USE_ALL_EVALS)
	{
		AttribEvals += FormatString(_T("temp.x += EvaluateAttributeSnapped(input.data0, int2(%d, %d)); "), 
			m_SamplePos.pos_x, m_SamplePos.pos_y);
		AttribEvals += FormatString(_T("temp.y += EvaluateAttributeSnapped(input.data1.x, int2(%d, %d)); "), 
			m_SamplePos.pos_x, m_SamplePos.pos_y);
		AttribEvals += FormatString(_T("temp.z += EvaluateAttributeAtSample(input.data1.y, %d); "), m_SampleIndex);
		AttribEvals += "temp.w += EvaluateAttributeCentroid(input.data1.z); \n";
	}

	// create pixel shader
	D3D10_SHADER_MACRO macroSet[] =
	{
		{ "ATTRIB_DATA_DCLS", AttribDcls.c_str() },
		{ "ATTRIB_EVALS", AttribEvals.c_str() },
		NULL
	};
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	ID3D10Blob* pShaderText = NULL;

	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;
	hr = g_TestApp.CompileShaderFromMemory(	
								pixelShaderStr.c_str(), 
								strlen(pixelShaderStr.c_str()),
								_T("PSMain"),
								D3D_SHADER_STAGE_PIXEL,
								D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_0,
								0,
								&pShaderBuf,
								&pErrorBuf,
								macroSet,
								&pShaderText );

	if ( FAILED(hr) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T( "CPullModel::BuildPixelShader(): Compiler errors:"));
		if (pErrorBuf)
		{
			LPVOID pBuf = pErrorBuf->GetBufferPointer();
			size_t bufSize = pErrorBuf->GetBufferSize();
			char* szError = new char[bufSize+1];
			memcpy(szError, pBuf, bufSize);
			szError[bufSize] = 0;
			WriteToLog( _T( "%s\n" ), szError );
			delete[] szError;
		}
		goto Done;
	}
	hr = m_pDevice->CreatePixelShader( 
		pShaderBuf->GetBufferPointer(), 
		pShaderBuf->GetBufferSize(), 
		NULL, 
		&m_pPS
	);

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CPullModel::BuildPixelShader() - Failed to create vertex shader, hr=%s" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto Done;
	}

Done:

	SAFE_RELEASE(pShaderBuf);
	SAFE_RELEASE(pErrorBuf);

	return tr;
}

//----------------------------------------------------------------------------------------------------------
void CPullModel::CleanupTestCase()
{
	SAFE_RELEASE(m_pRastState);

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVS);

	SAFE_RELEASE(m_pPS);

	SAFE_RELEASE(m_pRTTexture2D);
	SAFE_RELEASE(m_pRTView);
}

//----------------------------------------------------------------------------------------------------------
TEST_RESULT CPullModel::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;

	// disable the depth stencil (if needed)
	if( m_uForcedSampleCount > 0 )
	{
		// Depth/Stencil Views must not be bound, depth testing must be disabled, and the shader must not output depth while rendering with ForcedSampleCount 1 or greater, otherwise rendering behavior is undefined
		m_pDeviceContext->OMSetDepthStencilState( m_pDepthStateDisabled, 0 );
	}
	else
	{
		m_pDeviceContext->OMSetDepthStencilState( m_pDepthStateDefault, 0 );
	}

	// set RS stage
	m_pDeviceContext->RSSetViewports( 1, &m_vp );
	m_pDeviceContext->RSSetState(m_pRastState);
		
	// bind shaders
	m_pDeviceContext->VSSetShader(m_pVS, NULL, 0);
	m_pDeviceContext->PSSetShader(m_pPS, NULL, 0);

	// set vertex buffer and input layout
	UINT VBStride = 0;
	if (m_numInterValues == 1)
		VBStride = sizeof(Vertex1Value);
	else if (m_numInterValues == 4)
		VBStride = sizeof(Vertex4Value);
	UINT VBOffset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &VBStride, &VBOffset);
	m_pDeviceContext->IASetInputLayout(m_pVertexLayout);

	// set input primitive topology
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// set and clear rendertarget
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRTView, NULL);
	float clearZero[4] = {0.0, 0.0, 0.0, 0.0};
	m_pDeviceContext->ClearRenderTargetView(m_pRTView, clearZero);

	// draw
	m_pDeviceContext->Draw(3, 0);
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext failed. HR = %s"), D3DHResultToString( hr ).c_str());
		return RESULT_FAIL;
	}
		
	//Verify results
	float* pSampleColors = new float[16 * m_uRTSampleCount];
	if( !MSHelper::Read(GetFramework(), m_pRTTexture2D, pSampleColors, m_RTFormat) )
	{
		WriteToLog( _T( "MSHelper::Read failed\n") );
		tr = RESULT_FAIL;
		goto Done;
	}
	
	tr = VerifyResult( pSampleColors );

Done:
	delete[] pSampleColors;
	return tr;	
}


//----------------------------------------------------------------------------------------------------------
TEST_RESULT CPullModel::VerifyResult( float* pResultData )
{
	TEST_RESULT tr = RESULT_PASS;
	// calculate the table id for a sampleCount: sampleCount = 2^(tableId)
	const bool bForcedSampleCountIsUsed = (m_uRTSampleCount == 1) && (m_uForcedSampleCount > 0);
	const UINT effectiveSampleCount = bForcedSampleCountIsUsed ? m_uForcedSampleCount : m_uRTSampleCount;
	UINT sampleCount = effectiveSampleCount;
	UINT tableId = 0;
	UINT i = 0;
	for(; sampleCount > 0; i++ ) 
		sampleCount >>= 1; 
	tableId = i - 1; 

	UINT centSampleId = m_UpperLeftCentoid[tableId];

	// finding the sample position
	SamplePosition samplePosAll[3]; // used for USE_ALL_EVALS scenario
	SamplePosition samplePos;

	if ( m_evalFunc == EVAL_SNAPPED )
	{
		samplePos = m_SamplePos;
	}
	else if ( m_evalFunc == EVAL_SAMPLE )
	{
		samplePos = m_samplePosTable[tableId][m_SampleIndex];
	}
	else if ( m_evalFunc == EVAL_CENTROID )
	{
		samplePos = m_samplePosTable[tableId][centSampleId];
	}
	else if ( m_evalFunc == USE_ALL_EVALS )
	{
		samplePos = m_SamplePos;
		samplePosAll[0] = m_SamplePos;
		samplePosAll[1] = m_samplePosTable[tableId][m_SampleIndex];
		samplePosAll[2] = m_samplePosTable[tableId][centSampleId];
	}

	// compare the results for 3 pixels
	for ( i = 0; i < 3; i++ )
	{
		// skip pixels that won't get rendered when samplecount is 1
		if ( (i == 1 || i == 2) && effectiveSampleCount == 1)
			continue;

		SamplePosition realSampPos = samplePos;
		if ( i == 0 && m_evalFunc == EVAL_CENTROID ) // center is used for fully covered pixel with EVAL_CENTROID
		{
			realSampPos.pos_x = 0;
			realSampPos.pos_y = 0;
		}

		FLOAT_COLOR resultFloat;
		if( bForcedSampleCountIsUsed )
		{
			// The result array is only "single" sample
			assert( m_uRTSampleCount == 1 );
			resultFloat.comp[0] = pResultData[i * 4 + 0];
			resultFloat.comp[1] = pResultData[i * 4 + 1];
			resultFloat.comp[2] = pResultData[i * 4 + 2];
			resultFloat.comp[3] = pResultData[i * 4 + 3];
		}
		else
		{
			resultFloat.comp[0] = pResultData[centSampleId * 16 + i * 4 + 0];
			resultFloat.comp[1] = pResultData[centSampleId * 16 + i * 4 + 1];
			resultFloat.comp[2] = pResultData[centSampleId * 16 + i * 4 + 2];
			resultFloat.comp[3] = pResultData[centSampleId * 16 + i * 4 + 3];
		}
	
		// the first attribute data
		if ( (tr = CompareToExpected(i, resultFloat.comp[0], realSampPos, m_SampleGridValuesSnorm[i])) != RESULT_PASS )
		{
			WriteToLog(_T("CPullModel::VerifyResult: CompareToExpected() failed at pixel %d, component 0."), i);
			return RESULT_FAIL;
		}
		// the second attribute data, which  has 3 components
		if ( m_evalFunc != USE_ALL_EVALS )
		{
			if ( m_numInterValues == 4 )
			{
				for (UINT j = 1; j < 4; j ++)
				{
					if ( (tr = CompareToExpected(i, resultFloat.comp[j], realSampPos, m_SampleGridValuesFloat[i])) != RESULT_PASS )
					{
						WriteToLog(_T("CPullModel::VerifyResult: CompareToExpected() failed at pixel %d, component %d."), i, j);
						return RESULT_FAIL;
					}
				}

			}
		}
		else // m_evalFunc == USE_ALL_EVALS 
		{
			for (UINT j = 1; j < 4; j ++)
			{
				realSampPos = samplePosAll[j-1];
				if ( i == 0 && j == 3 )  // center is used for fully covered pixel with EVAL_CENTROID
				{
					realSampPos.pos_x = 0;
					realSampPos.pos_y = 0;
				}
				if ( (tr = CompareToExpected(i, resultFloat.comp[j], realSampPos, m_SampleGridValuesFloat[i])) != RESULT_PASS ) 
				{
					WriteToLog(_T("CPullModel::VerifyResult: CompareToExpected() failed at pixel %d, component %d."), i, j);
					return RESULT_FAIL;
				}
			}
		}

	}

	return RESULT_PASS;

}


//----------------------------------------------------------------------------------------------------------
TEST_RESULT CPullModel::CompareToExpected(UINT pixId, float resultValue, SamplePosition sampPos, float expectedValues[16][16])
{
	UINT idx = sampPos.pos_x + 8;
	UINT idy = sampPos.pos_y + 8;
	float epsil = abs( expectedValues[0][1] - expectedValues[1][1] );
	float expVal = expectedValues[idy][idx];
	
	if ( abs(resultValue - expVal ) <= epsil)
		return RESULT_PASS;
	else
	{
		WriteToLog(_T("CPullModel::CompareToExpected: result value (%f) is significantly different than\
					  the expected value (%f)."), resultValue, expVal );
		return RESULT_FAIL;
	}
}