#include "filter.h"

// TODO: vary anisotropy ratio in each dimension

double f_quantize(double in, int bits);

void 
CAnisoFilterTest::InitTestParameters()
{
	PreInitTestParams();

	int maxTexSize = max(m_nTextureSize[0], max(m_nTextureSize[1], m_nTextureSize[2]));
	m_nNumMips = (int)(log((float)maxTexSize)/log(2.0f)) + 1;

	// filter type
	AddParameter<D3D11_FILTER>( _T("Filter_Type"), &m_SamplerDesc.Filter );
	testfactor::RFactor rFilterType_Normal = AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_ANISOTROPIC ) 
											 +
											 AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MINIMUM_ANISOTROPIC )
											 +
											 AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MAXIMUM_ANISOTROPIC )
											 ;
	testfactor::RFactor rFilterType_Comparison = AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_ANISOTROPIC );

	// sample function
	AddParameter<eSampleFunction>( _T("Sample_Func"), &m_SampleFunction );
	SetParameterDefaultValue<eSampleFunction>( _T("Sample_Func"), Sample );
	testfactor::RFactor rSampleFunc_Sample = AddParameterValue<eSampleFunction>( _T("Sample_Func"), Sample );
	testfactor::RFactor rSampleFunc_SampleCmp = AddParameterValue<eSampleFunction>( _T("Sample_Func"), SampleCmp );
	CUserValueSet<eSampleFunction>* pSampleFuncs = new CUserValueSet<eSampleFunction>();
	for (int n = Sample; n < SampleFuncMaxVal; ++n)
		pSampleFuncs->AddValue((eSampleFunction)(n));
	testfactor::RFactor rSampleFunc_All = AddParameterValueSet<eSampleFunction>( _T("Sample_Func"), pSampleFuncs );

	// texture format
	AddParameter<DXGI_FORMAT>( _T("Format"), &m_eFormat );
	SetParameterDefaultValue( _T("Format"), DXGI_FORMAT_R16G16B16A16_FLOAT );
	testfactor::RFactor rTexFormat_RGBA_16F = AddParameterValue<DXGI_FORMAT>( _T("Format"), DXGI_FORMAT_R16G16B16A16_FLOAT );
	testfactor::RFactor rTexFormat_R_32F = AddParameterValue<DXGI_FORMAT>( _T("Format"), DXGI_FORMAT_R32_FLOAT );
	CFilterHandleCommonSource<DXGI_FORMAT> formatFilter = (
		FilterRegExp<DXGI_FORMAT>( _T( "FLOAT" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "UNORM" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "SNORM" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "SHAREDEXP" ) ) 
		) && !(
		FilterRegExp<DXGI_FORMAT>( _T( "D32" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "D24" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "D16" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "B5G6R5" ) ) ||
		FilterRegExp<DXGI_FORMAT>( _T( "B5G5R5A1" ) ) 
		);
	testfactor::RFactor rTexFormats = AddParameterValueSet( _T("Format"), FormatValueSet( formatFilter ) );

	// texture size
	AddParameter<int>( _T("TextureSize_X"), &m_nTextureSize[0] );
	AddParameter<int>( _T("TextureSize_Y"), &m_nTextureSize[1] );
	AddParameter<int>( _T("TextureSize_Z"), &m_nTextureSize[2] );
	SetParameterDefaultValue<int>( _T("TextureSize_X"), 7 );
	SetParameterDefaultValue<int>( _T("TextureSize_Y"), 7 );
	SetParameterDefaultValue<int>( _T("TextureSize_Z"), 7 );
	testfactor::RFactor rTexSize_7
		= AddParameterValue<int>( _T("TextureSize_X"), 7 )
		* AddParameterValue<int>( _T("TextureSize_Y"), 7 )
		* AddParameterValue<int>( _T("TextureSize_Z"), 7 );
	// TODO: expand the range of texture sizes used for aniso filtering
	testfactor::RFactor rTexSizes = rTexSize_7;

	// min/magnification coefficient
	AddParameter<double>( _T("MinMag_Coeff"), &m_fMinMagCoeff );
	SetParameterDefaultValue<double>( _T("MinMag_Coeff"), -1.0 );
	testfactor::RFactor rMinMagCoeff_Neg1 = AddParameterValue( _T("MinMag_Coeff"), -1.0 );
	testfactor::RFactor rMinMagCoeff = AddParameterValueSet( _T("MinMag_Coeff"), GradientValueSet<double>(-4.0, 4.0, 33) );

	// texture address mode
	AddParameter<D3D11_TEXTURE_ADDRESS_MODE>( _T("Tex_Address_0"), &m_SamplerDesc.AddressU );
	AddParameter<D3D11_TEXTURE_ADDRESS_MODE>( _T("Tex_Address_1"), &m_SamplerDesc.AddressV );
	AddParameter<D3D11_TEXTURE_ADDRESS_MODE>( _T("Tex_Address_2"), &m_SamplerDesc.AddressW );
	SetParameterDefaultValue(_T("Tex_Address_0"), D3D11_TEXTURE_ADDRESS_CLAMP );
	SetParameterDefaultValue(_T("Tex_Address_1"), D3D11_TEXTURE_ADDRESS_CLAMP );
	SetParameterDefaultValue(_T("Tex_Address_2"), D3D11_TEXTURE_ADDRESS_CLAMP );
	CUserValueSet<D3D11_TEXTURE_ADDRESS_MODE>* addressModeValues = new CUserValueSet<D3D11_TEXTURE_ADDRESS_MODE>();
	addressModeValues->AddValue(D3D11_TEXTURE_ADDRESS_WRAP);
	addressModeValues->AddValue(D3D11_TEXTURE_ADDRESS_MIRROR);
	addressModeValues->AddValue(D3D11_TEXTURE_ADDRESS_MIRROR_ONCE);
	addressModeValues->AddValue(D3D11_TEXTURE_ADDRESS_CLAMP);
	addressModeValues->AddValue(D3D11_TEXTURE_ADDRESS_BORDER);
	testfactor::RFactor rAddressMode0 = AddParameterValueSet( _T("Tex_Address_0"), addressModeValues );
	testfactor::RFactor rAddressMode1 = AddParameterValueSet( _T("Tex_Address_1"), addressModeValues );
	testfactor::RFactor rAddressMode2 = AddParameterValueSet( _T("Tex_Address_2"), addressModeValues );
	testfactor::RFactor rAddressMode_All 
		= rAddressMode0 + rAddressMode1 + rAddressMode2
		+ (rAddressMode0 % rAddressMode1 % rAddressMode2);
	testfactor::RFactor rAddressMode_WrapAll 
		= AddParameterValue( _T("Tex_Address_0"), D3D11_TEXTURE_ADDRESS_WRAP )
		* AddParameterValue( _T("Tex_Address_1"), D3D11_TEXTURE_ADDRESS_WRAP )
		* AddParameterValue( _T("Tex_Address_2"), D3D11_TEXTURE_ADDRESS_WRAP );
	testfactor::RFactor rAddressMode_BorderAll 
		= AddParameterValue( _T("Tex_Address_0"), D3D11_TEXTURE_ADDRESS_BORDER )
		* AddParameterValue( _T("Tex_Address_1"), D3D11_TEXTURE_ADDRESS_BORDER )
		* AddParameterValue( _T("Tex_Address_2"), D3D11_TEXTURE_ADDRESS_BORDER );

	// custom texture coords - single texel, etc
	AddParameter<bool>( _T("Custom_TexCoords"), &m_bCustomTexCoords);
	SetParameterDefaultValue<bool>( _T("Custom_TexCoords"), false );
	testfactor::RFactor rCustomTexCoords_T = AddParameterValue<bool>( _T("Custom_TexCoords"), true );
	testfactor::RFactor rCustomTexCoords_F = AddParameterValue<bool>( _T("Custom_TexCoords"), false );

	AddParameter<int>( _T("TexCoord_Number"), &m_nCustomTexCoordNumber );
	SetParameterDefaultValue<int>( _T("TexCoord_Number"), 0 );

	testfactor::RFactor rCustomCoords = rCustomTexCoords_F;
	if (m_nDimension == 4)
	{
		testfactor::RFactor rCustomTexCoord_Nbrs = AddParameterValueSet<int>( _T("TexCoord_Number"), 
			RangeValueSet<int>(0, 6+8-1, 1) );
		rCustomCoords = rCustomCoords + rCustomTexCoords_T * rCustomTexCoord_Nbrs;
		rMinMagCoeff = AddParameterValueSet( _T("MinMag_Coeff"), GradientValueSet<double>(-3.5, 1.5, 11) );
	}
	else
	{
		testfactor::RFactor rCustomTexCoord_Nbrs = AddParameterValueSet<int>( _T("TexCoord_Number"), 
			RangeValueSet<int>(0, 2, 1) );
		rCustomCoords 
			= rCustomCoords 
			+ rCustomTexCoords_T * rCustomTexCoord_Nbrs * rTexSizes * rAddressMode_BorderAll;
	}

	// border color
	for (int i = 0; i < 4; ++i)
		m_SamplerDesc.BorderColor[i] = (float)s_BorderColor.f[i];
	AddParameter<float>( _T("Border_Color"), (float*)&m_SamplerDesc.BorderColor[0] );
	SetParameterDefaultValue<float>( _T("Border_Color"), 0.5 );
	testfactor::RFactor rBorderColor_All
		= AddParameterValueSet( _T("Border_Color"), GradientValueSet<float>(-1.25, 1.25, 11) );

	// most-detailed mip
	AddParameter<int>( _T("Best_Mip"), &m_nMinMip );
	SetParameterDefaultValue<int>( _T("Best_Mip"), 0 );
	testfactor::RFactor rBestMip = AddParameterValueSet( _T("Best_Mip"), RangeValueSet(0, m_nNumMips-1, 1));

	AddParameter<float>( _T("Aniso_Morph"), &m_fAnisoMorph);
	SetParameterDefaultValue<float>( _T("Aniso_Morph"), 0.0 );
	testfactor::RFactor rAnisoMorph = AddParameterValueSet( _T("Aniso_Morph"), 
		GradientValueSet<float>(-4.0, 4.0, 5) );

	AddParameter<UINT>( _T("Max_Aniso"), &m_SamplerDesc.MaxAnisotropy );
	SetParameterDefaultValue<UINT>( _T("Max_Aniso"), 16 );
	CUserValueSet<UINT>* pAnisoVals = new CUserValueSet<UINT>();
	pAnisoVals->AddValue(1);
	// TODO: cycle on more max anisotropy values
	pAnisoVals->AddValue(2);
	//pAnisoVals->AddValue(4);
	//pAnisoVals->AddValue(8);
	//pAnisoVals->AddValue(16);
	testfactor::RFactor rMaxAniso = AddParameterValueSet<UINT>( _T("Max_Aniso"), pAnisoVals );

	// default sampler state
	AddParameter<bool>( _T("Default_Sampler"), &m_bDefaultSampler );
	SetParameterDefaultValue<bool>( _T("Default_Sampler"), false );
	testfactor::RFactor rDefault_F = AddParameterValue( _T("Default_Sampler"), false );

	// center position TODO
	// pixel jitter TODO
	// rotation angle TODO

	testfactor::RFactor rCycleFilterDefaultFormat 
		= (rFilterType_Normal * rTexFormat_RGBA_16F * rSampleFunc_Sample + rFilterType_Comparison * rTexFormat_R_32F * rSampleFunc_SampleCmp);
	testfactor::RFactor rCycleFilterTypeNoFormat = (rFilterType_Normal * rSampleFunc_Sample + rFilterType_Comparison * rSampleFunc_SampleCmp);

	AddParameter<float>( _T("Resource_MinLOD"), &m_fResourceMinLOD );
	SetParameterDefaultValue<float>( _T("Resource_MinLOD"), 0.0f );
	testfactor::RFactor rResourceMinLOD;

	// SetResourceMinLOD is only supported in feature level 11.0 and beyond
	if (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
	{
		rResourceMinLOD = AddParameterValue<float>( _T("Resource_MinLOD"), -0.01f ) 
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 0.0f ) 
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 0.49f ) 
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 0.5f ) 
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 0.51f ) 
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 0.99f ) 
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 1.0f )
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 1.5f )
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 2.0f )
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 3.99f )
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 4.0f )  // when NumMips = 5, this will NOT produce out-of-bounds results
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 4.01f ) // when NumMips = 5, this will produce out-of-bounds results
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 4.99f )
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 5.0f )
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 5.01f )
						+ AddParameterValue<float>( _T("Resource_MinLOD"), 6.0f )
						+ AddParameterValue<float>( _T("Resource_MinLOD"), FLT_MAX );
	}
	else
	{
		rResourceMinLOD = AddParameterValue<float>( _T("Resource_MinLOD"), 0.0f );
	}

	SetRootTestFactor(rMinMagCoeff_Neg1 * rDefault_F * 
		( rCycleFilterDefaultFormat *
			( rBestMip * rAnisoMorph
			+ rMinMagCoeff * (rAddressMode_All + rResourceMinLOD)
			+ rTexSizes
			+ rCustomCoords
			)
		+ (rCycleFilterTypeNoFormat * rTexFormats*(rAddressMode_WrapAll+rAddressMode_BorderAll*rBorderColor_All))	
		+ (rFilterType_Normal * rTexFormat_RGBA_16F + rFilterType_Comparison * rTexFormat_R_32F) * rSampleFunc_All
		) * rMaxAniso );

	AddPriorityPatternFilter(FilterEqual<double>(_T("MinMag_Coeff"), -1.0), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<double>(_T("MinMag_Coeff"), 0.0), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<double>(_T("MinMag_Coeff"), 1.0), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<bool>(_T("Custom_TexCoords"), false), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual(_T("Format"), DXGI_FORMAT_R16G16B16A16_FLOAT), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual(_T("Format"), DXGI_FORMAT_R32_FLOAT), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<int>(_T("TextureSize_X"), 7), WeightFactorLarge);
	// The following filters are to make contingency cases not to run on Integration test pass.
	// Contingency DirectX 107196
	AddPriorityPatternFilter(FilterEqual(_T("Filter_Type"), D3D11_FILTER_COMPARISON_ANISOTROPIC), WeightFactorSmall); 
	AddPriorityPatternFilter(FilterEqual(_T("Sample_Func"), SampleCmp)|| FilterEqual(_T("Sample_Func"), SampleCmpLvlZero),
		WeightFactorSmall); 

	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );

	/* We are now using PriorityPatternFilter to define test cases for RI test runs. WGFTEST_MODE_BVT is not used any more.
	if (g_App.GetTestMode() == WGFTEST_MODE_BVT)
	{
		SkipMatching(GetSkipMatchingFilter() || (
				(  FilterNotEqual<double>(_T("MinMag_Coeff"), -1.0) 
				&& FilterNotEqual<double>(_T("MinMag_Coeff"),  0.0) 
				&& FilterNotEqual<double>(_T("MinMag_Coeff"),  1.0))
			|| FilterNotEqual<bool>(_T("Custom_TexCoords"), false)
			|| FilterLess<float>(_T("Border_Color"), 0.75)
			|| FilterGreater<float>(_T("Border_Color"), 1.25)
			|| (FilterNotEqual(_T("Format"), DXGI_FORMAT_R16G16B16A16_FLOAT) && FilterNotEqual(_T("Format"), DXGI_FORMAT_R32_FLOAT))
			|| FilterEqual(_T("Filter_Type"), D3D11_FILTER_COMPARISON_ANISOTROPIC) // G80 contingency
			));
	}
	*/
}

TEST_RESULT 
CAnisoFilterTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = E_FAIL;
	ID3D11Resource* pTexRsc = NULL;
	ID3D11ShaderResourceView* pSRView = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	hr = UpdateResources();
	if (FAILED(hr))
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("%s UpdateResources failed."), LOG_HEADER );
		goto testDone;
	}

	if (!CreateTestMips(m_nNumMips, m_nTextureSize))
	{
		tRes = RESULT_SKIP;
		WriteToLog( _T("%s CreateTestMips failed."), LOG_HEADER );
		goto testDone;
	}

	hr = CreateTexturePattern(&pTexRsc, TexPattern_Checker);
	if (FAILED(hr))
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("%s CreateTexturePattern failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
		goto testDone;
	}
	if (hr == S_FALSE)
	{
		tRes = RESULT_SKIP;
		goto testDone;
	}

	switch (m_nDimension)
	{
	case 1: // 1D
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		srvDesc.Texture1D.MipLevels = m_nNumMips - m_nMinMip;
		srvDesc.Texture1D.MostDetailedMip = m_nMinMip;
		break;
	case 2: // 2D
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = m_nNumMips - m_nMinMip;
		srvDesc.Texture2D.MostDetailedMip = m_nMinMip;
		break;
	case 3: // 3D
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		srvDesc.Texture3D.MipLevels = m_nNumMips - m_nMinMip;
		srvDesc.Texture3D.MostDetailedMip = m_nMinMip;
		break;
	case 4: // Cube
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = m_nNumMips - m_nMinMip;
		srvDesc.TextureCube.MostDetailedMip = m_nMinMip;
		break;
	}

	hr = m_pDevice->CreateShaderResourceView(pTexRsc, &srvDesc, &pSRView);
	if (FAILED(hr))
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("%s CreateShaderResourceView failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
		goto testDone;
	}
	GetEffectiveContext()->PSSetShaderResources(0, 1, &pSRView);

	GenerateTextureCoords();

	// draw the textured quad
	if ( FAILED(DrawQuad()) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("%s DrawQuad failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
		goto testDone;
	}

	// test that things came out as expected
	if (tRes == RESULT_PASS)
	{
		// TODO: implement anisotropic TestPixels - this is just dummy coverage until then
		if (m_SamplerDesc.MaxAnisotropy == 1)
			tRes = TestDrawnPixels(m_vTexCoords);
		else
		{
			WriteToLog( _T("%s Anisotropic texture filtering not tested with MaxAniso > 1."), LOG_HEADER );
			tRes = RESULT_SKIP;
			SkipMatching( GetSkipMatchingFilter() || FilterEqual(_T("Max_Aniso"), m_SamplerDesc.MaxAnisotropy) );
		}
	}

	{
		// clear out the resource bindings
		ID3D11ShaderResourceView* nullSRVArr[1] = {NULL};
		GetEffectiveContext()->PSSetShaderResources(0, 1, nullSRVArr);
	}

	if (g_App.VisibleRender())
		DoVisibleRender();
	DoShowResources();

testDone:
	SAFE_RELEASE(pTexRsc);
	SAFE_RELEASE(pSRView);
	return tRes;
}

