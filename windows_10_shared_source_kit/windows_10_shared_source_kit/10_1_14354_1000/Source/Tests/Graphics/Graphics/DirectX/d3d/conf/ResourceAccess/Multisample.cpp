#include "Multisample.h"
#include "D3DMSHelper.h"

extern tstring __stdcall ToString( const ACCESS_METHOD &access );
extern bool __stdcall FromString( ACCESS_METHOD *pAccess, const tstring &str );

void CCopyMultisampleTest::InitTestParameters()
{
	// compressed formats not supported for multisampling
	CFilterHandleCommonSource<DXGI_FORMAT> formatFilter = !(
		FilterRegExp<DXGI_FORMAT>( _T( "UNKNOWN" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "TYPELESS" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "SHAREDEXP" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "BC" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "R8G8_B8G8" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "A8_UNORM" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "G8R8_G8B8" ) ) );
	testfactor::RFactor rfSrcFormat = AddParameter( _T( "SrcFormat" ), &m_SrcFormat, FormatValueSet( formatFilter ) );
	testfactor::RFactor rfDestFormat = AddParameter( _T( "DestFormat" ), &m_DestFormat, FormatValueSet( formatFilter ) );

	CUserValueSet<UINT32> *pSampleValues = new CUserValueSet<UINT32>( );
	pSampleValues->AddValue( 1 );
	pSampleValues->AddValue( 2 );
	pSampleValues->AddValue( 4 );
	pSampleValues->AddValue( 8 );
	pSampleValues->AddValue( 16 );
	pSampleValues->AddValue( 32 );
	testfactor::RFactor rfSamples = AddParameter( _T( "Samples" ), &m_uNumSamples, pSampleValues );

	CTestCaseParameter<D3D_RESOURCE_SCENARIO> *pResParam = AddParameter( _T( "ResourceScenario" ), &m_ResourceScenario );
	testfactor::RFactor rfRes2DMS = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D_MS );
	testfactor::RFactor rfRes2DMSArr = AddParameterValue<D3D_RESOURCE_SCENARIO>( pResParam, D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY );

	CUserValueSet<UINT32> *pWidthValues = new CUserValueSet<UINT32>( );
	pWidthValues->AddValue( 16 );
	pWidthValues->AddValue( 8 );
	pWidthValues->AddValue( 2 );
	pWidthValues->AddValue( 3 );
	testfactor::RFactor rfWidth = AddParameter( _T( "Width" ), &m_uWidth, pWidthValues );

	CUserValueSet<UINT32> *pHeightValues = new CUserValueSet<UINT32>( );
	pHeightValues->AddValue( 8 );
	pHeightValues->AddValue( 13 );
	pHeightValues->AddValue( 2 );
	pHeightValues->AddValue( 7 );
	testfactor::RFactor rfHeight = AddParameter( _T( "Height" ), &m_uHeight, pHeightValues );

	CTestCaseParameter<ACCESS_METHOD>* pAccessParam = AddParameter( _T( "AccessMethod" ), &m_AccessMethod );
	CUserValueSet<ACCESS_METHOD>* pAccessSet = new CUserValueSet<ACCESS_METHOD>();
	pAccessSet->AddValue( AM_COPY );
	pAccessSet->AddValue( AM_COPYREGION );
	testfactor::RFactor rfAccessMethod = AddParameterValueSet( pAccessParam, pAccessSet );

	SetRootTestFactor( ( rfAccessMethod * rfRes2DMS * ( rfSrcFormat % rfDestFormat ) * rfSamples * ( rfWidth % rfHeight ) ) );
}

TEST_RESULT CCopyMultisampleTest::Setup()
{
	if( GetFramework()->m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_0 )
		return RESULT_SKIP;

	return CResourceAccessTest::Setup();
}

TEST_RESULT CCopyMultisampleTest::SetupTestCase()
{
	if( GetTypelessFormat( m_SrcFormat ) != GetTypelessFormat( m_DestFormat ) )
	{
		SkipMatching( FilterEqual( _T("DestFormat"), m_DestFormat ) );
		return RESULT_SKIP;
	}

	UINT fmtFlags = 0;
	GetDevice( )->CheckFormatSupport( m_SrcFormat, &fmtFlags );
	if( (fmtFlags & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET) == 0 || (fmtFlags & D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD) == 0 )
	{
		SkipMatching( FilterEqual( _T("SrcFormat"), m_SrcFormat ) );
		return RESULT_SKIP;
	}

	UINT32 uQualityLevels = 0;
	GetDevice( )->CheckMultisampleQualityLevels( m_SrcFormat, m_uNumSamples, &uQualityLevels );
	if( uQualityLevels == 0 )
	{
		SkipMatching( FilterEqual( _T("Samples"), m_uNumSamples ) );
		return RESULT_SKIP;
	}

	TEST_RESULT tr = CreateResources();

	if( tr != RESULT_PASS )
		return tr;

	return RESULT_PASS;
}

TEST_RESULT CCopyMultisampleTest::CreateResources()
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC tex2DDesc;

	UINT scenFlags = 0;
	GetScenarioFlags( m_ResourceScenario, &scenFlags );
	bool bArray = (scenFlags & D3D_RS_FLAGS_ARRAY) != 0;
	UINT uElementWidth = max( GetBitsPerElement( m_SrcFormat ) / 8, 1 );
	UINT height = m_uHeight;

	UINT iA = 0;

	SIZE_T uSize = m_uWidth * m_uHeight * uElementWidth;
	BYTE *pResData = (BYTE*)m_pResourceData;
	for( UINT i = 0; i < m_uNumSamples; ++i )
	{
		if( !CFormatHelper::CreateGradient( DXGI_FORMAT_R32G32B32A32_FLOAT, height, m_uWidth, m_ColorSets[ i % NUM_COLOR_SETS ], pResData + uSize * i ) )
		{
			WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateGradient() failed." ) );
			return RESULT_FAIL;
		}		
	}

	tex2DDesc.ArraySize			= bArray ? RA_ARRAYSIZE : 1;
	tex2DDesc.BindFlags			= D3D11_BIND_RENDER_TARGET;
	tex2DDesc.CPUAccessFlags	= 0;
	tex2DDesc.Format			= m_SrcFormat;
	tex2DDesc.Height			= height;
	tex2DDesc.MipLevels			= 1;
	tex2DDesc.MiscFlags			= 0;
	tex2DDesc.SampleDesc.Count	= m_uNumSamples;
	tex2DDesc.SampleDesc.Quality= 0;
	tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
	tex2DDesc.Width				= m_uWidth;

	if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, reinterpret_cast<ID3D11Texture2D**>(&m_pSrcResource) ) ) )
	{
		WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture2D() failed for source resource, hr = 0x%x" ), hr );
		return RESULT_FAIL;
	}

	tex2DDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, reinterpret_cast<ID3D11Texture2D**>(&m_pDestResource) ) ) )
	{
		WriteToLog( _T( "CResourceAccessTest::CreateResources() - CreateTexture2D() failed for dest resource, hr = 0x%x" ), hr );
		return RESULT_FAIL;
	}

	MSHelper::Write( GetFramework( ), reinterpret_cast<ID3D11Texture2D*>( m_pSrcResource ), (FLOAT*) pResData, m_SrcFormat );
	
	m_bIntFormat = (ToString( m_SrcFormat ).find( "INT" ) != -1);
	BYTE *pTempData = new BYTE[ MAX_RESOURCE_SIZE ];
	if ( !CFormatHelper::ConvertPixels( CONV_TO_FORMAT, m_SrcFormat, (FLOAT_COLOR*)pResData, m_uHeight, m_uWidth * m_uNumSamples, pTempData ) )
	{
		delete[] pTempData;
		WriteToLog( _T("CResourceAccessTest::CreateResources() - CFormatHelper::ConvertPixels() failed") );
		return RESULT_FAIL;
	}
	if ( !CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, m_SrcFormat, (FLOAT_COLOR*)pResData, m_uHeight, m_uWidth * m_uNumSamples, pTempData ) )
	{
		delete[] pTempData;
		WriteToLog(_T("CResourceAccessTest::CreateResources() - CFormatHelper::ConvertPixels() failed"));
		return RESULT_FAIL;
	}
	delete [ ] pTempData;

	return RESULT_PASS;
}

TEST_RESULT CCopyMultisampleTest::ExecuteTestCase()
{
	//
	// Perform the resource copy on the GPU
	//
	if( m_AccessMethod == AM_COPY )
	{
		GetEffectiveContext()->CopyResource( m_pDestResource, m_pSrcResource );
	}
	else if( m_AccessMethod == AM_COPYREGION )
	{
		// CopySubresourceRegion on MSAA textures only supports destination point (0,0,0) and NULL source box (ie: copying the whole subresource only)
		GetEffectiveContext()->CopySubresourceRegion( m_pDestResource, 0, 0, 0, 0, m_pSrcResource, 0, NULL );
	}
	else
	{
		assert( "Unsupported access method" && false );
		return RESULT_BLOCKED;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
		return RESULT_FAIL;
	}

	//
	// Read back the output
	//
	BYTE *pOutputResult = new BYTE[ MAX_RESOURCE_SIZE ];
	memset( pOutputResult, 0, sizeof( pOutputResult ) );

	MSHelper::Read( GetFramework( ), reinterpret_cast<ID3D11Texture2D*>( m_pDestResource ), (FLOAT*) pOutputResult, m_SrcFormat );
	FLOAT *pExpected = (FLOAT*) m_pResourceData;
	FLOAT *pOutput = (FLOAT*) pOutputResult;
	
	//
	// Validate the output
	//
	for( UINT uS = 0; uS < m_uNumSamples; ++uS )
	{
		for( UINT uY = 0; uY < m_uHeight; ++uY )
		{
			for( UINT uX = 0; uX < m_uWidth; ++uX )
			{
				for( UINT uB = 0; uB < 4; ++uB )
				{
					if( !WithinOneULP( *pExpected, *pOutput ) )
					{
						WriteToLog( _T( "Sample %d does not match at texel %d, %d channel %d\n\t\tExpected: %f\n\t\tOutput: %f" ),
							uS, uX, uY, uB, *pExpected, *pOutput );
						delete [ ] pOutputResult;
						return RESULT_FAIL;
					}
					pExpected++; pOutput++;
				}
			}
		}
	}

	delete [ ] pOutputResult;

	return RESULT_PASS;
}
