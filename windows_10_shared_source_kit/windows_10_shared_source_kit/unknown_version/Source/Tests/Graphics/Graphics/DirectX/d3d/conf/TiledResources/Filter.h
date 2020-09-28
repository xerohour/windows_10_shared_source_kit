////////////////////////////////////////////////////////////////
// Filter.h
////////////////////////////////////////////////////////////////

#pragma once

#include "D3D11Test.h"
#include "TiledResources.h"

using namespace WindowsTest;

enum FILTER_MODE
{
    FMODE_SAMPLE,
    FMODE_GATHER
};

struct FILTER_SHADER_SETTINGS
{
    UINT useClamp;
    UINT useFeedback;
};


//Base class for Filter Test. Using Texture2D and Texture3D are derived
template <TILED_RESOURCE_BASE resourceBase>
class CFilterTestBase : public CResourceBaseTestHelper<resourceBase>
{
public:
    CFilterTestBase() :
        m_pFilterNormalTier1PS(nullptr),
        m_pFilterTiledTier1PS(nullptr),
        m_pFilterNormalTier2PS(nullptr),
        m_pFilterTiledTier2PS(nullptr),
        m_pGatherNormalTier1PS(nullptr),
        m_pGatherTiledTier1PS(nullptr),
        m_pGatherNormalTier2PS(nullptr),
        m_pGatherTiledTier2PS(nullptr),
        m_pStagedTexture(nullptr),
        m_pRasterizerState(nullptr)
    {
    }

    void Cleanup()
    {
        SAFE_RELEASE(m_pFilterNormalTier1PS);
        SAFE_RELEASE(m_pFilterTiledTier1PS);
        SAFE_RELEASE(m_pFilterNormalTier2PS);
        SAFE_RELEASE(m_pFilterTiledTier2PS);
        SAFE_RELEASE(m_pGatherNormalTier1PS);
        SAFE_RELEASE(m_pGatherTiledTier1PS);
        SAFE_RELEASE(m_pGatherNormalTier2PS);
        SAFE_RELEASE(m_pGatherTiledTier2PS);
        SAFE_RELEASE(m_pStagedTexture);
        SAFE_RELEASE(m_pRasterizerState);

        CTiledResourcesTest::Cleanup();
    }

    void InitTestParameters();
    TEST_RESULT Setup();
    TEST_RESULT ExecuteTestCase();

protected:
    // Helpers to clean up typenames
    CRESOURCE_BASE_TEST_HELPER__TYPENAME_TYPEDEFS

        ID3D11PixelShader		*m_pFilterNormalTier1PS;
    ID3D11PixelShader		*m_pFilterTiledTier1PS;
    ID3D11PixelShader		*m_pFilterNormalTier2PS;    // Texture2D and Texture3D share this shader member
    ID3D11PixelShader		*m_pFilterTiledTier2PS;     // Texture2D and Texture3D share this shader member
    ID3D11PixelShader		*m_pGatherNormalTier1PS;
    ID3D11PixelShader		*m_pGatherTiledTier1PS;
    ID3D11PixelShader		*m_pGatherNormalTier2PS;
    ID3D11PixelShader		*m_pGatherTiledTier2PS;
    ID3D11TextureType		*m_pStagedTexture;
    ID3D11RasterizerState	*m_pRasterizerState;

    // Parameters
    struct
    {
        CVariationParameter<RESIDENCY_PATTERN_2D_3D> *pResidencyPattern;
        CVariationParameter<float> *pTargetWidthScale;
        CVariationParameter<float> *pTargetHeightScale;
        CVariationParameter<float> *pTargetDepthScale;  // only used by Texture3D
        CVariationParameter<bool> *pUseClamp;
        CVariationParameter<bool> *pUseFeedback;
        CVariationParameter<MINMAX_FILTER> *pMinMaxFilter;
        CVariationParameter<FILTER_MODE> *pMode;
    } m_parameters;
};

DECLARE_NAMED_VALUES(FILTER_MODE);


// Actual tests
class CFilterTestTex2D : public CFilterTestBase < TRB_TEXTURE_2D > { };
class CFilterTestTex3D : public CFilterTestBase < TRB_TEXTURE_3D > { };



// Implementations
template <TILED_RESOURCE_BASE resourceBase>
void CFilterTestBase<resourceBase>::InitTestParameters()
{
    CreateParameter(_T("TiledResourcesTier"), &CTiledResourcesTest::m_parameters.pTiledResourcesTier);
    CreateParameter(_T("ResidencyPattern"), &m_parameters.pResidencyPattern);
    CreateParameter(_T("TargetWidthScale"), &m_parameters.pTargetWidthScale);
    CreateParameter(_T("TargetHeightScale"), &m_parameters.pTargetHeightScale);
    CreateParameter(_T("TargetDepthScale"), &m_parameters.pTargetDepthScale);
    CreateParameter(_T("UseClamp"), &m_parameters.pUseClamp);
    CreateParameter(_T("UseFeedback"), &m_parameters.pUseFeedback);
    CreateParameter(_T("MinMaxFilter"), &m_parameters.pMinMaxFilter);
    CreateParameter(_T("Mode"), &m_parameters.pMode);

    testfactor::RFactor tiledResourcesTier1 = AddParameterValue<UINT>(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 1U);
    testfactor::RFactor tiledResourcesTier2 = AddParameterValue<UINT>(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 2U);
    testfactor::RFactor tiledResourcesTier3 = AddParameterValue<UINT>(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 3U);

    CUserValueSet<RESIDENCY_PATTERN_2D_3D> *pResidencyPatternValues = new CUserValueSet<RESIDENCY_PATTERN_2D_3D>();
    pResidencyPatternValues->AddValue(RP2D_3D_FULL);
    pResidencyPatternValues->AddValue(RP2D_3D_FRAGMENTED_MULTI_TILE);
    pResidencyPatternValues->AddValue(RP2D_3D_FRAGMENTED_MIP_1);
    pResidencyPatternValues->AddValue(RP2D_3D_UNIFORM_BOX);

    testfactor::RFactor residencyPattern = AddParameterValueSet<RESIDENCY_PATTERN_2D_3D>(
        m_parameters.pResidencyPattern,
        pResidencyPatternValues
        );

    CUserValueSet<float> *pScaleValues = new CUserValueSet<float>();
    pScaleValues->AddValue(1.0f);
    pScaleValues->AddValue(0.75f);
    pScaleValues->AddValue(1.5f);

    CUserValueSet<MINMAX_FILTER> *pMinMaxFilters = new CUserValueSet<MINMAX_FILTER>();
    pMinMaxFilters->AddValue(MMF_MINIMUM_MIN_MAG_MIP_LINEAR);
    pMinMaxFilters->AddValue(MMF_MAXIMUM_MIN_MAG_MIP_LINEAR);

    testfactor::RFactor targetWidthScale = AddParameterValueSet<float>(
        m_parameters.pTargetWidthScale,
        pScaleValues
        );

    testfactor::RFactor targetHeightScale = AddParameterValueSet<float>(
        m_parameters.pTargetHeightScale,
        pScaleValues
        );

    testfactor::RFactor targetDepthScale = AddParameterValueSet<float>(
        m_parameters.pTargetDepthScale,
        pScaleValues
        );

    CBoolValueSet *pYesNoValues = new CBoolValueSet();
    testfactor::RFactor useClamp = AddParameterValueSet<bool>(
        m_parameters.pUseClamp,
        pYesNoValues
        );
    testfactor::RFactor useFeedback = AddParameterValueSet<bool>(
        m_parameters.pUseFeedback,
        pYesNoValues
        );
    testfactor::RFactor minMaxFilter = AddParameterValueSet<MINMAX_FILTER>(
        m_parameters.pMinMaxFilter,
        pMinMaxFilters
        );

    testfactor::RFactor modeSample = AddParameterValue(m_parameters.pMode, FMODE_SAMPLE);
    testfactor::RFactor modeGather = AddParameterValue(m_parameters.pMode, FMODE_GATHER);
    testfactor::RFactor root;

    // Conditional factors depending on Texture2D vs Texture3D
    // When using Texture2D, skip building factors for Tier 3 (Tier 2 tests correct functionality)
    // WHen using Texture3D, skip building factors for Tiers 1 and 2
    if (TRB_TEXTURE_2D == resourceBase)
    {
        testfactor::RFactor tier1Factors = tiledResourcesTier1 * (modeSample + modeGather);
        testfactor::RFactor tier2Factors = tiledResourcesTier2 * (modeSample * useClamp + modeGather) * useFeedback + tiledResourcesTier2 * minMaxFilter * modeSample;
        root = (tier1Factors + tier2Factors) * residencyPattern * targetWidthScale * targetHeightScale;
    }
    else
    {
        testfactor::RFactor tier3Factors = tiledResourcesTier3 * (modeSample * useClamp) * useFeedback + tiledResourcesTier3 * minMaxFilter * modeSample;
        root = tier3Factors * residencyPattern * targetWidthScale * targetHeightScale * targetDepthScale;
    }

    SetRootTestFactor(root);
}


template <TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CFilterTestBase<resourceBase>::Setup()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    ID3D11Device2 *pDevice = GetDevice2();
    CHandle<ID3D11TextureType> pStagingTexture;
    CHandle<ID3D11RasterizerState> pRS;

    tr = CTiledResourcesTest::Setup();
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // See if the device supports MinMaxFiltering
    D3D11_FEATURE_DATA_D3D11_OPTIONS1 optionsData;
    hr = pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &optionsData, sizeof(optionsData));
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CheckFeatureSupport);
        return RESULT_FAIL;
    }

    if (FALSE == optionsData.MinMaxFiltering)
    {
        // Hardware doesn't support minmax filtering so skip those tests
        CFilterHandle noMinMax = FilterActive<MINMAX_FILTER>(m_parameters.pMinMaxFilter);
        SkipMatching(noMinMax);
    }

    // Width and Height are both 4 tiles
    const UINT width = 4 * CResourceBaseTestHelper<resourceBase>::widthPerTile;
    const UINT height = 4 * CResourceBaseTestHelper<resourceBase>::heightPerTile;
    const UINT depth = 4 * CResourceBaseTestHelper<resourceBase>::depthPerTile;     // depth will be ignored by Texture2D methods

    // Helper for vertex shader creation
    auto CreateVertexShader = [&](ID3D11VertexShader **ppShader, const BYTE *pShaderByteCode, const SIZE_T shaderLength)
    {
        if (NULL == *ppShader)
        {
            HRESULT hr = pDevice->CreateVertexShader(pShaderByteCode, shaderLength, NULL, ppShader);
            if (FAILED(hr))
            {
                REPORT_HRESULT_FAILURE(hr, CreateVertexShader);
                return RESULT_FAIL;
            }
        }
        return RESULT_PASS;
    };

    // Helper for pixel shader creation
    auto CreatePixelShader = [&](ID3D11PixelShader **ppShader, const BYTE *pShaderByteCode, const SIZE_T shaderLength)
    {
        if (NULL == *ppShader)
        {
            hr = pDevice->CreatePixelShader(pShaderByteCode, shaderLength, NULL, ppShader);
            if (FAILED(hr))
            {
                REPORT_HRESULT_FAILURE(hr, CreatePixelShader);
                return RESULT_FAIL;
            }
        }
        return RESULT_PASS;
    };

    // Helper to load correct pixel shaders into member variables
    auto LoadAndAssignPixelShaders = [&](ID3D11PixelShader **shaderHandles[], const UINT numShaderHandles, LOADABLE_SHADER_BYTECODE_TYPE shaderTypes[], const UINT numShaderTypes)
    {
        UINT shaderIndex = 0;
        for (UINT shaderType = 0; shaderType < numShaderTypes; ++shaderType)
        {
            for (UINT normalOrTiled = 0; normalOrTiled < 2 && shaderIndex < numShaderHandles; ++normalOrTiled, ++shaderIndex)
            {
                const BYTE *pShaderByteCode;
                SIZE_T shaderLen;
                CTiledResourcesTest::GetShaderByteCode(shaderTypes[shaderType], &pShaderByteCode, &shaderLen, (normalOrTiled == 1), (TRB_TEXTURE_3D == resourceBase));
                if (RESULT_PASS != CreatePixelShader(shaderHandles[shaderIndex], pShaderByteCode, shaderLen))
                {
                    return RESULT_FAIL;
                }
            }
        }
        return RESULT_PASS;
    };


    // Get correct vertex shader byte codes based on Texture2D vs Texture3D
    const BYTE *pShaderByteCodeVertex;
    SIZE_T shaderLenVertex;
    CTiledResourcesTest::GetShaderByteCode(LSBT_VERTEX_COPY_TEX, &pShaderByteCodeVertex, &shaderLenVertex, false, (TRB_TEXTURE_3D == resourceBase));


    // Load pixel shaders necessary for Texture2D only
    if (TRB_TEXTURE_2D == resourceBase)
    {
        if (RESULT_PASS != CreateVertexShader(&m_pCopyTexVS, pShaderByteCodeVertex, shaderLenVertex))
        {
            return RESULT_FAIL;
        }

        LOADABLE_SHADER_BYTECODE_TYPE shaderTypes[] = { LSBT_PIXEL_FILTER_TIER_1, LSBT_PIXEL_GATHER_TIER_1 };
        ID3D11PixelShader **shaderHandles[] = {
            &m_pFilterNormalTier1PS,
            &m_pFilterTiledTier1PS,
            &m_pGatherNormalTier1PS,
            &m_pGatherTiledTier1PS
        };

        if (RESULT_PASS != LoadAndAssignPixelShaders(shaderHandles, sizeof(shaderHandles) / sizeof(ID3D11PixelShader**), shaderTypes, sizeof(shaderTypes) / sizeof(LOADABLE_SHADER_BYTECODE_TYPE)))
        {
            return RESULT_FAIL;
        }

        if (1 < m_maxSupportedTiledResourcesTier)
        {
            LOADABLE_SHADER_BYTECODE_TYPE shaderTypes[] = { LSBT_PIXEL_FILTER_TIER_2, LSBT_PIXEL_GATHER_TIER_2 };
            ID3D11PixelShader **shaderHandles[] = {
                &m_pFilterNormalTier2PS,
                &m_pFilterTiledTier2PS,
                &m_pGatherNormalTier2PS,
                &m_pGatherTiledTier2PS
            };

            if (RESULT_PASS != LoadAndAssignPixelShaders(shaderHandles, sizeof(shaderHandles) / sizeof(ID3D11PixelShader**), shaderTypes, sizeof(shaderTypes) / sizeof(LOADABLE_SHADER_BYTECODE_TYPE)))
            {
                return RESULT_FAIL;
            }
        }
    }
    // shaders for Texture3D only
    else
    {
        if (m_maxSupportedTiledResourcesTier < 3)
        {
            WriteToLog(_T("Skipping Texture3D test since Tiled Resources Tier does not support Tiled Texture3D."));
            return RESULT_SKIP;
        }
        if (RESULT_PASS != CreateVertexShader(&m_pCopyTexVSTex3D, pShaderByteCodeVertex, shaderLenVertex))
        {
            return RESULT_FAIL;
        }

        // Create Geometry Shader for Texture 3D copies (allows setting of RTArrayIndex, i.e. depth slice)
        const BYTE *pShaderByteCodeGeometry;
        SIZE_T shaderLenGeometry;
        assert(resourceBase == TRB_TEXTURE_3D); // GetShaderByteCode for Geometry shader assumes 3D
        CTiledResourcesTest::GetShaderByteCode(LSBT_GEOMETRY_COPY_TEX_3D, &pShaderByteCodeGeometry, &shaderLenGeometry, false, true);
        if (pShaderByteCodeGeometry == nullptr)
        {
            return RESULT_FAIL;
        }
        HRESULT hr = pDevice->CreateGeometryShader(pShaderByteCodeGeometry, shaderLenGeometry, NULL, &m_pCopyTexGSTex3D);
        if (FAILED(hr))
        {
            REPORT_HRESULT_FAILURE(hr, CreateGeometryShader);
            return RESULT_FAIL;
        }

        const BYTE *pShaderByteCodeCopy;
        SIZE_T shaderLenCopy;
        CTiledResourcesTest::GetShaderByteCode(LSBT_PIXEL_COPY_TEX_TIER_2, &pShaderByteCodeCopy, &shaderLenCopy, false, true);
        pDevice->CreatePixelShader(pShaderByteCodeCopy, shaderLenCopy, NULL, &m_pCopyTexTier2PSTex3D);

        LOADABLE_SHADER_BYTECODE_TYPE shaderTypes[] = { LSBT_PIXEL_FILTER_TIER_2 };//, LSBT_PIXEL_GATHER_TIER_2 };
        ID3D11PixelShader **shaderHandles[] = {
            &m_pFilterNormalTier2PS,
            &m_pFilterTiledTier2PS
            //            &m_pGatherNormalTier2PS,
            //            &m_pGatherTiledTier2PS
        };

        if (RESULT_PASS != LoadAndAssignPixelShaders(shaderHandles, sizeof(shaderHandles) / sizeof(ID3D11PixelShader**), shaderTypes, sizeof(shaderTypes) / sizeof(LOADABLE_SHADER_BYTECODE_TYPE)))
        {
            return RESULT_FAIL;
        }
    }

    //
    // Staging texture
    //

    tr = CResourceBaseTestHelper<resourceBase>::CreateStagingTexture(width, height, depth, &pStagingTexture, true);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    //
    // Create the rasterizer state
    //
    D3D11_RASTERIZER_DESC rsDesc;
    rsDesc.AntialiasedLineEnable = FALSE;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.DepthBias = 0;
    rsDesc.DepthBiasClamp = 0.0f;
    rsDesc.DepthClipEnable = TRUE;
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.FrontCounterClockwise = TRUE;
    rsDesc.MultisampleEnable = FALSE;
    rsDesc.ScissorEnable = FALSE;
    rsDesc.SlopeScaledDepthBias = 0;
    hr = pDevice->CreateRasterizerState(&rsDesc, &pRS);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRasterizerState);
        return RESULT_FAIL;
    }

    // Set member pointers
    m_pStagedTexture = pStagingTexture;
    m_pRasterizerState = pRS;

    // Detach smart pointers
    pStagingTexture.Detach();
    pRS.Detach();

    return tr;
}


template <TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CFilterTestBase<resourceBase>::ExecuteTestCase()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    CHandle<ID3D11TextureType> pTempTexture;
    CHandle<ID3D11TextureType> pTiledTexture;
    CHandle<ID3D11TextureType> pNormalTexture;
    CHandle<ID3D11TextureType> pNormalRT;
    CHandle<ID3D11TextureType> pTiledRT;
    CHandle<ID3D11SamplerState> pSamplerState;
    CHandle<ID3D11ShaderResourceView> pTiledSRV;
    CHandle<ID3D11ShaderResourceView> pNormalSRV;
    CHandle<ID3D11RenderTargetView> pNormalRTV;
    CHandle<ID3D11RenderTargetView> pTiledRTV;
    CHandle<ID3D11RenderTargetView> pTempRTV;
    CHandle<ID3D11Buffer> pSettingsBufferNormal;
    CHandle<ID3D11Buffer> pSettingsBufferTiled;
    CHandle<ID3D11Buffer> pConstVertexBuffer;
    TileMappingType tileMapping;

    // Width and Height are both 4 tiles
    const UINT width = 4 * CResourceBaseTestHelper<resourceBase>::widthPerTile;
    const UINT height = 4 * CResourceBaseTestHelper<resourceBase>::heightPerTile;
    const UINT depth = 4 * CResourceBaseTestHelper<resourceBase>::depthPerTile;

    //
    // Constant buffer for shader settings
    //

    // Normal shader execution
    FILTER_SHADER_SETTINGS shaderSettings;
    if (CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue() >= 2)
    {
        shaderSettings.useClamp = (FMODE_SAMPLE == m_parameters.pMode->GetValue() ? (m_parameters.pUseClamp->IsActive() ? m_parameters.pUseClamp->GetValue() : FALSE) : FALSE);
        shaderSettings.useFeedback = (m_parameters.pUseFeedback->IsActive() ? m_parameters.pUseFeedback->GetValue() : FALSE);
    }
    else
    {
        shaderSettings.useClamp = FALSE;
        shaderSettings.useFeedback = FALSE;
    }


    CD3D11_BUFFER_DESC cbDesc(
        16 * ((sizeof(shaderSettings) + 15) / 16), // Align to 16 bytes,
        D3D11_BIND_CONSTANT_BUFFER
        );

    D3D11_SUBRESOURCE_DATA cbInit;
    cbInit.pSysMem = &shaderSettings;
    cbInit.SysMemPitch = cbDesc.ByteWidth;
    cbInit.SysMemSlicePitch = 0;
    hr = pDevice->CreateBuffer(&cbDesc, &cbInit, &pSettingsBufferNormal);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateBuffer);
        return RESULT_FAIL;
    }

    // Tiled shader execution
    hr = pDevice->CreateBuffer(&cbDesc, &cbInit, &pSettingsBufferTiled);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateBuffer);
        return RESULT_FAIL;
    }

    //
    // Create both a tiled texture and a "duplicate" non-tiled texture to sample from
    //

    // Normal texture
    CD3D11_TEXTURE_TYPE_DESC texDesc;
    CResourceBaseTestHelper<resourceBase>::GenerateTextureDesc(
        texDesc,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        width,
        height,
        (TRB_TEXTURE_2D == resourceBase ? FIXED_ARRAY_SIZE : depth),
        0,
        D3D11_BIND_SHADER_RESOURCE
        );

    // Normal texture
    tr = CreateTexture(&texDesc, &pNormalTexture);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Tiled texture
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_TILED;
    tr = CreateTexture(&texDesc, &pTiledTexture);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Temp texture
    texDesc.MiscFlags = 0;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
    if (TRB_TEXTURE_2D == resourceBase)
    {
        // Cast is to please the compiler, but this should never be called for Texture3D
        ((CD3D11_TEXTURE2D_DESC*)(&texDesc))->ArraySize = FIXED_ARRAY_SIZE;
    }

    //	tr = CreateTex2D(&texDesc, &pTempTexture);
    tr = CreateTexture(&texDesc, &pTempTexture);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Set tile mappings on the tiled resource
    UINT maxTiles;
    pDevice->GetResourceTiling(pTiledTexture, &maxTiles, nullptr, nullptr, nullptr, 0, nullptr);
    UINT rangeStart = GetFramework()->GetTilePoolManager()->FetchRange(pTiledTexture, maxTiles);

    // Initialize all tier-1 tile mappings to blank tile
    if (1 == CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue())
    {
        // Set the entire resource to the shared NULL tile
        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledTexture, 1, GetFramework()->GetTilePoolManager()->GetTilePool(), RP2D_3D_CLEAR_ALL_MIPS, tileMapping);
        if (RESULT_PASS != tr)
        {
            return tr;
        }
    }

    tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(
        pTiledTexture,
        rangeStart,
        GetFramework()->GetTilePoolManager()->GetTilePool(),
        m_parameters.pResidencyPattern->GetValue(),
        tileMapping
        );
    if (RESULT_PASS != tr)
    {
        return tr;
    }


    //
    // Shader resource views
    //

    CD3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
    CResourceBaseTestHelper<resourceBase>::GenerateShaderResourceViewDesc(
        viewDesc,
        pTiledTexture,
        D3D11_SRV_DIMENSION_TEXTURE2D,  // ignored for Texture3D
        DXGI_FORMAT_R8G8B8A8_UNORM
        );

    hr = pDevice->CreateShaderResourceView(pTiledTexture, &viewDesc, &pTiledSRV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateShaderResourceView);
        return RESULT_FAIL;
    }

    hr = pDevice->CreateShaderResourceView(pNormalTexture, &viewDesc, &pNormalSRV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateShaderResourceView);
        return RESULT_FAIL;
    }


    //
    // Initialize the data
    //

    // Create a temporary RT texture to have clear rects where unmapped
    CD3D11_RENDER_TARGET_VIEW_DESC tempRTVDesc;
    CResourceBaseTestHelper<resourceBase>::GenerateRenderTargetViewDesc(
        tempRTVDesc,
        pTempTexture,
        D3D11_RTV_DIMENSION_TEXTURE2D,  // ignored for Texture3D
        DXGI_FORMAT_R8G8B8A8_UNORM,
        0,
        0,
        resourceBase == TRB_TEXTURE_2D ? 1 : -1 // RTV for all 3D slices
        );

    hr = pDevice->CreateRenderTargetView(pTempTexture, &tempRTVDesc, &pTempRTV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
        return RESULT_FAIL;
    }

    pContext->CopyResource(pTempTexture, m_pStagedTexture);
    pContext->Flush();

    pContext->TiledResourceBarrier(nullptr, pTiledTexture);

    if (RP2D_3D_FULL_MIP_1 == m_parameters.pResidencyPattern->GetValue() ||
        RP2D_3D_FRAGMENTED_MIP_1 == m_parameters.pResidencyPattern->GetValue())
    {
        // Clear the most detailed mip completely since only the second mip
        // level is resident in the tiled resource
        const float black[4] = { 0 };
        CResourceBaseTestHelper<resourceBase>::ClearView(pTempRTV, DXGI_FORMAT_R8G8B8A8_UNORM, black, nullptr, 0);
        pContext->Flush();

        // Release the previous temporary RTV
        pTempRTV->Release();

        // Create an RTV of the second mip level
        if (TRB_TEXTURE_2D == resourceBase)
        {
            tempRTVDesc.Texture2D.MipSlice = 1;
        }
        else
        {
            tempRTVDesc.Texture3D.MipSlice = 1;
            tempRTVDesc.Texture3D.FirstWSlice = 0;
            tempRTVDesc.Texture3D.WSize = -1;
        }
        hr = pDevice->CreateRenderTargetView(pTempTexture, &tempRTVDesc, &pTempRTV);
        if (FAILED(hr))
        {
            REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
            return RESULT_FAIL;
        }

        // Clear the rects representing blank/null tiles in the second mip level
        __if_exists(CD3D11_TEXTURE_TYPE_DESC::Depth)
        {
            ClearBlankRegions(tileMapping, pTempRTV, DXGI_FORMAT_R8G8B8A8_UNORM, 1 /*mip*/);
        }
        __if_not_exists(CD3D11_TEXTURE_TYPE_DESC::Depth)
        {
            ClearBlankRegions(tileMapping, pTempRTV, DXGI_FORMAT_R8G8B8A8_UNORM);
        }
        pContext->Flush();

        pContext->CopySubresourceRegion(pTiledTexture, 0, 0, 0, 0, pTempTexture, 0, nullptr);
        pContext->TiledResourceBarrier(pTiledTexture, pTiledTexture);
        pContext->CopySubresourceRegion(pTiledTexture, 1, 0, 0, 0, pTempTexture, 1, nullptr);

        pContext->CopySubresourceRegion(pNormalTexture, 0, 0, 0, 0, pTempTexture, 0, nullptr);
        pContext->CopySubresourceRegion(pNormalTexture, 1, 0, 0, 0, pTempTexture, 1, nullptr);
        pContext->Flush();
    }
    else
    {
        // Clear the rects representing blank/null tiles
        ClearBlankRegions(tileMapping, pTempRTV, DXGI_FORMAT_R8G8B8A8_UNORM);
        pContext->Flush();

        // Make the second mip level fully resident
        TileMappingType tempTileMapping;
        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(
            pTiledTexture,
            rangeStart + CResourceBaseTestHelper<resourceBase>::totalNumTiles,
            GetFramework()->GetTilePoolManager()->GetTilePool(),
            RP2D_3D_FULL_MIP_1,
            tempTileMapping
            );
        pContext->Flush();
        if (RESULT_PASS != tr)
        {
            return tr;
        }

        pContext->CopySubresourceRegion(pTiledTexture, 0, 0, 0, 0, pTempTexture, 0, nullptr);
        pContext->CopySubresourceRegion(pTiledTexture, 1, 0, 0, 0, pTempTexture, 1, nullptr);

        pContext->CopySubresourceRegion(pNormalTexture, 0, 0, 0, 0, pTempTexture, 0, nullptr);
        pContext->CopySubresourceRegion(pNormalTexture, 1, 0, 0, 0, pTempTexture, 1, nullptr);

        pContext->Flush();
    }


    //
    // Create the render targets
    //

    const UINT rtWidth = static_cast<UINT>(width * m_parameters.pTargetWidthScale->GetValue());
    const UINT rtHeight = static_cast<UINT>(height * m_parameters.pTargetHeightScale->GetValue());
    const UINT rtDepth = (TRB_TEXTURE_2D == resourceBase ? 1 : static_cast<UINT>(depth * m_parameters.pTargetDepthScale->GetValue()));
    CD3D11_TEXTURE_TYPE_DESC rtTexDesc;
    CResourceBaseTestHelper<resourceBase>::GenerateTextureDesc(
        rtTexDesc,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        rtWidth,
        rtHeight,
        (TRB_TEXTURE_2D == resourceBase ? 1 : rtDepth),
        0,
        D3D11_BIND_RENDER_TARGET
        );

    tr = CreateTexture(&rtTexDesc, &pNormalRT);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    tr = CreateTexture(&rtTexDesc, &pTiledRT);
    if (RESULT_PASS != tr)
    {
        return tr;
    }


    //
    // Create the sampler state
    // 

    D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    if (m_parameters.pMinMaxFilter->IsActive())
    {
        switch (m_parameters.pMinMaxFilter->GetValue())
        {
        default:
            assert(false);
            break;
        case MMF_MINIMUM_MIN_MAG_MIP_LINEAR:
            filter = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
            break;
        case MMF_MAXIMUM_MIN_MAG_MIP_LINEAR:
            filter = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
            break;
        }
    }

    CD3D11_SAMPLER_DESC sampDesc(
        filter,
        D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_WRAP,
        0.0f,
        0,
        D3D11_COMPARISON_ALWAYS,
        nullptr,
        0.0,
        D3D11_FLOAT32_MAX
        );

    hr = pDevice->CreateSamplerState(&sampDesc, &pSamplerState);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateSamplerState);
        return RESULT_FAIL;
    }


    //
    // Render target views
    //

    CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    CResourceBaseTestHelper<resourceBase>::GenerateRenderTargetViewDesc(
        rtvDesc,
        pNormalRT,
        D3D11_RTV_DIMENSION_TEXTURE2D,  // ignored for Texture3D
        DXGI_FORMAT_R8G8B8A8_UNORM
        );

    hr = pDevice->CreateRenderTargetView(pNormalRT, &rtvDesc, &pNormalRTV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
        return RESULT_FAIL;
    }

    hr = pDevice->CreateRenderTargetView(pTiledRT, &rtvDesc, &pTiledRTV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
        return RESULT_FAIL;
    }


    //
    // Setup pipeline
    //

    // Set the viewport
    D3D11_VIEWPORT viewport =
    {
        0.0f,						//	FLOAT TopLeftX;
        0.0f,						//	FLOAT TopLeftY;
        (float)rtWidth,			//	FLOAT Width;
        (float)rtHeight,			//	FLOAT Height;
        0.0f,						//	FLOAT MinDepth;
        1.0f,						//	FLOAT MaxDepth;
    };
    pContext->RSSetViewports(1, &viewport);

    // Create per-slice constant buffer
    struct cBufferType
    {
        // Position [0,1] of the quad to render relative to 3D volume
        // This is relevant only for Texture3D
        float quadSlicePosition;
        UINT numSlicesSRV;
        UINT numSlicesRTV;
    } cBufferData;
    cBufferData.quadSlicePosition = 0.f;
    cBufferData.numSlicesSRV = depth;
    cBufferData.numSlicesRTV = rtDepth; 
    D3D11_BUFFER_DESC cBufferDesc =
    {
        16 * ((sizeof(cBufferData) + 15) / 16), // ByteWidth, align to 16 bytes
        D3D11_USAGE_DEFAULT,                    // Usage
        D3D11_BIND_CONSTANT_BUFFER,             // BindFlags
        0,                                      // CPUAccessFlags
        0,                                      // MiscFlags
        0                                       // StructuredByteStride
    };
    D3D11_SUBRESOURCE_DATA cBufferInit =
    {
        &cBufferData,
        0,
        0
    };
    hr = pDevice->CreateBuffer(&cBufferDesc, &cBufferInit, &pConstVertexBuffer);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateBuffer);
        return RESULT_FAIL;
    }

    // Input assembler state
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    // Clear input layout as no input is needed since the 
    // vertex shader knows how to draw a quad using vertex ID's
    pContext->IASetInputLayout(nullptr);

    // Vertex shader state
    pContext->VSSetShader((TRB_TEXTURE_2D == resourceBase ? m_pCopyTexVS : m_pCopyTexVSTex3D), nullptr, 0);
    pContext->VSSetConstantBuffers(0, 1, &pConstVertexBuffer);

    // Geometry shader state
    assert(resourceBase == TRB_TEXTURE_2D || m_pCopyTexGSTex3D); // assert m_pCopyTexGSTex3D is created when operating on Tex3Ds
    pContext->GSSetShader((TRB_TEXTURE_3D == resourceBase ? m_pCopyTexGSTex3D : nullptr), nullptr, 0);

    // Rasterizer state
    pContext->RSSetState(m_pRasterizerState);

    // Pixel shader state
    pContext->PSSetSamplers(0, 1, &pSamplerState);


    //
    // Render test images
    //

    const UINT numSlices = (TRB_TEXTURE_2D == resourceBase ? 1 : depth); // use depth here (as opposed to rtDepth) to show input textures
    UINT nameIndex = 0;
    wchar_t combinedName[4 * numSlices][64];
    for (UINT slice = 0; slice < numSlices; ++slice)
    {
        wchar_t sliceName[32];
        auto GenerateName = [&](const wchar_t *prependName, const UINT mipLevel, const UINT inSlice) -> const wchar_t*
        {
            swprintf_s(sliceName, ARRAYSIZE(sliceName), L", Slice %d", inSlice);
            swprintf_s(combinedName[nameIndex], ARRAYSIZE(combinedName[nameIndex]), L"%s (Mip %d%s)", prependName, mipLevel, (numSlices == 1 ? L"" : sliceName));
            return combinedName[nameIndex++];
        };
        ShowResource(GenerateName(L"Normal input texture", 0, slice), pNormalTexture, 0, 0, slice);
        ShowResource(GenerateName(L"Tiled input texture", 0, slice), pTiledTexture, 0, 0, slice);
        ShowResource(GenerateName(L"Normal input texture", 1, slice), pNormalTexture, 1, 0, slice, 2.0f);
        ShowResource(GenerateName(L"Tiled input texture", 1, slice), pTiledTexture, 1, 0, slice, 2.0f);
    }

    // Helper to set correct pixel shader to null
    auto SetPixelShaderBySampleMode = [&](ID3D11PixelShader *pShaderFilter, ID3D11PixelShader *pShaderGather)
    {
        if (FMODE_SAMPLE == m_parameters.pMode->GetValue())
        {
            pContext->PSSetShader(pShaderFilter, nullptr, 0);
        }
        else
        {
            pContext->PSSetShader(pShaderGather, nullptr, 0);
        }
    };

    // Normal render
    if (1 == CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue())
    {
        SetPixelShaderBySampleMode(m_pFilterNormalTier1PS, m_pGatherNormalTier1PS);
    }
    else
    {
        SetPixelShaderBySampleMode(m_pFilterNormalTier2PS, m_pGatherNormalTier2PS);
    }

    pContext->PSSetConstantBuffers(0, 1, &pSettingsBufferNormal);
    pContext->PSSetShaderResources(0, 1, &pNormalSRV);
    pContext->OMSetRenderTargets(1, &pNormalRTV, nullptr);

    // Helper to draw our scene slice by slice
    auto DrawSlices = [&]()
    {
        // Draw screen-space quad once per necessary slice
        const float deltaSlice = (TRB_TEXTURE_2D == resourceBase ? 1.0f : 1.f/rtDepth);
        for (UINT slice = 0; slice < rtDepth; ++slice)
        {
            // Update constant buffer within vertex shader corresponding to the
            // z coordinate of vertex position to draw (this position is used
            // to look up within Texture3D directly)
            cBufferData.quadSlicePosition = deltaSlice * slice;
            pContext->UpdateSubresource(
                pConstVertexBuffer,
                0,
                nullptr,
                &cBufferData,
                0,
                0
                );

            pContext->Draw(4, 0);
        }
        pContext->Flush();
    };
    DrawSlices();


    // Tiled render
    if (1 == CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue())
    {
        SetPixelShaderBySampleMode(m_pFilterTiledTier1PS, m_pGatherTiledTier1PS);
    }
    else
    {
        SetPixelShaderBySampleMode(m_pFilterTiledTier2PS, m_pGatherTiledTier2PS);
    }

    pContext->PSSetConstantBuffers(0, 1, &pSettingsBufferTiled);
    pContext->PSSetShaderResources(0, 1, &pTiledSRV);
    pContext->OMSetRenderTargets(1, &pTiledRTV, nullptr);
    DrawSlices();

    pContext->TiledResourceBarrier(pTiledSRV, nullptr);

    {
        UINT nameIndex = 0;
        wchar_t combinedName[2 * numSlices][64];
        for (UINT slice = 0; slice < numSlices; ++slice)
        {
            wchar_t sliceName[32];
            auto GenerateName = [&](const wchar_t *prependName, const UINT inSlice) -> const wchar_t*
            {
                swprintf_s(sliceName, ARRAYSIZE(sliceName), L" (Slice %d)", inSlice);
                swprintf_s(combinedName[nameIndex], ARRAYSIZE(combinedName[nameIndex]), L"%s%s", prependName, (numSlices == 1 ? L"" : sliceName));
                return combinedName[nameIndex++];
            };
            ShowResource(GenerateName(L"Normal render results", slice), pNormalRT, 0, 0, slice);
            ShowResource(GenerateName(L"Tiled render results", slice), pTiledRT, 0, 0, slice);
        }
    }


    //
    // Compare rendering results
    //

    UINT fullData[64];              // Texture2D will use the first 16
    for (UINT i = 0; i < 64; ++i)
    {
        fullData[i] = i;
    }
    TileMappingType fullMapping;
    CResourceBaseTestHelper<resourceBase>::GenerateTileMapping(fullMapping, 4, 4, 4, fullData);
    tr = CTiledResourcesTest::CompareTiledResourceToReference(
        pTiledRT,
        pNormalRT,
        0,
        0,
        fullMapping,
        static_cast<UINT>(CResourceBaseTestHelper<resourceBase>::widthPerTile * m_parameters.pTargetWidthScale->GetValue()),
        static_cast<UINT>(CResourceBaseTestHelper<resourceBase>::heightPerTile * m_parameters.pTargetHeightScale->GetValue()),
        (TRB_TEXTURE_2D == resourceBase ? 1 : static_cast<UINT>(CResourceBaseTestHelper<resourceBase>::depthPerTile * m_parameters.pTargetDepthScale->GetValue()))
        );
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    pContext->Flush();

    return tr;
}
