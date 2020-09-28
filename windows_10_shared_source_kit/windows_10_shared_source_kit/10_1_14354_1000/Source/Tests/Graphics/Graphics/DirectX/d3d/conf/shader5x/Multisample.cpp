/////////////////////////////////////////////////////////////////////////
//  Multisample.cpp
//  created:	2005/06/29
//
//  purpose: Tests multisample related Shader5x issues
//////////////////////////////////////////////////////////////////////////

#include "Multisample.h"
#include <D3DMSHelper.h>
#include <MinMaxRange.h>

struct VERTEX
{
	D3DXVECTOR2 pos;
	D3DXVECTOR4 data;
};

// Vertex shader code
//  input: float2 position, float4 texcoord
//	output: float4 position, float4 texcoord
//
//  description: takes a vertex in screen space and applies an inverse
//				 viewport transformation
//
const char szVS[] =
	"dcl_input v[0].xy"										"\n"
	"dcl_input v[1].xyzw"									"\n"
	"dcl_output_sv  o[0].xyzw, position"					"\n"
	"dcl_output o[1].xyzw"									"\n"
	"dcl_constantBuffer  c0[1], immediateIndexed"			"\n"
	"mad o[0].xy, v[0].xyxx, c0[0].zwzz, c0[0].xyxx"		"\n"
	"mov o[0].zw, vec4( 0.0, 0.0, 0.0, 1.0 )"				"\n"
	"mov o[1].xyzw, v[1].xyzw"								"\n"
;

// Vertex shader code
//  input: float4 position, float4 texcoord
//	output: float4 position, float4 texcoord, float rt_array_index
//
//  description: passes through VS results and chooses rt array index
//
const char szGS[] =
	"dcl_inputPrimitive point"		"\n"
	"dcl_outputTopology pointlist"	"\n"
	"dcl_maxOutputVertexCount 1"	"\n"
	"dcl_constantBuffer c0[2], immediateIndexed"		"\n"
	"dcl_temps 6"					"\n"
	"dcl_input_sv v[1][0].xyzw, position"	"\n" // Result from previous stage
	"dcl_input v[1][1].xyzw"		"\n"		 // Result from previous stage
	"dcl_output_sv o[0].xyzw, position"			"\n"
	"dcl_output o[1].xyzw"			"\n"
	"dcl_output_sv o[2].x, renderTargetArrayIndex"	"\n" // render target array index
	"mov o[0].xyzw, v[0][0].xyzw"	"\n" // copy pos
	"mov o[1].xyzw, v[0][1].xyzw"	"\n" // copy data
	"mov o[2].x, c0[1].x"			"\n" // get array index from constant data
	"emit"							"\n"
	;

// Pixel shader code - needs to  be constructed
//  input: float4 texcoord
//	string (mov, ftou, ftoi)
const char szPSPass[] =
	"dcl_input_ps v[1].xyzw, constant"  "\n"
	"dcl_output o[0].xyzw"				"\n"
	"%s o[0].xyzw, v[1].xyzw"			"\n"
;

// Pixel shader code - needs to  be constructed
//  output: float4
//	UINT (optional sample count enclosed in ()), string (view dimension), string (type),
//  4x uint (srcaddress), offsetstring (mov, utof, itof), (sampleIndex or r1.x)
const char szPS[] =
	"dcl_output o[0].xyzw"								"\n"
	"dcl_resource%s t0, %s, %s"							"\n"
	"dcl_resource t1, Texture2D, UINT"					"\n"
	"dcl_temps 2"										"\n"
	"ld r1.xyzw, vec4(0,0,0,0), t1.xyzw"				"\n"
	"ld2dms r0.xyzw, vec4(%d, %d, %d, %d), t0.xyzw, %s"	"\n"
	"%s o[0].xyzw, r0.xyzw"								"\n"
;

const char szPSResidencyFeedback[] =
	"dcl_output o[0].xyzw"								"\n"
	"dcl_resource%s t0, %s, %s"							"\n"
	"dcl_resource t1, Texture2D, UINT"					"\n"
	"dcl_temps 3"										"\n"
	"ld r1.xyzw, vec4(0,0,0,0), t1.xyzw"				"\n"
	"ldms_s r0.xyzw, r2.x, vec4(%d, %d, %d, %d), t0.xyzw, %s"	"\n"
	"check_access_fully_mapped r2.y, r2.x"				"\n" // This line and the next two force the driver not to optimize out the otherwise unused residency feedback result
	"xor r2.y, r2.y, 0xffffffff"						"\n" // Invert the output of check_access_fully_mapped
	"xor r0.x, r0.x, r2.y"								"\n" // When fully resident r2.y should be 0x00000000 (leaving r0.x unchanged)
	"%s o[0].xyzw, r0.xyzw"								"\n"
;

DXGI_FORMAT MultisampleFormatsSmall[] =
{
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R16G16B16A16_SNORM,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_R16_SINT
};

DXGI_FORMAT MultisampleFormats[] =
{
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_SINT,
	DXGI_FORMAT_R32G32B32A32_UINT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32_SINT,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_R16G16B16A16_UINT,
	DXGI_FORMAT_R16G16B16A16_SNORM,
	DXGI_FORMAT_R16G16B16A16_SINT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32_SINT,
	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_R10G10B10A2_UINT,
	DXGI_FORMAT_R11G11B10_FLOAT,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	DXGI_FORMAT_R8G8B8A8_UINT,
	DXGI_FORMAT_R8G8B8A8_SNORM,
	DXGI_FORMAT_R8G8B8A8_SINT,
	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_R16G16_SNORM,
	DXGI_FORMAT_R16G16_SINT,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32_SINT,
	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_R8G8_UINT,
	DXGI_FORMAT_R8G8_SNORM,
	DXGI_FORMAT_R8G8_SINT,
	DXGI_FORMAT_R16_FLOAT,
	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R16_SNORM,
	DXGI_FORMAT_R16_SINT,
	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R8_UINT,
	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_R8_SINT
};

UINT Widths[] =
{
	1,
	7,
	11,
	128
};

UINT Heights[] =
{
	1,
	3,
	17,
	1
};

UINT ArraySizes[] =
{
	2,
	5
};

UINT SrcAddressList[][4] =
{
	{ 1, 0, 0, 0 },
	{ 7, 0, 0, 0 },
	{ 0, 3, 0, 0 },
	{ 11, 4, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 1, 2, 0 },
	{ 7, 0, 1, 0 },
	{ 1, 2, 5, 0 }
};

CMSLoadTest::CMSLoadTest()
: m_pGS(NULL),
  m_pMSRT(NULL),
  m_pRT(NULL),
  m_pRTStaging(NULL),
  m_pTex2DIndex(NULL),
  m_pMSRTV(NULL),
  m_pRTV(NULL),
  m_pSRV(NULL),
  m_pSRVIndex(NULL),
  m_pPSPass(NULL),
  m_pVBSamples(NULL),
  m_pVBQuad(NULL),
  m_pLayout(NULL),
  m_uSampleIndex(0)
{

}

void CMSLoadTest::InitTestParameters()
{
	//
	// Get Sample Count
	//

	// Parse the SampleCount from the group name.
	// Later this will be used to set the default value for
	// parameter "SampleCount"
	tstring szPath = GetPath().ToString( GetPath().Depth() - 2, 1);
	UINT uSampleCount = atoi(szPath.c_str());
	m_Tex2DDesc.SampleDesc.Count = uSampleCount;

	szPath = GetPath().ToString( GetPath().Depth() - 1 );
	UINT uMaxQualityLevel = atoi(szPath.c_str());

	//
	// Format
	//

	CTableValueSet< DXGI_FORMAT > *pVSFormat;
	// !Speedup
	if (m_Tex2DDesc.SampleDesc.Count == 2)
		pVSFormat = new CTableValueSet< DXGI_FORMAT >( MultisampleFormats, sizeof(DXGI_FORMAT), ARRAY_SIZE(MultisampleFormats) );
	else
		pVSFormat = new CTableValueSet< DXGI_FORMAT >( MultisampleFormatsSmall, sizeof(DXGI_FORMAT), ARRAY_SIZE(MultisampleFormatsSmall) );
	CTestCaseParameter< DXGI_FORMAT > *pFormatParam = AddParameter( _T( "Format" ), &m_Tex2DDesc.Format );
	testfactor::RFactor rfFormat = AddParameterValueSet< DXGI_FORMAT > ( pFormatParam, pVSFormat );

	//
	// Texture Dimension
	//

	CTestCaseParameter<D3D11_RTV_DIMENSION> *pDimParam = AddParameter( _T( "Dimension" ), &m_RTVDesc.ViewDimension );
	testfactor::RFactor rfTex2DMS = AddParameterValue( pDimParam, D3D11_RTV_DIMENSION_TEXTURE2DMS );
	testfactor::RFactor rfTex2DMSArray = AddParameterValue( pDimParam, D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY );

	//
	// Texture Declaration
	//

	CTestCaseParameter<bool> *pDeclCountParam = AddParameter( _T( "SpecifyCountInDecl" ), &m_bDeclCount );
	testfactor::RFactor rfDeclCount = AddParameterValue( pDeclCountParam, true ) +
									  AddParameterValue( pDeclCountParam, false );

	//
	// Literal Sample Index
	//

	CTestCaseParameter<bool> *pLiteralIndexParam = AddParameter( _T( "LiteralSampleIndex" ), &m_bLiteralIndex );
	testfactor::RFactor rfLiteralIndex = AddParameterValue( pLiteralIndexParam, true ) +
										 AddParameterValue( pLiteralIndexParam, false );


	//
	// Sample Description
	//

	// sample count and index
	srand(0);
	CTestCaseParameter< UINT > *pSampleCountParam = AddParameter( _T("SampleCount"), &m_Tex2DDesc.SampleDesc.Count );
	// Set the default value of parameter SampleCount to the value
	// parsed from the test group name
	pSampleCountParam->SetDefaultValue(uSampleCount);
	CTestCaseParameter<UINT> *pSampleIndexParam = AddParameter( _T( "SampleIndex" ), &m_uSampleIndex );
	CUserValueSet<UINT> *pSampleIndexSet = new CUserValueSet<UINT>();
	// !Speedup
	pSampleIndexSet->AddValue(0);
	if (m_Tex2DDesc.SampleDesc.Count > 1)
		pSampleIndexSet->AddValue(m_Tex2DDesc.SampleDesc.Count-1);
	if (m_Tex2DDesc.SampleDesc.Count > 8)
		pSampleIndexSet->AddValue(rand()%(m_Tex2DDesc.SampleDesc.Count-2) + 1);
	testfactor::RFactor rfSampleIndex = AddParameterValueSet( pSampleIndexParam, pSampleIndexSet );

	// sample quality
	CTestCaseParameter< UINT > *pQualityParam = AddParameter( _T("SampleQuality"), &m_Tex2DDesc.SampleDesc.Quality );
	CRangeValueSet< UINT > *pQualitySet = new CRangeValueSet< UINT >( 0, uMaxQualityLevel-1, 1 );
	testfactor::RFactor rfSampleMaxQuality = AddParameterValueSet< UINT >( pQualityParam, pQualitySet );

	// special patterns
	CUserValueSet< UINT> *pQualityPatternSet = new CUserValueSet< UINT >();
	pQualityPatternSet->AddValue( D3D11_CENTER_MULTISAMPLE_PATTERN );
	pQualityPatternSet->AddValue( D3D11_STANDARD_MULTISAMPLE_PATTERN );
	testfactor::RFactor rfSamplePatternQuality = AddParameterValueSet< UINT >(pQualityParam, pQualityPatternSet);

	// special patterns only supported for specific sample counts
	testfactor::RFactor rfSampleQuality = rfSampleMaxQuality;
	UINT count = m_Tex2DDesc.SampleDesc.Count;
	if ((count == 1) || (count == 2) || (count == 4) || (count == 8) || (count == 16))
	{
		rfSampleQuality = rfSampleMaxQuality + rfSamplePatternQuality;
	}

	//
	// Width and Height
	//

	// !speedup
	UINT uSizes = ARRAY_SIZE( Heights );
	if (m_Tex2DDesc.SampleDesc.Count > 4)
		uSizes = min( 2, uSizes );

	// height
	CTestCaseParameter< UINT > *pHeightParam = AddParameter( _T("Height"), &m_Tex2DDesc.Height );
	CTableValueSet<UINT> *pHeightValues = new CTableValueSet< UINT >( Heights, sizeof(UINT), uSizes );
	testfactor::RFactor rfHeight = AddParameterValueSet( pHeightParam, pHeightValues );

	// width
	CTestCaseParameter< UINT > *pWidthParam = AddParameter( _T("Width"), &m_Tex2DDesc.Width );
	CTableValueSet<UINT> *pWidthValues = new CTableValueSet< UINT >( Widths, sizeof(UINT), uSizes );
	testfactor::RFactor rfWidth = AddParameterValueSet( pWidthParam, pWidthValues );

	//
	// Src Address
	//

	CTestCaseParameter<UINT> *pSrcAddressXParam = AddParameter( _T( "SrcAddress_x" ), &m_uSrcAddress[0] );
	CTestCaseParameter<UINT> *pSrcAddressYParam = AddParameter( _T( "SrcAddress_y" ), &m_uSrcAddress[1] );
	CTestCaseParameter<UINT> *pSrcAddressZParam = AddParameter( _T( "SrcAddress_z" ), &m_uSrcAddress[2] );
	testfactor::RFactor rfSrcAddressAll = AddParameterValue( pSrcAddressXParam, (UINT)0 ) *
										  AddParameterValue( pSrcAddressYParam, (UINT)0 ) *
										  AddParameterValue( pSrcAddressZParam, (UINT)0 );
	for (UINT i = 0; i < ARRAY_SIZE(SrcAddressList); ++i)
		rfSrcAddressAll = rfSrcAddressAll + (AddParameterValue( pSrcAddressXParam, SrcAddressList[i][0] ) *
											AddParameterValue( pSrcAddressYParam, SrcAddressList[i][1] ) *
											AddParameterValue( pSrcAddressZParam, SrcAddressList[i][2] ));

	//
	// Arrays
	//

	CTestCaseParameter<UINT> *pArraySizeParam = AddParameter( _T( "ArraySize" ), &m_Tex2DDesc.ArraySize );
	CTestCaseParameter<UINT> *pArraySliceParam = AddParameter( _T( "ArraySlice" ), &m_uArraySlice );

	pArraySizeParam->SetDefaultValue(1);
	pArraySliceParam->SetDefaultValue(0);

	UINT uArrayStart = 1;
	UINT uSliceStart = 0;
	// !Speedup
	if (m_Tex2DDesc.SampleDesc.Count == 2)
	{
		uArrayStart = 2;
		uSliceStart = 1;
	}
	testfactor::RFactor rfArrayAll = AddParameterValue( pArraySizeParam, uArrayStart ) * AddParameterValue( pArraySliceParam, uSliceStart );
	for (UINT i = 0; i < ARRAY_SIZE( ArraySizes ); ++i)
	{
		// !Speedup
		if ((m_Tex2DDesc.SampleDesc.Count > 8) && (i > 0)) continue;

		// !Speedup
		if (m_Tex2DDesc.SampleDesc.Count == 2) break;

		// add only a few slices
		UINT uBeg = 0;
		UINT uEnd = ArraySizes[i]-1;
		UINT uMid = (ArraySizes[i]-1)/2;

		// don't add dupes
		CUserValueSet<UINT> *pArraySliceSet = new CUserValueSet<UINT>();
		pArraySliceSet->AddValue( uBeg );
		if ( uEnd > uBeg )
			pArraySliceSet->AddValue( uEnd );
		if ( uMid > uBeg )
			pArraySliceSet->AddValue( uMid );

		rfArrayAll = rfArrayAll + ( AddParameterValue( pArraySizeParam, ArraySizes[i] ) *
									AddParameterValueSet( pArraySliceParam, pArraySliceSet ) );
	}
	testfactor::RFactor rfArrayCube = AddParameterValue( pArraySizeParam, (UINT)6 ) *
									  AddParameterValueSet( pArraySliceParam, new CRangeValueSet<UINT>( 0, 5, 1 ) );

	//
	// Create Root
	//

	// Utilize pairwise to reduce the number of testcases
	testfactor::RFactor Tex2DMS = (rfTex2DMS + (rfArrayAll * rfTex2DMSArray)) * (rfHeight % rfWidth) * rfLiteralIndex * rfSrcAddressAll;
	testfactor::XFACTOR pairwiseTable[] =
	{
		// Factor, Group, pNotGrps, cNotGrps
		{rfFormat, 1, NULL, 0},
		{rfSampleQuality, 1, NULL, 0},
		{rfSampleIndex, 1, NULL, 0},
		{Tex2DMS, 1, NULL, 0},
	};

	size_t groupOrders[] =
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	testfactor::RFactor rfTestCases = rfDeclCount * NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "Pairwise", groupOrders, _countof(groupOrders));
	SetRootTestFactor( rfTestCases );

	//
	// Filters
	//

	CFilterHandle filter;
	// options include 10.0, 10.1 + force SM4.0, 10.1 + SM4.1
	if ( (GetFramework()->m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_0) || g_bForceSM40 )
	{
		// in shader4.0 you must specify the sample count in the shader decl
		filter = FilterEqual<bool>( _T("SpecifyCountInDecl"), false );

		// in shader4.0 the ld2dms sample index must be a literal
		filter = filter || FilterEqual<bool>( _T("LiteralSampleIndex"), false );

		//
		filter = filter || FilterEqual<UINT>( _T("SampleQuality"), D3D11_CENTER_MULTISAMPLE_PATTERN ) ||
						   FilterEqual<UINT>( _T("SampleQuality"), D3D11_STANDARD_MULTISAMPLE_PATTERN );
	}


	// speedup filters

	// summary: cylce on "specify count in decl" and "literal sample index" if:
	//			(width == 1) || (width == 7)
	SkipMatching( filter ||
				  (
					( FilterEqual<bool>( _T("SpecifyCountInDecl"), false ) || FilterEqual<bool>( _T("LiteralSampleIndex"), false ) ) &&
					( FilterNotEqual<UINT>( _T("Width"), 1 ) && FilterNotEqual<UINT>( _T("Width"), 7 ) )
				  )
				);

	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R8G8B8A8_UNORM), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R32G32B32A32_FLOAT), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R10G10B10A2_UNORM), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R8G8B8A8_TYPELESS), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R32G32B32A32_TYPELESS), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R10G10B10A2_TYPELESS), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<bool>(_T("LiteralSampleIndex"), true), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), 0), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), uMaxQualityLevel - 1), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), D3D11_STANDARD_MULTISAMPLE_PATTERN), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), D3D11_CENTER_MULTISAMPLE_PATTERN), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Width"), 128), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Height"), 17), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ArraySize"), 2), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 5.0f );
	DeclarePriorityLevelByPriorityValue( 2, 2.0f );
}

TEST_RESULT CMSLoadTestResidencyFeedback::Setup()
{
	auto const tr = CMSLoadTest::Setup();
	if (!TiledOpsSupported())
	{
		WriteToLog(_T("CMSLoadTestResidencyFeedback::Setup - Tiled resource ops not supported."));
		return RESULT_SKIP;
	}
	return tr;
}

//
// Setup - Creates shaders and buffers that remain as the same
//		   for each test case.
//
TEST_RESULT CMSLoadTest::Setup()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	try
	{

		//
		// Skip sample count if no formats support multisample
		//
		for (UINT uFormat = 0; uFormat < ARRAY_SIZE( MultisampleFormats ); ++uFormat)
		{
			UINT uLevels;
			GetDevice()->CheckMultisampleQualityLevels( MultisampleFormats[uFormat], m_Tex2DDesc.SampleDesc.Count, &uLevels);
			const bool bIsValidQualityRange = m_Tex2DDesc.SampleDesc.Quality < uLevels;
			const bool bIsD3D10_1SamplePattern = ( GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_1 ) &&
				( m_Tex2DDesc.SampleDesc.Quality == D3D11_STANDARD_MULTISAMPLE_PATTERN || m_Tex2DDesc.SampleDesc.Quality == D3D11_CENTER_MULTISAMPLE_PATTERN );

			if( uLevels == 0 ||											// If this method returns 0, the format and sample count combination is not supported for the installed adapter
				( !bIsValidQualityRange && !bIsD3D10_1SamplePattern ) )	// The valid range is between zero and one less than the level returned by ID3D11Device::CheckMultiSampleQualityLevels, or one of the sample patterns
			{
				SkipMatching( GetSkipMatchingFilter() || FilterEqual<DXGI_FORMAT>( _T("Format"), MultisampleFormats[uFormat] ) );
			}
		}

		//
		//Set Rasterizer state
		//
		D3D11_RASTERIZER_DESC rastDesc;
		rastDesc.CullMode = D3D11_CULL_NONE;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.MultisampleEnable = true;
		rastDesc.FrontCounterClockwise = true;
		rastDesc.DepthBias = false;
		rastDesc.DepthBiasClamp = 0;
		rastDesc.SlopeScaledDepthBias = 0;
		rastDesc.DepthClipEnable = true;
		rastDesc.ScissorEnable = false;
		rastDesc.AntialiasedLineEnable = false;
		if( FAILED( hr = GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSEnabled ) ) )
			throw TRException("CreateRasterizerState for m_pRSEnabled failed.", hr, RESULT_FAIL);

		GetEffectiveContext()->RSSetState( m_pRSEnabled );
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		//
		//Set DepthStencil state
		//
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		dsDesc.DepthEnable = FALSE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.StencilEnable = FALSE;
		if( FAILED( hr = GetDevice()->CreateDepthStencilState ( &dsDesc, &m_pDSDisabled ) ) )
			throw TRException("CreateDepthStencilState() failed.", hr, RESULT_FAIL);
		GetEffectiveContext()->OMSetDepthStencilState( m_pDSDisabled, 0 );

		//
		// Blend State
		//
		GetEffectiveContext()->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );

		//
		// Create the constant buffer
		//
		D3D11_BUFFER_DESC bufdesc;
		bufdesc.ByteWidth = CONST_BUFFER_SIZE;
		bufdesc.Usage = D3D11_USAGE_DYNAMIC;
		bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufdesc.MiscFlags = 0;
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufdesc, NULL, &m_pConstBuffer ) ) )
			throw TRException("CreateBuffer for m_pConstBuffer failed.", hr, RESULT_FAIL);
		GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );

		//
		// Create vertex buffer
		//
		D3D11_BUFFER_DESC bufferDesc =
		{
			6 * sizeof( VERTEX ),
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_VERTEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE,
			0
		};
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pVBQuad ) ) )
		{
			WriteToLog( _T( "CreateBuffer() result code '%s'" ), D3DHResultToString( hr ).c_str() );
			throw TRException("[CMSLoadTest::SetupVB] CreateBuffer failed for vertex buffer.", hr, RESULT_FAIL);
		}
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		if (FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("[CMSLoadTest::SetupVB] ExecuteEffectiveContext failed.", hr, RESULT_FAIL);
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMSLoadTest::Setup()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}

	CheckTiledSupport();

	return tRes;
}

//
// SetupTestCase - Prepares the MSRT with test data and sets the shaders
//				   for ld2dms.
//
TEST_RESULT CMSLoadTest::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	try
	{
		//
		// Textures
		//
		SetupTextures();

		//
		// Shaders
		//
		SetupShaders();

		//
		// Setup the default viewport
		//
		D3D11_VIEWPORT vp;
		vp.Height = (float)m_Tex2DDesc.Height;
		vp.Width = (float)m_Tex2DDesc.Width;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		GetEffectiveContext()->RSSetViewports( 1, &vp );

		//
		// setup const data
		//
		float bufData[4];
		// Inverse viewport scale.
		bufData[2] = 2 / (float)m_Tex2DDesc.Width;
		bufData[3] = -2 / (float)m_Tex2DDesc.Height;
		// Prescaled inverse viewport translation.
		bufData[0] = -1.0f;
		bufData[1] = 1.0f;
		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, bufData ) ) )
			throw TRException("Map on m_pConstBuffer Failed", hr, RESULT_FAIL);

		//
		// Create vertex buffer
		//
		D3D11_BUFFER_DESC bufferDesc =
		{
			m_Tex2DDesc.ArraySize * m_Tex2DDesc.Width * m_Tex2DDesc.Height * m_Tex2DDesc.SampleDesc.Count * sizeof( VERTEX ),
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_VERTEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE,
			0
		};
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pVBSamples ) ) )
		{
			WriteToLog( _T( "CreateBuffer() result code '%s'" ), D3DHResultToString( hr ).c_str() );
			throw TRException("[CMSLoadTest::SetupVB] CreateBuffer failed for vertex buffer.", hr, RESULT_FAIL);
		}

		//
		// Fill VBQuad
		//
		VERTEX pVert[4];
		pVert[0].pos = D3DXVECTOR2( 0, 0 );
		pVert[1].pos = D3DXVECTOR2( (float)m_Tex2DDesc.Width, 0 );
		pVert[2].pos = D3DXVECTOR2( 0, (float)m_Tex2DDesc.Height );
		pVert[3].pos = D3DXVECTOR2( (float)m_Tex2DDesc.Width, (float)m_Tex2DDesc.Height );

		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pVBQuad, pVert ) ) )
		{
			WriteToLog( _T( "Map() result code '%s'" ), D3DHResultToString( hr ).c_str() );
			throw TRException("[CMSLoadTest::SetupVB] Map on m_pVB failed.", hr, RESULT_FAIL);
		}

		if (FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T( "ExecuteEffectiveContext() failed '%s'" ), D3DHResultToString( hr ).c_str() );
			throw TRException("[CMSLoadTest::SetupVB] ExecuteEffectiveContext failed.", hr, RESULT_FAIL);
		}
	}
	catch (TRException& exc)
	{
		CleanupTestCase();
		tRes = exc.GetResult();
		exc.Prepend("[CMSLoadTest::SetupTestCase()] ");
		if (tRes != RESULT_PASS)
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}

void CMSLoadTest::SetupShaders()
{
	HRESULT hr;
	// Create the shaders that are used often
	//
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;


	try
	{
		//
		// Vertex Shader
		//

		// shader profile, like vs_5_0
		tstring profile = g_TestApp.GetShaderProfile(
			D3D_SHADER_STAGE_VERTEX,
			g_TestApp.m_D3DOptions.FeatureLevel,
			g_TestApp.m_D3DOptions.FeatureLevel);

		profile = profile + " \n" + szVS;

		if( !AssembleShader( profile.c_str(), &pShaderBuf, &pErrorBuf ) )
		{
			if (pErrorBuf != NULL)
			{
				WriteToLog( _T("AssembleShader failed assembling VS: %s" ), pErrorBuf->GetBufferPointer() );
			}
			throw TRException("");
		}
		if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pVS ) ) )
			throw TRException("CreateVertexShader() failed creating default VS", hr, RESULT_FAIL);
		GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );

		//
		// Set the input layout
		//
		D3D11_INPUT_ELEMENT_DESC DeclTL[] =
		{
			{"0_", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},  //position
			{"1_", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},  //texture coordinates
		};
		if(	FAILED(	hr = GetDevice()->CreateInputLayout( DeclTL, sizeof(DeclTL) / sizeof(DeclTL[ 0 ]), pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pLayout ) ) )
			throw TRException("CreateInputLayout() failed.", hr, RESULT_FAIL);
		GetEffectiveContext()->IASetInputLayout( m_pLayout );

		SAFE_RELEASE( pShaderBuf );
		SAFE_RELEASE( pErrorBuf );

		//
		// Geometry Shader
		//

		// shader profile, like vs_5_0
		profile = g_TestApp.GetShaderProfile(
			D3D_SHADER_STAGE_GEOMETRY,
			g_TestApp.m_D3DOptions.FeatureLevel,
			g_TestApp.m_D3DOptions.FeatureLevel);

		profile = profile + " \n" + szGS;

		if( !AssembleShader( profile.c_str(), &pShaderBuf, &pErrorBuf ) )
		{
			if (pErrorBuf != NULL)
			{
				WriteToLog( _T("AssembleShader failed assembling GS: %s" ), pErrorBuf->GetBufferPointer() );
			}
			throw TRException("");
		}

		if( FAILED( hr = GetDevice()->CreateGeometryShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pGS ) ) )
			throw TRException("CreateGeometryShader() failed creating default GS", hr, RESULT_FAIL);

		SAFE_RELEASE( pShaderBuf );
		SAFE_RELEASE( pErrorBuf );

		//
		// Optionally add sample count to the resource declaration
		//
		tstring szDeclCount = _T("");
		if (m_bDeclCount)
			szDeclCount = _T("(") + ToString(m_Tex2DDesc.SampleDesc.Count) + _T(")");

		//
		//	Literal index
		//
		tstring szSampleIndex = ToString( m_uSampleIndex );
		if (!m_bLiteralIndex)
			szSampleIndex = _T("r1.x");  // results from texture load

		//
		// Based on format, insert casting instructions
		//

		// various instrs
		char szMov[] = "mov";
		char szFtoU[] = "ftou";
		char szFtoI[] = "ftoi";
		char szUtoF[] = "utof";
		char szItoF[] = "itof";

		// types
		char szFloat[] = "float";
		char szUint[] = "uint";
		char szInt[] = "sint";

		char *szInstr;
		char *szInstrPass;
		char *szType;

		DWORD flags = GetFlags( m_Tex2DDesc.Format );
		if ((flags & FF_FLOAT) || (flags & FF_UNORM) || (flags & FF_SNORM))
		{
			szInstr = szMov;
			szInstrPass = szMov;
			szType = szFloat;
		}
		else if (flags & FF_UINT)
		{
			szInstr = szUtoF;
			szInstrPass = szFtoU;
			szType = szUint;
		}
		else if (flags & FF_SINT)
		{
			szInstr = szItoF;
			szInstrPass = szFtoI;
			szType = szInt;
		}
		else
			throw TRException("Format type flags not found.");

		//
		// Select appropriate view dimension string
		//
		char szTex2DMS[] = "Texture2DMS";
		char szTex2DMSA[] = "Texture2DMSArray";

		char *szViewDim;

		if (m_RTVDesc.ViewDimension == D3D11_RTV_DIMENSION_TEXTURE2DMS)
			szViewDim = szTex2DMS;
		else
			szViewDim = szTex2DMSA;

		// shader profile, like vs_5_0
		profile = g_TestApp.GetShaderProfile(
			D3D_SHADER_STAGE_PIXEL,
			g_TestApp.m_D3DOptions.FeatureLevel,
			g_TestApp.m_D3DOptions.FeatureLevel);

		tstring final = profile + " \n" + (UseResidencyFeedback() ? szPSResidencyFeedback : szPS);

		//
		// Fill in shader code
		//
		const UINT MAX_STR = 1000;
		char szPSPassFinal[MAX_STR];
		char szPSFinal[MAX_STR];
		_snprintf( szPSFinal, MAX_STR-1, final.c_str(),
				  szDeclCount.c_str(),
				  szViewDim,
				  szType,
				  m_uSrcAddress[0], m_uSrcAddress[1], m_uSrcAddress[2], 0,
				  szSampleIndex.c_str(),
				  szInstr );
		szPSFinal[sizeof( szPSFinal ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1

		tstring passFinal = profile + " \n" + szPSPass;

		_snprintf( szPSPassFinal, MAX_STR-1, passFinal.c_str(), szInstrPass );
		szPSPassFinal[sizeof( szPSPassFinal ) - 1] = 0;	// Fix for OACR error - MISSING_ZERO_TERMINATION1

		//
		// Create pixel shader that outputs what in the texcoord
		//

		if ( !AssembleShader(szPSPassFinal, &pShaderBuf, &pErrorBuf) )
		{
			if (pErrorBuf != NULL)
			{
				WriteToLog( _T("AssembleShader failed assembling PS: %s" ), pErrorBuf->GetBufferPointer() );
			}
			throw TRException("");
		}
		if (FAILED( hr = GetDevice()->CreatePixelShader((DWORD*)pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pPSPass)))
			throw TRException("CreateVertexShader() failed creating default PS", hr, RESULT_FAIL);

		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );

		//
		//	Create pixel shader that uses ld2dms and outputs results
		//
		const UINT globalFlags = UseResidencyFeedback() ? AssemblyHelper::GF_USES_TILED_RESOURCES : 0;
		if ( !AssembleShader(szPSFinal, &pShaderBuf, &pErrorBuf, globalFlags) )
		{
			if (pErrorBuf != NULL)
			{
				WriteToLog( _T("AssembleShader failed assembling PS: %s" ), pErrorBuf->GetBufferPointer() );
			}
			throw TRException("");
		}
		if (FAILED( hr = GetDevice()->CreatePixelShader((DWORD*)pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pPS)))
			throw TRException("CreateVertexShader() failed creating default PS", hr, RESULT_FAIL);

		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
	}
	catch (TRException& exc)
	{
		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
		exc.Prepend("[CMSLoadTest::SetupShaders()] ");
		throw(exc);
	}
}

//
// SetupMSRenderTarget
//
void CMSLoadTest::SetupTextures()
{
	D3D11_TEXTURE2D_DESC tex2DDesc;
	HRESULT hr;

	//
	// Check Support
	//

	UINT uCaps;
	if( FAILED( hr = GetDevice()->CheckFormatSupport( m_Tex2DDesc.Format, &uCaps ) ) )
		throw TRException("CheckFormatSupport failed.", hr, RESULT_FAIL);

	if (!(uCaps & D3D11_FORMAT_SUPPORT_TEXTURE2D) ||
		!(uCaps & D3D11_FORMAT_SUPPORT_RENDER_TARGET) ||
		!(uCaps & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET) ||
		!(uCaps & D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD))
	{
		SkipMatching( GetSkipMatchingFilter() || FilterEqual<DXGI_FORMAT>( _T("Format"),  m_Tex2DDesc.Format ) );
		throw TRException("Format not supported as render target or texture2d or doesn't support multisample load.", TEST_RESULT(RESULT_SKIP));
	}

	UINT uLevels;
	GetDevice()->CheckMultisampleQualityLevels( m_Tex2DDesc.Format, m_Tex2DDesc.SampleDesc.Count, &uLevels);
	const bool bIsValidQualityRange = m_Tex2DDesc.SampleDesc.Quality < uLevels;
	const bool bIsD3D10_1SamplePattern = ( GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_1 ) &&
		( m_Tex2DDesc.SampleDesc.Quality == D3D11_STANDARD_MULTISAMPLE_PATTERN || m_Tex2DDesc.SampleDesc.Quality == D3D11_CENTER_MULTISAMPLE_PATTERN );

	if( uLevels == 0 ||											// If this method returns 0, the format and sample count combination is not supported for the installed adapter
		( !bIsValidQualityRange && !bIsD3D10_1SamplePattern ) )	// The valid range is between zero and one less than the level returned by ID3D11Device::CheckMultiSampleQualityLevels, or one of the sample patterns
	{
		throw TRException("Quality level not supported for this format", TEST_RESULT(RESULT_SKIP));
	}

	//
	//	Textures
	//

	// multisample render target
	tex2DDesc.ArraySize			 = m_Tex2DDesc.ArraySize;
	tex2DDesc.BindFlags			 = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tex2DDesc.CPUAccessFlags	 = 0;
	tex2DDesc.Format			 = m_Tex2DDesc.Format;
	tex2DDesc.MipLevels		     = 1;
	tex2DDesc.MiscFlags			 = 0;
	tex2DDesc.Usage				 = D3D11_USAGE_DEFAULT;
	tex2DDesc.Width				 = m_Tex2DDesc.Width;
	tex2DDesc.Height			 = m_Tex2DDesc.Height;
	tex2DDesc.SampleDesc.Count   = m_Tex2DDesc.SampleDesc.Count;
	tex2DDesc.SampleDesc.Quality = m_Tex2DDesc.SampleDesc.Quality;
	if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, &m_pMSRT ) ) )
	{
		WriteToLog( _T( "CreateTexture2D() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateTexture2D failed for MS render target texture.", hr, RESULT_FAIL);
	}

	// render target
	tex2DDesc.Format			 = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex2DDesc.ArraySize			 = 1;
	tex2DDesc.BindFlags			 = D3D11_BIND_RENDER_TARGET;
	tex2DDesc.SampleDesc.Count	 = 1;
	tex2DDesc.SampleDesc.Quality = 0;
	if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, &m_pRT ) ) )
	{
		WriteToLog( _T( "CreateTexture2D() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateTexture2D failed for render target texture.", hr, RESULT_FAIL);
	}

	// staging resource desc
	tex2DDesc.BindFlags			= 0;
	tex2DDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;
	tex2DDesc.Usage				= D3D11_USAGE_STAGING;
	if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, &m_pRTStaging ) ) )
	{
		WriteToLog( _T( "CreateTexture2D() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateTexture2D failed for render target texture.", hr, RESULT_FAIL);
	}

	// 1x1 texture that contains the sample index
	D3D11_SUBRESOURCE_DATA srData;
	srData.pSysMem = &m_uSampleIndex;
	srData.SysMemPitch = sizeof(UINT);
	srData.SysMemSlicePitch = sizeof(UINT);

	// non-literal sample index tex
	tex2DDesc.Width				= 1;
	tex2DDesc.Height			= 1;
	tex2DDesc.Format			= DXGI_FORMAT_R32_UINT;
	tex2DDesc.CPUAccessFlags	= 0;
	tex2DDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
	if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, &srData, &m_pTex2DIndex ) ) )
	{
		WriteToLog( _T( "CreateTexture2D() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateTexture2D failed for sample index texture.", hr, RESULT_FAIL);
	}

	//
	//	Multisample Views
	//

	// rtv
	D3D11_RENDER_TARGET_VIEW_DESC rtv;
	rtv.Format = m_Tex2DDesc.Format;
	rtv.ViewDimension = m_RTVDesc.ViewDimension;

	// srv
	D3D11_SHADER_RESOURCE_VIEW_DESC srv;
	srv.Format = m_Tex2DDesc.Format;

	switch( m_RTVDesc.ViewDimension )
	{
	case D3D11_RTV_DIMENSION_TEXTURE2DMS:
		// srv
		srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
		// srv
		srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
		srv.Texture2DMSArray.ArraySize = m_Tex2DDesc.ArraySize - m_uArraySlice;
		srv.Texture2DMSArray.FirstArraySlice = m_uArraySlice;
		// rtv
		rtv.Texture2DMSArray.ArraySize = m_Tex2DDesc.ArraySize;
		rtv.Texture2DMSArray.FirstArraySlice = 0;
		break;
	}

	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pMSRT, &rtv, &m_pMSRTV ) ) )
	{
		WriteToLog( _T( "CreateRenderTargetView() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateRenderTargetView failed.", hr, RESULT_FAIL);
	}

	if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pMSRT, &srv, &m_pSRV ) ) )
	{
		WriteToLog( _T( "CreateShaderResourceView() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateShaderResourceView failed.", hr, RESULT_FAIL);
	}

	srv.Format = DXGI_FORMAT_R32_UINT;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = 1;
	srv.Texture2D.MostDetailedMip = 0;
	if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pTex2DIndex, &srv, &m_pSRVIndex ) ) )
	{
		WriteToLog( _T( "CreateShaderResourceView() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateShaderResourceView failed.", hr, RESULT_FAIL);
	}

	// render target view (non MS)
	rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtv.Texture2D.MipSlice = 0;
	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRT, &rtv, &m_pRTV ) ) )
	{
		WriteToLog( _T( "CreateRenderTargetView() result code '%s'" ), D3DHResultToString( hr ).c_str() );
		throw TRException("[CMSLoadTest::SetupTextures] CreateRenderTargetView failed.", hr, RESULT_FAIL);
	}
}

//
// ExecuteTestCase - Runs ld2dms shader and reads back results
//
TEST_RESULT CMSLoadTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	try
	{
		//
		// Init Test Data
		//
		const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(m_Tex2DDesc.Format);
		DWORD uFlags = GetFlags( m_Tex2DDesc.Format );

		// rand float between -50 and 50
		srand(0);
		UINT uSize = m_Tex2DDesc.ArraySize * m_Tex2DDesc.Width * m_Tex2DDesc.Height * m_Tex2DDesc.SampleDesc.Count;
		std::vector<D3DXVECTOR4> vData(uSize);
		for( UINT i = 0; i < vData.size(); ++i )
		{
			vData[i] = D3DXVECTOR4( ((rand() % 1000) - 500.f) / 10.0f,
								   ((rand() % 1000) - 500.f) / 10.0f,
								   ((rand() % 1000) - 500.f) / 10.0f,
								   ((rand() % 1000) - 500.f) / 10.0f );

			// if its unorm or snorm data, make it between -2 and 2
			if ((uFlags & FF_UNORM) || (uFlags & FF_SNORM))
				vData[i] = vData[i] * 0.04f;
		}
		// fill vertex buffer
		FillBuffer(vData);

		//
		// Fill MSRT
		//

		// state
		FLOAT color[] = { 0, 0, 0, 0 };
		UINT strides[] = { sizeof( VERTEX ) };
		UINT offsets[] = { 0 };
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVBSamples, strides, offsets );
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
		GetEffectiveContext()->PSSetShader( m_pPSPass, NULL, 0 );
		GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pMSRTV, NULL );

		// Draw
		GetEffectiveContext()->ClearRenderTargetView( m_pMSRTV, color );
		uSize = m_Tex2DDesc.Width*m_Tex2DDesc.Height;
		for (UINT uSlice = 0; uSlice < m_Tex2DDesc.ArraySize; ++uSlice)
		{
			SetSlice(uSlice);
			for (UINT uSample = 0; uSample < m_Tex2DDesc.SampleDesc.Count; ++uSample)
			{
				UINT uStart = uSample*uSize + uSlice*uSize*m_Tex2DDesc.SampleDesc.Count;
				GetEffectiveContext()->OMSetBlendState( NULL, 0, 1 << uSample);
				GetEffectiveContext()->Draw(uSize, uStart);
			}
		}

		//
		// MSLoad MSRT
		//

		// state
		ID3D11ShaderResourceView *pSRV[] = { m_pSRV, m_pSRVIndex };
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVBQuad, strides, offsets );
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );
		GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTV, NULL );
		GetEffectiveContext()->PSSetShaderResources( 0, 2, pSRV );
		GetEffectiveContext()->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );

		// draw
		GetEffectiveContext()->ClearRenderTargetView( m_pRTV, color );
		GetEffectiveContext()->Draw( 4, 0 );

		if( FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);

		//
		// Verify results
		//

		// Readback results
		std::vector<D3DXVECTOR4> vResults(m_Tex2DDesc.Width*m_Tex2DDesc.Height);
		GetResults(vResults, m_pRT);

		// compute expected index
		D3DXVECTOR4 vInitial;
		UINT uExpectedIndex = m_uSrcAddress[0] +
							  m_uSrcAddress[1]*m_Tex2DDesc.Width +
							  m_uSampleIndex*m_Tex2DDesc.Width*m_Tex2DDesc.Height;
		// expected index only ignores array address if Texture2DMS
		if (m_RTVDesc.ViewDimension == D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY)
			uExpectedIndex += (m_uSrcAddress[2] + m_uArraySlice)*m_Tex2DDesc.Width*m_Tex2DDesc.Height*m_Tex2DDesc.SampleDesc.Count;

		// get initial data if src address is in bounds
		if ((m_uSrcAddress[0] < m_Tex2DDesc.Width) &&		// out of bounds width
			(m_uSrcAddress[1] < m_Tex2DDesc.Height) &&		// out of bounds height
			((m_uSrcAddress[2] < (m_Tex2DDesc.ArraySize - m_uArraySlice)) ||	// out of bound array only matters for array resource
			 (m_RTVDesc.ViewDimension != D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY)))
		{
			assert( uExpectedIndex < vData.size() );
			vInitial = vData[uExpectedIndex];
		}
		else
			vInitial = D3DXVECTOR4(0,0,0,0);	// out of bounds behavior

		// compute expected value
		D3DXVECTOR4 vExpected = vInitial;
		if (pTexFormatInfo->dwFlags & FF_UNORM)
		{
			vExpected.x = max( min( vExpected.x, 1.f ), 0.f );
			vExpected.y = max( min( vExpected.y, 1.f ), 0.f );
			vExpected.z = max( min( vExpected.z, 1.f ), 0.f );
			vExpected.w = max( min( vExpected.w, 1.f ), 0.f );
		}
		else if (pTexFormatInfo->dwFlags & FF_SNORM)
		{
			vExpected.x = max( min( vExpected.x, 1.f ), -1.f );
			vExpected.y = max( min( vExpected.y, 1.f ), -1.f );
			vExpected.z = max( min( vExpected.z, 1.f ), -1.f );
			vExpected.w = max( min( vExpected.w, 1.f ), -1.f );
		}
		else if ((pTexFormatInfo->dwFlags & FF_UINT) ||
				 (pTexFormatInfo->dwFlags & FF_SINT))
		{
			vExpected.x = (float)(int)vExpected.x;
			vExpected.y = (float)(int)vExpected.y;
			vExpected.z = (float)(int)vExpected.z;
			vExpected.w = (float)(int)vExpected.w;
			if (pTexFormatInfo->pBitsPerComponent[3] == 2)
				vExpected.w = min( vExpected.w, 3 );
			if (pTexFormatInfo->dwFlags & FF_UINT)
			{
				vExpected.x = max(vExpected.x, 0);
				vExpected.y = max(vExpected.y, 0);
				vExpected.z = max(vExpected.z, 0);
				vExpected.w = max(vExpected.w, 0);
			}
		}

		// andy told me this format doesn't have a sign bit
		if (m_Tex2DDesc.Format == DXGI_FORMAT_R11G11B10_FLOAT)
		{
			vExpected.x = max( vExpected.x, 0.f );
			vExpected.y = max( vExpected.y, 0.f );
			vExpected.z = max( vExpected.z, 0.f );
			vExpected.w = max( vExpected.w, 0.f );
		}

		// only use correct format for float comparison
		DXGI_FORMAT format = m_Tex2DDesc.Format;
		if ((pTexFormatInfo->dwFlags & FF_UINT) ||
			(pTexFormatInfo->dwFlags & FF_SINT))
			format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		const DXGIFormatInfo* pTexFormatInfoFake = CFormatInfoMap::Find(format);

		// test results
		// NOTE: to speed things up, only check the last 16 pixels on the last 16 rows
		UINT uStartY = max( (int)0, (int)m_Tex2DDesc.Height - 16 );
		UINT uStartX = max( (int)0, (int)m_Tex2DDesc.Width - 16);

		// num components
		UINT uMaxComp = 0;
		for (UINT comp = 0; comp < 4; ++comp)
		{
			if (pTexFormatInfo->pBitsPerComponent[comp]) uMaxComp++;
		}

		for (UINT y = uStartY; y < m_Tex2DDesc.Height; ++y)
			for (UINT x = uStartX; x < m_Tex2DDesc.Width; ++x)
				for (UINT comp = 0; comp < uMaxComp; ++comp)
				{
					// jump to a specific component of a d3dxvector4
					float fInitial = *((float*)&vInitial + comp);
					float fExpected = *((float*)&vExpected + comp);
					float fActual = *((float*)&vResults[x + y*m_Tex2DDesc.Width] + comp);

					float fUlpDiff;
					// NOTE: on ref, the 11 bit float has ulpdiff > 1, the rest are < 1.
					if (!MinMaxRange::TestFloatsSame(fExpected, fActual, pTexFormatInfoFake, comp, 2.0f, &fUlpDiff))
					{
						WriteToLog("Failure Info:");
						WriteToLog("Component: %d", comp);
						WriteToLog("Pixel x: %d, y: %d", x, y);
						WriteToLog("Initial: \t\t%f", fInitial);
						WriteToLog("Actual: \t\t%f", fActual);
						WriteToLog("Expected: \t%f", fExpected);
						if (fUlpDiff < 50) // spew ulpdiff only if it appears to be a precision issue
							WriteToLog("Ulp difference: \t%f", fUlpDiff);
						throw TRException("ld2dms returned invalid results.");
					}
				}

	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMSLoadTest::ExecuteTestCase()] ");
		if (tRes != RESULT_PASS)
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}

void CMSLoadTest::FillBuffer(std::vector<D3DXVECTOR4> &vData)
{
	// precondition
	assert( vData.size() == (m_Tex2DDesc.ArraySize*m_Tex2DDesc.Height*m_Tex2DDesc.Width*m_Tex2DDesc.SampleDesc.Count) );

	HRESULT hr;

	//
	// Fill VBSamples
	//
	D3D11_MAPPED_SUBRESOURCE mappedVB;
	if ( FAILED( hr = GetEffectiveContext()->Map(m_pVBSamples, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB) ) )
		throw TRException("m_pVertexBuffer Map failed.", hr, RESULT_FAIL);
	VERTEX* pVB = (VERTEX*)mappedVB.pData;

	UINT uSliceSize = m_Tex2DDesc.Height*m_Tex2DDesc.Width*m_Tex2DDesc.SampleDesc.Count;
	for (UINT uSlice = 0; uSlice < m_Tex2DDesc.ArraySize; ++uSlice)
		for (UINT sample = 0; sample < m_Tex2DDesc.SampleDesc.Count; ++sample)
			for (UINT y = 0; y < m_Tex2DDesc.Height; y++)
				for (UINT x = 0; x < m_Tex2DDesc.Width; x++)
				{
					pVB->pos = D3DXVECTOR2( x + 0.5f, y + 0.5f );
					pVB->data = vData[x + y*m_Tex2DDesc.Width + sample*m_Tex2DDesc.Width*m_Tex2DDesc.Height + uSlice*uSliceSize];
					++pVB;
				}
	GetEffectiveContext()->Unmap(m_pVBSamples, 0);
}

void CMSLoadTest::SetSlice(UINT uSlice)
{
	assert(m_pConstBuffer);
	assert(uSlice < m_Tex2DDesc.ArraySize);

	HRESULT hr;

	//
	// setup const data
	//
	float bufData[5];
	// Inverse viewport scale.
	bufData[2] = 2 / (float)m_Tex2DDesc.Width;
	bufData[3] = -2 / (float)m_Tex2DDesc.Height;
	// Prescaled inverse viewport translation.
	bufData[0] = -1.0f;
	bufData[1] = 1.0f;
	bufData[4] = *((float*)&uSlice);
	if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, bufData ) ) )
		throw TRException("Map on m_pConstBuffer Failed", hr, RESULT_FAIL);
}

void CMSLoadTest::GetResults(std::vector<D3DXVECTOR4> &vData, ID3D11Resource *pRes)
{
	assert( vData.size() == (m_Tex2DDesc.Width * m_Tex2DDesc.Height) );

	//copy
	GetImmediateContext()->CopyResource( m_pRTStaging, pRes );//m_pRT );

	// map
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE texmap;
	if( FAILED( hr = GetImmediateContext()->Map(m_pRTStaging, 0, D3D11_MAP_READ, 0, &texmap ) ) )
	{
		WriteToLog( _T( "Failed to map depth stencil texture, hr = 0x%x" ), hr );
		throw TRException("[CDepthTest::GetDepth] Map failed on 1D depth staging texture.", hr, RESULT_FAIL);
	}

	// copy data
	float *pData = (float*)texmap.pData;
	for (UINT y = 0; y < m_Tex2DDesc.Height; ++y)
	{
		memcpy( &vData[y*m_Tex2DDesc.Width], pData, m_Tex2DDesc.Width*sizeof(D3DXVECTOR4) );
		pData = (float*)((BYTE*)pData + texmap.RowPitch);
	}

	// unmap
	GetImmediateContext()->Unmap( m_pRTStaging, 0 );
}

void CMSLoadTest::CleanupTestCase()
{
	ID3D11Buffer *pNullBuffers[] = { NULL };
	UINT offsets[] = { 0 };
	ID3D11RenderTargetView* pRTNull[] = { NULL, NULL, NULL, NULL };
	ID3D11ShaderResourceView *pSRVNull[] = { NULL, NULL };
	GetEffectiveContext()->OMSetRenderTargets( 1, pRTNull, NULL );
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, pNullBuffers, offsets, offsets );
	GetEffectiveContext()->IASetInputLayout( NULL );
	GetEffectiveContext()->PSSetShaderResources( 0, 2, pSRVNull );
	GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
	}

	SAFE_RELEASE( m_pMSRT );
	SAFE_RELEASE( m_pRT );
	SAFE_RELEASE( m_pRTStaging );
	SAFE_RELEASE( m_pTex2DIndex );
	SAFE_RELEASE( m_pMSRTV );
	SAFE_RELEASE( m_pRTV );
	SAFE_RELEASE( m_pSRV );
	SAFE_RELEASE( m_pSRVIndex );

	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pPSPass );

	SAFE_RELEASE( m_pLayout );
	SAFE_RELEASE( m_pVBSamples );
}

void CMSLoadTest::Cleanup()
{
	ID3D11Buffer* pBufferNull[] = { NULL };
	if (GetEffectiveContext())
	{
		GetEffectiveContext()->VSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->GSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 1, pBufferNull );
		GetEffectiveContext()->RSSetState( NULL );
		GetEffectiveContext()->OMSetDepthStencilState( NULL, 0 );

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			WriteToLog("ExecuteEffectiveContext() failed");
		}
	}

	SAFE_RELEASE( m_pRSEnabled );
	SAFE_RELEASE( m_pDSDisabled );
	SAFE_RELEASE( m_pConstBuffer );
	SAFE_RELEASE( m_pVBQuad );

	CShader5xTest::Cleanup();
}

//////////////////////////////////////////////////////////////////////////
// Multisample Instructions (10.1)
//////////////////////////////////////////////////////////////////////////

CMultisampleTest::CMultisampleTest()
: m_pVBQuad(NULL),
  m_pLayout(NULL),
  m_pSRView(NULL),
  m_pTex2DBackup(NULL),
  m_pRTVBackup(NULL)
{

}

void CMultisampleTest::InitTestParameters()
{
	//
	// Get Sample Count
	//
	tstring szPath = GetPath().ToString( GetPath().Depth() - 2, 1);
	UINT uSampleCount = atoi(szPath.c_str());
	m_Tex2DDesc.SampleDesc.Count = uSampleCount;

	szPath = GetPath().ToString( GetPath().Depth() - 1 );
	UINT uMaxQualityLevel = atoi(szPath.c_str());

	//
	// Format
	//

	CTableValueSet< DXGI_FORMAT > *pVSFormat;
	// !Speedup
	if (m_Tex2DDesc.SampleDesc.Count == 2)
		pVSFormat = new CTableValueSet< DXGI_FORMAT >( MultisampleFormats, sizeof(DXGI_FORMAT), ARRAY_SIZE(MultisampleFormats) );
	else
		pVSFormat = new CTableValueSet< DXGI_FORMAT >( MultisampleFormatsSmall, sizeof(DXGI_FORMAT), ARRAY_SIZE(MultisampleFormatsSmall) );
	CTestCaseParameter< DXGI_FORMAT > *pFormatParam = AddParameter( _T( "Format" ), &m_Tex2DDesc.Format );
	testfactor::RFactor rfFormat = AddParameterValueSet< DXGI_FORMAT > ( pFormatParam, pVSFormat );

	//
	// Texture Dimension
	//

	CTestCaseParameter<D3D11_RTV_DIMENSION> *pDimParam = AddParameter( _T( "Dimension" ), &m_RTVDesc.ViewDimension );
	testfactor::RFactor rfTex2DMS = AddParameterValue( pDimParam, D3D11_RTV_DIMENSION_TEXTURE2DMS );
	testfactor::RFactor rfTex2DMSArray = AddParameterValue( pDimParam, D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY );

	//
	// Sample Description
	//

	// sample count
	CTestCaseParameter< UINT > *pSampleCountParam = AddParameter( _T("SampleCount"), &m_Tex2DDesc.SampleDesc.Count );
	// Set the default value of parameter SampleCount to the value
	// parsed from the test group name
	pSampleCountParam->SetDefaultValue(uSampleCount);

	// sample quality
	CTestCaseParameter< UINT > *pQualityParam = AddParameter( _T("SampleQuality"), &m_Tex2DDesc.SampleDesc.Quality );
	CRangeValueSet< UINT > *pQualitySet = new CRangeValueSet< UINT >( 0, uMaxQualityLevel-1, 1 );
	testfactor::RFactor rfSampleMaxQuality = AddParameterValueSet< UINT >( pQualityParam, pQualitySet );

	// special patterns
	CUserValueSet< UINT> *pQualityPatternSet = new CUserValueSet< UINT >();
	pQualityPatternSet->AddValue( D3D11_CENTER_MULTISAMPLE_PATTERN );
	pQualityPatternSet->AddValue( D3D11_STANDARD_MULTISAMPLE_PATTERN );
	testfactor::RFactor rfSamplePatternQuality = AddParameterValueSet< UINT >(pQualityParam, pQualityPatternSet);

	// special patterns only supported for specific sample counts
	testfactor::RFactor rfSampleQuality = rfSampleMaxQuality;
	UINT count = m_Tex2DDesc.SampleDesc.Count;
	if ((count == 1) || (count == 2) || (count == 4) || (count == 8) || (count == 16))
	{
		rfSampleQuality = rfSampleMaxQuality + rfSamplePatternQuality;
	}

	//
	// Width and Height
	//

	// !speedup
	UINT uSizes = ARRAY_SIZE( Heights );
	if (m_Tex2DDesc.SampleDesc.Count > 4)
		uSizes = min( 2, uSizes );

	// height
	CTestCaseParameter< UINT > *pHeightParam = AddParameter( _T("Height"), &m_Tex2DDesc.Height );
	CTableValueSet<UINT> *pHeightValues = new CTableValueSet< UINT >( Heights, sizeof(UINT), uSizes );
	testfactor::RFactor rfHeight = AddParameterValueSet( pHeightParam, pHeightValues );

	// width
	CTestCaseParameter< UINT > *pWidthParam = AddParameter( _T("Width"), &m_Tex2DDesc.Width );
	CTableValueSet<UINT> *pWidthValues = new CTableValueSet< UINT >( Widths, sizeof(UINT), uSizes );
	testfactor::RFactor rfWidth = AddParameterValueSet( pWidthParam, pWidthValues );

	//
	// Arrays
	//

	CTestCaseParameter<UINT> *pArraySizeParam = AddParameter( _T( "ArraySize" ), &m_Tex2DDesc.ArraySize );
	CTestCaseParameter<UINT> *pArraySliceParam = AddParameter( _T( "ArraySlice" ), &m_uArraySlice );

	pArraySizeParam->SetDefaultValue(1);
	pArraySliceParam->SetDefaultValue(0);

	UINT uArrayStart = 1;
	UINT uSliceStart = 0;
	// !Speedup
	if (m_Tex2DDesc.SampleDesc.Count == 2)
	{
		uArrayStart = 2;
		uSliceStart = 1;
	}
	testfactor::RFactor rfArrayAll = AddParameterValue( pArraySizeParam, uArrayStart ) * AddParameterValue( pArraySliceParam, uSliceStart );
	for (UINT i = 0; i < ARRAY_SIZE( ArraySizes ); ++i)
	{
		// !Speedup
		if ((m_Tex2DDesc.SampleDesc.Count > 8) && (i > 0)) continue;

		// !Speedup
		if (m_Tex2DDesc.SampleDesc.Count == 2) break;

		// add only a few slices
		UINT uBeg = 0;
		UINT uEnd = ArraySizes[i]-1;
		UINT uMid = (ArraySizes[i]-1)/2;

		// don't add dupes
		CUserValueSet<UINT> *pArraySliceSet = new CUserValueSet<UINT>();
		pArraySliceSet->AddValue( uBeg );
		if ( uEnd > uBeg )
			pArraySliceSet->AddValue( uEnd );
		if ( uMid > uBeg )
			pArraySliceSet->AddValue( uMid );

		rfArrayAll = rfArrayAll + ( AddParameterValue( pArraySizeParam, ArraySizes[i] ) *
									AddParameterValueSet( pArraySliceParam, pArraySliceSet ) );
	}
	testfactor::RFactor rfArrayCube = AddParameterValue( pArraySizeParam, (UINT)6 ) *
									  AddParameterValueSet( pArraySliceParam, new CRangeValueSet<UINT>( 0, 5, 1 ) );

	//
	// Create Root
	//

	// Utilize pairwise to reduce the number of testcases
	testfactor::RFactor Tex2DMS = ( rfTex2DMS + (rfArrayAll * rfTex2DMSArray ) ) * (rfHeight % rfWidth);

	testfactor::XFACTOR pairwiseTable[] =
	{
		// Factor, Group, pNotGrps, cNotGrps
		{rfFormat, 1, NULL, 0},
		{rfSampleQuality, 1, NULL, 0},
		{Tex2DMS, 1, NULL, 0},
	};

	size_t groupOrders[] =
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "Pairwise", groupOrders, _countof(groupOrders));
	SetRootTestFactor( rfTestCases );

	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R8G8B8A8_UNORM), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R32G32B32A32_FLOAT), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R10G10B10A2_UNORM), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R8G8B8A8_TYPELESS), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R32G32B32A32_TYPELESS), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(_T("Format"), DXGI_FORMAT_R10G10B10A2_TYPELESS), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), 0), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), uMaxQualityLevel - 1), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), D3D11_STANDARD_MULTISAMPLE_PATTERN), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("SampleQuality"), D3D11_CENTER_MULTISAMPLE_PATTERN), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Width"), 128), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("Height"), 17), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ArraySize"), 2), WeightFactorLarge);

	// Not adding the DeclarePriorityLevelByPriorityValue() calls here since this is a base class
}

TEST_RESULT CMultisampleTest::Setup()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	try
	{
		//
		//Set Rasterizer state
		//
		D3D11_RASTERIZER_DESC rastDesc;
		rastDesc.CullMode = D3D11_CULL_NONE;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.MultisampleEnable = true;
		rastDesc.FrontCounterClockwise = true;
		rastDesc.DepthBias = false;
		rastDesc.DepthBiasClamp = 0;
		rastDesc.SlopeScaledDepthBias = 0;
		rastDesc.DepthClipEnable = true;
		rastDesc.ScissorEnable = false;
		rastDesc.AntialiasedLineEnable = false;
		if( FAILED( hr = GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSEnabled ) ) )
			throw TRException("CreateRasterizerState for m_pRSEnabled failed.", hr, RESULT_FAIL);

		GetEffectiveContext()->RSSetState( m_pRSEnabled );
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

		//
		//Set DepthStencil state
		//
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		dsDesc.DepthEnable = FALSE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.StencilEnable = FALSE;
		if( FAILED( hr = GetDevice()->CreateDepthStencilState ( &dsDesc, &m_pDSDisabled ) ) )
			throw TRException("CreateDepthStencilState() failed.", hr, RESULT_FAIL);
		GetEffectiveContext()->OMSetDepthStencilState( m_pDSDisabled, 0 );

		//
		// Blend State
		//
		GetEffectiveContext()->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );

		//
		// Create the constant buffer
		//
		D3D11_BUFFER_DESC bufdesc;
		bufdesc.ByteWidth = CONST_BUFFER_SIZE;
		bufdesc.Usage = D3D11_USAGE_DYNAMIC;
		bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufdesc.MiscFlags = 0;
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufdesc, NULL, &m_pConstBuffer ) ) )
			throw TRException("CreateBuffer for m_pConstBuffer failed.", hr, RESULT_FAIL);
		GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );

		//
		// Create vertex buffer
		//

		// vertex buffer initial data
		float pVBData[] = {-1.f, -1.f, -1.f, 1.f, 1.f, -1.f, 1.f, 1.f};  // full screen quad
		D3D11_SUBRESOURCE_DATA srVBData;
		srVBData.pSysMem = &pVBData[0];
		srVBData.SysMemPitch = sizeof(pVBData);
		srVBData.SysMemSlicePitch = sizeof(pVBData);

		// vertex buffer desc
		D3D11_BUFFER_DESC bufDesc;
		bufDesc.ByteWidth = 4 * 2 * sizeof(float);
		bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.MiscFlags = 0;
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufDesc, &srVBData, &m_pVBQuad ) ) )
			throw TRException("CreateBuffer failed for vertex buffer.", hr, RESULT_FAIL);
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

		// backup render target
		D3D11_TEXTURE2D_DESC tex2DDesc;
		tex2DDesc.ArraySize	   = 1;
		tex2DDesc.Width		   = 4;
		tex2DDesc.Height	   = 4;
		tex2DDesc.SampleDesc.Count = 1;
		tex2DDesc.SampleDesc.Quality = 0;
		tex2DDesc.Format	   = DXGI_FORMAT_R32G32_FLOAT;
		tex2DDesc.BindFlags	   = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tex2DDesc.CPUAccessFlags = 0;
		tex2DDesc.MipLevels	   = 1;
		tex2DDesc.MiscFlags	   = 0;
		tex2DDesc.Usage		   = D3D11_USAGE_DEFAULT;
		if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, &m_pTex2DBackup ) ) )
			throw TRException("CreateTexture2D failed for render target texture.", hr, RESULT_FAIL);

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Format = tex2DDesc.Format;
		rtvDesc.Texture2D.MipSlice = 0;
		if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pTex2DBackup, &rtvDesc, &m_pRTVBackup ) ) )
			throw TRException("CreateRenderTargetView failed for render target view. (hr = %s)", hr, RESULT_FAIL);

		if (FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest::Setup()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}

TEST_RESULT CMultisampleTest::SetupTestCase()
{
	//
	// Skip the current sample count if this format doesn't support it (ie: when quality levels returned is 0)
	//
	UINT uQualityLevels;
	GetDevice()->CheckMultisampleQualityLevels( m_Tex2DDesc.Format, m_Tex2DDesc.SampleDesc.Count, &uQualityLevels );
	const bool bIsValidQualityRange = m_Tex2DDesc.SampleDesc.Quality < uQualityLevels;
	const bool bIsD3D10_1SamplePattern = ( GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_1 ) &&
		( m_Tex2DDesc.SampleDesc.Quality == D3D11_STANDARD_MULTISAMPLE_PATTERN || m_Tex2DDesc.SampleDesc.Quality == D3D11_CENTER_MULTISAMPLE_PATTERN );

	if( uQualityLevels == 0 ||									// If this method returns 0, the format and sample count combination is not supported for the installed adapter
		( !bIsValidQualityRange && !bIsD3D10_1SamplePattern ) )	// The valid range is between zero and one less than the level returned by ID3D11Device::CheckMultiSampleQualityLevels, or one of the sample patterns
	{
		WriteToLog( "Skipping unsupported sample count for this format" );
		return RESULT_SKIP;
	}

	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	try
	{
		//
		// Textures
		//
		// multisample render target
		m_Tex2DDesc.BindFlags	   = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		m_Tex2DDesc.CPUAccessFlags = 0;
		m_Tex2DDesc.MipLevels	   = 1;
		m_Tex2DDesc.MiscFlags	   = 0;
		m_Tex2DDesc.Usage		   = D3D11_USAGE_DEFAULT;
		if( FAILED( hr = GetDevice()->CreateTexture2D( &m_Tex2DDesc, NULL, &m_pRTTexture2D ) ) )
			throw TRException("CreateTexture2D failed for MS render target texture.", hr, RESULT_FAIL);

		// rtv
		D3D11_RENDER_TARGET_VIEW_DESC rtv;
		rtv.Format = m_Tex2DDesc.Format;
		rtv.ViewDimension = m_RTVDesc.ViewDimension;

		// srv
		D3D11_SHADER_RESOURCE_VIEW_DESC srv;
		srv.Format = m_Tex2DDesc.Format;

		switch( m_RTVDesc.ViewDimension )
		{
		case D3D11_RTV_DIMENSION_TEXTURE2DMS:
			// srv
			srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			break;
		case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
			// srv
			srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
			srv.Texture2DMSArray.ArraySize = m_Tex2DDesc.ArraySize - m_uArraySlice;
			srv.Texture2DMSArray.FirstArraySlice = m_uArraySlice;
			// rtv
			rtv.Texture2DMSArray.ArraySize = m_Tex2DDesc.ArraySize;
			rtv.Texture2DMSArray.FirstArraySlice = 0;
			break;
		}

		if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRTTexture2D, &rtv, &m_pRTView ) ) )
			throw TRException("CreateRenderTargetView failed.", hr, RESULT_FAIL);

		if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pRTTexture2D, &srv, &m_pSRView ) ) )
			throw TRException("CreateShaderResourceView failed.", hr, RESULT_FAIL);

		//
		// Shaders
		//
		SetupShaders();

		//
		// Setup the default viewport
		//
		D3D11_VIEWPORT vp;
		vp.Height = (float)m_Tex2DDesc.Height;
		vp.Width = (float)m_Tex2DDesc.Width;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		GetEffectiveContext()->RSSetViewports( 1, &vp );

		//
		// Const buffer
		//
		UINT bufData[1];
		bufData[0] = m_uArraySlice;
		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, bufData ) ) )
			throw TRException("Map on m_pConstBuffer Failed", hr, RESULT_FAIL);

		// state
		FLOAT color[] = { 0, 0, 0, 0 };
		UINT strides[] = { 8 };
		UINT offsets[] = { 0 };
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVBQuad, strides, offsets );
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, NULL );
		GetEffectiveContext()->ClearRenderTargetView( m_pRTView, color );

		if (FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest::SetupTestCase()] ");
		if (tRes != RESULT_PASS)
			WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}


void CMultisampleTest::SetupShaders()
{
	HRESULT hr;
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	tstring szVS, szGS, szPS;

	//
	// Default shaders
	//

	szVS = g_TestApp.GetShaderProfile(
		D3D_SHADER_STAGE_VERTEX,
		g_TestApp.m_D3DOptions.FeatureLevel,
		g_TestApp.m_D3DOptions.FeatureLevel);

	szVS += " \n";

	// default VS
	const char VS[] =
	"dcl_input v[0].xy"										"\n"
	"dcl_output_sv  o[0].xyzw, position"					"\n"
	"mov o[0].xy, v[0].xyxx"								"\n"
	"mov o[0].zw, vec4( 0.0, 0.0, 0.0, 1.0 )"				"\n";

	szVS += VS;

	szGS = g_TestApp.GetShaderProfile(
		D3D_SHADER_STAGE_GEOMETRY,
		g_TestApp.m_D3DOptions.FeatureLevel,
		g_TestApp.m_D3DOptions.FeatureLevel);

	szGS += " \n";

	// default GS
	const char GS[] =
	"dcl_inputPrimitive triangle"		"\n"
	"dcl_outputTopology trianglestrip"	"\n"
	"dcl_maxOutputVertexCount 3"	"\n"
	"dcl_constantBuffer c0[1], immediateIndexed"		"\n"
	"dcl_temps 6"					"\n"
	"dcl_input_sv v[3][0].xyzw, position"	"\n" // Result from previous stage
	"dcl_output_sv o[0].xyzw, position"			"\n"
	"dcl_output_sv o[1].x, renderTargetArrayIndex"	"\n" // render target array index
	"mov o[0].xyzw, v[0][0].xyzw"	"\n" // copy pos
	"mov o[1].x, c0[0].x"			"\n" // get array index from constant data
	"emit"							"\n"
	"mov o[0].xyzw, v[1][0].xyzw"	"\n" // copy pos
	"emit"							"\n"
	"mov o[0].xyzw, v[2][0].xyzw"	"\n" // copy pos
	"emit"							"\n";

	szGS += GS;

	// fill in shader strings
	SetupShaderStrings(szVS, szGS, szPS);

	// vertex shader
	if( !AssembleShader( szVS.c_str(), &pShaderBuf, &pErrorBuf ) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T("[CMultisampleTest::SetupShaders] AssembleShader failed assembling VS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		SAFE_RELEASE( pErrorBuf );
		throw TRException("");
	}
	if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pVS ) ) )
	{
		SAFE_RELEASE( pShaderBuf );
		throw TRException("[CMultisampleTest::SetupShaders] CreateVertexShader() failed creating default VS", hr, RESULT_FAIL);
	}
	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );

	// Set the input layout
	D3D11_INPUT_ELEMENT_DESC DeclTL[] =
	{
		{"0_", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},  //position
	};
	if(	FAILED(	hr = GetDevice()->CreateInputLayout( DeclTL, sizeof(DeclTL) / sizeof(DeclTL[ 0 ]), pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pLayout ) ) )
		throw TRException("[CMultisampleTest::SetupShaders] CreateInputLayout() failed.", hr, RESULT_FAIL);

	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );
	GetEffectiveContext()->IASetInputLayout( m_pLayout );

	// Geometry Shader
	if( !AssembleShader( szGS.c_str(), &pShaderBuf, &pErrorBuf ) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T("[CMultisampleTest::SetupShaders] AssembleShader failed assembling GS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		SAFE_RELEASE( pErrorBuf );
		throw TRException("");
	}
	if( FAILED( hr = GetDevice()->CreateGeometryShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pGS ) ) )
	{
		SAFE_RELEASE( pShaderBuf );
		throw TRException("[CMultisampleTest::SetupShaders] CreateGeometryShader() failed creating default GS", hr, RESULT_FAIL);
	}
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	//	Create pixel shader
	if ( !AssembleShader(szPS.c_str(), &pShaderBuf, &pErrorBuf) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T("[CMultisampleTest::SetupShaders] AssembleShader failed assembling PS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		SAFE_RELEASE( pErrorBuf );
		throw TRException("");
	}
	if (FAILED( hr = GetDevice()->CreatePixelShader((DWORD*)pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pPS)))
	{
		SAFE_RELEASE( pShaderBuf );
		throw TRException("[CMultisampleTest::SetupShaders] CreateVertexShader() failed creating default PS", hr, RESULT_FAIL);
	}
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );
}

void CMultisampleTest::CleanupTestCase()
{
	// unbind
	ID3D11RenderTargetView* pRTNull[] = { NULL, NULL, NULL, NULL };
	ID3D11ShaderResourceView *pSRVNull[] = { NULL, NULL };
	GetEffectiveContext()->OMSetRenderTargets( 1, pRTNull, NULL );
	GetEffectiveContext()->PSSetShaderResources( 0, 1, pSRVNull );
	GetEffectiveContext()->IASetInputLayout( NULL );
	GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
	}

	// release
	SAFE_RELEASE( m_pRTTexture2D );
	SAFE_RELEASE( m_pRTView );
	SAFE_RELEASE( m_pSRView );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pLayout );
}

void CMultisampleTest::Cleanup()
{
	// cleanup VB
	if (GetEffectiveContext())
	{
		ID3D11Buffer *pNullBuffers[] = { NULL };
		UINT offsets[] = { 0 };
		GetEffectiveContext()->IASetVertexBuffers(0, 1, pNullBuffers, offsets, offsets);
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
	}

	SAFE_RELEASE( m_pVBQuad );

	SAFE_RELEASE( m_pTex2DBackup );
	SAFE_RELEASE( m_pRTVBackup );
	// cleanup base
	CShader5xTest::Cleanup();
}


//////////////////////////////////////////////////////////////////////////
// SampleInfo
//////////////////////////////////////////////////////////////////////////

CSampleInfoTest::CSampleInfoTest()
: CMultisampleTest()
{

}

void CSampleInfoTest::InitTestParameters()
{
	CMultisampleTest::InitTestParameters();

	// sample quality
	CTestCaseParameter< bool > *pInfoIntParam = AddParameter( _T("_uint"), &m_bInfoInt );
	testfactor::RFactor rfInfoInt = AddParameterValueSet< bool >( pInfoIntParam, new CBoolValueSet() );

	// rasterizer
	CTestCaseParameter< bool > *pRasterizerParam = AddParameter( _T("rasterizer"), &m_bRasterizer );
	testfactor::RFactor rfRasterizer = AddParameterValueSet< bool >( pRasterizerParam, new CBoolValueSet() );

	// bind resource
	CTestCaseParameter< bool > *pBindParam = AddParameter( _T("bindResource"), &m_bBindResource );
	testfactor::RFactor rfBind = AddParameterValueSet< bool >( pBindParam, new CBoolValueSet() );

	SetRootTestFactor( GetRootTestFactor() * rfInfoInt * rfRasterizer * rfBind );

	// Sample frequency interpolation are undefined
	// when using the center sample patterns (D3D10_CENTER_MULTISAMPLE_PATTERN)
	// that have more than one sample overlapping at the center of the pixel.
	CFilterHandle filter;
	filter = FilterEqual<bool>( _T("rasterizer"), true ) &&
		     FilterEqual<UINT>( _T("SampleQuality"), D3D11_CENTER_MULTISAMPLE_PATTERN ) &&
		     FilterGreater<UINT>( _T("SampleCount"), 1 );

	SkipMatching( filter );

	AddPriorityPatternFilter(FilterEqual<bool>(_T("bindResource"), true), WeightFactorVeryLarge);

	DeclarePriorityLevelByPriorityValue( 1, 5.0f );
	DeclarePriorityLevelByPriorityValue( 2, 3.0f );
}

void CSampleInfoTest::SetupShaderStrings(tstring &szVS, tstring &szGS, tstring &szPS)
{

	szPS = g_TestApp.GetShaderProfile(
		D3D_SHADER_STAGE_PIXEL,
		g_TestApp.m_D3DOptions.FeatureLevel,
		g_TestApp.m_D3DOptions.FeatureLevel);

	szPS += " \n";

	const char PS[] =
	"dcl_input_ps_sv v[0].xyzw, linear_noperspective, position"  "\n"
	"dcl_temps 1"												 "\n"
	"dcl_output o[0].xyzw"										 "\n";

	szPS += PS;

	// resource info
	D3D11_TEXTURE2D_DESC desc;
	m_pRTTexture2D->GetDesc(&desc);
	UINT uFlags = GetFlags(desc.Format);

	// declare resource
	szPS += "dcl_resource t0, Texture2DMS";
	if (m_RTVDesc.ViewDimension == D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY)
		szPS += "Array";

	if (uFlags & FF_UINT)
		szPS += ", uint \n";
	else if (uFlags & FF_SINT)
		szPS += ", sint \n";
	else
		szPS += ", float \n";

	// return type info
	ID3D11Texture2D *pTex2D = m_bRasterizer ? m_pRTTexture2D : m_pTex2DBackup;
	pTex2D->GetDesc(&desc);
	uFlags = GetFlags(desc.Format);

	// construct end of PS
	// part 1: call sampleinfo
	if (m_bInfoInt)
		szPS += tstring("sampleinfo_uint r0.xyzw, ");
	else
		szPS += tstring("sampleinfo r0.xyzw, ");

	// read from the rasterizer or a bound resource
	if (m_bRasterizer)
		szPS += tstring("rasterizer \n");
	else
		szPS += tstring("t0 \n");


	// part 2: convert to render target format
	if (m_bInfoInt)
	{
		if (uFlags & (FF_FLOAT | FF_UNORM | FF_SNORM))
		{
			szPS += tstring("utof r0, r0 \n");
			if (uFlags & (FF_UNORM | FF_SNORM))
				szPS += tstring("mul r0, r0, vec4(0.031250f, 0, 0, 0) \n");
		}
	}
	else
	{
		if (uFlags & (FF_UNORM | FF_SNORM))
			szPS += tstring("mul r0, r0, vec4(0.031250f, 0, 0, 0) \n");
		else if (uFlags & (FF_UINT | FF_SINT))
			szPS += tstring("ftou r0, r0 \n");
	}
	szPS += tstring("mov o[0], r0");

}

TEST_RESULT CSampleInfoTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	D3D11_TEXTURE2D_DESC desc;
	UINT uArraySlice = m_uArraySlice;
	ID3D11Texture2D *pTex2D = m_pRTTexture2D;
	if (!m_bRasterizer)
	{
		pTex2D = m_pTex2DBackup;
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTVBackup, NULL );

		if(m_bBindResource)
			GetEffectiveContext()->PSSetShaderResources( 0, 1, &m_pSRView );
		else
		{
			ID3D11ShaderResourceView *pNullView = NULL ;
			GetEffectiveContext()->PSSetShaderResources( 0, 1, &pNullView );
		}

		uArraySlice = 0;
	}
	pTex2D->GetDesc(&desc);

	// viewport
	D3D11_VIEWPORT vp;
	vp.Height = (float)desc.Height;
	vp.Width = (float)desc.Width;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	GetEffectiveContext()->RSSetViewports( 1, &vp );

	// draw
	GetEffectiveContext()->Draw(4, 0);

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("[CSampleInfoTest::ExecuteTestCase] ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	// verify
	std::vector<D3DXVECTOR4> vBuffer(desc.Width * desc.Height * desc.SampleDesc.Count * desc.ArraySize);
	if( !MSHelper::Read(GetFramework(), pTex2D, (FLOAT*)&vBuffer[0], desc.Format) )
	{
		WriteToLog("[CSampleInfoTest::ExecuteTestCase] MSHelper::Read failed. ");
		return RESULT_FAIL;
	}

	D3DXVECTOR4 *pData = &vBuffer[desc.Width * desc.Height * desc.SampleDesc.Count * uArraySlice];
	for (UINT uSample = 0; uSample < desc.SampleDesc.Count; ++uSample)
		for (UINT uY = 0; uY < desc.Height; ++uY)
			for (UINT uX = 0; uX < desc.Width; ++uX)
			{
				UINT uData;
				// convert unorms and snorms back to ints
				if (GetFlags(desc.Format) & (FF_UNORM | FF_SNORM))
					uData = (UINT)(pData->x*32 + 0.5);
				else
					uData = (UINT)pData->x;

				// compare reported sample count to supplied sample count
				// Note: The first check is making ensuring two things:
				// 1) The shader uses the sampleinfo instruction with a texture as an input (not the rasterizer)
				// 2) The test did NOT bind the texture as an input to the shader
				if( !m_bRasterizer && !m_bBindResource )
				{
					// Regression coverage for DirectX Bug: 107710
					if( uData != 0 )
					{
						WriteToLog("Pixel [%d,%d], Sample[%d]", uX, uY, uSample);
						WriteToLog("sampleinfo returned %d", uData);
						WriteToLog("[CSampleInfoTest::ExecuteTestCase()] sampleinfo did NOT return 0 for the sampleCount.");
						return RESULT_FAIL;
					}
				}
				else if (uData != m_Tex2DDesc.SampleDesc.Count)
				{
					WriteToLog("Pixel [%d,%d], Sample[%d]", uX, uY, uSample);
					WriteToLog("sampleinfo returned %d", uData);
					WriteToLog("[CSampleInfoTest::ExecuteTestCase()] Resource sample count != sampleinfo.");
					return RESULT_FAIL;
				}
				++pData;
			}

	return RESULT_PASS;
}


//////////////////////////////////////////////////////////////////////////
// SamplePos
//////////////////////////////////////////////////////////////////////////

CSamplePosTest::CSamplePosTest()
: CMultisampleTest()
{

}

void CSamplePosTest::InitTestParameters()
{
	CMultisampleTest::InitTestParameters();

	CUserValueSet<bool> *pTrueValueSet = new CUserValueSet< bool >();
	pTrueValueSet->AddValue( true );

	CUserValueSet<bool> *pFalseValueSet = new CUserValueSet< bool >();
	pFalseValueSet->AddValue( false );

	// rasterizer
	CTestCaseParameter< bool > *pRasterizerParam = AddParameter( _T("rasterizer"), &m_bRasterizer );
	testfactor::RFactor rfRasterizer      = AddParameterValueSet< bool >( pRasterizerParam, new CBoolValueSet() );
	testfactor::RFactor rfRasterizerTrue  = AddParameterValueSet< bool >( pRasterizerParam, pTrueValueSet );
	testfactor::RFactor rfRasterizerFalse = AddParameterValueSet< bool >( pRasterizerParam, pFalseValueSet );

	CTestCaseParameter< bool > *pSRVBound = AddParameter( _T("SRV_Bound"), &m_bSRVBound );
	testfactor::RFactor rfSRVBound        = AddParameterValueSet< bool >( pSRVBound, new CBoolValueSet() );
	testfactor::RFactor rfSRVBoundFalse   = AddParameterValueSet< bool >( pSRVBound, pFalseValueSet );


	// sample index (explicitly test out-of-bounds sample index)
	CTestCaseParameter< UINT > *pSampleIndexParam = AddParameter( _T("SampleIndex"), &m_uSampleIndex );
	testfactor::RFactor rfSampleIndex = AddParameterValueSet< UINT >( pSampleIndexParam, new CRangeValueSet<UINT>(0, m_Tex2DDesc.SampleDesc.Count+1, 1) );

	// The SRVBound test case only matters when m_bRasterizer = false
	SetRootTestFactor( (GetRootTestFactor() * rfRasterizerTrue  * rfSRVBoundFalse * rfSampleIndex) +
					   (GetRootTestFactor() * rfRasterizerFalse * rfSRVBound      * rfSampleIndex) );

	// Sample frequency interpolation are undefined
	// when using the center sample patterns (D3D10_CENTER_MULTISAMPLE_PATTERN)
	// that have more than one sample overlapping at the center of the pixel.
	CFilterHandle filter;
	filter = FilterEqual<bool>( _T("rasterizer"), true ) &&
		     FilterEqual<UINT>( _T("SampleQuality"), D3D11_CENTER_MULTISAMPLE_PATTERN ) &&
		     FilterGreater<UINT>( _T("SampleCount"), 1 );

	SkipMatching( filter );

	AddPriorityPatternFilter(FilterEqual<bool>(_T("SRV_Bound"), true), WeightFactorVeryLarge);

	DeclarePriorityLevelByPriorityValue( 1, 10.125f );
	DeclarePriorityLevelByPriorityValue( 2, 6.0f );
}

void CSamplePosTest::SetupShaderStrings(tstring &szVS, tstring &szGS, tstring &szPS)
{
	szPS = g_TestApp.GetShaderProfile(
		D3D_SHADER_STAGE_PIXEL,
		g_TestApp.m_D3DOptions.FeatureLevel,
		g_TestApp.m_D3DOptions.FeatureLevel);

	szPS += " \n";

	const char PS[] =
	"dcl_input_ps_sv v[0].xyzw, linear_noperspective, position"  "\n"
	"dcl_temps 1"												 "\n"
	"dcl_output o[0].xyzw"										 "\n";

	szPS += PS;


	// resource info
	D3D11_TEXTURE2D_DESC desc;
	m_pRTTexture2D->GetDesc(&desc);
	UINT uFlags = GetFlags(desc.Format);

	// declare resource
	szPS += "dcl_resource t0, Texture2DMS";
	if (m_RTVDesc.ViewDimension == D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY)
		szPS += "Array";

	if (uFlags & FF_UINT)
		szPS += ", uint \n";
	else if (uFlags & FF_SINT)
		szPS += ", sint \n";
	else
		szPS += ", float \n";

	// return type info
	ID3D11Texture2D *pTex2D = m_bRasterizer ? m_pRTTexture2D : m_pTex2DBackup;
	pTex2D->GetDesc(&desc);
	uFlags = GetFlags(desc.Format);

	// construct end of PS
	// part 1: call samplepos
	// read from the rasterizer or a bound resource
	if (m_bRasterizer)
		szPS += tstring("samplepos r0, rasterizer, ") + ToString(m_uSampleIndex)+ tstring("\n");
	else
		szPS += tstring("samplepos r0, t0, ") + ToString(m_uSampleIndex) + tstring("\n");

	// part 2: convert to render target format
	szPS += tstring("add r0, r0, vec4(0.5f, 0.5f, 0, 0) \n");
	if (uFlags & (FF_SINT | FF_UINT))
	{
		szPS += tstring("mul r0, r0, vec4(32.0f, 32.0f, 0, 0) \n");
		szPS += tstring("ftou r0, r0 \n");
	}
	szPS += tstring("mov o[0], r0 \n");
}

TEST_RESULT CSamplePosTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("[CSamplePosTest::ExecuteTestCase] ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	// determine sample locations
	if(!SampleFinder::Search(GetFramework(), m_pRTView, m_pRTTexture2D, m_vSamplePos))
		return RESULT_FAIL;


	// pick render target and shader resource based on test parameters
	D3D11_TEXTURE2D_DESC desc;
	UINT uArraySlice = m_uArraySlice;
	ID3D11Texture2D *pTex2D = m_pRTTexture2D;
	if (!m_bRasterizer)
	{
		pTex2D = m_pTex2DBackup;
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTVBackup, NULL );

		ID3D11ShaderResourceView* SRVArray[1];

		if (m_bSRVBound)
		{
			SRVArray[0] = m_pSRView;
		}
		else
		{
			SRVArray[0] = NULL;
		}

		GetEffectiveContext()->PSSetShaderResources( 0, 1, SRVArray );
		uArraySlice = 0;
	}
	pTex2D->GetDesc(&desc);

	// viewport
	D3D11_VIEWPORT vp;
	vp.Height = (float)desc.Height;
	vp.Width = (float)desc.Width;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	GetEffectiveContext()->RSSetViewports( 1, &vp );

	// draw
	GetEffectiveContext()->Draw(4, 0);

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("[CSamplePosTest::ExecuteTestCase] ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	// verify
	std::vector<D3DXVECTOR4> vBuffer(desc.Width * desc.Height * desc.SampleDesc.Count * desc.ArraySize);
	if( !MSHelper::Read(GetFramework(), pTex2D, (FLOAT*)&vBuffer[0], desc.Format) )
	{
		WriteToLog("[CSampleInfoTest::ExecuteTestCase] MSHelper::Read failed. ");
		return RESULT_FAIL;
	}

	// compute expected value
	// remap pExpected from [0, 1<<SubPixelPrecision] to [-16, 16]
	const int SubPixelPrecision = 8;

	POINT pExpected;

	if(!m_bRasterizer && !m_bSRVBound)
	{
		pExpected.x = 0;
		pExpected.y = 0;
	}
	else if(m_uSampleIndex < m_Tex2DDesc.SampleDesc.Count)
	{
		pExpected = m_vSamplePos[m_uSampleIndex];

		if(4 == SubPixelPrecision)
		{
			pExpected.x = pExpected.x*2 - 16;
			pExpected.y = pExpected.y*2 - 16;
		}
		else
		{
			assert(8 == SubPixelPrecision);
			pExpected.x = pExpected.x/8 - 16;
			pExpected.y = pExpected.y/8 - 16;
		}
	}
	else
	{
		pExpected.x = 0;
		pExpected.y = 0;
	}


	// verify
	D3DXVECTOR4 *pData = &vBuffer[desc.Width * desc.Height * desc.SampleDesc.Count * uArraySlice];
	for (UINT uSample = 0; uSample < desc.SampleDesc.Count; ++uSample)
		for (UINT uY = 0; uY < desc.Height; ++uY)
			for (UINT uX = 0; uX < desc.Width; ++uX)
			{
				POINT pActual;
				UINT uFlags = GetFlags(desc.Format);

				// convert coords to ints
				if (uFlags & (FF_FLOAT | FF_SNORM | FF_UNORM))
				{
					pActual.x = (int)(pData->x*32 + 0.5) - 16;
					pActual.y = (int)(pData->y*32 + 0.5) - 16;
				}
				else
				{
					pActual.x = (int)pData->x - 16;
					pActual.y = (int)pData->y - 16;
				}

				// is no G component, can't test y
				if (GetBitsPerComponent(1, desc.Format) == 0)
					pActual.y = pExpected.y;


				// compare to expected
				if ((pActual.x != pExpected.x) ||
					(pActual.y != pExpected.y))
				{
					WriteToLog("Pixel [%d,%d], Sample[%d]", uX, uY, uSample);
					WriteToLog("samplepos returned (%d, %d)", pActual.x, pActual.y);
					WriteToLog("actual sample position = (%d, %d)", pExpected.x, pExpected.y);
					WriteToLog("[CSampleInfoTest::ExecuteTestCase()] samplepos returned incorrect sample location.");
					return RESULT_FAIL;
				}
				++pData;
			}

	return RESULT_PASS;
}



//////////////////////////////////////////////////////////////////////////
// SampleIndex
//////////////////////////////////////////////////////////////////////////

CSampleIndexTest::CSampleIndexTest()
: CMultisampleTest()
{

}

void CSampleIndexTest::InitTestParameters()
{
	CMultisampleTest::InitTestParameters();

	// Sample frequency interpolation are undefined
	// when using the center sample patterns (D3D10_CENTER_MULTISAMPLE_PATTERN)
	// that have more than one sample overlapping at the center of the pixel.
	CFilterHandle filter;
	filter = FilterEqual<UINT>( _T("SampleQuality"), D3D11_CENTER_MULTISAMPLE_PATTERN ) &&
		     FilterGreater<UINT>( _T("SampleCount"), 1 );

	SkipMatching( filter );

	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 2.0f );
}

void CSampleIndexTest::SetupShaderStrings(tstring &szVS, tstring &szGS, tstring &szPS)
{

	// shader profile, like vs_5_0
	szPS = g_TestApp.GetShaderProfile(
		D3D_SHADER_STAGE_PIXEL,
		g_TestApp.m_D3DOptions.FeatureLevel,
		g_TestApp.m_D3DOptions.FeatureLevel);

	szPS += " \n";

	const char PS[] =
	"dcl_input_ps_sv v[0].xyzw, linear_noperspective, position"  "\n"
	"dcl_input_ps_sv v[1].x, constant, sampleIndex"						 "\n"
	"dcl_temps 1"												 "\n"
	"dcl_output o[0].xyzw"										 "\n";

	szPS += PS;

	// resource info
	D3D11_TEXTURE2D_DESC desc;
	m_pRTTexture2D->GetDesc(&desc);
	UINT uFlags = GetFlags(desc.Format);

	// construct end of PS
	// convert to render target format
	szPS += tstring("mov r0.yzww, vec4(0,0,0,0)");
	szPS += tstring("mov r0.x, v[1].x \n");
	if (uFlags & (FF_FLOAT | FF_UNORM | FF_SNORM))
	{
		szPS += tstring("utof r0, r0.x \n");
		if (uFlags & (FF_UNORM | FF_SNORM))
			szPS += tstring("mul r0, r0, vec4(0.031250f, 0, 0, 0) \n");
	}
	szPS += tstring("mov o[0], r0 \n");
}

TEST_RESULT CSampleIndexTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	// pick render target and shader resource based on test parameters
	D3D11_TEXTURE2D_DESC desc;
	UINT uArraySlice = m_uArraySlice;
	m_pRTTexture2D->GetDesc(&desc);

	// draw
	GetEffectiveContext()->Draw(4, 0);

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("[CSampleIndexTest::ExecuteTestCase] ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	// verify
	std::vector<D3DXVECTOR4> vBuffer(desc.Width * desc.Height * desc.SampleDesc.Count * desc.ArraySize);
	if( !MSHelper::Read(GetFramework(), m_pRTTexture2D, (FLOAT*)&vBuffer[0], desc.Format) )
	{
		WriteToLog("[CSamplePosTest::ExecuteTestCase] MSHelper::Read failed. ");
		return RESULT_FAIL;
	}

	// verify
	D3DXVECTOR4 *pData = &vBuffer[desc.Width * desc.Height * desc.SampleDesc.Count * uArraySlice];
	for (UINT uSample = 0; uSample < desc.SampleDesc.Count; ++uSample)
		for (UINT uY = 0; uY < desc.Height; ++uY)
			for (UINT uX = 0; uX < desc.Width; ++uX)
			{
				UINT uActual;
				// convert unorms and snorms back to ints
				if (GetFlags(desc.Format) & (FF_UNORM | FF_SNORM))
					uActual = (UINT)(pData->x*32 + 0.5);
				else
					uActual = (UINT)pData->x;

				// compare to expected
				if (uActual != uSample)
				{
					WriteToLog("Pixel [%d,%d]", uX, uY);
					WriteToLog("actual sample index = %d", uActual);
					WriteToLog("expected sample index = %d", uSample);
					WriteToLog("[CSamplePosTest::ExecuteTestCase()] sv_sampleindex is incorrect.");
					return RESULT_FAIL;
				}
				++pData;
			}

	return RESULT_PASS;
}


