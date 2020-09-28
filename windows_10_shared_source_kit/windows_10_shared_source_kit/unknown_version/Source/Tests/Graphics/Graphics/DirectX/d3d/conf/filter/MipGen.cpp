#include "filter.h"

// TODO: more advanced Texture3D cases to isolate the Z dimension
// TODO: more advanced TextureCube cases to expose all faces, edges, and corners
// TODO: arrays of textures

void 
CMipGenTest::InitTestParameters()
{
	PreInitTestParams();

	int maxSize = max(m_nPrimitiveW, m_nPrimitiveH);

	maxSize = 31;

	maxSize = 31;

	m_nTextureSize[0] = maxSize;
	m_nTextureSize[1] = max(1,maxSize/2);
	m_nTextureSize[2] = 4;

	m_nMinMip = 0;

	m_SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

	m_SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	m_SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	m_SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	m_SampleFunction = Sample;

	// array size
	AddParameter<int>( _T("Array_Size"), &m_nArraySize );
	SetParameterDefaultValue<int>( _T("Array_Size"), 0 );
	testfactor::RFactor rArraySize_All = AddParameterValueSet( _T("Array_Size"), 
		RangeValueSet<int>(0,2,1) );
	if (m_nDimension >= 3)
		rArraySize_All = AddParameterValue<int>( _T("Array_Size"), 0 );

	// texture format
	AddParameter<DXGI_FORMAT>( _T("Format"), &m_eFormat );
	testfactor::RFactor rTexFormats = AddParameterValueSet( _T("Format"), 
		FormatValueSet(FF_FLOAT|FF_UNORM|FF_SNORM|FF_SHAREDEXP, FF_D|FF_VIDEO));

	// texture size
	AddParameter<int>( _T("TextureSize_X"), &m_nTextureSize[0] );
	AddParameter<int>( _T("TextureSize_Y"), &m_nTextureSize[1] );
	AddParameter<int>( _T("TextureSize_Z"), &m_nTextureSize[2] );
	SetParameterDefaultValue<int>( _T("TextureSize_X"), 31 );
	SetParameterDefaultValue<int>( _T("TextureSize_Y"), 15 );
	SetParameterDefaultValue<int>( _T("TextureSize_Z"), 5 );
	testfactor::RFactor rTexSizes
		=
		/*( AddParameterValue<int>( _T("TextureSize_X"), 31 )
		% AddParameterValue<int>( _T("TextureSize_Y"), 15 )
		% AddParameterValue<int>( _T("TextureSize_Z"), 7 ))
		+ disabled since mipgen is not clearly spec'd. only pow2 will give somewhat-reliable results.*/
		( AddParameterValue<int>( _T("TextureSize_X"), 32 )
		% AddParameterValue<int>( _T("TextureSize_Y"), 16 )
		% AddParameterValue<int>( _T("TextureSize_Z"), 8 ));

	AddParameter<D3D11_FILTER>( _T("Filter_Type"), &m_SamplerDesc.Filter );
	testfactor::RFactor rFilterType_Normal = AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_MAG_MIP_POINT );

	// center position TODO
	// pixel jitter TODO
	// rotation angle TODO

	SetRootTestFactor( rArraySize_All * rTexFormats * rTexSizes * rFilterType_Normal );
}



TEST_RESULT 
CMipGenTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = E_FAIL;
	ID3D11Resource* pTexRsc = NULL;
	ID3D11ShaderResourceView* pSRView = NULL;
	ID3D11ShaderResourceView* pSRViewTemp = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	m_sVertexShaderName = _T("vs.vsh");
	m_sGeometryShaderName = _T("");
	m_sPixelShaderName = _T("ps_basic.psh");

	m_sVSEntryPt = _T("main");
	m_sPSEntryPt = _T("main_");
	m_sPSEntryPt += (TCHAR)('0' + m_nDimension);
	if (m_nArraySize > 0)
		m_sPSEntryPt += (TCHAR)('a');

	switch (m_nDimension)
	{
	case 1: // 1D
		if (m_nArraySize > 0)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
			srvDesc.Texture1DArray.MipLevels = -1;
			srvDesc.Texture1DArray.MostDetailedMip = 0;
			srvDesc.Texture1DArray.FirstArraySlice = 0;
			srvDesc.Texture1DArray.ArraySize = m_nArraySize;
		}
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
			srvDesc.Texture1D.MipLevels = -1;
			srvDesc.Texture1D.MostDetailedMip = 0;
		}
		break;
	case 2: // 2D
		if (m_nArraySize > 0)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.MipLevels = -1;
			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
			srvDesc.Texture2DArray.ArraySize = m_nArraySize;
		}
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = -1;
			srvDesc.Texture2D.MostDetailedMip = 0;
		}
		break;
	case 3: // 3D
		assert(m_nArraySize == 0);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		srvDesc.Texture3D.MipLevels = -1;
		srvDesc.Texture3D.MostDetailedMip = 0;
		break;
	case 4: // Cube
		assert(m_nArraySize == 0);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = -1;
		srvDesc.TextureCube.MostDetailedMip = 0;
		{
			const int maxDimension = max(m_nTextureSize[0], m_nTextureSize[1]);
			m_nTextureSize[0] = maxDimension;
			m_nTextureSize[1] = maxDimension;
			m_nTextureSize[2] = 1;
		}
		break;
	}

	hr = UpdateResources();
	if (FAILED(hr))
	{
		tRes = RESULT_FAIL;
		goto testDone;
	}

	int maxTexSize = m_nTextureSize[0];
	if (m_nDimension > 1)
		maxTexSize = max(maxTexSize, m_nTextureSize[1]);
	if (m_nDimension > 2)
		maxTexSize = max(maxTexSize, m_nTextureSize[2]);
	m_nNumMips = (int)(log((float)maxTexSize)/log(2.0f)) + 1;

	if (!CreateTestMips(m_nNumMips, m_nTextureSize))
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("%s CreateTestMips failed."), LOG_HEADER );
		goto testDone;
	}

	hr = CreateTexturePattern(&pTexRsc, TexPattern_Gradient, false, true);
	if (FAILED(hr))
	{
		tRes = RESULT_FAIL;
		goto testDone;
	}
	if (hr == S_FALSE)
	{
		tRes = RESULT_SKIP;
		goto testDone;
	}

	hr = m_pDevice->CreateShaderResourceView(pTexRsc, &srvDesc, &pSRView);
	if (FAILED(hr))
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("%s CreateShaderResourceView failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
		goto testDone;
	}

	GetEffectiveContext()->GenerateMips(pSRView);

	if( FAILED(hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog(_T("ExecuteEffectiveContext() failed"));
		tRes = RESULT_FAIL;
	}

	{
		ID3D11Resource* pTexCopy = NULL;
		hr = CreateTextureNoPattern(&pTexCopy, true);
		if (FAILED(hr))
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T("%s CreateTextureNoPattern failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
			goto testDone;
		}
		m_pDeviceContext->CopyResource(pTexCopy, pTexRsc);
		tRes = TestMipGenResult(pTexCopy);
		SAFE_RELEASE(pTexCopy);
	}

	{
		memset(&m_vTexCoords, 0, sizeof(m_vTexCoords));
		m_vTexCoords[0].z = 1;
		m_vTexCoords[1].x = 1;
		m_vTexCoords[1].z = 1;
		m_vTexCoords[2].x = 1;
		m_vTexCoords[2].y = 1;
		m_vTexCoords[3].y = 1;
	}

	const UINT arraySize = (m_nArraySize>0)?m_nArraySize:1;
	for (UINT arrayNum = 0; arrayNum < arraySize; ++arrayNum)
	{
		for (m_nMinMip = m_nNumMips-1; m_nMinMip >= 0; --m_nMinMip)
		{
			switch (m_nDimension)
			{
			case 1: // 1D
				if (m_nArraySize > 0)
				{
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
					srvDesc.Texture1DArray.MipLevels = 1;
					srvDesc.Texture1DArray.MostDetailedMip = m_nMinMip;
					srvDesc.Texture1DArray.FirstArraySlice = arrayNum;
					srvDesc.Texture1DArray.ArraySize = 1;
				}
				else
				{
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
					srvDesc.Texture1D.MipLevels = 1;
					srvDesc.Texture1D.MostDetailedMip = m_nMinMip;
				}
				break;
			case 2: // 2D
				if (m_nArraySize > 0)
				{
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
					srvDesc.Texture2DArray.MipLevels = 1;
					srvDesc.Texture2DArray.MostDetailedMip = m_nMinMip;
					srvDesc.Texture2DArray.FirstArraySlice = arrayNum;
					srvDesc.Texture2DArray.ArraySize = 1;
				}
				else
				{
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Texture2D.MipLevels = 1;
					srvDesc.Texture2D.MostDetailedMip = m_nMinMip;
				}
				break;
			case 3: // 3D
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
				srvDesc.Texture3D.MipLevels = 1;
				srvDesc.Texture3D.MostDetailedMip = m_nMinMip;
				break;
			case 4: // Cube
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
				srvDesc.TextureCube.MipLevels = 1;
				srvDesc.TextureCube.MostDetailedMip = m_nMinMip;
				break;
			}
			hr = m_pDevice->CreateShaderResourceView(pTexRsc, &srvDesc, &pSRViewTemp);
			if (FAILED(hr))
			{
				//tRes = RESULT_FAIL;
				WriteToLog( _T("%s CreateShaderResourceView failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
				goto testDone;
			}

			// draw the textured quad
			UpdateResources();
			GetEffectiveContext()->PSSetShaderResources(0, 1, &pSRViewTemp);
			if ( FAILED(DrawQuad()) )
			{
				//tRes = RESULT_FAIL;
				WriteToLog( _T("%s DrawQuad failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
				goto testDone;
			}

			{
				// clear out the resource bindings
				ID3D11ShaderResourceView* nullSRVArr[1] = {NULL};
				GetEffectiveContext()->PSSetShaderResources(0, 1, nullSRVArr);
				if( FAILED(hr = ExecuteEffectiveContext() ) )
				{
					WriteToLog(_T("ExecuteEffectiveContext() failed"));
					tRes = RESULT_FAIL;
				}
				SAFE_RELEASE(pSRViewTemp);
			}

			// test that things came out as expected
			/*
			{
				TEST_RESULT tResTemp = TestDrawnPixels(m_vTexCoords, 0.125f);
				if (tRes == RESULT_PASS)
					tRes = tResTemp;
			}
			*/

			if (g_App.VisibleRender())
				DoVisibleRender();
			DoShowResources();
		}
	}

testDone:
	{
		// clear out the resource bindings
		ID3D11ShaderResourceView* nullSRVArr[1] = {NULL};
		GetEffectiveContext()->PSSetShaderResources(0, 1, nullSRVArr);
		if( FAILED(hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog(_T("ExecuteEffectiveContext() failed"));
			tRes = RESULT_FAIL;
		}
	}

	SAFE_RELEASE(pTexRsc);
	SAFE_RELEASE(pSRView);
	SAFE_RELEASE(pSRViewTemp);
	return tRes;
}

