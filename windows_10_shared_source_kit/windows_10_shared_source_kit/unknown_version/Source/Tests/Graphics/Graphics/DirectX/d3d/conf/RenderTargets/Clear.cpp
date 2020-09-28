#include "Clear.h"

static const float initialColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

//
// Given a top level texture dimension: "dim"
// and a mip level: "mip"
// this returns the size of the coressponding dimension at the given mip level
//
UINT MipDimension(UINT dim, UINT mip)
{
	UINT result = dim;

	for( UINT i = 0; i < mip; ++i )
	{
		result = result / 2;
		result = max(result, 1);
	}

	return result;
}

void CClearRenderTargetViewTest::InitTestParameters( )
{
	CTestCaseParameter<D3D_RESOURCE_SCENARIO> *pResParam = AddParameter( _T( "ResourceType" ), &m_ResourceType );
	CUserValueSet<D3D_RESOURCE_SCENARIO> *pResValues = new CUserValueSet<D3D_RESOURCE_SCENARIO>( );
	pResValues->AddValue( D3D_RESOURCE_SCENARIO_BUFFER );
	pResValues->AddValue( D3D_RESOURCE_SCENARIO_TEXTURE1D );
	pResValues->AddValue( D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY );
	pResValues->AddValue( D3D_RESOURCE_SCENARIO_TEXTURE2D );
	pResValues->AddValue( D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY );
	pResValues->AddValue( D3D_RESOURCE_SCENARIO_TEXTURE3D );
	testfactor::RFactor rfResType = AddParameterValueSet( pResParam, pResValues );

	testfactor::RFactor rfFormat = AddParameter( _T( "Format" ), &m_Format, FormatValueSet( FF_ALL, FF_TYPELESS ) );
	
	UINT32 infp = INF_P;
	UINT32 infn = INF_N;
	UINT32 nan = NAN;
	UINT32 denorm = DENORM;

	CTestCaseParameter<float> *pClearParam = AddParameter( _T( "ClearValue" ), &m_fClearVal );
	CUserValueSet<float> *pClearValues = new CUserValueSet<float>( );
	
	// Values > 0 and < 1 are important for testing UNORM clears
	pClearValues->AddValue( 0.25f); 
	pClearValues->AddValue( 0.5f);
	pClearValues->AddValue( 0.75f);
	pClearValues->AddValue( 1.f );
	pClearValues->AddValue( 256.f );
	pClearValues->AddValue( 65536.f );
	pClearValues->AddValue( -1.f );
	pClearValues->AddValue( *( (float*) &infp ) );
	pClearValues->AddValue( *( (float*) &infn ) );
	//pClearValues->AddValue( *( (float*) &nan ) );
	pClearValues->AddValue( *( (float*) &denorm ) );
	testfactor::RFactor rfClearVal = AddParameterValueSet( pClearParam, pClearValues );

	CTestCaseParameter<tstring> *pSlotParam = AddParameter( _T( "SlotConfig" ), &m_szSlots );
	CUserValueSet<tstring> *pSlotValues = new CUserValueSet<tstring>( );
	pSlotValues->AddValue( _T( "ZZZZ" ) );
	pSlotValues->AddValue( _T( "ZVZV" ) );
	pSlotValues->AddValue( _T( "VZVZ" ) );
	pSlotValues->AddValue( _T( "VVVV" ) );
	testfactor::RFactor rfSlots = AddParameterValueSet( pSlotParam, pSlotValues );

	CUserValueSet<UINT>* pClearOffsetValues = new CUserValueSet<UINT>( );
	pClearOffsetValues->AddValue( 0 );
	pClearOffsetValues->AddValue( 1 );
	pClearOffsetValues->AddValue( 2 );

	CTestCaseParameter<UINT> *pClearOffsetParam0 = AddParameter( _T( "ClearOffset0" ), &m_uClearOffset[ 0 ] );
	testfactor::RFactor rfClearOffset0 = AddParameterValueSet( pClearOffsetParam0, pClearOffsetValues );

	CTestCaseParameter<UINT> *pClearOffsetParam1 = AddParameter( _T( "ClearOffset1" ), &m_uClearOffset[ 1 ] );
	testfactor::RFactor rfClearOffset1 = AddParameterValueSet( pClearOffsetParam1, pClearOffsetValues );

	CTestCaseParameter<UINT> *pClearOffsetParam2 = AddParameter( _T( "ClearOffset2" ), &m_uClearOffset[ 2 ] );
	testfactor::RFactor rfClearOffset2 = AddParameterValueSet( pClearOffsetParam2, pClearOffsetValues );

	SetRootTestFactor( (rfResType * rfFormat * rfClearVal * rfSlots) % ( rfClearOffset0 * rfClearOffset1 * rfClearOffset2 ) );

	AddPriorityPatternFilter(FilterEqual<D3D_RESOURCE_SCENARIO>(_T("ResourceType"), D3D_RESOURCE_SCENARIO_TEXTURE2D), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue(1, 1.5f);
}

TEST_RESULT CClearRenderTargetViewTest::Setup( )
{
	HRESULT hr = E_FAIL;

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

	return RESULT_PASS;
}
	
TEST_RESULT CClearRenderTargetViewTest::SetupTestCase( )
{
	HRESULT hr = S_OK;

	// We don't run all test cases on FL9 be default; only specific test cases for new features
	if( m_bFL9X && !IsValidFL9TestCase() )
	{
		WriteToLog( _T("The test is running at feature level 9.x. Not all test cases are necessarily supported.") );
		return RESULT_SKIP;
	}

	if( m_bFL9X )
	{
		switch( m_ResourceType )
		{
		case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURE2D_MS_ARRAY:
		case D3D_RESOURCE_SCENARIO_TEXTURECUBE_ARRAY:
			WriteToLog( _T("Feature level 9 doesn't support texture arrays.") );
			SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "ResourceType" ), m_ResourceType ) );
			return RESULT_SKIP;
			break;
		};
	}

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
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY ] = D3D11_FORMAT_SUPPORT_TEXTURE1D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE2D ] = D3D11_FORMAT_SUPPORT_TEXTURE2D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY ] = D3D11_FORMAT_SUPPORT_TEXTURE2D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURECUBE ] = D3D11_FORMAT_SUPPORT_TEXTURE2D;
	resTypes[ D3D_RESOURCE_SCENARIO_TEXTURE3D ] = D3D11_FORMAT_SUPPORT_TEXTURE3D;

	if( (supportFlags & resTypes[ m_ResourceType ]) == 0 )
	{
		SkipMatching( GetSkipMatchingFilter() || FilterEqual( _T( "ResourceType" ), m_ResourceType ) );
		return RESULT_SKIP;
	}

	D3D11_BUFFER_DESC bufferDesc;
	D3D11_TEXTURE1D_DESC tex1DDesc;
	D3D11_TEXTURE2D_DESC tex2DDesc;
	D3D11_TEXTURE3D_DESC tex3DDesc;

	UINT scenFlags = 0;
	GetScenarioFlags( m_ResourceType, &scenFlags );
	bool bCube = (scenFlags & D3D_RS_FLAGS_TEXTURECUBE) != 0;
	bool bArray = (scenFlags & D3D_RS_FLAGS_ARRAY) != 0;
	bool bFormatSupportsMips = (supportFlags & D3D11_FORMAT_SUPPORT_MIP) != 0;	// Some video formats don't support mip levels
	bool bPlanar = CD3D10FormatHelper_Test::Planar( m_Format ) ? true : false;
	UINT planarArraySize = 2;
	UINT mipLevels = bFormatSupportsMips ? 3 : 1;
	UINT bytesPerElem = max( GetBitsPerElement( m_Format ) / 8, 1 );
	
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = CRenderTargetsTest::GetRTViewFormat( m_Format );
	if( CD3D10FormatHelper_Test::YUV( m_Format ) )
	{
		WriteToLog( _T("RTV format set to %s."), ToString(rtvDesc.Format).c_str() );
	}

	if( bCube )
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	else
		GetViewDimension( m_ResourceType, &rtvDesc.ViewDimension );

	switch( m_ResourceType )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		{
			bufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
			bufferDesc.ByteWidth = bytesPerElem * CRTV_TEXDIM;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;

			if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufferDesc, NULL, reinterpret_cast<ID3D11Buffer**>( &m_pTexture ), m_Format ) ) )
			{
				WriteToLog( _T( "Failed to create Buffer resource, hr = %s" ), D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}

			bufferDesc.BindFlags = 0;
			bufferDesc.Usage = D3D11_USAGE_STAGING;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

			if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufferDesc, NULL, reinterpret_cast<ID3D11Buffer**>( &m_pTextureCopy ) ) ) )
			{
				WriteToLog( _T( "Failed to create copy of Buffer resource, hr = %s" ), D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}

			rtvDesc.Buffer.ElementOffset = m_uClearOffset[0];
			rtvDesc.Buffer.ElementWidth = CRTV_TEXDIM - (m_uClearOffset[0] + m_uClearOffset[1]);

			assert( (rtvDesc.Buffer.ElementOffset + rtvDesc.Buffer.ElementWidth) <= CRTV_TEXDIM );

			//
			// Clear the buffer to black
			//
			D3D11_RENDER_TARGET_VIEW_DESC rtvDescFull = rtvDesc;
			rtvDescFull.Buffer.ElementOffset = 0;
			rtvDescFull.Buffer.ElementWidth = CRTV_TEXDIM;

			ID3D11RenderTargetView* pRTVBuff = NULL;
			if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pTexture, &rtvDescFull, &pRTVBuff ) ) )
			{
				WriteToLog( _T( "Failed creation of RenderTargetView (full), hr = %s" ), D3DHResultToString( hr ).c_str( ) );
				return RESULT_FAIL;
			}

			GetEffectiveContext( )->ClearRenderTargetView( pRTVBuff, initialColor );

			pRTVBuff->Release();
			pRTVBuff = NULL;
		}
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		{
			tex1DDesc.ArraySize = bArray ? (bPlanar ? planarArraySize : 4) : 1;
			tex1DDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
			tex1DDesc.CPUAccessFlags = 0;
			tex1DDesc.Format = m_Format;
			tex1DDesc.MipLevels = mipLevels;
			tex1DDesc.MiscFlags = 0;
			tex1DDesc.Usage = D3D11_USAGE_DEFAULT;
			tex1DDesc.Width = CRTV_TEXDIM;

			if( FAILED( hr = GetDevice()->CreateTexture1D( &tex1DDesc, NULL, reinterpret_cast<ID3D11Texture1D**>( &m_pTexture ) ) ) )
			{
				WriteToLog( _T( "Failed to create Texture1D resource, hr = %s" ), D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}

			tex1DDesc.BindFlags = 0;
			tex1DDesc.Usage = D3D11_USAGE_STAGING;
			tex1DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

			if( FAILED( hr = GetDevice()->CreateTexture1D( &tex1DDesc, NULL, reinterpret_cast<ID3D11Texture1D**>( &m_pTextureCopy ) ) ) )
			{
				WriteToLog( _T( "Failed to create copy of Texture1D resource, hr = %s" ), D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}

			if( bArray )
			{
				rtvDesc.Texture1DArray.ArraySize = max( m_uClearOffset[0], 1 );
				rtvDesc.Texture1DArray.FirstArraySlice = m_uClearOffset[1];
				rtvDesc.Texture1DArray.MipSlice = m_uClearOffset[2];

				if( !bFormatSupportsMips &&	// Format doesn't support mip levels
					(rtvDesc.Texture1DArray.MipSlice != 0) )
				{
					WriteToLog( _T("%s doesn't support multiple mip levels but MipSlice = %u. Skipping test case."), ToString(m_Format).c_str(), rtvDesc.Texture1DArray.MipSlice );
					return RESULT_SKIP;
				}

				if( bPlanar &&	// Format is planar YUV
					((rtvDesc.Texture1DArray.FirstArraySlice + rtvDesc.Texture1DArray.ArraySize) > planarArraySize) )
				{
					WriteToLog( _T("%s is planar and has a max array size of %u. The view includes subresources past that slice. Skipping test case."), ToString(m_Format).c_str(), planarArraySize );
					return RESULT_SKIP;
				}

				assert( rtvDesc.Texture1DArray.ArraySize > 0 );
				assert( (rtvDesc.Texture1DArray.FirstArraySlice + rtvDesc.Texture1DArray.ArraySize) <= tex1DDesc.ArraySize );
				assert( rtvDesc.Texture1DArray.MipSlice < tex1DDesc.MipLevels );
			}
			else
			{
				rtvDesc.Texture1D.MipSlice = m_uClearOffset[0];

				if( !bFormatSupportsMips &&	// Format doesn't support mip levels
					rtvDesc.Texture1D.MipSlice != 0 )
				{
					WriteToLog( _T("%s doesn't support multiple mip levels but MipSlice = %u. Skipping test case."), ToString(m_Format).c_str(), rtvDesc.Texture1D.MipSlice );
					return RESULT_SKIP;
				}

				assert( rtvDesc.Texture1D.MipSlice < tex1DDesc.MipLevels );
			}

			//
			// Clear each mip level of the tex1D to black
			//
			for( UINT m = 0; m < tex1DDesc.MipLevels; ++m )
			{
				ID3D11RenderTargetView* pRTVTex1D = NULL;

				D3D11_RENDER_TARGET_VIEW_DESC rtvTex1DDesc = rtvDesc;

				if( bArray )
				{
					rtvTex1DDesc.Texture1DArray.MipSlice = m;
					rtvTex1DDesc.Texture1DArray.FirstArraySlice = 0;
					rtvTex1DDesc.Texture1DArray.ArraySize = tex1DDesc.ArraySize;
				}
				else
				{
					rtvTex1DDesc.Texture1D.MipSlice = m;
				}

				if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pTexture, &rtvTex1DDesc, &pRTVTex1D ) ) )
				{
					WriteToLog( _T( "Failed creation of RenderTargetView (full), hr = %s" ), D3DHResultToString( hr ).c_str( ) );
					return RESULT_FAIL;
				}

				GetEffectiveContext( )->ClearRenderTargetView( pRTVTex1D, initialColor );

				pRTVTex1D->Release();
				pRTVTex1D = NULL;
			}
		}
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		{
			tex2DDesc.ArraySize = bCube ? 6 : (bArray ? (bPlanar ? planarArraySize : 4) : 1);
			tex2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
			tex2DDesc.CPUAccessFlags = 0;
			tex2DDesc.Format = m_Format;
			tex2DDesc.MipLevels = mipLevels;
			tex2DDesc.MiscFlags = bCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;
			tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
			tex2DDesc.Width = CRTV_TEXDIM;
			tex2DDesc.Height = CRTV_TEXDIM;
			tex2DDesc.SampleDesc.Count = 1;
			tex2DDesc.SampleDesc.Quality = 0;

			if( FAILED( hr = GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional( &tex2DDesc, NULL, reinterpret_cast<ID3D11Texture2D**>( &m_pTexture ) ) ) )
			{
				WriteToLog( _T( "Failed to create Texture2D resource, hr = %s" ), D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}

			tex2DDesc.BindFlags = 0;
			tex2DDesc.Usage = D3D11_USAGE_STAGING;
			tex2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

			if( FAILED( hr = GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional( &tex2DDesc, NULL, reinterpret_cast<ID3D11Texture2D**>( &m_pTextureCopy ) ) ) )
			{
				WriteToLog( _T( "Failed to create copy of Texture2D resource, hr = %s" ), D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}

			if( bArray || bCube )
			{
				rtvDesc.Texture2DArray.ArraySize = max( m_uClearOffset[0], 1 );
				rtvDesc.Texture2DArray.FirstArraySlice = m_uClearOffset[1];
				rtvDesc.Texture2DArray.MipSlice = m_uClearOffset[2];

				if( !bFormatSupportsMips &&	// Format doesn't support mip levels
					rtvDesc.Texture2DArray.MipSlice != 0 )
				{
					WriteToLog( _T("%s doesn't support multiple mip levels but MipSlice = %u. Skipping test case."), ToString(m_Format).c_str(), rtvDesc.Texture2DArray.MipSlice );
					return RESULT_SKIP;
				}

				if( bPlanar &&	// Format is planar YUV
					((rtvDesc.Texture2DArray.FirstArraySlice + rtvDesc.Texture2DArray.ArraySize) > planarArraySize) )
				{
					WriteToLog( _T("%s is planar and has a max array size of %u. The view includes subresources past that slice. Skipping test case."), ToString(m_Format).c_str(), planarArraySize );
					return RESULT_SKIP;
				}

				assert( rtvDesc.Texture2DArray.ArraySize > 0 );
				assert( (rtvDesc.Texture2DArray.FirstArraySlice + rtvDesc.Texture2DArray.ArraySize) <= tex2DDesc.ArraySize );
				assert( rtvDesc.Texture2DArray.MipSlice < tex2DDesc.MipLevels );
			}
			else
			{
				rtvDesc.Texture2D.MipSlice = m_uClearOffset[0];

				if( !bFormatSupportsMips &&	// Format doesn't support mip levels
					rtvDesc.Texture2D.MipSlice != 0 )
				{
					WriteToLog( _T("%s doesn't support multiple mip levels but MipSlice = %u. Skipping test case."), ToString(m_Format).c_str(), rtvDesc.Texture2D.MipSlice );
					return RESULT_SKIP;
				}

				assert( rtvDesc.Texture2D.MipSlice < tex2DDesc.MipLevels );
			}

			//
			// Clear each mip level of the tex2D to black
			//
			for( UINT m = 0; m < tex2DDesc.MipLevels; ++m )
			{
				ID3D11RenderTargetView* pRTVTex2D = NULL;

				D3D11_RENDER_TARGET_VIEW_DESC rtvTex2DDesc = rtvDesc;

				if( tex2DDesc.ArraySize > 1 )
				{
					rtvTex2DDesc.Texture2DArray.MipSlice = m;
					rtvTex2DDesc.Texture2DArray.FirstArraySlice = 0;
					rtvTex2DDesc.Texture2DArray.ArraySize = tex2DDesc.ArraySize;

				}
				else
				{
					rtvTex2DDesc.Texture2D.MipSlice = m;
				}

				if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pTexture, &rtvTex2DDesc, &pRTVTex2D ) ) )
				{
					WriteToLog( _T( "Failed creation of RenderTargetView (full), hr = %s" ), D3DHResultToString( hr ).c_str( ) );
					return RESULT_FAIL;
				}

				GetEffectiveContext( )->ClearRenderTargetView( pRTVTex2D, initialColor );

				pRTVTex2D->Release();
				pRTVTex2D = NULL;
			}
		}
		break;
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		{
			tex3DDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
			tex3DDesc.CPUAccessFlags = 0;
			tex3DDesc.Format = m_Format;
			tex3DDesc.MipLevels = mipLevels;
			tex3DDesc.MiscFlags = 0;
			tex3DDesc.Usage = D3D11_USAGE_DEFAULT;
			tex3DDesc.Width = CRTV_TEXDIM;
			tex3DDesc.Height = CRTV_TEXDIM;
			tex3DDesc.Depth = 16;

			if( FAILED( hr = GetFramework()->CreateTexture3DMapDefaultOptional( &tex3DDesc, NULL, reinterpret_cast<ID3D11Texture3D**>( &m_pTexture ) ) ) )
			{
				WriteToLog( _T( "Failed to create Texture3D resource, hr = %s" ), D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}

			tex3DDesc.BindFlags = 0;
			tex3DDesc.Usage = D3D11_USAGE_STAGING;
			tex3DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

			if( FAILED( hr = GetFramework()->CreateTexture3DMapDefaultOptional( &tex3DDesc, NULL, reinterpret_cast<ID3D11Texture3D**>( &m_pTextureCopy ) ) ) )
			{
				WriteToLog( _T( "Failed to create copy of Texture3D resource, hr = %s" ), D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}

			rtvDesc.Texture3D.FirstWSlice = m_uClearOffset[0];
			rtvDesc.Texture3D.MipSlice = m_uClearOffset[1];
			rtvDesc.Texture3D.WSize = max( m_uClearOffset[0], 1 );

			if( !bFormatSupportsMips &&	// Format doesn't support mip levels
				rtvDesc.Texture3D.MipSlice != 0 )
			{
				WriteToLog( _T("%s doesn't support multiple mip levels but MipSlice = %u. Skipping test case."), ToString(m_Format).c_str(), rtvDesc.Texture3D.MipSlice );
				return RESULT_SKIP;
			}

			assert( rtvDesc.Texture3D.WSize > 0 );
			assert( (rtvDesc.Texture3D.FirstWSlice + rtvDesc.Texture3D.WSize) <= tex3DDesc.Depth );
			assert( rtvDesc.Texture3D.MipSlice < tex3DDesc.MipLevels );

			//
			// Clear each mip level of the tex3D to black
			//
			for( UINT m = 0; m < tex3DDesc.MipLevels; ++m )
			{
				ID3D11RenderTargetView* pRTVTex3D = NULL;

				D3D11_RENDER_TARGET_VIEW_DESC rtvTex3DDesc = rtvDesc;
				rtvTex3DDesc.Texture3D.MipSlice    = m;
				rtvTex3DDesc.Texture3D.FirstWSlice = 0;
				rtvTex3DDesc.Texture3D.WSize       = MipDimension(tex3DDesc.Depth, m);

				if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pTexture, &rtvTex3DDesc, &pRTVTex3D ) ) )
				{
					WriteToLog( _T( "Failed creation of RenderTargetView (full), hr = %s" ), D3DHResultToString( hr ).c_str( ) );
					return RESULT_FAIL;
				}

				GetEffectiveContext( )->ClearRenderTargetView( pRTVTex3D, initialColor );

				pRTVTex3D->Release();
				pRTVTex3D = NULL;
			}
		}
		break;
	}

	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pTexture, &rtvDesc, &m_pRTVSubset ) ) )
	{
		WriteToLog( _T( "Failed creation of RenderTargetView (subset), hr = %s" ), D3DHResultToString( hr ).c_str( ) );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

void CClearRenderTargetViewTest::CleanupTestCase( )
{
	SAFE_RELEASE( m_pRTVSubset );
	SAFE_RELEASE( m_pTexture );
	SAFE_RELEASE( m_pTextureCopy );
}

bool QuantizeColor(FLOAT_COLOR* pOut, FLOAT_COLOR* pIn, DXGI_FORMAT format)
{
	bool success = false;
	
	UINT bytesPerElem = max( GetBitsPerElement( format ) / 8, 1 );

	BYTE* buffer = new BYTE[ bytesPerElem ];

	success = CFormatHelper::ConvertPixels(CONV_TO_FORMAT, format, pIn,  1, 1, buffer);

	success = success && CFormatHelper::ConvertPixels(CONV_FROM_FORMAT, format, pOut, 1, 1, buffer);

	delete [ ] buffer;
	return success;
}

TEST_RESULT CClearRenderTargetViewTest::ExecuteTestCase( )
{
	const DXGI_FORMAT rtvFormat = CRenderTargetsTest::GetRTViewFormat( m_Format );
	FLOAT_COLOR clearValues;
	memset( &clearValues, 0, sizeof(clearValues) );
	for( UINT i = 0; i < 4; ++i )
		if( m_szSlots[ i ] == 'V' )
			clearValues.comp[ i ] = m_fClearVal;

	// If the clear value in (0.0, 1.0) then
	// quantize the clear value to the format of the render target view
	// this allows CompareResults to expect bit-accurate results
	FLOAT_COLOR quantizedColor;
	if ( !QuantizeColor(&quantizedColor, &clearValues, rtvFormat) )
	{
		WriteToLog("QuantizeColor() failed");
		return RESULT_FAIL;
	}

	for( UINT i = 0; i < 4; ++i)
	{
		if( (clearValues.comp[ i ] < 1.f) &&
			(clearValues.comp[ i ] > 0.f) &&
			!CRenderTargetsTest::IsDenorm( clearValues.comp[ i ] ) )
		{
			clearValues.comp[ i ] = quantizedColor.comp[ i ];
		}
	}

	// Clear a subset of the resource to clearValues
	GetEffectiveContext( )->ClearRenderTargetView( m_pRTVSubset, clearValues.comp );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	GetImmediateContext( )->CopyResource( m_pTextureCopy, m_pTexture );

	D3D11_MAPPED_SUBRESOURCE pData;
	D3D11_MAPPED_SUBRESOURCE mapTex2D;
	D3D11_MAPPED_SUBRESOURCE mapTex3D;

	D3D11_RENDER_TARGET_VIEW_DESC SubsetRTVDesc;
	m_pRTVSubset->GetDesc(&SubsetRTVDesc);

	D3D11_BUFFER_DESC DescBuff   = { '\0' };
	D3D11_TEXTURE1D_DESC Desc1D  = { '\0' };
	D3D11_TEXTURE2D_DESC Desc2D  = { '\0' };
	D3D11_TEXTURE3D_DESC Desc3D  = { '\0' };

	switch( m_ResourceType )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		{
			ID3D11Buffer* Buff = NULL;

			HRESULT hr = m_pTextureCopy->QueryInterface(__uuidof(ID3D11Buffer), (VOID**)&Buff);
			if(SUCCEEDED(hr))
			{
				Buff->GetDesc( &DescBuff );
				Buff->Release();
			}
			else
			{
				WriteToLog( _T( "QueryInterface failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}
		}
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		{
			ID3D11Texture1D* Tex1D = NULL;

			HRESULT hr = m_pTextureCopy->QueryInterface(__uuidof(ID3D11Texture1D), (VOID**)&Tex1D);
			if(SUCCEEDED(hr))
			{
				Tex1D->GetDesc( &Desc1D );
				Tex1D->Release();
			}
			else
			{
				WriteToLog( _T( "QueryInterface failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}
		}
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		{
			ID3D11Texture2D* Tex2D = NULL;

			HRESULT hr = m_pTextureCopy->QueryInterface(__uuidof(ID3D11Texture2D), (VOID**)&Tex2D);
			if(SUCCEEDED(hr))
			{
				Tex2D->GetDesc( &Desc2D );
				Tex2D->Release();
			}
			else
			{
				WriteToLog( _T( "QueryInterface failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}
		}
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		{
			ID3D11Texture3D* Tex3D = NULL;

			HRESULT hr = m_pTextureCopy->QueryInterface(__uuidof(ID3D11Texture3D), (VOID**)&Tex3D);
			if(SUCCEEDED(hr))
			{
				Tex3D->GetDesc( &Desc3D );
				Tex3D->Release();
			}
			else
			{
				WriteToLog( _T( "QueryInterface failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
				return RESULT_FAIL;
			}
		}
		break;

	default:
		assert( false );
	}


	FLOAT_COLOR black;
	assert(sizeof(black.comp) == sizeof(initialColor));
	memcpy(black.comp, initialColor, sizeof(initialColor));

	bool bPass = false;

	switch( m_ResourceType )
	{
	case D3D_RESOURCE_SCENARIO_BUFFER:
		{
			assert(D3D11_RTV_DIMENSION_BUFFER == SubsetRTVDesc.ViewDimension);

			UINT bytesPerElem = max( GetBitsPerElement( rtvFormat ) / 8, 1 );
			
			bPass = CompareResultsBuffer( SubsetRTVDesc.Buffer.ElementOffset, 
										  SubsetRTVDesc.Buffer.ElementWidth,
										  DescBuff.ByteWidth / bytesPerElem,
										  &clearValues,
										  &black );
		}
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURE1D:
		{
			assert(D3D11_RTV_DIMENSION_TEXTURE1D == SubsetRTVDesc.ViewDimension);

			bPass = CompareResultsTexture( 1, Desc1D.MipLevels,
										   Desc1D.Width, 1, 1,
										   SubsetRTVDesc.Texture1D.MipSlice, 0, 1,
										   0, 1,
										   &clearValues, &black );
		}
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURE1D_ARRAY:
		{
			assert(D3D11_RTV_DIMENSION_TEXTURE1DARRAY == SubsetRTVDesc.ViewDimension);

			bPass = CompareResultsTexture( Desc1D.ArraySize, Desc1D.MipLevels,
										   Desc1D.Width, 1, 1,
										   SubsetRTVDesc.Texture1DArray.MipSlice, SubsetRTVDesc.Texture1DArray.FirstArraySlice, SubsetRTVDesc.Texture1DArray.ArraySize,
										   0, 1,
										   &clearValues, &black );
		}
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURE2D:
		{
			assert(D3D11_RTV_DIMENSION_TEXTURE2D == SubsetRTVDesc.ViewDimension);

			bPass = CompareResultsTexture( 1, Desc2D.MipLevels,
										   Desc2D.Width, Desc2D.Height, 1,
										   SubsetRTVDesc.Texture2D.MipSlice, 0, 1,
										   0, 1,
										   &clearValues, &black );
		}
		break;

	case D3D_RESOURCE_SCENARIO_TEXTURE2D_ARRAY:
	case D3D_RESOURCE_SCENARIO_TEXTURECUBE:
		{
			assert(D3D11_RTV_DIMENSION_TEXTURE2DARRAY == SubsetRTVDesc.ViewDimension);

			bPass = CompareResultsTexture( Desc2D.ArraySize, Desc2D.MipLevels,
										   Desc2D.Width, Desc2D.Height, 1,
										   SubsetRTVDesc.Texture2DArray.MipSlice, SubsetRTVDesc.Texture2DArray.FirstArraySlice, SubsetRTVDesc.Texture2DArray.ArraySize,
										   0, 1,
										   &clearValues, &black );
		}
		break;

	
	case D3D_RESOURCE_SCENARIO_TEXTURE3D:
		{
			assert(D3D11_RTV_DIMENSION_TEXTURE3D == SubsetRTVDesc.ViewDimension);

			bPass = CompareResultsTexture( 1, Desc3D.MipLevels,
										   Desc3D.Width, Desc3D.Height, Desc3D.Depth,
										   SubsetRTVDesc.Texture3D.MipSlice, 0, 1,
										   SubsetRTVDesc.Texture3D.FirstWSlice, SubsetRTVDesc.Texture3D.WSize,
										   &clearValues, &black );
		}
		break;

	default:
		assert( false );
	}

	return bPass ? RESULT_PASS : RESULT_FAIL;
}

bool CClearRenderTargetViewTest::CompareResultsBuffer( UINT uSubsetX, UINT uSubsetWidth, UINT uFullWidth, 
													   FLOAT_COLOR* pExpectedInBounds, FLOAT_COLOR* pExpectedOutOfBounds )
{
	bool bPass = true;
	bool bLogAll = GetFramework()->LogVerbose( );

	D3D11_MAPPED_SUBRESOURCE pData;

	GetImmediateContext( )->Map( m_pTextureCopy, 0, D3D11_MAP_READ, 0, &pData );

	struct
	{
		UINT         uX;
		UINT         uWidth;
		FLOAT_COLOR* pExpectedColor;
	} TestCases[3] = 
	{
		{ 0,                       uSubsetX,                                pExpectedOutOfBounds }, // out of bounds to the left
		{ uSubsetX,                uSubsetWidth,                            pExpectedInBounds },    // in bounds
		{ uSubsetX + uSubsetWidth, uFullWidth - (uSubsetX + uSubsetWidth),  pExpectedOutOfBounds }, // out of bounds to the right
	};

	for( UINT i = 0; i < _countof(TestCases); ++i )
	{
		if( bPass || bLogAll )
		{
			bool bResult = CompareResults( pData.pData,
										   TestCases[i].uX, 0,
										   TestCases[i].uWidth, 1,
										   0,
										   0, 0,
										   TestCases[i].pExpectedColor );

			bPass = bPass && bResult;
		}
	}

	GetImmediateContext( )->Unmap( m_pTextureCopy, 0 );

	return bPass;
}

bool CClearRenderTargetViewTest::CompareResultsTexture( UINT uArraySize, UINT uMipLevels,
														UINT uWidth, UINT uHeight, UINT uDepth,
														UINT uRTVMipSlice, UINT uRTVFirstArraySlice, UINT uRTVArraySize,
														UINT uRTVFirstWSlice, UINT uRTVWSize,
														FLOAT_COLOR* pExpectedInBounds, FLOAT_COLOR* pExpectedOutOfBounds)
{
	bool bPass = true;
	bool bLogAll = GetFramework()->LogVerbose( );

	D3D11_MAPPED_SUBRESOURCE pData;

	for( UINT a = 0; a < uArraySize; ++a )
	{
		for( UINT m = 0; m < uMipLevels; ++m )
		{
			//
			// Adjust sizes downward for mip levels
			//
			UINT uMipWidth  = MipDimension(uWidth, m);
			UINT uMipHeight = MipDimension(uHeight, m);
			UINT uMipDepth  = MipDimension(uDepth, m);

			UINT uSubresource = D3D11CalcSubresource(m, a, uMipLevels);
			HRESULT hr = GetImmediateContext( )->Map( m_pTextureCopy, uSubresource, D3D11_MAP_READ, 0, &pData );

			if ( FAILED(hr) )
			{
				WriteToLog( _T( "CClearRenderTargetViewTest::CompareResultsTexture() - Map failed, hr = %s" ), D3DHResultToString( hr ).c_str() );
				return false;
			}

			for( UINT w = 0; w < uMipDepth; w++)
			{
				if( bPass || bLogAll )
				{
					bool bInBounds = (m == uRTVMipSlice) &&
									 (a >= uRTVFirstArraySlice) && 
									 (a < (uRTVFirstArraySlice + uRTVArraySize)) &&
									 (w >= uRTVFirstWSlice) &&
									 (w < (uRTVFirstWSlice + uRTVWSize));

					FLOAT_COLOR* expected = bInBounds ? pExpectedInBounds : pExpectedOutOfBounds;

					bool bResult = CompareResults( ((BYTE*)pData.pData) + (w * pData.DepthPitch),
												   0, 0,
												   uMipWidth, uMipHeight,
												   pData.RowPitch, 
												   uSubresource, w,
												   expected );

					

					bPass = bPass && bResult;
				}
			}

			GetImmediateContext( )->Unmap( m_pTextureCopy, uSubresource );
		}
	}

	return bPass;
}

bool CClearRenderTargetViewTest::IsValidFL9TestCase() const
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

bool CClearRenderTargetViewTest::CompareResults( void *pData, 
												 UINT uStartX, UINT uStartY,
												 UINT uWidth, UINT uHeight,
												 UINT uRowPitch, 
												 UINT uSubresource, UINT uWSlice,
												 FLOAT_COLOR* pClearValues )
{
	const DXGI_FORMAT rtvFormat = CRenderTargetsTest::GetRTViewFormat( m_Format );
	UINT bytesPerElem = max( GetBitsPerElement( rtvFormat ) / 8, 1 );
	bool bPass = true;
	bool bLogAll = GetFramework()->LogVerbose( );

	// Clear must flush denormalized values to 0
	FLOAT_COLOR expectedColor = *pClearValues;
	for( UINT i = 0; i < 4; ++i )
	{
		if( CRenderTargetsTest::IsDenorm( expectedColor.comp[ i ] ) )
			expectedColor.comp[ i ] = 0.0f;
	}

	//
	// Ignore the bits in the X8 component
	// the contents of these bits is undefined
	//
	UINT bytesPerElemToCompare = bytesPerElem;

	switch( rtvFormat )
	{
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		assert( 4 == bytesPerElemToCompare );
		bytesPerElemToCompare = 3;
		break;
	}

	BYTE *pExpectedRes = new BYTE[ bytesPerElem ];
	if ( !CFormatHelper::ConvertPixels( CONV_TO_FORMAT, rtvFormat, &expectedColor, 1, 1, pExpectedRes ) )
	{
		WriteToLog( _T("CClearRenderTargetViewTest::CompareResults() - CFormatHelper::ConvertPixels() failed") );
		bPass = false;
		goto methodDone;
	}

	for( UINT y = uStartY; y < (uStartY + uHeight); ++y )
	{
		for( UINT x = uStartX; x < (uStartX + uWidth); ++x )
		{
			BYTE* pCurrentRes = ((BYTE*)pData) + (y * uRowPitch) + (x * bytesPerElem);

			for( UINT iB = 0; iB < bytesPerElemToCompare; ++iB )
			{
				if( ((BYTE*)pCurrentRes)[ iB ] != pExpectedRes[ iB ] && ( bPass || bLogAll ) )
				{
					WriteToLog( _T( "Test case failed at (%d,%d), Subresource:%d, WSlice:%d byte %d, expected: %02x, result: %02x" ), x, y, uSubresource, uWSlice, iB, pExpectedRes[ iB ], ((BYTE*)pCurrentRes)[ iB ] );
					bPass = false;
				}
			}
		}
	}

methodDone:
	delete [ ] pExpectedRes;
	return bPass;
}
