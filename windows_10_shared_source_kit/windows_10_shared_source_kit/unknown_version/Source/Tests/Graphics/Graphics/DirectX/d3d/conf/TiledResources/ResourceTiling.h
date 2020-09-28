////////////////////////////////////////////////////////////////
// ResourceTiling.h
////////////////////////////////////////////////////////////////

#pragma once

#include "D3D11Test.h"
#include "TiledResources.h"

// Base class for Resource Tiling test to support both Texture2D and Texture3D resources
template <TILED_RESOURCE_BASE resourceBase>
class CResourceTilingTestBase : public CResourceBaseTestHelper<resourceBase>
{
    // Helpers to clean up typenames
    CRESOURCE_BASE_TEST_HELPER__TYPENAME_TYPEDEFS

        // Parameters, unified for both Texture2D and Texture3D
        // Test will discriminate correctly which to use
    struct
    {
        CVariationParameter<UINT> *pWidth;
        CVariationParameter<UINT> *pHeight;
        CVariationParameter<UINT> *pDepth;              // Texture3D only
        CVariationParameter<UINT> *pNumMips;
        CVariationParameter<DXGI_FORMAT> *pFormat;
        CVariationParameter<UINT> *pArraySize;          // Texture2D only
        CVariationParameter<UINT> *pSampleCount;        // Texture2D only
    } m_parameters;

public:
    void InitTestParameters();
    TEST_RESULT ExecuteTestCase();
};



// Actual test classes
class CResourceTilingTestTex2D : public CResourceTilingTestBase < TRB_TEXTURE_2D > { };
class CResourceTilingTestTex3D : public CResourceTilingTestBase < TRB_TEXTURE_3D > { };





// Implementations
template <TILED_RESOURCE_BASE resourceBase>
void CResourceTilingTestBase<resourceBase>::InitTestParameters()
{
    // Parameters
    CreateParameter(_T("Width"), &m_parameters.pWidth);
    CreateParameter(_T("Height"), &m_parameters.pHeight);
    CreateParameter(_T("NumMips"), &m_parameters.pNumMips);
    CreateParameter(_T("Format"), &m_parameters.pFormat);

    // ValueSets
    CUserValueSet<UINT> *pWidthValues = new CUserValueSet<UINT>(64U, 100U, 256U);
    CUserValueSet<UINT> *pHeightValues = new CUserValueSet<UINT>(64U, 252U, 128U);
    CUserValueSet<UINT> *pNumMipsValues = new CUserValueSet<UINT>(0U, 3U);
    CUserValueSet<DXGI_FORMAT> *pFormatValues = new CUserValueSet<DXGI_FORMAT>();
    pFormatValues->AddValue(DXGI_FORMAT_R32G32B32A32_FLOAT);
    pFormatValues->AddValue(DXGI_FORMAT_R8G8B8A8_UINT);
    pFormatValues->AddValue(DXGI_FORMAT_R8_SINT);
    pFormatValues->AddValue(DXGI_FORMAT_R8G8_SNORM);
    pFormatValues->AddValue(DXGI_FORMAT_BC2_UNORM_SRGB);
    pFormatValues->AddValue(DXGI_FORMAT_BC1_UNORM_SRGB);

    // Factors
    testfactor::RFactor width = AddParameterValueSet(m_parameters.pWidth, pWidthValues);
    testfactor::RFactor height = AddParameterValueSet(m_parameters.pHeight, pHeightValues);
    testfactor::RFactor numMips = AddParameterValueSet(m_parameters.pNumMips, pNumMipsValues);
    testfactor::RFactor numMips3 = AddParameterValue(m_parameters.pNumMips, 3U);
    testfactor::RFactor format = AddParameterValueSet(m_parameters.pFormat, pFormatValues);

    CreateParameter(_T("TiledResourcesTier"), &CTiledResourcesTest::m_parameters.pTiledResourcesTier);
    CTiledResourcesTest::m_parameters.pTiledResourcesTier->SetDefaultValue(1U);
    testfactor::RFactor tiledResourcesTier1 = AddParameterValue(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 1U);
    testfactor::RFactor tiledResourcesTier2 = AddParameterValue(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 2U);
    testfactor::RFactor tiledResourcesTier3 = AddParameterValue(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 3U);

    m_parameters.pWidth->SetDefaultValue(512);
    m_parameters.pHeight->SetDefaultValue(512);
    m_parameters.pNumMips->SetDefaultValue(1);
    m_parameters.pFormat->SetDefaultValue(DXGI_FORMAT_R8G8B8A8_UINT);

    testfactor::RFactor perTierFactor;

    // Texture2D specific set-up
    if (TRB_TEXTURE_2D == resourceBase)
    {
        // Parameters
        CreateParameter(_T("ArraySize"), &m_parameters.pArraySize);
        CreateParameter(_T("SampleCount"), &m_parameters.pSampleCount);

        // ValueSets
        CUserValueSet<UINT> *pArraySizeValues = new CUserValueSet<UINT>(1U, 3U);
        CUserValueSet<UINT> *pSampleCountValues = new CUserValueSet<UINT>(4U, 16U);

        // Factors
        //        testfactor::RFactor arraySize = AddParameterValueSet(m_parameters.pArraySize, pArraySizeValues);
        testfactor::RFactor arraySize1 = AddParameterValue(m_parameters.pArraySize, 1U);
        testfactor::RFactor arraySize2 = AddParameterValue(m_parameters.pArraySize, 2U);
        testfactor::RFactor sampleCount = AddParameterValueSet(m_parameters.pSampleCount, pSampleCountValues);

        // Defaults
        m_parameters.pSampleCount->SetDefaultValue(1U);

        // Tier factors to test
        testfactor::RFactor pairwiseFactor = testfactor::NewPairwiseFactor(width, height, (numMips + sampleCount));
        perTierFactor = arraySize1 * pairwiseFactor * format + arraySize2 * numMips3;
    }
    // Texture3D specific set-up
    else
    {
        // Parameters
        CreateParameter(_T("Depth"), &m_parameters.pDepth);

        // ValueSets
        CUserValueSet<UINT> *pDepthValues = new CUserValueSet<UINT>(1U, 3U);

        // Factors
        //        testfactor::RFactor depth = AddParameterValueSet(m_parameters.pDepth, pDepthValues);
        testfactor::RFactor depth1 = AddParameterValue(m_parameters.pDepth, 1U);
        testfactor::RFactor depth2 = AddParameterValue(m_parameters.pDepth, 2U);

        // Defaults
        m_parameters.pDepth->SetDefaultValue(1U);

        // Tier factors to test
        testfactor::RFactor pairwiseFactor = testfactor::NewPairwiseFactor(width, height, numMips);
        perTierFactor = depth1 * pairwiseFactor * format + depth2 * numMips3;
    }

    testfactor::RFactor tier1Factors = tiledResourcesTier1 * perTierFactor;
    testfactor::RFactor tier2Factors = tiledResourcesTier2 * perTierFactor;
    testfactor::RFactor tier3Factors = tiledResourcesTier3 * perTierFactor;

    // When using Texture2D, skip building factors for Tier 3 (Tier 2 tests correct functionality)
    // When using Texture3D, skip building factors for Tiers 1 and 2
    testfactor::RFactor root = (TRB_TEXTURE_2D == resourceBase ? (tier1Factors + tier2Factors) : tier3Factors);

    SetRootTestFactor(root);
}


template <TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CResourceTilingTestBase<resourceBase>::ExecuteTestCase()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    CHandle<ID3D11TextureType> pTexture;
    ID3D11Device2 *pDevice = GetDevice2();

    const UINT textureDescSampleCount = (TRB_TEXTURE_2D == resourceBase ? m_parameters.pSampleCount->GetValue() : 1);
    const UINT textureDescDepth = (TRB_TEXTURE_2D == resourceBase ? 1 : m_parameters.pDepth->GetValue());
    const UINT textureDescArraySize = (TRB_TEXTURE_2D == resourceBase ? m_parameters.pArraySize->GetValue() : 1);

    CD3D11_TEXTURE_TYPE_DESC desc;
    CResourceBaseTestHelper<resourceBase>::GenerateTextureDesc(
        desc,
        m_parameters.pFormat->GetValue(),
        m_parameters.pWidth->GetValue(),
        m_parameters.pHeight->GetValue(),
        (TRB_TEXTURE_2D == resourceBase ? m_parameters.pArraySize->GetValue() : m_parameters.pDepth->GetValue()),   // arraySize (Texture2D) or depth (Texture3D)
        m_parameters.pNumMips->GetValue(),                                              // mipLevels
        D3D11_BIND_SHADER_RESOURCE,                                                     // bindFlags
        D3D11_USAGE_DEFAULT,                                                            // usage
        0,                                                                              // cpaccessFlags
        D3D11_RESOURCE_MISC_TILED,                                                      // miscFlags
        textureDescSampleCount,                                                         // sampleCount (ignored for Texture3D)
        (TRB_TEXTURE_2D == resourceBase ? 0 : 0)                                        // sampleQuality (ignored for Texture3D)
        );

    // If multisampling then verify device supports tiling (only Texture2D)
    if (TRB_TEXTURE_2D == resourceBase)
    {
        if (textureDescSampleCount > 1)
        {
            UINT numQualityLevels = 0;
            hr = pDevice->CheckMultisampleQualityLevels1(desc.Format, textureDescSampleCount, D3D11_CHECK_MULTISAMPLE_QUALITY_LEVELS_TILED_RESOURCE, &numQualityLevels);
            if (FAILED(hr))
            {
                REPORT_HRESULT_FAILURE(hr, CheckMultisampleQualityLevels1);
                return RESULT_FAIL;
            }

            if (numQualityLevels == 0)
            {
                return RESULT_SKIP;
            }
        }
    }

    tr = CTiledResourcesTest::CreateTexture(&desc, &pTexture);
    if (RESULT_PASS != tr)
    {
        return RESULT_FAIL;
    }

    UINT expectedMipCount = desc.MipLevels;

    if (expectedMipCount == 0)
    {
        const UINT tmpDescDepth = (TRB_TEXTURE_2D == resourceBase ? desc.Width : textureDescDepth);     // for Texture2D, make depth value not matter
        for (UINT mask = max(max(desc.Width, desc.Height), tmpDescDepth); mask != 0; mask >>= 1)
        {
            expectedMipCount++;
        }
    }

    UINT numTilesForEntireResource;
    D3D11_PACKED_MIP_DESC packedMipDesc;
    D3D11_TILE_SHAPE tileShape;
    UINT numSubresourceTilings = 1024;
    UINT firstSubresource = 0;
    D3D11_SUBRESOURCE_TILING subresourceTiling[1024];
    pDevice->GetResourceTiling(
        pTexture,
        &numTilesForEntireResource,
        &packedMipDesc,
        &tileShape,
        &numSubresourceTilings,
        firstSubresource,
        subresourceTiling
        );

    D3D11_TILE_SHAPE expectedTileShape;

    // Verify the tile shape
    CResourceBaseTestHelper<resourceBase>::GetTileShapeFromTextureDesc(expectedTileShape, desc);

    if (expectedTileShape.WidthInTexels != tileShape.WidthInTexels)
    {
        WriteToLog(_T("Incorrect tile width for format (Expected %u, Observed %u)"), expectedTileShape.WidthInTexels, tileShape.WidthInTexels);
        tr = RESULT_FAIL;
    }

    if (expectedTileShape.HeightInTexels != tileShape.HeightInTexels)
    {
        WriteToLog(_T("Incorrect tile Height for format (Expected %u, Observed %u)"), expectedTileShape.HeightInTexels, tileShape.HeightInTexels);
        tr = RESULT_FAIL;
    }

    if (TRB_TEXTURE_3D == resourceBase)
    {
        if (expectedTileShape.DepthInTexels != tileShape.DepthInTexels)
        {
            WriteToLog(_T("Incorrect tile Depth for format (Expected %u, Observed %u)"), expectedTileShape.DepthInTexels, tileShape.DepthInTexels);
            tr = RESULT_FAIL;
        }
    }

    UINT expectedStandardMipCount = expectedMipCount - packedMipDesc.NumPackedMips;
    UINT nonPackedTileCount = 0;

    for (UINT slice = 0; slice < textureDescArraySize; ++slice)                     // textureDescArraySize == 1 for Texture3D
    {
        UINT subresourceId = slice * expectedMipCount;
        for (UINT i = 0; i < expectedStandardMipCount; ++i)
        {
            UINT width = desc.Width;
            UINT height = desc.Height;
            UINT depth = (TRB_TEXTURE_2D == resourceBase ? 0 : textureDescDepth);   // 0 for Texture2D, making GetMipDirections() ignore parameter
            CFormatHelper::GetMipDimensions(i, &width, &height, &depth);
            depth = (TRB_TEXTURE_2D == resourceBase ? 1 : depth);                   // force depth to be 1 for Texture2D to make math below work out

            UINT widthInTiles = 1 + (width - 1) / tileShape.WidthInTexels;
            UINT heightInTiles = 1 + (height - 1) / tileShape.HeightInTexels;
            UINT depthInTiles = 1 + (depth - 1) / tileShape.DepthInTexels;          // always 1 for Texture2D
            nonPackedTileCount += widthInTiles * heightInTiles * depthInTiles;

            if (subresourceTiling[subresourceId].WidthInTiles != widthInTiles)
            {
                tr = RESULT_FAIL;
                WriteToLog(_T("Incorrect tile width for mip level %u (Expected %u, Observed %u)"), i, widthInTiles, subresourceTiling[subresourceId].WidthInTiles);
            }

            if (subresourceTiling[subresourceId].HeightInTiles != heightInTiles)
            {
                tr = RESULT_FAIL;
                WriteToLog(_T("Incorrect tile height for mip level %u (Expected %u, Observed %u)"), i, heightInTiles, subresourceTiling[subresourceId].HeightInTiles);
            }

            if (TRB_TEXTURE_3D == resourceBase)
            {
                if (subresourceTiling[i].DepthInTiles != depthInTiles)
                {
                    tr = RESULT_FAIL;
                    WriteToLog(_T("Incorrect tile depth for mip level %u (Expected %u, Observed %u)"), i, depthInTiles, subresourceTiling[subresourceId].DepthInTiles);
                }
            }

            subresourceId++;
        }
    }

    UINT expectedTilesForEntireResource = nonPackedTileCount + packedMipDesc.NumTilesForPackedMips * textureDescArraySize;  // textureDescArraySize == 1 for Texture3D
    if (expectedTilesForEntireResource != numTilesForEntireResource)
    {
        WriteToLog(_T("Incorrect total tile count for entire resource (Expected: %u, Observed %u)"), expectedTilesForEntireResource, numTilesForEntireResource);
        tr = RESULT_FAIL;
    }

    return tr;
}
