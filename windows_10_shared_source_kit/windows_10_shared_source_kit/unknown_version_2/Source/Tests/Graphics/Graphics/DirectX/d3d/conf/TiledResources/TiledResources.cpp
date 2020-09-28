////////////////////////////////////////////////////////////////////////////////
// TiledResources.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "D3D11Test.h"
#include "FormatHelper.h"
#include "FormatDesc.hpp"
#include "LargeAddress.h"
#include "ZeroMapping.h"
#include "TiledResources.h"
#include "UpdateTileMappings.h"
#include "CopyTileMappings.h"
#include "CopyTiles.h"
#include "UpdateTiles.h"
#include "Views.h"
#include "Filter.h"
#include "RawStructured.h"
#include "ResourceTiling.h"

#include "d3d11sdklayers.h"

// Load all of shaders we need
// Texture2D shaders
#include <CopyTexCSNormal.hpp>
#include <CopyTexCSTiled.hpp>
#include <CopyTexTier1PS.hpp>
#include <CopyTexTier2PS.hpp>
#include <CopyTexVS.hpp>
#include <CopyTexGSTex3D.hpp>
#include <ProcessRawBufferTiled.hpp>
#include <ProcessStructuredBufferTiled.hpp>
#include <ProcessRawBufferNormal.hpp>
#include <ProcessStructuredBufferNormal.hpp>
#include <RenderDepthPS.hpp>
#include <RenderColorUsingDepthPS.hpp>
#include <FilterNormalTier1PS.hpp>
#include <FilterTiledTier1PS.hpp>
#include <FilterNormalTier2PS.hpp>
#include <FilterTiledTier2PS.hpp>
#include <GatherNormalTier1PS.hpp>
#include <GatherTiledTier1PS.hpp>
#include <GatherNormalTier2PS.hpp>
#include <GatherTiledTier2PS.hpp>

// Texture3D shaders
#include <CopyTexCSNormalTex3D.hpp>
#include <CopyTexCSTiledTex3D.hpp>
#include <CopyTexTier2PSTex3D.hpp>
#include <CopyTexVSTex3D.hpp>
#include <ProcessRawBufferTiledTex3D.hpp>
#include <ProcessStructuredBufferTiledTex3D.hpp>
#include <ProcessRawBufferNormalTex3D.hpp>
#include <ProcessStructuredBufferNormalTex3D.hpp>
#include <FilterNormalTier2PSTex3D.hpp>
#include <FilterTiledTier2PSTex3D.hpp>
//#include <GatherNormalTier2PSTex3D.hpp>       // to be added...
//#include <GatherTiledTier2PSTex3D.hpp>


using namespace WindowsTest;

////////////////////////////////////////////////////////////////////////////////
//
// Constants
//
////////////////////////////////////////////////////////////////////////////////
const UINT TILE_SIZE_IN_BYTES = 65536;
const UINT FIXED_ARRAY_SIZE = 1;
const DXGI_FORMAT FIXED_FORMAT_TEX2D = DXGI_FORMAT_R8G8B8A8_UNORM;
const DXGI_FORMAT FIXED_FORMAT_TEX3D = DXGI_FORMAT_R8G8B8A8_UNORM;
const UINT MAX_NUM_TILE_POOL_RANGES = 64;

CTiledResourcesApp g_App;


const UINT UnmappedTileId = static_cast<UINT>(-1);
const UINT SkippedTileId = UnmappedTileId - 1;
const UINT ReservedTileIdMask = 0x80000000UL;
bool UnmappedTile(UINT TileId)
{
    return TileId == UnmappedTileId;
}

bool SkippedTile(UINT TileId)
{
    return TileId == SkippedTileId;
}

bool UnmappedOrSkippedTile(UINT tileId)
{
    return UnmappedTile(tileId) || SkippedTile(tileId);
}

bool ReservedTile(UINT TileId)
{
    return 0 == (TileId & ReservedTileIdMask);
}

// Defines a pattern to be used to fill tiles when using Texture3D
// This lambda will be executed per pixel of the resource we passed in
// For meaning of i,j,k and a,b,c parameters, please see CreateStagingTextureHelper()
auto PerPixelLambdaTilePattern2D = [](R8G8B8A8_PIXEL **pixel, const UINT i, const UINT j, const UINT k, const UINT a, const UINT b, const UINT c, const UINT count)
{
    (*pixel)->r = (BYTE) 1 + count; // tileId + 1 (zero is special)
    (*pixel)->g = (BYTE) 1 + i * 32 + a;
    (*pixel)->b = (BYTE) 1 + j * 32 + b;
    (*pixel)->a = (BYTE) 255;
};

// Defines a pattern to be used to fill tiles when using Texture3D
// This lambda will be executed per pixel of the resource we passed in
// For meaning of i,j,k and a,b,c parameters, please see CreateStagingTextureHelper()
auto PerPixelLambdaTilePattern3D = [](R8G8B8A8_PIXEL **pixel, const UINT i, const UINT j, const UINT k, const UINT a, const UINT b, const UINT c, const UINT count)
{
    //        (*pixel)->r = (BYTE) 1 + count; // tileId + 1 (zero is special)
    //        (*pixel)->g = (BYTE) 1 + i * 32 + a;
    //        (*pixel)->b = (BYTE) 1 + j * 32 + b;
    //        (*pixel)->a = (BYTE) 1 + k * 32 + c;
    // Since Filter test relies on alpha being 0 or 1
    // Green and Blue will step with 6
    (*pixel)->r = (BYTE) 1 + count; // tileId + 1 (zero is special)
    (*pixel)->g = (BYTE) 1 + (k * 4 + i) * 6 + a;
    (*pixel)->b = (BYTE) 1 + (k * 4 + j) * 6 + b;
    (*pixel)->a = (BYTE) 255;
};

////////////////////////////////////////////////////////////////////////////////
//
// CTiledResourcesApp
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesApp::CTiledResourcesApp()
////////////////////////////////////////
CTiledResourcesApp::CTiledResourcesApp()
{
    m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_11_0;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesApp::InitTestGroups()
////////////////////////////////////////
bool CTiledResourcesApp::InitTestGroups()
{
    bool rval = true;

    // Tests using Texture2D resources
    ADD_TEST_AND_DESC(_T("Texture2D/ZeroMapping"), CZeroMappingTest, _T("Test cases for Zero mapping"));
    ADD_TEST_AND_DESC(_T("Texture2D/UpdateTileMappings"), CUpdateTileMappingsTestTex2D, _T("Test cases for UpdateTileMappings using Tex2D resources"));
    ADD_TEST_AND_DESC(_T("Texture2D/CopyTileMappings"), CCopyTileMappingsTestTex2D, _T("Test cases for CopyTileMappings using Tex2D resources"));
    ADD_TEST_AND_DESC(_T("Texture2D/RenderTargetView"), CTiledRenderTargetViewTestTex2D, _T("Test cases for using render target views on tiled resources using Tex2D"));
    ADD_TEST_AND_DESC(_T("Texture2D/ShaderResourceView"), CTiledShaderResourceViewTestTex2D, _T("Test cases for using shader resource views on tiled resources using Tex2D"));
    ADD_TEST_AND_DESC(_T("Texture2D/DepthStencilView"), CTiledDepthStencilViewTest, _T("Test cases for using depth stencil views on tiled resources"));
    ADD_TEST_AND_DESC(_T("Texture2D/UnorderedAccessView"), CTiledUnorderedAccessViewTestTex2D, _T("Test cases for using unordered access views on tiled resources using Tex2D"));
    ADD_TEST_AND_DESC(_T("Texture2D/Filter"), CFilterTestTex2D, _T("Test cases for using sample filters on tiled resources using Tex2D"));
    ADD_TEST_AND_DESC(_T("Texture2D/LargeAddress"), CLargeAddressTestTex2D, _T("Test cases for (virtually) very large resources using Tex2D"));
    ADD_TEST_AND_DESC(_T("Texture2D/ResourceTiling"), CResourceTilingTestTex2D, _T("Test cases for verifying resource tiling using Tex2D resources"));
    ADD_TEST_AND_DESC(_T("Texture2D/CopyTiles"), CCopyTilesTestTex2D, _T("Test cases for using CopyTiles between Tex2D resources and buffers"));
    ADD_TEST_AND_DESC(_T("Texture2D/UpdateTiles"), CUpdateTilesTestTex2D, _T("Test cases for using UpdateTiles to Tex2D resources"));
    ADD_TEST_AND_DESC(_T("Texture2D/Buffers/Raw"), CRawTestTex2D, _T("Test cases for loading from tiled raw buffers using Tex2D resources"));
    ADD_TEST_AND_DESC(_T("Texture2D/Buffers/Structured"), CStructuredTestTex2D, _T("Test cases for loading from tiled structured buffers using Tex2D resources"));

    // Tests using Texture3D resources
    ADD_TEST_AND_DESC(_T("Texture3D/UpdateTileMappings"), CUpdateTileMappingsTestTex3D, _T("Test cases for UpdateTileMappings using Tex3D resources"));
    ADD_TEST_AND_DESC(_T("Texture3D/CopyTileMappings"), CCopyTileMappingsTestTex3D, _T("Test cases for CopyTileMappings using Tex3D resources"));
    ADD_TEST_AND_DESC(_T("Texture3D/RenderTargetView"), CTiledRenderTargetViewTestTex3D, _T("Test cases for using render target views on tiled resources using Tex3D"));
    ADD_TEST_AND_DESC(_T("Texture3D/ShaderResourceView"), CTiledShaderResourceViewTestTex3D, _T("Test cases for using shader resource views on tiled resources using Tex3D"));
    // No depth stencil views allowed for Texture3D resources
    ADD_TEST_AND_DESC(_T("Texture3D/UnorderedAccessView"), CTiledUnorderedAccessViewTestTex3D, _T("Test cases for using unordered access views on tiled resources using Tex3D"));
    ADD_TEST_AND_DESC(_T("Texture3D/Filter"), CFilterTestTex3D, _T("Test cases for using sample filters on tiled resources using Tex3D"));
    ADD_TEST_AND_DESC(_T("Texture3D/LargeAddress"), CLargeAddressTestTex3D, _T("Test cases for (virtually) very large resources using Tex3D")); // may be interesting, but if Texture2D works, Texture3D should too. Leaving in for now
    ADD_TEST_AND_DESC(_T("Texture3D/ResourceTiling"), CResourceTilingTestTex3D, _T("Test cases for verifying resource tiling using Tex3D resources"));
    ADD_TEST_AND_DESC(_T("Texture3D/CopyTiles"), CCopyTilesTestTex3D, _T("Test cases for using CopyTiles between Tex3D resources and buffers"));
    ADD_TEST_AND_DESC(_T("Texture3D/UpdateTiles"), CUpdateTilesTestTex3D, _T("Test cases for using UpdateTiles to Tex3D resources"));
    ADD_TEST_AND_DESC(_T("Texture3D/Buffers/Raw"), CRawTestTex3D, _T("Test cases for loading from tiled raw buffers using Tex3D resources")); // no added benefit over Texture2D tests
    ADD_TEST_AND_DESC(_T("Texture3D/Buffers/Structured"), CStructuredTestTex3D, _T("Test cases for loading from tiled structured buffers using Tex3D resources"));

    return rval;
}

void CTiledResourcesApp::InitOptionVariables()
{
    CD3D11TestFramework::InitOptionVariables();

    // Prevent command line from changing tiled resource behavior
    UnRegisterOptionVariable(_T("tiled"));
}

////////////////////////////////////////////////////////////////////////////////
//
// CTiledResourcesTest
//
////////////////////////////////////////////////////////////////////////////////
BEGIN_NAMED_VALUES(CTiledResourcesTest::RESIDENCY_PATTERN_1D)
    NAMED_VALUE(_T("FRAGMENTED_MULTI_TILE"), CTiledResourcesTest::RP1D_FRAGMENTED_MULTI_TILE)
    NAMED_VALUE(_T("REUSE_SINGLE_TILE"), CTiledResourcesTest::RP1D_REUSE_SINGLE_TILE)
END_NAMED_VALUES(CTiledResourcesTest::RESIDENCY_PATTERN_1D);

BEGIN_NAMED_VALUES(CTiledResourcesTest::RESIDENCY_PATTERN_2D_3D)
    NAMED_VALUE(_T("FULL"), CTiledResourcesTest::RP2D_3D_FULL)
    NAMED_VALUE(_T("FULL_MULTI_REGION"), CTiledResourcesTest::RP2D_3D_FULL_MULTI_REGION)
    NAMED_VALUE(_T("FULL_MIP_1"), CTiledResourcesTest::RP2D_3D_FULL_MIP_1)
    NAMED_VALUE(_T("FRAGMENTED_MULTI_TILE"), CTiledResourcesTest::RP2D_3D_FRAGMENTED_MULTI_TILE)
    NAMED_VALUE(_T("FRAGMENTED_MULTI_REGION"), CTiledResourcesTest::RP2D_3D_FRAGMENTED_MULTI_REGION)
    NAMED_VALUE(_T("FRAGMENTED_MULTI_TILE_SKIPS"), CTiledResourcesTest::RP2D_3D_FRAGMENTED_MULTI_TILE_SKIPS)
    NAMED_VALUE(_T("FRAGMENTED_REUSE_SINGLE_TILE"), CTiledResourcesTest::RP2D_3D_FRAGMENTED_REUSE_SINGLE_TILE)
    NAMED_VALUE(_T("FRAGMENTED_MIP_1"), CTiledResourcesTest::RP2D_3D_FRAGMENTED_MIP_1)
    NAMED_VALUE(_T("UNIFORM_BOX"), CTiledResourcesTest::RP2D_3D_UNIFORM_BOX)
    NAMED_VALUE(_T("CLEAR_MIP_0"), CTiledResourcesTest::RP2D_3D_CLEAR_MIP_0)
    NAMED_VALUE(_T("CLEAR_MIP_1"), CTiledResourcesTest::RP2D_3D_CLEAR_MIP_1)
    NAMED_VALUE(_T("CLEAR_ALL_MIPS"), CTiledResourcesTest::RP2D_3D_CLEAR_ALL_MIPS)
END_NAMED_VALUES(CTiledResourcesTest::RESIDENCY_PATTERN_2D_3D);

BEGIN_NAMED_VALUES(CTiledResourcesTest::COPY_TILE_MAPPINGS_PATTERN)
    NAMED_VALUE(_T("FULL"), CTiledResourcesTest::COPY_TILE_MAPPINGS_FULL)
    NAMED_VALUE(_T("PARTIAL"), CTiledResourcesTest::COPY_TILE_MAPPINGS_PARTIAL)
    NAMED_VALUE(_T("OVERLAPPED"), CTiledResourcesTest::COPY_TILE_MAPPINGS_OVERLAPPED)
END_NAMED_VALUES(CTiledResourcesTest::COPY_TILE_MAPPINGS_PATTERN)

BEGIN_NAMED_VALUES(CTiledResourcesTest::COPY_TILES_PATTERN)
    NAMED_VALUE(_T("FULL"), CTiledResourcesTest::COPY_TILES_FULL)
    NAMED_VALUE(_T("CENTER_BOX"), CTiledResourcesTest::COPY_TILES_CENTER_BOX)
    NAMED_VALUE(_T("PARTIAL"), CTiledResourcesTest::COPY_TILES_PARTIAL)
    NAMED_VALUE(_T("MIP1"), CTiledResourcesTest::COPY_TILES_MIP1)
END_NAMED_VALUES(CTiledResourcesTest::COPY_TILES_PATTERN)

BEGIN_NAMED_VALUES(CTiledResourcesTest::MINMAX_FILTER)
    NAMED_VALUE(_T("MINIMUM_MIN_MAG_MIP_LINEAR"), CTiledResourcesTest::MMF_MINIMUM_MIN_MAG_MIP_LINEAR)
    NAMED_VALUE(_T("MAXIMUM_MIN_MAG_MIP_LINEAR"), CTiledResourcesTest::MMF_MAXIMUM_MIN_MAG_MIP_LINEAR)
END_NAMED_VALUES(CTiledResourcesTest::MINMAX_FILTER);

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesApp::CTiledResourcesTest()
////////////////////////////////////////
CTiledResourcesTest::CTiledResourcesTest() :
    m_blankTileIndex(0),
    m_pCopyTexVS(nullptr),
    m_pCopyTexTier1PS(nullptr),
    m_pCopyTexTier2PS(nullptr),
    m_pCopyTexVSTex3D(nullptr),
    m_pCopyTexGSTex3D(nullptr),
    m_pCopyTexTier1PSTex3D(nullptr),
    m_pCopyTexTier2PSTex3D(nullptr), 
    m_pNullSentinelTexture(nullptr)
{
    m_parameters.pTiledResourcesTier = nullptr;
}

TEST_RESULT CTiledResourcesTest::Setup()
{
    TEST_RESULT tr = CD3D11Test::Setup();
    HRESULT hr = S_OK;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    CTiledResourceHelper::CTilePoolManager *pTilePoolManager = GetFramework()->GetTilePoolManager();
    CHandle<ID3D11InfoQueue> pInfoQueue = nullptr;

    hr = pDevice->QueryInterface(__uuidof(pInfoQueue), (void **) &pInfoQueue);
    if(S_OK == hr)
    {
        // Disable some debug messages that are expected
        D3D11_MESSAGE_ID denyList[] =
        {
            D3D11_MESSAGE_ID_DUPLICATE_TILE_MAPPINGS_IN_COVERED_AREA,
            D3D11_MESSAGE_ID_TILE_MAPPINGS_IN_COVERED_AREA_DUPLICATED_OUTSIDE,
            D3D11_MESSAGE_ID_NULL_TILE_MAPPING_ACCESS_WARNING,
            D3D11_MESSAGE_ID_RESIZETILEPOOL_SHRINK_WITH_MAPPINGS_STILL_DEFINED_PAST_END,
            D3D11_MESSAGE_ID_DIRTY_TILE_MAPPING_ACCESS
        };

        D3D11_INFO_QUEUE_FILTER filter;
        ZeroMemory(&filter, sizeof(filter));
        filter.DenyList.NumIDs = _countof(denyList);
        filter.DenyList.pIDList = denyList;
        pInfoQueue->AddRetrievalFilterEntries(&filter);
        pInfoQueue->AddStorageFilterEntries(&filter);
    }

    if(tr != RESULT_PASS)
        return tr;

    if(pDevice == nullptr)
    {
        WriteToLog(_T("Skipping due to lack of ID3D11Device2 interface"));
        return RESULT_SKIP;
    }
    
    // Ensure device supports tiled resources and record Tier
    D3D11_FEATURE_DATA_D3D11_OPTIONS2 featureData;
    pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS2, &featureData, sizeof(featureData));

    // Skip if tiled resources is not supported
    if(featureData.TiledResourcesTier == D3D11_TILED_RESOURCES_NOT_SUPPORTED)
    {
        WriteToLog(_T("Skipping due to lack of tiled resources support"));
        return RESULT_SKIP;
    }

    if (featureData.TiledResourcesTier == D3D11_TILED_RESOURCES_TIER_3)
        m_maxSupportedTiledResourcesTier = 3;
    else
    if(featureData.TiledResourcesTier == D3D11_TILED_RESOURCES_TIER_2)
        m_maxSupportedTiledResourcesTier = 2;
    else
    if(featureData.TiledResourcesTier == D3D11_TILED_RESOURCES_TIER_1)
        m_maxSupportedTiledResourcesTier = 1;

    if(0 == m_maxSupportedTiledResourcesTier)
    {
        WriteToLog(_T("Devices reporting Tier-3 tiled resource support must also support Tier-2, and Tier-2 tiled resource support must also support Tier-1"));
        return RESULT_FAIL;
    }

    if(1 < m_maxSupportedTiledResourcesTier)
    {
        // Create NULL tile sentinel texture
        // This texture is exactly 1-tile in size
        D3D11_TEXTURE2D_DESC texDesc;
        texDesc.ArraySize = 1;
        texDesc.MipLevels = 1;
        texDesc.CPUAccessFlags = 0;
        texDesc.Width = CResourceBaseTestHelper<TRB_TEXTURE_2D>::widthPerTile;
        texDesc.Height = CResourceBaseTestHelper<TRB_TEXTURE_2D>::heightPerTile;
        texDesc.MiscFlags = D3D11_RESOURCE_MISC_TILED;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.Format = DXGI_FORMAT_R32_UINT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        hr = pDevice->CreateTexture2D(&texDesc, NULL, &m_pNullSentinelTexture);
        if(FAILED(hr))
        {
            REPORT_HRESULT_FAILURE(hr, CreateTexture2D);
            return RESULT_FAIL;
        }
    }

    // Fetch tile index for 'blank' tile from the tile pool manager
    // Note this range is assocuated with a NULL resource
    m_blankTileIndex = pTilePoolManager->FetchRange(NULL, 1);

    return tr;
}

testfactor::RFactor CTiledResourcesTest::InitTiledResourcesTierParam(const bool skipTiers12)
{
    CreateParameter(_T("TiledResourcesTier"), &m_parameters.pTiledResourcesTier);

    // For all current tests, when using Texture3D resources, we need to skip
    // factors for Tiers 1 and 2
    if (skipTiers12)
    {
        m_parameters.pTiledResourcesTier->SetDefaultValue(3);
        return AddParameterValueSet(m_parameters.pTiledResourcesTier, new CUserValueSet<UINT>(3));
    }

    // Otherwise, return factor for tiers 1 and 2, since it makes no sense
    // to run duplicate tests for higher tiers which would replicate tier 2
    // support testing
    m_parameters.pTiledResourcesTier->SetDefaultValue(1);
    return AddParameterValueSet(m_parameters.pTiledResourcesTier, new CUserValueSet<UINT>(1, 2));
}

void CTiledResourcesTest::GetTileSize(DXGI_FORMAT format, _Out_ UINT &tileWidth, _Out_ UINT &tileHeight)
{
    UINT elementSize = CD3D10FormatHelper_Test::GetBitsPerElement(format);

    switch(format)
    {
    default:
        switch(elementSize)
        {
        case 8:
            tileWidth = 256;
            tileHeight = 256;
            break;
        case 16:
            tileWidth = 256;
            tileHeight = 128;
            break;
        case 32:
            tileWidth = 128;
            tileHeight = 128;
            break;
        case 64:
            tileWidth = 128;
            tileHeight = 64;
            break;
        case 128:
            tileWidth = 64;
            tileHeight = 64;
            break;
        default:
            // Not good
            throw(std::exception("Format dimensions cannot be tiled"));
            return;
        }
        break;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        tileWidth = 512;
        tileHeight = 256;
        break;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        tileWidth = 256;
        tileHeight = 256;
        break;
    }
}

UINT CTiledResourcesTest::GetBlankRects(_In_ const CTileMapping &tileMapping, _Out_ D3D11_RECT *pRects, DXGI_FORMAT format)
{
    UINT numRects = 0;
    UINT tileWidth = 0;
    UINT tileHeight = 0;

    GetTileSize(format, tileWidth, tileHeight);

    // Count the number of blank tiles
    for(UINT i = 0; i < tileMapping.Width; ++i)
    {
        for(UINT j = 0; j < tileMapping.Height; ++j)
        {
            if(UnmappedOrSkippedTile(tileMapping.Row[j].Column[i]))
            {
                pRects[numRects].left = tileWidth * i;
                pRects[numRects].top = tileHeight * j;
                pRects[numRects].right = pRects[numRects].left + tileWidth;
                pRects[numRects].bottom = pRects[numRects].top + tileHeight;
                numRects++;
            }
        }
    }

    return numRects;
}

void CTiledResourcesTest::ClearBlankRegions(_In_ const CTileMapping &tileMapping, _Out_ ID3D11View *pView, DXGI_FORMAT format)
{
    // In this test there can be no more than 16 blank rects since resources are 4x4 tile or less
    D3D11_RECT rects[ CResourceBaseTestHelper<TRB_TEXTURE_2D>::totalNumTiles ];
    UINT numRects = GetBlankRects(tileMapping, rects, format);

    if(numRects > 0)
    {
        const float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        GetImmediateContext2()->ClearView(pView, black, rects, numRects);
    }
}

void CTiledResourcesTest::GetTileSize(DXGI_FORMAT format, _Out_ UINT &tileWidth, _Out_ UINT &tileHeight, _Out_ UINT &tileDepth)
{
    UINT elementSize = CD3D10FormatHelper_Test::GetBitsPerElement(format);

    switch (format)
    {
    default:
        switch (elementSize)
        {
        case 8:
            tileWidth = 64;
            tileHeight = 32;
            tileDepth = 32;
            break;
        case 16:
            tileWidth = 32;
            tileHeight = 32;
            tileDepth = 32;
            break;
        case 32:
            tileWidth = 32;
            tileHeight = 32;
            tileDepth = 16;
            break;
        case 64:
            tileWidth = 32;
            tileHeight = 16;
            tileDepth = 16;
            break;
        case 128:
            tileWidth = 16;
            tileHeight = 16;
            tileDepth = 16;
            break;
        default:
            // Not good
            throw(std::exception("Format dimensions cannot be tiled"));
            return;
        }
        break;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        tileWidth = 128;
        tileHeight = 64;
        tileDepth = 16;
        break;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        tileWidth = 64;
        tileHeight = 64;
        tileDepth = 16;
        break;
    }
}

UINT CTiledResourcesTest::GetBlankBoxes(_In_ const CVolumeTileMapping &tileMapping, _Out_ D3D11_BOX *pBoxes, DXGI_FORMAT format)
{
    UINT numBoxes = 0;
    UINT tileWidth = 0;
    UINT tileHeight = 0;
    UINT tileDepth = 0;

    GetTileSize(format, tileWidth, tileHeight, tileDepth);

    // Count the number of blank tiles
    for (UINT i = 0; i < tileMapping.Width; ++i)
    {
        for (UINT j = 0; j < tileMapping.Height; ++j)
        {
            for (UINT k = 0; k < tileMapping.Depth; ++k)
            {
                if (UnmappedOrSkippedTile(tileMapping.Slice[k].Row[j].Column[i]))
                {
                    pBoxes[numBoxes].left = tileWidth * i;
                    pBoxes[numBoxes].top = tileHeight * j;
                    pBoxes[numBoxes].front = tileDepth * k;
                    pBoxes[numBoxes].right = pBoxes[numBoxes].left + tileWidth;
                    pBoxes[numBoxes].bottom = pBoxes[numBoxes].top + tileHeight;
                    pBoxes[numBoxes].back = pBoxes[numBoxes].front + tileDepth;
                    numBoxes++;
                }
            }
        }
    }

    return numBoxes;
}

void CTiledResourcesTest::ClearBlankRegions(_In_ const CVolumeTileMapping &tileMapping, _In_ ID3D11View *pView, DXGI_FORMAT format, const UINT mip)
{
    // In this test there can be no more than 64 blank rects since resources are 4x4x4 tile or less
    D3D11_BOX boxes[ CResourceBaseTestHelper<TRB_TEXTURE_3D>::totalNumTiles ];
    UINT numBoxes = GetBlankBoxes(tileMapping, boxes, format);

    const float black[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (numBoxes > 0) // ClearViewTex3D clears the whole subresource if numBoxes == 0
    {
        ClearViewTex3D(pView, format, black, boxes, numBoxes, mip);
    }
}

HRESULT CTiledResourcesTest::ClearViewTex3D(_In_ ID3D11View *pView, DXGI_FORMAT format, _In_ const float *pColor, _In_reads_opt_(numBoxes) const D3D11_BOX *pBox, const UINT numBoxes, const UINT mip)
{

    // Since there's no equivalent to ClearView when using views derived
    // from Texture3D, we have to get clever:
    //
    // The plan is to generate a whole bunch of render target views
    // (one per box), and pretend that texture3D is really an array
    // of Texture2Ds. Then we can use ClearView on that instead
    //
    // **NOTE**: ClearViewTex3D behaves as ClearView does, where passing no Boxes (or rects) means clear the entire subresource

    CHandle<ID3D11Resource> pResource;
    D3D11_RESOURCE_DIMENSION pResourceDim;
    pView->GetResource(&pResource);
    pResource->GetType(&pResourceDim);

    assert(D3D11_RESOURCE_DIMENSION_TEXTURE3D == pResourceDim);
    if (D3D11_RESOURCE_DIMENSION_TEXTURE3D != pResourceDim)
    {
        return E_FAIL;
    }

    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();

    if (numBoxes > 0) // Clear based on boxes
    {
        for (UINT box = 0; box < numBoxes; ++box)
        {
            CHandle<ID3D11RenderTargetView> pRTVBox;
            D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
            viewDesc.Format = format;
            viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
            viewDesc.Texture3D.MipSlice = mip;
            viewDesc.Texture3D.FirstWSlice = (pBox ? pBox[box].front : 0);
            viewDesc.Texture3D.WSize = (pBox ? pBox[box].back - pBox[box].front : -1);

            HRESULT hr = pDevice->CreateRenderTargetView(pResource, &viewDesc, &pRTVBox);
            if (FAILED(hr))
            {
                REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
                return hr;
            }

            if (pBox)
            {
                D3D11_RECT rectToClear;
                rectToClear.left = pBox[box].left;
                rectToClear.top = pBox[box].top;
                rectToClear.right = pBox[box].right;
                rectToClear.bottom = pBox[box].bottom;

                pContext->ClearView(pRTVBox, pColor, &rectToClear, 1);
            }
            else
            {
                pContext->ClearView(pRTVBox, pColor, nullptr, 0);
            }
        }
    }
    else // Clear the whole view
    {
        pContext->ClearView(pView, pColor, nullptr, 0);
    }

    return S_OK;
}

/* static */ void CTiledResourcesTest::GetShaderByteCode(LOADABLE_SHADER_BYTECODE_TYPE shaderType, _Out_ const BYTE **ppShaderCode, _Out_ SIZE_T *pShaderCodeLength, const bool getTiledVersion, const bool getTexture3DVersion)
{
    if (!ppShaderCode || !pShaderCodeLength)
    {
        return;
    }
    *ppShaderCode = NULL;
    *pShaderCodeLength = 0;

    // Assigns shader byte code and length to appropriate variables
    // based on whether we are using Texture2D or Texture3D, and whether requested shader is normal or tiled
    #define AssignShaderCodeAndLength(_normal2D, _tiled2D, _normal3D, _tiled3D)         \
    {                                                                                   \
        if (getTiledVersion)                                                            \
        {                                                                               \
            if (getTexture3DVersion)                                                    \
            {                                                                           \
                *ppShaderCode = ##_tiled3D;                                             \
                *pShaderCodeLength = (##_tiled3D ? sizeof(##_tiled3D) : 0);             \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                *ppShaderCode = ##_tiled2D;                                             \
                *pShaderCodeLength = (##_tiled2D ? sizeof(##_tiled2D) : 0);             \
            }                                                                           \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            if (getTexture3DVersion)                                                    \
            {                                                                           \
                *ppShaderCode = ##_normal3D;                                            \
                *pShaderCodeLength = (##_normal3D ? sizeof(##_normal3D) : 0);           \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                *ppShaderCode = ##_normal2D;                                            \
                *pShaderCodeLength = (##_normal2D ? sizeof(##_normal2D) : 0);           \
            }                                                                           \
        }                                                                               \
    }                                                                                   \


    switch (shaderType)
    {
    case LSBT_COMPUTE_COPY_TEX:
        AssignShaderCodeAndLength(g_CopyTexCSNormal, g_CopyTexCSTiled, g_CopyTexCSNormalTex3D, g_CopyTexCSTiledTex3D);
        break;
    case LSBT_COMPUTE_PROCESS_RAW_BUFFER:
        AssignShaderCodeAndLength(g_ProcessRawBufferNormal, g_ProcessRawBufferTiled, g_ProcessRawBufferNormalTex3D, g_ProcessRawBufferTiledTex3D);
        break;
    case LSBT_COMPUTE_PROCESS_STUCTURED_BUFFER:
        AssignShaderCodeAndLength(g_ProcessStructuredBufferNormal, g_ProcessStructuredBufferTiled, g_ProcessStructuredBufferNormalTex3D, g_ProcessStructuredBufferTiledTex3D);
        break;
    case LSBT_PIXEL_COPY_TEX_TIER_1:
        AssignShaderCodeAndLength(g_CopyTexTier1PS, g_CopyTexTier1PS, nullptr, nullptr);
        break;
    case LSBT_PIXEL_COPY_TEX_TIER_2:
        AssignShaderCodeAndLength(g_CopyTexTier2PS, g_CopyTexTier2PS, g_CopyTexTier2PSTex3D, g_CopyTexTier2PSTex3D);
        break;
    case LSBT_PIXEL_RENDER_DEPTH:
        AssignShaderCodeAndLength(g_RenderDepthPS, nullptr, nullptr, nullptr);
        break;
    case LSBT_PIXEL_RENDER_COLOR_USING_DEPTH:
        AssignShaderCodeAndLength(g_RenderColorUsingDepthPS, nullptr, nullptr, nullptr);
        break;
    case LSBT_PIXEL_FILTER_TIER_1:
        AssignShaderCodeAndLength(g_FilterNormalTier1PS, g_FilterTiledTier1PS, nullptr, nullptr);
        break;
    case LSBT_PIXEL_FILTER_TIER_2:
        AssignShaderCodeAndLength(g_FilterNormalTier2PS, g_FilterTiledTier2PS, g_FilterNormalTier2PSTex3D, g_FilterTiledTier2PSTex3D);
        break;
    case LSBT_PIXEL_GATHER_TIER_1:
        AssignShaderCodeAndLength(g_GatherNormalTier1PS, g_GatherTiledTier1PS, nullptr, nullptr);
        break;
    case LSBT_PIXEL_GATHER_TIER_2:
        AssignShaderCodeAndLength(g_GatherNormalTier2PS, g_GatherTiledTier2PS, nullptr, nullptr);
        break;
    case LSBT_VERTEX_COPY_TEX:
        AssignShaderCodeAndLength(g_CopyTexVS, g_CopyTexVS, g_CopyTexVSTex3D, g_CopyTexVSTex3D);
        break;
    case LSBT_GEOMETRY_COPY_TEX_3D:
        // No 2D version (geometry shader chooses Z slice to render to)
        AssignShaderCodeAndLength(g_CopyTexGSTex3D, g_CopyTexGSTex3D, g_CopyTexGSTex3D, g_CopyTexGSTex3D);
        break;
    }
}

void CTiledResourcesTest::Cleanup()
{
    if(GetFramework()->GetTilePoolManager())
    {
        // Release ranges associated with NULL resource (blank tile)
        GetFramework()->GetTilePoolManager()->ReleaseRanges(NULL);
    }

    SAFE_RELEASE(m_pCopyTexVS);
    SAFE_RELEASE(m_pCopyTexTier1PS);
    SAFE_RELEASE(m_pCopyTexTier2PS);
    SAFE_RELEASE(m_pCopyTexVSTex3D);
    SAFE_RELEASE(m_pCopyTexGSTex3D);
    SAFE_RELEASE(m_pCopyTexTier1PSTex3D);
    SAFE_RELEASE(m_pCopyTexTier2PSTex3D);
    SAFE_RELEASE(m_pNullSentinelTexture);

    CD3D11Test::Cleanup();
}

TEST_RESULT CTiledResourcesTest::SetupTestCase()
{
    if(m_maxSupportedTiledResourcesTier < m_parameters.pTiledResourcesTier->GetValue())
        return RESULT_SKIP;

    return RESULT_PASS;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CreateTexture()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::CreateTexture(const D3D11_TEXTURE2D_DESC *pResourceDesc, ID3D11Texture2D **ppResource)
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    ID3D11Device2 *pDevice = GetFramework()->GetDevice2();
    CHandle<ID3D11Texture2D> pResource;

    hr = pDevice->CreateTexture2D(pResourceDesc, NULL, &pResource);
    if(hr != S_OK)
    {
        REPORT_HRESULT_FAILURE(hr, CreateTexture2D);
        return RESULT_FAIL;
    }

    *ppResource = pResource;
    pResource->AddRef();

    return tr;
}


////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CreateTexture()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::CreateTexture(const D3D11_TEXTURE3D_DESC *pResourceDesc, ID3D11Texture3D **ppResource)
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    ID3D11Device2 *pDevice = GetFramework()->GetDevice2();
    CHandle<ID3D11Texture3D> pResource;

    hr = pDevice->CreateTexture3D(pResourceDesc, NULL, &pResource);
    if (hr != S_OK)
    {
        REPORT_HRESULT_FAILURE(hr, CreateTexture3D);
        return RESULT_FAIL;
    }

    *ppResource = pResource;
    pResource->AddRef();

    return tr;
}


////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CreateBuffer()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::CreateBuffer(const D3D11_BUFFER_DESC *pResourceDesc, ID3D11Buffer **ppResource)
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    ID3D11Device2 *pDevice = GetFramework()->GetDevice2();
    CHandle<ID3D11Buffer> pResource;

    hr = pDevice->CreateBuffer(pResourceDesc, NULL, &pResource);
    if(hr != S_OK)
    {
        REPORT_HRESULT_FAILURE(hr, CreateBuffer);
        return RESULT_FAIL;
    }

    *ppResource = pResource;
    pResource->AddRef();

    return tr;
}


////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CreateTiledTexture()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::CreateTiledTexture(const UINT width, const UINT height, const UINT unused, ID3D11Texture2D **ppTiledResource, UINT additionalBindFlags)
{
    TEST_RESULT tr = RESULT_PASS;
    D3D11_TEXTURE2D_DESC resourceDesc;
    resourceDesc.ArraySize = FIXED_ARRAY_SIZE;
    resourceDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | additionalBindFlags;
    resourceDesc.CPUAccessFlags = 0;
    resourceDesc.Format = FIXED_FORMAT_TEX2D;
    resourceDesc.MipLevels = 0; // Generate full set
    resourceDesc.MiscFlags = D3D11_RESOURCE_MISC_TILED;
    resourceDesc.Usage = D3D11_USAGE_DEFAULT;
    resourceDesc.Width = width;
    resourceDesc.Height = height;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;

    return CreateCustomTiledTexture(&resourceDesc, ppTiledResource); 
};

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CreateTiledTexture()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::CreateTiledTexture(const UINT width, const UINT height, const UINT depth, ID3D11Texture3D **ppTiledResource, UINT additionalBindFlags)
{
    TEST_RESULT tr = RESULT_PASS;
    D3D11_TEXTURE3D_DESC resourceDesc;
    resourceDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | additionalBindFlags;
    resourceDesc.CPUAccessFlags = 0;
    resourceDesc.Format = FIXED_FORMAT_TEX3D;
    resourceDesc.MipLevels = 0; // Generate full set
    resourceDesc.MiscFlags = D3D11_RESOURCE_MISC_TILED;
    resourceDesc.Usage = D3D11_USAGE_DEFAULT;
    resourceDesc.Width = width;
    resourceDesc.Height = height;
    resourceDesc.Depth = depth;

    return CreateCustomTiledTexture(&resourceDesc, ppTiledResource);
};

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CreateTiledBuffer()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::CreateTiledBuffer(UINT byteWidth, ID3D11Buffer **ppTiledResource)
{
    TEST_RESULT tr = RESULT_PASS;
    D3D11_BUFFER_DESC tilePoolDesc;

    tilePoolDesc.BindFlags = 0;
    tilePoolDesc.ByteWidth = byteWidth;
    tilePoolDesc.CPUAccessFlags = 0;
    tilePoolDesc.MiscFlags = D3D11_RESOURCE_MISC_TILE_POOL;
    tilePoolDesc.Usage = D3D11_USAGE_DEFAULT;

    tr = CreateBuffer(&tilePoolDesc, ppTiledResource);

    return tr;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CreateCustomTiledTexture()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::CreateCustomTiledTexture(_In_ const D3D11_TEXTURE2D_DESC *pDesc, _Out_ ID3D11Texture2D **ppTiledResource)
{
    return CreateCustomTiledTextureHelper < D3D11_TEXTURE2D_DESC, ID3D11Texture2D > (pDesc, ppTiledResource);
}

TEST_RESULT CTiledResourcesTest::CreateCustomTiledTexture(_In_ const D3D11_TEXTURE3D_DESC *pDesc, _Out_ ID3D11Texture3D **ppTiledResource)
{
    return CreateCustomTiledTextureHelper < D3D11_TEXTURE3D_DESC, ID3D11Texture3D > (pDesc, ppTiledResource);
}

template <class D3D11_TEXTURE_DESC_TYPE, class ID3D11_TEXTURE_TYPE>
TEST_RESULT CTiledResourcesTest::CreateCustomTiledTextureHelper(_In_ const D3D11_TEXTURE_DESC_TYPE *pDesc, _Out_ ID3D11_TEXTURE_TYPE **ppTiledResource)
{
    TEST_RESULT tr = CreateTexture(pDesc, ppTiledResource);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // If currently using tier-1 then populate all logical tiles with the 'blank' tile
    if (m_parameters.pTiledResourcesTier == NULL || m_parameters.pTiledResourcesTier->GetValue() == 1)
    {
        MapEntireTextureToSingleTile(*ppTiledResource, m_blankTileIndex, GetFramework()->GetTilePoolManager()->GetTilePool());
    }

    return tr;
}


////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CreateStagingTextureHelper()
////////////////////////////////////////
template <bool isTexture3D, class ID3D11_TEXTURE_TYPE>
TEST_RESULT CTiledResourcesTest::CreateStagingTextureHelper(
    UINT width,
    UINT height,
    UINT depth,
    ID3D11_TEXTURE_TYPE **ppStagingResource,
    bool isSource,
    const UINT widthPerTile,
    const UINT heightPerTile,
    const UINT depthPerTile,
    const std::function<void(R8G8B8A8_PIXEL **pixel, const UINT i, const UINT j, const UINT k, const UINT a, const UINT b, const UINT c, const UINT count)> &PerPixelFunction
    )
{
    if (!PerPixelFunction)
    {
        return RESULT_FAIL;
    }

    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    const TILED_RESOURCE_BASE tiledResourceBase = (isTexture3D ? TRB_TEXTURE_3D : TRB_TEXTURE_2D);
    CResourceBaseTestHelper<tiledResourceBase>::CD3D11_TEXTURE_TYPE_DESC resourceDesc;
    CResourceBaseTestHelper<tiledResourceBase>::GenerateTextureDesc(
        resourceDesc,
        (isTexture3D ? FIXED_FORMAT_TEX3D : FIXED_FORMAT_TEX2D),    // format
        width,
        height,
        (isTexture3D ? depth : FIXED_ARRAY_SIZE),                   // arraySize (Texture2D) or depth (Texture3D)
        0,                                                          // mipLevels: 0 means generate full set
        0,                                                          // bindFlags
        D3D11_USAGE_STAGING,                                        // usage
        (isSource ? D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE : D3D11_CPU_ACCESS_READ),    // cpuaccessFlags
        0,                                                          // miscFlags
        1,                                                          // sampleCount (ignored for Texture3D)
        0                                                           // sampleQuality (ignored for Texture3D)
        );

    tr = CreateTexture(&resourceDesc, ppStagingResource);
    if (RESULT_PASS != tr)
    {
        return RESULT_FAIL;
    }

    if (isSource)
    {
        // For each subresource
        for (UINT subResource = 0; ; ++subResource)
        {
            const UINT widthInTiles = 1 + (width - 1) / widthPerTile;
            const UINT heightInTiles = 1 + (height - 1) / heightPerTile;
            const UINT depthInTiles = 1 + (depth - 1) / depthPerTile;

            // Initialize data
            ID3D11_TEXTURE_TYPE *pStagingResource = *ppStagingResource;
            D3D11_MAPPED_SUBRESOURCE map;
            hr = GetImmediateContext1()->Map(pStagingResource, subResource, D3D11_MAP_WRITE, 0, &map);
            if (FAILED(hr))
            {
                REPORT_HRESULT_FAILURE(hr, Map);
                tr = RESULT_FAIL;
            }

            UINT count = 0;

            // For each tile depth
            for (UINT k = 0; k < depthInTiles; ++k)
            {
                // For each tile row
                for (UINT j = 0; j < widthInTiles; ++j)
                {
                    // For each tile column
                    for (UINT i = 0; i < heightInTiles; ++i)
                    {
                        // For each pixel slice
                        for (UINT c = 0; c < depthPerTile; ++c)
                        {
                            UINT z = c + k * depthPerTile;
                            if (z >= depth)
                                break;

                            // For each pixel row
                            for (UINT b = 0; b < heightPerTile; ++b)
                            {
                                UINT y = b + j * heightPerTile;
                                if (y >= height)
                                    break;

                                // For each pixel column
                                for (UINT a = 0; a < widthPerTile; ++a)
                                {
                                    UINT x = a + i * widthPerTile;
                                    if (x >= width)
                                        break;

                                    R8G8B8A8_PIXEL *pixel = x + reinterpret_cast<R8G8B8A8_PIXEL *>(reinterpret_cast<BYTE *>(map.pData) + z * map.DepthPitch + y * map.RowPitch);
                                    PerPixelFunction(&pixel, i, j, k, a, b, c, count);
                                }
                            }
                        }

                        count++;
                    }
                }
            }

            GetImmediateContext1()->Unmap(pStagingResource, subResource);
            if (width == 1 &&
                height == 1 &&
                ((isTexture3D && depth == 1) || !isTexture3D)
                )
            {
                break; // already the smallest subresource (consider depth == 1 only when using Texture3D)
            }

            width /= 2;
            height /= 2;
            depth /= 2;

            // Adjust for degenerate mips
            if (height == 0)
                height = 1;
            if (width == 0)
                width = 1;
            if (depth == 0)
                depth = 1;
        }
    }

    return tr;
}

TEST_RESULT CTiledResourcesTest::CreateStagingTex2D(UINT width, UINT height, _Out_ ID3D11Texture2D **ppStagingResource, bool isSource)
{
    return CreateStagingTextureHelper < false, ID3D11Texture2D > (
        width,
        height,
        1,
        ppStagingResource,
        isSource,
        CResourceBaseTestHelper<TRB_TEXTURE_2D>::widthPerTile,
        CResourceBaseTestHelper<TRB_TEXTURE_2D>::heightPerTile,
        1,
        PerPixelLambdaTilePattern2D
        );
}

TEST_RESULT CTiledResourcesTest::CreateStagingTex3D(UINT width, UINT height, UINT depth, _Out_ ID3D11Texture3D **ppStagingResource, bool isSource)
{
    return CreateStagingTextureHelper < true, ID3D11Texture3D > (
        width,
        height,
        depth,
        ppStagingResource,
        isSource,
        CResourceBaseTestHelper<TRB_TEXTURE_3D>::widthPerTile,
        CResourceBaseTestHelper<TRB_TEXTURE_3D>::heightPerTile,
        CResourceBaseTestHelper<TRB_TEXTURE_3D>::depthPerTile,
        PerPixelLambdaTilePattern3D
        );
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CreateStagingBuffer()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::CreateStagingBuffer(UINT byteWidth, ID3D11Buffer **ppStagingResource, bool isSource)
{
    TEST_RESULT tr = RESULT_PASS;
    D3D11_BUFFER_DESC bufferDesc;

    bufferDesc.BindFlags = 0;
    bufferDesc.ByteWidth = byteWidth;
    bufferDesc.CPUAccessFlags = isSource ? D3D11_CPU_ACCESS_WRITE : D3D11_CPU_ACCESS_READ;
    bufferDesc.MiscFlags = 0;
    bufferDesc.Usage = D3D11_USAGE_STAGING;

    tr = CreateBuffer(&bufferDesc, ppStagingResource);

    return tr;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::InitTiledTex2D()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::UpdateTileMappingsTex2D(ID3D11Texture2D *pTiledResource, UINT tilePoolBaseOffset, ID3D11Buffer *pTilePool, RESIDENCY_PATTERN_2D_3D pattern, CTileMapping &tileMapping, bool useNoOverwrite)
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    ID3D11Device2 *pDevice = GetFramework()->GetDevice2();
    ID3D11DeviceContext2 *pContext = GetFramework()->GetImmediateContext2();

    D3D11_TILED_RESOURCE_COORDINATE tileRegionStartCoord;
    D3D11_TILE_REGION_SIZE tileRegionSize;
    std::vector<D3D11_TILED_RESOURCE_COORDINATE> tileRegionStartCoords;
    std::vector<D3D11_TILE_REGION_SIZE> tileRegionSizes;
    UINT numRanges;
    UINT tilePoolRangeStartOffsets[MAX_NUM_TILE_POOL_RANGES];
    UINT tilePoolRangeFlags[MAX_NUM_TILE_POOL_RANGES];
    UINT tilePoolRangeTileCounts[MAX_NUM_TILE_POOL_RANGES];
    UINT updateTileMappingsFlags;
    const char *mappingStrings[4] = {NULL};
    UINT *pTilePoolRangeStartOffsets = NULL;
    UINT *pTilePoolRangeFlags = NULL;
    UINT *pTilePoolRangeTileCounts = NULL;

    // TILE POOL LAYOUT: 0123456789ABCDEF
    // Pattern names use the above layout to indicate how tiles are mapped in a tiled resource.
    // On Tier-1 an 'X' in the pattern indicates a NULL mapped tile
    // On Tier-2 an 'X' in the pattern indicates a map to a default shared tile (index 16)
    // On Tier-3 an 'X' in the pattern indicates a map to a default shared tile (index 16)

    // Initialize potentially unused values to 0
    tileRegionStartCoord.X = 0;
    tileRegionStartCoord.Y = 0;
    tileRegionStartCoord.Z = 0;

    // Initialize width, height, and depth to 1
    tileRegionSize.Width = 1;
    tileRegionSize.Height = 1;
    tileRegionSize.Depth = 1;

    D3D11_PACKED_MIP_DESC packedMipDesc;
    D3D11_SUBRESOURCE_TILING nonPackedSubresourceTiling;

    switch(pattern)
    {
    case RP2D_3D_FULL:
        mappingStrings[0] = "0123";
        mappingStrings[1] = "4567";
        mappingStrings[2] = "89AB";
        mappingStrings[3] = "CDEF";
        tileMapping.SetFromStrings(mappingStrings);

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 16;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 1;
        tilePoolRangeStartOffsets[0] = 0x0 + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = 0;
        tilePoolRangeTileCounts[0] = 16;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_FULL_MULTI_REGION:
        mappingStrings[0] = "0123";
        mappingStrings[1] = "4567";
        mappingStrings[2] = "89AB";
        mappingStrings[3] = "CDEF";
        tileMapping.SetFromStrings(mappingStrings);

        // First region
        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 8;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        // Second region
        tileRegionSize.bUseBox = true;
        tileRegionSize.NumTiles = 8;
        tileRegionSize.Height = 2;
        tileRegionSize.Width = 4;
        tileRegionSize.Depth = 1;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoord.X = 0;
        tileRegionStartCoord.Y = 2;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 1;
        tilePoolRangeStartOffsets[0] = 0x0 + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = 0;
        tilePoolRangeTileCounts[0] = 16;
        
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_FULL_MIP_1:
        mappingStrings[0] = "01";
        mappingStrings[1] = "23";
        mappingStrings[2] = nullptr;
        tileMapping.SetFromStrings(mappingStrings);

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 4;
        tileRegionStartCoord.X = 0;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Subresource = 1;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 1;
        tilePoolRangeStartOffsets[0] = 0x0 + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = 0;
        tilePoolRangeTileCounts[0] = 4;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_FRAGMENTED_MIP_1:
        mappingStrings[0] = "s1";
        mappingStrings[1] = "2s";
        mappingStrings[2] = nullptr;
        tileMapping.SetFromStrings(mappingStrings);

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 2;
        tileRegionStartCoord.X = 1;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Subresource = 1;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 1;
        tilePoolRangeStartOffsets[0] = 0x1 + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = 0;
        tilePoolRangeTileCounts[0] = 2;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_FRAGMENTED_MULTI_TILE:
        mappingStrings[0] = "s123";
        mappingStrings[1] = "BCxx";
        mappingStrings[2] = "x9xx";
        mappingStrings[3] = "xx78";
        tileMapping.SetFromStrings(mappingStrings);

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 15;
        tileRegionStartCoord.X = 1;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 6;
        tilePoolRangeStartOffsets[0] = 0x1 + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[1] = 0xB + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[2] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[3] = 0x9 + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[4] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[5] = 0x7 + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = 0;
        tilePoolRangeFlags[1] = 0;
        tilePoolRangeFlags[2] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeFlags[3] = 0;
        tilePoolRangeFlags[4] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeFlags[5] = 0;
        tilePoolRangeTileCounts[0] = 3;
        tilePoolRangeTileCounts[1] = 2;
        tilePoolRangeTileCounts[2] = 3;
        tilePoolRangeTileCounts[3] = 1;
        tilePoolRangeTileCounts[4] = 4;
        tilePoolRangeTileCounts[5] = 2;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_FRAGMENTED_MULTI_REGION:
        mappingStrings[0] = "s123";
        mappingStrings[1] = "BCss";
        mappingStrings[2] = "s9xx";
        mappingStrings[3] = "xx78";
        tileMapping.SetFromStrings(mappingStrings);

        // First region
        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 5;
        tileRegionStartCoord.X = 1;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        // Second region
        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 7;
        tileRegionStartCoord.X = 1;
        tileRegionStartCoord.Y = 2;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 5;
        tilePoolRangeStartOffsets[0] = 0x1 + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[1] = 0xB + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[2] = 0x9 + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[3] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[4] = 0x7 + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = 0;
        tilePoolRangeFlags[1] = 0;
        tilePoolRangeFlags[2] = 0;
        tilePoolRangeFlags[3] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeFlags[4] = 0;
        tilePoolRangeTileCounts[0] = 3;
        tilePoolRangeTileCounts[1] = 2;
        tilePoolRangeTileCounts[2] = 1;
        tilePoolRangeTileCounts[3] = 4;
        tilePoolRangeTileCounts[4] = 2;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_FRAGMENTED_MULTI_TILE_SKIPS:
        mappingStrings[0] = "s123";
        mappingStrings[1] = "BCss";
        mappingStrings[2] = "s9ss";
        mappingStrings[3] = "ss78";
        tileMapping.SetFromStrings(mappingStrings);

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 15;
        tileRegionStartCoord.X = 1;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 6;
        tilePoolRangeStartOffsets[0] = 0x1 + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[1] = 0xB + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[2] = m_blankTileIndex;
        tilePoolRangeStartOffsets[3] = 0x9 + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[4] = m_blankTileIndex;
        tilePoolRangeStartOffsets[5] = 0x7 + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = 0;
        tilePoolRangeFlags[1] = 0;
        tilePoolRangeFlags[2] = D3D11_TILE_RANGE_SKIP;
        tilePoolRangeFlags[3] = 0;
        tilePoolRangeFlags[4] = D3D11_TILE_RANGE_SKIP;
        tilePoolRangeFlags[5] = 0;
        tilePoolRangeTileCounts[0] = 3;
        tilePoolRangeTileCounts[1] = 2;
        tilePoolRangeTileCounts[2] = 3;
        tilePoolRangeTileCounts[3] = 1;
        tilePoolRangeTileCounts[4] = 4;
        tilePoolRangeTileCounts[5] = 2;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_FRAGMENTED_REUSE_SINGLE_TILE:
        mappingStrings[0] = "ssss";
        mappingStrings[1] = "s222";
        mappingStrings[2] = "s222";
        mappingStrings[3] = "s222";
        tileMapping.SetFromStrings(mappingStrings);
        tileRegionSize.bUseBox = true;
        tileRegionSize.NumTiles = 9;
        tileRegionSize.Width = 3;
        tileRegionSize.Height = 3;
        tileRegionStartCoord.X = 1;
        tileRegionStartCoord.Y = 1;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 1;
        tilePoolRangeStartOffsets[0] = 0x2 + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = D3D11_TILE_RANGE_REUSE_SINGLE_TILE;
        tilePoolRangeTileCounts[0] = 9;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_UNIFORM_BOX:
        mappingStrings[0] = "ssss";
        mappingStrings[1] = "567s";
        mappingStrings[2] = "9ABs";
        mappingStrings[3] = "CDEs";
        tileMapping.SetFromStrings(mappingStrings);
        tileRegionSize.bUseBox = true;
        tileRegionSize.NumTiles = 9;
        tileRegionSize.Width = 3;
        tileRegionSize.Height = 3;
        tileRegionStartCoord.X = 0;
        tileRegionStartCoord.Y = 1;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 2;
        tilePoolRangeStartOffsets[0] = 0x5 + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[1] = 0x9 + tilePoolBaseOffset;
        tilePoolRangeTileCounts[0] = 3;
        tilePoolRangeTileCounts[1] = 6;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = nullptr;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_CLEAR_MIP_0:
        mappingStrings[0] = "xxxx";
        mappingStrings[1] = "xxxx";
        mappingStrings[2] = "xxxx";
        mappingStrings[3] = "xxxx";
        tileMapping.SetFromStrings(mappingStrings);
        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 16;
        tileRegionSize.Width = 0;
        tileRegionSize.Height = 0;
        tileRegionStartCoord.X = 0;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 1;
        tilePoolRangeFlags[0] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeStartOffsets[0] = m_blankTileIndex;
        tilePoolRangeTileCounts[0] = 16;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_CLEAR_MIP_1:
        mappingStrings[0] = "xx";
        mappingStrings[1] = "xx";
        mappingStrings[2] = nullptr;
        tileMapping.SetFromStrings(mappingStrings);
        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 4;
        tileRegionSize.Width = 0;
        tileRegionSize.Height = 0;
        tileRegionStartCoord.X = 0;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Subresource = 1;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 1;
        tilePoolRangeFlags[0] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeStartOffsets[0] = m_blankTileIndex;
        tilePoolRangeTileCounts[0] = 4;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_CLEAR_ALL_MIPS:
        mappingStrings[0] = "xxxx";
        mappingStrings[1] = "xxxx";
        mappingStrings[2] = "xxxx";
        mappingStrings[3] = "xxxx";
        tileMapping.SetFromStrings(mappingStrings);
        pDevice->GetResourceTiling(
            pTiledResource, 
            &tileRegionSize.NumTiles, 
            &packedMipDesc,
            nullptr,
            nullptr,
            0,
            &nonPackedSubresourceTiling
            );

        tileRegionSize.bUseBox = false;
        tileRegionStartCoord.X = 0;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        numRanges = 1;
        tilePoolRangeTileCounts[0] = tileRegionSize.NumTiles;
        tilePoolRangeFlags[0] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeStartOffsets[0] = m_blankTileIndex;
    }

    hr = pContext->UpdateTileMappings(
        pTiledResource, 
        tileRegionStartCoords.size(),
        tileRegionStartCoords.data(), 
        tileRegionSizes.data(), 
        pTilePool, 
        numRanges, 
        pTilePoolRangeFlags,
        pTilePoolRangeStartOffsets, 
        pTilePoolRangeTileCounts, 
        updateTileMappingsFlags
        );

    if(FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, UpdateTileMappings);
        tr = RESULT_FAIL;
    }

    return tr;
}


////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CopyTileMappings()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::CopyTileMappings(ID3D11Texture2D *pDest, ID3D11Texture2D *pSrc, COPY_TILE_MAPPINGS_PATTERN pattern, CTileMapping &tileMapping)
{
    D3D11_TILED_RESOURCE_COORDINATE destRegionStartCoord;
    D3D11_TILED_RESOURCE_COORDINATE srcRegionStartCoord;
    D3D11_TILE_REGION_SIZE tileRegionSize;
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    D3D11_TILE_MAPPING_FLAG flags = D3D11_TILE_MAPPING_NO_OVERWRITE;
    int xform[4][4];
    CTileMapping newMapping;

    switch(pattern)
    {
    case COPY_TILE_MAPPINGS_FULL:
        // 0123    0123
        // 4567 -> 4567
        // 89AB    89AB
        // CDEF    CDEF
        xform[0][0] = 0;  xform[0][1] = 1;  xform[0][2] = 2;  xform[0][3] = 3;
        xform[1][0] = 4;  xform[1][1] = 5;  xform[1][2] = 6;  xform[1][3] = 7;
        xform[2][0] = 8;  xform[2][1] = 9;  xform[2][2] = 10; xform[2][3] = 11;
        xform[3][0] = 12; xform[3][1] = 13; xform[3][2] = 14; xform[3][3] = 15;
        destRegionStartCoord.X = 0;
        destRegionStartCoord.Y = 0;
        destRegionStartCoord.Z = 0;
        destRegionStartCoord.Subresource = 0;
        srcRegionStartCoord.X = 0;
        srcRegionStartCoord.Y = 0;
        srcRegionStartCoord.Z = 0;
        srcRegionStartCoord.Subresource = 0;
        tileRegionSize.bUseBox = true;
        tileRegionSize.Width = 4;
        tileRegionSize.Height = 4;
        tileRegionSize.Depth = 1;
        tileRegionSize.NumTiles = tileRegionSize.Width * tileRegionSize.Height;
        break;

    case COPY_TILE_MAPPINGS_OVERLAPPED:
        // 0123    0456
        // 4567 -> 489A
        // 89AB    89AB
        // CDEF    CDEF
        //xform[0][0] = 0;  xform[0][1] = 4;  xform[0][2] = 5;  xform[0][3] = 6;
        //xform[1][0] = 4;  xform[1][1] = 8;  xform[1][2] = 9;  xform[1][3] = 10;
        //xform[2][0] = 8;  xform[2][1] = 9;  xform[2][2] = 10; xform[2][3] = 11;
        //xform[3][0] = 12; xform[3][1] = 13; xform[3][2] = 14; xform[3][3] = 15;
        // 0123    0123
        // 4567 -> 4123
        // 89AB    8567
        // CDEF    CDEF
        xform[0][0] = 0;  xform[0][1] = 1;  xform[0][2] = 2;  xform[0][3] = 3;
        xform[1][0] = 4;  xform[1][1] = 1;  xform[1][2] = 2;  xform[1][3] = 3;
        xform[2][0] = 8;  xform[2][1] = 5;  xform[2][2] = 6;  xform[2][3] = 7;
        xform[3][0] = 12; xform[3][1] = 13; xform[3][2] = 14; xform[3][3] = 15;
        newMapping = tileMapping;

        // Change pDest to pSrc
        assert(pDest == pSrc);
        destRegionStartCoord.X = 1;
        destRegionStartCoord.Y = 1;
        destRegionStartCoord.Z = 0;
        srcRegionStartCoord.X = 1;
        srcRegionStartCoord.Y = 0;
        srcRegionStartCoord.Z = 0;
        destRegionStartCoord.Subresource = 0;
        srcRegionStartCoord.Subresource = 0;
        tileRegionSize.bUseBox = true;
        tileRegionSize.Width = 3;
        tileRegionSize.Height = 2;
        tileRegionSize.Depth = 1;
        tileRegionSize.NumTiles = tileRegionSize.Width * tileRegionSize.Height;
        break;

    case COPY_TILE_MAPPINGS_PARTIAL:
        // 0123     x456
        // 4567  ->	x89A
        // 89AB		xxxx
        // CDEF		xxxx
        xform[0][0] = 0xffffffff;	xform[0][1] = 4;			xform[0][2] = 5;			xform[0][3] = 6;
        xform[1][0] = 0xffffffff;	xform[1][1] = 8;			xform[1][2] = 9;			xform[1][3] = 10;
        xform[2][0] = 0xffffffff;	xform[2][1] = 0xffffffff;	xform[2][2] = 0xffffffff;	xform[2][3] = 0xffffffff;
        xform[3][0] = 0xffffffff;	xform[3][1] = 0xffffffff;	xform[3][2] = 0xffffffff;	xform[3][3] = 0xffffffff;

        destRegionStartCoord.X = 1;
        destRegionStartCoord.Y = 0;
        destRegionStartCoord.Z = 0;
        destRegionStartCoord.Subresource = 0;
        srcRegionStartCoord.X = 0;
        srcRegionStartCoord.Y = 1;
        srcRegionStartCoord.Z = 0;
        srcRegionStartCoord.Subresource = 0;
        tileRegionSize.bUseBox = true;
        tileRegionSize.Width = 3;
        tileRegionSize.Height = 2;
        tileRegionSize.Depth = 1;
        tileRegionSize.NumTiles = tileRegionSize.Width * tileRegionSize.Height;
        break;
    }

    tileMapping.Transform(newMapping, xform);
    tileMapping = newMapping;

    hr = pContext->CopyTileMappings(pDest, &destRegionStartCoord, pSrc, &srcRegionStartCoord, &tileRegionSize, flags);
    if(FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CopyTileMappings);
        return RESULT_FAIL;
    }

    return tr;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CopyTileMappings()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::CopyTileMappings(ID3D11Texture3D *pDest, ID3D11Texture3D *pSrc, COPY_TILE_MAPPINGS_PATTERN pattern, CVolumeTileMapping &tileMapping)
{
    D3D11_TILED_RESOURCE_COORDINATE destRegionStartCoord;
    D3D11_TILED_RESOURCE_COORDINATE srcRegionStartCoord;
    D3D11_TILE_REGION_SIZE tileRegionSize;
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    D3D11_TILE_MAPPING_FLAG flags = D3D11_TILE_MAPPING_NO_OVERWRITE;
    int xform[4][4][4];
    CVolumeTileMapping newMapping;

    switch (pattern)
    {
    case COPY_TILE_MAPPINGS_FULL:
        // Slice 0:  ABCD      ABCD
        //           EFGH  ->  EFGH
        //           IJKL      IJKL
        //           MNOP      MNOP
        //
        // Slice 1:  QRST      QRST
        //           UVWX  ->  UVWX
        //           YZab      YZab
        //           cdef      cdef
        //
        // Slice 2:  ghij      ghij
        //           klmn  ->  klmn
        //           opqr      opqr
        //           stuv      stuv
        // 
        // Slice 3:  wxyz      wxyz
        //           0123  ->  0123
        //           4567      4567
        //           89+/      89+/

        xform[0][0][0] = 0;   xform[0][0][1] = 1;   xform[0][0][2] = 2;   xform[0][0][3] = 3;
        xform[0][1][0] = 4;   xform[0][1][1] = 5;   xform[0][1][2] = 6;   xform[0][1][3] = 7;
        xform[0][2][0] = 8;   xform[0][2][1] = 9;   xform[0][2][2] = 10;  xform[0][2][3] = 11;
        xform[0][3][0] = 12;  xform[0][3][1] = 13;  xform[0][3][2] = 14;  xform[0][3][3] = 15;

        xform[1][0][0] = 16;  xform[1][0][1] = 17;  xform[1][0][2] = 18;  xform[1][0][3] = 19;
        xform[1][1][0] = 20;  xform[1][1][1] = 21;  xform[1][1][2] = 22;  xform[1][1][3] = 23;
        xform[1][2][0] = 24;  xform[1][2][1] = 25;  xform[1][2][2] = 26;  xform[1][2][3] = 27;
        xform[1][3][0] = 28;  xform[1][3][1] = 29;  xform[1][3][2] = 30;  xform[1][3][3] = 31;

        xform[2][0][0] = 32;  xform[2][0][1] = 33;  xform[2][0][2] = 34;  xform[2][0][3] = 35;
        xform[2][1][0] = 36;  xform[2][1][1] = 37;  xform[2][1][2] = 38;  xform[2][1][3] = 39;
        xform[2][2][0] = 40;  xform[2][2][1] = 41;  xform[2][2][2] = 42;  xform[2][2][3] = 43;
        xform[2][3][0] = 44;  xform[2][3][1] = 45;  xform[2][3][2] = 46;  xform[2][3][3] = 47;

        xform[3][0][0] = 48;  xform[3][0][1] = 49;  xform[3][0][2] = 50;  xform[3][0][3] = 51;
        xform[3][1][0] = 52;  xform[3][1][1] = 53;  xform[3][1][2] = 54;  xform[3][1][3] = 55;
        xform[3][2][0] = 56;  xform[3][2][1] = 57;  xform[3][2][2] = 58;  xform[3][2][3] = 59;
        xform[3][3][0] = 60;  xform[3][3][1] = 61;  xform[3][3][2] = 62;  xform[3][3][3] = 63;

        destRegionStartCoord.X = 0;
        destRegionStartCoord.Y = 0;
        destRegionStartCoord.Z = 0;
        destRegionStartCoord.Subresource = 0;
        srcRegionStartCoord.X = 0;
        srcRegionStartCoord.Y = 0;
        srcRegionStartCoord.Z = 0;
        srcRegionStartCoord.Subresource = 0;
        tileRegionSize.bUseBox = true;
        tileRegionSize.Width = 4;
        tileRegionSize.Height = 4;
        tileRegionSize.Depth = 4;
        tileRegionSize.NumTiles = tileRegionSize.Width * tileRegionSize.Height * tileRegionSize.Depth;
        break;

    case COPY_TILE_MAPPINGS_OVERLAPPED:
        // Slice 0:  ABCD      ABCD
        //           EFGH  ->  EFGH
        //           IJKL      IJKL
        //           MNOP      MNOP
        //
        // Slice 1:  QRST      QEFG
        //           UVWX  ->  UIJK
        //           YZab      YZab
        //           cdef      cdef
        //
        // Slice 2:  ghij      gUVW
        //           klmn  ->  kYZa
        //           opqr      opqr
        //           stuv      stuv
        // 
        // Slice 3:  wxyz      wklm
        //           0123  ->  0opq
        //           4567      4567
        //           89+/      89+/

        xform[0][0][0] = 0;   xform[0][0][1] = 1;   xform[0][0][2] = 2;  xform[0][0][3] = 3;
        xform[0][1][0] = 4;   xform[0][1][1] = 5;   xform[0][1][2] = 6;  xform[0][1][3] = 7;
        xform[0][2][0] = 8;   xform[0][2][1] = 9;   xform[0][2][2] = 10; xform[0][2][3] = 11;
        xform[0][3][0] = 12;  xform[0][3][1] = 13;  xform[0][3][2] = 14; xform[0][3][3] = 15;

        xform[1][0][0] = 16;  xform[1][0][1] = 4;   xform[1][0][2] = 5;  xform[1][0][3] = 6;
        xform[1][1][0] = 20;  xform[1][1][1] = 8;   xform[1][1][2] = 9;  xform[1][1][3] = 10;
        xform[1][2][0] = 24;  xform[1][2][1] = 25;  xform[1][2][2] = 26; xform[1][2][3] = 27;
        xform[1][3][0] = 28;  xform[1][3][1] = 29;  xform[1][3][2] = 30; xform[1][3][3] = 31;

        xform[2][0][0] = 32;  xform[2][0][1] = 20;  xform[2][0][2] = 21; xform[2][0][3] = 22;
        xform[2][1][0] = 36;  xform[2][1][1] = 24;  xform[2][1][2] = 25; xform[2][1][3] = 26;
        xform[2][2][0] = 40;  xform[2][2][1] = 41;  xform[2][2][2] = 42; xform[2][2][3] = 43;
        xform[2][3][0] = 44;  xform[2][3][1] = 45;  xform[2][3][2] = 46; xform[2][3][3] = 47;

        xform[3][0][0] = 48;  xform[3][0][1] = 36;  xform[3][0][2] = 37; xform[3][0][3] = 38;
        xform[3][1][0] = 52;  xform[3][1][1] = 40;  xform[3][1][2] = 41; xform[3][1][3] = 42;
        xform[3][2][0] = 56;  xform[3][2][1] = 57;  xform[3][2][2] = 58; xform[3][2][3] = 59;
        xform[3][3][0] = 60;  xform[3][3][1] = 61;  xform[3][3][2] = 62; xform[3][3][3] = 63;

        newMapping = tileMapping;

        // Change pDest to pSrc
        assert(pDest == pSrc);
        destRegionStartCoord.X = 1;
        destRegionStartCoord.Y = 0;
        destRegionStartCoord.Z = 1;
        srcRegionStartCoord.X = 0;
        srcRegionStartCoord.Y = 1;
        srcRegionStartCoord.Z = 0;
        destRegionStartCoord.Subresource = 0;
        srcRegionStartCoord.Subresource = 0;
        tileRegionSize.bUseBox = true;
        tileRegionSize.Width = 3;
        tileRegionSize.Height = 2;
        tileRegionSize.Depth = 3;
        tileRegionSize.NumTiles = tileRegionSize.Width * tileRegionSize.Height * tileRegionSize.Depth;
        break;

    case COPY_TILE_MAPPINGS_PARTIAL:
        // Slice 0:  ABCD      ????
        //           EFGH  ->  ????
        //           IJKL      ????
        //           MNOP      ????
        //
        // Slice 1:  QRST      ?EFG
        //           UVWX  ->  ?IJK
        //           YZab      ????
        //           cdef      ????
        //
        // Slice 2:  ghij      ?UVW
        //           klmn  ->  ?YZa
        //           opqr      ????
        //           stuv      ????
        // 
        // Slice 3:  wxyz      ?klm
        //           0123  ->  ?opq
        //           4567      ????
        //           89+/      ????

        xform[0][0][0] = 0xffffffff;	xform[0][0][1] = 0xffffffff;	xform[0][0][2] = 0xffffffff;	xform[0][0][3] = 0xffffffff;
        xform[0][1][0] = 0xffffffff;	xform[0][1][1] = 0xffffffff;	xform[0][1][2] = 0xffffffff;	xform[0][1][3] = 0xffffffff;
        xform[0][2][0] = 0xffffffff;	xform[0][2][1] = 0xffffffff;	xform[0][2][2] = 0xffffffff;	xform[0][2][3] = 0xffffffff;
        xform[0][3][0] = 0xffffffff;	xform[0][3][1] = 0xffffffff;	xform[0][3][2] = 0xffffffff;	xform[0][3][3] = 0xffffffff;

        xform[1][0][0] = 0xffffffff;	xform[1][0][1] = 4;				xform[1][0][2] = 5;				xform[1][0][3] = 6;
        xform[1][1][0] = 0xffffffff;	xform[1][1][1] = 8;				xform[1][1][2] = 9;				xform[1][1][3] = 10;
        xform[1][2][0] = 0xffffffff;	xform[1][2][1] = 0xffffffff;	xform[1][2][2] = 0xffffffff;	xform[1][2][3] = 0xffffffff;
        xform[1][3][0] = 0xffffffff;	xform[1][3][1] = 0xffffffff;	xform[1][3][2] = 0xffffffff;	xform[1][3][3] = 0xffffffff;

        xform[2][0][0] = 0xffffffff;	xform[2][0][1] = 20;			xform[2][0][2] = 21;			xform[2][0][3] = 22;
        xform[2][1][0] = 0xffffffff;	xform[2][1][1] = 24;			xform[2][1][2] = 25;			xform[2][1][3] = 26;
        xform[2][2][0] = 0xffffffff;	xform[2][2][1] = 0xffffffff;	xform[2][2][2] = 0xffffffff;	xform[2][2][3] = 0xffffffff;
        xform[2][3][0] = 0xffffffff;	xform[2][3][1] = 0xffffffff;	xform[2][3][2] = 0xffffffff;	xform[2][3][3] = 0xffffffff;

        xform[3][0][0] = 0xffffffff;	xform[3][0][1] = 36;			xform[3][0][2] = 37;			xform[3][0][3] = 38;
        xform[3][1][0] = 0xffffffff;	xform[3][1][1] = 40;			xform[3][1][2] = 41;			xform[3][1][3] = 42;
        xform[3][2][0] = 0xffffffff;	xform[3][2][1] = 0xffffffff;	xform[3][2][2] = 0xffffffff;	xform[3][2][3] = 0xffffffff;
        xform[3][3][0] = 0xffffffff;	xform[3][3][1] = 0xffffffff;	xform[3][3][2] = 0xffffffff;	xform[3][3][3] = 0xffffffff;

        destRegionStartCoord.X = 1;
        destRegionStartCoord.Y = 0;
        destRegionStartCoord.Z = 1;
        destRegionStartCoord.Subresource = 0;
        srcRegionStartCoord.X = 0;
        srcRegionStartCoord.Y = 1;
        srcRegionStartCoord.Z = 0;
        srcRegionStartCoord.Subresource = 0;
        tileRegionSize.bUseBox = true;
        tileRegionSize.Width = 3;
        tileRegionSize.Height = 2;
        tileRegionSize.Depth = 3;
        tileRegionSize.NumTiles = tileRegionSize.Width * tileRegionSize.Height * tileRegionSize.Depth;
        break;
    }

    tileMapping.Transform(newMapping, xform);
    tileMapping = newMapping;

    hr = pContext->CopyTileMappings(pDest, &destRegionStartCoord, pSrc, &srcRegionStartCoord, &tileRegionSize, flags);
    pContext->Flush();
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CopyTileMappings);
        return RESULT_FAIL;
    }

    return tr;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::UpdateTileMappingsTex3D()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::UpdateTileMappingsTex3D(ID3D11Texture3D *pTiledResource, UINT tilePoolBaseOffset, ID3D11Buffer *pTilePool, RESIDENCY_PATTERN_2D_3D pattern, CVolumeTileMapping &tileMapping, bool useNoOverwrite)
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    ID3D11Device2 *pDevice = GetFramework()->GetDevice2();
    ID3D11DeviceContext2 *pContext = GetFramework()->GetImmediateContext2();

    D3D11_TILED_RESOURCE_COORDINATE tileRegionStartCoord;
    D3D11_TILE_REGION_SIZE tileRegionSize;
    std::vector<D3D11_TILED_RESOURCE_COORDINATE> tileRegionStartCoords;
    std::vector<D3D11_TILE_REGION_SIZE> tileRegionSizes;
    UINT numRanges;
    UINT tilePoolRangeStartOffsets[MAX_NUM_TILE_POOL_RANGES];
    UINT tilePoolRangeFlags[MAX_NUM_TILE_POOL_RANGES];
    UINT tilePoolRangeTileCounts[MAX_NUM_TILE_POOL_RANGES];
    UINT updateTileMappingsFlags;
    const char *mappingStrings[4][4] = { NULL };
    UINT *pTilePoolRangeStartOffsets = NULL;
    UINT *pTilePoolRangeFlags = NULL;
    UINT *pTilePoolRangeTileCounts = NULL;

    // TILE POOL LAYOUT: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/
    // Pattern names use the above layout to indicate how tiles are mapped in a tiled resource.
    // On Tier-1 an '?' in the pattern indicates a NULL mapped tile
    // On Tier-2 an '?' in the pattern indicates a map to a default shared tile (index 64)
    // On Tier-3 an '?' in the pattern indicates a map to a default shared tile (index 64)

    // Initialize potentially unused values to 0
    tileRegionStartCoord.X = 0;
    tileRegionStartCoord.Y = 0;
    tileRegionStartCoord.Z = 0;

    // Initialize width, height, and depth to 1
    tileRegionSize.Width = 1;
    tileRegionSize.Height = 1;
    tileRegionSize.Depth = 1;

    D3D11_PACKED_MIP_DESC packedMipDesc;
    D3D11_SUBRESOURCE_TILING nonPackedSubresourceTiling;

    switch (pattern)
    {
    case RP2D_3D_FULL:
        mappingStrings[0][0] = "ABCD";
        mappingStrings[0][1] = "EFGH";
        mappingStrings[0][2] = "IJKL";
        mappingStrings[0][3] = "MNOP";

        mappingStrings[1][0] = "QRST";
        mappingStrings[1][1] = "UVWX";
        mappingStrings[1][2] = "YZab";
        mappingStrings[1][3] = "cdef";

        mappingStrings[2][0] = "ghij";
        mappingStrings[2][1] = "klmn";
        mappingStrings[2][2] = "opqr";
        mappingStrings[2][3] = "stuv";

        mappingStrings[3][0] = "wxyz";
        mappingStrings[3][1] = "0123";
        mappingStrings[3][2] = "4567";
        mappingStrings[3][3] = "89+/";
        tileMapping.SetFromStrings(mappingStrings);

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 64;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 1;
        tilePoolRangeStartOffsets[0] = 0x0 + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = 0;
        tilePoolRangeTileCounts[0] = 64;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_FULL_MULTI_REGION:
        mappingStrings[0][0] = "ABCD";
        mappingStrings[0][1] = "EFGH";
        mappingStrings[0][2] = "IJKL";
        mappingStrings[0][3] = "MNOP";

        mappingStrings[1][0] = "QRST";
        mappingStrings[1][1] = "UVWX";
        mappingStrings[1][2] = "YZab";
        mappingStrings[1][3] = "cdef";

        mappingStrings[2][0] = "ghij";
        mappingStrings[2][1] = "klmn";
        mappingStrings[2][2] = "opqr";
        mappingStrings[2][3] = "stuv";

        mappingStrings[3][0] = "wxyz";
        mappingStrings[3][1] = "0123";
        mappingStrings[3][2] = "4567";
        mappingStrings[3][3] = "89+/";
        tileMapping.SetFromStrings(mappingStrings);

        // First region
        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 32;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        // Second region
        tileRegionSize.bUseBox = true;
        tileRegionSize.NumTiles = 32;
        tileRegionSize.Height = 4;
        tileRegionSize.Width = 4;
        tileRegionSize.Depth = 2;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoord.X = 0;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Z = 2;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 1;
        tilePoolRangeStartOffsets[0] = 0x0 + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = 0;
        tilePoolRangeTileCounts[0] = 64;

        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_FULL_MIP_1:
        mappingStrings[0][0] = "AB";
        mappingStrings[0][1] = "CD";
        mappingStrings[0][2] = nullptr;

        mappingStrings[1][0] = "EF";
        mappingStrings[1][1] = "GH";
        mappingStrings[1][2] = nullptr;

        mappingStrings[2][0] = nullptr;
        tileMapping.SetFromStrings(mappingStrings);

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 8;
        tileRegionStartCoord.X = 0;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Z = 0;
        tileRegionStartCoord.Subresource = 1;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 1;
        tilePoolRangeStartOffsets[0] = 0x0 + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = 0;
        tilePoolRangeTileCounts[0] = 8;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_FRAGMENTED_MIP_1:
        mappingStrings[0][0] = "-B";
        mappingStrings[0][1] = "CD";
        mappingStrings[0][2] = nullptr;

        mappingStrings[1][0] = "EF";
        mappingStrings[1][1] = "G-";
        mappingStrings[1][2] = nullptr;

        mappingStrings[2][0] = nullptr;
        tileMapping.SetFromStrings(mappingStrings);

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 6;
        tileRegionStartCoord.X = 1;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Z = 0;
        tileRegionStartCoord.Subresource = 1;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 1;
        tilePoolRangeStartOffsets[0] = 0x1 + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = 0;
        tilePoolRangeTileCounts[0] = 6;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_FRAGMENTED_MULTI_TILE:
        mappingStrings[0][0] = "-BCD";
        mappingStrings[0][1] = "EFGH";
        mappingStrings[0][2] = "IJKL";
        mappingStrings[0][3] = "MNOP";

        mappingStrings[1][0] = "QR??";
        mappingStrings[1][1] = "fgh?";
        mappingStrings[1][2] = "?Z??";
        mappingStrings[1][3] = "???u";

        mappingStrings[2][0] = "vw??";
        mappingStrings[2][1] = "????";
        mappingStrings[2][2] = "????";
        mappingStrings[2][3] = "???l";

        mappingStrings[3][0] = "mnop";
        mappingStrings[3][1] = "????";
        mappingStrings[3][2] = "????";
        mappingStrings[3][3] = "?WXY";

        tileMapping.SetFromStrings(mappingStrings);

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 63;
        tileRegionStartCoord.X = 1;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Z = 0;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 11;
        tilePoolRangeStartOffsets[0] = CVolumeTileMapping::DecodeChar('B') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[1] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[2] = CVolumeTileMapping::DecodeChar('f') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[3] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[4] = CVolumeTileMapping::DecodeChar('Z') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[5] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[6] = CVolumeTileMapping::DecodeChar('u') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[7] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[8] = CVolumeTileMapping::DecodeChar('l') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[9] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[10] = CVolumeTileMapping::DecodeChar('W') + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = 0;
        tilePoolRangeFlags[1] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeFlags[2] = 0;
        tilePoolRangeFlags[3] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeFlags[4] = 0;
        tilePoolRangeFlags[5] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeFlags[6] = 0;
        tilePoolRangeFlags[7] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeFlags[8] = 0;
        tilePoolRangeFlags[9] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeFlags[10] = 0;
        tilePoolRangeTileCounts[0]  = 17;
        tilePoolRangeTileCounts[1]  = 2;
        tilePoolRangeTileCounts[2]  = 3;
        tilePoolRangeTileCounts[3]  = 2;
        tilePoolRangeTileCounts[4]  = 1;
        tilePoolRangeTileCounts[5]  = 5;
        tilePoolRangeTileCounts[6]  = 3;
        tilePoolRangeTileCounts[7]  = 13;
        tilePoolRangeTileCounts[8]  = 5;
        tilePoolRangeTileCounts[9]  = 9;
        tilePoolRangeTileCounts[10] = 3;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_FRAGMENTED_MULTI_REGION:
        mappingStrings[0][0] = "-BCD";
        mappingStrings[0][1] = "EFGH";
        mappingStrings[0][2] = "IJKL";
        mappingStrings[0][3] = "MNOP";

        mappingStrings[1][0] = "QR--";
        mappingStrings[1][1] = "fgh?";
        mappingStrings[1][2] = "?Z??";
        mappingStrings[1][3] = "???u";

        mappingStrings[2][0] = "vw??";
        mappingStrings[2][1] = "????";
        mappingStrings[2][2] = "????";
        mappingStrings[2][3] = "???l";

        mappingStrings[3][0] = "mnop";
        mappingStrings[3][1] = "????";
        mappingStrings[3][2] = "????";
        mappingStrings[3][3] = "?WXY";

        tileMapping.SetFromStrings(mappingStrings);

        // First region
        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 17;
        tileRegionStartCoord.X = 1;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Z = 0;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        // Second region
        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 44;
        tileRegionStartCoord.X = 0;
        tileRegionStartCoord.Y = 1;
        tileRegionStartCoord.Z = 1;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 10;
        tilePoolRangeStartOffsets[0] = CVolumeTileMapping::DecodeChar('B') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[1] = CVolumeTileMapping::DecodeChar('f') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[2] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[3] = CVolumeTileMapping::DecodeChar('Z') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[4] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[5] = CVolumeTileMapping::DecodeChar('u') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[6] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[7] = CVolumeTileMapping::DecodeChar('l') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[8] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[9] = CVolumeTileMapping::DecodeChar('W') + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = 0;
        tilePoolRangeFlags[1] = 0;
        tilePoolRangeFlags[2] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeFlags[3] = 0;
        tilePoolRangeFlags[4] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeFlags[5] = 0;
        tilePoolRangeFlags[6] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeFlags[7] = 0;
        tilePoolRangeFlags[8] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeFlags[9] = 0;
        tilePoolRangeTileCounts[0] = 17;
        tilePoolRangeTileCounts[1] = 3;
        tilePoolRangeTileCounts[2] = 2;
        tilePoolRangeTileCounts[3] = 1;
        tilePoolRangeTileCounts[4] = 5;
        tilePoolRangeTileCounts[5] = 3;
        tilePoolRangeTileCounts[6] = 13;
        tilePoolRangeTileCounts[7] = 5;
        tilePoolRangeTileCounts[8] = 9;
        tilePoolRangeTileCounts[9] = 3;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_FRAGMENTED_MULTI_TILE_SKIPS:
        mappingStrings[0][0] = "-BCD";
        mappingStrings[0][1] = "EFGH";
        mappingStrings[0][2] = "IJKL";
        mappingStrings[0][3] = "MNOP";

        mappingStrings[1][0] = "QR--";
        mappingStrings[1][1] = "fgh-";
        mappingStrings[1][2] = "-Z--";
        mappingStrings[1][3] = "---u";

        mappingStrings[2][0] = "vw--";
        mappingStrings[2][1] = "----";
        mappingStrings[2][2] = "----";
        mappingStrings[2][3] = "---l";

        mappingStrings[3][0] = "mnop";
        mappingStrings[3][1] = "----";
        mappingStrings[3][2] = "----";
        mappingStrings[3][3] = "-WXY";

        tileMapping.SetFromStrings(mappingStrings);

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 63;
        tileRegionStartCoord.X = 1;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Z = 0;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 11;
        tilePoolRangeStartOffsets[0] = CVolumeTileMapping::DecodeChar('B') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[1] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[2] = CVolumeTileMapping::DecodeChar('f') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[3] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[4] = CVolumeTileMapping::DecodeChar('Z') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[5] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[6] = CVolumeTileMapping::DecodeChar('u') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[7] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[8] = CVolumeTileMapping::DecodeChar('l') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[9] = m_blankTileIndex; // may be NULL depending on range
        tilePoolRangeStartOffsets[10] = CVolumeTileMapping::DecodeChar('W') + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = 0;
        tilePoolRangeFlags[1] = D3D11_TILE_RANGE_SKIP;
        tilePoolRangeFlags[2] = 0;
        tilePoolRangeFlags[3] = D3D11_TILE_RANGE_SKIP;
        tilePoolRangeFlags[4] = 0;
        tilePoolRangeFlags[5] = D3D11_TILE_RANGE_SKIP;
        tilePoolRangeFlags[6] = 0;
        tilePoolRangeFlags[7] = D3D11_TILE_RANGE_SKIP;
        tilePoolRangeFlags[8] = 0;
        tilePoolRangeFlags[9] = D3D11_TILE_RANGE_SKIP;
        tilePoolRangeFlags[10] = 0;
        tilePoolRangeTileCounts[0] = 17;
        tilePoolRangeTileCounts[1] = 2;
        tilePoolRangeTileCounts[2] = 3;
        tilePoolRangeTileCounts[3] = 2;
        tilePoolRangeTileCounts[4] = 1;
        tilePoolRangeTileCounts[5] = 5;
        tilePoolRangeTileCounts[6] = 3;
        tilePoolRangeTileCounts[7] = 13;
        tilePoolRangeTileCounts[8] = 5;
        tilePoolRangeTileCounts[9] = 9;
        tilePoolRangeTileCounts[10] = 3;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_FRAGMENTED_REUSE_SINGLE_TILE:
        mappingStrings[0][0] = "----";
        mappingStrings[0][1] = "----";
        mappingStrings[0][2] = "----";
        mappingStrings[0][3] = "----";

        mappingStrings[1][0] = "----";
        mappingStrings[1][1] = "-CCC";
        mappingStrings[1][2] = "-CCC";
        mappingStrings[1][3] = "-CCC";

        mappingStrings[2][0] = "----";
        mappingStrings[2][1] = "-CCC";
        mappingStrings[2][2] = "-CCC";
        mappingStrings[2][3] = "-CCC";

        mappingStrings[3][0] = "----";
        mappingStrings[3][1] = "-CCC";
        mappingStrings[3][2] = "-CCC";
        mappingStrings[3][3] = "-CCC";

        tileMapping.SetFromStrings(mappingStrings);

        tileRegionSize.bUseBox = true;
        tileRegionSize.NumTiles = 27;
        tileRegionSize.Width = 3;
        tileRegionSize.Height = 3;
        tileRegionSize.Depth = 3;
        tileRegionStartCoord.X = 1;
        tileRegionStartCoord.Y = 1;
        tileRegionStartCoord.Z = 1;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 1;
        tilePoolRangeStartOffsets[0] = 0x2 + tilePoolBaseOffset;
        tilePoolRangeFlags[0] = D3D11_TILE_RANGE_REUSE_SINGLE_TILE;
        tilePoolRangeTileCounts[0] = 27;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_UNIFORM_BOX:
        mappingStrings[0][0] = "----";
        mappingStrings[0][1] = "FGH-";
        mappingStrings[0][2] = "JKL-";
        mappingStrings[0][3] = "MNO-";

        mappingStrings[1][0] = "----";
        mappingStrings[1][1] = "VWX-";
        mappingStrings[1][2] = "Zab-";
        mappingStrings[1][3] = "cde-";

        mappingStrings[2][0] = "----";
        mappingStrings[2][1] = "lmn-";
        mappingStrings[2][2] = "pqr-";
        mappingStrings[2][3] = "stu-";

        mappingStrings[3][0] = "----";
        mappingStrings[3][1] = "----";
        mappingStrings[3][2] = "----";
        mappingStrings[3][3] = "----";

        tileMapping.SetFromStrings(mappingStrings);

        tileRegionSize.bUseBox = true;
        tileRegionSize.NumTiles = 27;
        tileRegionSize.Width = 3;
        tileRegionSize.Height = 3;
        tileRegionSize.Depth = 3;
        tileRegionStartCoord.X = 0;
        tileRegionStartCoord.Y = 1;
        tileRegionStartCoord.Z = 0;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 6;
        tilePoolRangeStartOffsets[0] = CVolumeTileMapping::DecodeChar('F') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[1] = CVolumeTileMapping::DecodeChar('J') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[2] = CVolumeTileMapping::DecodeChar('V') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[3] = CVolumeTileMapping::DecodeChar('Z') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[4] = CVolumeTileMapping::DecodeChar('l') + tilePoolBaseOffset;
        tilePoolRangeStartOffsets[5] = CVolumeTileMapping::DecodeChar('p') + tilePoolBaseOffset;
        tilePoolRangeTileCounts[0] = 3;
        tilePoolRangeTileCounts[1] = 6;
        tilePoolRangeTileCounts[2] = 3;
        tilePoolRangeTileCounts[3] = 6;
        tilePoolRangeTileCounts[4] = 3;
        tilePoolRangeTileCounts[5] = 6;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = nullptr;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_CLEAR_MIP_0:
        mappingStrings[0][0] = "????";
        mappingStrings[0][1] = "????";
        mappingStrings[0][2] = "????";
        mappingStrings[0][3] = "????";

        mappingStrings[1][0] = "????";
        mappingStrings[1][1] = "????";
        mappingStrings[1][2] = "????";
        mappingStrings[1][3] = "????";

        mappingStrings[2][0] = "????";
        mappingStrings[2][1] = "????";
        mappingStrings[2][2] = "????";
        mappingStrings[2][3] = "????";

        mappingStrings[3][0] = "????";
        mappingStrings[3][1] = "????";
        mappingStrings[3][2] = "????";
        mappingStrings[3][3] = "????";

        tileMapping.SetFromStrings(mappingStrings);

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 64;
        tileRegionSize.Width = 0;
        tileRegionSize.Height = 0;
        tileRegionSize.Depth = 0;
        tileRegionStartCoord.X = 0;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Z = 0;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 1;
        tilePoolRangeFlags[0] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeStartOffsets[0] = m_blankTileIndex;
        tilePoolRangeTileCounts[0] = 64;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_CLEAR_MIP_1:
        mappingStrings[0][0] = "??";
        mappingStrings[0][1] = "??";
        mappingStrings[0][2] = nullptr;

        mappingStrings[1][0] = "??";
        mappingStrings[1][1] = "??";
        mappingStrings[1][2] = nullptr;

        mappingStrings[2][0] = nullptr;
        tileMapping.SetFromStrings(mappingStrings);

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 8;
        tileRegionSize.Width = 0;
        tileRegionSize.Height = 0;
        tileRegionSize.Depth = 0;
        tileRegionStartCoord.X = 0;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Z = 0;
        tileRegionStartCoord.Subresource = 1;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        numRanges = 1;
        tilePoolRangeFlags[0] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeStartOffsets[0] = m_blankTileIndex;
        tilePoolRangeTileCounts[0] = 8;
        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        break;

    case RP2D_3D_CLEAR_ALL_MIPS:
        mappingStrings[0][0] = "????";
        mappingStrings[0][1] = "????";
        mappingStrings[0][2] = "????";
        mappingStrings[0][3] = "????";

        mappingStrings[1][0] = "????";
        mappingStrings[1][1] = "????";
        mappingStrings[1][2] = "????";
        mappingStrings[1][3] = "????";

        mappingStrings[2][0] = "????";
        mappingStrings[2][1] = "????";
        mappingStrings[2][2] = "????";
        mappingStrings[2][3] = "????";

        mappingStrings[3][0] = "????";
        mappingStrings[3][1] = "????";
        mappingStrings[3][2] = "????";
        mappingStrings[3][3] = "????";

        tileMapping.SetFromStrings(mappingStrings);

        pDevice->GetResourceTiling(
            pTiledResource,
            &tileRegionSize.NumTiles,
            &packedMipDesc,
            nullptr,
            nullptr,
            0,
            &nonPackedSubresourceTiling
            );

        tileRegionSize.bUseBox = false;
        tileRegionStartCoord.X = 0;
        tileRegionStartCoord.Y = 0;
        tileRegionStartCoord.Z = 0;
        tileRegionStartCoord.Subresource = 0;
        tileRegionStartCoords.push_back(tileRegionStartCoord);
        tileRegionSizes.push_back(tileRegionSize);

        updateTileMappingsFlags = useNoOverwrite ? D3D11_TILE_MAPPING_NO_OVERWRITE : 0;
        pTilePoolRangeStartOffsets = tilePoolRangeStartOffsets;
        pTilePoolRangeFlags = tilePoolRangeFlags;
        pTilePoolRangeTileCounts = tilePoolRangeTileCounts;
        numRanges = 1;
        tilePoolRangeTileCounts[0] = tileRegionSize.NumTiles;
        tilePoolRangeFlags[0] = m_parameters.pTiledResourcesTier->GetValue() == 1 ? D3D11_TILE_RANGE_REUSE_SINGLE_TILE : D3D11_TILE_RANGE_NULL;
        tilePoolRangeStartOffsets[0] = m_blankTileIndex;
    }

    hr = pContext->UpdateTileMappings(
        pTiledResource,
        tileRegionStartCoords.size(),
        tileRegionStartCoords.data(),
        tileRegionSizes.data(),
        pTilePool,
        numRanges,
        pTilePoolRangeFlags,
        pTilePoolRangeStartOffsets,
        pTilePoolRangeTileCounts,
        updateTileMappingsFlags
        );

    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, UpdateTileMappings);
        tr = RESULT_FAIL;
    }

    return tr;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::UpdateTileMappingsBuffer()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::UpdateTileMappingsBuffer(ID3D11Buffer *pTileResource, ID3D11Buffer *pTilePool, RESIDENCY_PATTERN_1D pattern)
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;

    return tr;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CopyTiles()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::MapEntireTextureToSingleTile(_In_ ID3D11Texture2D *pTiledResource, UINT tileIndex, _In_ ID3D11Buffer *pTilePool)
{
    return MapEntireTextureToSingleTile < ID3D11Texture2D > (pTiledResource, tileIndex, pTilePool);
}

TEST_RESULT CTiledResourcesTest::MapEntireTextureToSingleTile(_In_ ID3D11Texture3D *pTiledResource, UINT tileIndex, _In_ ID3D11Buffer *pTilePool)
{
    return MapEntireTextureToSingleTile < ID3D11Texture3D > (pTiledResource, tileIndex, pTilePool);
}

template <class ID3D11_TEXTURE_TYPE>
TEST_RESULT CTiledResourcesTest::MapEntireTextureToSingleTile(_In_ ID3D11_TEXTURE_TYPE *pTiledResource, UINT tileIndex, _In_ ID3D11Buffer *pTilePool)
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr;
    ID3D11Device2 *pDevice = GetFramework()->GetDevice2();
    ID3D11DeviceContext2 *pContext = GetFramework()->GetImmediateContext2();
    D3D11_TILED_RESOURCE_COORDINATE tileRegionStartCoord;
    D3D11_TILE_REGION_SIZE tileRegionSize;
    UINT numRanges = 1;
    UINT tilePoolRangeStartOffset = tileIndex;
    UINT tilePoolRangeFlags = D3D11_TILE_RANGE_REUSE_SINGLE_TILE;
    UINT updateTileMappingsFlags = D3D11_TILE_MAPPING_NO_OVERWRITE;

    // Get the total number of logical tiles used by the resource
    UINT totalNumTiles = 0;
    pDevice->GetResourceTiling(pTiledResource, &totalNumTiles, NULL, NULL, NULL, 0, NULL);

    // Update the tile mappings for the entire resource to the provided tileIndex value
    tileRegionSize.bUseBox = false;
    tileRegionSize.NumTiles = totalNumTiles;
    tileRegionStartCoord.X = 0;
    tileRegionStartCoord.Y = 0;
    tileRegionStartCoord.Z = 0;
    tileRegionStartCoord.Subresource = 0;

    hr = pContext->UpdateTileMappings(
        pTiledResource,
        1,
        &tileRegionStartCoord,
        &tileRegionSize,
        pTilePool,
        numRanges,
        &tilePoolRangeFlags,
        &tilePoolRangeStartOffset,
        &totalNumTiles,
        updateTileMappingsFlags
        );

    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, UpdateTileMappings);
        tr = RESULT_FAIL;
    }

    return tr;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CopyTiles()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::CopyTiles(_In_ ID3D11Texture2D *pTiledResource, _In_ ID3D11Buffer *pBuffer, UINT bufferStartOffset, COPY_TILES_PATTERN pattern, UINT flags, _Out_ CTileMapping &bufferMapping)
{
    return CopyTiles < ID3D11Texture2D, CTileMapping >(pTiledResource, pBuffer, bufferStartOffset, pattern, flags, bufferMapping);
}
TEST_RESULT CTiledResourcesTest::CopyTiles(_In_ ID3D11Texture3D *pTiledResource, _In_ ID3D11Buffer *pBuffer, UINT bufferStartOffset, COPY_TILES_PATTERN pattern, UINT flags, _Out_ CVolumeTileMapping &bufferMapping)
{
    return CopyTiles < ID3D11Texture3D, CVolumeTileMapping > (pTiledResource, pBuffer, bufferStartOffset, pattern, flags, bufferMapping);
}

template <class ID3D11_TEXTURE_TYPE, class TILE_MAPPING_TYPE>
TEST_RESULT CTiledResourcesTest::CopyTiles(_In_ ID3D11_TEXTURE_TYPE *pTiledResource, _In_ ID3D11Buffer *pBuffer, UINT bufferStartOffset, COPY_TILES_PATTERN pattern, UINT flags, _Out_ TILE_MAPPING_TYPE &bufferMapping)
{
    TEST_RESULT tr = RESULT_PASS;
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();

    D3D11_TILED_RESOURCE_COORDINATE tiledResourceCoord;
    D3D11_TILE_REGION_SIZE tileRegionSize;

    tr = GetCoordinatesAndSizeFromCopyPattern(pattern, tiledResourceCoord, tileRegionSize, bufferStartOffset, bufferMapping);   // Overloaded for Texture2D and Texture3D
    pContext->CopyTiles(pTiledResource, &tiledResourceCoord, &tileRegionSize, pBuffer, bufferStartOffset, flags);

    return tr;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::UpdateTiles()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::UpdateTiles(_In_ ID3D11Texture2D *pTiledResource, _In_ BYTE *pBuffer, COPY_TILES_PATTERN pattern, UINT flags, _Out_ CTileMapping &bufferMapping)
{
    return UpdateTiles < ID3D11Texture2D, CTileMapping >(pTiledResource, pBuffer, pattern, flags, bufferMapping);
}

TEST_RESULT CTiledResourcesTest::UpdateTiles(_In_ ID3D11Texture3D *pTiledResource, _In_ BYTE *pBuffer, COPY_TILES_PATTERN pattern, UINT flags, _Out_ CVolumeTileMapping &bufferMapping)
{
    return UpdateTiles < ID3D11Texture3D, CVolumeTileMapping > (pTiledResource, pBuffer, pattern, flags, bufferMapping);
}

template <class ID3D11_TEXTURE_TYPE, class TILE_MAPPING_TYPE>
TEST_RESULT CTiledResourcesTest::UpdateTiles(_In_ ID3D11_TEXTURE_TYPE *pTiledResource, _In_ BYTE *pBuffer, COPY_TILES_PATTERN pattern, UINT flags, _Out_ TILE_MAPPING_TYPE &bufferMapping)
{
    TEST_RESULT tr = RESULT_PASS;
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();

    D3D11_TILED_RESOURCE_COORDINATE tiledResourceCoord;
    D3D11_TILE_REGION_SIZE tileRegionSize;
    UINT bufferOffset = 0;

    tr = GetCoordinatesAndSizeFromCopyPattern(pattern, tiledResourceCoord, tileRegionSize, bufferOffset, bufferMapping);    // Overloaded for Texture2D and Texture3D
    pContext->UpdateTiles(pTiledResource, &tiledResourceCoord, &tileRegionSize, (void*)(pBuffer + bufferOffset), flags);

    return tr;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::FillBufferWithTilePattern()
////////////////////////////////////////
TEST_RESULT FillBufferWithTilePatternHelper(
    _Inout_ R8G8B8A8_PIXEL *pBuffer,
    const UINT tileWidth,
    const UINT tileHeight,
    const UINT tileDepth,
    const UINT widthPerTile,
    const UINT heightPerTile,
    const UINT depthPerTile,
    const std::function<void(R8G8B8A8_PIXEL **pixel, const UINT i, const UINT j, const UINT k, const UINT a, const UINT b, const UINT c, const UINT count)> &PerPixelFunction
    )
{
    if (!PerPixelFunction)
    {
        return RESULT_FAIL;
    }

    UINT count = 0;
    R8G8B8A8_PIXEL *pixel = pBuffer;

    // For each tile slice
    for (UINT k = 0; k < tileDepth; ++k)
    {
        // For each tile row
        for (UINT j = 0; j < tileWidth; ++j)
        {
            // For each tile column
            for (UINT i = 0; i < tileHeight; ++i)
            {
                // FOr each pixel slice
                for (UINT c = 0; c < depthPerTile; ++c)
                {
                    // For each pixel row
                    for (UINT b = 0; b < heightPerTile; ++b)
                    {
                        // For each pixel column
                        for (UINT a = 0; a < widthPerTile; ++a)
                        {
                            // execute provided lambda per-pixel
                            PerPixelFunction(&pixel, i, j, k, a, b, c, count);
                            ++pixel;
                        }
                    }
                }

                count++;
            }
        }
    }

    return RESULT_PASS;
}

TEST_RESULT CTiledResourcesTest::FillBufferWithTilePattern(_Inout_ R8G8B8A8_PIXEL *pBuffer, UINT tileWidth, UINT tileHeight)
{
    return FillBufferWithTilePatternHelper(
        pBuffer,
        tileWidth,
        tileHeight,
        1,
        CResourceBaseTestHelper<TRB_TEXTURE_2D>::widthPerTile,
        CResourceBaseTestHelper<TRB_TEXTURE_2D>::heightPerTile,
        1,
        PerPixelLambdaTilePattern2D
        );
}

TEST_RESULT CTiledResourcesTest::FillBufferWithTilePattern(_Inout_ R8G8B8A8_PIXEL *pBuffer, UINT tileWidth, UINT tileHeight, UINT tileDepth)
{
    return FillBufferWithTilePatternHelper(
        pBuffer,
        tileWidth,
        tileHeight,
        tileDepth,
        CResourceBaseTestHelper<TRB_TEXTURE_3D>::widthPerTile,
        CResourceBaseTestHelper<TRB_TEXTURE_3D>::heightPerTile,
        CResourceBaseTestHelper<TRB_TEXTURE_3D>::depthPerTile,
        PerPixelLambdaTilePattern3D
        );
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CreateTileStreamBuffer()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::CreateTileStreamBuffer(ID3D11Buffer **ppBuffer, UINT bufferSize, UINT dataOffset, BYTE *bufferData)
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();

    D3D11_BUFFER_DESC bufferDesc;
    D3D11_SUBRESOURCE_DATA *pInitialData = NULL;
    R8G8B8A8_PIXEL *pixelData;

    if(bufferData)
    {
        D3D11_SUBRESOURCE_DATA initialData;
        initialData.pSysMem = reinterpret_cast<void *>(bufferData);

        initialData.SysMemPitch = 0;
        initialData.SysMemSlicePitch = 0;

        pInitialData = &initialData;
    }

    bufferDesc.ByteWidth = bufferSize;
    bufferDesc.BindFlags = 0;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    hr = pDevice->CreateBuffer(&bufferDesc, pInitialData, ppBuffer);
    if(FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateBuffer);
        tr = RESULT_FAIL;
    }

    return tr;
}


////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::GetCoordinatesAndSizeFromCopyPattern()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::GetCoordinatesAndSizeFromCopyPattern(COPY_TILES_PATTERN pattern, D3D11_TILED_RESOURCE_COORDINATE &tiledResourceCoord, D3D11_TILE_REGION_SIZE &tileRegionSize, UINT &bufferOffset, CTileMapping &bufferMapping)
{
    const char *mappingStrings[4];
    switch(pattern)
    {
    case COPY_TILES_FULL:
        mappingStrings[0] = "0123";
        mappingStrings[1] = "4567";
        mappingStrings[2] = "89AB";
        mappingStrings[3] = "CDEF";
        bufferMapping.SetFromStrings(mappingStrings);

        tiledResourceCoord.Subresource = 0;
        tiledResourceCoord.X = 0;
        tiledResourceCoord.Y = 0;
        tiledResourceCoord.Z = 0;

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 16;
        break;
    case COPY_TILES_CENTER_BOX:
        mappingStrings[0] = "ssss";
        mappingStrings[1] = "s12s";
        mappingStrings[2] = "s34s";
        mappingStrings[3] = "ssss";
        bufferMapping.SetFromStrings(mappingStrings);

        tiledResourceCoord.Subresource = 0;
        tiledResourceCoord.X = 1;
        tiledResourceCoord.Y = 1;
        tiledResourceCoord.Z = 0;

        tileRegionSize.bUseBox = true;
        tileRegionSize.NumTiles = 4;
        tileRegionSize.Width = 2;
        tileRegionSize.Height = 2;
        tileRegionSize.Depth = 1;
        bufferOffset += TILE_SIZE_IN_BYTES * 1;
        break;
    case COPY_TILES_PARTIAL:
        mappingStrings[0] = "xx12";
        mappingStrings[1] = "3456";
        mappingStrings[2] = "789A";
        mappingStrings[3] = "Bxxx";
        bufferMapping.SetFromStrings(mappingStrings);

        tiledResourceCoord.Subresource = 0;
        tiledResourceCoord.X = 2;
        tiledResourceCoord.Y = 0;
        tiledResourceCoord.Z = 0;

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 11;
        bufferOffset += TILE_SIZE_IN_BYTES * 1;
        break;
    case COPY_TILES_MIP1:
        mappingStrings[0] = "12";
        mappingStrings[1] = "34";
        mappingStrings[2] = NULL;
        bufferMapping.SetFromStrings(mappingStrings);

        tiledResourceCoord.Subresource = 1;
        tiledResourceCoord.X = 0;
        tiledResourceCoord.Y = 0;
        tiledResourceCoord.Z = 0;

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 4;
        bufferOffset += TILE_SIZE_IN_BYTES * 1;
        break;
    }
    return RESULT_PASS;
}

TEST_RESULT CTiledResourcesTest::GetCoordinatesAndSizeFromCopyPattern(COPY_TILES_PATTERN pattern, D3D11_TILED_RESOURCE_COORDINATE &tiledResourceCoord, D3D11_TILE_REGION_SIZE &tileRegionSize, UINT &bufferOffset, CVolumeTileMapping &bufferMapping)
{
    const char *mappingStrings[4][4];
    switch (pattern)
    {
    case COPY_TILES_FULL:
        mappingStrings[0][0] = "ABCD";
        mappingStrings[0][1] = "EFGH";
        mappingStrings[0][2] = "IJKL";
        mappingStrings[0][3] = "MNOP";

        mappingStrings[1][0] = "QRST";
        mappingStrings[1][1] = "UVWX";
        mappingStrings[1][2] = "YZab";
        mappingStrings[1][3] = "cdef";

        mappingStrings[2][0] = "ghij";
        mappingStrings[2][1] = "klmn";
        mappingStrings[2][2] = "opqr";
        mappingStrings[2][3] = "stuv";

        mappingStrings[3][0] = "wxyz";
        mappingStrings[3][1] = "0123";
        mappingStrings[3][2] = "4567";
        mappingStrings[3][3] = "89+/";

        bufferMapping.SetFromStrings(mappingStrings);

        tiledResourceCoord.Subresource = 0;
        tiledResourceCoord.X = 0;
        tiledResourceCoord.Y = 0;
        tiledResourceCoord.Z = 0;

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 64;
        break;
    case COPY_TILES_CENTER_BOX:
        mappingStrings[0][0] = "----";
        mappingStrings[0][1] = "----";
        mappingStrings[0][2] = "----";
        mappingStrings[0][3] = "----";

        mappingStrings[1][0] = "----";
        mappingStrings[1][1] = "-BC-";
        mappingStrings[1][2] = "-DE-";
        mappingStrings[1][3] = "----";

        mappingStrings[2][0] = "----";
        mappingStrings[2][1] = "-FG-";
        mappingStrings[2][2] = "-HI-";
        mappingStrings[2][3] = "----";

        mappingStrings[3][0] = "----";
        mappingStrings[3][1] = "----";
        mappingStrings[3][2] = "----";
        mappingStrings[3][3] = "----";

        bufferMapping.SetFromStrings(mappingStrings);

        tiledResourceCoord.Subresource = 0;
        tiledResourceCoord.X = 1;
        tiledResourceCoord.Y = 1;
        tiledResourceCoord.Z = 1;

        tileRegionSize.bUseBox = true;
        tileRegionSize.NumTiles = 8;
        tileRegionSize.Width = 2;
        tileRegionSize.Height = 2;
        tileRegionSize.Depth = 2;
        bufferOffset += TILE_SIZE_IN_BYTES * 1;
        break;
    case COPY_TILES_PARTIAL:
        mappingStrings[0][0] = "??BC";
        mappingStrings[0][1] = "DEFG";
        mappingStrings[0][2] = "HIJK";
        mappingStrings[0][3] = "LMNO";

        mappingStrings[1][0] = "PQRS";
        mappingStrings[1][1] = "TUVW";
        mappingStrings[1][2] = "XYZa";
        mappingStrings[1][3] = "bcde";

        mappingStrings[2][0] = "fghi";
        mappingStrings[2][1] = "jklm";
        mappingStrings[2][2] = "nopq";
        mappingStrings[2][3] = "rstu";

        mappingStrings[3][0] = "vwxy";
        mappingStrings[3][1] = "z012";
        mappingStrings[3][2] = "3456";
        mappingStrings[3][3] = "7???";

        bufferMapping.SetFromStrings(mappingStrings);

        tiledResourceCoord.Subresource = 0;
        tiledResourceCoord.X = 2;
        tiledResourceCoord.Y = 0;
        tiledResourceCoord.Z = 0;

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 59;
        bufferOffset += TILE_SIZE_IN_BYTES * 1;
        break;
    case COPY_TILES_MIP1:
        mappingStrings[0][0] = "BC";
        mappingStrings[0][1] = "DE";
        mappingStrings[0][1] = NULL;

        mappingStrings[1][0] = "FG";
        mappingStrings[1][1] = "HI";
        mappingStrings[1][2] = NULL;

        mappingStrings[2][0] = NULL;

        bufferMapping.SetFromStrings(mappingStrings);

        tiledResourceCoord.Subresource = 1;
        tiledResourceCoord.X = 0;
        tiledResourceCoord.Y = 0;
        tiledResourceCoord.Z = 0;

        tileRegionSize.bUseBox = false;
        tileRegionSize.NumTiles = 8;
        bufferOffset += TILE_SIZE_IN_BYTES * 1;
        break;
    }
    return RESULT_PASS;
}

void SetMapIfNonNull(UINT x, UINT y, UINT value, _In_ const CTileMapping& reference, _Out_ CTileMapping& newMap)
{
    newMap.Row[x].Column[y] = (UnmappedTile(reference.Row[x].Column[y])) ? 0xffffffffu : value;
}

void SetMapIfNonNull(UINT x, UINT y, UINT z, UINT value, _In_ const CVolumeTileMapping& reference, _Out_ CVolumeTileMapping& newMap)
{
    newMap.Slice[x].Row[y].Column[z] = (UnmappedTile(reference.Slice[x].Row[y].Column[z])) ? 0xffffffffu : value;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::IntersectMappings()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::IntersectMappings(_In_ const CTileMapping& residencyMap, COPY_TILES_PATTERN copyPattern, _Out_ CTileMapping& copiedMap, _Out_ CBufferMapping& bufferMap)
{
    // Use -2 to indicate "unchanged by the operation"
    for(UINT x = 0; x < 4; ++x)
        for(UINT y = 0; y < 4; ++y)
            SetMapIfNonNull(x, y, SkippedTileId, residencyMap, copiedMap);

    switch(copyPattern)
    {
    case COPY_TILES_FULL:
        // "0123";
        // "4567";
        // "89AB";
        // "CDEF";
        for(UINT x = 0; x < 4; ++x)
            for(UINT y = 0; y < 4; ++y)
                SetMapIfNonNull(x, y, (x * 4) + y, residencyMap, copiedMap);
        for(UINT i = 0; i < 16; ++i)
            bufferMap.Tiles[i] = residencyMap.Row[i / 4].Column[i % 4];
        break;
    case COPY_TILES_CENTER_BOX:
        // "xxxx";
        // "x12x";
        // "x34x";
        // "xxxx";
        SetMapIfNonNull(1, 1, 1, residencyMap, copiedMap);
        SetMapIfNonNull(1, 2, 2, residencyMap, copiedMap);
        SetMapIfNonNull(2, 1, 3, residencyMap, copiedMap);
        SetMapIfNonNull(2, 2, 4, residencyMap, copiedMap);
        bufferMap.Tiles[1] = residencyMap.Row[1].Column[1];
        bufferMap.Tiles[2] = residencyMap.Row[1].Column[2];
        bufferMap.Tiles[3] = residencyMap.Row[2].Column[1];
        bufferMap.Tiles[4] = residencyMap.Row[2].Column[2];
        break;
    case COPY_TILES_PARTIAL:
        // "xx12";
        // "3456";
        // "789A";
        // "Bxxx";
        for(UINT i = 1; i <= 0xB; ++i)
        {
            SetMapIfNonNull((i + 1) / 4, (i + 1) % 4, i, residencyMap, copiedMap);
            bufferMap.Tiles[i] = residencyMap.Row[(i + 1) / 4].Column[(i + 1) % 4];
        }
        break;
    case COPY_TILES_MIP1:
        // "12";
        // "34";
        // NULL;
        SetMapIfNonNull(0, 0, 1, residencyMap, copiedMap);
        SetMapIfNonNull(0, 1, 2, residencyMap, copiedMap);
        SetMapIfNonNull(1, 0, 3, residencyMap, copiedMap);
        SetMapIfNonNull(1, 1, 4, residencyMap, copiedMap);
        bufferMap.Tiles[1] = residencyMap.Row[0].Column[0];
        bufferMap.Tiles[2] = residencyMap.Row[0].Column[1];
        bufferMap.Tiles[3] = residencyMap.Row[1].Column[0];
        bufferMap.Tiles[4] = residencyMap.Row[1].Column[1];
        copiedMap.Width = 2;
        copiedMap.Height = 2;
        break;
    }

    return RESULT_PASS;
}

TEST_RESULT CTiledResourcesTest::IntersectMappings(_In_ const CVolumeTileMapping& residencyMap, COPY_TILES_PATTERN copyPattern, _Out_ CVolumeTileMapping& copiedMap, _Out_ CVolumeBufferMapping& bufferMap)
{
    // Use -2 to indicate "unchanged by the operation"
    for (UINT x = 0; x < 4; ++x)
        for (UINT y = 0; y < 4; ++y)
            for (UINT z = 0; z < 4; ++z)
                SetMapIfNonNull(x, y, z, SkippedTileId, residencyMap, copiedMap);

    switch (copyPattern)
    {
    case COPY_TILES_FULL:
        // Slice 0:  "ABCD"
        //           "EFGH"
        //           "IJKL"
        //           "MNOP"
        //
        // Slice 1:  "QRST"
        //           "UVWX"
        //           "YZab"
        //           "cdef"
        //
        // Slice 2:  "ghij"
        //           "klmn"
        //           "opqr"
        //           "stuv"
        //
        // Slice 3:  "wxyz"
        //           "0123"
        //           "4567"
        //           "89+/"
        for (UINT x = 0; x < 4; ++x)
            for (UINT y = 0; y < 4; ++y)
                for (UINT z = 0; z < 4; ++z)
                    SetMapIfNonNull(x, y, z, (x*16)+(y*4)+z, residencyMap, copiedMap);
        for (UINT i = 0; i < 64; ++i)
        {
            UINT sliceLeft = i % 16;
            UINT z = i / 16;
            UINT y = sliceLeft / 4;
            UINT x = sliceLeft % 4;
            bufferMap.Tiles[i] = residencyMap.Slice[z].Row[y].Column[x];
        }
        break;
    case COPY_TILES_CENTER_BOX:
        // Slice 0:  "????"
        //           "????"
        //           "????"
        //           "????"
        //
        // Slice 1:  "????"
        //           "?BC?"
        //           "?DE?"
        //           "????"
        //
        // Slice 2:  "????"
        //           "?FG?"
        //           "?HI?"
        //           "????"
        //
        // Slice 3:  "????"
        //           "????"
        //           "????"
        //           "????"
        SetMapIfNonNull(1, 1, 1, 1, residencyMap, copiedMap);
        SetMapIfNonNull(1, 1, 2, 2, residencyMap, copiedMap);
        SetMapIfNonNull(1, 2, 1, 3, residencyMap, copiedMap);
        SetMapIfNonNull(1, 2, 2, 4, residencyMap, copiedMap);

        SetMapIfNonNull(2, 1, 1, 5, residencyMap, copiedMap);
        SetMapIfNonNull(2, 1, 2, 6, residencyMap, copiedMap);
        SetMapIfNonNull(2, 2, 1, 7, residencyMap, copiedMap);
        SetMapIfNonNull(2, 2, 2, 8, residencyMap, copiedMap);

        bufferMap.Tiles[1] = residencyMap.Slice[1].Row[1].Column[1];
        bufferMap.Tiles[2] = residencyMap.Slice[1].Row[1].Column[2];
        bufferMap.Tiles[3] = residencyMap.Slice[1].Row[2].Column[1];
        bufferMap.Tiles[4] = residencyMap.Slice[1].Row[2].Column[2];

        bufferMap.Tiles[5] = residencyMap.Slice[2].Row[1].Column[1];
        bufferMap.Tiles[6] = residencyMap.Slice[2].Row[1].Column[2];
        bufferMap.Tiles[7] = residencyMap.Slice[2].Row[2].Column[1];
        bufferMap.Tiles[8] = residencyMap.Slice[2].Row[2].Column[2];
        break;
    case COPY_TILES_PARTIAL:
        // Slice 0:  "??BC"
        //           "DEFG"
        //           "HIJK"
        //           "LMNO"
        //
        // Slice 1:  "PQRS"
        //           "TUVW"
        //           "XYZa"
        //           "bcde"
        //
        // Slice 2:  "fghi"
        //           "jklm"
        //           "nopq"
        //           "rstu"
        //
        // Slice 3:  "vwxy"
        //           "z012"
        //           "3456"
        //           "7???"
        for (UINT i = 1; i <= 59; ++i)
        {
            UINT sliceLeft = (i + 1) % 16;
            UINT z = (i + 1) / 16;
            UINT y = sliceLeft / 4;
            UINT x = sliceLeft % 4;
            SetMapIfNonNull(z, y, x, i, residencyMap, copiedMap);
            bufferMap.Tiles[i] = residencyMap.Slice[z].Row[y].Column[x];
        }
        break;
    case COPY_TILES_MIP1:
        // Slice 0:  "BC"
        //           "DE"
        //           NULL
        //
        // Slice 1:  "FG"
        //           "HI"
        //           NULL
        //
        // Slice 2:  NULL
        SetMapIfNonNull(0, 0, 0, 1, residencyMap, copiedMap);
        SetMapIfNonNull(0, 0, 1, 2, residencyMap, copiedMap);
        SetMapIfNonNull(0, 1, 0, 3, residencyMap, copiedMap);
        SetMapIfNonNull(0, 1, 1, 4, residencyMap, copiedMap);

        SetMapIfNonNull(1, 0, 0, 5, residencyMap, copiedMap);
        SetMapIfNonNull(1, 0, 1, 6, residencyMap, copiedMap);
        SetMapIfNonNull(1, 1, 0, 7, residencyMap, copiedMap);
        SetMapIfNonNull(1, 1, 1, 8, residencyMap, copiedMap);

        bufferMap.Tiles[1] = residencyMap.Slice[0].Row[0].Column[0];
        bufferMap.Tiles[2] = residencyMap.Slice[0].Row[0].Column[1];
        bufferMap.Tiles[3] = residencyMap.Slice[0].Row[1].Column[0];
        bufferMap.Tiles[4] = residencyMap.Slice[0].Row[1].Column[1];

        bufferMap.Tiles[5] = residencyMap.Slice[1].Row[0].Column[0];
        bufferMap.Tiles[6] = residencyMap.Slice[1].Row[0].Column[1];
        bufferMap.Tiles[7] = residencyMap.Slice[1].Row[1].Column[0];
        bufferMap.Tiles[8] = residencyMap.Slice[1].Row[1].Column[1];

        copiedMap.Width = 2;
        copiedMap.Height = 2;
        copiedMap.Depth = 2;
        break;
    }

    return RESULT_PASS;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::TileDataFromTex2D()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::TileDataFromTex2D(_In_ ID3D11Texture2D *pTiledResource, UINT subResource, CTileMapping &observedMapping)
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    ID3D11DeviceContext2 *pDeviceContext = GetImmediateContext2();
    CHandle<ID3D11Texture2D> pStagingResource;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    D3D11_TEXTURE2D_DESC sourceDesc;

    UINT tileWidth = 0;
    UINT tileHeight = 0;

    pTiledResource->GetDesc(&sourceDesc);
    UINT mipWidth = sourceDesc.Width;
    UINT mipHeight = sourceDesc.Height;
    if(!CFormatHelper::GetMipDimensions(subResource, &mipWidth, &mipHeight) )
    {
        WriteToLog(_T("GetMipDimensions failed due to non-existent mip level"));
        return RESULT_FAIL;
    }

    // Get the tile size
    GetTileSize(sourceDesc.Format, tileWidth, tileHeight);
    assert(CResourceBaseTestHelper<TRB_TEXTURE_2D>::widthPerTile == tileWidth);
    assert(CResourceBaseTestHelper<TRB_TEXTURE_2D>::heightPerTile == tileHeight);

    // Create the staging resource
    tr = CreateStagingTex2D(sourceDesc.Width, sourceDesc.Height, &pStagingResource, false);
    if(tr != RESULT_PASS)
        return tr;

    // Copy the data from the tiled resource to the staging resource
    pDeviceContext->CopyResource(pStagingResource, pTiledResource);

    // Map the staged data for read
    hr = pDeviceContext->Map(pStagingResource, subResource, D3D11_MAP_READ, 0, &mappedResource);
    if(FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, Map);
        return RESULT_FAIL;
    }

    // Iterate through the tile data grabbing the first pixel from each logical tile copy
    UINT row = 0;
    for(UINT j = 0; j < mipHeight; j += tileHeight)
    {
        UINT col = 0;
        for(UINT i = 0; i < mipWidth; i += tileWidth)
        {
            int offset = j * mappedResource.RowPitch + i;
            R8G8B8A8_PIXEL *pixel = reinterpret_cast<R8G8B8A8_PIXEL *>(reinterpret_cast<BYTE *>(mappedResource.pData) + offset);

            assert(row < 4);
            assert(col < 4);

            if(pixel->g == 0)
            {
                // If the green channel is 0 then the tile is unmapped
                observedMapping.Row[row].Column[col] = 0xffffffffUL;
            }
            else
            {
                // The value in the red channel is the "ID" of the tile.  
                observedMapping.Row[row].Column[col] = (UINT) pixel->r;
            }
        
            col++;
            observedMapping.Width = col;
        }

        row++;
        observedMapping.Height = row;
    }

    pDeviceContext->Unmap(pStagingResource, subResource);

    return tr;
}

const R8G8B8A8_PIXEL* CastToPixel(const BYTE* p)
{
    return reinterpret_cast<const R8G8B8A8_PIXEL*>(p);
}

class TileMappingValidator
{
    public:
        TileMappingValidator(const D3D11_MAPPED_SUBRESOURCE& Gold, const D3D11_MAPPED_SUBRESOURCE& Test, tstring* Mapped, tstring* Unmapped);
        unsigned Validate(UINT GoldTileId, UINT TestTileRow, UINT TestTileColumn, UINT TileHeight /*= TEX2D_HEIGHT_PER_TILE*/, UINT TileWidth /*= TEX2D_WIDTH_PER_TILE*/);
        unsigned Validate(UINT GoldTileId, UINT TestTileSlice, UINT TestTileRow, UINT TestTileColumn, UINT TileDepth /*= TEX3D_DEPTH_PER_TILE*/, UINT TileHeight /*= TEX3D_HEIGHT_PER_TILE*/, UINT TileWidth /*= TEX3D_WIDTH_PER_TILE*/);
        R8G8B8A8_PIXEL GetTestTileContent(const UINT TileRow, const UINT TileColumn);
        R8G8B8A8_PIXEL GetTestTileContent(const UINT TileSlice, const UINT TileRow, const UINT TileColumn);

    private:
        struct MappedResource
        {
            BYTE* p;
            unsigned ByteRowPitch;
            unsigned ByteDepthPitch;
            const BYTE* GetTile(UINT Row, UINT Column)
            {
                return p + (Row * CResourceBaseTestHelper<TRB_TEXTURE_2D>::heightPerTile *ByteRowPitch) + (Column * CResourceBaseTestHelper<TRB_TEXTURE_2D>::widthPerTile *sizeof(R8G8B8A8_PIXEL));
            }
            const R8G8B8A8_PIXEL* GetPixel(UINT Row, UINT Column, UINT TileOffsetY, UINT TileOffsetX)
            {
                return CastToPixel(GetTile(Row, Column) + (TileOffsetY * ByteRowPitch)) + (TileOffsetX);
            }
            const R8G8B8A8_PIXEL* GetPixel(const BYTE* pTile, UINT TileOffsetY, UINT TileOffsetX)
            {
                return CastToPixel(pTile + (TileOffsetY * ByteRowPitch)) + TileOffsetX;
            }
            const BYTE* GetTile(UINT Slice, UINT Row, UINT Column)
            {
                const UINT columnOffset = Column * CResourceBaseTestHelper<TRB_TEXTURE_3D>::widthPerTile  * sizeof(R8G8B8A8_PIXEL);
                const UINT rowOffset    = Row    * CResourceBaseTestHelper<TRB_TEXTURE_3D>::heightPerTile * ByteRowPitch;
                const UINT sliceOffset  = Slice  * CResourceBaseTestHelper<TRB_TEXTURE_3D>::depthPerTile  * ByteDepthPitch;
                const UINT offset = sliceOffset + rowOffset + columnOffset;
                return p + offset;
            }
            const R8G8B8A8_PIXEL* GetPixel(UINT Slice, UINT Row, UINT Column, UINT TileOffsetZ, UINT TileOffsetY, UINT TileOffsetX)
            {
                return CastToPixel(GetTile(Slice, Row, Column) + (TileOffsetZ * ByteDepthPitch) + (TileOffsetY * ByteRowPitch)) + (TileOffsetX);
            }
            const R8G8B8A8_PIXEL* GetPixel(const BYTE* pTile, UINT TileOffsetZ, UINT TileOffsetY, UINT TileOffsetX)
            {
                return CastToPixel(pTile + (TileOffsetZ * ByteDepthPitch) + (TileOffsetY * ByteRowPitch)) + TileOffsetX;
            }
        };
        void RecordError(bool NullTile, UINT TileY, UINT TileX, const R8G8B8A8_PIXEL* Gold, const R8G8B8A8_PIXEL* Test);
        void RecordError(bool NullTile, UINT TileZ, UINT TileY, UINT TileX, const R8G8B8A8_PIXEL* Gold, const R8G8B8A8_PIXEL* Test);
        MappedResource GoldResource;
        MappedResource TestResource;
        tstring* MappedString; // String provided by caller for recording errors validating mapped tiles.
        tstring* UnmappedString; // String provided by caller for recording errors validating unmapped tiles.
};

R8G8B8A8_PIXEL TileMappingValidator::GetTestTileContent(UINT TileRow, UINT TileColumn)
{
    return *CastToPixel(TestResource.GetTile(TileRow, TileColumn));
}

R8G8B8A8_PIXEL TileMappingValidator::GetTestTileContent(UINT TileSlice, UINT TileRow, UINT TileColumn)
{
    return *CastToPixel(TestResource.GetTile(TileSlice, TileRow, TileColumn));
}

TileMappingValidator::TileMappingValidator(const D3D11_MAPPED_SUBRESOURCE& Gold, const D3D11_MAPPED_SUBRESOURCE& Test, tstring* Mapped, tstring* Unmapped) : MappedString (Mapped),
                                                                                                                                                             UnmappedString(Unmapped)
{
    GoldResource.p = reinterpret_cast<BYTE*>(Gold.pData);
    GoldResource.ByteRowPitch = Gold.RowPitch;
    GoldResource.ByteDepthPitch = Gold.DepthPitch;
    TestResource.p = reinterpret_cast<BYTE*>(Test.pData);
    TestResource.ByteRowPitch = Test.RowPitch;
    TestResource.ByteDepthPitch = Test.DepthPitch;
}

unsigned TileMappingValidator::Validate(UINT GoldTileId, UINT TestTileRow, UINT TestTileColumn, UINT TileHeight, UINT TileWidth)
{
    // GoldTileId determines which tile contents should be in the tile at TestTileRow & TestTileColumn
    const BYTE* pGoldTile = GoldResource.GetTile(GoldTileId / 4, GoldTileId % 4);
    const BYTE* pTestTile = TestResource.GetTile(TestTileRow, TestTileColumn);

    unsigned FailCount = 0;
    for (UINT Row = 0; Row < TileHeight; ++Row)
    {
        for (UINT Column = 0; Column < TileWidth; ++Column)
        {
            const R8G8B8A8_PIXEL NullPixel = {};
            const R8G8B8A8_PIXEL* GoldPixel = UnmappedOrSkippedTile(GoldTileId) ? &NullPixel : GoldResource.GetPixel(pGoldTile, Row, Column);
            const R8G8B8A8_PIXEL* TestPixel = TestResource.GetPixel(pTestTile, Row, Column);
            if (*GoldPixel != *TestPixel)
            {
                ++FailCount;
                RecordError(UnmappedOrSkippedTile(GoldTileId), TestTileRow, TestTileColumn, GoldPixel, TestPixel);
            }
        }
    }
    return FailCount;
}

unsigned TileMappingValidator::Validate(UINT GoldTileId, UINT TestTileSlice, UINT TestTileRow, UINT TestTileColumn, UINT TileDepth, UINT TileHeight, UINT TileWidth)
{
    // GoldTileId determines which tile contents should be in the tile at TestTileRow & TestTileColumn
    UINT sliceLeft = GoldTileId % 16;
    UINT goldTileZ = GoldTileId / 16;
    UINT goldTileY = sliceLeft / 4;
    UINT goldTileX = sliceLeft % 4;

    const BYTE* pGoldTile = GoldResource.GetTile(goldTileZ, goldTileY, goldTileX);
    const BYTE* pTestTile = TestResource.GetTile(TestTileSlice, TestTileRow, TestTileColumn);

    unsigned FailCount = 0;
    for (UINT Slice = 0; Slice < TileDepth; ++Slice)
    {
        for (UINT Row = 0; Row < TileHeight; ++Row)
        {
            for (UINT Column = 0; Column < TileWidth; ++Column)
            {
                const R8G8B8A8_PIXEL NullPixel = {};
                const R8G8B8A8_PIXEL* GoldPixel = UnmappedOrSkippedTile(GoldTileId) ? &NullPixel : GoldResource.GetPixel(pGoldTile, Slice, Row, Column);
                const R8G8B8A8_PIXEL* TestPixel = TestResource.GetPixel(pTestTile, Slice, Row, Column);
                if (*GoldPixel != *TestPixel)
                {
                    ++FailCount;
                    RecordError(UnmappedOrSkippedTile(GoldTileId), TestTileSlice, TestTileRow, TestTileColumn, GoldPixel, TestPixel);
                }
            }
        }
    }
    return FailCount;
}

void TileMappingValidator::RecordError(bool NullTile, UINT TileY, UINT TileX, const R8G8B8A8_PIXEL* Gold, const R8G8B8A8_PIXEL* Test)
{
    tstring* ErrorString = NullTile ? UnmappedString : MappedString;
    if (ErrorString->empty())
    {
        if (NullTile)
        {
            *ErrorString = FormatString(_T("NULL-mapped logical tile contains non-zero data at location X=%i, Y=%i"), TileX, TileY);
            return;
        }
        *ErrorString = 	FormatString(_T("Invalid data at logical tile location X=%i, Y=%i: observed 0x%2.2x%2.2x%2.2x%2.2x, Expected 0x%2.2x%2.2x%2.2x%2.2x"), 
                                        TileX, TileY,
                                        Test->r, Test->g, Test->b, Test->a,
                                        Gold->r, Gold->g, Gold->b, Gold->a);
    }
}

void TileMappingValidator::RecordError(bool NullTile, UINT TileZ, UINT TileY, UINT TileX, const R8G8B8A8_PIXEL* Gold, const R8G8B8A8_PIXEL* Test)
{
    tstring* ErrorString = NullTile ? UnmappedString : MappedString;
    if (ErrorString->empty())
    {
        if (NullTile)
        {
            *ErrorString = FormatString(_T("NULL-mapped logical tile contains non-zero data at location X=%i, Y=%i, Z=%i"), TileX, TileY, TileZ);
            return;
        }
        *ErrorString = FormatString(_T("Invalid data at logical tile location X=%i, Y=%i, Z=%i: observed 0x%2.2x%2.2x%2.2x%2.2x, Expected 0x%2.2x%2.2x%2.2x%2.2x"),
            TileX, TileY, TileZ,
            Test->r, Test->g, Test->b, Test->a,
            Gold->r, Gold->g, Gold->b, Gold->a);
    }
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::ValidateTileMapping()
////////////////////////////////////////
//
// The pStagedReference resource has texels layed out as though it were
// fully tiled in right-left, top-bottom order with all 16 (Texture2D) or 
// 64 (Texture3D) tiles from the tile pool.
//
// The pTiledResource texture is the tiled resource being tested for appropriate mapping
TEST_RESULT CTiledResourcesTest:: ValidateTileMapping(_In_ ID3D11Texture2D *pTiledResourceTest, _In_ ID3D11Texture2D *pReference, UINT mipLevel, UINT arraySlice, _In_ const CTileMapping &expectedMapping)
{
    return ValidateTileMapping < false, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, CTileMapping > (pTiledResourceTest, pReference, mipLevel, arraySlice, expectedMapping);
}

TEST_RESULT CTiledResourcesTest::ValidateTileMapping(_In_ ID3D11Texture3D *pTiledResourceTest, _In_ ID3D11Texture3D *pReference, UINT mipLevel, UINT unused, _In_ const CVolumeTileMapping &expectedMapping)
{
    return ValidateTileMapping < true, ID3D11Texture3D, D3D11_TEXTURE3D_DESC, CVolumeTileMapping > (pTiledResourceTest, pReference, mipLevel, unused, expectedMapping);
}

template <bool isTexture3D, class ID3D11_TEXTURE_TYPE, class D3D11_TEXTURE_DESC_TYPE, class TILE_MAPPING_TYPE>
TEST_RESULT CTiledResourcesTest::ValidateTileMapping(_In_ ID3D11_TEXTURE_TYPE *pTiledResource, _In_ ID3D11_TEXTURE_TYPE *pReference, UINT mipLevel, UINT arraySlice, const TILE_MAPPING_TYPE &expectedMapping)
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    CHandle<ID3D11_TEXTURE_TYPE> pStagedTiledResourceTest; // Test has the mapping described in the expectedMapping
    ID3D11DeviceContext2 *pDeviceContext = GetImmediateContext2();
    D3D11_TEXTURE_DESC_TYPE sourceDesc;
    D3D11_TEXTURE_DESC_TYPE refDesc;
    UINT unmappedFailCount = 0;
    UINT mappedFailCount = 0;
    tstring unmappedMessage;
    tstring mappedMessage;
    TILE_MAPPING_TYPE observedMapping;
    bool haveMapping[4][4][4] = { 0 };  // for Texture2D, won't go beyond slice 0

    pTiledResource->GetDesc(&sourceDesc);
    UINT mipWidth = sourceDesc.Width;
    UINT mipHeight = sourceDesc.Height;
    UINT mipDepth = 1;
    bool getMipDimensionsSuccess = false;
    if (isTexture3D)
    {
        // cast done to appease the compiler, but should never be called for Texture2D!
        mipDepth = ((D3D11_TEXTURE3D_DESC*)(&sourceDesc))->Depth;
        getMipDimensionsSuccess = CFormatHelper::GetMipDimensions(mipLevel, &mipWidth, &mipHeight, &mipDepth);
    }
    else
    {
        getMipDimensionsSuccess = CFormatHelper::GetMipDimensions(mipLevel, &mipWidth, &mipHeight);
    }
    if (!getMipDimensionsSuccess)
    {
        WriteToLog(_T("GetMipDimensions failed due to non-existent mip level"));
        return RESULT_FAIL;
    }

    // Create a staged copy of the tiled resources
    sourceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    sourceDesc.Usage = D3D11_USAGE_STAGING;
    sourceDesc.MiscFlags &= ~D3D11_RESOURCE_MISC_TILED;
    sourceDesc.BindFlags = 0;
    tr = CreateTexture(&sourceDesc, &pStagedTiledResourceTest);
    if (RESULT_PASS != tr)
    {
        return RESULT_FAIL;
    }

    UINT subResourceIndex = mipLevel + (isTexture3D ? 0 : arraySlice * sourceDesc.MipLevels);   // arrays of Texture3D are not supported
    pDeviceContext->CopySubresourceRegion(pStagedTiledResourceTest, subResourceIndex, 0, 0, 0, pTiledResource, subResourceIndex, NULL);

    // Map the most detailed mip of the reference image since it contains all the tiles
    // in order
    MapHelper<ID3D11_TEXTURE_TYPE> StagedReference(pDeviceContext, pReference, 0, D3D11_MAP_READ);
    hr = StagedReference.Map();
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, Map);
        return RESULT_FAIL;
    }

    // Map the mip level being tested from the tiled resource
    MapHelper<ID3D11_TEXTURE_TYPE> StagedTiledResource(pDeviceContext, pStagedTiledResourceTest, subResourceIndex, D3D11_MAP_READ);
    hr = StagedTiledResource.Map();
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, Map);
        return RESULT_FAIL;
    }

    observedMapping.Width = expectedMapping.Width;
    observedMapping.Height = expectedMapping.Height;
    UINT expectedMappingDepth = 1;
    __if_exists(TILE_MAPPING_TYPE::Depth)
    {
        expectedMappingDepth = expectedMapping.Depth;
        observedMapping.Depth = expectedMappingDepth;
    }

    TileMappingValidator Validator(StagedReference, StagedTiledResource, &mappedMessage, &unmappedMessage);

    // Get correct tile dimensions
    const UINT widthPerTile = (isTexture3D ? CResourceBaseTestHelper<TRB_TEXTURE_3D>::widthPerTile : CResourceBaseTestHelper<TRB_TEXTURE_2D>::widthPerTile);
    const UINT heightPerTile = (isTexture3D ? CResourceBaseTestHelper<TRB_TEXTURE_3D>::heightPerTile : CResourceBaseTestHelper<TRB_TEXTURE_2D>::heightPerTile);
    const UINT depthPerTile = (isTexture3D ? CResourceBaseTestHelper<TRB_TEXTURE_3D>::depthPerTile : 1);

    // Iterate through data and compare
    for (UINT tileZ = 0; tileZ < expectedMappingDepth && tileZ * depthPerTile < mipDepth; ++tileZ)
    {
        for (UINT tileY = 0; tileY < expectedMapping.Height && tileY * heightPerTile < mipHeight; ++tileY)
        {
            for (UINT tileX = 0; tileX < expectedMapping.Width && tileX * widthPerTile < mipWidth; ++tileX)
            {
                const UINT tileId = expectedMapping.Get(tileX, tileY, tileZ);
                if (m_parameters.pTiledResourcesTier->GetValue() == 1 && UnmappedOrSkippedTile(tileId))
                {
                    // Don't bother comparing data of unmapped tiles on tier 1 so just mark as "observed"
                    observedMapping.Set(0xffffffff, tileX, tileY, tileZ);
                    continue;
                }

                if (!haveMapping[tileX][tileY][tileZ])
                {
                    R8G8B8A8_PIXEL ObservedPixel = (isTexture3D ? Validator.GetTestTileContent(tileZ, tileY, tileX) : Validator.GetTestTileContent(tileY, tileX));
                    observedMapping.Set((0 == ObservedPixel.a ? 0xffffffff : ObservedPixel.r - 1), tileX, tileY, tileZ); // tileId = red - 1
                    haveMapping[tileX][tileY][tileZ] = true;
                }

                // Fix up the actual width / height of this tile if this tile extends beyond the edge of the surface.
                const UINT TileDepth = ((tileZ + 1) * depthPerTile <= mipDepth) ? depthPerTile : mipDepth - (tileZ * depthPerTile);
                const UINT TileHeight = ((tileY + 1) * heightPerTile <= mipHeight) ? heightPerTile : mipHeight - (tileY * heightPerTile);
                const UINT TileWidth = ((tileX + 1) * widthPerTile <= mipWidth) ? widthPerTile : mipWidth - (tileX * widthPerTile);

                UINT& FailCount = UnmappedOrSkippedTile(tileId) ? unmappedFailCount : mappedFailCount;
                FailCount += (isTexture3D ? Validator.Validate(tileId, tileZ, tileY, tileX, TileDepth, TileHeight, TileWidth)
                                          : Validator.Validate(tileId, tileY, tileX, TileHeight, TileWidth));
            }
        }
    }

    if (mappedFailCount > 0)
    {
        tr = RESULT_FAIL;

        WriteToLog(mappedMessage.c_str());

        if (mappedFailCount > 1)
            WriteToLog(FormatString(_T("%i additional mapped pixels contained incorrect data"), mappedFailCount - 1));
    }

    if (unmappedFailCount > 0)
    {
        tr = RESULT_FAIL;

        WriteToLog(unmappedMessage.c_str());

        if (unmappedFailCount > 1)
            WriteToLog(FormatString(_T("%i additional NULL mapped pixels contained non-zero data"), unmappedFailCount - 1));
    }

    if (!(expectedMapping == observedMapping))
    {
        tr = RESULT_FAIL;
        char strings[32][5];
        char *observedStrings[4][4] = { { strings[0], strings[1], strings[2], strings[3] },
                                        { strings[4], strings[5], strings[6], strings[7] },
                                        { strings[8], strings[9], strings[10], strings[11] },
                                        { strings[12], strings[13], strings[14], strings[15] } };
        char *expectedStrings[4][4] = { { strings[16], strings[17], strings[18], strings[19] },
                                        { strings[20], strings[21], strings[22], strings[23] },
                                        { strings[24], strings[25], strings[26], strings[27] },
                                        { strings[28], strings[29], strings[30], strings[31] } };

        // Write out expected tile mapping versus observed tile mapping
        const TILED_RESOURCE_BASE tileResourceBase = (isTexture3D ? TRB_TEXTURE_3D : TRB_TEXTURE_2D);
        CResourceBaseTestHelper<tileResourceBase>::GetTileMappingStrings(expectedMapping, expectedStrings);
        CResourceBaseTestHelper<tileResourceBase>::GetTileMappingStrings(observedMapping, observedStrings);
        tstring expectedFormat(_T("["));
        tstring observerdFormat(_T("["));
        for (UINT k = 0; k < expectedMappingDepth; ++k)
        {
            if (k > 0) {
                expectedFormat += tstring(_T("|"));
                observerdFormat += tstring(_T("|"));
            }
            for (UINT j = 0; j < expectedMapping.Height; ++j)
            {
                expectedFormat += tstring(_T("["));
                observerdFormat += tstring(_T("["));
                expectedFormat += ToString(std::string(expectedStrings[k][j]));
                observerdFormat += ToString(std::string(observedStrings[k][j]));
                expectedFormat += tstring(_T("]"));
                observerdFormat += tstring(_T("]"));
            }
        }
        expectedFormat += tstring(_T("]"));
        observerdFormat += tstring(_T("]"));

        WriteToLog(_T("Expected tile mapping:"));
        WriteToLog(expectedFormat.c_str());
        WriteToLog(_T("Possible observed tile mapping:"));
        WriteToLog(observerdFormat.c_str());
    }

    return tr;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::CompareTiledResourceToReference()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::CompareTiledResourceToReference(_In_ ID3D11Texture2D *pTiledResourceTest, _In_ ID3D11Texture2D *pReference, UINT mipLevel, UINT arraySlice, _In_ const CTileMapping &expectedMapping, UINT tileWidth, UINT tileHeight, UINT unused)
{
    return CompareTiledResourceToReference < false, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, CTileMapping > (pTiledResourceTest, pReference, mipLevel, arraySlice, expectedMapping, tileWidth, tileHeight, unused);
}

TEST_RESULT CTiledResourcesTest::CompareTiledResourceToReference(_In_ ID3D11Texture3D *pTiledResourceTest, _In_ ID3D11Texture3D *pReference, UINT mipLevel, UINT unused, _In_ const CVolumeTileMapping &expectedMapping, UINT tileWidth, UINT tileHeight, UINT tileDepth)
{
    return CompareTiledResourceToReference < true, ID3D11Texture3D, D3D11_TEXTURE3D_DESC, CVolumeTileMapping > (pTiledResourceTest, pReference, mipLevel, unused, expectedMapping, tileWidth, tileHeight, tileDepth);
}

template <bool isTexture3D, class ID3D11_TEXTURE_TYPE, class D3D11_TEXTURE_DESC_TYPE, class TILE_MAPPING_TYPE>
TEST_RESULT CTiledResourcesTest::CompareTiledResourceToReference(_In_ ID3D11_TEXTURE_TYPE *pTiledResource, _In_ ID3D11_TEXTURE_TYPE *pReference, UINT mipLevel, UINT arraySlice, const TILE_MAPPING_TYPE &expectedMapping, UINT tileWidth, UINT tileHeight, UINT tileDepth)
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    CHandle<ID3D11_TEXTURE_TYPE> pStagedTiledResourceTest; // Test has the mapping described in the expectedMapping
    ID3D11DeviceContext2 *pDeviceContext = GetImmediateContext2();
    CHandle<ID3D11_TEXTURE_TYPE> pStagedReference = pReference;
    D3D11_MAPPED_SUBRESOURCE mappedTest;
    D3D11_MAPPED_SUBRESOURCE mappedRef;
    D3D11_TEXTURE_DESC_TYPE sourceDesc;
    D3D11_TEXTURE_DESC_TYPE refDesc;
    UINT unmappedFailCount = 0;
    UINT mappedFailCount = 0;
    tstring unmappedMessage;
    tstring mappedMessage;

    // See if we need to create a staging copy of the reference texture
    pReference->GetDesc(&refDesc);
    UINT subResourceIndex = mipLevel + (isTexture3D ? 0 : arraySlice * refDesc.MipLevels);  // arrays of Texture3D are not supported
    if (0 == (refDesc.CPUAccessFlags & D3D11_CPU_ACCESS_READ))
    {
        // Release the reference to the input pReference value
        pStagedReference->Release();

        // Create a copy of the reference texture
        refDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        refDesc.Usage = D3D11_USAGE_STAGING;
        refDesc.BindFlags = 0;
        refDesc.MiscFlags &= ~D3D11_RESOURCE_MISC_TILED;
        tr = CreateTexture(&refDesc, &pStagedReference);
        if (RESULT_PASS != tr)
        {
            return RESULT_FAIL;
        }

        pDeviceContext->CopySubresourceRegion(pStagedReference, subResourceIndex, 0, 0, 0, pReference, subResourceIndex, nullptr);
    }

    pTiledResource->GetDesc(&sourceDesc);
    UINT mipWidth = sourceDesc.Width;
    UINT mipHeight = sourceDesc.Height;
    UINT mipDepth = 1;
    bool getMipDimensionsSuccess = false;
    if (isTexture3D)
    {
        // cast done to appease the compiler, but should never be called for Texture2D!
        mipDepth = ((D3D11_TEXTURE3D_DESC*)(&sourceDesc))->Depth;
        getMipDimensionsSuccess = CFormatHelper::GetMipDimensions(mipLevel, &mipWidth, &mipHeight, &mipDepth);
    }
    else
    {
        getMipDimensionsSuccess = CFormatHelper::GetMipDimensions(mipLevel, &mipWidth, &mipHeight);
    }
    if (!getMipDimensionsSuccess)
    {
        WriteToLog(_T("GetMipDimensions failed due to non-existent mip level"));
        return RESULT_FAIL;
    }

    assert(sourceDesc.Format == refDesc.Format);

    if (isTexture3D)
    {
        if (0 == tileWidth || 0 == tileHeight || 0 == tileDepth)
        {
            GetTileSize(sourceDesc.Format, tileWidth, tileHeight, tileDepth);
        }
    }
    else
    {
        if (0 == tileWidth || 0 == tileHeight)
        {
            GetTileSize(sourceDesc.Format, tileWidth, tileHeight);
        }
    }

    // Create a staged copy of the tiled resources
    sourceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    sourceDesc.Usage = D3D11_USAGE_STAGING;
    sourceDesc.MiscFlags &= ~D3D11_RESOURCE_MISC_TILED;
    sourceDesc.BindFlags = 0;
    tr = CreateTexture(&sourceDesc, &pStagedTiledResourceTest);
    if (RESULT_PASS != tr)
    {
        return RESULT_FAIL;
    }

    subResourceIndex = mipLevel + (isTexture3D ? 0 : arraySlice * sourceDesc.MipLevels);    // arrays of Texture3D are not supported
    pDeviceContext->CopySubresourceRegion(pStagedTiledResourceTest, subResourceIndex, 0, 0, 0, pTiledResource, subResourceIndex, NULL);

    // Map the mip level being tested of the reference image since it contains all the tiles
    // in order
    hr = pDeviceContext->Map(pStagedReference, subResourceIndex, D3D11_MAP_READ, 0, &mappedRef);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, Map);
        return RESULT_FAIL;
    }

    // Map the mip level being tested from the tiled resource
    hr = pDeviceContext->Map(pStagedTiledResourceTest, subResourceIndex, D3D11_MAP_READ, 0, &mappedTest);
    if (FAILED(hr))
    {
        pDeviceContext->Unmap(pStagedReference, mipLevel);
        REPORT_HRESULT_FAILURE(hr, Map);
        return RESULT_FAIL;
    }

    UINT elementSize = CD3D10FormatHelper_Test::GetBitsPerElement(refDesc.Format) / 8;

    // fix depth when using Texture2D
    tileDepth = (isTexture3D ? tileDepth : 1);
    UINT expectedMappingDepth = 1;
    __if_exists(TILE_MAPPING_TYPE::Depth)
    {
        expectedMappingDepth = expectedMapping.Depth;
    }

    // Iterate through data and compare
    for (UINT tileZ = 0; tileZ < expectedMappingDepth; ++tileZ)
    {
        for (UINT tileY = 0; tileY < expectedMapping.Height; ++tileY)
        {
            for (UINT tileX = 0; tileX < expectedMapping.Width; ++tileX)
            {
                const UINT tileId = expectedMapping.Get(tileX, tileY, tileZ);

                if (m_parameters.pTiledResourcesTier->GetValue() == 1 && UnmappedOrSkippedTile(tileId))
                {
                    // Don't bother comparing data of unmapped tiles on tier 1
                    continue;
                }

                for (UINT k = 0; k < tileDepth; ++k)
                {
                    UINT z = k + tileDepth * tileZ;

                    if (z >= mipDepth)
                        break;

                    for (UINT j = 0; j < tileHeight; ++j)
                    {
                        UINT y = j + tileHeight * tileY;

                        if (y >= mipHeight)
                            break;

                        for (UINT i = 0; i < tileWidth; ++i)
                        {
                            UINT x = i + tileWidth * tileX;

                            if (x >= mipWidth)
                                break;

                            TEXTURE_PIXEL_POINTER pixelRowTest;
                            TEXTURE_PIXEL_POINTER pixelTest;
                            pixelRowTest.pBits = reinterpret_cast<BYTE *>(z * mappedTest.DepthPitch + y * mappedTest.RowPitch + (BYTE *)mappedTest.pData);
                            pixelTest.pBits = pixelRowTest.pBits + x * elementSize;

                            TEXTURE_PIXEL_POINTER pixelRowRef;
                            TEXTURE_PIXEL_POINTER pixelRef;
                            pixelRowRef.pBits = reinterpret_cast<BYTE *>(z * mappedRef.DepthPitch + y * mappedRef.RowPitch + (BYTE *)mappedRef.pData);
                            pixelRef.pBits = pixelRowRef.pBits + x * elementSize;

                            if (UnmappedOrSkippedTile(tileId))
                            {
                                bool pass = true;
                                switch (elementSize)
                                {
                                case 4:
                                    if (pixelTest.pR32Data->r != 0)
                                    {
                                        pass = false;
                                    }
                                    break;

                                case 2:
                                    if (pixelTest.pR16Data->r != 0)
                                    {
                                        pass = false;
                                    }
                                    break;

                                case 1:
                                    if (pixelTest.pR8Data->r != 0)
                                    {
                                        pass = false;
                                    }
                                    break;

                                default:
                                    pDeviceContext->Unmap(pStagedReference, mipLevel);
                                    pDeviceContext->Unmap(pStagedTiledResourceTest, mipLevel);
                                    WriteToLog(_T("Resource format not handled by CompareTiledResourceToReference()"));
                                    return RESULT_FAIL;
                                    break;

                                }

                                if (!pass)
                                {
                                    unmappedFailCount++;
                                    if (unmappedMessage.empty())
                                    {
                                        if (isTexture3D)
                                        {
                                            unmappedMessage = FormatString(_T("NULL-mapped logical tile contains non-zero data at location X=%i, Y=%i, Z=%i"), tileX, tileY, tileZ);
                                        }
                                        else
                                        {
                                            unmappedMessage = FormatString(_T("NULL-mapped logical tile contains non-zero data at location X=%i, Y=%i"), tileX, tileY);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                bool pass = true;
                                tstring observed;
                                tstring expected;

                                switch (elementSize)
                                {
                                case 4:
                                    if (pixelTest.pR32Data->r != pixelRef.pR32Data->r)
                                    {
                                        pass = false;
                                        observed = FormatString("%8.8x", pixelTest.pR32Data->r);
                                        expected = FormatString("%8.8x", pixelRef.pR32Data->r);
                                    }
                                    break;

                                case 2:
                                    if (pixelTest.pR16Data->r != pixelRef.pR16Data->r)
                                    {
                                        pass = false;
                                        observed = FormatString("%4.4x", pixelTest.pR16Data->r);
                                        expected = FormatString("%4.4x", pixelRef.pR16Data->r);
                                    }
                                    break;

                                case 1:
                                    if (pixelTest.pR8Data->r != pixelRef.pR8Data->r)
                                    {
                                        pass = false;
                                        observed = FormatString("%2.2x", pixelTest.pR8Data->r);
                                        expected = FormatString("%2.2x", pixelRef.pR8Data->r);
                                    }
                                    break;

                                default:
                                    pDeviceContext->Unmap(pStagedReference, mipLevel);
                                    pDeviceContext->Unmap(pStagedTiledResourceTest, mipLevel);
                                    WriteToLog(_T("Resource format not handled by CompareTiledResourceToReference()"));
                                    return RESULT_FAIL;
                                    break;

                                }

                                if (!pass)
                                {
                                    mappedFailCount++;
                                    if (mappedMessage.empty())
                                    {
                                        if (isTexture3D)
                                        {
                                            mappedMessage = FormatString(_T("Invalid data at logical tile location X=%i, Y=%i, Z=%i (pixel coordinate x=%i, y=%i, z=%i): observed 0x%s, Expected 0x%s"),
                                                                         tileX, tileY, tileZ, x, y, z, observed.c_str(), expected.c_str());
                                        }
                                        else
                                        {
                                            mappedMessage = FormatString(_T("Invalid data at logical tile location X=%i, Y=%i (pixel coordinate x=%i, y=%i): observed 0x%s, Expected 0x%s"),
                                                                         tileX, tileY, x, y, observed.c_str(), expected.c_str());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (mappedFailCount > 0)
                {
                    tr = RESULT_FAIL;

                    WriteToLog(mappedMessage.c_str());

                    if (mappedFailCount > 1)
                    {
                        WriteToLog(FormatString(_T("%i additional mapped pixels contained incorrect data"), mappedFailCount - 1));
                    }
                }

                if (unmappedFailCount > 0)
                {
                    tr = RESULT_FAIL;

                    WriteToLog(unmappedMessage.c_str());

                    if (unmappedFailCount > 1)
                    {
                        WriteToLog(FormatString(_T("%i additional NULL mapped pixels contained non-zero data"), unmappedFailCount - 1));
                    }
                }

                mappedFailCount = 0;
                unmappedFailCount = 0;
                mappedMessage.clear();
                unmappedMessage.clear();
            }
        }
    }

    pDeviceContext->Unmap(pStagedReference, mipLevel);
    pDeviceContext->Unmap(pStagedTiledResourceTest, mipLevel);

    return tr;
}


////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::ValidateTileAgainstBuffer()
////////////////////////////////////////
//
// The buffer contains each tile laid out linearly
// Tiles with ID = -1 should be black, tiles with ID = -2 should be cleared
TEST_RESULT CTiledResourcesTest::ValidateTileAgainstBuffer(_In_ ID3D11Texture2D *pTiledResourceTest, _In_ R8G8B8A8_PIXEL *pBuffer, UINT mipLevel, UINT arraySlice, _In_ const CTileMapping &expectedMapping, float clearColor[4])
{
    return ValidateTileAgainstBuffer < false, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, CTileMapping > (pTiledResourceTest, pBuffer, mipLevel, arraySlice, expectedMapping, clearColor);
}
TEST_RESULT CTiledResourcesTest::ValidateTileAgainstBuffer(_In_ ID3D11Texture3D *pTiledResourceTest, _In_ R8G8B8A8_PIXEL *pBuffer, UINT mipLevel, UINT unused, _In_ const CVolumeTileMapping &expectedMapping, float clearColor[4])
{
    return ValidateTileAgainstBuffer < true, ID3D11Texture3D, D3D11_TEXTURE3D_DESC, CVolumeTileMapping > (pTiledResourceTest, pBuffer, mipLevel, unused, expectedMapping, clearColor);
}

template <bool isTexture3D, class ID3D11_TEXTURE_TYPE, class D3D11_TEXTURE_DESC_TYPE, class TILE_MAPPING_TYPE>
TEST_RESULT CTiledResourcesTest::ValidateTileAgainstBuffer(_In_ ID3D11_TEXTURE_TYPE *pTiledResource, _In_ R8G8B8A8_PIXEL *pBuffer, UINT mipLevel, UINT arraySlice, _In_ const TILE_MAPPING_TYPE &expectedMapping, float clearColor[4])
{
    const R8G8B8A8_PIXEL ClearedPixel = { (BYTE)(clearColor[0] * 255.0f + 0.5f),
        (BYTE)(clearColor[1] * 255.0f + 0.5f),
        (BYTE)(clearColor[2] * 255.0f + 0.5f),
        (BYTE)(clearColor[3] * 255.0f + 0.5f) };
    const R8G8B8A8_PIXEL NULLPixel = { 0, 0, 0, 0 };

    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    CHandle<ID3D11_TEXTURE_TYPE> pStagedTiledResourceTest; // Test has the mapping described in the expectedMapping
    ID3D11DeviceContext2 *pDeviceContext = GetImmediateContext2();
    D3D11_MAPPED_SUBRESOURCE mappedTest;
    D3D11_TEXTURE_DESC_TYPE sourceDesc;
    UINT failCount = 0;
    tstring message;

    pTiledResource->GetDesc(&sourceDesc);
    UINT mipWidth = sourceDesc.Width;
    UINT mipHeight = sourceDesc.Height;
    UINT mipDepth = 1;
    bool getMipDimensionsSuccess = false;
    if (isTexture3D)
    {
        // cast done to appease the compiler, but should never be called for Texture2D!
        mipDepth = ((D3D11_TEXTURE3D_DESC*)(&sourceDesc))->Depth;
        getMipDimensionsSuccess = CFormatHelper::GetMipDimensions(mipLevel, &mipWidth, &mipHeight, &mipDepth);
    }
    else
    {
        getMipDimensionsSuccess = CFormatHelper::GetMipDimensions(mipLevel, &mipWidth, &mipHeight);
    }
    if (!getMipDimensionsSuccess)
    {
        WriteToLog(_T("GetMipDimensions failed due to non-existent mip level"));
        return RESULT_FAIL;
    }

    // Create a staged copy of the tiled resources
    sourceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    sourceDesc.Usage = D3D11_USAGE_STAGING;
    sourceDesc.MiscFlags &= ~D3D11_RESOURCE_MISC_TILED;
    sourceDesc.BindFlags = 0;
    tr = CreateTexture(&sourceDesc, &pStagedTiledResourceTest);
    if (RESULT_PASS != tr)
    {
        return RESULT_FAIL;
    }

    UINT subResourceIndex = mipLevel + (isTexture3D ? 0 : arraySlice * sourceDesc.MipLevels); // arrays of Texture3D are not supported
    pDeviceContext->CopySubresourceRegion(pStagedTiledResourceTest, subResourceIndex, 0, 0, 0, pTiledResource, subResourceIndex, NULL);

    // Map the mip level being tested from the tiled resource
    hr = pDeviceContext->Map(pStagedTiledResourceTest, subResourceIndex, D3D11_MAP_READ, 0, &mappedTest);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, Map);
        return RESULT_FAIL;
    }

    // Set Width, Height, Depth numbers for our tiles, depending on Texture2D or Texture3D
    const UINT widthPerTile = (isTexture3D ? CResourceBaseTestHelper<TRB_TEXTURE_3D>::widthPerTile : CResourceBaseTestHelper<TRB_TEXTURE_2D>::widthPerTile);
    const UINT heightPerTile = (isTexture3D ? CResourceBaseTestHelper<TRB_TEXTURE_3D>::heightPerTile : CResourceBaseTestHelper<TRB_TEXTURE_2D>::heightPerTile);
    const UINT depthPerTile = (isTexture3D ? CResourceBaseTestHelper<TRB_TEXTURE_3D>::depthPerTile : 1);
    UINT expectedMappingDepth = 1;
    __if_exists(TILE_MAPPING_TYPE::Depth)
    {
        expectedMappingDepth = expectedMapping.Depth;
    }

    R8G8B8A8_PIXEL *tiledContents = reinterpret_cast<R8G8B8A8_PIXEL*>(mappedTest.pData);
    for (UINT tileZ = 0; tileZ < expectedMappingDepth; ++tileZ)
    {
        for (UINT tileY = 0; tileY < expectedMapping.Height; ++tileY)
        {
            for (UINT tileX = 0; tileX < expectedMapping.Width; ++tileX)
            {
                const UINT tileId = expectedMapping.Get(tileX, tileY, tileZ);
                if (m_parameters.pTiledResourcesTier->GetValue() == 1 && UnmappedOrSkippedTile(tileId))
                {
                    // Don't bother comparing data of unmapped tiles on tier 1
                    continue;
                }

                for (UINT k = 0; k < depthPerTile; ++k)
                {
                    const UINT z = tileZ * depthPerTile + k;
                    if (z >= mipDepth)
                        break;

                    for (UINT j = 0; j < heightPerTile; ++j)
                    {
                        const UINT y = tileY * heightPerTile + j;
                        if (y >= mipHeight)
                            break;

                        for (UINT i = 0; i < widthPerTile; ++i)
                        {
                            const UINT x = tileX * widthPerTile + i;
                            if (x >= mipWidth)
                                break;

                            const UINT mappedOffsetBytes = (isTexture3D ? z * mappedTest.DepthPitch : 0) + y * mappedTest.RowPitch;
                            const UINT mappedOffsetPixels = mappedOffsetBytes / sizeof(R8G8B8A8_PIXEL) + x;
                            R8G8B8A8_PIXEL *pTiledPixel = tiledContents + mappedOffsetPixels;

                            if (UnmappedTile(tileId))
                            {
                                if (*pTiledPixel != NULLPixel)
                                {
                                    ++failCount;
                                    if (message.empty())
                                    {
                                        if (isTexture3D)
                                        {
                                            message = FormatString(_T("NULL-mapped logical tile contains non-zero data at location X=%i, Y=%i, Z=%i"), tileX, tileY, tileZ);
                                        }
                                        else
                                        {
                                            message = FormatString(_T("NULL-mapped logical tile contains non-zero data at location X=%i, Y=%i"), tileX, tileY);
                                        }
                                    }
                                }
                            }
                            else if (SkippedTile(tileId))
                            {
                                if (*pTiledPixel != ClearedPixel)
                                {
                                    ++failCount;
                                    if (message.empty())
                                    {
                                        if (isTexture3D)
                                        {
                                            message = FormatString(_T("Data changed in tile outside the copy region, X=%i, Y=%i, Z=%i: observed 0x%08x, Expected 0x%08x"),
                                                tileX, tileY, tileZ, *(UINT32*)pTiledPixel, *(UINT32*)&ClearedPixel);
                                        }
                                        else
                                        {
                                            message = FormatString(_T("Data changed in tile outside the copy region, X=%i, Y=%i: observed 0x%08x, Expected 0x%08x"),
                                                tileX, tileY, *(UINT32*)pTiledPixel, *(UINT32*)&ClearedPixel);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                const UINT bufferOffsetBytes = tileId * TILE_SIZE_IN_BYTES;
                                const UINT bufferOffsetPixels = bufferOffsetBytes / sizeof(R8G8B8A8_PIXEL) + (k * (widthPerTile * heightPerTile)) + (j * widthPerTile) + i;
                                R8G8B8A8_PIXEL *pBufferPixel = pBuffer + bufferOffsetPixels;

                                if (*pTiledPixel != *pBufferPixel)
                                {
                                    ++failCount;
                                    if (message.empty())
                                    {
                                        if (isTexture3D)
                                        {
                                            message = FormatString(_T("Invalid data at logical tile location X=%i, Y=%i, Z=%i: observed 0x%08x, Expected 0x%08x"),
                                                tileX, tileY, tileZ, *(UINT32*)pTiledPixel, *(UINT32*)pBufferPixel);
                                        }
                                        else
                                        {
                                            message = FormatString(_T("Invalid data at logical tile location X=%i, Y=%i: observed 0x%08x, Expected 0x%08x"),
                                                tileX, tileY, *(UINT32*)pTiledPixel, *(UINT32*)pBufferPixel);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (failCount)
                {
                    tr = RESULT_FAIL;
                    WriteToLog(message.c_str());
                    if (failCount > 1)
                    {
                        WriteToLog(FormatString(_T("%d additional errors found in the same tile."), failCount - 1));
                    }
                }
                failCount = 0;
                message.clear();
            }
        }
    }

    pDeviceContext->Unmap(pStagedTiledResourceTest, mipLevel);
    return tr;
}

////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::ValidateBufferAgainstOriginal()
////////////////////////////////////////
//
// The buffer contains each tile laid out linearly
// The buffers should be identical except for tiles that were non-resident
TEST_RESULT CTiledResourcesTest::ValidateBufferAgainstOriginal(_In_ ID3D11Buffer *pOutputBuffer, _In_ R8G8B8A8_PIXEL *pOriginal, UINT numPixels, UINT bufferOffset, _In_ const CBufferMappingBaseParent &expectedMapping)
{
    const R8G8B8A8_PIXEL NULLPixel = { 0, 0, 0, 0 };
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    CHandle<ID3D11Buffer> pStagedBufferTest;
    ID3D11DeviceContext2 *pDeviceContext = GetImmediateContext2();
    D3D11_MAPPED_SUBRESOURCE mappedTest;
    D3D11_BUFFER_DESC sourceDesc;
    UINT failCount = 0;
    tstring message;
    pOutputBuffer->GetDesc(&sourceDesc);

    const UINT numPixelsPerTile = TILE_SIZE_IN_BYTES / sizeof(R8G8B8A8_PIXEL);
    const UINT numTiles = numPixels / numPixelsPerTile;

    // Create a staged copy of the tiled resources
    sourceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    sourceDesc.Usage = D3D11_USAGE_STAGING;
    sourceDesc.BindFlags = 0;
    hr = GetDevice2()->CreateBuffer(&sourceDesc, NULL, &pStagedBufferTest);
    if(FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateBuffer);
        return RESULT_FAIL;
    }

    pDeviceContext->CopyResource(pStagedBufferTest, pOutputBuffer);

    hr = pDeviceContext->Map(pStagedBufferTest, 0, D3D11_MAP_READ, 0, &mappedTest);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, Map);
        return RESULT_FAIL;
    }

    R8G8B8A8_PIXEL *pTestPixels = reinterpret_cast<R8G8B8A8_PIXEL*>(reinterpret_cast<BYTE*>(mappedTest.pData) + bufferOffset);
    for (UINT i = 0; i < numTiles; ++i)
    {
        UINT tileId = expectedMapping.GetTile(i);
        if (m_parameters.pTiledResourcesTier->GetValue() == 1 && 0x0000ffff < tileId)
        {
            // Don't bother comparing data of unmapped tiles on tier 1
            continue;
        }

        R8G8B8A8_PIXEL *pSourceTile = pOriginal + (numPixelsPerTile * i);
        R8G8B8A8_PIXEL *pTestTile = pTestPixels + (numPixelsPerTile * i);
        for (UINT j = 0; j < numPixelsPerTile; ++j, ++pSourceTile, ++pTestTile)
        {
            if (UnmappedOrSkippedTile(tileId))
            {
                if (*pTestTile != NULLPixel)
                {
                    failCount++;
                    if (message.empty())
                        message = FormatString(_T("CopyTiles from NULL-mapped tile produced nonzero results at buffer tile %i"), i);
                }
            }
            else if (*pTestTile != *pSourceTile)
            {
                failCount++;
                if (message.empty())
                    message = FormatString(_T("CopyTiles destination buffer does not match original contents. Tile %i, offset %i, observed 0x%08x, Expected 0x%08x"),
                                           i, j, *(UINT32*)pTestTile, *(UINT32*)pSourceTile);
            }
        }

        if (failCount)
        {
            tr = RESULT_FAIL;
            WriteToLog(message.c_str());
            if (failCount > 1)
                WriteToLog(FormatString(_T("%d additional errors found in the same tile."), failCount - 1));
        }
        failCount = 0;
        message.clear();
    }

    pDeviceContext->Unmap(pStagedBufferTest, 0);
    return tr;
}


TEST_RESULT CTiledResourcesTest::CheckNullSentinel()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext *pContext = GetImmediateContext2();
    CHandle<ID3D11Texture2D> pStagedTexture;

    if(nullptr == m_pNullSentinelTexture)
        return tr;

    // Create a staged copy of the sentinal texture
    D3D11_TEXTURE2D_DESC desc;
    m_pNullSentinelTexture->GetDesc(&desc);
    desc.MiscFlags = 0;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.BindFlags = 0;
    hr = pDevice->CreateTexture2D(&desc, NULL, &pStagedTexture);
    if(FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateTexture2D);
        return RESULT_FAIL;
    }

    pContext->CopyResource(pStagedTexture, m_pNullSentinelTexture);

    // Read the data in the staged resource and make sure it is all zeros
    D3D11_MAPPED_SUBRESOURCE mapData;
    hr = pContext->Map(pStagedTexture, 0, D3D11_MAP_READ, 0, &mapData);
    if(FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, Map);
        return RESULT_FAIL;
    }

    union
    {
        BYTE *pBits;
        UINT *pRowData;
    };

    pBits = reinterpret_cast<BYTE *>(mapData.pData);
    for(UINT y = 0; y < desc.Height; ++y)
    {
        for(UINT x = 0; x < desc.Width; ++x)
        {
            if(0 != pRowData[x])
            {
                WriteToLog(_T("Non-zero data read from NULL-mapped sentinel resource"));
                tr = RESULT_FAIL;
                goto end;
            }
        }

        pBits += mapData.RowPitch;
    }

end:
    pContext->Unmap(pStagedTexture, 0);

    return tr;
}

TEST_RESULT CTiledResourcesTest::TestNullTiles(_In_ ID3D11Texture2D *pTiledResource, UINT mipLevel, UINT arraySlice, _In_ const CTileMapping &tileMapping)
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    CHandle<ID3D11Texture2D> pStagingTexture;
    D3D11_RECT rects[16];

    D3D11_TEXTURE2D_DESC desc;
    pTiledResource->GetDesc(&desc);

    UINT bytesPerPixel = CD3D10FormatHelper_Test::GetBitsPerElementThrow(desc.Format) / 8;
    UINT byteWidth = desc.Width * bytesPerPixel;

    // Find the first NULL-mapped tile
    UINT nullTileCount = GetBlankRects(tileMapping, rects, desc.Format);
    UINT tileWidth = rects[0].right - rects[0].left;
    UINT tileHeight = rects[0].bottom - rects[0].top;
    UINT tileWidthInBytes = tileWidth * bytesPerPixel;
    UINT tileHeightInBytes = tileHeight * bytesPerPixel;

    if(nullTileCount < 2)
        return RESULT_PASS; // Nothing to see here

    UINT subResourceIndex = mipLevel + arraySlice * desc.MipLevels;

    // Create a staging resource similar to the resource provided
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE|D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    hr = pDevice->CreateTexture2D(&desc, NULL, &pStagingTexture);
    if(FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateTexture2D);
        return RESULT_FAIL;
    }

    // Map the staging resource and write "noise" to it
    D3D11_MAPPED_SUBRESOURCE mapData;
    hr = pContext->Map(pStagingTexture, 0, D3D11_MAP_WRITE, 0, &mapData);
    if(FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, Map);
        return RESULT_FAIL;
    }
    BYTE noise = 101;
    BYTE *pRow = reinterpret_cast<BYTE *>(mapData.pData);
    for(UINT y = 0; y < desc.Height; ++y)
    {
        for(UINT x = 0; x < tileWidthInBytes && x < byteWidth; ++x)
        {
            pRow[x] = noise;
            noise = static_cast<BYTE>(((noise * x) >> 1) & 0x00ff) ^ 73; // contrived noise encoding
        }
        pRow += mapData.RowPitch;
    }
    pContext->Unmap(pStagingTexture, 0);

    // Copy staged data to the first null tile
    D3D11_BOX srcBox;
    srcBox.left = 0;
    srcBox.top = 0;
    srcBox.front = 0;
    srcBox.right = tileWidth;
    srcBox.bottom = tileHeight;
    srcBox.back = 1;
    pContext->CopySubresourceRegion1(pTiledResource, subResourceIndex, rects[0].left, rects[0].top, 0, pStagingTexture, 0, &srcBox, 0);

    // Now read data from the second null tile
    srcBox.left = rects[1].left;
    srcBox.right = rects[1].right;
    srcBox.top = rects[1].top;
    srcBox.bottom = rects[1].bottom;
    pContext->CopySubresourceRegion1(pStagingTexture, 0, 0, 0, 0, pTiledResource, subResourceIndex, &srcBox, 0);

    // Make sure the second null tile data is black
    pContext->Map(pStagingTexture, 0, D3D11_MAP_READ, 0, &mapData);
    if(FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, Map);
        return RESULT_FAIL;
    }

    pRow = reinterpret_cast<BYTE *>(mapData.pData);
    for(UINT y = 0; y < desc.Height; ++y)
    {
        for(UINT x = 0; x < tileWidthInBytes && x < byteWidth; ++x)
        {
            if(0x00 != pRow[x])
                tr = RESULT_FAIL;
        }
        pRow += mapData.RowPitch;
    }

    pContext->Unmap(pStagingTexture, 0);

    if(RESULT_PASS != tr)
    {
        WriteToLog(_T("Corruption reading from NULL-mapped tile after writing to a different NULL tile in the same resource"));
    }

    return tr;
}

TEST_RESULT CTiledResourcesTest::TestNullTiles(_In_ ID3D11Texture3D *pTiledResource, UINT mipLevel, UINT unused, _In_ const CVolumeTileMapping &tileMapping)
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    CHandle<ID3D11Texture3D> pStagingTexture;
    D3D11_BOX boxes[64];

    D3D11_TEXTURE3D_DESC desc;
    pTiledResource->GetDesc(&desc);

    UINT bytesPerPixel = CD3D10FormatHelper_Test::GetBitsPerElementThrow(desc.Format) / 8;
    UINT byteWidth = desc.Width * bytesPerPixel;
    UINT byteHeight = desc.Height * bytesPerPixel;

    // Find the first NULL-mapped tile
    UINT nullTileCount = GetBlankBoxes(tileMapping, boxes, desc.Format);
    UINT tileWidth = boxes[0].right - boxes[0].left;
    UINT tileHeight = boxes[0].bottom - boxes[0].top;
    UINT tileDepth = boxes[0].back - boxes[0].front;
    UINT tileWidthInBytes = tileWidth * bytesPerPixel;
    UINT tileHeightInBytes = tileHeight * bytesPerPixel;
    UINT tileDepthInBytes = tileDepth * bytesPerPixel;

    if (nullTileCount < 2)
    {
        return RESULT_PASS; // Nothing to see here
    }

    UINT subResourceIndex = mipLevel; // arrays of Texture3D are not supported

    // Create a staging resource similar to the resource provided
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.MipLevels = 1;
    hr = pDevice->CreateTexture3D(&desc, NULL, &pStagingTexture);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateTexture3D);
        return RESULT_FAIL;
    }

    // Map the staging resource and write "noise" to it
    D3D11_MAPPED_SUBRESOURCE mapData;
    hr = pContext->Map(pStagingTexture, 0, D3D11_MAP_WRITE, 0, &mapData);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, Map);
        return RESULT_FAIL;
    }
    BYTE noise = 101;
    BYTE *pSlice = reinterpret_cast<BYTE *>(mapData.pData);
    for (UINT z = 0; z < desc.Depth; ++z)
    {
        BYTE *pRow = pSlice;
        for (UINT y = 0; y < tileHeightInBytes && y < byteHeight; ++y)
        {
            for (UINT x = 0; x < tileWidthInBytes && x < byteWidth; ++x)
            {
                pRow[x] = noise;
                noise = static_cast<BYTE>(((noise * x) >> 1) & 0x00ff) ^ 73; // contrived noise encoding
            }
            pRow += mapData.RowPitch;
        }
        pSlice += mapData.DepthPitch;
    }
    pContext->Unmap(pStagingTexture, 0);

    // Copy staged data to the first null tile
    D3D11_BOX srcBox;
    srcBox.left = 0;
    srcBox.top = 0;
    srcBox.front = 0;
    srcBox.right = tileWidth;
    srcBox.bottom = tileHeight;
    srcBox.back = tileDepth;
    pContext->CopySubresourceRegion1(pTiledResource, subResourceIndex, boxes[0].left, boxes[0].top, boxes[0].front, pStagingTexture, 0, &srcBox, 0);

    // Now read data from the second null tile
    srcBox.left = boxes[1].left;
    srcBox.right = boxes[1].right;
    srcBox.top = boxes[1].top;
    srcBox.bottom = boxes[1].bottom;
    srcBox.front = boxes[1].front;
    srcBox.back = boxes[1].back;
    pContext->CopySubresourceRegion1(pStagingTexture, 0, 0, 0, 0, pTiledResource, subResourceIndex, &srcBox, 0);

    // Make sure the second null tile data is black
    pContext->Map(pStagingTexture, 0, D3D11_MAP_READ, 0, &mapData);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, Map);
        return RESULT_FAIL;
    }

    pSlice = reinterpret_cast<BYTE *>(mapData.pData);
    for (UINT z = 0; z < desc.Depth; ++z)
    {
        BYTE *pRow = pSlice;
        for (UINT y = 0; y < tileHeightInBytes && y < byteHeight; ++y)
        {
            for (UINT x = 0; x < tileWidthInBytes && x < byteWidth; ++x)
            {
                if (0x00 != pRow[x])
                    tr = RESULT_FAIL;
            }
            pRow += mapData.RowPitch;
        }
        pSlice += mapData.DepthPitch;
    }

    pContext->Unmap(pStagingTexture, 0);

    if (RESULT_PASS != tr)
    {
        WriteToLog(_T("Corruption reading from NULL-mapped tile after writing to a different NULL tile in the same resource"));
    }

    return tr;
}


////////////////////////////////////////////////////////////////////////////////
// CTiledResourcesTest::RenderTextureToSimilar()
////////////////////////////////////////
TEST_RESULT CTiledResourcesTest::RenderTextureToSimilar(_In_ ID3D11Texture2D *pDest, _In_ ID3D11Texture2D *pSrc, const UINT mipLevel, UINT const arraySlice)
{
    return RenderTextureToSimilar < false, ID3D11Texture2D, D3D11_TEXTURE2D_DESC > (pDest, pSrc, mipLevel, arraySlice);
}

TEST_RESULT CTiledResourcesTest::RenderTextureToSimilar(_In_ ID3D11Texture3D *pDest, _In_ ID3D11Texture3D *pSrc, const UINT mipLevel, UINT const arraySlice)
{
    return RenderTextureToSimilar < true, ID3D11Texture3D, D3D11_TEXTURE3D_DESC > (pDest, pSrc, mipLevel, arraySlice);
}

template <bool isTexture3D, class ID3D11_TEXTURE_TYPE, class D3D11_TEXTURE_DESC_TYPE>
TEST_RESULT CTiledResourcesTest::RenderTextureToSimilar(_In_ ID3D11_TEXTURE_TYPE *pDest, _In_ ID3D11_TEXTURE_TYPE *pSrc, const UINT mipLevel, UINT const arraySlice)
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();

    D3D11_TEXTURE_DESC_TYPE texDesc;
    pDest->GetDesc(&texDesc);

    // Create the render target view of the dest
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = texDesc.Format;
    if (isTexture3D)
    {
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
        rtvDesc.Texture3D.MipSlice = mipLevel;
        rtvDesc.Texture3D.FirstWSlice = 0;
        rtvDesc.Texture3D.WSize = -1;
    }
    else
    {
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
        rtvDesc.Texture2DArray.MipSlice = mipLevel;
        rtvDesc.Texture2DArray.ArraySize = 1;
        rtvDesc.Texture2DArray.FirstArraySlice = arraySlice;
    }

    WindowsTest::CHandle<ID3D11RenderTargetView> pRTV;
    hr = pDevice->CreateRenderTargetView(pDest, &rtvDesc, &pRTV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
        return RESULT_FAIL;
    }

    // Create the shader resource view of the source
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = texDesc.Format;
    if (isTexture3D)
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
        srvDesc.Texture3D.MipLevels = 1;
        srvDesc.Texture3D.MostDetailedMip = mipLevel;
    }
    else
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2DArray.ArraySize = 1;
        srvDesc.Texture2DArray.FirstArraySlice = arraySlice;
        srvDesc.Texture2DArray.MipLevels = 1;
        srvDesc.Texture2DArray.MostDetailedMip = mipLevel;
    }

    WindowsTest::CHandle<ID3D11ShaderResourceView> pSRV;
    hr = pDevice->CreateShaderResourceView(pSrc, &srvDesc, &pSRV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateShaderResourceView);
        return RESULT_FAIL;
    }

    UINT mipWidth = texDesc.Width;
    UINT mipHeight = texDesc.Height;
    UINT mipDepth = 1;
    bool getMipDimensionsSuccess = false;
    if (isTexture3D)
    {
        // cast done to appease the compiler, but should never be called for Texture2D!
        mipDepth = ((D3D11_TEXTURE3D_DESC*)(&texDesc))->Depth;
        getMipDimensionsSuccess = CFormatHelper::GetMipDimensions(mipLevel, &mipWidth, &mipHeight, &mipDepth);
    }
    else
    {
        getMipDimensionsSuccess = CFormatHelper::GetMipDimensions(mipLevel, &mipWidth, &mipHeight);
    }
    if (!getMipDimensionsSuccess)
    {
        WriteToLog(_T("GetMipDimensions failed due to non-existent mip level"));
        return RESULT_FAIL;
    }

    // Clear the render target first (in case it is tiled)
    float black[4] = { 0 };
    if (isTexture3D)
    {
        ClearViewTex3D(pRTV, rtvDesc.Format, black, nullptr, 0);
    }
    else
    {
        pContext->ClearView(pRTV, black, nullptr, 0);
    }

    return RenderViewToSimilar(pRTV, pSRV, mipWidth, mipHeight, mipDepth, isTexture3D);
}


TEST_RESULT CTiledResourcesTest::RenderViewToSimilar(_In_ ID3D11RenderTargetView *pDest, _In_ ID3D11ShaderResourceView *pSrc, const UINT width, const UINT height, const UINT depth, bool isTexture3D)
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    ID3D11PixelShader *pPS = nullptr;
    CHandle<ID3D11Buffer> pConstVertexBuffer;

    // Create the viewport
    D3D11_VIEWPORT viewport =
    {
        0.0f,          //    FLOAT TopLeftX;
        0.0f,          //    FLOAT TopLeftY;
        (float)width,  //    FLOAT Width;
        (float)height, //    FLOAT Height;
        0.0f,          //    FLOAT MinDepth;
        1.0f,          //    FLOAT MaxDepth;
    };

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

    // Helper for Geometry Shader creation
    auto CreateGeometryShader = [&](ID3D11GeometryShader **ppShader, const BYTE *pShaderByteCode, const SIZE_T shaderLength)
    {
        if (NULL == *ppShader)
        {
            hr = pDevice->CreateGeometryShader(pShaderByteCode, shaderLength, NULL, ppShader);
            if (FAILED(hr))
            {
                REPORT_HRESULT_FAILURE(hr, CreateGeometryShader);
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

    // Helper to load correct pixel shader based on tiled resource tier
    // NOTE: modifies member variables, including local variable pPS
    auto LoadCorrectPixelShaderBasedOnTier = [&](ID3D11PixelShader **ppShaderTier1, ID3D11PixelShader **ppShaderTier2, const BYTE *pShaderByteCode, const SIZE_T shaderLength, const UINT tier)
    {
        if (1 == tier)
        {
            if (RESULT_PASS != CreatePixelShader(ppShaderTier1, pShaderByteCode, shaderLength))
            {
                return RESULT_FAIL;
            }
            pPS = *ppShaderTier1;
        }
        else
        {
            if (RESULT_PASS != CreatePixelShader(ppShaderTier2, pShaderByteCode, shaderLength))
            {
                return RESULT_FAIL;
            }
            pPS = *ppShaderTier2;
        }
        return RESULT_PASS;
    };


    // Get correct shader byte code based on Texture2D vs Texture3D
    const LOADABLE_SHADER_BYTECODE_TYPE pixelTierToGet = (1 == m_parameters.pTiledResourcesTier->GetValue() ? LSBT_PIXEL_COPY_TEX_TIER_1 : LSBT_PIXEL_COPY_TEX_TIER_2);
    const BYTE *pShaderByteCodeVertex;
    const BYTE *pShaderByteCodeGeometry;
    const BYTE *pShaderByteCodePixel;
    SIZE_T shaderLenVertex;
    SIZE_T shaderLenGeometry;
    SIZE_T shaderLenPixel;
    CTiledResourcesTest::GetShaderByteCode(LSBT_VERTEX_COPY_TEX, &pShaderByteCodeVertex, &shaderLenVertex, false, isTexture3D);
    CTiledResourcesTest::GetShaderByteCode(LSBT_GEOMETRY_COPY_TEX_3D, &pShaderByteCodeGeometry, &shaderLenGeometry, false, isTexture3D);
    CTiledResourcesTest::GetShaderByteCode(pixelTierToGet, &pShaderByteCodePixel, &shaderLenPixel, false, isTexture3D);

    // Create the shaders into correct members, modify pPS
    if (isTexture3D)
    {
        if (RESULT_PASS != CreateVertexShader(&m_pCopyTexVSTex3D, pShaderByteCodeVertex, shaderLenVertex))
        {
            return RESULT_FAIL;
        }

        if (RESULT_PASS != CreateGeometryShader(&m_pCopyTexGSTex3D, pShaderByteCodeGeometry, shaderLenGeometry))
        {
            return RESULT_FAIL;
        }

        if (RESULT_PASS != LoadCorrectPixelShaderBasedOnTier(&m_pCopyTexTier1PSTex3D, &m_pCopyTexTier2PSTex3D, pShaderByteCodePixel, shaderLenPixel, m_parameters.pTiledResourcesTier->GetValue()))
        {
            return RESULT_FAIL;
        }
    }
    else
    {
        if (RESULT_PASS != CreateVertexShader(&m_pCopyTexVS, pShaderByteCodeVertex, shaderLenVertex))
        {
            return RESULT_FAIL;
        }

        if (RESULT_PASS != LoadCorrectPixelShaderBasedOnTier(&m_pCopyTexTier1PS, &m_pCopyTexTier2PS, pShaderByteCodePixel, shaderLenPixel, m_parameters.pTiledResourcesTier->GetValue()))
        {
            return RESULT_FAIL;
        }
    }

    // Create the rasterizer state
    WindowsTest::CHandle<ID3D11RasterizerState1> pRS;
    D3D11_RASTERIZER_DESC1 rsDesc =
    {
        D3D11_FILL_SOLID,		//D3D11_FILL_MODE FillMode;
        D3D11_CULL_NONE,		//D3D11_CULL_MODE CullMode;
        TRUE,					//BOOL FrontCounterClockwise;
        0,						//INT DepthBias;
        0.0,					//FLOAT DepthBiasClamp;
        0.0,					//FLOAT SlopeScaledDepthBias;
        FALSE,					//BOOL DepthClipEnable;
        FALSE,					//BOOL ScissorEnable;
        FALSE,					//BOOL MultisampleEnable;
        FALSE,					//BOOL AntialiasedLineEnable;
        0,						//UINT ForcedSampleCount;
    };

    hr = pDevice->CreateRasterizerState1(&rsDesc, &pRS);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRasterizerState1);
        return RESULT_FAIL;
    }

    // Create the depth stencil state (with disabled depth and stencil)
    WindowsTest::CHandle<ID3D11DepthStencilState> pDS;
    CD3D11_DEPTH_STENCIL_DESC dsDesc(D3D11_DEFAULT);
    dsDesc.DepthEnable = false;
    dsDesc.StencilEnable = false;
    hr = pDevice->CreateDepthStencilState(&dsDesc, &pDS);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRasterizerState1);
        return RESULT_FAIL;
    }

    // Create per-slice constant buffer
    struct cBufferType
    {
        // Position [0,1] of the quad to render relative to 3D volume
        // This is relevant only for Texture3D
        float quadSlicePosition;
        UINT numSlicesSRV;
        UINT numScliesRTV;
    } cBufferData;

    D3D11_BUFFER_DESC cBufferDesc =
    {
        16 * ((sizeof(cBufferData) + 15) / 16), // ByteWidth, align to 16 bytes
        D3D11_USAGE_DEFAULT,                    // Usage
        D3D11_BIND_CONSTANT_BUFFER,             // BindFlags
        0,                                      // CPUAccessFlags
        0,                                      // MiscFlags
        0                                       // StructuredByteStride
    };

    cBufferData.quadSlicePosition = 0.f;
    cBufferData.numSlicesSRV = depth;
    cBufferData.numScliesRTV = depth;
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

    // Set the pipeline state
    pContext->RSSetViewports(1, &viewport);
    pContext->PSSetShaderResources(0, 1, &pSrc);
    pContext->OMSetRenderTargets(1, &pDest, NULL);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    pContext->RSSetState(pRS);
    pContext->OMSetDepthStencilState(pDS, 0);
    pContext->PSSetShader(pPS, NULL, 0);
    pContext->VSSetShader((isTexture3D ? m_pCopyTexVSTex3D : m_pCopyTexVS), NULL, 0);
    pContext->VSSetConstantBuffers(0, 1, &pConstVertexBuffer);
    pContext->GSSetShader((isTexture3D ? m_pCopyTexGSTex3D : nullptr), nullptr, 0);

    // Clear input layout as no input is needed since the 
    // vertex shader knows how to draw a quad using vertex ID's
    pContext->IASetInputLayout(NULL);

    // Draw screen-space quad once per necessary slice
    const UINT numSlices = (isTexture3D ? depth : 1);
    const float deltaSlice = (isTexture3D ? 1.f/depth : 1.0f);

    for (UINT slice = 0; slice < numSlices; ++slice)
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

    return tr;
}


////////////////////////////////////////////////////////////////////////////////
//
// CTileMapping
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// CTileMapping::CTileMapping()
////////////////////////////////////////
CTileMapping::CTileMapping()
{
    Row[0].Column[0] = 
    Row[0].Column[1] = 
    Row[0].Column[2] = 
    Row[0].Column[3] = 
    Row[1].Column[0] = 
    Row[1].Column[1] = 
    Row[1].Column[2] = 
    Row[1].Column[3] = 
    Row[2].Column[0] = 
    Row[2].Column[1] = 
    Row[2].Column[2] = 
    Row[2].Column[3] = 
    Row[3].Column[0] = 
    Row[3].Column[1] = 
    Row[3].Column[2] = 
    Row[3].Column[3] = 0xffffffffUL;
    Width = Height = 4;
}

////////////////////////////////////////////////////////////////////////////////
// CTileMapping::CTileMapping()
////////////////////////////////////////
CTileMapping::CTileMapping(const CTileMapping &copy)
{
    Set(copy);
}

////////////////////////////////////////////////////////////////////////////////
// CTileMapping::CTileMapping()
////////////////////////////////////////
CTileMapping::CTileMapping(UINT width, UINT height, const UINT *mappingData)
{
    Clear();

    Width = width;
    Height = height;

    if(nullptr != mappingData)
    {
        for(UINT j = 0; j < height; ++j)
        {
            for(UINT i = 0; i < width; ++i)
            {
                Row[j].Column[i] = *mappingData;
                mappingData++;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CTileMapping::Set()
////////////////////////////////////////
void CTileMapping::Set(const CTileMapping &copy) {
    Row[0].Column[0] = copy.Row[0].Column[0];
    Row[0].Column[1] = copy.Row[0].Column[1];
    Row[0].Column[2] = copy.Row[0].Column[2];
    Row[0].Column[3] = copy.Row[0].Column[3];
    Row[1].Column[0] = copy.Row[1].Column[0];
    Row[1].Column[1] = copy.Row[1].Column[1];
    Row[1].Column[2] = copy.Row[1].Column[2];
    Row[1].Column[3] = copy.Row[1].Column[3];
    Row[2].Column[0] = copy.Row[2].Column[0];
    Row[2].Column[1] = copy.Row[2].Column[1];
    Row[2].Column[2] = copy.Row[2].Column[2];
    Row[2].Column[3] = copy.Row[2].Column[3];
    Row[3].Column[0] = copy.Row[3].Column[0];
    Row[3].Column[1] = copy.Row[3].Column[1];
    Row[3].Column[2] = copy.Row[3].Column[2];
    Row[3].Column[3] = copy.Row[3].Column[3];
    Width = copy.Width;
    Height = copy.Height;
}

////////////////////////////////////////////////////////////////////////////////
// CTileMapping::SetFromStrings()
////////////////////////////////////////
void CTileMapping::SetFromStrings(const char *rows[4])
{
    Width = 4;
    Height = 4;

    for(UINT j = 0; j < Height; ++j)
    {
        for(UINT i = 0; i < Width; ++i)
        {
            UINT value = 0xffffffffUL;

            if(rows[j])
            {
                char ch = rows[j][i];
                if(ch == 0)
                    Width = i;
                else
                if( ch >= '0' && ch <= '9')
                    value = ch - '0';
                else
                if(ch >='A' && ch <='F')
                    value = 0xa + ch - 'A';
                else
                if(ch >='a' && ch <='f')
                    value = 0xa + ch - 'f';
                else
                if(ch == 's' || ch == 'S')
                    value = 0xfffffffeUL; // Indicates skip
            }
            else
            {
                Height = j;
            }

            Row[j].Column[i] = value;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CTileMapping::GetStrings()
////////////////////////////////////////
void CTileMapping::GetStrings(char *rows[4]) const
{
    for(UINT j = 0; j < 4 && j < Height; ++j)
    {
        UINT i;
        for(i = 0; i < 4 && i < Width; ++i)
        {
            UINT value = Row[j].Column[i];

            if(value <= 9)
                rows[j][i] = '0' + value;
            else
            if(value >= 0xa && value <= 0xf)
                rows[j][i] = 'A' + value - 0x0a;
            else
                rows[j][i] = 'x';
        }
        rows[j][i] = 0; // NULL terminator
    }
}


void CTileMapping::Transform(CTileMapping &newmap, const int xform[4][4])
{
    for(UINT j = 0; j < 4; ++j)
    {
        for(UINT i = 0; i < 4; ++i)
        {
            // Only transform when index is not 0xffffffff
            if(!UnmappedOrSkippedTile(xform[j][i]))
            {
                UINT x = xform[j][i] % 4;
                UINT y = xform[j][i] / 4;

                newmap.Row[j].Column[i] = Row[y].Column[x];
            }
        }
    }
}

void CTileMapping::Overlay(const CTileMapping &overlay)
{
    assert(overlay.Width == Width);
    assert(overlay.Height == Height);

    for(UINT x = 0; x < Width; ++x)
    {
        for(UINT y = 0; y < Height; ++y)
        {
            if(SkippedTileId != overlay.Row[y].Column[x])
            {
                Row[y].Column[x] = overlay.Row[y].Column[x];
            }
        }
    }
}

void CTileMapping::Clear()
{
    Row[0].Column[0] = 
    Row[0].Column[1] = 
    Row[0].Column[2] = 
    Row[0].Column[3] = 
    Row[1].Column[0] = 
    Row[1].Column[1] = 
    Row[1].Column[2] = 
    Row[1].Column[3] = 
    Row[2].Column[0] = 
    Row[2].Column[1] = 
    Row[2].Column[2] = 
    Row[2].Column[3] = 
    Row[3].Column[0] = 
    Row[3].Column[1] = 
    Row[3].Column[2] = 
    Row[3].Column[3] = 0xffffffffUL;
}

////////////////////////////////////////////////////////////////////////////////
// CTileMapping::operator==()
////////////////////////////////////////
bool CTileMapping::operator==(const CTileMapping &tileMapping) const
{
    if(Width != tileMapping.Width) return false;
    if(Height != tileMapping.Height) return false;

    for(UINT j = 0; j < Height; ++j)
        for(UINT i = 0; i < Width; ++i)
            if(Row[j].Column[i] != tileMapping.Row[j].Column[i]) return false;

    return true;
}


////////////////////////////////////////////////////////////////////////////////
//
// CVolumeTileMapping
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// CVolumeTileMapping::CVolumeTileMapping()
////////////////////////////////////////
CVolumeTileMapping::CVolumeTileMapping()
{
    for (UINT k = 0; k < 4; ++k)
        for (UINT j = 0; j < 4; ++j)
            for (UINT i = 0; i < 4; ++i)
                Slice[k].Row[j].Column[i] = 0xffffffffUL;
    Width = Height = Depth = 4;
}

////////////////////////////////////////////////////////////////////////////////
// CVolumeTileMapping::CVolumeTileMapping()
////////////////////////////////////////
CVolumeTileMapping::CVolumeTileMapping(const CVolumeTileMapping &copy)
{
    Set(copy);
}

////////////////////////////////////////////////////////////////////////////////
// CVolumeTileMapping::CVolumeTileMapping()
////////////////////////////////////////
CVolumeTileMapping::CVolumeTileMapping(UINT width, UINT height, UINT depth, const UINT *mappingData)
{
    Clear();

    Width = width;
    Height = height;
    Depth = depth;

    if (nullptr != mappingData)
    {
        for (UINT k = 0; k < depth; ++k)
        {
            for (UINT j = 0; j < height; ++j)
            {
                for (UINT i = 0; i < width; ++i)
                {
                    Slice[k].Row[j].Column[i] = *mappingData;
                    mappingData++;
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CVolumeTileMapping::Set()
////////////////////////////////////////
void CVolumeTileMapping::Set(const CVolumeTileMapping &copy)
{
    for (UINT k = 0; k < 4; ++k)
        for (UINT j = 0; j < 4; ++j)
            for (UINT i = 0; i < 4; ++i)
                Slice[k].Row[j].Column[i] = copy.Slice[k].Row[j].Column[i];
    Width = copy.Width;
    Height = copy.Height;
    Depth = copy.Depth;
}

////////////////////////////////////////////////////////////////////////////////
// CVolumeTileMapping::SetFromStrings()
////////////////////////////////////////
void CVolumeTileMapping::SetFromStrings(const char *slices[4][4])
{
    Width = 4;
    Height = 4;
    Depth = 4;

    for (UINT k = 0; k < Depth; ++k)
    {
        for (UINT j = 0; j < Height && k < Depth; ++j)
        {
            for (UINT i = 0; i < Width && j < Height && k < Depth; ++i)
            {
                UINT value = 0xffffffffUL;

                if (slices[k][0])
                {
                    if (slices[k][j])
                    {
                        char ch = slices[k][j][i];
                        if (ch == 0)
                        {
                            Width = i;
                            break;
                        }
                        else
                        {
                            value = CVolumeTileMapping::DecodeChar(ch);
                        }
                    }
                    else
                    {
                        Height = j;
                        break;
                    }
                }
                else
                {
                    Depth = k;
                    break;
                }

                Slice[k].Row[j].Column[i] = value;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CVolumeTileMapping::GetStrings()
////////////////////////////////////////
void CVolumeTileMapping::GetStrings(char *slices[4][4]) const
{
    for (UINT k = 0; k < 4 && k < Depth; ++k)
    {
        for (UINT j = 0; j < 4 && j < Height; ++j)
        {
            UINT i;
            for (i = 0; i < 4 && i < Width; ++i)
            {
                UINT value = Slice[k].Row[j].Column[i];
                slices[k][j][i] = CVolumeTileMapping::EncodeChar(value);
            }
            slices[k][j][i] = 0; // NULL terminator
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CVolumeTileMapping::Transform()
////////////////////////////////////////
void CVolumeTileMapping::Transform(CVolumeTileMapping &newmap, const int xform[4][4][4])
{
    for (UINT k = 0; k < 4; ++k)
    {
        for (UINT j = 0; j < 4; ++j)
        {
            for (UINT i = 0; i < 4; ++i)
            {
                // Only transform when index is not 0xffffffff
                if (!UnmappedOrSkippedTile(xform[k][j][i]))
                {
                    UINT withinSlice = xform[k][j][i] % 16;
                    UINT z = xform[k][j][i] / 16;
                    UINT y = withinSlice / 4;
                    UINT x = withinSlice % 4;

                    newmap.Slice[k].Row[j].Column[i] = Slice[z].Row[y].Column[x];
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CVolumeTileMapping::Overlay()
////////////////////////////////////////
void CVolumeTileMapping::Overlay(const CVolumeTileMapping &overlay)
{
    assert(overlay.Width == Width);
    assert(overlay.Height == Height);
    assert(overlay.Depth == Depth);

    for (UINT x = 0; x < Width; ++x)
    {
        for (UINT y = 0; y < Height; ++y)
        {
            for (UINT z = 0; z < Depth; ++z)
            {
                if (SkippedTileId != overlay.Slice[z].Row[y].Column[x])
                {
                    Slice[z].Row[y].Column[x] = overlay.Slice[z].Row[y].Column[x];
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CVolumeTileMapping::Clear()
////////////////////////////////////////
void CVolumeTileMapping::Clear()
{
    for (UINT k = 0; k < 4; ++k)
        for (UINT j = 0; j < 4; ++j)
            for (UINT i = 0; i < 4; ++i)
                Slice[k].Row[j].Column[i] = 0xffffffffUL;
}

////////////////////////////////////////////////////////////////////////////////
// CVolumeTileMapping::operator==()
////////////////////////////////////////
bool CVolumeTileMapping::operator==(const CVolumeTileMapping &tileMapping) const
{
    if (Width != tileMapping.Width) return false;
    if (Height != tileMapping.Height) return false;
    if (Depth != tileMapping.Depth) return false;

    for (UINT k = 0; k < Depth; ++k)
        for (UINT j = 0; j < Height; ++j)
            for (UINT i = 0; i < Width; ++i)
                if (Slice[k].Row[j].Column[i] != tileMapping.Slice[k].Row[j].Column[i]) return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// CVolumeTileMapping::EncodeChar()
////////////////////////////////////////
char CVolumeTileMapping::EncodeChar(const UINT toEncode)
{
    // same mapping as in CVolumeTileMapping::DecodeChar()
    if (toEncode <= 25)
        return 'A' + toEncode;
    if (toEncode <= 51)
        return 'a' + toEncode - 26;
    if (toEncode <= 61)
        return '0' + toEncode - 52;
    if (toEncode == 62)
        return '+';
    if (toEncode == 63)
        return '/';
    if (toEncode == 0xfffffffeUL)
        return '-'; // Indicates skip
    return '?';
}

////////////////////////////////////////////////////////////////////////////////
// CVolumeTileMapping::DecodeChar()
////////////////////////////////////////
UINT CVolumeTileMapping::DecodeChar(const char toDecode)
{
    // We use basic Base64 index encoding here
    // A -> 0
    // Z -> 25
    // a -> 26
    // z -> 51
    // 0 -> 52
    // 9 -> 61
    // + -> 62
    // / -> 63
    // - -> skip
    // ? -> null mapped tile?

    if (toDecode >= 'A' && toDecode <= 'Z')
        return toDecode - 'A';
    if (toDecode >= 'a' && toDecode <= 'z')
        return 26 + toDecode - 'a';
    if (toDecode >= '0' && toDecode <= '9')
        return 52 + toDecode - '0';
    if (toDecode == '+')
        return 62;
    if (toDecode == '/')
        return 63;
    if (toDecode == '-')
        return 0xfffffffeUL; // Indicates skip
    return 0xffffffffUL;
}
