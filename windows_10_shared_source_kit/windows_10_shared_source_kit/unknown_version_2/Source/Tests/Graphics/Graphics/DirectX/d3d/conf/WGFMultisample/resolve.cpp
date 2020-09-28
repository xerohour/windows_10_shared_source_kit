ffectiveContext() failed");
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
	m_pRTView->GetResource( &pRes );	// A