//////////////////////////////////////////////////////////////////////////
//  Multisample.cpp
//  created:	2005/07/01
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include <WGFTestCore.h>
#include <stdlib.h>
#include <queue>
using namespace std;

#include "multisample.h"
#include "FormatDesc.hpp"
#include "Handle.h"

#define CONST_BUFFER_SIZE 512

CMultisampleTestApp g_TestApp;


DXGI_FORMAT MultisampleFormats[] =
{
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R32G32B32A32_TYPELESS,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_UINT,
	DXGI_FORMAT_R32G32B32A32_SINT,

	DXGI_FORMAT_R32G32B32_TYPELESS,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32_SINT,

	DXGI_FORMAT_R16G16B16A16_TYPELESS,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_R16G16B16A16_UINT,
	DXGI_FORMAT_R16G16B16A16_SNORM,

	DXGI_FORMAT_R32G32_TYPELESS,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32_SINT,

	DXGI_FORMAT_R10G10B10A2_TYPELESS,
	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_R10G10B10A2_UINT,

	DXGI_FORMAT_R11G11B10_FLOAT,

	DXGI_FORMAT_R8G8B8A8_TYPELESS,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	DXGI_FORMAT_R8G8B8A8_UINT,
	DXGI_FORMAT_R8G8B8A8_SNORM,
	DXGI_FORMAT_R8G8B8A8_SINT,

	DXGI_FORMAT_R16G16_TYPELESS,
	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_R16G16_SNORM,
	DXGI_FORMAT_R16G16_SINT,

	DXGI_FORMAT_R32_TYPELESS,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32_SINT,

	DXGI_FORMAT_R8G8_TYPELESS,
	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_R8G8_UINT,
	DXGI_FORMAT_R8G8_SNORM,
	DXGI_FORMAT_R8G8_SINT,

	DXGI_FORMAT_R16_TYPELESS,
	DXGI_FORMAT_R16_FLOAT,
	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R16_SNORM,
	DXGI_FORMAT_R16_SINT,

	DXGI_FORMAT_R8_TYPELESS,
	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R8_UINT,
	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_R8_SINT,

	DXGI_FORMAT_B5G6R5_UNORM,	// Optional FL10.0, Required on FL10.1+
	DXGI_FORMAT_B5G5R5A1_UNORM, // Optional FL10.0+
	DXGI_FORMAT_B4G4R4A4_UNORM,	// Optional FL10.0+
};
UINT MSFormatsSize = ARRAY_SIZE(MultisampleFormats);



DXGI_FORMAT R32G32B32A32[] =
{
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_UINT,
	DXGI_FORMAT_R32G32B32A32_SINT,
};
vector<DXGI_FORMAT> R32G32B32A32Family( R32G32B32A32, R32G32B32A32 + ARRAY_SIZE(R32G32B32A32) );

DXGI_FORMAT R32G32B32[] =
{
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32_SINT,
};
vector<DXGI_FORMAT> R32G32B32Family( R32G32B32, R32G32B32 + ARRAY_SIZE(R32G32B32) );

DXGI_FORMAT R32G32[] =
{
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32_SINT,
};
vector<DXGI_FORMAT> R32G32Family( R32G32, R32G32 + ARRAY_SIZE(R32G32) );

DXGI_FORMAT R32[] =
{
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32_SINT,
};
vector<DXGI_FORMAT> R32Family( R32, R32 + ARRAY_SIZE(R32) );

DXGI_FORMAT R16G16B16A16[] =
{
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_R16G16B16A16_UINT,
	DXGI_FORMAT_R16G16B16A16_SNORM,
};
vector<DXGI_FORMAT> R16G16B16A16Family( R16G16B16A16, R16G16B16A16 + ARRAY_SIZE(R16G16B16A16) );

DXGI_FORMAT R16G16[] =
{
	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_R16G16_SNORM,
	DXGI_FORMAT_R16G16_SINT,
};
vector<DXGI_FORMAT> R16G16Family( R16G16, R16G16 + ARRAY_SIZE(R16G16) );

DXGI_FORMAT R16[] =
{
	DXGI_FORMAT_R16_FLOAT,
	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R16_SNORM,
	DXGI_FORMAT_R16_SINT,
};
vector<DXGI_FORMAT> R16Family( R16, R16 + ARRAY_SIZE(R16) );

DXGI_FORMAT R8G8B8A8[] =
{
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	DXGI_FORMAT_R8G8B8A8_UINT,
	DXGI_FORMAT_R8G8B8A8_SNORM,
	DXGI_FORMAT_R8G8B8A8_SINT,
};
vector<DXGI_FORMAT> R8G8B8A8Family( R8G8B8A8, R8G8B8A8 + ARRAY_SIZE(R8G8B8A8) );

DXGI_FORMAT R8G8[] =
{
	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_R8G8_UINT,
	DXGI_FORMAT_R8G8_SNORM,
	DXGI_FORMAT_R8G8_SINT,
};
vector<DXGI_FORMAT> R8G8Family( R8G8, R8G8 + ARRAY_SIZE(R8G8) );

DXGI_FORMAT R8[] =
{
	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R8_UINT,
	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_R8_SINT
};
vector<DXGI_FORMAT> R8Family( R8, R8 + ARRAY_SIZE(R8) );

DXGI_FORMAT R10G10B10A2[] =
{
	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_R10G10B10A2_UINT,
};
vector<DXGI_FORMAT> R10G10B10A2Family( R10G10B10A2, R10G10B10A2 + ARRAY_SIZE(R10G10B10A2) );

#define TypelessFormatsSize 11
TypelessSubset TypelessFormats[TypelessFormatsSize] =
{
	{ DXGI_FORMAT_R32G32B32A32_TYPELESS, R32G32B32A32, ARRAY_SIZE(R32G32B32A32) }, 
	{ DXGI_FORMAT_R32G32B32_TYPELESS, R32G32B32, ARRAY_SIZE(R32G32B32) }, 
	{ DXGI_FORMAT_R32G32_TYPELESS, R32G32, ARRAY_SIZE(R32G32) }, 
	{ DXGI_FORMAT_R32_TYPELESS, R32, ARRAY_SIZE(R32) },
	
	{ DXGI_FORMAT_R16G16B16A16_TYPELESS, R16G16B16A16, ARRAY_SIZE(R16G16B16A16) },
	{ DXGI_FORMAT_R16G16_TYPELESS, R16G16, ARRAY_SIZE(R16G16) },
	{ DXGI_FORMAT_R16_TYPELESS, R16, ARRAY_SIZE(R16) },

	{ DXGI_FORMAT_R8G8B8A8_TYPELESS, R8G8B8A8, ARRAY_SIZE(R8G8B8A8) },
	{ DXGI_FORMAT_R8G8_TYPELESS, R8G8, ARRAY_SIZE(R8G8) },
	{ DXGI_FORMAT_R8_TYPELESS, R8, ARRAY_SIZE(R8) },

	{ DXGI_FORMAT_R10G10B10A2_TYPELESS, R10G10B10A2, ARRAY_SIZE(R10G10B10A2)}
};


DXGI_FORMAT SingularFormats[] = 
{
	DXGI_FORMAT_R11G11B10_FLOAT,

	DXGI_FORMAT_B5G6R5_UNORM,
	DXGI_FORMAT_B5G5R5A1_UNORM,
	DXGI_FORMAT_B4G4R4A4_UNORM,
};

vector<DXGI_FORMAT> EmptyFamily;

DXGI_FORMAT CompactMultisampleFormats[] =
{
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R10G10B10A2_UINT,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R16G16_SINT,
	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_B5G6R5_UNORM
};
UINT CompactMultisampleFormatsSize = ARRAY_SIZE(CompactMultisampleFormats);
TypelessSubset CompactTypelessFormats[] =
{
	{ DXGI_FORMAT_R16G16B16A16_TYPELESS, R16G16B16A16, ARRAY_SIZE(R16G16B16A16) }
};

DXGI_FORMAT DSFormats[] =
{
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
	DXGI_FORMAT_D24_UNORM_S8_UINT
};
UINT DSFormatsSize = ARRAY_SIZE(DSFormats);

DXGI_FORMAT DFormats[] =
{
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
	DXGI_FORMAT_D24_UNORM_S8_UINT,
	DXGI_FORMAT_D32_FLOAT,
	DXGI_FORMAT_D16_UNORM
};
UINT DFormatsSize = ARRAY_SIZE( DFormats );

// the list of formats that require multisample resolve for d3d11
DXGI_FORMAT ResolveFormats[] = 
{
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_R16G16B16A16_SNORM,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_R11G11B10_FLOAT,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	DXGI_FORMAT_R8G8B8A8_SNORM,
	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_R16G16_SNORM,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_R8G8_SNORM,
	DXGI_FORMAT_R16_FLOAT,
	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R16_SNORM,
	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_A8_UNORM,
	DXGI_FORMAT_B8G8R8A8_UNORM,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
	DXGI_FORMAT_B8G8R8X8_UNORM,
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
	DXGI_FORMAT_B5G6R5_UNORM,	// This is optional for FL10.0, unless D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET is supported
	DXGI_FORMAT_B5G5R5A1_UNORM,	// This is optional for FL10.0+
	DXGI_FORMAT_B4G4R4A4_UNORM,	// This is optional for FL10.0+
};
UINT ResolveFormatsSize = ARRAY_SIZE(ResolveFormats);

DXGI_FORMAT CompactResolveFormats[] = 
{
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R8G8B8A8_SNORM,  
	DXGI_FORMAT_R8G8_SNORM,
	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
	DXGI_FORMAT_B5G6R5_UNORM
};
UINT CompactResolveFormatsSize = ARRAY_SIZE(CompactResolveFormats);


// should mirror everything in the enum but custom
PIX_POS PixPos[] =
{
	PP_TOP_LEFT,
	PP_TOP_RIGHT,
	PP_BOTTOM_LEFT,
	PP_BOTTOM_RIGHT
};

MS_READBACK ReadbackMethods[] =
{
	MS_RESOLVE,
	MS_LOAD
	//MS_PRESENT
};

// only sample counts 1(trival), 2, 4, 8 and 16 support standard sample pattern
UINT StandardSampleCounts[] = 
{
	2, 4, 8, 16
};
UINT StandardSampleCountsSize = ARRAY_SIZE(StandardSampleCounts);


BEGIN_NAMED_VALUES( QUALITY_LEVEL_SCENARIO )
	NAMED_VALUE( "Quality_level_zero", QUALITY_LEVEL_ZERO )
	NAMED_VALUE( "Quality_level_max_supported", QUALITY_LEVEL_MAX_SUPPORTED )
	NAMED_VALUE( "Quality_level_standard", QUALITY_LEVEL_STANDARD )
	NAMED_VALUE( "Quality_level_center", QUALITY_LEVEL_CENTER )
END_NAMED_VALUES( QUALITY_LEVEL_SCENARIO )

//////////////////////////////////////////////////////////////////////////
// CMultisampleTestApp
//////////////////////////////////////////////////////////////////////////

void CMultisampleTestApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();

	int n = AddOptionCategory( _T( "Debug" ) );  
	RegisterOptionVariable( _T( "LogShaders" ), _T( "Logs the shaders for failing cases." ), &m_bLogShaders, false, OPTION_VALUE_CONSTANT, n );

	m_bPresent = false;
	if (!GetShell()->IsConsole())
	{
		RegisterOptionVariable( _T( "Present" ), _T( "Causes the tests to actually Present the render target." ), &m_bPresent, false, 0, n );
	}
	
	UnRegisterOptionVariable("srconly");
	UnRegisterOptionVariable("Ref");
	UnRegisterOptionVariable("OutputRef");
	UnRegisterOptionVariable("AdapterRef");
	m_D3DOptions.SrcOnly = true;

	return;
}



TEST_RESULT CMultisampleTestApp::PreSetupTestCase()
{
	TEST_RESULT tRes = ((CMultisampleTest*)GetCurrentTest())->PreSetupTestCase();
	if ( RESULT_PASS == tRes )
		return CD3D11TestFramework::PreSetupTestCase();
	else
		return tRes;
}
	
UINT *pHeights[] = {
	&g_TestApp.m_RenderTargetOptions.SwapChainDesc.Height, 
	&g_TestApp.m_RenderTargetOptions.RenderTargetTextureDesc.Height,
	&g_TestApp.m_RenderTargetOptions.DepthTextureDesc.Height
};

UINT *pWidths[] = {
	&g_TestApp.m_RenderTargetOptions.SwapChainDesc.Width, 
	&g_TestApp.m_RenderTargetOptions.RenderTargetTextureDesc.Width,
	&g_TestApp.m_RenderTargetOptions.DepthTextureDesc.Width
};

//////////////////////////////////////////////////////////////////////////
// CMultiSampleTest
//////////////////////////////////////////////////////////////////////////
CMultisampleTest::CMultisampleTest( ) : m_pRTTexture2D( NULL ),
										m_pRTView( NULL ),
										m_pConstBuffer( NULL ),
										m_pVertexBuffer( NULL ),
										m_pVertexLayout( NULL ),
										m_pRSEnabled( NULL ),
										m_RTFormat( DXGI_FORMAT_UNKNOWN ),
										m_RTHeight( TriUINT( pHeights ) ),
										m_RTWidth( TriUINT( pWidths ) ),
										m_pBS( NULL ),
										m_pVS( NULL ),
										m_pPS( NULL ),
										m_bMSEnable( true ),
										m_ReadBack( MS_RESOLVE ),
										m_bA2C( false ),
										m_uSampleCount( 1 ),
										m_uSampleQuality( 0 ),
										m_uSampleMask( 0xffffffff ),
										m_uVertexCount( 0 ),
										m_PixPosID( PP_CUSTOM ),
										m_pDevice( NULL ),
										m_PrevDSFormat( DXGI_FORMAT_UNKNOWN ),
										m_PrevRTVFormat( DXGI_FORMAT_UNKNOWN ),
										m_uPrevSampleCount( 0 ),
										m_uPrevForcedSampleCount( 0 ),
										m_PrevQualityLevelScenario( QUALITY_LEVEL_ZERO ),
										m_bDepthTest( false ),
										m_bUpdateFormatOrSampleDesc( true ),
										m_uForcedSampleCount( 0 ),
										m_uEffectiveSampleCount( 0 ),
										m_pDepthStateDefault( NULL ),
										m_pDepthStateDisabled( NULL )
{
	m_vPixPos.x = 0;
	m_vPixPos.y = 0;
	
	g_TestApp.m_RenderTargetOptions.DepthTextureDesc.MipLevels = 
		g_TestApp.m_RenderTargetOptions.RenderTargetTextureDesc.MipLevels =
		g_TestApp.m_RenderTargetOptions.RenderTargetTextureDesc.ArraySize = 1;

	//This updates both the framework's RT texture and swap chain dimensions : look at the DualParam class in multisample.h
	m_RTWidth = 4;
	m_RTHeight = 4;
}


void CMultisampleTest::DecodeTestPath()
{
	size_t index = 0;
	tstring szTest = GetPath().ToString( index++, 1 );

	tstring szSamples = GetPath().ToString( index++, 1 );
	UINT uSamples = atoi(szSamples.c_str());

	while ( 0 == uSamples ) //If we didn't get a number, try that again, because some tests have a deeper path
	{
		szSamples = GetPath().ToString( index++, 1 );
		uSamples = atoi(szSamples.c_str());
	}	

	tstring szFormat = GetPath().ToString( index++, 1 );
	tstring szViewFormat;
	FromString( &m_RTFormat, szFormat );

	if ( 0 < (GetFlags(m_RTFormat) & FF_TYPELESS) )
	{
		szViewFormat = GetPath().ToString( index++, 1 );
		FromString( &m_RTVFormat, szViewFormat );
	}
	else
	{
		m_RTVFormat = m_RTFormat;
	}

	// Quality can be a UINT or a string "STANDARD_PATTERN" if its a 10.1 device
	tstring szQuality = GetPath().ToString( index++, 1 );
	UINT uQuality;
	if (szQuality == "STANDARD_PATTERN")
		uQuality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
	else if (szQuality == "CENTER_PATTERN")
		uQuality = D3D11_CENTER_MULTISAMPLE_PATTERN;
	else
		uQuality = atoi(szQuality.c_str());

	if ( tstring(_T("Depth")) == szTest )
	{
		g_TestApp.m_RenderTargetOptions.DepthTextureDesc.Format = 
			g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.Format = m_DSFormat = m_RTFormat;
		m_bDepthTest = true;

		tstring szRTFormat =  GetPath().ToString( index++, 1 );
		FromString( &m_RTFormat, szRTFormat );
		m_RTVFormat = m_RTFormat;
	}
	else
	{
		m_bDepthTest = false;
	}
	
	g_TestApp.m_RenderTargetOptions.DepthTextureDesc.SampleDesc.Count = 
		g_TestApp.m_RenderTargetOptions.RenderTargetTextureDesc.SampleDesc.Count = 
		m_uSampleCount = uSamples;

	g_TestApp.m_RenderTargetOptions.DepthTextureDesc.SampleDesc.Quality =
		g_TestApp.m_RenderTargetOptions.RenderTargetTextureDesc.SampleDesc.Quality = 
		m_uSampleQuality = uQuality;

	g_TestApp.m_RenderTargetOptions.UseDefaultRenderTargetView = false;
	g_TestApp.m_RenderTargetOptions.RenderTargetViewDesc.Format = m_RTVFormat;
	g_TestApp.m_RenderTargetOptions.RenderTargetTextureDesc.Format = m_RTFormat;

	if ( m_uSampleCount > 1 )
	{
		g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		g_TestApp.m_RenderTargetOptions.RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		g_TestApp.m_RenderTargetOptions.RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		g_TestApp.m_RenderTargetOptions.RenderTargetViewDesc.Texture2D.MipSlice = 0;
	}

	g_TestApp.m_RenderTargetOptions.RenderTargetTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	g_TestApp.m_RenderTargetOptions.NoDepthStencils = true;
}


void CMultisampleTest::InitMultisampleParameters()
{
	
	CTableValueSet< PIX_POS >		*pVSPixPos	= new CTableValueSet< PIX_POS >( PixPos, sizeof(PIX_POS), ARRAY_SIZE(PixPos) );
	CTestCaseParameter< PIX_POS > *pPixPosParam = AddParameter( _T( "PixelPosition" ), &m_PixPosID );
	pPixPosParam->SetDefaultValue( PP_TOP_LEFT );
	m_rfPixPos = AddParameterValueSet< PIX_POS > ( pPixPosParam, pVSPixPos );

	m_pSampleCountParam = AddParameter<UINT>( _T( "Sample_Count" ), &m_uSampleCount );
	m_pQualityLevelParam = AddParameter<QUALITY_LEVEL_SCENARIO>( _T( "Quality_Level" ), &m_QualityLevelScenario );

	testfactor::RFactor rfSampleCountStandard = AddParameterValueSet<UINT>( m_pSampleCountParam, 
		new CTableValueSet<UINT>( StandardSampleCounts, sizeof(StandardSampleCounts[0]), ARRAY_SIZE(StandardSampleCounts) ) );
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

	m_rfSampleDesc = 
		rfSampleCountOne * rfQualityLevelZero + 
		rfSampleCount32 * ( rfQualityLevelZero + rfQualityLevelMaxSupported ) + 
		// only sample counts 1(trival), 2, 4, 8 and 16 support standard and center sample patterns
		rfSampleCountStandard * ( rfQualityLevelZero + rfQualityLevelMaxSupported + rfQualityLevelStandard + rfQualityLevelCenter );

	m_rfSampleDescStandard = ( rfSampleCountOne + rfSampleCountStandard ) * ( rfQualityLevelStandard + rfQualityLevelCenter );


	// Test case parameters for formats
	m_pRTFormatParam = AddParameter( _T( "RT_resource_format" ), &m_RTFormat );
	m_pRTVFormatParam = AddParameter( _T( "RT_view_format" ), &m_RTVFormat );
	// m_RTVFormat is only used when m_RTFormat is typeless
	m_pRTVFormatParam->SetDefaultValue( DXGI_FORMAT_UNKNOWN);
	
	// format value sets
	// if the RT format is typeless, the RTV format needs to cycle through all sub-formats
	testfactor::RFactor rfRTTypeless[TypelessFormatsSize];
	testfactor::RFactor rfRTVSubFormats[TypelessFormatsSize];
	CTableValueSet<DXGI_FORMAT> * pSubFormatValues[TypelessFormatsSize];
	for (size_t i = 0; i < TypelessFormatsSize; i++)
	{
		pSubFormatValues[i] = new CTableValueSet<DXGI_FORMAT>(TypelessFormats[i].pSubset, 
															sizeof(TypelessFormats[i].pSubset[0]), 
															TypelessFormats[i].subsetSize);
		rfRTVSubFormats[i] = AddParameterValueSet< DXGI_FORMAT >(m_pRTVFormatParam, pSubFormatValues[i]);
		rfRTTypeless[i] = AddParameterValue< DXGI_FORMAT >( m_pRTFormatParam, TypelessFormats[i].typelessFormat );
	}

	m_rfRTFormatTypeless = rfRTTypeless[0] * rfRTVSubFormats[0];
	for (size_t i = 1; i < TypelessFormatsSize; i++)
	{
		m_rfRTFormatTypeless = m_rfRTFormatTypeless + rfRTTypeless[i] * rfRTVSubFormats[i];
	}

	// when the RT format is not typeless
	CTableValueSet<DXGI_FORMAT> *pSingularFormatValues = 
		new CTableValueSet<DXGI_FORMAT>(SingularFormats, sizeof( SingularFormats[0]), ARRAY_SIZE(SingularFormats) );

	m_rfRTFormatTypedAll = AddParameterValueSet< DXGI_FORMAT >( m_pRTFormatParam, pSingularFormatValues); 
	for (size_t i = 0; i < TypelessFormatsSize; i++)
	{
		m_rfRTFormatTypedAll = m_rfRTFormatTypedAll + AddParameterValueSet< DXGI_FORMAT >( m_pRTFormatParam, pSubFormatValues[i]);
	}

	// _UINT formats
	CUserValueSet<DXGI_FORMAT> *pUINTFormatValues = new CUserValueSet<DXGI_FORMAT>;
	for (size_t i = 0; i < TypelessFormatsSize; i++)
	{
		for (size_t j = 0; j < TypelessFormats[i].subsetSize; j++)
		{
			if ( GetFlags( TypelessFormats[i].pSubset[j] ) & FF_UINT )
				pUINTFormatValues->AddValue( TypelessFormats[i].pSubset[j] );
		}
	}
	m_rfRTFormatTypedUINT = AddParameterValueSet< DXGI_FORMAT >( m_pRTFormatParam, pUINTFormatValues );

	// a short list of formats
		
	testfactor::RFactor rfCompactFormatTyped = AddParameterValueSet< DXGI_FORMAT >( m_pRTFormatParam,			
												new CTableValueSet<DXGI_FORMAT>( CompactMultisampleFormats, 
																				sizeof(CompactMultisampleFormats[0]), 
																				ARRAY_SIZE(CompactMultisampleFormats)) );
	testfactor::RFactor rfCompactFormatTypeless = AddParameterValue< DXGI_FORMAT >( m_pRTFormatParam, 
																					CompactTypelessFormats[0].typelessFormat );
	testfactor::RFactor rfCompactRTVSubFormat = AddParameterValueSet< DXGI_FORMAT >( m_pRTVFormatParam,			
												new CTableValueSet<DXGI_FORMAT>( CompactTypelessFormats[0].pSubset, 
																				sizeof(CompactTypelessFormats[0].pSubset[0]), 
																				CompactTypelessFormats[0].subsetSize) );

	m_rfRTFormatTypedSubset = rfCompactFormatTyped + rfCompactFormatTypeless * rfCompactRTVSubFormat; 

	AddPriorityPatternFilter(FilterEqual<UINT>(m_pSampleCountParam, 1), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(m_pSampleCountParam, 2), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(m_pSampleCountParam, 4), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(m_pRTVFormatParam, DXGI_FORMAT_UNKNOWN), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(m_pRTFormatParam, DXGI_FORMAT_R8G8B8A8_UNORM), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(m_pRTFormatParam, DXGI_FORMAT_R32G32B32A32_FLOAT), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(m_pRTFormatParam, DXGI_FORMAT_R10G10B10A2_UINT), WeightFactorVeryLarge);

	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(m_pRTVFormatParam, DXGI_FORMAT_R8G8B8A8_UNORM), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(m_pRTVFormatParam, DXGI_FORMAT_R32G32B32A32_FLOAT), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(m_pRTVFormatParam, DXGI_FORMAT_R10G10B10A2_UINT), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(m_pRTFormatParam, DXGI_FORMAT_R8G8B8A8_TYPELESS), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(m_pRTFormatParam, DXGI_FORMAT_R32G32B32A32_TYPELESS), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(m_pRTFormatParam, DXGI_FORMAT_R10G10B10A2_TYPELESS), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue(1, 3.0f);
	DeclarePriorityLevelByPriorityValue(2, 1.0f);
}

TEST_RESULT CMultisampleTest::Setup()
{
	// Local variables
	HRESULT hr = E_FAIL;

	m_pDevice = GetDevice1();
	m_pDeviceContext = GetEffectiveContext1();

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
		WriteToLog( _T( "CMultisampleTest::CreateDepthStencilState failed with %s while creating the depth disabled state." ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

TEST_RESULT CMultisampleTest::SetFormatAndQualityLevel( bool isDSFormat )
{
	HRESULT hr = S_OK;
	UINT caps;
	UINT levels;

	if ( (m_QualityLevelScenario == QUALITY_LEVEL_STANDARD || m_QualityLevelScenario == QUALITY_LEVEL_CENTER) && 
		 g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_1 )
	{
		WriteToLog(_T("STANDARD and CENTER quality levels are not available on feature level 10.0 and under."));	
		SkipMatching( GetSkipMatchingFilter() || FilterEqual<QUALITY_LEVEL_SCENARIO>(_T("Quality_Level"), m_QualityLevelScenario));
		return RESULT_SKIP;
	}

	if ( isDSFormat )
	{
		hr = m_pDevice->CheckMultisampleQualityLevels( m_DSFormat, m_uSampleCount, &levels );
		if( FAILED( hr ) )
		{
			WriteToLog( _T( "CMultisampleTest::SetupTestCase(): CheckMultisampleQualityLevels() failed." ) );
			return RESULT_FAIL;
		}
	
		UINT levelsRT;
		// find a render target with that supports the same QLs for this sample count
		size_t j = 0;
		for (; j < sizeof( CompactResolveFormats ); j++)
		{
			hr = m_pDevice->CheckMultisampleQualityLevels( CompactResolveFormats[j], m_uSampleCount, &levelsRT );
			if( FAILED( hr ) )
			{
				WriteToLog( _T( "CMultisampleTest::SetupTestCase(): CheckMultisampleQualityLevels() failed." ) );
				return RESULT_FAIL;
			}
			if ( levelsRT >= levels )
			{
				m_RTVFormat = m_RTFormat = CompactResolveFormats[j];
				break;
			}
		}
		// couldn't find a match
		if ( j == sizeof( CompactResolveFormats ) )
		{
			WriteToLog( _T( "CMultisampleTest::SetupTestCase(): failed to find a matching RT format." ) );
			return RESULT_FAIL;
		}

		g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.Format = m_DSFormat;
		g_TestApp.m_RenderTargetOptions.NoDepthStencils = false;
	}
	else
	{	
		if ( !( GetFlags(m_RTFormat) & FF_TYPELESS ) )
			m_RTVFormat = m_RTFormat;

		// checking render target support
		hr = m_pDevice->CheckFormatSupport( m_RTVFormat, &caps );
		if( FAILED( hr ) )
		{
			// If the driver doesn't support the 11.1 DDI then it might fail for some formats
			if( !GetFramework()->Is11_1DDISupported( m_pDevice ) &&			// Check to see if the driver supports the 11.1 DDI
				CD3D10FormatHelper_Test::Is16BitBGRFormat( m_RTVFormat ) )	// The 16 bit formats (B5G6R5, etc...) are only required to pass CheckFormatSupport on the 11.1 DDI
			{
				WriteToLog( _T( "CMultisampleTest::SetupTestCase(): CheckFormatSupport() failed. However the driver doesn't support the D3D11.1 DDI, so this is OK for the 16-bit formats. " ) );
				return RESULT_SKIP;
			}

			WriteToLog( _T( "CMultisampleTest::SetupTestCase(): CheckFormatSupport() failed. HR = %s." ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		if ( 1 == m_uSampleCount && !(caps & D3D11_FORMAT_SUPPORT_RENDER_TARGET) )
		{
			WriteToLog( _T( "CMultisampleTest::SetupTestCase(): Format does not support RENDER_TARGET." ) );
			return RESULT_SKIP;
		}
		else if ( 1 < m_uSampleCount && !(caps & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET) )
		{
			WriteToLog( _T( "CMultisampleTest::SetupTestCase(): Format does not support MULTISAMPLE_RENDERTARGET." ) );
			return RESULT_SKIP;
		}
		g_TestApp.m_RenderTargetOptions.NoDepthStencils = true;
	}

	// checking quality levels support
	if ( isDSFormat )
		hr = m_pDevice->CheckMultisampleQualityLevels( m_DSFormat, m_uSampleCount, &levels );
	else
		hr = m_pDevice->CheckMultisampleQualityLevels( m_RTVFormat, m_uSampleCount, &levels );

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CMultisampleTest::SetupTestCase(): CheckMultisampleQualityLevels() failed." ) );
		return RESULT_FAIL;
	}
	if ( levels == 0)
	{
		WriteToLog( _T( "CMultisampleTest::SetupTestCase(): SampleCount is not supported on the format." ) );
		return RESULT_SKIP;
	}
	if ( levels == 1 && m_QualityLevelScenario == QUALITY_LEVEL_MAX_SUPPORTED )
	{
		WriteToLog( _T( "CMultisampleTest::SetupTestCase(): Max supported quality level is 0." ) );
		return RESULT_SKIP;
	}

	switch( m_QualityLevelScenario )
	{
	case QUALITY_LEVEL_ZERO:
		m_uSampleQuality = 0;
		break;
	case QUALITY_LEVEL_MAX_SUPPORTED:
		{
		m_uSampleQuality = levels - 1;
		WriteToLog( _T( "Max supported quality level is %d." ), levels - 1 );
		}
		break;
	case QUALITY_LEVEL_STANDARD:
		m_uSampleQuality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
		break;
	case QUALITY_LEVEL_CENTER:
		m_uSampleQuality = D3D11_CENTER_MULTISAMPLE_PATTERN;
		break;
	default:
		WriteToLog(_T("Unexpected Quality Level Scenario Value!"));
		return RESULT_FAIL;
	}

	g_TestApp.m_RenderTargetOptions.DepthTextureDesc.SampleDesc.Count = 
		g_TestApp.m_RenderTargetOptions.RenderTargetTextureDesc.SampleDesc.Count = 
		m_uSampleCount;

	g_TestApp.m_RenderTargetOptions.DepthTextureDesc.SampleDesc.Quality =
		g_TestApp.m_RenderTargetOptions.RenderTargetTextureDesc.SampleDesc.Quality = 
		m_uSampleQuality;

	g_TestApp.m_RenderTargetOptions.UseDefaultRenderTargetView = false;
	g_TestApp.m_RenderTargetOptions.RenderTargetViewDesc.Format = m_RTVFormat;
	g_TestApp.m_RenderTargetOptions.RenderTargetTextureDesc.Format = m_RTFormat;

	if ( m_uSampleCount > 1 )
	{
		g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		g_TestApp.m_RenderTargetOptions.RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		g_TestApp.m_RenderTargetOptions.DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		g_TestApp.m_RenderTargetOptions.RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		g_TestApp.m_RenderTargetOptions.RenderTargetViewDesc.Texture2D.MipSlice = 0;
	}

	g_TestApp.m_RenderTargetOptions.RenderTargetTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	
	g_TestApp.CleanupRenderTarget();
	TEST_RESULT ts = g_TestApp.SetupRenderTarget();
	if (ts == RESULT_FAIL)
	{
		WriteToLog( _T( "CMultisampleTest::SetFormatAndQualityLevel: SetupRenderTargets failed." ) );
		return ts;
	}

	return RESULT_PASS;
}

// determine if the related pipeline status need to be updated due to the change of format or SampleDesc
bool CMultisampleTest::UpdateFormatOrSampleDesc()
{
	m_bUpdateFormatOrSampleDesc = false;
	if ( m_bDepthTest )
	{
		if ( m_PrevDSFormat != m_DSFormat)
		{
			m_PrevDSFormat = m_DSFormat;
			m_bUpdateFormatOrSampleDesc = true;
		}
	}
	
	if ( m_PrevRTVFormat != m_RTVFormat || 
		m_uPrevSampleCount != m_uSampleCount ||
		m_uPrevForcedSampleCount != m_uForcedSampleCount ||
		m_PrevQualityLevelScenario != m_QualityLevelScenario )
	{
		m_PrevRTVFormat = m_RTVFormat;
		m_uPrevSampleCount = m_uSampleCount;
		m_uPrevForcedSampleCount = m_uForcedSampleCount;
		m_PrevQualityLevelScenario = m_QualityLevelScenario;
		m_bUpdateFormatOrSampleDesc = true;
	}

	return m_bUpdateFormatOrSampleDesc;
}



void CMultisampleTest::Cleanup()
{
	SAFE_RELEASE( m_pDepthStateDefault );
	SAFE_RELEASE( m_pDepthStateDisabled );
}


///
// NOTE:	if something needs to be changed in SetupD3D for your test, it probably means
//			that the code should be moved to each test's Initialize function
///
TEST_RESULT CMultisampleTest::SetupD3D()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;
	ID3D10Blob *pVSBuf = NULL;
	ID3D10Blob *pPSBuf = NULL;

	try 
	{
		UINT caps;

		if ( !m_bDepthTest )
		{
			m_pDevice->CheckFormatSupport( m_RTVFormat, &caps );
			if (!(caps & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE) &&
				!(caps & D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD))
				throw TRException("Format doesn't support multisample load or resolve.", TEST_RESULT(RESULT_SKIP));
		}

		//
		// Setup the default viewport
		//
		D3D11_VIEWPORT vp;
		vp.Height = (float) m_RTHeight;
		vp.Width = (float) m_RTWidth;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_pDeviceContext->RSSetViewports( 1, &vp );

		//
		// Set Rasterizer state
		//

		// If a forced sample count is being used, make sure the driver supports it
		// Forced sample counts (TIR) is only valid on a driver that supports the 11.1 DDI. Forced sample count == 0 means TIR is off.
		if( !GetFramework()->Is11_1DDISupported( m_pDevice ) &&
			m_uForcedSampleCount > 0 )
		{
			WriteToLog( _T( "Skipping test case because the driver doesn't support the 11.1 DDI and forced sample count > 0." ) );
			return RESULT_SKIP;
		}

		// FL11.1+ requires support for all sample counts. FL10.0-FL11.0 require support for forced sample count 1.
		if( m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_1 &&
			m_uForcedSampleCount > 1 )
		{
			WriteToLog( _T( "Skipping test case because forced sample count's > 1 are only supported on FL11.1+ HW." ) );
			return RESULT_SKIP;
		}

		// Win8:307101 - Sample count 1 is only valid on D3D_FEATURE_LEVEL_11_0 or lower when MultisampleEnable is false
		const bool bMultisampleEnable = ((m_uForcedSampleCount == 1) && (GetDevice1()->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_1)) ? false : m_bMSEnable;

		D3D11_RASTERIZER_DESC1 rastDesc;
		rastDesc.CullMode = D3D11_CULL_NONE;
		rastDesc.FrontCounterClockwise = true;
		rastDesc.DepthBias = false;
		rastDesc.DepthBiasClamp = 0;
		rastDesc.SlopeScaledDepthBias = 0;
		rastDesc.DepthClipEnable = true;
		rastDesc.ScissorEnable = false;
		rastDesc.MultisampleEnable = bMultisampleEnable;
		rastDesc.AntialiasedLineEnable = false;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.ForcedSampleCount = m_uForcedSampleCount;
		if( FAILED( hr = m_pDevice->CreateRasterizerState1( &rastDesc, &m_pRSEnabled ) ) )
			throw TRException("CreateRasterizerState for m_pRSEnabled failed.", hr, RESULT_FAIL);


		m_pDeviceContext->RSSetState( m_pRSEnabled );
		m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

		//
		// Set Blend state
		//
		D3D11_BLEND_DESC blendDesc;	
		blendDesc.AlphaToCoverageEnable = m_bA2C;
		blendDesc.RenderTarget[0].BlendEnable = FALSE;  // TODO add alpha param
		blendDesc.RenderTarget[1].BlendEnable = FALSE;
		blendDesc.RenderTarget[2].BlendEnable = FALSE;
		blendDesc.RenderTarget[3].BlendEnable = FALSE;
		blendDesc.RenderTarget[4].BlendEnable = FALSE;
		blendDesc.RenderTarget[5].BlendEnable = FALSE;
		blendDesc.RenderTarget[6].BlendEnable = FALSE;
		blendDesc.RenderTarget[7].BlendEnable = FALSE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = 0xf;
		blendDesc.RenderTarget[1].RenderTargetWriteMask = 0xf;
		blendDesc.RenderTarget[2].RenderTargetWriteMask = 0xf;
		blendDesc.RenderTarget[3].RenderTargetWriteMask = 0xf;
		blendDesc.RenderTarget[4].RenderTargetWriteMask = 0xf;
		blendDesc.RenderTarget[5].RenderTargetWriteMask = 0xf;
		blendDesc.RenderTarget[6].RenderTargetWriteMask = 0xf;
		blendDesc.RenderTarget[7].RenderTargetWriteMask = 0xf;
		if( FAILED( hr = m_pDevice->CreateBlendState( &blendDesc, &m_pBS ) ) )
			throw TRException("CreateBlendState failed.", hr, RESULT_FAIL);

		m_pDeviceContext->OMSetBlendState( m_pBS, 0, D3D10_DEFAULT_SAMPLE_MASK );

		//
		// Create the constant buffer
		//
		D3D11_BUFFER_DESC bufdesc;
		bufdesc.ByteWidth = CONST_BUFFER_SIZE;
		bufdesc.Usage = D3D11_USAGE_DYNAMIC;
		bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufdesc.MiscFlags = 0;
		if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufdesc, NULL, &m_pConstBuffer ) ) )
			throw TRException("CreateBuffer for m_pConstBuffer failed.", hr, RESULT_FAIL);

		m_pDeviceContext->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		m_pDeviceContext->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		m_pDeviceContext->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );

		//
		// Create Shaders
		//

		ID3D10Blob *pError(NULL);
		if ( FAILED( hr = D3DX10CompileFromResource(NULL, "vs.vsh", NULL, NULL, NULL, "main", "vs_4_0", 0, 0, NULL, &pVSBuf, &pError, NULL) ) )
		{
			if (pError) 
				WriteToLog("%s", pError->GetBufferPointer());
			throw TRException("CompileShaderFromResource Failed.", hr, RESULT_FAIL);
		}
		SAFE_RELEASE(pError);


		if ( FAILED( hr = m_pDevice->CreateVertexShader( (DWORD*) pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), NULL, &m_pVS ) ) )
			throw TRException("CreateVertexShader Failed.", hr, RESULT_FAIL);

		if ( FAILED( hr = D3DX10CompileFromResource(NULL, "ps.psh", NULL, NULL, NULL, "main", "ps_4_0", 0, 0, NULL, &pPSBuf, &pError, NULL ) ) )
		{
			if (pError) 
				WriteToLog("%s", pError->GetBufferPointer());
			throw TRException("CompileShaderFromResource Failed.", hr, RESULT_FAIL);
		}
		SAFE_RELEASE(pError);

		if ( FAILED( hr = m_pDevice->CreatePixelShader( (DWORD*) pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &m_pPS ) ) )
			throw TRException("CreatePixelShader Failed.", hr, RESULT_FAIL);

		m_pDeviceContext->VSSetShader( m_pVS, NULL, 0 );
		m_pDeviceContext->GSSetShader( NULL, NULL, 0 );
		m_pDeviceContext->PSSetShader( m_pPS, NULL, 0 );

		// Define our vertex data layout
		const D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "tex", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if(	FAILED(	hr = m_pDevice->CreateInputLayout( layout, 2, pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), &m_pVertexLayout ) ) )
			throw TRException("CreateInputLayout for m_pVertexLayout Failed.", hr, RESULT_FAIL);

		m_pDeviceContext->IASetInputLayout( m_pVertexLayout );

		//
		// Create vertex buffer
		//
		D3D11_BUFFER_DESC bufferDesc =
		{
			100 * sizeof( VERTEX ),
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_VERTEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE,
			0
		};
		if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufferDesc, NULL, &m_pVertexBuffer ) ) )
			throw TRException("CreateBuffer for m_pVertexBuffer Failed.", hr, RESULT_FAIL);

		UINT strides[] = { sizeof( VERTEX ) };
		UINT offsets[] = { 0 };
		m_pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, strides, offsets );

		//
		// setup common test case data
		//
		ResolvePixPos();

		//
		// setup const data
		//
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

		SetupRenderTarget();

		POINT pix = {m_PixBox.left, m_PixBox.top};
		tRes = m_Finder.Setup( GetFramework(), m_pRTView, m_pRTTexture2D, pix, m_uForcedSampleCount );
		if (tRes != RESULT_PASS)
		{
			throw TRException("Setup SampleFinder Failed", E_FAIL, RESULT_FAIL);
		}
	}
	catch (TRException& exc)
	{
		SAFE_RELEASE( pVSBuf );
		SAFE_RELEASE( pPSBuf );	
		exc.Prepend("[CMultisampleTest::SetupD3D()] ");
		throw;
	}

	SAFE_RELEASE( pVSBuf );
	SAFE_RELEASE( pPSBuf );

	return tRes;
}

TEST_RESULT CMultisampleTest::SetupRenderTarget()
{
	m_pRTTexture2D = g_TestApp.GetRTTexture();
	m_pRTView = g_TestApp.GetRTView();
	return RESULT_PASS;
}

void CMultisampleTest::ResetStates()
{
	UINT strides[] = { sizeof( VERTEX ) };
	UINT offsets[] = { 0 };
	ID3D11Buffer *pNullBuffers[] = { NULL };

	m_pDeviceContext->OMSetBlendState( m_pBS, 0, D3D10_DEFAULT_SAMPLE_MASK );
	m_pDeviceContext->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	m_pDeviceContext->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	m_pDeviceContext->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	m_pDeviceContext->VSSetShader( m_pVS, NULL, 0 );
	m_pDeviceContext->GSSetShader( NULL, NULL, 0 );
	m_pDeviceContext->PSSetShader( m_pPS, NULL, 0 );
	m_pDeviceContext->IASetInputLayout( m_pVertexLayout );
	m_pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, strides, offsets );
	m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, NULL );
	m_pDeviceContext->RSSetState( m_pRSEnabled );
	m_pDeviceContext->SOSetTargets( 1, pNullBuffers, offsets );
}
// Adds a quad to the vertex buffer covering the specified pixel
void CMultisampleTest::CoverPixel( UINT px, UINT py )
{
	HRESULT hr;
	VERTEX *pDestVerts;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	float x = (float)px;
	float y = (float)py;

	//Fill vertex buffer
	if( FAILED( hr = GetEffectiveContext()->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
		throw TRException("[CMultisampleTest::CoverPixel] Map on m_pVertexBuffer failed.", hr, RESULT_FAIL);
	pDestVerts = (VERTEX*) mappedRes.pData;

	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(x, y);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(0, 0);
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(x, y+1);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(0, 1);
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(x+1, y);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(1, 0);
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(x+1, y+1);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(1, 1);

	GetEffectiveContext()->Unmap(m_pVertexBuffer,0);
}

void CMultisampleTest::CoverPixel( )
{
	CoverPixel(m_vPixPos.x, m_vPixPos.y);
}

// Adds a triangle to the vertex buffer covering only the specified sample location in the pixel
void CMultisampleTest::CoverSample( UINT px, UINT py, UINT uSampleIndex )
{
	HRESULT hr;
	if( m_uForcedSampleCount == 0 )
	{
		if( uSampleIndex >= m_uSampleCount )
		{
			WriteToLog( _T("Forced sample count = 0 (TIR is OFF) and the sample count is %u, but the sample index is %u."), m_uSampleCount, uSampleIndex );
			throw TRException("[CMultisampleTest::CoverSample] Invalid sample index.");
		}
	}
	else
	{
		if( uSampleIndex >= m_uForcedSampleCount )
		{
			WriteToLog( _T("Forced sample count = %u (TIR is ON), but the sample index is %u."), m_uForcedSampleCount, uSampleIndex );
			throw TRException("[CMultisampleTest::CoverSample] Invalid sample index.");
		}
	}

	POINT loc = m_Finder.GetLoc(uSampleIndex);
	float x = px + loc.x/256.0f;
	float y = py + loc.y/256.0f;
	float size = 1.0/256;

	VERTEX *pDestVerts;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	//Fill vertex buffer
	if( FAILED( hr = GetEffectiveContext()->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
		throw TRException("[CMultisampleTest::CoverSample] Map on m_pVertexBuffer failed.", hr, RESULT_FAIL);
	pDestVerts = (VERTEX*) mappedRes.pData;
	
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(x, y);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(0, 0);
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(x+size, y);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(1, 0);
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(x, y+size);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(0, 1);
	
	GetEffectiveContext()->Unmap(m_pVertexBuffer,0);
}

void CMultisampleTest::CoverSample( UINT uSampleIndex )
{
	CoverSample( m_vPixPos.x, m_vPixPos.y, uSampleIndex );
}

// Adds a quad to the vertex buffer covering the buffer
void CMultisampleTest::CoverBuffer( )
{
	HRESULT hr;
	VERTEX *pDestVerts;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	float width = (float)m_RTWidth;
	float height = (float)m_RTHeight;

	//Fill vertex buffer
	if( FAILED( hr = GetEffectiveContext()->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
		throw TRException("[CMultisampleTest::CoverPixel] Map on m_pVertexBuffer failed.", hr, RESULT_FAIL);
	pDestVerts = (VERTEX*) mappedRes.pData;

	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(0, 0);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(0, 0);
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(0, height);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(0, 1);
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(width, 0);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(1, 0);
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(width, height);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(1, 1);

	GetEffectiveContext()->Unmap(m_pVertexBuffer,0);
}

// Adds a triangle that covers approximately 1/2 of the samples in a pixel
void CMultisampleTest::CoverHalfPixel( UINT px, UINT py )
{
	HRESULT hr;

	float x = (float)px;
	float y = (float)py;
	float size = 1.0f;   

	VERTEX *pDestVerts;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	//Fill vertex buffer
	if( FAILED( hr = GetEffectiveContext()->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
		throw TRException("[CMultisampleTest::CoverSample] Map on m_pVertexBuffer failed.", hr, RESULT_FAIL);
	pDestVerts = (VERTEX*) mappedRes.pData;
	
	// These coordinates were are such that the pixel center is covered, which is assumed by the CInputCoverage test
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(x+size, y);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(0, 0);
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(x+size, y+size);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(1, 0);
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(x, y+size);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(0, 1);
	
	GetEffectiveContext()->Unmap(m_pVertexBuffer,0);
}

void CMultisampleTest::ResolvePixPos( )
{
	// custom pix positions should have m_vPixPos already set
	switch( m_PixPosID )
	{
	case PP_TOP_LEFT:
		m_vPixPos.x = 0;
		m_vPixPos.y = 0;
		break;
	case PP_TOP_RIGHT:
		m_vPixPos.x = m_RTWidth - 1;
		m_vPixPos.y = 0;
		break;
	case PP_BOTTOM_LEFT:
		m_vPixPos.x = 0;
		m_vPixPos.y = m_RTHeight - 1;
		break;
	case PP_BOTTOM_RIGHT:
		m_vPixPos.x = m_RTWidth - 1;
		m_vPixPos.y = m_RTHeight - 1;
		break;
	//case default:
		//TODO throw error
	}

	m_PixBox.left = m_vPixPos.x;
	m_PixBox.right = m_vPixPos.x + 1;
	m_PixBox.top = m_vPixPos.y;
	m_PixBox.bottom = m_vPixPos.y + 1;
	m_PixBox.front = 0;
	m_PixBox.back = 1;
}

void CMultisampleTest::CleanupD3D()
{
	m_Finder.Cleanup();

	ID3D11RenderTargetView* pRTNull[] = { NULL, NULL, NULL, NULL };
	ID3D11ShaderResourceView	*pSRVNull[] = { NULL, NULL, NULL, NULL };
	ID3D11Buffer* pBufferNull[] = { NULL };
	ID3D11Buffer *pNullBuffers[] = { NULL };
	UINT offsets[] = { 0 };

	if ( m_pDevice ) //otherwise, we failed to create the device in the first place
	{
		m_pDeviceContext->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );
		m_pDeviceContext->IASetInputLayout( NULL );
		m_pDeviceContext->IASetVertexBuffers( 0, 1, pNullBuffers, offsets, offsets );
		m_pDeviceContext->VSSetShader( NULL, NULL, 0 );
		m_pDeviceContext->GSSetShader( NULL, NULL, 0 );
		m_pDeviceContext->PSSetShader( NULL, NULL, 0 );
		m_pDeviceContext->PSSetShaderResources( 0, 4, pSRVNull );
		m_pDeviceContext->OMSetRenderTargets( 4, pRTNull, NULL );
		m_pDeviceContext->VSSetConstantBuffers( 0, 1, pBufferNull );
		m_pDeviceContext->GSSetConstantBuffers( 0, 1, pBufferNull );
		m_pDeviceContext->PSSetConstantBuffers( 0, 1, pBufferNull );
		m_pDeviceContext->RSSetState( NULL );
		m_pDeviceContext->SOSetTargets( 1, pNullBuffers, offsets );

		SAFE_RELEASE( m_pVS );
		SAFE_RELEASE( m_pPS );

		m_pRTView = NULL;
		m_pRTTexture2D = NULL;
		SAFE_RELEASE( m_pConstBuffer );

		SAFE_RELEASE( m_pRSEnabled );
		SAFE_RELEASE( m_pBS );

		SAFE_RELEASE( m_pVertexLayout );
		SAFE_RELEASE( m_pVertexBuffer );
	}
}

void CMultisampleTest::Present()
{
	HRESULT hr;
	if ( !g_TestApp.m_bPresent && !g_TestApp.ViewResources() )
		return;

	ID3D10Blob			*pPSBuf = NULL;
	ID3D11PixelShader	*pPS = NULL;
	ID3D11Texture2D		*pSwapChainTexture = NULL;

	ID3D11RenderTargetView			*pRTView = NULL;
	D3D11_RENDER_TARGET_VIEW_DESC rvDesc;

	ID3D11ShaderResourceView				*pSRView = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC	svDesc;

	ID3D11SamplerState		*pSamState = NULL;
	D3D11_SAMPLER_DESC	samDesc;

	ID3D11SamplerState		*pNullState[] = { NULL, NULL, NULL, NULL };
	ID3D11ShaderResourceView	*pSRVNull[] = { NULL, NULL, NULL, NULL };

	VERTEX				*pDestVerts;

	m_pDeviceContext->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );
	m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	//
	// Create Shader
	//
	char szPSH[] =
		"Texture2D<%s> Tex;													\n"

		"struct PSIn														\n"
		"{																	\n"
		"	float4 pos : SV_Position;										\n"
		"	float2 tex : tex;												\n"
		"};																	\n"

		"struct PSOut														\n"
		"{																	\n"
		"	float4 Color : SV_Target;										\n"
		"};																	\n"

		"cbuffer cb0 : register(b0)											\n"
		"{																	\n"
		"	float4 inverseViewport;											\n"
		"	float alpha;													\n"
		"};																	\n"

		"PSOut main(PSIn input)												\n"
		"{																	\n"
		"	PSOut output;													\n"
		"	%s color;														\n"
		"	int2 Loc = { input.pos.x, input.pos.y };						\n"

		"	color = Tex.Load(int3(Loc.x, Loc.y, 0));						\n"
		
		"	output.Color.x = (float)color.x;								\n"
		"	output.Color.y = (float)color.y;								\n"
		"	output.Color.z = (float)color.z;								\n"
		"	output.Color.w = (float)color.w;								\n"

		"	return output;													\n"
		"}";

	char *szType;
	char szFloat4[] = "float4";
	char szUint4[] = "uint4";
	char szInt4[] = "int4";
	char szShader[ 4000 ];
	// get description for format
	D3D11_TEXTURE2D_DESC rtDesc;
	m_pRTTexture2D->GetDesc( &rtDesc );
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	m_pRTView->GetDesc(&rtvDesc);
	DWORD flags = GetFlags( rtvDesc.Format );

	if (flags & FF_UINT)
		szType = szUint4;
	else if (flags & FF_SINT)
		szType = szInt4;
	else
		szType = szFloat4;

	UINT uRedBits = 1, uGreenBits = 1, uBlueBits = 1, uAlphaBits = 1;

	if ( flags & (FF_SINT | FF_UINT) )
	{
		uRedBits = GetBitsPerComponent( 0, rtDesc.Format );
		uGreenBits = GetBitsPerComponent( 0, rtDesc.Format );
		uBlueBits = GetBitsPerComponent( 0, rtDesc.Format );
		uAlphaBits = GetBitsPerComponent( 0, rtDesc.Format );

		if ( 0 == uRedBits )
		{
			uRedBits = 1;
		}
		else
		{
			if ( FF_SINT == flags )
				uRedBits = ( 1 << (uRedBits - 1) ) - 1;
			else
				uRedBits = 0x20 == uRedBits? -1: ( 1 << uRedBits ) - 1;
		}
		if ( 0 == uGreenBits )
		{
			uGreenBits = 1;
		}
		else
		{
			if ( FF_SINT == flags )
				uGreenBits = ( 1 << (uGreenBits - 1) ) - 1;
			else
				uGreenBits = 0x20 == uGreenBits? -1: ( 1 << uGreenBits ) - 1;
		}
		if ( 0 == uBlueBits )
		{
			uBlueBits = 1;
		}
		else
		{
			if ( FF_SINT == flags )
				uBlueBits = ( 1 << (uBlueBits - 1) ) - 1;
			else
				uBlueBits = 0x20 == uBlueBits? -1: ( 1 << uBlueBits ) - 1;
		}
		if ( 0 == uAlphaBits )
		{
			uAlphaBits = 1;
		}
		else
		{
			if ( FF_SINT == flags )
				uAlphaBits = ( 1 << (uAlphaBits - 1) ) - 1;
			else
				uAlphaBits = 0x20 == uAlphaBits? -1: ( 1 << uAlphaBits ) - 1;
		}
	}

	// construct PS, requires output type info
	_snprintf( szShader, 3999, szPSH, szType, szType, uRedBits, uGreenBits, uBlueBits, uAlphaBits );
	szShader[3999] = 0;

	ID3D10Blob *pbug;

	if ( FAILED( hr = D3DX10CompileFromMemory( szShader, strlen(szShader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, NULL, &pPSBuf, &pbug, NULL ) ) )
		throw TRException("[CMultisampleTest::Present] CompileShaderFromResource Failed.", hr, RESULT_FAIL);

	//LPCSTR szerr = (LPCSTR)pbug->GetBufferPointer();
	//WriteToLog(szerr);

	if ( FAILED( hr = m_pDevice->CreatePixelShader( (DWORD*) pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &pPS ) ) )
		throw TRException("[CMultisampleTest::Present] CreatePixelShader Failed.", hr, RESULT_FAIL);

	SAFE_RELEASE( pPSBuf );

	m_pDeviceContext->PSSetShader( pPS, NULL, 0 );
	m_pDeviceContext->VSSetShader( m_pVS, NULL, 0 );

	UINT strides[] = { sizeof( VERTEX ) };
	UINT offsets[] = { 0 };

	m_pDeviceContext->IASetInputLayout( m_pVertexLayout );
	m_pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, strides, offsets );
	m_pDeviceContext->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	m_pDeviceContext->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	m_pDeviceContext->RSSetState( m_pRSEnabled );

	//
	// Create Resource View
	//
	svDesc.Format = m_RTVFormat;
	svDesc.Texture2D.MipLevels = 1;
	svDesc.Texture2D.MostDetailedMip = 0;
	svDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	m_pDevice->CreateShaderResourceView( m_Finder.ResolveTex(), &svDesc, &pSRView );
	
	m_pDeviceContext->PSSetShaderResources( 0, 1, &pSRView );

	//
	// Create Sampler
	//
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samDesc.MipLODBias = 0;
	samDesc.MaxAnisotropy = 1;
	samDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samDesc.BorderColor[0] = 0.0f;
	samDesc.BorderColor[1] = 0.0f;
	samDesc.BorderColor[2] = 0.0f;
	samDesc.BorderColor[3] = 0.0f;
	samDesc.MinLOD = -FLT_MAX;
	samDesc.MaxLOD = FLT_MAX;
	m_pDevice->CreateSamplerState( &samDesc, &pSamState );
	m_pDeviceContext->PSSetSamplers( 0, 1, &pSamState );

	//
	// Fill vertex buffer
	//
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if( FAILED( hr = GetEffectiveContext()->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
		throw TRException("[CMultisampleTest::Present] Map on m_pVertexBuffer failed.", hr, RESULT_FAIL);
	pDestVerts = (VERTEX*) mappedRes.pData;

	m_uVertexCount = 0;
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(0.0f, 0.0f);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(0.0f, 0.0f);
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2((FLOAT)m_RTWidth, 0.0f);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(1.0f, 0.0f);
	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2(0.0f, (FLOAT)m_RTHeight);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(0.0f, 1.0f);

	pDestVerts[m_uVertexCount].pos = D3DXVECTOR2((FLOAT)m_RTWidth, (FLOAT)m_RTHeight);
	pDestVerts[m_uVertexCount++].tex = D3DXVECTOR2(1.0f, 1.0f);

	GetEffectiveContext()->Unmap(m_pVertexBuffer,0);

	rvDesc.Format = g_TestApp.m_RenderTargetOptions.SwapChainDesc.Format;
	rvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rvDesc.Texture2D.MipSlice = 0;

	if (g_TestApp.m_bPresent)
	{
		IDXGISwapChain * pSwapChain = g_TestApp.GetSwapChain();
		if ( FAILED( hr = pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**) &pSwapChainTexture ) ) )
			throw TRException("[CMultisampleTest::Present] CreateTexture2DFromSwapChain failed.", hr, RESULT_FAIL);
	
		if ( FAILED( hr = m_pDevice->CreateRenderTargetView( pSwapChainTexture, &rvDesc, &pRTView ) ) )
			throw TRException("[CMultisampleTest::Present] CreateRenderTargetView failed.", hr, RESULT_FAIL);	
	
		m_pDeviceContext->OMSetRenderTargets( 1, &pRTView, NULL );
		m_pDeviceContext->Draw( m_uVertexCount, 0 );

		g_TestApp.Present( NULL, NULL, NULL, NULL, NULL, NULL );
	}
	else
	{
		WindowsTest::CHandle<ID3D11Texture2D> pTexture;
		D3D11_TEXTURE2D_DESC Tex2DDesc = {};
		Tex2DDesc.Width = g_TestApp.m_RenderTargetOptions.SwapChainDesc.Width;
		Tex2DDesc.Height = g_TestApp.m_RenderTargetOptions.SwapChainDesc.Height;
		Tex2DDesc.Format = g_TestApp.m_RenderTargetOptions.SwapChainDesc.Format;
		Tex2DDesc.SampleDesc = g_TestApp.m_RenderTargetOptions.SwapChainDesc.SampleDesc;
		Tex2DDesc.MipLevels = 1;
		Tex2DDesc.ArraySize = 1;
		Tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
		Tex2DDesc.BindFlags =
			((g_TestApp.m_RenderTargetOptions.SwapChainDesc.BufferUsage & DXGI_USAGE_RENDER_TARGET_OUTPUT) ? D3D11_BIND_RENDER_TARGET : 0) |
			((g_TestApp.m_RenderTargetOptions.SwapChainDesc.BufferUsage & DXGI_USAGE_SHADER_INPUT) ? D3D11_BIND_SHADER_RESOURCE : 0) |
			((g_TestApp.m_RenderTargetOptions.SwapChainDesc.BufferUsage & DXGI_USAGE_UNORDERED_ACCESS) ? D3D11_BIND_UNORDERED_ACCESS : 0);
		Tex2DDesc.MiscFlags = 0;
		Tex2DDesc.CPUAccessFlags = 0;
	
		if ( FAILED( hr = m_pDevice->CreateTexture2D( &Tex2DDesc, nullptr, &pTexture ) ) )
			throw TRException("[CMultisampleTest::Present] CreateTexture2D failed.", hr, RESULT_FAIL);	
	
		if ( FAILED( hr = m_pDevice->CreateRenderTargetView( pTexture, &rvDesc, &pRTView ) ) )
			throw TRException("[CMultisampleTest::Present] CreateRenderTargetView failed.", hr, RESULT_FAIL);	
	
		m_pDeviceContext->OMSetRenderTargets( 1, &pRTView, NULL );
		m_pDeviceContext->Draw( m_uVertexCount, 0 );

		ShowResource(L"Multisample Present", pTexture);
	}

	m_pDeviceContext->PSSetShader( NULL, NULL, 0 );
	m_pDeviceContext->PSSetShaderResources( 0, 1, pSRVNull );
	m_pDeviceContext->PSSetSamplers( 0, 1, pNullState );

	m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, NULL );

	SAFE_RELEASE(pPS);
	SAFE_RELEASE(pRTView);
	SAFE_RELEASE(pSRView);
	SAFE_RELEASE(pSamState);
	SAFE_RELEASE(pSwapChainTexture);
}

tstring __stdcall ToString( PIX_POS pos )
{
	switch( pos )
	{
		case PP_TOP_LEFT:
			return tstring( _T( "TOP_LEFT" ) );
			break;
		case PP_TOP_RIGHT:
			return tstring( _T( "TOP_RIGHT" ) );
			break;
		case PP_BOTTOM_LEFT:
			return tstring( _T( "BOTTOM_LEFT" ) );
			break;
		case PP_BOTTOM_RIGHT:
			return tstring( _T( "BOTTOM_RIGHT" ) );
			break;	
		default:
			return tstring( _T( "" ) );
			break;
	}
}

bool __stdcall FromString( PIX_POS *pPos, const tstring &str )
{
	if( tcistring( str ) == _T( "TOP_LEFT" ) )
	{
		*pPos = PP_TOP_LEFT;
		return true;
	}
	
	if( tcistring( str ) == _T( "TOP_RIGHT" ) )
	{
		*pPos = PP_TOP_RIGHT;
		return true;
	}
	
	if( tcistring( str ) == _T( "BOTTOM_LEFT" ) )
	{
		*pPos = PP_BOTTOM_LEFT;
		return true;
	}
	
	if( tcistring( str ) == _T( "BOTTOM_RIGHT" ) )
	{
		*pPos = PP_BOTTOM_RIGHT;
		return true;
	}
	
	return false;
}


tstring __stdcall ToString( MS_READBACK method )
{
	switch( method )
	{
	case MS_RESOLVE:
		return tstring( _T( "RESOLVE" ) );
		break;
	case MS_LOAD:
		return tstring( _T( "LOAD" ) );
		break;
	case MS_PRESENT:
		return tstring( _T( "PRESENT" ) );
		break;
	default:
		return tstring( _T( "" ) );
		break;
	}
}

bool __stdcall FromString( MS_READBACK *pMethod, const tstring &str )
{
	if( tcistring( str ) == _T( "RESOLVE" ) )
	{
		*pMethod = MS_RESOLVE;
		return true;
	}

	if( tcistring( str ) == _T( "LOAD" ) )
	{
		*pMethod = MS_LOAD;
		return true;
	}

	if( tcistring( str ) == _T( "PRESENT" ) )
	{
		*pMethod = MS_PRESENT;
		return true;
	}

	return false;
}


tstring __stdcall ToString( SHADER_FREQUENCY x )
{
	switch( x )
	{
	case FREQUENCY_PIXEL:
		return tstring( _T( "FREQUENCY_PIXEL" ) );
		break;
	case FREQUENCY_SAMPLE:
		return tstring( _T( "FREQUENCY_SAMPLE" ) );
		break;
	default:
		return tstring( _T( "" ) );
		break;
	}
}

bool __stdcall FromString( SHADER_FREQUENCY *x, const tstring &str )
{
	if( tcistring( str ) == _T( "FREQUENCY_PIXEL" ) )
	{
		*x = FREQUENCY_PIXEL;
		return true;
	}

	if( tcistring( str ) == _T( "FREQUENCY_SAMPLE" ) )
	{
		*x = FREQUENCY_SAMPLE;
		return true;
	}
	return false;
}

const vector<DXGI_FORMAT>& GetFamily( const DXGI_FORMAT & Fmt )
{
	switch ( Fmt )
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		return R32G32B32A32Family;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
		return R32G32B32Family;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		return R16G16B16A16Family;

	case DXGI_FORMAT_R32G32_TYPELESS:
		return R32G32Family;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		return R10G10B10A2Family;

	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		return R8G8B8A8Family;

	case DXGI_FORMAT_R16G16_TYPELESS:
		return R16G16Family;

	case DXGI_FORMAT_R32_TYPELESS:
		return R32Family;

	case DXGI_FORMAT_R8G8_TYPELESS:
		return R8G8Family;

	case DXGI_FORMAT_R16_TYPELESS:
		return R16Family;

	case DXGI_FORMAT_R8_TYPELESS:
		return R8Family;

	default:
		return EmptyFamily;
	}
}
const DXGI_FORMAT GetTypeless( const DXGI_FORMAT fmt )
{
	switch( fmt )
	{
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return DXGI_FORMAT_R32G32B32A32_TYPELESS;

	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return DXGI_FORMAT_R32G32B32_TYPELESS;

	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
		return DXGI_FORMAT_R16G16B16A16_TYPELESS;

	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
		return DXGI_FORMAT_R32G32_TYPELESS;

	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
		return DXGI_FORMAT_R10G10B10A2_TYPELESS;

	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return DXGI_FORMAT_R8G8B8A8_TYPELESS;

	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
		return DXGI_FORMAT_R16G16_TYPELESS;
		
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
		return DXGI_FORMAT_R32_TYPELESS;

	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
		return DXGI_FORMAT_R8G8_TYPELESS;

	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
		return DXGI_FORMAT_R16_TYPELESS;
		
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
		return DXGI_FORMAT_R8_TYPELESS;

	default:
		return fmt;
	}
}
