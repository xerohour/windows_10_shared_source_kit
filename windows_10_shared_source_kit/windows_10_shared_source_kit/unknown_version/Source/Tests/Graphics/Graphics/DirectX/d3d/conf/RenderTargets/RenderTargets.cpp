#include "RenderTargets.h"
#include "Clear.h"
#include "Direct3DFailureCategoryStrings.h"

#include <WGFTestCore.h>

using namespace WindowsTest;
using namespace WindowsTest::Graphics;

CRenderTargetsTestApp g_TestApp;

FLOAT_COLOR CRenderTargetsTest::m_ColorSets [ NUM_COLOR_SETS ] [ NUM_COLORS ] = 
{
	{
		{ 0.f, 0.f, 0.f, 1.f },
		{ 0.f, 1.f, 0.f, 1.f },
		{ 0.f, 0.f, 1.f, 1.f },
		{ 0.5f, 0.5f, 0.f, 1.f }
	},
	{
		{ 0.f, 0.f, 0.f, 1.f },
		{ 0.f, 1.5f, 0.f, 0.5f },
		{ -23.f, 0.f, 1.f, 0.f },
		{ 0.5f, 0.5f, 0.f, 1.f }
	},
	{
		{ 1.f, 0.f, 0.f, 1.f },
		{ 0.f, 1.f, 0.f, 1.f },
		{ 0.f, 0.f, 1.f, 1.f },
		{ 0.5f, 0.5f, 0.f, 1.f }
	}
};

bool WithinOneULP( float fReference, float fOutput )
{
	// WARNING:
	// This function does not correctly handle edge cases involving negative floats, zeros
	// and NaNs. If future test result discrepancies are discovered to have been masked by this function,
	// please start using CompareFloatULP() function in Framework\Core\FloatUtils.cpp which
	// does not suffer from these problems.

	if( fReference == fOutput )
		return true;

	UINT32 uRef = *( (UINT32*) &fReference );
	UINT32 uOut = *( (UINT32*) &fOutput );
	if( (int)max( uRef, uOut ) - (int)min( uRef, uOut ) <= 1 )
		return true;

	return false;
}

bool GetScenarioFlags( D3D_RESOURCE_SCENARIO scenario, UINT *pFlags )
{
	if( pFlags == NULL )
		return false;

	*pFlags = 0;
	switch( scenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		*pFlags |= D3D_RS_FLAGS_BUFFER;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		*pFlags |= D3D_RS_FLAGS_ARRAY;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		*pFlags |= D3D_RS_FLAGS_TEXTURE1D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		*pFlags |= D3D_RS_FLAGS_ARRAY;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		*pFlags = (*pFlags | D3D_RS_FLAGS_MS | D3D_RS_FLAGS_TEXTURE2D);
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		*pFlags |= D3D_RS_FLAGS_ARRAY;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		*pFlags |= D3D_RS_FLAGS_TEXTURE2D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE_ARRAY:
		*pFlags |= D3D_RS_FLAGS_ARRAY;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		*pFlags |= D3D_RS_FLAGS_TEXTURECUBE;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		*pFlags |= D3D_RS_FLAGS_TEXTURE3D;
		break;
	}

	return true;
}

bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_SRV_DIMENSION *pSRVDim )
{
	if( !pSRVDim )
		return false;

	switch( scenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		*pSRVDim = D3D11_SRV_DIMENSION_BUFFER;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE1D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE2D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURECUBE;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE_ARRAY:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		*pSRVDim = D3D11_SRV_DIMENSION_TEXTURE3D;
		break;
	}

	return true;
}

bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_RTV_DIMENSION *pRTVDim )
{
	if( !pRTVDim )
		return false;

	switch( scenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		*pRTVDim = D3D11_RTV_DIMENSION_BUFFER;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE1D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE2D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		*pRTVDim = D3D11_RTV_DIMENSION_TEXTURE3D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE_ARRAY:
		return false;
	}

	return true;
}

bool GetViewDimension( D3D_RESOURCE_SCENARIO scenario, D3D11_DSV_DIMENSION *pDSVDim )
{
	if( !pDSVDim )
		return false;

	switch( scenario )
	{
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE1D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE2D;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		*pDSVDim = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
		break;
	case D3D_RESOURCE_SCENARIO_BUFFER:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		return false;
	}

	return true;
}

tstring __stdcall ToString( const D3D_RESOURCE_SCENARIO &scenario )
{
	tstring res;
	switch( scenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		res = _T( "BUFFER" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		res = _T( "TEXTURE1D" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		res = _T( "TEXTURE1D_ARRAY" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		res = _T( "TEXTURE2D" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		res = _T( "TEXTURE2D_ARRAY" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		res = _T( "TEXTURE2D_MS" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		res = _T( "TEXTURE2D_MS_ARRAY" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		res = _T( "TEXTURECUBE" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE_ARRAY:
		res = _T( "TEXTURECUBE_ARRAY" );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		res = _T( "TEXTURE3D" );
		break;
	}

	return res;
}

bool __stdcall FromString( D3D_RESOURCE_SCENARIO *pScenario, const tstring &str )
{
	if( str == _T( "BUFFER" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_BUFFER;
	else if( str == _T( "TEXTURE1D" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE1D;
	else if( str == _T( "TEXTURE1D_ARRAY" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY;
	else if( str == _T( "TEXTURE2D" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE2D;
	else if( str == _T( "TEXTURE2D_ARRAY" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY;
	else if( str == _T( "TEXTURE2D_MS" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE2D_MS;
	else if( str == _T( "TEXTURE2D_MS_ARRAY" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY;
	else if( str == _T( "TEXTURECUBE" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURECUBE;
	else if( str == _T( "TEXTURECUBE_ARRAY" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURECUBE_ARRAY;
	else if( str == _T( "TEXTURE3D" ) )
		*pScenario = D3D_RESOURCE_SCENARIO_TEXTURE3D;
	else
		return false;

	return true;
}


tstring __stdcall ToString( const IMAGE_PATTERN &pattern )
{
	tstring res;
	switch( pattern )
	{
	case IP_STRIPES:
		res = _T( "Stripes" );
		break;
	case IP_GRADIENT:
		res = _T( "Gradient" );
		break;
	case IP_TILES:
		res = _T( "Tiles" );
		break;
	default:
		res = _T( "<unknown" );
	}

	return res;
}

//---------------------------------------------------------------

bool __stdcall FromString( IMAGE_PATTERN *pPattern, const tstring &str )
{
	if( str == _T( "Stripes" ) )
		*pPattern = IP_STRIPES;
	else if( str == _T( "Gradient" ) )
		*pPattern = IP_GRADIENT;
	else if( str == _T( "Tiles" ) )
		*pPattern = IP_TILES;
	else
		return false;

	return true;
}

void CRenderTargetsTest::InitTestParameters()
{
	CTestCaseParameter<IMAGE_PATTERN> *pPatternParam = AddParameter( _T( "ImagePattern" ), &m_ImagePattern );
	testfactor::RFactor rfStripes = AddParameterValue<IMAGE_PATTERN>( pPatternParam, IP_STRIPES );
	testfactor::RFactor rfGradient = AddParameterValue<IMAGE_PATTERN>( pPatternParam, IP_GRADIENT );
	testfactor::RFactor rfTiles = AddParameterValue<IMAGE_PATTERN>( pPatternParam, IP_TILES );

	CTestCaseParameter<UINT32> *pNumStripesParam = AddParameter( _T( "NumStripes" ), &m_uNumStripes );
	CUserValueSet<UINT32> *pNumStripesValues = new CUserValueSet<UINT32>( );
	pNumStripesValues->AddValue( 0 ); // to use current width/height
	pNumStripesValues->AddValue( 1 );
	pNumStripesValues->AddValue( 5 );
	testfactor::RFactor rfNumStripes = AddParameterValueSet( pNumStripesParam, pNumStripesValues );

	CTestCaseParameter<bool> *pVertStripesParam = AddParameter( _T( "VerticalStripes" ), &m_bVertStripes );
	testfactor::RFactor rfVertStripes = AddParameterValueSet( pVertStripesParam, BoolValueSet( ) );

	CTestCaseParameter<UINT32> *pTileParam = AddParameter( _T( "TileSize" ), &m_uTileSize );
	CUserValueSet<UINT32> *pTileValues = new CUserValueSet<UINT32>( );
	pTileValues->AddValue( 1 );
	pTileValues->AddValue( 3 );
	pTileValues->AddValue( 4 );
	pTileValues->AddValue( 16 );
	testfactor::RFactor rfTileSize = AddParameterValueSet( pTileParam, pTileValues );
	testfactor::RFactor rfTileSizeOne = AddParameterValue( pTileParam, (UINT32) 1 );

	CTestCaseParameter<UINT32> *pWidthParam = AddParameter( _T( "Width" ), &m_uWidth );
	CUserValueSet<UINT32> *pWidthValues = new CUserValueSet<UINT32>( );
	pWidthValues->AddValue( 64 );
	pWidthValues->AddValue( 32 );
	pWidthValues->AddValue( 45 );
	testfactor::RFactor rfWidth = AddParameterValueSet( pWidthParam, pWidthValues );

	CTestCaseParameter<UINT32> *pHeightParam = AddParameter( _T( "Height" ), &m_uHeight );
	CUserValueSet<UINT32> *pHeightValues = new CUserValueSet<UINT32>( );
	pHeightValues->AddValue( 64 );
	pHeightValues->AddValue( 128 );
	pHeightValues->AddValue( 45 );
	testfactor::RFactor rfHeight = AddParameterValueSet( pHeightParam, pHeightValues );

	CTestCaseParameter<UINT32> *pDepthParam = AddParameter( _T( "Depth" ), &m_uDepth );
	CUserValueSet<UINT32> *pDepthValues = new CUserValueSet<UINT32>( );
	pDepthValues->AddValue( 32 );
	pDepthValues->AddValue( 16 );
	pDepthValues->AddValue( 1 );
	testfactor::RFactor rfDepth = AddParameterValueSet( pDepthParam, pDepthValues );

	CTestCaseParameter<D3D_RESOURCE_SCENARIO> *pResParam = AddParameter( _T( "ResourceType" ), &m_ResourceType );
	testfactor::RFactor rfBuffer = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_BUFFER );
	testfactor::RFactor rfTex1D = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE1D );
	testfactor::RFactor rfTex2D = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D );
	testfactor::RFactor rfTex3D = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE3D );
	testfactor::RFactor rfTexCube = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURECUBE );

	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T( "Format" ), &m_Format );
	CFilterHandleCommonSource<DXGI_FORMAT> formatFilter = !(
		FilterRegExp<DXGI_FORMAT>( _T( "UNKNOWN" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "TYPELESS" ) ) );
	testfactor::RFactor rfFormats = AddParameterValueSet<DXGI_FORMAT>( pFormatParam, FormatValueSet( formatFilter ) );

	CUserValueSet<UINT32> *pSliceValues = new CUserValueSet<UINT32>( );
	pSliceValues->AddValue( 0 );
	pSliceValues->AddValue( 1 );
	pSliceValues->AddValue( 2 );
	
	CTestCaseParameter<UINT32> *pMipLevelParam = AddParameter( _T( "MipLevel" ), &m_uMipLevel );
	testfactor::RFactor rfMipLevel = AddParameterValueSet<UINT32>( pMipLevelParam, pSliceValues );
	testfactor::RFactor rfMipDef = AddParameterValue<UINT32>( pMipLevelParam, 0 );

	CTestCaseParameter<D3D11_BIND_FLAG> *pBindParam = AddParameter( _T( "AdditionalBind" ), &m_BindFlag );
	CUserValueSet<D3D11_BIND_FLAG> *pBindValues = new CUserValueSet<D3D11_BIND_FLAG>( );
	pBindValues->AddValue( D3D11_BIND_RENDER_TARGET );
	pBindValues->AddValue( D3D11_BIND_VERTEX_BUFFER );
	pBindValues->AddValue( D3D11_BIND_INDEX_BUFFER );
	pBindValues->AddValue( D3D11_BIND_SHADER_RESOURCE );
	testfactor::RFactor rfBind = AddParameterValueSet<D3D11_BIND_FLAG>( pBindParam, pBindValues );

	CUserValueSet<D3D11_BIND_FLAG> *pBindValuesTex = new CUserValueSet<D3D11_BIND_FLAG>();
	pBindValuesTex->AddValue( D3D11_BIND_RENDER_TARGET );
	pBindValuesTex->AddValue( D3D11_BIND_SHADER_RESOURCE );
	testfactor::RFactor rfBindTex = AddParameterValueSet<D3D11_BIND_FLAG>( pBindParam, pBindValuesTex );

	CTestCaseParameter<UINT32> *pRTIndexParam = AddParameter( _T( "RTIndex" ), &m_uRTIndex );
	CUserValueSet<UINT32> *pRTIndexValues = new CUserValueSet<UINT32>( );
	pRTIndexValues->AddValue( 0 );
	pRTIndexValues->AddValue( 1 );
	pRTIndexValues->AddValue( 5 );
	pRTIndexValues->AddValue( 15 );
	testfactor::RFactor rfRTIndex = AddParameterValueSet<UINT32>( pRTIndexParam, pRTIndexValues );
	testfactor::RFactor rfRTIndexDef = AddParameterValue<UINT32>( pRTIndexParam, 0 );

	CTestCaseParameter<bool> *pCubeArrayParam = AddParameter( _T( "CubeArray" ), &m_bCubeArray );
	testfactor::RFactor rfCubeArray = AddParameterValueSet<bool>( pCubeArrayParam, BoolValueSet() );
	
	SetParameterDefaultValue<D3D11_BIND_FLAG>( pBindParam, D3D11_BIND_RENDER_TARGET );
	SetParameterDefaultValue<bool>( pCubeArrayParam, false );

	SetRootTestFactor( 
		( rfFormats * rfBuffer * rfTiles * rfTileSizeOne * rfWidth * rfBind * rfRTIndexDef ) +
		( rfFormats * rfTex1D * rfMipLevel * rfTiles * rfTileSizeOne * rfWidth * rfBindTex * rfRTIndex ) +
		( rfFormats * rfTex2D * rfMipLevel * rfTiles * rfTileSize * (rfWidth % rfHeight) * rfBindTex * rfRTIndex ) +
		( rfFormats * rfTex2D * rfMipLevel * rfStripes * rfNumStripes * rfVertStripes * (rfWidth % rfHeight) * rfRTIndex) +
		( rfFormats * rfTex2D * rfMipLevel * rfGradient * (rfWidth % rfHeight) * rfRTIndex ) +
		( rfFormats * rfTex3D * rfMipLevel * rfTiles * rfTileSize * (rfWidth % rfHeight % rfDepth) * rfRTIndexDef ) +
		( rfFormats * rfTex3D * rfMipLevel * rfStripes * rfNumStripes * rfVertStripes * (rfWidth % rfHeight % rfDepth) * rfRTIndexDef ) +
		( rfFormats * rfTex3D * rfMipLevel * rfGradient * (rfWidth % rfHeight % rfDepth) * rfRTIndexDef ) +
		( rfFormats * rfTexCube * rfCubeArray * rfTiles * rfTileSize * rfWidth * rfMipDef * rfRTIndex ) +
		( rfFormats * rfTexCube * rfCubeArray * rfStripes * rfNumStripes * rfVertStripes * rfWidth * rfMipDef * rfRTIndex ) +
		( rfFormats * rfTexCube * rfCubeArray * rfGradient * rfWidth * rfMipDef * rfRTIndex ) 
		);	
	
	AddPriorityPatternFilter(FilterEqual<D3D_RESOURCE_SCENARIO>(pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D), WeightFactorVeryLarge);

	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(pFormatParam, DXGI_FORMAT_R8G8B8A8_UNORM), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(pFormatParam, DXGI_FORMAT_B8G8R8A8_UNORM), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<IMAGE_PATTERN>(pPatternParam, IP_GRADIENT), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(pWidthParam, 32), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(pHeightParam, 128), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT32>(pRTIndexParam, 0), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 4.5f );
	DeclarePriorityLevelByPriorityValue( 2, 2.0f );

}

//---------------------------------------------------------------

TEST_RESULT CRenderTargetsTest::Setup( )
{
	HRESULT hr = E_FAIL;
	TEST_RESULT tr = CD3D11Test::Setup();
	if( tr != RESULT_PASS )
		return tr;

	// Check what feature level we're running at
	m_bFL9X = GetDevice()->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_0;

	//
	// Get the adapter descriptor and WDDM version.
	//

	m_pAdapterDescriptor = DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor( GetDevice() );
	if( nullptr == m_pAdapterDescriptor )
	{
		LogError( __FILEW__, __LINE__, L"GetAdapterDescriptor returned null.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCRequiredCapabilityNotSupported );
		return RESULT_FAIL;
	}

	// Get the WDDM version
	if ( GetFramework()->m_D3DOptions.DriverType == D3D_DRIVER_TYPE_REFERENCE )
	{
		// Ref doesn't support the GetDisplayDriverModel() call
		m_wddmDriverVersion = KMT_DRIVERVERSION_WDDM_1_2;
	}
	else
	{
		if( FAILED( hr = m_pAdapterDescriptor->GetDisplayDriverModel( m_wddmDriverVersion ) ) )
		{
			LogError( __FILEW__, __LINE__, L"GetDisplayDriverModel failed.", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::gFCRequiredCapabilityNotSupported );
			return RESULT_FAIL;
		}
	}

	D3D11_SUBRESOURCE_DATA initData = { 0 };
	D3D11_BUFFER_DESC desc = { 0 };
	desc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	//desc.ByteWidth set below based on the feature level
	desc.CPUAccessFlags = 0;
	desc.MiscFlags		= 0;
	desc.Usage			= D3D11_USAGE_DEFAULT;
	if( m_bFL9X )
	{
		// feature level 9 doesn't support the GS, so we'll render a quad
		desc.ByteWidth = sizeof( TLVERTEX ) * 4;

		TLVERTEX vertices[4] = 
		{
			{ {1.f,1.f},		1.f, 0.f, 0.f, 0.f },
			{ {1.f,-1.f},		1.f, 1.f, 0.f, 0.f },
			{ {-1.f,1.f},		0.f, 0.f, 0.f, 0.f },
			{ {-1.f,-1.f},		0.f, 1.f, 0.f, 0.f },
		};

		assert( sizeof(vertices) == sizeof(m_pVertexData) );
		ZeroMemory( m_pVertexData, sizeof(m_pVertexData) );
		memcpy( m_pVertexData, vertices, sizeof( vertices ) );

		initData.SysMemPitch = sizeof( TLVERTEX ) * 4;
		initData.SysMemSlicePitch = sizeof( TLVERTEX ) * 4;
		initData.pSysMem = m_pVertexData;
	}
	else
	{
		// create one vertex, actual quad will be made in GS according to cycled dimensions
		desc.ByteWidth = sizeof( TLVERTEX );
		
		ZeroMemory( m_pVertexData, sizeof(m_pVertexData) );

		initData.SysMemPitch = sizeof( TLVERTEX );
		initData.SysMemSlicePitch = sizeof( TLVERTEX );
		initData.pSysMem = m_pVertexData;
	}

	if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &desc, &initData, &m_pVertexBuffer ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::Setup() - failed to create vertex buffer, hr = 0x%x" ), hr );
		return RESULT_FAIL;
	}

	UINT strides = sizeof( TLVERTEX );
	UINT offsets = 0;
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &strides, &offsets );
	GetEffectiveContext()->IASetPrimitiveTopology( m_bFL9X ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP : D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );

	// create and set rasterizer state
	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = false;
	rastDesc.DepthBias = false;
	rastDesc.DepthBiasClamp = 0;
	rastDesc.SlopeScaledDepthBias = 0;
	rastDesc.DepthClipEnable = m_bFL9X ? true : false;
	rastDesc.ScissorEnable = false;
	rastDesc.MultisampleEnable = false;
	rastDesc.AntialiasedLineEnable = false;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	if( FAILED( hr = GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSEnabled ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateRasterizerState() failed.", hr, ERRORCODE_TYPE_HRESULT, Direct3D::gFCDeviceCreateRasterizerState );
		return RESULT_FAIL;
	}
	if( FAILED( hr = GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSDisabled ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateRasterizerState() failed.", hr, ERRORCODE_TYPE_HRESULT, Direct3D::gFCDeviceCreateRasterizerState );
		return RESULT_FAIL;
	}

	GetEffectiveContext()->RSSetState( m_pRSEnabled );

//	GetDevice()->OMSetDepthStencilState( m_pDepthStencilState, 0 );

	// create and set sampler
	// Note: FL9 cannot use the border address mode or MaxLOD < FLT_MAX
	m_SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	m_SamplerDesc.AddressU = m_bFL9X ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.AddressV = m_bFL9X ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.AddressW = m_bFL9X ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.MipLODBias = 0;
	m_SamplerDesc.MaxAnisotropy = 1;
	m_SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	m_SamplerDesc.BorderColor[0] = 0.5f;
	m_SamplerDesc.BorderColor[1] = 0.5f;
	m_SamplerDesc.BorderColor[2] = 0.5f;
	m_SamplerDesc.BorderColor[3] = 1.0f;
	m_SamplerDesc.MinLOD = 0;
	m_SamplerDesc.MaxLOD = m_bFL9X ? FLT_MAX : 5;

	if( FAILED( hr = GetDevice()->CreateSamplerState(&m_SamplerDesc, &m_pSampler ) ))
	{
		WriteToLog( _T("CRenderTargetsTest::Setup() - CreateSamplerState() failed. hr = 0x%x"), hr );
		return RESULT_FAIL;
	}

	// setup constant buffer
	D3D11_BUFFER_DESC bufdesc;
	bufdesc.ByteWidth = 512;
	bufdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufdesc.MiscFlags = 0;
	if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufdesc, NULL, &m_pConstBuffer ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CreateBufferTiledOptional() failed.", hr, ERRORCODE_TYPE_HRESULT, Direct3D::gFCDeviceCreateResource );
		return RESULT_FAIL;
	}

	GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	if( !m_bFL9X )
	{
		GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	}
	GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

//---------------------------------------------------------------

bool CRenderTargetsTest::SetupShaders(bool bForceFloatInput)
{
	bool bResult = true;
	HRESULT hr;
	D3DXMATRIX MatWorld;
	tstring psTexInputType = "float";
	tstring psOutputType = "float4";
	tstring numComponents = "";

	m_pVS = NULL; 
	m_pGS = NULL;
	m_pPS = NULL;

	if( !m_bFL9X )
	{
		GetEffectiveContext()->VSSetSamplers(0, 1, &m_pSampler);	// Vertex shader textures aren't supported on FL9
		GetEffectiveContext()->GSSetSamplers(0, 1, &m_pSampler);
	}
	GetEffectiveContext()->PSSetSamplers(0, 1, &m_pSampler);

	// create shaders and layout
	ID3D10Blob *pShaderBlob = NULL;
	ID3D10Blob *pErrorBlob = NULL;
	
	// Geometry shaders are not supported on FL9.X
	if( !m_bFL9X )
	{
		if( FAILED( hr = GetFramework()->CompileShaderFromResource( 
			nullptr,
			_T( "RC_RENDERTARGETS_GSH" ),
			_T( "GShaderMain" ), 
			D3D_SHADER_STAGE_GEOMETRY,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_10_0,
			D3D10_SHADER_DEBUG,
			&pShaderBlob,
			&pErrorBlob ) ) || (pShaderBlob == nullptr) )
		{
			WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - D3DXCompileShaderFromResource() failed with geometry shader, hr = 0x%x, errors: \n%s" ), hr, pErrorBlob ? pErrorBlob->GetBufferPointer( ) : _T("") );
			goto Fail;
		}

		
		if( FAILED( hr = GetDevice()->CreateGeometryShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize( ), NULL, &m_pGS ) ) )
		{
			WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - CreateGeometryShader failed, hr = 0x%x" ), hr );
			goto Fail;
		}

		SAFE_RELEASE( pShaderBlob );
		SAFE_RELEASE( pErrorBlob );
	}

	//
	// Determine how to decl the input textures in the PS
	//
	const DXGI_FORMAT rtvFormat = GetRTViewFormat( m_Format );
	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(rtvFormat);
	
	// Find out how many components the format has
	if (pTexFormatInfo->dwFlags & (FF_RGBA | FF_SRGB))
		numComponents = "4";
	else if( pTexFormatInfo->dwFlags & FF_RGB )
		numComponents = "3";
	else if( pTexFormatInfo->dwFlags & FF_RG )
		numComponents = "2";
	else
		numComponents = ""; // "" == 1

	// Determine the texture format type
	if (!bForceFloatInput)
	{
		if (pTexFormatInfo->dwFlags & FF_UINT)
			psTexInputType = "uint";
		else if (pTexFormatInfo->dwFlags & FF_SINT)
			psTexInputType = "int";
		else if (pTexFormatInfo->dwFlags & FF_UNORM)
			psTexInputType = "unorm float";
		else if (pTexFormatInfo->dwFlags & FF_SNORM)
			psTexInputType = "snorm float";
		else
			psTexInputType = "float";
	}

	// Add the number of components to the decl
	psTexInputType = FormatString("%s%s", psTexInputType.c_str(), numComponents.c_str());

	//
	// Determine what the PS output type is
	//
	if( pTexFormatInfo->dwFlags & FF_UINT )
		psOutputType = "uint4";
	else if( pTexFormatInfo->dwFlags & FF_SINT )
		psOutputType = "int4";
	else
		psOutputType = "float4";

	D3D10_SHADER_MACRO macros [] =
	{
		{ "TEX_INPUT_TYPE", psTexInputType.c_str() },
		{ "PS_OUTPUT_TYPE", psOutputType.c_str() },
		NULL
	};

	if( FAILED( hr = GetFramework()->CompileShaderFromResource( 
		nullptr,
		m_bFL9X ? _T( "RC_RENDERTARGETS_FL9X_PSH" ) : _T( "RC_RENDERTARGETS_PSH" ),
		_T( "PShaderMain" ), 
		D3D_SHADER_STAGE_PIXEL,
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D10_SHADER_DEBUG,
		&pShaderBlob,
		&pErrorBlob,
		m_bFL9X ? nullptr : macros ) ) || (pShaderBlob == nullptr) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - D3DXCompileShaderFromResource() failed with pixel shader, hr = 0x%x, errors: \n%s" ), hr, pErrorBlob ? pErrorBlob->GetBufferPointer( ) : _T("") );
		goto Fail;
	}

	if( FAILED( hr = GetDevice()->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize( ), NULL, &m_pPS ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - CreatePixelShader failed, hr = 0x%x" ), hr );
		goto Fail;
	}

	SAFE_RELEASE( pShaderBlob );
	SAFE_RELEASE( pErrorBlob );

	if( FAILED( hr = GetFramework()->CompileShaderFromResource( 
		nullptr,
		m_bFL9X ? _T( "RC_RENDERTARGETS_SR_READBACK_FL9X_PSH" ) : _T( "RC_RENDERTARGETS_SR_READBACK_PSH" ),
		_T( "PShaderMain" ), 
		D3D_SHADER_STAGE_PIXEL,
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D10_SHADER_DEBUG,
		&pShaderBlob,
		&pErrorBlob,
		m_bFL9X ? nullptr : macros ) ) || (pShaderBlob == nullptr) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - D3DXCompileShaderFromResource() failed with pixel shader, hr = 0x%x, errors: \n%s" ), hr, pErrorBlob ? pErrorBlob->GetBufferPointer( ) : _T("") );
		goto Fail;
	}

	if( FAILED( hr = GetDevice()->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize( ), NULL, &m_pPS_SR_Readback ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - CreatePixelShader failed, hr = 0x%x" ), hr );
		goto Fail;
	}

	SAFE_RELEASE( pShaderBlob );
	SAFE_RELEASE( pErrorBlob );

	if( FAILED( hr = GetFramework()->CompileShaderFromResource( 
		nullptr,
		m_bFL9X ? _T( "RC_RENDERTARGETS_FL9X_VSH" ) : _T( "RC_RENDERTARGETS_VSH" ),
		_T( "VShaderMain" ), 
		D3D_SHADER_STAGE_VERTEX,
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D10_SHADER_DEBUG,
		&pShaderBlob,
		&pErrorBlob ) ) || (pShaderBlob == nullptr) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - D3DXCompileShaderFromResource() failed with vertex shader, hr = 0x%x, errors: \n%s" ), hr, pErrorBlob ? pErrorBlob->GetBufferPointer( ) : _T("") );
		goto Fail;
	}

	if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize( ), NULL, &m_pVS ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - CreateVertexShader failed, hr = 0x%x" ), hr );
		goto Fail;
	}

	// setup input layout
	D3D11_INPUT_ELEMENT_DESC inputDesc [ ] =
	{
		{"pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},  //position
		{"texCoord", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},  //texture coordinates
	};

	if( FAILED( hr = GetDevice()->CreateInputLayout( inputDesc, sizeof( inputDesc ) / sizeof( *inputDesc ), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize( ), &m_pLayout ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupShaders() - CreateInputLayout failed, hr = 0x%x" ), hr );
		goto Fail;
	}

	GetEffectiveContext()->IASetInputLayout( m_pLayout );

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );

	goto Pass;
Fail:
	bResult = false;
Pass:
	SAFE_RELEASE( pShaderBlob );
	SAFE_RELEASE( pErrorBlob )

	return bResult;
}

//---------------------------------------------------------------

bool CRenderTargetsTest::IsValidFL9TestCase() const
{
	// Local variables
	bool bWDDM1_3OrHigher = false;
	bool bSwapbufferFormat = false;

	//
	// Common checks.
	//

	// Check the WDDM level
    if( m_wddmDriverVersion >= KMT_DRIVERVERSION_WDDM_1_3 )
    {
        bWDDM1_3OrHigher = true;
    }

	// Check resource dimensions
	// Texture2D dimensions on FL9.x must be a power of 2 if they have multiple miplevels (this test always does)
	if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE2D && (
        ((m_uWidth > 0) && ((m_uWidth & (m_uWidth - 1)) != 0)) ||		// Width is a not a power of 2
        ((m_uHeight > 0) && ((m_uHeight & (m_uHeight - 1)) != 0)) ) ) 	// Height is a not a power of 2
    {
        WriteToLog( _T("D3D9 Texture2D's don't allow non power of 2 dimensions when multiple mip levels are specified.") );
        return false;
    }

	// TextureCubes dimensions on FL9.x must be a power of 2
	if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE && (
        ((m_uWidth > 0) && ((m_uWidth & (m_uWidth - 1)) != 0)) ||		// Width is a not a power of 2
        ((m_uHeight > 0) && ((m_uHeight & (m_uHeight - 1)) != 0)) ) ) 	// Height is a not a power of 2
    {
        WriteToLog( _T("D3D9 TextureCube's don't allow non power of 2 dimensions.") );
        return false;
    }

	//
	// Swapbuffer test cases
	// Driver must be WDDM1.3 or higher
	// Format must be A8_UNORM, R8_UNORM, or R8G8_UNORM
	//

	// Check the current format
	switch( m_Format )
    {
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8G8_UNORM:
        bSwapbufferFormat = true;
        break;
    }

	if( bWDDM1_3OrHigher && bSwapbufferFormat )
	{
		return true;
	}

	return false;
}

//---------------------------------------------------------------

TEST_RESULT CRenderTargetsTest::SetupTestCase()
{
	HRESULT hr = S_OK;

	GetResourceViewer().HideAll();	// We only want to show windows that are valid for the current test case

	// We don't run all test cases on FL9 be default; only specific test cases for new features
	if( m_bFL9X && !IsValidFL9TestCase() )
	{
		WriteToLog( _T("The test is running at feature level 9.x. Not all test cases are necessarily supported.") );
		return RESULT_SKIP;
	}

	// BUGBUG: Add support for these formats

	if( m_Format == DXGI_FORMAT_A8_UNORM )
		return RESULT_SKIP;

	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );

	if( m_bFL9X )
	{
		if( m_uRTIndex >= 1 )
		{
			WriteToLog( _T("Feature level 9 doesn't support texture arrays.") );
			SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "RTIndex" ), m_uRTIndex ) );
			return RESULT_SKIP;
		}

		if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE3D )
		{
			WriteToLog( _T("Feature level 9 doesn't support Texture3D as a render target.") );
			SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "ResourceType" ), m_ResourceType ) );
			return RESULT_SKIP;
		}
	}

	if( m_bCubeArray && GetFramework()->m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_0 )
	{
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "CubeArray" ), m_bCubeArray ) );
		return RESULT_SKIP;
	}

	if( !m_bCubeArray && m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE && m_uRTIndex >= 6 )
	{
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "RTIndex" ), m_uRTIndex ) );
		return RESULT_SKIP;
	}

	if( CD3D10FormatHelper_Test::Planar(m_Format) )
	{
		// Planar formats change the default array max size to 2...
		if( m_uRTIndex > 1 )
		{
			SkipMatching( GetSkipMatchingFilter() || (FilterEqual( _T( "Format" ), m_Format ) && FilterEqual( _T( "RTIndex" ), m_uRTIndex ) ) );
			return RESULT_SKIP;
		}

		// TODO: Currently we don't support "ReformatPixels" pixels for any planar format besides NV12
		// Other planar formats should be skipped for now.
		if( m_Format != DXGI_FORMAT_NV12 )
		{
			SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "Format" ), m_Format ) );
			return RESULT_SKIP;
		}
	}

	if( m_BindFlag == D3D11_BIND_SHADER_RESOURCE && m_ResourceType != D3D_RESOURCE_SCENARIO_TEXTURE2D )
		return RESULT_SKIP;

	UINT supportFlags = 0;
	if( FAILED( hr = GetDevice()->CheckFormatSupport( m_Format, &supportFlags ) ) )
	{
		WriteToLog( _T("CheckFormatSupport() failed with %s. Skipping format."), D3DHResultToString(hr).c_str() );
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "Format" ), m_Format ) );
		return RESULT_SKIP;
	}
	if( (supportFlags & D3D11_FORMAT_SUPPORT_RENDER_TARGET) == 0)
	{
		WriteToLog( _T("Format doesn't support being a render target. Skipping format.") );
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "Format" ), m_Format ) );
		return RESULT_SKIP;
	}

	std::map< D3D_RESOURCE_SCENARIO, D3D11_FORMAT_SUPPORT > resTypes;
	resTypes[ D3D_RESOURCE_SCENARIO_BUFFER ] = D3D11_FORMAT_SUPPORT_BUFFER;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE1D ] = D3D11_FORMAT_SUPPORT_TEXTURE1D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE2D ] = D3D11_FORMAT_SUPPORT_TEXTURE2D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE3D ] = D3D11_FORMAT_SUPPORT_TEXTURE3D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURECUBE ] = D3D11_FORMAT_SUPPORT_TEXTURECUBE;

	if( (supportFlags & resTypes[ m_ResourceType ]) == 0 )
	{
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "ResourceType" ), m_ResourceType ) );
		return RESULT_SKIP;
	}

	if( (supportFlags & resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE2D ]) == 0 ||
		(supportFlags & D3D11_FORMAT_SUPPORT_SHADER_LOAD) == 0)
	{
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "Format" ), m_Format ) );
		return RESULT_SKIP;
	}

	if( ( m_ResourceType == D3D_RESOURCE_SCENARIO_BUFFER && m_BindFlag != D3D11_BIND_RENDER_TARGET ) &&
		( m_Format == DXGI_FORMAT_R32G32B32_FLOAT || m_Format == DXGI_FORMAT_R32G32B32_SINT || m_Format == DXGI_FORMAT_R32G32B32_UINT ) )
	{
		return RESULT_SKIP;
	}

	const bool bFormatSupportsMips = (supportFlags & D3D11_FORMAT_SUPPORT_MIP) != 0;	// Some video formats don't support mip levels
	if( !bFormatSupportsMips &&	// The format doesn't support multiple mip levels
		m_uMipLevel != 0 )
	{
		WriteToLog( _T("%s doesn't support multiple mip levels but m_uMipLevel = %u. Skipping test case."), ToString(m_Format).c_str(), m_uMipLevel );
		return RESULT_SKIP;
	}

	if( CD3D10FormatHelper_Test::YUV( m_Format ) )
	{
		// Video formats need to check that the resource is aligned
		const UINT horizontalAlignment = CD3D10FormatHelper_Test::GetWidthAlignment( m_Format );
		const UINT verticalAlignment = CD3D10FormatHelper_Test::GetHeightAlignment( m_Format );

		// Check the box alignment and resource alignment
		switch( m_ResourceType )
		{
		default:
			WriteToLog( _T("Invalid m_ResourceScenario = %s (%u)."), ToString(m_ResourceType).c_str(), m_ResourceType );
			return RESULT_FAIL;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE3D:
			// No box depth alignment checks
			// fall-through
		case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
			if( (m_uHeight % verticalAlignment) != 0 )
			{
				WriteToLog( _T("The resource's height (%u) doesn't match the format's (%s) vertical alignment (%u)"), m_uHeight, ToString(m_Format).c_str(), verticalAlignment );
				return RESULT_SKIP;
			}
			// fall-through
		case D3D_RESOURCE_SCENARIO_BUFFER:
		case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
			if( (m_uWidth % horizontalAlignment) != 0 )
			{
				WriteToLog( _T("The resource's width (%u) doesn't match the format's (%s) horizontal alignment (%u)"), m_uWidth, ToString(m_Format).c_str(), horizontalAlignment );
				return RESULT_SKIP;
			}
			break;
		}
	}

	// Set shader resource(s)
	if( !CreateShaderResources( ) )
		return RESULT_FAIL;

	// Set render target(s)
	if( !CreateRenderTargets( ) )
		return RESULT_FAIL;

	// create and set viewport
	UINT height, width, depth;
	depth = 1;
	width = m_uWidth;
	if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE1D || m_ResourceType == D3D_RESOURCE_SCENARIO_BUFFER ) 
		height = 1;
	else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE )
		height = m_uWidth;
	else
		height = m_uHeight;
	
	if( m_ResourceType != D3D_RESOURCE_SCENARIO_BUFFER )
		GetMipDimensions( m_uMipLevel, width, height, depth );
	
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float) width;
	vp.Height   = (float) height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	GetEffectiveContext()->RSSetViewports(1, &vp);

	if( !SetupShaders( ) )
		return RESULT_FAIL;

	// pass current width and height to shader
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if( FAILED( hr = GetImmediateContext()->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupTestCase() - failed to map const buffer, hr = 0x%x" ), hr );
		return RESULT_FAIL;
	}
	
	D3DXMatrixIdentity( &m_mxTrans );
	float transx = (float)width / 2.f;
	float transy = (float)height / 2.f;
	
	// Inverse viewport scale.
	m_mxTrans._13 = 1.f / transx;
	m_mxTrans._14 = -1.f / transy;

	// Prescaled inverse viewport translation.
	m_mxTrans._11 = -transx * m_mxTrans._13;
	m_mxTrans._12 = -transy * m_mxTrans._14;

	memcpy( mappedRes.pData, &m_mxTrans, sizeof( D3DXMATRIX ) );
	// write to c4, past transformation matrix
	float dims[ 2 ] = { (float)width, (float)height };
	int iRes = 0;
	if( m_ResourceType == D3D_RESOURCE_SCENARIO_BUFFER )
		iRes = 0;
	else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE1D )
		iRes = 1;
	else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE2D )
		iRes = 2;
	else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE3D )
		iRes = 3;
	else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE )
		iRes = 4;

	memcpy( ((BYTE*)mappedRes.pData) + sizeof(D3DXMATRIX), dims, sizeof(float) * 2 );
	memcpy( ((BYTE*)mappedRes.pData) + sizeof(D3DXMATRIX) + sizeof(float) * 2, &m_uRTIndex, sizeof(UINT32) );
	memcpy( ((BYTE*)mappedRes.pData) + sizeof(D3DXMATRIX) + sizeof(float) * 2 + sizeof(UINT32), &iRes, sizeof(int) );

	GetImmediateContext()->Unmap(m_pConstBuffer, 0);

	m_bLastFaces = !m_bLastFaces;

	return RESULT_PASS;
}

//---------------------------------------------------------------

void CRenderTargetsTest::Cleanup( )
{
	CD3D11Test::Cleanup();
	
	SAFE_RELEASE( m_pConstBuffer );
	SAFE_RELEASE( m_pRSDisabled );
	SAFE_RELEASE( m_pRSEnabled );
	SAFE_RELEASE( m_pSampler );
	SAFE_RELEASE( m_pVertexBuffer );
	
	if( m_pAdapterDescriptor )
	{
		delete m_pAdapterDescriptor;
		m_pAdapterDescriptor = nullptr;
	}
}

//---------------------------------------------------------------

void CRenderTargetsTest::CleanupTestCase()
{
	ID3D11ShaderResourceView *pNullSRV[ NUM_RESOURCES ] = { NULL, NULL, NULL };
	ID3D11RenderTargetView *pNullRTV[ NUM_RESOURCES ] = { NULL, NULL, NULL };
	GetEffectiveContext()->PSSetShaderResources( 0, NUM_RESOURCES, pNullSRV );
	GetEffectiveContext()->OMSetRenderTargets( 3, pNullRTV, NULL );
	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
	}

	SAFE_RELEASE( m_pRTView );
	SAFE_RELEASE( m_pRTCurrent );
	SAFE_RELEASE( m_pSRView );
	SAFE_RELEASE( m_pShaderResource );

	SAFE_RELEASE( m_pLayout );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pPS_SR_Readback );
}

//---------------------------------------------------------------

TEST_RESULT CRenderTargetsTest::ExecuteTestCase()
{
	HRESULT hr;
	ID3D11Resource *pRT = NULL;
	ID3D11Resource *pRTforSR = NULL;
	const UINT numArraySlices = m_bFL9X ? 1 : NUM_ARRAY_SLICES;
	float color[ 4 ] = { 1.f, 1.f, 1.f, 1.f };
	GetEffectiveContext( )->ClearRenderTargetView( m_pRTView, color );
	GetEffectiveContext( )->Draw( m_bFL9X ? 4 : 1, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		goto fail;
	}

	UINT mipLevel = m_uMipLevel;
	if( m_ResourceType == D3D_RESOURCE_SCENARIO_BUFFER )
		mipLevel = 0;

	UINT numSlices = numArraySlices;
	if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE3D )
		numSlices = 0;
	bool bCube = m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE;

	// if we're testing rendering to shader resource and read-back...
	if( m_BindFlag == D3D11_BIND_SHADER_RESOURCE )
	{
		GetEffectiveContext()->PSSetShader( m_pPS_SR_Readback, NULL, 0 );

		ID3D11ShaderResourceView *pNullSRVs[ NUM_RESOURCES ] = { NULL, NULL, NULL };
		GetEffectiveContext()->PSSetShaderResources( 0, NUM_RESOURCES, pNullSRVs );
		ID3D11RenderTargetView *pNullRTVs[ NUM_RESOURCES ] = { NULL, NULL, NULL };
		GetEffectiveContext()->OMSetRenderTargets( NUM_RESOURCES, pNullRTVs, NULL );
		if( FAILED( ExecuteEffectiveContext() ) )
		{
			WriteToLog("ExecuteEffectiveContext() failed");
			goto fail;
		}

		if( !CopyResource( m_pRTCurrent, &pRTforSR, m_ResourceType, false ) )
		{
			goto fail;
		}

		SAFE_RELEASE( m_pSRView );

		// create a new set of SRVs from the rendered result
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = CRenderTargetsTest::GetSRViewFormat( m_Format );
		if( CD3D10FormatHelper_Test::YUV( m_Format ) )
		{
			WriteToLog( _T("SRV format set to %s."), ToString(srvDesc.Format).c_str() );
		}

		switch( m_ResourceType )
		{
		case D3D_RESOURCE_SCENARIO_BUFFER:
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.ElementOffset = 0;
			srvDesc.Buffer.ElementWidth = m_uWidth;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE1D:
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
			srvDesc.Texture1DArray.ArraySize = 1;
			srvDesc.Texture1DArray.FirstArraySlice = m_uRTIndex;
			srvDesc.Texture1DArray.MipLevels = m_bFL9X ? -1 : 1; // FL9.x must include the least detailed mip
			srvDesc.Texture1DArray.MostDetailedMip = m_uMipLevel;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE2D:
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = m_bFL9X ? -1 : 1; // FL9.x must include the least detailed mip
			srvDesc.Texture2D.MostDetailedMip = m_uMipLevel;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.ArraySize = 1;
			srvDesc.Texture2DArray.FirstArraySlice = m_uRTIndex;
			srvDesc.Texture2DArray.MipLevels = m_bFL9X ? -1 : 1; // FL9.x must include the least detailed mip
			srvDesc.Texture2DArray.MostDetailedMip = m_uMipLevel;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE3D:
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
			srvDesc.Texture3D.MipLevels = m_bFL9X ? -1 : 1; // FL9.x must include the least detailed mip
			srvDesc.Texture3D.MostDetailedMip = m_uMipLevel;
			break;
		}

		if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pRTCurrent, &srvDesc, &m_pSRView ) ) )
		{
			WriteToLog( _T( "CRenderTargetsTest::ExecuteTestCase() - Failed to create SRV for render to SR test case, hr = %s" ), D3DHResultToString( hr ).c_str() );
			goto fail;
		}
		
		int iRes = 0;
		if( m_bFL9X )
		{
			if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE2D )
			iRes = 0;
			else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE3D )
				iRes = 1;
			else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE )
			{
				assert(!"TexCube not supported here");
				iRes = 2;
			}
		}
		else
		{
			if( m_ResourceType == D3D_RESOURCE_SCENARIO_BUFFER )
				iRes = 0;
			else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE1D )
				iRes = 1;
			else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE2D )
				iRes = 2;
			else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE3D )
				iRes = 3;
			else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE )
				iRes = 4;
		}
		GetEffectiveContext()->PSSetShaderResources( iRes, 1, &m_pSRView );

		// create another RTV
		SAFE_RELEASE( m_pRTView );
		D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
		rtDesc.Format = CRenderTargetsTest::GetRTViewFormat( m_Format );
		if( CD3D10FormatHelper_Test::YUV( m_Format ) )
		{
			WriteToLog( _T("RTV format set to %s."), ToString(rtDesc.Format).c_str() );
		}

		UINT depth = 1;
		if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE3D )
		{
			depth = m_uDepth;
			UINT height = m_uHeight;
			UINT width = m_uWidth;
			GetMipDimensions( m_uMipLevel, width, height, depth );
		}

		switch( m_ResourceType )
		{
		case D3D_RESOURCE_SCENARIO_BUFFER:
			rtDesc.ViewDimension = D3D11_RTV_DIMENSION_BUFFER;
			rtDesc.Buffer.ElementOffset = 0;
			rtDesc.Buffer.ElementWidth = m_uWidth;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE1D:
			rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
			rtDesc.Texture1DArray.ArraySize = NUM_ARRAY_SLICES;
			rtDesc.Texture1DArray.FirstArraySlice = 0;
			rtDesc.Texture1DArray.MipSlice = m_uMipLevel;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE2D:
			rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtDesc.Texture2D.MipSlice = m_uMipLevel;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
			// Planar formats are limited to array size 2
			rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtDesc.Texture2DArray.ArraySize = CD3D10FormatHelper_Test::Planar( m_Format ) ? 2 : (bCube ? 6 : numArraySlices);
			rtDesc.Texture2DArray.FirstArraySlice = 0;
			rtDesc.Texture2DArray.MipSlice = m_uMipLevel;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE3D:
			rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
			rtDesc.Texture3D.FirstWSlice = depth - 1;
			rtDesc.Texture3D.MipSlice = m_uMipLevel;
			rtDesc.Texture3D.WSize = 1;
			break;
		}

		if( FAILED( hr = GetDevice()->CreateRenderTargetView( pRTforSR, &rtDesc, &m_pRTView ) ) )
		{
			WriteToLog( _T( "CRenderTargetsTest::ExecuteTestCase() - Failed to create render target view. hr = 0x%x" ), hr );
			goto fail;
		}

		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, NULL );

		float clearZero[ 4 ] = { 0.f, 0.f, 0.f, 0.f };
		GetEffectiveContext()->ClearRenderTargetView( m_pRTView, clearZero );
		GetEffectiveContext()->Draw( m_bFL9X ? 4 : 1, 0 );

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			WriteToLog("ExecuteEffectiveContext() failed");
			goto fail;
		}

		// Draw the RT for SR
		CResourceViewerWindow *pWindow = ShowResource(L"Render Target for Shader Resource",  pRTforSR, m_uMipLevel, m_uRTIndex, 0, 6.0f * (1 << m_uMipLevel) );

		if( !CopyResource( pRTforSR, &pRT, m_ResourceType, true ) )
		{
			goto fail;
		}
	}
	else
	{
		// Draw the RT
		CResourceViewerWindow *pWindow = ShowResource(L"Render Target",  m_pRTCurrent, m_uMipLevel, m_uRTIndex, 0, 6.0f * (1 << m_uMipLevel) );

		if( !CopyResource( m_pRTCurrent, &pRT, m_ResourceType, true ) )
		{
			goto fail;
		}
	}

	ID3D11Resource *pRes = NULL;
	m_pRTView->GetResource( &pRes );	// Adds Ref
	UINT numMipLevels = 1;
	switch( m_ResourceType )
		{
		case D3D_RESOURCE_SCENARIO_TEXTURE1D:
			D3D11_TEXTURE1D_DESC desc1D;
			(reinterpret_cast<ID3D11Texture1D*>(pRes))->GetDesc( &desc1D );
			numMipLevels = desc1D.MipLevels;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE2D:	
		case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
			D3D11_TEXTURE2D_DESC desc2D;
			(reinterpret_cast<ID3D11Texture2D*>(pRes))->GetDesc( &desc2D );
			numMipLevels = desc2D.MipLevels;
			break;
		case D3D_RESOURCE_SCENARIO_TEXTURE3D:
			D3D11_TEXTURE3D_DESC desc3D;
			(reinterpret_cast<ID3D11Texture3D*>(pRes))->GetDesc( &desc3D );
			numMipLevels = desc3D.MipLevels;
			break;
		}
	SAFE_RELEASE( pRes );

	void *pData;
	UINT uDepthPitch, uRowPitch;
	if( !LockResource( pRT, m_uRTIndex * numMipLevels + mipLevel, m_ResourceType, &pData, uRowPitch, uDepthPitch ) ||
		!CompareResults( pData, m_uRTIndex, uRowPitch, uDepthPitch ) || 
		!UnLockResource( pRT, m_uRTIndex * numMipLevels + mipLevel, m_ResourceType ) )
	{
		goto fail;
	}

	SAFE_RELEASE( pRT );
	SAFE_RELEASE( pRTforSR );
	return RESULT_PASS;

fail:
	SAFE_RELEASE( pRT );
	SAFE_RELEASE( pRTforSR );
	return RESULT_FAIL;
}

//---------------------------------------------------------------

bool CRenderTargetsTest::CreateShaderResources( )
{
	HRESULT hr;
	
	UINT height, width, depth;
	if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE1D || m_ResourceType == D3D_RESOURCE_SCENARIO_BUFFER ) 
		height = 1;
	else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE )
		height = m_uWidth;
	else
		height = m_uHeight;
	width = m_uWidth;
	depth = 1;

	if( m_ResourceType != D3D_RESOURCE_SCENARIO_BUFFER )
		GetMipDimensions( m_uMipLevel, width, height, depth );

	D3D11_TEXTURE2D_DESC desc;
	desc.ArraySize			= 1;
	desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags		= 0;
	desc.Format				= m_Format;
	desc.Height				= height;
	desc.Width				= width;
	desc.MipLevels			= 1;
	desc.MiscFlags			= 0;
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage				= D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA initData;
	const DXGI_FORMAT srvFormat = GetSRViewFormat( m_Format );
	initData.SysMemPitch = width * ( GetBitsPerElement( m_Format ) / 8 );
	initData.SysMemSlicePitch = initData.SysMemPitch * height;

	memset( m_pResourceData[ 0 ], 0, MAX_RESOURCE_SIZE );
	memset( m_pResourceDataCopy[ 0 ], 0, MAX_RESOURCE_SIZE );
	initData.pSysMem = m_pResourceData[ 0 ];

	switch( m_ImagePattern )
	{
	case IP_GRADIENT:
		if( !CFormatHelper::CreateGradient( m_Format, height, width, m_ColorSets[ 0 ], (void*) m_pResourceData[ 0 ] ) )
		{
			WriteToLog( _T( "CRenderTargetsTest::CreateShaderResources() - failed to create gradient texture" ) );
			return false;
		}
		break;
	case IP_STRIPES:
		if( !CFormatHelper::CreateStripes( m_Format, height, width, m_uNumStripes == 0 ? width : m_uNumStripes, m_ColorSets[ 1 ], NUM_COLORS, (void*) m_pResourceData[ 0 ], m_bVertStripes ) )
		{
			WriteToLog( _T( "CRenderTargetsTest::CreateShaderResources() - failed to create stripes texture" ) );
			return false;
		}
		break;
	case IP_TILES:
		if( !CFormatHelper::CreateTiles( m_Format, height, width, width < m_uTileSize ? width : m_uTileSize, m_ColorSets[ 2 ], NUM_COLORS, (void*) m_pResourceData[ 0 ] ) )
		{
			WriteToLog( _T( "CRenderTargetsTest::CreateShaderResources() - failed to create tiles texture" ) );
			return false;
		}
		break;
	}

	if( FAILED( hr = GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional( &desc, &initData, &m_pShaderResource ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::CreateShaderResources() - Failed to create texture, hr = 0x%x" ), hr );
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format						= srvFormat;
	viewDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels		= 1;
	viewDesc.Texture2D.MostDetailedMip	= 0;
	if( CD3D10FormatHelper_Test::YUV( m_Format ) )
	{
		WriteToLog( _T("SRV format set to %s."), ToString(viewDesc.Format).c_str() );
	}

	if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pShaderResource, &viewDesc, &m_pSRView ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::CreateShaderResources() - Failed to create view for texture, hr = 0x%x" ), hr );
		return false;
	}

	GetEffectiveContext()->PSSetShaderResources( 0, 1, &m_pSRView );

	ShowResource(L"Shader Resource", m_pShaderResource, 0, 0, 0, 6.0f * (1 << m_uMipLevel) );

	return true;
}

//---------------------------------------------------------------

bool CRenderTargetsTest::CreateRenderTargets( )
{
	HRESULT hr;
	D3D11_BUFFER_DESC descBuffer;
	D3D11_TEXTURE1D_DESC descTex1D;
	D3D11_TEXTURE2D_DESC descTex2D;
	D3D11_TEXTURE3D_DESC descTex3D;
	const UINT numArraySlices = m_bFL9X ? 1 : NUM_ARRAY_SLICES;

	UINT depth = 1;
	if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE3D )
	{
		depth = m_uDepth;
		UINT height = m_uHeight;
		UINT width = m_uWidth;
		GetMipDimensions( m_uMipLevel, width, height, depth );
	}

	UINT formatSupport = 0;
	if( FAILED( hr = GetDevice()->CheckFormatSupport( m_Format, &formatSupport ) ) )
	{
		WriteToLog( _T("CheckFormatSupport() failed with %s. The test should skip before it gets to this point if the format is not supported."), D3DHResultToString(hr).c_str() );
		return false;
	}

	bool bCube = m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE;
	bool bFormatSupportsMips = (formatSupport & D3D11_FORMAT_SUPPORT_MIP) != 0;	// Some video formats don't support mip levels
	bool bPlanar = CD3D10FormatHelper_Test::Planar( m_Format ) ? true : false;
	UINT planarArraySize = 2;
	UINT mipLevels = bFormatSupportsMips ? NUM_RESOURCES : 1;

	switch( m_ResourceType )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		descBuffer.BindFlags = D3D11_BIND_RENDER_TARGET | m_BindFlag;
		descBuffer.ByteWidth = ( GetBitsPerElement( m_Format ) / 8 ) * m_uWidth;
		descBuffer.CPUAccessFlags = 0;
		descBuffer.MiscFlags = 0;
		descBuffer.Usage = D3D11_USAGE_DEFAULT;
		hr = GetFramework()->CreateBufferTiledOptional( &descBuffer, NULL, (ID3D11Buffer**)&m_pRTCurrent, m_Format );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		descTex1D.ArraySize = bPlanar ? planarArraySize : NUM_ARRAY_SLICES;
		descTex1D.BindFlags = D3D11_BIND_RENDER_TARGET | m_BindFlag;
		descTex1D.CPUAccessFlags = 0;
		descTex1D.Format = m_Format;
		descTex1D.MipLevels = mipLevels;
		descTex1D.MiscFlags = 0;
		descTex1D.Usage = D3D11_USAGE_DEFAULT;
		descTex1D.Width = m_uWidth;
		hr = GetDevice( )->CreateTexture1D( &descTex1D, 0, (ID3D11Texture1D**)&m_pRTCurrent );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:	
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		descTex2D.ArraySize = bPlanar ? planarArraySize : (bCube && !m_bCubeArray ? 6 : numArraySlices);
		descTex2D.BindFlags = D3D11_BIND_RENDER_TARGET | m_BindFlag;
		descTex2D.CPUAccessFlags = 0;
		descTex2D.Format = m_Format;
		descTex2D.Height = bCube ? m_uWidth : m_uHeight;
		descTex2D.MipLevels = mipLevels;
		descTex2D.MiscFlags = bCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;
		descTex2D.Usage = D3D11_USAGE_DEFAULT;
		descTex2D.Width = m_uWidth;
		descTex2D.SampleDesc.Count = 1;
		descTex2D.SampleDesc.Quality = 0;
		hr = GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional( &descTex2D, 0, (ID3D11Texture2D**)&m_pRTCurrent );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		descTex3D.BindFlags = D3D11_BIND_RENDER_TARGET | m_BindFlag;
		descTex3D.CPUAccessFlags = 0;
		descTex3D.Depth = m_uDepth;
		descTex3D.Format = m_Format;
		descTex3D.Height = m_uHeight;
		descTex3D.MipLevels = mipLevels;
		descTex3D.MiscFlags = 0;
		descTex3D.Usage = D3D11_USAGE_DEFAULT;
		descTex3D.Width = m_uWidth;
		hr = GetFramework()->CreateTexture3DMapDefaultOptional( &descTex3D, 0, (ID3D11Texture3D**)&m_pRTCurrent );
		break;
	}

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::CreateRenderTargets() - Failed to create render target resource. hr = 0x%x" ), hr );
		return false;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = CRenderTargetsTest::GetRTViewFormat( m_Format );
	if( CD3D10FormatHelper_Test::YUV( m_Format ) )
	{
		WriteToLog( _T("RTV format set to %s."), ToString(rtDesc.Format).c_str() );
	}
	
	switch( m_ResourceType )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		rtDesc.ViewDimension = D3D11_RTV_DIMENSION_BUFFER;
		rtDesc.Buffer.ElementOffset = 0;
		rtDesc.Buffer.ElementWidth = m_uWidth;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
		rtDesc.Texture1DArray.ArraySize = bPlanar ? planarArraySize : NUM_ARRAY_SLICES;
		rtDesc.Texture1DArray.FirstArraySlice = 0;
		rtDesc.Texture1DArray.MipSlice = m_uMipLevel;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		rtDesc.Texture2DArray.ArraySize = bPlanar ? planarArraySize : (bCube && !m_bCubeArray ? 6 : numArraySlices);
		rtDesc.Texture2DArray.FirstArraySlice = 0;
		rtDesc.Texture2DArray.MipSlice = m_uMipLevel;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
		rtDesc.Texture3D.FirstWSlice = depth - 1;
		rtDesc.Texture3D.MipSlice = m_uMipLevel;
		rtDesc.Texture3D.WSize = 1;
		break;
	}

	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRTCurrent, &rtDesc, &m_pRTView ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::CreateRenderTargets() - Failed to create render target view. hr = 0x%x" ), hr );
		SAFE_RELEASE( m_pRTCurrent );
		return false;
	}
	
	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, NULL );

	return true;
}

//---------------------------------------------------------------

bool CRenderTargetsTest::CopyResource( ID3D11Resource *pResSource, ID3D11Resource **pResDest, D3D_RESOURCE_SCENARIO resType, bool bRead )
{
	HRESULT hr;
	D3D11_BUFFER_DESC descBuffer;
	D3D11_TEXTURE1D_DESC descTex1D;
	D3D11_TEXTURE2D_DESC descTex2D;
	D3D11_TEXTURE3D_DESC descTex3D;
	const UINT numArraySlices = m_bFL9X ? 1 : NUM_ARRAY_SLICES;
	
	UINT formatSupport = 0;
	if( FAILED( hr = GetDevice()->CheckFormatSupport( m_Format, &formatSupport ) ) )
	{
		WriteToLog( _T("CheckFormatSupport() failed with %s. The test should skip before it gets to this point if the format is not supported."), D3DHResultToString(hr).c_str() );
		return false;
	}

	bool bCube = m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE;
	bool bFormatSupportsMips = (formatSupport & D3D11_FORMAT_SUPPORT_MIP) != 0;	// Some video formats don't support mip levels
	bool bPlanar = CD3D10FormatHelper_Test::Planar( m_Format ) ? true : false;
	UINT planarArraySize = 2;

	UINT uCPUAccess, uBind;
	D3D11_USAGE usage;
	if( bRead )
	{
		uCPUAccess = D3D11_CPU_ACCESS_READ;
		uBind = 0;
		usage = D3D11_USAGE_STAGING;
	}
	else
	{
		uCPUAccess = 0;
		uBind = D3D11_BIND_RENDER_TARGET;
		usage = D3D11_USAGE_DEFAULT;
	}

	switch( resType )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		descBuffer.ByteWidth		= ( GetBitsPerElement( m_Format ) / 8 ) * m_uWidth;
		descBuffer.MiscFlags		= 0;
		descBuffer.CPUAccessFlags	= uCPUAccess;
		descBuffer.BindFlags		= uBind;
		descBuffer.Usage			= usage;
		
		hr = GetFramework()->CreateBufferTiledOptional( &descBuffer, NULL, (ID3D11Buffer**)pResDest );
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		descTex1D.ArraySize				= m_bSwapChain ? 1 : NUM_ARRAY_SLICES;
		descTex1D.Format				= m_Format;
		descTex1D.MipLevels				= m_bSwapChain ? 1 : NUM_RESOURCES;
		descTex1D.MiscFlags				= 0;
		descTex1D.Width					= m_uWidth;
		descTex1D.Usage					= usage;
		descTex1D.BindFlags				= uBind;
		descTex1D.CPUAccessFlags		= uCPUAccess;
        
		hr = GetDevice( )->CreateTexture1D( &descTex1D, 0, (ID3D11Texture1D**)pResDest );

		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:	
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		descTex2D.ArraySize				= bPlanar ? planarArraySize : ( m_bSwapChain ? 1 : (bCube && !m_bCubeArray ? 6 : numArraySlices) );
		descTex2D.Format				= m_Format;
		descTex2D.Height				= bCube ? m_uWidth : m_uHeight;
		descTex2D.MipLevels				= (m_bSwapChain || !bFormatSupportsMips) ? 1 : NUM_RESOURCES;
		descTex2D.MiscFlags				= bCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;
		descTex2D.Width					= m_uWidth;
		descTex2D.SampleDesc.Count		= 1;
		descTex2D.SampleDesc.Quality	= 0;
		descTex2D.BindFlags				= uBind;
		descTex2D.CPUAccessFlags		= uCPUAccess;
		descTex2D.Usage					= usage;

	    hr = GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional( &descTex2D, 0, (ID3D11Texture2D**)pResDest );

		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		descTex3D.Depth					= m_uDepth;
		descTex3D.Format				= m_Format;
		descTex3D.Height				= m_uHeight;
		descTex3D.MipLevels				= m_bSwapChain ? 1 : NUM_RESOURCES;
		descTex3D.MiscFlags				= 0;
		descTex3D.Width					= m_uWidth;
		descTex3D.BindFlags				= uBind;
		descTex3D.CPUAccessFlags		= uCPUAccess;
		descTex3D.Usage					= usage;
        
        hr = GetFramework()->CreateTexture3DMapDefaultOptional( &descTex3D, 0, (ID3D11Texture3D**)pResDest );

		break;
	}

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::CopyResource() - Create call failed, hr = 0x%x" ), hr );
		return false;
	}

	GetImmediateContext()->CopyResource( *pResDest, pResSource );
	
	return true;
}

//---------------------------------------------------------------

bool CRenderTargetsTest::LockResource( ID3D11Resource *pResource, UINT32 nSubresource, D3D_RESOURCE_SCENARIO resType, void **ppData, UINT &uRowPitch, UINT &uDepthPitch )
{
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	HRESULT hr = S_OK;

	uRowPitch = 0;
	uDepthPitch = 0;

	switch( resType )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		hr = GetImmediateContext()->Map( pResource, 0, D3D11_MAP_READ, 0, &mappedRes );
		*ppData = mappedRes.pData;
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		hr = GetImmediateContext()->Map( pResource, nSubresource, D3D11_MAP_READ, 0, &mappedRes );
		*ppData = mappedRes.pData;
		uRowPitch = mappedRes.RowPitch;
		uDepthPitch = mappedRes.DepthPitch;
		break;
	}

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::LockResource() - Map() failed, hr = 0x%x" ), hr );
		return false;
	}

	return true;
}

//---------------------------------------------------------------

bool CRenderTargetsTest::UnLockResource( ID3D11Resource *pResource, UINT32 nSubresource, D3D_RESOURCE_SCENARIO resType )
{
	switch( resType )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		GetImmediateContext()->Unmap(pResource,0);
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:	
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		GetImmediateContext()->Unmap(pResource, nSubresource);
		break;
	}

	return true;
}

//---------------------------------------------------------------

bool CRenderTargetsTest::CompareResults( void *pResultData, UINT32 nSubresource, UINT uRowPitch, UINT uDepthPitch )
{
	const DXGI_FORMAT rtvFormat = GetRTViewFormat(m_Format);
	UINT uDim;
	UINT width = m_uWidth;
	UINT height = m_uHeight;
	UINT depth = 1;
	if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE3D )
		depth = m_uDepth;

	if( m_ResourceType == D3D_RESOURCE_SCENARIO_BUFFER || m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE1D )
	{
		height = 1;
		uDim = m_uWidth;
	}
	else if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURECUBE )
	{
		height = m_uWidth;
		uDim = m_uWidth * m_uWidth;
	}
	else
		uDim = m_uWidth * m_uHeight;
	bool bOneDim = m_ResourceType == D3D_RESOURCE_SCENARIO_BUFFER || m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE1D || m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY;

	UINT size = max( GetBitsPerElement( rtvFormat ) / 8, 1 );
	if( m_ResourceType != D3D_RESOURCE_SCENARIO_BUFFER && !m_bSwapChain)
		GetMipDimensions( m_uMipLevel, width, height, depth );

	BYTE *pDataBytes = (BYTE*) pResultData;
	BYTE *pExpected = m_pResourceData[ 0 ];
	BYTE *pYUVPlanarReformattedData = new BYTE[ size * width * height * depth ];	// We take the IHV's planar data (NV12, etc...), format it to the RTV format, and then verify the data from here
	if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE3D )
		pDataBytes += uDepthPitch * (depth - 1);

	UINT bytesToCheck = size;

	//
	// Do not check the X channel, writes to the X channel are undefined
	//
	switch( m_Format )
	{
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		assert(bytesToCheck == 4);
		bytesToCheck = 3;
		break;
	}

	// If the format is YUV planar, then we need to convert the pdata to the suitable format. Because the memory is laid out linearly.
	if( CD3D10FormatHelper_Test::Planar( m_Format ) )
	{
		// TODO: Currently we don't support "ReformatPixels" pixels for any planar format besides NV12
		// Other planar formats should be skipped for now.
		assert( m_Format == DXGI_FORMAT_NV12 );
		assert( depth == 1 );
		
		const UINT resNumElements = width * height * depth;
		FLOAT_COLOR *pScratch = new FLOAT_COLOR[ resNumElements ];
		ZeroMemory( pScratch, sizeof( FLOAT_COLOR ) * resNumElements );

		// Convert the YUV data to the view format
		CFormatHelper::ReformatPixels( 
                    CONV_FROM_FORMAT, m_Format, pScratch, height, width, depth, pResultData, uRowPitch );

		// The data in ReformatPixels is always treated as UNORM. See if we need to convert to UINT
		const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(rtvFormat);
		if( pTexFormatInfo->dwFlags & FF_UINT )
		{
			// Convert from UNORM to UINT
			for( UINT i = 0; i < resNumElements; i++ )
			{
				pScratch[i].comp[0] *= 255.f;
				pScratch[i].comp[1] *= 255.f;
				pScratch[i].comp[2] *= 255.f;
				pScratch[i].comp[3] *= 255.f;
			}
		}
		else if( pTexFormatInfo->dwFlags & FF_UNORM )
		{
			// Nothing to do...
		}
		else
		{
			LogError( __FILEW__, __LINE__, L"Planar video format is used, but the RTV format is not UINT or UNORM type.", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConflictingParameters );
			return false;
		}

		if ( !CFormatHelper::ConvertPixels(
			CONV_TO_FORMAT, GetRTViewFormat(m_Format), pScratch, height, width, depth, pYUVPlanarReformattedData ) )
		{
			LogError( __FILEW__, __LINE__, L"CFormatHelper::ConvertPixels() failed", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConflictingParameters );
			return false;
		}

		// Convert the expected data to the view format

		// Now the data in pYUVPlanarReformattedData is tightly packed
		pResultData = pYUVPlanarReformattedData;
		uRowPitch = width * size;
		uDepthPitch = height * uRowPitch;
		delete [] pScratch;
	}

	bool bPass = true;
	bool bLogAll = GetFramework( )->LogVerbose( );
	for( UINT y = 0; y < height; ++y )
	{
		for( UINT x = 0; x < width; ++x )
		{	
			for( UINT iB = 0; iB < bytesToCheck; ++iB )
			{
				BYTE resultExpected = pExpected[ iB ];
				BYTE resultRendered = pDataBytes[ iB ];
				if( resultExpected != resultRendered && (bPass || bLogAll) )
				{
					WriteToLog( _T( "Test case failed for subresource %d, byte %d at (%d,%d) does not match.\n"
						"	Expected: 0x%02x\n"
						"	Actual:	  0x%02x" ), nSubresource, iB, x, y, resultExpected, resultRendered );
					bPass = false;
				}
			}
			pExpected += size;
			pDataBytes += size;
		}

		if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE3D )
			pDataBytes = (BYTE*) pResultData + uDepthPitch * (depth - 1) + (y + 1) * uRowPitch;
		else
			pDataBytes = (BYTE*) pResultData + (y + 1) * uRowPitch;
	}
	
	delete [] pYUVPlanarReformattedData;
	return bPass;
}

bool CRenderTargetsTest::IsDenorm( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;

	return ((uValue & exp) == 0) && ((uValue & frac) != 0);
}

bool CRenderTargetsTest::IsNaN( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;

	return ((uValue & exp) == 0x7f800000) && ((uValue & frac) != 0);
}

DXGI_FORMAT CRenderTargetsTest::GetRTViewFormat( const DXGI_FORMAT &resourceFormat )
{
	/*
		If the format is a video format, return a supported RTV format. If it is not a video format, return the same format that was passed in.
	*/

	if( CD3D10FormatHelper_Test::YUV( resourceFormat ) )
	{
		// The format is a video format, see if it supports RTVs
		std::vector<DXGI_FORMAT> luminanceDataFormats, chrominanceDataFormats;
		if( CFormatHelper::GetRTViewFormatsForVideoResourceFormat( resourceFormat, luminanceDataFormats, chrominanceDataFormats ) )
		{
			// The format is a video format AND supports RTVs, so we need to determine the correct view format...
			// BUGBUG, we will just get luminance coverage for formats that split the views. Even then, we only get the first format.
			assert( luminanceDataFormats.size() >= 1 );
			return luminanceDataFormats[0];
		}
	}

	return resourceFormat;
}

DXGI_FORMAT CRenderTargetsTest::GetSRViewFormat( const DXGI_FORMAT &resourceFormat )
{
	/*
		If the format is a video format, return a supported SRV format. If it is not a video format, return the same format that was passed in.
	*/

	if( CD3D10FormatHelper_Test::YUV( resourceFormat ) )
	{
		// The format is a video format, see if it supports SRVs
		std::vector<DXGI_FORMAT> luminanceDataFormats, chrominanceDataFormats;
		if( CFormatHelper::GetSRViewFormatsForVideoResourceFormat( resourceFormat, luminanceDataFormats, chrominanceDataFormats ) )
		{
			// The format is a video format AND supports SRVs, so we need to determine the correct view format...
			// BUGBUG, we will just get luminance coverage for formats that split the views. Even then, we only get the first format.
			assert( luminanceDataFormats.size() >= 1 );
			return luminanceDataFormats[0];
		}
	}

	return resourceFormat;
}

bool CRenderTargetsTest::GetMipDimensions( UINT uMipSlice, UINT &uWidth, UINT &uHeight, UINT &uDepth )
{
	if( uWidth == 0 || uHeight == 0 || uDepth == 0 )
		return false;

	bool bSmallestMip = false;

	UINT uCurrMip = 0;
	while( uCurrMip < uMipSlice )
	{
		if( bSmallestMip )
			return false;

		if( uWidth > 1 ) uWidth /= 2;
		if( uHeight > 1 ) uHeight /= 2;
		if( uDepth > 1 ) uDepth /= 2;
		uCurrMip++;

		if( uWidth == 1 && uHeight == 1 && uDepth == 1 )
			bSmallestMip = true;
	}

	return true;
}

bool CRenderTargetsTest::WithinOneULP( float fReference, float fOutput )
{
	// WARNING:
	// This function does not correctly handle edge cases involving negative floats, zeros
	// and NaNs. If future test result discrepancies are discovered to have been masked by this function,
	// please start using CompareFloatULP() function in Framework\Core\FloatUtils.cpp which
	// does not suffer from these problems.

	if( fReference == fOutput )
		return true;

	UINT32 uRef = *( (UINT32*) &fReference );
	UINT32 uOut = *( (UINT32*) &fOutput );
	if( (int)max( uRef, uOut ) - (int)min( uRef, uOut ) <= 1 )
		return true;

	return false;
}

//---------------------------------------------------------------

void CRenderTargetsTestApp::InitOptionVariables( )
{
	CD3D11TestFramework::InitOptionVariables();
	UnRegisterOptionVariable( "SrcOnly" );
	m_D3DOptions.SrcOnly = true;

	// Indicate that this test works on feature level 9
	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;

	int n = AddOptionCategory( _T( "Debug" ) );
}

//---------------------------------------------------------------

bool CRenderTargetsTestApp::InitTestGroups( )
{
	if( !ADD_TEST( "RenderTargets", CRenderTargetsTest ) ) return false;
	if( !ADD_TEST( "Clear", CClearRenderTargetViewTest ) ) return false;

	return true;
}
