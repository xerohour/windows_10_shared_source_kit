AT_R16_UNORM:
        if (b10_0)
            retVal |= D3D11_FORMAT_SUPPORT_BLENDABLE;
        break;
    // depth formats
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
        retVal &= (~D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD);
        break;
    // intentional fall-through to give these sample but take away MSAA
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        if (b10_0)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_SAMPLE;
    // no multi-sample stuff for typeless formats
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    // or for depth-alikes
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        retVal &= (~D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET);
        retVal &= (~D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD);
        break;
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
        break;
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        retVal |= D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT;
        //optional for a 10.* extended-format driver: (required for 11)
        if ( b10_1 | b10_0 )
            retVal |= D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST;
        break;
    case DXGI_FORMAT_B8G8R8A8_UNORM:            
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        retVal |= D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT;
        //optional for a 10.* extended-format driver: (required for 11)
        if ( b10_1 | b10_0 )
        {
            retVal |= D3D11_FORMAT_SUPPORT_DISPLAY | D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST;
            retVal |= D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT;
        }
        //fall through:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
        // Due to the fall through, we need to check the format before setting these video bits
        if( DXGI_FORMAT_B8G8R8X8_UNORM == fmt )
        {
            retVal |= D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT;
            retVal |= D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT;
        }
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        retVal |= D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET;
        retVal |= D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD;
        //these are all the bits that are optional for an extended-format 10.* driver:
        if ( b10_1 | b10_0 )
        {
            retVal |=
              D3D11_FORMAT_SUPPORT_BUFFER 
            | D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER
            | D3D11_FORMAT_SUPPORT_TEXTURE1D
            | D3D11_FORMAT_SUPPORT_TEXTURE3D
            | D3D11_FORMAT_SUPPORT_TEXTURECUBE
            | D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE
            | D3D11_FORMAT_SUPPORT_MIP
            | D3D11_FORMAT_SUPPORT_MIP_AUTOGEN;

            //and these are all the bits that have to be set by a 10.0 or 10.1 non-extended driver, but are not accessible because 
            //BGR* is not a public format with such drivers. These bits are therefore optional also, because they get turned on by 
            //extended format drivers.
            retVal |=
              D3D11_FORMAT_SUPPORT_TEXTURE1D
            | D3D11_FORMAT_SUPPORT_TEXTURE2D
            | D3D11_FORMAT_SUPPORT_TEXTURE3D
            | D3D11_FORMAT_SUPPORT_SHADER_LOAD
            | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE
            | D3D11_FORMAT_SUPPORT_RENDER_TARGET
            | D3D11_FORMAT_SUPPORT_BLENDABLE
            | D3D11_FORMAT_SUPPORT_CPU_LOCKABLE
            | D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT;
        }
        //fallthrough:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        if ( b10_1 | b10_0 )
        {
            //These are all the bits that have to be set for TYPELESS by a 10.0 or 10.1 non-extended driver, but are not accessible because 
            //BGR* is not a public format with such drivers. These bits are therefore optional also, because they get turned on by 
            //extended format drivers.
            retVal |=
              D3D11_FORMAT_SUPPORT_TEXTURE1D
            | D3D11_FORMAT_SUPPORT_TEXTURE2D
            | D3D11_FORMAT_SUPPORT_TEXTURE3D
            | D3D11_FORMAT_SUPPORT_TEXTURECUBE
            | D3D11_FORMAT_SUPPORT_MIP
            | D3D11_FORMAT_SUPPORT_CPU_LOCKABLE
            | D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT;
        }
        break;

    case DXGI_FORMAT_B4G4R4A4_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
        retVal = D3D11_FORMAT_SUPPORT_BUFFER
            | D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER
            | D3D11_FORMAT_SUPPORT_MIP_AUTOGEN
            | D3D11_FORMAT_SUPPORT_RENDER_TARGET
            | D3D11_FORMAT_SUPPORT_BLENDABLE
            | ((b11_0 || b11_1) ? EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_STORE) : 0)
            | ((b11_1OrLess) ? D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW : 0)
            | ((b11_1OrLess) ? D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET : 0)
            | D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE
            | D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD;
        break;

    case DXGI_FORMAT_B5G6R5_UNORM:
        retVal = D3D11_FORMAT_SUPPORT_BUFFER
            | D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER
            | ((b11_0 || b11_1) ? EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_STORE) : 0)
            | ((b11_1OrLess) ? D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW : 0)
            | ((b10_0) ? D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET : 0); // Optional on FL10.0 ONLY for 4x MSAA.
        break;

    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        //optional for a 10.* extended-format driver:
        if ( b10_1 | b10_0 )
        {
            retVal = (D3D11_FORMAT_SUPPORT_TEXTURE2D | 
                D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT | 
                D3D11_FORMAT_SUPPORT_DISPLAY |
                D3D11_FORMAT_SUPPORT_CPU_LOCKABLE |
                D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST |
                D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT);
        }
        retVal |= D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT;
        break;

    case DXGI_FORMAT_AYUV:
    case DXGI_FORMAT_YUY2:
        retVal = D3D11_FORMAT_SUPPORT_DECODER_OUTPUT
            | D3D11_FORMAT_SUPPORT_VIDEO_ENCODER
            | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT
            | D3D11_FORMAT_SUPPORT_CAPTURE
            | EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_MULTIPLANE_OVERLAY);
        break;

    case DXGI_FORMAT_Y416:
    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
    case DXGI_FORMAT_NV11:
        retVal |= D3D11_FORMAT_SUPPORT_CAPTURE
               | EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_MULTIPLANE_OVERLAY);
        //fallthrough:
    case DXGI_FORMAT_Y410:
        retVal |= D3D11_FORMAT_SUPPORT_DECODER_OUTPUT
            | D3D11_FORMAT_SUPPORT_VIDEO_ENCODER
            | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT
            | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT
            | EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_MULTIPLANE_OVERLAY);
        break;

    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
        if( b10_0 )
        {
            retVal |= D3D11_FORMAT_SUPPORT_BLENDABLE;
        }
        retVal |= D3D11_FORMAT_SUPPORT_DECODER_OUTPUT
            | D3D11_FORMAT_SUPPORT_VIDEO_ENCODER
            | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT
            | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT
            | D3D11_FORMAT_SUPPORT_CAPTURE
            | EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_MULTIPLANE_OVERLAY);
        break;

    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_420_OPAQUE:
        retVal |= D3D11_FORMAT_SUPPORT_VIDEO_ENCODER
            | D3D11_FORMAT_SUPPORT_CAPTURE
            | EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_MULTIPLANE_OVERLAY);
        break;
    
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_P8:
    case DXGI_FORMAT_A8P8:
        retVal = EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_MULTIPLANE_OVERLAY);
        break;
    case DXGI_FORMAT_P208:
    case DXGI_FORMAT_V208:
    case DXGI_FORMAT_V408:
        retVal = EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_MULTIPLANE_OVERLAY);
        break;
    }
    
    // Typed UAV Load optional support
    switch (fmt)
    {
        // All-or-nothing subset
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SINT:
        // Additional formats
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_A8_UNORM:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_B4G4R4A4_UNORM:
            // while technically this feature requires WDDM2.0+ and a cap bit set,
            // we have D3DCore_*_conf.dll TAEF tests that verify this so we only check FL here
            if (g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
                retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_LOAD);
            break;
        default:
            break;
    }

    // Logic ops are optional only on renderable UINT formats for FL11 and below
    const UINT64 tempSupportBits = retVal | getRequiredSupport(fmt, WDDMVersion);
    const bool bUINTFormat = (CFormatInfoMap::Find(fmt)->dwFlags & FF_UINT) ? true : false;
    const bool bRenderable = (tempSupportBits & D3D11_FORMAT_SUPPORT_RENDER_TARGET) != 0;
    if( b11_0OrLess && bUINTFormat && bRenderable )
    {
        retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_OUTPUT_MERGER_LOGIC_OP);
    }

    if (! (retVal & D3D11_FORMAT_SUPPORT_RENDER_TARGET))
        retVal &= (~D3D11_FORMAT_SUPPORT_MIP_AUTOGEN);

    return retVal;
}

//This test is to enable extended formats
UINT64
getOptionalGroup(DXGI_FORMAT fmt)
{
    const bool b10_0 
        = g_TestApp.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0;
    const bool b10_1 
        = g_TestApp.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_1;
    const bool b11_0 
        = g_TestApp.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_11_0;
    UINT64 retVal = 0;

    if ( b11_0 | b10_0 )
        return retVal;

    switch (fmt)
    {
    default:
        break;

        //For BGR*_UNORM, the optional group is everything that wasn't required in 10.0 (which are commented out here):
    case DXGI_FORMAT_B8G8R8A8_UNORM:            
        retVal |= 
              D3D11_FORMAT_SUPPORT_DISPLAY 
            | D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST;
        //fallthrough:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
        retVal |= 
              D3D11_FORMAT_SUPPORT_BUFFER 
            | D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER
            | D3D11_FORMAT_SUPPORT_TEXTURE1D
            //| D3D11_FORMAT_SUPPORT_TEXTURE2D
            | D3D11_FORMAT_SUPPORT_TEXTURE3D
            | D3D11_FORMAT_SUPPORT_TEXTURECUBE
            //| D3D11_FORMAT_SUPPORT_SHADER_LOAD
            //| D3D11_FORMAT_SUPPORT_SHADER_SAMPLE
            | D3D11_FORMAT_SUPPORT_MIP
            | D3D11_FORMAT_SUPPORT_MIP_AUTOGEN
            //| D3D11_FORMAT_SUPPORT_RENDER_TARGET
            //| D3D11_FORMAT_SUPPORT_BLENDABLE
            //| D3D11_FORMAT_SUPPORT_CPU_LOCKABLE
            //| D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT
            ;
        break;
        //The BGR*_SRGB formats are new, so if they are supported at all, then they need to support everything:
        //(And BGRA is special cuz of scanout)
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:   
        retVal |= 
              D3D11_FORMAT_SUPPORT_DISPLAY 
            | D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST;
        //fall through:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        retVal |= 
              D3D11_FORMAT_SUPPORT_TEXTURE1D
            | D3D11_FORMAT_SUPPORT_TEXTURE2D
            | D3D11_FORMAT_SUPPORT_TEXTURE3D
            | D3D11_FORMAT_SUPPORT_TEXTURECUBE
            | D3D11_FORMAT_SUPPORT_SHADER_LOAD
            | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE
            | D3D11_FORMAT_SUPPORT_MIP
            | D3D11_FORMAT_SUPPORT_MIP_AUTOGEN
            | D3D11_FORMAT_SUPPORT_RENDER_TARGET
            | D3D11_FORMAT_SUPPORT_BLENDABLE
            | D3D11_FORMAT_SUPPORT_CPU_LOCKABLE
            | D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT;
        break;
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        retVal = (D3D11_FORMAT_SUPPORT_TEXTURE2D | 
            D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT | 
            D3D11_FORMAT_SUPPORT_DISPLAY |
            D3D11_FORMAT_SUPPORT_CPU_LOCKABLE |
            D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST);
        break;
    }

    return retVal;
}

void
printFormatSupportBitNames(CTest* pTest, UINT64 wrongBits)
{
    // Each enum is still 32 bits, but when both D3D11_FORMAT_SUPPORT and D3D11_FORMAT_SUPPORT2 are combined it is 64 bit.
#define PRINT_BIT(x) if (0 == (wrongBits & x)) {} else pTest->WriteToLog("   0x%x (Low-Word, 32-bit) = %s", x, #x)
#define PRINT_BIT2(x) if (0 == (wrongBits & EXTEND_64BIT(x))) {} else pTest->WriteToLog("   0x%x (High-Word, 32-bit) = %s", x, #x)
    pTest->WriteToLog("Wrong Bits:");
    PRINT_BIT(D3D11_FORMAT_SUPPORT_BUFFER);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_SO_BUFFER);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_TEXTURE1D);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_TEXTURE2D);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_TEXTURE3D);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_TEXTURECUBE);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_SHADER_LOAD);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_SHADER_SAMPLE);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_MONO_TEXT);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_MIP);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_MIP_AUTOGEN);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_RENDER_TARGET);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_BLENDABLE);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_DEPTH_STENCIL);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_CPU_LOCKABLE);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_DISPLAY);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_SHADER_GATHER);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_SHADER_GATHER_COMPARISON);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_DECODER_OUTPUT);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_VIDEO_ENCODER);
    PRINT_BIT(D3D11_FORMAT_SUPPORT_CAPTURE);
    PRINT_BIT2(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_ADD);
    PRINT_BIT2(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_BITWISE_OPS);
    PRINT_BIT2(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_COMPARE_STORE_OR_COMPARE_EXCHANGE);
    PRINT_BIT2(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_EXCHANGE);
    PRINT_BIT2(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_SIGNED_MIN_OR_MAX);
    PRINT_BIT2(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_UNSIGNED_MIN_OR_MAX);
    PRINT_BIT2(D3D11_FORMAT_SUPPORT2_UAV_TYPED_LOAD);
    PRINT_BIT2(D3D11_FORMAT_SUPPORT2_UAV_TYPED_STORE);
    PRINT_BIT2(D3D11_FORMAT_SUPPORT2_OUTPUT_MERGER_LOGIC_OP);
    PRINT_BIT2(D3D11_FORMAT_SUPPORT2_TILED);
    PRINT_BIT2(D3D11_FORMAT_SUPPORT2_SHAREABLE);
#undef PRINT_BIT
#undef PRINT_BIT2
}

void
CCheckFormatSupportTest::Cleanup()
{
    if( m_pAdapterDescriptor )
    {
        delete m_pAdapterDescriptor;
        m_pAdapterDescriptor = nullptr;
    }
}

UINT64
removeVideoSupport(UINT64 support)
{
    //
    // REF doesn't support video
    //

    // Local variables
    const UINT64 lowBitsToRemove 
        = D3D11_FORMAT_SUPPORT_DECODER_OUTPUT
        | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT
        | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT
        | D3D11_FORMAT_SUPPORT_VIDEO_ENCODER
        | D3D11_FORMAT_SUPPORT_CAPTURE;

    // Negate the bits and perform a bitwise AND
    return (support & ~lowBitsToRemove);
}

UINT64
addTiledResourceSupport(UINT64 support, ID3D11Device2* pDevice, DXGI_FORMAT Format, D3D_FEATURE_LEVEL FeatureLevel)
{
    if (FeatureLevel < D3D_FEATURE_LEVEL_11_0)
    {
        return support;
    }

    D3D11_FEATURE_DATA_D3D11_OPTIONS2 featureData;
    {
        HRESULT hr = pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS2, &featureData, sizeof(featureData));
        if ( 
            hr != S_OK ||
            CD3D10FormatHelper_Test::YUV(Format) ||
            Format == DXGI_FORMAT_R1_UNORM || Format == DXGI_FORMAT_R8G8_B8G8_UNORM || Format == DXGI_FORMAT_G8R8_G8B8_UNORM  || // YUV + these 3 formats disallowed explicitly
            Format == DXGI_FORMAT_D24_UNORM_S8_UINT || Format == DXGI_FORMAT_R24G8_TYPELESS || Format == DXGI_FORMAT_R24_UNORM_X8_TYPELESS || Format == DXGI_FORMAT_X24_TYPELESS_G8_UINT || // Stencil formats disallowed explicitily
            Format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT || Format == DXGI_FORMAT_R24_UNORM_X8_TYPELESS || Format == DXGI_FORMAT_R32G8X24_TYPELESS || Format == DXGI_FORMAT_X32_TYPELESS_G8X24_UINT ||
            Format == DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS
            )
        {
            return support;
        }
    }

    switch(CD3D10FormatHelper_Test::GetBitsPerElement(Format) * CD3D10FormatHelper_Test::GetWidthAlignment(Format) *
           CD3D10FormatHelper_Test::GetHeightAlignment(Format) * CD3D10FormatHelper_Test::GetDepthAlignment(Format))
    {
        case 8:
        case 16:
        case 32:
        case 64:
        case 128:
            break;
        default:
            return support;
    }

    if (support & D3D11_FORMAT_SUPPORT_TEXTURE2D)
    {
        if (featureData.TiledResourcesTier != D3D11_TILED_RESOURCES_NOT_SUPPORTED)
        {
            support |= (static_cast<UINT64>(D3D11_FORMAT_SUPPORT2_TILED) << 32);
        }
    }

    return support;
}

void
adjustNV12CaptureSupport(ID3D11Device2* pDevice, UINT64 support, UINT64* pRequired, UINT64* pOptional)
{
    D3D11_FEATURE_DATA_D3D11_OPTIONS4 featureData;
    HRESULT hr = pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS4, &featureData, sizeof(featureData));
    if (hr != S_OK)
    {
        GetFramework()->WriteToLogF(
            "CheckFeatureSupport failed for D3D11_OPTIONS4 in adjustNV12CaptureSupport."
            );
        return;
    }

    if (featureData.ExtendedNV12SharedTextureSupported
        && (support & D3D11_FORMAT_SUPPORT_CAPTURE)
        && !(*pRequired & D3D11_FORMAT_SUPPORT_VIDEO_ENCODER)
        )
    {
        GetFramework()->WriteToLogF(
            "On WDDM2.1 drivers and later, VIDEO_ENCODER must be support when CAPTURE is support on NV12 textures."
            );
        *pRequired |= D3D11_FORMAT_SUPPORT_VIDEO_ENCODER;
        *pOptional &= ~D3D11_FORMAT_SUPPORT_VIDEO_ENCODER;
    }
}

UINT64
checkRequiredSupport(DXGI_FORMAT fmt, UINT64 support, UINT64* pMissingBits, UINT64* pUnnecessaryBits, D3DKMT_DRIVERVERSION WDDMVersion)
{
    UINT64 requiredSupport = getRequiredSupport(fmt, WDDMVersion);
    UINT64 optionalSupport = getOptionalSupport(fmt, WDDMVersion);

    if (g_TestApp.GetDevice2() != nullptr)
    {
        requiredSupport = addTiledResourceSupport(requiredSupport, g_TestApp.GetDevice2(), fmt, g_TestApp.m_D3DOptions.FeatureLevel);

        if (DXGI_FORMAT_NV12 == fmt)
        {
            // VIDEO_ENCODER is required when CAPTURE is present for NV12 textures on WDDM2.1 and later.
            adjustNV12CaptureSupport(g_TestApp.GetDevice2(), support, &requiredSupport, &optionalSupport);
        }
    }

    if (fmt == DXGI_FORMAT_R32G32B32_FLOAT)
    {
        // blendable is only required for this format if rendertarget is supported
        if (!(support & D3D11_FORMAT_SUPPORT_RENDER_TARGET))
        {
            requiredSupport &= ~D3D11_FORMAT_SUPPORT_BLENDABLE;
            optionalSupport |=  D3D11_FORMAT_SUPPORT_BLENDABLE;
        }
    }

    if (fmt == DXGI_FORMAT_B5G6R5_UNORM && 
        g_TestApp.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0)
    {
        // For FL10.0, Multisample Resolve and Multisample Load are only required for this format if MSAA is supported
        if (!(support & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET))
        {
            requiredSupport &= ~D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE;
            requiredSupport &= ~D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD;

            optionalSupport |=  D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE;
            optionalSupport |=  D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD;
        }
    }

    if(0 != (requiredSupport & optionalSupport))
    {
        TEST_RESULT tRes = RESULT_FAIL;
        GetFramework()->WriteToLogF("Test Bug: required and optional bits overlap for format '%s', %#I64x", ToString(fmt).c_str(), (UINT64)(requiredSupport & optionalSupport) );
    }

    bool bWARPDevice = g_TestApp.IsWARPAdapter( g_TestApp.GetDXGIAdapter() );
    bool b11On12 = g_TestApp.IsD3D11On12Device( g_TestApp.GetDevice() );
    // If running on ref/WARP, remove video support (REF/WARP doesn't support video)
    // If the driver doesn't support the 11.1 DDI, then video is not supported
    const bool b11_1DDIIsSupported = g_TestApp.Is11_1DDISupported( g_TestApp.GetDevice() );
    if( D3D_DRIVER_TYPE_REFERENCE == g_TestApp.m_D3DOptions.DriverType ||
        !b11_1DDIIsSupported || bWARPDevice || b11On12 )
    {
        if( !b11_1DDIIsSupported )
        {
            GetFramework()->WriteToLogF("Driver doesn't support the 11.1 DDI. Removing expected video support bits.");
        }

        requiredSupport = removeVideoSupport( requiredSupport );
        optionalSupport = removeVideoSupport( optionalSupport );
    }

    UINT64 supportOptionalRemoved = support & (~optionalSupport);

    UINT64 wrongBits = requiredSupport ^ supportOptionalRemoved;
    UINT64 missingBits = requiredSupport & wrongBits;
    UINT64 unnecessaryBits = (~(requiredSupport | optionalSupport)) & (wrongBits);

    if (pMissingBits)
        *pMissingBits = missingBits;
    if (pUnnecessaryBits)
        *pUnnecessaryBits = unnecessaryBits;

    return wrongBits;
}

UINT64
checkOptionalGroupSupport(DXGI_FORMAT fmt, UINT64 support )
{
    UINT64 groupSupport = getOptionalGroup(fmt);

    UINT64 missingBits = groupSupport & (~(support & groupSupport));

    return missingBits;
}


void
CCheckFormatSupportTest::InitTestParameters()
{
    AddParameter<DXGI_FORMAT>( _T("Format"), &m_Format );
    SetParameterDefaultValue( _T("Format"), DXGI_FORMAT_UNKNOWN );
    testfactor::RFactor rAllFormats = AddParameterValueSet( _T("Format"), 
        FormatValueSet(FF_ALL, 0));

    SetRootTestFactor(rAllFormats);
}

TEST_RESULT
CCheckFormatSupportTest::ExecuteTestCase()
{
    TEST_RESULT tRes = RESULT_PASS;
    ID3D11Device* pDevice = g_TestApp.GetDevice();
    
    bool bExtendedFormat = false;
    const DXGIFormatInfo *pFormatInfo = CFormatInfoMap::Find(m_Format);
    if( !pFormatInfo )
    {
        WriteToLog( _T("Couldn't find format %s in CFormatInfoMap."), ToString(m_Format).c_str() );
        tRes = RESULT_FAIL;
        goto execute_done;
    }

    bool bVideoFormat = (pFormatInfo->dwFlags & FF_VIDEO) > 0;

    switch( m_Format )
    {
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC7_TYPELESS:
        if( g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
            bExtendedFormat = true;
        break;
    }
    
    UINT support;
    HRESULT hr = pDevice->CheckFormatSupport(m_Format, &support);

    // Sometimes a failure is OK
    if( FAILED( hr ) )
    {
        const bool b11_1DDIIsSupported = GetFramework()->Is11_1DDISupported( pDevice );

        // Some extended formats and video formats are optional
        if( bExtendedFormat )
        {
            WriteToLog( _T("The extended format %s is optional."), ToString(m_Format).c_str() );
            WriteToLog( _T("Skipping the test because CheckFormatSupport returned %s."), D3DHResultToString(hr).c_str() );
            tRes = RESULT_SKIP;
            goto execute_done;
        }

        // NOTE: Some video formats are not optional
        if( bVideoFormat )
        {
            // Some formats are required on FL10.0+ if the 11.1 DDI is supported
            // NOTE: REF doesn't support video at all
            std::vector<DXGI_FORMAT> requiredVideoFormats;
            requiredVideoFormats.clear();
            if( b11_1DDIIsSupported &&
                D3D_DRIVER_TYPE_REFERENCE != g_TestApp.m_D3DOptions.DriverType )
            {
                assert( pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_0 );
                requiredVideoFormats.push_back( DXGI_FORMAT_420_OPAQUE );
                requiredVideoFormats.push_back( DXGI_FORMAT_NV12 );
                requiredVideoFormats.push_back( DXGI_FORMAT_YUY2 );
            }

            // If the format is not in the required format list, then the failure is OK
            bool bFoundFormat = false;
            for( UINT i = 0; i < requiredVideoFormats.size(); i++ )
            {
                if( requiredVideoFormats[i] == m_Format )
                {
                    bFoundFormat = true;
                    break;
                }
            }

            if( !bFoundFormat )
            {
                WriteToLog( _T("The video format %s is optional."), ToString(m_Format).c_str() );
                tRes = RESULT_SKIP;
                goto execute_done;
            }
            else
            {
                assert( D3D_DRIVER_TYPE_REFERENCE != g_TestApp.m_D3DOptions.DriverType );
                WriteToLog( _T("The video format %s is required."), ToString(m_Format).c_str() );
                // Do nothing, it will fail below
            }
        }

        // If the driver doesn't support the 11.1 DDI then it might fail for some formats
        if( !b11_1DDIIsSupported &&                                    // Check to see if the driver supports the 11.1 DDI
            CD3D10FormatHelper_Test::Is16BitBGRFormat( m_Format ) )    // The 16 bit formats (B5G6R5, etc...) are only required to pass CheckFormatSupport on the 11.1 DDI
        {
            WriteToLog( _T( "CheckFormatSupport() failed. However the driver doesn't support the D3D11.1 DDI, so this is OK for the 16-bit formats. " ) );
            return RESULT_SKIP;
        }

        // Win8: 63065 - WGFResourceFormats: Remove expected support for R1_UNORM in CheckFormatSupport group
        // R1_UNORM should not be supported
        if( DXGI_FORMAT_R1_UNORM == m_Format &&
            E_FAIL == hr )
        {
            WriteToLog( _T("CheckFormatSupport failed correctly.") );
            tRes = RESULT_PASS;
            goto execute_done;
        }

        // If we got here, CheckFormatSupport() should have passed
        WriteToLog( _T("CheckFormatSupport failed for format %s. HR = %s."), ToString(m_Format).c_str(), D3DHResultToString(hr).c_str() );
        tRes = RESULT_FAIL;
        goto execute_done;
    }
    else
    {
        // Sometimes a success is BAD
        if( bExtendedFormat &&
            support != 0 )
        {
            tRes = RESULT_FAIL;
            WriteToLog(_T("Format '%s' should've failed CheckFormatSupport since it's not supported on this feature level. support = %#I64x"), 
                ToString(m_Format).c_str(), support);
            printFormatSupportBitNames(this, support);
        }

        // Win8: 63065 - WGFResourceFormats: Remove expected support for R1_UNORM in CheckFormatSupport group
        // R1_UNORM should not be supported
        if( DXGI_FORMAT_R1_UNORM == m_Format &&
            E_FAIL != hr )
        {
            WriteToLog(_T("CheckFormatSupport didn't failed for format %s. It should not supported. HR = %s."), ToString(m_Format).c_str(), D3DHResultToString(hr).c_str() );
            tRes = RESULT_FAIL;
            goto execute_done;
        }
    }

    UINT64 fullSupport = 0;
    {
        D3D11_FEATURE_DATA_FORMAT_SUPPORT featureFormatSupport1;
        D3D11_FEATURE_DATA_FORMAT_SUPPORT2 featureFormatSupport2;
        featureFormatSupport1.InFormat = m_Format;
        featureFormatSupport1.OutFormatSupport = 0;
        hr = pDevice->CheckFeatureSupport(D3D11_FEATURE_FORMAT_SUPPORT, &featureFormatSupport1, sizeof(featureFormatSupport1) );
        if (featureFormatSupport1.OutFormatSupport != support)
        {
            UINT bitDiff = support ^ featureFormatSupport1.OutFormatSupport;
            tRes = RESULT_FAIL;
            WriteToLog(_T("CheckFormatSupport: %#I64x. CheckFeatureSupport(D3D11_FEATURE_FORMAT_SUPPORT): %#I64x. Diff: %#I64x"), 
                support, featureFormatSupport1.OutFormatSupport, bitDiff );
            printFormatSupportBitNames(this, bitDiff);
        }
        featureFormatSupport2.InFormat = m_Format;
        featureFormatSupport2.OutFormatSupport2 = 0;
        hr = pDevice->CheckFeatureSupport(D3D11_FEATURE_FORMAT_SUPPORT2, &featureFormatSupport2, sizeof(featureFormatSupport2) );

        fullSupport = (UINT64)featureFormatSupport1.OutFormatSupport | ((UINT64)featureFormatSupport2.OutFormatSupport2 << 32);
    }

    UINT64 wrongBits, missingBits, unnecessaryBits;
    wrongBits = checkRequiredSupport(m_Format, fullSupport, &missingBits, &unnecessaryBits, m_wddmDriverVersion);
    if (wrongBits != 0)
    {
        tRes = RESULT_FAIL;
        WriteToLog(_T("Format '%s': Required support bits are incorrect: %#I64x. Required but missing: %#I64x. Not required or optional but set: %#I64x"), 
            ToString(m_Format).c_str(), wrongBits, missingBits, unnecessaryBits );
        printFormatSupportBitNames(this, wrongBits);
    }

    missingBits = checkOptionalGroupSupport(m_Format, support);
    if (missingBits != 0)
    {
        tRes = RESULT_FAIL;
        WriteToLog(_T("Format '%s': If any of these optional bits are set, they all should be set. Required but missing: %#I64x. "), 
            ToString(m_Format).c_str(), missingBits );
        printFormatSupportBitNames(this, missingBits);
    }

execute_done:
    return tRes;
}

//////////////////////////////////////////////////////////////////////
// File:  PresentFS.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of DXGI present full-screen swap chain tests.
//
// History:
// ?? ??? ????  AndrewLu  Created.
// 05 Oct 2007  SEdmison  Rewrote, cleaned up, and added feature level
//						support.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "TestApp.h"

#include "DXGIValueSets.h"
#include "DXGIScanoutFormats.h"
#include "DXGISwapEffects.h"

#include <StrSafe.h>
#include "winuserp.h"

#pragma warning(disable: 4355)


/////////////////////////////////////////////////////////////////
//
// Constants
//
/////////////////////////////////////////////////////////////////

const char * const szScanLineNames[] =
{
	"DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED",
	"DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE",
	"DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST",
	"DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST"
};

const char * const szScalingNames[] =
{
	"DXGI_MODE_SCALING_UNSPECIFIED",
	"DXGI_MODE_SCALING_CENTERED",
	"DXGI_MODE_SCALING_STRETCHED"
};


/////////////////////////////////////////////////////////////////
//
// CPresentFS implementation
//
/////////////////////////////////////////////////////////////////

CPresentFS::CPresentFS()
:
m_bTestRotatedModes( false ),
m_bOneModeOnly( false ),
m_bIgnoreRefresh( true ),
m_bUseMinimalFormatList(false),
m_bAlwaysFilterLowResolution( false )
{
	m_NumberOfPresents = 1;
	m_TexturesToLoad = m_NumberOfPresents;
	m_MaxBufferCount = 2;

	ZeroMemory( &m_OutputModeSet, MAX_DISPLAYMODES * sizeof( ModeInformation ) );
	ZeroMemory( &m_ModeInformation, sizeof( ModeInformation ) );

};


CPresentFS::~CPresentFS()
{
};

HRESULT CPresentFS::ModeListTransform(
	D3D_DEVICE_POINTER devicePointer,
	__in std::vector< ModeInformation >* pModes,
	__out std::vector< ModeInformation >* pTransformedModes)
{
	HRESULT hr = GetSupportedFormats(
		devicePointer,
		pModes,
		pTransformedModes,
		m_RequiredFormatSupport);

	return hr;
}


void CPresentFS::InitModeParameters()
{
	CDXGITestFramework * pFramework = dynamic_cast< CDXGITestFramework * >( GetFramework() );

	//
	// Enumerate desktop modes.
	//
	RFactor modeFactor;

	std::vector < Adapter > adapterEnumerated;
	std::vector < Adapter > adapterTransformed;

	HRESULT hresult = EnumerateModes(&adapterEnumerated);
	if ( FAILED( hresult ) )
	{
		WriteToLog
			(
			"CPresentFS::InitTestParameters:  EnumerateModes failed with error 0x%x.",
			hresult
			);

		goto Cleanup;
	};

	PrintModes(&adapterEnumerated);
	WriteToLog("*********Enumerated Modes\n");

	if( !g_Application.IsUseDesktopResolution() )
	{
		if(m_bAlwaysFilterLowResolution ||
			g_Application.IsNoLowResolution())
		{
			WriteToLog("*********FilterLowResolutionModes\n");
			FilterLowResolutionModes(&adapterEnumerated);
			PrintModes(&adapterEnumerated);
		}

		if ( m_bIgnoreRefresh )
		{
			RemoveModesThatVaryOnlyByRefresh(&adapterEnumerated);
			PrintModes(&adapterEnumerated);
			WriteToLog("*********RemoveModesThatVaryOnlyByRefresh\n");

		}

		if (m_bUseMinimalFormatList)
		{
			UseMinimalFormatList(&adapterEnumerated);
			PrintModes(&adapterEnumerated);
			WriteToLog("*********UseMinimalFormatList\n");
		}


		if (m_bOneModeOnly)
		{
		FirstModePerFormat(&adapterEnumerated);
		PrintModes(&adapterEnumerated);
		WriteToLog("*********FirstModePerFormat\n\n\n\n");

		}


		if(g_Application.IsMinResolutionSet())
		{
			if(pFramework->GetTestMode() == WGFTEST_MODE_WHQL)
			{
				WriteToLog("*********GetHighLowMidResolutionModes\n\n");
				GetHighLowMidResolutionModes(&adapterEnumerated);    
			} else
			{
				WriteToLog("*********GetHighResolutionModes\n\n");
				GetHighResolutionModes(&adapterEnumerated);      
			}
			PrintModes(&adapterEnumerated);
		}
	}

	for(UINT k = 0; k < adapterEnumerated.size();k++)
	{
		Adapter adapter;
		adapterTransformed.push_back(adapter);

		//
		// TEST_FEATURE_LEVEL is a parameter and is not available in InitParameters.
		//
		TEST_FEATURE_LEVEL testFeatureLevel= TEST_FEATURE_10_0_LEVEL_10_0;
		for(UINT i = 0; i < TEST_FEATURE_COUNT; i++)
		{
			if(pFramework->m_RequestedFeatureLevels[i] == true)
			{
				testFeatureLevel = (TEST_FEATURE_LEVEL)i;
			}
		}

		D3D_DEVICE_POINTER devicePointer;
		ZeroMemory(&devicePointer, sizeof(devicePointer));

		hresult = CreateAdapterDevice(testFeatureLevel, k, &devicePointer);
		if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CPresentFS::InitTestParameters:  EnumerateModes failed with error 0x%x.",
				hresult
			);

			goto Cleanup;
		};



		for(UINT j = 0; j < adapterEnumerated[k].OutputModeContainer.size();j++)
		{
			OutputModeContainer output;
			adapterTransformed[k].OutputModeContainer.push_back(output);

			for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
			{

				hresult = ModeListTransform(
					devicePointer,
					&adapterEnumerated[k].OutputModeContainer[j].RawModes[rotation],
					&adapterTransformed[k].OutputModeContainer[j].RawModes[rotation]);

				if(FAILED(hresult))
				{
					if ( NULL != devicePointer.pDevice)
					{
						devicePointer.pDevice->Release();
						devicePointer.pDevice = NULL;
					};

					goto Cleanup;
				}
			}
		}

		if ( NULL != devicePointer.pDevice)
		{
			devicePointer.pDevice->Release();
			devicePointer.pDevice = NULL;
		};

	}

	m_iTotalModes = 0;

	for(UINT k = 0; k < adapterTransformed.size();k++)
	{
		for(UINT j = 0; j < adapterTransformed[k].OutputModeContainer.size();j++)
		{
			//TCHAR parameterName[ 255 ] = {0};	
			//StringCchPrintf
			//    (
			//    parameterName,
			//    ARRAYSIZE( parameterName ),
			//    _T("Mode_Adapter_%d_output_%d"),
			//    k,
			//    j
			//    );  

			if ( adapterTransformed[k].OutputModeContainer[j].GetTotalModeCount() > 0 )
			{
				for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
				{
					for(UINT z = 0 ; z < adapterTransformed[k].OutputModeContainer[j].RawModes[rotation].size() 
						&& z < ( MAX_MODES_PER_OUTPUT ) / 4; 
						z++)
					{
						assert( m_iTotalModes < MAX_DISPLAYMODES );
						m_OutputModeSet[ m_iTotalModes ] = adapterTransformed[k].OutputModeContainer[j].RawModes[rotation][z];
						m_iTotalModes++;
					}
				}
			}
			else
			{
				DebugOut.Write("No Modes found for adapter[%d].Output[%d] \n", k,j);
				m_ParameterError = -1;
			}
		}
	}

Cleanup:;

}


void CPresentFS::InitTestParameters()
{
	InitFSTestParameters();

	RFactor testRFactors= IntitTestRFactors();


	if(m_globalRFactor.Valid() && m_FeatureLevelFactor.Valid())
	{

		//
		// If derived test sets some rfactors use them. If there is an error or they dont exist dont use the.
		//
		if(testRFactors.Valid())
		{
			SetRootTestFactor(m_FeatureLevelFactor * m_globalRFactor * testRFactors );
		}
		else
		{
			SetRootTestFactor(m_FeatureLevelFactor * m_globalRFactor );  
		}
	}
	else
	{
		if( m_FeatureLevelFactor.Valid() )
			SetRootTestFactor( m_FeatureLevelFactor );

		WriteToLog("CPresentFS::SetRootTestFactor parameter combination is invalid. m_globalRFactor.Valid=%d m_FeatureLevelFactor.Valid=%d", m_globalRFactor.Valid(), m_FeatureLevelFactor.Valid());
		m_ParameterError = -1;
	}
}


void CPresentFS::InitFSTestParameters()
{

	RFactor rotation, format, width, height, refreshrate[2], scanlineordering, scaling, adapter, output, multisamplecount, multisamplequality, isInitialized;
	//
	// This test group doesn't run under TS.
	//
	if
	(
		g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION ) != 0
	)
	{
		goto Cleanup;
	};


	//
	// Call the base class implementation.
	//

	CDXGITest::InitTestParameters();

	//
	// Enumerate\Initailize modes
	//
	InitModeParameters();

	if (m_iTotalModes > 0 )
	{
		rotation = AddParameter(
			_T( "Rotation" ),
			&m_ModeInformation.devmode.dmDisplayOrientation,
			new CTableValueSet< DWORD >(
				&m_OutputModeSet[0].devmode.dmDisplayOrientation,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		format = AddParameter(
			_T( "ModeDesc.Format" ),
			&m_ModeInformation.ModeDesc.Format,
			new CTableValueSet< DXGI_FORMAT >(
				&m_OutputModeSet[0].ModeDesc.Format,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		width = AddParameter(
			_T( "ModeDesc.Width" ),
			&m_ModeInformation.ModeDesc.Width,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].ModeDesc.Width,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		height = AddParameter(
			_T( "ModeDesc.Height" ),
			&m_ModeInformation.ModeDesc.Height,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].ModeDesc.Height,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		refreshrate[0] = AddParameter(
			_T( "ModeDesc.RefreshRate.Numerator" ),
			&m_ModeInformation.ModeDesc.RefreshRate.Numerator,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].ModeDesc.RefreshRate.Numerator,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		refreshrate[1] = AddParameter(
			_T( "ModeDesc.RefreshRate.Denominator" ),
			&m_ModeInformation.ModeDesc.RefreshRate.Denominator,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].ModeDesc.RefreshRate.Denominator,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		scanlineordering = AddParameter(
			_T( "ModeDesc.ScanlineOrdering" ),
			&m_ModeInformation.ModeDesc.ScanlineOrdering,
			new CTableValueSet< DXGI_MODE_SCANLINE_ORDER >(
				&m_OutputModeSet[0].ModeDesc.ScanlineOrdering,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		scaling = AddParameter(
			_T( "ModeDesc.Scaling" ),
			&m_ModeInformation.ModeDesc.Scaling,
			new CTableValueSet< DXGI_MODE_SCALING >(
				&m_OutputModeSet[0].ModeDesc.Scaling,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		adapter = AddParameter(
			_T( "DXGIAdapterOrdinal" ),
			&m_ModeInformation.uiDXGIAdapterOrdinal,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].uiDXGIAdapterOrdinal,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		output = AddParameter(
			_T( "DXGIOutputOrdinal" ),
			&m_ModeInformation.uiDXGIOutputOrdinal,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].uiDXGIOutputOrdinal,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		multisamplecount = AddParameter(
			_T( "MultisampleCount" ),
			&m_ModeInformation.uiMultisampleCount,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].uiMultisampleCount,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		multisamplequality = AddParameter(
			_T( "MultisampleQuality" ),
			&m_ModeInformation.uiMultisampleQuality,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].uiMultisampleQuality,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		isInitialized = AddParameter(
			_T( "isInitialized" ),
			&m_ModeInformation.isInitialized,
			new CTableValueSet< BOOL >(
				&m_OutputModeSet[0].isInitialized,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		m_globalRFactor = rotation % format % width % height % refreshrate[0] % refreshrate[1] % scanlineordering % scaling % adapter % output % multisamplecount % multisamplequality % isInitialized;

		if( m_vecAdapterDescriptors.size() < 2)
			AddPriorityPatternFilter((FilterEqual< UINT >(_T("DXGIAdapterOrdinal"), 0 ) && FilterEqual< UINT >(_T("DXGIOutputOrdinal"), 0 )), WeightFactorLarge);

		AddPriorityPatternFilter((FilterEqual< UINT >(_T("DXGIAdapterOrdinal"), 1 ) && FilterEqual< UINT >(_T("DXGIOutputOrdinal"), 0 )), WeightFactorLarge);
		AddPriorityPatternFilter((FilterEqual< UINT >(_T("DXGIAdapterOrdinal"), 0 ) && FilterEqual< UINT >(_T("DXGIOutputOrdinal"), 1 )), WeightFactorLarge);
		AddPriorityPatternFilter(FilterEqual< DXGI_FORMAT >(_T("ModeDesc.Format"), DXGI_FORMAT_R8G8B8A8_UNORM ), WeightFactorLarge);
		AddPriorityPatternFilter(FilterEqual< DXGI_FORMAT >(_T("ModeDesc.Format"), DXGI_FORMAT_B8G8R8A8_UNORM ), WeightFactorLarge);
		AddPriorityPatternFilter(FilterEqual< DXGI_FORMAT >(_T("ModeDesc.Format"), DXGI_FORMAT_R16G16B16A16_FLOAT ), WeightFactorSmall);
		AddPriorityPatternFilter(FilterEqual< DXGI_MODE_SCANLINE_ORDER >(_T("ModeDesc.ScanlineOrdering"), DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE), WeightFactorLarge);
		AddPriorityPatternFilter(FilterEqual< DXGI_MODE_SCALING >(_T("ModeDesc.Scaling"), DXGI_MODE_SCALING_STRETCHED ), WeightFactorLarge);
		AddPriorityPatternFilter(FilterEqual< DWORD >(_T("Rotation"), 3), WeightFactorSmall);
		AddPriorityPatternFilter(FilterEqual< DWORD >(_T("Rotation"), 2), WeightFactorSmall);

		DeclarePriorityLevelByPriorityValue(1, 2.0f);
		DeclarePriorityLevelByPriorityValue(2, 1.0f);
	}


Cleanup:
	DebugOut.Write("Exiting InitTestParameters.\n");
};


TEST_RESULT CPresentFS::Setup()
{
	//
	// Initialize locals.
	//

	TEST_RESULT tResult = RESULT_FAIL;
	HRESULT hResult = S_OK;

	//
	// Skip this group if we're testing Terminal Services.
	//

	if
	(
		g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION ) != 0
	)
	{
		tResult = RESULT_SKIP;
		goto Cleanup;
	};


	//
	// Initialize our window class.
	//

	bool bInitializeWindowClassResult = InitializeWindowClass();

	if ( bInitializeWindowClassResult != true )
	{
		WriteToLog
		(
			_T( "CPresentFS::Setup:  InitializeWindowClass failed." )
		);

		tResult = RESULT_ABORT;
		goto Cleanup;
	};


	//
	// Create our test window.
	//

	hResult = CreateTestWindows
	(
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT
	);

	if ( FAILED( hResult ) )
	{
		WriteToLog
		(
			"CPresentFS::Setup:  CreateTestWindows failed. hr = 0x%x", hResult
		);

		goto Cleanup;
	};
	//
	// Display the window.
	//

	ActivateWindow
	(
		m_hwndFocusWindow
	);


	m_hwndDeviceWindow = m_hwndFocusWindow;


	//
	// Initialize the device.
	//

	assert( m_DevicePointer.pDevice == NULL );
	hResult = g_Application.CreateFactoryAdapterDevice
	(
		&m_FactoryPointer,
		g_Application.m_DXGIOptions.AdapterIDSrc == ADAPTER_UNDEFINED ? 0 : g_Application.m_DXGIOptions.AdapterIDSrc,
		&m_AdapterPointer,
		g_Application.m_D3DOptions.SrcDriverType,
		D3D10_CREATE_DEVICE_SINGLETHREADED,
		m_TestFeatureLevel,
		0,
		&m_DevicePointer
	);

	if
	(
		hResult == DXGI_ERROR_NOT_FOUND &&
		m_AdapterPointer.pAdapter == NULL
	)
	{
		WriteToLog
		(
			"No adapter found at ordinal %u.",
			g_Application.m_DXGIOptions.AdapterIDSrc == ADAPTER_UNDEFINED ? 0 : g_Application.m_DXGIOptions.AdapterIDSrc
		);

		tResult = RESULT_SKIP;
		goto Cleanup;
	}
	else if
	(
		hResult == DXGI_ERROR_UNSUPPORTED ||
		hResult == E_NOINTERFACE ||
		hResult == E_NOTIMPL ||
		hResult == E_FAIL &&
		(
			m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_1 ||
			m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_2 ||
			m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_3 ||
			m_TestFeatureLevel == TEST_FEATURE_11_0_LEVEL_9_1 ||
			m_TestFeatureLevel == TEST_FEATURE_11_0_LEVEL_9_2 ||
			m_TestFeatureLevel == TEST_FEATURE_11_0_LEVEL_9_3
		)
	)
	{
		WriteToLog
		(
			"Could not create device."
		);

		tResult = RESULT_SKIP;
		goto Cleanup;
	}
	else if ( FAILED( hResult ) )
	{
		WriteToLog
		(
			"CPresentFS::Setup:  CreateFactoryAdapterDevice failed with error 0x%x.",
			hResult
		);

		goto Cleanup;
	};

	if ( m_DevicePointer.pDevice == NULL )
	{
		WriteToLog
		(
			"CPresentFS::Setup:  CreateFactoryAdapterDevice succeeded but returned NULL device pointer."
		);

		goto Cleanup;
	};


	PrimaryAdapterDevice.WrapDevice( m_DevicePointer.pDevice );


	////
	//// Associate the focus window with the factory.
	////

	//hResult = m_FactoryPointer.pFactory->MakeWindowAssociation
	//(
	//	m_hwndFocusWindow,
	//	0
	//);

	//if ( FAILED( hResult ) )
	//{
	//	WriteToLog
	//	(
	//		"CPresentFS::Setup:  MakeWindowAssociation failed with error 0x%x.",
	//		hResult
	//	);

	//	goto Cleanup;
	//};


	//
	// Break focus window association and release factory.
	//

	if ( m_FactoryPointer.pFactory != NULL )
	{
		HWND hwndCurrentFocus = NULL;

		m_FactoryPointer.pFactory->GetWindowAssociation( &hwndCurrentFocus );
		if ( hwndCurrentFocus != NULL )
		{
			m_FactoryPointer.pFactory->MakeWindowAssociation
			(
				NULL,
				0
			);
		};

		m_FactoryPointer.pFactory->Release();
		m_FactoryPointer.pFactory = NULL;
	};


	tResult = RESULT_PASS;


Cleanup:
	DwmIsCompositionEnabled( &m_bDWMState );


	return tResult;
};


void CPresentFS::Cleanup()
{
	DebugOut.Write( "Entering Cleanup.\n" );


	//
	// Release our swap chains.
	//

	if ( m_pSwapChain != NULL )
	{
		if(FAILED(m_pSwapChain->SetFullscreenState( FALSE, NULL )))
		{
			CheckForegroundWindow( m_szFocusWindowClassName );
		}

		m_pSwapChain->Release();
		m_pSwapChain = NULL;
	};

	if ( m_pSwapChainMS != NULL )
	{
		if(FAILED(m_pSwapChainMS->SetFullscreenState( FALSE, NULL )))
		{
			CheckForegroundWindow( m_szFocusWindowClassName );
		}

		m_pSwapChainMS->Release();
		m_pSwapChainMS = NULL;
	};


	//
	// Release device objects and device.
	//

	if ( m_DevicePointer.pDevice != NULL )
	{
		// @DX11PORT [SEdmison]:
		// Replace this.
		//D3DX10UnsetAllDeviceObjects( m_DevicePointer.pDevice );

		m_DevicePointer.pDevice->Release();
		m_DevicePointer.pDevice = NULL;
	};

	PrimaryAdapterDevice.ReleaseAllObjects();

	//
	// Release our adapter pointer.
	//

	if ( m_AdapterPointer.pAdapter != NULL )
	{
		m_AdapterPointer.pAdapter->Release();
		m_AdapterPointer.pAdapter = NULL;
	};


	//
	// Break focus window association and release factory.
	//

	if ( m_FactoryPointer.pFactory != NULL )
	{
		HWND hwndCurrentFocus = NULL;

		m_FactoryPointer.pFactory->GetWindowAssociation( &hwndCurrentFocus );
		if ( hwndCurrentFocus != NULL )
		{
			m_FactoryPointer.pFactory->MakeWindowAssociation
			(
				NULL,
				0
			);
		};

		m_FactoryPointer.pFactory->Release();
		m_FactoryPointer.pFactory = NULL;
	};


	//
	// Destroy our device window.
	//

	if ( m_hwndDeviceWindow != NULL )
	{
		if ( m_hwndDeviceWindow != m_hwndFocusWindow )
		{
			DestroyWindow( m_hwndDeviceWindow );
		};

		m_hwndDeviceWindow = NULL;
	};


	//
	// Destroy our focus window.
	//

	if ( m_hwndFocusWindow != NULL )
	{
		DestroyWindow( m_hwndFocusWindow );
		m_hwndFocusWindow = NULL;
	};


	g_Application.DwmEnableComposition( m_bDWMState );


	//
	// Unregister our window class.
	//

	CleanupWindowClass();


	DebugOut.Write( "Exiting Cleanup.\n" );
};


HRESULT CPresentFS::EnumerateAdapterModes
(
	std::vector < Adapter >* pAdapters,
	IDXGIAdapter * pDXGIAdapterEnumMode,
	UINT uiAdapterOrdinal
)
{
	//
	// Initialize locals.
	//

	HRESULT hresult = E_FAIL;
	IDXGIOutput * pDXGIOutputEnumMode = NULL;
	Adapter nextAdapter;
	pAdapters->push_back( nextAdapter );
	UINT uiAdapterCount = pAdapters->size();

	//
	// Enumerate outputs.
	//
	for
	(
		UINT uiOutputOrdinal = 0;
		uiOutputOrdinal < MAX_OUTPUTS;
		++uiOutputOrdinal
	)
	{
		//
		// Release the output from the prior iteration.
		//

		if ( pDXGIOutputEnumMode != NULL )
		{
			pDXGIOutputEnumMode->Release();
			pDXGIOutputEnumMode = NULL;
		};


		//
		// Enumerate the output at the current ordinal.
		//

		hresult = pDXGIAdapterEnumMode->EnumOutputs
		(
			uiOutputOrdinal,
			&pDXGIOutputEnumMode
		);


		if ( hresult == DXGI_ERROR_NOT_FOUND )
		{
			continue;
		}
		else if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CPresentFS::EnumerateAdapterModes:  EnumOutputs on output %u failed with error 0x%x.",
				uiOutputOrdinal,
				hresult
			);

			goto Cleanup;
		};

		if ( pDXGIOutputEnumMode == NULL )
		{
			WriteToLog
			(
				"CPresentFS::EnumerateAdapterModes:  EnumOutputs succeeded but returned NULL output pointer."
			);

			hresult = E_FAIL;
			goto Cleanup;
		};


		OutputModeContainer output;
		(*pAdapters)[uiAdapterCount-1].OutputModeContainer.push_back(output);

		//
		// Get a descriptor for the output.
		//

		DXGI_OUTPUT_DESC outputDesc;
		hresult = pDXGIOutputEnumMode->GetDesc( &outputDesc );
		if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CPresentFS::EnumerateAdapterModes:  GetDesc on output %u failed with error 0x%x.",
				uiOutputOrdinal,
				hresult
			);

			goto Cleanup;
		};


		//
		// Determine the current mode for the output.
		//

		DEVMODEW Mode;
		ZeroMemory( &Mode, sizeof( Mode ) );
		Mode.dmSize = sizeof( Mode );


		BOOL bGetCurrentModeResult = EnumDisplaySettingsW
		(
			outputDesc.DeviceName,
			ENUM_CURRENT_SETTINGS,
			&Mode
		);

		if ( bGetCurrentModeResult != TRUE )
		{
			DWORD GetCurrentModeError = GetLastError();

			WriteToLog
			(
				"CPresentFS::EnumerateAdapterModes:  EnumDisplaySettingsW failed with error %u.",
				GetCurrentModeError
			);

			if ( GetCurrentModeError != ERROR_SUCCESS )
			{
				hresult = HRESULT_FROM_WIN32( GetCurrentModeError );
			};

			goto Cleanup;
		};


		//
		// Find the mode for this monitor.
		//

		std::map< HMONITOR, DEVMODEW >::const_iterator itMode = m_DesktopMode.find( outputDesc.Monitor );
		if( m_DesktopMode.end() == itMode )
		{
			m_DesktopMode[ outputDesc.Monitor ] = Mode;
		};


		for
		(
			UINT modeRotation = DXGI_MODE_ROTATION_IDENTITY;
			modeRotation <= DXGI_MODE_ROTATION_ROTATE270;
			++modeRotation
		)
		{
			//
			// Only do DXGI_MODE_ROTATION_IDENTITY for non rotated cases
			//
			if
			(
				DXGI_MODE_ROTATION_IDENTITY != modeRotation &&
				!m_bTestRotatedModes
			)
			{
				continue;
			};


			//
			// don't do DXGI_MODE_ROTATION_IDENTITY for rotated cases
			//
			if
			(
				DXGI_MODE_ROTATION_IDENTITY == modeRotation &&
				m_bTestRotatedModes
			)
			{
				continue;
			};

			DEVMODEW rotatedMode = Mode;

			rotatedMode.dmFields = DM_BITSPERPEL |
				DM_PELSWIDTH |
				DM_PELSHEIGHT |
				DM_DISPLAYFLAGS |
				DM_DISPLAYFREQUENCY |
				DM_DISPLAYFIXEDOUTPUT |
				DM_DISPLAYORIENTATION;

			rotatedMode.dmDisplayFixedOutput = DMDFO_DEFAULT;

			//
			// If ENUM_CURRENT_SETTINGS is rotated 90 or 270 Width and Height are already swapped. Switch statement below expects unswapped
			// Width and Height. 
			//

			if(Mode.dmDisplayOrientation == DMDO_90 || Mode.dmDisplayOrientation == DMDO_180)
			{
				std::swap( rotatedMode.dmPelsWidth, rotatedMode.dmPelsHeight );    
			}


			switch ( modeRotation )
			{
				case DXGI_MODE_ROTATION_IDENTITY:
				{
					rotatedMode.dmDisplayOrientation = DMDO_DEFAULT;
				} break;

				case DXGI_MODE_ROTATION_ROTATE90:
				{
					std::swap( rotatedMode.dmPelsWidth, rotatedMode.dmPelsHeight );
					rotatedMode.dmDisplayOrientation = DMDO_90;
				} break;

				case DXGI_MODE_ROTATION_ROTATE180:
				{
					rotatedMode.dmDisplayOrientation = DMDO_180;
				} break;

				case DXGI_MODE_ROTATION_ROTATE270:
				{
					std::swap( rotatedMode.dmPelsWidth, rotatedMode.dmPelsHeight );
					rotatedMode.dmDisplayOrientation = DMDO_270;
				} break;
			};


			//
			// Switch to the mode.
			//
			LONG lResult = ChangeDisplaySettingsExW
			(	
				outputDesc.DeviceName,
				&rotatedMode,
				NULL,
				0,
				NULL
			);

			if(lResult != DISP_CHANGE_SUCCESSFUL)
			{
				WriteToLog
				(
					_T("attempted Mode: Adapter=%u Output=%u dmPelsWidth %d dmPelsHeight %d dmDisplayFrequency %d dmDisplayFlags %d dmFields %d dmDisplayFixedOutput %d dmDisplayOrientation %d"),
					uiAdapterOrdinal,
					uiOutputOrdinal,
					rotatedMode.dmPelsWidth,
					rotatedMode.dmPelsHeight,
					rotatedMode.dmDisplayFrequency,
					rotatedMode.dmDisplayFlags,
					rotatedMode.dmFields, 
					rotatedMode.dmDisplayFixedOutput, 
					rotatedMode.dmDisplayOrientation
				);
			}

			switch ( lResult )
			{
				case DISP_CHANGE_SUCCESSFUL:
				{
					/*WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW returned DISP_CHANGE_SUCCESSFUL."
					);*/

				} break;

				case DISP_CHANGE_BADDUALVIEW:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with DISP_CHANGE_BADDUALVIEW."
					);

					continue;
				} break;

				case DISP_CHANGE_BADFLAGS:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with DISP_CHANGE_BADFLAGS."
					);

					continue;
				} break;

				case DISP_CHANGE_BADMODE:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with DISP_CHANGE_BADMODE."
					);

					continue;
				} break;

				case DISP_CHANGE_BADPARAM:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with DISP_CHANGE_BADPARAM."
					);

					continue;
				} break;

				case DISP_CHANGE_FAILED:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with DISP_CHANGE_FAILED."
					);

					continue;
				} break;

				case DISP_CHANGE_NOTUPDATED:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with DISP_CHANGE_NOTUPDATED."
					);

					continue;
				} break;

				case DISP_CHANGE_RESTART:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with DISP_CHANGE_RESTART."
					);

					continue;
				} break;

				default:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with unexpected error %d.",
						lResult
					);

					continue;
				} break;
			};

			for
			(
				UINT uiFormatIndex = 0;
				uiFormatIndex < clsScanOutFormats::cuiNoOfScanOutFormats;
				++uiFormatIndex
			)
			{
				UINT uiModeCount = 0;

				//
				// Determine the number of supported modes for
				// this format.
				//

				hresult = pDXGIOutputEnumMode->GetDisplayModeList
				(
					clsScanOutFormats::DXGIScanOutFormats[ uiFormatIndex ],
					DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING,
					&uiModeCount,
					NULL
				);

				if ( FAILED( hresult ) )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes: Adapter=%u Output=%u foramt %d GetDisplayModeList (1st call) failed with error 0x%x.",
						uiAdapterOrdinal,
						uiOutputOrdinal,
						clsScanOutFormats::DXGIScanOutFormats[ uiFormatIndex ],
						hresult
					);

					continue;
				};


				if ( uiModeCount == 0 )
				{
					/*
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  GetDisplayModeList returned 0 modes for this format."
					);
					*/

					continue;
				};


				DXGI_MODE_DESC * pModeDescArray = new DXGI_MODE_DESC[ uiModeCount ];

				if( pModeDescArray == NULL )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  Failed to allocate mode descriptor array."
					);

					hresult = E_OUTOFMEMORY;
					goto Cleanup;
				};

				ZeroMemory( pModeDescArray, uiModeCount * sizeof( DXGI_MODE_DESC ) );


				//
				// Get the array of supported modes for this format.
				//

				hresult = pDXGIOutputEnumMode->GetDisplayModeList
				(
					clsScanOutFormats::DXGIScanOutFormats[ uiFormatIndex ],
					DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING,
					&uiModeCount,
					pModeDescArray
				);

				if ( FAILED( hresult ) )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  GetDisplayModeList failed with error 0x%x.",
						hresult
					);

					delete [] pModeDescArray;
					pModeDescArray = NULL;

					goto Cleanup;
				};

				for(UINT modes = 0; modes < uiModeCount; modes++)
				{

					// DXGI_MODE_DESC dimensions are rotated
					if( !g_Application.IsUseDesktopResolution() || ( (pModeDescArray + modes)->Width == rotatedMode.dmPelsWidth && (pModeDescArray + modes)->Height == rotatedMode.dmPelsHeight) )
					{
						ModeInformation ModeInit;

						ModeInit.uiDXGIAdapterOrdinal = uiAdapterOrdinal;
						ModeInit.uiDXGIOutputOrdinal = uiOutputOrdinal;
						ModeInit.ModeDesc = *(pModeDescArray + modes);
						ModeInit.uiMultisampleCount = 1;
						ModeInit.uiMultisampleQuality = 0;
						ModeInit.isInitialized = TRUE;
						ModeInit.hmonitor = outputDesc.Monitor;
						ZeroMemory( &ModeInit.devmode, sizeof( ModeInit.devmode ) );
						ModeInit.devmode.dmDisplayOrientation = rotatedMode.dmDisplayOrientation;						
						(*pAdapters)[uiAdapterCount-1].OutputModeContainer[uiOutputOrdinal].RawModes[(UINT)modeRotation - 1].push_back( ModeInit );
					}

					/*DebugOut.Write("output %d DXGIScanOutFormats %d rotation %d ModeDesc.Width %d ModeDesc.Height %d ModeDesc.RefreshRate.Numerator %d ModeDesc.RefreshRate.Denominator %d ModeDesc.ScanlineOrdering %d ModeDesc.Scaling %d uiMultisampleCount %d uiMultisampleQuality %d \n" ,
					uiOutputOrdinal,
					uiFormatIndex,
					modeRotation,
					ModeInit.ModeDesc.Width, 
					ModeInit.ModeDesc.Height,
					ModeInit.ModeDesc.RefreshRate.Numerator,
					ModeInit.ModeDesc.RefreshRate.Denominator,
					ModeInit.ModeDesc.ScanlineOrdering,
					ModeInit.ModeDesc.Scaling,
					ModeInit.uiMultisampleCount,
					ModeInit.uiMultisampleQuality);*/
				}

				delete [] pModeDescArray;
				pModeDescArray = NULL;
			};

		};


		pDXGIOutputEnumMode->Release();
		pDXGIOutputEnumMode = NULL;


		//
		// Restore the output to the original mode.
		//

		LONG lResult = ChangeDisplaySettingsExW
		(
			outputDesc.DeviceName,
			&Mode,
			NULL,
			0,
			NULL
		);

		if ( lResult != DISP_CHANGE_SUCCESSFUL )
		{
			WriteToLog
			(
				"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsExW failed with error %d while restoring the mode.",
				lResult
			);
		};
	};

	//
	// If we made it this far, then we're good to go.
	//

	hresult = S_OK;


Cleanup:
	if ( pDXGIOutputEnumMode != NULL )
	{
		pDXGIOutputEnumMode->Release();
		pDXGIOutputEnumMode = NULL;
	};


	return hresult;
};


HRESULT CPresentFS::EnumerateModes(std::vector < Adapter >* pAdapters)
{
	//
	// Initialize locals.
	//
	HRESULT hresult = E_FAIL;

	DXGI_FACTORY_POINTER FactoryPointer;
	DXGI_ADAPTER_POINTER AdapterPointer;
	AdapterPointer.pAdapter = NULL;

	hresult = g_Application.CreateDXGIFactory
	(
		&FactoryPointer
	);

	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentFS::EnumerateModes:  CreateDXGIFactory failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	if ( FactoryPointer.pFactory == NULL )
	{
		WriteToLog
		(
			"CPresentFS::EnumerateModes:  CreateFactory succeeded but returned a NULL factory pointer."
		);

		hresult = E_FAIL;
		goto Cleanup;
	};

	switch ( g_Application.m_D3DOptions.SrcDriverType )
	{
		case D3D10_DRIVER_TYPE_SOFTWARE:
		case D3D10_DRIVER_TYPE_REFERENCE:
		case D3D10_DRIVER_TYPE_NULL:
		{
			HMODULE hRasterizer = NULL;
			if ( g_Application.m_D3DOptions.SrcDriverType == D3D10_DRIVER_TYPE_REFERENCE )
			{
				hRasterizer = LoadLibraryExW
				(
					D3D_REF_DLLNAME,
					NULL,
					0
				);

				if ( hRasterizer == NULL )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateModes:  Failed to load reference rasterizer."
					);

					goto Cleanup;
				};
			}
			else if ( g_Application.m_D3DOptions.SrcDriverType == D3D10_DRIVER_TYPE_SOFTWARE )
			{
				hRasterizer = LoadLibraryExW
				(
					g_Application.m_D3DOptions.SoftwareRasterizerDLLName,
					NULL,
					0
				);

				if ( hRasterizer == NULL )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateModes:  Failed to load software rasterizer."
					);

					goto Cleanup;
				};
			};


			//
			// Create a software adapter.
			//

			hresult = FactoryPointer.pFactory->CreateSoftwareAdapter
			(
				hRasterizer,
				&AdapterPointer.pAdapter
			);


			if ( hRasterizer != NULL )
			{
				FreeLibrary( hRasterizer );
				hRasterizer = NULL;
			};

			if ( FAILED( hresult ) )
			{
				WriteToLog
				(
					"CPresentFS::EnumerateModes:  CreateSoftwareAdapter failed with error 0x%x.",
					hresult
				);

				goto Cleanup;
			};

			if ( AdapterPointer.pAdapter == NULL )
			{
				WriteToLog
				(
					"CPresentFS::EnumerateModes:  CreateSoftwareAdapter succeeded but returned a NULL software adapter pointer."
				);

				goto Cleanup;
			};


			//
			// Enumerate outputs and modes for this adapter.
			//
			hresult = EnumerateAdapterModes
			(
				pAdapters,
				AdapterPointer.pAdapter,
				0
			);

			if ( FAILED( hresult ) )
			{
				WriteToLog
				(
					"CPresentFS::EnumerateModes:  EnumerateAdapterModes failed with error 0x%x.",
					hresult
				);

				goto Cleanup;
			};
		} break;


		case D3D10_DRIVER_TYPE_HARDWARE:
		{
			//
			// Enumerate outputs and modes for all adapters.
			//
			for
			(
				UINT uiAdapterOrdinal = 0;
				uiAdapterOrdinal < MAX_ADAPTERS;
				++uiAdapterOrdinal
			)
			{
				//
				// Release the adapter from the prior iteration.
				//

				if ( AdapterPointer.pAdapter != NULL )
				{
					AdapterPointer.pAdapter->Release();
					AdapterPointer.pAdapter = NULL;
				};

				// 
				// If there is a value for Adapter command line parameter
				// only enumerate modes for that adapter.
				//
				if( g_Application.m_DXGIOptions.AdapterIDSrc != ADAPTER_UNDEFINED &&
					g_Application.m_DXGIOptions.AdapterIDSrc != uiAdapterOrdinal)
				{
					continue;
				}

				//
				// Enumerate the adapter at the specified ordinal.
				//

				hresult = FactoryPointer.pFactory->EnumAdapters
				(
					uiAdapterOrdinal,
					&AdapterPointer.pAdapter
				);

				if ( hresult == DXGI_ERROR_NOT_FOUND )
				{
					break;
				}
				else if ( FAILED( hresult ) )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateModes:  EnumAdapters on adapter %u failed with error 0x%x.",
						uiAdapterOrdinal,
						hresult
					);

					goto Cleanup;
				};

				if ( AdapterPointer.pAdapter == NULL )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateModes:  EnumAdapters on adapter %u succeeded but returned a NULL adapter pointer.",
						uiAdapterOrdinal
					);

					hresult = E_FAIL;
					goto Cleanup;
				};

				DXGI_ADAPTER_DESC AdapterDescriptor;

				if ( AdapterPointer.pAdapter->GetDesc( &AdapterDescriptor ) == S_OK )
				{
					m_vecAdapterDescriptors.push_back( AdapterDescriptor );
					if( AdapterDescriptor.Description != NULL )
					{
						TCHAR* pszAdapterDesc = new TCHAR[ 128 ];

						// copied from CDXGITestFramework::UpdateCurrentDeviceInfo. WCHAR->TCHAR
#ifdef UNICODE
						StringCchCopy
						(
							pszAdapterDesc,
							128,
							AdapterDescriptor.Description
						);
#else
						int cCharactersConverted = WideCharToMultiByte
						(
							CP_ACP,
							WC_NO_BEST_FIT_CHARS,
							AdapterDescriptor.Description,
							-1,
							pszAdapterDesc,
							128,
							NULL,
							NULL
						);

						assert( cCharactersConverted > 0 );
#endif

						if( g_Application.m_DXGIOptions.Version >= TEST_DXGI_VERSION_1_1 && g_Application.m_DXGIOptions.Version < TEST_DXGI_VERSION_BESTAVAILABLE ) 
						{
							// AdapterType[0] is the Src adapter type
							if( g_Application.IsAdapterOfType( AdapterPointer.pAdapter1, g_Application.m_DXGIOptions.AdapterType[0] )  )
							{
								WriteToLog
								(
									"CFSTestCases::EnumerateModes:  Adding adapter at AdapterOrdinal: %d AdapterDescription: %s",
									uiAdapterOrdinal,
									pszAdapterDesc
								);
							}
							else
							{
								// ignore HW adapters
								delete [] pszAdapterDesc;
								pszAdapterDesc = NULL;

								SAFE_RELEASE( AdapterPointer.pAdapter );

								continue;                    
							}
						}

						delete [] pszAdapterDesc;
						pszAdapterDesc = NULL;
					}
				}

				//
				// Enumerate outputs and modes for this adapter.
				//
				hresult = EnumerateAdapterModes
				(
					pAdapters,	
					AdapterPointer.pAdapter,
					uiAdapterOrdinal
				);

				if ( FAILED( hresult ) )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateModes:  EnumerateAdapterModes failed with error 0x%x.",
						hresult
					);

					goto Cleanup;
				};


				//
				// Release the adapter.
				//

				if ( AdapterPointer.pAdapter != NULL )
				{
					AdapterPointer.pAdapter->Release();
					AdapterPointer.pAdapter = NULL;
				};
			};


			//
			// Release the factory.
			//

			FactoryPointer.pFactory->Release();
			FactoryPointer.pFactory = NULL;
		} break;


		default:
		{
			assert( FALSE );
		} break;
	};


	hresult = S_OK;


Cleanup:
	if ( AdapterPointer.pAdapter != NULL )
	{
		AdapterPointer.pAdapter->Release();
		AdapterPointer.pAdapter = NULL;
	};


	if ( FactoryPointer.pFactory != NULL )
	{
		FactoryPointer.pFactory->Release();
		FactoryPointer.pFactory = NULL;
	};


	return hresult;
};


HRESULT CPresentFS::SetDisplayMode
(
	IDXGIOutput * pTargetOutput
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Validate parameters.
	//

	if ( pTargetOutput == NULL )
	{
		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Get a descriptor for the output.
	//

	DXGI_OUTPUT_DESC outputDesc;
	HRESULT hGetDescResult = pTargetOutput->GetDesc( &outputDesc );

	if ( FAILED( hGetDescResult ) )
	{
		WriteToLog
		(
			"GetDesc on target output failed with error 0x%x.",
			hGetDescResult
		);

		hOverallResult = hGetDescResult;
		goto Cleanup;
	};

	hOverallResult = hGetDescResult;


	//
	// Rotate the display if necessary.
	//

	if ( outputDesc.Rotation != m_RotationMode  )
	{
		//
		// Initialize our mode structure.
		//

		DEVMODEW dmModeToChangeTo;
		dmModeToChangeTo.dmSize = sizeof( DEVMODEW );
		dmModeToChangeTo.dmSpecVersion = DM_SPECVERSION;


		//
		// Get the current desktop mode.
		//

		BOOL bGetCurrentModeResult = EnumDisplaySettingsW
		(
			outputDesc.DeviceName,
			ENUM_CURRENT_SETTINGS,
			&dmModeToChangeTo
		);

		if ( bGetCurrentModeResult != TRUE )
		{
			DWORD GetCurrentModeError = GetLastError();

			WriteToLog
			(
				"CPresentFS::SetDisplayMode:  EnumDisplaySettingsW failed with error %u.",
				GetCurrentModeError
			);

			if ( GetCurrentModeError != ERROR_SUCCESS )
			{
				hOverallResult = HRESULT_FROM_WIN32( GetCurrentModeError );
			};

			goto Cleanup;
		};

		// Get GDI as close as we know how. GDI rotation should be preserved.
		dmModeToChangeTo.dmBitsPerPel = CFormatInfoMap::Find( m_SCDesc.BufferDesc.Format )->uBitsPerElement;

		switch (dmModeToChangeTo.dmBitsPerPel)
		{
			case 16:
			case 32:
			break;

			default:
				//if we'd confuse GDI, we'll substitute something it'll grok
				dmModeToChangeTo.dmBitsPerPel = 32;
		}


		dmModeToChangeTo.dmPelsWidth = m_ModeInformation.ModeDesc.Width;
		dmModeToChangeTo.dmPelsHeight = m_ModeInformation.ModeDesc.Height;
		dmModeToChangeTo.dmDisplayOrientation = DxgiModeRotationToDisplayOrientation(m_RotationMode);

		dmModeToChangeTo.dmDisplayFrequency = UINT( double( m_SCDesc.BufferDesc.RefreshRate.Numerator ) / m_SCDesc.BufferDesc.RefreshRate.Denominator + 0.5 );
		dmModeToChangeTo.dmDisplayFlags =
			(m_SCDesc.BufferDesc.ScanlineOrdering != DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE ? DM_INTERLACED : 0);

		dmModeToChangeTo.dmFields = (dmModeToChangeTo.dmFields & DM_POSITION) | /* Preserve position if previously specified */
			DM_BITSPERPEL |
			DM_PELSWIDTH |
			DM_PELSHEIGHT |
			DM_DISPLAYFLAGS |
			DM_DISPLAYFIXEDOUTPUT |
			DM_DISPLAYFREQUENCY |
			DM_DISPLAYORIENTATION;

		switch (m_SCDesc.BufferDesc.Scaling)
		{
			case (DXGI_MODE_SCALING_UNSPECIFIED): dmModeToChangeTo.dmDisplayFixedOutput = DMDFO_DEFAULT; break;
			case (DXGI_MODE_SCALING_STRETCHED): dmModeToChangeTo.dmDisplayFixedOutput = DMDFO_STRETCH; break;
			case (DXGI_MODE_SCALING_CENTERED): dmModeToChangeTo.dmDisplayFixedOutput = DMDFO_CENTER; break;
		}

		dmModeToChangeTo.dmDeviceName[0] = 0;


		//
		// Attempt the mode switch.
		//


		hOverallResult = E_FAIL;
		LONG lResult = ChangeDisplaySettingsExW
		(
			outputDesc.DeviceName,
			&dmModeToChangeTo,
			NULL,
			0,
			NULL
		);

		if( lResult != DISP_CHANGE_SUCCESSFUL )
		{        
			WriteToLog(_T("dmModeToChangeTo.dmPelsWidth %d dmModeToChangeTo.dmPelsHeight %d dmModeToChangeTo.dmDisplayFrequency %d dmModeToChangeTo.dmDisplayFlags %d dmModeToChangeTo.dmFields %d dmModeToChangeTo.dmDisplayFixedOutput %d dmModeToChangeTo.dmDisplayOrientation %d"),dmModeToChangeTo.dmPelsWidth,dmModeToChangeTo.dmPelsHeight,dmModeToChangeTo.dmDisplayFrequency,dmModeToChangeTo.dmDisplayFlags,dmModeToChangeTo.dmFields, dmModeToChangeTo.dmDisplayFixedOutput, dmModeToChangeTo.dmDisplayOrientation);

			DXGI_MODE_DESC  matchDesc = { 0 };
			DXGI_MODE_DESC  closestDesc = { 0 };
			memcpy( &matchDesc, &m_ModeInformation.ModeDesc, sizeof(DXGI_MODE_DESC));

			//
			// Find closest match does not work with _SRGB
			//
			if(m_ModeInformation.ModeDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB)
			{
				matchDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			}
			else if(m_ModeInformation.ModeDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
			{
				matchDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			}

			HRESULT hrFindClosest = pTargetOutput->FindClosestMatchingMode(
				&matchDesc,
				&closestDesc,
				NULL);

			if( SUCCEEDED(hrFindClosest) )
			{
				assert( matchDesc.Format == closestDesc.Format );
				dmModeToChangeTo.dmDisplayFrequency = 0;
				dmModeToChangeTo.dmFields &= ~DM_DISPLAYFIXEDOUTPUT;
				dmModeToChangeTo.dmDisplayFlags =
					(closestDesc.ScanlineOrdering != DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE ? DM_INTERLACED : 0);
				dmModeToChangeTo.dmDisplayFixedOutput = 0;
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  Resorted to use FindClosestMatchingMode: BufferDesc Mode = %d x %d @ %f Hz, %s, %s.",
					closestDesc.Width,
					closestDesc.Height,
					(float) closestDesc.RefreshRate.Numerator /
					(float) closestDesc.RefreshRate.Denominator,
					szScanLineNames[ (UINT) closestDesc.ScanlineOrdering ],
					szScalingNames[ (UINT) closestDesc.Scaling ]
				);

			}

			lResult = ChangeDisplaySettingsExW
			(
				outputDesc.DeviceName,
				&dmModeToChangeTo,
				NULL,
				CDS_TRYCLOSEST,
				NULL
			);

			if( lResult == DISP_CHANGE_SUCCESSFUL )
			{
				if ( SUCCEEDED(hrFindClosest) )
				{
					m_SCDesc.BufferDesc.RefreshRate.Numerator = closestDesc.RefreshRate.Numerator;
					m_SCDesc.BufferDesc.RefreshRate.Denominator = closestDesc.RefreshRate.Denominator;
					m_SCDesc.BufferDesc.Scaling = closestDesc.Scaling;
					m_SCDesc.BufferDesc.ScanlineOrdering = closestDesc.ScanlineOrdering;
				}

				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  Resorted to CDS_TRYCLOSEST mode."
				);
			} 
			else
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode: ChangeDisplaySettingsExW could not set mode using CDS_TRYCLOSEST ."
				);
			}
		}

		switch( lResult )
		{
			case DISP_CHANGE_SUCCESSFUL:
			{
				EnumDisplaySettingsW
				(
					outputDesc.DeviceName,
					ENUM_CURRENT_SETTINGS,
					&m_ModeInformation.devmode
				);

				/*
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW returned DISP_CHANGE_SUCCESSFUL."
				);
				*/

				hOverallResult = S_OK;
			} break;

			case DISP_CHANGE_BADDUALVIEW:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with error DISP_CHANGE_BADDUALVIEW."
				);
			} break;

			case DISP_CHANGE_BADFLAGS:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with error DISP_CHANGE_BADFLAGS."
				);
			} break;

			case DISP_CHANGE_BADMODE:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with error DISP_CHANGE_BADMODE."
				);
			} break;

			case DISP_CHANGE_BADPARAM:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with error DISP_CHANGE_BADPARAM."
				);
			} break;

			case DISP_CHANGE_FAILED:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with error DISP_CHANGE_FAILED."
				);
			} break;

			case DISP_CHANGE_NOTUPDATED:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with error DISP_CHANGE_NOTUPDATED."
				);
			} break;

			case DISP_CHANGE_RESTART:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with error DISP_CHANGE_RESTART."
				);
			} break;

			default:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with unexpected error %d.",
					lResult
				);
			} break;
		};
	};


Cleanup:
	return hOverallResult;
};


TEST_RESULT CPresentFS::CreateDeviceAndSwapChain()
{
	//
	// Initialize locals.
	//

	TEST_RESULT test_result = RESULT_FAIL;
	HRESULT hresult = E_FAIL;
	IDXGIOutput * pTargetOutput = NULL;


	//
	// Release objects that we might be holding.
	//

	ReleaseTestCaseObjects();


	//
	// Release device objects and device.
	//

	if ( m_DevicePointer.pDevice != NULL )
	{
		// @DX11PORT [SEdmison]:
		// Replace this.
		//D3DX10UnsetAllDeviceObjects( m_DevicePointer.pDevice );

		m_DevicePointer.pDevice->Release();
		m_DevicePointer.pDevice = NULL;
	};

	PrimaryAdapterDevice.ReleaseAllObjects();


	//
	// Turn DWM off if necessary.
	//

	BOOL bDWMState = FALSE;
	DwmIsCompositionEnabled( &bDWMState );
	if ( bDWMState == TRUE )
	{
		g_Application.DwmEnableComposition( FALSE );
		Sleep( 1000 );
	};


	//
	// Create our device.
	//

	assert( m_DevicePointer.pDevice == NULL );
	hresult = g_Application.CreateFactoryAdapterDevice
	(
		&m_FactoryPointer,
		m_ModeInformation.uiDXGIAdapterOrdinal,
		&m_AdapterPointer,
		g_Application.m_D3DOptions.SrcDriverType,
		D3D10_CREATE_DEVICE_SINGLETHREADED,
		m_TestFeatureLevel,
		0,
		&m_DevicePointer
	);


	if
	(
		hresult == DXGI_ERROR_NOT_FOUND &&
		m_AdapterPointer.pAdapter == NULL
	)
	{
		WriteToLog
		(
			"No adapter found at ordinal %u.",
			m_ModeInformation.uiDXGIAdapterOrdinal
		);

		test_result = RESULT_SKIP;
		goto Cleanup;
	}
	else if
	(
		hresult == DXGI_ERROR_UNSUPPORTED ||
		hresult == E_NOINTERFACE ||
		hresult == E_NOTIMPL
	)
	{
		WriteToLog
		(
			"Could not create device."
		);

		test_result = RESULT_SKIP;
		goto Cleanup;
	}
	else if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  CreateFactoryAdapterDevice failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	if ( m_DevicePointer.pDevice == NULL )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  CreateFactoryAdapterDevice succeeded but returned NULL device pointer."
		);

		goto Cleanup;
	};


	//
	// Wrap the device.
	//

	PrimaryAdapterDevice.WrapDevice( m_DevicePointer.pDevice );


	//
	// Check for format support.
	//

	UINT FormatSupport = 0;
	hresult = m_DevicePointer.pDevice->CheckFormatSupport
	(
		m_ModeInformation.ModeDesc.Format,
		&FormatSupport
	);

	if ( hresult == E_FAIL )
	{
		WriteToLog
		(
			"Device does not support this format."
		);

		test_result = RESULT_SKIP;
		goto Cleanup;
	}
	else if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  CheckFormatSupport failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	UINT RequiredFormatSupport = m_RequiredFormatSupport |
		( m_ModeInformation.uiMultisampleCount > 1 ? D3D10_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET : 0 );

	//
	// DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM does not support Render target.  
	// Will need to create a DXGI_FORMAT_R10G10B10_A2_UNORM render target.
	//
	if(m_ModeInformation.ModeDesc.Format == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM)
	{
		RequiredFormatSupport = RequiredFormatSupport & ~D3D10_FORMAT_SUPPORT_RENDER_TARGET;
	}


	WriteToLog(_T("D3D10_FORMAT_SUPPORT_TEXTURE2D %d, D3D10_FORMAT_SUPPORT_RENDER_TARGET = %d, D3D10_FORMAT_SUPPORT_CPU_LOCKABLE = %d D3D10_FORMAT_SUPPORT_DISPLAY %d"),
		FormatSupport & D3D10_FORMAT_SUPPORT_TEXTURE2D,
		FormatSupport & D3D10_FORMAT_SUPPORT_RENDER_TARGET,
		FormatSupport & D3D10_FORMAT_SUPPORT_CPU_LOCKABLE,
		FormatSupport & D3D10_FORMAT_SUPPORT_DISPLAY);



	if ( ( FormatSupport & RequiredFormatSupport ) != RequiredFormatSupport )
	{
		WriteToLog
		(
			"Device does not support capabilities with this format needed to test it."
		);

		test_result = RESULT_SKIP;
		goto Cleanup;
	};


	//
	// Check for multisample support, and skip if unsupported.
	//

	UINT uSupportedMultisampleQuality = 0;
	if
	(
		m_ModeInformation.uiMultisampleCount > 1 &&
		(
			FAILED
			(
				m_DevicePointer.pDevice->CheckMultisampleQualityLevels
				(
					m_ModeInformation.ModeDesc.Format,
					m_ModeInformation.uiMultisampleCount,
					&uSupportedMultisampleQuality
				)
			) ||
			uSupportedMultisampleQuality <= (UINT) m_ModeInformation.uiMultisampleQuality
		)
	)
	{
		WriteToLog
		(
			"Device does not support sample quality %u.",
			m_ModeInformation.uiMultisampleQuality
		);

		test_result = RESULT_SKIP;
		goto Cleanup;
	};


	//
	// Show and activate the focus window.
	//

	ActivateWindow
	(
		m_hwndFocusWindow
	);


	ProcessMessages();


	//
	// Sanity-check our adapter and factory pointers.
	//

	assert( m_AdapterPointer.pAdapter != NULL );
	assert( m_FactoryPointer.pFactory != NULL );


	//
	// Associate our focus window with the DXGI factory.
	//

	DebugOut.Write( "CPresentFS::CreateDeviceAndSwapChain:  Calling MakeWindowAssociation.\n" );
	hresult = m_FactoryPointer.pFactory->MakeWindowAssociation
	(
		m_hwndFocusWindow,
		0
	);

	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  MakeWindowAssociation failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};


	//
	// Specify the swap chain properties.
	//

	ZeroMemory((void *)&m_SCDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	//m_SCDesc.BufferDesc = m_ModeInformation.ModeDesc;
	memcpy( &m_SCDesc.BufferDesc, &m_ModeInformation.ModeDesc, sizeof( DXGI_MODE_DESC ) );


	PrimaryAdapterDevice.SetRotationMode
	(
		m_bNonPrerotated
		? m_RotationMode
		: DXGI_MODE_ROTATION_IDENTITY
	);

	IDXGIOutput * pLocalTargetOutput = NULL;
	DXGI_OUTPUT_DESC desc;
	ZeroMemory(&desc,sizeof(DXGI_OUTPUT_DESC));

	hresult = m_AdapterPointer.pAdapter->EnumOutputs
	(
		m_ModeInformation.uiDXGIOutputOrdinal,
		&pLocalTargetOutput
	);

	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  EnumOutputs on adapter %u, output %u failed with error 0x%x.",
			m_ModeInformation.uiDXGIAdapterOrdinal,
			m_ModeInformation.uiDXGIOutputOrdinal,
			hresult
		);

		goto Cleanup;
	};

	pLocalTargetOutput->GetDesc(&desc);
	if ( m_RotationMode != DXGI_MODE_ROTATION_IDENTITY )
	{
		if
		(
			!m_bNonPrerotated &&
			(
				DXGI_MODE_ROTATION_ROTATE90 == m_RotationMode ||
				DXGI_MODE_ROTATION_ROTATE270 == m_RotationMode
			)
		)
		{
			//
			// App doesn't know about rotation. Create the swapchain with unrotated values
			// 
			std::swap
			(
				m_SCDesc.BufferDesc.Width,
				m_SCDesc.BufferDesc.Height
			);

			WriteToLog
			(
				"BufferDesc Mode = %d x %d ",
				m_SCDesc.BufferDesc.Width,
				m_SCDesc.BufferDesc.Height
			);
		};

		//
		// Set Display mode for the test if this is a rotation test
		//
		hresult = SetDisplayMode( pLocalTargetOutput );
		if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CPresentFS::CreateDeviceAndSwapChain: SetDisplayMode failed with error 0x%x.",
				hresult
			);

			test_result = RESULT_SKIP;
			goto Cleanup;
		};
	};

	if (pLocalTargetOutput)
	{
		pLocalTargetOutput->Release();
		pLocalTargetOutput = NULL;
	}


	//
	// Move the window onto the target output so createswapchain can find it, if the window was created on a different
	// adpater.
	//

	if(desc.AttachedToDesktop)
	{
		SetWindowPos
		(
			m_hwndDeviceWindow,
			HWND_TOP,
			desc.DesktopCoordinates.left,
			desc.DesktopCoordinates.top,
			0,
			0,
			SWP_NOSIZE
		);
	}

	m_SCDesc.SampleDesc.Count = 1;
	m_SCDesc.SampleDesc.Quality = 0;

	m_SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	m_SCDesc.BufferCount = m_BufferCount;

	m_SCDesc.OutputWindow = m_hwndDeviceWindow;

	m_SCDesc.Windowed = ( m_ModeInformation.uiMultisampleCount > 1 ) ? TRUE : FALSE;

	m_SCDesc.SwapEffect = m_SwapEffect.dxgiSwapEffect;

	m_SCDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if ( m_bNonPrerotated && !m_SCDesc.Windowed )
	{
		m_SCDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_NONPREROTATED;
	};


	DXGI_SWAP_CHAIN_DESC SCMSDesc;
	memcpy( &SCMSDesc, &m_SCDesc, sizeof( DXGI_SWAP_CHAIN_DESC ) );
	SCMSDesc.SampleDesc.Count = m_ModeInformation.uiMultisampleCount;
	SCMSDesc.SampleDesc.Quality = m_ModeInformation.uiMultisampleQuality;


	//
	// Dump swap chain parameters.
	//

	{
		const DXGIFormatInfo *pEnumeratedModeFormatRecord = CFormatInfoMap::Find
		(
			m_SCDesc.BufferDesc.Format
		);

		WriteToLog
		(
			"BufferDesc Mode = %d x %d @ %f Hz (%s), %s, %s.",
			m_SCDesc.BufferDesc.Width,
			m_SCDesc.BufferDesc.Height,
			(float) m_SCDesc.BufferDesc.RefreshRate.Numerator /
			(float) m_SCDesc.BufferDesc.RefreshRate.Denominator,
			pEnumeratedModeFormatRecord
				? pEnumeratedModeFormatRecord->szDesc
				: "(Unrecognized format)",
			szScanLineNames[ (UINT) m_SCDesc.BufferDesc.ScanlineOrdering ],
			szScalingNames[ (UINT) m_SCDesc.BufferDesc.Scaling ]
		);
	};


	//
	// Attempt the swap chain creation.
	//

	DebugOut.Write("CPresentFS::CreateDeviceAndSwapChain:  Initializing swap chain.\n");

	if
	(
		TestedSwapChain.InitializeSwapChain( &m_SCDesc ) != TRUE
	)
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  InitializeSwapChain failed."
		);

		if( m_bLargeMemoryUsage && TestedSwapChain.GetLastError() == E_OUTOFMEMORY )
		{
			test_result = RESULT_SKIP;
			WriteToLog( "ExecuteTestCase: Test case using large amount of memory. Swapchain creation failure expected. Skipping test case. " );
		}

		goto Cleanup;
	};


	if ( m_ModeInformation.uiMultisampleCount > 1 )
	{
		if
		(
			TestedSwapChainMS.InitializeSwapChain( &SCMSDesc ) != TRUE
		)
		{
			WriteToLog
			(
				"CPresentFS::CreateDeviceAndSwapChain:  InitializeSwapChain on multisampled swap chain failed."
			);

			if( m_bLargeMemoryUsage && TestedSwapChainMS.GetLastError() == E_OUTOFMEMORY )
			{
				test_result = RESULT_SKIP;
				WriteToLog( "ExecuteTestCase: Test case using large amount of memory. Swapchain creation failure expected. Skipping test case. " );
			}

			goto Cleanup;
		};
	};


	//
	// Get references to our swap chains.
	//

	assert( m_pSwapChain == NULL );
	TestedSwapChain.GetSwapChainPtr( &m_pSwapChain );
	if ( m_pSwapChain == NULL )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  Swap chain pointer is NULL."
		);

		test_result = RESULT_ABORT;
		goto Cleanup;
	};

	assert( m_pSwapChainMS == NULL );
	TestedSwapChainMS.GetSwapChainPtr( &m_pSwapChainMS );
	if ( m_ModeInformation.uiMultisampleCount > 1 && m_pSwapChainMS == NULL )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  Multisample swap chain pointer is NULL."
		);

		test_result = RESULT_ABORT;
		goto Cleanup;
	};


	//
	// Dump the back buffer format.
	//

	DebugOut.Write("***********************************\n");
	DebugOut.Write("CPresentFS::CreateDeviceAndSwapChain:  InitializeSwapChain succeeded!\n");

	{
		const DXGIFormatInfo *pBBFormatInfo = NULL;

		if (pBBFormatInfo = CFormatInfoMap::Find((m_SCDesc.BufferDesc).Format))
		{
			DebugOut.Write(pBBFormatInfo->szDesc);
			DebugOut.Write("\n");
		}
	}

	DebugOut.Write("***********************************\n");


	//
	// Enumerate the target output.
	//
	hresult = m_AdapterPointer.pAdapter->EnumOutputs
	(
		m_ModeInformation.uiDXGIOutputOrdinal,
		&pTargetOutput
	);

	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  EnumOutputs on adapter %u, output %u failed with error 0x%x.",
			m_ModeInformation.uiDXGIAdapterOrdinal,
			m_ModeInformation.uiDXGIOutputOrdinal,
			hresult
		);

		goto Cleanup;
	};


	//
	// Set the swap chain to full-screen on the target output.
	//

	if
	(
		m_SCDesc.Windowed == FALSE
	)
	{
		hresult = m_pSwapChain->SetFullscreenState
		(
			TRUE,
			pTargetOutput
		);

		if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CPresentFS::CreateDeviceAndSwapChain:  SetFullscreenState failed with error 0x%x.",
				hresult
			);

			CheckForegroundWindow( m_szFocusWindowClassName );
			// Dump DXGI Journal to get clues as to why SetFullscreenState failed 
			if( g_Application.m_bLogDXGIJounrnal )
				g_Application.InitiateJournalDump( true );
			
			goto Cleanup;
		};

		//
		// If m_bNonPrerotated == 0, that means the ChangeDisplaySettingsEx was called to rotate the display.
		// Seems pTargetOutput->GetDesc only returns modes set by ChangeDisplaySettingsEx not SetFullscreenState. 
		//
		if(m_bNonPrerotated)
		{
			//
			// Get a descriptor for the output to check if test is still in the mode requested.
			//
			DXGI_OUTPUT_DESC outputDesc;
			hresult = pTargetOutput->GetDesc( &outputDesc );

			if ( FAILED( hresult ) )
			{
				WriteToLog
				(
					"GetDesc failed with error 0x%x.",
					hresult
				);

				goto Cleanup;
			};


			UINT actualWidth = outputDesc.DesktopCoordinates.right - outputDesc.DesktopCoordinates.left;
			UINT actualHeight = outputDesc.DesktopCoordinates.bottom - outputDesc.DesktopCoordinates.top;
			UINT expectedWidth = m_SCDesc.BufferDesc.Width;
			UINT expectedHeight = m_SCDesc.BufferDesc.Height;

			if
			(
				actualWidth != m_SCDesc.BufferDesc.Width ||
				actualHeight != m_SCDesc.BufferDesc.Height ||
				m_RotationMode != outputDesc.Rotation 
			)
			{	   

				WriteToLog("Mismatched Mode After SetFullscreenState:");
				WriteToLog
				(
					"Actual Mode: %d x %d x Rotation = %d", 
					actualWidth,
					actualHeight,
					outputDesc.Rotation
				); 

				WriteToLog
				(
					"Expected Mode: %d x %d x Rotation = %d", 
					expectedWidth,
					expectedHeight,
					m_RotationMode
				); 

				goto Cleanup;
			} 
		}
	};


	//
	// Initialize the Direct3D device resources needed
	// for texture rendering.
	//

	bool bCreateResourcesResult = false;
	switch ( m_TestFeatureLevel )
	{
		case TEST_FEATURE_11_0_LEVEL_9_1:
		case TEST_FEATURE_11_0_LEVEL_9_2:
		case TEST_FEATURE_11_0_LEVEL_9_3:
		case TEST_FEATURE_10_1_LEVEL_9_1:
		case TEST_FEATURE_10_1_LEVEL_9_2:
		case TEST_FEATURE_10_1_LEVEL_9_3:
		{
			bCreateResourcesResult = PrimaryAdapterDevice.CreateTexRenderResources
			(
				"vs_4_0_level_9_1",
				"ps_4_0_level_9_1"
			);
		} break;

		default:
		{
			bCreateResourcesResult = PrimaryAdapterDevice.CreateTexRenderResources
			(
				"vs_4_0",
				"ps_4_0"
			);
		} break;
	};


	if ( bCreateResourcesResult != true )
	{
		WriteToLog
		(
			"CreateTexRenderResources failed."
		);

		if( m_bLargeMemoryUsage && PrimaryAdapterDevice.GetLastError() == E_OUTOFMEMORY )
		{
			test_result = RESULT_SKIP;
			WriteToLog( "ExecuteTestCase: Test case using large amount of memory. Resource creation failure expected. Skipping test case. " );
		}

		goto Cleanup;
	};


	//
	// Keep a reference to the target output.
	//

	pDeviceWindowOutput = pTargetOutput;
	pTargetOutput = NULL;


	//
	// If we made it this far, then we're good to go.
	//

	test_result = RESULT_PASS;


Cleanup:
	if
	(
		m_AdapterPointer.pAdapter != NULL &&
		test_result != RESULT_PASS
	)
	{
		m_AdapterPointer.pAdapter->Release();
		m_AdapterPointer.pAdapter = NULL;
	};

#pragma prefast( suppress:  28922 )
	if ( pTargetOutput != NULL )
	{
		pTargetOutput->Release();
		pTargetOutput = NULL;
	};


	return test_result;
};


HRESULT CPresentFS::ResizeBuffers
(
	IDXGISwapChain * pSwapChain,
	DXGI_SWAP_CHAIN_DESC * pSwapChainDesc
)
{
	return S_OK;
};


void CPresentFS::CleanupTestCase()
{
	//
	// Release the adapter.
	//

	if ( m_AdapterPointer.pAdapter != NULL )
	{
		m_AdapterPointer.pAdapter->Release();
		m_AdapterPointer.pAdapter = NULL;
	};


	//
	// Break focus window association and release factory.
	//

	if ( m_FactoryPointer.pFactory != NULL )
	{
		HWND hwndCurrentFocus = NULL;

		m_FactoryPointer.pFactory->GetWindowAssociation( &hwndCurrentFocus );
		if ( hwndCurrentFocus != NULL )
		{
			m_FactoryPointer.pFactory->MakeWindowAssociation
			(
				NULL,
				0
			);
		};

		m_FactoryPointer.pFactory->Release();
		m_FactoryPointer.pFactory = NULL;
	};

};


TEST_RESULT CPresentFS::ExecuteTestCase()
{
	DebugOut.Write("Entering ExecuteTestCase.\n");
	TEST_RESULT test_result = RESULT_FAIL;
	TEST_RESULT CreateDeviceAndSwapChainResult = RESULT_FAIL;
	HRESULT hr = E_FAIL;
	BOOL found = FALSE;
	UINT texturesToLoad = m_TexturesToLoad;

	if(m_ParameterError == -1)
	{
		WriteToLog
		(
			"CPresentFS::ExecuteTestCase m_ParameterError = %d. No supported modes or formats were found",
			m_ParameterError
		); 

		test_result = RESULT_SKIP;
		goto Cleanup;

	}

	if(m_ModeInformation.isInitialized != TRUE )
	{
		WriteToLog
		(
			"CPresentFS::ExecuteTestCase Could not find Adapter-Output format parameter."
		);		

		test_result = RESULT_SKIP;
		goto Cleanup;        
	}


	//
	// Validate Flip Model (DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) compatible parameters
	//

	if (IsFlipModel(m_SwapEffect.dxgiSwapEffect))
	{
		if (m_BufferCount < 2)
		{
			WriteToLog("*** Flip model swapchains requires at least 2 buffers.  Skipping test case. ***");
			test_result = RESULT_SKIP;
			goto Cleanup;
		}

		switch (m_TestFeatureLevel)
		{
			// All levels except feature level 9 should
			// be in this list.  Please add them as they
			// are created.
			case TEST_FEATURE_10_0_LEVEL_10_0:
			case TEST_FEATURE_10_1_LEVEL_10_0:
			case TEST_FEATURE_10_1_LEVEL_10_1:
			case TEST_FEATURE_11_0_LEVEL_10_0:
			case TEST_FEATURE_11_0_LEVEL_10_1:
			case TEST_FEATURE_11_0_LEVEL_11_0:

			//uncomment these when DX11.1 support is added to DXGIFramework helpers.
			//            case TEST_FEATURE_11_1_LEVEL_10_0:  
			//            case TEST_FEATURE_11_1_LEVEL_10_1:
			//            case TEST_FEATURE_11_1_LEVEL_11_0:
			//            case TEST_FEATURE_11_1_LEVEL_11_1:
			// Validate back buffer formats compatible with FLIP_SEQUENTIAL on Feature level 10 or greater.
			switch (m_ModeInformation.ModeDesc.Format)
			{
				case DXGI_FORMAT_R16G16B16A16_FLOAT:
				case DXGI_FORMAT_B8G8R8X8_UNORM:
				case DXGI_FORMAT_B8G8R8A8_UNORM:
				case DXGI_FORMAT_R8G8B8A8_UNORM:
				break;

				default:
					WriteToLog("*** Flip model swapchains not supported with this pixel format.  Skipping test case. ***");                            
					test_result = RESULT_SKIP;
					goto Cleanup;
			} break; // End of DX10 or higher switch case bl