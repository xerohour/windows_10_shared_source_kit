ourceType == D3D_RESOURCE_SCENARIO_TEXTURE1D || m_ResourceType == D3D_RESOURCE_SCENARIO_BUFFER ) 
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
			LogError( __FILEW__, __LINE__, L"Planar video format is used, but the RTV format is not UINT or UNORM type.", false, ERRORCODE_TYPE_BOOL, 