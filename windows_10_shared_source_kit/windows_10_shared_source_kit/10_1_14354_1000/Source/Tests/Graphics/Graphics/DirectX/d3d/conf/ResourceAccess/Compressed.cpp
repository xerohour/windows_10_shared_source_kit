#include "Compressed.h"

void CCopyCompressedTest::InitTestParameters()
{
	CTestCaseParameter<UINT> *pArrayParam = AddParameter( _T( "ArraySize" ), &m_uArraySize );
	SetParameterDefaultValue<UINT>(pArrayParam, 1);
	CUserValueSet<UINT> *pArrayValues = new CUserValueSet<UINT>( );
	pArrayValues->AddValue( 1 );
	pArrayValues->AddValue( 10 );
	pArrayValues->AddValue( 512 );
	testfactor::RFactor rfArraySize = AddParameterValueSet( pArrayParam, pArrayValues );

	CTestCaseParameter<UINT32> *pWidthParam = AddParameter( _T( "Width" ), &m_uWidth );
	CUserValueSet<UINT32> *pWidthValues = new CUserValueSet<UINT32>( );
	pWidthValues->AddValue( 32 );
	pWidthValues->AddValue( 16 );
	pWidthValues->AddValue( 10 );
	testfactor::RFactor rfWidth = AddParameterValueSet( pWidthParam, pWidthValues );

	CTestCaseParameter<UINT32> *pHeightParam = AddParameter( _T( "Height" ), &m_uHeight );
	CUserValueSet<UINT32> *pHeightValues = new CUserValueSet<UINT32>( );
	pHeightValues->AddValue( 32 );
	pHeightValues->AddValue( 64 );
	pHeightValues->AddValue( 10 );
	testfactor::RFactor rfHeight = AddParameterValueSet( pHeightParam, pHeightValues );

	CTestCaseParameter<UINT32> *pDepthParam = AddParameter( _T( "Depth" ), &m_uDepth );
	CUserValueSet<UINT32> *pDepthValues = new CUserValueSet<UINT32>( );
	//pDepthValues->AddValue( 10 );
	//pDepthValues->AddValue( 5 );
	//pDepthValues->AddValue( 2 );
	pDepthValues->AddValue( 1 );
	testfactor::RFactor rfDepth = AddParameterValueSet( pDepthParam, pDepthValues );

	CTestCaseParameter<D3D_RESOURCE_SCENARIO> *pResParam = AddParameter( _T("ResourceScenario"), &m_ResourceScenario );
	testfactor::RFactor rfResTex2D = AddParameterValue( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D );
	testfactor::RFactor rfResTex2DArr = AddParameterValue( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY );
	testfactor::RFactor rfResTex3D = AddParameterValue( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE3D );
	testfactor::RFactor rfResTexCube = AddParameterValue( pResParam, D3D_RESOURCE_SCENARIO_TEXTURECUBE );

	CUserValueSet<DXGI_FORMAT> *pFormatSrcValues = new CUserValueSet<DXGI_FORMAT>( );
	pFormatSrcValues->AddValue( DXGI_FORMAT_R9G9B9E5_SHAREDEXP );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC1_UNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC1_UNORM_SRGB );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC2_UNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC2_UNORM_SRGB );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC3_UNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC3_UNORM_SRGB );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC4_UNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC4_SNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC5_UNORM );
	pFormatSrcValues->AddValue( DXGI_FORMAT_BC5_SNORM );

	CUserValueSet<DXGI_FORMAT> *pFormatDestValues = new CUserValueSet<DXGI_FORMAT>( );
	pFormatDestValues->AddValue( DXGI_FORMAT_R32G32B32A32_UINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R32G32B32A32_SINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R16G16B16A16_UINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R16G16B16A16_SINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R32G32_SINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R32G32_UINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R32_SINT );
	pFormatDestValues->AddValue( DXGI_FORMAT_R32_UINT );
	
	testfactor::RFactor rfFormatSrc = AddParameter( _T("SrcFormat"), &m_SrcFormat, pFormatSrcValues );
	testfactor::RFactor rfFormatDest = AddParameter( _T("DestFormat"), &m_DestFormat, pFormatDestValues );

	testfactor::RFactor rfFormatSrcSame = AddParameter( _T( "SrcFormatSame" ), &m_SrcFormatSame, pFormatSrcValues );

	SetParameterDesc( _T("ArraySize"), _T(DESC_PARAM_ARRAYSIZE) );
	SetParameterDesc( _T("Width"), _T(DESC_PARAM_WIDTH) );
	SetParameterDesc( _T("Height"), _T(DESC_PARAM_HEIGHT) );
	SetParameterDesc( _T("Depth"), _T(DESC_PARAM_DEPTH) );
	SetParameterDesc( _T("ResourceScenario"), _T(DESC_PARAM_RESOURCESCENARIO) );
	SetParameterDesc( _T("SrcFormat"), _T(DESC_PARAM_SRCFORMAT) );
	SetParameterDesc( _T("DestFormat"), _T(DESC_PARAM_DESTFORMAT) );
	SetParameterDesc( _T("SrcFormatSame"), _T(DESC_PARAM_SRCFORMATSAME) );
	
	SetRootTestFactor(
		( rfFormatSrc * rfFormatDest * rfResTex2D * rfWidth * rfHeight ) +
		( rfFormatSrc * rfFormatDest * rfResTex2DArr * rfWidth * rfHeight * rfArraySize ) +
		( rfFormatSrc * rfFormatDest * rfResTex3D * rfWidth * rfHeight * rfDepth ) +
		( rfFormatSrc * rfFormatDest * rfResTexCube * rfWidth )
	);
}

TEST_RESULT	CCopyCompressedTest::Setup()
{
	if( GetFramework()->m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_0 )
		return RESULT_SKIP;

	return CResourceAccessTest::Setup();
}

TEST_RESULT CCopyCompressedTest::SetupTestCase()
{
	if( !IsFormatCompatible( m_DestFormat, m_SrcFormat ) )
	{
		SkipMatching( FilterEqual( _T("DestFormat"), m_DestFormat ) );
		return RESULT_SKIP;
	}


	return CResourceAccessTest::SetupTestCase();
}

bool CCopyCompressedTest::IsFormatCompatible( DXGI_FORMAT structuredFmt, DXGI_FORMAT compressedFmt )
{
	if( m_Formats.find( structuredFmt ) == m_Formats.end() )
		return false;

	list<DXGI_FORMAT> compFormats = m_Formats[ structuredFmt ];
	list<DXGI_FORMAT>::const_iterator itr = compFormats.begin();
	while( itr != compFormats.end() )
	{
		DXGI_FORMAT fmt = *itr;
		if( fmt == compressedFmt )
			return true;
		itr++;
	}

	return false;
}

TEST_RESULT CCopyCompressedTest::CreateResources()
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC tex2DDesc;
	D3D11_TEXTURE3D_DESC tex3DDesc;
	D3D11_SUBRESOURCE_DATA subResData[ MAX_ARRAYSIZE ];

	bool bSameFormat = GetTestCaseParameter( _T( "SrcFormatSame" ) )->IsActive( );
	bool bCreateTwo = m_AccessMethod == AM_COPY || ( m_AccessMethod == AM_COPYREGION && !m_bSameResource );
	DXGI_FORMAT srcFormat = bSameFormat ? m_SrcFormatSame : m_SrcFormat;
	DXGI_FORMAT destFormat = bSameFormat ? m_DestFormatSame : m_DestFormat;

	UINT scenFlags = 0;
	GetScenarioFlags( m_ResourceScenario, &scenFlags );
	bool bOneDim = m_ResourceScenario == D3D_RESOURCE_SCENARIO_BUFFER || (scenFlags & D3D_RS_FLAGS_TEXTURE1D);
	bool bCube = (scenFlags & D3D_RS_FLAGS_TEXTURECUBE) != 0;
	bool bArray = (scenFlags & D3D_RS_FLAGS_ARRAY) != 0;
	bool bMS = (scenFlags & D3D_RS_FLAGS_MS) != 0;
	UINT uElementWidth = max( GetBitsPerElement( srcFormat ) / 8, 1 );
	UINT height = bCube ? m_uWidth : m_uHeight;

	UINT uBlockPitch;
	switch( srcFormat )
	{
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_SNORM:
	case DXGI_FORMAT_BC4_UNORM:
		uBlockPitch = 8;
		break;
	default:
		uBlockPitch = 16;
		break;
	}

	UINT rowPitch = m_bBC ? (uBlockPitch * (m_uWidth / 4)) : (m_uWidth * uElementWidth);
	UINT slicePitch = rowPitch * height;

	UINT iA = 0;

	for( UINT i = 0; i < (bCube ? 6 : (UINT) RA_DATA_ELEMENTS); ++i )
	{
		if( !CFormatHelper::CreateGradient( srcFormat, bOneDim ? 1 : height, m_uWidth, m_ColorSets[ i % NUM_COLOR_SETS ], m_pResourceData[ i ], rowPitch ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateGradient() failed." ) );
			return RESULT_FAIL;
		}		
	}

	switch( m_ResourceScenario )
	{
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		tex2DDesc.ArraySize			= bCube ? 6 : m_uArraySize;
		tex2DDesc.BindFlags			= 0;
		tex2DDesc.CPUAccessFlags	= bCreateTwo ? D3D11_CPU_ACCESS_READ : 0;
		tex2DDesc.Format			= srcFormat;
		tex2DDesc.Height			= height;
		tex2DDesc.MipLevels			= RA_MIPLEVELS;
		tex2DDesc.MiscFlags			= bCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;
		tex2DDesc.SampleDesc.Count	= 1;
		tex2DDesc.SampleDesc.Quality= 0;
		tex2DDesc.Usage				= bCreateTwo ? D3D11_USAGE_STAGING : D3D11_USAGE_DEFAULT;;
		tex2DDesc.Width				= m_uWidth;

		for( iA = 0; iA < (bCube ? 6 : (UINT)m_uArraySize); ++iA )
		{
			subResData[ iA ].pSysMem = m_pResourceData[ iA % RA_DATA_ELEMENTS ];
			subResData[ iA ].SysMemPitch = rowPitch;
			subResData[ iA ].SysMemSlicePitch = slicePitch;
		}

		if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, subResData, reinterpret_cast<ID3D11Texture2D**>(&m_pSrcResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture2D() failed for source resource, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		tex2DDesc.BindFlags = 0;
		if( m_SrcFormat != DXGI_FORMAT_R9G9B9E5_SHAREDEXP )
		{
			tex2DDesc.Width /= 4;
			tex2DDesc.Height /= 4;
		}
		tex2DDesc.Format = bCreateTwo ? destFormat : srcFormat;
		tex2DDesc.Usage = D3D11_USAGE_STAGING;
		tex2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, reinterpret_cast<ID3D11Texture2D**>(&m_pDestResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture2D() failed for destination resource, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:

		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		tex3DDesc.BindFlags			= 0;
		tex3DDesc.CPUAccessFlags	= bCreateTwo ? D3D11_CPU_ACCESS_READ : 0;
		tex3DDesc.Depth				= m_uDepth;
		tex3DDesc.Format			= srcFormat;
		tex3DDesc.Height			= m_uHeight;
		tex3DDesc.MipLevels			= RA_MIPLEVELS;
		tex3DDesc.MiscFlags			= 0;
		tex3DDesc.Usage				= bCreateTwo ? D3D11_USAGE_STAGING : D3D11_USAGE_DEFAULT;
		tex3DDesc.Width				= m_uWidth;

		for( iA = 0; iA < RA_MIPLEVELS; ++iA )
		{
			subResData[ iA ].pSysMem = m_pResourceData[0];
			subResData[ iA ].SysMemPitch = rowPitch;
			subResData[ iA ].SysMemSlicePitch = slicePitch;
		}

		if( FAILED( hr = GetDevice()->CreateTexture3D( &tex3DDesc, subResData, reinterpret_cast<ID3D11Texture3D**>(&m_pSrcResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture3D() failed for source resource, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		tex3DDesc.BindFlags = 0;
		tex3DDesc.Format = bCreateTwo ? destFormat : srcFormat;
		tex3DDesc.Usage = D3D11_USAGE_STAGING;
		if( m_SrcFormat != DXGI_FORMAT_R9G9B9E5_SHAREDEXP )
		{
			tex3DDesc.Width /= 4;
			tex3DDesc.Height /= 4;
		}
		tex3DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		if( FAILED( hr = GetDevice()->CreateTexture3D( &tex3DDesc, NULL, reinterpret_cast<ID3D11Texture3D**>(&m_pDestResource) ) ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture3D() failed for destination resource, hr = 0x%x" ), hr );
			return RESULT_FAIL;
		}

		break;
	}

	return RESULT_PASS;
}

TEST_RESULT CCopyCompressedTest::ExecuteTestCase()
{
	HRESULT hr;
	
	GetEffectiveContext()->CopyResource( m_pDestResource, m_pSrcResource );
	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
		return RESULT_FAIL;
	}

	if( RESULT_PASS != ValidateResult( ) )
	{
		WriteToLog( _T( "Copy from compressed to structured failed" ) );
		return RESULT_FAIL;
	}

	GetEffectiveContext()->CopyResource( m_pSrcResource, m_pDestResource );
	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
		return RESULT_FAIL;
	}

	if( RESULT_PASS != ValidateResult( ) )
	{
		WriteToLog( _T( "Copy from structured to compressed failed" ) );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}
