#include "ResourceFormats.h"
#include <map>

#define EXTEND_64BIT(x) ((UINT64)(x) << 32)
#define D3D11_FORMAT_SUPPORT_CAPTURE 0x80000000    // Internal, see generate_d3d11_state.idl

TEST_RESULT
CCheckFormatSupportTest::Setup()
{
    HRESULT hr = S_OK;
    m_pAdapterDescriptor = DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor( g_TestApp.GetDevice() );
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

UINT64 getRequiredSupport(DXGI_FORMAT fmt, D3DKMT_DRIVERVERSION WDDMVersion)
{
    const bool b10_0
        = g_TestApp.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0;
    const bool b10_1 
        = g_TestApp.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_1;
    const bool b11_0 
        = g_TestApp.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_11_0;

    const bool b10_0OrHigher
        = g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_0;
    const bool b10_1OrHigher
        = g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1;
    const bool b11_0OrHigher
        = g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0;
    const bool b11_1OrHigher
        = g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_1;

    const UINT64 basicTypelessSupport
        = D3D11_FORMAT_SUPPORT_TEXTURE1D
        | D3D11_FORMAT_SUPPORT_TEXTURE2D
        | D3D11_FORMAT_SUPPORT_TEXTURE3D
        | D3D11_FORMAT_SUPPORT_TEXTURECUBE
        | D3D11_FORMAT_SUPPORT_MIP
        | D3D11_FORMAT_SUPPORT_CPU_LOCKABLE
        | D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT
        ;
    const UINT64 basicFloatSupport
        = D3D11_FORMAT_SUPPORT_BUFFER 
        | D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER
        | D3D11_FORMAT_SUPPORT_TEXTURE1D
        | D3D11_FORMAT_SUPPORT_TEXTURE2D
        | D3D11_FORMAT_SUPPORT_TEXTURE3D
        | D3D11_FORMAT_SUPPORT_TEXTURECUBE
        | D3D11_FORMAT_SUPPORT_SHADER_LOAD
        | D3D11_FORMAT_SUPPORT_MIP
        | D3D11_FORMAT_SUPPORT_MIP_AUTOGEN
        | D3D11_FORMAT_SUPPORT_RENDER_TARGET
        | D3D11_FORMAT_SUPPORT_BLENDABLE
        | D3D11_FORMAT_SUPPORT_CPU_LOCKABLE
        | D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE
        | D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT
        | ((b10_1OrHigher) ? (D3D11_FORMAT_SUPPORT_BLENDABLE | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE | D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD) : 0)
        | ((b11_0OrHigher) ? (D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW | D3D11_FORMAT_SUPPORT_SHADER_GATHER) : 0)
        | ((b11_0OrHigher) ? EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_STORE) : 0)
        ;
    const UINT64 basicIntSupport
        = D3D11_FORMAT_SUPPORT_BUFFER 
        | D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER
        | D3D11_FORMAT_SUPPORT_TEXTURE1D
        | D3D11_FORMAT_SUPPORT_TEXTURE2D
        | D3D11_FORMAT_SUPPORT_TEXTURE3D
        | D3D11_FORMAT_SUPPORT_TEXTURECUBE
        | D3D11_FORMAT_SUPPORT_SHADER_LOAD
        | D3D11_FORMAT_SUPPORT_MIP
        | D3D11_FORMAT_SUPPORT_RENDER_TARGET
        | D3D11_FORMAT_SUPPORT_CPU_LOCKABLE
        | D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT
        | ((b10_1OrHigher) ? (D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD) : 0)
        | ((b11_0OrHigher) ? D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW : 0)
        | ((b11_0OrHigher) ? EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_STORE) : 0)
        ;
    const UINT64 basicUIntSupport
        = basicIntSupport 
        | ((b11_1OrHigher) ? EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_OUTPUT_MERGER_LOGIC_OP) : 0)
        ;
    const UINT64 basicSNormSupport
        = D3D11_FORMAT_SUPPORT_BUFFER 
        | D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER
        | D3D11_FORMAT_SUPPORT_TEXTURE1D
        | D3D11_FORMAT_SUPPORT_TEXTURE2D
        | D3D11_FORMAT_SUPPORT_TEXTURE3D
        | D3D11_FORMAT_SUPPORT_TEXTURECUBE
        | D3D11_FORMAT_SUPPORT_SHADER_LOAD
        | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE
        | D3D11_FORMAT_SUPPORT_MIP
        | D3D11_FORMAT_SUPPORT_MIP_AUTOGEN
        | D3D11_FORMAT_SUPPORT_RENDER_TARGET
        | D3D11_FORMAT_SUPPORT_CPU_LOCKABLE
        | D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE
        | D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT
        | ((b10_1OrHigher) ? (D3D11_FORMAT_SUPPORT_BLENDABLE | D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD) : 0)
        | ((b11_0OrHigher) ? (D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW | D3D11_FORMAT_SUPPORT_SHADER_GATHER) : 0)
        | ((b11_0OrHigher) ? EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_STORE) : 0)
        ;
    const UINT64 basicUNormSupport
        = basicSNormSupport
        | D3D11_FORMAT_SUPPORT_BLENDABLE
        ;
    const UINT64 basic16BitUNormSupport
        = D3D11_FORMAT_SUPPORT_TEXTURE1D
        | D3D11_FORMAT_SUPPORT_TEXTURE2D
        | D3D11_FORMAT_SUPPORT_TEXTURE3D
        | D3D11_FORMAT_SUPPORT_TEXTURECUBE
        | D3D11_FORMAT_SUPPORT_SHADER_LOAD
        | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE
        | ((b11_0OrHigher) ? D3D11_FORMAT_SUPPORT_SHADER_GATHER : 0)
        | D3D11_FORMAT_SUPPORT_MIP
        | D3D11_FORMAT_SUPPORT_CPU_LOCKABLE
        ;
    const UINT64 basicMixedRGXSupport
        = D3D11_FORMAT_SUPPORT_TEXTURE1D
        | D3D11_FORMAT_SUPPORT_TEXTURE2D
        | D3D11_FORMAT_SUPPORT_TEXTURECUBE
        | D3D11_FORMAT_SUPPORT_MIP
        | D3D11_FORMAT_SUPPORT_CPU_LOCKABLE
        | D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT
        | ((b10_1OrHigher) ? (D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD) : 0)
        ;
    const UINT64 basicDepthSupport
        = (basicMixedRGXSupport & ~(D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD))
        | D3D11_FORMAT_SUPPORT_DEPTH_STENCIL
        ;
    const UINT64 basicBCSupport
        = D3D11_FORMAT_SUPPORT_TEXTURE2D
        | D3D11_FORMAT_SUPPORT_TEXTURE3D
        | D3D11_FORMAT_SUPPORT_TEXTURECUBE
        | D3D11_FORMAT_SUPPORT_SHADER_LOAD
        | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE
        | D3D11_FORMAT_SUPPORT_MIP
        | D3D11_FORMAT_SUPPORT_CPU_LOCKABLE
        | D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT
        // expected but not present in spec
        //| (b11_0OrHigher) ? D3D11_FORMAT_SUPPORT_SHADER_GATHER : 0)
        ;
    const UINT64 basicMixedChannelSupport
        = D3D11_FORMAT_SUPPORT_TEXTURE1D
        | D3D11_FORMAT_SUPPORT_TEXTURE2D
        | D3D11_FORMAT_SUPPORT_TEXTURE3D
        | D3D11_FORMAT_SUPPORT_TEXTURECUBE
        | D3D11_FORMAT_SUPPORT_SHADER_LOAD
        | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE
        | D3D11_FORMAT_SUPPORT_MIP
        | D3D11_FORMAT_SUPPORT_CPU_LOCKABLE
        ;

    const UINT64 float32support
        = basicFloatSupport
        | D3D11_FORMAT_SUPPORT_SO_BUFFER;
    const UINT64 int32support
        = basicIntSupport
        | D3D11_FORMAT_SUPPORT_SO_BUFFER;
    const UINT64 uint32support
        = int32support
        | ((b11_1OrHigher) ? EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_OUTPUT_MERGER_LOGIC_OP) : 0);
    const UINT64 float16support
        = basicFloatSupport
        | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE;
    const UINT64 int16support
        = basicIntSupport;
    const UINT64 uint16support
        = int16support
        | ((b11_1OrHigher) ? EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_OUTPUT_MERGER_LOGIC_OP) : 0);
    const UINT64 basicVideoSupport
        = D3D11_FORMAT_SUPPORT_TEXTURE2D
        | D3D11_FORMAT_SUPPORT_SHADER_LOAD
        | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE
        | D3D11_FORMAT_SUPPORT_CPU_LOCKABLE
        | ((b11_0OrHigher) ? D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW : 0)
        | ((b11_0OrHigher) ? EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_STORE) : 0)
        | ((b10_1OrHigher) ? D3D11_FORMAT_SUPPORT_SHADER_GATHER : 0);

    UINT64 retVal = 0;
    switch(fmt)
    {
    default:
        assert(!"Unrecognized format");
        retVal = 0;
        break;
    case DXGI_FORMAT_UNKNOWN:
        retVal = D3D11_FORMAT_SUPPORT_BUFFER;
        retVal |= D3D11_FORMAT_SUPPORT_CPU_LOCKABLE;
        break;

    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        retVal = basicTypelessSupport;
        break;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        retVal = float32support;
        break;
    case DXGI_FORMAT_R32G32B32A32_UINT:
        retVal = uint32support;
        break;
    case DXGI_FORMAT_R32G32B32A32_SINT:
        retVal = int32support;
        break;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
        retVal = basicTypelessSupport;
        break;
    case DXGI_FORMAT_R32G32B32_FLOAT:
        retVal = float32support;
        retVal &= ~(D3D11_FORMAT_SUPPORT_SHADER_SAMPLE | D3D11_FORMAT_SUPPORT_BLENDABLE | D3D11_FORMAT_SUPPORT_RENDER_TARGET | D3D11_FORMAT_SUPPORT_MIP_AUTOGEN | D3D11_FORMAT_SUPPORT_SHADER_GATHER | D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW );
        retVal &= ~(EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_STORE));
        if (b10_1OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_BLENDABLE;
        break;
    case DXGI_FORMAT_R32G32B32_UINT:
        retVal = uint32support;
        retVal &= ~(D3D11_FORMAT_SUPPORT_RENDER_TARGET | D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW);
        retVal &= ~(EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_STORE));
        break;
    case DXGI_FORMAT_R32G32B32_SINT:
        retVal = int32support;
        retVal &= ~(D3D11_FORMAT_SUPPORT_RENDER_TARGET | D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW);
        retVal &= ~(EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_STORE));
        break;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        retVal = basicTypelessSupport;
        break;
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        retVal = float16support;
        retVal |= D3D11_FORMAT_SUPPORT_DISPLAY;
        if( b10_0OrHigher )
        {
            retVal |= D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT;
        }
        break;
    case DXGI_FORMAT_R16G16B16A16_UNORM:
        retVal = basicUNormSupport;
        if (b10_0)
            retVal &= (~(D3D11_FORMAT_SUPPORT_BLENDABLE));
        break;
    case DXGI_FORMAT_R16G16B16A16_UINT:
        retVal = uint16support;
        break;
    case DXGI_FORMAT_R16G16B16A16_SNORM:
        retVal = basicSNormSupport;
        break;
    case DXGI_FORMAT_R16G16B16A16_SINT:
        retVal = int16support;
        break;

    case DXGI_FORMAT_R32G32_TYPELESS:
        retVal = basicTypelessSupport;
        break;
    case DXGI_FORMAT_R32G32_FLOAT:
        retVal = float32support;
        break;
    case DXGI_FORMAT_R32G32_UINT:
        retVal = uint32support;
        break;
    case DXGI_FORMAT_R32G32_SINT:
        retVal = int32support;
        break;

    case DXGI_FORMAT_R32G8X24_TYPELESS:
        retVal = basicMixedRGXSupport;
        retVal &= ~(D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD);
        break;
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        retVal = basicDepthSupport;
        break;
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        retVal = basicMixedRGXSupport;
        retVal |= D3D11_FORMAT_SUPPORT_SHADER_LOAD;
        retVal |= D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON;
        if (b10_1OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_SAMPLE;
        if (b10_1OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER;
        if (b11_0OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER_COMPARISON;
        break;
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        retVal = basicMixedRGXSupport;
        retVal |= D3D11_FORMAT_SUPPORT_SHADER_LOAD;
        break;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        retVal = basicTypelessSupport;
        break;
    case DXGI_FORMAT_R10G10B10A2_UNORM:
        retVal = basicUNormSupport;
        retVal |= D3D11_FORMAT_SUPPORT_DISPLAY;
        if (b10_0OrHigher)
        {
            retVal |= D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT;
        }
        if (b11_0OrHigher)
        {
            retVal |= D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST;
        }
        break;
    case DXGI_FORMAT_R10G10B10A2_UINT:
        retVal = basicUIntSupport;
        break;
    case DXGI_FORMAT_R11G11B10_FLOAT:
        retVal = basicFloatSupport;
        retVal &= (~(D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT));
        retVal |= D3D11_FORMAT_SUPPORT_SHADER_SAMPLE;
        break;

    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        retVal = basicTypelessSupport;
        break;
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        retVal = basicUNormSupport;
        retVal |= D3D11_FORMAT_SUPPORT_DISPLAY;
        if (b10_0OrHigher)
        {
            retVal |= D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT;
        }
        if (b11_0OrHigher)
        {
            retVal |= D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST;
        }
        break;
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        retVal = basicUNormSupport;
        retVal |= D3D11_FORMAT_SUPPORT_DISPLAY;
        retVal &= ~(D3D11_FORMAT_SUPPORT_BUFFER | D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER | D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW);
        retVal &= ~(EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_STORE));
        if (b10_0OrHigher)
        {
            retVal |= D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT;
        }
        if (b11_0OrHigher)
        {
            retVal |= D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST;
        }
        break;
    case DXGI_FORMAT_R8G8B8A8_UINT:
        retVal = basicUIntSupport;
        break;
    case DXGI_FORMAT_R8G8B8A8_SNORM:
        retVal = basicSNormSupport;
        break;
    case DXGI_FORMAT_R8G8B8A8_SINT:
        retVal = basicIntSupport;
        break;

    case DXGI_FORMAT_R16G16_TYPELESS:
        retVal = basicTypelessSupport;
        break;
    case DXGI_FORMAT_R16G16_FLOAT:
        retVal = float16support;
        break;
    case DXGI_FORMAT_R16G16_UNORM:
        retVal = basicUNormSupport;
        if (b10_0)
            retVal &= (~(D3D11_FORMAT_SUPPORT_BLENDABLE));
        break;
    case DXGI_FORMAT_R16G16_UINT:
        retVal = uint16support;
        break;
    case DXGI_FORMAT_R16G16_SNORM:
        retVal = basicSNormSupport;
        break;
    case DXGI_FORMAT_R16G16_SINT:
        retVal = int16support;
        break;

    case DXGI_FORMAT_R32_TYPELESS:
        retVal = basicTypelessSupport;
        break;
    case DXGI_FORMAT_D32_FLOAT:
        retVal = basicDepthSupport;
        break;
    case DXGI_FORMAT_R32_FLOAT:
        retVal = float32support;
        retVal |= D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON;
        if (b10_1OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER;
        if (b11_0OrHigher)
        {
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER_COMPARISON;
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_LOAD);
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_EXCHANGE);
        }
        break;
    case DXGI_FORMAT_R32_UINT:
        retVal = uint32support;
        retVal |= D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER;
        if (b11_0OrHigher)
        {
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_LOAD);
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_ADD);
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_BITWISE_OPS);
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_COMPARE_STORE_OR_COMPARE_EXCHANGE);
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_EXCHANGE);
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_UNSIGNED_MIN_OR_MAX);
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_SIGNED_MIN_OR_MAX);
        }
        break;
    case DXGI_FORMAT_R32_SINT:
        retVal = int32support;
        if (b11_0OrHigher)
        {
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_LOAD);
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_ADD);
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_BITWISE_OPS);
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_COMPARE_STORE_OR_COMPARE_EXCHANGE);
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_EXCHANGE);
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_SIGNED_MIN_OR_MAX);
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_ATOMIC_UNSIGNED_MIN_OR_MAX);
        }
        break;

    case DXGI_FORMAT_R24G8_TYPELESS:
        retVal = basicMixedRGXSupport;
        retVal &= ~(D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD);
        break;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        retVal = basicDepthSupport;
        break;
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        retVal = basicMixedRGXSupport;
        retVal |= D3D11_FORMAT_SUPPORT_SHADER_LOAD;
        retVal |= D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON;
        if (b10_1OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_SAMPLE;
        if (b10_1OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER;
        if (b11_0OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER_COMPARISON;
        break;
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        retVal = basicMixedRGXSupport;
        retVal |= D3D11_FORMAT_SUPPORT_SHADER_LOAD;
        break;

    case DXGI_FORMAT_R8G8_TYPELESS:
        retVal = basicTypelessSupport;
        break;
    case DXGI_FORMAT_R8G8_UNORM:
        retVal = basicUNormSupport;
        break;
    case DXGI_FORMAT_R8G8_UINT:
        retVal = basicUIntSupport;
        break;
    case DXGI_FORMAT_R8G8_SNORM:
        retVal = basicSNormSupport;
        break;
    case DXGI_FORMAT_R8G8_SINT:
        retVal = basicIntSupport;
        break;

    case DXGI_FORMAT_R16_TYPELESS:
        retVal = basicTypelessSupport;
        break;
    case DXGI_FORMAT_R16_FLOAT:
        retVal = float16support;
        if (b10_1OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER;
        break;
    case DXGI_FORMAT_D16_UNORM:
        retVal = basicDepthSupport;
        break;
    case DXGI_FORMAT_R16_UNORM:
        retVal = basicUNormSupport;
        retVal |= D3D11_FORMAT_SUPPORT_SHADER_SAMPLE_COMPARISON;
        if (b10_0)
            retVal &= (~(D3D11_FORMAT_SUPPORT_BLENDABLE));
        if (b10_1OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER;
        if (b11_0OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER_COMPARISON;
        break;
    case DXGI_FORMAT_R16_UINT:
        retVal = uint16support;
        retVal |= D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER;
        break;
    case DXGI_FORMAT_R16_SNORM:
        retVal = basicSNormSupport;
        if (b10_1OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER;
        break;
    case DXGI_FORMAT_R16_SINT:
        retVal = int16support;
        break;

    case DXGI_FORMAT_R8_TYPELESS:
        retVal = basicTypelessSupport;
        break;
    case DXGI_FORMAT_R8_UNORM:
        retVal = basicUNormSupport;
        if (b10_1OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER;
        break;
    case DXGI_FORMAT_R8_UINT:
        retVal = basicUIntSupport;
        break;
    case DXGI_FORMAT_R8_SNORM:
        retVal = basicSNormSupport;
        if (b10_1OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER;
        break;
    case DXGI_FORMAT_R8_SINT:
        retVal = basicIntSupport;
        break;

    case DXGI_FORMAT_A8_UNORM:
        retVal = basicUNormSupport;
        retVal &= (~(D3D11_FORMAT_SUPPORT_BUFFER | D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER | D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT));
        break;

    case DXGI_FORMAT_R1_UNORM:
        retVal = 0;
        break;

    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        retVal = basicMixedChannelSupport;
        if (b11_0OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER;
        break;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC7_TYPELESS:
        retVal = basicBCSupport;
        retVal &= (~(D3D11_FORMAT_SUPPORT_SHADER_LOAD | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE | D3D11_FORMAT_SUPPORT_SHADER_GATHER ));
        break;

    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        retVal = basicBCSupport;
        if (b11_0OrHigher)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER;
        break;

    case DXGI_FORMAT_B4G4R4A4_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
        retVal = basic16BitUNormSupport;
        break;

    case DXGI_FORMAT_B5G6R5_UNORM:
        retVal = basic16BitUNormSupport
            | D3D11_FORMAT_SUPPORT_MIP_AUTOGEN
            | D3D11_FORMAT_SUPPORT_RENDER_TARGET
            | D3D11_FORMAT_SUPPORT_BLENDABLE
            | ((b10_1OrHigher) ? D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET : 0)    // User needs to check CheckMultisampleQualityLevels() for 4x, 8x

            // Note these are really only required if FL10.1 or higher, but we will remove this bit in "checkRequiredSupport" on FL10.0 if MSAA is not supported
            | ((b10_0OrHigher) ? D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE : 0) 
            | ((b10_0OrHigher) ? D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD : 0);

        break;

    case DXGI_FORMAT_B8G8R8X8_TYPELESS:         
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:         
        if( b11_0OrHigher )
            retVal = basicTypelessSupport;
        break;
    
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        if( b11_0OrHigher )
        {
            retVal |= D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST | D3D11_FORMAT_SUPPORT_DISPLAY | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT;
        }
        //fall through
    case DXGI_FORMAT_B8G8R8X8_UNORM:
        if( b11_0OrHigher )
        {
            retVal |= basicUNormSupport;
            retVal &= ~(D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD | D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW);
            retVal &= ~(EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_STORE));
        }
        else if ( b10_0 || b10_1 )
        {

        }
        break;

    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:   
        if( b11_0OrHigher )
        {
            retVal |= D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST | D3D11_FORMAT_SUPPORT_DISPLAY | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT;
        }
        //fall through
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        if( b11_0OrHigher )
        {
            retVal |= basicUNormSupport;
            retVal &= ~(D3D11_FORMAT_SUPPORT_BUFFER | D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER | D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD | D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW);
            retVal &= ~(EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_UAV_TYPED_STORE));
        }
        break;
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        if ( b11_0OrHigher )
        {
            retVal = (D3D11_FORMAT_SUPPORT_TEXTURE2D | 
                D3D11_FORMAT_SUPPORT_CAST_WITHIN_BIT_LAYOUT | 
                D3D11_FORMAT_SUPPORT_DISPLAY |
                D3D11_FORMAT_SUPPORT_CPU_LOCKABLE |
                D3D11_FORMAT_SUPPORT_BACK_BUFFER_CAST |
                D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT);
        }
        break;

    // Video formats
    case DXGI_FORMAT_AYUV:
        if( b10_0OrHigher )
        {
            retVal = basicVideoSupport
            | D3D11_FORMAT_SUPPORT_MIP
            | D3D11_FORMAT_SUPPORT_MIP_AUTOGEN
            | D3D11_FORMAT_SUPPORT_RENDER_TARGET
            | D3D11_FORMAT_SUPPORT_BLENDABLE
            | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT;
        }
        break;

    case DXGI_FORMAT_NV12:
        if( b10_0OrHigher )
        {
            retVal = basicVideoSupport
            | D3D11_FORMAT_SUPPORT_RENDER_TARGET
            | D3D11_FORMAT_SUPPORT_BLENDABLE
            | D3D11_FORMAT_SUPPORT_DECODER_OUTPUT
            | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT
            | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT;
        }
        break;

    case DXGI_FORMAT_YUY2:
        if( b10_0OrHigher )
        {
            retVal = basicVideoSupport
            | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT;
        }
        break;

    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_P8:
    case DXGI_FORMAT_A8P8:
        if( b10_0OrHigher )
        {
            retVal = D3D11_FORMAT_SUPPORT_TEXTURE2D
            | D3D11_FORMAT_SUPPORT_CPU_LOCKABLE
            | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT;
        }
        break;

    case DXGI_FORMAT_420_OPAQUE:
        if( b10_0OrHigher )
        {
            retVal = D3D11_FORMAT_SUPPORT_TEXTURE2D
            | D3D11_FORMAT_SUPPORT_DECODER_OUTPUT
            | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_OUTPUT
            | D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT;
        }
        break;

    case DXGI_FORMAT_Y416:
        if( b10_0OrHigher )
        {
            retVal = basicVideoSupport
            | D3D11_FORMAT_SUPPORT_MIP;
        }
        break;

    case DXGI_FORMAT_Y410:
    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
        if( b10_0OrHigher )
        {
            retVal = basicVideoSupport;
        }
        break;

    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
        if( b10_1OrHigher )
        {
            retVal = basicVideoSupport
            | D3D11_FORMAT_SUPPORT_RENDER_TARGET
            | D3D11_FORMAT_SUPPORT_BLENDABLE;
        }
        else if( b10_0 )
        {
            retVal = basicVideoSupport
            | D3D11_FORMAT_SUPPORT_RENDER_TARGET;
        }
        break;
    case DXGI_FORMAT_NV11:
        if( b10_0OrHigher )
        {
            retVal = basicVideoSupport
            | D3D11_FORMAT_SUPPORT_RENDER_TARGET
            | D3D11_FORMAT_SUPPORT_BLENDABLE;
        }
        break;
    }

    if( WDDMVersion >= KMT_DRIVERVERSION_WDDM_1_1 )
    {
        switch( fmt )
        {
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
            retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_SHAREABLE);
        }

        if( WDDMVersion >= KMT_DRIVERVERSION_WDDM_1_2 )
        {
            switch( fmt )
            {
            case DXGI_FORMAT_420_OPAQUE:
            case DXGI_FORMAT_A8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_TYPELESS:
            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            case DXGI_FORMAT_B8G8R8X8_TYPELESS:
            case DXGI_FORMAT_B8G8R8X8_UNORM:
            case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            case DXGI_FORMAT_D16_UNORM:
            case DXGI_FORMAT_D32_FLOAT:
            case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            case DXGI_FORMAT_R10G10B10A2_UINT:
            case DXGI_FORMAT_R10G10B10A2_UNORM:
            case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
            case DXGI_FORMAT_R16G16B16A16_FLOAT:
            case DXGI_FORMAT_R16G16B16A16_SINT:
            case DXGI_FORMAT_R16G16B16A16_SNORM:
            case DXGI_FORMAT_R16G16B16A16_TYPELESS:
            case DXGI_FORMAT_R16G16B16A16_UINT:
            case DXGI_FORMAT_R16G16B16A16_UNORM:
            case DXGI_FORMAT_R16_FLOAT:
            case DXGI_FORMAT_R16_SINT:
            case DXGI_FORMAT_R16_SNORM:
            case DXGI_FORMAT_R16_TYPELESS:
            case DXGI_FORMAT_R16_UINT:
            case DXGI_FORMAT_R16_UNORM:
            case DXGI_FORMAT_R32G32B32A32_FLOAT:
            case DXGI_FORMAT_R32G32B32A32_SINT:
            case DXGI_FORMAT_R32G32B32A32_TYPELESS:
            case DXGI_FORMAT_R32G32B32A32_UINT:
            case DXGI_FORMAT_R32_FLOAT:
            case DXGI_FORMAT_R32_SINT:
            case DXGI_FORMAT_R32_TYPELESS:
            case DXGI_FORMAT_R32_UINT:
            case DXGI_FORMAT_R8G8B8A8_SINT:
            case DXGI_FORMAT_R8G8B8A8_SNORM:
            case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            case DXGI_FORMAT_R8G8B8A8_UINT:
            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            case DXGI_FORMAT_R8_SINT:
            case DXGI_FORMAT_R8_SNORM:
            case DXGI_FORMAT_R8_TYPELESS:
            case DXGI_FORMAT_R8_UINT:
            case DXGI_FORMAT_R8_UNORM:
            case DXGI_FORMAT_AYUV:
            case DXGI_FORMAT_YUY2:
            case DXGI_FORMAT_NV11:
            case DXGI_FORMAT_NV12:
            case DXGI_FORMAT_P010:
            case DXGI_FORMAT_P016:
            case DXGI_FORMAT_Y216:
            case DXGI_FORMAT_Y210:
            case DXGI_FORMAT_Y416:
            case DXGI_FORMAT_Y410:
                retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_SHAREABLE);

            }
        }
    
        if( WDDMVersion >= KMT_DRIVERVERSION_WDDM_1_3 )
        {
            switch( fmt )
            {
            case DXGI_FORMAT_BC1_TYPELESS:
            case DXGI_FORMAT_BC1_UNORM:
            case DXGI_FORMAT_BC1_UNORM_SRGB:
            case DXGI_FORMAT_BC2_TYPELESS:
            case DXGI_FORMAT_BC2_UNORM:
            case DXGI_FORMAT_BC2_UNORM_SRGB:
            case DXGI_FORMAT_BC3_TYPELESS:
            case DXGI_FORMAT_BC3_UNORM:
            case DXGI_FORMAT_BC3_UNORM_SRGB:
            case DXGI_FORMAT_R8G8_UNORM:
                retVal |= EXTEND_64BIT(D3D11_FORMAT_SUPPORT2_SHAREABLE);
            }
        }
    }

    return retVal;
}

UINT64
getOptionalSupport(DXGI_FORMAT fmt, D3DKMT_DRIVERVERSION WDDMVersion)
{
    const bool b10_0 
        = g_TestApp.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0;
    const bool b10_1 
        = g_TestApp.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_1;
    const bool b11_0 
        = g_TestApp.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_11_0;
    const bool b11_1
        = g_TestApp.m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_11_1;

    const bool b11_0OrLess 
        = g_TestApp.m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_11_0;

    const bool b11_1OrLess 
        = g_TestApp.m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_11_1;

    UINT64 retVal = 0;
    if (fmt >= DXGI_FORMAT_R32G32B32A32_TYPELESS && fmt <= DXGI_FORMAT_A8_UNORM)
    {
        retVal |= D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET;
        if (b10_0)
            retVal |= D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD;
    }
    switch (fmt)
    {
    default:
        break;
    // intentional fall-through on these, R32G32B32 is a special case
    case DXGI_FORMAT_R32G32B32_FLOAT:
        retVal |= D3D11_FORMAT_SUPPORT_MIP_AUTOGEN;
        if (b10_0)
            retVal |= D3D11_FORMAT_SUPPORT_BLENDABLE;
        if (b10_1 || b11_0 || b11_1 )
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_SAMPLE;
        if (b11_0 || b11_1)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_GATHER;
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        retVal |= D3D11_FORMAT_SUPPORT_RENDER_TARGET;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
        if (b10_0)
            retVal |= D3D11_FORMAT_SUPPORT_SHADER_SAMPLE;
        break;
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        retVal |= D3D11_FORMAT_SUPPORT_VIDEO_PROCESSOR_INPUT;
        break;
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
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

