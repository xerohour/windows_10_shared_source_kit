dds Ref
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
	if( m_Res