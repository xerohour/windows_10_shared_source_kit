    #include "filter.h"

// DONE: non-pow2 texture sizes, texture patterns to identify sampling mistakes
// DONE: texture size parameter variation
// TODO: more advanced Texture3D cases to isolate the Z dimension
// DONE: more advanced TextureCube cases to expose all faces, edges, and corners
// TODO: sample at extremes of valid texture coordinate range to ensure proper filtering
// TODO: arrays of textures
// DONE: fixed derivative (sample_d)
// DONE: fixed LOD (sample_l)
// DONE: shader LOD bias (sample_b)
// DONE: sampler LOD bias (samplerState.MipLODBias)
// DONE: comparison filter (sample_c)

float
f_getINF(bool neg)
{
    UINT32 dwINFVal = 0x7f800000; // QNaN
    if (neg)
        dwINFVal |= 0x80000000;
    return *(float*)(&dwINFVal);
}


void 
CBasicFilterTest::InitTestParameters()
{
    PreInitTestParams();

    m_nNumMips = 1;
    m_nMinMip = 0;

    // default sampler state
    AddParameter<bool>( _T("Default_Sampler"), &m_bDefaultSampler );
    SetParameterDefaultValue<bool>( _T("Default_Sampler"), false );
    testfactor::RFactor rDefaultSampler = AddParameterValue( _T("Default_Sampler"), true );

    // TODO: decide if the mip_linear cases are necessary (they double the number of testcases)
    // filter type
    AddParameter<D3D11_FILTER>( _T("Filter_Type"), &m_SamplerDesc.Filter );
    testfactor::RFactor rFilterType_Normal;
    testfactor::RFactor rFilterType_Comparison;
    tstring sPath = GetPath().ToString( GetPath().Depth() - 2, 1 );
    if (sPath == "Point")
    {
        rFilterType_Normal 
            = 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_MAG_MIP_POINT )
            +
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT )
            +
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT )
            //+ 
            //AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR )
            ;
        rFilterType_Comparison 
            = 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT )
            //+ 
            //AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR )
            ;
    }
    else if (sPath == "Linear")
    {
        rFilterType_Normal 
            = 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT )
            +
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT )
            +
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT )
            //+ 
            //AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_MAG_MIP_LINEAR )
            +
            rDefaultSampler
            ;
        rFilterType_Comparison 
            = 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT )
            //+ 
            //AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR )
            ;
    }
    else if (sPath == "Mixed")
    {
        rFilterType_Normal 
            = 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT )
            //+ 
            //AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT )
            //+ 
            //AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR )
            +
            rDefaultSampler
            ;
        rFilterType_Comparison 
            = 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT )
            //+ 
            //AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT )
            //+ 
            //AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR )
            ;
    }
    else if (sPath == "Depth")
    {
        rFilterType_Normal 
            = 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_MAG_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT )
            +
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT )
            +
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT )
            +
            rDefaultSampler
            ;
        rFilterType_Comparison 
            = 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT )
            + 
            AddParameterValue<D3D11_FILTER>( _T("Filter_Type"), D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT )
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

    if (sPath == "Depth")
    {
        SetParameterDefaultValue( _T("Format"), DXGI_FORMAT_R32_FLOAT );
        rTexFormat_Default = rTexFormat_R32F;
        rTexFormats = rTexFormats_DepthCompatible;
        rTexFormats_AllSizes = rTexFormats_DepthCompatible;
    }

    // texture size
    AddParameter<int>( _T("TextureSize_X"), &m_nTextureSize[0] );
    AddParameter<int>( _T("TextureSize_Y"), &m_nTextureSize[1] );
    AddParameter<int>( _T("TextureSize_Z"), &m_nTextureSize[2] );
    SetParameterDefaultValue<int>( _T("TextureSize_X"), 7 );
    SetParameterDefaultValue<int>( _T("TextureSize_Y"), 7 );
    SetParameterDefaultValue<int>( _T("TextureSize_Z"), 7 );
    testfactor::RFactor rTexSize_7 
        = AddParameterValue( _T("TextureSize_X"), 7 )
        * AddParameterValue( _T("TextureSize_Y"), 7 )
        * AddParameterValue( _T("TextureSize_Z"), 7 );

    testfactor::RFactor rTexSizes;
    {
        int texSizes[][3] = 
        {
            {1,1,1},
            {4,4,4},
            {7,7,7},
//			{32,1024,2},
//			{64,512,2},
            {512,2,64},
//			{2,64,512},
            {8192,2,1},
            {2,8192,1},
            {2,1,2048},
            /*
            {1182,2,1}, // 16-byte-element texture-cube = 128MB
            {1365,2,1}, // 12-byte-element texture-cube = 128MB
            {1672,2,1}, // 8-byte-element texture-cube = 128MB
            {2364,2,1}, // 4-byte-element texture-cube = 128MB
            {3344,2,1}, // 2-byte-element texture-cube = 128MB
            {4729,2,1}, // 1-byte-element texture-cube = 128MB
            */
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
            if (sPath == "Depth")
            {
                size_t totalSize = texSizes[nTexSize][0];
                if (m_nDimension == 4)
                    totalSize *= 6 * texSizes[nTexSize][0];
                else
                {
                    if (m_nDimension > 1)
                        totalSize *= texSizes[nTexSize][1];
                    if (m_nDimension > 2)
                        totalSize *= texSizes[nTexSize][2];
                }
                if (totalSize > 64*64)
                    continue;
            }

            if( g_App.GetIsFL9Test() )
            {
                if (m_nDimension == 2)
                {
                    texSizes[nTexSize][0] = min(2048, texSizes[nTexSize][0]);
                    texSizes[nTexSize][1] = min(2048, texSizes[nTexSize][1]);
                    texSizes[nTexSize][2] = min(2048, texSizes[nTexSize][2]);
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

            testfactor::RFactor rTexSize_n 
                = AddParameterValue<int>( _T("TextureSize_X"), texSizes[nTexSize][0] ) 
                * AddParameterValue<int>( _T("TextureSize_Y"), texSizes[nTexSize][(m_nDimension == 4)?0:1] ) 
                * AddParameterValue<int>( _T("TextureSize_Z"), (m_nDimension == 4)?1:texSizes[nTexSize][2] );
            if (rTexSizes.Get() == NULL)
                rTexSizes = rTexSize_n;
            else
                rTexSizes = rTexSizes + rTexSize_n;
        }
    }

    // min/magnification coefficient
    AddParameter<double>( _T("MinMag_Coeff"), &m_fMinMagCoeff );
    SetParameterDefaultValue<double>(_T("MinMag_Coeff"), -1.0 );
    testfactor::RFactor rMinMagCoeff = AddParameterValueSet( _T("MinMag_Coeff"), GradientValueSet<double>(-4.0, 4.0, 33) );
    testfactor::RFactor rMinMagCoeff_Few = AddParameterValueSet( _T("MinMag_Coeff"), GradientValueSet<double>(-3.0, 2.0, 11) );
    testfactor::RFactor rMinMagCoeff_0 = AddParameterValue<double>( _T("MinMag_Coeff"), 0.0 );
    testfactor::RFactor rMinMagCoeff_Neg1 = AddParameterValue( _T("MinMag_Coeff"), -1.0 );

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


    //AddParameter<int>( _T("Num_Mips"), &m_nNumMips );
    //SetParameterDefaultValue<int>( _T("Num_Mips"), 1 );
    m_nNumMips = 1;

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
            RangeValueSet<int>(0, 4, 1) );
        rCustomCoords 
            = rCustomCoords 
            + rCustomTexCoords_T * rCustomTexCoord_Nbrs * rTexSizes * rAddressMode_WrapAll;
    }
    testfactor::RFactor rCustomTexCoord_3 = AddParameterValue<int>( _T("TexCoord_Number"), 3 );

    // rotation angle
    AddParameter<int>( _T("TexCoord_Rotation"), &m_TexCoordRotation);
    SetParameterDefaultValue<int>( _T("TexCoord_Rotation"), 0 );
    testfactor::RFactor rTexCoordRotations 
        = AddParameterValueSet<int>( _T("TexCoord_Rotation"), RangeValueSet<int>(0, 360, 15) );

    // border color
    for (int i = 0; i < 4; ++i)
        m_SamplerDesc.BorderColor[i] = (float)s_BorderColor.f[i];
    AddParameter<float>( _T("Border_Color"), (float*)&m_SamplerDesc.BorderColor[0] );
    SetParameterDefaultValue<float>( _T("Border_Color"), 0.5 );
    AddParameter<float>( _T("Border_Color_Alpha"), (float*)&m_SamplerDesc.BorderColor[3] );
    SetParameterDefaultValue<float>( _T("Border_Color_Alpha"), 0.5 );
    testfactor::RFactor rBorderColor_All
        = ( AddParameterValueSet( _T("Border_Color"),       GradientValueSet<float>(-1.25, 1.25, 11) )
          % AddParameterValueSet( _T("Border_Color_Alpha"), GradientValueSet<float>(-1.25, 1.25, 11) )
          )
        + ( AddParameterValue<float>( _T("Border_Color"),       f_getINF(false) )
          % AddParameterValue<float>( _T("Border_Color_Alpha"), f_getINF(false) )
          )
        + ( AddParameterValue<float>( _T("Border_Color"),       f_getINF(true) )
          % AddParameterValue<float>( _T("Border_Color_Alpha"), f_getINF(true) )
          )
          ;

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

    AddParameter<bool>( _T("Multisample"), &m_bMultisampleEnable);
    SetParameterDefaultValue<bool>( _T("Multisample"), false );
    testfactor::RFactor rMultisample_F = AddParameterValue<bool>( _T("Multisample"), false );
    testfactor::RFactor rMultisample_T = AddParameterValue<bool>( _T("Multisample"), true );

    AddParameter<bool>( _T("SampleFreq_Shader"), &m_bSampleFreqShader);
    SetParameterDefaultValue<bool>( _T("SampleFreq_Shader"), false );
    testfactor::RFactor rSampleFreqShader_F = AddParameterValue<bool>( _T("SampleFreq_Shader"), false );
    testfactor::RFactor rSampleFreqShader_T = AddParameterValue<bool>( _T("SampleFreq_Shader"), true );

    // center position TODO
    // pixel jitter TODO

    testfactor::RFactor rCycleFilterDefaultFormat = (rFilterType_Normal * rTexFormat_Default + rFilterType_Comparison * rTexFormat_R32F);
    testfactor::RFactor rCycleFilterTypeNoFormat = 
        (rFilterType_Normal + rFilterType_Comparison) * (rSampleFunc_Sample + rSampleFunc_SampleCmp) * rMinMagCoeff_Neg1;

    if( g_App.GetIsFL9Test() )
    {
        tstring sPathResourceType = GetPath().ToString( GetPath().Depth() - 1, 1 );
        CUserValueSet<DXGI_FORMAT> *pL9FormatValues = new CUserValueSet<DXGI_FORMAT>;
        // R8_UNORM supports 2D, 3D and Cube on WDDM1.3 FL9
        pL9FormatValues->AddValue( DXGI_FORMAT_R8_UNORM );
        // A8_UNORM and R8G8_UNORM only support 2D on WDDM1.3 FL9
        if (sPathResourceType == "2D")
        {
            pL9FormatValues->AddValue( DXGI_FORMAT_A8_UNORM );
            pL9FormatValues->AddValue( DXGI_FORMAT_R8G8_UNORM );
        }
        testfactor::RFactor rTexFormats_FL9 = AddParameterValueSet( _T("Format"), pL9FormatValues);

        // FL9.1 only supports sample and sampleBias
        testfactor::RFactor rSampleFunc_FL9 = AddParameterValue<eSampleFunction>( _T("Sample_Func"), Sample ) + 
            AddParameterValue<eSampleFunction>( _T("Sample_Func"), SampleBias );

        testfactor::RFactor rRoot = rTexFormats_FL9 * rFilterType_Normal * rSampleFunc_FL9 * rTexSizes * rAddressMode_WrapAll;
        SetRootTestFactor( rRoot );
        return;
    }


    SetRootTestFactor(
         (
         rCycleFilterTypeNoFormat * ( rAddressMode_BorderAll * ( rTexFormats % rBorderColor_All ) ) * rTexSize_7 + 
         rCycleFilterTypeNoFormat * rTexFormats_AllSizes * rCustomCoords * rAddressMode_WrapAll * rTexSize_7 + 
         rCycleFilterTypeNoFormat * rTexFormats_AllSizes * rAddressMode_WrapAll * rTexSize_7 * rMultisample_T 
             * (rSampleFreqShader_F + rSampleFreqShader_T) +
         rCycleFilterTypeNoFormat * ( rTexFormats_AllSizes * rTexSizes ) * rAddressMode_WrapAll
         ) + 

         rCycleFilterDefaultFormat * 
            ( (rSampleFunc_Sample + rSampleFunc_SampleCmp) * 
                ( rTexSize_7 *
                    ( rMinMagCoeff
                    + rMinMagCoeff * rSampleAOff_T * (rAddressMode_WrapAll + rAddressMode_BorderAll)
                    + rMinMagCoeff_Few * rAddressMode_All
                    + ( rTexCoordRotations % rMinMagCoeff_Few ) * (rAddressMode_WrapAll + rAddressMode_BorderAll)
                    )
                + rTexSizes * rMinMagCoeff_Neg1 * rSampleAOff_T * rCustomTexCoords_T * rCustomTexCoord_3 * (rAddressMode_WrapAll + rAddressMode_BorderAll)
                ) + 
                rSampleFunc_All * rMinMagCoeff_Few * rTexSize_7 * rSampleAOff_Both * rAddressMode_WrapAll +
                rSampleFunc_All * rMinMagCoeff_Neg1 * ( rSamplerMipBias + rTexCoordRotations ) * rTexSize_7 * 
                    rSampleAOff_Both * rAddressMode_WrapAll
                    ) 
        );


    AddPriorityPatternFilter(FilterEqual<double>(_T("MinMag_Coeff"), -1.0), WeightFactorLarge);
    AddPriorityPatternFilter(FilterEqual<double>(_T("MinMag_Coeff"), 0.0), WeightFactorLarge);
    AddPriorityPatternFilter(FilterEqual<double>(_T("MinMag_Coeff"), 1.0), WeightFactorLarge);
    AddPriorityPatternFilter(FilterEqual<int>(_T("TexCoord_Rotation"), 0), WeightFactorLarge);
    AddPriorityPatternFilter(FilterEqual<bool>(_T("Custom_TexCoords"), false), WeightFactorLarge);
    AddPriorityPatternFilter(FilterEqual(_T("Format"), DXGI_FORMAT_R16G16B16A16_FLOAT), WeightFactorLarge);
    AddPriorityPatternFilter(FilterEqual(_T("Format"), DXGI_FORMAT_R32_FLOAT), WeightFactorLarge);
    AddPriorityPatternFilter(FilterEqual<int>(_T("TextureSize_X"), 7), WeightFactorLarge);
    
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
            || FilterLess<int>(_T("TextureSize_X"), 7)
            || FilterGreater<int>(_T("TextureSize_X"), 8)
            || FilterLess<float>(_T("Border_Color"), 0.75)
            || FilterGreater<float>(_T("Border_Color"), 1.25)
            || (FilterNotEqual(_T("Format"), DXGI_FORMAT_R16G16B16A16_FLOAT) && FilterNotEqual(_T("Format"), DXGI_FORMAT_R32_FLOAT))
            || (FilterNotEqual<float>(_T("Sampler_MipBias"), 0) && FilterNotEqual<double>(_T("MinMag_Coeff"), 1.0))
            ));
    }*/
}

TEST_RESULT 
CBasicFilterTest::Setup()
{
    if (g_App.m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_0)
    {
        SkipMatching(GetSkipMatchingFilter() || (
            FilterEqual<bool>(_T("SampleFreq_Shader"), true)
            ));
    }
    
    return CFilterConfTest::Setup();
}

TEST_RESULT 
CBasicFilterTest::ExecuteTestCase()
{
    TEST_RESULT tRes = RESULT_PASS;
    HRESULT hr = E_FAIL;
    ID3D11Resource* pTexRsc = NULL;
    ID3D11ShaderResourceView* pSRView = NULL;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = m_eFormat;

    if (!CreateTestMips(1, m_nTextureSize))
    {
        tRes = RESULT_SKIP;
        WriteToLog( _T("%s CreateTestMips failed."), LOG_HEADER );
        goto testDone;
    }

    bool bNeedsConversion = false;
    eTexPattern texPattern = TexPattern_Checker;
    {
        tstring sPath = GetPath().ToString( GetPath().Depth() - 2, 1 );
        if (sPath == "Point" || sPath == "Mixed")
            texPattern = TexPattern_Gradient;
    }
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
        bNeedsConversion = true;
        break;
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
        bNeedsConversion = true;
        break;
    case DXGI_FORMAT_R1_UNORM:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        bNeedsConversion = true;
        break;
    }

    hr = CreateTexturePattern(&pTexRsc, texPattern);
    if (FAILED(hr))
    {
        if (hr == E_OUTOFMEMORY)
        {
            const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(m_eFormat);
            double texSize = 0;
            switch (m_nDimension)
            {
            default:
                assert(!"Unexpected dimension value");
                break;
            case 1:
                texSize = m_nTextureSize[0]*pTexFormatInfo->uBitsPerElement*(1.0/8);
                break;
            case 2:
                texSize = m_nTextureSize[0]*m_nTextureSize[1]*pTexFormatInfo->uBitsPerElement*(1.0/8);
                break;
            case 3:
                texSize 
                    = m_nTextureSize[0]*m_nTextureSize[1]*m_nTextureSize[2]*pTexFormatInfo->uBitsPerElement*(1.0/8);
                break;
            case 4:
                texSize = m_nTextureSize[0]*m_nTextureSize[0]*6.0*pTexFormatInfo->uBitsPerElement*(1.0/8);
                break;
            }
            if (texSize > (128.0*1024*1024) || bNeedsConversion)
            {
                tRes = RESULT_SKIP;
                goto testDone;
            }
        }

        tRes = RESULT_FAIL;
        WriteToLog( _T("%s CreateTexturePattern failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
        goto testDone;
    }
    if (hr == S_FALSE)
    {
        tRes = RESULT_SKIP;
        goto testDone;
    }

    hr = UpdateResources();
    if (FAILED(hr))
    {
        tRes = RESULT_FAIL;
        WriteToLog( _T("%s UpdateResources failed."), LOG_HEADER );
        goto testDone;
    }

    switch (m_nDimension)
    {
    case 1: // 1D
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
        srvDesc.Texture1D.MipLevels = 1;
        srvDesc.Texture1D.MostDetailedMip = 0;
        break;
    case 2: // 2D
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;
        break;
    case 3: // 3D
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
        srvDesc.Texture3D.MipLevels = 1;
        srvDesc.Texture3D.MostDetailedMip = 0;
        break;
    case 4: // Cube
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MipLevels = 1;
        srvDesc.TextureCube.MostDetailedMip = 0;
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
        tRes = TestDrawnPixels(m_vTexCoords);

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

