#include "filter.h"

double f_quantize(double in, int bits);

int
CalcMipLevels(int bigSize)
{
	if (bigSize <= 0)
		return 0;
	int numLevels = 0;
	while (bigSize > 0)
	{
		numLevels += 1;
		bigSize /= 2;
	}
	return numLevels;
}

void 
CMipFilterTest::InitTestParameters()
{
	PreInitTestParams();

	int maxTexSize = max(m_nTextureSize[0], max(m_nTextureSize[1], m_nTextureSize[2]));
	m_nNumMips = (int)(log((float)maxTexSize)/log(2.0f)) + 1;

	// default sampler state
	AddParameter<bool>( _T("Default_Sampler"), &m_bDefaultSampler );
	SetParameterDefaultValue<bool>( _T("Default_Sampler"), false );
	testfactor::RFactor rDefaultSampler = AddParameterValue( _T("Default_Sampler"), true );

	// filter type
	AddParameter<D3D11_FILTER>( _T("Filter_Type"), &m_SamplerDesc.Filter );
	testfactor::RFactor rFilterType_Normal;
	testfactor::RFactor rFilterType_Comparison;
	tstring sPathFilterType = GetPath().ToString( GetPath().Depth() - 2, 1 );
	if (sPathFilterType == "Point")
	{
		rFilterType_Normal // TODO: re-enable more filtering modes
			= AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_MAG_MIP_POINT )
			+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT )
			+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT )
			;
		rFilterType_Comparison
			= AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT )
			;
	}
	else if (sPathFilterType == "Linear")
	{
		rFilterType_Normal // TODO: re-enable more filtering modes
			= AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR )
			+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR )
			+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_MAG_MIP_LINEAR )
			+ rDefaultSampler
			;
		rFilterType_Comparison
			= AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR )
			;
	}
	else if (sPathFilterType == "VS" || sPathFilterType == "GS")
	{
		rFilterType_Normal // TODO: re-enable more filtering modes
			= AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_MAG_MIP_LINEAR )
			+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR )
			+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_MAG_MIP_LINEAR )
			;
		rFilterType_Comparison
			= AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR )
			//+ AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR )
			;
	}
	else
	{
		assert(!"Invalid filter type");
	}

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
	testfactor::RFactor rTexFormat_R32F = AddParameterValue<DXGI_FORMAT>( _T("Format"), DXGI_FORMAT_R32_FLOAT );
	testfactor::RFactor rTexFormat_Default = rTexFormat_RGBA_16F;
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
	testfactor::RFactor rTexFormats_AllSizes
		= AddParameterValue( _T("Format"), DXGI_FORMAT_R32G32B32A32_FLOAT)	// 128
		+ AddParameterValue( _T("Format"), DXGI_FORMAT_R32G32B32_FLOAT)		// 96
		+ AddParameterValue( _T("Format"), DXGI_FORMAT_R16G16B16A16_FLOAT)	// 64
		+ AddParameterValue( _T("Format"), DXGI_FORMAT_R16G16_FLOAT)		// 32
		+ AddParameterValue( _T("Format"), DXGI_FORMAT_R16_FLOAT)			// 16
		+ AddParameterValue( _T("Format"), DXGI_FORMAT_R8G8_UNORM)			// 16
		+ AddParameterValue( _T("Format"), DXGI_FORMAT_R8_UNORM)			// 8
		+ AddParameterValue( _T("Format"), DXGI_FORMAT_R9G9B9E5_SHAREDEXP)	// 32?
		+ AddParameterValue( _T("Format"), DXGI_FORMAT_R8G8_B8G8_UNORM)		// 32?
		+ AddParameterValue( _T("Format"), DXGI_FORMAT_BC7_UNORM)			// 8?
		+ AddParameterValue( _T("Format"), DXGI_FORMAT_BC3_UNORM)			// 8?
		+ AddParameterValue( _T("Format"), DXGI_FORMAT_BC1_UNORM)			// 4?
		;
	testfactor::RFactor rTexFormats_DepthCompatible
		= AddParameterValue( _T("Format"), DXGI_FORMAT_R32_FLOAT)
		+ AddParameterValue( _T("Format"), DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS)
		+ AddParameterValue( _T("Format"), DXGI_FORMAT_R24_UNORM_X8_TYPELESS)
		+ AddParameterValue( _T("Format"), DXGI_FORMAT_R16_UNORM)
		;

	// texture size
	AddParameter<int>( _T("TextureSize_X"), &m_nTextureSize[0] );
	AddParameter<int>( _T("TextureSize_Y"), &m_nTextureSize[1] );
	AddParameter<int>( _T("TextureSize_Z"), &m_nTextureSize[2] );
	SetParameterDefaultValue<int>( _T("TextureSize_X"), 17 );
	SetParameterDefaultValue<int>( _T("TextureSize_Y"), 17 );
	SetParameterDefaultValue<int>( _T("TextureSize_Z"), 17 );
	testfactor::RFactor rTexSize_17 
		= AddParameterValue( _T("TextureSize_X"), 17 )
		* AddParameterValue( _T("TextureSize_Y"), 17 )
		* AddParameterValue( _T("TextureSize_Z"), 17 );

	// num mips
	AddParameter<int>( _T("Num_Mips"), &m_nNumMips );
	SetParameterDefaultValue<int>( _T("Num_Mips"), 5 );
	testfactor::RFactor rNumMips_5 = AddParameterValue( _T("Num_Mips"), 5 );

	// most-detailed mip
	AddParameter<int>( _T("Best_Mip"), &m_nMinMip );
	SetParameterDefaultValue<int>( _T("Best_Mip"), 0 );
	testfactor::RFactor rBestMip_0 = AddParameterValue( _T("Best_Mip"), 0 );

	testfactor::RFactor rBestMip_012 = AddParameterValue( _T("Best_Mip"), 0 )
									 + AddParameterValue( _T("Best_Mip"), 1 )
									 + AddParameterValue( _T("Best_Mip"), 2 );

	// minimum LOD in SamplerDesc
	AddParameter<float>( _T("Min_LOD"), &m_SamplerDesc.MinLOD );
	SetParameterDefaultValue<float>( _T("Min_LOD"), -16.0f );
	testfactor::RFactor rMinLOD_All 
		= AddParameterValue<float>( _T("Min_LOD"),-16 )
		+ AddParameterValue<float>( _T("Min_LOD"), -1 )
		+ AddParameterValue<float>( _T("Min_LOD"), -0.25 )
		+ AddParameterValue<float>( _T("Min_LOD"),  0.25 )
		+ AddParameterValue<float>( _T("Min_LOD"),  1 )
		+ AddParameterValue<float>( _T("Min_LOD"),  4 )
		;

	testfactor::RFactor rMinLOD_Few 
		= AddParameterValue<float>( _T("Min_LOD"), -0.25 )
		+ AddParameterValue<float>( _T("Min_LOD"),  1 )
		;

	// maximum LOD in SamplerDesc
	AddParameter<float>( _T("Max_LOD"), &m_SamplerDesc.MaxLOD );
	SetParameterDefaultValue<float>( _T("Max_LOD"), 256.0f );
	testfactor::RFactor rMaxLOD_All 
		= AddParameterValue<float>( _T("Max_LOD"), -0.25 )
		+ AddParameterValue<float>( _T("Max_LOD"),  0.25 )
		+ AddParameterValue<float>( _T("Max_LOD"),  1 )
		+ AddParameterValue<float>( _T("Max_LOD"),  4 )
		+ AddParameterValue<float>( _T("Max_LOD"),  8 )
		+ AddParameterValue<float>( _T("Max_LOD"), 16 )
		;
	testfactor::RFactor rMaxLOD_Few 
		= AddParameterValue<float>( _T("Max_LOD"),  1 )
		+ AddParameterValue<float>( _T("Max_LOD"),  8 )
		;

	testfactor::RFactor rMinMaxLOD_All = rMinLOD_All % rMaxLOD_All;
	testfactor::RFactor rMinMaxLOD_Few = rMinLOD_Few % rMaxLOD_Few;

	testfactor::RFactor rTexMipFactor;
	{
		int texSizes[][3] = 
		{
			{1,1,1},
			{4,4,4},
			{7,7,7},
//			{2,512,64},
//			{64,2,512},
			{512,64,2},
			{8192,1,2},
			{2,8192,1},
			{1,2,2048},
		};
		for (int nTexSize = 0; nTexSize < sizeof(texSizes)/sizeof(texSizes[0]); ++nTexSize)
		{
			if (m_nDimension == 3)
			{
				// tex3d requires only 2048 texels in U/V/W
				texSizes[nTexSize][0] = min(2048, texSizes[nTexSize][0]);
				texSizes[nTexSize][1] = min(2048, texSizes[nTexSize][1]);
				texSizes[nTexSize][2] = min(2048, texSizes[nTexSize][2]);
			}
			else if (m_nDimension == 4)
			{
				//const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(m_eFormat);
				//const int maxAllowable = (int)sqrt((128.0*1024*1024)/(6.0*pTexFormatInfo->uBitsPerElement*(1.0/8)));
				const int maxAllowable = 1024;
				int texSize = min(maxAllowable, m_nTextureSize[0]);
				m_nTextureSize[0] = texSize;
				m_nTextureSize[1] = texSize;
				m_nTextureSize[2] = 1;
			}

			if( g_App.GetIsFL9Test() )
			{
				if (m_nDimension == 2)
				{
					texSizes[nTexSize][0] = min(2048, pow2roundup( texSizes[nTexSize][0]) );
					texSizes[nTexSize][1] = min(2048, pow2roundup( texSizes[nTexSize][1]) );
					texSizes[nTexSize][2] = min(2048, pow2roundup( texSizes[nTexSize][2]) );
				}
				else if (m_nDimension == 3)
				{
					texSizes[nTexSize][0] = min(256, pow2roundup(texSizes[nTexSize][0]) );
					texSizes[nTexSize][1] = min(256, pow2roundup(texSizes[nTexSize][1]) );
					texSizes[nTexSize][2] = min(256, pow2roundup(texSizes[nTexSize][2]) );
				}
				else if (m_nDimension == 4)
				{
					texSizes[nTexSize][0] = min(512, pow2roundup(texSizes[nTexSize][0]) );
					texSizes[nTexSize][1] = min(512, pow2roundup(texSizes[nTexSize][1]) );
					texSizes[nTexSize][2] = 1;
				}
			}

			int maxTexSize = texSizes[nTexSize][0];
			if (m_nDimension > 1 && m_nDimension != 4)
				maxTexSize = max(maxTexSize, texSizes[nTexSize][1]);
			if (m_nDimension > 2 && m_nDimension != 4)
				maxTexSize = max(maxTexSize, texSizes[nTexSize][2]);
			if (m_nDimension == 4 && maxTexSize > 2048)
				continue;
			const int mipSize = CalcMipLevels(maxTexSize);
			testfactor::RFactor rTexSize_n 
				= AddParameterValue<int>( _T("TextureSize_X"), texSizes[nTexSize][0] ) 
				* AddParameterValue<int>( _T("TextureSize_Y"), texSizes[nTexSize][1] ) 
				* AddParameterValue<int>( _T("TextureSize_Z"), texSizes[nTexSize][2] );
			testfactor::RFactor rTexMipInstance 
				= rTexSize_n 
				* AddParameterValue<int>( _T("Num_Mips"), mipSize ) 
				* AddParameterValueSet( _T("Best_Mip"), RangeValueSet<int>(0, mipSize-1, 1) );
			if (rTexMipFactor.Get() != NULL)
				rTexMipFactor = rTexMipFactor + rTexMipInstance;
			else
				rTexMipFactor = rTexMipInstance;
		}
	}
	assert(rTexMipFactor.Get() != NULL);

	// min/magnification coefficient
	AddParameter<double>( _T("MinMag_Coeff"), &m_fMinMagCoeff );
	SetParameterDefaultValue<double>( _T("MinMag_Coeff"), 0.75 );
	testfactor::RFactor rMinMagCoeff = AddParameterValueSet( _T("MinMag_Coeff"), GradientValueSet<double>(-1.0, 4.0, 81) );
	testfactor::RFactor rMinMagCoeff_Neg1 = AddParameterValue( _T("MinMag_Coeff"), -1.0 );
	testfactor::RFactor rMinMagCoeff_Pt75 = AddParameterValue( _T("MinMag_Coeff"), 0.75 );
	testfactor::RFactor rMinMagCoeff_Few = AddParameterValueSet( _T("MinMag_Coeff"), GradientValueSet<double>(-1.0, 3.0, 9) );
	testfactor::RFactor rMinMagCoeff_PreciseFew = AddParameterValueSet( _T("MinMag_Coeff"), GradientValueSet<double>(-1.0, 3.0, 33) );
	testfactor::RFactor rMinMagCoeff_VeryPrecise = AddParameterValueSet( _T("MinMag_Coeff"), GradientValueSet<double>(0.0, 1.0, 129) );

	// texture address mode
	AddParameter<D3D11_TEXTURE_ADDRESS_MODE>( _T("Tex_Address_0"), &m_SamplerDesc.AddressU );
	AddParameter<D3D11_TEXTURE_ADDRESS_MODE>( _T("Tex_Address_1"), &m_SamplerDesc.AddressV );
	AddParameter<D3D11_TEXTURE_ADDRESS_MODE>( _T("Tex_Address_2"), &m_SamplerDesc.AddressW );
	SetParameterDefaultValue(_T("Tex_Address_0"), D3D11_TEXTURE_ADDRESS_WRAP );
	SetParameterDefaultValue(_T("Tex_Address_1"), D3D11_TEXTURE_ADDRESS_WRAP );
	SetParameterDefaultValue(_T("Tex_Address_2"), D3D11_TEXTURE_ADDRESS_WRAP );
	CUserValueSet<D3D11_TEXTURE_ADDRESS_MODE>* addressModeValues = new CUserValueSet<D3D11_TEXTURE_ADDRESS_MODE>();
	addressModeValues->AddValue(D3D11_TEXTURE_ADDRESS_WRAP);
	addressModeValues->AddValue(D3D11_TEXTURE_ADDRESS_MIRROR);
	addressModeValues->AddValue(D3D11_TEXTURE_ADDRESS_MIRROR_ONCE);
	addressModeValues->AddValue(D3D11_TEXTURE_ADDRESS_CLAMP);
	addressModeValues->AddValue(D3D11_TEXTURE_ADDRESS_BORDER);
	testfactor::RFactor rAddressMode0 = AddParameterValueSet( _T("Tex_Address_0"), addressModeValues );
	testfactor::RFactor rAddressMode1 = AddParameterValueSet( _T("Tex_Address_1"), addressModeValues );
	testfactor::RFactor rAddressMode2 = AddParameterValueSet( _T("Tex_Address_2"), addressModeValues );
	testfactor::RFactor rAddressMode_All = rAddressMode0 % rAddressMode1 % rAddressMode2;
	testfactor::RFactor rAddressMode_WrapAll 
		= AddParameterValue( _T("Tex_Address_0"), D3D11_TEXTURE_ADDRESS_WRAP )
		* AddParameterValue( _T("Tex_Address_1"), D3D11_TEXTURE_ADDRESS_WRAP )
		* AddParameterValue( _T("Tex_Address_2"), D3D11_TEXTURE_ADDRESS_WRAP );
	testfactor::RFactor rAddressMode_BorderAll 
		= AddParameterValue( _T("Tex_Address_0"), D3D11_TEXTURE_ADDRESS_BORDER )
		* AddParameterValue( _T("Tex_Address_1"), D3D11_TEXTURE_ADDRESS_BORDER )
		* AddParameterValue( _T("Tex_Address_2"), D3D11_TEXTURE_ADDRESS_BORDER );
	testfactor::RFactor rAddressMode_WrapBorderAll = rAddressMode_WrapAll + rAddressMode_BorderAll;

	// custom texture coords - single texel, etc
	AddParameter<bool>( _T("Custom_TexCoords"), &m_bCustomTexCoords);
	SetParameterDefaultValue<bool>( _T("Custom_TexCoords"), false );
	testfactor::RFactor rCustomTexCoords_T = AddParameterValue<bool>( _T("Custom_TexCoords"), true );
	testfactor::RFactor rCustomTexCoords_F = AddParameterValue<bool>( _T("Custom_TexCoords"), false );

	AddParameter<int>( _T("TexCoord_Number"), &m_nCustomTexCoordNumber );
	SetParameterDefaultValue<int>( _T("TexCoord_Number"), 0 );
	testfactor::RFactor rCustomTexCoord_Nbrs = AddParameterValueSet<int>( _T("TexCoord_Number"), 
		RangeValueSet<int>(0, 6+8-1, 1) );

	testfactor::RFactor rCustomCoords = rCustomTexCoords_F * rMinMagCoeff_Pt75;
	if (m_nDimension == 4)
	{
		rCustomCoords = rCustomCoords + rCustomTexCoords_T * rCustomTexCoord_Nbrs;
		rMinMagCoeff = AddParameterValueSet( _T("MinMag_Coeff"), GradientValueSet<double>(-3.5, 1.5, 11) );
	}

	// rotation angle
	AddParameter<int>( _T("TexCoord_Rotation"), &m_TexCoordRotation);
	SetParameterDefaultValue<int>( _T("TexCoord_Rotation"), 0 );
	testfactor::RFactor rTexCoordRotations 
		= AddParameterValueSet<int>( _T("TexCoord_Rotation"), RangeValueSet<int>(0, 360, 20) );

	// border color
	for (int i = 0; i < 4; ++i)
		m_SamplerDesc.BorderColor[i] = (float)s_BorderColor.f[i];
	AddParameter<float>( _T("Border_Color"), (float*)&m_SamplerDesc.BorderColor[0] );
	SetParameterDefaultValue<float>( _T("Border_Color"), 0.5 );
	testfactor::RFactor rBorderColor_All
		= AddParameterValueSet( _T("Border_Color"), GradientValueSet<float>(-1.25, 1.25, 11) );

	// sample with bias in sampler state
	AddParameter<float>( _T("Sampler_MipBias"), &m_SamplerDesc.MipLODBias);
	SetParameterDefaultValue<float>( _T("Sampler_MipBias"), 0 );
	testfactor::RFactor rSamplerMipBias 
		= AddParameterValue<float>( _T("Sampler_MipBias"),  0.0f )
		+ AddParameterValue<float>( _T("Sampler_MipBias"), -1.25f )
		+ AddParameterValue<float>( _T("Sampler_MipBias"),  1.25f )
		;

	// sample with address offset
	AddParameter<bool>( _T("Address_Offset"), &m_bSampleAOffImmI);
	SetParameterDefaultValue<bool>( _T("Address_Offset"), false );
	testfactor::RFactor rSampleAOff_T = AddParameterValue<bool>( _T("Address_Offset"), true );
	testfactor::RFactor rSampleAOff_Both = AddParameterValueSet<bool>( _T("Address_Offset"), BoolValueSet(false) );

	// center position TODO
	// pixel jitter TODO
	// rotation angle TODO

	testfactor::RFactor rCycleFilterDefaultFormat = (rFilterType_Normal * rTexFormat_Default + rFilterType_Comparison * rTexFormat_R32F);
	testfactor::RFactor rCycleFilterTypeNoFormat = (rFilterType_Normal + rFilterType_Comparison);

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
	
	if( g_App.GetIsFL9Test() )
	{
		
		CUserValueSet<DXGI_FORMAT> *pL9FormatValues = new CUserValueSet<DXGI_FORMAT>;
		// R8_UNORM supports 2D, 3D, Cube and mipmap on WDDM1.3 FL9
		pL9FormatValues->AddValue( DXGI_FORMAT_R8_UNORM );
		testfactor::RFactor rTexFormats_FL9 = AddParameterValueSet( _T("Format"), pL9FormatValues);
		
		// MinLOD
		testfactor::RFactor rMinLOD_L9 
			= AddParameterValue<float>( _T("Min_LOD"),-16 )
			+ AddParameterValue<float>( _T("Min_LOD"), -1 )
			+ AddParameterValue<float>( _T("Min_LOD"), 0 )
			+ AddParameterValue<float>( _T("Min_LOD"),  1 )
			+ AddParameterValue<float>( _T("Min_LOD"),  4 )
			;

		// MaxLOD in SamplerDesc
		testfactor::RFactor rMaxLOD_L9 
			= AddParameterValue<float>( _T("Max_LOD"), FLT_MAX );

		// FL9.1 only supports sample and sampleBias
		testfactor::RFactor rSampleFunc_FL9 = AddParameterValue<eSampleFunction>( _T("Sample_Func"), Sample ) + 
			AddParameterValue<eSampleFunction>( _T("Sample_Func"), SampleBias );

		testfactor::RFactor rMipSampleFunc = testfactor::NewPairwiseFactor( rTexMipFactor, rSampleFunc_FL9, rMinLOD_L9 * rMaxLOD_L9 );

		testfactor::RFactor rRoot = rResourceMinLOD * rTexFormats_FL9 * rFilterType_Normal * rAddressMode_WrapAll * rMipSampleFunc;
		SetRootTestFactor( rRoot );
		return;
	}

	//SetRootTestFactor(rTexFormat_RGBA_32F * rMinMagCoeff_Neg1 * (rFilterType_Normal + rFilterType_Comparison * rTexFormat_R32F) * (
	//	(rTexFormats*(rAddressMode_WrapAll+rAddressMode_BorderAll*rBorderColor_All))	
	//	+ rTexMipFactor 
	//	+ rMinMagCoeff * rMinMaxLOD_All
	//	+ rAddressMode_All 
	//	+ rTexFormats*rCustomCoords
	//	+ rSampleBias_T
	//	+ rSampleGrad_T
	//	+ rSampleLevel_T
	//	));
	SetRootTestFactor(
		rResourceMinLOD * ((rCycleFilterDefaultFormat * rSampleFunc_All * rTexSize_17 * rNumMips_5 * rBestMip_012 * rMinMaxLOD_Few) % rMinMagCoeff)
		+ rCycleFilterTypeNoFormat * (rSampleFunc_Sample + rSampleFunc_SampleCmp) * rTexSize_17 * rNumMips_5 * rBestMip_0 * rMinMagCoeff_Neg1 *
			(   rAddressMode_BorderAll * ( rTexFormats % rBorderColor_All ) 
				+ rAddressMode_WrapAll * rTexFormats_AllSizes * rCustomCoords
			) + 
		
		rCycleFilterDefaultFormat * 
			( (rSampleFunc_Sample + rSampleFunc_SampleCmp) *
				( rTexSize_17 * rNumMips_5 * rBestMip_0 *
					( rMinMagCoeff * rAddressMode_WrapAll
					+ rMinMagCoeff * rSampleAOff_T * (rAddressMode_WrapAll + rAddressMode_BorderAll)
					+ rMinMagCoeff_PreciseFew * rAddressMode_All
					+ rMinMagCoeff_VeryPrecise * rAddressMode_WrapAll
					+ rMinMagCoeff_Few * rMinMaxLOD_All * rAddressMode_WrapAll
					+ rMinMagCoeff_Few * rSampleAOff_Both *  rSamplerMipBias
					+ rMinMagCoeff_Neg1 * rTexCoordRotations * (rAddressMode_WrapAll + rAddressMode_BorderAll) 
					)
				+ rMinMagCoeff_Neg1 * rTexMipFactor 
				) + 
			 rSampleFunc_All * rTexSize_17 * rNumMips_5 * rBestMip_0 * rSampleAOff_Both *
				( rMinMagCoeff_PreciseFew % rTexCoordRotations )
			)
		);

	
	if (m_nDimension == 1)
	{
		SkipMatching( GetSkipMatchingFilter() || (
			FilterEqual(_T("TexCoord_Rotation"), 40) || FilterEqual(_T("TexCoord_Rotation"), 60)
			|| FilterEqual(_T("TexCoord_Rotation"), 120) || FilterEqual(_T("TexCoord_Rotation"), 140)
			|| FilterEqual(_T("TexCoord_Rotation"), 220) || FilterEqual(_T("TexCoord_Rotation"), 240)
			|| FilterEqual(_T("TexCoord_Rotation"), 300) || FilterEqual(_T("TexCoord_Rotation"), 320)
			) );
	} 

	
	AddPriorityPatternFilter( FilterEqual<double>(_T("MinMag_Coeff"), -1.0), WeightFactorLarge );
	AddPriorityPatternFilter( FilterEqual<double>(_T("MinMag_Coeff"), 0.0), WeightFactorLarge);
	AddPriorityPatternFilter( FilterEqual<double>(_T("MinMag_Coeff"), 1.0), WeightFactorLarge); 
	AddPriorityPatternFilter(FilterEqual<float>(_T("Resource_MinLOD"), 0.99f ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<float>(_T("Resource_MinLOD"), 4.0f ), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<float>(_T("Resource_MinLOD"), 4.01f ), WeightFactorLarge);
	
	AddPriorityPatternFilter(FilterEqual<int>(_T("TexCoord_Rotation"), 0), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<bool>(_T("Custom_TexCoords"), false), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual(_T("Format"), DXGI_FORMAT_R16G16B16A16_FLOAT), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual(_T("Format"), DXGI_FORMAT_R32_FLOAT), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<int>(_T("TextureSize_X"), 7), WeightFactorLarge);

	// The following filters are to make contingency cases not to run on Integration test pass.
	// Contingency DirectX 107196
	AddPriorityPatternFilter(FilterEqual(_T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR), WeightFactorSmall); 
	AddPriorityPatternFilter(FilterEqual(_T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR), WeightFactorSmall); 
	AddPriorityPatternFilter(FilterEqual(_T("Filter_Type"), D3D11_FILTER_COMPARISON_ANISOTROPIC), WeightFactorSmall); 

	// bug 714120 not determined Errata or Contingency
	AddPriorityPatternFilter( ( FilterEqual(_T("Filter_Type"), D3D11_FILTER_MIN_MAG_MIP_POINT) 
		|| FilterEqual(_T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT) )
		&& FilterEqual<float>(_T("Resource_MinLOD"), 0.99f )
		&& FilterEqual<double>(_T("MinMag_Coeff"), -1.0)
		&& FilterEqual<float>(_T("Min_LOD"), -0.25),
		WeightFactorSmall); 

	// Contingency DirectX 107202
	tstring sPathTexType = GetPath().ToString( GetPath().Depth() - 1, 1 );
	if ( sPathTexType == _T("Cube"))
	{
		AddPriorityPatternFilter( FilterGreater(_T("Best_Mip"), 0 ), WeightFactorSmall ); 
	}
	
	DeclarePriorityLevelByPriorityValue( 1, 3.375f );
	DeclarePriorityLevelByPriorityValue( 2, 2.25f );


	/* We are now using PriorityPatternFilter to define test cases for RI test runs. WGFTEST_MODE_BVT is not used any more.
	if (g_App.GetTestMode() == WGFTEST_MODE_BVT)
	{
		SkipMatching(GetSkipMatchingFilter() || (
				(  FilterNotEqual<double>(_T("MinMag_Coeff"), -1.0) 
				&& FilterNotEqual<double>(_T("MinMag_Coeff"),  0.0) 
				&& FilterNotEqual<double>(_T("MinMag_Coeff"),  1.0))
			|| FilterNotEqual<int>(_T("TexCoord_Rotation"), 0)
			|| FilterNotEqual<bool>(_T("Custom_TexCoords"), false)
			|| FilterLess<int>(_T("TextureSize_X"), 17)
			|| FilterGreater<int>(_T("TextureSize_X"), 32)
			|| FilterLess<float>(_T("Border_Color"), 0.75)
			|| FilterGreater<float>(_T("Border_Color"), 1.25)
			|| (FilterNotEqual(_T("Format"), DXGI_FORMAT_R16G16B16A16_FLOAT) && FilterNotEqual(_T("Format"), DXGI_FORMAT_R32_FLOAT))
			|| (FilterNotEqual<float>(_T("Sampler_MipBias"), 0) && FilterNotEqual<double>(_T("MinMag_Coeff"), 1.0))
			|| FilterEqual(_T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR) // G80 contingency
			|| FilterEqual(_T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR) // G80 contingency
			|| FilterEqual(_T("Filter_Type"), D3D11_FILTER_COMPARISON_ANISOTROPIC) // G80 contingency
			));
	}
	*/
	
}

TEST_RESULT 
CMipFilterTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = E_FAIL;
	ID3D11Resource* pTexRsc = NULL;
	ID3D11ShaderResourceView* pSRView = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = m_eFormat;

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

	eTexPattern texPattern = TexPattern_Checker;
	switch(m_eFormat)
	{
	default:
		break;
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		texPattern = TexPattern_Gradient;
		break;
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
		break;
	}

	hr = CreateTexturePattern(&pTexRsc, texPattern);
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
	if( !g_App.GetIsFL9Test() )
	{
		GetEffectiveContext()->VSSetShaderResources(0, 1, &pSRView);
		GetEffectiveContext()->GSSetShaderResources(0, 1, &pSRView);
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
		tRes = TestDrawnPixels(m_vTexCoords);

	{
		// clear out the resource bindings
		ID3D11ShaderResourceView* nullSRVArr[1] = {NULL};
		if( !g_App.GetIsFL9Test() )
		{
			GetEffectiveContext()->VSSetShaderResources(0, 1, nullSRVArr);
			GetEffectiveContext()->GSSetShaderResources(0, 1, nullSRVArr);
		}
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

