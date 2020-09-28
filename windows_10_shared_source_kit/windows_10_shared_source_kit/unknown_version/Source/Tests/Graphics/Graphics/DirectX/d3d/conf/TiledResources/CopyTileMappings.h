////////////////////////////////////////////////////////////////////////////////
// CopyTileMappings.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TiledResources.h"

// Base for Copy Tile Mappings Test, used for both Texture2D and Texture3D
template <TILED_RESOURCE_BASE resourceBase>
class CCopyTileMappingsTestBase : public CResourceBaseTestHelper<resourceBase>
{
    // Helpers to clean up typenames
    CRESOURCE_BASE_TEST_HELPER__TYPENAME_TYPEDEFS

        // Parameters
    struct
    {
        CVariationParameter<RESIDENCY_PATTERN_2D_3D> *pResidencyPattern;
        CVariationParameter<COPY_TILE_MAPPINGS_PATTERN> *pCopyTileMappingsPattern;
    } m_parameters;

public:
    void InitTestParameters();
    TEST_RESULT ExecuteTestCase();
};



// Actual test classes
class CCopyTileMappingsTestTex2D : public CCopyTileMappingsTestBase < TRB_TEXTURE_2D > { };
class CCopyTileMappingsTestTex3D : public CCopyTileMappingsTestBase < TRB_TEXTURE_3D > { };





// Implementations
template <TILED_RESOURCE_BASE resourceBase>
void CCopyTileMappingsTestBase<resourceBase>::InitTestParameters()
{
    CreateParameter(_T("ResidencyPattern"), &m_parameters.pResidencyPattern);
    CreateParameter(_T("CopyTileMappingsPattern"), &m_parameters.pCopyTileMappingsPattern);

    CUserValueSet<RESIDENCY_PATTERN_2D_3D> *pResidencyPatternValues = new CUserValueSet<RESIDENCY_PATTERN_2D_3D>();
    pResidencyPatternValues->AddValue(RP2D_3D_FULL);
    pResidencyPatternValues->AddValue(RP2D_3D_FRAGMENTED_MULTI_TILE);
    pResidencyPatternValues->AddValue(RP2D_3D_FRAGMENTED_REUSE_SINGLE_TILE);
    pResidencyPatternValues->AddValue(RP2D_3D_UNIFORM_BOX);

    testfactor::RFactor residencyPattern = AddParameterValueSet<RESIDENCY_PATTERN_2D_3D>(
        m_parameters.pResidencyPattern,
        pResidencyPatternValues
        );

    testfactor::RFactor copyTileMappingsPattern = AddParameterValueSet<COPY_TILE_MAPPINGS_PATTERN>(
        m_parameters.pCopyTileMappingsPattern,
        new CUserValueSet<COPY_TILE_MAPPINGS_PATTERN>(
        COPY_TILE_MAPPINGS_FULL,
        COPY_TILE_MAPPINGS_PARTIAL,
        COPY_TILE_MAPPINGS_OVERLAPPED
        )
        );

    // When using Texture2D, skip building factors for Tier 3 (Tier 2 tests correct functionality)
    // When using Texture3D, skip building factors for Tiers 1 and 2
    testfactor::RFactor root = residencyPattern * copyTileMappingsPattern * InitTiledResourcesTierParam((TRB_TEXTURE_3D == resourceBase));
}


template <TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CCopyTileMappingsTestBase<resourceBase>::ExecuteTestCase()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    CHandle<ID3D11TextureType> pStagingSource = NULL;
    CHandle<ID3D11TextureType> pTiledResourceSrc = NULL;
    CHandle<ID3D11TextureType> pTiledResourceDest = NULL;
    CHandle<ID3D11TextureType> pTiledResourceFull = NULL;
    CHandle<ID3D11DeviceContext2> pContext = GetImmediateContext2();
    TileMappingType tileMapping;
    TileMappingType tileMappingTemp;
    TileMappingType fullMapping;
    TileMappingType foundMapping;
    CTiledResourceHelper::CTilePoolManager *pTilePoolManager = GetFramework()->GetTilePoolManager();
    UINT startTiles;

    // Mapping strings are different between Texture2D (const char *strings[4]) 
    // and Texture3D (const char *strings[4][4]), so we'll allocate enough space
    // for Texture3D. We'll fill them and update tileMapping according to
    // Texture2D or Texture3D.
    //
    // Also null mapped tiles are identified differently, x for Texture2D, and
    // ? for Texture3D
    const char *mappingStrings[4][4] = { { NULL }, { NULL }, { NULL }, { NULL } };
    if (TRB_TEXTURE_2D == resourceBase)
    {
        mappingStrings[0][0] = "xxxx";
        mappingStrings[0][1] = "xxxx";
        mappingStrings[0][2] = "xxxx";
        mappingStrings[0][3] = "xxxx";
    }
    else
    {
        for (UINT i = 0; i < 4; ++i)
            for (UINT j = 0; j < 4; ++j)
                mappingStrings[i][j] = "????";
    }
    CResourceBaseTestHelper<resourceBase>::SetTileMappingFromStrings(tileMapping, mappingStrings);

    const UINT width = 4 * CResourceBaseTestHelper<resourceBase>::widthPerTile;
    const UINT height = 4 * CResourceBaseTestHelper<resourceBase>::heightPerTile;
    const UINT depth = 4 * CResourceBaseTestHelper<resourceBase>::depthPerTile;     // depth is ignored by every Texture2D specific method

    // Create the tiled texture used to initialize the tile pool tiles
    tr = CTiledResourcesTest::CreateTiledTexture(width, height, depth, &pTiledResourceFull);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Allocate the tiles from the framework tile pool
    startTiles = pTilePoolManager->FetchRange(pTiledResourceFull, CResourceBaseTestHelper<resourceBase>::totalNumTiles);

    // Create the source tiled texture
    tr = CTiledResourcesTest::CreateTiledTexture(width, height, depth, &pTiledResourceSrc);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Create the dest tiled texture
    tr = CTiledResourcesTest::CreateTiledTexture(width, height, depth, &pTiledResourceDest);
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

    // Initialize all tier-1 tile mappings to blank tile
    if (1 == CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue())
    {
        // Set the entire resource to the shared NULL tile
        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResourceFull, 1, pTilePoolManager->GetTilePool(), RP2D_3D_CLEAR_ALL_MIPS, tileMapping);
        if (RESULT_PASS != tr)
        {
            return tr;
        }

        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResourceSrc, 2 * CResourceBaseTestHelper<resourceBase>::totalNumTiles, pTilePoolManager->GetTilePool(), RP2D_3D_CLEAR_ALL_MIPS, tileMapping);
        if (RESULT_PASS != tr)
        {
            return tr;
        }

        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResourceDest, 2 * CResourceBaseTestHelper<resourceBase>::totalNumTiles, pTilePoolManager->GetTilePool(), RP2D_3D_CLEAR_ALL_MIPS, tileMapping);
        if (RESULT_PASS != tr)
        {
            return tr;
        }
    }

    // Update the fully-resident tiled resources
    RESIDENCY_PATTERN_2D_3D residencyPattern = m_parameters.pResidencyPattern->GetValue();
    tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResourceFull, startTiles, pTilePoolManager->GetTilePool(), RP2D_3D_FULL, fullMapping);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResourceSrc, startTiles, pTilePoolManager->GetTilePool(), residencyPattern, tileMappingTemp);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    tileMapping.Overlay(tileMappingTemp);

    // Even though tiled resources used by this test are all 'safe' to overwrite it is 
    // a good idea to place a barrier before each new write
    pContext->TiledResourceBarrier(nullptr, pTiledResourceFull);

    pContext->CopyResource(pTiledResourceFull, pStagingSource);
    pContext->Flush();

    // Since the tile data is read using a different resource a TiledResourceBarrier is needed
    pContext->TiledResourceBarrier(pTiledResourceFull, pTiledResourceDest);

    // Copy the tiled resource data
    COPY_TILE_MAPPINGS_PATTERN copyPattern = m_parameters.pCopyTileMappingsPattern->GetValue();

    // If using overlapped copy then point the dest to the source
    if (copyPattern == COPY_TILE_MAPPINGS_OVERLAPPED)
    {
        pTiledResourceDest = pTiledResourceSrc;
    }
    tr = CTiledResourcesTest::CopyTileMappings(pTiledResourceDest, pTiledResourceSrc, copyPattern, tileMapping);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Test for NULL tile corruptions
    if (false && CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue() > 1)
    {
        if (RESULT_PASS != CTiledResourcesTest::TestNullTiles(pTiledResourceDest, 0, 0, tileMapping))
        {
            tr = RESULT_FAIL;
            return tr;
        }
    }

    GetFramework()->GetResourceViewer().HideAll();

    pContext->TiledResourceBarrier(pTiledResourceSrc, nullptr);
    pContext->TiledResourceBarrier(pTiledResourceDest, nullptr);

    {
        const UINT mipToShow = ((RP2D_3D_FRAGMENTED_MIP_1 == residencyPattern || RP2D_3D_FULL_MIP_1 == residencyPattern) ? 1 : 0);
        const float scaleToShow = (mipToShow == 1 ? 2.0f : 1.0f);
        const UINT numSlices = (TRB_TEXTURE_2D == resourceBase ? 1 : depth);
        UINT nameIndex = 0;
        wchar_t combinedName[2 * numSlices][64];
        for (UINT slice = 0; slice < numSlices; ++slice)
        {
            wchar_t sliceName[32];
            auto GenerateName = [&](const wchar_t *prependName, const UINT mipLevel, const UINT inSlice) -> const wchar_t*
            {
                swprintf_s(sliceName, ARRAYSIZE(sliceName), L", Slice %d", inSlice);
                swprintf_s(combinedName[nameIndex], ARRAYSIZE(combinedName[nameIndex]), L"%s (Mip %d%s)", prependName, mipLevel, (numSlices == 1 ? L"" : sliceName));
                return combinedName[nameIndex++];
            };
            ShowResource(GenerateName(L"Source Tiled Resource", mipToShow, slice), pTiledResourceSrc, mipToShow, 0, slice, scaleToShow);
            ShowResource(GenerateName(L"Dest Tiled Resource", mipToShow, slice), pTiledResourceDest, mipToShow, 0, slice, scaleToShow);
        }
    }

    // Get the actual data mapping in the tiled resource
    tr = CTiledResourcesTest::ValidateTileMapping(pTiledResourceDest, pStagingSource, (residencyPattern == RP2D_3D_FRAGMENTED_MIP_1 ? 1 : 0), 0, tileMapping);
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
