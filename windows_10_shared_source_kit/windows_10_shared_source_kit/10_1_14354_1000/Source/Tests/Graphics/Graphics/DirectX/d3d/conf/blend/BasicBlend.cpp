#include "Blend.h"
#include "FormatDesc.hpp" // used for makeD3DColor()
#include "FormatHelper.h"

static const int s_NumRenderTargets[] = 
{
	1,
	2,
	8
};
static const int s_CountNumRenderTargets = sizeof(s_NumRenderTargets) / sizeof(*s_NumRenderTargets);

void 
CBasicBlendTest::InitTestParameters()
{
	m_nPrimitiveW = g_App.Width();
	m_nPrimitiveH = g_App.Height();

	g_App.m_RenderTargetOptions.SwapChainDesc.Width = m_nPrimitiveW*2;
	g_App.m_RenderTargetOptions.SwapChainDesc.Height = m_nPrimitiveH;
	g_App.m_RenderTargetOptions.DepthTextureDesc.Width = m_nPrimitiveW*2;
	g_App.m_RenderTargetOptions.DepthTextureDesc.Height = m_nPrimitiveH;


	// set up some defaults;
	ZeroMemory(&m_vBlendFactor, sizeof(m_vBlendFactor));
	m_vBlendFactor.x = 0.5f;
	m_vBlendFactor.y = 0.625f;
	m_vBlendFactor.z = 0.75f;
	m_vBlendFactor.w = 0.875f;
	ZeroMemory(&m_BlendDesc_1, sizeof(m_BlendDesc_1));
	m_BlendDesc_1.AlphaToCoverageEnable = FALSE;
	m_BlendDesc_1.IndependentBlendEnable = FALSE;
	for (int n = 0; n < 8; ++n)
	{
		m_BlendDesc_1.RenderTarget[n].BlendEnable = FALSE;
		m_BlendDesc_1.RenderTarget[n].BlendOp = D3D11_BLEND_OP_ADD;
		m_BlendDesc_1.RenderTarget[n].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		m_BlendDesc_1.RenderTarget[n].SrcBlend = D3D11_BLEND_ONE;
		m_BlendDesc_1.RenderTarget[n].SrcBlendAlpha = D3D11_BLEND_ONE;
		m_BlendDesc_1.RenderTarget[n].DestBlend = D3D11_BLEND_ZERO;
		m_BlendDesc_1.RenderTarget[n].DestBlendAlpha = D3D11_BLEND_ZERO;
		m_BlendDesc_1.RenderTarget[n].RenderTargetWriteMask = 0xf;
	}

	m_sPixelShaderName = _T("ps_blend.psh");
	m_sPSEntryPt = _T("main");
	m_sVertexShaderName = _T("vs.vsh");

	m_sPixelShaderNameFL9X = _T("ps_blend_fl9x.psh");
	m_sVertexShaderNameFL9X = _T("vs_fl9x.vsh");

	CUserValueSet<D3D11_BLEND_OP>* pBlendOps_NoMinMax = new CUserValueSet<D3D11_BLEND_OP>();
	pBlendOps_NoMinMax->AddValue(D3D11_BLEND_OP_ADD);
	pBlendOps_NoMinMax->AddValue(D3D11_BLEND_OP_SUBTRACT);
	pBlendOps_NoMinMax->AddValue(D3D11_BLEND_OP_REV_SUBTRACT);
	CUserValueSet<D3D11_BLEND_OP>* pBlendOps_MinMax = new CUserValueSet<D3D11_BLEND_OP>();
	pBlendOps_MinMax->AddValue(D3D11_BLEND_OP_MIN);
	pBlendOps_MinMax->AddValue(D3D11_BLEND_OP_MAX);

	CUserValueSet<D3D11_BLEND>* pBlendTypes = new CUserValueSet<D3D11_BLEND>();
	pBlendTypes->AddValue(D3D11_BLEND_ZERO);
	pBlendTypes->AddValue(D3D11_BLEND_ONE);
	pBlendTypes->AddValue(D3D11_BLEND_SRC_COLOR);
	pBlendTypes->AddValue(D3D11_BLEND_INV_SRC_COLOR);
	pBlendTypes->AddValue(D3D11_BLEND_SRC_ALPHA);
	pBlendTypes->AddValue(D3D11_BLEND_INV_SRC_ALPHA);
	pBlendTypes->AddValue(D3D11_BLEND_DEST_ALPHA);
	pBlendTypes->AddValue(D3D11_BLEND_INV_DEST_ALPHA);
	pBlendTypes->AddValue(D3D11_BLEND_DEST_COLOR);
	pBlendTypes->AddValue(D3D11_BLEND_INV_DEST_COLOR);
	pBlendTypes->AddValue(D3D11_BLEND_SRC_ALPHA_SAT);
	pBlendTypes->AddValue(D3D11_BLEND_BLEND_FACTOR);
	pBlendTypes->AddValue(D3D11_BLEND_INV_BLEND_FACTOR);
	pBlendTypes->AddValue(D3D11_BLEND_SRC1_COLOR);
	pBlendTypes->AddValue(D3D11_BLEND_INV_SRC1_COLOR);
	pBlendTypes->AddValue(D3D11_BLEND_SRC1_ALPHA);
	pBlendTypes->AddValue(D3D11_BLEND_INV_SRC1_ALPHA);

	CUserValueSet<D3D11_BLEND>* pBlendAlphaTypes = new CUserValueSet<D3D11_BLEND>();
	pBlendAlphaTypes->AddValue(D3D11_BLEND_ZERO);
	pBlendAlphaTypes->AddValue(D3D11_BLEND_ONE);
	pBlendAlphaTypes->AddValue(D3D11_BLEND_SRC_ALPHA);
	pBlendAlphaTypes->AddValue(D3D11_BLEND_INV_SRC_ALPHA);
	pBlendAlphaTypes->AddValue(D3D11_BLEND_DEST_ALPHA);
	pBlendAlphaTypes->AddValue(D3D11_BLEND_INV_DEST_ALPHA);
	pBlendAlphaTypes->AddValue(D3D11_BLEND_SRC_ALPHA_SAT);
	pBlendAlphaTypes->AddValue(D3D11_BLEND_BLEND_FACTOR);
	pBlendAlphaTypes->AddValue(D3D11_BLEND_INV_BLEND_FACTOR);
	pBlendAlphaTypes->AddValue(D3D11_BLEND_SRC1_ALPHA);
	pBlendAlphaTypes->AddValue(D3D11_BLEND_INV_SRC1_ALPHA);

	// blend op
	CTestCaseParameter<D3D11_BLEND_OP> *pBlendOpParam = 
						  AddParameter<D3D11_BLEND_OP>( _T("Blend_Op"), &m_BlendDesc_1.RenderTarget[0].BlendOp );
	SetParameterDefaultValue( _T("Blend_Op"), D3D11_BLEND_OP_ADD );
	testfactor::RFactor rBlendOp_NoMinMax 
		= AddParameterValueSet( _T("Blend_Op"), pBlendOps_NoMinMax );
	testfactor::RFactor rBlendOp_All
		= rBlendOp_NoMinMax
		+ AddParameterValueSet( _T("Blend_Op"), pBlendOps_MinMax );

	// src blend
	CTestCaseParameter<D3D11_BLEND> *pSrcBlendParam = 
						  AddParameter<D3D11_BLEND>( _T("Src_Blend"), &m_BlendDesc_1.RenderTarget[0].SrcBlend );
	SetParameterDefaultValue( _T("Src_Blend"), D3D11_BLEND_ONE );
	testfactor::RFactor rSrcBlend = AddParameterValueSet( _T("Src_Blend"), pBlendTypes );

	// dest blend
	CTestCaseParameter<D3D11_BLEND> *pDestBlendParam = 
						  AddParameter<D3D11_BLEND>( _T("Dst_Blend"), &m_BlendDesc_1.RenderTarget[0].DestBlend );
	SetParameterDefaultValue( _T("Dst_Blend"), D3D11_BLEND_ONE );
	testfactor::RFactor rDstBlend = AddParameterValueSet( _T("Dst_Blend"), pBlendTypes );
	testfactor::RFactor rDstBlend_One = AddParameterValue( _T("Dst_Blend"), D3D11_BLEND_ONE );

	// alpha blend op
	CTestCaseParameter<D3D11_BLEND_OP> *pBlendOpAlphaParam = 
						  AddParameter<D3D11_BLEND_OP>( _T("Blend_Op_A"), &m_BlendDesc_1.RenderTarget[0].BlendOpAlpha );
	SetParameterDefaultValue( _T("Blend_Op_A"), D3D11_BLEND_OP_ADD );
	testfactor::RFactor rBlendOpAlpha_NoMinMax 
		= AddParameterValueSet( _T("Blend_Op_A"), pBlendOps_NoMinMax );
	testfactor::RFactor rBlendOpAlpha_All 
		= rBlendOpAlpha_NoMinMax
		+ AddParameterValueSet( _T("Blend_Op_A"), pBlendOps_MinMax );

	// alpha src blend
	CTestCaseParameter<D3D11_BLEND> *pSrcBlendAlphaParam = 
						  AddParameter<D3D11_BLEND>( _T("Src_Blend_A"), &m_BlendDesc_1.RenderTarget[0].SrcBlendAlpha );
	SetParameterDefaultValue( _T("Src_Blend_A"), D3D11_BLEND_ONE );
	testfactor::RFactor rSrcBlendAlpha = AddParameterValueSet( _T("Src_Blend_A"), pBlendAlphaTypes );
	testfactor::RFactor rSrcBlendAlphaExtra = AddParameterValueSet( _T("Src_Blend_A"), pBlendTypes );

	// alpha dest blend
	CTestCaseParameter<D3D11_BLEND> *pDestBlendAlphaParam = 
						  AddParameter<D3D11_BLEND>( _T("Dst_Blend_A"), &m_BlendDesc_1.RenderTarget[0].DestBlendAlpha );
	SetParameterDefaultValue( _T("Dst_Blend_A"), D3D11_BLEND_ONE );
	testfactor::RFactor rDstBlendAlpha = AddParameterValueSet( _T("Dst_Blend_A"), pBlendAlphaTypes );
	testfactor::RFactor rDstBlendAlphaExtra = AddParameterValueSet( _T("Dst_Blend_A"), pBlendTypes );
	testfactor::RFactor rDstBlendAlpha_One = AddParameterValue( _T("Dst_Blend_A"), D3D11_BLEND_ONE );

	// color saturation
	AddParameter<float>( _T("Color_Shift"), &m_fColorShift );
	SetParameterDefaultValue<float>( _T("Color_Shift"), 0.0f );
	testfactor::RFactor rColorShift_2 = AddParameterValue<float>( _T("Color_Shift"), 2.0f );
	testfactor::RFactor rColorShift_0 = AddParameterValue<float>( _T("Color_Shift"), 0.0f );

	testfactor::RFactor rBlendFactors 
		= (rBlendOp_All * rSrcBlend * rDstBlend)
		% (rBlendOpAlpha_All * rSrcBlendAlphaExtra * rDstBlendAlphaExtra);

	testfactor::RFactor rBlendFactors_Simplified 
		= (rBlendOp_NoMinMax * rSrcBlend * rDstBlend_One) 
		% (rBlendOpAlpha_NoMinMax * rSrcBlendAlphaExtra * rDstBlendAlpha_One);

	testfactor::RFactor rBlendAndColorFactors
		= rColorShift_0 * rBlendFactors + rColorShift_2 * rBlendFactors_Simplified;

	// texture format
	AddParameter<DXGI_FORMAT>( _T("RT_Format"), &m_eRTFormats[0] );
	SetParameterDefaultValue( _T("RT_Format"), DXGI_FORMAT_R32G32B32A32_FLOAT );
	tstring sPath = GetPath().ToString( GetPath().Depth() - 1 );
	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(sPath);
	testfactor::RFactor rRTFormatSingle;
	if (pTexFormatInfo)
		rRTFormatSingle = AddParameterValue( _T("RT_Format"), pTexFormatInfo->Format );

	// write mask
	AddParameter<UINT8>( _T("RT_WriteMask"), &m_BlendDesc_1.RenderTarget[0].RenderTargetWriteMask );
	SetParameterDefaultValue( _T("RT_WriteMask"), (UINT8)D3D11_COLOR_WRITE_ENABLE_ALL );
	testfactor::RFactor rRTWriteMask = AddParameterValue( _T("RT_WriteMask"), (UINT8)D3D11_COLOR_WRITE_ENABLE_ALL );
	if (sPath == _T("WriteMask"))
	{
		rRTFormatSingle = AddParameterValue( _T("RT_Format"), DXGI_FORMAT_R32G32B32A32_FLOAT );
		rRTWriteMask 
			= AddParameterValue( _T("RT_WriteMask"), (UINT8)D3D11_COLOR_WRITE_ENABLE_RED )
			+ AddParameterValue( _T("RT_WriteMask"), (UINT8)D3D11_COLOR_WRITE_ENABLE_GREEN )
			+ AddParameterValue( _T("RT_WriteMask"), (UINT8)D3D11_COLOR_WRITE_ENABLE_BLUE )
			+ AddParameterValue( _T("RT_WriteMask"), (UINT8)D3D11_COLOR_WRITE_ENABLE_ALPHA )
			+ AddParameterValue( _T("RT_WriteMask"), (UINT8)(D3D11_COLOR_WRITE_ENABLE_RED|D3D11_COLOR_WRITE_ENABLE_ALPHA) )
			+ AddParameterValue( _T("RT_WriteMask"), (UINT8)(D3D11_COLOR_WRITE_ENABLE_GREEN|D3D11_COLOR_WRITE_ENABLE_BLUE) )
			+ AddParameterValue( _T("RT_WriteMask"), (UINT8)D3D11_COLOR_WRITE_ENABLE_ALL )
			;

		rBlendAndColorFactors = rColorShift_0 * rBlendFactors_Simplified;
	}
	else
	{
		assert(pTexFormatInfo != NULL);
	}

	// number of render targets
	AddParameter<int>( _T("Num_RTs"), &m_NumRTs );
	SetParameterDefaultValue<int>( _T("Num_RTs"), 1 );
	testfactor::RFactor rNumRTs[s_CountNumRenderTargets];
	for (int n = 0; n < s_CountNumRenderTargets; ++n)
		rNumRTs[n] = AddParameterValue<int>( _T("Num_RTs"), s_NumRenderTargets[n] );
	// TODO: cycle through render target number

	// blend-enabled render targets
	AddParameter<int>( _T("RT_Enable"), &m_RTEnableCombo );
	SetParameterDefaultValue<int>( _T("RT_Enable"), 1 );
	testfactor::RFactor rRTEnable[s_CountNumRenderTargets];
	for (int n = 0; n < s_CountNumRenderTargets; ++n)
	{
		if (sPath == _T("WriteMask"))
		{
			rRTEnable[n] 
				= AddParameterValue<int>( _T("RT_Enable"), ((1<<s_NumRenderTargets[n])-1) & 0xBB )
				+ AddParameterValue<int>( _T("RT_Enable"), (s_NumRenderTargets[n])-1 )
				;
		}
		else
			rRTEnable[n] = AddParameterValue<int>( _T("RT_Enable"), s_NumRenderTargets[n] | 0x55 );
	}
	testfactor::RFactor rRTCycle = rNumRTs[0] * rRTEnable[0];
	for (int n = 1; n < s_CountNumRenderTargets; ++n)
		rRTCycle = rRTCycle + rNumRTs[n] * rRTEnable[n];

	SetRootTestFactor( rRTFormatSingle * rRTCycle * rRTWriteMask * rBlendAndColorFactors );

	if (g_App.GetTestMode() == WGFTEST_MODE_BVT)
	{
		SkipMatching(GetSkipMatchingFilter() || (
			   FilterLess(_T("Num_RTs"), 8)
			//|| FilterNotEqual(_T("RT_Enable"), 255)
			//|| FilterNotEqual( _T("RT_WriteMask"), (UINT8)D3D11_COLOR_WRITE_ENABLE_ALL )
			|| FilterNotEqual( _T("Color_Shift"), 2.0f )
			|| (   (FilterEqual( _T("Src_Blend"), D3D11_BLEND_ZERO ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_ONE ))
				|| (FilterEqual( _T("Src_Blend"), D3D11_BLEND_ONE ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_SRC_COLOR ))
				|| (FilterEqual( _T("Src_Blend"), D3D11_BLEND_SRC_COLOR ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_INV_SRC_COLOR ))
				|| (FilterEqual( _T("Src_Blend"), D3D11_BLEND_INV_SRC_COLOR ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_SRC_ALPHA ))
				|| (FilterEqual( _T("Src_Blend"), D3D11_BLEND_SRC_ALPHA ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_INV_SRC_ALPHA ))
				|| (FilterEqual( _T("Src_Blend"), D3D11_BLEND_INV_SRC_ALPHA ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_DEST_ALPHA ))
				|| (FilterEqual( _T("Src_Blend"), D3D11_BLEND_DEST_ALPHA ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_INV_DEST_ALPHA ))
				|| (FilterEqual( _T("Src_Blend"), D3D11_BLEND_INV_DEST_ALPHA ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_SRC_ALPHA_SAT ))
				|| (FilterEqual( _T("Src_Blend"), D3D11_BLEND_SRC_ALPHA_SAT ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_BLEND_FACTOR ))
				|| (FilterEqual( _T("Src_Blend"), D3D11_BLEND_BLEND_FACTOR ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_INV_BLEND_FACTOR ))
				|| (FilterEqual( _T("Src_Blend"), D3D11_BLEND_INV_BLEND_FACTOR ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_SRC1_COLOR ))
				|| (FilterEqual( _T("Src_Blend"), D3D11_BLEND_SRC1_COLOR ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_INV_SRC1_COLOR ))
				|| (FilterEqual( _T("Src_Blend"), D3D11_BLEND_INV_SRC1_COLOR ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_SRC1_ALPHA ))
				|| (FilterEqual( _T("Src_Blend"), D3D11_BLEND_SRC1_ALPHA ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_INV_SRC1_ALPHA ))
				|| (FilterEqual( _T("Src_Blend"), D3D11_BLEND_INV_SRC1_ALPHA ) && FilterNotEqual( _T("Dst_Blend"), D3D11_BLEND_ZERO ))
				)
			));
	}

	// Shader blend
	AddPriorityPatternFilter(FilterEqual<D3D11_BLEND_OP>(pBlendOpParam, D3D11_BLEND_OP_ADD), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<D3D11_BLEND>(pSrcBlendParam, D3D11_BLEND_SRC_ALPHA), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<D3D11_BLEND>(pSrcBlendParam, D3D11_BLEND_INV_SRC_ALPHA), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<D3D11_BLEND>(pSrcBlendParam, D3D11_BLEND_SRC_COLOR), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<D3D11_BLEND>(pSrcBlendParam, D3D11_BLEND_INV_SRC_COLOR), WeightFactorLarge);

	// Alpha blend
	AddPriorityPatternFilter(FilterEqual<D3D11_BLEND_OP>(pBlendOpAlphaParam, D3D11_BLEND_OP_ADD), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<D3D11_BLEND>(pSrcBlendAlphaParam, D3D11_BLEND_SRC_ALPHA), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<D3D11_BLEND>(pSrcBlendAlphaParam, D3D11_BLEND_INV_SRC_ALPHA), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

static
bool
isDualSource(D3D11_BLEND blend)
{
	switch(blend)
	{
	case D3D11_BLEND_SRC1_COLOR:
	case D3D11_BLEND_INV_SRC1_COLOR:
	case D3D11_BLEND_SRC1_ALPHA:
	case D3D11_BLEND_INV_SRC1_ALPHA:
		return true;
	default:
		return false;
	}
}

static
D3D11_BLEND
colorToAlpha(D3D11_BLEND colorBlend)
{
	switch(colorBlend)
	{
	default:
		return colorBlend;
	case D3D11_BLEND_SRC_COLOR:
		return D3D11_BLEND_SRC_ALPHA;
	case D3D11_BLEND_INV_SRC_COLOR:
		return D3D11_BLEND_INV_SRC_ALPHA;
	case D3D11_BLEND_DEST_COLOR:
		return D3D11_BLEND_DEST_ALPHA;
	case D3D11_BLEND_INV_DEST_COLOR:
		return D3D11_BLEND_INV_DEST_ALPHA;
	case D3D11_BLEND_SRC1_COLOR:
		return D3D11_BLEND_SRC1_ALPHA;
	case D3D11_BLEND_INV_SRC1_COLOR:
		return D3D11_BLEND_INV_SRC1_ALPHA;
	}
}

TEST_RESULT
CBasicBlendTest::SetupTestCase()
{
	HRESULT hr = S_OK;
	TEST_RESULT tRes = CBlendConfTest::SetupTestCase();
	if (tRes != RESULT_PASS)
		return tRes;

	for (int n = 1; n < 8; ++n)
		m_eRTFormats[n] = m_eRTFormats[0];

	m_BlendDesc_1.RenderTarget[0].SrcBlendAlpha = colorToAlpha(m_BlendDesc_1.RenderTarget[0].SrcBlendAlpha);
	m_BlendDesc_1.RenderTarget[0].DestBlendAlpha = colorToAlpha(m_BlendDesc_1.RenderTarget[0].DestBlendAlpha);

	bool bDualSource_Src = isDualSource(m_BlendDesc_1.RenderTarget[0].SrcBlend);
	bool bDualSource_Dst = isDualSource(m_BlendDesc_1.RenderTarget[0].DestBlend);
	bool bDualSource = bDualSource_Src || bDualSource_Dst;

	bool bDualSourceAlpha_Src = isDualSource(m_BlendDesc_1.RenderTarget[0].SrcBlendAlpha);
	bool bDualSourceAlpha_Dst = isDualSource(m_BlendDesc_1.RenderTarget[0].DestBlendAlpha);
	bool bDualSourceAlpha = bDualSourceAlpha_Src || bDualSourceAlpha_Dst;

	bool bDestBlendAlphaSat = 
		m_BlendDesc_1.RenderTarget[0].DestBlend == D3D11_BLEND_SRC_ALPHA_SAT ||
		m_BlendDesc_1.RenderTarget[0].DestBlendAlpha == D3D11_BLEND_SRC_ALPHA_SAT;

	if( m_bFL9X )
	{
		if( bDestBlendAlphaSat )
		{
			WriteToLog( _T("D3D_FEATURE_LEVEL_9_* does not support D3D11_BLEND_SRC_ALPHA_SAT for DestBlend or DestBlendAlpha.") );
			return RESULT_SKIP;
		}

		if( bDualSource || bDualSourceAlpha )
		{
			WriteToLog( _T("D3D_FEATURE_LEVEL_9_* does not support any dual-source blending.") );
			return RESULT_SKIP;
		}
	}

	if (bDualSource || bDualSourceAlpha)
	{
		if (m_NumRTs > 1)
		{
			if (bDualSource_Src)
			{
				SkipMatching(GetSkipMatchingFilter() || (
					FilterEqual(_T("Num_RTs"), m_NumRTs)
					&& FilterEqual(_T("Src_Blend"), m_BlendDesc_1.RenderTarget[0].SrcBlend)
					));
			}
			if (bDualSource_Dst)
			{
				SkipMatching(GetSkipMatchingFilter() || (
					FilterEqual(_T("Num_RTs"), m_NumRTs)
					&& FilterEqual(_T("Dst_Blend"), m_BlendDesc_1.RenderTarget[0].DestBlend)
					));
			}
			if (bDualSourceAlpha_Src)
			{
				SkipMatching(GetSkipMatchingFilter() || (
					FilterEqual(_T("Num_RTs"), m_NumRTs)
					&& FilterEqual(_T("Src_Blend_A"), m_BlendDesc_1.RenderTarget[0].SrcBlendAlpha) 
					));
			}
			if (bDualSourceAlpha_Dst)
			{
				SkipMatching(GetSkipMatchingFilter() || (
					FilterEqual(_T("Num_RTs"), m_NumRTs)
					&& FilterEqual(_T("Dst_Blend_A"), m_BlendDesc_1.RenderTarget[0].DestBlendAlpha)
					));
			}
			WriteToLog( _T("Cannot use dual-source blending with multiple render targets") );
			tRes = RESULT_SKIP;
			goto testDone;
		}
		bool bInvalidDualSourceMinMax = false;
		if (  bDualSource 
			&& (   m_BlendDesc_1.RenderTarget[0].BlendOp == D3D11_BLEND_OP_MIN
				|| m_BlendDesc_1.RenderTarget[0].BlendOp == D3D11_BLEND_OP_MAX)
			)
		{
			bInvalidDualSourceMinMax = true;
			if (bDualSource_Src)
			{
				SkipMatching(GetSkipMatchingFilter() || (
					FilterEqual(_T("Src_Blend"), m_BlendDesc_1.RenderTarget[0].SrcBlend) 
					&& (FilterEqual(_T("Blend_Op"), D3D11_BLEND_OP_MIN) || FilterEqual(_T("Blend_Op"), D3D11_BLEND_OP_MAX)) 
					));
			}
			if (bDualSource_Dst)
			{
				SkipMatching(GetSkipMatchingFilter() || (
					FilterEqual(_T("Dst_Blend"), m_BlendDesc_1.RenderTarget[0].DestBlend)
					&& (FilterEqual(_T("Blend_Op"), D3D11_BLEND_OP_MIN) || FilterEqual(_T("Blend_Op"), D3D11_BLEND_OP_MAX)) 
					));
			}
		}
		if (   bDualSourceAlpha 
			&& (   m_BlendDesc_1.RenderTarget[0].BlendOpAlpha == D3D11_BLEND_OP_MIN
				|| m_BlendDesc_1.RenderTarget[0].BlendOpAlpha == D3D11_BLEND_OP_MAX)
			)
		{
			bInvalidDualSourceMinMax = true;
			if (bDualSourceAlpha_Src)
			{
				SkipMatching(GetSkipMatchingFilter() || (
					FilterEqual(_T("Src_Blend_A"), m_BlendDesc_1.RenderTarget[0].SrcBlendAlpha) 
					&& (FilterEqual(_T("Blend_Op_A"), D3D11_BLEND_OP_MIN) || FilterEqual(_T("Blend_Op_A"), D3D11_BLEND_OP_MAX)) 
					));
			}
			if (bDualSourceAlpha_Dst)
			{
				SkipMatching(GetSkipMatchingFilter() || (
					FilterEqual(_T("Dst_Blend_A"), m_BlendDesc_1.RenderTarget[0].DestBlendAlpha)
					&& (FilterEqual(_T("Blend_Op_A"), D3D11_BLEND_OP_MIN) || FilterEqual(_T("Blend_Op_A"), D3D11_BLEND_OP_MAX)) 
					));
			}
		}
		if (bInvalidDualSourceMinMax)
		{
			WriteToLog( _T("Cannot use dual-source blending with MIN or MAX") );
			tRes = RESULT_SKIP;
			goto testDone;
		}
		m_sPSEntryPt = _T("main_ds");
	}
	else
	{
		m_sPSEntryPt = _T("main");
	}

	for (int n = 0; n < m_NumRTs; ++n)
	{
		UINT formatSupport;
		m_pDevice1->CheckFormatSupport(m_eRTFormats[n], &formatSupport);
		if (0 == (formatSupport & D3D11_FORMAT_SUPPORT_BLENDABLE))
		{
			WriteToLog( _T("[CBasicBlendTest::ExecuteTestCase] format does not support blending.") );
			tRes = RESULT_SKIP;
			SkipMatching( FilterEqual<DXGI_FORMAT>( _T("RT_Format"), m_eRTFormats[n] ) );
			goto testDone;
		}
	}

	// If the formats being used are video formats, then the effective formats will differ. 
	// E.g. RTV and SRV formats will be these effective formats while the underlying resource will be the intended format.
	for (int n = 0; n < m_NumRTs; ++n)
	{
		// By default it is the same format...
		m_eRTFormatsEffectiveRTV[n] = m_eRTFormats[n];
		m_eRTFormatsEffectiveSRV[n] = DXGI_FORMAT_UNKNOWN;

		// If it's video, update it...
		if( CD3D10FormatHelper_Test::YUV( m_eRTFormats[n] ) )
		{
			std::vector<DXGI_FORMAT> luminanceDataFormats, chrominanceDataFormats;

			//
			// The format is a video format, see if it supports RTVs
			//

			if( CFormatHelper::GetRTViewFormatsForVideoResourceFormat( m_eRTFormats[n], luminanceDataFormats, chrominanceDataFormats ) )
			{
				// The format is a video format AND supports RTVs, so we need to determine the correct view format...
				// BUGBUG, we will just get luminance coverage for formats that split the views. Even then, we only get the first format that is blendable.
				assert( luminanceDataFormats.size() >= 1 );

				// Find the first blendable format...
				bool bFoundBlendableFormat = false;
				for( UINT i = 0; i < luminanceDataFormats.size(); i++ )
				{
					UINT formatSupport = 0;
					if( FAILED( hr = m_pDevice1->CheckFormatSupport( luminanceDataFormats[i], &formatSupport ) ) )
					{
						LogError( __FILEW__, __LINE__, L"CheckFormatSupport() failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCheckFormatSupport );
						tRes = RESULT_FAIL;
						goto testDone;
					}

					// Is the format blendable?
					if( formatSupport & D3D11_FORMAT_SUPPORT_BLENDABLE )
					{
						m_eRTFormatsEffectiveRTV[n] = luminanceDataFormats[i];
						WriteToLog( _T("RTV #%d's format is %s."), n, ToString(m_eRTFormatsEffectiveRTV[n]).c_str() );
						bFoundBlendableFormat = true;
						break;
					}
				}

				if( !bFoundBlendableFormat )
				{
					WriteToLog( _T("The video format, %s, supports a blendable RT, so at least one RTV format must support blending ... but we didn't find one. This may be a test or runtime issue."), ToString(m_eRTFormats[n]).c_str() );
					LogError( __FILEW__, __LINE__, L"Failed to find a blendable RTV format for the video resource.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
					tRes = RESULT_FAIL;
					goto testDone;
				}
			}
			else
			{
				LogError( __FILEW__, __LINE__, L"Video format doesn't support RTVs.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConflictingParameters );
				tRes = RESULT_FAIL;
				goto testDone;
			}

			//
			// The format is a video format, see if it supports SRVs
			//

			if( CFormatHelper::GetSRViewFormatsForVideoResourceFormat( m_eRTFormats[n], luminanceDataFormats, chrominanceDataFormats ) )
			{
				// The format is a video format AND supports SRVs, so we need to determine the correct view format...
				// BUGBUG, we will just get luminance coverage for formats that split the views. Even then, we only use the same format as the RTV
				assert( luminanceDataFormats.size() >= 1 );

				// Find the first blendable format...
				bool bFoundSameFormat = false;
				for( UINT i = 0; i < luminanceDataFormats.size(); i++ )
				{
					if( luminanceDataFormats[i] == m_eRTFormatsEffectiveRTV[n] )
					{
						m_eRTFormatsEffectiveSRV[n] = luminanceDataFormats[i];
						WriteToLog( _T("SRV #%d's format is %s."), n, ToString(m_eRTFormatsEffectiveSRV[n]).c_str() );
						bFoundSameFormat = true;
						break;
					}
				}

				if( !bFoundSameFormat )
				{
					LogError( __FILEW__, __LINE__, L"Failed to match the SRV format to the RTV format.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCLogicError );
					tRes = RESULT_FAIL;
					goto testDone;
				}
			}
			else
			{
				LogError( __FILEW__, __LINE__, L"Video format doesn't support SRVs.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConflictingParameters );
				tRes = RESULT_FAIL;
				goto testDone;
			}
		}
	}

	hr = UpdateResources();
	if (FAILED(hr))
	{
		tRes = RESULT_FAIL;
		goto testDone;
	}

testDone:
	return tRes;
}

TEST_RESULT 
CBasicBlendTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = E_FAIL;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	TEST_VEC4F dstVertColors[4], srcVertColors[2][4], blendFactor;
	ZeroMemory(&dstVertColors, sizeof(dstVertColors));
	ZeroMemory(&srcVertColors, sizeof(srcVertColors));
	blendFactor = m_vBlendFactor;

	{
		// basic setup:
		// dst = horizontal gradient, 0 at left, 1 at right
		// src0 = vertical gradient, 0 at top, 1 at bottom
		// src1 = diagonal gradient, 0 at top-left, 1 at bottom-right
		TEST_VEC4F vZero = {0,0,0,0};
		TEST_VEC4F vOne = {1,1,1,1};
		vZero = vZero - (vOne * m_fColorShift);
		vOne = vOne + (vOne * m_fColorShift);
		dstVertColors[0] = dstVertColors[3] = vZero;
		dstVertColors[1] = dstVertColors[2] = vOne;
		srcVertColors[0][0] = srcVertColors[0][1] = vZero;
		srcVertColors[0][2] = srcVertColors[0][3] = vOne;
		srcVertColors[1][0] = vOne;
		srcVertColors[1][1] = srcVertColors[1][3] = vZero + ((vOne - vZero) * 0.5f);
		srcVertColors[1][2] = vZero;
		blendFactor.x -= 2*m_fColorShift;
		blendFactor.y += m_fColorShift;
		blendFactor.z -= 2*m_fColorShift;
		blendFactor.w += m_fColorShift;

		for (int n = 0; n < 4; ++n)
		{
			dstVertColors[n] *= 0.5;
			dstVertColors[n].f[n] *= 2;
			srcVertColors[0][3-n] *= 0.5;
			srcVertColors[0][3-n].f[n] *= 2;
			srcVertColors[1][(n+2)%4] *= 0.5;
			srcVertColors[1][(n+2)%4].f[n] *= 2;
		}
	}

	// create and set viewport
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float) m_nPrimitiveW;
	vp.Height   = (float) m_nPrimitiveH;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	m_pDeviceContext1->RSSetViewports(1, &vp);

	// set up render targets & clear them
	ID3D11RenderTargetView* pViews[8];
	ZeroMemory(pViews, sizeof(pViews));
	memcpy(pViews, m_pRTViews, min(8,m_NumRTs)*sizeof(ID3D11RenderTargetView*));
	const INT maxRTsToSet = (GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_0) ? 8 : ( (GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_9_3) ? 4 : 1 );
	m_pDeviceContext1->OMSetRenderTargets( min(m_NumRTs, maxRTsToSet), pViews, NULL );
	for (int rtNum = 0; rtNum < m_NumRTs; ++rtNum)
	{
		FLOAT colors[4] = { 0.1f, 0.2f, 0.3f, 0.4f };
		if (m_pRTViews[rtNum] != NULL)
		{
			m_pDeviceContext1->ClearRenderTargetView( m_pRTViews[rtNum], colors );
		}
	}

	// draw the dest quad
	if ( FAILED(DrawQuad(dstVertColors, false, blendFactor)) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("[CBasicBlendTest::ExecuteTestCase] DrawQuad(dst) failed. hr=%s"), D3DHResultToString(hr).c_str() );
		goto testDone;
	}

	// draw the src quad
	if ( FAILED(DrawQuad(srcVertColors[0], true, blendFactor, srcVertColors[1])) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("[CBasicBlendTest::ExecuteTestCase] DrawQuad(src) failed. hr=%s"), D3DHResultToString(hr).c_str() );
		goto testDone;
	}

	// execute any deferred operations
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		tRes = RESULT_FAIL;
		WriteToLog("ExecuteEffectiveContext failed. hr = %s", D3DHResultToString(hr).c_str() );
		goto testDone;
	}

	// test that things came out as expected
	if (tRes == RESULT_PASS)
		tRes = TestDrawnPixels(dstVertColors, srcVertColors, blendFactor);

	if (g_App.VisibleRender())
		DoVisibleRender();
	DoShowResources();

	{
		// clear out the resource bindings
		ID3D11ShaderResourceView* nullSRVArr[1] = {NULL};
		m_pDeviceContext1->PSSetShaderResources(0, 1, nullSRVArr);
	}

testDone:
	return tRes;
}

