////////////////////////////////////////////////////////////////////////////////
// UpdateTileMappings.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TiledResources.h"

// Base class for Update Tile Mappings Test. Using Texture2D and Texture3D are derived
template <TILED_RESOURCE_BASE resourceBase>
class CUpdateTileMappingsTestBase : public CResourceBaseTestHelper<resourceBase>
{
    // Helpers to clean up typenames
    CRESOURCE_BASE_TEST_HELPER__TYPENAME_TYPEDEFS

        // Parameters
    struct
    {
        CVariationParameter<UINT> *pPassCount;
        CVariationParameter<RESIDENCY_PATTERN_2D_3D> *pResidencyPatternPass1;
        CVariationParameter<RESIDENCY_PATTERN_2D_3D> *pResidencyPatternPass2;
        CVariationParameter<float>* pWidthMultiplier;
        CVariationParameter<float>* pHeightMultiplier;
        CVariationParameter<float>* pDepthMultiplier;       // used only for Texture3D
        CVariationParameter<bool>* pUseNoOverwrite;
        CVariationParameter<bool>* pSwapTilePoolOnPass2;
    } m_parameters;

    TileMappingType tileMapping;

public:
    void InitTestParameters();
    TEST_RESULT ExecuteTestCase();
};



// Actual tests
class CUpdateTileMappingsTestTex2D : public CUpdateTileMappingsTestBase < TRB_TEXTURE_2D > { };
class CUpdateTileMappingsTestTex3D : public CUpdateTileMappingsTestBase < TRB_TEXTURE_3D > { };




// Implementations
template <TILED_RESOURCE_BASE resourceBase>
void CUpdateTileMappingsTestBase<resourceBase>::InitTestParameters()
{
    CreateParameter(_T("ResidencyPatternPass1"), &m_parameters.pResidencyPatternPass1);
    CreateParameter(_T("ResidencyPatternPass2"), &m_parameters.pResidencyPatternPass2);
    CreateParameter(_T("SwapTilePoolOnPass2"), &m_parameters.pSwapTilePoolOnPass2);
    CreateParameter(_T("PassCount"), &m_parameters.pPassCount);
    CreateParameter(_T("UseNoOverwrite"), &m_parameters.pUseNoOverwrite);

    CUserValueSet<RESIDENCY_PATTERN_2D_3D> *pResidencyPatternValues = new CUserValueSet<RESIDENCY_PATTERN_2D_3D>();
    pResidencyPatternValues->AddValue(RP2D_3D_FULL);
    pResidencyPatternValues->AddValue(RP2D_3D_FULL_MULTI_REGION);
    pResidencyPatternValues->AddValue(RP2D_3D_FRAGMENTED_MULTI_TILE);
    pResidencyPatternValues->AddValue(RP2D_3D_FRAGMENTED_MULTI_REGION);
    pResidencyPatternValues->AddValue(RP2D_3D_FRAGMENTED_REUSE_SINGLE_TILE);
    pResidencyPatternValues->AddValue(RP2D_3D_FRAGMENTED_MIP_1);
    pResidencyPatternValues->AddValue(RP2D_3D_UNIFORM_BOX);

    testfactor::RFactor residencyPatternPass1 = AddParameterValueSet<RESIDENCY_PATTERN_2D_3D>(
        m_parameters.pResidencyPatternPass1,
        pResidencyPatternValues
        );

    pResidencyPatternValues->AddValue(RP2D_3D_FRAGMENTED_MULTI_TILE_SKIPS);

    testfactor::RFactor residencyPatternPass2 = AddParameterValueSet<RESIDENCY_PATTERN_2D_3D>(
        m_parameters.pResidencyPatternPass2,
        pResidencyPatternValues
        );

    testfactor::RFactor swapTilePoolOnPass2 = AddParameterValueSet<bool>(
        m_parameters.pSwapTilePoolOnPass2,
        new CBoolValueSet()
        );

    testfactor::RFactor passCount1 = AddParameterValue<UINT>(
        m_parameters.pPassCount,
        1
        );

    testfactor::RFactor passCount2 = AddParameterValue<UINT>(
        m_parameters.pPassCount,
        2
        );

    testfactor::RFactor useNoOverwrite = AddParameterValueSet(
        m_parameters.pUseNoOverwrite,
        new CBoolValueSet(true)
        );

    CreateParameter(_T("TiledResourcesTier"), &CTiledResourcesTest::m_parameters.pTiledResourcesTier);
    CTiledResourcesTest::m_parameters.pTiledResourcesTier->SetDefaultValue(1U);
    testfactor::RFactor tiledResourcesTier1 = AddParameterValue(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 1U);
    testfactor::RFactor tiledResourcesTier2 = AddParameterValue(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 2U);
    testfactor::RFactor tiledResourcesTier3 = AddParameterValue(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 3U);

    CUserValueSet<float>* pSizeMultipliers = new CUserValueSet<float>;
    pSizeMultipliers->AddValue(1.0f);
    pSizeMultipliers->AddValue(0.9f);

    CreateParameter(_T("WidthSizeMultiplier"), &m_parameters.pWidthMultiplier);
    m_parameters.pWidthMultiplier->SetDefaultValue(1.0f);
    testfactor::RFactor widthMultiplier = AddParameterValueSet<float>(m_parameters.pWidthMultiplier, pSizeMultipliers);

    CreateParameter(_T("HeightSizeMultiplier"), &m_parameters.pHeightMultiplier);
    m_parameters.pHeightMultiplier->SetDefaultValue(1.0f);
    testfactor::RFactor heightMultiplier = AddParameterValueSet<float>(m_parameters.pHeightMultiplier, pSizeMultipliers);

    testfactor::RFactor root;

    // Conditional factors depending on Texture2D vs Texture3D
    // When using Texture2D, skip building factors for Tier 3 (Tier 2 tests correct functionality)
    // WHen using Texture3D, skip building factors for Tiers 1 and 2
    if (TRB_TEXTURE_2D == resourceBase)
    {
        testfactor::RFactor pass1tier1 = passCount1 * useNoOverwrite * residencyPatternPass1 * tiledResourcesTier1;
        testfactor::RFactor pass2tier1 = passCount2 * useNoOverwrite * swapTilePoolOnPass2 * tiledResourcesTier1 * residencyPatternPass1 * residencyPatternPass2;

        testfactor::RFactor tier23pairwiseFactor = testfactor::NewPairwiseFactor(residencyPatternPass1, residencyPatternPass2, widthMultiplier, heightMultiplier);
        testfactor::RFactor pass1tier2 = passCount1 * useNoOverwrite * residencyPatternPass1 * tiledResourcesTier2 * widthMultiplier * heightMultiplier;
        testfactor::RFactor pass2tier2 = passCount2 * useNoOverwrite * swapTilePoolOnPass2 * tiledResourcesTier2 * tier23pairwiseFactor;

        root = pass1tier1 + pass2tier1 + pass1tier2 + pass2tier2;
    }
    else
    {
        CreateParameter(_T("DepthSizeMultiplier"), &m_parameters.pDepthMultiplier);
        m_parameters.pDepthMultiplier->SetDefaultValue(1.0f);
        testfactor::RFactor depthMultiplier = AddParameterValueSet<float>(m_parameters.pDepthMultiplier, pSizeMultipliers);

        testfactor::RFactor tier23pairwiseFactor = testfactor::NewPairwiseFactor(residencyPatternPass1, residencyPatternPass2, widthMultiplier, heightMultiplier) * depthMultiplier;
        testfactor::RFactor pass1tier3 = passCount1 * useNoOverwrite * residencyPatternPass1 * tiledResourcesTier3 * widthMultiplier * heightMultiplier * depthMultiplier;
        testfactor::RFactor pass2tier3 = passCount2 * useNoOverwrite * swapTilePoolOnPass2 * tiledResourcesTier3 * tier23pairwiseFactor;

        root = pass1tier3 + pass2tier3;
    }

    SetRootTestFactor(root);
}


template <TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CUpdateTileMappingsTestBase<resourceBase>::ExecuteTestCase()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    CHandle<ID3D11TextureType> pStagingSource = NULL;
    CHandle<ID3D11TextureType> pTiledResourceFull = NULL;
    CHandle<ID3D11TextureType> pTiledResource = NULL;
    CHandle<ID3D11DeviceContext2> pContext = GetImmediateContext2();
    TileMappingType tileMapping;
    TileMappingType tileMappingMip0;
    TileMappingType tileMappingMip1;
    TileMappingType fullMapping;
    TileMappingType foundMapping;
    CTiledResourceHelper::CTilePoolManager *pTilePoolManager = GetFramework()->GetTilePoolManager();
    CHandle<ID3D11Buffer> pTilePool;
    UINT startTiles;
    RESIDENCY_PATTERN_2D_3D residencyPattern;
    bool useNoOverwrite = m_parameters.pUseNoOverwrite->GetValue();

    CResourceBaseTestHelper<resourceBase>::GenerateTileMapping(tileMappingMip1, 2, 2, 2, nullptr);

    const UINT width = 4 * CResourceBaseTestHelper<resourceBase>::widthPerTile;
    const UINT height = 4 * CResourceBaseTestHelper<resourceBase>::heightPerTile;
    const UINT depth = 4 * CResourceBaseTestHelper<resourceBase>::depthPerTile;     // depth will be ignored by Texture2D methods

    pTilePool = pTilePoolManager->GetTilePool();

    // Create the tiled texture used to initialize the tile pool
    tr = CTiledResourcesTest::CreateTiledTexture(width, height, depth, &pTiledResourceFull);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Allocate the tiles from the framework tile pool
    startTiles = pTilePoolManager->FetchRange(pTiledResourceFull, CResourceBaseTestHelper<resourceBase>::totalNumTiles);

    // Create the tiled texture used to test the tile mapping
    const float multiplierX = m_parameters.pWidthMultiplier->GetValue();
    const float multiplierY = m_parameters.pHeightMultiplier->GetValue();
    const float multiplierZ = (TRB_TEXTURE_3D == resourceBase ? m_parameters.pDepthMultiplier->GetValue() : 1.f);
    tr = CTiledResourcesTest::CreateTiledTexture(
        static_cast<unsigned>(width * multiplierX),
        static_cast<unsigned>(height * multiplierY),
        static_cast<unsigned>(depth * multiplierZ),     // depth is ignored by Texture2D version of method
        &pTiledResource
        );
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Setup the staging source texture
    tr = CResourceBaseTestHelper<resourceBase>::CreateStagingTexture(width, height, depth, &pStagingSource, true);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    if (1 == CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue())
    {
        // Set the entire resource to the shared NULL tile
        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResourceFull, 1, pTilePool, RP2D_3D_CLEAR_ALL_MIPS, tileMapping);
        if (RESULT_PASS != tr)
        {
            return tr;
        }

        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResource, 2 * CResourceBaseTestHelper<resourceBase>::totalNumTiles, pTilePool, RP2D_3D_CLEAR_ALL_MIPS, tileMapping);
        if (RESULT_PASS != tr)
        {
            return tr;
        }
    }

    for (UINT pass = 0; pass < m_parameters.pPassCount->GetValue(); ++pass)
    {
        WriteToLog("Begin Pass %u", pass + 1);

        switch (pass)
        {
        case 0:
            residencyPattern = m_parameters.pResidencyPatternPass1->GetValue();
            break;

        case 1:
            residencyPattern = m_parameters.pResidencyPatternPass2->GetValue();
            if (m_parameters.pSwapTilePoolOnPass2->GetValue())
            {
                CD3D11_BUFFER_DESC tempPoolDesc(
                    TILE_SIZE_IN_BYTES * 4 * CResourceBaseTestHelper<resourceBase>::totalNumTiles,
                    0
                    );
                tempPoolDesc.MiscFlags = D3D11_RESOURCE_MISC_TILE_POOL;
                pTilePool->Release();
                hr = GetDevice2()->CreateBuffer(&tempPoolDesc, nullptr, &pTilePool);
                if (FAILED(hr))
                {
                    REPORT_HRESULT_FAILURE(hr, CreateBuffer);
                    return RESULT_FAIL;
                }
                startTiles = 1;
                tileMappingMip0.Clear();
                tileMappingMip1.Clear();

                if (1 == CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue())
                {
                    // Reset the entire resource to the new shared NULL tile
                    UINT restoreBlankTileIndex = m_blankTileIndex;
                    m_blankTileIndex = 0;
                    tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResourceFull, 2 * CResourceBaseTestHelper<resourceBase>::totalNumTiles, pTilePool, RP2D_3D_CLEAR_ALL_MIPS, tileMapping);
                    if (RESULT_PASS != tr)
                    {
                        m_blankTileIndex = restoreBlankTileIndex;
                        return tr;
                    }

                    tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResource, 3 * CResourceBaseTestHelper<resourceBase>::totalNumTiles, pTilePool, RP2D_3D_CLEAR_ALL_MIPS, tileMapping);
                    if (RESULT_PASS != tr)
                    {
                        m_blankTileIndex = restoreBlankTileIndex;
                        return tr;
                    }
                }
            }

            break;
        }

        if (0 == pass || m_parameters.pSwapTilePoolOnPass2->GetValue())
        {
            // Update the tiled resources
            tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResourceFull, startTiles, pTilePool, RP2D_3D_FULL, fullMapping, useNoOverwrite);
            if (RESULT_PASS != tr)
            {
                return tr;
            }

            // Even though tiled resources used by this test are all 'safe' to overwrite it is 
            // a good idea to place a barrier before each new write
            pContext->TiledResourceBarrier(nullptr, pTiledResourceFull);

            // Copy the source data to the fully mapped tiled resource
            pContext->CopyResource(pTiledResourceFull, pStagingSource);

            pContext->TiledResourceBarrier(pTiledResourceFull, pTiledResource);
        }
        else
        {
            pContext->TiledResourceBarrier(nullptr, pTiledResourceFull);
        }

        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResource, startTiles, pTilePool, residencyPattern, tileMapping, useNoOverwrite);
        if (RESULT_PASS != tr)
        {
            return tr;
        }

        switch (residencyPattern)
        {
        default:
            tileMappingMip0.Overlay(tileMapping);
            tileMapping = tileMappingMip0;
            break;

        case RP2D_3D_FULL_MIP_1:
        case RP2D_3D_FRAGMENTED_MIP_1:
        case RP2D_3D_CLEAR_MIP_1:
            tileMappingMip1.Overlay(tileMapping);
            tileMapping = tileMappingMip1;
            break;
        }

        if (0 == pass)
        {
            // Test for NULL tile corruptions
            if (false && CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue() > 1)
            {
                if (RESULT_PASS != CTiledResourcesTest::TestNullTiles(pTiledResource, 0, 0, tileMappingMip0))
                {
                    tr = RESULT_FAIL;
                    return tr;
                }
                if (RESULT_PASS != CTiledResourcesTest::TestNullTiles(pTiledResource, 1, 0, tileMappingMip1))
                {
                    tr = RESULT_FAIL;
                    return tr;
                }
            }
        }

        // Handle 'skips' in tile mappings
        {
            const UINT numSlices = (TRB_TEXTURE_2D == resourceBase ? 1 : depth);
            UINT nameIndex = 0;
            wchar_t combinedName[2 * numSlices][64];
            for (UINT slice = 0; slice < numSlices; ++slice)
            {
                wchar_t sliceName[32];
                auto GenerateName = [&](const UINT mipLevel, const UINT passNum, const UINT inSlice) -> const wchar_t*
                {
                    swprintf_s(sliceName, ARRAYSIZE(sliceName), L", Slice %d", inSlice);
                    swprintf_s(combinedName[nameIndex], ARRAYSIZE(combinedName[nameIndex]), L"Tiled Resource (Mip %d, Pass %d%s)", mipLevel, passNum, (numSlices == 1 ? L"" : sliceName));
                    return combinedName[nameIndex++];
                };
                ShowResource(GenerateName(1, pass, slice), pTiledResource, 1, 0, slice, 2.0f);
                ShowResource(GenerateName(0, pass, slice), pTiledResource, 0, 0, slice, 1.0f);
            }
        }

        // Get the actual data mapping in the tiled resource
        tr = CTiledResourcesTest::ValidateTileMapping(pTiledResource, pStagingSource, (RP2D_3D_FRAGMENTED_MIP_1 == residencyPattern ? 1 : 0), 0, tileMapping);
        if (RESULT_PASS != tr)
        {
            return tr;
        }

        WriteToLog("End Pass %u", pass + 1);
    }

    if (RESULT_PASS != CheckNullSentinel())
    {
        tr = RESULT_FAIL;
    }

    pContext->TiledResourceBarrier(pTiledResourceFull, nullptr);

    return tr;
}
