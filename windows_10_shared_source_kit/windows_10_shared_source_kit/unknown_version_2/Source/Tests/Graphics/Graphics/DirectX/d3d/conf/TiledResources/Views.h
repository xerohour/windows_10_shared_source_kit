////////////////////////////////////////////////////////////////////////////////
// Views.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TiledResources.h"

// Base class for all Resource View tests, except for Depth Stencil
// because Texture3D can not be used as a depth stencil
//
// Each derived test (per view) must implement it's own parameter
// initialization and text execution functions. This means
// each derived test will be another template based on Texture2D or
// Texture3D
template <TILED_RESOURCE_BASE resourceBase>
class CTiledResourceViewTestBase : public CResourceBaseTestHelper<resourceBase>
{
protected:
    // Helpers to clean up typenames
    CRESOURCE_BASE_TEST_HELPER__TYPENAME_TYPEDEFS

        ID3D11TextureType *m_pTiledResource;
    ID3D11TextureType *m_pRefResource;
    TileMappingType m_tileMapping;

    struct
    {
        CVariationParameter<RESIDENCY_PATTERN_2D_3D> *pResidencyPattern;
    } m_parameters;

protected:
    CTiledResourceViewTestBase() : CResourceBaseTestHelper<resourceBase>(), m_pTiledResource(NULL), m_pRefResource(NULL)
    {
        m_parameters.pResidencyPattern = NULL;
    }

    TEST_RESULT SetupTestCase();
    void CleanupTestCase();

    TEST_RESULT CreateResources(DXGI_FORMAT format, const UINT bindFlags);

    virtual TEST_RESULT CreateResources() = 0;
};


// Render Target Base
template <TILED_RESOURCE_BASE resourceBase>
class CTiledRenderTargetViewTestBase : public CTiledResourceViewTestBase<resourceBase>
{
protected:
    TEST_RESULT CreateResources()
    {
        return CTiledResourceViewTestBase<resourceBase>::CreateResources(DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_RENDER_TARGET);
    }

    void InitTestParameters();
    TEST_RESULT ExecuteTestCase();
};


// Shader Resource Base
template <TILED_RESOURCE_BASE resourceBase>
class CTiledShaderResourceViewTestBase : public CTiledResourceViewTestBase<resourceBase>
{
protected:
    TEST_RESULT CreateResources()
    {
        return CTiledResourceViewTestBase<resourceBase>::CreateResources(DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE);
    }

    void InitTestParameters();
    TEST_RESULT ExecuteTestCase();
};


// Unordered Access additional parameters
enum RW_MODE
{
    RW_MODE_READ_FROM_TILED_RESOURCE = 0,
    RW_MODE_WRITE_TO_TILED_RESOURCE = 1,
};
DECLARE_NAMED_VALUES(RW_MODE);


// Unordered Access Base
template <TILED_RESOURCE_BASE resourceBase>
class CTiledUnorderedAccessViewTestBase : public CTiledResourceViewTestBase<resourceBase>
{
public:
    CTiledUnorderedAccessViewTestBase() : CTiledResourceViewTestBase<resourceBase>(), m_pCSTiled(nullptr), m_pCSNormal(nullptr) { }

protected:
    TEST_RESULT CreateResources()
    {
        return CTiledResourceViewTestBase<resourceBase>::CreateResources(DXGI_FORMAT_R8G8B8A8_TYPELESS, D3D11_BIND_UNORDERED_ACCESS);
    }

    void Cleanup()
    {
        SAFE_RELEASE(m_pCSTiled);
        SAFE_RELEASE(m_pCSNormal);
        CTiledResourceViewTestBase<resourceBase>::Cleanup();
    }

    void InitTestParameters();
    TEST_RESULT Setup();
    TEST_RESULT ExecuteTestCase();

protected:
    struct
    {
        CVariationParameter<RW_MODE> *pReadWriteMode;
    } m_parameters;

    ID3D11ComputeShader *m_pCSTiled;
    ID3D11ComputeShader *m_pCSNormal;
};




// Actual Tests
class CTiledRenderTargetViewTestTex2D : public CTiledRenderTargetViewTestBase < TRB_TEXTURE_2D > { };
class CTiledRenderTargetViewTestTex3D : public CTiledRenderTargetViewTestBase < TRB_TEXTURE_3D > { };

class CTiledShaderResourceViewTestTex2D : public CTiledShaderResourceViewTestBase < TRB_TEXTURE_2D > { };
class CTiledShaderResourceViewTestTex3D : public CTiledShaderResourceViewTestBase < TRB_TEXTURE_3D > { };

class CTiledUnorderedAccessViewTestTex2D : public CTiledUnorderedAccessViewTestBase < TRB_TEXTURE_2D > { };
class CTiledUnorderedAccessViewTestTex3D : public CTiledUnorderedAccessViewTestBase < TRB_TEXTURE_3D > { };


// Only Texture2D can be used as a depth stencil
class CTiledDepthStencilViewTest : public CTiledResourceViewTestBase < TRB_TEXTURE_2D >
{
public:
    CTiledDepthStencilViewTest();
    void InitTestParameters();

protected:
    TEST_RESULT Setup();
    void Cleanup();

    TEST_RESULT CreateResources();
    TEST_RESULT ExecuteTestCase();

protected:
    struct
    {
        CVariationParameter<DXGI_FORMAT> *pDepthFormat;
    } m_parameters;

    ID3D11Texture2D *m_pSRTexture;	// Texture used to render both depth and color
    ID3D11ShaderResourceView *m_pSRView;
    ID3D11PixelShader *m_pRenderDepthPS;
    ID3D11PixelShader *m_pRenderColorPS;
    ID3D11RasterizerState1 *m_pRS;
    ID3D11DepthStencilState *m_pDSS;
};




// Implementations
template <TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CTiledResourceViewTestBase<resourceBase>::SetupTestCase()
{
    TEST_RESULT tr = CTiledResourcesTest::SetupTestCase();
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    tr = CreateResources();
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    return tr;
}


template <TILED_RESOURCE_BASE resourceBase>
void CTiledResourceViewTestBase<resourceBase>::CleanupTestCase()
{
    SAFE_RELEASE(m_pTiledResource);
    SAFE_RELEASE(m_pRefResource);
    GetFramework()->GetTilePoolManager()->CleanupResources();
}


template <TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CTiledResourceViewTestBase<resourceBase>::CreateResources(DXGI_FORMAT format, const UINT bindFlags)
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    ID3D11Device2 *pDevice = GetDevice2();
    CTiledResourceHelper::CTilePoolManager *pTilePoolManager = GetFramework()->GetTilePoolManager();

    UINT tileWidth;
    UINT tileHeight;
    UINT tileDepth = 1;     // need to initialize when using Texture2D

    if (TRB_TEXTURE_2D == resourceBase)
    {
        CTiledResourcesTest::GetTileSize(format, tileWidth, tileHeight);
    }
    else
    {
        CTiledResourcesTest::GetTileSize(format, tileWidth, tileHeight, tileDepth);
    }

    const UINT width = 4 * tileWidth;
    const UINT height = 4 * tileHeight;
    const UINT depth = 4 * tileDepth;

    CD3D11_TEXTURE_TYPE_DESC desc;

    // Create the reference texture
    CResourceBaseTestHelper<resourceBase>::GenerateTextureDesc(
        desc,
        format,
        width,
        height,
        (TRB_TEXTURE_2D == resourceBase ? 1 : depth),   // arraySize (Texture2D) or depth (Texture3D)
        1,                                              // mipLevels
        bindFlags,                                      // bindFlags
        D3D11_USAGE_DEFAULT,                            // usage
        0,                                              // cpaccessFlags
        0,                                              // miscFlags
        1,                                              // sampleCount (ignored for Texture3D)
        0                                               // sampleQuality (ignored for Texture3D)
        );

    tr = CTiledResourcesTest::CreateTexture(&desc, &m_pRefResource);
    if (RESULT_PASS != tr)
    {
        return tr;
    }


    // Create the tiled texture
    desc.MiscFlags = D3D11_RESOURCE_MISC_TILED;

    tr = CTiledResourcesTest::CreateCustomTiledTexture(&desc, &m_pTiledResource);

    // Allocate the tiles from the framework tile pool
    UINT startTiles = pTilePoolManager->FetchRange(m_pTiledResource, CResourceBaseTestHelper<resourceBase>::totalNumTiles);

    // Initialize all tier-1 tile mappings to blank tile
    if (1 == CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue())
    {
        // Set the entire resource to the shared NULL tile
        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(m_pTiledResource, 1, pTilePoolManager->GetTilePool(), RP2D_3D_CLEAR_ALL_MIPS, m_tileMapping);
        if (RESULT_PASS != tr)
        {
            return tr;
        }
    }

    // Update the tiled resources
    RESIDENCY_PATTERN_2D_3D residencyPattern = m_parameters.pResidencyPattern->GetValue();
    tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(m_pTiledResource, startTiles, pTilePoolManager->GetTilePool(), residencyPattern, m_tileMapping);

    return tr;
}




// Render Target
template <TILED_RESOURCE_BASE resourceBase>
void CTiledRenderTargetViewTestBase<resourceBase>::InitTestParameters()
{
    CreateParameter(_T("ResidencyPattern"), &CTiledResourceViewTestBase<resourceBase>::m_parameters.pResidencyPattern);

    CUserValueSet<RESIDENCY_PATTERN_2D_3D> *pResidencyPatternValues = new CUserValueSet<RESIDENCY_PATTERN_2D_3D>();
    pResidencyPatternValues->AddValue(RP2D_3D_FULL);
    pResidencyPatternValues->AddValue(RP2D_3D_FRAGMENTED_MULTI_TILE);
    pResidencyPatternValues->AddValue(RP2D_3D_UNIFORM_BOX);

    testfactor::RFactor residencyPattern = AddParameterValueSet<RESIDENCY_PATTERN_2D_3D>(
        CTiledResourceViewTestBase<resourceBase>::m_parameters.pResidencyPattern,
        pResidencyPatternValues
        );

    // When using Texture2D, skip building factors for Tier 3 (Tier 2 tests correct functionality)
    // When using Texture3D, skip building factors for Tiers 1 and 2
    testfactor::RFactor tiledResourcesTier = CTiledResourcesTest::InitTiledResourcesTierParam((TRB_TEXTURE_3D == resourceBase));

    SetRootTestFactor(residencyPattern * tiledResourcesTier);
}

template <TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CTiledRenderTargetViewTestBase<resourceBase>::ExecuteTestCase()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    CHandle<ID3D11TextureType> pStagingSource;
    CHandle<ID3D11TextureType> pSourceTexture;
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    ID3D11Device2 *pDevice = GetDevice2();

    // Width, Height and Depth are both 4
    const UINT width = 4 * CResourceBaseTestHelper<resourceBase>::widthPerTile;
    const UINT height = 4 * CResourceBaseTestHelper<resourceBase>::heightPerTile;
    const UINT depth = 4 * CResourceBaseTestHelper<resourceBase>::depthPerTile;     // depth ignored by Texture2D methods

    // Setup the staging source texture used to initialize the SRV
    // texture that then gets rendered to the RTV
    tr = CResourceBaseTestHelper<resourceBase>::CreateStagingTexture(width, height, depth, &pStagingSource, true);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Create the SRV texture
    CD3D11_TEXTURE_TYPE_DESC desc;
    CResourceBaseTestHelper<resourceBase>::GenerateTextureDesc(
        desc,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        width,
        height,
        (TRB_TEXTURE_2D == resourceBase ? 1 : depth),   // arraySize (Texture2D) or depth (Texture3D)
        0,                                              // mipLevels (generates all)
        D3D11_BIND_SHADER_RESOURCE,                     // bindFlags
        D3D11_USAGE_DEFAULT,                            // usage
        0,                                              // cpaccessFlags
        0,                                              // miscFlags
        1,                                              // sampleCount (ignored for Texture3D)
        0                                               // sampleQuality (ignored for Texture3D)
        );
    tr = CTiledResourcesTest::CreateTexture(&desc, &pSourceTexture);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Copy the staging texture to the SRV texture
    // Copy the source data to the fully mapped tiled resource
    pContext->CopyResource(pSourceTexture, pStagingSource);

    CHandle<ID3D11ShaderResourceView> pSRV;
    hr = pDevice->CreateShaderResourceView(pSourceTexture, nullptr, &pSRV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateShaderResourceView);
        return RESULT_FAIL;
    }

    CHandle<ID3D11RenderTargetView> pRTVTiled;
    hr = pDevice->CreateRenderTargetView(m_pTiledResource, nullptr, &pRTVTiled);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
        return RESULT_FAIL;
    }

    CHandle<ID3D11RenderTargetView> pRTVNormal;
    hr = pDevice->CreateRenderTargetView(m_pRefResource, nullptr, &pRTVNormal);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
        return RESULT_FAIL;
    }

    pContext->TiledResourceBarrier(nullptr, m_pTiledResource);
    float black[4] = { 0 };
    CResourceBaseTestHelper<resourceBase>::ClearView(pRTVTiled, DXGI_FORMAT_R8G8B8A8_UNORM, black, nullptr, 0);
    tr = CResourceBaseTestHelper<resourceBase>::RenderViewToSimilar(pRTVTiled, pSRV, width, height, depth, (TRB_TEXTURE_3D == resourceBase));
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Render the SRV texture to the reference resource
    tr = CResourceBaseTestHelper<resourceBase>::RenderViewToSimilar(pRTVNormal, pSRV, width, height, depth, (TRB_TEXTURE_3D == resourceBase));
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    CTiledResourcesTest::ClearBlankRegions(m_tileMapping, pRTVNormal, desc.Format);
    CTiledResourcesTest::ClearBlankRegions(m_tileMapping, pRTVTiled, desc.Format);

    const UINT numSlices = (TRB_TEXTURE_2D == resourceBase ? 1 : depth);
    UINT nameIndex = 0;
    wchar_t combinedName[3 * numSlices][64];
    for (UINT slice = 0; slice < numSlices; ++slice)
    {
        wchar_t sliceName[32];
        auto GenerateName = [&](const wchar_t *prependName, const UINT inSlice) -> const wchar_t*
        {
            swprintf_s(sliceName, ARRAYSIZE(sliceName), L" (Slice %d)", inSlice);
            swprintf_s(combinedName[nameIndex], ARRAYSIZE(combinedName[nameIndex]), L"%s%s", prependName, (numSlices == 1 ? L"" : sliceName));
            return combinedName[nameIndex++];
        };
        ShowResource(GenerateName(L"Source (non-tiled)", slice), pSourceTexture, 0, 0, slice);
        ShowResource(GenerateName(L"Dest (tiled)", slice), m_pTiledResource, 0, 0, slice);
        ShowResource(GenerateName(L"Expected results (tiled)", slice), m_pRefResource, 0, 0, slice);
    }

    // Compare the tiled resource to the reference resource data
    tr = CTiledResourcesTest::CompareTiledResourceToReference(m_pTiledResource, m_pRefResource, 0, 0, m_tileMapping);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    if (RESULT_PASS != CheckNullSentinel())
    {
        tr = RESULT_FAIL;
    }

    return tr;
}


// Shader Resource
template <TILED_RESOURCE_BASE resourceBase>
void CTiledShaderResourceViewTestBase<resourceBase>::InitTestParameters()
{
    CreateParameter(_T("ResidencyPattern"), &CTiledResourceViewTestBase<resourceBase>::m_parameters.pResidencyPattern);

    CUserValueSet<RESIDENCY_PATTERN_2D_3D> *pResidencyPatternValues = new CUserValueSet<RESIDENCY_PATTERN_2D_3D>();
    pResidencyPatternValues->AddValue(RP2D_3D_FULL);
    pResidencyPatternValues->AddValue(RP2D_3D_FRAGMENTED_MULTI_TILE);
    pResidencyPatternValues->AddValue(RP2D_3D_UNIFORM_BOX);

    testfactor::RFactor residencyPattern = AddParameterValueSet<RESIDENCY_PATTERN_2D_3D>(
        CTiledResourceViewTestBase<resourceBase>::m_parameters.pResidencyPattern,
        pResidencyPatternValues
        );

    // When using Texture2D, skip building factors for Tier 3 (Tier 2 tests correct functionality)
    // When using Texture3D, skip building factors for Tiers 1 and 2
    testfactor::RFactor tiledResourcesTier = CTiledResourcesTest::InitTiledResourcesTierParam((TRB_TEXTURE_3D == resourceBase));

    SetRootTestFactor(residencyPattern * tiledResourcesTier);
}

template <TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CTiledShaderResourceViewTestBase<resourceBase>::ExecuteTestCase()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    CHandle<ID3D11TextureType> pStagingSource;
    CHandle<ID3D11TextureType> pRenderTarget1;
    CHandle<ID3D11TextureType> pRenderTarget2;
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    ID3D11Device2 *pDevice = GetDevice2();

    // Width, Height and Depth are both 4
    const UINT width = 4 * CResourceBaseTestHelper<resourceBase>::widthPerTile;
    const UINT height = 4 * CResourceBaseTestHelper<resourceBase>::heightPerTile;
    const UINT depth = 4 * CResourceBaseTestHelper<resourceBase>::depthPerTile;     // depth ignored by Texture2D methods

    // Setup the staging source texture used to initialize the SRV
    // texture that then gets rendered to the RTV
    tr = CResourceBaseTestHelper<resourceBase>::CreateStagingTexture(width, height, depth, &pStagingSource, true);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Create two render target textures
    CD3D11_TEXTURE_TYPE_DESC desc;
    CResourceBaseTestHelper<resourceBase>::GenerateTextureDesc(
        desc,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        width,
        height,
        (TRB_TEXTURE_2D == resourceBase ? 1 : depth),   // arraySize (Texture2D) or depth (Texture3D)
        0,                                              // mipLevels (generates all)
        D3D11_BIND_RENDER_TARGET,                       // bindFlags
        D3D11_USAGE_DEFAULT,                            // usage
        0,                                              // cpaccessFlags
        0,                                              // miscFlags
        1,                                              // sampleCount (ignored for Texture3D)
        0                                               // sampleQuality (ignored for Texture3D)
        );
    tr = CTiledResourcesTest::CreateTexture(&desc, &pRenderTarget1);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    tr = CTiledResourcesTest::CreateTexture(&desc, &pRenderTarget2);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Copy the staging texture to both the tiled and non-tiled SRV texture
    // Copy the source data to the fully mapped tiled resource
    pContext->TiledResourceBarrier(nullptr, m_pTiledResource);
    pContext->CopySubresourceRegion(m_pTiledResource, 0, 0, 0, 0, pStagingSource, 0, nullptr);
    pContext->CopySubresourceRegion(m_pRefResource, 0, 0, 0, 0, pStagingSource, 0, nullptr);

    // Render the tiled SRV texture to the first render target
    tr = CTiledResourcesTest::RenderTextureToSimilar(pRenderTarget1, m_pTiledResource, 0, 0);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Render the SRV texture to the reference resource
    tr = CTiledResourcesTest::RenderTextureToSimilar(pRenderTarget2, m_pRefResource, 0, 0);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    const UINT numSlices = (TRB_TEXTURE_2D == resourceBase ? 1 : depth);
    UINT nameIndex = 0;
    wchar_t combinedName[4 * numSlices][64];
    for (UINT slice = 0; slice < numSlices; ++slice)
    {
        wchar_t sliceName[32];
        auto GenerateName = [&](const wchar_t *prependName, const UINT inSlice) -> const wchar_t*
        {
            swprintf_s(sliceName, ARRAYSIZE(sliceName), L" (Slice %d)", inSlice);
            swprintf_s(combinedName[nameIndex], ARRAYSIZE(combinedName[nameIndex]), L"%s%s", prependName, (numSlices == 1 ? L"" : sliceName));
            return combinedName[nameIndex++];
        };
        ShowResource(GenerateName(L"Tiled SRV Texture", slice), m_pTiledResource, 0, 0, slice);
        ShowResource(GenerateName(L"Normal SRV Texture", slice), m_pRefResource, 0, 0, slice);
        ShowResource(GenerateName(L"Tiled Render Results", slice), pRenderTarget1, 0, 0, slice);
        ShowResource(GenerateName(L"Normal Render Results", slice), pRenderTarget2, 0, 0, slice);
    }

    // Compare the tiled resource to the reference resource data
    tr = CTiledResourcesTest::CompareTiledResourceToReference(pRenderTarget1, pRenderTarget2, 0, 0, m_tileMapping);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    if (RESULT_PASS != CheckNullSentinel())
    {
        tr = RESULT_FAIL;
    }

    return tr;
}


// Unordered Access
template <TILED_RESOURCE_BASE resourceBase>
void CTiledUnorderedAccessViewTestBase<resourceBase>::InitTestParameters()
{
    CreateParameter(_T("ResidencyPattern"), &CTiledResourceViewTestBase<resourceBase>::m_parameters.pResidencyPattern);
    CreateParameter(_T("Mode"), &m_parameters.pReadWriteMode);

    CUserValueSet<RESIDENCY_PATTERN_2D_3D> *pResidencyPatternValues = new CUserValueSet<RESIDENCY_PATTERN_2D_3D>();
    pResidencyPatternValues->AddValue(RP2D_3D_FULL);
    pResidencyPatternValues->AddValue(RP2D_3D_FRAGMENTED_MULTI_TILE);
    pResidencyPatternValues->AddValue(RP2D_3D_UNIFORM_BOX);

    CUserValueSet<RW_MODE> *pReadWriteValues = new CUserValueSet<RW_MODE>();
    pReadWriteValues->AddValue(RW_MODE_READ_FROM_TILED_RESOURCE);
    pReadWriteValues->AddValue(RW_MODE_WRITE_TO_TILED_RESOURCE);

    testfactor::RFactor residencyPattern = AddParameterValueSet<RESIDENCY_PATTERN_2D_3D>(
        CTiledResourceViewTestBase<resourceBase>::m_parameters.pResidencyPattern,
        pResidencyPatternValues
        );

    // When using Texture2D, skip building factors for Tier 3 (Tier 2 tests correct functionality)
    // When using Texture3D, skip building factors for Tiers 1 and 2
    testfactor::RFactor tiledResourcesTier = CTiledResourcesTest::InitTiledResourcesTierParam((TRB_TEXTURE_3D == resourceBase));

    testfactor::RFactor readWriteMode = AddParameterValueSet<RW_MODE>(
        m_parameters.pReadWriteMode,
        pReadWriteValues
        );

    SetRootTestFactor(readWriteMode * residencyPattern * tiledResourcesTier);
}

template <TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CTiledUnorderedAccessViewTestBase<resourceBase>::Setup()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    ID3D11Device2 *pDevice = GetDevice2();
    CHandle<ID3D11ComputeShader> pCSTiled;
    CHandle<ID3D11ComputeShader> pCSNormal;

    tr = CTiledResourceViewTestBase<resourceBase>::Setup();
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Get correct shaders based on whether we use Texture2D or Texture3D
    const BYTE *pShaderByteCodeNormal;
    const BYTE *pShaderByteCodeTiled;
    SIZE_T shaderLenNormal;
    SIZE_T shaderLenTiled;
    CTiledResourcesTest::GetShaderByteCode(CTiledResourcesTest::LSBT_COMPUTE_COPY_TEX, &pShaderByteCodeNormal, &shaderLenNormal, false, (TRB_TEXTURE_3D == resourceBase));
    CTiledResourcesTest::GetShaderByteCode(CTiledResourcesTest::LSBT_COMPUTE_COPY_TEX, &pShaderByteCodeTiled, &shaderLenTiled, true, (TRB_TEXTURE_3D == resourceBase));

    //
    // Create the compute shaders
    //
    hr = pDevice->CreateComputeShader(pShaderByteCodeNormal, shaderLenNormal, NULL, &pCSNormal);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateComputeShader);
        return RESULT_FAIL;
    }

    if (1 < m_maxSupportedTiledResourcesTier)
    {
        hr = pDevice->CreateComputeShader(pShaderByteCodeTiled, shaderLenTiled, NULL, &pCSTiled);
        if (FAILED(hr))
        {
            REPORT_HRESULT_FAILURE(hr, CreateComputeShader);
            return RESULT_FAIL;
        }
    }

    // Detach temporary smart pointers
    m_pCSTiled = pCSTiled;
    pCSTiled.Detach();
    m_pCSNormal = pCSNormal;
    pCSNormal.Detach();

    return tr;
}

template <TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CTiledUnorderedAccessViewTestBase<resourceBase>::ExecuteTestCase()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    CHandle<ID3D11TextureType> pNormalTexture;
    CHandle<ID3D11TextureType> pUntiledCopyTexture;
    CHandle<ID3D11TextureType> pTempTexture;
    CHandle<ID3D11TextureType> pControlTexture;
    CHandle<ID3D11UnorderedAccessView> pNormalView;
    CHandle<ID3D11UnorderedAccessView> pTiledView;
    CHandle<ID3D11UnorderedAccessView> pUntiledCopyView;
    CHandle<ID3D11UnorderedAccessView> pControlView;
    CTiledResourceHelper::CTilePoolManager *pTilePoolManager = GetFramework()->GetTilePoolManager();
    TileMappingType tileMapping;

    // Width, Height and Depth are both 4
    const UINT width = 4 * CResourceBaseTestHelper<resourceBase>::widthPerTile;
    const UINT height = 4 * CResourceBaseTestHelper<resourceBase>::heightPerTile;
    const UINT depth = 4 * CResourceBaseTestHelper<resourceBase>::depthPerTile;     // depth ignored by Texture2D methods

    // Setup the staging source texture used to initialize the SRV
    // texture that then gets rendered to the RTV
    tr = CResourceBaseTestHelper<resourceBase>::CreateStagingTexture(width, height, depth, &pTempTexture, true);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Create normal (non-tiled) texture
    CD3D11_TEXTURE_TYPE_DESC desc;
    CResourceBaseTestHelper<resourceBase>::GenerateTextureDesc(
        desc,
        DXGI_FORMAT_R8G8B8A8_TYPELESS,
        width,
        height,
        (TRB_TEXTURE_2D == resourceBase ? 1 : depth),   // arraySize (Texture2D) or depth (Texture3D)
        0,                                              // mipLevels (generates all)
        D3D11_BIND_UNORDERED_ACCESS,                    // bindFlags
        D3D11_USAGE_DEFAULT,                            // usage
        0,                                              // cpaccessFlags
        0,                                              // miscFlags
        1,                                              // sampleCount (ignored for Texture3D)
        0                                               // sampleQuality (ignored for Texture3D)
        );
    tr = CTiledResourcesTest::CreateTexture(&desc, &pNormalTexture);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Create a similar control texture for comparing rendered results to
    tr = CTiledResourcesTest::CreateTexture(&desc, &pControlTexture);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    //
    // Create unordered access view for normal resource
    //
    CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    CResourceBaseTestHelper<resourceBase>::GenerateUnorderedAccessViewDesc(
        uavDesc,
        pNormalTexture,
        (TRB_TEXTURE_2D == resourceBase ? D3D11_UAV_DIMENSION_TEXTURE2D : D3D11_UAV_DIMENSION_TEXTURE3D),
        DXGI_FORMAT_R32_UINT
        );

    hr = pDevice->CreateUnorderedAccessView(pNormalTexture, &uavDesc, &pNormalView);
    if (FAILED(hr))
    {
        return RESULT_FAIL;
    }

    //
    // Create unordered access view for tiled resource
    //
    hr = pDevice->CreateUnorderedAccessView(m_pTiledResource, &uavDesc, &pTiledView);
    if (FAILED(hr))
    {
        return RESULT_FAIL;
    }

    //
    // Create unordered access view for control resource
    //
    hr = pDevice->CreateUnorderedAccessView(pControlTexture, &uavDesc, &pControlView);
    if (FAILED(hr))
    {
        return RESULT_FAIL;
    }

    if (RW_MODE_READ_FROM_TILED_RESOURCE == m_parameters.pReadWriteMode->GetValue())
    {
        // Create a untiled texture that will be a copy of the tiled texture when 
        // mode is RW_MODE_READ_FROM_TILED_RESOURCE
        tr = CTiledResourcesTest::CreateTexture(&desc, &pUntiledCopyTexture);
        if (RESULT_PASS != tr)
        {
            return tr;
        }

        //
        // Create unordered access view for control resource
        //
        hr = pDevice->CreateUnorderedAccessView(pUntiledCopyTexture, &uavDesc, &pUntiledCopyView);
        if (FAILED(hr))
        {
            return RESULT_FAIL;
        }
    }

    //
    // Initialize the tile mapping on the tiled depth texture
    //
    UINT startTiles = pTilePoolManager->FetchRange(m_pTiledResource, CResourceBaseTestHelper<resourceBase>::totalNumTiles);
    if (RESULT_PASS != CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(
        m_pTiledResource,
        startTiles,
        pTilePoolManager->GetTilePool(),
        CTiledResourceViewTestBase<resourceBase>::m_parameters.pResidencyPattern->GetValue(),
        tileMapping
        ))
    {
        return RESULT_FAIL;
    }

    //
    // Configure the compute pipeline
    //
    ID3D11UnorderedAccessView *uavListTiled[2];
    ID3D11UnorderedAccessView *uavListControl[2];
    ID3D11TextureType *pTextureIn = NULL;
    ID3D11TextureType *pTextureOut = NULL;

    // For Texture2D we only need a single slice below
    // but must initialize all 4 slices for Texture3D case
    const char *fullyMappedRows[4][4] = { { NULL }, { NULL }, { NULL }, { NULL } };
    if (TRB_TEXTURE_2D == resourceBase)
    {
        fullyMappedRows[0][0] = "0123";
        fullyMappedRows[0][1] = "4567";
        fullyMappedRows[0][2] = "89ab";
        fullyMappedRows[0][3] = "cdef";
    }
    else
    {
        fullyMappedRows[0][0] = "ABCD";
        fullyMappedRows[0][1] = "EFGH";
        fullyMappedRows[0][2] = "IJKL";
        fullyMappedRows[0][3] = "MNOP";

        fullyMappedRows[1][0] = "QRST";
        fullyMappedRows[1][1] = "UVWX";
        fullyMappedRows[1][2] = "YZab";
        fullyMappedRows[1][3] = "cdef";

        fullyMappedRows[2][0] = "ghij";
        fullyMappedRows[2][1] = "klmn";
        fullyMappedRows[2][2] = "opqr";
        fullyMappedRows[2][3] = "stuv";

        fullyMappedRows[3][0] = "wxyz";
        fullyMappedRows[3][1] = "0123";
        fullyMappedRows[3][2] = "4567";
        fullyMappedRows[3][3] = "89+/";
    }

    switch (m_parameters.pReadWriteMode->GetValue())
    {
    case RW_MODE_READ_FROM_TILED_RESOURCE:
        // Copy the temp texture to the tiled texture
        pContext->TiledResourceBarrier(nullptr, m_pTiledResource);
        pContext->CopySubresourceRegion(m_pTiledResource, 0, 0, 0, 0, pTempTexture, 0, nullptr);

        // Clear the blank/null rects in the tiled texture
        CTiledResourcesTest::ClearBlankRegions(tileMapping, pTiledView, DXGI_FORMAT_R32_UINT);

        // Copy the tiled resource back to the untiled copy texture to
        // match blank/null regions
        pContext->CopySubresourceRegion(pUntiledCopyTexture, 0, 0, 0, 0, m_pTiledResource, 0, nullptr);

        // Update view lists for compute shader
        uavListTiled[0] = pTiledView;
        uavListTiled[1] = pNormalView;
        uavListControl[0] = pUntiledCopyView;
        uavListControl[1] = pControlView;

        pTextureIn = m_pTiledResource;
        pTextureOut = pNormalTexture;

        // Since the output will be untiled modify the tile
        // mapping to appear fully resident
        CTiledResourceViewTestBase<resourceBase>::SetTileMappingFromStrings(tileMapping, fullyMappedRows);
        break;

    case RW_MODE_WRITE_TO_TILED_RESOURCE:
        // Copy the temp texture to the tiled texture
        pContext->CopySubresourceRegion(pNormalTexture, 0, 0, 0, 0, pTempTexture, 0, nullptr);

        // Update view lists for compute shader
        uavListTiled[0] = pNormalView;
        uavListTiled[1] = pTiledView;
        uavListControl[0] = pNormalView;
        uavListControl[1] = pControlView;

        pTextureIn = pNormalTexture;
        pTextureOut = m_pTiledResource;
        pContext->TiledResourceBarrier(nullptr, pTiledView);
        break;
    }

    //
    // Set up correct dispatch size based on Texture2D or Texture3D
    //
    const UINT dispatchNumX = (TRB_TEXTURE_2D == resourceBase ? 64 : 64);
    const UINT dispatchNumY = (TRB_TEXTURE_2D == resourceBase ? 64 : 64);
    const UINT dispatchNumZ = (TRB_TEXTURE_2D == resourceBase ? 1 : 64);

    //
    // Draw test image
    //
    pContext->CSSetUnorderedAccessViews(0, 2, uavListTiled, nullptr);
    if (CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue() > 1)
    {
        pContext->CSSetShader(m_pCSTiled, nullptr, 0);
    }
    else
    {
        pContext->CSSetShader(m_pCSNormal, nullptr, 0);
    }
    pContext->Dispatch(dispatchNumX, dispatchNumY, dispatchNumZ);

    CTiledResourcesTest::ClearBlankRegions(tileMapping, pTiledView, DXGI_FORMAT_R32_UINT);

    //
    // Draw control image
    //
    pContext->CSSetUnorderedAccessViews(0, 2, uavListControl, nullptr);
    pContext->CSSetShader(m_pCSNormal, nullptr, 0);
    pContext->Dispatch(dispatchNumX, dispatchNumY, dispatchNumZ);

    if (RW_MODE_WRITE_TO_TILED_RESOURCE == m_parameters.pReadWriteMode->GetValue())
    {
        CTiledResourcesTest::ClearBlankRegions(tileMapping, pControlView, DXGI_FORMAT_R8G8B8A8_UNORM);
    }

    //
    // Display the results
    //
    {
        const UINT numSlices = (TRB_TEXTURE_2D == resourceBase ? 1 : depth);
        UINT nameIndex = 0;
        wchar_t combinedName[3 * numSlices][64];
        for (UINT slice = 0; slice < numSlices; ++slice)
        {
            wchar_t sliceName[32];
            auto GenerateName = [&](const wchar_t *prependName, const UINT inSlice) -> const wchar_t*
            {
                swprintf_s(sliceName, ARRAYSIZE(sliceName), L" (Slice %d)", inSlice);
                swprintf_s(combinedName[nameIndex], ARRAYSIZE(combinedName[nameIndex]), L"%s%s", prependName, (numSlices == 1 ? L"" : sliceName));
                return combinedName[nameIndex++];
            };
            ShowResource(GenerateName(L"Input Texture", slice), pTextureIn, 0, 0, slice, 1.0f, DXGI_FORMAT_R8G8B8A8_UNORM);
            ShowResource(GenerateName(L"Output Texture", slice), pTextureOut, 0, 0, slice, 1.0f, DXGI_FORMAT_R8G8B8A8_UNORM);
            ShowResource(GenerateName(L"Expected Output Texture", slice), pControlTexture, 0, 0, slice, 1.0f, DXGI_FORMAT_R8G8B8A8_UNORM);
        }
    }

    //
    // Compare results against control
    //
    tr = CTiledResourcesTest::CompareTiledResourceToReference(pTextureOut, pControlTexture, 0, 0, tileMapping);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    return tr;
}
