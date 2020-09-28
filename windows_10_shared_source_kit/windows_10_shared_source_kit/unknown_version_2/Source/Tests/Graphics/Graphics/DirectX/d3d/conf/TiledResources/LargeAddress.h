////////////////////////////////////////////////////////////////
// LargeAddress.h
////////////////////////////////////////////////////////////////

#pragma once

#include "D3D11Test.h"
#include "TiledResources.h"

enum ADDRESS_SPACE_USAGE
{
    ADDRESS_SPACE_USAGE_64M,
    ADDRESS_SPACE_USAGE_128M,
    ADDRESS_SPACE_USAGE_1G,
    ADDRESS_SPACE_USAGE_2G,
    ADDRESS_SPACE_USAGE_4G,
    ADDRESS_SPACE_USAGE_64G,
    ADDRESS_SPACE_USAGE_128G,
    ADDRESS_SPACE_USAGE_256G,
    ADDRESS_SPACE_USAGE_512G,
    ADDRESS_SPACE_USAGE_920G,
};

DECLARE_NAMED_VALUES(ADDRESS_SPACE_USAGE);

template <TILED_RESOURCE_BASE resourceBase>
class CLargeAddressTestBase : public CResourceBaseTestHelper<resourceBase>
{
protected:
    // Helpers to clean up typenames
    CRESOURCE_BASE_TEST_HELPER__TYPENAME_TYPEDEFS

        // Parameters
    struct
    {
        CVariationParameter<ADDRESS_SPACE_USAGE> *pAddressSpaceUsage;
        CVariationParameter<UINT> *pTextureCount;
    } m_parameters;

    void GetResourceDimensions(UINT *width, UINT *height, UINT *arraySizeOrDepth, UINT8 *addressSpaceBits, UINT textureCount, ADDRESS_SPACE_USAGE addressSpaceUsage) const;

    static void CalcColor(const UINT x, const UINT y, float *color)
    {
        float numerator = (float)(x * y) / 64;
        color[0] = numerator / (1 + x + y);
        color[1] = numerator / (1 + 64 - x + y);
        color[2] = numerator / (1 + x + 64 - y);
        color[3] = numerator / (1 + 64 - x + 64 - y);
    }

    static void CalcColor(const UINT x, const UINT y, const UINT z, float *color)
    {
        float numerator = (float)(x * y * z) / 16;
        color[0] = numerator / (1 + x + y + z);
        color[1] = numerator / (1 + 16 - x + y + z);
        color[2] = numerator / (1 + x + 16 - y + z);
        color[3] = numerator / (1 + x + y + 16 - z);
    }

public:
    void InitTestParameters();
    TEST_RESULT ExecuteTestCase();
};



// Actual test classes
class CLargeAddressTestTex2D : public CLargeAddressTestBase < TRB_TEXTURE_2D > { };
class CLargeAddressTestTex3D : public CLargeAddressTestBase < TRB_TEXTURE_3D > { };




// Implementation
template <TILED_RESOURCE_BASE resourceBase>
void CLargeAddressTestBase<resourceBase>::InitTestParameters()
{
    // When using Texture2D, skip building factors for Tier 3 (Tier 2 tests correct functionality)
    // When using Texture3D, skip building factors for Tiers 1 and 2
    testfactor::RFactor tiledResourcesTier = InitTiledResourcesTierParam((TRB_TEXTURE_3D == resourceBase));

    CreateParameter(_T("AddressSpaceUsage"), &m_parameters.pAddressSpaceUsage);
    CreateParameter(_T("TextureCount"), &m_parameters.pTextureCount);

    CUserValueSet<ADDRESS_SPACE_USAGE> *pAddressSpaceUsageValues = new CUserValueSet<ADDRESS_SPACE_USAGE>();
    pAddressSpaceUsageValues->AddValue(ADDRESS_SPACE_USAGE_64M);
    pAddressSpaceUsageValues->AddValue(ADDRESS_SPACE_USAGE_128M);
    pAddressSpaceUsageValues->AddValue(ADDRESS_SPACE_USAGE_64G);
    pAddressSpaceUsageValues->AddValue(ADDRESS_SPACE_USAGE_128G);
    pAddressSpaceUsageValues->AddValue(ADDRESS_SPACE_USAGE_256G);
    pAddressSpaceUsageValues->AddValue(ADDRESS_SPACE_USAGE_512G);
    pAddressSpaceUsageValues->AddValue(ADDRESS_SPACE_USAGE_920G);

    CUserValueSet<UINT> *pTextureCountValues = new CUserValueSet<UINT>();
    pTextureCountValues->AddValue(1U);
    pTextureCountValues->AddValue(2U);
    pTextureCountValues->AddValue(4U);
    pTextureCountValues->AddValue(8U);

    testfactor::RFactor addressSpaceUsage = AddParameterValueSet(m_parameters.pAddressSpaceUsage, pAddressSpaceUsageValues);
    testfactor::RFactor textureCount = AddParameterValueSet(m_parameters.pTextureCount, pTextureCountValues);

    SetRootTestFactor(tiledResourcesTier * addressSpaceUsage * textureCount);
}

template <TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CLargeAddressTestBase<resourceBase>::ExecuteTestCase()
{
    HRESULT hr;
    TEST_RESULT tr = RESULT_PASS;
    CHandle<ID3D11TextureType> pLargeTextures[8];
    CHandle<ID3D11TextureType> pStagingSource;
    CHandle<ID3D11TextureType> pStagingDest;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    CTiledResourceHelper::CTilePoolManager *pTilePoolManager = GetFramework()->GetTilePoolManager();

    GetResourceViewer().HideAll();

    //
    // Flush to make sure textures from the previous test case are destroyed by the driver.
    //
    pContext->ClearState();
    pContext->Flush();

    const UINT tileWidth = (TRB_TEXTURE_2D == resourceBase ? 64 : 16);
    const UINT tileHeight = (TRB_TEXTURE_2D == resourceBase ? 64 : 16);
    const UINT tileDepth = (TRB_TEXTURE_2D == resourceBase ? 1 : 16);

    assert(m_parameters.pTextureCount->GetValue() <= _countof(pLargeTextures));

    // Skip the following for Texture3D, since we can't create a single texture this large
    if (TRB_TEXTURE_3D == resourceBase)
    {
        switch (m_parameters.pAddressSpaceUsage->GetValue())
        {
        case ADDRESS_SPACE_USAGE_256G:
        case ADDRESS_SPACE_USAGE_512G:
        case ADDRESS_SPACE_USAGE_920G:
            return RESULT_SKIP;
        }
    }

    UINT8 addressSpaceBits = 128;
    UINT widthInPixels = 0;
    UINT heightInPixels = 0;
    UINT arraySizeOrDepthInPixels = 0;  // arraySize (Texture2D) or depthInPixels (Texture3D)
    GetResourceDimensions(
        &widthInPixels,
        &heightInPixels,
        &arraySizeOrDepthInPixels,
        &addressSpaceBits,
        m_parameters.pTextureCount->GetValue(),
        m_parameters.pAddressSpaceUsage->GetValue()
        );

    D3D11_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT GPUVACaps;
    pDevice->CheckFeatureSupport(D3D11_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, &GPUVACaps, sizeof(GPUVACaps));
    if (GPUVACaps.MaxGPUVirtualAddressBitsPerResource <= addressSpaceBits)
    {
        // An exact tolerance isn't acceptable, since we can't guarentee the whole contiguous range can be exercised.
        // Tests commonly enforce only 1/2 of these type of maximums.
        WriteToLog(
            _T("Not enough VA space for resource and other objects as D3D11_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT::MaxGPUVirtualAddressBitsPerResource is %u."),
            GPUVACaps.MaxGPUVirtualAddressBitsPerResource
            );
        return RESULT_SKIP;
    }

    UINT widthInTiles = widthInPixels / tileWidth;
    UINT heightInTiles = heightInPixels / tileHeight;
    UINT depthInTiles = arraySizeOrDepthInPixels / tileDepth;   // arraySize (Texture2D) or depthInPixels / tileDepth (Texture3D)

    // Create the very large tiled texture
    CD3D11_TEXTURE_TYPE_DESC texDesc;
    CResourceBaseTestHelper<resourceBase>::GenerateTextureDesc(
        texDesc,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        widthInPixels,
        heightInPixels,
        arraySizeOrDepthInPixels,
        1,
        D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET
        );
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_TILED;


    // Create tileWidth x tileHeight staging textures containing DXGI_FORMAT_R32G32B32A32_FLOAT data
    CD3D11_TEXTURE_TYPE_DESC desc;
    CResourceBaseTestHelper<resourceBase>::GenerateTextureDesc(
        desc,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        tileWidth,
        tileHeight,
        tileDepth,  // for Texture2D, tileDepth == 1 
        1,
        0,
        D3D11_USAGE_STAGING,
        D3D11_CPU_ACCESS_WRITE
        );


    // Source
    tr = CTiledResourcesTest::CreateTexture(&desc, &pStagingSource);
    if (RESULT_PASS != tr)
    {
        return RESULT_FAIL;
    }

    // Dest
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    if (TRB_TEXTURE_2D == resourceBase)
    {
        // Just to please the compiler, this should never need to compile for Texture3D
        ((CD3D11_TEXTURE2D_DESC*)(&desc))->ArraySize = arraySizeOrDepthInPixels;
    }

    tr = CTiledResourcesTest::CreateTexture(&desc, &pStagingDest);
    if (RESULT_PASS != tr)
    {
        return RESULT_FAIL;
    }

    typedef float BIG_PIXEL[4];

    union
    {
        BYTE *pRowBits;
        BIG_PIXEL *pRowPixels;
    };
    BYTE *pSliceBits;

    MapHelper<ID3D11TextureType> sourceMap(pContext, pStagingSource, 0, D3D11_MAP_WRITE);
    hr = sourceMap.Map();
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, Map);
        return RESULT_FAIL;
    }

    pSliceBits = (BYTE *)sourceMap.pData;
    for (UINT z = 0; z < tileDepth; ++z)        // for Texture2D, tileDepth == 1
    {
        pRowBits = pSliceBits;
        for (UINT y = 0; y < tileHeight; ++y)
        {
            for (UINT x = 0; x < tileWidth; ++x)
            {
                if (TRB_TEXTURE_2D == resourceBase)
                {
                    CalcColor(x, y, pRowPixels[x]);
                }
                else
                {
                    CalcColor(x, y, z, pRowPixels[x]);
                }
            }

            pRowBits += sourceMap.RowPitch;
        }

        pSliceBits += sourceMap.DepthPitch;
    }

    sourceMap.Unmap();

    UINT nameIndex = 0;
    wchar_t combinedName[tileDepth][64];
    for (UINT slice = 0; slice < tileDepth; ++slice)
    {
        wchar_t sliceName[32];
        swprintf_s(sliceName, ARRAYSIZE(sliceName), L" (Slice %d)", slice);
        swprintf_s(combinedName[nameIndex], ARRAYSIZE(combinedName[nameIndex]), L"Tile Data Written%s", (tileDepth == 1 ? L"" : sliceName));
        ShowResource(combinedName[nameIndex++], pStagingSource, 0, 0, slice, 4.0f);
    }

    // For each texture...
    for (UINT index = 0; index < m_parameters.pTextureCount->GetValue(); ++index)
    {
        CHandle<ID3D11TextureType> pLargeTexture;

        tr = CTiledResourcesTest::CreateTexture(&texDesc, &pLargeTexture);
        if (RESULT_PASS != tr)
        {
            WriteToLog(_T("Failed to create resource number %u with dimensions %ux%ux%u"), index, widthInPixels, heightInPixels, arraySizeOrDepthInPixels);
            return RESULT_FAIL;
        }

        // Map a single tile to the largest x,y,z position in the resource
        D3D11_TILED_RESOURCE_COORDINATE coord;
        D3D11_TILE_REGION_SIZE size;
        size.bUseBox = false;
        size.Depth = 1;
        size.Width = 1;
        size.Height = 1;
        size.NumTiles = 1;

        UINT rangeFlag = 0;

        // Fetch 2 tiles to share across alternating array slice
        UINT offset = pTilePoolManager->FetchRange(pLargeTexture, 2);
        UINT count = 1;

        // For each array slice...
        // Texture2D: 'slice' actually means array slice (arraySizeOrDepthInPixels)
        // Texture3D: 'slice' actually means iteration #, which at most is 2, thus reusing logic
        //            to map 2 tiles in total - one at top-left and other at bottom-right of 
        //            the texture at mip 0
        const UINT maxNumIterations = (TRB_TEXTURE_2D == resourceBase ? arraySizeOrDepthInPixels : 1);
        for (UINT slice = 0; slice < maxNumIterations; ++slice)
        {
            UINT odd = slice & 0x00000001;
            UINT bottomRight = (slice & 0x00000002) >> 1;
            UINT tileIndex = offset + odd;
            coord.Subresource = (TRB_TEXTURE_2D == resourceBase ? slice : 0);   // for Texture3D, we use mip level 0

            // Alternate between top-left and bottom-right
            if (0 == bottomRight)
            {
                coord.X = widthInTiles - 1;
                coord.Y = heightInTiles - 1;
                coord.Z = (TRB_TEXTURE_2D == resourceBase ? 0 : depthInTiles - 1);
            }
            else
            {
                coord.X = 0;
                coord.Y = 0;
                coord.Z = 0;
            }

            // ...map a shared physical tile to the last logical tile
            hr = pContext->UpdateTileMappings(
                pLargeTexture,
                1,
                &coord,
                &size,
                pTilePoolManager->GetTilePool(),
                1,
                &rangeFlag,
                &tileIndex,
                &count,
                0
                );
            if (FAILED(hr))
            {
                REPORT_HRESULT_FAILURE(hr, UpdateTileMappings);
                return RESULT_FAIL;
            }

            // ...and initialize only the tiles mapped to the first two array slices
            if (slice < 2)
            {
                // Copy the staging data to the mapped tile
                pContext->TiledResourceBarrier(nullptr, pLargeTexture);

                pContext->CopySubresourceRegion(
                    pLargeTexture,
                    (TRB_TEXTURE_2D == resourceBase ? slice : 0),                                           // Texture2D: Update should affect all logical tiles sharing the same physical tile
                    widthInPixels - tileWidth,
                    heightInPixels - tileHeight,
                    (TRB_TEXTURE_2D == resourceBase ? 0 : arraySizeOrDepthInPixels - tileDepth),            // Texture3D: arraySizeOrDepthInPixels == depthInPixels
                    pStagingSource,
                    0,
                    nullptr
                    );
                pContext->Flush();
            }
        }

        pLargeTextures[index] = pLargeTexture;

        // For each array slice...
        // Recall definition of 'slice' from previous loop above
        for (UINT slice = 0; slice < maxNumIterations; ++slice)
        {
            UINT bottomRight = (slice & 0x00000002) >> 1;

            // ...copy the mapped tile to another staging texture...
            D3D11_BOX box;
            // Alternate between top-left and bottom-right
            if (0 == bottomRight)
            {
                box.top = heightInPixels - tileHeight;
                box.left = widthInPixels - tileWidth;
                box.front = (TRB_TEXTURE_2D == resourceBase ? 0 : arraySizeOrDepthInPixels - tileDepth),    // Texture3D: arraySizeOrDepthInPixels == depthInPixels
                    box.bottom = heightInPixels;
                box.right = widthInPixels;
                box.back = (TRB_TEXTURE_2D == resourceBase ? 1 : arraySizeOrDepthInPixels);                 // Texture3D: arraySizeOrDepthInPixels == depthInPixels
            }
            else
            {
                box.top = 0;
                box.left = 0;
                box.front = 0;
                box.bottom = tileHeight;
                box.right = tileWidth;
                box.back = (TRB_TEXTURE_2D == resourceBase ? 1 : tileDepth);
            }

            pContext->CopySubresourceRegion(
                pStagingDest,
                0,
                0,
                0,
                0,
                pLargeTexture,
                slice,
                &box
                );
            pContext->Flush();

            {
                UINT nameIndex = 0;
                wchar_t name[tileDepth][64];
                for (UINT dSlice = 0; dSlice < tileDepth; ++dSlice)
                {
                    wchar_t sliceName[32];
                    swprintf_s(sliceName, ARRAYSIZE(sliceName), L" (Slice %d)", dSlice);
                    swprintf_s(name[nameIndex], ARRAYSIZE(name[nameIndex]), L"Tile Data Read texture %d%s", index, (tileDepth == 1 ? L"" : sliceName));
                    ShowResource(name[nameIndex++], pStagingDest, 0, 0, dSlice, 4.0f);
                }
            }

            // ...then validate the data
            MapHelper<ID3D11TextureType> destMap(pContext, pStagingDest, 0, D3D11_MAP_READ);
            hr = destMap.Map();
            if (FAILED(hr))
            {
                REPORT_HRESULT_FAILURE(hr, Map);
                return RESULT_FAIL;
            }

            pSliceBits = (BYTE *)destMap.pData;
            for (UINT z = 0; z < tileDepth; ++z)
            {
                pRowBits = pSliceBits;
                for (UINT y = 0; y < tileHeight; ++y)
                {
                    for (UINT x = 0; x < tileWidth; ++x)
                    {
                        float expected[4];
                        if (TRB_TEXTURE_2D == resourceBase)
                        {
                            CalcColor(x, y, expected);
                        }
                        else
                        {
                            CalcColor(x, y, z, expected);
                        }
                        if (pRowPixels[x][0] != expected[0]) tr = RESULT_FAIL;
                        if (pRowPixels[x][1] != expected[1]) tr = RESULT_FAIL;
                        if (pRowPixels[x][2] != expected[2]) tr = RESULT_FAIL;
                        if (pRowPixels[x][3] != expected[3]) tr = RESULT_FAIL;

                        if (RESULT_PASS != tr)
                        {
                            WriteToLog(_T("Data read from last tile in tiled resource %u, %s %u doesn't match data written"), index, (TRB_TEXTURE_2D == resourceBase ? L"array slice" : L"corner index"), slice);
                            return tr;
                        }
                    }

                    pRowBits += destMap.RowPitch;
                }

                pSliceBits += destMap.DepthPitch;
            }
        }
    }

    return tr;
}

template <TILED_RESOURCE_BASE resourceBase>
void CLargeAddressTestBase<resourceBase>::GetResourceDimensions(UINT *width, UINT *height, UINT *arraySizeOrDepth, UINT8 *addressSpaceBits, UINT textureCount, ADDRESS_SPACE_USAGE addressSpaceUsage) const
{
    if (TRB_TEXTURE_2D == resourceBase)
    {
        switch (addressSpaceUsage)
        {
        case ADDRESS_SPACE_USAGE_64M:
            *width = 512;
            *height = 512;
            *arraySizeOrDepth = 16 / textureCount;
            *addressSpaceBits = 26;
            break;
        case ADDRESS_SPACE_USAGE_128M:
            *width = 512;
            *height = 512;
            *arraySizeOrDepth = 32 / textureCount;
            *addressSpaceBits = 27;
            break;
        case ADDRESS_SPACE_USAGE_1G:
            *width = 2048;
            *height = 2048;
            *arraySizeOrDepth = 16 / textureCount;
            *addressSpaceBits = 30;
            break;
        case ADDRESS_SPACE_USAGE_2G:
            *width = 2048;
            *height = 2048;
            *arraySizeOrDepth = 32 / textureCount;
            *addressSpaceBits = 31;
            break;
        case ADDRESS_SPACE_USAGE_4G:
            *width = 2048;
            *height = 2048;
            *arraySizeOrDepth = 64 / textureCount;
            *addressSpaceBits = 32;
            break;
        case ADDRESS_SPACE_USAGE_64G:
            *width = 16384;
            *height = 16384;
            *arraySizeOrDepth = 16 / textureCount;
            *addressSpaceBits = 36;
            break;
        case ADDRESS_SPACE_USAGE_128G:
            *width = 16384;
            *height = 16384;
            *arraySizeOrDepth = 32 / textureCount;
            *addressSpaceBits = 37;
            break;
        case ADDRESS_SPACE_USAGE_256G:
            *width = 16384;
            *height = 16384;
            *arraySizeOrDepth = 64 / textureCount;
            *addressSpaceBits = 38;
            break;
        case ADDRESS_SPACE_USAGE_512G:
            *width = 16384;
            *height = 16384;
            *arraySizeOrDepth = 128 / textureCount;
            *addressSpaceBits = 39;
            break;
        case ADDRESS_SPACE_USAGE_920G:
            *width = 16384;
            *height = 16384;
            *arraySizeOrDepth = 230 / textureCount;
            *addressSpaceBits = 40;
            break;
        default:
            throw std::exception("Invalid ADDRESS_SPACE_USAGE value");
            break;
        }
    }
    else
    {
        assert(TRB_TEXTURE_3D == resourceBase);
        const UINT BytesPerElement = 16; // R32G32B32A32
        switch (addressSpaceUsage)
        {
        case ADDRESS_SPACE_USAGE_64M:
            *width  = 128;
            *height = 128;
            *arraySizeOrDepth = 256 / textureCount;
            assert(*width * *height * *arraySizeOrDepth * textureCount * BytesPerElement == 67108864);
            *addressSpaceBits = 26;
            break;
        case ADDRESS_SPACE_USAGE_128M:
            *width  = 128;
            *height = 128;
            *arraySizeOrDepth = 512 / textureCount;
            assert(*width * *height * *arraySizeOrDepth * textureCount * BytesPerElement == 134217728);
            *addressSpaceBits = 27;
            break;
        case ADDRESS_SPACE_USAGE_1G:
            *width  = 512;
            *height = 512;
            *arraySizeOrDepth = 256 / textureCount;
            assert(*width * *height * *arraySizeOrDepth * textureCount * BytesPerElement == 1073741824);
            *addressSpaceBits = 30;
            break;
        case ADDRESS_SPACE_USAGE_2G:
            *width  = 512;
            *height = 512;
            *arraySizeOrDepth = 512 / textureCount;
            assert(*width * *height * *arraySizeOrDepth * textureCount * BytesPerElement == 2147483648);
            *addressSpaceBits = 31;
            break;
        case ADDRESS_SPACE_USAGE_4G:
            *width  = 1024;
            *height = 1024;
            *arraySizeOrDepth = 256 / textureCount;
            assert(*width * *height * *arraySizeOrDepth * textureCount * BytesPerElement == 4294967296);
            *addressSpaceBits = 32;
            break;
        case ADDRESS_SPACE_USAGE_64G:
            *width  = 2048;
            *height = 2048;
            *arraySizeOrDepth = 1024 / textureCount;
            assert(UINT64(*width) * *height * *arraySizeOrDepth * textureCount * BytesPerElement == 68719476736);
            *addressSpaceBits = 36;
            break;
        case ADDRESS_SPACE_USAGE_128G:
            *width  = 2048;
            *height = 2048;
            *arraySizeOrDepth = 2048 / textureCount;
            assert(UINT64(*width) * *height * *arraySizeOrDepth * textureCount * BytesPerElement == 137438953472);
            *addressSpaceBits = 37;
            break;
            // The most single tile allocation we can do with Texture3D is
            // 2048 * 2048 * 2048 * 16 B = 128 GB
        case ADDRESS_SPACE_USAGE_256G:
        case ADDRESS_SPACE_USAGE_512G:
        case ADDRESS_SPACE_USAGE_920G:
        default:
            throw std::exception("Invalid ADDRESS_SPACE_USAGE value");
            break;
        }
        assert((*arraySizeOrDepth > 0) && ((*arraySizeOrDepth % 16) == 0)); // Depth must be on tile boundaries for mem usage to be correct
    }
}
