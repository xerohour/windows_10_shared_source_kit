//////////////////////////////////////////////////////////////////////////
//  D3D11Test.cpp
//  created:	2004/10/13
//
//  purpose: Implementation of CD3D11Test class
//////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "d3d11test.h"
#include "FormatHelper.h"
#include <random>

typedef HRESULT (WINAPI PA_D3DDisassemble)
    (LPCVOID pSrcData,
     SIZE_T SrcDataSize,
     UINT Flags,
     LPCSTR szComments,
     ID3D10Blob** ppDisassembly);
typedef HRESULT (WINAPI PA_D3DReflect)
        (	LPCVOID pSrcData,
            SIZE_T SrcDataSize,
            REFIID pInterface,
            void** ppReflector);

typedef HRESULT (WINAPI PA_D3DGetSignatureBlob)
    (LPCVOID pSrcData,
     SIZE_T SrcDataSize,
     ID3DBlob** ppSignatureBlob);

class CD3D11TestDeviceManager : public CD3D11DeviceManager
{
public:
    CD3D11TestDeviceManager(
    _In_ CD3D11TestFramework *pFramework,
    _In_ D3D_OPTIONS *pD3DOptions,
    _In_ D3D_RENDER_TARGET_OPTIONS *pRenderTargetOptions
    ) :
    CD3D11DeviceManager(pD3DOptions, pRenderTargetOptions),
    m_pFramework(pFramework)
    {
    }

private:
    CD3D11TestFramework *m_pFramework;

    virtual HRESULT SetupProxyDriver(_Out_ HMODULE *hProxyDriver)
    {
        return m_pFramework->SetupProxyDriver(hProxyDriver);
    }
};

//--------------------------------------------------------------------

CD3D11TestFramework::CD3D11TestFramework()
:
    CGraphicsTestFramework
    (
    ),
    m_pD3DDeviceManager(nullptr),
    m_bSDKTest(false),
    m_pOutputParamTiledResourceUsed(nullptr),
    m_pOutputParamMapDefaultResourceUsed(nullptr),
    m_bMapDefaultDisabled(false)

{
    ZeroMemory(&m_D3DOptions, sizeof(m_D3DOptions));
    ZeroMemory(&m_RenderTargetOptions, sizeof(m_RenderTargetOptions));

    // Create the device manager
    m_pD3DDeviceManager = new CD3D11TestDeviceManager(this, &m_D3DOptions, &m_RenderTargetOptions);
    m_pD3DDeviceManager->m_nQueryTimeout = GetDefaultQueryTimeout();
}


//--------------------------------------------------------------------

CD3D11TestFramework::~CD3D11TestFramework()
{
}


//--------------------------------------------------------------------

bool CD3D11TestFramework::Setup()
{
    bool bFullScreen = false;
    HRESULT hr = S_OK;

    if(!__super::Setup())
    {
        return false;
    }

    // Initialize the device manager
    hr = m_pD3DDeviceManager->Setup(m_pLogWrapper);

    if (FAILED(hr))
        return false;

    m_pD3DDeviceManager->SetSession0Mode(CWGFTestFramework::IsSession0());

    if (GetShell())
    {
        m_pD3DDeviceManager->SetIsMobile(IsMobile());
    }

    // If running in Mobile mode, have the FormatHelper use WARP instead of REF
    CFormatHelper::SetReferenceDeviceType(IsMobile());

    // Setup D3D (device may be recycled during setup of first test group)
    TEST_RESULT tr = SetupD3D();
    if (tr != RESULT_PASS)
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------

void CD3D11TestFramework::Cleanup()
{
    // Delete the device manager
    SAFE_DELETE(m_pD3DDeviceManager);

    // Cleanup parent
    __super::Cleanup();
}

//--------------------------------------------------------------------

void CD3D11TestFramework::CleanupTest()
{
    //if(!m_DXGLog.InTestCase())
    //{
    //	m_DXGLog.BeginTestCase(_T("WDDM Aperture Corruption Detected."));
    //}
    //						
    //m_DXGLog.TestCaseResult(WTT_TESTCASE_RESULT_FAIL);
    //m_DXGLog.EndTestCase();
    // BUGBUG - MARTY ADD APERTURE CODE HERE...
    CWGFTestFramework::CleanupTest();

    // BUGBUG - There are better ways to handle cleanup of format helper resources, possibly using RAII (Resource Acquisition Is Initialization).
    // In other words the format helper should cleanup after itself
    CFormatHelper::Cleanup();

    // Cleanup d3d interfaces
    m_tiledResourceHelper.Cleanup();

    // Cleanup D3D device manager
    m_pD3DDeviceManager->Cleanup();
}


//--------------------------------------------------------------------

TEST_RESULT CD3D11TestFramework::SetupTest()
{
    // Reinitialize render target
    SetRenderTargetProperties();

    // Potentially recycle the device
    TEST_RESULT tr = RefreshGraphics();

    if(tr != RESULT_PASS)
    {
        return tr;
    }

    return CGraphicsTestFramework::SetupTest();
}


//--------------------------------------------------------------------

TEST_RESULT CD3D11TestFramework::PreSetupTestCase()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;

    if(!m_viewResources)
    {
        // Destroy all resource viewers if m_viewResources is false (may have been changed by the user)
        m_resourceViewer.DestroyAll();
    }

    // Call Start() to shrink tile pool, if possible
    m_tiledResourceHelper.Start();

    if( !GetCurrentTest() )
    {
        return RESULT_NONE;
    }

    if(m_pD3DDeviceManager->RefreshNeeded())
    {
        CTest *pTest = GetCurrentTest();

        // Call the current test object's Cleanup()
        pTest->Cleanup();

        // Cleanup tiled resource helper
        m_tiledResourceHelper.Cleanup();

        // Recycle the graphics system and re-setup the test 
        m_pD3DDeviceManager->CleanupDXGI();

        tr = SetupTest(); 
        if (RESULT_PASS != tr)
        {
            return tr;
        }
    }

    return tr;
}


void CD3D11TestFramework::PostCleanupTestCase()
{
    // Cleanup any tiled resources no longer in use
    m_tiledResourceHelper.Finish();

    m_pD3DDeviceManager->Finish();
}

//--------------------------------------------------------------------
void CD3D11TestFramework::PreSetupTest() 
{
    // In the event that the test made changes to the D3D or RenderTarget options they need to be restored
    // BUGBUG: This is a practice that should be discouraged.  Tests developers should not assume that leaving
    // framework variables in a random state when exiting a test group is okay.  Unfortunately for now there 
    // are a large number of tests that rely on automatic restoration of these options.
    CopyMemory(&m_D3DOptionsCached, &m_D3DOptions, sizeof(D3D_OPTIONS));
    CopyMemory(&m_RenderTargetOptionsCached, &m_RenderTargetOptions, sizeof(D3D_RENDER_TARGET_OPTIONS));
}

//--------------------------------------------------------------------
void CD3D11TestFramework::PostCleanupTest() 
{
    if(RtlCompareMemory(&m_D3DOptions, &m_D3DOptionsCached, sizeof(D3D_OPTIONS)) < sizeof(D3D_OPTIONS))
    {
        WriteToLog(_T("Restoring m_D3DOptions: Test modified m_D3DOptions members and did not restore them. Support for this is at risk of being deprecated."));
        CopyMemory(&m_D3DOptions, &m_D3DOptionsCached, sizeof(D3D_OPTIONS));
    }

    if(RtlCompareMemory(&m_RenderTargetOptions, &m_RenderTargetOptionsCached, sizeof(D3D_RENDER_TARGET_OPTIONS)) < sizeof(D3D_RENDER_TARGET_OPTIONS))
    {
        WriteToLog(_T("Restoring m_RenderTargetOptions: Test modified m_RenderTargetOptions members and did not restore them. Support for this is at risk of being deprecated."));
        CopyMemory(&m_RenderTargetOptions, &m_RenderTargetOptionsCached, sizeof(D3D_RENDER_TARGET_OPTIONS));
    }
}


//--------------------------------------------------------------------
void CD3D11TestFramework::InitOptionVariables()
{
    CGraphicsTestFramework::InitOptionVariables();

    RegisterOptionVariable( _T( "SDK" ), _T( "Run in SDK mode. Includes additional test conditions for SDK validation." ), &m_bSDKTest, false, 0, 0);
    RegisterOptionVariable(_T("ViewResources"), _T("Brings up windows for resources with resource viewer"), &m_viewResources, false, 0, 0, _T("-viewresources"));

    int CategoryD3D = AddOptionCategory( "D3D" );
    RegisterOptionVariable( _T( "DisableMapDefault" ), _T( "Allow test to substitue CPU-accessible default textures in place of CPU-accessible staging textures or non-CPU accessible default textures." ), &m_bMapDefaultDisabled, false, 0, CategoryD3D, _T("-DisableMapDefault") );

    m_tiledResourceHelper.InitOptionVariables(GetOptionsPropertyManager());
    m_pD3DDeviceManager->InitOptionVariables(GetOptionsPropertyManager());
}

//--------------------------------------------------------------------
void CD3D11TestFramework::InitOutputParameters(_In_ class CD3D11Test *pTest)
{
    if(CTiledResourceHelper::TILED_RESOURCE_MODE_ALLOW == m_tiledResourceHelper.GetMode())
    {
        pTest->CreateOutputParameter<bool>(_T("TiledResourcesUsed"), &m_pOutputParamTiledResourceUsed);
    }

    if(!m_bMapDefaultDisabled)
    {
        pTest->CreateOutputParameter<bool>(_T("MapDefaultTextureUsed"), &m_pOutputParamMapDefaultResourceUsed);
    }
}

//--------------------------------------------------------------------
HRESULT CD3D11TestFramework::IsLegacyD3D10WarpDll(_In_ IDXGIAdapter* pAdapter, _In_ ID3D11Device* pDevice, _Out_ bool& IsLegacyWarp10Dll) const
{
    HRESULT hr = S_OK;

    // Use DriverConcurrentCreates to distinguish between a legacy implementation of warp and the newer version that will eventually replace it. 
    const bool IsWarpAdapter = IsWARPAdapter(pAdapter);

    IsLegacyWarp10Dll = false;
    if (IsWarpAdapter)
    {
        D3D11_FEATURE_DATA_THREADING featureDataThreading = {};
        if (SUCCEEDED(hr = pDevice->CheckFeatureSupport(D3D11_FEATURE_THREADING, &featureDataThreading, sizeof(featureDataThreading))))
        {
            // D3D10WARP.DLL does not support DriverConcurrentCreates.  D3D12WARP.DLL does.
            IsLegacyWarp10Dll = !featureDataThreading.DriverConcurrentCreates;
        }
    }

    return hr;
}

//--------------------------------------------------------------------
bool CD3D11TestFramework::FormatSupportsMap(DXGI_FORMAT format)
{
    if (CFormatHelper::ElementBits(format) == 96 || CFormatHelper::IsPlanar(format))
    {
        return false;
    }
       
    switch (format)
    {
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        case DXGI_FORMAT_AI44:               
        case DXGI_FORMAT_IA44:               
        case DXGI_FORMAT_P8:                 
        case DXGI_FORMAT_A8P8:  
            return false;
        default:
            return true;
    }
}

// Attempt to retrieve parameters for creating a CPU-accessible 
// default texture for use in place of a non-CPU-accessible default
// texture, or CPU-accessible staging texture.
HRESULT CD3D11TestFramework::TryGetMapDefaultParameters(
    bool fRequireLinearCpuAccess,
    UINT dimensionCount,
    DXGI_FORMAT format,
    UINT mipLevels,
    UINT arraySize,
    UINT bindFlags,
    DXGI_SAMPLE_DESC sampleDesc,
    __out bool *pfSupported,
    __out UINT *pCpuFlags,
    __out D3D11_TEXTURE_LAYOUT *pTextureLayout
    )
{    
    HRESULT hr = S_OK;
    
    *pfSupported = false;
    *pCpuFlags = 0;
    *pTextureLayout = D3D11_TEXTURE_LAYOUT_UNDEFINED;

    // We will switch between the following CPU access flags and texture layouts    
    const int rgCpuFlags[] = 
    {
        0,
        D3D11_CPU_ACCESS_READ,
        D3D11_CPU_ACCESS_WRITE,
        D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE
    };

    const D3D11_TEXTURE_LAYOUT rgLayouts[] = 
    {
        D3D11_TEXTURE_LAYOUT_UNDEFINED,
        D3D11_TEXTURE_LAYOUT_64K_STANDARD_SWIZZLE, 
        D3D11_TEXTURE_LAYOUT_ROW_MAJOR,         
    };

    // Create a psuedo-random number generator seeded with the test case index
    std::tr1::mt19937 generator;
    generator.seed(GetCurrentTestCase());

    // Define a distribution between 0 and ARRAYSIZE(rgCpuFlags) - 1 inclusive
    // and generate the random value
    std::tr1::uniform_int<DWORD> cpuUsageDist(0, ARRAYSIZE(rgCpuFlags) - 1);
    UINT cpuFlags = rgCpuFlags[cpuUsageDist(generator)];
    
    D3D11_TEXTURE_LAYOUT layout = D3D11_TEXTURE_LAYOUT_UNDEFINED;

    if (fRequireLinearCpuAccess)
    {
        // The caller needs to read the texture and to do so linearly
        if (cpuFlags == 0)
        {
            *pfSupported = false;
            return S_OK;
        }
        
        layout = (dimensionCount == 1) ? D3D11_TEXTURE_LAYOUT_64K_STANDARD_SWIZZLE : D3D11_TEXTURE_LAYOUT_ROW_MAJOR;
    }
    else
    {
        if (cpuFlags != 0)
        {
            UINT supportedLayoutCount = (dimensionCount == 1 || (bindFlags != 0)) ? 2 : 3;

            // Define a distribution between 0 and supportedLayoutCount inclusive
            // and generate the random value
            std::tr1::uniform_int<DWORD> layoutDist(0, supportedLayoutCount - 1);
            
            layout = rgLayouts[layoutDist(generator)];
        }
    }
    
    // Find whether the CPU access and layout type that is randomly selected is supported
    // by the GPU for this texture type.
    D3D11_FEATURE_DATA_D3D11_OPTIONS2 options2;
    if ( FAILED ( hr = GetDevice2()->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS2, (void*)&options2, sizeof(options2)) ) )
    {
        WriteToLog(_T("CheckFeatureSupport() failed"));
        return hr;
    }

    // Planar and 96 BPP formats are not supported by Map.
    bool fFormatSupported = FormatSupportsMap(format);

    // CPU-accessible mipmaps are not supported on default textures that are
    // 3D and not standard swizzle
    if (dimensionCount == 3 && layout != D3D11_TEXTURE_LAYOUT_64K_STANDARD_SWIZZLE && mipLevels > 1)
    {
        *pfSupported = false;
        return S_OK;
    }
    
    // Check against miscellaneous limitations where Map Default is supported
    if (!fFormatSupported ||
        (cpuFlags != 0 && !options2.MapOnDefaultTextures) ||
        (layout == D3D11_TEXTURE_LAYOUT_64K_STANDARD_SWIZZLE && !options2.StandardSwizzle) ||
        ((mipLevels > 1 || arraySize > 1) && layout == D3D11_TEXTURE_LAYOUT_ROW_MAJOR) ||
        (sampleDesc.Count != 1 || sampleDesc.Quality != 0) ||
        (bindFlags & D3D11_BIND_DEPTH_STENCIL) != 0)
    {
        *pfSupported = false;
        return S_OK;
    }

    *pfSupported = true;
    *pCpuFlags = cpuFlags;
    *pTextureLayout = layout;
    return S_OK;
}

HRESULT CD3D11TestFramework::CreateTexture2DTiledOptionalMapDefaultOptional(
    _In_ const D3D11_TEXTURE2D_DESC *pDesc,
    _In_opt_ const D3D11_SUBRESOURCE_DATA *pInitialData,
    _Outptr_ ID3D11Texture2D **ppTexture2D
    )
{    
    return CreateTexture2DHelper(pDesc, pInitialData, true, true, ppTexture2D);
}

    
HRESULT CD3D11TestFramework::CreateTexture2DMapDefaultOptional(
    _In_ const D3D11_TEXTURE2D_DESC *pDesc,
    _In_opt_ const D3D11_SUBRESOURCE_DATA *pInitialData,
    _Outptr_ ID3D11Texture2D **ppTexture2D
    )
{
    return CreateTexture2DHelper(pDesc, pInitialData, false, true, ppTexture2D);
}

HRESULT CD3D11TestFramework::CreateTexture2DHelper(
    _In_ const D3D11_TEXTURE2D_DESC *pDesc,
    _In_opt_ const D3D11_SUBRESOURCE_DATA *pInitialData,
    bool fTiledOptional,
    bool fMapDefaultOptional,
    _Outptr_ ID3D11Texture2D **ppTexture2D
    )
{    
    HRESULT hr = S_OK;
    *ppTexture2D = NULL;

    bool fMapSupported = false;
    UINT cpuFlags;
    D3D11_TEXTURE_LAYOUT layout;

    if (fMapDefaultOptional && !m_bMapDefaultDisabled)
    {
        // This method should only be used with default textures, or immutable
        // or staging resources which we will change to default.  
        if (pDesc->Usage == D3D11_USAGE_DEFAULT || pDesc->Usage == D3D11_USAGE_IMMUTABLE 
            || pDesc->Usage == D3D11_USAGE_STAGING)
        {
            // Require a linear layout when creating a default texture in place of a 
            // staging texture. This ensures the caller can map and read the texture.
            bool fRequireLinearCpuAccess = (pDesc->Usage == D3D11_USAGE_STAGING);

            if (FAILED(hr = TryGetMapDefaultParameters(
                fRequireLinearCpuAccess,
                2,
                pDesc->Format,
                pDesc->MipLevels,
                pDesc->ArraySize,
                pDesc->BindFlags,
                pDesc->SampleDesc,
                &fMapSupported,
                &cpuFlags,
                &layout)))
            {
                return hr;
            }
        }
    }
    
    if (fMapDefaultOptional && m_pOutputParamMapDefaultResourceUsed)
    {
        m_pOutputParamMapDefaultResourceUsed->SetValue(fMapSupported);
    }
    
    if (!fMapSupported)
    {
        if (fTiledOptional)
        {
            return CreateTexture2DTiledOptional(pDesc, pInitialData, ppTexture2D);
        }
        else
        {
            D3D11_TEXTURE2D_DESC LclDesc;
            const D3D11_TEXTURE2D_DESC* pLclDesc = ExerciseVariousNV12Capabilities(pDesc, &LclDesc);
            return GetDevice2()->CreateTexture2D(pLclDesc, pInitialData, ppTexture2D);
        }
    }
    
    if (fTiledOptional && m_pOutputParamTiledResourceUsed)
    {
        m_pOutputParamTiledResourceUsed->SetValue(false);
    }

    WriteToLog("Using 2D CPU-accessible default texture");

    // When creating a staging texture, don't modify the CPU access flags.
    // Just create a default texture instead of a staging texture
    if (pDesc->Usage == D3D11_USAGE_STAGING)
    {
        assert(pDesc->CPUAccessFlags != 0);
        cpuFlags = pDesc->CPUAccessFlags;
    }
    else
    {
        assert(pDesc->CPUAccessFlags == 0);
    }

    // Populate the D3D11_TEXTURE2D_DESC1 argument    

    D3D11_TEXTURE2D_DESC1 desc1;
    CopyMemory(&desc1, pDesc, sizeof(*pDesc));
    desc1.CPUAccessFlags = cpuFlags;
    desc1.TextureLayout = layout;
    desc1.Usage = D3D11_USAGE_DEFAULT;

    WindowsTest::CHandle<ID3D11Device3> spDevice3;
    hr = GetDevice2()->QueryInterface(__uuidof(ID3D11Device3), (void **) &spDevice3);
    if(FAILED(hr))
    {
        return hr;
    }

    WindowsTest::CHandle<ID3D11Texture2D1> spTexture2D1;
    hr = spDevice3->CreateTexture2D1(&desc1, pInitialData, &spTexture2D1);
    if(FAILED(hr))
    {
        return hr;
    }

    hr = spTexture2D1->QueryInterface(__uuidof(ID3D11Texture2D), (void **) ppTexture2D);
    if(FAILED(hr))
    {
        return hr;
    }

    return hr;
}

HRESULT CD3D11TestFramework::CreateTexture3DMapDefaultOptional(
    _In_ const D3D11_TEXTURE3D_DESC *pDesc,
    _In_opt_ const D3D11_SUBRESOURCE_DATA *pInitialData,
    _Outptr_ ID3D11Texture3D **ppTexture3D
    )
{    
    HRESULT hr = S_OK;
    *ppTexture3D = NULL;
    
    bool fSupported = false;
    UINT cpuFlags;
    D3D11_TEXTURE_LAYOUT layout;
    DXGI_SAMPLE_DESC sampleDesc = {1, 0};

    if (!m_bMapDefaultDisabled)
    {
        // This method should only be used with default textures, or immutable
        // or staging resources which we will change to default.  
        if (pDesc->Usage == D3D11_USAGE_DEFAULT || pDesc->Usage == D3D11_USAGE_IMMUTABLE 
            || pDesc->Usage == D3D11_USAGE_STAGING)
        {
            // Require a linear layout when creating a default texture in place of a 
            // staging texture. This ensures the caller can map and read the texture.
            bool fRequireLinearCpuAccess = (pDesc->Usage == D3D11_USAGE_STAGING);
        
            if (FAILED(hr = TryGetMapDefaultParameters(
                fRequireLinearCpuAccess,
                3,
                pDesc->Format,
                pDesc->MipLevels,
                1,
                pDesc->BindFlags,
                sampleDesc,
                &fSupported,
                &cpuFlags,
                &layout)))
            {
                return hr;
            }

        }
            
        if (!fSupported)
        {
            if (m_pOutputParamMapDefaultResourceUsed)
            {
                m_pOutputParamMapDefaultResourceUsed->SetValue(false);
            }

            return GetDevice2()->CreateTexture3D(pDesc, pInitialData, ppTexture3D);
        }
    }

    // When creating a staging texture, don't modify the CPU access flags.
    // Just create a default texture instead of a staging texture
    if (pDesc->Usage == D3D11_USAGE_STAGING)
    {
        assert(pDesc->CPUAccessFlags != 0);
        cpuFlags = pDesc->CPUAccessFlags;
    }
    else
    {
        assert(pDesc->CPUAccessFlags == 0);
    }
    
    if (m_pOutputParamMapDefaultResourceUsed)
    {
        m_pOutputParamMapDefaultResourceUsed->SetValue(true);
    }

    WriteToLog("Using 3D CPU-accessible default texture");

    // Populate the D3D11_TEXTURE3D_DESC1 argument    

    D3D11_TEXTURE3D_DESC1 desc1;
    CopyMemory(&desc1, pDesc, sizeof(*pDesc));
    desc1.CPUAccessFlags = cpuFlags;
    desc1.TextureLayout = layout;
    desc1.Usage = D3D11_USAGE_DEFAULT;

    WindowsTest::CHandle<ID3D11Device3> spDevice3;
    hr = GetDevice2()->QueryInterface(__uuidof(ID3D11Device3), (void **) &spDevice3);
    if(FAILED(hr))
    {
        return hr;
    }

    WindowsTest::CHandle<ID3D11Texture3D1> spTexture3D1;
    hr = spDevice3->CreateTexture3D1(&desc1, pInitialData, &spTexture3D1);
    if(FAILED(hr))
    {
        return hr;
    }

    hr = spTexture3D1->QueryInterface(__uuidof(ID3D11Texture3D), (void **) ppTexture3D);
    if(FAILED(hr))
    {
        return hr;
    }

    return hr;
}


HRESULT CD3D11TestFramework::CreateTexture2DTiledOptional(
    _In_ const D3D11_TEXTURE2D_DESC *pDesc, 
    _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, 
    _Out_ ID3D11Texture2D **ppTexture2D)
{
    HRESULT hr = m_tiledResourceHelper.CreateTexture2DTiledOptional(pDesc, pInitialData, ppTexture2D);
    if(m_pOutputParamTiledResourceUsed)
    {
        m_pOutputParamTiledResourceUsed->SetValue(m_tiledResourceHelper.WasLastResourceTiled());
    }

    if(m_tiledResourceHelper.WasLastResourceTiled())
    {
        WriteToLog("Using tiled texture2d");
    }
    else
    {
        D3D11_TEXTURE2D_DESC LclDesc;
        const D3D11_TEXTURE2D_DESC* pLclDesc = ExerciseVariousNV12Capabilities(pDesc, &LclDesc);
        hr = GetDevice2()->CreateTexture2D(pLclDesc, pInitialData, ppTexture2D);
    }

    return hr;
}

HRESULT CD3D11TestFramework::CreateBufferTiledOptionalMapDefaultOptional(
    _In_ const D3D11_BUFFER_DESC *pDesc, 
    _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, 
    _Out_ ID3D11Buffer **ppBuffer, 
    DXGI_FORMAT ViewFormat)
{
    // Attempt to switch a default buffers without CPU access to a default buffer 
    // with CPU access.  
    if (pDesc->Usage == D3D11_USAGE_DEFAULT && (pDesc->CPUAccessFlags == 0) && !m_bMapDefaultDisabled)
    {
        bool fCompatibleBindFlags = !!(pDesc->BindFlags &  (D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE)) &&
                                     !(pDesc->BindFlags & ~(D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE));

        bool fCompatibleMiscFlags = !(pDesc->MiscFlags & ~(D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS | D3D11_RESOURCE_MISC_BUFFER_STRUCTURED));

        if (fCompatibleBindFlags && fCompatibleMiscFlags)
        {
            // Placing additonal '0' entries in this struct places greater emphasis on testing
            // non-mappable video memory buffers.
            const int rgCpuFlags[] = 
            {
                0,
                0,
                0,
                0,
                D3D11_CPU_ACCESS_READ,
                D3D11_CPU_ACCESS_WRITE,
                D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE
            };
            
            // Create a psuedo-random number generator seeded with the test case index
            std::tr1::mt19937 generator;
            generator.seed(GetCurrentTestCase());

            // Define a distribution between 0 and ARRAYSIZE(rgCpuFlags) - 1 inclusive
            // and generate the random value
            std::tr1::uniform_int<DWORD> cpuUsageDist(0, ARRAYSIZE(rgCpuFlags) - 1);
            UINT cpuFlags = rgCpuFlags[cpuUsageDist(generator)];

            if (cpuFlags != 0)
            {
                // Find whether the CPU access on buffers is supported
                D3D11_FEATURE_DATA_D3D11_OPTIONS1 options1;
                HRESULT hr = S_OK;
                if ( FAILED ( hr = GetDevice2()->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, (void*)&options1, sizeof(options1)) ) )
                {
                    WriteToLog(_T("CheckFeatureSupport() failed"));
                    return hr;
                }
                
                if (options1.MapOnDefaultBuffers)
                {    
                    if (m_pOutputParamTiledResourceUsed)
                    {
                        m_pOutputParamTiledResourceUsed->SetValue(false);
                    }
                
                    if (m_pOutputParamMapDefaultResourceUsed)
                    {
                        m_pOutputParamMapDefaultResourceUsed->SetValue(true);
                    }
                    
                    D3D11_BUFFER_DESC desc = *pDesc;
                    desc.CPUAccessFlags = cpuFlags;
                    return GetDevice2()->CreateBuffer(&desc, pInitialData, ppBuffer);
                }
            }
        }
    }
    
    if (m_pOutputParamMapDefaultResourceUsed)
    {
        m_pOutputParamMapDefaultResourceUsed->SetValue(false);
    }

    return CreateBufferTiledOptional(pDesc, pInitialData, ppBuffer, ViewFormat);
}

HRESULT CD3D11TestFramework::CreateBufferTiledOptional(
    _In_ const D3D11_BUFFER_DESC *pDesc, 
    _In_ const D3D11_SUBRESOURCE_DATA *pInitialData, 
    _Out_ ID3D11Buffer **ppBuffer, 
    DXGI_FORMAT ViewFormat)
{
    HRESULT hr = m_tiledResourceHelper.CreateBufferTiledOptional(pDesc, pInitialData, ppBuffer, ViewFormat);
    if(m_pOutputParamTiledResourceUsed)
    {
        m_pOutputParamTiledResourceUsed->SetValue(m_tiledResourceHelper.WasLastResourceTiled());
    }

    if(m_tiledResourceHelper.WasLastResourceTiled())
    {
        WriteToLog("Using tiled buffer");
    }
    
    return hr;
}

_Use_decl_annotations_
const D3D11_TEXTURE2D_DESC* CD3D11TestFramework::ExerciseVariousNV12Capabilities(
    const D3D11_TEXTURE2D_DESC* pDesc
    , D3D11_TEXTURE2D_DESC* pLclDesc
    )
{
    if (GetD3DDeviceManager()->IsExtendedNV12SharedSupport()
        && pDesc->Format == DXGI_FORMAT_NV12
        && pDesc->Usage == D3D11_USAGE_DEFAULT
        && pDesc->CPUAccessFlags == 0
        && pDesc->ArraySize <= 2
        && !(pDesc->BindFlags & D3D11_BIND_DECODER)
        )
    {
        *pLclDesc = *pDesc;
        std::tr1::mt19937 generator;
        generator.seed(GetCurrentTestCase());
        
        if (!(pLclDesc->BindFlags & (D3D11_PRIVATE_BIND_CAPTURE | D3D11_BIND_VIDEO_ENCODER)))
        {
            const DWORD BindFlagAdditions[] =
            {
                0,
                D3D11_PRIVATE_BIND_CAPTURE,
                D3D11_BIND_VIDEO_ENCODER,
            };
            std::tr1::uniform_int<DWORD> BindFlagAdditionDist(0, ARRAYSIZE(BindFlagAdditions) - 1);
            DWORD BindFlagAddition = BindFlagAdditions[BindFlagAdditionDist(generator)];

            // Setting video-encoder requires additional flags if shader-resource is also set:
            if ((BindFlagAddition & D3D11_BIND_VIDEO_ENCODER) != 0 && (pLclDesc->BindFlags & D3D11_BIND_SHADER_RESOURCE) != 0)
            {
                // Prefer to set render-target or unordered-access, as they don't introduce further interaction complications.
                const DWORD EncoderFlagAdditions[] =
                {
                    D3D11_BIND_RENDER_TARGET,
                    D3D11_BIND_UNORDERED_ACCESS,
                };
                std::tr1::uniform_int<DWORD> EncoderFlagAdditionDist(0, ARRAYSIZE(EncoderFlagAdditions) - 1);
                BindFlagAddition |= EncoderFlagAdditions[EncoderFlagAdditionDist(generator)];
            }
            
            pLclDesc->BindFlags |= BindFlagAddition;
        }
        
        if ((pLclDesc->BindFlags & (D3D11_PRIVATE_BIND_CAPTURE | D3D11_BIND_VIDEO_ENCODER)) != 0
            && !(pLclDesc->MiscFlags & (D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_RESOURCE_CLAMP))
            && !(pLclDesc->MiscFlags & (D3D11_RESOURCE_MISC_SHARED | D3D11_RESOURCE_MISC_SHARED_NTHANDLE | D3D11_RESOURCE_MISC_SHARED_NTHANDLE))
            )
        {
            const DWORD MiscFlagAdditions[] =
            {
                0,
                D3D11_RESOURCE_MISC_SHARED_NTHANDLE | D3D11_RESOURCE_MISC_SHARED,
            };
            std::tr1::uniform_int<DWORD> MiscFlagAdditionDist(0, ARRAYSIZE(MiscFlagAdditions) - 1);
            pLclDesc->MiscFlags |= MiscFlagAdditions[MiscFlagAdditionDist(generator)];
        }
        return pLclDesc;        
    }
    return pDesc;
}

TEST_RESULT CD3D11TestFramework::SetupDXGIAndD3D()
{
    m_pD3DDeviceManager->InvalidateDXGI();
    m_pD3DDeviceManager->InvalidateD3D();
        
    return RefreshGraphics();
}

TEST_RESULT CD3D11TestFramework::SetupDXGI()
{
    m_pD3DDeviceManager->InvalidateDXGI();
        
    return RefreshGraphics();
}

TEST_RESULT CD3D11TestFramework::SetupD3D()
{
    m_pD3DDeviceManager->InvalidateD3D();
        
    return RefreshGraphics();
}

TEST_RESULT CD3D11TestFramework::SetupRenderTarget()
{
    m_pD3DDeviceManager->InvalidateRenderTarget();
        
    return RefreshGraphics();
}

TEST_RESULT CD3D11TestFramework::RefreshGraphics()
{
    HRESULT hr = S_OK;

    hr = m_pD3DDeviceManager->RefreshGraphics();

    if (E_NOINTERFACE == hr)
    {
        // E_NOINTERFACE implies the device is incapable of supporting a test/framework requested feature.
        // Treat this as a skip rather than a fail
        return RESULT_SKIP;
    }

    if (FAILED(hr))
    {
        return RESULT_FAIL;
    }

    if (m_pD3DDeviceManager->m_RefreshGraphicsFlags & CD3D11DeviceManager::D3D_RECYCLED)
    {
        if (GetDevice2())
        {
            m_tiledResourceHelper.Cleanup();
            hr = m_tiledResourceHelper.Setup(GetDevice2(), m_pLogWrapper);
            if (FAILED(hr))
                return RESULT_FAIL;
        }

        if (GetDevice1())
        {
            m_resourceViewer.Cleanup();
            hr = m_resourceViewer.Setup(GetDevice1());
            if (FAILED(hr))
                return RESULT_FAIL;
        }
    }

    return RESULT_PASS;
}

void CD3D11TestFramework::SetRenderTargetProperties() 
{
    HWND hWnd = NULL;
    bool bFullScreen = false;
    bool bCreateRTFromSwapChainProps = false;

    CD3D11Test *pTest = dynamic_cast< CD3D11Test *>( GetCurrentTest() );

    if(pTest)
    {
        UINT numRenderWindowsPerDevice = pTest->GetNumRenderWindows();
        bCreateRTFromSwapChainProps = pTest->CreateRTFromSwapChainProps();

        // Make sure the shell has the desired number of test windows.
        if( GetShell() )
        {
            GetShell()->SetNumDeviceWindows( numRenderWindowsPerDevice );

            if ( numRenderWindowsPerDevice > 0 )
            {
                bFullScreen = pTest->FullScreen();

                if( bFullScreen )
                {
                    hWnd = GetShell()->GetMainHWnd();
                }
                else
                {
                    hWnd = GetShell()->GetDeviceRenderWindow(0);
                }
            }
        }
    }

    m_pD3DDeviceManager->SetRenderTargetProperties(hWnd, bFullScreen, bCreateRTFromSwapChainProps);
}

void CD3D11TestFramework::CleanupDXGI() 
{
    CleanupD3D();
    m_pD3DDeviceManager->CleanupDXGI(); 
}

void CD3D11TestFramework::CleanupD3D()
{
    CleanupRenderTarget();
    m_resourceViewer.Cleanup();
    m_tiledResourceHelper.Cleanup();
    m_pD3DDeviceManager->CleanupD3D();
}

void CD3D11TestFramework::CleanupRenderTarget()
{
    m_pD3DDeviceManager->CleanupRenderTarget(); 
}


// Entries from D3DCompiler_TEST.dll
/////////////////////////////////////////////////////////////
HRESULT CD3D11TestFramework::LL_D3DDisassemble( 
               LPCVOID pSrcData,
               SIZE_T SrcDataSize,
               UINT Flags,
               LPCSTR szComments,
               ID3D10Blob** ppDisassembly)
{
    HMODULE hD3DCompile = LoadLibrary("D3DCompiler_TEST.dll");
    if( hD3DCompile == NULL )
    {
        WriteError( _T( "D3DCompiler_TEST.dll not found." ) );
        return E_FAIL;
    }

    PA_D3DDisassemble *pD3DDisassemble = (PA_D3DDisassemble*) GetProcAddress( hD3DCompile, "D3DDisassemble" );
    if( pD3DDisassemble == NULL )
    {
        WriteError( _T("D3DDisassemble - Function not found in D3DCompiler_TEST.dll") );
        return E_FAIL;
    }

    return pD3DDisassemble( pSrcData, SrcDataSize, Flags, szComments, ppDisassembly );
}

HRESULT CD3D11TestFramework::LL_D3DReflect(
                LPCVOID pSrcData,
                SIZE_T SrcDataSize,
                REFIID pInterface,
                void** ppReflector)
{
    HMODULE hD3DCompile = LoadLibrary("D3DCompiler_TEST.dll");
    if( hD3DCompile == NULL )
    {
        WriteError( _T( "D3DCompiler_TEST.dll not found." ) );
        return E_FAIL;
    }

    PA_D3DReflect *pD3DReflect = (PA_D3DReflect*) GetProcAddress( hD3DCompile, "D3DReflect" );
    if( pD3DReflect == NULL )
    {
        WriteError( _T("D3DReflect - Function not found in D3DCompiler_TEST.dll") );
        return E_FAIL;
    }

    return pD3DReflect( pSrcData, SrcDataSize, pInterface, ppReflector );
}


HRESULT CD3D11TestFramework::LL_D3DGetInputAndOutputSignatureBlob(
                LPCVOID pSrcData,
                SIZE_T SrcDataSize,
                ID3DBlob** ppSignatureBlob)
{
    HMODULE hD3DCompile = LoadLibrary("D3DCompiler_TEST.dll");
    if( hD3DCompile == NULL )
    {
        WriteError( _T( "D3DCompiler_TEST.dll not found." ) );
        return E_FAIL;
    }

    PA_D3DGetSignatureBlob *pD3DGetInputAndOutputSignatureBlob = (PA_D3DGetSignatureBlob*) GetProcAddress( hD3DCompile, "D3DGetInputAndOutputSignatureBlob" );
    if( pD3DGetInputAndOutputSignatureBlob == NULL )
    {
        WriteError( _T("pD3DGetInputAndOutputSignatureBlob - Function not found in D3DCompiler_TEST.dll") );
        return E_FAIL;
    }

    return pD3DGetInputAndOutputSignatureBlob( pSrcData, SrcDataSize, ppSignatureBlob );
}





//--------------------------------------------------------------------

CD3D11Test::CD3D11Test()
{
}

void CD3D11Test::InitOutputParameters()
{
    GetFramework()->InitOutputParameters(this);
}

bool CD3D11Test::BlockingFinish() 
{
    return GetFramework()->GetD3DDeviceManager()->WaitForAllPendingGraphicsWork(10000);
}

void CD3D11Test::Cleanup()
{
    CTest::Cleanup();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
