////////////////////////////////////////////////////////////////////////////////
// UpdateTiles.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TiledResources.h"

// Base Update Tiles Test class, from which we derive specializations for Texture2D and Texture3D
template <TILED_RESOURCE_BASE resourceBase>
class CUpdateTilesTestBase : public CResourceBaseTestHelper<resourceBase>
{
    // Helpers to clean up typenames
    CRESOURCE_BASE_TEST_HELPER__TYPENAME_TYPEDEFS

        // Parameters
    struct
    {
        CVariationParameter<RESIDENCY_PATTERN_2D_3D> *pResidencyPattern;
        CVariationParameter<COPY_TILES_PATTERN> *pCopyTilesPattern;
        CVariationParameter<UINT> *pTextureDimensions;
    } m_parameters;

protected:
    wchar_t *windowNames;
    CUpdateTilesTestBase() : windowNames(nullptr) {}
    virtual ~CUpdateTilesTestBase() { if (windowNames) delete[] windowNames; }

public:
    void InitTestParameters();
    TEST_RESULT ExecuteTestCase();
};


// Actual test classes
class CUpdateTilesTestTex2D : public CUpdateTilesTestBase < TRB_TEXTURE_2D > { };
class CUpdateTilesTestTex3D : public CUpdateTilesTestBase < TRB_TEXTURE_3D > { };




// Test Implementation
template <TILED_RESOURCE_BASE resourceBase>
void CUpdateTilesTestBase<resourceBase>::InitTestParameters()
{
    CreateParameter(_T("TiledResourcesTier"), &CTiledResourcesTest::m_parameters.pTiledResourcesTier);
    CreateParameter(_T("ResidencyPattern"), &m_parameters.pResidencyPattern);
    CreateParameter(_T("CopyTilesPattern"), &m_parameters.pCopyTilesPattern);
    CreateParameter(_T("TextureDimensions"), &m_parameters.pTextureDimensions);

    testfactor::RFactor tiledResourcesTier1 = AddParameterValue<UINT>(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 1U);
    testfactor::RFactor tiledResourcesTier2 = AddParameterValue<UINT>(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 2U);
    testfactor::RFactor tiledResourcesTier3 = AddParameterValue<UINT>(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 3U);

    testfactor::RFactor residencyPatternMip0 = AddParameterValueSet<RESIDENCY_PATTERN_2D_3D>(
        m_parameters.pResidencyPattern,
        new CUserValueSet<RESIDENCY_PATTERN_2D_3D>(
        RP2D_3D_FULL,
        RP2D_3D_FRAGMENTED_MULTI_TILE,
        RP2D_3D_UNIFORM_BOX
        )
        );
    testfactor::RFactor residencyPatternMip1 = AddParameterValue(m_parameters.pResidencyPattern, RP2D_3D_FRAGMENTED_MIP_1);

    testfactor::RFactor copyTilesPatternMip0 = AddParameterValueSet<COPY_TILES_PATTERN>(
        m_parameters.pCopyTilesPattern,
        new CUserValueSet<COPY_TILES_PATTERN>(
        COPY_TILES_FULL,
        COPY_TILES_CENTER_BOX,
        COPY_TILES_PARTIAL
        )
        );
    testfactor::RFactor copyTilesPatternMip1 = AddParameterValue(m_parameters.pCopyTilesPattern, COPY_TILES_MIP1);

    testfactor::RFactor textureDimensionsTier1 = AddParameterValue<UINT>(
        m_parameters.pTextureDimensions,
        (UINT)(4 * CResourceBaseTestHelper<resourceBase>::widthPerTile)
        );
    testfactor::RFactor textureDimensionsTier2 = AddParameterValue<UINT>(
        m_parameters.pTextureDimensions,
        (UINT)(3.5 * CResourceBaseTestHelper<resourceBase>::widthPerTile)
        );

    // When using Texture2D, skip building factors for Tier 3 (Tier 2 tests correct functionality)
    // When using Texture3D, skip building factors for Tiers 1 and 2
    testfactor::RFactor textureDimensions;
    if (TRB_TEXTURE_2D == resourceBase)
    {
        textureDimensions = (tiledResourcesTier1 * textureDimensionsTier1) +
            ((textureDimensionsTier1 + textureDimensionsTier2) * tiledResourcesTier2);
    }
    else
    {
        textureDimensions = ((textureDimensionsTier1 + textureDimensionsTier2) * tiledResourcesTier3);
    }

    testfactor::RFactor patterns = (residencyPatternMip0 * copyTilesPatternMip0) + (residencyPatternMip1 * copyTilesPatternMip1);
    testfactor::RFactor root = patterns * textureDimensions;
    SetRootTestFactor(root);
}

template <TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CUpdateTilesTestBase<resourceBase>::ExecuteTestCase()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    CHandle<ID3D11TextureType> pTiledResource = NULL;
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    TileMappingType tileMappingMip0;
    TileMappingType tileMappingMip1;
    TileMappingType tileMapping;
    TileMappingType bufferMapping;
    BufferMappingType bufferOutputMapping;
    TileMappingType postTileMappings;
    CTiledResourceHelper::CTilePoolManager *pTilePoolManager = GetFramework()->GetTilePoolManager();
    UINT startTiles;
    CHandle<ID3D11RenderTargetView> pRTV;
    COPY_TILES_PATTERN copyTilesPattern = m_parameters.pCopyTilesPattern->GetValue();
    std::vector<BYTE> bufferData(TILE_SIZE_IN_BYTES * 255, 0);
    UINT mipLevel = 0;

    CResourceBaseTestHelper<resourceBase>::GenerateTileMapping(tileMappingMip1, 2, 2, 2, nullptr);

    // Width, Height and Depth (if applicable) are the same
    const UINT width = m_parameters.pTextureDimensions->GetValue();
    const UINT height = width;
    const UINT depth = width;

    const UINT pixelsPerTile = TILE_SIZE_IN_BYTES / sizeof(R8G8B8A8_PIXEL);
    const UINT pixelDataSize = pixelsPerTile * sizeof(R8G8B8A8_PIXEL) * CResourceBaseTestHelper<resourceBase>::totalNumTiles;
    assert(pixelDataSize < bufferData.size());

    R8G8B8A8_PIXEL *pixelData = reinterpret_cast<R8G8B8A8_PIXEL *>(bufferData.data());
    CResourceBaseTestHelper<resourceBase>::FillBufferWithTilePattern(pixelData, 4, 4, 4);

    // Create the tiled texture 
    tr = CTiledResourcesTest::CreateTiledTexture(width, height, depth, &pTiledResource, D3D11_BIND_RENDER_TARGET);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Allocate the tiles from the framework tile pool
    startTiles = pTilePoolManager->FetchRange(pTiledResource, CResourceBaseTestHelper<resourceBase>::totalNumTiles);

    // Initialize all tier-1 tile mappings to blank tile
    if (1 == CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue())
    {
        // Set the entire resource to the shared NULL tile
        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResource, 1, pTilePoolManager->GetTilePool(), RP2D_3D_CLEAR_ALL_MIPS, tileMapping);
        if (RESULT_PASS != tr)
        {
            return tr;
        }
    }

    // Update the tiled resource
    tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResource, startTiles, pTilePoolManager->GetTilePool(), m_parameters.pResidencyPattern->GetValue(), tileMapping);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    switch (m_parameters.pResidencyPattern->GetValue())
    {
    case RP2D_3D_FULL:
    case RP2D_3D_FRAGMENTED_MULTI_TILE:
    case RP2D_3D_FRAGMENTED_MULTI_TILE_SKIPS:
    case RP2D_3D_FRAGMENTED_REUSE_SINGLE_TILE:
    case RP2D_3D_UNIFORM_BOX:
    case RP2D_3D_CLEAR_MIP_0:
        // Clear the mip level not under test
        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResource, startTiles, pTilePoolManager->GetTilePool(), RP2D_3D_CLEAR_MIP_1, tileMappingMip1);
        if (RESULT_PASS != tr)
        {
            return tr;
        }
        mipLevel = 0;
        tileMappingMip0.Overlay(tileMapping);
        tileMapping = tileMappingMip0;
        break;

    case RP2D_3D_FULL_MIP_1:
    case RP2D_3D_FRAGMENTED_MIP_1:
    case RP2D_3D_CLEAR_MIP_1:
        // Clear the mip level not under test
        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResource, startTiles, pTilePoolManager->GetTilePool(), RP2D_3D_CLEAR_MIP_0, tileMappingMip0);
        if (RESULT_PASS != tr)
        {
            return tr;
        }
        mipLevel = 1;
        tileMappingMip1.Overlay(tileMapping);
        tileMapping = tileMappingMip1;
        break;
    }

    // Clear the tiled resource
    float clearColor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
    CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    CResourceBaseTestHelper<resourceBase>::GenerateRenderTargetViewDesc(
        rtvDesc,
        pTiledResource,
        D3D11_RTV_DIMENSION_TEXTURE2D,  // ignored for Texture3D
        DXGI_FORMAT_R8G8B8A8_UNORM,
        0
        );

    hr = GetDevice()->CreateRenderTargetView(pTiledResource, &rtvDesc, &pRTV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
        return RESULT_FAIL;
    }
    pContext->TiledResourceBarrier(nullptr, pRTV);
    pContext->ClearRenderTargetView(pRTV, clearColor);
    pRTV->Release();

    if (TRB_TEXTURE_2D == resourceBase)
    {
        rtvDesc.Texture2D.MipSlice = 1;
    }
    else
    {
        rtvDesc.Texture3D.MipSlice = 1;
        rtvDesc.Texture3D.FirstWSlice = 0;
        rtvDesc.Texture3D.WSize = -1;
    }
    hr = GetDevice()->CreateRenderTargetView(pTiledResource, &rtvDesc, &pRTV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
        return RESULT_FAIL;
    }
    pContext->TiledResourceBarrier(nullptr, pRTV);
    pContext->ClearRenderTargetView(pRTV, clearColor);

    // Copy the tiles from the source buffer
    pContext->TiledResourceBarrier(pTiledResource, pTiledResource);
    tr = CTiledResourcesTest::UpdateTiles(pTiledResource, bufferData.data(), copyTilesPattern, 0, bufferMapping);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Figure out which parts of the resource to validate
    tr = CResourceBaseTestHelper<resourceBase>::IntersectMappings(tileMapping, copyTilesPattern, postTileMappings, bufferOutputMapping);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    {
        const UINT numSlices = (TRB_TEXTURE_2D == resourceBase ? 1 : CResourceBaseTestHelper<resourceBase>::depthPerTile * (mipLevel == 0 ? 4 : 2));
        UINT nameIndex = 0;
        if (CUpdateTilesTestBase<resourceBase>::windowNames == nullptr)
        {
            CUpdateTilesTestBase<resourceBase>::windowNames = new wchar_t[2 * numSlices * 64];
        }
        for (UINT slice = 0; slice < numSlices; ++slice)
        {
            wchar_t sliceName[32];
            auto GenerateName = [&](const UINT mipLevel, const UINT inSlice) -> const wchar_t*
            {
                swprintf_s(sliceName, ARRAYSIZE(sliceName), L", Slice %d", inSlice);
                swprintf_s(windowNames + (64 * nameIndex), 64, L"Tiled Resource (Mip %d%s)", 0, (numSlices == 1 ? L"" : sliceName));
                return(windowNames + 64 * nameIndex++);
            };
            ShowResource(GenerateName(0, slice), pTiledResource, 0, 0, slice);
            ShowResource(GenerateName(1, slice), pTiledResource, 1, 0, slice, 2.0f);
        }
    }

    // Validate that the tiled resource matches expected, accounting for NULLs and unchanged tiles
    tr = CTiledResourcesTest::ValidateTileAgainstBuffer(pTiledResource, pixelData, (COPY_TILES_MIP1 == copyTilesPattern ? 1 : 0), 0, postTileMappings, clearColor);
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
