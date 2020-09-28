#pragma once

#include "ResourceFormats.h"
#include "D3DDepthHelper.h"

#include "MinMaxRange.h"

#include <WGFTestCore.h>

FLOAT_COLOR CResourceFormatsTest::m_ColorSets [ NUM_COLOR_SETS ] [ NUM_COLORS ] = 
{
	{
		{ 1.f, 0.f, 0.f, 1.f },
		{ 0.f, 1.f, 0.f, 1.f },
		{ 0.f, 0.f, 1.f, 1.f },
		{ 0.5f, 0.5f, 0.f, 1.f }
	},
	{
		{ 0.f, 0.f, 0.f, 1.f },
		{ 110.f, 1.5f, 0.f, 0.5f },
		{ -23.f, 0.f, 1.f, 0.f },
		{ 0.5f, 0.5f, 0.f, 1.f }
	},
	{
		{ 1.f, 0.f, 0.5f, 1.f },
		{ 0.f, 1.f, 0.f, 1.f },
		{ 0.f, 0.f, 1.f, 1.f },
		{ 0.5f, 0.5f, 0.f, 1.f }
	}
};

CResourceFormatsTestApp g_TestApp;

/******************************
 * CResourceFormatsTest methods
 ******************************/

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

//---------------------------------------------------------------

void CResourceFormatsTest::InitTestParameters( )
{
	CTestCaseParameter<IMAGE_PATTERN> *pPatternParam = AddParameter( _T( "ImagePattern" ), &m_ImagePattern );
	testfactor::RFactor rfStripes = AddParameterValue<IMAGE_PATTERN>( pPatternParam, IP_STRIPES );
	testfactor::RFactor rfGradient = AddParameterValue<IMAGE_PATTERN>( pPatternParam, IP_GRADIENT );
	testfactor::RFactor rfTiles = AddParameterValue<IMAGE_PATTERN>( pPatternParam, IP_TILES );
	
	CTestCaseParameter<UINT32> *pNumStripesParam = AddParameter( _T( "NumStripes" ), &m_uNumStripes );
	CUserValueSet<UINT32> *pNumStripesValues = new CUserValueSet<UINT32>( );
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

	CTestCaseParameter<UINT32> *pWidthParam = AddParameter( _T( "Width" ), &m_uWidth );
	CUserValueSet<UINT32> *pWidthValues = new CUserValueSet<UINT32>( );
	pWidthValues->AddValue( 128 );
	pWidthValues->AddValue( 64 );
	pWidthValues->AddValue( 90 );
	pWidthValues->AddValue( 1 );
	testfactor::RFactor rfWidth = AddParameterValueSet( pWidthParam, pWidthValues );
	testfactor::RFactor rfWidthMipped = AddParameterValue<UINT32>( pWidthParam, TEXTURE_SIZE );

	CTestCaseParameter<UINT32> *pHeightParam = AddParameter( _T( "Height" ), &m_uHeight );
	CUserValueSet<UINT32> *pHeightValues = new CUserValueSet<UINT32>( );
	pHeightValues->AddValue( 128 );
	pHeightValues->AddValue( 256 );
	pHeightValues->AddValue( 90 );
	pHeightValues->AddValue( 1 );
	testfactor::RFactor rfHeight = AddParameterValueSet( pHeightParam, pHeightValues );
	testfactor::RFactor rfHeightMipped = AddParameterValue<UINT32>( pHeightParam, TEXTURE_SIZE );

	CTestCaseParameter<UINT32> *pDepthParam = AddParameter( _T( "Depth" ), &m_uDepth );
	CUserValueSet<UINT32> *pDepthValues = new CUserValueSet<UINT32>( );
	pDepthValues->AddValue( 4 );
	pDepthValues->AddValue( 3 );
	pDepthValues->AddValue( 2 );
	pDepthValues->AddValue( 1 );
	testfactor::RFactor rfDepth = AddParameterValueSet( pDepthParam, pDepthValues );
	testfactor::RFactor rfDepthMipped = AddParameterValue<UINT32>( pDepthParam, 5 );

	CTestCaseParameter<UINT32> *pMipSliceParam = AddParameter( _T( "MipSlice" ), &m_uMipSlice );
	CUserValueSet<UINT32> *pMipSliceValues = new CUserValueSet<UINT32>( );
	pMipSliceValues->AddValue( 0 );
	pMipSliceValues->AddValue( 1 );
	pMipSliceValues->AddValue( 2 );
	pMipSliceValues->AddValue( 3 );
	pMipSliceValues->AddValue( 4 );
	pMipSliceValues->AddValue( 5 );
	testfactor::RFactor rfMipSlice = AddParameterValueSet( pMipSliceParam, pMipSliceValues );

	CTestCaseParameter<UINT32> *pArraySliceParam = AddParameter( _T( "ArraySlice" ), &m_uArraySlice );
	CUserValueSet<UINT32> *pArraySliceValues = new CUserValueSet<UINT32>( );
	pArraySliceValues->AddValue( 0 );
	pArraySliceValues->AddValue( 1 );
	pArraySliceValues->AddValue( 2 );
	pArraySliceValues->AddValue( 3 );
	pArraySliceValues->AddValue( 4 );
	pArraySliceValues->AddValue( 5 );
	testfactor::RFactor rfArraySlice = AddParameterValueSet( pArraySliceParam, pArraySliceValues );

	CTestCaseParameter<D3D_RESOURCE_SCENARIO> *pResParam = AddParameter( _T( "ResourceScenario" ), &m_ResourceScenario );
	testfactor::RFactor rfBuffer = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_BUFFER );
	testfactor::RFactor rfTex1D = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE1D );
	testfactor::RFactor rfTex1DArr = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY );
	testfactor::RFactor rfTex2D = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D );
	testfactor::RFactor rfTex2DArr = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY );
	testfactor::RFactor rfTex3D = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE3D );
	
	CTestCaseParameter<bool> *pTypelessParam = AddParameter( _T("TypelessResource"), &m_bUseTypeless );
	testfactor::RFactor rfTypeless = AddParameterValue<bool>( pTypelessParam, true );
	testfactor::RFactor rfTyped = AddParameterValue<bool>( pTypelessParam, false );

	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T( "Format"), &m_Format );
	CFilterHandleCommonSource<DXGI_FORMAT> formatFilter = !(
		FilterRegExp<DXGI_FORMAT>( _T( "R1" ) ) ||	// Win8: 63065 - WGFResourceFormats: Remove expected support for R1_UNORM in CheckFormatSupport group
		FilterRegExp<DXGI_FORMAT>( _T( "UNKNOWN" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "TYPELESS" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "BC6H" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "BC7" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "SHAREDEXP" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "R8G8_B8G8" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "G8R8_G8B8" ) ) );
	testfactor::RFactor rfFormats = AddParameterValueSet( pFormatParam, FormatValueSet( formatFilter ) );

	CUserValueSet<DXGI_FORMAT> *pDepthFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	pDepthFormatValues->AddValue( DXGI_FORMAT_D32_FLOAT );
	pDepthFormatValues->AddValue( DXGI_FORMAT_D32_FLOAT_S8X24_UINT );
	pDepthFormatValues->AddValue( DXGI_FORMAT_D24_UNORM_S8_UINT );
	pDepthFormatValues->AddValue( DXGI_FORMAT_D16_UNORM );
	testfactor::RFactor rfDepthFormats = AddParameterValueSet( pFormatParam, pDepthFormatValues );

	CTestCaseParameter<bool> *pOneMipParam = AddParameter( _T( "OneMipLevel" ), &m_bOneMip );
	testfactor::RFactor rfOneMipT = AddParameterValue<bool>( pOneMipParam, true );
	testfactor::RFactor rfOneMipF = AddParameterValue<bool>( pOneMipParam, false );
	testfactor::RFactor rfOneMip = AddParameterValueSet<bool>( pOneMipParam, BoolValueSet() );

	SetRootTestFactor( 
		( rfFormats * rfBuffer * rfTiles * rfTileSize * rfWidth * rfOneMipF ) +
		( rfFormats * rfTypeless * rfTex1D * rfTiles * rfTileSize * rfWidthMipped * rfMipSlice * rfOneMip ) +
		( rfFormats * rfTyped * rfTex1DArr * rfTiles * rfTileSize * rfWidth * rfArraySlice * rfOneMipF ) +
		( rfFormats * rfTyped * rfTex2D * rfTiles * rfTileSize * (rfWidthMipped % rfHeightMipped) * rfMipSlice * rfOneMip ) +
		( rfFormats * rfTypeless * rfTex2DArr * rfTiles * rfTileSize * (rfWidth % rfHeight) * rfArraySlice * rfOneMipF ) +
		( rfFormats * rfTyped * rfTex2D * rfStripes * rfNumStripes * rfVertStripes * (rfWidthMipped % rfHeightMipped) * rfMipSlice * rfOneMip ) +
		( rfFormats * rfTyped * rfTex2DArr * rfStripes * rfNumStripes * rfVertStripes * (rfWidth % rfHeight) * rfArraySlice * rfOneMipF ) +
		( rfFormats * rfTypeless* rfTex2D * rfGradient * (rfWidthMipped % rfHeightMipped) * rfMipSlice * rfOneMip ) +
		( rfFormats * rfTyped * rfTex2DArr * rfGradient * (rfWidth % rfHeight) * rfArraySlice * rfOneMipF ) +
		( rfFormats * rfTypeless* rfTex3D * rfTiles * rfTileSize * (rfWidthMipped % rfHeightMipped % rfDepthMipped) * rfOneMip ) +
		( rfFormats * rfTyped * rfTex3D * rfStripes * rfNumStripes * rfVertStripes * (rfWidthMipped % rfHeightMipped % rfDepthMipped) * rfOneMip ) +
		( rfFormats * rfTypeless * rfTex3D * rfGradient * (rfWidthMipped % rfHeightMipped % rfDepthMipped) * rfOneMip )

	);

	AddPriorityPatternFilter(FilterEqual<D3D_RESOURCE_SCENARIO>(pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D), WeightFactorVeryLarge);

	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(pFormatParam, DXGI_FORMAT_R8G8B8A8_UNORM), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<DXGI_FORMAT>(pFormatParam, DXGI_FORMAT_B8G8R8A8_UNORM), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<IMAGE_PATTERN>(pPatternParam, IP_GRADIENT), WeightFactorLarge);

	AddPriorityPatternFilter(FilterEqual<bool>(pTypelessParam, true), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<bool>(pOneMipParam, true), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<UINT>(pWidthParam, 128), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<UINT>(pHeightParam, 128), WeightFactorSmall);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

//---------------------------------------------------------------

void CResourceFormatsTest::GetFormatsFactor( testfactor::RFactor *pRFactor )
{
	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T( "Format"), &m_Format );
	CFilterHandleCommonSource<DXGI_FORMAT> formatFilter = !(
		FilterRegExp<DXGI_FORMAT>( _T( "UNKNOWN" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "TYPELESS" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "BC6H" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "BC7" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "SHAREDEXP" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "R8G8_B8G8" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "G8R8_G8B8" ) ) );
	*pRFactor = AddParameterValueSet( pFormatParam, FormatValueSet( formatFilter ) );
}

//---------------------------------------------------------------

TEST_RESULT CResourceFormatsTest::SetupGeometry( )
{
	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.ByteWidth      = sizeof( TLVERTEX );
	buffer_desc.Usage          = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags      = 0;

	HRESULT hr;
	if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &buffer_desc, NULL, &m_pVertexBuffer) ) )
	{
		WriteToLog( _T("CResourceFormatsTest::SetupGeometry() - CreateBuffer() for Stream Input Buffer failed. hr = 0x%x"), hr );
		return RESULT_FAIL;
	}

	UINT StridesTL = sizeof( TLVERTEX );
	UINT Offset = 0;

	TLVERTEX vertices [ ] = 
	{
		{ 0.f, 0.f, 0.f, 0.f, 0.f, 0.f }
	};

	D3D11_MAPPED_SUBRESOURCE pBufferData;
	if( FAILED( hr = GetImmediateContext( )->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pBufferData ) ) )
	{
		WriteToLog( _T("CResourceFormatsTest::SetupGeometry() - Map() failed. hr = 0x%x"), hr );
		return RESULT_FAIL;
	}

	memcpy( pBufferData.pData, vertices, sizeof( vertices ) );
	GetImmediateContext( )->Unmap( m_pVertexBuffer, 0 );

	GetEffectiveContext( )->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	GetEffectiveContext( )->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &StridesTL, &Offset);

	return RESULT_PASS;
}

//---------------------------------------------------------------

TEST_RESULT CResourceFormatsTest::SetupShaders( )
{
	TEST_RESULT bResult = RESULT_PASS;
	HRESULT hr;
	D3DXMATRIX MatWorld;

	m_pVS = NULL; 
	m_pGS = NULL;
	m_pPS = NULL;

	GetEffectiveContext()->VSSetSamplers(0, 1, &m_pSampler);
	GetEffectiveContext()->GSSetSamplers(0, 1, &m_pSampler);
	GetEffectiveContext()->PSSetSamplers(0, 1, &m_pSampler);

	// create shaders and layout
	ID3D10Blob *pShaderBlob = NULL;
	ID3D10Blob *pErrorBlob = NULL;

	if( FAILED( hr = g_TestApp.CompileShaderFromResource(	
		NULL, 
		"RC_RESOURCEFORMATS_GSH", 
		"GShaderMain", 
		D3D_SHADER_STAGE_GEOMETRY,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_11_0,
		D3D10_SHADER_DEBUG, 
		&pShaderBlob, 
		&pErrorBlob ) ) )
	{
		WriteToLog( _T( "CResourceFormatsTest::Setup() - D3DXCompileShaderFromResource() failed with geometry shader, hr = 0x%x, errors: \n%s" ), hr, pErrorBlob->GetBufferPointer( ) );
		goto Fail;
	}

	if( FAILED( hr = GetDevice()->CreateGeometryShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize( ), NULL, &m_pGS ) ) )
	{
		WriteToLog( _T( "CResourceFormatsTest::Setup() - CreateGeometryShader failed, hr = 0x%x" ), hr );
		goto Fail;
	}

	SAFE_RELEASE( pShaderBlob );
	SAFE_RELEASE( pErrorBlob );

	if( FAILED( hr = g_TestApp.CompileShaderFromResource(	
		NULL, 
		"RC_RESOURCEFORMATS_PSH", 
		"PShaderMain", 
		D3D_SHADER_STAGE_PIXEL,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_11_0,
		D3D10_SHADER_DEBUG, 
		&pShaderBlob, 
		&pErrorBlob ) ) )
	{
		WriteToLog( _T( "CResourceFormatsTest::Setup() - D3DXCompileShaderFromResource() failed with pixel shader, hr = 0x%x, errors: \n%s" ), hr, pErrorBlob->GetBufferPointer( ) );
		goto Fail;
	}

	if( FAILED( hr = GetDevice()->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize( ), NULL, &m_pPS ) ) )
	{
		WriteToLog( _T( "CResourceFormatsTest::Setup() - CreatePixelShader failed, hr = 0x%x" ), hr );
		goto Fail;
	}

	SAFE_RELEASE( pShaderBlob );
	SAFE_RELEASE( pErrorBlob );

	if( FAILED( hr = g_TestApp.CompileShaderFromResource(	
		NULL, 
		"RC_RESOURCEFORMATS_VSH", 
		"VShaderMain", 
		D3D_SHADER_STAGE_VERTEX,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_11_0,
		D3D10_SHADER_DEBUG, 
		&pShaderBlob, 
		&pErrorBlob ) ) )
	{
		WriteToLog( _T( "CResourceFormatsTest::Setup() - D3DXCompileShaderFromResource() failed with vertex shader, hr = 0x%x, errors: \n%s" ), hr, pErrorBlob->GetBufferPointer( ) );
		goto Fail;
	}

	if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize( ), NULL, &m_pVS ) ) )
	{
		WriteToLog( _T( "CResourceFormatsTest::Setup() - CreateVertexShader failed, hr = 0x%x" ), hr );
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
		WriteToLog( _T( "CResourceFormatsTest::Setup() - CreateInputLayout failed, hr = 0x%x" ), hr );
		goto Fail;
	}

	GetEffectiveContext()->IASetInputLayout( m_pLayout );

	// setup constant buffer
	D3D11_BUFFER_DESC bufdesc;
	bufdesc.ByteWidth = 512;
	bufdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufdesc.MiscFlags = 0;
	GetFramework()->CreateBufferTiledOptional( &bufdesc, NULL, &m_pConstBuffer );

	GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );

	D3D11_MAPPED_SUBRESOURCE pConstData;
	if( FAILED( hr = GetImmediateContext()->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pConstData ) ) )
	{
		WriteToLog( _T( "CResourceFormatsTest::Setup() - Map failed, hr = 0x%x" ), hr );
		goto Fail;
	}

	D3DXMatrixIdentity( &MatWorld );
	float transx = (float) m_nPrimitiveSize / 2.f;
	float transy = (float) m_nPrimitiveSize / 2.f;

	// Inverse viewport scale.
	MatWorld._13 = 1 / transx;
	MatWorld._14 = -1 / transy;

	// Prescaled inverse viewport translation.
	MatWorld._11 = -transx * MatWorld._13;
	MatWorld._12 = -transy * MatWorld._14;

	m_mxTrans = MatWorld;

	memcpy( pConstData.pData, MatWorld, sizeof( MatWorld ) );
	GetImmediateContext()->Unmap(m_pConstBuffer,0);

	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );

	goto Pass;
Fail:
	bResult = RESULT_FAIL;
Pass:
	SAFE_RELEASE( pShaderBlob );
	SAFE_RELEASE( pErrorBlob )

	return bResult;
}

//---------------------------------------------------------------

TEST_RESULT CResourceFormatsTest::Setup( )
{
	TEST_RESULT tr;
	HRESULT hr;

	if( (tr = SetupGeometry( )) != RESULT_PASS )
		return tr;

	if( (tr = SetupShaders( )) != RESULT_PASS )
		return tr;

	// create and set viewport
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float) m_nPrimitiveSize;
	vp.Height   = (float) m_nPrimitiveSize;
	vp.MinDepth     = 0;
	vp.MaxDepth     = 1;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	GetEffectiveContext()->RSSetViewports(1, &vp);

	// create and set rasterizer state
	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = false;
	rastDesc.DepthBias = false;
	rastDesc.DepthBiasClamp = 0;
	rastDesc.SlopeScaledDepthBias = 0;
	rastDesc.DepthClipEnable = false;
	rastDesc.ScissorEnable = false;
	rastDesc.MultisampleEnable = false;
	rastDesc.AntialiasedLineEnable = false;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSEnabled );
	GetDevice()->CreateRasterizerState( &rastDesc, &m_pRSDisabled );

	GetEffectiveContext()->RSSetState( m_pRSEnabled );

	//	GetDevice()->OMSetDepthStencilState( m_pDepthStencilState, 0 );

	// create and set sampler
	m_SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	m_SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	m_SamplerDesc.MipLODBias = 0;
	m_SamplerDesc.MaxAnisotropy = 1;
	m_SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	m_SamplerDesc.BorderColor[0] = 0.75f;
	m_SamplerDesc.BorderColor[1] = 0.75f;
	m_SamplerDesc.BorderColor[2] = 0.75f;
	m_SamplerDesc.BorderColor[3] = 0.75f;
	m_SamplerDesc.MinLOD = 0;
	m_SamplerDesc.MaxLOD = 5;

	if( FAILED( hr = GetDevice()->CreateSamplerState(&m_SamplerDesc, &m_pSampler ) ))
	{
		WriteToLog( _T("CRenderTargetsTest::Setup() - CreateSamplerState() failed. hr = 0x%x"), hr );
		return RESULT_FAIL;
	}

	GetEffectiveContext()->PSSetSamplers(0, 1, &m_pSampler);

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

//---------------------------------------------------------------

void CResourceFormatsTest::Cleanup( )
{
	if( GetDevice( ) )
	{
		ID3D11Buffer *pNullBuffers [ ] = { NULL, NULL };
		UINT uZero = 0;
		GetEffectiveContext()->VSSetConstantBuffers( 0, 2, pNullBuffers );
		GetEffectiveContext()->GSSetConstantBuffers( 0, 2, pNullBuffers );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 2, pNullBuffers );
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, pNullBuffers, &uZero, &uZero );

		ID3D11VertexShader *pVSNull = NULL;
		ID3D11GeometryShader *pGSNull = NULL;
		ID3D11PixelShader *pPSNull = NULL;
		GetEffectiveContext()->VSSetShader( pVSNull, NULL, 0 );
		GetEffectiveContext()->GSSetShader( pGSNull, NULL, 0 );
		GetEffectiveContext()->PSSetShader( pPSNull, NULL, 0 );

		ID3D11InputLayout *pILNull = NULL;
		ID3D11RasterizerState *pRSNull = NULL;
		GetEffectiveContext()->IASetInputLayout( pILNull );
		GetEffectiveContext()->RSSetState( pRSNull );
	}

	CD3D11Test::Cleanup();

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
	}

	SAFE_RELEASE( m_pConstBuffer );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pRSDisabled );
	SAFE_RELEASE( m_pRSEnabled );
	SAFE_RELEASE( m_pRTView );
	SAFE_RELEASE( m_pTextureRT );
	SAFE_RELEASE( m_pTextureRTCopy );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pLayout );
	SAFE_RELEASE( m_pSampler );
	SAFE_RELEASE( m_pSRV );
}

//---------------------------------------------------------------

TEST_RESULT CResourceFormatsTest::SetupTestCase( )
{
	HRESULT hr = E_FAIL;
	UINT supportFlags;

	hr = GetDevice()->CheckFormatSupport( m_Format, &supportFlags );
	if( FAILED( hr ) )
	{
		// New video formats are optional and can fail.
		if( CD3D10FormatHelper_Test::YUV( m_Format ) )
		{
			WriteToLog( _T("%s is a video format. Video formats are optional."), ToString(m_Format).c_str());
			SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "Format"), m_Format ) );
			return RESULT_SKIP;
		}

		// The 16 bits formats may fail if the driver doesn't support the D3D11.1 DDI
		if( !GetFramework()->Is11_1DDISupported( GetDevice() ) &&	// Check to see if the driver supports the 11.1 DDI
			CD3D10FormatHelper_Test::Is16BitBGRFormat( m_Format ) )	// The 16 bit formats (B5G6R5, etc...) are only required to pass CheckFormatSupport on the 11.1 DDI
		{
			WriteToLog( _T( "CheckFormatSupport() failed. However the driver doesn't support the D3D11.1 DDI, so this is OK for the 16-bit formats. " ) );
			SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "Format"), m_Format ) );
			return RESULT_SKIP;
		}

		WriteToLog( _T("CheckFormatSupport() failed for %s, hr = %s"), ToString(m_Format).c_str(), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	if( (supportFlags & D3D10_FORMAT_SUPPORT_SHADER_LOAD) == 0)
	{
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "Format"), m_Format ) );
		return RESULT_SKIP;
	}

	UINT uScenFlags = 0;
	GetScenarioFlags( m_ResourceScenario, &uScenFlags );

	if( m_uWidth == 1 && m_uHeight == 1 && (m_ImagePattern == IP_TILES || m_ImagePattern == IP_STRIPES ) )
		return RESULT_SKIP;

	std::map< D3D_RESOURCE_SCENARIO, D3D11_FORMAT_SUPPORT > resTypes;
	resTypes[ D3D_RESOURCE_SCENARIO_BUFFER ] = D3D11_FORMAT_SUPPORT_BUFFER;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE1D ] = D3D11_FORMAT_SUPPORT_TEXTURE1D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY ] = D3D11_FORMAT_SUPPORT_TEXTURE1D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE2D ] = D3D11_FORMAT_SUPPORT_TEXTURE2D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY ] = D3D11_FORMAT_SUPPORT_TEXTURE2D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE3D ] = D3D11_FORMAT_SUPPORT_TEXTURE3D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURECUBE ] = D3D11_FORMAT_SUPPORT_TEXTURE2D;

	if( (supportFlags & resTypes[ m_ResourceScenario ]) == 0 )
	{
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "ResourceScenario" ), m_ResourceScenario ) );
		return RESULT_SKIP;
	}

	D3D10_FORMAT_COMPONENT_INTERPRETATION fci = CD3D10FormatHelper_Test::GetFormatComponentInterpretation( GetSRViewFormat(m_Format), 0 );
	if( fci == D3D10FCI_SINT || fci == D3D10FCI_UINT )
		m_FormatRT = FORMAT_RT_INT;
	else 
		m_FormatRT = FORMAT_RT_FLOAT;

	bool bBC = false;
	switch(m_Format)
	{
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:     
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:     
	case DXGI_FORMAT_BC4_SNORM: 
	case DXGI_FORMAT_BC2_TYPELESS:  
	case DXGI_FORMAT_BC2_UNORM:     
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:  
	case DXGI_FORMAT_BC3_UNORM:     
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:   
	case DXGI_FORMAT_BC5_UNORM:     
	case DXGI_FORMAT_BC5_SNORM: 
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		bBC = true;
		break;
	}

	bool bCube = (uScenFlags & D3D_RS_FLAGS_TEXTURECUBE ) != 0;
	bool bOneDim = (uScenFlags & D3D_RS_FLAGS_TEXTURE1D) || (uScenFlags & D3D_RS_FLAGS_BUFFER);
	bool bArray = (uScenFlags & D3D_RS_FLAGS_ARRAY) != 0;
	bool bFormatSupportsMips = (supportFlags & D3D11_FORMAT_SUPPORT_MIP) != 0;	// Some video formats don't support mip levels
	bool bMipped = bFormatSupportsMips && !bArray && !bCube && m_ResourceScenario != D3D_RESOURCE_SCENARIO_BUFFER && m_ResourceScenario != D3D_RESOURCE_SCENARIO_TEXTURE3D;

	UINT height;
	UINT width = m_uWidth;
	UINT depth = 1;
	if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D || m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY || m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER ) 
		height = 1;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURECUBE )
		height = m_uWidth;
	else
		height = m_uHeight;

	GetMipDimensions( !bMipped ? 0 : m_uMipSlice, width, height, depth );

	if( bBC && (bMipped || (width % 4 != 0) || (height % 4 != 0) ) )
	{
		return RESULT_SKIP;
	}

	if ( !bFormatSupportsMips && m_uMipSlice > 0 )
	{
		return RESULT_SKIP;
	}

	// Set shader resource(s)
	TEST_RESULT tr = CreateShaderResources( );
	if( tr != RESULT_PASS )
		return tr;

	// Set render target(s)
	if( !CreateRenderTargets( ) )
		return RESULT_FAIL;

	// create and set viewport
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = (float) width;
	vp.Height   = (float) height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	GetEffectiveContext()->RSSetViewports(1, &vp);

	// pass current width and height to shader
	D3D11_MAPPED_SUBRESOURCE pData;
	if( FAILED( hr = GetImmediateContext( )->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupTestCase() - failed to map const buffer, hr = 0x%x" ), hr );
		return RESULT_FAIL;
	}

	D3DXMatrixIdentity( &m_mxTrans );
	float transx = (float) width / 2.f;
	float transy = (float) height / 2.f;

	// Inverse viewport scale.
	m_mxTrans._13 = 1.f / transx;
	m_mxTrans._14 = -1.f / transy;

	// Prescaled inverse viewport translation.
	m_mxTrans._11 = -transx * m_mxTrans._13;
	m_mxTrans._12 = -transy * m_mxTrans._14;

	SIZE_T offset = sizeof( float ) * 4;
	memcpy( pData.pData, &m_mxTrans, offset );
	
	float dims[ 2 ] = { (float) width, (float) height };
	memcpy( ((BYTE*)pData.pData) + offset, dims, sizeof(float) * 2 );
	offset += sizeof(float) * 2;

	int iRes = 0;
	if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER )
		iRes = 0;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D )
		iRes = 1;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY )
		iRes = 2;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D )
		iRes = 3;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY )
		iRes = 4;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D )
		iRes = 5;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURECUBE )
		iRes = 6;

	memcpy( ((BYTE*)pData.pData) + offset, &iRes, sizeof(int) );
	offset += sizeof( int );

	UINT uSlice = m_bOneMip ? 0 : m_uMipSlice;
	if( bArray )
		uSlice = m_uArraySlice;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D )
		uSlice = m_uDepth;

	memcpy( ((BYTE*)pData.pData) + offset, &uSlice, sizeof(int) );

	GetImmediateContext( )->Unmap( m_pConstBuffer, 0 );

	return RESULT_PASS;
}

//---------------------------------------------------------------

TEST_RESULT CResourceFormatsTest::CreateShaderResources()
{
	HRESULT hr;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_TEXTURE1D_DESC tex1DDesc;
	D3D11_TEXTURE2D_DESC tex2DDesc;
	D3D11_TEXTURE3D_DESC tex3DDesc;
	D3D11_SUBRESOURCE_DATA subResData[ 6 ];
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

	UINT formatSupport = 0;
	if( FAILED( hr = GetDevice()->CheckFormatSupport( m_Format, &formatSupport ) ) )
	{
		WriteToLog( _T("CheckFormatSupport() failed with %s. The test should skip before it gets to this point if the format is not supported."), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	UINT scenFlags = 0;
	GetScenarioFlags( m_ResourceScenario, &scenFlags );
	bool bOneDim = m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER || (scenFlags & D3D_RS_FLAGS_TEXTURE1D);
	bool bCube = (scenFlags & D3D_RS_FLAGS_TEXTURECUBE) != 0;
	bool bArray = (scenFlags & D3D_RS_FLAGS_ARRAY) != 0;
	bool bMS = (scenFlags & D3D_RS_FLAGS_MS) != 0;
	bool bFormatSupportsMips = (formatSupport & D3D11_FORMAT_SUPPORT_MIP) != 0;	// Some video formats don't support mip levels
	bool bMipped = bFormatSupportsMips && !bArray && !bCube && m_ResourceScenario != D3D_RESOURCE_SCENARIO_BUFFER && m_ResourceScenario != D3D_RESOURCE_SCENARIO_TEXTURE3D;
	bool bYUV = CD3D10FormatHelper_Test::YUV( m_Format ) ? true : false;
	bool bPlanar = CD3D10FormatHelper_Test::Planar( m_Format ) ? true : false;
	UINT uElementWidth = CD3D10FormatHelper_Test::GetBitsPerElement( m_Format ) / 8;
	UINT iA = 0;
	UINT iRes = 0;
	UINT arraySize = 1;
	if( bArray )
		arraySize = FORMATS_ARRAY_SIZE;
	else if( bCube )
		arraySize = 6;
	else
		arraySize = FORMATS_MIP_LEVELS;

	// Check to see if arrays are supported on the format
	if ( bPlanar && bArray && arraySize > 2 )
	{
		WriteToLog( _T("Texture arrays larger then 2 are not allowed for planar formats.") );
		return RESULT_SKIP;
	}

	UINT height, width, depth;	
	depth = height = 1;
	width = m_uWidth;

	UINT blockSize = 1;
	switch(m_Format)
	{
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:     
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:     
	case DXGI_FORMAT_BC4_SNORM: 
		blockSize = 8;
		break;
	case DXGI_FORMAT_BC2_TYPELESS:  
	case DXGI_FORMAT_BC2_UNORM:     
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:  
	case DXGI_FORMAT_BC3_UNORM:     
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:   
	case DXGI_FORMAT_BC5_UNORM:     
	case DXGI_FORMAT_BC5_SNORM: 
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		blockSize = 16;
		break;
	}

	if( blockSize > 1 && m_uWidth % 4 && m_uHeight % 4 )
		return RESULT_SKIP;

	
	// Check the resource alignment for video
	if( CD3D10FormatHelper_Test::YUV( m_Format ) )
	{
		// Video formats need to check that both the box and the actual resource are aligned
		const UINT horizontalAlignment = CD3D10FormatHelper_Test::GetWidthAlignment( m_Format );
		const UINT verticalAlignment = CD3D10FormatHelper_Test::GetHeightAlignment( m_Format );

		// Check the box alignment and resource alignment
		switch( m_ResourceScenario )
		{
		default:
			WriteToLog( _T("Invalid m_ResourceScenario = %s (%u)."), ToString(m_ResourceScenario).c_str(), m_ResourceScenario );
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
				WriteToLog( _T("The resource's height (%u) doesn't match the format's (%s) vertical alignment (%u)."), m_uHeight, ToString(m_Format).c_str(), verticalAlignment );
				return RESULT_SKIP;
			}
			// fall-through
		case D3D_RESOURCE_SCENARIO_BUFFER:
		case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
			if( (m_uWidth % horizontalAlignment) != 0 )
			{
				WriteToLog( _T("The resource's width (%u) doesn't match the format's (%s) horizontal alignment (%u)."), m_uWidth, ToString(m_Format).c_str(), horizontalAlignment );
				return RESULT_SKIP;
			}
			break;
		}
	}

	for( UINT i = 0; i < arraySize; ++i )
	{
		height = bOneDim ? 1 : (bCube ? m_uWidth : m_uHeight);
		width = m_uWidth;
		
		if( bMipped )
			GetMipDimensions( i, width, height, depth );

		UINT pitch;
		if( blockSize > 1 )
			pitch = (width / 4) * blockSize;
		else 
			pitch = width * uElementWidth;

		if( m_ImagePattern == IP_STRIPES && ( m_uNumStripes > width || m_uNumStripes > height ) )
			m_uNumStripes = min( width, height );
		else if( m_ImagePattern == IP_TILES && (m_uTileSize > width || m_uTileSize > height) )
			m_uTileSize = min( height, width );

		switch( m_ImagePattern )
		{
		case IP_GRADIENT:
			if( !CFormatHelper::CreateGradient( m_Format, height, width, m_ColorSets[ i % NUM_COLOR_SETS ], m_pResourceData[ i ], pitch ) )
			{
				WriteToLog( _T( "CResourceFormatsTest::CreateShaderResources() - CreateGradient() failed." ) );
				return RESULT_FAIL;
			}
			break;
		case IP_STRIPES:
			if( !CFormatHelper::CreateStripes( m_Format, height, width, m_uNumStripes, m_ColorSets[ i % NUM_COLOR_SETS ], NUM_COLORS, m_pResourceData[ i ], m_bVertStripes, pitch ) )
			{
				WriteToLog( _T( "CResourceFormatsTest::CreateShaderResources() - CreateStripes() failed." ) );
				return RESULT_FAIL;
			}
			break;
		case IP_TILES:
			if( !CFormatHelper::CreateTiles( m_Format, height, width, bOneDim ? 1 : m_uTileSize, m_ColorSets[ i % NUM_COLOR_SETS ], NUM_COLORS, m_pResourceData[ i ], pitch ) )
			{
				WriteToLog( _T( "CResourceFormatsTest::CreateShaderResources() - CreateTiles() failed." ) );
				return RESULT_FAIL;
			}
			break;
		}
	}

	viewDesc.Format = GetSRViewFormat( m_Format );
	if( CD3D10FormatHelper_Test::YUV( m_Format ) )
	{
		WriteToLog( _T("SRV format set to %s."), ToString(viewDesc.Format).c_str() );
	}
	GetViewDimension( m_ResourceScenario, &viewDesc.ViewDimension );
	
	switch( m_ResourceScenario )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		bufferDesc.BindFlags		= D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.ByteWidth		= m_uWidth * uElementWidth;
		bufferDesc.CPUAccessFlags	= 0;
		bufferDesc.MiscFlags		= 0;
		bufferDesc.Usage			= D3D11_USAGE_IMMUTABLE;

		subResData[ 0 ].pSysMem = m_pResourceData;
		subResData[ 0 ].SysMemPitch = m_uWidth * uElementWidth;
		subResData[ 0 ].SysMemSlicePitch = m_uWidth * uElementWidth;

		if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufferDesc, subResData, reinterpret_cast<ID3D11Buffer**>(&m_pTextureSR) ) ) )
		{
			WriteToLog( _T( "CResourceFormatsTest::CreateShaderResources() - CreateBuffer() failed, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		viewDesc.Buffer.ElementOffset = 0;
		viewDesc.Buffer.ElementWidth = m_uWidth;
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		tex1DDesc.ArraySize			= bArray ? FORMATS_ARRAY_SIZE : 1;
		tex1DDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		tex1DDesc.CPUAccessFlags	= 0;
		tex1DDesc.Format			= m_bUseTypeless ? GetTypelessFormat( m_Format ) : m_Format;
		tex1DDesc.MipLevels			= (!bFormatSupportsMips || bArray) ? 1 : FORMATS_MIP_LEVELS;
		tex1DDesc.MiscFlags			= 0;
		tex1DDesc.Usage				= D3D11_USAGE_IMMUTABLE;
		tex1DDesc.Width				= m_uWidth;

		for( iA = 0; iA < arraySize; ++iA )
		{
			width = m_uWidth;
			
			if( !bArray )
				GetMipDimensions( iA, width, height, depth );
			
			subResData[ iA ].pSysMem = m_pResourceData[ iA ];
			if( blockSize > 1 )
				subResData[ iA ].SysMemPitch = (width / 4) * blockSize;
			else 
				subResData[ iA ].SysMemPitch = width * uElementWidth;
			subResData[ iA ].SysMemSlicePitch = width * uElementWidth * blockSize;
		}

		if( FAILED( hr = GetDevice()->CreateTexture1D( &tex1DDesc, subResData, reinterpret_cast<ID3D11Texture1D**>(&m_pTextureSR) ) ) )
		{
			WriteToLog( _T( "CResourceFormatsTest::CreateShaderResources() - CreateTexture1D() failed, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		if( bArray )
		{
			viewDesc.Texture1DArray.MipLevels = 1;
			viewDesc.Texture1DArray.MostDetailedMip = 0;
			viewDesc.Texture1DArray.ArraySize = FORMATS_ARRAY_SIZE;
			viewDesc.Texture1DArray.FirstArraySlice = 0;
		}
		else if( !bFormatSupportsMips )
		{
			viewDesc.Texture1D.MipLevels = 1;
			viewDesc.Texture1D.MostDetailedMip = 0;
		}
		else
		{
			viewDesc.Texture1D.MipLevels = m_bOneMip ? 1 : FORMATS_MIP_LEVELS;
			viewDesc.Texture1D.MostDetailedMip = m_bOneMip ? m_uMipSlice : 0;
		}

		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		tex2DDesc.ArraySize			= bCube ? 6 : (bArray ? FORMATS_ARRAY_SIZE : 1);
		tex2DDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		tex2DDesc.CPUAccessFlags	= 0;
		tex2DDesc.Format			= m_bUseTypeless ? GetTypelessFormat( m_Format ) : m_Format;
		tex2DDesc.Height			= bCube ? m_uWidth : m_uHeight;
		tex2DDesc.MipLevels			= (!bFormatSupportsMips || bArray || bCube) ? 1 : FORMATS_MIP_LEVELS;
		tex2DDesc.MiscFlags			= bCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;
		tex2DDesc.SampleDesc.Count	= 1;
		tex2DDesc.SampleDesc.Quality= 0;
		tex2DDesc.Usage				= D3D11_USAGE_IMMUTABLE;
		tex2DDesc.Width				= m_uWidth;

		for( iA = 0; iA < (bCube ? 6 : (UINT)FORMATS_ARRAY_SIZE); ++iA )
		{
			height = bCube ? m_uWidth : m_uHeight;
			width = m_uWidth;

			if( !bArray && !bCube )
				GetMipDimensions( iA, width, height, depth );
			
			subResData[ iA ].pSysMem = m_pResourceData[ iA ];
			if( blockSize > 1 )
				subResData[ iA ].SysMemPitch = (width / 4) * blockSize;
			else
				subResData[ iA ].SysMemPitch = width * uElementWidth;
			subResData[ iA ].SysMemSlicePitch = width * height * uElementWidth ;
		}

		if( FAILED( hr = GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional( &tex2DDesc, subResData, reinterpret_cast<ID3D11Texture2D**>(&m_pTextureSR) ) ) )
		{
			WriteToLog( _T( "CResourceFormatsTest::CreateShaderResources() - CreateTexture2D() failed, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		if( bArray || bCube )
		{
			viewDesc.Texture2DArray.MipLevels = 1;
			viewDesc.Texture2DArray.MostDetailedMip = 0;
			viewDesc.Texture2DArray.ArraySize = FORMATS_ARRAY_SIZE;
			viewDesc.Texture2DArray.FirstArraySlice = 0;
		}
		else if( !bFormatSupportsMips )
		{
			viewDesc.Texture2D.MipLevels = 1;
			viewDesc.Texture2D.MostDetailedMip = 0;
		}
		else
		{
			viewDesc.Texture2D.MipLevels = m_bOneMip ? 1 : FORMATS_MIP_LEVELS;;
			viewDesc.Texture2D.MostDetailedMip = m_bOneMip ? m_uMipSlice : 0;
		}

		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:

		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		tex3DDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		tex3DDesc.CPUAccessFlags	= 0;
		tex3DDesc.Depth				= FORMATS_ARRAY_SIZE;
		tex3DDesc.Format			= m_bUseTypeless ? GetTypelessFormat( m_Format ) : m_Format;
		tex3DDesc.Height			= m_uHeight;
		tex3DDesc.MipLevels			= 1;
		tex3DDesc.MiscFlags			= 0;
		tex3DDesc.Usage				= D3D11_USAGE_IMMUTABLE;
		tex3DDesc.Width				= m_uWidth;

		height = m_uHeight;
		width = m_uWidth;
		
		subResData[ 0 ].pSysMem = m_pResourceData;
		if( blockSize > 1 )
			subResData[ iA ].SysMemPitch = (width / 4) * blockSize;
		else
			subResData[ 0 ].SysMemPitch = width * uElementWidth;
		subResData[ 0 ].SysMemSlicePitch = MAX_RESOURCE_SIZE;
		
		if( FAILED( hr = GetFramework()->CreateTexture3DMapDefaultOptional( &tex3DDesc, subResData, reinterpret_cast<ID3D11Texture3D**>(&m_pTextureSR) ) ) )
		{
			WriteToLog( _T( "CResourceFormatsTest::CreateShaderResources() - CreateTexture3D() failed, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		viewDesc.Texture3D.MipLevels = 1;
		viewDesc.Texture3D.MostDetailedMip = 0;

		break;
	}

	if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pTextureSR, &viewDesc, &m_pSRV ) ) )
	{
		WriteToLog( _T( "CResourceFormatsTest::CreateShaderResources() - CreateShaderResourceView() failed, hr = 0x%x" ), hr );
		return RESULT_FAIL;
	}

	if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER )
		iRes = 0;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D )
		iRes = 1;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY )
		iRes = 2;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D )
		iRes = 3;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY )
		iRes = 4;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D )
		iRes = 5;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURECUBE )
		iRes = 6;

	GetEffectiveContext()->PSSetShaderResources( iRes, 1, &m_pSRV );

	// If not YUV, convert to 32-bit float, used to compare against rendered result
	// If YUV, we'll compare bits later on instead
	if( !CD3D10FormatHelper_Test::YUV( m_Format ) )
	{
		for( UINT i = 0; i < arraySize; ++i )
		{
			width = m_uWidth;
			height = bOneDim ? 1 : (bCube ? m_uWidth : m_uHeight);
			depth = m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D ? m_uDepth : 1;
			if( bMipped )
				GetMipDimensions( i, width, height, depth );

			if ( !CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, m_Format, (FLOAT_COLOR*) m_pColorsTemp[ i ], height * depth, width, m_pResourceData[ i ] ) )
			{
				WriteToLog( _T("CResourceFormatsTest::CreateShaderResources() - CFormatHelper::ConvertPixels() failed") );
				return RESULT_FAIL;
			}
			memcpy( m_pResourceData[ i ], m_pColorsTemp[ i ], MAX_RESOURCE_SIZE );
		}
	}
	
	return RESULT_PASS;
}

//---------------------------------------------------------------

DXGI_FORMAT CResourceFormatsTest::GetTypelessFormat( DXGI_FORMAT format )
{
	DXGI_FORMAT typelessFmt = DXGI_FORMAT_UNKNOWN;

	switch( format )
	{
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:  
	case DXGI_FORMAT_R32G32B32A32_SINT:
		typelessFmt = DXGI_FORMAT_R32G32B32A32_TYPELESS;
		break;
	case DXGI_FORMAT_R32G32B32_FLOAT:    
	case DXGI_FORMAT_R32G32B32_UINT:     
	case DXGI_FORMAT_R32G32B32_SINT:     
		typelessFmt = DXGI_FORMAT_R32G32B32_TYPELESS;
		break;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM: 
	case DXGI_FORMAT_R16G16B16A16_UINT:  
	case DXGI_FORMAT_R16G16B16A16_SNORM: 
	case DXGI_FORMAT_R16G16B16A16_SINT:  
		typelessFmt = DXGI_FORMAT_R16G16B16A16_TYPELESS;
		break;
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:        
	case DXGI_FORMAT_R32G32_SINT:        
		typelessFmt = DXGI_FORMAT_R32G32_TYPELESS;
		break;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:    
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT: 
		typelessFmt = DXGI_FORMAT_R32G8X24_TYPELESS;
		break;
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		typelessFmt = DXGI_FORMAT_R10G10B10A2_TYPELESS;
		break;
	case DXGI_FORMAT_R11G11B10_FLOAT:
		typelessFmt = DXGI_FORMAT_R11G11B10_FLOAT;
		break;
	case DXGI_FORMAT_R8G8B8A8_UNORM:         
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:           
	case DXGI_FORMAT_R8G8B8A8_SNORM:          
	case DXGI_FORMAT_R8G8B8A8_SINT:
		typelessFmt = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		break;         
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:            
	case DXGI_FORMAT_R16G16_UINT:             
	case DXGI_FORMAT_R16G16_SNORM:            
	case DXGI_FORMAT_R16G16_SINT:
		typelessFmt = DXGI_FORMAT_R16G16_TYPELESS;
		break;           
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:             
	case DXGI_FORMAT_R32_UINT:              
	case DXGI_FORMAT_R32_SINT:
		typelessFmt = DXGI_FORMAT_R32_TYPELESS;
		break;           
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS: 
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		typelessFmt = DXGI_FORMAT_R24G8_TYPELESS;
		break;            
	case DXGI_FORMAT_R8G8_UNORM:            
	case DXGI_FORMAT_R8G8_UINT:             
	case DXGI_FORMAT_R8G8_SNORM:            
	case DXGI_FORMAT_R8G8_SINT:
		typelessFmt = DXGI_FORMAT_R8G8_TYPELESS;
		break;             
	case DXGI_FORMAT_R16_FLOAT:             
	case DXGI_FORMAT_D16_UNORM:             
	case DXGI_FORMAT_R16_UNORM:             
	case DXGI_FORMAT_R16_UINT:              
	case DXGI_FORMAT_R16_SNORM:             
	case DXGI_FORMAT_R16_SINT:
		typelessFmt = DXGI_FORMAT_R16_TYPELESS;
		break;           
	case DXGI_FORMAT_R8_UNORM:          
	case DXGI_FORMAT_R8_UINT:         
	case DXGI_FORMAT_R8_SNORM:          
	case DXGI_FORMAT_R8_SINT:
		typelessFmt = DXGI_FORMAT_R8_TYPELESS;
		break;
	case DXGI_FORMAT_A8_UNORM:
		typelessFmt = DXGI_FORMAT_A8_UNORM;
		break;
	case DXGI_FORMAT_R1_UNORM:
		typelessFmt = DXGI_FORMAT_R1_UNORM;
		break;
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		typelessFmt = DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
		break;
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
		typelessFmt = DXGI_FORMAT_R8G8_B8G8_UNORM;
		break;
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		typelessFmt = DXGI_FORMAT_G8R8_G8B8_UNORM;
		break;
	case DXGI_FORMAT_BC1_UNORM:         
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		typelessFmt = DXGI_FORMAT_BC1_TYPELESS;
		break;
	case DXGI_FORMAT_BC2_UNORM:         
	case DXGI_FORMAT_BC2_UNORM_SRGB:
		typelessFmt = DXGI_FORMAT_BC2_TYPELESS;
		break;         
	case DXGI_FORMAT_BC3_UNORM:         
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		typelessFmt = DXGI_FORMAT_BC3_TYPELESS;
		break;         
	case DXGI_FORMAT_BC4_UNORM:         
	case DXGI_FORMAT_BC4_SNORM:
		typelessFmt = DXGI_FORMAT_BC4_TYPELESS;
		break;         
	case DXGI_FORMAT_BC5_UNORM:         
	case DXGI_FORMAT_BC5_SNORM:
		typelessFmt = DXGI_FORMAT_BC5_TYPELESS;
		break;
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
		typelessFmt = DXGI_FORMAT_BC6H_TYPELESS;
		break;
	case DXGI_FORMAT_BC7_UNORM:         
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		typelessFmt = DXGI_FORMAT_BC7_TYPELESS;
		break;
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		typelessFmt = DXGI_FORMAT_B8G8R8A8_TYPELESS;
		break;
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		typelessFmt = DXGI_FORMAT_B8G8R8X8_TYPELESS;
		break;
	
	// These formats dont have a typeless
	case DXGI_FORMAT_B4G4R4A4_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_AYUV:
	case DXGI_FORMAT_Y416:
	case DXGI_FORMAT_NV12:
	case DXGI_FORMAT_YUY2:
	case DXGI_FORMAT_AI44:
	case DXGI_FORMAT_IA44:
	case DXGI_FORMAT_P8:
	case DXGI_FORMAT_A8P8:
	case DXGI_FORMAT_420_OPAQUE:
	case DXGI_FORMAT_Y410:
	case DXGI_FORMAT_P010:
	case DXGI_FORMAT_P016:
	case DXGI_FORMAT_Y210:
	case DXGI_FORMAT_Y216:
	case DXGI_FORMAT_NV11:
		typelessFmt = format;
		break;
	}
	
	return typelessFmt;
}

//---------------------------------------------------------------

DXGI_FORMAT CResourceFormatsTest::GetRTViewFormat( const DXGI_FORMAT &resourceFormat )
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

//---------------------------------------------------------------

DXGI_FORMAT CResourceFormatsTest::GetSRViewFormat( const DXGI_FORMAT &resourceFormat )
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

//---------------------------------------------------------------

bool CResourceFormatsTest::CreateRenderTargets()
{
	UINT scenFlags = 0;
	GetScenarioFlags( m_ResourceScenario, &scenFlags );
	bool bOneDim = m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER || (scenFlags & D3D_RS_FLAGS_TEXTURE1D);
	bool bArray = (scenFlags & D3D_RS_FLAGS_ARRAY) != 0;
	bool bCube = (scenFlags & D3D_RS_FLAGS_TEXTURECUBE) != 0;
	bool bMipped = !bArray && !bCube && m_ResourceScenario != D3D_RESOURCE_SCENARIO_BUFFER && m_ResourceScenario != D3D_RESOURCE_SCENARIO_TEXTURE3D;

	UINT width = m_uWidth;
	UINT height = (bOneDim ? 1 : m_uHeight);
	UINT depth = 1;
	if( bMipped )
		GetMipDimensions( m_uMipSlice, width, height, depth );
	
	D3D11_TEXTURE2D_DESC desc;
	desc.ArraySize				= 1;
	desc.BindFlags				= D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags			= 0;
	desc.Format					= m_FormatRT;
	desc.Height					= bCube ? m_uWidth : height;
	desc.Width					= width;
	desc.MipLevels				= 1;
	desc.MiscFlags				= 0;
	desc.SampleDesc.Count		= 1;
	desc.SampleDesc.Quality		= 0;
	desc.Usage					= D3D11_USAGE_DEFAULT;

	HRESULT hr;
	if( FAILED( hr = GetFramework()->CreateTexture2DMapDefaultOptional( &desc, NULL, &m_pTextureRT ) ) )
	{
		WriteToLog( _T( "CResourceFormatsTest::CreateRenderTargets() - Failed to create render target, hr = 0x%x" ), hr );
		return false;
	}

	desc.CPUAccessFlags			= D3D11_CPU_ACCESS_READ;
	desc.BindFlags				= 0;
	desc.Usage					= D3D11_USAGE_STAGING;

	if( FAILED( hr = GetFramework()->CreateTexture2DMapDefaultOptional( &desc, NULL, &m_pTextureRTCopy ) ) )
	{
		WriteToLog( _T( "CResourceFormatsTest::CreateRenderTargets() - Failed to create render target copy, hr = 0x%x" ), hr );
		return false;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format				= GetRTViewFormat( m_FormatRT );
	rtDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice	= 0;
	if( CD3D10FormatHelper_Test::YUV( m_FormatRT ) )
	{
		WriteToLog( _T("RTV format set to %s."), ToString(rtDesc.Format).c_str() );
	}

	if( FAILED( hr = GetDevice( )->CreateRenderTargetView( m_pTextureRT, &rtDesc, &m_pRTView ) ) )
	{
		WriteToLog( _T( "CResourceFormatsTest::CreateRenderTargets() - Failed to create render target view, hr = 0x%x" ), hr );		
		return false;
	}

	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, NULL );
	
	return true;
}

//---------------------------------------------------------------

TEST_RESULT CResourceFormatsTest::ExecuteTestCase( )
{
	TEST_RESULT res = RESULT_PASS;
	HRESULT hr;
	float fColors [ ] = { 0.f, 0.f, 0.f, 0.f };
	GetEffectiveContext()->ClearRenderTargetView( m_pRTView, fColors );
	GetEffectiveContext()->Draw( 1, 0 );

	UINT uScenFlags = 0;
	GetScenarioFlags( m_ResourceScenario, &uScenFlags );
	bool bCube = (uScenFlags & D3D_RS_FLAGS_TEXTURECUBE ) != 0;
	bool bOneDim = (uScenFlags & D3D_RS_FLAGS_TEXTURE1D) || (uScenFlags & D3D_RS_FLAGS_BUFFER);
	bool bArray = (uScenFlags & D3D_RS_FLAGS_ARRAY) != 0;

	D3D11_MAPPED_SUBRESOURCE texMap;
	
	GetEffectiveContext()->CopyResource( m_pTextureRTCopy, m_pTextureRT );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
		return RESULT_FAIL;
	}

	UINT subRes = bArray ? m_uArraySlice : (m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE3D ? m_uDepth : m_uMipSlice);
	if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER )
		subRes = 0;

	if( FAILED( hr = GetImmediateContext( )->Map( m_pTextureRTCopy, 0, D3D11_MAP_READ, 0, &texMap ) ) )
	{
		WriteToLog( _T( "CResourceFormatsTest::ExecuteTestCase() - Map() failed, hr = %x" ), hr );
		return RESULT_FAIL;
	}

	if( !CompareResults( subRes, texMap.pData, texMap.RowPitch ) )
		res = RESULT_FAIL;

	GetImmediateContext( )->Unmap( m_pTextureRTCopy, 0 );

	return res;
}

//---------------------------------------------------------------

bool CResourceFormatsTest::CompareResults( UINT uSlice, void *pOutput, UINT uPitch )
{
	UINT uScenFlags = 0;
	GetScenarioFlags( m_ResourceScenario, &uScenFlags );
	bool bCube = (uScenFlags & D3D_RS_FLAGS_TEXTURECUBE ) != 0;
	bool bOneDim = (uScenFlags & D3D_RS_FLAGS_TEXTURE1D) || (uScenFlags & D3D_RS_FLAGS_BUFFER);
	bool bArray = (uScenFlags & D3D_RS_FLAGS_ARRAY) != 0;
	bool bMipped = !bArray && !bCube && m_ResourceScenario != D3D_RESOURCE_SCENARIO_BUFFER && m_ResourceScenario != D3D_RESOURCE_SCENARIO_TEXTURE3D;
	const DXGI_FORMAT srvFormat = GetSRViewFormat(m_Format);
	D3D10_FORMAT_COMPONENT_INTERPRETATION fci = CD3D10FormatHelper_Test::GetFormatComponentInterpretation( srvFormat, 0 );
	UINT numComponents = CD3D10FormatHelper_Test::GetNumComponentsInFormat( srvFormat );
	UINT bytesPerElementRT = CD3D10FormatHelper_Test::GetBitsPerElement( m_FormatRT ) / 8;
	D3D10_FORMAT_COMPONENT_NAME pComponentNames[ 4 ];
	D3D10_FORMAT_COMPONENT_NAME pFormatComponents[ 4 ];
	for( UINT i = 0; i < numComponents; ++i )
		pFormatComponents[ i ] = CD3D10FormatHelper_Test::GetComponentName( srvFormat, i );
	pComponentNames[ 0 ] = D3D10FCN_R;
	pComponentNames[ 1 ] = D3D10FCN_G;
	pComponentNames[ 2 ] = D3D10FCN_B;
	pComponentNames[ 3 ] = D3D10FCN_A;
	
	bool bLogAll = GetFramework()->LogVerbose();
	bool bPass = true;

	UINT width = m_uWidth;
	UINT height = (bOneDim ? 1 : bCube ? m_uWidth : m_uHeight);
	UINT depth = 1;
	if( bMipped )
		GetMipDimensions( uSlice, width, height, depth );

	if( CD3D10FormatHelper_Test::YUV( m_Format ) )
	{
		// If YUV, we have a view where each component maps to a channel
		// We're going to compare bits rather than converting each to its FLOAT_COLOR equivalent

		BYTE *pRow = (BYTE*) pOutput;
		BYTE *pCurrentExp = (BYTE*) m_pResourceData[ uSlice ];

		for( UINT y = 0; y < height; ++y )
		{
			UINT32 *pCurrentOut = (UINT32*) pRow; // RTV is R32G32B32A32
			for( UINT x = 0; x < width && (bPass || bLogAll); ++x )
			{
				if ( (m_Format == DXGI_FORMAT_YUY2 || m_Format == DXGI_FORMAT_Y210 || m_Format == DXGI_FORMAT_Y216 ) && x >= width/2 )
				{
					// 2:1 horizontally downsampled, go to next row
					break;
				}

				if ( m_Format == DXGI_FORMAT_Y410 )
				{
					UINT32 elementExp = *(UINT32*)pCurrentExp;

					// Component 0 (U)
					if ( (elementExp & 0x3FF) != pCurrentOut[0] )
					{
							WriteToLog( _T( "CResourceFormatsTest::CompareResults() - Component %d failed at location (%d,%d)\n\t\tExpected: %d\n\t\tOutput: %d" ), 0, x, y, elementExp & 0x3FF, pCurrentOut[0] );
							bPass = false;
					}

					// Component 1 (Y)
					if ( (elementExp >> 10 & 0x3FF) != pCurrentOut[1] )
					{
							WriteToLog( _T( "CResourceFormatsTest::CompareResults() - Component %d failed at location (%d,%d)\n\t\tExpected: %d\n\t\tOutput: %d" ), 1, x, y, elementExp >> 10 & 0x3FF, pCurrentOut[1] );
							bPass = false;
					}

					// Component 2 (V)
					if ( (elementExp >> 20 & 0x3FF) != pCurrentOut[2] )
					{
							WriteToLog( _T( "CResourceFormatsTest::CompareResults() - Component %d failed at location (%d,%d)\n\t\tExpected: %d\n\t\tOutput: %d" ), 2, x, y, elementExp >> 20 & 0x3FF, pCurrentOut[2] );
							bPass = false;
					}

					// Component 3 (A)
					if ( (elementExp >> 30) != pCurrentOut[3] )
					{
							WriteToLog( _T( "CResourceFormatsTest::CompareResults() - Component %d failed at location (%d,%d)\n\t\tExpected: %d\n\t\tOutput: %d" ), 3, x, y, elementExp >> 30, pCurrentOut[3] );
							bPass = false;
					}
					
					pCurrentExp += 4;

				}
				else if ( m_Format == DXGI_FORMAT_Y416 || m_Format == DXGI_FORMAT_Y210 || m_Format == DXGI_FORMAT_Y216 )
				{
					WORD *pElementExp = (WORD*)pCurrentExp;

					for( UINT c = 0; c < numComponents; ++c )
					{
						if (pCurrentOut[c] != pElementExp[c])
						{
							WriteToLog( _T( "CResourceFormatsTest::CompareResults() - Component %d failed at location (%d,%d)\n\t\tExpected: %d\n\t\tOutput: %d" ), c, x, y, pElementExp[c], pCurrentOut[c] );
							bPass = false;
						}
					}

					pCurrentExp += 8;				
				}
				else if ( m_Format == DXGI_FORMAT_P010 || m_Format == DXGI_FORMAT_P016 )
				{
					WORD *pElementExp = (WORD*)pCurrentExp;

					for( UINT c = 0; c < numComponents; ++c )
					{
						if (pCurrentOut[c] != pElementExp[c])
						{
							WriteToLog( _T( "CResourceFormatsTest::CompareResults() - Component %d failed at location (%d,%d)\n\t\tExpected: %d\n\t\tOutput: %d" ), c, x, y, pCurrentExp[c], pCurrentOut[c] );
							bPass = false;
						}
					}
					pCurrentExp += 2;
				}
				else
				{
					for( UINT c = 0; c < numComponents; ++c )
					{
						if (pCurrentOut[c] != pCurrentExp[c])
						{
							WriteToLog( _T( "CResourceFormatsTest::CompareResults() - Component %d failed at location (%d,%d)\n\t\tExpected: %d\n\t\tOutput: %d" ), c, x, y, pCurrentExp[c], pCurrentOut[c] );
							bPass = false;
						}
					}
					pCurrentExp += numComponents;
				}
				
				pCurrentOut += bytesPerElementRT / 4;
				
			}
			// Go to next row
			pRow += uPitch;
		}
	}
	else
	{
		FLOAT_COLOR *pCurrentOut = (FLOAT_COLOR*) pOutput;
		FLOAT_COLOR *pCurrentExp = (FLOAT_COLOR*) m_pResourceData[ uSlice ];
	
		const DXGIFormatInfo *pFmtInfo = CFormatInfoMap::Find( MinMaxRange::GetFormatEquivalentBits(srvFormat) );
		for( UINT y = 0; y < height; ++y )
		{
			for( UINT x = 0; x < width && bPass; ++x )
			{
				FLOAT_COLOR output;
				output = pCurrentOut[ x ];
				for( UINT c = 0; c < 4; ++c )
				{
					bool bHasComponent = false;
					for( UINT i = 0; i < numComponents; ++i )
						if( pFormatComponents[ i ] == pComponentNames[ c ] )
							bHasComponent = true;
					if( !bHasComponent )
						continue;
				
					UINT bits = CD3D10FormatHelper_Test::GetBitsPerComponent( srvFormat, c );

					float fUlp = 0.0f;

					// The decode process for some block compressed formats allows for a wide range of results
					// fMaxBCError contains the maximum absolute error (in normalized floating point) for the format
					// For most formats, this is 0.0, in which case the spec defines the tolerance for the general conversion (UNORM->Float, SNORM->Float, etc)
					float fMaxBCError = CFormatHelper::GetBCFormatMaxTolerance( srvFormat );

					if( fMaxBCError != 0.0f )
					{
						// This code path is used for block-compressed formats with a tolerance
						UINT uncompressedBits = pFmtInfo->pBitsPerComponent[c];
						float range = (float)(pFmtInfo->dwFlags&FF_SNORM ? 1 << (uncompressedBits-1) : 1 << uncompressedBits);

						// Determine the range of allowed normalized floating point values
						// the range is [fMinVal, fMaxVal]
						float fMinVal;
						float fMaxVal;

						fMinVal = output.comp[ c ] - fMaxBCError;
						fMaxVal = output.comp[ c ] + fMaxBCError;

						if( fci == D3D10FCI_UNORM_SRGB )
						{
							// For BC*_UNORM formats, there are 2 potential sources of allowed variation
							// 1. The error allowed in the decoding process (captured by fMaxBCError)
							// 2. The error allowed in the UNORM->SRGB conversion
							// These two factors are combined to produce a single range of allowable values [fMinVal, fMaxVal]
							// 
							// At this point, the range of results allowed by factor 1 has already been computed: [fMinVal, fMaxVal]
							// The spec'd way to measure the error for factor 2 is to convert the result from float to SRGB, using the ideal conversion
							// without rounding to integer.  The tolerance is then 0.5 ULP.  In SRGB space the "unit in the last place" is 1/range (1/255)
							// So [fMinVal, fMaxVal] is transformed into
							// [FloatToSRGB(fMinVal) - 0.5/255, FloatToSRGB(fMaxVal) + 0.5/255]
							// Note that the FloatToSRGB conversion returns a result in [0,1]

							pCurrentExp[ y * width + x ].comp[ c ] = FloatToSRGB( pCurrentExp[ y * width + x ].comp[ c ], uncompressedBits );
							fMinVal = FloatToSRGB( fMinVal, uncompressedBits ) - (0.5f / range);
							fMaxVal = FloatToSRGB( fMaxVal, uncompressedBits ) + (0.5f / range);
						}

						// At this point, the range of allowable values is known [fMinVal, fMaxVal]
						// Because the test uses MinMaxRange::TestFloatsSame for comparision
						// this must be converted into an exact value (output.comp[ c ]) and a allowable ulp error (fUlp)
						// The "unit in the the last place" used by MinMaxRange::TestFloatsSame is (range)

						assert( fMaxVal >= fMinVal );

						float fAverage = (fMinVal + fMaxVal) * 0.5f;

						assert( fMaxVal >= fAverage );
						assert( fMinVal <= fAverage );

						float fMaxError = max( fMaxVal - fAverage, fAverage - fMinVal );

						fUlp = range * fMaxError;

						output.comp[ c ] = fAverage;
					}
					else
					{
						fUlp = 1.0f;

						if( fci == D3D10FCI_UNORM_SRGB )
						{
							pCurrentExp[ y * width + x ].comp[ c ] = FloatToSRGB( pCurrentExp[ y * width + x ].comp[ c ], bits );
							output.comp[ c ] = FloatToSRGB( output.comp[ c ], bits );
							fUlp = 0.5f;
						}
					}
				
					if( (bPass || bLogAll) && bHasComponent )
					{
						float ulpDiff;
						if( ( fci != D3D10FCI_SINT && fci != D3D10FCI_UINT ) &&
							( !MinMaxRange::TestFloatsSame( pCurrentExp[ y * width + x ].comp[ c ], output.comp[ c ], pFmtInfo, c, fUlp, &ulpDiff ) && !( IsDenorm( *( (UINT32*) &output.comp[ c ] ), bits ) && pCurrentExp[ y * width + x ].comp[ c ] == 0.f ) ) )
						{
							WriteToLog( _T( "CResourceFormatsTest::CompareResults() - Component %d failed at location (%d,%d)\n\t\tExpected: %f\n\t\tOutput: %f" ), c, x, y, pCurrentExp[ y * width + x ].comp[ c ], output.comp[ c ] );
							bPass = false;
						}
					}
				}
			}
			pCurrentOut += (uPitch / sizeof(FLOAT_COLOR));
		}
	}

	return bPass;
}

float CResourceFormatsTest::FloatToSRGB( float fSRGBVal, int nBits )
{
	float fFixed;
	if( IsNaN( fSRGBVal ) || fSRGBVal < 0.f )
		fFixed = 0.f;
	else if( fSRGBVal > 1.f )
		fFixed = 1.f;
	else
		fFixed = fSRGBVal;

	if( fFixed <= 0.0031308f )
		fFixed = 12.92f * fFixed;
	else
		fFixed = 1.055f * powf( fFixed, 1.f / 2.4f ) - 0.055f;
	fFixed = fFixed * ((1 << nBits) - 1);
	fFixed /= 255.f;

	return fFixed;
}

//---------------------------------------------------------------

bool CResourceFormatsTest::WithinOneULP( float fReference, float fOutput )
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

bool CResourceFormatsTest::IsDenorm( UINT32 uValue, UINT32 uBits )
{
	UINT32 frac, exp;
	if( uBits == 16 )
	{
		exp = 0x00007c00;
		frac = 0x000003ff;
	}
	else if( uBits == 11 )
	{
		exp = 0x000007c0;
		frac = 0x0000003f;
	}
	else if( uBits == 10 )
	{
		exp = 0x000003e0;
		frac = 0x0000001f;
	}
	else
	{
		exp = 0x7f800000;
		frac = 0x007fffff;
	}

	return ((uValue & exp) == 0) && ((uValue & frac) != 0);
}

//---------------------------------------------------------------

void CResourceFormatsTest::CleanupTestCase( )
{
	CD3D11Test::CleanupTestCase();

	ID3D11ShaderResourceView *pNullSRVs [ ] = { NULL, NULL, NULL };
	ID3D11RenderTargetView *pNullRTVs [ ] = { NULL, NULL, NULL };

	GetEffectiveContext()->PSSetShaderResources( 0, 3, pNullSRVs );
	GetEffectiveContext()->OMSetRenderTargets( 3, pNullRTVs, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
	}

	SAFE_RELEASE( m_pTextureSR );
	SAFE_RELEASE( m_pTextureRT );
	SAFE_RELEASE( m_pSRV );
	SAFE_RELEASE( m_pRTView );
	SAFE_RELEASE( m_pTextureRTCopy );

	GetImmediateContext()->Flush();
}

bool CResourceFormatsTest::GetMipDimensions( UINT uMipSlice, UINT &uWidth, UINT &uHeight, UINT &uDepth )
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

bool CResourceFormatsTest::IsNaN( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;

	return ((uValue & exp) == 0x7f800000) && ((uValue & frac) != 0);
}

bool CResourceFormatsTest::IsDenorm( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;

	return ((uValue & exp) == 0) && ((uValue & frac) != 0);
}

//---------------------------------------------------------------

/******************************
* CDepthReadbackTest methods
******************************/

void CDepthReadbackTest::InitTestParameters()
{
	CTestCaseParameter<D3D_RESOURCE_SCENARIO> *pResParam = AddParameter( _T( "ResourceScenario" ), &m_ResourceScenario );
	testfactor::RFactor rfTex1D = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE1D );
	testfactor::RFactor rfTex2D = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D );
	testfactor::RFactor rfTexCube = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURECUBE );

	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T( "Format"), &m_Format );
	CUserValueSet<DXGI_FORMAT> *pDepthFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	pDepthFormatValues->AddValue( DXGI_FORMAT_D32_FLOAT );
	pDepthFormatValues->AddValue( DXGI_FORMAT_D32_FLOAT_S8X24_UINT );
	pDepthFormatValues->AddValue( DXGI_FORMAT_D24_UNORM_S8_UINT );
	pDepthFormatValues->AddValue( DXGI_FORMAT_D16_UNORM );
	testfactor::RFactor rfDepthFormats = AddParameterValueSet( pFormatParam, pDepthFormatValues );

	SetRootTestFactor( 
		rfTex1D * rfDepthFormats +
		rfTex2D * rfDepthFormats
	);
}

TEST_RESULT CDepthReadbackTest::SetupTestCase()
{
	HRESULT hr = S_OK;
	m_pDSV = NULL;
	m_FormatRT = FORMAT_RT_FLOAT;

	D3D11_TEXTURE1D_DESC tex1DDesc;
	D3D11_TEXTURE2D_DESC tex2DDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	bool bCube = m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURECUBE;

	srvDesc.Format = GetRGBFormat( m_Format );
	dsvDesc.Format = m_Format;
	dsvDesc.Flags = 0;
	
	switch( m_ResourceScenario )
	{
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		tex1DDesc.ArraySize = 1;
		tex1DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		tex1DDesc.CPUAccessFlags = 0;
		tex1DDesc.Format = GetTypelessFormat( m_Format );
		tex1DDesc.MipLevels = 1;
		tex1DDesc.MiscFlags = 0;
		tex1DDesc.Usage = D3D11_USAGE_DEFAULT;
		tex1DDesc.Width = TEXTURE_SIZE;

		if( FAILED( hr = GetDevice()->CreateTexture1D( &tex1DDesc, NULL, reinterpret_cast<ID3D11Texture1D**>(&m_pTextureSR) ) ) )
		{
			WriteToLog( _T( "CDepthFormatTest::SetupTestCase() - Failed to create 1D texture for SRV, hr = 0x%s" ), D3DHResultToString( hr ).c_str() );
			return RESULT_FAIL;
		}

		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		srvDesc.Texture1D.MipLevels = 1;
		srvDesc.Texture1D.MostDetailedMip = 0;

		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
		dsvDesc.Texture1D.MipSlice = 0;

		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		tex2DDesc.ArraySize = bCube ? 6 : 1;
		tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		tex2DDesc.CPUAccessFlags = 0;
		tex2DDesc.Format = GetTypelessFormat( m_Format );
		tex2DDesc.MipLevels = 1;
		tex2DDesc.MiscFlags = bCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;
		tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
		tex2DDesc.Width = TEXTURE_SIZE;
		tex2DDesc.Height = TEXTURE_SIZE;
		tex2DDesc.SampleDesc.Count = 1;
		tex2DDesc.SampleDesc.Quality = 0;

		if( FAILED( hr = GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional( &tex2DDesc, NULL, reinterpret_cast<ID3D11Texture2D**>(&m_pTextureSR) ) ) )
		{
			WriteToLog( _T( "CDepthFormatTest::SetupTestCase() - Failed to create 2D texture for SRV, hr = 0x%s" ), D3DHResultToString( hr ).c_str() );
			return RESULT_FAIL;
		}

		//if( bCube )
		//{
		//	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		//	srvDesc.TextureCube.MipLevels = 1;
		//	srvDesc.TextureCube.MostDetailedMip = 0;
		//}
		//else
		//{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;
		//}

		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		break;

	default:
		return RESULT_FAIL;
	}

	if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pTextureSR, &srvDesc, &m_pSRV ) ) )
	{
		WriteToLog( _T( "CDepthFormatTest::SetupTestCase() - Failed to create shader resource view, hr = 0x%s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	if( FAILED( hr = GetDevice()->CreateDepthStencilView( m_pTextureSR, &dsvDesc, &m_pDSV ) ) )
	{
		WriteToLog( _T( "CDepthFormatTest::SetupTestCase() - Failed to create depth stencil view, hr = 0x%s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	UINT scenFlags = 0;
	GetScenarioFlags( m_ResourceScenario, &scenFlags );
	bool bOneDim = m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER || (scenFlags & D3D_RS_FLAGS_TEXTURE1D);
	
	UINT width = TEXTURE_SIZE;
	UINT height = (bOneDim ? 1 : TEXTURE_SIZE);
	
	D3D11_TEXTURE2D_DESC desc;
	desc.ArraySize				= 1;
	desc.BindFlags				= D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags			= 0;
	desc.Format					= m_FormatRT;
	desc.Height					= bCube ? width : height;
	desc.Width					= width;
	desc.MipLevels				= 1;
	desc.MiscFlags				= 0;
	desc.SampleDesc.Count		= 1;
	desc.SampleDesc.Quality		= 0;
	desc.Usage					= D3D11_USAGE_DEFAULT;

	if( FAILED( hr = GetFramework()->CreateTexture2DMapDefaultOptional( &desc, NULL, &m_pTextureRT ) ) )
	{
		WriteToLog( _T( "CDepthFormatTest::SetupTestCase() - Failed to create render target, hr = 0x%x" ), hr );
		return RESULT_FAIL;
	}

	desc.CPUAccessFlags			= D3D11_CPU_ACCESS_READ;
	desc.BindFlags				= 0;
	desc.Usage					= D3D11_USAGE_STAGING;

	if( FAILED( hr = GetFramework()->CreateTexture2DMapDefaultOptional( &desc, NULL, &m_pTextureRTCopy ) ) )
	{
		WriteToLog( _T( "CDepthFormatTest::SetupTestCase() - Failed to create render target copy, hr = 0x%x" ), hr );
		return RESULT_FAIL;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format				= GetRTViewFormat( m_FormatRT );
	rtDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice	= 0;
	WriteToLog( _T("RTV format set to %s."), ToString(rtDesc.Format).c_str() );

	if( FAILED( hr = GetDevice( )->CreateRenderTargetView( m_pTextureRT, &rtDesc, &m_pRTView ) ) )
	{
		WriteToLog( _T( "CDepthFormatTest::SetupTestCase() - Failed to create render target view, hr = 0x%x" ), hr );		
		return RESULT_FAIL;
	}

	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, NULL );

	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width    = (float) width;
	m_Viewport.Height   = (float) height;
	m_Viewport.MinDepth = 0;
	m_Viewport.MaxDepth = 1;
	GetImmediateContext()->RSSetViewports(1, &m_Viewport);

	// pass current width and height to shader
	D3D11_MAPPED_SUBRESOURCE pData;
	if( FAILED( hr = GetImmediateContext( )->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData ) ) )
	{
		WriteToLog( _T( "CRenderTargetsTest::SetupTestCase() - failed to map const buffer, hr = 0x%x" ), hr );
		return RESULT_FAIL;
	}

	D3DXMatrixIdentity( &m_mxTrans );
	float transx = (float) TEXTURE_SIZE / 2.f;
	float transy = (float) height / 2.f;

	// Inverse viewport scale.
	m_mxTrans._13 = 1.f / transx;
	m_mxTrans._14 = -1.f / transy;

	// Prescaled inverse viewport translation.
	m_mxTrans._11 = -transx * m_mxTrans._13;
	m_mxTrans._12 = -transy * m_mxTrans._14;

	SIZE_T offset = sizeof( float ) * 4;
	memcpy( pData.pData, &m_mxTrans, offset );

	float dims[ 2 ] = { (float) TEXTURE_SIZE, (float) height };
	memcpy( ((BYTE*)pData.pData) + offset, dims, sizeof(float) * 2 );
	offset += sizeof(float) * 2;

	int iRes = 0;
	if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D )
		iRes = 1;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D )
		iRes = 3;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURECUBE )
		iRes = 3;

	memcpy( ((BYTE*)pData.pData) + offset, &iRes, sizeof(int) );
	offset += sizeof( int );
	iRes = 0;
	memcpy( ((BYTE*)pData.pData) + offset, &iRes, sizeof(int) );

	GetImmediateContext()->Unmap( m_pConstBuffer, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

void CDepthReadbackTest::CleanupTestCase()
{
	CD3D11Test::CleanupTestCase();

	ID3D11ShaderResourceView *pNullSRVs [ ] = { NULL, NULL, NULL };
	ID3D11RenderTargetView *pNullRTVs [ ] = { NULL, NULL, NULL };
	ID3D11DepthStencilView *pNullDSV = NULL;

	GetEffectiveContext()->PSSetShaderResources( 0, 3, pNullSRVs );
	GetEffectiveContext()->OMSetRenderTargets( 3, pNullRTVs, pNullDSV );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
	}

	SAFE_RELEASE( m_pTextureSR );
	SAFE_RELEASE( m_pTextureRT );
	SAFE_RELEASE( m_pSRV );
	SAFE_RELEASE( m_pRTView );
	SAFE_RELEASE( m_pDSV );
	SAFE_RELEASE( m_pTextureRTCopy );

	GetImmediateContext()->Flush();
}

TEST_RESULT CDepthReadbackTest::ExecuteTestCase()
{
	bool bOneDim = m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D;
	UINT uHeight = bOneDim ? 1 : TEXTURE_SIZE;

	// for roundtrip conversion
	FLOAT_COLOR pDepthData[ TEXTURE_SIZE * TEXTURE_SIZE ];
	for( UINT i = 0; i < TEXTURE_SIZE * TEXTURE_SIZE; ++i )
		pDepthData[ i ].r = (float) ( i % 16 ) / 100.f;
	// for DepthHelper
	float pDepthDataF[ TEXTURE_SIZE * TEXTURE_SIZE ];
	for( UINT i = 0; i < TEXTURE_SIZE * TEXTURE_SIZE; ++i )
		pDepthDataF[ i ] = pDepthData[ i ].r;

	DepthHelper dh;
	dh.Setup( GetFramework(), m_pDSV );
	dh.Fill( pDepthDataF, TEXTURE_SIZE, uHeight );
	dh.Cleanup();

	// roundtrip conversion
	if( m_Format == DXGI_FORMAT_D24_UNORM_S8_UINT || m_Format == DXGI_FORMAT_D16_UNORM )
	{
		BYTE tempData[ TEXTURE_SIZE * TEXTURE_SIZE * sizeof( float ) ];
		if ( !CFormatHelper::ConvertPixels( CONV_TO_FORMAT, GetRGBFormat( m_Format ), pDepthData, TEXTURE_SIZE, TEXTURE_SIZE, tempData ) )
		{
			WriteToLog(_T("CDepthReadbackTest::ExecuteTestCase() - CFormatHelper::ConvertPixels() failed"));
			return RESULT_FAIL;
		}
		if ( !CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, GetRGBFormat( m_Format ), pDepthData, TEXTURE_SIZE, TEXTURE_SIZE, tempData ) )
		{
			WriteToLog(_T("CDepthReadbackTest::ExecuteTestCase() - CFormatHelper::ConvertPixels() failed"));
			return RESULT_FAIL;
		}
	}
	
	// restore states modified by DepthHelper
	UINT StridesTL = sizeof( TLVERTEX );
	UINT Offset = 0;
	GetEffectiveContext()->IASetInputLayout( m_pLayout );
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &StridesTL, &Offset );
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	GetEffectiveContext()->RSSetState( m_pRSEnabled );
	GetEffectiveContext()->RSSetViewports( 1, &m_Viewport );
	GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );
	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, NULL );

	UINT iRes = 0;
	if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE1D )
		iRes = 1;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURE2D )
		iRes = 3;
	else if( m_ResourceScenario == D3D_RESOURCE_SCENARIO_TEXTURECUBE )
		iRes = 3;
	
	GetEffectiveContext()->PSSetShaderResources( iRes, 1, &m_pSRV );
	GetEffectiveContext()->Draw( 1, 0 );

	GetEffectiveContext()->CopyResource( m_pTextureRTCopy, m_pTextureRT );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("CDepthReadbackTest::ExecuteTestCase() - ExecuteEffectiveContext() failed"));
		return RESULT_FAIL;
	}

	D3D11_MAPPED_SUBRESOURCE texMap;
	HRESULT hr = GetImmediateContext( )->Map( m_pTextureRTCopy, 0, D3D11_MAP_READ, 0, &texMap );

	if ( FAILED(hr) )
	{
		WriteToLog( _T("CDepthReadbackTest::ExecuteTestCase() - Map() failed, hr = %s"), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	BYTE *pCurrentRes = (BYTE*) texMap.pData;
	const DXGIFormatInfo *pFmtInfo = CFormatInfoMap::Find( MinMaxRange::GetFormatEquivalentBits(m_Format) );
	float ulpDiff;
	for( UINT y = 0; y < uHeight; ++y )
	{
		for( UINT x = 0; x < TEXTURE_SIZE; ++x )
		{
			if( !MinMaxRange::TestFloatsSame( pDepthData[ y * TEXTURE_SIZE + x ].r, *((float*)pCurrentRes), pFmtInfo, 0, 1.1f, &ulpDiff ) )
			{
				WriteToLog( _T("Test case failed at (%d,%d), expected: %.9f actual: %.9f"), x, y, pDepthData[ y * TEXTURE_SIZE + x ].r, *((float*)pCurrentRes)  );
				GetImmediateContext( )->Unmap( m_pTextureRTCopy, 0 );
				return RESULT_FAIL;
			}
			pCurrentRes += max( GetBitsPerElement( m_FormatRT ) / 8, 1 );
		}
		pCurrentRes = (BYTE*) texMap.pData + ( (y+1) * texMap.RowPitch );
	}

	GetImmediateContext( )->Unmap( m_pTextureRTCopy, 0 );

	return RESULT_PASS;
}

DXGI_FORMAT	CDepthReadbackTest::GetRGBFormat( DXGI_FORMAT depthFormat )
{
	DXGI_FORMAT rgbFormat;

	switch( depthFormat )
	{
	case DXGI_FORMAT_D32_FLOAT:
		rgbFormat = DXGI_FORMAT_R32_FLOAT;
		break;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		rgbFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		break;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		rgbFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		break;
	case DXGI_FORMAT_D16_UNORM:
		rgbFormat = DXGI_FORMAT_R16_UNORM;
		break;
	}

	return rgbFormat;
}

//---------------------------------------------------------------


/******************************
 * CResourceFormatsTestApp methods
 ******************************/

void CResourceFormatsTestApp::InitOptionVariables( )
{
	int n = AddOptionCategory( _T( "Debug" ) );

	return CD3D11TestFramework::InitOptionVariables();
}

//---------------------------------------------------------------

bool CResourceFormatsTestApp::InitTestGroups( )
{
	if( !ADD_TEST( _T( "Formats" ), CResourceFormatsTest ) ) return false;
	if( !ADD_TEST( _T( "DepthReadback" ), CDepthReadbackTest ) ) return false;

	if (!ADD_TEST( _T("CheckFormatSupport"), CCheckFormatSupportTest ) ) return false;

	return true;
}
